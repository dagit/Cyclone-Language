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
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 173
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 246
extern struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*,int);
# 265
extern void*Cyc_List_find_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 166
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 170
enum Cyc_Absyn_AliasQualVal{Cyc_Absyn_Aliasable_qual =0U,Cyc_Absyn_Unique_qual =1U,Cyc_Absyn_Refcnt_qual =2U,Cyc_Absyn_Restricted_qual =3U};
# 186 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 331
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 338
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 450 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 529
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 536
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 554
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 568
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;int f3;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};
# 736 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 923
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 926
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
int Cyc_Absyn_tvar_id(struct Cyc_Absyn_Tvar*);
# 931
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 944
void*Cyc_Absyn_compress(void*);
# 957
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 959
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 962
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 964
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 966
extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_wchar_type (void);
void*Cyc_Absyn_gen_float_type(unsigned);
# 971
extern void*Cyc_Absyn_heap_rgn_type;
# 973
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 977
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 979
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_enum_type(struct _tuple1*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_tuple_type(struct Cyc_List_List*);
# 998
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 1002
void*Cyc_Absyn_exn_type (void);
# 1010
extern void*Cyc_Absyn_fat_bound_type;
# 1012
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 1014
void*Cyc_Absyn_bounds_one (void);
# 1017
void*Cyc_Absyn_fatconst (void);
void*Cyc_Absyn_thinconst (void);
# 1020
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1024
void*Cyc_Absyn_atb_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 1026
void*Cyc_Absyn_star_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1028
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1032
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1040
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1044
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1050
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 1052
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1077
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1085
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1107
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1112
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
# 1114
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
# 1121
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1124
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1129
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,int,unsigned);
# 1136
struct Cyc_Absyn_Exp*Cyc_Absyn_uniqueaqual_exp (void);
# 1141
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1152
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1161
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1165
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1170
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple1*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1210
void*Cyc_Absyn_pointer_expand(void*,int);
# 1212
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1217
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1219
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);
# 1244
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);
# 1247
void Cyc_Absyn_visit_type(int(*)(void*,void*),void*,void*);
# 37 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 43
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 74
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 81
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 55 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);
# 36 "tcutil.h"
int Cyc_Tcutil_is_arithmetic_type(void*);
# 38
int Cyc_Tcutil_is_tag_type(void*);
# 42
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 52
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 55
int Cyc_Tcutil_is_bits_only_type(void*);
# 60
void*Cyc_Tcutil_pointer_elt_type(void*);
# 62
void*Cyc_Tcutil_pointer_region(void*);
# 65
int Cyc_Tcutil_rgn_of_pointer(void*,void**);
# 67
void*Cyc_Tcutil_pointer_aqual(void*);
# 70
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 78
int Cyc_Tcutil_is_zero_pointer_type_elt(void*,void**);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*);
# 106
int Cyc_Tcutil_coerce_arg(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_list(struct Cyc_List_List*,void*,struct Cyc_List_List*);
# 121 "tcutil.h"
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple12{struct Cyc_Absyn_Decl*f0;struct Cyc_Absyn_Exp*f1;int f2;};
# 124
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*,void*);
# 126
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 132
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 139
struct Cyc_List_List*Cyc_Tcutil_rsubst_effconstr(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 147
int Cyc_Tcutil_subset_effect(int,void*,void*);
# 155
void*Cyc_Tcutil_evaluate_effect(struct Cyc_List_List*,void*);
# 160
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_List_List*f0;struct _RegionHandle*f1;};struct _tuple14{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 165
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 176
void Cyc_Tcutil_check_nonzero_bound(unsigned,void*);
# 178
void Cyc_Tcutil_check_bound(unsigned,unsigned,void*,int);
# 180
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 196
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*,void*,int);
# 201
int Cyc_Tcutil_is_noalias_path(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 206
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*,void*);struct _tuple15{int f0;void*f1;};
# 210
struct _tuple15 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 213
void*Cyc_Tcutil_normalize_effect(void*);
# 227
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 236
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 247
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 250
int Cyc_Tcutil_zeroable_type(void*);
# 254
int Cyc_Tcutil_force_type2bool(int,void*);
# 257
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 259
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);
# 274
void*Cyc_Tcutil_eval_aquals_of(struct Cyc_List_List*,void*);
# 280
int Cyc_Tcutil_is_cvar_type(void*);
void*Cyc_Tcutil_ptrbnd_cvar_equivalent(void*);
void*Cyc_Tcutil_get_pointer_bounds(void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
# 53 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_ako;
# 55
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
# 57
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
# 60
extern struct Cyc_Core_Opt Cyc_Kinds_aqko;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
struct Cyc_Core_Opt Cyc_Kinds_tvar_kind_opt(struct Cyc_Absyn_Tvar*);
# 93
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
int Cyc_Kinds_kind_eq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 75 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*,int);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 82
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 88
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 92
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 114
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 119
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 123
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
# 139
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*);
# 142
struct Cyc_List_List*Cyc_Tcenv_curr_effect_constraints(struct Cyc_Tcenv_Tenv*);
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
# 149
void Cyc_Tcenv_check_effect_accessible_nodelay(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 151
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 153
void Cyc_Tcenv_check_effect_constraints(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_List_List*);
# 33 "currgn.h"
void*Cyc_CurRgn_instantiate(void*,void*);struct _tuple16{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 32
extern int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 53
extern int Cyc_Evexp_okay_szofarg(void*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned);
# 34
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned);
# 47 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 79
int Cyc_Atts_attribute_cmp(void*,void*);
# 17 "bansheeif.h"
void*Cyc_BansheeIf_equality_constraint(void*,void*);
# 28
int Cyc_BansheeIf_add_constraint(unsigned,void*);
# 9 "subtype.h"
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible(unsigned,void*,void*);
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_warn(unsigned,void*,void*);
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_exact(unsigned,void*,void*);
# 18
int Cyc_Subtype_check_aqual_bounds(struct Cyc_List_List*,void*,void*);struct _tuple17{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct Cyc_Tcpat_TcPatResult{struct _tuple17*tvars_and_effconstr_opt;struct Cyc_List_List*patvars;struct Cyc_List_List*aquals_bounds;};
# 58 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**,struct Cyc_Absyn_Exp*);
# 112
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**);
# 53 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr ap){
# 55
Cyc_Warn_verr2(loc,ap);
if(topt!=0)return*topt;else{return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 58
static void Cyc_Tcexp_err_and_explain(unsigned loc,struct _fat_ptr ap){
Cyc_Warn_verr2(loc,ap);
Cyc_Unify_explain_failure();}
# 63
void Cyc_Tcexp_check_consume(unsigned loc,struct Cyc_List_List*aqb,void*t,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(aqb,t)&& !Cyc_Tcutil_is_noalias_path(aqb,e))
# 68
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Cannot consume non-unique paths; do swap instead",sizeof(char),49U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});}
# 73
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type_with_aqual(struct Cyc_Tcenv_Tenv*te,void*aq){
struct Cyc_List_List*tenv_tvs=Cyc_Tcenv_lookup_type_vars(te);
void*t2=Cyc_Absyn_new_evar(& Cyc_Kinds_ako,({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=tenv_tvs;_Tmp0;}));
void*rt=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=tenv_tvs;_Tmp0;}));
void*nbl=Cyc_Tcutil_any_bool(tenv_tvs);
void*b=Cyc_Tcutil_any_bounds(tenv_tvs);
void*zt=Cyc_Tcutil_any_bool(tenv_tvs);
void*rel=Cyc_Tcutil_any_bool(tenv_tvs);
struct Cyc_Absyn_PtrInfo _Tmp0;_Tmp0.elt_type=t2,({struct Cyc_Absyn_Tqual _Tmp1=Cyc_Absyn_empty_tqual(0U);_Tmp0.elt_tq=_Tmp1;}),_Tmp0.ptr_atts.eff=rt,_Tmp0.ptr_atts.nullable=nbl,_Tmp0.ptr_atts.bounds=b,_Tmp0.ptr_atts.zero_term=zt,_Tmp0.ptr_atts.ptrloc=0,_Tmp0.ptr_atts.autoreleased=rel,_Tmp0.ptr_atts.aqual=aq;return _Tmp0;}
# 84
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*tenv_tvs=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Tcenv_Tenv*_Tmp0=te;return Cyc_Tcexp_fresh_pointer_type_with_aqual(_Tmp0,({void*_Tmp1=Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=tenv_tvs;_Tmp2;}));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_rtd_qual_type);}));}struct _tuple18{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 90
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 94
if(topt==0){
# 98
if(des!=0){
struct _tuple18 _Tmp0=*((struct _tuple18*)des->hd);void*_Tmp1;_Tmp1=_Tmp0.f0;{struct Cyc_List_List*dls=_Tmp1;
if(dls!=0){
void*_Tmp2=(void*)dls->hd;void*_Tmp3;if(*((int*)_Tmp2)==1){_Tmp3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp2)->f1;{struct _fat_ptr*n=_Tmp3;
# 103
({void*_Tmp4=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct));_Tmp5->tag=29,_Tmp5->f1=0,_Tmp5->f2=0,_Tmp5->f3=des;_Tmp5;});e->r=_Tmp4;});
return;}}else{
goto _LL3;}_LL3:;}}}
# 109
({void*_Tmp0=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp1->tag=25,_Tmp1->f1=des;_Tmp1;});e->r=_Tmp0;});
return;}{
# 113
void*t=*topt;
void*_Tmp0=Cyc_Absyn_compress(t);int _Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;union Cyc_Absyn_AggrInfo _Tmp4;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp4;
# 116
{void*_Tmp5;if(info.UnknownAggr.tag==1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("struct type improperly set",sizeof(char),27U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({int(*_Tmp8)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;_Tmp8;})(loc,_tag_fat(_Tmp7,sizeof(void*),1));});else{_Tmp5=*info.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp5;
({void*_Tmp6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp7->tag=28,_Tmp7->f1=ad->name,_Tmp7->f2=0,_Tmp7->f3=des,_Tmp7->f4=ad;_Tmp7;});e->r=_Tmp6;});}};}
# 120
goto _LL8;}}else{goto _LLF;}case 5: _Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.tq;{void*at=_Tmp3;struct Cyc_Absyn_Tqual aq=_Tmp2;
({void*_Tmp5=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp6->tag=25,_Tmp6->f1=des;_Tmp6;});e->r=_Tmp5;});goto _LL8;}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{int it=_Tmp1;
({void*_Tmp5=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct));_Tmp6->tag=29,_Tmp6->f1=t,_Tmp6->f2=it,_Tmp6->f3=des;_Tmp6;});e->r=_Tmp5;});goto _LL8;}default: _LLF:
({void*_Tmp5=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp6->tag=25,_Tmp6->f1=des;_Tmp6;});e->r=_Tmp5;});goto _LL8;}_LL8:;}}
# 130
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 134
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;if(*((int*)_Tmp0)==4){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2==0){
# 137
if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("assignment in test",sizeof(char),19U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_warn2(e->loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL0;}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 145
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("test of ",sizeof(char),9U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=msg_part;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" has type ",sizeof(char),11U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)e->topt;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" instead of integral or pointer type",sizeof(char),37U);_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp5,sizeof(void*),5));});}
# 167 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 172
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*string_elt_typ=Cyc_Absyn_char_type;
int string_numelts=0;
union Cyc_Absyn_Cnst _Tmp0=*_check_null(c);struct _fat_ptr _Tmp1;enum Cyc_Absyn_Sign _Tmp2;int _Tmp3;switch(_Tmp0.String_c.tag){case 2: switch((int)_Tmp0.Char_c.val.f0){case Cyc_Absyn_Signed:
 return Cyc_Absyn_schar_type;case Cyc_Absyn_Unsigned:
 return Cyc_Absyn_uchar_type;default:
 return Cyc_Absyn_char_type;}case 3:
 return Cyc_Absyn_wchar_type();case 4: if(_Tmp0.Short_c.val.f0==Cyc_Absyn_Unsigned)
return Cyc_Absyn_ushort_type;else{
return Cyc_Absyn_sshort_type;}case 6: if(_Tmp0.LongLong_c.val.f0==Cyc_Absyn_Unsigned)
return Cyc_Absyn_ulonglong_type;else{
return Cyc_Absyn_slonglong_type;}case 7: _Tmp3=_Tmp0.Float_c.val.f1;{int i=_Tmp3;
# 187
if(topt==0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_Tmp4=Cyc_Absyn_compress(*topt);int _Tmp5;if(*((int*)_Tmp4)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==2){_Tmp5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{int i=_Tmp5;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E:
 return Cyc_Absyn_gen_float_type((unsigned)i);};}}case 5: _Tmp2=_Tmp0.Int_c.val.f0;_Tmp3=_Tmp0.Int_c.val.f1;{enum Cyc_Absyn_Sign csn=_Tmp2;int i=_Tmp3;
# 197
if(topt==0){
if((int)csn==1)return Cyc_Absyn_uint_type;else{return Cyc_Absyn_sint_type;}}{
void*_Tmp4=Cyc_Absyn_compress(*topt);void*_Tmp5;enum Cyc_Absyn_Sign _Tmp6;switch(*((int*)_Tmp4)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f2){case Cyc_Absyn_Char_sz: _Tmp6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{enum Cyc_Absyn_Sign sn=_Tmp6;
# 201
({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Char_c(sn,(char)i);*c=_Tmp7;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _Tmp6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{enum Cyc_Absyn_Sign sn=_Tmp6;
# 204
({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Short_c(sn,(short)i);*c=_Tmp7;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _Tmp6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{enum Cyc_Absyn_Sign sn=_Tmp6;
_Tmp6=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _Tmp6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_Tmp6;
# 210
({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(sn,i);*c=_Tmp7;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 5: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2!=0){_Tmp5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2->hd;{void*t1=_Tmp5;
# 219
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp7->tag=9,({struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_uint_exp((unsigned)i,0U);_Tmp7->f1=_Tmp8;});_Tmp7;}));}}else{goto _LL2D;}default: goto _LL2D;}case 4: if(i==0){
# 214
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _Tmp7=({union Cyc_Absyn_Cnst _Tmp8;_Tmp8.Null_c.tag=1U,_Tmp8.Null_c.val=0;_Tmp8;});*c=_Tmp7;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}else{goto _LL2D;}default: _LL2D:
# 221
 if((int)csn==1)return Cyc_Absyn_uint_type;else{return Cyc_Absyn_sint_type;}};}}case 1:
# 224
 if(topt!=0){
void*_Tmp4=Cyc_Absyn_compress(*topt);void*_Tmp5;if(*((int*)_Tmp4)==4){_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1.ptr_atts.nullable;{void*nbl=_Tmp5;
# 227
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("Used NULL when expecting a value of type ",sizeof(char),42U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)*topt;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp8,sizeof(void*),2));});
return*topt;}}else{
goto _LL2F;}_LL2F:;}{
# 232
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.ptr_atts.nullable=Cyc_Absyn_true_type;
return Cyc_Absyn_pointer_type(pi);}case 8: _Tmp1=_Tmp0.String_c.val;{struct _fat_ptr s=_Tmp1;
# 237
string_numelts=(int)_get_fat_size(s,sizeof(char));
_Tmp1=s;goto _LL1A;}default: _Tmp1=_Tmp0.Wstring_c.val;_LL1A: {struct _fat_ptr s=_Tmp1;
# 240
if(string_numelts==0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 244
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _Tmp4=Cyc_Absyn_Int_c(1U,string_numelts);Cyc_Absyn_const_exp(_Tmp4,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 249
void*t=({void*_Tmp4=string_elt_typ;void*_Tmp5=Cyc_Absyn_heap_rgn_type;void*_Tmp6=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp7=Cyc_Absyn_const_tqual(0U);void*_Tmp8=
Cyc_Absyn_thin_bounds_exp(elen);
# 249
void*_Tmp9=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Absyn_false_type);});
# 251
if(topt==0)
return t;{
void*_Tmp4=Cyc_Absyn_compress(*topt);struct Cyc_Absyn_Tqual _Tmp5;switch(*((int*)_Tmp4)){case 5: _Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp4)->f1.tq;{struct Cyc_Absyn_Tqual tq=_Tmp5;
# 257
void*_Tmp6=string_elt_typ;struct Cyc_Absyn_Tqual _Tmp7=tq;struct Cyc_Absyn_Exp*_Tmp8=elen;return Cyc_Absyn_array_type(_Tmp6,_Tmp7,_Tmp8,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);}case 4:  {
# 262
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(e->loc,t,*topt);
if((int)c!=0 &&(int)c!=1){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
return*topt;}
# 268
return t;}default:
 return t;};}}}}};}
# 275
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_Tmp0=*_check_null(b);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_Tmp0)->f1;{struct _tuple1*q=_Tmp1;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("unresolved binding in tcVar",sizeof(char),28U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;_Tmp4;})(loc,_tag_fat(_Tmp3,sizeof(void*),1));});}case 1: _Tmp1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 281
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2: _Tmp1=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 288
if(fd->fn_vardecl==0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5: _Tmp1=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
_Tmp1=vd;goto _LLA;}case 4: _Tmp1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_Tmp0)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_Tmp1;
_Tmp1=vd;goto _LLC;}default: _Tmp1=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_Tmp0)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 294
if(te->allow_valueof){
void*_Tmp2=Cyc_Absyn_compress(vd->type);void*_Tmp3;if(*((int*)_Tmp2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2!=0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2->hd;{void*i=_Tmp3;
({void*_Tmp4=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp5->tag=38,_Tmp5->f1=i;_Tmp5;});e->r=_Tmp4;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10:
 goto _LLD;}_LLD:;}
# 299
return vd->type;}};}
# 303
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 310
void*_Tmp0=fmt->r;struct _fat_ptr _Tmp1;switch(*((int*)_Tmp0)){case 0: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.tag==8){_Tmp1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.val;{struct _fat_ptr s=_Tmp1;
_Tmp1=s;goto _LL4;}}else{goto _LL5;}case 14: if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->r)==0){if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->r)->f1.String_c.tag==8){_Tmp1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->r)->f1.String_c.val;_LL4: {struct _fat_ptr s=_Tmp1;
# 313
struct Cyc_List_List*desc_types=type_getter(te,s,isCproto,fmt->loc);
if(opt_args==0)
return;{
struct Cyc_List_List*args=(struct Cyc_List_List*)opt_args->v;
# 318
for(1;desc_types!=0 && args!=0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_List_List*_Tmp2=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp3=e;Cyc_Tcutil_coerce_arg(_Tmp2,_Tmp3,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("descriptor has type ",sizeof(char),21U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" but argument has type ",sizeof(char),24U);_Tmp5;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp6;_Tmp6.tag=3,_Tmp6.f1=(void*)e->topt;_Tmp6;});void*_Tmp6[4];_Tmp6[0]=& _Tmp2,_Tmp6[1]=& _Tmp3,_Tmp6[2]=& _Tmp4,_Tmp6[3]=& _Tmp5;Cyc_Tcexp_err_and_explain(e->loc,_tag_fat(_Tmp6,sizeof(void*),4));});
if(alias_coercion)
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=(void*)arg_cnt,_Tmp3->tl=*alias_arg_exps;_Tmp3;});*alias_arg_exps=_Tmp2;});
({unsigned _Tmp2=e->loc;struct Cyc_List_List*_Tmp3=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp4=t;Cyc_Tcexp_check_consume(_Tmp2,_Tmp3,_Tmp4,e);});}
# 332
if(desc_types!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("too few arguments",sizeof(char),18U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(fmt->loc,_tag_fat(_Tmp3,sizeof(void*),1));});
if(args!=0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("too many arguments",sizeof(char),19U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(((struct Cyc_Absyn_Exp*)args->hd)->loc,_tag_fat(_Tmp3,sizeof(void*),1));});
# 337
for(1;args!=0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}
# 340
return;}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
# 344
 if(opt_args==0)
return;
{struct Cyc_List_List*args=(struct Cyc_List_List*)opt_args->v;for(0;args!=0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
({unsigned _Tmp2=((struct Cyc_Absyn_Exp*)args->hd)->loc;struct Cyc_List_List*_Tmp3=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp4=_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt);Cyc_Tcexp_check_consume(_Tmp2,_Tmp3,_Tmp4,(struct Cyc_Absyn_Exp*)args->hd);});}}
# 350
return;};}
# 354
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 356
void*t=Cyc_Absyn_compress(_check_null(e->topt));
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Minus: _LL4:
# 360
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting numeric type but found ",sizeof(char),34U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return _check_null(e->topt);case Cyc_Absyn_Not:
# 364
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting integral or * type but found ",sizeof(char),40U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot:
# 369
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting integral type but found ",sizeof(char),35U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return _check_null(e->topt);case Cyc_Absyn_Numelts:
# 373
{void*_Tmp0;if(*((int*)t)==4){_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;{void*b=_Tmp0;
# 375
if(Cyc_Tcutil_is_cvar_type(b))
# 378
({unsigned _Tmp1=loc;Cyc_BansheeIf_add_constraint(_Tmp1,({void*_Tmp2=_check_null(Cyc_Tcutil_ptrbnd_cvar_equivalent(b));Cyc_BansheeIf_equality_constraint(_Tmp2,Cyc_Absyn_fatconst());}));});else{
# 381
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if((eopt!=0 && !Cyc_Evexp_c_can_eval(eopt))&& !te->allow_valueof)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("cannot use numelts on a pointer with abstract bounds",sizeof(char),53U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});}
# 385
goto _LLF;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("numelts requires pointer type, not ",sizeof(char),36U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});}_LLF:;}
# 388
return Cyc_Absyn_uint_type;case Cyc_Absyn_Tagof:  {
# 392
struct Cyc_Absyn_PtrInfo pinfo=Cyc_Tcexp_fresh_pointer_type(te);
pinfo.ptr_atts.nullable=Cyc_Absyn_false_type;
({void*_Tmp0=Cyc_Absyn_bounds_one();pinfo.ptr_atts.bounds=_Tmp0;});
pinfo.ptr_atts.zero_term=Cyc_Absyn_false_type;{
void*p=Cyc_Absyn_pointer_type(pinfo);
if(Cyc_Tcutil_is_pointer_type(t)){
# 406 "tcexp.cyc"
enum Cyc_Absyn_Coercion coercion=Cyc_Subtype_coercible_exact(loc,t,p);
if((int)coercion==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting @thin@numelts(1)@nozeroterm pointer type but found %s",sizeof(char),64U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_uint_type;}}else{
# 413
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting @thin@numelts(1)@nozeroterm pointer type but found %s",sizeof(char),64U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_uint_type;}{
# 417
void*elt_type=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(p));
void*rgn_type=Cyc_Tcutil_pointer_region(p);
if(*((int*)elt_type)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_type)->f1)){case 22:
 goto _LL14;case 23:
 goto _LL14;default: goto _LL19;}else{_LL19:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting pointer to datatype but found %s",sizeof(char),43U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)elt_type;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});}_LL14:;
# 424
Cyc_Tcenv_check_effect_accessible(te,loc,rgn_type);
return Cyc_Absyn_uint_type;}}}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Non-unary primop",sizeof(char),17U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;_Tmp2;})(loc,_tag_fat(_Tmp1,sizeof(void*),1));});};}
# 432
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=_check_null(e1->topt);
void*t2=_check_null(e2->topt);
void*new_typ=Cyc_Tcutil_max_arithmetic_type(t1,t2);
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,1U);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,1U);
return new_typ;}
# 442
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 445
if(!checker(e1)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type ",sizeof(char),6U);_Tmp1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=(void*)e1->topt;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" cannot be used here",sizeof(char),21U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;return Cyc_Tcexp_expr_err(te,e1->loc,0,_tag_fat(_Tmp3,sizeof(void*),3));}
if(!checker(e2)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type ",sizeof(char),6U);_Tmp1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=(void*)e2->topt;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" cannot be used here",sizeof(char),21U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;return Cyc_Tcexp_expr_err(te,e2->loc,0,_tag_fat(_Tmp3,sizeof(void*),3));}
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 452
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;if(*((int*)t1)==4){_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_tq;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.eff;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.autoreleased;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.aqual;{void*et=_Tmp7;struct Cyc_Absyn_Tqual tq=_Tmp6;void*r=_Tmp5;void*n=_Tmp4;void*b=_Tmp3;void*zt=_Tmp2;void*rel=_Tmp1;void*aq=_Tmp0;
# 457
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Kinds_mk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("can't do arithmetic on abstract pointer type",sizeof(char),45U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp9,sizeof(void*),1));});
if(({struct Cyc_List_List*_Tmp8=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer(_Tmp8,t1,1);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("can't do arithmetic on unique pointer",sizeof(char),38U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp9,sizeof(void*),1));});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("expecting int but found ",sizeof(char),25U);_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)t2;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err2(e2->loc,_tag_fat(_TmpA,sizeof(void*),2));});
if(!Cyc_Tcutil_is_cvar_type(b)){
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if(eopt==0)
return t1;
# 469
if(Cyc_Tcutil_force_type2bool(0,zt)){
# 471
struct _tuple16 _Tmp8=Cyc_Evexp_eval_const_uint_exp(eopt);int _Tmp9;unsigned _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{unsigned i=_TmpA;int known=_Tmp9;
if(known && i==1U)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("pointer arithmetic on thin, zero-terminated pointer may be expensive.",sizeof(char),70U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_Warn_warn2(e1->loc,_tag_fat(_TmpC,sizeof(void*),1));});}}}else{
# 479
if(!({unsigned _Tmp8=e1->loc;Cyc_BansheeIf_add_constraint(_Tmp8,({void*_Tmp9=b;Cyc_BansheeIf_equality_constraint(_Tmp9,Cyc_Absyn_fatconst());}));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("Unable to determine bound on pointer type ",sizeof(char),43U);_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)t1;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err2(e1->loc,_tag_fat(_TmpA,sizeof(void*),2));});}{
# 488
struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1;new_t1=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct)),new_t1->tag=4,new_t1->f1.elt_type=et,new_t1->f1.elt_tq=tq,new_t1->f1.ptr_atts.eff=r,new_t1->f1.ptr_atts.nullable=Cyc_Absyn_true_type,new_t1->f1.ptr_atts.bounds=Cyc_Absyn_fat_bound_type,new_t1->f1.ptr_atts.zero_term=zt,new_t1->f1.ptr_atts.ptrloc=0,new_t1->f1.ptr_atts.autoreleased=rel,new_t1->f1.ptr_atts.aqual=aq;
# 491
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,3U);
return(void*)new_t1;}}}else{
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);};}
# 496
static void*Cyc_Tcexp_relqual(void*t){
void*_Tmp0;if(*((int*)t)==4){_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.autoreleased;{void*rel=_Tmp0;
return rel;}}else{
return Cyc_Absyn_false_type;};}
# 507
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=_check_null(e1->topt);
void*t2=_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
# 513
if(!Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2))
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,3U);
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 520
if(!Cyc_Tcutil_is_pointer_type(t2)){
# 522
if((unsigned)topt && Cyc_Tcutil_is_pointer_type(*topt)|| !((unsigned)topt)){
if(!Cyc_Tcutil_is_fat_pointer_type(t1)){
void*pb=Cyc_Tcutil_get_pointer_bounds(t1);
if(!Cyc_Tcutil_is_cvar_type(_check_null(pb))|| !({
unsigned _Tmp0=e1->loc;Cyc_BansheeIf_add_constraint(_Tmp0,({void*_Tmp1=pb;Cyc_BansheeIf_equality_constraint(_Tmp1,Cyc_Absyn_fatconst());}));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Expected fat pointer type but got ",sizeof(char),35U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp2,sizeof(void*),2));});}
# 530
return t1;}else{
# 533
if(Cyc_Tcutil_is_fat_pointer_type(t1)||
 Cyc_Tcutil_is_cvar_type(_check_null(Cyc_Tcutil_get_pointer_bounds(t2)))&& !({
unsigned _Tmp0=e1->loc;Cyc_BansheeIf_add_constraint(_Tmp0,({void*_Tmp1=_check_null(Cyc_Tcutil_get_pointer_bounds(t1));Cyc_BansheeIf_equality_constraint(_Tmp1,
Cyc_Absyn_thinconst());}));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Expected thin pointer type but got ",sizeof(char),36U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t2;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp2,sizeof(void*),2));});
# 539
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,3U);
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,3U);
return Cyc_Absyn_sint_type;}}{
# 546
void*t1_elt=Cyc_Tcutil_pointer_elt_type(t1);
void*t2_elt=Cyc_Tcutil_pointer_elt_type(t2);
enum Cyc_Absyn_Coercion coercion=Cyc_Subtype_coercible_exact(e1->loc,t1_elt,t2_elt);
if((int)coercion==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("pointer arithmetic on values of different ",sizeof(char),43U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=
_tag_fat("types (",sizeof(char),8U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t1;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t2;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(")",sizeof(char),2U);_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Tcexp_err_and_explain(e1->loc,_tag_fat(_Tmp6,sizeof(void*),6));});
# 554
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Kinds_mk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't perform arithmetic on abstract pointer type",sizeof(char),50U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
if(({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer(_Tmp0,t1,1);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't perform arithmetic on unique pointer",sizeof(char),43U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 560
if(!({int _Tmp0=Cyc_Tcutil_is_fat_pointer_type(t1);_Tmp0 ^ Cyc_Tcutil_is_fat_pointer_type(t2);}))
return Cyc_Absyn_sint_type;
# 563
if(Cyc_Tcutil_is_fat_pointer_type(t1))
({struct Cyc_Absyn_Exp*_Tmp0=e1;Cyc_Tcutil_unchecked_cast(_Tmp0,({void*_Tmp1=Cyc_Tcutil_pointer_elt_type(t1);void*_Tmp2=
Cyc_Tcutil_pointer_region(t1);
# 564
void*_Tmp3=
Cyc_Tcutil_pointer_aqual(t1);
# 564
struct Cyc_Absyn_Tqual _Tmp4=
# 566
Cyc_Absyn_empty_tqual(0U);
# 564
void*_Tmp5=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,
# 566
Cyc_Tcexp_relqual(t1));}),3U);});else{
# 570
({struct Cyc_Absyn_Exp*_Tmp0=e2;Cyc_Tcutil_unchecked_cast(_Tmp0,({void*_Tmp1=Cyc_Tcutil_pointer_elt_type(t2);void*_Tmp2=
Cyc_Tcutil_pointer_region(t2);
# 570
void*_Tmp3=
Cyc_Tcutil_pointer_aqual(t2);
# 570
struct Cyc_Absyn_Tqual _Tmp4=
# 572
Cyc_Absyn_empty_tqual(0U);
# 570
void*_Tmp5=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,
# 572
Cyc_Tcexp_relqual(t2));}),3U);});}
# 576
return Cyc_Absyn_sint_type;}}
# 579
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 586
if((int)Cyc_Tcutil_type_kind(t1)->kind==2 &&(int)Cyc_Tcutil_type_kind(t2)->kind==2){
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;{
# 591
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(loc,t1,t2);
if((int)c==0){
c=Cyc_Subtype_coercible(loc,t2,t1);
if((int)c!=0){
Cyc_Tcutil_unchecked_cast(e2,t1,c);
return Cyc_Absyn_sint_type;}}else{
# 600
Cyc_Tcutil_unchecked_cast(e1,t2,c);
return Cyc_Absyn_sint_type;}}}
# 606
{struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0.f0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==4){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==4)
return Cyc_Absyn_sint_type;else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}case 4: if(*((int*)_Tmp0.f1)==4){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;{void*t1a=_Tmp2;void*t2a=_Tmp1;
# 609
if((int)Cyc_Subtype_coercible_exact(loc,t1a,t2a)!=0 ||(int)
Cyc_Subtype_coercible_exact(loc,t2a,t1a)!=0)
return Cyc_Absyn_sint_type;
# 613
goto _LL6;}}else{goto _LL5;}default: _LL5: _LL6: {
# 615
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(loc,t1,t2);
if((int)c!=0){
Cyc_Tcutil_unchecked_cast(e1,t2,c);
return Cyc_Absyn_sint_type;}else{
# 621
c=Cyc_Subtype_coercible(loc,t2,t1);
if((int)c!=0){
Cyc_Tcutil_unchecked_cast(e2,t1,c);
return Cyc_Absyn_sint_type;}}
# 627
goto _LL0;}}_LL0:;}
# 630
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("comparison not allowed between ",sizeof(char),32U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" and ",sizeof(char),6U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_err_and_explain(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 636
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop*p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 638
enum Cyc_Absyn_Primop _Tmp0=*p;switch((int)_Tmp0){case Cyc_Absyn_Plus:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
 return Cyc_Tcexp_tcMinus(te,topt,e1,e2);case Cyc_Absyn_Times:
# 642
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_UDiv:
 goto _LLA;case Cyc_Absyn_Div: _LLA: {
# 646
void*res=Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);
void*_Tmp1=Cyc_Absyn_compress(res);if(*((int*)_Tmp1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)->f1==Cyc_Absyn_Unsigned){
# 649
*p=20U;return res;}else{goto _LL32;}}else{goto _LL32;}}else{_LL32:
 return res;};}case Cyc_Absyn_UMod:
# 653
 goto _LLE;case Cyc_Absyn_Mod: _LLE: {
# 656
void*res=Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);
void*_Tmp1=Cyc_Absyn_compress(res);if(*((int*)_Tmp1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)->f1==Cyc_Absyn_Unsigned){
# 659
*p=21U;return res;}else{goto _LL37;}}else{goto _LL37;}}else{_LL37:
 return res;};}case Cyc_Absyn_Bitand:
# 663
 goto _LL12;case Cyc_Absyn_Bitor: _LL12:
 goto _LL14;case Cyc_Absyn_Bitxor: _LL14:
# 666
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Bitlshift:
 goto _LL18;case Cyc_Absyn_Bitlrshift: _LL18:
# 669
 if(!Cyc_Tcutil_is_integral(e1)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("type ",sizeof(char),6U);_Tmp2;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3;_Tmp3.tag=3,_Tmp3.f1=(void*)e1->topt;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" cannot be used with << or >>",sizeof(char),30U);_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;return Cyc_Tcexp_expr_err(te,e1->loc,0,_tag_fat(_Tmp4,sizeof(void*),3));}
if(!Cyc_Tcutil_is_integral(e2)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("type ",sizeof(char),6U);_Tmp2;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3;_Tmp3.tag=3,_Tmp3.f1=(void*)e2->topt;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" cannot be used with << or >>",sizeof(char),30U);_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;return Cyc_Tcexp_expr_err(te,e2->loc,0,_tag_fat(_Tmp4,sizeof(void*),3));}
return _check_null(e1->topt);case Cyc_Absyn_Eq:
# 676
 goto _LL1C;case Cyc_Absyn_Neq: _LL1C:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);case Cyc_Absyn_Gt:
 goto _LL20;case Cyc_Absyn_Lt: _LL20:
 goto _LL22;case Cyc_Absyn_Gte: _LL22:
 goto _LL24;case Cyc_Absyn_Lte: _LL24:
 goto _LL26;case Cyc_Absyn_UGt: _LL26:
 goto _LL28;case Cyc_Absyn_ULt: _LL28:
 goto _LL2A;case Cyc_Absyn_UGte: _LL2A:
 goto _LL2C;case Cyc_Absyn_ULte: _LL2C: {
# 688
void*res=Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);
{struct _tuple0 _Tmp1=({struct _tuple0 _Tmp2;({void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));_Tmp2.f0=_Tmp3;}),({void*_Tmp3=Cyc_Absyn_compress(_check_null(e2->topt));_Tmp2.f1=_Tmp3;});_Tmp2;});if(*((int*)_Tmp1.f0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1.f0)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1.f0)->f1)->f1==Cyc_Absyn_Unsigned)
goto _LL3D;else{switch(*((int*)_Tmp1.f1)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1.f1)->f1)->f1==Cyc_Absyn_Unsigned)goto _LL3C;else{goto _LL42;}}else{goto _LL42;}case 4: goto _LL40;default: goto _LL42;}}}else{switch(*((int*)_Tmp1.f1)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1.f1)->f1)->f1==Cyc_Absyn_Unsigned)goto _LL3C;else{goto _LL42;}}else{goto _LL42;}case 4: goto _LL40;default: goto _LL42;}}}else{if(*((int*)_Tmp1.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1.f1)->f1)->f1==Cyc_Absyn_Unsigned){_LL3C: _LL3D:
 goto _LL3F;}else{if(*((int*)_Tmp1.f0)==4)goto _LL3E;else{goto _LL42;}}}else{if(*((int*)_Tmp1.f0)==4)goto _LL3E;else{goto _LL42;}}}else{if(*((int*)_Tmp1.f0)==4){_LL3E: _LL3F:
 goto _LL41;}else{if(*((int*)_Tmp1.f1)==4){_LL40: _LL41:
# 694
 if((int)*p==7)*p=22U;
if((int)*p==8)*p=23U;
if((int)*p==9)*p=24U;
if((int)*p==10)*p=25U;
goto _LL39;}else{_LL42:
 goto _LL39;}}}}_LL39:;}
# 701
return res;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("bad binary primop",sizeof(char),18U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;({int(*_Tmp3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp3;})(_tag_fat(_Tmp2,sizeof(void*),1));});};}
# 706
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop*p,struct Cyc_List_List*es){
# 714
if((int)*p==2 && Cyc_List_length(es)==1){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;
void*t=Cyc_Tcexp_tcExp(te,topt,e);
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting numeric type but found ",sizeof(char),34U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return t;}
# 721
{struct Cyc_List_List*es2=es;for(0;es2!=0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _Tmp0=Cyc_List_length(es);switch((int)_Tmp0){case 0:  {
struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("primitive operator has 0 arguments",sizeof(char),35U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp2,sizeof(void*),1));}case 1:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,*p,(struct Cyc_Absyn_Exp*)_check_null(es)->hd);case 2:  {
struct Cyc_Tcenv_Tenv*_Tmp1=te;unsigned _Tmp2=loc;void**_Tmp3=topt;enum Cyc_Absyn_Primop*_Tmp4=p;struct Cyc_Absyn_Exp*_Tmp5=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;return Cyc_Tcexp_tcBinPrimop(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd);}default:  {
struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("primitive operator has > 2 arguments",sizeof(char),37U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp2,sizeof(void*),1));}};}}struct _tuple19{struct Cyc_Absyn_Tqual f0;void*f1;};
# 731
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
t=Cyc_Absyn_compress(t);{
struct Cyc_Absyn_Tqual _Tmp0;void*_Tmp1;switch(*((int*)t)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 24: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.tag==2){_Tmp1=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 735
if(ad->impl==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("attempt to write abstract ",sizeof(char),27U);_Tmp3;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp4;_Tmp4.tag=6,_Tmp4.f1=ad;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),2));});
return 0;}
# 739
_Tmp1=ad->impl->fields;goto _LL4;}}else{goto _LL9;}case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownDatatypefield.tag==2){_Tmp1=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownDatatypefield.val.f1;{struct Cyc_Absyn_Datatypefield*df=_Tmp1;
# 751
{struct Cyc_List_List*fs=df->typs;for(0;fs!=0;fs=fs->tl){
struct _tuple19*_Tmp2=(struct _tuple19*)fs->hd;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("attempt to overwrite a datatype field (",sizeof(char),40U);_Tmp6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=df->name;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(") with a const member",sizeof(char),22U);_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),3));});
return 0;}
# 758
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 760
return 1;}}else{goto _LL9;}default: goto _LL9;}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;_LL4: {struct Cyc_List_List*fs=_Tmp1;
# 741
for(1;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)fs->hd;
if(f->tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("attempt to overwrite an aggregate with const member ",sizeof(char),53U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*f->name;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),2));});
return 0;}
# 747
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 749
return 1;}case 5: _Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp0=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;{void*elt_type=_Tmp1;struct Cyc_Absyn_Tqual tq=_Tmp0;
# 762
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("attempt to overwrite a const array",sizeof(char),35U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),1));});
return 0;}
# 766
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}default: _LL9:
 return 1;};}}
# 774
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 777
if(!Cyc_Tcexp_check_writable_aggr(e->loc,_check_null(e->topt)))return;
{void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 3: _Tmp2=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL4;}case 4: _Tmp2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL6;}case 5: _Tmp2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL8;}case 1: _Tmp2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
if(!vd->tq.real_const)return;goto _LL0;}default: goto _LL15;}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 784
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));struct Cyc_Absyn_Tqual _Tmp4;switch(*((int*)_Tmp3)){case 4: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_tq;{struct Cyc_Absyn_Tqual tq=_Tmp4;
_Tmp4=tq;goto _LL1B;}case 5: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp3)->f1.tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_Tmp4;
if(!tq.real_const)return;goto _LL17;}default:
 goto _LL17;}_LL17:;}
# 789
goto _LL0;}case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 791
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp4;switch(*((int*)_Tmp3)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f1.KnownAggr.tag==2){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp4;
# 793
struct Cyc_Absyn_Aggrfield*sf=adp==0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf==0 || !sf->tq.real_const)return;
goto _LL1E;}}else{goto _LL23;}}else{goto _LL23;}case 7: _Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp3)->f3;{struct Cyc_List_List*fs=_Tmp4;
# 797
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf==0 || !sf->tq.real_const)return;
goto _LL1E;}default: _LL23:
 goto _LL1E;}_LL1E:;}
# 802
goto _LL0;}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 804
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;if(*((int*)_Tmp3)==4){_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_tq;{void*elt_typ=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;
# 806
if(!tq.real_const){
void*_Tmp6=Cyc_Absyn_compress(elt_typ);void*_Tmp7;switch(*((int*)_Tmp6)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownAggr.tag==2){_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp7;
# 809
struct Cyc_Absyn_Aggrfield*sf=
adp==0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf==0 || !sf->tq.real_const)return;
goto _LL2A;}}else{goto _LL2F;}}else{goto _LL2F;}case 7: _Tmp7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp6)->f3;{struct Cyc_List_List*fs=_Tmp7;
# 814
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf==0 || !sf->tq.real_const)return;
goto _LL2A;}default: _LL2F:
 goto _LL2A;}_LL2A:;}
# 820
goto _LL25;}}else{
goto _LL25;}_LL25:;}
# 823
goto _LL0;}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
# 825
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));struct Cyc_Absyn_Tqual _Tmp4;switch(*((int*)_Tmp3)){case 4: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_tq;{struct Cyc_Absyn_Tqual tq=_Tmp4;
_Tmp4=tq;goto _LL35;}case 5: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp3)->f1.tq;_LL35: {struct Cyc_Absyn_Tqual tq=_Tmp4;
if(!tq.real_const)return;goto _LL31;}default:
 goto _LL31;}_LL31:;}
# 830
goto _LL0;}case 12: _Tmp2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
_Tmp2=e1;goto _LL14;}case 13: _Tmp2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp2;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15:
 goto _LL0;}_LL0:;}
# 835
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("attempt to write a const location: ",sizeof(char),36U);_Tmp1;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=e;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});}
# 838
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 841
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_Tmp0,0,e);});
if(!Cyc_Absyn_is_lvalue(e)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("increment/decrement of non-lvalue",sizeof(char),34U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp1,sizeof(void*),1));}
Cyc_Tcexp_check_writable(te,e);{
void*t=_check_null(e->topt);
# 847
if(!Cyc_Tcutil_is_numeric(e)){
void*pb=Cyc_Tcutil_get_pointer_bounds(t);
void*telt=Cyc_Absyn_void_type;
if((unsigned)pb && Cyc_Tcutil_is_cvar_type(pb)){
# 852
if(!({unsigned _Tmp0=e->loc;Cyc_BansheeIf_add_constraint(_Tmp0,({void*_Tmp1=pb;Cyc_BansheeIf_equality_constraint(_Tmp1,Cyc_Absyn_fatconst());}));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("constraint failure: expecting arithmetic or ? type but found ",sizeof(char),62U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});}else{
# 857
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i==0 ||(int)i==1)){
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Kinds_mk))
Cyc_Warn_err(e->loc,_tag_fat("can't perform arithmetic on abstract pointer type",sizeof(char),50U),_tag_fat(0U,sizeof(void*),0));
if(({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer(_Tmp0,t,1);}))
Cyc_Warn_err(e->loc,_tag_fat("can't perform arithmetic on unique pointer",sizeof(char),43U),_tag_fat(0U,sizeof(void*),0));}else{
# 865
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting arithmetic or ? type but found ",sizeof(char),42U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});}}}
# 867
return t;}}
# 871
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 873
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_Tmp1=e1;Cyc_Tcexp_tcTest(_Tmp0,_Tmp1,_tag_fat("conditional expression",sizeof(char),23U));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*ko=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_ako:& Cyc_Kinds_mko;
void*t=({struct Cyc_Core_Opt*_Tmp0=ko;Cyc_Absyn_new_evar(_Tmp0,Cyc_Tcenv_lookup_opt_type_vars(te));});
struct Cyc_List_List l1=({struct Cyc_List_List _Tmp0;_Tmp0.hd=e3,_Tmp0.tl=0;_Tmp0;});
struct Cyc_List_List l2=({struct Cyc_List_List _Tmp0;_Tmp0.hd=e2,_Tmp0.tl=& l1;_Tmp0;});
if(!({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_coerce_list(_Tmp0,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("conditional clause types do not match: ",sizeof(char),40U);_Tmp1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=(void*)e2->topt;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat(" != ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)e3->topt;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_err_and_explain(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return t;}}
# 887
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 889
Cyc_Tcexp_tcTest(te,e1,_tag_fat("logical-and expression",sizeof(char),23U));
Cyc_Tcexp_tcTest(te,e2,_tag_fat("logical-and expression",sizeof(char),23U));
return Cyc_Absyn_sint_type;}
# 893
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 895
Cyc_Tcexp_tcTest(te,e1,_tag_fat("logical-or expression",sizeof(char),22U));
Cyc_Tcexp_tcTest(te,e2,_tag_fat("logical-or expression",sizeof(char),22U));
return Cyc_Absyn_sint_type;}
# 901
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 908
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_Tmp0,0,e1);});{
void*t1=_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=_check_null(e2->topt);
# 913
{void*_Tmp0=Cyc_Absyn_compress(t1);if(*((int*)_Tmp0)==5){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("cannot assign to an array",sizeof(char),26U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});goto _LL0;}else{
goto _LL0;}_LL0:;}
# 918
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Kinds_mk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type is abstract (can't determine size)",sizeof(char),40U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 922
if(!Cyc_Absyn_is_lvalue(e1)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("assignment to non-lvalue",sizeof(char),25U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp1,sizeof(void*),1));}
Cyc_Tcexp_check_writable(te,e1);
if(po==0){
({unsigned _Tmp0=e2->loc;struct Cyc_List_List*_Tmp1=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp2=t2;Cyc_Tcexp_check_consume(_Tmp0,_Tmp1,_Tmp2,e2);});
if(!({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp1=e2;Cyc_Tcutil_coerce_assign(_Tmp0,_Tmp1,t1);})){
void*result=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type mismatch: ",sizeof(char),16U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp4,sizeof(void*),4));});
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}
# 933
return t1;}{
# 935
struct Cyc_Absyn_Exp*e1copy=Cyc_Absyn_copy_exp(e1);
void*t_result=Cyc_Tcexp_tcBinPrimop(te,loc,0,(enum Cyc_Absyn_Primop*)((enum Cyc_Absyn_Primop)& po->v),e1copy,e2);
if(((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))&& !Cyc_Tcutil_is_tag_type(t1))
return t1;
if(!(Cyc_Unify_unify(t_result,t1)||(Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))&& !Cyc_Tcutil_is_tag_type(t1))){
void*result=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=
_tag_fat("Cannot use this operator in an assignment when ",sizeof(char),48U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=
_tag_fat("the arguments have types ",sizeof(char),26U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t1;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" and ",sizeof(char),6U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t2;_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp5,sizeof(void*),5));});
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}
# 947
return t_result;}}}}
# 951
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,0,e1);});
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);void**_Tmp1=topt;Cyc_Tcexp_tcExp(_Tmp0,_Tmp1,e2);});
return _check_null(e2->topt);}
# 960
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 963
struct Cyc_List_List*fields;
void*t1=_check_null(e->topt);
# 966
{void*_Tmp0=Cyc_Absyn_compress(t1);if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1==0){
# 968
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,1U);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f2){case Cyc_Absyn_Char_sz:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL6:
# 971
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,1U);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;}
# 975
for(fields=fs;fields!=0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _Tmp0=*((struct Cyc_Absyn_Datatypefield*)fields->hd);enum Cyc_Absyn_Scope _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp0.name;_Tmp3=_Tmp0.typs;_Tmp2=_Tmp0.loc;_Tmp1=_Tmp0.sc;{struct _tuple1*n=_Tmp4;struct Cyc_List_List*typs=_Tmp3;unsigned loc=_Tmp2;enum Cyc_Absyn_Scope sc=_Tmp1;
# 978
if(typs==0 || typs->tl!=0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple19*)typs->hd)).f1);
# 981
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 985
for(fields=fs;fields!=0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _Tmp0=*((struct Cyc_Absyn_Datatypefield*)fields->hd);enum Cyc_Absyn_Scope _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp0.name;_Tmp3=_Tmp0.typs;_Tmp2=_Tmp0.loc;_Tmp1=_Tmp0.sc;{struct _tuple1*n=_Tmp4;struct Cyc_List_List*typs=_Tmp3;unsigned loc=_Tmp2;enum Cyc_Absyn_Scope sc=_Tmp1;
# 988
if(typs==0 || typs->tl!=0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple19*)typs->hd)).f1);
# 992
int bogus=0;
# 994
enum Cyc_Absyn_Coercion crc=Cyc_Subtype_coercible_warn(e->loc,_check_null(e->topt),t2);
if((int)crc!=0){
if((int)crc!=1)
Cyc_Tcutil_unchecked_cast(e,t2,crc);
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}}
# 1001
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't find a field in ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)tu;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" to inject value of type ",sizeof(char),26U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t1;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return 0;}
# 1005
static void Cyc_Tcexp_check_all_aqual_bounds(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*fn_or_aggr_qual_bnd){
# 1009
struct Cyc_List_List*qbit=fn_or_aggr_qual_bnd;for(0;qbit!=0;qbit=qbit->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)qbit->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*qv=_Tmp2;void*bnd=_Tmp1;
void*qvc=Cyc_Absyn_compress(qv);
void*_Tmp3;void*_Tmp4;switch(*((int*)qvc)){case 1: _Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)qvc)->f2;{void**ref=_Tmp4;
# 1015
goto _LL3;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f1)){case 17: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2)->hd)==1){_Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2->hd)->f2;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2->tl;{void**ref=(void**)_Tmp4;struct Cyc_List_List*ebnd=_Tmp3;
# 1017
if(!Cyc_Subtype_check_aqual_bounds(0,(void*)_check_null(ebnd)->hd,bnd)&& !
Cyc_Subtype_check_aqual_bounds(0,bnd,(void*)ebnd->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)ebnd->hd;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" <> ",sizeof(char),5U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)bnd;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),4));});
goto _LL3;}}else{goto _LLA;}}else{goto _LLA;}case 15: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2)->hd)==1){
# 1023
if(!({struct Cyc_List_List*_Tmp5=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp6=Cyc_Absyn_al_qual_type;Cyc_Subtype_check_aqual_bounds(_Tmp5,_Tmp6,bnd);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)qvc;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" <> ",sizeof(char),5U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)bnd;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),4));});
goto _LL3;}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}default: _LLA:
# 1027
 if(!({struct Cyc_List_List*_Tmp5=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp6=qvc;Cyc_Subtype_check_aqual_bounds(_Tmp5,_Tmp6,bnd);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)qvc;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" <> ",sizeof(char),5U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)bnd;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),4));});
goto _LL3;}_LL3:;}}}char Cyc_Tcexp_Found_evar[11U]="Found_evar";struct Cyc_Tcexp_Found_evar_exn_struct{char*tag;};
# 1035
static struct Cyc_Tcexp_Found_evar_exn_struct Cyc_Tcexp_Found_evar_val={Cyc_Tcexp_Found_evar};
# 1037
static int Cyc_Tcexp_contains_free_evar(void*env,void*t){
{void*_Tmp0=Cyc_Absyn_compress(t);if(*((int*)_Tmp0)==1)
# 1040
_throw(& Cyc_Tcexp_Found_evar_val);else{
# 1042
goto _LL0;}_LL0:;}
# 1044
return 1;}
# 1047
static struct Cyc_List_List*Cyc_Tcexp_fresh_effect_evars(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*inst,struct Cyc_List_List*orig){
# 1050
if(({int _Tmp0=Cyc_List_length(inst);_Tmp0!=Cyc_List_length(orig);}))
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("injection orig and inst must have same length",sizeof(char),46U),_tag_fat(0U,sizeof(void*),0));{
void*currrgn=Cyc_Tcenv_curr_rgn(te);
struct Cyc_List_List*fresh=0;
for(1;(unsigned)inst &&(unsigned)orig;(inst=inst->tl,orig=orig->tl)){
struct _tuple14*_Tmp0=(struct _tuple14*)orig->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_Absyn_Tvar*otv=_Tmp2;void*ot=_Tmp1;
struct Cyc_Core_Opt _Tmp3=Cyc_Kinds_tvar_kind_opt(otv);void*_Tmp4;if(_Tmp3.v==0){
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("Tvar kind must be set by now",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));return 0;}else{_Tmp4=(struct Cyc_Absyn_Kind*)_Tmp3.v;{struct Cyc_Absyn_Kind*k=_Tmp4;
# 1059
if(Cyc_Kinds_kind_eq(k,& Cyc_Kinds_ek)){
void*cot=Cyc_Tcutil_normalize_effect(ot);
struct _tuple14*_Tmp5=(struct _tuple14*)inst->hd;void*_Tmp6;_Tmp6=(void**)& _Tmp5->f1;{void**t=(void**)_Tmp6;
if(*((int*)cot)==1){
# 1064
({void*_Tmp7=Cyc_Absyn_join_eff(({void*_Tmp8[2];_Tmp8[0]=currrgn,({void*_Tmp9=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,Cyc_Tcenv_lookup_opt_type_vars(te));_Tmp8[1]=_Tmp9;});Cyc_List_list(_tag_fat(_Tmp8,sizeof(void*),2));}));*t=_Tmp7;});
fresh=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=(struct _tuple14*)inst->hd,_Tmp7->tl=fresh;_Tmp7;});
goto _LLB;}else{
# 1068
int dummy=0;
{struct _handler_cons _Tmp7;_push_handler(& _Tmp7);{int _Tmp8=0;if(setjmp(_Tmp7.handler))_Tmp8=1;if(!_Tmp8){
({void(*_Tmp9)(int(*)(int,void*),int,void*)=(void(*)(int(*)(int,void*),int,void*))Cyc_Absyn_visit_type;_Tmp9;})(({int(*_Tmp9)(int,void*)=(int(*)(int,void*))Cyc_Tcexp_contains_free_evar;_Tmp9;}),dummy,cot);;_pop_handler();}else{void*_Tmp9=(void*)Cyc_Core_get_exn_thrown();void*_TmpA;if(((struct Cyc_Tcexp_Found_evar_exn_struct*)_Tmp9)->tag==Cyc_Tcexp_Found_evar){
# 1073
struct Cyc_List_List*inst_effect;inst_effect=_cycalloc(sizeof(struct Cyc_List_List)),inst_effect->hd=currrgn,inst_effect->tl=0;
{void*_TmpB;if(*((int*)cot)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)cot)->f1)==9){_TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)cot)->f2;{struct Cyc_List_List*ts=_TmpB;
# 1076
for(1;(unsigned)ts;ts=ts->tl){
void*_TmpC=(void*)ts->hd;if(*((int*)_TmpC)==1)
goto _LL1A;else{
# 1080
inst_effect=({struct Cyc_List_List*_TmpD=_cycalloc(sizeof(struct Cyc_List_List));_TmpD->hd=(void*)ts->hd,_TmpD->tl=inst_effect;_TmpD;});
goto _LL1A;}_LL1A:;}
# 1084
goto _LL15;}}else{goto _LL18;}}else{_LL18:
# 1086
({int(*_TmpC)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpC;})(_tag_fat("!evar && Contains evar => JoinCon",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));}_LL15:;}
# 1088
inst_effect=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpC=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,Cyc_Tcenv_lookup_opt_type_vars(te));_TmpB->hd=_TmpC;}),_TmpB->tl=inst_effect;_TmpB;});
# 1090
({void*_TmpB=Cyc_Absyn_join_eff(inst_effect);*t=_TmpB;});
fresh=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));_TmpB->hd=(struct _tuple14*)inst->hd,_TmpB->tl=fresh;_TmpB;});
goto _LL10;}else{_TmpA=_Tmp9;{void*exn=_TmpA;_rethrow(exn);}}_LL10:;}}}
# 1094
goto _LLB;}_LLB:;
# 1096
goto _LL3;}}
# 1098
goto _LL3;}}_LL3:;}}
# 1101
return fresh;}}
# 1104
static int Cyc_Tcexp_inst_with_joineff(struct Cyc_List_List*orig,struct Cyc_Hashtable_Table*inst_effects){
# 1106
int found=0;
for(1;orig!=0;orig=orig->tl){
struct _tuple14*_Tmp0=(struct _tuple14*)orig->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp2;void*origrgn=_Tmp1;
struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){
{struct Cyc_List_List*l=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*)=(struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*))Cyc_Hashtable_lookup;_Tmp5;})(inst_effects,tv);
found=1;
if(!({void*_Tmp5=origrgn;Cyc_Unify_unify(_Tmp5,Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(l)));})){
int _Tmp5=0;_npop_handler(0);return _Tmp5;}}
# 1110
;_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp5)->tag==Cyc_Core_Not_found)
# 1116
goto _LL3;else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}}_LL3:;}}}}
# 1119
return found;}struct _tuple20{struct Cyc_Absyn_Vardecl*f0;struct Cyc_Absyn_Exp*f1;};
# 1122
static int Cyc_Tcexp_substitute_vars_exp(struct Cyc_List_List*subst,struct Cyc_Absyn_Exp*e){
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1)switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 3: _Tmp1=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
_Tmp1=vd;goto _LL4;}case 4: _Tmp1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_Tmp1;
_Tmp1=vd;goto _LL6;}case 5: _Tmp1=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1127
{struct Cyc_List_List*s=subst;for(0;s!=0;s=s->tl){
struct _tuple20 _Tmp2=*((struct _tuple20*)s->hd);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_Absyn_Vardecl*vdopt=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
if(vdopt==0)continue;
if(Cyc_Absyn_qvar_cmp(vdopt->name,vd->name)==0){
e->r=e2->r;
return 0;}}}}
# 1135
goto _LL0;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;}
# 1138
return 1;}
# 1141
static int Cyc_Tcexp_substitute_vars_stmt(struct Cyc_List_List*subst,struct Cyc_Absyn_Stmt*e){
return 1;}struct _tuple21{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 1147
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*call,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1155
struct Cyc_List_List*es=args;
int arg_cnt=0;
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_Tmp0;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress(_check_null(e->topt));
# 1164
void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;if(*((int*)t)==4){_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.eff;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.nullable;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;{void*t1=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*rgn=_Tmp4;void*x=_Tmp3;void*b=_Tmp2;void*zt=_Tmp1;
# 1169
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rgn);
# 1171
Cyc_Tcutil_check_nonzero_bound(loc,b);
# 1174
t1=({void*_Tmp7=Cyc_Absyn_compress(t1);Cyc_CurRgn_instantiate(_Tmp7,Cyc_Tcenv_curr_lifo_rgn(te));});{
void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;void*_Tmp11;void*_Tmp12;void*_Tmp13;void*_Tmp14;int _Tmp15;void*_Tmp16;void*_Tmp17;struct Cyc_Absyn_Tqual _Tmp18;void*_Tmp19;void*_Tmp1A;if(*((int*)t1)==6){_Tmp1A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.tvars;_Tmp19=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.effect;_Tmp18=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.ret_tqual;_Tmp17=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.ret_type;_Tmp16=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.args;_Tmp15=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.c_varargs;_Tmp14=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.cyc_varargs;_Tmp13=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.qual_bnd;_Tmp12=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.attributes;_Tmp11=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.checks_clause;_Tmp10=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.checks_assn;_TmpF=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.requires_clause;_TmpE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.requires_assn;_TmpD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.ensures_clause;_TmpC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.ensures_assn;_TmpB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.throws_clause;_TmpA=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.throws_assn;_Tmp9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.return_value;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.arg_vardecls;_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.effconstr;{struct Cyc_List_List*tvars=_Tmp1A;void*eff=_Tmp19;struct Cyc_Absyn_Tqual res_tq=_Tmp18;void*res_typ=_Tmp17;struct Cyc_List_List*orig_args_info=_Tmp16;int c_vararg=_Tmp15;struct Cyc_Absyn_VarargInfo*cyc_vararg=_Tmp14;struct Cyc_List_List*qb=_Tmp13;struct Cyc_List_List*atts=_Tmp12;struct Cyc_Absyn_Exp*chk=_Tmp11;struct Cyc_AssnDef_ExistAssnFn*chk_relns=_Tmp10;struct Cyc_Absyn_Exp*req=_TmpF;struct Cyc_AssnDef_ExistAssnFn*req_relns=_TmpE;struct Cyc_Absyn_Exp*ens=_TmpD;struct Cyc_AssnDef_ExistAssnFn*ens_relns=_TmpC;struct Cyc_Absyn_Exp*thrws=_TmpB;struct Cyc_AssnDef_ExistAssnFn*thrws_relns=_TmpA;struct Cyc_Absyn_Vardecl*ret_var=_Tmp9;struct Cyc_List_List*arg_vardecls=_Tmp8;struct Cyc_List_List*effc=_Tmp7;
# 1178
if(tvars!=0 || effc!=0)
Cyc_Warn_err(e->loc,_tag_fat("function should have been instantiated prior to use -- probably a compiler bug",sizeof(char),79U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*args_info=orig_args_info;
# 1183
if(topt!=0)Cyc_Unify_unify(res_typ,*topt);
# 1185
while(es!=0 && args_info!=0){
# 1187
{int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f2;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_List_List*_Tmp1B=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp1C=e1;Cyc_Tcutil_coerce_arg(_Tmp1B,_Tmp1C,t2,& alias_coercion);})){
struct _fat_ptr s0=_tag_fat("actual argument has type ",sizeof(char),26U);
struct _fat_ptr s1=Cyc_Absynpp_typ2string(_check_null(e1->topt));
struct _fat_ptr s2=_tag_fat(" but formal has type ",sizeof(char),22U);
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _Tmp1B=({unsigned long _Tmp1C=({unsigned long _Tmp1D=Cyc_strlen(s0);_Tmp1D + Cyc_strlen(s1);});_Tmp1C + Cyc_strlen(s2);});_Tmp1B + Cyc_strlen(s3);})>= 70U)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=s0;_Tmp1C;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=_tag_fat("\n\t",sizeof(char),3U);_Tmp1D;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=s1;_Tmp1E;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1F;_Tmp1F.tag=0,_Tmp1F.f1=_tag_fat("\n",sizeof(char),2U);_Tmp1F;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1F=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp20;_Tmp20.tag=0,_Tmp20.f1=s2;_Tmp20;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp21;_Tmp21.tag=0,_Tmp21.f1=_tag_fat("\n\t",sizeof(char),3U);_Tmp21;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp22;_Tmp22.tag=0,_Tmp22.f1=s3;_Tmp22;});void*_Tmp22[7];_Tmp22[0]=& _Tmp1B,_Tmp22[1]=& _Tmp1C,_Tmp22[2]=& _Tmp1D,_Tmp22[3]=& _Tmp1E,_Tmp22[4]=& _Tmp1F,_Tmp22[5]=& _Tmp20,_Tmp22[6]=& _Tmp21;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp22,sizeof(void*),7));});else{
# 1199
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=s0;_Tmp1C;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=s1;_Tmp1D;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=s2;_Tmp1E;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1F;_Tmp1F.tag=0,_Tmp1F.f1=s3;_Tmp1F;});void*_Tmp1F[4];_Tmp1F[0]=& _Tmp1B,_Tmp1F[1]=& _Tmp1C,_Tmp1F[2]=& _Tmp1D,_Tmp1F[3]=& _Tmp1E;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp1F,sizeof(void*),4));});}
Cyc_Unify_unify(_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 1204
if(alias_coercion)
({struct Cyc_List_List*_Tmp1B=({struct Cyc_List_List*_Tmp1C=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1C->hd=(void*)arg_cnt,_Tmp1C->tl=*alias_arg_exps;_Tmp1C;});*alias_arg_exps=_Tmp1B;});
({unsigned _Tmp1B=e1->loc;struct Cyc_List_List*_Tmp1C=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp1D=t2;Cyc_Tcexp_check_consume(_Tmp1B,_Tmp1C,_Tmp1D,e1);});
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}
# 1187
1U;}{
# 1214
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a!=0;a=a->tl){
void*_Tmp1B=(void*)a->hd;int _Tmp1C;int _Tmp1D;enum Cyc_Absyn_Format_Type _Tmp1E;if(*((int*)_Tmp1B)==19){_Tmp1E=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp1B)->f1;_Tmp1D=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp1B)->f2;_Tmp1C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp1B)->f3;{enum Cyc_Absyn_Format_Type ft=_Tmp1E;int fmt_arg_pos=_Tmp1D;int arg_start_pos=_Tmp1C;
# 1218
{struct _handler_cons _Tmp1F;_push_handler(& _Tmp1F);{int _Tmp20=0;if(setjmp(_Tmp1F.handler))_Tmp20=1;if(!_Tmp20){
# 1220
{struct Cyc_Absyn_Exp*fmt_arg=({struct Cyc_Absyn_Exp*(*_Tmp21)(struct Cyc_List_List*,int)=(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;_Tmp21;})(args,fmt_arg_pos - 1);
# 1222
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos==0)
fmt_args=0;else{
# 1226
fmt_args=({struct Cyc_Core_Opt*_Tmp21=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp22=Cyc_List_nth_tail(args,arg_start_pos - 1);_Tmp21->v=_Tmp22;});_Tmp21;});}
args_already_checked=1;
switch((int)ft){case Cyc_Absyn_Printf_ft:
# 1230
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 1233
goto _LLF;case Cyc_Absyn_Scanf_ft:
 goto _LL15;default: _LL15:
# 1236
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 1239
goto _LLF;}_LLF:;}
# 1220
;_pop_handler();}else{void*_Tmp21=(void*)Cyc_Core_get_exn_thrown();void*_Tmp22;if(((struct Cyc_List_Nth_exn_struct*)_Tmp21)->tag==Cyc_List_Nth){
# 1241
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp23=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp24;_Tmp24.tag=0,_Tmp24.f1=_tag_fat("bad format arguments",sizeof(char),21U);_Tmp24;});void*_Tmp24[1];_Tmp24[0]=& _Tmp23;Cyc_Warn_err2(loc,_tag_fat(_Tmp24,sizeof(void*),1));});goto _LL16;}else{_Tmp22=_Tmp21;{void*exn=_Tmp22;_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{
goto _LLA;}_LLA:;}}
# 1246
if(args_info!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=_tag_fat("too few arguments for function",sizeof(char),31U);_Tmp1C;});void*_Tmp1C[1];_Tmp1C[0]=& _Tmp1B;Cyc_Warn_err2(loc,_tag_fat(_Tmp1C,sizeof(void*),1));});else{
# 1249
if((es!=0 || c_vararg)|| cyc_vararg!=0){
if(c_vararg)
for(1;es!=0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=_tag_fat("too many arguments for function",sizeof(char),32U);_Tmp1C;});void*_Tmp1C[1];_Tmp1C[0]=& _Tmp1B;Cyc_Warn_err2(loc,_tag_fat(_Tmp1C,sizeof(void*),1));});else{
# 1256
struct Cyc_Absyn_VarargInfo _Tmp1B=*cyc_vararg;int _Tmp1C;void*_Tmp1D;_Tmp1D=_Tmp1B.type;_Tmp1C=_Tmp1B.inject;{void*vt=_Tmp1D;int inject=_Tmp1C;
struct Cyc_Absyn_VarargCallInfo*vci;vci=_cycalloc(sizeof(struct Cyc_Absyn_VarargCallInfo)),vci->num_varargs=0,vci->injectors=0,vci->vai=cyc_vararg;
# 1260
*vararg_call_info=vci;
# 1262
if(!inject)
# 1264
for(1;es!=0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_List_List*_Tmp1E=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp1F=e1;Cyc_Tcutil_coerce_arg(_Tmp1E,_Tmp1F,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1F;_Tmp1F.tag=0,_Tmp1F.f1=_tag_fat("vararg requires type ",sizeof(char),22U);_Tmp1F;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1F=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp20;_Tmp20.tag=2,_Tmp20.f1=(void*)vt;_Tmp20;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp20=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp21;_Tmp21.tag=0,_Tmp21.f1=
_tag_fat(" but argument has type ",sizeof(char),24U);_Tmp21;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp21=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp22;_Tmp22.tag=3,_Tmp22.f1=(void*)e1->topt;_Tmp22;});void*_Tmp22[4];_Tmp22[0]=& _Tmp1E,_Tmp22[1]=& _Tmp1F,_Tmp22[2]=& _Tmp20,_Tmp22[3]=& _Tmp21;Cyc_Tcexp_err_and_explain(loc,_tag_fat(_Tmp22,sizeof(void*),4));});
if(alias_coercion)
({struct Cyc_List_List*_Tmp1E=({struct Cyc_List_List*_Tmp1F=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1F->hd=(void*)arg_cnt,_Tmp1F->tl=*alias_arg_exps;_Tmp1F;});*alias_arg_exps=_Tmp1E;});
({unsigned _Tmp1E=e1->loc;struct Cyc_List_List*_Tmp1F=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp20=vt;Cyc_Tcexp_check_consume(_Tmp1E,_Tmp1F,_Tmp20,e1);});}else{
# 1279
void*_Tmp1E=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_Tmp1F;void*_Tmp20;if(*((int*)_Tmp1E)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1E)->f1)==22){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1E)->f1)->f1.KnownDatatype.tag==2){_Tmp20=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1E)->f1)->f1.KnownDatatype.val;_Tmp1F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1E)->f2;{struct Cyc_Absyn_Datatypedecl*td=_Tmp20;struct Cyc_List_List*targs=_Tmp1F;
# 1283
struct Cyc_Absyn_Datatypedecl*_Tmp21=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_Tmp21;
struct Cyc_List_List*fields=0;
if(td->fields==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp22=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp23;_Tmp23.tag=0,_Tmp23.f1=_tag_fat("can't inject into abstract ",sizeof(char),28U);_Tmp23;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp23=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp24;_Tmp24.tag=2,_Tmp24.f1=(void*)vt;_Tmp24;});void*_Tmp24[2];_Tmp24[0]=& _Tmp22,_Tmp24[1]=& _Tmp23;Cyc_Warn_err2(loc,_tag_fat(_Tmp24,sizeof(void*),2));});else{
fields=(struct Cyc_List_List*)td->fields->v;}{
# 1295
struct Cyc_List_List*inst=Cyc_List_zip(td->tvs,targs);
struct Cyc_List_List*orig=Cyc_List_zip(td->tvs,targs);
struct Cyc_Hashtable_Table*inst_effects=
({struct Cyc_Hashtable_Table*(*_Tmp22)(int,int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),int(*)(struct Cyc_Absyn_Tvar*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),int(*)(struct Cyc_Absyn_Tvar*)))Cyc_Hashtable_create;_Tmp22;})(10,Cyc_Absyn_tvar_cmp,Cyc_Absyn_tvar_id);
int empty_inst_effects=1;
struct Cyc_List_List*espos=es;
for(1;es!=0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1305
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
({unsigned _Tmp22=e1->loc;struct Cyc_List_List*_Tmp23=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp24=_check_null(e1->topt);Cyc_Tcexp_check_consume(_Tmp22,_Tmp23,_Tmp24,e1);});}{
# 1309
struct Cyc_List_List*fresh=Cyc_Tcexp_fresh_effect_evars(te,inst,orig);
struct Cyc_Absyn_Datatypefield*f=({struct Cyc_Tcenv_Tenv*_Tmp22=te;struct Cyc_Absyn_Exp*_Tmp23=e1;void*_Tmp24=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_Tmp25=inst;Cyc_Tcexp_tcInjection(_Tmp22,_Tmp23,_Tmp24,_Tmp25,fields);});
if(f!=0)
({struct Cyc_List_List*_Tmp22=({
struct Cyc_List_List*_Tmp23=vci->injectors;Cyc_List_append(_Tmp23,({struct Cyc_List_List*_Tmp24=_cycalloc(sizeof(struct Cyc_List_List));_Tmp24->hd=f,_Tmp24->tl=0;_Tmp24;}));});
# 1312
vci->injectors=_Tmp22;});
# 1314
for(1;fresh!=0;fresh=fresh->tl){
struct _tuple14*_Tmp22=(struct _tuple14*)fresh->hd;void*_Tmp23;void*_Tmp24;_Tmp24=_Tmp22->f0;_Tmp23=_Tmp22->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp24;void*ev=_Tmp23;
void*cf=Cyc_Absyn_compress(ev);
if(*((int*)cf)==1)
goto _LL26;else{
# 1320
struct Cyc_List_List*l=0;
{struct _handler_cons _Tmp25;_push_handler(& _Tmp25);{int _Tmp26=0;if(setjmp(_Tmp25.handler))_Tmp26=1;if(!_Tmp26){
l=({struct Cyc_List_List*(*_Tmp27)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*)=(struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*))Cyc_Hashtable_lookup;_Tmp27;})(inst_effects,tv);;_pop_handler();}else{void*_Tmp27=(void*)Cyc_Core_get_exn_thrown();void*_Tmp28;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp27)->tag==Cyc_Core_Not_found){
# 1324
l=0;goto _LL2B;}else{_Tmp28=_Tmp27;{void*exn=_Tmp28;_rethrow(exn);}}_LL2B:;}}}
# 1326
({void(*_Tmp25)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*,struct Cyc_List_List*)=({void(*_Tmp26)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*,struct Cyc_List_List*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Tvar*,struct Cyc_List_List*))Cyc_Hashtable_insert;_Tmp26;});struct Cyc_Hashtable_Table*_Tmp26=inst_effects;struct Cyc_Absyn_Tvar*_Tmp27=tv;_Tmp25(_Tmp26,_Tmp27,({struct Cyc_List_List*_Tmp28=_cycalloc(sizeof(struct Cyc_List_List));_Tmp28->hd=cf,_Tmp28->tl=l;_Tmp28;}));});
empty_inst_effects=0;
goto _LL26;}_LL26:;}}}}}
# 1332
if(empty_inst_effects)
goto _LL1E;
if(Cyc_Tcexp_inst_with_joineff(orig,inst_effects)){
# 1337
struct Cyc_List_List*es=espos;struct Cyc_List_List*prevInjectors=vci->injectors;
if(({int _Tmp22=Cyc_List_length(prevInjectors);_Tmp22!=Cyc_List_length(es);}))
goto _LL22;
for(1;(unsigned)es &&(unsigned)prevInjectors;(es=es->tl,prevInjectors=prevInjectors->tl)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
struct Cyc_Absyn_Datatypefield*f=({struct Cyc_Tcenv_Tenv*_Tmp22=te;struct Cyc_Absyn_Exp*_Tmp23=e1;void*_Tmp24=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_Tmp25=orig;Cyc_Tcexp_tcInjection(_Tmp22,_Tmp23,_Tmp24,_Tmp25,fields);});
if(f==0 || f!=(struct Cyc_Absyn_Datatypefield*)prevInjectors->hd)
goto _LL22;}}else{
# 1348
goto _LL22;}
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp21=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp22;_Tmp22.tag=0,_Tmp22.f1=_tag_fat("bad inject vararg type",sizeof(char),23U);_Tmp22;});void*_Tmp22[1];_Tmp22[0]=& _Tmp21;Cyc_Warn_err2(loc,_tag_fat(_Tmp22,sizeof(void*),1));});goto _LL1E;}_LL1E:;}}}}}}
# 1357
Cyc_Tcexp_check_all_aqual_bounds(loc,te,qb);
if(*alias_arg_exps==0)
# 1367 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,_check_null(eff));
# 1388 "tcexp.cyc"
if(chk!=0){
struct Cyc_List_List*vds=0;
struct Cyc_List_List*ts=0;
struct Cyc_List_List*new_args=0;
# 1393
int i=0;
{struct Cyc_List_List*ainfo=orig_args_info;for(0;ainfo!=0;(ainfo=ainfo->tl,i ++)){
struct _tuple9*_Tmp1B=(struct _tuple9*)ainfo->hd;void*_Tmp1C;struct Cyc_Absyn_Tqual _Tmp1D;_Tmp1D=_Tmp1B->f1;_Tmp1C=_Tmp1B->f2;{struct Cyc_Absyn_Tqual tq=_Tmp1D;void*t=_Tmp1C;
ts=({struct Cyc_List_List*_Tmp1E=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp1F=({struct _tuple19*_Tmp20=_cycalloc(sizeof(struct _tuple19));_Tmp20->f0=tq,_Tmp20->f1=t;_Tmp20;});_Tmp1E->hd=_Tmp1F;}),_Tmp1E->tl=ts;_Tmp1E;});{
struct _fat_ptr*ux;ux=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp1E=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp1F=({struct Cyc_Int_pa_PrintArg_struct _Tmp20;_Tmp20.tag=1,_Tmp20.f1=(unsigned long)i;_Tmp20;});void*_Tmp20[1];_Tmp20[0]=& _Tmp1F;Cyc_aprintf(_tag_fat("arg%d",sizeof(char),6U),_tag_fat(_Tmp20,sizeof(void*),1));});*ux=_Tmp1E;});{
struct _tuple1*x;x=_cycalloc(sizeof(struct _tuple1)),x->f0=Cyc_Absyn_Loc_n,x->f1=ux;{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(loc,x,t,0,0);
vds=({struct Cyc_List_List*_Tmp1E=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1E->hd=vd,_Tmp1E->tl=vds;_Tmp1E;});{
struct Cyc_Absyn_Exp*new_arg=({void*_Tmp1E=(void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_Tmp1F=_cycalloc(sizeof(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct));_Tmp1F->tag=5,_Tmp1F->f1=vd;_Tmp1F;});Cyc_Absyn_varb_exp(_Tmp1E,loc);});
new_arg->topt=t;
new_args=({struct Cyc_List_List*_Tmp1E=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1E->hd=new_arg,_Tmp1E->tl=new_args;_Tmp1E;});}}}}}}}
# 1405
new_args=Cyc_List_imp_rev(new_args);
ts=Cyc_List_imp_rev(ts);
vds=Cyc_List_imp_rev(vds);{
# 1409
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*vs=vds;for(0;vs!=0;vs=vs->tl){
struct Cyc_Absyn_Pat*p=({void*_Tmp1B=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp1C=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp1C->tag=1,_Tmp1C->f1=(struct Cyc_Absyn_Vardecl*)vs->hd,({struct Cyc_Absyn_Pat*_Tmp1D=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,loc);_Tmp1C->f2=_Tmp1D;});_Tmp1C;});Cyc_Absyn_new_pat(_Tmp1B,loc);});
p->topt=((struct Cyc_Absyn_Vardecl*)vs->hd)->type;
ps=({struct Cyc_List_List*_Tmp1B=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple21*_Tmp1C=({struct _tuple21*_Tmp1D=_cycalloc(sizeof(struct _tuple21));_Tmp1D->f0=0,_Tmp1D->f1=p;_Tmp1D;});_Tmp1B->hd=_Tmp1C;}),_Tmp1B->tl=ps;_Tmp1B;});}}
# 1415
ps=Cyc_List_imp_rev(ps);{
void*tup_type=Cyc_Absyn_tuple_type(ts);
struct Cyc_Absyn_Pat*tup_pat=({void*_Tmp1B=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp1C=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp1C->tag=6,_Tmp1C->f1=tup_type,_Tmp1C->f2=1,_Tmp1C->f3=0,_Tmp1C->f4=ps,_Tmp1C->f5=0;_Tmp1C;});Cyc_Absyn_new_pat(_Tmp1B,loc);});
tup_pat->topt=tup_type;{
struct Cyc_Absyn_Exp*tup_exp=Cyc_Absyn_tuple_exp(args,loc);
tup_exp->topt=tup_type;{
struct Cyc_Absyn_Exp*subst_chk=Cyc_Tcutil_deep_copy_exp(1,chk);
# 1423
struct Cyc_List_List*subst=0;
{struct Cyc_List_List*es=new_args;for(0;arg_vardecls!=0;(
arg_vardecls=arg_vardecls->tl,es=es->tl)){
subst=({struct Cyc_List_List*_Tmp1B=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple20*_Tmp1C=({struct _tuple20*_Tmp1D=_cycalloc(sizeof(struct _tuple20));_Tmp1D->f0=(struct Cyc_Absyn_Vardecl*)arg_vardecls->hd,_Tmp1D->f1=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;_Tmp1D;});_Tmp1B->hd=_Tmp1C;}),_Tmp1B->tl=subst;_Tmp1B;});}}
# 1428
({void(*_Tmp1B)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_List_List*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Exp*))Cyc_Absyn_visit_exp;_Tmp1B;})(Cyc_Tcexp_substitute_vars_exp,Cyc_Tcexp_substitute_vars_stmt,subst,subst_chk);{
struct Cyc_Absyn_Exp*asrt=Cyc_Absyn_assert_exp(subst_chk,0,loc);
asrt->topt=Cyc_Absyn_sint_type;{
struct Cyc_Absyn_Exp*fncall=Cyc_Absyn_fncall_exp(e,new_args,loc);
fncall->topt=res_typ;{
struct Cyc_Absyn_Vardecl*tempvd=({unsigned _Tmp1B=loc;struct _tuple1*_Tmp1C=({struct _tuple1*_Tmp1D=_cycalloc(sizeof(struct _tuple1));_Tmp1D->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp1E=({struct _fat_ptr*_Tmp1F=_cycalloc(sizeof(struct _fat_ptr));*_Tmp1F=_tag_fat("temp",sizeof(char),5U);_Tmp1F;});_Tmp1D->f1=_Tmp1E;});_Tmp1D;});void*_Tmp1D=tup_type;Cyc_Absyn_new_vardecl(_Tmp1B,_Tmp1C,_Tmp1D,tup_exp,0);});
# 1435
struct Cyc_Absyn_Exp*tempexp=({void*_Tmp1B=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp1C=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp1C->tag=4,_Tmp1C->f1=tempvd;_Tmp1C;});Cyc_Absyn_varb_exp(_Tmp1B,loc);});
tempexp->topt=tup_type;{
struct Cyc_Tcpat_TcPatResult pat_res=Cyc_Tcpat_tcPat(te,tup_pat,& tup_type,0);
void*dp=0;
Cyc_Tcpat_check_let_pat_exhaustive(loc,te,tup_pat,& dp);{
void*rd=(void*)({struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_Tmp1B=_cycalloc(sizeof(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct));_Tmp1B->tag=2,_Tmp1B->f1=tup_pat,({struct Cyc_Core_Opt*_Tmp1C=({struct Cyc_Core_Opt*_Tmp1D=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1D->v=pat_res.patvars;_Tmp1D;});_Tmp1B->f2=_Tmp1C;}),_Tmp1B->f3=tempexp,_Tmp1B->f4=dp;_Tmp1B;});
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Decl*_Tmp1B=Cyc_Absyn_new_decl(rd,loc);struct Cyc_Absyn_Stmt*_Tmp1C=({
struct Cyc_Absyn_Stmt*_Tmp1D=Cyc_Absyn_exp_stmt(asrt,loc);struct Cyc_Absyn_Stmt*_Tmp1E=
Cyc_Absyn_exp_stmt(fncall,loc);
# 1442
Cyc_Absyn_seq_stmt(_Tmp1D,_Tmp1E,loc);});
# 1441
Cyc_Absyn_decl_stmt(_Tmp1B,_Tmp1C,loc);});
# 1444
s=({struct Cyc_Absyn_Decl*_Tmp1B=({void*_Tmp1C=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_Tmp1D=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_Tmp1D->tag=0,_Tmp1D->f1=tempvd;_Tmp1D;});Cyc_Absyn_new_decl(_Tmp1C,loc);});struct Cyc_Absyn_Stmt*_Tmp1C=s;Cyc_Absyn_decl_stmt(_Tmp1B,_Tmp1C,loc);});
({void*_Tmp1B=(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_Tmp1C=_cycalloc(sizeof(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct));_Tmp1C->tag=36,_Tmp1C->f1=s;_Tmp1C;});call->r=_Tmp1B;});}}}}}}}}}}
# 1447
return res_typ;}}}}else{
# 1449
struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=_tag_fat("expected pointer to function but found ",sizeof(char),40U);_Tmp1C;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=2,_Tmp1D.f1=(void*)t;_Tmp1D;});void*_Tmp1D[2];_Tmp1D[0]=& _Tmp1B,_Tmp1D[1]=& _Tmp1C;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp1D,sizeof(void*),2));};}}}else{
# 1452
struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("expected pointer to function but found ",sizeof(char),40U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp9,sizeof(void*),2));};}}}
# 1456
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*exception_type=Cyc_Absyn_exn_type();
Cyc_Tcexp_tcExp(te,& exception_type,e);
# 1461
if((int)Cyc_Subtype_coercible(loc,_check_null(e->topt),exception_type)==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expected ",sizeof(char),10U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)exception_type;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" but found ",sizeof(char),12U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)e->topt;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
if(topt!=0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1468
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1470
void*t1=Cyc_Absyn_compress(_check_null(e->topt));
{struct Cyc_Absyn_PtrAtts _Tmp0;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;if(*((int*)t1)==4){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_tq;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts;{void*t=_Tmp2;struct Cyc_Absyn_Tqual tq=_Tmp1;struct Cyc_Absyn_PtrAtts atts=_Tmp0;
# 1473
{void*_Tmp3=Cyc_Absyn_compress(t);struct Cyc_Absyn_FnInfo _Tmp4;if(*((int*)_Tmp3)==6){_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp3)->f1;{struct Cyc_Absyn_FnInfo info=_Tmp4;
# 1475
struct Cyc_List_List*tvars=info.tvars;
struct Cyc_List_List*instantiation=0;
# 1478
for(1;ts!=0 && tvars!=0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Kinds_bk);
({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;struct Cyc_List_List*_Tmp7=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_Tmp8=k;Cyc_Tctyp_check_type(_Tmp5,_Tmp6,_Tmp7,_Tmp8,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp6=({struct _tuple14*_Tmp7=_cycalloc(sizeof(struct _tuple14));_Tmp7->f0=(struct Cyc_Absyn_Tvar*)tvars->hd,_Tmp7->f1=(void*)ts->hd;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=instantiation;_Tmp5;});}
# 1484
info.tvars=tvars;
if(ts!=0){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("too many type variables in instantiation",sizeof(char),41U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp6,sizeof(void*),1));}
# 1490
if(tvars==0){
# 1492
({struct Cyc_List_List*_Tmp5=Cyc_Tcutil_rsubst_effconstr(Cyc_Core_heap_region,instantiation,info.effconstr);info.effconstr=_Tmp5;});
Cyc_Tcenv_check_effect_constraints(te,loc,info.effconstr);
info.effconstr=0;}{
# 1496
void*new_fn_typ=({struct Cyc_List_List*_Tmp5=instantiation;Cyc_Tcutil_substitute(_Tmp5,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp6->tag=6,_Tmp6->f1=info;_Tmp6;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp5;_Tmp5.elt_type=new_fn_typ,_Tmp5.elt_tq=tq,_Tmp5.ptr_atts=atts;_Tmp5;}));}}}else{
goto _LL5;}_LL5:;}
# 1500
goto _LL0;}}else{
goto _LL0;}_LL0:;}{
# 1503
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting polymorphic type but found ",sizeof(char),38U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp2,sizeof(void*),2));}}
# 1507
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1509
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1511
({void*_Tmp0=Cyc_Absyn_pointer_expand(_check_null(e->topt),0);e->topt=_Tmp0;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1516
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1518
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_Tmp3=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_ak:& Cyc_Kinds_mk;
# 1518
Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,1,0,t);});
# 1520
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=_check_null(e->topt);
enum Cyc_Absyn_Coercion cc=Cyc_Subtype_coercible(loc,t2,t);
if((int)cc==0){
void*result=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("cannot cast ",sizeof(char),13U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t2;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" to ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_expr_err(te,loc,& t,_tag_fat(_Tmp4,sizeof(void*),4));});
return result;}
# 1528
*_check_null(c)=cc;{
# 1533
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=e->r,({void*_Tmp2=Cyc_Absyn_compress(t);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;if(*((int*)_Tmp0.f0)==33){if(*((int*)_Tmp0.f1)==4){_Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0.f0)->f1.fat_result;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;{int fat_result=_Tmp5;void*nbl=_Tmp4;void*bds=_Tmp3;void*zt=_Tmp2;void*rel=_Tmp1;
# 1536
if(((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl))&& !Cyc_Tcutil_force_type2bool(0,rel)){
if(!Cyc_Tcutil_is_cvar_type(bds)){
struct Cyc_Absyn_Exp*eopt=({void*_Tmp6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp6,bds);});
if(eopt!=0){
if(Cyc_Evexp_eval_const_uint_exp(eopt).f0==1U)
Cyc_Warn_warn(loc,_tag_fat("cast from ? pointer to * pointer will lose size information",sizeof(char),60U),_tag_fat(0U,sizeof(void*),0));}}else{
# 1546
if(!({unsigned _Tmp6=loc;Cyc_BansheeIf_add_constraint(_Tmp6,({void*_Tmp7=_check_null(Cyc_Tcutil_ptrbnd_cvar_equivalent(bds));Cyc_BansheeIf_equality_constraint(_Tmp7,
Cyc_Absyn_fatconst());}));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("Unable to constrain pointer bound",sizeof(char),34U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)t;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),2));});}}
# 1552
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;};}}}
# 1558
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**topt2=0;
struct Cyc_Absyn_Tqual tq2=Cyc_Absyn_empty_tqual(0U);
int nullable=0;
if(topt!=0){
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;if(*((int*)_Tmp0)==4){_Tmp3=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_tq;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;{void**elttype=(void**)_Tmp3;struct Cyc_Absyn_Tqual tq=_Tmp2;void*n=_Tmp1;
# 1565
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 1578
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_Tmp1=topt2;Cyc_Tcexp_tcExpNoInst(_Tmp0,_Tmp1,e);});
# 1580
if(({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer_or_aggr(_Tmp0,_check_null(e->topt));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("cannot take the address of an alias-free path",sizeof(char),46U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 1585
{void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==23){_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1589
({void*_Tmp3=Cyc_Absyn_add_exp(e1,e2,0U)->r;e0->r=_Tmp3;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}}else{
goto _LL5;}_LL5:;}
# 1595
{void*_Tmp0=e->r;switch(*((int*)_Tmp0)){case 21: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3==1)
goto _LLE;else{goto _LLF;}case 22: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3==1){_LLE:
# 1598
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("cannot take the address of a @tagged union member",sizeof(char),50U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LLA;}else{goto _LLF;}default: _LLF:
 goto _LLA;}_LLA:;}{
# 1604
struct _tuple15 _Tmp0=Cyc_Tcutil_addressof_props(e);void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int is_const=_Tmp2;void*rgn=_Tmp1;
# 1609
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _Tmp3=tq.real_const=1;tq.print_const=_Tmp3;});
# 1613
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))(_check_null(e->topt),rgn,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}}}
# 1617
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1620
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,& Cyc_Kinds_mk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("sizeof applied to type ",sizeof(char),24U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" which has unknown size here",sizeof(char),29U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});
if(topt==0)
return Cyc_Absyn_uint_type;{
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*tt=_Tmp1;
# 1630
if(({void*_Tmp2=tt;Cyc_Unify_unify(_Tmp2,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp3->tag=9,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_sizeoftype_exp(t,0U);_Tmp3->f1=_Tmp4;});_Tmp3;}));}))
return Cyc_Absyn_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return Cyc_Absyn_uint_type;};}}
# 1637
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp(*n,*sf->name)==0)
return sf->type;
return 0;}
# 1645
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1647
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,& Cyc_Kinds_mk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*l=fs;
void*t=t_orig;
for(1;l!=0;l=l->tl){
void*n=(void*)l->hd;
void*_Tmp0;_Tmp0=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)n)->f1;{struct _fat_ptr*n=_Tmp0;
# 1655
int bad_type=1;
{void*_Tmp1=Cyc_Absyn_compress(t);void*_Tmp2;switch(*((int*)_Tmp1)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)->f1.KnownAggr.tag==2){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp2;
# 1658
if((*adp)->impl==0)
goto _LL3;
_Tmp2=(*adp)->impl->fields;goto _LL7;}}else{goto _LL8;}}else{goto _LL8;}case 7: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp1)->f3;_LL7: {struct Cyc_List_List*fields=_Tmp2;
# 1662
void*t2=({void*(*_Tmp3)(void*(*)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*,struct Cyc_List_List*)=(void*(*)(void*(*)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*,struct Cyc_List_List*))Cyc_List_find_c;_Tmp3;})(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("no field of struct/union has name %s",sizeof(char),37U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*n;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),2));});else{
# 1666
t=t2;}
bad_type=0;
goto _LL3;}default: _LL8:
 goto _LL3;}_LL3:;}
# 1671
if(bad_type){
if(l==fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" is not a known struct/union type",sizeof(char),34U);_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});else{
# 1675
struct _fat_ptr s=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_typ2string(t_orig);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("(%s)",sizeof(char),5U),_tag_fat(_Tmp2,sizeof(void*),1));});
{struct Cyc_List_List*x=fs;for(0;x!=l;x=x->tl){
void*_Tmp1=(void*)_check_null(x)->hd;void*_Tmp2;_Tmp2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_Tmp1)->f1;{struct _fat_ptr*n=_Tmp2;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*n;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s.%s",sizeof(char),6U),_tag_fat(_Tmp5,sizeof(void*),2));});goto _LLA;}_LLA:;}}
# 1680
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=s;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" == ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" is not a struct/union type",sizeof(char),28U);_Tmp5;});void*_Tmp5[4];_Tmp5[0]=& _Tmp1,_Tmp5[1]=& _Tmp2,_Tmp5[2]=& _Tmp3,_Tmp5[3]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),4));});}}
# 1683
goto _LL0;}_LL0:;}
# 1686
return Cyc_Absyn_uint_type;}}
# 1690
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig_e,struct Cyc_Absyn_Exp*e){
# 1692
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress(_check_null(e->topt));
if(*((int*)t)==1){
# 1697
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
({void*_Tmp0=Cyc_Absyn_new_evar(& Cyc_Kinds_ako,({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);_Tmp1->v=_Tmp2;});_Tmp1;}));pi.elt_type=_Tmp0;});{
void*new_typ=Cyc_Absyn_pointer_type(pi);
Cyc_Unify_unify(t,new_typ);
t=Cyc_Absyn_compress(t);
goto _LL0;}}else{
goto _LL0;}_LL0:;{
# 1705
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)t)==4){_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.eff;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;{void*t2=_Tmp3;void*rt=_Tmp2;void*b=_Tmp1;void*zt=_Tmp0;
# 1707
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Kinds_mk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_Tmp4=Cyc_Absyn_compress(t2);if(*((int*)_Tmp4)==6){
# 1712
if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("unnecessary dereference for function type",sizeof(char),42U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
# 1715
if(orig_e!=0)
orig_e->r=e->r;
return t;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("cannot dereference abstract pointer type",sizeof(char),41U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});};}
# 1720
return t2;}}else{
struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("expecting pointer type but found ",sizeof(char),34U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp6,sizeof(void*),2));};}}}
# 1725
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1729
({int _Tmp0=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_Tmp0;});{
void*_Tmp0=Cyc_Absyn_compress(aggr_type);enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.tag==2){_Tmp3=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.val;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 1732
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_decl_field(ad,f);
if(finfo==0){
struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp5;_Tmp5.tag=6,_Tmp5.f1=ad;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" has no ",sizeof(char),9U);_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=*f;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" member",sizeof(char),8U);_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp8,sizeof(void*),4));}
# 1736
if(_check_null(ad->impl)->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts!=0)
t2=({struct Cyc_List_List*_Tmp4=Cyc_List_zip(ad->tvs,ts);Cyc_Tcutil_substitute(_Tmp4,finfo->type);});{
struct Cyc_Absyn_Kind*t2_kind=Cyc_Tcutil_type_kind(t2);
# 1744
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_Tmp4=Cyc_Absyn_compress(t2);if(*((int*)_Tmp4)==5)
goto _LL7;else{
struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("cannot get member ",sizeof(char),19U);_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=*f;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(" since its type is abstract",sizeof(char),28U);_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp8,sizeof(void*),3));}_LL7:;}
# 1752
if(((((int)ad->kind==1 && !_check_null(ad->impl)->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause==0){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("cannot read union member ",sizeof(char),26U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*f;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=
_tag_fat(" since it is not `bits-only'",sizeof(char),29U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp7,sizeof(void*),3));}
if(_check_null(ad->impl)->exist_vars!=0){
# 1758
if(!({void*_Tmp4=t2;Cyc_Unify_unify(_Tmp4,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("must use pattern-matching to access field ",sizeof(char),43U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*f;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("\n\tdue to existential type variables.",sizeof(char),37U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp7,sizeof(void*),3));}}
# 1761
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp3;
# 1763
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_field(fs,f);
if(finfo==0){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("type ",sizeof(char),6U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)aggr_type;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" has no ",sizeof(char),9U);_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=*f;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat(" member",sizeof(char),8U);_Tmp9;});void*_Tmp9[5];_Tmp9[0]=& _Tmp4,_Tmp9[1]=& _Tmp5,_Tmp9[2]=& _Tmp6,_Tmp9[3]=& _Tmp7,_Tmp9[4]=& _Tmp8;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp9,sizeof(void*),5));}
# 1768
if((((int)k==1 && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause==0){
# 1770
struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("cannot read union member ",sizeof(char),26U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*f;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" since it is not `bits-only'",sizeof(char),29U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp7,sizeof(void*),3));}
return finfo->type;}default: _LL5: {
# 1773
struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("expecting struct or union, found ",sizeof(char),34U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)aggr_type;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp6,sizeof(void*),2));}};}}
# 1778
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1782
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_Tmp0,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,_check_null(e->topt),f,is_tagged,is_read);}
# 1787
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1791
void*t2=({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _Tmp1=loc;Cyc_Tcexp_tcDeref(_Tmp0,_Tmp1,0,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1797
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1799
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,0,e1);});
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,0,e2);});{
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting int subscript, found ",sizeof(char),32U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t2;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;return Cyc_Tcexp_expr_err(te,e2->loc,topt,_tag_fat(_Tmp2,sizeof(void*),2));}{
# 1808
int _Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;switch(*((int*)t1)){case 4: _Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_tq;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.eff;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.bounds;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.zero_term;{void*t=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*rt=_Tmp3;void*b=_Tmp2;void*zt=_Tmp1;
# 1810
if(Cyc_Tcutil_is_cvar_type(b)&& !({
unsigned _Tmp6=loc;Cyc_BansheeIf_add_constraint(_Tmp6,({void*_Tmp7=b;Cyc_BansheeIf_equality_constraint(_Tmp7,Cyc_Absyn_fatconst());}));}))
# 1813
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("Unable to determine bound on pointer type ",sizeof(char),43U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)t1;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),2));});
# 1817
if(Cyc_Tcutil_force_type2bool(0,zt)){
int emit_warning=0;
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if(eopt!=0){
struct Cyc_Absyn_Exp*e3=eopt;
struct _tuple16 _Tmp6=Cyc_Evexp_eval_const_uint_exp(e3);int _Tmp7;unsigned _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{unsigned i=_Tmp8;int known_i=_Tmp7;
if(known_i && i==1U)emit_warning=1;
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _Tmp9=Cyc_Evexp_eval_const_uint_exp(e2);int _TmpA;unsigned _TmpB;_TmpB=_Tmp9.f0;_TmpA=_Tmp9.f1;{unsigned j=_TmpB;int known_j=_TmpA;
if(known_j){
struct _tuple16 _TmpC=Cyc_Evexp_eval_const_uint_exp(e3);int _TmpD;unsigned _TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{unsigned j=_TmpE;int knownj=_TmpD;
if((known_i && j > i)&& i!=1U)
Cyc_Warn_err(loc,_tag_fat("subscript will fail at run-time",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));}}}}}}
# 1833
if(emit_warning)
Cyc_Warn_warn(e2->loc,_tag_fat("subscript on thin, zero-terminated pointer could be expensive.",sizeof(char),63U),_tag_fat(0U,sizeof(void*),0));}else{
# 1837
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _Tmp6=Cyc_Evexp_eval_const_uint_exp(e2);int _Tmp7;unsigned _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{unsigned i=_Tmp8;int known=_Tmp7;
if(known)
# 1842
({unsigned _Tmp9=loc;unsigned _TmpA=i;void*_TmpB=b;Cyc_Tcutil_check_bound(_Tmp9,_TmpA,_TmpB,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1850
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1853
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rt);
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_mk)&& !Cyc_Tcenv_abstract_val_ok(te))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("can't subscript an abstract pointer",sizeof(char),36U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
return t;}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f1==Cyc_Absyn_StructA){_Tmp0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f2;_Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f3;if((int)_Tmp0){int is_tuple=_Tmp0;struct Cyc_List_List*fields=_Tmp5;
# 1858
struct _tuple16 _Tmp6=Cyc_Evexp_eval_const_uint_exp(e2);int _Tmp7;unsigned _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{unsigned i=_Tmp8;int known=_Tmp7;
if(!known){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=
_tag_fat("tuple projection cannot use sizeof or offsetof",sizeof(char),47U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;return Cyc_Tcexp_expr_err(te,loc,0,_tag_fat(_TmpA,sizeof(void*),1));}{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp9=Cyc_Absyn_tuple_field_designator((int)i);void*_TmpA;_TmpA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp9)->f1;{struct _fat_ptr*n=_TmpA;
({void*_TmpB=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_TmpC->tag=21,_TmpC->f1=e1,_TmpC->f2=n,_TmpC->f3=0,({int _TmpD=!Cyc_Tcenv_in_notreadctxt(te);_TmpC->f4=_TmpD;});_TmpC;});orig->r=_TmpB;});{
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_field(fields,n);
if(finfo==0){
struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("index is ",sizeof(char),10U);_TmpC;});struct Cyc_Warn_Int_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Int_Warn_Warg_struct _TmpD;_TmpD.tag=12,_TmpD.f1=(int)i;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat(" but tuple has only ",sizeof(char),21U);_TmpE;});struct Cyc_Warn_Int_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Int_Warn_Warg_struct _TmpF;_TmpF.tag=12,({
int _Tmp10=Cyc_List_length(fields);_TmpF.f1=_Tmp10;});_TmpF;});struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat(" fields",sizeof(char),8U);_Tmp10;});void*_Tmp10[5];_Tmp10[0]=& _TmpB,_Tmp10[1]=& _TmpC,_Tmp10[2]=& _TmpD,_Tmp10[3]=& _TmpE,_Tmp10[4]=& _TmpF;return Cyc_Tcexp_expr_err(te,loc,0,_tag_fat(_Tmp10,sizeof(void*),5));}
# 1869
return finfo->type;}}}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5: {
struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("subscript applied to ",sizeof(char),22U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)t1;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp8,sizeof(void*),2));}};}}}
# 1875
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1880
void*_Tmp0;_Tmp0=targ->f2;{void*t=_Tmp0;
({unsigned _Tmp1=loc;struct Cyc_Tcenv_Tenv*_Tmp2=te;struct Cyc_List_List*_Tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_Tmp4=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_ak:& Cyc_Kinds_mk;
# 1881
Cyc_Tctyp_check_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,1,0,t);});
# 1883
({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,_Tmp2->f2=des;_Tmp2;});orig_exp->r=_Tmp1;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return _check_null(orig_exp->topt);}}
# 1896 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1899
void*res_t2;
int num_es=Cyc_List_length(des);
struct Cyc_List_List*es=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_Absyn_Exp*(*)(struct _tuple18*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple18*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(({struct Cyc_Absyn_Exp*(*_Tmp0)(struct _tuple18*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd;_Tmp0;}),des);
void*res=Cyc_Absyn_new_evar(& Cyc_Kinds_mko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp;sz_rexp=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct)),sz_rexp->tag=0,({union Cyc_Absyn_Cnst _Tmp0=Cyc_Absyn_Int_c(1U,num_es);sz_rexp->f1=_Tmp0;});{
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1907
if(zero_term){
struct Cyc_Absyn_Exp*e=({struct Cyc_Absyn_Exp*(*_Tmp0)(struct Cyc_List_List*,int)=(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;_Tmp0;})(es,num_es - 1);
if(!Cyc_Tcutil_is_zero(e))
Cyc_Warn_err(e->loc,_tag_fat("zero-terminated array doesn't end with zero.",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));}
# 1912
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_Tmp0=res;struct Cyc_Absyn_Tqual _Tmp1=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1913
struct Cyc_Absyn_Exp*_Tmp2=sz_exp;Cyc_Absyn_array_type(_Tmp0,_Tmp1,_Tmp2,
# 1915
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1917
{struct Cyc_List_List*es2=es;for(0;es2!=0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1920
if(!({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp1=res;Cyc_Tcutil_coerce_list(_Tmp0,_Tmp1,es);}))
# 1922
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("elements of array do not all have the same type (",sizeof(char),50U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)res;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat(")",sizeof(char),2U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(((struct Cyc_Absyn_Exp*)_check_null(es)->hd)->loc,_tag_fat(_Tmp3,sizeof(void*),3));});
# 1925
{int offset=0;for(0;des!=0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f0;
if(ds!=0){
void*_Tmp0=(void*)ds->hd;void*_Tmp1;if(*((int*)_Tmp0)==1){
# 1930
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("only array index designators are supported",sizeof(char),43U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),1));});goto _LL0;}else{_Tmp1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 1932
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _Tmp2=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp3;unsigned _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{unsigned i=_Tmp4;int known=_Tmp3;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("index designator cannot use sizeof or offsetof",sizeof(char),47U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp6,sizeof(void*),1));});else{
if(i!=(unsigned)offset)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("expecting index designator ",sizeof(char),28U);_Tmp6;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp7;_Tmp7.tag=12,_Tmp7.f1=offset;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" but found ",sizeof(char),12U);_Tmp8;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp9;_Tmp9.tag=12,_Tmp9.f1=(int)i;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp9,sizeof(void*),4));});}
goto _LL0;}}}}_LL0:;}}}
# 1942
return res_t2;}}struct _tuple22{void**f0;struct Cyc_Absyn_Tqual*f1;void**f2;};
# 1947
static struct _tuple22 Cyc_Tcexp_ptrEltAtts(void**topt){
if(!((unsigned)topt)){
struct _tuple22 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=0,_Tmp0.f2=0;return _Tmp0;}{
void*_Tmp0=Cyc_Absyn_compress(*topt);struct Cyc_Absyn_ArrayInfo _Tmp1;struct Cyc_Absyn_PtrInfo _Tmp2;switch(*((int*)_Tmp0)){case 4: _Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo x=_Tmp2;
# 1952
struct _tuple22 _Tmp3;({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=x.elt_type;_Tmp5;});_Tmp3.f0=_Tmp4;}),({struct Cyc_Absyn_Tqual*_Tmp4=({struct Cyc_Absyn_Tqual*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tqual));*_Tmp5=x.elt_tq;_Tmp5;});_Tmp3.f1=_Tmp4;}),({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=x.ptr_atts.zero_term;_Tmp5;});_Tmp3.f2=_Tmp4;});return _Tmp3;}case 5: _Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_ArrayInfo x=_Tmp1;
struct _tuple22 _Tmp3;({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=x.elt_type;_Tmp5;});_Tmp3.f0=_Tmp4;}),({struct Cyc_Absyn_Tqual*_Tmp4=({struct Cyc_Absyn_Tqual*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tqual));*_Tmp5=x.tq;_Tmp5;});_Tmp3.f1=_Tmp4;}),({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=x.zero_term;_Tmp5;});_Tmp3.f2=_Tmp4;});return _Tmp3;}default:  {
struct _tuple22 _Tmp3;_Tmp3.f0=0,_Tmp3.f1=0,_Tmp3.f2=0;return _Tmp3;}};}}
# 1959
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1962
Cyc_Tcexp_tcExp(te,0,bound);
{void*_Tmp0=Cyc_Absyn_compress(_check_null(bound->topt));void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp1;
# 1967
if((int)Cyc_Tcenv_new_status(te)==2){
struct Cyc_Absyn_Exp*b=({void*_Tmp2=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_Tmp2,Cyc_Absyn_valueof_exp(t,0U),0,1U,0U);});
b->topt=bound->topt;
bound=b;}
# 1972
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1974
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting unsigned int, found ",sizeof(char),31U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)bound->topt;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(bound->loc,_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}
# 1978
if(!vd->tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("comprehension index variable is not declared const!",sizeof(char),52U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
# 1981
if(te->le!=0)
te=Cyc_Tcenv_new_block(loc,te);{
# 1984
struct _tuple22 _Tmp0=Cyc_Tcexp_ptrEltAtts(topt);void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void**topt2=_Tmp3;struct Cyc_Absyn_Tqual*tqopt=_Tmp2;void**ztopt=_Tmp1;
# 1987
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1989
({unsigned _Tmp4=body->loc;struct Cyc_List_List*_Tmp5=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp6=t;Cyc_Tcexp_check_consume(_Tmp4,_Tmp5,_Tmp6,body);});
if(te->le==0){
# 1992
if(!Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bound is not constant",sizeof(char),22U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(bound->loc,_tag_fat(_Tmp5,sizeof(void*),1));});
if(!Cyc_Tcutil_is_const_exp(body))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("body is not constant",sizeof(char),21U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(body->loc,_tag_fat(_Tmp5,sizeof(void*),1));});}
# 1997
if(ztopt!=0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1999
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_uint_exp(1U,0U);e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 2003
({unsigned _Tmp4=body->loc;struct Cyc_List_List*_Tmp5=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp6=_check_null(body->topt);Cyc_Tcexp_check_consume(_Tmp4,_Tmp5,_Tmp6,body);});{
# 2005
void*_Tmp4=t;struct Cyc_Absyn_Tqual _Tmp5=tqopt==0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_Tmp6=bound;return Cyc_Absyn_array_type(_Tmp4,_Tmp5,_Tmp6,
ztopt==0?Cyc_Absyn_false_type:*ztopt,0U);}}}}
# 2010
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 2013
Cyc_Tcexp_tcExp(te,0,bound);
{void*_Tmp0=Cyc_Absyn_compress(_check_null(bound->topt));void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp1;
# 2018
if((int)Cyc_Tcenv_new_status(te)==2){
struct Cyc_Absyn_Exp*b=({void*_Tmp2=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_Tmp2,Cyc_Absyn_valueof_exp(t,0U),0,1U,0U);});
b->topt=bound->topt;
bound=b;}
# 2023
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 2025
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting unsigned int, found ",sizeof(char),31U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)bound->topt;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(bound->loc,_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}{
# 2028
struct _tuple22 _Tmp0=Cyc_Tcexp_ptrEltAtts(topt);void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void**topt2=_Tmp3;struct Cyc_Absyn_Tqual*tqopt=_Tmp2;void**ztopt=_Tmp1;
# 2031
({unsigned _Tmp4=loc;struct Cyc_Tcenv_Tenv*_Tmp5=te;struct Cyc_List_List*_Tmp6=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_Tmp4,_Tmp5,_Tmp6,& Cyc_Kinds_mk,1,1,t);});
if(topt2!=0)
Cyc_Unify_unify(*topt2,t);
# 2035
if(te->le==0 && !Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bound is not constant",sizeof(char),22U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(bound->loc,_tag_fat(_Tmp5,sizeof(void*),1));});
# 2039
if(ztopt!=0 && Cyc_Tcutil_force_type2bool(0,*ztopt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("non-initializing comprehensions do not currently support @zeroterm arrays",sizeof(char),74U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});{
# 2043
void*_Tmp4=t;struct Cyc_Absyn_Tqual _Tmp5=tqopt==0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_Tmp6=bound;return Cyc_Absyn_array_type(_Tmp4,_Tmp5,_Tmp6,
ztopt==0?Cyc_Absyn_false_type:*ztopt,0U);}}}}
# 2047
static int Cyc_Tcexp_ensure_typqual_nothrow(void*t){
void*_Tmp0;switch(*((int*)t)){case 6: _Tmp0=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*i=_Tmp0;
# 2050
return Cyc_List_mem(Cyc_Atts_attribute_cmp,i->attributes,(void*)& Cyc_Atts_No_throw_att_val);}case 4: _Tmp0=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_Tmp0;
# 2054
return Cyc_Tcexp_ensure_typqual_nothrow(pi->elt_type);}default:
# 2056
 return 1;};}
# 2060
static int Cyc_Tcexp_ensure_nothrow_initializer(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==14){_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Tcexp_ensure_nothrow_initializer(e1);}}else{
return Cyc_Tcexp_ensure_typqual_nothrow(_check_null(e->topt));};}struct _tuple23{struct Cyc_Absyn_Aggrfield*f0;struct Cyc_Absyn_Exp*f1;};
# 2077 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 2081
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt!=0){
ad=*ad_opt;
adptr=({struct Cyc_Absyn_Aggrdecl**_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp0=ad;_Tmp0;});}else{
# 2087
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2087
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 2090
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("unbound struct/union name ",sizeof(char),27U);_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=*tn;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),2));});
if(topt!=0)return*topt;else{return Cyc_Absyn_void_type;}}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 2093
*ad_opt=ad;
*tn=ad->name;}
# 2096
if(ad->impl==0){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't construct abstract ",sizeof(char),26U);_Tmp1;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp2;_Tmp2.tag=6,_Tmp2.f1=ad;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;return Cyc_Tcexp_expr_err(te,loc,0,_tag_fat(_Tmp2,sizeof(void*),2));}{
# 2103
struct Cyc_Tcenv_Tenv*te2=(int)Cyc_Tcenv_new_status(te)==1?Cyc_Tcenv_set_new_status(2U,te): te;
# 2109
struct _tuple13 env=({struct _tuple13 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_Tcenv_lookup_type_vars(te2);_Tmp0.f0=_Tmp1;}),_Tmp0.f1=Cyc_Core_heap_region;_Tmp0;});
struct Cyc_List_List*all_inst=({struct Cyc_List_List*(*_Tmp0)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp0;})(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);
struct Cyc_List_List*exist_inst=({struct Cyc_List_List*(*_Tmp0)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp0;})(Cyc_Tcutil_r_make_inst_var,& env,_check_null(ad->impl)->exist_vars);
struct Cyc_List_List*all_typs=({struct Cyc_List_List*(*_Tmp0)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(({void*(*_Tmp0)(struct _tuple14*)=(void*(*)(struct _tuple14*))Cyc_Core_snd;_Tmp0;}),all_inst);
struct Cyc_List_List*exist_typs=({struct Cyc_List_List*(*_Tmp0)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(({void*(*_Tmp0)(struct _tuple14*)=(void*(*)(struct _tuple14*))Cyc_Core_snd;_Tmp0;}),exist_inst);
struct Cyc_List_List*inst=Cyc_List_append(all_inst,exist_inst);
struct Cyc_List_List*qb_inst=0;
{struct Cyc_List_List*qbit=_check_null(ad->impl)->qual_bnd;for(0;qbit!=0;qbit=qbit->tl){
qb_inst=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));({void*_Tmp3=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)qbit->hd)).f0);_Tmp2->f0=_Tmp3;}),({
void*_Tmp3=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)qbit->hd)).f1);_Tmp2->f1=_Tmp3;});_Tmp2;});
# 2117
_Tmp0->hd=_Tmp1;}),_Tmp0->tl=qb_inst;_Tmp0;});}}{
# 2120
void*res_typ;
# 2129 "tcexp.cyc"
if(topt!=0){
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.tag==2){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.val;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Aggrdecl**adptr2=_Tmp2;struct Cyc_List_List*all_typs2=_Tmp1;
# 2132
if(*adptr2!=*adptr)
goto _LL9;
{struct Cyc_List_List*ats=all_typs;for(0;ats!=0 && all_typs2!=0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 2138
res_typ=*topt;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 res_typ=({union Cyc_Absyn_AggrInfo _Tmp3=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_Tmp3,all_typs);});}_LL5:;}else{
# 2143
res_typ=({union Cyc_Absyn_AggrInfo _Tmp0=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_Tmp0,all_typs);});}{
# 2146
struct Cyc_List_List*user_ex_ts=*ts;
struct Cyc_List_List*ex_ts=exist_typs;
while(user_ex_ts!=0 && ex_ts!=0){
# 2150
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te2;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,& Cyc_Kinds_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
if(!Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad instantiation of existential type",sizeof(char),38U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Unify_explain_failure();}
# 2156
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;
# 2150
1U;}
# 2159
if(user_ex_ts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("too many explicit witness types",sizeof(char),32U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 2162
*ts=exist_typs;{
# 2165
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(Cyc_Core_heap_region,loc,args,ad->kind,_check_null(ad->impl)->fields);
for(1;fields!=0;fields=fields->tl){
int bogus=0;
struct _tuple23*_Tmp0=(struct _tuple23*)fields->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_Absyn_Aggrfield*field=_Tmp2;struct Cyc_Absyn_Exp*field_exp=_Tmp1;
void*inst_fieldtyp=Cyc_Tcutil_substitute(inst,field->type);
void*ftyp=({struct Cyc_Tcenv_Tenv*_Tmp3=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_Tmp3,& inst_fieldtyp,field_exp);});
# 2173
if(!({struct Cyc_List_List*_Tmp3=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp4=field_exp;Cyc_Tcutil_coerce_arg(_Tmp3,_Tmp4,inst_fieldtyp,& bogus);})){
Cyc_Unify_unify(ftyp,inst_fieldtyp);
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("field ",sizeof(char),7U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*field->name;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" of ",sizeof(char),5U);_Tmp6;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp7;_Tmp7.tag=6,_Tmp7.f1=ad;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(" expects type ",sizeof(char),15U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)inst_fieldtyp;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat(" != ",sizeof(char),5U);_TmpA;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpB;_TmpB.tag=2,_TmpB.f1=(void*)ftyp;_TmpB;});void*_TmpB[8];_TmpB[0]=& _Tmp3,_TmpB[1]=& _Tmp4,_TmpB[2]=& _Tmp5,_TmpB[3]=& _Tmp6,_TmpB[4]=& _Tmp7,_TmpB[5]=& _Tmp8,_TmpB[6]=& _Tmp9,_TmpB[7]=& _TmpA;Cyc_Tcexp_err_and_explain(field_exp->loc,_tag_fat(_TmpB,sizeof(void*),8));});}{
# 2179
int nothrow_fld=Cyc_List_mem(Cyc_Atts_attribute_cmp,field->attributes,(void*)& Cyc_Atts_No_throw_att_val);
# 2182
if(nothrow_fld && !Cyc_Tcexp_ensure_nothrow_initializer(field_exp))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("field ",sizeof(char),7U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*field->name;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" can only hold a no_throw function",sizeof(char),35U);_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_Warn_err2(field_exp->loc,_tag_fat(_Tmp6,sizeof(void*),3));});}}}{
# 2186
struct Cyc_List_List*constraint_checks=0;
struct Cyc_List_List*effc=Cyc_Tcutil_rsubst_effconstr(Cyc_Core_heap_region,inst,_check_null(ad->impl)->effconstr);
{struct Cyc_List_List*ec=effc;for(0;ec!=0;ec=ec->tl){
void*_Tmp0=(void*)ec->hd;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 2: _Tmp2=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_Tmp0)->f2;{void*e1s=_Tmp2;void*e2s=_Tmp1;
# 2191
void*ev1=({struct Cyc_List_List*_Tmp3=Cyc_Tcenv_curr_effect_constraints(te);Cyc_Tcutil_evaluate_effect(_Tmp3,e1s);});
if(!Cyc_Unify_unify(ev1,e2s)&& !
Cyc_Tcutil_subset_effect(0,ev1,e2s))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("Effect equality constraint not satisfied",sizeof(char),41U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),1));});
# 2196
goto _LLD;}case 0: _Tmp2=(void*)((struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_Tmp0)->f1;{void*e1=_Tmp2;
# 2198
constraint_checks=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=(void*)ec->hd,_Tmp3->tl=constraint_checks;_Tmp3;});
goto _LLD;}default:
# 2201
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Disjoint constraints in aggregates are not supported yet",sizeof(char),57U),_tag_fat(0U,sizeof(void*),0));}_LLD:;}}
# 2204
Cyc_Tcenv_check_effect_constraints(te,loc,constraint_checks);
# 2213 "tcexp.cyc"
Cyc_Tcexp_check_all_aqual_bounds(loc,te2,qb_inst);
return res_typ;}}}}}}
# 2217
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig,void**ts,int is_tuple,struct Cyc_List_List*args){
# 2221
void*struct_type;
if(*ts!=0)struct_type=*ts;else{
if(topt!=0)struct_type=*topt;else{
struct_type=Cyc_Absyn_new_evar(& Cyc_Kinds_mko,Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2226
{void*_Tmp0=Cyc_Absyn_compress(struct_type);enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;union Cyc_Absyn_AggrInfo _Tmp3;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{union Cyc_Absyn_AggrInfo ainfo=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 2229
struct _tuple1*name;
struct Cyc_Absyn_Aggrdecl*adopt=0;
{void*_Tmp4;if(ainfo.UnknownAggr.tag==1){_Tmp4=ainfo.UnknownAggr.val.f1;{struct _tuple1*n=_Tmp4;
name=n;goto _LL9;}}else{_Tmp4=*ainfo.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
name=ad->name;adopt=ad;goto _LL9;}}_LL9:;}
# 2235
({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp5->tag=28,_Tmp5->f1=name,_Tmp5->f2=ts,_Tmp5->f3=args,_Tmp5->f4=adopt;_Tmp5;});orig->r=_Tmp4;});
Cyc_Tcexp_tcExpNoInst(te,topt,orig);
goto _LL0;}}else{goto _LL7;}case 1:  {
# 2241
struct Cyc_List_List*fs=0;
struct Cyc_Core_Opt*tvs=Cyc_Tcenv_lookup_opt_type_vars(te);
int i=0;
{struct Cyc_List_List*a=args;for(0;a!=0;(a=a->tl,++ i)){
struct _tuple18 _Tmp4=*((struct _tuple18*)a->hd);void*_Tmp5;_Tmp5=_Tmp4.f0;{struct Cyc_List_List*ds=_Tmp5;
if(ds==0)({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("tcAnonStruct -- missing designators!",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));{
struct _fat_ptr*n;
{void*_Tmp6=(void*)ds->hd;void*_Tmp7;if(*((int*)_Tmp6)==1){_Tmp7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp6)->f1;{struct _fat_ptr*name=_Tmp7;
n=name;goto _LL11;}}else{
({int(*_Tmp8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp8;})(_tag_fat("tcAnonStruct -- designator is array!",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));}_LL11:;}{
# 2252
struct Cyc_Absyn_Aggrfield*f;f=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),f->name=n,({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);f->tq=_Tmp6;}),({
void*_Tmp6=Cyc_Absyn_new_evar(& Cyc_Kinds_mko,tvs);f->type=_Tmp6;}),f->width=0,f->attributes=0,f->requires_clause=0;
# 2255
fs=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=f,_Tmp6->tl=fs;_Tmp6;});}}}}}
# 2257
fs=Cyc_List_imp_rev(fs);
({void*_Tmp4=struct_type;Cyc_Unify_unify(_Tmp4,(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp5->tag=7,_Tmp5->f1=0U,_Tmp5->f2=is_tuple,_Tmp5->f3=fs;_Tmp5;}));});
*ts=struct_type;
_Tmp1=0U;_Tmp2=fs;goto _LL6;}case 7: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;_LL6: {enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp2;
# 2262
if((int)k==1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("expecting struct but found union",sizeof(char),33U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});{
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(Cyc_Core_heap_region,loc,args,0U,fs);
for(1;fields!=0;fields=fields->tl){
int bogus=0;
struct _tuple23*_Tmp4=(struct _tuple23*)fields->hd;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4->f0;_Tmp5=_Tmp4->f1;{struct Cyc_Absyn_Aggrfield*field=_Tmp6;struct Cyc_Absyn_Exp*field_exp=_Tmp5;
({struct Cyc_Tcenv_Tenv*_Tmp7=Cyc_Tcenv_clear_abstract_val_ok(te);void**_Tmp8=& field->type;Cyc_Tcexp_tcExpInitializer(_Tmp7,_Tmp8,field_exp);});
if(!({struct Cyc_List_List*_Tmp7=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp8=field_exp;Cyc_Tcutil_coerce_arg(_Tmp7,_Tmp8,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("field ",sizeof(char),7U);_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=*field->name;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=
_tag_fat(" of struct expects type ",sizeof(char),25U);_TmpA;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpB;_TmpB.tag=2,_TmpB.f1=(void*)field->type;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" != ",sizeof(char),5U);_TmpC;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpC=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpD;_TmpD.tag=3,_TmpD.f1=(void*)field_exp->topt;_TmpD;});void*_TmpD[6];_TmpD[0]=& _Tmp7,_TmpD[1]=& _Tmp8,_TmpD[2]=& _Tmp9,_TmpD[3]=& _TmpA,_TmpD[4]=& _TmpB,_TmpD[5]=& _TmpC;Cyc_Tcexp_err_and_explain(field_exp->loc,_tag_fat(_TmpD,sizeof(void*),6));});}}
# 2275
goto _LL0;}}default: _LL7:
# 2277
 if((*ts==0 && topt!=0)&&*topt!=0)
# 2279
return Cyc_Tcexp_tcAnonStruct(te,loc,0,orig,ts,is_tuple,args);else{
# 2282
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("tcAnonStruct: wrong type ",sizeof(char),26U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)struct_type;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" for exp ",sizeof(char),10U);_Tmp7;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp8;_Tmp8.tag=4,_Tmp8.f1=orig;_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;({int(*_Tmp9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp9;})(_tag_fat(_Tmp8,sizeof(void*),4));});}}_LL0:;}
# 2284
return struct_type;}
# 2288
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2291
struct _tuple13 env=({struct _tuple13 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_Tcenv_lookup_type_vars(te);_Tmp0.f0=_Tmp1;}),_Tmp0.f1=Cyc_Core_heap_region;_Tmp0;});
struct Cyc_List_List*inst=({struct Cyc_List_List*(*_Tmp0)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp0;})(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);
struct Cyc_List_List*all_typs=({struct Cyc_List_List*(*_Tmp0)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(({void*(*_Tmp0)(struct _tuple14*)=(void*(*)(struct _tuple14*))Cyc_Core_snd;_Tmp0;}),inst);
void*res=({union Cyc_Absyn_DatatypeFieldInfo _Tmp0=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_Tmp0,all_typs);});
# 2296
if(topt!=0){
void*_Tmp0=Cyc_Absyn_compress(*topt);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==23){
Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 2302
struct Cyc_List_List*ts=tuf->typs;
for(1;es!=0 && ts!=0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_substitute(inst,(*((struct _tuple19*)ts->hd)).f1);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp1=e;Cyc_Tcutil_coerce_arg(_Tmp0,_Tmp1,t,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("datatype constructor ",sizeof(char),22U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=tuf->name;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat(" expects argument of type ",sizeof(char),27U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" but this argument has type ",sizeof(char),29U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,
e->topt==0?_Tmp6.f1=(struct _fat_ptr)_tag_fat("?",sizeof(char),2U):({struct _fat_ptr _Tmp7=Cyc_Absynpp_typ2string(e->topt);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp6,sizeof(void*),6));});
Cyc_Unify_explain_failure();}}
# 2315
if(es!=0){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=
_tag_fat("too many arguments for datatype constructor ",sizeof(char),45U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=tuf->name;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;return Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,_tag_fat(_Tmp2,sizeof(void*),2));}
if(ts!=0){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=
_tag_fat("too few arguments for datatype constructor ",sizeof(char),44U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=tuf->name;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp2,sizeof(void*),2));}
return res;}}
# 2325
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 2327
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 2330
if(topt==0)
return 0;{
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;if(*((int*)_Tmp0)==4){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*elt_typ=_Tmp1;
# 2334
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{
return 0;};}}
# 2340
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2342
return Cyc_Absyn_heap_rgn_type;}
# 2348
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,struct Cyc_Absyn_Exp**aqopt,void***t,struct Cyc_Absyn_Exp**e,enum Cyc_Absyn_MallocKind*mknd,int*is_fat){
# 2352
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt!=0){
# 2356
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_eko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,_check_null(*ropt));
void*_Tmp0=Cyc_Absyn_compress(handle_type);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*r=_Tmp1;
# 2361
rgn=r;
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 2365
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting region_t type but found ",sizeof(char),35U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)handle_type;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(_check_null(*ropt)->loc,_tag_fat(_Tmp4,sizeof(void*),2));});
goto _LL0;}_LL0:;}else{
# 2371
if(topt!=0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))
rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{
# 2378
void*aqual_type=Cyc_Absyn_al_qual_type;
if(*aqopt!=0){
void*expected_type=
Cyc_Absyn_aqual_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*qhandle_type=Cyc_Tcexp_tcExp(te,& expected_type,_check_null(*aqopt));
void*_Tmp0=Cyc_Absyn_compress(qhandle_type);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==18){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*aq=_Tmp1;
# 2385
aqual_type=aq;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 2388
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting aqual_t type but found ",sizeof(char),34U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)qhandle_type;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(_check_null(*ropt)->loc,_tag_fat(_Tmp4,sizeof(void*),2));});
goto _LL5;}_LL5:;}else{
# 2392
if(topt!=0){
void*aq=({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_eval_aquals_of(_Tmp0,*topt);});
if(aq!=0){
if(aq!=0){if(*((int*)aq)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)aq)->f1)==16){if(((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)aq)->f1)->f1==Cyc_Absyn_Unique_qual){
# 2397
aqual_type=Cyc_Absyn_un_qual_type;
({struct Cyc_Absyn_Exp*_Tmp0=Cyc_Absyn_uniqueaqual_exp();*aqopt=_Tmp0;});
goto _LLA;}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
 goto _LLA;}_LLA:;}}}
# 2405
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,& Cyc_Absyn_uint_type,*e);});{
# 2414 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if((int)*mknd==1){
if(*t==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("calloc with empty type",sizeof(char),23U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
elt_type=*(*t);
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,& Cyc_Kinds_mk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("calloc cannot be used with type ",sizeof(char),33U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)elt_type;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat("\n\t(type needs initialization)",sizeof(char),30U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});
num_elts=*e;
one_elt=0;}else{
# 2429
void*er=(*e)->r;
retry_sizeof: {
void*_Tmp0;void*_Tmp1;switch(*((int*)er)){case 17: _Tmp1=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)er)->f1;{void*t2=_Tmp1;
# 2433
elt_type=t2;
({void**_Tmp2=({void**_Tmp3=_cycalloc(sizeof(void*));*_Tmp3=elt_type;_Tmp3;});*t=_Tmp2;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LLF;}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f1==Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2)->tl)->tl==0){_Tmp1=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2->hd;_Tmp0=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2->tl->hd;{struct Cyc_Absyn_Exp*e1=_Tmp1;struct Cyc_Absyn_Exp*e2=_Tmp0;
# 2440
{struct _tuple0 _Tmp2=({struct _tuple0 _Tmp3;_Tmp3.f0=e1->r,_Tmp3.f1=e2->r;_Tmp3;});void*_Tmp3;if(*((int*)_Tmp2.f0)==17){_Tmp3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp2.f0)->f1;{void*t1=_Tmp3;
# 2442
e1=e2;
_Tmp3=t1;goto _LL1A;}}else{if(*((int*)_Tmp2.f1)==17){_Tmp3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp2.f1)->f1;_LL1A: {void*t2=_Tmp3;
# 2446
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 2449
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("malloc cannot be used with type ",sizeof(char),33U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t2;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=
_tag_fat("\n\t(type needs initialization)",sizeof(char),30U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),3));});else{
# 2453
*mknd=1U;}}
# 2455
elt_type=t2;
({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=elt_type;_Tmp5;});*t=_Tmp4;});
num_elts=e1;
one_elt=0;
goto _LL16;}}else{
goto No_sizeof;}}_LL16:;}
# 2462
goto _LLF;}}else{goto _LL14;}}else{goto _LL14;}}else{goto _LL14;}}else{goto _LL14;}default: _LL14:
# 2464
 No_sizeof: {
# 2467
struct Cyc_Absyn_Exp*real_e=*e;
{void*_Tmp2=real_e->r;void*_Tmp3;if(*((int*)_Tmp2)==14){_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Exp*e=_Tmp3;
real_e=e;goto _LL1D;}}else{
goto _LL1D;}_LL1D:;}
# 2472
{void*_Tmp2=Cyc_Absyn_compress(_check_null(real_e->topt));void*_Tmp3;if(*((int*)_Tmp2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2!=0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2->hd;{void*tagt=_Tmp3;
# 2474
{void*_Tmp4=Cyc_Absyn_compress(tagt);void*_Tmp5;if(*((int*)_Tmp4)==9){_Tmp5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Exp*vexp=_Tmp5;
er=vexp->r;goto retry_sizeof;}}else{
goto _LL27;}_LL27:;}
# 2478
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25:
 goto _LL22;}_LL22:;}
# 2481
elt_type=Cyc_Absyn_char_type;
({void**_Tmp2=({void**_Tmp3=_cycalloc(sizeof(void*));*_Tmp3=elt_type;_Tmp3;});*t=_Tmp2;});
num_elts=*e;
one_elt=0;}
# 2486
goto _LLF;}_LLF:;}}
# 2490
*is_fat=!one_elt;
# 2493
{void*_Tmp0;if(*((int*)elt_type)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_type)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_type)->f1)->f1.KnownAggr.tag==2){_Tmp0=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_type)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp0;
# 2495
if(ad->impl!=0 && ad->impl->exist_vars!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("malloc with existential types not yet implemented",sizeof(char),50U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F:
 goto _LL2C;}_LL2C:;}{
# 2502
void*(*ptr_maker)(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*)=Cyc_Absyn_at_type;
void*zero_term=Cyc_Absyn_false_type;
if(topt!=0){
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0)==4){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.autoreleased;{void*n=_Tmp4;void*b=_Tmp3;void*zt=_Tmp2;void*rel=_Tmp1;
# 2507
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,rel))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("cannot malloc() an autoreleased pointer",sizeof(char),40U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2514
if(Cyc_Tcutil_force_type2bool(0,zt)&& !((int)*mknd==1)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("converting malloc to calloc to ensure zero-termination",sizeof(char),55U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
*mknd=1U;}
# 2520
if(!Cyc_Tcutil_is_cvar_type(b)){
struct Cyc_Absyn_Exp*eopt=({void*_Tmp5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp5,b);});
if(eopt!=0 && !one_elt){
struct Cyc_Absyn_Exp*upper_exp=eopt;
int is_constant=Cyc_Evexp_c_can_eval(num_elts);
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;{
void*_Tmp5=elt_type;void*_Tmp6=rgn;void*_Tmp7=aqual_type;struct Cyc_Absyn_Tqual _Tmp8=Cyc_Absyn_empty_tqual(0U);void*_Tmp9=b;void*_TmpA=zero_term;return Cyc_Absyn_atb_type(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Absyn_false_type);}}{
# 2529
void*_Tmp5=Cyc_Absyn_compress(_check_null(num_elts->topt));void*_Tmp6;if(*((int*)_Tmp5)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2!=0){_Tmp6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2->hd;{void*tagtyp=_Tmp6;
# 2531
struct Cyc_Absyn_Exp*tagtyp_exp=({void*_Tmp7=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_Tmp7,Cyc_Absyn_valueof_exp(tagtyp,0U),0,1U,0U);});
# 2533
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;{
void*_Tmp7=elt_type;void*_Tmp8=rgn;void*_Tmp9=aqual_type;struct Cyc_Absyn_Tqual _TmpA=Cyc_Absyn_empty_tqual(0U);void*_TmpB=b;void*_TmpC=zero_term;return Cyc_Absyn_atb_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,_TmpB,_TmpC,Cyc_Absyn_false_type);}}
# 2537
goto _LL36;}}else{goto _LL39;}}else{goto _LL39;}}else{_LL39:
 goto _LL36;}_LL36:;}}}else{
# 2542
if((int)*mknd==0 && !Cyc_Tcutil_is_bits_only_type(elt_type)){
# 2545
if(!({unsigned _Tmp5=loc;Cyc_BansheeIf_add_constraint(_Tmp5,({void*_Tmp6=_check_null(Cyc_Tcutil_ptrbnd_cvar_equivalent(b));Cyc_BansheeIf_equality_constraint(_Tmp6,
Cyc_Absyn_thinconst());}));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("Unable to constrain pointer bound",sizeof(char),34U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)*topt;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),2));});}}
# 2550
goto _LL31;}}else{
goto _LL31;}_LL31:;}
# 2553
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;{
void*(*_Tmp0)(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*)=ptr_maker;void*_Tmp1=elt_type;void*_Tmp2=rgn;void*_Tmp3=aqual_type;struct Cyc_Absyn_Tqual _Tmp4=Cyc_Absyn_empty_tqual(0U);void*_Tmp5=zero_term;return _Tmp0(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_false_type);}}}}}
# 2558
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2560
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*t1=_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=_check_null(e1->topt);
void*t2=_check_null(e2->topt);
# 2567
{void*_Tmp0=Cyc_Absyn_compress(t1);if(*((int*)_Tmp0)==5){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("cannot assign to an array",sizeof(char),26U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});goto _LL0;}else{
goto _LL0;}_LL0:;}
# 2572
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("swap not allowed for non-pointer or non-word-sized types",sizeof(char),57U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 2576
if(!Cyc_Absyn_is_lvalue(e1)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("swap non-lvalue",sizeof(char),16U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;return Cyc_Tcexp_expr_err(te,e1->loc,topt,_tag_fat(_Tmp1,sizeof(void*),1));}
if(!Cyc_Absyn_is_lvalue(e2)){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("swap non-lvalue",sizeof(char),16U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;return Cyc_Tcexp_expr_err(te,e2->loc,topt,_tag_fat(_Tmp1,sizeof(void*),1));}
# 2581
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
# 2584
if((int)Cyc_Subtype_coercible_exact(loc,t1,t2)==0){
struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type mismatch: ",sizeof(char),16U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp4,sizeof(void*),4));}
return Cyc_Absyn_void_type;}}}
# 2590
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_Tmp0,s,1);});
# 2593
while(1){
{void*_Tmp0=s->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 2597
void*t=_check_null(e->topt);
if(!({void*_Tmp2=t;Cyc_Unify_unify(_Tmp2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("statement expression returns type ",sizeof(char),35U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Tcexp_err_and_explain(loc,_tag_fat(_Tmp4,sizeof(void*),2));});
return t;}case 2: _Tmp1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s2=_Tmp1;
s=s2;continue;}case 12: _Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp1;
s=s1;continue;}default:  {
struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat("statement expression must end with expression",sizeof(char),46U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;return Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp3,sizeof(void*),1));}};}
# 2594
1U;}}
# 2608
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Absyn_compress(({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,0,e);}));
{void*_Tmp0;if(*((int*)t)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.tag==2){_Tmp0=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp0;
# 2612
if(((int)ad->kind==1 && ad->impl!=0)&& ad->impl->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("expecting @tagged union but found ",sizeof(char),35U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});goto _LL0;}_LL0:;}
# 2617
return Cyc_Absyn_uint_type;}
# 2620
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e,int static_only){
if(static_only)
# 2625
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_allow_valueof(te);struct Cyc_Absyn_Exp*_Tmp1=e;Cyc_Tcexp_tcTest(_Tmp0,_Tmp1,_tag_fat("@assert",sizeof(char),8U));});else{
# 2627
Cyc_Tcexp_tcTest(te,e,_tag_fat("assert",sizeof(char),7U));}
# 2629
return Cyc_Absyn_sint_type;}
# 2633
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp**qual_hndl,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2638
void*rgn=Cyc_Absyn_heap_rgn_type;
te=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(1U,te));
if(*rgn_handle!=0){
# 2643
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_eko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,_check_null(*rgn_handle));
void*_Tmp0=Cyc_Absyn_compress(handle_type);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*r=_Tmp1;
# 2648
rgn=r;
Cyc_Tcenv_check_effect_accessible_nodelay(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 2652
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting region_t type but found ",sizeof(char),35U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)handle_type;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(_check_null(*rgn_handle)->loc,_tag_fat(_Tmp4,sizeof(void*),2));});
# 2654
goto _LL0;}_LL0:;}else{
# 2656
if(topt!=0){
# 2659
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))
rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{
# 2665
void*aqual=Cyc_Absyn_al_qual_type;
if(*qual_hndl!=0){
# 2669
void*expected_type=
Cyc_Absyn_aqual_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*qhndl_type=Cyc_Tcexp_tcExp(te,& expected_type,_check_null(*qual_hndl));
void*_Tmp0=Cyc_Absyn_compress(qhndl_type);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==18){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*aq=_Tmp1;
# 2674
aqual=aq;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 2677
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting aqual_t type but found ",sizeof(char),34U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)qhndl_type;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(_check_null(*qual_hndl)->loc,_tag_fat(_Tmp4,sizeof(void*),2));});
# 2679
goto _LL5;}_LL5:;}else{
# 2682
if(topt!=0){
void*aq=({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_eval_aquals_of(_Tmp0,*topt);});
if(aq!=0){
if(aq!=0){if(*((int*)aq)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)aq)->f1)==16){if(((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)aq)->f1)->f1==Cyc_Absyn_Unique_qual){
# 2687
aqual=Cyc_Absyn_un_qual_type;
({struct Cyc_Absyn_Exp*_Tmp0=Cyc_Absyn_uniqueaqual_exp();*qual_hndl=_Tmp0;});
goto _LLA;}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
 goto _LLA;}_LLA:;}}}{
# 2695
void*_Tmp0=e1->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 26:
 goto _LL13;case 27: _LL13: {
# 2700
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("tcNew: comprehension returned non-array type",sizeof(char),45U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,aqual,1);
if(topt!=0){
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(loc,res_typ,*topt);
if((int)c!=0 &&(int)c!=1){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
res_typ=*topt;}}
# 2712
return res_typ;}case 35: _Tmp2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*nopt=_Tmp2;struct Cyc_List_List*des=_Tmp1;
# 2714
({void*_Tmp3=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp4->tag=25,_Tmp4->f1=des;_Tmp4;});e1->r=_Tmp3;});
_Tmp2=des;goto _LL17;}case 25: _Tmp2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL17: {struct Cyc_List_List*des=_Tmp2;
# 2717
void**elt_typ_opt=0;
int zero_term=0;
if(topt!=0){
void*_Tmp3=Cyc_Absyn_compress(*topt);void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp3)==4){_Tmp5=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.ptr_atts.zero_term;{void**elt_typ=(void**)_Tmp5;void*zt=_Tmp4;
# 2723
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL1E;}}else{
goto _LL1E;}_LL1E:;}{
# 2728
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("tcExpNoPromote on Array_e returned non-array type",sizeof(char),50U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,aqual,0);
if(topt!=0){
# 2737
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(loc,res_typ,*topt);
if((int)c!=0 &&(int)c!=1){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
res_typ=*topt;}}
# 2744
return res_typ;}}case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wstring_c.tag){case 8:  {
# 2749
void*topt2=({void*_Tmp3=Cyc_Absyn_char_type;void*_Tmp4=rgn;void*_Tmp5=aqual;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_const_tqual(0U);void*_Tmp7=Cyc_Absyn_fat_bound_type;void*_Tmp8=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_false_type);});
# 2751
void*t=Cyc_Tcexp_tcExp(te,& topt2,e1);
void*_Tmp3=t;void*_Tmp4=rgn;void*_Tmp5=aqual;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2752
void*_Tmp8=Cyc_Absyn_false_type;return Cyc_Absyn_atb_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_false_type);}case 9:  {
# 2756
void*topt2=({void*_Tmp3=Cyc_Absyn_wchar_type();void*_Tmp4=rgn;void*_Tmp5=aqual;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_const_tqual(0U);void*_Tmp7=Cyc_Absyn_fat_bound_type;void*_Tmp8=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_false_type);});
# 2758
void*t=Cyc_Tcexp_tcExp(te,& topt2,e1);
void*_Tmp3=t;void*_Tmp4=rgn;void*_Tmp5=aqual;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2759
void*_Tmp8=Cyc_Absyn_false_type;return Cyc_Absyn_atb_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_false_type);}default: goto _LL1C;}default: _LL1C: {
# 2766
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt!=0){
void*_Tmp3=Cyc_Absyn_compress(*topt);struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;switch(*((int*)_Tmp3)){case 4: _Tmp5=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_tq;{void**elttype=(void**)_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;
# 2771
topt2=elttype;goto _LL23;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==22){
# 2775
bogus=*topt;
topt2=& bogus;
goto _LL23;}else{goto _LL28;}default: _LL28:
 goto _LL23;}_LL23:;}{
# 2781
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2783
({unsigned _Tmp3=e1->loc;struct Cyc_List_List*_Tmp4=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp5=telt;Cyc_Tcexp_check_consume(_Tmp3,_Tmp4,_Tmp5,e1);});{
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type_with_aqual(te,aqual);
pi.elt_type=telt;
pi.ptr_atts.eff=rgn;
({void*_Tmp3=Cyc_Absyn_bounds_one();pi.ptr_atts.bounds=_Tmp3;});
pi.ptr_atts.zero_term=Cyc_Absyn_false_type;
pi.ptr_atts.autoreleased=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if(topt!=0){
enum Cyc_Absyn_Coercion c=Cyc_Subtype_coercible(loc,res_typ,*topt);
if((int)c!=0 &&(int)c!=1){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,c);
res_typ=*topt;}}
# 2799
return res_typ;}}}}};}}}
# 2805
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_Tmp0=t=({void*_Tmp1=t;void*_Tmp2=Cyc_Tcutil_addressof_props(e).f1;Cyc_Tcutil_promote_array(_Tmp1,_Tmp2,Cyc_Absyn_al_qual_type,0);});e->topt=_Tmp0;});
return t;}
# 2815
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2819
({unsigned _Tmp0=e->loc;struct Cyc_List_List*_Tmp1=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp2=t;Cyc_Tcexp_check_consume(_Tmp0,_Tmp1,_Tmp2,e);});{
void*_Tmp0=e->r;switch(*((int*)_Tmp0)){case 25:
 goto _LL4;case 26: _LL4:
 goto _LL6;case 27: _LL6:
 goto _LL8;case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.tag){case 9: _LL8:
 goto _LLA;case 8: _LLA:
 return t;default: goto _LLB;}default: _LLB:
# 2827
 if(Cyc_Tcutil_is_array_type(t))
({void*_Tmp1=t=({void*_Tmp2=t;void*_Tmp3=Cyc_Tcutil_addressof_props(e).f1;Cyc_Tcutil_promote_array(_Tmp2,_Tmp3,Cyc_Absyn_al_qual_type,0);});e->topt=_Tmp1;});
return t;};}}
# 2840 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*cached_type=_check_null(e->topt);
void*t=({void*_Tmp0=Cyc_Absyn_compress(Cyc_Absyn_pointer_expand(e->topt,0));e->topt=_Tmp0;});
# 2845
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==12){_Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp1;
return t;}}else{
goto _LL0;}_LL0:;}{
# 2850
void*_Tmp0;if(*((int*)t)==4){_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;{void*t2=_Tmp0;
# 2852
void*_Tmp1=Cyc_Absyn_compress(t2);struct Cyc_Absyn_FnInfo _Tmp2;if(*((int*)_Tmp1)==6){_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp1)->f1;{struct Cyc_Absyn_FnInfo info=_Tmp2;
# 2854
struct _tuple13 env=({struct _tuple13 _Tmp3;({struct Cyc_List_List*_Tmp4=Cyc_Tcenv_lookup_type_vars(te);_Tmp3.f0=_Tmp4;}),_Tmp3.f1=Cyc_Core_heap_region;_Tmp3;});
struct Cyc_List_List*inst=({struct Cyc_List_List*(*_Tmp3)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp3;})(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);
# 2858
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp3)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(({void*(*_Tmp3)(struct _tuple14*)=(void*(*)(struct _tuple14*))Cyc_Core_snd;_Tmp3;}),inst);
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);
({void*_Tmp3=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct));_Tmp4->tag=13,_Tmp4->f1=inner,_Tmp4->f2=ts;_Tmp4;});e->r=_Tmp3;});
({void*_Tmp3=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_Tmp3;});
return e->topt;}}else{
return t;};}}else{
# 2865
return t;};}}}
# 2875
static int Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2877
struct Cyc_List_List*decls=0;
# 2879
{void*_Tmp0=fn_exp->r;void*_Tmp1;if(*((int*)_Tmp0)==10){_Tmp1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp1;
# 2881
{void*_Tmp2=e->r;void*_Tmp3;if(*((int*)_Tmp2)==10){_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp2)->f2;{struct Cyc_List_List*es2=_Tmp3;
# 2883
struct Cyc_List_List*arg_exps=alias_arg_exps;
int arg_cnt=0;
while(arg_exps!=0){
while(arg_cnt!=(int)arg_exps->hd){
if(es==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bad count ",sizeof(char),11U);_Tmp5;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp6;_Tmp6.tag=12,_Tmp6.f1=arg_cnt;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("/",sizeof(char),2U);_Tmp7;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp8;_Tmp8.tag=12,_Tmp8.f1=(int)arg_exps->hd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=
_tag_fat(" for alias coercion!",sizeof(char),21U);_Tmp9;});void*_Tmp9[5];_Tmp9[0]=& _Tmp4,_Tmp9[1]=& _Tmp5,_Tmp9[2]=& _Tmp6,_Tmp9[3]=& _Tmp7,_Tmp9[4]=& _Tmp8;({
# 2888
int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpA;})(_tag_fat(_Tmp9,sizeof(void*),5));});
# 2890
++ arg_cnt;
es=es->tl;
es2=_check_null(es2)->tl;
# 2887
1U;}{
# 2895
struct _tuple12 _Tmp4=({struct Cyc_Absyn_Exp*_Tmp5=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;Cyc_Tcutil_insert_alias(_Tmp5,Cyc_Tcutil_copy_type(_check_null(((struct Cyc_Absyn_Exp*)_check_null(es2)->hd)->topt)));});int _Tmp5;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp4.f0;_Tmp6=_Tmp4.f1;_Tmp5=_Tmp4.f2;{struct Cyc_Absyn_Decl*d=_Tmp7;struct Cyc_Absyn_Exp*ve=_Tmp6;int succ=_Tmp5;
if(!succ)
return 0;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=d,_Tmp8->tl=decls;_Tmp8;});
arg_exps=arg_exps->tl;}}
# 2886
1U;}
# 2902
goto _LL5;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("not a function call!",sizeof(char),21U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});}_LL5:;}
# 2905
goto _LL0;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("not a function call!",sizeof(char),21U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});}_LL0:;}
# 2909
while(decls!=0){
{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)decls->hd;
fn_exp=({struct Cyc_Absyn_Stmt*_Tmp0=({struct Cyc_Absyn_Decl*_Tmp1=d;struct Cyc_Absyn_Stmt*_Tmp2=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_Tmp1,_Tmp2,e->loc);});Cyc_Absyn_stmt_exp(_Tmp0,e->loc);});
decls=decls->tl;}
# 2910
1U;}
# 2915
e->topt=0;
e->r=fn_exp->r;
return 1;}
# 2921
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2925
{void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;enum Cyc_Absyn_Incrementor _Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;switch(*((int*)_Tmp0)){case 12: _Tmp8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp8;
# 2930
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=_check_null(e2->topt);
return;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4==0)
# 2934
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("unresolved function: tcExpNoInst",sizeof(char),33U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});else{_Tmp8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_List_List*es=_Tmp7;struct Cyc_Absyn_VarargCallInfo**vci=_Tmp6;
# 2957
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _Tmp9;_push_handler(& _Tmp9);{int _TmpA=0;if(setjmp(_Tmp9.handler))_TmpA=1;if(!_TmpA){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_TmpB=(void*)Cyc_Core_get_exn_thrown();void*_TmpC;if(((struct Cyc_Core_Failure_exn_struct*)_TmpB)->tag==Cyc_Core_Failure){
# 2962
ok=0;
fn_exp=e;
goto _LL59;}else{_TmpC=_TmpB;{void*exn=_TmpC;_rethrow(exn);}}_LL59:;}}}
# 2966
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps!=0 && ok){
alias_arg_exps=Cyc_List_imp_rev(alias_arg_exps);
if(Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps))
Cyc_Tcexp_tcExpNoInst(te,topt,e);}
# 2972
goto _LL0;}}case 35: _Tmp8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*nopt=_Tmp8;struct Cyc_List_List*des=_Tmp7;
# 2938
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0: _Tmp8=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst*c=_Tmp8;
# 2942
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1: _Tmp8=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void**b=_Tmp8;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3: _Tmp8=(enum Cyc_Absyn_Primop*)&((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop*p=_Tmp8;struct Cyc_List_List*es=_Tmp7;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5: _Tmp8=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;enum Cyc_Absyn_Incrementor i=_Tmp5;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4: _Tmp8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Core_Opt*popt=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6: _Tmp8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;struct Cyc_Absyn_Exp*e3=_Tmp6;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7: _Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8: _Tmp8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9: _Tmp8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11: _Tmp8=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
# 2973
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13: _Tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_Tmp8;struct Cyc_List_List*ts=_Tmp7;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14: _Tmp8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*t1=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp7;enum Cyc_Absyn_Coercion*c=_Tmp6;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15: _Tmp8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16: _Tmp8=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp**rgn_handle=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp**qual_hndl=_Tmp6;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,qual_hndl,e,e1);goto _LL0;}case 18: _Tmp8=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
_Tmp8=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17: _Tmp8=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2A: {void*t1=_Tmp8;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19: _Tmp8=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t1=_Tmp8;struct Cyc_List_List*l=_Tmp7;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20: _Tmp8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e,e1);goto _LL0;}case 21: _Tmp8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct _fat_ptr*f=_Tmp7;int*is_tagged=_Tmp6;int*is_read=_Tmp4;
# 2983
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22: _Tmp8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct _fat_ptr*f=_Tmp7;int*is_tagged=_Tmp6;int*is_read=_Tmp4;
# 2985
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23: _Tmp8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e,e1,e2);goto _LL0;}case 24: _Tmp8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple9*t1=_Tmp8;struct Cyc_List_List*des=_Tmp7;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 25: _Tmp8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*des=_Tmp8;
# 2991
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt!=0){
void*_Tmp9=Cyc_Absyn_compress(*topt);void*_TmpA;void*_TmpB;void*_TmpC;if(*((int*)_Tmp9)==5){_TmpC=(void**)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp9)->f1.elt_type;_TmpB=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp9)->f1.tq;_TmpA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp9)->f1.zero_term;{void**et=(void**)_TmpC;struct Cyc_Absyn_Tqual*etq=(struct Cyc_Absyn_Tqual*)_TmpB;void*zt=_TmpA;
# 2997
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5E;}}else{
goto _LL5E;}_LL5E:;}
# 3003
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 36: _Tmp8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp8;
# 3005
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 26: _Tmp8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;int*iszeroterm=_Tmp4;
# 3007
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 27: _Tmp8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp8;void*t1=_Tmp7;int*iszeroterm=_Tmp6;
# 3009
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 28: _Tmp8=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct _tuple1**tn=_Tmp8;struct Cyc_List_List**ts=_Tmp7;struct Cyc_List_List*args=_Tmp6;struct Cyc_Absyn_Aggrdecl**sd_opt=_Tmp4;
# 3011
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 29: _Tmp8=(void**)&((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{void**ts=_Tmp8;int is_tuple=_Tmp3;struct Cyc_List_List*args=_Tmp7;
# 3013
t=Cyc_Tcexp_tcAnonStruct(te,loc,topt,e,ts,is_tuple,args);goto _LL0;}case 30: _Tmp8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp8;struct Cyc_Absyn_Datatypedecl*tud=_Tmp7;struct Cyc_Absyn_Datatypefield*tuf=_Tmp6;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 31: _Tmp8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp8;struct Cyc_Absyn_Enumfield*ef=_Tmp7;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 32: _Tmp8=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t2=_Tmp8;struct Cyc_Absyn_Enumfield*ef=_Tmp7;
t=t2;goto _LL0;}case 33: _Tmp8=(enum Cyc_Absyn_MallocKind*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_Tmp7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp6=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp4=(void***)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp1=(int*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.fat_result;{enum Cyc_Absyn_MallocKind*mknd=_Tmp8;struct Cyc_Absyn_Exp**ropt=_Tmp7;struct Cyc_Absyn_Exp**aqopt=(struct Cyc_Absyn_Exp**)_Tmp6;void***t2=(void***)_Tmp4;struct Cyc_Absyn_Exp**e2=(struct Cyc_Absyn_Exp**)_Tmp2;int*isfat=(int*)_Tmp1;
# 3018
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,aqopt,t2,e2,mknd,isfat);goto _LL0;}case 34: _Tmp8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 37: _Tmp8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp8;struct _fat_ptr*f=_Tmp7;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 40: _Tmp8=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 41: _Tmp8=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp8;int static_only=_Tmp3;
t=Cyc_Tcexp_tcAssert(te,loc,e,static_only);goto _LL0;}case 42: _Tmp8=((struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
t=Cyc_Tcexp_tcAssert(te,loc,e,1);goto _LL0;}case 38: _Tmp8=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t2=_Tmp8;
# 3025
if(!te->allow_valueof)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("valueof(-) can only occur within types",sizeof(char),39U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_err2(e->loc,_tag_fat(_TmpA,sizeof(void*),1));});
# 3033
t=Cyc_Absyn_sint_type;
goto _LL0;}default:
# 3037
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 3039
e->topt=t;}

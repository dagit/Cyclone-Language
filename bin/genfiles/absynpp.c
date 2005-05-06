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
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
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
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);struct _tuple0{void*f0;void*f1;};
# 112
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 258
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 387
extern int Cyc_List_list_prefix(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 139 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 160
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 164
enum Cyc_Absyn_AliasQualVal{Cyc_Absyn_Aliasable_qual =0U,Cyc_Absyn_Unique_qual =1U,Cyc_Absyn_Refcnt_qual =2U,Cyc_Absyn_Restricted_qual =3U};
# 180 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 186
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U,Cyc_Absyn_AqualKind =8U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;void*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;void*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;void*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 418 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;struct Cyc_Absyn_Exp*f9;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 493
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 500
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 518
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 532
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 695 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 878
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 892
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 898
void*Cyc_Absyn_compress(void*);
# 902
int Cyc_Absyn_type2bool(int,void*);
# 911
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);struct _tuple12{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;};
# 1168
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1176
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct _tuple13{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 209 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 81 "kinds.h"
struct _fat_ptr Cyc_Kinds_kind2string(struct Cyc_Absyn_Kind*);
# 89
void*Cyc_Kinds_compress_kb(void*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 83 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Doc;
# 50
extern void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*,int,struct Cyc___cycFILE*);
# 53
extern struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*,int);
# 67 "pp.h"
extern struct Cyc_PP_Doc*Cyc_PP_nil_doc (void);
# 69
extern struct Cyc_PP_Doc*Cyc_PP_blank_doc (void);
# 72
extern struct Cyc_PP_Doc*Cyc_PP_line_doc (void);
# 78
extern struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr);
# 80
extern struct Cyc_PP_Doc*Cyc_PP_textptr(struct _fat_ptr*);
# 83
extern struct Cyc_PP_Doc*Cyc_PP_text_width(struct _fat_ptr,int);
# 91
extern struct Cyc_PP_Doc*Cyc_PP_nest(int,struct Cyc_PP_Doc*);
# 94
extern struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr);
# 108
extern struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr,struct Cyc_List_List*);
# 112
extern struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*)(void*),struct _fat_ptr,struct Cyc_List_List*);
# 117
extern struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr,struct Cyc_List_List*);
# 120
extern struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*)(void*),struct _fat_ptr,struct Cyc_List_List*);
# 123
extern struct Cyc_PP_Doc*Cyc_PP_group(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,struct Cyc_List_List*);
# 129
extern struct Cyc_PP_Doc*Cyc_PP_egroup(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,struct Cyc_List_List*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 51 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 59
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*);
# 65
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 68
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 78
extern struct _fat_ptr Cyc_Absynpp_cyc_string;
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;struct Cyc_List_List*f2;};struct _tuple15{int f0;struct Cyc_List_List*f1;};struct _tuple16{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 37 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple16*);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int,struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int,struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple1*);
# 46
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int,enum Cyc_Absyn_Primop,struct Cyc_List_List*);struct _tuple17{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple17*);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual,void*,struct Cyc_Core_Opt*);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,void*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 57
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*);
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*);
# 65
static int Cyc_Absynpp_expand_typedefs;
# 69
static int Cyc_Absynpp_qvar_to_Cids;static char _TmpG0[4U]="Cyc";
# 71
struct _fat_ptr Cyc_Absynpp_cyc_string={_TmpG0,_TmpG0,_TmpG0 + 4U};
struct _fat_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;static char _TmpG1[14U]="__NoCycPrefix";
# 75
static struct _fat_ptr Cyc_Absynpp_nocyc_str={_TmpG1,_TmpG1,_TmpG1 + 14U};
static struct _fat_ptr*Cyc_Absynpp_nocyc_strptr=& Cyc_Absynpp_nocyc_str;
# 79
static int Cyc_Absynpp_add_cyc_prefix;
# 83
static int Cyc_Absynpp_to_VC;
# 86
static int Cyc_Absynpp_decls_first;
# 90
static int Cyc_Absynpp_rewrite_temp_tvars;
# 93
static int Cyc_Absynpp_print_all_tvars;
# 96
static int Cyc_Absynpp_print_all_kinds;
# 99
static int Cyc_Absynpp_print_all_effects;
# 102
static int Cyc_Absynpp_print_using_stmts;
# 107
static int Cyc_Absynpp_print_externC_stmts;
# 111
static int Cyc_Absynpp_print_full_evars;
# 114
static int Cyc_Absynpp_generate_line_directives;
# 117
static int Cyc_Absynpp_use_curr_namespace;
# 120
static int Cyc_Absynpp_print_zeroterm;
# 123
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 126
int Cyc_Absynpp_print_for_cycdoc=0;
# 147
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 166
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 186
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 206
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 226
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 247
static void Cyc_Absynpp_curr_namespace_add(struct _fat_ptr*v){
Cyc_Absynpp_curr_namespace=({struct Cyc_List_List*_Tmp0=Cyc_Absynpp_curr_namespace;Cyc_List_imp_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=v,_Tmp1->tl=0;_Tmp1;}));});}
# 251
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(_check_null(*l)->tl==0)
*l=0;else{
# 255
Cyc_Absynpp_suppr_last(&(*l)->tl);}}
# 257
static void Cyc_Absynpp_curr_namespace_drop (void){
Cyc_Absynpp_suppr_last(& Cyc_Absynpp_curr_namespace);}
# 261
struct _fat_ptr Cyc_Absynpp_char_escape(char c){
switch((int)c){case 7:
 return _tag_fat("\\a",sizeof(char),3U);case 8:
 return _tag_fat("\\b",sizeof(char),3U);case 12:
 return _tag_fat("\\f",sizeof(char),3U);case 10:
 return _tag_fat("\\n",sizeof(char),3U);case 13:
 return _tag_fat("\\r",sizeof(char),3U);case 9:
 return _tag_fat("\\t",sizeof(char),3U);case 11:
 return _tag_fat("\\v",sizeof(char),3U);case 92:
 return _tag_fat("\\\\",sizeof(char),3U);case 34:
 return _tag_fat("\"",sizeof(char),2U);case 39:
 return _tag_fat("\\'",sizeof(char),3U);default:
# 274
 if((int)c >= 32 &&(int)c <= 126){
struct _fat_ptr t=Cyc_Core_new_string(2U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),0);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
return t;}{
# 279
struct _fat_ptr t=Cyc_Core_new_string(5U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),0);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\\';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(48 + ((int)((unsigned char)c)>> 6 & 3));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),2);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(48 + ((int)c >> 3 & 7));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(t,sizeof(char),3);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(48 + ((int)c & 7));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
return t;}};}
# 288
static int Cyc_Absynpp_special(struct _fat_ptr s){
int sz=(int)(_get_fat_size(s,sizeof(char))- 1U);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((((int)c <= 32 ||(int)c >= 126)||(int)c==34)||(int)c==92)
return 1;}}
# 295
return 0;}
# 298
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 301
int n=(int)(_get_fat_size(s,sizeof(char))- 1U);
# 303
if(n > 0 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),n))==0)-- n;{
# 305
int len=0;
{int i=0;for(0;i <= n;++ i){
char _Tmp0=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _Tmp1;switch((int)_Tmp0){case 7:
 goto _LL4;case 8: _LL4:
 goto _LL6;case 12: _LL6:
 goto _LL8;case 10: _LL8:
 goto _LLA;case 13: _LLA:
 goto _LLC;case 9: _LLC:
 goto _LLE;case 11: _LLE:
 goto _LL10;case 92: _LL10:
 goto _LL12;case 34: _LL12:
 len +=2;goto _LL0;default: _Tmp1=_Tmp0;{char c=_Tmp1;
# 318
if((int)c >= 32 &&(int)c <= 126)++ len;else{
len +=4;}
goto _LL0;}}_LL0:;}}{
# 323
struct _fat_ptr t=Cyc_Core_new_string((unsigned)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _Tmp0=*((const char*)_check_fat_subscript(s,sizeof(char),i));char _Tmp1;switch((int)_Tmp0){case 7:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='a';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 8:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='b';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 12:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='f';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 10:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='n';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 13:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='r';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 9:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='t';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 11:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='v';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 92:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;case 34:
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='"';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});goto _LL15;default: _Tmp1=_Tmp0;{char c=_Tmp1;
# 337
if((int)c >= 32 &&(int)c <= 126)({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=c;if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});else{
# 339
unsigned char uc=(unsigned char)c;
# 342
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='\\';if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)uc >> 6 & 7));if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)uc >> 3 & 7));if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});
({struct _fat_ptr _Tmp2=_fat_ptr_plus(t,sizeof(char),j ++);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)(48 + ((int)uc & 7));if(_get_fat_size(_Tmp2,sizeof(char))==1U &&(_Tmp3==0 && _Tmp4!=0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});}
# 347
goto _LL15;}}_LL15:;}}
# 349
return t;}}}}static char _TmpG2[6U]="const";static char _TmpG3[9U]="volatile";static char _TmpG4[9U]="restrict";
# 352
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
static struct _fat_ptr restrict_string={_TmpG4,_TmpG4,_TmpG4 + 9U};
static struct _fat_ptr volatile_string={_TmpG3,_TmpG3,_TmpG3 + 9U};
static struct _fat_ptr const_string={_TmpG2,_TmpG2,_TmpG2 + 6U};
struct Cyc_List_List*l=0;
if(tq.q_restrict)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=& restrict_string,_Tmp0->tl=l;_Tmp0;});
if(tq.q_volatile)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=& volatile_string,_Tmp0->tl=l;_Tmp0;});
if(tq.print_const)l=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=& const_string,_Tmp0->tl=l;_Tmp0;});
return({struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp1=_tag_fat(" ",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(" ",sizeof(char),2U);Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_PP_textptr,l));});}
# 363
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){return Cyc_Kinds_kind2string(ka);}
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 366
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_Tmp0=Cyc_Kinds_compress_kb(c);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_Absynpp_kind2string(k);}case 1:
# 371
 return Cyc_PP_tex_output?_tag_fat("{?}",sizeof(char),4U): _tag_fat("?",sizeof(char),2U);default: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(k));_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("<=%s",sizeof(char),5U),_tag_fat(_Tmp3,sizeof(void*),1));});}};}
# 376
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_Tmp0=Cyc_Kinds_compress_kb(c);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}case 1:
# 381
 return Cyc_PP_tex_output?Cyc_PP_text(_tag_fat("{?}",sizeof(char),4U)): Cyc_PP_text(_tag_fat("?",sizeof(char),2U));default: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}};}
# 386
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _Tmp0=_tag_fat("<",sizeof(char),2U);struct _fat_ptr _Tmp1=_tag_fat(">",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_typ2doc,ts));});}
# 390
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr n=*tv->name;
# 394
if(Cyc_Absynpp_rewrite_temp_tvars && Cyc_Tcutil_is_temp_tvar(tv)){
struct _fat_ptr kstring=_tag_fat("K",sizeof(char),2U);
{void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
_Tmp1=k;goto _LL4;}case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL4: {struct Cyc_Absyn_Kind*k=_Tmp1;
kstring=Cyc_Absynpp_kind2string(k);goto _LL0;}default:
 goto _LL0;}_LL0:;}
# 401
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)kstring);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(n,sizeof(char),1));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("`G%s%s",sizeof(char),7U),_tag_fat(_Tmp2,sizeof(void*),2));}));}
# 403
return Cyc_PP_text(n);}
# 406
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_BoxKind){_LL4:
 return Cyc_Absynpp_tvar2doc(tv);}else{_Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL8: {struct Cyc_Absyn_Kind*k=_Tmp1;
# 411
return({struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_tvar2doc(tv);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat("::",sizeof(char),3U));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_kind2doc(k);_Tmp2[2]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));});}}default: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Kind*k=_Tmp1;
# 410
_Tmp1=k;goto _LL8;}};}
# 415
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
return({struct _fat_ptr _Tmp0=_tag_fat("<",sizeof(char),2U);struct _fat_ptr _Tmp1=_tag_fat(">",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_ktvar2doc,tvs));});}
# 418
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*)=Cyc_Absynpp_print_all_kinds?Cyc_Absynpp_ktvar2doc: Cyc_Absynpp_tvar2doc;
return({struct _fat_ptr _Tmp0=_tag_fat("<",sizeof(char),2U);struct _fat_ptr _Tmp1=_tag_fat(">",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(f,tvs));});}struct _tuple18{struct Cyc_Absyn_Tqual f0;void*f1;};
# 423
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple18*t){
return Cyc_Absynpp_tqtd2doc((*t).f0,(*t).f1,0);}
# 426
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
return({struct _fat_ptr _Tmp0=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _Tmp1=_tag_fat(")",sizeof(char),2U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple18*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_arg2doc,ts));});}
# 430
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
switch(*((int*)att)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 return Cyc_PP_nil_doc();default:
 return Cyc_PP_text(Cyc_Absynpp_attribute2string(att));};}
# 439
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 1:
 return Cyc_PP_text(_tag_fat(" _stdcall ",sizeof(char),11U));case 2:
 return Cyc_PP_text(_tag_fat(" _cdecl ",sizeof(char),9U));case 3:
 return Cyc_PP_text(_tag_fat(" _fastcall ",sizeof(char),12U));default:
 goto _LL0;}_LL0:;}
# 447
return Cyc_PP_nil_doc();}
# 449
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2!=0;atts2=atts2->tl){
void*_Tmp0=(void*)atts2->hd;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 goto _LL0;default:
 hasatt=1;goto _LL0;}_LL0:;}}
# 458
if(!hasatt)
return Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" __declspec(",sizeof(char),13U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp3=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp4=_tag_fat(" ",sizeof(char),2U);Cyc_PP_group(_Tmp2,_Tmp3,_Tmp4,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_noncallatt2doc,atts));});_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
# 465
struct _fat_ptr Cyc_Absynpp_attribute2string(void*a){
int _Tmp0;struct _fat_ptr _Tmp1;void*_Tmp2;int _Tmp3;switch(*((int*)a)){case 0: _Tmp3=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)a)->f1;{int i=_Tmp3;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("regparm(%d)",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 1:
 return _tag_fat("stdcall",sizeof(char),8U);case 2:
 return _tag_fat("cdecl",sizeof(char),6U);case 3:
 return _tag_fat("fastcall",sizeof(char),9U);case 4:
 return _tag_fat("noreturn",sizeof(char),9U);case 5:
 return _tag_fat("const",sizeof(char),6U);case 6: _Tmp2=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)a)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
# 474
if(e==0)return _tag_fat("aligned",sizeof(char),8U);
switch((int)Cyc_Flags_c_compiler){case Cyc_Flags_Gcc_c:
 return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("aligned(%s)",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));});case Cyc_Flags_Vc_c:
 goto _LL41;default: _LL41: return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("align(%s)",sizeof(char),10U),_tag_fat(_Tmp5,sizeof(void*),1));});};}case 7:
# 479
 return _tag_fat("packed",sizeof(char),7U);case 8: _Tmp1=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)a)->f1;{struct _fat_ptr s=_Tmp1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("section(\"%s\")",sizeof(char),14U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 9:
 return _tag_fat("nocommon",sizeof(char),9U);case 10:
 return _tag_fat("shared",sizeof(char),7U);case 11:
 return _tag_fat("unused",sizeof(char),7U);case 12:
 return _tag_fat("weak",sizeof(char),5U);case 13:
 return _tag_fat("dllimport",sizeof(char),10U);case 14:
 return _tag_fat("dllexport",sizeof(char),10U);case 15:
 return _tag_fat("no_instrument_function",sizeof(char),23U);case 16:
 return _tag_fat("constructor",sizeof(char),12U);case 17:
 return _tag_fat("destructor",sizeof(char),11U);case 18:
 return _tag_fat("no_check_memory_usage",sizeof(char),22U);case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f1==Cyc_Absyn_Printf_ft){_Tmp3=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f2;_Tmp0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f3;{int n=_Tmp3;int m=_Tmp0;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)n;_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned)m;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("format(printf,%u,%u)",sizeof(char),21U),_tag_fat(_Tmp6,sizeof(void*),2));});}}else{_Tmp3=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f2;_Tmp0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)a)->f3;{int n=_Tmp3;int m=_Tmp0;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)n;_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned)m;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("format(scanf,%u,%u)",sizeof(char),20U),_tag_fat(_Tmp6,sizeof(void*),2));});}}case 20: _Tmp3=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)a)->f1;{int n=_Tmp3;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)n;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("initializes(%d)",sizeof(char),16U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 21: _Tmp3=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)a)->f1;{int n=_Tmp3;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)n;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("noliveunique(%d)",sizeof(char),17U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 22: _Tmp3=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)a)->f1;{int n=_Tmp3;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)n;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("consume(%d)",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 23:
 return _tag_fat("pure",sizeof(char),5U);case 26:
 return _tag_fat("always_inline",sizeof(char),14U);case 24: _Tmp1=((struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)a)->f1;{struct _fat_ptr s=_Tmp1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("__mode__(\"%s\")",sizeof(char),15U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 25: _Tmp1=((struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*)a)->f1;{struct _fat_ptr s=_Tmp1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("alias(\"%s\")",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));});}default:
 return _tag_fat("no_throw",sizeof(char),9U);};}
# 504
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){return Cyc_PP_text(Cyc_Absynpp_attribute2string(a));}
# 506
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts==0)return Cyc_PP_nil_doc();
if(Cyc_Flags_c_compiler==Cyc_Flags_Vc_c)
return Cyc_Absynpp_noncallconv2doc(atts);else{
return({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" __attribute__",sizeof(char),15U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=_tag_fat("((",sizeof(char),3U);struct _fat_ptr _Tmp3=_tag_fat("))",sizeof(char),3U);struct _fat_ptr _Tmp4=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_Tmp2,_Tmp3,_Tmp4,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_att2doc,atts));});_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});};}
# 515
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms==0)return 0;{
void*_Tmp0=(void*)tms->hd;switch(*((int*)_Tmp0)){case 2:
 return 1;case 5:
# 520
 if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c)
return 0;else{
return Cyc_Absynpp_next_is_pointer(tms->tl);};default:
# 524
 return 0;};}}
# 528
static struct Cyc_PP_Doc*Cyc_Absynpp_question (void){
static struct Cyc_PP_Doc*cache_question=0;
if(!((unsigned)cache_question))
cache_question=Cyc_PP_tex_output?Cyc_PP_text_width(_tag_fat("{?}",sizeof(char),4U),1): Cyc_PP_text(_tag_fat("?",sizeof(char),2U));
return _check_null(cache_question);}
# 534
static struct Cyc_PP_Doc*Cyc_Absynpp_lb (void){
static struct Cyc_PP_Doc*cache_lb=0;
if(!((unsigned)cache_lb))
cache_lb=Cyc_PP_tex_output?Cyc_PP_text_width(_tag_fat("{\\lb}",sizeof(char),6U),1): Cyc_PP_text(_tag_fat("{",sizeof(char),2U));
return _check_null(cache_lb);}
# 540
static struct Cyc_PP_Doc*Cyc_Absynpp_rb (void){
static struct Cyc_PP_Doc*cache_rb=0;
if(!((unsigned)cache_rb))
cache_rb=Cyc_PP_tex_output?Cyc_PP_text_width(_tag_fat("{\\rb}",sizeof(char),6U),1): Cyc_PP_text(_tag_fat("}",sizeof(char),2U));
return _check_null(cache_rb);}
# 546
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar (void){
static struct Cyc_PP_Doc*cache_dollar=0;
if(!((unsigned)cache_dollar))
cache_dollar=Cyc_PP_tex_output?Cyc_PP_text_width(_tag_fat("\\$",sizeof(char),3U),1): Cyc_PP_text(_tag_fat("$",sizeof(char),2U));
return _check_null(cache_dollar);}
# 553
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_seq(sep,ss);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
# 558
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(1;tms!=0;tms=tms->tl){
void*_Tmp0=(void*)tms->hd;void*_Tmp1;switch(*((int*)_Tmp0)){case 0:
 Cyc_fprintf(Cyc_stderr,_tag_fat("Carray_mod ",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 1:
 Cyc_fprintf(Cyc_stderr,_tag_fat("ConstArray_mod ",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 3: if(*((int*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1)==1){_Tmp1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1)->f1;{struct Cyc_List_List*ys=_Tmp1;
# 564
Cyc_fprintf(Cyc_stderr,_tag_fat("Function_mod(",sizeof(char),14U),_tag_fat(0U,sizeof(void*),0));
for(1;ys!=0;ys=ys->tl){
struct _fat_ptr*v=(*((struct _tuple9*)ys->hd)).f0;
if(v==0)Cyc_fprintf(Cyc_stderr,_tag_fat("?",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));else{
Cyc_fprintf(Cyc_stderr,*v,_tag_fat(0U,sizeof(void*),0));}
if(ys->tl!=0)Cyc_fprintf(Cyc_stderr,_tag_fat(",",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 571
Cyc_fprintf(Cyc_stderr,_tag_fat(") ",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}else{
Cyc_fprintf(Cyc_stderr,_tag_fat("Function_mod()",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 5:
 Cyc_fprintf(Cyc_stderr,_tag_fat("Attributes_mod ",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 4:
 Cyc_fprintf(Cyc_stderr,_tag_fat("TypeParams_mod ",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));goto _LL0;default:
 Cyc_fprintf(Cyc_stderr,_tag_fat("Pointer_mod ",sizeof(char),13U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 578
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 581
static struct Cyc_PP_Doc*const(*Cyc_Absynpp_rgn2doc)(void*)=(struct Cyc_PP_Doc*const(*)(void*))Cyc_Absynpp_ntyp2doc;
# 583
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch((int)sc){case Cyc_Absyn_Static:
 return Cyc_PP_text(_tag_fat("static ",sizeof(char),8U));case Cyc_Absyn_Public:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern:
 return Cyc_PP_text(_tag_fat("extern ",sizeof(char),8U));case Cyc_Absyn_ExternC:
 return Cyc_PP_text(_tag_fat("extern \"C\" ",sizeof(char),12U));case Cyc_Absyn_Abstract:
 return Cyc_PP_text(_tag_fat("abstract ",sizeof(char),10U));case Cyc_Absyn_Register:
 return Cyc_PP_text(_tag_fat("register ",sizeof(char),10U));default:
 return Cyc_PP_nil_doc();};}
# 596
static struct Cyc_PP_Doc*Cyc_Absynpp_aqual_val2doc(enum Cyc_Absyn_AliasQualVal aqv){
switch((int)aqv){case Cyc_Absyn_Aliasable_qual:
 return Cyc_PP_text(_tag_fat("ALIASABLE",sizeof(char),10U));case Cyc_Absyn_Unique_qual:
 return Cyc_PP_text(_tag_fat("UNIQUE",sizeof(char),7U));goto _LL0;case Cyc_Absyn_Refcnt_qual:
 return Cyc_PP_text(_tag_fat("REFCNT",sizeof(char),7U));goto _LL0;case Cyc_Absyn_Restricted_qual:
 return Cyc_PP_text(_tag_fat("RESTRICTED",sizeof(char),11U));goto _LL0;default:
 _throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("Absynpp::aqual_val2doc unrecognized value",sizeof(char),42U);_Tmp0;}));}_LL0:;}
# 606
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms==0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
struct Cyc_PP_Doc*p_rest=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=rest,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;unsigned _Tmp7;void*_Tmp8;void*_Tmp9;switch(*((int*)_Tmp0)){case 0: _Tmp9=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*zeroterm=_Tmp9;
# 612
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,Cyc_Absyn_type2bool(0,zeroterm)?({
struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("[]@zeroterm",sizeof(char),12U));_TmpA[1]=_TmpB;}):({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("[]",sizeof(char),3U));_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});}case 1: _Tmp9=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp8=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp9;void*zeroterm=_Tmp8;
# 616
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
return({struct Cyc_PP_Doc*_TmpA[4];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("[",sizeof(char),2U));_TmpA[1]=_TmpB;}),({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_exp2doc(e);_TmpA[2]=_TmpB;}),
Cyc_Absyn_type2bool(0,zeroterm)?({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("]@zeroterm",sizeof(char),11U));_TmpA[3]=_TmpB;}):({struct Cyc_PP_Doc*_TmpB=Cyc_PP_text(_tag_fat("]",sizeof(char),2U));_TmpA[3]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),4));});}case 3: _Tmp9=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp9;
# 620
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
unsigned _TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;void*_Tmp11;int _Tmp12;void*_Tmp13;if(*((int*)args)==1){_Tmp13=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f1;_Tmp12=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f2;_Tmp11=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f3;_Tmp10=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f4;_TmpF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f5;_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f6;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f7;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f8;_TmpB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f9;{struct Cyc_List_List*args2=_Tmp13;int c_varargs=_Tmp12;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp11;void*effopt=_Tmp10;struct Cyc_List_List*rgn_po=_TmpF;struct Cyc_List_List*qb=_TmpE;struct Cyc_Absyn_Exp*req=_TmpD;struct Cyc_Absyn_Exp*ens=_TmpC;struct Cyc_Absyn_Exp*thrws=_TmpB;
# 623
return({struct Cyc_PP_Doc*_Tmp14[2];_Tmp14[0]=rest,({struct Cyc_PP_Doc*_Tmp15=Cyc_Absynpp_funargs2doc(args2,c_varargs,cyc_varargs,effopt,rgn_po,qb,req,ens,thrws);_Tmp14[1]=_Tmp15;});Cyc_PP_cat(_tag_fat(_Tmp14,sizeof(struct Cyc_PP_Doc*),2));});}}else{_Tmp13=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f1;_TmpA=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f2;{struct Cyc_List_List*sl=_Tmp13;unsigned loc=_TmpA;
# 625
return({struct Cyc_PP_Doc*_Tmp14[2];_Tmp14[0]=rest,({struct Cyc_PP_Doc*_Tmp15=({struct _fat_ptr _Tmp16=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _Tmp17=_tag_fat(")",sizeof(char),2U);struct _fat_ptr _Tmp18=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_Tmp16,_Tmp17,_Tmp18,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_PP_textptr,sl));});_Tmp14[1]=_Tmp15;});Cyc_PP_cat(_tag_fat(_Tmp14,sizeof(struct Cyc_PP_Doc*),2));});}};}}case 5: _Tmp9=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*atts=_Tmp9;
# 628
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
# 630
if(Cyc_Absynpp_next_is_pointer(tms->tl))
rest=p_rest;
return({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_atts2doc(atts);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});}else{
# 634
if(Cyc_Absynpp_next_is_pointer(tms->tl))
return({struct Cyc_PP_Doc*_TmpA[2];({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_callconv2doc(atts);_TmpA[0]=_TmpB;}),_TmpA[1]=rest;Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
return rest;};}case 4: _Tmp9=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_List_List*ts=_Tmp9;unsigned loc=_Tmp7;int print_kinds=_Tmp6;
# 639
if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(print_kinds)
return({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_ktvars2doc(ts);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});
return({struct Cyc_PP_Doc*_TmpA[2];_TmpA[0]=rest,({struct Cyc_PP_Doc*_TmpB=Cyc_Absynpp_tvars2doc(ts);_TmpA[1]=_TmpB;});Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),2));});}default: _Tmp9=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.rgn;_Tmp8=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.nullable;_Tmp5=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.bounds;_Tmp4=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.zero_term;_Tmp3=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.autoreleased;_Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.aqual;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*rgn=_Tmp9;void*nullable=_Tmp8;void*bd=_Tmp5;void*zt=_Tmp4;void*rel=_Tmp3;void*aq=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 646
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*aqd=mt;
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*reld=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(tq2);
{void*_TmpA=Cyc_Absyn_compress(bd);void*_TmpB;if(*((int*)_TmpA)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)){case 15:
 ptr=Cyc_Absynpp_question();goto _LL17;case 14: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f2!=0){_TmpB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f2->hd;{void*targ=_TmpB;
# 656
{void*_TmpC=Cyc_Absyn_compress(targ);void*_TmpD;if(*((int*)_TmpC)==8){_TmpD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_TmpC)->f1;{struct Cyc_Absyn_Exp*e=_TmpD;
# 658
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?_tag_fat("*",sizeof(char),2U): _tag_fat("@",sizeof(char),2U));{
struct _tuple13 _TmpE=Cyc_Evexp_eval_const_uint_exp(e);int _TmpF;unsigned _Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{unsigned val=_Tmp10;int known=_TmpF;
if(!known || val!=1U)
ptr=({struct Cyc_PP_Doc*_Tmp11[4];_Tmp11[0]=ptr,({struct Cyc_PP_Doc*_Tmp12=Cyc_Absynpp_lb();_Tmp11[1]=_Tmp12;}),({struct Cyc_PP_Doc*_Tmp12=Cyc_Absynpp_exp2doc(e);_Tmp11[2]=_Tmp12;}),({struct Cyc_PP_Doc*_Tmp12=Cyc_Absynpp_rb();_Tmp11[3]=_Tmp12;});Cyc_PP_cat(_tag_fat(_Tmp11,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1E;}}}}else{
# 664
ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?_tag_fat("*",sizeof(char),2U): _tag_fat("@",sizeof(char),2U));
ptr=({struct Cyc_PP_Doc*_TmpE[4];_TmpE[0]=ptr,({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_lb();_TmpE[1]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_typ2doc(targ);_TmpE[2]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_rb();_TmpE[3]=_TmpF;});Cyc_PP_cat(_tag_fat(_TmpE,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL1E;}_LL1E:;}
# 668
goto _LL17;}}else{goto _LL1C;}default: goto _LL1C;}else{_LL1C:
# 670
 ptr=Cyc_PP_text(Cyc_Absyn_type2bool(1,nullable)?_tag_fat("*",sizeof(char),2U): _tag_fat("@",sizeof(char),2U));
ptr=({struct Cyc_PP_Doc*_TmpC[4];_TmpC[0]=ptr,({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(bd);_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL17;}_LL17:;}
# 674
if(Cyc_Absynpp_print_zeroterm){
{void*_TmpA=Cyc_Absyn_compress(aq);void*_TmpB;enum Cyc_Absyn_AliasQualVal _TmpC;if(*((int*)_TmpA)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)){case 17: _TmpC=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)->f1;{enum Cyc_Absyn_AliasQualVal aqv=_TmpC;
# 677
if(aqv==Cyc_Absyn_Aliasable_qual)
goto _LL2D;else{
# 680
aqd=({struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("@aqual(",sizeof(char),8U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_aqual_val2doc(aqv);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL2D;}_LL2D:;
# 683
goto _LL26;}case 18: _TmpB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f2;{struct Cyc_List_List*tv=_TmpB;
# 685
aqd=({struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat("@aqual(",sizeof(char),8U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_typ2doc((void*)tv->hd);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpD[2]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL26;}default: goto _LL2B;}else{_LL2B:
# 692
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("unexpected alias qualifier",sizeof(char),27U);_TmpE;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpF;_TmpF.tag=2,_TmpF.f1=(void*)aq;_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpF,sizeof(void*),2));});}_LL26:;}
# 694
if(!is_char_ptr && Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(_tag_fat("@zeroterm",sizeof(char),10U));else{
if(is_char_ptr && !Cyc_Absyn_type2bool(0,zt))
ztd=Cyc_PP_text(_tag_fat("@nozeroterm",sizeof(char),12U));}
if(Cyc_Absyn_type2bool(0,rel))
reld=Cyc_PP_text(_tag_fat("@autoreleased",sizeof(char),14U));}
# 701
{void*_TmpA=Cyc_Absyn_compress(rgn);switch(*((int*)_TmpA)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)==6)
goto _LL32;else{goto _LL37;}case 1: if(Cyc_Absynpp_print_for_cycdoc)
goto _LL32;else{goto _LL37;}default: _LL37:
 rgd=Cyc_Absynpp_rgn2doc(rgn);}_LL32:;}{
# 706
struct Cyc_PP_Doc*spacer0=reld!=mt && ztd!=mt?Cyc_PP_text(_tag_fat(" ",sizeof(char),2U)): mt;
struct Cyc_PP_Doc*spacer1=tqd!=mt &&((ztd!=mt || rgd!=mt)|| reld!=mt)?Cyc_PP_text(_tag_fat(" ",sizeof(char),2U)): mt;
struct Cyc_PP_Doc*spacer2=rest!=mt?Cyc_PP_text(_tag_fat(" ",sizeof(char),2U)): mt;
return({struct Cyc_PP_Doc*_TmpA[10];_TmpA[0]=ptr,_TmpA[1]=aqd,_TmpA[2]=ztd,_TmpA[3]=spacer0,_TmpA[4]=reld,_TmpA[5]=rgd,_TmpA[6]=spacer1,_TmpA[7]=tqd,_TmpA[8]=spacer2,_TmpA[9]=rest;Cyc_PP_cat(_tag_fat(_TmpA,sizeof(struct Cyc_PP_Doc*),10));});}}};}}
# 713
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 716
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*r=_Tmp1;
# 718
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_rgn2doc(r);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=*rgions;_Tmp3;});*rgions=_Tmp2;});goto _LL0;}}else{goto _LL5;}case 10: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp1;
# 720
for(1;ts!=0;ts=ts->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)ts->hd);}
goto _LL0;}default: goto _LL5;}else{_LL5:
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_typ2doc(t);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=*effects;_Tmp3;});*effects=_Tmp2;});goto _LL0;}_LL0:;}
# 726
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=Cyc_List_imp_rev(rgions);
effects=Cyc_List_imp_rev(effects);
if(rgions==0 && effects!=0)
return Cyc_PP_group(_tag_fat("",sizeof(char),1U),_tag_fat("",sizeof(char),1U),_tag_fat("+",sizeof(char),2U),effects);{
struct Cyc_PP_Doc*doc1=Cyc_Absynpp_group_braces(_tag_fat(",",sizeof(char),2U),rgions);
return({struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp1=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp2=_tag_fat("+",sizeof(char),2U);Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({struct Cyc_List_List*_Tmp3=effects;Cyc_List_imp_append(_Tmp3,({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=doc1,_Tmp4->tl=0;_Tmp4;}));}));});}}
# 738
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
return(int)k==0?Cyc_PP_text(_tag_fat("struct ",sizeof(char),8U)): Cyc_PP_text(_tag_fat("union ",sizeof(char),7U));}
# 742
static struct _tuple18*Cyc_Absynpp_aggrfield2arg(struct Cyc_Absyn_Aggrfield*f){
return({struct _tuple18*_Tmp0=_cycalloc(sizeof(struct _tuple18));_Tmp0->f0=f->tq,_Tmp0->f1=f->type;_Tmp0;});}
# 747
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
enum Cyc_Absyn_AggrKind _Tmp0;void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_AliasQualVal _Tmp3;struct _fat_ptr _Tmp4;union Cyc_Absyn_AggrInfo _Tmp5;int _Tmp6;enum Cyc_Absyn_Size_of _Tmp7;enum Cyc_Absyn_Sign _Tmp8;union Cyc_Absyn_DatatypeFieldInfo _Tmp9;void*_TmpA;union Cyc_Absyn_DatatypeInfo _TmpB;switch(*((int*)t)){case 4:
# 750
 return Cyc_PP_text(_tag_fat("[[[array]]]",sizeof(char),12U));case 5:
 return Cyc_PP_nil_doc();case 3:
 return Cyc_PP_nil_doc();case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 0:
# 754
 return Cyc_PP_text(_tag_fat("void",sizeof(char),5U));case 23: _TmpB=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_DatatypeInfo tu_info=_TmpB;struct Cyc_List_List*ts=_TmpA;
# 767
int _TmpC;void*_TmpD;if(tu_info.UnknownDatatype.tag==1){_TmpD=tu_info.UnknownDatatype.val.name;_TmpC=tu_info.UnknownDatatype.val.is_extensible;{struct _tuple1*n=_TmpD;int is_x=_TmpC;
_TmpD=n;_TmpC=is_x;goto _LL53;}}else{_TmpD=(*tu_info.KnownDatatype.val)->name;_TmpC=(*tu_info.KnownDatatype.val)->is_extensible;_LL53: {struct _tuple1*n=_TmpD;int is_x=_TmpC;
# 770
struct Cyc_PP_Doc*kw=Cyc_PP_text(_tag_fat("datatype ",sizeof(char),10U));
struct Cyc_PP_Doc*ext=is_x?Cyc_PP_text(_tag_fat("@extensible ",sizeof(char),13U)): Cyc_PP_nil_doc();
return({struct Cyc_PP_Doc*_TmpE[4];_TmpE[0]=ext,_TmpE[1]=kw,({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_qvar2doc(n);_TmpE[2]=_TmpF;}),({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_tps2doc(ts);_TmpE[3]=_TmpF;});Cyc_PP_cat(_tag_fat(_TmpE,sizeof(struct Cyc_PP_Doc*),4));});}};}case 24: _Tmp9=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_DatatypeFieldInfo tuf_info=_Tmp9;struct Cyc_List_List*ts=_TmpA;
# 775
int _TmpC;void*_TmpD;void*_TmpE;if(tuf_info.UnknownDatatypefield.tag==1){_TmpE=tuf_info.UnknownDatatypefield.val.datatype_name;_TmpD=tuf_info.UnknownDatatypefield.val.field_name;_TmpC=tuf_info.UnknownDatatypefield.val.is_extensible;{struct _tuple1*tname=_TmpE;struct _tuple1*fname=_TmpD;int is_x=_TmpC;
# 777
_TmpE=tname;_TmpC=is_x;_TmpD=fname;goto _LL58;}}else{_TmpE=tuf_info.KnownDatatypefield.val.f0->name;_TmpC=tuf_info.KnownDatatypefield.val.f0->is_extensible;_TmpD=tuf_info.KnownDatatypefield.val.f1->name;_LL58: {struct _tuple1*tname=_TmpE;int is_x=_TmpC;struct _tuple1*fname=_TmpD;
# 780
struct Cyc_PP_Doc*kw=Cyc_PP_text(is_x?_tag_fat("@extensible datatype ",sizeof(char),22U): _tag_fat("datatype ",sizeof(char),10U));
return({struct Cyc_PP_Doc*_TmpF[4];_TmpF[0]=kw,({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_qvar2doc(tname);_TmpF[1]=_Tmp10;}),({struct Cyc_PP_Doc*_Tmp10=Cyc_PP_text(_tag_fat(".",sizeof(char),2U));_TmpF[2]=_Tmp10;}),({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_qvar2doc(fname);_TmpF[3]=_Tmp10;});Cyc_PP_cat(_tag_fat(_TmpF,sizeof(struct Cyc_PP_Doc*),4));});}};}case 1: _Tmp8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp7=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Sign sn=_Tmp8;enum Cyc_Absyn_Size_of sz=_Tmp7;
# 784
struct _fat_ptr sns;
struct _fat_ptr ts;
switch((int)sn){case Cyc_Absyn_None:
 goto _LL5D;case Cyc_Absyn_Signed: _LL5D:
 sns=_tag_fat("",sizeof(char),1U);goto _LL59;default:
 sns=_tag_fat("unsigned ",sizeof(char),10U);goto _LL59;}_LL59:;
# 791
switch((int)sz){case Cyc_Absyn_Char_sz:
# 793
 switch((int)sn){case Cyc_Absyn_None:
 sns=_tag_fat("",sizeof(char),1U);goto _LL6D;case Cyc_Absyn_Signed:
 sns=_tag_fat("signed ",sizeof(char),8U);goto _LL6D;default:
 sns=_tag_fat("unsigned ",sizeof(char),10U);goto _LL6D;}_LL6D:;
# 798
ts=_tag_fat("char",sizeof(char),5U);
goto _LL60;case Cyc_Absyn_Short_sz:
 ts=_tag_fat("short",sizeof(char),6U);goto _LL60;case Cyc_Absyn_Int_sz:
 ts=_tag_fat("int",sizeof(char),4U);goto _LL60;case Cyc_Absyn_Long_sz:
 ts=_tag_fat("long",sizeof(char),5U);goto _LL60;case Cyc_Absyn_LongLong_sz:
 goto _LL6C;default: _LL6C:
# 805
 if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
ts=_tag_fat("long long",sizeof(char),10U);goto _LL74;}else{
ts=_tag_fat("__int64",sizeof(char),8U);goto _LL74;}_LL74:;
# 809
goto _LL60;}_LL60:;
# 811
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=(struct _fat_ptr)((struct _fat_ptr)sns);_TmpD;});struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=(struct _fat_ptr)((struct _fat_ptr)ts);_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_aprintf(_tag_fat("%s%s",sizeof(char),5U),_tag_fat(_TmpE,sizeof(void*),2));}));}case 2: _Tmp6=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{int i=_Tmp6;
# 813
switch((int)i){case 0:
 return Cyc_PP_text(_tag_fat("float",sizeof(char),6U));case 1:
 return Cyc_PP_text(_tag_fat("double",sizeof(char),7U));default:
 return Cyc_PP_text(_tag_fat("long double",sizeof(char),12U));};}case 3: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpA;
# 818
return({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("_Complex ",sizeof(char),10U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_ntyp2doc((void*)ts->hd);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});}case 25: _Tmp5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp5;struct Cyc_List_List*ts=_TmpA;
# 820
struct _tuple12 _TmpC=Cyc_Absyn_aggr_kinded_name(info);void*_TmpD;enum Cyc_Absyn_AggrKind _TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{enum Cyc_Absyn_AggrKind k=_TmpE;struct _tuple1*n=_TmpD;
return({struct Cyc_PP_Doc*_TmpF[3];({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_aggr_kind2doc(k);_TmpF[0]=_Tmp10;}),({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_qvar2doc(n);_TmpF[1]=_Tmp10;}),({struct Cyc_PP_Doc*_Tmp10=Cyc_Absynpp_tps2doc(ts);_TmpF[2]=_Tmp10;});Cyc_PP_cat(_tag_fat(_TmpF,sizeof(struct Cyc_PP_Doc*),3));});}}case 21: _TmpA=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct Cyc_List_List*fs=_TmpA;
# 828
return({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("enum ",sizeof(char),6U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(fs));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});}case 20: _TmpA=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct _tuple1*n=_TmpA;
return({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("enum ",sizeof(char),6U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_qvar2doc(n);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});}case 22: _Tmp4=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct _fat_ptr t=_Tmp4;
# 832
return Cyc_PP_text(t);}case 4: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpA;
# 841
return({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("region_t<",sizeof(char),10U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rgn2doc((void*)ts->hd);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}case 19: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpA;
# 843
return({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("aqual_t<",sizeof(char),9U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc((void*)ts->hd);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}case 18: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpA;
# 845
return Cyc_Absynpp_typ2doc((void*)ts->hd);goto _LL0;}case 5: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpA;
# 847
return({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("tag_t<",sizeof(char),7U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc((void*)ts->hd);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}case 11: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_TmpA;
# 849
return({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("regions(",sizeof(char),9U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc((void*)ts->hd);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}case 6:
 return Cyc_PP_text(_tag_fat("`H",sizeof(char),3U));case 7:
 return Cyc_PP_text(_tag_fat("`U",sizeof(char),3U));case 8:
 return Cyc_PP_text(_tag_fat("`RC",sizeof(char),4U));case 9:
 goto _LL3A;case 10: _LL3A:
 return Cyc_Absynpp_eff2doc(t);case 14: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2!=0){_TmpA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2->hd;{void*t=_TmpA;
# 859
return({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("@thin @numelts",sizeof(char),15U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc(t);_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});}}else{
# 862
return Cyc_PP_text(_tag_fat("@thin",sizeof(char),6U));}case 17: _Tmp3=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{enum Cyc_Absyn_AliasQualVal aqv=_Tmp3;
# 860
return Cyc_Absynpp_aqual_val2doc(aqv);}case 16: _TmpA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*qtv=_TmpA;
return({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("aquals(",sizeof(char),8U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_typ2doc((void*)qtv->hd);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}case 15:
# 863
 return Cyc_PP_text(_tag_fat("@fat",sizeof(char),5U));case 12:
 return Cyc_PP_text(_tag_fat("@true",sizeof(char),6U));default:
 return Cyc_PP_text(_tag_fat("@false",sizeof(char),7U));}case 1: _TmpA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f3;_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f4;{struct Cyc_Core_Opt*k=_TmpA;void*topt=_Tmp2;int i=_Tmp6;struct Cyc_Core_Opt*tvs=_Tmp1;
# 756
if(topt!=0)
# 758
return Cyc_Absynpp_ntyp2doc(topt);{
struct _fat_ptr kindstring=k==0?_tag_fat("K",sizeof(char),2U): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)k->v);
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=(struct _fat_ptr)((struct _fat_ptr)kindstring);_TmpD;});struct Cyc_Int_pa_PrintArg_struct _TmpD=({struct Cyc_Int_pa_PrintArg_struct _TmpE;_TmpE.tag=1,_TmpE.f1=(unsigned long)i;_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_aprintf(_tag_fat("`E%s%d",sizeof(char),7U),_tag_fat(_TmpE,sizeof(void*),2));}));}}case 2: _TmpA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*tv=_TmpA;
# 762
struct Cyc_PP_Doc*s=Cyc_Absynpp_tvar2doc(tv);
if(Cyc_Absynpp_print_all_kinds)
s=({struct Cyc_PP_Doc*_TmpC[3];_TmpC[0]=s,({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("::",sizeof(char),3U));_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_kindbound2doc(tv->kind);_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});
return s;}case 6: _Tmp0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f1;_Tmp6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f2;_TmpA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;{enum Cyc_Absyn_AggrKind k=_Tmp0;int is_tuple=_Tmp6;struct Cyc_List_List*fs=_TmpA;
# 823
if(is_tuple)
return({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_dollar();_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_args2doc(({(struct Cyc_List_List*(*)(struct _tuple18*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_aggrfield2arg,fs));_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});else{
# 826
return({struct Cyc_PP_Doc*_TmpC[4];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_aggr_kind2doc(k);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_lb();_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(fs));_TmpC[2]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_rb();_TmpC[3]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),4));});}}case 8: _TmpA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 830
return({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("valueof_t(",sizeof(char),11U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}case 10: _TmpA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
return({struct Cyc_PP_Doc*_TmpC[3];({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat("typeof(",sizeof(char),8U));_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_exp2doc(e);_TmpC[1]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpC[2]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),3));});}case 7: _TmpA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;{struct _tuple1*n=_TmpA;struct Cyc_List_List*ts=_Tmp2;struct Cyc_Absyn_Typedefdecl*kopt=_Tmp1;
# 839
return({struct Cyc_PP_Doc*_TmpC[2];({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_qvar2doc(n);_TmpC[0]=_TmpD;}),({struct Cyc_PP_Doc*_TmpD=Cyc_Absynpp_tps2doc(ts);_TmpC[1]=_TmpD;});Cyc_PP_cat(_tag_fat(_TmpC,sizeof(struct Cyc_PP_Doc*),2));});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1)->r)){case 0: _TmpA=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*d=_TmpA;
# 855
return Cyc_Absynpp_aggrdecl2doc(d);}case 1: _TmpA=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*d=_TmpA;
return Cyc_Absynpp_enumdecl2doc(d);}default: _TmpA=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*d=_TmpA;
return Cyc_Absynpp_datatypedecl2doc(d);}}}_LL0:;}
# 869
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple0*cmp){
void*_Tmp0;void*_Tmp1;_Tmp1=cmp->f0;_Tmp0=cmp->f1;{void*r1=_Tmp1;void*r2=_Tmp0;
return({struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_rgn2doc(r1);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(" > ",sizeof(char),4U));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_rgn2doc(r2);_Tmp2[2]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));});}}
# 873
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct _tuple0*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;})(Cyc_Absynpp_rgn_cmp2doc,_tag_fat(",",sizeof(char),2U),po);}
# 878
struct Cyc_PP_Doc*Cyc_Absynpp_qb_cmp2doc(struct _tuple0*cmp){
void*_Tmp0;void*_Tmp1;_Tmp1=cmp->f0;_Tmp0=cmp->f1;{void*t1=_Tmp1;void*t2=_Tmp0;
return({struct Cyc_PP_Doc*_Tmp2[3];({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_ntyp2doc(t2);_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(" >= ",sizeof(char),5U));_Tmp2[1]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_ntyp2doc(t1);_Tmp2[2]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),3));});}}
# 882
struct Cyc_PP_Doc*Cyc_Absynpp_qualbnd2doc(struct Cyc_List_List*qb){
return({struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp1=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple0*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_qb_cmp2doc,qb));});}
# 886
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple9*t){
struct _fat_ptr*vo=(*t).f0;
struct Cyc_Core_Opt*dopt=vo==0?0:({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(*vo);_Tmp0->v=_Tmp1;});_Tmp0;});
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,dopt);}
# 892
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_List_List*qb,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens,struct Cyc_Absyn_Exp*thrws){
# 898
struct Cyc_List_List*arg_docs=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple9*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs)
arg_docs=({struct Cyc_List_List*_Tmp0=arg_docs;Cyc_List_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(_tag_fat("...",sizeof(char),4U));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));});else{
if(cyc_varargs!=0){
struct Cyc_PP_Doc*varargs_doc=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("...",sizeof(char),4U));_Tmp0[0]=_Tmp1;}),
cyc_varargs->inject?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" inject ",sizeof(char),9U));_Tmp0[1]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_funarg2doc(({struct _tuple9*_Tmp2=_cycalloc(sizeof(struct _tuple9));_Tmp2->f0=cyc_varargs->name,_Tmp2->f1=cyc_varargs->tq,_Tmp2->f2=cyc_varargs->type;_Tmp2;}));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
# 907
arg_docs=({struct Cyc_List_List*_Tmp0=arg_docs;Cyc_List_append(_Tmp0,({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=varargs_doc,_Tmp1->tl=0;_Tmp1;}));});}}{
# 909
struct Cyc_PP_Doc*arg_doc=Cyc_PP_group(_tag_fat("",sizeof(char),1U),_tag_fat("",sizeof(char),1U),_tag_fat(",",sizeof(char),2U),arg_docs);
if(effopt!=0 && Cyc_Absynpp_print_all_effects)
arg_doc=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_eff2doc(effopt);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(rgn_po!=0)
arg_doc=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(":",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rgnpo2doc(rgn_po);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(qb!=0){
struct Cyc_PP_Doc*sep=rgn_po==0?Cyc_PP_text(_tag_fat(":",sizeof(char),2U)): Cyc_PP_text(_tag_fat(",",sizeof(char),2U));
arg_doc=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=arg_doc,_Tmp0[1]=sep,({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qualbnd2doc(qb);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}{
# 918
struct Cyc_PP_Doc*res=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=arg_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(req!=0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" @requires(",sizeof(char),12U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(req);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
if(ens!=0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" @ensures(",sizeof(char),11U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(ens);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
if(thrws!=0)
res=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=res,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" @throws(",sizeof(char),10U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(thrws);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
return res;}}}
# 928
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple1*q){
struct Cyc_List_List*prefix=0;
int match;
{union Cyc_Absyn_Nmspace _Tmp0=(*q).f0;void*_Tmp1;switch(_Tmp0.C_n.tag){case 4:
 _Tmp1=0;goto _LL4;case 1: _Tmp1=_Tmp0.Rel_n.val;_LL4: {struct Cyc_List_List*x=_Tmp1;
# 934
match=0;
if(((unsigned)x && Cyc_Absynpp_qvar_to_Cids)&& !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 938
prefix=x;}
goto _LL0;}case 3: _Tmp1=_Tmp0.C_n.val;{struct Cyc_List_List*x=_Tmp1;
# 941
match=Cyc_Absynpp_use_curr_namespace &&({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_prefix;})(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
# 943
goto _LL0;}default: _Tmp1=_Tmp0.Abs_n.val;{struct Cyc_List_List*x=_Tmp1;
# 945
match=Cyc_Absynpp_use_curr_namespace &&({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_prefix;})(Cyc_strptrcmp,x,Cyc_Absynpp_curr_namespace);
if(Cyc_Absynpp_qvar_to_Cids){
if((unsigned)x && !Cyc_strptrcmp((struct _fat_ptr*)x->hd,Cyc_Absynpp_nocyc_strptr))
prefix=x->tl;else{
# 950
prefix=Cyc_Absynpp_add_cyc_prefix?({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=Cyc_Absynpp_cyc_stringptr,_Tmp2->tl=x;_Tmp2;}): x;}}
# 952
goto _LL0;}}_LL0:;}
# 954
if(Cyc_Absynpp_qvar_to_Cids)
return({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=prefix;Cyc_List_append(_Tmp1,({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(*q).f1,_Tmp2->tl=0;_Tmp2;}));});Cyc_str_sepstr(_Tmp0,_tag_fat("_",sizeof(char),2U));});
if(match)
return*(*q).f1;
return({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=prefix;Cyc_List_append(_Tmp1,({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(*q).f1,_Tmp2->tl=0;_Tmp2;}));});Cyc_str_sepstr(_Tmp0,_tag_fat("::",sizeof(char),3U));});}
# 961
struct _fat_ptr Cyc_Absynpp_typedef_name2string(struct _tuple1*v){
# 963
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 966
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _Tmp0=(*v).f0;void*_Tmp1;switch(_Tmp0.C_n.tag){case 4:
 goto _LL4;case 1: if(_Tmp0.Rel_n.val==0){_LL4:
 return*(*v).f1;}else{_LLA:
# 975
 return({struct _fat_ptr _Tmp2=_tag_fat("/* bad namespace : */ ",sizeof(char),23U);Cyc_strconcat(_Tmp2,Cyc_Absynpp_qvar2string(v));});}case 3: _Tmp1=_Tmp0.C_n.val;{struct Cyc_List_List*l=_Tmp1;
# 970
_Tmp1=l;goto _LL8;}default: _Tmp1=_Tmp0.Abs_n.val;_LL8: {struct Cyc_List_List*l=_Tmp1;
# 972
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_strptrcmp,l,Cyc_Absynpp_curr_namespace)==0)
return*(*v).f1;
goto _LLA;}};}
# 978
return*(*v).f1;}
# 981
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple1*q){return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple1*v){return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 984
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple1*q){
struct _fat_ptr qs=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output)
return({struct _fat_ptr _Tmp0=(struct _fat_ptr)({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_strconcat(_tag_fat("\\textbf{",sizeof(char),9U),(struct _fat_ptr)qs);Cyc_strconcat(_Tmp1,_tag_fat("}",sizeof(char),2U));});Cyc_PP_text_width(_Tmp0,(int)Cyc_strlen(qs));});
return Cyc_PP_text(qs);}
# 991
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple1*v){
struct _fat_ptr vs=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output)
return({struct _fat_ptr _Tmp0=(struct _fat_ptr)({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_strconcat(_tag_fat("\\textbf{",sizeof(char),9U),(struct _fat_ptr)vs);Cyc_strconcat(_Tmp1,_tag_fat("}",sizeof(char),2U));});Cyc_PP_text_width(_Tmp0,(int)Cyc_strlen(vs));});
return Cyc_PP_text(vs);}
# 998
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return({struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);Cyc_Absynpp_tqtd2doc(_Tmp0,t,0);});}
# 1002
static struct Cyc_PP_Doc*Cyc_Absynpp_offsetof_field_to_doc(void*f){
void*_Tmp0;_Tmp0=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)f)->f1;{struct _fat_ptr*n=_Tmp0;
return Cyc_PP_textptr(n);};}
# 1015 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 1: _LL4:
 return 10000;case 3: _Tmp2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{enum Cyc_Absyn_Primop p=_Tmp2;
# 1020
switch((int)p){case Cyc_Absyn_Plus:
 return 100;case Cyc_Absyn_Times:
 return 110;case Cyc_Absyn_Minus:
 return 100;case Cyc_Absyn_Div:
 goto _LL5F;case Cyc_Absyn_Mod: _LL5F:
 return 110;case Cyc_Absyn_Eq:
 goto _LL63;case Cyc_Absyn_Neq: _LL63:
 return 70;case Cyc_Absyn_Gt:
 goto _LL67;case Cyc_Absyn_Lt: _LL67:
 goto _LL69;case Cyc_Absyn_Gte: _LL69:
 goto _LL6B;case Cyc_Absyn_Lte: _LL6B:
 return 80;case Cyc_Absyn_Not:
 goto _LL6F;case Cyc_Absyn_Bitnot: _LL6F:
 return 130;case Cyc_Absyn_Bitand:
 return 60;case Cyc_Absyn_Bitor:
 return 40;case Cyc_Absyn_Bitxor:
 return 50;case Cyc_Absyn_Bitlshift:
 return 90;case Cyc_Absyn_Bitlrshift:
 return 80;case Cyc_Absyn_Numelts:
 return 140;default:
 return 140;};}case 4:
# 1042
 return 20;case 5:
 return 130;case 6:
 return 30;case 7:
 return 35;case 8:
 return 30;case 9:
 return -10;case 10:
 return 140;case 2:
 return 140;case 11:
 return 130;case 12: _Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp1;
return Cyc_Absynpp_exp_prec(e2);}case 13: _Tmp1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp1;
return Cyc_Absynpp_exp_prec(e2);}case 14:
 return 120;case 16:
 return 15;case 15:
 goto _LL24;case 17: _LL24:
 goto _LL26;case 18: _LL26:
 goto _LL28;case 38: _LL28:
 goto _LL2A;case 39: _LL2A:
 goto _LL2C;case 37: _LL2C:
 goto _LL2E;case 19: _LL2E:
 goto _LL30;case 20: _LL30:
 goto _LL32;case 41: _LL32:
 goto _LL34;case 40: _LL34:
 return 130;case 21:
 goto _LL38;case 22: _LL38:
 goto _LL3A;case 23: _LL3A:
 return 140;case 24:
 goto _LL3E;case 25: _LL3E:
 goto _LL40;case 26: _LL40:
 goto _LL42;case 27: _LL42:
 goto _LL44;case 28: _LL44:
 goto _LL46;case 29: _LL46:
 goto _LL48;case 30: _LL48:
 goto _LL4A;case 31: _LL4A:
 goto _LL4C;case 32: _LL4C:
 goto _LL4E;case 33: _LL4E:
 goto _LL50;case 34: _LL50:
 goto _LL52;case 35: _LL52:
 return 140;default:
 return 10000;};}struct _tuple19{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};
# 1084
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc_in(struct Cyc_List_List*o){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
while((unsigned)o){
struct _tuple19*_Tmp0=(struct _tuple19*)o->hd;void*_Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct _fat_ptr c=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
s=({struct Cyc_PP_Doc*_Tmp3[6];_Tmp3[0]=s,({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp3[1]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(c);_Tmp3[2]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat("\" (",sizeof(char),4U));_Tmp3[3]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_exp2doc(e);_Tmp3[4]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp3[5]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),6));});
o=o->tl;
if((unsigned)o)
s=({struct Cyc_PP_Doc*_Tmp3[2];_Tmp3[0]=s,({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_Tmp3[1]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1093
return s;}
# 1096
static struct Cyc_PP_Doc*Cyc_Absynpp_asm_iolist_doc(struct Cyc_List_List*o,struct Cyc_List_List*i,struct Cyc_List_List*cl){
struct Cyc_PP_Doc*s=Cyc_PP_nil_doc();
if((unsigned)o)
s=({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(o);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});
if((unsigned)i){
if(!((unsigned)o))
s=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": : ",sizeof(char),5U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(i);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1104
s=({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_asm_iolist_doc_in(i);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}}
# 1106
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
s=({struct Cyc_PP_Doc*_Tmp0[2];_Tmp0[0]=s,ncol==0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": : :",sizeof(char),6U));_Tmp0[1]=_Tmp1;}):(ncol==1?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" : : ",sizeof(char),6U));_Tmp0[1]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_Tmp0[1]=_Tmp1;}));Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});
while(cl!=0){
s=({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(*((struct _fat_ptr*)cl->hd));_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
cl=cl->tl;
if((unsigned)cl)
s=({struct Cyc_PP_Doc*_Tmp0[2];_Tmp0[0]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(", ",sizeof(char),3U));_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});}}
# 1116
return s;}
# 1119
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_Tmp0=e->r;void*_Tmp1;enum Cyc_Absyn_MallocKind _Tmp2;int _Tmp3;enum Cyc_Absyn_Incrementor _Tmp4;void*_Tmp5;void*_Tmp6;enum Cyc_Absyn_Primop _Tmp7;struct _fat_ptr _Tmp8;void*_Tmp9;union Cyc_Absyn_Cnst _TmpA;switch(*((int*)_Tmp0)){case 0: _TmpA=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_TmpA;
s=Cyc_Absynpp_cnst2doc(c);goto _LL0;}case 1: _Tmp9=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp9;
s=Cyc_Absynpp_qvar2doc(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2: _Tmp8=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr p=_Tmp8;
# 1126
s=({struct Cyc_PP_Doc*_TmpB[4];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("__cyclone_pragma__",sizeof(char),19U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(p);_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[3]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 3: _Tmp7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp7;struct Cyc_List_List*es=_Tmp9;
s=Cyc_Absynpp_primapp2doc(myprec,p,es);goto _LL0;}case 4: _Tmp9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Core_Opt*popt=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1129
s=({struct Cyc_PP_Doc*_TmpB[5];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_TmpB[1]=_TmpC;}),
popt==0?({struct Cyc_PP_Doc*_TmpC=Cyc_PP_nil_doc();_TmpB[2]=_TmpC;}):({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)popt->v);_TmpB[2]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("= ",sizeof(char),3U));_TmpB[3]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpB[4]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 5: _Tmp9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_Tmp9;enum Cyc_Absyn_Incrementor i=_Tmp4;
# 1136
struct Cyc_PP_Doc*es=Cyc_Absynpp_exp2doc_prec(myprec,e2);
switch((int)i){case Cyc_Absyn_PreInc:
 s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("++",sizeof(char),3U));_TmpB[0]=_TmpC;}),_TmpB[1]=es;Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL55;case Cyc_Absyn_PreDec:
 s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("--",sizeof(char),3U));_TmpB[0]=_TmpC;}),_TmpB[1]=es;Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL55;case Cyc_Absyn_PostInc:
 s=({struct Cyc_PP_Doc*_TmpB[2];_TmpB[0]=es,({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("++",sizeof(char),3U));_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL55;case Cyc_Absyn_PostDec:
 goto _LL5F;default: _LL5F:
 s=({struct Cyc_PP_Doc*_TmpB[2];_TmpB[0]=es,({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("--",sizeof(char),3U));_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL55;}_LL55:;
# 1144
goto _LL0;}case 6: _Tmp9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;struct Cyc_Absyn_Exp*e3=_Tmp5;
# 1146
s=({struct Cyc_PP_Doc*_TmpB[5];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" ? ",sizeof(char),4U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(0,e2);_TmpB[2]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e3);_TmpB[4]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 7: _Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1150
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" && ",sizeof(char),5U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 8: _Tmp9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1153
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" || ",sizeof(char),5U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 9: _Tmp9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1158
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec - 1,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(", ",sizeof(char),3U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec - 1,e2);_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 10: _Tmp9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_List_List*es=_Tmp6;
# 1161
s=({struct Cyc_PP_Doc*_TmpB[4];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exps2doc_prec(20,es);_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[3]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 11: _Tmp9=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("throw ",sizeof(char),7U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 12: _Tmp9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 13: _Tmp9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
s=Cyc_Absynpp_exp2doc_prec(inprec,e1);goto _LL0;}case 14: _Tmp9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp6;
# 1167
s=({struct Cyc_PP_Doc*_TmpB[4];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typ2doc(t);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[3]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 15: _Tmp9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("&",sizeof(char),2U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 16: _Tmp9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*ropt=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*qual=_Tmp5;
# 1170
if(ropt==0){
struct Cyc_PP_Doc*qd=Cyc_PP_nil_doc();
if((unsigned)qual)
qd=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(qual);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});
# 1175
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("new ",sizeof(char),5U));_TmpB[0]=_TmpC;}),_TmpB[1]=qd,({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});}else{
# 1178
struct Cyc_PP_Doc*qd=Cyc_PP_nil_doc();
if((unsigned)qual)
qd=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(qual);_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});
# 1182
s=({struct Cyc_PP_Doc*_TmpB[5];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("rnew(",sizeof(char),6U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(ropt);_TmpB[1]=_TmpC;}),_TmpB[2]=qd,({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[4]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),5));});}
# 1184
goto _LL0;}case 17: _Tmp9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp9;
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("sizeof(",sizeof(char),8U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typ2doc(t);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 18: _Tmp9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("sizeof(",sizeof(char),8U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e1);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 41: _Tmp9=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("@assert(",sizeof(char),9U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 40: _Tmp9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 1189
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("__extension__(",sizeof(char),15U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 38: _Tmp9=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp9;
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("valueof(",sizeof(char),9U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typ2doc(t);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 39: _Tmp3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp6=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;_Tmp5=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f5;{int vol=_Tmp3;struct _fat_ptr t=_Tmp8;struct Cyc_List_List*o=_Tmp9;struct Cyc_List_List*i=_Tmp6;struct Cyc_List_List*cl=_Tmp5;
# 1192
if(vol)
s=({struct Cyc_PP_Doc*_TmpB[7];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("__asm__",sizeof(char),8U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" volatile (",sizeof(char),12U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_TmpB[4]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_TmpB[5]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[6]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1195
s=({struct Cyc_PP_Doc*_TmpB[6];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("__asm__(",sizeof(char),9U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(Cyc_Absynpp_string_escape(t));_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_asm_iolist_doc(o,i,cl);_TmpB[4]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[5]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),6));});}
goto _LL0;}case 37: _Tmp9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp9;struct _fat_ptr*f=_Tmp6;
# 1198
s=({struct Cyc_PP_Doc*_TmpB[5];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("tagcheck(",sizeof(char),10U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(".",sizeof(char),2U));_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_textptr(f);_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[4]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 19: _Tmp9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp9;struct Cyc_List_List*l=_Tmp6;
# 1201
s=({struct Cyc_PP_Doc*_TmpB[5];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("offsetof(",sizeof(char),10U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typ2doc(t);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[2]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=({struct _fat_ptr _TmpD=_tag_fat(".",sizeof(char),2U);Cyc_PP_seq(_TmpD,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_offsetof_field_to_doc,l));});_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[4]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 20: _Tmp9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("*",sizeof(char),2U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 21: _Tmp9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*n=_Tmp6;
# 1206
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(".",sizeof(char),2U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_textptr(n);_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 22: _Tmp9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*n=_Tmp6;
# 1208
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("->",sizeof(char),3U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_textptr(n);_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 23: _Tmp9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1210
s=({struct Cyc_PP_Doc*_TmpB[4];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("[",sizeof(char),2U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e2);_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("]",sizeof(char),2U));_TmpB[3]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),4));});goto _LL0;}case 24: _Tmp9=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple9*vat=_Tmp9;struct Cyc_List_List*des=_Tmp6;
# 1212
s=({struct Cyc_PP_Doc*_TmpB[4];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typ2doc((*vat).f2);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=({struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpD,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});_TmpB[3]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),4));});
goto _LL0;}case 25: _Tmp9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*des=_Tmp9;
s=({struct _fat_ptr _TmpB=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpB,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});goto _LL0;}case 26: _Tmp9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;
# 1217
s=({struct Cyc_PP_Doc*_TmpB[8];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_lb();_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("for ",sizeof(char),5U));_TmpB[1]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(*(*vd->name).f1);_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" < ",sizeof(char),4U));_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e1);_TmpB[4]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_TmpB[5]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e2);_TmpB[6]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_rb();_TmpB[7]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),8));});
goto _LL0;}case 27: _Tmp9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp9;void*t=_Tmp6;
# 1222
s=({struct Cyc_PP_Doc*_TmpB[7];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_lb();_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("for x ",sizeof(char),7U));_TmpB[1]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" < ",sizeof(char),4U));_TmpB[2]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e);_TmpB[3]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" : ",sizeof(char),4U));_TmpB[4]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typ2doc(t);_TmpB[5]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_rb();_TmpB[6]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}case 28: _Tmp9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct _tuple1*n=_Tmp9;struct Cyc_List_List*ts=_Tmp6;struct Cyc_List_List*des=_Tmp5;
# 1230
struct Cyc_List_List*des_doc=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des);
s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_qvar2doc(n);_TmpB[0]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=({struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpD,
ts!=0?({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_PP_Doc*_TmpF=Cyc_Absynpp_tps2doc(ts);_TmpE->hd=_TmpF;}),_TmpE->tl=des_doc;_TmpE;}): des_doc);});
# 1232
_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});
# 1234
goto _LL0;}case 29: _Tmp3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{int is_tuple=_Tmp3;struct Cyc_List_List*des=_Tmp9;
# 1236
if(is_tuple)
s=({struct Cyc_PP_Doc*_TmpB[4];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_dollar();_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exps2doc_prec(20,({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd;}),des));_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[3]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1239
s=({struct _fat_ptr _TmpB=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpB,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});}
goto _LL0;}case 30: _Tmp9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp9;struct Cyc_Absyn_Datatypefield*tuf=_Tmp6;
# 1243
s=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_qvar2doc(tuf->name);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=({struct _fat_ptr _TmpD=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _TmpE=_tag_fat(")",sizeof(char),2U);struct _fat_ptr _TmpF=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_TmpD,_TmpE,_TmpF,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_exp2doc,es));});_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});
goto _LL0;}case 31: _Tmp9=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp9;
_Tmp9=ef;goto _LL4C;}case 32: _Tmp9=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL4C: {struct Cyc_Absyn_Enumfield*ef=_Tmp9;
s=Cyc_Absynpp_qvar2doc(ef->name);goto _LL0;}case 33: _Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_Tmp9=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp6=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp3=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.inline_call;{enum Cyc_Absyn_MallocKind mknd=_Tmp2;struct Cyc_Absyn_Exp*rgnopt=_Tmp9;struct Cyc_Absyn_Exp*aqopt=_Tmp6;void**topt=_Tmp5;struct Cyc_Absyn_Exp*e=_Tmp1;int inline_call=_Tmp3;
# 1250
if((int)mknd==1){
# 1252
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftype_exp(*_check_null(topt),0U);
if(rgnopt==0){
if(aqopt==0)
s=({struct Cyc_PP_Doc*_TmpB[5];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("calloc(",sizeof(char),8U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(st);_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[4]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),5));});else{
# 1257
s=({struct Cyc_PP_Doc*_TmpB[7];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("qcalloc(",sizeof(char),9U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(aqopt);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[2]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e);_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[4]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(st);_TmpB[5]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[6]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1260
s=({struct Cyc_PP_Doc*_TmpB[7];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("rcalloc(",sizeof(char),9U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(rgnopt);_TmpB[1]=_TmpC;}),
aqopt!=0?({struct Cyc_PP_Doc*_TmpC=({struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_exp2doc(aqopt);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpD[2]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));});_TmpB[2]=_TmpC;}):({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[2]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(e);_TmpB[3]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[4]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(st);_TmpB[5]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[6]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),7));});}}else{
# 1265
struct Cyc_PP_Doc*new_e=
Cyc_Absynpp_exp2doc(topt==0?e:({struct Cyc_Absyn_Exp*_TmpB=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_TmpB,e,0U);}));
struct _fat_ptr fn_name=_tag_fat("malloc(",sizeof(char),8U);
if(rgnopt==0){
if(aqopt==0)
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(fn_name);_TmpB[0]=_TmpC;}),_TmpB[1]=new_e,({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});else{
# 1272
fn_name=_tag_fat("qmalloc(",sizeof(char),9U);
s=({struct Cyc_PP_Doc*_TmpB[5];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(fn_name);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(aqopt);_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[2]=_TmpC;}),_TmpB[3]=new_e,({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[4]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),5));});}}else{
# 1277
if(inline_call)fn_name=_tag_fat("rmalloc_inline(",sizeof(char),16U);else{
if((int)mknd==2)fn_name=_tag_fat("rvmalloc(",sizeof(char),10U);else{
fn_name=_tag_fat("rmalloc(",sizeof(char),9U);}}
s=({struct Cyc_PP_Doc*_TmpB[5];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(fn_name);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc(rgnopt);_TmpB[1]=_TmpC;}),
aqopt!=0?({struct Cyc_PP_Doc*_TmpC=({struct Cyc_PP_Doc*_TmpD[3];({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_exp2doc(aqopt);_TmpD[1]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpD[2]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),3));});_TmpB[2]=_TmpC;}):({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(",",sizeof(char),2U));_TmpB[2]=_TmpC;}),_TmpB[3]=new_e,({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[4]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),5));});}}
# 1285
goto _LL0;}case 34: _Tmp9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 1287
s=({struct Cyc_PP_Doc*_TmpB[3];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e1);_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(" :=: ",sizeof(char),6U));_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_exp2doc_prec(myprec,e2);_TmpB[2]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),3));});
goto _LL0;}case 35: _Tmp9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*n=_Tmp9;struct Cyc_List_List*des=_Tmp6;
# 1290
s=({struct _fat_ptr _TmpB=_tag_fat(",",sizeof(char),2U);Cyc_Absynpp_group_braces(_TmpB,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_de2doc,des));});
goto _LL0;}default: _Tmp9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s2=_Tmp9;
# 1293
s=({struct Cyc_PP_Doc*_TmpB[7];({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_TmpB[0]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_lb();_TmpB[1]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_blank_doc();_TmpB[2]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(s2,1,0));_TmpB[3]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_PP_blank_doc();_TmpB[4]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_rb();_TmpB[5]=_TmpC;}),({struct Cyc_PP_Doc*_TmpC=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_TmpB[6]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}}_LL0:;}
# 1298
if(inprec >= myprec)
s=({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=s,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
return s;}
# 1302
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1306
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_Tmp0;if(*((int*)d)==0){_Tmp0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)d)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
return({struct Cyc_PP_Doc*_Tmp1[3];({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(_tag_fat(".[",sizeof(char),3U));_Tmp1[0]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_Absynpp_exp2doc(e);_Tmp1[1]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(_tag_fat("]",sizeof(char),2U));_Tmp1[2]=_Tmp2;});Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),3));});}}else{_Tmp0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)d)->f1;{struct _fat_ptr*v=_Tmp0;
return({struct Cyc_PP_Doc*_Tmp1[2];({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_text(_tag_fat(".",sizeof(char),2U));_Tmp1[0]=_Tmp2;}),({struct Cyc_PP_Doc*_Tmp2=Cyc_PP_textptr(v);_Tmp1[1]=_Tmp2;});Cyc_PP_cat(_tag_fat(_Tmp1,sizeof(struct Cyc_PP_Doc*),2));});}};}
# 1313
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple17*de){
if((*de).f0==0)return Cyc_Absynpp_exp2doc((*de).f1);
return({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp3=_tag_fat("=",sizeof(char),2U);struct _fat_ptr _Tmp4=_tag_fat("=",sizeof(char),2U);Cyc_PP_egroup(_Tmp2,_Tmp3,_Tmp4,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_designator2doc,(*de).f0));});_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc((*de).f1);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});}
# 1319
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
return({struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp1=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp2=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_Tmp0,_Tmp1,_Tmp2,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Absynpp_exp2doc_prec,inprec,es));});}
# 1323
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long i){
struct _fat_ptr x=_tag_fat(({unsigned _Tmp0=28U + 1U;({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=28U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='z';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;});}),sizeof(char),29U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),27);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),26);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='L';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),25);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='L';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),24);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='U';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),23);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});{
int index=23;
while(i!=0U){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(x,sizeof(char),index --);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(48U + i % 10U);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
i=i / 10U;}
# 1335
return _fat_ptr_plus(_fat_ptr_plus(x,sizeof(char),index),sizeof(char),1);}}
# 1339
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
long long _Tmp0;int _Tmp1;short _Tmp2;struct _fat_ptr _Tmp3;char _Tmp4;enum Cyc_Absyn_Sign _Tmp5;switch(c.String_c.tag){case 2: _Tmp5=c.Char_c.val.f0;_Tmp4=c.Char_c.val.f1;{enum Cyc_Absyn_Sign sg=_Tmp5;char ch=_Tmp4;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("'%s'",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));}));}case 3: _Tmp3=c.Wchar_c.val;{struct _fat_ptr s=_Tmp3;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("L'%s'",sizeof(char),6U),_tag_fat(_Tmp7,sizeof(void*),1));}));}case 4: _Tmp5=c.Short_c.val.f0;_Tmp2=c.Short_c.val.f1;{enum Cyc_Absyn_Sign sg=_Tmp5;short s=_Tmp2;
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)s);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp7,sizeof(void*),1));}));}case 5: _Tmp5=c.Int_c.val.f0;_Tmp1=c.Int_c.val.f1;{enum Cyc_Absyn_Sign sn=_Tmp5;int i=_Tmp1;
# 1345
if((int)sn==1)return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("%uU",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));}));
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp7,sizeof(void*),1));}));}case 6: _Tmp5=c.LongLong_c.val.f0;_Tmp0=c.LongLong_c.val.f1;{enum Cyc_Absyn_Sign sg=_Tmp5;long long i=_Tmp0;
return Cyc_PP_text(Cyc_Absynpp_longlong2string((unsigned long long)i));}case 7: _Tmp3=c.Float_c.val.f0;{struct _fat_ptr x=_Tmp3;
return Cyc_PP_text(x);}case 1:
 return Cyc_PP_text(_tag_fat("NULL",sizeof(char),5U));case 8: _Tmp3=c.String_c.val;{struct _fat_ptr s=_Tmp3;
return({struct Cyc_PP_Doc*_Tmp6[3];({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp6[0]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(Cyc_Absynpp_string_escape(s));_Tmp6[1]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp6[2]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),3));});}default: _Tmp3=c.Wstring_c.val;{struct _fat_ptr s=_Tmp3;
return({struct Cyc_PP_Doc*_Tmp6[3];({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat("L\"",sizeof(char),3U));_Tmp6[0]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(s);_Tmp6[1]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat("\"",sizeof(char),2U));_Tmp6[2]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),3));});}};}
# 1355
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if((int)p==(int)18U){
if(es==0 || es->tl!=0)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({
# 1359
(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Absynpp::primapp2doc Numelts: %s with bad args",sizeof(char),47U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 1361
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("numelts(",sizeof(char),9U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}{
# 1363
struct Cyc_List_List*ds=({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds==0)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Absynpp::primapp2doc: %s with no args",sizeof(char),38U),_tag_fat(_Tmp1,sizeof(void*),1));});
if(ds->tl==0)
return({struct Cyc_PP_Doc*_Tmp0[3];_Tmp0[0]=ps,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),_Tmp0[2]=(struct Cyc_PP_Doc*)ds->hd;Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});
if(ds->tl->tl!=0)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(ps,72));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({
# 1369
(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Absynpp::primapp2doc: %s with more than 2 args",sizeof(char),47U),_tag_fat(_Tmp1,sizeof(void*),1));});
# 1371
return({struct Cyc_PP_Doc*_Tmp0[5];_Tmp0[0]=(struct Cyc_PP_Doc*)ds->hd,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp0[1]=_Tmp1;}),_Tmp0[2]=ps,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp0[3]=_Tmp1;}),_Tmp0[4]=(struct Cyc_PP_Doc*)ds->tl->hd;Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),5));});}}
# 1374
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 return _tag_fat("+",sizeof(char),2U);case Cyc_Absyn_Times:
 return _tag_fat("*",sizeof(char),2U);case Cyc_Absyn_Minus:
 return _tag_fat("-",sizeof(char),2U);case Cyc_Absyn_Div:
 return _tag_fat("/",sizeof(char),2U);case Cyc_Absyn_Mod:
 return Cyc_Absynpp_print_for_cycdoc?_tag_fat("\\%",sizeof(char),3U): _tag_fat("%",sizeof(char),2U);case Cyc_Absyn_Eq:
 return _tag_fat("==",sizeof(char),3U);case Cyc_Absyn_Neq:
 return _tag_fat("!=",sizeof(char),3U);case Cyc_Absyn_Gt:
 return _tag_fat(">",sizeof(char),2U);case Cyc_Absyn_Lt:
 return _tag_fat("<",sizeof(char),2U);case Cyc_Absyn_Gte:
 return _tag_fat(">=",sizeof(char),3U);case Cyc_Absyn_Lte:
 return _tag_fat("<=",sizeof(char),3U);case Cyc_Absyn_Not:
 return _tag_fat("!",sizeof(char),2U);case Cyc_Absyn_Bitnot:
 return _tag_fat("~",sizeof(char),2U);case Cyc_Absyn_Bitand:
 return _tag_fat("&",sizeof(char),2U);case Cyc_Absyn_Bitor:
 return _tag_fat("|",sizeof(char),2U);case Cyc_Absyn_Bitxor:
 return _tag_fat("^",sizeof(char),2U);case Cyc_Absyn_Bitlshift:
 return _tag_fat("<<",sizeof(char),3U);case Cyc_Absyn_Bitlrshift:
 return _tag_fat(">>",sizeof(char),3U);case Cyc_Absyn_Numelts:
 return _tag_fat("numelts",sizeof(char),8U);default:
 return _tag_fat("?",sizeof(char),2U);};}
# 1398
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1402
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;if(*((int*)_Tmp0)==12)
return 1;else{
return 0;};}
# 1417 "absynpp.cyc"
struct _tuple15 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*d,struct Cyc_List_List*varsinblock){
void*_Tmp0=d->r;void*_Tmp1;if(*((int*)_Tmp0)==0){_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1420
if(({(int(*)(int(*)(struct _tuple1*,struct _tuple1*),struct Cyc_List_List*,struct _tuple1*))Cyc_List_mem;})(Cyc_Absyn_qvar_cmp,varsinblock,vd->name))
return({struct _tuple15 _Tmp2;_Tmp2.f0=1,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=vd->name,_Tmp4->tl=0;_Tmp4;});_Tmp2.f1=_Tmp3;});_Tmp2;});
return({struct _tuple15 _Tmp2;_Tmp2.f0=0,({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=vd->name,_Tmp4->tl=varsinblock;_Tmp4;});_Tmp2.f1=_Tmp3;});_Tmp2;});}}else{
return({struct _tuple15 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=varsinblock;_Tmp2;});};}
# 1427
struct Cyc_PP_Doc*Cyc_Absynpp_block(int stmtexp,struct Cyc_PP_Doc*d){
if(stmtexp)
return({struct Cyc_PP_Doc*_Tmp0[8];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,d);_Tmp0[3]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[4]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[5]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(");",sizeof(char),3U));_Tmp0[6]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[7]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),8));});
return({struct Cyc_PP_Doc*_Tmp0[6];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,d);_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[3]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[4]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[5]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),6));});}
# 1434
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp,struct Cyc_List_List*varsinblock){
void*_Tmp0=st->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 return Cyc_PP_text(_tag_fat(";",sizeof(char),2U));case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 1439
if(!Cyc_Absynpp_decls_first)
return({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,0);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,0);_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});
if(Cyc_Absynpp_is_declaration(s1))
return({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[0]=_Tmp6;}),
Cyc_Absynpp_is_declaration(s2)?({
struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_Tmp5[1]=_Tmp6;}):({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});
if(Cyc_Absynpp_is_declaration(s2))
return({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s2,stmtexp,0));});_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});
return({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,0,varsinblock);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s2,stmtexp,varsinblock);_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});}case 3: _Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp4;
# 1453
if(eopt==0)
return Cyc_PP_text(_tag_fat("return;",sizeof(char),8U));
return({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("return ",sizeof(char),8U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(eopt);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 1457
int print_else;
{void*_Tmp5=s2->r;if(*((int*)_Tmp5)==0){
print_else=0;goto _LL23;}else{
print_else=1;goto _LL23;}_LL23:;}
# 1462
return({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("if (",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[3]=_Tmp6;}),
print_else?({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_line_doc();_Tmp7[0]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat("else ",sizeof(char),6U));_Tmp7[1]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s2,0,0));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});
# 1467
_Tmp5[4]=_Tmp6;}):({
# 1470
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nil_doc();_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1472
return({struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("while (",sizeof(char),8U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[3]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});}case 6:
 return Cyc_PP_text(_tag_fat("break;",sizeof(char),7U));case 7:
 return Cyc_PP_text(_tag_fat("continue;",sizeof(char),10U));case 8: _Tmp4=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct _fat_ptr*x=_Tmp4;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)*x);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("goto %s;",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),1));}));}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 1478
return({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("for(",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e1);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("; ",sizeof(char),3U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e2);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("; ",sizeof(char),3U));_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e3);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 1487
return({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("switch (",sizeof(char),9U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(") ",sizeof(char),3U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_switchclauses2doc(ss);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1==0)
return Cyc_PP_text(_tag_fat("fallthru;",sizeof(char),10U));else{_Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp4;
# 1497
return({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("fallthru(",sizeof(char),10U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exps2doc_prec(20,es);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(");",sizeof(char),3U));_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});}}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1499
struct _tuple15 _Tmp5=Cyc_Absynpp_shadows(d,varsinblock);void*_Tmp6;int _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{int newblock=_Tmp7;struct Cyc_List_List*newvarsinblock=_Tmp6;
struct Cyc_PP_Doc*s=({struct Cyc_PP_Doc*_Tmp8[3];({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_decl2doc(d);_Tmp8[0]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_PP_line_doc();_Tmp8[1]=_Tmp9;}),({struct Cyc_PP_Doc*_Tmp9=Cyc_Absynpp_stmt2doc(s1,stmtexp,newvarsinblock);_Tmp8[2]=_Tmp9;});Cyc_PP_cat(_tag_fat(_Tmp8,sizeof(struct Cyc_PP_Doc*),3));});
if(newblock)s=Cyc_Absynpp_block(stmtexp,s);
return s;}}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*x=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1504
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(s1))
return({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(x);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({int _Tmp7=stmtexp;Cyc_Absynpp_block(_Tmp7,Cyc_Absynpp_stmt2doc(s1,stmtexp,0));});_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});
return({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(x);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_stmt2doc(s1,stmtexp,varsinblock);_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 1508
return({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("do ",sizeof(char),4U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" while (",sizeof(char),9U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(");",sizeof(char),3U));_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});}default: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 1514
return({struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("try ",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_stmt2doc(s1,0,0));_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" catch ",sizeof(char),8U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_block(0,Cyc_Absynpp_switchclauses2doc(ss));_Tmp5[3]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});}};}
# 1521
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct _fat_ptr _Tmp5;char _Tmp6;int _Tmp7;enum Cyc_Absyn_Sign _Tmp8;switch(*((int*)_Tmp0)){case 0:
 return Cyc_PP_text(_tag_fat("_",sizeof(char),2U));case 8:
 return Cyc_PP_text(_tag_fat("NULL",sizeof(char),5U));case 9: _Tmp8=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign sg=_Tmp8;int i=_Tmp7;
# 1526
if((int)sg!=(int)1U)
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned long)i;_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_TmpA,sizeof(void*),1));}));
return Cyc_PP_text(({struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned)i;_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_aprintf(_tag_fat("%u",sizeof(char),3U),_tag_fat(_TmpA,sizeof(void*),1));}));}case 10: _Tmp6=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char ch=_Tmp6;
return Cyc_PP_text(({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_char_escape(ch));_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_aprintf(_tag_fat("'%s'",sizeof(char),5U),_tag_fat(_TmpA,sizeof(void*),1));}));}case 11: _Tmp5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _fat_ptr x=_Tmp5;
return Cyc_PP_text(x);}case 1: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return Cyc_Absynpp_qvar2doc(vd->name);}}else{_Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp3;
return({struct Cyc_PP_Doc*_Tmp9[3];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(vd->name);_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(" as ",sizeof(char),5U));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_pat2doc(p2);_Tmp9[2]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),3));});}}case 2: _Tmp4=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1534
return({struct Cyc_PP_Doc*_Tmp9[5];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("alias",sizeof(char),6U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(" <",sizeof(char),3U));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_tvar2doc(tv);_Tmp9[2]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("> ",sizeof(char),3U));_Tmp9[3]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_vardecl2doc(vd);_Tmp9[4]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),5));});}case 4: _Tmp4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1536
return({struct Cyc_PP_Doc*_Tmp9[4];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(vd->name);_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("<",sizeof(char),2U));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_tvar2doc(tv);_Tmp9[2]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_Tmp9[3]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),4));});}case 5: _Tmp4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp4;
return({struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("&",sizeof(char),2U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_pat2doc(p2);_Tmp9[1]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 1539
return({struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("*",sizeof(char),2U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(vd->name);_Tmp9[1]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});}}else{_Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp3;
# 1541
return({struct Cyc_PP_Doc*_Tmp9[4];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("*",sizeof(char),2U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(vd->name);_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(" as ",sizeof(char),5U));_Tmp9[2]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_pat2doc(p2);_Tmp9[3]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),4));});}}case 14: _Tmp4=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple1*q=_Tmp4;
return Cyc_Absynpp_qvar2doc(q);}case 15: _Tmp4=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct _tuple1*q=_Tmp4;struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp7;
# 1544
struct _fat_ptr term=dots?_tag_fat(", ...)",sizeof(char),7U): _tag_fat(")",sizeof(char),2U);
return({struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(q);_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _TmpC=term;struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_TmpB,_TmpC,_TmpD,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_pat2doc,ps));});_Tmp9[1]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});}case 6: _Tmp4=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f5;{void*topt=_Tmp4;int is_tuple=_Tmp7;struct Cyc_List_List*exists=_Tmp3;struct Cyc_List_List*dps=_Tmp2;int dots=_Tmp1;
# 1547
if(is_tuple)
return({struct Cyc_PP_Doc*_Tmp9[4];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_dollar();_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat("(",sizeof(char),2U));_Tmp9[1]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat(",",sizeof(char),2U);({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;})(Cyc_Absynpp_pat2doc,_TmpB,({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Pat*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Pat*(*)(struct _tuple16*))Cyc_Core_snd;}),dps));});_Tmp9[2]=_TmpA;}),
dots?({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(", ...)",sizeof(char),7U));_Tmp9[3]=_TmpA;}):({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp9[3]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),4));});else{
# 1551
struct _fat_ptr term=dots?_tag_fat(", ...}",sizeof(char),7U): _tag_fat("}",sizeof(char),2U);
if(topt!=0){
void*_Tmp9=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _TmpA;if(*((int*)_Tmp9)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f1)==25){_TmpA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_TmpA;
# 1555
struct _tuple12 _TmpB=Cyc_Absyn_aggr_kinded_name(info);void*_TmpC;_TmpC=_TmpB.f1;{struct _tuple1*n=_TmpC;
return({struct Cyc_PP_Doc*_TmpD[4];({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_qvar2doc(n);_TmpD[0]=_TmpE;}),({struct Cyc_PP_Doc*_TmpE=Cyc_Absynpp_lb();_TmpD[1]=_TmpE;}),({
struct Cyc_PP_Doc*_TmpE=({struct _fat_ptr _TmpF=_tag_fat("[",sizeof(char),2U);struct _fat_ptr _Tmp10=_tag_fat("]",sizeof(char),2U);struct _fat_ptr _Tmp11=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_TmpF,_Tmp10,_Tmp11,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_tvar2doc,exists));});_TmpD[2]=_TmpE;}),({
struct Cyc_PP_Doc*_TmpE=({struct _fat_ptr _TmpF=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp10=term;struct _fat_ptr _Tmp11=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_TmpF,_Tmp10,_Tmp11,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_dp2doc,dps));});_TmpD[3]=_TmpE;});Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),4));});}}}else{goto _LL2C;}}else{_LL2C:
 goto _LL29;}_LL29:;}
# 1562
return({struct Cyc_PP_Doc*_Tmp9[3];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_lb();_Tmp9[0]=_TmpA;}),({
struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat("[",sizeof(char),2U);struct _fat_ptr _TmpC=_tag_fat("]",sizeof(char),2U);struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_TmpB,_TmpC,_TmpD,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_tvar2doc,exists));});_Tmp9[1]=_TmpA;}),({
struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat("",sizeof(char),1U);struct _fat_ptr _TmpC=term;struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_PP_group(_TmpB,_TmpC,_TmpD,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_dp2doc,dps));});_Tmp9[2]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),3));});}}case 12: _Tmp4=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp4;
# 1566
_Tmp4=ef;goto _LL22;}case 13: _Tmp4=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL22: {struct Cyc_Absyn_Enumfield*ef=_Tmp4;
return Cyc_Absynpp_qvar2doc(ef->name);}case 7: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3==0){_Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;
return Cyc_Absynpp_qvar2doc(tuf->name);}}else{_Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp7;
# 1570
struct _fat_ptr term=dots?_tag_fat(", ...)",sizeof(char),7U): _tag_fat(")",sizeof(char),2U);
return({struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_qvar2doc(tuf->name);_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=({struct _fat_ptr _TmpB=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _TmpC=term;struct _fat_ptr _TmpD=_tag_fat(",",sizeof(char),2U);Cyc_PP_egroup(_TmpB,_TmpC,_TmpD,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_pat2doc,ps));});_Tmp9[1]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});}}default: _Tmp4=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return Cyc_Absynpp_exp2doc(e);}};}
# 1576
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple16*dp){
return({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=({struct _fat_ptr _Tmp2=_tag_fat("",sizeof(char),1U);struct _fat_ptr _Tmp3=_tag_fat("=",sizeof(char),2U);struct _fat_ptr _Tmp4=_tag_fat("=",sizeof(char),2U);Cyc_PP_egroup(_Tmp2,_Tmp3,_Tmp4,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_designator2doc,(*dp).f0));});_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc((*dp).f1);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});}
# 1581
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
# 1583
struct Cyc_PP_Doc*body=Cyc_Absynpp_stmt2doc(c->body,0,0);
if(Cyc_Absynpp_decls_first && Cyc_Absynpp_is_declaration(c->body))
body=Cyc_Absynpp_block(0,body);
if(c->where_clause==0 && c->pattern->r==(void*)& Cyc_Absyn_Wild_p_val)
return({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("default: ",sizeof(char),10U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));});
if(c->where_clause==0)
return({struct Cyc_PP_Doc*_Tmp0[4];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("case ",sizeof(char),6U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc(c->pattern);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
return({struct Cyc_PP_Doc*_Tmp0[6];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("case ",sizeof(char),6U));_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_pat2doc(c->pattern);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" && ",sizeof(char),5U));_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(_check_null(c->where_clause));_Tmp0[3]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(": ",sizeof(char),3U));_Tmp0[4]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),_Tmp2[1]=body;Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[5]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),6));});}
# 1602
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Switch_clause*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_switchclause2doc,_tag_fat("",sizeof(char),1U),cs);}
# 1606
static struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag==0)
return Cyc_Absynpp_qvar2doc(f->name);
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2doc(f->name);_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(" = ",sizeof(char),4U));_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(_check_null(f->tag));_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
# 1611
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Enumfield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_enumfield2doc,_tag_fat(",",sizeof(char),2U),fs);}
# 1615
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1618
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Vardecl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseq;})(Cyc_Absynpp_id2doc,_tag_fat(",",sizeof(char),2U),vds);}
# 1622
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
void*_Tmp0;void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;unsigned _Tmp4;void*_Tmp5;enum Cyc_Absyn_Scope _Tmp6;_Tmp6=vd->sc;_Tmp5=vd->name;_Tmp4=vd->varloc;_Tmp3=vd->tq;_Tmp2=vd->type;_Tmp1=vd->initializer;_Tmp0=vd->attributes;{enum Cyc_Absyn_Scope sc=_Tmp6;struct _tuple1*name=_Tmp5;unsigned varloc=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*type=_Tmp2;struct Cyc_Absyn_Exp*initializer=_Tmp1;struct Cyc_List_List*atts=_Tmp0;
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(name);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(atts);
struct Cyc_PP_Doc*beforenamedoc;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
beforenamedoc=attsdoc;goto _LL3;}else{
# 1631
{void*_Tmp7=Cyc_Absyn_compress(type);void*_Tmp8;if(*((int*)_Tmp7)==5){_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp7)->f1.attributes;{struct Cyc_List_List*atts2=_Tmp8;
# 1633
beforenamedoc=Cyc_Absynpp_callconv2doc(atts2);
goto _LL8;}}else{
beforenamedoc=Cyc_PP_nil_doc();goto _LL8;}_LL8:;}
# 1637
goto _LL3;}_LL3:;{
# 1640
struct Cyc_PP_Doc*tmp_doc;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
tmp_doc=Cyc_PP_nil_doc();goto _LLD;}else{
tmp_doc=attsdoc;goto _LLD;}_LLD:;
# 1645
s=({struct Cyc_PP_Doc*_Tmp7[5];_Tmp7[0]=tmp_doc,({
struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_scope2doc(sc);_Tmp7[1]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=({struct Cyc_Absyn_Tqual _Tmp9=tq;void*_TmpA=type;Cyc_Absynpp_tqtd2doc(_Tmp9,_TmpA,({struct Cyc_Core_Opt*_TmpB=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_TmpC=({struct Cyc_PP_Doc*_TmpD[2];_TmpD[0]=beforenamedoc,_TmpD[1]=sn;Cyc_PP_cat(_tag_fat(_TmpD,sizeof(struct Cyc_PP_Doc*),2));});_TmpB->v=_TmpC;});_TmpB;}));});_Tmp7[2]=_Tmp8;}),
initializer==0?({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_nil_doc();_Tmp7[3]=_Tmp8;}):({struct Cyc_PP_Doc*_Tmp8=({struct Cyc_PP_Doc*_Tmp9[2];({struct Cyc_PP_Doc*_TmpA=Cyc_PP_text(_tag_fat(" = ",sizeof(char),4U));_Tmp9[0]=_TmpA;}),({struct Cyc_PP_Doc*_TmpA=Cyc_Absynpp_exp2doc(initializer);_Tmp9[1]=_TmpA;});Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});_Tmp7[3]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp7[4]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),5));});
return s;}}}struct _tuple20{unsigned f0;struct _tuple1*f1;int f2;};
# 1653
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple20*x){
return Cyc_Absynpp_qvar2doc((*x).f1);}
# 1657
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl==0)
return({struct Cyc_PP_Doc*_Tmp0[4];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(ad->sc);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_aggr_kind2doc(ad->kind);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2bolddoc(ad->name);_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc(ad->tvs);_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});
return({struct Cyc_PP_Doc*_Tmp0[13];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(ad->sc);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_aggr_kind2doc(ad->kind);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_qvar2bolddoc(ad->name);_Tmp0[2]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc(ad->tvs);_Tmp0[3]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_blank_doc();_Tmp0[4]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_lb();_Tmp0[5]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_ktvars2doc(ad->impl->exist_vars);_Tmp0[6]=_Tmp1;}),
ad->impl->rgn_po==0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nil_doc();_Tmp0[7]=_Tmp1;}):({
struct Cyc_PP_Doc*_Tmp1=({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(_tag_fat(":",sizeof(char),2U));_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_rgnpo2doc(ad->impl->rgn_po);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));});_Tmp0[7]=_Tmp1;}),
ad->impl->qual_bnd==0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nil_doc();_Tmp0[8]=_Tmp1;}):({
struct Cyc_PP_Doc*_Tmp1=({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_text(ad->impl->rgn_po==0?_tag_fat(":",sizeof(char),2U): _tag_fat(",",sizeof(char),2U));_Tmp2[0]=_Tmp3;}),({
struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_qualbnd2doc(ad->impl->qual_bnd);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));});
# 1672
_Tmp0[8]=_Tmp1;}),({
# 1674
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp2[2];({struct Cyc_PP_Doc*_Tmp3=Cyc_PP_line_doc();_Tmp2[0]=_Tmp3;}),({struct Cyc_PP_Doc*_Tmp3=Cyc_Absynpp_aggrfields2doc(ad->impl->fields);_Tmp2[1]=_Tmp3;});Cyc_PP_cat(_tag_fat(_Tmp2,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp0[9]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[10]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_rb();_Tmp0[11]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_atts2doc(ad->attributes);_Tmp0[12]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),13));});}
# 1680
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
int _Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;enum Cyc_Absyn_Scope _Tmp4;_Tmp4=dd->sc;_Tmp3=dd->name;_Tmp2=dd->tvs;_Tmp1=dd->fields;_Tmp0=dd->is_extensible;{enum Cyc_Absyn_Scope sc=_Tmp4;struct _tuple1*name=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_Core_Opt*fields=_Tmp1;int is_x=_Tmp0;
if(fields==0)
return({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_scope2doc(sc);_Tmp5[0]=_Tmp6;}),
is_x?({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("@extensible ",sizeof(char),13U));_Tmp5[1]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("datatype ",sizeof(char),10U));_Tmp5[2]=_Tmp6;}),
is_x?({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2bolddoc(name);_Tmp5[3]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_typedef_name2bolddoc(name);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_ktvars2doc(tvs);_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
return({struct Cyc_PP_Doc*_Tmp5[10];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_scope2doc(sc);_Tmp5[0]=_Tmp6;}),
is_x?({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("@extensible ",sizeof(char),13U));_Tmp5[1]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("datatype ",sizeof(char),10U));_Tmp5[2]=_Tmp6;}),
is_x?({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2bolddoc(name);_Tmp5[3]=_Tmp6;}):({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_typedef_name2bolddoc(name);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_ktvars2doc(tvs);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[5]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp7[2];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_line_doc();_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)fields->v);_Tmp7[1]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp5[7]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[8]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[9]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),10));});}}
# 1699
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=ed->sc;_Tmp1=ed->name;_Tmp0=ed->fields;{enum Cyc_Absyn_Scope sc=_Tmp2;struct _tuple1*n=_Tmp1;struct Cyc_Core_Opt*fields=_Tmp0;
if(fields==0)
return({struct Cyc_PP_Doc*_Tmp3[3];({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_scope2doc(sc);_Tmp3[0]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat("enum ",sizeof(char),6U));_Tmp3[1]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_typedef_name2bolddoc(n);_Tmp3[2]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),3));});
return({struct Cyc_PP_Doc*_Tmp3[8];({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_scope2doc(sc);_Tmp3[0]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_PP_text(_tag_fat("enum ",sizeof(char),6U));_Tmp3[1]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_qvar2bolddoc(n);_Tmp3[2]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_PP_blank_doc();_Tmp3[3]=_Tmp4;}),({struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_lb();_Tmp3[4]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)fields->v);_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp3[5]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_PP_line_doc();_Tmp3[6]=_Tmp4;}),({
struct Cyc_PP_Doc*_Tmp4=Cyc_Absynpp_rb();_Tmp3[7]=_Tmp4;});Cyc_PP_cat(_tag_fat(_Tmp3,sizeof(struct Cyc_PP_Doc*),8));});}}
# 1712
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 1716
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp5->tag=5,_Tmp5->f1=type_info;_Tmp5;});
if(fd->cached_type!=0){
void*_Tmp5=Cyc_Absyn_compress(_check_null(fd->cached_type));struct Cyc_Absyn_FnInfo _Tmp6;if(*((int*)_Tmp5)==5){_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp5)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp6;
# 1722
({struct Cyc_List_List*_Tmp7=Cyc_List_append(fd->i.attributes,i.attributes);fd->i.attributes=_Tmp7;});goto _LL23;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("function has non-function type",sizeof(char),31U),_tag_fat(0U,sizeof(void*),0));}_LL23:;}{
# 1725
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(fd->i.attributes);
struct Cyc_PP_Doc*inlinedoc;
if(fd->is_inline){
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
inlinedoc=Cyc_PP_text(_tag_fat("inline ",sizeof(char),8U));goto _LL28;}else{
inlinedoc=Cyc_PP_text(_tag_fat("__inline ",sizeof(char),10U));goto _LL28;}_LL28:;}else{
# 1733
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(fd->sc);
struct Cyc_PP_Doc*beforenamedoc;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
beforenamedoc=attsdoc;goto _LL2D;}else{
beforenamedoc=Cyc_Absynpp_callconv2doc(fd->i.attributes);goto _LL2D;}_LL2D:;{
# 1740
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(fd->name);
struct Cyc_PP_Doc*tqtddoc=({struct Cyc_Absyn_Tqual _Tmp5=Cyc_Absyn_empty_tqual(0U);void*_Tmp6=t;Cyc_Absynpp_tqtd2doc(_Tmp5,_Tmp6,({struct Cyc_Core_Opt*_Tmp7=_cycalloc(sizeof(struct Cyc_Core_Opt));({
struct Cyc_PP_Doc*_Tmp8=({struct Cyc_PP_Doc*_Tmp9[2];_Tmp9[0]=beforenamedoc,_Tmp9[1]=namedoc;Cyc_PP_cat(_tag_fat(_Tmp9,sizeof(struct Cyc_PP_Doc*),2));});_Tmp7->v=_Tmp8;});_Tmp7;}));});
# 1748
struct Cyc_PP_Doc*bodydoc=({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nest(2,({struct Cyc_PP_Doc*_Tmp7[2];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_line_doc();_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_stmt2doc(fd->body,0,0);_Tmp7[1]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),2));}));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
s=({struct Cyc_PP_Doc*_Tmp5[4];_Tmp5[0]=inlinedoc,_Tmp5[1]=scopedoc,_Tmp5[2]=tqtddoc,_Tmp5[3]=bodydoc;Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});
if(Cyc_Flags_c_compiler==Cyc_Flags_Vc_c){
s=({struct Cyc_PP_Doc*_Tmp5[2];_Tmp5[0]=attsdoc,_Tmp5[1]=s;Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL32;}else{
goto _LL32;}_LL32:;
# 1757
goto _LL0;}}}}}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 1759
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_aggrdecl2doc(ad);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
s=Cyc_Absynpp_vardecl2doc(vd);goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp4;
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_datatypedecl2doc(dd);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp4;
s=({struct Cyc_PP_Doc*_Tmp5[3];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("let ",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_ids2doc(vds);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[2]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),3));});goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_enumdecl2doc(ed);_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp2;
# 1765
s=({struct Cyc_PP_Doc*_Tmp5[7];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("region",sizeof(char),7U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("<",sizeof(char),2U));_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_tvar2doc(tv);_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(">",sizeof(char),2U));_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(vd->name);_Tmp5[4]=_Tmp6;}),
(unsigned)open_exp_opt?({struct Cyc_PP_Doc*_Tmp6=({struct Cyc_PP_Doc*_Tmp7[3];({struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat(" = open(",sizeof(char),9U));_Tmp7[0]=_Tmp8;}),({struct Cyc_PP_Doc*_Tmp8=Cyc_Absynpp_exp2doc(open_exp_opt);_Tmp7[1]=_Tmp8;}),({
struct Cyc_PP_Doc*_Tmp8=Cyc_PP_text(_tag_fat(")",sizeof(char),2U));_Tmp7[2]=_Tmp8;});Cyc_PP_cat(_tag_fat(_Tmp7,sizeof(struct Cyc_PP_Doc*),3));});
# 1770
_Tmp5[5]=_Tmp6;}):({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_nil_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[6]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),7));});
goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 1775
s=({struct Cyc_PP_Doc*_Tmp5[5];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("let ",sizeof(char),5U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_pat2doc(p);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" = ",sizeof(char),4U));_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_exp2doc(e);_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[4]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),5));});
goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 1778
void*t=td->defn!=0?_check_null(td->defn): Cyc_Absyn_new_evar(td->kind,0);
s=({struct Cyc_PP_Doc*_Tmp5[4];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("typedef ",sizeof(char),9U));_Tmp5[0]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({struct Cyc_Absyn_Tqual _Tmp7=td->tq;void*_Tmp8=t;Cyc_Absynpp_tqtd2doc(_Tmp7,_Tmp8,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));({
# 1782
struct Cyc_PP_Doc*_TmpA=({struct Cyc_PP_Doc*_TmpB[2];({struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_typedef_name2bolddoc(td->name);_TmpB[0]=_TmpC;}),({
struct Cyc_PP_Doc*_TmpC=Cyc_Absynpp_tvars2doc(td->tvs);_TmpB[1]=_TmpC;});Cyc_PP_cat(_tag_fat(_TmpB,sizeof(struct Cyc_PP_Doc*),2));});
# 1782
_Tmp9->v=_TmpA;});_Tmp9;}));});
# 1780
_Tmp5[1]=_Tmp6;}),({
# 1785
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_atts2doc(td->atts);_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp5[3]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),4));});
# 1788
goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 1790
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(v);
s=({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("namespace ",sizeof(char),11U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_textptr(v);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _tuple1*q=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 1799
if(Cyc_Absynpp_print_using_stmts)
s=({struct Cyc_PP_Doc*_Tmp5[8];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("using ",sizeof(char),7U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(q);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),8));});else{
# 1806
s=({struct Cyc_PP_Doc*_Tmp5[11];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* using ",sizeof(char),10U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_qvar2doc(q);_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_blank_doc();_Tmp5[2]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[6]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[7]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* ",sizeof(char),4U));_Tmp5[8]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[9]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[10]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),11));});}
goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*tdl=_Tmp4;
# 1813
if(Cyc_Absynpp_print_externC_stmts)
s=({struct Cyc_PP_Doc*_Tmp5[6];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("extern \"C\" ",sizeof(char),12U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[5]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),6));});else{
# 1819
s=({struct Cyc_PP_Doc*_Tmp5[9];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* extern \"C\" ",sizeof(char),15U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* ",sizeof(char),4U));_Tmp5[6]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[8]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),9));});}
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List*tdl=_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exs=_Tmp2;struct _tuple11*wc=_Tmp1;
# 1826
if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
struct Cyc_PP_Doc*ovrs_doc;
if(exs!=0)
exs_doc=({struct Cyc_PP_Doc*_Tmp5[7];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" export ",sizeof(char),9U));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct _tuple20*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_export2doc,_tag_fat(",",sizeof(char),2U),exs);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[6]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1834
exs_doc=Cyc_Absynpp_rb();}
if(ovrs!=0)
ovrs_doc=({struct Cyc_PP_Doc*_Tmp5[7];({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" cycdef ",sizeof(char),9U));_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),ovrs);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[6]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),7));});else{
# 1840
ovrs_doc=Cyc_Absynpp_rb();}
s=({struct Cyc_PP_Doc*_Tmp5[6];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("extern \"C include\" ",sizeof(char),20U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[4]=_Tmp6;}),_Tmp5[5]=exs_doc;Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),6));});}else{
# 1847
s=({struct Cyc_PP_Doc*_Tmp5[9];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* extern \"C include\" ",sizeof(char),23U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[2]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[3]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_decl2doc,_tag_fat("",sizeof(char),1U),tdl);_Tmp5[4]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_line_doc();_Tmp5[5]=_Tmp6;}),({
struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("/* ",sizeof(char),4U));_Tmp5[6]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_rb();_Tmp5[7]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat(" */",sizeof(char),4U));_Tmp5[8]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),9));});}
goto _LL0;}case 13:
# 1854
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("__cyclone_port_on__;",sizeof(char),21U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;case 14:
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("__cyclone_port_off__;",sizeof(char),22U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;case 15:
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("__tempest_on__;",sizeof(char),16U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;default:
 s=({struct Cyc_PP_Doc*_Tmp5[2];({struct Cyc_PP_Doc*_Tmp6=Cyc_PP_text(_tag_fat("__tempest_off__;",sizeof(char),17U));_Tmp5[0]=_Tmp6;}),({struct Cyc_PP_Doc*_Tmp6=Cyc_Absynpp_lb();_Tmp5[1]=_Tmp6;});Cyc_PP_cat(_tag_fat(_Tmp5,sizeof(struct Cyc_PP_Doc*),2));});goto _LL0;}_LL0:;}
# 1859
return s;}
# 1863
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
return Cyc_Tcutil_is_temp_tvar(tv);}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==10){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*l=_Tmp1;
return Cyc_List_exists(Cyc_Absynpp_exists_temp_tvar_in_effect,l);}}else{goto _LL5;}default: _LL5:
 return 0;};}
# 1875
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_Tmp0;void*_Tmp1;switch(*((int*)t)){case 6:
 goto _LL4;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==21){_LL4:
 return 1;}else{goto _LL7;}case 7: _Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;if((void*)_Tmp0!=0){struct Cyc_Absyn_Typedefdecl*td=_Tmp1;void*x=_Tmp0;
# 1882
return Cyc_Absynpp_is_anon_aggrtype(x);}else{goto _LL7;}default: _LL7:
 return 0;};}
# 1889
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1892
if(tms!=0 && tms->tl!=0){
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=(void*)tms->hd,_Tmp1.f1=(void*)tms->tl->hd;_Tmp1;});if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==3)
# 1895
return({struct Cyc_List_List*_Tmp1=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp3->hd=(void*)tms->tl->hd,({struct Cyc_List_List*_Tmp4=Cyc_Absynpp_bubble_attributes(r,atts,tms->tl->tl);_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1->tl=_Tmp2;});_Tmp1;});else{goto _LL3;}}else{_LL3:
 return({struct Cyc_List_List*_Tmp1=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=atts,_Tmp1->tl=tms;_Tmp1;});};}
# 1898
return({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=atts,_Tmp0->tl=tms;_Tmp0;});}
# 1901
static void Cyc_Absynpp_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr s=Cyc_strconcat(_tag_fat("`",sizeof(char),2U),*t->name);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='t';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));*_Tmp1=(struct _fat_ptr)s;_Tmp1;});t->name=_Tmp0;});}}
# 1910
struct _tuple14 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1912
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;int _Tmp7;void*_Tmp8;struct Cyc_Absyn_PtrAtts _Tmp9;unsigned _TmpA;void*_TmpB;void*_TmpC;struct Cyc_Absyn_Tqual _TmpD;void*_TmpE;switch(*((int*)t)){case 4: _TmpE=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_TmpD=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;_TmpC=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;_TmpB=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;_TmpA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*t2=_TmpE;struct Cyc_Absyn_Tqual tq2=_TmpD;struct Cyc_Absyn_Exp*e=_TmpC;void*zeroterm=_TmpB;unsigned ztl=_TmpA;
# 1915
struct _tuple14 _TmpF=Cyc_Absynpp_to_tms(r,tq2,t2);void*_Tmp10;void*_Tmp11;struct Cyc_Absyn_Tqual _Tmp12;_Tmp12=_TmpF.f0;_Tmp11=_TmpF.f1;_Tmp10=_TmpF.f2;{struct Cyc_Absyn_Tqual tq3=_Tmp12;void*t3=_Tmp11;struct Cyc_List_List*tml3=_Tmp10;
void*tm;
if(e==0)
tm=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp13=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp13->tag=0,_Tmp13->f1=zeroterm,_Tmp13->f2=ztl;_Tmp13;});else{
# 1920
tm=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp13=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp13->tag=1,_Tmp13->f1=e,_Tmp13->f2=zeroterm,_Tmp13->f3=ztl;_Tmp13;});}
return({struct _tuple14 _Tmp13;_Tmp13.f0=tq3,_Tmp13.f1=t3,({struct Cyc_List_List*_Tmp14=({struct Cyc_List_List*_Tmp15=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp15->hd=tm,_Tmp15->tl=tml3;_Tmp15;});_Tmp13.f2=_Tmp14;});_Tmp13;});}}case 3: _TmpE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_TmpD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts;{void*t2=_TmpE;struct Cyc_Absyn_Tqual tq2=_TmpD;struct Cyc_Absyn_PtrAtts ptratts=_Tmp9;
# 1924
struct _tuple14 _TmpF=Cyc_Absynpp_to_tms(r,tq2,t2);void*_Tmp10;void*_Tmp11;struct Cyc_Absyn_Tqual _Tmp12;_Tmp12=_TmpF.f0;_Tmp11=_TmpF.f1;_Tmp10=_TmpF.f2;{struct Cyc_Absyn_Tqual tq3=_Tmp12;void*t3=_Tmp11;struct Cyc_List_List*tml3=_Tmp10;
tml3=({struct Cyc_List_List*_Tmp13=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp14=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_Tmp15=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_Tmp15->tag=2,_Tmp15->f1=ptratts,_Tmp15->f2=tq;_Tmp15;});_Tmp13->hd=_Tmp14;}),_Tmp13->tl=tml3;_Tmp13;});
return({struct _tuple14 _Tmp13;_Tmp13.f0=tq3,_Tmp13.f1=t3,_Tmp13.f2=tml3;_Tmp13;});}}case 5: _TmpE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.tvars;_TmpC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.effect;_TmpD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ret_tqual;_TmpB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ret_type;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.args;_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.c_varargs;_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.cyc_varargs;_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.rgn_po;_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.qual_bnd;_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.attributes;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.requires_clause;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ensures_clause;_Tmp0=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.throws_clause;{struct Cyc_List_List*typvars=_TmpE;void*effopt=_TmpC;struct Cyc_Absyn_Tqual t2qual=_TmpD;void*t2=_TmpB;struct Cyc_List_List*args=_Tmp8;int c_varargs=_Tmp7;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp6;struct Cyc_List_List*rgn_po=_Tmp5;struct Cyc_List_List*qb=_Tmp4;struct Cyc_List_List*fn_atts=_Tmp3;struct Cyc_Absyn_Exp*req=_Tmp2;struct Cyc_Absyn_Exp*ens=_Tmp1;struct Cyc_Absyn_Exp*thrws=_Tmp0;
# 1930
if(!Cyc_Absynpp_print_all_tvars){
# 1934
if(effopt==0 || Cyc_Absynpp_exists_temp_tvar_in_effect(effopt)){
effopt=0;
typvars=0;}}else{
# 1938
if(Cyc_Absynpp_rewrite_temp_tvars)
# 1941
({(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_iter;})(Cyc_Absynpp_rewrite_temp_tvar,typvars);}{
# 1944
struct _tuple14 _TmpF=Cyc_Absynpp_to_tms(r,t2qual,t2);void*_Tmp10;void*_Tmp11;struct Cyc_Absyn_Tqual _Tmp12;_Tmp12=_TmpF.f0;_Tmp11=_TmpF.f1;_Tmp10=_TmpF.f2;{struct Cyc_Absyn_Tqual tq3=_Tmp12;void*t3=_Tmp11;struct Cyc_List_List*tml3=_Tmp10;
struct Cyc_List_List*tms=tml3;
# 1955 "absynpp.cyc"
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
# 1959
if(fn_atts!=0)
tms=({struct _RegionHandle*_Tmp13=r;void*_Tmp14=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp15=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp15->tag=5,_Tmp15->f1=0U,_Tmp15->f2=fn_atts;_Tmp15;});Cyc_Absynpp_bubble_attributes(_Tmp13,_Tmp14,tms);});
tms=({struct Cyc_List_List*_Tmp13=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp14=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp15=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));
_Tmp15->tag=3,({void*_Tmp16=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp17=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp17->tag=1,_Tmp17->f1=args,_Tmp17->f2=c_varargs,_Tmp17->f3=cyc_varargs,_Tmp17->f4=effopt,_Tmp17->f5=rgn_po,_Tmp17->f6=qb,_Tmp17->f7=req,_Tmp17->f8=ens,_Tmp17->f9=thrws;_Tmp17;});_Tmp15->f1=_Tmp16;});_Tmp15;});
# 1961
_Tmp13->hd=_Tmp14;}),_Tmp13->tl=tms;_Tmp13;});
# 1965
goto _LL16;}else{
# 1967
tms=({struct Cyc_List_List*_Tmp13=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp14=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp15=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));
_Tmp15->tag=3,({void*_Tmp16=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp17=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp17->tag=1,_Tmp17->f1=args,_Tmp17->f2=c_varargs,_Tmp17->f3=cyc_varargs,_Tmp17->f4=effopt,_Tmp17->f5=rgn_po,_Tmp17->f6=qb,_Tmp17->f7=req,_Tmp17->f8=ens,_Tmp17->f9=thrws;_Tmp17;});_Tmp15->f1=_Tmp16;});_Tmp15;});
# 1967
_Tmp13->hd=_Tmp14;}),_Tmp13->tl=tms;_Tmp13;});
# 1971
for(1;fn_atts!=0;fn_atts=fn_atts->tl){
void*_Tmp13=(void*)fn_atts->hd;switch(*((int*)_Tmp13)){case 1:
 goto _LL1F;case 2: _LL1F:
 goto _LL21;case 3: _LL21:
# 1976
 tms=({struct Cyc_List_List*_Tmp14=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp15=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp16=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp16->tag=5,_Tmp16->f1=0U,({struct Cyc_List_List*_Tmp17=({struct Cyc_List_List*_Tmp18=_cycalloc(sizeof(struct Cyc_List_List));_Tmp18->hd=(void*)fn_atts->hd,_Tmp18->tl=0;_Tmp18;});_Tmp16->f2=_Tmp17;});_Tmp16;});_Tmp14->hd=_Tmp15;}),_Tmp14->tl=tms;_Tmp14;});
goto AfterAtts;default:
 goto _LL1B;}_LL1B:;}
# 1980
goto _LL16;}_LL16:;
# 1983
AfterAtts:
 if(typvars!=0)
tms=({struct Cyc_List_List*_Tmp13=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp14=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp15=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp15->tag=4,_Tmp15->f1=typvars,_Tmp15->f2=0U,_Tmp15->f3=1;_Tmp15;});_Tmp13->hd=_Tmp14;}),_Tmp13->tl=tms;_Tmp13;});
return({struct _tuple14 _Tmp13;_Tmp13.f0=tq3,_Tmp13.f1=t3,_Tmp13.f2=tms;_Tmp13;});}}}case 1: _TmpE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f1;_TmpC=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;_Tmp7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f3;{struct Cyc_Core_Opt*k=_TmpE;void*topt=_TmpC;int i=_Tmp7;
# 1989
return topt==0?({struct _tuple14 _TmpF;_TmpF.f0=tq,_TmpF.f1=t,_TmpF.f2=0;_TmpF;}): Cyc_Absynpp_to_tms(r,tq,topt);}case 7: _TmpE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_TmpC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_TmpB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f3;_Tmp8=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f4;{struct _tuple1*n=_TmpE;struct Cyc_List_List*ts=_TmpC;struct Cyc_Absyn_Typedefdecl*td=_TmpB;void*topt=_Tmp8;
# 1995
if(topt==0 || !Cyc_Absynpp_expand_typedefs)
return({struct _tuple14 _TmpF;_TmpF.f0=tq,_TmpF.f1=t,_TmpF.f2=0;_TmpF;});
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,topt);}default:
# 2001
 return({struct _tuple14 _TmpF;_TmpF.f0=tq,_TmpF.f1=t,_TmpF.f2=0;_TmpF;});};}
# 2005
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2007
void*_Tmp0;switch(*((int*)t)){case 1: _Tmp0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;if((void*)_Tmp0!=0){void*def=_Tmp0;
return Cyc_Absynpp_is_char_ptr(def);}else{goto _LL5;}case 3: _Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;{void*elt_typ=_Tmp0;
# 2010
while(1){
void*_Tmp1;switch(*((int*)elt_typ)){case 1: _Tmp1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)elt_typ)->f2;if((void*)_Tmp1!=0){void*t=_Tmp1;
elt_typ=t;continue;}else{goto _LLE;}case 7: _Tmp1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)elt_typ)->f4;if((void*)_Tmp1!=0){void*t=_Tmp1;
elt_typ=t;continue;}else{goto _LLE;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_typ)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_typ)->f1)->f2==Cyc_Absyn_Char_sz)
return 1;else{goto _LLE;}}else{goto _LLE;}default: _LLE:
 return 0;};}}default: _LL5:
# 2017
 return 0;};}
# 2021
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _Tmp0=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
{struct _tuple14 _Tmp1=Cyc_Absynpp_to_tms(temp,tq,typ);void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;struct Cyc_List_List*tms=_Tmp2;
tms=Cyc_List_imp_rev(tms);
if(tms==0 && dopt==0){
struct Cyc_PP_Doc*_Tmp5=({struct Cyc_PP_Doc*_Tmp6[2];({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_tqual2doc(tq);_Tmp6[0]=_Tmp7;}),({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_ntyp2doc(t);_Tmp6[1]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),2));});_npop_handler(0);return _Tmp5;}{
struct Cyc_PP_Doc*_Tmp5=({struct Cyc_PP_Doc*_Tmp6[4];({struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_tqual2doc(tq);_Tmp6[0]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_Absynpp_ntyp2doc(t);_Tmp6[1]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=Cyc_PP_text(_tag_fat(" ",sizeof(char),2U));_Tmp6[2]=_Tmp7;}),({
struct Cyc_PP_Doc*_Tmp7=({int _Tmp8=Cyc_Absynpp_is_char_ptr(typ);struct Cyc_PP_Doc*_Tmp9=dopt==0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v;Cyc_Absynpp_dtms2doc(_Tmp8,_Tmp9,tms);});_Tmp6[3]=_Tmp7;});Cyc_PP_cat(_tag_fat(_Tmp6,sizeof(struct Cyc_PP_Doc*),4));});_npop_handler(0);return _Tmp5;}}}
# 2023
;_pop_region();}
# 2033
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Exp*req=f->requires_clause;
struct Cyc_Absyn_Exp*wid=f->width;
struct Cyc_PP_Doc*requires_doc=(unsigned)req?({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("@requires ",sizeof(char),11U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(req);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));}): Cyc_PP_nil_doc();
struct Cyc_PP_Doc*width_doc=(unsigned)wid?({struct Cyc_PP_Doc*_Tmp0[2];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(":",sizeof(char),2U));_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_exp2doc(wid);_Tmp0[1]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),2));}): Cyc_PP_nil_doc();
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c)
# 2040
return({struct Cyc_PP_Doc*_Tmp0[5];({struct Cyc_PP_Doc*_Tmp1=({struct Cyc_Absyn_Tqual _Tmp2=f->tq;void*_Tmp3=f->type;Cyc_Absynpp_tqtd2doc(_Tmp2,_Tmp3,({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp5=Cyc_PP_textptr(f->name);_Tmp4->v=_Tmp5;});_Tmp4;}));});_Tmp0[0]=_Tmp1;}),_Tmp0[1]=width_doc,({
struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_atts2doc(f->attributes);_Tmp0[2]=_Tmp1;}),_Tmp0[3]=requires_doc,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp0[4]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),5));});else{
# 2043
return({struct Cyc_PP_Doc*_Tmp0[5];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_atts2doc(f->attributes);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=({struct Cyc_Absyn_Tqual _Tmp2=f->tq;void*_Tmp3=f->type;Cyc_Absynpp_tqtd2doc(_Tmp2,_Tmp3,({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp5=Cyc_PP_textptr(f->name);_Tmp4->v=_Tmp5;});_Tmp4;}));});_Tmp0[1]=_Tmp1;}),_Tmp0[2]=width_doc,_Tmp0[3]=requires_doc,({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat(";",sizeof(char),2U));_Tmp0[4]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),5));});};}
# 2048
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Aggrfield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_aggrfield2doc,_tag_fat("",sizeof(char),1U),fields);}
# 2052
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_scope2doc(f->sc);_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_typedef_name2doc(f->name);_Tmp0[1]=_Tmp1;}),
f->typs==0?({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_nil_doc();_Tmp0[2]=_Tmp1;}):({struct Cyc_PP_Doc*_Tmp1=Cyc_Absynpp_args2doc(f->typs);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
# 2056
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
return({(struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Datatypefield*),struct _fat_ptr,struct Cyc_List_List*))Cyc_PP_ppseql;})(Cyc_Absynpp_datatypefield2doc,_tag_fat(",",sizeof(char),2U),fields);}
# 2061
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(1;tdl!=0;tdl=tdl->tl){
({struct Cyc_PP_Doc*_Tmp0=Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd);Cyc_PP_file_of_doc(_Tmp0,72,f);});
Cyc_fprintf(f,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}}
# 2068
struct _fat_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
return Cyc_PP_string_of_doc(({struct _fat_ptr _Tmp0=_tag_fat("",sizeof(char),1U);Cyc_PP_seql(_Tmp0,({(struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Absynpp_decl2doc,tdl));}),72);}
# 2071
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0,0),72);}
struct _fat_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _fat_ptr Cyc_Absynpp_rgnpo2string(struct Cyc_List_List*po){
return Cyc_PP_string_of_doc(Cyc_Absynpp_rgnpo2doc(po),72);}
# 2078
struct _fat_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _fat_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;}}
# 2088
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _fat_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst c){return Cyc_PP_string_of_doc(Cyc_Absynpp_cnst2doc(c),72);}

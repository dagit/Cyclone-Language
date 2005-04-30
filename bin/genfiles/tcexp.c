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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f0;void*f1;};
# 112 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
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
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 135 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 156
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 160
enum Cyc_Absyn_AliasQualVal{Cyc_Absyn_Aliasable_qual =0U,Cyc_Absyn_Unique_qual =1U,Cyc_Absyn_Refcnt_qual =2U,Cyc_Absyn_Restricted_qual =3U};
# 176 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 182
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U,Cyc_Absyn_AqualKind =8U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 308
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 315
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 407 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 481
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 488
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 506
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 520
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 879 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 886
void*Cyc_Absyn_compress(void*);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 901
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 904
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulonglong_type;
# 906
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slonglong_type;
# 908
extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_wchar_type (void);
void*Cyc_Absyn_gen_float_type(unsigned);
# 913
extern void*Cyc_Absyn_heap_rgn_type;
# 915
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 919
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 921
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*,struct Cyc_Absyn_Enumdecl*);
# 940
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 944
void*Cyc_Absyn_exn_type (void);
# 952
extern void*Cyc_Absyn_fat_bound_type;
# 954
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 956
void*Cyc_Absyn_bounds_one (void);
# 958
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 962
void*Cyc_Absyn_atb_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*,void*);
# 964
void*Cyc_Absyn_star_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 966
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 970
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 978
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 982
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 988
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 990
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_uniqueaqual_exp (void);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1136
void*Cyc_Absyn_pointer_expand(void*,int);
# 1138
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1140
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 1143
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1145
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66
void Cyc_Warn_verr2(unsigned,struct _fat_ptr);
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 73
void*Cyc_Warn_impos_loc2(unsigned,struct _fat_ptr);
# 45 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct Cyc_RgnOrder_RgnPO;
# 36 "tcutil.h"
int Cyc_Tcutil_is_arithmetic_type(void*);
int Cyc_Tcutil_is_tag_type(void*);
# 41
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
int Cyc_Tcutil_is_boxed(void*);
# 51
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 54
int Cyc_Tcutil_is_bits_only_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 61
void*Cyc_Tcutil_pointer_region(void*);
# 64
int Cyc_Tcutil_rgn_of_pointer(void*,void**);
# 66
void*Cyc_Tcutil_pointer_aqual(void*);
# 69
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 74
int Cyc_Tcutil_is_fat_pointer_type_elt(void*,void**);
# 76
int Cyc_Tcutil_is_zero_pointer_type_elt(void*,void**);
# 81
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 84
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 91
void*Cyc_Tcutil_copy_type(void*);
# 94
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 98
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*);
# 104
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,struct Cyc_List_List*);
# 108
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 110
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 115
int Cyc_Tcutil_check_aqual_bounds(struct Cyc_List_List*,void*,void*);
# 117
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);struct _tuple12{struct Cyc_Absyn_Decl*f0;struct Cyc_Absyn_Exp*f1;int f2;};
# 120
struct _tuple12 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*,void*);
# 122
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 132
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_List_List*f0;struct _RegionHandle*f1;};struct _tuple14{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 151
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 162
void Cyc_Tcutil_check_nonzero_bound(unsigned,void*);
# 164
void Cyc_Tcutil_check_bound(unsigned,unsigned,void*,int);
# 166
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 182
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*,void*,int);
# 187
int Cyc_Tcutil_is_noalias_path(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 192
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*,void*);struct _tuple15{int f0;void*f1;};
# 196
struct _tuple15 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 212
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 219
void Cyc_Tcutil_check_no_qual(unsigned,void*);
# 230
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 233
int Cyc_Tcutil_zeroable_type(void*);
# 237
int Cyc_Tcutil_force_type2bool(int,void*);
# 240
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 242
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*);
# 256
void*Cyc_Tcutil_eval_aquals_of(struct Cyc_List_List*,void*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
# 52 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
extern struct Cyc_Core_Opt Cyc_Kinds_ako;
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
# 60
extern struct Cyc_Core_Opt Cyc_Kinds_aqko;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 93
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 76 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*,int);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 93
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 120
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 124
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
# 138
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
void*Cyc_Tcenv_curr_lifo_rgn(struct Cyc_Tcenv_Tenv*);
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
# 146
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 148
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 153
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*,unsigned,struct Cyc_List_List*);
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 33
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
# 47 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _fat_ptr ap){
# 49
Cyc_Warn_verr2(loc,ap);
return topt!=0?*topt: Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 52
static void Cyc_Tcexp_err_and_explain(unsigned loc,struct _fat_ptr ap){
Cyc_Warn_verr2(loc,ap);
Cyc_Unify_explain_failure();}
# 57
void Cyc_Tcexp_check_consume(unsigned loc,struct Cyc_List_List*aqb,void*t,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(aqb,t)&& !Cyc_Tcutil_is_noalias_path(aqb,e))
# 62
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Cannot consume non-unique paths; do swap instead",sizeof(char),49U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});}
# 67
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type_with_aqual(struct Cyc_Tcenv_Tenv*te,void*aq){
struct Cyc_List_List*tenv_tvs=Cyc_Tcenv_lookup_type_vars(te);
void*t2=Cyc_Absyn_new_evar(& Cyc_Kinds_ako,({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=tenv_tvs;_Tmp0;}));
void*rt=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=tenv_tvs;_Tmp0;}));
void*nbl=Cyc_Tcutil_any_bool(tenv_tvs);
void*b=Cyc_Tcutil_any_bounds(tenv_tvs);
void*zt=Cyc_Tcutil_any_bool(tenv_tvs);
void*rel=Cyc_Tcutil_any_bool(tenv_tvs);
return({struct Cyc_Absyn_PtrInfo _Tmp0;_Tmp0.elt_type=t2,({struct Cyc_Absyn_Tqual _Tmp1=Cyc_Absyn_empty_tqual(0U);_Tmp0.elt_tq=_Tmp1;}),_Tmp0.ptr_atts.rgn=rt,_Tmp0.ptr_atts.nullable=nbl,_Tmp0.ptr_atts.bounds=b,_Tmp0.ptr_atts.zero_term=zt,_Tmp0.ptr_atts.ptrloc=0,_Tmp0.ptr_atts.autoreleased=rel,_Tmp0.ptr_atts.aqual=aq;_Tmp0;});}
# 78
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*tenv_tvs=Cyc_Tcenv_lookup_type_vars(te);
return({struct Cyc_Tcenv_Tenv*_Tmp0=te;Cyc_Tcexp_fresh_pointer_type_with_aqual(_Tmp0,({void*_Tmp1=Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=tenv_tvs;_Tmp2;}));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_rtd_qual_type);}));});}struct _tuple17{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 84
static void Cyc_Tcexp_resolve_unresolved_mem(unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 88
if(topt==0){
# 92
if(des!=0){
struct _tuple17 _Tmp0=*((struct _tuple17*)des->hd);void*_Tmp1;_Tmp1=_Tmp0.f0;{struct Cyc_List_List*dls=_Tmp1;
if(dls!=0){
void*_Tmp2=(void*)dls->hd;void*_Tmp3;if(*((int*)_Tmp2)==1){_Tmp3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp2)->f1;{struct _fat_ptr*n=_Tmp3;
# 97
({void*_Tmp4=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct));_Tmp5->tag=29,_Tmp5->f1=0,_Tmp5->f2=0,_Tmp5->f3=des;_Tmp5;});e->r=_Tmp4;});
return;}}else{
goto _LL3;}_LL3:;}}}
# 103
({void*_Tmp0=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp1->tag=25,_Tmp1->f1=des;_Tmp1;});e->r=_Tmp0;});
return;}{
# 107
void*t=*topt;
void*_Tmp0=Cyc_Absyn_compress(t);int _Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;union Cyc_Absyn_AggrInfo _Tmp4;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==25){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp4;
# 110
{void*_Tmp5;if(info.UnknownAggr.tag==1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("struct type improperly set",sizeof(char),27U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(loc,_tag_fat(_Tmp7,sizeof(void*),1));});else{_Tmp5=*info.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp5;
({void*_Tmp6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp7->tag=28,_Tmp7->f1=ad->name,_Tmp7->f2=0,_Tmp7->f3=des,_Tmp7->f4=ad;_Tmp7;});e->r=_Tmp6;});}};}
# 114
goto _LL8;}}else{goto _LLF;}case 4: _Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.tq;{void*at=_Tmp3;struct Cyc_Absyn_Tqual aq=_Tmp2;
({void*_Tmp5=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp6->tag=25,_Tmp6->f1=des;_Tmp6;});e->r=_Tmp5;});goto _LL8;}case 6: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{int it=_Tmp1;
({void*_Tmp5=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct));_Tmp6->tag=29,_Tmp6->f1=t,_Tmp6->f2=it,_Tmp6->f3=des;_Tmp6;});e->r=_Tmp5;});goto _LL8;}default: _LLF:
({void*_Tmp5=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp6->tag=25,_Tmp6->f1=des;_Tmp6;});e->r=_Tmp5;});goto _LL8;}_LL8:;}}
# 124
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 128
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;if(*((int*)_Tmp0)==4){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2==0){
# 131
if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("assignment in test",sizeof(char),19U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_warn2(e->loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL0;}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 139
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _fat_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_type,e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("test of ",sizeof(char),9U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=msg_part;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" has type ",sizeof(char),11U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)e->topt;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" instead of integral or pointer type",sizeof(char),37U);_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp5,sizeof(void*),5));});}
# 161 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _fat_ptr s){
return 1;}
# 166
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
# 181
if(topt==0)
return Cyc_Absyn_gen_float_type((unsigned)i);{
void*_Tmp4=Cyc_Absyn_compress(*topt);int _Tmp5;if(*((int*)_Tmp4)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==2){_Tmp5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{int i=_Tmp5;
return Cyc_Absyn_gen_float_type((unsigned)i);}}else{goto _LL1E;}}else{_LL1E:
 return Cyc_Absyn_gen_float_type((unsigned)i);};}}case 5: _Tmp2=_Tmp0.Int_c.val.f0;_Tmp3=_Tmp0.Int_c.val.f1;{enum Cyc_Absyn_Sign csn=_Tmp2;int i=_Tmp3;
# 191
if(topt==0)
return(int)csn==1?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;{
void*_Tmp4=Cyc_Absyn_compress(*topt);void*_Tmp5;enum Cyc_Absyn_Sign _Tmp6;switch(*((int*)_Tmp4)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)){case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f2){case Cyc_Absyn_Char_sz: _Tmp6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{enum Cyc_Absyn_Sign sn=_Tmp6;
# 195
({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Char_c(sn,(char)i);*c=_Tmp7;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Short_sz: _Tmp6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{enum Cyc_Absyn_Sign sn=_Tmp6;
# 198
({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Short_c(sn,(short)i);*c=_Tmp7;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}case Cyc_Absyn_Int_sz: _Tmp6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{enum Cyc_Absyn_Sign sn=_Tmp6;
_Tmp6=sn;goto _LL28;}case Cyc_Absyn_Long_sz: _Tmp6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;_LL28: {enum Cyc_Absyn_Sign sn=_Tmp6;
# 204
({union Cyc_Absyn_Cnst _Tmp7=Cyc_Absyn_Int_c(sn,i);*c=_Tmp7;});
return Cyc_Tcexp_tcConst(te,loc,0,c,e);}default: goto _LL2D;}case 5: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2!=0){_Tmp5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2->hd;{void*t1=_Tmp5;
# 213
return Cyc_Absyn_tag_type((void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp7->tag=8,({struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_uint_exp((unsigned)i,0U);_Tmp7->f1=_Tmp8;});_Tmp7;}));}}else{goto _LL2D;}default: goto _LL2D;}case 3: if(i==0){
# 208
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
({union Cyc_Absyn_Cnst _Tmp7=({union Cyc_Absyn_Cnst _Tmp8;_Tmp8.Null_c.tag=1U,_Tmp8.Null_c.val=0;_Tmp8;});*c=_Tmp7;});
return Cyc_Tcexp_tcConst(te,loc,topt,c,e);}else{goto _LL2D;}default: _LL2D:
# 215
 return(int)csn==1?Cyc_Absyn_uint_type: Cyc_Absyn_sint_type;};}}case 1:
# 218
 if(topt!=0){
void*_Tmp4=Cyc_Absyn_compress(*topt);void*_Tmp5;if(*((int*)_Tmp4)==3){_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1.ptr_atts.nullable;{void*nbl=_Tmp5;
# 221
if(!Cyc_Tcutil_force_type2bool(1,nbl))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("Used NULL when expecting a value of type ",sizeof(char),42U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)*topt;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp8,sizeof(void*),2));});
return*topt;}}else{
goto _LL2F;}_LL2F:;}{
# 226
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.ptr_atts.nullable=Cyc_Absyn_true_type;
return Cyc_Absyn_pointer_type(pi);}case 8: _Tmp1=_Tmp0.String_c.val;{struct _fat_ptr s=_Tmp1;
# 231
string_numelts=(int)_get_fat_size(s,sizeof(char));
_Tmp1=s;goto _LL1A;}default: _Tmp1=_Tmp0.Wstring_c.val;_LL1A: {struct _fat_ptr s=_Tmp1;
# 234
if(string_numelts==0){
string_numelts=Cyc_Tcexp_wchar_numelts(s);
string_elt_typ=Cyc_Absyn_wchar_type();}{
# 238
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _Tmp4=Cyc_Absyn_Int_c(1U,string_numelts);Cyc_Absyn_const_exp(_Tmp4,loc);});
elen->topt=Cyc_Absyn_uint_type;{
# 243
void*t=({void*_Tmp4=string_elt_typ;void*_Tmp5=Cyc_Absyn_heap_rgn_type;void*_Tmp6=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp7=Cyc_Absyn_const_tqual(0U);void*_Tmp8=
Cyc_Absyn_thin_bounds_exp(elen);
# 243
void*_Tmp9=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Absyn_false_type);});
# 245
if(topt==0)
return t;{
void*_Tmp4=Cyc_Absyn_compress(*topt);struct Cyc_Absyn_Tqual _Tmp5;switch(*((int*)_Tmp4)){case 4: _Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp4)->f1.tq;{struct Cyc_Absyn_Tqual tq=_Tmp5;
# 251
return({void*_Tmp6=string_elt_typ;struct Cyc_Absyn_Tqual _Tmp7=tq;struct Cyc_Absyn_Exp*_Tmp8=elen;Cyc_Absyn_array_type(_Tmp6,_Tmp7,_Tmp8,
Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te)),0U);});}case 3:
# 256
 if(!Cyc_Unify_unify(*topt,t)&&({struct Cyc_RgnOrder_RgnPO*_Tmp6=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp7=loc;void*_Tmp8=t;Cyc_Tcutil_silent_castable(_Tmp6,_Tmp7,_Tmp8,*topt);})){
e->topt=t;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
return*topt;}
# 261
return t;default:
 return t;};}}}}};}
# 268
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_Tmp0=*_check_null(b);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_Tmp0)->f1;{struct _tuple1*q=_Tmp1;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("unresolved binding in tcVar",sizeof(char),28U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(loc,_tag_fat(_Tmp3,sizeof(void*),1));});}case 1: _Tmp1=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 274
Cyc_Tcenv_lookup_ordinary_global(te,loc,vd->name,1);
return vd->type;}case 2: _Tmp1=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 281
if(fd->fn_vardecl==0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,fd->name,1);
return Cyc_Tcutil_fndecl2type(fd);}case 5: _Tmp1=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
_Tmp1=vd;goto _LLA;}case 4: _Tmp1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_Tmp0)->f1;_LLA: {struct Cyc_Absyn_Vardecl*vd=_Tmp1;
_Tmp1=vd;goto _LLC;}default: _Tmp1=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_Tmp0)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 287
if(te->allow_valueof){
void*_Tmp2=Cyc_Absyn_compress(vd->type);void*_Tmp3;if(*((int*)_Tmp2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2!=0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2->hd;{void*i=_Tmp3;
({void*_Tmp4=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp5->tag=38,_Tmp5->f1=i;_Tmp5;});e->r=_Tmp4;});goto _LLD;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10:
 goto _LLD;}_LLD:;}
# 292
return vd->type;}};}
# 296
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,int isCproto,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _fat_ptr,int,unsigned)){
# 303
void*_Tmp0=fmt->r;struct _fat_ptr _Tmp1;switch(*((int*)_Tmp0)){case 0: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.tag==8){_Tmp1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.val;{struct _fat_ptr s=_Tmp1;
_Tmp1=s;goto _LL4;}}else{goto _LL5;}case 14: if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->r)==0){if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->r)->f1.String_c.tag==8){_Tmp1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->r)->f1.String_c.val;_LL4: {struct _fat_ptr s=_Tmp1;
# 306
struct Cyc_List_List*desc_types=type_getter(te,s,isCproto,fmt->loc);
if(opt_args==0)
return;{
struct Cyc_List_List*args=(struct Cyc_List_List*)opt_args->v;
# 311
for(1;desc_types!=0 && args!=0;(
desc_types=desc_types->tl,args=args->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)args->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp2=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp3=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp4=e;Cyc_Tcutil_coerce_arg(_Tmp2,_Tmp3,_Tmp4,t,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("descriptor has type ",sizeof(char),21U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" but argument has type ",sizeof(char),24U);_Tmp5;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp6;_Tmp6.tag=3,_Tmp6.f1=(void*)e->topt;_Tmp6;});void*_Tmp6[4];_Tmp6[0]=& _Tmp2,_Tmp6[1]=& _Tmp3,_Tmp6[2]=& _Tmp4,_Tmp6[3]=& _Tmp5;Cyc_Tcexp_err_and_explain(e->loc,_tag_fat(_Tmp6,sizeof(void*),4));});
if(alias_coercion)
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=(void*)arg_cnt,_Tmp3->tl=*alias_arg_exps;_Tmp3;});*alias_arg_exps=_Tmp2;});
({unsigned _Tmp2=e->loc;struct Cyc_List_List*_Tmp3=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp4=t;Cyc_Tcexp_check_consume(_Tmp2,_Tmp3,_Tmp4,e);});}
# 325
if(desc_types!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("too few arguments",sizeof(char),18U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(fmt->loc,_tag_fat(_Tmp3,sizeof(void*),1));});
if(args!=0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("too many arguments",sizeof(char),19U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(((struct Cyc_Absyn_Exp*)args->hd)->loc,_tag_fat(_Tmp3,sizeof(void*),1));});
# 330
for(1;args!=0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);}}
# 333
return;}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
# 337
 if(opt_args==0)
return;
{struct Cyc_List_List*args=(struct Cyc_List_List*)opt_args->v;for(0;args!=0;args=args->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)args->hd);
({unsigned _Tmp2=((struct Cyc_Absyn_Exp*)args->hd)->loc;struct Cyc_List_List*_Tmp3=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp4=_check_null(((struct Cyc_Absyn_Exp*)args->hd)->topt);Cyc_Tcexp_check_consume(_Tmp2,_Tmp3,_Tmp4,(struct Cyc_Absyn_Exp*)args->hd);});}}
# 343
return;};}
# 347
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 349
void*t=Cyc_Absyn_compress(_check_null(e->topt));
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Minus: _LL4:
# 353
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting numeric type but found ",sizeof(char),34U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return _check_null(e->topt);case Cyc_Absyn_Not:
# 357
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting integral or * type but found ",sizeof(char),40U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_sint_type;case Cyc_Absyn_Bitnot:
# 362
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting integral type but found ",sizeof(char),35U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return _check_null(e->topt);case Cyc_Absyn_Numelts:
# 366
{void*_Tmp0;if(*((int*)t)==3){_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;{void*b=_Tmp0;
# 368
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if((eopt!=0 && !Cyc_Evexp_c_can_eval(eopt))&& !((unsigned)Cyc_Tcenv_allow_valueof))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("cannot use numelts on a pointer with abstract bounds",sizeof(char),53U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LLD;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("numelts requires pointer type, not ",sizeof(char),36U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});}_LLD:;}
# 374
return Cyc_Absyn_uint_type;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Non-unary primop",sizeof(char),17U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(unsigned,struct _fat_ptr))Cyc_Warn_impos_loc2;})(loc,_tag_fat(_Tmp1,sizeof(void*),1));});};}
# 381
static void*Cyc_Tcexp_arith_convert(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=_check_null(e1->topt);
void*t2=_check_null(e2->topt);
void*new_typ=Cyc_Tcutil_max_arithmetic_type(t1,t2);
if(!Cyc_Unify_unify(t1,new_typ))Cyc_Tcutil_unchecked_cast(e1,new_typ,1U);
if(!Cyc_Unify_unify(t2,new_typ))Cyc_Tcutil_unchecked_cast(e2,new_typ,1U);
return new_typ;}
# 391
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 394
if(!checker(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type ",sizeof(char),6U);_Tmp1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=(void*)e1->topt;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" cannot be used here",sizeof(char),21U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Tcexp_expr_err(te,e1->loc,0,_tag_fat(_Tmp3,sizeof(void*),3));});
if(!checker(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type ",sizeof(char),6U);_Tmp1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=(void*)e2->topt;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" cannot be used here",sizeof(char),21U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Tcexp_expr_err(te,e2->loc,0,_tag_fat(_Tmp3,sizeof(void*),3));});
return Cyc_Tcexp_arith_convert(te,e1,e2);}
# 401
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;if(*((int*)t1)==3){_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_tq;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.rgn;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.autoreleased;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.aqual;{void*et=_Tmp7;struct Cyc_Absyn_Tqual tq=_Tmp6;void*r=_Tmp5;void*n=_Tmp4;void*b=_Tmp3;void*zt=_Tmp2;void*rel=_Tmp1;void*aq=_Tmp0;
# 406
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(et),& Cyc_Kinds_mk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("can't do arithmetic on abstract pointer type",sizeof(char),45U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp9,sizeof(void*),1));});
if(({struct Cyc_List_List*_Tmp8=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer(_Tmp8,t1,1);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("can't do arithmetic on unique pointer",sizeof(char),38U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp9,sizeof(void*),1));});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("expecting int but found ",sizeof(char),25U);_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)t2;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err2(e2->loc,_tag_fat(_TmpA,sizeof(void*),2));});{
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);
if(eopt==0)
return t1;
# 417
if(Cyc_Tcutil_force_type2bool(0,zt)){
struct _tuple16 _Tmp8=Cyc_Evexp_eval_const_uint_exp(eopt);int _Tmp9;unsigned _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{unsigned i=_TmpA;int known=_Tmp9;
if(known && i==1U)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("pointer arithmetic on thin, zero-terminated pointer may be expensive.",sizeof(char),70U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_Warn_warn2(e1->loc,_tag_fat(_TmpC,sizeof(void*),1));});}}{
# 428
struct Cyc_Absyn_PointerType_Absyn_Type_struct*new_t1;new_t1=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct)),new_t1->tag=3,new_t1->f1.elt_type=et,new_t1->f1.elt_tq=tq,new_t1->f1.ptr_atts.rgn=r,new_t1->f1.ptr_atts.nullable=Cyc_Absyn_true_type,new_t1->f1.ptr_atts.bounds=Cyc_Absyn_fat_bound_type,new_t1->f1.ptr_atts.zero_term=zt,new_t1->f1.ptr_atts.ptrloc=0,new_t1->f1.ptr_atts.autoreleased=rel,new_t1->f1.ptr_atts.aqual=aq;
# 431
Cyc_Tcutil_unchecked_cast(e1,(void*)new_t1,3U);
return(void*)new_t1;}}}}else{
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);};}
# 436
static void*Cyc_Tcexp_relqual(void*t){
void*_Tmp0;if(*((int*)t)==3){_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.autoreleased;{void*rel=_Tmp0;
return rel;}}else{
return Cyc_Absyn_false_type;};}
# 444
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=_check_null(e1->topt);
void*t2=_check_null(e2->topt);
void*t1_elt=Cyc_Absyn_void_type;
void*t2_elt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_fat_pointer_type_elt(t2,& t2_elt)){
if(!Cyc_Unify_unify(t1_elt,t2_elt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("pointer arithmetic on values of different ",sizeof(char),43U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=
_tag_fat("types (",sizeof(char),8U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t1;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t2;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(")",sizeof(char),2U);_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Tcexp_err_and_explain(e1->loc,_tag_fat(_Tmp6,sizeof(void*),6));});
return Cyc_Absyn_sint_type;}
# 456
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_Tmp0=t1_elt;Cyc_Unify_unify(_Tmp0,Cyc_Tcutil_pointer_elt_type(t2));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("pointer arithmetic on values of different ",sizeof(char),43U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=
_tag_fat("types(",sizeof(char),7U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t1;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t2;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(")",sizeof(char),2U);_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Tcexp_err_and_explain(e1->loc,_tag_fat(_Tmp6,sizeof(void*),6));});
# 461
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("coercing fat pointer to thin pointer for subtraction",sizeof(char),53U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_Absyn_Exp*_Tmp0=e1;Cyc_Tcutil_unchecked_cast(_Tmp0,({void*_Tmp1=t1_elt;void*_Tmp2=Cyc_Tcutil_pointer_region(t1);void*_Tmp3=Cyc_Tcutil_pointer_aqual(t1);struct Cyc_Absyn_Tqual _Tmp4=
Cyc_Absyn_empty_tqual(0U);
# 462
void*_Tmp5=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,
Cyc_Tcexp_relqual(t1));}),3U);});
# 465
return Cyc_Absyn_sint_type;}
# 467
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t1_elt),& Cyc_Kinds_mk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't perform arithmetic on abstract pointer type",sizeof(char),50U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
if(({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer(_Tmp0,t1,1);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't perform arithmetic on unique pointer",sizeof(char),43U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
if(!Cyc_Tcutil_coerce_sint_type(e2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting ",sizeof(char),11U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" or int but found ",sizeof(char),19U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_err_and_explain(e2->loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return t1;}
# 476
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2)&&({void*_Tmp0=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Unify_unify(_Tmp0,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_fat_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("coercing fat pointer to thin pointer for subtraction",sizeof(char),53U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_Absyn_Exp*_Tmp0=e2;Cyc_Tcutil_unchecked_cast(_Tmp0,({void*_Tmp1=Cyc_Tcutil_pointer_elt_type(t2);void*_Tmp2=
Cyc_Tcutil_pointer_region(t2);
# 481
void*_Tmp3=
Cyc_Tcutil_pointer_aqual(t2);
# 481
struct Cyc_Absyn_Tqual _Tmp4=
# 483
Cyc_Absyn_empty_tqual(0U);
# 481
void*_Tmp5=Cyc_Absyn_false_type;Cyc_Absyn_star_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,
# 483
Cyc_Tcexp_relqual(t2));}),3U);});}
# 486
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("thin pointer subtraction!",sizeof(char),26U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return Cyc_Absyn_sint_type;}
# 489
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("coercing thin pointer to integer for subtraction",sizeof(char),49U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Tcutil_unchecked_cast(e1,Cyc_Absyn_sint_type,3U);}
# 492
if(Cyc_Tcutil_is_pointer_type(t2)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("coercing pointer to integer for subtraction",sizeof(char),44U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(e1->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Tcutil_unchecked_cast(e2,Cyc_Absyn_sint_type,3U);}
# 497
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 500
static void*Cyc_Tcexp_tcCmpBinop(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
if(Cyc_Tcutil_is_numeric(e1)&& Cyc_Tcutil_is_numeric(e2)){
Cyc_Tcexp_arith_convert(te,e1,e2);
return Cyc_Absyn_sint_type;}
# 508
if((int)Cyc_Tcutil_type_kind(t1)->kind==2 ||({
void*_Tmp0=t1;Cyc_Unify_unify(_Tmp0,Cyc_Absyn_new_evar(& Cyc_Kinds_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_sint_type;
# 513
if(({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp1=loc;void*_Tmp2=t2;Cyc_Tcutil_silent_castable(_Tmp0,_Tmp1,_Tmp2,t1);})){
Cyc_Tcutil_unchecked_cast(e2,t1,3U);
return Cyc_Absyn_sint_type;}
# 517
if(({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp1=loc;void*_Tmp2=t1;Cyc_Tcutil_silent_castable(_Tmp0,_Tmp1,_Tmp2,t2);})){
Cyc_Tcutil_unchecked_cast(e1,t2,3U);
return Cyc_Absyn_sint_type;}
# 521
if(Cyc_Tcutil_zero_to_null(t2,e1)|| Cyc_Tcutil_zero_to_null(t1,e2))
return Cyc_Absyn_sint_type;}
# 526
{struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t1);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(t2);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0.f0)){case 3: if(*((int*)_Tmp0.f1)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;{void*t1a=_Tmp2;void*t2a=_Tmp1;
# 528
if(Cyc_Unify_unify(t1a,t2a))
return Cyc_Absyn_sint_type;
goto _LL0;}}else{goto _LL5;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==4){if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==4)
return Cyc_Absyn_sint_type;else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
 goto _LL0;}_LL0:;}
# 535
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("comparison not allowed between ",sizeof(char),32U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" and ",sizeof(char),6U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_err_and_explain(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 541
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 543
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times:
# 547
 goto _LL8;case Cyc_Absyn_Div: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod:
# 550
 goto _LLC;case Cyc_Absyn_Bitand: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL14:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq:
# 559
 goto _LL18;case Cyc_Absyn_Neq: _LL18:
 goto _LL1A;case Cyc_Absyn_Gt: _LL1A:
 goto _LL1C;case Cyc_Absyn_Lt: _LL1C:
 goto _LL1E;case Cyc_Absyn_Gte: _LL1E:
 goto _LL20;case Cyc_Absyn_Lte: _LL20:
 return Cyc_Tcexp_tcCmpBinop(te,loc,e1,e2);default:
# 566
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad binary primop",sizeof(char),18U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});};}
# 570
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 578
if((int)p==2 && Cyc_List_length(es)==1){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;
void*t=Cyc_Tcexp_tcExp(te,topt,e);
if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting numeric type but found ",sizeof(char),34U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return t;}
# 585
{struct Cyc_List_List*es2=es;for(0;es2!=0;es2=es2->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es2->hd);}}{
int _Tmp0=Cyc_List_length(es);switch((int)_Tmp0){case 0:
 return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("primitive operator has 0 arguments",sizeof(char),35U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp2,sizeof(void*),1));});case 1:
 return Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)_check_null(es)->hd);case 2:
 return({struct Cyc_Tcenv_Tenv*_Tmp1=te;unsigned _Tmp2=loc;void**_Tmp3=topt;enum Cyc_Absyn_Primop _Tmp4=p;struct Cyc_Absyn_Exp*_Tmp5=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;Cyc_Tcexp_tcBinPrimop(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd);});default:
 return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("primitive operator has > 2 arguments",sizeof(char),37U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp2,sizeof(void*),1));});};}}struct _tuple18{struct Cyc_Absyn_Tqual f0;void*f1;};
# 595
static int Cyc_Tcexp_check_writable_aggr(unsigned loc,void*t){
t=Cyc_Absyn_compress(t);{
struct Cyc_Absyn_Tqual _Tmp0;void*_Tmp1;switch(*((int*)t)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 25: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.tag==2){_Tmp1=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 599
if(ad->impl==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("attempt to write abstract ",sizeof(char),27U);_Tmp3;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp4;_Tmp4.tag=6,_Tmp4.f1=ad;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),2));});
return 0;}
# 603
_Tmp1=ad->impl->fields;goto _LL4;}}else{goto _LL9;}case 24: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownDatatypefield.tag==2){_Tmp1=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownDatatypefield.val.f1;{struct Cyc_Absyn_Datatypefield*df=_Tmp1;
# 615
{struct Cyc_List_List*fs=df->typs;for(0;fs!=0;fs=fs->tl){
struct _tuple18*_Tmp2=(struct _tuple18*)fs->hd;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("attempt to overwrite a datatype field (",sizeof(char),40U);_Tmp6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=df->name;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(") with a const member",sizeof(char),22U);_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),3));});
return 0;}
# 622
if(!Cyc_Tcexp_check_writable_aggr(loc,t))return 0;}}}
# 624
return 1;}}else{goto _LL9;}default: goto _LL9;}case 6: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;_LL4: {struct Cyc_List_List*fs=_Tmp1;
# 605
for(1;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)fs->hd;
if(f->tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("attempt to overwrite an aggregate with const member ",sizeof(char),53U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*f->name;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),2));});
return 0;}
# 611
if(!Cyc_Tcexp_check_writable_aggr(loc,f->type))return 0;}
# 613
return 1;}case 4: _Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp0=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;{void*elt_type=_Tmp1;struct Cyc_Absyn_Tqual tq=_Tmp0;
# 626
if(tq.real_const){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("attempt to overwrite a const array",sizeof(char),35U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),1));});
return 0;}
# 630
return Cyc_Tcexp_check_writable_aggr(loc,elt_type);}default: _LL9:
 return 1;};}}
# 638
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 641
if(!Cyc_Tcexp_check_writable_aggr(e->loc,_check_null(e->topt)))return;
{void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 3: _Tmp2=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL4;}case 4: _Tmp2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL6;}case 5: _Tmp2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL6: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL8;}case 1: _Tmp2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
if(!vd->tq.real_const)return;goto _LL0;}default: goto _LL15;}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 648
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));struct Cyc_Absyn_Tqual _Tmp4;switch(*((int*)_Tmp3)){case 3: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_tq;{struct Cyc_Absyn_Tqual tq=_Tmp4;
_Tmp4=tq;goto _LL1B;}case 4: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp3)->f1.tq;_LL1B: {struct Cyc_Absyn_Tqual tq=_Tmp4;
if(!tq.real_const)return;goto _LL17;}default:
 goto _LL17;}_LL17:;}
# 653
goto _LL0;}case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 655
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));void*_Tmp4;switch(*((int*)_Tmp3)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f1.KnownAggr.tag==2){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp4;
# 657
struct Cyc_Absyn_Aggrfield*sf=adp==0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf==0 || !sf->tq.real_const)return;
goto _LL1E;}}else{goto _LL23;}}else{goto _LL23;}case 6: _Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp3)->f3;{struct Cyc_List_List*fs=_Tmp4;
# 661
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf==0 || !sf->tq.real_const)return;
goto _LL1E;}default: _LL23:
 goto _LL1E;}_LL1E:;}
# 666
goto _LL0;}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 668
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;if(*((int*)_Tmp3)==3){_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_tq;{void*elt_typ=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;
# 670
if(!tq.real_const){
void*_Tmp6=Cyc_Absyn_compress(elt_typ);void*_Tmp7;switch(*((int*)_Tmp6)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownAggr.tag==2){_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp7;
# 673
struct Cyc_Absyn_Aggrfield*sf=
adp==0?0: Cyc_Absyn_lookup_decl_field(*adp,f);
if(sf==0 || !sf->tq.real_const)return;
goto _LL2A;}}else{goto _LL2F;}}else{goto _LL2F;}case 6: _Tmp7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp6)->f3;{struct Cyc_List_List*fs=_Tmp7;
# 678
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(fs,f);
if(sf==0 || !sf->tq.real_const)return;
goto _LL2A;}default: _LL2F:
 goto _LL2A;}_LL2A:;}
# 684
goto _LL25;}}else{
goto _LL25;}_LL25:;}
# 687
goto _LL0;}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
# 689
{void*_Tmp3=Cyc_Absyn_compress(_check_null(e1->topt));struct Cyc_Absyn_Tqual _Tmp4;switch(*((int*)_Tmp3)){case 3: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_tq;{struct Cyc_Absyn_Tqual tq=_Tmp4;
_Tmp4=tq;goto _LL35;}case 4: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp3)->f1.tq;_LL35: {struct Cyc_Absyn_Tqual tq=_Tmp4;
if(!tq.real_const)return;goto _LL31;}default:
 goto _LL31;}_LL31:;}
# 694
goto _LL0;}case 12: _Tmp2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
_Tmp2=e1;goto _LL14;}case 13: _Tmp2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp2;
Cyc_Tcexp_check_writable(te,e1);return;}default: _LL15:
 goto _LL0;}_LL0:;}
# 699
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("attempt to write a const location: ",sizeof(char),36U);_Tmp1;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=e;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});}
# 702
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 705
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_Tmp0,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("increment/decrement of non-lvalue",sizeof(char),34U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Tcexp_check_writable(te,e);{
void*t=_check_null(e->topt);
# 711
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=Cyc_Absyn_void_type;
if(Cyc_Tcutil_is_fat_pointer_type_elt(t,& telt)||
 Cyc_Tcutil_is_zero_pointer_type_elt(t,& telt)&&((int)i==0 ||(int)i==1)){
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(telt),& Cyc_Kinds_mk))
Cyc_Warn_err(e->loc,_tag_fat("can't perform arithmetic on abstract pointer type",sizeof(char),50U),_tag_fat(0U,sizeof(void*),0));
if(({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer(_Tmp0,t,1);}))
Cyc_Warn_err(e->loc,_tag_fat("can't perform arithmetic on unique pointer",sizeof(char),43U),_tag_fat(0U,sizeof(void*),0));}else{
# 720
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting arithmetic or ? type but found ",sizeof(char),42U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),2));});}}
# 722
return t;}}
# 726
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 728
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_Tmp1=e1;Cyc_Tcexp_tcTest(_Tmp0,_Tmp1,_tag_fat("conditional expression",sizeof(char),23U));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
struct Cyc_Core_Opt*ko=Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_ako:& Cyc_Kinds_mko;
void*t=({struct Cyc_Core_Opt*_Tmp0=ko;Cyc_Absyn_new_evar(_Tmp0,Cyc_Tcenv_lookup_opt_type_vars(te));});
struct Cyc_List_List l1=({struct Cyc_List_List _Tmp0;_Tmp0.hd=e3,_Tmp0.tl=0;_Tmp0;});
struct Cyc_List_List l2=({struct Cyc_List_List _Tmp0;_Tmp0.hd=e2,_Tmp0.tl=& l1;_Tmp0;});
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp1=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_coerce_list(_Tmp0,_Tmp1,t,& l2);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("conditional clause types do not match: ",sizeof(char),40U);_Tmp1;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=(void*)e2->topt;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat(" != ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)e3->topt;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_err_and_explain(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return t;}}
# 742
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 744
Cyc_Tcexp_tcTest(te,e1,_tag_fat("logical-and expression",sizeof(char),23U));
Cyc_Tcexp_tcTest(te,e2,_tag_fat("logical-and expression",sizeof(char),23U));
return Cyc_Absyn_sint_type;}
# 748
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 750
Cyc_Tcexp_tcTest(te,e1,_tag_fat("logical-or expression",sizeof(char),22U));
Cyc_Tcexp_tcTest(te,e2,_tag_fat("logical-or expression",sizeof(char),22U));
return Cyc_Absyn_sint_type;}
# 756
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 763
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_Tmp0,0,e1);});{
void*t1=_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=_check_null(e2->topt);
# 768
{void*_Tmp0=Cyc_Absyn_compress(t1);if(*((int*)_Tmp0)==4){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("cannot assign to an array",sizeof(char),26U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});goto _LL0;}else{
goto _LL0;}_LL0:;}
# 773
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t1),& Cyc_Kinds_mk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type is abstract (can't determine size)",sizeof(char),40U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 777
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("assignment to non-lvalue",sizeof(char),25U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_Tcexp_check_writable(te,e1);
if(po==0){
({unsigned _Tmp0=e2->loc;struct Cyc_List_List*_Tmp1=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp2=t2;Cyc_Tcexp_check_consume(_Tmp0,_Tmp1,_Tmp2,e2);});
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp1=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp2=e2;Cyc_Tcutil_coerce_assign(_Tmp0,_Tmp1,_Tmp2,t1);})){
void*result=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type mismatch: ",sizeof(char),16U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp4,sizeof(void*),4));});
Cyc_Unify_unify(t1,t2);
Cyc_Unify_explain_failure();
return result;}
# 788
return t1;}{
# 790
struct Cyc_Absyn_Exp*e1copy=Cyc_Absyn_copy_exp(e1);
void*t_result=Cyc_Tcexp_tcBinPrimop(te,loc,0,(enum Cyc_Absyn_Primop)po->v,e1copy,e2);
if(((!Cyc_Unify_unify(t_result,t1)&& Cyc_Tcutil_is_arithmetic_type(t_result))&& Cyc_Tcutil_is_arithmetic_type(t1))&& !Cyc_Tcutil_is_tag_type(t1))
return t1;
if(!(Cyc_Unify_unify(t_result,t1)||(Cyc_Tcutil_is_arithmetic_type(t_result)&& Cyc_Tcutil_is_arithmetic_type(t1))&& !Cyc_Tcutil_is_tag_type(t1))){
void*result=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=
_tag_fat("Cannot use this operator in an assignment when ",sizeof(char),48U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=
_tag_fat("the arguments have types ",sizeof(char),26U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t1;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" and ",sizeof(char),6U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t2;_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp5,sizeof(void*),5));});
Cyc_Unify_unify(t_result,t1);
Cyc_Unify_explain_failure();
return result;}
# 802
return t_result;}}}}
# 806
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,0,e1);});
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);void**_Tmp1=topt;Cyc_Tcexp_tcExp(_Tmp0,_Tmp1,e2);});
return _check_null(e2->topt);}
# 813
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 816
struct Cyc_List_List*fields;
void*t1=_check_null(e->topt);
# 819
{void*_Tmp0=Cyc_Absyn_compress(t1);if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 2: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1==0){
# 821
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_double_type,1U);t1=Cyc_Absyn_double_type;goto _LL0;}else{goto _LL7;}case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f2){case Cyc_Absyn_Char_sz:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL6:
# 824
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,1U);t1=Cyc_Absyn_sint_type;goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7:
 goto _LL0;}_LL0:;}
# 828
for(fields=fs;fields!=0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _Tmp0=*((struct Cyc_Absyn_Datatypefield*)fields->hd);enum Cyc_Absyn_Scope _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp0.name;_Tmp3=_Tmp0.typs;_Tmp2=_Tmp0.loc;_Tmp1=_Tmp0.sc;{struct _tuple1*n=_Tmp4;struct Cyc_List_List*typs=_Tmp3;unsigned loc=_Tmp2;enum Cyc_Absyn_Scope sc=_Tmp1;
# 831
if(typs==0 || typs->tl!=0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple18*)typs->hd)).f1);
# 834
if(Cyc_Unify_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 838
for(fields=fs;fields!=0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _Tmp0=*((struct Cyc_Absyn_Datatypefield*)fields->hd);enum Cyc_Absyn_Scope _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp0.name;_Tmp3=_Tmp0.typs;_Tmp2=_Tmp0.loc;_Tmp1=_Tmp0.sc;{struct _tuple1*n=_Tmp4;struct Cyc_List_List*typs=_Tmp3;unsigned loc=_Tmp2;enum Cyc_Absyn_Scope sc=_Tmp1;
# 841
if(typs==0 || typs->tl!=0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple18*)typs->hd)).f1);
# 845
int bogus=0;
if(({struct Cyc_RgnOrder_RgnPO*_Tmp5=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp6=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp7=e;Cyc_Tcutil_coerce_arg(_Tmp5,_Tmp6,_Tmp7,t2,& bogus);}))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;}}}
# 849
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't find a field in ",sizeof(char),23U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)tu;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" to inject value of type ",sizeof(char),26U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t1;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp4,sizeof(void*),4));});
return 0;}
# 853
static void Cyc_Tcexp_check_all_aqual_bounds(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*fn_or_aggr_qual_bnd){
# 857
struct Cyc_List_List*qbit=fn_or_aggr_qual_bnd;for(0;qbit!=0;qbit=qbit->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)qbit->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*qv=_Tmp2;void*bnd=_Tmp1;
void*qvc=Cyc_Absyn_compress(qv);
void*_Tmp3;void*_Tmp4;switch(*((int*)qvc)){case 1: _Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)qvc)->f2;{void**ref=_Tmp4;
# 863
goto _LL3;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f1)){case 18: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2)->hd)==1){_Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2->hd)->f2;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2->tl;{void**ref=(void**)_Tmp4;struct Cyc_List_List*ebnd=_Tmp3;
# 865
if(!Cyc_Tcutil_check_aqual_bounds(0,(void*)ebnd->hd,bnd)&& !
Cyc_Tcutil_check_aqual_bounds(0,bnd,(void*)ebnd->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)ebnd->hd;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" <> ",sizeof(char),5U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)bnd;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),4));});
goto _LL3;}}else{goto _LLA;}}else{goto _LLA;}case 16: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)qvc)->f2)->hd)==1){
# 871
if(!({struct Cyc_List_List*_Tmp5=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp6=Cyc_Absyn_al_qual_type;Cyc_Tcutil_check_aqual_bounds(_Tmp5,_Tmp6,bnd);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)qvc;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" <> ",sizeof(char),5U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)bnd;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),4));});
goto _LL3;}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}default: _LLA:
# 875
 if(!({struct Cyc_List_List*_Tmp5=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp6=qvc;Cyc_Tcutil_check_aqual_bounds(_Tmp5,_Tmp6,bnd);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("alias qualifier bounds violated: ",sizeof(char),34U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)qvc;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" <> ",sizeof(char),5U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)bnd;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),4));});
goto _LL3;}_LL3:;}}}
# 884
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 890
struct Cyc_List_List*es=args;
int arg_cnt=0;
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);{struct Cyc_Tcenv_Tenv*te=_Tmp0;
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress(_check_null(e->topt));
# 899
void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;if(*((int*)t)==3){_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.rgn;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.nullable;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;{void*t1=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*rgn=_Tmp4;void*x=_Tmp3;void*b=_Tmp2;void*zt=_Tmp1;
# 904
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
# 906
Cyc_Tcutil_check_nonzero_bound(loc,b);
# 909
t1=({void*_Tmp7=Cyc_Absyn_compress(t1);Cyc_CurRgn_instantiate(_Tmp7,Cyc_Tcenv_curr_lifo_rgn(te));});{
void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;int _Tmp10;void*_Tmp11;void*_Tmp12;struct Cyc_Absyn_Tqual _Tmp13;void*_Tmp14;void*_Tmp15;if(*((int*)t1)==5){_Tmp15=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.tvars;_Tmp14=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.effect;_Tmp13=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.ret_tqual;_Tmp12=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.ret_type;_Tmp11=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.args;_Tmp10=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.c_varargs;_TmpF=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.cyc_varargs;_TmpE=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.rgn_po;_TmpD=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.qual_bnd;_TmpC=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.attributes;_TmpB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.requires_clause;_TmpA=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.requires_relns;_Tmp9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.ensures_clause;_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.ensures_relns;_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t1)->f1.return_value;{struct Cyc_List_List*tvars=_Tmp15;void*eff=_Tmp14;struct Cyc_Absyn_Tqual res_tq=_Tmp13;void*res_typ=_Tmp12;struct Cyc_List_List*args_info=_Tmp11;int c_vararg=_Tmp10;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpF;struct Cyc_List_List*rgn_po=_TmpE;struct Cyc_List_List*qb=_TmpD;struct Cyc_List_List*atts=_TmpC;struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_List_List*req_relns=_TmpA;struct Cyc_Absyn_Exp*ens=_Tmp9;struct Cyc_List_List*ens_relns=_Tmp8;struct Cyc_Absyn_Vardecl*ret_var=_Tmp7;
# 914
if(tvars!=0 || rgn_po!=0)
Cyc_Warn_err(e->loc,_tag_fat("function should have been instantiated prior to use -- probably a compiler bug",sizeof(char),79U),_tag_fat(0U,sizeof(void*),0));
# 918
if(topt!=0)Cyc_Unify_unify(res_typ,*topt);
# 920
while(es!=0 && args_info!=0){
# 922
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
void*t2=(*((struct _tuple9*)args_info->hd)).f2;
Cyc_Tcexp_tcExp(te,& t2,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp16=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp17=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp18=e1;Cyc_Tcutil_coerce_arg(_Tmp16,_Tmp17,_Tmp18,t2,& alias_coercion);})){
struct _fat_ptr s0=_tag_fat("actual argument has type ",sizeof(char),26U);
struct _fat_ptr s1=Cyc_Absynpp_typ2string(_check_null(e1->topt));
struct _fat_ptr s2=_tag_fat(" but formal has type ",sizeof(char),22U);
struct _fat_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _Tmp16=({unsigned long _Tmp17=({unsigned long _Tmp18=Cyc_strlen((struct _fat_ptr)s0);_Tmp18 + Cyc_strlen((struct _fat_ptr)s1);});_Tmp17 + Cyc_strlen((struct _fat_ptr)s2);});_Tmp16 + Cyc_strlen(s3);})>= 70U)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17;_Tmp17.tag=0,_Tmp17.f1=s0;_Tmp17;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=_tag_fat("\n\t",sizeof(char),3U);_Tmp18;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19;_Tmp19.tag=0,_Tmp19.f1=s1;_Tmp19;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=_tag_fat("\n",sizeof(char),2U);_Tmp1A;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=0,_Tmp1B.f1=s2;_Tmp1B;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=_tag_fat("\n\t",sizeof(char),3U);_Tmp1C;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=s3;_Tmp1D;});void*_Tmp1D[7];_Tmp1D[0]=& _Tmp16,_Tmp1D[1]=& _Tmp17,_Tmp1D[2]=& _Tmp18,_Tmp1D[3]=& _Tmp19,_Tmp1D[4]=& _Tmp1A,_Tmp1D[5]=& _Tmp1B,_Tmp1D[6]=& _Tmp1C;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp1D,sizeof(void*),7));});else{
# 934
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17;_Tmp17.tag=0,_Tmp17.f1=s0;_Tmp17;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp18;_Tmp18.tag=0,_Tmp18.f1=s1;_Tmp18;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp18=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19;_Tmp19.tag=0,_Tmp19.f1=s2;_Tmp19;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=s3;_Tmp1A;});void*_Tmp1A[4];_Tmp1A[0]=& _Tmp16,_Tmp1A[1]=& _Tmp17,_Tmp1A[2]=& _Tmp18,_Tmp1A[3]=& _Tmp19;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp1A,sizeof(void*),4));});}
Cyc_Unify_unify(_check_null(e1->topt),t2);
Cyc_Unify_explain_failure();}
# 939
if(alias_coercion)
({struct Cyc_List_List*_Tmp16=({struct Cyc_List_List*_Tmp17=_cycalloc(sizeof(struct Cyc_List_List));_Tmp17->hd=(void*)arg_cnt,_Tmp17->tl=*alias_arg_exps;_Tmp17;});*alias_arg_exps=_Tmp16;});
({unsigned _Tmp16=e1->loc;struct Cyc_List_List*_Tmp17=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp18=t2;Cyc_Tcexp_check_consume(_Tmp16,_Tmp17,_Tmp18,e1);});
es=es->tl;
args_info=args_info->tl;
++ arg_cnt;}{
# 949
int args_already_checked=0;
{struct Cyc_List_List*a=atts;for(0;a!=0;a=a->tl){
void*_Tmp16=(void*)a->hd;int _Tmp17;int _Tmp18;enum Cyc_Absyn_Format_Type _Tmp19;if(*((int*)_Tmp16)==19){_Tmp19=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp16)->f1;_Tmp18=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp16)->f2;_Tmp17=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp16)->f3;{enum Cyc_Absyn_Format_Type ft=_Tmp19;int fmt_arg_pos=_Tmp18;int arg_start_pos=_Tmp17;
# 953
{struct _handler_cons _Tmp1A;_push_handler(& _Tmp1A);{int _Tmp1B=0;if(setjmp(_Tmp1A.handler))_Tmp1B=1;if(!_Tmp1B){
# 955
{struct Cyc_Absyn_Exp*fmt_arg=({(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(args,fmt_arg_pos - 1);
# 957
struct Cyc_Core_Opt*fmt_args;
if(arg_start_pos==0)
fmt_args=0;else{
# 961
fmt_args=({struct Cyc_Core_Opt*_Tmp1C=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp1D=Cyc_List_nth_tail(args,arg_start_pos - 1);_Tmp1C->v=_Tmp1D;});_Tmp1C;});}
args_already_checked=1;
switch((int)ft){case Cyc_Absyn_Printf_ft:
# 965
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_format_types);
# 968
goto _LLF;case Cyc_Absyn_Scanf_ft:
 goto _LL15;default: _LL15:
# 971
 Cyc_Tcexp_check_format_args(te,fmt_arg,fmt_args,arg_start_pos - 1,alias_arg_exps,c_vararg,Cyc_Formatstr_get_scanf_types);
# 974
goto _LLF;}_LLF:;}
# 955
;_pop_handler();}else{void*_Tmp1C=(void*)Cyc_Core_get_exn_thrown();void*_Tmp1D;if(((struct Cyc_List_Nth_exn_struct*)_Tmp1C)->tag==Cyc_List_Nth){
# 976
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1F;_Tmp1F.tag=0,_Tmp1F.f1=_tag_fat("bad format arguments",sizeof(char),21U);_Tmp1F;});void*_Tmp1F[1];_Tmp1F[0]=& _Tmp1E;Cyc_Warn_err2(loc,_tag_fat(_Tmp1F,sizeof(void*),1));});goto _LL16;}else{_Tmp1D=_Tmp1C;{void*exn=_Tmp1D;_rethrow(exn);}}_LL16:;}}}
goto _LLA;}}else{
goto _LLA;}_LLA:;}}
# 981
if(args_info!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17;_Tmp17.tag=0,_Tmp17.f1=_tag_fat("too few arguments for function",sizeof(char),31U);_Tmp17;});void*_Tmp17[1];_Tmp17[0]=& _Tmp16;Cyc_Warn_err2(loc,_tag_fat(_Tmp17,sizeof(void*),1));});else{
# 984
if((es!=0 || c_vararg)|| cyc_vararg!=0){
if(c_vararg)
for(1;es!=0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}else{
if(cyc_vararg==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17;_Tmp17.tag=0,_Tmp17.f1=_tag_fat("too many arguments for function",sizeof(char),32U);_Tmp17;});void*_Tmp17[1];_Tmp17[0]=& _Tmp16;Cyc_Warn_err2(loc,_tag_fat(_Tmp17,sizeof(void*),1));});else{
# 991
struct Cyc_Absyn_VarargInfo _Tmp16=*cyc_vararg;int _Tmp17;void*_Tmp18;_Tmp18=_Tmp16.type;_Tmp17=_Tmp16.inject;{void*vt=_Tmp18;int inject=_Tmp17;
struct Cyc_Absyn_VarargCallInfo*vci;vci=_cycalloc(sizeof(struct Cyc_Absyn_VarargCallInfo)),vci->num_varargs=0,vci->injectors=0,vci->vai=cyc_vararg;
# 995
*vararg_call_info=vci;
# 997
if(!inject)
# 999
for(1;es!=0;(es=es->tl,arg_cnt ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
++ vci->num_varargs;
Cyc_Tcexp_tcExp(te,& vt,e1);
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp19=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp1A=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp1B=e1;Cyc_Tcutil_coerce_arg(_Tmp19,_Tmp1A,_Tmp1B,vt,& alias_coercion);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp19=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1A;_Tmp1A.tag=0,_Tmp1A.f1=_tag_fat("vararg requires type ",sizeof(char),22U);_Tmp1A;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1A=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1B;_Tmp1B.tag=2,_Tmp1B.f1=(void*)vt;_Tmp1B;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1B=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C;_Tmp1C.tag=0,_Tmp1C.f1=
_tag_fat(" but argument has type ",sizeof(char),24U);_Tmp1C;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=3,_Tmp1D.f1=(void*)e1->topt;_Tmp1D;});void*_Tmp1D[4];_Tmp1D[0]=& _Tmp19,_Tmp1D[1]=& _Tmp1A,_Tmp1D[2]=& _Tmp1B,_Tmp1D[3]=& _Tmp1C;Cyc_Tcexp_err_and_explain(loc,_tag_fat(_Tmp1D,sizeof(void*),4));});
if(alias_coercion)
({struct Cyc_List_List*_Tmp19=({struct Cyc_List_List*_Tmp1A=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1A->hd=(void*)arg_cnt,_Tmp1A->tl=*alias_arg_exps;_Tmp1A;});*alias_arg_exps=_Tmp19;});
({unsigned _Tmp19=e1->loc;struct Cyc_List_List*_Tmp1A=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp1B=vt;Cyc_Tcexp_check_consume(_Tmp19,_Tmp1A,_Tmp1B,e1);});}else{
# 1014
void*_Tmp19=Cyc_Absyn_compress(Cyc_Tcutil_pointer_elt_type(vt));void*_Tmp1A;void*_Tmp1B;if(*((int*)_Tmp19)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp19)->f1)==23){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp19)->f1)->f1.KnownDatatype.tag==2){_Tmp1B=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp19)->f1)->f1.KnownDatatype.val;_Tmp1A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp19)->f2;{struct Cyc_Absyn_Datatypedecl*td=_Tmp1B;struct Cyc_List_List*targs=_Tmp1A;
# 1018
struct Cyc_Absyn_Datatypedecl*_Tmp1C=*Cyc_Tcenv_lookup_datatypedecl(te,loc,td->name);{struct Cyc_Absyn_Datatypedecl*td=_Tmp1C;
struct Cyc_List_List*fields=0;
if(td->fields==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1E;_Tmp1E.tag=0,_Tmp1E.f1=_tag_fat("can't inject into abstract ",sizeof(char),28U);_Tmp1E;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1E=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1F;_Tmp1F.tag=2,_Tmp1F.f1=(void*)vt;_Tmp1F;});void*_Tmp1F[2];_Tmp1F[0]=& _Tmp1D,_Tmp1F[1]=& _Tmp1E;Cyc_Warn_err2(loc,_tag_fat(_Tmp1F,sizeof(void*),2));});else{
fields=(struct Cyc_List_List*)td->fields->v;}
# 1028
({void*_Tmp1D=Cyc_Tcutil_pointer_region(vt);Cyc_Unify_unify(_Tmp1D,Cyc_Tcenv_curr_rgn(te));});{
# 1030
struct Cyc_List_List*inst=Cyc_List_zip(td->tvs,targs);
for(1;es!=0;es=es->tl){
++ vci->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)es->hd;
# 1035
if(!args_already_checked){
Cyc_Tcexp_tcExp(te,0,e1);
({unsigned _Tmp1D=e1->loc;struct Cyc_List_List*_Tmp1E=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp1F=_check_null(e1->topt);Cyc_Tcexp_check_consume(_Tmp1D,_Tmp1E,_Tmp1F,e1);});}{
# 1039
struct Cyc_Absyn_Datatypefield*f=({struct Cyc_Tcenv_Tenv*_Tmp1D=te;struct Cyc_Absyn_Exp*_Tmp1E=e1;void*_Tmp1F=Cyc_Tcutil_pointer_elt_type(vt);struct Cyc_List_List*_Tmp20=inst;Cyc_Tcexp_tcInjection(_Tmp1D,_Tmp1E,_Tmp1F,_Tmp20,fields);});
if(f!=0)
({struct Cyc_List_List*_Tmp1D=({
struct Cyc_List_List*_Tmp1E=vci->injectors;Cyc_List_append(_Tmp1E,({struct Cyc_List_List*_Tmp1F=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1F->hd=f,_Tmp1F->tl=0;_Tmp1F;}));});
# 1041
vci->injectors=_Tmp1D;});}}}
# 1044
goto _LL1E;}}}}else{goto _LL21;}}else{goto _LL21;}}else{_LL21:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1C=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1D;_Tmp1D.tag=0,_Tmp1D.f1=_tag_fat("bad inject vararg type",sizeof(char),23U);_Tmp1D;});void*_Tmp1D[1];_Tmp1D[0]=& _Tmp1C;Cyc_Warn_err2(loc,_tag_fat(_Tmp1D,sizeof(void*),1));});goto _LL1E;}_LL1E:;}}}}}}
# 1052
Cyc_Tcexp_check_all_aqual_bounds(loc,te,qb);
if(*alias_arg_exps==0)
# 1062 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(te,loc,_check_null(eff));
# 1064
return res_typ;}}}else{
# 1066
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp17;_Tmp17.tag=0,_Tmp17.f1=_tag_fat("expected pointer to function but found ",sizeof(char),40U);_Tmp17;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp17=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp18;_Tmp18.tag=2,_Tmp18.f1=(void*)t;_Tmp18;});void*_Tmp18[2];_Tmp18[0]=& _Tmp16,_Tmp18[1]=& _Tmp17;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp18,sizeof(void*),2));});};}}}else{
# 1069
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("expected pointer to function but found ",sizeof(char),40U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp9,sizeof(void*),2));});};}}}
# 1073
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*exception_type=Cyc_Absyn_exn_type();
Cyc_Tcexp_tcExp(te,& exception_type,e);
if(!((int)({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp1=loc;void*_Tmp2=_check_null(e->topt);Cyc_Tcutil_castable(_Tmp0,_Tmp1,_Tmp2,exception_type);})))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expected ",sizeof(char),10U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)exception_type;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" but found ",sizeof(char),12U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)e->topt;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});
if(topt!=0)
return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1084
static void*Cyc_Tcexp_doInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1086
void*t1=Cyc_Absyn_compress(_check_null(e->topt));
{struct Cyc_Absyn_PtrAtts _Tmp0;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;if(*((int*)t1)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_tq;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts;{void*t=_Tmp2;struct Cyc_Absyn_Tqual tq=_Tmp1;struct Cyc_Absyn_PtrAtts atts=_Tmp0;
# 1089
{void*_Tmp3=Cyc_Absyn_compress(t);struct Cyc_Absyn_FnInfo _Tmp4;if(*((int*)_Tmp3)==5){_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp3)->f1;{struct Cyc_Absyn_FnInfo info=_Tmp4;
# 1091
struct Cyc_List_List*tvars=info.tvars;
struct Cyc_List_List*instantiation=0;
# 1094
for(1;ts!=0 && tvars!=0;(ts=ts->tl,tvars=tvars->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvars->hd,& Cyc_Kinds_bk);
({unsigned _Tmp5=loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;struct Cyc_List_List*_Tmp7=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_Tmp8=k;Cyc_Tctyp_check_type(_Tmp5,_Tmp6,_Tmp7,_Tmp8,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp6=({struct _tuple14*_Tmp7=_cycalloc(sizeof(struct _tuple14));_Tmp7->f0=(struct Cyc_Absyn_Tvar*)tvars->hd,_Tmp7->f1=(void*)ts->hd;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=instantiation;_Tmp5;});}
# 1100
info.tvars=tvars;
if(ts!=0)
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("too many type variables in instantiation",sizeof(char),41U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp6,sizeof(void*),1));});
# 1106
if(tvars==0){
({struct Cyc_List_List*_Tmp5=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,info.rgn_po);info.rgn_po=_Tmp5;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,info.rgn_po);
info.rgn_po=0;}{
# 1111
void*new_fn_typ=({struct Cyc_List_List*_Tmp5=instantiation;Cyc_Tcutil_substitute(_Tmp5,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp6->tag=5,_Tmp6->f1=info;_Tmp6;}));});
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp5;_Tmp5.elt_type=new_fn_typ,_Tmp5.elt_tq=tq,_Tmp5.ptr_atts=atts;_Tmp5;}));}}}else{
goto _LL5;}_LL5:;}
# 1115
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 1118
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting polymorphic type but found ",sizeof(char),38U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp2,sizeof(void*),2));});}
# 1122
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1124
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1126
({void*_Tmp0=Cyc_Absyn_pointer_expand(_check_null(e->topt),0);e->topt=_Tmp0;});
return Cyc_Tcexp_doInstantiate(te,loc,topt,e,ts);}
# 1131
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1133
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_Tmp3=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_ak:& Cyc_Kinds_mk;
# 1133
Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,_Tmp3,1,0,t);});
# 1135
Cyc_Tcutil_check_no_qual(loc,t);
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=_check_null(e->topt);
if(({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp1=loc;void*_Tmp2=t2;Cyc_Tcutil_silent_castable(_Tmp0,_Tmp1,_Tmp2,t);}))
*_check_null(c)=1U;else{
# 1141
enum Cyc_Absyn_Coercion crc=({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp1=loc;void*_Tmp2=t2;Cyc_Tcutil_castable(_Tmp0,_Tmp1,_Tmp2,t);});
if((int)crc!=0)
*_check_null(c)=crc;else{
if(Cyc_Tcutil_zero_to_null(t,e))
*_check_null(c)=1U;else{
# 1148
Cyc_Unify_unify(t2,t);{
void*result=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("cannot cast ",sizeof(char),13U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t2;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" to ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_expr_err(te,loc,& t,_tag_fat(_Tmp4,sizeof(void*),4));});
Cyc_Unify_explain_failure();
return result;}}}}{
# 1157
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=e->r,({void*_Tmp2=Cyc_Absyn_compress(t);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;if(*((int*)_Tmp0.f0)==33){if(*((int*)_Tmp0.f1)==3){_Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0.f0)->f1.fat_result;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.autoreleased;{int fat_result=_Tmp5;void*nbl=_Tmp4;void*bds=_Tmp3;void*zt=_Tmp2;void*rel=_Tmp1;
# 1161
if(((fat_result && !Cyc_Tcutil_force_type2bool(0,zt))&& Cyc_Tcutil_force_type2bool(0,nbl))&& !
Cyc_Tcutil_force_type2bool(0,rel)){
struct Cyc_Absyn_Exp*eopt=({void*_Tmp6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp6,bds);});
if(eopt!=0){
if(Cyc_Evexp_eval_const_uint_exp(e).f0==1U)
Cyc_Warn_warn(loc,_tag_fat("cast from ? pointer to * pointer will lose size information",sizeof(char),60U),_tag_fat(0U,sizeof(void*),0));}}
# 1168
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
 return t;};}}}
# 1174
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**topt2=0;
struct Cyc_Absyn_Tqual tq2=Cyc_Absyn_empty_tqual(0U);
int nullable=0;
if(topt!=0){
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;if(*((int*)_Tmp0)==3){_Tmp3=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_tq;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;{void**elttype=(void**)_Tmp3;struct Cyc_Absyn_Tqual tq=_Tmp2;void*n=_Tmp1;
# 1181
topt2=elttype;
tq2=tq;
nullable=Cyc_Tcutil_force_type2bool(0,n);
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 1194
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_Tmp1=topt2;Cyc_Tcexp_tcExpNoInst(_Tmp0,_Tmp1,e);});
# 1196
if(({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer_or_aggr(_Tmp0,_check_null(e->topt));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("cannot take the address of an alias-free path",sizeof(char),46U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 1201
{void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==23){_Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1205
({void*_Tmp3=Cyc_Absyn_add_exp(e1,e2,0U)->r;e0->r=_Tmp3;});
return Cyc_Tcexp_tcPlus(te,e1,e2);}}else{
goto _LL5;}_LL5:;}
# 1211
{void*_Tmp0=e->r;switch(*((int*)_Tmp0)){case 21: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3==1)
goto _LLE;else{goto _LLF;}case 22: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3==1){_LLE:
# 1214
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("cannot take the address of a @tagged union member",sizeof(char),50U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LLA;}else{goto _LLF;}default: _LLF:
 goto _LLA;}_LLA:;}{
# 1220
struct _tuple15 _Tmp0=Cyc_Tcutil_addressof_props(e);void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int is_const=_Tmp2;void*rgn=_Tmp1;
# 1225
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(is_const)
({int _Tmp3=tq.real_const=1;tq.print_const=_Tmp3;});
# 1229
return((nullable?Cyc_Absyn_star_type: Cyc_Absyn_at_type))(_check_null(e->topt),rgn,Cyc_Absyn_al_qual_type,tq,Cyc_Absyn_false_type,Cyc_Absyn_false_type);}}}
# 1233
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t){
# 1236
if(te->allow_valueof)
return Cyc_Absyn_uint_type;
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,& Cyc_Kinds_mk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("sizeof applied to type ",sizeof(char),24U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" which has unknown size here",sizeof(char),29U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});
if(topt==0)
return Cyc_Absyn_uint_type;{
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*tt=_Tmp1;
# 1246
if(({void*_Tmp2=tt;Cyc_Unify_unify(_Tmp2,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_Tmp3->tag=8,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_Absyn_sizeoftype_exp(t,0U);_Tmp3->f1=_Tmp4;});_Tmp3;}));}))
return Cyc_Absyn_compress(*topt);
return Cyc_Absyn_uint_type;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return Cyc_Absyn_uint_type;};}}
# 1253
void*Cyc_Tcexp_structfield_type(struct _fat_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp(*n,*sf->name)==0)
return sf->type;
return 0;}
# 1261
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1263
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,& Cyc_Kinds_mk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*l=fs;
void*t=t_orig;
for(1;l!=0;l=l->tl){
void*n=(void*)l->hd;
void*_Tmp0;_Tmp0=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)n)->f1;{struct _fat_ptr*n=_Tmp0;
# 1271
int bad_type=1;
{void*_Tmp1=Cyc_Absyn_compress(t);void*_Tmp2;switch(*((int*)_Tmp1)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)->f1.KnownAggr.tag==2){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp2;
# 1274
if((*adp)->impl==0)
goto _LL3;
_Tmp2=(*adp)->impl->fields;goto _LL7;}}else{goto _LL8;}}else{goto _LL8;}case 6: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp1)->f3;_LL7: {struct Cyc_List_List*fields=_Tmp2;
# 1278
void*t2=({(void*(*)(void*(*)(struct _fat_ptr*,struct Cyc_Absyn_Aggrfield*),struct _fat_ptr*,struct Cyc_List_List*))Cyc_List_find_c;})(Cyc_Tcexp_structfield_type,n,fields);
if(!((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("no field of struct/union has name %s",sizeof(char),37U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*n;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),2));});else{
# 1282
t=t2;}
bad_type=0;
goto _LL3;}default: _LL8:
 goto _LL3;}_LL3:;}
# 1287
if(bad_type){
if(l==fs)
({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" is not a known struct/union type",sizeof(char),34U);_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});else{
# 1291
struct _fat_ptr s=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t_orig));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("(%s)",sizeof(char),5U),_tag_fat(_Tmp2,sizeof(void*),1));});
{struct Cyc_List_List*x=fs;for(0;x!=l;x=x->tl){
void*_Tmp1=(void*)x->hd;void*_Tmp2;_Tmp2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_Tmp1)->f1;{struct _fat_ptr*n=_Tmp2;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)*n);_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s.%s",sizeof(char),6U),_tag_fat(_Tmp5,sizeof(void*),2));});goto _LLA;}_LLA:;}}
# 1296
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)s;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" == ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" is not a struct/union type",sizeof(char),28U);_Tmp5;});void*_Tmp5[4];_Tmp5[0]=& _Tmp1,_Tmp5[1]=& _Tmp2,_Tmp5[2]=& _Tmp3,_Tmp5[3]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),4));});}}
# 1299
goto _LL0;}_LL0:;}
# 1302
return Cyc_Absyn_uint_type;}}
# 1306
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig_e,struct Cyc_Absyn_Exp*e){
# 1308
te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te));
Cyc_Tcexp_tcExp(te,0,e);{
void*t=Cyc_Absyn_compress(_check_null(e->topt));
if(*((int*)t)==1){
# 1313
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
({void*_Tmp0=Cyc_Absyn_new_evar(& Cyc_Kinds_ako,({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);_Tmp1->v=_Tmp2;});_Tmp1;}));pi.elt_type=_Tmp0;});{
void*new_typ=Cyc_Absyn_pointer_type(pi);
Cyc_Unify_unify(t,new_typ);
t=Cyc_Absyn_compress(t);
goto _LL0;}}else{
goto _LL0;}_LL0:;{
# 1321
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)t)==3){_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.rgn;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;{void*t2=_Tmp3;void*rt=_Tmp2;void*b=_Tmp1;void*zt=_Tmp0;
# 1323
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
Cyc_Tcutil_check_nonzero_bound(loc,b);
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t2),& Cyc_Kinds_mk)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_Tmp4=Cyc_Absyn_compress(t2);if(*((int*)_Tmp4)==5){
# 1328
if(Cyc_Flags_tc_aggressive_warn)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("unnecessary dereference for function type",sizeof(char),42U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
# 1331
if(orig_e!=0)
orig_e->r=e->r;
return t;}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("cannot dereference abstract pointer type",sizeof(char),41U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});};}
# 1336
return t2;}}else{
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("expecting pointer type but found ",sizeof(char),34U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp6,sizeof(void*),2));});};}}}
# 1341
static void*Cyc_Tcexp_tcAggrMember2(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,void*aggr_type,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1345
({int _Tmp0=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_Tmp0;});{
void*_Tmp0=Cyc_Absyn_compress(aggr_type);enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.tag==2){_Tmp3=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.val;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 1348
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_decl_field(ad,f);
if(finfo==0)
return({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp5;_Tmp5.tag=6,_Tmp5.f1=ad;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" has no ",sizeof(char),9U);_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=*f;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" member",sizeof(char),8U);_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp8,sizeof(void*),4));});
# 1352
if(ad->impl->tagged)*is_tagged=1;{
void*t2=finfo->type;
if(ts!=0)
t2=({struct Cyc_List_List*_Tmp4=Cyc_List_zip(ad->tvs,ts);Cyc_Tcutil_substitute(_Tmp4,finfo->type);});{
struct Cyc_Absyn_Kind*t2_kind=Cyc_Tcutil_type_kind(t2);
# 1360
if(Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,t2_kind)&& !Cyc_Tcenv_abstract_val_ok(te)){
void*_Tmp4=Cyc_Absyn_compress(t2);if(*((int*)_Tmp4)==4)
goto _LL7;else{
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("cannot get member ",sizeof(char),19U);_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=*f;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(" since its type is abstract",sizeof(char),28U);_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp8,sizeof(void*),3));});}_LL7:;}
# 1368
if(((((int)ad->kind==1 && !ad->impl->tagged)&& !Cyc_Tcutil_is_bits_only_type(t2))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause==0)
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("cannot read union member ",sizeof(char),26U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*f;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=
_tag_fat(" since it is not `bits-only'",sizeof(char),29U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp7,sizeof(void*),3));});
if(ad->impl->exist_vars!=0){
# 1374
if(!({void*_Tmp4=t2;Cyc_Unify_unify(_Tmp4,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("must use pattern-matching to access field ",sizeof(char),43U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*f;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("\n\tdue to existential type variables.",sizeof(char),37U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp7,sizeof(void*),3));});}
# 1377
return t2;}}}}else{goto _LL5;}}else{goto _LL5;}case 6: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp3;
# 1379
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_field(fs,f);
if(finfo==0)
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("type ",sizeof(char),6U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)aggr_type;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" has no ",sizeof(char),9U);_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=*f;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat(" member",sizeof(char),8U);_Tmp9;});void*_Tmp9[5];_Tmp9[0]=& _Tmp4,_Tmp9[1]=& _Tmp5,_Tmp9[2]=& _Tmp6,_Tmp9[3]=& _Tmp7,_Tmp9[4]=& _Tmp8;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp9,sizeof(void*),5));});
# 1384
if((((int)k==1 && !Cyc_Tcutil_is_bits_only_type(finfo->type))&& !Cyc_Tcenv_in_notreadctxt(te))&& finfo->requires_clause==0)
# 1386
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("cannot read union member ",sizeof(char),26U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*f;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" since it is not `bits-only'",sizeof(char),29U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp7,sizeof(void*),3));});
return finfo->type;}default: _LL5:
# 1389
 return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("expecting struct or union, found ",sizeof(char),34U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)aggr_type;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp6,sizeof(void*),2));});};}}
# 1394
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1398
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_Tmp0,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,_check_null(e->topt),f,is_tagged,is_read);}
# 1403
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f,int*is_tagged,int*is_read){
# 1407
void*t2=({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_abstract_val_ok(te);unsigned _Tmp1=loc;Cyc_Tcexp_tcDeref(_Tmp0,_Tmp1,0,0,e);});
return Cyc_Tcexp_tcAggrMember2(te,loc,topt,t2,f,is_tagged,is_read);}
# 1413
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1415
struct Cyc_Tcenv_Tenv*te=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,0,e1);});
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,0,e2);});{
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_type(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("expecting int subscript, found ",sizeof(char),32U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t2;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Tcexp_expr_err(te,e2->loc,topt,_tag_fat(_Tmp2,sizeof(void*),2));});{
# 1424
int _Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;switch(*((int*)t1)){case 3: _Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.elt_tq;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.rgn;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.bounds;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t1)->f1.ptr_atts.zero_term;{void*t=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*rt=_Tmp3;void*b=_Tmp2;void*zt=_Tmp1;
# 1428
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
# 1444
if(emit_warning)
Cyc_Warn_warn(e2->loc,_tag_fat("subscript on thin, zero-terminated pointer could be expensive.",sizeof(char),63U),_tag_fat(0U,sizeof(void*),0));}else{
# 1448
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple16 _Tmp6=Cyc_Evexp_eval_const_uint_exp(e2);int _Tmp7;unsigned _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{unsigned i=_Tmp8;int known=_Tmp7;
if(known)
# 1453
({unsigned _Tmp9=loc;unsigned _TmpA=i;void*_TmpB=b;Cyc_Tcutil_check_bound(_Tmp9,_TmpA,_TmpB,Cyc_Tcenv_abstract_val_ok(te));});}}else{
# 1461
Cyc_Tcutil_check_nonzero_bound(loc,b);}}
# 1464
Cyc_Tcenv_check_rgn_accessible(te,loc,rt);
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_mk)&& !Cyc_Tcenv_abstract_val_ok(te))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("can't subscript an abstract pointer",sizeof(char),36U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(e1->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
return t;}case 6: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f1==Cyc_Absyn_StructA){_Tmp0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f2;_Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t1)->f3;if((int)_Tmp0){int is_tuple=_Tmp0;struct Cyc_List_List*fields=_Tmp5;
# 1469
struct _tuple16 _Tmp6=Cyc_Evexp_eval_const_uint_exp(e2);int _Tmp7;unsigned _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{unsigned i=_Tmp8;int known=_Tmp7;
if(!known)
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=
_tag_fat("tuple projection cannot use sizeof or offsetof",sizeof(char),47U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Tcexp_expr_err(te,loc,0,_tag_fat(_TmpA,sizeof(void*),1));});{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp9=Cyc_Absyn_tuple_field_designator((int)i);void*_TmpA;_TmpA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp9)->f1;{struct _fat_ptr*n=_TmpA;
({void*_TmpB=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct));_TmpC->tag=21,_TmpC->f1=e1,_TmpC->f2=n,_TmpC->f3=0,({int _TmpD=!Cyc_Tcenv_in_notreadctxt(te);_TmpC->f4=_TmpD;});_TmpC;});orig->r=_TmpB;});{
struct Cyc_Absyn_Aggrfield*finfo=Cyc_Absyn_lookup_field(fields,n);
if(finfo==0)
return({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("index is ",sizeof(char),10U);_TmpC;});struct Cyc_Warn_Int_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Int_Warn_Warg_struct _TmpD;_TmpD.tag=12,_TmpD.f1=(int)i;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat(" but tuple has only ",sizeof(char),21U);_TmpE;});struct Cyc_Warn_Int_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Int_Warn_Warg_struct _TmpF;_TmpF.tag=12,({
int _Tmp10=Cyc_List_length(fields);_TmpF.f1=_Tmp10;});_TmpF;});struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat(" fields",sizeof(char),8U);_Tmp10;});void*_Tmp10[5];_Tmp10[0]=& _TmpB,_Tmp10[1]=& _TmpC,_Tmp10[2]=& _TmpD,_Tmp10[3]=& _TmpE,_Tmp10[4]=& _TmpF;Cyc_Tcexp_expr_err(te,loc,0,_tag_fat(_Tmp10,sizeof(void*),5));});
# 1480
return finfo->type;}}}}}else{goto _LL5;}}else{goto _LL5;}default: _LL5:
 return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("subscript applied to ",sizeof(char),22U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)t1;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp8,sizeof(void*),2));});};}}}
# 1486
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*orig_exp,void**topt,struct _tuple9*targ,struct Cyc_List_List*des){
# 1491
void*_Tmp0;_Tmp0=targ->f2;{void*t=_Tmp0;
({unsigned _Tmp1=loc;struct Cyc_Tcenv_Tenv*_Tmp2=te;struct Cyc_List_List*_Tmp3=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_Tmp4=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Kinds_ak:& Cyc_Kinds_mk;
# 1492
Cyc_Tctyp_check_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,1,0,t);});
# 1494
({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,_Tmp2->f2=des;_Tmp2;});orig_exp->r=_Tmp1;});
Cyc_Tcexp_resolve_unresolved_mem(loc,& t,orig_exp,des);
Cyc_Tcexp_tcExpNoInst(te,topt,orig_exp);
return _check_null(orig_exp->topt);}}
# 1507 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1510
void*res_t2;
int num_es=Cyc_List_length(des);
struct Cyc_List_List*es=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd;}),des);
void*res=Cyc_Absyn_new_evar(& Cyc_Kinds_mko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*sz_rexp;sz_rexp=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct)),sz_rexp->tag=0,({union Cyc_Absyn_Cnst _Tmp0=Cyc_Absyn_Int_c(1U,num_es);sz_rexp->f1=_Tmp0;});{
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)sz_rexp,loc);
# 1518
if(zero_term){
struct Cyc_Absyn_Exp*e=({(struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*,int))Cyc_List_nth;})(es,num_es - 1);
if(!Cyc_Tcutil_is_zero(e))
Cyc_Warn_err(e->loc,_tag_fat("zero-terminated array doesn't end with zero.",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));}
# 1523
sz_exp->topt=Cyc_Absyn_uint_type;
res_t2=({void*_Tmp0=res;struct Cyc_Absyn_Tqual _Tmp1=
(unsigned)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1524
struct Cyc_Absyn_Exp*_Tmp2=sz_exp;Cyc_Absyn_array_type(_Tmp0,_Tmp1,_Tmp2,
# 1526
zero_term?Cyc_Absyn_true_type: Cyc_Absyn_false_type,0U);});
# 1528
{struct Cyc_List_List*es2=es;for(0;es2!=0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1531
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp1=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp2=res;Cyc_Tcutil_coerce_list(_Tmp0,_Tmp1,_Tmp2,es);}))
# 1533
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("elements of array do not all have the same type (",sizeof(char),50U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)res;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat(")",sizeof(char),2U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(((struct Cyc_Absyn_Exp*)_check_null(es)->hd)->loc,_tag_fat(_Tmp3,sizeof(void*),3));});
# 1536
{int offset=0;for(0;des!=0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f0;
if(ds!=0){
void*_Tmp0=(void*)ds->hd;void*_Tmp1;if(*((int*)_Tmp0)==1){
# 1541
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("only array index designators are supported",sizeof(char),43U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),1));});goto _LL0;}else{_Tmp1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 1543
Cyc_Tcexp_tcExpInitializer(te,0,e);{
struct _tuple16 _Tmp2=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp3;unsigned _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{unsigned i=_Tmp4;int known=_Tmp3;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("index designator cannot use sizeof or offsetof",sizeof(char),47U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp6,sizeof(void*),1));});else{
if(i!=(unsigned)offset)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("expecting index designator ",sizeof(char),28U);_Tmp6;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp7;_Tmp7.tag=12,_Tmp7.f1=offset;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" but found ",sizeof(char),12U);_Tmp8;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp9;_Tmp9.tag=12,_Tmp9.f1=(int)i;_Tmp9;});void*_Tmp9[4];_Tmp9[0]=& _Tmp5,_Tmp9[1]=& _Tmp6,_Tmp9[2]=& _Tmp7,_Tmp9[3]=& _Tmp8;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp9,sizeof(void*),4));});}
goto _LL0;}}}}_LL0:;}}}
# 1553
return res_t2;}}struct _tuple19{void**f0;struct Cyc_Absyn_Tqual*f1;void**f2;};
# 1558
static struct _tuple19 Cyc_Tcexp_ptrEltAtts(void**topt){
if(!((unsigned)topt))
return({struct _tuple19 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=0,_Tmp0.f2=0;_Tmp0;});{
void*_Tmp0=Cyc_Absyn_compress(*topt);struct Cyc_Absyn_ArrayInfo _Tmp1;struct Cyc_Absyn_PtrInfo _Tmp2;switch(*((int*)_Tmp0)){case 3: _Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo x=_Tmp2;
# 1563
return({struct _tuple19 _Tmp3;({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=x.elt_type;_Tmp5;});_Tmp3.f0=_Tmp4;}),({struct Cyc_Absyn_Tqual*_Tmp4=({struct Cyc_Absyn_Tqual*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tqual));*_Tmp5=x.elt_tq;_Tmp5;});_Tmp3.f1=_Tmp4;}),({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=x.ptr_atts.zero_term;_Tmp5;});_Tmp3.f2=_Tmp4;});_Tmp3;});}case 4: _Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_ArrayInfo x=_Tmp1;
return({struct _tuple19 _Tmp3;({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=x.elt_type;_Tmp5;});_Tmp3.f0=_Tmp4;}),({struct Cyc_Absyn_Tqual*_Tmp4=({struct Cyc_Absyn_Tqual*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tqual));*_Tmp5=x.tq;_Tmp5;});_Tmp3.f1=_Tmp4;}),({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=x.zero_term;_Tmp5;});_Tmp3.f2=_Tmp4;});_Tmp3;});}default:
 return({struct _tuple19 _Tmp3;_Tmp3.f0=0,_Tmp3.f1=0,_Tmp3.f2=0;_Tmp3;});};}}
# 1570
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1573
Cyc_Tcexp_tcExp(te,0,bound);
{void*_Tmp0=Cyc_Absyn_compress(_check_null(bound->topt));void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp1;
# 1578
if((int)Cyc_Tcenv_new_status(te)==2){
struct Cyc_Absyn_Exp*b=({void*_Tmp2=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_Tmp2,Cyc_Absyn_valueof_exp(t,0U),0,1U,0U);});
b->topt=bound->topt;
bound=b;}
# 1583
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1585
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting unsigned int, found ",sizeof(char),31U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)bound->topt;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(bound->loc,_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}
# 1589
if(!vd->tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("comprehension index variable is not declared const!",sizeof(char),52U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
# 1592
if(te->le!=0)
te=Cyc_Tcenv_new_block(loc,te);{
# 1595
struct _tuple19 _Tmp0=Cyc_Tcexp_ptrEltAtts(topt);void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void**topt2=_Tmp3;struct Cyc_Absyn_Tqual*tqopt=_Tmp2;void**ztopt=_Tmp1;
# 1598
void*t=Cyc_Tcexp_tcExp(te,topt2,body);
# 1600
({unsigned _Tmp4=body->loc;struct Cyc_List_List*_Tmp5=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp6=t;Cyc_Tcexp_check_consume(_Tmp4,_Tmp5,_Tmp6,body);});
if(te->le==0){
# 1603
if(!Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bound is not constant",sizeof(char),22U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(bound->loc,_tag_fat(_Tmp5,sizeof(void*),1));});
if(!Cyc_Tcutil_is_const_exp(body))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("body is not constant",sizeof(char),21U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(body->loc,_tag_fat(_Tmp5,sizeof(void*),1));});}
# 1608
if(ztopt!=0 && Cyc_Tcutil_force_type2bool(0,*ztopt)){
# 1610
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_uint_exp(1U,0U);e1->topt=Cyc_Absyn_uint_type;
bound=Cyc_Absyn_add_exp(bound,e1,0U);bound->topt=Cyc_Absyn_uint_type;
*is_zero_term=1;}
# 1614
({unsigned _Tmp4=body->loc;struct Cyc_List_List*_Tmp5=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp6=_check_null(body->topt);Cyc_Tcexp_check_consume(_Tmp4,_Tmp5,_Tmp6,body);});
# 1616
return({void*_Tmp4=t;struct Cyc_Absyn_Tqual _Tmp5=tqopt==0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_Tmp6=bound;Cyc_Absyn_array_type(_Tmp4,_Tmp5,_Tmp6,
ztopt==0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1621
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1624
Cyc_Tcexp_tcExp(te,0,bound);
{void*_Tmp0=Cyc_Absyn_compress(_check_null(bound->topt));void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp1;
# 1629
if((int)Cyc_Tcenv_new_status(te)==2){
struct Cyc_Absyn_Exp*b=({void*_Tmp2=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_Tmp2,Cyc_Absyn_valueof_exp(t,0U),0,1U,0U);});
b->topt=bound->topt;
bound=b;}
# 1634
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1636
 if(!Cyc_Tcutil_coerce_uint_type(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting unsigned int, found ",sizeof(char),31U);_Tmp3;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp4;_Tmp4.tag=3,_Tmp4.f1=(void*)bound->topt;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(bound->loc,_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}{
# 1639
struct _tuple19 _Tmp0=Cyc_Tcexp_ptrEltAtts(topt);void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void**topt2=_Tmp3;struct Cyc_Absyn_Tqual*tqopt=_Tmp2;void**ztopt=_Tmp1;
# 1642
({unsigned _Tmp4=loc;struct Cyc_Tcenv_Tenv*_Tmp5=te;struct Cyc_List_List*_Tmp6=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_Tmp4,_Tmp5,_Tmp6,& Cyc_Kinds_mk,1,1,t);});
if(topt2!=0)
Cyc_Unify_unify(*topt2,t);
# 1646
if(te->le==0 && !Cyc_Tcutil_is_const_exp(bound))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bound is not constant",sizeof(char),22U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(bound->loc,_tag_fat(_Tmp5,sizeof(void*),1));});
# 1650
if(ztopt!=0 && Cyc_Tcutil_force_type2bool(0,*ztopt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("non-initializing comprehensions do not currently support @zeroterm arrays",sizeof(char),74U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});
# 1654
return({void*_Tmp4=t;struct Cyc_Absyn_Tqual _Tmp5=tqopt==0?Cyc_Absyn_empty_tqual(0U):*tqopt;struct Cyc_Absyn_Exp*_Tmp6=bound;Cyc_Absyn_array_type(_Tmp4,_Tmp5,_Tmp6,
ztopt==0?Cyc_Absyn_false_type:*ztopt,0U);});}}}
# 1658
static int Cyc_Tcexp_ensure_typqual_nothrow(void*t){
void*_Tmp0;switch(*((int*)t)){case 5: _Tmp0=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*i=_Tmp0;
# 1661
return Cyc_List_mem(Cyc_Atts_attribute_cmp,i->attributes,(void*)& Cyc_Atts_No_throw_att_val);}case 3: _Tmp0=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_Tmp0;
# 1665
return Cyc_Tcexp_ensure_typqual_nothrow(pi->elt_type);}default:
# 1667
 return 1;};}
# 1671
static int Cyc_Tcexp_ensure_nothrow_initializer(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==14){_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp1;
return Cyc_Tcexp_ensure_nothrow_initializer(e1);}}else{
return Cyc_Tcexp_ensure_typqual_nothrow(_check_null(e->topt));};}struct _tuple20{struct Cyc_Absyn_Aggrfield*f0;struct Cyc_Absyn_Exp*f1;};
# 1688 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1692
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt!=0){
ad=_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp0=ad;_Tmp0;});}else{
# 1698
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1698
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 1701
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("unbound struct/union name ",sizeof(char),27U);_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=*tn;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),2));});
return topt!=0?*topt: Cyc_Absyn_void_type;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 1704
*ad_opt=ad;
*tn=ad->name;}
# 1707
if(ad->impl==0)
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("can't construct abstract ",sizeof(char),26U);_Tmp1;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp2;_Tmp2.tag=6,_Tmp2.f1=ad;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Tcexp_expr_err(te,loc,0,_tag_fat(_Tmp2,sizeof(void*),2));});{
# 1714
struct Cyc_Tcenv_Tenv*te2=(int)Cyc_Tcenv_new_status(te)==1?Cyc_Tcenv_set_new_status(2U,te): te;
# 1720
struct _tuple13 env=({struct _tuple13 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_Tcenv_lookup_type_vars(te2);_Tmp0.f0=_Tmp1;}),_Tmp0.f1=Cyc_Core_heap_region;_Tmp0;});
struct Cyc_List_List*all_inst=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);
struct Cyc_List_List*exist_inst=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,ad->impl->exist_vars);
struct Cyc_List_List*all_typs=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),all_inst);
struct Cyc_List_List*exist_typs=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),exist_inst);
struct Cyc_List_List*inst=Cyc_List_append(all_inst,exist_inst);
struct Cyc_List_List*qb_inst=0;
{struct Cyc_List_List*qbit=ad->impl->qual_bnd;for(0;qbit!=0;qbit=qbit->tl){
qb_inst=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));({void*_Tmp3=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)qbit->hd)).f0);_Tmp2->f0=_Tmp3;}),({
void*_Tmp3=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)qbit->hd)).f1);_Tmp2->f1=_Tmp3;});_Tmp2;});
# 1728
_Tmp0->hd=_Tmp1;}),_Tmp0->tl=qb_inst;_Tmp0;});}}{
# 1731
void*res_typ;
# 1736
if(topt!=0){
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.tag==2){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.val;_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Aggrdecl**adptr2=_Tmp2;struct Cyc_List_List*all_typs2=_Tmp1;
# 1739
if(*adptr2!=*adptr)
goto _LL9;
{struct Cyc_List_List*ats=all_typs;for(0;ats!=0 && all_typs2!=0;(
ats=ats->tl,all_typs2=all_typs2->tl)){
Cyc_Unify_unify((void*)ats->hd,(void*)all_typs2->hd);}}
# 1745
res_typ=*topt;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 res_typ=({union Cyc_Absyn_AggrInfo _Tmp3=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_Tmp3,all_typs);});}_LL5:;}else{
# 1750
res_typ=({union Cyc_Absyn_AggrInfo _Tmp0=Cyc_Absyn_KnownAggr(adptr);Cyc_Absyn_aggr_type(_Tmp0,all_typs);});}{
# 1753
struct Cyc_List_List*user_ex_ts=*ts;
struct Cyc_List_List*ex_ts=exist_typs;
while(user_ex_ts!=0 && ex_ts!=0){
# 1757
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te2;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,& Cyc_Kinds_ak,1,0,(void*)user_ex_ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)user_ex_ts->hd);
Cyc_Unify_unify((void*)user_ex_ts->hd,(void*)ex_ts->hd);
user_ex_ts=user_ex_ts->tl;
ex_ts=ex_ts->tl;}
# 1763
if(user_ex_ts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("too many explicit witness types",sizeof(char),32U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 1766
*ts=exist_typs;{
# 1769
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(Cyc_Core_heap_region,loc,args,ad->kind,ad->impl->fields);
for(1;fields!=0;fields=fields->tl){
int bogus=0;
struct _tuple20*_Tmp0=(struct _tuple20*)fields->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct Cyc_Absyn_Aggrfield*field=_Tmp2;struct Cyc_Absyn_Exp*field_exp=_Tmp1;
void*inst_fieldtyp=Cyc_Tcutil_substitute(inst,field->type);
void*ftyp=({struct Cyc_Tcenv_Tenv*_Tmp3=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_Tmp3,& inst_fieldtyp,field_exp);});
# 1777
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp3=Cyc_Tcenv_curr_rgnpo(te2);struct Cyc_List_List*_Tmp4=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp5=field_exp;Cyc_Tcutil_coerce_arg(_Tmp3,_Tmp4,_Tmp5,inst_fieldtyp,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("field ",sizeof(char),7U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*field->name;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" of ",sizeof(char),5U);_Tmp6;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp7;_Tmp7.tag=6,_Tmp7.f1=ad;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(" expects type ",sizeof(char),15U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)inst_fieldtyp;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat(" != ",sizeof(char),5U);_TmpA;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpA=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpB;_TmpB.tag=3,_TmpB.f1=(void*)field_exp->topt;_TmpB;});void*_TmpB[8];_TmpB[0]=& _Tmp3,_TmpB[1]=& _Tmp4,_TmpB[2]=& _Tmp5,_TmpB[3]=& _Tmp6,_TmpB[4]=& _Tmp7,_TmpB[5]=& _Tmp8,_TmpB[6]=& _Tmp9,_TmpB[7]=& _TmpA;Cyc_Tcexp_err_and_explain(field_exp->loc,_tag_fat(_TmpB,sizeof(void*),8));});{
# 1781
int nothrow_fld=Cyc_List_mem(Cyc_Atts_attribute_cmp,field->attributes,(void*)& Cyc_Atts_No_throw_att_val);
# 1784
if(nothrow_fld && !Cyc_Tcexp_ensure_nothrow_initializer(field_exp))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("field ",sizeof(char),7U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*field->name;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" can only hold a no_throw function",sizeof(char),35U);_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_Warn_err2(field_exp->loc,_tag_fat(_Tmp6,sizeof(void*),3));});}}}{
# 1788
struct Cyc_List_List*rpo_inst=0;
{struct Cyc_List_List*rpo=ad->impl->rgn_po;for(0;rpo!=0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));({void*_Tmp3=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f0);_Tmp2->f0=_Tmp3;}),({
void*_Tmp3=Cyc_Tcutil_substitute(inst,(*((struct _tuple0*)rpo->hd)).f1);_Tmp2->f1=_Tmp3;});_Tmp2;});
# 1790
_Tmp0->hd=_Tmp1;}),_Tmp0->tl=rpo_inst;_Tmp0;});}}
# 1793
rpo_inst=Cyc_List_imp_rev(rpo_inst);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,rpo_inst);
# 1796
Cyc_Tcexp_check_all_aqual_bounds(loc,te2,qb_inst);
return res_typ;}}}}}}
# 1800
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*orig,void**ts,int is_tuple,struct Cyc_List_List*args){
# 1804
void*struct_type;
if(*_check_null(ts)!=0)struct_type=_check_null(*ts);else{
if(topt!=0)struct_type=*topt;else{
struct_type=Cyc_Absyn_new_evar(& Cyc_Kinds_mko,Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 1809
{void*_Tmp0=Cyc_Absyn_compress(struct_type);enum Cyc_Absyn_AggrKind _Tmp1;void*_Tmp2;union Cyc_Absyn_AggrInfo _Tmp3;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==25){_Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{union Cyc_Absyn_AggrInfo ainfo=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 1812
struct _tuple1*name;
struct Cyc_Absyn_Aggrdecl*adopt=0;
{void*_Tmp4;if(ainfo.UnknownAggr.tag==1){_Tmp4=ainfo.UnknownAggr.val.f1;{struct _tuple1*n=_Tmp4;
name=n;goto _LL9;}}else{_Tmp4=*ainfo.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
name=ad->name;adopt=ad;goto _LL9;}}_LL9:;}
# 1818
({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp5->tag=28,_Tmp5->f1=name,_Tmp5->f2=ts,_Tmp5->f3=args,_Tmp5->f4=adopt;_Tmp5;});orig->r=_Tmp4;});
Cyc_Tcexp_tcExpNoInst(te,topt,orig);
goto _LL0;}}else{goto _LL7;}case 1:  {
# 1824
struct Cyc_List_List*fs=0;
struct Cyc_Core_Opt*tvs=Cyc_Tcenv_lookup_opt_type_vars(te);
int i=0;
{struct Cyc_List_List*a=args;for(0;a!=0;(a=a->tl,++ i)){
struct _tuple17 _Tmp4=*((struct _tuple17*)a->hd);void*_Tmp5;_Tmp5=_Tmp4.f0;{struct Cyc_List_List*ds=_Tmp5;
if(ds==0)({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("tcAnonStruct -- missing designators!",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));{
struct _fat_ptr*n;
{void*_Tmp6=(void*)ds->hd;void*_Tmp7;if(*((int*)_Tmp6)==1){_Tmp7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp6)->f1;{struct _fat_ptr*name=_Tmp7;
n=name;goto _LL11;}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("tcAnonStruct -- designator is array!",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));}_LL11:;}{
# 1835
struct Cyc_Absyn_Aggrfield*f;f=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),f->name=n,({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);f->tq=_Tmp6;}),({
void*_Tmp6=Cyc_Absyn_new_evar(& Cyc_Kinds_mko,tvs);f->type=_Tmp6;}),f->width=0,f->attributes=0,f->requires_clause=0;
# 1838
fs=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=f,_Tmp6->tl=fs;_Tmp6;});}}}}}
# 1840
fs=Cyc_List_imp_rev(fs);
({void*_Tmp4=struct_type;Cyc_Unify_unify(_Tmp4,(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp5->tag=6,_Tmp5->f1=0U,_Tmp5->f2=is_tuple,_Tmp5->f3=fs;_Tmp5;}));});
*ts=struct_type;
_Tmp1=0U;_Tmp2=fs;goto _LL6;}case 6: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;_LL6: {enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp2;
# 1845
if((int)k==1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("expecting struct but found union",sizeof(char),33U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});{
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(Cyc_Core_heap_region,loc,args,0U,fs);
for(1;fields!=0;fields=fields->tl){
int bogus=0;
struct _tuple20*_Tmp4=(struct _tuple20*)fields->hd;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4->f0;_Tmp5=_Tmp4->f1;{struct Cyc_Absyn_Aggrfield*field=_Tmp6;struct Cyc_Absyn_Exp*field_exp=_Tmp5;
({struct Cyc_Tcenv_Tenv*_Tmp7=Cyc_Tcenv_clear_abstract_val_ok(te);void**_Tmp8=& field->type;Cyc_Tcexp_tcExpInitializer(_Tmp7,_Tmp8,field_exp);});
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp7=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp8=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp9=field_exp;Cyc_Tcutil_coerce_arg(_Tmp7,_Tmp8,_Tmp9,field->type,& bogus);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("field ",sizeof(char),7U);_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=*field->name;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=
_tag_fat(" of struct expects type ",sizeof(char),25U);_TmpA;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpB;_TmpB.tag=2,_TmpB.f1=(void*)field->type;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" != ",sizeof(char),5U);_TmpC;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpC=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpD;_TmpD.tag=3,_TmpD.f1=(void*)field_exp->topt;_TmpD;});void*_TmpD[6];_TmpD[0]=& _Tmp7,_TmpD[1]=& _Tmp8,_TmpD[2]=& _Tmp9,_TmpD[3]=& _TmpA,_TmpD[4]=& _TmpB,_TmpD[5]=& _TmpC;Cyc_Tcexp_err_and_explain(field_exp->loc,_tag_fat(_TmpD,sizeof(void*),6));});}}
# 1858
goto _LL0;}}default: _LL7:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("tcAnonStruct: wrong type ",sizeof(char),26U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)struct_type;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" for exp ",sizeof(char),10U);_Tmp7;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp8;_Tmp8.tag=4,_Tmp8.f1=orig;_Tmp8;});void*_Tmp8[4];_Tmp8[0]=& _Tmp4,_Tmp8[1]=& _Tmp5,_Tmp8[2]=& _Tmp6,_Tmp8[3]=& _Tmp7;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp8,sizeof(void*),4));});}_LL0:;}
# 1861
return struct_type;}
# 1865
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 1868
struct _tuple13 env=({struct _tuple13 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_Tcenv_lookup_type_vars(te);_Tmp0.f0=_Tmp1;}),_Tmp0.f1=Cyc_Core_heap_region;_Tmp0;});
struct Cyc_List_List*inst=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);
struct Cyc_List_List*all_typs=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),inst);
void*res=({union Cyc_Absyn_DatatypeFieldInfo _Tmp0=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_Tmp0,all_typs);});
# 1873
if(topt!=0){
void*_Tmp0=Cyc_Absyn_compress(*topt);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){
Cyc_Unify_unify(*topt,res);goto _LL0;}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 1879
struct Cyc_List_List*ts=tuf->typs;
for(1;es!=0 && ts!=0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=Cyc_Tcutil_substitute(inst,(*((struct _tuple18*)ts->hd)).f1);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_List_List*_Tmp1=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp2=e;Cyc_Tcutil_coerce_arg(_Tmp0,_Tmp1,_Tmp2,t,& bogus);})){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("datatype constructor ",sizeof(char),22U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=tuf->name;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat(" expects argument of type ",sizeof(char),27U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat(" but this argument has type ",sizeof(char),29U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,
e->topt==0?_Tmp6.f1=(struct _fat_ptr)_tag_fat("?",sizeof(char),2U):({struct _fat_ptr _Tmp7=Cyc_Absynpp_typ2string(_check_null(e->topt));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[6];_Tmp6[0]=& _Tmp0,_Tmp6[1]=& _Tmp1,_Tmp6[2]=& _Tmp2,_Tmp6[3]=& _Tmp3,_Tmp6[4]=& _Tmp4,_Tmp6[5]=& _Tmp5;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp6,sizeof(void*),6));});
Cyc_Unify_explain_failure();}}
# 1892
if(es!=0)
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=
_tag_fat("too many arguments for datatype constructor ",sizeof(char),45U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=tuf->name;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,_tag_fat(_Tmp2,sizeof(void*),2));});
if(ts!=0)
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=
_tag_fat("too few arguments for datatype constructor ",sizeof(char),44U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=tuf->name;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp2,sizeof(void*),2));});
return res;}}
# 1902
static int Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned loc,void**topt,void*t){
# 1904
if(Cyc_Tcutil_is_bits_only_type(t)|| allow_zero && Cyc_Tcutil_zeroable_type(t))
return 1;
# 1907
if(topt==0)
return 0;{
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*elt_typ=_Tmp1;
# 1911
Cyc_Unify_unify(elt_typ,t);
return Cyc_Tcexp_check_malloc_type(allow_zero,loc,0,t);}}else{
return 0;};}}
# 1917
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 1919
return Cyc_Absyn_heap_rgn_type;}
# 1926
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**ropt,struct Cyc_Absyn_Exp**aqopt,void***t,struct Cyc_Absyn_Exp**e,enum Cyc_Absyn_MallocKind*mknd,int*is_fat){
# 1930
void*rgn=Cyc_Absyn_heap_rgn_type;
if(*ropt!=0){
# 1934
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,_check_null(*ropt));
void*_Tmp0=Cyc_Absyn_compress(handle_type);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*r=_Tmp1;
# 1939
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1943
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting region_t type but found ",sizeof(char),35U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)handle_type;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(_check_null(*ropt)->loc,_tag_fat(_Tmp4,sizeof(void*),2));});
goto _LL0;}_LL0:;}else{
# 1949
if(topt!=0){
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))
rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{
# 1956
void*aqual_type=Cyc_Absyn_al_qual_type;
if(*aqopt!=0){
void*expected_type=
Cyc_Absyn_aqual_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*qhandle_type=Cyc_Tcexp_tcExp(te,& expected_type,_check_null(*aqopt));
void*_Tmp0=Cyc_Absyn_compress(qhandle_type);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==19){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*aq=_Tmp1;
# 1963
aqual_type=aq;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 1966
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting aqual_t type but found ",sizeof(char),34U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)qhandle_type;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(_check_null(*ropt)->loc,_tag_fat(_Tmp4,sizeof(void*),2));});
goto _LL5;}_LL5:;}else{
# 1970
if(topt!=0){
void*aq=({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_eval_aquals_of(_Tmp0,*topt);});
if(aq!=0){
if(aq!=0){if(*((int*)aq)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)aq)->f1)==17){if(((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)aq)->f1)->f1==Cyc_Absyn_Unique_qual){
# 1975
aqual_type=Cyc_Absyn_un_qual_type;
({struct Cyc_Absyn_Exp*_Tmp0=Cyc_Absyn_uniqueaqual_exp();*aqopt=_Tmp0;});
goto _LLA;}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
 goto _LLA;}_LLA:;}}}
# 1983
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,& Cyc_Absyn_uint_type,*e);});{
# 1992 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if((int)*mknd==1){
if(*t==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("calloc with empty type",sizeof(char),23U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
elt_type=*(*t);
({unsigned _Tmp0=loc;struct Cyc_Tcenv_Tenv*_Tmp1=te;struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_Tmp0,_Tmp1,_Tmp2,& Cyc_Kinds_mk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("calloc cannot be used with type ",sizeof(char),33U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)elt_type;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat("\n\t(type needs initialization)",sizeof(char),30U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});
num_elts=*e;
one_elt=0;}else{
# 2007
void*er=(*e)->r;
retry_sizeof: {
void*_Tmp0;void*_Tmp1;switch(*((int*)er)){case 17: _Tmp1=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)er)->f1;{void*t2=_Tmp1;
# 2011
elt_type=t2;
({void**_Tmp2=({void**_Tmp3=_cycalloc(sizeof(void*));*_Tmp3=elt_type;_Tmp3;});*t=_Tmp2;});
num_elts=Cyc_Absyn_uint_exp(1U,0U);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_type,num_elts);
one_elt=1;
goto _LLF;}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f1==Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2)->tl)->tl==0){_Tmp1=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2->hd;_Tmp0=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)er)->f2->tl->hd;{struct Cyc_Absyn_Exp*e1=_Tmp1;struct Cyc_Absyn_Exp*e2=_Tmp0;
# 2018
{struct _tuple0 _Tmp2=({struct _tuple0 _Tmp3;_Tmp3.f0=e1->r,_Tmp3.f1=e2->r;_Tmp3;});void*_Tmp3;if(*((int*)_Tmp2.f0)==17){_Tmp3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp2.f0)->f1;{void*t1=_Tmp3;
# 2020
e1=e2;
_Tmp3=t1;goto _LL1A;}}else{if(*((int*)_Tmp2.f1)==17){_Tmp3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp2.f1)->f1;_LL1A: {void*t2=_Tmp3;
# 2024
if(!Cyc_Tcexp_check_malloc_type(0,loc,topt,t2)){
# 2027
if(!Cyc_Tcexp_check_malloc_type(1,loc,topt,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("malloc cannot be used with type ",sizeof(char),33U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t2;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=
_tag_fat("\n\t(type needs initialization)",sizeof(char),30U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),3));});else{
# 2031
*mknd=1U;}}
# 2033
elt_type=t2;
({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=elt_type;_Tmp5;});*t=_Tmp4;});
num_elts=e1;
one_elt=0;
goto _LL16;}}else{
goto No_sizeof;}}_LL16:;}
# 2040
goto _LLF;}}else{goto _LL14;}}else{goto _LL14;}}else{goto _LL14;}}else{goto _LL14;}default: _LL14:
# 2042
 No_sizeof: {
# 2045
struct Cyc_Absyn_Exp*real_e=*e;
{void*_Tmp2=real_e->r;void*_Tmp3;if(*((int*)_Tmp2)==14){_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Exp*e=_Tmp3;
real_e=e;goto _LL1D;}}else{
goto _LL1D;}_LL1D:;}
# 2050
{void*_Tmp2=Cyc_Absyn_compress(_check_null(real_e->topt));void*_Tmp3;if(*((int*)_Tmp2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2!=0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2->hd;{void*tagt=_Tmp3;
# 2052
{void*_Tmp4=Cyc_Absyn_compress(tagt);void*_Tmp5;if(*((int*)_Tmp4)==8){_Tmp5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Exp*vexp=_Tmp5;
er=vexp->r;goto retry_sizeof;}}else{
goto _LL27;}_LL27:;}
# 2056
goto _LL22;}}else{goto _LL25;}}else{goto _LL25;}}else{_LL25:
 goto _LL22;}_LL22:;}
# 2059
elt_type=Cyc_Absyn_char_type;
({void**_Tmp2=({void**_Tmp3=_cycalloc(sizeof(void*));*_Tmp3=elt_type;_Tmp3;});*t=_Tmp2;});
num_elts=*e;
one_elt=0;}
# 2064
goto _LLF;}_LLF:;}}
# 2068
*is_fat=!one_elt;
# 2071
{void*_Tmp0;if(*((int*)elt_type)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_type)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_type)->f1)->f1.KnownAggr.tag==2){_Tmp0=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_type)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp0;
# 2073
if(ad->impl!=0 && ad->impl->exist_vars!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("malloc with existential types not yet implemented",sizeof(char),50U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL2C;}}else{goto _LL2F;}}else{goto _LL2F;}}else{_LL2F:
 goto _LL2C;}_LL2C:;}{
# 2080
void*(*ptr_maker)(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*)=Cyc_Absyn_at_type;
void*zero_term=Cyc_Absyn_false_type;
if(topt!=0){
void*_Tmp0=Cyc_Absyn_compress(*topt);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0)==3){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.autoreleased;{void*n=_Tmp4;void*b=_Tmp3;void*zt=_Tmp2;void*rel=_Tmp1;
# 2085
zero_term=zt;
if(Cyc_Tcutil_force_type2bool(0,rel))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("cannot malloc() an autoreleased pointer",sizeof(char),40U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
if(Cyc_Tcutil_force_type2bool(0,n))
ptr_maker=Cyc_Absyn_star_type;
# 2092
if(Cyc_Tcutil_force_type2bool(0,zt)&& !((int)*mknd==1)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("converting malloc to calloc to ensure zero-termination",sizeof(char),55U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});
*mknd=1U;}{
# 2098
struct Cyc_Absyn_Exp*eopt=({void*_Tmp5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp5,b);});
if(eopt!=0 && !one_elt){
struct Cyc_Absyn_Exp*upper_exp=eopt;
int is_constant=Cyc_Evexp_c_can_eval(num_elts);
if(is_constant && Cyc_Evexp_same_uint_const_exp(upper_exp,num_elts)){
*is_fat=0;
return({void*_Tmp5=elt_type;void*_Tmp6=rgn;void*_Tmp7=aqual_type;struct Cyc_Absyn_Tqual _Tmp8=Cyc_Absyn_empty_tqual(0U);void*_Tmp9=b;void*_TmpA=zero_term;Cyc_Absyn_atb_type(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Absyn_false_type);});}{
# 2106
void*_Tmp5=Cyc_Absyn_compress(_check_null(num_elts->topt));void*_Tmp6;if(*((int*)_Tmp5)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2!=0){_Tmp6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2->hd;{void*tagtyp=_Tmp6;
# 2108
struct Cyc_Absyn_Exp*tagtyp_exp=({void*_Tmp7=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_Tmp7,Cyc_Absyn_valueof_exp(tagtyp,0U),0,1U,0U);});
# 2110
if(Cyc_Evexp_same_uint_const_exp(tagtyp_exp,upper_exp)){
*is_fat=0;
return({void*_Tmp7=elt_type;void*_Tmp8=rgn;void*_Tmp9=aqual_type;struct Cyc_Absyn_Tqual _TmpA=Cyc_Absyn_empty_tqual(0U);void*_TmpB=b;void*_TmpC=zero_term;Cyc_Absyn_atb_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,_TmpB,_TmpC,Cyc_Absyn_false_type);});}
# 2114
goto _LL36;}}else{goto _LL39;}}else{goto _LL39;}}else{_LL39:
 goto _LL36;}_LL36:;}}
# 2118
goto _LL31;}}}else{
goto _LL31;}_LL31:;}
# 2121
if(!one_elt)ptr_maker=Cyc_Absyn_fatptr_type;
return({void*(*_Tmp0)(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*)=ptr_maker;void*_Tmp1=elt_type;void*_Tmp2=rgn;void*_Tmp3=aqual_type;struct Cyc_Absyn_Tqual _Tmp4=Cyc_Absyn_empty_tqual(0U);void*_Tmp5=zero_term;_Tmp0(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_false_type);});}}}}
# 2126
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2128
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(te2,0,e1);{
void*t1=_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(te2,& t1,e2);{
void*t1=_check_null(e1->topt);
void*t2=_check_null(e2->topt);
# 2135
{void*_Tmp0=Cyc_Absyn_compress(t1);if(*((int*)_Tmp0)==4){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("cannot assign to an array",sizeof(char),26U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});goto _LL0;}else{
goto _LL0;}_LL0:;}
# 2140
if(!Cyc_Tcutil_is_boxed(t1)&& !Cyc_Tcutil_is_pointer_type(t1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("swap not allowed for non-pointer or non-word-sized types",sizeof(char),57U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 2144
if(!Cyc_Absyn_is_lvalue(e1))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("swap non-lvalue",sizeof(char),16U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Tcexp_expr_err(te,e1->loc,topt,_tag_fat(_Tmp1,sizeof(void*),1));});
if(!Cyc_Absyn_is_lvalue(e2))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("swap non-lvalue",sizeof(char),16U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Tcexp_expr_err(te,e2->loc,topt,_tag_fat(_Tmp1,sizeof(void*),1));});
# 2149
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Unify_unify(t1,t2))
return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("type mismatch: ",sizeof(char),16U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)t1;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t2;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp4,sizeof(void*),4));});
return Cyc_Absyn_void_type;}}}
# 2157
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_Tmp0,s,1);});
# 2160
while(1){
void*_Tmp0=s->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
# 2164
void*t=_check_null(e->topt);
if(!({void*_Tmp2=t;Cyc_Unify_unify(_Tmp2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("statement expression returns type ",sizeof(char),35U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)t;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Tcexp_err_and_explain(loc,_tag_fat(_Tmp4,sizeof(void*),2));});
return t;}case 2: _Tmp1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s2=_Tmp1;
s=s2;continue;}case 12: _Tmp1=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp1;
s=s1;continue;}default:
 return({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=
_tag_fat("statement expression must end with expression",sizeof(char),46U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Tcexp_expr_err(te,loc,topt,_tag_fat(_Tmp3,sizeof(void*),1));});};}}
# 2175
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp*e,struct _fat_ptr*f){
void*t=Cyc_Absyn_compress(({struct Cyc_Tcenv_Tenv*_Tmp0=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_Tmp0,0,e);}));
{void*_Tmp0;if(*((int*)t)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==25){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.tag==2){_Tmp0=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp0;
# 2179
if(((int)ad->kind==1 && ad->impl!=0)&& ad->impl->tagged)
goto _LL0;
goto _LL4;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("expecting @tagged union but found ",sizeof(char),35U);_Tmp2;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3;_Tmp3.tag=2,_Tmp3.f1=(void*)t;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_err2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});goto _LL0;}_LL0:;}
# 2184
return Cyc_Absyn_uint_type;}
# 2187
static void*Cyc_Tcexp_tcAssert(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcTest(te,e,_tag_fat("@assert",sizeof(char),8U));
if(!Cyc_Absyn_no_side_effects_exp(e))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("@assert expression may have side effects",sizeof(char),41U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return Cyc_Absyn_sint_type;}
# 2195
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp**qual_hndl,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2200
void*rgn=Cyc_Absyn_heap_rgn_type;
te=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(1U,te));
if(*rgn_handle!=0){
# 2205
void*expected_type=
Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,_check_null(*rgn_handle));
void*_Tmp0=Cyc_Absyn_compress(handle_type);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*r=_Tmp1;
# 2210
rgn=r;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 2214
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting region_t type but found ",sizeof(char),35U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)handle_type;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(_check_null(*rgn_handle)->loc,_tag_fat(_Tmp4,sizeof(void*),2));});
# 2216
goto _LL0;}_LL0:;}else{
# 2218
if(topt!=0){
# 2221
void*optrgn=Cyc_Absyn_void_type;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn))
rgn=Cyc_Tcexp_mallocRgn(optrgn);}}{
# 2227
void*aqual=Cyc_Absyn_al_qual_type;
if(*qual_hndl!=0){
# 2231
void*expected_type=
Cyc_Absyn_aqual_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,Cyc_Tcenv_lookup_opt_type_vars(te)));
void*qhndl_type=Cyc_Tcexp_tcExp(te,& expected_type,_check_null(*qual_hndl));
void*_Tmp0=Cyc_Absyn_compress(qhndl_type);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==19){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*aq=_Tmp1;
# 2236
aqual=aq;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 2255 "tcexp.cyc"
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("expecting aqual_t type but found ",sizeof(char),34U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)qhndl_type;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(_check_null(*qual_hndl)->loc,_tag_fat(_Tmp4,sizeof(void*),2));});
# 2257
goto _LL5;}_LL5:;}else{
# 2260
if(topt!=0){
void*aq=({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_eval_aquals_of(_Tmp0,*topt);});
if(aq!=0){
if(aq!=0){if(*((int*)aq)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)aq)->f1)==17){if(((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)aq)->f1)->f1==Cyc_Absyn_Unique_qual){
# 2265
aqual=Cyc_Absyn_un_qual_type;
({struct Cyc_Absyn_Exp*_Tmp0=Cyc_Absyn_uniqueaqual_exp();*qual_hndl=_Tmp0;});
goto _LLA;}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
 goto _LLA;}_LLA:;}}}{
# 2273
void*_Tmp0=e1->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 26:
 goto _LL13;case 27: _LL13: {
# 2278
void*res_typ=Cyc_Tcexp_tcExpNoPromote(te,topt,e1);
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("tcNew: comprehension returned non-array type",sizeof(char),45U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),1));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,aqual,1);
if(topt!=0){
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_Tmp3=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp4=loc;void*_Tmp5=res_typ;Cyc_Tcutil_silent_castable(_Tmp3,_Tmp4,_Tmp5,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
res_typ=*topt;}}
# 2289
return res_typ;}case 35: _Tmp2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*nopt=_Tmp2;struct Cyc_List_List*des=_Tmp1;
# 2291
({void*_Tmp3=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct));_Tmp4->tag=25,_Tmp4->f1=des;_Tmp4;});e1->r=_Tmp3;});
_Tmp2=des;goto _LL17;}case 25: _Tmp2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL17: {struct Cyc_List_List*des=_Tmp2;
# 2294
void**elt_typ_opt=0;
int zero_term=0;
if(topt!=0){
void*_Tmp3=Cyc_Absyn_compress(*topt);void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp3)==3){_Tmp5=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.ptr_atts.zero_term;{void**elt_typ=(void**)_Tmp5;void*zt=_Tmp4;
# 2300
elt_typ_opt=elt_typ;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL1E;}}else{
goto _LL1E;}_LL1E:;}{
# 2305
void*res_typ=Cyc_Tcexp_tcArray(te,e1->loc,elt_typ_opt,0,zero_term,des);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array_type(res_typ))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("tcExpNoPromote on Array_e returned non-array type",sizeof(char),50U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),1));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,aqual,0);
if(topt!=0){
# 2314
if(!Cyc_Unify_unify(*topt,res_typ)&&({
struct Cyc_RgnOrder_RgnPO*_Tmp3=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp4=loc;void*_Tmp5=res_typ;Cyc_Tcutil_silent_castable(_Tmp3,_Tmp4,_Tmp5,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
res_typ=*topt;}}
# 2320
return res_typ;}}case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wstring_c.tag){case 8:  {
# 2325
void*topt2=({void*_Tmp3=Cyc_Absyn_char_type;void*_Tmp4=rgn;void*_Tmp5=aqual;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_const_tqual(0U);void*_Tmp7=Cyc_Absyn_fat_bound_type;void*_Tmp8=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_false_type);});
# 2327
void*t=Cyc_Tcexp_tcExp(te,& topt2,e1);
return({void*_Tmp3=t;void*_Tmp4=rgn;void*_Tmp5=aqual;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2328
void*_Tmp8=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_false_type);});}case 9:  {
# 2332
void*topt2=({void*_Tmp3=Cyc_Absyn_wchar_type();void*_Tmp4=rgn;void*_Tmp5=aqual;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_const_tqual(0U);void*_Tmp7=Cyc_Absyn_fat_bound_type;void*_Tmp8=Cyc_Absyn_true_type;Cyc_Absyn_atb_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_false_type);});
# 2334
void*t=Cyc_Tcexp_tcExp(te,& topt2,e1);
return({void*_Tmp3=t;void*_Tmp4=rgn;void*_Tmp5=aqual;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_uint_exp(1U,0U));
# 2335
void*_Tmp8=Cyc_Absyn_false_type;Cyc_Absyn_atb_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,_Tmp8,Cyc_Absyn_false_type);});}default: goto _LL1C;}default: _LL1C: {
# 2342
void*bogus=Cyc_Absyn_void_type;
void**topt2=0;
if(topt!=0){
void*_Tmp3=Cyc_Absyn_compress(*topt);struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;switch(*((int*)_Tmp3)){case 3: _Tmp5=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp3)->f1.elt_tq;{void**elttype=(void**)_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;
# 2347
topt2=elttype;goto _LL23;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==23){
# 2351
bogus=*topt;
topt2=& bogus;
goto _LL23;}else{goto _LL28;}default: _LL28:
 goto _LL23;}_LL23:;}{
# 2357
void*telt=Cyc_Tcexp_tcExp(te,topt2,e1);
# 2359
({unsigned _Tmp3=e1->loc;struct Cyc_List_List*_Tmp4=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp5=telt;Cyc_Tcexp_check_consume(_Tmp3,_Tmp4,_Tmp5,e1);});{
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type_with_aqual(te,aqual);
pi.elt_type=telt;
pi.ptr_atts.rgn=rgn;
({void*_Tmp3=Cyc_Absyn_bounds_one();pi.ptr_atts.bounds=_Tmp3;});
pi.ptr_atts.zero_term=Cyc_Absyn_false_type;
pi.ptr_atts.autoreleased=Cyc_Absyn_false_type;{
void*res_typ=Cyc_Absyn_pointer_type(pi);
if((topt!=0 && !Cyc_Unify_unify(*topt,res_typ))&&({
struct Cyc_RgnOrder_RgnPO*_Tmp3=Cyc_Tcenv_curr_rgnpo(te);unsigned _Tmp4=loc;void*_Tmp5=res_typ;Cyc_Tcutil_silent_castable(_Tmp3,_Tmp4,_Tmp5,*topt);})){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(e,*topt,3U);
res_typ=*topt;}
# 2373
return res_typ;}}}}};}}}
# 2379
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
if(Cyc_Tcutil_is_array_type(t))
({void*_Tmp0=t=({void*_Tmp1=t;void*_Tmp2=Cyc_Tcutil_addressof_props(e).f1;Cyc_Tcutil_promote_array(_Tmp1,_Tmp2,Cyc_Absyn_al_qual_type,0);});e->topt=_Tmp0;});
return t;}
# 2389
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2393
({unsigned _Tmp0=e->loc;struct Cyc_List_List*_Tmp1=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp2=t;Cyc_Tcexp_check_consume(_Tmp0,_Tmp1,_Tmp2,e);});{
void*_Tmp0=e->r;switch(*((int*)_Tmp0)){case 25:
 goto _LL4;case 26: _LL4:
 goto _LL6;case 27: _LL6:
 goto _LL8;case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.tag){case 9: _LL8:
 goto _LLA;case 8: _LLA:
 return t;default: goto _LLB;}default: _LLB:
# 2401
 if(Cyc_Tcutil_is_array_type(t))
({void*_Tmp1=t=({void*_Tmp2=t;void*_Tmp3=Cyc_Tcutil_addressof_props(e).f1;Cyc_Tcutil_promote_array(_Tmp2,_Tmp3,Cyc_Absyn_al_qual_type,0);});e->topt=_Tmp1;});
return t;};}}
# 2414 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
Cyc_Tcexp_tcExpNoInst(te,topt,e);{
void*cached_type=_check_null(e->topt);
void*t=({void*_Tmp0=Cyc_Absyn_compress(Cyc_Absyn_pointer_expand(_check_null(e->topt),0));e->topt=_Tmp0;});
# 2419
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==12){_Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp1;
return t;}}else{
goto _LL0;}_LL0:;}{
# 2424
void*_Tmp0;if(*((int*)t)==3){_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;{void*t2=_Tmp0;
# 2426
void*_Tmp1=Cyc_Absyn_compress(t2);struct Cyc_Absyn_FnInfo _Tmp2;if(*((int*)_Tmp1)==5){_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp1)->f1;{struct Cyc_Absyn_FnInfo info=_Tmp2;
# 2428
struct _tuple13 env=({struct _tuple13 _Tmp3;({struct Cyc_List_List*_Tmp4=Cyc_Tcenv_lookup_type_vars(te);_Tmp3.f0=_Tmp4;}),_Tmp3.f1=Cyc_Core_heap_region;_Tmp3;});
struct Cyc_List_List*inst=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,info.tvars);
# 2432
struct Cyc_List_List*ts=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),inst);
struct Cyc_Absyn_Exp*inner=Cyc_Absyn_copy_exp(e);
({void*_Tmp3=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct));_Tmp4->tag=13,_Tmp4->f1=inner,_Tmp4->f2=ts;_Tmp4;});e->r=_Tmp3;});
({void*_Tmp3=Cyc_Tcexp_doInstantiate(te,e->loc,topt,inner,ts);e->topt=_Tmp3;});
return _check_null(e->topt);}}else{
return t;};}}else{
# 2439
return t;};}}}
# 2449
static int Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2451
struct Cyc_List_List*decls=0;
# 2453
{void*_Tmp0=fn_exp->r;void*_Tmp1;if(*((int*)_Tmp0)==10){_Tmp1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp1;
# 2455
{void*_Tmp2=e->r;void*_Tmp3;if(*((int*)_Tmp2)==10){_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp2)->f2;{struct Cyc_List_List*es2=_Tmp3;
# 2457
struct Cyc_List_List*arg_exps=alias_arg_exps;
int arg_cnt=0;
while(arg_exps!=0){
while(arg_cnt!=(int)arg_exps->hd){
if(es==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bad count ",sizeof(char),11U);_Tmp5;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp6;_Tmp6.tag=12,_Tmp6.f1=arg_cnt;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("/",sizeof(char),2U);_Tmp7;});struct Cyc_Warn_Int_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Int_Warn_Warg_struct _Tmp8;_Tmp8.tag=12,_Tmp8.f1=(int)arg_exps->hd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=
_tag_fat(" for alias coercion!",sizeof(char),21U);_Tmp9;});void*_Tmp9[5];_Tmp9[0]=& _Tmp4,_Tmp9[1]=& _Tmp5,_Tmp9[2]=& _Tmp6,_Tmp9[3]=& _Tmp7,_Tmp9[4]=& _Tmp8;({
# 2462
(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp9,sizeof(void*),5));});
# 2464
++ arg_cnt;
es=es->tl;
es2=_check_null(es2)->tl;}{
# 2469
struct _tuple12 _Tmp4=({struct Cyc_Absyn_Exp*_Tmp5=(struct Cyc_Absyn_Exp*)_check_null(es)->hd;Cyc_Tcutil_insert_alias(_Tmp5,Cyc_Tcutil_copy_type(_check_null(((struct Cyc_Absyn_Exp*)_check_null(es2)->hd)->topt)));});int _Tmp5;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp4.f0;_Tmp6=_Tmp4.f1;_Tmp5=_Tmp4.f2;{struct Cyc_Absyn_Decl*d=_Tmp7;struct Cyc_Absyn_Exp*ve=_Tmp6;int succ=_Tmp5;
if(!succ)
return 0;
es->hd=(void*)ve;
decls=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=d,_Tmp8->tl=decls;_Tmp8;});
arg_exps=arg_exps->tl;}}}
# 2476
goto _LL5;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("not a function call!",sizeof(char),21U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});}_LL5:;}
# 2479
goto _LL0;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("not a function call!",sizeof(char),21U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp3,sizeof(void*),1));});}_LL0:;}
# 2483
while(decls!=0){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)decls->hd;
fn_exp=({struct Cyc_Absyn_Stmt*_Tmp0=({struct Cyc_Absyn_Decl*_Tmp1=d;struct Cyc_Absyn_Stmt*_Tmp2=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_Tmp1,_Tmp2,e->loc);});Cyc_Absyn_stmt_exp(_Tmp0,e->loc);});
decls=decls->tl;}
# 2489
e->topt=0;
e->r=fn_exp->r;
return 1;}
# 2495
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned loc=e->loc;
void*t;
# 2499
{void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;enum Cyc_Absyn_Incrementor _Tmp5;enum Cyc_Absyn_Primop _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;switch(*((int*)_Tmp0)){case 12: _Tmp9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp9;
# 2504
Cyc_Tcexp_tcExpNoInst(te,0,e2);
e->topt=_check_null(e2->topt);
return;}case 10: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4==0)
# 2508
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("unresolved function: tcExpNoInst",sizeof(char),33U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpB,sizeof(void*),1));});else{_Tmp9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_List_List*es=_Tmp8;struct Cyc_Absyn_VarargCallInfo**vci=_Tmp7;
# 2531
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _TmpA;_push_handler(& _TmpA);{int _TmpB=0;if(setjmp(_TmpA.handler))_TmpB=1;if(!_TmpB){fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_TmpC=(void*)Cyc_Core_get_exn_thrown();void*_TmpD;if(((struct Cyc_Core_Failure_exn_struct*)_TmpC)->tag==Cyc_Core_Failure){
# 2536
ok=0;
fn_exp=e;
goto _LL57;}else{_TmpD=_TmpC;{void*exn=_TmpD;_rethrow(exn);}}_LL57:;}}}
# 2540
t=Cyc_Tcexp_tcFnCall(te,loc,topt,e1,es,vci,& alias_arg_exps);
if(alias_arg_exps!=0 && ok){
alias_arg_exps=Cyc_List_imp_rev(alias_arg_exps);
if(Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps))
Cyc_Tcexp_tcExpNoInst(te,topt,e);}
# 2546
goto _LL0;}}case 35: _Tmp9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*nopt=_Tmp9;struct Cyc_List_List*des=_Tmp8;
# 2512
Cyc_Tcexp_resolve_unresolved_mem(loc,topt,e,des);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}case 0: _Tmp9=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst*c=_Tmp9;
# 2516
t=Cyc_Tcexp_tcConst(te,loc,topt,c,e);goto _LL0;}case 1: _Tmp9=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void**b=_Tmp9;
t=Cyc_Tcexp_tcVar(te,loc,topt,e,b);goto _LL0;}case 2:
 t=Cyc_Absyn_sint_type;goto _LL0;case 3: _Tmp6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp6;struct Cyc_List_List*es=_Tmp9;
t=Cyc_Tcexp_tcPrimop(te,loc,topt,p,es);goto _LL0;}case 5: _Tmp9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;enum Cyc_Absyn_Incrementor i=_Tmp5;
t=Cyc_Tcexp_tcIncrement(te,loc,topt,e1,i);goto _LL0;}case 4: _Tmp9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Core_Opt*popt=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
t=Cyc_Tcexp_tcAssignOp(te,loc,topt,e1,popt,e2);goto _LL0;}case 6: _Tmp9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp8;struct Cyc_Absyn_Exp*e3=_Tmp7;
t=Cyc_Tcexp_tcConditional(te,loc,topt,e1,e2,e3);goto _LL0;}case 7: _Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp8;
t=Cyc_Tcexp_tcAnd(te,loc,e1,e2);goto _LL0;}case 8: _Tmp9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp8;
t=Cyc_Tcexp_tcOr(te,loc,e1,e2);goto _LL0;}case 9: _Tmp9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp8;
t=Cyc_Tcexp_tcSeqExp(te,loc,topt,e1,e2);goto _LL0;}case 11: _Tmp9=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 2547
t=Cyc_Tcexp_tcThrow(te,loc,topt,e1);goto _LL0;}case 13: _Tmp9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e2=_Tmp9;struct Cyc_List_List*ts=_Tmp8;
t=Cyc_Tcexp_tcInstantiate(te,loc,topt,e2,ts);goto _LL0;}case 14: _Tmp9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*t1=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp8;enum Cyc_Absyn_Coercion*c=_Tmp7;
t=Cyc_Tcexp_tcCast(te,loc,topt,t1,e1,c);goto _LL0;}case 15: _Tmp9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
t=Cyc_Tcexp_tcAddress(te,loc,e,topt,e1);goto _LL0;}case 16: _Tmp9=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp**rgn_handle=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp**qual_hndl=_Tmp7;
t=Cyc_Tcexp_tcNew(te,loc,topt,rgn_handle,qual_hndl,e,e1);goto _LL0;}case 18: _Tmp9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
_Tmp9=Cyc_Tcexp_tcExpNoPromote(te,0,e1);goto _LL2A;}case 17: _Tmp9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL2A: {void*t1=_Tmp9;
t=Cyc_Tcexp_tcSizeof(te,loc,topt,t1);goto _LL0;}case 19: _Tmp9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t1=_Tmp9;struct Cyc_List_List*l=_Tmp8;
t=Cyc_Tcexp_tcOffsetof(te,loc,topt,t1,l);goto _LL0;}case 20: _Tmp9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
t=Cyc_Tcexp_tcDeref(te,loc,topt,e,e1);goto _LL0;}case 21: _Tmp9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*f=_Tmp8;int*is_tagged=_Tmp7;int*is_read=_Tmp4;
# 2557
t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 22: _Tmp9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*f=_Tmp8;int*is_tagged=_Tmp7;int*is_read=_Tmp4;
# 2559
t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,e1,f,is_tagged,is_read);goto _LL0;}case 23: _Tmp9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp8;
t=Cyc_Tcexp_tcSubscript(te,loc,topt,e,e1,e2);goto _LL0;}case 24: _Tmp9=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple9*t1=_Tmp9;struct Cyc_List_List*des=_Tmp8;
t=Cyc_Tcexp_tcCompoundLit(te,loc,e,topt,t1,des);goto _LL0;}case 25: _Tmp9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*des=_Tmp9;
# 2565
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt!=0){
void*_TmpA=Cyc_Absyn_compress(*topt);void*_TmpB;void*_TmpC;void*_TmpD;if(*((int*)_TmpA)==4){_TmpD=(void**)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TmpA)->f1.elt_type;_TmpC=(struct Cyc_Absyn_Tqual*)&((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TmpA)->f1.tq;_TmpB=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TmpA)->f1.zero_term;{void**et=(void**)_TmpD;struct Cyc_Absyn_Tqual*etq=(struct Cyc_Absyn_Tqual*)_TmpC;void*zt=_TmpB;
# 2571
elt_topt=et;
elt_tqopt=etq;
zero_term=Cyc_Tcutil_force_type2bool(0,zt);
goto _LL5C;}}else{
goto _LL5C;}_LL5C:;}
# 2577
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,des);goto _LL0;}case 36: _Tmp9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp9;
# 2579
t=Cyc_Tcexp_tcStmtExp(te,loc,topt,s);goto _LL0;}case 26: _Tmp9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Vardecl*vd=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;int*iszeroterm=_Tmp4;
# 2581
t=Cyc_Tcexp_tcComprehension(te,loc,topt,vd,e1,e2,iszeroterm);goto _LL0;}case 27: _Tmp9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;void*t1=_Tmp8;int*iszeroterm=_Tmp7;
# 2583
t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,e1,t1,iszeroterm);goto _LL0;}case 28: _Tmp9=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct _tuple1**tn=_Tmp9;struct Cyc_List_List**ts=_Tmp8;struct Cyc_List_List*args=_Tmp7;struct Cyc_Absyn_Aggrdecl**sd_opt=_Tmp4;
# 2585
t=Cyc_Tcexp_tcAggregate(te,loc,topt,tn,ts,args,sd_opt);goto _LL0;}case 29: _Tmp9=(void**)&((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{void**ts=_Tmp9;int is_tuple=_Tmp3;struct Cyc_List_List*args=_Tmp8;
# 2587
t=Cyc_Tcexp_tcAnonStruct(te,loc,topt,e,ts,is_tuple,args);goto _LL0;}case 30: _Tmp9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp9;struct Cyc_Absyn_Datatypedecl*tud=_Tmp8;struct Cyc_Absyn_Datatypefield*tuf=_Tmp7;
t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,es,tud,tuf);goto _LL0;}case 31: _Tmp9=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp9;struct Cyc_Absyn_Enumfield*ef=_Tmp8;
t=Cyc_Absyn_enum_type(ed->name,ed);goto _LL0;}case 32: _Tmp9=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t2=_Tmp9;struct Cyc_Absyn_Enumfield*ef=_Tmp8;
t=t2;goto _LL0;}case 33: _Tmp9=(enum Cyc_Absyn_MallocKind*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_Tmp8=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp7=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp4=(void***)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp2=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp1=(int*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.fat_result;{enum Cyc_Absyn_MallocKind*mknd=_Tmp9;struct Cyc_Absyn_Exp**ropt=_Tmp8;struct Cyc_Absyn_Exp**aqopt=(struct Cyc_Absyn_Exp**)_Tmp7;void***t2=(void***)_Tmp4;struct Cyc_Absyn_Exp**e2=(struct Cyc_Absyn_Exp**)_Tmp2;int*isfat=(int*)_Tmp1;
# 2592
t=Cyc_Tcexp_tcMalloc(te,loc,topt,ropt,aqopt,t2,e2,mknd,isfat);goto _LL0;}case 34: _Tmp9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp8;
t=Cyc_Tcexp_tcSwap(te,loc,topt,e1,e2);goto _LL0;}case 37: _Tmp9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp9;struct _fat_ptr*f=_Tmp8;
t=Cyc_Tcexp_tcTagcheck(te,loc,topt,e,f);goto _LL0;}case 40: _Tmp9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
t=Cyc_Tcexp_tcExp(te,topt,e1);goto _LL0;}case 41: _Tmp9=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
t=Cyc_Tcexp_tcAssert(te,loc,e);goto _LL0;}case 38: _Tmp9=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t2=_Tmp9;
# 2598
if(!te->allow_valueof)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("valueof(-) can only occur within types",sizeof(char),39U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(e->loc,_tag_fat(_TmpB,sizeof(void*),1));});
# 2606
t=Cyc_Absyn_sint_type;
goto _LL0;}default:
# 2610
 t=Cyc_Absyn_void_type;goto _LL0;}_LL0:;}
# 2612
e->topt=t;}

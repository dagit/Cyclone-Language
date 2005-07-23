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
# 115 "core.h"
void*Cyc_Core_snd(struct _tuple0*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 104
extern int Cyc_fputc(int,struct Cyc___cycFILE*);
# 278 "cycboot.h"
extern int Cyc_file_string_write(struct Cyc___cycFILE*,struct _fat_ptr,int,int);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 165
enum Cyc_Absyn_AliasQualVal{Cyc_Absyn_Aliasable_qual =0U,Cyc_Absyn_Unique_qual =1U,Cyc_Absyn_Refcnt_qual =2U,Cyc_Absyn_Restricted_qual =3U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_AssnMap*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_AssnMap*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_AssnMap*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_AliasQualVal f1;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;struct Cyc_Absyn_Exp*f9;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 542
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 706 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 905
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 911
void*Cyc_Absyn_compress(void*);
# 915
int Cyc_Absyn_type2bool(int,void*);
# 924
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);struct _tuple12{enum Cyc_Absyn_AggrKind f0;struct _tuple1*f1;};
# 1185
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1193
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 61
struct _fat_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
# 79
extern struct _fat_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_char_escape(char);
struct _fat_ptr Cyc_Absynpp_string_escape(struct _fat_ptr);
struct _fat_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*);struct _tuple13{struct Cyc_Absyn_Tqual f0;void*f1;struct Cyc_List_List*f2;};
struct _tuple13 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual,void*);struct _tuple14{int f0;struct Cyc_List_List*f1;};
# 94 "absynpp.h"
struct _tuple14 Cyc_Absynpp_shadows(struct Cyc_Absyn_Decl*,struct Cyc_List_List*);
# 25 "attributes.h"
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Atts_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Atts_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Atts_Fastcall_att_val;
# 33
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Atts_Unused_att_val;struct _tuple15{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 89 "kinds.h"
void*Cyc_Kinds_compress_kb(void*);
# 83 "flags.h"
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;struct _tuple16{struct _fat_ptr f0;unsigned f1;};
# 28 "absyndump.cyc"
extern struct _tuple16 Cyc_Lex_xlate_pos(unsigned);
# 37
static int Cyc_Absyndump_expand_typedefs;
# 41
static int Cyc_Absyndump_qvar_to_Cids;
# 44
static int Cyc_Absyndump_add_cyc_prefix;
# 47
static int Cyc_Absyndump_generate_line_directives;
# 51
static int Cyc_Absyndump_to_VC;static char _TmpG0[14U]="__NoCycPrefix";
# 53
static struct _fat_ptr Cyc_Absyndump_nocyc_str={_TmpG0,_TmpG0,_TmpG0 + 14U};
static struct _fat_ptr*Cyc_Absyndump_nocyc_strptr=& Cyc_Absyndump_nocyc_str;
# 56
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 71 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 74
static void Cyc_Absyndump_dumptyp(void*);
static void Cyc_Absyndump_dumpntyp(void*);
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int,struct Cyc_List_List*);
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned,int);
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
static void Cyc_Absyndump_dumptms(int,struct Cyc_List_List*,void(*)(void*),void*);
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*)(void*),void*);
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*);
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*);
# 96
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
static char Cyc_Absyndump_prev_char='x';
# 99
static int Cyc_Absyndump_need_space_before (void){
switch((int)Cyc_Absyndump_prev_char){case 123:
 goto _LL4;case 125: _LL4:
 goto _LL6;case 40: _LL6:
 goto _LL8;case 41: _LL8:
 goto _LLA;case 91: _LLA:
 goto _LLC;case 93: _LLC:
 goto _LLE;case 59: _LLE:
 goto _LL10;case 44: _LL10:
 goto _LL12;case 61: _LL12:
 goto _LL14;case 63: _LL14:
 goto _LL16;case 33: _LL16:
 goto _LL18;case 32: _LL18:
 goto _LL1A;case 10: _LL1A:
 goto _LL1C;case 42: _LL1C:
 return 0;default:
 return 1;};}
# 119
static void Cyc_Absyndump_dump(struct _fat_ptr s){
unsigned sz=Cyc_strlen(s);
# 126
if(sz >= 1U){
if(Cyc_Absyndump_need_space_before())
Cyc_fputc(32,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=((const char*)s.curr)[(int)(sz - 1U)];
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 134
static void Cyc_Absyndump_dump_nospace(struct _fat_ptr s){
int sz=(int)Cyc_strlen(s);
# 137
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=((const char*)s.curr)[sz - 1];}}
# 142
static void Cyc_Absyndump_dump_char(int c){
# 144
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _TmpG1[1U]="";
# 148
static void Cyc_Absyndump_dumploc(unsigned loc){
static struct _fat_ptr last_file={_TmpG1,_TmpG1,_TmpG1 + 1U};
static unsigned last_line=0U;
if(loc==0U)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 154
struct _tuple16 _Tmp0=Cyc_Lex_xlate_pos(loc);unsigned _Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct _fat_ptr f=_Tmp2;unsigned d=_Tmp1;
if((char*)f.curr==(char*)_tag_fat(0,0,0).curr ||(char*)f.curr==(char*)last_file.curr && d==last_line)return;
if((char*)f.curr==(char*)last_file.curr && d==last_line + 1U)
Cyc_fprintf(*Cyc_Absyndump_dump_file,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));else{
if((char*)f.curr==(char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)((int)d);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(*Cyc_Absyndump_dump_file,_tag_fat("\n# %d\n",sizeof(char),7U),_tag_fat(_Tmp4,sizeof(void*),1));});else{
# 161
({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)((int)d);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)f;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(*Cyc_Absyndump_dump_file,_tag_fat("\n# %d %s\n",sizeof(char),10U),_tag_fat(_Tmp5,sizeof(void*),2));});}}
# 163
last_file=f;
last_line=d;}}}
# 169
static void Cyc_Absyndump_dump_str(struct _fat_ptr*s){
Cyc_Absyndump_dump(*s);}
# 172
static void Cyc_Absyndump_dump_str_nospace(struct _fat_ptr*s){
Cyc_Absyndump_dump_nospace(*s);}
# 176
static void Cyc_Absyndump_dump_semi (void){
Cyc_Absyndump_dump_char(59);}
# 180
static void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr sep){
if(l==0)
return;
for(1;_check_null(l)->tl!=0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 187
f(l->hd);}
# 189
static void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr sep){
if(l==0)
return;
for(1;_check_null(l)->tl!=0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 196
f(env,l->hd);}
# 198
static void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 200
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 204
static void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 206
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 210
static void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _fat_ptr start,struct _fat_ptr end,struct _fat_ptr sep){
# 212
if(l!=0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 216
static void Cyc_Absyndump_ignore(void*x){;}
# 218
static void Cyc_Absyndump_dumpqvar(struct _tuple1*v){
struct Cyc_List_List*nsl=0;
struct _fat_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _Tmp0=(*v).f0;void*_Tmp1;switch(_Tmp0.C_n.tag){case 4:
 goto _LL0;case 1: _Tmp1=_Tmp0.Rel_n.val;{struct Cyc_List_List*x=_Tmp1;
nsl=x;goto _LL0;}case 3: _Tmp1=_Tmp0.C_n.val;{struct Cyc_List_List*x=_Tmp1;
goto _LL0;}default: _Tmp1=_Tmp0.Abs_n.val;{struct Cyc_List_List*x=_Tmp1;
# 226
if(Cyc_Absyndump_qvar_to_Cids || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
nsl=x;
goto _LL0;}}_LL0:;}
# 231
if(Cyc_Absyndump_need_space_before())
Cyc_Absyndump_dump_char(32);{
struct _fat_ptr sep=Cyc_Absyndump_qvar_to_Cids?_tag_fat("_",sizeof(char),2U): _tag_fat("::",sizeof(char),3U);
if((unsigned)nsl && !Cyc_strptrcmp((struct _fat_ptr*)nsl->hd,Cyc_Absyndump_nocyc_strptr)){
nsl=nsl->tl;
prefix=0;}
# 238
if(prefix!=0){
Cyc_Absyndump_dump_nospace(*(*prefix));Cyc_Absyndump_dump_nospace(sep);}
# 241
if(nsl!=0){
({void(*_Tmp0)(void(*)(struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr)=(void(*)(void(*)(struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;_Tmp0;})(Cyc_Absyndump_dump_str_nospace,nsl,sep);Cyc_Absyndump_dump_nospace(sep);}
# 244
Cyc_Absyndump_dump_nospace(*(*v).f1);}}
# 247
static void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(_tag_fat("restrict",sizeof(char),9U));
if(tq.q_volatile)Cyc_Absyndump_dump(_tag_fat("volatile",sizeof(char),9U));
if(tq.print_const)Cyc_Absyndump_dump(_tag_fat("const",sizeof(char),6U));}
# 252
static void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
switch((int)sc){case Cyc_Absyn_Static:
 Cyc_Absyndump_dump(_tag_fat("static",sizeof(char),7U));return;case Cyc_Absyn_Public:
 return;case Cyc_Absyn_Extern:
 Cyc_Absyndump_dump(_tag_fat("extern",sizeof(char),7U));return;case Cyc_Absyn_ExternC:
 Cyc_Absyndump_dump(_tag_fat("extern \"C\"",sizeof(char),11U));return;case Cyc_Absyn_Abstract:
 Cyc_Absyndump_dump(_tag_fat("abstract",sizeof(char),9U));return;case Cyc_Absyn_Register:
 Cyc_Absyndump_dump(_tag_fat("register",sizeof(char),9U));return;default:
 return;};}
# 263
static void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
if(k==Cyc_Absyn_StructA){
Cyc_Absyndump_dump(_tag_fat("struct",sizeof(char),7U));return;}else{
Cyc_Absyndump_dump(_tag_fat("union",sizeof(char),6U));return;};}
# 270
static void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
Cyc_Absyndump_dump(Cyc_Absynpp_kind2string(ka));}
# 274
static void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
Cyc_Absyndump_egroup(Cyc_Absyndump_dumptyp,ts,_tag_fat("<",sizeof(char),2U),_tag_fat(">",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));}
# 277
static void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _fat_ptr n=*tv->name;
if((int)*((const char*)_check_fat_subscript(n,sizeof(char),0))==35){
Cyc_Absyndump_dump(_tag_fat("`G",sizeof(char),3U));
{void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
_Tmp1=k;goto _LL4;}case 2: _Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_Tmp1;
Cyc_Absyndump_dumpkind(k);goto _LL0;}default:
 Cyc_Absyndump_dump_nospace(_tag_fat("K",sizeof(char),2U));goto _LL0;}_LL0:;}
# 286
Cyc_Absyndump_dump_nospace(_fat_ptr_plus(n,sizeof(char),1));}else{
# 288
Cyc_Absyndump_dump(n);}}
# 290
static void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_Tmp0=Cyc_Kinds_compress_kb(tv->kind);void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_BoxKind){_LL6:
 goto _LL0;}else{_Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Kind*k=_Tmp1;
Cyc_Absyndump_dump(_tag_fat("::",sizeof(char),3U));Cyc_Absyndump_dumpkind(k);goto _LL0;}}}_LL0:;}}
# 299
static void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({void(*_Tmp0)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;_Tmp0;})(Cyc_Absyndump_dumptvar,tvs,_tag_fat("<",sizeof(char),2U),_tag_fat(">",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));}
# 302
static void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({void(*_Tmp0)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;_Tmp0;})(Cyc_Absyndump_dumpkindedtvar,tvs,_tag_fat("<",sizeof(char),2U),_tag_fat(">",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));}struct _tuple17{struct Cyc_Absyn_Tqual f0;void*f1;};
# 305
static void Cyc_Absyndump_dumparg(struct _tuple17*pr){
({void(*_Tmp0)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int))Cyc_Absyndump_dumptqtd;_Tmp0;})((*pr).f0,(*pr).f1,({void(*_Tmp0)(int)=(void(*)(int))Cyc_Absyndump_ignore;_Tmp0;}),0);}
# 308
static void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({void(*_Tmp0)(void(*)(struct _tuple17*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple17*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_Tmp0;})(Cyc_Absyndump_dumparg,ts,_tag_fat("(",sizeof(char),2U),_tag_fat(")",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));}
# 312
static void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 1:
 Cyc_Absyndump_dump(_tag_fat("_stdcall",sizeof(char),9U));return;case 2:
 Cyc_Absyndump_dump(_tag_fat("_cdecl",sizeof(char),7U));return;case 3:
 Cyc_Absyndump_dump(_tag_fat("_fastcall",sizeof(char),10U));return;default:
 goto _LL0;}_LL0:;}}
# 322
static void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 324
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2!=0;atts2=atts2->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 1:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 3: _LL6:
 goto _LL0;default:
 hasatt=1;goto _LL0;}_LL0:;}}
# 332
if(!hasatt)
return;
Cyc_Absyndump_dump(_tag_fat("__declspec(",sizeof(char),12U));
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 1:
 goto _LLD;case 2: _LLD:
 goto _LLF;case 3: _LLF:
 goto _LL9;default:
 Cyc_Absyndump_dump(Cyc_Absynpp_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 342
Cyc_Absyndump_dump_char(41);}
# 345
static void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts==0)return;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
# 349
Cyc_Absyndump_dump(_tag_fat(" __attribute__((",sizeof(char),17U));
for(1;atts!=0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absynpp_attribute2string((void*)atts->hd));
if(atts->tl!=0)Cyc_Absyndump_dump(_tag_fat(",",sizeof(char),2U));}
# 354
Cyc_Absyndump_dump(_tag_fat(")) ",sizeof(char),4U));
return;}else{
# 357
Cyc_Absyndump_dump_noncallconv(atts);
return;};}
# 362
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple18{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 366
static struct _tuple18 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
{void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==9){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp1;
# 372
for(1;ts!=0;ts=ts->tl){
struct _tuple18 _Tmp2=Cyc_Absyndump_effects_split((void*)ts->hd);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_List_List*rs=_Tmp4;struct Cyc_List_List*es=_Tmp3;
rgions=Cyc_List_imp_append(rs,rgions);
effects=Cyc_List_imp_append(es,effects);}}
# 377
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 effects=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=t,_Tmp2->tl=effects;_Tmp2;});goto _LL0;}_LL0:;}{
# 380
struct _tuple18 _Tmp0;_Tmp0.f0=rgions,_Tmp0.f1=effects;return _Tmp0;}}
# 383
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple18 _Tmp0=Cyc_Absyndump_effects_split(t);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*rgions=_Tmp2;struct Cyc_List_List*effects=_Tmp1;
rgions=Cyc_List_imp_rev(rgions);
effects=Cyc_List_imp_rev(effects);
if(effects!=0){
Cyc_Absyndump_group(Cyc_Absyndump_dumpntyp,effects,_tag_fat("",sizeof(char),1U),_tag_fat("",sizeof(char),1U),_tag_fat("+",sizeof(char),2U));
if(rgions!=0)Cyc_Absyndump_dump_char(43);}
# 391
if(rgions!=0 || effects==0)
Cyc_Absyndump_group(Cyc_Absyndump_dumprgn,rgions,_tag_fat("{",sizeof(char),2U),_tag_fat("}",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));}}
# 395
static void Cyc_Absyndump_dump_aqual_val(enum Cyc_Absyn_AliasQualVal aqv){
switch((int)aqv){case Cyc_Absyn_Aliasable_qual:
 Cyc_Absyndump_dump(_tag_fat("ALIASABLE",sizeof(char),10U));goto _LL0;case Cyc_Absyn_Unique_qual:
 Cyc_Absyndump_dump(_tag_fat("UNIQUE",sizeof(char),7U));goto _LL0;case Cyc_Absyn_Refcnt_qual:
 Cyc_Absyndump_dump(_tag_fat("REFCNT",sizeof(char),7U));goto _LL0;case Cyc_Absyn_Restricted_qual:
 Cyc_Absyndump_dump(_tag_fat("RESTRICTED",sizeof(char),11U));goto _LL0;default:
 goto _LL0;}_LL0:;}
# 405
static struct _tuple17*Cyc_Absyndump_aggrfield2arg(struct Cyc_Absyn_Aggrfield*f){
struct _tuple17*_Tmp0=_cycalloc(sizeof(struct _tuple17));_Tmp0->f0=f->tq,_Tmp0->f1=f->type;return _Tmp0;}
# 410
static void Cyc_Absyndump_dumpntyp(void*t){
enum Cyc_Absyn_AggrKind _Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)t)){case 4:
# 413
 goto _LL4;case 5: _LL4:
 goto _LL6;case 3: _LL6:
 return;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==0){
Cyc_Absyndump_dump(_tag_fat("void",sizeof(char),5U));return;}else{_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1;_Tmp2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{void*c=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 438
enum Cyc_Absyn_AliasQualVal _Tmp4;struct _fat_ptr _Tmp5;void*_Tmp6;union Cyc_Absyn_AggrInfo _Tmp7;int _Tmp8;union Cyc_Absyn_DatatypeFieldInfo _Tmp9;union Cyc_Absyn_DatatypeInfo _TmpA;switch(*((int*)c)){case 22: _TmpA=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_DatatypeInfo tu_info=_TmpA;
# 440
{int _TmpB;void*_TmpC;if(tu_info.UnknownDatatype.tag==1){_TmpC=tu_info.UnknownDatatype.val.name;_TmpB=tu_info.UnknownDatatype.val.is_extensible;{struct _tuple1*n=_TmpC;int is_x=_TmpB;
_TmpC=n;_TmpB=is_x;goto _LL6E;}}else{_TmpC=(*tu_info.KnownDatatype.val)->name;_TmpB=(*tu_info.KnownDatatype.val)->is_extensible;_LL6E: {struct _tuple1*n=_TmpC;int is_x=_TmpB;
# 443
if(is_x)Cyc_Absyndump_dump(_tag_fat("@extensible",sizeof(char),12U));
Cyc_Absyndump_dump(_tag_fat("datatype",sizeof(char),9U));Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
goto _LL6A;}}_LL6A:;}
# 447
return;}case 23: _Tmp9=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_DatatypeFieldInfo tuf_info=_Tmp9;
# 449
{int _TmpB;void*_TmpC;void*_TmpD;if(tuf_info.UnknownDatatypefield.tag==1){_TmpD=tuf_info.UnknownDatatypefield.val.datatype_name;_TmpC=tuf_info.UnknownDatatypefield.val.field_name;_TmpB=tuf_info.UnknownDatatypefield.val.is_extensible;{struct _tuple1*tname=_TmpD;struct _tuple1*fname=_TmpC;int is_x=_TmpB;
# 451
_TmpD=tname;_TmpB=is_x;_TmpC=fname;goto _LL73;}}else{_TmpD=tuf_info.KnownDatatypefield.val.f0->name;_TmpB=tuf_info.KnownDatatypefield.val.f0->is_extensible;_TmpC=tuf_info.KnownDatatypefield.val.f1->name;_LL73: {struct _tuple1*tname=_TmpD;int is_x=_TmpB;struct _tuple1*fname=_TmpC;
# 454
if(is_x)Cyc_Absyndump_dump(_tag_fat("@extensible ",sizeof(char),13U));
Cyc_Absyndump_dump(_tag_fat("datatype ",sizeof(char),10U));Cyc_Absyndump_dumpqvar(tname);Cyc_Absyndump_dump(_tag_fat(".",sizeof(char),2U));Cyc_Absyndump_dumpqvar(fname);
Cyc_Absyndump_dumptps(ts);
goto _LL6F;}}_LL6F:;}
# 459
return;}case 3:
 Cyc_Absyndump_dump(_tag_fat("_Complex ",sizeof(char),10U));Cyc_Absyndump_dumpntyp((void*)_check_null(ts)->hd);return;case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)c)->f1){case Cyc_Absyn_None: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)c)->f2){case Cyc_Absyn_Int_sz:
 goto _LL29;case Cyc_Absyn_Long_sz:
# 463
 goto _LL2D;case Cyc_Absyn_Char_sz:
# 465
 Cyc_Absyndump_dump(_tag_fat("char",sizeof(char),5U));return;case Cyc_Absyn_Short_sz:
# 468
 goto _LL37;case Cyc_Absyn_LongLong_sz: _LL41:
# 474
 goto _LL43;default: goto _LL68;}case Cyc_Absyn_Signed: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)c)->f2){case Cyc_Absyn_Int_sz: _LL29:
# 462
 Cyc_Absyndump_dump(_tag_fat("int",sizeof(char),4U));return;case Cyc_Absyn_Long_sz: _LL2D:
# 464
 Cyc_Absyndump_dump(_tag_fat("long",sizeof(char),5U));return;case Cyc_Absyn_Char_sz:
# 466
 Cyc_Absyndump_dump(_tag_fat("signed char",sizeof(char),12U));return;case Cyc_Absyn_Short_sz: _LL37:
# 469
 Cyc_Absyndump_dump(_tag_fat("short",sizeof(char),6U));return;case Cyc_Absyn_LongLong_sz: _LL43:
# 476
 if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
Cyc_Absyndump_dump(_tag_fat("long long",sizeof(char),10U));return;}else{
Cyc_Absyndump_dump(_tag_fat("__int64",sizeof(char),8U));return;};default: goto _LL68;}case Cyc_Absyn_Unsigned: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)c)->f2){case Cyc_Absyn_Char_sz:
# 467
 Cyc_Absyndump_dump(_tag_fat("unsigned char",sizeof(char),14U));return;case Cyc_Absyn_Short_sz:
# 470
 Cyc_Absyndump_dump(_tag_fat("unsigned short",sizeof(char),15U));return;case Cyc_Absyn_Int_sz:
 Cyc_Absyndump_dump(_tag_fat("unsigned",sizeof(char),9U));return;case Cyc_Absyn_Long_sz:
 Cyc_Absyndump_dump(_tag_fat("unsigned long",sizeof(char),14U));return;case Cyc_Absyn_LongLong_sz:
 Cyc_Absyndump_dump(_tag_fat("unsigned",sizeof(char),9U));goto _LL41;default: goto _LL68;}default: goto _LL68;}case 2: _Tmp8=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)c)->f1;{int i=_Tmp8;
# 481
switch((int)i){case 0:
 Cyc_Absyndump_dump(_tag_fat("float",sizeof(char),6U));return;case 1:
 Cyc_Absyndump_dump(_tag_fat("double",sizeof(char),7U));return;default:
 Cyc_Absyndump_dump(_tag_fat("long double",sizeof(char),12U));return;};}case 24: _Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)c)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp7;
# 487
struct _tuple12 _TmpB=Cyc_Absyn_aggr_kinded_name(info);void*_TmpC;enum Cyc_Absyn_AggrKind _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{enum Cyc_Absyn_AggrKind k=_TmpD;struct _tuple1*n=_TmpC;
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dumpqvar(n);Cyc_Absyndump_dumptps(ts);
return;}}case 19: _Tmp6=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)c)->f1;{struct _tuple1*n=_Tmp6;
Cyc_Absyndump_dump(_tag_fat("enum",sizeof(char),5U));Cyc_Absyndump_dumpqvar(n);return;}case 20: _Tmp6=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)c)->f1;{struct Cyc_List_List*fs=_Tmp6;
Cyc_Absyndump_dump(_tag_fat("enum{",sizeof(char),6U));Cyc_Absyndump_dumpenumfields(fs);Cyc_Absyndump_dump(_tag_fat("}",sizeof(char),2U));return;}case 21: _Tmp5=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)c)->f1;{struct _fat_ptr t=_Tmp5;
Cyc_Absyndump_dump(t);return;}case 4:
 Cyc_Absyndump_dump(_tag_fat("region_t<",sizeof(char),10U));Cyc_Absyndump_dumprgn((void*)_check_null(ts)->hd);Cyc_Absyndump_dump(_tag_fat(">",sizeof(char),2U));return;case 5:
 Cyc_Absyndump_dump(_tag_fat("tag_t<",sizeof(char),7U));Cyc_Absyndump_dumptyp((void*)_check_null(ts)->hd);Cyc_Absyndump_dump(_tag_fat(">",sizeof(char),2U));return;case 7:
 Cyc_Absyndump_dump(_tag_fat("`U",sizeof(char),3U));return;case 8:
 Cyc_Absyndump_dump(_tag_fat("`RC",sizeof(char),4U));return;case 6:
 Cyc_Absyndump_dump(_tag_fat("`H",sizeof(char),3U));return;case 10:
# 499
 Cyc_Absyndump_dump(_tag_fat("regions(",sizeof(char),9U));Cyc_Absyndump_dumptyp((void*)_check_null(ts)->hd);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));return;case 9:
 Cyc_Absyndump_group(Cyc_Absyndump_dumptyp,ts,_tag_fat("",sizeof(char),1U),_tag_fat("",sizeof(char),1U),_tag_fat("+",sizeof(char),2U));return;case 11:
 Cyc_Absyndump_dump(_tag_fat("@true",sizeof(char),6U));return;case 12:
 Cyc_Absyndump_dump(_tag_fat("@false",sizeof(char),7U));return;case 13:
 Cyc_Absyndump_dump(_tag_fat("@thin",sizeof(char),6U));return;case 14:
 Cyc_Absyndump_dump(_tag_fat("@fat",sizeof(char),5U));return;case 16: _Tmp4=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)c)->f1;{enum Cyc_Absyn_AliasQualVal v=_Tmp4;
Cyc_Absyndump_dump_aqual_val(v);return;}case 15:
 Cyc_Absyndump_dump(_tag_fat("aquals(",sizeof(char),8U));Cyc_Absyndump_dumptyp((void*)_check_null(ts)->hd);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));return;default: _LL68:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("bad type constructor",sizeof(char),21U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({int(*_TmpD)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpD;})(_tag_fat(_TmpC,sizeof(void*),1));});};}}case 2: _Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp3;
# 417
Cyc_Absyndump_dumptvar(tv);return;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2==0){_Tmp3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f1;_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f3;{struct Cyc_Core_Opt*k=_Tmp3;int i=_Tmp1;
# 419
Cyc_Absyndump_dump(_tag_fat("`E",sizeof(char),3U));
if(k==0)Cyc_Absyndump_dump(_tag_fat("K",sizeof(char),2U));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)k->v);}
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));}));return;}}else{_Tmp3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;_Tmp1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f3;{struct Cyc_Core_Opt*k=_Tmp3;void*t=_Tmp2;int i=_Tmp1;
Cyc_Absyndump_dumpntyp(t);return;}}case 6: _Tmp0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f1;_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f2;_Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;{enum Cyc_Absyn_AggrKind k=_Tmp0;int is_tuple=_Tmp1;struct Cyc_List_List*fs=_Tmp3;
# 424
if(is_tuple){
Cyc_Absyndump_dump_char(36);
Cyc_Absyndump_dumpargs(({struct Cyc_List_List*(*_Tmp4)(struct _tuple17*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple17*(*)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*))Cyc_List_map;_Tmp4;})(Cyc_Absyndump_aggrfield2arg,fs));}else{
# 428
Cyc_Absyndump_dumpaggr_kind(k);Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpaggrfields(fs);Cyc_Absyndump_dump_char(125);}
# 430
return;}case 9: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1)->r)){case 0: _Tmp3=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*d=_Tmp3;
Cyc_Absyndump_dump_aggrdecl(d);return;}case 1: _Tmp3=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*d=_Tmp3;
Cyc_Absyndump_dump_enumdecl(d);return;}default: _Tmp3=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)t)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*d=_Tmp3;
Cyc_Absyndump_dump_datatypedecl(d);return;}}case 7: _Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;_Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;{struct _tuple1*n=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
Cyc_Absyndump_dumpqvar(n),Cyc_Absyndump_dumptps(ts);return;}case 8: _Tmp3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
Cyc_Absyndump_dump(_tag_fat("valueof_t(",sizeof(char),11U));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));return;}default: _Tmp3=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
Cyc_Absyndump_dump(_tag_fat("typeof(",sizeof(char),8U));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));return;}};}
# 512
static void Cyc_Absyndump_dumpvaropt(struct _fat_ptr*vo){
if(vo!=0)Cyc_Absyndump_dump_str(vo);}
# 515
static void Cyc_Absyndump_dumpfunarg(struct _tuple9*t){
({void(*_Tmp0)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd;_Tmp0;})((*t).f1,(*t).f2,Cyc_Absyndump_dumpvaropt,(*t).f0);}
# 518
static void Cyc_Absyndump_dump_rgncmp(struct _tuple0*cmp){
Cyc_Absyndump_dumpeff((*cmp).f0);Cyc_Absyndump_dump_char(62);Cyc_Absyndump_dumprgn((*cmp).f1);}
# 521
static void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({void(*_Tmp0)(void(*)(struct _tuple0*),struct Cyc_List_List*,struct _fat_ptr)=(void(*)(void(*)(struct _tuple0*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;_Tmp0;})(Cyc_Absyndump_dump_rgncmp,rgn_po,_tag_fat(",",sizeof(char),2U));}
# 524
static void Cyc_Absyndump_dump_oneeffc(void*fc){
void*_Tmp0;void*_Tmp1;switch(*((int*)fc)){case 2: _Tmp1=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)fc)->f1;_Tmp0=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)fc)->f2;{void*e1=_Tmp1;void*e2=_Tmp0;
# 527
Cyc_Absyndump_dumpeff(e1);Cyc_Absyndump_dump(_tag_fat("<=",sizeof(char),3U));Cyc_Absyndump_dumpeff(e2);
goto _LL0;}case 1: _Tmp1=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)fc)->f1;_Tmp0=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)fc)->f2;{void*e1=_Tmp1;void*e2=_Tmp0;
# 530
Cyc_Absyndump_dumpeff(e1);Cyc_Absyndump_dump_char(124);Cyc_Absyndump_dumpeff(e2);
goto _LL0;}default: _Tmp1=(void*)((struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)fc)->f1;{void*e=_Tmp1;
# 533
Cyc_Absyndump_dump(_tag_fat("single(",sizeof(char),8U));Cyc_Absyndump_dumpeff(e);Cyc_Absyndump_dump_char(41);
goto _LL0;}}_LL0:;}
# 537
static void Cyc_Absyndump_dump_effc(struct Cyc_List_List*effc){
Cyc_Absyndump_dump_sep(Cyc_Absyndump_dump_oneeffc,effc,_tag_fat(",",sizeof(char),2U));}
# 540
static void Cyc_Absyndump_dump_qb_elt(struct _tuple0*q){
Cyc_Absyndump_dumptyp((*q).f1);Cyc_Absyndump_dump(_tag_fat(">=",sizeof(char),3U));Cyc_Absyndump_dumptyp((*q).f0);}
# 543
static void Cyc_Absyndump_dump_qualbnd(struct Cyc_List_List*qual_bnd){
({void(*_Tmp0)(void(*)(struct _tuple0*),struct Cyc_List_List*,struct _fat_ptr)=(void(*)(void(*)(struct _tuple0*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;_Tmp0;})(Cyc_Absyndump_dump_qb_elt,qual_bnd,_tag_fat(",",sizeof(char),2U));}
# 546
static void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*effc,struct Cyc_List_List*qual_bnd,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens,struct Cyc_Absyn_Exp*thrws){
# 551
if((((args==0 && !c_varargs)&& cyc_varargs==0)&& effopt==0)&& effc==0)
# 553
Cyc_Absyndump_dump(_tag_fat("(void)",sizeof(char),7U));else{
# 556
({void(*_Tmp0)(void(*)(struct _tuple9*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple9*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_Tmp0;})(Cyc_Absyndump_dumpfunarg,args,_tag_fat("(",sizeof(char),2U),_tag_fat("",sizeof(char),1U),_tag_fat(",",sizeof(char),2U));
if(c_varargs || cyc_varargs!=0)
Cyc_Absyndump_dump(_tag_fat(",...",sizeof(char),5U));
if(cyc_varargs!=0){
if(cyc_varargs->inject)Cyc_Absyndump_dump(_tag_fat(" inject ",sizeof(char),9U));
Cyc_Absyndump_dumpfunarg(({struct _tuple9*_Tmp0=_cycalloc(sizeof(struct _tuple9));_Tmp0->f0=cyc_varargs->name,_Tmp0->f1=cyc_varargs->tq,_Tmp0->f2=cyc_varargs->type;_Tmp0;}));}
# 563
if(effopt!=0){
Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpeff(effopt);}
# 566
if(effc!=0){
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dump_effc(effc);}
# 569
if(qual_bnd!=0){
char sep=effc==0?':':',';
Cyc_Absyndump_dump_char((int)sep);Cyc_Absyndump_dump_qualbnd(qual_bnd);}
# 573
Cyc_Absyndump_dump_char(41);}
# 575
if(req!=0){
Cyc_Absyndump_dump(_tag_fat(" @requires(",sizeof(char),12U));Cyc_Absyndump_dumpexp(req);Cyc_Absyndump_dump_char(41);}
# 578
if(ens!=0){
Cyc_Absyndump_dump(_tag_fat(" @ensures(",sizeof(char),11U));Cyc_Absyndump_dumpexp(ens);Cyc_Absyndump_dump_char(41);}
# 581
if(thrws!=0){
Cyc_Absyndump_dump(_tag_fat(" @throws(",sizeof(char),10U));Cyc_Absyndump_dumpexp(thrws);Cyc_Absyndump_dump_char(41);}}
# 586
static void Cyc_Absyndump_dumptyp(void*t){
({void(*_Tmp0)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int)=({void(*_Tmp1)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(int),int))Cyc_Absyndump_dumptqtd;_Tmp1;});struct Cyc_Absyn_Tqual _Tmp1=Cyc_Absyn_empty_tqual(0U);void*_Tmp2=t;_Tmp0(_Tmp1,_Tmp2,({void(*_Tmp3)(int)=(void(*)(int))Cyc_Absyndump_ignore;_Tmp3;}),0);});}
# 590
static void Cyc_Absyndump_dumpdesignator(void*d){
void*_Tmp0;if(*((int*)d)==0){_Tmp0=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)d)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
Cyc_Absyndump_dump(_tag_fat(".[",sizeof(char),3U));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(93);goto _LL0;}}else{_Tmp0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)d)->f1;{struct _fat_ptr*v=_Tmp0;
Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*v);goto _LL0;}}_LL0:;}struct _tuple19{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 596
static void Cyc_Absyndump_dumpde(struct _tuple19*de){
Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*de).f0,_tag_fat("",sizeof(char),1U),_tag_fat("=",sizeof(char),2U),_tag_fat("=",sizeof(char),2U));
Cyc_Absyndump_dumpexp((*de).f1);}
# 601
static void Cyc_Absyndump_dumpoffset_field(void*f){
void*_Tmp0;_Tmp0=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)f)->f1;{struct _fat_ptr*n=_Tmp0;
Cyc_Absyndump_dump_nospace(*n);return;};}struct _tuple20{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};
# 607
static void Cyc_Absyndump_dump_asm_iolist(struct Cyc_List_List*iolist){
while((unsigned)iolist){
{struct _tuple20*_Tmp0=(struct _tuple20*)iolist->hd;void*_Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{struct _fat_ptr constr=_Tmp2;struct Cyc_Absyn_Exp*e1=_Tmp1;
Cyc_Absyndump_dump_char(34);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(constr));Cyc_Absyndump_dump_char(34);
Cyc_Absyndump_dump_char(40);Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);
iolist=iolist->tl;
if((unsigned)iolist)
Cyc_Absyndump_dump_char(44);}}
# 609
1U;}}
# 618
static void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({void(*_Tmp0)(void(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(int,struct Cyc_Absyn_Exp*),int,struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group_c;_Tmp0;})(Cyc_Absyndump_dumpexp_prec,inprec,es,_tag_fat("",sizeof(char),1U),_tag_fat("",sizeof(char),1U),_tag_fat(",",sizeof(char),2U));}
# 622
static void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec &&(inprec!=140 || myprec!=140))
# 626
Cyc_Absyndump_dump_nospace(_tag_fat("(",sizeof(char),2U));
{void*_Tmp0=e->r;void*_Tmp1;enum Cyc_Absyn_MallocKind _Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_Primop _Tmp5;void*_Tmp6;long long _Tmp7;int _Tmp8;short _Tmp9;struct _fat_ptr _TmpA;char _TmpB;enum Cyc_Absyn_Sign _TmpC;switch(*((int*)_Tmp0)){case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.tag){case 2: _TmpC=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Char_c.val.f0;_TmpB=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Char_c.val.f1;{enum Cyc_Absyn_Sign sg=_TmpC;char ch=_TmpB;
# 629
Cyc_Absyndump_dump_char(39);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_char(39);
goto _LL0;}case 3: _TmpA=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wchar_c.val;{struct _fat_ptr s=_TmpA;
Cyc_Absyndump_dump(({struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=(struct _fat_ptr)s;_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;Cyc_aprintf(_tag_fat("L'%s'",sizeof(char),6U),_tag_fat(_TmpE,sizeof(void*),1));}));goto _LL0;}case 4: _TmpC=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Short_c.val.f0;_Tmp9=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Short_c.val.f1;{enum Cyc_Absyn_Sign sg=_TmpC;short s=_Tmp9;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _TmpD=({struct Cyc_Int_pa_PrintArg_struct _TmpE;_TmpE.tag=1,_TmpE.f1=(unsigned long)((int)s);_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_TmpE,sizeof(void*),1));}));goto _LL0;}case 5: switch((int)((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f0){case Cyc_Absyn_None: _Tmp8=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f1;{int i=_Tmp8;
_Tmp8=i;goto _LLA;}case Cyc_Absyn_Signed: _Tmp8=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f1;_LLA: {int i=_Tmp8;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _TmpD=({struct Cyc_Int_pa_PrintArg_struct _TmpE;_TmpE.tag=1,_TmpE.f1=(unsigned long)i;_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_TmpE,sizeof(void*),1));}));goto _LL0;}default: _Tmp8=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f1;{int i=_Tmp8;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _TmpD=({struct Cyc_Int_pa_PrintArg_struct _TmpE;_TmpE.tag=1,_TmpE.f1=(unsigned)i;_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;Cyc_aprintf(_tag_fat("%uU",sizeof(char),4U),_tag_fat(_TmpE,sizeof(void*),1));}));goto _LL0;}}case 6: _TmpC=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.LongLong_c.val.f0;_Tmp7=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.LongLong_c.val.f1;{enum Cyc_Absyn_Sign sg=_TmpC;long long i=_Tmp7;
# 638
Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)i));goto _LL0;}case 7: _TmpA=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Float_c.val.f0;{struct _fat_ptr x=_TmpA;
Cyc_Absyndump_dump(x);goto _LL0;}case 1:
 Cyc_Absyndump_dump(_tag_fat("NULL",sizeof(char),5U));goto _LL0;case 8: _TmpA=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.val;{struct _fat_ptr s=_TmpA;
# 642
Cyc_Absyndump_dump_char(34);Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(s));Cyc_Absyndump_dump_char(34);
goto _LL0;}default: _TmpA=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wstring_c.val;{struct _fat_ptr s=_TmpA;
# 645
Cyc_Absyndump_dump(_tag_fat("L\"",sizeof(char),3U));Cyc_Absyndump_dump_nospace(s);Cyc_Absyndump_dump_char(34);goto _LL0;}}case 1: _Tmp6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp6;
# 647
Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(b));goto _LL0;}case 2: _TmpA=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr p=_TmpA;
# 650
Cyc_Absyndump_dump(_tag_fat("__cyclone_pragma__(",sizeof(char),20U));Cyc_Absyndump_dump_nospace(p);Cyc_Absyndump_dump_char(41);goto _LL0;}case 3: _Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp5;struct Cyc_List_List*es=_Tmp6;
# 653
struct _fat_ptr pstr=Cyc_Absynpp_prim2str(p);
{int _TmpD=Cyc_List_length(es);switch((int)_TmpD){case 1:
# 656
 if((int)p==(int)18U){
Cyc_Absyndump_dump(_tag_fat("numelts(",sizeof(char),9U));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(es)->hd);
Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));}else{
if((int)p==(int)19U){
Cyc_Absyndump_dump(_tag_fat("tagof(",sizeof(char),7U));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(es)->hd);
Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));}else{
# 665
Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)_check_null(es)->hd);}}
# 668
goto _LL71;case 2:
# 670
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)_check_null(es)->hd);
switch((int)p){case Cyc_Absyn_Eq:
 goto _LL7C;case Cyc_Absyn_Neq: _LL7C:
 Cyc_Absyndump_dump_nospace(pstr);goto _LL78;default:
# 675
 Cyc_Absyndump_dump(pstr);
Cyc_Absyndump_dump_char(32);}_LL78:;
# 678
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)_check_null(es->tl)->hd);
goto _LL71;default:
({int(*_TmpE)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpE;})(_tag_fat("Absyndump -- Bad number of arguments to primop",sizeof(char),47U),_tag_fat(0U,sizeof(void*),0));}_LL71:;}
# 682
goto _LL0;}case 4: _Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Core_Opt*popt=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 685
Cyc_Absyndump_dumpexp_prec(myprec,e1);
if(popt!=0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)popt->v));
Cyc_Absyndump_dump_nospace(_tag_fat("=",sizeof(char),2U));
Cyc_Absyndump_dumpexp_prec(myprec,e2);
goto _LL0;}case 5: switch((int)((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2){case Cyc_Absyn_PreInc: _Tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp6;
# 692
Cyc_Absyndump_dump(_tag_fat("++",sizeof(char),3U));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PreDec: _Tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp6;
Cyc_Absyndump_dump(_tag_fat("--",sizeof(char),3U));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case Cyc_Absyn_PostInc: _Tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp6;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(_tag_fat("++",sizeof(char),3U));goto _LL0;}default: _Tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e2=_Tmp6;
Cyc_Absyndump_dumpexp_prec(myprec,e2);Cyc_Absyndump_dump(_tag_fat("--",sizeof(char),3U));goto _LL0;}}case 6: _Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;
# 698
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_char(63);Cyc_Absyndump_dumpexp_prec(0,e2);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp_prec(myprec,e3);
goto _LL0;}case 7: _Tmp6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 705
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(_tag_fat("&&",sizeof(char),3U));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 8: _Tmp6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 709
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump(_tag_fat("||",sizeof(char),3U));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 9: _Tmp6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 715
Cyc_Absyndump_dumploc(e->loc);
# 718
Cyc_Absyndump_dumpexp_prec(myprec - 1,e1);Cyc_Absyndump_dump_char(44);Cyc_Absyndump_dumpexp_prec(myprec - 1,e2);
goto _LL0;}case 10: _Tmp6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_List_List*es=_Tmp4;
# 722
Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,e1);
Cyc_Absyndump_dump_nospace(_tag_fat("(",sizeof(char),2U));{
# 727
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives && !(e->loc==0U))&& !(e1->loc==0U);
Cyc_Absyndump_dumpexps_prec(20,es);
Cyc_Absyndump_dump_nospace(_tag_fat(")",sizeof(char),2U));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;}}case 11: _Tmp6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 736
Cyc_Absyndump_dumploc(e->loc);Cyc_Absyndump_dump(_tag_fat("throw",sizeof(char),6U));Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 12: _Tmp6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 738
_Tmp6=e1;goto _LL36;}case 13: _Tmp6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL36: {struct Cyc_Absyn_Exp*e1=_Tmp6;
Cyc_Absyndump_dumpexp_prec(inprec,e1);goto _LL0;}case 14: _Tmp6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp6;struct Cyc_Absyn_Exp*e1=_Tmp4;
# 742
Cyc_Absyndump_dump_char(40);Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dumpexp_prec(myprec,e1);
goto _LL0;}case 15: _Tmp6=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 745
Cyc_Absyndump_dump_char(38);Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 20: _Tmp6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
Cyc_Absyndump_dump_char(42);Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 16: _Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*ropt=_Tmp6;struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*qual=_Tmp3;
# 749
if((unsigned)ropt){
Cyc_Absyndump_dump(_tag_fat("rnew(",sizeof(char),6U));Cyc_Absyndump_dumpexp_prec(140,ropt);
if((unsigned)qual){
Cyc_Absyndump_dump_char(44);Cyc_Absyndump_dumpexp_prec(140,qual);}
# 754
Cyc_Absyndump_dump_char(41);}else{
# 757
Cyc_Absyndump_dump(_tag_fat("new",sizeof(char),4U));
if((unsigned)qual){
Cyc_Absyndump_dump_char(40);Cyc_Absyndump_dumpexp_prec(140,qual);Cyc_Absyndump_dump_char(41);}}
# 762
Cyc_Absyndump_dumpexp_prec(myprec,e1);goto _LL0;}case 17: _Tmp6=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp6;
Cyc_Absyndump_dump(_tag_fat("sizeof(",sizeof(char),8U));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);goto _LL0;}case 18: _Tmp6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
Cyc_Absyndump_dump(_tag_fat("sizeof(",sizeof(char),8U));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 38: _Tmp6=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp6;
Cyc_Absyndump_dump(_tag_fat("valueof(",sizeof(char),9U));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(41);goto _LL0;}case 40: _Tmp6=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 768
Cyc_Absyndump_dump(_tag_fat("__extension__(",sizeof(char),15U));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 41: _Tmp6=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 770
Cyc_Absyndump_dump(_tag_fat("@assert(",sizeof(char),9U));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 42: _Tmp6=((struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp6;
# 772
Cyc_Absyndump_dump(_tag_fat("@assert_false(",sizeof(char),15U));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_char(41);goto _LL0;}case 39: _Tmp8=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp4=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;_Tmp3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_Tmp0)->f5;{int vol=_Tmp8;struct _fat_ptr t=_TmpA;struct Cyc_List_List*o=_Tmp6;struct Cyc_List_List*i=_Tmp4;struct Cyc_List_List*cl=_Tmp3;
# 775
Cyc_Absyndump_dump(_tag_fat("__asm__",sizeof(char),8U));
if(vol)Cyc_Absyndump_dump(_tag_fat(" volatile ",sizeof(char),11U));
Cyc_Absyndump_dump_nospace(_tag_fat("(\"",sizeof(char),3U));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(t));Cyc_Absyndump_dump_char(34);
if((unsigned)o){
Cyc_Absyndump_dump_char(58);
Cyc_Absyndump_dump_asm_iolist(o);}
# 782
if((unsigned)i){
if(!((unsigned)o))
Cyc_Absyndump_dump_char(58);
Cyc_Absyndump_dump_char(58);
Cyc_Absyndump_dump_asm_iolist(i);}
# 788
if((unsigned)cl){
int ncol=(unsigned)i?2:((unsigned)o?1: 0);
{int cols=0;for(0;cols < 3 - ncol;++ cols){
Cyc_Absyndump_dump_nospace(_tag_fat(" :",sizeof(char),3U));}}
while((unsigned)cl){
Cyc_Absyndump_dump_char(34);
Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(*((struct _fat_ptr*)cl->hd)));
Cyc_Absyndump_dump_char(34);
cl=cl->tl;
if((unsigned)cl)
Cyc_Absyndump_dump_char(44);
# 793
1U;}}
# 801
Cyc_Absyndump_dump_char(41);
goto _LL0;}case 37: _Tmp6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct _fat_ptr*f=_Tmp4;
# 805
Cyc_Absyndump_dump(_tag_fat("tagcheck(",sizeof(char),10U));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*f);
Cyc_Absyndump_dump_char(41);
goto _LL0;}case 19: _Tmp6=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp6;struct Cyc_List_List*l=_Tmp4;
# 810
Cyc_Absyndump_dump(_tag_fat("offsetof(",sizeof(char),10U));Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_group(Cyc_Absyndump_dumpoffset_field,l,_tag_fat(",",sizeof(char),2U),_tag_fat(")",sizeof(char),2U),_tag_fat(".",sizeof(char),2U));goto _LL0;}case 21: _Tmp6=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct _fat_ptr*n=_Tmp4;
# 813
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char(46);Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 22: _Tmp6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct _fat_ptr*n=_Tmp4;
# 815
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(_tag_fat("->",sizeof(char),3U));Cyc_Absyndump_dump_nospace(*n);goto _LL0;}case 23: _Tmp6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 818
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_char(91);Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char(93);goto _LL0;}case 24: _Tmp6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct _tuple9*vat=_Tmp6;struct Cyc_List_List*des=_Tmp4;
# 821
Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dumptyp((*vat).f2);
Cyc_Absyndump_dump_char(41);
({void(*_TmpD)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_TmpD;})(Cyc_Absyndump_dumpde,des,_tag_fat("{",sizeof(char),2U),_tag_fat("}",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));
goto _LL0;}case 25: _Tmp6=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_List_List*des=_Tmp6;
# 827
({void(*_TmpD)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_TmpD;})(Cyc_Absyndump_dumpde,des,_tag_fat("{",sizeof(char),2U),_tag_fat("}",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));goto _LL0;}case 26: _Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 830
Cyc_Absyndump_dump(_tag_fat("{for",sizeof(char),5U));Cyc_Absyndump_dump_str((*vd->name).f1);Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumpexp(e1);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 27: _Tmp6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;void*t=_Tmp4;
# 835
Cyc_Absyndump_dump(_tag_fat("{for x ",sizeof(char),8U));Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumpexp(e);
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumptyp(t);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 28: _Tmp6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct _tuple1*n=_Tmp6;struct Cyc_List_List*ts=_Tmp4;struct Cyc_List_List*des=_Tmp3;
# 840
Cyc_Absyndump_dumpqvar(n);
Cyc_Absyndump_dump_char(123);
if(ts!=0)
Cyc_Absyndump_dumptps(ts);
({void(*_TmpD)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_TmpD;})(Cyc_Absyndump_dumpde,des,_tag_fat("",sizeof(char),1U),_tag_fat("}",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));
goto _LL0;}case 29: _Tmp8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{int is_tuple=_Tmp8;struct Cyc_List_List*des=_Tmp6;
# 848
if(is_tuple){
Cyc_Absyndump_dump(_tag_fat("$(",sizeof(char),3U));Cyc_Absyndump_dumpexps_prec(20,({struct Cyc_List_List*(*_TmpD)(struct Cyc_Absyn_Exp*(*)(struct _tuple19*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple19*),struct Cyc_List_List*))Cyc_List_map;_TmpD;})(({struct Cyc_Absyn_Exp*(*_TmpD)(struct _tuple19*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple19*))Cyc_Core_snd;_TmpD;}),des));Cyc_Absyndump_dump_char(41);}else{
# 851
({void(*_TmpD)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_TmpD;})(Cyc_Absyndump_dumpde,des,_tag_fat("{",sizeof(char),2U),_tag_fat("}",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));}
# 853
goto _LL0;}case 31: _Tmp6=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp6;
_Tmp6=ef;goto _LL66;}case 32: _Tmp6=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL66: {struct Cyc_Absyn_Enumfield*ef=_Tmp6;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 30: _Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp6;struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;
# 858
Cyc_Absyndump_dumpqvar(tuf->name);
if(es!=0)({void(*_TmpD)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_TmpD;})(Cyc_Absyndump_dumpexp,es,_tag_fat("(",sizeof(char),2U),_tag_fat(")",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));
goto _LL0;}case 33: _Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.mknd;_Tmp6=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.rgn;_Tmp4=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.aqual;_Tmp3=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;_Tmp8=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.inline_call;{enum Cyc_Absyn_MallocKind mknd=_Tmp2;struct Cyc_Absyn_Exp*ropt=_Tmp6;struct Cyc_Absyn_Exp*aqopt=_Tmp4;void**topt=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp1;int inline_call=_Tmp8;
# 863
Cyc_Absyndump_dumploc(e->loc);
if((int)mknd==1){
if(ropt!=0){
Cyc_Absyndump_dump(_tag_fat("rcalloc(",sizeof(char),9U));Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(_tag_fat(",",sizeof(char),2U));
if(aqopt!=0){
Cyc_Absyndump_dumpexp(aqopt);Cyc_Absyndump_dump(_tag_fat(",",sizeof(char),2U));}}else{
# 871
if(aqopt!=0){
Cyc_Absyndump_dump(_tag_fat("qcalloc(",sizeof(char),9U));Cyc_Absyndump_dumpexp(aqopt);Cyc_Absyndump_dump(_tag_fat(",",sizeof(char),2U));}else{
# 875
Cyc_Absyndump_dump(_tag_fat("calloc(",sizeof(char),8U));}}
# 877
Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump(_tag_fat(",",sizeof(char),2U));Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftype_exp(*_check_null(topt),0U));Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));}else{
# 880
if(ropt!=0){
Cyc_Absyndump_dump((int)mknd==2?_tag_fat("rvmalloc(",sizeof(char),10U):(inline_call?_tag_fat("rmalloc_inline(",sizeof(char),16U): _tag_fat("rmalloc(",sizeof(char),9U)));
Cyc_Absyndump_dumpexp(ropt);Cyc_Absyndump_dump(_tag_fat(",",sizeof(char),2U));
if(aqopt!=0){
Cyc_Absyndump_dumpexp(aqopt);Cyc_Absyndump_dump(_tag_fat(",",sizeof(char),2U));}}else{
# 887
if(aqopt!=0){
Cyc_Absyndump_dump(_tag_fat("qmalloc(",sizeof(char),9U));Cyc_Absyndump_dumpexp(aqopt);Cyc_Absyndump_dump(_tag_fat(",",sizeof(char),2U));}else{
# 891
Cyc_Absyndump_dump(_tag_fat("malloc(",sizeof(char),8U));}}
# 894
if(topt!=0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_TmpD=Cyc_Absyn_sizeoftype_exp(*topt,0U);Cyc_Absyn_times_exp(_TmpD,e,0U);}));else{
# 897
Cyc_Absyndump_dumpexp(e);}
Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));}
# 900
goto _LL0;}case 34: _Tmp6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 903
Cyc_Absyndump_dumpexp_prec(myprec,e1);Cyc_Absyndump_dump_nospace(_tag_fat(":=:",sizeof(char),4U));Cyc_Absyndump_dumpexp_prec(myprec,e2);goto _LL0;}case 35: _Tmp6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt*n=_Tmp6;struct Cyc_List_List*des=_Tmp4;
# 906
({void(*_TmpD)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple19*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_TmpD;})(Cyc_Absyndump_dumpde,des,_tag_fat("{",sizeof(char),2U),_tag_fat("}",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));goto _LL0;}default: _Tmp6=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp6;
# 909
Cyc_Absyndump_dump_nospace(_tag_fat("({",sizeof(char),3U));Cyc_Absyndump_dumpstmt(s,1,0);Cyc_Absyndump_dump_nospace(_tag_fat("})",sizeof(char),3U));goto _LL0;}}_LL0:;}
# 911
if(inprec >= myprec &&(inprec!=140 || myprec!=140))
# 913
Cyc_Absyndump_dump_char(41);}
# 916
static void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 920
static void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs,struct Cyc_List_List*varsinblock){
# 922
for(1;scs!=0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*c=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(c->where_clause==0 && c->pattern->r==(void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(_tag_fat("default:",sizeof(char),9U));else{
# 927
Cyc_Absyndump_dump(_tag_fat("case",sizeof(char),5U));
Cyc_Absyndump_dumppat(c->pattern);
if(c->where_clause!=0){
Cyc_Absyndump_dump(_tag_fat("&&",sizeof(char),3U));
Cyc_Absyndump_dumpexp(_check_null(c->where_clause));}
# 933
Cyc_Absyndump_dump_nospace(_tag_fat(":",sizeof(char),2U));}
# 935
if(Cyc_Absynpp_is_declaration(c->body)){
Cyc_Absyndump_dump(_tag_fat(" {",sizeof(char),3U));Cyc_Absyndump_dumpstmt(c->body,0,0),Cyc_Absyndump_dump_char(125);}else{
# 938
Cyc_Absyndump_dumpstmt(c->body,0,varsinblock);}}}
# 942
static void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt,struct Cyc_List_List*varsinblock){
Cyc_Absyndump_dumploc(s->loc);{
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Absyndump_dumpexp_prec(-100,e);Cyc_Absyndump_dump_semi();goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
# 951
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);}else{
# 954
Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}
if(Cyc_Absynpp_is_declaration(s2)){
if(expstmt)Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpstmt(s2,expstmt,0);Cyc_Absyndump_dump_char(125);
if(expstmt){Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dump_semi();}}else{
# 960
Cyc_Absyndump_dumpstmt(s2,expstmt,varsinblock);}
goto _LL0;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1==0){
Cyc_Absyndump_dump(_tag_fat("return;",sizeof(char),8U));goto _LL0;}else{_Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Absyndump_dump(_tag_fat("return",sizeof(char),7U));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_semi();goto _LL0;}}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 965
Cyc_Absyndump_dump(_tag_fat("if(",sizeof(char),4U));Cyc_Absyndump_dumpexp(e);
{void*_Tmp5=s1->r;switch(*((int*)_Tmp5)){case 2:
 goto _LL29;case 12: _LL29:
 goto _LL2B;case 4: _LL2B:
 goto _LL2D;case 13: _LL2D:
# 971
 Cyc_Absyndump_dump_nospace(_tag_fat("){",sizeof(char),3U));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);goto _LL25;default:
 Cyc_Absyndump_dump_char(41);Cyc_Absyndump_dumpstmt(s1,0,varsinblock);}_LL25:;}
# 974
{void*_Tmp5=s2->r;if(*((int*)_Tmp5)==0)
goto _LL30;else{
Cyc_Absyndump_dump(_tag_fat("else{",sizeof(char),6U));Cyc_Absyndump_dumpstmt(s2,0,0);Cyc_Absyndump_dump_char(125);goto _LL30;}_LL30:;}
# 978
goto _LL0;}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 981
Cyc_Absyndump_dump(_tag_fat("while(",sizeof(char),7U));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(_tag_fat("){",sizeof(char),3U));
Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 985
Cyc_Absyndump_dump(_tag_fat("for(",sizeof(char),5U));Cyc_Absyndump_dumpexp(e1);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e2);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(e3);
Cyc_Absyndump_dump_nospace(_tag_fat("){",sizeof(char),3U));Cyc_Absyndump_dumpstmt(s1,0,0);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 989
Cyc_Absyndump_dump(_tag_fat("do{",sizeof(char),4U));Cyc_Absyndump_dumpstmt(s1,0,0);
Cyc_Absyndump_dump_nospace(_tag_fat("}while(",sizeof(char),8U));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(_tag_fat(");",sizeof(char),3U));
goto _LL0;}case 6:
# 993
 Cyc_Absyndump_dump(_tag_fat("break;",sizeof(char),7U));goto _LL0;case 7:
 Cyc_Absyndump_dump(_tag_fat("continue;",sizeof(char),10U));goto _LL0;case 8: _Tmp4=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct _fat_ptr*x=_Tmp4;
Cyc_Absyndump_dump(_tag_fat("goto",sizeof(char),5U));Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_semi();goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 998
Cyc_Absyndump_dump(_tag_fat("switch(",sizeof(char),8U));Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_nospace(_tag_fat("){",sizeof(char),3U));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);
Cyc_Absyndump_dump_char(125);
goto _LL0;}case 15: _Tmp4=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_List_List*ss=_Tmp3;
# 1003
Cyc_Absyndump_dump(_tag_fat("try",sizeof(char),4U));Cyc_Absyndump_dumpstmt(s1,0,varsinblock);
Cyc_Absyndump_dump(_tag_fat("catch{",sizeof(char),7U));
Cyc_Absyndump_dumpswitchclauses(ss,varsinblock);Cyc_Absyndump_dump_char(125);
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1009
struct _tuple14 _Tmp5=Cyc_Absynpp_shadows(d,varsinblock);void*_Tmp6;int _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{int newblock=_Tmp7;struct Cyc_List_List*newvarsinblock=_Tmp6;
if(newblock){
if(expstmt)Cyc_Absyndump_dump(_tag_fat("({",sizeof(char),3U));else{Cyc_Absyndump_dump(_tag_fat("{",sizeof(char),2U));}
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(_tag_fat("});",sizeof(char),4U));else{Cyc_Absyndump_dump(_tag_fat("}",sizeof(char),2U));}}else{
# 1016
Cyc_Absyndump_dumpdecl(d);Cyc_Absyndump_dumpstmt(s1,expstmt,newvarsinblock);}
# 1018
goto _LL0;}}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*x=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;
# 1024
if(Cyc_Absynpp_is_declaration(s1)){
Cyc_Absyndump_dump_str(x);
if(expstmt)Cyc_Absyndump_dump_nospace(_tag_fat(": ({",sizeof(char),5U));else{Cyc_Absyndump_dump_nospace(_tag_fat(": {",sizeof(char),4U));}
Cyc_Absyndump_dumpstmt(s1,expstmt,0);
if(expstmt)Cyc_Absyndump_dump_nospace(_tag_fat("});",sizeof(char),4U));else{Cyc_Absyndump_dump_char(125);}}else{
# 1030
Cyc_Absyndump_dump_str(x);Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpstmt(s1,expstmt,varsinblock);}
# 1032
goto _LL0;}default: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1==0){
# 1034
Cyc_Absyndump_dump(_tag_fat("fallthru;",sizeof(char),10U));goto _LL0;}else{_Tmp4=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_List_List*es=_Tmp4;
# 1036
Cyc_Absyndump_dump(_tag_fat("fallthru(",sizeof(char),10U));Cyc_Absyndump_dumpexps_prec(20,es);Cyc_Absyndump_dump_nospace(_tag_fat(");",sizeof(char),3U));goto _LL0;}}}_LL0:;}}struct _tuple21{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 1040
static void Cyc_Absyndump_dumpdp(struct _tuple21*dp){
Cyc_Absyndump_egroup(Cyc_Absyndump_dumpdesignator,(*dp).f0,_tag_fat("",sizeof(char),1U),_tag_fat("=",sizeof(char),2U),_tag_fat("=",sizeof(char),2U));
Cyc_Absyndump_dumppat((*dp).f1);}
# 1044
static struct _fat_ptr Cyc_Absyndump_pat_term(int dots){if(dots)return _tag_fat("...)",sizeof(char),5U);else{return _tag_fat(")",sizeof(char),2U);}}
# 1046
static void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;int _Tmp1;void*_Tmp2;void*_Tmp3;char _Tmp4;void*_Tmp5;struct _fat_ptr _Tmp6;int _Tmp7;switch(*((int*)_Tmp0)){case 0:
 Cyc_Absyndump_dump_char(95);goto _LL0;case 8:
 Cyc_Absyndump_dump(_tag_fat("NULL",sizeof(char),5U));goto _LL0;case 9: switch((int)((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1){case Cyc_Absyn_None: _Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{int i=_Tmp7;
_Tmp7=i;goto _LL8;}case Cyc_Absyn_Signed: _Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL8: {int i=_Tmp7;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)i;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp9,sizeof(void*),1));}));goto _LL0;}default: _Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{int i=_Tmp7;
Cyc_Absyndump_dump(({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned)i;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("%u",sizeof(char),3U),_tag_fat(_Tmp9,sizeof(void*),1));}));goto _LL0;}}case 11: _Tmp6=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _fat_ptr x=_Tmp6;
Cyc_Absyndump_dump(x);goto _LL0;}case 12: _Tmp5=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp5;
_Tmp5=ef;goto _LL10;}case 13: _Tmp5=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL10: {struct Cyc_Absyn_Enumfield*ef=_Tmp5;
Cyc_Absyndump_dumpqvar(ef->name);goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char ch=_Tmp4;
# 1057
Cyc_Absyndump_dump(_tag_fat("'",sizeof(char),2U));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(ch));Cyc_Absyndump_dump_nospace(_tag_fat("'",sizeof(char),2U));goto _LL0;}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;
Cyc_Absyndump_dump(_tag_fat("*",sizeof(char),2U));_Tmp5=vd;goto _LL16;}}else{_Tmp5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;struct Cyc_Absyn_Pat*p2=_Tmp3;
# 1060
Cyc_Absyndump_dump(_tag_fat("*",sizeof(char),2U));_Tmp5=vd;_Tmp3=p2;goto _LL1A;}}case 1: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp5=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_LL16: {struct Cyc_Absyn_Vardecl*vd=_Tmp5;
# 1059
Cyc_Absyndump_dumpqvar(vd->name);goto _LL0;}}else{_Tmp5=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Vardecl*vd=_Tmp5;struct Cyc_Absyn_Pat*p2=_Tmp3;
# 1061
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump(_tag_fat(" as ",sizeof(char),5U));Cyc_Absyndump_dumppat(p2);goto _LL0;}}case 2: _Tmp5=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp5;struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1063
Cyc_Absyndump_dump(_tag_fat("alias",sizeof(char),6U));Cyc_Absyndump_dump(_tag_fat("<",sizeof(char),2U));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(_tag_fat(">",sizeof(char),2U));
Cyc_Absyndump_dumpvardecl(vd,p->loc,0);
goto _LL0;}case 5: _Tmp5=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp5;
Cyc_Absyndump_dump(_tag_fat("&",sizeof(char),2U));Cyc_Absyndump_dumppat(p2);goto _LL0;}case 4: _Tmp5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp5;struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 1068
Cyc_Absyndump_dumpqvar(vd->name);Cyc_Absyndump_dump_char(60);Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump_char(62);goto _LL0;}case 14: _Tmp5=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple1*q=_Tmp5;
Cyc_Absyndump_dumpqvar(q);goto _LL0;}case 15: _Tmp5=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct _tuple1*q=_Tmp5;struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp7;
# 1071
Cyc_Absyndump_dumpqvar(q);({void(*_Tmp8)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({void(*_Tmp9)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_Tmp9;});struct Cyc_List_List*_Tmp9=ps;struct _fat_ptr _TmpA=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _TmpB=Cyc_Absyndump_pat_term(dots);_Tmp8(Cyc_Absyndump_dumppat,_Tmp9,_TmpA,_TmpB,_tag_fat(",",sizeof(char),2U));});goto _LL0;}case 6: _Tmp5=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f5;{void*topt=_Tmp5;int is_tuple=_Tmp7;struct Cyc_List_List*exists=_Tmp3;struct Cyc_List_List*dps=_Tmp2;int dots=_Tmp1;
# 1073
if(is_tuple)
({void(*_Tmp8)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({void(*_Tmp9)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_Tmp9;});struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*(*_TmpA)(struct Cyc_Absyn_Pat*(*)(struct _tuple21*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Pat*(*)(struct _tuple21*),struct Cyc_List_List*))Cyc_List_map;_TmpA;})(({struct Cyc_Absyn_Pat*(*_TmpA)(struct _tuple21*)=(struct Cyc_Absyn_Pat*(*)(struct _tuple21*))Cyc_Core_snd;_TmpA;}),dps);struct _fat_ptr _TmpA=_tag_fat("$(",sizeof(char),3U);struct _fat_ptr _TmpB=Cyc_Absyndump_pat_term(dots);_Tmp8(Cyc_Absyndump_dumppat,_Tmp9,_TmpA,_TmpB,_tag_fat(",",sizeof(char),2U));});else{
# 1076
if(topt!=0){
void*_Tmp8=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _Tmp9;if(*((int*)_Tmp8)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)==24){_Tmp9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp9;
# 1079
Cyc_Absyndump_dumpqvar(Cyc_Absyn_aggr_kinded_name(info).f1);goto _LL2B;}}else{goto _LL2E;}}else{_LL2E:
 goto _LL2B;}_LL2B:;}
# 1083
Cyc_Absyndump_dump_char(123);
({void(*_Tmp8)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;_Tmp8;})(Cyc_Absyndump_dumptvar,exists,_tag_fat("<",sizeof(char),2U),_tag_fat(">",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));
({void(*_Tmp8)(void(*)(struct _tuple21*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({void(*_Tmp9)(void(*)(struct _tuple21*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple21*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_Tmp9;});struct Cyc_List_List*_Tmp9=dps;struct _fat_ptr _TmpA=_tag_fat("",sizeof(char),1U);struct _fat_ptr _TmpB=Cyc_Absyndump_pat_term(dots);_Tmp8(Cyc_Absyndump_dumpdp,_Tmp9,_TmpA,_TmpB,_tag_fat(",",sizeof(char),2U));});}
# 1087
goto _LL0;}case 7: _Tmp5=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp5;struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp7;
# 1089
Cyc_Absyndump_dumpqvar(tuf->name);
if(ps!=0)({void(*_Tmp8)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=({void(*_Tmp9)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_Tmp9;});struct Cyc_List_List*_Tmp9=ps;struct _fat_ptr _TmpA=_tag_fat("(",sizeof(char),2U);struct _fat_ptr _TmpB=Cyc_Absyndump_pat_term(dots);_Tmp8(Cyc_Absyndump_dumppat,_Tmp9,_TmpA,_TmpB,_tag_fat(",",sizeof(char),2U));});
goto _LL0;}default: _Tmp5=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
Cyc_Absyndump_dumpexp(e);goto _LL0;}}_LL0:;}
# 1096
static void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs!=0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1101
static void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({void(*_Tmp0)(void(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;_Tmp0;})(Cyc_Absyndump_dumpdatatypefield,fields,_tag_fat(",",sizeof(char),2U));}
# 1104
static void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag!=0){
Cyc_Absyndump_dump(_tag_fat("=",sizeof(char),2U));Cyc_Absyndump_dumpexp(_check_null(ef->tag));}}
# 1110
static void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({void(*_Tmp0)(void(*)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*,struct _fat_ptr))Cyc_Absyndump_dump_sep;_Tmp0;})(Cyc_Absyndump_dumpenumfield,fields,_tag_fat(",",sizeof(char),2U));}
# 1114
static void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(1;fields!=0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_Tmp0=(struct Cyc_Absyn_Aggrfield*)fields->hd;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;_Tmp6=_Tmp0->name;_Tmp5=_Tmp0->tq;_Tmp4=_Tmp0->type;_Tmp3=_Tmp0->width;_Tmp2=_Tmp0->attributes;_Tmp1=_Tmp0->requires_clause;{struct _fat_ptr*name=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*type=_Tmp4;struct Cyc_Absyn_Exp*width=_Tmp3;struct Cyc_List_List*atts=_Tmp2;struct Cyc_Absyn_Exp*req=_Tmp1;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
({void(*_Tmp7)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd;_Tmp7;})(tq,type,Cyc_Absyndump_dump_str,name);Cyc_Absyndump_dumpatts(atts);goto _LL3;}else{
Cyc_Absyndump_dumpatts(atts);({void(*_Tmp7)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _fat_ptr*),struct _fat_ptr*))Cyc_Absyndump_dumptqtd;_Tmp7;})(tq,type,Cyc_Absyndump_dump_str,name);goto _LL3;}_LL3:;
# 1121
if((unsigned)req){
Cyc_Absyndump_dump(_tag_fat("@requires ",sizeof(char),11U));Cyc_Absyndump_dumpexp(req);}
# 1124
if(width!=0){
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dumpexp(width);}
# 1127
Cyc_Absyndump_dump_semi();}}}
# 1131
static void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1136
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts(fd->i.attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple22{void*f0;struct _tuple1*f1;};
# 1140
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple22*pr){
{void*_Tmp0=(*pr).f0;switch(*((int*)_Tmp0)){case 11:
 goto _LL0;case 1:
 Cyc_Absyndump_dump(_tag_fat("_stdcall",sizeof(char),9U));goto _LL0;case 2:
 Cyc_Absyndump_dump(_tag_fat("_cdecl",sizeof(char),7U));goto _LL0;case 3:
 Cyc_Absyndump_dump(_tag_fat("_fastcall",sizeof(char),10U));goto _LL0;default:
 goto _LL0;}_LL0:;}
# 1148
Cyc_Absyndump_dumpqvar((*pr).f1);}
# 1150
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->i.attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1154
static void Cyc_Absyndump_dumpid(struct Cyc_Absyn_Vardecl*vd){
Cyc_Absyndump_dumpqvar(vd->name);}
# 1158
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1160
void*_Tmp0;switch(*((int*)t)){case 1: _Tmp0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)t)->f2;if((void*)_Tmp0!=0){void*def=_Tmp0;
return Cyc_Absyndump_is_char_ptr(def);}else{goto _LL5;}case 3: _Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;{void*elt_typ=_Tmp0;
# 1163
while(1){
{void*_Tmp1;switch(*((int*)elt_typ)){case 1: _Tmp1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)elt_typ)->f2;if((void*)_Tmp1!=0){void*t=_Tmp1;
elt_typ=t;goto _LL7;}else{goto _LLE;}case 7: _Tmp1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)elt_typ)->f4;if((void*)_Tmp1!=0){void*t=_Tmp1;
elt_typ=t;goto _LL7;}else{goto _LLE;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_typ)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)elt_typ)->f1)->f2==Cyc_Absyn_Char_sz)
return 1;else{goto _LLE;}}else{goto _LLE;}default: _LLE:
 return 0;}_LL7:;}
# 1164
1U;}}default: _LL5:
# 1170
 return 0;};}
# 1174
static void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned loc,int do_semi){
int _Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;unsigned _Tmp5;void*_Tmp6;enum Cyc_Absyn_Scope _Tmp7;_Tmp7=vd->sc;_Tmp6=vd->name;_Tmp5=vd->varloc;_Tmp4=vd->tq;_Tmp3=vd->type;_Tmp2=vd->initializer;_Tmp1=vd->attributes;_Tmp0=vd->is_proto;{enum Cyc_Absyn_Scope sc=_Tmp7;struct _tuple1*name=_Tmp6;unsigned varloc=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*type=_Tmp3;struct Cyc_Absyn_Exp*initializer=_Tmp2;struct Cyc_List_List*atts=_Tmp1;int is_proto=_Tmp0;
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
# 1179
if((int)sc==3 && Cyc_Absyndump_qvar_to_Cids){
void*_Tmp8=Cyc_Absyn_compress(type);if(*((int*)_Tmp8)==5){
# 1182
if(!is_proto)
Cyc_Absyndump_dumpscope(sc);
goto _LL8;}else{
Cyc_Absyndump_dumpscope(sc);}_LL8:;}else{
# 1188
Cyc_Absyndump_dumpscope(sc);}
({void(*_Tmp8)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _tuple1*),struct _tuple1*)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct _tuple1*),struct _tuple1*))Cyc_Absyndump_dumptqtd;_Tmp8;})(tq,type,Cyc_Absyndump_dumpqvar,name);
Cyc_Absyndump_dumpatts(atts);
goto _LL3;}else{
# 1193
Cyc_Absyndump_dumpatts(atts);
Cyc_Absyndump_dumpscope(sc);{
struct _RegionHandle _Tmp8=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp8;_push_region(temp);
{int is_cp=Cyc_Absyndump_is_char_ptr(type);
struct _tuple13 _Tmp9=Cyc_Absynpp_to_tms(temp,tq,type);void*_TmpA;void*_TmpB;struct Cyc_Absyn_Tqual _TmpC;_TmpC=_Tmp9.f0;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{struct Cyc_Absyn_Tqual tq=_TmpC;void*t=_TmpB;struct Cyc_List_List*tms=_TmpA;
# 1199
void*call_conv=(void*)& Cyc_Atts_Unused_att_val;
{struct Cyc_List_List*tms2=tms;for(0;tms2!=0;tms2=tms2->tl){
void*_TmpD=(void*)tms2->hd;void*_TmpE;if(*((int*)_TmpD)==5){_TmpE=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_TmpD)->f2;{struct Cyc_List_List*atts=_TmpE;
# 1203
for(1;atts!=0;atts=atts->tl){
void*_TmpF=(void*)atts->hd;switch(*((int*)_TmpF)){case 1:
 call_conv=(void*)& Cyc_Atts_Stdcall_att_val;goto _LL15;case 2:
 call_conv=(void*)& Cyc_Atts_Cdecl_att_val;goto _LL15;case 3:
 call_conv=(void*)& Cyc_Atts_Fastcall_att_val;goto _LL15;default:
 goto _LL15;}_LL15:;}
# 1210
goto _LL10;}}else{
goto _LL10;}_LL10:;}}
# 1213
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);{
struct _tuple22 pr=({struct _tuple22 _TmpD;_TmpD.f0=call_conv,_TmpD.f1=name;_TmpD;});
({void(*_TmpD)(int,struct Cyc_List_List*,void(*)(struct _tuple22*),struct _tuple22*)=({void(*_TmpE)(int,struct Cyc_List_List*,void(*)(struct _tuple22*),struct _tuple22*)=(void(*)(int,struct Cyc_List_List*,void(*)(struct _tuple22*),struct _tuple22*))Cyc_Absyndump_dumptms;_TmpE;});int _TmpE=is_cp;_TmpD(_TmpE,Cyc_List_imp_rev(tms),Cyc_Absyndump_dump_callconv_qvar,& pr);});
_npop_handler(0);goto _LL3;}}}
# 1196
;_pop_region();}}_LL3:;
# 1220
if(initializer!=0){
Cyc_Absyndump_dump_char(61);Cyc_Absyndump_dumpexp(initializer);}
# 1223
if(do_semi)Cyc_Absyndump_dump_semi();}}
# 1226
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl!=0 && ad->impl->tagged)
Cyc_Absyndump_dump(_tag_fat("@tagged ",sizeof(char),9U));
Cyc_Absyndump_dumpaggr_kind(ad->kind);Cyc_Absyndump_dumpqvar(ad->name);Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl!=0){
Cyc_Absyndump_dump_char(123);
({void(*_Tmp0)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;_Tmp0;})(Cyc_Absyndump_dumpkindedtvar,_check_null(ad->impl)->exist_vars,_tag_fat("<",sizeof(char),2U),_tag_fat(">",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));
if(_check_null(ad->impl)->effconstr!=0){
Cyc_Absyndump_dump_char(58);Cyc_Absyndump_dump_effc(_check_null(ad->impl)->effconstr);}
# 1237
if(_check_null(ad->impl)->qual_bnd!=0){
char sep=ad->impl->effconstr==0?':':',';
Cyc_Absyndump_dump_char((int)sep);Cyc_Absyndump_dump_qualbnd(_check_null(ad->impl)->qual_bnd);}
# 1241
Cyc_Absyndump_dumpaggrfields(_check_null(ad->impl)->fields);
Cyc_Absyndump_dump_char(125);
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1246
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Scope _Tmp2;_Tmp2=ed->sc;_Tmp1=ed->name;_Tmp0=ed->fields;{enum Cyc_Absyn_Scope sc=_Tmp2;struct _tuple1*nm=_Tmp1;struct Cyc_Core_Opt*fields=_Tmp0;
Cyc_Absyndump_dumpscope(sc);Cyc_Absyndump_dump(_tag_fat("enum ",sizeof(char),6U));Cyc_Absyndump_dumpqvar(nm);
if(fields!=0){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char(125);}}}
# 1253
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
int _Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;enum Cyc_Absyn_Scope _Tmp4;_Tmp4=dd->sc;_Tmp3=dd->name;_Tmp2=dd->tvs;_Tmp1=dd->fields;_Tmp0=dd->is_extensible;{enum Cyc_Absyn_Scope sc=_Tmp4;struct _tuple1*name=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_Core_Opt*fields=_Tmp1;int is_x=_Tmp0;
Cyc_Absyndump_dumpscope(sc);
if(is_x)
Cyc_Absyndump_dump(_tag_fat("@extensible ",sizeof(char),13U));
Cyc_Absyndump_dump(_tag_fat("datatype ",sizeof(char),10U));Cyc_Absyndump_dumpqvar(name);Cyc_Absyndump_dumptvars(tvs);
if(fields!=0){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)fields->v);Cyc_Absyndump_dump_char(125);}}}struct _tuple23{unsigned f0;struct _tuple1*f1;int f2;};
# 1264
static void Cyc_Absyndump_dumpexport(struct _tuple23*tup){
Cyc_Absyndump_dumpqvar((*tup).f1);}
# 1267
static void Cyc_Absyndump_dump_braced_decls(struct Cyc_List_List*tdl){
Cyc_Absyndump_dump_char(123);
for(1;tdl!=0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_Absyndump_dump_char(125);}
# 1274
static void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 1278
if(Cyc_Flags_c_compiler==Cyc_Flags_Vc_c){
Cyc_Absyndump_dumpatts(fd->i.attributes);goto _LL23;}else{
goto _LL23;}_LL23:;
# 1282
if(fd->is_inline){
if(Cyc_Flags_c_compiler==Cyc_Flags_Vc_c){
Cyc_Absyndump_dump(_tag_fat("__inline",sizeof(char),9U));goto _LL28;}else{
Cyc_Absyndump_dump(_tag_fat("inline",sizeof(char),7U));goto _LL28;}_LL28:;}
# 1287
Cyc_Absyndump_dumpscope(fd->sc);{
# 1289
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp5->tag=5,_Tmp5->f1=type_info;_Tmp5;});
if(fd->cached_type!=0){
void*_Tmp5=Cyc_Absyn_compress(fd->cached_type);struct Cyc_Absyn_FnInfo _Tmp6;if(*((int*)_Tmp5)==5){_Tmp6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp5)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp6;
# 1295
({struct Cyc_List_List*_Tmp7=Cyc_List_append(fd->i.attributes,i.attributes);fd->i.attributes=_Tmp7;});goto _LL2D;}}else{
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("function has non-function type",sizeof(char),31U),_tag_fat(0U,sizeof(void*),0));}_LL2D:;}
# 1299
if(Cyc_Flags_c_compiler==Cyc_Flags_Gcc_c){
({void(*_Tmp5)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=({void(*_Tmp6)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;_Tmp6;});struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=t;_Tmp5(_Tmp6,_Tmp7,Cyc_Absyndump_dump_atts_qvar,fd);});goto _LL32;}else{
({void(*_Tmp5)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=({void(*_Tmp6)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;_Tmp6;});struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=t;_Tmp5(_Tmp6,_Tmp7,Cyc_Absyndump_dump_callconv_fdqvar,fd);});goto _LL32;}_LL32:;
# 1303
Cyc_Absyndump_dump_char(123);
Cyc_Absyndump_dumpstmt(fd->body,0,0);
Cyc_Absyndump_dump_char(125);
goto _LL0;}}}case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);goto _LL0;}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
Cyc_Absyndump_dump_aggrdecl(ad);Cyc_Absyndump_dump_semi();goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp4;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump_semi();goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump_semi();goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp4;
({void(*_Tmp5)(void(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_Tmp5;})(Cyc_Absyndump_dumpid,vds,_tag_fat(" let ",sizeof(char),6U),_tag_fat(";",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));goto _LL0;}case 2: _Tmp4=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Pat*p=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 1313
Cyc_Absyndump_dump(_tag_fat("let",sizeof(char),4U));Cyc_Absyndump_dumppat(p);Cyc_Absyndump_dump_char(61);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_semi();goto _LL0;}case 4: _Tmp4=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*open_exp_opt=_Tmp2;
# 1315
Cyc_Absyndump_dump(_tag_fat("region",sizeof(char),7U));Cyc_Absyndump_dump(_tag_fat("<",sizeof(char),2U));Cyc_Absyndump_dumptvar(tv);Cyc_Absyndump_dump(_tag_fat("> ",sizeof(char),3U));Cyc_Absyndump_dumpqvar(vd->name);
if((unsigned)open_exp_opt){
Cyc_Absyndump_dump(_tag_fat(" = open(",sizeof(char),9U));Cyc_Absyndump_dumpexp(open_exp_opt);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));}
# 1319
Cyc_Absyndump_dump_semi();
goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 1326
Cyc_Absyndump_dump(_tag_fat("typedef",sizeof(char),8U));{
void*t=td->defn==0?Cyc_Absyn_new_evar(td->kind,0): td->defn;
({void(*_Tmp5)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd;_Tmp5;})(td->tq,t,Cyc_Absyndump_dumptypedefname,td);
Cyc_Absyndump_dumpatts(td->atts);
Cyc_Absyndump_dump_semi();
# 1332
goto _LL0;}}case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 1334
Cyc_Absyndump_dump(_tag_fat("namespace",sizeof(char),10U));Cyc_Absyndump_dump_str(v);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _tuple1*q=_Tmp4;struct Cyc_List_List*tdl=_Tmp3;
# 1336
Cyc_Absyndump_dump(_tag_fat("using",sizeof(char),6U));Cyc_Absyndump_dumpqvar(q);Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*tdl=_Tmp4;
# 1338
Cyc_Absyndump_dump(_tag_fat("extern \"C\" ",sizeof(char),12U));Cyc_Absyndump_dump_braced_decls(tdl);goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List*tdl=_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exs=_Tmp2;struct _tuple11*wc=_Tmp1;
# 1340
Cyc_Absyndump_dump(_tag_fat("extern \"C include\" ",sizeof(char),20U));Cyc_Absyndump_dump_braced_decls(tdl);
if(ovrs!=0){
Cyc_Absyndump_dump(_tag_fat(" cyclone_override ",sizeof(char),19U));
Cyc_Absyndump_dump_braced_decls(ovrs);}
# 1345
if((unsigned)wc){
Cyc_Absyndump_dump(_tag_fat(" export { * }\n",sizeof(char),15U));{
void*_Tmp5;_Tmp5=wc->f1;{struct Cyc_List_List*hides=_Tmp5;
if((unsigned)hides)
({void(*_Tmp6)(void(*)(struct _tuple1*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple1*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;_Tmp6;})(Cyc_Absyndump_dumpqvar,hides,_tag_fat(" cyclone_hide {",sizeof(char),16U),_tag_fat("}",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));}}}else{
# 1353
({void(*_Tmp5)(void(*)(struct _tuple23*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _tuple23*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_egroup;_Tmp5;})(Cyc_Absyndump_dumpexport,exs,_tag_fat(" export {",sizeof(char),10U),_tag_fat("}",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));}
# 1355
goto _LL0;}case 13:
 Cyc_Absyndump_dump(_tag_fat(" __cyclone_port_on__; ",sizeof(char),23U));goto _LL0;case 14:
 Cyc_Absyndump_dump(_tag_fat(" __cyclone_port_off__; ",sizeof(char),24U));goto _LL0;case 15:
 Cyc_Absyndump_dump(_tag_fat(" __tempest_on__; ",sizeof(char),18U));goto _LL0;default:
 Cyc_Absyndump_dump(_tag_fat(" __tempest_off__; ",sizeof(char),19U));goto _LL0;}_LL0:;}}
# 1363
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple15 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f0!=1U || !pr.f1){
Cyc_Absyndump_dump_char(123);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(125);}}
# 1370
static void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
# 1372
if(tms==0){
f(a);
return;}{
# 1376
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;if(*((int*)_Tmp0)==2){_Tmp7=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.eff;_Tmp6=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.nullable;_Tmp5=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.bounds;_Tmp4=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.zero_term;_Tmp3=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.autoreleased;_Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1.aqual;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*rgn=_Tmp7;void*nullable=_Tmp6;void*bd=_Tmp5;void*zt=_Tmp4;void*rel=_Tmp3;void*aq=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 1380
{void*_Tmp8=Cyc_Absyn_compress(bd);void*_Tmp9;if(*((int*)_Tmp8)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)){case 14:
 Cyc_Absyndump_dump_char(63);goto _LL5;case 13: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f2)->hd)==8){_Tmp9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f2->hd)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 1383
Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));Cyc_Absyndump_dump_upperbound(e);goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}default: goto _LLA;}else{_LLA:
# 1385
 Cyc_Absyndump_dump_char((int)(Cyc_Absyn_type2bool(1,nullable)?'*':'@'));}_LL5:;}
# 1387
if(!Cyc_Absyndump_qvar_to_Cids){
void*_Tmp8=Cyc_Absyn_compress(aq);void*_Tmp9;enum Cyc_Absyn_AliasQualVal _TmpA;switch(*((int*)_Tmp8)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)){case 16: _TmpA=((struct Cyc_Absyn_AqualConstCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)->f1;{enum Cyc_Absyn_AliasQualVal aqv=_TmpA;
# 1390
if(aqv==Cyc_Absyn_Aliasable_qual)
goto _LL15;else{
# 1393
Cyc_Absyndump_dump(_tag_fat("@aqual(",sizeof(char),8U));Cyc_Absyndump_dump_aqual_val(aqv);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));goto _LL15;}_LL15:;
# 1395
goto _LLC;}case 17: _Tmp9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f2;{struct Cyc_List_List*tv=_Tmp9;
# 1397
Cyc_Absyndump_dump(_tag_fat("@aqual(",sizeof(char),8U));Cyc_Absyndump_dumpntyp((void*)_check_null(tv)->hd);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));
goto _LLC;}default: goto _LL13;}case 1:
# 1400
 Cyc_Absyndump_dump(_tag_fat("@aqual(",sizeof(char),8U));Cyc_Absyndump_dumpntyp(aq);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));
goto _LLC;default: _LL13:
# 1403
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("unexpected alias qualifier",sizeof(char),27U);_TmpC;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpD;_TmpD.tag=2,_TmpD.f1=(void*)aq;_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;Cyc_Warn_impos2(_tag_fat(_TmpD,sizeof(void*),2));});}_LLC:;}
# 1405
if((!Cyc_Absyndump_qvar_to_Cids && !is_char_ptr)&& Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(_tag_fat("@zeroterm",sizeof(char),10U));
if((!Cyc_Absyndump_qvar_to_Cids && is_char_ptr)&& !Cyc_Absyn_type2bool(0,zt))Cyc_Absyndump_dump(_tag_fat("@nozeroterm",sizeof(char),12U));
if(!Cyc_Absyndump_qvar_to_Cids && Cyc_Absyn_type2bool(0,rel))Cyc_Absyndump_dump(_tag_fat("@autoreleased",sizeof(char),14U));
{void*_Tmp8=Cyc_Absyn_compress(rgn);void*_Tmp9;switch(*((int*)_Tmp8)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)){case 6:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(_tag_fat("`H",sizeof(char),3U));goto _LL1A;case 7:
 Cyc_Absyndump_dump(_tag_fat("`U",sizeof(char),3U));goto _LL1A;case 8:
 Cyc_Absyndump_dump(_tag_fat("`RC",sizeof(char),4U));goto _LL1A;default: goto _LL27;}case 2: _Tmp9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp8)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp9;
Cyc_Absyndump_dumptvar(tv);goto _LL1A;}case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_Tmp8)->f2==0){
Cyc_Absyndump_dumpntyp(Cyc_Absyn_compress(rgn));goto _LL1A;}else{goto _LL27;}case 7:
 Cyc_Absyndump_dump(_tag_fat("@region(",sizeof(char),9U));Cyc_Absyndump_dumptyp(rgn);Cyc_Absyndump_dump(_tag_fat(")",sizeof(char),2U));goto _LL1A;default: _LL27:
 Cyc_Warn_impos(_tag_fat("dumptms: bad rgn type in Pointer_mod",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));}_LL1A:;}
# 1417
Cyc_Absyndump_dumptq(tq2);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}}else{
# 1422
int next_is_pointer=0;
if(tms->tl!=0){
void*_Tmp8=(void*)tms->tl->hd;if(*((int*)_Tmp8)==2){
next_is_pointer=1;goto _LL29;}else{
goto _LL29;}_LL29:;}
# 1428
if(next_is_pointer)
Cyc_Absyndump_dump_char(40);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char(41);
{void*_Tmp8=(void*)tms->hd;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;int _TmpF;void*_Tmp10;void*_Tmp11;switch(*((int*)_Tmp8)){case 0: _Tmp11=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1;{void*zeroterm=_Tmp11;
# 1435
Cyc_Absyndump_dump(_tag_fat("[]",sizeof(char),3U));
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(_tag_fat("@zeroterm",sizeof(char),10U));
goto _LL2E;}case 1: _Tmp11=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1;_Tmp10=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp8)->f2;{struct Cyc_Absyn_Exp*e=_Tmp11;void*zeroterm=_Tmp10;
# 1439
Cyc_Absyndump_dump_char(91);Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char(93);
if(Cyc_Absyn_type2bool(0,zeroterm))Cyc_Absyndump_dump(_tag_fat("@zeroterm",sizeof(char),10U));
goto _LL2E;}case 3: if(*((int*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)==1){_Tmp11=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f1;_TmpF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f2;_Tmp10=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f3;_TmpE=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f4;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f5;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f6;_TmpB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f7;_TmpA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f8;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f9;{struct Cyc_List_List*args=_Tmp11;int c_varargs=_TmpF;struct Cyc_Absyn_VarargInfo*cyc_varargs=_Tmp10;void*effopt=_TmpE;struct Cyc_List_List*effc=_TmpD;struct Cyc_List_List*qb=_TmpC;struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
# 1443
Cyc_Absyndump_dumpfunargs(args,c_varargs,cyc_varargs,effopt,effc,qb,req,ens,thrws);goto _LL2E;}}else{_Tmp11=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1)->f1;{struct Cyc_List_List*sl=_Tmp11;
# 1445
({void(*_Tmp12)(void(*)(struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr)=(void(*)(void(*)(struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr))Cyc_Absyndump_group;_Tmp12;})(Cyc_Absyndump_dump_str,sl,_tag_fat("(",sizeof(char),2U),_tag_fat(")",sizeof(char),2U),_tag_fat(",",sizeof(char),2U));goto _LL2E;}}case 4: _Tmp11=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp8)->f1;_TmpF=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp8)->f3;{struct Cyc_List_List*ts=_Tmp11;int print_kinds=_TmpF;
# 1447
if(print_kinds)Cyc_Absyndump_dumpkindedtvars(ts);else{Cyc_Absyndump_dumptvars(ts);}goto _LL2E;}case 5: _Tmp11=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp8)->f2;{struct Cyc_List_List*atts=_Tmp11;
Cyc_Absyndump_dumpatts(atts);goto _LL2E;}default:
 Cyc_Warn_impos(_tag_fat("dumptms",sizeof(char),8U),_tag_fat(0U,sizeof(void*),0));}_LL2E:;}
# 1451
return;};}}
# 1455
static void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int cp=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _Tmp0=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
{struct _tuple13 _Tmp1=Cyc_Absynpp_to_tms(temp,tq,t);void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;struct Cyc_List_List*tms=_Tmp2;
Cyc_Absyndump_dumptq(tq);
Cyc_Absyndump_dumpntyp(t);
({int _Tmp5=cp;struct Cyc_List_List*_Tmp6=Cyc_List_imp_rev(tms);void(*_Tmp7)(void*)=f;Cyc_Absyndump_dumptms(_Tmp5,_Tmp6,_Tmp7,a);});}}
# 1458
;_pop_region();}
# 1464
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1466
*Cyc_Absyndump_dump_file=f;
for(1;tdl!=0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
Cyc_fprintf(f,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 1472
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 1475
if((int)vd->sc==0)return;{
struct Cyc_Absyn_Exp*init=vd->initializer;
vd->initializer=0;
if((int)vd->sc==2)
Cyc_Absyndump_dump(_tag_fat("extern ",sizeof(char),8U));else{
if(!vd->is_proto){
void*_Tmp5=Cyc_Absyn_compress(vd->type);if(*((int*)_Tmp5)==5){
Cyc_Absyndump_dump(_tag_fat("extern ",sizeof(char),8U));goto _LL17;}else{
goto _LL17;}_LL17:;}}
# 1485
Cyc_Absyndump_dumpvardecl(vd,d->loc,1);
Cyc_Absyndump_dump(_tag_fat("\n",sizeof(char),2U));
vd->initializer=init;
goto _LL0;}}case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 1490
if((int)fd->sc==0)return;
Cyc_Absyndump_dumpscope(fd->sc);{
struct Cyc_Absyn_FnInfo type_info=fd->i;
type_info.attributes=0;{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp5->tag=5,_Tmp5->f1=type_info;_Tmp5;});
({void(*_Tmp5)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=({void(*_Tmp6)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*)=(void(*)(struct Cyc_Absyn_Tqual,void*,void(*)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd;_Tmp6;});struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=t;_Tmp5(_Tmp6,_Tmp7,Cyc_Absyndump_dump_atts_qvar,fd);});
Cyc_Absyndump_dump(_tag_fat(";\n",sizeof(char),3U));
goto _LL0;}}}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 1499
if((int)ad->sc==0)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=ad->impl;
if((int)ad->sc==1)ad->impl=0;
Cyc_Absyndump_dump_aggrdecl(ad);
ad->impl=impl;
Cyc_Absyndump_dump(_tag_fat(";\n",sizeof(char),3U));
goto _LL0;}}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp4;
Cyc_Absyndump_dump_datatypedecl(dd);Cyc_Absyndump_dump(_tag_fat(";\n",sizeof(char),3U));goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
Cyc_Absyndump_dump_enumdecl(ed);Cyc_Absyndump_dump(_tag_fat(";\n",sizeof(char),3U));goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 1509
if(td->defn==0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(_tag_fat("\n",sizeof(char),2U));}
# 1513
goto _LL0;}case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*ds=_Tmp3;
# 1515
Cyc_Absyndump_dump(_tag_fat("namespace ",sizeof(char),11U));
Cyc_Absyndump_dump_str(v);
Cyc_Absyndump_dump(_tag_fat("{\n",sizeof(char),3U));
for(1;ds!=0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(_tag_fat("}\n",sizeof(char),3U));
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*ds=_Tmp4;
# 1523
for(1;ds!=0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds=_Tmp4;
# 1527
Cyc_Absyndump_dump(_tag_fat("extern \"C\" {",sizeof(char),13U));
for(1;ds!=0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}
Cyc_Absyndump_dump(_tag_fat("}\n",sizeof(char),3U));
goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List*ds=_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exs=_Tmp2;struct _tuple11*wc=_Tmp1;
# 1548 "absyndump.cyc"
goto _LL0;}default:
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("bad top-level declaration",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 1553
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(1;ds!=0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}

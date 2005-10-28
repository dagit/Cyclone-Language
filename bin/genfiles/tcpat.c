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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f0;void*f1;};
# 115 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
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
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 379
extern struct Cyc_List_List*Cyc_List_tabulate_c(int,void*(*)(void*,int),void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 65
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 96
extern int Cyc_Set_cardinality(struct Cyc_Set_Set*);
# 99
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 139
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 309
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 322
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 329
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 706 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};
# 890
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 904
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 911
void*Cyc_Absyn_compress(void*);
# 924
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 926
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 929
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 931
extern void*Cyc_Absyn_sint_type;
# 934
void*Cyc_Absyn_gen_float_type(unsigned);
# 940
extern void*Cyc_Absyn_al_qual_type;
# 944
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 946
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_enum_type(struct _tuple2*,struct Cyc_Absyn_Enumdecl*);
# 965
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 981
void*Cyc_Absyn_bounds_one (void);
# 983
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1006
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1071
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1098
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1187
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 73
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 36 "tcutil.h"
int Cyc_Tcutil_is_arithmetic_type(void*);
# 41
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
# 48
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 94
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 98
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 105
int Cyc_Tcutil_coerce_assign(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
# 112
int Cyc_Tcutil_subtype(struct Cyc_List_List*,void*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 135
struct Cyc_List_List*Cyc_Tcutil_rsubst_effconstr(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple13{struct Cyc_List_List*f0;struct _RegionHandle*f1;};struct _tuple14{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 161
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 168
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr);
# 176
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 189
int Cyc_Tcutil_is_noalias_qual(void*,int);
# 197
int Cyc_Tcutil_is_noalias_path(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 202
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*,void*);
# 212
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 223
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 243
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 250
int Cyc_Tcutil_force_type2bool(int,void*);
# 253
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
# 31
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
# 55 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
# 57
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
# 60
extern struct Cyc_Core_Opt Cyc_Kinds_aqko;
# 89 "kinds.h"
void*Cyc_Kinds_compress_kb(void*);
# 93
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 99 "tcenv.h"
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 144
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_aquals_bound(struct Cyc_Tcenv_Tenv*,void*,void*);
# 147
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int,int);
# 150
void Cyc_Tcenv_check_effect_accessible_nodelay(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 47 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 32
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_effconstr_opt;struct Cyc_List_List*patvars;struct Cyc_List_List*aquals_bounds;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;void*f1;int f2;struct _fat_ptr*f3;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct _tuple15{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;switch(*((int*)_Tmp0)){case 6: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1==0){_Tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f5;{int is_tuple=_Tmp5;struct Cyc_List_List*exist_ts=_Tmp4;struct Cyc_List_List*dps=_Tmp3;int dots=_Tmp2;
# 50
if(topt==0){
if(is_tuple && !dots){
struct Cyc_List_List*fields=0;
unsigned n=(unsigned)Cyc_List_length(dps);
{int i=0;for(0;(unsigned)i < n;++ i){
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp6=Cyc_Absyn_tuple_field_designator(i);void*_Tmp7;_Tmp7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp6)->f1;{struct _fat_ptr*n=_Tmp7;
struct Cyc_Absyn_Aggrfield*f;f=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield)),f->name=n,({struct Cyc_Absyn_Tqual _Tmp8=Cyc_Absyn_empty_tqual(0U);f->tq=_Tmp8;}),({void*_Tmp8=Cyc_Absyn_new_evar(& Cyc_Kinds_mko,Cyc_Tcenv_lookup_opt_type_vars(te));f->type=_Tmp8;}),f->width=0,f->attributes=0,f->requires_clause=0;
fields=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=f,_Tmp8->tl=fields;_Tmp8;});}}}
# 59
fields=Cyc_List_imp_rev(fields);{
void*t=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp6->tag=6,_Tmp6->f1=0U,_Tmp6->f2=is_tuple,_Tmp6->f3=fields;_Tmp6;});
({void*_Tmp6=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp7->tag=6,_Tmp7->f1=t,_Tmp7->f2=is_tuple,_Tmp7->f3=exist_ts,_Tmp7->f4=dps,_Tmp7->f5=dots;_Tmp7;});p->r=_Tmp6;});}}else{
# 63
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("cannot determine pattern type",sizeof(char),30U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),1));});}}else{
# 65
void*type=Cyc_Absyn_compress(*topt);
switch(*((int*)type)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)type)->f1)==24){
# 68
({void*_Tmp6=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp7->tag=6,_Tmp7->f1=type,_Tmp7->f2=is_tuple,_Tmp7->f3=exist_ts,_Tmp7->f4=dps,_Tmp7->f5=dots;_Tmp7;});p->r=_Tmp6;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL10;}else{goto _LL15;}case 6:
# 72
({void*_Tmp6=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp7->tag=6,_Tmp7->f1=type,_Tmp7->f2=is_tuple,_Tmp7->f3=exist_ts,_Tmp7->f4=dps,_Tmp7->f5=dots;_Tmp7;});p->r=_Tmp6;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL10;default: _LL15:
# 76
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("pattern expects aggregate type instead of ",sizeof(char),43U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)*topt;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL10;}_LL10:;}
# 80
return;}}else{_Tmp4=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp3=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f5;{void*topt=_Tmp4;int is_tuple=_Tmp5;struct Cyc_List_List**exist_ts=(struct Cyc_List_List**)_Tmp3;struct Cyc_List_List*dps=_Tmp1;int dots=_Tmp2;
# 83
void*_Tmp6=Cyc_Absyn_compress(topt);void*_Tmp7;switch(*((int*)_Tmp6)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownAggr.tag==2){_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp7;
# 85
struct Cyc_Absyn_Aggrdecl*ad=*adp;
if(ad->impl==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("can't destructure an abstract",sizeof(char),30U);_Tmp9;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpA;_TmpA.tag=6,_TmpA.f1=ad;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err2(p->loc,_tag_fat(_TmpA,sizeof(void*),2));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 91
int more_exists=({int _Tmp8=Cyc_List_length(ad->impl->exist_vars);_Tmp8 - Cyc_List_length(*exist_ts);});
if(more_exists < 0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("too many existential type variables in pattern",sizeof(char),47U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp9,sizeof(void*),1));});{
struct Cyc_List_List**ts=exist_ts;
{int n=Cyc_List_length(ad->impl->exist_vars);for(0;n!=0;-- n){
ts=&(*ts)->tl;}}
*ts=0;}}else{
if(more_exists > 0){
# 100
struct Cyc_List_List*new_ts=0;
for(1;more_exists!=0;-- more_exists){
new_ts=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Tvar*_Tmp9=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_TmpA->tag=1,_TmpA->f1=0;_TmpA;}));_Tmp8->hd=_Tmp9;}),_Tmp8->tl=new_ts;_Tmp8;});}
({struct Cyc_List_List*_Tmp8=Cyc_List_imp_append(*exist_ts,new_ts);*exist_ts=_Tmp8;});}}
# 105
return;}}}else{goto _LL1C;}}else{goto _LL1C;}case 6:
# 107
 return;default: _LL1C:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("resolve_pat unknown aggrtype ",sizeof(char),30U);_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)topt;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),2));});};}}case 16: _Tmp4=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
# 111
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcutil_is_const_exp(e)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("non-constant expression in case pattern",sizeof(char),40U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 117
struct _tuple15 _Tmp6=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp7;unsigned _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{unsigned i=_Tmp8;int known=_Tmp7;
if(!known){
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=(struct _fat_ptr)Cyc_Absynpp_exp2string(e);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_err(p->loc,_tag_fat("cannot evaluate expression at compile time\nWe suggest using x && x == %s instead",sizeof(char),81U),_tag_fat(_TmpA,sizeof(void*),1));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}
# 123
({void*_Tmp9=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_TmpA->tag=9,_TmpA->f1=2U,_TmpA->f2=(int)i;_TmpA;});p->r=_Tmp9;});
return;}}}case 14:
# 126
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("resolve_pat UnknownId_p",sizeof(char),24U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({int(*_Tmp8)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp8;})(_tag_fat(_Tmp7,sizeof(void*),1));});case 15:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("resolve_pat UnknownCall_p",sizeof(char),26U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({int(*_Tmp8)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp8;})(_tag_fat(_Tmp7,sizeof(void*),1));});default:
# 129
 return;};}
# 133
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){
struct Cyc_Absyn_Pat*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Pat));_Tmp0->loc=loc,_Tmp0->topt=0,_Tmp0->r=(void*)& Cyc_Absyn_Wild_p_val;return _Tmp0;}struct _tuple16{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 136
static struct _tuple16*Cyc_Tcpat_wild_dlp(unsigned loc){
struct _tuple16*_Tmp0=_cycalloc(sizeof(struct _tuple16));_Tmp0->f0=0,({struct Cyc_Absyn_Pat*_Tmp1=Cyc_Tcpat_wild_pat(loc);_Tmp0->f1=_Tmp1;});return _Tmp0;}
# 140
static struct Cyc_List_List*Cyc_Tcpat_remove_dots(struct Cyc_List_List*earlier,int num_pats,int num_fields,void*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str){
# 142
if(num_pats >= num_fields)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("unnecessary ... in ",sizeof(char),20U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=msg_str;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" pattern",sizeof(char),9U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});{
struct Cyc_List_List*new_ps=0;
{int i=0;for(0;i < num_fields - num_pats;++ i){
new_ps=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1=f(loc);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=new_ps;_Tmp0;});}}
return Cyc_List_imp_append(earlier,new_ps);}}
# 149
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f1;}
# 152
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt==0)return Cyc_Absyn_new_evar(& Cyc_Kinds_mko,({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=s;_Tmp0;}));else{return*topt;}}
# 155
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 159
if(topt==0)
return numt;
if(Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;{
void*_Tmp0=Cyc_Absyn_compress(numt);if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 19:
 goto _LL4;case 20: _LL4:
 return*topt;default: goto _LL5;}else{_LL5:
 return numt;};}}struct _tuple17{struct Cyc_Absyn_Vardecl**f0;struct Cyc_Absyn_Exp*f1;};
# 170
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 172
if(vd!=0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_Tmp0;});}
# 176
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp2=({struct _tuple17*_Tmp3=_cycalloc(sizeof(struct _tuple17));_Tmp3->f0=vd,_Tmp3->f1=e;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*v_result_ptr;_Tmp1;});*v_result_ptr=_Tmp0;});}
# 178
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 180
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=res1.tvars_and_effconstr_opt;_Tmp1=res1.patvars;_Tmp0=res1.aquals_bounds;{struct _tuple1*p1=_Tmp2;struct Cyc_List_List*vs1=_Tmp1;struct Cyc_List_List*aq1=_Tmp0;
void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=res2.tvars_and_effconstr_opt;_Tmp4=res2.patvars;_Tmp3=res2.aquals_bounds;{struct _tuple1*p2=_Tmp5;struct Cyc_List_List*vs2=_Tmp4;struct Cyc_List_List*aq2=_Tmp3;
struct Cyc_List_List*aq=Cyc_List_append(aq1,aq2);
if(p1==0 && p2==0){
struct Cyc_Tcpat_TcPatResult _Tmp6;_Tmp6.tvars_and_effconstr_opt=0,({struct Cyc_List_List*_Tmp7=Cyc_List_append(vs1,vs2);_Tmp6.patvars=_Tmp7;}),_Tmp6.aquals_bounds=aq;return _Tmp6;}
if(p1==0)
p1=({struct _tuple1*_Tmp6=_cycalloc(sizeof(struct _tuple1));_Tmp6->f0=0,_Tmp6->f1=0;_Tmp6;});
if(p2==0)
p2=({struct _tuple1*_Tmp6=_cycalloc(sizeof(struct _tuple1));_Tmp6->f0=0,_Tmp6->f1=0;_Tmp6;});{
# 190
struct Cyc_Tcpat_TcPatResult _Tmp6;({struct _tuple1*_Tmp7=({struct _tuple1*_Tmp8=_cycalloc(sizeof(struct _tuple1));({struct Cyc_List_List*_Tmp9=Cyc_List_append((*p1).f0,(*p2).f0);_Tmp8->f0=_Tmp9;}),({
struct Cyc_List_List*_Tmp9=Cyc_List_append((*p1).f1,(*p2).f1);_Tmp8->f1=_Tmp9;});_Tmp8;});
# 190
_Tmp6.tvars_and_effconstr_opt=_Tmp7;}),({
# 192
struct Cyc_List_List*_Tmp7=Cyc_List_append(vs1,vs2);_Tmp6.patvars=_Tmp7;}),_Tmp6.aquals_bounds=aq;return _Tmp6;}}}}
# 197
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
if(Cyc_Tcutil_is_array_type(t)){
void*_Tmp0=t;void*_Tmp1=rgn_opt==0?Cyc_Absyn_new_evar(& Cyc_Kinds_eko,({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp3=Cyc_Tcenv_lookup_type_vars(te);_Tmp2->v=_Tmp3;});_Tmp2;})): rgn_opt;return Cyc_Tcutil_promote_array(_Tmp0,_Tmp1,Cyc_Absyn_al_qual_type,0);}else{return t;}}struct _tuple18{struct Cyc_Absyn_Tvar*f0;int f1;};
# 204
static struct _tuple18*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
struct _tuple18*_Tmp0=_cycalloc(sizeof(struct _tuple18));_Tmp0->f0=tv,_Tmp0->f1=0;return _Tmp0;}struct _tuple19{struct Cyc_Absyn_Aggrfield*f0;struct Cyc_Absyn_Pat*f1;};struct _tuple20{struct Cyc_Absyn_Tqual f0;void*f1;};
# 208
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 211
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _Tmp0;_Tmp0.tvars_and_effconstr_opt=0,_Tmp0.patvars=0,_Tmp0.aquals_bounds=0;_Tmp0;});
# 216
{void*_Tmp0=p->r;int _Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0:
# 218
 t=({struct Cyc_List_List*_Tmp6=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_Tmp6,topt);});goto _LL0;case 1: _Tmp5=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;struct Cyc_Absyn_Pat*p2=_Tmp4;
# 222
struct _tuple2*_Tmp6=vd->name;struct _fat_ptr _Tmp7;_Tmp7=*_Tmp6->f1;{struct _fat_ptr v=_Tmp7;
if(Cyc_strcmp(v,_tag_fat("true",sizeof(char),5U))==0 || Cyc_strcmp(v,_tag_fat("false",sizeof(char),6U))==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("you probably do not want to use ",sizeof(char),33U);_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=v;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=
_tag_fat(" as a local variable",sizeof(char),21U);_TmpB;});void*_TmpB[3];_TmpB[0]=& _Tmp8,_TmpB[1]=& _Tmp9,_TmpB[2]=& _TmpA;Cyc_Warn_warn2(p->loc,_tag_fat(_TmpB,sizeof(void*),3));});
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=_check_null(p2->topt);
# 230
{void*_Tmp8=Cyc_Absyn_compress(t);if(*((int*)_Tmp8)==4){
# 232
if(rgn_pat==0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("array reference would point into unknown/unallowed region",sizeof(char),58U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_err2(p->loc,_tag_fat(_TmpA,sizeof(void*),1));});
goto _LL28;}else{
# 236
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_mk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("pattern would point to an abstract member",sizeof(char),42U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_err2(p->loc,_tag_fat(_TmpA,sizeof(void*),1));});
goto _LL28;}_LL28:;}
# 240
({struct Cyc_Absyn_Vardecl**_Tmp8=({struct Cyc_Absyn_Vardecl**_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));*_Tmp9=vd;_Tmp9;});struct Cyc_Absyn_Exp*_Tmp9=access_exp;struct Cyc_List_List**_TmpA=& res.patvars;Cyc_Tcpat_set_vd(_Tmp8,_Tmp9,_TmpA,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}}case 2: _Tmp5=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp5;struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 243
struct Cyc_Tcenv_Tenv*te2=({unsigned _Tmp6=p->loc;struct Cyc_Tcenv_Tenv*_Tmp7=te;Cyc_Tcenv_add_type_vars(_Tmp6,_Tmp7,({struct Cyc_Absyn_Tvar*_Tmp8[1];_Tmp8[0]=tv;Cyc_List_list(_tag_fat(_Tmp8,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_effconstr_opt==0)
({struct _tuple1*_Tmp6=({struct _tuple1*_Tmp7=_cycalloc(sizeof(struct _tuple1));_Tmp7->f0=0,_Tmp7->f1=0;_Tmp7;});res.tvars_and_effconstr_opt=_Tmp6;});
({struct Cyc_List_List*_Tmp6=({
struct Cyc_List_List*_Tmp7=(*res.tvars_and_effconstr_opt).f0;Cyc_List_append(_Tmp7,({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp9=({struct _tuple18*_TmpA=_cycalloc(sizeof(struct _tuple18));_TmpA->f0=tv,_TmpA->f1=1;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_Tmp8;}));});
# 246
(*res.tvars_and_effconstr_opt).f0=_Tmp6;});
# 248
({unsigned _Tmp6=p->loc;struct Cyc_Tcenv_Tenv*_Tmp7=te2;struct Cyc_List_List*_Tmp8=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_Tmp6,_Tmp7,_Tmp8,& Cyc_Kinds_mk,1,0,vd->type);});
{void*_Tmp6=vd->type;void*_Tmp7;if(*((int*)_Tmp6)==3){_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1.ptr_atts.eff;{void*eff=_Tmp7;
# 251
void*atv=Cyc_Absyn_var_type(tv);
if(!({void*_Tmp8=eff;Cyc_Unify_unify(_Tmp8,Cyc_Absyn_var_type(tv));}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("Mismatched effect variables in alias pattern:",sizeof(char),46U);_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)eff;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat(", ",sizeof(char),3U);_TmpB;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpB=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpC;_TmpC.tag=2,_TmpC.f1=(void*)atv;_TmpC;});void*_TmpC[4];_TmpC[0]=& _Tmp8,_TmpC[1]=& _Tmp9,_TmpC[2]=& _TmpA,_TmpC[3]=& _TmpB;Cyc_Warn_err2(p->loc,_tag_fat(_TmpC,sizeof(void*),4));});
goto _LL2D;}}else{
# 256
goto _LL2D;}_LL2D:;}
# 260
t=({struct Cyc_List_List*_Tmp6=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_Tmp6,topt);});
{void*_Tmp6=Cyc_Absyn_compress(t);if(*((int*)_Tmp6)==4){
# 263
if(rgn_pat==0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("array reference would point into unknown/unallowed region",sizeof(char),58U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp8,sizeof(void*),1));});
goto _LL32;}else{
# 267
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_mk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("pattern would point to an abstract member",sizeof(char),42U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp8,sizeof(void*),1));});
goto _LL32;}_LL32:;}
# 271
({struct Cyc_Absyn_Vardecl**_Tmp6=({struct Cyc_Absyn_Vardecl**_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));*_Tmp7=vd;_Tmp7;});struct Cyc_Absyn_Exp*_Tmp7=access_exp;struct Cyc_List_List**_Tmp8=& res.patvars;Cyc_Tcpat_set_vd(_Tmp6,_Tmp7,_Tmp8,vd->type);});
goto _LL0;}case 3: _Tmp5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;struct Cyc_Absyn_Pat*p2=_Tmp4;
# 275
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=_check_null(p2->topt);
if(!allow_ref_pat || rgn_pat==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("* pattern would point into an unknown/unallowed region",sizeof(char),55U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL0;}else{
if(({struct Cyc_List_List*_Tmp6=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer_or_aggr(_Tmp6,t);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("* pattern cannot take the address of an alias-free path",sizeof(char),56U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),1));});}{
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.elt_type=t;
pi.ptr_atts.eff=rgn_pat;
pi.ptr_atts.nullable=Cyc_Absyn_false_type;{
void*t2=Cyc_Absyn_pointer_type(pi);
if((unsigned)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);
new_access_exp->topt=t2;}
# 292
({struct Cyc_Absyn_Vardecl**_Tmp6=({struct Cyc_Absyn_Vardecl**_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));*_Tmp7=vd;_Tmp7;});struct Cyc_Absyn_Exp*_Tmp7=new_access_exp;struct Cyc_List_List**_Tmp8=& res.patvars;Cyc_Tcpat_set_vd(_Tmp6,_Tmp7,_Tmp8,t2);});
goto _LL0;}}}case 4: _Tmp5=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp5;struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 298
({struct Cyc_Absyn_Vardecl**_Tmp6=({struct Cyc_Absyn_Vardecl**_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));*_Tmp7=vd;_Tmp7;});struct Cyc_Absyn_Exp*_Tmp7=access_exp;struct Cyc_List_List**_Tmp8=& res.patvars;Cyc_Tcpat_set_vd(_Tmp6,_Tmp7,_Tmp8,vd->type);});
# 302
({unsigned _Tmp6=p->loc;struct Cyc_Tcenv_Tenv*_Tmp7=te;Cyc_Tcenv_add_type_vars(_Tmp6,_Tmp7,({struct Cyc_Absyn_Tvar*_Tmp8[1];_Tmp8[0]=tv;Cyc_List_list(_tag_fat(_Tmp8,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_effconstr_opt==0)
({struct _tuple1*_Tmp6=({struct _tuple1*_Tmp7=_cycalloc(sizeof(struct _tuple1));_Tmp7->f0=0,_Tmp7->f1=0;_Tmp7;});res.tvars_and_effconstr_opt=_Tmp6;});
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple18*_Tmp8=({struct _tuple18*_Tmp9=_cycalloc(sizeof(struct _tuple18));_Tmp9->f0=tv,_Tmp9->f1=0;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=(*res.tvars_and_effconstr_opt).f0;_Tmp7;});
# 305
(*res.tvars_and_effconstr_opt).f0=_Tmp6;});
# 307
t=Cyc_Absyn_uint_type;
goto _LL0;}case 9: if(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1==Cyc_Absyn_Unsigned){
# 310
t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;}else{
t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;}case 10:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 11: _Tmp3=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{int i=_Tmp3;
t=({void**_Tmp6=topt;Cyc_Tcpat_num_type(_Tmp6,Cyc_Absyn_gen_float_type((unsigned)i));});goto _LL0;}case 12: _Tmp5=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp5;
t=({void**_Tmp6=topt;Cyc_Tcpat_num_type(_Tmp6,Cyc_Absyn_enum_type(ed->name,ed));});goto _LL0;}case 13: _Tmp5=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{void*tenum=_Tmp5;
t=Cyc_Tcpat_num_type(topt,tenum);goto _LL0;}case 8:
# 317
 if(topt!=0){
void*_Tmp6=Cyc_Absyn_compress(*topt);if(*((int*)_Tmp6)==3){
t=*topt;goto tcpat_end;}else{
goto _LL37;}_LL37:;}{
# 322
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.ptr_atts.nullable=Cyc_Absyn_true_type;
t=Cyc_Absyn_pointer_type(pi);
goto _LL0;}case 5: _Tmp5=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp5;
# 330
void*inner_typ=Cyc_Absyn_void_type;
void**inner_topt=0;
int elt_const=0;
if(topt!=0){
void*_Tmp6=Cyc_Absyn_compress(*topt);struct Cyc_Absyn_Tqual _Tmp7;void*_Tmp8;if(*((int*)_Tmp6)==3){_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1.elt_tq;{void*elt_typ=_Tmp8;struct Cyc_Absyn_Tqual elt_tq=_Tmp7;
# 336
inner_typ=elt_typ;
inner_topt=& inner_typ;
elt_const=elt_tq.real_const;
goto _LL3C;}}else{
goto _LL3C;}_LL3C:;}{
# 345
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);
res=({struct Cyc_Tcpat_TcPatResult _Tmp6=res;Cyc_Tcpat_combine_results(_Tmp6,Cyc_Tcpat_tcPatRec(te,p2,inner_topt,ptr_rgn,1,new_access_exp));});
# 354
{void*_Tmp6=Cyc_Absyn_compress(_check_null(p2->topt));void*_Tmp7;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp6)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)==23){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownDatatypefield.tag==2){_Tmp9=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownDatatypefield.val.f0;_Tmp8=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownDatatypefield.val.f1;_Tmp7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp9;struct Cyc_Absyn_Datatypefield*tuf=_Tmp8;struct Cyc_List_List*targs=_Tmp7;
# 358
{void*_TmpA=Cyc_Absyn_compress(inner_typ);if(*((int*)_TmpA)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpA)->f1)==23)
# 360
goto DONT_PROMOTE;else{goto _LL49;}}else{_LL49:
 goto _LL46;}_LL46:;}{
# 364
void*new_type=({union Cyc_Absyn_DatatypeInfo _TmpA=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));*_TmpB=tud;_TmpB;}));Cyc_Absyn_datatype_type(_TmpA,targs);});
# 366
p2->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct));_TmpA->tag=3,_TmpA->f1.elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _TmpB=Cyc_Absyn_const_tqual(0U);_TmpA->f1.elt_tq=_TmpB;}):({
struct Cyc_Absyn_Tqual _TmpB=Cyc_Absyn_empty_tqual(0U);_TmpA->f1.elt_tq=_TmpB;}),
_TmpA->f1.ptr_atts.eff=ptr_rgn,({void*_TmpB=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));_TmpA->f1.ptr_atts.nullable=_TmpB;}),({
void*_TmpB=Cyc_Absyn_bounds_one();_TmpA->f1.ptr_atts.bounds=_TmpB;}),_TmpA->f1.ptr_atts.zero_term=Cyc_Absyn_false_type,_TmpA->f1.ptr_atts.ptrloc=0,({
void*_TmpB=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));_TmpA->f1.ptr_atts.autoreleased=_TmpB;}),({
void*_TmpB=Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,Cyc_Tcenv_lookup_opt_type_vars(te));_TmpA->f1.ptr_atts.aqual=_TmpB;});_TmpA;});
goto _LL41;}}}else{goto _LL44;}}else{goto _LL44;}}else{_LL44:
# 376
 DONT_PROMOTE: {
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.elt_type=_check_null(p2->topt);
({struct Cyc_Absyn_Tqual _TmpA=elt_const?Cyc_Absyn_const_tqual(0U): Cyc_Absyn_empty_tqual(0U);pi.elt_tq=_TmpA;});
pi.ptr_atts.eff=ptr_rgn;
t=Cyc_Absyn_pointer_type(pi);}}_LL41:;}
# 383
if((unsigned)new_access_exp)new_access_exp->topt=p2->topt;
goto _LL0;}}case 6: _Tmp5=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp2=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f5;{void*atype=_Tmp5;int is_tuple=_Tmp3;struct Cyc_List_List*exist_ts=_Tmp4;struct Cyc_List_List**dps_p=_Tmp2;int dots=_Tmp1;
# 387
struct Cyc_List_List*dps=*dps_p;
if(atype==0){
# 390
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL0;}
# 393
t=atype;
{void*_Tmp6=Cyc_Absyn_compress(t);int _Tmp7;enum Cyc_Absyn_AggrKind _Tmp8;void*_Tmp9;union Cyc_Absyn_AggrInfo _TmpA;switch(*((int*)_Tmp6)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownAggr.tag==2){_TmpA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1;_Tmp9=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)->f1.KnownAggr.val;{union Cyc_Absyn_AggrInfo ainfo=_TmpA;struct Cyc_Absyn_Aggrdecl*ad=_Tmp9;
# 396
if(ad->impl==0){
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("can't destructure abstract ",sizeof(char),28U);_TmpC;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpD;_TmpD.tag=6,_TmpD.f1=ad;_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;Cyc_Warn_err2(p->loc,_tag_fat(_TmpD,sizeof(void*),2));});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL4B;}
# 403
if((int)ad->kind==1 && ad->impl->tagged)
allow_ref_pat=0;
if(exist_ts!=0){
# 409
if(topt==0 || Cyc_Tcutil_type_kind(*topt)!=& Cyc_Kinds_ak)
allow_ref_pat=0;}
# 412
{struct _RegionHandle _TmpB=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _TmpB;_push_region(rgn);
# 414
{struct Cyc_List_List*var_tvs=0;
struct Cyc_List_List*u=_check_null(ad->impl)->exist_vars;
{struct Cyc_List_List*t=exist_ts;for(0;t!=0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)_check_null(u)->hd;
u=u->tl;{
void*kb1=Cyc_Kinds_compress_kb(tv->kind);
void*kb2=Cyc_Kinds_compress_kb(uv->kind);
struct Cyc_Absyn_Kind*k2;
{void*_TmpC;switch(*((int*)kb2)){case 2: _TmpC=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb2)->f2;{struct Cyc_Absyn_Kind*k=_TmpC;
_TmpC=k;goto _LL56;}case 0: _TmpC=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)kb2)->f1;_LL56: {struct Cyc_Absyn_Kind*k=_TmpC;
k2=k;goto _LL52;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("unconstrained existential tyvar in aggregate",sizeof(char),45U);_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;({int(*_TmpF)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpF;})(_tag_fat(_TmpE,sizeof(void*),1));});}_LL52:;}
# 428
{void*_TmpC;void*_TmpD;switch(*((int*)kb1)){case 0: _TmpD=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)kb1)->f1;{struct Cyc_Absyn_Kind*k1=_TmpD;
# 431
if(!Cyc_Kinds_kind_leq(k2,k1))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("type variable ",sizeof(char),15U);_TmpF;});struct Cyc_Warn_Tvar_Warn_Warg_struct _TmpF=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp10;_Tmp10.tag=7,_Tmp10.f1=tv;_Tmp10;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11;_Tmp11.tag=0,_Tmp11.f1=_tag_fat(" has kind ",sizeof(char),11U);_Tmp11;});struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_KindBound_Warn_Warg_struct _Tmp12;_Tmp12.tag=8,_Tmp12.f1=(void*)kb1;_Tmp12;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=
_tag_fat(" but must have at least kind ",sizeof(char),30U);_Tmp13;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp13=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp14;_Tmp14.tag=9,_Tmp14.f1=k2;_Tmp14;});void*_Tmp14[6];_Tmp14[0]=& _TmpE,_Tmp14[1]=& _TmpF,_Tmp14[2]=& _Tmp10,_Tmp14[3]=& _Tmp11,_Tmp14[4]=& _Tmp12,_Tmp14[5]=& _Tmp13;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp14,sizeof(void*),6));});
goto _LL59;}case 2: _TmpD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb1)->f1;_TmpC=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb1)->f2;{struct Cyc_Core_Opt**f=_TmpD;struct Cyc_Absyn_Kind*k=_TmpC;
_TmpD=f;goto _LL5F;}default: _TmpD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)kb1)->f1;_LL5F: {struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_TmpD;
({struct Cyc_Core_Opt*_TmpE=({struct Cyc_Core_Opt*_TmpF=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpF->v=kb2;_TmpF;});*f=_TmpE;});goto _LL59;}}_LL59:;}{
# 438
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_TmpC->tag=2,_TmpC->f1=tv;_TmpC;});
var_tvs=({struct Cyc_List_List*_TmpC=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_TmpC->hd=vartype,_TmpC->tl=var_tvs;_TmpC;});}}}}
# 452 "tcpat.cyc"
var_tvs=Cyc_List_imp_rev(var_tvs);{
# 454
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,exist_ts);
# 456
struct _tuple13 env=({struct _tuple13 _TmpC;({struct Cyc_List_List*_TmpD=Cyc_Tcenv_lookup_type_vars(te2);_TmpC.f0=_TmpD;}),_TmpC.f1=rgn;_TmpC;});
struct Cyc_List_List*all_inst=({struct Cyc_List_List*(*_TmpC)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c;_TmpC;})(rgn,Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);
struct Cyc_List_List*exist_inst=Cyc_List_rzip(rgn,rgn,_check_null(ad->impl)->exist_vars,var_tvs);
struct Cyc_List_List*all_typs=({struct Cyc_List_List*(*_TmpC)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;_TmpC;})(({void*(*_TmpC)(struct _tuple14*)=(void*(*)(struct _tuple14*))Cyc_Core_snd;_TmpC;}),all_inst);
struct Cyc_List_List*exist_typs=({struct Cyc_List_List*(*_TmpC)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;_TmpC;})(({void*(*_TmpC)(struct _tuple14*)=(void*(*)(struct _tuple14*))Cyc_Core_snd;_TmpC;}),exist_inst);
struct Cyc_List_List*inst=Cyc_List_rappend(rgn,all_inst,exist_inst);
# 463
if(exist_ts!=0 || _check_null(ad->impl)->effconstr!=0){
if(res.tvars_and_effconstr_opt==0)
({struct _tuple1*_TmpC=({struct _tuple1*_TmpD=_cycalloc(sizeof(struct _tuple1));_TmpD->f0=0,_TmpD->f1=0;_TmpD;});res.tvars_and_effconstr_opt=_TmpC;});
({struct Cyc_List_List*_TmpC=({
struct Cyc_List_List*_TmpD=(*res.tvars_and_effconstr_opt).f0;Cyc_List_append(_TmpD,({struct Cyc_List_List*(*_TmpE)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;_TmpE;})(Cyc_Tcpat_add_false,exist_ts));});
# 466
(*res.tvars_and_effconstr_opt).f0=_TmpC;});{
# 470
struct Cyc_List_List*ec_inst=Cyc_Tcutil_rsubst_effconstr(rgn,inst,_check_null(ad->impl)->effconstr);
# 472
({struct Cyc_List_List*_TmpC=
Cyc_List_append((*res.tvars_and_effconstr_opt).f1,ec_inst);
# 472
(*res.tvars_and_effconstr_opt).f1=_TmpC;});{
# 474
struct Cyc_List_List*aqb_inst=0;
{struct Cyc_List_List*aqb=_check_null(ad->impl)->qual_bnd;for(0;aqb!=0;aqb=aqb->tl){
struct _tuple0 _TmpC=({struct _tuple0 _TmpD;({void*_TmpE=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)aqb->hd)).f0);_TmpD.f0=_TmpE;}),({
void*_TmpE=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)aqb->hd)).f1);_TmpD.f1=_TmpE;});_TmpD;});
# 476
void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*tv=_TmpE;void*b=_TmpD;
# 478
aqb_inst=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp10=({struct _tuple0*_Tmp11=_cycalloc(sizeof(struct _tuple0));_Tmp11->f0=tv,_Tmp11->f1=b;_Tmp11;});_TmpF->hd=_Tmp10;}),_TmpF->tl=aqb_inst;_TmpF;});}}}
# 480
aqb_inst=Cyc_List_imp_rev(aqb_inst);
res.aquals_bounds=aqb_inst;}}}
# 485
t=({union Cyc_Absyn_AggrInfo _TmpC=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_TmpD=ad;_TmpD;}));Cyc_Absyn_aggr_type(_TmpC,all_typs);});
if(dots &&(int)ad->kind==1)
Cyc_Warn_warn(p->loc,_tag_fat("`...' pattern not allowed in union pattern",sizeof(char),43U),_tag_fat(0U,sizeof(void*),0));else{
if(dots){
({struct Cyc_List_List*_TmpC=({struct Cyc_List_List*(*_TmpD)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=({struct Cyc_List_List*(*_TmpE)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_TmpE;});struct Cyc_List_List*_TmpE=dps;int _TmpF=Cyc_List_length(dps);int _Tmp10=Cyc_List_length(_check_null(ad->impl)->fields);unsigned _Tmp11=p->loc;_TmpD(_TmpE,_TmpF,_Tmp10,Cyc_Tcpat_wild_dlp,_Tmp11,
_tag_fat("struct",sizeof(char),7U));});
# 489
*dps_p=_TmpC;});
# 491
dps=*dps_p;}}{
# 493
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(rgn,p->loc,dps,ad->kind,_check_null(ad->impl)->fields);
for(1;fields!=0;fields=fields->tl){
struct _tuple19*_TmpC=(struct _tuple19*)fields->hd;void*_TmpD;void*_TmpE;_TmpE=_TmpC->f0;_TmpD=_TmpC->f1;{struct Cyc_Absyn_Aggrfield*field=_TmpE;struct Cyc_Absyn_Pat*pat=_TmpD;
void*inst_fieldtyp=Cyc_Tcutil_rsubstitute(rgn,inst,field->type);
# 499
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,field->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _TmpF=res;Cyc_Tcpat_combine_results(_TmpF,Cyc_Tcpat_tcPatRec(te2,pat,& inst_fieldtyp,rgn_pat,allow_ref_pat,new_access_exp));});
# 507
if(!Cyc_Unify_unify(_check_null(pat->topt),inst_fieldtyp))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat("field ",sizeof(char),7U);_Tmp10;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11;_Tmp11.tag=0,_Tmp11.f1=*field->name;_Tmp11;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=_tag_fat(" of ",sizeof(char),5U);_Tmp12;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp13;_Tmp13.tag=6,_Tmp13.f1=ad;_Tmp13;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp14;_Tmp14.tag=0,_Tmp14.f1=
_tag_fat(" pattern expects type ",sizeof(char),23U);_Tmp14;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp14=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp15;_Tmp15.tag=2,_Tmp15.f1=(void*)inst_fieldtyp;_Tmp15;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp16;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp17;_Tmp17.tag=2,_Tmp17.f1=(void*)
_check_null(pat->topt);_Tmp17;});void*_Tmp17[8];_Tmp17[0]=& _TmpF,_Tmp17[1]=& _Tmp10,_Tmp17[2]=& _Tmp11,_Tmp17[3]=& _Tmp12,_Tmp17[4]=& _Tmp13,_Tmp17[5]=& _Tmp14,_Tmp17[6]=& _Tmp15,_Tmp17[7]=& _Tmp16;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp17,sizeof(void*),8));});
if((unsigned)new_access_exp)new_access_exp->topt=pat->topt;}}}}}
# 414 "tcpat.cyc"
;_pop_region();}
# 514 "tcpat.cyc"
goto _LL4B;}}else{goto _LL50;}}else{goto _LL50;}case 6: _Tmp8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp6)->f1;_Tmp7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp6)->f2;_Tmp9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp6)->f3;{enum Cyc_Absyn_AggrKind ak=_Tmp8;int is_tuple=_Tmp7;struct Cyc_List_List*afs=_Tmp9;
# 517
if(exist_ts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("bad existentially-bound type variables",sizeof(char),39U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_Warn_err2(p->loc,_tag_fat(_TmpC,sizeof(void*),1));});
if(dots &&(int)ak==1)
Cyc_Warn_warn(p->loc,_tag_fat("`...' patterns not allowed in union pattern",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));else{
if(dots){
({struct Cyc_List_List*_TmpB=({struct Cyc_List_List*(*_TmpC)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=({struct Cyc_List_List*(*_TmpD)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_TmpD;});struct Cyc_List_List*_TmpD=dps;int _TmpE=Cyc_List_length(dps);int _TmpF=Cyc_List_length(afs);unsigned _Tmp10=p->loc;_TmpC(_TmpD,_TmpE,_TmpF,Cyc_Tcpat_wild_dlp,_Tmp10,
is_tuple?_tag_fat("tuple",sizeof(char),6U): _tag_fat("struct",sizeof(char),7U));});
# 522
*dps_p=_TmpB;});
# 524
dps=*dps_p;}}
# 526
{struct _RegionHandle _TmpB=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _TmpB;_push_region(rgn);
{struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(rgn,p->loc,dps,ak,afs);
for(1;fields!=0;fields=fields->tl){
struct _tuple19*_TmpC=(struct _tuple19*)fields->hd;void*_TmpD;void*_TmpE;_TmpE=_TmpC->f0;_TmpD=_TmpC->f1;{struct Cyc_Absyn_Aggrfield*field=_TmpE;struct Cyc_Absyn_Pat*pat=_TmpD;
void*fieldtyp=field->type;
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,field->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _TmpF=res;Cyc_Tcpat_combine_results(_TmpF,Cyc_Tcpat_tcPatRec(te,pat,& fieldtyp,rgn_pat,allow_ref_pat,new_access_exp));});
# 540
if(!Cyc_Unify_unify(_check_null(pat->topt),fieldtyp))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat("field ",sizeof(char),7U);_Tmp10;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp11;_Tmp11.tag=0,_Tmp11.f1=*field->name;_Tmp11;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=
_tag_fat(" pattern expects type ",sizeof(char),23U);_Tmp12;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp13;_Tmp13.tag=2,_Tmp13.f1=(void*)fieldtyp;_Tmp13;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp14;_Tmp14.tag=0,_Tmp14.f1=_tag_fat(" != ",sizeof(char),5U);_Tmp14;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp14=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp15;_Tmp15.tag=2,_Tmp15.f1=(void*)
_check_null(pat->topt);_Tmp15;});void*_Tmp15[6];_Tmp15[0]=& _TmpF,_Tmp15[1]=& _Tmp10,_Tmp15[2]=& _Tmp11,_Tmp15[3]=& _Tmp12,_Tmp15[4]=& _Tmp13,_Tmp15[5]=& _Tmp14;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp15,sizeof(void*),6));});
if((unsigned)new_access_exp)new_access_exp->topt=pat->topt;}}}
# 527
;_pop_region();}
# 547
goto _LL4B;}default: _LL50:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("bad type in tcpat",sizeof(char),18U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({int(*_TmpD)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpD;})(_tag_fat(_TmpC,sizeof(void*),1));});}_LL4B:;}
# 550
goto _LL0;}case 7: _Tmp5=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp2=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp5;struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;struct Cyc_List_List**ps_p=(struct Cyc_List_List**)_Tmp2;int dots=_Tmp3;
# 553
struct Cyc_List_List*ps=*ps_p;
struct Cyc_List_List*tqts=tuf->typs;
# 556
struct _tuple13 env=({struct _tuple13 _Tmp6;({struct Cyc_List_List*_Tmp7=Cyc_Tcenv_lookup_type_vars(te);_Tmp6.f0=_Tmp7;}),_Tmp6.f1=Cyc_Core_heap_region;_Tmp6;});
struct Cyc_List_List*inst=({struct Cyc_List_List*(*_Tmp6)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp6;})(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);
struct Cyc_List_List*all_typs=({struct Cyc_List_List*(*_Tmp6)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;_Tmp6;})(({void*(*_Tmp6)(struct _tuple14*)=(void*(*)(struct _tuple14*))Cyc_Core_snd;_Tmp6;}),inst);
t=({union Cyc_Absyn_DatatypeFieldInfo _Tmp6=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_Tmp6,all_typs);});
if(dots){
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*(*_Tmp7)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr)=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_Tmp8;});struct Cyc_List_List*_Tmp8=ps;int _Tmp9=Cyc_List_length(ps);int _TmpA=Cyc_List_length(tqts);unsigned _TmpB=p->loc;_Tmp7(_Tmp8,_Tmp9,_TmpA,Cyc_Tcpat_wild_pat,_TmpB,
_tag_fat("datatype field",sizeof(char),15U));});
# 561
*ps_p=_Tmp6;});
# 563
ps=*ps_p;}
# 565
for(1;ps!=0 && tqts!=0;(ps=ps->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)ps->hd;
# 569
void*field_typ=Cyc_Tcutil_substitute(inst,(*((struct _tuple20*)tqts->hd)).f1);
# 572
if((unsigned)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
res=({struct Cyc_Tcpat_TcPatResult _Tmp6=res;Cyc_Tcpat_combine_results(_Tmp6,Cyc_Tcpat_tcPatRec(te,p2,& field_typ,rgn_pat,allow_ref_pat,0));});
# 579
if(!Cyc_Unify_unify(_check_null(p2->topt),field_typ))
({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=tuf->name;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat(" expects argument type ",sizeof(char),24U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)field_typ;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=
_tag_fat(" not ",sizeof(char),6U);_TmpA;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpB;_TmpB.tag=2,_TmpB.f1=(void*)_check_null(p2->topt);_TmpB;});void*_TmpB[5];_TmpB[0]=& _Tmp6,_TmpB[1]=& _Tmp7,_TmpB[2]=& _Tmp8,_TmpB[3]=& _Tmp9,_TmpB[4]=& _TmpA;Cyc_Warn_err2(p2->loc,_tag_fat(_TmpB,sizeof(void*),5));});}
# 583
if(ps!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("too many arguments for datatype constructor ",sizeof(char),45U);_Tmp7;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=tuf->name;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp8,sizeof(void*),2));});
if(tqts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("too few arguments for datatype constructor ",sizeof(char),44U);_Tmp7;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=tuf->name;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}case 14:
# 589
 goto _LL22;case 16: _LL22:
 goto _LL24;default: _LL24:
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL0;}_LL0:;}
# 593
tcpat_end:
 p->topt=t;
return res;}}
# 598
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 600
struct Cyc_Tcpat_TcPatResult ans=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 602
struct _tuple1 _Tmp0=Cyc_List_split(ans.patvars);void*_Tmp1;_Tmp1=_Tmp0.f0;{struct Cyc_List_List*vs1=_Tmp1;
struct Cyc_List_List*vs=0;
{struct Cyc_List_List*x=vs1;for(0;x!=0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd!=0)vs=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=*((struct Cyc_Absyn_Vardecl**)x->hd),_Tmp2->tl=vs;_Tmp2;});}}
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*(*_Tmp3)(struct _fat_ptr*(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_Tcpat_get_name,vs);unsigned _Tmp3=p->loc;Cyc_Tcutil_check_unique_vars(_Tmp2,_Tmp3,
_tag_fat("pattern contains a repeated variable",sizeof(char),37U));});
# 611
{struct Cyc_List_List*x=ans.patvars;for(0;x!=0;x=x->tl){
struct _tuple17*_Tmp2=(struct _tuple17*)x->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=(struct Cyc_Absyn_Exp**)& _Tmp2->f1;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp4;struct Cyc_Absyn_Exp**expopt=(struct Cyc_Absyn_Exp**)_Tmp3;
if(*expopt!=0 &&*expopt!=pat_var_exp)
({struct Cyc_Absyn_Exp*_Tmp5=Cyc_Tcutil_deep_copy_exp(1,*expopt);*expopt=_Tmp5;});}}}
# 616
return ans;}}
# 622
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 625
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(old_type);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(new_type);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_Absyn_PtrInfo _Tmp1;struct Cyc_Absyn_PtrInfo _Tmp2;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_PtrInfo pold=_Tmp2;struct Cyc_Absyn_PtrInfo pnew=_Tmp1;
# 627
struct Cyc_Absyn_PointerType_Absyn_Type_struct*ptry;ptry=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct)),ptry->tag=3,ptry->f1.elt_type=pold.elt_type,ptry->f1.elt_tq=pnew.elt_tq,
ptry->f1.ptr_atts.eff=pold.ptr_atts.eff,ptry->f1.ptr_atts.nullable=pnew.ptr_atts.nullable,ptry->f1.ptr_atts.bounds=pnew.ptr_atts.bounds,ptry->f1.ptr_atts.zero_term=pnew.ptr_atts.zero_term,ptry->f1.ptr_atts.ptrloc=pold.ptr_atts.ptrloc,ptry->f1.ptr_atts.autoreleased=pnew.ptr_atts.autoreleased,ptry->f1.ptr_atts.aqual=pold.ptr_atts.aqual;
# 635
return Cyc_Tcutil_subtype(assump,(void*)ptry,new_type)&&({
struct Cyc_List_List*_Tmp3=Cyc_Tcenv_curr_aquals_bounds(tenv);struct Cyc_Absyn_Exp*_Tmp4=initializer;Cyc_Tcutil_coerce_assign(_Tmp3,_Tmp4,(void*)ptry);});}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 645
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 648
struct Cyc_List_List*assump=0;
{struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(old_type);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(new_type);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.eff;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.aqual;_Tmp1=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.aqual;{void*oldrgn=_Tmp3;void*aq_old=_Tmp2;void**aq_new=(void**)_Tmp1;
# 651
assump=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=oldrgn,({void*_Tmp7=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp8->tag=2,_Tmp8->f1=tv;_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=0;_Tmp4;});
{void*_Tmp4=*aq_new;void*_Tmp5;if(*((int*)_Tmp4)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==17){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2)->hd)==1){_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f2->tl;{struct Cyc_List_List*bnd=_Tmp5;
# 654
*aq_new=Cyc_Absyn_al_qual_type;
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 658
 goto _LL5;}_LL5:;}
# 660
goto _LL0;}}else{goto _LL3;}}else{_LL3:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("alias requires pointer type",sizeof(char),28U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}_LL0:;}
# 664
if(Cyc_Tcutil_subtype(assump,old_type,new_type)){
# 681 "tcpat.cyc"
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(old_type);_Tmp1.f0=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(new_type);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_Absyn_PtrInfo _Tmp1;struct Cyc_Absyn_PtrInfo _Tmp2;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_PtrInfo pold=_Tmp2;struct Cyc_Absyn_PtrInfo pnew=_Tmp1;
goto _LLA;}}else{goto _LLD;}}else{_LLD:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("alias requires pointer type",sizeof(char),28U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),1));});goto _LLA;}_LLA:;}else{
# 686
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("cannot alias value of type ",sizeof(char),28U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)old_type;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" to type ",sizeof(char),10U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)new_type;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});}}
# 692
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 694
void*_Tmp0=p->r;void*_Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 5: _Tmp4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp4;
# 696
void*_Tmp5=_check_null(p->topt);void*_Tmp6;void*_Tmp7;if(*((int*)_Tmp5)==3){_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp5)->f1.ptr_atts.eff;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp5)->f1.ptr_atts.aqual;{void*rt=_Tmp7;void*aq=_Tmp6;
# 698
Cyc_Tcenv_check_effect_accessible_nodelay(te,p->loc,rt);
({struct Cyc_Tcenv_Tenv*_Tmp8=te;struct Cyc_Absyn_Pat*_Tmp9=p2;int _TmpA=Cyc_Tcutil_is_noalias_qual(aq,0);Cyc_Tcpat_check_pat_regions_rec(_Tmp8,_Tmp9,_TmpA,patvars);});
return;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("check_pat_regions: bad pointer type",sizeof(char),36U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpA;})(_tag_fat(_Tmp9,sizeof(void*),1));});};}case 6: _Tmp4=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{void*ai=_Tmp4;int is_tuple=_Tmp3;struct Cyc_List_List*exist_ts=_Tmp2;struct Cyc_List_List*dps=_Tmp1;
# 704
for(1;dps!=0;dps=dps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple16*)dps->hd)).f1,did_noalias_deref,patvars);}
return;}case 7: _Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_List_List*ps=_Tmp4;
# 709
did_noalias_deref=0;
for(1;ps!=0;ps=ps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)ps->hd,did_noalias_deref,patvars);}
return;}case 3: _Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 714
{struct Cyc_List_List*x=patvars;for(0;x!=0;x=x->tl){
struct _tuple17*_Tmp5=(struct _tuple17*)x->hd;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5->f0;_Tmp6=_Tmp5->f1;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp7;struct Cyc_Absyn_Exp*eopt=_Tmp6;
# 720
if((vdopt!=0 &&*vdopt==vd)&& eopt!=0){
{void*_Tmp8=eopt->r;void*_Tmp9;if(*((int*)_Tmp8)==15){_Tmp9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp8)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 723
if(({struct Cyc_List_List*_TmpA=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer_or_aggr(_TmpA,_check_null(e->topt));}))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("reference pattern not allowed on alias-free pointers",sizeof(char),53U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(p->loc,_tag_fat(_TmpB,sizeof(void*),1));});
goto _LL17;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("check_pat_regions: bad reference access exp",sizeof(char),44U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({int(*_TmpC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpC;})(_tag_fat(_TmpB,sizeof(void*),1));});}_LL17:;}
# 728
break;}}}}
# 731
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 1: _Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 734
{void*_Tmp5=p->topt;if(_Tmp5!=0){if(*((int*)_Tmp5)==4){
# 736
if(!did_noalias_deref)
goto _LL1C;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("pattern to array would create alias of no-alias pointer",sizeof(char),56U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
return;}else{goto _LL1F;}}else{_LL1F:
 goto _LL1C;}_LL1C:;}
# 742
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 2: _Tmp4=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 745
{struct Cyc_List_List*x=patvars;for(0;x!=0;x=x->tl){
struct _tuple17*_Tmp5=(struct _tuple17*)x->hd;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5->f0;_Tmp6=_Tmp5->f1;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp7;struct Cyc_Absyn_Exp*eopt=_Tmp6;
# 749
if(vdopt!=0 &&*vdopt==vd){
if(eopt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("cannot alias pattern expression in datatype",sizeof(char),44U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp9,sizeof(void*),1));});else{
# 753
struct Cyc_Tcenv_Tenv*te2=({unsigned _Tmp8=p->loc;struct Cyc_Tcenv_Tenv*_Tmp9=te;Cyc_Tcenv_add_type_vars(_Tmp8,_Tmp9,({struct Cyc_Absyn_Tvar*_TmpA[1];_TmpA[0]=tv;Cyc_List_list(_tag_fat(_TmpA,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
te2=({struct Cyc_Tcenv_Tenv*_Tmp8=te2;Cyc_Tcenv_add_region(_Tmp8,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp9->tag=2,_Tmp9->f1=tv;_Tmp9;}),0,0);});
# 756
Cyc_Tcpat_check_alias_coercion(te2,p->loc,_check_null(eopt->topt),tv,vd->type,eopt);}
# 758
break;}}}}
# 761
goto _LL0;}default:
 return;}_LL0:;}
# 777 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_Tcpat_TcPatResult*pat_res){
struct Cyc_List_List*patvars=pat_res->patvars;
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
# 781
void*_Tmp0;_Tmp0=pat_res->aquals_bounds;{struct Cyc_List_List*aqb=_Tmp0;
for(1;aqb!=0;aqb=aqb->tl){
struct _tuple0*_Tmp1=(struct _tuple0*)aqb->hd;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{void*tv=_Tmp3;void*b=_Tmp2;
void*_Tmp4=Cyc_Absyn_compress(tv);if(*((int*)_Tmp4)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==15){
# 786
te=Cyc_Tcenv_add_aquals_bound(te,tv,b);
goto _LL6;}else{goto _LL9;}}else{_LL9:
# 789
 goto _LL6;}_LL6:;}}{
# 792
struct Cyc_List_List*x=patvars;for(0;x!=0;x=x->tl){
struct _tuple17*_Tmp1=(struct _tuple17*)x->hd;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp3;struct Cyc_Absyn_Exp*eopt=_Tmp2;
if(eopt!=0){
struct Cyc_Absyn_Exp*e=eopt;
if(({struct Cyc_List_List*_Tmp4=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer_or_aggr(_Tmp4,_check_null(e->topt));})&& !({
struct Cyc_List_List*_Tmp4=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_path(_Tmp4,e);})){
# 799
if(vdopt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("pattern dereferences an alias-free-pointer ",sizeof(char),44U);_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=
_tag_fat("from a non-unique path",sizeof(char),23U);_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 803
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("pattern for variable ",sizeof(char),22U);_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(*vdopt)->name;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=
_tag_fat(" dereferences an alias-free-pointer from a non-unique path",sizeof(char),59U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),3));});}}}}}}}}}
# 850 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 859
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 873
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){union Cyc_Tcpat_Name_value _Tmp0;_Tmp0.Name_v.tag=1U,_Tmp0.Name_v.val=s;return _Tmp0;}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _Tmp0;_Tmp0.Int_v.tag=2U,_Tmp0.Int_v.val=i;return _Tmp0;}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 890
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _Tmp0=c1->name;int _Tmp1;struct _fat_ptr _Tmp2;if(_Tmp0.Name_v.tag==1){_Tmp2=_Tmp0.Name_v.val;{struct _fat_ptr n1=_Tmp2;
# 893
union Cyc_Tcpat_Name_value _Tmp3=c2->name;struct _fat_ptr _Tmp4;if(_Tmp3.Name_v.tag==1){_Tmp4=_Tmp3.Name_v.val;{struct _fat_ptr n2=_Tmp4;
return Cyc_strcmp(n1,n2);}}else{
return -1;};}}else{_Tmp1=_Tmp0.Int_v.val;{int i1=_Tmp1;
# 898
union Cyc_Tcpat_Name_value _Tmp3=c2->name;int _Tmp4;if(_Tmp3.Name_v.tag==1)
return 1;else{_Tmp4=_Tmp3.Int_v.val;{int i2=_Tmp4;
return i1 - i2;}};}};}
# 906
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set (void){
return({struct Cyc_Set_Set*(*_Tmp0)(struct _RegionHandle*,int(*)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*))=(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty;_Tmp0;})(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 910
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 914
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned ans=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(td->fields)->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)_check_null(fs)->hd)->name)!=0){
++ ans;
fs=fs->tl;
# 918
1U;}
# 921
return ans;}
# 924
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*fields=_check_null(ad->impl)->fields;for(0;fields!=0;(fields=fields->tl,++ i)){
if(Cyc_strcmp(*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,*f)==0)return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("get_member_offset ",sizeof(char),19U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=*f;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(" failed",sizeof(char),8U);_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),3));});}
# 931
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
void*_Tmp0;if(pw.where_clause.tag==2){_Tmp0=pw.where_clause.val;{struct Cyc_Absyn_Exp*e=_Tmp0;
return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct));_Tmp1->tag=0,_Tmp1->f1=e;_Tmp1;});}}else{_Tmp0=pw.pattern.val;{struct Cyc_Absyn_Pat*p=_Tmp0;
# 935
void*_Tmp1=p->r;void*_Tmp2;struct _fat_ptr _Tmp3;char _Tmp4;int _Tmp5;enum Cyc_Absyn_Sign _Tmp6;void*_Tmp7;switch(*((int*)_Tmp1)){case 1: _Tmp7=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct Cyc_Absyn_Pat*p1=_Tmp7;
_Tmp7=p1;goto _LL9;}case 3: _Tmp7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;_LL9: {struct Cyc_Absyn_Pat*p1=_Tmp7;
return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _Tmp8;_Tmp8.pattern.tag=1U,_Tmp8.pattern.val=p1;_Tmp8;}));}case 8:
 return(void*)& Cyc_Tcpat_EqNull_val;case 9: _Tmp6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{enum Cyc_Absyn_Sign s=_Tmp6;int i=_Tmp5;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct));_Tmp8->tag=6,_Tmp8->f1=(unsigned)i;_Tmp8;});}case 10: _Tmp4=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;{char c=_Tmp4;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct));_Tmp8->tag=6,_Tmp8->f1=(unsigned)c;_Tmp8;});}case 11: _Tmp3=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct _fat_ptr f=_Tmp3;int i=_Tmp5;
return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct));_Tmp8->tag=5,_Tmp8->f1=f,_Tmp8->f2=i;_Tmp8;});}case 12: _Tmp7=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp2=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;struct Cyc_Absyn_Enumfield*ef=_Tmp2;
return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct));_Tmp8->tag=3,_Tmp8->f1=ed,_Tmp8->f2=ef;_Tmp8;});}case 13: _Tmp7=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp2=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{void*t=_Tmp7;struct Cyc_Absyn_Enumfield*ef=_Tmp2;
return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct));_Tmp8->tag=4,_Tmp8->f1=t,_Tmp8->f2=ef;_Tmp8;});}case 5:
# 945
 if(!Cyc_Tcutil_is_pointer_type(_check_null(p->topt))|| !
Cyc_Tcutil_is_nullable_pointer_type(_check_null(p->topt),0))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("non-null pointer type or non-pointer type in pointer pattern",sizeof(char),61U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpA;})(_tag_fat(_Tmp9,sizeof(void*),1));});
return(void*)& Cyc_Tcpat_NeqNull_val;case 7: _Tmp7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct Cyc_Absyn_Datatypedecl*ddecl=_Tmp7;struct Cyc_Absyn_Datatypefield*df=_Tmp2;
# 950
if(ddecl->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct));_Tmp8->tag=9,_Tmp8->f1=ddecl,_Tmp8->f2=df;_Tmp8;});
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct));_Tmp8->tag=7,({int _Tmp9=(int)Cyc_Tcpat_datatype_tag_number(ddecl,df->name);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=ddecl,_Tmp8->f3=df;_Tmp8;});}case 6: _Tmp7=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f4;if((void*)_Tmp7!=0){void*atype=_Tmp7;struct Cyc_List_List*dlps=_Tmp2;
# 954
void*_Tmp8=Cyc_Absyn_compress(atype);union Cyc_Absyn_AggrInfo _Tmp9;if(*((int*)_Tmp8)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)==24){_Tmp9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp9;
# 956
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(!_check_null(ad->impl)->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("non-tagged aggregate in pattern test",sizeof(char),37U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({int(*_TmpC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpC;})(_tag_fat(_TmpB,sizeof(void*),1));});{
void*_TmpA=(void*)_check_null((*((struct _tuple16*)_check_null(dlps)->hd)).f0)->hd;void*_TmpB;if(*((int*)_TmpA)==1){_TmpB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_TmpA)->f1;{struct _fat_ptr*f=_TmpB;
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct));_TmpC->tag=8,_TmpC->f1=f,({int _TmpD=Cyc_Tcpat_get_member_offset(ad,f);_TmpC->f2=_TmpD;});_TmpC;});}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat("no field name in tagged union pattern",sizeof(char),38U);_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({int(*_TmpE)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpE;})(_tag_fat(_TmpD,sizeof(void*),1));});};}}}else{goto _LL21;}}else{_LL21:
# 963
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("non-tagged aggregate in pattern test",sizeof(char),37U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({int(*_TmpC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpC;})(_tag_fat(_TmpB,sizeof(void*),1));});};}else{goto _LL1C;}default: _LL1C:
# 965
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("non-test pattern in pattern test",sizeof(char),33U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpA;})(_tag_fat(_Tmp9,sizeof(void*),1));});};}};}
# 970
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
union Cyc_Tcpat_PatOrWhere _Tmp0;_Tmp0.pattern.tag=1U,_Tmp0.pattern.val=p;return _Tmp0;}
# 974
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(_tag_fat("NULL",sizeof(char),5U));_Tmp0->name=_Tmp1;}),_Tmp0->arity=0,_Tmp0->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});return _Tmp0;}
# 977
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(_tag_fat("&",sizeof(char),2U));_Tmp0->name=_Tmp1;}),_Tmp0->arity=1,_Tmp0->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});return _Tmp0;}
# 980
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(_tag_fat("&",sizeof(char),2U));_Tmp0->name=_Tmp1;}),_Tmp0->arity=1,_Tmp0->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});return _Tmp0;}
# 983
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Int_v(i);_Tmp0->name=_Tmp1;}),_Tmp0->arity=0,_Tmp0->span=0,_Tmp0->orig_pat=p;return _Tmp0;}
# 986
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(f);_Tmp0->name=_Tmp1;}),_Tmp0->arity=0,_Tmp0->span=0,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});return _Tmp0;}
# 989
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Int_v((int)c);_Tmp0->name=_Tmp1;}),_Tmp0->arity=0,_Tmp0->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});return _Tmp0;}
# 992
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(_tag_fat("$",sizeof(char),2U));_Tmp0->name=_Tmp1;}),_Tmp0->arity=i,_Tmp0->span=& Cyc_Tcpat_one_opt,_Tmp0->orig_pat=p;return _Tmp0;}
# 997
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_null_con(p);_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 1000
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_int_con(i,p);_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 1003
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_char_con(c,p);_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 1006
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_float_con(f,p);_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 1009
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_null_ptr_con(p0);_Tmp0->f1=_Tmp1;}),({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=p,_Tmp2->tl=0;_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 1012
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_ptr_con(p0);_Tmp0->f1=_Tmp1;}),({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=p,_Tmp2->tl=0;_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 1015
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=({int _Tmp2=Cyc_List_length(ss);Cyc_Tcpat_tuple_con(_Tmp2,p);});_Tmp0->f1=_Tmp1;}),_Tmp0->f2=ss;_Tmp0;});}
# 1018
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1020
struct Cyc_Tcpat_Con_s*c;c=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s)),({union Cyc_Tcpat_Name_value _Tmp0=Cyc_Tcpat_Name_v(con_name);c->name=_Tmp0;}),({int _Tmp0=Cyc_List_length(ps);c->arity=_Tmp0;}),c->span=span,({union Cyc_Tcpat_PatOrWhere _Tmp0=Cyc_Tcpat_pw(p);c->orig_pat=_Tmp0;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,_Tmp0->f1=c,_Tmp0->f2=ps;_Tmp0;});}
# 1025
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_Tmp0=p->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct _fat_ptr _Tmp4;char _Tmp5;int _Tmp6;enum Cyc_Absyn_Sign _Tmp7;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 4: _LL6:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));_Tmp8->tag=0;_Tmp8;});goto _LL0;case 8:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 9: _Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign sn=_Tmp7;int i=_Tmp6;
s=({int _Tmp8=i;Cyc_Tcpat_int_pat(_Tmp8,Cyc_Tcpat_pw(p));});goto _LL0;}case 10: _Tmp5=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp5;
s=Cyc_Tcpat_char_pat(c,p);goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _fat_ptr f=_Tmp4;
s=Cyc_Tcpat_float_pat(f,p);goto _LL0;}case 1: _Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Pat*p2=_Tmp3;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 3: _Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Pat*p2=_Tmp3;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 5: _Tmp3=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*pp=_Tmp3;
# 1038
{void*_Tmp8=Cyc_Absyn_compress(_check_null(p->topt));void*_Tmp9;if(*((int*)_Tmp8)==3){_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp8)->f1.ptr_atts.nullable;{void*n=_Tmp9;
# 1040
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
void*ss=Cyc_Tcpat_compile_pat(pp);
s=is_nullable?Cyc_Tcpat_null_ptr_pat(ss,p): Cyc_Tcpat_ptr_pat(ss,p);
goto _LL23;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("expecting pointertype for pattern!",sizeof(char),35U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({int(*_TmpC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpC;})(_tag_fat(_TmpB,sizeof(void*),1));});}_LL23:;}
# 1046
goto _LL0;}case 7: _Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp3;struct Cyc_Absyn_Datatypefield*tuf=_Tmp2;struct Cyc_List_List*ps=_Tmp1;
# 1048
int*span;
{void*_Tmp8=Cyc_Absyn_compress(_check_null(p->topt));if(*((int*)_Tmp8)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)){case 22:
# 1051
 span=tud->is_extensible?0:({int*_Tmp9=_cycalloc_atomic(sizeof(int));({int _TmpA=Cyc_List_length((struct Cyc_List_List*)_check_null(tud->fields)->v);*_Tmp9=_TmpA;});_Tmp9;});
goto _LL28;case 23:
 span=& Cyc_Tcpat_one_opt;goto _LL28;default: goto _LL2D;}else{_LL2D:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("void datatype pattern has bad type",sizeof(char),35U);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({int(*_TmpB)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpB;})(_tag_fat(_TmpA,sizeof(void*),1));});}_LL28:;}
# 1056
s=({struct _fat_ptr _Tmp8=*(*tuf->name).f1;int*_Tmp9=span;struct Cyc_List_List*_TmpA=({struct Cyc_List_List*(*_TmpB)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;_TmpB;})(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_con_pat(_Tmp8,_Tmp9,_TmpA,p);});
goto _LL0;}case 6: _Tmp3=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{void*atype=_Tmp3;struct Cyc_List_List*dlps=_Tmp2;
# 1060
struct Cyc_List_List*fields;
enum Cyc_Absyn_AggrKind aggr_kind;
int is_tagged=0;
{void*_Tmp8=Cyc_Absyn_compress(_check_null(atype));void*_Tmp9;enum Cyc_Absyn_AggrKind _TmpA;union Cyc_Absyn_AggrInfo _TmpB;switch(*((int*)_Tmp8)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)==24){_TmpB=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)->f1;{union Cyc_Absyn_AggrInfo ainfo=_TmpB;
# 1065
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);
aggr_kind=ad->kind;
fields=_check_null(ad->impl)->fields;
is_tagged=ad->impl->tagged;
goto _LL2F;}}else{goto _LL34;}case 6: _TmpA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp8)->f1;_Tmp9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp8)->f3;{enum Cyc_Absyn_AggrKind ak=_TmpA;struct Cyc_List_List*fs=_Tmp9;
# 1071
aggr_kind=ak;
fields=fs;
goto _LL2F;}default: _LL34:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat("bad type on aggr pattern",sizeof(char),25U);_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({int(*_TmpE)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpE;})(_tag_fat(_TmpD,sizeof(void*),1));});}_LL2F:;}
# 1078
if((int)aggr_kind==0){
struct Cyc_List_List*ps=0;
for(1;fields!=0;fields=fields->tl){
# 1082
int found=Cyc_strcmp(*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,_tag_fat("",sizeof(char),1U))==0;
{struct Cyc_List_List*dlps0=dlps;for(0;!found && dlps0!=0;dlps0=dlps0->tl){
struct _tuple16*_Tmp8=(struct _tuple16*)dlps0->hd;void*_Tmp9;void*_TmpA;_TmpA=_Tmp8->f0;_Tmp9=_Tmp8->f1;{struct Cyc_List_List*dl=_TmpA;struct Cyc_Absyn_Pat*p=_Tmp9;
void*_TmpB;if(dl!=0){if(*((int*)((struct Cyc_List_List*)dl)->hd)==1){if(((struct Cyc_List_List*)dl)->tl==0){_TmpB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)dl->hd)->f1;{struct _fat_ptr*f=_TmpB;
# 1087
if(Cyc_strptrcmp(f,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)==0){
ps=({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpD=Cyc_Tcpat_compile_pat(p);_TmpC->hd=_TmpD;}),_TmpC->tl=ps;_TmpC;});
found=1;}
# 1091
goto _LL39;}}else{goto _LL3C;}}else{goto _LL3C;}}else{_LL3C:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=_tag_fat("bad designator(s)",sizeof(char),18U);_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({int(*_TmpE)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpE;})(_tag_fat(_TmpD,sizeof(void*),1));});}_LL39:;}}}
# 1095
if(!found)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("bad designator",sizeof(char),15U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpA;})(_tag_fat(_Tmp9,sizeof(void*),1));});}
# 1098
s=({struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(ps);Cyc_Tcpat_tuple_pat(_Tmp8,Cyc_Tcpat_pw(p));});}else{
# 1101
if(!is_tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("patterns on untagged unions not yet supported.",sizeof(char),47U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp9,sizeof(void*),1));});{
int*span;span=_cycalloc_atomic(sizeof(int)),({int _Tmp8=Cyc_List_length(fields);*span=_Tmp8;});{
void*_Tmp8;void*_Tmp9;if(dlps!=0){if(((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f0!=0){if(*((int*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f0)->hd)==1){if(((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f0)->tl==0){if(((struct Cyc_List_List*)dlps)->tl==0){_Tmp9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct _tuple16*)dlps->hd)->f0->hd)->f1;_Tmp8=((struct _tuple16*)dlps->hd)->f1;{struct _fat_ptr*f=_Tmp9;struct Cyc_Absyn_Pat*p2=_Tmp8;
# 1106
s=({struct _fat_ptr _TmpA=*f;int*_TmpB=span;struct Cyc_List_List*_TmpC=({struct Cyc_List_List*_TmpD=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpE=Cyc_Tcpat_compile_pat(p2);_TmpD->hd=_TmpE;}),_TmpD->tl=0;_TmpD;});Cyc_Tcpat_con_pat(_TmpA,_TmpB,_TmpC,p);});
goto _LL3E;}}else{goto _LL41;}}else{goto _LL41;}}else{goto _LL41;}}else{goto _LL41;}}else{_LL41:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("bad union pattern",sizeof(char),18U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({int(*_TmpC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpC;})(_tag_fat(_TmpB,sizeof(void*),1));});}_LL3E:;}}}
# 1111
goto _LL0;}case 12: _Tmp3=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp3;
# 1118
_Tmp3=ef;goto _LL1C;}case 13: _Tmp3=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL1C: {struct Cyc_Absyn_Enumfield*ef=_Tmp3;
s=Cyc_Tcpat_con_pat(*(*ef->name).f1,0,0,p);goto _LL0;}case 14:
# 1121
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));_Tmp8->tag=0;_Tmp8;});}_LL0:;}
# 1125
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1150
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});int _Tmp1;int _Tmp2;unsigned _Tmp3;void*_Tmp4;unsigned _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0.f0)){case 0: if(*((int*)_Tmp0.f1)==0)
return 1;else{goto _LL9;}case 1: if(*((int*)_Tmp0.f1)==1)
return 1;else{goto _LL9;}case 2: if(*((int*)_Tmp0.f1)==2){_Tmp6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0.f0)->f2;_Tmp5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0.f0)->f3;_Tmp4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0.f1)->f2;_Tmp3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0.f1)->f3;{struct Cyc_Absyn_Datatypefield*df1=_Tmp6;unsigned i1=_Tmp5;struct Cyc_Absyn_Datatypefield*df2=_Tmp4;unsigned i2=_Tmp3;
# 1155
return df1==df2 && i1==i2;}}else{goto _LL9;}default: if(*((int*)_Tmp0.f1)==3){_Tmp2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0.f0)->f2;_Tmp6=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0.f1)->f2;_Tmp4=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0.f1)->f3;{int b1=_Tmp2;struct _fat_ptr*f1=_Tmp6;int b2=_Tmp1;struct _fat_ptr*f2=_Tmp4;
# 1157
return b1==b2 && Cyc_strptrcmp(f1,f2)==0;}}else{_LL9:
 return 0;}};}
# 1162
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
for(1;p1!=0 && p2!=0;(p1=p1->tl,p2=p2->tl)){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;}
if(p1!=p2)return 0;
return 1;}
# 1169
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*test=Cyc_Tcpat_get_pat_test(con->orig_pat);
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(*((int*)d2)==2){_Tmp2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d2)->f1;_Tmp1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d2)->f2;_Tmp0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d2)->f3;{struct Cyc_List_List*access2=_Tmp2;struct Cyc_List_List*switch_clauses=_Tmp1;void*default_decision=_Tmp0;
# 1173
if(Cyc_Tcpat_same_path(access,access2)&&(int)(con->orig_pat.pattern.tag==1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct));_Tmp3->tag=2,_Tmp3->f1=access2,({
struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f0=test,_Tmp7->f1=d1;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=switch_clauses;_Tmp5;});_Tmp3->f2=_Tmp4;}),_Tmp3->f3=default_decision;_Tmp3;});
# 1177
goto _LL4;}}else{_LL4:
 return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct));_Tmp3->tag=2,_Tmp3->f1=access,({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f0=test,_Tmp7->f1=d1;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=0;_Tmp5;});_Tmp3->f2=_Tmp4;}),_Tmp3->f3=d2;_Tmp3;});};}struct _tuple21{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1190
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1193
static void Cyc_Tcpat_print_tab(int i){
for(1;i!=0;-- i){
Cyc_fprintf(Cyc_stderr,_tag_fat(" ",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}}
# 1197
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _Tmp0=c->name;int _Tmp1;struct _fat_ptr _Tmp2;if(_Tmp0.Name_v.tag==1){_Tmp2=_Tmp0.Name_v.val;{struct _fat_ptr s=_Tmp2;
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)s;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp4,sizeof(void*),1));});goto _LL0;}}else{_Tmp1=_Tmp0.Int_v.val;{int i=_Tmp1;
({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)i;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp4,sizeof(void*),1));});goto _LL0;}}_LL0:;}
# 1204
static void Cyc_Tcpat_print_access(void*a){
int _Tmp0;unsigned _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 Cyc_fprintf(Cyc_stderr,_tag_fat("DUMMY",sizeof(char),6U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 1:
 Cyc_fprintf(Cyc_stderr,_tag_fat("*",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 2: _Tmp2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)a)->f2;_Tmp1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)a)->f3;{struct Cyc_Absyn_Datatypefield*f=_Tmp2;unsigned i=_Tmp1;
# 1209
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)i);_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s[%d]",sizeof(char),7U),_tag_fat(_Tmp5,sizeof(void*),2));});goto _LL0;}default: _Tmp0=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)a)->f2;_Tmp2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)a)->f3;{int tagged=_Tmp0;struct _fat_ptr*f=_Tmp2;
# 1211
if(tagged)
Cyc_fprintf(Cyc_stderr,_tag_fat(".tagunion",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)*f;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,_tag_fat(".%s",sizeof(char),4U),_tag_fat(_Tmp4,sizeof(void*),1));});
goto _LL0;}}_LL0:;}
# 1218
static void Cyc_Tcpat_print_pat_test(void*p){
int _Tmp0;unsigned _Tmp1;struct _fat_ptr _Tmp2;void*_Tmp3;switch(*((int*)p)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)p)->f1==0){
Cyc_fprintf(Cyc_stderr,_tag_fat("where(NULL)",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}else{_Tmp3=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)p)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("where(%s)",sizeof(char),10U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}}case 1:
 Cyc_fprintf(Cyc_stderr,_tag_fat("NULL",sizeof(char),5U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 2:
 Cyc_fprintf(Cyc_stderr,_tag_fat("NOT-NULL:",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 4: _Tmp3=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)p)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp3;
_Tmp3=ef;goto _LLC;}case 3: _Tmp3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)p)->f2;_LLC: {struct Cyc_Absyn_Enumfield*ef=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_qvar2string(ef->name);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 5: _Tmp2=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)p)->f1;{struct _fat_ptr s=_Tmp2;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 6: _Tmp1=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)p)->f1;{unsigned i=_Tmp1;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)i);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 7: _Tmp0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)p)->f1;{int i=_Tmp0;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("datatypetag(%d)",sizeof(char),16U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 8: _Tmp3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)p)->f1;_Tmp0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)p)->f2;{struct _fat_ptr*f=_Tmp3;int i=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)*f;_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)i;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_fprintf(Cyc_stderr,_tag_fat("uniontag[%s](%d)",sizeof(char),17U),_tag_fat(_Tmp6,sizeof(void*),2));});goto _LL0;}default: _Tmp3=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)p)->f2;{struct Cyc_Absyn_Datatypefield*f=_Tmp3;
# 1231
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_qvar2string(f->name);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("datatypefield(%s)",sizeof(char),18U),_tag_fat(_Tmp5,sizeof(void*),1));});}}_LL0:;}
# 1235
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_Absynpp_stmt2string(r->rhs);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 1239
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)d)){case 1: _Tmp2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)d)->f1;{struct Cyc_Tcpat_Rhs*rhs=_Tmp2;
# 1242
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,_tag_fat("Success(",sizeof(char),9U),_tag_fat(0U,sizeof(void*),0));Cyc_Tcpat_print_rhs(rhs);Cyc_fprintf(Cyc_stderr,_tag_fat(")\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}case 0:
 Cyc_fprintf(Cyc_stderr,_tag_fat("Failure\n",sizeof(char),9U),_tag_fat(0U,sizeof(void*),0));goto _LL0;default: _Tmp2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f1;_Tmp1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f2;_Tmp0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f3;{struct Cyc_List_List*a=_Tmp2;struct Cyc_List_List*cases=_Tmp1;void*def=_Tmp0;
# 1247
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,_tag_fat("Switch[",sizeof(char),8U),_tag_fat(0U,sizeof(void*),0));
for(1;a!=0;a=a->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)a->hd)->access);
if(a->tl!=0)Cyc_fprintf(Cyc_stderr,_tag_fat(",",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 1253
Cyc_fprintf(Cyc_stderr,_tag_fat("] {\n",sizeof(char),5U),_tag_fat(0U,sizeof(void*),0));
for(1;cases!=0;cases=cases->tl){
struct _tuple0 _Tmp3=*((struct _tuple0*)cases->hd);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{void*pt=_Tmp5;void*d=_Tmp4;
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,_tag_fat("case ",sizeof(char),6U),_tag_fat(0U,sizeof(void*),0));
Cyc_Tcpat_print_pat_test(pt);
Cyc_fprintf(Cyc_stderr,_tag_fat(":\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));
Cyc_Tcpat_print_dec_tree(d,tab + 7);}}
# 1262
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,_tag_fat("default:\n",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));
Cyc_Tcpat_print_dec_tree(def,tab + 7);
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,_tag_fat("}\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));}}_LL0:;}
# 1270
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1278
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_Tmp0;if(*((int*)td)==1){_Tmp0=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)td)->f1;{struct Cyc_Set_Set*cs=_Tmp0;
# 1288
return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_Tmp1->tag=1,({struct Cyc_Set_Set*_Tmp2=({struct Cyc_Set_Set*(*_Tmp3)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_insert;_Tmp3;})(cs,c);_Tmp1->f1=_Tmp2;});_Tmp1;});}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("add_neg called when td is Positive",sizeof(char),35U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;({int(*_Tmp3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp3;})(_tag_fat(_Tmp2,sizeof(void*),1));});};}
# 1295
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_Tmp0;if(*((int*)td)==0){_Tmp0=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)td)->f1;{struct Cyc_Tcpat_Con_s*c2=_Tmp0;
# 1299
if(Cyc_Tcpat_compare_con(c,c2)==0)return 0U;else{return 1U;}}}else{_Tmp0=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)td)->f1;{struct Cyc_Set_Set*cs=_Tmp0;
# 1302
if(({int(*_Tmp1)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;_Tmp1;})(cs,c))return 1U;
# 1305
if(c->span!=0 &&({int _Tmp1=*c->span;_Tmp1==Cyc_Set_cardinality(cs)+ 1;}))
return 0U;
# 1308
return 2U;}};}struct _tuple22{struct Cyc_Tcpat_Con_s*f0;struct Cyc_List_List*f1;};
# 1316
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(ctxt==0)
return 0;else{_Tmp2=((struct _tuple22*)ctxt->hd)->f0;_Tmp1=((struct _tuple22*)ctxt->hd)->f1;_Tmp0=ctxt->tl;{struct Cyc_Tcpat_Con_s*c=_Tmp2;struct Cyc_List_List*args=_Tmp1;struct Cyc_List_List*tl=_Tmp0;
struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple22*_Tmp4=({struct _tuple22*_Tmp5=_cycalloc(sizeof(struct _tuple22));_Tmp5->f0=c,({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=dsc,_Tmp7->tl=args;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=tl;return _Tmp3;}};}
# 1326
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(ctxt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("norm_context: empty context",sizeof(char),28U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});else{_Tmp2=((struct _tuple22*)ctxt->hd)->f0;_Tmp1=((struct _tuple22*)ctxt->hd)->f1;_Tmp0=ctxt->tl;{struct Cyc_Tcpat_Con_s*c=_Tmp2;struct Cyc_List_List*args=_Tmp1;struct Cyc_List_List*tl=_Tmp0;
struct Cyc_List_List*_Tmp3=tl;return Cyc_Tcpat_augment(_Tmp3,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct));_Tmp4->tag=0,_Tmp4->f1=c,({struct Cyc_List_List*_Tmp5=Cyc_List_rev(args);_Tmp4->f2=_Tmp5;});_Tmp4;}));}};}
# 1338
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1340
struct _tuple1 _Tmp0=({struct _tuple1 _Tmp1;_Tmp1.f0=ctxt,_Tmp1.f1=work;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;if(_Tmp0.f0==0){if(_Tmp0.f1==0)
return dsc;else{
goto _LL6;}}else{if(_Tmp0.f1==0){_LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("build_desc: ctxt and work don't match",sizeof(char),38U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({int(*_Tmp8)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp8;})(_tag_fat(_Tmp7,sizeof(void*),1));});}else{_Tmp5=((struct _tuple22*)_Tmp0.f0->hd)->f0;_Tmp4=((struct _tuple22*)_Tmp0.f0->hd)->f1;_Tmp3=_Tmp0.f0->tl;_Tmp2=((struct _tuple21*)_Tmp0.f1->hd)->f2;_Tmp1=_Tmp0.f1->tl;{struct Cyc_Tcpat_Con_s*c=_Tmp5;struct Cyc_List_List*args=_Tmp4;struct Cyc_List_List*rest=_Tmp3;struct Cyc_List_List*dargs=_Tmp2;struct Cyc_List_List*work2=_Tmp1;
# 1345
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*td;td=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct)),td->tag=0,td->f1=c,({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=Cyc_List_rev(args);Cyc_List_append(_Tmp7,({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=dsc,_Tmp8->tl=dargs;_Tmp8;}));});td->f2=_Tmp6;});
return Cyc_Tcpat_build_desc(rest,(void*)td,work2);}}};}
# 1350
static void*Cyc_Tcpat_match(void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Tcpat_Rhs*,struct Cyc_List_List*);struct _tuple23{void*f0;struct Cyc_Tcpat_Rhs*f1;};
# 1355
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(allmrules==0)
return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct));_Tmp3->tag=0,_Tmp3->f1=dsc;_Tmp3;});else{_Tmp2=((struct _tuple23*)allmrules->hd)->f0;_Tmp1=((struct _tuple23*)allmrules->hd)->f1;_Tmp0=allmrules->tl;{void*pat1=_Tmp2;struct Cyc_Tcpat_Rhs*rhs1=_Tmp1;struct Cyc_List_List*rulerest=_Tmp0;
# 1359
return Cyc_Tcpat_match(pat1,0,dsc,0,0,rhs1,rulerest);}};}
# 1364
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
void*_Tmp0=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_Tmp1->tag=1,({struct Cyc_Set_Set*_Tmp2=Cyc_Tcpat_empty_con_set();_Tmp1->f1=_Tmp2;});_Tmp1;});return Cyc_Tcpat_or_match(_Tmp0,allmrules);}
# 1371
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1374
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;if(work==0)
return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcpat_Success_Tcpat_Decision_struct));_Tmp4->tag=1,_Tmp4->f1=right_hand_side;_Tmp4;});else{if(((struct _tuple21*)((struct Cyc_List_List*)work)->hd)->f0==0){if(((struct _tuple21*)((struct Cyc_List_List*)work)->hd)->f1==0){if(((struct _tuple21*)((struct Cyc_List_List*)work)->hd)->f2==0){_Tmp3=work->tl;{struct Cyc_List_List*workr=_Tmp3;
# 1377
struct Cyc_List_List*_Tmp4=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_Tmp5=workr;struct Cyc_Tcpat_Rhs*_Tmp6=right_hand_side;return Cyc_Tcpat_and_match(_Tmp4,_Tmp5,_Tmp6,rules);}}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _Tmp3=((struct _tuple21*)work->hd)->f0;_Tmp2=((struct _tuple21*)work->hd)->f1;_Tmp1=((struct _tuple21*)work->hd)->f2;_Tmp0=work->tl;{struct Cyc_List_List*pats=_Tmp3;struct Cyc_List_List*objs=_Tmp2;struct Cyc_List_List*dscs=_Tmp1;struct Cyc_List_List*workr=_Tmp0;
# 1379
if((pats==0 || objs==0)|| dscs==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("tcpat:and_match: malformed work frame",sizeof(char),38U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});{
struct Cyc_List_List*_Tmp4=pats;void*_Tmp5;void*_Tmp6;_Tmp6=(void*)_Tmp4->hd;_Tmp5=_Tmp4->tl;{void*pat1=_Tmp6;struct Cyc_List_List*patr=_Tmp5;
struct Cyc_List_List*_Tmp7=objs;void*_Tmp8;void*_Tmp9;_Tmp9=(struct Cyc_List_List*)_Tmp7->hd;_Tmp8=_Tmp7->tl;{struct Cyc_List_List*obj1=_Tmp9;struct Cyc_List_List*objr=_Tmp8;
struct Cyc_List_List*_TmpA=dscs;void*_TmpB;void*_TmpC;_TmpC=(void*)_TmpA->hd;_TmpB=_TmpA->tl;{void*dsc1=_TmpC;struct Cyc_List_List*dscr=_TmpB;
struct _tuple21*wf;wf=_cycalloc(sizeof(struct _tuple21)),wf->f0=patr,wf->f1=objr,wf->f2=dscr;{
void*_TmpD=pat1;struct Cyc_List_List*_TmpE=obj1;void*_TmpF=dsc1;struct Cyc_List_List*_Tmp10=ctx;struct Cyc_List_List*_Tmp11=({struct Cyc_List_List*_Tmp12=_cycalloc(sizeof(struct Cyc_List_List));_Tmp12->hd=wf,_Tmp12->tl=workr;_Tmp12;});struct Cyc_Tcpat_Rhs*_Tmp12=right_hand_side;return Cyc_Tcpat_match(_TmpD,_TmpE,_TmpF,_Tmp10,_Tmp11,_Tmp12,rules);}}}}}}}};}
# 1390
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_Tmp0;if(*((int*)dsc)==1){_Tmp0=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)dsc)->f1;{struct Cyc_Set_Set*ncs=_Tmp0;
# 1396
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_Tmp1->tag=1,({struct Cyc_Set_Set*_Tmp2=Cyc_Tcpat_empty_con_set();_Tmp1->f1=_Tmp2;});_Tmp1;});
struct Cyc_List_List*res=0;
{int i=0;for(0;i < pcon->arity;++ i){
res=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=any,_Tmp1->tl=res;_Tmp1;});}}
return res;}}else{_Tmp0=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)dsc)->f2;{struct Cyc_List_List*dargs=_Tmp0;
return dargs;}};}
# 1405
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
void*_Tmp0;if(pw.where_clause.tag==2)
return(void*)& Cyc_Tcpat_Dummy_val;else{_Tmp0=pw.pattern.val;{struct Cyc_Absyn_Pat*p=_Tmp0;
# 1409
void*_Tmp1=p->r;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp1)){case 5:
# 1411
 if(i!=0)
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("get_access on pointer pattern with offset %d\n",sizeof(char),46U),_tag_fat(_Tmp5,sizeof(void*),1));});
return(void*)& Cyc_Tcpat_Deref_val;case 7: _Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp3;struct Cyc_Absyn_Datatypefield*tuf=_Tmp2;
return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct));_Tmp4->tag=2,_Tmp4->f1=tud,_Tmp4->f2=tuf,_Tmp4->f3=(unsigned)i;_Tmp4;});}case 6: _Tmp3=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f4;{void*atype=_Tmp3;struct Cyc_List_List*dlps=_Tmp2;
# 1416
struct Cyc_List_List*fields;
int is_tagged=0;
{void*_Tmp4=Cyc_Absyn_compress(_check_null(atype));void*_Tmp5;union Cyc_Absyn_AggrInfo _Tmp6;switch(*((int*)_Tmp4)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)==24){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp4)->f1)->f1;{union Cyc_Absyn_AggrInfo ainfo=_Tmp6;
# 1420
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);
is_tagged=_check_null(ad->impl)->tagged;
fields=ad->impl->fields;
goto _LLE;}}else{goto _LL13;}case 6: _Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp4)->f3;{struct Cyc_List_List*fs=_Tmp5;
# 1425
fields=fs;
goto _LLE;}default: _LL13:
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("bad type in aggr pattern",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));}_LLE:;}
# 1429
if(is_tagged){
struct Cyc_List_List*_Tmp4=(*((struct _tuple16*)_check_null(dlps)->hd)).f0;void*_Tmp5;if(_Tmp4!=0){if(*((int*)((struct Cyc_List_List*)_Tmp4)->hd)==1){if(((struct Cyc_List_List*)_Tmp4)->tl==0){_Tmp5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp4->hd)->f1;{struct _fat_ptr*f=_Tmp5;
# 1432
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct));_Tmp6->tag=3,_Tmp6->f1=atype,_Tmp6->f2=is_tagged,_Tmp6->f3=f;_Tmp6;});}}else{goto _LL18;}}else{goto _LL18;}}else{_LL18:
# 1434
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_pat2string(p);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({int(*_Tmp8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp8;})(_tag_fat("get_access on bad aggr pattern: %s",sizeof(char),35U),_tag_fat(_Tmp7,sizeof(void*),1));});};}{
# 1437
int orig_i=i;
for(1;i!=0;-- i){fields=_check_null(fields)->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct));_Tmp4->tag=3,_Tmp4->f1=atype,_Tmp4->f2=is_tagged,_Tmp4->f3=((struct Cyc_Absyn_Aggrfield*)_check_null(fields)->hd)->name;_Tmp4;});}}default:
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_pat2string(p);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("get_access on bad pattern: %s",sizeof(char),30U),_tag_fat(_Tmp5,sizeof(void*),1));});};}};}struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Tcpat_Con_s*f1;};
# 1446
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple24*env,int i){
void*_Tmp0;void*_Tmp1;_Tmp1=env->f0;_Tmp0=env->f1;{struct Cyc_List_List*obj=_Tmp1;struct Cyc_Tcpat_Con_s*pcon=_Tmp0;
void*acc=Cyc_Tcpat_get_access(pcon->orig_pat,i);
struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Tcpat_PathNode*_Tmp3=({struct Cyc_Tcpat_PathNode*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcpat_PathNode));_Tmp4->orig_pat=pcon->orig_pat,_Tmp4->access=acc;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=obj;return _Tmp2;}}
# 1451
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple24 env=({struct _tuple24 _Tmp0;_Tmp0.f0=obj,_Tmp0.f1=pcon;_Tmp0;});
return({struct Cyc_List_List*(*_Tmp0)(int,struct Cyc_List_List*(*)(struct _tuple24*,int),struct _tuple24*)=(struct Cyc_List_List*(*)(int,struct Cyc_List_List*(*)(struct _tuple24*,int),struct _tuple24*))Cyc_List_tabulate_c;_Tmp0;})(pcon->arity,Cyc_Tcpat_getoarg,& env);}
# 1459
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1462
void*_Tmp0;void*_Tmp1;if(*((int*)p)==0){
struct Cyc_List_List*_Tmp2=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_Tmp3=work;struct Cyc_Tcpat_Rhs*_Tmp4=right_hand_side;return Cyc_Tcpat_and_match(_Tmp2,_Tmp3,_Tmp4,rules);}else{_Tmp1=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)p)->f1;_Tmp0=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)p)->f2;{struct Cyc_Tcpat_Con_s*pcon=_Tmp1;struct Cyc_List_List*pargs=_Tmp0;
# 1465
enum Cyc_Tcpat_Answer _Tmp2=Cyc_Tcpat_static_match(pcon,dsc);switch((int)_Tmp2){case Cyc_Tcpat_Yes:  {
# 1467
struct Cyc_List_List*ctx2;ctx2=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple22*_Tmp3=({struct _tuple22*_Tmp4=_cycalloc(sizeof(struct _tuple22));_Tmp4->f0=pcon,_Tmp4->f1=0;_Tmp4;});ctx2->hd=_Tmp3;}),ctx2->tl=ctx;{
struct _tuple21*work_frame;work_frame=_cycalloc(sizeof(struct _tuple21)),work_frame->f0=pargs,({struct Cyc_List_List*_Tmp3=Cyc_Tcpat_getoargs(pcon,obj);work_frame->f1=_Tmp3;}),({
struct Cyc_List_List*_Tmp3=Cyc_Tcpat_getdargs(pcon,dsc);work_frame->f2=_Tmp3;});{
struct Cyc_List_List*work2;work2=_cycalloc(sizeof(struct Cyc_List_List)),work2->hd=work_frame,work2->tl=work;
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}}}case Cyc_Tcpat_No:  {
# 1473
void*_Tmp3=Cyc_Tcpat_build_desc(ctx,dsc,work);return Cyc_Tcpat_or_match(_Tmp3,rules);}default:  {
# 1475
struct Cyc_List_List*ctx2;ctx2=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple22*_Tmp3=({struct _tuple22*_Tmp4=_cycalloc(sizeof(struct _tuple22));_Tmp4->f0=pcon,_Tmp4->f1=0;_Tmp4;});ctx2->hd=_Tmp3;}),ctx2->tl=ctx;{
struct _tuple21*work_frame;work_frame=_cycalloc(sizeof(struct _tuple21)),work_frame->f0=pargs,({struct Cyc_List_List*_Tmp3=Cyc_Tcpat_getoargs(pcon,obj);work_frame->f1=_Tmp3;}),({
struct Cyc_List_List*_Tmp3=Cyc_Tcpat_getdargs(pcon,dsc);work_frame->f2=_Tmp3;});{
struct Cyc_List_List*work2;work2=_cycalloc(sizeof(struct Cyc_List_List)),work2->hd=work_frame,work2->tl=work;{
void*s=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*f=({void*_Tmp3=({struct Cyc_List_List*_Tmp4=ctx;void*_Tmp5=Cyc_Tcpat_add_neg(dsc,pcon);Cyc_Tcpat_build_desc(_Tmp4,_Tmp5,work);});Cyc_Tcpat_or_match(_Tmp3,rules);});
return Cyc_Tcpat_ifeq(obj,pcon,s,f);}}}}};}};}
# 1491
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1496
void*_Tmp0;void*_Tmp1;switch(*((int*)d)){case 0: _Tmp1=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)d)->f1;{void*td=_Tmp1;
# 1498
if(!(*exhaust_done)){not_exhaust(env1,td);*exhaust_done=1;}
goto _LL0;}case 1: _Tmp1=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)d)->f1;{struct Cyc_Tcpat_Rhs*r=_Tmp1;
rhs_appears(env2,r);goto _LL0;}default: _Tmp1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f2;_Tmp0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f3;{struct Cyc_List_List*cases=_Tmp1;void*def=_Tmp0;
# 1502
for(1;cases!=0;cases=cases->tl){
struct _tuple0 _Tmp2=*((struct _tuple0*)cases->hd);void*_Tmp3;_Tmp3=_Tmp2.f1;{void*d=_Tmp3;
Cyc_Tcpat_check_exhaust_overlap(d,not_exhaust,env1,rhs_appears,env2,exhaust_done);}}
# 1507
Cyc_Tcpat_check_exhaust_overlap(def,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1509
goto _LL0;}}_LL0:;}
# 1517
static struct _tuple23*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs)),rhs->used=0,rhs->pat_loc=swc->pattern->loc,rhs->rhs=swc->body;{
void*sp;
if(swc->where_clause!=0){
union Cyc_Tcpat_PatOrWhere w=({union Cyc_Tcpat_PatOrWhere _Tmp0;_Tmp0.where_clause.tag=2U,_Tmp0.where_clause.val=swc->where_clause;_Tmp0;});
sp=({struct Cyc_List_List*_Tmp0=({void*_Tmp1[2];_Tmp1[0]=sp0,({void*_Tmp2=Cyc_Tcpat_int_pat(*ctr,w);_Tmp1[1]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_Tmp0,w);});
*ctr=*ctr + 1;}else{
# 1526
sp=({struct Cyc_List_List*_Tmp0=({void*_Tmp1[2];_Tmp1[0]=sp0,({void*_Tmp2=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));_Tmp3->tag=0;_Tmp3;});_Tmp1[1]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_Tmp0,({union Cyc_Tcpat_PatOrWhere _Tmp1;_Tmp1.where_clause.tag=2U,_Tmp1.where_clause.val=0;_Tmp1;}));});}{
struct _tuple23*_Tmp0=_cycalloc(sizeof(struct _tuple23));_Tmp0->f0=sp,_Tmp0->f1=rhs;return _Tmp0;}}}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1532
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1534
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){
void*_Tmp0;void*_Tmp1;if(*((int*)d)==0){_Tmp1=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)d)->f1;_Tmp0=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)d)->f2;{struct Cyc_Tcpat_Con_s*c=_Tmp1;struct Cyc_List_List*ds=_Tmp0;
# 1538
union Cyc_Tcpat_Name_value n=c->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _Tmp2=c->orig_pat;void*_Tmp3;if(_Tmp2.where_clause.tag==2)
return Cyc_Tcpat_desc2string((void*)_check_null(ds)->hd);else{_Tmp3=_Tmp2.pattern.val;{struct Cyc_Absyn_Pat*p2=_Tmp3;
p=p2;goto _LL5;}}_LL5:;}{
# 1544
void*_Tmp2=p->r;struct _fat_ptr _Tmp3;char _Tmp4;int _Tmp5;void*_Tmp6;void*_Tmp7;switch(*((int*)_Tmp2)){case 0:
 return _tag_fat("_",sizeof(char),2U);case 1: _Tmp7=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;
return Cyc_Absynpp_qvar2string(vd->name);}case 3: _Tmp7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("*%s",sizeof(char),4U),_tag_fat(_Tmp9,sizeof(void*),1));});}case 4: _Tmp7=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;_Tmp6=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp7;struct Cyc_Absyn_Vardecl*vd=_Tmp6;
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=(struct _fat_ptr)*tv->name;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_aprintf(_tag_fat("%s<`%s>",sizeof(char),8U),_tag_fat(_TmpA,sizeof(void*),2));});}case 5:
# 1550
 return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_Tcpat_descs2string(ds);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("&%s",sizeof(char),4U),_tag_fat(_Tmp9,sizeof(void*),1));});case 6: _Tmp7=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;_Tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp2)->f4;{void*atype=_Tmp7;struct Cyc_List_List*dlps=_Tmp6;
# 1552
void*_Tmp8=Cyc_Absyn_compress(_check_null(atype));int _Tmp9;union Cyc_Absyn_AggrInfo _TmpA;switch(*((int*)_Tmp8)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)==24){_TmpA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)->f1;{union Cyc_Absyn_AggrInfo ainfo=_TmpA;
# 1554
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);
if((int)ad->kind==1){
void*_TmpB;if(dlps!=0){if(((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f0!=0){if(*((int*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f0)->hd)==1){_TmpB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct _tuple16*)dlps->hd)->f0->hd)->f1;{struct _fat_ptr*f=_TmpB;
# 1558
return({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,({struct _fat_ptr _TmpE=(struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name);_TmpD.f1=_TmpE;});_TmpD;});struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=(struct _fat_ptr)*f;_TmpE;});struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({
struct _fat_ptr _Tmp10=(struct _fat_ptr)Cyc_Tcpat_descs2string(ds);_TmpF.f1=_Tmp10;});_TmpF;});void*_TmpF[3];_TmpF[0]=& _TmpC,_TmpF[1]=& _TmpD,_TmpF[2]=& _TmpE;Cyc_aprintf(_tag_fat("%s{.%s = %s}",sizeof(char),13U),_tag_fat(_TmpF,sizeof(void*),3));});}}else{goto _LL33;}}else{goto _LL33;}}else{_LL33:
 goto _LL30;}_LL30:;}
# 1562
return({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({struct _fat_ptr _TmpD=(struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name);_TmpC.f1=_TmpD;});_TmpC;});struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,({struct _fat_ptr _TmpE=(struct _fat_ptr)Cyc_Tcpat_descs2string(ds);_TmpD.f1=_TmpE;});_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;Cyc_aprintf(_tag_fat("%s{%s}",sizeof(char),7U),_tag_fat(_TmpD,sizeof(void*),2));});}}else{goto _LL2E;}case 6: _Tmp9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp8)->f2;{int is_tuple=_Tmp9;
# 1564
if(is_tuple)return({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({struct _fat_ptr _TmpD=(struct _fat_ptr)Cyc_Tcpat_descs2string(ds);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_aprintf(_tag_fat("$(%s)",sizeof(char),6U),_tag_fat(_TmpC,sizeof(void*),1));});else{
return({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({struct _fat_ptr _TmpD=(struct _fat_ptr)Cyc_Tcpat_descs2string(ds);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;Cyc_aprintf(_tag_fat("{%s}",sizeof(char),5U),_tag_fat(_TmpC,sizeof(void*),1));});}}default: _LL2E:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat("bad type in aggr_p",sizeof(char),19U);_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({int(*_TmpD)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpD;})(_tag_fat(_TmpC,sizeof(void*),1));});};}case 7: _Tmp7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp7;
# 1569
if(ds==0)
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp9,sizeof(void*),1));});else{
# 1572
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_Absynpp_qvar2string(tuf->name);_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=(struct _fat_ptr)Cyc_Tcpat_descs2string(ds);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_aprintf(_tag_fat("%s(%s)",sizeof(char),7U),_tag_fat(_TmpA,sizeof(void*),2));});}}case 8:
 return _tag_fat("NULL",sizeof(char),5U);case 9: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{int i=_Tmp5;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)i;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp9,sizeof(void*),1));});}case 10: _Tmp4=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;{char c=_Tmp4;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)((int)c);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp9,sizeof(void*),1));});}case 11: _Tmp3=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;_Tmp5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{struct _fat_ptr f=_Tmp3;int i=_Tmp5;
return f;}case 12: _Tmp7=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp7;
_Tmp7=ef;goto _LL24;}case 13: _Tmp7=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;_LL24: {struct Cyc_Absyn_Enumfield*ef=_Tmp7;
return Cyc_Absynpp_qvar2string(ef->name);}case 16: _Tmp7=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp7;
return Cyc_Absynpp_exp2string(e);}default:
 _throw(& Cyc_Tcpat_Desc2string_val);};}}}else{_Tmp1=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)d)->f1;{struct Cyc_Set_Set*s=_Tmp1;
# 1583
if(Cyc_Set_is_empty(s))return _tag_fat("_",sizeof(char),2U);{
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*(*_Tmp2)(struct Cyc_Set_Set*)=(struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*))Cyc_Set_choose;_Tmp2;})(s);
# 1587
if((int)(c->orig_pat.where_clause.tag==2))_throw(& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*orig_pat=({union Cyc_Tcpat_PatOrWhere _Tmp2=c->orig_pat;if(_Tmp2.pattern.tag!=1)_throw_match();_Tmp2.pattern.val;});
void*_Tmp2=Cyc_Absyn_compress(_check_null(orig_pat->topt));union Cyc_Absyn_AggrInfo _Tmp3;void*_Tmp4;switch(*((int*)_Tmp2)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f2==Cyc_Absyn_Char_sz){
# 1592
{int i=0;for(0;i < 256;++ i){
if(!({int(*_Tmp5)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({int(*_Tmp6)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;_Tmp6;});struct Cyc_Set_Set*_Tmp6=s;_Tmp5(_Tmp6,Cyc_Tcpat_char_con((char)i,orig_pat));}))
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)i;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp6,sizeof(void*),1));});}}
_throw(& Cyc_Tcpat_Desc2string_val);}else{
# 1598
{unsigned i=0U;for(0;i < 4294967295U;++ i){
if(!({int(*_Tmp5)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({int(*_Tmp6)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;_Tmp6;});struct Cyc_Set_Set*_Tmp6=s;_Tmp5(_Tmp6,Cyc_Tcpat_int_con((int)i,c->orig_pat));}))
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)((int)i);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("%d",sizeof(char),3U),_tag_fat(_Tmp6,sizeof(void*),1));});}}
_throw(& Cyc_Tcpat_Desc2string_val);}case 22: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1.KnownDatatype.tag==2){_Tmp4=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;
# 1607
if(tud->is_extensible)_throw(& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*fields=(struct Cyc_List_List*)_check_null(tud->fields)->v;
int span=Cyc_List_length(fields);
for(1;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)fields->hd)->name).f1;
struct Cyc_List_List*args=((struct Cyc_Absyn_Datatypefield*)fields->hd)->typs;
if(!({int(*_Tmp5)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({int(*_Tmp6)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;_Tmp6;});struct Cyc_Set_Set*_Tmp6=s;_Tmp5(_Tmp6,({struct Cyc_Tcpat_Con_s*_Tmp7=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp8=Cyc_Tcpat_Name_v(n);_Tmp7->name=_Tmp8;}),_Tmp7->arity=0,_Tmp7->span=0,_Tmp7->orig_pat=c->orig_pat;_Tmp7;}));})){
if(Cyc_List_length(args)==0)return n;else{return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)n;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("%s(...)",sizeof(char),8U),_tag_fat(_Tmp6,sizeof(void*),1));});}}}
# 1616
_throw(& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL40;}case 24: _Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1;{union Cyc_Absyn_AggrInfo ainfo=_Tmp3;
# 1618
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);
if((int)ad->kind!=1)_throw(& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*fields=_check_null(ad->impl)->fields;
int span=Cyc_List_length(fields);
struct _tuple2*_Tmp5=ad->name;struct _fat_ptr _Tmp6;_Tmp6=*_Tmp5->f1;{struct _fat_ptr union_name=_Tmp6;
for(1;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;
if(!({int(*_Tmp7)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({int(*_Tmp8)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;_Tmp8;});struct Cyc_Set_Set*_Tmp8=s;_Tmp7(_Tmp8,({struct Cyc_Tcpat_Con_s*_Tmp9=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _TmpA=Cyc_Tcpat_Name_v(n);_Tmp9->name=_TmpA;}),_Tmp9->arity=0,_Tmp9->span=0,_Tmp9->orig_pat=c->orig_pat;_Tmp9;}));}))
return({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=(struct _fat_ptr)union_name;_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=(struct _fat_ptr)n;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_aprintf(_tag_fat("%s{.%s = _}",sizeof(char),12U),_tag_fat(_Tmp9,sizeof(void*),2));});}
# 1628
_throw(& Cyc_Tcpat_Desc2string_val);}}}default: goto _LL40;}case 3: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.nullable;{void*n=_Tmp4;
# 1603
if(Cyc_Tcutil_force_type2bool(0,n)&& !({int(*_Tmp5)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=({int(*_Tmp6)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;_Tmp6;});struct Cyc_Set_Set*_Tmp6=s;_Tmp5(_Tmp6,Cyc_Tcpat_null_con(orig_pat));}))
return _tag_fat("NULL",sizeof(char),5U);
return _tag_fat("&_",sizeof(char),3U);}default: _LL40:
# 1629
 _throw(& Cyc_Tcpat_Desc2string_val);};}}}};}
# 1633
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){
struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=Cyc_Tcpat_desc2string(d);*_Tmp0=_Tmp1;});return _Tmp0;}
# 1636
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*ss=({struct Cyc_List_List*(*_Tmp0)(struct _fat_ptr*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(Cyc_Tcpat_desc2stringptr,ds);
struct _fat_ptr*comma;comma=_cycalloc(sizeof(struct _fat_ptr)),*comma=_tag_fat(",",sizeof(char),2U);
{struct Cyc_List_List*x=ss;for(0;x!=0;x=x->tl){
if(x->tl!=0){
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=comma,_Tmp1->tl=x->tl;_Tmp1;});x->tl=_Tmp0;});
x=x->tl;}}}
# 1644
return Cyc_strconcat_l(ss);}
# 1647
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _fat_ptr s=Cyc_Tcpat_desc2string(desc);
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("patterns may not be exhaustive.\n\tmissing case for ",sizeof(char),51U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),2));});}
# 1649
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_Tmp2)->tag==Cyc_Tcpat_Desc2string){
# 1652
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("patterns may not be exhaustive.",sizeof(char),32U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}
# 1656
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1660
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1666
int where_ctr=0;
int*env=& where_ctr;
struct Cyc_List_List*match_rules=({struct Cyc_List_List*(*_Tmp0)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp0;})(Cyc_Tcpat_get_match,env,swcs);
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);
*dopt=dec_tree;{
# 1672
int ex_done=0;
({void(*_Tmp0)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*)=(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;_Tmp0;})(dec_tree,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1675
for(1;match_rules!=0;match_rules=match_rules->tl){
struct _tuple23*_Tmp0=(struct _tuple23*)match_rules->hd;unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0->f1->used;_Tmp1=_Tmp0->f1->pat_loc;{int b=_Tmp2;unsigned loc2=_Tmp1;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("redundant pattern (check for misspelled constructors in earlier patterns)",sizeof(char),74U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(loc2,_tag_fat(_Tmp4,sizeof(void*),1));});
# 1680
break;}}}}}
# 1689
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple15*pr,void*desc){
(*pr).f1=0;{
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _fat_ptr s=Cyc_Tcpat_desc2string(desc);
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("pattern not exhaustive.\n\tmissing case for ",sizeof(char),43U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_warn2((*pr).f0,_tag_fat(_Tmp4,sizeof(void*),2));});}
# 1692
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_Tmp2)->tag==Cyc_Tcpat_Desc2string){
# 1695
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("pattern not exhaustive.",sizeof(char),24U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_warn2((*pr).f0,_tag_fat(_Tmp5,sizeof(void*),1));});
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}}
# 1699
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1703
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs)),rhs->used=0,rhs->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_Tmp0=Cyc_Absyn_skip_stmt(0U);rhs->rhs=_Tmp0;});{
struct Cyc_List_List*match_rules;match_rules=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple23*_Tmp0=({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({void*_Tmp2=Cyc_Tcpat_compile_pat(p);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=rhs;_Tmp1;});match_rules->hd=_Tmp0;}),match_rules->tl=0;{
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);
struct _tuple15 exhaust_env=({struct _tuple15 _Tmp0;_Tmp0.f0=loc,_Tmp0.f1=1;_Tmp0;});
int ex_done=0;
({void(*_Tmp0)(void*,void(*)(struct _tuple15*,void*),struct _tuple15*,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*)=(void(*)(void*,void(*)(struct _tuple15*,void*),struct _tuple15*,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;_Tmp0;})(dec_tree,Cyc_Tcpat_not_exhaust_warn,& exhaust_env,Cyc_Tcpat_dummy_fn,0,& ex_done);
# 1711
*dopt=dec_tree;
return exhaust_env.f1;}}}
# 1719
static struct _tuple23*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1723
if(swc->where_clause!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("&&-clauses not supported in exception handlers.",sizeof(char),48U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(swc->where_clause->loc,_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1726
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs)),rhs->used=0,rhs->pat_loc=swc->pattern->loc,rhs->rhs=swc->body;{
struct _tuple23*_Tmp0=_cycalloc(sizeof(struct _tuple23));_Tmp0->f0=sp0,_Tmp0->f1=rhs;return _Tmp0;}}}
# 1729
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1732
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1736
struct Cyc_List_List*match_rules=({struct Cyc_List_List*(*_Tmp0)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(Cyc_Tcpat_get_match2,swcs);
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);
*dopt=dec_tree;{
int ex_done=0;
({void(*_Tmp0)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*)=(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;_Tmp0;})(dec_tree,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1742
for(1;match_rules!=0;match_rules=match_rules->tl){
# 1744
if(match_rules->tl==0)break;{
struct _tuple23*_Tmp0=(struct _tuple23*)match_rules->hd;unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0->f1->used;_Tmp1=_Tmp0->f1->pat_loc;{int b=_Tmp2;unsigned loc2=_Tmp1;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("redundant pattern (check for misspelled constructors in earlier patterns)",sizeof(char),74U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(loc2,_tag_fat(_Tmp4,sizeof(void*),1));});
break;}}}}}}
# 1753
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*l=(struct Cyc_List_List*)_check_null(pat_vars)->v;for(0;l!=0;l=l->tl){
if((*((struct _tuple17*)l->hd)).f0!=0)
return 1;}}
return 0;}

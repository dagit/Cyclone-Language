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
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
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
# 313
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 326
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 333
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 731 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 932
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 939
void*Cyc_Absyn_compress(void*);
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 954
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 957
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 959
extern void*Cyc_Absyn_sint_type;
# 962
void*Cyc_Absyn_gen_float_type(unsigned);
# 968
extern void*Cyc_Absyn_al_qual_type;
# 972
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 974
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_enum_type(struct _tuple2*,struct Cyc_Absyn_Enumdecl*);
# 993
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 1009
void*Cyc_Absyn_bounds_one (void);
# 1015
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1038
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1108
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1112
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1138
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1229
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 75
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*);
# 37 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 43
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 33
int Cyc_Unify_unify_and_constrain(unsigned,void*,void*);
# 35 "tcutil.h"
int Cyc_Tcutil_is_arithmetic_type(void*);
# 41
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
# 48
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 95
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 99
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 106
int Cyc_Tcutil_coerce_assign(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
# 131 "tcutil.h"
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 134
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 138
struct Cyc_List_List*Cyc_Tcutil_rsubst_effconstr(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple13{struct Cyc_List_List*f0;struct _RegionHandle*f1;};struct _tuple14{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 164
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 171
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr);
# 179
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 192
int Cyc_Tcutil_is_noalias_qual(void*,int);
# 200
int Cyc_Tcutil_is_noalias_path(struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 205
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*,void*);
# 215
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 226
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 246
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 253
int Cyc_Tcutil_force_type2bool(int,void*);
# 256
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
# 98 "tcenv.h"
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 143
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_aquals_bound(struct Cyc_Tcenv_Tenv*,void*,void*);
# 146
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int,int);
# 149
void Cyc_Tcenv_check_effect_accessible_nodelay(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 47 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 32
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_effconstr_opt;struct Cyc_List_List*patvars;struct Cyc_List_List*aquals_bounds;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;void*f1;struct _fat_ptr*f2;int f3;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;void*f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;void*f4;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;void*f1;int f2;struct _fat_ptr*f3;void*f4;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct _tuple15{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 16 "subtype.h"
int Cyc_Subtype_subtype(unsigned,struct Cyc_List_List*,void*,void*);
# 48 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T3;void*_T4;int _T5;int _T6;int _T7;int _T8;unsigned _T9;unsigned _TA;struct Cyc_Absyn_Aggrfield*_TB;struct Cyc_Absyn_Aggrfield*_TC;struct Cyc_Absyn_Aggrfield*_TD;struct Cyc_Core_Opt*_TE;struct Cyc_Core_Opt*_TF;struct Cyc_Core_Opt*_T10;struct Cyc_Absyn_Aggrfield*_T11;struct Cyc_Absyn_Aggrfield*_T12;struct Cyc_Absyn_Aggrfield*_T13;struct Cyc_List_List*_T14;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T15;struct Cyc_Absyn_Pat*_T16;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T17;struct Cyc_Warn_String_Warn_Warg_struct _T18;struct Cyc_Absyn_Pat*_T19;unsigned _T1A;struct _fat_ptr _T1B;void**_T1C;void*_T1D;void*_T1E;int*_T1F;unsigned _T20;void*_T21;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22;void*_T23;int*_T24;int _T25;struct Cyc_Absyn_Pat*_T26;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T27;struct Cyc_Absyn_Pat*_T28;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;struct Cyc_Warn_Typ_Warn_Warg_struct _T2B;void**_T2C;struct Cyc_Absyn_Pat*_T2D;unsigned _T2E;struct _fat_ptr _T2F;void*_T30;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T31;struct Cyc_List_List**_T32;int*_T33;unsigned _T34;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T35;void*_T36;int*_T37;int _T38;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T39;void*_T3A;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T3B;union Cyc_Absyn_AggrInfo _T3C;struct _union_AggrInfo_KnownAggr _T3D;unsigned _T3E;void*_T3F;union Cyc_Absyn_AggrInfo _T40;struct _union_AggrInfo_KnownAggr _T41;struct Cyc_Absyn_Aggrdecl**_T42;struct Cyc_Absyn_Aggrdecl*_T43;struct Cyc_Absyn_AggrdeclImpl*_T44;struct Cyc_Warn_String_Warn_Warg_struct _T45;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T46;struct Cyc_Absyn_Pat*_T47;unsigned _T48;struct _fat_ptr _T49;struct Cyc_Absyn_Pat*_T4A;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T4B;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T4C;struct Cyc_Absyn_Aggrdecl*_T4D;struct Cyc_Absyn_AggrdeclImpl*_T4E;struct Cyc_List_List*_T4F;int _T50;struct Cyc_List_List**_T51;struct Cyc_List_List*_T52;int _T53;struct Cyc_Warn_String_Warn_Warg_struct _T54;struct Cyc_Absyn_Pat*_T55;unsigned _T56;struct _fat_ptr _T57;struct Cyc_Absyn_Aggrdecl*_T58;struct Cyc_Absyn_AggrdeclImpl*_T59;struct Cyc_Absyn_AggrdeclImpl*_T5A;struct Cyc_List_List*_T5B;struct Cyc_List_List**_T5C;struct Cyc_List_List*_T5D;struct Cyc_List_List*_T5E;struct Cyc_List_List**_T5F;struct Cyc_List_List*_T60;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T61;void*_T62;struct Cyc_List_List**_T63;struct Cyc_List_List**_T64;struct Cyc_List_List*_T65;struct Cyc_List_List*_T66;struct Cyc_Warn_String_Warn_Warg_struct _T67;struct Cyc_Warn_Typ_Warn_Warg_struct _T68;int(*_T69)(struct _fat_ptr);void*(*_T6A)(struct _fat_ptr);struct _fat_ptr _T6B;int _T6C;struct Cyc_Warn_String_Warn_Warg_struct _T6D;struct Cyc_Absyn_Pat*_T6E;unsigned _T6F;struct _fat_ptr _T70;struct Cyc_Absyn_Pat*_T71;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T72;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T73;int _T74;struct Cyc_String_pa_PrintArg_struct _T75;struct Cyc_Absyn_Pat*_T76;unsigned _T77;struct _fat_ptr _T78;struct _fat_ptr _T79;struct Cyc_Absyn_Pat*_T7A;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T7B;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T7C;struct Cyc_Absyn_Pat*_T7D;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T7E;unsigned _T7F;struct Cyc_Warn_String_Warn_Warg_struct _T80;int(*_T81)(struct _fat_ptr);void*(*_T82)(struct _fat_ptr);struct _fat_ptr _T83;struct Cyc_Warn_String_Warn_Warg_struct _T84;int(*_T85)(struct _fat_ptr);void*(*_T86)(struct _fat_ptr);struct _fat_ptr _T87;_T0=p;{
void*_T88=_T0->r;struct Cyc_Absyn_Exp*_T89;struct Cyc_List_List*_T8A;int _T8B;void*_T8C;void*_T8D;int _T8E;_T1=(int*)_T88;_T2=*_T1;switch(_T2){case 6: _T3=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T88;_T4=_T3->f1;if(_T4!=0)goto _TL1;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T8F=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T88;_T8E=_T8F->f2;_T8D=_T8F->f3;_T8C=_T8F->f4;_T8B=_T8F->f5;}{int is_tuple=_T8E;struct Cyc_List_List*exist_ts=_T8D;struct Cyc_List_List*dps=_T8C;int dots=_T8B;
# 52
if(topt!=0)goto _TL3;_T5=is_tuple;
if(!_T5)goto _TL5;_T6=dots;if(_T6)goto _TL5;else{goto _TL7;}
_TL7:{struct Cyc_List_List*fields=0;_T7=
Cyc_List_length(dps);{unsigned n=(unsigned)_T7;{
int i=0;_TLB: _T8=i;_T9=(unsigned)_T8;_TA=n;if(_T9 < _TA)goto _TL9;else{goto _TLA;}
_TL9:{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T8F=Cyc_Absyn_tuple_field_designator(i);struct _fat_ptr*_T90;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T91=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T8F;_T90=_T91->f1;}{struct _fat_ptr*n=_T90;
struct Cyc_Absyn_Aggrfield*f;f=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_TB=f;_TB->name=n;_TC=f;_TC->tq=Cyc_Absyn_empty_tqual(0U);_TD=f;_TE=& Cyc_Kinds_mko;_TF=(struct Cyc_Core_Opt*)_TE;_T10=Cyc_Tcenv_lookup_opt_type_vars(te);_TD->type=Cyc_Absyn_new_evar(_TF,_T10);_T11=f;_T11->width=0;_T12=f;_T12->attributes=0;_T13=f;_T13->requires_clause=0;{struct Cyc_List_List*_T91=_cycalloc(sizeof(struct Cyc_List_List));
_T91->hd=f;_T91->tl=fields;_T14=(struct Cyc_List_List*)_T91;}fields=_T14;}}
# 56
i=i + 1;goto _TLB;_TLA:;}
# 61
fields=Cyc_List_imp_rev(fields);{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T8F=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_T8F->tag=7;
_T8F->f1=0U;_T8F->f2=is_tuple;_T8F->f3=fields;_T15=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T8F;}{void*t=(void*)_T15;_T16=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T8F=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T8F->tag=6;
_T8F->f1=t;_T8F->f2=is_tuple;_T8F->f3=exist_ts;_T8F->f4=dps;_T8F->f5=dots;_T17=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T8F;}_T16->r=(void*)_T17;}}}goto _TL6;
# 65
_TL5:{struct Cyc_Warn_String_Warn_Warg_struct _T8F;_T8F.tag=0;_T8F.f1=_tag_fat("cannot determine pattern type",sizeof(char),30U);_T18=_T8F;}{struct Cyc_Warn_String_Warn_Warg_struct _T8F=_T18;void*_T90[1];_T90[0]=& _T8F;_T19=p;_T1A=_T19->loc;_T1B=_tag_fat(_T90,sizeof(void*),1);Cyc_Warn_err2(_T1A,_T1B);}_TL6: goto _TL4;
# 67
_TL3: _T1C=topt;_T1D=*_T1C;{void*type=Cyc_Absyn_compress(_T1D);_T1E=type;_T1F=(int*)_T1E;_T20=*_T1F;switch(_T20){case 0: _T21=type;_T22=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T21;_T23=_T22->f1;_T24=(int*)_T23;_T25=*_T24;if(_T25!=24)goto _TLD;_T26=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T8F=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T8F->tag=6;
# 70
_T8F->f1=type;_T8F->f2=is_tuple;_T8F->f3=exist_ts;_T8F->f4=dps;_T8F->f5=dots;_T27=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T8F;}_T26->r=(void*)_T27;
Cyc_Tcpat_resolve_pat(te,topt,p);goto _LL10;_TLD: goto _LL15;case 7: _T28=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T8F=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T8F->tag=6;
# 74
_T8F->f1=type;_T8F->f2=is_tuple;_T8F->f3=exist_ts;_T8F->f4=dps;_T8F->f5=dots;_T29=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T8F;}_T28->r=(void*)_T29;
Cyc_Tcpat_resolve_pat(te,topt,p);goto _LL10;default: _LL15:{struct Cyc_Warn_String_Warn_Warg_struct _T8F;_T8F.tag=0;
# 78
_T8F.f1=_tag_fat("pattern expects aggregate type instead of ",sizeof(char),43U);_T2A=_T8F;}{struct Cyc_Warn_String_Warn_Warg_struct _T8F=_T2A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T90;_T90.tag=2;_T2C=topt;_T90.f1=*_T2C;_T2B=_T90;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T90=_T2B;void*_T91[2];_T91[0]=& _T8F;_T91[1]=& _T90;_T2D=p;_T2E=_T2D->loc;_T2F=_tag_fat(_T91,sizeof(void*),2);Cyc_Warn_err2(_T2E,_T2F);}}goto _LL10;}_LL10:;}_TL4:
# 82
 return;}_TL1:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T8F=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T88;_T30=_T8F->f1;_T8D=(void*)_T30;_T8E=_T8F->f2;_T31=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T88;_T32=& _T31->f3;_T8C=(struct Cyc_List_List**)_T32;_T8A=_T8F->f4;_T8B=_T8F->f5;}{void*topt=_T8D;int is_tuple=_T8E;struct Cyc_List_List**exist_ts=(struct Cyc_List_List**)_T8C;struct Cyc_List_List*dps=_T8A;int dots=_T8B;
# 85
void*_T8F=Cyc_Absyn_compress(topt);struct Cyc_Absyn_Aggrdecl**_T90;_T33=(int*)_T8F;_T34=*_T33;switch(_T34){case 0: _T35=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8F;_T36=_T35->f1;_T37=(int*)_T36;_T38=*_T37;if(_T38!=24)goto _TL10;_T39=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8F;_T3A=_T39->f1;_T3B=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T3A;_T3C=_T3B->f1;_T3D=_T3C.KnownAggr;_T3E=_T3D.tag;if(_T3E!=2)goto _TL12;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T91=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8F;_T3F=_T91->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T92=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T3F;_T40=_T92->f1;_T41=_T40.KnownAggr;_T90=_T41.val;}}{struct Cyc_Absyn_Aggrdecl**adp=_T90;_T42=adp;{
# 87
struct Cyc_Absyn_Aggrdecl*ad=*_T42;_T43=ad;_T44=_T43->impl;
if(_T44!=0)goto _TL14;{struct Cyc_Warn_String_Warn_Warg_struct _T91;_T91.tag=0;
_T91.f1=_tag_fat("can't destructure an abstract",sizeof(char),30U);_T45=_T91;}{struct Cyc_Warn_String_Warn_Warg_struct _T91=_T45;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T92;_T92.tag=6;_T92.f1=ad;_T46=_T92;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T92=_T46;void*_T93[2];_T93[0]=& _T91;_T93[1]=& _T92;_T47=p;_T48=_T47->loc;_T49=_tag_fat(_T93,sizeof(void*),2);Cyc_Warn_err2(_T48,_T49);}}_T4A=p;_T4B=& Cyc_Absyn_Wild_p_val;_T4C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T4B;
_T4A->r=(void*)_T4C;
return;_TL14: _T4D=ad;_T4E=_T4D->impl;_T4F=_T4E->exist_vars;_T50=
# 93
Cyc_List_length(_T4F);_T51=exist_ts;_T52=*_T51;_T53=Cyc_List_length(_T52);{int more_exists=_T50 - _T53;
if(more_exists >= 0)goto _TL16;{struct Cyc_Warn_String_Warn_Warg_struct _T91;_T91.tag=0;
_T91.f1=_tag_fat("too many existential type variables in pattern",sizeof(char),47U);_T54=_T91;}{struct Cyc_Warn_String_Warn_Warg_struct _T91=_T54;void*_T92[1];_T92[0]=& _T91;_T55=p;_T56=_T55->loc;_T57=_tag_fat(_T92,sizeof(void*),1);Cyc_Warn_err2(_T56,_T57);}{
struct Cyc_List_List**ts=exist_ts;_T58=ad;_T59=_T58->impl;_T5A=
_check_null(_T59);_T5B=_T5A->exist_vars;{int n=Cyc_List_length(_T5B);_TL1B: if(n!=0)goto _TL19;else{goto _TL1A;}
_TL19: _T5C=ts;_T5D=*_T5C;_T5E=_check_null(_T5D);ts=& _T5E->tl;
# 97
n=n + -1;goto _TL1B;_TL1A:;}_T5F=ts;
# 99
*_T5F=0;}goto _TL17;
_TL16: if(more_exists <= 0)goto _TL1C;{
# 102
struct Cyc_List_List*new_ts=0;
_TL21: if(more_exists!=0)goto _TL1F;else{goto _TL20;}
_TL1F:{struct Cyc_List_List*_T91=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T92=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_T92->tag=1;_T92->f1=0;_T61=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T92;}_T62=(void*)_T61;_T91->hd=Cyc_Tcutil_new_tvar(_T62);_T91->tl=new_ts;_T60=(struct Cyc_List_List*)_T91;}new_ts=_T60;
# 103
more_exists=more_exists + -1;goto _TL21;_TL20: _T63=exist_ts;_T64=exist_ts;_T65=*_T64;_T66=new_ts;
# 105
*_T63=Cyc_List_imp_append(_T65,_T66);}goto _TL1D;_TL1C: _TL1D: _TL17:
# 107
 return;}}}_TL12: goto _LL1C;_TL10: goto _LL1C;case 7:
# 109
 return;default: _LL1C:{struct Cyc_Warn_String_Warn_Warg_struct _T91;_T91.tag=0;
_T91.f1=_tag_fat("resolve_pat unknown aggrtype ",sizeof(char),30U);_T67=_T91;}{struct Cyc_Warn_String_Warn_Warg_struct _T91=_T67;{struct Cyc_Warn_Typ_Warn_Warg_struct _T92;_T92.tag=2;_T92.f1=topt;_T68=_T92;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T92=_T68;void*_T93[2];_T93[0]=& _T91;_T93[1]=& _T92;_T6A=Cyc_Warn_impos2;{int(*_T94)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T6A;_T69=_T94;}_T6B=_tag_fat(_T93,sizeof(void*),2);_T69(_T6B);}}};};case 16:{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T8F=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T88;_T89=_T8F->f1;}{struct Cyc_Absyn_Exp*e=_T89;
# 113
Cyc_Tcexp_tcExp(te,0,e);_T6C=
Cyc_Tcutil_is_const_exp(e);if(_T6C)goto _TL22;else{goto _TL24;}
_TL24:{struct Cyc_Warn_String_Warn_Warg_struct _T8F;_T8F.tag=0;_T8F.f1=_tag_fat("non-constant expression in case pattern",sizeof(char),40U);_T6D=_T8F;}{struct Cyc_Warn_String_Warn_Warg_struct _T8F=_T6D;void*_T90[1];_T90[0]=& _T8F;_T6E=p;_T6F=_T6E->loc;_T70=_tag_fat(_T90,sizeof(void*),1);Cyc_Warn_err2(_T6F,_T70);}_T71=p;_T72=& Cyc_Absyn_Wild_p_val;_T73=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T72;
_T71->r=(void*)_T73;
return;_TL22: {
# 119
struct _tuple15 _T8F=Cyc_Evexp_eval_const_uint_exp(e);int _T90;unsigned _T91;_T91=_T8F.f0;_T90=_T8F.f1;{unsigned i=_T91;int known=_T90;_T74=known;
if(_T74)goto _TL25;else{goto _TL27;}
_TL27:{struct Cyc_String_pa_PrintArg_struct _T92;_T92.tag=0;_T92.f1=Cyc_Absynpp_exp2string(e);_T75=_T92;}{struct Cyc_String_pa_PrintArg_struct _T92=_T75;void*_T93[1];_T93[0]=& _T92;_T76=p;_T77=_T76->loc;_T78=_tag_fat("cannot evaluate expression at compile time\nWe suggest using x && x == %s instead",sizeof(char),81U);_T79=_tag_fat(_T93,sizeof(void*),1);Cyc_Warn_err(_T77,_T78,_T79);}_T7A=p;_T7B=& Cyc_Absyn_Wild_p_val;_T7C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T7B;
_T7A->r=(void*)_T7C;
return;_TL25: _T7D=p;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T92=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T92->tag=9;
# 125
_T92->f1=2U;_T7F=i;_T92->f2=(int)_T7F;_T7E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T92;}_T7D->r=(void*)_T7E;
return;}}}case 14:{struct Cyc_Warn_String_Warn_Warg_struct _T8F;_T8F.tag=0;
# 128
_T8F.f1=_tag_fat("resolve_pat UnknownId_p",sizeof(char),24U);_T80=_T8F;}{struct Cyc_Warn_String_Warn_Warg_struct _T8F=_T80;void*_T90[1];_T90[0]=& _T8F;_T82=Cyc_Warn_impos2;{int(*_T91)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T82;_T81=_T91;}_T83=_tag_fat(_T90,sizeof(void*),1);_T81(_T83);}case 15:{struct Cyc_Warn_String_Warn_Warg_struct _T8F;_T8F.tag=0;
_T8F.f1=_tag_fat("resolve_pat UnknownCall_p",sizeof(char),26U);_T84=_T8F;}{struct Cyc_Warn_String_Warn_Warg_struct _T8F=_T84;void*_T90[1];_T90[0]=& _T8F;_T86=Cyc_Warn_impos2;{int(*_T91)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T86;_T85=_T91;}_T87=_tag_fat(_T90,sizeof(void*),1);_T85(_T87);}default:
# 131
 return;};}}
# 135
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){struct Cyc_Absyn_Pat*_T0;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T2;{struct Cyc_Absyn_Pat*_T3=_cycalloc(sizeof(struct Cyc_Absyn_Pat));
_T3->loc=loc;_T3->topt=0;_T1=& Cyc_Absyn_Wild_p_val;_T2=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T1;_T3->r=(void*)_T2;_T0=(struct Cyc_Absyn_Pat*)_T3;}return _T0;}struct _tuple16{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 138
static struct _tuple16*Cyc_Tcpat_wild_dlp(unsigned loc){struct _tuple16*_T0;{struct _tuple16*_T1=_cycalloc(sizeof(struct _tuple16));
_T1->f0=0;_T1->f1=Cyc_Tcpat_wild_pat(loc);_T0=(struct _tuple16*)_T1;}return _T0;}
# 142
static struct Cyc_List_List*Cyc_Tcpat_remove_dots(struct Cyc_List_List*earlier,int num_pats,int num_fields,void*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str){struct Cyc_Warn_String_Warn_Warg_struct _T0;struct Cyc_Warn_String_Warn_Warg_struct _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;unsigned _T3;struct _fat_ptr _T4;int _T5;int _T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;
# 144
if(num_pats < num_fields)goto _TL28;{struct Cyc_Warn_String_Warn_Warg_struct _T9;_T9.tag=0;
_T9.f1=_tag_fat("unnecessary ... in ",sizeof(char),20U);_T0=_T9;}{struct Cyc_Warn_String_Warn_Warg_struct _T9=_T0;{struct Cyc_Warn_String_Warn_Warg_struct _TA;_TA.tag=0;_TA.f1=msg_str;_T1=_TA;}{struct Cyc_Warn_String_Warn_Warg_struct _TA=_T1;{struct Cyc_Warn_String_Warn_Warg_struct _TB;_TB.tag=0;_TB.f1=_tag_fat(" pattern",sizeof(char),9U);_T2=_TB;}{struct Cyc_Warn_String_Warn_Warg_struct _TB=_T2;void*_TC[3];_TC[0]=& _T9;_TC[1]=& _TA;_TC[2]=& _TB;_T3=loc;_T4=_tag_fat(_TC,sizeof(void*),3);Cyc_Warn_warn2(_T3,_T4);}}}goto _TL29;_TL28: _TL29: {
struct Cyc_List_List*new_ps=0;{
int i=0;_TL2D: _T5=i;_T6=num_fields - num_pats;if(_T5 < _T6)goto _TL2B;else{goto _TL2C;}
_TL2B:{struct Cyc_List_List*_T9=_cycalloc(sizeof(struct Cyc_List_List));_T9->hd=f(loc);_T9->tl=new_ps;_T7=(struct Cyc_List_List*)_T9;}new_ps=_T7;
# 147
i=i + 1;goto _TL2D;_TL2C:;}_T8=
# 149
Cyc_List_imp_append(earlier,new_ps);return _T8;}}
# 151
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl*_T0;struct _tuple2*_T1;struct _tuple2 _T2;struct _fat_ptr*_T3;_T0=vd;_T1=_T0->name;_T2=*_T1;_T3=_T2.f1;
return _T3;}
# 154
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){void*_T0;struct Cyc_Core_Opt*_T1;struct Cyc_Core_Opt*_T2;struct Cyc_Core_Opt*_T3;void**_T4;
if(topt!=0)goto _TL2E;_T1=& Cyc_Kinds_mko;_T2=(struct Cyc_Core_Opt*)_T1;{struct Cyc_Core_Opt*_T5=_cycalloc(sizeof(struct Cyc_Core_Opt));_T5->v=s;_T3=(struct Cyc_Core_Opt*)_T5;}_T0=Cyc_Absyn_new_evar(_T2,_T3);goto _TL2F;_TL2E: _T4=topt;_T0=*_T4;_TL2F: return _T0;}
# 157
static void*Cyc_Tcpat_num_type(void**topt,void*numt){void*_T0;void**_T1;void*_T2;int _T3;void**_T4;void*_T5;int*_T6;int _T7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8;void*_T9;int*_TA;unsigned _TB;void**_TC;void*_TD;void*_TE;
# 161
if(topt!=0)goto _TL30;_T0=numt;
return _T0;_TL30: _T1=topt;_T2=*_T1;_T3=
Cyc_Tcutil_is_arithmetic_type(_T2);if(!_T3)goto _TL32;_T4=topt;_T5=*_T4;
return _T5;_TL32: {
void*_TF=Cyc_Absyn_compress(numt);_T6=(int*)_TF;_T7=*_T6;if(_T7!=0)goto _TL34;_T8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF;_T9=_T8->f1;_TA=(int*)_T9;_TB=*_TA;switch(_TB){case 19: goto _LL4;case 20: _LL4: _TC=topt;_TD=*_TC;
# 167
return _TD;default: goto _LL5;}goto _TL35;_TL34: _LL5: _TE=numt;
return _TE;_TL35:;}}struct _tuple17{struct Cyc_Absyn_Vardecl**f0;struct Cyc_Absyn_Exp*f1;};
# 172
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){struct Cyc_Absyn_Vardecl**_T0;struct Cyc_Absyn_Vardecl*_T1;struct Cyc_Absyn_Vardecl**_T2;struct Cyc_Absyn_Vardecl*_T3;struct Cyc_List_List**_T4;struct Cyc_List_List*_T5;struct _tuple17*_T6;struct Cyc_List_List**_T7;
# 174
if(vd==0)goto _TL37;_T0=vd;_T1=*_T0;
_T1->type=t;_T2=vd;_T3=*_T2;
_T3->tq=Cyc_Absyn_empty_tqual(0U);goto _TL38;_TL37: _TL38: _T4=v_result_ptr;{struct Cyc_List_List*_T8=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple17*_T9=_cycalloc(sizeof(struct _tuple17));
# 178
_T9->f0=vd;_T9->f1=e;_T6=(struct _tuple17*)_T9;}_T8->hd=_T6;_T7=v_result_ptr;_T8->tl=*_T7;_T5=(struct Cyc_List_List*)_T8;}*_T4=_T5;}
# 180
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){struct Cyc_Tcpat_TcPatResult _T0;struct Cyc_Tcpat_TcPatResult _T1;struct Cyc_Tcpat_TcPatResult _T2;struct Cyc_Tcpat_TcPatResult _T3;struct Cyc_Tcpat_TcPatResult _T4;struct Cyc_Tcpat_TcPatResult _T5;struct Cyc_Tcpat_TcPatResult _T6;struct _tuple1*_T7;struct _tuple1*_T8;struct Cyc_Tcpat_TcPatResult _T9;struct _tuple1*_TA;struct _tuple1*_TB;struct _tuple1 _TC;struct Cyc_List_List*_TD;struct _tuple1*_TE;struct _tuple1 _TF;struct Cyc_List_List*_T10;struct _tuple1*_T11;struct _tuple1 _T12;struct Cyc_List_List*_T13;struct _tuple1*_T14;struct _tuple1 _T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;struct _tuple1*_T19;_T0=res1;_T19=_T0.tvars_and_effconstr_opt;_T1=res1;_T18=_T1.patvars;_T2=res1;_T17=_T2.aquals_bounds;{struct _tuple1*p1=_T19;struct Cyc_List_List*vs1=_T18;struct Cyc_List_List*aq1=_T17;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct _tuple1*_T1C;_T3=res2;_T1C=_T3.tvars_and_effconstr_opt;_T4=res2;_T1B=_T4.patvars;_T5=res2;_T1A=_T5.aquals_bounds;{struct _tuple1*p2=_T1C;struct Cyc_List_List*vs2=_T1B;struct Cyc_List_List*aq2=_T1A;
# 184
struct Cyc_List_List*aq=Cyc_List_append(aq1,aq2);
if(p1!=0)goto _TL39;if(p2!=0)goto _TL39;{struct Cyc_Tcpat_TcPatResult _T1D;
_T1D.tvars_and_effconstr_opt=0;_T1D.patvars=Cyc_List_append(vs1,vs2);_T1D.aquals_bounds=aq;_T6=_T1D;}return _T6;_TL39:
 if(p1!=0)goto _TL3B;{struct _tuple1*_T1D=_cycalloc(sizeof(struct _tuple1));
_T1D->f0=0;_T1D->f1=0;_T7=(struct _tuple1*)_T1D;}p1=_T7;goto _TL3C;_TL3B: _TL3C:
 if(p2!=0)goto _TL3D;{struct _tuple1*_T1D=_cycalloc(sizeof(struct _tuple1));
_T1D->f0=0;_T1D->f1=0;_T8=(struct _tuple1*)_T1D;}p2=_T8;goto _TL3E;_TL3D: _TL3E:{struct Cyc_Tcpat_TcPatResult _T1D;{struct _tuple1*_T1E=_cycalloc(sizeof(struct _tuple1));_TB=p1;_TC=*_TB;_TD=_TC.f0;_TE=p2;_TF=*_TE;_T10=_TF.f0;
# 192
_T1E->f0=Cyc_List_append(_TD,_T10);_T11=p1;_T12=*_T11;_T13=_T12.f1;_T14=p2;_T15=*_T14;_T16=_T15.f1;
_T1E->f1=Cyc_List_append(_T13,_T16);_TA=(struct _tuple1*)_T1E;}
# 192
_T1D.tvars_and_effconstr_opt=_TA;
# 194
_T1D.patvars=Cyc_List_append(vs1,vs2);
_T1D.aquals_bounds=aq;_T9=_T1D;}
# 192
return _T9;}}}
# 199
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){void*_T0;int _T1;void*_T2;void*_T3;struct Cyc_Core_Opt*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;void*_T7;_T1=
Cyc_Tcutil_is_array_type(t);if(!_T1)goto _TL3F;_T2=t;
if(rgn_opt!=0)goto _TL41;_T4=& Cyc_Kinds_eko;_T5=(struct Cyc_Core_Opt*)_T4;{struct Cyc_Core_Opt*_T8=_cycalloc(sizeof(struct Cyc_Core_Opt));_T8->v=Cyc_Tcenv_lookup_type_vars(te);_T6=(struct Cyc_Core_Opt*)_T8;}_T3=Cyc_Absyn_new_evar(_T5,_T6);goto _TL42;_TL41: _T3=rgn_opt;_TL42: _T7=Cyc_Absyn_al_qual_type;_T0=Cyc_Tcutil_promote_array(_T2,_T3,_T7,0);goto _TL40;_TL3F: _T0=t;_TL40:
# 200
 return _T0;}struct _tuple18{struct Cyc_Absyn_Tvar*f0;int f1;};
# 206
static struct _tuple18*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){struct _tuple18*_T0;{struct _tuple18*_T1=_cycalloc(sizeof(struct _tuple18));
_T1->f0=tv;_T1->f1=0;_T0=(struct _tuple18*)_T1;}return _T0;}struct _tuple19{struct Cyc_Absyn_Aggrfield*f0;struct Cyc_Absyn_Pat*f1;};struct _tuple20{struct Cyc_Absyn_Tqual f0;void*f1;};
# 210
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){struct Cyc_Tcpat_TcPatResult _T0;struct Cyc_Absyn_Pat*_T1;int*_T2;unsigned _T3;struct Cyc_List_List*_T4;void**_T5;struct Cyc_Absyn_Vardecl*_T6;struct _fat_ptr*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;int _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;struct Cyc_Absyn_Pat*_T11;unsigned _T12;struct _fat_ptr _T13;struct Cyc_Absyn_Pat*_T14;void*_T15;int*_T16;int _T17;int _T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;struct Cyc_Absyn_Pat*_T1A;unsigned _T1B;struct _fat_ptr _T1C;struct Cyc_Absyn_Kind*_T1D;struct Cyc_Absyn_Kind*_T1E;struct Cyc_Absyn_Kind*_T1F;int _T20;struct Cyc_Warn_String_Warn_Warg_struct _T21;struct Cyc_Absyn_Pat*_T22;unsigned _T23;struct _fat_ptr _T24;struct Cyc_Absyn_Vardecl**_T25;struct Cyc_Absyn_Exp*_T26;struct Cyc_List_List**_T27;void*_T28;struct Cyc_Absyn_Pat*_T29;unsigned _T2A;struct Cyc_Tcenv_Tenv*_T2B;struct Cyc_List_List*_T2C;struct _fat_ptr _T2D;struct Cyc_Tcpat_TcPatResult _T2E;struct _tuple1*_T2F;struct _tuple1*_T30;struct Cyc_Tcpat_TcPatResult _T31;struct _tuple1*_T32;struct Cyc_Tcpat_TcPatResult _T33;struct _tuple1*_T34;struct _tuple1 _T35;struct Cyc_List_List*_T36;struct Cyc_List_List*_T37;struct _tuple18*_T38;struct Cyc_Absyn_Pat*_T39;unsigned _T3A;struct Cyc_Tcenv_Tenv*_T3B;struct Cyc_List_List*_T3C;struct Cyc_Absyn_Kind*_T3D;struct Cyc_Absyn_Kind*_T3E;struct Cyc_Absyn_Vardecl*_T3F;void*_T40;struct Cyc_Absyn_Vardecl*_T41;int*_T42;int _T43;struct Cyc_Absyn_PtrInfo _T44;struct Cyc_Absyn_PtrAtts _T45;void*_T46;void*_T47;int _T48;struct Cyc_Warn_String_Warn_Warg_struct _T49;struct Cyc_Warn_Typ_Warn_Warg_struct _T4A;struct Cyc_Warn_String_Warn_Warg_struct _T4B;struct Cyc_Warn_Typ_Warn_Warg_struct _T4C;struct Cyc_Absyn_Pat*_T4D;unsigned _T4E;struct _fat_ptr _T4F;struct Cyc_List_List*_T50;void**_T51;int*_T52;int _T53;int _T54;struct Cyc_Warn_String_Warn_Warg_struct _T55;struct Cyc_Absyn_Pat*_T56;unsigned _T57;struct _fat_ptr _T58;struct Cyc_Absyn_Kind*_T59;struct Cyc_Absyn_Kind*_T5A;struct Cyc_Absyn_Kind*_T5B;int _T5C;struct Cyc_Warn_String_Warn_Warg_struct _T5D;struct Cyc_Absyn_Pat*_T5E;unsigned _T5F;struct _fat_ptr _T60;struct Cyc_Absyn_Vardecl**_T61;struct Cyc_Absyn_Exp*_T62;struct Cyc_List_List**_T63;struct Cyc_Absyn_Vardecl*_T64;void*_T65;struct Cyc_Absyn_Pat*_T66;void*_T67;int _T68;struct Cyc_Warn_String_Warn_Warg_struct _T69;struct Cyc_Absyn_Pat*_T6A;unsigned _T6B;struct _fat_ptr _T6C;struct Cyc_List_List*_T6D;void*_T6E;int _T6F;struct Cyc_Warn_String_Warn_Warg_struct _T70;struct Cyc_Absyn_Pat*_T71;unsigned _T72;struct _fat_ptr _T73;struct Cyc_Absyn_Exp*_T74;unsigned _T75;struct Cyc_Absyn_Exp*_T76;struct Cyc_Absyn_Vardecl**_T77;struct Cyc_Absyn_Exp*_T78;struct Cyc_List_List**_T79;void*_T7A;struct Cyc_Absyn_Vardecl**_T7B;struct Cyc_Absyn_Exp*_T7C;struct Cyc_List_List**_T7D;struct Cyc_Absyn_Vardecl*_T7E;void*_T7F;struct Cyc_Absyn_Pat*_T80;unsigned _T81;struct Cyc_Tcenv_Tenv*_T82;struct Cyc_List_List*_T83;struct _fat_ptr _T84;struct Cyc_Tcpat_TcPatResult _T85;struct _tuple1*_T86;struct _tuple1*_T87;struct Cyc_Tcpat_TcPatResult _T88;struct _tuple1*_T89;struct Cyc_List_List*_T8A;struct _tuple18*_T8B;struct Cyc_Tcpat_TcPatResult _T8C;struct _tuple1*_T8D;struct _tuple1 _T8E;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T8F;enum Cyc_Absyn_Sign _T90;void**_T91;int _T92;unsigned _T93;void*_T94;void**_T95;struct Cyc_Absyn_Enumdecl*_T96;struct _tuple2*_T97;struct Cyc_Absyn_Enumdecl*_T98;void*_T99;void*_T9A;void**_T9B;void*_T9C;int*_T9D;int _T9E;void**_T9F;void**_TA0;void*_TA1;int*_TA2;int _TA3;struct Cyc_Absyn_PtrInfo _TA4;struct Cyc_Absyn_PtrInfo _TA5;struct Cyc_Absyn_Tqual _TA6;struct Cyc_Core_Opt*_TA7;struct Cyc_Core_Opt*_TA8;struct Cyc_Core_Opt*_TA9;struct Cyc_Absyn_Exp*_TAA;unsigned _TAB;struct Cyc_Tcpat_TcPatResult _TAC;struct Cyc_Tcpat_TcPatResult _TAD;struct Cyc_Absyn_Pat*_TAE;void*_TAF;void*_TB0;int*_TB1;int _TB2;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB3;void*_TB4;int*_TB5;int _TB6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB7;void*_TB8;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TB9;union Cyc_Absyn_DatatypeFieldInfo _TBA;struct _union_DatatypeFieldInfo_KnownDatatypefield _TBB;unsigned _TBC;void*_TBD;union Cyc_Absyn_DatatypeFieldInfo _TBE;struct _union_DatatypeFieldInfo_KnownDatatypefield _TBF;struct _tuple3 _TC0;union Cyc_Absyn_DatatypeFieldInfo _TC1;struct _union_DatatypeFieldInfo_KnownDatatypefield _TC2;struct _tuple3 _TC3;int*_TC4;int _TC5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC6;void*_TC7;int*_TC8;int _TC9;struct Cyc_Absyn_Datatypedecl**_TCA;union Cyc_Absyn_DatatypeInfo _TCB;struct Cyc_List_List*_TCC;struct Cyc_Absyn_Pat*_TCD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TCE;int _TCF;struct Cyc_List_List*_TD0;struct Cyc_List_List*_TD1;struct Cyc_Core_Opt*_TD2;struct Cyc_Core_Opt*_TD3;struct Cyc_Core_Opt*_TD4;struct Cyc_Absyn_Pat*_TD5;void*_TD6;struct Cyc_Absyn_Tqual _TD7;int _TD8;struct Cyc_Absyn_Exp*_TD9;unsigned _TDA;struct Cyc_Absyn_Exp*_TDB;struct Cyc_Absyn_Pat*_TDC;void*_TDD;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_TDE;struct Cyc_List_List**_TDF;struct Cyc_List_List**_TE0;struct Cyc_Core_Opt*_TE1;int*_TE2;unsigned _TE3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE4;void*_TE5;int*_TE6;int _TE7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE8;void*_TE9;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TEA;union Cyc_Absyn_AggrInfo _TEB;struct _union_AggrInfo_KnownAggr _TEC;unsigned _TED;void*_TEE;struct _union_AggrInfo_KnownAggr _TEF;struct Cyc_Absyn_Aggrdecl**_TF0;struct Cyc_Absyn_Aggrdecl*_TF1;struct Cyc_Absyn_AggrdeclImpl*_TF2;struct Cyc_Warn_String_Warn_Warg_struct _TF3;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TF4;struct Cyc_Absyn_Pat*_TF5;unsigned _TF6;struct _fat_ptr _TF7;struct Cyc_Core_Opt*_TF8;struct Cyc_Absyn_Aggrdecl*_TF9;enum Cyc_Absyn_AggrKind _TFA;int _TFB;struct Cyc_Absyn_Aggrdecl*_TFC;struct Cyc_Absyn_AggrdeclImpl*_TFD;int _TFE;void**_TFF;void*_T100;struct Cyc_Absyn_Kind*_T101;struct Cyc_Absyn_Kind*_T102;struct Cyc_Absyn_Kind*_T103;struct Cyc_Absyn_Aggrdecl*_T104;struct Cyc_Absyn_AggrdeclImpl*_T105;struct Cyc_Absyn_AggrdeclImpl*_T106;struct Cyc_List_List*_T107;void*_T108;struct Cyc_List_List*_T109;void*_T10A;struct Cyc_List_List*_T10B;struct Cyc_Absyn_Tvar*_T10C;void*_T10D;struct Cyc_Absyn_Tvar*_T10E;void*_T10F;void*_T110;int*_T111;unsigned _T112;void*_T113;void*_T114;struct Cyc_Warn_String_Warn_Warg_struct _T115;int(*_T116)(struct _fat_ptr);void*(*_T117)(struct _fat_ptr);struct _fat_ptr _T118;void*_T119;int*_T11A;unsigned _T11B;void*_T11C;int _T11D;struct Cyc_Warn_String_Warn_Warg_struct _T11E;struct Cyc_Warn_Tvar_Warn_Warg_struct _T11F;struct Cyc_Warn_String_Warn_Warg_struct _T120;struct Cyc_Warn_KindBound_Warn_Warg_struct _T121;struct Cyc_Warn_String_Warn_Warg_struct _T122;struct Cyc_Warn_Kind_Warn_Warg_struct _T123;struct Cyc_Absyn_Pat*_T124;unsigned _T125;struct _fat_ptr _T126;void*_T127;void*_T128;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T129;struct Cyc_Core_Opt**_T12A;void*_T12B;void*_T12C;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T12D;struct Cyc_Core_Opt**_T12E;struct Cyc_Core_Opt**_T12F;struct Cyc_Core_Opt*_T130;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T131;struct Cyc_List_List*_T132;struct _RegionHandle*_T133;struct Cyc_List_List*_T134;struct Cyc_Absyn_Pat*_T135;unsigned _T136;struct Cyc_Tcenv_Tenv*_T137;struct Cyc_List_List*_T138;struct _tuple13 _T139;struct Cyc_List_List*(*_T13A)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T13B)(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _RegionHandle*_T13C;struct _tuple14*(*_T13D)(struct _tuple13*,struct Cyc_Absyn_Tvar*);struct _tuple13*_T13E;struct Cyc_Absyn_Aggrdecl*_T13F;struct Cyc_List_List*_T140;struct _RegionHandle*_T141;struct _RegionHandle*_T142;struct Cyc_Absyn_Aggrdecl*_T143;struct Cyc_Absyn_AggrdeclImpl*_T144;struct Cyc_Absyn_AggrdeclImpl*_T145;struct Cyc_List_List*_T146;struct Cyc_List_List*_T147;struct Cyc_List_List*(*_T148)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T149)(void*(*)(void*),struct Cyc_List_List*);void*(*_T14A)(struct _tuple14*);void*(*_T14B)(struct _tuple0*);struct Cyc_List_List*_T14C;struct Cyc_List_List*(*_T14D)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T14E)(void*(*)(void*),struct Cyc_List_List*);void*(*_T14F)(struct _tuple14*);void*(*_T150)(struct _tuple0*);struct Cyc_List_List*_T151;struct Cyc_Absyn_Aggrdecl*_T152;struct Cyc_Absyn_AggrdeclImpl*_T153;struct Cyc_Absyn_AggrdeclImpl*_T154;struct Cyc_List_List*_T155;struct Cyc_Tcpat_TcPatResult _T156;struct _tuple1*_T157;struct _tuple1*_T158;struct Cyc_Tcpat_TcPatResult _T159;struct _tuple1*_T15A;struct Cyc_Tcpat_TcPatResult _T15B;struct _tuple1*_T15C;struct _tuple1 _T15D;struct Cyc_List_List*_T15E;struct Cyc_List_List*(*_T15F)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*);struct Cyc_List_List*(*_T160)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T161;struct Cyc_List_List*_T162;struct _RegionHandle*_T163;struct Cyc_List_List*_T164;struct Cyc_Absyn_Aggrdecl*_T165;struct Cyc_Absyn_AggrdeclImpl*_T166;struct Cyc_Absyn_AggrdeclImpl*_T167;struct Cyc_List_List*_T168;struct Cyc_Tcpat_TcPatResult _T169;struct _tuple1*_T16A;struct Cyc_Tcpat_TcPatResult _T16B;struct _tuple1*_T16C;struct _tuple1 _T16D;struct Cyc_List_List*_T16E;struct Cyc_List_List*_T16F;struct Cyc_Absyn_Aggrdecl*_T170;struct Cyc_Absyn_AggrdeclImpl*_T171;struct Cyc_Absyn_AggrdeclImpl*_T172;struct _tuple0 _T173;struct _RegionHandle*_T174;struct Cyc_List_List*_T175;struct Cyc_List_List*_T176;void*_T177;struct _tuple0*_T178;struct _tuple0 _T179;void*_T17A;struct _RegionHandle*_T17B;struct Cyc_List_List*_T17C;struct Cyc_List_List*_T17D;void*_T17E;struct _tuple0*_T17F;struct _tuple0 _T180;void*_T181;struct Cyc_List_List*_T182;struct _tuple0*_T183;struct Cyc_List_List*_T184;struct Cyc_Absyn_Aggrdecl**_T185;union Cyc_Absyn_AggrInfo _T186;struct Cyc_List_List*_T187;int _T188;struct Cyc_Absyn_Aggrdecl*_T189;enum Cyc_Absyn_AggrKind _T18A;int _T18B;struct Cyc_Absyn_Pat*_T18C;unsigned _T18D;struct _fat_ptr _T18E;struct _fat_ptr _T18F;int _T190;struct Cyc_List_List**_T191;struct Cyc_List_List*(*_T192)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr);struct Cyc_List_List*_T193;int _T194;struct Cyc_Absyn_Aggrdecl*_T195;struct Cyc_Absyn_AggrdeclImpl*_T196;struct Cyc_Absyn_AggrdeclImpl*_T197;struct Cyc_List_List*_T198;int _T199;struct Cyc_Absyn_Pat*_T19A;unsigned _T19B;struct _fat_ptr _T19C;struct Cyc_List_List**_T19D;struct _RegionHandle*_T19E;struct Cyc_Absyn_Pat*_T19F;unsigned _T1A0;struct Cyc_List_List*_T1A1;struct Cyc_Absyn_Aggrdecl*_T1A2;enum Cyc_Absyn_AggrKind _T1A3;struct Cyc_Absyn_Aggrdecl*_T1A4;struct Cyc_Absyn_AggrdeclImpl*_T1A5;struct Cyc_Absyn_AggrdeclImpl*_T1A6;struct Cyc_List_List*_T1A7;struct Cyc_List_List*_T1A8;void*_T1A9;struct _RegionHandle*_T1AA;struct Cyc_List_List*_T1AB;struct Cyc_Absyn_Aggrfield*_T1AC;void*_T1AD;struct Cyc_Absyn_Exp*_T1AE;unsigned _T1AF;struct Cyc_Absyn_Exp*_T1B0;struct Cyc_Absyn_Aggrfield*_T1B1;struct _fat_ptr*_T1B2;struct Cyc_Tcpat_TcPatResult _T1B3;struct Cyc_Tcenv_Tenv*_T1B4;struct Cyc_Absyn_Pat*_T1B5;void**_T1B6;void*_T1B7;int _T1B8;struct Cyc_Absyn_Exp*_T1B9;struct Cyc_Tcpat_TcPatResult _T1BA;struct Cyc_Absyn_Pat*_T1BB;unsigned _T1BC;struct Cyc_Absyn_Pat*_T1BD;void*_T1BE;void*_T1BF;void*_T1C0;int _T1C1;struct Cyc_Warn_String_Warn_Warg_struct _T1C2;struct Cyc_Warn_String_Warn_Warg_struct _T1C3;struct Cyc_Absyn_Aggrfield*_T1C4;struct _fat_ptr*_T1C5;struct Cyc_Warn_String_Warn_Warg_struct _T1C6;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T1C7;struct Cyc_Warn_String_Warn_Warg_struct _T1C8;struct Cyc_Warn_Typ_Warn_Warg_struct _T1C9;struct Cyc_Warn_String_Warn_Warg_struct _T1CA;struct Cyc_Warn_Typ_Warn_Warg_struct _T1CB;struct Cyc_Absyn_Pat*_T1CC;void*_T1CD;struct Cyc_Absyn_Pat*_T1CE;unsigned _T1CF;struct _fat_ptr _T1D0;struct Cyc_Absyn_Exp*_T1D1;unsigned _T1D2;struct Cyc_Absyn_Exp*_T1D3;struct Cyc_Absyn_Pat*_T1D4;struct Cyc_List_List*_T1D5;struct Cyc_Warn_String_Warn_Warg_struct _T1D6;struct Cyc_Absyn_Pat*_T1D7;unsigned _T1D8;struct _fat_ptr _T1D9;int _T1DA;enum Cyc_Absyn_AggrKind _T1DB;int _T1DC;struct Cyc_Absyn_Pat*_T1DD;unsigned _T1DE;struct _fat_ptr _T1DF;struct _fat_ptr _T1E0;int _T1E1;struct Cyc_List_List**_T1E2;struct Cyc_List_List*(*_T1E3)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr);struct Cyc_List_List*_T1E4;int _T1E5;int _T1E6;struct Cyc_Absyn_Pat*_T1E7;unsigned _T1E8;struct _fat_ptr _T1E9;int _T1EA;struct Cyc_List_List**_T1EB;struct _RegionHandle*_T1EC;struct Cyc_Absyn_Pat*_T1ED;unsigned _T1EE;struct Cyc_List_List*_T1EF;enum Cyc_Absyn_AggrKind _T1F0;struct Cyc_List_List*_T1F1;struct Cyc_List_List*_T1F2;void*_T1F3;struct Cyc_Absyn_Aggrfield*_T1F4;struct Cyc_Absyn_Exp*_T1F5;unsigned _T1F6;struct Cyc_Absyn_Exp*_T1F7;struct Cyc_Absyn_Aggrfield*_T1F8;struct _fat_ptr*_T1F9;struct Cyc_Tcpat_TcPatResult _T1FA;struct Cyc_Tcenv_Tenv*_T1FB;struct Cyc_Absyn_Pat*_T1FC;void**_T1FD;void*_T1FE;int _T1FF;struct Cyc_Absyn_Exp*_T200;struct Cyc_Tcpat_TcPatResult _T201;struct Cyc_Absyn_Pat*_T202;unsigned _T203;struct Cyc_Absyn_Pat*_T204;void*_T205;void*_T206;void*_T207;int _T208;struct Cyc_Warn_String_Warn_Warg_struct _T209;struct Cyc_Warn_String_Warn_Warg_struct _T20A;struct Cyc_Absyn_Aggrfield*_T20B;struct _fat_ptr*_T20C;struct Cyc_Warn_String_Warn_Warg_struct _T20D;struct Cyc_Warn_Typ_Warn_Warg_struct _T20E;struct Cyc_Warn_String_Warn_Warg_struct _T20F;struct Cyc_Warn_Typ_Warn_Warg_struct _T210;struct Cyc_Absyn_Pat*_T211;void*_T212;struct Cyc_Absyn_Pat*_T213;unsigned _T214;struct _fat_ptr _T215;struct Cyc_Absyn_Exp*_T216;unsigned _T217;struct Cyc_Absyn_Exp*_T218;struct Cyc_Absyn_Pat*_T219;struct Cyc_List_List*_T21A;struct Cyc_Warn_String_Warn_Warg_struct _T21B;int(*_T21C)(struct _fat_ptr);void*(*_T21D)(struct _fat_ptr);struct _fat_ptr _T21E;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T21F;struct Cyc_List_List**_T220;struct Cyc_List_List**_T221;struct Cyc_Absyn_Datatypefield*_T222;struct _tuple13 _T223;struct Cyc_List_List*(*_T224)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T225)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple14*(*_T226)(struct _tuple13*,struct Cyc_Absyn_Tvar*);struct _tuple13*_T227;struct Cyc_Absyn_Datatypedecl*_T228;struct Cyc_List_List*_T229;struct Cyc_List_List*(*_T22A)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T22B)(void*(*)(void*),struct Cyc_List_List*);void*(*_T22C)(struct _tuple14*);void*(*_T22D)(struct _tuple0*);struct Cyc_List_List*_T22E;union Cyc_Absyn_DatatypeFieldInfo _T22F;struct Cyc_List_List*_T230;int _T231;struct Cyc_List_List**_T232;struct Cyc_List_List*(*_T233)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr);struct Cyc_List_List*_T234;int _T235;int _T236;struct Cyc_Absyn_Pat*_T237;unsigned _T238;struct _fat_ptr _T239;struct Cyc_List_List**_T23A;struct Cyc_List_List*_T23B;void*_T23C;struct Cyc_List_List*_T23D;struct Cyc_List_List*_T23E;void*_T23F;struct _tuple20*_T240;struct _tuple20 _T241;void*_T242;struct Cyc_Absyn_Exp*_T243;unsigned _T244;struct Cyc_Absyn_Exp*_T245;struct Cyc_List_List**_T246;void*_T247;struct Cyc_Tcpat_TcPatResult _T248;struct Cyc_Tcenv_Tenv*_T249;struct Cyc_Absyn_Pat*_T24A;void**_T24B;void*_T24C;int _T24D;struct Cyc_Tcpat_TcPatResult _T24E;struct Cyc_Absyn_Pat*_T24F;unsigned _T250;struct Cyc_Absyn_Pat*_T251;void*_T252;void*_T253;void*_T254;int _T255;struct Cyc_Warn_Qvar_Warn_Warg_struct _T256;struct Cyc_Absyn_Datatypefield*_T257;struct Cyc_Warn_String_Warn_Warg_struct _T258;struct Cyc_Warn_Typ_Warn_Warg_struct _T259;struct Cyc_Warn_String_Warn_Warg_struct _T25A;struct Cyc_Warn_Typ_Warn_Warg_struct _T25B;struct Cyc_Absyn_Pat*_T25C;void*_T25D;struct Cyc_Absyn_Pat*_T25E;unsigned _T25F;struct _fat_ptr _T260;struct Cyc_List_List*_T261;struct Cyc_List_List*_T262;struct Cyc_Warn_String_Warn_Warg_struct _T263;struct Cyc_Warn_Qvar_Warn_Warg_struct _T264;struct Cyc_Absyn_Datatypefield*_T265;struct Cyc_Absyn_Pat*_T266;unsigned _T267;struct _fat_ptr _T268;struct Cyc_Warn_String_Warn_Warg_struct _T269;struct Cyc_Warn_Qvar_Warn_Warg_struct _T26A;struct Cyc_Absyn_Datatypefield*_T26B;struct Cyc_Absyn_Pat*_T26C;unsigned _T26D;struct _fat_ptr _T26E;struct Cyc_Core_Opt*_T26F;struct Cyc_Absyn_Pat*_T270;struct Cyc_Tcpat_TcPatResult _T271;
# 213
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;{struct Cyc_Tcpat_TcPatResult _T272;
_T272.tvars_and_effconstr_opt=0;_T272.patvars=0;_T272.aquals_bounds=0;_T0=_T272;}{struct Cyc_Tcpat_TcPatResult res=_T0;_T1=p;{
# 218
void*_T272=_T1->r;struct Cyc_Absyn_Datatypefield*_T273;struct Cyc_Absyn_Datatypedecl*_T274;int _T275;struct Cyc_List_List*_T276;int _T277;void*_T278;void*_T279;_T2=(int*)_T272;_T3=*_T2;switch(_T3){case 0: _T4=
# 220
Cyc_Tcenv_lookup_type_vars(te);_T5=topt;t=Cyc_Tcpat_any_type(_T4,_T5);goto _LL0;case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T272;_T279=_T27A->f1;_T278=_T27A->f2;}{struct Cyc_Absyn_Vardecl*vd=_T279;struct Cyc_Absyn_Pat*p2=_T278;_T6=vd;{
# 224
struct _tuple2*_T27A=_T6->name;struct _fat_ptr _T27B;{struct _tuple2 _T27C=*_T27A;_T7=_T27C.f1;{struct _fat_ptr _T27D=*_T7;_T27B=_T27D;}}{struct _fat_ptr v=_T27B;_T8=v;_T9=
_tag_fat("true",sizeof(char),5U);_TA=Cyc_strcmp(_T8,_T9);if(_TA==0)goto _TL46;else{goto _TL47;}_TL47: _TB=v;_TC=_tag_fat("false",sizeof(char),6U);_TD=Cyc_strcmp(_TB,_TC);if(_TD==0)goto _TL46;else{goto _TL44;}
_TL46:{struct Cyc_Warn_String_Warn_Warg_struct _T27C;_T27C.tag=0;_T27C.f1=_tag_fat("you probably do not want to use ",sizeof(char),33U);_TE=_T27C;}{struct Cyc_Warn_String_Warn_Warg_struct _T27C=_TE;{struct Cyc_Warn_String_Warn_Warg_struct _T27D;_T27D.tag=0;_T27D.f1=v;_TF=_T27D;}{struct Cyc_Warn_String_Warn_Warg_struct _T27D=_TF;{struct Cyc_Warn_String_Warn_Warg_struct _T27E;_T27E.tag=0;
_T27E.f1=_tag_fat(" as a local variable",sizeof(char),21U);_T10=_T27E;}{struct Cyc_Warn_String_Warn_Warg_struct _T27E=_T10;void*_T27F[3];_T27F[0]=& _T27C;_T27F[1]=& _T27D;_T27F[2]=& _T27E;_T11=p;_T12=_T11->loc;_T13=_tag_fat(_T27F,sizeof(void*),3);Cyc_Warn_warn2(_T12,_T13);}}}goto _TL45;_TL44: _TL45:
 res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);_T14=p2;_T15=_T14->topt;
t=_check_null(_T15);{
# 232
void*_T27C=Cyc_Absyn_compress(t);_T16=(int*)_T27C;_T17=*_T16;if(_T17!=5)goto _TL48;
# 234
if(rgn_pat==0)goto _TL4C;else{goto _TL4D;}_TL4D: _T18=allow_ref_pat;if(_T18)goto _TL4A;else{goto _TL4C;}
_TL4C:{struct Cyc_Warn_String_Warn_Warg_struct _T27D;_T27D.tag=0;_T27D.f1=_tag_fat("array reference would point into unknown/unallowed region",sizeof(char),58U);_T19=_T27D;}{struct Cyc_Warn_String_Warn_Warg_struct _T27D=_T19;void*_T27E[1];_T27E[0]=& _T27D;_T1A=p;_T1B=_T1A->loc;_T1C=_tag_fat(_T27E,sizeof(void*),1);Cyc_Warn_err2(_T1B,_T1C);}goto _TL4B;_TL4A: _TL4B: goto _LL28;_TL48: _T1D=
# 238
Cyc_Tcutil_type_kind(t);_T1E=& Cyc_Kinds_mk;_T1F=(struct Cyc_Absyn_Kind*)_T1E;_T20=Cyc_Kinds_kind_leq(_T1D,_T1F);if(_T20)goto _TL4E;else{goto _TL50;}
_TL50:{struct Cyc_Warn_String_Warn_Warg_struct _T27D;_T27D.tag=0;_T27D.f1=_tag_fat("pattern would point to an abstract member",sizeof(char),42U);_T21=_T27D;}{struct Cyc_Warn_String_Warn_Warg_struct _T27D=_T21;void*_T27E[1];_T27E[0]=& _T27D;_T22=p;_T23=_T22->loc;_T24=_tag_fat(_T27E,sizeof(void*),1);Cyc_Warn_err2(_T23,_T24);}goto _TL4F;_TL4E: _TL4F: goto _LL28;_LL28:;}{struct Cyc_Absyn_Vardecl**_T27C=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));
# 242
*_T27C=vd;_T25=(struct Cyc_Absyn_Vardecl**)_T27C;}_T26=access_exp;_T27=& res.patvars;_T28=Cyc_Tcpat_pat_promote_array(te,t,rgn_pat);Cyc_Tcpat_set_vd(_T25,_T26,_T27,_T28);goto _LL0;}}}case 2:{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T272;_T279=_T27A->f1;_T278=_T27A->f2;}{struct Cyc_Absyn_Tvar*tv=_T279;struct Cyc_Absyn_Vardecl*vd=_T278;_T29=p;_T2A=_T29->loc;_T2B=te;{struct Cyc_Absyn_Tvar*_T27A[1];_T27A[0]=tv;_T2D=_tag_fat(_T27A,sizeof(struct Cyc_Absyn_Tvar*),1);_T2C=Cyc_List_list(_T2D);}{
# 245
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_T2A,_T2B,_T2C);_T2E=res;_T2F=_T2E.tvars_and_effconstr_opt;
if(_T2F!=0)goto _TL51;{struct _tuple1*_T27A=_cycalloc(sizeof(struct _tuple1));
_T27A->f0=0;_T27A->f1=0;_T30=(struct _tuple1*)_T27A;}res.tvars_and_effconstr_opt=_T30;goto _TL52;_TL51: _TL52: _T31=res;_T32=_T31.tvars_and_effconstr_opt;_T33=res;_T34=_T33.tvars_and_effconstr_opt;_T35=*_T34;_T36=_T35.f0;{struct Cyc_List_List*_T27A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T27B=_cycalloc(sizeof(struct _tuple18));
# 249
_T27B->f0=tv;_T27B->f1=1;_T38=(struct _tuple18*)_T27B;}_T27A->hd=_T38;_T27A->tl=0;_T37=(struct Cyc_List_List*)_T27A;}
# 248
(*_T32).f0=
Cyc_List_append(_T36,_T37);_T39=p;_T3A=_T39->loc;_T3B=te2;_T3C=
Cyc_Tcenv_lookup_type_vars(te2);_T3D=& Cyc_Kinds_mk;_T3E=(struct Cyc_Absyn_Kind*)_T3D;_T3F=vd;_T40=_T3F->type;Cyc_Tctyp_check_type(_T3A,_T3B,_T3C,_T3E,1,0,_T40);_T41=vd;{
void*_T27A=_T41->type;void*_T27B;_T42=(int*)_T27A;_T43=*_T42;if(_T43!=4)goto _TL53;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T27C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T27A;_T44=_T27C->f1;_T45=_T44.ptr_atts;_T27B=_T45.eff;}{void*eff=_T27B;
# 253
void*atv=Cyc_Absyn_var_type(tv);_T46=eff;_T47=
Cyc_Absyn_var_type(tv);_T48=Cyc_Unify_unify(_T46,_T47);if(_T48)goto _TL55;else{goto _TL57;}
_TL57:{struct Cyc_Warn_String_Warn_Warg_struct _T27C;_T27C.tag=0;_T27C.f1=_tag_fat("Mismatched effect variables in alias pattern:",sizeof(char),46U);_T49=_T27C;}{struct Cyc_Warn_String_Warn_Warg_struct _T27C=_T49;{struct Cyc_Warn_Typ_Warn_Warg_struct _T27D;_T27D.tag=2;_T27D.f1=eff;_T4A=_T27D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T27D=_T4A;{struct Cyc_Warn_String_Warn_Warg_struct _T27E;_T27E.tag=0;_T27E.f1=_tag_fat(", ",sizeof(char),3U);_T4B=_T27E;}{struct Cyc_Warn_String_Warn_Warg_struct _T27E=_T4B;{struct Cyc_Warn_Typ_Warn_Warg_struct _T27F;_T27F.tag=2;_T27F.f1=atv;_T4C=_T27F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T27F=_T4C;void*_T280[4];_T280[0]=& _T27C;_T280[1]=& _T27D;_T280[2]=& _T27E;_T280[3]=& _T27F;_T4D=p;_T4E=_T4D->loc;_T4F=_tag_fat(_T280,sizeof(void*),4);Cyc_Warn_err2(_T4E,_T4F);}}}}goto _TL56;_TL55: _TL56: goto _LL2D;}_TL53: goto _LL2D;_LL2D:;}_T50=
# 262
Cyc_Tcenv_lookup_type_vars(te);_T51=topt;t=Cyc_Tcpat_any_type(_T50,_T51);{
void*_T27A=Cyc_Absyn_compress(t);_T52=(int*)_T27A;_T53=*_T52;if(_T53!=5)goto _TL58;
# 265
if(rgn_pat==0)goto _TL5C;else{goto _TL5D;}_TL5D: _T54=allow_ref_pat;if(_T54)goto _TL5A;else{goto _TL5C;}
_TL5C:{struct Cyc_Warn_String_Warn_Warg_struct _T27B;_T27B.tag=0;_T27B.f1=_tag_fat("array reference would point into unknown/unallowed region",sizeof(char),58U);_T55=_T27B;}{struct Cyc_Warn_String_Warn_Warg_struct _T27B=_T55;void*_T27C[1];_T27C[0]=& _T27B;_T56=p;_T57=_T56->loc;_T58=_tag_fat(_T27C,sizeof(void*),1);Cyc_Warn_err2(_T57,_T58);}goto _TL5B;_TL5A: _TL5B: goto _LL32;_TL58: _T59=
# 269
Cyc_Tcutil_type_kind(t);_T5A=& Cyc_Kinds_mk;_T5B=(struct Cyc_Absyn_Kind*)_T5A;_T5C=Cyc_Kinds_kind_leq(_T59,_T5B);if(_T5C)goto _TL5E;else{goto _TL60;}
_TL60:{struct Cyc_Warn_String_Warn_Warg_struct _T27B;_T27B.tag=0;_T27B.f1=_tag_fat("pattern would point to an abstract member",sizeof(char),42U);_T5D=_T27B;}{struct Cyc_Warn_String_Warn_Warg_struct _T27B=_T5D;void*_T27C[1];_T27C[0]=& _T27B;_T5E=p;_T5F=_T5E->loc;_T60=_tag_fat(_T27C,sizeof(void*),1);Cyc_Warn_err2(_T5F,_T60);}goto _TL5F;_TL5E: _TL5F: goto _LL32;_LL32:;}{struct Cyc_Absyn_Vardecl**_T27A=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));
# 273
*_T27A=vd;_T61=(struct Cyc_Absyn_Vardecl**)_T27A;}_T62=access_exp;_T63=& res.patvars;_T64=vd;_T65=_T64->type;Cyc_Tcpat_set_vd(_T61,_T62,_T63,_T65);goto _LL0;}}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T272;_T279=_T27A->f1;_T278=_T27A->f2;}{struct Cyc_Absyn_Vardecl*vd=_T279;struct Cyc_Absyn_Pat*p2=_T278;
# 277
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);_T66=p2;_T67=_T66->topt;
t=_check_null(_T67);_T68=allow_ref_pat;
if(_T68)goto _TL64;else{goto _TL63;}_TL64: if(rgn_pat==0)goto _TL63;else{goto _TL61;}
_TL63:{struct Cyc_Warn_String_Warn_Warg_struct _T27A;_T27A.tag=0;_T27A.f1=_tag_fat("* pattern would point into an unknown/unallowed region",sizeof(char),55U);_T69=_T27A;}{struct Cyc_Warn_String_Warn_Warg_struct _T27A=_T69;void*_T27B[1];_T27B[0]=& _T27A;_T6A=p;_T6B=_T6A->loc;_T6C=_tag_fat(_T27B,sizeof(void*),1);Cyc_Warn_err2(_T6B,_T6C);}goto _LL0;
# 282
_TL61: _T6D=Cyc_Tcenv_curr_aquals_bounds(te);_T6E=t;_T6F=Cyc_Tcutil_is_noalias_pointer_or_aggr(_T6D,_T6E);if(!_T6F)goto _TL65;{struct Cyc_Warn_String_Warn_Warg_struct _T27A;_T27A.tag=0;
_T27A.f1=_tag_fat("* pattern cannot take the address of an alias-free path",sizeof(char),56U);_T70=_T27A;}{struct Cyc_Warn_String_Warn_Warg_struct _T27A=_T70;void*_T27B[1];_T27B[0]=& _T27A;_T71=p;_T72=_T71->loc;_T73=_tag_fat(_T27B,sizeof(void*),1);Cyc_Warn_err2(_T72,_T73);}goto _TL66;_TL65: _TL66: {
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.elt_type=t;
pi.ptr_atts.eff=rgn_pat;
pi.ptr_atts.nullable=Cyc_Absyn_false_type;{
void*t2=Cyc_Absyn_pointer_type(pi);_T74=access_exp;_T75=(unsigned)_T74;
if(!_T75)goto _TL67;
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);_T76=new_access_exp;
_T76->topt=t2;goto _TL68;_TL67: _TL68:{struct Cyc_Absyn_Vardecl**_T27A=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));
# 294
*_T27A=vd;_T77=(struct Cyc_Absyn_Vardecl**)_T27A;}_T78=new_access_exp;_T79=& res.patvars;_T7A=t2;Cyc_Tcpat_set_vd(_T77,_T78,_T79,_T7A);goto _LL0;}}}case 4:{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T272;_T279=_T27A->f1;_T278=_T27A->f2;}{struct Cyc_Absyn_Tvar*tv=_T279;struct Cyc_Absyn_Vardecl*vd=_T278;{struct Cyc_Absyn_Vardecl**_T27A=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));
# 300
*_T27A=vd;_T7B=(struct Cyc_Absyn_Vardecl**)_T27A;}_T7C=access_exp;_T7D=& res.patvars;_T7E=vd;_T7F=_T7E->type;Cyc_Tcpat_set_vd(_T7B,_T7C,_T7D,_T7F);_T80=p;_T81=_T80->loc;_T82=te;{struct Cyc_Absyn_Tvar*_T27A[1];_T27A[0]=tv;_T84=_tag_fat(_T27A,sizeof(struct Cyc_Absyn_Tvar*),1);_T83=Cyc_List_list(_T84);}
# 304
Cyc_Tcenv_add_type_vars(_T81,_T82,_T83);_T85=res;_T86=_T85.tvars_and_effconstr_opt;
if(_T86!=0)goto _TL69;{struct _tuple1*_T27A=_cycalloc(sizeof(struct _tuple1));
_T27A->f0=0;_T27A->f1=0;_T87=(struct _tuple1*)_T27A;}res.tvars_and_effconstr_opt=_T87;goto _TL6A;_TL69: _TL6A: _T88=res;_T89=_T88.tvars_and_effconstr_opt;{struct Cyc_List_List*_T27A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T27B=_cycalloc(sizeof(struct _tuple18));
# 308
_T27B->f0=tv;_T27B->f1=0;_T8B=(struct _tuple18*)_T27B;}_T27A->hd=_T8B;_T8C=res;_T8D=_T8C.tvars_and_effconstr_opt;_T8E=*_T8D;_T27A->tl=_T8E.f0;_T8A=(struct Cyc_List_List*)_T27A;}
# 307
(*_T89).f0=_T8A;
# 309
t=Cyc_Absyn_uint_type;goto _LL0;}case 9: _T8F=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T272;_T90=_T8F->f1;if(_T90!=Cyc_Absyn_Unsigned)goto _TL6B;
# 312
t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;_TL6B:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;case 10:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T272;_T277=_T27A->f2;}{int i=_T277;_T91=topt;_T92=i;_T93=(unsigned)_T92;_T94=
Cyc_Absyn_gen_float_type(_T93);t=Cyc_Tcpat_num_type(_T91,_T94);goto _LL0;}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T272;_T279=_T27A->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T279;_T95=topt;_T96=ed;_T97=_T96->name;_T98=ed;_T99=
Cyc_Absyn_enum_type(_T97,_T98);t=Cyc_Tcpat_num_type(_T95,_T99);goto _LL0;}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T272;_T9A=_T27A->f1;_T279=(void*)_T9A;}{void*tenum=_T279;
t=Cyc_Tcpat_num_type(topt,tenum);goto _LL0;}case 8:
# 319
 if(topt==0)goto _TL6D;_T9B=topt;_T9C=*_T9B;{
void*_T27A=Cyc_Absyn_compress(_T9C);_T9D=(int*)_T27A;_T9E=*_T9D;if(_T9E!=4)goto _TL6F;_T9F=topt;
t=*_T9F;goto tcpat_end;_TL6F: goto _LL37;_LL37:;}goto _TL6E;_TL6D: _TL6E: {
# 324
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.ptr_atts.nullable=Cyc_Absyn_true_type;
t=Cyc_Absyn_pointer_type(pi);goto _LL0;}case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T272;_T279=_T27A->f1;}{struct Cyc_Absyn_Pat*p2=_T279;
# 332
void*inner_typ=Cyc_Absyn_void_type;
void**inner_topt=0;
int elt_const=0;
if(topt==0)goto _TL71;_TA0=topt;_TA1=*_TA0;{
void*_T27A=Cyc_Absyn_compress(_TA1);struct Cyc_Absyn_Tqual _T27B;void*_T27C;_TA2=(int*)_T27A;_TA3=*_TA2;if(_TA3!=4)goto _TL73;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T27D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T27A;_TA4=_T27D->f1;_T27C=_TA4.elt_type;_TA5=_T27D->f1;_T27B=_TA5.elt_tq;}{void*elt_typ=_T27C;struct Cyc_Absyn_Tqual elt_tq=_T27B;
# 338
inner_typ=elt_typ;
inner_topt=& inner_typ;_TA6=elt_tq;
elt_const=_TA6.real_const;goto _LL3C;}_TL73: goto _LL3C;_LL3C:;}goto _TL72;_TL71: _TL72: _TA7=& Cyc_Kinds_eko;_TA8=(struct Cyc_Core_Opt*)_TA7;_TA9=
# 347
Cyc_Tcenv_lookup_opt_type_vars(te);{void*ptr_rgn=Cyc_Absyn_new_evar(_TA8,_TA9);
struct Cyc_Absyn_Exp*new_access_exp=0;_TAA=access_exp;_TAB=(unsigned)_TAA;
if(!_TAB)goto _TL75;new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);goto _TL76;_TL75: _TL76: _TAC=res;_TAD=
Cyc_Tcpat_tcPatRec(te,p2,inner_topt,ptr_rgn,1,new_access_exp);res=Cyc_Tcpat_combine_results(_TAC,_TAD);_TAE=p2;_TAF=_TAE->topt;_TB0=
# 356
_check_null(_TAF);{void*_T27A=Cyc_Absyn_compress(_TB0);struct Cyc_List_List*_T27B;struct Cyc_Absyn_Datatypefield*_T27C;struct Cyc_Absyn_Datatypedecl*_T27D;_TB1=(int*)_T27A;_TB2=*_TB1;if(_TB2!=0)goto _TL77;_TB3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27A;_TB4=_TB3->f1;_TB5=(int*)_TB4;_TB6=*_TB5;if(_TB6!=23)goto _TL79;_TB7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27A;_TB8=_TB7->f1;_TB9=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TB8;_TBA=_TB9->f1;_TBB=_TBA.KnownDatatypefield;_TBC=_TBB.tag;if(_TBC!=2)goto _TL7B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T27E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27A;_TBD=_T27E->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T27F=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TBD;_TBE=_T27F->f1;_TBF=_TBE.KnownDatatypefield;_TC0=_TBF.val;_T27D=_TC0.f0;_TC1=_T27F->f1;_TC2=_TC1.KnownDatatypefield;_TC3=_TC2.val;_T27C=_TC3.f1;}_T27B=_T27E->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T27D;struct Cyc_Absyn_Datatypefield*tuf=_T27C;struct Cyc_List_List*targs=_T27B;{
# 360
void*_T27E=Cyc_Absyn_compress(inner_typ);_TC4=(int*)_T27E;_TC5=*_TC4;if(_TC5!=0)goto _TL7D;_TC6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27E;_TC7=_TC6->f1;_TC8=(int*)_TC7;_TC9=*_TC8;if(_TC9!=23)goto _TL7F;goto DONT_PROMOTE;_TL7F: goto _LL49;_TL7D: _LL49: goto _LL46;_LL46:;}{struct Cyc_Absyn_Datatypedecl**_T27E=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));
# 366
*_T27E=tud;_TCA=(struct Cyc_Absyn_Datatypedecl**)_T27E;}_TCB=Cyc_Absyn_KnownDatatype(_TCA);_TCC=targs;{void*new_type=Cyc_Absyn_datatype_type(_TCB,_TCC);_TCD=p2;
# 368
_TCD->topt=new_type;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T27E=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct));_T27E->tag=4;
_T27E->f1.elt_type=new_type;_TCF=elt_const;
if(!_TCF)goto _TL81;_T27E->f1.elt_tq=Cyc_Absyn_const_tqual(0U);goto _TL82;_TL81:
 _T27E->f1.elt_tq=Cyc_Absyn_empty_tqual(0U);_TL82:
 _T27E->f1.ptr_atts.eff=ptr_rgn;_TD0=Cyc_Tcenv_lookup_type_vars(te);_T27E->f1.ptr_atts.nullable=Cyc_Tcutil_any_bool(_TD0);
_T27E->f1.ptr_atts.bounds=Cyc_Absyn_bounds_one();_T27E->f1.ptr_atts.zero_term=Cyc_Absyn_false_type;
_T27E->f1.ptr_atts.ptrloc=0;_TD1=Cyc_Tcenv_lookup_type_vars(te);_T27E->f1.ptr_atts.autoreleased=Cyc_Tcutil_any_bool(_TD1);_TD2=& Cyc_Kinds_aqko;_TD3=(struct Cyc_Core_Opt*)_TD2;_TD4=
Cyc_Tcenv_lookup_opt_type_vars(te);_T27E->f1.ptr_atts.aqual=Cyc_Absyn_new_evar(_TD3,_TD4);_TCE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T27E;}
# 369
t=(void*)_TCE;goto _LL41;}}_TL7B: goto _LL44;_TL79: goto _LL44;_TL77: _LL44:
# 378
 DONT_PROMOTE:{
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);_TD5=p2;_TD6=_TD5->topt;
pi.elt_type=_check_null(_TD6);_TD8=elt_const;
if(!_TD8)goto _TL83;_TD7=Cyc_Absyn_const_tqual(0U);goto _TL84;_TL83: _TD7=Cyc_Absyn_empty_tqual(0U);_TL84: pi.elt_tq=_TD7;
pi.ptr_atts.eff=ptr_rgn;
t=Cyc_Absyn_pointer_type(pi);}_LL41:;}_TD9=new_access_exp;_TDA=(unsigned)_TD9;
# 385
if(!_TDA)goto _TL85;_TDB=new_access_exp;_TDC=p2;_TDB->topt=_TDC->topt;goto _TL86;_TL85: _TL86: goto _LL0;}}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T272;_TDD=_T27A->f1;_T279=(void*)_TDD;_T277=_T27A->f2;_T276=_T27A->f3;_TDE=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T272;_TDF=& _TDE->f4;_T278=(struct Cyc_List_List**)_TDF;_T275=_T27A->f5;}{void*atype=_T279;int is_tuple=_T277;struct Cyc_List_List*exist_ts=_T276;struct Cyc_List_List**dps_p=(struct Cyc_List_List**)_T278;int dots=_T275;_TE0=dps_p;{
# 389
struct Cyc_List_List*dps=*_TE0;
if(atype!=0)goto _TL87;_TE1=
# 392
Cyc_Tcenv_lookup_opt_type_vars(te);t=Cyc_Absyn_wildtyp(_TE1);goto _LL0;_TL87:
# 395
 t=atype;{
void*_T27A=Cyc_Absyn_compress(t);struct Cyc_List_List*_T27B;int _T27C;enum Cyc_Absyn_AggrKind _T27D;struct Cyc_Absyn_Aggrdecl*_T27E;union Cyc_Absyn_AggrInfo _T27F;_TE2=(int*)_T27A;_TE3=*_TE2;switch(_TE3){case 0: _TE4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27A;_TE5=_TE4->f1;_TE6=(int*)_TE5;_TE7=*_TE6;if(_TE7!=24)goto _TL8A;_TE8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27A;_TE9=_TE8->f1;_TEA=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TE9;_TEB=_TEA->f1;_TEC=_TEB.KnownAggr;_TED=_TEC.tag;if(_TED!=2)goto _TL8C;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T280=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27A;_TEE=_T280->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T281=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TEE;_T27F=_T281->f1;_TEF=_T27F.KnownAggr;_TF0=_TEF.val;{struct Cyc_Absyn_Aggrdecl*_T282=*_TF0;_T27E=_T282;}}}{union Cyc_Absyn_AggrInfo ainfo=_T27F;struct Cyc_Absyn_Aggrdecl*ad=_T27E;_TF1=ad;_TF2=_TF1->impl;
# 398
if(_TF2!=0)goto _TL8E;{struct Cyc_Warn_String_Warn_Warg_struct _T280;_T280.tag=0;
_T280.f1=_tag_fat("can't destructure abstract ",sizeof(char),28U);_TF3=_T280;}{struct Cyc_Warn_String_Warn_Warg_struct _T280=_TF3;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T281;_T281.tag=6;_T281.f1=ad;_TF4=_T281;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T281=_TF4;void*_T282[2];_T282[0]=& _T280;_T282[1]=& _T281;_TF5=p;_TF6=_TF5->loc;_TF7=_tag_fat(_T282,sizeof(void*),2);Cyc_Warn_err2(_TF6,_TF7);}}_TF8=
Cyc_Tcenv_lookup_opt_type_vars(te);t=Cyc_Absyn_wildtyp(_TF8);goto _LL4B;_TL8E: _TF9=ad;_TFA=_TF9->kind;_TFB=(int)_TFA;
# 405
if(_TFB!=1)goto _TL90;_TFC=ad;_TFD=_TFC->impl;_TFE=_TFD->tagged;if(!_TFE)goto _TL90;
allow_ref_pat=0;goto _TL91;_TL90: _TL91:
 if(exist_ts==0)goto _TL92;
# 411
if(topt==0)goto _TL96;else{goto _TL97;}_TL97: _TFF=topt;_T100=*_TFF;_T101=Cyc_Tcutil_type_kind(_T100);_T102=& Cyc_Kinds_ak;_T103=(struct Cyc_Absyn_Kind*)_T102;if(_T101!=_T103)goto _TL96;else{goto _TL94;}
_TL96: allow_ref_pat=0;goto _TL95;_TL94: _TL95: goto _TL93;_TL92: _TL93:{struct _RegionHandle _T280=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _T280;_push_region(rgn);{
# 416
struct Cyc_List_List*var_tvs=0;_T104=ad;_T105=_T104->impl;_T106=
_check_null(_T105);{struct Cyc_List_List*u=_T106->exist_vars;{
struct Cyc_List_List*t=exist_ts;_TL9B: if(t!=0)goto _TL99;else{goto _TL9A;}
_TL99: _T107=t;_T108=_T107->hd;{struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_T108;_T109=
_check_null(u);_T10A=_T109->hd;{struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)_T10A;_T10B=u;
u=_T10B->tl;_T10C=tv;_T10D=_T10C->kind;{
void*kb1=Cyc_Kinds_compress_kb(_T10D);_T10E=uv;_T10F=_T10E->kind;{
void*kb2=Cyc_Kinds_compress_kb(_T10F);
struct Cyc_Absyn_Kind*k2;{struct Cyc_Absyn_Kind*_T281;_T110=kb2;_T111=(int*)_T110;_T112=*_T111;switch(_T112){case 2: _T113=kb2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T282=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T113;_T281=_T282->f2;}{struct Cyc_Absyn_Kind*k=_T281;_T281=k;goto _LL56;}case 0: _T114=kb2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T282=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T114;_T281=_T282->f1;}_LL56: {struct Cyc_Absyn_Kind*k=_T281;
# 427
k2=k;goto _LL52;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T282;_T282.tag=0;
_T282.f1=_tag_fat("unconstrained existential tyvar in aggregate",sizeof(char),45U);_T115=_T282;}{struct Cyc_Warn_String_Warn_Warg_struct _T282=_T115;void*_T283[1];_T283[0]=& _T282;_T117=Cyc_Warn_impos2;{int(*_T284)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T117;_T116=_T284;}_T118=_tag_fat(_T283,sizeof(void*),1);_T116(_T118);}}_LL52:;}{struct Cyc_Absyn_Kind*_T281;void*_T282;_T119=kb1;_T11A=(int*)_T119;_T11B=*_T11A;switch(_T11B){case 0: _T11C=kb1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T283=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T11C;_T282=_T283->f1;}{struct Cyc_Absyn_Kind*k1=_T282;_T11D=
# 433
Cyc_Kinds_kind_leq(k2,k1);if(_T11D)goto _TL9E;else{goto _TLA0;}
_TLA0:{struct Cyc_Warn_String_Warn_Warg_struct _T283;_T283.tag=0;_T283.f1=_tag_fat("type variable ",sizeof(char),15U);_T11E=_T283;}{struct Cyc_Warn_String_Warn_Warg_struct _T283=_T11E;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T284;_T284.tag=7;_T284.f1=tv;_T11F=_T284;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T284=_T11F;{struct Cyc_Warn_String_Warn_Warg_struct _T285;_T285.tag=0;_T285.f1=_tag_fat(" has kind ",sizeof(char),11U);_T120=_T285;}{struct Cyc_Warn_String_Warn_Warg_struct _T285=_T120;{struct Cyc_Warn_KindBound_Warn_Warg_struct _T286;_T286.tag=8;_T286.f1=kb1;_T121=_T286;}{struct Cyc_Warn_KindBound_Warn_Warg_struct _T286=_T121;{struct Cyc_Warn_String_Warn_Warg_struct _T287;_T287.tag=0;
_T287.f1=_tag_fat(" but must have at least kind ",sizeof(char),30U);_T122=_T287;}{struct Cyc_Warn_String_Warn_Warg_struct _T287=_T122;{struct Cyc_Warn_Kind_Warn_Warg_struct _T288;_T288.tag=9;_T288.f1=k2;_T123=_T288;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T288=_T123;void*_T289[6];_T289[0]=& _T283;_T289[1]=& _T284;_T289[2]=& _T285;_T289[3]=& _T286;_T289[4]=& _T287;_T289[5]=& _T288;_T124=p;_T125=_T124->loc;_T126=_tag_fat(_T289,sizeof(void*),6);Cyc_Warn_err2(_T125,_T126);}}}}}}goto _TL9F;_TL9E: _TL9F: goto _LL59;}case 2: _T127=kb1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T283=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T127;_T128=kb1;_T129=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T128;_T12A=& _T129->f1;_T282=(struct Cyc_Core_Opt**)_T12A;_T281=_T283->f2;}{struct Cyc_Core_Opt**f=_T282;struct Cyc_Absyn_Kind*k=_T281;_T282=f;goto _LL5F;}default: _T12B=kb1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T283=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T12B;_T12C=kb1;_T12D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T12C;_T12E=& _T12D->f1;_T282=(struct Cyc_Core_Opt**)_T12E;}_LL5F: {struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T282;_T12F=f;{struct Cyc_Core_Opt*_T283=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 438
_T283->v=kb2;_T130=(struct Cyc_Core_Opt*)_T283;}*_T12F=_T130;goto _LL59;}}_LL59:;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T281=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T281->tag=2;
# 440
_T281->f1=tv;_T131=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T281;}{void*vartype=(void*)_T131;_T133=rgn;{struct Cyc_List_List*_T281=_region_malloc(_T133,0U,sizeof(struct Cyc_List_List));
_T281->hd=vartype;_T281->tl=var_tvs;_T132=(struct Cyc_List_List*)_T281;}var_tvs=_T132;}}}}}_T134=t;
# 418
t=_T134->tl;goto _TL9B;_TL9A:;}
# 454 "tcpat.cyc"
var_tvs=Cyc_List_imp_rev(var_tvs);_T135=p;_T136=_T135->loc;_T137=te;_T138=exist_ts;{
# 456
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_T136,_T137,_T138);{struct _tuple13 _T281;
# 458
_T281.f0=Cyc_Tcenv_lookup_type_vars(te2);_T281.f1=rgn;_T139=_T281;}{struct _tuple13 env=_T139;_T13B=Cyc_List_rmap_c;{
struct Cyc_List_List*(*_T281)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))_T13B;_T13A=_T281;}_T13C=rgn;_T13D=Cyc_Tcutil_r_make_inst_var;_T13E=& env;_T13F=ad;_T140=_T13F->tvs;{struct Cyc_List_List*all_inst=_T13A(_T13C,_T13D,_T13E,_T140);_T141=rgn;_T142=rgn;_T143=ad;_T144=_T143->impl;_T145=
_check_null(_T144);_T146=_T145->exist_vars;_T147=var_tvs;{struct Cyc_List_List*exist_inst=Cyc_List_rzip(_T141,_T142,_T146,_T147);_T149=Cyc_List_map;{
struct Cyc_List_List*(*_T281)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T149;_T148=_T281;}_T14B=Cyc_Core_snd;{void*(*_T281)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T14B;_T14A=_T281;}_T14C=all_inst;{struct Cyc_List_List*all_typs=_T148(_T14A,_T14C);_T14E=Cyc_List_map;{
struct Cyc_List_List*(*_T281)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T14E;_T14D=_T281;}_T150=Cyc_Core_snd;{void*(*_T281)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T150;_T14F=_T281;}_T151=exist_inst;{struct Cyc_List_List*exist_typs=_T14D(_T14F,_T151);
struct Cyc_List_List*inst=Cyc_List_rappend(rgn,all_inst,exist_inst);
# 465
if(exist_ts!=0)goto _TLA3;else{goto _TLA4;}_TLA4: _T152=ad;_T153=_T152->impl;_T154=_check_null(_T153);_T155=_T154->effconstr;if(_T155!=0)goto _TLA3;else{goto _TLA1;}
_TLA3: _T156=res;_T157=_T156.tvars_and_effconstr_opt;if(_T157!=0)goto _TLA5;{struct _tuple1*_T281=_cycalloc(sizeof(struct _tuple1));
_T281->f0=0;_T281->f1=0;_T158=(struct _tuple1*)_T281;}res.tvars_and_effconstr_opt=_T158;goto _TLA6;_TLA5: _TLA6: _T159=res;_T15A=_T159.tvars_and_effconstr_opt;_T15B=res;_T15C=_T15B.tvars_and_effconstr_opt;_T15D=*_T15C;_T15E=_T15D.f0;_T160=Cyc_List_map;{
# 469
struct Cyc_List_List*(*_T281)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))_T160;_T15F=_T281;}_T161=exist_ts;_T162=_T15F(Cyc_Tcpat_add_false,_T161);
# 468
(*_T15A).f0=
Cyc_List_append(_T15E,_T162);_T163=rgn;_T164=inst;_T165=ad;_T166=_T165->impl;_T167=
# 472
_check_null(_T166);_T168=_T167->effconstr;{struct Cyc_List_List*ec_inst=Cyc_Tcutil_rsubst_effconstr(_T163,_T164,_T168);_T169=res;_T16A=_T169.tvars_and_effconstr_opt;_T16B=res;_T16C=_T16B.tvars_and_effconstr_opt;_T16D=*_T16C;_T16E=_T16D.f1;_T16F=ec_inst;
# 474
(*_T16A).f1=
Cyc_List_append(_T16E,_T16F);{
struct Cyc_List_List*aqb_inst=0;_T170=ad;_T171=_T170->impl;_T172=
_check_null(_T171);{struct Cyc_List_List*aqb=_T172->qual_bnd;_TLAA: if(aqb!=0)goto _TLA8;else{goto _TLA9;}
_TLA8:{struct _tuple0 _T281;_T174=rgn;_T175=inst;_T176=aqb;_T177=_T176->hd;_T178=(struct _tuple0*)_T177;_T179=*_T178;_T17A=_T179.f0;_T281.f0=Cyc_Tcutil_rsubstitute(_T174,_T175,_T17A);_T17B=rgn;_T17C=inst;_T17D=aqb;_T17E=_T17D->hd;_T17F=(struct _tuple0*)_T17E;_T180=*_T17F;_T181=_T180.f1;
_T281.f1=Cyc_Tcutil_rsubstitute(_T17B,_T17C,_T181);_T173=_T281;}{
# 478
struct _tuple0 _T281=_T173;void*_T282;void*_T283;_T283=_T281.f0;_T282=_T281.f1;{void*tv=_T283;void*b=_T282;{struct Cyc_List_List*_T284=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T285=_cycalloc(sizeof(struct _tuple0));
# 480
_T285->f0=tv;_T285->f1=b;_T183=(struct _tuple0*)_T285;}_T284->hd=_T183;_T284->tl=aqb_inst;_T182=(struct Cyc_List_List*)_T284;}aqb_inst=_T182;}}_T184=aqb;
# 477
aqb=_T184->tl;goto _TLAA;_TLA9:;}
# 482
aqb_inst=Cyc_List_imp_rev(aqb_inst);
res.aquals_bounds=aqb_inst;}}goto _TLA2;_TLA1: _TLA2:{struct Cyc_Absyn_Aggrdecl**_T281=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 487
*_T281=ad;_T185=(struct Cyc_Absyn_Aggrdecl**)_T281;}_T186=Cyc_Absyn_KnownAggr(_T185);_T187=all_typs;t=Cyc_Absyn_aggr_type(_T186,_T187);_T188=dots;
if(!_T188)goto _TLAB;_T189=ad;_T18A=_T189->kind;_T18B=(int)_T18A;if(_T18B!=1)goto _TLAB;_T18C=p;_T18D=_T18C->loc;_T18E=
_tag_fat("`...' pattern not allowed in union pattern",sizeof(char),43U);_T18F=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T18D,_T18E,_T18F);goto _TLAC;
_TLAB: _T190=dots;if(!_T190)goto _TLAD;_T191=dps_p;{
struct Cyc_List_List*(*_T281)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_T192=_T281;}_T193=dps;_T194=Cyc_List_length(dps);_T195=ad;_T196=_T195->impl;_T197=_check_null(_T196);_T198=_T197->fields;_T199=Cyc_List_length(_T198);_T19A=p;_T19B=_T19A->loc;_T19C=
_tag_fat("struct",sizeof(char),7U);
# 491
*_T191=_T192(_T193,_T194,_T199,Cyc_Tcpat_wild_dlp,_T19B,_T19C);_T19D=dps_p;
# 493
dps=*_T19D;goto _TLAE;_TLAD: _TLAE: _TLAC: _T19E=rgn;_T19F=p;_T1A0=_T19F->loc;_T1A1=dps;_T1A2=ad;_T1A3=_T1A2->kind;_T1A4=ad;_T1A5=_T1A4->impl;_T1A6=
# 496
_check_null(_T1A5);_T1A7=_T1A6->fields;{
# 495
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(_T19E,_T1A0,_T1A1,_T1A3,_T1A7);
_TLB2: if(fields!=0)goto _TLB0;else{goto _TLB1;}
_TLB0: _T1A8=fields;_T1A9=_T1A8->hd;{struct _tuple19*_T281=(struct _tuple19*)_T1A9;struct Cyc_Absyn_Pat*_T282;struct Cyc_Absyn_Aggrfield*_T283;{struct _tuple19 _T284=*_T281;_T283=_T284.f0;_T282=_T284.f1;}{struct Cyc_Absyn_Aggrfield*field=_T283;struct Cyc_Absyn_Pat*pat=_T282;_T1AA=rgn;_T1AB=inst;_T1AC=field;_T1AD=_T1AC->type;{
void*inst_fieldtyp=Cyc_Tcutil_rsubstitute(_T1AA,_T1AB,_T1AD);
# 501
struct Cyc_Absyn_Exp*new_access_exp=0;_T1AE=access_exp;_T1AF=(unsigned)_T1AE;
if(!_T1AF)goto _TLB3;_T1B0=access_exp;_T1B1=field;_T1B2=_T1B1->name;
new_access_exp=Cyc_Absyn_aggrmember_exp(_T1B0,_T1B2,0U);goto _TLB4;_TLB3: _TLB4: _T1B3=res;_T1B4=te2;_T1B5=pat;_T1B6=& inst_fieldtyp;_T1B7=rgn_pat;_T1B8=allow_ref_pat;_T1B9=new_access_exp;_T1BA=
Cyc_Tcpat_tcPatRec(_T1B4,_T1B5,_T1B6,_T1B7,_T1B8,_T1B9);res=Cyc_Tcpat_combine_results(_T1B3,_T1BA);_T1BB=p;_T1BC=_T1BB->loc;_T1BD=pat;_T1BE=_T1BD->topt;_T1BF=
# 509
_check_null(_T1BE);_T1C0=inst_fieldtyp;_T1C1=Cyc_Unify_unify_and_constrain(_T1BC,_T1BF,_T1C0);if(_T1C1)goto _TLB5;else{goto _TLB7;}
_TLB7:{struct Cyc_Warn_String_Warn_Warg_struct _T284;_T284.tag=0;_T284.f1=_tag_fat("field ",sizeof(char),7U);_T1C2=_T284;}{struct Cyc_Warn_String_Warn_Warg_struct _T284=_T1C2;{struct Cyc_Warn_String_Warn_Warg_struct _T285;_T285.tag=0;_T1C4=field;_T1C5=_T1C4->name;_T285.f1=*_T1C5;_T1C3=_T285;}{struct Cyc_Warn_String_Warn_Warg_struct _T285=_T1C3;{struct Cyc_Warn_String_Warn_Warg_struct _T286;_T286.tag=0;_T286.f1=_tag_fat(" of ",sizeof(char),5U);_T1C6=_T286;}{struct Cyc_Warn_String_Warn_Warg_struct _T286=_T1C6;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T287;_T287.tag=6;_T287.f1=ad;_T1C7=_T287;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T287=_T1C7;{struct Cyc_Warn_String_Warn_Warg_struct _T288;_T288.tag=0;
_T288.f1=_tag_fat(" pattern expects type ",sizeof(char),23U);_T1C8=_T288;}{struct Cyc_Warn_String_Warn_Warg_struct _T288=_T1C8;{struct Cyc_Warn_Typ_Warn_Warg_struct _T289;_T289.tag=2;_T289.f1=inst_fieldtyp;_T1C9=_T289;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T289=_T1C9;{struct Cyc_Warn_String_Warn_Warg_struct _T28A;_T28A.tag=0;_T28A.f1=_tag_fat(" != ",sizeof(char),5U);_T1CA=_T28A;}{struct Cyc_Warn_String_Warn_Warg_struct _T28A=_T1CA;{struct Cyc_Warn_Typ_Warn_Warg_struct _T28B;_T28B.tag=2;_T1CC=pat;_T1CD=_T1CC->topt;
_T28B.f1=_check_null(_T1CD);_T1CB=_T28B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T28B=_T1CB;void*_T28C[8];_T28C[0]=& _T284;_T28C[1]=& _T285;_T28C[2]=& _T286;_T28C[3]=& _T287;_T28C[4]=& _T288;_T28C[5]=& _T289;_T28C[6]=& _T28A;_T28C[7]=& _T28B;_T1CE=p;_T1CF=_T1CE->loc;_T1D0=_tag_fat(_T28C,sizeof(void*),8);Cyc_Warn_err2(_T1CF,_T1D0);}}}}}}}}goto _TLB6;_TLB5: _TLB6: _T1D1=new_access_exp;_T1D2=(unsigned)_T1D1;
if(!_T1D2)goto _TLB8;_T1D3=new_access_exp;_T1D4=pat;_T1D3->topt=_T1D4->topt;goto _TLB9;_TLB8: _TLB9:;}}}_T1D5=fields;
# 497
fields=_T1D5->tl;goto _TLB2;_TLB1:;}}}}}}}}}_pop_region();}goto _LL4B;}_TL8C: goto _LL50;_TL8A: goto _LL50;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T280=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T27A;_T27D=_T280->f1;_T27C=_T280->f2;_T27B=_T280->f3;}{enum Cyc_Absyn_AggrKind ak=_T27D;int is_tuple=_T27C;struct Cyc_List_List*afs=_T27B;
# 519
if(exist_ts==0)goto _TLBA;{struct Cyc_Warn_String_Warn_Warg_struct _T280;_T280.tag=0;
_T280.f1=_tag_fat("bad existentially-bound type variables",sizeof(char),39U);_T1D6=_T280;}{struct Cyc_Warn_String_Warn_Warg_struct _T280=_T1D6;void*_T281[1];_T281[0]=& _T280;_T1D7=p;_T1D8=_T1D7->loc;_T1D9=_tag_fat(_T281,sizeof(void*),1);Cyc_Warn_err2(_T1D8,_T1D9);}goto _TLBB;_TLBA: _TLBB: _T1DA=dots;
if(!_T1DA)goto _TLBC;_T1DB=ak;_T1DC=(int)_T1DB;if(_T1DC!=1)goto _TLBC;_T1DD=p;_T1DE=_T1DD->loc;_T1DF=
_tag_fat("`...' patterns not allowed in union pattern",sizeof(char),44U);_T1E0=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T1DE,_T1DF,_T1E0);goto _TLBD;
_TLBC: _T1E1=dots;if(!_T1E1)goto _TLBE;_T1E2=dps_p;{
struct Cyc_List_List*(*_T280)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_T1E3=_T280;}_T1E4=dps;_T1E5=Cyc_List_length(dps);_T1E6=Cyc_List_length(afs);_T1E7=p;_T1E8=_T1E7->loc;_T1EA=is_tuple;
if(!_T1EA)goto _TLC0;_T1E9=_tag_fat("tuple",sizeof(char),6U);goto _TLC1;_TLC0: _T1E9=_tag_fat("struct",sizeof(char),7U);_TLC1:
# 524
*_T1E2=_T1E3(_T1E4,_T1E5,_T1E6,Cyc_Tcpat_wild_dlp,_T1E8,_T1E9);_T1EB=dps_p;
# 526
dps=*_T1EB;goto _TLBF;_TLBE: _TLBF: _TLBD:{struct _RegionHandle _T280=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _T280;_push_region(rgn);_T1EC=rgn;_T1ED=p;_T1EE=_T1ED->loc;_T1EF=dps;_T1F0=ak;_T1F1=afs;{
# 529
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(_T1EC,_T1EE,_T1EF,_T1F0,_T1F1);
_TLC5: if(fields!=0)goto _TLC3;else{goto _TLC4;}
_TLC3: _T1F2=fields;_T1F3=_T1F2->hd;{struct _tuple19*_T281=(struct _tuple19*)_T1F3;struct Cyc_Absyn_Pat*_T282;struct Cyc_Absyn_Aggrfield*_T283;{struct _tuple19 _T284=*_T281;_T283=_T284.f0;_T282=_T284.f1;}{struct Cyc_Absyn_Aggrfield*field=_T283;struct Cyc_Absyn_Pat*pat=_T282;_T1F4=field;{
void*fieldtyp=_T1F4->type;
struct Cyc_Absyn_Exp*new_access_exp=0;_T1F5=access_exp;_T1F6=(unsigned)_T1F5;
if(!_T1F6)goto _TLC6;_T1F7=access_exp;_T1F8=field;_T1F9=_T1F8->name;
new_access_exp=Cyc_Absyn_aggrmember_exp(_T1F7,_T1F9,0U);goto _TLC7;_TLC6: _TLC7: _T1FA=res;_T1FB=te;_T1FC=pat;_T1FD=& fieldtyp;_T1FE=rgn_pat;_T1FF=allow_ref_pat;_T200=new_access_exp;_T201=
Cyc_Tcpat_tcPatRec(_T1FB,_T1FC,_T1FD,_T1FE,_T1FF,_T200);res=Cyc_Tcpat_combine_results(_T1FA,_T201);_T202=p;_T203=_T202->loc;_T204=pat;_T205=_T204->topt;_T206=
# 542
_check_null(_T205);_T207=fieldtyp;_T208=Cyc_Unify_unify_and_constrain(_T203,_T206,_T207);if(_T208)goto _TLC8;else{goto _TLCA;}
_TLCA:{struct Cyc_Warn_String_Warn_Warg_struct _T284;_T284.tag=0;_T284.f1=_tag_fat("field ",sizeof(char),7U);_T209=_T284;}{struct Cyc_Warn_String_Warn_Warg_struct _T284=_T209;{struct Cyc_Warn_String_Warn_Warg_struct _T285;_T285.tag=0;_T20B=field;_T20C=_T20B->name;_T285.f1=*_T20C;_T20A=_T285;}{struct Cyc_Warn_String_Warn_Warg_struct _T285=_T20A;{struct Cyc_Warn_String_Warn_Warg_struct _T286;_T286.tag=0;
_T286.f1=_tag_fat(" pattern expects type ",sizeof(char),23U);_T20D=_T286;}{struct Cyc_Warn_String_Warn_Warg_struct _T286=_T20D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T287;_T287.tag=2;_T287.f1=fieldtyp;_T20E=_T287;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T287=_T20E;{struct Cyc_Warn_String_Warn_Warg_struct _T288;_T288.tag=0;_T288.f1=_tag_fat(" != ",sizeof(char),5U);_T20F=_T288;}{struct Cyc_Warn_String_Warn_Warg_struct _T288=_T20F;{struct Cyc_Warn_Typ_Warn_Warg_struct _T289;_T289.tag=2;_T211=pat;_T212=_T211->topt;
_T289.f1=_check_null(_T212);_T210=_T289;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T289=_T210;void*_T28A[6];_T28A[0]=& _T284;_T28A[1]=& _T285;_T28A[2]=& _T286;_T28A[3]=& _T287;_T28A[4]=& _T288;_T28A[5]=& _T289;_T213=p;_T214=_T213->loc;_T215=_tag_fat(_T28A,sizeof(void*),6);Cyc_Warn_err2(_T214,_T215);}}}}}}goto _TLC9;_TLC8: _TLC9: _T216=new_access_exp;_T217=(unsigned)_T216;
if(!_T217)goto _TLCB;_T218=new_access_exp;_T219=pat;_T218->topt=_T219->topt;goto _TLCC;_TLCB: _TLCC:;}}}_T21A=fields;
# 531
fields=_T21A->tl;goto _TLC5;_TLC4:;}_pop_region();}goto _LL4B;}default: _LL50:{struct Cyc_Warn_String_Warn_Warg_struct _T280;_T280.tag=0;
# 550
_T280.f1=_tag_fat("bad type in tcpat",sizeof(char),18U);_T21B=_T280;}{struct Cyc_Warn_String_Warn_Warg_struct _T280=_T21B;void*_T281[1];_T281[0]=& _T280;_T21D=Cyc_Warn_impos2;{int(*_T282)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T21D;_T21C=_T282;}_T21E=_tag_fat(_T281,sizeof(void*),1);_T21C(_T21E);}}_LL4B:;}goto _LL0;}}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T27A=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T272;_T274=_T27A->f1;_T273=_T27A->f2;_T21F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T272;_T220=& _T21F->f3;_T279=(struct Cyc_List_List**)_T220;_T277=_T27A->f4;}{struct Cyc_Absyn_Datatypedecl*tud=_T274;struct Cyc_Absyn_Datatypefield*tuf=_T273;struct Cyc_List_List**ps_p=(struct Cyc_List_List**)_T279;int dots=_T277;_T221=ps_p;{
# 555
struct Cyc_List_List*ps=*_T221;_T222=tuf;{
struct Cyc_List_List*tqts=_T222->typs;{struct _tuple13 _T27A;
# 558
_T27A.f0=Cyc_Tcenv_lookup_type_vars(te);_T27A.f1=Cyc_Core_heap_region;_T223=_T27A;}{struct _tuple13 env=_T223;_T225=Cyc_List_map_c;{
struct Cyc_List_List*(*_T27A)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))_T225;_T224=_T27A;}_T226=Cyc_Tcutil_r_make_inst_var;_T227=& env;_T228=tud;_T229=_T228->tvs;{struct Cyc_List_List*inst=_T224(_T226,_T227,_T229);_T22B=Cyc_List_map;{
struct Cyc_List_List*(*_T27A)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T22B;_T22A=_T27A;}_T22D=Cyc_Core_snd;{void*(*_T27A)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T22D;_T22C=_T27A;}_T22E=inst;{struct Cyc_List_List*all_typs=_T22A(_T22C,_T22E);_T22F=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_T230=all_typs;t=Cyc_Absyn_datatype_field_type(_T22F,_T230);_T231=dots;
if(!_T231)goto _TLCD;_T232=ps_p;{
struct Cyc_List_List*(*_T27A)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_T233=_T27A;}_T234=ps;_T235=Cyc_List_length(ps);_T236=Cyc_List_length(tqts);_T237=p;_T238=_T237->loc;_T239=
_tag_fat("datatype field",sizeof(char),15U);
# 563
*_T232=_T233(_T234,_T235,_T236,Cyc_Tcpat_wild_pat,_T238,_T239);_T23A=ps_p;
# 565
ps=*_T23A;goto _TLCE;_TLCD: _TLCE:
# 567
 _TLD2: if(ps!=0)goto _TLD3;else{goto _TLD1;}_TLD3: if(tqts!=0)goto _TLD0;else{goto _TLD1;}
_TLD0: _T23B=ps;_T23C=_T23B->hd;{struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)_T23C;_T23D=inst;_T23E=tqts;_T23F=_T23E->hd;_T240=(struct _tuple20*)_T23F;_T241=*_T240;_T242=_T241.f1;{
# 571
void*field_typ=Cyc_Tcutil_substitute(_T23D,_T242);_T243=access_exp;_T244=(unsigned)_T243;
# 574
if(!_T244)goto _TLD4;_T245=access_exp;_T246=& res.patvars;_T247=Cyc_Absyn_char_type;
Cyc_Tcpat_set_vd(0,_T245,_T246,_T247);goto _TLD5;_TLD4: _TLD5: _T248=res;_T249=te;_T24A=p2;_T24B=& field_typ;_T24C=rgn_pat;_T24D=allow_ref_pat;_T24E=
Cyc_Tcpat_tcPatRec(_T249,_T24A,_T24B,_T24C,_T24D,0);res=Cyc_Tcpat_combine_results(_T248,_T24E);_T24F=p2;_T250=_T24F->loc;_T251=p2;_T252=_T251->topt;_T253=
# 581
_check_null(_T252);_T254=field_typ;_T255=Cyc_Unify_unify_and_constrain(_T250,_T253,_T254);if(_T255)goto _TLD6;else{goto _TLD8;}
_TLD8:{struct Cyc_Warn_Qvar_Warn_Warg_struct _T27A;_T27A.tag=1;_T257=tuf;_T27A.f1=_T257->name;_T256=_T27A;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T27A=_T256;{struct Cyc_Warn_String_Warn_Warg_struct _T27B;_T27B.tag=0;_T27B.f1=_tag_fat(" expects argument type ",sizeof(char),24U);_T258=_T27B;}{struct Cyc_Warn_String_Warn_Warg_struct _T27B=_T258;{struct Cyc_Warn_Typ_Warn_Warg_struct _T27C;_T27C.tag=2;_T27C.f1=field_typ;_T259=_T27C;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T27C=_T259;{struct Cyc_Warn_String_Warn_Warg_struct _T27D;_T27D.tag=0;
_T27D.f1=_tag_fat(" not ",sizeof(char),6U);_T25A=_T27D;}{struct Cyc_Warn_String_Warn_Warg_struct _T27D=_T25A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T27E;_T27E.tag=2;_T25C=p2;_T25D=_T25C->topt;_T27E.f1=_check_null(_T25D);_T25B=_T27E;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T27E=_T25B;void*_T27F[5];_T27F[0]=& _T27A;_T27F[1]=& _T27B;_T27F[2]=& _T27C;_T27F[3]=& _T27D;_T27F[4]=& _T27E;_T25E=p2;_T25F=_T25E->loc;_T260=_tag_fat(_T27F,sizeof(void*),5);Cyc_Warn_err2(_T25F,_T260);}}}}}goto _TLD7;_TLD6: _TLD7:;}}_T261=ps;
# 567
ps=_T261->tl;_T262=tqts;tqts=_T262->tl;goto _TLD2;_TLD1:
# 585
 if(ps==0)goto _TLD9;{struct Cyc_Warn_String_Warn_Warg_struct _T27A;_T27A.tag=0;
_T27A.f1=_tag_fat("too many arguments for datatype constructor ",sizeof(char),45U);_T263=_T27A;}{struct Cyc_Warn_String_Warn_Warg_struct _T27A=_T263;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T27B;_T27B.tag=1;_T265=tuf;_T27B.f1=_T265->name;_T264=_T27B;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T27B=_T264;void*_T27C[2];_T27C[0]=& _T27A;_T27C[1]=& _T27B;_T266=p;_T267=_T266->loc;_T268=_tag_fat(_T27C,sizeof(void*),2);Cyc_Warn_err2(_T267,_T268);}}goto _TLDA;_TLD9: _TLDA:
 if(tqts==0)goto _TLDB;{struct Cyc_Warn_String_Warn_Warg_struct _T27A;_T27A.tag=0;
_T27A.f1=_tag_fat("too few arguments for datatype constructor ",sizeof(char),44U);_T269=_T27A;}{struct Cyc_Warn_String_Warn_Warg_struct _T27A=_T269;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T27B;_T27B.tag=1;_T26B=tuf;_T27B.f1=_T26B->name;_T26A=_T27B;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T27B=_T26A;void*_T27C[2];_T27C[0]=& _T27A;_T27C[1]=& _T27B;_T26C=p;_T26D=_T26C->loc;_T26E=_tag_fat(_T27C,sizeof(void*),2);Cyc_Warn_err2(_T26D,_T26E);}}goto _TLDC;_TLDB: _TLDC: goto _LL0;}}}}}}case 14: goto _LL22;case 16: _LL22: goto _LL24;default: _LL24: _T26F=
# 593
Cyc_Tcenv_lookup_opt_type_vars(te);t=Cyc_Absyn_wildtyp(_T26F);goto _LL0;}_LL0:;}
# 595
tcpat_end: _T270=p;
_T270->topt=t;_T271=res;
return _T271;}}}
# 600
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){struct Cyc_Tcpat_TcPatResult _T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Vardecl**_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_Absyn_Vardecl**_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*(*_TA)(struct _fat_ptr*(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*);struct Cyc_List_List*(*_TB)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_Absyn_Pat*_TE;unsigned _TF;struct _fat_ptr _T10;struct Cyc_Tcpat_TcPatResult _T11;struct Cyc_List_List*_T12;void*_T13;struct Cyc_Absyn_Exp**_T14;struct Cyc_Absyn_Exp**_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Absyn_Exp**_T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Exp**_T1A;struct Cyc_Absyn_Exp**_T1B;struct Cyc_Absyn_Exp*_T1C;struct Cyc_List_List*_T1D;struct Cyc_Tcpat_TcPatResult _T1E;
# 602
struct Cyc_Tcpat_TcPatResult ans=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);_T0=ans;_T1=_T0.patvars;{
# 604
struct _tuple1 _T1F=Cyc_List_split(_T1);struct Cyc_List_List*_T20;_T20=_T1F.f0;{struct Cyc_List_List*vs1=_T20;
struct Cyc_List_List*vs=0;{
struct Cyc_List_List*x=vs1;_TLE0: if(x!=0)goto _TLDE;else{goto _TLDF;}
_TLDE: _T2=x;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Vardecl**)_T3;if(_T4==0)goto _TLE1;{struct Cyc_List_List*_T21=_cycalloc(sizeof(struct Cyc_List_List));_T6=x;_T7=_T6->hd;_T8=(struct Cyc_Absyn_Vardecl**)_T7;_T21->hd=*_T8;_T21->tl=vs;_T5=(struct Cyc_List_List*)_T21;}vs=_T5;goto _TLE2;_TLE1: _TLE2: _T9=x;
# 606
x=_T9->tl;goto _TLE0;_TLDF:;}_TB=Cyc_List_map;{
# 608
struct Cyc_List_List*(*_T21)(struct _fat_ptr*(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*))_TB;_TA=_T21;}_TC=vs;_TD=_TA(Cyc_Tcpat_get_name,_TC);_TE=p;_TF=_TE->loc;_T10=
_tag_fat("pattern contains a repeated variable",sizeof(char),37U);
# 608
Cyc_Tcutil_check_unique_vars(_TD,_TF,_T10);_T11=ans;{
# 613
struct Cyc_List_List*x=_T11.patvars;_TLE6: if(x!=0)goto _TLE4;else{goto _TLE5;}
_TLE4: _T12=x;_T13=_T12->hd;{struct _tuple17*_T21=(struct _tuple17*)_T13;void*_T22;struct Cyc_Absyn_Vardecl**_T23;{struct _tuple17 _T24=*_T21;_T23=_T24.f0;_T14=& _T21->f1;_T22=(struct Cyc_Absyn_Exp**)_T14;}{struct Cyc_Absyn_Vardecl**vdopt=_T23;struct Cyc_Absyn_Exp**expopt=(struct Cyc_Absyn_Exp**)_T22;_T15=expopt;_T16=*_T15;
if(_T16==0)goto _TLE7;_T17=expopt;_T18=*_T17;_T19=pat_var_exp;if(_T18==_T19)goto _TLE7;_T1A=expopt;_T1B=expopt;_T1C=*_T1B;
*_T1A=Cyc_Tcutil_deep_copy_exp(1,_T1C);goto _TLE8;_TLE7: _TLE8:;}}_T1D=x;
# 613
x=_T1D->tl;goto _TLE6;_TLE5:;}_T1E=ans;
# 618
return _T1E;}}}
# 624
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){struct _tuple0 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TC;struct Cyc_Absyn_PtrInfo _TD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TE;struct Cyc_Absyn_PtrInfo _TF;struct Cyc_Absyn_PtrAtts _T10;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T11;struct Cyc_Absyn_PtrInfo _T12;struct Cyc_Absyn_PtrAtts _T13;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T14;struct Cyc_Absyn_PtrInfo _T15;struct Cyc_Absyn_PtrAtts _T16;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T17;struct Cyc_Absyn_PtrInfo _T18;struct Cyc_Absyn_PtrAtts _T19;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T1A;struct Cyc_Absyn_PtrInfo _T1B;struct Cyc_Absyn_PtrAtts _T1C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T1D;struct Cyc_Absyn_PtrInfo _T1E;struct Cyc_Absyn_PtrAtts _T1F;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T20;struct Cyc_Absyn_PtrInfo _T21;struct Cyc_Absyn_PtrAtts _T22;int _T23;struct Cyc_Absyn_Exp*_T24;unsigned _T25;struct Cyc_List_List*_T26;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T27;void*_T28;void*_T29;int _T2A;struct Cyc_List_List*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2D;void*_T2E;{struct _tuple0 _T2F;
# 627
_T2F.f0=Cyc_Absyn_compress(old_type);_T2F.f1=Cyc_Absyn_compress(new_type);_T0=_T2F;}{struct _tuple0 _T2F=_T0;struct Cyc_Absyn_PtrInfo _T30;struct Cyc_Absyn_PtrInfo _T31;_T1=_T2F.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=4)goto _TLE9;_T4=_T2F.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=4)goto _TLEB;_T7=_T2F.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T32=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T7;_T31=_T32->f1;}_T8=_T2F.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T32=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T8;_T30=_T32->f1;}{struct Cyc_Absyn_PtrInfo pold=_T31;struct Cyc_Absyn_PtrInfo pnew=_T30;
# 629
struct Cyc_Absyn_PointerType_Absyn_Type_struct*ptry;ptry=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct));_T9=ptry;_T9->tag=4;_TA=ptry;_TB=pold;_TA->f1.elt_type=_TB.elt_type;_TC=ptry;_TD=pnew;_TC->f1.elt_tq=_TD.elt_tq;_TE=ptry;_TF=pold;_T10=_TF.ptr_atts;
_TE->f1.ptr_atts.eff=_T10.eff;_T11=ptry;_T12=pnew;_T13=_T12.ptr_atts;
_T11->f1.ptr_atts.nullable=_T13.nullable;_T14=ptry;_T15=pnew;_T16=_T15.ptr_atts;
_T14->f1.ptr_atts.bounds=_T16.bounds;_T17=ptry;_T18=pnew;_T19=_T18.ptr_atts;
_T17->f1.ptr_atts.zero_term=_T19.zero_term;_T1A=ptry;_T1B=pold;_T1C=_T1B.ptr_atts;
_T1A->f1.ptr_atts.ptrloc=_T1C.ptrloc;_T1D=ptry;_T1E=pnew;_T1F=_T1E.ptr_atts;
_T1D->f1.ptr_atts.autoreleased=_T1F.autoreleased;_T20=ptry;_T21=pold;_T22=_T21.ptr_atts;
_T20->f1.ptr_atts.aqual=_T22.aqual;_T24=initializer;_T25=_T24->loc;_T26=assump;_T27=ptry;_T28=(void*)_T27;_T29=new_type;_T2A=
Cyc_Subtype_subtype(_T25,_T26,_T28,_T29);if(!_T2A)goto _TLED;_T2B=
Cyc_Tcenv_curr_aquals_bounds(tenv);_T2C=initializer;_T2D=ptry;_T2E=(void*)_T2D;_T23=Cyc_Tcutil_coerce_assign(_T2B,_T2C,_T2E);goto _TLEE;_TLED: _T23=0;_TLEE:
# 637
 return _T23;}_TLEB: goto _LL3;_TLE9: _LL3:
# 639
 return 0;;}}
# 647
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){struct _tuple0 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;struct Cyc_Absyn_PtrInfo _T8;struct Cyc_Absyn_PtrAtts _T9;struct Cyc_Absyn_PtrInfo _TA;struct Cyc_Absyn_PtrAtts _TB;void*_TC;void*_TD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TE;void**_TF;struct Cyc_List_List*_T10;struct _tuple0*_T11;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T12;void**_T13;int*_T14;int _T15;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T16;void*_T17;int*_T18;int _T19;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;void*_T1F;int*_T20;int _T21;struct Cyc_List_List*_T22;void**_T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;unsigned _T25;struct _fat_ptr _T26;int _T27;struct _tuple0 _T28;void*_T29;int*_T2A;int _T2B;void*_T2C;int*_T2D;int _T2E;void*_T2F;void*_T30;struct Cyc_Warn_String_Warn_Warg_struct _T31;unsigned _T32;struct _fat_ptr _T33;struct Cyc_Warn_String_Warn_Warg_struct _T34;struct Cyc_Warn_Typ_Warn_Warg_struct _T35;struct Cyc_Warn_String_Warn_Warg_struct _T36;struct Cyc_Warn_Typ_Warn_Warg_struct _T37;unsigned _T38;struct _fat_ptr _T39;
# 650
struct Cyc_List_List*assump=0;{struct _tuple0 _T3A;
_T3A.f0=Cyc_Absyn_compress(old_type);_T3A.f1=Cyc_Absyn_compress(new_type);_T0=_T3A;}{struct _tuple0 _T3A=_T0;void*_T3B;void*_T3C;void*_T3D;_T1=_T3A.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=4)goto _TLEF;_T4=_T3A.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=4)goto _TLF1;_T7=_T3A.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T7;_T8=_T3E->f1;_T9=_T8.ptr_atts;_T3D=_T9.eff;_TA=_T3E->f1;_TB=_TA.ptr_atts;_T3C=_TB.aqual;}_TC=_T3A.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TC;_TD=_T3A.f1;_TE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD;_TF=& _TE->f1.ptr_atts.aqual;_T3B=(void**)_TF;}{void*oldrgn=_T3D;void*aq_old=_T3C;void**aq_new=(void**)_T3B;{struct Cyc_List_List*_T3E=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T3F=_cycalloc(sizeof(struct _tuple0));
# 653
_T3F->f0=oldrgn;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T40=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T40->tag=2;_T40->f1=tv;_T12=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T40;}_T3F->f1=(void*)_T12;_T11=(struct _tuple0*)_T3F;}_T3E->hd=_T11;_T3E->tl=0;_T10=(struct Cyc_List_List*)_T3E;}assump=_T10;_T13=aq_new;{
void*_T3E=*_T13;struct Cyc_List_List*_T3F;_T14=(int*)_T3E;_T15=*_T14;if(_T15!=0)goto _TLF3;_T16=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3E;_T17=_T16->f1;_T18=(int*)_T17;_T19=*_T18;if(_T19!=17)goto _TLF5;_T1A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3E;_T1B=_T1A->f2;if(_T1B==0)goto _TLF7;_T1C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3E;_T1D=_T1C->f2;_T1E=(struct Cyc_List_List*)_T1D;_T1F=_T1E->hd;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=1)goto _TLF9;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T40=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3E;_T22=_T40->f2;{struct Cyc_List_List _T41=*_T22;_T3F=_T41.tl;}}{struct Cyc_List_List*bnd=_T3F;_T23=aq_new;
# 656
*_T23=Cyc_Absyn_al_qual_type;goto _LL5;}_TLF9: goto _LL8;_TLF7: goto _LL8;_TLF5: goto _LL8;_TLF3: _LL8: goto _LL5;_LL5:;}goto _LL0;}_TLF1: goto _LL3;_TLEF: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _T3E;_T3E.tag=0;
# 663
_T3E.f1=_tag_fat("alias requires pointer type",sizeof(char),28U);_T24=_T3E;}{struct Cyc_Warn_String_Warn_Warg_struct _T3E=_T24;void*_T3F[1];_T3F[0]=& _T3E;_T25=loc;_T26=_tag_fat(_T3F,sizeof(void*),1);Cyc_Warn_err2(_T25,_T26);}goto _LL0;_LL0:;}_T27=
# 666
Cyc_Subtype_subtype(loc,assump,old_type,new_type);if(!_T27)goto _TLFB;{struct _tuple0 _T3A;
# 683 "tcpat.cyc"
_T3A.f0=Cyc_Absyn_compress(old_type);_T3A.f1=Cyc_Absyn_compress(new_type);_T28=_T3A;}{struct _tuple0 _T3A=_T28;struct Cyc_Absyn_PtrInfo _T3B;struct Cyc_Absyn_PtrInfo _T3C;_T29=_T3A.f0;_T2A=(int*)_T29;_T2B=*_T2A;if(_T2B!=4)goto _TLFD;_T2C=_T3A.f1;_T2D=(int*)_T2C;_T2E=*_T2D;if(_T2E!=4)goto _TLFF;_T2F=_T3A.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2F;_T3C=_T3D->f1;}_T30=_T3A.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T30;_T3B=_T3D->f1;}{struct Cyc_Absyn_PtrInfo pold=_T3C;struct Cyc_Absyn_PtrInfo pnew=_T3B;goto _LLA;}_TLFF: goto _LLD;_TLFD: _LLD:{struct Cyc_Warn_String_Warn_Warg_struct _T3D;_T3D.tag=0;
# 685
_T3D.f1=_tag_fat("alias requires pointer type",sizeof(char),28U);_T31=_T3D;}{struct Cyc_Warn_String_Warn_Warg_struct _T3D=_T31;void*_T3E[1];_T3E[0]=& _T3D;_T32=loc;_T33=_tag_fat(_T3E,sizeof(void*),1);Cyc_Warn_err2(_T32,_T33);}goto _LLA;_LLA:;}goto _TLFC;
# 688
_TLFB:{struct Cyc_Warn_String_Warn_Warg_struct _T3A;_T3A.tag=0;_T3A.f1=_tag_fat("cannot alias value of type ",sizeof(char),28U);_T34=_T3A;}{struct Cyc_Warn_String_Warn_Warg_struct _T3A=_T34;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3B;_T3B.tag=2;_T3B.f1=old_type;_T35=_T3B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3B=_T35;{struct Cyc_Warn_String_Warn_Warg_struct _T3C;_T3C.tag=0;_T3C.f1=_tag_fat(" to type ",sizeof(char),10U);_T36=_T3C;}{struct Cyc_Warn_String_Warn_Warg_struct _T3C=_T36;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3D;_T3D.tag=2;_T3D.f1=new_type;_T37=_T3D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3D=_T37;void*_T3E[4];_T3E[0]=& _T3A;_T3E[1]=& _T3B;_T3E[2]=& _T3C;_T3E[3]=& _T3D;_T38=loc;_T39=_tag_fat(_T3E,sizeof(void*),4);Cyc_Warn_err2(_T38,_T39);}}}}_TLFC:;}
# 694
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Pat*_T3;void*_T4;int*_T5;int _T6;struct Cyc_Absyn_PtrInfo _T7;struct Cyc_Absyn_PtrAtts _T8;struct Cyc_Absyn_PtrInfo _T9;struct Cyc_Absyn_PtrAtts _TA;struct Cyc_Tcenv_Tenv*_TB;struct Cyc_Absyn_Pat*_TC;unsigned _TD;void*_TE;struct Cyc_Tcenv_Tenv*_TF;struct Cyc_Absyn_Pat*_T10;int _T11;struct Cyc_List_List*_T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;int(*_T14)(struct _fat_ptr);void*(*_T15)(struct _fat_ptr);struct _fat_ptr _T16;void*_T17;struct Cyc_Tcenv_Tenv*_T18;struct Cyc_List_List*_T19;void*_T1A;struct _tuple16*_T1B;struct _tuple16 _T1C;struct Cyc_Absyn_Pat*_T1D;int _T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_Tcenv_Tenv*_T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_Absyn_Pat*_T24;int _T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;void*_T29;struct Cyc_Absyn_Vardecl**_T2A;struct Cyc_Absyn_Vardecl*_T2B;struct Cyc_Absyn_Vardecl*_T2C;struct Cyc_Absyn_Exp*_T2D;int*_T2E;int _T2F;struct Cyc_List_List*_T30;struct Cyc_Absyn_Exp*_T31;void*_T32;void*_T33;int _T34;struct Cyc_Warn_String_Warn_Warg_struct _T35;struct Cyc_Absyn_Pat*_T36;unsigned _T37;struct _fat_ptr _T38;struct Cyc_Warn_String_Warn_Warg_struct _T39;int(*_T3A)(struct _fat_ptr);void*(*_T3B)(struct _fat_ptr);struct _fat_ptr _T3C;struct Cyc_List_List*_T3D;struct Cyc_Absyn_Pat*_T3E;int*_T3F;int _T40;int _T41;struct Cyc_Warn_String_Warn_Warg_struct _T42;struct Cyc_Absyn_Pat*_T43;unsigned _T44;struct _fat_ptr _T45;struct Cyc_List_List*_T46;void*_T47;struct Cyc_Absyn_Vardecl**_T48;struct Cyc_Absyn_Vardecl*_T49;struct Cyc_Absyn_Vardecl*_T4A;struct Cyc_Warn_String_Warn_Warg_struct _T4B;struct Cyc_Absyn_Pat*_T4C;unsigned _T4D;struct _fat_ptr _T4E;struct Cyc_Absyn_Pat*_T4F;unsigned _T50;struct Cyc_Tcenv_Tenv*_T51;struct Cyc_List_List*_T52;struct _fat_ptr _T53;struct Cyc_Tcenv_Tenv*_T54;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T55;void*_T56;struct Cyc_Tcenv_Tenv*_T57;struct Cyc_Absyn_Pat*_T58;unsigned _T59;struct Cyc_Absyn_Exp*_T5A;void*_T5B;void*_T5C;struct Cyc_Absyn_Tvar*_T5D;struct Cyc_Absyn_Vardecl*_T5E;void*_T5F;struct Cyc_Absyn_Exp*_T60;struct Cyc_List_List*_T61;_T0=p;{
# 696
void*_T62=_T0->r;struct Cyc_Absyn_Tvar*_T63;struct Cyc_Absyn_Pat*_T64;struct Cyc_Absyn_Vardecl*_T65;struct Cyc_List_List*_T66;struct Cyc_List_List*_T67;int _T68;void*_T69;_T1=(int*)_T62;_T2=*_T1;switch(_T2){case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T6A=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T62;_T69=_T6A->f1;}{struct Cyc_Absyn_Pat*p2=_T69;_T3=p;_T4=_T3->topt;{
# 698
void*_T6A=_check_null(_T4);void*_T6B;void*_T6C;_T5=(int*)_T6A;_T6=*_T5;if(_T6!=4)goto _TL102;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T6D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6A;_T7=_T6D->f1;_T8=_T7.ptr_atts;_T6C=_T8.eff;_T9=_T6D->f1;_TA=_T9.ptr_atts;_T6B=_TA.aqual;}{void*rt=_T6C;void*aq=_T6B;_TB=te;_TC=p;_TD=_TC->loc;_TE=rt;
# 700
Cyc_Tcenv_check_effect_accessible_nodelay(_TB,_TD,_TE);_TF=te;_T10=p2;_T11=
Cyc_Tcutil_is_noalias_qual(aq,0);_T12=patvars;Cyc_Tcpat_check_pat_regions_rec(_TF,_T10,_T11,_T12);
return;}_TL102:{struct Cyc_Warn_String_Warn_Warg_struct _T6D;_T6D.tag=0;
_T6D.f1=_tag_fat("check_pat_regions: bad pointer type",sizeof(char),36U);_T13=_T6D;}{struct Cyc_Warn_String_Warn_Warg_struct _T6D=_T13;void*_T6E[1];_T6E[0]=& _T6D;_T15=Cyc_Warn_impos2;{int(*_T6F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T15;_T14=_T6F;}_T16=_tag_fat(_T6E,sizeof(void*),1);_T14(_T16);};}}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T6A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T62;_T17=_T6A->f1;_T69=(void*)_T17;_T68=_T6A->f2;_T67=_T6A->f3;_T66=_T6A->f4;}{void*ai=_T69;int is_tuple=_T68;struct Cyc_List_List*exist_ts=_T67;struct Cyc_List_List*dps=_T66;
# 706
_TL107: if(dps!=0)goto _TL105;else{goto _TL106;}
_TL105: _T18=te;_T19=dps;_T1A=_T19->hd;_T1B=(struct _tuple16*)_T1A;_T1C=*_T1B;_T1D=_T1C.f1;_T1E=did_noalias_deref;_T1F=patvars;Cyc_Tcpat_check_pat_regions_rec(_T18,_T1D,_T1E,_T1F);_T20=dps;
# 706
dps=_T20->tl;goto _TL107;_TL106:
# 708
 return;}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T6A=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T62;_T67=_T6A->f3;}{struct Cyc_List_List*ps=_T67;
# 711
did_noalias_deref=0;
_TL10B: if(ps!=0)goto _TL109;else{goto _TL10A;}
_TL109: _T21=te;_T22=ps;_T23=_T22->hd;_T24=(struct Cyc_Absyn_Pat*)_T23;_T25=did_noalias_deref;_T26=patvars;Cyc_Tcpat_check_pat_regions_rec(_T21,_T24,_T25,_T26);_T27=ps;
# 712
ps=_T27->tl;goto _TL10B;_TL10A:
# 714
 return;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T6A=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T62;_T65=_T6A->f1;_T64=_T6A->f2;}{struct Cyc_Absyn_Vardecl*vd=_T65;struct Cyc_Absyn_Pat*p2=_T64;{
# 716
struct Cyc_List_List*x=patvars;_TL10F: if(x!=0)goto _TL10D;else{goto _TL10E;}
_TL10D: _T28=x;_T29=_T28->hd;{struct _tuple17*_T6A=(struct _tuple17*)_T29;struct Cyc_Absyn_Exp*_T6B;struct Cyc_Absyn_Vardecl**_T6C;{struct _tuple17 _T6D=*_T6A;_T6C=_T6D.f0;_T6B=_T6D.f1;}{struct Cyc_Absyn_Vardecl**vdopt=_T6C;struct Cyc_Absyn_Exp*eopt=_T6B;
# 722
if(vdopt==0)goto _TL110;_T2A=vdopt;_T2B=*_T2A;_T2C=vd;if(_T2B!=_T2C)goto _TL110;if(eopt==0)goto _TL110;_T2D=eopt;{
void*_T6D=_T2D->r;struct Cyc_Absyn_Exp*_T6E;_T2E=(int*)_T6D;_T2F=*_T2E;if(_T2F!=15)goto _TL112;{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T6F=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T6D;_T6E=_T6F->f1;}{struct Cyc_Absyn_Exp*e=_T6E;_T30=
# 725
Cyc_Tcenv_curr_aquals_bounds(te);_T31=e;_T32=_T31->topt;_T33=_check_null(_T32);_T34=Cyc_Tcutil_is_noalias_pointer_or_aggr(_T30,_T33);if(!_T34)goto _TL114;{struct Cyc_Warn_String_Warn_Warg_struct _T6F;_T6F.tag=0;
_T6F.f1=_tag_fat("reference pattern not allowed on alias-free pointers",sizeof(char),53U);_T35=_T6F;}{struct Cyc_Warn_String_Warn_Warg_struct _T6F=_T35;void*_T70[1];_T70[0]=& _T6F;_T36=p;_T37=_T36->loc;_T38=_tag_fat(_T70,sizeof(void*),1);Cyc_Warn_err2(_T37,_T38);}goto _TL115;_TL114: _TL115: goto _LL17;}_TL112:{struct Cyc_Warn_String_Warn_Warg_struct _T6F;_T6F.tag=0;
# 728
_T6F.f1=_tag_fat("check_pat_regions: bad reference access exp",sizeof(char),44U);_T39=_T6F;}{struct Cyc_Warn_String_Warn_Warg_struct _T6F=_T39;void*_T70[1];_T70[0]=& _T6F;_T3B=Cyc_Warn_impos2;{int(*_T71)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T3B;_T3A=_T71;}_T3C=_tag_fat(_T70,sizeof(void*),1);_T3A(_T3C);}_LL17:;}goto _TL10E;_TL110:;}}_T3D=x;
# 716
x=_T3D->tl;goto _TL10F;_TL10E:;}
# 733
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T6A=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T62;_T65=_T6A->f1;_T64=_T6A->f2;}{struct Cyc_Absyn_Vardecl*vd=_T65;struct Cyc_Absyn_Pat*p2=_T64;_T3E=p;{
# 736
void*_T6A=_T3E->topt;if(_T6A==0)goto _TL116;_T3F=(int*)_T6A;_T40=*_T3F;if(_T40!=5)goto _TL118;_T41=did_noalias_deref;
# 738
if(_T41)goto _TL11A;else{goto _TL11C;}
_TL11C: goto _LL1C;_TL11A:{struct Cyc_Warn_String_Warn_Warg_struct _T6B;_T6B.tag=0;
_T6B.f1=_tag_fat("pattern to array would create alias of no-alias pointer",sizeof(char),56U);_T42=_T6B;}{struct Cyc_Warn_String_Warn_Warg_struct _T6B=_T42;void*_T6C[1];_T6C[0]=& _T6B;_T43=p;_T44=_T43->loc;_T45=_tag_fat(_T6C,sizeof(void*),1);Cyc_Warn_err2(_T44,_T45);}
return;_TL118: goto _LL1F;_TL116: _LL1F: goto _LL1C;_LL1C:;}
# 744
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 2:{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T6A=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T62;_T63=_T6A->f1;_T65=_T6A->f2;}{struct Cyc_Absyn_Tvar*tv=_T63;struct Cyc_Absyn_Vardecl*vd=_T65;{
# 747
struct Cyc_List_List*x=patvars;_TL120: if(x!=0)goto _TL11E;else{goto _TL11F;}
_TL11E: _T46=x;_T47=_T46->hd;{struct _tuple17*_T6A=(struct _tuple17*)_T47;struct Cyc_Absyn_Exp*_T6B;struct Cyc_Absyn_Vardecl**_T6C;{struct _tuple17 _T6D=*_T6A;_T6C=_T6D.f0;_T6B=_T6D.f1;}{struct Cyc_Absyn_Vardecl**vdopt=_T6C;struct Cyc_Absyn_Exp*eopt=_T6B;
# 751
if(vdopt==0)goto _TL121;_T48=vdopt;_T49=*_T48;_T4A=vd;if(_T49!=_T4A)goto _TL121;
if(eopt!=0)goto _TL123;{struct Cyc_Warn_String_Warn_Warg_struct _T6D;_T6D.tag=0;
_T6D.f1=_tag_fat("cannot alias pattern expression in datatype",sizeof(char),44U);_T4B=_T6D;}{struct Cyc_Warn_String_Warn_Warg_struct _T6D=_T4B;void*_T6E[1];_T6E[0]=& _T6D;_T4C=p;_T4D=_T4C->loc;_T4E=_tag_fat(_T6E,sizeof(void*),1);Cyc_Warn_err2(_T4D,_T4E);}goto _TL124;
# 755
_TL123: _T4F=p;_T50=_T4F->loc;_T51=te;{struct Cyc_Absyn_Tvar*_T6D[1];_T6D[0]=tv;_T53=_tag_fat(_T6D,sizeof(struct Cyc_Absyn_Tvar*),1);_T52=Cyc_List_list(_T53);}{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_T50,_T51,_T52);_T54=te2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T6D=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T6D->tag=2;
_T6D->f1=tv;_T55=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T6D;}_T56=(void*)_T55;te2=Cyc_Tcenv_add_region(_T54,_T56,0,0);_T57=te2;_T58=p;_T59=_T58->loc;_T5A=eopt;_T5B=_T5A->topt;_T5C=
# 758
_check_null(_T5B);_T5D=tv;_T5E=vd;_T5F=_T5E->type;_T60=eopt;Cyc_Tcpat_check_alias_coercion(_T57,_T59,_T5C,_T5D,_T5F,_T60);}_TL124: goto _TL11F;_TL121:;}}_T61=x;
# 747
x=_T61->tl;goto _TL120;_TL11F:;}goto _LL0;}default:
# 764
 return;}_LL0:;}}
# 779 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_Tcpat_TcPatResult*pat_res){struct Cyc_Tcpat_TcPatResult*_T0;struct Cyc_Tcpat_TcPatResult*_T1;struct Cyc_List_List*_T2;void*_T3;int*_T4;int _T5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6;void*_T7;int*_T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_List_List*_TD;struct Cyc_Absyn_Exp*_TE;void*_TF;void*_T10;int _T11;struct Cyc_List_List*_T12;struct Cyc_Absyn_Exp*_T13;int _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Absyn_Pat*_T17;unsigned _T18;struct _fat_ptr _T19;struct Cyc_Warn_String_Warn_Warg_struct _T1A;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1B;struct Cyc_Absyn_Vardecl**_T1C;struct Cyc_Absyn_Vardecl*_T1D;struct Cyc_Warn_String_Warn_Warg_struct _T1E;struct Cyc_Absyn_Pat*_T1F;unsigned _T20;struct _fat_ptr _T21;struct Cyc_List_List*_T22;_T0=pat_res;{
struct Cyc_List_List*patvars=_T0->patvars;
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{struct Cyc_List_List*_T23;_T1=pat_res;{struct Cyc_Tcpat_TcPatResult _T24=*_T1;_T23=_T24.aquals_bounds;}{struct Cyc_List_List*aqb=_T23;
# 784
_TL128: if(aqb!=0)goto _TL126;else{goto _TL127;}
_TL126: _T2=aqb;_T3=_T2->hd;{struct _tuple0*_T24=(struct _tuple0*)_T3;void*_T25;void*_T26;{struct _tuple0 _T27=*_T24;_T26=_T27.f0;_T25=_T27.f1;}{void*tv=_T26;void*b=_T25;
void*_T27=Cyc_Absyn_compress(tv);_T4=(int*)_T27;_T5=*_T4;if(_T5!=0)goto _TL129;_T6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T27;_T7=_T6->f1;_T8=(int*)_T7;_T9=*_T8;if(_T9!=15)goto _TL12B;
# 788
te=Cyc_Tcenv_add_aquals_bound(te,tv,b);goto _LL6;_TL12B: goto _LL9;_TL129: _LL9: goto _LL6;_LL6:;}}_TA=aqb;
# 784
aqb=_TA->tl;goto _TL128;_TL127: {
# 794
struct Cyc_List_List*x=patvars;_TL130: if(x!=0)goto _TL12E;else{goto _TL12F;}
_TL12E: _TB=x;_TC=_TB->hd;{struct _tuple17*_T24=(struct _tuple17*)_TC;struct Cyc_Absyn_Exp*_T25;struct Cyc_Absyn_Vardecl**_T26;{struct _tuple17 _T27=*_T24;_T26=_T27.f0;_T25=_T27.f1;}{struct Cyc_Absyn_Vardecl**vdopt=_T26;struct Cyc_Absyn_Exp*eopt=_T25;
if(eopt==0)goto _TL131;{
struct Cyc_Absyn_Exp*e=eopt;_TD=
Cyc_Tcenv_curr_aquals_bounds(te);_TE=e;_TF=_TE->topt;_T10=_check_null(_TF);_T11=Cyc_Tcutil_is_noalias_pointer_or_aggr(_TD,_T10);if(!_T11)goto _TL133;_T12=
Cyc_Tcenv_curr_aquals_bounds(te);_T13=e;_T14=Cyc_Tcutil_is_noalias_path(_T12,_T13);
# 798
if(_T14)goto _TL133;else{goto _TL135;}
# 801
_TL135: if(vdopt!=0)goto _TL136;{struct Cyc_Warn_String_Warn_Warg_struct _T27;_T27.tag=0;
_T27.f1=_tag_fat("pattern dereferences an alias-free-pointer ",sizeof(char),44U);_T15=_T27;}{struct Cyc_Warn_String_Warn_Warg_struct _T27=_T15;{struct Cyc_Warn_String_Warn_Warg_struct _T28;_T28.tag=0;
_T28.f1=_tag_fat("from a non-unique path",sizeof(char),23U);_T16=_T28;}{struct Cyc_Warn_String_Warn_Warg_struct _T28=_T16;void*_T29[2];_T29[0]=& _T27;_T29[1]=& _T28;_T17=p;_T18=_T17->loc;_T19=_tag_fat(_T29,sizeof(void*),2);Cyc_Warn_err2(_T18,_T19);}}goto _TL137;
# 805
_TL136:{struct Cyc_Warn_String_Warn_Warg_struct _T27;_T27.tag=0;_T27.f1=_tag_fat("pattern for variable ",sizeof(char),22U);_T1A=_T27;}{struct Cyc_Warn_String_Warn_Warg_struct _T27=_T1A;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T28;_T28.tag=1;_T1C=vdopt;_T1D=*_T1C;_T28.f1=_T1D->name;_T1B=_T28;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T28=_T1B;{struct Cyc_Warn_String_Warn_Warg_struct _T29;_T29.tag=0;
_T29.f1=_tag_fat(" dereferences an alias-free-pointer from a non-unique path",sizeof(char),59U);_T1E=_T29;}{struct Cyc_Warn_String_Warn_Warg_struct _T29=_T1E;void*_T2A[3];_T2A[0]=& _T27;_T2A[1]=& _T28;_T2A[2]=& _T29;_T1F=p;_T20=_T1F->loc;_T21=_tag_fat(_T2A,sizeof(void*),3);Cyc_Warn_err2(_T20,_T21);}}}_TL137: goto _TL134;_TL133: _TL134:;}goto _TL132;_TL131: _TL132:;}}_T22=x;
# 794
x=_T22->tl;goto _TL130;_TL12F:;}}}}}
# 852 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 861
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 874
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){union Cyc_Tcpat_Name_value _T0;{union Cyc_Tcpat_Name_value _T1;_T1.Name_v.tag=1U;_T1.Name_v.val=s;_T0=_T1;}return _T0;}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _T0;{union Cyc_Tcpat_Name_value _T1;_T1.Int_v.tag=2U;_T1.Int_v.val=i;_T0=_T1;}return _T0;}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 891
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){struct Cyc_Tcpat_Con_s*_T0;struct _union_Name_value_Name_v _T1;unsigned _T2;struct _union_Name_value_Name_v _T3;struct Cyc_Tcpat_Con_s*_T4;struct _union_Name_value_Name_v _T5;unsigned _T6;struct _union_Name_value_Name_v _T7;int _T8;int _T9;struct _union_Name_value_Int_v _TA;struct Cyc_Tcpat_Con_s*_TB;struct _union_Name_value_Name_v _TC;unsigned _TD;struct _union_Name_value_Int_v _TE;int _TF;_T0=c1;{
union Cyc_Tcpat_Name_value _T10=_T0->name;int _T11;struct _fat_ptr _T12;_T1=_T10.Name_v;_T2=_T1.tag;if(_T2!=1)goto _TL138;_T3=_T10.Name_v;_T12=_T3.val;{struct _fat_ptr n1=_T12;_T4=c2;{
# 894
union Cyc_Tcpat_Name_value _T13=_T4->name;struct _fat_ptr _T14;_T5=_T13.Name_v;_T6=_T5.tag;if(_T6!=1)goto _TL13A;_T7=_T13.Name_v;_T14=_T7.val;{struct _fat_ptr n2=_T14;_T8=
Cyc_strcmp(n1,n2);return _T8;}_TL13A: _T9=- 1;
return _T9;;}}goto _TL139;_TL138: _TA=_T10.Int_v;_T11=_TA.val;{int i1=_T11;_TB=c2;{
# 899
union Cyc_Tcpat_Name_value _T13=_TB->name;int _T14;_TC=_T13.Name_v;_TD=_TC.tag;if(_TD!=1)goto _TL13C;
return 1;_TL13C: _TE=_T13.Int_v;_T14=_TE.val;{int i2=_T14;_TF=i1 - i2;
return _TF;};}}_TL139:;}}
# 907
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set (void){struct Cyc_Set_Set*(*_T0)(struct _RegionHandle*,int(*)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*));struct Cyc_Set_Set*(*_T1)(struct _RegionHandle*,int(*)(void*,void*));struct _RegionHandle*_T2;struct Cyc_Set_Set*_T3;_T1=Cyc_Set_rempty;{
struct Cyc_Set_Set*(*_T4)(struct _RegionHandle*,int(*)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*))=(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))_T1;_T0=_T4;}_T2=Cyc_Core_heap_region;_T3=_T0(_T2,Cyc_Tcpat_compare_con);return _T3;}
# 911
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 915
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){struct Cyc_Absyn_Datatypedecl*_T0;struct Cyc_Core_Opt*_T1;struct Cyc_Core_Opt*_T2;void*_T3;struct _tuple2*_T4;struct Cyc_List_List*_T5;void*_T6;struct Cyc_Absyn_Datatypefield*_T7;struct _tuple2*_T8;int _T9;struct Cyc_List_List*_TA;unsigned _TB;
unsigned ans=0U;_T0=td;_T1=_T0->fields;_T2=
_check_null(_T1);_T3=_T2->v;{struct Cyc_List_List*fs=(struct Cyc_List_List*)_T3;
_TL13E: _T4=name;_T5=_check_null(fs);_T6=_T5->hd;_T7=(struct Cyc_Absyn_Datatypefield*)_T6;_T8=_T7->name;_T9=Cyc_Absyn_qvar_cmp(_T4,_T8);if(_T9!=0)goto _TL13F;else{goto _TL140;}
_TL13F: ans=ans + 1;_TA=fs;
fs=_TA->tl;goto _TL13E;_TL140: _TB=ans;
# 922
return _TB;}}
# 925
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){struct Cyc_Absyn_Aggrdecl*_T0;struct Cyc_Absyn_AggrdeclImpl*_T1;struct Cyc_Absyn_AggrdeclImpl*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Aggrfield*_T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;struct _fat_ptr*_T8;struct _fat_ptr _T9;int _TA;int _TB;struct Cyc_List_List*_TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct _fat_ptr*_TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;int(*_T11)(struct _fat_ptr);void*(*_T12)(struct _fat_ptr);struct _fat_ptr _T13;
int i=1;_T0=ad;_T1=_T0->impl;_T2=
_check_null(_T1);{struct Cyc_List_List*fields=_T2->fields;_TL144: if(fields!=0)goto _TL142;else{goto _TL143;}
_TL142: _T3=fields;_T4=_T3->hd;_T5=(struct Cyc_Absyn_Aggrfield*)_T4;_T6=_T5->name;_T7=*_T6;_T8=f;_T9=*_T8;_TA=Cyc_strcmp(_T7,_T9);if(_TA!=0)goto _TL145;_TB=i;return _TB;_TL145: _TC=fields;
# 927
fields=_TC->tl;i=i + 1;goto _TL144;_TL143:;}{struct Cyc_Warn_String_Warn_Warg_struct _T14;_T14.tag=0;
# 929
_T14.f1=_tag_fat("get_member_offset ",sizeof(char),19U);_TD=_T14;}{struct Cyc_Warn_String_Warn_Warg_struct _T14=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;_TF=f;_T15.f1=*_TF;_TE=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_TE;{struct Cyc_Warn_String_Warn_Warg_struct _T16;_T16.tag=0;_T16.f1=_tag_fat(" failed",sizeof(char),8U);_T10=_T16;}{struct Cyc_Warn_String_Warn_Warg_struct _T16=_T10;void*_T17[3];_T17[0]=& _T14;_T17[1]=& _T15;_T17[2]=& _T16;_T12=Cyc_Warn_impos2;{int(*_T18)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T12;_T11=_T18;}_T13=_tag_fat(_T17,sizeof(void*),3);_T11(_T13);}}}}
# 932
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){union Cyc_Tcpat_PatOrWhere _T0;struct _union_PatOrWhere_where_clause _T1;unsigned _T2;union Cyc_Tcpat_PatOrWhere _T3;struct _union_PatOrWhere_where_clause _T4;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T5;void*_T6;union Cyc_Tcpat_PatOrWhere _T7;struct _union_PatOrWhere_pattern _T8;struct Cyc_Absyn_Pat*_T9;int*_TA;unsigned _TB;union Cyc_Tcpat_PatOrWhere _TC;void*_TD;struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct*_TE;struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct*_TF;void*_T10;struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T11;int _T12;void*_T13;struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T14;char _T15;void*_T16;struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_T17;void*_T18;struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_T19;void*_T1A;void*_T1B;struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_T1C;void*_T1D;struct Cyc_Absyn_Pat*_T1E;void*_T1F;void*_T20;int _T21;struct Cyc_Absyn_Pat*_T22;void*_T23;void*_T24;int _T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;int(*_T27)(struct _fat_ptr);void*(*_T28)(struct _fat_ptr);struct _fat_ptr _T29;struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct*_T2A;struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct*_T2B;void*_T2C;struct Cyc_Absyn_Datatypedecl*_T2D;int _T2E;struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_T2F;void*_T30;struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_T31;struct Cyc_Absyn_Datatypedecl*_T32;struct Cyc_Absyn_Datatypefield*_T33;struct _tuple2*_T34;unsigned _T35;void*_T36;void*_T37;void*_T38;int*_T39;int _T3A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3B;void*_T3C;int*_T3D;int _T3E;void*_T3F;struct Cyc_Absyn_Aggrdecl*_T40;struct Cyc_Absyn_AggrdeclImpl*_T41;struct Cyc_Absyn_AggrdeclImpl*_T42;int _T43;struct Cyc_Warn_String_Warn_Warg_struct _T44;int(*_T45)(struct _fat_ptr);void*(*_T46)(struct _fat_ptr);struct _fat_ptr _T47;struct Cyc_List_List*_T48;void*_T49;struct _tuple16*_T4A;struct _tuple16 _T4B;struct Cyc_List_List*_T4C;struct Cyc_List_List*_T4D;int*_T4E;int _T4F;struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T50;void*_T51;struct Cyc_Warn_String_Warn_Warg_struct _T52;int(*_T53)(struct _fat_ptr);void*(*_T54)(struct _fat_ptr);struct _fat_ptr _T55;struct Cyc_Warn_String_Warn_Warg_struct _T56;int(*_T57)(struct _fat_ptr);void*(*_T58)(struct _fat_ptr);struct _fat_ptr _T59;struct Cyc_Warn_String_Warn_Warg_struct _T5A;int(*_T5B)(struct _fat_ptr);void*(*_T5C)(struct _fat_ptr);struct _fat_ptr _T5D;struct Cyc_Absyn_Pat*_T5E;struct Cyc_Absyn_Exp*_T5F;_T0=pw;_T1=_T0.where_clause;_T2=_T1.tag;if(_T2!=2)goto _TL147;_T3=pw;_T4=_T3.where_clause;_T5F=_T4.val;{struct Cyc_Absyn_Exp*e=_T5F;{struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T60=_cycalloc(sizeof(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct));_T60->tag=0;
# 934
_T60->f1=e;_T5=(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_T60;}_T6=(void*)_T5;return _T6;}_TL147: _T7=pw;_T8=_T7.pattern;_T5E=_T8.val;{struct Cyc_Absyn_Pat*p=_T5E;_T9=p;{
# 936
void*_T60=_T9->r;struct Cyc_List_List*_T61;struct Cyc_Absyn_Datatypefield*_T62;struct Cyc_Absyn_Enumfield*_T63;struct _fat_ptr _T64;char _T65;int _T66;enum Cyc_Absyn_Sign _T67;void*_T68;_TA=(int*)_T60;_TB=*_TA;switch(_TB){case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T60;_T68=_T69->f2;}{struct Cyc_Absyn_Pat*p1=_T68;_T68=p1;goto _LL9;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T60;_T68=_T69->f2;}_LL9: {struct Cyc_Absyn_Pat*p1=_T68;{union Cyc_Tcpat_PatOrWhere _T69;_T69.pattern.tag=1U;
# 938
_T69.pattern.val=p1;_TC=_T69;}_TD=Cyc_Tcpat_get_pat_test(_TC);return _TD;}case 8: _TE=& Cyc_Tcpat_EqNull_val;_TF=(struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct*)_TE;_T10=(void*)_TF;
return _T10;case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T60;_T67=_T69->f1;_T66=_T69->f2;}{enum Cyc_Absyn_Sign s=_T67;int i=_T66;{struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T69=_cycalloc(sizeof(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct));_T69->tag=6;_T12=i;
_T69->f1=(unsigned)_T12;_T11=(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_T69;}_T13=(void*)_T11;return _T13;}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T60;_T65=_T69->f1;}{char c=_T65;{struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T69=_cycalloc(sizeof(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct));_T69->tag=6;_T15=c;
_T69->f1=(unsigned)_T15;_T14=(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_T69;}_T16=(void*)_T14;return _T16;}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T60;_T64=_T69->f1;_T66=_T69->f2;}{struct _fat_ptr f=_T64;int i=_T66;{struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_T69=_cycalloc(sizeof(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct));_T69->tag=5;
_T69->f1=f;_T69->f2=i;_T17=(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_T69;}_T18=(void*)_T17;return _T18;}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T60;_T68=_T69->f1;_T63=_T69->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T68;struct Cyc_Absyn_Enumfield*ef=_T63;{struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_T69=_cycalloc(sizeof(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct));_T69->tag=3;
_T69->f1=ed;_T69->f2=ef;_T19=(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_T69;}_T1A=(void*)_T19;return _T1A;}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T60;_T1B=_T69->f1;_T68=(void*)_T1B;_T63=_T69->f2;}{void*t=_T68;struct Cyc_Absyn_Enumfield*ef=_T63;{struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_T69=_cycalloc(sizeof(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct));_T69->tag=4;
_T69->f1=t;_T69->f2=ef;_T1C=(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_T69;}_T1D=(void*)_T1C;return _T1D;}case 5: _T1E=p;_T1F=_T1E->topt;_T20=
# 946
_check_null(_T1F);_T21=Cyc_Tcutil_is_pointer_type(_T20);if(_T21)goto _TL14D;else{goto _TL14C;}_TL14D: _T22=p;_T23=_T22->topt;_T24=
_check_null(_T23);_T25=Cyc_Tcutil_is_nullable_pointer_type(_T24,0);
# 946
if(_T25)goto _TL14A;else{goto _TL14C;}
# 948
_TL14C:{struct Cyc_Warn_String_Warn_Warg_struct _T69;_T69.tag=0;_T69.f1=_tag_fat("non-null pointer type or non-pointer type in pointer pattern",sizeof(char),61U);_T26=_T69;}{struct Cyc_Warn_String_Warn_Warg_struct _T69=_T26;void*_T6A[1];_T6A[0]=& _T69;_T28=Cyc_Warn_impos2;{int(*_T6B)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T28;_T27=_T6B;}_T29=_tag_fat(_T6A,sizeof(void*),1);_T27(_T29);}goto _TL14B;_TL14A: _TL14B: _T2A=& Cyc_Tcpat_NeqNull_val;_T2B=(struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct*)_T2A;_T2C=(void*)_T2B;
return _T2C;case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T60;_T68=_T69->f1;_T62=_T69->f2;}{struct Cyc_Absyn_Datatypedecl*ddecl=_T68;struct Cyc_Absyn_Datatypefield*df=_T62;_T2D=ddecl;_T2E=_T2D->is_extensible;
# 951
if(!_T2E)goto _TL14E;{struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_T69=_cycalloc(sizeof(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct));_T69->tag=9;
_T69->f1=ddecl;_T69->f2=df;_T2F=(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_T69;}_T30=(void*)_T2F;return _T30;_TL14E:{struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_T69=_cycalloc(sizeof(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct));_T69->tag=7;_T32=ddecl;_T33=df;_T34=_T33->name;_T35=
Cyc_Tcpat_datatype_tag_number(_T32,_T34);_T69->f1=(int)_T35;_T69->f2=ddecl;_T69->f3=df;_T31=(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_T69;}_T36=(void*)_T31;return _T36;}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T69=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T60;_T37=_T69->f1;_T68=(void*)_T37;_T61=_T69->f4;}_T38=(void*)_T68;if(_T38==0)goto _TL150;{void*atype=_T68;struct Cyc_List_List*dlps=_T61;
# 955
void*_T69=Cyc_Absyn_compress(atype);union Cyc_Absyn_AggrInfo _T6A;_T39=(int*)_T69;_T3A=*_T39;if(_T3A!=0)goto _TL152;_T3B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T69;_T3C=_T3B->f1;_T3D=(int*)_T3C;_T3E=*_T3D;if(_T3E!=24)goto _TL154;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T69;_T3F=_T6B->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T6C=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T3F;_T6A=_T6C->f1;}}{union Cyc_Absyn_AggrInfo info=_T6A;
# 957
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T40=ad;_T41=_T40->impl;_T42=
_check_null(_T41);_T43=_T42->tagged;if(_T43)goto _TL156;else{goto _TL158;}
_TL158:{struct Cyc_Warn_String_Warn_Warg_struct _T6B;_T6B.tag=0;_T6B.f1=_tag_fat("non-tagged aggregate in pattern test",sizeof(char),37U);_T44=_T6B;}{struct Cyc_Warn_String_Warn_Warg_struct _T6B=_T44;void*_T6C[1];_T6C[0]=& _T6B;_T46=Cyc_Warn_impos2;{int(*_T6D)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T46;_T45=_T6D;}_T47=_tag_fat(_T6C,sizeof(void*),1);_T45(_T47);}goto _TL157;_TL156: _TL157: _T48=
_check_null(dlps);_T49=_T48->hd;_T4A=(struct _tuple16*)_T49;_T4B=*_T4A;_T4C=_T4B.f0;_T4D=_check_null(_T4C);{void*_T6B=_T4D->hd;struct _fat_ptr*_T6C;_T4E=(int*)_T6B;_T4F=*_T4E;if(_T4F!=1)goto _TL159;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T6D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T6B;_T6C=_T6D->f1;}{struct _fat_ptr*f=_T6C;{struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T6D=_cycalloc(sizeof(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct));_T6D->tag=8;
_T6D->f1=atype;_T6D->f2=f;_T6D->f3=Cyc_Tcpat_get_member_offset(ad,f);_T50=(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_T6D;}_T51=(void*)_T50;return _T51;}_TL159:{struct Cyc_Warn_String_Warn_Warg_struct _T6D;_T6D.tag=0;
_T6D.f1=_tag_fat("no field name in tagged union pattern",sizeof(char),38U);_T52=_T6D;}{struct Cyc_Warn_String_Warn_Warg_struct _T6D=_T52;void*_T6E[1];_T6E[0]=& _T6D;_T54=Cyc_Warn_impos2;{int(*_T6F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T54;_T53=_T6F;}_T55=_tag_fat(_T6E,sizeof(void*),1);_T53(_T55);};}}goto _TL155;_TL154: goto _LL21;_TL155: goto _TL153;_TL152: _LL21:{struct Cyc_Warn_String_Warn_Warg_struct _T6B;_T6B.tag=0;
# 964
_T6B.f1=_tag_fat("non-tagged aggregate in pattern test",sizeof(char),37U);_T56=_T6B;}{struct Cyc_Warn_String_Warn_Warg_struct _T6B=_T56;void*_T6C[1];_T6C[0]=& _T6B;_T58=Cyc_Warn_impos2;{int(*_T6D)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T58;_T57=_T6D;}_T59=_tag_fat(_T6C,sizeof(void*),1);_T57(_T59);}_TL153:;}goto _TL151;_TL150: goto _LL1C;_TL151:;default: _LL1C:{struct Cyc_Warn_String_Warn_Warg_struct _T69;_T69.tag=0;
# 966
_T69.f1=_tag_fat("non-test pattern in pattern test",sizeof(char),33U);_T5A=_T69;}{struct Cyc_Warn_String_Warn_Warg_struct _T69=_T5A;void*_T6A[1];_T6A[0]=& _T69;_T5C=Cyc_Warn_impos2;{int(*_T6B)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T5C;_T5B=_T6B;}_T5D=_tag_fat(_T6A,sizeof(void*),1);_T5B(_T5D);}};}};}
# 971
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){union Cyc_Tcpat_PatOrWhere _T0;{union Cyc_Tcpat_PatOrWhere _T1;_T1.pattern.tag=1U;
_T1.pattern.val=p;_T0=_T1;}return _T0;}
# 975
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_T0;struct _fat_ptr _T1;{struct Cyc_Tcpat_Con_s*_T2=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T1=
_tag_fat("NULL",sizeof(char),5U);_T2->name=Cyc_Tcpat_Name_v(_T1);_T2->arity=0;_T2->span=& Cyc_Tcpat_two_opt;_T2->orig_pat=Cyc_Tcpat_pw(p);_T0=(struct Cyc_Tcpat_Con_s*)_T2;}return _T0;}
# 978
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_T0;struct _fat_ptr _T1;{struct Cyc_Tcpat_Con_s*_T2=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T1=
_tag_fat("&",sizeof(char),2U);_T2->name=Cyc_Tcpat_Name_v(_T1);_T2->arity=1;_T2->span=& Cyc_Tcpat_two_opt;_T2->orig_pat=Cyc_Tcpat_pw(p);_T0=(struct Cyc_Tcpat_Con_s*)_T2;}return _T0;}
# 981
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_T0;struct _fat_ptr _T1;{struct Cyc_Tcpat_Con_s*_T2=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T1=
_tag_fat("&",sizeof(char),2U);_T2->name=Cyc_Tcpat_Name_v(_T1);_T2->arity=1;_T2->span=& Cyc_Tcpat_one_opt;_T2->orig_pat=Cyc_Tcpat_pw(p);_T0=(struct Cyc_Tcpat_Con_s*)_T2;}return _T0;}
# 984
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){struct Cyc_Tcpat_Con_s*_T0;{struct Cyc_Tcpat_Con_s*_T1=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));
_T1->name=Cyc_Tcpat_Int_v(i);_T1->arity=0;_T1->span=0;_T1->orig_pat=p;_T0=(struct Cyc_Tcpat_Con_s*)_T1;}return _T0;}
# 987
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_T0;{struct Cyc_Tcpat_Con_s*_T1=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));
_T1->name=Cyc_Tcpat_Name_v(f);_T1->arity=0;_T1->span=0;_T1->orig_pat=Cyc_Tcpat_pw(p);_T0=(struct Cyc_Tcpat_Con_s*)_T1;}return _T0;}
# 990
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_T0;char _T1;int _T2;{struct Cyc_Tcpat_Con_s*_T3=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T1=c;_T2=(int)_T1;
_T3->name=Cyc_Tcpat_Int_v(_T2);_T3->arity=0;_T3->span=& Cyc_Tcpat_twofiftysix_opt;_T3->orig_pat=Cyc_Tcpat_pw(p);_T0=(struct Cyc_Tcpat_Con_s*)_T3;}return _T0;}
# 993
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){struct Cyc_Tcpat_Con_s*_T0;struct _fat_ptr _T1;{struct Cyc_Tcpat_Con_s*_T2=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T1=
_tag_fat("$",sizeof(char),2U);_T2->name=Cyc_Tcpat_Name_v(_T1);_T2->arity=i;_T2->span=& Cyc_Tcpat_one_opt;_T2->orig_pat=p;_T0=(struct Cyc_Tcpat_Con_s*)_T2;}return _T0;}
# 998
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T0;void*_T1;{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T2=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_T2->tag=1;
_T2->f1=Cyc_Tcpat_null_con(p);_T2->f2=0;_T0=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 1001
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T0;void*_T1;{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T2=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_T2->tag=1;
_T2->f1=Cyc_Tcpat_int_con(i,p);_T2->f2=0;_T0=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 1004
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T0;void*_T1;{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T2=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_T2->tag=1;
_T2->f1=Cyc_Tcpat_char_con(c,p);_T2->f2=0;_T0=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 1007
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T0;void*_T1;{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T2=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_T2->tag=1;
_T2->f1=Cyc_Tcpat_float_con(f,p);_T2->f2=0;_T0=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 1010
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T0;struct Cyc_List_List*_T1;void*_T2;{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T3=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_T3->tag=1;
_T3->f1=Cyc_Tcpat_null_ptr_con(p0);{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));_T4->hd=p;_T4->tl=0;_T1=(struct Cyc_List_List*)_T4;}_T3->f2=_T1;_T0=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T3;}_T2=(void*)_T0;return _T2;}
# 1013
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T0;struct Cyc_List_List*_T1;void*_T2;{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T3=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_T3->tag=1;
_T3->f1=Cyc_Tcpat_ptr_con(p0);{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));_T4->hd=p;_T4->tl=0;_T1=(struct Cyc_List_List*)_T4;}_T3->f2=_T1;_T0=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T3;}_T2=(void*)_T0;return _T2;}
# 1016
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T0;int _T1;union Cyc_Tcpat_PatOrWhere _T2;void*_T3;{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T4=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_T4->tag=1;_T1=
Cyc_List_length(ss);_T2=p;_T4->f1=Cyc_Tcpat_tuple_con(_T1,_T2);_T4->f2=ss;_T0=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T4;}_T3=(void*)_T0;return _T3;}
# 1019
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){struct Cyc_Tcpat_Con_s*_T0;struct Cyc_Tcpat_Con_s*_T1;struct Cyc_Tcpat_Con_s*_T2;struct Cyc_Tcpat_Con_s*_T3;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T4;void*_T5;
# 1021
struct Cyc_Tcpat_Con_s*c;c=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T0=c;_T0->name=Cyc_Tcpat_Name_v(con_name);_T1=c;_T1->arity=Cyc_List_length(ps);_T2=c;_T2->span=span;_T3=c;_T3->orig_pat=Cyc_Tcpat_pw(p);{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T6=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_T6->tag=1;
_T6->f1=c;_T6->f2=ps;_T4=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T6;}_T5=(void*)_T4;return _T5;}
# 1026
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_T3;int _T4;union Cyc_Tcpat_PatOrWhere _T5;struct Cyc_Absyn_Pat*_T6;void*_T7;void*_T8;int*_T9;int _TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrAtts _TC;void*_TD;int _TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;int(*_T10)(struct _fat_ptr);void*(*_T11)(struct _fat_ptr);struct _fat_ptr _T12;struct Cyc_Absyn_Pat*_T13;void*_T14;void*_T15;int*_T16;int _T17;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18;void*_T19;int*_T1A;unsigned _T1B;int*_T1C;struct Cyc_Absyn_Datatypedecl*_T1D;int _T1E;int*_T1F;struct Cyc_Absyn_Datatypedecl*_T20;struct Cyc_Core_Opt*_T21;struct Cyc_Core_Opt*_T22;void*_T23;struct Cyc_List_List*_T24;struct Cyc_Warn_String_Warn_Warg_struct _T25;int(*_T26)(struct _fat_ptr);void*(*_T27)(struct _fat_ptr);struct _fat_ptr _T28;struct Cyc_Absyn_Datatypefield*_T29;struct _tuple2*_T2A;struct _tuple2 _T2B;struct _fat_ptr*_T2C;struct _fat_ptr _T2D;int*_T2E;struct Cyc_List_List*(*_T2F)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*);struct Cyc_List_List*(*_T30)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T31;struct Cyc_List_List*_T32;struct Cyc_Absyn_Pat*_T33;void*_T34;void*_T35;int*_T36;unsigned _T37;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T38;void*_T39;int*_T3A;int _T3B;void*_T3C;struct Cyc_Absyn_Aggrdecl*_T3D;struct Cyc_Absyn_Aggrdecl*_T3E;struct Cyc_Absyn_AggrdeclImpl*_T3F;struct Cyc_Absyn_AggrdeclImpl*_T40;struct Cyc_Absyn_Aggrdecl*_T41;struct Cyc_Absyn_AggrdeclImpl*_T42;struct Cyc_Warn_String_Warn_Warg_struct _T43;int(*_T44)(struct _fat_ptr);void*(*_T45)(struct _fat_ptr);struct _fat_ptr _T46;enum Cyc_Absyn_AggrKind _T47;int _T48;struct Cyc_List_List*_T49;void*_T4A;struct Cyc_Absyn_Aggrfield*_T4B;struct _fat_ptr*_T4C;struct _fat_ptr _T4D;struct _fat_ptr _T4E;int _T4F;int _T50;struct Cyc_List_List*_T51;void*_T52;struct Cyc_List_List*_T53;struct Cyc_List_List*_T54;void*_T55;int*_T56;int _T57;struct Cyc_List_List*_T58;struct Cyc_List_List*_T59;struct Cyc_List_List*_T5A;struct Cyc_List_List*_T5B;void*_T5C;struct _fat_ptr*_T5D;struct Cyc_List_List*_T5E;void*_T5F;struct Cyc_Absyn_Aggrfield*_T60;struct _fat_ptr*_T61;int _T62;struct Cyc_List_List*_T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;int(*_T65)(struct _fat_ptr);void*(*_T66)(struct _fat_ptr);struct _fat_ptr _T67;struct Cyc_List_List*_T68;int _T69;struct Cyc_Warn_String_Warn_Warg_struct _T6A;int(*_T6B)(struct _fat_ptr);void*(*_T6C)(struct _fat_ptr);struct _fat_ptr _T6D;struct Cyc_List_List*_T6E;struct Cyc_List_List*_T6F;union Cyc_Tcpat_PatOrWhere _T70;int _T71;struct Cyc_Warn_String_Warn_Warg_struct _T72;struct Cyc_Absyn_Pat*_T73;unsigned _T74;struct _fat_ptr _T75;int*_T76;struct Cyc_List_List*_T77;struct Cyc_List_List*_T78;struct _tuple16*_T79;struct _tuple16*_T7A;struct Cyc_List_List*_T7B;struct Cyc_List_List*_T7C;struct Cyc_List_List*_T7D;struct _tuple16*_T7E;struct _tuple16*_T7F;struct Cyc_List_List*_T80;struct Cyc_List_List*_T81;void*_T82;int*_T83;int _T84;struct Cyc_List_List*_T85;struct Cyc_List_List*_T86;struct _tuple16*_T87;struct _tuple16*_T88;struct Cyc_List_List*_T89;struct Cyc_List_List*_T8A;struct Cyc_List_List*_T8B;struct Cyc_List_List*_T8C;struct Cyc_List_List*_T8D;struct Cyc_List_List*_T8E;struct Cyc_List_List*_T8F;void*_T90;struct _tuple16*_T91;struct Cyc_List_List*_T92;void*_T93;struct _fat_ptr*_T94;struct _fat_ptr _T95;int*_T96;struct Cyc_List_List*_T97;struct Cyc_Absyn_Pat*_T98;struct Cyc_Warn_String_Warn_Warg_struct _T99;int(*_T9A)(struct _fat_ptr);void*(*_T9B)(struct _fat_ptr);struct _fat_ptr _T9C;struct Cyc_Absyn_Enumfield*_T9D;struct _tuple2*_T9E;struct _tuple2 _T9F;struct _fat_ptr*_TA0;struct _fat_ptr _TA1;struct Cyc_Absyn_Pat*_TA2;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_TA3;void*_TA4;
void*s;_T0=p;{
void*_TA5=_T0->r;struct Cyc_Absyn_Enumfield*_TA6;struct Cyc_List_List*_TA7;struct Cyc_Absyn_Datatypefield*_TA8;void*_TA9;struct _fat_ptr _TAA;char _TAB;int _TAC;enum Cyc_Absyn_Sign _TAD;_T1=(int*)_TA5;_T2=*_T1;switch(_T2){case 0: goto _LL4;case 2: _LL4: goto _LL6;case 4: _LL6:{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_TAE=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));
# 1031
_TAE->tag=0;_T3=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_TAE;}s=(void*)_T3;goto _LL0;case 8:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_TA5;_TAD=_TAE->f1;_TAC=_TAE->f2;}{enum Cyc_Absyn_Sign sn=_TAD;int i=_TAC;_T4=i;_T5=
Cyc_Tcpat_pw(p);s=Cyc_Tcpat_int_pat(_T4,_T5);goto _LL0;}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_TA5;_TAB=_TAE->f1;}{char c=_TAB;
s=Cyc_Tcpat_char_pat(c,p);goto _LL0;}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_TA5;_TAA=_TAE->f1;}{struct _fat_ptr f=_TAA;
s=Cyc_Tcpat_float_pat(f,p);goto _LL0;}case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_TA5;_TA9=_TAE->f2;}{struct Cyc_Absyn_Pat*p2=_TA9;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_TA5;_TA9=_TAE->f2;}{struct Cyc_Absyn_Pat*p2=_TA9;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_TA5;_TA9=_TAE->f1;}{struct Cyc_Absyn_Pat*pp=_TA9;_T6=p;_T7=_T6->topt;_T8=
# 1039
_check_null(_T7);{void*_TAE=Cyc_Absyn_compress(_T8);void*_TAF;_T9=(int*)_TAE;_TA=*_T9;if(_TA!=4)goto _TL15C;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TB0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TAE;_TB=_TB0->f1;_TC=_TB.ptr_atts;_TAF=_TC.nullable;}{void*n=_TAF;
# 1041
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
void*ss=Cyc_Tcpat_compile_pat(pp);_TE=is_nullable;
if(!_TE)goto _TL15E;_TD=Cyc_Tcpat_null_ptr_pat(ss,p);goto _TL15F;_TL15E: _TD=Cyc_Tcpat_ptr_pat(ss,p);_TL15F: s=_TD;goto _LL23;}_TL15C:{struct Cyc_Warn_String_Warn_Warg_struct _TB0;_TB0.tag=0;
# 1045
_TB0.f1=_tag_fat("expecting pointertype for pattern!",sizeof(char),35U);_TF=_TB0;}{struct Cyc_Warn_String_Warn_Warg_struct _TB0=_TF;void*_TB1[1];_TB1[0]=& _TB0;_T11=Cyc_Warn_impos2;{int(*_TB2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T11;_T10=_TB2;}_T12=_tag_fat(_TB1,sizeof(void*),1);_T10(_T12);}_LL23:;}goto _LL0;}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_TA5;_TA9=_TAE->f1;_TA8=_TAE->f2;_TA7=_TAE->f3;}{struct Cyc_Absyn_Datatypedecl*tud=_TA9;struct Cyc_Absyn_Datatypefield*tuf=_TA8;struct Cyc_List_List*ps=_TA7;
# 1049
int*span;_T13=p;_T14=_T13->topt;_T15=
_check_null(_T14);{void*_TAE=Cyc_Absyn_compress(_T15);_T16=(int*)_TAE;_T17=*_T16;if(_T17!=0)goto _TL160;_T18=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TAE;_T19=_T18->f1;_T1A=(int*)_T19;_T1B=*_T1A;switch(_T1B){case 22: _T1D=tud;_T1E=_T1D->is_extensible;
# 1052
if(!_T1E)goto _TL163;_T1C=0;goto _TL164;_TL163:{int*_TAF=_cycalloc_atomic(sizeof(int));_T20=tud;_T21=_T20->fields;_T22=_check_null(_T21);_T23=_T22->v;_T24=(struct Cyc_List_List*)_T23;*_TAF=Cyc_List_length(_T24);_T1F=(int*)_TAF;}_T1C=_T1F;_TL164: span=_T1C;goto _LL28;case 23:
# 1054
 span=& Cyc_Tcpat_one_opt;goto _LL28;default: goto _LL2D;}goto _TL161;_TL160: _LL2D:{struct Cyc_Warn_String_Warn_Warg_struct _TAF;_TAF.tag=0;
_TAF.f1=_tag_fat("void datatype pattern has bad type",sizeof(char),35U);_T25=_TAF;}{struct Cyc_Warn_String_Warn_Warg_struct _TAF=_T25;void*_TB0[1];_TB0[0]=& _TAF;_T27=Cyc_Warn_impos2;{int(*_TB1)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T27;_T26=_TB1;}_T28=_tag_fat(_TB0,sizeof(void*),1);_T26(_T28);}_TL161: _LL28:;}_T29=tuf;_T2A=_T29->name;_T2B=*_T2A;_T2C=_T2B.f1;_T2D=*_T2C;_T2E=span;_T30=Cyc_List_map;{
# 1057
struct Cyc_List_List*(*_TAE)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))_T30;_T2F=_TAE;}_T31=ps;_T32=_T2F(Cyc_Tcpat_compile_pat,_T31);_T33=p;s=Cyc_Tcpat_con_pat(_T2D,_T2E,_T32,_T33);goto _LL0;}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_TA5;_T34=_TAE->f1;_TA9=(void*)_T34;_TA7=_TAE->f4;}{void*atype=_TA9;struct Cyc_List_List*dlps=_TA7;
# 1061
struct Cyc_List_List*fields;
enum Cyc_Absyn_AggrKind aggr_kind;
int is_tagged=0;_T35=
_check_null(atype);{void*_TAE=Cyc_Absyn_compress(_T35);struct Cyc_List_List*_TAF;enum Cyc_Absyn_AggrKind _TB0;union Cyc_Absyn_AggrInfo _TB1;_T36=(int*)_TAE;_T37=*_T36;switch(_T37){case 0: _T38=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TAE;_T39=_T38->f1;_T3A=(int*)_T39;_T3B=*_T3A;if(_T3B!=24)goto _TL166;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TAE;_T3C=_TB2->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TB3=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T3C;_TB1=_TB3->f1;}}{union Cyc_Absyn_AggrInfo ainfo=_TB1;
# 1066
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);_T3D=ad;
aggr_kind=_T3D->kind;_T3E=ad;_T3F=_T3E->impl;_T40=
_check_null(_T3F);fields=_T40->fields;_T41=ad;_T42=_T41->impl;
is_tagged=_T42->tagged;goto _LL2F;}_TL166: goto _LL34;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_TB2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_TAE;_TB0=_TB2->f1;_TAF=_TB2->f3;}{enum Cyc_Absyn_AggrKind ak=_TB0;struct Cyc_List_List*fs=_TAF;
# 1072
aggr_kind=ak;
fields=fs;goto _LL2F;}default: _LL34:{struct Cyc_Warn_String_Warn_Warg_struct _TB2;_TB2.tag=0;
# 1075
_TB2.f1=_tag_fat("bad type on aggr pattern",sizeof(char),25U);_T43=_TB2;}{struct Cyc_Warn_String_Warn_Warg_struct _TB2=_T43;void*_TB3[1];_TB3[0]=& _TB2;_T45=Cyc_Warn_impos2;{int(*_TB4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T45;_T44=_TB4;}_T46=_tag_fat(_TB3,sizeof(void*),1);_T44(_T46);}}_LL2F:;}_T47=aggr_kind;_T48=(int)_T47;
# 1079
if(_T48!=0)goto _TL168;{
struct Cyc_List_List*ps=0;
_TL16D: if(fields!=0)goto _TL16B;else{goto _TL16C;}
# 1083
_TL16B: _T49=fields;_T4A=_T49->hd;_T4B=(struct Cyc_Absyn_Aggrfield*)_T4A;_T4C=_T4B->name;_T4D=*_T4C;_T4E=_tag_fat("",sizeof(char),1U);_T4F=Cyc_strcmp(_T4D,_T4E);{int found=_T4F==0;{
struct Cyc_List_List*dlps0=dlps;_TL171: _T50=found;if(_T50)goto _TL170;else{goto _TL172;}_TL172: if(dlps0!=0)goto _TL16F;else{goto _TL170;}
_TL16F: _T51=dlps0;_T52=_T51->hd;{struct _tuple16*_TAE=(struct _tuple16*)_T52;struct Cyc_Absyn_Pat*_TAF;struct Cyc_List_List*_TB0;{struct _tuple16 _TB1=*_TAE;_TB0=_TB1.f0;_TAF=_TB1.f1;}{struct Cyc_List_List*dl=_TB0;struct Cyc_Absyn_Pat*p=_TAF;struct _fat_ptr*_TB1;if(dl==0)goto _TL173;_T53=dl;_T54=(struct Cyc_List_List*)_T53;_T55=_T54->hd;_T56=(int*)_T55;_T57=*_T56;if(_T57!=1)goto _TL175;_T58=dl;_T59=(struct Cyc_List_List*)_T58;_T5A=_T59->tl;if(_T5A!=0)goto _TL177;_T5B=dl;{struct Cyc_List_List _TB2=*_T5B;_T5C=_TB2.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TB3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T5C;_TB1=_TB3->f1;}}{struct _fat_ptr*f=_TB1;_T5D=f;_T5E=fields;_T5F=_T5E->hd;_T60=(struct Cyc_Absyn_Aggrfield*)_T5F;_T61=_T60->name;_T62=
# 1088
Cyc_strptrcmp(_T5D,_T61);if(_T62!=0)goto _TL179;{struct Cyc_List_List*_TB2=_cycalloc(sizeof(struct Cyc_List_List));
_TB2->hd=Cyc_Tcpat_compile_pat(p);_TB2->tl=ps;_T63=(struct Cyc_List_List*)_TB2;}ps=_T63;
found=1;goto _TL17A;_TL179: _TL17A: goto _LL39;}_TL177: goto _LL3C;_TL175: goto _LL3C;_TL173: _LL3C:{struct Cyc_Warn_String_Warn_Warg_struct _TB2;_TB2.tag=0;
# 1093
_TB2.f1=_tag_fat("bad designator(s)",sizeof(char),18U);_T64=_TB2;}{struct Cyc_Warn_String_Warn_Warg_struct _TB2=_T64;void*_TB3[1];_TB3[0]=& _TB2;_T66=Cyc_Warn_impos2;{int(*_TB4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T66;_T65=_TB4;}_T67=_tag_fat(_TB3,sizeof(void*),1);_T65(_T67);}_LL39:;}}_T68=dlps0;
# 1084
dlps0=_T68->tl;goto _TL171;_TL170:;}_T69=found;
# 1096
if(_T69)goto _TL17B;else{goto _TL17D;}
_TL17D:{struct Cyc_Warn_String_Warn_Warg_struct _TAE;_TAE.tag=0;_TAE.f1=_tag_fat("bad designator",sizeof(char),15U);_T6A=_TAE;}{struct Cyc_Warn_String_Warn_Warg_struct _TAE=_T6A;void*_TAF[1];_TAF[0]=& _TAE;_T6C=Cyc_Warn_impos2;{int(*_TB0)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T6C;_T6B=_TB0;}_T6D=_tag_fat(_TAF,sizeof(void*),1);_T6B(_T6D);}goto _TL17C;_TL17B: _TL17C:;}_T6E=fields;
# 1081
fields=_T6E->tl;goto _TL16D;_TL16C: _T6F=
# 1099
Cyc_List_imp_rev(ps);_T70=Cyc_Tcpat_pw(p);s=Cyc_Tcpat_tuple_pat(_T6F,_T70);}goto _TL169;
# 1102
_TL168: _T71=is_tagged;if(_T71)goto _TL17E;else{goto _TL180;}
_TL180:{struct Cyc_Warn_String_Warn_Warg_struct _TAE;_TAE.tag=0;_TAE.f1=_tag_fat("patterns on untagged unions not yet supported.",sizeof(char),47U);_T72=_TAE;}{struct Cyc_Warn_String_Warn_Warg_struct _TAE=_T72;void*_TAF[1];_TAF[0]=& _TAE;_T73=p;_T74=_T73->loc;_T75=_tag_fat(_TAF,sizeof(void*),1);Cyc_Warn_err2(_T74,_T75);}goto _TL17F;_TL17E: _TL17F:{
int*span;span=_cycalloc_atomic(sizeof(int));_T76=span;*_T76=Cyc_List_length(fields);{struct Cyc_Absyn_Pat*_TAE;struct _fat_ptr*_TAF;if(dlps==0)goto _TL181;_T77=dlps;_T78=(struct Cyc_List_List*)_T77;_T79=_T78->hd;_T7A=(struct _tuple16*)_T79;_T7B=_T7A->f0;if(_T7B==0)goto _TL183;_T7C=dlps;_T7D=(struct Cyc_List_List*)_T7C;_T7E=_T7D->hd;_T7F=(struct _tuple16*)_T7E;_T80=_T7F->f0;_T81=(struct Cyc_List_List*)_T80;_T82=_T81->hd;_T83=(int*)_T82;_T84=*_T83;if(_T84!=1)goto _TL185;_T85=dlps;_T86=(struct Cyc_List_List*)_T85;_T87=_T86->hd;_T88=(struct _tuple16*)_T87;_T89=_T88->f0;_T8A=(struct Cyc_List_List*)_T89;_T8B=_T8A->tl;if(_T8B!=0)goto _TL187;_T8C=dlps;_T8D=(struct Cyc_List_List*)_T8C;_T8E=_T8D->tl;if(_T8E!=0)goto _TL189;_T8F=dlps;{struct Cyc_List_List _TB0=*_T8F;_T90=_TB0.hd;_T91=(struct _tuple16*)_T90;{struct _tuple16 _TB1=*_T91;_T92=_TB1.f0;{struct Cyc_List_List _TB2=*_T92;_T93=_TB2.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TB3=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T93;_TAF=_TB3->f1;}}_TAE=_TB1.f1;}}{struct _fat_ptr*f=_TAF;struct Cyc_Absyn_Pat*p2=_TAE;_T94=f;_T95=*_T94;_T96=span;{struct Cyc_List_List*_TB0=_cycalloc(sizeof(struct Cyc_List_List));
# 1107
_TB0->hd=Cyc_Tcpat_compile_pat(p2);_TB0->tl=0;_T97=(struct Cyc_List_List*)_TB0;}_T98=p;s=Cyc_Tcpat_con_pat(_T95,_T96,_T97,_T98);goto _LL3E;}_TL189: goto _LL41;_TL187: goto _LL41;_TL185: goto _LL41;_TL183: goto _LL41;_TL181: _LL41:{struct Cyc_Warn_String_Warn_Warg_struct _TB0;_TB0.tag=0;
# 1109
_TB0.f1=_tag_fat("bad union pattern",sizeof(char),18U);_T99=_TB0;}{struct Cyc_Warn_String_Warn_Warg_struct _TB0=_T99;void*_TB1[1];_TB1[0]=& _TB0;_T9B=Cyc_Warn_impos2;{int(*_TB2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T9B;_T9A=_TB2;}_T9C=_tag_fat(_TB1,sizeof(void*),1);_T9A(_T9C);}_LL3E:;}}_TL169: goto _LL0;}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_TA5;_TA6=_TAE->f2;}{struct Cyc_Absyn_Enumfield*ef=_TA6;_TA6=ef;goto _LL1C;}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_TAE=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_TA5;_TA6=_TAE->f2;}_LL1C: {struct Cyc_Absyn_Enumfield*ef=_TA6;_T9D=ef;_T9E=_T9D->name;_T9F=*_T9E;_TA0=_T9F.f1;_TA1=*_TA0;_TA2=p;
# 1120
s=Cyc_Tcpat_con_pat(_TA1,0,0,_TA2);goto _LL0;}case 14: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22:{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_TAE=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));
# 1124
_TAE->tag=0;_TA3=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_TAE;}s=(void*)_TA3;}_LL0:;}_TA4=s;
# 1126
return _TA4;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1151
static int Cyc_Tcpat_same_access(void*a1,void*a2){struct _tuple0 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;int _T6;void*_T7;int*_T8;int _T9;void*_TA;int*_TB;int _TC;void*_TD;void*_TE;int _TF;void*_T10;int*_T11;int _T12;void*_T13;void*_T14;int _T15;int _T16;{struct _tuple0 _T17;
_T17.f0=a1;_T17.f1=a2;_T0=_T17;}{struct _tuple0 _T17=_T0;struct _fat_ptr*_T18;int _T19;struct _fat_ptr*_T1A;int _T1B;unsigned _T1C;struct Cyc_Absyn_Datatypefield*_T1D;unsigned _T1E;struct Cyc_Absyn_Datatypefield*_T1F;_T1=_T17.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 0: _T4=_T17.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL18C;
return 1;_TL18C: goto _LL9;case 1: _T7=_T17.f1;_T8=(int*)_T7;_T9=*_T8;if(_T9!=1)goto _TL18E;
return 1;_TL18E: goto _LL9;case 2: _TA=_T17.f1;_TB=(int*)_TA;_TC=*_TB;if(_TC!=2)goto _TL190;_TD=_T17.f0;{struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T20=(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_TD;_T1F=_T20->f2;_T1E=_T20->f3;}_TE=_T17.f1;{struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T20=(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_TE;_T1D=_T20->f2;_T1C=_T20->f3;}{struct Cyc_Absyn_Datatypefield*df1=_T1F;unsigned i1=_T1E;struct Cyc_Absyn_Datatypefield*df2=_T1D;unsigned i2=_T1C;
# 1156
if(df1!=df2)goto _TL192;_TF=i1==i2;goto _TL193;_TL192: _TF=0;_TL193: return _TF;}_TL190: goto _LL9;default: _T10=_T17.f1;_T11=(int*)_T10;_T12=*_T11;if(_T12!=3)goto _TL194;_T13=_T17.f0;{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T20=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T13;_T1B=_T20->f2;_T1A=_T20->f3;}_T14=_T17.f1;{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T20=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T14;_T19=_T20->f2;_T18=_T20->f3;}{int b1=_T1B;struct _fat_ptr*f1=_T1A;int b2=_T19;struct _fat_ptr*f2=_T18;
# 1158
if(b1!=b2)goto _TL196;_T16=Cyc_strptrcmp(f1,f2);_T15=_T16==0;goto _TL197;_TL196: _T15=0;_TL197: return _T15;}_TL194: _LL9:
 return 0;};}}
# 1163
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Tcpat_PathNode*_T2;void*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_Tcpat_PathNode*_T6;void*_T7;int _T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;
_TL19B: if(p1!=0)goto _TL19C;else{goto _TL19A;}_TL19C: if(p2!=0)goto _TL199;else{goto _TL19A;}
_TL199: _T0=p1;_T1=_T0->hd;_T2=(struct Cyc_Tcpat_PathNode*)_T1;_T3=_T2->access;_T4=p2;_T5=_T4->hd;_T6=(struct Cyc_Tcpat_PathNode*)_T5;_T7=_T6->access;_T8=Cyc_Tcpat_same_access(_T3,_T7);if(_T8)goto _TL19D;else{goto _TL19F;}_TL19F: return 0;_TL19D: _T9=p1;
# 1164
p1=_T9->tl;_TA=p2;p2=_TA->tl;goto _TL19B;_TL19A:
# 1166
 if(p1==p2)goto _TL1A0;return 0;_TL1A0:
 return 1;}
# 1170
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){struct Cyc_Tcpat_Con_s*_T0;union Cyc_Tcpat_PatOrWhere _T1;void*_T2;int*_T3;int _T4;void*_T5;void*_T6;int _T7;struct Cyc_Tcpat_Con_s*_T8;union Cyc_Tcpat_PatOrWhere _T9;struct _union_PatOrWhere_pattern _TA;unsigned _TB;unsigned _TC;int _TD;struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_TE;struct Cyc_List_List*_TF;struct _tuple0*_T10;void*_T11;struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_T12;struct Cyc_List_List*_T13;struct _tuple0*_T14;void*_T15;_T0=con;_T1=_T0->orig_pat;{
void*test=Cyc_Tcpat_get_pat_test(_T1);void*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;_T2=d2;_T3=(int*)_T2;_T4=*_T3;if(_T4!=2)goto _TL1A2;_T5=d2;{struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_T19=(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_T5;_T18=_T19->f1;_T17=_T19->f2;_T6=_T19->f3;_T16=(void*)_T6;}{struct Cyc_List_List*access2=_T18;struct Cyc_List_List*switch_clauses=_T17;void*default_decision=_T16;_T7=
# 1174
Cyc_Tcpat_same_path(access,access2);if(!_T7)goto _TL1A4;_T8=con;_T9=_T8->orig_pat;_TA=_T9.pattern;_TB=_TA.tag;_TC=_TB==1;_TD=(int)_TC;if(!_TD)goto _TL1A4;{struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_T19=_cycalloc(sizeof(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct));_T19->tag=2;
_T19->f1=access2;{struct Cyc_List_List*_T1A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T1B=_cycalloc(sizeof(struct _tuple0));
_T1B->f0=test;_T1B->f1=d1;_T10=(struct _tuple0*)_T1B;}_T1A->hd=_T10;_T1A->tl=switch_clauses;_TF=(struct Cyc_List_List*)_T1A;}_T19->f2=_TF;
_T19->f3=default_decision;_TE=(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_T19;}_T11=(void*)_TE;
# 1175
return _T11;_TL1A4: goto _LL4;}_TL1A2: _LL4:{struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_T19=_cycalloc(sizeof(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct));_T19->tag=2;
# 1179
_T19->f1=access;{struct Cyc_List_List*_T1A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T1B=_cycalloc(sizeof(struct _tuple0));_T1B->f0=test;_T1B->f1=d1;_T14=(struct _tuple0*)_T1B;}_T1A->hd=_T14;_T1A->tl=0;_T13=(struct Cyc_List_List*)_T1A;}_T19->f2=_T13;_T19->f3=d2;_T12=(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_T19;}_T15=(void*)_T12;return _T15;;}}struct _tuple21{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1191
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1194
static void Cyc_Tcpat_print_tab(int i){struct Cyc___cycFILE*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;
_TL1A9: if(i!=0)goto _TL1A7;else{goto _TL1A8;}
_TL1A7: _T0=Cyc_stderr;_T1=_tag_fat(" ",sizeof(char),2U);_T2=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T0,_T1,_T2);
# 1195
i=i + -1;goto _TL1A9;_TL1A8:;}
# 1198
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){struct Cyc_Tcpat_Con_s*_T0;struct _union_Name_value_Name_v _T1;unsigned _T2;struct _union_Name_value_Name_v _T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc___cycFILE*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct _union_Name_value_Int_v _T8;struct Cyc_Int_pa_PrintArg_struct _T9;int _TA;struct Cyc___cycFILE*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;_T0=c;{
union Cyc_Tcpat_Name_value _TE=_T0->name;int _TF;struct _fat_ptr _T10;_T1=_TE.Name_v;_T2=_T1.tag;if(_T2!=1)goto _TL1AA;_T3=_TE.Name_v;_T10=_T3.val;{struct _fat_ptr s=_T10;{struct Cyc_String_pa_PrintArg_struct _T11;_T11.tag=0;
_T11.f1=s;_T4=_T11;}{struct Cyc_String_pa_PrintArg_struct _T11=_T4;void*_T12[1];_T12[0]=& _T11;_T5=Cyc_stderr;_T6=_tag_fat("%s",sizeof(char),3U);_T7=_tag_fat(_T12,sizeof(void*),1);Cyc_fprintf(_T5,_T6,_T7);}goto _LL0;}_TL1AA: _T8=_TE.Int_v;_TF=_T8.val;{int i=_TF;{struct Cyc_Int_pa_PrintArg_struct _T11;_T11.tag=1;_TA=i;
_T11.f1=(unsigned long)_TA;_T9=_T11;}{struct Cyc_Int_pa_PrintArg_struct _T11=_T9;void*_T12[1];_T12[0]=& _T11;_TB=Cyc_stderr;_TC=_tag_fat("%d",sizeof(char),3U);_TD=_tag_fat(_T12,sizeof(void*),1);Cyc_fprintf(_TB,_TC,_TD);}goto _LL0;}_LL0:;}}
# 1205
static void Cyc_Tcpat_print_access(void*a){void*_T0;int*_T1;unsigned _T2;struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;void*_T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc_Absyn_Datatypefield*_TB;struct _tuple2*_TC;struct Cyc_Int_pa_PrintArg_struct _TD;unsigned _TE;int _TF;struct Cyc___cycFILE*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;void*_T13;int _T14;struct Cyc___cycFILE*_T15;struct _fat_ptr _T16;struct _fat_ptr _T17;struct Cyc_String_pa_PrintArg_struct _T18;struct _fat_ptr*_T19;struct Cyc___cycFILE*_T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;struct _fat_ptr*_T1D;int _T1E;unsigned _T1F;struct Cyc_Absyn_Datatypefield*_T20;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=Cyc_stderr;_T4=
# 1207
_tag_fat("DUMMY",sizeof(char),6U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3,_T4,_T5);goto _LL0;case 1: _T6=Cyc_stderr;_T7=
_tag_fat("*",sizeof(char),2U);_T8=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6,_T7,_T8);goto _LL0;case 2: _T9=a;{struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T21=(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_T9;_T20=_T21->f2;_T1F=_T21->f3;}{struct Cyc_Absyn_Datatypefield*f=_T20;unsigned i=_T1F;{struct Cyc_String_pa_PrintArg_struct _T21;_T21.tag=0;_TB=f;_TC=_TB->name;
# 1210
_T21.f1=Cyc_Absynpp_qvar2string(_TC);_TA=_T21;}{struct Cyc_String_pa_PrintArg_struct _T21=_TA;{struct Cyc_Int_pa_PrintArg_struct _T22;_T22.tag=1;_TE=i;_TF=(int)_TE;_T22.f1=(unsigned long)_TF;_TD=_T22;}{struct Cyc_Int_pa_PrintArg_struct _T22=_TD;void*_T23[2];_T23[0]=& _T21;_T23[1]=& _T22;_T10=Cyc_stderr;_T11=_tag_fat("%s[%d]",sizeof(char),7U);_T12=_tag_fat(_T23,sizeof(void*),2);Cyc_fprintf(_T10,_T11,_T12);}}goto _LL0;}default: _T13=a;{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T21=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T13;_T1E=_T21->f2;_T1D=_T21->f3;}{int tagged=_T1E;struct _fat_ptr*f=_T1D;_T14=tagged;
# 1212
if(!_T14)goto _TL1AD;_T15=Cyc_stderr;_T16=
_tag_fat(".tagunion",sizeof(char),10U);_T17=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T15,_T16,_T17);goto _TL1AE;_TL1AD: _TL1AE:{struct Cyc_String_pa_PrintArg_struct _T21;_T21.tag=0;_T19=f;
_T21.f1=*_T19;_T18=_T21;}{struct Cyc_String_pa_PrintArg_struct _T21=_T18;void*_T22[1];_T22[0]=& _T21;_T1A=Cyc_stderr;_T1B=_tag_fat(".%s",sizeof(char),4U);_T1C=_tag_fat(_T22,sizeof(void*),1);Cyc_fprintf(_T1A,_T1B,_T1C);}goto _LL0;}}_LL0:;}
# 1219
static void Cyc_Tcpat_print_pat_test(void*p){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T4;struct Cyc_Absyn_Exp*_T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;void*_T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc___cycFILE*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct Cyc___cycFILE*_TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc___cycFILE*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;void*_T14;void*_T15;struct Cyc_String_pa_PrintArg_struct _T16;struct Cyc_Absyn_Enumfield*_T17;struct _tuple2*_T18;struct Cyc___cycFILE*_T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;void*_T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc___cycFILE*_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;void*_T21;struct Cyc_Int_pa_PrintArg_struct _T22;unsigned _T23;int _T24;struct Cyc___cycFILE*_T25;struct _fat_ptr _T26;struct _fat_ptr _T27;void*_T28;struct Cyc_Int_pa_PrintArg_struct _T29;int _T2A;struct Cyc___cycFILE*_T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;void*_T2E;struct Cyc_String_pa_PrintArg_struct _T2F;struct _fat_ptr*_T30;struct Cyc_Int_pa_PrintArg_struct _T31;int _T32;struct Cyc___cycFILE*_T33;struct _fat_ptr _T34;struct _fat_ptr _T35;void*_T36;struct Cyc_String_pa_PrintArg_struct _T37;struct Cyc_Absyn_Datatypefield*_T38;struct _tuple2*_T39;struct Cyc___cycFILE*_T3A;struct _fat_ptr _T3B;struct _fat_ptr _T3C;struct Cyc_Absyn_Datatypefield*_T3D;struct _fat_ptr*_T3E;int _T3F;unsigned _T40;struct _fat_ptr _T41;struct Cyc_Absyn_Enumfield*_T42;struct Cyc_Absyn_Exp*_T43;_T0=p;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=p;_T4=(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_T3;_T5=_T4->f1;if(_T5!=0)goto _TL1B0;_T6=Cyc_stderr;_T7=
# 1221
_tag_fat("where(NULL)",sizeof(char),12U);_T8=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6,_T7,_T8);goto _LL0;_TL1B0: _T9=p;{struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T44=(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_T9;_T43=_T44->f1;}{struct Cyc_Absyn_Exp*e=_T43;{struct Cyc_String_pa_PrintArg_struct _T44;_T44.tag=0;
_T44.f1=Cyc_Absynpp_exp2string(e);_TA=_T44;}{struct Cyc_String_pa_PrintArg_struct _T44=_TA;void*_T45[1];_T45[0]=& _T44;_TB=Cyc_stderr;_TC=_tag_fat("where(%s)",sizeof(char),10U);_TD=_tag_fat(_T45,sizeof(void*),1);Cyc_fprintf(_TB,_TC,_TD);}goto _LL0;}case 1: _TE=Cyc_stderr;_TF=
_tag_fat("NULL",sizeof(char),5U);_T10=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TE,_TF,_T10);goto _LL0;case 2: _T11=Cyc_stderr;_T12=
_tag_fat("NOT-NULL:",sizeof(char),10U);_T13=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T11,_T12,_T13);goto _LL0;case 4: _T14=p;{struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_T44=(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_T14;_T42=_T44->f2;}{struct Cyc_Absyn_Enumfield*ef=_T42;_T42=ef;goto _LLC;}case 3: _T15=p;{struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_T44=(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_T15;_T42=_T44->f2;}_LLC: {struct Cyc_Absyn_Enumfield*ef=_T42;{struct Cyc_String_pa_PrintArg_struct _T44;_T44.tag=0;_T17=ef;_T18=_T17->name;
# 1226
_T44.f1=Cyc_Absynpp_qvar2string(_T18);_T16=_T44;}{struct Cyc_String_pa_PrintArg_struct _T44=_T16;void*_T45[1];_T45[0]=& _T44;_T19=Cyc_stderr;_T1A=_tag_fat("%s",sizeof(char),3U);_T1B=_tag_fat(_T45,sizeof(void*),1);Cyc_fprintf(_T19,_T1A,_T1B);}goto _LL0;}case 5: _T1C=p;{struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_T44=(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_T1C;_T41=_T44->f1;}{struct _fat_ptr s=_T41;{struct Cyc_String_pa_PrintArg_struct _T44;_T44.tag=0;
_T44.f1=s;_T1D=_T44;}{struct Cyc_String_pa_PrintArg_struct _T44=_T1D;void*_T45[1];_T45[0]=& _T44;_T1E=Cyc_stderr;_T1F=_tag_fat("%s",sizeof(char),3U);_T20=_tag_fat(_T45,sizeof(void*),1);Cyc_fprintf(_T1E,_T1F,_T20);}goto _LL0;}case 6: _T21=p;{struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T44=(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_T21;_T40=_T44->f1;}{unsigned i=_T40;{struct Cyc_Int_pa_PrintArg_struct _T44;_T44.tag=1;_T23=i;_T24=(int)_T23;
_T44.f1=(unsigned long)_T24;_T22=_T44;}{struct Cyc_Int_pa_PrintArg_struct _T44=_T22;void*_T45[1];_T45[0]=& _T44;_T25=Cyc_stderr;_T26=_tag_fat("%d",sizeof(char),3U);_T27=_tag_fat(_T45,sizeof(void*),1);Cyc_fprintf(_T25,_T26,_T27);}goto _LL0;}case 7: _T28=p;{struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_T44=(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_T28;_T3F=_T44->f1;}{int i=_T3F;{struct Cyc_Int_pa_PrintArg_struct _T44;_T44.tag=1;_T2A=i;
_T44.f1=(unsigned long)_T2A;_T29=_T44;}{struct Cyc_Int_pa_PrintArg_struct _T44=_T29;void*_T45[1];_T45[0]=& _T44;_T2B=Cyc_stderr;_T2C=_tag_fat("datatypetag(%d)",sizeof(char),16U);_T2D=_tag_fat(_T45,sizeof(void*),1);Cyc_fprintf(_T2B,_T2C,_T2D);}goto _LL0;}case 8: _T2E=p;{struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T44=(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_T2E;_T3E=_T44->f2;_T3F=_T44->f3;}{struct _fat_ptr*f=_T3E;int i=_T3F;{struct Cyc_String_pa_PrintArg_struct _T44;_T44.tag=0;_T30=f;
_T44.f1=*_T30;_T2F=_T44;}{struct Cyc_String_pa_PrintArg_struct _T44=_T2F;{struct Cyc_Int_pa_PrintArg_struct _T45;_T45.tag=1;_T32=i;_T45.f1=(unsigned long)_T32;_T31=_T45;}{struct Cyc_Int_pa_PrintArg_struct _T45=_T31;void*_T46[2];_T46[0]=& _T44;_T46[1]=& _T45;_T33=Cyc_stderr;_T34=_tag_fat("uniontag[%s](%d)",sizeof(char),17U);_T35=_tag_fat(_T46,sizeof(void*),2);Cyc_fprintf(_T33,_T34,_T35);}}goto _LL0;}default: _T36=p;{struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_T44=(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_T36;_T3D=_T44->f2;}{struct Cyc_Absyn_Datatypefield*f=_T3D;{struct Cyc_String_pa_PrintArg_struct _T44;_T44.tag=0;_T38=f;_T39=_T38->name;
# 1232
_T44.f1=Cyc_Absynpp_qvar2string(_T39);_T37=_T44;}{struct Cyc_String_pa_PrintArg_struct _T44=_T37;void*_T45[1];_T45[0]=& _T44;_T3A=Cyc_stderr;_T3B=_tag_fat("datatypefield(%s)",sizeof(char),18U);_T3C=_tag_fat(_T45,sizeof(void*),1);Cyc_fprintf(_T3A,_T3B,_T3C);}}}_LL0:;}
# 1236
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){struct Cyc_String_pa_PrintArg_struct _T0;struct Cyc_Tcpat_Rhs*_T1;struct Cyc_Absyn_Stmt*_T2;struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;{struct Cyc_String_pa_PrintArg_struct _T6;_T6.tag=0;_T1=r;_T2=_T1->rhs;
_T6.f1=Cyc_Absynpp_stmt2string(_T2);_T0=_T6;}{struct Cyc_String_pa_PrintArg_struct _T6=_T0;void*_T7[1];_T7[0]=& _T6;_T3=Cyc_stderr;_T4=_tag_fat("%s",sizeof(char),3U);_T5=_tag_fat(_T7,sizeof(void*),1);Cyc_fprintf(_T3,_T4,_T5);}}
# 1240
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc___cycFILE*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc___cycFILE*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;void*_TD;void*_TE;struct Cyc___cycFILE*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct Cyc_List_List*_T12;void*_T13;struct Cyc_Tcpat_PathNode*_T14;void*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;struct Cyc___cycFILE*_T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;struct Cyc_List_List*_T1B;struct Cyc___cycFILE*_T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;struct Cyc_List_List*_T1F;void*_T20;struct _tuple0*_T21;struct Cyc___cycFILE*_T22;struct _fat_ptr _T23;struct _fat_ptr _T24;struct Cyc___cycFILE*_T25;struct _fat_ptr _T26;struct _fat_ptr _T27;void*_T28;int _T29;struct Cyc_List_List*_T2A;struct Cyc___cycFILE*_T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;void*_T2E;int _T2F;struct Cyc___cycFILE*_T30;struct _fat_ptr _T31;struct _fat_ptr _T32;struct Cyc_List_List*_T33;struct Cyc_List_List*_T34;void*_T35;_T0=d;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 1: _T3=d;{struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_T36=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_T3;_T35=_T36->f1;}{struct Cyc_Tcpat_Rhs*rhs=_T35;
# 1243
Cyc_Tcpat_print_tab(tab);_T4=Cyc_stderr;_T5=
_tag_fat("Success(",sizeof(char),9U);_T6=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4,_T5,_T6);Cyc_Tcpat_print_rhs(rhs);_T7=Cyc_stderr;_T8=_tag_fat(")\n",sizeof(char),3U);_T9=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T7,_T8,_T9);goto _LL0;}case 0: _TA=Cyc_stderr;_TB=
# 1246
_tag_fat("Failure\n",sizeof(char),9U);_TC=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TA,_TB,_TC);goto _LL0;default: _TD=d;{struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_T36=(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_TD;_T34=_T36->f1;_T33=_T36->f2;_TE=_T36->f3;_T35=(void*)_TE;}{struct Cyc_List_List*a=_T34;struct Cyc_List_List*cases=_T33;void*def=_T35;
# 1248
Cyc_Tcpat_print_tab(tab);_TF=Cyc_stderr;_T10=
_tag_fat("Switch[",sizeof(char),8U);_T11=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TF,_T10,_T11);
_TL1B6: if(a!=0)goto _TL1B4;else{goto _TL1B5;}
_TL1B4: _T12=a;_T13=_T12->hd;_T14=(struct Cyc_Tcpat_PathNode*)_T13;_T15=_T14->access;Cyc_Tcpat_print_access(_T15);_T16=a;_T17=_T16->tl;
if(_T17==0)goto _TL1B7;_T18=Cyc_stderr;_T19=_tag_fat(",",sizeof(char),2U);_T1A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T18,_T19,_T1A);goto _TL1B8;_TL1B7: _TL1B8: _T1B=a;
# 1250
a=_T1B->tl;goto _TL1B6;_TL1B5: _T1C=Cyc_stderr;_T1D=
# 1254
_tag_fat("] {\n",sizeof(char),5U);_T1E=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1C,_T1D,_T1E);
_TL1BC: if(cases!=0)goto _TL1BA;else{goto _TL1BB;}
_TL1BA: _T1F=cases;_T20=_T1F->hd;_T21=(struct _tuple0*)_T20;{struct _tuple0 _T36=*_T21;void*_T37;void*_T38;_T38=_T36.f0;_T37=_T36.f1;{void*pt=_T38;void*d=_T37;
Cyc_Tcpat_print_tab(tab);_T22=Cyc_stderr;_T23=
_tag_fat("case ",sizeof(char),6U);_T24=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T22,_T23,_T24);
Cyc_Tcpat_print_pat_test(pt);_T25=Cyc_stderr;_T26=
_tag_fat(":\n",sizeof(char),3U);_T27=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T25,_T26,_T27);_T28=d;_T29=tab + 7;
Cyc_Tcpat_print_dec_tree(_T28,_T29);}}_T2A=cases;
# 1255
cases=_T2A->tl;goto _TL1BC;_TL1BB:
# 1263
 Cyc_Tcpat_print_tab(tab);_T2B=Cyc_stderr;_T2C=
_tag_fat("default:\n",sizeof(char),10U);_T2D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2B,_T2C,_T2D);_T2E=def;_T2F=tab + 7;
Cyc_Tcpat_print_dec_tree(_T2E,_T2F);
Cyc_Tcpat_print_tab(tab);_T30=Cyc_stderr;_T31=
_tag_fat("}\n",sizeof(char),3U);_T32=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T30,_T31,_T32);}}_LL0:;}
# 1271
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1279
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_T4;struct Cyc_Set_Set*(*_T5)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);struct Cyc_Set_Set*(*_T6)(struct Cyc_Set_Set*,void*);void*_T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;int(*_T9)(struct _fat_ptr);void*(*_TA)(struct _fat_ptr);struct _fat_ptr _TB;struct Cyc_Set_Set*_TC;_T0=td;_T1=(int*)_T0;_T2=*_T1;if(_T2!=1)goto _TL1BD;_T3=td;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_TD=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_T3;_TC=_TD->f1;}{struct Cyc_Set_Set*cs=_TC;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_TD=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_TD->tag=1;_T6=Cyc_Set_insert;{
# 1289
struct Cyc_Set_Set*(*_TE)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_T6;_T5=_TE;}_TD->f1=_T5(cs,c);_T4=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_TD;}_T7=(void*)_T4;return _T7;}_TL1BD:{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;
_TD.f1=_tag_fat("add_neg called when td is Positive",sizeof(char),35U);_T8=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T8;void*_TE[1];_TE[0]=& _TD;_TA=Cyc_Warn_impos2;{int(*_TF)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TA;_T9=_TF;}_TB=_tag_fat(_TE,sizeof(void*),1);_T9(_TB);};}
# 1296
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){void*_T0;int*_T1;int _T2;void*_T3;enum Cyc_Tcpat_Answer _T4;int _T5;void*_T6;int(*_T7)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_T8)(struct Cyc_Set_Set*,void*);int _T9;struct Cyc_Tcpat_Con_s*_TA;int*_TB;struct Cyc_Tcpat_Con_s*_TC;int*_TD;int _TE;int _TF;int _T10;struct Cyc_Set_Set*_T11;struct Cyc_Tcpat_Con_s*_T12;_T0=td;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL1BF;_T3=td;{struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T13=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_T3;_T12=_T13->f1;}{struct Cyc_Tcpat_Con_s*c2=_T12;_T5=
# 1300
Cyc_Tcpat_compare_con(c,c2);if(_T5!=0)goto _TL1C1;_T4=0U;goto _TL1C2;_TL1C1: _T4=1U;_TL1C2: return _T4;}_TL1BF: _T6=td;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_T13=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_T6;_T11=_T13->f1;}{struct Cyc_Set_Set*cs=_T11;_T8=Cyc_Set_member;{
# 1303
int(*_T13)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_T8;_T7=_T13;}_T9=_T7(cs,c);if(!_T9)goto _TL1C3;return 1U;_TL1C3: _TA=c;_TB=_TA->span;
# 1306
if(_TB==0)goto _TL1C5;_TC=c;_TD=_TC->span;_TE=*_TD;_TF=Cyc_Set_cardinality(cs);_T10=_TF + 1;if(_TE!=_T10)goto _TL1C5;
return 0U;_TL1C5:
# 1309
 return 2U;};}struct _tuple22{struct Cyc_Tcpat_Con_s*f0;struct Cyc_List_List*f1;};
# 1317
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){struct Cyc_List_List*_T0;void*_T1;struct _tuple22*_T2;struct Cyc_List_List*_T3;struct _tuple22*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_Tcpat_Con_s*_T8;if(ctxt!=0)goto _TL1C7;
# 1319
return 0;_TL1C7: _T0=ctxt;{struct Cyc_List_List _T9=*_T0;_T1=_T9.hd;_T2=(struct _tuple22*)_T1;{struct _tuple22 _TA=*_T2;_T8=_TA.f0;_T7=_TA.f1;}_T6=_T9.tl;}{struct Cyc_Tcpat_Con_s*c=_T8;struct Cyc_List_List*args=_T7;struct Cyc_List_List*tl=_T6;{struct Cyc_List_List*_T9=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple22*_TA=_cycalloc(sizeof(struct _tuple22));
_TA->f0=c;{struct Cyc_List_List*_TB=_cycalloc(sizeof(struct Cyc_List_List));_TB->hd=dsc;_TB->tl=args;_T5=(struct Cyc_List_List*)_TB;}_TA->f1=_T5;_T4=(struct _tuple22*)_TA;}_T9->hd=_T4;_T9->tl=tl;_T3=(struct Cyc_List_List*)_T9;}return _T3;};}
# 1327
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){struct Cyc_Warn_String_Warn_Warg_struct _T0;int(*_T1)(struct _fat_ptr);void*(*_T2)(struct _fat_ptr);struct _fat_ptr _T3;struct Cyc_List_List*_T4;void*_T5;struct _tuple22*_T6;struct Cyc_List_List*_T7;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T8;void*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_Tcpat_Con_s*_TD;if(ctxt!=0)goto _TL1C9;{struct Cyc_Warn_String_Warn_Warg_struct _TE;_TE.tag=0;
# 1329
_TE.f1=_tag_fat("norm_context: empty context",sizeof(char),28U);_T0=_TE;}{struct Cyc_Warn_String_Warn_Warg_struct _TE=_T0;void*_TF[1];_TF[0]=& _TE;_T2=Cyc_Warn_impos2;{int(*_T10)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T2;_T1=_T10;}_T3=_tag_fat(_TF,sizeof(void*),1);_T1(_T3);}goto _TL1CA;_TL1C9: _T4=ctxt;{struct Cyc_List_List _TE=*_T4;_T5=_TE.hd;_T6=(struct _tuple22*)_T5;{struct _tuple22 _TF=*_T6;_TD=_TF.f0;_TC=_TF.f1;}_TB=_TE.tl;}{struct Cyc_Tcpat_Con_s*c=_TD;struct Cyc_List_List*args=_TC;struct Cyc_List_List*tl=_TB;_T7=tl;{struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_TE=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct));_TE->tag=0;
_TE->f1=c;_TE->f2=Cyc_List_rev(args);_T8=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_TE;}_T9=(void*)_T8;_TA=Cyc_Tcpat_augment(_T7,_T9);return _TA;}_TL1CA:;}
# 1339
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple1 _T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;int(*_T6)(struct _fat_ptr);void*(*_T7)(struct _fat_ptr);struct _fat_ptr _T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple22*_TB;struct Cyc_List_List*_TC;void*_TD;struct _tuple21*_TE;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_TF;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T10;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T15;void*_T16;struct Cyc_List_List*_T17;void*_T18;{struct _tuple1 _T19;
# 1341
_T19.f0=ctxt;_T19.f1=work;_T0=_T19;}{struct _tuple1 _T19=_T0;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_Tcpat_Con_s*_T1E;_T1=_T19.f0;if(_T1!=0)goto _TL1CB;_T2=_T19.f1;if(_T2!=0)goto _TL1CD;_T3=dsc;
return _T3;_TL1CD: goto _LL6;_TL1CB: _T4=_T19.f1;if(_T4!=0)goto _TL1CF;_LL6:{struct Cyc_Warn_String_Warn_Warg_struct _T1F;_T1F.tag=0;
# 1344
_T1F.f1=_tag_fat("build_desc: ctxt and work don't match",sizeof(char),38U);_T5=_T1F;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F=_T5;void*_T20[1];_T20[0]=& _T1F;_T7=Cyc_Warn_impos2;{int(*_T21)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T7;_T6=_T21;}_T8=_tag_fat(_T20,sizeof(void*),1);_T6(_T8);}goto _TL1D0;_TL1CF: _T9=_T19.f0;{struct Cyc_List_List _T1F=*_T9;_TA=_T1F.hd;_TB=(struct _tuple22*)_TA;{struct _tuple22 _T20=*_TB;_T1E=_T20.f0;_T1D=_T20.f1;}_T1C=_T1F.tl;}_TC=_T19.f1;{struct Cyc_List_List _T1F=*_TC;_TD=_T1F.hd;_TE=(struct _tuple21*)_TD;{struct _tuple21 _T20=*_TE;_T1B=_T20.f2;}_T1A=_T1F.tl;}{struct Cyc_Tcpat_Con_s*c=_T1E;struct Cyc_List_List*args=_T1D;struct Cyc_List_List*rest=_T1C;struct Cyc_List_List*dargs=_T1B;struct Cyc_List_List*work2=_T1A;
# 1346
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*td;td=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct));_TF=td;_TF->tag=0;_T10=td;_T10->f1=c;_T11=td;_T12=Cyc_List_rev(args);{struct Cyc_List_List*_T1F=_cycalloc(sizeof(struct Cyc_List_List));_T1F->hd=dsc;_T1F->tl=dargs;_T13=(struct Cyc_List_List*)_T1F;}_T11->f2=Cyc_List_append(_T12,_T13);_T14=rest;_T15=td;_T16=(void*)_T15;_T17=work2;_T18=
Cyc_Tcpat_build_desc(_T14,_T16,_T17);return _T18;}_TL1D0:;}}
# 1351
static void*Cyc_Tcpat_match(void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Tcpat_Rhs*,struct Cyc_List_List*);struct _tuple23{void*f0;struct Cyc_Tcpat_Rhs*f1;};
# 1356
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple23*_T4;void*_T5;struct Cyc_List_List*_T6;struct Cyc_Tcpat_Rhs*_T7;void*_T8;if(allmrules!=0)goto _TL1D1;{struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_T9=_cycalloc(sizeof(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct));_T9->tag=0;
# 1358
_T9->f1=dsc;_T0=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_T9;}_T1=(void*)_T0;return _T1;_TL1D1: _T2=allmrules;{struct Cyc_List_List _T9=*_T2;_T3=_T9.hd;_T4=(struct _tuple23*)_T3;{struct _tuple23 _TA=*_T4;_T8=_TA.f0;_T7=_TA.f1;}_T6=_T9.tl;}{void*pat1=_T8;struct Cyc_Tcpat_Rhs*rhs1=_T7;struct Cyc_List_List*rulerest=_T6;_T5=
# 1360
Cyc_Tcpat_match(pat1,0,dsc,0,0,rhs1,rulerest);return _T5;};}
# 1365
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_T4=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_T4->tag=1;
_T4->f1=Cyc_Tcpat_empty_con_set();_T0=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_T4;}_T1=(void*)_T0;_T2=allmrules;_T3=Cyc_Tcpat_or_match(_T1,_T2);return _T3;}
# 1372
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_T0;void*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct _tuple21*_T4;struct _tuple21*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct _tuple21*_T9;struct _tuple21*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct _tuple21*_TE;struct _tuple21*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_Tcpat_Rhs*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_List_List*_T17;void*_T18;struct _tuple21*_T19;struct Cyc_Warn_String_Warn_Warg_struct _T1A;int(*_T1B)(struct _fat_ptr);void*(*_T1C)(struct _fat_ptr);struct _fat_ptr _T1D;void*_T1E;void*_T1F;void*_T20;struct _tuple21*_T21;struct _tuple21*_T22;struct _tuple21*_T23;void*_T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_Tcpat_Rhs*_T29;struct Cyc_List_List*_T2A;void*_T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;if(work!=0)goto _TL1D3;{struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_T30=_cycalloc(sizeof(struct Cyc_Tcpat_Success_Tcpat_Decision_struct));_T30->tag=1;
# 1376
_T30->f1=right_hand_side;_T0=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_T30;}_T1=(void*)_T0;return _T1;_TL1D3: _T2=work;_T3=(struct Cyc_List_List*)_T2;_T4=_T3->hd;_T5=(struct _tuple21*)_T4;_T6=_T5->f0;if(_T6!=0)goto _TL1D5;_T7=work;_T8=(struct Cyc_List_List*)_T7;_T9=_T8->hd;_TA=(struct _tuple21*)_T9;_TB=_TA->f1;if(_TB!=0)goto _TL1D7;_TC=work;_TD=(struct Cyc_List_List*)_TC;_TE=_TD->hd;_TF=(struct _tuple21*)_TE;_T10=_TF->f2;if(_T10!=0)goto _TL1D9;_T11=work;{struct Cyc_List_List _T30=*_T11;_T2F=_T30.tl;}{struct Cyc_List_List*workr=_T2F;_T12=
# 1378
Cyc_Tcpat_norm_context(ctx);_T13=workr;_T14=right_hand_side;_T15=rules;_T16=Cyc_Tcpat_and_match(_T12,_T13,_T14,_T15);return _T16;}_TL1D9: goto _LL5;_TL1D7: goto _LL5;_TL1D5: _LL5: _T17=work;{struct Cyc_List_List _T30=*_T17;_T18=_T30.hd;_T19=(struct _tuple21*)_T18;{struct _tuple21 _T31=*_T19;_T2F=_T31.f0;_T2E=_T31.f1;_T2D=_T31.f2;}_T2C=_T30.tl;}{struct Cyc_List_List*pats=_T2F;struct Cyc_List_List*objs=_T2E;struct Cyc_List_List*dscs=_T2D;struct Cyc_List_List*workr=_T2C;
# 1380
if(pats==0)goto _TL1DD;else{goto _TL1DF;}_TL1DF: if(objs==0)goto _TL1DD;else{goto _TL1DE;}_TL1DE: if(dscs==0)goto _TL1DD;else{goto _TL1DB;}
_TL1DD:{struct Cyc_Warn_String_Warn_Warg_struct _T30;_T30.tag=0;_T30.f1=_tag_fat("tcpat:and_match: malformed work frame",sizeof(char),38U);_T1A=_T30;}{struct Cyc_Warn_String_Warn_Warg_struct _T30=_T1A;void*_T31[1];_T31[0]=& _T30;_T1C=Cyc_Warn_impos2;{int(*_T32)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1C;_T1B=_T32;}_T1D=_tag_fat(_T31,sizeof(void*),1);_T1B(_T1D);}goto _TL1DC;_TL1DB: _TL1DC: {
struct Cyc_List_List*_T30=pats;struct Cyc_List_List*_T31;void*_T32;{struct Cyc_List_List _T33=*_T30;_T1E=_T33.hd;_T32=(void*)_T1E;_T31=_T33.tl;}{void*pat1=_T32;struct Cyc_List_List*patr=_T31;
struct Cyc_List_List*_T33=objs;struct Cyc_List_List*_T34;struct Cyc_List_List*_T35;{struct Cyc_List_List _T36=*_T33;_T1F=_T36.hd;_T35=(struct Cyc_List_List*)_T1F;_T34=_T36.tl;}{struct Cyc_List_List*obj1=_T35;struct Cyc_List_List*objr=_T34;
struct Cyc_List_List*_T36=dscs;struct Cyc_List_List*_T37;void*_T38;{struct Cyc_List_List _T39=*_T36;_T20=_T39.hd;_T38=(void*)_T20;_T37=_T39.tl;}{void*dsc1=_T38;struct Cyc_List_List*dscr=_T37;
struct _tuple21*wf;wf=_cycalloc(sizeof(struct _tuple21));_T21=wf;_T21->f0=patr;_T22=wf;_T22->f1=objr;_T23=wf;_T23->f2=dscr;_T24=pat1;_T25=obj1;_T26=dsc1;_T27=ctx;{struct Cyc_List_List*_T39=_cycalloc(sizeof(struct Cyc_List_List));
_T39->hd=wf;_T39->tl=workr;_T28=(struct Cyc_List_List*)_T39;}_T29=right_hand_side;_T2A=rules;_T2B=Cyc_Tcpat_match(_T24,_T25,_T26,_T27,_T28,_T29,_T2A);return _T2B;}}}}};}
# 1391
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_T4;int _T5;struct Cyc_Tcpat_Con_s*_T6;int _T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_Set_Set*_TD;_T0=dsc;_T1=(int*)_T0;_T2=*_T1;if(_T2!=1)goto _TL1E0;_T3=dsc;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_TE=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_T3;_TD=_TE->f1;}{struct Cyc_Set_Set*ncs=_TD;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_TE=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_TE->tag=1;
# 1397
_TE->f1=Cyc_Tcpat_empty_con_set();_T4=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_TE;}{void*any=(void*)_T4;
struct Cyc_List_List*res=0;{
int i=0;_TL1E5: _T5=i;_T6=pcon;_T7=_T6->arity;if(_T5 < _T7)goto _TL1E3;else{goto _TL1E4;}
_TL1E3:{struct Cyc_List_List*_TE=_cycalloc(sizeof(struct Cyc_List_List));_TE->hd=any;_TE->tl=res;_T8=(struct Cyc_List_List*)_TE;}res=_T8;
# 1399
i=i + 1;goto _TL1E5;_TL1E4:;}_T9=res;
# 1401
return _T9;}}_TL1E0: _TA=dsc;{struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_TE=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_TA;_TC=_TE->f2;}{struct Cyc_List_List*dargs=_TC;_TB=dargs;
return _TB;};}
# 1406
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){union Cyc_Tcpat_PatOrWhere _T0;struct _union_PatOrWhere_where_clause _T1;unsigned _T2;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*_T3;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*_T4;void*_T5;union Cyc_Tcpat_PatOrWhere _T6;struct _union_PatOrWhere_pattern _T7;struct Cyc_Absyn_Pat*_T8;int*_T9;unsigned _TA;struct Cyc_Int_pa_PrintArg_struct _TB;int _TC;int(*_TD)(struct _fat_ptr,struct _fat_ptr);void*(*_TE)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_Tcpat_Deref_Tcpat_Access_struct*_T11;struct Cyc_Absyn_Pat*_T12;void*_T13;struct Cyc_List_List*_T14;struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T15;int _T16;struct Cyc_List_List*_T17;void*_T18;struct Cyc_Absyn_Pat*_T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;int*_T1E;unsigned _T1F;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20;void*_T21;int*_T22;int _T23;void*_T24;struct Cyc_Absyn_Aggrdecl*_T25;struct Cyc_Absyn_AggrdeclImpl*_T26;struct Cyc_Absyn_AggrdeclImpl*_T27;struct Cyc_Absyn_Aggrdecl*_T28;struct Cyc_Absyn_AggrdeclImpl*_T29;int(*_T2A)(struct _fat_ptr,struct _fat_ptr);void*(*_T2B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2C;struct _fat_ptr _T2D;int _T2E;struct Cyc_List_List*_T2F;void*_T30;struct _tuple16*_T31;struct _tuple16 _T32;struct Cyc_List_List*_T33;void*_T34;int*_T35;int _T36;struct Cyc_List_List*_T37;struct Cyc_List_List*_T38;void*_T39;struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T3A;struct Cyc_List_List*_T3B;void*_T3C;struct _tuple16*_T3D;struct Cyc_Absyn_Pat*_T3E;void*_T3F;void*_T40;struct Cyc_String_pa_PrintArg_struct _T41;int(*_T42)(struct _fat_ptr,struct _fat_ptr);void*(*_T43)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T44;struct _fat_ptr _T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;void*_T48;struct Cyc_Absyn_Aggrfield*_T49;struct Cyc_List_List*_T4A;void*_T4B;struct _tuple16*_T4C;struct Cyc_List_List*_T4D;void*_T4E;int*_T4F;int _T50;struct Cyc_List_List*_T51;struct Cyc_List_List*_T52;void*_T53;int _T54;struct Cyc_List_List*_T55;void*_T56;struct _tuple16*_T57;struct Cyc_Absyn_Pat*_T58;void*_T59;int(*_T5A)(struct _fat_ptr,struct _fat_ptr);void*(*_T5B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T5C;struct _fat_ptr _T5D;struct Cyc_List_List*_T5E;struct Cyc_String_pa_PrintArg_struct _T5F;struct _fat_ptr*_T60;struct _fat_ptr _T61;int(*_T62)(struct _fat_ptr,struct _fat_ptr);void*(*_T63)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T64;struct _fat_ptr _T65;struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T66;struct Cyc_List_List*_T67;void*_T68;struct Cyc_Absyn_Aggrfield*_T69;void*_T6A;struct Cyc_String_pa_PrintArg_struct _T6B;int(*_T6C)(struct _fat_ptr,struct _fat_ptr);void*(*_T6D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T6E;struct _fat_ptr _T6F;struct Cyc_Absyn_Pat*_T70;_T0=pw;_T1=_T0.where_clause;_T2=_T1.tag;if(_T2!=2)goto _TL1E6;_T3=& Cyc_Tcpat_Dummy_val;_T4=(struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_T3;_T5=(void*)_T4;
# 1408
return _T5;_TL1E6: _T6=pw;_T7=_T6.pattern;_T70=_T7.val;{struct Cyc_Absyn_Pat*p=_T70;_T8=p;{
# 1410
void*_T71=_T8->r;struct Cyc_List_List*_T72;struct Cyc_Absyn_Datatypefield*_T73;void*_T74;_T9=(int*)_T71;_TA=*_T9;switch(_TA){case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T75=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T71;_T74=_T75->f1;}{struct Cyc_Absyn_Pat*p2=_T74;
# 1412
if(i==0)goto _TL1E9;{struct Cyc_Int_pa_PrintArg_struct _T75;_T75.tag=1;_TC=i;
_T75.f1=(unsigned long)_TC;_TB=_T75;}{struct Cyc_Int_pa_PrintArg_struct _T75=_TB;void*_T76[1];_T76[0]=& _T75;_TE=Cyc_Warn_impos;{int(*_T77)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TE;_TD=_T77;}_TF=_tag_fat("get_access on pointer pattern with offset %d\n",sizeof(char),46U);_T10=_tag_fat(_T76,sizeof(void*),1);_TD(_TF,_T10);}goto _TL1EA;_TL1E9: _TL1EA:{struct Cyc_Tcpat_Deref_Tcpat_Access_struct*_T75=_cycalloc(sizeof(struct Cyc_Tcpat_Deref_Tcpat_Access_struct));_T75->tag=1;_T12=p2;
_T75->f1=_T12->topt;_T11=(struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_T75;}_T13=(void*)_T11;return _T13;}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T75=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T71;_T74=_T75->f1;_T73=_T75->f2;_T72=_T75->f3;}{struct Cyc_Absyn_Datatypedecl*tud=_T74;struct Cyc_Absyn_Datatypefield*tuf=_T73;struct Cyc_List_List*args=_T72;
# 1416
int orig_i=i;
_TL1EE: if(i!=0)goto _TL1EC;else{goto _TL1ED;}_TL1EC: _T14=_check_null(args);args=_T14->tl;i=i + -1;goto _TL1EE;_TL1ED:{struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T75=_cycalloc(sizeof(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct));_T75->tag=2;
_T75->f1=tud;_T75->f2=tuf;_T16=orig_i;_T75->f3=(unsigned)_T16;_T17=_check_null(args);_T18=_T17->hd;_T19=(struct Cyc_Absyn_Pat*)_T18;_T1A=_T19->topt;_T75->f4=_check_null(_T1A);_T15=(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_T75;}_T1B=(void*)_T15;return _T1B;}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T75=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T71;_T1C=_T75->f1;_T74=(void*)_T1C;_T72=_T75->f4;}{void*atype=_T74;struct Cyc_List_List*dlps=_T72;
# 1420
struct Cyc_List_List*fields;
int is_tagged=0;_T1D=
_check_null(atype);{void*_T75=Cyc_Absyn_compress(_T1D);struct Cyc_List_List*_T76;union Cyc_Absyn_AggrInfo _T77;_T1E=(int*)_T75;_T1F=*_T1E;switch(_T1F){case 0: _T20=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T75;_T21=_T20->f1;_T22=(int*)_T21;_T23=*_T22;if(_T23!=24)goto _TL1F0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T78=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T75;_T24=_T78->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T79=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T24;_T77=_T79->f1;}}{union Cyc_Absyn_AggrInfo ainfo=_T77;
# 1424
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);_T25=ad;_T26=_T25->impl;_T27=
_check_null(_T26);is_tagged=_T27->tagged;_T28=ad;_T29=_T28->impl;
fields=_T29->fields;goto _LLE;}_TL1F0: goto _LL13;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T78=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T75;_T76=_T78->f3;}{struct Cyc_List_List*fs=_T76;
# 1429
fields=fs;goto _LLE;}default: _LL13: _T2B=Cyc_Warn_impos;{
# 1431
int(*_T78)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2B;_T2A=_T78;}_T2C=_tag_fat("bad type in aggr pattern",sizeof(char),25U);_T2D=_tag_fat(0U,sizeof(void*),0);_T2A(_T2C,_T2D);}_LLE:;}_T2E=is_tagged;
# 1433
if(!_T2E)goto _TL1F2;_T2F=
_check_null(dlps);_T30=_T2F->hd;_T31=(struct _tuple16*)_T30;_T32=*_T31;{struct Cyc_List_List*_T75=_T32.f0;struct _fat_ptr*_T76;if(_T75==0)goto _TL1F4;_T33=(struct Cyc_List_List*)_T75;_T34=_T33->hd;_T35=(int*)_T34;_T36=*_T35;if(_T36!=1)goto _TL1F6;_T37=(struct Cyc_List_List*)_T75;_T38=_T37->tl;if(_T38!=0)goto _TL1F8;{struct Cyc_List_List _T77=*_T75;_T39=_T77.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T78=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T39;_T76=_T78->f1;}}{struct _fat_ptr*f=_T76;{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T77=_cycalloc(sizeof(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct));_T77->tag=3;
# 1436
_T77->f1=atype;_T77->f2=is_tagged;_T77->f3=f;_T3B=dlps;_T3C=_T3B->hd;_T3D=(struct _tuple16*)_T3C;_T3E=_T3D->f1;_T3F=_T3E->topt;_T77->f4=_check_null(_T3F);_T3A=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T77;}_T40=(void*)_T3A;return _T40;}_TL1F8: goto _LL18;_TL1F6: goto _LL18;_TL1F4: _LL18:{struct Cyc_String_pa_PrintArg_struct _T77;_T77.tag=0;
# 1438
_T77.f1=Cyc_Absynpp_pat2string(p);_T41=_T77;}{struct Cyc_String_pa_PrintArg_struct _T77=_T41;void*_T78[1];_T78[0]=& _T77;_T43=Cyc_Warn_impos;{int(*_T79)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T43;_T42=_T79;}_T44=_tag_fat("get_access on bad aggr pattern: %s",sizeof(char),35U);_T45=_tag_fat(_T78,sizeof(void*),1);_T42(_T44,_T45);};}goto _TL1F3;_TL1F2: _TL1F3: {
# 1441
int orig_i=i;
_TL1FD: if(i!=0)goto _TL1FB;else{goto _TL1FC;}_TL1FB: _T46=_check_null(fields);fields=_T46->tl;i=i + -1;goto _TL1FD;_TL1FC: _T47=
_check_null(fields);_T48=_T47->hd;_T49=(struct Cyc_Absyn_Aggrfield*)_T48;{struct _fat_ptr*name=_T49->name;
void*field_type=Cyc_Absyn_void_type;
_TL201: if(dlps!=0)goto _TL1FF;else{goto _TL200;}
_TL1FF: _T4A=dlps;_T4B=_T4A->hd;_T4C=(struct _tuple16*)_T4B;{struct Cyc_List_List*_T75=_T4C->f0;struct _fat_ptr*_T76;if(_T75==0)goto _TL202;_T4D=(struct Cyc_List_List*)_T75;_T4E=_T4D->hd;_T4F=(int*)_T4E;_T50=*_T4F;if(_T50!=1)goto _TL204;_T51=(struct Cyc_List_List*)_T75;_T52=_T51->tl;if(_T52!=0)goto _TL206;{struct Cyc_List_List _T77=*_T75;_T53=_T77.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T78=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T53;_T76=_T78->f1;}}{struct _fat_ptr*f=_T76;_T54=
# 1448
Cyc_strptrcmp(f,name);if(_T54==0)goto _TL208;goto _TL1FE;_TL208: _T55=dlps;_T56=_T55->hd;_T57=(struct _tuple16*)_T56;_T58=_T57->f1;_T59=_T58->topt;
field_type=_check_null(_T59);goto _LL1A;}_TL206: goto _LL1D;_TL204: goto _LL1D;_TL202: _LL1D: _T5B=Cyc_Warn_impos;{
# 1452
int(*_T77)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T5B;_T5A=_T77;}_T5C=_tag_fat("get_access bad designator list",sizeof(char),31U);_T5D=_tag_fat(0U,sizeof(void*),0);_T5A(_T5C,_T5D);_LL1A:;}goto _TL200;_TL1FE: _T5E=dlps;
# 1445
dlps=_T5E->tl;goto _TL201;_TL200:
# 1456
 if(dlps!=0)goto _TL20A;{struct Cyc_String_pa_PrintArg_struct _T75;_T75.tag=0;_T60=name;_T61=*_T60;_T75.f1=_T61;_T5F=_T75;}{struct Cyc_String_pa_PrintArg_struct _T75=_T5F;void*_T76[1];_T76[0]=& _T75;_T63=Cyc_Warn_impos;{int(*_T77)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T63;_T62=_T77;}_T64=_tag_fat("get_access can't find field %s",sizeof(char),31U);_T65=_tag_fat(_T76,sizeof(void*),1);_T62(_T64,_T65);}goto _TL20B;_TL20A: _TL20B:{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T75=_cycalloc(sizeof(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct));_T75->tag=3;
_T75->f1=atype;_T75->f2=is_tagged;_T67=fields;_T68=_T67->hd;_T69=(struct Cyc_Absyn_Aggrfield*)_T68;_T75->f3=_T69->name;_T75->f4=field_type;_T66=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T75;}_T6A=(void*)_T66;return _T6A;}}}default:{struct Cyc_String_pa_PrintArg_struct _T75;_T75.tag=0;
_T75.f1=Cyc_Absynpp_pat2string(p);_T6B=_T75;}{struct Cyc_String_pa_PrintArg_struct _T75=_T6B;void*_T76[1];_T76[0]=& _T75;_T6D=Cyc_Warn_impos;{int(*_T77)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T6D;_T6C=_T77;}_T6E=_tag_fat("get_access on bad pattern: %s",sizeof(char),30U);_T6F=_tag_fat(_T76,sizeof(void*),1);_T6C(_T6E,_T6F);}};}};}struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Tcpat_Con_s*f1;};
# 1464
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple24*env,int i){struct _tuple24*_T0;struct Cyc_Tcpat_Con_s*_T1;union Cyc_Tcpat_PatOrWhere _T2;int _T3;struct Cyc_List_List*_T4;struct Cyc_Tcpat_PathNode*_T5;struct Cyc_Tcpat_Con_s*_T6;struct Cyc_Tcpat_Con_s*_T7;struct Cyc_List_List*_T8;_T0=env;{struct _tuple24 _T9=*_T0;_T8=_T9.f0;_T7=_T9.f1;}{struct Cyc_List_List*obj=_T8;struct Cyc_Tcpat_Con_s*pcon=_T7;_T1=pcon;_T2=_T1->orig_pat;_T3=i;{
# 1466
void*acc=Cyc_Tcpat_get_access(_T2,_T3);{struct Cyc_List_List*_T9=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Tcpat_PathNode*_TA=_cycalloc(sizeof(struct Cyc_Tcpat_PathNode));_T6=pcon;
_TA->orig_pat=_T6->orig_pat;_TA->access=acc;_T5=(struct Cyc_Tcpat_PathNode*)_TA;}_T9->hd=_T5;_T9->tl=obj;_T4=(struct Cyc_List_List*)_T9;}return _T4;}}}
# 1469
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){struct _tuple24 _T0;struct Cyc_List_List*(*_T1)(int,struct Cyc_List_List*(*)(struct _tuple24*,int),struct _tuple24*);struct Cyc_List_List*(*_T2)(int,void*(*)(void*,int),void*);struct Cyc_Tcpat_Con_s*_T3;int _T4;struct _tuple24*_T5;struct Cyc_List_List*_T6;{struct _tuple24 _T7;
_T7.f0=obj;_T7.f1=pcon;_T0=_T7;}{struct _tuple24 env=_T0;_T2=Cyc_List_tabulate_c;{
struct Cyc_List_List*(*_T7)(int,struct Cyc_List_List*(*)(struct _tuple24*,int),struct _tuple24*)=(struct Cyc_List_List*(*)(int,struct Cyc_List_List*(*)(struct _tuple24*,int),struct _tuple24*))_T2;_T1=_T7;}_T3=pcon;_T4=_T3->arity;_T5=& env;_T6=_T1(_T4,Cyc_Tcpat_getoarg,_T5);return _T6;}}
# 1477
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){void*_T0;int*_T1;int _T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_Tcpat_Rhs*_T5;struct Cyc_List_List*_T6;void*_T7;void*_T8;int _T9;struct Cyc_List_List*_TA;struct _tuple22*_TB;struct Cyc_List_List*_TC;struct _tuple21*_TD;struct _tuple21*_TE;struct _tuple21*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;void*_T12;void*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_List_List*_T16;struct _tuple22*_T17;struct Cyc_List_List*_T18;struct _tuple21*_T19;struct _tuple21*_T1A;struct _tuple21*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_List_List*_T24;struct Cyc_Tcpat_Con_s*_T25;_T0=p;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL20C;_T3=
# 1481
Cyc_Tcpat_augment(ctx,dsc);_T4=work;_T5=right_hand_side;_T6=rules;_T7=Cyc_Tcpat_and_match(_T3,_T4,_T5,_T6);return _T7;_TL20C: _T8=p;{struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_T26=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_T8;_T25=_T26->f1;_T24=_T26->f2;}{struct Cyc_Tcpat_Con_s*pcon=_T25;struct Cyc_List_List*pargs=_T24;
# 1483
enum Cyc_Tcpat_Answer _T26=Cyc_Tcpat_static_match(pcon,dsc);_T9=(int)_T26;switch(_T9){case Cyc_Tcpat_Yes:  {
# 1485
struct Cyc_List_List*ctx2;ctx2=_cycalloc(sizeof(struct Cyc_List_List));_TA=ctx2;{struct _tuple22*_T27=_cycalloc(sizeof(struct _tuple22));_T27->f0=pcon;_T27->f1=0;_TB=(struct _tuple22*)_T27;}_TA->hd=_TB;_TC=ctx2;_TC->tl=ctx;{
struct _tuple21*work_frame;work_frame=_cycalloc(sizeof(struct _tuple21));_TD=work_frame;_TD->f0=pargs;_TE=work_frame;_TE->f1=Cyc_Tcpat_getoargs(pcon,obj);_TF=work_frame;
_TF->f2=Cyc_Tcpat_getdargs(pcon,dsc);{
struct Cyc_List_List*work2;work2=_cycalloc(sizeof(struct Cyc_List_List));_T10=work2;_T10->hd=work_frame;_T11=work2;_T11->tl=work;_T12=
Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);return _T12;}}}case Cyc_Tcpat_No: _T13=
# 1491
Cyc_Tcpat_build_desc(ctx,dsc,work);_T14=rules;_T15=Cyc_Tcpat_or_match(_T13,_T14);return _T15;default:  {
# 1493
struct Cyc_List_List*ctx2;ctx2=_cycalloc(sizeof(struct Cyc_List_List));_T16=ctx2;{struct _tuple22*_T27=_cycalloc(sizeof(struct _tuple22));_T27->f0=pcon;_T27->f1=0;_T17=(struct _tuple22*)_T27;}_T16->hd=_T17;_T18=ctx2;_T18->tl=ctx;{
struct _tuple21*work_frame;work_frame=_cycalloc(sizeof(struct _tuple21));_T19=work_frame;_T19->f0=pargs;_T1A=work_frame;_T1A->f1=Cyc_Tcpat_getoargs(pcon,obj);_T1B=work_frame;
_T1B->f2=Cyc_Tcpat_getdargs(pcon,dsc);{
struct Cyc_List_List*work2;work2=_cycalloc(sizeof(struct Cyc_List_List));_T1C=work2;_T1C->hd=work_frame;_T1D=work2;_T1D->tl=work;{
void*s=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);_T1E=ctx;_T1F=
Cyc_Tcpat_add_neg(dsc,pcon);_T20=work;_T21=Cyc_Tcpat_build_desc(_T1E,_T1F,_T20);_T22=rules;{void*f=Cyc_Tcpat_or_match(_T21,_T22);_T23=
Cyc_Tcpat_ifeq(obj,pcon,s,f);return _T23;}}}}}};};}
# 1509
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;int*_T5;int _T6;int*_T7;void*_T8;void*_T9;void*_TA;struct Cyc_List_List*_TB;void*_TC;struct _tuple0*_TD;void*_TE;void(*_TF)(void*,void*);void*_T10;void(*_T11)(void*,struct Cyc_Tcpat_Rhs*);void*_T12;int*_T13;struct Cyc_List_List*_T14;void*_T15;void(*_T16)(void*,void*);void*_T17;void(*_T18)(void*,struct Cyc_Tcpat_Rhs*);void*_T19;int*_T1A;struct Cyc_List_List*_T1B;void*_T1C;_T0=d;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=d;{struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_T1D=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_T3;_T4=_T1D->f1;_T1C=(void*)_T4;}{void*td=_T1C;_T5=exhaust_done;_T6=*_T5;
# 1516
if(_T6)goto _TL210;else{goto _TL212;}_TL212: not_exhaust(env1,td);_T7=exhaust_done;*_T7=1;goto _TL211;_TL210: _TL211: goto _LL0;}case 1: _T8=d;{struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_T1D=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_T8;_T1C=_T1D->f1;}{struct Cyc_Tcpat_Rhs*r=_T1C;
# 1518
rhs_appears(env2,r);goto _LL0;}default: _T9=d;{struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_T1D=(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_T9;_T1B=_T1D->f2;_TA=_T1D->f3;_T1C=(void*)_TA;}{struct Cyc_List_List*cases=_T1B;void*def=_T1C;
# 1520
_TL216: if(cases!=0)goto _TL214;else{goto _TL215;}
_TL214: _TB=cases;_TC=_TB->hd;_TD=(struct _tuple0*)_TC;{struct _tuple0 _T1D=*_TD;void*_T1E;_T1E=_T1D.f1;{void*d=_T1E;_TE=d;_TF=not_exhaust;_T10=env1;_T11=rhs_appears;_T12=env2;_T13=exhaust_done;
Cyc_Tcpat_check_exhaust_overlap(_TE,_TF,_T10,_T11,_T12,_T13);}}_T14=cases;
# 1520
cases=_T14->tl;goto _TL216;_TL215: _T15=def;_T16=not_exhaust;_T17=env1;_T18=rhs_appears;_T19=env2;_T1A=exhaust_done;
# 1525
Cyc_Tcpat_check_exhaust_overlap(_T15,_T16,_T17,_T18,_T19,_T1A);goto _LL0;}}_LL0:;}
# 1535
static struct _tuple23*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){struct Cyc_Absyn_Switch_clause*_T0;struct Cyc_Absyn_Pat*_T1;struct Cyc_Tcpat_Rhs*_T2;struct Cyc_Tcpat_Rhs*_T3;struct Cyc_Absyn_Switch_clause*_T4;struct Cyc_Absyn_Pat*_T5;struct Cyc_Tcpat_Rhs*_T6;struct Cyc_Absyn_Switch_clause*_T7;struct Cyc_Absyn_Switch_clause*_T8;struct Cyc_Absyn_Exp*_T9;union Cyc_Tcpat_PatOrWhere _TA;struct Cyc_Absyn_Switch_clause*_TB;struct Cyc_List_List*_TC;int*_TD;int _TE;union Cyc_Tcpat_PatOrWhere _TF;struct _fat_ptr _T10;union Cyc_Tcpat_PatOrWhere _T11;int*_T12;int*_T13;int _T14;struct Cyc_List_List*_T15;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_T16;struct _fat_ptr _T17;union Cyc_Tcpat_PatOrWhere _T18;struct _tuple23*_T19;_T0=swc;_T1=_T0->pattern;{
void*sp0=Cyc_Tcpat_compile_pat(_T1);
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs));_T2=rhs;_T2->used=0;_T3=rhs;_T4=swc;_T5=_T4->pattern;_T3->pat_loc=_T5->loc;_T6=rhs;_T7=swc;_T6->rhs=_T7->body;{
void*sp;_T8=swc;_T9=_T8->where_clause;
if(_T9==0)goto _TL217;{union Cyc_Tcpat_PatOrWhere _T1A;_T1A.where_clause.tag=2U;_TB=swc;
_T1A.where_clause.val=_TB->where_clause;_TA=_T1A;}{union Cyc_Tcpat_PatOrWhere w=_TA;{void*_T1A[2];
_T1A[0]=sp0;_TD=ctr;_TE=*_TD;_TF=w;_T1A[1]=Cyc_Tcpat_int_pat(_TE,_TF);_T10=_tag_fat(_T1A,sizeof(void*),2);_TC=Cyc_List_list(_T10);}_T11=w;sp=Cyc_Tcpat_tuple_pat(_TC,_T11);_T12=ctr;_T13=ctr;_T14=*_T13;
*_T12=_T14 + 1;}goto _TL218;
# 1544
_TL217:{void*_T1A[2];_T1A[0]=sp0;{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_T1B=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));_T1B->tag=0;_T16=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_T1B;}_T1A[1]=(void*)_T16;_T17=_tag_fat(_T1A,sizeof(void*),2);_T15=Cyc_List_list(_T17);}{union Cyc_Tcpat_PatOrWhere _T1A;_T1A.where_clause.tag=2U;_T1A.where_clause.val=0;_T18=_T1A;}sp=Cyc_Tcpat_tuple_pat(_T15,_T18);_TL218:{struct _tuple23*_T1A=_cycalloc(sizeof(struct _tuple23));
_T1A->f0=sp;_T1A->f1=rhs;_T19=(struct _tuple23*)_T1A;}return _T19;}}}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1550
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1552
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Tcpat_Con_s*_T4;struct Cyc_Tcpat_Con_s*_T5;struct _union_PatOrWhere_where_clause _T6;unsigned _T7;struct Cyc_List_List*_T8;void*_T9;struct _fat_ptr _TA;struct _union_PatOrWhere_pattern _TB;struct Cyc_Absyn_Pat*_TC;int*_TD;unsigned _TE;struct _fat_ptr _TF;struct Cyc_Absyn_Vardecl*_T10;struct _tuple2*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_String_pa_PrintArg_struct _T14;struct Cyc_Absyn_Vardecl*_T15;struct _tuple2*_T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct _fat_ptr _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct Cyc_Absyn_Vardecl*_T1B;struct _tuple2*_T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc_Absyn_Tvar*_T1E;struct _fat_ptr*_T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct _fat_ptr _T22;struct Cyc_String_pa_PrintArg_struct _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;void*_T26;void*_T27;int*_T28;unsigned _T29;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2A;void*_T2B;int*_T2C;int _T2D;void*_T2E;struct Cyc_Absyn_Aggrdecl*_T2F;enum Cyc_Absyn_AggrKind _T30;int _T31;struct Cyc_List_List*_T32;struct Cyc_List_List*_T33;struct _tuple16*_T34;struct _tuple16*_T35;struct Cyc_List_List*_T36;struct Cyc_List_List*_T37;struct Cyc_List_List*_T38;struct _tuple16*_T39;struct _tuple16*_T3A;struct Cyc_List_List*_T3B;struct Cyc_List_List*_T3C;void*_T3D;int*_T3E;int _T3F;struct Cyc_List_List*_T40;void*_T41;struct _tuple16*_T42;struct Cyc_List_List*_T43;void*_T44;struct _fat_ptr _T45;struct Cyc_String_pa_PrintArg_struct _T46;struct Cyc_Absyn_Aggrdecl*_T47;struct _tuple2*_T48;struct Cyc_String_pa_PrintArg_struct _T49;struct _fat_ptr*_T4A;struct Cyc_String_pa_PrintArg_struct _T4B;struct _fat_ptr _T4C;struct _fat_ptr _T4D;struct _fat_ptr _T4E;struct Cyc_String_pa_PrintArg_struct _T4F;struct Cyc_Absyn_Aggrdecl*_T50;struct _tuple2*_T51;struct Cyc_String_pa_PrintArg_struct _T52;struct _fat_ptr _T53;struct _fat_ptr _T54;int _T55;struct _fat_ptr _T56;struct Cyc_String_pa_PrintArg_struct _T57;struct _fat_ptr _T58;struct _fat_ptr _T59;struct _fat_ptr _T5A;struct Cyc_String_pa_PrintArg_struct _T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;struct Cyc_Warn_String_Warn_Warg_struct _T5E;int(*_T5F)(struct _fat_ptr);void*(*_T60)(struct _fat_ptr);struct _fat_ptr _T61;struct _fat_ptr _T62;struct Cyc_String_pa_PrintArg_struct _T63;struct Cyc_Absyn_Datatypefield*_T64;struct _tuple2*_T65;struct _fat_ptr _T66;struct _fat_ptr _T67;struct _fat_ptr _T68;struct Cyc_String_pa_PrintArg_struct _T69;struct Cyc_Absyn_Datatypefield*_T6A;struct _tuple2*_T6B;struct Cyc_String_pa_PrintArg_struct _T6C;struct _fat_ptr _T6D;struct _fat_ptr _T6E;struct _fat_ptr _T6F;struct _fat_ptr _T70;struct Cyc_Int_pa_PrintArg_struct _T71;int _T72;struct _fat_ptr _T73;struct _fat_ptr _T74;struct _fat_ptr _T75;struct Cyc_Int_pa_PrintArg_struct _T76;char _T77;int _T78;struct _fat_ptr _T79;struct _fat_ptr _T7A;struct _fat_ptr _T7B;struct Cyc_Absyn_Enumfield*_T7C;struct _tuple2*_T7D;struct _fat_ptr _T7E;struct _fat_ptr _T7F;struct Cyc_Tcpat_Desc2string_exn_struct*_T80;struct Cyc_Tcpat_Desc2string_exn_struct*_T81;void*_T82;int _T83;struct _fat_ptr _T84;struct Cyc_Tcpat_Con_s*(*_T85)(struct Cyc_Set_Set*);void*(*_T86)(struct Cyc_Set_Set*);struct Cyc_Tcpat_Con_s*_T87;union Cyc_Tcpat_PatOrWhere _T88;struct _union_PatOrWhere_where_clause _T89;unsigned _T8A;unsigned _T8B;int _T8C;struct Cyc_Tcpat_Desc2string_exn_struct*_T8D;struct Cyc_Tcpat_Desc2string_exn_struct*_T8E;struct Cyc_Absyn_Pat*_T8F;struct Cyc_Tcpat_Con_s*_T90;struct _union_PatOrWhere_pattern _T91;unsigned _T92;struct _union_PatOrWhere_pattern _T93;struct Cyc_Absyn_Pat*_T94;void*_T95;void*_T96;int*_T97;unsigned _T98;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T99;void*_T9A;int*_T9B;unsigned _T9C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9D;void*_T9E;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T9F;enum Cyc_Absyn_Size_of _TA0;int(*_TA1)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_TA2)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_TA3;int _TA4;char _TA5;struct Cyc_Absyn_Pat*_TA6;struct Cyc_Tcpat_Con_s*_TA7;int _TA8;struct _fat_ptr _TA9;struct Cyc_Int_pa_PrintArg_struct _TAA;int _TAB;struct _fat_ptr _TAC;struct _fat_ptr _TAD;struct Cyc_Tcpat_Desc2string_exn_struct*_TAE;struct Cyc_Tcpat_Desc2string_exn_struct*_TAF;int(*_TB0)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_TB1)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_TB2;unsigned _TB3;int _TB4;struct Cyc_Tcpat_Con_s*_TB5;union Cyc_Tcpat_PatOrWhere _TB6;struct Cyc_Tcpat_Con_s*_TB7;int _TB8;struct _fat_ptr _TB9;struct Cyc_Int_pa_PrintArg_struct _TBA;unsigned _TBB;int _TBC;struct _fat_ptr _TBD;struct _fat_ptr _TBE;struct Cyc_Tcpat_Desc2string_exn_struct*_TBF;struct Cyc_Tcpat_Desc2string_exn_struct*_TC0;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC1;void*_TC2;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TC3;union Cyc_Absyn_DatatypeInfo _TC4;struct _union_DatatypeInfo_KnownDatatype _TC5;unsigned _TC6;void*_TC7;union Cyc_Absyn_DatatypeInfo _TC8;struct _union_DatatypeInfo_KnownDatatype _TC9;struct Cyc_Absyn_Datatypedecl**_TCA;struct Cyc_Absyn_Datatypedecl*_TCB;int _TCC;struct Cyc_Tcpat_Desc2string_exn_struct*_TCD;struct Cyc_Tcpat_Desc2string_exn_struct*_TCE;struct Cyc_Absyn_Datatypedecl*_TCF;struct Cyc_Core_Opt*_TD0;struct Cyc_Core_Opt*_TD1;void*_TD2;struct Cyc_List_List*_TD3;unsigned _TD4;struct Cyc_List_List*_TD5;void*_TD6;struct Cyc_Absyn_Datatypefield*_TD7;struct _tuple2*_TD8;struct _tuple2 _TD9;struct _fat_ptr*_TDA;struct Cyc_List_List*_TDB;void*_TDC;struct Cyc_Absyn_Datatypefield*_TDD;int(*_TDE)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_TDF)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_TE0;struct Cyc_Tcpat_Con_s*_TE1;struct Cyc_Tcpat_Con_s*_TE2;int _TE3;struct _fat_ptr _TE4;int _TE5;struct _fat_ptr _TE6;struct Cyc_String_pa_PrintArg_struct _TE7;struct _fat_ptr _TE8;struct _fat_ptr _TE9;struct Cyc_List_List*_TEA;struct Cyc_Tcpat_Desc2string_exn_struct*_TEB;struct Cyc_Tcpat_Desc2string_exn_struct*_TEC;void*_TED;struct Cyc_Absyn_Aggrdecl*_TEE;enum Cyc_Absyn_AggrKind _TEF;int _TF0;struct Cyc_Tcpat_Desc2string_exn_struct*_TF1;struct Cyc_Tcpat_Desc2string_exn_struct*_TF2;struct Cyc_Absyn_Aggrdecl*_TF3;struct Cyc_Absyn_AggrdeclImpl*_TF4;struct Cyc_Absyn_AggrdeclImpl*_TF5;struct Cyc_Absyn_Aggrdecl*_TF6;struct _fat_ptr*_TF7;struct Cyc_List_List*_TF8;unsigned _TF9;struct Cyc_List_List*_TFA;void*_TFB;struct Cyc_Absyn_Aggrfield*_TFC;struct _fat_ptr*_TFD;int(*_TFE)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_TFF)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T100;struct Cyc_Tcpat_Con_s*_T101;struct Cyc_Tcpat_Con_s*_T102;int _T103;struct _fat_ptr _T104;struct Cyc_String_pa_PrintArg_struct _T105;struct Cyc_String_pa_PrintArg_struct _T106;struct _fat_ptr _T107;struct _fat_ptr _T108;struct Cyc_List_List*_T109;struct Cyc_Tcpat_Desc2string_exn_struct*_T10A;struct Cyc_Tcpat_Desc2string_exn_struct*_T10B;struct Cyc_Absyn_PtrInfo _T10C;struct Cyc_Absyn_PtrAtts _T10D;int _T10E;int(*_T10F)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_T110)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T111;struct Cyc_Tcpat_Con_s*_T112;int _T113;struct _fat_ptr _T114;struct _fat_ptr _T115;struct Cyc_Tcpat_Desc2string_exn_struct*_T116;struct Cyc_Tcpat_Desc2string_exn_struct*_T117;struct Cyc_Set_Set*_T118;struct Cyc_List_List*_T119;struct Cyc_Tcpat_Con_s*_T11A;_T0=d;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL219;_T3=d;{struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T11B=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_T3;_T11A=_T11B->f1;_T119=_T11B->f2;}{struct Cyc_Tcpat_Con_s*c=_T11A;struct Cyc_List_List*ds=_T119;_T4=c;{
# 1556
union Cyc_Tcpat_Name_value n=_T4->name;
struct Cyc_Absyn_Pat*p;_T5=c;{
union Cyc_Tcpat_PatOrWhere _T11B=_T5->orig_pat;struct Cyc_Absyn_Pat*_T11C;_T6=_T11B.where_clause;_T7=_T6.tag;if(_T7!=2)goto _TL21B;_T8=
_check_null(ds);_T9=_T8->hd;_TA=Cyc_Tcpat_desc2string(_T9);return _TA;_TL21B: _TB=_T11B.pattern;_T11C=_TB.val;{struct Cyc_Absyn_Pat*p2=_T11C;
p=p2;goto _LL5;}_LL5:;}_TC=p;{
# 1562
void*_T11B=_TC->r;struct Cyc_Absyn_Exp*_T11C;struct Cyc_Absyn_Enumfield*_T11D;struct _fat_ptr _T11E;char _T11F;int _T120;struct Cyc_Absyn_Datatypefield*_T121;struct Cyc_List_List*_T122;struct Cyc_Absyn_Vardecl*_T123;void*_T124;_TD=(int*)_T11B;_TE=*_TD;switch(_TE){case 0: _TF=
_tag_fat("_",sizeof(char),2U);return _TF;case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T11B;_T124=_T125->f1;}{struct Cyc_Absyn_Vardecl*vd=_T124;_T10=vd;_T11=_T10->name;_T12=
Cyc_Absynpp_qvar2string(_T11);return _T12;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T11B;_T124=_T125->f1;}{struct Cyc_Absyn_Vardecl*vd=_T124;{struct Cyc_String_pa_PrintArg_struct _T125;_T125.tag=0;_T15=vd;_T16=_T15->name;
_T125.f1=Cyc_Absynpp_qvar2string(_T16);_T14=_T125;}{struct Cyc_String_pa_PrintArg_struct _T125=_T14;void*_T126[1];_T126[0]=& _T125;_T17=_tag_fat("*%s",sizeof(char),4U);_T18=_tag_fat(_T126,sizeof(void*),1);_T13=Cyc_aprintf(_T17,_T18);}return _T13;}case 4:{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T11B;_T124=_T125->f1;_T123=_T125->f2;}{struct Cyc_Absyn_Tvar*tv=_T124;struct Cyc_Absyn_Vardecl*vd=_T123;{struct Cyc_String_pa_PrintArg_struct _T125;_T125.tag=0;_T1B=vd;_T1C=_T1B->name;
_T125.f1=Cyc_Absynpp_qvar2string(_T1C);_T1A=_T125;}{struct Cyc_String_pa_PrintArg_struct _T125=_T1A;{struct Cyc_String_pa_PrintArg_struct _T126;_T126.tag=0;_T1E=tv;_T1F=_T1E->name;
_T126.f1=*_T1F;_T1D=_T126;}{struct Cyc_String_pa_PrintArg_struct _T126=_T1D;void*_T127[2];_T127[0]=& _T125;_T127[1]=& _T126;_T20=
# 1566
_tag_fat("%s<`%s>",sizeof(char),8U);_T21=_tag_fat(_T127,sizeof(void*),2);_T19=Cyc_aprintf(_T20,_T21);}}return _T19;}case 5:{struct Cyc_String_pa_PrintArg_struct _T125;_T125.tag=0;
# 1568
_T125.f1=Cyc_Tcpat_descs2string(ds);_T23=_T125;}{struct Cyc_String_pa_PrintArg_struct _T125=_T23;void*_T126[1];_T126[0]=& _T125;_T24=_tag_fat("&%s",sizeof(char),4U);_T25=_tag_fat(_T126,sizeof(void*),1);_T22=Cyc_aprintf(_T24,_T25);}return _T22;case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T11B;_T26=_T125->f1;_T124=(void*)_T26;_T122=_T125->f4;}{void*atype=_T124;struct Cyc_List_List*dlps=_T122;_T27=
# 1570
_check_null(atype);{void*_T125=Cyc_Absyn_compress(_T27);int _T126;union Cyc_Absyn_AggrInfo _T127;_T28=(int*)_T125;_T29=*_T28;switch(_T29){case 0: _T2A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T125;_T2B=_T2A->f1;_T2C=(int*)_T2B;_T2D=*_T2C;if(_T2D!=24)goto _TL21F;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T128=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T125;_T2E=_T128->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T129=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T2E;_T127=_T129->f1;}}{union Cyc_Absyn_AggrInfo ainfo=_T127;
# 1572
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);_T2F=ad;_T30=_T2F->kind;_T31=(int)_T30;
if(_T31!=1)goto _TL221;{struct _fat_ptr*_T128;if(dlps==0)goto _TL223;_T32=dlps;_T33=(struct Cyc_List_List*)_T32;_T34=_T33->hd;_T35=(struct _tuple16*)_T34;_T36=_T35->f0;if(_T36==0)goto _TL225;_T37=dlps;_T38=(struct Cyc_List_List*)_T37;_T39=_T38->hd;_T3A=(struct _tuple16*)_T39;_T3B=_T3A->f0;_T3C=(struct Cyc_List_List*)_T3B;_T3D=_T3C->hd;_T3E=(int*)_T3D;_T3F=*_T3E;if(_T3F!=1)goto _TL227;_T40=dlps;{struct Cyc_List_List _T129=*_T40;_T41=_T129.hd;_T42=(struct _tuple16*)_T41;{struct _tuple16 _T12A=*_T42;_T43=_T12A.f0;{struct Cyc_List_List _T12B=*_T43;_T44=_T12B.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T12C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T44;_T128=_T12C->f1;}}}}{struct _fat_ptr*f=_T128;{struct Cyc_String_pa_PrintArg_struct _T129;_T129.tag=0;_T47=ad;_T48=_T47->name;
# 1576
_T129.f1=Cyc_Absynpp_qvar2string(_T48);_T46=_T129;}{struct Cyc_String_pa_PrintArg_struct _T129=_T46;{struct Cyc_String_pa_PrintArg_struct _T12A;_T12A.tag=0;_T4A=f;_T12A.f1=*_T4A;_T49=_T12A;}{struct Cyc_String_pa_PrintArg_struct _T12A=_T49;{struct Cyc_String_pa_PrintArg_struct _T12B;_T12B.tag=0;
_T12B.f1=Cyc_Tcpat_descs2string(ds);_T4B=_T12B;}{struct Cyc_String_pa_PrintArg_struct _T12B=_T4B;void*_T12C[3];_T12C[0]=& _T129;_T12C[1]=& _T12A;_T12C[2]=& _T12B;_T4C=
# 1576
_tag_fat("%s{.%s = %s}",sizeof(char),13U);_T4D=_tag_fat(_T12C,sizeof(void*),3);_T45=Cyc_aprintf(_T4C,_T4D);}}}return _T45;}_TL227: goto _LL33;_TL225: goto _LL33;_TL223: _LL33: goto _LL30;_LL30:;}goto _TL222;_TL221: _TL222:{struct Cyc_String_pa_PrintArg_struct _T128;_T128.tag=0;_T50=ad;_T51=_T50->name;
# 1580
_T128.f1=Cyc_Absynpp_qvar2string(_T51);_T4F=_T128;}{struct Cyc_String_pa_PrintArg_struct _T128=_T4F;{struct Cyc_String_pa_PrintArg_struct _T129;_T129.tag=0;_T129.f1=Cyc_Tcpat_descs2string(ds);_T52=_T129;}{struct Cyc_String_pa_PrintArg_struct _T129=_T52;void*_T12A[2];_T12A[0]=& _T128;_T12A[1]=& _T129;_T53=_tag_fat("%s{%s}",sizeof(char),7U);_T54=_tag_fat(_T12A,sizeof(void*),2);_T4E=Cyc_aprintf(_T53,_T54);}}return _T4E;}_TL21F: goto _LL2E;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T128=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T125;_T126=_T128->f2;}{int is_tuple=_T126;_T55=is_tuple;
# 1582
if(!_T55)goto _TL229;{struct Cyc_String_pa_PrintArg_struct _T128;_T128.tag=0;_T128.f1=Cyc_Tcpat_descs2string(ds);_T57=_T128;}{struct Cyc_String_pa_PrintArg_struct _T128=_T57;void*_T129[1];_T129[0]=& _T128;_T58=_tag_fat("$(%s)",sizeof(char),6U);_T59=_tag_fat(_T129,sizeof(void*),1);_T56=Cyc_aprintf(_T58,_T59);}return _T56;
_TL229:{struct Cyc_String_pa_PrintArg_struct _T128;_T128.tag=0;_T128.f1=Cyc_Tcpat_descs2string(ds);_T5B=_T128;}{struct Cyc_String_pa_PrintArg_struct _T128=_T5B;void*_T129[1];_T129[0]=& _T128;_T5C=_tag_fat("{%s}",sizeof(char),5U);_T5D=_tag_fat(_T129,sizeof(void*),1);_T5A=Cyc_aprintf(_T5C,_T5D);}return _T5A;}default: _LL2E:{struct Cyc_Warn_String_Warn_Warg_struct _T128;_T128.tag=0;
_T128.f1=_tag_fat("bad type in aggr_p",sizeof(char),19U);_T5E=_T128;}{struct Cyc_Warn_String_Warn_Warg_struct _T128=_T5E;void*_T129[1];_T129[0]=& _T128;_T60=Cyc_Warn_impos2;{int(*_T12A)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T60;_T5F=_T12A;}_T61=_tag_fat(_T129,sizeof(void*),1);_T5F(_T61);}};}}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T11B;_T121=_T125->f2;}{struct Cyc_Absyn_Datatypefield*tuf=_T121;
# 1587
if(ds!=0)goto _TL22B;{struct Cyc_String_pa_PrintArg_struct _T125;_T125.tag=0;_T64=tuf;_T65=_T64->name;
_T125.f1=Cyc_Absynpp_qvar2string(_T65);_T63=_T125;}{struct Cyc_String_pa_PrintArg_struct _T125=_T63;void*_T126[1];_T126[0]=& _T125;_T66=_tag_fat("%s",sizeof(char),3U);_T67=_tag_fat(_T126,sizeof(void*),1);_T62=Cyc_aprintf(_T66,_T67);}return _T62;
# 1590
_TL22B:{struct Cyc_String_pa_PrintArg_struct _T125;_T125.tag=0;_T6A=tuf;_T6B=_T6A->name;_T125.f1=Cyc_Absynpp_qvar2string(_T6B);_T69=_T125;}{struct Cyc_String_pa_PrintArg_struct _T125=_T69;{struct Cyc_String_pa_PrintArg_struct _T126;_T126.tag=0;_T126.f1=Cyc_Tcpat_descs2string(ds);_T6C=_T126;}{struct Cyc_String_pa_PrintArg_struct _T126=_T6C;void*_T127[2];_T127[0]=& _T125;_T127[1]=& _T126;_T6D=_tag_fat("%s(%s)",sizeof(char),7U);_T6E=_tag_fat(_T127,sizeof(void*),2);_T68=Cyc_aprintf(_T6D,_T6E);}}return _T68;}case 8: _T6F=
_tag_fat("NULL",sizeof(char),5U);return _T6F;case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T11B;_T120=_T125->f2;}{int i=_T120;{struct Cyc_Int_pa_PrintArg_struct _T125;_T125.tag=1;_T72=i;
_T125.f1=(unsigned long)_T72;_T71=_T125;}{struct Cyc_Int_pa_PrintArg_struct _T125=_T71;void*_T126[1];_T126[0]=& _T125;_T73=_tag_fat("%d",sizeof(char),3U);_T74=_tag_fat(_T126,sizeof(void*),1);_T70=Cyc_aprintf(_T73,_T74);}return _T70;}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T11B;_T11F=_T125->f1;}{char c=_T11F;{struct Cyc_Int_pa_PrintArg_struct _T125;_T125.tag=1;_T77=c;_T78=(int)_T77;
_T125.f1=(unsigned long)_T78;_T76=_T125;}{struct Cyc_Int_pa_PrintArg_struct _T125=_T76;void*_T126[1];_T126[0]=& _T125;_T79=_tag_fat("%d",sizeof(char),3U);_T7A=_tag_fat(_T126,sizeof(void*),1);_T75=Cyc_aprintf(_T79,_T7A);}return _T75;}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T11B;_T11E=_T125->f1;_T120=_T125->f2;}{struct _fat_ptr f=_T11E;int i=_T120;_T7B=f;
return _T7B;}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T11B;_T11D=_T125->f2;}{struct Cyc_Absyn_Enumfield*ef=_T11D;_T11D=ef;goto _LL24;}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T11B;_T11D=_T125->f2;}_LL24: {struct Cyc_Absyn_Enumfield*ef=_T11D;_T7C=ef;_T7D=_T7C->name;_T7E=
# 1596
Cyc_Absynpp_qvar2string(_T7D);return _T7E;}case 16:{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T125=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T11B;_T11C=_T125->f1;}{struct Cyc_Absyn_Exp*e=_T11C;_T7F=
Cyc_Absynpp_exp2string(e);return _T7F;}default: _T80=& Cyc_Tcpat_Desc2string_val;_T81=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T80;_throw(_T81);};}}}goto _TL21A;_TL219: _T82=d;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_T11B=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_T82;_T118=_T11B->f1;}{struct Cyc_Set_Set*s=_T118;_T83=
# 1601
Cyc_Set_is_empty(s);if(!_T83)goto _TL22D;_T84=_tag_fat("_",sizeof(char),2U);return _T84;_TL22D: _T86=Cyc_Set_choose;{
struct Cyc_Tcpat_Con_s*(*_T11B)(struct Cyc_Set_Set*)=(struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*))_T86;_T85=_T11B;}{struct Cyc_Tcpat_Con_s*c=_T85(s);_T87=c;_T88=_T87->orig_pat;_T89=_T88.where_clause;_T8A=_T89.tag;_T8B=_T8A==2;_T8C=(int)_T8B;
# 1605
if(!_T8C)goto _TL22F;_T8D=& Cyc_Tcpat_Desc2string_val;_T8E=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T8D;_throw(_T8E);goto _TL230;_TL22F: _TL230: _T90=c;{union Cyc_Tcpat_PatOrWhere _T11B=_T90->orig_pat;_T91=_T11B.pattern;_T92=_T91.tag;if(_T92==1)goto _TL231;_throw_match();goto _TL232;_TL231: _TL232: _T93=_T11B.pattern;_T8F=_T93.val;}{
struct Cyc_Absyn_Pat*orig_pat=_T8F;_T94=orig_pat;_T95=_T94->topt;_T96=
_check_null(_T95);{void*_T11B=Cyc_Absyn_compress(_T96);union Cyc_Absyn_AggrInfo _T11C;void*_T11D;_T97=(int*)_T11B;_T98=*_T97;switch(_T98){case 0: _T99=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11B;_T9A=_T99->f1;_T9B=(int*)_T9A;_T9C=*_T9B;switch(_T9C){case 1: _T9D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11B;_T9E=_T9D->f1;_T9F=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T9E;_TA0=_T9F->f2;if(_TA0!=Cyc_Absyn_Char_sz)goto _TL235;{
# 1610
int i=0;_TL23A: if(i < 256)goto _TL238;else{goto _TL239;}
_TL238: _TA2=Cyc_Set_member;{int(*_T11E)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_TA2;_TA1=_T11E;}_TA3=s;_TA4=i;_TA5=(char)_TA4;_TA6=orig_pat;_TA7=Cyc_Tcpat_char_con(_TA5,_TA6);_TA8=_TA1(_TA3,_TA7);if(_TA8)goto _TL23B;else{goto _TL23D;}
_TL23D:{struct Cyc_Int_pa_PrintArg_struct _T11E;_T11E.tag=1;_TAB=i;_T11E.f1=(unsigned long)_TAB;_TAA=_T11E;}{struct Cyc_Int_pa_PrintArg_struct _T11E=_TAA;void*_T11F[1];_T11F[0]=& _T11E;_TAC=_tag_fat("%d",sizeof(char),3U);_TAD=_tag_fat(_T11F,sizeof(void*),1);_TA9=Cyc_aprintf(_TAC,_TAD);}return _TA9;_TL23B:
# 1610
 i=i + 1;goto _TL23A;_TL239:;}_TAE=& Cyc_Tcpat_Desc2string_val;_TAF=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TAE;_throw(_TAF);goto _TL236;_TL235:{
# 1616
unsigned i=0U;_TL241: if(i < 4294967295U)goto _TL23F;else{goto _TL240;}
_TL23F: _TB1=Cyc_Set_member;{int(*_T11E)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_TB1;_TB0=_T11E;}_TB2=s;_TB3=i;_TB4=(int)_TB3;_TB5=c;_TB6=_TB5->orig_pat;_TB7=Cyc_Tcpat_int_con(_TB4,_TB6);_TB8=_TB0(_TB2,_TB7);if(_TB8)goto _TL242;else{goto _TL244;}
_TL244:{struct Cyc_Int_pa_PrintArg_struct _T11E;_T11E.tag=1;_TBB=i;_TBC=(int)_TBB;_T11E.f1=(unsigned long)_TBC;_TBA=_T11E;}{struct Cyc_Int_pa_PrintArg_struct _T11E=_TBA;void*_T11F[1];_T11F[0]=& _T11E;_TBD=_tag_fat("%d",sizeof(char),3U);_TBE=_tag_fat(_T11F,sizeof(void*),1);_TB9=Cyc_aprintf(_TBD,_TBE);}return _TB9;_TL242:
# 1616
 i=i + 1;goto _TL241;_TL240:;}_TBF=& Cyc_Tcpat_Desc2string_val;_TC0=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TBF;_throw(_TC0);_TL236:;case 22: _TC1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11B;_TC2=_TC1->f1;_TC3=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC2;_TC4=_TC3->f1;_TC5=_TC4.KnownDatatype;_TC6=_TC5.tag;if(_TC6!=2)goto _TL245;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11B;_TC7=_T11E->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T11F=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC7;_TC8=_T11F->f1;_TC9=_TC8.KnownDatatype;_TCA=_TC9.val;{struct Cyc_Absyn_Datatypedecl*_T120=*_TCA;_T11D=_T120;}}}{struct Cyc_Absyn_Datatypedecl*tud=_T11D;_TCB=tud;_TCC=_TCB->is_extensible;
# 1625
if(!_TCC)goto _TL247;_TCD=& Cyc_Tcpat_Desc2string_val;_TCE=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TCD;_throw(_TCE);goto _TL248;_TL247: _TL248: _TCF=tud;_TD0=_TCF->fields;_TD1=
_check_null(_TD0);_TD2=_TD1->v;{struct Cyc_List_List*fields=(struct Cyc_List_List*)_TD2;
int span=Cyc_List_length(fields);
_TL24C: _TD3=fields;_TD4=(unsigned)_TD3;if(_TD4)goto _TL24A;else{goto _TL24B;}
_TL24A: _TD5=fields;_TD6=_TD5->hd;_TD7=(struct Cyc_Absyn_Datatypefield*)_TD6;_TD8=_TD7->name;_TD9=*_TD8;_TDA=_TD9.f1;{struct _fat_ptr n=*_TDA;_TDB=fields;_TDC=_TDB->hd;_TDD=(struct Cyc_Absyn_Datatypefield*)_TDC;{
struct Cyc_List_List*args=_TDD->typs;_TDF=Cyc_Set_member;{
int(*_T11E)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_TDF;_TDE=_T11E;}_TE0=s;{struct Cyc_Tcpat_Con_s*_T11E=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T11E->name=Cyc_Tcpat_Name_v(n);_T11E->arity=0;_T11E->span=0;_TE2=c;_T11E->orig_pat=_TE2->orig_pat;_TE1=(struct Cyc_Tcpat_Con_s*)_T11E;}_TE3=_TDE(_TE0,_TE1);if(_TE3)goto _TL24D;else{goto _TL24F;}
_TL24F: _TE5=Cyc_List_length(args);if(_TE5!=0)goto _TL250;_TE4=n;goto _TL251;_TL250:{struct Cyc_String_pa_PrintArg_struct _T11E;_T11E.tag=0;_T11E.f1=n;_TE7=_T11E;}{struct Cyc_String_pa_PrintArg_struct _T11E=_TE7;void*_T11F[1];_T11F[0]=& _T11E;_TE8=_tag_fat("%s(...)",sizeof(char),8U);_TE9=_tag_fat(_T11F,sizeof(void*),1);_TE6=Cyc_aprintf(_TE8,_TE9);}_TE4=_TE6;_TL251: return _TE4;_TL24D:;}}_TEA=fields;
# 1628
fields=_TEA->tl;goto _TL24C;_TL24B: _TEB=& Cyc_Tcpat_Desc2string_val;_TEC=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TEB;_throw(_TEC);}}goto _TL246;_TL245: goto _LL40;_TL246:;case 24:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11B;_TED=_T11E->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T11F=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TED;_T11C=_T11F->f1;}}{union Cyc_Absyn_AggrInfo ainfo=_T11C;
# 1636
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);_TEE=ad;_TEF=_TEE->kind;_TF0=(int)_TEF;
if(_TF0==1)goto _TL252;_TF1=& Cyc_Tcpat_Desc2string_val;_TF2=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TF1;_throw(_TF2);goto _TL253;_TL252: _TL253: _TF3=ad;_TF4=_TF3->impl;_TF5=
_check_null(_TF4);{struct Cyc_List_List*fields=_TF5->fields;
int span=Cyc_List_length(fields);_TF6=ad;{
struct _tuple2*_T11E=_TF6->name;struct _fat_ptr _T11F;{struct _tuple2 _T120=*_T11E;_TF7=_T120.f1;{struct _fat_ptr _T121=*_TF7;_T11F=_T121;}}{struct _fat_ptr union_name=_T11F;
_TL257: _TF8=fields;_TF9=(unsigned)_TF8;if(_TF9)goto _TL255;else{goto _TL256;}
_TL255: _TFA=fields;_TFB=_TFA->hd;_TFC=(struct Cyc_Absyn_Aggrfield*)_TFB;_TFD=_TFC->name;{struct _fat_ptr n=*_TFD;_TFF=Cyc_Set_member;{
int(*_T120)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_TFF;_TFE=_T120;}_T100=s;{struct Cyc_Tcpat_Con_s*_T120=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T120->name=Cyc_Tcpat_Name_v(n);_T120->arity=0;_T120->span=0;_T102=c;_T120->orig_pat=_T102->orig_pat;_T101=(struct Cyc_Tcpat_Con_s*)_T120;}_T103=_TFE(_T100,_T101);if(_T103)goto _TL258;else{goto _TL25A;}
_TL25A:{struct Cyc_String_pa_PrintArg_struct _T120;_T120.tag=0;_T120.f1=union_name;_T105=_T120;}{struct Cyc_String_pa_PrintArg_struct _T120=_T105;{struct Cyc_String_pa_PrintArg_struct _T121;_T121.tag=0;_T121.f1=n;_T106=_T121;}{struct Cyc_String_pa_PrintArg_struct _T121=_T106;void*_T122[2];_T122[0]=& _T120;_T122[1]=& _T121;_T107=_tag_fat("%s{.%s = _}",sizeof(char),12U);_T108=_tag_fat(_T122,sizeof(void*),2);_T104=Cyc_aprintf(_T107,_T108);}}return _T104;_TL258:;}_T109=fields;
# 1641
fields=_T109->tl;goto _TL257;_TL256: _T10A=& Cyc_Tcpat_Desc2string_val;_T10B=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T10A;_throw(_T10B);}}}}default: goto _LL40;};case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T11E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T11B;_T10C=_T11E->f1;_T10D=_T10C.ptr_atts;_T11D=_T10D.nullable;}{void*n=_T11D;_T10E=
# 1621
Cyc_Tcutil_force_type2bool(0,n);if(!_T10E)goto _TL25B;_T110=Cyc_Set_member;{int(*_T11E)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_T110;_T10F=_T11E;}_T111=s;_T112=Cyc_Tcpat_null_con(orig_pat);_T113=_T10F(_T111,_T112);if(_T113)goto _TL25B;else{goto _TL25D;}
_TL25D: _T114=_tag_fat("NULL",sizeof(char),5U);return _T114;_TL25B: _T115=
_tag_fat("&_",sizeof(char),3U);return _T115;}default: _LL40: _T116=& Cyc_Tcpat_Desc2string_val;_T117=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T116;_throw(_T117);};}}}}_TL21A:;}
# 1651
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){struct _fat_ptr*_T0;{struct _fat_ptr*_T1=_cycalloc(sizeof(struct _fat_ptr));
*_T1=Cyc_Tcpat_desc2string(d);_T0=(struct _fat_ptr*)_T1;}return _T0;}
# 1654
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){struct Cyc_List_List*(*_T0)(struct _fat_ptr*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T2;struct _fat_ptr*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct _fat_ptr _TB;_T1=Cyc_List_map;{
struct Cyc_List_List*(*_TC)(struct _fat_ptr*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(void*),struct Cyc_List_List*))_T1;_T0=_TC;}_T2=ds;{struct Cyc_List_List*ss=_T0(Cyc_Tcpat_desc2stringptr,_T2);
struct _fat_ptr*comma;comma=_cycalloc(sizeof(struct _fat_ptr));_T3=comma;*_T3=_tag_fat(",",sizeof(char),2U);{
struct Cyc_List_List*x=ss;_TL261: if(x!=0)goto _TL25F;else{goto _TL260;}
_TL25F: _T4=x;_T5=_T4->tl;if(_T5==0)goto _TL262;_T6=x;{struct Cyc_List_List*_TC=_cycalloc(sizeof(struct Cyc_List_List));
_TC->hd=comma;_T8=x;_TC->tl=_T8->tl;_T7=(struct Cyc_List_List*)_TC;}_T6->tl=_T7;_T9=x;
x=_T9->tl;goto _TL263;_TL262: _TL263: _TA=x;
# 1657
x=_TA->tl;goto _TL261;_TL260:;}_TB=
# 1662
Cyc_strconcat_l(ss);return _TB;}}
# 1665
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){struct _handler_cons*_T0;int*_T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;unsigned _T5;struct _fat_ptr _T6;void*_T7;struct Cyc_Tcpat_Desc2string_exn_struct*_T8;char*_T9;char*_TA;struct Cyc_Warn_String_Warn_Warg_struct _TB;unsigned _TC;struct _fat_ptr _TD;struct _handler_cons _TE;_T0=& _TE;_push_handler(_T0);{int _TF=0;_T1=_TE.handler;_T2=setjmp(_T1);if(!_T2)goto _TL264;_TF=1;goto _TL265;_TL264: _TL265: if(_TF)goto _TL266;else{goto _TL268;}_TL268:{
# 1667
struct _fat_ptr s=Cyc_Tcpat_desc2string(desc);{struct Cyc_Warn_String_Warn_Warg_struct _T10;_T10.tag=0;
_T10.f1=_tag_fat("patterns may not be exhaustive.\n\tmissing case for ",sizeof(char),51U);_T3=_T10;}{struct Cyc_Warn_String_Warn_Warg_struct _T10=_T3;{struct Cyc_Warn_String_Warn_Warg_struct _T11;_T11.tag=0;_T11.f1=s;_T4=_T11;}{struct Cyc_Warn_String_Warn_Warg_struct _T11=_T4;void*_T12[2];_T12[0]=& _T10;_T12[1]=& _T11;_T5=loc;_T6=_tag_fat(_T12,sizeof(void*),2);Cyc_Warn_err2(_T5,_T6);}}}_pop_handler();goto _TL267;_TL266: _T7=Cyc_Core_get_exn_thrown();{void*_T10=(void*)_T7;void*_T11;_T8=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T10;_T9=_T8->tag;_TA=Cyc_Tcpat_Desc2string;if(_T9!=_TA)goto _TL269;{struct Cyc_Warn_String_Warn_Warg_struct _T12;_T12.tag=0;
# 1670
_T12.f1=_tag_fat("patterns may not be exhaustive.",sizeof(char),32U);_TB=_T12;}{struct Cyc_Warn_String_Warn_Warg_struct _T12=_TB;void*_T13[1];_T13[0]=& _T12;_TC=loc;_TD=_tag_fat(_T13,sizeof(void*),1);Cyc_Warn_err2(_TC,_TD);}goto _LL0;_TL269: _T11=_T10;{void*exn=_T11;_rethrow(exn);}_LL0:;}_TL267:;}}
# 1674
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){struct Cyc_Tcpat_Rhs*_T0;_T0=rhs;
_T0->used=1;}
# 1678
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){struct Cyc_List_List*(*_T0)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*,void*),void*,struct Cyc_List_List*);int*_T2;struct Cyc_List_List*_T3;void**_T4;void(*_T5)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*);void*_T6;unsigned _T7;int*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_Tcpat_Rhs*_TB;int _TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;unsigned _TE;struct _fat_ptr _TF;struct Cyc_List_List*_T10;
# 1684
int where_ctr=0;
int*env=& where_ctr;_T1=Cyc_List_map_c;{
struct Cyc_List_List*(*_T11)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*))_T1;_T0=_T11;}_T2=env;_T3=swcs;{struct Cyc_List_List*match_rules=_T0(Cyc_Tcpat_get_match,_T2,_T3);
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);_T4=dopt;
*_T4=dec_tree;{
# 1690
int ex_done=0;{
void(*_T11)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*)=(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;_T5=_T11;}_T6=dec_tree;_T7=loc;_T8=& ex_done;_T5(_T6,Cyc_Tcpat_not_exhaust_err,_T7,Cyc_Tcpat_rule_occurs,0,_T8);
# 1693
_TL26E: if(match_rules!=0)goto _TL26C;else{goto _TL26D;}
_TL26C: _T9=match_rules;_TA=_T9->hd;{struct _tuple23*_T11=(struct _tuple23*)_TA;unsigned _T12;int _T13;{struct _tuple23 _T14=*_T11;_TB=_T14.f1;{struct Cyc_Tcpat_Rhs _T15=*_TB;_T13=_T15.used;_T12=_T15.pat_loc;}}{int b=_T13;unsigned loc2=_T12;_TC=b;
if(_TC)goto _TL26F;else{goto _TL271;}
_TL271:{struct Cyc_Warn_String_Warn_Warg_struct _T14;_T14.tag=0;_T14.f1=_tag_fat("redundant pattern (check for misspelled constructors in earlier patterns)",sizeof(char),74U);_TD=_T14;}{struct Cyc_Warn_String_Warn_Warg_struct _T14=_TD;void*_T15[1];_T15[0]=& _T14;_TE=loc2;_TF=_tag_fat(_T15,sizeof(void*),1);Cyc_Warn_err2(_TE,_TF);}goto _TL26D;_TL26F:;}}_T10=match_rules;
# 1693
match_rules=_T10->tl;goto _TL26E;_TL26D:;}}}
# 1707
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple15*pr,void*desc){struct _tuple15*_T0;struct _handler_cons*_T1;int*_T2;int _T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;struct _tuple15*_T6;struct _tuple15 _T7;unsigned _T8;struct _fat_ptr _T9;void*_TA;struct Cyc_Tcpat_Desc2string_exn_struct*_TB;char*_TC;char*_TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct _tuple15*_TF;struct _tuple15 _T10;unsigned _T11;struct _fat_ptr _T12;_T0=pr;
(*_T0).f1=0;{struct _handler_cons _T13;_T1=& _T13;_push_handler(_T1);{int _T14=0;_T2=_T13.handler;_T3=setjmp(_T2);if(!_T3)goto _TL272;_T14=1;goto _TL273;_TL272: _TL273: if(_T14)goto _TL274;else{goto _TL276;}_TL276:{
# 1710
struct _fat_ptr s=Cyc_Tcpat_desc2string(desc);{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;
_T15.f1=_tag_fat("pattern not exhaustive.\n\tmissing case for ",sizeof(char),43U);_T4=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_T4;{struct Cyc_Warn_String_Warn_Warg_struct _T16;_T16.tag=0;_T16.f1=s;_T5=_T16;}{struct Cyc_Warn_String_Warn_Warg_struct _T16=_T5;void*_T17[2];_T17[0]=& _T15;_T17[1]=& _T16;_T6=pr;_T7=*_T6;_T8=_T7.f0;_T9=_tag_fat(_T17,sizeof(void*),2);Cyc_Warn_warn2(_T8,_T9);}}}_pop_handler();goto _TL275;_TL274: _TA=Cyc_Core_get_exn_thrown();{void*_T15=(void*)_TA;void*_T16;_TB=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T15;_TC=_TB->tag;_TD=Cyc_Tcpat_Desc2string;if(_TC!=_TD)goto _TL277;{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
# 1713
_T17.f1=_tag_fat("pattern not exhaustive.",sizeof(char),24U);_TE=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TE;void*_T18[1];_T18[0]=& _T17;_TF=pr;_T10=*_TF;_T11=_T10.f0;_T12=_tag_fat(_T18,sizeof(void*),1);Cyc_Warn_warn2(_T11,_T12);}goto _LL0;_TL277: _T16=_T15;{void*exn=_T16;_rethrow(exn);}_LL0:;}_TL275:;}}}
# 1717
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1721
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){struct Cyc_Tcpat_Rhs*_T0;struct Cyc_Tcpat_Rhs*_T1;struct Cyc_Absyn_Pat*_T2;struct Cyc_Tcpat_Rhs*_T3;struct Cyc_List_List*_T4;struct _tuple23*_T5;struct Cyc_List_List*_T6;struct _tuple15 _T7;void(*_T8)(void*,void(*)(struct _tuple15*,void*),struct _tuple15*,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*);void*_T9;struct _tuple15*_TA;int*_TB;void**_TC;struct _tuple15 _TD;int _TE;
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs));_T0=rhs;_T0->used=0;_T1=rhs;_T2=p;_T1->pat_loc=_T2->loc;_T3=rhs;_T3->rhs=Cyc_Absyn_skip_stmt(0U);{
struct Cyc_List_List*match_rules;match_rules=_cycalloc(sizeof(struct Cyc_List_List));_T4=match_rules;{struct _tuple23*_TF=_cycalloc(sizeof(struct _tuple23));_TF->f0=Cyc_Tcpat_compile_pat(p);_TF->f1=rhs;_T5=(struct _tuple23*)_TF;}_T4->hd=_T5;_T6=match_rules;_T6->tl=0;{
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);{struct _tuple15 _TF;
_TF.f0=loc;_TF.f1=1;_T7=_TF;}{struct _tuple15 exhaust_env=_T7;
int ex_done=0;{
void(*_TF)(void*,void(*)(struct _tuple15*,void*),struct _tuple15*,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*)=(void(*)(void*,void(*)(struct _tuple15*,void*),struct _tuple15*,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;_T8=_TF;}_T9=dec_tree;_TA=& exhaust_env;_TB=& ex_done;_T8(_T9,Cyc_Tcpat_not_exhaust_warn,_TA,Cyc_Tcpat_dummy_fn,0,_TB);_TC=dopt;
# 1729
*_TC=dec_tree;_TD=exhaust_env;_TE=_TD.f1;
return _TE;}}}}
# 1737
static struct _tuple23*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){struct Cyc_Absyn_Switch_clause*_T0;struct Cyc_Absyn_Pat*_T1;struct Cyc_Absyn_Switch_clause*_T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;struct Cyc_Absyn_Switch_clause*_T5;struct Cyc_Absyn_Exp*_T6;unsigned _T7;struct _fat_ptr _T8;struct Cyc_Tcpat_Rhs*_T9;struct Cyc_Tcpat_Rhs*_TA;struct Cyc_Absyn_Switch_clause*_TB;struct Cyc_Absyn_Pat*_TC;struct Cyc_Tcpat_Rhs*_TD;struct Cyc_Absyn_Switch_clause*_TE;struct _tuple23*_TF;_T0=swc;_T1=_T0->pattern;{
void*sp0=Cyc_Tcpat_compile_pat(_T1);_T2=swc;_T3=_T2->where_clause;
# 1741
if(_T3==0)goto _TL279;{struct Cyc_Warn_String_Warn_Warg_struct _T10;_T10.tag=0;
_T10.f1=_tag_fat("&&-clauses not supported in exception handlers.",sizeof(char),48U);_T4=_T10;}{struct Cyc_Warn_String_Warn_Warg_struct _T10=_T4;void*_T11[1];_T11[0]=& _T10;_T5=swc;_T6=_T5->where_clause;_T7=_T6->loc;_T8=_tag_fat(_T11,sizeof(void*),1);Cyc_Warn_err2(_T7,_T8);}goto _TL27A;_TL279: _TL27A: {
# 1744
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs));_T9=rhs;_T9->used=0;_TA=rhs;_TB=swc;_TC=_TB->pattern;_TA->pat_loc=_TC->loc;_TD=rhs;_TE=swc;_TD->rhs=_TE->body;{struct _tuple23*_T10=_cycalloc(sizeof(struct _tuple23));
_T10->f0=sp0;_T10->f1=rhs;_TF=(struct _tuple23*)_T10;}return _TF;}}}
# 1747
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1750
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){struct Cyc_List_List*(*_T0)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T2;void**_T3;void(*_T4)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*);void*_T5;unsigned _T6;int*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Tcpat_Rhs*_TC;int _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;unsigned _TF;struct _fat_ptr _T10;struct Cyc_List_List*_T11;_T1=Cyc_List_map;{
# 1754
struct Cyc_List_List*(*_T12)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))_T1;_T0=_T12;}_T2=swcs;{struct Cyc_List_List*match_rules=_T0(Cyc_Tcpat_get_match2,_T2);
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);_T3=dopt;
*_T3=dec_tree;{
int ex_done=0;{
void(*_T12)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*)=(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;_T4=_T12;}_T5=dec_tree;_T6=loc;_T7=& ex_done;_T4(_T5,Cyc_Tcpat_not_exhaust_err2,_T6,Cyc_Tcpat_rule_occurs,0,_T7);
# 1760
_TL27E: if(match_rules!=0)goto _TL27C;else{goto _TL27D;}
# 1762
_TL27C: _T8=match_rules;_T9=_T8->tl;if(_T9!=0)goto _TL27F;goto _TL27D;_TL27F: _TA=match_rules;_TB=_TA->hd;{
struct _tuple23*_T12=(struct _tuple23*)_TB;unsigned _T13;int _T14;{struct _tuple23 _T15=*_T12;_TC=_T15.f1;{struct Cyc_Tcpat_Rhs _T16=*_TC;_T14=_T16.used;_T13=_T16.pat_loc;}}{int b=_T14;unsigned loc2=_T13;_TD=b;
if(_TD)goto _TL281;else{goto _TL283;}
_TL283:{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;_T15.f1=_tag_fat("redundant pattern (check for misspelled constructors in earlier patterns)",sizeof(char),74U);_TE=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_TE;void*_T16[1];_T16[0]=& _T15;_TF=loc2;_T10=_tag_fat(_T16,sizeof(void*),1);Cyc_Warn_err2(_TF,_T10);}goto _TL27D;_TL281:;}}_T11=match_rules;
# 1760
match_rules=_T11->tl;goto _TL27E;_TL27D:;}}}
# 1771
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){struct Cyc_Core_Opt*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple17*_T4;struct _tuple17 _T5;struct Cyc_Absyn_Vardecl**_T6;struct Cyc_List_List*_T7;_T0=
_check_null(pat_vars);_T1=_T0->v;{struct Cyc_List_List*l=(struct Cyc_List_List*)_T1;_TL287: if(l!=0)goto _TL285;else{goto _TL286;}
_TL285: _T2=l;_T3=_T2->hd;_T4=(struct _tuple17*)_T3;_T5=*_T4;_T6=_T5.f0;if(_T6==0)goto _TL288;
return 1;_TL288: _T7=l;
# 1772
l=_T7->tl;goto _TL287;_TL286:;}
# 1775
return 0;}

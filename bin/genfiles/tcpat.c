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
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T3;void*_T4;int _T5;int _T6;int _T7;int _T8;unsigned _T9;unsigned _TA;struct Cyc_Absyn_Aggrfield*_TB;struct Cyc_Absyn_Aggrfield*_TC;struct Cyc_Absyn_Aggrfield*_TD;struct Cyc_Core_Opt*_TE;struct Cyc_Core_Opt*_TF;struct Cyc_Core_Opt*_T10;struct Cyc_Absyn_Aggrfield*_T11;struct Cyc_Absyn_Aggrfield*_T12;struct Cyc_Absyn_Aggrfield*_T13;struct Cyc_List_List*_T14;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T15;struct Cyc_Absyn_Pat*_T16;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T17;struct Cyc_Warn_String_Warn_Warg_struct _T18;void**_T19;struct Cyc_Absyn_Pat*_T1A;unsigned _T1B;struct _fat_ptr _T1C;void**_T1D;void*_T1E;void*_T1F;int*_T20;unsigned _T21;void*_T22;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T23;void*_T24;int*_T25;int _T26;struct Cyc_Absyn_Pat*_T27;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T28;struct Cyc_Absyn_Pat*_T29;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T2A;struct Cyc_Warn_String_Warn_Warg_struct _T2B;struct Cyc_Warn_Typ_Warn_Warg_struct _T2C;void**_T2D;void**_T2E;void**_T2F;struct Cyc_Absyn_Pat*_T30;unsigned _T31;struct _fat_ptr _T32;void*_T33;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T34;struct Cyc_List_List**_T35;int*_T36;unsigned _T37;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T38;void*_T39;int*_T3A;int _T3B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C;void*_T3D;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T3E;union Cyc_Absyn_AggrInfo _T3F;struct _union_AggrInfo_KnownAggr _T40;unsigned _T41;void*_T42;union Cyc_Absyn_AggrInfo _T43;struct _union_AggrInfo_KnownAggr _T44;struct Cyc_Absyn_Aggrdecl**_T45;struct Cyc_Absyn_Aggrdecl*_T46;struct Cyc_Absyn_AggrdeclImpl*_T47;struct Cyc_Warn_String_Warn_Warg_struct _T48;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T49;void**_T4A;void**_T4B;struct Cyc_Absyn_Pat*_T4C;unsigned _T4D;struct _fat_ptr _T4E;struct Cyc_Absyn_Pat*_T4F;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T50;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T51;struct Cyc_Absyn_Aggrdecl*_T52;struct Cyc_Absyn_AggrdeclImpl*_T53;struct Cyc_List_List*_T54;int _T55;struct Cyc_List_List**_T56;struct Cyc_List_List*_T57;int _T58;struct Cyc_Warn_String_Warn_Warg_struct _T59;void**_T5A;struct Cyc_Absyn_Pat*_T5B;unsigned _T5C;struct _fat_ptr _T5D;struct Cyc_Absyn_Aggrdecl*_T5E;struct Cyc_Absyn_AggrdeclImpl*_T5F;struct Cyc_Absyn_AggrdeclImpl*_T60;struct Cyc_List_List*_T61;struct Cyc_List_List**_T62;struct Cyc_List_List*_T63;struct Cyc_List_List*_T64;struct Cyc_List_List**_T65;struct Cyc_List_List*_T66;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T67;void*_T68;struct Cyc_List_List**_T69;struct Cyc_List_List**_T6A;struct Cyc_List_List*_T6B;struct Cyc_List_List*_T6C;struct Cyc_Warn_String_Warn_Warg_struct _T6D;struct Cyc_Warn_Typ_Warn_Warg_struct _T6E;void**_T6F;void**_T70;int(*_T71)(struct _fat_ptr);void*(*_T72)(struct _fat_ptr);struct _fat_ptr _T73;int _T74;struct Cyc_Warn_String_Warn_Warg_struct _T75;void**_T76;struct Cyc_Absyn_Pat*_T77;unsigned _T78;struct _fat_ptr _T79;struct Cyc_Absyn_Pat*_T7A;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T7B;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T7C;int _T7D;struct Cyc_String_pa_PrintArg_struct _T7E;void**_T7F;struct Cyc_Absyn_Pat*_T80;unsigned _T81;struct _fat_ptr _T82;struct _fat_ptr _T83;struct Cyc_Absyn_Pat*_T84;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T85;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T86;struct Cyc_Absyn_Pat*_T87;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T88;unsigned _T89;struct Cyc_Warn_String_Warn_Warg_struct _T8A;void**_T8B;int(*_T8C)(struct _fat_ptr);void*(*_T8D)(struct _fat_ptr);struct _fat_ptr _T8E;struct Cyc_Warn_String_Warn_Warg_struct _T8F;void**_T90;int(*_T91)(struct _fat_ptr);void*(*_T92)(struct _fat_ptr);struct _fat_ptr _T93;_T0=p;{
void*_T94=_T0->r;struct Cyc_Absyn_Exp*_T95;struct Cyc_List_List*_T96;int _T97;void*_T98;void*_T99;int _T9A;_T1=(int*)_T94;_T2=*_T1;switch(_T2){case 6: _T3=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T94;_T4=_T3->f1;if(_T4!=0)goto _TL1;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T9B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T94;_T9A=_T9B->f2;_T99=_T9B->f3;_T98=_T9B->f4;_T97=_T9B->f5;}{int is_tuple=_T9A;struct Cyc_List_List*exist_ts=_T99;struct Cyc_List_List*dps=_T98;int dots=_T97;
# 52
if(topt!=0)goto _TL3;_T5=is_tuple;
if(!_T5)goto _TL5;_T6=dots;if(_T6)goto _TL5;else{goto _TL7;}
_TL7:{struct Cyc_List_List*fields=0;_T7=
Cyc_List_length(dps);{unsigned n=(unsigned)_T7;{
int i=0;_TLB: _T8=i;_T9=(unsigned)_T8;_TA=n;if(_T9 < _TA)goto _TL9;else{goto _TLA;}
_TL9:{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T9B=Cyc_Absyn_tuple_field_designator(i);struct _fat_ptr*_T9C;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T9D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T9B;_T9C=_T9D->f1;}{struct _fat_ptr*n=_T9C;
struct Cyc_Absyn_Aggrfield*f;f=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_TB=f;_TB->name=n;_TC=f;_TC->tq=Cyc_Absyn_empty_tqual(0U);_TD=f;_TE=& Cyc_Kinds_mko;_TF=(struct Cyc_Core_Opt*)_TE;_T10=Cyc_Tcenv_lookup_opt_type_vars(te);_TD->type=Cyc_Absyn_new_evar(_TF,_T10);_T11=f;_T11->width=0;_T12=f;_T12->attributes=0;_T13=f;_T13->requires_clause=0;{struct Cyc_List_List*_T9D=_cycalloc(sizeof(struct Cyc_List_List));
_T9D->hd=f;_T9D->tl=fields;_T14=(struct Cyc_List_List*)_T9D;}fields=_T14;}}
# 56
i=i + 1;goto _TLB;_TLA:;}
# 61
fields=Cyc_List_imp_rev(fields);{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T9B=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_T9B->tag=7;
_T9B->f1=0U;_T9B->f2=is_tuple;_T9B->f3=fields;_T15=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T9B;}{void*t=(void*)_T15;_T16=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T9B=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T9B->tag=6;
_T9B->f1=t;_T9B->f2=is_tuple;_T9B->f3=exist_ts;_T9B->f4=dps;_T9B->f5=dots;_T17=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T9B;}_T16->r=(void*)_T17;}}}goto _TL6;
# 65
_TL5:{struct Cyc_Warn_String_Warn_Warg_struct _T9B;_T9B.tag=0;_T9B.f1=_tag_fat("cannot determine pattern type",sizeof(char),30U);_T18=_T9B;}{struct Cyc_Warn_String_Warn_Warg_struct _T9B=_T18;void*_T9C[1];_T19=_T9C + 0;*_T19=& _T9B;_T1A=p;_T1B=_T1A->loc;_T1C=_tag_fat(_T9C,sizeof(void*),1);Cyc_Warn_err2(_T1B,_T1C);}_TL6: goto _TL4;
# 67
_TL3: _T1D=topt;_T1E=*_T1D;{void*type=Cyc_Absyn_compress(_T1E);_T1F=type;_T20=(int*)_T1F;_T21=*_T20;switch(_T21){case 0: _T22=type;_T23=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T22;_T24=_T23->f1;_T25=(int*)_T24;_T26=*_T25;if(_T26!=24)goto _TLD;_T27=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T9B=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T9B->tag=6;
# 70
_T9B->f1=type;_T9B->f2=is_tuple;_T9B->f3=exist_ts;_T9B->f4=dps;_T9B->f5=dots;_T28=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T9B;}_T27->r=(void*)_T28;
Cyc_Tcpat_resolve_pat(te,topt,p);goto _LL10;_TLD: goto _LL15;case 7: _T29=p;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T9B=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T9B->tag=6;
# 74
_T9B->f1=type;_T9B->f2=is_tuple;_T9B->f3=exist_ts;_T9B->f4=dps;_T9B->f5=dots;_T2A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T9B;}_T29->r=(void*)_T2A;
Cyc_Tcpat_resolve_pat(te,topt,p);goto _LL10;default: _LL15:{struct Cyc_Warn_String_Warn_Warg_struct _T9B;_T9B.tag=0;
# 78
_T9B.f1=_tag_fat("pattern expects aggregate type instead of ",sizeof(char),43U);_T2B=_T9B;}{struct Cyc_Warn_String_Warn_Warg_struct _T9B=_T2B;{struct Cyc_Warn_Typ_Warn_Warg_struct _T9C;_T9C.tag=2;_T2D=topt;_T9C.f1=*_T2D;_T2C=_T9C;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T9C=_T2C;void*_T9D[2];_T2E=_T9D + 0;*_T2E=& _T9B;_T2F=_T9D + 1;*_T2F=& _T9C;_T30=p;_T31=_T30->loc;_T32=_tag_fat(_T9D,sizeof(void*),2);Cyc_Warn_err2(_T31,_T32);}}goto _LL10;}_LL10:;}_TL4:
# 82
 return;}_TL1:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T9B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T94;_T33=_T9B->f1;_T99=(void*)_T33;_T9A=_T9B->f2;_T34=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T94;_T35=& _T34->f3;_T98=(struct Cyc_List_List**)_T35;_T96=_T9B->f4;_T97=_T9B->f5;}{void*topt=_T99;int is_tuple=_T9A;struct Cyc_List_List**exist_ts=(struct Cyc_List_List**)_T98;struct Cyc_List_List*dps=_T96;int dots=_T97;
# 85
void*_T9B=Cyc_Absyn_compress(topt);struct Cyc_Absyn_Aggrdecl**_T9C;_T36=(int*)_T9B;_T37=*_T36;switch(_T37){case 0: _T38=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9B;_T39=_T38->f1;_T3A=(int*)_T39;_T3B=*_T3A;if(_T3B!=24)goto _TL10;_T3C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9B;_T3D=_T3C->f1;_T3E=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T3D;_T3F=_T3E->f1;_T40=_T3F.KnownAggr;_T41=_T40.tag;if(_T41!=2)goto _TL12;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9B;_T42=_T9D->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T9E=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T42;_T43=_T9E->f1;_T44=_T43.KnownAggr;_T9C=_T44.val;}}{struct Cyc_Absyn_Aggrdecl**adp=_T9C;_T45=adp;{
# 87
struct Cyc_Absyn_Aggrdecl*ad=*_T45;_T46=ad;_T47=_T46->impl;
if(_T47!=0)goto _TL14;{struct Cyc_Warn_String_Warn_Warg_struct _T9D;_T9D.tag=0;
_T9D.f1=_tag_fat("can't destructure an abstract",sizeof(char),30U);_T48=_T9D;}{struct Cyc_Warn_String_Warn_Warg_struct _T9D=_T48;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T9E;_T9E.tag=6;_T9E.f1=ad;_T49=_T9E;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T9E=_T49;void*_T9F[2];_T4A=_T9F + 0;*_T4A=& _T9D;_T4B=_T9F + 1;*_T4B=& _T9E;_T4C=p;_T4D=_T4C->loc;_T4E=_tag_fat(_T9F,sizeof(void*),2);Cyc_Warn_err2(_T4D,_T4E);}}_T4F=p;_T50=& Cyc_Absyn_Wild_p_val;_T51=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T50;
_T4F->r=(void*)_T51;
return;_TL14: _T52=ad;_T53=_T52->impl;_T54=_T53->exist_vars;_T55=
# 93
Cyc_List_length(_T54);_T56=exist_ts;_T57=*_T56;_T58=Cyc_List_length(_T57);{int more_exists=_T55 - _T58;
if(more_exists >= 0)goto _TL16;{struct Cyc_Warn_String_Warn_Warg_struct _T9D;_T9D.tag=0;
_T9D.f1=_tag_fat("too many existential type variables in pattern",sizeof(char),47U);_T59=_T9D;}{struct Cyc_Warn_String_Warn_Warg_struct _T9D=_T59;void*_T9E[1];_T5A=_T9E + 0;*_T5A=& _T9D;_T5B=p;_T5C=_T5B->loc;_T5D=_tag_fat(_T9E,sizeof(void*),1);Cyc_Warn_err2(_T5C,_T5D);}{
struct Cyc_List_List**ts=exist_ts;_T5E=ad;_T5F=_T5E->impl;_T60=
_check_null(_T5F);_T61=_T60->exist_vars;{int n=Cyc_List_length(_T61);_TL1B: if(n!=0)goto _TL19;else{goto _TL1A;}
_TL19: _T62=ts;_T63=*_T62;_T64=_check_null(_T63);ts=& _T64->tl;
# 97
n=n + -1;goto _TL1B;_TL1A:;}_T65=ts;
# 99
*_T65=0;}goto _TL17;
_TL16: if(more_exists <= 0)goto _TL1C;{
# 102
struct Cyc_List_List*new_ts=0;
_TL21: if(more_exists!=0)goto _TL1F;else{goto _TL20;}
_TL1F:{struct Cyc_List_List*_T9D=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T9E=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_T9E->tag=1;_T9E->f1=0;_T67=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T9E;}_T68=(void*)_T67;_T9D->hd=Cyc_Tcutil_new_tvar(_T68);_T9D->tl=new_ts;_T66=(struct Cyc_List_List*)_T9D;}new_ts=_T66;
# 103
more_exists=more_exists + -1;goto _TL21;_TL20: _T69=exist_ts;_T6A=exist_ts;_T6B=*_T6A;_T6C=new_ts;
# 105
*_T69=Cyc_List_imp_append(_T6B,_T6C);}goto _TL1D;_TL1C: _TL1D: _TL17:
# 107
 return;}}}_TL12: goto _LL1C;_TL10: goto _LL1C;case 7:
# 109
 return;default: _LL1C:{struct Cyc_Warn_String_Warn_Warg_struct _T9D;_T9D.tag=0;
_T9D.f1=_tag_fat("resolve_pat unknown aggrtype ",sizeof(char),30U);_T6D=_T9D;}{struct Cyc_Warn_String_Warn_Warg_struct _T9D=_T6D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T9E;_T9E.tag=2;_T9E.f1=topt;_T6E=_T9E;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T9E=_T6E;void*_T9F[2];_T6F=_T9F + 0;*_T6F=& _T9D;_T70=_T9F + 1;*_T70=& _T9E;_T72=Cyc_Warn_impos2;{int(*_TA0)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T72;_T71=_TA0;}_T73=_tag_fat(_T9F,sizeof(void*),2);_T71(_T73);}}};};case 16:{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T9B=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T94;_T95=_T9B->f1;}{struct Cyc_Absyn_Exp*e=_T95;
# 113
Cyc_Tcexp_tcExp(te,0,e);_T74=
Cyc_Tcutil_is_const_exp(e);if(_T74)goto _TL22;else{goto _TL24;}
_TL24:{struct Cyc_Warn_String_Warn_Warg_struct _T9B;_T9B.tag=0;_T9B.f1=_tag_fat("non-constant expression in case pattern",sizeof(char),40U);_T75=_T9B;}{struct Cyc_Warn_String_Warn_Warg_struct _T9B=_T75;void*_T9C[1];_T76=_T9C + 0;*_T76=& _T9B;_T77=p;_T78=_T77->loc;_T79=_tag_fat(_T9C,sizeof(void*),1);Cyc_Warn_err2(_T78,_T79);}_T7A=p;_T7B=& Cyc_Absyn_Wild_p_val;_T7C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T7B;
_T7A->r=(void*)_T7C;
return;_TL22: {
# 119
struct _tuple15 _T9B=Cyc_Evexp_eval_const_uint_exp(e);int _T9C;unsigned _T9D;_T9D=_T9B.f0;_T9C=_T9B.f1;{unsigned i=_T9D;int known=_T9C;_T7D=known;
if(_T7D)goto _TL25;else{goto _TL27;}
_TL27:{struct Cyc_String_pa_PrintArg_struct _T9E;_T9E.tag=0;_T9E.f1=Cyc_Absynpp_exp2string(e);_T7E=_T9E;}{struct Cyc_String_pa_PrintArg_struct _T9E=_T7E;void*_T9F[1];_T7F=_T9F + 0;*_T7F=& _T9E;_T80=p;_T81=_T80->loc;_T82=_tag_fat("cannot evaluate expression at compile time\nWe suggest using x && x == %s instead",sizeof(char),81U);_T83=_tag_fat(_T9F,sizeof(void*),1);Cyc_Warn_err(_T81,_T82,_T83);}_T84=p;_T85=& Cyc_Absyn_Wild_p_val;_T86=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T85;
_T84->r=(void*)_T86;
return;_TL25: _T87=p;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T9E=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T9E->tag=9;
# 125
_T9E->f1=2U;_T89=i;_T9E->f2=(int)_T89;_T88=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T9E;}_T87->r=(void*)_T88;
return;}}}case 14:{struct Cyc_Warn_String_Warn_Warg_struct _T9B;_T9B.tag=0;
# 128
_T9B.f1=_tag_fat("resolve_pat UnknownId_p",sizeof(char),24U);_T8A=_T9B;}{struct Cyc_Warn_String_Warn_Warg_struct _T9B=_T8A;void*_T9C[1];_T8B=_T9C + 0;*_T8B=& _T9B;_T8D=Cyc_Warn_impos2;{int(*_T9D)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T8D;_T8C=_T9D;}_T8E=_tag_fat(_T9C,sizeof(void*),1);_T8C(_T8E);}case 15:{struct Cyc_Warn_String_Warn_Warg_struct _T9B;_T9B.tag=0;
_T9B.f1=_tag_fat("resolve_pat UnknownCall_p",sizeof(char),26U);_T8F=_T9B;}{struct Cyc_Warn_String_Warn_Warg_struct _T9B=_T8F;void*_T9C[1];_T90=_T9C + 0;*_T90=& _T9B;_T92=Cyc_Warn_impos2;{int(*_T9D)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T92;_T91=_T9D;}_T93=_tag_fat(_T9C,sizeof(void*),1);_T91(_T93);}default:
# 131
 return;};}}
# 135
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){struct Cyc_Absyn_Pat*_T0;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T2;{struct Cyc_Absyn_Pat*_T3=_cycalloc(sizeof(struct Cyc_Absyn_Pat));
_T3->loc=loc;_T3->topt=0;_T1=& Cyc_Absyn_Wild_p_val;_T2=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T1;_T3->r=(void*)_T2;_T0=(struct Cyc_Absyn_Pat*)_T3;}return _T0;}struct _tuple16{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 138
static struct _tuple16*Cyc_Tcpat_wild_dlp(unsigned loc){struct _tuple16*_T0;{struct _tuple16*_T1=_cycalloc(sizeof(struct _tuple16));
_T1->f0=0;_T1->f1=Cyc_Tcpat_wild_pat(loc);_T0=(struct _tuple16*)_T1;}return _T0;}
# 142
static struct Cyc_List_List*Cyc_Tcpat_remove_dots(struct Cyc_List_List*earlier,int num_pats,int num_fields,void*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str){struct Cyc_Warn_String_Warn_Warg_struct _T0;struct Cyc_Warn_String_Warn_Warg_struct _T1;struct Cyc_Warn_String_Warn_Warg_struct _T2;void**_T3;void**_T4;void**_T5;unsigned _T6;struct _fat_ptr _T7;int _T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;
# 144
if(num_pats < num_fields)goto _TL28;{struct Cyc_Warn_String_Warn_Warg_struct _TC;_TC.tag=0;
_TC.f1=_tag_fat("unnecessary ... in ",sizeof(char),20U);_T0=_TC;}{struct Cyc_Warn_String_Warn_Warg_struct _TC=_T0;{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;_TD.f1=msg_str;_T1=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T1;{struct Cyc_Warn_String_Warn_Warg_struct _TE;_TE.tag=0;_TE.f1=_tag_fat(" pattern",sizeof(char),9U);_T2=_TE;}{struct Cyc_Warn_String_Warn_Warg_struct _TE=_T2;void*_TF[3];_T3=_TF + 0;*_T3=& _TC;_T4=_TF + 1;*_T4=& _TD;_T5=_TF + 2;*_T5=& _TE;_T6=loc;_T7=_tag_fat(_TF,sizeof(void*),3);Cyc_Warn_warn2(_T6,_T7);}}}goto _TL29;_TL28: _TL29: {
struct Cyc_List_List*new_ps=0;{
int i=0;_TL2D: _T8=i;_T9=num_fields - num_pats;if(_T8 < _T9)goto _TL2B;else{goto _TL2C;}
_TL2B:{struct Cyc_List_List*_TC=_cycalloc(sizeof(struct Cyc_List_List));_TC->hd=f(loc);_TC->tl=new_ps;_TA=(struct Cyc_List_List*)_TC;}new_ps=_TA;
# 147
i=i + 1;goto _TL2D;_TL2C:;}_TB=
# 149
Cyc_List_imp_append(earlier,new_ps);return _TB;}}
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
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){struct Cyc_Tcpat_TcPatResult _T0;struct Cyc_Absyn_Pat*_T1;int*_T2;unsigned _T3;struct Cyc_List_List*_T4;void**_T5;struct Cyc_Absyn_Vardecl*_T6;struct _fat_ptr*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;int _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;void**_T11;void**_T12;void**_T13;struct Cyc_Absyn_Pat*_T14;unsigned _T15;struct _fat_ptr _T16;struct Cyc_Absyn_Pat*_T17;void*_T18;int*_T19;int _T1A;int _T1B;struct Cyc_Warn_String_Warn_Warg_struct _T1C;void**_T1D;struct Cyc_Absyn_Pat*_T1E;unsigned _T1F;struct _fat_ptr _T20;struct Cyc_Absyn_Kind*_T21;struct Cyc_Absyn_Kind*_T22;struct Cyc_Absyn_Kind*_T23;int _T24;struct Cyc_Warn_String_Warn_Warg_struct _T25;void**_T26;struct Cyc_Absyn_Pat*_T27;unsigned _T28;struct _fat_ptr _T29;struct Cyc_Absyn_Vardecl**_T2A;struct Cyc_Absyn_Exp*_T2B;struct Cyc_List_List**_T2C;void*_T2D;struct Cyc_Absyn_Pat*_T2E;unsigned _T2F;struct Cyc_Tcenv_Tenv*_T30;struct Cyc_List_List*_T31;struct Cyc_Absyn_Tvar**_T32;struct _fat_ptr _T33;struct Cyc_Tcpat_TcPatResult _T34;struct _tuple1*_T35;struct _tuple1*_T36;struct Cyc_Tcpat_TcPatResult _T37;struct _tuple1*_T38;struct Cyc_Tcpat_TcPatResult _T39;struct _tuple1*_T3A;struct _tuple1 _T3B;struct Cyc_List_List*_T3C;struct Cyc_List_List*_T3D;struct _tuple18*_T3E;struct Cyc_Absyn_Pat*_T3F;unsigned _T40;struct Cyc_Tcenv_Tenv*_T41;struct Cyc_List_List*_T42;struct Cyc_Absyn_Kind*_T43;struct Cyc_Absyn_Kind*_T44;struct Cyc_Absyn_Vardecl*_T45;void*_T46;struct Cyc_Absyn_Vardecl*_T47;int*_T48;int _T49;struct Cyc_Absyn_PtrInfo _T4A;struct Cyc_Absyn_PtrAtts _T4B;void*_T4C;void*_T4D;int _T4E;struct Cyc_Warn_String_Warn_Warg_struct _T4F;struct Cyc_Warn_Typ_Warn_Warg_struct _T50;struct Cyc_Warn_String_Warn_Warg_struct _T51;struct Cyc_Warn_Typ_Warn_Warg_struct _T52;void**_T53;void**_T54;void**_T55;void**_T56;struct Cyc_Absyn_Pat*_T57;unsigned _T58;struct _fat_ptr _T59;struct Cyc_List_List*_T5A;void**_T5B;int*_T5C;int _T5D;int _T5E;struct Cyc_Warn_String_Warn_Warg_struct _T5F;void**_T60;struct Cyc_Absyn_Pat*_T61;unsigned _T62;struct _fat_ptr _T63;struct Cyc_Absyn_Kind*_T64;struct Cyc_Absyn_Kind*_T65;struct Cyc_Absyn_Kind*_T66;int _T67;struct Cyc_Warn_String_Warn_Warg_struct _T68;void**_T69;struct Cyc_Absyn_Pat*_T6A;unsigned _T6B;struct _fat_ptr _T6C;struct Cyc_Absyn_Vardecl**_T6D;struct Cyc_Absyn_Exp*_T6E;struct Cyc_List_List**_T6F;struct Cyc_Absyn_Vardecl*_T70;void*_T71;struct Cyc_Absyn_Pat*_T72;void*_T73;int _T74;struct Cyc_Warn_String_Warn_Warg_struct _T75;void**_T76;struct Cyc_Absyn_Pat*_T77;unsigned _T78;struct _fat_ptr _T79;struct Cyc_List_List*_T7A;void*_T7B;int _T7C;struct Cyc_Warn_String_Warn_Warg_struct _T7D;void**_T7E;struct Cyc_Absyn_Pat*_T7F;unsigned _T80;struct _fat_ptr _T81;struct Cyc_Absyn_Exp*_T82;unsigned _T83;struct Cyc_Absyn_Exp*_T84;struct Cyc_Absyn_Vardecl**_T85;struct Cyc_Absyn_Exp*_T86;struct Cyc_List_List**_T87;void*_T88;struct Cyc_Absyn_Vardecl**_T89;struct Cyc_Absyn_Exp*_T8A;struct Cyc_List_List**_T8B;struct Cyc_Absyn_Vardecl*_T8C;void*_T8D;struct Cyc_Absyn_Pat*_T8E;unsigned _T8F;struct Cyc_Tcenv_Tenv*_T90;struct Cyc_List_List*_T91;struct Cyc_Absyn_Tvar**_T92;struct _fat_ptr _T93;struct Cyc_Tcpat_TcPatResult _T94;struct _tuple1*_T95;struct _tuple1*_T96;struct Cyc_Tcpat_TcPatResult _T97;struct _tuple1*_T98;struct Cyc_List_List*_T99;struct _tuple18*_T9A;struct Cyc_Tcpat_TcPatResult _T9B;struct _tuple1*_T9C;struct _tuple1 _T9D;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T9E;enum Cyc_Absyn_Sign _T9F;void**_TA0;int _TA1;unsigned _TA2;void*_TA3;void**_TA4;struct Cyc_Absyn_Enumdecl*_TA5;struct _tuple2*_TA6;struct Cyc_Absyn_Enumdecl*_TA7;void*_TA8;void*_TA9;void**_TAA;void*_TAB;int*_TAC;int _TAD;void**_TAE;void**_TAF;void*_TB0;int*_TB1;int _TB2;struct Cyc_Absyn_PtrInfo _TB3;struct Cyc_Absyn_PtrInfo _TB4;struct Cyc_Absyn_Tqual _TB5;struct Cyc_Core_Opt*_TB6;struct Cyc_Core_Opt*_TB7;struct Cyc_Core_Opt*_TB8;struct Cyc_Absyn_Exp*_TB9;unsigned _TBA;struct Cyc_Tcpat_TcPatResult _TBB;struct Cyc_Tcpat_TcPatResult _TBC;struct Cyc_Absyn_Pat*_TBD;void*_TBE;void*_TBF;int*_TC0;int _TC1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC2;void*_TC3;int*_TC4;int _TC5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TC6;void*_TC7;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TC8;union Cyc_Absyn_DatatypeFieldInfo _TC9;struct _union_DatatypeFieldInfo_KnownDatatypefield _TCA;unsigned _TCB;void*_TCC;union Cyc_Absyn_DatatypeFieldInfo _TCD;struct _union_DatatypeFieldInfo_KnownDatatypefield _TCE;struct _tuple3 _TCF;union Cyc_Absyn_DatatypeFieldInfo _TD0;struct _union_DatatypeFieldInfo_KnownDatatypefield _TD1;struct _tuple3 _TD2;int*_TD3;int _TD4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD5;void*_TD6;int*_TD7;int _TD8;struct Cyc_Absyn_Datatypedecl**_TD9;union Cyc_Absyn_DatatypeInfo _TDA;struct Cyc_List_List*_TDB;struct Cyc_Absyn_Pat*_TDC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TDD;int _TDE;struct Cyc_List_List*_TDF;struct Cyc_List_List*_TE0;struct Cyc_Core_Opt*_TE1;struct Cyc_Core_Opt*_TE2;struct Cyc_Core_Opt*_TE3;struct Cyc_Absyn_Pat*_TE4;void*_TE5;struct Cyc_Absyn_Tqual _TE6;int _TE7;struct Cyc_Absyn_Exp*_TE8;unsigned _TE9;struct Cyc_Absyn_Exp*_TEA;struct Cyc_Absyn_Pat*_TEB;void*_TEC;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_TED;struct Cyc_List_List**_TEE;struct Cyc_List_List**_TEF;struct Cyc_Core_Opt*_TF0;int*_TF1;unsigned _TF2;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TF3;void*_TF4;int*_TF5;int _TF6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TF7;void*_TF8;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TF9;union Cyc_Absyn_AggrInfo _TFA;struct _union_AggrInfo_KnownAggr _TFB;unsigned _TFC;void*_TFD;struct _union_AggrInfo_KnownAggr _TFE;struct Cyc_Absyn_Aggrdecl**_TFF;struct Cyc_Absyn_Aggrdecl*_T100;struct Cyc_Absyn_AggrdeclImpl*_T101;struct Cyc_Warn_String_Warn_Warg_struct _T102;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T103;void**_T104;void**_T105;struct Cyc_Absyn_Pat*_T106;unsigned _T107;struct _fat_ptr _T108;struct Cyc_Core_Opt*_T109;struct Cyc_Absyn_Aggrdecl*_T10A;enum Cyc_Absyn_AggrKind _T10B;int _T10C;struct Cyc_Absyn_Aggrdecl*_T10D;struct Cyc_Absyn_AggrdeclImpl*_T10E;int _T10F;void**_T110;void*_T111;struct Cyc_Absyn_Kind*_T112;struct Cyc_Absyn_Kind*_T113;struct Cyc_Absyn_Kind*_T114;struct Cyc_Absyn_Aggrdecl*_T115;struct Cyc_Absyn_AggrdeclImpl*_T116;struct Cyc_Absyn_AggrdeclImpl*_T117;struct Cyc_List_List*_T118;void*_T119;struct Cyc_List_List*_T11A;void*_T11B;struct Cyc_List_List*_T11C;struct Cyc_Absyn_Tvar*_T11D;void*_T11E;struct Cyc_Absyn_Tvar*_T11F;void*_T120;void*_T121;int*_T122;unsigned _T123;void*_T124;void*_T125;struct Cyc_Warn_String_Warn_Warg_struct _T126;void**_T127;int(*_T128)(struct _fat_ptr);void*(*_T129)(struct _fat_ptr);struct _fat_ptr _T12A;void*_T12B;int*_T12C;unsigned _T12D;void*_T12E;int _T12F;struct Cyc_Warn_String_Warn_Warg_struct _T130;struct Cyc_Warn_Tvar_Warn_Warg_struct _T131;struct Cyc_Warn_String_Warn_Warg_struct _T132;struct Cyc_Warn_KindBound_Warn_Warg_struct _T133;struct Cyc_Warn_String_Warn_Warg_struct _T134;struct Cyc_Warn_Kind_Warn_Warg_struct _T135;void**_T136;void**_T137;void**_T138;void**_T139;void**_T13A;void**_T13B;struct Cyc_Absyn_Pat*_T13C;unsigned _T13D;struct _fat_ptr _T13E;void*_T13F;void*_T140;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T141;struct Cyc_Core_Opt**_T142;void*_T143;void*_T144;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T145;struct Cyc_Core_Opt**_T146;struct Cyc_Core_Opt**_T147;struct Cyc_Core_Opt*_T148;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T149;struct Cyc_List_List*_T14A;struct _RegionHandle*_T14B;struct Cyc_List_List*_T14C;struct Cyc_Absyn_Pat*_T14D;unsigned _T14E;struct Cyc_Tcenv_Tenv*_T14F;struct Cyc_List_List*_T150;struct _tuple13 _T151;struct Cyc_List_List*(*_T152)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T153)(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _RegionHandle*_T154;struct _tuple14*(*_T155)(struct _tuple13*,struct Cyc_Absyn_Tvar*);struct _tuple13*_T156;struct Cyc_Absyn_Aggrdecl*_T157;struct Cyc_List_List*_T158;struct _RegionHandle*_T159;struct _RegionHandle*_T15A;struct Cyc_Absyn_Aggrdecl*_T15B;struct Cyc_Absyn_AggrdeclImpl*_T15C;struct Cyc_Absyn_AggrdeclImpl*_T15D;struct Cyc_List_List*_T15E;struct Cyc_List_List*_T15F;struct Cyc_List_List*(*_T160)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T161)(void*(*)(void*),struct Cyc_List_List*);void*(*_T162)(struct _tuple14*);void*(*_T163)(struct _tuple0*);struct Cyc_List_List*_T164;struct Cyc_List_List*(*_T165)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T166)(void*(*)(void*),struct Cyc_List_List*);void*(*_T167)(struct _tuple14*);void*(*_T168)(struct _tuple0*);struct Cyc_List_List*_T169;struct Cyc_Absyn_Aggrdecl*_T16A;struct Cyc_Absyn_AggrdeclImpl*_T16B;struct Cyc_Absyn_AggrdeclImpl*_T16C;struct Cyc_List_List*_T16D;struct Cyc_Tcpat_TcPatResult _T16E;struct _tuple1*_T16F;struct _tuple1*_T170;struct Cyc_Tcpat_TcPatResult _T171;struct _tuple1*_T172;struct Cyc_Tcpat_TcPatResult _T173;struct _tuple1*_T174;struct _tuple1 _T175;struct Cyc_List_List*_T176;struct Cyc_List_List*(*_T177)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*);struct Cyc_List_List*(*_T178)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T179;struct Cyc_List_List*_T17A;struct _RegionHandle*_T17B;struct Cyc_List_List*_T17C;struct Cyc_Absyn_Aggrdecl*_T17D;struct Cyc_Absyn_AggrdeclImpl*_T17E;struct Cyc_Absyn_AggrdeclImpl*_T17F;struct Cyc_List_List*_T180;struct Cyc_Tcpat_TcPatResult _T181;struct _tuple1*_T182;struct Cyc_Tcpat_TcPatResult _T183;struct _tuple1*_T184;struct _tuple1 _T185;struct Cyc_List_List*_T186;struct Cyc_List_List*_T187;struct Cyc_Absyn_Aggrdecl*_T188;struct Cyc_Absyn_AggrdeclImpl*_T189;struct Cyc_Absyn_AggrdeclImpl*_T18A;struct _tuple0 _T18B;struct _RegionHandle*_T18C;struct Cyc_List_List*_T18D;struct Cyc_List_List*_T18E;void*_T18F;struct _tuple0*_T190;struct _tuple0 _T191;void*_T192;struct _RegionHandle*_T193;struct Cyc_List_List*_T194;struct Cyc_List_List*_T195;void*_T196;struct _tuple0*_T197;struct _tuple0 _T198;void*_T199;struct Cyc_List_List*_T19A;struct _tuple0*_T19B;struct Cyc_List_List*_T19C;struct Cyc_Absyn_Aggrdecl**_T19D;union Cyc_Absyn_AggrInfo _T19E;struct Cyc_List_List*_T19F;int _T1A0;struct Cyc_Absyn_Aggrdecl*_T1A1;enum Cyc_Absyn_AggrKind _T1A2;int _T1A3;struct Cyc_Absyn_Pat*_T1A4;unsigned _T1A5;struct _fat_ptr _T1A6;struct _fat_ptr _T1A7;int _T1A8;struct Cyc_List_List**_T1A9;struct Cyc_List_List*(*_T1AA)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr);struct Cyc_List_List*_T1AB;int _T1AC;struct Cyc_Absyn_Aggrdecl*_T1AD;struct Cyc_Absyn_AggrdeclImpl*_T1AE;struct Cyc_Absyn_AggrdeclImpl*_T1AF;struct Cyc_List_List*_T1B0;int _T1B1;struct Cyc_Absyn_Pat*_T1B2;unsigned _T1B3;struct _fat_ptr _T1B4;struct Cyc_List_List**_T1B5;struct _RegionHandle*_T1B6;struct Cyc_Absyn_Pat*_T1B7;unsigned _T1B8;struct Cyc_List_List*_T1B9;struct Cyc_Absyn_Aggrdecl*_T1BA;enum Cyc_Absyn_AggrKind _T1BB;struct Cyc_Absyn_Aggrdecl*_T1BC;struct Cyc_Absyn_AggrdeclImpl*_T1BD;struct Cyc_Absyn_AggrdeclImpl*_T1BE;struct Cyc_List_List*_T1BF;struct Cyc_List_List*_T1C0;void*_T1C1;struct _RegionHandle*_T1C2;struct Cyc_List_List*_T1C3;struct Cyc_Absyn_Aggrfield*_T1C4;void*_T1C5;struct Cyc_Absyn_Exp*_T1C6;unsigned _T1C7;struct Cyc_Absyn_Exp*_T1C8;struct Cyc_Absyn_Aggrfield*_T1C9;struct _fat_ptr*_T1CA;struct Cyc_Tcpat_TcPatResult _T1CB;struct Cyc_Tcenv_Tenv*_T1CC;struct Cyc_Absyn_Pat*_T1CD;void**_T1CE;void*_T1CF;int _T1D0;struct Cyc_Absyn_Exp*_T1D1;struct Cyc_Tcpat_TcPatResult _T1D2;struct Cyc_Absyn_Pat*_T1D3;unsigned _T1D4;struct Cyc_Absyn_Pat*_T1D5;void*_T1D6;void*_T1D7;void*_T1D8;int _T1D9;struct Cyc_Warn_String_Warn_Warg_struct _T1DA;struct Cyc_Warn_String_Warn_Warg_struct _T1DB;struct Cyc_Absyn_Aggrfield*_T1DC;struct _fat_ptr*_T1DD;struct Cyc_Warn_String_Warn_Warg_struct _T1DE;struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T1DF;struct Cyc_Warn_String_Warn_Warg_struct _T1E0;struct Cyc_Warn_Typ_Warn_Warg_struct _T1E1;struct Cyc_Warn_String_Warn_Warg_struct _T1E2;struct Cyc_Warn_Typ_Warn_Warg_struct _T1E3;struct Cyc_Absyn_Pat*_T1E4;void*_T1E5;void**_T1E6;void**_T1E7;void**_T1E8;void**_T1E9;void**_T1EA;void**_T1EB;void**_T1EC;void**_T1ED;struct Cyc_Absyn_Pat*_T1EE;unsigned _T1EF;struct _fat_ptr _T1F0;struct Cyc_Absyn_Exp*_T1F1;unsigned _T1F2;struct Cyc_Absyn_Exp*_T1F3;struct Cyc_Absyn_Pat*_T1F4;struct Cyc_List_List*_T1F5;struct Cyc_Warn_String_Warn_Warg_struct _T1F6;void**_T1F7;struct Cyc_Absyn_Pat*_T1F8;unsigned _T1F9;struct _fat_ptr _T1FA;int _T1FB;enum Cyc_Absyn_AggrKind _T1FC;int _T1FD;struct Cyc_Absyn_Pat*_T1FE;unsigned _T1FF;struct _fat_ptr _T200;struct _fat_ptr _T201;int _T202;struct Cyc_List_List**_T203;struct Cyc_List_List*(*_T204)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr);struct Cyc_List_List*_T205;int _T206;int _T207;struct Cyc_Absyn_Pat*_T208;unsigned _T209;struct _fat_ptr _T20A;int _T20B;struct Cyc_List_List**_T20C;struct _RegionHandle*_T20D;struct Cyc_Absyn_Pat*_T20E;unsigned _T20F;struct Cyc_List_List*_T210;enum Cyc_Absyn_AggrKind _T211;struct Cyc_List_List*_T212;struct Cyc_List_List*_T213;void*_T214;struct Cyc_Absyn_Aggrfield*_T215;struct Cyc_Absyn_Exp*_T216;unsigned _T217;struct Cyc_Absyn_Exp*_T218;struct Cyc_Absyn_Aggrfield*_T219;struct _fat_ptr*_T21A;struct Cyc_Tcpat_TcPatResult _T21B;struct Cyc_Tcenv_Tenv*_T21C;struct Cyc_Absyn_Pat*_T21D;void**_T21E;void*_T21F;int _T220;struct Cyc_Absyn_Exp*_T221;struct Cyc_Tcpat_TcPatResult _T222;struct Cyc_Absyn_Pat*_T223;unsigned _T224;struct Cyc_Absyn_Pat*_T225;void*_T226;void*_T227;void*_T228;int _T229;struct Cyc_Warn_String_Warn_Warg_struct _T22A;struct Cyc_Warn_String_Warn_Warg_struct _T22B;struct Cyc_Absyn_Aggrfield*_T22C;struct _fat_ptr*_T22D;struct Cyc_Warn_String_Warn_Warg_struct _T22E;struct Cyc_Warn_Typ_Warn_Warg_struct _T22F;struct Cyc_Warn_String_Warn_Warg_struct _T230;struct Cyc_Warn_Typ_Warn_Warg_struct _T231;struct Cyc_Absyn_Pat*_T232;void*_T233;void**_T234;void**_T235;void**_T236;void**_T237;void**_T238;void**_T239;struct Cyc_Absyn_Pat*_T23A;unsigned _T23B;struct _fat_ptr _T23C;struct Cyc_Absyn_Exp*_T23D;unsigned _T23E;struct Cyc_Absyn_Exp*_T23F;struct Cyc_Absyn_Pat*_T240;struct Cyc_List_List*_T241;struct Cyc_Warn_String_Warn_Warg_struct _T242;void**_T243;int(*_T244)(struct _fat_ptr);void*(*_T245)(struct _fat_ptr);struct _fat_ptr _T246;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T247;struct Cyc_List_List**_T248;struct Cyc_List_List**_T249;struct Cyc_Absyn_Datatypefield*_T24A;struct _tuple13 _T24B;struct Cyc_List_List*(*_T24C)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*);struct Cyc_List_List*(*_T24D)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple14*(*_T24E)(struct _tuple13*,struct Cyc_Absyn_Tvar*);struct _tuple13*_T24F;struct Cyc_Absyn_Datatypedecl*_T250;struct Cyc_List_List*_T251;struct Cyc_List_List*(*_T252)(void*(*)(struct _tuple14*),struct Cyc_List_List*);struct Cyc_List_List*(*_T253)(void*(*)(void*),struct Cyc_List_List*);void*(*_T254)(struct _tuple14*);void*(*_T255)(struct _tuple0*);struct Cyc_List_List*_T256;union Cyc_Absyn_DatatypeFieldInfo _T257;struct Cyc_List_List*_T258;int _T259;struct Cyc_List_List**_T25A;struct Cyc_List_List*(*_T25B)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr);struct Cyc_List_List*_T25C;int _T25D;int _T25E;struct Cyc_Absyn_Pat*_T25F;unsigned _T260;struct _fat_ptr _T261;struct Cyc_List_List**_T262;struct Cyc_List_List*_T263;void*_T264;struct Cyc_List_List*_T265;struct Cyc_List_List*_T266;void*_T267;struct _tuple20*_T268;struct _tuple20 _T269;void*_T26A;struct Cyc_Absyn_Exp*_T26B;unsigned _T26C;struct Cyc_Absyn_Exp*_T26D;struct Cyc_List_List**_T26E;void*_T26F;struct Cyc_Tcpat_TcPatResult _T270;struct Cyc_Tcenv_Tenv*_T271;struct Cyc_Absyn_Pat*_T272;void**_T273;void*_T274;int _T275;struct Cyc_Tcpat_TcPatResult _T276;struct Cyc_Absyn_Pat*_T277;unsigned _T278;struct Cyc_Absyn_Pat*_T279;void*_T27A;void*_T27B;void*_T27C;int _T27D;struct Cyc_Warn_Qvar_Warn_Warg_struct _T27E;struct Cyc_Absyn_Datatypefield*_T27F;struct Cyc_Warn_String_Warn_Warg_struct _T280;struct Cyc_Warn_Typ_Warn_Warg_struct _T281;struct Cyc_Warn_String_Warn_Warg_struct _T282;struct Cyc_Warn_Typ_Warn_Warg_struct _T283;struct Cyc_Absyn_Pat*_T284;void*_T285;void**_T286;void**_T287;void**_T288;void**_T289;void**_T28A;struct Cyc_Absyn_Pat*_T28B;unsigned _T28C;struct _fat_ptr _T28D;struct Cyc_List_List*_T28E;struct Cyc_List_List*_T28F;struct Cyc_Warn_String_Warn_Warg_struct _T290;struct Cyc_Warn_Qvar_Warn_Warg_struct _T291;struct Cyc_Absyn_Datatypefield*_T292;void**_T293;void**_T294;struct Cyc_Absyn_Pat*_T295;unsigned _T296;struct _fat_ptr _T297;struct Cyc_Warn_String_Warn_Warg_struct _T298;struct Cyc_Warn_Qvar_Warn_Warg_struct _T299;struct Cyc_Absyn_Datatypefield*_T29A;void**_T29B;void**_T29C;struct Cyc_Absyn_Pat*_T29D;unsigned _T29E;struct _fat_ptr _T29F;struct Cyc_Core_Opt*_T2A0;struct Cyc_Absyn_Pat*_T2A1;struct Cyc_Tcpat_TcPatResult _T2A2;
# 213
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;{struct Cyc_Tcpat_TcPatResult _T2A3;
_T2A3.tvars_and_effconstr_opt=0;_T2A3.patvars=0;_T2A3.aquals_bounds=0;_T0=_T2A3;}{struct Cyc_Tcpat_TcPatResult res=_T0;_T1=p;{
# 218
void*_T2A3=_T1->r;struct Cyc_Absyn_Datatypefield*_T2A4;struct Cyc_Absyn_Datatypedecl*_T2A5;int _T2A6;struct Cyc_List_List*_T2A7;int _T2A8;void*_T2A9;void*_T2AA;_T2=(int*)_T2A3;_T3=*_T2;switch(_T3){case 0: _T4=
# 220
Cyc_Tcenv_lookup_type_vars(te);_T5=topt;t=Cyc_Tcpat_any_type(_T4,_T5);goto _LL0;case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T2A3;_T2AA=_T2AB->f1;_T2A9=_T2AB->f2;}{struct Cyc_Absyn_Vardecl*vd=_T2AA;struct Cyc_Absyn_Pat*p2=_T2A9;_T6=vd;{
# 224
struct _tuple2*_T2AB=_T6->name;struct _fat_ptr _T2AC;{struct _tuple2 _T2AD=*_T2AB;_T7=_T2AD.f1;{struct _fat_ptr _T2AE=*_T7;_T2AC=_T2AE;}}{struct _fat_ptr v=_T2AC;_T8=v;_T9=
_tag_fat("true",sizeof(char),5U);_TA=Cyc_strcmp(_T8,_T9);if(_TA==0)goto _TL46;else{goto _TL47;}_TL47: _TB=v;_TC=_tag_fat("false",sizeof(char),6U);_TD=Cyc_strcmp(_TB,_TC);if(_TD==0)goto _TL46;else{goto _TL44;}
_TL46:{struct Cyc_Warn_String_Warn_Warg_struct _T2AD;_T2AD.tag=0;_T2AD.f1=_tag_fat("you probably do not want to use ",sizeof(char),33U);_TE=_T2AD;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AD=_TE;{struct Cyc_Warn_String_Warn_Warg_struct _T2AE;_T2AE.tag=0;_T2AE.f1=v;_TF=_T2AE;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AE=_TF;{struct Cyc_Warn_String_Warn_Warg_struct _T2AF;_T2AF.tag=0;
_T2AF.f1=_tag_fat(" as a local variable",sizeof(char),21U);_T10=_T2AF;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AF=_T10;void*_T2B0[3];_T11=_T2B0 + 0;*_T11=& _T2AD;_T12=_T2B0 + 1;*_T12=& _T2AE;_T13=_T2B0 + 2;*_T13=& _T2AF;_T14=p;_T15=_T14->loc;_T16=_tag_fat(_T2B0,sizeof(void*),3);Cyc_Warn_warn2(_T15,_T16);}}}goto _TL45;_TL44: _TL45:
 res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);_T17=p2;_T18=_T17->topt;
t=_check_null(_T18);{
# 232
void*_T2AD=Cyc_Absyn_compress(t);_T19=(int*)_T2AD;_T1A=*_T19;if(_T1A!=5)goto _TL48;
# 234
if(rgn_pat==0)goto _TL4C;else{goto _TL4D;}_TL4D: _T1B=allow_ref_pat;if(_T1B)goto _TL4A;else{goto _TL4C;}
_TL4C:{struct Cyc_Warn_String_Warn_Warg_struct _T2AE;_T2AE.tag=0;_T2AE.f1=_tag_fat("array reference would point into unknown/unallowed region",sizeof(char),58U);_T1C=_T2AE;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AE=_T1C;void*_T2AF[1];_T1D=_T2AF + 0;*_T1D=& _T2AE;_T1E=p;_T1F=_T1E->loc;_T20=_tag_fat(_T2AF,sizeof(void*),1);Cyc_Warn_err2(_T1F,_T20);}goto _TL4B;_TL4A: _TL4B: goto _LL28;_TL48: _T21=
# 238
Cyc_Tcutil_type_kind(t);_T22=& Cyc_Kinds_mk;_T23=(struct Cyc_Absyn_Kind*)_T22;_T24=Cyc_Kinds_kind_leq(_T21,_T23);if(_T24)goto _TL4E;else{goto _TL50;}
_TL50:{struct Cyc_Warn_String_Warn_Warg_struct _T2AE;_T2AE.tag=0;_T2AE.f1=_tag_fat("pattern would point to an abstract member",sizeof(char),42U);_T25=_T2AE;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AE=_T25;void*_T2AF[1];_T26=_T2AF + 0;*_T26=& _T2AE;_T27=p;_T28=_T27->loc;_T29=_tag_fat(_T2AF,sizeof(void*),1);Cyc_Warn_err2(_T28,_T29);}goto _TL4F;_TL4E: _TL4F: goto _LL28;_LL28:;}{struct Cyc_Absyn_Vardecl**_T2AD=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));
# 242
*_T2AD=vd;_T2A=(struct Cyc_Absyn_Vardecl**)_T2AD;}_T2B=access_exp;_T2C=& res.patvars;_T2D=Cyc_Tcpat_pat_promote_array(te,t,rgn_pat);Cyc_Tcpat_set_vd(_T2A,_T2B,_T2C,_T2D);goto _LL0;}}}case 2:{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T2A3;_T2AA=_T2AB->f1;_T2A9=_T2AB->f2;}{struct Cyc_Absyn_Tvar*tv=_T2AA;struct Cyc_Absyn_Vardecl*vd=_T2A9;_T2E=p;_T2F=_T2E->loc;_T30=te;{struct Cyc_Absyn_Tvar*_T2AB[1];_T32=_T2AB + 0;*_T32=tv;_T33=_tag_fat(_T2AB,sizeof(struct Cyc_Absyn_Tvar*),1);_T31=Cyc_List_list(_T33);}{
# 245
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_T2F,_T30,_T31);_T34=res;_T35=_T34.tvars_and_effconstr_opt;
if(_T35!=0)goto _TL51;{struct _tuple1*_T2AB=_cycalloc(sizeof(struct _tuple1));
_T2AB->f0=0;_T2AB->f1=0;_T36=(struct _tuple1*)_T2AB;}res.tvars_and_effconstr_opt=_T36;goto _TL52;_TL51: _TL52: _T37=res;_T38=_T37.tvars_and_effconstr_opt;_T39=res;_T3A=_T39.tvars_and_effconstr_opt;_T3B=*_T3A;_T3C=_T3B.f0;{struct Cyc_List_List*_T2AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T2AC=_cycalloc(sizeof(struct _tuple18));
# 249
_T2AC->f0=tv;_T2AC->f1=1;_T3E=(struct _tuple18*)_T2AC;}_T2AB->hd=_T3E;_T2AB->tl=0;_T3D=(struct Cyc_List_List*)_T2AB;}
# 248
(*_T38).f0=
Cyc_List_append(_T3C,_T3D);_T3F=p;_T40=_T3F->loc;_T41=te2;_T42=
Cyc_Tcenv_lookup_type_vars(te2);_T43=& Cyc_Kinds_mk;_T44=(struct Cyc_Absyn_Kind*)_T43;_T45=vd;_T46=_T45->type;Cyc_Tctyp_check_type(_T40,_T41,_T42,_T44,1,0,_T46);_T47=vd;{
void*_T2AB=_T47->type;void*_T2AC;_T48=(int*)_T2AB;_T49=*_T48;if(_T49!=4)goto _TL53;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2AD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2AB;_T4A=_T2AD->f1;_T4B=_T4A.ptr_atts;_T2AC=_T4B.eff;}{void*eff=_T2AC;
# 253
void*atv=Cyc_Absyn_var_type(tv);_T4C=eff;_T4D=
Cyc_Absyn_var_type(tv);_T4E=Cyc_Unify_unify(_T4C,_T4D);if(_T4E)goto _TL55;else{goto _TL57;}
_TL57:{struct Cyc_Warn_String_Warn_Warg_struct _T2AD;_T2AD.tag=0;_T2AD.f1=_tag_fat("Mismatched effect variables in alias pattern:",sizeof(char),46U);_T4F=_T2AD;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AD=_T4F;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2AE;_T2AE.tag=2;_T2AE.f1=eff;_T50=_T2AE;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2AE=_T50;{struct Cyc_Warn_String_Warn_Warg_struct _T2AF;_T2AF.tag=0;_T2AF.f1=_tag_fat(", ",sizeof(char),3U);_T51=_T2AF;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AF=_T51;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2B0;_T2B0.tag=2;_T2B0.f1=atv;_T52=_T2B0;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2B0=_T52;void*_T2B1[4];_T53=_T2B1 + 0;*_T53=& _T2AD;_T54=_T2B1 + 1;*_T54=& _T2AE;_T55=_T2B1 + 2;*_T55=& _T2AF;_T56=_T2B1 + 3;*_T56=& _T2B0;_T57=p;_T58=_T57->loc;_T59=_tag_fat(_T2B1,sizeof(void*),4);Cyc_Warn_err2(_T58,_T59);}}}}goto _TL56;_TL55: _TL56: goto _LL2D;}_TL53: goto _LL2D;_LL2D:;}_T5A=
# 262
Cyc_Tcenv_lookup_type_vars(te);_T5B=topt;t=Cyc_Tcpat_any_type(_T5A,_T5B);{
void*_T2AB=Cyc_Absyn_compress(t);_T5C=(int*)_T2AB;_T5D=*_T5C;if(_T5D!=5)goto _TL58;
# 265
if(rgn_pat==0)goto _TL5C;else{goto _TL5D;}_TL5D: _T5E=allow_ref_pat;if(_T5E)goto _TL5A;else{goto _TL5C;}
_TL5C:{struct Cyc_Warn_String_Warn_Warg_struct _T2AC;_T2AC.tag=0;_T2AC.f1=_tag_fat("array reference would point into unknown/unallowed region",sizeof(char),58U);_T5F=_T2AC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AC=_T5F;void*_T2AD[1];_T60=_T2AD + 0;*_T60=& _T2AC;_T61=p;_T62=_T61->loc;_T63=_tag_fat(_T2AD,sizeof(void*),1);Cyc_Warn_err2(_T62,_T63);}goto _TL5B;_TL5A: _TL5B: goto _LL32;_TL58: _T64=
# 269
Cyc_Tcutil_type_kind(t);_T65=& Cyc_Kinds_mk;_T66=(struct Cyc_Absyn_Kind*)_T65;_T67=Cyc_Kinds_kind_leq(_T64,_T66);if(_T67)goto _TL5E;else{goto _TL60;}
_TL60:{struct Cyc_Warn_String_Warn_Warg_struct _T2AC;_T2AC.tag=0;_T2AC.f1=_tag_fat("pattern would point to an abstract member",sizeof(char),42U);_T68=_T2AC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AC=_T68;void*_T2AD[1];_T69=_T2AD + 0;*_T69=& _T2AC;_T6A=p;_T6B=_T6A->loc;_T6C=_tag_fat(_T2AD,sizeof(void*),1);Cyc_Warn_err2(_T6B,_T6C);}goto _TL5F;_TL5E: _TL5F: goto _LL32;_LL32:;}{struct Cyc_Absyn_Vardecl**_T2AB=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));
# 273
*_T2AB=vd;_T6D=(struct Cyc_Absyn_Vardecl**)_T2AB;}_T6E=access_exp;_T6F=& res.patvars;_T70=vd;_T71=_T70->type;Cyc_Tcpat_set_vd(_T6D,_T6E,_T6F,_T71);goto _LL0;}}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T2A3;_T2AA=_T2AB->f1;_T2A9=_T2AB->f2;}{struct Cyc_Absyn_Vardecl*vd=_T2AA;struct Cyc_Absyn_Pat*p2=_T2A9;
# 277
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);_T72=p2;_T73=_T72->topt;
t=_check_null(_T73);_T74=allow_ref_pat;
if(_T74)goto _TL64;else{goto _TL63;}_TL64: if(rgn_pat==0)goto _TL63;else{goto _TL61;}
_TL63:{struct Cyc_Warn_String_Warn_Warg_struct _T2AB;_T2AB.tag=0;_T2AB.f1=_tag_fat("* pattern would point into an unknown/unallowed region",sizeof(char),55U);_T75=_T2AB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AB=_T75;void*_T2AC[1];_T76=_T2AC + 0;*_T76=& _T2AB;_T77=p;_T78=_T77->loc;_T79=_tag_fat(_T2AC,sizeof(void*),1);Cyc_Warn_err2(_T78,_T79);}goto _LL0;
# 282
_TL61: _T7A=Cyc_Tcenv_curr_aquals_bounds(te);_T7B=t;_T7C=Cyc_Tcutil_is_noalias_pointer_or_aggr(_T7A,_T7B);if(!_T7C)goto _TL65;{struct Cyc_Warn_String_Warn_Warg_struct _T2AB;_T2AB.tag=0;
_T2AB.f1=_tag_fat("* pattern cannot take the address of an alias-free path",sizeof(char),56U);_T7D=_T2AB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AB=_T7D;void*_T2AC[1];_T7E=_T2AC + 0;*_T7E=& _T2AB;_T7F=p;_T80=_T7F->loc;_T81=_tag_fat(_T2AC,sizeof(void*),1);Cyc_Warn_err2(_T80,_T81);}goto _TL66;_TL65: _TL66: {
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.elt_type=t;
pi.ptr_atts.eff=rgn_pat;
pi.ptr_atts.nullable=Cyc_Absyn_false_type;{
void*t2=Cyc_Absyn_pointer_type(pi);_T82=access_exp;_T83=(unsigned)_T82;
if(!_T83)goto _TL67;
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);_T84=new_access_exp;
_T84->topt=t2;goto _TL68;_TL67: _TL68:{struct Cyc_Absyn_Vardecl**_T2AB=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));
# 294
*_T2AB=vd;_T85=(struct Cyc_Absyn_Vardecl**)_T2AB;}_T86=new_access_exp;_T87=& res.patvars;_T88=t2;Cyc_Tcpat_set_vd(_T85,_T86,_T87,_T88);goto _LL0;}}}case 4:{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T2A3;_T2AA=_T2AB->f1;_T2A9=_T2AB->f2;}{struct Cyc_Absyn_Tvar*tv=_T2AA;struct Cyc_Absyn_Vardecl*vd=_T2A9;{struct Cyc_Absyn_Vardecl**_T2AB=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));
# 300
*_T2AB=vd;_T89=(struct Cyc_Absyn_Vardecl**)_T2AB;}_T8A=access_exp;_T8B=& res.patvars;_T8C=vd;_T8D=_T8C->type;Cyc_Tcpat_set_vd(_T89,_T8A,_T8B,_T8D);_T8E=p;_T8F=_T8E->loc;_T90=te;{struct Cyc_Absyn_Tvar*_T2AB[1];_T92=_T2AB + 0;*_T92=tv;_T93=_tag_fat(_T2AB,sizeof(struct Cyc_Absyn_Tvar*),1);_T91=Cyc_List_list(_T93);}
# 304
Cyc_Tcenv_add_type_vars(_T8F,_T90,_T91);_T94=res;_T95=_T94.tvars_and_effconstr_opt;
if(_T95!=0)goto _TL69;{struct _tuple1*_T2AB=_cycalloc(sizeof(struct _tuple1));
_T2AB->f0=0;_T2AB->f1=0;_T96=(struct _tuple1*)_T2AB;}res.tvars_and_effconstr_opt=_T96;goto _TL6A;_TL69: _TL6A: _T97=res;_T98=_T97.tvars_and_effconstr_opt;{struct Cyc_List_List*_T2AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T2AC=_cycalloc(sizeof(struct _tuple18));
# 308
_T2AC->f0=tv;_T2AC->f1=0;_T9A=(struct _tuple18*)_T2AC;}_T2AB->hd=_T9A;_T9B=res;_T9C=_T9B.tvars_and_effconstr_opt;_T9D=*_T9C;_T2AB->tl=_T9D.f0;_T99=(struct Cyc_List_List*)_T2AB;}
# 307
(*_T98).f0=_T99;
# 309
t=Cyc_Absyn_uint_type;goto _LL0;}case 9: _T9E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T2A3;_T9F=_T9E->f1;if(_T9F!=Cyc_Absyn_Unsigned)goto _TL6B;
# 312
t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;_TL6B:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;case 10:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T2A3;_T2A8=_T2AB->f2;}{int i=_T2A8;_TA0=topt;_TA1=i;_TA2=(unsigned)_TA1;_TA3=
Cyc_Absyn_gen_float_type(_TA2);t=Cyc_Tcpat_num_type(_TA0,_TA3);goto _LL0;}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T2A3;_T2AA=_T2AB->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T2AA;_TA4=topt;_TA5=ed;_TA6=_TA5->name;_TA7=ed;_TA8=
Cyc_Absyn_enum_type(_TA6,_TA7);t=Cyc_Tcpat_num_type(_TA4,_TA8);goto _LL0;}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T2A3;_TA9=_T2AB->f1;_T2AA=(void*)_TA9;}{void*tenum=_T2AA;
t=Cyc_Tcpat_num_type(topt,tenum);goto _LL0;}case 8:
# 319
 if(topt==0)goto _TL6D;_TAA=topt;_TAB=*_TAA;{
void*_T2AB=Cyc_Absyn_compress(_TAB);_TAC=(int*)_T2AB;_TAD=*_TAC;if(_TAD!=4)goto _TL6F;_TAE=topt;
t=*_TAE;goto tcpat_end;_TL6F: goto _LL37;_LL37:;}goto _TL6E;_TL6D: _TL6E: {
# 324
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.ptr_atts.nullable=Cyc_Absyn_true_type;
t=Cyc_Absyn_pointer_type(pi);goto _LL0;}case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T2A3;_T2AA=_T2AB->f1;}{struct Cyc_Absyn_Pat*p2=_T2AA;
# 332
void*inner_typ=Cyc_Absyn_void_type;
void**inner_topt=0;
int elt_const=0;
if(topt==0)goto _TL71;_TAF=topt;_TB0=*_TAF;{
void*_T2AB=Cyc_Absyn_compress(_TB0);struct Cyc_Absyn_Tqual _T2AC;void*_T2AD;_TB1=(int*)_T2AB;_TB2=*_TB1;if(_TB2!=4)goto _TL73;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2AE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2AB;_TB3=_T2AE->f1;_T2AD=_TB3.elt_type;_TB4=_T2AE->f1;_T2AC=_TB4.elt_tq;}{void*elt_typ=_T2AD;struct Cyc_Absyn_Tqual elt_tq=_T2AC;
# 338
inner_typ=elt_typ;
inner_topt=& inner_typ;_TB5=elt_tq;
elt_const=_TB5.real_const;goto _LL3C;}_TL73: goto _LL3C;_LL3C:;}goto _TL72;_TL71: _TL72: _TB6=& Cyc_Kinds_eko;_TB7=(struct Cyc_Core_Opt*)_TB6;_TB8=
# 347
Cyc_Tcenv_lookup_opt_type_vars(te);{void*ptr_rgn=Cyc_Absyn_new_evar(_TB7,_TB8);
struct Cyc_Absyn_Exp*new_access_exp=0;_TB9=access_exp;_TBA=(unsigned)_TB9;
if(!_TBA)goto _TL75;new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);goto _TL76;_TL75: _TL76: _TBB=res;_TBC=
Cyc_Tcpat_tcPatRec(te,p2,inner_topt,ptr_rgn,1,new_access_exp);res=Cyc_Tcpat_combine_results(_TBB,_TBC);_TBD=p2;_TBE=_TBD->topt;_TBF=
# 356
_check_null(_TBE);{void*_T2AB=Cyc_Absyn_compress(_TBF);struct Cyc_List_List*_T2AC;struct Cyc_Absyn_Datatypefield*_T2AD;struct Cyc_Absyn_Datatypedecl*_T2AE;_TC0=(int*)_T2AB;_TC1=*_TC0;if(_TC1!=0)goto _TL77;_TC2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2AB;_TC3=_TC2->f1;_TC4=(int*)_TC3;_TC5=*_TC4;if(_TC5!=23)goto _TL79;_TC6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2AB;_TC7=_TC6->f1;_TC8=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TC7;_TC9=_TC8->f1;_TCA=_TC9.KnownDatatypefield;_TCB=_TCA.tag;if(_TCB!=2)goto _TL7B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2AF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2AB;_TCC=_T2AF->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T2B0=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TCC;_TCD=_T2B0->f1;_TCE=_TCD.KnownDatatypefield;_TCF=_TCE.val;_T2AE=_TCF.f0;_TD0=_T2B0->f1;_TD1=_TD0.KnownDatatypefield;_TD2=_TD1.val;_T2AD=_TD2.f1;}_T2AC=_T2AF->f2;}{struct Cyc_Absyn_Datatypedecl*tud=_T2AE;struct Cyc_Absyn_Datatypefield*tuf=_T2AD;struct Cyc_List_List*targs=_T2AC;{
# 360
void*_T2AF=Cyc_Absyn_compress(inner_typ);_TD3=(int*)_T2AF;_TD4=*_TD3;if(_TD4!=0)goto _TL7D;_TD5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2AF;_TD6=_TD5->f1;_TD7=(int*)_TD6;_TD8=*_TD7;if(_TD8!=23)goto _TL7F;goto DONT_PROMOTE;_TL7F: goto _LL49;_TL7D: _LL49: goto _LL46;_LL46:;}{struct Cyc_Absyn_Datatypedecl**_T2AF=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));
# 366
*_T2AF=tud;_TD9=(struct Cyc_Absyn_Datatypedecl**)_T2AF;}_TDA=Cyc_Absyn_KnownDatatype(_TD9);_TDB=targs;{void*new_type=Cyc_Absyn_datatype_type(_TDA,_TDB);_TDC=p2;
# 368
_TDC->topt=new_type;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2AF=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct));_T2AF->tag=4;
_T2AF->f1.elt_type=new_type;_TDE=elt_const;
if(!_TDE)goto _TL81;_T2AF->f1.elt_tq=Cyc_Absyn_const_tqual(0U);goto _TL82;_TL81:
 _T2AF->f1.elt_tq=Cyc_Absyn_empty_tqual(0U);_TL82:
 _T2AF->f1.ptr_atts.eff=ptr_rgn;_TDF=Cyc_Tcenv_lookup_type_vars(te);_T2AF->f1.ptr_atts.nullable=Cyc_Tcutil_any_bool(_TDF);
_T2AF->f1.ptr_atts.bounds=Cyc_Absyn_bounds_one();_T2AF->f1.ptr_atts.zero_term=Cyc_Absyn_false_type;
_T2AF->f1.ptr_atts.ptrloc=0;_TE0=Cyc_Tcenv_lookup_type_vars(te);_T2AF->f1.ptr_atts.autoreleased=Cyc_Tcutil_any_bool(_TE0);_TE1=& Cyc_Kinds_aqko;_TE2=(struct Cyc_Core_Opt*)_TE1;_TE3=
Cyc_Tcenv_lookup_opt_type_vars(te);_T2AF->f1.ptr_atts.aqual=Cyc_Absyn_new_evar(_TE2,_TE3);_TDD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T2AF;}
# 369
t=(void*)_TDD;goto _LL41;}}_TL7B: goto _LL44;_TL79: goto _LL44;_TL77: _LL44:
# 378
 DONT_PROMOTE:{
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);_TE4=p2;_TE5=_TE4->topt;
pi.elt_type=_check_null(_TE5);_TE7=elt_const;
if(!_TE7)goto _TL83;_TE6=Cyc_Absyn_const_tqual(0U);goto _TL84;_TL83: _TE6=Cyc_Absyn_empty_tqual(0U);_TL84: pi.elt_tq=_TE6;
pi.ptr_atts.eff=ptr_rgn;
t=Cyc_Absyn_pointer_type(pi);}_LL41:;}_TE8=new_access_exp;_TE9=(unsigned)_TE8;
# 385
if(!_TE9)goto _TL85;_TEA=new_access_exp;_TEB=p2;_TEA->topt=_TEB->topt;goto _TL86;_TL85: _TL86: goto _LL0;}}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T2A3;_TEC=_T2AB->f1;_T2AA=(void*)_TEC;_T2A8=_T2AB->f2;_T2A7=_T2AB->f3;_TED=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T2A3;_TEE=& _TED->f4;_T2A9=(struct Cyc_List_List**)_TEE;_T2A6=_T2AB->f5;}{void*atype=_T2AA;int is_tuple=_T2A8;struct Cyc_List_List*exist_ts=_T2A7;struct Cyc_List_List**dps_p=(struct Cyc_List_List**)_T2A9;int dots=_T2A6;_TEF=dps_p;{
# 389
struct Cyc_List_List*dps=*_TEF;
if(atype!=0)goto _TL87;_TF0=
# 392
Cyc_Tcenv_lookup_opt_type_vars(te);t=Cyc_Absyn_wildtyp(_TF0);goto _LL0;_TL87:
# 395
 t=atype;{
void*_T2AB=Cyc_Absyn_compress(t);struct Cyc_List_List*_T2AC;int _T2AD;enum Cyc_Absyn_AggrKind _T2AE;struct Cyc_Absyn_Aggrdecl*_T2AF;union Cyc_Absyn_AggrInfo _T2B0;_TF1=(int*)_T2AB;_TF2=*_TF1;switch(_TF2){case 0: _TF3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2AB;_TF4=_TF3->f1;_TF5=(int*)_TF4;_TF6=*_TF5;if(_TF6!=24)goto _TL8A;_TF7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2AB;_TF8=_TF7->f1;_TF9=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TF8;_TFA=_TF9->f1;_TFB=_TFA.KnownAggr;_TFC=_TFB.tag;if(_TFC!=2)goto _TL8C;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2B1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2AB;_TFD=_T2B1->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T2B2=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TFD;_T2B0=_T2B2->f1;_TFE=_T2B0.KnownAggr;_TFF=_TFE.val;{struct Cyc_Absyn_Aggrdecl*_T2B3=*_TFF;_T2AF=_T2B3;}}}{union Cyc_Absyn_AggrInfo ainfo=_T2B0;struct Cyc_Absyn_Aggrdecl*ad=_T2AF;_T100=ad;_T101=_T100->impl;
# 398
if(_T101!=0)goto _TL8E;{struct Cyc_Warn_String_Warn_Warg_struct _T2B1;_T2B1.tag=0;
_T2B1.f1=_tag_fat("can't destructure abstract ",sizeof(char),28U);_T102=_T2B1;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B1=_T102;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T2B2;_T2B2.tag=6;_T2B2.f1=ad;_T103=_T2B2;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T2B2=_T103;void*_T2B3[2];_T104=_T2B3 + 0;*_T104=& _T2B1;_T105=_T2B3 + 1;*_T105=& _T2B2;_T106=p;_T107=_T106->loc;_T108=_tag_fat(_T2B3,sizeof(void*),2);Cyc_Warn_err2(_T107,_T108);}}_T109=
Cyc_Tcenv_lookup_opt_type_vars(te);t=Cyc_Absyn_wildtyp(_T109);goto _LL4B;_TL8E: _T10A=ad;_T10B=_T10A->kind;_T10C=(int)_T10B;
# 405
if(_T10C!=1)goto _TL90;_T10D=ad;_T10E=_T10D->impl;_T10F=_T10E->tagged;if(!_T10F)goto _TL90;
allow_ref_pat=0;goto _TL91;_TL90: _TL91:
 if(exist_ts==0)goto _TL92;
# 411
if(topt==0)goto _TL96;else{goto _TL97;}_TL97: _T110=topt;_T111=*_T110;_T112=Cyc_Tcutil_type_kind(_T111);_T113=& Cyc_Kinds_ak;_T114=(struct Cyc_Absyn_Kind*)_T113;if(_T112!=_T114)goto _TL96;else{goto _TL94;}
_TL96: allow_ref_pat=0;goto _TL95;_TL94: _TL95: goto _TL93;_TL92: _TL93:{struct _RegionHandle _T2B1=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _T2B1;_push_region(rgn);{
# 416
struct Cyc_List_List*var_tvs=0;_T115=ad;_T116=_T115->impl;_T117=
_check_null(_T116);{struct Cyc_List_List*u=_T117->exist_vars;{
struct Cyc_List_List*t=exist_ts;_TL9B: if(t!=0)goto _TL99;else{goto _TL9A;}
_TL99: _T118=t;_T119=_T118->hd;{struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_T119;_T11A=
_check_null(u);_T11B=_T11A->hd;{struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)_T11B;_T11C=u;
u=_T11C->tl;_T11D=tv;_T11E=_T11D->kind;{
void*kb1=Cyc_Kinds_compress_kb(_T11E);_T11F=uv;_T120=_T11F->kind;{
void*kb2=Cyc_Kinds_compress_kb(_T120);
struct Cyc_Absyn_Kind*k2;{struct Cyc_Absyn_Kind*_T2B2;_T121=kb2;_T122=(int*)_T121;_T123=*_T122;switch(_T123){case 2: _T124=kb2;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2B3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T124;_T2B2=_T2B3->f2;}{struct Cyc_Absyn_Kind*k=_T2B2;_T2B2=k;goto _LL56;}case 0: _T125=kb2;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T2B3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T125;_T2B2=_T2B3->f1;}_LL56: {struct Cyc_Absyn_Kind*k=_T2B2;
# 427
k2=k;goto _LL52;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T2B3;_T2B3.tag=0;
_T2B3.f1=_tag_fat("unconstrained existential tyvar in aggregate",sizeof(char),45U);_T126=_T2B3;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B3=_T126;void*_T2B4[1];_T127=_T2B4 + 0;*_T127=& _T2B3;_T129=Cyc_Warn_impos2;{int(*_T2B5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T129;_T128=_T2B5;}_T12A=_tag_fat(_T2B4,sizeof(void*),1);_T128(_T12A);}}_LL52:;}{struct Cyc_Absyn_Kind*_T2B2;void*_T2B3;_T12B=kb1;_T12C=(int*)_T12B;_T12D=*_T12C;switch(_T12D){case 0: _T12E=kb1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T2B4=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T12E;_T2B3=_T2B4->f1;}{struct Cyc_Absyn_Kind*k1=_T2B3;_T12F=
# 433
Cyc_Kinds_kind_leq(k2,k1);if(_T12F)goto _TL9E;else{goto _TLA0;}
_TLA0:{struct Cyc_Warn_String_Warn_Warg_struct _T2B4;_T2B4.tag=0;_T2B4.f1=_tag_fat("type variable ",sizeof(char),15U);_T130=_T2B4;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B4=_T130;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T2B5;_T2B5.tag=7;_T2B5.f1=tv;_T131=_T2B5;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T2B5=_T131;{struct Cyc_Warn_String_Warn_Warg_struct _T2B6;_T2B6.tag=0;_T2B6.f1=_tag_fat(" has kind ",sizeof(char),11U);_T132=_T2B6;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B6=_T132;{struct Cyc_Warn_KindBound_Warn_Warg_struct _T2B7;_T2B7.tag=8;_T2B7.f1=kb1;_T133=_T2B7;}{struct Cyc_Warn_KindBound_Warn_Warg_struct _T2B7=_T133;{struct Cyc_Warn_String_Warn_Warg_struct _T2B8;_T2B8.tag=0;
_T2B8.f1=_tag_fat(" but must have at least kind ",sizeof(char),30U);_T134=_T2B8;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B8=_T134;{struct Cyc_Warn_Kind_Warn_Warg_struct _T2B9;_T2B9.tag=9;_T2B9.f1=k2;_T135=_T2B9;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T2B9=_T135;void*_T2BA[6];_T136=_T2BA + 0;*_T136=& _T2B4;_T137=_T2BA + 1;*_T137=& _T2B5;_T138=_T2BA + 2;*_T138=& _T2B6;_T139=_T2BA + 3;*_T139=& _T2B7;_T13A=_T2BA + 4;*_T13A=& _T2B8;_T13B=_T2BA + 5;*_T13B=& _T2B9;_T13C=p;_T13D=_T13C->loc;_T13E=_tag_fat(_T2BA,sizeof(void*),6);Cyc_Warn_err2(_T13D,_T13E);}}}}}}goto _TL9F;_TL9E: _TL9F: goto _LL59;}case 2: _T13F=kb1;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2B4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T13F;_T140=kb1;_T141=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T140;_T142=& _T141->f1;_T2B3=(struct Cyc_Core_Opt**)_T142;_T2B2=_T2B4->f2;}{struct Cyc_Core_Opt**f=_T2B3;struct Cyc_Absyn_Kind*k=_T2B2;_T2B3=f;goto _LL5F;}default: _T143=kb1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T2B4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T143;_T144=kb1;_T145=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T144;_T146=& _T145->f1;_T2B3=(struct Cyc_Core_Opt**)_T146;}_LL5F: {struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T2B3;_T147=f;{struct Cyc_Core_Opt*_T2B4=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 438
_T2B4->v=kb2;_T148=(struct Cyc_Core_Opt*)_T2B4;}*_T147=_T148;goto _LL59;}}_LL59:;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T2B2=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T2B2->tag=2;
# 440
_T2B2->f1=tv;_T149=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T2B2;}{void*vartype=(void*)_T149;_T14B=rgn;{struct Cyc_List_List*_T2B2=_region_malloc(_T14B,0U,sizeof(struct Cyc_List_List));
_T2B2->hd=vartype;_T2B2->tl=var_tvs;_T14A=(struct Cyc_List_List*)_T2B2;}var_tvs=_T14A;}}}}}_T14C=t;
# 418
t=_T14C->tl;goto _TL9B;_TL9A:;}
# 454 "tcpat.cyc"
var_tvs=Cyc_List_imp_rev(var_tvs);_T14D=p;_T14E=_T14D->loc;_T14F=te;_T150=exist_ts;{
# 456
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_T14E,_T14F,_T150);{struct _tuple13 _T2B2;
# 458
_T2B2.f0=Cyc_Tcenv_lookup_type_vars(te2);_T2B2.f1=rgn;_T151=_T2B2;}{struct _tuple13 env=_T151;_T153=Cyc_List_rmap_c;{
struct Cyc_List_List*(*_T2B2)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))_T153;_T152=_T2B2;}_T154=rgn;_T155=Cyc_Tcutil_r_make_inst_var;_T156=& env;_T157=ad;_T158=_T157->tvs;{struct Cyc_List_List*all_inst=_T152(_T154,_T155,_T156,_T158);_T159=rgn;_T15A=rgn;_T15B=ad;_T15C=_T15B->impl;_T15D=
_check_null(_T15C);_T15E=_T15D->exist_vars;_T15F=var_tvs;{struct Cyc_List_List*exist_inst=Cyc_List_rzip(_T159,_T15A,_T15E,_T15F);_T161=Cyc_List_map;{
struct Cyc_List_List*(*_T2B2)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T161;_T160=_T2B2;}_T163=Cyc_Core_snd;{void*(*_T2B2)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T163;_T162=_T2B2;}_T164=all_inst;{struct Cyc_List_List*all_typs=_T160(_T162,_T164);_T166=Cyc_List_map;{
struct Cyc_List_List*(*_T2B2)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T166;_T165=_T2B2;}_T168=Cyc_Core_snd;{void*(*_T2B2)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T168;_T167=_T2B2;}_T169=exist_inst;{struct Cyc_List_List*exist_typs=_T165(_T167,_T169);
struct Cyc_List_List*inst=Cyc_List_rappend(rgn,all_inst,exist_inst);
# 465
if(exist_ts!=0)goto _TLA3;else{goto _TLA4;}_TLA4: _T16A=ad;_T16B=_T16A->impl;_T16C=_check_null(_T16B);_T16D=_T16C->effconstr;if(_T16D!=0)goto _TLA3;else{goto _TLA1;}
_TLA3: _T16E=res;_T16F=_T16E.tvars_and_effconstr_opt;if(_T16F!=0)goto _TLA5;{struct _tuple1*_T2B2=_cycalloc(sizeof(struct _tuple1));
_T2B2->f0=0;_T2B2->f1=0;_T170=(struct _tuple1*)_T2B2;}res.tvars_and_effconstr_opt=_T170;goto _TLA6;_TLA5: _TLA6: _T171=res;_T172=_T171.tvars_and_effconstr_opt;_T173=res;_T174=_T173.tvars_and_effconstr_opt;_T175=*_T174;_T176=_T175.f0;_T178=Cyc_List_map;{
# 469
struct Cyc_List_List*(*_T2B2)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))_T178;_T177=_T2B2;}_T179=exist_ts;_T17A=_T177(Cyc_Tcpat_add_false,_T179);
# 468
(*_T172).f0=
Cyc_List_append(_T176,_T17A);_T17B=rgn;_T17C=inst;_T17D=ad;_T17E=_T17D->impl;_T17F=
# 472
_check_null(_T17E);_T180=_T17F->effconstr;{struct Cyc_List_List*ec_inst=Cyc_Tcutil_rsubst_effconstr(_T17B,_T17C,_T180);_T181=res;_T182=_T181.tvars_and_effconstr_opt;_T183=res;_T184=_T183.tvars_and_effconstr_opt;_T185=*_T184;_T186=_T185.f1;_T187=ec_inst;
# 474
(*_T182).f1=
Cyc_List_append(_T186,_T187);{
struct Cyc_List_List*aqb_inst=0;_T188=ad;_T189=_T188->impl;_T18A=
_check_null(_T189);{struct Cyc_List_List*aqb=_T18A->qual_bnd;_TLAA: if(aqb!=0)goto _TLA8;else{goto _TLA9;}
_TLA8:{struct _tuple0 _T2B2;_T18C=rgn;_T18D=inst;_T18E=aqb;_T18F=_T18E->hd;_T190=(struct _tuple0*)_T18F;_T191=*_T190;_T192=_T191.f0;_T2B2.f0=Cyc_Tcutil_rsubstitute(_T18C,_T18D,_T192);_T193=rgn;_T194=inst;_T195=aqb;_T196=_T195->hd;_T197=(struct _tuple0*)_T196;_T198=*_T197;_T199=_T198.f1;
_T2B2.f1=Cyc_Tcutil_rsubstitute(_T193,_T194,_T199);_T18B=_T2B2;}{
# 478
struct _tuple0 _T2B2=_T18B;void*_T2B3;void*_T2B4;_T2B4=_T2B2.f0;_T2B3=_T2B2.f1;{void*tv=_T2B4;void*b=_T2B3;{struct Cyc_List_List*_T2B5=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T2B6=_cycalloc(sizeof(struct _tuple0));
# 480
_T2B6->f0=tv;_T2B6->f1=b;_T19B=(struct _tuple0*)_T2B6;}_T2B5->hd=_T19B;_T2B5->tl=aqb_inst;_T19A=(struct Cyc_List_List*)_T2B5;}aqb_inst=_T19A;}}_T19C=aqb;
# 477
aqb=_T19C->tl;goto _TLAA;_TLA9:;}
# 482
aqb_inst=Cyc_List_imp_rev(aqb_inst);
res.aquals_bounds=aqb_inst;}}goto _TLA2;_TLA1: _TLA2:{struct Cyc_Absyn_Aggrdecl**_T2B2=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 487
*_T2B2=ad;_T19D=(struct Cyc_Absyn_Aggrdecl**)_T2B2;}_T19E=Cyc_Absyn_KnownAggr(_T19D);_T19F=all_typs;t=Cyc_Absyn_aggr_type(_T19E,_T19F);_T1A0=dots;
if(!_T1A0)goto _TLAB;_T1A1=ad;_T1A2=_T1A1->kind;_T1A3=(int)_T1A2;if(_T1A3!=1)goto _TLAB;_T1A4=p;_T1A5=_T1A4->loc;_T1A6=
_tag_fat("`...' pattern not allowed in union pattern",sizeof(char),43U);_T1A7=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T1A5,_T1A6,_T1A7);goto _TLAC;
_TLAB: _T1A8=dots;if(!_T1A8)goto _TLAD;_T1A9=dps_p;{
struct Cyc_List_List*(*_T2B2)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_T1AA=_T2B2;}_T1AB=dps;_T1AC=Cyc_List_length(dps);_T1AD=ad;_T1AE=_T1AD->impl;_T1AF=_check_null(_T1AE);_T1B0=_T1AF->fields;_T1B1=Cyc_List_length(_T1B0);_T1B2=p;_T1B3=_T1B2->loc;_T1B4=
_tag_fat("struct",sizeof(char),7U);
# 491
*_T1A9=_T1AA(_T1AB,_T1AC,_T1B1,Cyc_Tcpat_wild_dlp,_T1B3,_T1B4);_T1B5=dps_p;
# 493
dps=*_T1B5;goto _TLAE;_TLAD: _TLAE: _TLAC: _T1B6=rgn;_T1B7=p;_T1B8=_T1B7->loc;_T1B9=dps;_T1BA=ad;_T1BB=_T1BA->kind;_T1BC=ad;_T1BD=_T1BC->impl;_T1BE=
# 496
_check_null(_T1BD);_T1BF=_T1BE->fields;{
# 495
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(_T1B6,_T1B8,_T1B9,_T1BB,_T1BF);
_TLB2: if(fields!=0)goto _TLB0;else{goto _TLB1;}
_TLB0: _T1C0=fields;_T1C1=_T1C0->hd;{struct _tuple19*_T2B2=(struct _tuple19*)_T1C1;struct Cyc_Absyn_Pat*_T2B3;struct Cyc_Absyn_Aggrfield*_T2B4;{struct _tuple19 _T2B5=*_T2B2;_T2B4=_T2B5.f0;_T2B3=_T2B5.f1;}{struct Cyc_Absyn_Aggrfield*field=_T2B4;struct Cyc_Absyn_Pat*pat=_T2B3;_T1C2=rgn;_T1C3=inst;_T1C4=field;_T1C5=_T1C4->type;{
void*inst_fieldtyp=Cyc_Tcutil_rsubstitute(_T1C2,_T1C3,_T1C5);
# 501
struct Cyc_Absyn_Exp*new_access_exp=0;_T1C6=access_exp;_T1C7=(unsigned)_T1C6;
if(!_T1C7)goto _TLB3;_T1C8=access_exp;_T1C9=field;_T1CA=_T1C9->name;
new_access_exp=Cyc_Absyn_aggrmember_exp(_T1C8,_T1CA,0U);goto _TLB4;_TLB3: _TLB4: _T1CB=res;_T1CC=te2;_T1CD=pat;_T1CE=& inst_fieldtyp;_T1CF=rgn_pat;_T1D0=allow_ref_pat;_T1D1=new_access_exp;_T1D2=
Cyc_Tcpat_tcPatRec(_T1CC,_T1CD,_T1CE,_T1CF,_T1D0,_T1D1);res=Cyc_Tcpat_combine_results(_T1CB,_T1D2);_T1D3=p;_T1D4=_T1D3->loc;_T1D5=pat;_T1D6=_T1D5->topt;_T1D7=
# 509
_check_null(_T1D6);_T1D8=inst_fieldtyp;_T1D9=Cyc_Unify_unify_and_constrain(_T1D4,_T1D7,_T1D8);if(_T1D9)goto _TLB5;else{goto _TLB7;}
_TLB7:{struct Cyc_Warn_String_Warn_Warg_struct _T2B5;_T2B5.tag=0;_T2B5.f1=_tag_fat("field ",sizeof(char),7U);_T1DA=_T2B5;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B5=_T1DA;{struct Cyc_Warn_String_Warn_Warg_struct _T2B6;_T2B6.tag=0;_T1DC=field;_T1DD=_T1DC->name;_T2B6.f1=*_T1DD;_T1DB=_T2B6;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B6=_T1DB;{struct Cyc_Warn_String_Warn_Warg_struct _T2B7;_T2B7.tag=0;_T2B7.f1=_tag_fat(" of ",sizeof(char),5U);_T1DE=_T2B7;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B7=_T1DE;{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T2B8;_T2B8.tag=6;_T2B8.f1=ad;_T1DF=_T2B8;}{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _T2B8=_T1DF;{struct Cyc_Warn_String_Warn_Warg_struct _T2B9;_T2B9.tag=0;
_T2B9.f1=_tag_fat(" pattern expects type ",sizeof(char),23U);_T1E0=_T2B9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B9=_T1E0;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2BA;_T2BA.tag=2;_T2BA.f1=inst_fieldtyp;_T1E1=_T2BA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2BA=_T1E1;{struct Cyc_Warn_String_Warn_Warg_struct _T2BB;_T2BB.tag=0;_T2BB.f1=_tag_fat(" != ",sizeof(char),5U);_T1E2=_T2BB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2BB=_T1E2;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2BC;_T2BC.tag=2;_T1E4=pat;_T1E5=_T1E4->topt;
_T2BC.f1=_check_null(_T1E5);_T1E3=_T2BC;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2BC=_T1E3;void*_T2BD[8];_T1E6=_T2BD + 0;*_T1E6=& _T2B5;_T1E7=_T2BD + 1;*_T1E7=& _T2B6;_T1E8=_T2BD + 2;*_T1E8=& _T2B7;_T1E9=_T2BD + 3;*_T1E9=& _T2B8;_T1EA=_T2BD + 4;*_T1EA=& _T2B9;_T1EB=_T2BD + 5;*_T1EB=& _T2BA;_T1EC=_T2BD + 6;*_T1EC=& _T2BB;_T1ED=_T2BD + 7;*_T1ED=& _T2BC;_T1EE=p;_T1EF=_T1EE->loc;_T1F0=_tag_fat(_T2BD,sizeof(void*),8);Cyc_Warn_err2(_T1EF,_T1F0);}}}}}}}}goto _TLB6;_TLB5: _TLB6: _T1F1=new_access_exp;_T1F2=(unsigned)_T1F1;
if(!_T1F2)goto _TLB8;_T1F3=new_access_exp;_T1F4=pat;_T1F3->topt=_T1F4->topt;goto _TLB9;_TLB8: _TLB9:;}}}_T1F5=fields;
# 497
fields=_T1F5->tl;goto _TLB2;_TLB1:;}}}}}}}}}_pop_region();}goto _LL4B;}_TL8C: goto _LL50;_TL8A: goto _LL50;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T2B1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T2AB;_T2AE=_T2B1->f1;_T2AD=_T2B1->f2;_T2AC=_T2B1->f3;}{enum Cyc_Absyn_AggrKind ak=_T2AE;int is_tuple=_T2AD;struct Cyc_List_List*afs=_T2AC;
# 519
if(exist_ts==0)goto _TLBA;{struct Cyc_Warn_String_Warn_Warg_struct _T2B1;_T2B1.tag=0;
_T2B1.f1=_tag_fat("bad existentially-bound type variables",sizeof(char),39U);_T1F6=_T2B1;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B1=_T1F6;void*_T2B2[1];_T1F7=_T2B2 + 0;*_T1F7=& _T2B1;_T1F8=p;_T1F9=_T1F8->loc;_T1FA=_tag_fat(_T2B2,sizeof(void*),1);Cyc_Warn_err2(_T1F9,_T1FA);}goto _TLBB;_TLBA: _TLBB: _T1FB=dots;
if(!_T1FB)goto _TLBC;_T1FC=ak;_T1FD=(int)_T1FC;if(_T1FD!=1)goto _TLBC;_T1FE=p;_T1FF=_T1FE->loc;_T200=
_tag_fat("`...' patterns not allowed in union pattern",sizeof(char),44U);_T201=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T1FF,_T200,_T201);goto _TLBD;
_TLBC: _T202=dots;if(!_T202)goto _TLBE;_T203=dps_p;{
struct Cyc_List_List*(*_T2B1)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_T204=_T2B1;}_T205=dps;_T206=Cyc_List_length(dps);_T207=Cyc_List_length(afs);_T208=p;_T209=_T208->loc;_T20B=is_tuple;
if(!_T20B)goto _TLC0;_T20A=_tag_fat("tuple",sizeof(char),6U);goto _TLC1;_TLC0: _T20A=_tag_fat("struct",sizeof(char),7U);_TLC1:
# 524
*_T203=_T204(_T205,_T206,_T207,Cyc_Tcpat_wild_dlp,_T209,_T20A);_T20C=dps_p;
# 526
dps=*_T20C;goto _TLBF;_TLBE: _TLBF: _TLBD:{struct _RegionHandle _T2B1=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _T2B1;_push_region(rgn);_T20D=rgn;_T20E=p;_T20F=_T20E->loc;_T210=dps;_T211=ak;_T212=afs;{
# 529
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(_T20D,_T20F,_T210,_T211,_T212);
_TLC5: if(fields!=0)goto _TLC3;else{goto _TLC4;}
_TLC3: _T213=fields;_T214=_T213->hd;{struct _tuple19*_T2B2=(struct _tuple19*)_T214;struct Cyc_Absyn_Pat*_T2B3;struct Cyc_Absyn_Aggrfield*_T2B4;{struct _tuple19 _T2B5=*_T2B2;_T2B4=_T2B5.f0;_T2B3=_T2B5.f1;}{struct Cyc_Absyn_Aggrfield*field=_T2B4;struct Cyc_Absyn_Pat*pat=_T2B3;_T215=field;{
void*fieldtyp=_T215->type;
struct Cyc_Absyn_Exp*new_access_exp=0;_T216=access_exp;_T217=(unsigned)_T216;
if(!_T217)goto _TLC6;_T218=access_exp;_T219=field;_T21A=_T219->name;
new_access_exp=Cyc_Absyn_aggrmember_exp(_T218,_T21A,0U);goto _TLC7;_TLC6: _TLC7: _T21B=res;_T21C=te;_T21D=pat;_T21E=& fieldtyp;_T21F=rgn_pat;_T220=allow_ref_pat;_T221=new_access_exp;_T222=
Cyc_Tcpat_tcPatRec(_T21C,_T21D,_T21E,_T21F,_T220,_T221);res=Cyc_Tcpat_combine_results(_T21B,_T222);_T223=p;_T224=_T223->loc;_T225=pat;_T226=_T225->topt;_T227=
# 542
_check_null(_T226);_T228=fieldtyp;_T229=Cyc_Unify_unify_and_constrain(_T224,_T227,_T228);if(_T229)goto _TLC8;else{goto _TLCA;}
_TLCA:{struct Cyc_Warn_String_Warn_Warg_struct _T2B5;_T2B5.tag=0;_T2B5.f1=_tag_fat("field ",sizeof(char),7U);_T22A=_T2B5;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B5=_T22A;{struct Cyc_Warn_String_Warn_Warg_struct _T2B6;_T2B6.tag=0;_T22C=field;_T22D=_T22C->name;_T2B6.f1=*_T22D;_T22B=_T2B6;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B6=_T22B;{struct Cyc_Warn_String_Warn_Warg_struct _T2B7;_T2B7.tag=0;
_T2B7.f1=_tag_fat(" pattern expects type ",sizeof(char),23U);_T22E=_T2B7;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B7=_T22E;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2B8;_T2B8.tag=2;_T2B8.f1=fieldtyp;_T22F=_T2B8;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2B8=_T22F;{struct Cyc_Warn_String_Warn_Warg_struct _T2B9;_T2B9.tag=0;_T2B9.f1=_tag_fat(" != ",sizeof(char),5U);_T230=_T2B9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B9=_T230;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2BA;_T2BA.tag=2;_T232=pat;_T233=_T232->topt;
_T2BA.f1=_check_null(_T233);_T231=_T2BA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2BA=_T231;void*_T2BB[6];_T234=_T2BB + 0;*_T234=& _T2B5;_T235=_T2BB + 1;*_T235=& _T2B6;_T236=_T2BB + 2;*_T236=& _T2B7;_T237=_T2BB + 3;*_T237=& _T2B8;_T238=_T2BB + 4;*_T238=& _T2B9;_T239=_T2BB + 5;*_T239=& _T2BA;_T23A=p;_T23B=_T23A->loc;_T23C=_tag_fat(_T2BB,sizeof(void*),6);Cyc_Warn_err2(_T23B,_T23C);}}}}}}goto _TLC9;_TLC8: _TLC9: _T23D=new_access_exp;_T23E=(unsigned)_T23D;
if(!_T23E)goto _TLCB;_T23F=new_access_exp;_T240=pat;_T23F->topt=_T240->topt;goto _TLCC;_TLCB: _TLCC:;}}}_T241=fields;
# 531
fields=_T241->tl;goto _TLC5;_TLC4:;}_pop_region();}goto _LL4B;}default: _LL50:{struct Cyc_Warn_String_Warn_Warg_struct _T2B1;_T2B1.tag=0;
# 550
_T2B1.f1=_tag_fat("bad type in tcpat",sizeof(char),18U);_T242=_T2B1;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B1=_T242;void*_T2B2[1];_T243=_T2B2 + 0;*_T243=& _T2B1;_T245=Cyc_Warn_impos2;{int(*_T2B3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T245;_T244=_T2B3;}_T246=_tag_fat(_T2B2,sizeof(void*),1);_T244(_T246);}}_LL4B:;}goto _LL0;}}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T2AB=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T2A3;_T2A5=_T2AB->f1;_T2A4=_T2AB->f2;_T247=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T2A3;_T248=& _T247->f3;_T2AA=(struct Cyc_List_List**)_T248;_T2A8=_T2AB->f4;}{struct Cyc_Absyn_Datatypedecl*tud=_T2A5;struct Cyc_Absyn_Datatypefield*tuf=_T2A4;struct Cyc_List_List**ps_p=(struct Cyc_List_List**)_T2AA;int dots=_T2A8;_T249=ps_p;{
# 555
struct Cyc_List_List*ps=*_T249;_T24A=tuf;{
struct Cyc_List_List*tqts=_T24A->typs;{struct _tuple13 _T2AB;
# 558
_T2AB.f0=Cyc_Tcenv_lookup_type_vars(te);_T2AB.f1=Cyc_Core_heap_region;_T24B=_T2AB;}{struct _tuple13 env=_T24B;_T24D=Cyc_List_map_c;{
struct Cyc_List_List*(*_T2AB)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))_T24D;_T24C=_T2AB;}_T24E=Cyc_Tcutil_r_make_inst_var;_T24F=& env;_T250=tud;_T251=_T250->tvs;{struct Cyc_List_List*inst=_T24C(_T24E,_T24F,_T251);_T253=Cyc_List_map;{
struct Cyc_List_List*(*_T2AB)(void*(*)(struct _tuple14*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))_T253;_T252=_T2AB;}_T255=Cyc_Core_snd;{void*(*_T2AB)(struct _tuple14*)=(void*(*)(struct _tuple14*))_T255;_T254=_T2AB;}_T256=inst;{struct Cyc_List_List*all_typs=_T252(_T254,_T256);_T257=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_T258=all_typs;t=Cyc_Absyn_datatype_field_type(_T257,_T258);_T259=dots;
if(!_T259)goto _TLCD;_T25A=ps_p;{
struct Cyc_List_List*(*_T2AB)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr)=(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;_T25B=_T2AB;}_T25C=ps;_T25D=Cyc_List_length(ps);_T25E=Cyc_List_length(tqts);_T25F=p;_T260=_T25F->loc;_T261=
_tag_fat("datatype field",sizeof(char),15U);
# 563
*_T25A=_T25B(_T25C,_T25D,_T25E,Cyc_Tcpat_wild_pat,_T260,_T261);_T262=ps_p;
# 565
ps=*_T262;goto _TLCE;_TLCD: _TLCE:
# 567
 _TLD2: if(ps!=0)goto _TLD3;else{goto _TLD1;}_TLD3: if(tqts!=0)goto _TLD0;else{goto _TLD1;}
_TLD0: _T263=ps;_T264=_T263->hd;{struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)_T264;_T265=inst;_T266=tqts;_T267=_T266->hd;_T268=(struct _tuple20*)_T267;_T269=*_T268;_T26A=_T269.f1;{
# 571
void*field_typ=Cyc_Tcutil_substitute(_T265,_T26A);_T26B=access_exp;_T26C=(unsigned)_T26B;
# 574
if(!_T26C)goto _TLD4;_T26D=access_exp;_T26E=& res.patvars;_T26F=Cyc_Absyn_char_type;
Cyc_Tcpat_set_vd(0,_T26D,_T26E,_T26F);goto _TLD5;_TLD4: _TLD5: _T270=res;_T271=te;_T272=p2;_T273=& field_typ;_T274=rgn_pat;_T275=allow_ref_pat;_T276=
Cyc_Tcpat_tcPatRec(_T271,_T272,_T273,_T274,_T275,0);res=Cyc_Tcpat_combine_results(_T270,_T276);_T277=p2;_T278=_T277->loc;_T279=p2;_T27A=_T279->topt;_T27B=
# 581
_check_null(_T27A);_T27C=field_typ;_T27D=Cyc_Unify_unify_and_constrain(_T278,_T27B,_T27C);if(_T27D)goto _TLD6;else{goto _TLD8;}
_TLD8:{struct Cyc_Warn_Qvar_Warn_Warg_struct _T2AB;_T2AB.tag=1;_T27F=tuf;_T2AB.f1=_T27F->name;_T27E=_T2AB;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T2AB=_T27E;{struct Cyc_Warn_String_Warn_Warg_struct _T2AC;_T2AC.tag=0;_T2AC.f1=_tag_fat(" expects argument type ",sizeof(char),24U);_T280=_T2AC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AC=_T280;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2AD;_T2AD.tag=2;_T2AD.f1=field_typ;_T281=_T2AD;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2AD=_T281;{struct Cyc_Warn_String_Warn_Warg_struct _T2AE;_T2AE.tag=0;
_T2AE.f1=_tag_fat(" not ",sizeof(char),6U);_T282=_T2AE;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AE=_T282;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2AF;_T2AF.tag=2;_T284=p2;_T285=_T284->topt;_T2AF.f1=_check_null(_T285);_T283=_T2AF;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2AF=_T283;void*_T2B0[5];_T286=_T2B0 + 0;*_T286=& _T2AB;_T287=_T2B0 + 1;*_T287=& _T2AC;_T288=_T2B0 + 2;*_T288=& _T2AD;_T289=_T2B0 + 3;*_T289=& _T2AE;_T28A=_T2B0 + 4;*_T28A=& _T2AF;_T28B=p2;_T28C=_T28B->loc;_T28D=_tag_fat(_T2B0,sizeof(void*),5);Cyc_Warn_err2(_T28C,_T28D);}}}}}goto _TLD7;_TLD6: _TLD7:;}}_T28E=ps;
# 567
ps=_T28E->tl;_T28F=tqts;tqts=_T28F->tl;goto _TLD2;_TLD1:
# 585
 if(ps==0)goto _TLD9;{struct Cyc_Warn_String_Warn_Warg_struct _T2AB;_T2AB.tag=0;
_T2AB.f1=_tag_fat("too many arguments for datatype constructor ",sizeof(char),45U);_T290=_T2AB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AB=_T290;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T2AC;_T2AC.tag=1;_T292=tuf;_T2AC.f1=_T292->name;_T291=_T2AC;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T2AC=_T291;void*_T2AD[2];_T293=_T2AD + 0;*_T293=& _T2AB;_T294=_T2AD + 1;*_T294=& _T2AC;_T295=p;_T296=_T295->loc;_T297=_tag_fat(_T2AD,sizeof(void*),2);Cyc_Warn_err2(_T296,_T297);}}goto _TLDA;_TLD9: _TLDA:
 if(tqts==0)goto _TLDB;{struct Cyc_Warn_String_Warn_Warg_struct _T2AB;_T2AB.tag=0;
_T2AB.f1=_tag_fat("too few arguments for datatype constructor ",sizeof(char),44U);_T298=_T2AB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2AB=_T298;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T2AC;_T2AC.tag=1;_T29A=tuf;_T2AC.f1=_T29A->name;_T299=_T2AC;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T2AC=_T299;void*_T2AD[2];_T29B=_T2AD + 0;*_T29B=& _T2AB;_T29C=_T2AD + 1;*_T29C=& _T2AC;_T29D=p;_T29E=_T29D->loc;_T29F=_tag_fat(_T2AD,sizeof(void*),2);Cyc_Warn_err2(_T29E,_T29F);}}goto _TLDC;_TLDB: _TLDC: goto _LL0;}}}}}}case 14: goto _LL22;case 16: _LL22: goto _LL24;default: _LL24: _T2A0=
# 593
Cyc_Tcenv_lookup_opt_type_vars(te);t=Cyc_Absyn_wildtyp(_T2A0);goto _LL0;}_LL0:;}
# 595
tcpat_end: _T2A1=p;
_T2A1->topt=t;_T2A2=res;
return _T2A2;}}}
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
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){struct _tuple0 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;struct Cyc_Absyn_PtrInfo _T8;struct Cyc_Absyn_PtrAtts _T9;struct Cyc_Absyn_PtrInfo _TA;struct Cyc_Absyn_PtrAtts _TB;void*_TC;void*_TD;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TE;void**_TF;struct Cyc_List_List*_T10;struct _tuple0*_T11;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T12;void**_T13;int*_T14;int _T15;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T16;void*_T17;int*_T18;int _T19;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;void*_T1F;int*_T20;int _T21;struct Cyc_List_List*_T22;void**_T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;void**_T25;unsigned _T26;struct _fat_ptr _T27;int _T28;struct _tuple0 _T29;void*_T2A;int*_T2B;int _T2C;void*_T2D;int*_T2E;int _T2F;void*_T30;void*_T31;struct Cyc_Warn_String_Warn_Warg_struct _T32;void**_T33;unsigned _T34;struct _fat_ptr _T35;struct Cyc_Warn_String_Warn_Warg_struct _T36;struct Cyc_Warn_Typ_Warn_Warg_struct _T37;struct Cyc_Warn_String_Warn_Warg_struct _T38;struct Cyc_Warn_Typ_Warn_Warg_struct _T39;void**_T3A;void**_T3B;void**_T3C;void**_T3D;unsigned _T3E;struct _fat_ptr _T3F;
# 650
struct Cyc_List_List*assump=0;{struct _tuple0 _T40;
_T40.f0=Cyc_Absyn_compress(old_type);_T40.f1=Cyc_Absyn_compress(new_type);_T0=_T40;}{struct _tuple0 _T40=_T0;void*_T41;void*_T42;void*_T43;_T1=_T40.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=4)goto _TLEF;_T4=_T40.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=4)goto _TLF1;_T7=_T40.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T44=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T7;_T8=_T44->f1;_T9=_T8.ptr_atts;_T43=_T9.eff;_TA=_T44->f1;_TB=_TA.ptr_atts;_T42=_TB.aqual;}_TC=_T40.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T44=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TC;_TD=_T40.f1;_TE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TD;_TF=& _TE->f1.ptr_atts.aqual;_T41=(void**)_TF;}{void*oldrgn=_T43;void*aq_old=_T42;void**aq_new=(void**)_T41;{struct Cyc_List_List*_T44=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T45=_cycalloc(sizeof(struct _tuple0));
# 653
_T45->f0=oldrgn;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T46=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T46->tag=2;_T46->f1=tv;_T12=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T46;}_T45->f1=(void*)_T12;_T11=(struct _tuple0*)_T45;}_T44->hd=_T11;_T44->tl=0;_T10=(struct Cyc_List_List*)_T44;}assump=_T10;_T13=aq_new;{
void*_T44=*_T13;struct Cyc_List_List*_T45;_T14=(int*)_T44;_T15=*_T14;if(_T15!=0)goto _TLF3;_T16=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T44;_T17=_T16->f1;_T18=(int*)_T17;_T19=*_T18;if(_T19!=17)goto _TLF5;_T1A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T44;_T1B=_T1A->f2;if(_T1B==0)goto _TLF7;_T1C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T44;_T1D=_T1C->f2;_T1E=(struct Cyc_List_List*)_T1D;_T1F=_T1E->hd;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=1)goto _TLF9;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T46=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T44;_T22=_T46->f2;{struct Cyc_List_List _T47=*_T22;_T45=_T47.tl;}}{struct Cyc_List_List*bnd=_T45;_T23=aq_new;
# 656
*_T23=Cyc_Absyn_al_qual_type;goto _LL5;}_TLF9: goto _LL8;_TLF7: goto _LL8;_TLF5: goto _LL8;_TLF3: _LL8: goto _LL5;_LL5:;}goto _LL0;}_TLF1: goto _LL3;_TLEF: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _T44;_T44.tag=0;
# 663
_T44.f1=_tag_fat("alias requires pointer type",sizeof(char),28U);_T24=_T44;}{struct Cyc_Warn_String_Warn_Warg_struct _T44=_T24;void*_T45[1];_T25=_T45 + 0;*_T25=& _T44;_T26=loc;_T27=_tag_fat(_T45,sizeof(void*),1);Cyc_Warn_err2(_T26,_T27);}goto _LL0;_LL0:;}_T28=
# 666
Cyc_Subtype_subtype(loc,assump,old_type,new_type);if(!_T28)goto _TLFB;{struct _tuple0 _T40;
# 683 "tcpat.cyc"
_T40.f0=Cyc_Absyn_compress(old_type);_T40.f1=Cyc_Absyn_compress(new_type);_T29=_T40;}{struct _tuple0 _T40=_T29;struct Cyc_Absyn_PtrInfo _T41;struct Cyc_Absyn_PtrInfo _T42;_T2A=_T40.f0;_T2B=(int*)_T2A;_T2C=*_T2B;if(_T2C!=4)goto _TLFD;_T2D=_T40.f1;_T2E=(int*)_T2D;_T2F=*_T2E;if(_T2F!=4)goto _TLFF;_T30=_T40.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T43=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T30;_T42=_T43->f1;}_T31=_T40.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T43=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T31;_T41=_T43->f1;}{struct Cyc_Absyn_PtrInfo pold=_T42;struct Cyc_Absyn_PtrInfo pnew=_T41;goto _LLA;}_TLFF: goto _LLD;_TLFD: _LLD:{struct Cyc_Warn_String_Warn_Warg_struct _T43;_T43.tag=0;
# 685
_T43.f1=_tag_fat("alias requires pointer type",sizeof(char),28U);_T32=_T43;}{struct Cyc_Warn_String_Warn_Warg_struct _T43=_T32;void*_T44[1];_T33=_T44 + 0;*_T33=& _T43;_T34=loc;_T35=_tag_fat(_T44,sizeof(void*),1);Cyc_Warn_err2(_T34,_T35);}goto _LLA;_LLA:;}goto _TLFC;
# 688
_TLFB:{struct Cyc_Warn_String_Warn_Warg_struct _T40;_T40.tag=0;_T40.f1=_tag_fat("cannot alias value of type ",sizeof(char),28U);_T36=_T40;}{struct Cyc_Warn_String_Warn_Warg_struct _T40=_T36;{struct Cyc_Warn_Typ_Warn_Warg_struct _T41;_T41.tag=2;_T41.f1=old_type;_T37=_T41;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T41=_T37;{struct Cyc_Warn_String_Warn_Warg_struct _T42;_T42.tag=0;_T42.f1=_tag_fat(" to type ",sizeof(char),10U);_T38=_T42;}{struct Cyc_Warn_String_Warn_Warg_struct _T42=_T38;{struct Cyc_Warn_Typ_Warn_Warg_struct _T43;_T43.tag=2;_T43.f1=new_type;_T39=_T43;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T43=_T39;void*_T44[4];_T3A=_T44 + 0;*_T3A=& _T40;_T3B=_T44 + 1;*_T3B=& _T41;_T3C=_T44 + 2;*_T3C=& _T42;_T3D=_T44 + 3;*_T3D=& _T43;_T3E=loc;_T3F=_tag_fat(_T44,sizeof(void*),4);Cyc_Warn_err2(_T3E,_T3F);}}}}_TLFC:;}
# 694
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Pat*_T3;void*_T4;int*_T5;int _T6;struct Cyc_Absyn_PtrInfo _T7;struct Cyc_Absyn_PtrAtts _T8;struct Cyc_Absyn_PtrInfo _T9;struct Cyc_Absyn_PtrAtts _TA;struct Cyc_Tcenv_Tenv*_TB;struct Cyc_Absyn_Pat*_TC;unsigned _TD;void*_TE;struct Cyc_Tcenv_Tenv*_TF;struct Cyc_Absyn_Pat*_T10;int _T11;struct Cyc_List_List*_T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;void**_T14;int(*_T15)(struct _fat_ptr);void*(*_T16)(struct _fat_ptr);struct _fat_ptr _T17;void*_T18;struct Cyc_Tcenv_Tenv*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct _tuple16*_T1C;struct _tuple16 _T1D;struct Cyc_Absyn_Pat*_T1E;int _T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;struct Cyc_Tcenv_Tenv*_T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Absyn_Pat*_T25;int _T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_Absyn_Vardecl**_T2B;struct Cyc_Absyn_Vardecl*_T2C;struct Cyc_Absyn_Vardecl*_T2D;struct Cyc_Absyn_Exp*_T2E;int*_T2F;int _T30;struct Cyc_List_List*_T31;struct Cyc_Absyn_Exp*_T32;void*_T33;void*_T34;int _T35;struct Cyc_Warn_String_Warn_Warg_struct _T36;void**_T37;struct Cyc_Absyn_Pat*_T38;unsigned _T39;struct _fat_ptr _T3A;struct Cyc_Warn_String_Warn_Warg_struct _T3B;void**_T3C;int(*_T3D)(struct _fat_ptr);void*(*_T3E)(struct _fat_ptr);struct _fat_ptr _T3F;struct Cyc_List_List*_T40;struct Cyc_Absyn_Pat*_T41;int*_T42;int _T43;int _T44;struct Cyc_Warn_String_Warn_Warg_struct _T45;void**_T46;struct Cyc_Absyn_Pat*_T47;unsigned _T48;struct _fat_ptr _T49;struct Cyc_List_List*_T4A;void*_T4B;struct Cyc_Absyn_Vardecl**_T4C;struct Cyc_Absyn_Vardecl*_T4D;struct Cyc_Absyn_Vardecl*_T4E;struct Cyc_Warn_String_Warn_Warg_struct _T4F;void**_T50;struct Cyc_Absyn_Pat*_T51;unsigned _T52;struct _fat_ptr _T53;struct Cyc_Absyn_Pat*_T54;unsigned _T55;struct Cyc_Tcenv_Tenv*_T56;struct Cyc_List_List*_T57;struct Cyc_Absyn_Tvar**_T58;struct _fat_ptr _T59;struct Cyc_Tcenv_Tenv*_T5A;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T5B;void*_T5C;struct Cyc_Tcenv_Tenv*_T5D;struct Cyc_Absyn_Pat*_T5E;unsigned _T5F;struct Cyc_Absyn_Exp*_T60;void*_T61;void*_T62;struct Cyc_Absyn_Tvar*_T63;struct Cyc_Absyn_Vardecl*_T64;void*_T65;struct Cyc_Absyn_Exp*_T66;struct Cyc_List_List*_T67;_T0=p;{
# 696
void*_T68=_T0->r;struct Cyc_Absyn_Tvar*_T69;struct Cyc_Absyn_Pat*_T6A;struct Cyc_Absyn_Vardecl*_T6B;struct Cyc_List_List*_T6C;struct Cyc_List_List*_T6D;int _T6E;void*_T6F;_T1=(int*)_T68;_T2=*_T1;switch(_T2){case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T70=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T68;_T6F=_T70->f1;}{struct Cyc_Absyn_Pat*p2=_T6F;_T3=p;_T4=_T3->topt;{
# 698
void*_T70=_check_null(_T4);void*_T71;void*_T72;_T5=(int*)_T70;_T6=*_T5;if(_T6!=4)goto _TL102;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T73=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T70;_T7=_T73->f1;_T8=_T7.ptr_atts;_T72=_T8.eff;_T9=_T73->f1;_TA=_T9.ptr_atts;_T71=_TA.aqual;}{void*rt=_T72;void*aq=_T71;_TB=te;_TC=p;_TD=_TC->loc;_TE=rt;
# 700
Cyc_Tcenv_check_effect_accessible_nodelay(_TB,_TD,_TE);_TF=te;_T10=p2;_T11=
Cyc_Tcutil_is_noalias_qual(aq,0);_T12=patvars;Cyc_Tcpat_check_pat_regions_rec(_TF,_T10,_T11,_T12);
return;}_TL102:{struct Cyc_Warn_String_Warn_Warg_struct _T73;_T73.tag=0;
_T73.f1=_tag_fat("check_pat_regions: bad pointer type",sizeof(char),36U);_T13=_T73;}{struct Cyc_Warn_String_Warn_Warg_struct _T73=_T13;void*_T74[1];_T14=_T74 + 0;*_T14=& _T73;_T16=Cyc_Warn_impos2;{int(*_T75)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T16;_T15=_T75;}_T17=_tag_fat(_T74,sizeof(void*),1);_T15(_T17);};}}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T70=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T68;_T18=_T70->f1;_T6F=(void*)_T18;_T6E=_T70->f2;_T6D=_T70->f3;_T6C=_T70->f4;}{void*ai=_T6F;int is_tuple=_T6E;struct Cyc_List_List*exist_ts=_T6D;struct Cyc_List_List*dps=_T6C;
# 706
_TL107: if(dps!=0)goto _TL105;else{goto _TL106;}
_TL105: _T19=te;_T1A=dps;_T1B=_T1A->hd;_T1C=(struct _tuple16*)_T1B;_T1D=*_T1C;_T1E=_T1D.f1;_T1F=did_noalias_deref;_T20=patvars;Cyc_Tcpat_check_pat_regions_rec(_T19,_T1E,_T1F,_T20);_T21=dps;
# 706
dps=_T21->tl;goto _TL107;_TL106:
# 708
 return;}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T70=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T68;_T6D=_T70->f3;}{struct Cyc_List_List*ps=_T6D;
# 711
did_noalias_deref=0;
_TL10B: if(ps!=0)goto _TL109;else{goto _TL10A;}
_TL109: _T22=te;_T23=ps;_T24=_T23->hd;_T25=(struct Cyc_Absyn_Pat*)_T24;_T26=did_noalias_deref;_T27=patvars;Cyc_Tcpat_check_pat_regions_rec(_T22,_T25,_T26,_T27);_T28=ps;
# 712
ps=_T28->tl;goto _TL10B;_TL10A:
# 714
 return;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T70=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T68;_T6B=_T70->f1;_T6A=_T70->f2;}{struct Cyc_Absyn_Vardecl*vd=_T6B;struct Cyc_Absyn_Pat*p2=_T6A;{
# 716
struct Cyc_List_List*x=patvars;_TL10F: if(x!=0)goto _TL10D;else{goto _TL10E;}
_TL10D: _T29=x;_T2A=_T29->hd;{struct _tuple17*_T70=(struct _tuple17*)_T2A;struct Cyc_Absyn_Exp*_T71;struct Cyc_Absyn_Vardecl**_T72;{struct _tuple17 _T73=*_T70;_T72=_T73.f0;_T71=_T73.f1;}{struct Cyc_Absyn_Vardecl**vdopt=_T72;struct Cyc_Absyn_Exp*eopt=_T71;
# 722
if(vdopt==0)goto _TL110;_T2B=vdopt;_T2C=*_T2B;_T2D=vd;if(_T2C!=_T2D)goto _TL110;if(eopt==0)goto _TL110;_T2E=eopt;{
void*_T73=_T2E->r;struct Cyc_Absyn_Exp*_T74;_T2F=(int*)_T73;_T30=*_T2F;if(_T30!=15)goto _TL112;{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T75=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T73;_T74=_T75->f1;}{struct Cyc_Absyn_Exp*e=_T74;_T31=
# 725
Cyc_Tcenv_curr_aquals_bounds(te);_T32=e;_T33=_T32->topt;_T34=_check_null(_T33);_T35=Cyc_Tcutil_is_noalias_pointer_or_aggr(_T31,_T34);if(!_T35)goto _TL114;{struct Cyc_Warn_String_Warn_Warg_struct _T75;_T75.tag=0;
_T75.f1=_tag_fat("reference pattern not allowed on alias-free pointers",sizeof(char),53U);_T36=_T75;}{struct Cyc_Warn_String_Warn_Warg_struct _T75=_T36;void*_T76[1];_T37=_T76 + 0;*_T37=& _T75;_T38=p;_T39=_T38->loc;_T3A=_tag_fat(_T76,sizeof(void*),1);Cyc_Warn_err2(_T39,_T3A);}goto _TL115;_TL114: _TL115: goto _LL17;}_TL112:{struct Cyc_Warn_String_Warn_Warg_struct _T75;_T75.tag=0;
# 728
_T75.f1=_tag_fat("check_pat_regions: bad reference access exp",sizeof(char),44U);_T3B=_T75;}{struct Cyc_Warn_String_Warn_Warg_struct _T75=_T3B;void*_T76[1];_T3C=_T76 + 0;*_T3C=& _T75;_T3E=Cyc_Warn_impos2;{int(*_T77)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T3E;_T3D=_T77;}_T3F=_tag_fat(_T76,sizeof(void*),1);_T3D(_T3F);}_LL17:;}goto _TL10E;_TL110:;}}_T40=x;
# 716
x=_T40->tl;goto _TL10F;_TL10E:;}
# 733
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T70=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T68;_T6B=_T70->f1;_T6A=_T70->f2;}{struct Cyc_Absyn_Vardecl*vd=_T6B;struct Cyc_Absyn_Pat*p2=_T6A;_T41=p;{
# 736
void*_T70=_T41->topt;if(_T70==0)goto _TL116;_T42=(int*)_T70;_T43=*_T42;if(_T43!=5)goto _TL118;_T44=did_noalias_deref;
# 738
if(_T44)goto _TL11A;else{goto _TL11C;}
_TL11C: goto _LL1C;_TL11A:{struct Cyc_Warn_String_Warn_Warg_struct _T71;_T71.tag=0;
_T71.f1=_tag_fat("pattern to array would create alias of no-alias pointer",sizeof(char),56U);_T45=_T71;}{struct Cyc_Warn_String_Warn_Warg_struct _T71=_T45;void*_T72[1];_T46=_T72 + 0;*_T46=& _T71;_T47=p;_T48=_T47->loc;_T49=_tag_fat(_T72,sizeof(void*),1);Cyc_Warn_err2(_T48,_T49);}
return;_TL118: goto _LL1F;_TL116: _LL1F: goto _LL1C;_LL1C:;}
# 744
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 2:{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T70=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T68;_T69=_T70->f1;_T6B=_T70->f2;}{struct Cyc_Absyn_Tvar*tv=_T69;struct Cyc_Absyn_Vardecl*vd=_T6B;{
# 747
struct Cyc_List_List*x=patvars;_TL120: if(x!=0)goto _TL11E;else{goto _TL11F;}
_TL11E: _T4A=x;_T4B=_T4A->hd;{struct _tuple17*_T70=(struct _tuple17*)_T4B;struct Cyc_Absyn_Exp*_T71;struct Cyc_Absyn_Vardecl**_T72;{struct _tuple17 _T73=*_T70;_T72=_T73.f0;_T71=_T73.f1;}{struct Cyc_Absyn_Vardecl**vdopt=_T72;struct Cyc_Absyn_Exp*eopt=_T71;
# 751
if(vdopt==0)goto _TL121;_T4C=vdopt;_T4D=*_T4C;_T4E=vd;if(_T4D!=_T4E)goto _TL121;
if(eopt!=0)goto _TL123;{struct Cyc_Warn_String_Warn_Warg_struct _T73;_T73.tag=0;
_T73.f1=_tag_fat("cannot alias pattern expression in datatype",sizeof(char),44U);_T4F=_T73;}{struct Cyc_Warn_String_Warn_Warg_struct _T73=_T4F;void*_T74[1];_T50=_T74 + 0;*_T50=& _T73;_T51=p;_T52=_T51->loc;_T53=_tag_fat(_T74,sizeof(void*),1);Cyc_Warn_err2(_T52,_T53);}goto _TL124;
# 755
_TL123: _T54=p;_T55=_T54->loc;_T56=te;{struct Cyc_Absyn_Tvar*_T73[1];_T58=_T73 + 0;*_T58=tv;_T59=_tag_fat(_T73,sizeof(struct Cyc_Absyn_Tvar*),1);_T57=Cyc_List_list(_T59);}{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(_T55,_T56,_T57);_T5A=te2;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T73=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T73->tag=2;
_T73->f1=tv;_T5B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T73;}_T5C=(void*)_T5B;te2=Cyc_Tcenv_add_region(_T5A,_T5C,0,0);_T5D=te2;_T5E=p;_T5F=_T5E->loc;_T60=eopt;_T61=_T60->topt;_T62=
# 758
_check_null(_T61);_T63=tv;_T64=vd;_T65=_T64->type;_T66=eopt;Cyc_Tcpat_check_alias_coercion(_T5D,_T5F,_T62,_T63,_T65,_T66);}_TL124: goto _TL11F;_TL121:;}}_T67=x;
# 747
x=_T67->tl;goto _TL120;_TL11F:;}goto _LL0;}default:
# 764
 return;}_LL0:;}}
# 779 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_Tcpat_TcPatResult*pat_res){struct Cyc_Tcpat_TcPatResult*_T0;struct Cyc_Tcpat_TcPatResult*_T1;struct Cyc_List_List*_T2;void*_T3;int*_T4;int _T5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6;void*_T7;int*_T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_List_List*_TD;struct Cyc_Absyn_Exp*_TE;void*_TF;void*_T10;int _T11;struct Cyc_List_List*_T12;struct Cyc_Absyn_Exp*_T13;int _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;void**_T17;void**_T18;struct Cyc_Absyn_Pat*_T19;unsigned _T1A;struct _fat_ptr _T1B;struct Cyc_Warn_String_Warn_Warg_struct _T1C;struct Cyc_Warn_Qvar_Warn_Warg_struct _T1D;struct Cyc_Absyn_Vardecl**_T1E;struct Cyc_Absyn_Vardecl*_T1F;struct Cyc_Warn_String_Warn_Warg_struct _T20;void**_T21;void**_T22;void**_T23;struct Cyc_Absyn_Pat*_T24;unsigned _T25;struct _fat_ptr _T26;struct Cyc_List_List*_T27;_T0=pat_res;{
struct Cyc_List_List*patvars=_T0->patvars;
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{struct Cyc_List_List*_T28;_T1=pat_res;{struct Cyc_Tcpat_TcPatResult _T29=*_T1;_T28=_T29.aquals_bounds;}{struct Cyc_List_List*aqb=_T28;
# 784
_TL128: if(aqb!=0)goto _TL126;else{goto _TL127;}
_TL126: _T2=aqb;_T3=_T2->hd;{struct _tuple0*_T29=(struct _tuple0*)_T3;void*_T2A;void*_T2B;{struct _tuple0 _T2C=*_T29;_T2B=_T2C.f0;_T2A=_T2C.f1;}{void*tv=_T2B;void*b=_T2A;
void*_T2C=Cyc_Absyn_compress(tv);_T4=(int*)_T2C;_T5=*_T4;if(_T5!=0)goto _TL129;_T6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2C;_T7=_T6->f1;_T8=(int*)_T7;_T9=*_T8;if(_T9!=15)goto _TL12B;
# 788
te=Cyc_Tcenv_add_aquals_bound(te,tv,b);goto _LL6;_TL12B: goto _LL9;_TL129: _LL9: goto _LL6;_LL6:;}}_TA=aqb;
# 784
aqb=_TA->tl;goto _TL128;_TL127: {
# 794
struct Cyc_List_List*x=patvars;_TL130: if(x!=0)goto _TL12E;else{goto _TL12F;}
_TL12E: _TB=x;_TC=_TB->hd;{struct _tuple17*_T29=(struct _tuple17*)_TC;struct Cyc_Absyn_Exp*_T2A;struct Cyc_Absyn_Vardecl**_T2B;{struct _tuple17 _T2C=*_T29;_T2B=_T2C.f0;_T2A=_T2C.f1;}{struct Cyc_Absyn_Vardecl**vdopt=_T2B;struct Cyc_Absyn_Exp*eopt=_T2A;
if(eopt==0)goto _TL131;{
struct Cyc_Absyn_Exp*e=eopt;_TD=
Cyc_Tcenv_curr_aquals_bounds(te);_TE=e;_TF=_TE->topt;_T10=_check_null(_TF);_T11=Cyc_Tcutil_is_noalias_pointer_or_aggr(_TD,_T10);if(!_T11)goto _TL133;_T12=
Cyc_Tcenv_curr_aquals_bounds(te);_T13=e;_T14=Cyc_Tcutil_is_noalias_path(_T12,_T13);
# 798
if(_T14)goto _TL133;else{goto _TL135;}
# 801
_TL135: if(vdopt!=0)goto _TL136;{struct Cyc_Warn_String_Warn_Warg_struct _T2C;_T2C.tag=0;
_T2C.f1=_tag_fat("pattern dereferences an alias-free-pointer ",sizeof(char),44U);_T15=_T2C;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C=_T15;{struct Cyc_Warn_String_Warn_Warg_struct _T2D;_T2D.tag=0;
_T2D.f1=_tag_fat("from a non-unique path",sizeof(char),23U);_T16=_T2D;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D=_T16;void*_T2E[2];_T17=_T2E + 0;*_T17=& _T2C;_T18=_T2E + 1;*_T18=& _T2D;_T19=p;_T1A=_T19->loc;_T1B=_tag_fat(_T2E,sizeof(void*),2);Cyc_Warn_err2(_T1A,_T1B);}}goto _TL137;
# 805
_TL136:{struct Cyc_Warn_String_Warn_Warg_struct _T2C;_T2C.tag=0;_T2C.f1=_tag_fat("pattern for variable ",sizeof(char),22U);_T1C=_T2C;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C=_T1C;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T2D;_T2D.tag=1;_T1E=vdopt;_T1F=*_T1E;_T2D.f1=_T1F->name;_T1D=_T2D;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T2D=_T1D;{struct Cyc_Warn_String_Warn_Warg_struct _T2E;_T2E.tag=0;
_T2E.f1=_tag_fat(" dereferences an alias-free-pointer from a non-unique path",sizeof(char),59U);_T20=_T2E;}{struct Cyc_Warn_String_Warn_Warg_struct _T2E=_T20;void*_T2F[3];_T21=_T2F + 0;*_T21=& _T2C;_T22=_T2F + 1;*_T22=& _T2D;_T23=_T2F + 2;*_T23=& _T2E;_T24=p;_T25=_T24->loc;_T26=_tag_fat(_T2F,sizeof(void*),3);Cyc_Warn_err2(_T25,_T26);}}}_TL137: goto _TL134;_TL133: _TL134:;}goto _TL132;_TL131: _TL132:;}}_T27=x;
# 794
x=_T27->tl;goto _TL130;_TL12F:;}}}}}
# 852 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 861
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 874
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){union Cyc_Tcpat_Name_value _T0;{union Cyc_Tcpat_Name_value _T1;_T1.Name_v.tag=1U;_T1.Name_v.val=s;_T0=_T1;}return _T0;}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _T0;{union Cyc_Tcpat_Name_value _T1;_T1.Int_v.tag=2U;_T1.Int_v.val=i;_T0=_T1;}return _T0;}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 891
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){struct Cyc_Tcpat_Con_s*_T0;struct _union_Name_value_Name_v _T1;unsigned _T2;struct _union_Name_value_Name_v _T3;struct Cyc_Tcpat_Con_s*_T4;struct _union_Name_value_Name_v _T5;unsigned _T6;struct _union_Name_value_Name_v _T7;int _T8;struct _union_Name_value_Int_v _T9;struct Cyc_Tcpat_Con_s*_TA;struct _union_Name_value_Name_v _TB;unsigned _TC;struct _union_Name_value_Int_v _TD;int _TE;_T0=c1;{
union Cyc_Tcpat_Name_value _TF=_T0->name;int _T10;struct _fat_ptr _T11;_T1=_TF.Name_v;_T2=_T1.tag;if(_T2!=1)goto _TL138;_T3=_TF.Name_v;_T11=_T3.val;{struct _fat_ptr n1=_T11;_T4=c2;{
# 894
union Cyc_Tcpat_Name_value _T12=_T4->name;struct _fat_ptr _T13;_T5=_T12.Name_v;_T6=_T5.tag;if(_T6!=1)goto _TL13A;_T7=_T12.Name_v;_T13=_T7.val;{struct _fat_ptr n2=_T13;_T8=
Cyc_strcmp(n1,n2);return _T8;}_TL13A:
 return -1;;}}goto _TL139;_TL138: _T9=_TF.Int_v;_T10=_T9.val;{int i1=_T10;_TA=c2;{
# 899
union Cyc_Tcpat_Name_value _T12=_TA->name;int _T13;_TB=_T12.Name_v;_TC=_TB.tag;if(_TC!=1)goto _TL13C;
return 1;_TL13C: _TD=_T12.Int_v;_T13=_TD.val;{int i2=_T13;_TE=i1 - i2;
return _TE;};}}_TL139:;}}
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
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){struct Cyc_Absyn_Aggrdecl*_T0;struct Cyc_Absyn_AggrdeclImpl*_T1;struct Cyc_Absyn_AggrdeclImpl*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Aggrfield*_T5;struct _fat_ptr*_T6;struct _fat_ptr _T7;struct _fat_ptr*_T8;struct _fat_ptr _T9;int _TA;int _TB;struct Cyc_List_List*_TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct _fat_ptr*_TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;void**_T11;void**_T12;void**_T13;int(*_T14)(struct _fat_ptr);void*(*_T15)(struct _fat_ptr);struct _fat_ptr _T16;
int i=1;_T0=ad;_T1=_T0->impl;_T2=
_check_null(_T1);{struct Cyc_List_List*fields=_T2->fields;_TL144: if(fields!=0)goto _TL142;else{goto _TL143;}
_TL142: _T3=fields;_T4=_T3->hd;_T5=(struct Cyc_Absyn_Aggrfield*)_T4;_T6=_T5->name;_T7=*_T6;_T8=f;_T9=*_T8;_TA=Cyc_strcmp(_T7,_T9);if(_TA!=0)goto _TL145;_TB=i;return _TB;_TL145: _TC=fields;
# 927
fields=_TC->tl;i=i + 1;goto _TL144;_TL143:;}{struct Cyc_Warn_String_Warn_Warg_struct _T17;_T17.tag=0;
# 929
_T17.f1=_tag_fat("get_member_offset ",sizeof(char),19U);_TD=_T17;}{struct Cyc_Warn_String_Warn_Warg_struct _T17=_TD;{struct Cyc_Warn_String_Warn_Warg_struct _T18;_T18.tag=0;_TF=f;_T18.f1=*_TF;_TE=_T18;}{struct Cyc_Warn_String_Warn_Warg_struct _T18=_TE;{struct Cyc_Warn_String_Warn_Warg_struct _T19;_T19.tag=0;_T19.f1=_tag_fat(" failed",sizeof(char),8U);_T10=_T19;}{struct Cyc_Warn_String_Warn_Warg_struct _T19=_T10;void*_T1A[3];_T11=_T1A + 0;*_T11=& _T17;_T12=_T1A + 1;*_T12=& _T18;_T13=_T1A + 2;*_T13=& _T19;_T15=Cyc_Warn_impos2;{int(*_T1B)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T15;_T14=_T1B;}_T16=_tag_fat(_T1A,sizeof(void*),3);_T14(_T16);}}}}
# 932
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){union Cyc_Tcpat_PatOrWhere _T0;struct _union_PatOrWhere_where_clause _T1;unsigned _T2;union Cyc_Tcpat_PatOrWhere _T3;struct _union_PatOrWhere_where_clause _T4;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T5;void*_T6;union Cyc_Tcpat_PatOrWhere _T7;struct _union_PatOrWhere_pattern _T8;struct Cyc_Absyn_Pat*_T9;int*_TA;unsigned _TB;union Cyc_Tcpat_PatOrWhere _TC;void*_TD;struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct*_TE;struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct*_TF;void*_T10;struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T11;int _T12;void*_T13;struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T14;char _T15;void*_T16;struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_T17;void*_T18;struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_T19;void*_T1A;void*_T1B;struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_T1C;void*_T1D;struct Cyc_Absyn_Pat*_T1E;void*_T1F;void*_T20;int _T21;struct Cyc_Absyn_Pat*_T22;void*_T23;void*_T24;int _T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;void**_T27;int(*_T28)(struct _fat_ptr);void*(*_T29)(struct _fat_ptr);struct _fat_ptr _T2A;struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct*_T2B;struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct*_T2C;void*_T2D;struct Cyc_Absyn_Datatypedecl*_T2E;int _T2F;struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_T30;void*_T31;struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_T32;struct Cyc_Absyn_Datatypedecl*_T33;struct Cyc_Absyn_Datatypefield*_T34;struct _tuple2*_T35;unsigned _T36;void*_T37;void*_T38;void*_T39;int*_T3A;int _T3B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C;void*_T3D;int*_T3E;int _T3F;void*_T40;struct Cyc_Absyn_Aggrdecl*_T41;struct Cyc_Absyn_AggrdeclImpl*_T42;struct Cyc_Absyn_AggrdeclImpl*_T43;int _T44;struct Cyc_Warn_String_Warn_Warg_struct _T45;void**_T46;int(*_T47)(struct _fat_ptr);void*(*_T48)(struct _fat_ptr);struct _fat_ptr _T49;struct Cyc_List_List*_T4A;void*_T4B;struct _tuple16*_T4C;struct _tuple16 _T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;int*_T50;int _T51;struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T52;void*_T53;struct Cyc_Warn_String_Warn_Warg_struct _T54;void**_T55;int(*_T56)(struct _fat_ptr);void*(*_T57)(struct _fat_ptr);struct _fat_ptr _T58;struct Cyc_Warn_String_Warn_Warg_struct _T59;void**_T5A;int(*_T5B)(struct _fat_ptr);void*(*_T5C)(struct _fat_ptr);struct _fat_ptr _T5D;struct Cyc_Warn_String_Warn_Warg_struct _T5E;void**_T5F;int(*_T60)(struct _fat_ptr);void*(*_T61)(struct _fat_ptr);struct _fat_ptr _T62;struct Cyc_Absyn_Pat*_T63;struct Cyc_Absyn_Exp*_T64;_T0=pw;_T1=_T0.where_clause;_T2=_T1.tag;if(_T2!=2)goto _TL147;_T3=pw;_T4=_T3.where_clause;_T64=_T4.val;{struct Cyc_Absyn_Exp*e=_T64;{struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T65=_cycalloc(sizeof(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct));_T65->tag=0;
# 934
_T65->f1=e;_T5=(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_T65;}_T6=(void*)_T5;return _T6;}_TL147: _T7=pw;_T8=_T7.pattern;_T63=_T8.val;{struct Cyc_Absyn_Pat*p=_T63;_T9=p;{
# 936
void*_T65=_T9->r;struct Cyc_List_List*_T66;struct Cyc_Absyn_Datatypefield*_T67;struct Cyc_Absyn_Enumfield*_T68;struct _fat_ptr _T69;char _T6A;int _T6B;enum Cyc_Absyn_Sign _T6C;void*_T6D;_TA=(int*)_T65;_TB=*_TA;switch(_TB){case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T65;_T6D=_T6E->f2;}{struct Cyc_Absyn_Pat*p1=_T6D;_T6D=p1;goto _LL9;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T65;_T6D=_T6E->f2;}_LL9: {struct Cyc_Absyn_Pat*p1=_T6D;{union Cyc_Tcpat_PatOrWhere _T6E;_T6E.pattern.tag=1U;
# 938
_T6E.pattern.val=p1;_TC=_T6E;}_TD=Cyc_Tcpat_get_pat_test(_TC);return _TD;}case 8: _TE=& Cyc_Tcpat_EqNull_val;_TF=(struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct*)_TE;_T10=(void*)_TF;
return _T10;case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T65;_T6C=_T6E->f1;_T6B=_T6E->f2;}{enum Cyc_Absyn_Sign s=_T6C;int i=_T6B;{struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T6E=_cycalloc(sizeof(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct));_T6E->tag=6;_T12=i;
_T6E->f1=(unsigned)_T12;_T11=(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_T6E;}_T13=(void*)_T11;return _T13;}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T65;_T6A=_T6E->f1;}{char c=_T6A;{struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T6E=_cycalloc(sizeof(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct));_T6E->tag=6;_T15=c;
_T6E->f1=(unsigned)_T15;_T14=(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_T6E;}_T16=(void*)_T14;return _T16;}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T65;_T69=_T6E->f1;_T6B=_T6E->f2;}{struct _fat_ptr f=_T69;int i=_T6B;{struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_T6E=_cycalloc(sizeof(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct));_T6E->tag=5;
_T6E->f1=f;_T6E->f2=i;_T17=(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_T6E;}_T18=(void*)_T17;return _T18;}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T65;_T6D=_T6E->f1;_T68=_T6E->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T6D;struct Cyc_Absyn_Enumfield*ef=_T68;{struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_T6E=_cycalloc(sizeof(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct));_T6E->tag=3;
_T6E->f1=ed;_T6E->f2=ef;_T19=(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_T6E;}_T1A=(void*)_T19;return _T1A;}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T65;_T1B=_T6E->f1;_T6D=(void*)_T1B;_T68=_T6E->f2;}{void*t=_T6D;struct Cyc_Absyn_Enumfield*ef=_T68;{struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_T6E=_cycalloc(sizeof(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct));_T6E->tag=4;
_T6E->f1=t;_T6E->f2=ef;_T1C=(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_T6E;}_T1D=(void*)_T1C;return _T1D;}case 5: _T1E=p;_T1F=_T1E->topt;_T20=
# 946
_check_null(_T1F);_T21=Cyc_Tcutil_is_pointer_type(_T20);if(_T21)goto _TL14D;else{goto _TL14C;}_TL14D: _T22=p;_T23=_T22->topt;_T24=
_check_null(_T23);_T25=Cyc_Tcutil_is_nullable_pointer_type(_T24,0);
# 946
if(_T25)goto _TL14A;else{goto _TL14C;}
# 948
_TL14C:{struct Cyc_Warn_String_Warn_Warg_struct _T6E;_T6E.tag=0;_T6E.f1=_tag_fat("non-null pointer type or non-pointer type in pointer pattern",sizeof(char),61U);_T26=_T6E;}{struct Cyc_Warn_String_Warn_Warg_struct _T6E=_T26;void*_T6F[1];_T27=_T6F + 0;*_T27=& _T6E;_T29=Cyc_Warn_impos2;{int(*_T70)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T29;_T28=_T70;}_T2A=_tag_fat(_T6F,sizeof(void*),1);_T28(_T2A);}goto _TL14B;_TL14A: _TL14B: _T2B=& Cyc_Tcpat_NeqNull_val;_T2C=(struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct*)_T2B;_T2D=(void*)_T2C;
return _T2D;case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T65;_T6D=_T6E->f1;_T67=_T6E->f2;}{struct Cyc_Absyn_Datatypedecl*ddecl=_T6D;struct Cyc_Absyn_Datatypefield*df=_T67;_T2E=ddecl;_T2F=_T2E->is_extensible;
# 951
if(!_T2F)goto _TL14E;{struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_T6E=_cycalloc(sizeof(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct));_T6E->tag=9;
_T6E->f1=ddecl;_T6E->f2=df;_T30=(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_T6E;}_T31=(void*)_T30;return _T31;_TL14E:{struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_T6E=_cycalloc(sizeof(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct));_T6E->tag=7;_T33=ddecl;_T34=df;_T35=_T34->name;_T36=
Cyc_Tcpat_datatype_tag_number(_T33,_T35);_T6E->f1=(int)_T36;_T6E->f2=ddecl;_T6E->f3=df;_T32=(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_T6E;}_T37=(void*)_T32;return _T37;}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T6E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T65;_T38=_T6E->f1;_T6D=(void*)_T38;_T66=_T6E->f4;}_T39=(void*)_T6D;if(_T39==0)goto _TL150;{void*atype=_T6D;struct Cyc_List_List*dlps=_T66;
# 955
void*_T6E=Cyc_Absyn_compress(atype);union Cyc_Absyn_AggrInfo _T6F;_T3A=(int*)_T6E;_T3B=*_T3A;if(_T3B!=0)goto _TL152;_T3C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6E;_T3D=_T3C->f1;_T3E=(int*)_T3D;_T3F=*_T3E;if(_T3F!=24)goto _TL154;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T70=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6E;_T40=_T70->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T71=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T40;_T6F=_T71->f1;}}{union Cyc_Absyn_AggrInfo info=_T6F;
# 957
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T41=ad;_T42=_T41->impl;_T43=
_check_null(_T42);_T44=_T43->tagged;if(_T44)goto _TL156;else{goto _TL158;}
_TL158:{struct Cyc_Warn_String_Warn_Warg_struct _T70;_T70.tag=0;_T70.f1=_tag_fat("non-tagged aggregate in pattern test",sizeof(char),37U);_T45=_T70;}{struct Cyc_Warn_String_Warn_Warg_struct _T70=_T45;void*_T71[1];_T46=_T71 + 0;*_T46=& _T70;_T48=Cyc_Warn_impos2;{int(*_T72)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T48;_T47=_T72;}_T49=_tag_fat(_T71,sizeof(void*),1);_T47(_T49);}goto _TL157;_TL156: _TL157: _T4A=
_check_null(dlps);_T4B=_T4A->hd;_T4C=(struct _tuple16*)_T4B;_T4D=*_T4C;_T4E=_T4D.f0;_T4F=_check_null(_T4E);{void*_T70=_T4F->hd;struct _fat_ptr*_T71;_T50=(int*)_T70;_T51=*_T50;if(_T51!=1)goto _TL159;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T72=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T70;_T71=_T72->f1;}{struct _fat_ptr*f=_T71;{struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T72=_cycalloc(sizeof(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct));_T72->tag=8;
_T72->f1=atype;_T72->f2=f;_T72->f3=Cyc_Tcpat_get_member_offset(ad,f);_T52=(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_T72;}_T53=(void*)_T52;return _T53;}_TL159:{struct Cyc_Warn_String_Warn_Warg_struct _T72;_T72.tag=0;
_T72.f1=_tag_fat("no field name in tagged union pattern",sizeof(char),38U);_T54=_T72;}{struct Cyc_Warn_String_Warn_Warg_struct _T72=_T54;void*_T73[1];_T55=_T73 + 0;*_T55=& _T72;_T57=Cyc_Warn_impos2;{int(*_T74)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T57;_T56=_T74;}_T58=_tag_fat(_T73,sizeof(void*),1);_T56(_T58);};}}goto _TL155;_TL154: goto _LL21;_TL155: goto _TL153;_TL152: _LL21:{struct Cyc_Warn_String_Warn_Warg_struct _T70;_T70.tag=0;
# 964
_T70.f1=_tag_fat("non-tagged aggregate in pattern test",sizeof(char),37U);_T59=_T70;}{struct Cyc_Warn_String_Warn_Warg_struct _T70=_T59;void*_T71[1];_T5A=_T71 + 0;*_T5A=& _T70;_T5C=Cyc_Warn_impos2;{int(*_T72)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T5C;_T5B=_T72;}_T5D=_tag_fat(_T71,sizeof(void*),1);_T5B(_T5D);}_TL153:;}goto _TL151;_TL150: goto _LL1C;_TL151:;default: _LL1C:{struct Cyc_Warn_String_Warn_Warg_struct _T6E;_T6E.tag=0;
# 966
_T6E.f1=_tag_fat("non-test pattern in pattern test",sizeof(char),33U);_T5E=_T6E;}{struct Cyc_Warn_String_Warn_Warg_struct _T6E=_T5E;void*_T6F[1];_T5F=_T6F + 0;*_T5F=& _T6E;_T61=Cyc_Warn_impos2;{int(*_T70)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T61;_T60=_T70;}_T62=_tag_fat(_T6F,sizeof(void*),1);_T60(_T62);}};}};}
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
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_T3;int _T4;union Cyc_Tcpat_PatOrWhere _T5;struct Cyc_Absyn_Pat*_T6;void*_T7;void*_T8;int*_T9;int _TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrAtts _TC;void*_TD;int _TE;struct Cyc_Warn_String_Warn_Warg_struct _TF;void**_T10;int(*_T11)(struct _fat_ptr);void*(*_T12)(struct _fat_ptr);struct _fat_ptr _T13;struct Cyc_Absyn_Pat*_T14;void*_T15;void*_T16;int*_T17;int _T18;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T19;void*_T1A;int*_T1B;unsigned _T1C;int*_T1D;struct Cyc_Absyn_Datatypedecl*_T1E;int _T1F;int*_T20;struct Cyc_Absyn_Datatypedecl*_T21;struct Cyc_Core_Opt*_T22;struct Cyc_Core_Opt*_T23;void*_T24;struct Cyc_List_List*_T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;void**_T27;int(*_T28)(struct _fat_ptr);void*(*_T29)(struct _fat_ptr);struct _fat_ptr _T2A;struct Cyc_Absyn_Datatypefield*_T2B;struct _tuple2*_T2C;struct _tuple2 _T2D;struct _fat_ptr*_T2E;struct _fat_ptr _T2F;int*_T30;struct Cyc_List_List*(*_T31)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*);struct Cyc_List_List*(*_T32)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T33;struct Cyc_List_List*_T34;struct Cyc_Absyn_Pat*_T35;void*_T36;void*_T37;int*_T38;unsigned _T39;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3A;void*_T3B;int*_T3C;int _T3D;void*_T3E;struct Cyc_Absyn_Aggrdecl*_T3F;struct Cyc_Absyn_Aggrdecl*_T40;struct Cyc_Absyn_AggrdeclImpl*_T41;struct Cyc_Absyn_AggrdeclImpl*_T42;struct Cyc_Absyn_Aggrdecl*_T43;struct Cyc_Absyn_AggrdeclImpl*_T44;struct Cyc_Warn_String_Warn_Warg_struct _T45;void**_T46;int(*_T47)(struct _fat_ptr);void*(*_T48)(struct _fat_ptr);struct _fat_ptr _T49;enum Cyc_Absyn_AggrKind _T4A;int _T4B;struct Cyc_List_List*_T4C;void*_T4D;struct Cyc_Absyn_Aggrfield*_T4E;struct _fat_ptr*_T4F;struct _fat_ptr _T50;struct _fat_ptr _T51;int _T52;int _T53;struct Cyc_List_List*_T54;void*_T55;struct Cyc_List_List*_T56;struct Cyc_List_List*_T57;void*_T58;int*_T59;int _T5A;struct Cyc_List_List*_T5B;struct Cyc_List_List*_T5C;struct Cyc_List_List*_T5D;struct Cyc_List_List*_T5E;void*_T5F;struct _fat_ptr*_T60;struct Cyc_List_List*_T61;void*_T62;struct Cyc_Absyn_Aggrfield*_T63;struct _fat_ptr*_T64;int _T65;struct Cyc_List_List*_T66;struct Cyc_Warn_String_Warn_Warg_struct _T67;void**_T68;int(*_T69)(struct _fat_ptr);void*(*_T6A)(struct _fat_ptr);struct _fat_ptr _T6B;struct Cyc_List_List*_T6C;int _T6D;struct Cyc_Warn_String_Warn_Warg_struct _T6E;void**_T6F;int(*_T70)(struct _fat_ptr);void*(*_T71)(struct _fat_ptr);struct _fat_ptr _T72;struct Cyc_List_List*_T73;struct Cyc_List_List*_T74;union Cyc_Tcpat_PatOrWhere _T75;int _T76;struct Cyc_Warn_String_Warn_Warg_struct _T77;void**_T78;struct Cyc_Absyn_Pat*_T79;unsigned _T7A;struct _fat_ptr _T7B;int*_T7C;struct Cyc_List_List*_T7D;struct Cyc_List_List*_T7E;struct _tuple16*_T7F;struct _tuple16*_T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;struct Cyc_List_List*_T83;struct _tuple16*_T84;struct _tuple16*_T85;struct Cyc_List_List*_T86;struct Cyc_List_List*_T87;void*_T88;int*_T89;int _T8A;struct Cyc_List_List*_T8B;struct Cyc_List_List*_T8C;struct _tuple16*_T8D;struct _tuple16*_T8E;struct Cyc_List_List*_T8F;struct Cyc_List_List*_T90;struct Cyc_List_List*_T91;struct Cyc_List_List*_T92;struct Cyc_List_List*_T93;struct Cyc_List_List*_T94;struct Cyc_List_List*_T95;void*_T96;struct _tuple16*_T97;struct Cyc_List_List*_T98;void*_T99;struct _fat_ptr*_T9A;struct _fat_ptr _T9B;int*_T9C;struct Cyc_List_List*_T9D;struct Cyc_Absyn_Pat*_T9E;struct Cyc_Warn_String_Warn_Warg_struct _T9F;void**_TA0;int(*_TA1)(struct _fat_ptr);void*(*_TA2)(struct _fat_ptr);struct _fat_ptr _TA3;struct Cyc_Absyn_Enumfield*_TA4;struct _tuple2*_TA5;struct _tuple2 _TA6;struct _fat_ptr*_TA7;struct _fat_ptr _TA8;struct Cyc_Absyn_Pat*_TA9;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_TAA;void*_TAB;
void*s;_T0=p;{
void*_TAC=_T0->r;struct Cyc_Absyn_Enumfield*_TAD;struct Cyc_List_List*_TAE;struct Cyc_Absyn_Datatypefield*_TAF;void*_TB0;struct _fat_ptr _TB1;char _TB2;int _TB3;enum Cyc_Absyn_Sign _TB4;_T1=(int*)_TAC;_T2=*_T1;switch(_T2){case 0: goto _LL4;case 2: _LL4: goto _LL6;case 4: _LL6:{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_TB5=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));
# 1031
_TB5->tag=0;_T3=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_TB5;}s=(void*)_T3;goto _LL0;case 8:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_TAC;_TB4=_TB5->f1;_TB3=_TB5->f2;}{enum Cyc_Absyn_Sign sn=_TB4;int i=_TB3;_T4=i;_T5=
Cyc_Tcpat_pw(p);s=Cyc_Tcpat_int_pat(_T4,_T5);goto _LL0;}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_TAC;_TB2=_TB5->f1;}{char c=_TB2;
s=Cyc_Tcpat_char_pat(c,p);goto _LL0;}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_TAC;_TB1=_TB5->f1;}{struct _fat_ptr f=_TB1;
s=Cyc_Tcpat_float_pat(f,p);goto _LL0;}case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_TAC;_TB0=_TB5->f2;}{struct Cyc_Absyn_Pat*p2=_TB0;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_TAC;_TB0=_TB5->f2;}{struct Cyc_Absyn_Pat*p2=_TB0;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_TAC;_TB0=_TB5->f1;}{struct Cyc_Absyn_Pat*pp=_TB0;_T6=p;_T7=_T6->topt;_T8=
# 1039
_check_null(_T7);{void*_TB5=Cyc_Absyn_compress(_T8);void*_TB6;_T9=(int*)_TB5;_TA=*_T9;if(_TA!=4)goto _TL15C;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TB7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TB5;_TB=_TB7->f1;_TC=_TB.ptr_atts;_TB6=_TC.nullable;}{void*n=_TB6;
# 1041
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
void*ss=Cyc_Tcpat_compile_pat(pp);_TE=is_nullable;
if(!_TE)goto _TL15E;_TD=Cyc_Tcpat_null_ptr_pat(ss,p);goto _TL15F;_TL15E: _TD=Cyc_Tcpat_ptr_pat(ss,p);_TL15F: s=_TD;goto _LL23;}_TL15C:{struct Cyc_Warn_String_Warn_Warg_struct _TB7;_TB7.tag=0;
# 1045
_TB7.f1=_tag_fat("expecting pointertype for pattern!",sizeof(char),35U);_TF=_TB7;}{struct Cyc_Warn_String_Warn_Warg_struct _TB7=_TF;void*_TB8[1];_T10=_TB8 + 0;*_T10=& _TB7;_T12=Cyc_Warn_impos2;{int(*_TB9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T12;_T11=_TB9;}_T13=_tag_fat(_TB8,sizeof(void*),1);_T11(_T13);}_LL23:;}goto _LL0;}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_TAC;_TB0=_TB5->f1;_TAF=_TB5->f2;_TAE=_TB5->f3;}{struct Cyc_Absyn_Datatypedecl*tud=_TB0;struct Cyc_Absyn_Datatypefield*tuf=_TAF;struct Cyc_List_List*ps=_TAE;
# 1049
int*span;_T14=p;_T15=_T14->topt;_T16=
_check_null(_T15);{void*_TB5=Cyc_Absyn_compress(_T16);_T17=(int*)_TB5;_T18=*_T17;if(_T18!=0)goto _TL160;_T19=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB5;_T1A=_T19->f1;_T1B=(int*)_T1A;_T1C=*_T1B;switch(_T1C){case 22: _T1E=tud;_T1F=_T1E->is_extensible;
# 1052
if(!_T1F)goto _TL163;_T1D=0;goto _TL164;_TL163:{int*_TB6=_cycalloc_atomic(sizeof(int));_T21=tud;_T22=_T21->fields;_T23=_check_null(_T22);_T24=_T23->v;_T25=(struct Cyc_List_List*)_T24;*_TB6=Cyc_List_length(_T25);_T20=(int*)_TB6;}_T1D=_T20;_TL164: span=_T1D;goto _LL28;case 23:
# 1054
 span=& Cyc_Tcpat_one_opt;goto _LL28;default: goto _LL2D;}goto _TL161;_TL160: _LL2D:{struct Cyc_Warn_String_Warn_Warg_struct _TB6;_TB6.tag=0;
_TB6.f1=_tag_fat("void datatype pattern has bad type",sizeof(char),35U);_T26=_TB6;}{struct Cyc_Warn_String_Warn_Warg_struct _TB6=_T26;void*_TB7[1];_T27=_TB7 + 0;*_T27=& _TB6;_T29=Cyc_Warn_impos2;{int(*_TB8)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T29;_T28=_TB8;}_T2A=_tag_fat(_TB7,sizeof(void*),1);_T28(_T2A);}_TL161: _LL28:;}_T2B=tuf;_T2C=_T2B->name;_T2D=*_T2C;_T2E=_T2D.f1;_T2F=*_T2E;_T30=span;_T32=Cyc_List_map;{
# 1057
struct Cyc_List_List*(*_TB5)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))_T32;_T31=_TB5;}_T33=ps;_T34=_T31(Cyc_Tcpat_compile_pat,_T33);_T35=p;s=Cyc_Tcpat_con_pat(_T2F,_T30,_T34,_T35);goto _LL0;}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_TAC;_T36=_TB5->f1;_TB0=(void*)_T36;_TAE=_TB5->f4;}{void*atype=_TB0;struct Cyc_List_List*dlps=_TAE;
# 1061
struct Cyc_List_List*fields;
enum Cyc_Absyn_AggrKind aggr_kind;
int is_tagged=0;_T37=
_check_null(atype);{void*_TB5=Cyc_Absyn_compress(_T37);struct Cyc_List_List*_TB6;enum Cyc_Absyn_AggrKind _TB7;union Cyc_Absyn_AggrInfo _TB8;_T38=(int*)_TB5;_T39=*_T38;switch(_T39){case 0: _T3A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB5;_T3B=_T3A->f1;_T3C=(int*)_T3B;_T3D=*_T3C;if(_T3D!=24)goto _TL166;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TB5;_T3E=_TB9->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TBA=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T3E;_TB8=_TBA->f1;}}{union Cyc_Absyn_AggrInfo ainfo=_TB8;
# 1066
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);_T3F=ad;
aggr_kind=_T3F->kind;_T40=ad;_T41=_T40->impl;_T42=
_check_null(_T41);fields=_T42->fields;_T43=ad;_T44=_T43->impl;
is_tagged=_T44->tagged;goto _LL2F;}_TL166: goto _LL34;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_TB9=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_TB5;_TB7=_TB9->f1;_TB6=_TB9->f3;}{enum Cyc_Absyn_AggrKind ak=_TB7;struct Cyc_List_List*fs=_TB6;
# 1072
aggr_kind=ak;
fields=fs;goto _LL2F;}default: _LL34:{struct Cyc_Warn_String_Warn_Warg_struct _TB9;_TB9.tag=0;
# 1075
_TB9.f1=_tag_fat("bad type on aggr pattern",sizeof(char),25U);_T45=_TB9;}{struct Cyc_Warn_String_Warn_Warg_struct _TB9=_T45;void*_TBA[1];_T46=_TBA + 0;*_T46=& _TB9;_T48=Cyc_Warn_impos2;{int(*_TBB)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T48;_T47=_TBB;}_T49=_tag_fat(_TBA,sizeof(void*),1);_T47(_T49);}}_LL2F:;}_T4A=aggr_kind;_T4B=(int)_T4A;
# 1079
if(_T4B!=0)goto _TL168;{
struct Cyc_List_List*ps=0;
_TL16D: if(fields!=0)goto _TL16B;else{goto _TL16C;}
# 1083
_TL16B: _T4C=fields;_T4D=_T4C->hd;_T4E=(struct Cyc_Absyn_Aggrfield*)_T4D;_T4F=_T4E->name;_T50=*_T4F;_T51=_tag_fat("",sizeof(char),1U);_T52=Cyc_strcmp(_T50,_T51);{int found=_T52==0;{
struct Cyc_List_List*dlps0=dlps;_TL171: _T53=found;if(_T53)goto _TL170;else{goto _TL172;}_TL172: if(dlps0!=0)goto _TL16F;else{goto _TL170;}
_TL16F: _T54=dlps0;_T55=_T54->hd;{struct _tuple16*_TB5=(struct _tuple16*)_T55;struct Cyc_Absyn_Pat*_TB6;struct Cyc_List_List*_TB7;{struct _tuple16 _TB8=*_TB5;_TB7=_TB8.f0;_TB6=_TB8.f1;}{struct Cyc_List_List*dl=_TB7;struct Cyc_Absyn_Pat*p=_TB6;struct _fat_ptr*_TB8;if(dl==0)goto _TL173;_T56=dl;_T57=(struct Cyc_List_List*)_T56;_T58=_T57->hd;_T59=(int*)_T58;_T5A=*_T59;if(_T5A!=1)goto _TL175;_T5B=dl;_T5C=(struct Cyc_List_List*)_T5B;_T5D=_T5C->tl;if(_T5D!=0)goto _TL177;_T5E=dl;{struct Cyc_List_List _TB9=*_T5E;_T5F=_TB9.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBA=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T5F;_TB8=_TBA->f1;}}{struct _fat_ptr*f=_TB8;_T60=f;_T61=fields;_T62=_T61->hd;_T63=(struct Cyc_Absyn_Aggrfield*)_T62;_T64=_T63->name;_T65=
# 1088
Cyc_strptrcmp(_T60,_T64);if(_T65!=0)goto _TL179;{struct Cyc_List_List*_TB9=_cycalloc(sizeof(struct Cyc_List_List));
_TB9->hd=Cyc_Tcpat_compile_pat(p);_TB9->tl=ps;_T66=(struct Cyc_List_List*)_TB9;}ps=_T66;
found=1;goto _TL17A;_TL179: _TL17A: goto _LL39;}_TL177: goto _LL3C;_TL175: goto _LL3C;_TL173: _LL3C:{struct Cyc_Warn_String_Warn_Warg_struct _TB9;_TB9.tag=0;
# 1093
_TB9.f1=_tag_fat("bad designator(s)",sizeof(char),18U);_T67=_TB9;}{struct Cyc_Warn_String_Warn_Warg_struct _TB9=_T67;void*_TBA[1];_T68=_TBA + 0;*_T68=& _TB9;_T6A=Cyc_Warn_impos2;{int(*_TBB)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T6A;_T69=_TBB;}_T6B=_tag_fat(_TBA,sizeof(void*),1);_T69(_T6B);}_LL39:;}}_T6C=dlps0;
# 1084
dlps0=_T6C->tl;goto _TL171;_TL170:;}_T6D=found;
# 1096
if(_T6D)goto _TL17B;else{goto _TL17D;}
_TL17D:{struct Cyc_Warn_String_Warn_Warg_struct _TB5;_TB5.tag=0;_TB5.f1=_tag_fat("bad designator",sizeof(char),15U);_T6E=_TB5;}{struct Cyc_Warn_String_Warn_Warg_struct _TB5=_T6E;void*_TB6[1];_T6F=_TB6 + 0;*_T6F=& _TB5;_T71=Cyc_Warn_impos2;{int(*_TB7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T71;_T70=_TB7;}_T72=_tag_fat(_TB6,sizeof(void*),1);_T70(_T72);}goto _TL17C;_TL17B: _TL17C:;}_T73=fields;
# 1081
fields=_T73->tl;goto _TL16D;_TL16C: _T74=
# 1099
Cyc_List_imp_rev(ps);_T75=Cyc_Tcpat_pw(p);s=Cyc_Tcpat_tuple_pat(_T74,_T75);}goto _TL169;
# 1102
_TL168: _T76=is_tagged;if(_T76)goto _TL17E;else{goto _TL180;}
_TL180:{struct Cyc_Warn_String_Warn_Warg_struct _TB5;_TB5.tag=0;_TB5.f1=_tag_fat("patterns on untagged unions not yet supported.",sizeof(char),47U);_T77=_TB5;}{struct Cyc_Warn_String_Warn_Warg_struct _TB5=_T77;void*_TB6[1];_T78=_TB6 + 0;*_T78=& _TB5;_T79=p;_T7A=_T79->loc;_T7B=_tag_fat(_TB6,sizeof(void*),1);Cyc_Warn_err2(_T7A,_T7B);}goto _TL17F;_TL17E: _TL17F:{
int*span;span=_cycalloc_atomic(sizeof(int));_T7C=span;*_T7C=Cyc_List_length(fields);{struct Cyc_Absyn_Pat*_TB5;struct _fat_ptr*_TB6;if(dlps==0)goto _TL181;_T7D=dlps;_T7E=(struct Cyc_List_List*)_T7D;_T7F=_T7E->hd;_T80=(struct _tuple16*)_T7F;_T81=_T80->f0;if(_T81==0)goto _TL183;_T82=dlps;_T83=(struct Cyc_List_List*)_T82;_T84=_T83->hd;_T85=(struct _tuple16*)_T84;_T86=_T85->f0;_T87=(struct Cyc_List_List*)_T86;_T88=_T87->hd;_T89=(int*)_T88;_T8A=*_T89;if(_T8A!=1)goto _TL185;_T8B=dlps;_T8C=(struct Cyc_List_List*)_T8B;_T8D=_T8C->hd;_T8E=(struct _tuple16*)_T8D;_T8F=_T8E->f0;_T90=(struct Cyc_List_List*)_T8F;_T91=_T90->tl;if(_T91!=0)goto _TL187;_T92=dlps;_T93=(struct Cyc_List_List*)_T92;_T94=_T93->tl;if(_T94!=0)goto _TL189;_T95=dlps;{struct Cyc_List_List _TB7=*_T95;_T96=_TB7.hd;_T97=(struct _tuple16*)_T96;{struct _tuple16 _TB8=*_T97;_T98=_TB8.f0;{struct Cyc_List_List _TB9=*_T98;_T99=_TB9.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBA=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T99;_TB6=_TBA->f1;}}_TB5=_TB8.f1;}}{struct _fat_ptr*f=_TB6;struct Cyc_Absyn_Pat*p2=_TB5;_T9A=f;_T9B=*_T9A;_T9C=span;{struct Cyc_List_List*_TB7=_cycalloc(sizeof(struct Cyc_List_List));
# 1107
_TB7->hd=Cyc_Tcpat_compile_pat(p2);_TB7->tl=0;_T9D=(struct Cyc_List_List*)_TB7;}_T9E=p;s=Cyc_Tcpat_con_pat(_T9B,_T9C,_T9D,_T9E);goto _LL3E;}_TL189: goto _LL41;_TL187: goto _LL41;_TL185: goto _LL41;_TL183: goto _LL41;_TL181: _LL41:{struct Cyc_Warn_String_Warn_Warg_struct _TB7;_TB7.tag=0;
# 1109
_TB7.f1=_tag_fat("bad union pattern",sizeof(char),18U);_T9F=_TB7;}{struct Cyc_Warn_String_Warn_Warg_struct _TB7=_T9F;void*_TB8[1];_TA0=_TB8 + 0;*_TA0=& _TB7;_TA2=Cyc_Warn_impos2;{int(*_TB9)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TA2;_TA1=_TB9;}_TA3=_tag_fat(_TB8,sizeof(void*),1);_TA1(_TA3);}_LL3E:;}}_TL169: goto _LL0;}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_TAC;_TAD=_TB5->f2;}{struct Cyc_Absyn_Enumfield*ef=_TAD;_TAD=ef;goto _LL1C;}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_TB5=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_TAC;_TAD=_TB5->f2;}_LL1C: {struct Cyc_Absyn_Enumfield*ef=_TAD;_TA4=ef;_TA5=_TA4->name;_TA6=*_TA5;_TA7=_TA6.f1;_TA8=*_TA7;_TA9=p;
# 1120
s=Cyc_Tcpat_con_pat(_TA8,0,0,_TA9);goto _LL0;}case 14: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22:{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_TB5=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));
# 1124
_TB5->tag=0;_TAA=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_TB5;}s=(void*)_TAA;}_LL0:;}_TAB=s;
# 1126
return _TAB;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
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
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){struct Cyc_Tcpat_Con_s*_T0;struct _union_Name_value_Name_v _T1;unsigned _T2;struct _union_Name_value_Name_v _T3;struct Cyc_String_pa_PrintArg_struct _T4;void**_T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct _union_Name_value_Int_v _T9;struct Cyc_Int_pa_PrintArg_struct _TA;int _TB;void**_TC;struct Cyc___cycFILE*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;_T0=c;{
union Cyc_Tcpat_Name_value _T10=_T0->name;int _T11;struct _fat_ptr _T12;_T1=_T10.Name_v;_T2=_T1.tag;if(_T2!=1)goto _TL1AA;_T3=_T10.Name_v;_T12=_T3.val;{struct _fat_ptr s=_T12;{struct Cyc_String_pa_PrintArg_struct _T13;_T13.tag=0;
_T13.f1=s;_T4=_T13;}{struct Cyc_String_pa_PrintArg_struct _T13=_T4;void*_T14[1];_T5=_T14 + 0;*_T5=& _T13;_T6=Cyc_stderr;_T7=_tag_fat("%s",sizeof(char),3U);_T8=_tag_fat(_T14,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}goto _LL0;}_TL1AA: _T9=_T10.Int_v;_T11=_T9.val;{int i=_T11;{struct Cyc_Int_pa_PrintArg_struct _T13;_T13.tag=1;_TB=i;
_T13.f1=(unsigned long)_TB;_TA=_T13;}{struct Cyc_Int_pa_PrintArg_struct _T13=_TA;void*_T14[1];_TC=_T14 + 0;*_TC=& _T13;_TD=Cyc_stderr;_TE=_tag_fat("%d",sizeof(char),3U);_TF=_tag_fat(_T14,sizeof(void*),1);Cyc_fprintf(_TD,_TE,_TF);}goto _LL0;}_LL0:;}}
# 1205
static void Cyc_Tcpat_print_access(void*a){void*_T0;int*_T1;unsigned _T2;struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;void*_T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc_Absyn_Datatypefield*_TB;struct _tuple2*_TC;struct Cyc_Int_pa_PrintArg_struct _TD;unsigned _TE;int _TF;void**_T10;void**_T11;struct Cyc___cycFILE*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;void*_T15;int _T16;struct Cyc___cycFILE*_T17;struct _fat_ptr _T18;struct _fat_ptr _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct _fat_ptr*_T1B;void**_T1C;struct Cyc___cycFILE*_T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;struct _fat_ptr*_T20;int _T21;unsigned _T22;struct Cyc_Absyn_Datatypefield*_T23;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=Cyc_stderr;_T4=
# 1207
_tag_fat("DUMMY",sizeof(char),6U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3,_T4,_T5);goto _LL0;case 1: _T6=Cyc_stderr;_T7=
_tag_fat("*",sizeof(char),2U);_T8=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6,_T7,_T8);goto _LL0;case 2: _T9=a;{struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T24=(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_T9;_T23=_T24->f2;_T22=_T24->f3;}{struct Cyc_Absyn_Datatypefield*f=_T23;unsigned i=_T22;{struct Cyc_String_pa_PrintArg_struct _T24;_T24.tag=0;_TB=f;_TC=_TB->name;
# 1210
_T24.f1=Cyc_Absynpp_qvar2string(_TC);_TA=_T24;}{struct Cyc_String_pa_PrintArg_struct _T24=_TA;{struct Cyc_Int_pa_PrintArg_struct _T25;_T25.tag=1;_TE=i;_TF=(int)_TE;_T25.f1=(unsigned long)_TF;_TD=_T25;}{struct Cyc_Int_pa_PrintArg_struct _T25=_TD;void*_T26[2];_T10=_T26 + 0;*_T10=& _T24;_T11=_T26 + 1;*_T11=& _T25;_T12=Cyc_stderr;_T13=_tag_fat("%s[%d]",sizeof(char),7U);_T14=_tag_fat(_T26,sizeof(void*),2);Cyc_fprintf(_T12,_T13,_T14);}}goto _LL0;}default: _T15=a;{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T24=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T15;_T21=_T24->f2;_T20=_T24->f3;}{int tagged=_T21;struct _fat_ptr*f=_T20;_T16=tagged;
# 1212
if(!_T16)goto _TL1AD;_T17=Cyc_stderr;_T18=
_tag_fat(".tagunion",sizeof(char),10U);_T19=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T17,_T18,_T19);goto _TL1AE;_TL1AD: _TL1AE:{struct Cyc_String_pa_PrintArg_struct _T24;_T24.tag=0;_T1B=f;
_T24.f1=*_T1B;_T1A=_T24;}{struct Cyc_String_pa_PrintArg_struct _T24=_T1A;void*_T25[1];_T1C=_T25 + 0;*_T1C=& _T24;_T1D=Cyc_stderr;_T1E=_tag_fat(".%s",sizeof(char),4U);_T1F=_tag_fat(_T25,sizeof(void*),1);Cyc_fprintf(_T1D,_T1E,_T1F);}goto _LL0;}}_LL0:;}
# 1219
static void Cyc_Tcpat_print_pat_test(void*p){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T4;struct Cyc_Absyn_Exp*_T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;void*_T9;struct Cyc_String_pa_PrintArg_struct _TA;void**_TB;struct Cyc___cycFILE*_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc___cycFILE*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct Cyc___cycFILE*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;void*_T15;void*_T16;struct Cyc_String_pa_PrintArg_struct _T17;struct Cyc_Absyn_Enumfield*_T18;struct _tuple2*_T19;void**_T1A;struct Cyc___cycFILE*_T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;void*_T1E;struct Cyc_String_pa_PrintArg_struct _T1F;void**_T20;struct Cyc___cycFILE*_T21;struct _fat_ptr _T22;struct _fat_ptr _T23;void*_T24;struct Cyc_Int_pa_PrintArg_struct _T25;unsigned _T26;int _T27;void**_T28;struct Cyc___cycFILE*_T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;void*_T2C;struct Cyc_Int_pa_PrintArg_struct _T2D;int _T2E;void**_T2F;struct Cyc___cycFILE*_T30;struct _fat_ptr _T31;struct _fat_ptr _T32;void*_T33;struct Cyc_String_pa_PrintArg_struct _T34;struct _fat_ptr*_T35;struct Cyc_Int_pa_PrintArg_struct _T36;int _T37;void**_T38;void**_T39;struct Cyc___cycFILE*_T3A;struct _fat_ptr _T3B;struct _fat_ptr _T3C;void*_T3D;struct Cyc_String_pa_PrintArg_struct _T3E;struct Cyc_Absyn_Datatypefield*_T3F;struct _tuple2*_T40;void**_T41;struct Cyc___cycFILE*_T42;struct _fat_ptr _T43;struct _fat_ptr _T44;struct Cyc_Absyn_Datatypefield*_T45;struct _fat_ptr*_T46;int _T47;unsigned _T48;struct _fat_ptr _T49;struct Cyc_Absyn_Enumfield*_T4A;struct Cyc_Absyn_Exp*_T4B;_T0=p;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=p;_T4=(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_T3;_T5=_T4->f1;if(_T5!=0)goto _TL1B0;_T6=Cyc_stderr;_T7=
# 1221
_tag_fat("where(NULL)",sizeof(char),12U);_T8=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6,_T7,_T8);goto _LL0;_TL1B0: _T9=p;{struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_T4C=(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_T9;_T4B=_T4C->f1;}{struct Cyc_Absyn_Exp*e=_T4B;{struct Cyc_String_pa_PrintArg_struct _T4C;_T4C.tag=0;
_T4C.f1=Cyc_Absynpp_exp2string(e);_TA=_T4C;}{struct Cyc_String_pa_PrintArg_struct _T4C=_TA;void*_T4D[1];_TB=_T4D + 0;*_TB=& _T4C;_TC=Cyc_stderr;_TD=_tag_fat("where(%s)",sizeof(char),10U);_TE=_tag_fat(_T4D,sizeof(void*),1);Cyc_fprintf(_TC,_TD,_TE);}goto _LL0;}case 1: _TF=Cyc_stderr;_T10=
_tag_fat("NULL",sizeof(char),5U);_T11=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TF,_T10,_T11);goto _LL0;case 2: _T12=Cyc_stderr;_T13=
_tag_fat("NOT-NULL:",sizeof(char),10U);_T14=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T12,_T13,_T14);goto _LL0;case 4: _T15=p;{struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_T4C=(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_T15;_T4A=_T4C->f2;}{struct Cyc_Absyn_Enumfield*ef=_T4A;_T4A=ef;goto _LLC;}case 3: _T16=p;{struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_T4C=(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_T16;_T4A=_T4C->f2;}_LLC: {struct Cyc_Absyn_Enumfield*ef=_T4A;{struct Cyc_String_pa_PrintArg_struct _T4C;_T4C.tag=0;_T18=ef;_T19=_T18->name;
# 1226
_T4C.f1=Cyc_Absynpp_qvar2string(_T19);_T17=_T4C;}{struct Cyc_String_pa_PrintArg_struct _T4C=_T17;void*_T4D[1];_T1A=_T4D + 0;*_T1A=& _T4C;_T1B=Cyc_stderr;_T1C=_tag_fat("%s",sizeof(char),3U);_T1D=_tag_fat(_T4D,sizeof(void*),1);Cyc_fprintf(_T1B,_T1C,_T1D);}goto _LL0;}case 5: _T1E=p;{struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_T4C=(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_T1E;_T49=_T4C->f1;}{struct _fat_ptr s=_T49;{struct Cyc_String_pa_PrintArg_struct _T4C;_T4C.tag=0;
_T4C.f1=s;_T1F=_T4C;}{struct Cyc_String_pa_PrintArg_struct _T4C=_T1F;void*_T4D[1];_T20=_T4D + 0;*_T20=& _T4C;_T21=Cyc_stderr;_T22=_tag_fat("%s",sizeof(char),3U);_T23=_tag_fat(_T4D,sizeof(void*),1);Cyc_fprintf(_T21,_T22,_T23);}goto _LL0;}case 6: _T24=p;{struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_T4C=(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_T24;_T48=_T4C->f1;}{unsigned i=_T48;{struct Cyc_Int_pa_PrintArg_struct _T4C;_T4C.tag=1;_T26=i;_T27=(int)_T26;
_T4C.f1=(unsigned long)_T27;_T25=_T4C;}{struct Cyc_Int_pa_PrintArg_struct _T4C=_T25;void*_T4D[1];_T28=_T4D + 0;*_T28=& _T4C;_T29=Cyc_stderr;_T2A=_tag_fat("%d",sizeof(char),3U);_T2B=_tag_fat(_T4D,sizeof(void*),1);Cyc_fprintf(_T29,_T2A,_T2B);}goto _LL0;}case 7: _T2C=p;{struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_T4C=(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_T2C;_T47=_T4C->f1;}{int i=_T47;{struct Cyc_Int_pa_PrintArg_struct _T4C;_T4C.tag=1;_T2E=i;
_T4C.f1=(unsigned long)_T2E;_T2D=_T4C;}{struct Cyc_Int_pa_PrintArg_struct _T4C=_T2D;void*_T4D[1];_T2F=_T4D + 0;*_T2F=& _T4C;_T30=Cyc_stderr;_T31=_tag_fat("datatypetag(%d)",sizeof(char),16U);_T32=_tag_fat(_T4D,sizeof(void*),1);Cyc_fprintf(_T30,_T31,_T32);}goto _LL0;}case 8: _T33=p;{struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_T4C=(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_T33;_T46=_T4C->f2;_T47=_T4C->f3;}{struct _fat_ptr*f=_T46;int i=_T47;{struct Cyc_String_pa_PrintArg_struct _T4C;_T4C.tag=0;_T35=f;
_T4C.f1=*_T35;_T34=_T4C;}{struct Cyc_String_pa_PrintArg_struct _T4C=_T34;{struct Cyc_Int_pa_PrintArg_struct _T4D;_T4D.tag=1;_T37=i;_T4D.f1=(unsigned long)_T37;_T36=_T4D;}{struct Cyc_Int_pa_PrintArg_struct _T4D=_T36;void*_T4E[2];_T38=_T4E + 0;*_T38=& _T4C;_T39=_T4E + 1;*_T39=& _T4D;_T3A=Cyc_stderr;_T3B=_tag_fat("uniontag[%s](%d)",sizeof(char),17U);_T3C=_tag_fat(_T4E,sizeof(void*),2);Cyc_fprintf(_T3A,_T3B,_T3C);}}goto _LL0;}default: _T3D=p;{struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_T4C=(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_T3D;_T45=_T4C->f2;}{struct Cyc_Absyn_Datatypefield*f=_T45;{struct Cyc_String_pa_PrintArg_struct _T4C;_T4C.tag=0;_T3F=f;_T40=_T3F->name;
# 1232
_T4C.f1=Cyc_Absynpp_qvar2string(_T40);_T3E=_T4C;}{struct Cyc_String_pa_PrintArg_struct _T4C=_T3E;void*_T4D[1];_T41=_T4D + 0;*_T41=& _T4C;_T42=Cyc_stderr;_T43=_tag_fat("datatypefield(%s)",sizeof(char),18U);_T44=_tag_fat(_T4D,sizeof(void*),1);Cyc_fprintf(_T42,_T43,_T44);}}}_LL0:;}
# 1236
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){struct Cyc_String_pa_PrintArg_struct _T0;struct Cyc_Tcpat_Rhs*_T1;struct Cyc_Absyn_Stmt*_T2;void**_T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;{struct Cyc_String_pa_PrintArg_struct _T7;_T7.tag=0;_T1=r;_T2=_T1->rhs;
_T7.f1=Cyc_Absynpp_stmt2string(_T2);_T0=_T7;}{struct Cyc_String_pa_PrintArg_struct _T7=_T0;void*_T8[1];_T3=_T8 + 0;*_T3=& _T7;_T4=Cyc_stderr;_T5=_tag_fat("%s",sizeof(char),3U);_T6=_tag_fat(_T8,sizeof(void*),1);Cyc_fprintf(_T4,_T5,_T6);}}
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
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_T4;struct Cyc_Set_Set*(*_T5)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);struct Cyc_Set_Set*(*_T6)(struct Cyc_Set_Set*,void*);void*_T7;struct Cyc_Warn_String_Warn_Warg_struct _T8;void**_T9;int(*_TA)(struct _fat_ptr);void*(*_TB)(struct _fat_ptr);struct _fat_ptr _TC;struct Cyc_Set_Set*_TD;_T0=td;_T1=(int*)_T0;_T2=*_T1;if(_T2!=1)goto _TL1BD;_T3=td;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_TE=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_T3;_TD=_TE->f1;}{struct Cyc_Set_Set*cs=_TD;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_TE=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_TE->tag=1;_T6=Cyc_Set_insert;{
# 1289
struct Cyc_Set_Set*(*_TF)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_T6;_T5=_TF;}_TE->f1=_T5(cs,c);_T4=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_TE;}_T7=(void*)_T4;return _T7;}_TL1BD:{struct Cyc_Warn_String_Warn_Warg_struct _TE;_TE.tag=0;
_TE.f1=_tag_fat("add_neg called when td is Positive",sizeof(char),35U);_T8=_TE;}{struct Cyc_Warn_String_Warn_Warg_struct _TE=_T8;void*_TF[1];_T9=_TF + 0;*_T9=& _TE;_TB=Cyc_Warn_impos2;{int(*_T10)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TB;_TA=_T10;}_TC=_tag_fat(_TF,sizeof(void*),1);_TA(_TC);};}
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
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){struct Cyc_Warn_String_Warn_Warg_struct _T0;void**_T1;int(*_T2)(struct _fat_ptr);void*(*_T3)(struct _fat_ptr);struct _fat_ptr _T4;struct Cyc_List_List*_T5;void*_T6;struct _tuple22*_T7;struct Cyc_List_List*_T8;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_Tcpat_Con_s*_TE;if(ctxt!=0)goto _TL1C9;{struct Cyc_Warn_String_Warn_Warg_struct _TF;_TF.tag=0;
# 1329
_TF.f1=_tag_fat("norm_context: empty context",sizeof(char),28U);_T0=_TF;}{struct Cyc_Warn_String_Warn_Warg_struct _TF=_T0;void*_T10[1];_T1=_T10 + 0;*_T1=& _TF;_T3=Cyc_Warn_impos2;{int(*_T11)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T3;_T2=_T11;}_T4=_tag_fat(_T10,sizeof(void*),1);_T2(_T4);}goto _TL1CA;_TL1C9: _T5=ctxt;{struct Cyc_List_List _TF=*_T5;_T6=_TF.hd;_T7=(struct _tuple22*)_T6;{struct _tuple22 _T10=*_T7;_TE=_T10.f0;_TD=_T10.f1;}_TC=_TF.tl;}{struct Cyc_Tcpat_Con_s*c=_TE;struct Cyc_List_List*args=_TD;struct Cyc_List_List*tl=_TC;_T8=tl;{struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_TF=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct));_TF->tag=0;
_TF->f1=c;_TF->f2=Cyc_List_rev(args);_T9=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_TF;}_TA=(void*)_T9;_TB=Cyc_Tcpat_augment(_T8,_TA);return _TB;}_TL1CA:;}
# 1339
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){struct _tuple1 _T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;void**_T6;int(*_T7)(struct _fat_ptr);void*(*_T8)(struct _fat_ptr);struct _fat_ptr _T9;struct Cyc_List_List*_TA;void*_TB;struct _tuple22*_TC;struct Cyc_List_List*_TD;void*_TE;struct _tuple21*_TF;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T10;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T11;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T16;void*_T17;struct Cyc_List_List*_T18;void*_T19;{struct _tuple1 _T1A;
# 1341
_T1A.f0=ctxt;_T1A.f1=work;_T0=_T1A;}{struct _tuple1 _T1A=_T0;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_Tcpat_Con_s*_T1F;_T1=_T1A.f0;if(_T1!=0)goto _TL1CB;_T2=_T1A.f1;if(_T2!=0)goto _TL1CD;_T3=dsc;
return _T3;_TL1CD: goto _LL6;_TL1CB: _T4=_T1A.f1;if(_T4!=0)goto _TL1CF;_LL6:{struct Cyc_Warn_String_Warn_Warg_struct _T20;_T20.tag=0;
# 1344
_T20.f1=_tag_fat("build_desc: ctxt and work don't match",sizeof(char),38U);_T5=_T20;}{struct Cyc_Warn_String_Warn_Warg_struct _T20=_T5;void*_T21[1];_T6=_T21 + 0;*_T6=& _T20;_T8=Cyc_Warn_impos2;{int(*_T22)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T8;_T7=_T22;}_T9=_tag_fat(_T21,sizeof(void*),1);_T7(_T9);}goto _TL1D0;_TL1CF: _TA=_T1A.f0;{struct Cyc_List_List _T20=*_TA;_TB=_T20.hd;_TC=(struct _tuple22*)_TB;{struct _tuple22 _T21=*_TC;_T1F=_T21.f0;_T1E=_T21.f1;}_T1D=_T20.tl;}_TD=_T1A.f1;{struct Cyc_List_List _T20=*_TD;_TE=_T20.hd;_TF=(struct _tuple21*)_TE;{struct _tuple21 _T21=*_TF;_T1C=_T21.f2;}_T1B=_T20.tl;}{struct Cyc_Tcpat_Con_s*c=_T1F;struct Cyc_List_List*args=_T1E;struct Cyc_List_List*rest=_T1D;struct Cyc_List_List*dargs=_T1C;struct Cyc_List_List*work2=_T1B;
# 1346
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*td;td=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct));_T10=td;_T10->tag=0;_T11=td;_T11->f1=c;_T12=td;_T13=Cyc_List_rev(args);{struct Cyc_List_List*_T20=_cycalloc(sizeof(struct Cyc_List_List));_T20->hd=dsc;_T20->tl=dargs;_T14=(struct Cyc_List_List*)_T20;}_T12->f2=Cyc_List_append(_T13,_T14);_T15=rest;_T16=td;_T17=(void*)_T16;_T18=work2;_T19=
Cyc_Tcpat_build_desc(_T15,_T17,_T18);return _T19;}_TL1D0:;}}
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
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_T0;void*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct _tuple21*_T4;struct _tuple21*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct _tuple21*_T9;struct _tuple21*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct _tuple21*_TE;struct _tuple21*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_Tcpat_Rhs*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_List_List*_T17;void*_T18;struct _tuple21*_T19;struct Cyc_Warn_String_Warn_Warg_struct _T1A;void**_T1B;int(*_T1C)(struct _fat_ptr);void*(*_T1D)(struct _fat_ptr);struct _fat_ptr _T1E;void*_T1F;void*_T20;void*_T21;struct _tuple21*_T22;struct _tuple21*_T23;struct _tuple21*_T24;void*_T25;struct Cyc_List_List*_T26;void*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_Tcpat_Rhs*_T2A;struct Cyc_List_List*_T2B;void*_T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*_T30;if(work!=0)goto _TL1D3;{struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_T31=_cycalloc(sizeof(struct Cyc_Tcpat_Success_Tcpat_Decision_struct));_T31->tag=1;
# 1376
_T31->f1=right_hand_side;_T0=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_T31;}_T1=(void*)_T0;return _T1;_TL1D3: _T2=work;_T3=(struct Cyc_List_List*)_T2;_T4=_T3->hd;_T5=(struct _tuple21*)_T4;_T6=_T5->f0;if(_T6!=0)goto _TL1D5;_T7=work;_T8=(struct Cyc_List_List*)_T7;_T9=_T8->hd;_TA=(struct _tuple21*)_T9;_TB=_TA->f1;if(_TB!=0)goto _TL1D7;_TC=work;_TD=(struct Cyc_List_List*)_TC;_TE=_TD->hd;_TF=(struct _tuple21*)_TE;_T10=_TF->f2;if(_T10!=0)goto _TL1D9;_T11=work;{struct Cyc_List_List _T31=*_T11;_T30=_T31.tl;}{struct Cyc_List_List*workr=_T30;_T12=
# 1378
Cyc_Tcpat_norm_context(ctx);_T13=workr;_T14=right_hand_side;_T15=rules;_T16=Cyc_Tcpat_and_match(_T12,_T13,_T14,_T15);return _T16;}_TL1D9: goto _LL5;_TL1D7: goto _LL5;_TL1D5: _LL5: _T17=work;{struct Cyc_List_List _T31=*_T17;_T18=_T31.hd;_T19=(struct _tuple21*)_T18;{struct _tuple21 _T32=*_T19;_T30=_T32.f0;_T2F=_T32.f1;_T2E=_T32.f2;}_T2D=_T31.tl;}{struct Cyc_List_List*pats=_T30;struct Cyc_List_List*objs=_T2F;struct Cyc_List_List*dscs=_T2E;struct Cyc_List_List*workr=_T2D;
# 1380
if(pats==0)goto _TL1DD;else{goto _TL1DF;}_TL1DF: if(objs==0)goto _TL1DD;else{goto _TL1DE;}_TL1DE: if(dscs==0)goto _TL1DD;else{goto _TL1DB;}
_TL1DD:{struct Cyc_Warn_String_Warn_Warg_struct _T31;_T31.tag=0;_T31.f1=_tag_fat("tcpat:and_match: malformed work frame",sizeof(char),38U);_T1A=_T31;}{struct Cyc_Warn_String_Warn_Warg_struct _T31=_T1A;void*_T32[1];_T1B=_T32 + 0;*_T1B=& _T31;_T1D=Cyc_Warn_impos2;{int(*_T33)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1D;_T1C=_T33;}_T1E=_tag_fat(_T32,sizeof(void*),1);_T1C(_T1E);}goto _TL1DC;_TL1DB: _TL1DC: {
struct Cyc_List_List*_T31=pats;struct Cyc_List_List*_T32;void*_T33;{struct Cyc_List_List _T34=*_T31;_T1F=_T34.hd;_T33=(void*)_T1F;_T32=_T34.tl;}{void*pat1=_T33;struct Cyc_List_List*patr=_T32;
struct Cyc_List_List*_T34=objs;struct Cyc_List_List*_T35;struct Cyc_List_List*_T36;{struct Cyc_List_List _T37=*_T34;_T20=_T37.hd;_T36=(struct Cyc_List_List*)_T20;_T35=_T37.tl;}{struct Cyc_List_List*obj1=_T36;struct Cyc_List_List*objr=_T35;
struct Cyc_List_List*_T37=dscs;struct Cyc_List_List*_T38;void*_T39;{struct Cyc_List_List _T3A=*_T37;_T21=_T3A.hd;_T39=(void*)_T21;_T38=_T3A.tl;}{void*dsc1=_T39;struct Cyc_List_List*dscr=_T38;
struct _tuple21*wf;wf=_cycalloc(sizeof(struct _tuple21));_T22=wf;_T22->f0=patr;_T23=wf;_T23->f1=objr;_T24=wf;_T24->f2=dscr;_T25=pat1;_T26=obj1;_T27=dsc1;_T28=ctx;{struct Cyc_List_List*_T3A=_cycalloc(sizeof(struct Cyc_List_List));
_T3A->hd=wf;_T3A->tl=workr;_T29=(struct Cyc_List_List*)_T3A;}_T2A=right_hand_side;_T2B=rules;_T2C=Cyc_Tcpat_match(_T25,_T26,_T27,_T28,_T29,_T2A,_T2B);return _T2C;}}}}};}
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
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){union Cyc_Tcpat_PatOrWhere _T0;struct _union_PatOrWhere_where_clause _T1;unsigned _T2;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*_T3;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*_T4;void*_T5;union Cyc_Tcpat_PatOrWhere _T6;struct _union_PatOrWhere_pattern _T7;struct Cyc_Absyn_Pat*_T8;int*_T9;unsigned _TA;struct Cyc_Int_pa_PrintArg_struct _TB;int _TC;void**_TD;int(*_TE)(struct _fat_ptr,struct _fat_ptr);void*(*_TF)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T10;struct _fat_ptr _T11;struct Cyc_Tcpat_Deref_Tcpat_Access_struct*_T12;struct Cyc_Absyn_Pat*_T13;void*_T14;struct Cyc_List_List*_T15;struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T16;int _T17;struct Cyc_List_List*_T18;void*_T19;struct Cyc_Absyn_Pat*_T1A;void*_T1B;void*_T1C;void*_T1D;void*_T1E;int*_T1F;unsigned _T20;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T21;void*_T22;int*_T23;int _T24;void*_T25;struct Cyc_Absyn_Aggrdecl*_T26;struct Cyc_Absyn_AggrdeclImpl*_T27;struct Cyc_Absyn_AggrdeclImpl*_T28;struct Cyc_Absyn_Aggrdecl*_T29;struct Cyc_Absyn_AggrdeclImpl*_T2A;int(*_T2B)(struct _fat_ptr,struct _fat_ptr);void*(*_T2C)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2D;struct _fat_ptr _T2E;int _T2F;struct Cyc_List_List*_T30;void*_T31;struct _tuple16*_T32;struct _tuple16 _T33;struct Cyc_List_List*_T34;void*_T35;int*_T36;int _T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;void*_T3A;struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T3B;struct Cyc_List_List*_T3C;void*_T3D;struct _tuple16*_T3E;struct Cyc_Absyn_Pat*_T3F;void*_T40;void*_T41;struct Cyc_String_pa_PrintArg_struct _T42;void**_T43;int(*_T44)(struct _fat_ptr,struct _fat_ptr);void*(*_T45)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T46;struct _fat_ptr _T47;struct Cyc_List_List*_T48;struct Cyc_List_List*_T49;void*_T4A;struct Cyc_Absyn_Aggrfield*_T4B;struct Cyc_List_List*_T4C;void*_T4D;struct _tuple16*_T4E;struct Cyc_List_List*_T4F;void*_T50;int*_T51;int _T52;struct Cyc_List_List*_T53;struct Cyc_List_List*_T54;void*_T55;int _T56;struct Cyc_List_List*_T57;void*_T58;struct _tuple16*_T59;struct Cyc_Absyn_Pat*_T5A;void*_T5B;int(*_T5C)(struct _fat_ptr,struct _fat_ptr);void*(*_T5D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T5E;struct _fat_ptr _T5F;struct Cyc_List_List*_T60;struct Cyc_String_pa_PrintArg_struct _T61;struct _fat_ptr*_T62;struct _fat_ptr _T63;void**_T64;int(*_T65)(struct _fat_ptr,struct _fat_ptr);void*(*_T66)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T67;struct _fat_ptr _T68;struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T69;struct Cyc_List_List*_T6A;void*_T6B;struct Cyc_Absyn_Aggrfield*_T6C;void*_T6D;struct Cyc_String_pa_PrintArg_struct _T6E;void**_T6F;int(*_T70)(struct _fat_ptr,struct _fat_ptr);void*(*_T71)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T72;struct _fat_ptr _T73;struct Cyc_Absyn_Pat*_T74;_T0=pw;_T1=_T0.where_clause;_T2=_T1.tag;if(_T2!=2)goto _TL1E6;_T3=& Cyc_Tcpat_Dummy_val;_T4=(struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_T3;_T5=(void*)_T4;
# 1408
return _T5;_TL1E6: _T6=pw;_T7=_T6.pattern;_T74=_T7.val;{struct Cyc_Absyn_Pat*p=_T74;_T8=p;{
# 1410
void*_T75=_T8->r;struct Cyc_List_List*_T76;struct Cyc_Absyn_Datatypefield*_T77;void*_T78;_T9=(int*)_T75;_TA=*_T9;switch(_TA){case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T79=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T75;_T78=_T79->f1;}{struct Cyc_Absyn_Pat*p2=_T78;
# 1412
if(i==0)goto _TL1E9;{struct Cyc_Int_pa_PrintArg_struct _T79;_T79.tag=1;_TC=i;
_T79.f1=(unsigned long)_TC;_TB=_T79;}{struct Cyc_Int_pa_PrintArg_struct _T79=_TB;void*_T7A[1];_TD=_T7A + 0;*_TD=& _T79;_TF=Cyc_Warn_impos;{int(*_T7B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF;_TE=_T7B;}_T10=_tag_fat("get_access on pointer pattern with offset %d\n",sizeof(char),46U);_T11=_tag_fat(_T7A,sizeof(void*),1);_TE(_T10,_T11);}goto _TL1EA;_TL1E9: _TL1EA:{struct Cyc_Tcpat_Deref_Tcpat_Access_struct*_T79=_cycalloc(sizeof(struct Cyc_Tcpat_Deref_Tcpat_Access_struct));_T79->tag=1;_T13=p2;
_T79->f1=_T13->topt;_T12=(struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_T79;}_T14=(void*)_T12;return _T14;}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T79=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T75;_T78=_T79->f1;_T77=_T79->f2;_T76=_T79->f3;}{struct Cyc_Absyn_Datatypedecl*tud=_T78;struct Cyc_Absyn_Datatypefield*tuf=_T77;struct Cyc_List_List*args=_T76;
# 1416
int orig_i=i;
_TL1EE: if(i!=0)goto _TL1EC;else{goto _TL1ED;}_TL1EC: _T15=_check_null(args);args=_T15->tl;i=i + -1;goto _TL1EE;_TL1ED:{struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_T79=_cycalloc(sizeof(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct));_T79->tag=2;
_T79->f1=tud;_T79->f2=tuf;_T17=orig_i;_T79->f3=(unsigned)_T17;_T18=_check_null(args);_T19=_T18->hd;_T1A=(struct Cyc_Absyn_Pat*)_T19;_T1B=_T1A->topt;_T79->f4=_check_null(_T1B);_T16=(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_T79;}_T1C=(void*)_T16;return _T1C;}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T79=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T75;_T1D=_T79->f1;_T78=(void*)_T1D;_T76=_T79->f4;}{void*atype=_T78;struct Cyc_List_List*dlps=_T76;
# 1420
struct Cyc_List_List*fields;
int is_tagged=0;_T1E=
_check_null(atype);{void*_T79=Cyc_Absyn_compress(_T1E);struct Cyc_List_List*_T7A;union Cyc_Absyn_AggrInfo _T7B;_T1F=(int*)_T79;_T20=*_T1F;switch(_T20){case 0: _T21=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T79;_T22=_T21->f1;_T23=(int*)_T22;_T24=*_T23;if(_T24!=24)goto _TL1F0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T79;_T25=_T7C->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T7D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T25;_T7B=_T7D->f1;}}{union Cyc_Absyn_AggrInfo ainfo=_T7B;
# 1424
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);_T26=ad;_T27=_T26->impl;_T28=
_check_null(_T27);is_tagged=_T28->tagged;_T29=ad;_T2A=_T29->impl;
fields=_T2A->fields;goto _LLE;}_TL1F0: goto _LL13;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T7C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T79;_T7A=_T7C->f3;}{struct Cyc_List_List*fs=_T7A;
# 1429
fields=fs;goto _LLE;}default: _LL13: _T2C=Cyc_Warn_impos;{
# 1431
int(*_T7C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2C;_T2B=_T7C;}_T2D=_tag_fat("bad type in aggr pattern",sizeof(char),25U);_T2E=_tag_fat(0U,sizeof(void*),0);_T2B(_T2D,_T2E);}_LLE:;}_T2F=is_tagged;
# 1433
if(!_T2F)goto _TL1F2;_T30=
_check_null(dlps);_T31=_T30->hd;_T32=(struct _tuple16*)_T31;_T33=*_T32;{struct Cyc_List_List*_T79=_T33.f0;struct _fat_ptr*_T7A;if(_T79==0)goto _TL1F4;_T34=(struct Cyc_List_List*)_T79;_T35=_T34->hd;_T36=(int*)_T35;_T37=*_T36;if(_T37!=1)goto _TL1F6;_T38=(struct Cyc_List_List*)_T79;_T39=_T38->tl;if(_T39!=0)goto _TL1F8;{struct Cyc_List_List _T7B=*_T79;_T3A=_T7B.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T7C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T3A;_T7A=_T7C->f1;}}{struct _fat_ptr*f=_T7A;{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T7B=_cycalloc(sizeof(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct));_T7B->tag=3;
# 1436
_T7B->f1=atype;_T7B->f2=is_tagged;_T7B->f3=f;_T3C=dlps;_T3D=_T3C->hd;_T3E=(struct _tuple16*)_T3D;_T3F=_T3E->f1;_T40=_T3F->topt;_T7B->f4=_check_null(_T40);_T3B=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T7B;}_T41=(void*)_T3B;return _T41;}_TL1F8: goto _LL18;_TL1F6: goto _LL18;_TL1F4: _LL18:{struct Cyc_String_pa_PrintArg_struct _T7B;_T7B.tag=0;
# 1438
_T7B.f1=Cyc_Absynpp_pat2string(p);_T42=_T7B;}{struct Cyc_String_pa_PrintArg_struct _T7B=_T42;void*_T7C[1];_T43=_T7C + 0;*_T43=& _T7B;_T45=Cyc_Warn_impos;{int(*_T7D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T45;_T44=_T7D;}_T46=_tag_fat("get_access on bad aggr pattern: %s",sizeof(char),35U);_T47=_tag_fat(_T7C,sizeof(void*),1);_T44(_T46,_T47);};}goto _TL1F3;_TL1F2: _TL1F3: {
# 1441
int orig_i=i;
_TL1FD: if(i!=0)goto _TL1FB;else{goto _TL1FC;}_TL1FB: _T48=_check_null(fields);fields=_T48->tl;i=i + -1;goto _TL1FD;_TL1FC: _T49=
_check_null(fields);_T4A=_T49->hd;_T4B=(struct Cyc_Absyn_Aggrfield*)_T4A;{struct _fat_ptr*name=_T4B->name;
void*field_type=Cyc_Absyn_void_type;
_TL201: if(dlps!=0)goto _TL1FF;else{goto _TL200;}
_TL1FF: _T4C=dlps;_T4D=_T4C->hd;_T4E=(struct _tuple16*)_T4D;{struct Cyc_List_List*_T79=_T4E->f0;struct _fat_ptr*_T7A;if(_T79==0)goto _TL202;_T4F=(struct Cyc_List_List*)_T79;_T50=_T4F->hd;_T51=(int*)_T50;_T52=*_T51;if(_T52!=1)goto _TL204;_T53=(struct Cyc_List_List*)_T79;_T54=_T53->tl;if(_T54!=0)goto _TL206;{struct Cyc_List_List _T7B=*_T79;_T55=_T7B.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T7C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T55;_T7A=_T7C->f1;}}{struct _fat_ptr*f=_T7A;_T56=
# 1448
Cyc_strptrcmp(f,name);if(_T56==0)goto _TL208;goto _TL1FE;_TL208: _T57=dlps;_T58=_T57->hd;_T59=(struct _tuple16*)_T58;_T5A=_T59->f1;_T5B=_T5A->topt;
field_type=_check_null(_T5B);goto _LL1A;}_TL206: goto _LL1D;_TL204: goto _LL1D;_TL202: _LL1D: _T5D=Cyc_Warn_impos;{
# 1452
int(*_T7B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T5D;_T5C=_T7B;}_T5E=_tag_fat("get_access bad designator list",sizeof(char),31U);_T5F=_tag_fat(0U,sizeof(void*),0);_T5C(_T5E,_T5F);_LL1A:;}goto _TL200;_TL1FE: _T60=dlps;
# 1445
dlps=_T60->tl;goto _TL201;_TL200:
# 1456
 if(dlps!=0)goto _TL20A;{struct Cyc_String_pa_PrintArg_struct _T79;_T79.tag=0;_T62=name;_T63=*_T62;_T79.f1=_T63;_T61=_T79;}{struct Cyc_String_pa_PrintArg_struct _T79=_T61;void*_T7A[1];_T64=_T7A + 0;*_T64=& _T79;_T66=Cyc_Warn_impos;{int(*_T7B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T66;_T65=_T7B;}_T67=_tag_fat("get_access can't find field %s",sizeof(char),31U);_T68=_tag_fat(_T7A,sizeof(void*),1);_T65(_T67,_T68);}goto _TL20B;_TL20A: _TL20B:{struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_T79=_cycalloc(sizeof(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct));_T79->tag=3;
_T79->f1=atype;_T79->f2=is_tagged;_T6A=fields;_T6B=_T6A->hd;_T6C=(struct Cyc_Absyn_Aggrfield*)_T6B;_T79->f3=_T6C->name;_T79->f4=field_type;_T69=(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_T79;}_T6D=(void*)_T69;return _T6D;}}}default:{struct Cyc_String_pa_PrintArg_struct _T79;_T79.tag=0;
_T79.f1=Cyc_Absynpp_pat2string(p);_T6E=_T79;}{struct Cyc_String_pa_PrintArg_struct _T79=_T6E;void*_T7A[1];_T6F=_T7A + 0;*_T6F=& _T79;_T71=Cyc_Warn_impos;{int(*_T7B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T71;_T70=_T7B;}_T72=_tag_fat("get_access on bad pattern: %s",sizeof(char),30U);_T73=_tag_fat(_T7A,sizeof(void*),1);_T70(_T72,_T73);}};}};}struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Tcpat_Con_s*f1;};
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
static struct _tuple23*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){struct Cyc_Absyn_Switch_clause*_T0;struct Cyc_Absyn_Pat*_T1;struct Cyc_Tcpat_Rhs*_T2;struct Cyc_Tcpat_Rhs*_T3;struct Cyc_Absyn_Switch_clause*_T4;struct Cyc_Absyn_Pat*_T5;struct Cyc_Tcpat_Rhs*_T6;struct Cyc_Absyn_Switch_clause*_T7;struct Cyc_Absyn_Switch_clause*_T8;struct Cyc_Absyn_Exp*_T9;union Cyc_Tcpat_PatOrWhere _TA;struct Cyc_Absyn_Switch_clause*_TB;struct Cyc_List_List*_TC;void**_TD;void**_TE;int*_TF;int _T10;union Cyc_Tcpat_PatOrWhere _T11;struct _fat_ptr _T12;union Cyc_Tcpat_PatOrWhere _T13;int*_T14;int*_T15;int _T16;struct Cyc_List_List*_T17;void**_T18;void**_T19;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_T1A;struct _fat_ptr _T1B;union Cyc_Tcpat_PatOrWhere _T1C;struct _tuple23*_T1D;_T0=swc;_T1=_T0->pattern;{
void*sp0=Cyc_Tcpat_compile_pat(_T1);
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs));_T2=rhs;_T2->used=0;_T3=rhs;_T4=swc;_T5=_T4->pattern;_T3->pat_loc=_T5->loc;_T6=rhs;_T7=swc;_T6->rhs=_T7->body;{
void*sp;_T8=swc;_T9=_T8->where_clause;
if(_T9==0)goto _TL217;{union Cyc_Tcpat_PatOrWhere _T1E;_T1E.where_clause.tag=2U;_TB=swc;
_T1E.where_clause.val=_TB->where_clause;_TA=_T1E;}{union Cyc_Tcpat_PatOrWhere w=_TA;{void*_T1E[2];_TD=_T1E + 0;
*_TD=sp0;_TE=_T1E + 1;_TF=ctr;_T10=*_TF;_T11=w;*_TE=Cyc_Tcpat_int_pat(_T10,_T11);_T12=_tag_fat(_T1E,sizeof(void*),2);_TC=Cyc_List_list(_T12);}_T13=w;sp=Cyc_Tcpat_tuple_pat(_TC,_T13);_T14=ctr;_T15=ctr;_T16=*_T15;
*_T14=_T16 + 1;}goto _TL218;
# 1544
_TL217:{void*_T1E[2];_T18=_T1E + 0;*_T18=sp0;_T19=_T1E + 1;{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_T1F=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));_T1F->tag=0;_T1A=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_T1F;}*_T19=(void*)_T1A;_T1B=_tag_fat(_T1E,sizeof(void*),2);_T17=Cyc_List_list(_T1B);}{union Cyc_Tcpat_PatOrWhere _T1E;_T1E.where_clause.tag=2U;_T1E.where_clause.val=0;_T1C=_T1E;}sp=Cyc_Tcpat_tuple_pat(_T17,_T1C);_TL218:{struct _tuple23*_T1E=_cycalloc(sizeof(struct _tuple23));
_T1E->f0=sp;_T1E->f1=rhs;_T1D=(struct _tuple23*)_T1E;}return _T1D;}}}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1550
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1552
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Tcpat_Con_s*_T4;struct Cyc_Tcpat_Con_s*_T5;struct _union_PatOrWhere_where_clause _T6;unsigned _T7;struct Cyc_List_List*_T8;void*_T9;struct _fat_ptr _TA;struct _union_PatOrWhere_pattern _TB;struct Cyc_Absyn_Pat*_TC;int*_TD;unsigned _TE;struct _fat_ptr _TF;struct Cyc_Absyn_Vardecl*_T10;struct _tuple2*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_String_pa_PrintArg_struct _T14;struct Cyc_Absyn_Vardecl*_T15;struct _tuple2*_T16;void**_T17;struct _fat_ptr _T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;struct Cyc_String_pa_PrintArg_struct _T1B;struct Cyc_Absyn_Vardecl*_T1C;struct _tuple2*_T1D;struct Cyc_String_pa_PrintArg_struct _T1E;struct Cyc_Absyn_Tvar*_T1F;struct _fat_ptr*_T20;void**_T21;void**_T22;struct _fat_ptr _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_String_pa_PrintArg_struct _T26;void**_T27;struct _fat_ptr _T28;struct _fat_ptr _T29;void*_T2A;void*_T2B;int*_T2C;unsigned _T2D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2E;void*_T2F;int*_T30;int _T31;void*_T32;struct Cyc_Absyn_Aggrdecl*_T33;enum Cyc_Absyn_AggrKind _T34;int _T35;struct Cyc_List_List*_T36;struct Cyc_List_List*_T37;struct _tuple16*_T38;struct _tuple16*_T39;struct Cyc_List_List*_T3A;struct Cyc_List_List*_T3B;struct Cyc_List_List*_T3C;struct _tuple16*_T3D;struct _tuple16*_T3E;struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;void*_T41;int*_T42;int _T43;struct Cyc_List_List*_T44;void*_T45;struct _tuple16*_T46;struct Cyc_List_List*_T47;void*_T48;struct _fat_ptr _T49;struct Cyc_String_pa_PrintArg_struct _T4A;struct Cyc_Absyn_Aggrdecl*_T4B;struct _tuple2*_T4C;struct Cyc_String_pa_PrintArg_struct _T4D;struct _fat_ptr*_T4E;struct Cyc_String_pa_PrintArg_struct _T4F;void**_T50;void**_T51;void**_T52;struct _fat_ptr _T53;struct _fat_ptr _T54;struct _fat_ptr _T55;struct Cyc_String_pa_PrintArg_struct _T56;struct Cyc_Absyn_Aggrdecl*_T57;struct _tuple2*_T58;struct Cyc_String_pa_PrintArg_struct _T59;void**_T5A;void**_T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;int _T5E;struct _fat_ptr _T5F;struct Cyc_String_pa_PrintArg_struct _T60;void**_T61;struct _fat_ptr _T62;struct _fat_ptr _T63;struct _fat_ptr _T64;struct Cyc_String_pa_PrintArg_struct _T65;void**_T66;struct _fat_ptr _T67;struct _fat_ptr _T68;struct Cyc_Warn_String_Warn_Warg_struct _T69;void**_T6A;int(*_T6B)(struct _fat_ptr);void*(*_T6C)(struct _fat_ptr);struct _fat_ptr _T6D;struct _fat_ptr _T6E;struct Cyc_String_pa_PrintArg_struct _T6F;struct Cyc_Absyn_Datatypefield*_T70;struct _tuple2*_T71;void**_T72;struct _fat_ptr _T73;struct _fat_ptr _T74;struct _fat_ptr _T75;struct Cyc_String_pa_PrintArg_struct _T76;struct Cyc_Absyn_Datatypefield*_T77;struct _tuple2*_T78;struct Cyc_String_pa_PrintArg_struct _T79;void**_T7A;void**_T7B;struct _fat_ptr _T7C;struct _fat_ptr _T7D;struct _fat_ptr _T7E;struct _fat_ptr _T7F;struct Cyc_Int_pa_PrintArg_struct _T80;int _T81;void**_T82;struct _fat_ptr _T83;struct _fat_ptr _T84;struct _fat_ptr _T85;struct Cyc_Int_pa_PrintArg_struct _T86;char _T87;int _T88;void**_T89;struct _fat_ptr _T8A;struct _fat_ptr _T8B;struct _fat_ptr _T8C;struct Cyc_Absyn_Enumfield*_T8D;struct _tuple2*_T8E;struct _fat_ptr _T8F;struct _fat_ptr _T90;struct Cyc_Tcpat_Desc2string_exn_struct*_T91;struct Cyc_Tcpat_Desc2string_exn_struct*_T92;void*_T93;int _T94;struct _fat_ptr _T95;struct Cyc_Tcpat_Con_s*(*_T96)(struct Cyc_Set_Set*);void*(*_T97)(struct Cyc_Set_Set*);struct Cyc_Tcpat_Con_s*_T98;union Cyc_Tcpat_PatOrWhere _T99;struct _union_PatOrWhere_where_clause _T9A;unsigned _T9B;unsigned _T9C;int _T9D;struct Cyc_Tcpat_Desc2string_exn_struct*_T9E;struct Cyc_Tcpat_Desc2string_exn_struct*_T9F;struct Cyc_Absyn_Pat*_TA0;struct Cyc_Tcpat_Con_s*_TA1;struct _union_PatOrWhere_pattern _TA2;unsigned _TA3;struct _union_PatOrWhere_pattern _TA4;struct Cyc_Absyn_Pat*_TA5;void*_TA6;void*_TA7;int*_TA8;unsigned _TA9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAA;void*_TAB;int*_TAC;unsigned _TAD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TAE;void*_TAF;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TB0;enum Cyc_Absyn_Size_of _TB1;int(*_TB2)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_TB3)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_TB4;int _TB5;char _TB6;struct Cyc_Absyn_Pat*_TB7;struct Cyc_Tcpat_Con_s*_TB8;int _TB9;struct _fat_ptr _TBA;struct Cyc_Int_pa_PrintArg_struct _TBB;int _TBC;void**_TBD;struct _fat_ptr _TBE;struct _fat_ptr _TBF;struct Cyc_Tcpat_Desc2string_exn_struct*_TC0;struct Cyc_Tcpat_Desc2string_exn_struct*_TC1;int(*_TC2)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_TC3)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_TC4;unsigned _TC5;int _TC6;struct Cyc_Tcpat_Con_s*_TC7;union Cyc_Tcpat_PatOrWhere _TC8;struct Cyc_Tcpat_Con_s*_TC9;int _TCA;struct _fat_ptr _TCB;struct Cyc_Int_pa_PrintArg_struct _TCC;unsigned _TCD;int _TCE;void**_TCF;struct _fat_ptr _TD0;struct _fat_ptr _TD1;struct Cyc_Tcpat_Desc2string_exn_struct*_TD2;struct Cyc_Tcpat_Desc2string_exn_struct*_TD3;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD4;void*_TD5;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD6;union Cyc_Absyn_DatatypeInfo _TD7;struct _union_DatatypeInfo_KnownDatatype _TD8;unsigned _TD9;void*_TDA;union Cyc_Absyn_DatatypeInfo _TDB;struct _union_DatatypeInfo_KnownDatatype _TDC;struct Cyc_Absyn_Datatypedecl**_TDD;struct Cyc_Absyn_Datatypedecl*_TDE;int _TDF;struct Cyc_Tcpat_Desc2string_exn_struct*_TE0;struct Cyc_Tcpat_Desc2string_exn_struct*_TE1;struct Cyc_Absyn_Datatypedecl*_TE2;struct Cyc_Core_Opt*_TE3;struct Cyc_Core_Opt*_TE4;void*_TE5;struct Cyc_List_List*_TE6;unsigned _TE7;struct Cyc_List_List*_TE8;void*_TE9;struct Cyc_Absyn_Datatypefield*_TEA;struct _tuple2*_TEB;struct _tuple2 _TEC;struct _fat_ptr*_TED;struct Cyc_List_List*_TEE;void*_TEF;struct Cyc_Absyn_Datatypefield*_TF0;int(*_TF1)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_TF2)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_TF3;struct Cyc_Tcpat_Con_s*_TF4;struct Cyc_Tcpat_Con_s*_TF5;int _TF6;struct _fat_ptr _TF7;int _TF8;struct _fat_ptr _TF9;struct Cyc_String_pa_PrintArg_struct _TFA;void**_TFB;struct _fat_ptr _TFC;struct _fat_ptr _TFD;struct Cyc_List_List*_TFE;struct Cyc_Tcpat_Desc2string_exn_struct*_TFF;struct Cyc_Tcpat_Desc2string_exn_struct*_T100;void*_T101;struct Cyc_Absyn_Aggrdecl*_T102;enum Cyc_Absyn_AggrKind _T103;int _T104;struct Cyc_Tcpat_Desc2string_exn_struct*_T105;struct Cyc_Tcpat_Desc2string_exn_struct*_T106;struct Cyc_Absyn_Aggrdecl*_T107;struct Cyc_Absyn_AggrdeclImpl*_T108;struct Cyc_Absyn_AggrdeclImpl*_T109;struct Cyc_Absyn_Aggrdecl*_T10A;struct _fat_ptr*_T10B;struct Cyc_List_List*_T10C;unsigned _T10D;struct Cyc_List_List*_T10E;void*_T10F;struct Cyc_Absyn_Aggrfield*_T110;struct _fat_ptr*_T111;int(*_T112)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_T113)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T114;struct Cyc_Tcpat_Con_s*_T115;struct Cyc_Tcpat_Con_s*_T116;int _T117;struct _fat_ptr _T118;struct Cyc_String_pa_PrintArg_struct _T119;struct Cyc_String_pa_PrintArg_struct _T11A;void**_T11B;void**_T11C;struct _fat_ptr _T11D;struct _fat_ptr _T11E;struct Cyc_List_List*_T11F;struct Cyc_Tcpat_Desc2string_exn_struct*_T120;struct Cyc_Tcpat_Desc2string_exn_struct*_T121;struct Cyc_Absyn_PtrInfo _T122;struct Cyc_Absyn_PtrAtts _T123;int _T124;int(*_T125)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*);int(*_T126)(struct Cyc_Set_Set*,void*);struct Cyc_Set_Set*_T127;struct Cyc_Tcpat_Con_s*_T128;int _T129;struct _fat_ptr _T12A;struct _fat_ptr _T12B;struct Cyc_Tcpat_Desc2string_exn_struct*_T12C;struct Cyc_Tcpat_Desc2string_exn_struct*_T12D;struct Cyc_Set_Set*_T12E;struct Cyc_List_List*_T12F;struct Cyc_Tcpat_Con_s*_T130;_T0=d;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL219;_T3=d;{struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_T131=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_T3;_T130=_T131->f1;_T12F=_T131->f2;}{struct Cyc_Tcpat_Con_s*c=_T130;struct Cyc_List_List*ds=_T12F;_T4=c;{
# 1556
union Cyc_Tcpat_Name_value n=_T4->name;
struct Cyc_Absyn_Pat*p;_T5=c;{
union Cyc_Tcpat_PatOrWhere _T131=_T5->orig_pat;struct Cyc_Absyn_Pat*_T132;_T6=_T131.where_clause;_T7=_T6.tag;if(_T7!=2)goto _TL21B;_T8=
_check_null(ds);_T9=_T8->hd;_TA=Cyc_Tcpat_desc2string(_T9);return _TA;_TL21B: _TB=_T131.pattern;_T132=_TB.val;{struct Cyc_Absyn_Pat*p2=_T132;
p=p2;goto _LL5;}_LL5:;}_TC=p;{
# 1562
void*_T131=_TC->r;struct Cyc_Absyn_Exp*_T132;struct Cyc_Absyn_Enumfield*_T133;struct _fat_ptr _T134;char _T135;int _T136;struct Cyc_Absyn_Datatypefield*_T137;struct Cyc_List_List*_T138;struct Cyc_Absyn_Vardecl*_T139;void*_T13A;_TD=(int*)_T131;_TE=*_TD;switch(_TE){case 0: _TF=
_tag_fat("_",sizeof(char),2U);return _TF;case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T131;_T13A=_T13B->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13A;_T10=vd;_T11=_T10->name;_T12=
Cyc_Absynpp_qvar2string(_T11);return _T12;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T131;_T13A=_T13B->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13A;{struct Cyc_String_pa_PrintArg_struct _T13B;_T13B.tag=0;_T15=vd;_T16=_T15->name;
_T13B.f1=Cyc_Absynpp_qvar2string(_T16);_T14=_T13B;}{struct Cyc_String_pa_PrintArg_struct _T13B=_T14;void*_T13C[1];_T17=_T13C + 0;*_T17=& _T13B;_T18=_tag_fat("*%s",sizeof(char),4U);_T19=_tag_fat(_T13C,sizeof(void*),1);_T13=Cyc_aprintf(_T18,_T19);}return _T13;}case 4:{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T131;_T13A=_T13B->f1;_T139=_T13B->f2;}{struct Cyc_Absyn_Tvar*tv=_T13A;struct Cyc_Absyn_Vardecl*vd=_T139;{struct Cyc_String_pa_PrintArg_struct _T13B;_T13B.tag=0;_T1C=vd;_T1D=_T1C->name;
_T13B.f1=Cyc_Absynpp_qvar2string(_T1D);_T1B=_T13B;}{struct Cyc_String_pa_PrintArg_struct _T13B=_T1B;{struct Cyc_String_pa_PrintArg_struct _T13C;_T13C.tag=0;_T1F=tv;_T20=_T1F->name;
_T13C.f1=*_T20;_T1E=_T13C;}{struct Cyc_String_pa_PrintArg_struct _T13C=_T1E;void*_T13D[2];_T21=_T13D + 0;*_T21=& _T13B;_T22=_T13D + 1;*_T22=& _T13C;_T23=
# 1566
_tag_fat("%s<`%s>",sizeof(char),8U);_T24=_tag_fat(_T13D,sizeof(void*),2);_T1A=Cyc_aprintf(_T23,_T24);}}return _T1A;}case 5:{struct Cyc_String_pa_PrintArg_struct _T13B;_T13B.tag=0;
# 1568
_T13B.f1=Cyc_Tcpat_descs2string(ds);_T26=_T13B;}{struct Cyc_String_pa_PrintArg_struct _T13B=_T26;void*_T13C[1];_T27=_T13C + 0;*_T27=& _T13B;_T28=_tag_fat("&%s",sizeof(char),4U);_T29=_tag_fat(_T13C,sizeof(void*),1);_T25=Cyc_aprintf(_T28,_T29);}return _T25;case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T131;_T2A=_T13B->f1;_T13A=(void*)_T2A;_T138=_T13B->f4;}{void*atype=_T13A;struct Cyc_List_List*dlps=_T138;_T2B=
# 1570
_check_null(atype);{void*_T13B=Cyc_Absyn_compress(_T2B);int _T13C;union Cyc_Absyn_AggrInfo _T13D;_T2C=(int*)_T13B;_T2D=*_T2C;switch(_T2D){case 0: _T2E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13B;_T2F=_T2E->f1;_T30=(int*)_T2F;_T31=*_T30;if(_T31!=24)goto _TL21F;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T13E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13B;_T32=_T13E->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T13F=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T32;_T13D=_T13F->f1;}}{union Cyc_Absyn_AggrInfo ainfo=_T13D;
# 1572
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);_T33=ad;_T34=_T33->kind;_T35=(int)_T34;
if(_T35!=1)goto _TL221;{struct _fat_ptr*_T13E;if(dlps==0)goto _TL223;_T36=dlps;_T37=(struct Cyc_List_List*)_T36;_T38=_T37->hd;_T39=(struct _tuple16*)_T38;_T3A=_T39->f0;if(_T3A==0)goto _TL225;_T3B=dlps;_T3C=(struct Cyc_List_List*)_T3B;_T3D=_T3C->hd;_T3E=(struct _tuple16*)_T3D;_T3F=_T3E->f0;_T40=(struct Cyc_List_List*)_T3F;_T41=_T40->hd;_T42=(int*)_T41;_T43=*_T42;if(_T43!=1)goto _TL227;_T44=dlps;{struct Cyc_List_List _T13F=*_T44;_T45=_T13F.hd;_T46=(struct _tuple16*)_T45;{struct _tuple16 _T140=*_T46;_T47=_T140.f0;{struct Cyc_List_List _T141=*_T47;_T48=_T141.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T142=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T48;_T13E=_T142->f1;}}}}{struct _fat_ptr*f=_T13E;{struct Cyc_String_pa_PrintArg_struct _T13F;_T13F.tag=0;_T4B=ad;_T4C=_T4B->name;
# 1576
_T13F.f1=Cyc_Absynpp_qvar2string(_T4C);_T4A=_T13F;}{struct Cyc_String_pa_PrintArg_struct _T13F=_T4A;{struct Cyc_String_pa_PrintArg_struct _T140;_T140.tag=0;_T4E=f;_T140.f1=*_T4E;_T4D=_T140;}{struct Cyc_String_pa_PrintArg_struct _T140=_T4D;{struct Cyc_String_pa_PrintArg_struct _T141;_T141.tag=0;
_T141.f1=Cyc_Tcpat_descs2string(ds);_T4F=_T141;}{struct Cyc_String_pa_PrintArg_struct _T141=_T4F;void*_T142[3];_T50=_T142 + 0;*_T50=& _T13F;_T51=_T142 + 1;*_T51=& _T140;_T52=_T142 + 2;*_T52=& _T141;_T53=
# 1576
_tag_fat("%s{.%s = %s}",sizeof(char),13U);_T54=_tag_fat(_T142,sizeof(void*),3);_T49=Cyc_aprintf(_T53,_T54);}}}return _T49;}_TL227: goto _LL33;_TL225: goto _LL33;_TL223: _LL33: goto _LL30;_LL30:;}goto _TL222;_TL221: _TL222:{struct Cyc_String_pa_PrintArg_struct _T13E;_T13E.tag=0;_T57=ad;_T58=_T57->name;
# 1580
_T13E.f1=Cyc_Absynpp_qvar2string(_T58);_T56=_T13E;}{struct Cyc_String_pa_PrintArg_struct _T13E=_T56;{struct Cyc_String_pa_PrintArg_struct _T13F;_T13F.tag=0;_T13F.f1=Cyc_Tcpat_descs2string(ds);_T59=_T13F;}{struct Cyc_String_pa_PrintArg_struct _T13F=_T59;void*_T140[2];_T5A=_T140 + 0;*_T5A=& _T13E;_T5B=_T140 + 1;*_T5B=& _T13F;_T5C=_tag_fat("%s{%s}",sizeof(char),7U);_T5D=_tag_fat(_T140,sizeof(void*),2);_T55=Cyc_aprintf(_T5C,_T5D);}}return _T55;}_TL21F: goto _LL2E;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T13E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T13B;_T13C=_T13E->f2;}{int is_tuple=_T13C;_T5E=is_tuple;
# 1582
if(!_T5E)goto _TL229;{struct Cyc_String_pa_PrintArg_struct _T13E;_T13E.tag=0;_T13E.f1=Cyc_Tcpat_descs2string(ds);_T60=_T13E;}{struct Cyc_String_pa_PrintArg_struct _T13E=_T60;void*_T13F[1];_T61=_T13F + 0;*_T61=& _T13E;_T62=_tag_fat("$(%s)",sizeof(char),6U);_T63=_tag_fat(_T13F,sizeof(void*),1);_T5F=Cyc_aprintf(_T62,_T63);}return _T5F;
_TL229:{struct Cyc_String_pa_PrintArg_struct _T13E;_T13E.tag=0;_T13E.f1=Cyc_Tcpat_descs2string(ds);_T65=_T13E;}{struct Cyc_String_pa_PrintArg_struct _T13E=_T65;void*_T13F[1];_T66=_T13F + 0;*_T66=& _T13E;_T67=_tag_fat("{%s}",sizeof(char),5U);_T68=_tag_fat(_T13F,sizeof(void*),1);_T64=Cyc_aprintf(_T67,_T68);}return _T64;}default: _LL2E:{struct Cyc_Warn_String_Warn_Warg_struct _T13E;_T13E.tag=0;
_T13E.f1=_tag_fat("bad type in aggr_p",sizeof(char),19U);_T69=_T13E;}{struct Cyc_Warn_String_Warn_Warg_struct _T13E=_T69;void*_T13F[1];_T6A=_T13F + 0;*_T6A=& _T13E;_T6C=Cyc_Warn_impos2;{int(*_T140)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T6C;_T6B=_T140;}_T6D=_tag_fat(_T13F,sizeof(void*),1);_T6B(_T6D);}};}}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T131;_T137=_T13B->f2;}{struct Cyc_Absyn_Datatypefield*tuf=_T137;
# 1587
if(ds!=0)goto _TL22B;{struct Cyc_String_pa_PrintArg_struct _T13B;_T13B.tag=0;_T70=tuf;_T71=_T70->name;
_T13B.f1=Cyc_Absynpp_qvar2string(_T71);_T6F=_T13B;}{struct Cyc_String_pa_PrintArg_struct _T13B=_T6F;void*_T13C[1];_T72=_T13C + 0;*_T72=& _T13B;_T73=_tag_fat("%s",sizeof(char),3U);_T74=_tag_fat(_T13C,sizeof(void*),1);_T6E=Cyc_aprintf(_T73,_T74);}return _T6E;
# 1590
_TL22B:{struct Cyc_String_pa_PrintArg_struct _T13B;_T13B.tag=0;_T77=tuf;_T78=_T77->name;_T13B.f1=Cyc_Absynpp_qvar2string(_T78);_T76=_T13B;}{struct Cyc_String_pa_PrintArg_struct _T13B=_T76;{struct Cyc_String_pa_PrintArg_struct _T13C;_T13C.tag=0;_T13C.f1=Cyc_Tcpat_descs2string(ds);_T79=_T13C;}{struct Cyc_String_pa_PrintArg_struct _T13C=_T79;void*_T13D[2];_T7A=_T13D + 0;*_T7A=& _T13B;_T7B=_T13D + 1;*_T7B=& _T13C;_T7C=_tag_fat("%s(%s)",sizeof(char),7U);_T7D=_tag_fat(_T13D,sizeof(void*),2);_T75=Cyc_aprintf(_T7C,_T7D);}}return _T75;}case 8: _T7E=
_tag_fat("NULL",sizeof(char),5U);return _T7E;case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T131;_T136=_T13B->f2;}{int i=_T136;{struct Cyc_Int_pa_PrintArg_struct _T13B;_T13B.tag=1;_T81=i;
_T13B.f1=(unsigned long)_T81;_T80=_T13B;}{struct Cyc_Int_pa_PrintArg_struct _T13B=_T80;void*_T13C[1];_T82=_T13C + 0;*_T82=& _T13B;_T83=_tag_fat("%d",sizeof(char),3U);_T84=_tag_fat(_T13C,sizeof(void*),1);_T7F=Cyc_aprintf(_T83,_T84);}return _T7F;}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T131;_T135=_T13B->f1;}{char c=_T135;{struct Cyc_Int_pa_PrintArg_struct _T13B;_T13B.tag=1;_T87=c;_T88=(int)_T87;
_T13B.f1=(unsigned long)_T88;_T86=_T13B;}{struct Cyc_Int_pa_PrintArg_struct _T13B=_T86;void*_T13C[1];_T89=_T13C + 0;*_T89=& _T13B;_T8A=_tag_fat("%d",sizeof(char),3U);_T8B=_tag_fat(_T13C,sizeof(void*),1);_T85=Cyc_aprintf(_T8A,_T8B);}return _T85;}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T131;_T134=_T13B->f1;_T136=_T13B->f2;}{struct _fat_ptr f=_T134;int i=_T136;_T8C=f;
return _T8C;}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T131;_T133=_T13B->f2;}{struct Cyc_Absyn_Enumfield*ef=_T133;_T133=ef;goto _LL24;}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T131;_T133=_T13B->f2;}_LL24: {struct Cyc_Absyn_Enumfield*ef=_T133;_T8D=ef;_T8E=_T8D->name;_T8F=
# 1596
Cyc_Absynpp_qvar2string(_T8E);return _T8F;}case 16:{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T13B=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T131;_T132=_T13B->f1;}{struct Cyc_Absyn_Exp*e=_T132;_T90=
Cyc_Absynpp_exp2string(e);return _T90;}default: _T91=& Cyc_Tcpat_Desc2string_val;_T92=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T91;_throw(_T92);};}}}goto _TL21A;_TL219: _T93=d;{struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_T131=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_T93;_T12E=_T131->f1;}{struct Cyc_Set_Set*s=_T12E;_T94=
# 1601
Cyc_Set_is_empty(s);if(!_T94)goto _TL22D;_T95=_tag_fat("_",sizeof(char),2U);return _T95;_TL22D: _T97=Cyc_Set_choose;{
struct Cyc_Tcpat_Con_s*(*_T131)(struct Cyc_Set_Set*)=(struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*))_T97;_T96=_T131;}{struct Cyc_Tcpat_Con_s*c=_T96(s);_T98=c;_T99=_T98->orig_pat;_T9A=_T99.where_clause;_T9B=_T9A.tag;_T9C=_T9B==2;_T9D=(int)_T9C;
# 1605
if(!_T9D)goto _TL22F;_T9E=& Cyc_Tcpat_Desc2string_val;_T9F=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T9E;_throw(_T9F);goto _TL230;_TL22F: _TL230: _TA1=c;{union Cyc_Tcpat_PatOrWhere _T131=_TA1->orig_pat;_TA2=_T131.pattern;_TA3=_TA2.tag;if(_TA3==1)goto _TL231;_throw_match();goto _TL232;_TL231: _TL232: _TA4=_T131.pattern;_TA0=_TA4.val;}{
struct Cyc_Absyn_Pat*orig_pat=_TA0;_TA5=orig_pat;_TA6=_TA5->topt;_TA7=
_check_null(_TA6);{void*_T131=Cyc_Absyn_compress(_TA7);union Cyc_Absyn_AggrInfo _T132;void*_T133;_TA8=(int*)_T131;_TA9=*_TA8;switch(_TA9){case 0: _TAA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T131;_TAB=_TAA->f1;_TAC=(int*)_TAB;_TAD=*_TAC;switch(_TAD){case 1: _TAE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T131;_TAF=_TAE->f1;_TB0=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TAF;_TB1=_TB0->f2;if(_TB1!=Cyc_Absyn_Char_sz)goto _TL235;{
# 1610
int i=0;_TL23A: if(i < 256)goto _TL238;else{goto _TL239;}
_TL238: _TB3=Cyc_Set_member;{int(*_T134)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_TB3;_TB2=_T134;}_TB4=s;_TB5=i;_TB6=(char)_TB5;_TB7=orig_pat;_TB8=Cyc_Tcpat_char_con(_TB6,_TB7);_TB9=_TB2(_TB4,_TB8);if(_TB9)goto _TL23B;else{goto _TL23D;}
_TL23D:{struct Cyc_Int_pa_PrintArg_struct _T134;_T134.tag=1;_TBC=i;_T134.f1=(unsigned long)_TBC;_TBB=_T134;}{struct Cyc_Int_pa_PrintArg_struct _T134=_TBB;void*_T135[1];_TBD=_T135 + 0;*_TBD=& _T134;_TBE=_tag_fat("%d",sizeof(char),3U);_TBF=_tag_fat(_T135,sizeof(void*),1);_TBA=Cyc_aprintf(_TBE,_TBF);}return _TBA;_TL23B:
# 1610
 i=i + 1;goto _TL23A;_TL239:;}_TC0=& Cyc_Tcpat_Desc2string_val;_TC1=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TC0;_throw(_TC1);goto _TL236;_TL235:{
# 1616
unsigned i=0U;_TL241: if(i < 4294967295U)goto _TL23F;else{goto _TL240;}
_TL23F: _TC3=Cyc_Set_member;{int(*_T134)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_TC3;_TC2=_T134;}_TC4=s;_TC5=i;_TC6=(int)_TC5;_TC7=c;_TC8=_TC7->orig_pat;_TC9=Cyc_Tcpat_int_con(_TC6,_TC8);_TCA=_TC2(_TC4,_TC9);if(_TCA)goto _TL242;else{goto _TL244;}
_TL244:{struct Cyc_Int_pa_PrintArg_struct _T134;_T134.tag=1;_TCD=i;_TCE=(int)_TCD;_T134.f1=(unsigned long)_TCE;_TCC=_T134;}{struct Cyc_Int_pa_PrintArg_struct _T134=_TCC;void*_T135[1];_TCF=_T135 + 0;*_TCF=& _T134;_TD0=_tag_fat("%d",sizeof(char),3U);_TD1=_tag_fat(_T135,sizeof(void*),1);_TCB=Cyc_aprintf(_TD0,_TD1);}return _TCB;_TL242:
# 1616
 i=i + 1;goto _TL241;_TL240:;}_TD2=& Cyc_Tcpat_Desc2string_val;_TD3=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TD2;_throw(_TD3);_TL236:;case 22: _TD4=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T131;_TD5=_TD4->f1;_TD6=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TD5;_TD7=_TD6->f1;_TD8=_TD7.KnownDatatype;_TD9=_TD8.tag;if(_TD9!=2)goto _TL245;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T134=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T131;_TDA=_T134->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T135=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TDA;_TDB=_T135->f1;_TDC=_TDB.KnownDatatype;_TDD=_TDC.val;{struct Cyc_Absyn_Datatypedecl*_T136=*_TDD;_T133=_T136;}}}{struct Cyc_Absyn_Datatypedecl*tud=_T133;_TDE=tud;_TDF=_TDE->is_extensible;
# 1625
if(!_TDF)goto _TL247;_TE0=& Cyc_Tcpat_Desc2string_val;_TE1=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TE0;_throw(_TE1);goto _TL248;_TL247: _TL248: _TE2=tud;_TE3=_TE2->fields;_TE4=
_check_null(_TE3);_TE5=_TE4->v;{struct Cyc_List_List*fields=(struct Cyc_List_List*)_TE5;
int span=Cyc_List_length(fields);
_TL24C: _TE6=fields;_TE7=(unsigned)_TE6;if(_TE7)goto _TL24A;else{goto _TL24B;}
_TL24A: _TE8=fields;_TE9=_TE8->hd;_TEA=(struct Cyc_Absyn_Datatypefield*)_TE9;_TEB=_TEA->name;_TEC=*_TEB;_TED=_TEC.f1;{struct _fat_ptr n=*_TED;_TEE=fields;_TEF=_TEE->hd;_TF0=(struct Cyc_Absyn_Datatypefield*)_TEF;{
struct Cyc_List_List*args=_TF0->typs;_TF2=Cyc_Set_member;{
int(*_T134)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_TF2;_TF1=_T134;}_TF3=s;{struct Cyc_Tcpat_Con_s*_T134=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T134->name=Cyc_Tcpat_Name_v(n);_T134->arity=0;_T134->span=0;_TF5=c;_T134->orig_pat=_TF5->orig_pat;_TF4=(struct Cyc_Tcpat_Con_s*)_T134;}_TF6=_TF1(_TF3,_TF4);if(_TF6)goto _TL24D;else{goto _TL24F;}
_TL24F: _TF8=Cyc_List_length(args);if(_TF8!=0)goto _TL250;_TF7=n;goto _TL251;_TL250:{struct Cyc_String_pa_PrintArg_struct _T134;_T134.tag=0;_T134.f1=n;_TFA=_T134;}{struct Cyc_String_pa_PrintArg_struct _T134=_TFA;void*_T135[1];_TFB=_T135 + 0;*_TFB=& _T134;_TFC=_tag_fat("%s(...)",sizeof(char),8U);_TFD=_tag_fat(_T135,sizeof(void*),1);_TF9=Cyc_aprintf(_TFC,_TFD);}_TF7=_TF9;_TL251: return _TF7;_TL24D:;}}_TFE=fields;
# 1628
fields=_TFE->tl;goto _TL24C;_TL24B: _TFF=& Cyc_Tcpat_Desc2string_val;_T100=(struct Cyc_Tcpat_Desc2string_exn_struct*)_TFF;_throw(_T100);}}goto _TL246;_TL245: goto _LL40;_TL246:;case 24:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T134=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T131;_T101=_T134->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T135=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T101;_T132=_T135->f1;}}{union Cyc_Absyn_AggrInfo ainfo=_T132;
# 1636
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(ainfo);_T102=ad;_T103=_T102->kind;_T104=(int)_T103;
if(_T104==1)goto _TL252;_T105=& Cyc_Tcpat_Desc2string_val;_T106=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T105;_throw(_T106);goto _TL253;_TL252: _TL253: _T107=ad;_T108=_T107->impl;_T109=
_check_null(_T108);{struct Cyc_List_List*fields=_T109->fields;
int span=Cyc_List_length(fields);_T10A=ad;{
struct _tuple2*_T134=_T10A->name;struct _fat_ptr _T135;{struct _tuple2 _T136=*_T134;_T10B=_T136.f1;{struct _fat_ptr _T137=*_T10B;_T135=_T137;}}{struct _fat_ptr union_name=_T135;
_TL257: _T10C=fields;_T10D=(unsigned)_T10C;if(_T10D)goto _TL255;else{goto _TL256;}
_TL255: _T10E=fields;_T10F=_T10E->hd;_T110=(struct Cyc_Absyn_Aggrfield*)_T10F;_T111=_T110->name;{struct _fat_ptr n=*_T111;_T113=Cyc_Set_member;{
int(*_T136)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_T113;_T112=_T136;}_T114=s;{struct Cyc_Tcpat_Con_s*_T136=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));_T136->name=Cyc_Tcpat_Name_v(n);_T136->arity=0;_T136->span=0;_T116=c;_T136->orig_pat=_T116->orig_pat;_T115=(struct Cyc_Tcpat_Con_s*)_T136;}_T117=_T112(_T114,_T115);if(_T117)goto _TL258;else{goto _TL25A;}
_TL25A:{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;_T136.f1=union_name;_T119=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_T119;{struct Cyc_String_pa_PrintArg_struct _T137;_T137.tag=0;_T137.f1=n;_T11A=_T137;}{struct Cyc_String_pa_PrintArg_struct _T137=_T11A;void*_T138[2];_T11B=_T138 + 0;*_T11B=& _T136;_T11C=_T138 + 1;*_T11C=& _T137;_T11D=_tag_fat("%s{.%s = _}",sizeof(char),12U);_T11E=_tag_fat(_T138,sizeof(void*),2);_T118=Cyc_aprintf(_T11D,_T11E);}}return _T118;_TL258:;}_T11F=fields;
# 1641
fields=_T11F->tl;goto _TL257;_TL256: _T120=& Cyc_Tcpat_Desc2string_val;_T121=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T120;_throw(_T121);}}}}default: goto _LL40;};case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T134=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T131;_T122=_T134->f1;_T123=_T122.ptr_atts;_T133=_T123.nullable;}{void*n=_T133;_T124=
# 1621
Cyc_Tcutil_force_type2bool(0,n);if(!_T124)goto _TL25B;_T126=Cyc_Set_member;{int(*_T134)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*)=(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))_T126;_T125=_T134;}_T127=s;_T128=Cyc_Tcpat_null_con(orig_pat);_T129=_T125(_T127,_T128);if(_T129)goto _TL25B;else{goto _TL25D;}
_TL25D: _T12A=_tag_fat("NULL",sizeof(char),5U);return _T12A;_TL25B: _T12B=
_tag_fat("&_",sizeof(char),3U);return _T12B;}default: _LL40: _T12C=& Cyc_Tcpat_Desc2string_val;_T12D=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T12C;_throw(_T12D);};}}}}_TL21A:;}
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
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){struct _handler_cons*_T0;int*_T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;void**_T5;void**_T6;unsigned _T7;struct _fat_ptr _T8;void*_T9;struct Cyc_Tcpat_Desc2string_exn_struct*_TA;char*_TB;char*_TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;void**_TE;unsigned _TF;struct _fat_ptr _T10;struct _handler_cons _T11;_T0=& _T11;_push_handler(_T0);{int _T12=0;_T1=_T11.handler;_T2=setjmp(_T1);if(!_T2)goto _TL264;_T12=1;goto _TL265;_TL264: _TL265: if(_T12)goto _TL266;else{goto _TL268;}_TL268:{
# 1667
struct _fat_ptr s=Cyc_Tcpat_desc2string(desc);{struct Cyc_Warn_String_Warn_Warg_struct _T13;_T13.tag=0;
_T13.f1=_tag_fat("patterns may not be exhaustive.\n\tmissing case for ",sizeof(char),51U);_T3=_T13;}{struct Cyc_Warn_String_Warn_Warg_struct _T13=_T3;{struct Cyc_Warn_String_Warn_Warg_struct _T14;_T14.tag=0;_T14.f1=s;_T4=_T14;}{struct Cyc_Warn_String_Warn_Warg_struct _T14=_T4;void*_T15[2];_T5=_T15 + 0;*_T5=& _T13;_T6=_T15 + 1;*_T6=& _T14;_T7=loc;_T8=_tag_fat(_T15,sizeof(void*),2);Cyc_Warn_err2(_T7,_T8);}}}_pop_handler();goto _TL267;_TL266: _T9=Cyc_Core_get_exn_thrown();{void*_T13=(void*)_T9;void*_T14;_TA=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T13;_TB=_TA->tag;_TC=Cyc_Tcpat_Desc2string;if(_TB!=_TC)goto _TL269;{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;
# 1670
_T15.f1=_tag_fat("patterns may not be exhaustive.",sizeof(char),32U);_TD=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_TD;void*_T16[1];_TE=_T16 + 0;*_TE=& _T15;_TF=loc;_T10=_tag_fat(_T16,sizeof(void*),1);Cyc_Warn_err2(_TF,_T10);}goto _LL0;_TL269: _T14=_T13;{void*exn=_T14;_rethrow(exn);}_LL0:;}_TL267:;}}
# 1674
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){struct Cyc_Tcpat_Rhs*_T0;_T0=rhs;
_T0->used=1;}
# 1678
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){struct Cyc_List_List*(*_T0)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*,void*),void*,struct Cyc_List_List*);int*_T2;struct Cyc_List_List*_T3;void**_T4;void(*_T5)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*);void*_T6;unsigned _T7;int*_T8;struct Cyc_List_List*_T9;void*_TA;struct Cyc_Tcpat_Rhs*_TB;int _TC;struct Cyc_Warn_String_Warn_Warg_struct _TD;void**_TE;unsigned _TF;struct _fat_ptr _T10;struct Cyc_List_List*_T11;
# 1684
int where_ctr=0;
int*env=& where_ctr;_T1=Cyc_List_map_c;{
struct Cyc_List_List*(*_T12)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*))_T1;_T0=_T12;}_T2=env;_T3=swcs;{struct Cyc_List_List*match_rules=_T0(Cyc_Tcpat_get_match,_T2,_T3);
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);_T4=dopt;
*_T4=dec_tree;{
# 1690
int ex_done=0;{
void(*_T12)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*)=(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;_T5=_T12;}_T6=dec_tree;_T7=loc;_T8=& ex_done;_T5(_T6,Cyc_Tcpat_not_exhaust_err,_T7,Cyc_Tcpat_rule_occurs,0,_T8);
# 1693
_TL26E: if(match_rules!=0)goto _TL26C;else{goto _TL26D;}
_TL26C: _T9=match_rules;_TA=_T9->hd;{struct _tuple23*_T12=(struct _tuple23*)_TA;unsigned _T13;int _T14;{struct _tuple23 _T15=*_T12;_TB=_T15.f1;{struct Cyc_Tcpat_Rhs _T16=*_TB;_T14=_T16.used;_T13=_T16.pat_loc;}}{int b=_T14;unsigned loc2=_T13;_TC=b;
if(_TC)goto _TL26F;else{goto _TL271;}
_TL271:{struct Cyc_Warn_String_Warn_Warg_struct _T15;_T15.tag=0;_T15.f1=_tag_fat("redundant pattern (check for misspelled constructors in earlier patterns)",sizeof(char),74U);_TD=_T15;}{struct Cyc_Warn_String_Warn_Warg_struct _T15=_TD;void*_T16[1];_TE=_T16 + 0;*_TE=& _T15;_TF=loc2;_T10=_tag_fat(_T16,sizeof(void*),1);Cyc_Warn_err2(_TF,_T10);}goto _TL26D;_TL26F:;}}_T11=match_rules;
# 1693
match_rules=_T11->tl;goto _TL26E;_TL26D:;}}}
# 1707
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple15*pr,void*desc){struct _tuple15*_T0;struct _handler_cons*_T1;int*_T2;int _T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;struct Cyc_Warn_String_Warn_Warg_struct _T5;void**_T6;void**_T7;struct _tuple15*_T8;struct _tuple15 _T9;unsigned _TA;struct _fat_ptr _TB;void*_TC;struct Cyc_Tcpat_Desc2string_exn_struct*_TD;char*_TE;char*_TF;struct Cyc_Warn_String_Warn_Warg_struct _T10;void**_T11;struct _tuple15*_T12;struct _tuple15 _T13;unsigned _T14;struct _fat_ptr _T15;_T0=pr;
(*_T0).f1=0;{struct _handler_cons _T16;_T1=& _T16;_push_handler(_T1);{int _T17=0;_T2=_T16.handler;_T3=setjmp(_T2);if(!_T3)goto _TL272;_T17=1;goto _TL273;_TL272: _TL273: if(_T17)goto _TL274;else{goto _TL276;}_TL276:{
# 1710
struct _fat_ptr s=Cyc_Tcpat_desc2string(desc);{struct Cyc_Warn_String_Warn_Warg_struct _T18;_T18.tag=0;
_T18.f1=_tag_fat("pattern not exhaustive.\n\tmissing case for ",sizeof(char),43U);_T4=_T18;}{struct Cyc_Warn_String_Warn_Warg_struct _T18=_T4;{struct Cyc_Warn_String_Warn_Warg_struct _T19;_T19.tag=0;_T19.f1=s;_T5=_T19;}{struct Cyc_Warn_String_Warn_Warg_struct _T19=_T5;void*_T1A[2];_T6=_T1A + 0;*_T6=& _T18;_T7=_T1A + 1;*_T7=& _T19;_T8=pr;_T9=*_T8;_TA=_T9.f0;_TB=_tag_fat(_T1A,sizeof(void*),2);Cyc_Warn_warn2(_TA,_TB);}}}_pop_handler();goto _TL275;_TL274: _TC=Cyc_Core_get_exn_thrown();{void*_T18=(void*)_TC;void*_T19;_TD=(struct Cyc_Tcpat_Desc2string_exn_struct*)_T18;_TE=_TD->tag;_TF=Cyc_Tcpat_Desc2string;if(_TE!=_TF)goto _TL277;{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;
# 1713
_T1A.f1=_tag_fat("pattern not exhaustive.",sizeof(char),24U);_T10=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_T10;void*_T1B[1];_T11=_T1B + 0;*_T11=& _T1A;_T12=pr;_T13=*_T12;_T14=_T13.f0;_T15=_tag_fat(_T1B,sizeof(void*),1);Cyc_Warn_warn2(_T14,_T15);}goto _LL0;_TL277: _T19=_T18;{void*exn=_T19;_rethrow(exn);}_LL0:;}_TL275:;}}}
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
static struct _tuple23*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){struct Cyc_Absyn_Switch_clause*_T0;struct Cyc_Absyn_Pat*_T1;struct Cyc_Absyn_Switch_clause*_T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_Warn_String_Warn_Warg_struct _T4;void**_T5;struct Cyc_Absyn_Switch_clause*_T6;struct Cyc_Absyn_Exp*_T7;unsigned _T8;struct _fat_ptr _T9;struct Cyc_Tcpat_Rhs*_TA;struct Cyc_Tcpat_Rhs*_TB;struct Cyc_Absyn_Switch_clause*_TC;struct Cyc_Absyn_Pat*_TD;struct Cyc_Tcpat_Rhs*_TE;struct Cyc_Absyn_Switch_clause*_TF;struct _tuple23*_T10;_T0=swc;_T1=_T0->pattern;{
void*sp0=Cyc_Tcpat_compile_pat(_T1);_T2=swc;_T3=_T2->where_clause;
# 1741
if(_T3==0)goto _TL279;{struct Cyc_Warn_String_Warn_Warg_struct _T11;_T11.tag=0;
_T11.f1=_tag_fat("&&-clauses not supported in exception handlers.",sizeof(char),48U);_T4=_T11;}{struct Cyc_Warn_String_Warn_Warg_struct _T11=_T4;void*_T12[1];_T5=_T12 + 0;*_T5=& _T11;_T6=swc;_T7=_T6->where_clause;_T8=_T7->loc;_T9=_tag_fat(_T12,sizeof(void*),1);Cyc_Warn_err2(_T8,_T9);}goto _TL27A;_TL279: _TL27A: {
# 1744
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs));_TA=rhs;_TA->used=0;_TB=rhs;_TC=swc;_TD=_TC->pattern;_TB->pat_loc=_TD->loc;_TE=rhs;_TF=swc;_TE->rhs=_TF->body;{struct _tuple23*_T11=_cycalloc(sizeof(struct _tuple23));
_T11->f0=sp0;_T11->f1=rhs;_T10=(struct _tuple23*)_T11;}return _T10;}}}
# 1747
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1750
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){struct Cyc_List_List*(*_T0)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T2;void**_T3;void(*_T4)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*);void*_T5;unsigned _T6;int*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_Tcpat_Rhs*_TC;int _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;void**_TF;unsigned _T10;struct _fat_ptr _T11;struct Cyc_List_List*_T12;_T1=Cyc_List_map;{
# 1754
struct Cyc_List_List*(*_T13)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))_T1;_T0=_T13;}_T2=swcs;{struct Cyc_List_List*match_rules=_T0(Cyc_Tcpat_get_match2,_T2);
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);_T3=dopt;
*_T3=dec_tree;{
int ex_done=0;{
void(*_T13)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*)=(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;_T4=_T13;}_T5=dec_tree;_T6=loc;_T7=& ex_done;_T4(_T5,Cyc_Tcpat_not_exhaust_err2,_T6,Cyc_Tcpat_rule_occurs,0,_T7);
# 1760
_TL27E: if(match_rules!=0)goto _TL27C;else{goto _TL27D;}
# 1762
_TL27C: _T8=match_rules;_T9=_T8->tl;if(_T9!=0)goto _TL27F;goto _TL27D;_TL27F: _TA=match_rules;_TB=_TA->hd;{
struct _tuple23*_T13=(struct _tuple23*)_TB;unsigned _T14;int _T15;{struct _tuple23 _T16=*_T13;_TC=_T16.f1;{struct Cyc_Tcpat_Rhs _T17=*_TC;_T15=_T17.used;_T14=_T17.pat_loc;}}{int b=_T15;unsigned loc2=_T14;_TD=b;
if(_TD)goto _TL281;else{goto _TL283;}
_TL283:{struct Cyc_Warn_String_Warn_Warg_struct _T16;_T16.tag=0;_T16.f1=_tag_fat("redundant pattern (check for misspelled constructors in earlier patterns)",sizeof(char),74U);_TE=_T16;}{struct Cyc_Warn_String_Warn_Warg_struct _T16=_TE;void*_T17[1];_TF=_T17 + 0;*_TF=& _T16;_T10=loc2;_T11=_tag_fat(_T17,sizeof(void*),1);Cyc_Warn_err2(_T10,_T11);}goto _TL27D;_TL281:;}}_T12=match_rules;
# 1760
match_rules=_T12->tl;goto _TL27E;_TL27D:;}}}
# 1771
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){struct Cyc_Core_Opt*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple17*_T4;struct _tuple17 _T5;struct Cyc_Absyn_Vardecl**_T6;struct Cyc_List_List*_T7;_T0=
_check_null(pat_vars);_T1=_T0->v;{struct Cyc_List_List*l=(struct Cyc_List_List*)_T1;_TL287: if(l!=0)goto _TL285;else{goto _TL286;}
_TL285: _T2=l;_T3=_T2->hd;_T4=(struct _tuple17*)_T3;_T5=*_T4;_T6=_T5.f0;if(_T6==0)goto _TL288;
return 1;_TL288: _T7=l;
# 1772
l=_T7->tl;goto _TL287;_TL286:;}
# 1775
return 0;}

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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 147 "dict.h"
extern void Cyc_Dict_iter(void(*)(void*,void*),struct Cyc_Dict_Dict);
# 212
extern struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 222
extern int Cyc_Dict_forall_intersect(int(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 283
extern struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 911 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 939
void*Cyc_Absyn_compress(void*);
# 974
extern void*Cyc_Absyn_void_type;
# 1229
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1231
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 54 "tcutil.h"
int Cyc_Tcutil_is_bits_only_type(void*);
# 134 "tcutil.h"
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 143
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 195
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*,void*,int);
# 253
int Cyc_Tcutil_force_type2bool(int,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 41 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 40 "cf_flowinfo.h"
extern int Cyc_CfFlowInfo_anal_error;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 73
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};char Cyc_CfFlowInfo_IsZero[7U]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8U]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_UnknownZ[9U]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;int f4;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _union_FlowInfo_ReachableFL{int tag;struct Cyc_Dict_Dict val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163 "cf_flowinfo.h"
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 173
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*);
# 177
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 179
void Cyc_CfFlowInfo_print_absrval(void*);
# 39 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 42
Cyc_CfFlowInfo_anal_error=1;
Cyc_Warn_verr(loc,fmt,ap);}
# 47
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_NotZero_val={Cyc_CfFlowInfo_NotZero};
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_UnknownZ_val={Cyc_CfFlowInfo_UnknownZ};
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){union Cyc_CfFlowInfo_AbsLVal _T0;{union Cyc_CfFlowInfo_AbsLVal _T1;_T1.PlaceL.tag=1U;
_T1.PlaceL.val=x;_T0=_T1;}return _T0;}
# 53
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void){union Cyc_CfFlowInfo_AbsLVal _T0;{union Cyc_CfFlowInfo_AbsLVal _T1;_T1.UnknownL.tag=2U;
_T1.UnknownL.val=0;_T0=_T1;}return _T0;}
# 56
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void){union Cyc_CfFlowInfo_FlowInfo _T0;{union Cyc_CfFlowInfo_FlowInfo _T1;_T1.BottomFL.tag=1U;_T1.BottomFL.val=0;_T0=_T1;}return _T0;}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd){union Cyc_CfFlowInfo_FlowInfo _T0;{union Cyc_CfFlowInfo_FlowInfo _T1;_T1.ReachableFL.tag=2U;
_T1.ReachableFL.val=fd;_T0=_T1;}return _T0;}
# 61
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void){struct Cyc_CfFlowInfo_FlowEnv*_T0;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_T1;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_T2;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T3;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T4;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_T5;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_T6;int(*_T7)(void*,void*);struct Cyc_CfFlowInfo_Place*_T8;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T9;struct Cyc_Absyn_Exp*_TA;
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)& dummy_rawexp),0U,(void*)((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)& Cyc_Absyn_EmptyAnnot_val)};{struct Cyc_CfFlowInfo_FlowEnv*_TB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_FlowEnv));{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_TC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct));
# 65
_TC->tag=0;_T1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_TC;}_TB->zero=(void*)_T1;{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_TC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct));
_TC->tag=1;_T2=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_TC;}_TB->notzeroall=(void*)_T2;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_TC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct));_TC->tag=2;
_TC->f1=0U;_T3=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TC;}_TB->unknown_none=(void*)_T3;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_TC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct));_TC->tag=2;
_TC->f1=1U;_T4=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_TC;}_TB->unknown_all=(void*)_T4;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_TC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct));_TC->tag=3;
_TC->f1=0U;_T5=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_TC;}_TB->esc_none=(void*)_T5;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_TC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct));_TC->tag=3;
_TC->f1=1U;_T6=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_TC;}_TB->esc_all=(void*)_T6;_T7=Cyc_CfFlowInfo_root_cmp;
_TB->mt_flowdict=Cyc_Dict_empty(_T7);{struct Cyc_CfFlowInfo_Place*_TC=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_TD=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_TD->tag=1;_TA=& dummy_exp;
_TD->f1=(struct Cyc_Absyn_Exp*)_TA;_TD->f2=Cyc_Absyn_void_type;_T9=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_TD;}_TC->root=(void*)_T9;_TC->path=0;_T8=(struct Cyc_CfFlowInfo_Place*)_TC;}_TB->dummy_place=_T8;_T0=(struct Cyc_CfFlowInfo_FlowEnv*)_TB;}
# 64
return _T0;}
# 76
struct _fat_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){struct Cyc_CfFlowInfo_Place*_T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Absyn_Vardecl*_T5;struct _tuple0*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_String_pa_PrintArg_struct _T9;struct Cyc_Absyn_Vardecl*_TA;struct _tuple0*_TB;void**_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;int(*_TF)(struct _fat_ptr,struct _fat_ptr);void*(*_T10)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T11;struct _fat_ptr _T12;_T0=place;{
struct Cyc_CfFlowInfo_Place _T13=*_T0;struct Cyc_List_List*_T14;void*_T15;_T15=_T13.root;_T14=_T13.path;{void*root=_T15;struct Cyc_List_List*fields=_T14;struct Cyc_Absyn_Vardecl*_T16;_T1=root;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL0;_T4=root;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T17=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_T4;_T16=_T17->f1;}{struct Cyc_Absyn_Vardecl*vd=_T16;
# 80
if(fields!=0)goto _TL2;_T5=vd;_T6=_T5->name;_T7=
Cyc_Absynpp_qvar2string(_T6);return _T7;
# 83
_TL2:{struct Cyc_String_pa_PrintArg_struct _T17;_T17.tag=0;_TA=vd;_TB=_TA->name;_T17.f1=Cyc_Absynpp_qvar2string(_TB);_T9=_T17;}{struct Cyc_String_pa_PrintArg_struct _T17=_T9;void*_T18[1];_TC=_T18 + 0;*_TC=& _T17;_TD=_tag_fat("reachable from %s",sizeof(char),18U);_TE=_tag_fat(_T18,sizeof(void*),1);_T8=Cyc_aprintf(_TD,_TE);}return _T8;}_TL0: _T10=Cyc_Warn_impos;{
int(*_T17)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T10;_TF=_T17;}_T11=_tag_fat("error locations not for VarRoots",sizeof(char),33U);_T12=_tag_fat(0U,sizeof(void*),0);_TF(_T11,_T12);;}}}
# 89
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Aggrfield*_T4;int _T5;int _T6;struct Cyc_List_List*_T7;struct Cyc_String_pa_PrintArg_struct _T8;struct _fat_ptr*_T9;void**_TA;int(*_TB)(struct _fat_ptr,struct _fat_ptr);void*(*_TC)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TD;struct _fat_ptr _TE;
int n=0;
_TL7: _T0=fs;_T1=(unsigned)_T0;if(_T1)goto _TL5;else{goto _TL6;}
_TL5: _T2=fs;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Aggrfield*)_T3;{struct _fat_ptr*f2=_T4->name;_T5=
Cyc_strptrcmp(f2,f);if(_T5!=0)goto _TL8;_T6=n;return _T6;_TL8:
 n=n + 1;}_T7=fs;
# 91
fs=_T7->tl;goto _TL7;_TL6:{struct Cyc_String_pa_PrintArg_struct _TF;_TF.tag=0;_T9=f;
# 96
_TF.f1=*_T9;_T8=_TF;}{struct Cyc_String_pa_PrintArg_struct _TF=_T8;void*_T10[1];_TA=_T10 + 0;*_TA=& _TF;_TC=Cyc_Warn_impos;{int(*_T11)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TC;_TB=_T11;}_TD=_tag_fat("get_field_index_fs failed: %s",sizeof(char),30U);_TE=_tag_fat(_T10,sizeof(void*),1);_TB(_TD,_TE);}}
# 99
int Cyc_CfFlowInfo_get_field_index(void*t,struct _fat_ptr*f){int*_T0;unsigned _T1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct Cyc_Absyn_Aggrdecl*_T7;struct Cyc_Absyn_AggrdeclImpl*_T8;struct Cyc_Absyn_AggrdeclImpl*_T9;int _TA;struct Cyc_String_pa_PrintArg_struct _TB;void**_TC;int(*_TD)(struct _fat_ptr,struct _fat_ptr);void*(*_TE)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TF;struct _fat_ptr _T10;
void*_T11=Cyc_Absyn_compress(t);struct Cyc_List_List*_T12;union Cyc_Absyn_AggrInfo _T13;_T0=(int*)_T11;_T1=*_T0;switch(_T1){case 0: _T2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T3=_T2->f1;_T4=(int*)_T3;_T5=*_T4;if(_T5!=24)goto _TLB;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T14=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T6=_T14->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T15=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6;_T13=_T15->f1;}}{union Cyc_Absyn_AggrInfo info=_T13;_T7=
# 102
Cyc_Absyn_get_known_aggrdecl(info);_T8=_T7->impl;_T9=_check_null(_T8);{struct Cyc_List_List*fs=_T9->fields;_T12=fs;goto _LL4;}}_TLB: goto _LL5;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T14=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T11;_T12=_T14->f3;}_LL4: {struct Cyc_List_List*fs=_T12;_TA=
# 105
Cyc_CfFlowInfo_get_field_index_fs(fs,f);return _TA;}default: _LL5:{struct Cyc_String_pa_PrintArg_struct _T14;_T14.tag=0;
# 107
_T14.f1=Cyc_Absynpp_typ2string(t);_TB=_T14;}{struct Cyc_String_pa_PrintArg_struct _T14=_TB;void*_T15[1];_TC=_T15 + 0;*_TC=& _T14;_TE=Cyc_Warn_impos;{int(*_T16)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TE;_TD=_T16;}_TF=_tag_fat("get_field_index failed: %s",sizeof(char),27U);_T10=_tag_fat(_T15,sizeof(void*),1);_TD(_TF,_T10);}};}struct _tuple11{void*f0;void*f1;};
# 111
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){void*_T0;int _T1;void*_T2;int _T3;struct _tuple11 _T4;void*_T5;int*_T6;int _T7;void*_T8;int*_T9;int _TA;void*_TB;void*_TC;struct Cyc_Absyn_Vardecl*_TD;int _TE;struct Cyc_Absyn_Vardecl*_TF;int _T10;int _T11;void*_T12;int*_T13;int _T14;void*_T15;int*_T16;int _T17;void*_T18;int*_T19;int _T1A;void*_T1B;void*_T1C;struct Cyc_Absyn_Exp*_T1D;int _T1E;struct Cyc_Absyn_Exp*_T1F;int _T20;int _T21;void*_T22;int*_T23;int _T24;void*_T25;void*_T26;int _T27;_T0=r1;_T1=(int)_T0;_T2=r2;_T3=(int)_T2;
if(_T1!=_T3)goto _TLD;
return 0;_TLD:{struct _tuple11 _T28;
_T28.f0=r1;_T28.f1=r2;_T4=_T28;}{struct _tuple11 _T28=_T4;int _T29;int _T2A;struct Cyc_Absyn_Exp*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Vardecl*_T2D;struct Cyc_Absyn_Vardecl*_T2E;_T5=_T28.f0;_T6=(int*)_T5;_T7=*_T6;if(_T7!=0)goto _TLF;_T8=_T28.f1;_T9=(int*)_T8;_TA=*_T9;if(_TA!=0)goto _TL11;_TB=_T28.f0;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T2F=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_TB;_T2E=_T2F->f1;}_TC=_T28.f1;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T2F=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_TC;_T2D=_T2F->f1;}{struct Cyc_Absyn_Vardecl*vd1=_T2E;struct Cyc_Absyn_Vardecl*vd2=_T2D;_TD=vd1;_TE=(int)_TD;_TF=vd2;_T10=(int)_TF;_T11=_TE - _T10;
return _T11;}_TL11:
 return -1;_TLF: _T12=_T28.f1;_T13=(int*)_T12;_T14=*_T13;if(_T14!=0)goto _TL13;
return 1;_TL13: _T15=_T28.f0;_T16=(int*)_T15;_T17=*_T16;if(_T17!=1)goto _TL15;_T18=_T28.f1;_T19=(int*)_T18;_T1A=*_T19;if(_T1A!=1)goto _TL17;_T1B=_T28.f0;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T2F=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_T1B;_T2C=_T2F->f1;}_T1C=_T28.f1;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T2F=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_T1C;_T2B=_T2F->f1;}{struct Cyc_Absyn_Exp*e1=_T2C;struct Cyc_Absyn_Exp*e2=_T2B;_T1D=e1;_T1E=(int)_T1D;_T1F=e2;_T20=(int)_T1F;_T21=_T1E - _T20;
return _T21;}_TL17:
 return -1;_TL15: _T22=_T28.f1;_T23=(int*)_T22;_T24=*_T23;if(_T24!=1)goto _TL19;
return 1;_TL19: _T25=_T28.f0;{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T2F=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_T25;_T2A=_T2F->f1;}_T26=_T28.f1;{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_T2F=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_T26;_T29=_T2F->f1;}{int i1=_T2A;int i2=_T29;_T27=i1 - i2;
return _T27;};}}
# 125
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){struct _tuple11 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;void*_T9;int*_TA;int _TB;{struct _tuple11 _TC;
_TC.f0=p1;_TC.f1=p2;_T0=_TC;}{struct _tuple11 _TC=_T0;int _TD;int _TE;_T1=_TC.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL1B;_T4=_TC.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL1D;_T7=_TC.f0;{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_TF=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_T7;_TE=_TF->f1;}_T8=_TC.f1;{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_TF=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_T8;_TD=_TF->f1;}{int i1=_TE;int i2=_TD;
# 128
if(i1!=i2)goto _TL1F;return 0;_TL1F:
 if(i1 >= i2)goto _TL21;return -1;
_TL21: return 1;}_TL1D:
# 133
 return 1;_TL1B: _T9=_TC.f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=1)goto _TL23;
# 131
return 0;_TL23:
 return -1;;}}
# 137
static int Cyc_CfFlowInfo_path_cmp(struct Cyc_List_List*path1,struct Cyc_List_List*path2){struct Cyc_List_List*_T0;struct Cyc_List_List*_T1;int _T2;_T0=path1;_T1=path2;_T2=
Cyc_List_list_cmp(Cyc_CfFlowInfo_pathcon_cmp,_T0,_T1);return _T2;}
# 141
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){struct Cyc_CfFlowInfo_Place*_T0;int _T1;struct Cyc_CfFlowInfo_Place*_T2;int _T3;struct Cyc_CfFlowInfo_Place*_T4;struct Cyc_CfFlowInfo_Place*_T5;int _T6;int _T7;_T0=p1;_T1=(int)_T0;_T2=p2;_T3=(int)_T2;
if(_T1!=_T3)goto _TL25;
return 0;_TL25: _T4=p1;{
struct Cyc_CfFlowInfo_Place _T8=*_T4;struct Cyc_List_List*_T9;void*_TA;_TA=_T8.root;_T9=_T8.path;{void*root1=_TA;struct Cyc_List_List*path1=_T9;_T5=p2;{
struct Cyc_CfFlowInfo_Place _TB=*_T5;struct Cyc_List_List*_TC;void*_TD;_TD=_TB.root;_TC=_TB.path;{void*root2=_TD;struct Cyc_List_List*path2=_TC;
int i=Cyc_CfFlowInfo_root_cmp(root1,root2);
if(i==0)goto _TL27;_T6=i;
return _T6;_TL27: _T7=
Cyc_CfFlowInfo_path_cmp(path1,path2);return _T7;}}}}}
# 161 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,int,int,void*,void*);
# 165
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){int _T0;struct _fat_ptr _T1;void**_T2;unsigned _T3;void**_T4;unsigned _T5;struct Cyc_CfFlowInfo_FlowEnv*_T6;int _T7;unsigned _T8;unsigned _T9;struct Cyc_List_List*_TA;void*_TB;struct _fat_ptr*_TC;struct _fat_ptr _TD;unsigned _TE;void**_TF;unsigned char*_T10;void**_T11;int _T12;struct Cyc_List_List*_T13;struct _fat_ptr _T14;_T0=
# 171
Cyc_List_length(fs);{unsigned sz=(unsigned)_T0;{unsigned _T15=sz;_T3=_check_times(_T15,sizeof(void*));{void**_T16=_cycalloc(_T3);{unsigned _T17=sz;unsigned i;i=0;_TL2C: if(i < _T17)goto _TL2A;else{goto _TL2B;}_TL2A: _T5=i;_T4=_T16 + _T5;_T6=fenv;
*_T4=_T6->unknown_all;i=i + 1;goto _TL2C;_TL2B:;}_T2=(void**)_T16;}_T1=_tag_fat(_T2,sizeof(void*),_T15);}{struct _fat_ptr d=_T1;{
int i=0;_TL30: _T7=i;_T8=(unsigned)_T7;_T9=sz;if(_T8 < _T9)goto _TL2E;else{goto _TL2F;}
_TL2E: _TA=_check_null(fs);_TB=_TA->hd;{struct Cyc_Absyn_Aggrfield*_T15=(struct Cyc_Absyn_Aggrfield*)_TB;void*_T16;struct _fat_ptr*_T17;{struct Cyc_Absyn_Aggrfield _T18=*_T15;_T17=_T18.name;_T16=_T18.type;}{struct _fat_ptr*n=_T17;void*t2=_T16;_TC=n;_TD=*_TC;_TE=
_get_fat_size(_TD,sizeof(char));if(_TE==1U)goto _TL31;
_T10=d.curr;_T11=(void**)_T10;_T12=i;_TF=_T11 + _T12;*_TF=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,t2,leafval);goto _TL32;_TL31: _TL32:;}}
# 173
i=i + 1;_T13=fs;fs=_T13->tl;goto _TL30;_TL2F:;}_T14=d;
# 179
return _T14;}}}struct _tuple12{struct _RegionHandle*f0;struct Cyc_List_List*f1;};
# 183
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple12*env,struct Cyc_Absyn_Aggrfield*f){struct _tuple12*_T0;struct _RegionHandle*_T1;struct Cyc_List_List*_T2;struct Cyc_Absyn_Aggrfield*_T3;void*_T4;struct Cyc_Absyn_Aggrfield*_T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Aggrfield*_T7;struct _RegionHandle*_T8;struct Cyc_Absyn_Aggrfield*_T9;struct Cyc_Absyn_Aggrfield*_TA;struct Cyc_Absyn_Aggrfield*_TB;struct Cyc_Absyn_Aggrfield*_TC;struct Cyc_List_List*_TD;struct _RegionHandle*_TE;_T0=env;{struct _tuple12 _TF=*_T0;_TE=_TF.f0;_TD=_TF.f1;}{struct _RegionHandle*t=_TE;struct Cyc_List_List*inst=_TD;_T1=t;_T2=inst;_T3=f;_T4=_T3->type;{
# 188
void*new_typ=Cyc_Tcutil_rsubstitute(_T1,_T2,_T4);_T5=f;{
struct Cyc_Absyn_Exp*r=_T5->requires_clause;
if(r!=0)goto _TL33;_T6=0;goto _TL34;_TL33: _T6=Cyc_Tcutil_rsubsexp(t,inst,r);_TL34: {struct Cyc_Absyn_Exp*new_r=_T6;_T8=t;{struct Cyc_Absyn_Aggrfield*_TF=_region_malloc(_T8,0U,sizeof(struct Cyc_Absyn_Aggrfield));_T9=f;
_TF->name=_T9->name;_TA=f;_TF->tq=_TA->tq;_TF->type=new_typ;_TB=f;_TF->width=_TB->width;_TC=f;_TF->attributes=_TC->attributes;_TF->requires_clause=new_r;_T7=(struct Cyc_Absyn_Aggrfield*)_TF;}return _T7;}}}}}struct _tuple13{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 198
static struct _fat_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;struct Cyc_Absyn_Tvar*_T3;struct Cyc_Absyn_Kind*_T4;struct Cyc_Absyn_Kind*_T5;struct Cyc_Absyn_Kind*_T6;enum Cyc_Absyn_KindQual _T7;struct Cyc_List_List*_T8;struct _RegionHandle*_T9;struct _tuple13*_TA;struct _RegionHandle*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct _tuple12 _TE;struct Cyc_List_List*(*_TF)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*)(struct _tuple12*,struct Cyc_Absyn_Aggrfield*),struct _tuple12*,struct Cyc_List_List*);struct Cyc_List_List*(*_T10)(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _RegionHandle*_T11;struct _tuple12*_T12;struct Cyc_List_List*_T13;struct _RegionHandle _T14=_new_region(0U,"temp");struct _RegionHandle*temp=& _T14;_push_region(temp);{
# 208
struct Cyc_List_List*inst=0;
_TL38: if(tvs!=0)goto _TL36;else{goto _TL37;}
_TL36: _T0=tvs;_T1=_T0->hd;{struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)_T1;_T2=
_check_null(targs);{void*t=_T2->hd;_T3=tv;_T4=& Cyc_Kinds_bk;_T5=(struct Cyc_Absyn_Kind*)_T4;{
struct Cyc_Absyn_Kind*_T15=Cyc_Kinds_tvar_kind(_T3,_T5);_T6=(struct Cyc_Absyn_Kind*)_T15;_T7=_T6->kind;if(_T7!=Cyc_Absyn_EffKind)goto _TL39;goto _TL35;_TL39: goto _LL0;_LL0:;}_T9=temp;{struct Cyc_List_List*_T15=_region_malloc(_T9,0U,sizeof(struct Cyc_List_List));_TB=temp;{struct _tuple13*_T16=_region_malloc(_TB,0U,sizeof(struct _tuple13));
# 217
_T16->f0=tv;_T16->f1=t;_TA=(struct _tuple13*)_T16;}_T15->hd=_TA;_T15->tl=inst;_T8=(struct Cyc_List_List*)_T15;}inst=_T8;}}_TL35: _TC=tvs;
# 209
tvs=_TC->tl;_TD=targs;targs=_TD->tl;goto _TL38;_TL37:
# 219
 if(inst==0)goto _TL3B;{struct _tuple12 _T15;
_T15.f0=temp;_T15.f1=Cyc_List_imp_rev(inst);_TE=_T15;}{struct _tuple12 env=_TE;_T10=Cyc_List_rmap_c;{
struct Cyc_List_List*(*_T15)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*)(struct _tuple12*,struct Cyc_Absyn_Aggrfield*),struct _tuple12*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*)(struct _tuple12*,struct Cyc_Absyn_Aggrfield*),struct _tuple12*,struct Cyc_List_List*))_T10;_TF=_T15;}_T11=temp;_T12=& env;_T13=fs;{struct Cyc_List_List*subs_fs=_TF(_T11,Cyc_CfFlowInfo_substitute_field,_T12,_T13);struct _fat_ptr _T15=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _T15;}}
# 224
_TL3B: {struct _fat_ptr _T15=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _T15;}}_pop_region();}struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;};
# 228
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){int _T0;struct Cyc_CfFlowInfo_FlowEnv*_T1;void*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;unsigned _T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;void*_TA;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TB;union Cyc_Absyn_DatatypeFieldInfo _TC;struct _union_DatatypeFieldInfo_KnownDatatypefield _TD;unsigned _TE;void*_TF;union Cyc_Absyn_DatatypeFieldInfo _T10;struct _union_DatatypeFieldInfo_KnownDatatypefield _T11;struct _tuple1 _T12;struct Cyc_Absyn_Datatypefield*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_Absyn_Datatypefield*_T16;int _T17;int _T18;struct _fat_ptr _T19;void**_T1A;unsigned _T1B;void**_T1C;unsigned _T1D;struct Cyc_CfFlowInfo_FlowEnv*_T1E;struct _fat_ptr _T1F;char*_T20;void**_T21;int _T22;unsigned _T23;unsigned _T24;void**_T25;unsigned char*_T26;void**_T27;int _T28;struct Cyc_CfFlowInfo_FlowEnv*_T29;int _T2A;struct Cyc_List_List*_T2B;void*_T2C;struct _tuple14*_T2D;struct _tuple14 _T2E;void*_T2F;void*_T30;struct Cyc_List_List*_T31;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T32;void*_T33;void*_T34;struct Cyc_Absyn_Aggrdecl*_T35;struct Cyc_Absyn_AggrdeclImpl*_T36;struct Cyc_Absyn_Aggrdecl*_T37;struct Cyc_Absyn_AggrdeclImpl*_T38;struct Cyc_Absyn_Aggrdecl*_T39;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T3A;struct Cyc_Absyn_Aggrdecl*_T3B;enum Cyc_Absyn_AggrKind _T3C;int _T3D;struct Cyc_CfFlowInfo_FlowEnv*_T3E;struct Cyc_Absyn_Aggrdecl*_T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*_T41;struct Cyc_List_List*_T42;struct Cyc_Absyn_Aggrdecl*_T43;enum Cyc_Absyn_AggrKind _T44;int _T45;int _T46;void*_T47;void*_T48;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T49;struct Cyc_List_List*_T4A;struct Cyc_List_List*_T4B;void*_T4C;void*_T4D;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T4E;enum Cyc_Absyn_AggrKind _T4F;int _T50;struct Cyc_CfFlowInfo_FlowEnv*_T51;struct Cyc_List_List*_T52;enum Cyc_Absyn_AggrKind _T53;int _T54;int _T55;void*_T56;void*_T57;struct Cyc_Absyn_ArrayInfo _T58;struct Cyc_Absyn_ArrayInfo _T59;int _T5A;void*_T5B;int _T5C;int _T5D;int _T5E;struct Cyc_CfFlowInfo_FlowEnv*_T5F;struct Cyc_Absyn_PtrInfo _T60;struct Cyc_Absyn_PtrAtts _T61;int _T62;void*_T63;int*_T64;int _T65;void*_T66;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T67;enum Cyc_CfFlowInfo_InitLevel _T68;struct Cyc_CfFlowInfo_FlowEnv*_T69;void*_T6A;void*_T6B;int _T6C;int _T6D;struct Cyc_CfFlowInfo_FlowEnv*_T6E;_T0=
# 232
Cyc_Absyn_is_nontagged_nonrequire_union_type(t);if(!_T0)goto _TL3D;_T1=fenv;_T2=_T1->unknown_all;return _T2;_TL3D:{
# 234
void*_T6F=Cyc_Absyn_compress(t);void*_T70;enum Cyc_Absyn_AggrKind _T71;union Cyc_Absyn_AggrInfo _T72;void*_T73;_T3=(int*)_T6F;_T4=*_T3;switch(_T4){case 0: _T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6F;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 23: _T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6F;_TA=_T9->f1;_TB=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TA;_TC=_TB->f1;_TD=_TC.KnownDatatypefield;_TE=_TD.tag;if(_TE!=2)goto _TL41;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T74=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6F;_TF=_T74->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T75=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TF;_T10=_T75->f1;_T11=_T10.KnownDatatypefield;_T12=_T11.val;_T73=_T12.f1;}}{struct Cyc_Absyn_Datatypefield*tuf=_T73;_T13=tuf;_T14=_T13->typs;
# 238
if(_T14!=0)goto _TL43;_T15=leafval;
return _T15;_TL43: _T16=tuf;{
struct Cyc_List_List*tqts=_T16->typs;_T17=
Cyc_List_length(tqts);_T18=_T17 + 1;{unsigned sz=(unsigned)_T18;{unsigned _T74=sz;_T1B=_check_times(_T74,sizeof(void*));{void**_T75=_cycalloc(_T1B);{unsigned _T76=sz;unsigned i;i=0;_TL48: if(i < _T76)goto _TL46;else{goto _TL47;}_TL46: _T1D=i;_T1C=_T75 + _T1D;_T1E=fenv;
*_T1C=_T1E->unknown_all;i=i + 1;goto _TL48;_TL47:;}_T1A=(void**)_T75;}_T19=_tag_fat(_T1A,sizeof(void*),_T74);}{struct _fat_ptr d=_T19;_T1F=d;_T20=_check_fat_subscript(_T1F,sizeof(void*),0);_T21=(void**)_T20;
*_T21=leafval;{
int i=1;_TL4C: _T22=i;_T23=(unsigned)_T22;_T24=sz;if(_T23 < _T24)goto _TL4A;else{goto _TL4B;}
_TL4A: _T26=d.curr;_T27=(void**)_T26;_T28=i;_T25=_T27 + _T28;_T29=fenv;_T2A=no_init_bits_only;_T2B=_check_null(tqts);_T2C=_T2B->hd;_T2D=(struct _tuple14*)_T2C;_T2E=*_T2D;_T2F=_T2E.f1;_T30=leafval;*_T25=Cyc_CfFlowInfo_i_typ_to_absrval(_T29,0,_T2A,_T2F,_T30);_T31=tqts;
tqts=_T31->tl;
# 244
i=i + 1;goto _TL4C;_TL4B:;}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T74=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T74->tag=6;
# 248
_T74->f1.is_union=0;_T74->f1.fieldnum=-1;_T74->f2=d;_T32=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T74;}_T33=(void*)_T32;return _T33;}}}}_TL41: goto _LLD;case 24:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T74=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6F;_T34=_T74->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T75=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T34;_T72=_T75->f1;}_T73=_T74->f2;}{union Cyc_Absyn_AggrInfo info=_T72;struct Cyc_List_List*targs=_T73;
# 250
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T35=ad;_T36=_T35->impl;
if(_T36!=0)goto _TL4D;goto _LL0;_TL4D: _T37=ad;_T38=_T37->impl;{
# 253
struct Cyc_List_List*fields=_T38->fields;
if(targs!=0)goto _TL4F;_T39=ad;_T71=_T39->kind;_T73=fields;goto _LL6;_TL4F:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T74=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T74->tag=6;_T3B=ad;_T3C=_T3B->kind;_T3D=(int)_T3C;
_T74->f1.is_union=_T3D==1;_T74->f1.fieldnum=-1;_T3E=fenv;_T3F=ad;_T40=_T3F->tvs;_T41=targs;_T42=fields;_T43=ad;_T44=_T43->kind;_T45=(int)_T44;_T46=_T45==1;_T47=leafval;
_T74->f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(_T3E,_T40,_T41,_T42,_T46,_T47);_T3A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T74;}_T48=(void*)_T3A;
# 255
return _T48;}}case 5: _T49=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6F;_T4A=_T49->f2;if(_T4A==0)goto _TL51;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T74=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6F;_T4B=_T74->f2;{struct Cyc_List_List _T75=*_T4B;_T4C=_T75.hd;_T73=(void*)_T4C;}}{void*t=_T73;_T4D=leafval;
# 273
return _T4D;}_TL51: goto _LLD;default: goto _LLD;};case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T74=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T6F;_T71=_T74->f1;_T73=_T74->f3;}_LL6: {enum Cyc_Absyn_AggrKind k=_T71;struct Cyc_List_List*fs=_T73;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T74=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T74->tag=6;_T4F=k;_T50=(int)_T4F;
# 265
_T74->f1.is_union=_T50==1;_T74->f1.fieldnum=-1;_T51=fenv;_T52=fs;_T53=k;_T54=(int)_T53;_T55=_T54==1;_T56=leafval;
_T74->f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(_T51,_T52,_T55,_T56);_T4E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T74;}_T57=(void*)_T4E;
# 265
return _T57;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T74=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T6F;_T58=_T74->f1;_T73=_T58.elt_type;_T59=_T74->f1;_T70=_T59.zero_term;}{void*et=_T73;void*zeroterm=_T70;_T5A=
# 268
Cyc_Tcutil_force_type2bool(0,zeroterm);if(!_T5A)goto _TL53;_T5C=allow_zeroterm;
# 271
if(!_T5C)goto _TL55;_T5D=no_init_bits_only;if(_T5D)goto _TL55;else{goto _TL57;}_TL57: _T5E=Cyc_Tcutil_is_bits_only_type(et);if(!_T5E)goto _TL55;_T5F=fenv;_T5B=_T5F->unknown_all;goto _TL56;_TL55: _T5B=leafval;_TL56: return _T5B;_TL53: goto _LL0;}case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T74=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T6F;_T60=_T74->f1;_T61=_T60.ptr_atts;_T73=_T61.nullable;}{void*nbl=_T73;_T62=
# 275
Cyc_Tcutil_force_type2bool(0,nbl);if(_T62)goto _TL58;else{goto _TL5A;}
_TL5A: _T63=leafval;_T64=(int*)_T63;_T65=*_T64;if(_T65!=2)goto _TL5B;_T66=leafval;_T67=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T66;_T68=_T67->f1;if(_T68!=Cyc_CfFlowInfo_AllIL)goto _TL5D;_T69=fenv;_T6A=_T69->notzeroall;
return _T6A;_TL5D: goto _LL12;_TL5B: _LL12: goto _LLF;_LLF: goto _TL59;_TL58: _TL59: goto _LL0;}default: _LLD: goto _LL0;}_LL0:;}_T6C=no_init_bits_only;
# 284
if(_T6C)goto _TL5F;else{goto _TL61;}_TL61: _T6D=Cyc_Tcutil_is_bits_only_type(t);if(!_T6D)goto _TL5F;_T6E=fenv;_T6B=_T6E->unknown_all;goto _TL60;_TL5F: _T6B=leafval;_TL60: return _T6B;}
# 287
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){void*_T0;_T0=
Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);return _T0;}
# 293
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;int*_T5;void*_T6;struct Cyc_CfFlowInfo_UnionRInfo _T7;struct Cyc_CfFlowInfo_UnionRInfo _T8;int _T9;struct _fat_ptr _TA;int _TB;unsigned _TC;unsigned _TD;struct Cyc_Absyn_Exp*_TE;int _TF;struct _fat_ptr _T10;unsigned char*_T11;void**_T12;int _T13;void*_T14;int*_T15;int _T16;struct Cyc_Absyn_Exp*_T17;int _T18;struct _fat_ptr _T19;int _T1A;char*_T1B;void**_T1C;void*_T1D;int*_T1E;int _T1F;void*_T20;void*_T21;int _T22;struct _fat_ptr _T23;int _T24;void*_T25;int _T26;void*_T27;_T0=r;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 7: _T3=r;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T28=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T3;_T27=_T28->f1;_T26=_T28->f2;_T4=_T28->f3;_T25=(void*)_T4;_T24=_T28->f4;}{struct Cyc_Absyn_Exp*consumer=_T27;int iteration=_T26;void*r=_T25;int local=_T24;
# 296
if(consumer!=e)goto _TL63;if(iteration!=env_iteration)goto _TL63;_T5=needs_unconsume;
*_T5=1;
return 0;_TL63:
# 300
 return 1;}case 6: _T6=r;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T28=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T6;_T7=_T28->f1;_T26=_T7.is_union;_T8=_T28->f1;_T24=_T8.fieldnum;_T23=_T28->f2;}{int is_union=_T26;int field_no=_T24;struct _fat_ptr d=_T23;_T9=is_union;
# 302
if(_T9)goto _TL68;else{goto _TL67;}_TL68: if(field_no==-1)goto _TL67;else{goto _TL65;}
_TL67: _TA=d;{unsigned sz=_get_fat_size(_TA,sizeof(void*));{
int i=0;_TL6C: _TB=i;_TC=(unsigned)_TB;_TD=sz;if(_TC < _TD)goto _TL6A;else{goto _TL6B;}
_TL6A: _TE=e;_TF=env_iteration;_T10=d;_T11=_T10.curr;_T12=(void**)_T11;_T13=i;_T14=_T12[_T13];_T15=needs_unconsume;_T16=Cyc_CfFlowInfo_is_unique_consumed(_TE,_TF,_T14,_T15);if(!_T16)goto _TL6D;
return 1;_TL6D:
# 304
 i=i + 1;goto _TL6C;_TL6B:;}
# 307
return 0;}
# 310
_TL65: _T17=e;_T18=env_iteration;_T19=d;_T1A=field_no;_T1B=_check_fat_subscript(_T19,sizeof(void*),_T1A);_T1C=(void**)_T1B;_T1D=*_T1C;_T1E=needs_unconsume;_T1F=Cyc_CfFlowInfo_is_unique_consumed(_T17,_T18,_T1D,_T1E);return _T1F;}case 8: _T20=r;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T28=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T20;_T21=_T28->f2;_T27=(void*)_T21;}{void*r=_T27;_T22=
# 312
Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,r,needs_unconsume);return _T22;}default:
 return 0;};}
# 319
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;void**_T9;unsigned _TA;void**_TB;unsigned _TC;struct _fat_ptr _TD;unsigned char*_TE;void**_TF;unsigned _T10;int _T11;int _T12;unsigned _T13;unsigned _T14;void**_T15;unsigned char*_T16;void**_T17;int _T18;struct Cyc_CfFlowInfo_FlowEnv*_T19;struct _fat_ptr _T1A;unsigned char*_T1B;void**_T1C;int _T1D;void*_T1E;struct _fat_ptr _T1F;unsigned char*_T20;void**_T21;int _T22;void*_T23;struct _fat_ptr _T24;unsigned char*_T25;void**_T26;int _T27;void*_T28;int _T29;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T2A;void*_T2B;void*_T2C;void*_T2D;void*_T2E;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T2F;void*_T30;void*_T31;void*_T32;struct Cyc_Absyn_Vardecl*_T33;struct _fat_ptr _T34;struct Cyc_CfFlowInfo_UnionRInfo _T35;int _T36;void*_T37;int _T38;void*_T39;_T0=r;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 7: _T3=r;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T3A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T3;_T39=_T3A->f1;_T38=_T3A->f2;_T4=_T3A->f3;_T37=(void*)_T4;_T36=_T3A->f4;}{struct Cyc_Absyn_Exp*consumer=_T39;int iteration=_T38;void*r=_T37;int local=_T36;_T5=r;
# 322
return _T5;}case 6: _T6=r;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T3A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T6;_T35=_T3A->f1;_T34=_T3A->f2;}{struct Cyc_CfFlowInfo_UnionRInfo uinfo=_T35;struct _fat_ptr d=_T34;_T7=d;{
# 324
unsigned sz=_get_fat_size(_T7,sizeof(void*));
int change=0;{unsigned _T3A=sz;_TA=_check_times(_T3A,sizeof(void*));{void**_T3B=_cycalloc(_TA);{unsigned _T3C=sz;unsigned i;i=0;_TL73: if(i < _T3C)goto _TL71;else{goto _TL72;}_TL71: _TC=i;_TB=_T3B + _TC;_TD=d;_TE=_TD.curr;_TF=(void**)_TE;_T10=i;_T11=(int)_T10;
*_TB=_TF[_T11];i=i + 1;goto _TL73;_TL72:;}_T9=(void**)_T3B;}_T8=_tag_fat(_T9,sizeof(void*),_T3A);}{struct _fat_ptr d2=_T8;{
int i=0;_TL77: _T12=i;_T13=(unsigned)_T12;_T14=sz;if(_T13 < _T14)goto _TL75;else{goto _TL76;}
_TL75: _T16=d2.curr;_T17=(void**)_T16;_T18=i;_T15=_T17 + _T18;_T19=fenv;_T1A=d;_T1B=_T1A.curr;_T1C=(void**)_T1B;_T1D=i;_T1E=_T1C[_T1D];*_T15=Cyc_CfFlowInfo_make_unique_unconsumed(_T19,_T1E);_T1F=d2;_T20=_T1F.curr;_T21=(void**)_T20;_T22=i;_T23=_T21[_T22];_T24=d;_T25=_T24.curr;_T26=(void**)_T25;_T27=i;_T28=_T26[_T27];
if(_T23==_T28)goto _TL78;
change=1;goto _TL79;_TL78: _TL79:
# 327
 i=i + 1;goto _TL77;_TL76:;}_T29=change;
# 332
if(!_T29)goto _TL7A;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T3A=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T3A->tag=6;
_T3A->f1=uinfo;_T3A->f2=d2;_T2A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T3A;}_T2B=(void*)_T2A;return _T2B;
_TL7A: _T2C=r;return _T2C;}}}case 8: _T2D=r;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T3A=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T2D;_T33=_T3A->f1;_T2E=_T3A->f2;_T39=(void*)_T2E;}{struct Cyc_Absyn_Vardecl*n=_T33;void*r2=_T39;
# 336
void*r3=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,r2);
if(r3==r2)goto _TL7C;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T3A=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T3A->tag=8;
_T3A->f1=n;_T3A->f2=r3;_T2F=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T3A;}_T30=(void*)_T2F;return _T30;_TL7C: _T31=r;
return _T31;}default: _T32=r;
return _T32;};}
# 346
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*aqb,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r,int local_alias){struct _tuple11 _T0;void*_T1;int*_T2;int _T3;void*_T4;void*_T5;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T6;void*_T7;void*_T8;void*_T9;int*_TA;unsigned _TB;void*_TC;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD;void*_TE;int*_TF;int _T10;void*_T11;int*_T12;int _T13;void*_T14;void*_T15;void*_T16;struct Cyc_Absyn_Aggrdecl*_T17;struct Cyc_Absyn_AggrdeclImpl*_T18;void*_T19;struct Cyc_Absyn_Aggrdecl*_T1A;struct Cyc_Absyn_Aggrdecl*_T1B;struct Cyc_Absyn_AggrdeclImpl*_T1C;void*_T1D;int*_T1E;int _T1F;void*_T20;void*_T21;struct _fat_ptr _T22;struct _fat_ptr _T23;void**_T24;unsigned _T25;struct _fat_ptr _T26;void**_T27;unsigned _T28;struct _fat_ptr _T29;unsigned char*_T2A;void**_T2B;unsigned _T2C;int _T2D;int _T2E;int _T2F;unsigned _T30;unsigned _T31;struct Cyc_List_List*_T32;void*_T33;struct _fat_ptr*_T34;struct _fat_ptr _T35;unsigned _T36;void**_T37;unsigned char*_T38;void**_T39;int _T3A;struct Cyc_CfFlowInfo_FlowEnv*_T3B;struct Cyc_List_List*_T3C;void*_T3D;struct Cyc_Absyn_Exp*_T3E;int _T3F;struct _fat_ptr _T40;int _T41;char*_T42;void**_T43;void*_T44;int _T45;struct Cyc_List_List*_T46;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T47;void*_T48;int _T49;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T4A;void*_T4B;void*_T4C;{struct _tuple11 _T4D;
_T4D.f0=Cyc_Absyn_compress(t);_T4D.f1=r;_T0=_T4D;}{struct _tuple11 _T4D=_T0;struct Cyc_List_List*_T4E;enum Cyc_Absyn_AggrKind _T4F;struct _fat_ptr _T50;struct Cyc_CfFlowInfo_UnionRInfo _T51;union Cyc_Absyn_AggrInfo _T52;void*_T53;struct Cyc_Absyn_Vardecl*_T54;_T1=_T4D.f1;_T2=(int*)_T1;_T3=*_T2;if(_T3!=8)goto _TL7E;_T4=_T4D.f1;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T55=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T4;_T54=_T55->f1;_T5=_T55->f2;_T53=(void*)_T5;}{struct Cyc_Absyn_Vardecl*s=_T54;void*r2=_T53;
# 349
void*r3=Cyc_CfFlowInfo_make_unique_consumed(fenv,aqb,t,consumer,iteration,r2,local_alias);
if(r3==r2)goto _TL80;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T55=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T55->tag=8;
_T55->f1=s;_T55->f2=r3;_T6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T55;}_T7=(void*)_T6;return _T7;_TL80: _T8=r;
return _T8;}_TL7E: _T9=_T4D.f0;_TA=(int*)_T9;_TB=*_TA;switch(_TB){case 0: _TC=_T4D.f0;_TD=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TC;_TE=_TD->f1;_TF=(int*)_TE;_T10=*_TF;if(_T10!=24)goto _TL83;_T11=_T4D.f1;_T12=(int*)_T11;_T13=*_T12;if(_T13!=6)goto _TL85;_T14=_T4D.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T55=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T14;_T15=_T55->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T56=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T15;_T52=_T56->f1;}}_T16=_T4D.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T55=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T16;_T51=_T55->f1;_T50=_T55->f2;}{union Cyc_Absyn_AggrInfo info=_T52;struct Cyc_CfFlowInfo_UnionRInfo b=_T51;struct _fat_ptr d=_T50;
# 354
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);_T17=ad;_T18=_T17->impl;
if(_T18!=0)goto _TL87;_T19=r;return _T19;_TL87: _T1A=ad;_T4F=_T1A->kind;_T1B=ad;_T1C=_T1B->impl;_T4E=_T1C->fields;_T51=b;_T50=d;goto _LL6;}_TL85: goto _LL7;_TL83: goto _LL7;case 7: _T1D=_T4D.f1;_T1E=(int*)_T1D;_T1F=*_T1E;if(_T1F!=6)goto _TL89;_T20=_T4D.f0;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T55=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T20;_T4F=_T55->f1;_T4E=_T55->f3;}_T21=_T4D.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T55=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T21;_T51=_T55->f1;_T50=_T55->f2;}_LL6:{enum Cyc_Absyn_AggrKind k=_T4F;struct Cyc_List_List*fs=_T4E;struct Cyc_CfFlowInfo_UnionRInfo b=_T51;struct _fat_ptr d=_T50;_T23=d;{unsigned _T55=
# 358
_get_fat_size(_T23,sizeof(void*));_T25=_check_times(_T55,sizeof(void*));{void**_T56=_cycalloc(_T25);_T26=d;{unsigned _T57=_get_fat_size(_T26,sizeof(void*));unsigned i;i=0;_TL8E: if(i < _T57)goto _TL8C;else{goto _TL8D;}_TL8C: _T28=i;_T27=_T56 + _T28;_T29=d;_T2A=_T29.curr;_T2B=(void**)_T2A;_T2C=i;_T2D=(int)_T2C;*_T27=_T2B[_T2D];i=i + 1;goto _TL8E;_TL8D:;}_T24=(void**)_T56;}_T22=_tag_fat(_T24,sizeof(void*),_T55);}{struct _fat_ptr d2=_T22;_T2E=
Cyc_List_length(fs);{unsigned sz=(unsigned)_T2E;{
int i=0;_TL92: _T2F=i;_T30=(unsigned)_T2F;_T31=sz;if(_T30 < _T31)goto _TL90;else{goto _TL91;}
_TL90: _T32=_check_null(fs);_T33=_T32->hd;{struct Cyc_Absyn_Aggrfield*_T55=(struct Cyc_Absyn_Aggrfield*)_T33;void*_T56;struct _fat_ptr*_T57;{struct Cyc_Absyn_Aggrfield _T58=*_T55;_T57=_T58.name;_T56=_T58.type;}{struct _fat_ptr*n=_T57;void*t2=_T56;_T34=n;_T35=*_T34;_T36=
_get_fat_size(_T35,sizeof(char));if(_T36==1U)goto _TL93;
_T38=d2.curr;_T39=(void**)_T38;_T3A=i;_T37=_T39 + _T3A;_T3B=fenv;_T3C=aqb;_T3D=t2;_T3E=consumer;_T3F=iteration;_T40=d;_T41=i;_T42=_check_fat_subscript(_T40,sizeof(void*),_T41);_T43=(void**)_T42;_T44=*_T43;_T45=local_alias;*_T37=Cyc_CfFlowInfo_make_unique_consumed(_T3B,_T3C,_T3D,_T3E,_T3F,_T44,_T45);goto _TL94;_TL93: _TL94:;}}
# 360
i=i + 1;_T46=fs;fs=_T46->tl;goto _TL92;_TL91:;}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T55=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T55->tag=6;
# 365
_T55->f1=b;_T55->f2=d2;_T47=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T55;}_T48=(void*)_T47;return _T48;}}}_TL89: goto _LL7;default: _LL7: _T49=
# 367
Cyc_Tcutil_is_noalias_pointer(aqb,t,0);if(!_T49)goto _TL95;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T55=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_T55->tag=7;
_T55->f1=consumer;_T55->f2=iteration;_T55->f3=r;_T55->f4=local_alias;_T4A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T55;}_T4B=(void*)_T4A;return _T4B;_TL95: _T4C=r;
return _T4C;};}}struct _tuple15{struct Cyc_CfFlowInfo_Place*f0;unsigned f1;};
# 373
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){struct _tuple15 _T0;int(*_T1)(struct Cyc_Iter_Iter,struct _tuple15*);int(*_T2)(struct Cyc_Iter_Iter,void*);struct Cyc_Iter_Iter _T3;struct _tuple15*_T4;int _T5;struct _tuple15 _T6;struct Cyc_CfFlowInfo_Place*_T7;struct Cyc_CfFlowInfo_Place*_T8;int _T9;struct Cyc_List_List*_TA;void*_TB;struct Cyc_List_List*_TC;void*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct _RegionHandle _T10=_new_region(0U,"r");struct _RegionHandle*r=& _T10;_push_region(r);{struct _tuple15 _T11;
# 376
_T11.f0=place;_T11.f1=0U;_T0=_T11;}{struct _tuple15 elem=_T0;
struct Cyc_Iter_Iter iter=Cyc_Dict_make_iter(r,set);
_TL97: _T2=Cyc_Iter_next;{int(*_T11)(struct Cyc_Iter_Iter,struct _tuple15*)=(int(*)(struct Cyc_Iter_Iter,struct _tuple15*))_T2;_T1=_T11;}_T3=iter;_T4=& elem;_T5=_T1(_T3,_T4);if(_T5)goto _TL98;else{goto _TL99;}
_TL98: _T6=elem;{struct Cyc_CfFlowInfo_Place*place2=_T6.f0;_T7=place;{
struct Cyc_CfFlowInfo_Place _T11=*_T7;struct Cyc_List_List*_T12;void*_T13;_T13=_T11.root;_T12=_T11.path;{void*root1=_T13;struct Cyc_List_List*fs1=_T12;_T8=place2;{
struct Cyc_CfFlowInfo_Place _T14=*_T8;struct Cyc_List_List*_T15;void*_T16;_T16=_T14.root;_T15=_T14.path;{void*root2=_T16;struct Cyc_List_List*fs2=_T15;_T9=
Cyc_CfFlowInfo_root_cmp(root1,root2);if(_T9==0)goto _TL9A;goto _TL97;_TL9A:
# 384
 _TL9F: if(fs1!=0)goto _TLA0;else{goto _TL9E;}_TLA0: if(fs2!=0)goto _TL9D;else{goto _TL9E;}
_TL9D: _TA=fs1;_TB=_TA->hd;_TC=fs2;_TD=_TC->hd;if(_TB==_TD)goto _TLA1;goto _TL9E;_TLA1: _TE=fs1;
# 384
fs1=_TE->tl;_TF=fs2;fs2=_TF->tl;goto _TL9F;_TL9E:
# 386
 if(fs1!=0)goto _TLA3;{int _T17=1;_npop_handler(0);return _T17;}_TLA3:;}}}}}goto _TL97;_TL99: {int _T11=0;_npop_handler(0);return _T11;}}_pop_region();}struct Cyc_CfFlowInfo_EscPile{struct Cyc_List_List*places;};
# 399
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){int(*_T0)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*);int(*_T1)(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_CfFlowInfo_EscPile*_T2;struct Cyc_List_List*_T3;struct Cyc_CfFlowInfo_Place*_T4;int _T5;struct Cyc_CfFlowInfo_EscPile*_T6;struct Cyc_List_List*_T7;struct Cyc_CfFlowInfo_EscPile*_T8;_T1=Cyc_List_mem;{
# 402
int(*_T9)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*)=(int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))_T1;_T0=_T9;}_T2=pile;_T3=_T2->places;_T4=place;_T5=_T0(Cyc_CfFlowInfo_place_cmp,_T3,_T4);if(_T5)goto _TLA5;else{goto _TLA7;}
_TLA7: _T6=pile;{struct Cyc_List_List*_T9=_cycalloc(sizeof(struct Cyc_List_List));_T9->hd=place;_T8=pile;_T9->tl=_T8->places;_T7=(struct Cyc_List_List*)_T9;}_T6->places=_T7;goto _TLA6;_TLA5: _TLA6:;}
# 405
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;void*_T8;int _T9;unsigned _TA;struct _fat_ptr _TB;unsigned _TC;struct Cyc_CfFlowInfo_EscPile*_TD;struct _fat_ptr _TE;unsigned char*_TF;void**_T10;int _T11;void*_T12;struct _fat_ptr _T13;struct Cyc_CfFlowInfo_UnionRInfo _T14;struct Cyc_CfFlowInfo_Place*_T15;void*_T16;_T0=r;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 7: _T3=r;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T17=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T3;_T4=_T17->f3;_T16=(void*)_T4;}{void*r=_T16;_T16=r;goto _LL4;}case 8: _T5=r;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T17=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T5;_T6=_T17->f2;_T16=(void*)_T6;}_LL4: {void*r=_T16;
# 408
Cyc_CfFlowInfo_add_places(pile,r);return;}case 4: _T7=r;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T17=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T7;_T15=_T17->f1;}{struct Cyc_CfFlowInfo_Place*p=_T15;
Cyc_CfFlowInfo_add_place(pile,p);return;}case 6: _T8=r;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T17=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T8;_T14=_T17->f1;_T13=_T17->f2;}{struct Cyc_CfFlowInfo_UnionRInfo b=_T14;struct _fat_ptr d=_T13;{
# 411
int i=0;_TLAC: _T9=i;_TA=(unsigned)_T9;_TB=d;_TC=_get_fat_size(_TB,sizeof(void*));if(_TA < _TC)goto _TLAA;else{goto _TLAB;}
_TLAA: _TD=pile;_TE=d;_TF=_TE.curr;_T10=(void**)_TF;_T11=i;_T12=_T10[_T11];Cyc_CfFlowInfo_add_places(_TD,_T12);
# 411
i=i + 1;goto _TLAC;_TLAB:;}
# 413
return;}default:
 return;};}
# 421
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_CfFlowInfo_UnionRInfo _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;void**_T7;unsigned _T8;struct _fat_ptr _T9;void**_TA;unsigned _TB;void*_TC;struct _fat_ptr _TD;unsigned _TE;int _TF;char*_T10;void**_T11;void*_T12;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T13;void*_T14;void*_T15;void*_T16;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T17;void*_T18;void*_T19;void*_T1A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T1B;void*_T1C;void*_T1D;struct Cyc_Absyn_Vardecl*_T1E;int _T1F;void*_T20;void*_T21;struct _fat_ptr _T22;int _T23;_T0=old_val;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 6: _T3=old_val;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T24=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T3;_T4=_T24->f1;_T23=_T4.is_union;_T22=_T24->f2;}{int is_union=_T23;struct _fat_ptr d=_T22;_T6=d;{unsigned _T24=
# 424
_get_fat_size(_T6,sizeof(void*));_T8=_check_times(_T24,sizeof(void*));{void**_T25=_cycalloc(_T8);_T9=d;{unsigned _T26=_get_fat_size(_T9,sizeof(void*));unsigned i;i=0;_TLB1: if(i < _T26)goto _TLAF;else{goto _TLB0;}_TLAF: _TB=i;_TA=_T25 + _TB;_TC=new_val;_TD=d;_TE=i;_TF=(int)_TE;_T10=_check_fat_subscript(_TD,sizeof(void*),_TF);_T11=(void**)_T10;_T12=*_T11;
*_TA=Cyc_CfFlowInfo_insert_place_inner(_TC,_T12);i=i + 1;goto _TLB1;_TLB0:;}_T7=(void**)_T25;}_T5=
# 424
_tag_fat(_T7,sizeof(void*),_T24);}{struct _fat_ptr d2=_T5;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T24=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T24->tag=6;
# 428
_T24->f1.is_union=is_union;_T24->f1.fieldnum=-1;_T24->f2=d2;_T13=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T24;}_T14=(void*)_T13;return _T14;}}case 7: _T15=old_val;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T24=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T15;_T21=_T24->f1;_T23=_T24->f2;_T16=_T24->f3;_T20=(void*)_T16;_T1F=_T24->f4;}{struct Cyc_Absyn_Exp*e=_T21;int i=_T23;void*rval=_T20;int l=_T1F;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T24=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_T24->tag=7;
# 430
_T24->f1=e;_T24->f2=i;_T24->f3=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_T24->f4=l;_T17=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T24;}_T18=(void*)_T17;return _T18;}case 8: _T19=old_val;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T24=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T19;_T1E=_T24->f1;_T1A=_T24->f2;_T21=(void*)_T1A;}{struct Cyc_Absyn_Vardecl*n=_T1E;void*rval=_T21;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T24=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T24->tag=8;
# 432
_T24->f1=n;_T24->f2=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_T1B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T24;}_T1C=(void*)_T1B;return _T1C;}default: _T1D=new_val;
return _T1D;};}
# 439
static struct _fat_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _fat_ptr d,int n,void*rval){struct _fat_ptr _T0;int _T1;char*_T2;void**_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;void**_T7;unsigned _T8;struct _fat_ptr _T9;void**_TA;unsigned _TB;struct _fat_ptr _TC;unsigned char*_TD;void**_TE;unsigned _TF;int _T10;void**_T11;unsigned char*_T12;void**_T13;int _T14;struct _fat_ptr _T15;_T0=d;_T1=n;_T2=_check_fat_subscript(_T0,sizeof(void*),_T1);_T3=(void**)_T2;{
void*old_rval=*_T3;
if(old_rval!=rval)goto _TLB2;_T4=d;return _T4;_TLB2: _T6=d;{unsigned _T16=
_get_fat_size(_T6,sizeof(void*));_T8=_check_times(_T16,sizeof(void*));{void**_T17=_cycalloc(_T8);_T9=d;{unsigned _T18=_get_fat_size(_T9,sizeof(void*));unsigned i;i=0;_TLB7: if(i < _T18)goto _TLB5;else{goto _TLB6;}_TLB5: _TB=i;_TA=_T17 + _TB;_TC=d;_TD=_TC.curr;_TE=(void**)_TD;_TF=i;_T10=(int)_TF;*_TA=_TE[_T10];i=i + 1;goto _TLB7;_TLB6:;}_T7=(void**)_T17;}_T5=_tag_fat(_T7,sizeof(void*),_T16);}{struct _fat_ptr res=_T5;
_T12=res.curr;_T13=(void**)_T12;_T14=n;_T11=_T13 + _T14;*_T11=rval;_T15=res;
return _T15;}}}struct _tuple16{struct Cyc_List_List*f0;void*f1;};
# 452
static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*path,void*old_val,void*new_val){void*_T0;struct _tuple16 _T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;void*_T5;int*_T6;int _T7;void*_T8;int*_T9;unsigned _TA;struct Cyc_List_List*_TB;void*_TC;void*_TD;struct Cyc_CfFlowInfo_UnionRInfo _TE;struct Cyc_List_List*_TF;struct _fat_ptr _T10;int _T11;char*_T12;void**_T13;void*_T14;void*_T15;struct _fat_ptr _T16;unsigned char*_T17;void**_T18;struct _fat_ptr _T19;unsigned char*_T1A;void**_T1B;void*_T1C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T1D;void*_T1E;void*_T1F;int*_T20;unsigned _T21;struct Cyc_List_List*_T22;void*_T23;void*_T24;void*_T25;struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T26;void*_T27;void*_T28;int*_T29;int _T2A;void*_T2B;void*_T2C;void*_T2D;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T2E;void*_T2F;int(*_T30)(struct _fat_ptr,struct _fat_ptr);void*(*_T31)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T32;struct _fat_ptr _T33;
# 454
if(path!=0)goto _TLB8;_T0=
Cyc_CfFlowInfo_insert_place_inner(new_val,old_val);return _T0;_TLB8:{struct _tuple16 _T34;
_T34.f0=path;_T34.f1=old_val;_T1=_T34;}{struct _tuple16 _T34=_T1;struct Cyc_Absyn_Vardecl*_T35;void*_T36;struct _fat_ptr _T37;int _T38;void*_T39;int _T3A;_T2=_T34.f0;if(_T2==0)goto _TLBA;_T3=_T34.f0;_T4=(struct Cyc_List_List*)_T3;_T5=_T4->hd;_T6=(int*)_T5;_T7=*_T6;if(_T7!=0)goto _TLBC;_T8=_T34.f1;_T9=(int*)_T8;_TA=*_T9;switch(_TA){case 6: _TB=_T34.f0;{struct Cyc_List_List _T3B=*_TB;_TC=_T3B.hd;{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_T3C=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_TC;_T3A=_T3C->f1;}_T39=_T3B.tl;}_TD=_T34.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T3B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TD;_TE=_T3B->f1;_T38=_TE.is_union;_T37=_T3B->f2;}{int i=_T3A;struct Cyc_List_List*tl=_T39;int is_union=_T38;struct _fat_ptr d=_T37;_TF=tl;_T10=d;_T11=i;_T12=_check_fat_subscript(_T10,sizeof(void*),_T11);_T13=(void**)_T12;_T14=*_T13;_T15=new_val;{
# 458
void*new_child=Cyc_CfFlowInfo_insert_place_outer(_TF,_T14,_T15);
struct _fat_ptr new_d=Cyc_CfFlowInfo_aggr_dict_insert(d,i,new_child);_T16=new_d;_T17=_T16.curr;_T18=(void**)_T17;_T19=d;_T1A=_T19.curr;_T1B=(void**)_T1A;
if(_T18!=_T1B)goto _TLBF;_T1C=old_val;return _T1C;_TLBF:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T3B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T3B->tag=6;
_T3B->f1.is_union=is_union;_T3B->f1.fieldnum=-1;_T3B->f2=new_d;_T1D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T3B;}_T1E=(void*)_T1D;return _T1E;}}case 8: goto _LL5;default: goto _LL7;}goto _TLBD;_TLBC: _T1F=_T34.f1;_T20=(int*)_T1F;_T21=*_T20;switch(_T21){case 5: _T22=_T34.f0;{struct Cyc_List_List _T3B=*_T22;_T39=_T3B.tl;}_T23=_T34.f1;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T3B=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T23;_T24=_T3B->f1;_T36=(void*)_T24;}{struct Cyc_List_List*tl=_T39;void*rval=_T36;
# 463
void*new_rval=Cyc_CfFlowInfo_insert_place_outer(tl,rval,new_val);
if(new_rval!=rval)goto _TLC2;_T25=old_val;return _T25;_TLC2:{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T3B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_T3B->tag=5;
_T3B->f1=new_rval;_T26=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T3B;}_T27=(void*)_T26;return _T27;}case 8: goto _LL5;default: goto _LL7;}_TLBD: goto _TLBB;_TLBA: _T28=_T34.f1;_T29=(int*)_T28;_T2A=*_T29;if(_T2A!=8)goto _TLC4;_LL5: _T2B=_T34.f1;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T3B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T2B;_T35=_T3B->f1;_T2C=_T3B->f2;_T39=(void*)_T2C;}{struct Cyc_Absyn_Vardecl*n=_T35;void*rval=_T39;
# 467
void*new_rval=Cyc_CfFlowInfo_insert_place_outer(path,rval,new_val);
if(new_rval!=rval)goto _TLC6;_T2D=old_val;return _T2D;_TLC6:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T3B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T3B->tag=8;
_T3B->f1=n;_T3B->f2=new_rval;_T2E=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T3B;}_T2F=(void*)_T2E;return _T2F;}_TLC4: _LL7: _T31=Cyc_Warn_impos;{
int(*_T3B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T31;_T30=_T3B;}_T32=_tag_fat("bad insert place",sizeof(char),17U);_T33=_tag_fat(0U,sizeof(void*),0);_T30(_T32,_T33);_TLBB:;}}
# 477
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){struct Cyc_CfFlowInfo_EscPile*_T0;struct Cyc_List_List*_T1;struct Cyc_CfFlowInfo_EscPile*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_CfFlowInfo_EscPile*_T5;struct Cyc_CfFlowInfo_EscPile*_T6;struct Cyc_List_List*_T7;struct _handler_cons*_T8;int*_T9;int _TA;void*_TB;struct Cyc_Dict_Absent_exn_struct*_TC;char*_TD;char*_TE;struct Cyc_CfFlowInfo_FlowEnv*_TF;struct Cyc_CfFlowInfo_FlowEnv*_T10;struct Cyc_CfFlowInfo_Place*_T11;struct Cyc_Dict_Dict _T12;void*_T13;struct Cyc_List_List*_T14;void*_T15;void*_T16;void*_T17;struct Cyc_Dict_Dict _T18;
_TLC8: _T0=pile;_T1=_T0->places;if(_T1!=0)goto _TLC9;else{goto _TLCA;}
_TLC9: _T2=pile;_T3=_T2->places;_T4=_T3->hd;{struct Cyc_CfFlowInfo_Place*place=(struct Cyc_CfFlowInfo_Place*)_T4;_T5=pile;_T6=pile;_T7=_T6->places;
_T5->places=_T7->tl;{
void*oldval;void*newval;{struct _handler_cons _T19;_T8=& _T19;_push_handler(_T8);{int _T1A=0;_T9=_T19.handler;_TA=setjmp(_T9);if(!_TA)goto _TLCB;_T1A=1;goto _TLCC;_TLCB: _TLCC: if(_T1A)goto _TLCD;else{goto _TLCF;}_TLCF:
 oldval=Cyc_CfFlowInfo_lookup_place(d,place);_pop_handler();goto _TLCE;_TLCD: _TB=Cyc_Core_get_exn_thrown();{void*_T1B=(void*)_TB;void*_T1C;_TC=(struct Cyc_Dict_Absent_exn_struct*)_T1B;_TD=_TC->tag;_TE=Cyc_Dict_Absent;if(_TD!=_TE)goto _TLD0;goto _TLC8;_TLD0: _T1C=_T1B;{void*exn=_T1C;_rethrow(exn);};}_TLCE:;}}{
# 484
enum Cyc_CfFlowInfo_InitLevel _T19=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);if(_T19!=Cyc_CfFlowInfo_AllIL)goto _TLD2;_TF=fenv;
newval=_TF->esc_all;goto _LL5;_TLD2: _T10=fenv;
newval=_T10->esc_none;goto _LL5;_LL5:;}
# 488
Cyc_CfFlowInfo_add_places(pile,oldval);_T11=place;{
struct Cyc_CfFlowInfo_Place _T19=*_T11;struct Cyc_List_List*_T1A;void*_T1B;_T1B=_T19.root;_T1A=_T19.path;{void*root=_T1B;struct Cyc_List_List*path=_T1A;_T12=d;_T13=root;_T14=path;_T15=
# 492
Cyc_Dict_lookup(d,root);_T16=newval;_T17=
# 491
Cyc_CfFlowInfo_insert_place_outer(_T14,_T15,_T16);
# 490
d=Cyc_Dict_insert(_T12,_T13,_T17);}}}}goto _TLC8;_TLCA: _T18=d;
# 495
return _T18;}struct Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 505
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){void*_T0;int*_T1;unsigned _T2;void*_T3;enum Cyc_CfFlowInfo_InitLevel _T4;void*_T5;enum Cyc_CfFlowInfo_InitLevel _T6;void*_T7;void*_T8;int(*_T9)(struct _fat_ptr,struct _fat_ptr);void*(*_TA)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TB;struct _fat_ptr _TC;void*_TD;enum Cyc_CfFlowInfo_InitLevel _TE;_T0=r;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 2: _T3=r;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_TF=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T3;_TE=_TF->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_TE;_T4=il;
# 507
return _T4;}case 3: _T5=r;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_TF=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_T5;_TE=_TF->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_TE;_T6=il;
return _T6;}case 0: goto _LL8;case 1: _LL8:
# 510
 return 1U;case 7: _T7=r;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TF=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T7;_T8=_TF->f3;_TD=(void*)_T8;}{void*r=_TD;
return 0U;}default:
# 513
 Cyc_CfFlowInfo_print_absrval(r);_TA=Cyc_Warn_impos;{
int(*_TF)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TA;_T9=_TF;}_TB=_tag_fat("initlevel_approx",sizeof(char),17U);_TC=_tag_fat(0U,sizeof(void*),0);_T9(_TB,_TC);};}
# 517
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){enum Cyc_CfFlowInfo_InitLevel _T0;int _T1;void*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;enum Cyc_CfFlowInfo_InitLevel _T7;void*_T8;struct Cyc_CfFlowInfo_UnionRInfo _T9;struct Cyc_CfFlowInfo_UnionRInfo _TA;int _TB;struct _fat_ptr _TC;int _TD;unsigned _TE;unsigned _TF;struct Cyc_CfFlowInfo_InitlevelEnv*_T10;struct _fat_ptr _T11;unsigned char*_T12;void**_T13;int _T14;void*_T15;enum Cyc_CfFlowInfo_InitLevel _T16;int _T17;struct Cyc_CfFlowInfo_InitlevelEnv*_T18;struct _fat_ptr _T19;int _T1A;char*_T1B;void**_T1C;void*_T1D;enum Cyc_CfFlowInfo_InitLevel _T1E;int _T1F;void*_T20;struct _fat_ptr _T21;int _T22;unsigned _T23;unsigned _T24;struct Cyc_CfFlowInfo_InitlevelEnv*_T25;struct _fat_ptr _T26;unsigned char*_T27;void**_T28;int _T29;void*_T2A;enum Cyc_CfFlowInfo_InitLevel _T2B;void*_T2C;int(*_T2D)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*);int(*_T2E)(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_CfFlowInfo_InitlevelEnv*_T2F;struct Cyc_List_List*_T30;struct Cyc_CfFlowInfo_Place*_T31;int _T32;struct Cyc_CfFlowInfo_InitlevelEnv*_T33;struct Cyc_List_List*_T34;struct Cyc_CfFlowInfo_InitlevelEnv*_T35;struct Cyc_CfFlowInfo_InitlevelEnv*_T36;struct Cyc_CfFlowInfo_InitlevelEnv*_T37;struct Cyc_Dict_Dict _T38;struct Cyc_CfFlowInfo_Place*_T39;void*_T3A;struct Cyc_CfFlowInfo_InitlevelEnv*_T3B;struct Cyc_CfFlowInfo_InitlevelEnv*_T3C;struct Cyc_List_List*_T3D;struct Cyc_List_List*_T3E;void*_T3F;void*_T40;enum Cyc_CfFlowInfo_InitLevel _T41;
# 519
enum Cyc_CfFlowInfo_InitLevel this_ans;_T0=acc;_T1=(int)_T0;
if(_T1!=0)goto _TLD5;return 0U;_TLD5:{struct _fat_ptr _T42;int _T43;int _T44;void*_T45;_T2=r;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 8: _T5=r;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T46=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T5;_T6=_T46->f2;_T45=(void*)_T6;}{void*r=_T45;_T7=
# 522
Cyc_CfFlowInfo_initlevel_rec(env,r,acc);return _T7;}case 6: _T8=r;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T46=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T8;_T9=_T46->f1;_T44=_T9.is_union;_TA=_T46->f1;_T43=_TA.fieldnum;_T42=_T46->f2;}_TB=(int)_T44;if(!_TB)goto _TLD8;{int iu=_T44;int f=_T43;struct _fat_ptr d=_T42;_TC=d;{
# 527
unsigned sz=_get_fat_size(_TC,sizeof(void*));
this_ans=0U;
if(f!=-1)goto _TLDA;{
int i=0;_TLDF: _TD=i;_TE=(unsigned)_TD;_TF=sz;if(_TE < _TF)goto _TLDD;else{goto _TLDE;}
_TLDD: _T10=env;_T11=d;_T12=_T11.curr;_T13=(void**)_T12;_T14=i;_T15=_T13[_T14];_T16=Cyc_CfFlowInfo_initlevel_rec(_T10,_T15,1U);_T17=(int)_T16;if(_T17!=1)goto _TLE0;
this_ans=1U;goto _TLDE;_TLE0:
# 530
 i=i + 1;goto _TLDF;_TLDE:;}goto _TLDB;
# 535
_TLDA: _T18=env;_T19=d;_T1A=f;_T1B=_check_fat_subscript(_T19,sizeof(void*),_T1A);_T1C=(void**)_T1B;_T1D=*_T1C;_T1E=Cyc_CfFlowInfo_initlevel_rec(_T18,_T1D,1U);_T1F=(int)_T1E;if(_T1F!=1)goto _TLE2;
this_ans=1U;goto _TLE3;_TLE2: _TLE3: _TLDB: goto _LL0;}}_TLD8: _T20=r;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T46=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T20;_T42=_T46->f2;}{struct _fat_ptr d=_T42;_T21=d;{
# 539
unsigned sz=_get_fat_size(_T21,sizeof(void*));
this_ans=1U;{
int i=0;_TLE7: _T22=i;_T23=(unsigned)_T22;_T24=sz;if(_T23 < _T24)goto _TLE5;else{goto _TLE6;}
_TLE5: _T25=env;_T26=d;_T27=_T26.curr;_T28=(void**)_T27;_T29=i;_T2A=_T28[_T29];_T2B=this_ans;this_ans=Cyc_CfFlowInfo_initlevel_rec(_T25,_T2A,_T2B);
# 541
i=i + 1;goto _TLE7;_TLE6:;}goto _LL0;}}case 4: _T2C=r;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T46=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T2C;_T45=_T46->f1;}{struct Cyc_CfFlowInfo_Place*p=_T45;_T2E=Cyc_List_mem;{
# 545
int(*_T46)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*)=(int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))_T2E;_T2D=_T46;}_T2F=env;_T30=_T2F->seen;_T31=p;_T32=_T2D(Cyc_CfFlowInfo_place_cmp,_T30,_T31);if(!_T32)goto _TLE8;
this_ans=1U;goto _TLE9;
# 548
_TLE8: _T33=env;{struct Cyc_List_List*_T46=_cycalloc(sizeof(struct Cyc_List_List));_T46->hd=p;_T35=env;_T46->tl=_T35->seen;_T34=(struct Cyc_List_List*)_T46;}_T33->seen=_T34;_T36=env;_T37=env;_T38=_T37->d;_T39=p;_T3A=
Cyc_CfFlowInfo_lookup_place(_T38,_T39);this_ans=Cyc_CfFlowInfo_initlevel_rec(_T36,_T3A,1U);_T3B=env;_T3C=env;_T3D=_T3C->seen;_T3E=
_check_null(_T3D);_T3B->seen=_T3E->tl;_TLE9: goto _LL0;}case 5: _T3F=r;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T46=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T3F;_T40=_T46->f1;_T45=(void*)_T40;}{void*r=_T45;
# 553
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,r,1U);goto _LL0;}default:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);goto _LL0;}_LL0:;}_T41=this_ans;
# 556
return _T41;}
# 558
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){struct Cyc_CfFlowInfo_InitlevelEnv _T0;struct Cyc_CfFlowInfo_InitlevelEnv*_T1;struct Cyc_CfFlowInfo_InitlevelEnv*_T2;void*_T3;enum Cyc_CfFlowInfo_InitLevel _T4;{struct Cyc_CfFlowInfo_InitlevelEnv _T5;
_T5.d=d;_T5.seen=0;_T0=_T5;}{struct Cyc_CfFlowInfo_InitlevelEnv env2=_T0;_T1=& env2;_T2=(struct Cyc_CfFlowInfo_InitlevelEnv*)_T1;_T3=r;_T4=
Cyc_CfFlowInfo_initlevel_rec(_T2,_T3,1U);return _T4;}}
# 563
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){struct Cyc_CfFlowInfo_Place*_T0;struct _tuple11 _T1;struct Cyc_List_List*_T2;void*_T3;int*_T4;unsigned _T5;void*_T6;void*_T7;void*_T8;void*_T9;void*_TA;int*_TB;int _TC;void*_TD;void*_TE;struct _fat_ptr _TF;int _T10;char*_T11;void**_T12;void*_T13;int*_T14;int _T15;void*_T16;void*_T17;int(*_T18)(struct _fat_ptr,struct _fat_ptr);void*(*_T19)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T1A;struct _fat_ptr _T1B;struct Cyc_List_List*_T1C;void*_T1D;_T0=place;{
struct Cyc_CfFlowInfo_Place _T1E=*_T0;struct Cyc_List_List*_T1F;void*_T20;_T20=_T1E.root;_T1F=_T1E.path;{void*root=_T20;struct Cyc_List_List*path=_T1F;
void*ans=Cyc_Dict_lookup(d,root);
_TLED: if(path!=0)goto _TLEB;else{goto _TLEC;}
_TLEB: retry:{struct _tuple11 _T21;_T21.f0=ans;_T2=path;_T21.f1=_T2->hd;_T1=_T21;}{struct _tuple11 _T21=_T1;int _T22;struct _fat_ptr _T23;struct Cyc_CfFlowInfo_UnionRInfo _T24;void*_T25;_T3=_T21.f0;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 8: _T6=_T21.f0;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T26=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T6;_T7=_T26->f2;_T25=(void*)_T7;}{void*rval=_T25;
# 569
ans=rval;goto retry;}case 7: _T8=_T21.f0;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T26=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T8;_T9=_T26->f3;_T25=(void*)_T9;}{void*rval=_T25;
# 572
ans=rval;goto retry;}case 6: _TA=_T21.f1;_TB=(int*)_TA;_TC=*_TB;if(_TC!=0)goto _TLEF;_TD=_T21.f0;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T26=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TD;_T24=_T26->f1;_T23=_T26->f2;}_TE=_T21.f1;{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_T26=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_TE;_T22=_T26->f1;}{struct Cyc_CfFlowInfo_UnionRInfo is_union=_T24;struct _fat_ptr d2=_T23;int fname=_T22;_TF=d2;_T10=fname;_T11=_check_fat_subscript(_TF,sizeof(void*),_T10);_T12=(void**)_T11;
ans=*_T12;goto _LL3;}_TLEF: goto _LLC;case 5: _T13=_T21.f1;_T14=(int*)_T13;_T15=*_T14;if(_T15!=1)goto _TLF1;_T16=_T21.f0;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T26=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T16;_T17=_T26->f1;_T25=(void*)_T17;}{void*rval=_T25;
ans=rval;goto _LL3;}_TLF1: goto _LLC;default: _LLC: _T19=Cyc_Warn_impos;{
# 581
int(*_T26)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T19;_T18=_T26;}_T1A=_tag_fat("bad lookup_place",sizeof(char),17U);_T1B=_tag_fat(0U,sizeof(void*),0);_T18(_T1A,_T1B);}_LL3:;}_T1C=path;
# 566
path=_T1C->tl;goto _TLED;_TLEC: _T1D=ans;
# 583
return _T1D;}}}
# 586
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;int _T5;void*_T6;void*_T7;int _T8;void*_T9;struct Cyc_CfFlowInfo_UnionRInfo _TA;struct Cyc_CfFlowInfo_UnionRInfo _TB;int _TC;struct _fat_ptr _TD;int _TE;char*_TF;void**_T10;void*_T11;int _T12;int _T13;struct _fat_ptr _T14;int _T15;unsigned _T16;unsigned _T17;struct _fat_ptr _T18;unsigned char*_T19;void**_T1A;int _T1B;void*_T1C;int _T1D;struct _fat_ptr _T1E;int _T1F;int _T20;void*_T21;_T0=rval;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 3:
# 588
 return 0;case 7: _T3=rval;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T22=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T3;_T4=_T22->f3;_T21=(void*)_T4;}{void*r=_T21;_T5=
Cyc_CfFlowInfo_is_rval_unescaped(r);return _T5;}case 8: _T6=rval;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T22=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T6;_T7=_T22->f2;_T21=(void*)_T7;}{void*r=_T21;_T8=
Cyc_CfFlowInfo_is_rval_unescaped(r);return _T8;}case 6: _T9=rval;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T22=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T9;_TA=_T22->f1;_T20=_TA.is_union;_TB=_T22->f1;_T1F=_TB.fieldnum;_T1E=_T22->f2;}{int is_union=_T20;int field_no=_T1F;struct _fat_ptr d=_T1E;_TC=is_union;
# 592
if(!_TC)goto _TLF4;if(field_no==-1)goto _TLF4;_TD=d;_TE=field_no;_TF=_check_fat_subscript(_TD,sizeof(void*),_TE);_T10=(void**)_TF;_T11=*_T10;_T12=
Cyc_CfFlowInfo_is_rval_unescaped(_T11);_T13=!_T12;return _T13;
# 595
_TLF4: _T14=d;{unsigned sz=_get_fat_size(_T14,sizeof(void*));{
int i=0;_TLF9: _T15=i;_T16=(unsigned)_T15;_T17=sz;if(_T16 < _T17)goto _TLF7;else{goto _TLF8;}
_TLF7: _T18=d;_T19=_T18.curr;_T1A=(void**)_T19;_T1B=i;_T1C=_T1A[_T1B];_T1D=Cyc_CfFlowInfo_is_rval_unescaped(_T1C);if(_T1D)goto _TLFA;else{goto _TLFC;}_TLFC: return 0;_TLFA:
# 596
 i=i + 1;goto _TLF9;_TLF8:;}
# 598
return 1;}}default:
# 600
 return 1;};}
# 603
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){void*_T0;int _T1;_T0=
Cyc_CfFlowInfo_lookup_place(d,place);_T1=Cyc_CfFlowInfo_is_rval_unescaped(_T0);return _T1;}
# 606
int Cyc_CfFlowInfo_is_init_pointer(void*rval){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;int _T5;void*_T6;_T0=rval;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 8: _T3=rval;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T3;_T4=_T7->f2;_T6=(void*)_T4;}{void*r=_T6;_T5=
# 608
Cyc_CfFlowInfo_is_init_pointer(r);return _T5;}case 4: goto _LL6;case 5: _LL6:
# 610
 return 1;default:
 return 0;};}
# 616
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){struct Cyc_CfFlowInfo_EscPile*_T0;struct _RegionHandle _T1=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _T1;_push_region(rgn);{
# 618
struct Cyc_CfFlowInfo_EscPile*pile;pile=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_T0=pile;_T0->places=0;
Cyc_CfFlowInfo_add_places(pile,r);{struct Cyc_Dict_Dict _T2=
Cyc_CfFlowInfo_escape_these(fenv,pile,d);_npop_handler(0);return _T2;}}_pop_region();}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 630
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){struct _tuple11 _T0;void*_T1;int*_T2;int _T3;void*_T4;void*_T5;void*_T6;void*_T7;int*_T8;int _T9;void*_TA;void*_TB;void*_TC;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TD;void*_TE;void*_TF;int*_T10;unsigned _T11;void*_T12;int*_T13;int _T14;void*_T15;struct Cyc_CfFlowInfo_AssignEnv*_T16;struct Cyc_CfFlowInfo_EscPile*_T17;struct Cyc_CfFlowInfo_Place*_T18;struct Cyc_CfFlowInfo_AssignEnv*_T19;struct Cyc_CfFlowInfo_FlowEnv*_T1A;struct Cyc_CfFlowInfo_AssignEnv*_T1B;struct Cyc_Dict_Dict _T1C;void*_T1D;enum Cyc_CfFlowInfo_InitLevel _T1E;int _T1F;struct Cyc_CfFlowInfo_AssignEnv*_T20;unsigned _T21;struct _fat_ptr _T22;struct _fat_ptr _T23;struct Cyc_CfFlowInfo_AssignEnv*_T24;struct Cyc_CfFlowInfo_FlowEnv*_T25;void*_T26;void*_T27;int*_T28;unsigned _T29;void*_T2A;void*_T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;void**_T2E;unsigned _T2F;struct _fat_ptr _T30;void**_T31;unsigned _T32;struct Cyc_CfFlowInfo_AssignEnv*_T33;struct _fat_ptr _T34;unsigned _T35;int _T36;char*_T37;void**_T38;void*_T39;struct _fat_ptr _T3A;unsigned _T3B;int _T3C;char*_T3D;void**_T3E;void*_T3F;int _T40;unsigned _T41;struct _fat_ptr _T42;unsigned _T43;struct _fat_ptr _T44;int _T45;char*_T46;void**_T47;void*_T48;struct _fat_ptr _T49;unsigned char*_T4A;void**_T4B;int _T4C;void*_T4D;int _T4E;struct Cyc_CfFlowInfo_UnionRInfo _T4F;int _T50;void*_T51;int _T52;unsigned _T53;struct _fat_ptr _T54;unsigned _T55;struct _fat_ptr _T56;int _T57;char*_T58;void**_T59;void*_T5A;struct _fat_ptr _T5B;int _T5C;char*_T5D;void**_T5E;void*_T5F;int _T60;struct Cyc_CfFlowInfo_UnionRInfo _T61;int _T62;void*_T63;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T64;void*_T65;void*_T66;int*_T67;int _T68;void*_T69;enum Cyc_CfFlowInfo_InitLevel _T6A;struct Cyc_CfFlowInfo_AssignEnv*_T6B;struct Cyc_CfFlowInfo_FlowEnv*_T6C;void*_T6D;struct Cyc_CfFlowInfo_AssignEnv*_T6E;struct Cyc_CfFlowInfo_FlowEnv*_T6F;void*_T70;void*_T71;{struct _tuple11 _T72;
# 635
_T72.f0=oldval;_T72.f1=newval;_T0=_T72;}{struct _tuple11 _T72=_T0;enum Cyc_CfFlowInfo_InitLevel _T73;struct _fat_ptr _T74;struct Cyc_CfFlowInfo_UnionRInfo _T75;struct _fat_ptr _T76;struct Cyc_CfFlowInfo_UnionRInfo _T77;struct Cyc_CfFlowInfo_Place*_T78;struct Cyc_Absyn_Vardecl*_T79;void*_T7A;_T1=_T72.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=8)goto _TLFE;_T4=_T72.f0;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T7B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T4;_T5=_T7B->f2;_T7A=(void*)_T5;}{void*r1=_T7A;_T6=
# 637
Cyc_CfFlowInfo_assign_place_inner(env,r1,newval);return _T6;}_TLFE: _T7=_T72.f1;_T8=(int*)_T7;_T9=*_T8;if(_T9!=8)goto _TL100;_TA=_T72.f1;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T7B=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TA;_T79=_T7B->f1;_TB=_T7B->f2;_T7A=(void*)_TB;}{struct Cyc_Absyn_Vardecl*n=_T79;void*r=_T7A;
# 639
void*new_rval=Cyc_CfFlowInfo_assign_place_inner(env,oldval,r);
if(new_rval!=r)goto _TL102;_TC=newval;return _TC;
_TL102:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T7B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T7B->tag=8;_T7B->f1=n;_T7B->f2=new_rval;_TD=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T7B;}_TE=(void*)_TD;return _TE;}_TL100: _TF=_T72.f0;_T10=(int*)_TF;_T11=*_T10;switch(_T11){case 3: _T12=_T72.f1;_T13=(int*)_T12;_T14=*_T13;if(_T14!=4)goto _TL105;_T15=_T72.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T7B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T15;_T78=_T7B->f1;}{struct Cyc_CfFlowInfo_Place*p=_T78;_T16=env;_T17=_T16->pile;_T18=p;
Cyc_CfFlowInfo_add_place(_T17,_T18);goto _LL8;}_TL105: _LL8: _T19=env;_T1A=_T19->fenv;_T1B=env;_T1C=_T1B->d;_T1D=newval;_T1E=
# 644
Cyc_CfFlowInfo_initlevel(_T1A,_T1C,_T1D);_T1F=(int)_T1E;if(_T1F==1)goto _TL107;_T20=env;_T21=_T20->loc;_T22=
# 646
_tag_fat("assignment puts possibly-uninitialized data in an escaped location",sizeof(char),67U);_T23=_tag_fat(0U,sizeof(void*),0);
# 645
Cyc_CfFlowInfo_aerr(_T21,_T22,_T23);goto _TL108;_TL107: _TL108: _T24=env;_T25=_T24->fenv;_T26=_T25->esc_all;
# 647
return _T26;case 6: _T27=_T72.f1;_T28=(int*)_T27;_T29=*_T28;switch(_T29){case 6: _T2A=_T72.f0;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T7B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T2A;_T77=_T7B->f1;_T76=_T7B->f2;}_T2B=_T72.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T7B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T2B;_T75=_T7B->f1;_T74=_T7B->f2;}{struct Cyc_CfFlowInfo_UnionRInfo is_union1=_T77;struct _fat_ptr d1=_T76;struct Cyc_CfFlowInfo_UnionRInfo is_union2=_T75;struct _fat_ptr d2=_T74;_T2D=d1;{unsigned _T7B=
# 650
_get_fat_size(_T2D,sizeof(void*));_T2F=_check_times(_T7B,sizeof(void*));{void**_T7C=_cycalloc(_T2F);_T30=d1;{unsigned _T7D=_get_fat_size(_T30,sizeof(void*));unsigned i;i=0;_TL10D: if(i < _T7D)goto _TL10B;else{goto _TL10C;}_TL10B: _T32=i;_T31=_T7C + _T32;_T33=env;_T34=d1;_T35=i;_T36=(int)_T35;_T37=_check_fat_subscript(_T34,sizeof(void*),_T36);_T38=(void**)_T37;_T39=*_T38;_T3A=d2;_T3B=i;_T3C=(int)_T3B;_T3D=_check_fat_subscript(_T3A,sizeof(void*),_T3C);_T3E=(void**)_T3D;_T3F=*_T3E;*_T31=Cyc_CfFlowInfo_assign_place_inner(_T33,_T39,_T3F);i=i + 1;goto _TL10D;_TL10C:;}_T2E=(void**)_T7C;}_T2C=_tag_fat(_T2E,sizeof(void*),_T7B);}{
# 649
struct _fat_ptr new_d=_T2C;
# 653
int change=0;{
int i=0;_TL111: _T40=i;_T41=(unsigned)_T40;_T42=d1;_T43=_get_fat_size(_T42,sizeof(void*));if(_T41 < _T43)goto _TL10F;else{goto _TL110;}
_TL10F: _T44=new_d;_T45=i;_T46=_check_fat_subscript(_T44,sizeof(void*),_T45);_T47=(void**)_T46;_T48=*_T47;_T49=d1;_T4A=_T49.curr;_T4B=(void**)_T4A;_T4C=i;_T4D=_T4B[_T4C];if(_T48==_T4D)goto _TL112;
change=1;goto _TL110;_TL112:
# 654
 i=i + 1;goto _TL111;_TL110:;}_T4E=change;
# 658
if(_T4E)goto _TL114;else{goto _TL116;}
_TL116: _T4F=is_union1;_T50=_T4F.is_union;if(_T50)goto _TL117;else{goto _TL119;}_TL119: _T51=oldval;return _T51;_TL117:
 new_d=d1;goto _TL115;
# 663
_TL114: change=0;{
int i=0;_TL11D: _T52=i;_T53=(unsigned)_T52;_T54=d1;_T55=_get_fat_size(_T54,sizeof(void*));if(_T53 < _T55)goto _TL11B;else{goto _TL11C;}
_TL11B: _T56=new_d;_T57=i;_T58=_check_fat_subscript(_T56,sizeof(void*),_T57);_T59=(void**)_T58;_T5A=*_T59;_T5B=d2;_T5C=i;_T5D=_check_fat_subscript(_T5B,sizeof(void*),_T5C);_T5E=(void**)_T5D;_T5F=*_T5E;if(_T5A==_T5F)goto _TL11E;
change=1;goto _TL11C;_TL11E:
# 664
 i=i + 1;goto _TL11D;_TL11C:;}_T60=change;
# 668
if(_T60)goto _TL120;else{goto _TL122;}
_TL122: _T61=is_union1;_T62=_T61.is_union;if(_T62)goto _TL123;else{goto _TL125;}_TL125: _T63=newval;return _T63;_TL123:
 new_d=d2;goto _TL121;_TL120: _TL121: _TL115:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T7B=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T7B->tag=6;
# 673
_T7B->f1=is_union2;_T7B->f2=new_d;_T64=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T7B;}_T65=(void*)_T64;return _T65;}}case 3: goto _LLB;default: goto _LLD;};default: _T66=_T72.f1;_T67=(int*)_T66;_T68=*_T67;if(_T68!=3)goto _TL126;_LLB: _T69=_T72.f1;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_T7B=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_T69;_T73=_T7B->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_T73;_T6A=il;if(_T6A!=Cyc_CfFlowInfo_NoneIL)goto _TL128;_T6B=env;_T6C=_T6B->fenv;_T6D=_T6C->unknown_none;
# 676
return _T6D;_TL128: _T6E=env;_T6F=_T6E->fenv;_T70=_T6F->unknown_all;
return _T70;;}goto _TL127;_TL126: _LLD: _T71=newval;
# 679
return _T71;_TL127:;};}}
# 685
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){int*_T0;int _T1;int*_T2;int*_T3;int _T4;_T0=n;_T1=*_T0;
if(_T1 <= 0)goto _TL12A;_T2=n;_T3=n;_T4=*_T3;*_T2=_T4 - 1;return 1;
_TL12A: return 0;}
# 689
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*path,int path_prefix,void*oldval,void*newval){void*_T0;struct _tuple16 _T1;void*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;void*_T7;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T8;void*_T9;void*_TA;void*_TB;void*_TC;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TD;void*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;void*_T12;int*_T13;int _T14;void*_T15;int*_T16;int _T17;struct Cyc_List_List*_T18;void*_T19;void*_T1A;struct Cyc_CfFlowInfo_AssignEnv*_T1B;struct Cyc_List_List*_T1C;int _T1D;void*_T1E;void*_T1F;void*_T20;struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T21;void*_T22;void*_T23;int*_T24;int _T25;struct Cyc_List_List*_T26;void*_T27;void*_T28;struct Cyc_CfFlowInfo_UnionRInfo _T29;struct Cyc_CfFlowInfo_UnionRInfo _T2A;struct Cyc_CfFlowInfo_AssignEnv*_T2B;struct Cyc_List_List*_T2C;int _T2D;struct _fat_ptr _T2E;int _T2F;char*_T30;void**_T31;void*_T32;void*_T33;struct _fat_ptr _T34;unsigned char*_T35;void**_T36;struct _fat_ptr _T37;unsigned char*_T38;void**_T39;int _T3A;void*_T3B;int _T3C;struct _fat_ptr _T3D;unsigned _T3E;struct _fat_ptr _T3F;int _T40;struct Cyc_CfFlowInfo_AssignEnv*_T41;struct Cyc_CfFlowInfo_FlowEnv*_T42;void*_T43;struct _fat_ptr _T44;int _T45;char*_T46;void**_T47;void*_T48;void*_T49;struct _fat_ptr _T4A;unsigned char*_T4B;void**_T4C;struct _fat_ptr _T4D;unsigned char*_T4E;void**_T4F;int _T50;struct Cyc_CfFlowInfo_AssignEnv*_T51;struct Cyc_List_List*(*_T52)(int(*)(int*,void*),int*,struct Cyc_List_List*);struct Cyc_List_List*(*_T53)(int(*)(void*,void*),void*,struct Cyc_List_List*);int*_T54;struct Cyc_List_List*_T55;struct Cyc_CfFlowInfo_Place*_T56;struct Cyc_CfFlowInfo_Place*_T57;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T58;void*_T59;int(*_T5A)(struct _fat_ptr,struct _fat_ptr);void*(*_T5B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T5C;struct _fat_ptr _T5D;
# 700
if(path!=0)goto _TL12C;_T0=Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);return _T0;_TL12C:{struct _tuple16 _T5E;
_T5E.f0=path;_T5E.f1=oldval;_T1=_T5E;}{struct _tuple16 _T5E=_T1;struct _fat_ptr _T5F;int _T60;struct Cyc_List_List*_T61;int _T62;int _T63;void*_T64;void*_T65;_T2=_T5E.f1;_T3=(int*)_T2;_T4=*_T3;switch(_T4){case 8: _T5=_T5E.f1;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T66=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T5;_T65=_T66->f1;_T6=_T66->f2;_T64=(void*)_T6;}{struct Cyc_Absyn_Vardecl*n=_T65;void*r=_T64;
# 703
void*new_r=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);
if(new_r!=r)goto _TL12F;_T7=oldval;return _T7;
_TL12F:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T66=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T66->tag=8;_T66->f1=n;_T66->f2=new_r;_T8=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T66;}_T9=(void*)_T8;return _T9;}case 7: _TA=_T5E.f1;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T66=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_TA;_T65=_T66->f1;_T63=_T66->f2;_TB=_T66->f3;_T64=(void*)_TB;_T62=_T66->f4;}{struct Cyc_Absyn_Exp*x=_T65;int y=_T63;void*r=_T64;int l=_T62;
# 707
void*new_r=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);
if(new_r!=r)goto _TL131;_TC=oldval;return _TC;
_TL131:{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T66=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_T66->tag=7;_T66->f1=x;_T66->f2=y;_T66->f3=new_r;_T66->f4=l;_TD=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T66;}_TE=(void*)_TD;return _TE;}default: _TF=_T5E.f0;if(_TF==0)goto _TL133;_T10=_T5E.f0;_T11=(struct Cyc_List_List*)_T10;_T12=_T11->hd;_T13=(int*)_T12;_T14=*_T13;if(_T14!=1)goto _TL135;_T15=_T5E.f1;_T16=(int*)_T15;_T17=*_T16;if(_T17!=5)goto _TL137;_T18=_T5E.f0;{struct Cyc_List_List _T66=*_T18;_T61=_T66.tl;}_T19=_T5E.f1;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T66=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T19;_T1A=_T66->f1;_T65=(void*)_T1A;}{struct Cyc_List_List*tl=_T61;void*r=_T65;_T1B=env;_T1C=tl;_T1D=path_prefix + 1;_T1E=r;_T1F=newval;{
# 711
void*new_r=Cyc_CfFlowInfo_assign_place_outer(_T1B,_T1C,_T1D,_T1E,_T1F);
if(new_r!=r)goto _TL139;_T20=oldval;return _T20;
_TL139:{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T66=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_T66->tag=5;_T66->f1=new_r;_T21=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T66;}_T22=(void*)_T21;return _T22;}}_TL137: goto _LL9;_TL135: _T23=_T5E.f1;_T24=(int*)_T23;_T25=*_T24;if(_T25!=6)goto _TL13B;_T26=_T5E.f0;{struct Cyc_List_List _T66=*_T26;_T27=_T66.hd;{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_T67=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_T27;_T63=_T67->f1;}_T61=_T66.tl;}_T28=_T5E.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T66=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T28;_T29=_T66->f1;_T62=_T29.is_union;_T2A=_T66->f1;_T60=_T2A.fieldnum;_T5F=_T66->f2;}{int fnum=_T63;struct Cyc_List_List*tl=_T61;int is_union=_T62;int fldnum=_T60;struct _fat_ptr d=_T5F;_T2B=env;_T2C=tl;_T2D=path_prefix + 1;_T2E=d;_T2F=fnum;_T30=_check_fat_subscript(_T2E,sizeof(void*),_T2F);_T31=(void**)_T30;_T32=*_T31;_T33=newval;{
# 715
void*new_child=Cyc_CfFlowInfo_assign_place_outer(_T2B,_T2C,_T2D,_T32,_T33);
# 717
struct _fat_ptr new_child_agg=Cyc_CfFlowInfo_aggr_dict_insert(d,fnum,new_child);_T34=new_child_agg;_T35=_T34.curr;_T36=(void**)_T35;_T37=d;_T38=_T37.curr;_T39=(void**)_T38;
if(_T36!=_T39)goto _TL13D;_T3A=is_union;if(_T3A)goto _TL140;else{goto _TL13F;}_TL140: if(fldnum==fnum)goto _TL13F;else{goto _TL13D;}_TL13F: _T3B=oldval;return _T3B;_TL13D:
 d=new_child_agg;_T3C=is_union;
# 722
if(!_T3C)goto _TL141;{
int changed=0;_T3D=d;_T3E=
_get_fat_size(_T3D,sizeof(void*));{int sz=(int)_T3E;{
int i=0;_TL146: if(i < sz)goto _TL144;else{goto _TL145;}
_TL144: if(i==fnum)goto _TL147;_T3F=d;_T40=i;_T41=env;_T42=_T41->fenv;_T43=_T42->unknown_all;_T44=d;_T45=i;_T46=_check_fat_subscript(_T44,sizeof(void*),_T45);_T47=(void**)_T46;_T48=*_T47;_T49=
# 729
Cyc_CfFlowInfo_insert_place_inner(_T43,_T48);{
# 727
struct _fat_ptr new_d=
Cyc_CfFlowInfo_aggr_dict_insert(_T3F,_T40,_T49);_T4A=new_d;_T4B=_T4A.curr;_T4C=(void**)_T4B;_T4D=d;_T4E=_T4D.curr;_T4F=(void**)_T4E;
# 730
if(_T4C==_T4F)goto _TL149;
d=new_d;
changed=1;goto _TL14A;_TL149: _TL14A:;}goto _TL148;_TL147: _TL148:
# 725
 i=i + 1;goto _TL146;_TL145:;}_T50=changed;
# 738
if(!_T50)goto _TL14B;_T51=env;{
struct Cyc_CfFlowInfo_Place*_T66=_T51->root_place;struct Cyc_List_List*_T67;void*_T68;{struct Cyc_CfFlowInfo_Place _T69=*_T66;_T68=_T69.root;_T67=_T69.path;}{void*root=_T68;struct Cyc_List_List*path=_T67;_T53=Cyc_List_filter_c;{
struct Cyc_List_List*(*_T69)(int(*)(int*,void*),int*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(int*,void*),int*,struct Cyc_List_List*))_T53;_T52=_T69;}{int*_T69=_cycalloc_atomic(sizeof(int));*_T69=path_prefix;_T54=(int*)_T69;}_T55=path;{struct Cyc_List_List*new_path=_T52(Cyc_CfFlowInfo_nprefix,_T54,_T55);
struct Cyc_CfFlowInfo_Place*curr_place;curr_place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));_T56=curr_place;_T56->root=root;_T57=curr_place;_T57->path=new_path;}}}goto _TL14C;_TL14B: _TL14C:;}}goto _TL142;_TL141: _TL142:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T66=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T66->tag=6;
# 744
_T66->f1.is_union=is_union;_T66->f1.fieldnum=fnum;_T66->f2=d;_T58=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T66;}_T59=(void*)_T58;return _T59;}}_TL13B: goto _LL9;_TL133: _LL9: _T5B=Cyc_Warn_impos;{
int(*_T66)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T5B;_T5A=_T66;}_T5C=_tag_fat("bad assign place",sizeof(char),17U);_T5D=_tag_fat(0U,sizeof(void*),0);_T5A(_T5C,_T5D);;};}}
# 748
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){struct Cyc_CfFlowInfo_Place*_T0;struct Cyc_CfFlowInfo_AssignEnv _T1;struct Cyc_CfFlowInfo_EscPile*_T2;struct Cyc_CfFlowInfo_AssignEnv*_T3;struct Cyc_CfFlowInfo_AssignEnv*_T4;struct Cyc_List_List*_T5;void*_T6;void*_T7;struct Cyc_CfFlowInfo_FlowEnv*_T8;struct Cyc_CfFlowInfo_AssignEnv _T9;struct Cyc_CfFlowInfo_EscPile*_TA;struct Cyc_Dict_Dict _TB;struct Cyc_Dict_Dict _TC;struct Cyc_List_List*_TD;void*_TE;_T0=place;{struct Cyc_CfFlowInfo_Place _TF=*_T0;_TE=_TF.root;_TD=_TF.path;}{void*root=_TE;struct Cyc_List_List*path=_TD;{struct Cyc_CfFlowInfo_AssignEnv _TF;
# 757
_TF.fenv=fenv;{struct Cyc_CfFlowInfo_EscPile*_T10=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_T10->places=0;_T2=(struct Cyc_CfFlowInfo_EscPile*)_T10;}_TF.pile=_T2;_TF.d=d;_TF.loc=loc;_TF.root_place=place;_T1=_TF;}{struct Cyc_CfFlowInfo_AssignEnv env=_T1;_T3=& env;_T4=(struct Cyc_CfFlowInfo_AssignEnv*)_T3;_T5=path;_T6=
Cyc_Dict_lookup(d,root);_T7=r;{void*newval=Cyc_CfFlowInfo_assign_place_outer(_T4,_T5,0,_T6,_T7);_T8=fenv;_T9=env;_TA=_T9.pile;_TB=
Cyc_Dict_insert(d,root,newval);_TC=Cyc_CfFlowInfo_escape_these(_T8,_TA,_TB);return _TC;}}}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;int is_try_flow;};
# 772
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*,void*,void*);
# 774
static int Cyc_CfFlowInfo_contains_local_consumed(void*r){void*_T0;int*_T1;unsigned _T2;void*_T3;int _T4;void*_T5;void*_T6;int _T7;void*_T8;void*_T9;int _TA;void*_TB;int _TC;_T0=r;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 7: _T3=r;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TD=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T3;_TC=_TD->f4;}{int l=_TC;_T4=l;
# 776
return _T4;}case 5: _T5=r;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TD=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T5;_T6=_TD->f1;_TB=(void*)_T6;}{void*r1=_TB;_T7=
Cyc_CfFlowInfo_contains_local_consumed(r1);return _T7;}case 8: _T8=r;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TD=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T8;_T9=_TD->f2;_TB=(void*)_T9;}{void*ac=_TB;_TA=
Cyc_CfFlowInfo_contains_local_consumed(ac);return _TA;}case 6:
 return 0;default:
 return 0;};}
# 784
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*);struct _tuple17{int f0;int f1;};
# 790
static void*Cyc_CfFlowInfo_join_absRval_tryflow(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){void*_T0;struct _tuple17 _T1;int _T2;int _T3;int _T4;void*_T5;int _T6;void*_T7;void*_T8;
if(r1!=r2)goto _TL14E;_T0=r1;return _T0;_TL14E:{struct _tuple17 _T9;
_T9.f0=Cyc_CfFlowInfo_contains_local_consumed(r1);_T9.f1=Cyc_CfFlowInfo_contains_local_consumed(r2);_T1=_T9;}{struct _tuple17 _T9=_T1;_T2=_T9.f0;_T3=(int)_T2;switch(_T3){case 1: _T4=_T9.f1;if(_T4!=0)goto _TL151;_T5=r2;
return _T5;_TL151: goto _LL5;case 0: _T6=_T9.f1;if(_T6!=1)goto _TL153;_T7=r1;
return _T7;_TL153: goto _LL5;default: _LL5: _T8=
Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);return _T8;};}}struct _tuple18{enum Cyc_CfFlowInfo_InitLevel f0;enum Cyc_CfFlowInfo_InitLevel f1;};
# 805
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){void*_T0;struct _tuple11 _T1;void*_T2;int*_T3;int _T4;void*_T5;int*_T6;int _T7;void*_T8;void*_T9;void*_TA;void*_TB;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;void*_T12;int*_T13;int _T14;void*_T15;void*_T16;void*_T17;void*_T18;int*_T19;int _T1A;void*_T1B;int*_T1C;int _T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T22;int _T23;int _T24;void*_T25;void*_T26;int*_T27;int _T28;void*_T29;struct Cyc_CfFlowInfo_JoinEnv*_T2A;struct Cyc_CfFlowInfo_EscPile*_T2B;struct Cyc_CfFlowInfo_Place*_T2C;void*_T2D;int*_T2E;int _T2F;void*_T30;struct Cyc_CfFlowInfo_JoinEnv*_T31;struct Cyc_CfFlowInfo_EscPile*_T32;struct Cyc_CfFlowInfo_Place*_T33;void*_T34;void*_T35;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T36;void*_T37;void*_T38;int*_T39;int _T3A;void*_T3B;void*_T3C;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T3D;void*_T3E;void*_T3F;int*_T40;unsigned _T41;void*_T42;int*_T43;unsigned _T44;void*_T45;void*_T46;int _T47;void*_T48;struct Cyc_CfFlowInfo_JoinEnv*_T49;struct Cyc_CfFlowInfo_EscPile*_T4A;struct Cyc_CfFlowInfo_Place*_T4B;struct Cyc_CfFlowInfo_JoinEnv*_T4C;struct Cyc_CfFlowInfo_EscPile*_T4D;struct Cyc_CfFlowInfo_Place*_T4E;void*_T4F;struct Cyc_CfFlowInfo_JoinEnv*_T50;struct Cyc_CfFlowInfo_EscPile*_T51;struct Cyc_CfFlowInfo_Place*_T52;struct Cyc_CfFlowInfo_JoinEnv*_T53;struct Cyc_CfFlowInfo_FlowEnv*_T54;struct Cyc_CfFlowInfo_JoinEnv*_T55;struct Cyc_Dict_Dict _T56;void*_T57;struct Cyc_CfFlowInfo_JoinEnv*_T58;struct Cyc_CfFlowInfo_FlowEnv*_T59;void*_T5A;struct Cyc_CfFlowInfo_JoinEnv*_T5B;struct Cyc_CfFlowInfo_FlowEnv*_T5C;void*_T5D;void*_T5E;struct Cyc_CfFlowInfo_JoinEnv*_T5F;struct Cyc_CfFlowInfo_EscPile*_T60;struct Cyc_CfFlowInfo_Place*_T61;void*_T62;int*_T63;unsigned _T64;void*_T65;struct Cyc_CfFlowInfo_JoinEnv*_T66;struct Cyc_CfFlowInfo_EscPile*_T67;struct Cyc_CfFlowInfo_Place*_T68;struct Cyc_CfFlowInfo_JoinEnv*_T69;struct Cyc_CfFlowInfo_FlowEnv*_T6A;struct Cyc_CfFlowInfo_JoinEnv*_T6B;struct Cyc_Dict_Dict _T6C;void*_T6D;struct Cyc_CfFlowInfo_JoinEnv*_T6E;struct Cyc_CfFlowInfo_FlowEnv*_T6F;void*_T70;struct Cyc_CfFlowInfo_JoinEnv*_T71;struct Cyc_CfFlowInfo_FlowEnv*_T72;void*_T73;void*_T74;void*_T75;struct Cyc_CfFlowInfo_JoinEnv*_T76;struct Cyc_CfFlowInfo_FlowEnv*_T77;struct Cyc_CfFlowInfo_JoinEnv*_T78;struct Cyc_Dict_Dict _T79;void*_T7A;struct Cyc_CfFlowInfo_JoinEnv*_T7B;struct Cyc_CfFlowInfo_FlowEnv*_T7C;void*_T7D;struct Cyc_CfFlowInfo_JoinEnv*_T7E;struct Cyc_CfFlowInfo_FlowEnv*_T7F;void*_T80;void*_T81;int*_T82;unsigned _T83;void*_T84;void*_T85;void*_T86;void*_T87;struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T88;void*_T89;void*_T8A;void*_T8B;struct Cyc_CfFlowInfo_JoinEnv*_T8C;struct Cyc_CfFlowInfo_FlowEnv*_T8D;struct Cyc_CfFlowInfo_JoinEnv*_T8E;struct Cyc_Dict_Dict _T8F;void*_T90;struct Cyc_CfFlowInfo_JoinEnv*_T91;struct Cyc_CfFlowInfo_FlowEnv*_T92;void*_T93;struct Cyc_CfFlowInfo_JoinEnv*_T94;struct Cyc_CfFlowInfo_FlowEnv*_T95;void*_T96;void*_T97;int*_T98;int _T99;void*_T9A;struct Cyc_CfFlowInfo_JoinEnv*_T9B;struct Cyc_CfFlowInfo_EscPile*_T9C;struct Cyc_CfFlowInfo_Place*_T9D;void*_T9E;int*_T9F;int _TA0;void*_TA1;int*_TA2;int _TA3;void*_TA4;struct Cyc_CfFlowInfo_UnionRInfo _TA5;struct Cyc_CfFlowInfo_UnionRInfo _TA6;void*_TA7;struct Cyc_CfFlowInfo_UnionRInfo _TA8;struct Cyc_CfFlowInfo_UnionRInfo _TA9;struct _fat_ptr _TAA;struct _fat_ptr _TAB;void**_TAC;unsigned _TAD;struct _fat_ptr _TAE;struct Cyc_CfFlowInfo_JoinEnv*_TAF;int _TB0;void**_TB1;unsigned _TB2;void*(*_TB3)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*);struct Cyc_CfFlowInfo_JoinEnv*_TB4;struct _fat_ptr _TB5;unsigned _TB6;int _TB7;char*_TB8;void**_TB9;void*_TBA;struct _fat_ptr _TBB;unsigned _TBC;int _TBD;char*_TBE;void**_TBF;void*_TC0;void**_TC1;unsigned _TC2;struct Cyc_CfFlowInfo_JoinEnv*_TC3;void*_TC4;struct _fat_ptr _TC5;unsigned _TC6;int _TC7;char*_TC8;void**_TC9;void*_TCA;struct _fat_ptr _TCB;unsigned _TCC;int _TCD;char*_TCE;void**_TCF;void*_TD0;int _TD1;unsigned _TD2;struct _fat_ptr _TD3;unsigned _TD4;struct _fat_ptr _TD5;int _TD6;char*_TD7;void**_TD8;void*_TD9;struct _fat_ptr _TDA;unsigned char*_TDB;void**_TDC;int _TDD;void*_TDE;int _TDF;int _TE0;void*_TE1;int _TE2;unsigned _TE3;struct _fat_ptr _TE4;unsigned _TE5;struct _fat_ptr _TE6;int _TE7;char*_TE8;void**_TE9;void*_TEA;struct _fat_ptr _TEB;int _TEC;char*_TED;void**_TEE;void*_TEF;int _TF0;int _TF1;void*_TF2;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_TF3;void*_TF4;struct Cyc_CfFlowInfo_JoinEnv*_TF5;struct Cyc_CfFlowInfo_FlowEnv*_TF6;struct Cyc_CfFlowInfo_JoinEnv*_TF7;struct Cyc_Dict_Dict _TF8;void*_TF9;struct Cyc_CfFlowInfo_JoinEnv*_TFA;struct Cyc_CfFlowInfo_FlowEnv*_TFB;struct Cyc_CfFlowInfo_JoinEnv*_TFC;struct Cyc_Dict_Dict _TFD;void*_TFE;struct _tuple11 _TFF;void*_T100;int*_T101;int _T102;void*_T103;int*_T104;int _T105;struct _tuple18 _T106;enum Cyc_CfFlowInfo_InitLevel _T107;enum Cyc_CfFlowInfo_InitLevel _T108;struct Cyc_CfFlowInfo_JoinEnv*_T109;struct Cyc_CfFlowInfo_FlowEnv*_T10A;void*_T10B;struct Cyc_CfFlowInfo_JoinEnv*_T10C;struct Cyc_CfFlowInfo_FlowEnv*_T10D;void*_T10E;struct _tuple18 _T10F;enum Cyc_CfFlowInfo_InitLevel _T110;enum Cyc_CfFlowInfo_InitLevel _T111;struct Cyc_CfFlowInfo_JoinEnv*_T112;struct Cyc_CfFlowInfo_FlowEnv*_T113;void*_T114;struct Cyc_CfFlowInfo_JoinEnv*_T115;struct Cyc_CfFlowInfo_FlowEnv*_T116;void*_T117;
if(r1!=r2)goto _TL155;_T0=r1;return _T0;_TL155:{struct _tuple11 _T118;
# 808
_T118.f0=r1;_T118.f1=r2;_T1=_T118;}{struct _tuple11 _T118=_T1;struct _fat_ptr _T119;struct _fat_ptr _T11A;struct Cyc_CfFlowInfo_Place*_T11B;int _T11C;int _T11D;struct Cyc_Absyn_Exp*_T11E;int _T11F;int _T120;void*_T121;void*_T122;void*_T123;void*_T124;_T2=_T118.f0;_T3=(int*)_T2;_T4=*_T3;if(_T4!=8)goto _TL157;_T5=_T118.f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=8)goto _TL159;_T8=_T118.f0;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T8;_T124=_T125->f1;_T9=_T125->f2;_T123=(void*)_T9;}_TA=_T118.f1;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TA;_T122=_T125->f1;_TB=_T125->f2;_T121=(void*)_TB;}{struct Cyc_Absyn_Vardecl*n1=_T124;void*r1=_T123;struct Cyc_Absyn_Vardecl*n2=_T122;void*r2=_T121;
# 811
if(n1!=n2)goto _TL15B;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T125=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_T125->tag=8;
_T125->f1=n1;_T125->f2=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T125;}_TD=(void*)_TC;return _TD;_TL15B: _TE=
Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);return _TE;}_TL159: _TF=_T118.f0;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_TF;_T124=_T125->f1;_T10=_T125->f2;_T123=(void*)_T10;}{struct Cyc_Absyn_Vardecl*n1=_T124;void*r1=_T123;_T11=
Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);return _T11;}_TL157: _T12=_T118.f1;_T13=(int*)_T12;_T14=*_T13;if(_T14!=8)goto _TL15D;_T15=_T118.f1;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T15;_T124=_T125->f1;_T16=_T125->f2;_T123=(void*)_T16;}{struct Cyc_Absyn_Vardecl*n2=_T124;void*r2=_T123;_T17=
Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);return _T17;}_TL15D: _T18=_T118.f0;_T19=(int*)_T18;_T1A=*_T19;if(_T1A!=7)goto _TL15F;_T1B=_T118.f1;_T1C=(int*)_T1B;_T1D=*_T1C;if(_T1D!=7)goto _TL161;_T1E=_T118.f0;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T1E;_T124=_T125->f1;_T120=_T125->f2;_T1F=_T125->f3;_T123=(void*)_T1F;_T11F=_T125->f4;}_T20=_T118.f1;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T20;_T11E=_T125->f1;_T11D=_T125->f2;_T21=_T125->f3;_T122=(void*)_T21;_T11C=_T125->f4;}{struct Cyc_Absyn_Exp*e1=_T124;int i1=_T120;void*r1=_T123;int l1=_T11F;struct Cyc_Absyn_Exp*e2=_T11E;int i2=_T11D;void*r2=_T122;int l2=_T11C;
# 818
if(e1!=e2)goto _TL163;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T125=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_T125->tag=7;
_T125->f1=e1;if(i1 <= i2)goto _TL165;_T125->f2=i1;goto _TL166;_TL165: _T125->f2=i2;_TL166: _T125->f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_T24=l1;if(!_T24)goto _TL167;_T23=l2;goto _TL168;_TL167: _T23=0;_TL168: _T125->f4=_T23;_T22=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T125;}_T25=(void*)_T22;return _T25;_TL163:{struct Cyc_CfFlowInfo_Place*_T125;_T26=r1;_T27=(int*)_T26;_T28=*_T27;if(_T28!=4)goto _TL169;_T29=r1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T126=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T29;_T125=_T126->f1;}{struct Cyc_CfFlowInfo_Place*p=_T125;_T2A=env;_T2B=_T2A->pile;_T2C=p;
# 821
Cyc_CfFlowInfo_add_place(_T2B,_T2C);goto _LL21;}_TL169: goto _LL21;_LL21:;}{struct Cyc_CfFlowInfo_Place*_T125;_T2D=r2;_T2E=(int*)_T2D;_T2F=*_T2E;if(_T2F!=4)goto _TL16B;_T30=r2;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T126=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T30;_T125=_T126->f1;}{struct Cyc_CfFlowInfo_Place*p=_T125;_T31=env;_T32=_T31->pile;_T33=p;
# 825
Cyc_CfFlowInfo_add_place(_T32,_T33);goto _LL26;}_TL16B: goto _LL26;_LL26:;}goto _LL0;}_TL161: _T34=_T118.f0;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T34;_T124=_T125->f1;_T120=_T125->f2;_T35=_T125->f3;_T123=(void*)_T35;_T11F=_T125->f4;}{struct Cyc_Absyn_Exp*e1=_T124;int i1=_T120;void*r1=_T123;int l1=_T11F;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T125=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_T125->tag=7;
# 830
_T125->f1=e1;_T125->f2=i1;_T125->f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_T125->f4=l1;_T36=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T125;}_T37=(void*)_T36;return _T37;}_TL15F: _T38=_T118.f1;_T39=(int*)_T38;_T3A=*_T39;if(_T3A!=7)goto _TL16D;_T3B=_T118.f1;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T3B;_T124=_T125->f1;_T120=_T125->f2;_T3C=_T125->f3;_T123=(void*)_T3C;_T11F=_T125->f4;}{struct Cyc_Absyn_Exp*e2=_T124;int i2=_T120;void*r2=_T123;int l2=_T11F;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T125=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_T125->tag=7;
# 832
_T125->f1=e2;_T125->f2=i2;_T125->f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_T125->f4=l2;_T3D=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T125;}_T3E=(void*)_T3D;return _T3E;}_TL16D: _T3F=_T118.f0;_T40=(int*)_T3F;_T41=*_T40;switch(_T41){case 4: _T42=_T118.f1;_T43=(int*)_T42;_T44=*_T43;switch(_T44){case 4: _T45=_T118.f0;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T45;_T124=_T125->f1;}_T46=_T118.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T46;_T123=_T125->f1;}{struct Cyc_CfFlowInfo_Place*p1=_T124;struct Cyc_CfFlowInfo_Place*p2=_T123;_T47=
# 835
Cyc_CfFlowInfo_place_cmp(p1,p2);if(_T47!=0)goto _TL171;_T48=r1;return _T48;_TL171: _T49=env;_T4A=_T49->pile;_T4B=p1;
Cyc_CfFlowInfo_add_place(_T4A,_T4B);_T4C=env;_T4D=_T4C->pile;_T4E=p2;
Cyc_CfFlowInfo_add_place(_T4D,_T4E);goto _LL0;}case 1: _T4F=_T118.f0;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T4F;_T124=_T125->f1;}{struct Cyc_CfFlowInfo_Place*p=_T124;_T50=env;_T51=_T50->pile;_T52=p;
# 841
Cyc_CfFlowInfo_add_place(_T51,_T52);_T53=env;_T54=_T53->fenv;_T55=env;_T56=_T55->d1;_T57=r1;{
enum Cyc_CfFlowInfo_InitLevel _T125=Cyc_CfFlowInfo_initlevel(_T54,_T56,_T57);if(_T125!=Cyc_CfFlowInfo_AllIL)goto _TL173;_T58=env;_T59=_T58->fenv;_T5A=_T59->notzeroall;
return _T5A;_TL173: _T5B=env;_T5C=_T5B->fenv;_T5D=_T5C->unknown_none;
return _T5D;;}}default: _T5E=_T118.f0;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T5E;_T124=_T125->f1;}{struct Cyc_CfFlowInfo_Place*p=_T124;_T5F=env;_T60=_T5F->pile;_T61=p;
# 867
Cyc_CfFlowInfo_add_place(_T60,_T61);goto _LL0;}};case 1: _T62=_T118.f1;_T63=(int*)_T62;_T64=*_T63;switch(_T64){case 4: _T65=_T118.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T65;_T124=_T125->f1;}{struct Cyc_CfFlowInfo_Place*p=_T124;_T66=env;_T67=_T66->pile;_T68=p;
# 847
Cyc_CfFlowInfo_add_place(_T67,_T68);_T69=env;_T6A=_T69->fenv;_T6B=env;_T6C=_T6B->d2;_T6D=r2;{
enum Cyc_CfFlowInfo_InitLevel _T125=Cyc_CfFlowInfo_initlevel(_T6A,_T6C,_T6D);if(_T125!=Cyc_CfFlowInfo_AllIL)goto _TL176;_T6E=env;_T6F=_T6E->fenv;_T70=_T6F->notzeroall;
return _T70;_TL176: _T71=env;_T72=_T71->fenv;_T73=_T72->unknown_none;
return _T73;;}}case 5: _T74=_T118.f1;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T74;_T75=_T125->f1;_T124=(void*)_T75;}{void*r2=_T124;_T76=env;_T77=_T76->fenv;_T78=env;_T79=_T78->d2;_T7A=r2;{
# 862
enum Cyc_CfFlowInfo_InitLevel _T125=Cyc_CfFlowInfo_initlevel(_T77,_T79,_T7A);if(_T125!=Cyc_CfFlowInfo_AllIL)goto _TL178;_T7B=env;_T7C=_T7B->fenv;_T7D=_T7C->notzeroall;
return _T7D;_TL178: _T7E=env;_T7F=_T7E->fenv;_T80=_T7F->unknown_none;
return _T80;;}}default: goto _LL1F;};case 5: _T81=_T118.f1;_T82=(int*)_T81;_T83=*_T82;switch(_T83){case 5: _T84=_T118.f0;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T84;_T85=_T125->f1;_T124=(void*)_T85;}_T86=_T118.f1;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T86;_T87=_T125->f1;_T123=(void*)_T87;}{void*r1=_T124;void*r2=_T123;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T125=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_T125->tag=5;
# 854
_T125->f1=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_T88=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T125;}_T89=(void*)_T88;return _T89;}case 1: _T8A=_T118.f0;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T8A;_T8B=_T125->f1;_T124=(void*)_T8B;}{void*r1=_T124;_T8C=env;_T8D=_T8C->fenv;_T8E=env;_T8F=_T8E->d1;_T90=r1;{
# 857
enum Cyc_CfFlowInfo_InitLevel _T125=Cyc_CfFlowInfo_initlevel(_T8D,_T8F,_T90);if(_T125!=Cyc_CfFlowInfo_AllIL)goto _TL17B;_T91=env;_T92=_T91->fenv;_T93=_T92->notzeroall;
return _T93;_TL17B: _T94=env;_T95=_T94->fenv;_T96=_T95->unknown_none;
return _T96;;}}case 4: goto _LL1B;default: goto _LL1F;};default: _T97=_T118.f1;_T98=(int*)_T97;_T99=*_T98;if(_T99!=4)goto _TL17D;_LL1B: _T9A=_T118.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T9A;_T11B=_T125->f1;}{struct Cyc_CfFlowInfo_Place*p=_T11B;_T9B=env;_T9C=_T9B->pile;_T9D=p;
# 868
Cyc_CfFlowInfo_add_place(_T9C,_T9D);goto _LL0;}_TL17D: _T9E=_T118.f0;_T9F=(int*)_T9E;_TA0=*_T9F;if(_TA0!=6)goto _TL17F;_TA1=_T118.f1;_TA2=(int*)_TA1;_TA3=*_TA2;if(_TA3!=6)goto _TL181;_TA4=_T118.f0;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TA4;_TA5=_T125->f1;_T120=_TA5.is_union;_TA6=_T125->f1;_T11F=_TA6.fieldnum;_T11A=_T125->f2;}_TA7=_T118.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T125=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_TA7;_TA8=_T125->f1;_T11D=_TA8.is_union;_TA9=_T125->f1;_T11C=_TA9.fieldnum;_T119=_T125->f2;}{int is_union1=_T120;int field_no1=_T11F;struct _fat_ptr d1=_T11A;int is_union2=_T11D;int field_no2=_T11C;struct _fat_ptr d2=_T119;_TAB=d1;{unsigned _T125=
# 872
_get_fat_size(_TAB,sizeof(void*));_TAD=_check_times(_T125,sizeof(void*));{void**_T126=_cycalloc(_TAD);_TAE=d1;{unsigned _T127=_get_fat_size(_TAE,sizeof(void*));unsigned i;i=0;_TL186: if(i < _T127)goto _TL184;else{goto _TL185;}_TL184: _TAF=env;_TB0=_TAF->is_try_flow;if(!_TB0)goto _TL187;_TB2=i;_TB1=_T126 + _TB2;{void*(*_T128)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*)=(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval_tryflow;_TB3=_T128;}_TB4=env;_TB5=d1;_TB6=i;_TB7=(int)_TB6;_TB8=_check_fat_subscript(_TB5,sizeof(void*),_TB7);_TB9=(void**)_TB8;_TBA=*_TB9;_TBB=d2;_TBC=i;_TBD=(int)_TBC;_TBE=_check_fat_subscript(_TBB,sizeof(void*),_TBD);_TBF=(void**)_TBE;_TC0=*_TBF;*_TB1=_TB3(_TB4,0,_TBA,_TC0);goto _TL188;_TL187: _TC2=i;_TC1=_T126 + _TC2;_TC3=env;_TC4=a;_TC5=d1;_TC6=i;_TC7=(int)_TC6;_TC8=_check_fat_subscript(_TC5,sizeof(void*),_TC7);_TC9=(void**)_TC8;_TCA=*_TC9;_TCB=d2;_TCC=i;_TCD=(int)_TCC;_TCE=_check_fat_subscript(_TCB,sizeof(void*),_TCD);_TCF=(void**)_TCE;_TD0=*_TCF;
*_TC1=Cyc_CfFlowInfo_join_absRval(_TC3,_TC4,_TCA,_TD0);_TL188: i=i + 1;goto _TL186;_TL185:;}_TAC=(void**)_T126;}_TAA=
# 872
_tag_fat(_TAC,sizeof(void*),_T125);}{
# 871
struct _fat_ptr new_d=_TAA;
# 877
int change=0;{
int i=0;_TL18C: _TD1=i;_TD2=(unsigned)_TD1;_TD3=d1;_TD4=_get_fat_size(_TD3,sizeof(void*));if(_TD2 < _TD4)goto _TL18A;else{goto _TL18B;}
_TL18A: _TD5=new_d;_TD6=i;_TD7=_check_fat_subscript(_TD5,sizeof(void*),_TD6);_TD8=(void**)_TD7;_TD9=*_TD8;_TDA=d1;_TDB=_TDA.curr;_TDC=(void**)_TDB;_TDD=i;_TDE=_TDC[_TDD];if(_TD9==_TDE)goto _TL18D;
change=1;goto _TL18B;_TL18D:
# 878
 i=i + 1;goto _TL18C;_TL18B:;}_TDF=change;
# 882
if(_TDF)goto _TL18F;else{goto _TL191;}
_TL191: _TE0=is_union1;if(_TE0)goto _TL192;else{goto _TL194;}_TL194: _TE1=r1;return _TE1;_TL192:
 new_d=d1;goto _TL190;
# 887
_TL18F: change=0;{
int i=0;_TL198: _TE2=i;_TE3=(unsigned)_TE2;_TE4=d1;_TE5=_get_fat_size(_TE4,sizeof(void*));if(_TE3 < _TE5)goto _TL196;else{goto _TL197;}
_TL196: _TE6=new_d;_TE7=i;_TE8=_check_fat_subscript(_TE6,sizeof(void*),_TE7);_TE9=(void**)_TE8;_TEA=*_TE9;_TEB=d2;_TEC=i;_TED=_check_fat_subscript(_TEB,sizeof(void*),_TEC);_TEE=(void**)_TED;_TEF=*_TEE;if(_TEA==_TEF)goto _TL199;
change=1;goto _TL197;_TL199:
# 888
 i=i + 1;goto _TL198;_TL197:;}_TF0=change;
# 892
if(_TF0)goto _TL19B;else{goto _TL19D;}
_TL19D: _TF1=is_union1;if(_TF1)goto _TL19E;else{goto _TL1A0;}_TL1A0: _TF2=r2;return _TF2;_TL19E:
 new_d=d2;goto _TL19C;_TL19B: _TL19C: _TL190:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T125=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_T125->tag=6;
# 897
_T125->f1.is_union=is_union1;if(field_no1!=field_no2)goto _TL1A1;_T125->f1.fieldnum=field_no1;goto _TL1A2;_TL1A1: _T125->f1.fieldnum=-1;_TL1A2: _T125->f2=new_d;_TF3=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T125;}_TF4=(void*)_TF3;return _TF4;}}_TL181: goto _LL1F;_TL17F: _LL1F: goto _LL0;}_LL0:;}_TF5=env;_TF6=_TF5->fenv;_TF7=env;_TF8=_TF7->d1;_TF9=r1;{
# 901
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(_TF6,_TF8,_TF9);_TFA=env;_TFB=_TFA->fenv;_TFC=env;_TFD=_TFC->d2;_TFE=r2;{
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(_TFB,_TFD,_TFE);{struct _tuple11 _T118;
_T118.f0=r1;_T118.f1=r2;_TFF=_T118;}{struct _tuple11 _T118=_TFF;_T100=_T118.f0;_T101=(int*)_T100;_T102=*_T101;if(_T102!=3)goto _TL1A3;goto _LL43;_TL1A3: _T103=_T118.f1;_T104=(int*)_T103;_T105=*_T104;if(_T105!=3)goto _TL1A5;_LL43:{struct _tuple18 _T119;
# 906
_T119.f0=il1;_T119.f1=il2;_T106=_T119;}{struct _tuple18 _T119=_T106;_T107=_T119.f1;if(_T107!=Cyc_CfFlowInfo_NoneIL)goto _TL1A7;goto _LL4A;_TL1A7: _T108=_T119.f0;if(_T108!=Cyc_CfFlowInfo_NoneIL)goto _TL1A9;_LL4A: _T109=env;_T10A=_T109->fenv;_T10B=_T10A->esc_none;
# 908
return _T10B;_TL1A9: _T10C=env;_T10D=_T10C->fenv;_T10E=_T10D->esc_all;
return _T10E;;}goto _TL1A6;_TL1A5:{struct _tuple18 _T119;
# 912
_T119.f0=il1;_T119.f1=il2;_T10F=_T119;}{struct _tuple18 _T119=_T10F;_T110=_T119.f1;if(_T110!=Cyc_CfFlowInfo_NoneIL)goto _TL1AB;goto _LL51;_TL1AB: _T111=_T119.f0;if(_T111!=Cyc_CfFlowInfo_NoneIL)goto _TL1AD;_LL51: _T112=env;_T113=_T112->fenv;_T114=_T113->unknown_none;
# 914
return _T114;_TL1AD: _T115=env;_T116=_T115->fenv;_T117=_T116->unknown_all;
return _T117;;}_TL1A6:;}}}}struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f0;union Cyc_CfFlowInfo_FlowInfo f1;};
# 924
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_tryflow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo newflow,union Cyc_CfFlowInfo_FlowInfo oldflow){struct _tuple19 _T0;union Cyc_CfFlowInfo_FlowInfo _T1;struct _union_FlowInfo_BottomFL _T2;unsigned _T3;union Cyc_CfFlowInfo_FlowInfo _T4;union Cyc_CfFlowInfo_FlowInfo _T5;struct _union_FlowInfo_BottomFL _T6;unsigned _T7;union Cyc_CfFlowInfo_FlowInfo _T8;union Cyc_CfFlowInfo_FlowInfo _T9;struct _union_FlowInfo_ReachableFL _TA;union Cyc_CfFlowInfo_FlowInfo _TB;struct _union_FlowInfo_ReachableFL _TC;struct Cyc_Dict_Dict _TD;const struct Cyc_Dict_T*_TE;struct Cyc_Dict_Dict _TF;const struct Cyc_Dict_T*_T10;union Cyc_CfFlowInfo_FlowInfo _T11;int _T12;union Cyc_CfFlowInfo_FlowInfo _T13;int _T14;union Cyc_CfFlowInfo_FlowInfo _T15;struct Cyc_CfFlowInfo_JoinEnv _T16;struct Cyc_CfFlowInfo_EscPile*_T17;struct Cyc_Dict_Dict(*_T18)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict(*_T19)(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_CfFlowInfo_JoinEnv*_T1A;struct Cyc_CfFlowInfo_JoinEnv*_T1B;struct Cyc_Dict_Dict _T1C;struct Cyc_Dict_Dict _T1D;struct Cyc_CfFlowInfo_FlowEnv*_T1E;struct Cyc_CfFlowInfo_JoinEnv _T1F;struct Cyc_CfFlowInfo_EscPile*_T20;struct Cyc_Dict_Dict _T21;struct Cyc_Dict_Dict _T22;union Cyc_CfFlowInfo_FlowInfo _T23;{struct _tuple19 _T24;
# 926
_T24.f0=newflow;_T24.f1=oldflow;_T0=_T24;}{struct _tuple19 _T24=_T0;struct Cyc_Dict_Dict _T25;struct Cyc_Dict_Dict _T26;_T1=_T24.f0;_T2=_T1.BottomFL;_T3=_T2.tag;if(_T3!=1)goto _TL1AF;_T4=oldflow;
return _T4;_TL1AF: _T5=_T24.f1;_T6=_T5.BottomFL;_T7=_T6.tag;if(_T7!=1)goto _TL1B1;_T8=newflow;
return _T8;_TL1B1: _T9=_T24.f0;_TA=_T9.ReachableFL;_T26=_TA.val;_TB=_T24.f1;_TC=_TB.ReachableFL;_T25=_TC.val;{struct Cyc_Dict_Dict d1=_T26;struct Cyc_Dict_Dict d2=_T25;_TD=d1;_TE=_TD.t;_TF=d2;_T10=_TF.t;
# 932
if(_TE!=_T10)goto _TL1B3;_T11=newflow;return _T11;_TL1B3: _T12=
Cyc_CfFlowInfo_flow_lessthan_approx(newflow,oldflow);if(!_T12)goto _TL1B5;_T13=oldflow;return _T13;_TL1B5: _T14=
Cyc_CfFlowInfo_flow_lessthan_approx(oldflow,newflow);if(!_T14)goto _TL1B7;_T15=newflow;return _T15;_TL1B7:{struct Cyc_CfFlowInfo_JoinEnv _T27;
_T27.fenv=fenv;{struct Cyc_CfFlowInfo_EscPile*_T28=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_T28->places=0;_T17=(struct Cyc_CfFlowInfo_EscPile*)_T28;}_T27.pile=_T17;_T27.d1=d1;_T27.d2=d2;_T27.is_try_flow=1;_T16=_T27;}{struct Cyc_CfFlowInfo_JoinEnv env=_T16;_T19=Cyc_Dict_intersect_c;{
struct Cyc_Dict_Dict(*_T27)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))_T19;_T18=_T27;}_T1A=& env;_T1B=(struct Cyc_CfFlowInfo_JoinEnv*)_T1A;_T1C=d1;_T1D=d2;{struct Cyc_Dict_Dict outdict=_T18(Cyc_CfFlowInfo_join_absRval_tryflow,_T1B,_T1C,_T1D);_T1E=fenv;_T1F=env;_T20=_T1F.pile;_T21=outdict;_T22=
Cyc_CfFlowInfo_escape_these(_T1E,_T20,_T21);_T23=Cyc_CfFlowInfo_ReachableFL(_T22);return _T23;}}};}}
# 941
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple19 _T0;union Cyc_CfFlowInfo_FlowInfo _T1;struct _union_FlowInfo_BottomFL _T2;unsigned _T3;union Cyc_CfFlowInfo_FlowInfo _T4;union Cyc_CfFlowInfo_FlowInfo _T5;struct _union_FlowInfo_BottomFL _T6;unsigned _T7;union Cyc_CfFlowInfo_FlowInfo _T8;union Cyc_CfFlowInfo_FlowInfo _T9;struct _union_FlowInfo_ReachableFL _TA;union Cyc_CfFlowInfo_FlowInfo _TB;struct _union_FlowInfo_ReachableFL _TC;struct Cyc_Dict_Dict _TD;const struct Cyc_Dict_T*_TE;struct Cyc_Dict_Dict _TF;const struct Cyc_Dict_T*_T10;union Cyc_CfFlowInfo_FlowInfo _T11;int _T12;union Cyc_CfFlowInfo_FlowInfo _T13;int _T14;union Cyc_CfFlowInfo_FlowInfo _T15;struct Cyc_CfFlowInfo_JoinEnv _T16;struct Cyc_CfFlowInfo_EscPile*_T17;struct Cyc_Dict_Dict(*_T18)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_Dict_Dict(*_T19)(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);struct Cyc_CfFlowInfo_JoinEnv*_T1A;struct Cyc_CfFlowInfo_JoinEnv*_T1B;struct Cyc_Dict_Dict _T1C;struct Cyc_Dict_Dict _T1D;struct Cyc_CfFlowInfo_FlowEnv*_T1E;struct Cyc_CfFlowInfo_JoinEnv _T1F;struct Cyc_CfFlowInfo_EscPile*_T20;struct Cyc_Dict_Dict _T21;struct Cyc_Dict_Dict _T22;union Cyc_CfFlowInfo_FlowInfo _T23;{struct _tuple19 _T24;
# 943
_T24.f0=f1;_T24.f1=f2;_T0=_T24;}{struct _tuple19 _T24=_T0;struct Cyc_Dict_Dict _T25;struct Cyc_Dict_Dict _T26;_T1=_T24.f0;_T2=_T1.BottomFL;_T3=_T2.tag;if(_T3!=1)goto _TL1B9;_T4=f2;
return _T4;_TL1B9: _T5=_T24.f1;_T6=_T5.BottomFL;_T7=_T6.tag;if(_T7!=1)goto _TL1BB;_T8=f1;
return _T8;_TL1BB: _T9=_T24.f0;_TA=_T9.ReachableFL;_T26=_TA.val;_TB=_T24.f1;_TC=_TB.ReachableFL;_T25=_TC.val;{struct Cyc_Dict_Dict d1=_T26;struct Cyc_Dict_Dict d2=_T25;_TD=d1;_TE=_TD.t;_TF=d2;_T10=_TF.t;
# 949
if(_TE!=_T10)goto _TL1BD;_T11=f1;return _T11;_TL1BD: _T12=
Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2);if(!_T12)goto _TL1BF;_T13=f2;return _T13;_TL1BF: _T14=
Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1);if(!_T14)goto _TL1C1;_T15=f1;return _T15;_TL1C1:{struct Cyc_CfFlowInfo_JoinEnv _T27;
_T27.fenv=fenv;{struct Cyc_CfFlowInfo_EscPile*_T28=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_T28->places=0;_T17=(struct Cyc_CfFlowInfo_EscPile*)_T28;}_T27.pile=_T17;_T27.d1=d1;_T27.d2=d2;_T27.is_try_flow=0;_T16=_T27;}{struct Cyc_CfFlowInfo_JoinEnv env=_T16;_T19=Cyc_Dict_intersect_c;{
struct Cyc_Dict_Dict(*_T27)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))_T19;_T18=_T27;}_T1A=& env;_T1B=(struct Cyc_CfFlowInfo_JoinEnv*)_T1A;_T1C=d1;_T1D=d2;{struct Cyc_Dict_Dict outdict=_T18(Cyc_CfFlowInfo_join_absRval,_T1B,_T1C,_T1D);_T1E=fenv;_T1F=env;_T20=_T1F.pile;_T21=outdict;_T22=
Cyc_CfFlowInfo_escape_these(_T1E,_T20,_T21);_T23=Cyc_CfFlowInfo_ReachableFL(_T22);return _T23;}}};}}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f0;void*f1;};struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f0;union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 958
struct _tuple20 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple20 pr1,struct _tuple20 pr2){struct _tuple20 _T0;struct _tuple20 _T1;struct _tuple20 _T2;struct _tuple20 _T3;struct _tuple21 _T4;union Cyc_CfFlowInfo_FlowInfo _T5;struct _union_FlowInfo_BottomFL _T6;unsigned _T7;struct _tuple20 _T8;union Cyc_CfFlowInfo_FlowInfo _T9;struct _union_FlowInfo_BottomFL _TA;unsigned _TB;struct _tuple20 _TC;union Cyc_CfFlowInfo_FlowInfo _TD;struct _union_FlowInfo_ReachableFL _TE;unsigned _TF;union Cyc_CfFlowInfo_FlowInfo _T10;struct _union_FlowInfo_ReachableFL _T11;union Cyc_CfFlowInfo_FlowInfo _T12;struct _union_FlowInfo_ReachableFL _T13;union Cyc_CfFlowInfo_FlowInfo _T14;struct _union_FlowInfo_ReachableFL _T15;int(*_T16)(int,void*,void*);int(*_T17)(void*,void*,void*);int _T18;struct _tuple20 _T19;int(*_T1A)(int,void*,void*);int(*_T1B)(void*,void*,void*);int _T1C;struct _tuple20 _T1D;struct Cyc_CfFlowInfo_JoinEnv _T1E;struct Cyc_CfFlowInfo_EscPile*_T1F;void*(*_T20)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*);struct Cyc_CfFlowInfo_JoinEnv*_T21;struct Cyc_CfFlowInfo_JoinEnv*_T22;void*_T23;void*_T24;struct _tuple20 _T25;struct Cyc_CfFlowInfo_FlowEnv*_T26;struct Cyc_CfFlowInfo_JoinEnv _T27;struct Cyc_CfFlowInfo_EscPile*_T28;struct Cyc_Dict_Dict _T29;struct Cyc_Dict_Dict _T2A;int(*_T2B)(struct _fat_ptr,struct _fat_ptr);void*(*_T2C)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2D;struct _fat_ptr _T2E;void*_T2F;union Cyc_CfFlowInfo_FlowInfo _T30;_T0=pr1;_T30=_T0.f0;_T1=pr1;_T2F=_T1.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_T30;void*r1=_T2F;void*_T31;union Cyc_CfFlowInfo_FlowInfo _T32;_T2=pr2;_T32=_T2.f0;_T3=pr2;_T31=_T3.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_T32;void*r2=_T31;
# 965
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);{struct _tuple21 _T33;
_T33.f0=f1;_T33.f1=f2;_T33.f2=outflow;_T4=_T33;}{struct _tuple21 _T33=_T4;struct Cyc_Dict_Dict _T34;struct Cyc_Dict_Dict _T35;struct Cyc_Dict_Dict _T36;_T5=_T33.f0;_T6=_T5.BottomFL;_T7=_T6.tag;if(_T7!=1)goto _TL1C3;{struct _tuple20 _T37;
_T37.f0=outflow;_T37.f1=r2;_T8=_T37;}return _T8;_TL1C3: _T9=_T33.f1;_TA=_T9.BottomFL;_TB=_TA.tag;if(_TB!=1)goto _TL1C5;{struct _tuple20 _T37;
_T37.f0=outflow;_T37.f1=r1;_TC=_T37;}return _TC;_TL1C5: _TD=_T33.f2;_TE=_TD.ReachableFL;_TF=_TE.tag;if(_TF!=2)goto _TL1C7;_T10=_T33.f0;_T11=_T10.ReachableFL;_T36=_T11.val;_T12=_T33.f1;_T13=_T12.ReachableFL;_T35=_T13.val;_T14=_T33.f2;_T15=_T14.ReachableFL;_T34=_T15.val;{struct Cyc_Dict_Dict d1=_T36;struct Cyc_Dict_Dict d2=_T35;struct Cyc_Dict_Dict outd=_T34;_T17=Cyc_CfFlowInfo_absRval_lessthan_approx;{
# 971
int(*_T37)(int,void*,void*)=(int(*)(int,void*,void*))_T17;_T16=_T37;}_T18=_T16(0,r1,r2);if(!_T18)goto _TL1C9;{struct _tuple20 _T37;_T37.f0=outflow;_T37.f1=r2;_T19=_T37;}return _T19;_TL1C9: _T1B=Cyc_CfFlowInfo_absRval_lessthan_approx;{
int(*_T37)(int,void*,void*)=(int(*)(int,void*,void*))_T1B;_T1A=_T37;}_T1C=_T1A(0,r2,r1);if(!_T1C)goto _TL1CB;{struct _tuple20 _T37;_T37.f0=outflow;_T37.f1=r1;_T1D=_T37;}return _T1D;_TL1CB:{struct Cyc_CfFlowInfo_JoinEnv _T37;
_T37.fenv=fenv;{struct Cyc_CfFlowInfo_EscPile*_T38=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_T38->places=0;_T1F=(struct Cyc_CfFlowInfo_EscPile*)_T38;}_T37.pile=_T1F;_T37.d1=d1;_T37.d2=d2;_T37.is_try_flow=0;_T1E=_T37;}{struct Cyc_CfFlowInfo_JoinEnv env=_T1E;{
void*(*_T37)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*)=(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval;_T20=_T37;}_T21=& env;_T22=(struct Cyc_CfFlowInfo_JoinEnv*)_T21;_T23=r1;_T24=r2;{void*outr=_T20(_T22,0,_T23,_T24);{struct _tuple20 _T37;_T26=fenv;_T27=env;_T28=_T27.pile;_T29=outd;_T2A=
Cyc_CfFlowInfo_escape_these(_T26,_T28,_T29);_T37.f0=Cyc_CfFlowInfo_ReachableFL(_T2A);_T37.f1=outr;_T25=_T37;}return _T25;}}}_TL1C7: _T2C=Cyc_Warn_impos;{
int(*_T37)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2C;_T2B=_T37;}_T2D=_tag_fat("join_flow_and_rval: BottomFL outflow",sizeof(char),37U);_T2E=_tag_fat(0U,sizeof(void*),0);_T2B(_T2D,_T2E);;}}}}
# 981
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){struct _tuple11 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;void*_T9;void*_TA;int _TB;void*_TC;int*_TD;int _TE;void*_TF;int*_T10;int _T11;void*_T12;int*_T13;int _T14;void*_T15;void*_T16;int _T17;int _T18;void*_T19;int*_T1A;int _T1B;void*_T1C;int*_T1D;int _T1E;void*_T1F;int*_T20;int _T21;void*_T22;void*_T23;void*_T24;void*_T25;int _T26;void*_T27;int*_T28;int _T29;void*_T2A;int*_T2B;int _T2C;void*_T2D;int*_T2E;unsigned _T2F;void*_T30;struct Cyc_CfFlowInfo_UnionRInfo _T31;struct Cyc_CfFlowInfo_UnionRInfo _T32;void*_T33;struct Cyc_CfFlowInfo_UnionRInfo _T34;struct Cyc_CfFlowInfo_UnionRInfo _T35;int _T36;struct _fat_ptr _T37;unsigned char*_T38;void**_T39;struct _fat_ptr _T3A;unsigned char*_T3B;void**_T3C;int _T3D;unsigned _T3E;struct _fat_ptr _T3F;unsigned _T40;int(*_T41)(int,void*,void*);struct _fat_ptr _T42;unsigned char*_T43;void**_T44;int _T45;void*_T46;struct _fat_ptr _T47;int _T48;char*_T49;void**_T4A;void*_T4B;int _T4C;void*_T4D;int*_T4E;unsigned _T4F;void*_T50;int*_T51;int _T52;void*_T53;int*_T54;int _T55;struct _tuple18 _T56;enum Cyc_CfFlowInfo_InitLevel _T57;enum Cyc_CfFlowInfo_InitLevel _T58;
if(r1!=r2)goto _TL1CD;return 1;_TL1CD:{struct _tuple11 _T59;
# 984
_T59.f0=r1;_T59.f1=r2;_T0=_T59;}{struct _tuple11 _T59=_T0;struct _fat_ptr _T5A;int _T5B;int _T5C;struct _fat_ptr _T5D;int _T5E;int _T5F;void*_T60;struct Cyc_Absyn_Vardecl*_T61;void*_T62;void*_T63;_T1=_T59.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=8)goto _TL1CF;_T4=_T59.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=8)goto _TL1D1;_T7=_T59.f0;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T64=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T7;_T63=_T64->f1;_T8=_T64->f2;_T62=(void*)_T8;}_T9=_T59.f1;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T64=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T9;_T61=_T64->f1;_TA=_T64->f2;_T60=(void*)_TA;}{struct Cyc_Absyn_Vardecl*n1=_T63;void*r1=_T62;struct Cyc_Absyn_Vardecl*n2=_T61;void*r2=_T60;
# 986
if(n1!=n2)goto _TL1D3;_TB=Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);goto _TL1D4;_TL1D3: _TB=0;_TL1D4: return _TB;}_TL1D1: goto _LL6;_TL1CF: _TC=_T59.f1;_TD=(int*)_TC;_TE=*_TD;if(_TE!=8)goto _TL1D5;_LL6:
# 988
 return 0;_TL1D5: _TF=_T59.f0;_T10=(int*)_TF;_T11=*_T10;if(_T11!=4)goto _TL1D7;_T12=_T59.f1;_T13=(int*)_T12;_T14=*_T13;if(_T14!=4)goto _TL1D9;_T15=_T59.f0;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T64=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T15;_T63=_T64->f1;}_T16=_T59.f1;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T64=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T16;_T62=_T64->f1;}{struct Cyc_CfFlowInfo_Place*p1=_T63;struct Cyc_CfFlowInfo_Place*p2=_T62;_T17=
Cyc_CfFlowInfo_place_cmp(p1,p2);_T18=_T17==0;return _T18;}_TL1D9: goto _LLC;_TL1D7: _T19=_T59.f1;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=4)goto _TL1DB;_LLC:
# 991
 return 0;_TL1DB: _T1C=_T59.f0;_T1D=(int*)_T1C;_T1E=*_T1D;if(_T1E!=5)goto _TL1DD;_T1F=_T59.f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=5)goto _TL1DF;_T22=_T59.f0;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T64=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T22;_T23=_T64->f1;_T63=(void*)_T23;}_T24=_T59.f1;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T64=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T24;_T25=_T64->f1;_T62=(void*)_T25;}{void*r1=_T63;void*r2=_T62;_T26=
Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);return _T26;}_TL1DF: goto _LL12;_TL1DD: _T27=_T59.f1;_T28=(int*)_T27;_T29=*_T28;if(_T29!=5)goto _TL1E1;_LL12:
# 994
 return 0;_TL1E1: _T2A=_T59.f0;_T2B=(int*)_T2A;_T2C=*_T2B;if(_T2C!=6)goto _TL1E3;_T2D=_T59.f1;_T2E=(int*)_T2D;_T2F=*_T2E;switch(_T2F){case 6: _T30=_T59.f0;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T64=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T30;_T31=_T64->f1;_T5F=_T31.is_union;_T32=_T64->f1;_T5E=_T32.fieldnum;_T5D=_T64->f2;}_T33=_T59.f1;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T64=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T33;_T34=_T64->f1;_T5C=_T34.is_union;_T35=_T64->f1;_T5B=_T35.fieldnum;_T5A=_T64->f2;}{int is_union1=_T5F;int fld1=_T5E;struct _fat_ptr d1=_T5D;int is_union2=_T5C;int fld2=_T5B;struct _fat_ptr d2=_T5A;_T36=is_union1;
# 997
if(!_T36)goto _TL1E6;if(fld1==fld2)goto _TL1E6;return 0;_TL1E6: _T37=d1;_T38=_T37.curr;_T39=(void**)_T38;_T3A=d2;_T3B=_T3A.curr;_T3C=(void**)_T3B;
if(_T39!=_T3C)goto _TL1E8;return 1;_TL1E8:{
int i=0;_TL1ED: _T3D=i;_T3E=(unsigned)_T3D;_T3F=d1;_T40=_get_fat_size(_T3F,sizeof(void*));if(_T3E < _T40)goto _TL1EB;else{goto _TL1EC;}
_TL1EB:{int(*_T64)(int,void*,void*)=(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;_T41=_T64;}_T42=d1;_T43=_T42.curr;_T44=(void**)_T43;_T45=i;_T46=_T44[_T45];_T47=d2;_T48=i;_T49=_check_fat_subscript(_T47,sizeof(void*),_T48);_T4A=(void**)_T49;_T4B=*_T4A;_T4C=_T41(0,_T46,_T4B);if(_T4C)goto _TL1EE;else{goto _TL1F0;}_TL1F0: return 0;_TL1EE:
# 999
 i=i + 1;goto _TL1ED;_TL1EC:;}
# 1001
return 1;}case 0: goto _LL15;case 1: goto _LL17;default: goto _LL1D;}goto _TL1E4;_TL1E3: _T4D=_T59.f1;_T4E=(int*)_T4D;_T4F=*_T4E;switch(_T4F){case 0: _LL15: goto _LL18;case 1: _LL17: _LL18:
# 1003
 return 0;default: _T50=_T59.f0;_T51=(int*)_T50;_T52=*_T51;if(_T52!=3)goto _TL1F2;_T53=_T59.f1;_T54=(int*)_T53;_T55=*_T54;if(_T55!=3)goto _TL1F4;goto _LL0;_TL1F4:
# 1005
 return 0;_TL1F2: _LL1D: goto _LL0;}_TL1E4: _LL0:;}{struct _tuple18 _T59;
# 1014
_T59.f0=Cyc_CfFlowInfo_initlevel_approx(r1);_T59.f1=Cyc_CfFlowInfo_initlevel_approx(r2);_T56=_T59;}{struct _tuple18 _T59=_T56;_T57=_T59.f0;if(_T57!=Cyc_CfFlowInfo_AllIL)goto _TL1F6;goto _LL23;_TL1F6: _T58=_T59.f1;if(_T58!=Cyc_CfFlowInfo_NoneIL)goto _TL1F8;_LL23:
# 1016
 return 1;_TL1F8:
 return 0;;}}
# 1027
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){struct _tuple19 _T0;union Cyc_CfFlowInfo_FlowInfo _T1;struct _union_FlowInfo_BottomFL _T2;unsigned _T3;union Cyc_CfFlowInfo_FlowInfo _T4;struct _union_FlowInfo_BottomFL _T5;unsigned _T6;union Cyc_CfFlowInfo_FlowInfo _T7;struct _union_FlowInfo_ReachableFL _T8;union Cyc_CfFlowInfo_FlowInfo _T9;struct _union_FlowInfo_ReachableFL _TA;struct Cyc_Dict_Dict _TB;const struct Cyc_Dict_T*_TC;struct Cyc_Dict_Dict _TD;const struct Cyc_Dict_T*_TE;struct Cyc_Dict_Dict _TF;struct Cyc_Dict_Dict _T10;int _T11;{struct _tuple19 _T12;
# 1029
_T12.f0=f1;_T12.f1=f2;_T0=_T12;}{struct _tuple19 _T12=_T0;struct Cyc_Dict_Dict _T13;struct Cyc_Dict_Dict _T14;_T1=_T12.f0;_T2=_T1.BottomFL;_T3=_T2.tag;if(_T3!=1)goto _TL1FA;
return 1;_TL1FA: _T4=_T12.f1;_T5=_T4.BottomFL;_T6=_T5.tag;if(_T6!=1)goto _TL1FC;
return 0;_TL1FC: _T7=_T12.f0;_T8=_T7.ReachableFL;_T14=_T8.val;_T9=_T12.f1;_TA=_T9.ReachableFL;_T13=_TA.val;{struct Cyc_Dict_Dict d1=_T14;struct Cyc_Dict_Dict d2=_T13;_TB=d1;_TC=_TB.t;_TD=d2;_TE=_TD.t;
# 1033
if(_TC!=_TE)goto _TL1FE;return 1;_TL1FE: _TF=d1;_T10=d2;_T11=
Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,_TF,_T10);return _T11;};}}struct _tuple22{void*f0;struct Cyc_List_List*f1;};
# 1038
struct _tuple22 Cyc_CfFlowInfo_unname_rval(void*rv){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_List_List*_T5;struct _tuple22 _T6;
struct Cyc_List_List*names=0;
_TL200: if(1)goto _TL201;else{goto _TL202;}
_TL201:{void*_T7;struct Cyc_Absyn_Vardecl*_T8;_T0=rv;_T1=(int*)_T0;_T2=*_T1;if(_T2!=8)goto _TL203;_T3=rv;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T3;_T8=_T9->f1;_T4=_T9->f2;_T7=(void*)_T4;}{struct Cyc_Absyn_Vardecl*vd=_T8;void*rv2=_T7;{struct Cyc_List_List*_T9=_cycalloc(sizeof(struct Cyc_List_List));
_T9->hd=vd;_T9->tl=names;_T5=(struct Cyc_List_List*)_T9;}names=_T5;rv=rv2;goto _LL0;}_TL203:{struct _tuple22 _T9;
_T9.f0=rv;_T9.f1=names;_T6=_T9;}return _T6;_LL0:;}goto _TL200;_TL202:;}
# 1047
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){enum Cyc_CfFlowInfo_InitLevel _T0;struct Cyc___cycFILE*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;_T0=il;if(_T0!=Cyc_CfFlowInfo_NoneIL)goto _TL205;_T1=Cyc_stderr;_T2=
# 1049
_tag_fat("uninitialized",sizeof(char),14U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1,_T2,_T3);goto _LL0;_TL205: _T4=Cyc_stderr;_T5=
_tag_fat("all-initialized",sizeof(char),16U);_T6=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4,_T5,_T6);goto _LL0;_LL0:;}
# 1054
void Cyc_CfFlowInfo_print_root(void*root){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc_Absyn_Vardecl*_T5;struct _tuple0*_T6;void**_T7;struct Cyc___cycFILE*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;void*_TB;void*_TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_String_pa_PrintArg_struct _TE;void**_TF;void**_T10;struct Cyc___cycFILE*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc___cycFILE*_T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc_Absyn_Exp*_T17;void*_T18;_T0=root;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=root;{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_T19=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_T3;_T18=_T19->f1;}{struct Cyc_Absyn_Vardecl*vd=_T18;{struct Cyc_String_pa_PrintArg_struct _T19;_T19.tag=0;_T5=vd;_T6=_T5->name;
# 1057
_T19.f1=Cyc_Absynpp_qvar2string(_T6);_T4=_T19;}{struct Cyc_String_pa_PrintArg_struct _T19=_T4;void*_T1A[1];_T7=_T1A + 0;*_T7=& _T19;_T8=Cyc_stderr;_T9=_tag_fat("Root(%s)",sizeof(char),9U);_TA=_tag_fat(_T1A,sizeof(void*),1);Cyc_fprintf(_T8,_T9,_TA);}goto _LL0;}case 1: _TB=root;{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_T19=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_TB;_T17=_T19->f1;_TC=_T19->f2;_T18=(void*)_TC;}{struct Cyc_Absyn_Exp*e=_T17;void*t=_T18;{struct Cyc_String_pa_PrintArg_struct _T19;_T19.tag=0;
# 1060
_T19.f1=Cyc_Absynpp_exp2string(e);_TD=_T19;}{struct Cyc_String_pa_PrintArg_struct _T19=_TD;{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;_T1A.f1=Cyc_Absynpp_typ2string(t);_TE=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_TE;void*_T1B[2];_TF=_T1B + 0;*_TF=& _T19;_T10=_T1B + 1;*_T10=& _T1A;_T11=Cyc_stderr;_T12=
# 1059
_tag_fat("MallocPt(%s,%s)",sizeof(char),16U);_T13=_tag_fat(_T1B,sizeof(void*),2);Cyc_fprintf(_T11,_T12,_T13);}}goto _LL0;}default: _T14=Cyc_stderr;_T15=
# 1062
_tag_fat("InitParam(_,_)",sizeof(char),15U);_T16=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T14,_T15,_T16);goto _LL0;}_LL0:;}
# 1066
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){struct Cyc_List_List*_T0;int*_T1;int _T2;struct Cyc_Int_pa_PrintArg_struct _T3;int _T4;void**_T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;int*_TD;int _TE;struct Cyc_List_List*_TF;struct Cyc_Int_pa_PrintArg_struct _T10;int _T11;void**_T12;struct Cyc___cycFILE*_T13;struct _fat_ptr _T14;struct _fat_ptr _T15;struct Cyc___cycFILE*_T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct Cyc_List_List*_T19;struct Cyc___cycFILE*_T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;{
struct Cyc_List_List*x=p;_TL20B: if(x!=0)goto _TL209;else{goto _TL20A;}
_TL209: _T0=x;{void*_T1D=_T0->hd;int _T1E;_T1=(int*)_T1D;_T2=*_T1;if(_T2!=0)goto _TL20C;{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_T1F=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_T1D;_T1E=_T1F->f1;}{int i=_T1E;{struct Cyc_Int_pa_PrintArg_struct _T1F;_T1F.tag=1;_T4=i;
# 1070
_T1F.f1=(unsigned long)_T4;_T3=_T1F;}{struct Cyc_Int_pa_PrintArg_struct _T1F=_T3;void*_T20[1];_T5=_T20 + 0;*_T5=& _T1F;_T6=Cyc_stderr;_T7=_tag_fat(".%d",sizeof(char),4U);_T8=_tag_fat(_T20,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}goto _LL0;}_TL20C: _T9=x;_TA=_T9->tl;
# 1073
if(_TA==0)goto _TL20E;_TB=x;_TC=_TB->tl;{
void*_T1F=_TC->hd;int _T20;_TD=(int*)_T1F;_TE=*_TD;if(_TE!=0)goto _TL210;{struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*_T21=(struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_T1F;_T20=_T21->f1;}{int i=_T20;_TF=x;
# 1076
x=_TF->tl;{struct Cyc_Int_pa_PrintArg_struct _T21;_T21.tag=1;_T11=i;
_T21.f1=(unsigned long)_T11;_T10=_T21;}{struct Cyc_Int_pa_PrintArg_struct _T21=_T10;void*_T22[1];_T12=_T22 + 0;*_T12=& _T21;_T13=Cyc_stderr;_T14=_tag_fat("->%d",sizeof(char),5U);_T15=_tag_fat(_T22,sizeof(void*),1);Cyc_fprintf(_T13,_T14,_T15);}goto _TL208;}_TL210: goto _LL5;_LL5:;}goto _TL20F;_TL20E: _TL20F: _T16=Cyc_stderr;_T17=
# 1082
_tag_fat("*",sizeof(char),2U);_T18=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T16,_T17,_T18);_LL0:;}_TL208: _T19=
# 1067
_check_null(x);x=_T19->tl;goto _TL20B;_TL20A:;}_T1A=Cyc_stderr;_T1B=
# 1084
_tag_fat(" ",sizeof(char),2U);_T1C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1A,_T1B,_T1C);}
# 1087
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){struct Cyc_CfFlowInfo_Place*_T0;void*_T1;struct Cyc_CfFlowInfo_Place*_T2;struct Cyc_List_List*_T3;_T0=p;_T1=_T0->root;
Cyc_CfFlowInfo_print_root(_T1);_T2=p;_T3=_T2->path;
Cyc_CfFlowInfo_print_path(_T3);}
# 1092
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){int _T0;struct Cyc___cycFILE*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_List_List*_T6;struct Cyc___cycFILE*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;
int first=1;
_TL212: if(x!=0)goto _TL213;else{goto _TL214;}
_TL213: _T0=first;if(_T0)goto _TL215;else{goto _TL217;}_TL217: _T1=Cyc_stderr;_T2=_tag_fat(", ",sizeof(char),3U);_T3=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1,_T2,_T3);first=0;goto _TL216;_TL215: _TL216: _T4=x;_T5=_T4->hd;
pr(_T5);_T6=x;
x=_T6->tl;goto _TL212;_TL214: _T7=Cyc_stderr;_T8=
# 1099
_tag_fat("\n",sizeof(char),2U);_T9=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T7,_T8,_T9);}
# 1102
void Cyc_CfFlowInfo_print_absrval(void*rval){void*_T0;int*_T1;unsigned _T2;struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;void*_T9;struct Cyc___cycFILE*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc___cycFILE*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;void*_T10;struct Cyc___cycFILE*_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc___cycFILE*_T14;struct _fat_ptr _T15;struct _fat_ptr _T16;void*_T17;struct Cyc___cycFILE*_T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;struct Cyc___cycFILE*_T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;void*_T1E;void*_T1F;struct Cyc___cycFILE*_T20;struct _fat_ptr _T21;struct _fat_ptr _T22;struct Cyc___cycFILE*_T23;struct _fat_ptr _T24;struct _fat_ptr _T25;void*_T26;struct Cyc_CfFlowInfo_UnionRInfo _T27;struct Cyc_CfFlowInfo_UnionRInfo _T28;int _T29;struct Cyc___cycFILE*_T2A;struct _fat_ptr _T2B;struct _fat_ptr _T2C;struct Cyc_Int_pa_PrintArg_struct _T2D;int _T2E;void**_T2F;struct Cyc___cycFILE*_T30;struct _fat_ptr _T31;struct _fat_ptr _T32;struct Cyc___cycFILE*_T33;struct _fat_ptr _T34;struct _fat_ptr _T35;int _T36;unsigned _T37;struct _fat_ptr _T38;unsigned _T39;struct _fat_ptr _T3A;unsigned char*_T3B;void**_T3C;int _T3D;void*_T3E;int _T3F;unsigned _T40;struct _fat_ptr _T41;unsigned _T42;struct Cyc___cycFILE*_T43;struct _fat_ptr _T44;struct _fat_ptr _T45;struct Cyc___cycFILE*_T46;struct _fat_ptr _T47;struct _fat_ptr _T48;void*_T49;void*_T4A;struct Cyc_String_pa_PrintArg_struct _T4B;struct Cyc_Int_pa_PrintArg_struct _T4C;int _T4D;void**_T4E;void**_T4F;struct Cyc___cycFILE*_T50;struct _fat_ptr _T51;struct _fat_ptr _T52;struct Cyc_String_pa_PrintArg_struct _T53;int _T54;void**_T55;struct Cyc___cycFILE*_T56;struct _fat_ptr _T57;struct _fat_ptr _T58;void*_T59;void*_T5A;struct Cyc_String_pa_PrintArg_struct _T5B;struct Cyc_Absyn_Vardecl*_T5C;struct _tuple0*_T5D;void**_T5E;struct Cyc___cycFILE*_T5F;struct _fat_ptr _T60;struct _fat_ptr _T61;struct Cyc___cycFILE*_T62;struct _fat_ptr _T63;struct _fat_ptr _T64;struct Cyc_Absyn_Vardecl*_T65;void*_T66;struct _fat_ptr _T67;int _T68;int _T69;void*_T6A;enum Cyc_CfFlowInfo_InitLevel _T6B;_T0=rval;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=Cyc_stderr;_T4=
# 1104
_tag_fat("Zero",sizeof(char),5U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3,_T4,_T5);goto _LL0;case 1: _T6=Cyc_stderr;_T7=
_tag_fat("NotZeroAll",sizeof(char),11U);_T8=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6,_T7,_T8);goto _LL0;case 2: _T9=rval;{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_T6C=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_T9;_T6B=_T6C->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_T6B;_TA=Cyc_stderr;_TB=
_tag_fat("Unknown(",sizeof(char),9U);_TC=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TA,_TB,_TC);Cyc_CfFlowInfo_print_initlevel(il);_TD=Cyc_stderr;_TE=
_tag_fat(")",sizeof(char),2U);_TF=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TD,_TE,_TF);goto _LL0;}case 3: _T10=rval;{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_T6C=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_T10;_T6B=_T6C->f1;}{enum Cyc_CfFlowInfo_InitLevel il=_T6B;_T11=Cyc_stderr;_T12=
_tag_fat("Esc(",sizeof(char),5U);_T13=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T11,_T12,_T13);Cyc_CfFlowInfo_print_initlevel(il);_T14=Cyc_stderr;_T15=
_tag_fat(")",sizeof(char),2U);_T16=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T14,_T15,_T16);goto _LL0;}case 4: _T17=rval;{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_T6C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_T17;_T6A=_T6C->f1;}{struct Cyc_CfFlowInfo_Place*p=_T6A;_T18=Cyc_stderr;_T19=
_tag_fat("AddrOf(",sizeof(char),8U);_T1A=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T18,_T19,_T1A);Cyc_CfFlowInfo_print_place(p);_T1B=Cyc_stderr;_T1C=
_tag_fat(")",sizeof(char),2U);_T1D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T1B,_T1C,_T1D);goto _LL0;}case 5: _T1E=rval;{struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_T6C=(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_T1E;_T1F=_T6C->f1;_T6A=(void*)_T1F;}{void*r=_T6A;_T20=Cyc_stderr;_T21=
_tag_fat("UniquePtr(",sizeof(char),11U);_T22=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T20,_T21,_T22);Cyc_CfFlowInfo_print_absrval(r);_T23=Cyc_stderr;_T24=
_tag_fat(")",sizeof(char),2U);_T25=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T23,_T24,_T25);goto _LL0;}case 6: _T26=rval;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_T6C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_T26;_T27=_T6C->f1;_T69=_T27.is_union;_T28=_T6C->f1;_T68=_T28.fieldnum;_T67=_T6C->f2;}{int is_union=_T69;int f=_T68;struct _fat_ptr d=_T67;_T29=is_union;
# 1115
if(!_T29)goto _TL219;_T2A=Cyc_stderr;_T2B=
_tag_fat("AggrUnion{",sizeof(char),11U);_T2C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2A,_T2B,_T2C);
if(f==-1)goto _TL21B;{struct Cyc_Int_pa_PrintArg_struct _T6C;_T6C.tag=1;_T2E=f;
_T6C.f1=(unsigned long)_T2E;_T2D=_T6C;}{struct Cyc_Int_pa_PrintArg_struct _T6C=_T2D;void*_T6D[1];_T2F=_T6D + 0;*_T2F=& _T6C;_T30=Cyc_stderr;_T31=_tag_fat("tag == %d;",sizeof(char),11U);_T32=_tag_fat(_T6D,sizeof(void*),1);Cyc_fprintf(_T30,_T31,_T32);}goto _TL21C;_TL21B: _TL21C: goto _TL21A;
# 1120
_TL219: _T33=Cyc_stderr;_T34=_tag_fat("AggrStruct{",sizeof(char),12U);_T35=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T33,_T34,_T35);_TL21A:{
int i=0;_TL220: _T36=i;_T37=(unsigned)_T36;_T38=d;_T39=_get_fat_size(_T38,sizeof(void*));if(_T37 < _T39)goto _TL21E;else{goto _TL21F;}
_TL21E: _T3A=d;_T3B=_T3A.curr;_T3C=(void**)_T3B;_T3D=i;_T3E=_T3C[_T3D];Cyc_CfFlowInfo_print_absrval(_T3E);_T3F=i + 1;_T40=(unsigned)_T3F;_T41=d;_T42=
_get_fat_size(_T41,sizeof(void*));if(_T40 >= _T42)goto _TL221;_T43=Cyc_stderr;_T44=_tag_fat(",",sizeof(char),2U);_T45=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T43,_T44,_T45);goto _TL222;_TL221: _TL222:
# 1121
 i=i + 1;goto _TL220;_TL21F:;}_T46=Cyc_stderr;_T47=
# 1125
_tag_fat("}",sizeof(char),2U);_T48=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T46,_T47,_T48);goto _LL0;}case 7: _T49=rval;{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_T6C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_T49;_T6A=_T6C->f1;_T69=_T6C->f2;_T4A=_T6C->f3;_T66=(void*)_T4A;_T68=_T6C->f4;}{struct Cyc_Absyn_Exp*e=_T6A;int i=_T69;void*r=_T66;int l=_T68;{struct Cyc_String_pa_PrintArg_struct _T6C;_T6C.tag=0;
# 1128
_T6C.f1=Cyc_Absynpp_exp2string(e);_T4B=_T6C;}{struct Cyc_String_pa_PrintArg_struct _T6C=_T4B;{struct Cyc_Int_pa_PrintArg_struct _T6D;_T6D.tag=1;_T4D=i;_T6D.f1=(unsigned long)_T4D;_T4C=_T6D;}{struct Cyc_Int_pa_PrintArg_struct _T6D=_T4C;void*_T6E[2];_T4E=_T6E + 0;*_T4E=& _T6C;_T4F=_T6E + 1;*_T4F=& _T6D;_T50=Cyc_stderr;_T51=_tag_fat("[Consumed(%s,%d,",sizeof(char),17U);_T52=_tag_fat(_T6E,sizeof(void*),2);Cyc_fprintf(_T50,_T51,_T52);}}
Cyc_CfFlowInfo_print_absrval(r);{struct Cyc_String_pa_PrintArg_struct _T6C;_T6C.tag=0;_T54=l;if(!_T54)goto _TL223;_T6C.f1=_tag_fat("local",sizeof(char),6U);goto _TL224;_TL223: _T6C.f1=_tag_fat("",sizeof(char),1U);_TL224: _T53=_T6C;}{struct Cyc_String_pa_PrintArg_struct _T6C=_T53;void*_T6D[1];_T55=_T6D + 0;*_T55=& _T6C;_T56=Cyc_stderr;_T57=_tag_fat(",%s)]",sizeof(char),6U);_T58=_tag_fat(_T6D,sizeof(void*),1);Cyc_fprintf(_T56,_T57,_T58);}goto _LL0;}default: _T59=rval;{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_T6C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_T59;_T65=_T6C->f1;_T5A=_T6C->f2;_T6A=(void*)_T5A;}{struct Cyc_Absyn_Vardecl*n=_T65;void*r=_T6A;{struct Cyc_String_pa_PrintArg_struct _T6C;_T6C.tag=0;_T5C=n;_T5D=_T5C->name;
# 1132
_T6C.f1=Cyc_Absynpp_qvar2string(_T5D);_T5B=_T6C;}{struct Cyc_String_pa_PrintArg_struct _T6C=_T5B;void*_T6D[1];_T5E=_T6D + 0;*_T5E=& _T6C;_T5F=Cyc_stderr;_T60=_tag_fat("[NamedLocation(%s,",sizeof(char),19U);_T61=_tag_fat(_T6D,sizeof(void*),1);Cyc_fprintf(_T5F,_T60,_T61);}
Cyc_CfFlowInfo_print_absrval(r);_T62=Cyc_stderr;_T63=_tag_fat(")]",sizeof(char),3U);_T64=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T62,_T63,_T64);goto _LL0;}}_LL0:;}
# 1138
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){struct Cyc___cycFILE*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;_T0=Cyc_stderr;_T1=
_tag_fat("    ",sizeof(char),5U);_T2=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T0,_T1,_T2);
Cyc_CfFlowInfo_print_root(root);_T3=Cyc_stderr;_T4=
_tag_fat(" --> ",sizeof(char),6U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3,_T4,_T5);
Cyc_CfFlowInfo_print_absrval(rval);_T6=Cyc_stderr;_T7=
_tag_fat("\n",sizeof(char),2U);_T8=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T6,_T7,_T8);}
# 1146
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){struct Cyc_Dict_Dict _T0;_T0=d;
Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,_T0);}
# 1150
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){union Cyc_CfFlowInfo_FlowInfo _T0;struct _union_FlowInfo_BottomFL _T1;unsigned _T2;struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;union Cyc_CfFlowInfo_FlowInfo _T6;struct _union_FlowInfo_ReachableFL _T7;struct Cyc___cycFILE*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct Cyc___cycFILE*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct Cyc_Dict_Dict _TE;_T0=f;_T1=_T0.BottomFL;_T2=_T1.tag;if(_T2!=1)goto _TL225;_T3=Cyc_stderr;_T4=
# 1152
_tag_fat("  BottomFL\n",sizeof(char),12U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T3,_T4,_T5);goto _LL0;_TL225: _T6=f;_T7=_T6.ReachableFL;_TE=_T7.val;{struct Cyc_Dict_Dict fd=_TE;_T8=Cyc_stderr;_T9=
# 1154
_tag_fat("  ReachableFL:\n",sizeof(char),16U);_TA=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T8,_T9,_TA);
Cyc_CfFlowInfo_print_flowdict(fd);_TB=Cyc_stderr;_TC=
_tag_fat("\n",sizeof(char),2U);_TD=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TB,_TC,_TD);goto _LL0;}_LL0:;}

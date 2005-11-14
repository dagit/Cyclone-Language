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
 struct Cyc_Core_Opt{void*v;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 885 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 913
void*Cyc_Absyn_compress(void*);
# 948
extern void*Cyc_Absyn_void_type;
# 1189
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1191
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 114 "relations-ap.h"
int Cyc_Relations_relns_approx(struct Cyc_List_List*,struct Cyc_List_List*);
# 116
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 118
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 54 "tcutil.h"
int Cyc_Tcutil_is_bits_only_type(void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 140
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 192
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*,void*,int);
# 250
int Cyc_Tcutil_force_type2bool(int,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 33 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 41 "cf_flowinfo.h"
extern int Cyc_CfFlowInfo_anal_error;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};char Cyc_CfFlowInfo_IsZero[7U]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8U]="NotZero";char Cyc_CfFlowInfo_UnknownZ[9U]="UnknownZ";struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;int f4;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f0;struct Cyc_List_List*f1;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 162 "cf_flowinfo.h"
int Cyc_CfFlowInfo_root_cmp(void*,void*);
# 172
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict,void*);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict,struct Cyc_CfFlowInfo_Place*);
# 176
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);
# 38 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 41
Cyc_CfFlowInfo_anal_error=1;
Cyc_Warn_verr(loc,fmt,ap);}
# 46
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
union Cyc_CfFlowInfo_AbsLVal _Tmp0;_Tmp0.PlaceL.tag=1U,_Tmp0.PlaceL.val=x;return _Tmp0;}
# 50
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL (void){
union Cyc_CfFlowInfo_AbsLVal _Tmp0;_Tmp0.UnknownL.tag=2U,_Tmp0.UnknownL.val=0;return _Tmp0;}
# 53
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL (void){union Cyc_CfFlowInfo_FlowInfo _Tmp0;_Tmp0.BottomFL.tag=1U,_Tmp0.BottomFL.val=0;return _Tmp0;}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
union Cyc_CfFlowInfo_FlowInfo _Tmp0;_Tmp0.ReachableFL.tag=2U,_Tmp0.ReachableFL.val.f0=fd,_Tmp0.ReachableFL.val.f1=r;return _Tmp0;}
# 58
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env (void){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_FlowEnv*_Tmp0=_cycalloc(sizeof(struct Cyc_CfFlowInfo_FlowEnv));
({void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=0;_Tmp2;});_Tmp0->zero=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=1;_Tmp2;});_Tmp0->notzeroall=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=2,_Tmp2->f1=0U;_Tmp2;});_Tmp0->unknown_none=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=2,_Tmp2->f1=1U;_Tmp2;});_Tmp0->unknown_all=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=3,_Tmp2->f1=0U;_Tmp2;});_Tmp0->esc_none=_Tmp1;}),({
void*_Tmp1=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct));_Tmp2->tag=3,_Tmp2->f1=1U;_Tmp2;});_Tmp0->esc_all=_Tmp1;}),({
struct Cyc_Dict_Dict _Tmp1=Cyc_Dict_empty(Cyc_CfFlowInfo_root_cmp);_Tmp0->mt_flowdict=_Tmp1;}),({
struct Cyc_CfFlowInfo_Place*_Tmp1=({struct Cyc_CfFlowInfo_Place*_Tmp2=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place));({void*_Tmp3=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct));_Tmp4->tag=1,_Tmp4->f1=& dummy_exp,_Tmp4->f2=Cyc_Absyn_void_type;_Tmp4;});_Tmp2->root=_Tmp3;}),_Tmp2->path=0;_Tmp2;});_Tmp0->dummy_place=_Tmp1;});return _Tmp0;}
# 74
struct _fat_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _Tmp0=*place;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.root;_Tmp1=_Tmp0.path;{void*root=_Tmp2;struct Cyc_List_List*fields=_Tmp1;
void*_Tmp3;if(*((int*)root)==0){_Tmp3=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)root)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 78
if(fields==0)
return Cyc_Absynpp_qvar2string(vd->name);else{
# 81
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("reachable from %s",sizeof(char),18U),_tag_fat(_Tmp5,sizeof(void*),1));});}}}else{
({int(*_Tmp4)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp4;})(_tag_fat("error locations not for VarRoots",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));};}}
# 87
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _fat_ptr*f){
int n=0;
for(1;(unsigned)fs;fs=fs->tl){
struct _fat_ptr*f2=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(f2,f)==0)return n;
n=n + 1;}
# 94
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)*f;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("get_field_index_fs failed: %s",sizeof(char),30U),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 97
int Cyc_CfFlowInfo_get_field_index(void*t,struct _fat_ptr*f){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;union Cyc_Absyn_AggrInfo _Tmp2;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp2;
# 100
struct Cyc_List_List*fs=_check_null(Cyc_Absyn_get_known_aggrdecl(info)->impl)->fields;
_Tmp1=fs;goto _LL4;}}else{goto _LL5;}case 6: _Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;_LL4: {struct Cyc_List_List*fs=_Tmp1;
# 103
return Cyc_CfFlowInfo_get_field_index_fs(fs,f);}default: _LL5:
# 105
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_Absynpp_typ2string(t);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("get_field_index failed: %s",sizeof(char),27U),_tag_fat(_Tmp4,sizeof(void*),1));});};}struct _tuple12{void*f0;void*f1;};
# 109
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1==(int)r2)
return 0;{
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=r1,_Tmp1.f1=r2;_Tmp1;});int _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp4=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl*vd1=_Tmp4;struct Cyc_Absyn_Vardecl*vd2=_Tmp3;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==0)
return 1;else{if(*((int*)_Tmp0.f0)==1){if(*((int*)_Tmp0.f1)==1){_Tmp4=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
return(int)e1 - (int)e2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==1)
return 1;else{_Tmp2=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_Tmp0.f1)->f1;{int i1=_Tmp2;int i2=_Tmp1;
return i1 - i2;}}}}};}}
# 123
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=p1,_Tmp1.f1=p2;_Tmp1;});int _Tmp1;int _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp0.f1)->f1;{int i1=_Tmp2;int i2=_Tmp1;
# 126
if(i1==i2)return 0;
if(i1 < i2)return -1;else{
return 1;}}}else{
# 131
return 1;}}else{if(*((int*)_Tmp0.f1)==1)
# 129
return 0;else{
return -1;}};}
# 135
static int Cyc_CfFlowInfo_path_cmp(struct Cyc_List_List*path1,struct Cyc_List_List*path2){
return Cyc_List_list_cmp(Cyc_CfFlowInfo_pathcon_cmp,path1,path2);}
# 139
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1==(int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _Tmp0=*p1;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.root;_Tmp1=_Tmp0.path;{void*root1=_Tmp2;struct Cyc_List_List*path1=_Tmp1;
struct Cyc_CfFlowInfo_Place _Tmp3=*p2;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.root;_Tmp4=_Tmp3.path;{void*root2=_Tmp5;struct Cyc_List_List*path2=_Tmp4;
int i=Cyc_CfFlowInfo_root_cmp(root1,root2);
if(i!=0)
return i;
return Cyc_CfFlowInfo_path_cmp(path1,path2);}}}}
# 159 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,int,int,void*,void*);
# 163
struct _fat_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 169
unsigned sz=(unsigned)Cyc_List_length(fs);
struct _fat_ptr d=({unsigned _Tmp0=sz;_tag_fat(({void**_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(void*)));({{unsigned _Tmp2=sz;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=fenv->unknown_all;}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_Tmp0=(struct Cyc_Absyn_Aggrfield*)_check_null(fs)->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->name;_Tmp1=_Tmp0->type;{struct _fat_ptr*n=_Tmp2;void*t2=_Tmp1;
if(_get_fat_size(*n,sizeof(char))!=1U)
({void*_Tmp3=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,t2,leafval);((void**)d.curr)[i]=_Tmp3;});}}}
# 177
return d;}struct _tuple13{struct _RegionHandle*f0;struct Cyc_List_List*f1;};
# 181
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple13*env,struct Cyc_Absyn_Aggrfield*f){
# 185
void*_Tmp0;void*_Tmp1;_Tmp1=env->f0;_Tmp0=env->f1;{struct _RegionHandle*t=_Tmp1;struct Cyc_List_List*inst=_Tmp0;
void*new_typ=Cyc_Tcutil_rsubstitute(t,inst,f->type);
struct Cyc_Absyn_Exp*r=f->requires_clause;
struct Cyc_Absyn_Exp*new_r=r==0?0: Cyc_Tcutil_rsubsexp(t,inst,r);
struct Cyc_Absyn_Aggrfield*_Tmp2=_region_malloc(t,0U,sizeof(struct Cyc_Absyn_Aggrfield));_Tmp2->name=f->name,_Tmp2->tq=f->tq,_Tmp2->type=new_typ,_Tmp2->width=f->width,_Tmp2->attributes=f->attributes,_Tmp2->requires_clause=new_r;return _Tmp2;}}struct _tuple14{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 196
static struct _fat_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 202
struct _RegionHandle _Tmp0=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
# 206
{struct Cyc_List_List*inst=0;
for(1;tvs!=0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*t=(void*)_check_null(targs)->hd;
{struct Cyc_Absyn_Kind*_Tmp1=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);if(((struct Cyc_Absyn_Kind*)_Tmp1)->kind==Cyc_Absyn_EffKind)
# 212
continue;else{
goto _LL0;}_LL0:;}
# 215
inst=({struct Cyc_List_List*_Tmp1=_region_malloc(temp,0U,sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp2=({struct _tuple14*_Tmp3=_region_malloc(temp,0U,sizeof(struct _tuple14));_Tmp3->f0=tv,_Tmp3->f1=t;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=inst;_Tmp1;});}
# 217
if(inst!=0){
struct _tuple13 env=({struct _tuple13 _Tmp1;_Tmp1.f0=temp,({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(inst);_Tmp1.f1=_Tmp2;});_Tmp1;});
struct Cyc_List_List*subs_fs=({struct Cyc_List_List*(*_Tmp1)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*)(struct _tuple13*,struct Cyc_Absyn_Aggrfield*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c;_Tmp1;})(temp,Cyc_CfFlowInfo_substitute_field,& env,fs);
struct _fat_ptr _Tmp1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _Tmp1;}else{
# 222
struct _fat_ptr _Tmp1=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _Tmp1;}}
# 206
;_pop_region();}struct _tuple15{struct Cyc_Absyn_Tqual f0;void*f1;};
# 226
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 230
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 232
{void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;enum Cyc_Absyn_AggrKind _Tmp2;union Cyc_Absyn_AggrInfo _Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 23: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatypefield.tag==2){_Tmp4=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatypefield.val.f1;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp4;
# 236
if(tuf->typs==0)
return leafval;{
struct Cyc_List_List*tqts=tuf->typs;
unsigned sz=(unsigned)(Cyc_List_length(tqts)+ 1);
struct _fat_ptr d=({unsigned _Tmp5=sz;_tag_fat(({void**_Tmp6=_cycalloc(_check_times(_Tmp5,sizeof(void*)));({{unsigned _Tmp7=sz;unsigned i;for(i=0;i < _Tmp7;++ i){_Tmp6[i]=fenv->unknown_all;}}0;});_Tmp6;}),sizeof(void*),_Tmp5);});
*((void**)_check_fat_subscript(d,sizeof(void*),0))=leafval;
{int i=1;for(0;(unsigned)i < sz;++ i){
({void*_Tmp5=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple15*)_check_null(tqts)->hd)).f1,leafval);((void**)d.curr)[i]=_Tmp5;});
tqts=tqts->tl;}}
# 246
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=6,_Tmp5->f1.is_union=0,_Tmp5->f1.fieldnum=-1,_Tmp5->f2=d;_Tmp5;});}}}else{goto _LLD;}case 24: _Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp3;struct Cyc_List_List*targs=_Tmp4;
# 248
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl==0)
goto _LL0;{
struct Cyc_List_List*fields=ad->impl->fields;
if(targs==0){_Tmp2=ad->kind;_Tmp4=fields;goto _LL6;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=6,_Tmp5->f1.is_union=(int)ad->kind==1,_Tmp5->f1.fieldnum=-1,({
struct _fat_ptr _Tmp6=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,ad->tvs,targs,fields,(int)ad->kind==1,leafval);_Tmp5->f2=_Tmp6;});_Tmp5;});}}case 5: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp4;
# 271
return leafval;}}else{goto _LLD;}default: goto _LLD;}case 6: _Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;_LL6: {enum Cyc_Absyn_AggrKind k=_Tmp2;struct Cyc_List_List*fs=_Tmp4;
# 263
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=6,_Tmp5->f1.is_union=(int)k==1,_Tmp5->f1.fieldnum=-1,({
struct _fat_ptr _Tmp6=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,(int)k==1,leafval);_Tmp5->f2=_Tmp6;});_Tmp5;});}case 4: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp1=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.zero_term;{void*et=_Tmp4;void*zeroterm=_Tmp1;
# 266
if(Cyc_Tcutil_force_type2bool(0,zeroterm)){
# 269
if((allow_zeroterm && !no_init_bits_only)&& Cyc_Tcutil_is_bits_only_type(et))return fenv->unknown_all;else{return leafval;}}
goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.nullable;{void*nbl=_Tmp4;
# 273
if(!Cyc_Tcutil_force_type2bool(0,nbl)){
if(*((int*)leafval)==2){if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)leafval)->f1==Cyc_CfFlowInfo_AllIL)
return fenv->notzeroall;else{goto _LL12;}}else{_LL12:
 goto _LLF;}_LLF:;}
# 278
goto _LL0;}default: _LLD:
 goto _LL0;}_LL0:;}
# 282
if(!no_init_bits_only && Cyc_Tcutil_is_bits_only_type(t))return fenv->unknown_all;else{return leafval;}}
# 285
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 291
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
struct _fat_ptr _Tmp0;int _Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;switch(*((int*)r)){case 7: _Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f1;_Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f2;_Tmp2=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f3;_Tmp1=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f4;{struct Cyc_Absyn_Exp*consumer=_Tmp4;int iteration=_Tmp3;void*r=_Tmp2;int local=_Tmp1;
# 294
if(consumer==e && iteration==env_iteration){
*needs_unconsume=1;
return 0;}
# 298
return 1;}case 6: _Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1.is_union;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1.fieldnum;_Tmp0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f2;{int is_union=_Tmp3;int field_no=_Tmp1;struct _fat_ptr d=_Tmp0;
# 300
if(!is_union || field_no==-1){
unsigned sz=_get_fat_size(d,sizeof(void*));
{int i=0;for(0;(unsigned)i < sz;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_fat_subscript(d,sizeof(void*),i)),needs_unconsume))
return 1;}}
return 0;}else{
# 308
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_fat_subscript(d,sizeof(void*),field_no)),needs_unconsume);}}case 8: _Tmp4=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f2;{void*r=_Tmp4;
# 310
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,r,needs_unconsume);}default:
 return 0;};}
# 317
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
struct _fat_ptr _Tmp0;struct Cyc_CfFlowInfo_UnionRInfo _Tmp1;int _Tmp2;void*_Tmp3;int _Tmp4;void*_Tmp5;switch(*((int*)r)){case 7: _Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f2;_Tmp3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f3;_Tmp2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f4;{struct Cyc_Absyn_Exp*consumer=_Tmp5;int iteration=_Tmp4;void*r=_Tmp3;int local=_Tmp2;
# 320
return r;}case 6: _Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1;_Tmp0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f2;{struct Cyc_CfFlowInfo_UnionRInfo uinfo=_Tmp1;struct _fat_ptr d=_Tmp0;
# 322
unsigned sz=_get_fat_size(d,sizeof(void*));
int change=0;
struct _fat_ptr d2=({unsigned _Tmp6=sz;_tag_fat(({void**_Tmp7=_cycalloc(_check_times(_Tmp6,sizeof(void*)));({{unsigned _Tmp8=sz;unsigned i;for(i=0;i < _Tmp8;++ i){_Tmp7[i]=*((void**)_check_fat_subscript(d,sizeof(void*),(int)i));}}0;});_Tmp7;}),sizeof(void*),_Tmp6);});
{int i=0;for(0;(unsigned)i < sz;++ i){
({void*_Tmp6=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,*((void**)_check_fat_subscript(d,sizeof(void*),i)));*((void**)_check_fat_subscript(d2,sizeof(void*),i))=_Tmp6;});
if(((void**)d2.curr)[i]!=((void**)d.curr)[i])
change=1;}}
# 330
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=6,_Tmp6->f1=uinfo,_Tmp6->f2=d2;_Tmp6;});else{
return r;}}case 8: _Tmp5=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f1;_Tmp3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp5;void*r2=_Tmp3;
# 334
void*r3=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,r2);
if(r3!=r2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=8,_Tmp6->f1=n,_Tmp6->f2=r3;_Tmp6;});
return r;}default:
 return r;};}
# 344
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*aqb,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r,int local_alias){
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(t);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=r;_Tmp1;});enum Cyc_Absyn_AggrKind _Tmp1;struct _fat_ptr _Tmp2;struct Cyc_CfFlowInfo_UnionRInfo _Tmp3;union Cyc_Absyn_AggrInfo _Tmp4;void*_Tmp5;void*_Tmp6;if(*((int*)_Tmp0.f1)==8){_Tmp6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*s=_Tmp6;void*r2=_Tmp5;
# 347
void*r3=Cyc_CfFlowInfo_make_unique_consumed(fenv,aqb,t,consumer,iteration,r2,local_alias);
if(r3!=r2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=8,_Tmp7->f1=s,_Tmp7->f2=r3;_Tmp7;});
return r;}}else{switch(*((int*)_Tmp0.f0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==24){if(*((int*)_Tmp0.f1)==6){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{union Cyc_Absyn_AggrInfo info=_Tmp4;struct Cyc_CfFlowInfo_UnionRInfo b=_Tmp3;struct _fat_ptr d=_Tmp2;
# 352
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(ad->impl==0)return r;
_Tmp1=ad->kind;_Tmp6=ad->impl->fields;_Tmp3=b;_Tmp2=d;goto _LL6;}}else{goto _LL7;}}else{goto _LL7;}case 6: if(*((int*)_Tmp0.f1)==6){_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f0)->f3;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_LL6: {enum Cyc_Absyn_AggrKind k=_Tmp1;struct Cyc_List_List*fs=_Tmp6;struct Cyc_CfFlowInfo_UnionRInfo b=_Tmp3;struct _fat_ptr d=_Tmp2;
# 356
struct _fat_ptr d2=({unsigned _Tmp7=_get_fat_size(d,sizeof(void*));_tag_fat(({void**_Tmp8=_cycalloc(_check_times(_Tmp7,sizeof(void*)));({{unsigned _Tmp9=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _Tmp9;++ i){_Tmp8[i]=((void**)d.curr)[(int)i];}}0;});_Tmp8;}),sizeof(void*),_Tmp7);});
unsigned sz=(unsigned)Cyc_List_length(fs);
{int i=0;for(0;(unsigned)i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_Tmp7=(struct Cyc_Absyn_Aggrfield*)_check_null(fs)->hd;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7->name;_Tmp8=_Tmp7->type;{struct _fat_ptr*n=_Tmp9;void*t2=_Tmp8;
if(_get_fat_size(*n,sizeof(char))!=1U)
({void*_TmpA=Cyc_CfFlowInfo_make_unique_consumed(fenv,aqb,t2,consumer,iteration,*((void**)_check_fat_subscript(d,sizeof(void*),i)),local_alias);((void**)d2.curr)[i]=_TmpA;});}}}
# 363
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=6,_Tmp7->f1=b,_Tmp7->f2=d2;_Tmp7;});}}else{goto _LL7;}default: _LL7:
# 365
 if(Cyc_Tcutil_is_noalias_pointer(aqb,t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=7,_Tmp7->f1=consumer,_Tmp7->f2=iteration,_Tmp7->f3=r,_Tmp7->f4=local_alias;_Tmp7;});
return r;}};}struct _tuple16{struct Cyc_CfFlowInfo_Place*f0;unsigned f1;};
# 371
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 373
struct _RegionHandle _Tmp0=_new_region(0U,"r");struct _RegionHandle*r=& _Tmp0;_push_region(r);
{struct _tuple16 elem=({struct _tuple16 _Tmp1;_Tmp1.f0=place,_Tmp1.f1=0U;_Tmp1;});
struct Cyc_Iter_Iter iter=Cyc_Dict_make_iter(r,set);
while(({int(*_Tmp1)(struct Cyc_Iter_Iter,struct _tuple16*)=(int(*)(struct Cyc_Iter_Iter,struct _tuple16*))Cyc_Iter_next;_Tmp1;})(iter,& elem)){
{struct Cyc_CfFlowInfo_Place*place2=elem.f0;
struct Cyc_CfFlowInfo_Place _Tmp1=*place;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.root;_Tmp2=_Tmp1.path;{void*root1=_Tmp3;struct Cyc_List_List*fs1=_Tmp2;
struct Cyc_CfFlowInfo_Place _Tmp4=*place2;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.root;_Tmp5=_Tmp4.path;{void*root2=_Tmp6;struct Cyc_List_List*fs2=_Tmp5;
if(Cyc_CfFlowInfo_root_cmp(root1,root2)!=0)
continue;
for(1;fs1!=0 && fs2!=0;(fs1=fs1->tl,fs2=fs2->tl)){
if((void*)fs1->hd!=(void*)fs2->hd)break;}
if(fs1==0){
int _Tmp7=1;_npop_handler(0);return _Tmp7;}}}}
# 377
1U;}{
# 387
int _Tmp1=0;_npop_handler(0);return _Tmp1;}}
# 374
;_pop_region();}struct Cyc_CfFlowInfo_EscPile{struct Cyc_List_List*places;};
# 397
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 400
if(!({int(*_Tmp0)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*)=(int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))Cyc_List_mem;_Tmp0;})(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=place,_Tmp1->tl=pile->places;_Tmp1;});pile->places=_Tmp0;});}
# 403
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
struct _fat_ptr _Tmp0;struct Cyc_CfFlowInfo_UnionRInfo _Tmp1;void*_Tmp2;switch(*((int*)r)){case 7: _Tmp2=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f3;{void*r=_Tmp2;
_Tmp2=r;goto _LL4;}case 8: _Tmp2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f2;_LL4: {void*r=_Tmp2;
Cyc_CfFlowInfo_add_places(pile,r);return;}case 4: _Tmp2=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)r)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp2;
Cyc_CfFlowInfo_add_place(pile,p);return;}case 6: _Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1;_Tmp0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f2;{struct Cyc_CfFlowInfo_UnionRInfo b=_Tmp1;struct _fat_ptr d=_Tmp0;
# 409
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)d.curr)[i]);}}
return;}default:
 return;};}
# 419
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){
int _Tmp0;void*_Tmp1;void*_Tmp2;struct _fat_ptr _Tmp3;int _Tmp4;switch(*((int*)old_val)){case 6: _Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)old_val)->f1.is_union;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)old_val)->f2;{int is_union=_Tmp4;struct _fat_ptr d=_Tmp3;
# 422
struct _fat_ptr d2=({unsigned _Tmp5=_get_fat_size(d,sizeof(void*));_tag_fat(({void**_Tmp6=_cycalloc(_check_times(_Tmp5,sizeof(void*)));({{unsigned _Tmp7=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _Tmp7;++ i){({void*_Tmp8=Cyc_CfFlowInfo_insert_place_inner(new_val,*((void**)_check_fat_subscript(d,sizeof(void*),(int)i)));_Tmp6[i]=_Tmp8;});}}0;});_Tmp6;}),sizeof(void*),_Tmp5);});
# 426
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=6,_Tmp5->f1.is_union=is_union,_Tmp5->f1.fieldnum=-1,_Tmp5->f2=d2;_Tmp5;});}case 7: _Tmp2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)old_val)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)old_val)->f2;_Tmp1=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)old_val)->f3;_Tmp0=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)old_val)->f4;{struct Cyc_Absyn_Exp*e=_Tmp2;int i=_Tmp4;void*rval=_Tmp1;int l=_Tmp0;
# 428
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=7,_Tmp5->f1=e,_Tmp5->f2=i,({void*_Tmp6=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_Tmp5->f3=_Tmp6;}),_Tmp5->f4=l;_Tmp5;});}case 8: _Tmp2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)old_val)->f1;_Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)old_val)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp2;void*rval=_Tmp1;
# 430
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp5->tag=8,_Tmp5->f1=n,({void*_Tmp6=Cyc_CfFlowInfo_insert_place_inner(new_val,rval);_Tmp5->f2=_Tmp6;});_Tmp5;});}default:
 return new_val;};}
# 437
static struct _fat_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _fat_ptr d,int n,void*rval){
void*old_rval=*((void**)_check_fat_subscript(d,sizeof(void*),n));
if(old_rval==rval)return d;{
struct _fat_ptr res=({unsigned _Tmp0=_get_fat_size(d,sizeof(void*));_tag_fat(({void**_Tmp1=_cycalloc(_check_times(_Tmp0,sizeof(void*)));({{unsigned _Tmp2=_get_fat_size(d,sizeof(void*));unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=((void**)d.curr)[(int)i];}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});
((void**)res.curr)[n]=rval;
return res;}}struct _tuple17{struct Cyc_List_List*f0;void*f1;};
# 450
static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*path,void*old_val,void*new_val){
# 452
if(path==0)
return Cyc_CfFlowInfo_insert_place_inner(new_val,old_val);{
struct _tuple17 _Tmp0=({struct _tuple17 _Tmp1;_Tmp1.f0=path,_Tmp1.f1=old_val;_Tmp1;});void*_Tmp1;struct _fat_ptr _Tmp2;int _Tmp3;void*_Tmp4;int _Tmp5;if(_Tmp0.f0!=0){if(*((int*)((struct Cyc_List_List*)_Tmp0.f0)->hd)==0)switch(*((int*)_Tmp0.f1)){case 6: _Tmp5=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp0.f0->hd)->f1;_Tmp4=_Tmp0.f0->tl;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1.is_union;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{int i=_Tmp5;struct Cyc_List_List*tl=_Tmp4;int is_union=_Tmp3;struct _fat_ptr d=_Tmp2;
# 456
void*new_child=Cyc_CfFlowInfo_insert_place_outer(tl,*((void**)_check_fat_subscript(d,sizeof(void*),i)),new_val);
struct _fat_ptr new_d=Cyc_CfFlowInfo_aggr_dict_insert(d,i,new_child);
if((void**)new_d.curr==(void**)d.curr)return old_val;
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=6,_Tmp6->f1.is_union=is_union,_Tmp6->f1.fieldnum=-1,_Tmp6->f2=new_d;_Tmp6;});}case 8: goto _LL5;default: goto _LL7;}else{switch(*((int*)_Tmp0.f1)){case 5: _Tmp4=_Tmp0.f0->tl;_Tmp1=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_List_List*tl=_Tmp4;void*rval=_Tmp1;
# 461
void*new_rval=Cyc_CfFlowInfo_insert_place_outer(tl,rval,new_val);
if(new_rval==rval)return old_val;
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=5,_Tmp6->f1=new_rval;_Tmp6;});}case 8: goto _LL5;default: goto _LL7;}}}else{if(*((int*)_Tmp0.f1)==8){_LL5: _Tmp4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp4;void*rval=_Tmp1;
# 465
void*new_rval=Cyc_CfFlowInfo_insert_place_outer(path,rval,new_val);
if(new_rval==rval)return old_val;
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp6->tag=8,_Tmp6->f1=n,_Tmp6->f2=new_rval;_Tmp6;});}}else{_LL7:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("bad insert place",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));}};}}
# 475
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
while(pile->places!=0){
{struct Cyc_CfFlowInfo_Place*place=(struct Cyc_CfFlowInfo_Place*)pile->places->hd;
pile->places=pile->places->tl;{
void*oldval;void*newval;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){oldval=Cyc_CfFlowInfo_lookup_place(d,place);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent)
continue;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
{enum Cyc_CfFlowInfo_InitLevel _Tmp0=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);if(_Tmp0==Cyc_CfFlowInfo_AllIL){
newval=fenv->esc_all;goto _LL5;}else{
newval=fenv->esc_none;goto _LL5;}_LL5:;}
# 486
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _Tmp0=*place;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.root;_Tmp1=_Tmp0.path;{void*root=_Tmp2;struct Cyc_List_List*path=_Tmp1;
d=({struct Cyc_Dict_Dict _Tmp3=d;void*_Tmp4=root;Cyc_Dict_insert(_Tmp3,_Tmp4,({
struct Cyc_List_List*_Tmp5=path;void*_Tmp6=
Cyc_Dict_lookup(d,root);
# 489
Cyc_CfFlowInfo_insert_place_outer(_Tmp5,_Tmp6,newval);}));});}}}}
# 477
1U;}
# 493
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 503
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_Tmp0;enum Cyc_CfFlowInfo_InitLevel _Tmp1;switch(*((int*)r)){case 2: _Tmp1=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)r)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp1;
return il;}case 3: _Tmp1=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)r)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp1;
return il;}case 0:
 goto _LL8;case 1: _LL8:
 return 1U;case 7: _Tmp0=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f3;{void*r=_Tmp0;
return 0U;}default:
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("initlevel_approx",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));};}
# 513
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 515
enum Cyc_CfFlowInfo_InitLevel this_ans;
if((int)acc==0)return 0U;
{struct _fat_ptr _Tmp0;int _Tmp1;int _Tmp2;void*_Tmp3;switch(*((int*)r)){case 8: _Tmp3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f2;{void*r=_Tmp3;
return Cyc_CfFlowInfo_initlevel_rec(env,r,acc);}case 6: _Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1.is_union;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f1.fieldnum;_Tmp0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f2;if((int)_Tmp2){int iu=_Tmp2;int f=_Tmp1;struct _fat_ptr d=_Tmp0;
# 523
unsigned sz=_get_fat_size(d,sizeof(void*));
this_ans=0U;
if(f==-1){
int i=0;for(0;(unsigned)i < sz;++ i){
if((int)Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_fat_subscript(d,sizeof(void*),i)),1U)==1){
this_ans=1U;
break;}}}else{
# 531
if((int)Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_fat_subscript(d,sizeof(void*),f)),1U)==1)
this_ans=1U;}
goto _LL0;}else{_Tmp0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)r)->f2;{struct _fat_ptr d=_Tmp0;
# 535
unsigned sz=_get_fat_size(d,sizeof(void*));
this_ans=1U;
{int i=0;for(0;(unsigned)i < sz;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_fat_subscript(d,sizeof(void*),i)),this_ans);}}
goto _LL0;}}case 4: _Tmp3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)r)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp3;
# 541
if(({int(*_Tmp4)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*)=(int(*)(int(*)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*,struct Cyc_CfFlowInfo_Place*))Cyc_List_mem;_Tmp4;})(Cyc_CfFlowInfo_place_cmp,env->seen,p))
this_ans=1U;else{
# 544
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=p,_Tmp5->tl=env->seen;_Tmp5;});env->seen=_Tmp4;});
this_ans=({struct Cyc_CfFlowInfo_InitlevelEnv*_Tmp4=env;Cyc_CfFlowInfo_initlevel_rec(_Tmp4,Cyc_CfFlowInfo_lookup_place(env->d,p),1U);});
env->seen=_check_null(env->seen)->tl;}
# 548
goto _LL0;}case 5: _Tmp3=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)r)->f1;{void*r=_Tmp3;
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,r,1U);goto _LL0;}default:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);goto _LL0;}_LL0:;}
# 552
return this_ans;}
# 554
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv env2=({struct Cyc_CfFlowInfo_InitlevelEnv _Tmp0;_Tmp0.d=d,_Tmp0.seen=0;_Tmp0;});
return Cyc_CfFlowInfo_initlevel_rec(& env2,r,1U);}
# 559
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _Tmp0=*place;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.root;_Tmp1=_Tmp0.path;{void*root=_Tmp2;struct Cyc_List_List*path=_Tmp1;
void*ans=Cyc_Dict_lookup(d,root);
for(1;path!=0;path=path->tl){
retry: {struct _tuple12 _Tmp3=({struct _tuple12 _Tmp4;_Tmp4.f0=ans,_Tmp4.f1=(void*)path->hd;_Tmp4;});int _Tmp4;struct _fat_ptr _Tmp5;struct Cyc_CfFlowInfo_UnionRInfo _Tmp6;void*_Tmp7;switch(*((int*)_Tmp3.f0)){case 8: _Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp3.f0)->f2;{void*rval=_Tmp7;
# 565
ans=rval;goto retry;}case 7: _Tmp7=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp3.f0)->f3;{void*rval=_Tmp7;
# 568
ans=rval;goto retry;}case 6: if(*((int*)_Tmp3.f1)==0){_Tmp6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp3.f0)->f1;_Tmp5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp3.f0)->f2;_Tmp4=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp3.f1)->f1;{struct Cyc_CfFlowInfo_UnionRInfo is_union=_Tmp6;struct _fat_ptr d2=_Tmp5;int fname=_Tmp4;
ans=*((void**)_check_fat_subscript(d2,sizeof(void*),fname));goto _LL3;}}else{goto _LLC;}case 5: if(*((int*)_Tmp3.f1)==1){_Tmp7=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp3.f0)->f1;{void*rval=_Tmp7;
ans=rval;goto _LL3;}}else{goto _LLC;}default: _LLC:
# 577
({int(*_Tmp8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp8;})(_tag_fat("bad lookup_place",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));}_LL3:;}}
# 579
return ans;}}
# 582
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
struct _fat_ptr _Tmp0;int _Tmp1;int _Tmp2;void*_Tmp3;switch(*((int*)rval)){case 3:
 return 0;case 7: _Tmp3=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)rval)->f3;{void*r=_Tmp3;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 8: _Tmp3=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rval)->f2;{void*r=_Tmp3;
return Cyc_CfFlowInfo_is_rval_unescaped(r);}case 6: _Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)rval)->f1.is_union;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)rval)->f1.fieldnum;_Tmp0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)rval)->f2;{int is_union=_Tmp2;int field_no=_Tmp1;struct _fat_ptr d=_Tmp0;
# 588
if(is_union && field_no!=-1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_fat_subscript(d,sizeof(void*),field_no)));else{
# 591
unsigned sz=_get_fat_size(d,sizeof(void*));
{int i=0;for(0;(unsigned)i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_fat_subscript(d,sizeof(void*),i))))return 0;}}
return 1;}}default:
# 596
 return 1;};}
# 599
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 602
int Cyc_CfFlowInfo_is_init_pointer(void*rval){
void*_Tmp0;switch(*((int*)rval)){case 8: _Tmp0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rval)->f2;{void*r=_Tmp0;
return Cyc_CfFlowInfo_is_init_pointer(r);}case 4:
 goto _LL6;case 5: _LL6:
 return 1;default:
 return 0;};}
# 612
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle _Tmp0=_new_region(0U,"rgn");struct _RegionHandle*rgn=& _Tmp0;_push_region(rgn);
{struct Cyc_CfFlowInfo_EscPile*pile;pile=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile)),pile->places=0;
Cyc_CfFlowInfo_add_places(pile,r);{
struct Cyc_Dict_Dict _Tmp1=Cyc_CfFlowInfo_escape_these(fenv,pile,d);_npop_handler(0);return _Tmp1;}}
# 614
;_pop_region();}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 626
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 631
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=oldval,_Tmp1.f1=newval;_Tmp1;});enum Cyc_CfFlowInfo_InitLevel _Tmp1;struct _fat_ptr _Tmp2;struct Cyc_CfFlowInfo_UnionRInfo _Tmp3;struct _fat_ptr _Tmp4;struct Cyc_CfFlowInfo_UnionRInfo _Tmp5;void*_Tmp6;void*_Tmp7;if(*((int*)_Tmp0.f0)==8){_Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;{void*r1=_Tmp7;
# 633
return Cyc_CfFlowInfo_assign_place_inner(env,r1,newval);}}else{if(*((int*)_Tmp0.f1)==8){_Tmp7=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp7;void*r=_Tmp6;
# 635
void*new_rval=Cyc_CfFlowInfo_assign_place_inner(env,oldval,r);
if(new_rval==r)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp8->tag=8,_Tmp8->f1=n,_Tmp8->f2=new_rval;_Tmp8;});}}}else{switch(*((int*)_Tmp0.f0)){case 3: if(*((int*)_Tmp0.f1)==4){_Tmp7=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp7;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL8;}}else{_LL8:
# 640
 if((int)Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!=1)
Cyc_CfFlowInfo_aerr(env->loc,
_tag_fat("assignment puts possibly-uninitialized data in an escaped location",sizeof(char),67U),_tag_fat(0U,sizeof(void*),0));
return env->fenv->esc_all;}case 6: switch(*((int*)_Tmp0.f1)){case 6: _Tmp5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_CfFlowInfo_UnionRInfo is_union1=_Tmp5;struct _fat_ptr d1=_Tmp4;struct Cyc_CfFlowInfo_UnionRInfo is_union2=_Tmp3;struct _fat_ptr d2=_Tmp2;
# 645
struct _fat_ptr new_d=({unsigned _Tmp8=
_get_fat_size(d1,sizeof(void*));_tag_fat(({void**_Tmp9=_cycalloc(_check_times(_Tmp8,sizeof(void*)));({{unsigned _TmpA=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _TmpA;++ i){({void*_TmpB=({struct Cyc_CfFlowInfo_AssignEnv*_TmpC=env;void*_TmpD=*((void**)_check_fat_subscript(d1,sizeof(void*),(int)i));Cyc_CfFlowInfo_assign_place_inner(_TmpC,_TmpD,*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));});_Tmp9[i]=_TmpB;});}}0;});_Tmp9;}),sizeof(void*),_Tmp8);});
# 649
int change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(*((void**)_check_fat_subscript(new_d,sizeof(void*),i))!=((void**)d1.curr)[i]){
change=1;break;}}}
# 654
if(!change){
if(!is_union1.is_union)return oldval;
new_d=d1;}else{
# 659
change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(({void*_Tmp8=*((void**)_check_fat_subscript(new_d,sizeof(void*),i));_Tmp8!=*((void**)_check_fat_subscript(d2,sizeof(void*),i));})){
change=1;break;}}}
# 664
if(!change){
if(!is_union1.is_union)return newval;
new_d=d2;}}
# 669
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp8->tag=6,_Tmp8->f1=is_union2,_Tmp8->f2=new_d;_Tmp8;});}case 3: goto _LLB;default: goto _LLD;}default: if(*((int*)_Tmp0.f1)==3){_LLB: _Tmp1=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp1;
# 671
if(il==Cyc_CfFlowInfo_NoneIL)
return env->fenv->unknown_none;else{
return env->fenv->unknown_all;};}}else{_LLD:
# 675
 return newval;}}}};}
# 681
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 685
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*path,int path_prefix,void*oldval,void*newval){
# 696
if(path==0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple17 _Tmp0=({struct _tuple17 _Tmp1;_Tmp1.f0=path,_Tmp1.f1=oldval;_Tmp1;});struct _fat_ptr _Tmp1;int _Tmp2;int _Tmp3;int _Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0.f1)){case 8: _Tmp6=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp6;void*r=_Tmp5;
# 699
void*new_r=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);
if(new_r==r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=8,_Tmp7->f1=n,_Tmp7->f2=new_r;_Tmp7;});}}case 7: _Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f3;_Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f4;{struct Cyc_Absyn_Exp*x=_Tmp6;int y=_Tmp4;void*r=_Tmp5;int l=_Tmp3;
# 703
void*new_r=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,r,newval);
if(new_r==r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=7,_Tmp7->f1=x,_Tmp7->f2=y,_Tmp7->f3=new_r,_Tmp7->f4=l;_Tmp7;});}}default: if(_Tmp0.f0!=0){if(*((int*)((struct Cyc_List_List*)_Tmp0.f0)->hd)==1){if(*((int*)_Tmp0.f1)==5){_Tmp6=_Tmp0.f0->tl;_Tmp5=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_List_List*tl=_Tmp6;void*r=_Tmp5;
# 707
void*new_r=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,r,newval);
if(new_r==r)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=5,_Tmp7->f1=new_r;_Tmp7;});}}}else{goto _LL9;}}else{if(*((int*)_Tmp0.f1)==6){_Tmp4=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp0.f0->hd)->f1;_Tmp6=_Tmp0.f0->tl;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1.is_union;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1.fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{int fnum=_Tmp4;struct Cyc_List_List*tl=_Tmp6;int is_union=_Tmp3;int fldnum=_Tmp2;struct _fat_ptr d=_Tmp1;
# 711
void*new_child=Cyc_CfFlowInfo_assign_place_outer(env,tl,path_prefix + 1,*((void**)_check_fat_subscript(d,sizeof(void*),fnum)),newval);
# 713
struct _fat_ptr new_child_agg=Cyc_CfFlowInfo_aggr_dict_insert(d,fnum,new_child);
if((void**)new_child_agg.curr==(void**)d.curr &&(!is_union || fldnum==fnum))return oldval;
d=new_child_agg;
# 718
if(is_union){
int changed=0;
int sz=(int)_get_fat_size(d,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i!=fnum){
struct _fat_ptr new_d=({
struct _fat_ptr _Tmp7=d;int _Tmp8=i;Cyc_CfFlowInfo_aggr_dict_insert(_Tmp7,_Tmp8,
Cyc_CfFlowInfo_insert_place_inner(env->fenv->unknown_all,*((void**)_check_fat_subscript(d,sizeof(void*),i))));});
if((void**)new_d.curr!=(void**)d.curr){
d=new_d;
changed=1;}}}}
# 734
if(changed){
struct Cyc_CfFlowInfo_Place*_Tmp7=env->root_place;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7->root;_Tmp8=_Tmp7->path;{void*root=_Tmp9;struct Cyc_List_List*path=_Tmp8;
struct Cyc_List_List*new_path=({struct Cyc_List_List*(*_TmpA)(int(*)(int*,void*),int*,struct Cyc_List_List*)=({struct Cyc_List_List*(*_TmpB)(int(*)(int*,void*),int*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(int*,void*),int*,struct Cyc_List_List*))Cyc_List_filter_c;_TmpB;});int*_TmpB=({int*_TmpC=_cycalloc_atomic(sizeof(int));*_TmpC=path_prefix;_TmpC;});_TmpA(Cyc_CfFlowInfo_nprefix,_TmpB,path);});
struct Cyc_CfFlowInfo_Place*curr_place;curr_place=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Place)),curr_place->root=root,curr_place->path=new_path;}}}
# 740
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_Tmp7->tag=6,_Tmp7->f1.is_union=is_union,_Tmp7->f1.fieldnum=fnum,_Tmp7->f2=d;_Tmp7;});}}else{goto _LL9;}}}else{_LL9:
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("bad assign place",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));}};}}
# 744
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 752
void*_Tmp0;void*_Tmp1;_Tmp1=place->root;_Tmp0=place->path;{void*root=_Tmp1;struct Cyc_List_List*path=_Tmp0;
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _Tmp2;_Tmp2.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_Tmp3=({struct Cyc_CfFlowInfo_EscPile*_Tmp4=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_Tmp4->places=0;_Tmp4;});_Tmp2.pile=_Tmp3;}),_Tmp2.d=d,_Tmp2.loc=loc,_Tmp2.root_place=place;_Tmp2;});
void*newval=({struct Cyc_List_List*_Tmp2=path;void*_Tmp3=Cyc_Dict_lookup(d,root);Cyc_CfFlowInfo_assign_place_outer(& env,_Tmp2,0,_Tmp3,r);});
struct Cyc_CfFlowInfo_FlowEnv*_Tmp2=fenv;struct Cyc_CfFlowInfo_EscPile*_Tmp3=env.pile;return Cyc_CfFlowInfo_escape_these(_Tmp2,_Tmp3,Cyc_Dict_insert(d,root,newval));}}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;int is_try_flow;};
# 768
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*,void*,void*);
# 770
static int Cyc_CfFlowInfo_contains_local_consumed(void*r){
void*_Tmp0;int _Tmp1;switch(*((int*)r)){case 7: _Tmp1=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)r)->f4;{int l=_Tmp1;
return l;}case 5: _Tmp0=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)r)->f1;{void*r1=_Tmp0;
return Cyc_CfFlowInfo_contains_local_consumed(r1);}case 8: _Tmp0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)r)->f2;{void*ac=_Tmp0;
return Cyc_CfFlowInfo_contains_local_consumed(ac);}case 6:
 return 0;default:
 return 0;};}
# 780
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*);struct _tuple18{int f0;int f1;};
# 786
static void*Cyc_CfFlowInfo_join_absRval_tryflow(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1==r2)return r1;{
struct _tuple18 _Tmp0=({struct _tuple18 _Tmp1;({int _Tmp2=Cyc_CfFlowInfo_contains_local_consumed(r1);_Tmp1.f0=_Tmp2;}),({int _Tmp2=Cyc_CfFlowInfo_contains_local_consumed(r2);_Tmp1.f1=_Tmp2;});_Tmp1;});switch((int)_Tmp0.f0){case 1: if(_Tmp0.f1==0)
return r2;else{goto _LL5;}case 0: if(_Tmp0.f1==1)
return r1;else{goto _LL5;}default: _LL5:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);};}}struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f0;enum Cyc_CfFlowInfo_InitLevel f1;};
# 801
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1==r2)return r1;
# 804
{struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=r1,_Tmp1.f1=r2;_Tmp1;});struct _fat_ptr _Tmp1;struct _fat_ptr _Tmp2;int _Tmp3;int _Tmp4;int _Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;if(*((int*)_Tmp0.f0)==8){if(*((int*)_Tmp0.f1)==8){_TmpA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;_Tmp8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*n1=_TmpA;void*r1=_Tmp9;struct Cyc_Absyn_Vardecl*n2=_Tmp8;void*r2=_Tmp7;
# 807
if(n1==n2)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct));_TmpB->tag=8,_TmpB->f1=n1,({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f2=_TmpC;});_TmpB;});
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}else{_TmpA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;{struct Cyc_Absyn_Vardecl*n1=_TmpA;void*r1=_Tmp9;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}}else{if(*((int*)_Tmp0.f1)==8){_TmpA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*n2=_TmpA;void*r2=_Tmp9;
return Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);}}else{if(*((int*)_Tmp0.f0)==7){if(*((int*)_Tmp0.f1)==7){_TmpA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f3;_Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f4;_Tmp8=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f3;_Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;int i1=_Tmp6;void*r1=_Tmp9;int l1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp8;int i2=_Tmp4;void*r2=_Tmp7;int l2=_Tmp3;
# 814
if(e1==e2)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_TmpB->tag=7,_TmpB->f1=e1,i1 > i2?_TmpB->f2=i1:(_TmpB->f2=i2),({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f3=_TmpC;}),_TmpB->f4=l1 && l2;_TmpB;});
{void*_TmpB;if(*((int*)r1)==4){_TmpB=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)r1)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpB;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL21;}}else{
goto _LL21;}_LL21:;}
# 820
{void*_TmpB;if(*((int*)r2)==4){_TmpB=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)r2)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpB;
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL26;}}else{
goto _LL26;}_LL26:;}
# 824
goto _LL0;}}else{_TmpA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f3;_Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;int i1=_Tmp6;void*r1=_Tmp9;int l1=_Tmp5;
# 826
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_TmpB->tag=7,_TmpB->f1=e1,_TmpB->f2=i1,({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f3=_TmpC;}),_TmpB->f4=l1;_TmpB;});}}}else{if(*((int*)_Tmp0.f1)==7){_TmpA=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp6=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;_Tmp9=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f3;_Tmp5=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f4;{struct Cyc_Absyn_Exp*e2=_TmpA;int i2=_Tmp6;void*r2=_Tmp9;int l2=_Tmp5;
# 828
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct));_TmpB->tag=7,_TmpB->f1=e2,_TmpB->f2=i2,({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f3=_TmpC;}),_TmpB->f4=l2;_TmpB;});}}else{switch(*((int*)_Tmp0.f0)){case 4: switch(*((int*)_Tmp0.f1)){case 4: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_CfFlowInfo_Place*p1=_TmpA;struct Cyc_CfFlowInfo_Place*p2=_Tmp9;
# 831
if(Cyc_CfFlowInfo_place_cmp(p1,p2)==0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,p1);
Cyc_CfFlowInfo_add_place(env->pile,p2);
goto _LL0;}case 1: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 837
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _TmpB=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);if(_TmpB==Cyc_CfFlowInfo_AllIL)
return env->fenv->notzeroall;else{
return env->fenv->unknown_none;};}}default: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 863
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}case 1: switch(*((int*)_Tmp0.f1)){case 4: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 843
Cyc_CfFlowInfo_add_place(env->pile,p);{
enum Cyc_CfFlowInfo_InitLevel _TmpB=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);if(_TmpB==Cyc_CfFlowInfo_AllIL)
return env->fenv->notzeroall;else{
return env->fenv->unknown_none;};}}case 5: _TmpA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{void*r2=_TmpA;
# 858
enum Cyc_CfFlowInfo_InitLevel _TmpB=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);if(_TmpB==Cyc_CfFlowInfo_AllIL)
return env->fenv->notzeroall;else{
return env->fenv->unknown_none;};}default: goto _LL1F;}case 5: switch(*((int*)_Tmp0.f1)){case 5: _TmpA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{void*r1=_TmpA;void*r2=_Tmp9;
# 850
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct));_TmpB->tag=5,({void*_TmpC=Cyc_CfFlowInfo_join_absRval(env,a,r1,r2);_TmpB->f1=_TmpC;});_TmpB;});}case 1: _TmpA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;{void*r1=_TmpA;
# 853
enum Cyc_CfFlowInfo_InitLevel _TmpB=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);if(_TmpB==Cyc_CfFlowInfo_AllIL)
return env->fenv->notzeroall;else{
return env->fenv->unknown_none;};}case 4: goto _LL1B;default: goto _LL1F;}default: if(*((int*)_Tmp0.f1)==4){_LL1B: _TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_CfFlowInfo_Place*p=_TmpA;
# 864
Cyc_CfFlowInfo_add_place(env->pile,p);goto _LL0;}}else{if(*((int*)_Tmp0.f0)==6){if(*((int*)_Tmp0.f1)==6){_Tmp6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1.is_union;_Tmp5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1.fieldnum;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1.is_union;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1.fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{int is_union1=_Tmp6;int field_no1=_Tmp5;struct _fat_ptr d1=_Tmp2;int is_union2=_Tmp4;int field_no2=_Tmp3;struct _fat_ptr d2=_Tmp1;
# 867
struct _fat_ptr new_d=({unsigned _TmpB=
_get_fat_size(d1,sizeof(void*));_tag_fat(({void**_TmpC=_cycalloc(_check_times(_TmpB,sizeof(void*)));({{unsigned _TmpD=_get_fat_size(d1,sizeof(void*));unsigned i;for(i=0;i < _TmpD;++ i){env->is_try_flow?({void*_TmpE=({void*(*_TmpF)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*)=({void*(*_Tmp10)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*)=(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval_tryflow;_Tmp10;});struct Cyc_CfFlowInfo_JoinEnv*_Tmp10=env;void*_Tmp11=*((void**)_check_fat_subscript(d1,sizeof(void*),(int)i));_TmpF(_Tmp10,0,_Tmp11,*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));});_TmpC[i]=_TmpE;}):({void*_TmpE=({struct Cyc_CfFlowInfo_JoinEnv*_TmpF=env;void*_Tmp10=a;void*_Tmp11=*((void**)_check_fat_subscript(d1,sizeof(void*),(int)i));Cyc_CfFlowInfo_join_absRval(_TmpF,_Tmp10,_Tmp11,*((void**)_check_fat_subscript(d2,sizeof(void*),(int)i)));});_TmpC[i]=_TmpE;});}}0;});_TmpC;}),sizeof(void*),_TmpB);});
# 873
int change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(*((void**)_check_fat_subscript(new_d,sizeof(void*),i))!=((void**)d1.curr)[i]){
change=1;break;}}}
# 878
if(!change){
if(!is_union1)return r1;
new_d=d1;}else{
# 883
change=0;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(({void*_TmpB=*((void**)_check_fat_subscript(new_d,sizeof(void*),i));_TmpB!=*((void**)_check_fat_subscript(d2,sizeof(void*),i));})){
change=1;break;}}}
# 888
if(!change){
if(!is_union1)return r2;
new_d=d2;}}
# 893
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct));_TmpB->tag=6,_TmpB->f1.is_union=is_union1,field_no1==field_no2?_TmpB->f1.fieldnum=field_no1:(_TmpB->f1.fieldnum=-1),_TmpB->f2=new_d;_TmpB;});}}else{goto _LL1F;}}else{_LL1F:
# 895
 goto _LL0;}}}}}}}_LL0:;}{
# 897
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=r1,_Tmp1.f1=r2;_Tmp1;});if(*((int*)_Tmp0.f0)==3)
goto _LL43;else{if(*((int*)_Tmp0.f1)==3){_LL43: {
# 902
struct _tuple19 _Tmp1=({struct _tuple19 _Tmp2;_Tmp2.f0=il1,_Tmp2.f1=il2;_Tmp2;});if(_Tmp1.f1==Cyc_CfFlowInfo_NoneIL)
goto _LL4A;else{if(_Tmp1.f0==Cyc_CfFlowInfo_NoneIL){_LL4A:
 return env->fenv->esc_none;}else{
return env->fenv->esc_all;}};}}else{
# 908
struct _tuple19 _Tmp1=({struct _tuple19 _Tmp2;_Tmp2.f0=il1,_Tmp2.f1=il2;_Tmp2;});if(_Tmp1.f1==Cyc_CfFlowInfo_NoneIL)
goto _LL51;else{if(_Tmp1.f0==Cyc_CfFlowInfo_NoneIL){_LL51:
 return env->fenv->unknown_none;}else{
return env->fenv->unknown_all;}};}};}}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f0;union Cyc_CfFlowInfo_FlowInfo f1;};
# 920
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_tryflow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo newflow,union Cyc_CfFlowInfo_FlowInfo oldflow){
# 922
struct _tuple20 _Tmp0=({struct _tuple20 _Tmp1;_Tmp1.f0=newflow,_Tmp1.f1=oldflow;_Tmp1;});void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;struct Cyc_Dict_Dict _Tmp4;if(_Tmp0.f0.BottomFL.tag==1)
return oldflow;else{if(_Tmp0.f1.BottomFL.tag==1)
return newflow;else{_Tmp4=_Tmp0.f0.ReachableFL.val.f0;_Tmp3=_Tmp0.f0.ReachableFL.val.f1;_Tmp2=_Tmp0.f1.ReachableFL.val.f0;_Tmp1=_Tmp0.f1.ReachableFL.val.f1;{struct Cyc_Dict_Dict d1=_Tmp4;struct Cyc_List_List*r1=_Tmp3;struct Cyc_Dict_Dict d2=_Tmp2;struct Cyc_List_List*r2=_Tmp1;
# 928
if(d1.t==d2.t && r1==r2)return newflow;
if(Cyc_CfFlowInfo_flow_lessthan_approx(newflow,oldflow))return oldflow;
if(Cyc_CfFlowInfo_flow_lessthan_approx(oldflow,newflow))return newflow;{
struct Cyc_CfFlowInfo_JoinEnv env=({struct Cyc_CfFlowInfo_JoinEnv _Tmp5;_Tmp5.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_Tmp6=({struct Cyc_CfFlowInfo_EscPile*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_Tmp7->places=0;_Tmp7;});_Tmp5.pile=_Tmp6;}),_Tmp5.d1=d1,_Tmp5.d2=d2,_Tmp5.is_try_flow=1;_Tmp5;});
struct Cyc_Dict_Dict outdict=({struct Cyc_Dict_Dict(*_Tmp5)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;_Tmp5;})(Cyc_CfFlowInfo_join_absRval_tryflow,& env,d1,d2);
struct Cyc_List_List*r=Cyc_Relations_join_relns(Cyc_Core_heap_region,r1,r2);
struct Cyc_Dict_Dict _Tmp5=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outdict);return Cyc_CfFlowInfo_ReachableFL(_Tmp5,r);}}}};}
# 938
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 940
struct _tuple20 _Tmp0=({struct _tuple20 _Tmp1;_Tmp1.f0=f1,_Tmp1.f1=f2;_Tmp1;});void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;struct Cyc_Dict_Dict _Tmp4;if(_Tmp0.f0.BottomFL.tag==1)
return f2;else{if(_Tmp0.f1.BottomFL.tag==1)
return f1;else{_Tmp4=_Tmp0.f0.ReachableFL.val.f0;_Tmp3=_Tmp0.f0.ReachableFL.val.f1;_Tmp2=_Tmp0.f1.ReachableFL.val.f0;_Tmp1=_Tmp0.f1.ReachableFL.val.f1;{struct Cyc_Dict_Dict d1=_Tmp4;struct Cyc_List_List*r1=_Tmp3;struct Cyc_Dict_Dict d2=_Tmp2;struct Cyc_List_List*r2=_Tmp1;
# 946
if(d1.t==d2.t && r1==r2)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct Cyc_CfFlowInfo_JoinEnv env=({struct Cyc_CfFlowInfo_JoinEnv _Tmp5;_Tmp5.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_Tmp6=({struct Cyc_CfFlowInfo_EscPile*_Tmp7=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_Tmp7->places=0;_Tmp7;});_Tmp5.pile=_Tmp6;}),_Tmp5.d1=d1,_Tmp5.d2=d2,_Tmp5.is_try_flow=0;_Tmp5;});
struct Cyc_Dict_Dict outdict=({struct Cyc_Dict_Dict(*_Tmp5)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;_Tmp5;})(Cyc_CfFlowInfo_join_absRval,& env,d1,d2);
struct Cyc_List_List*r=Cyc_Relations_join_relns(Cyc_Core_heap_region,r1,r2);
struct Cyc_Dict_Dict _Tmp5=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outdict);return Cyc_CfFlowInfo_ReachableFL(_Tmp5,r);}}}};}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f0;void*f1;};struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f0;union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 956
struct _tuple21 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple21 pr1,struct _tuple21 pr2){
# 961
void*_Tmp0;union Cyc_CfFlowInfo_FlowInfo _Tmp1;_Tmp1=pr1.f0;_Tmp0=pr1.f1;{union Cyc_CfFlowInfo_FlowInfo f1=_Tmp1;void*r1=_Tmp0;
void*_Tmp2;union Cyc_CfFlowInfo_FlowInfo _Tmp3;_Tmp3=pr2.f0;_Tmp2=pr2.f1;{union Cyc_CfFlowInfo_FlowInfo f2=_Tmp3;void*r2=_Tmp2;
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,f1,f2);
struct _tuple22 _Tmp4=({struct _tuple22 _Tmp5;_Tmp5.f0=f1,_Tmp5.f1=f2,_Tmp5.f2=outflow;_Tmp5;});void*_Tmp5;struct Cyc_Dict_Dict _Tmp6;struct Cyc_Dict_Dict _Tmp7;struct Cyc_Dict_Dict _Tmp8;if(_Tmp4.f0.BottomFL.tag==1){
struct _tuple21 _Tmp9;_Tmp9.f0=outflow,_Tmp9.f1=r2;return _Tmp9;}else{if(_Tmp4.f1.BottomFL.tag==1){
struct _tuple21 _Tmp9;_Tmp9.f0=outflow,_Tmp9.f1=r1;return _Tmp9;}else{if(_Tmp4.f2.ReachableFL.tag==2){_Tmp8=_Tmp4.f0.ReachableFL.val.f0;_Tmp7=_Tmp4.f1.ReachableFL.val.f0;_Tmp6=_Tmp4.f2.ReachableFL.val.f0;_Tmp5=_Tmp4.f2.ReachableFL.val.f1;{struct Cyc_Dict_Dict d1=_Tmp8;struct Cyc_Dict_Dict d2=_Tmp7;struct Cyc_Dict_Dict outd=_Tmp6;struct Cyc_List_List*relns=_Tmp5;
# 969
if(({int(*_Tmp9)(int,void*,void*)=(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;_Tmp9;})(0,r1,r2)){struct _tuple21 _Tmp9;_Tmp9.f0=outflow,_Tmp9.f1=r2;return _Tmp9;}
if(({int(*_Tmp9)(int,void*,void*)=(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;_Tmp9;})(0,r2,r1)){struct _tuple21 _Tmp9;_Tmp9.f0=outflow,_Tmp9.f1=r1;return _Tmp9;}{
struct Cyc_CfFlowInfo_JoinEnv env=({struct Cyc_CfFlowInfo_JoinEnv _Tmp9;_Tmp9.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_TmpA=({struct Cyc_CfFlowInfo_EscPile*_TmpB=_cycalloc(sizeof(struct Cyc_CfFlowInfo_EscPile));_TmpB->places=0;_TmpB;});_Tmp9.pile=_TmpA;}),_Tmp9.d1=d1,_Tmp9.d2=d2,_Tmp9.is_try_flow=0;_Tmp9;});
void*outr=({void*(*_Tmp9)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*)=(void*(*)(struct Cyc_CfFlowInfo_JoinEnv*,int,void*,void*))Cyc_CfFlowInfo_join_absRval;_Tmp9;})(& env,0,r1,r2);
struct _tuple21 _Tmp9;({union Cyc_CfFlowInfo_FlowInfo _TmpA=({struct Cyc_Dict_Dict _TmpB=Cyc_CfFlowInfo_escape_these(fenv,env.pile,outd);Cyc_CfFlowInfo_ReachableFL(_TmpB,relns);});_Tmp9.f0=_TmpA;}),_Tmp9.f1=outr;return _Tmp9;}}}else{
# 975
({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp9;})(_tag_fat("join_flow_and_rval: BottomFL outflow",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));}}};}}}
# 980
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1==r2)return 1;
# 983
{struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=r1,_Tmp1.f1=r2;_Tmp1;});struct _fat_ptr _Tmp1;int _Tmp2;int _Tmp3;struct _fat_ptr _Tmp4;int _Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;if(*((int*)_Tmp0.f0)==8){if(*((int*)_Tmp0.f1)==8){_TmpA=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;_Tmp8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*n1=_TmpA;void*r1=_Tmp9;struct Cyc_Absyn_Vardecl*n2=_Tmp8;void*r2=_Tmp7;
# 985
return n1==n2 && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{
goto _LL6;}}else{if(*((int*)_Tmp0.f1)==8){_LL6:
 return 0;}else{if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_TmpA=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{struct Cyc_CfFlowInfo_Place*p1=_TmpA;struct Cyc_CfFlowInfo_Place*p2=_Tmp9;
return Cyc_CfFlowInfo_place_cmp(p1,p2)==0;}}else{
goto _LLC;}}else{if(*((int*)_Tmp0.f1)==4){_LLC:
 return 0;}else{if(*((int*)_Tmp0.f0)==5){if(*((int*)_Tmp0.f1)==5){_TmpA=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1;{void*r1=_TmpA;void*r2=_Tmp9;
return Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,r1,r2);}}else{
goto _LL12;}}else{if(*((int*)_Tmp0.f1)==5){_LL12:
 return 0;}else{if(*((int*)_Tmp0.f0)==6)switch(*((int*)_Tmp0.f1)){case 6: _Tmp6=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1.is_union;_Tmp5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f1.fieldnum;_Tmp4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1.is_union;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f1.fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_Tmp0.f1)->f2;{int is_union1=_Tmp6;int fld1=_Tmp5;struct _fat_ptr d1=_Tmp4;int is_union2=_Tmp3;int fld2=_Tmp2;struct _fat_ptr d2=_Tmp1;
# 996
if(is_union1 && fld1!=fld2)return 0;
if((void**)d1.curr==(void**)d2.curr)return 1;
{int i=0;for(0;(unsigned)i < _get_fat_size(d1,sizeof(void*));++ i){
if(!({int(*_TmpB)(int,void*,void*)=(int(*)(int,void*,void*))Cyc_CfFlowInfo_absRval_lessthan_approx;_TmpB;})(0,((void**)d1.curr)[i],*((void**)_check_fat_subscript(d2,sizeof(void*),i))))return 0;}}
return 1;}case 0: goto _LL15;case 1: goto _LL17;default: goto _LL1D;}else{switch(*((int*)_Tmp0.f1)){case 0: _LL15:
 goto _LL18;case 1: _LL17: _LL18:
 return 0;default: if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3)
goto _LL0;else{
return 0;}}else{_LL1D:
# 1011
 goto _LL0;}}}}}}}}}_LL0:;}{
# 1013
struct _tuple19 _Tmp0=({struct _tuple19 _Tmp1;({enum Cyc_CfFlowInfo_InitLevel _Tmp2=Cyc_CfFlowInfo_initlevel_approx(r1);_Tmp1.f0=_Tmp2;}),({enum Cyc_CfFlowInfo_InitLevel _Tmp2=Cyc_CfFlowInfo_initlevel_approx(r2);_Tmp1.f1=_Tmp2;});_Tmp1;});if(_Tmp0.f0==Cyc_CfFlowInfo_AllIL)
goto _LL23;else{if(_Tmp0.f1==Cyc_CfFlowInfo_NoneIL){_LL23:
 return 1;}else{
return 0;}};}}
# 1026
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1028
struct _tuple20 _Tmp0=({struct _tuple20 _Tmp1;_Tmp1.f0=f1,_Tmp1.f1=f2;_Tmp1;});void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;struct Cyc_Dict_Dict _Tmp4;if(_Tmp0.f0.BottomFL.tag==1)
return 1;else{if(_Tmp0.f1.BottomFL.tag==1)
return 0;else{_Tmp4=_Tmp0.f0.ReachableFL.val.f0;_Tmp3=_Tmp0.f0.ReachableFL.val.f1;_Tmp2=_Tmp0.f1.ReachableFL.val.f0;_Tmp1=_Tmp0.f1.ReachableFL.val.f1;{struct Cyc_Dict_Dict d1=_Tmp4;struct Cyc_List_List*r1=_Tmp3;struct Cyc_Dict_Dict d2=_Tmp2;struct Cyc_List_List*r2=_Tmp1;
# 1032
if(d1.t==d2.t && r1==r2)return 1;
return Cyc_Dict_forall_intersect(Cyc_CfFlowInfo_absRval_lessthan_approx,d1,d2)&&
 Cyc_Relations_relns_approx(r1,r2);}}};}struct _tuple23{void*f0;struct Cyc_List_List*f1;};
# 1038
struct _tuple23 Cyc_CfFlowInfo_unname_rval(void*rv){
struct Cyc_List_List*names=0;
while(1){
{void*_Tmp0;void*_Tmp1;if(*((int*)rv)==8){_Tmp1=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rv)->f1;_Tmp0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rv)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;void*rv2=_Tmp0;
names=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=vd,_Tmp2->tl=names;_Tmp2;});rv=rv2;goto _LL0;}}else{
struct _tuple23 _Tmp2;_Tmp2.f0=rv,_Tmp2.f1=names;return _Tmp2;}_LL0:;}
# 1041
1U;}}
# 1047
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
if(il==Cyc_CfFlowInfo_NoneIL){
Cyc_fprintf(Cyc_stderr,_tag_fat("uninitialized",sizeof(char),14U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}else{
Cyc_fprintf(Cyc_stderr,_tag_fat("all-initialized",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 1054
void Cyc_CfFlowInfo_print_root(void*root){
void*_Tmp0;void*_Tmp1;switch(*((int*)root)){case 0: _Tmp1=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)root)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1057
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat("Root(%s)",sizeof(char),9U),_tag_fat(_Tmp3,sizeof(void*),1));});goto _LL0;}case 1: _Tmp1=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)root)->f1;_Tmp0=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)root)->f2;{struct Cyc_Absyn_Exp*e=_Tmp1;void*t=_Tmp0;
# 1059
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({
struct _fat_ptr _Tmp4=(struct _fat_ptr)Cyc_Absynpp_exp2string(e);_Tmp3.f1=_Tmp4;});_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_Absynpp_typ2string(t);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_fprintf(Cyc_stderr,_tag_fat("MallocPt(%s,%s)",sizeof(char),16U),_tag_fat(_Tmp4,sizeof(void*),2));});goto _LL0;}default:
# 1062
 Cyc_fprintf(Cyc_stderr,_tag_fat("InitParam(_,_)",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 1066
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){
{struct Cyc_List_List*x=p;for(0;x!=0;x=_check_null(x)->tl){
void*_Tmp0=(void*)x->hd;int _Tmp1;if(*((int*)_Tmp0)==0){_Tmp1=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp0)->f1;{int i=_Tmp1;
# 1070
({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)i;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,_tag_fat(".%d",sizeof(char),4U),_tag_fat(_Tmp3,sizeof(void*),1));});
goto _LL0;}}else{
# 1073
if(x->tl!=0){
void*_Tmp2=(void*)x->tl->hd;int _Tmp3;if(*((int*)_Tmp2)==0){_Tmp3=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_Tmp2)->f1;{int i=_Tmp3;
# 1076
x=x->tl;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("->%d",sizeof(char),5U),_tag_fat(_Tmp5,sizeof(void*),1));});
continue;}}else{
goto _LL5;}_LL5:;}
# 1082
Cyc_fprintf(Cyc_stderr,_tag_fat("*",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}}
# 1084
Cyc_fprintf(Cyc_stderr,_tag_fat(" ",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 1087
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);
Cyc_CfFlowInfo_print_path(p->path);}
# 1092
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int first=1;
while(x!=0){
if(!first){Cyc_fprintf(Cyc_stderr,_tag_fat(", ",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));first=0;}
pr(x->hd);
x=x->tl;
# 1095
1U;}
# 1099
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 1102
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_Tmp0;struct _fat_ptr _Tmp1;int _Tmp2;int _Tmp3;void*_Tmp4;enum Cyc_CfFlowInfo_InitLevel _Tmp5;switch(*((int*)rval)){case 0:
 Cyc_fprintf(Cyc_stderr,_tag_fat("Zero",sizeof(char),5U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 1:
 Cyc_fprintf(Cyc_stderr,_tag_fat("NotZeroAll",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 2: _Tmp5=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)rval)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp5;
Cyc_fprintf(Cyc_stderr,_tag_fat("Unknown(",sizeof(char),9U),_tag_fat(0U,sizeof(void*),0));Cyc_CfFlowInfo_print_initlevel(il);
Cyc_fprintf(Cyc_stderr,_tag_fat(")",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 3: _Tmp5=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)rval)->f1;{enum Cyc_CfFlowInfo_InitLevel il=_Tmp5;
Cyc_fprintf(Cyc_stderr,_tag_fat("Esc(",sizeof(char),5U),_tag_fat(0U,sizeof(void*),0));Cyc_CfFlowInfo_print_initlevel(il);
Cyc_fprintf(Cyc_stderr,_tag_fat(")",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 4: _Tmp4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)rval)->f1;{struct Cyc_CfFlowInfo_Place*p=_Tmp4;
Cyc_fprintf(Cyc_stderr,_tag_fat("AddrOf(",sizeof(char),8U),_tag_fat(0U,sizeof(void*),0));Cyc_CfFlowInfo_print_place(p);
Cyc_fprintf(Cyc_stderr,_tag_fat(")",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 5: _Tmp4=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)rval)->f1;{void*r=_Tmp4;
Cyc_fprintf(Cyc_stderr,_tag_fat("UniquePtr(",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));Cyc_CfFlowInfo_print_absrval(r);
Cyc_fprintf(Cyc_stderr,_tag_fat(")",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}case 6: _Tmp3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)rval)->f1.is_union;_Tmp2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)rval)->f1.fieldnum;_Tmp1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)rval)->f2;{int is_union=_Tmp3;int f=_Tmp2;struct _fat_ptr d=_Tmp1;
# 1115
if(is_union){
Cyc_fprintf(Cyc_stderr,_tag_fat("AggrUnion{",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));
if(f!=-1)
({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)f;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("tag == %d;",sizeof(char),11U),_tag_fat(_Tmp7,sizeof(void*),1));});}else{
# 1120
Cyc_fprintf(Cyc_stderr,_tag_fat("AggrStruct{",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));}
{int i=0;for(0;(unsigned)i < _get_fat_size(d,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)d.curr)[i]);
if((unsigned)(i + 1)< _get_fat_size(d,sizeof(void*)))Cyc_fprintf(Cyc_stderr,_tag_fat(",",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}}
# 1125
Cyc_fprintf(Cyc_stderr,_tag_fat("}",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}case 7: _Tmp4=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)rval)->f1;_Tmp3=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)rval)->f2;_Tmp0=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)rval)->f3;_Tmp2=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)rval)->f4;{struct Cyc_Absyn_Exp*e=_Tmp4;int i=_Tmp3;void*r=_Tmp0;int l=_Tmp2;
# 1128
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_exp2string(e);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_fprintf(Cyc_stderr,_tag_fat("[Consumed(%s,%d,",sizeof(char),17U),_tag_fat(_Tmp8,sizeof(void*),2));});
Cyc_CfFlowInfo_print_absrval(r);({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,l?_Tmp7.f1=_tag_fat("local",sizeof(char),6U):(_Tmp7.f1=_tag_fat("",sizeof(char),1U));_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat(",%s)]",sizeof(char),6U),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL0;}default: _Tmp4=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rval)->f1;_Tmp0=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)rval)->f2;{struct Cyc_Absyn_Vardecl*n=_Tmp4;void*r=_Tmp0;
# 1132
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_qvar2string(n->name);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("[NamedLocation(%s,",sizeof(char),19U),_tag_fat(_Tmp7,sizeof(void*),1));});
Cyc_CfFlowInfo_print_absrval(r);Cyc_fprintf(Cyc_stderr,_tag_fat(")]",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}_LL0:;}
# 1138
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
Cyc_fprintf(Cyc_stderr,_tag_fat("    ",sizeof(char),5U),_tag_fat(0U,sizeof(void*),0));
Cyc_CfFlowInfo_print_root(root);
Cyc_fprintf(Cyc_stderr,_tag_fat(" --> ",sizeof(char),6U),_tag_fat(0U,sizeof(void*),0));
Cyc_CfFlowInfo_print_absrval(rval);
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 1146
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
Cyc_Dict_iter(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1150
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
void*_Tmp0;struct Cyc_Dict_Dict _Tmp1;if(f.BottomFL.tag==1){
Cyc_fprintf(Cyc_stderr,_tag_fat("  BottomFL\n",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}else{_Tmp1=f.ReachableFL.val.f0;_Tmp0=f.ReachableFL.val.f1;{struct Cyc_Dict_Dict fd=_Tmp1;struct Cyc_List_List*rlns=_Tmp0;
# 1154
Cyc_fprintf(Cyc_stderr,_tag_fat("  ReachableFL:\n",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));
Cyc_CfFlowInfo_print_flowdict(fd);
Cyc_fprintf(Cyc_stderr,_tag_fat("\n  Relations: ",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
Cyc_Relations_print_relns(Cyc_stderr,rlns);
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}_LL0:;}

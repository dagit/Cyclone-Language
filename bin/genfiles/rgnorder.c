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
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);struct _tuple0{void*f0;void*f1;};
# 229 "dict.h"
extern struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 283
extern struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 176 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 182
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U,Cyc_Absyn_AqualKind =8U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 869 "absyn.h"
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 886
void*Cyc_Absyn_compress(void*);
# 904
extern void*Cyc_Absyn_uint_type;
# 913
extern void*Cyc_Absyn_heap_rgn_type;
# 917
extern void*Cyc_Absyn_empty_effect;
# 921
extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);struct Cyc_RgnOrder_RgnPO;
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 98 "tcutil.h"
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 199
void*Cyc_Tcutil_normalize_effect(void*);
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_RgnOrder_RgnPO{struct Cyc_Dict_Dict d;void*these_outlive_heap;void*these_outlive_unique;struct Cyc_Absyn_Tvar*youngest;void*opened_regions;};
# 67 "rgnorder.cyc"
static int Cyc_RgnOrder_valid_constraint(void*eff,void*rgn){
struct Cyc_Absyn_Kind*_Tmp0=Cyc_Tcutil_type_kind(rgn);enum Cyc_Absyn_KindQual _Tmp1;_Tmp1=_Tmp0->kind;{enum Cyc_Absyn_KindQual k=_Tmp1;
if((int)k!=3)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("bad region type |",sizeof(char),18U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)rgn;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("| passed to add_outlives_constraint",sizeof(char),36U);_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),3));});{
void*_Tmp2=Cyc_Absyn_compress(eff);void*_Tmp3;switch(*((int*)_Tmp2)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)){case 10: _Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2;{struct Cyc_List_List*es=_Tmp3;
# 73
for(1;es!=0;es=es->tl){
if(!Cyc_RgnOrder_valid_constraint((void*)es->hd,rgn))return 0;}
return 1;}case 11: _LLB:
# 81
 return 1;case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2!=0){_Tmp3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f2->hd;{void*r=_Tmp3;
# 83
struct Cyc_Absyn_Kind*_Tmp4=Cyc_Tcutil_type_kind(r);enum Cyc_Absyn_KindQual _Tmp5;_Tmp5=_Tmp4->kind;{enum Cyc_Absyn_KindQual rk=_Tmp5;
return(int)rk==3;}}}else{goto _LLE;}default: goto _LLE;}case 1:
# 77
 goto _LL9;case 2: _LL9:
 goto _LLB;default: _LLE:
# 86
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("bad effect |",sizeof(char),13U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)eff;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("| passed to add_outlives_constraint",sizeof(char),36U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp7,sizeof(void*),3));});};}}}
# 93
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned loc){
# 95
eff=Cyc_Tcutil_normalize_effect(eff);{
struct Cyc_RgnOrder_RgnPO*ans;ans=_cycalloc(sizeof(struct Cyc_RgnOrder_RgnPO)),*ans=*po;
# 98
if(!Cyc_RgnOrder_valid_constraint(eff,rgn)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Invalid region ordering constraint; kind mismatch",sizeof(char),50U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return ans;}{
# 103
void*_Tmp0=Cyc_Absyn_compress(rgn);void*_Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
# 105
struct Cyc_Dict_Dict d=po->d;
void*new_eff=
({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member;})(d,tv)?Cyc_Absyn_join_eff(({void*_Tmp2[2];_Tmp2[0]=eff,({void*_Tmp3=({(void*(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_lookup;})(d,tv);_Tmp2[1]=_Tmp3;});Cyc_List_list(_tag_fat(_Tmp2,sizeof(void*),2));})): eff;
d=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert;})(d,tv,new_eff);
ans->d=d;
return ans;}case 1:
# 112
 Cyc_Unify_unify(rgn,Cyc_Absyn_heap_rgn_type);
goto _LL6;case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==6){_LL6:
# 115
({void*_Tmp2=Cyc_Absyn_join_eff(({void*_Tmp3[2];_Tmp3[0]=eff,_Tmp3[1]=po->these_outlive_heap;Cyc_List_list(_tag_fat(_Tmp3,sizeof(void*),2));}));ans->these_outlive_heap=_Tmp2;});
return ans;}else{goto _LL7;}default: _LL7:
# 124
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("RgnOrder::add_outlives_constraint passed a bad region",sizeof(char),54U),_tag_fat(0U,sizeof(void*),0));};}}}
# 128
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened){
# 130
if(!opened &&({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member;})(po->d,rgn))
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("RgnOrder::add_youngest: repeated region",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_RgnOrder_RgnPO*ans;ans=_cycalloc(sizeof(struct Cyc_RgnOrder_RgnPO)),*ans=*po;
if(opened){
({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert;})(po->d,rgn,Cyc_Absyn_empty_effect);ans->d=_Tmp0;});
({void*_Tmp0=Cyc_Absyn_join_eff(({void*_Tmp1[2];({void*_Tmp2=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(rgn));_Tmp1[0]=_Tmp2;}),_Tmp1[1]=ans->opened_regions;Cyc_List_list(_tag_fat(_Tmp1,sizeof(void*),2));}));ans->opened_regions=_Tmp0;});}else{
# 137
void*eff=Cyc_Absyn_join_eff(({void*_Tmp0[2];({void*_Tmp1=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(po->youngest));_Tmp0[0]=_Tmp1;}),_Tmp0[1]=ans->opened_regions;Cyc_List_list(_tag_fat(_Tmp0,sizeof(void*),2));}));
# 139
({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert;})(po->d,rgn,eff);ans->d=_Tmp0;});
ans->youngest=rgn;}
# 142
return ans;}}
# 144
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn){
if(({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member;})(po->d,rgn))
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("RgnOrder::add_unordered: repeated region",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_RgnOrder_RgnPO*ans;ans=_cycalloc(sizeof(struct Cyc_RgnOrder_RgnPO)),*ans=*po;
({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert;})(ans->d,rgn,Cyc_Absyn_empty_effect);ans->d=_Tmp0;});
return ans;}}
# 152
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned loc){
# 155
struct Cyc_Dict_Dict d=({(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*)))Cyc_Dict_empty;})(Cyc_Absyn_tvar_cmp);
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
if((int)Cyc_Kinds_tvar_kind((struct Cyc_Absyn_Tvar*)tvs2->hd,& Cyc_Kinds_bk)->kind==3)
d=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert;})(d,(struct Cyc_Absyn_Tvar*)tvs2->hd,Cyc_Absyn_empty_effect);}}
# 160
if(!({void*_Tmp0=effect;Cyc_RgnOrder_valid_constraint(_Tmp0,Cyc_Absyn_var_type(fst_rgn));})){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Invalid region ordering constraint; kind mismatch",sizeof(char),50U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return({struct Cyc_RgnOrder_RgnPO*_Tmp0=_cycalloc(sizeof(struct Cyc_RgnOrder_RgnPO));_Tmp0->d=d,_Tmp0->these_outlive_heap=Cyc_Absyn_empty_effect,_Tmp0->these_outlive_unique=Cyc_Absyn_empty_effect,_Tmp0->youngest=fst_rgn,_Tmp0->opened_regions=Cyc_Absyn_empty_effect;_Tmp0;});}
# 164
d=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*,void*))Cyc_Dict_insert;})(d,fst_rgn,effect);{
# 166
struct Cyc_RgnOrder_RgnPO*ans;ans=_cycalloc(sizeof(struct Cyc_RgnOrder_RgnPO)),ans->d=d,ans->these_outlive_heap=Cyc_Absyn_empty_effect,ans->these_outlive_unique=Cyc_Absyn_empty_effect,ans->youngest=fst_rgn,ans->opened_regions=Cyc_Absyn_empty_effect;
for(1;po!=0;po=po->tl){
ans=Cyc_RgnOrder_add_outlives_constraint(ans,(*((struct _tuple0*)po->hd)).f0,(*((struct _tuple0*)po->hd)).f1,loc);}
return ans;}}
# 194 "rgnorder.cyc"
static int Cyc_RgnOrder_contains_rgnseff(struct Cyc_Absyn_Tvar*rgns_of_var,void*eff){
void*_Tmp0=Cyc_Tcutil_normalize_effect(Cyc_Absyn_compress(eff));void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp1;
# 197
void*_Tmp2=Cyc_Absyn_compress(t);void*_Tmp3;if(*((int*)_Tmp2)==2){_Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp3;
return Cyc_Absyn_tvar_cmp(tv,rgns_of_var)==0;}}else{
return 0;};}}else{goto _LL5;}case 10: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*effs=_Tmp1;
# 202
for(1;effs!=0;effs=effs->tl){
if(Cyc_RgnOrder_contains_rgnseff(rgns_of_var,(void*)effs->hd))
return 1;}
return 0;}default: goto _LL5;}else{_LL5:
 return 0;};}struct Cyc_RgnOrder_OutlivesEnv{struct _RegionHandle*r;struct Cyc_List_List*seen;struct Cyc_List_List*todo;};
# 216
static void Cyc_RgnOrder_add_to_search(struct Cyc_RgnOrder_OutlivesEnv*env,void*eff){
void*_Tmp0=Cyc_Absyn_compress(eff);void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->hd)==2){_Tmp1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp1;
# 219
{struct Cyc_List_List*seen=env->seen;for(0;seen!=0;seen=seen->tl){
if(Cyc_Absyn_tvar_cmp(tv,(struct Cyc_Absyn_Tvar*)seen->hd)==0)
return;}}
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(env->r,0U,sizeof(struct Cyc_List_List));_Tmp3->hd=tv,_Tmp3->tl=env->seen;_Tmp3;});env->seen=_Tmp2;});
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(env->r,0U,sizeof(struct Cyc_List_List));_Tmp3->hd=tv,_Tmp3->tl=env->todo;_Tmp3;});env->todo=_Tmp2;});
return;}}else{goto _LL5;}}else{goto _LL5;}case 10: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*effs=_Tmp1;
# 226
for(1;effs!=0;effs=effs->tl){
Cyc_RgnOrder_add_to_search(env,(void*)effs->hd);}
return;}default: goto _LL5;}else{_LL5:
 return;};}
# 233
static struct Cyc_RgnOrder_OutlivesEnv Cyc_RgnOrder_initial_env(struct _RegionHandle*listrgn,struct Cyc_RgnOrder_RgnPO*po,void*rgn){
# 235
struct Cyc_RgnOrder_OutlivesEnv ans=({struct Cyc_RgnOrder_OutlivesEnv _Tmp0;_Tmp0.r=listrgn,_Tmp0.seen=0,_Tmp0.todo=0;_Tmp0;});
void*r=Cyc_Absyn_compress(rgn);
struct Cyc_Absyn_Kind*_Tmp0=Cyc_Tcutil_type_kind(r);enum Cyc_Absyn_AliasHint _Tmp1;enum Cyc_Absyn_KindQual _Tmp2;_Tmp2=_Tmp0->kind;_Tmp1=_Tmp0->aliashint;{enum Cyc_Absyn_KindQual k=_Tmp2;enum Cyc_Absyn_AliasHint a=_Tmp1;
if((int)k!=3)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("RgnOrder: rgn not of correct kind",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));
Cyc_RgnOrder_add_to_search(& ans,po->these_outlive_unique);
Cyc_RgnOrder_add_to_search(& ans,po->these_outlive_heap);{
# 256 "rgnorder.cyc"
void*_Tmp3;if(*((int*)r)==2){_Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)r)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp3;
# 258
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(listrgn,0U,sizeof(struct Cyc_List_List));_Tmp5->hd=tv,_Tmp5->tl=ans.seen;_Tmp5;});ans.seen=_Tmp4;});
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_region_malloc(listrgn,0U,sizeof(struct Cyc_List_List));_Tmp5->hd=tv,_Tmp5->tl=ans.todo;_Tmp5;});ans.todo=_Tmp4;});
goto _LL7;}}else{_LL7:
 return ans;};}}}
# 267
static int Cyc_RgnOrder_atomic_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
struct _RegionHandle _Tmp0=_new_region(0U,"listrgn");struct _RegionHandle*listrgn=& _Tmp0;_push_region(listrgn);
{void*_Tmp1=Cyc_Absyn_compress(eff);void*_Tmp2;if(*((int*)_Tmp1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)){case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2!=0){_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2->hd;{void*vt=_Tmp2;
# 272
void*_Tmp3=Cyc_Absyn_compress(vt);void*_Tmp4;switch(*((int*)_Tmp3)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==6){
# 284 "rgnorder.cyc"
struct Cyc_Absyn_Kind*_Tmp5=Cyc_Tcutil_type_kind(Cyc_Absyn_compress(rgn));enum Cyc_Absyn_KindQual _Tmp6;_Tmp6=_Tmp5->kind;{enum Cyc_Absyn_KindQual k=_Tmp6;
int _Tmp7=(int)k==3;_npop_handler(0);return _Tmp7;}}else{goto _LLC;}case 2: _Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp4;
# 287
if(po==0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("RgnOrder: tvar without a partial-order",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));{
# 299 "rgnorder.cyc"
struct Cyc_RgnOrder_OutlivesEnv env=Cyc_RgnOrder_initial_env(listrgn,po,rgn);
while(env.todo!=0){
struct Cyc_Absyn_Tvar*next=(struct Cyc_Absyn_Tvar*)_check_null(env.todo)->hd;
env.todo=_check_null(env.todo)->tl;
if(Cyc_Absyn_tvar_cmp(next,tv)==0){
int _Tmp5=1;_npop_handler(0);return _Tmp5;}
# 309
if(({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member;})(po->d,next))
Cyc_RgnOrder_add_to_search(& env,({(void*(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_lookup;})(po->d,next));else{
# 312
int _Tmp5=0;_npop_handler(0);return _Tmp5;}}{
# 315
int _Tmp5=0;_npop_handler(0);return _Tmp5;}}}default: _LLC: {
int _Tmp5=0;_npop_handler(0);return _Tmp5;}};}}else{goto _LL5;}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2!=0){_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f2->hd;{void*vt=_Tmp2;
# 319
void*_Tmp3=Cyc_Absyn_compress(vt);void*_Tmp4;if(*((int*)_Tmp3)==2){_Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Tvar*tv=_Tmp4;
# 322
if(po==0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("RgnOrder: tvar without a partial-order",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));{
# 327
struct Cyc_RgnOrder_OutlivesEnv env=Cyc_RgnOrder_initial_env(listrgn,po,rgn);
struct Cyc_Absyn_Kind*_Tmp5=Cyc_Tcutil_type_kind(rgn);enum Cyc_Absyn_AliasHint _Tmp6;enum Cyc_Absyn_KindQual _Tmp7;_Tmp7=_Tmp5->kind;_Tmp6=_Tmp5->aliashint;{enum Cyc_Absyn_KindQual k=_Tmp7;enum Cyc_Absyn_AliasHint a=_Tmp6;
# 330
if((int)k==3){
# 332
if(Cyc_RgnOrder_contains_rgnseff(tv,po->these_outlive_heap)||
 Cyc_RgnOrder_contains_rgnseff(tv,po->these_outlive_unique)){
int _Tmp8=1;_npop_handler(0);return _Tmp8;}}
# 340
while(env.todo!=0){
struct Cyc_Absyn_Tvar*next=(struct Cyc_Absyn_Tvar*)_check_null(env.todo)->hd;
env.todo=_check_null(env.todo)->tl;
if(!({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_member;})(po->d,next))
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("RgnOrder: type variable not in dict",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));{
void*next_eff=({(void*(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Tvar*))Cyc_Dict_lookup;})(po->d,next);
if(Cyc_RgnOrder_contains_rgnseff(tv,next_eff)){
int _Tmp8=1;_npop_handler(0);return _Tmp8;}
Cyc_RgnOrder_add_to_search(& env,next_eff);}}{
# 350
int _Tmp8=0;_npop_handler(0);return _Tmp8;}}}}}else{
int _Tmp5=0;_npop_handler(0);return _Tmp5;};}}else{goto _LL5;}default: goto _LL5;}else{_LL5: {
# 353
int _Tmp3=0;_npop_handler(0);return _Tmp3;}};}
# 269 "rgnorder.cyc"
;_pop_region();}
# 359 "rgnorder.cyc"
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn){
eff=Cyc_Tcutil_normalize_effect(eff);{
void*_Tmp0=Cyc_Absyn_compress(eff);void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==10){_Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*effs=_Tmp1;
# 363
for(1;effs!=0;effs=effs->tl){
if(!Cyc_RgnOrder_effect_outlives(po,(void*)effs->hd,rgn))
return 0;}
return 1;}}else{goto _LL3;}}else{_LL3:
 return Cyc_RgnOrder_atomic_effect_outlives(po,eff,rgn);};}}
# 371
static void Cyc_RgnOrder_pin_effect(void*eff,void*bd){
eff=Cyc_Tcutil_normalize_effect(eff);{
void*_Tmp0=Cyc_Absyn_compress(eff);void*_Tmp1;switch(*((int*)_Tmp0)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 10: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*effs=_Tmp1;
# 375
for(1;effs!=0;effs=effs->tl){
Cyc_RgnOrder_pin_effect((void*)effs->hd,bd);}
return;}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*t=_Tmp1;
# 379
void*_Tmp2=Cyc_Absyn_compress(t);if(*((int*)_Tmp2)==1){
Cyc_Unify_unify(t,Cyc_Absyn_uint_type);return;}else{
return;};}}else{goto _LL9;}case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*r=_Tmp1;
# 384
void*_Tmp2=Cyc_Absyn_compress(r);if(*((int*)_Tmp2)==1){
Cyc_Unify_unify(r,bd);return;}else{
return;};}}else{goto _LL9;}default: goto _LL9;}case 1:
# 388
 Cyc_Unify_unify(eff,Cyc_Absyn_empty_effect);return;default: _LL9:
 return;};}}
# 395
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*po,void*rgn1,void*rgn2){
return({struct Cyc_RgnOrder_RgnPO*_Tmp0=po;void*_Tmp1=Cyc_Absyn_access_eff(rgn1);Cyc_RgnOrder_effect_outlives(_Tmp0,_Tmp1,rgn2);});}
# 399
static int Cyc_RgnOrder_eff_outlives_atomic_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
# 401
eff2=Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff2));{
void*_Tmp0;if(*((int*)eff2)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff2)->f1)){case 10: _Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff2)->f2;{struct Cyc_List_List*effs=_Tmp0;
# 404
for(1;effs!=0;effs=effs->tl){
if(Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,(void*)effs->hd))
return 1;}
return 0;}case 9: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff2)->f2!=0){_Tmp0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff2)->f2->hd;{void*vt=_Tmp0;
return Cyc_RgnOrder_effect_outlives(po,eff1,vt);}}else{goto _LL7;}case 11: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff2)->f2!=0){_Tmp0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff2)->f2->hd;{void*vt1=_Tmp0;
# 410
void*_Tmp1;if(*((int*)eff1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff1)->f1)==11){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff1)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff1)->f2->hd;{void*vt2=_Tmp1;
# 412
{struct _tuple0 _Tmp2=({struct _tuple0 _Tmp3;({void*_Tmp4=Cyc_Absyn_compress(vt1);_Tmp3.f0=_Tmp4;}),({void*_Tmp4=Cyc_Absyn_compress(vt2);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp2.f0)==2){if(*((int*)_Tmp2.f1)==2){_Tmp4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp2.f0)->f1;_Tmp3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp2.f1)->f1;{struct Cyc_Absyn_Tvar*tv1=_Tmp4;struct Cyc_Absyn_Tvar*tv2=_Tmp3;
return Cyc_Absyn_tvar_cmp(tv1,tv2)==0;}}else{goto _LL11;}}else{_LL11:
 goto _LLE;}_LLE:;}
# 416
goto _LLD;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _LLD:
 return eff1==Cyc_Absyn_heap_rgn_type;};}}else{goto _LL7;}default: goto _LL7;}else{_LL7:
# 419
 return eff1==Cyc_Absyn_heap_rgn_type;};}}
# 423
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2){
eff1=Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff1));{
void*_Tmp0;if(*((int*)eff1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff1)->f1)==10){_Tmp0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)eff1)->f2;{struct Cyc_List_List*effs=_Tmp0;
# 427
for(1;effs!=0;effs=effs->tl){
if(!Cyc_RgnOrder_eff_outlives_eff(po,(void*)effs->hd,eff2))
return 0;}
return 1;}}else{goto _LL3;}}else{_LL3:
 return Cyc_RgnOrder_eff_outlives_atomic_eff(po,eff1,eff2);};}}
# 445 "rgnorder.cyc"
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin){
# 449
{struct Cyc_List_List*cs=constraints;for(0;cs!=0;cs=cs->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)cs->hd;void*_Tmp1;_Tmp1=_Tmp0->f1;{void*bd=_Tmp1;
void*_Tmp2=Cyc_Absyn_compress(bd);if(*((int*)_Tmp2)==1){
# 453
if(do_pin)
Cyc_Unify_unify(bd,default_bound);
goto _LL3;}else{
goto _LL3;}_LL3:;}}}
# 459
{struct Cyc_List_List*cs=constraints;for(0;cs!=0;cs=cs->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)cs->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*eff=_Tmp2;void*bd=_Tmp1;
if(do_pin)
Cyc_RgnOrder_pin_effect(eff,bd);
if(!Cyc_RgnOrder_effect_outlives(po,eff,bd))
return 0;}}}
# 466
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 470
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po){
struct Cyc_Iter_Iter iter=Cyc_Dict_make_iter(Cyc_Core_heap_region,po->d);
struct _tuple12 elem=*({(struct _tuple12*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rchoose;})(Cyc_Core_heap_region,po->d);
Cyc_fprintf(Cyc_stderr,_tag_fat("region po:\n",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));
while(({(int(*)(struct Cyc_Iter_Iter,struct _tuple12*))Cyc_Iter_next;})(iter,& elem)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)((struct _fat_ptr)*elem.f0->name);_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({
struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(elem.f1));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("  %s outlived by %s\n",sizeof(char),21U),_tag_fat(_Tmp2,sizeof(void*),2));});}
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(po->these_outlive_heap));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("  these outlive heap: %s\n",sizeof(char),26U),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(po->these_outlive_unique));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("  these outlive unique: %s\n",sizeof(char),28U),_tag_fat(_Tmp1,sizeof(void*),1));});}

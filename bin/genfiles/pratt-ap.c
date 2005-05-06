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
 struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 139 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
# 7 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_zero;
# 13
extern char*Cyc_AP_tostr(struct Cyc_AP_T*,int);
# 16
extern struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 32
extern int Cyc_AP_cmp(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned val;};struct _union_Node_NParamNumelts{int tag;unsigned val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 61 "pratt-ap.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;struct Cyc_Pratt_Graph;
# 85 "pratt-ap.cyc"
union Cyc_Pratt_Node Cyc_Pratt_zero_node={.NZero={1,0}};
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*vd){return({union Cyc_Pratt_Node _Tmp0;_Tmp0.NVar.tag=2U,_Tmp0.NVar.val=vd;_Tmp0;});}
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*vd){return({union Cyc_Pratt_Node _Tmp0;_Tmp0.NNumelts.tag=3U,_Tmp0.NNumelts.val=vd;_Tmp0;});}
union Cyc_Pratt_Node Cyc_Pratt_NType(void*t){return({union Cyc_Pratt_Node _Tmp0;_Tmp0.NType.tag=4U,_Tmp0.NType.val=t;_Tmp0;});}
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned i){return({union Cyc_Pratt_Node _Tmp0;_Tmp0.NParam.tag=5U,_Tmp0.NParam.val=i;_Tmp0;});}
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned i){return({union Cyc_Pratt_Node _Tmp0;_Tmp0.NParamNumelts.tag=6U,_Tmp0.NParamNumelts.val=i;_Tmp0;});}
union Cyc_Pratt_Node Cyc_Pratt_NReturn (void){return({union Cyc_Pratt_Node _Tmp0;_Tmp0.NReturn.tag=7U,_Tmp0.NReturn.val=0;_Tmp0;});}struct _tuple12{union Cyc_Pratt_Node f0;union Cyc_Pratt_Node f1;};
# 93
static int Cyc_Pratt_cmp_node(union Cyc_Pratt_Node n1,union Cyc_Pratt_Node n2){
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=n1,_Tmp1.f1=n2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;if(_Tmp0.f0.NZero.tag==1){if(_Tmp0.f1.NZero.tag==1)
return 0;else{
return -1;}}else{if(_Tmp0.f1.NZero.tag==1)
return 1;else{if(_Tmp0.f0.NReturn.tag==7){if(_Tmp0.f1.NReturn.tag==7)
return 0;else{
return -1;}}else{if(_Tmp0.f1.NReturn.tag==7)
return 1;else{if(_Tmp0.f0.NVar.tag==2){if(_Tmp0.f1.NVar.tag==2){_Tmp4=_Tmp0.f0.NVar.val;_Tmp3=_Tmp0.f1.NVar.val;{struct Cyc_Absyn_Vardecl*x=_Tmp4;struct Cyc_Absyn_Vardecl*y=_Tmp3;
return(int)x - (int)y;}}else{
return -1;}}else{if(_Tmp0.f1.NVar.tag==2)
return 1;else{if(_Tmp0.f0.NNumelts.tag==3){if(_Tmp0.f1.NNumelts.tag==3){_Tmp4=_Tmp0.f0.NNumelts.val;_Tmp3=_Tmp0.f1.NNumelts.val;{struct Cyc_Absyn_Vardecl*x=_Tmp4;struct Cyc_Absyn_Vardecl*y=_Tmp3;
return(int)x - (int)y;}}else{_Tmp4=_Tmp0.f0.NNumelts.val;{struct Cyc_Absyn_Vardecl*x=_Tmp4;
return -1;}}}else{if(_Tmp0.f1.NNumelts.tag==3){_Tmp4=_Tmp0.f1.NNumelts.val;{struct Cyc_Absyn_Vardecl*y=_Tmp4;
return 1;}}else{if(_Tmp0.f0.NType.tag==4){if(_Tmp0.f1.NType.tag==4){_Tmp4=_Tmp0.f0.NType.val;_Tmp3=_Tmp0.f1.NType.val;{void*x=_Tmp4;void*y=_Tmp3;
# 108
if(Cyc_Unify_unify(x,y))return 0;
return Cyc_Tcutil_typecmp(x,y);}}else{
return -1;}}else{if(_Tmp0.f1.NType.tag==4)
return 1;else{if(_Tmp0.f0.NParam.tag==5){if(_Tmp0.f1.NParam.tag==5){_Tmp2=_Tmp0.f0.NParam.val;_Tmp1=_Tmp0.f1.NParam.val;{unsigned i=_Tmp2;unsigned j=_Tmp1;
return(int)i - (int)j;}}else{_Tmp2=_Tmp0.f0.NParam.val;{unsigned i=_Tmp2;
return -1;}}}else{if(_Tmp0.f1.NParam.tag==5){_Tmp2=_Tmp0.f1.NParam.val;{unsigned j=_Tmp2;
return 1;}}else{_Tmp2=_Tmp0.f0.NParamNumelts.val;_Tmp1=_Tmp0.f1.NParamNumelts.val;{unsigned i=_Tmp2;unsigned j=_Tmp1;
return(int)i - (int)j;}}}}}}}}}}}}};}
# 119
struct _fat_ptr Cyc_Pratt_node2string(union Cyc_Pratt_Node n){
unsigned _Tmp0;void*_Tmp1;switch(n.NParamNumelts.tag){case 1:
 return _tag_fat("0",sizeof(char),2U);case 2: _Tmp1=n.NVar.val;{struct Cyc_Absyn_Vardecl*x=_Tmp1;
return Cyc_Absynpp_qvar2string(x->name);}case 4: _Tmp1=n.NType.val;{void*x=_Tmp1;
return Cyc_Absynpp_typ2string(x);}case 3: _Tmp1=n.NNumelts.val;{struct Cyc_Absyn_Vardecl*x=_Tmp1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(x->name));_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("numelts(%s)",sizeof(char),12U),_tag_fat(_Tmp3,sizeof(void*),1));});}case 5: _Tmp0=n.NParam.val;{unsigned i=_Tmp0;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)((int)i);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("param(%d)",sizeof(char),10U),_tag_fat(_Tmp3,sizeof(void*),1));});}case 6: _Tmp0=n.NParamNumelts.val;{unsigned i=_Tmp0;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)((int)i);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("numelts(param(%d))",sizeof(char),19U),_tag_fat(_Tmp3,sizeof(void*),1));});}default:
 return Cyc_aprintf(_tag_fat("return_value",sizeof(char),13U),_tag_fat(0U,sizeof(void*),0));};}struct Cyc_Pratt_Edge{union Cyc_Pratt_Node x;union Cyc_Pratt_Node y;struct Cyc_AP_T*value;};struct Cyc_Pratt_Graph{struct Cyc_List_List*nodes;struct Cyc_List_List*edges;};
# 149
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph (void){
struct Cyc_List_List*nodes;nodes=_cycalloc(sizeof(struct Cyc_List_List)),nodes->hd=& Cyc_Pratt_zero_node,nodes->tl=0;
return({struct Cyc_Pratt_Graph*_Tmp0=_cycalloc(sizeof(struct Cyc_Pratt_Graph));_Tmp0->nodes=nodes,_Tmp0->edges=0;_Tmp0;});}
# 155
static struct Cyc_List_List*Cyc_Pratt_copy_edges(struct Cyc_List_List*ds){
struct Cyc_List_List*res=0;
for(1;ds!=0;ds=ds->tl){
struct Cyc_Pratt_Edge*e=(struct Cyc_Pratt_Edge*)ds->hd;
res=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Pratt_Edge*_Tmp1=({struct Cyc_Pratt_Edge*_Tmp2=_cycalloc(sizeof(struct Cyc_Pratt_Edge));_Tmp2->x=e->x,_Tmp2->y=e->y,_Tmp2->value=e->value;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=res;_Tmp0;});}
# 161
return Cyc_List_imp_rev(res);}
# 164
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*G){
void*_Tmp0;void*_Tmp1;_Tmp1=G->nodes;_Tmp0=G->edges;{struct Cyc_List_List*nodes=_Tmp1;struct Cyc_List_List*edges=_Tmp0;
return({struct Cyc_Pratt_Graph*_Tmp2=_cycalloc(sizeof(struct Cyc_Pratt_Graph));_Tmp2->nodes=nodes,({struct Cyc_List_List*_Tmp3=Cyc_Pratt_copy_edges(edges);_Tmp2->edges=_Tmp3;});_Tmp2;});}}
# 171
static struct Cyc_AP_T**Cyc_Pratt_lookup_distance(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node x,union Cyc_Pratt_Node y){
# 173
if(Cyc_Pratt_cmp_node(x,y)==0)return& Cyc_AP_zero;
{struct Cyc_List_List*ds=G->edges;for(0;ds!=0;ds=ds->tl){
struct Cyc_Pratt_Edge*e=(struct Cyc_Pratt_Edge*)ds->hd;
if(Cyc_Pratt_cmp_node(e->x,x)==0 && Cyc_Pratt_cmp_node(e->y,y)==0)
return& e->value;}}
# 179
return 0;}
# 185
static void Cyc_Pratt_set_distance(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node x,union Cyc_Pratt_Node y,struct Cyc_AP_T*d){
struct Cyc_AP_T**r=Cyc_Pratt_lookup_distance(G,x,y);
if(r==0)
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Pratt_Edge*_Tmp2=({struct Cyc_Pratt_Edge*_Tmp3=_cycalloc(sizeof(struct Cyc_Pratt_Edge));_Tmp3->x=x,_Tmp3->y=y,_Tmp3->value=d;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=G->edges;_Tmp1;});G->edges=_Tmp0;});else{
# 190
*r=d;}}
# 194
void Cyc_Pratt_print_graph(struct Cyc_Pratt_Graph*G){
if(G==0){
Cyc_fprintf(Cyc_stderr,_tag_fat("<inconsistent>",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
return;}{
# 199
struct Cyc_Pratt_Graph _Tmp0=*G;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.nodes;_Tmp1=_Tmp0.edges;{struct Cyc_List_List*nodes=_Tmp2;struct Cyc_List_List*ds=_Tmp1;
for(1;ds!=0;ds=ds->tl){
struct Cyc_Pratt_Edge*e=(struct Cyc_Pratt_Edge*)ds->hd;
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({
struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Pratt_node2string(e->x));_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Pratt_node2string(e->y));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)({char*_Tmp8=Cyc_AP_tostr(e->value,10);_tag_fat((void*)_Tmp8,sizeof(char),_get_zero_arr_size_char((void*)_Tmp8,1U));});_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_fprintf(Cyc_stderr,_tag_fat("%s - %s <= %s",sizeof(char),14U),_tag_fat(_Tmp6,sizeof(void*),3));});
if(ds->tl!=0)Cyc_fprintf(Cyc_stderr,_tag_fat(", ",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));}}}}
# 209
static void Cyc_Pratt_add_node(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node x){
{struct Cyc_List_List*vs=G->nodes;for(0;vs!=0;vs=vs->tl){
union Cyc_Pratt_Node y=*((union Cyc_Pratt_Node*)vs->hd);
if(Cyc_Pratt_cmp_node(x,y)==0)return;}}
# 215
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({union Cyc_Pratt_Node*_Tmp2=({union Cyc_Pratt_Node*_Tmp3=_cycalloc(sizeof(union Cyc_Pratt_Node));*_Tmp3=x;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=G->nodes;_Tmp1;});G->nodes=_Tmp0;});}
# 221
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node i,union Cyc_Pratt_Node j,struct Cyc_AP_T*a){
# 228
Cyc_Pratt_add_node(G,i);
Cyc_Pratt_add_node(G,j);{
# 231
struct Cyc_AP_T**dist=Cyc_Pratt_lookup_distance(G,i,j);
# 233
if(dist!=0 && Cyc_AP_cmp(*dist,a)< 1)
# 237
return G;
# 240
dist=Cyc_Pratt_lookup_distance(G,j,i);
if(dist!=0 &&({struct Cyc_AP_T*_Tmp0=Cyc_AP_add(*dist,a);Cyc_AP_cmp(_Tmp0,Cyc_AP_zero);})< 0)
# 245
return 0;
# 248
{struct Cyc_List_List*ks=G->nodes;for(0;ks!=0;ks=ks->tl){
union Cyc_Pratt_Node k=*((union Cyc_Pratt_Node*)ks->hd);
struct Cyc_AP_T**dist_ki=Cyc_Pratt_lookup_distance(G,k,i);
if(dist_ki==0)continue;{
struct Cyc_List_List*ls=G->nodes;for(0;ls!=0;ls=ls->tl){
union Cyc_Pratt_Node l=*((union Cyc_Pratt_Node*)ls->hd);
struct Cyc_AP_T**dist_jl=Cyc_Pratt_lookup_distance(G,j,l);
if(dist_jl!=0){
struct Cyc_AP_T**dist_kl=Cyc_Pratt_lookup_distance(G,k,l);
# 258
struct Cyc_AP_T*d=({struct Cyc_AP_T*_Tmp0=*dist_ki;Cyc_AP_add(_Tmp0,Cyc_AP_add(a,*dist_jl));});
# 262
if(dist_kl==0)
# 266
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Pratt_Edge*_Tmp2=({struct Cyc_Pratt_Edge*_Tmp3=_cycalloc(sizeof(struct Cyc_Pratt_Edge));_Tmp3->x=k,_Tmp3->y=l,_Tmp3->value=d;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=G->edges;_Tmp1;});G->edges=_Tmp0;});else{
if(Cyc_AP_cmp(*dist_kl,d)> 0)
# 271
*dist_kl=d;}}}}}}
# 276
return G;}}

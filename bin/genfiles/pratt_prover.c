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
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, unsigned orig_s) {  
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
  return _region_malloc(r,orig_s); 
}

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 155 "absyn.h"
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 453 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 478
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};
# 854 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 872
extern void*Cyc_Absyn_uint_type;
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 63 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 66
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
void*Cyc_AssnDef_zero (void);
# 73
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop,void*,void*);
# 77
void*Cyc_AssnDef_cast(void*,void*);
# 80
int Cyc_AssnDef_cmp_term(void*,void*);
void*Cyc_AssnDef_get_term_type(void*);
# 85
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 123 "assndef.h"
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
void*Cyc_AssnDef_not(void*);
# 131
void*Cyc_AssnDef_slt(void*,void*);
# 133
void*Cyc_AssnDef_ult(void*,void*);
# 137
void*Cyc_AssnDef_reduce(void*);struct Cyc_PrattProver_Node;struct Cyc_PrattProver_Distance{struct Cyc_PrattProver_Distance*next;struct Cyc_PrattProver_Node*target;int dist;};struct Cyc_PrattProver_Node{struct Cyc_PrattProver_Node*next;struct Cyc_PrattProver_Node*equiv_class;void*rep;struct Cyc_PrattProver_Distance*unsigned_distances;struct Cyc_PrattProver_Distance*signed_distances;};struct Cyc_PrattProver_Graph{struct Cyc_PrattProver_Graph*next;struct Cyc_PrattProver_Node*rows;};
# 85 "pratt_prover.cyc"
static unsigned Cyc_PrattProver_num_graphs(struct Cyc_PrattProver_Graph*gs){
unsigned c=0U;
for(1;gs!=0;gs=gs->next){
++ c;}
return c;}
# 93
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_true_graph (void){return({struct Cyc_PrattProver_Graph*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Graph));_Tmp0->next=0,_Tmp0->rows=0;_Tmp0;});}
# 96
static struct Cyc_PrattProver_Node*Cyc_PrattProver_compress(struct Cyc_PrattProver_Node*n){
struct Cyc_PrattProver_Node*e=n->equiv_class;
if(e==0)return n;{
struct Cyc_PrattProver_Node*parent=Cyc_PrattProver_compress(e);
n->equiv_class=parent;
return parent;}}
# 105
void Cyc_PrattProver_print_graph(struct Cyc_PrattProver_Graph*g){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="{";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
if(g->rows==0)Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="<true>";_tag_fat(_Tmp0,sizeof(char),7U);}),_tag_fat(0U,sizeof(void*),0));else{
# 109
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
struct Cyc_PrattProver_Node*_Tmp0=rs;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp0->equiv_class;_Tmp3=_Tmp0->rep;_Tmp2=_Tmp0->unsigned_distances;_Tmp1=_Tmp0->signed_distances;{struct Cyc_PrattProver_Node*equiv_class=_Tmp4;void*rep=_Tmp3;struct Cyc_PrattProver_Distance*uds=_Tmp2;struct Cyc_PrattProver_Distance*sds=_Tmp1;
if(equiv_class!=0){
void*p=Cyc_PrattProver_compress(equiv_class)->rep;
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(rep);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(p);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_fprintf(Cyc_stderr,({const char*_Tmp8="%s == %s\n ";_tag_fat(_Tmp8,sizeof(char),11U);}),_tag_fat(_Tmp7,sizeof(void*),2));});}else{
# 115
for(1;uds!=0;uds=uds->next){
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(rep);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(Cyc_PrattProver_compress(uds->target)->rep);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)uds->dist;_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_fprintf(Cyc_stderr,({const char*_Tmp9="%s - %s U<= %d\n ";_tag_fat(_Tmp9,sizeof(char),17U);}),_tag_fat(_Tmp8,sizeof(void*),3));});}
for(1;sds!=0;sds=sds->next){
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(rep);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(Cyc_PrattProver_compress(sds->target)->rep);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)sds->dist;_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_fprintf(Cyc_stderr,({const char*_Tmp9="%s - %s S<= %d\n ";_tag_fat(_Tmp9,sizeof(char),17U);}),_tag_fat(_Tmp8,sizeof(void*),3));});}}}}}
# 124
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="}\n";_tag_fat(_Tmp0,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));}
# 128
void Cyc_PrattProver_print_graphs(struct Cyc_PrattProver_Graph*g){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="Graphs:-----------\n";_tag_fat(_Tmp0,sizeof(char),20U);}),_tag_fat(0U,sizeof(void*),0));
if(g==0)Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="<false>\n";_tag_fat(_Tmp0,sizeof(char),9U);}),_tag_fat(0U,sizeof(void*),0));else{
# 132
for(1;g!=0;g=g->next){Cyc_PrattProver_print_graph(g);}}
# 134
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="------------------\n";_tag_fat(_Tmp0,sizeof(char),20U);}),_tag_fat(0U,sizeof(void*),0));}
# 138
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node_opt(struct Cyc_PrattProver_Graph*,void*);
static int Cyc_PrattProver_set_distance(struct Cyc_PrattProver_Graph*,void*,void*,int,int);
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*,void*,enum Cyc_AssnDef_Primreln,void*);
static int Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*,void*,void*);
# 147
static int Cyc_PrattProver_add_type_info(struct Cyc_PrattProver_Graph*g,void*term){
{void*_Tmp0;if(*((int*)term)==4){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)term)->f1==Cyc_Absyn_Numelts){_Tmp0=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)term)->f2;{void*x=_Tmp0;
# 151
if((!({struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=term;Cyc_PrattProver_set_distance(_Tmp1,_Tmp2,Cyc_AssnDef_zero(),2147483646,1);})|| !({
struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=term;Cyc_PrattProver_set_distance(_Tmp1,_Tmp2,Cyc_AssnDef_zero(),2147483646,0);}))|| !({
struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp1,_Tmp2,term,0,0);}))return 0;
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 157
void*topt=Cyc_AssnDef_get_term_type(term);
if(topt!=0){
void*t=topt;
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(t);
if(eopt!=0){
void*t1=Cyc_AssnDef_unop(18U,term,Cyc_Absyn_uint_type);
# 164
struct Cyc_Absyn_Exp*e=eopt;
loop: {
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==14){_Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*tp2=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 168
{void*_Tmp3=Cyc_Absyn_compress(tp2);enum Cyc_Absyn_Size_of _Tmp4;if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==1){_Tmp4=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f2;{enum Cyc_Absyn_Size_of s=_Tmp4;
# 170
if((int)s==2 ||(int)s==3){
e=e2;goto loop;}
# 173
goto _LLA;}}else{goto _LLD;}}else{_LLD:
 goto _LLA;}_LLA:;}
# 176
goto _LL5;}}else{
goto _LL5;}_LL5:;}{
# 179
void*t2=Cyc_AssnDef_cnst(e);
if(!Cyc_PrattProver_add_constraint(g,t2,5U,t1))return 0;
if(!Cyc_PrattProver_add_constraint(g,t2,3U,t1))return 0;}}{
# 183
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1==Cyc_Absyn_Unsigned){
# 185
if(!({struct Cyc_PrattProver_Graph*_Tmp2=g;void*_Tmp3=Cyc_AssnDef_zero();Cyc_PrattProver_add_constraint(_Tmp2,_Tmp3,5U,term);}))return 0;
goto _LLF;}else{goto _LL14;}case 4: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*v=_Tmp1;
# 188
{void*_Tmp2=Cyc_Absyn_compress(v);void*_Tmp3;if(*((int*)_Tmp2)==9){_Tmp3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
({struct Cyc_PrattProver_Graph*_Tmp4=g;void*_Tmp5=term;Cyc_PrattProver_add_eq(_Tmp4,_Tmp5,Cyc_AssnDef_cnst(e));});goto _LL16;}}else{_Tmp3=_Tmp2;{void*v2=_Tmp3;
# 191
if(!({struct Cyc_PrattProver_Graph*_Tmp4=g;void*_Tmp5=term;Cyc_PrattProver_add_eq(_Tmp4,_Tmp5,Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp(v2,0U)));}))return 0;
goto _LL16;}}_LL16:;}
# 194
goto _LLF;}}else{goto _LL14;}default: goto _LL14;}else{_LL14:
 goto _LLF;}_LLF:;}}
# 198
return 1;}}
# 203
static struct Cyc_PrattProver_Node*Cyc_PrattProver_find_equiv_term(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*n){
void*t=n->rep;
{void*_Tmp0;void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;switch(*((int*)t)){case 5: _Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp2;void*t2=_Tmp1;void*tp=_Tmp0;
# 207
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node_opt(g,t1);
struct Cyc_PrattProver_Node*n2=Cyc_PrattProver_term2node_opt(g,t2);
if(n1==0 || n2==0)return 0;
if(t1!=n1->rep || t2!=n2->rep){
void*newt=Cyc_AssnDef_binop(p,n1->rep,n2->rep,tp);
struct Cyc_PrattProver_Node*r=g->rows;for(0;r!=0;r=r->next){
if(Cyc_AssnDef_cmp_term(newt,r->rep)==0){
struct Cyc_PrattProver_Node*m=Cyc_PrattProver_compress(r);
if(m!=n)n->equiv_class=m;
break;}}}
# 220
goto _LL0;}case 4: _Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp2;void*tp=_Tmp1;
# 222
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node_opt(g,t1);
if(n1==0)return 0;
if(t1!=n1->rep){
void*newt=Cyc_AssnDef_unop(p,n1->rep,tp);
struct Cyc_PrattProver_Node*r=g->rows;for(0;r!=0;r=r->next){
if(Cyc_AssnDef_cmp_term(newt,r->rep)==0){
struct Cyc_PrattProver_Node*m=Cyc_PrattProver_compress(r);
if(m!=n)n->equiv_class=m;
break;}}}
# 234
goto _LL0;}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp2;void*t1=_Tmp1;
# 236
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node_opt(g,t1);
if(n1==0)return 0;
if(t1!=n1->rep){
void*newt=Cyc_AssnDef_cast(tp,n1->rep);
struct Cyc_PrattProver_Node*r=g->rows;for(0;r!=0;r=r->next){
if(Cyc_AssnDef_cmp_term(newt,r->rep)==0){
struct Cyc_PrattProver_Node*m=Cyc_PrattProver_compress(r);
if(m!=n)n->equiv_class=m;
break;}}}
# 248
goto _LL0;}default:
# 250
 goto _LL0;}_LL0:;}
# 252
return Cyc_PrattProver_compress(n);}
# 257
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node_opt(struct Cyc_PrattProver_Graph*g,void*t){
# 259
{struct Cyc_PrattProver_Node*r=g->rows;for(0;r!=0;r=r->next){
if(Cyc_AssnDef_cmp_term(t,r->rep)==0)return Cyc_PrattProver_compress(r);}}{
# 263
struct Cyc_PrattProver_Node*n;n=_cycalloc(sizeof(struct Cyc_PrattProver_Node)),n->next=g->rows,n->equiv_class=0,n->rep=t,n->unsigned_distances=0,n->signed_distances=0;
g->rows=n;
# 266
n=Cyc_PrattProver_find_equiv_term(g,n);
if(n==0)return 0;
# 269
if(!Cyc_PrattProver_add_type_info(g,t))return 0;
return n;}}
# 273
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node(struct Cyc_PrattProver_Graph*g,void*t){
return _check_null(Cyc_PrattProver_term2node_opt(g,t));}
# 280
static int*Cyc_PrattProver_lookup_dist(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed){
static int zero=0;
source=Cyc_PrattProver_compress(source);
target=Cyc_PrattProver_compress(target);
if(source==target)return& zero;{
struct Cyc_PrattProver_Distance*ds=is_signed?source->signed_distances: source->unsigned_distances;
# 287
for(1;ds!=0;ds=ds->next){
if(ds->target==target)return& ds->dist;}
return 0;}}
# 295
static int*Cyc_PrattProver_lookup_and_delete_dist(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed){
struct Cyc_PrattProver_Distance**dsp=is_signed?& source->signed_distances:& source->unsigned_distances;
# 298
{struct Cyc_PrattProver_Distance*ds=*dsp;for(0;ds!=0;ds=ds->next){
if(ds->target==target){
*dsp=ds->next;
return& ds->dist;}
# 303
dsp=& ds->next;}}
# 305
return 0;}
# 309
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_graph_append(struct Cyc_PrattProver_Graph*g1,struct Cyc_PrattProver_Graph*g2){
if(g1==0)return g2;
if(g2==0)return g1;{
struct Cyc_PrattProver_Graph*p=g1;
{struct Cyc_PrattProver_Graph*x=p->next;for(0;x!=0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}struct _tuple12{int f1;int f2;};
# 320
enum Cyc_PrattProver_Operator{Cyc_PrattProver_PLUS =0U,Cyc_PrattProver_MINUS =1U};
# 330
static struct _tuple12 Cyc_PrattProver_is_signed_overflow(int c1,enum Cyc_PrattProver_Operator op,int c2){
switch((int)op){case Cyc_PrattProver_PLUS:  {
# 333
int sum=c1 + c2;
if((~(c1 ^ c2)& (sum ^ c1))>> 31){
# 336
if(sum > 0)return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=-2147483648;_Tmp0;});else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=2147483647;_Tmp0;});}}else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=sum;_Tmp0;});}}case Cyc_PrattProver_MINUS:  {
# 340
int dif=c1 - c2;
if(((c1 ^ c2)& (c1 ^ dif))>> 31){
# 343
if(dif > 0)return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=-2147483648;_Tmp0;});else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=2147483647;_Tmp0;});}}else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=dif;_Tmp0;});}}default:
# 347
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp0="Pratt_Prover: bad operator in overflow check";_tag_fat(_Tmp0,sizeof(char),45U);}),_tag_fat(0U,sizeof(void*),0));};}
# 355
static struct _tuple12 Cyc_PrattProver_is_unsigned_overflow(unsigned c1,enum Cyc_PrattProver_Operator op,unsigned c2){
# 357
if(op==Cyc_PrattProver_MINUS){
# 359
int dif=(int)(c1 - c2);
if((int)(((c1 ^ c2)& ~(c1 ^ (unsigned)dif))>> 31U)){
# 362
if(dif > 0)return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=-2147483648;_Tmp0;});else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=1,_Tmp0.f2=2147483647;_Tmp0;});}}else{
return({struct _tuple12 _Tmp0;_Tmp0.f1=0,_Tmp0.f2=dif;_Tmp0;});}}else{
# 366
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp0="Pratt_Prover: bad operator in overflow check";_tag_fat(_Tmp0,sizeof(char),45U);}),_tag_fat(0U,sizeof(void*),0));};}
# 372
static void Cyc_PrattProver_new_distance(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed,int d){
struct Cyc_PrattProver_Distance*dist;dist=_cycalloc(sizeof(struct Cyc_PrattProver_Distance)),dist->next=0,dist->target=target,dist->dist=d;
if(is_signed){
dist->next=source->signed_distances;
source->signed_distances=dist;}else{
# 378
dist->next=source->unsigned_distances;
source->unsigned_distances=dist;}}
# 388
static int Cyc_PrattProver_set_dist(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*i,struct Cyc_PrattProver_Node*j,int dist,int is_signed){
# 390
if(i==j && dist!=0)return 0;{
int*ij_dist=Cyc_PrattProver_lookup_dist(i,j,is_signed);
# 393
if(ij_dist!=0 &&*ij_dist < dist)return 1;{
int*ji_dist=Cyc_PrattProver_lookup_dist(j,i,is_signed);
# 396
if(ji_dist!=0){
struct _tuple12 _Tmp0=Cyc_PrattProver_is_signed_overflow(*ji_dist,0U,dist);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int overflow=_Tmp2;int sum=_Tmp1;
if(sum < 0)return 0;else{
# 401
if(overflow && sum > 0)return 1;}}}
# 405
{struct Cyc_PrattProver_Node*k=g->rows;for(0;k!=0;k=k->next){
# 407
if(k->equiv_class!=0)continue;{
int*ki_dist=Cyc_PrattProver_lookup_dist(k,i,is_signed);
if(ki_dist==0)continue;{
struct Cyc_PrattProver_Node*l=g->rows;for(0;l!=0;l=l->next){
# 413
if(k==l || l->equiv_class!=0)continue;{
int*kl_dist=Cyc_PrattProver_lookup_dist(k,l,is_signed);
# 416
if(kl_dist!=0 &&*kl_dist==-2147483648)continue;{
int*jl_dist=Cyc_PrattProver_lookup_dist(j,l,is_signed);
if(jl_dist==0)continue;{
# 420
struct _tuple12 final_sum;
struct _tuple12 _Tmp0=Cyc_PrattProver_is_signed_overflow(*ki_dist,0U,dist);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int of1=_Tmp2;int sum1=_Tmp1;
if(!of1)
final_sum=Cyc_PrattProver_is_signed_overflow(sum1,0U,*jl_dist);else{
# 427
struct _tuple12 _Tmp3=Cyc_PrattProver_is_signed_overflow(*ki_dist,0U,*jl_dist);int _Tmp4;int _Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{int of1=_Tmp5;int sum1=_Tmp4;
if(!of1)
# 430
final_sum=Cyc_PrattProver_is_signed_overflow(sum1,0U,dist);else{
# 435
final_sum=({struct _tuple12 _Tmp6;_Tmp6.f1=1,dist < 0?_Tmp6.f2=-2147483648:(_Tmp6.f2=2147483647);_Tmp6;});}}}{
# 438
int _Tmp3;int _Tmp4;_Tmp4=final_sum.f1;_Tmp3=final_sum.f2;{int final_overflow=_Tmp4;int sum=_Tmp3;
# 441
if(!final_overflow || sum < 0){
# 443
if(k==l && sum!=0)return 0;else{
# 445
if(kl_dist==0)Cyc_PrattProver_new_distance(k,l,is_signed,sum);else{
# 447
if(*kl_dist > sum)*kl_dist=sum;}}}}}}}}}}}}}}
# 451
return 1;}}}
# 455
static int Cyc_PrattProver_set_distance(struct Cyc_PrattProver_Graph*g,void*it,void*jt,int dist,int is_signed){
struct Cyc_PrattProver_Node*iopt=Cyc_PrattProver_term2node_opt(g,it);
struct Cyc_PrattProver_Node*jopt=Cyc_PrattProver_term2node_opt(g,jt);
if(iopt==0 || jopt==0)return 0;{
struct Cyc_PrattProver_Node*i=iopt;
struct Cyc_PrattProver_Node*j=jopt;
# 463
return Cyc_PrattProver_set_dist(g,i,j,dist,is_signed);}}
# 467
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_copy_distances(struct Cyc_PrattProver_Graph*newg,struct Cyc_PrattProver_Distance*ds){
struct Cyc_PrattProver_Distance*res=0;
for(1;ds!=0;ds=ds->next){
void*t=Cyc_PrattProver_compress(ds->target)->rep;
res=({struct Cyc_PrattProver_Distance*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Distance));_Tmp0->next=res,({struct Cyc_PrattProver_Node*_Tmp1=Cyc_PrattProver_term2node(newg,t);_Tmp0->target=_Tmp1;}),_Tmp0->dist=ds->dist;_Tmp0;});}
# 473
return res;}
# 477
static struct Cyc_PrattProver_Node*Cyc_PrattProver_revnodes(struct Cyc_PrattProver_Node*n){
if(n==0)return 0;{
struct Cyc_PrattProver_Node*first=n;
struct Cyc_PrattProver_Node*second=n->next;
n->next=0;
while(second!=0){
struct Cyc_PrattProver_Node*temp=second->next;
second->next=first;
first=second;
second=temp;}
# 488
return first;}}
# 493
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_copy_graphs(struct Cyc_PrattProver_Graph*gopt){
if(gopt==0)return 0;{
struct Cyc_PrattProver_Graph*g=gopt;
struct Cyc_PrattProver_Graph*newg;newg=_cycalloc(sizeof(struct Cyc_PrattProver_Graph)),({struct Cyc_PrattProver_Graph*_Tmp0=Cyc_PrattProver_copy_graphs(g->next);newg->next=_Tmp0;}),newg->rows=0;{
# 498
struct Cyc_PrattProver_Node*newrs=0;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
newrs=({struct Cyc_PrattProver_Node*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp0->next=newrs,_Tmp0->equiv_class=0,_Tmp0->rep=rs->rep,_Tmp0->unsigned_distances=0,_Tmp0->signed_distances=0;_Tmp0;});}}
# 502
newrs=Cyc_PrattProver_revnodes(newrs);
newg->rows=newrs;
# 506
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;(rs=rs->next,newrs=newrs->next)){
if(rs->equiv_class!=0)
# 511
({struct Cyc_PrattProver_Node*_Tmp0=({struct Cyc_PrattProver_Graph*_Tmp1=newg;Cyc_PrattProver_term2node(_Tmp1,Cyc_PrattProver_compress(rs)->rep);});_check_null(newrs)->equiv_class=_Tmp0;});else{
# 515
({struct Cyc_PrattProver_Distance*_Tmp0=Cyc_PrattProver_copy_distances(newg,rs->unsigned_distances);_check_null(newrs)->unsigned_distances=_Tmp0;});
({struct Cyc_PrattProver_Distance*_Tmp0=Cyc_PrattProver_copy_distances(newg,rs->unsigned_distances);newrs->signed_distances=_Tmp0;});}}}
# 519
return newg;}}}struct _tuple13{void*f1;void*f2;};
# 525
static int Cyc_PrattProver_unify_nodes(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*n1,struct Cyc_PrattProver_Node*n2){
n1=Cyc_PrattProver_compress(n1);
n2=Cyc_PrattProver_compress(n2);
if(n1==n2)return 1;{
struct Cyc_PrattProver_Node*source;struct Cyc_PrattProver_Node*target;
# 533
{struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f1=n1->rep,_Tmp1.f2=n2->rep;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned i1=_Tmp2;unsigned i2=_Tmp1;
if(i1!=i2)return 0;goto _LL4;}}else{goto _LL9;}}else{if(*((int*)_Tmp0.f2)==0){_LL4:
 goto _LL6;}else{switch(*((int*)_Tmp0.f1)){case 2: _LL6:
 goto _LL8;case 3: _LL8:
# 538
 source=n1;
target=n2;
goto _LL0;default: _LL9:
# 542
 source=n2;
target=n1;
goto _LL0;}}}_LL0:;}
# 548
{struct Cyc_PrattProver_Distance*sds=source->signed_distances;for(0;sds!=0;sds=sds->next){
if(!Cyc_PrattProver_set_dist(g,target,sds->target,sds->dist,1))return 0;}}
# 551
source->signed_distances=0;
{struct Cyc_PrattProver_Distance*uds=source->unsigned_distances;for(0;uds!=0;uds=uds->next){
if(!Cyc_PrattProver_set_dist(g,target,uds->target,uds->dist,0))return 0;}}
source->unsigned_distances=0;
# 559
{struct Cyc_PrattProver_Node*r=g->rows;for(0;r!=0;r=r->next){
# 561
if(r==source || r->equiv_class!=0)continue;{
# 564
int*rt_signed=Cyc_PrattProver_lookup_dist(r,target,1);
# 568
if(rt_signed!=0){
int*rs_signed=Cyc_PrattProver_lookup_and_delete_dist(r,source,1);
if(rs_signed!=0 &&(rt_signed==0 ||*rt_signed > *rs_signed)){
if(!Cyc_PrattProver_set_dist(g,r,target,*rs_signed,1))return 0;}}{
# 574
int*rt_unsigned=Cyc_PrattProver_lookup_dist(r,target,0);
if(rt_unsigned!=0){
int*rs_unsigned=Cyc_PrattProver_lookup_and_delete_dist(r,source,0);
if(rs_unsigned!=0 &&(rt_unsigned==0 ||*rt_unsigned > *rs_unsigned)){
if(!Cyc_PrattProver_set_dist(g,r,target,*rs_unsigned,0))return 0;}}}}}}
# 583
source->equiv_class=target;
return 1;}}
# 594 "pratt_prover.cyc"
static int Cyc_PrattProver_congruence_close_graph(struct Cyc_PrattProver_Graph*g){
int changed=1;
while(changed){
changed=0;{
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
struct Cyc_PrattProver_Node*rs_class=Cyc_PrattProver_compress(rs);
struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
struct Cyc_PrattProver_Node*ts_class=Cyc_PrattProver_compress(ts);
if(rs_class==ts_class)continue;{
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f1=rs->rep,_Tmp1.f2=ts->rep;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;void*_Tmp4;void*_Tmp5;enum Cyc_Absyn_Primop _Tmp6;switch(*((int*)_Tmp0.f1)){case 5: if(*((int*)_Tmp0.f2)==5){_Tmp6=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f2)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f2)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f2)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp6)==(int)((enum Cyc_Absyn_Primop)_Tmp3)){enum Cyc_Absyn_Primop p1=_Tmp6;void*t11=_Tmp5;void*t12=_Tmp4;enum Cyc_Absyn_Primop p2=_Tmp3;void*t21=_Tmp2;void*t22=_Tmp1;
# 605
struct Cyc_PrattProver_Node*n11=Cyc_PrattProver_term2node_opt(g,t11);
struct Cyc_PrattProver_Node*n21=Cyc_PrattProver_term2node_opt(g,t21);
if(n11==0 || n21==0)return 0;
if(n11!=n21)goto _LL0;{
struct Cyc_PrattProver_Node*n12=Cyc_PrattProver_term2node_opt(g,t12);
struct Cyc_PrattProver_Node*n22=Cyc_PrattProver_term2node_opt(g,t22);
if(n12==0 || n22==0)return 0;
if(n12!=n22)goto _LL0;
if(!Cyc_PrattProver_unify_nodes(g,rs,ts))return 0;
goto _LL0;}}else{goto _LL7;}}else{goto _LL7;}case 4: if(*((int*)_Tmp0.f2)==4){_Tmp6=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f2)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f2)->f2;if((int)((enum Cyc_Absyn_Primop)_Tmp6)==(int)((enum Cyc_Absyn_Primop)_Tmp3)){enum Cyc_Absyn_Primop p1=_Tmp6;void*t1=_Tmp5;enum Cyc_Absyn_Primop p2=_Tmp3;void*t2=_Tmp4;
# 616
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node_opt(g,t1);
struct Cyc_PrattProver_Node*n2=Cyc_PrattProver_term2node_opt(g,t2);
if(n1==0 || n2==0)return 0;
if(n1!=n2)goto _LL0;
if(!Cyc_PrattProver_unify_nodes(g,rs,ts))return 0;
goto _LL0;}else{goto _LL7;}}else{goto _LL7;}case 6: if(*((int*)_Tmp0.f2)==6){_Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f2)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f2)->f2;{void*tp1=_Tmp5;void*t1=_Tmp4;void*tp2=_Tmp2;void*t2=_Tmp1;
# 623
if(Cyc_Tcutil_typecmp(tp1,tp2)==0){
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node_opt(g,t1);
struct Cyc_PrattProver_Node*n2=Cyc_PrattProver_term2node_opt(g,t2);
if(n1==0 || n2==0)return 0;
if(n1==n2){
changed=1;
if(!Cyc_PrattProver_unify_nodes(g,rs,ts))return 0;}}
# 632
goto _LL0;}}else{goto _LL7;}default: _LL7:
 goto _LL0;}_LL0:;}}}}}
# 638
return 1;}
# 642
static struct _tuple11 Cyc_PrattProver_eval_term(void*t){
void*_Tmp0;unsigned _Tmp1;switch(*((int*)t)){case 0: _Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp1;
return({struct _tuple11 _Tmp2;_Tmp2.f1=i,_Tmp2.f2=1;_Tmp2;});}case 1: _Tmp0=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
return Cyc_Evexp_eval_const_uint_exp(e);}default:
 return({struct _tuple11 _Tmp2;_Tmp2.f1=0U,_Tmp2.f2=0;_Tmp2;});};}struct _tuple14{void*f1;int f2;};
# 651
static struct _tuple14 Cyc_PrattProver_break_term(struct Cyc_PrattProver_Graph*g,int is_signed,void*t){
int c=0;
unsigned cu=0U;
void*res=t;
{unsigned _Tmp0;void*_Tmp1;if(*((int*)t)==5){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned c2=_Tmp0;
# 657
res=t1;
c=(int)c2;
cu=c2;
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 662
 return({struct _tuple14 _Tmp2;_Tmp2.f1=t,_Tmp2.f2=0;_Tmp2;});}_LL0:;}
# 666
if(is_signed){
if(c==0)return({struct _tuple14 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=0;_Tmp0;});else{
if(c > 0){
# 670
int*dist=({struct Cyc_PrattProver_Node*_Tmp0=Cyc_PrattProver_term2node(g,res);Cyc_PrattProver_lookup_dist(_Tmp0,({struct Cyc_PrattProver_Graph*_Tmp1=g;Cyc_PrattProver_term2node(_Tmp1,Cyc_AssnDef_zero());}),1);});
if(dist==0 ||*dist > 2147483647 - c)
return({struct _tuple14 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});else{
# 674
return({struct _tuple14 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=c;_Tmp0;});}}else{
# 677
int*dist=({struct Cyc_PrattProver_Node*_Tmp0=({struct Cyc_PrattProver_Graph*_Tmp1=g;Cyc_PrattProver_term2node(_Tmp1,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp0,Cyc_PrattProver_term2node(g,res),1);});
if(dist==0 ||*dist > c - -2147483648)
return({struct _tuple14 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});else{
# 684
return({struct _tuple14 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=c;_Tmp0;});}}}}else{
# 688
int*dist=({struct Cyc_PrattProver_Node*_Tmp0=Cyc_PrattProver_term2node(g,res);Cyc_PrattProver_lookup_dist(_Tmp0,({struct Cyc_PrattProver_Graph*_Tmp1=g;Cyc_PrattProver_term2node(_Tmp1,Cyc_AssnDef_zero());}),0);});
if(dist==0)return({struct _tuple14 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});
if(*dist <= 0)return({struct _tuple14 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=(int)cu;_Tmp0;});
if(*dist > 0 &&(unsigned)*dist <= 4294967295U - cu)
return({struct _tuple14 _Tmp0;_Tmp0.f1=res,_Tmp0.f2=(int)cu;_Tmp0;});}
# 694
return({struct _tuple14 _Tmp0;_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});}
# 701
static int Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*g,void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
{struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned c1=_Tmp2;unsigned c2=_Tmp1;
# 704
switch((int)p){case Cyc_AssnDef_ULt:
 return c1 < c2;case Cyc_AssnDef_ULte:
 return c1 <= c2;case Cyc_AssnDef_SLt:
 return(int)c1 < (int)c2;case Cyc_AssnDef_SLte:
 return(int)c1 <= (int)c2;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp3="Vcgen: found bad primop in add_constraint";_tag_fat(_Tmp3,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));};}}else{_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned c1=_Tmp2;
# 752
switch((int)p){case Cyc_AssnDef_ULt:
# 755
 if(c1==4294967295U)return 0;
c1=c1 + 1U;
goto _LL23;case Cyc_AssnDef_ULte: _LL23:
# 760
 if(c1 > 2147483647U){
# 763
if(!({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,Cyc_AssnDef_zero(),-1,1);}))return 0;{
int k=-(int)c1;
if(k > 0){
if(!({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,k,1);}))return 0;}}}
# 769
if(c1 <= 2147483648U){
# 771
int k=-(int)c1;
return({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,k,0);});}
# 774
return 1;case Cyc_AssnDef_SLt:
# 777
 if(c1==2147483647U)return 0;
c1=c1 + 1U;
goto _LL27;case Cyc_AssnDef_SLte: _LL27:
# 782
 if(c1==2147483648U)return 1;else{
# 784
int k=-(int)c1;
if((int)c1 >= 0){
# 787
if(!({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,k,0);}))return 0;}
# 789
return({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,k,1);});}default:
# 791
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp3="Vcgen: found bad primop in add_constraint";_tag_fat(_Tmp3,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));};}}}else{if(*((int*)_Tmp0.f2)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned c2=_Tmp2;
# 713
switch((int)p){case Cyc_AssnDef_ULt:
# 716
 if(c2==0U)return 0;
c2=c2 - 1U;
goto _LL18;case Cyc_AssnDef_ULte: _LL18:
# 721
 if(c2 <= 2147483647U){
if(!({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(int)c2,0);}))return 0;
# 724
return({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(int)c2,1);});}else{
# 726
int*c1=({struct Cyc_PrattProver_Node*_Tmp3=Cyc_PrattProver_term2node(g,t1);Cyc_PrattProver_lookup_dist(_Tmp3,({struct Cyc_PrattProver_Graph*_Tmp4=g;Cyc_PrattProver_term2node(_Tmp4,Cyc_AssnDef_zero());}),1);});
if(c1!=0 &&*c1 <= -1)
return({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(int)c2,1);});}
# 731
return 1;case Cyc_AssnDef_SLt:
# 734
 if(c2==2147483648U)return 0;
c2=c2 - 1U;
goto _LL1C;case Cyc_AssnDef_SLte: _LL1C:
# 739
 if(!({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(int)c2,1);}))return 0;
if((int)c2 > 0){
# 742
int*c1=({struct Cyc_PrattProver_Node*_Tmp3=({struct Cyc_PrattProver_Graph*_Tmp4=g;Cyc_PrattProver_term2node(_Tmp4,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp3,Cyc_PrattProver_term2node(g,t1),1);});
if(c1!=0 &&*c1 <= 0)
# 745
return({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(int)c2,0);});}
# 747
return 1;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp3="Vcgen: found bad primop in add_constraint";_tag_fat(_Tmp3,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));};}}else{
# 793
goto _LL0;}}_LL0:;}
# 799
switch((int)p){case Cyc_AssnDef_ULt:  {
# 802
struct _tuple14 _Tmp0=Cyc_PrattProver_break_term(g,0,t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*tt1=_Tmp2;int c1=_Tmp1;
struct _tuple14 _Tmp3=Cyc_PrattProver_break_term(g,0,t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{void*tt2=_Tmp5;int c2=_Tmp4;
struct _tuple12 _Tmp6=Cyc_PrattProver_is_unsigned_overflow((unsigned)c2,1U,(unsigned)c1);int _Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int of=_Tmp8;int dist=_Tmp7;
if(!of){
struct _tuple12 _Tmp9=Cyc_PrattProver_is_unsigned_overflow((unsigned)dist,1U,1U);int _TmpA;int _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{int of=_TmpB;int dist=_TmpA;
if(!of){
if(!Cyc_PrattProver_set_distance(g,tt1,tt2,dist,0))
return 0;}}}
# 812
return Cyc_PrattProver_set_distance(g,t1,t2,-1,0);}}}}case Cyc_AssnDef_ULte:  {
# 815
struct _tuple14 _Tmp0=Cyc_PrattProver_break_term(g,0,t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*tt1=_Tmp2;int c1=_Tmp1;
struct _tuple14 _Tmp3=Cyc_PrattProver_break_term(g,0,t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{void*tt2=_Tmp5;int c2=_Tmp4;
struct _tuple12 _Tmp6=Cyc_PrattProver_is_unsigned_overflow((unsigned)c2,1U,(unsigned)c1);int _Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int of=_Tmp8;int dist=_Tmp7;
if(!of){
# 821
if(!Cyc_PrattProver_set_distance(g,tt1,tt2,dist,0))
return 0;}
# 824
return Cyc_PrattProver_set_distance(g,t1,t2,0,0);}}}}case Cyc_AssnDef_SLt:  {
# 826
struct _tuple14 _Tmp0=Cyc_PrattProver_break_term(g,1,t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*tt1=_Tmp2;int c1=_Tmp1;
struct _tuple14 _Tmp3=Cyc_PrattProver_break_term(g,1,t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{void*tt2=_Tmp5;int c2=_Tmp4;
struct _tuple12 _Tmp6=Cyc_PrattProver_is_signed_overflow(c2,1U,c1);int _Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int of=_Tmp8;int dist=_Tmp7;
if(!of){
struct _tuple12 _Tmp9=Cyc_PrattProver_is_signed_overflow(dist,1U,1);int _TmpA;int _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{int of=_TmpB;int dist=_TmpA;
if(!of){
if(!Cyc_PrattProver_set_distance(g,tt1,tt2,dist,1))
return 0;}}}
# 836
return Cyc_PrattProver_set_distance(g,t1,t2,-1,1);}}}}case Cyc_AssnDef_SLte:  {
# 839
struct _tuple14 _Tmp0=Cyc_PrattProver_break_term(g,1,t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*tt1=_Tmp2;int c1=_Tmp1;
struct _tuple14 _Tmp3=Cyc_PrattProver_break_term(g,1,t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{void*tt2=_Tmp5;int c2=_Tmp4;
struct _tuple12 _Tmp6=Cyc_PrattProver_is_signed_overflow(c2,1U,c1);int _Tmp7;int _Tmp8;_Tmp8=_Tmp6.f1;_Tmp7=_Tmp6.f2;{int of=_Tmp8;int dist=_Tmp7;
if(!of){
if(!Cyc_PrattProver_set_distance(g,tt1,tt2,dist,1))
return 0;}
# 846
return Cyc_PrattProver_set_distance(g,t1,t2,0,1);}}}}default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Tcutil_impos;})(({const char*_Tmp0="Vcgen:found bad primop in add_constraint";_tag_fat(_Tmp0,sizeof(char),41U);}),_tag_fat(0U,sizeof(void*),0));};}
# 851
static int Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node_opt(g,t1);
struct Cyc_PrattProver_Node*n2=Cyc_PrattProver_term2node_opt(g,t2);
if(n1==0 || n2==0)return 0;
return
# 857
(((Cyc_PrattProver_unify_nodes(g,n1,n2)&&
 Cyc_PrattProver_add_constraint(g,t1,5U,t2))&&
 Cyc_PrattProver_add_constraint(g,t2,5U,t1))&&
 Cyc_PrattProver_add_constraint(g,t1,3U,t2))&&
 Cyc_PrattProver_add_constraint(g,t2,3U,t1);}char Cyc_PrattProver_TooLarge[9U]="TooLarge";struct Cyc_PrattProver_TooLarge_exn_struct{char*tag;};
# 865
struct Cyc_PrattProver_TooLarge_exn_struct Cyc_PrattProver_too_large={Cyc_PrattProver_TooLarge};
# 868
unsigned Cyc_PrattProver_max_paths=33U;
unsigned Cyc_PrattProver_max_paths_seen=0U;
# 882 "pratt_prover.cyc"
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_cgraph(struct Cyc_PrattProver_Graph*gs,void*a){
LOOP:
# 886
 if(gs==0)return gs;
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL0;case 1:
 gs=0;goto _LL0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 891
gs=Cyc_PrattProver_cgraph(gs,a1);
a=a2;
goto LOOP;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 895
unsigned n=Cyc_PrattProver_num_graphs(gs);
if(Cyc_PrattProver_max_paths_seen < n)Cyc_PrattProver_max_paths_seen=n;
# 898
if(n >= Cyc_PrattProver_max_paths)
(void*)_throw((void*)& Cyc_PrattProver_too_large);{
# 902
struct Cyc_PrattProver_Graph*gs1=gs;
struct Cyc_PrattProver_Graph*gs2=Cyc_PrattProver_copy_graphs(gs);
# 905
gs1=Cyc_PrattProver_cgraph(gs1,a1);
# 907
gs2=Cyc_PrattProver_cgraph(gs2,a2);
# 909
gs=Cyc_PrattProver_graph_append(gs1,gs2);
goto _LL0;}}case 2: if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){_Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;void*t2=_Tmp1;
# 914
a=({void*_Tmp3=({void*_Tmp4=Cyc_AssnDef_slt(t1,t2);Cyc_AssnDef_or(_Tmp4,Cyc_AssnDef_slt(t2,t1));});Cyc_AssnDef_and(_Tmp3,({void*_Tmp4=Cyc_AssnDef_ult(t1,t2);Cyc_AssnDef_or(_Tmp4,Cyc_AssnDef_ult(t2,t1));}));});
goto LOOP;}}else{_Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 923
struct Cyc_PrattProver_Graph*prev=0;
{struct Cyc_PrattProver_Graph*g=gs;for(0;g!=0;g=g->next){
int res;
if((int)p==0)
res=Cyc_PrattProver_add_eq(g,t1,t2);else{
# 929
res=Cyc_PrattProver_add_constraint(g,t1,p,t2);}
if(!res){
if(prev==0)
gs=g->next;else{
# 934
prev->next=g->next;}}else{
# 937
prev=g;}}}
# 943
goto _LL0;}}default:
# 946
 a=Cyc_AssnDef_reduce(a);
goto LOOP;}_LL0:;}
# 949
return gs;}
# 955
static int Cyc_PrattProver_consistent(void*a){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
# 958
{struct Cyc_PrattProver_Graph*gs=({struct Cyc_PrattProver_Graph*_Tmp2=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_Tmp2,a);});
# 982 "pratt_prover.cyc"
int _Tmp2=gs!=0;_npop_handler(0);return _Tmp2;}
# 958 "pratt_prover.cyc"
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_PrattProver_TooLarge_exn_struct*)_Tmp2)->tag==Cyc_PrattProver_TooLarge)
# 986 "pratt_prover.cyc"
return 1;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}};}}}
# 991
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a){
void*b=({void*_Tmp0=ctxt;Cyc_AssnDef_and(_Tmp0,Cyc_AssnDef_not(a));});
# 996
return !Cyc_PrattProver_consistent(b);}

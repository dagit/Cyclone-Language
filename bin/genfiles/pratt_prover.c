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
# 156 "absyn.h"
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 481 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 506
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};
# 886 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 904
extern void*Cyc_Absyn_uint_type;
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 72 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 38 "tcutil.h"
int Cyc_Tcutil_is_signed_type(void*);
# 48
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
# 212
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 79
extern struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 114
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 82 "assndef.h"
extern struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 84
extern void*Cyc_AssnDef_uint(unsigned);
extern void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
extern void*Cyc_AssnDef_zero (void);
# 95
extern void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
extern void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop,void*,void*);
# 105
extern int Cyc_AssnDef_cmp_term(void*,void*);
extern void*Cyc_AssnDef_get_term_type(void*);
# 110
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 122
extern int Cyc_AssnDef_assncmp(void*,void*);
# 148 "assndef.h"
extern void*Cyc_AssnDef_and(void*,void*);
extern void*Cyc_AssnDef_or(void*,void*);
extern void*Cyc_AssnDef_not(void*);
# 156
extern void*Cyc_AssnDef_slt(void*,void*);
# 158
extern void*Cyc_AssnDef_ult(void*,void*);
# 162
extern void*Cyc_AssnDef_reduce(void*);struct Cyc_PrattProver_Node;struct Cyc_PrattProver_Distance{struct Cyc_PrattProver_Distance*next;struct Cyc_PrattProver_Node*target;int dist;};struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct{int tag;};struct Cyc_PrattProver_Shortest_PrattProver_ShortestPathInfo_struct{int tag;int f1;};struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct{int tag;int f1;};struct Cyc_PrattProver_Node{struct Cyc_PrattProver_Node*next;void*rep;int broken_as_signed: 1;int broken_as_unsigned: 1;struct Cyc_PrattProver_Distance*unsigned_distances;struct Cyc_PrattProver_Distance*signed_distances;int signeddistFromS;int unsigneddistFromS;void*shortest_path_info;};struct Cyc_PrattProver_Graph{struct Cyc_PrattProver_Graph*next;struct Cyc_PrattProver_Node*rows;int changed;};
# 193 "pratt_prover.cyc"
static unsigned Cyc_PrattProver_num_graphs(struct Cyc_PrattProver_Graph*gs){
unsigned c=0U;
for(1;gs!=0;gs=gs->next){
++ c;}
return c;}
# 201
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_true_graph (void){return({struct Cyc_PrattProver_Graph*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Graph));_Tmp0->next=0,_Tmp0->rows=0,_Tmp0->changed=0;_Tmp0;});}
# 203
static int Cyc_PrattProver_constraints_added=0;
static int Cyc_PrattProver_graphs_copied=0;
static int Cyc_PrattProver_max_lookup=0;
static int Cyc_PrattProver_already_seen_hits=0;
# 209
void Cyc_PrattProver_print_graph(struct Cyc_PrattProver_Graph*g){
Cyc_fprintf(Cyc_stderr,_tag_fat("{",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
if(g->rows==0)Cyc_fprintf(Cyc_stderr,_tag_fat("<true>",sizeof(char),7U),_tag_fat(0U,sizeof(void*),0));else{
# 213
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
struct Cyc_PrattProver_Node*_Tmp0=rs;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->rep;_Tmp2=_Tmp0->unsigned_distances;_Tmp1=_Tmp0->signed_distances;{void*rep=_Tmp3;struct Cyc_PrattProver_Distance*uds=_Tmp2;struct Cyc_PrattProver_Distance*sds=_Tmp1;
struct _fat_ptr s=Cyc_AssnDef_term2string(rep);
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("row %s:",sizeof(char),8U),_tag_fat(_Tmp5,sizeof(void*),1));});
if(uds==0)Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
for(1;uds!=0;uds=uds->next){
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(uds->target->rep));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)uds->dist;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("  %s - %s U<= %d\n ",sizeof(char),19U),_tag_fat(_Tmp7,sizeof(void*),3));});}
for(1;sds!=0;sds=sds->next){
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(sds->target->rep));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)sds->dist;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("  %s - %s S<= %d\n ",sizeof(char),19U),_tag_fat(_Tmp7,sizeof(void*),3));});}}}}
# 226
Cyc_fprintf(Cyc_stderr,_tag_fat("}\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));}
# 230
void Cyc_PrattProver_print_graphs(struct Cyc_PrattProver_Graph*g){
Cyc_fprintf(Cyc_stderr,_tag_fat("Graphs:-----------\n",sizeof(char),20U),_tag_fat(0U,sizeof(void*),0));
if(g==0)Cyc_fprintf(Cyc_stderr,_tag_fat("<false>\n",sizeof(char),9U),_tag_fat(0U,sizeof(void*),0));else{
# 234
for(1;g!=0;g=g->next){Cyc_PrattProver_print_graph(g);}}
# 236
Cyc_fprintf(Cyc_stderr,_tag_fat("------------------\n",sizeof(char),20U),_tag_fat(0U,sizeof(void*),0));}char Cyc_PrattProver_Inconsistent[13U]="Inconsistent";struct Cyc_PrattProver_Inconsistent_exn_struct{char*tag;};
# 241
struct Cyc_PrattProver_Inconsistent_exn_struct Cyc_PrattProver_inconsistent={Cyc_PrattProver_Inconsistent};
# 244
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node(struct Cyc_PrattProver_Graph*,void*);
static void Cyc_PrattProver_set_distance(struct Cyc_PrattProver_Graph*,void*,void*,unsigned,int);
static void Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*,void*,enum Cyc_AssnDef_Primreln,void*);
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*,void*,void*);struct _tuple11{int f0;unsigned f1;void*f2;};
static struct _tuple11 Cyc_PrattProver_subst_term_with_const(struct Cyc_PrattProver_Graph*,void*);
# 251
static struct Cyc_Absyn_Exp*Cyc_PrattProver_strip_cast(struct Cyc_Absyn_Exp*e){
LOOP: {
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==14){_Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*tp2=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 255
{void*_Tmp3=Cyc_Absyn_compress(tp2);enum Cyc_Absyn_Size_of _Tmp4;if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==1){_Tmp4=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f2;{enum Cyc_Absyn_Size_of s=_Tmp4;
# 257
if((int)s==2 ||(int)s==3){
e=e2;goto LOOP;}
# 260
goto _LL5;}}else{goto _LL8;}}else{_LL8:
 goto _LL5;}_LL5:;}
# 263
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 266
return e;}
# 273
static void Cyc_PrattProver_add_type_info(struct Cyc_PrattProver_Graph*g,void*term){
# 277
({struct Cyc_PrattProver_Graph*_Tmp0=g;void*_Tmp1=term;Cyc_PrattProver_set_distance(_Tmp0,_Tmp1,Cyc_AssnDef_zero(),2147483647U,1);});
({struct Cyc_PrattProver_Graph*_Tmp0=g;void*_Tmp1=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp0,_Tmp1,term,0U,0);});
{void*_Tmp0;if(*((int*)term)==7){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)term)->f1==Cyc_Absyn_Numelts){_Tmp0=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)term)->f2;{void*x=_Tmp0;
# 282
({struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=term;Cyc_PrattProver_set_distance(_Tmp1,_Tmp2,Cyc_AssnDef_zero(),2147483646U,0);});
# 284
Cyc_PrattProver_term2node(g,x);
goto _LL0;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 288
void*topt=Cyc_AssnDef_get_term_type(term);
if(topt!=0){
void*t=topt;
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(t);
if(eopt!=0 && Cyc_Tcutil_is_const_exp(eopt)){
void*t1=Cyc_AssnDef_unop(18U,term,Cyc_Absyn_uint_type);
# 295
struct Cyc_Absyn_Exp*e=Cyc_PrattProver_strip_cast(eopt);
void*t2=Cyc_AssnDef_cnst(e);
# 300
if(!Cyc_Tcutil_is_nullable_pointer_type(t,0)){
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);}}{
# 305
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1==Cyc_Absyn_Unsigned){
# 307
({struct Cyc_PrattProver_Graph*_Tmp2=g;void*_Tmp3=Cyc_AssnDef_zero();Cyc_PrattProver_add_constraint(_Tmp2,_Tmp3,5U,term);});
goto _LL5;}else{goto _LLA;}case 5: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*v=_Tmp1;
# 310
{void*_Tmp2=Cyc_Absyn_compress(v);void*_Tmp3;if(*((int*)_Tmp2)==8){_Tmp3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
({struct Cyc_PrattProver_Graph*_Tmp4=g;void*_Tmp5=term;Cyc_PrattProver_add_eq(_Tmp4,_Tmp5,Cyc_AssnDef_cnst(e));});goto _LLC;}}else{_Tmp3=_Tmp2;{void*v2=_Tmp3;
# 313
({struct Cyc_PrattProver_Graph*_Tmp4=g;void*_Tmp5=term;Cyc_PrattProver_add_eq(_Tmp4,_Tmp5,Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp(v2,0U)));});
goto _LLC;}}_LLC:;}
# 316
goto _LL5;}}else{goto _LLA;}default: goto _LLA;}else{_LLA:
 goto _LL5;}_LL5:;}}}}
# 324
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node(struct Cyc_PrattProver_Graph*g,void*t){
# 326
struct Cyc_PrattProver_Node**prev=& g->rows;
int count=0;
{struct Cyc_PrattProver_Node*r=g->rows;for(0;r!=0;r=r->next){
++ count;
if(count > Cyc_PrattProver_max_lookup)Cyc_PrattProver_max_lookup=count;{
int c=Cyc_AssnDef_cmp_term(t,r->rep);
if(c==0)return r;
if(c > 1)break;
prev=& r->next;}}}{
# 337
struct Cyc_PrattProver_Node*n;
{unsigned _Tmp0;void*_Tmp1;if(*((int*)t)==8){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned c2=_Tmp0;
# 340
n=({struct Cyc_PrattProver_Node*_Tmp2=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp2->next=*prev,_Tmp2->rep=t,_Tmp2->broken_as_signed=0,_Tmp2->broken_as_unsigned=0,_Tmp2->unsigned_distances=0,_Tmp2->signed_distances=0,_Tmp2->signeddistFromS=0,_Tmp2->unsigneddistFromS=0,({void*_Tmp3=(void*)({struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct));_Tmp4->tag=0;_Tmp4;});_Tmp2->shortest_path_info=_Tmp3;});_Tmp2;});
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 343
 n=({struct Cyc_PrattProver_Node*_Tmp2=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp2->next=*prev,_Tmp2->rep=t,_Tmp2->broken_as_signed=1,_Tmp2->broken_as_unsigned=1,_Tmp2->unsigned_distances=0,_Tmp2->signed_distances=0,_Tmp2->signeddistFromS=0,_Tmp2->unsigneddistFromS=0,({void*_Tmp3=(void*)({struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct));_Tmp4->tag=0;_Tmp4;});_Tmp2->shortest_path_info=_Tmp3;});_Tmp2;});
goto _LL0;}_LL0:;}
# 346
*prev=n;
Cyc_PrattProver_add_type_info(g,t);
return n;}}
# 355
static int*Cyc_PrattProver_lookup_dist(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed){
static int zero=0;
if(source==target)return& zero;{
struct Cyc_PrattProver_Distance*ds=is_signed?source->signed_distances: source->unsigned_distances;
# 360
for(1;ds!=0;ds=ds->next){
if(ds->target==target)return& ds->dist;}
return 0;}}
# 368
static int Cyc_PrattProver_eq_nodes(struct Cyc_PrattProver_Node*r,struct Cyc_PrattProver_Node*t,int is_signed){
int*rt_dist=Cyc_PrattProver_lookup_dist(r,t,is_signed);
int*tr_dist=Cyc_PrattProver_lookup_dist(t,r,is_signed);
return((rt_dist!=0 &&*rt_dist==0)&& tr_dist!=0)&&*tr_dist==0;}
# 375
static int Cyc_PrattProver_equal_nodes(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Node*r){
return Cyc_PrattProver_eq_nodes(s,r,0)|| Cyc_PrattProver_eq_nodes(s,r,1);}struct _tuple12{void*f0;void*f1;};
# 379
static int Cyc_PrattProver_equal_terms(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
struct _tuple11 _Tmp0=Cyc_PrattProver_subst_term_with_const(g,t1);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok1=_Tmp2;unsigned c1=_Tmp1;
struct _tuple11 _Tmp3=Cyc_PrattProver_subst_term_with_const(g,t2);unsigned _Tmp4;int _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{int ok2=_Tmp5;unsigned c2=_Tmp4;
if((ok1 && ok2)&& c1==c2)return 1;{
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node(g,t1);
struct Cyc_PrattProver_Node*n2=Cyc_PrattProver_term2node(g,t2);
if(Cyc_PrattProver_equal_nodes(n1,n2))return 1;
{struct _tuple12 _Tmp6=({struct _tuple12 _Tmp7;_Tmp7.f0=t1,_Tmp7.f1=t2;_Tmp7;});void*_Tmp7;void*_Tmp8;void*_Tmp9;enum Cyc_Absyn_Primop _TmpA;void*_TmpB;enum Cyc_Absyn_Primop _TmpC;switch(*((int*)_Tmp6.f0)){case 7: if(*((int*)_Tmp6.f1)==7){_TmpC=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpB=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpA=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f1)->f2;if((int)((enum Cyc_Absyn_Primop)_TmpC)==(int)((enum Cyc_Absyn_Primop)_TmpA)){enum Cyc_Absyn_Primop p11=_TmpC;void*t11=_TmpB;enum Cyc_Absyn_Primop p21=_TmpA;void*t21=_Tmp9;
# 388
return Cyc_PrattProver_equal_terms(g,t11,t21);}else{goto _LLD;}}else{goto _LLD;}case 8: if(*((int*)_Tmp6.f1)==8){_TmpC=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpB=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f3;_TmpA=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp8=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f3;if((int)((enum Cyc_Absyn_Primop)_TmpC)==(int)((enum Cyc_Absyn_Primop)_TmpA)){enum Cyc_Absyn_Primop p11=_TmpC;void*t11=_TmpB;void*t12=_Tmp9;enum Cyc_Absyn_Primop p21=_TmpA;void*t21=_Tmp8;void*t22=_Tmp7;
# 390
return Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22);}else{goto _LLD;}}else{goto _LLD;}case 9: if(*((int*)_Tmp6.f1)==9){_TmpB=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f0)->f2;_Tmp8=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f1)->f2;{void*tp1=_TmpB;void*t11=_Tmp9;void*tp2=_Tmp8;void*t21=_Tmp7;
# 392
return Cyc_Tcutil_typecmp(tp1,tp2)==0 && Cyc_PrattProver_equal_terms(g,t11,t21);}}else{goto _LLD;}default: _LLD:
# 394
 return 0;};}
# 396
return 0;}}}}
# 400
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_graph_append(struct Cyc_PrattProver_Graph*g1,struct Cyc_PrattProver_Graph*g2){
if(g1==0)return g2;
if(g2==0)return g1;{
struct Cyc_PrattProver_Graph*p=g1;
{struct Cyc_PrattProver_Graph*x=p->next;for(0;x!=0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}struct _tuple13{int f0;int f1;};
# 413
inline static struct _tuple13 Cyc_PrattProver_is_signed_overflow(int c1,int c2){
int sum=c1 + c2;
if((~(c1 ^ c2)& (sum ^ c1))>> 31){
# 417
if(sum > 0)return({struct _tuple13 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=-1;_Tmp0;});else{
return({struct _tuple13 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=1;_Tmp0;});}}else{
# 420
return({struct _tuple13 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=sum;_Tmp0;});}}
# 425
static void Cyc_PrattProver_new_distance(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed,int d){
struct Cyc_PrattProver_Distance*dist;dist=_cycalloc(sizeof(struct Cyc_PrattProver_Distance)),dist->next=0,dist->target=target,dist->dist=d;
if(is_signed){
dist->next=source->signed_distances;
source->signed_distances=dist;}else{
# 431
dist->next=source->unsigned_distances;
source->unsigned_distances=dist;}}
# 520 "pratt_prover.cyc"
inline static void Cyc_PrattProver_set_dist(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*i,struct Cyc_PrattProver_Node*j,int dist,int is_signed){
# 522
if(i==j &&(dist < 0 ||(unsigned)dist==2147483648U))_throw(& Cyc_PrattProver_inconsistent);
if(i==j && dist > 0)return;{
int*ij_dist=Cyc_PrattProver_lookup_dist(i,j,is_signed);
# 526
if(ij_dist!=0 &&(unsigned)*ij_dist==2147483648U)return;
if((ij_dist!=0 &&(unsigned)dist!=2147483648U)&&*ij_dist < dist)return;{
# 532
int*ji_dist=Cyc_PrattProver_lookup_dist(j,i,is_signed);
if(ji_dist!=0){
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(*ji_dist,dist);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
if(sum < 0)
# 539
_throw(& Cyc_PrattProver_inconsistent);}}
# 542
if(ij_dist!=0)*ij_dist=dist;else{
Cyc_PrattProver_new_distance(i,j,is_signed,dist);}
g->changed=1;
return;}}}
# 586 "pratt_prover.cyc"
static int Cyc_PrattProver_num_of_nodes(struct Cyc_PrattProver_Graph*g){
int n=0;
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
++ n;}}
# 591
return n;}
# 603 "pratt_prover.cyc"
static void Cyc_PrattProver_bellman_ford(int dummy,struct Cyc_PrattProver_Graph*g){
# 607
int n=Cyc_PrattProver_num_of_nodes(g);
# 609
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
node->signeddistFromS=0;
node->unsigneddistFromS=0;}}
# 615
{int i=0;for(0;i < n;++ i){
# 617
struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 624
if((unsigned)dv_unsigned==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_unsigned,uv_unsigned);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 627
if((!overflow &&(unsigned)dv_unsigned!=2147483648U)&& sum < dv_unsigned)dists->target->unsigneddistFromS=sum;
# 629
if(overflow && sum < 0)dists->target->unsigneddistFromS=-2147483648;}}}}{
# 631
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 637
if((unsigned)dv_signed==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_signed,uv_signed);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 640
if((!overflow &&(unsigned)dv_signed!=2147483648U)&& sum < dv_signed)dists->target->signeddistFromS=sum;
# 642
if(overflow && sum < 0)dists->target->signeddistFromS=-2147483648;}}}}}}}
# 650
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 657
if((unsigned)dv_unsigned==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_unsigned,uv_unsigned);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 660
if(!overflow && sum < dv_unsigned || overflow && sum < 0)
# 664
_throw(& Cyc_PrattProver_inconsistent);}}}}{
# 667
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 673
if((unsigned)dv_signed==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_signed,uv_signed);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
if(!overflow && sum < dv_signed || overflow && sum < 0)
# 679
_throw(& Cyc_PrattProver_inconsistent);}}}}}}
# 683
return;}
# 689
static void Cyc_PrattProver_initialize_dist_set(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g){
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
if(node==s)({void*_Tmp0=(void*)({struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct));_Tmp1->tag=2,_Tmp1->f1=0;_Tmp1;});node->shortest_path_info=_Tmp0;});else{
({void*_Tmp0=(void*)({struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct));_Tmp1->tag=0;_Tmp1;});node->shortest_path_info=_Tmp0;});}}}
# 694
return;}
# 703
static struct Cyc_PrattProver_Node*Cyc_PrattProver_extract_min(struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*current_shortest_node=0;
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
void*_Tmp0=node->shortest_path_info;int _Tmp1;switch(*((int*)_Tmp0)){case 0:
 goto _LL0;case 1:
 goto _LL0;default: _Tmp1=((struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct*)_Tmp0)->f1;{int d_new=_Tmp1;
# 710
if(current_shortest_node==0)current_shortest_node=node;else{
# 712
void*_Tmp2=current_shortest_node->shortest_path_info;int _Tmp3;if(*((int*)_Tmp2)==2){_Tmp3=((struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct*)_Tmp2)->f1;{int d_old=_Tmp3;
# 714
if(d_new <= d_old)current_shortest_node=node;
goto _LL7;}}else{
# 717
({struct _fat_ptr _Tmp4=(struct _fat_ptr)Cyc_aprintf(_tag_fat("current_shortest_node should always be NULL or &Current(d)\n",sizeof(char),60U),_tag_fat(0U,sizeof(void*),0));({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_Tmp4,_tag_fat(0U,sizeof(void*),0));});}_LL7:;}
# 720
goto _LL0;}}_LL0:;}}
# 723
return current_shortest_node;}
# 726
static void Cyc_PrattProver_relaxation(struct Cyc_PrattProver_Node*u,struct Cyc_PrattProver_Graph*g,int is_signed){
struct Cyc_PrattProver_Distance*dists;
int su;
{void*_Tmp0=u->shortest_path_info;int _Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_PrattProver_Shortest_PrattProver_ShortestPathInfo_struct*)_Tmp0)->f1;{int d=_Tmp1;
su=d;goto _LL0;}}else{
# 732
({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_aprintf(_tag_fat("current_shortest_node should always containe &Shortest(d)\n",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_Tmp2,_tag_fat(0U,sizeof(void*),0));});
goto _LL0;}_LL0:;}
# 735
if(is_signed)dists=u->signed_distances;else{
dists=u->unsigned_distances;}
for(1;dists!=0;dists=dists->next){
void*_Tmp0=dists->target->shortest_path_info;int _Tmp1;switch(*((int*)_Tmp0)){case 1:
 goto _LL5;case 2: _Tmp1=((struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct*)_Tmp0)->f1;{int sv=_Tmp1;
# 741
int uv=dists->dist;
struct _tuple13 _Tmp2=Cyc_PrattProver_is_signed_overflow(su,uv);int _Tmp3;int _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{int overflow=_Tmp4;int sum=_Tmp3;
if(!overflow && sum < sv)({void*_Tmp5=(void*)({struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct));_Tmp6->tag=2,_Tmp6->f1=sum;_Tmp6;});dists->target->shortest_path_info=_Tmp5;});
goto _LL5;}}default:  {
# 746
int uv=dists->dist;
struct _tuple13 _Tmp2=Cyc_PrattProver_is_signed_overflow(su,uv);int _Tmp3;int _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{int overflow=_Tmp4;int sum=_Tmp3;
if(!overflow)({void*_Tmp5=(void*)({struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct));_Tmp6->tag=2,_Tmp6->f1=sum;_Tmp6;});dists->target->shortest_path_info=_Tmp5;});
goto _LL5;}}}_LL5:;}
# 752
return;}
# 758
static void Cyc_PrattProver_set_shortest_dist_from_s(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g,int is_signed){
struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
void*_Tmp0=node->shortest_path_info;int _Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_PrattProver_Shortest_PrattProver_ShortestPathInfo_struct*)_Tmp0)->f1;{int d=_Tmp1;
Cyc_PrattProver_set_dist(g,s,node,d,is_signed);goto _LL0;}}else{
goto _LL0;}_LL0:;}}
# 768
static void Cyc_PrattProver_dijkstra(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g){
# 774
Cyc_PrattProver_initialize_dist_set(s,g);
while(1){
struct Cyc_PrattProver_Node*current_shortest_node=Cyc_PrattProver_extract_min(g);
# 778
if(current_shortest_node==0)break;else{
# 780
void*_Tmp0=current_shortest_node->shortest_path_info;int _Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=((struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct*)_Tmp0)->f1;{int d=_Tmp1;
# 783
({void*_Tmp2=(void*)({struct Cyc_PrattProver_Shortest_PrattProver_ShortestPathInfo_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_PrattProver_Shortest_PrattProver_ShortestPathInfo_struct));_Tmp3->tag=1,_Tmp3->f1=d;_Tmp3;});current_shortest_node->shortest_path_info=_Tmp2;});
goto _LL0;}}else{
# 786
({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_aprintf(_tag_fat("current_shortest_node should always contain &Current(d)\n",sizeof(char),57U),_tag_fat(0U,sizeof(void*),0));({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_Tmp2,_tag_fat(0U,sizeof(void*),0));});
goto _LL0;}_LL0:;}
# 790
Cyc_PrattProver_relaxation(current_shortest_node,g,0);}
# 792
Cyc_PrattProver_set_shortest_dist_from_s(s,g,0);
# 795
Cyc_PrattProver_initialize_dist_set(s,g);
while(1){
struct Cyc_PrattProver_Node*current_shortest_node=Cyc_PrattProver_extract_min(g);
# 799
if(current_shortest_node==0)break;else{
# 801
void*_Tmp0=current_shortest_node->shortest_path_info;int _Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=((struct Cyc_PrattProver_Current_PrattProver_ShortestPathInfo_struct*)_Tmp0)->f1;{int d=_Tmp1;
# 804
({void*_Tmp2=(void*)({struct Cyc_PrattProver_Shortest_PrattProver_ShortestPathInfo_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_PrattProver_Shortest_PrattProver_ShortestPathInfo_struct));_Tmp3->tag=1,_Tmp3->f1=d;_Tmp3;});current_shortest_node->shortest_path_info=_Tmp2;});
goto _LL5;}}else{
# 807
({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_aprintf(_tag_fat("current_shortest_node should always contain &Current(d)\n",sizeof(char),57U),_tag_fat(0U,sizeof(void*),0));({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_Tmp2,_tag_fat(0U,sizeof(void*),0));});
goto _LL5;}_LL5:;}
# 811
Cyc_PrattProver_relaxation(current_shortest_node,g,1);}
# 813
Cyc_PrattProver_set_shortest_dist_from_s(s,g,1);
return;}
# 824 "pratt_prover.cyc"
static void Cyc_PrattProver_johnson(int dummy,struct Cyc_PrattProver_Graph*g){
# 830
if(!g->changed)return;
# 834
Cyc_PrattProver_bellman_ford(0,g);
# 837
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 845
dists->dist=(uv_unsigned + du_unsigned)- dv_unsigned;}}{
# 847
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 852
dists->dist=(uv_signed + du_signed)- dv_signed;}}}}
# 857
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
Cyc_PrattProver_dijkstra(node,g);}}
# 862
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
dists->dist=(uv_unsigned + dv_unsigned)- du_unsigned;}}{
# 869
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 874
dists->dist=(uv_signed + dv_signed)- du_signed;}}}}
# 880
return;}
# 883
static void Cyc_PrattProver_set_distance(struct Cyc_PrattProver_Graph*g,void*it,void*jt,unsigned dist,int is_signed){
struct Cyc_PrattProver_Node*i=Cyc_PrattProver_term2node(g,it);
struct Cyc_PrattProver_Node*j=Cyc_PrattProver_term2node(g,jt);
Cyc_PrattProver_set_dist(g,i,j,(int)dist,is_signed);}
# 890
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_copy_distances(struct Cyc_PrattProver_Graph*newg,struct Cyc_PrattProver_Distance*ds){
struct Cyc_PrattProver_Distance*res=0;
for(1;ds!=0;ds=ds->next){
void*t=ds->target->rep;
res=({struct Cyc_PrattProver_Distance*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Distance));_Tmp0->next=res,({struct Cyc_PrattProver_Node*_Tmp1=Cyc_PrattProver_term2node(newg,t);_Tmp0->target=_Tmp1;}),_Tmp0->dist=ds->dist;_Tmp0;});}
# 896
return res;}
# 900
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
# 911
return first;}}
# 915
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_copy_graphs(struct Cyc_PrattProver_Graph*gopt){
struct Cyc_PrattProver_Graph*res=0;
for(1;gopt!=0;gopt=gopt->next){
++ Cyc_PrattProver_graphs_copied;{
struct Cyc_PrattProver_Graph*g=gopt;
struct Cyc_PrattProver_Graph*newg;newg=_cycalloc(sizeof(struct Cyc_PrattProver_Graph)),newg->next=res,newg->rows=0,newg->changed=g->changed;
res=newg;{
# 923
struct Cyc_PrattProver_Node*newrs=0;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
newrs=({struct Cyc_PrattProver_Node*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp0->next=newrs,_Tmp0->rep=rs->rep,_Tmp0->broken_as_signed=rs->broken_as_signed,_Tmp0->broken_as_unsigned=rs->broken_as_unsigned,_Tmp0->unsigned_distances=0,_Tmp0->signed_distances=0,_Tmp0->signeddistFromS=0,_Tmp0->unsigneddistFromS=0,({
void*_Tmp1=(void*)({struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_PrattProver_Infinity_PrattProver_ShortestPathInfo_struct));_Tmp2->tag=0;_Tmp2;});_Tmp0->shortest_path_info=_Tmp1;});_Tmp0;});}}
# 928
newrs=Cyc_PrattProver_revnodes(newrs);
newg->rows=newrs;{
# 931
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;(rs=rs->next,newrs=newrs->next)){
({struct Cyc_PrattProver_Distance*_Tmp0=Cyc_PrattProver_copy_distances(newg,rs->unsigned_distances);_check_null(newrs)->unsigned_distances=_Tmp0;});
({struct Cyc_PrattProver_Distance*_Tmp0=Cyc_PrattProver_copy_distances(newg,rs->unsigned_distances);newrs->signed_distances=_Tmp0;});}}}}}
# 936
return res;}
# 940
static int Cyc_PrattProver_graphs_change=0;
# 945
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_app_graphs(void(*f)(void*,struct Cyc_PrattProver_Graph*),void*e,struct Cyc_PrattProver_Graph*gs){
struct Cyc_PrattProver_Graph*prev=0;
{struct Cyc_PrattProver_Graph*g=gs;for(0;g!=0;g=g->next){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
f(e,g);
prev=g;
# 949
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_PrattProver_Inconsistent_exn_struct*)_Tmp2)->tag==Cyc_PrattProver_Inconsistent){
# 953
Cyc_PrattProver_graphs_change=1;
if(prev==0)
gs=g->next;else{
# 957
prev->next=g->next;}
# 959
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}}
# 962
return gs;}
# 965
static int Cyc_PrattProver_is_relation(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Gt:
 goto _LL4;case Cyc_Absyn_Lt: _LL4:
 goto _LL6;case Cyc_Absyn_Gte: _LL6:
 goto _LL8;case Cyc_Absyn_Lte: _LL8:
 return 1;default:
 return 0;};}
# 975
static unsigned Cyc_PrattProver_eval_binop(enum Cyc_Absyn_Primop p,unsigned c1,unsigned c2){
# 979
switch((int)p){case Cyc_Absyn_Plus:
 return c1 + c2;case Cyc_Absyn_Times:
 return c1 * c2;case Cyc_Absyn_Minus:
 return c1 - c2;case Cyc_Absyn_Div:
 return c1 / c2;case Cyc_Absyn_Mod:
 return c1 % c2;case Cyc_Absyn_Eq:
 return(unsigned)(c1==c2);case Cyc_Absyn_Neq:
 return(unsigned)(c1!=c2);case Cyc_Absyn_Bitand:
 return c1 & c2;case Cyc_Absyn_Bitor:
 return c1 | c2;case Cyc_Absyn_Bitxor:
 return c1 ^ c2;case Cyc_Absyn_Bitlshift:
 return c1 << c2;case Cyc_Absyn_Bitlrshift:
 return c1 >> c2;default:
({struct _fat_ptr _Tmp0=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("Invalid binop %s during constant evaluation",sizeof(char),44U),_tag_fat(_Tmp2,sizeof(void*),1));});({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_Tmp0,_tag_fat(0U,sizeof(void*),0));});};
# 994
return 0U;}
# 997
static unsigned Cyc_PrattProver_eval_unop(enum Cyc_Absyn_Primop p,unsigned c){
switch((int)p){case Cyc_Absyn_Not:
 return(unsigned)!((int)c);case Cyc_Absyn_Bitnot:
 return ~ c;case Cyc_Absyn_Plus:
 return c;case Cyc_Absyn_Minus:
 return - c;default:
({struct _fat_ptr _Tmp0=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("Invalid unop %s during constant evaluation",sizeof(char),43U),_tag_fat(_Tmp2,sizeof(void*),1));});({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_Tmp0,_tag_fat(0U,sizeof(void*),0));});};
# 1005
return 0U;}struct _tuple14{int f0;unsigned f1;};
# 1008
static struct _tuple14 Cyc_PrattProver_eq_node_const(struct Cyc_PrattProver_Node*n,struct Cyc_PrattProver_Node*z,int is_signed){
int*n2z=Cyc_PrattProver_lookup_dist(n,z,is_signed);
int*z2n=Cyc_PrattProver_lookup_dist(z,n,is_signed);
if((n2z!=0 && z2n!=0)&&*z2n==*n2z)return({struct _tuple14 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=(unsigned)*n2z;_Tmp0;});
return({struct _tuple14 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=2989U;_Tmp0;});}
# 1017
static struct _tuple14 Cyc_PrattProver_equal_node_const(struct Cyc_PrattProver_Node*n,struct Cyc_PrattProver_Node*z){
struct _tuple14 _Tmp0=Cyc_PrattProver_eq_node_const(n,z,1);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c=_Tmp1;
if(!ok)return Cyc_PrattProver_eq_node_const(n,z,0);else{
return({struct _tuple14 _Tmp3;_Tmp3.f0=ok,_Tmp3.f1=c;_Tmp3;});}}}
# 1024
static struct _tuple11 Cyc_PrattProver_subst_term_with_const(struct Cyc_PrattProver_Graph*g,void*t){
void*newterm=0;
{void*_Tmp0;void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;unsigned _Tmp4;switch(*((int*)t)){case 0: _Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned c=_Tmp4;
return({struct _tuple11 _Tmp5;_Tmp5.f0=1,_Tmp5.f1=c,_Tmp5.f2=0;_Tmp5;});}case 8: _Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp2;void*t2=_Tmp1;void*topt=_Tmp0;
# 1032
if(Cyc_PrattProver_is_relation(p))goto _LL0;{
struct _tuple11 _Tmp5=Cyc_PrattProver_subst_term_with_const(g,t1);void*_Tmp6;unsigned _Tmp7;int _Tmp8;_Tmp8=_Tmp5.f0;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{int ok1=_Tmp8;unsigned c1=_Tmp7;void*nt1=_Tmp6;
struct _tuple11 _Tmp9=Cyc_PrattProver_subst_term_with_const(g,t2);void*_TmpA;unsigned _TmpB;int _TmpC;_TmpC=_Tmp9.f0;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{int ok2=_TmpC;unsigned c2=_TmpB;void*nt2=_TmpA;
# 1037
if(nt1!=0){
newterm=Cyc_AssnDef_binop(p,nt1,t2,topt);
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1041
if(nt2!=0){
newterm=Cyc_AssnDef_binop(p,t1,nt2,topt);
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1045
if(ok1){
newterm=({enum Cyc_Absyn_Primop _TmpD=p;void*_TmpE=Cyc_AssnDef_uint(c1);void*_TmpF=t2;Cyc_AssnDef_binop(_TmpD,_TmpE,_TmpF,topt);});
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1049
if(ok2){
newterm=({enum Cyc_Absyn_Primop _TmpD=p;void*_TmpE=t1;void*_TmpF=Cyc_AssnDef_uint(c2);Cyc_AssnDef_binop(_TmpD,_TmpE,_TmpF,topt);});
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1053
if(ok1 && ok2)
return({struct _tuple11 _TmpD;_TmpD.f0=1,({unsigned _TmpE=Cyc_PrattProver_eval_binop(p,c1,c2);_TmpD.f1=_TmpE;}),_TmpD.f2=0;_TmpD;});
# 1056
goto _LL0;}}}}case 7: _Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp2;void*topt=_Tmp1;
# 1058
if((int)p==18)goto _LL0;{
struct _tuple11 _Tmp5=Cyc_PrattProver_subst_term_with_const(g,t1);unsigned _Tmp6;int _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{int ok1=_Tmp7;unsigned c1=_Tmp6;
if(ok1){
({struct Cyc_PrattProver_Graph*_Tmp8=g;void*_Tmp9=t;Cyc_PrattProver_add_eq(_Tmp8,_Tmp9,Cyc_AssnDef_uint(Cyc_PrattProver_eval_unop(p,c1)));});
return({struct _tuple11 _Tmp8;_Tmp8.f0=1,({unsigned _Tmp9=Cyc_PrattProver_eval_unop(p,c1);_Tmp8.f1=_Tmp9;}),_Tmp8.f2=0;_Tmp8;});}
# 1064
goto _LL0;}}}default:
 goto _LL0;}_LL0:;}
# 1067
if(t==newterm)newterm=0;{
struct Cyc_PrattProver_Node*n_node=Cyc_PrattProver_term2node(g,t);
struct Cyc_PrattProver_Node*z_node=({struct Cyc_PrattProver_Graph*_Tmp0=g;Cyc_PrattProver_term2node(_Tmp0,Cyc_AssnDef_zero());});
struct _tuple14 _Tmp0=Cyc_PrattProver_equal_node_const(n_node,z_node);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c=_Tmp1;
if(ok)return({struct _tuple11 _Tmp3;_Tmp3.f0=1,_Tmp3.f1=c,_Tmp3.f2=newterm;_Tmp3;});
return({struct _tuple11 _Tmp3;_Tmp3.f0=0,_Tmp3.f1=2989U,_Tmp3.f2=newterm;_Tmp3;});}}}
# 1083 "pratt_prover.cyc"
static void Cyc_PrattProver_congruence_close_graph(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=rs->rep,_Tmp1.f1=ts->rep;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;void*_Tmp4;void*_Tmp5;enum Cyc_Absyn_Primop _Tmp6;switch(*((int*)_Tmp0.f0)){case 8: if(*((int*)_Tmp0.f1)==8){_Tmp6=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp6)==(int)((enum Cyc_Absyn_Primop)_Tmp3)){enum Cyc_Absyn_Primop p1=_Tmp6;void*t11=_Tmp5;void*t12=_Tmp4;enum Cyc_Absyn_Primop p2=_Tmp3;void*t21=_Tmp2;void*t22=_Tmp1;
# 1088
if(!Cyc_PrattProver_equal_terms(g,t11,t21))goto _LL0;
if(!Cyc_PrattProver_equal_terms(g,t12,t22))goto _LL0;
Cyc_PrattProver_add_eq(g,rs->rep,ts->rep);
goto _LL0;}else{goto _LL7;}}else{goto _LL7;}case 7: if(*((int*)_Tmp0.f1)==7){_Tmp6=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;if((int)((enum Cyc_Absyn_Primop)_Tmp6)==(int)((enum Cyc_Absyn_Primop)_Tmp3)){enum Cyc_Absyn_Primop p1=_Tmp6;void*t1=_Tmp5;enum Cyc_Absyn_Primop p2=_Tmp3;void*t2=_Tmp4;
# 1093
if(!Cyc_PrattProver_equal_terms(g,t1,t2))goto _LL0;
Cyc_PrattProver_add_eq(g,rs->rep,ts->rep);
goto _LL0;}else{goto _LL7;}}else{goto _LL7;}case 9: if(*((int*)_Tmp0.f1)==9){_Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tp1=_Tmp5;void*t1=_Tmp4;void*tp2=_Tmp2;void*t2=_Tmp1;
# 1097
if(Cyc_Tcutil_typecmp(tp1,tp2)==0){
if(!Cyc_PrattProver_equal_terms(g,t1,t2))goto _LL0;
Cyc_PrattProver_add_eq(g,rs->rep,ts->rep);}
# 1101
goto _LL0;}}else{goto _LL7;}default: _LL7:
 goto _LL0;}_LL0:;}}}
# 1109
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_congruence_close_graphs(struct Cyc_PrattProver_Graph*gs){
return({(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;})(Cyc_PrattProver_congruence_close_graph,0,gs);}struct _tuple15{int f0;void*f1;int f2;};
# 1114
static struct _tuple15 Cyc_PrattProver_break_term(struct Cyc_PrattProver_Graph*g,void*t,int is_signed){
{unsigned _Tmp0;void*_Tmp1;if(*((int*)t)==8){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned c2=_Tmp0;
# 1117
if((int)c2 >= 0){
if(is_signed){
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=Cyc_PrattProver_term2node(g,t1);Cyc_PrattProver_lookup_dist(_Tmp2,({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());}),1);});
# 1121
if(dist!=0 &&*dist <= 2147483647 - (int)c2)
return({struct _tuple15 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;_Tmp2;});}else{
# 1126
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=Cyc_PrattProver_term2node(g,t1);Cyc_PrattProver_lookup_dist(_Tmp2,({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());}),0);});
if(dist!=0)
return({struct _tuple15 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;_Tmp2;});}}else{
# 1132
if(is_signed){
# 1134
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp2,Cyc_PrattProver_term2node(g,t1),1);});
if(dist!=0 &&*dist <= (int)(c2 - 2147483648U))
return({struct _tuple15 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;_Tmp2;});}else{
# 1140
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp2,Cyc_PrattProver_term2node(g,t1),0);});
if(dist!=0 &&*dist <= (int)c2)
return({struct _tuple15 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;_Tmp2;});}}
# 1146
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1148
 goto _LL0;}_LL0:;}
# 1150
return({struct _tuple15 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=t,_Tmp0.f2=0;_Tmp0;});}
# 1153
static void Cyc_PrattProver_break_term_in_graph(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
if(!rs->broken_as_signed){
struct _tuple15 _Tmp0=Cyc_PrattProver_break_term(g,rs->rep,1);int _Tmp1;void*_Tmp2;int _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int ok=_Tmp3;void*t1=_Tmp2;int c1=_Tmp1;
if(ok){
rs->broken_as_signed=1;
Cyc_PrattProver_set_distance(g,rs->rep,t1,(unsigned)c1,1);
if((unsigned)c1!=2147483648U)
Cyc_PrattProver_set_distance(g,t1,rs->rep,(unsigned)(- c1),1);}}}
# 1164
if(!rs->broken_as_unsigned){
struct _tuple15 _Tmp0=Cyc_PrattProver_break_term(g,rs->rep,0);int _Tmp1;void*_Tmp2;int _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int ok=_Tmp3;void*t1=_Tmp2;int c1=_Tmp1;
if(ok){
rs->broken_as_unsigned=1;
Cyc_PrattProver_set_distance(g,rs->rep,t1,(unsigned)c1,0);
if((unsigned)c1!=2147483648U)
Cyc_PrattProver_set_distance(g,t1,rs->rep,(unsigned)(- c1),0);}}}{
# 1173
struct _tuple11 _Tmp0=Cyc_PrattProver_subst_term_with_const(g,rs->rep);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c1=_Tmp1;
# 1175
if(ok){
void*_Tmp3=rs->rep;unsigned _Tmp4;void*_Tmp5;if(*((int*)_Tmp3)==8){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f3)==0){_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f2;_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f3)->f1;{void*t1=_Tmp5;unsigned c2=_Tmp4;
# 1178
rs->broken_as_signed=1;
rs->broken_as_unsigned=1;
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t1;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_uint(c1 - c2));});
goto _LL9;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC:
# 1183
 goto _LL9;}_LL9:;}}}}}
# 1189
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_break_term_in_graphs(struct Cyc_PrattProver_Graph*gs){
return({(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;})(Cyc_PrattProver_break_term_in_graph,0,gs);}
# 1198
static int Cyc_PrattProver_range_of_term(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*t){
struct Cyc_PrattProver_Node*zero_node=({struct Cyc_PrattProver_Graph*_Tmp0=g;Cyc_PrattProver_term2node(_Tmp0,Cyc_AssnDef_zero());});
# 1201
int*dist=Cyc_PrattProver_lookup_dist(zero_node,t,0);
if(dist!=0 &&(unsigned)*dist==2147483648U)
return 1;{
# 1205
int*dist=Cyc_PrattProver_lookup_dist(t,zero_node,0);
if(dist!=0 &&(unsigned)*dist <= 2147483647U)
return -1;{
# 1209
int*dist=Cyc_PrattProver_lookup_dist(zero_node,t,1);
if(dist!=0 &&*dist <= 0)
return 1;{
# 1213
int*dist=Cyc_PrattProver_lookup_dist(t,zero_node,1);
if(dist!=0 &&*dist <= -1)
return -1;
# 1217
return 0;}}}}
# 1228 "pratt_prover.cyc"
static void Cyc_PrattProver_associate_ud_sd_in_graph(int dummy,struct Cyc_PrattProver_Graph*g){
int range_of_src;int range_of_tgt;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
range_of_src=Cyc_PrattProver_range_of_term(g,rs);
if(range_of_src!=0){
{struct Cyc_PrattProver_Distance*uds=rs->unsigned_distances;for(0;uds!=0;uds=uds->next){
range_of_tgt=Cyc_PrattProver_range_of_term(g,uds->target);
if(range_of_src==range_of_tgt)
Cyc_PrattProver_set_distance(g,rs->rep,uds->target->rep,(unsigned)uds->dist,1);}}{
# 1239
struct Cyc_PrattProver_Distance*sds=rs->signed_distances;for(0;sds!=0;sds=sds->next){
range_of_tgt=Cyc_PrattProver_range_of_term(g,sds->target);
if(range_of_src==range_of_tgt)
Cyc_PrattProver_set_distance(g,rs->rep,sds->target->rep,(unsigned)sds->dist,0);}}}}}
# 1247
return;}
# 1250
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_associate_ud_sd_in_graphs(struct Cyc_PrattProver_Graph*gs){
return({(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;})(Cyc_PrattProver_associate_ud_sd_in_graph,0,gs);}
# 1257
static void Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*g,void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
++ Cyc_PrattProver_constraints_added;
{struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned c1=_Tmp2;unsigned c2=_Tmp1;
# 1261
switch((int)p){case Cyc_AssnDef_ULt:
 if(c1 < c2)return;goto _LL9;case Cyc_AssnDef_ULte:
 if(c1 <= c2)return;goto _LL9;case Cyc_AssnDef_SLt:
 if((int)c1 < (int)c2)return;goto _LL9;case Cyc_AssnDef_SLte:
 if((int)c1 <= (int)c2)return;goto _LL9;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));}_LL9:;
# 1271
_throw(& Cyc_PrattProver_inconsistent);}}else{_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;{unsigned c1=_Tmp2;
# 1310
switch((int)p){case Cyc_AssnDef_ULt:
# 1313
 if(c1==4294967295U)
# 1317
_throw(& Cyc_PrattProver_inconsistent);
# 1319
c1=c1 + 1U;
goto _LL23;case Cyc_AssnDef_ULte: _LL23:
# 1323
 if(c1 > 2147483647U)
# 1326
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,t2,2147483648U,0);});else{
# 1331
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)(-(int)c1),0);});}
# 1333
return;case Cyc_AssnDef_SLt:
# 1336
 if(c1==2147483647U)
# 1340
_throw(& Cyc_PrattProver_inconsistent);
# 1342
c1=c1 + 1U;
goto _LL27;case Cyc_AssnDef_SLte: _LL27:
# 1347
 if(c1==2147483648U)return;
return({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)(-(int)c1),1);});default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));};}}}else{if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned c2=_Tmp2;
# 1274
switch((int)p){case Cyc_AssnDef_ULt:
# 1277
 if(c2==0U)
# 1281
_throw(& Cyc_PrattProver_inconsistent);
# 1283
c2=c2 - 1U;
goto _LL18;case Cyc_AssnDef_ULte: _LL18:
# 1287
 if(c2 <= 2147483647U)
# 1289
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)((int)c2),0);});
# 1291
return;case Cyc_AssnDef_SLt:
# 1294
 if(c2==2147483648U)
# 1298
_throw(& Cyc_PrattProver_inconsistent);
# 1300
c2=c2 - 1U;
goto _LL1C;case Cyc_AssnDef_SLte: _LL1C:
# 1304
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)((int)c2),1);});
return;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));};}}else{
# 1351
goto _LL0;}}_LL0:;}
# 1355
switch((int)p){case Cyc_AssnDef_ULt:
# 1358
 return Cyc_PrattProver_set_distance(g,t1,t2,-1,0);case Cyc_AssnDef_ULte:
# 1361
 return Cyc_PrattProver_set_distance(g,t1,t2,0U,0);case Cyc_AssnDef_SLt:
# 1364
 return Cyc_PrattProver_set_distance(g,t1,t2,-1,1);case Cyc_AssnDef_SLte:
# 1367
 return Cyc_PrattProver_set_distance(g,t1,t2,0U,1);default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Vcgen:found bad primop in add_constraint",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));};}
# 1373
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
if(t1==t2)return;
Cyc_PrattProver_add_constraint(g,t1,5U,t2);
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t1,3U,t2);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);}struct _tuple16{void*f0;enum Cyc_AssnDef_Primreln f1;void*f2;};
# 1381
static void Cyc_PrattProver_add_prim(struct _tuple16*p,struct Cyc_PrattProver_Graph*g){
struct _tuple16 _Tmp0=*p;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
if((int)p==0)
Cyc_PrattProver_add_eq(g,t1,t2);else{
# 1386
Cyc_PrattProver_add_constraint(g,t1,p,t2);}}}char Cyc_PrattProver_TooLarge[9U]="TooLarge";struct Cyc_PrattProver_TooLarge_exn_struct{char*tag;};
# 1390
struct Cyc_PrattProver_TooLarge_exn_struct Cyc_PrattProver_too_large={Cyc_PrattProver_TooLarge};
# 1393
unsigned Cyc_PrattProver_max_paths=17U;
unsigned Cyc_PrattProver_max_paths_seen=0U;
# 1407 "pratt_prover.cyc"
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_cgraph(struct Cyc_PrattProver_Graph*gs,struct Cyc_Set_Set**already_seen,void*a,unsigned*total_paths){
# 1409
LOOP:
 if(gs==0)return gs;
# 1414
if(Cyc_Set_member(*already_seen,a)){++ Cyc_PrattProver_already_seen_hits;return gs;}
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(*already_seen,a);*already_seen=_Tmp0;});
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL0;case 1:
 gs=0;goto _LL0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1420
gs=Cyc_PrattProver_cgraph(gs,already_seen,a2,total_paths);
a=a1;
goto LOOP;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1424
unsigned n=Cyc_PrattProver_num_graphs(gs);
if(*_check_null(total_paths)> Cyc_PrattProver_max_paths)
_throw(& Cyc_PrattProver_too_large);
# 1430
*total_paths=*total_paths + n;
gs=({(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;})(Cyc_PrattProver_johnson,0,gs);{
struct Cyc_PrattProver_Graph*gs1=gs;
struct Cyc_PrattProver_Graph*gs2=Cyc_PrattProver_copy_graphs(gs);
struct Cyc_Set_Set*already_seen1=*already_seen;
struct Cyc_Set_Set*already_seen2=*already_seen;
# 1437
gs1=Cyc_PrattProver_cgraph(gs1,& already_seen1,a1,total_paths);
gs1=Cyc_PrattProver_associate_ud_sd_in_graphs(gs1);
gs1=Cyc_PrattProver_break_term_in_graphs(gs1);
# 1441
gs1=Cyc_PrattProver_congruence_close_graphs(gs1);
gs1=Cyc_PrattProver_associate_ud_sd_in_graphs(gs1);
# 1445
gs2=Cyc_PrattProver_cgraph(gs2,& already_seen2,a2,total_paths);
gs2=Cyc_PrattProver_associate_ud_sd_in_graphs(gs2);
gs2=Cyc_PrattProver_break_term_in_graphs(gs2);
# 1449
gs2=Cyc_PrattProver_congruence_close_graphs(gs2);
gs2=Cyc_PrattProver_associate_ud_sd_in_graphs(gs2);
# 1452
({struct Cyc_Set_Set*_Tmp3=Cyc_Set_intersect(already_seen1,already_seen2);*already_seen=_Tmp3;});
gs=Cyc_PrattProver_graph_append(gs1,gs2);
goto _LL0;}}case 2: if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1)->f1==0){if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){_Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t2=_Tmp2;
_Tmp2=t2;goto _LLC;}}else{goto _LLF;}}else{if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)->f1==0)goto _LLB;else{goto _LLD;}}else{goto _LLD;}}else{goto _LLF;}}}else{if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)->f1==0){_LLB: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_LLC: {void*t1=_Tmp2;
# 1459
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt==0 || Cyc_Tcutil_is_signed_type(topt)){
_Tmp2=t1;_Tmp1=Cyc_AssnDef_zero();goto _LLE;}
a=({void*_Tmp3=Cyc_AssnDef_zero();Cyc_AssnDef_ult(_Tmp3,t1);});
goto LOOP;}}else{goto _LLD;}}else{_LLD: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;_LLE: {void*t1=_Tmp2;void*t2=_Tmp1;
# 1467
a=({void*_Tmp3=({void*_Tmp4=Cyc_AssnDef_slt(t1,t2);Cyc_AssnDef_or(_Tmp4,Cyc_AssnDef_slt(t2,t1));});Cyc_AssnDef_and(_Tmp3,({void*_Tmp4=Cyc_AssnDef_ult(t1,t2);Cyc_AssnDef_or(_Tmp4,Cyc_AssnDef_ult(t2,t1));}));});
goto LOOP;}}}else{_LLF: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1476
struct _tuple16 env=({struct _tuple16 _Tmp3;_Tmp3.f0=t1,_Tmp3.f1=p,_Tmp3.f2=t2;_Tmp3;});
gs=({(struct Cyc_PrattProver_Graph*(*)(void(*)(struct _tuple16*,struct Cyc_PrattProver_Graph*),struct _tuple16*,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;})(Cyc_PrattProver_add_prim,& env,gs);
# 1481
goto _LL0;}}}default:
# 1484
 a=Cyc_AssnDef_reduce(a);
goto LOOP;}_LL0:;}
# 1487
return gs;}
# 1490
static struct Cyc_Set_Set*Cyc_PrattProver_get_pointer_terms(void*t,struct Cyc_Set_Set*s){
LOOP: {
void*topt=Cyc_AssnDef_get_term_type(t);
if((topt!=0 && Cyc_Tcutil_is_nullable_pointer_type(topt,0))&&
 Cyc_Tcutil_get_type_bound(topt)!=0)
s=Cyc_Set_insert(s,t);
{void*_Tmp0;void*_Tmp1;switch(*((int*)t)){case 9: _Tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp1;
t=t1;goto LOOP;}case 7: _Tmp1=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp1;
t=t1;goto LOOP;}case 8: _Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 1500
s=Cyc_PrattProver_get_pointer_terms(t1,s);
t=t2;goto LOOP;}default:
 goto _LL0;}_LL0:;}
# 1504
return s;}}
# 1507
static struct Cyc_Set_Set*Cyc_PrattProver_get_graph_pointer_terms(struct Cyc_PrattProver_Graph*g,struct Cyc_Set_Set*s){
# 1509
for(1;g!=0;g=g->next){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
s=Cyc_PrattProver_get_pointer_terms(rs->rep,s);}}
return s;}struct _tuple17{void*f0;void*f1;void*f2;};
# 1515
static void Cyc_PrattProver_add_ptr_info(struct _tuple17*env,struct Cyc_PrattProver_Graph*g){
struct _tuple17 _Tmp0=*env;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t=_Tmp3;void*tnumelts=_Tmp2;void*bnd=_Tmp1;
# 1518
int*dist=({struct Cyc_PrattProver_Node*_Tmp4=({struct Cyc_PrattProver_Graph*_Tmp5=g;Cyc_PrattProver_term2node(_Tmp5,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp4,Cyc_PrattProver_term2node(g,t),0);});
if(dist!=0 &&*dist <= -1){
Cyc_PrattProver_add_constraint(g,bnd,5U,tnumelts);
Cyc_PrattProver_add_constraint(g,bnd,3U,tnumelts);}}}
# 1525
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_add_ptr_type_info(void*t,struct Cyc_PrattProver_Graph*gs){
void*type=_check_null(Cyc_AssnDef_get_term_type(t));
struct Cyc_Absyn_Exp*e=Cyc_PrattProver_strip_cast(_check_null(Cyc_Tcutil_get_type_bound(type)));
void*bnd=Cyc_AssnDef_cnst(e);
void*tnumelts=Cyc_AssnDef_unop(18U,t,Cyc_Absyn_uint_type);
struct _tuple17 env=({struct _tuple17 _Tmp0;_Tmp0.f0=t,_Tmp0.f1=tnumelts,_Tmp0.f2=bnd;_Tmp0;});
return({(struct Cyc_PrattProver_Graph*(*)(void(*)(struct _tuple17*,struct Cyc_PrattProver_Graph*),struct _tuple17*,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;})(Cyc_PrattProver_add_ptr_info,& env,gs);}
# 1538
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_add_type_assns(struct Cyc_PrattProver_Graph*gs){
struct Cyc_Set_Set*ptrs=({struct Cyc_PrattProver_Graph*_Tmp0=gs;Cyc_PrattProver_get_graph_pointer_terms(_Tmp0,Cyc_Set_empty(Cyc_AssnDef_cmp_term));});
return({(struct Cyc_PrattProver_Graph*(*)(struct Cyc_PrattProver_Graph*(*)(void*,struct Cyc_PrattProver_Graph*),struct Cyc_Set_Set*,struct Cyc_PrattProver_Graph*))Cyc_Set_fold;})(Cyc_PrattProver_add_ptr_type_info,ptrs,gs);}
# 1546
static int Cyc_PrattProver_consistent(void*a){
# 1550
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
# 1552
Cyc_PrattProver_constraints_added=0;
Cyc_PrattProver_graphs_copied=0;
Cyc_PrattProver_max_lookup=0;
Cyc_PrattProver_already_seen_hits=0;{
struct Cyc_Set_Set*already_seen=Cyc_Set_empty(Cyc_AssnDef_assncmp);
unsigned total_paths=1U;
struct Cyc_PrattProver_Graph*gs=({struct Cyc_PrattProver_Graph*_Tmp2=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_Tmp2,& already_seen,a,& total_paths);});
# 1565
Cyc_PrattProver_graphs_change=1;
while(Cyc_PrattProver_graphs_change && gs!=0){
gs=({(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;})(Cyc_PrattProver_johnson,0,gs);
Cyc_PrattProver_graphs_change=0;
gs=Cyc_PrattProver_associate_ud_sd_in_graphs(gs);
gs=Cyc_PrattProver_add_type_assns(gs);
gs=Cyc_PrattProver_congruence_close_graphs(gs);
gs=Cyc_PrattProver_break_term_in_graphs(gs);
gs=Cyc_PrattProver_associate_ud_sd_in_graphs(gs);}{
# 1577
int _Tmp2=gs!=0;_npop_handler(0);return _Tmp2;}}
# 1552
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_PrattProver_TooLarge_exn_struct*)_Tmp2)->tag==Cyc_PrattProver_TooLarge)
# 1581
return 1;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 1586
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a){
void*b=({void*_Tmp0=ctxt;Cyc_AssnDef_and(_Tmp0,Cyc_AssnDef_not(a));});
return !Cyc_PrattProver_consistent(b);}

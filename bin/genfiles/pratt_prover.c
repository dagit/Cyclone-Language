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

# 128 "core.h"
 int Cyc_Core_ptrcmp(void*,void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};
# 913 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 1084
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 65
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 81
extern struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 116
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;void*f2;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Select_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Update_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Aggr_AssnDef_Term_struct{int tag;int f1;unsigned f2;struct Cyc_List_List*f3;void*f4;};struct Cyc_AssnDef_Proj_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Alloc_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;void*f4;void*f5;};struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Offseti_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Tagof_AssnDef_Term_struct{int tag;void*f1;};
# 139 "assndef.h"
int Cyc_AssnDef_termcmp(void*,void*);
int Cyc_AssnDef_termhash(void*);
# 142
extern struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 156
extern void*Cyc_AssnDef_zero (void);
# 160
extern void*Cyc_AssnDef_uint(unsigned);
extern void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
extern void*Cyc_AssnDef_select(void*,void*,void*);
extern void*Cyc_AssnDef_update(void*,void*,void*);
# 165
extern void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop,void*,void*);
extern void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
extern void*Cyc_AssnDef_cast(void*,void*);
extern void*Cyc_AssnDef_aggr(int,unsigned,struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_tagof_tm(void*);
extern void*Cyc_AssnDef_proj(void*,unsigned,void*);
extern void*Cyc_AssnDef_aggr_update(void*,unsigned,void*);
# 174
extern void*Cyc_AssnDef_offsetf(void*,unsigned,void*);
extern void*Cyc_AssnDef_offseti(void*,void*,void*);
# 178
extern void*Cyc_AssnDef_numelts_term(void*);
# 199
extern void*Cyc_AssnDef_get_term_type(void*);
# 212 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 224
int Cyc_AssnDef_assncmp(void*,void*);
# 226
extern struct _fat_ptr Cyc_AssnDef_assn2dag(void*);
# 231
extern void*Cyc_AssnDef_and(void*,void*);
extern void*Cyc_AssnDef_or(void*,void*);
extern void*Cyc_AssnDef_not(void*);
# 237
extern void*Cyc_AssnDef_slt(void*,void*);
# 239
extern void*Cyc_AssnDef_ult(void*,void*);
# 38 "tcutil.h"
int Cyc_Tcutil_is_signed_type(void*);
# 48
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
# 225
int Cyc_Tcutil_is_pure_exp(struct Cyc_Absyn_Exp*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 72 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 83 "flags.h"
extern int Cyc_Flags_print_unprovable;
# 148 "flags.h"
extern unsigned Cyc_Flags_max_vc_paths;struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 42 "xarray.h"
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 31 "pratt_prover.cyc"
extern void**Cyc_Hashtable_lookup_ptr_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc_PrattProver_Node;struct Cyc_PrattProver_Distance{struct Cyc_PrattProver_Distance*next;struct Cyc_PrattProver_Node*target;int dist;};struct _union_ShortestPathInfo_Infinity{int tag;int val;};struct _union_ShortestPathInfo_Shortest{int tag;int val;};struct _union_ShortestPathInfo_Current{int tag;int val;};union Cyc_PrattProver_ShortestPathInfo{struct _union_ShortestPathInfo_Infinity Infinity;struct _union_ShortestPathInfo_Shortest Shortest;struct _union_ShortestPathInfo_Current Current;};
# 180 "pratt_prover.cyc"
union Cyc_PrattProver_ShortestPathInfo Cyc_PrattProver_infinity={.Infinity={1,0}};struct Cyc_PrattProver_Node{struct Cyc_PrattProver_Node*next;void*rep;int broken_as_signed: 1;int broken_as_unsigned: 1;struct Cyc_PrattProver_Distance*unsigned_distances;struct Cyc_PrattProver_Distance*signed_distances;int signeddistFromS;int unsigneddistFromS;union Cyc_PrattProver_ShortestPathInfo shortest_path_info;};struct Cyc_PrattProver_Graph{struct Cyc_PrattProver_Graph*next;struct Cyc_PrattProver_Node*rows;struct Cyc_Hashtable_Table*map;int changed;};
# 207
static unsigned Cyc_PrattProver_num_graphs(struct Cyc_PrattProver_Graph*gs){
unsigned c=0U;
for(1;gs!=0;gs=gs->next){
++ c;}
return c;}
# 214
static struct Cyc_Hashtable_Table*Cyc_PrattProver_empty_map(unsigned n){
return Cyc_Hashtable_create((int)n,Cyc_Core_ptrcmp,Cyc_AssnDef_termhash);}
# 220
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_true_graph (void){
struct Cyc_PrattProver_Graph*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Graph));_Tmp0->next=0,_Tmp0->rows=0,({struct Cyc_Hashtable_Table*_Tmp1=Cyc_PrattProver_empty_map(17U);_Tmp0->map=_Tmp1;}),_Tmp0->changed=0;return _Tmp0;}
# 224
static int Cyc_PrattProver_constraints_added=0;
static int Cyc_PrattProver_graphs_copied=0;
static int Cyc_PrattProver_max_lookup=0;
static int Cyc_PrattProver_already_seen_hits=0;
# 229
static void Cyc_PrattProver_print_shortestpathinfo(union Cyc_PrattProver_ShortestPathInfo info){
{int _Tmp0;switch(info.Shortest.tag){case 1:
# 232
 Cyc_fprintf(Cyc_stderr,_tag_fat("not reachable from s\n",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;case 2: _Tmp0=info.Shortest.val;{int d=_Tmp0;
# 235
({struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)d;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("shortest distance from s is %d\n",sizeof(char),32U),_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL0;}default: _Tmp0=info.Current.val;{int d=_Tmp0;
# 238
({struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)d;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("current distance from s is %d\n",sizeof(char),31U),_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL0;}}_LL0:;}
# 241
return;}
# 244
void Cyc_PrattProver_print_graph(struct Cyc_PrattProver_Graph*g){
Cyc_fprintf(Cyc_stderr,_tag_fat("{",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
if(g->rows==0)Cyc_fprintf(Cyc_stderr,_tag_fat("<true>",sizeof(char),7U),_tag_fat(0U,sizeof(void*),0));else{
# 248
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
struct Cyc_PrattProver_Node*_Tmp0=rs;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->rep;_Tmp2=_Tmp0->unsigned_distances;_Tmp1=_Tmp0->signed_distances;{void*rep=_Tmp3;struct Cyc_PrattProver_Distance*uds=_Tmp2;struct Cyc_PrattProver_Distance*sds=_Tmp1;
struct _fat_ptr s=Cyc_AssnDef_term2string(rep);
# 258
for(1;uds!=0;uds=uds->next){
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)s;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_AssnDef_term2string(uds->target->rep);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)uds->dist;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("  %s - %s U<= %d\n ",sizeof(char),19U),_tag_fat(_Tmp7,sizeof(void*),3));});}
for(1;sds!=0;sds=sds->next){
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)s;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_AssnDef_term2string(sds->target->rep);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)sds->dist;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("  %s - %s S<= %d\n ",sizeof(char),19U),_tag_fat(_Tmp7,sizeof(void*),3));});}}}}
# 266
Cyc_fprintf(Cyc_stderr,_tag_fat("}\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));}
# 270
void Cyc_PrattProver_print_graphs(struct Cyc_PrattProver_Graph*g){
Cyc_fprintf(Cyc_stderr,_tag_fat("Graphs:-----------\n",sizeof(char),20U),_tag_fat(0U,sizeof(void*),0));
if(g==0)Cyc_fprintf(Cyc_stderr,_tag_fat("<false>\n",sizeof(char),9U),_tag_fat(0U,sizeof(void*),0));else{
# 274
int i=0;for(0;g!=0;(i ++,g=g->next)){
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)i;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("graph %d:\n",sizeof(char),11U),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_PrattProver_print_graph(g);}}
# 279
Cyc_fprintf(Cyc_stderr,_tag_fat("------------------\n",sizeof(char),20U),_tag_fat(0U,sizeof(void*),0));}char Cyc_PrattProver_Inconsistent[13U]="Inconsistent";struct Cyc_PrattProver_Inconsistent_exn_struct{char*tag;};
# 284
struct Cyc_PrattProver_Inconsistent_exn_struct Cyc_PrattProver_inconsistent={Cyc_PrattProver_Inconsistent};
# 287
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node(struct Cyc_PrattProver_Graph*,void*);
static void Cyc_PrattProver_set_distance(struct Cyc_PrattProver_Graph*,void*,void*,unsigned,int);
static void Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*,void*,enum Cyc_AssnDef_Primreln,void*);
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*,void*,void*);struct _tuple11{int f0;unsigned f1;void*f2;};
static struct _tuple11 Cyc_PrattProver_subst_term_with_const(struct Cyc_PrattProver_Graph*,void*);
# 294
static struct Cyc_Absyn_Exp*Cyc_PrattProver_strip_cast(struct Cyc_Absyn_Exp*e){
LOOP: {
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==14){_Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*tp2=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 298
{void*_Tmp3=Cyc_Absyn_compress(tp2);enum Cyc_Absyn_Size_of _Tmp4;if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==1){_Tmp4=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f2;{enum Cyc_Absyn_Size_of s=_Tmp4;
# 300
if((int)s==2 ||(int)s==3){
e=e2;goto LOOP;}
# 303
goto _LL5;}}else{goto _LL8;}}else{_LL8:
 goto _LL5;}_LL5:;}
# 306
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 309
return e;}
# 316
static void Cyc_PrattProver_add_type_info(struct Cyc_PrattProver_Graph*g,void*term){
# 320
void*z=Cyc_AssnDef_zero();
Cyc_PrattProver_set_distance(g,term,z,2147483647U,1);
Cyc_PrattProver_set_distance(g,z,term,0U,0);
{void*_Tmp0;switch(*((int*)term)){case 3: if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)term)->f1==Cyc_Absyn_Numelts){_Tmp0=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)term)->f2;{void*x=_Tmp0;
# 326
Cyc_PrattProver_set_distance(g,term,z,2147483646U,0);
# 328
Cyc_PrattProver_term2node(g,x);
goto _LL0;}}else{goto _LL7;}case 12: _Tmp0=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)term)->f3;{void*n=_Tmp0;
# 332
Cyc_PrattProver_add_constraint(g,z,4U,term);
({struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=Cyc_AssnDef_numelts_term(term);Cyc_PrattProver_add_constraint(_Tmp1,_Tmp2,5U,n);});
goto _LL0;}case 11:
# 337
 Cyc_PrattProver_add_constraint(g,z,4U,term);
goto _LL0;default: _LL7:
 goto _LL0;}_LL0:;}{
# 341
void*topt=Cyc_AssnDef_get_term_type(term);
if(topt!=0){
void*t=topt;
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(t);
if(eopt!=0 && Cyc_Tcutil_is_pure_exp(eopt)){
void*t1=Cyc_AssnDef_numelts_term(term);
# 348
struct Cyc_Absyn_Exp*e=Cyc_PrattProver_strip_cast(eopt);
# 350
void*t2=Cyc_AssnDef_cnst(e);
# 354
if(!Cyc_Tcutil_is_nullable_pointer_type(t,0)){
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);
Cyc_PrattProver_add_constraint(g,z,4U,term);}}{
# 360
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1==Cyc_Absyn_Unsigned){
# 362
Cyc_PrattProver_add_constraint(g,z,5U,term);
goto _LL9;}else{goto _LLE;}case 5: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*v=_Tmp1;
# 365
{void*_Tmp2=Cyc_Absyn_compress(v);void*_Tmp3;if(*((int*)_Tmp2)==8){_Tmp3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 367
void*c=Cyc_AssnDef_cnst(e);
Cyc_PrattProver_add_eq(g,term,c);
goto _LL10;}}else{_Tmp3=_Tmp2;{void*v2=_Tmp3;
# 371
void*c=Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp(v2,0U));
Cyc_PrattProver_add_eq(g,term,c);
goto _LL10;}}_LL10:;}
# 375
goto _LL9;}}else{goto _LLE;}default: goto _LLE;}else{_LLE:
 goto _LL9;}_LL9:;}}}}
# 383
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node(struct Cyc_PrattProver_Graph*g,void*t){
# 385
struct Cyc_PrattProver_Node**nopt=({struct Cyc_PrattProver_Node**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_PrattProver_Node**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_ptr_opt;_Tmp0;})(g->map,t);
if(nopt!=0)return*nopt;{
# 388
struct Cyc_PrattProver_Node**prev=& g->rows;
{struct Cyc_PrattProver_Node*r=g->rows;for(0;r!=0;r=r->next){
# 392
struct Cyc_PrattProver_Node*n=r;
void*rep=n->rep;
int c=Cyc_AssnDef_termcmp(t,rep);
if(c > 1)break;
if(c==0){
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*))Cyc_Hashtable_insert;_Tmp0;})(g->map,t,n);
return n;}
# 400
prev=& n->next;}}{
# 402
struct Cyc_PrattProver_Node*n;
{unsigned _Tmp0;void*_Tmp1;if(*((int*)t)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned c2=_Tmp0;
# 405
n=({struct Cyc_PrattProver_Node*_Tmp2=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp2->next=*prev,_Tmp2->rep=t,_Tmp2->broken_as_signed=0,_Tmp2->broken_as_unsigned=0,_Tmp2->unsigned_distances=0,_Tmp2->signed_distances=0,_Tmp2->signeddistFromS=0,_Tmp2->unsigneddistFromS=0,_Tmp2->shortest_path_info=Cyc_PrattProver_infinity;_Tmp2;});
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 408
 n=({struct Cyc_PrattProver_Node*_Tmp2=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp2->next=*prev,_Tmp2->rep=t,_Tmp2->broken_as_signed=1,_Tmp2->broken_as_unsigned=1,_Tmp2->unsigned_distances=0,_Tmp2->signed_distances=0,_Tmp2->signeddistFromS=0,_Tmp2->unsigneddistFromS=0,_Tmp2->shortest_path_info=Cyc_PrattProver_infinity;_Tmp2;});
goto _LL0;}_LL0:;}
# 411
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*))Cyc_Hashtable_insert;_Tmp0;})(g->map,t,n);
*prev=n;
Cyc_PrattProver_add_type_info(g,t);
return n;}}}
# 421
static int*Cyc_PrattProver_lookup_dist(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed){
static int zero=0;
if(source==target)return& zero;{
struct Cyc_PrattProver_Distance*ds=is_signed?source->signed_distances: source->unsigned_distances;
# 426
for(1;ds!=0;ds=ds->next){
if(ds->target==target)return& ds->dist;}
return 0;}}
# 434
static int Cyc_PrattProver_eq_nodes(struct Cyc_PrattProver_Node*r,struct Cyc_PrattProver_Node*t,int is_signed){
if(r==t)return 1;{
int*rt_dist=Cyc_PrattProver_lookup_dist(r,t,is_signed);
int*tr_dist=Cyc_PrattProver_lookup_dist(t,r,is_signed);
return((rt_dist!=0 &&*rt_dist==0)&& tr_dist!=0)&&*tr_dist==0;}}
# 442
static int Cyc_PrattProver_equal_nodes(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Node*r){
return Cyc_PrattProver_eq_nodes(s,r,0)|| Cyc_PrattProver_eq_nodes(s,r,1);}struct _tuple12{void*f0;void*f1;};
# 446
static int Cyc_PrattProver_equal_terms(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
if(t1==t2)return 1;{
struct _tuple11 _Tmp0=Cyc_PrattProver_subst_term_with_const(g,t1);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok1=_Tmp2;unsigned c1=_Tmp1;
struct _tuple11 _Tmp3=Cyc_PrattProver_subst_term_with_const(g,t2);unsigned _Tmp4;int _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{int ok2=_Tmp5;unsigned c2=_Tmp4;
if((ok1 && ok2)&& c1==c2)return 1;{
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node(g,t1);
struct Cyc_PrattProver_Node*n2=Cyc_PrattProver_term2node(g,t2);
if(Cyc_PrattProver_equal_nodes(n1,n2))return 1;
{struct _tuple12 _Tmp6=({struct _tuple12 _Tmp7;_Tmp7.f0=t1,_Tmp7.f1=t2;_Tmp7;});int _Tmp7;int _Tmp8;unsigned _Tmp9;unsigned _TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;enum Cyc_Absyn_Primop _Tmp10;void*_Tmp11;enum Cyc_Absyn_Primop _Tmp12;switch(*((int*)_Tmp6.f0)){case 3: if(*((int*)_Tmp6.f1)==3){_Tmp12=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f0)->f1;_Tmp11=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f0)->f2;_Tmp10=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f1)->f2;if((int)((enum Cyc_Absyn_Primop)_Tmp12)==(int)((enum Cyc_Absyn_Primop)_Tmp10)){enum Cyc_Absyn_Primop p11=_Tmp12;void*t11=_Tmp11;enum Cyc_Absyn_Primop p21=_Tmp10;void*t21=_TmpF;
# 456
return Cyc_PrattProver_equal_terms(g,t11,t21);}else{goto _LL19;}}else{goto _LL19;}case 4: if(*((int*)_Tmp6.f1)==4){_Tmp12=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f1;_Tmp11=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpF=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f3;_Tmp10=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpE=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f2;_TmpD=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp12)==(int)((enum Cyc_Absyn_Primop)_Tmp10)){enum Cyc_Absyn_Primop p11=_Tmp12;void*t11=_Tmp11;void*t12=_TmpF;enum Cyc_Absyn_Primop p21=_Tmp10;void*t21=_TmpE;void*t22=_TmpD;
# 458
return Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22);}else{goto _LL19;}}else{goto _LL19;}case 5: if(*((int*)_Tmp6.f1)==5){_Tmp11=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpE=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpD=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f1)->f2;{void*tp1=_Tmp11;void*t11=_TmpF;void*tp2=_TmpE;void*t21=_TmpD;
# 460
return Cyc_Tcutil_typecmp(tp1,tp2)==0 && Cyc_PrattProver_equal_terms(g,t11,t21);}}else{goto _LL19;}case 6: if(*((int*)_Tmp6.f1)==6){_Tmp11=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpE=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpD=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6.f1)->f2;{void*t11=_Tmp11;void*t12=_TmpF;void*t21=_TmpE;void*t22=_TmpD;
# 462
return Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22);}}else{goto _LL19;}case 7: if(*((int*)_Tmp6.f1)==7){_Tmp11=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpE=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f0)->f3;_TmpD=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpC=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f1)->f2;_TmpB=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f1)->f3;{void*t11=_Tmp11;void*t12=_TmpF;void*t13=_TmpE;void*t21=_TmpD;void*t22=_TmpC;void*t23=_TmpB;
# 464
return(Cyc_PrattProver_equal_terms(g,t11,t21)&&
 Cyc_PrattProver_equal_terms(g,t12,t22))&& Cyc_PrattProver_equal_terms(g,t13,t23);}}else{goto _LL19;}case 9: if(*((int*)_Tmp6.f1)==9){_Tmp11=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpA=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpF=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp9=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6.f1)->f2;{void*t1=_Tmp11;unsigned i1=_TmpA;void*t2=_TmpF;unsigned i2=_Tmp9;
# 467
return i1==i2 && Cyc_PrattProver_equal_terms(g,t1,t2);}}else{goto _LL19;}case 10: if(*((int*)_Tmp6.f1)==10){_Tmp11=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpA=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpF=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f0)->f3;_TmpE=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp9=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f1)->f2;_TmpD=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f1)->f3;{void*t1=_Tmp11;unsigned i1=_TmpA;void*v1=_TmpF;void*t2=_TmpE;unsigned i2=_Tmp9;void*v2=_TmpD;
# 469
return(i1==i2 && Cyc_PrattProver_equal_terms(g,t1,t2))&& Cyc_PrattProver_equal_terms(g,v1,v2);}}else{goto _LL19;}case 8: if(*((int*)_Tmp6.f1)==8){_Tmp8=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpA=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f0)->f2;_Tmp11=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f0)->f3;_Tmp7=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp9=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f1)->f2;_TmpF=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f1)->f3;{int is_union1=_Tmp8;unsigned tag1=_TmpA;struct Cyc_List_List*ts1=_Tmp11;int is_union2=_Tmp7;unsigned tag2=_Tmp9;struct Cyc_List_List*ts2=_TmpF;
# 471
for(1;ts1!=0 && ts2!=0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_PrattProver_equal_terms(g,(void*)ts1->hd,(void*)ts2->hd))return 0;}
return tag1==tag2 && is_union1==is_union2;}}else{goto _LL19;}case 15: if(*((int*)_Tmp6.f1)==15){_Tmp11=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp6.f1)->f1;{void*t1=_Tmp11;void*t2=_TmpF;
# 475
return Cyc_PrattProver_equal_terms(g,t1,t2);}}else{goto _LL19;}default: _LL19:
# 477
 return 0;};}
# 479
return 0;}}}}}
# 483
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_graph_append(struct Cyc_PrattProver_Graph*g1,struct Cyc_PrattProver_Graph*g2){
if(g1==0)return g2;
if(g2==0)return g1;{
struct Cyc_PrattProver_Graph*p=g1;
{struct Cyc_PrattProver_Graph*x=p->next;for(0;x!=0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}struct _tuple13{int f0;int f1;};
# 496
inline static struct _tuple13 Cyc_PrattProver_is_signed_overflow(int c1,int c2){
int sum=c1 + c2;
if((~(c1 ^ c2)& (sum ^ c1))>> 31){
# 500
if(sum > 0){struct _tuple13 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=-1;return _Tmp0;}else{
struct _tuple13 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=1;return _Tmp0;}}else{
# 503
struct _tuple13 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=sum;return _Tmp0;}}
# 508
static void Cyc_PrattProver_new_distance(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed,int d){
struct Cyc_PrattProver_Distance*dist;dist=_cycalloc(sizeof(struct Cyc_PrattProver_Distance)),dist->next=0,dist->target=target,dist->dist=d;
if(is_signed){
dist->next=source->signed_distances;
source->signed_distances=dist;}else{
# 514
dist->next=source->unsigned_distances;
source->unsigned_distances=dist;}}
# 603 "pratt_prover.cyc"
inline static void Cyc_PrattProver_set_dist(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*i,struct Cyc_PrattProver_Node*j,int dist,int is_signed){
# 610
if(i==j){
if(dist < 0 ||(unsigned)dist==2147483648U)_throw(& Cyc_PrattProver_inconsistent);
if(dist > 0)return;}{
# 614
int*ij_dist=Cyc_PrattProver_lookup_dist(i,j,is_signed);
# 620
if(ij_dist!=0){
if((unsigned)*ij_dist==2147483648U)return;
if((unsigned)dist!=2147483648U &&*ij_dist < dist)return;
*ij_dist=dist;}else{
# 625
Cyc_PrattProver_new_distance(i,j,is_signed,dist);}
# 627
g->changed=1;{
int*ji_dist=Cyc_PrattProver_lookup_dist(j,i,is_signed);
if(ji_dist!=0){
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(*ji_dist,dist);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
if(sum < 0)
# 635
_throw(& Cyc_PrattProver_inconsistent);}}
# 638
return;}}}
# 679 "pratt_prover.cyc"
static int Cyc_PrattProver_num_of_nodes(struct Cyc_PrattProver_Graph*g){
int n=0;
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
++ n;}}
# 684
return n;}
# 696 "pratt_prover.cyc"
static void Cyc_PrattProver_bellman_ford(int dummy,struct Cyc_PrattProver_Graph*g){
# 700
int n=Cyc_PrattProver_num_of_nodes(g);
# 702
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
node->signeddistFromS=0;
node->unsigneddistFromS=0;}}
# 709
{int i=0;for(0;i < n;++ i){
# 711
struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 718
if((unsigned)dv_unsigned==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_unsigned,uv_unsigned);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 721
if((!overflow &&(unsigned)dv_unsigned!=2147483648U)&& sum < dv_unsigned)dists->target->unsigneddistFromS=sum;
# 723
if(overflow && sum < 0)dists->target->unsigneddistFromS=-2147483648;}}}}{
# 725
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 731
if((unsigned)dv_signed==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_signed,uv_signed);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 734
if((!overflow &&(unsigned)dv_signed!=2147483648U)&& sum < dv_signed)dists->target->signeddistFromS=sum;
# 736
if(overflow && sum < 0)dists->target->signeddistFromS=-2147483648;}}}}}}}
# 744
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 751
if((unsigned)dv_unsigned==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_unsigned,uv_unsigned);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 754
if(!overflow && sum < dv_unsigned || overflow && sum < 0)
# 758
_throw(& Cyc_PrattProver_inconsistent);}}}}{
# 761
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 767
if((unsigned)dv_signed==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_signed,uv_signed);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
if(!overflow && sum < dv_signed || overflow && sum < 0)
# 773
_throw(& Cyc_PrattProver_inconsistent);}}}}}}
# 777
return;}
# 783
static void Cyc_PrattProver_initialize_dist_set(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g){
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
if(node==s)({union Cyc_PrattProver_ShortestPathInfo _Tmp0=({union Cyc_PrattProver_ShortestPathInfo _Tmp1;_Tmp1.Current.tag=3U,_Tmp1.Current.val=0;_Tmp1;});node->shortest_path_info=_Tmp0;});else{
node->shortest_path_info=Cyc_PrattProver_infinity;}}}
# 788
return;}
# 797
static struct Cyc_PrattProver_Node*Cyc_PrattProver_extract_min(struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*current_shortest_node=0;
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
union Cyc_PrattProver_ShortestPathInfo _Tmp0=node->shortest_path_info;int _Tmp1;switch(_Tmp0.Shortest.tag){case 1:
 goto _LL0;case 2:
 goto _LL0;default: _Tmp1=_Tmp0.Current.val;{int d_new=_Tmp1;
# 804
if(current_shortest_node==0)current_shortest_node=node;else{
# 806
union Cyc_PrattProver_ShortestPathInfo _Tmp2=current_shortest_node->shortest_path_info;int _Tmp3;if(_Tmp2.Current.tag==3){_Tmp3=_Tmp2.Current.val;{int d_old=_Tmp3;
# 808
if(d_new <= d_old)current_shortest_node=node;
goto _LL7;}}else{
# 811
({int(*_Tmp4)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;});struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_aprintf(_tag_fat("current_shortest_node should always be NULL or &Current(d)\n",sizeof(char),60U),_tag_fat(0U,sizeof(void*),0));_Tmp4(_Tmp5,_tag_fat(0U,sizeof(void*),0));});}_LL7:;}
# 814
goto _LL0;}}_LL0:;}}
# 817
return current_shortest_node;}
# 820
static void Cyc_PrattProver_relaxation(struct Cyc_PrattProver_Node*u,struct Cyc_PrattProver_Graph*g,int is_signed){
struct Cyc_PrattProver_Distance*dists;
int su;
{union Cyc_PrattProver_ShortestPathInfo _Tmp0=u->shortest_path_info;int _Tmp1;if(_Tmp0.Shortest.tag==2){_Tmp1=_Tmp0.Shortest.val;{int d=_Tmp1;
su=d;goto _LL0;}}else{
# 826
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;});struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_aprintf(_tag_fat("current_shortest_node should always containe &Shortest(d)\n",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));_Tmp2(_Tmp3,_tag_fat(0U,sizeof(void*),0));});
goto _LL0;}_LL0:;}
# 829
if(is_signed)dists=u->signed_distances;else{
dists=u->unsigned_distances;}
for(1;dists!=0;dists=dists->next){
union Cyc_PrattProver_ShortestPathInfo _Tmp0=dists->target->shortest_path_info;int _Tmp1;switch(_Tmp0.Current.tag){case 2:
 goto _LL5;case 3: _Tmp1=_Tmp0.Current.val;{int sv=_Tmp1;
# 835
int uv=dists->dist;
struct _tuple13 _Tmp2=Cyc_PrattProver_is_signed_overflow(su,uv);int _Tmp3;int _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{int overflow=_Tmp4;int sum=_Tmp3;
if(!overflow && sum < sv)({union Cyc_PrattProver_ShortestPathInfo _Tmp5=({union Cyc_PrattProver_ShortestPathInfo _Tmp6;_Tmp6.Current.tag=3U,_Tmp6.Current.val=sum;_Tmp6;});dists->target->shortest_path_info=_Tmp5;});
goto _LL5;}}default:  {
# 840
int uv=dists->dist;
struct _tuple13 _Tmp2=Cyc_PrattProver_is_signed_overflow(su,uv);int _Tmp3;int _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{int overflow=_Tmp4;int sum=_Tmp3;
if(!overflow)({union Cyc_PrattProver_ShortestPathInfo _Tmp5=({union Cyc_PrattProver_ShortestPathInfo _Tmp6;_Tmp6.Current.tag=3U,_Tmp6.Current.val=sum;_Tmp6;});dists->target->shortest_path_info=_Tmp5;});
goto _LL5;}}}_LL5:;}
# 846
return;}
# 852
static void Cyc_PrattProver_set_shortest_dist_from_s(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g,int is_signed){
struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
union Cyc_PrattProver_ShortestPathInfo _Tmp0=node->shortest_path_info;int _Tmp1;if(_Tmp0.Shortest.tag==2){_Tmp1=_Tmp0.Shortest.val;{int d=_Tmp1;
Cyc_PrattProver_set_dist(g,s,node,d,is_signed);goto _LL0;}}else{
goto _LL0;}_LL0:;}}
# 862
static void Cyc_PrattProver_dijkstra(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g){
# 868
Cyc_PrattProver_initialize_dist_set(s,g);
while(1){
{struct Cyc_PrattProver_Node*current_shortest_node=Cyc_PrattProver_extract_min(g);
# 872
if(current_shortest_node==0)break;else{
# 874
union Cyc_PrattProver_ShortestPathInfo _Tmp0=current_shortest_node->shortest_path_info;int _Tmp1;if(_Tmp0.Current.tag==3){_Tmp1=_Tmp0.Current.val;{int d=_Tmp1;
# 877
({union Cyc_PrattProver_ShortestPathInfo _Tmp2=({union Cyc_PrattProver_ShortestPathInfo _Tmp3;_Tmp3.Shortest.tag=2U,_Tmp3.Shortest.val=d;_Tmp3;});current_shortest_node->shortest_path_info=_Tmp2;});
goto _LL0;}}else{
# 880
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;});struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_aprintf(_tag_fat("current_shortest_node should always contain &Current(d)\n",sizeof(char),57U),_tag_fat(0U,sizeof(void*),0));_Tmp2(_Tmp3,_tag_fat(0U,sizeof(void*),0));});
goto _LL0;}_LL0:;}
# 884
Cyc_PrattProver_relaxation(current_shortest_node,g,0);}
# 870
1U;}
# 886
Cyc_PrattProver_set_shortest_dist_from_s(s,g,0);
# 889
Cyc_PrattProver_initialize_dist_set(s,g);
while(1){
{struct Cyc_PrattProver_Node*current_shortest_node=Cyc_PrattProver_extract_min(g);
# 893
if(current_shortest_node==0)break;else{
# 895
union Cyc_PrattProver_ShortestPathInfo _Tmp0=current_shortest_node->shortest_path_info;int _Tmp1;if(_Tmp0.Current.tag==3){_Tmp1=_Tmp0.Current.val;{int d=_Tmp1;
# 898
({union Cyc_PrattProver_ShortestPathInfo _Tmp2=({union Cyc_PrattProver_ShortestPathInfo _Tmp3;_Tmp3.Shortest.tag=2U,_Tmp3.Shortest.val=d;_Tmp3;});current_shortest_node->shortest_path_info=_Tmp2;});
goto _LL5;}}else{
# 901
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;});struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_aprintf(_tag_fat("current_shortest_node should always contain &Current(d)\n",sizeof(char),57U),_tag_fat(0U,sizeof(void*),0));_Tmp2(_Tmp3,_tag_fat(0U,sizeof(void*),0));});
goto _LL5;}_LL5:;}
# 905
Cyc_PrattProver_relaxation(current_shortest_node,g,1);}
# 891
1U;}
# 907
Cyc_PrattProver_set_shortest_dist_from_s(s,g,1);
return;}
# 918 "pratt_prover.cyc"
static void Cyc_PrattProver_johnson(int dummy,struct Cyc_PrattProver_Graph*g){
# 924
if(!g->changed)return;
# 932
Cyc_PrattProver_bellman_ford(0,g);
# 939
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
struct Cyc_PrattProver_Distance*pre_dists;struct Cyc_PrattProver_Distance*dists;
for((pre_dists=0,dists=node->unsigned_distances);dists!=0;(
pre_dists=dists,dists=dists->next)){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 949
dists->dist=(uv_unsigned + du_unsigned)- dv_unsigned;
# 951
if(dists->dist < 0){
# 954
if(pre_dists==0)
node->unsigned_distances=dists->next;else{
# 957
pre_dists->next=dists->next;}}}{
# 960
int du_signed=node->signeddistFromS;
for((pre_dists=0,dists=node->signed_distances);dists!=0;(
pre_dists=dists,dists=dists->next)){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 966
dists->dist=(uv_signed + du_signed)- dv_signed;
# 968
if(dists->dist < 0){
dists->dist=0;
if(pre_dists==0)
node->signed_distances=dists->next;else{
# 973
pre_dists->next=dists->next;}}}}}}
# 985
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
Cyc_PrattProver_dijkstra(node,g);}}
# 995
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
dists->dist=(uv_unsigned + dv_unsigned)- du_unsigned;}}{
# 1002
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 1007
dists->dist=(uv_signed + dv_signed)- du_signed;}}}}
# 1014
return;}
# 1017
static void Cyc_PrattProver_set_distance(struct Cyc_PrattProver_Graph*g,void*it,void*jt,unsigned dist,int is_signed){
struct Cyc_PrattProver_Node*i=Cyc_PrattProver_term2node(g,it);
struct Cyc_PrattProver_Node*j=Cyc_PrattProver_term2node(g,jt);
Cyc_PrattProver_set_dist(g,i,j,(int)dist,is_signed);}
# 1024
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_copy_distances(struct Cyc_PrattProver_Graph*newg,struct Cyc_PrattProver_Distance*_ds){
struct Cyc_PrattProver_Distance*res=0;
struct Cyc_PrattProver_Distance*ds=_ds;
for(1;ds!=0;ds=ds->next){
void*t=ds->target->rep;
res=({struct Cyc_PrattProver_Distance*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Distance));_Tmp0->next=res,({struct Cyc_PrattProver_Node*_Tmp1=Cyc_PrattProver_term2node(newg,t);_Tmp0->target=_Tmp1;}),_Tmp0->dist=ds->dist;_Tmp0;});}
# 1031
return res;}
# 1035
static struct Cyc_PrattProver_Node*Cyc_PrattProver_revnodes(struct Cyc_PrattProver_Node*n){
if(n==0)return 0;{
struct Cyc_PrattProver_Node*first=n;
struct Cyc_PrattProver_Node*second=n->next;
n->next=0;
while(second!=0){
{struct Cyc_PrattProver_Node*temp=second->next;
second->next=first;
first=second;
second=temp;}
# 1041
1U;}
# 1046
return first;}}
# 1050
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_copy_graphs(struct Cyc_PrattProver_Graph*_gopt){
struct Cyc_PrattProver_Graph*res=0;
struct Cyc_PrattProver_Graph*gopt=_gopt;
for(1;gopt!=0;gopt=gopt->next){
++ Cyc_PrattProver_graphs_copied;{
struct Cyc_PrattProver_Graph*g=gopt;
# 1057
struct Cyc_PrattProver_Graph*newg;newg=_cycalloc(sizeof(struct Cyc_PrattProver_Graph)),newg->next=res,newg->rows=0,({struct Cyc_Hashtable_Table*_Tmp0=Cyc_PrattProver_empty_map(17U);newg->map=_Tmp0;}),newg->changed=g->changed;
# 1059
res=newg;{
# 1061
struct Cyc_PrattProver_Node*newrs=0;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
newrs=({struct Cyc_PrattProver_Node*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp0->next=newrs,_Tmp0->rep=rs->rep,_Tmp0->broken_as_signed=rs->broken_as_signed,_Tmp0->broken_as_unsigned=rs->broken_as_unsigned,_Tmp0->unsigned_distances=0,_Tmp0->signed_distances=0,_Tmp0->signeddistFromS=0,_Tmp0->unsigneddistFromS=0,_Tmp0->shortest_path_info=Cyc_PrattProver_infinity;_Tmp0;});
# 1065
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*))Cyc_Hashtable_insert;_Tmp0;})(newg->map,rs->rep,newrs);}}
# 1067
newrs=Cyc_PrattProver_revnodes(newrs);
newg->rows=newrs;{
# 1070
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;(rs=rs->next,newrs=newrs->next)){
({struct Cyc_PrattProver_Distance*_Tmp0=Cyc_PrattProver_copy_distances(newg,rs->unsigned_distances);_check_null(newrs)->unsigned_distances=_Tmp0;});
({struct Cyc_PrattProver_Distance*_Tmp0=Cyc_PrattProver_copy_distances(newg,rs->unsigned_distances);newrs->signed_distances=_Tmp0;});}}}}}
# 1075
return res;}
# 1079
static int Cyc_PrattProver_graphs_change=0;
# 1084
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_app_graphs(void(*f)(void*,struct Cyc_PrattProver_Graph*),void*e,struct Cyc_PrattProver_Graph*gs){
struct Cyc_PrattProver_Graph*prev=0;
{struct Cyc_PrattProver_Graph*g=gs;for(0;g!=0;g=g->next){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
f(e,g);
prev=g;
# 1088
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_PrattProver_Inconsistent_exn_struct*)_Tmp2)->tag==Cyc_PrattProver_Inconsistent){
# 1092
Cyc_PrattProver_graphs_change=1;
if(prev==0)
gs=g->next;else{
# 1096
prev->next=g->next;}
# 1098
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}}
# 1101
return gs;}
# 1104
static int Cyc_PrattProver_is_relation(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Gt:
 goto _LL4;case Cyc_Absyn_Lt: _LL4:
 goto _LL6;case Cyc_Absyn_Gte: _LL6:
 goto _LL8;case Cyc_Absyn_Lte: _LL8:
 return 1;default:
 return 0;};}
# 1114
static unsigned Cyc_PrattProver_eval_binop(enum Cyc_Absyn_Primop p,unsigned c1,unsigned c2){
# 1118
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
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp1)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp1;});struct _fat_ptr _Tmp1=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)Cyc_Absynpp_prim2string(p);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("Invalid binop %s during constant evaluation",sizeof(char),44U),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0(_Tmp1,_tag_fat(0U,sizeof(void*),0));});};
# 1133
return 0U;}
# 1136
static unsigned Cyc_PrattProver_eval_unop(enum Cyc_Absyn_Primop p,unsigned c){
switch((int)p){case Cyc_Absyn_Not:
 return(unsigned)!((int)c);case Cyc_Absyn_Bitnot:
 return ~ c;case Cyc_Absyn_Plus:
 return c;case Cyc_Absyn_Minus:
 return - c;default:
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp1)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp1;});struct _fat_ptr _Tmp1=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)Cyc_Absynpp_prim2string(p);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("Invalid unop %s during constant evaluation",sizeof(char),43U),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0(_Tmp1,_tag_fat(0U,sizeof(void*),0));});};
# 1144
return 0U;}struct _tuple14{int f0;unsigned f1;};
# 1147
static struct _tuple14 Cyc_PrattProver_eq_node_const(struct Cyc_PrattProver_Node*n,struct Cyc_PrattProver_Node*z,int is_signed){
int*n2z=Cyc_PrattProver_lookup_dist(n,z,is_signed);
int*z2n=Cyc_PrattProver_lookup_dist(z,n,is_signed);
if((n2z!=0 && z2n!=0)&&*z2n==*n2z){struct _tuple14 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=(unsigned)*n2z;return _Tmp0;}{
struct _tuple14 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=2989U;return _Tmp0;}}
# 1156
static struct _tuple14 Cyc_PrattProver_equal_node_const(struct Cyc_PrattProver_Node*n,struct Cyc_PrattProver_Node*z){
struct _tuple14 _Tmp0=Cyc_PrattProver_eq_node_const(n,z,1);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c=_Tmp1;
if(!ok)return Cyc_PrattProver_eq_node_const(n,z,0);else{
struct _tuple14 _Tmp3;_Tmp3.f0=ok,_Tmp3.f1=c;return _Tmp3;}}}
# 1163
static struct _tuple11 Cyc_PrattProver_subst_term_with_const(struct Cyc_PrattProver_Graph*g,void*t){
void*newterm=0;
{void*_Tmp0;void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;unsigned _Tmp4;switch(*((int*)t)){case 0: _Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned c=_Tmp4;
struct _tuple11 _Tmp5;_Tmp5.f0=1,_Tmp5.f1=c,_Tmp5.f2=0;return _Tmp5;}case 4: _Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp2;void*t2=_Tmp1;void*topt=_Tmp0;
# 1171
if(Cyc_PrattProver_is_relation(p))goto _LL0;{
struct _tuple11 _Tmp5=Cyc_PrattProver_subst_term_with_const(g,t1);void*_Tmp6;unsigned _Tmp7;int _Tmp8;_Tmp8=_Tmp5.f0;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{int ok1=_Tmp8;unsigned c1=_Tmp7;void*nt1=_Tmp6;
struct _tuple11 _Tmp9=Cyc_PrattProver_subst_term_with_const(g,t2);void*_TmpA;unsigned _TmpB;int _TmpC;_TmpC=_Tmp9.f0;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{int ok2=_TmpC;unsigned c2=_TmpB;void*nt2=_TmpA;
# 1176
if(nt1!=0){
newterm=Cyc_AssnDef_binop(p,nt1,t2,topt);
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1180
if(nt2!=0){
newterm=Cyc_AssnDef_binop(p,t1,nt2,topt);
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1184
if(ok1){
newterm=({enum Cyc_Absyn_Primop _TmpD=p;void*_TmpE=Cyc_AssnDef_uint(c1);void*_TmpF=t2;Cyc_AssnDef_binop(_TmpD,_TmpE,_TmpF,topt);});
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1188
if(ok2){
newterm=({enum Cyc_Absyn_Primop _TmpD=p;void*_TmpE=t1;void*_TmpF=Cyc_AssnDef_uint(c2);Cyc_AssnDef_binop(_TmpD,_TmpE,_TmpF,topt);});
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1192
if(ok1 && ok2){
struct _tuple11 _TmpD;_TmpD.f0=1,({unsigned _TmpE=Cyc_PrattProver_eval_binop(p,c1,c2);_TmpD.f1=_TmpE;}),_TmpD.f2=0;return _TmpD;}
# 1195
goto _LL0;}}}}case 3: _Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp2;void*topt=_Tmp1;
# 1197
if((int)p==18)goto _LL0;{
struct _tuple11 _Tmp5=Cyc_PrattProver_subst_term_with_const(g,t1);unsigned _Tmp6;int _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{int ok1=_Tmp7;unsigned c1=_Tmp6;
if(ok1){
({struct Cyc_PrattProver_Graph*_Tmp8=g;void*_Tmp9=t;Cyc_PrattProver_add_eq(_Tmp8,_Tmp9,Cyc_AssnDef_uint(Cyc_PrattProver_eval_unop(p,c1)));});{
struct _tuple11 _Tmp8;_Tmp8.f0=1,({unsigned _Tmp9=Cyc_PrattProver_eval_unop(p,c1);_Tmp8.f1=_Tmp9;}),_Tmp8.f2=0;return _Tmp8;}}
# 1203
goto _LL0;}}}default:
 goto _LL0;}_LL0:;}
# 1206
if(t==newterm)newterm=0;{
struct Cyc_PrattProver_Node*n_node=Cyc_PrattProver_term2node(g,t);
struct Cyc_PrattProver_Node*z_node=({struct Cyc_PrattProver_Graph*_Tmp0=g;Cyc_PrattProver_term2node(_Tmp0,Cyc_AssnDef_zero());});
struct _tuple14 _Tmp0=Cyc_PrattProver_equal_node_const(n_node,z_node);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c=_Tmp1;
if(ok){struct _tuple11 _Tmp3;_Tmp3.f0=1,_Tmp3.f1=c,_Tmp3.f2=newterm;return _Tmp3;}{
struct _tuple11 _Tmp3;_Tmp3.f0=0,_Tmp3.f1=2989U,_Tmp3.f2=newterm;return _Tmp3;}}}}
# 1216
static int Cyc_PrattProver_better_term(void*t1,void*t2){
loop: {
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2)
# 1221
return(int)t1 < (int)t2;else{
# 1223
return 0;}}else{if(*((int*)_Tmp0.f1)==2)
# 1222
return 1;else{if(*((int*)_Tmp0.f0)==0)
# 1224
return 1;else{if(*((int*)_Tmp0.f1)==0)
return 0;else{if(*((int*)_Tmp0.f0)==11)
return 1;else{if(*((int*)_Tmp0.f1)==11)
return 0;else{if(*((int*)_Tmp0.f0)==12)
return 1;else{if(*((int*)_Tmp0.f1)==12)
return 0;else{switch(*((int*)_Tmp0.f0)){case 13: if(*((int*)_Tmp0.f1)==13){_Tmp4=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*t1a=_Tmp4;void*t2a=_Tmp3;
t1=t1a;t2=t2a;goto loop;}}else{goto _LL19;}case 14: if(*((int*)_Tmp0.f1)==14){_Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*t1a=_Tmp4;void*t1b=_Tmp3;void*t2a=_Tmp2;void*t2b=_Tmp1;
# 1232
if(!Cyc_PrattProver_better_term(t1a,t2a))return 0;
t1=t1b;t2=t2b;goto loop;}}else{goto _LL19;}case 15: if(*((int*)_Tmp0.f1)==15){_Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*t1a=_Tmp4;void*t2a=_Tmp3;
t1=t1a;t2=t2a;goto loop;}}else{goto _LL19;}default: _LL19:
 return 0;}}}}}}}}};}}
# 1240
static int Cyc_PrattProver_occurs(void*t1,void*t2){
loop:
 if(t1==t2)return 1;{
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)t2)){case 3: _Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t2)->f2;{void*s=_Tmp2;
t2=s;goto loop;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3;{void*s1=_Tmp2;void*s2=_Tmp1;
if(Cyc_PrattProver_occurs(t1,s1))return 1;t2=s2;goto loop;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t2)->f2;{void*s=_Tmp2;
t2=s;goto loop;}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t2)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t2)->f2;{void*s1=_Tmp2;void*s2=_Tmp1;
if(Cyc_PrattProver_occurs(t1,s1))return 1;t2=s2;goto loop;}case 7: _Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t2)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t2)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t2)->f3;{void*s1=_Tmp2;void*s2=_Tmp1;void*s3=_Tmp0;
if(Cyc_PrattProver_occurs(t1,s1)|| Cyc_PrattProver_occurs(t2,s2))return 1;
t2=s3;goto loop;}case 8: _Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t2)->f3;{struct Cyc_List_List*ss=_Tmp2;
# 1251
for(1;ss!=0;ss=ss->tl){if(Cyc_PrattProver_occurs(t1,(void*)ss->hd))return 1;}
return 0;}case 9: _Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t2)->f1;{void*s=_Tmp2;
t2=s;goto loop;}case 10: _Tmp2=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t2)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t2)->f3;{void*s1=_Tmp2;void*s2=_Tmp1;
# 1255
if(Cyc_PrattProver_occurs(t1,s1))return 1;t2=s2;goto loop;}case 11: _Tmp2=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t2)->f2;{void*s=_Tmp2;
t2=s;goto loop;}case 12: _Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t2)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t2)->f4;{void*s1=_Tmp2;void*s2=_Tmp1;
# 1258
if(Cyc_PrattProver_occurs(t1,s1))return 1;t2=s2;goto loop;}case 13: _Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t2)->f1;{void*s=_Tmp2;
t2=s;goto loop;}case 14: _Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t2)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t2)->f2;{void*s1=_Tmp2;void*s2=_Tmp1;
if(Cyc_PrattProver_occurs(t1,s1))return 1;t2=s2;goto loop;}case 15: _Tmp2=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t2)->f1;{void*s=_Tmp2;
t2=s;goto loop;}default:
 return 0;};}}
# 1266
static void*Cyc_PrattProver_lookup_eq(struct Cyc_Hashtable_Table*eq,void*t){
while(1){
{void**topt=Cyc_Hashtable_lookup_opt(eq,t);
if(topt==0 ||*topt==t)return t;
t=*topt;}
# 1268
1U;}}
# 1274
static void Cyc_PrattProver_subst_close_graph(int dummy,struct Cyc_PrattProver_Graph*g){
# 1279
struct Cyc_Hashtable_Table*eq=Cyc_Hashtable_create(33,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);
int changed=0;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
void*t1=rs->rep;
void*best=t1;
# 1285
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
void*t2=ts->rep;
if((Cyc_PrattProver_better_term(t2,best)&& Cyc_PrattProver_equal_nodes(rs,ts))&& !Cyc_PrattProver_occurs(t1,t2))
best=t2;}}
# 1290
if(best!=t1){
changed=1;
Cyc_Hashtable_insert(eq,t1,best);}}}
# 1295
if(!changed)return;{
# 1297
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
void*t=rs->rep;
int _Tmp0;void*_Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_Primop _Tmp5;switch(*((int*)t)){case 3: _Tmp5=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp5;void*t1=_Tmp4;void*type=_Tmp3;
# 1301
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
if(t1!=newt1)({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_unop(p,newt1,type));});
goto _LL0;}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;unsigned i=_Tmp2;void*type=_Tmp3;
# 1305
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
if(t1!=newt1)({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_proj(newt1,i,type));});
goto _LL0;}case 4: _Tmp5=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp5;void*t1=_Tmp4;void*t2=_Tmp3;void*type=_Tmp1;
# 1309
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
void*newt2=Cyc_PrattProver_lookup_eq(eq,t2);
if(t1!=newt1 || t2!=newt2)
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_binop(p,newt1,newt2,type));});
goto _LL0;}case 6: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*type=_Tmp1;
# 1315
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
void*newt2=Cyc_PrattProver_lookup_eq(eq,t2);
if(t1!=newt1 || t2!=newt2)({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_select(newt1,newt2,type));});
goto _LL0;}case 7: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp1;
# 1320
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
void*newt2=Cyc_PrattProver_lookup_eq(eq,t2);
void*newt3=Cyc_PrattProver_lookup_eq(eq,t3);
if((t1!=newt1 || t2!=newt2)|| t3!=newt3)
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_update(newt1,newt2,newt3));});
goto _LL0;}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*type=_Tmp4;void*t1=_Tmp3;
# 1327
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
if(t1!=newt1)({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_cast(type,newt1));});
goto _LL0;}case 8: _Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp3=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int is_union=_Tmp0;unsigned tag=_Tmp2;struct Cyc_List_List*terms=_Tmp4;void*topt=_Tmp3;
# 1331
struct Cyc_List_List*new_terms=0;
int change=0;
{struct Cyc_List_List*ts=terms;for(0;ts!=0;ts=ts->tl){
void*t=(void*)ts->hd;
void*newt=Cyc_PrattProver_lookup_eq(eq,t);
if(t!=newt)change=1;
new_terms=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=newt,_Tmp6->tl=new_terms;_Tmp6;});}}
# 1339
if(change)
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,({int _Tmp8=is_union;unsigned _Tmp9=tag;struct Cyc_List_List*_TmpA=Cyc_List_imp_rev(new_terms);Cyc_AssnDef_aggr(_Tmp8,_Tmp9,_TmpA,topt);}));});
# 1342
goto _LL0;}case 10: _Tmp4=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;unsigned i=_Tmp2;void*t2=_Tmp3;
# 1344
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
void*newt2=Cyc_PrattProver_lookup_eq(eq,t2);
if(t1!=newt1 || t2!=newt2)
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_aggr_update(newt1,i,newt2));});
goto _LL0;}case 13: _Tmp4=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;unsigned i=_Tmp2;void*topt=_Tmp3;
# 1350
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
if(t1!=newt1)({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_offsetf(newt1,i,topt));});
goto _LL0;}case 14: _Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*topt=_Tmp1;
# 1354
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
void*newt2=Cyc_PrattProver_lookup_eq(eq,t2);
if(t1!=newt1 || t2!=newt2)
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_offseti(newt1,newt2,topt));});
goto _LL0;}case 15: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp4;
# 1360
void*newt1=Cyc_PrattProver_lookup_eq(eq,t1);
if(t1!=newt1)({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_tagof_tm(newt1));});
goto _LL0;}default:
 goto _LL0;}_LL0:;}}}
# 1368
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_subst_close_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_subst_close_graph,0,gs);}
# 1374
static void Cyc_PrattProver_congruence_close_graph(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
void*t=rs->rep;
void*_Tmp0;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;enum Cyc_Absyn_Primop _Tmp4;switch(*((int*)t)){case 3: _Tmp4=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp4;void*t1=_Tmp3;void*type=_Tmp2;
# 1379
struct Cyc_PrattProver_Node*t1node=Cyc_PrattProver_term2node(g,t1);
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
if(t1node!=ts && Cyc_PrattProver_equal_nodes(ts,t1node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_unop(p,ts->rep,type));});}}
goto _LL0;}case 9: _Tmp3=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp3;unsigned i=_Tmp1;void*type=_Tmp2;
# 1385
struct Cyc_PrattProver_Node*t1node=Cyc_PrattProver_term2node(g,t1);
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
if(t1node!=ts && Cyc_PrattProver_equal_nodes(ts,t1node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_proj(ts->rep,i,type));});}}
goto _LL0;}case 4: _Tmp4=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp4;void*t1=_Tmp3;void*t2=_Tmp2;void*type=_Tmp0;
# 1393
struct Cyc_PrattProver_Node*t1node=Cyc_PrattProver_term2node(g,t1);
struct Cyc_PrattProver_Node*t2node=Cyc_PrattProver_term2node(g,t2);
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
if(t1node!=ts && Cyc_PrattProver_equal_nodes(ts,t1node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_binop(p,ts->rep,t2,type));});
if(t2node!=ts && Cyc_PrattProver_equal_nodes(ts,t2node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_binop(p,t1,ts->rep,type));});}}
# 1401
goto _LL0;}case 6: _Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp3;void*t2=_Tmp2;void*type=_Tmp0;
# 1405
struct Cyc_PrattProver_Node*t1node=Cyc_PrattProver_term2node(g,t1);
struct Cyc_PrattProver_Node*t2node=Cyc_PrattProver_term2node(g,t2);
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
if(t1node!=ts && Cyc_PrattProver_equal_nodes(ts,t1node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_select(ts->rep,t2,type));});
if(t2node!=ts && Cyc_PrattProver_equal_nodes(ts,t2node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_select(t1,ts->rep,type));});}}
# 1413
goto _LL0;}case 7: _Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp3;void*t2=_Tmp2;void*t3=_Tmp0;
# 1417
struct Cyc_PrattProver_Node*t1node=Cyc_PrattProver_term2node(g,t1);
struct Cyc_PrattProver_Node*t2node=Cyc_PrattProver_term2node(g,t2);
struct Cyc_PrattProver_Node*t3node=Cyc_PrattProver_term2node(g,t3);
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
if(t1node!=ts && Cyc_PrattProver_equal_nodes(ts,t1node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_update(ts->rep,t2,t3));});
if(t2node!=ts && Cyc_PrattProver_equal_nodes(ts,t2node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_update(t1,ts->rep,t3));});
if(t3node!=ts && Cyc_PrattProver_equal_nodes(ts,t3node))
({struct Cyc_PrattProver_Graph*_Tmp5=g;void*_Tmp6=t;Cyc_PrattProver_add_eq(_Tmp5,_Tmp6,Cyc_AssnDef_update(t1,t2,ts->rep));});}}
# 1428
goto _LL0;}default:
 goto _LL0;}_LL0:;}}
# 1435
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_congruence_close_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_congruence_close_graph,0,gs);}
# 1442
static void Cyc_PrattProver_simplify_matrix(unsigned Rows,unsigned Columns,struct _fat_ptr M){
unsigned c=0U;for(0;c < Columns;++ c){
# 1446
unsigned r;
for(r=0U;r < Rows;++ r){
if(*((int*)_check_fat_subscript(*((struct _fat_ptr*)_check_fat_subscript(M,sizeof(struct _fat_ptr),(int)r)),sizeof(int),(int)c))!=0)break;}
# 1451
if(r >= Rows)continue;{
# 1454
unsigned r2=0U;for(0;r2 < Rows;++ r2){
if(r2==r ||*((int*)_check_fat_subscript(*((struct _fat_ptr*)_check_fat_subscript(M,sizeof(struct _fat_ptr),(int)r2)),sizeof(int),(int)c))==0)continue;{
int kr2=((int*)((struct _fat_ptr*)M.curr)[(int)r2].curr)[(int)c];
int kr=*((int*)_check_fat_subscript(*((struct _fat_ptr*)_check_fat_subscript(M,sizeof(struct _fat_ptr),(int)r)),sizeof(int),(int)c));
# 1459
unsigned i=0U;for(0;i < Columns + 1U;++ i){
({int _Tmp0=({int _Tmp1=kr * *((int*)_check_fat_subscript(*((struct _fat_ptr*)_check_fat_subscript(M,sizeof(struct _fat_ptr),(int)r2)),sizeof(int),(int)i));_Tmp1 - kr2 * *((int*)_check_fat_subscript(((struct _fat_ptr*)M.curr)[(int)r],sizeof(int),(int)i));});((int*)((struct _fat_ptr*)M.curr)[(int)r2].curr)[(int)i]=_Tmp0;});}}}}}}
# 1466
static void Cyc_PrattProver_print_matrix(unsigned R,unsigned C,struct Cyc_Xarray_Xarray*ts,struct _fat_ptr M){
# 1468
unsigned i=0U;for(0;i < R;++ i){
int found_column=0;
{unsigned j=0U;for(0;j < C;++ j){
int v=*((int*)_check_fat_subscript(*((struct _fat_ptr*)_check_fat_subscript(M,sizeof(struct _fat_ptr),(int)i)),sizeof(int),(int)j));
if(v==0)continue;
if(found_column)Cyc_fprintf(Cyc_stderr,_tag_fat(" + ",sizeof(char),4U),_tag_fat(0U,sizeof(void*),0));
found_column=1;
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)v;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_AssnDef_term2string(Cyc_Xarray_get(ts,(int)j));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("%d*%s",sizeof(char),6U),_tag_fat(_Tmp2,sizeof(void*),2));});}}
# 1477
if(!found_column)Cyc_fprintf(Cyc_stderr,_tag_fat("0",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)*((int*)_check_fat_subscript(*((struct _fat_ptr*)_check_fat_subscript(M,sizeof(struct _fat_ptr),(int)i)),sizeof(int),(int)C));_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat(" == %d\n",sizeof(char),8U),_tag_fat(_Tmp1,sizeof(void*),1));});}}struct _tuple15{int f0;void*f1;};
# 1483
static void Cyc_PrattProver_print_poly(struct Cyc_List_List*ts,unsigned c){
if(ts==0)Cyc_fprintf(Cyc_stderr,_tag_fat("0",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));else{
# 1486
for(1;ts!=0;ts=ts->tl){
struct _tuple15 _Tmp0=*((struct _tuple15*)ts->hd);void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int i=_Tmp2;void*t=_Tmp1;
({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)i;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_AssnDef_term2string(t);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%d*%s",sizeof(char),6U),_tag_fat(_Tmp5,sizeof(void*),2));});
if(ts->tl!=0)Cyc_fprintf(Cyc_stderr,_tag_fat(" + ",sizeof(char),4U),_tag_fat(0U,sizeof(void*),0));}}}
# 1492
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)((int)(- c));_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat(" == %d\n",sizeof(char),8U),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 1616 "pratt_prover.cyc"
static void Cyc_PrattProver_equality_close_graph(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
void*rep1=rs->rep;
int _Tmp0;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_Primop _Tmp5;switch(*((int*)rep1)){case 4: _Tmp5=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)rep1)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)rep1)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)rep1)->f3;{enum Cyc_Absyn_Primop p1=_Tmp5;void*t11=_Tmp4;void*t12=_Tmp3;
# 1621
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;void*_Tmp7;void*_Tmp8;enum Cyc_Absyn_Primop _Tmp9;if(*((int*)_Tmp6)==4){_Tmp9=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6)->f1;_Tmp8=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6)->f3;if((int)p1==(int)((enum Cyc_Absyn_Primop)_Tmp9)){enum Cyc_Absyn_Primop p2=_Tmp9;void*t21=_Tmp8;void*t22=_Tmp7;
# 1624
if(Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22))
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1627
continue;}else{goto _LL18;}}else{_LL18:
 continue;};}}
# 1630
goto _LL0;}case 3: _Tmp5=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)rep1)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)rep1)->f2;{enum Cyc_Absyn_Primop p1=_Tmp5;void*t1=_Tmp4;
# 1632
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;void*_Tmp7;enum Cyc_Absyn_Primop _Tmp8;if(*((int*)_Tmp6)==3){_Tmp8=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6)->f2;if((int)p1==(int)((enum Cyc_Absyn_Primop)_Tmp8)){enum Cyc_Absyn_Primop p2=_Tmp8;void*t2=_Tmp7;
# 1635
if(Cyc_PrattProver_equal_terms(g,t1,t2))
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1638
continue;}else{goto _LL1D;}}else{_LL1D:
 continue;};}}
# 1641
goto _LL0;}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)rep1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)rep1)->f2;{void*tp1=_Tmp4;void*t1=_Tmp3;
# 1643
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp6)==5){_Tmp8=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6)->f2;{void*tp2=_Tmp8;void*t2=_Tmp7;
# 1646
if(Cyc_Tcutil_typecmp(tp1,tp2)==0 && Cyc_PrattProver_equal_terms(g,t1,t2))
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1649
continue;}}else{
continue;};}}
# 1652
goto _LL0;}case 6: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)rep1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)rep1)->f2;{void*t11=_Tmp4;void*t12=_Tmp3;
# 1654
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp6)==6){_Tmp8=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6)->f2;{void*t21=_Tmp8;void*t22=_Tmp7;
# 1657
if(Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22))
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1660
continue;}}else{
continue;};}}
# 1663
goto _LL0;}case 7: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)rep1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)rep1)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)rep1)->f3;{void*t11=_Tmp4;void*t12=_Tmp3;void*t13=_Tmp2;
# 1665
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;void*_Tmp7;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp6)==7){_Tmp9=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6)->f1;_Tmp8=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6)->f3;{void*t21=_Tmp9;void*t22=_Tmp8;void*t23=_Tmp7;
# 1668
if((Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22))&&
 Cyc_PrattProver_equal_terms(g,t13,t23))
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1672
continue;}}else{
continue;};}}
# 1675
goto _LL0;}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)rep1)->f1;_Tmp1=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)rep1)->f2;{void*t1=_Tmp4;unsigned i1=_Tmp1;
# 1677
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;unsigned _Tmp7;void*_Tmp8;if(*((int*)_Tmp6)==9){_Tmp8=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6)->f1;_Tmp7=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6)->f2;{void*t2=_Tmp8;unsigned i2=_Tmp7;
# 1680
if(i1==i2 && Cyc_PrattProver_equal_terms(g,t1,t2))
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1683
continue;}}else{
continue;};}}
# 1686
goto _LL0;}case 10: _Tmp4=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)rep1)->f1;_Tmp1=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)rep1)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)rep1)->f3;{void*t11=_Tmp4;unsigned i1=_Tmp1;void*t12=_Tmp3;
# 1688
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;void*_Tmp7;unsigned _Tmp8;void*_Tmp9;if(*((int*)_Tmp6)==10){_Tmp9=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6)->f1;_Tmp8=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6)->f3;if(i1==(unsigned)_Tmp8){void*t21=_Tmp9;unsigned i2=_Tmp8;void*t22=_Tmp7;
# 1691
if(Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22))
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1694
continue;}else{goto _LL36;}}else{_LL36:
 continue;};}}
# 1697
goto _LL0;}case 8: _Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)rep1)->f1;_Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)rep1)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)rep1)->f3;{int is_union1=_Tmp0;unsigned tag1=_Tmp1;struct Cyc_List_List*ts1=_Tmp4;
# 1699
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;void*_Tmp7;unsigned _Tmp8;int _Tmp9;if(*((int*)_Tmp6)==8){_Tmp9=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6)->f1;_Tmp8=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6)->f2;_Tmp7=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6)->f3;if(tag1==(unsigned)_Tmp8){int is_union2=_Tmp9;unsigned tag2=_Tmp8;struct Cyc_List_List*ts2=_Tmp7;
# 1702
for(1;ts1!=0 && ts2!=0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_PrattProver_equal_terms(g,(void*)ts1->hd,(void*)ts2->hd))break;}
if(ts1==ts2 && is_union1==is_union2)
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1707
continue;}else{goto _LL3B;}}else{_LL3B:
 continue;};}}
# 1710
goto _LL0;}case 15: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)rep1)->f1;{void*t1=_Tmp4;
# 1712
{struct Cyc_PrattProver_Node*ts=rs->next;for(0;ts!=0;ts=ts->next){
void*_Tmp6=ts->rep;void*_Tmp7;if(*((int*)_Tmp6)==15){_Tmp7=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp6)->f1;{void*t2=_Tmp7;
# 1715
if(Cyc_PrattProver_equal_terms(g,t1,t2))
Cyc_PrattProver_add_eq(g,rep1,ts->rep);
# 1718
continue;}}else{
continue;};}}
# 1721
goto _LL0;}default:
 goto _LL0;}_LL0:;}}
# 1728
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_equality_close_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_equality_close_graph,0,gs);}struct _tuple16{int f0;void*f1;int f2;};
# 1733
static struct _tuple16 Cyc_PrattProver_break_term(struct Cyc_PrattProver_Graph*g,void*t,int is_signed){
{unsigned _Tmp0;void*_Tmp1;if(*((int*)t)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned c2=_Tmp0;
# 1736
if((int)c2 >= 0){
if(is_signed){
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=Cyc_PrattProver_term2node(g,t1);Cyc_PrattProver_lookup_dist(_Tmp2,({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());}),1);});
# 1740
if(dist!=0 &&*dist <= 2147483647 - (int)c2){
struct _tuple16 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;return _Tmp2;}}else{
# 1745
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=Cyc_PrattProver_term2node(g,t1);Cyc_PrattProver_lookup_dist(_Tmp2,({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());}),0);});
if(dist!=0){
struct _tuple16 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;return _Tmp2;}}}else{
# 1751
if(is_signed){
# 1753
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp2,Cyc_PrattProver_term2node(g,t1),1);});
if(dist!=0 &&*dist <= (int)(c2 - 2147483648U)){
struct _tuple16 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;return _Tmp2;}}else{
# 1759
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp2,Cyc_PrattProver_term2node(g,t1),0);});
if(dist!=0 &&*dist <= (int)c2){
struct _tuple16 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;return _Tmp2;}}}
# 1765
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1767
 goto _LL0;}_LL0:;}{
# 1769
struct _tuple16 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=t,_Tmp0.f2=0;return _Tmp0;}}
# 1772
static void Cyc_PrattProver_break_term_in_graph(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
if(!rs->broken_as_signed){
struct _tuple16 _Tmp0=Cyc_PrattProver_break_term(g,rs->rep,1);int _Tmp1;void*_Tmp2;int _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int ok=_Tmp3;void*t1=_Tmp2;int c1=_Tmp1;
if(ok){
rs->broken_as_signed=1;
Cyc_PrattProver_set_distance(g,rs->rep,t1,(unsigned)c1,1);
if((unsigned)c1!=2147483648U)
Cyc_PrattProver_set_distance(g,t1,rs->rep,(unsigned)(- c1),1);}}}
# 1783
if(!rs->broken_as_unsigned){
struct _tuple16 _Tmp0=Cyc_PrattProver_break_term(g,rs->rep,0);int _Tmp1;void*_Tmp2;int _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int ok=_Tmp3;void*t1=_Tmp2;int c1=_Tmp1;
if(ok){
rs->broken_as_unsigned=1;
Cyc_PrattProver_set_distance(g,rs->rep,t1,(unsigned)c1,0);
if((unsigned)c1!=2147483648U)
Cyc_PrattProver_set_distance(g,t1,rs->rep,(unsigned)(- c1),0);}}}{
# 1792
struct _tuple11 _Tmp0=Cyc_PrattProver_subst_term_with_const(g,rs->rep);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c1=_Tmp1;
# 1794
if(ok){
void*_Tmp3=rs->rep;unsigned _Tmp4;void*_Tmp5;if(*((int*)_Tmp3)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f3)==0){_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f2;_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f3)->f1;{void*t1=_Tmp5;unsigned c2=_Tmp4;
# 1797
rs->broken_as_signed=1;
rs->broken_as_unsigned=1;
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t1;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_uint(c1 - c2));});
goto _LL9;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC:
# 1802
 goto _LL9;}_LL9:;}}}}}
# 1808
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_break_term_in_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_break_term_in_graph,0,gs);}
# 1817
static int Cyc_PrattProver_range_of_term(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*t){
struct Cyc_PrattProver_Node*zero_node=({struct Cyc_PrattProver_Graph*_Tmp0=g;Cyc_PrattProver_term2node(_Tmp0,Cyc_AssnDef_zero());});
# 1820
int*dist=Cyc_PrattProver_lookup_dist(zero_node,t,0);
if(dist!=0 &&(unsigned)*dist==2147483648U)
return 1;{
# 1824
int*dist=Cyc_PrattProver_lookup_dist(t,zero_node,0);
if(dist!=0 &&(unsigned)*dist <= 2147483647U)
return -1;{
# 1828
int*dist=Cyc_PrattProver_lookup_dist(zero_node,t,1);
if(dist!=0 &&*dist <= 0)
return 1;{
# 1832
int*dist=Cyc_PrattProver_lookup_dist(t,zero_node,1);
if(dist!=0 &&*dist <= -1)
return -1;
# 1836
return 0;}}}}
# 1847 "pratt_prover.cyc"
static void Cyc_PrattProver_associate_ud_sd_in_graph(int dummy,struct Cyc_PrattProver_Graph*g){
int range_of_src;int range_of_tgt;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
range_of_src=Cyc_PrattProver_range_of_term(g,rs);
if(range_of_src!=0){
{struct Cyc_PrattProver_Distance*uds=rs->unsigned_distances;for(0;uds!=0;uds=uds->next){
range_of_tgt=Cyc_PrattProver_range_of_term(g,uds->target);
if(range_of_src==range_of_tgt)
Cyc_PrattProver_set_distance(g,rs->rep,uds->target->rep,(unsigned)uds->dist,1);}}{
# 1858
struct Cyc_PrattProver_Distance*sds=rs->signed_distances;for(0;sds!=0;sds=sds->next){
range_of_tgt=Cyc_PrattProver_range_of_term(g,sds->target);
if(range_of_src==range_of_tgt)
Cyc_PrattProver_set_distance(g,rs->rep,sds->target->rep,(unsigned)sds->dist,0);}}}}}
# 1866
return;}
# 1869
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_associate_ud_sd_in_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_associate_ud_sd_in_graph,0,gs);}
# 1876
static void Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*g,void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
++ Cyc_PrattProver_constraints_added;
{struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned c1=_Tmp2;unsigned c2=_Tmp1;
# 1880
switch((int)p){case Cyc_AssnDef_ULt:
 if(c1 < c2)return;goto _LL9;case Cyc_AssnDef_ULte:
 if(c1 <= c2)return;goto _LL9;case Cyc_AssnDef_SLt:
 if((int)c1 < (int)c2)return;goto _LL9;case Cyc_AssnDef_SLte:
 if((int)c1 <= (int)c2)return;goto _LL9;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));}_LL9:;
# 1890
_throw(& Cyc_PrattProver_inconsistent);}}else{_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;{unsigned c1=_Tmp2;
# 1929
switch((int)p){case Cyc_AssnDef_ULt:
# 1932
 if(c1==4294967295U)
# 1936
_throw(& Cyc_PrattProver_inconsistent);
# 1938
c1=c1 + 1U;
goto _LL23;case Cyc_AssnDef_ULte: _LL23:
# 1942
 if(c1 > 2147483647U)
# 1945
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,t2,2147483648U,0);});else{
# 1950
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)(-(int)c1),0);});}
# 1952
return;case Cyc_AssnDef_SLt:
# 1955
 if(c1==2147483647U)
# 1959
_throw(& Cyc_PrattProver_inconsistent);
# 1961
c1=c1 + 1U;
goto _LL27;case Cyc_AssnDef_SLte: _LL27:
# 1966
 if(c1==2147483648U)return;{
struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;return Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)(-(int)c1),1);}default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));};}}}else{if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned c2=_Tmp2;
# 1893
switch((int)p){case Cyc_AssnDef_ULt:
# 1896
 if(c2==0U)
# 1900
_throw(& Cyc_PrattProver_inconsistent);
# 1902
c2=c2 - 1U;
goto _LL18;case Cyc_AssnDef_ULte: _LL18:
# 1906
 if(c2 <= 2147483647U)
# 1908
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)((int)c2),0);});
# 1910
return;case Cyc_AssnDef_SLt:
# 1913
 if(c2==2147483648U)
# 1917
_throw(& Cyc_PrattProver_inconsistent);
# 1919
c2=c2 - 1U;
goto _LL1C;case Cyc_AssnDef_SLte: _LL1C:
# 1923
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)((int)c2),1);});
return;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));};}}else{
# 1970
goto _LL0;}}_LL0:;}
# 1974
switch((int)p){case Cyc_AssnDef_ULt:
# 1977
 return Cyc_PrattProver_set_distance(g,t1,t2,-1,0);case Cyc_AssnDef_ULte:
# 1980
 return Cyc_PrattProver_set_distance(g,t1,t2,0U,0);case Cyc_AssnDef_SLt:
# 1983
 return Cyc_PrattProver_set_distance(g,t1,t2,-1,1);case Cyc_AssnDef_SLte:
# 1986
 return Cyc_PrattProver_set_distance(g,t1,t2,0U,1);default:
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Vcgen:found bad primop in add_constraint",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));};}
# 1992
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
if(t1==t2)return;
Cyc_PrattProver_add_constraint(g,t1,5U,t2);
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t1,3U,t2);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);}struct _tuple17{void*f0;enum Cyc_AssnDef_Primreln f1;void*f2;};
# 2000
static void Cyc_PrattProver_add_prim(struct _tuple17*p,struct Cyc_PrattProver_Graph*g){
struct _tuple17 _Tmp0=*p;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
if((int)p==0)
Cyc_PrattProver_add_eq(g,t1,t2);else{
# 2005
Cyc_PrattProver_add_constraint(g,t1,p,t2);}}}char Cyc_PrattProver_TooLarge[9U]="TooLarge";struct Cyc_PrattProver_TooLarge_exn_struct{char*tag;};
# 2105 "pratt_prover.cyc"
struct Cyc_PrattProver_TooLarge_exn_struct Cyc_PrattProver_too_large={Cyc_PrattProver_TooLarge};
# 2118 "pratt_prover.cyc"
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_cgraph(struct Cyc_PrattProver_Graph*gs,struct Cyc_Set_Set**already_seen,void*a,unsigned*total_paths){
# 2120
LOOP:
 if(gs==0)return gs;
# 2125
if(Cyc_Set_member(*already_seen,a)){++ Cyc_PrattProver_already_seen_hits;return gs;}
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(*already_seen,a);*already_seen=_Tmp0;});
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL0;case 1:
 gs=0;goto _LL0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 2131
gs=Cyc_PrattProver_cgraph(gs,already_seen,a2,total_paths);
a=a1;
goto LOOP;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 2135
unsigned n=Cyc_PrattProver_num_graphs(gs);
if(*_check_null(total_paths)> Cyc_Flags_max_vc_paths)
_throw(& Cyc_PrattProver_too_large);
# 2141
*total_paths=*total_paths + n;
gs=({struct Cyc_PrattProver_Graph*(*_Tmp3)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp3;})(Cyc_PrattProver_johnson,0,gs);{
struct Cyc_PrattProver_Graph*gs1=gs;
struct Cyc_PrattProver_Graph*gs2=Cyc_PrattProver_copy_graphs(gs);
struct Cyc_Set_Set*already_seen1=*already_seen;
struct Cyc_Set_Set*already_seen2=*already_seen;
# 2148
gs1=Cyc_PrattProver_cgraph(gs1,& already_seen1,a1,total_paths);
# 2150
if(*((int*)a1)==4)goto NOCLOSE1;else{goto _LL11;}_LL11:;
gs1=Cyc_PrattProver_associate_ud_sd_in_graphs(gs1);
gs1=Cyc_PrattProver_break_term_in_graphs(gs1);
gs1=Cyc_PrattProver_subst_close_graphs(gs1);
# 2156
gs1=Cyc_PrattProver_associate_ud_sd_in_graphs(gs1);
# 2159
NOCLOSE1:
 if(*((int*)a2)==4)goto NOCLOSE2;else{goto _LL16;}_LL16:;
gs2=Cyc_PrattProver_cgraph(gs2,& already_seen2,a2,total_paths);
gs2=Cyc_PrattProver_associate_ud_sd_in_graphs(gs2);
gs2=Cyc_PrattProver_break_term_in_graphs(gs2);
gs2=Cyc_PrattProver_subst_close_graphs(gs2);
# 2167
gs2=Cyc_PrattProver_associate_ud_sd_in_graphs(gs2);
NOCLOSE2:
# 2170
({struct Cyc_Set_Set*_Tmp3=Cyc_Set_intersect(already_seen1,already_seen2);*already_seen=_Tmp3;});
gs=Cyc_PrattProver_graph_append(gs1,gs2);
goto _LL0;}}default: if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1)->f1==0){if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){_Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t2=_Tmp2;
_Tmp2=t2;goto _LLC;}}else{goto _LLF;}}else{if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)->f1==0)goto _LLB;else{goto _LLD;}}else{goto _LLD;}}else{goto _LLF;}}}else{if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)->f1==0){_LLB: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_LLC: {void*t1=_Tmp2;
# 2177
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt==0 || Cyc_Tcutil_is_signed_type(topt)){
_Tmp2=t1;_Tmp1=Cyc_AssnDef_zero();goto _LLE;}
a=({void*_Tmp3=Cyc_AssnDef_zero();Cyc_AssnDef_ult(_Tmp3,t1);});
goto LOOP;}}else{goto _LLD;}}else{_LLD: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;_LLE: {void*t1=_Tmp2;void*t2=_Tmp1;
# 2185
a=({void*_Tmp3=({void*_Tmp4=Cyc_AssnDef_slt(t1,t2);Cyc_AssnDef_or(_Tmp4,Cyc_AssnDef_slt(t2,t1));});Cyc_AssnDef_and(_Tmp3,({void*_Tmp4=Cyc_AssnDef_ult(t1,t2);Cyc_AssnDef_or(_Tmp4,Cyc_AssnDef_ult(t2,t1));}));});
goto LOOP;}}}else{_LLF: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 2194
struct _tuple17 env=({struct _tuple17 _Tmp3;_Tmp3.f0=t1,_Tmp3.f1=p,_Tmp3.f2=t2;_Tmp3;});
gs=({struct Cyc_PrattProver_Graph*(*_Tmp3)(void(*)(struct _tuple17*,struct Cyc_PrattProver_Graph*),struct _tuple17*,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(struct _tuple17*,struct Cyc_PrattProver_Graph*),struct _tuple17*,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp3;})(Cyc_PrattProver_add_prim,& env,gs);
goto _LL0;}}}}_LL0:;}
# 2198
return gs;}
# 2201
static struct Cyc_Set_Set*Cyc_PrattProver_get_pointer_terms(void*t,struct Cyc_Set_Set*s){
LOOP: {
void*topt=Cyc_AssnDef_get_term_type(t);
if((topt!=0 && Cyc_Tcutil_is_nullable_pointer_type(topt,0))&&
 Cyc_Tcutil_get_type_bound(topt)!=0)
s=Cyc_Set_insert(s,t);
{void*_Tmp0;void*_Tmp1;switch(*((int*)t)){case 5: _Tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp1;
t=t1;goto LOOP;}case 3: _Tmp1=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp1;
t=t1;goto LOOP;}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 2211
s=Cyc_PrattProver_get_pointer_terms(t1,s);
t=t2;goto LOOP;}default:
 goto _LL0;}_LL0:;}
# 2215
return s;}}
# 2218
static struct Cyc_Set_Set*Cyc_PrattProver_get_graph_pointer_terms(struct Cyc_PrattProver_Graph*_g,struct Cyc_Set_Set*s){
# 2220
struct Cyc_PrattProver_Graph*g=_g;
for(1;g!=0;g=g->next){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
s=Cyc_PrattProver_get_pointer_terms(rs->rep,s);}}
return s;}struct _tuple18{void*f0;void*f1;void*f2;};
# 2227
static void Cyc_PrattProver_add_ptr_info(struct _tuple18*env,struct Cyc_PrattProver_Graph*g){
struct _tuple18 _Tmp0=*env;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t=_Tmp3;void*tnumelts=_Tmp2;void*bnd=_Tmp1;
# 2230
int*dist=({struct Cyc_PrattProver_Node*_Tmp4=({struct Cyc_PrattProver_Graph*_Tmp5=g;Cyc_PrattProver_term2node(_Tmp5,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp4,Cyc_PrattProver_term2node(g,t),0);});
if(dist!=0 &&*dist <= -1){
Cyc_PrattProver_add_constraint(g,bnd,5U,tnumelts);
Cyc_PrattProver_add_constraint(g,bnd,3U,tnumelts);}}}
# 2237
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_add_ptr_type_info(void*t,struct Cyc_PrattProver_Graph*gs){
void*type=_check_null(Cyc_AssnDef_get_term_type(t));
struct Cyc_Absyn_Exp*e=Cyc_PrattProver_strip_cast(_check_null(Cyc_Tcutil_get_type_bound(type)));
void*bnd=Cyc_AssnDef_cnst(e);
void*tnumelts=Cyc_AssnDef_numelts_term(t);
struct _tuple18 env=({struct _tuple18 _Tmp0;_Tmp0.f0=t,_Tmp0.f1=tnumelts,_Tmp0.f2=bnd;_Tmp0;});
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(struct _tuple18*,struct Cyc_PrattProver_Graph*),struct _tuple18*,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(struct _tuple18*,struct Cyc_PrattProver_Graph*),struct _tuple18*,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_add_ptr_info,& env,gs);}
# 2250
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_add_type_assns(struct Cyc_PrattProver_Graph*gs){
struct Cyc_Set_Set*ptrs=({struct Cyc_PrattProver_Graph*_Tmp0=gs;Cyc_PrattProver_get_graph_pointer_terms(_Tmp0,Cyc_Set_empty(Cyc_AssnDef_termcmp));});
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(struct Cyc_PrattProver_Graph*(*)(void*,struct Cyc_PrattProver_Graph*),struct Cyc_Set_Set*,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(struct Cyc_PrattProver_Graph*(*)(void*,struct Cyc_PrattProver_Graph*),struct Cyc_Set_Set*,struct Cyc_PrattProver_Graph*))Cyc_Set_fold;_Tmp0;})(Cyc_PrattProver_add_ptr_type_info,ptrs,gs);}
# 2256
static void Cyc_PrattProver_add_numelts_info(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*zeronode=({struct Cyc_PrattProver_Graph*_Tmp0=g;Cyc_PrattProver_term2node(_Tmp0,Cyc_AssnDef_zero());});
# 2259
struct Cyc_PrattProver_Node*n=g->rows;for(0;n!=0;n=n->next){
void*_Tmp0=n->rep;unsigned _Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f1==Cyc_Absyn_Numelts){if(*((int*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f2)==14){if(*((int*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f2)->f2)==0){_Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f2)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f2)->f2)->f1;{void*x=_Tmp2;unsigned uk=_Tmp1;
# 2262
int k=(int)uk;
if(k > 0){
# 2265
void*numelts_x=Cyc_AssnDef_numelts_term(x);
struct Cyc_PrattProver_Node*n_numelts_x=Cyc_PrattProver_term2node(g,numelts_x);
# 2268
int*dist_opt=Cyc_PrattProver_lookup_dist(zeronode,n_numelts_x,0);
if(dist_opt!=0){
int j=*dist_opt;
# 2272
int negj=- j;
# 2274
if(negj > 0){
int c=negj - 1;
# 2277
if(k <= c)
# 2279
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_uint((unsigned)(c - k));Cyc_PrattProver_add_constraint(_Tmp3,_Tmp4,4U,n->rep);});}}}
# 2284
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}}
# 2290
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_add_numelts_info_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_add_numelts_info,0,gs);}
# 2294
static int Cyc_PrattProver_paths_exhausted=0;
# 2299
static int Cyc_PrattProver_consistent(void*a){
# 2303
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
# 2305
Cyc_PrattProver_paths_exhausted=0;
Cyc_PrattProver_constraints_added=0;
Cyc_PrattProver_graphs_copied=0;
Cyc_PrattProver_max_lookup=0;
Cyc_PrattProver_already_seen_hits=0;{
struct Cyc_Set_Set*already_seen=Cyc_Set_empty(Cyc_AssnDef_assncmp);
unsigned total_paths=1U;
struct Cyc_PrattProver_Graph*gs=({struct Cyc_PrattProver_Graph*_Tmp2=Cyc_PrattProver_true_graph();Cyc_PrattProver_cgraph(_Tmp2,& already_seen,a,& total_paths);});
# 2319
Cyc_PrattProver_graphs_change=1;
while(Cyc_PrattProver_graphs_change && gs!=0){
gs=({struct Cyc_PrattProver_Graph*(*_Tmp2)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp2;})(Cyc_PrattProver_johnson,0,gs);
Cyc_PrattProver_graphs_change=0;
gs=Cyc_PrattProver_associate_ud_sd_in_graphs(gs);
gs=Cyc_PrattProver_add_type_assns(gs);
gs=Cyc_PrattProver_add_numelts_info_graphs(gs);
# 2330
gs=Cyc_PrattProver_break_term_in_graphs(gs);
gs=Cyc_PrattProver_associate_ud_sd_in_graphs(gs);
# 2321
1U;}{
# 2339
int _Tmp2=gs!=0;_npop_handler(0);return _Tmp2;}}
# 2305
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_PrattProver_TooLarge_exn_struct*)_Tmp2)->tag==Cyc_PrattProver_TooLarge){
# 2344
Cyc_PrattProver_paths_exhausted=1;
return 1;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 2350
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a){
void*b=({void*_Tmp0=ctxt;Cyc_AssnDef_and(_Tmp0,Cyc_AssnDef_not(a));});
int res=!Cyc_PrattProver_consistent(b);
if(!res && Cyc_Flags_print_unprovable){
Cyc_fprintf(Cyc_stderr,_tag_fat("unable to prove inconsistency of \n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_AssnDef_assn2dag(b);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});
if(Cyc_PrattProver_paths_exhausted)
Cyc_fprintf(Cyc_stderr,_tag_fat("(paths exhausted)",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));
# 2359
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 2361
return res;}

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
 int Cyc_Core_ptrcmp(void*,void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 166
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 522 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 547
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};
# 933 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 1108
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
# 116
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;void*f2;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Select_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Update_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Aggr_AssnDef_Term_struct{int tag;int f1;unsigned f2;struct Cyc_List_List*f3;void*f4;};struct Cyc_AssnDef_Proj_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Alloc_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;void*f4;void*f5;};struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Offseti_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Tagof_AssnDef_Term_struct{int tag;void*f1;};
# 140 "assndef.h"
int Cyc_AssnDef_termcmp(void*,void*);
int Cyc_AssnDef_termhash(void*);
# 143
extern struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 157
extern void*Cyc_AssnDef_zero (void);
# 161
extern void*Cyc_AssnDef_uint(unsigned);
extern void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
extern void*Cyc_AssnDef_select(void*,void*,void*);
extern void*Cyc_AssnDef_update(void*,void*,void*);
# 167
extern void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
# 170
extern void*Cyc_AssnDef_tagof_tm(void*);
extern void*Cyc_AssnDef_proj(void*,unsigned,void*);
extern void*Cyc_AssnDef_aggr_update(void*,unsigned,void*);
# 179
extern void*Cyc_AssnDef_numelts_term(void*);
# 200
extern void*Cyc_AssnDef_get_term_type(void*);
# 213 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 227
extern struct _fat_ptr Cyc_AssnDef_assn2dag(void*);
# 229
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
# 232
extern void*Cyc_AssnDef_and(void*,void*);
extern void*Cyc_AssnDef_or(void*,void*);
extern void*Cyc_AssnDef_not(void*);
# 238
extern void*Cyc_AssnDef_slt(void*,void*);
# 240
extern void*Cyc_AssnDef_ult(void*,void*);
# 347 "assndef.h"
extern void*Cyc_AssnDef_widen(void*);
# 357
extern struct Cyc_Hashtable_Table*Cyc_AssnDef_empty_term_table (void);
extern void*Cyc_AssnDef_subst_table(struct Cyc_Hashtable_Table*,void*);
# 39 "tcutil.h"
int Cyc_Tcutil_is_signed_type(void*);
# 49
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 74
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 129 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 229
int Cyc_Tcutil_is_pure_exp(struct Cyc_Absyn_Exp*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 74 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 89 "flags.h"
extern int Cyc_Flags_print_unprovable;
# 154 "flags.h"
extern unsigned Cyc_Flags_max_vc_paths;
# 31 "pratt_prover.cyc"
extern void**Cyc_Hashtable_lookup_ptr_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc_PrattProver_Node;struct Cyc_PrattProver_Distance{struct Cyc_PrattProver_Distance*next;struct Cyc_PrattProver_Node*target;int dist;};struct _union_ShortestPathInfo_Infinity{int tag;int val;};struct _union_ShortestPathInfo_Shortest{int tag;int val;};struct _union_ShortestPathInfo_Current{int tag;int val;};union Cyc_PrattProver_ShortestPathInfo{struct _union_ShortestPathInfo_Infinity Infinity;struct _union_ShortestPathInfo_Shortest Shortest;struct _union_ShortestPathInfo_Current Current;};
# 183 "pratt_prover.cyc"
union Cyc_PrattProver_ShortestPathInfo Cyc_PrattProver_infinity={.Infinity={1,0}};struct Cyc_PrattProver_Node{struct Cyc_PrattProver_Node*next;void*rep;int broken_as_signed: 1;int broken_as_unsigned: 1;struct Cyc_PrattProver_Distance*unsigned_distances;struct Cyc_PrattProver_Distance*signed_distances;int signeddistFromS;int unsigneddistFromS;union Cyc_PrattProver_ShortestPathInfo shortest_path_info;};struct Cyc_PrattProver_Graph{struct Cyc_PrattProver_Graph*next;struct Cyc_PrattProver_Node*rows;struct Cyc_Hashtable_Table*map;int changed;};
# 210
static unsigned Cyc_PrattProver_num_graphs(struct Cyc_PrattProver_Graph*gs){
unsigned c=0U;
for(1;gs!=0;gs=gs->next){
++ c;}
return c;}
# 217
static struct Cyc_Hashtable_Table*Cyc_PrattProver_empty_map(unsigned n){
return Cyc_Hashtable_create((int)n,Cyc_Core_ptrcmp,Cyc_AssnDef_termhash);}
# 223
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_true_graph (void){
struct Cyc_PrattProver_Graph*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Graph));_Tmp0->next=0,_Tmp0->rows=0,({struct Cyc_Hashtable_Table*_Tmp1=Cyc_PrattProver_empty_map(17U);_Tmp0->map=_Tmp1;}),_Tmp0->changed=0;return _Tmp0;}
# 227
static int Cyc_PrattProver_constraints_added=0;
static int Cyc_PrattProver_graphs_copied=0;
static int Cyc_PrattProver_max_lookup=0;
static int Cyc_PrattProver_already_seen_hits=0;
# 232
static void Cyc_PrattProver_print_shortestpathinfo(union Cyc_PrattProver_ShortestPathInfo info){
{int _Tmp0;switch(info.Shortest.tag){case 1:
# 235
 Cyc_fprintf(Cyc_stderr,_tag_fat("not reachable from s\n",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;case 2: _Tmp0=info.Shortest.val;{int d=_Tmp0;
# 238
({struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)d;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("shortest distance from s is %d\n",sizeof(char),32U),_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL0;}default: _Tmp0=info.Current.val;{int d=_Tmp0;
# 241
({struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)d;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("current distance from s is %d\n",sizeof(char),31U),_tag_fat(_Tmp2,sizeof(void*),1));});
goto _LL0;}}_LL0:;}
# 244
return;}
# 247
void Cyc_PrattProver_print_graph(struct Cyc_PrattProver_Graph*g){
Cyc_fprintf(Cyc_stderr,_tag_fat("{",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
if(g->rows==0)Cyc_fprintf(Cyc_stderr,_tag_fat("<true>",sizeof(char),7U),_tag_fat(0U,sizeof(void*),0));else{
# 251
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
struct Cyc_PrattProver_Node*_Tmp0=rs;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->rep;_Tmp2=_Tmp0->unsigned_distances;_Tmp1=_Tmp0->signed_distances;{void*rep=_Tmp3;struct Cyc_PrattProver_Distance*uds=_Tmp2;struct Cyc_PrattProver_Distance*sds=_Tmp1;
struct _fat_ptr s=Cyc_AssnDef_term2string(rep);
# 261
for(1;uds!=0;uds=uds->next){
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(uds->target->rep);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)uds->dist;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("  %s - %s U<= %d\n ",sizeof(char),19U),_tag_fat(_Tmp7,sizeof(void*),3));});}
for(1;sds!=0;sds=sds->next){
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(sds->target->rep);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)sds->dist;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_fprintf(Cyc_stderr,_tag_fat("  %s - %s S<= %d\n ",sizeof(char),19U),_tag_fat(_Tmp7,sizeof(void*),3));});}}}}
# 269
Cyc_fprintf(Cyc_stderr,_tag_fat("}\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));}
# 273
void Cyc_PrattProver_print_graphs(struct Cyc_PrattProver_Graph*g){
Cyc_fprintf(Cyc_stderr,_tag_fat("Graphs:-----------\n",sizeof(char),20U),_tag_fat(0U,sizeof(void*),0));
if(g==0)Cyc_fprintf(Cyc_stderr,_tag_fat("<false>\n",sizeof(char),9U),_tag_fat(0U,sizeof(void*),0));else{
# 277
int i=0;for(0;g!=0;(i ++,g=g->next)){
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)i;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("graph %d:\n",sizeof(char),11U),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_PrattProver_print_graph(g);}}
# 282
Cyc_fprintf(Cyc_stderr,_tag_fat("------------------\n",sizeof(char),20U),_tag_fat(0U,sizeof(void*),0));}char Cyc_PrattProver_Inconsistent[13U]="Inconsistent";struct Cyc_PrattProver_Inconsistent_exn_struct{char*tag;};
# 287
struct Cyc_PrattProver_Inconsistent_exn_struct Cyc_PrattProver_inconsistent={Cyc_PrattProver_Inconsistent};
# 290
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node(struct Cyc_PrattProver_Graph*,void*);
static void Cyc_PrattProver_set_distance(struct Cyc_PrattProver_Graph*,void*,void*,unsigned,int);
static void Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*,void*,enum Cyc_AssnDef_Primreln,void*);
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*,void*,void*);struct _tuple11{int f0;unsigned f1;void*f2;};
static struct _tuple11 Cyc_PrattProver_subst_term_with_const(struct Cyc_PrattProver_Graph*,void*);
# 297
static struct Cyc_Absyn_Exp*Cyc_PrattProver_strip_cast(struct Cyc_Absyn_Exp*e){
LOOP: {
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==14){_Tmp2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*tp2=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 301
{void*_Tmp3=Cyc_Absyn_compress(tp2);enum Cyc_Absyn_Size_of _Tmp4;if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==1){_Tmp4=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f2;{enum Cyc_Absyn_Size_of s=_Tmp4;
# 303
if((int)s==2 ||(int)s==3){
e=e2;goto LOOP;}
# 306
goto _LL5;}}else{goto _LL8;}}else{_LL8:
 goto _LL5;}_LL5:;}
# 309
goto _LL0;}}else{
goto _LL0;}_LL0:;}
# 312
return e;}
# 319
static void Cyc_PrattProver_add_type_info(struct Cyc_PrattProver_Graph*g,void*term){
# 323
void*z=Cyc_AssnDef_zero();
Cyc_PrattProver_set_distance(g,term,z,2147483647U,1);
Cyc_PrattProver_set_distance(g,z,term,0U,0);
{void*_Tmp0;switch(*((int*)term)){case 3: if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)term)->f1==Cyc_Absyn_Numelts){_Tmp0=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)term)->f2;{void*x=_Tmp0;
# 329
Cyc_PrattProver_set_distance(g,term,z,2147483646U,0);
# 331
Cyc_PrattProver_term2node(g,x);
goto _LL0;}}else{goto _LL7;}case 12: _Tmp0=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)term)->f3;{void*n=_Tmp0;
# 335
Cyc_PrattProver_add_constraint(g,z,4U,term);
({struct Cyc_PrattProver_Graph*_Tmp1=g;void*_Tmp2=Cyc_AssnDef_numelts_term(term);Cyc_PrattProver_add_constraint(_Tmp1,_Tmp2,5U,n);});
goto _LL0;}case 11:
# 340
 Cyc_PrattProver_add_constraint(g,z,4U,term);
goto _LL0;default: _LL7:
 goto _LL0;}_LL0:;}{
# 344
void*topt=Cyc_AssnDef_get_term_type(term);
if(topt!=0){
void*t=topt;
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(t);
if(eopt!=0 && Cyc_Tcutil_is_pure_exp(eopt)){
void*t1=Cyc_AssnDef_numelts_term(term);
# 351
struct Cyc_Absyn_Exp*e=Cyc_PrattProver_strip_cast(eopt);
# 353
void*t2=Cyc_AssnDef_cnst(e);
# 357
if(!Cyc_Tcutil_is_nullable_pointer_type(t,0)){
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);
Cyc_PrattProver_add_constraint(g,z,4U,term);}}{
# 363
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1==Cyc_Absyn_Unsigned){
# 365
Cyc_PrattProver_add_constraint(g,z,5U,term);
goto _LL9;}else{goto _LLE;}case 5: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*v=_Tmp1;
# 368
{void*_Tmp2=Cyc_Absyn_compress(v);void*_Tmp3;if(*((int*)_Tmp2)==9){_Tmp3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 370
void*c=Cyc_AssnDef_cnst(e);
Cyc_PrattProver_add_eq(g,term,c);
goto _LL10;}}else{_Tmp3=_Tmp2;{void*v2=_Tmp3;
# 374
void*c=Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp(v2,0U));
Cyc_PrattProver_add_eq(g,term,c);
goto _LL10;}}_LL10:;}
# 378
goto _LL9;}}else{goto _LLE;}default: goto _LLE;}else{_LLE:
 goto _LL9;}_LL9:;}}}}
# 386
static struct Cyc_PrattProver_Node*Cyc_PrattProver_term2node(struct Cyc_PrattProver_Graph*g,void*t){
# 388
struct Cyc_PrattProver_Node**nopt=({struct Cyc_PrattProver_Node**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_PrattProver_Node**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_ptr_opt;_Tmp0;})(g->map,t);
if(nopt!=0)return*nopt;{
# 391
struct Cyc_PrattProver_Node**prev=& g->rows;
{struct Cyc_PrattProver_Node*r=g->rows;for(0;r!=0;r=r->next){
# 395
struct Cyc_PrattProver_Node*n=r;
void*rep=n->rep;
int c=Cyc_AssnDef_termcmp(t,rep);
if(c > 1)break;
if(c==0){
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*))Cyc_Hashtable_insert;_Tmp0;})(g->map,t,n);
return n;}
# 403
prev=& n->next;}}{
# 405
struct Cyc_PrattProver_Node*n;
{unsigned _Tmp0;void*_Tmp1;if(*((int*)t)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned c2=_Tmp0;
# 408
n=({struct Cyc_PrattProver_Node*_Tmp2=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp2->next=*prev,_Tmp2->rep=t,_Tmp2->broken_as_signed=0,_Tmp2->broken_as_unsigned=0,_Tmp2->unsigned_distances=0,_Tmp2->signed_distances=0,_Tmp2->signeddistFromS=0,_Tmp2->unsigneddistFromS=0,_Tmp2->shortest_path_info=Cyc_PrattProver_infinity;_Tmp2;});
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 411
 n=({struct Cyc_PrattProver_Node*_Tmp2=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp2->next=*prev,_Tmp2->rep=t,_Tmp2->broken_as_signed=1,_Tmp2->broken_as_unsigned=1,_Tmp2->unsigned_distances=0,_Tmp2->signed_distances=0,_Tmp2->signeddistFromS=0,_Tmp2->unsigneddistFromS=0,_Tmp2->shortest_path_info=Cyc_PrattProver_infinity;_Tmp2;});
goto _LL0;}_LL0:;}
# 414
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*))Cyc_Hashtable_insert;_Tmp0;})(g->map,t,n);
*prev=n;
Cyc_PrattProver_add_type_info(g,t);
return n;}}}
# 424
static int*Cyc_PrattProver_lookup_dist(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed){
static int zero=0;
if(source==target)return& zero;{
struct Cyc_PrattProver_Distance*ds=is_signed?source->signed_distances: source->unsigned_distances;
# 429
for(1;ds!=0;ds=ds->next){
if(ds->target==target)return& ds->dist;}
return 0;}}
# 437
static int Cyc_PrattProver_eq_nodes(struct Cyc_PrattProver_Node*r,struct Cyc_PrattProver_Node*t,int is_signed){
if(r==t)return 1;{
int*rt_dist=Cyc_PrattProver_lookup_dist(r,t,is_signed);
int*tr_dist=Cyc_PrattProver_lookup_dist(t,r,is_signed);
return((rt_dist!=0 &&*rt_dist==0)&& tr_dist!=0)&&*tr_dist==0;}}
# 445
static int Cyc_PrattProver_equal_nodes(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Node*r){
return Cyc_PrattProver_eq_nodes(s,r,0)|| Cyc_PrattProver_eq_nodes(s,r,1);}struct _tuple12{void*f0;void*f1;};
# 449
static int Cyc_PrattProver_equal_terms(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
if(t1==t2)return 1;{
struct _tuple11 _Tmp0=Cyc_PrattProver_subst_term_with_const(g,t1);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok1=_Tmp2;unsigned c1=_Tmp1;
struct _tuple11 _Tmp3=Cyc_PrattProver_subst_term_with_const(g,t2);unsigned _Tmp4;int _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{int ok2=_Tmp5;unsigned c2=_Tmp4;
if((ok1 && ok2)&& c1==c2)return 1;{
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node(g,t1);
struct Cyc_PrattProver_Node*n2=Cyc_PrattProver_term2node(g,t2);
if(Cyc_PrattProver_equal_nodes(n1,n2))return 1;
{struct _tuple12 _Tmp6=({struct _tuple12 _Tmp7;_Tmp7.f0=t1,_Tmp7.f1=t2;_Tmp7;});int _Tmp7;int _Tmp8;unsigned _Tmp9;unsigned _TmpA;void*_TmpB;void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;enum Cyc_Absyn_Primop _Tmp10;void*_Tmp11;enum Cyc_Absyn_Primop _Tmp12;switch(*((int*)_Tmp6.f0)){case 3: if(*((int*)_Tmp6.f1)==3){_Tmp12=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f0)->f1;_Tmp11=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f0)->f2;_Tmp10=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp6.f1)->f2;if((int)((enum Cyc_Absyn_Primop)_Tmp12)==(int)((enum Cyc_Absyn_Primop)_Tmp10)){enum Cyc_Absyn_Primop p11=_Tmp12;void*t11=_Tmp11;enum Cyc_Absyn_Primop p21=_Tmp10;void*t21=_TmpF;
# 459
return Cyc_PrattProver_equal_terms(g,t11,t21);}else{goto _LL19;}}else{goto _LL19;}case 4: if(*((int*)_Tmp6.f1)==4){_Tmp12=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f1;_Tmp11=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpF=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f0)->f3;_Tmp10=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpE=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f2;_TmpD=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp6.f1)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp12)==(int)((enum Cyc_Absyn_Primop)_Tmp10)){enum Cyc_Absyn_Primop p11=_Tmp12;void*t11=_Tmp11;void*t12=_TmpF;enum Cyc_Absyn_Primop p21=_Tmp10;void*t21=_TmpE;void*t22=_TmpD;
# 461
return Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22);}else{goto _LL19;}}else{goto _LL19;}case 5: if(*((int*)_Tmp6.f1)==5){_Tmp11=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpE=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpD=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp6.f1)->f2;{void*tp1=_Tmp11;void*t11=_TmpF;void*tp2=_TmpE;void*t21=_TmpD;
# 463
return Cyc_Tcutil_typecmp(tp1,tp2)==0 && Cyc_PrattProver_equal_terms(g,t11,t21);}}else{goto _LL19;}case 6: if(*((int*)_Tmp6.f1)==6){_Tmp11=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpE=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpD=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp6.f1)->f2;{void*t11=_Tmp11;void*t12=_TmpF;void*t21=_TmpE;void*t22=_TmpD;
# 465
return Cyc_PrattProver_equal_terms(g,t11,t21)&& Cyc_PrattProver_equal_terms(g,t12,t22);}}else{goto _LL19;}case 7: if(*((int*)_Tmp6.f1)==7){_Tmp11=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpE=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f0)->f3;_TmpD=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f1)->f1;_TmpC=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f1)->f2;_TmpB=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp6.f1)->f3;{void*t11=_Tmp11;void*t12=_TmpF;void*t13=_TmpE;void*t21=_TmpD;void*t22=_TmpC;void*t23=_TmpB;
# 467
return(Cyc_PrattProver_equal_terms(g,t11,t21)&&
 Cyc_PrattProver_equal_terms(g,t12,t22))&& Cyc_PrattProver_equal_terms(g,t13,t23);}}else{goto _LL19;}case 9: if(*((int*)_Tmp6.f1)==9){_Tmp11=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpA=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpF=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp9=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp6.f1)->f2;{void*t1=_Tmp11;unsigned i1=_TmpA;void*t2=_TmpF;unsigned i2=_Tmp9;
# 470
return i1==i2 && Cyc_PrattProver_equal_terms(g,t1,t2);}}else{goto _LL19;}case 10: if(*((int*)_Tmp6.f1)==10){_Tmp11=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpA=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f0)->f2;_TmpF=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f0)->f3;_TmpE=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp9=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f1)->f2;_TmpD=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)_Tmp6.f1)->f3;{void*t1=_Tmp11;unsigned i1=_TmpA;void*v1=_TmpF;void*t2=_TmpE;unsigned i2=_Tmp9;void*v2=_TmpD;
# 472
return(i1==i2 && Cyc_PrattProver_equal_terms(g,t1,t2))&& Cyc_PrattProver_equal_terms(g,v1,v2);}}else{goto _LL19;}case 8: if(*((int*)_Tmp6.f1)==8){_Tmp8=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpA=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f0)->f2;_Tmp11=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f0)->f3;_Tmp7=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f1)->f1;_Tmp9=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f1)->f2;_TmpF=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp6.f1)->f3;{int is_union1=_Tmp8;unsigned tag1=_TmpA;struct Cyc_List_List*ts1=_Tmp11;int is_union2=_Tmp7;unsigned tag2=_Tmp9;struct Cyc_List_List*ts2=_TmpF;
# 474
for(1;ts1!=0 && ts2!=0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_PrattProver_equal_terms(g,(void*)ts1->hd,(void*)ts2->hd))return 0;}
return tag1==tag2 && is_union1==is_union2;}}else{goto _LL19;}case 15: if(*((int*)_Tmp6.f1)==15){_Tmp11=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp6.f0)->f1;_TmpF=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp6.f1)->f1;{void*t1=_Tmp11;void*t2=_TmpF;
# 478
return Cyc_PrattProver_equal_terms(g,t1,t2);}}else{goto _LL19;}default: _LL19:
# 480
 return 0;};}
# 482
return 0;}}}}}
# 486
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_graph_append(struct Cyc_PrattProver_Graph*g1,struct Cyc_PrattProver_Graph*g2){
if(g1==0)return g2;
if(g2==0)return g1;{
struct Cyc_PrattProver_Graph*p=g1;
{struct Cyc_PrattProver_Graph*x=p->next;for(0;x!=0;(p=x,x=p->next)){;}}
p->next=g2;
return g1;}}struct _tuple13{int f0;int f1;};
# 499
inline static struct _tuple13 Cyc_PrattProver_is_signed_overflow(int c1,int c2){
int sum=c1 + c2;
if((~(c1 ^ c2)& (sum ^ c1))>> 31){
# 503
if(sum > 0){struct _tuple13 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=-1;return _Tmp0;}else{
struct _tuple13 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=1;return _Tmp0;}}else{
# 506
struct _tuple13 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=sum;return _Tmp0;}}
# 511
static void Cyc_PrattProver_new_distance(struct Cyc_PrattProver_Node*source,struct Cyc_PrattProver_Node*target,int is_signed,int d){
struct Cyc_PrattProver_Distance*dist;dist=_cycalloc(sizeof(struct Cyc_PrattProver_Distance)),dist->next=0,dist->target=target,dist->dist=d;
if(is_signed){
dist->next=source->signed_distances;
source->signed_distances=dist;}else{
# 517
dist->next=source->unsigned_distances;
source->unsigned_distances=dist;}}
# 522
inline static void Cyc_PrattProver_set_dist(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*i,struct Cyc_PrattProver_Node*j,int dist,int is_signed){
# 529
if(i==j){
if(dist < 0 ||(unsigned)dist==2147483648U)_throw(& Cyc_PrattProver_inconsistent);
if(dist > 0)return;}{
# 533
int*ij_dist=Cyc_PrattProver_lookup_dist(i,j,is_signed);
# 539
if(ij_dist!=0){
if((unsigned)*ij_dist==2147483648U)return;
if((unsigned)dist!=2147483648U &&*ij_dist < dist)return;
*ij_dist=dist;}else{
# 544
Cyc_PrattProver_new_distance(i,j,is_signed,dist);}
# 546
g->changed=1;{
int*ji_dist=Cyc_PrattProver_lookup_dist(j,i,is_signed);
if(ji_dist!=0){
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(*ji_dist,dist);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
if(sum < 0)
# 554
_throw(& Cyc_PrattProver_inconsistent);}}
# 557
return;}}}
# 560
static int Cyc_PrattProver_num_of_nodes(struct Cyc_PrattProver_Graph*g){
int n=0;
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
++ n;}}
# 565
return n;}
# 577 "pratt_prover.cyc"
static void Cyc_PrattProver_bellman_ford(int dummy,struct Cyc_PrattProver_Graph*g){
# 581
int n=Cyc_PrattProver_num_of_nodes(g);
# 583
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
node->signeddistFromS=0;
node->unsigneddistFromS=0;}}
# 590
{int i=0;for(0;i < n;++ i){
# 592
struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 599
if((unsigned)dv_unsigned==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_unsigned,uv_unsigned);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 602
if((!overflow &&(unsigned)dv_unsigned!=2147483648U)&& sum < dv_unsigned)dists->target->unsigneddistFromS=sum;
# 604
if(overflow && sum < 0)dists->target->unsigneddistFromS=-2147483648;}}}}{
# 606
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 612
if((unsigned)dv_signed==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_signed,uv_signed);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 615
if((!overflow &&(unsigned)dv_signed!=2147483648U)&& sum < dv_signed)dists->target->signeddistFromS=sum;
# 617
if(overflow && sum < 0)dists->target->signeddistFromS=-2147483648;}}}}}}}
# 625
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 632
if((unsigned)dv_unsigned==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_unsigned,uv_unsigned);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
# 635
if(!overflow && sum < dv_unsigned || overflow && sum < 0)
# 639
_throw(& Cyc_PrattProver_inconsistent);}}}}{
# 642
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 648
if((unsigned)dv_signed==2147483648U)continue;{
struct _tuple13 _Tmp0=Cyc_PrattProver_is_signed_overflow(du_signed,uv_signed);int _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int overflow=_Tmp2;int sum=_Tmp1;
if(!overflow && sum < dv_signed || overflow && sum < 0)
# 654
_throw(& Cyc_PrattProver_inconsistent);}}}}}}
# 658
return;}
# 664
static void Cyc_PrattProver_initialize_dist_set(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g){
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
if(node==s)({union Cyc_PrattProver_ShortestPathInfo _Tmp0=({union Cyc_PrattProver_ShortestPathInfo _Tmp1;_Tmp1.Current.tag=3U,_Tmp1.Current.val=0;_Tmp1;});node->shortest_path_info=_Tmp0;});else{
node->shortest_path_info=Cyc_PrattProver_infinity;}}}
# 669
return;}
# 678
static struct Cyc_PrattProver_Node*Cyc_PrattProver_extract_min(struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*current_shortest_node=0;
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
union Cyc_PrattProver_ShortestPathInfo _Tmp0=node->shortest_path_info;int _Tmp1;switch(_Tmp0.Shortest.tag){case 1:
 goto _LL0;case 2:
 goto _LL0;default: _Tmp1=_Tmp0.Current.val;{int d_new=_Tmp1;
# 685
if(current_shortest_node==0)current_shortest_node=node;else{
# 687
union Cyc_PrattProver_ShortestPathInfo _Tmp2=current_shortest_node->shortest_path_info;int _Tmp3;if(_Tmp2.Current.tag==3){_Tmp3=_Tmp2.Current.val;{int d_old=_Tmp3;
# 689
if(d_new <= d_old)current_shortest_node=node;
goto _LL7;}}else{
# 692
({int(*_Tmp4)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;});struct _fat_ptr _Tmp5=Cyc_aprintf(_tag_fat("current_shortest_node should always be NULL or &Current(d)\n",sizeof(char),60U),_tag_fat(0U,sizeof(void*),0));_Tmp4(_Tmp5,_tag_fat(0U,sizeof(void*),0));});}_LL7:;}
# 695
goto _LL0;}}_LL0:;}}
# 698
return current_shortest_node;}
# 701
static void Cyc_PrattProver_relaxation(struct Cyc_PrattProver_Node*u,struct Cyc_PrattProver_Graph*g,int is_signed){
struct Cyc_PrattProver_Distance*dists;
int su;
{union Cyc_PrattProver_ShortestPathInfo _Tmp0=u->shortest_path_info;int _Tmp1;if(_Tmp0.Shortest.tag==2){_Tmp1=_Tmp0.Shortest.val;{int d=_Tmp1;
su=d;goto _LL0;}}else{
# 707
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;});struct _fat_ptr _Tmp3=Cyc_aprintf(_tag_fat("current_shortest_node should always containe &Shortest(d)\n",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));_Tmp2(_Tmp3,_tag_fat(0U,sizeof(void*),0));});
goto _LL0;}_LL0:;}
# 710
if(is_signed)dists=u->signed_distances;else{
dists=u->unsigned_distances;}
for(1;dists!=0;dists=dists->next){
union Cyc_PrattProver_ShortestPathInfo _Tmp0=dists->target->shortest_path_info;int _Tmp1;switch(_Tmp0.Current.tag){case 2:
 goto _LL5;case 3: _Tmp1=_Tmp0.Current.val;{int sv=_Tmp1;
# 716
int uv=dists->dist;
struct _tuple13 _Tmp2=Cyc_PrattProver_is_signed_overflow(su,uv);int _Tmp3;int _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{int overflow=_Tmp4;int sum=_Tmp3;
if(!overflow && sum < sv)({union Cyc_PrattProver_ShortestPathInfo _Tmp5=({union Cyc_PrattProver_ShortestPathInfo _Tmp6;_Tmp6.Current.tag=3U,_Tmp6.Current.val=sum;_Tmp6;});dists->target->shortest_path_info=_Tmp5;});
goto _LL5;}}default:  {
# 721
int uv=dists->dist;
struct _tuple13 _Tmp2=Cyc_PrattProver_is_signed_overflow(su,uv);int _Tmp3;int _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{int overflow=_Tmp4;int sum=_Tmp3;
if(!overflow)({union Cyc_PrattProver_ShortestPathInfo _Tmp5=({union Cyc_PrattProver_ShortestPathInfo _Tmp6;_Tmp6.Current.tag=3U,_Tmp6.Current.val=sum;_Tmp6;});dists->target->shortest_path_info=_Tmp5;});
goto _LL5;}}}_LL5:;}
# 727
return;}
# 733
static void Cyc_PrattProver_set_shortest_dist_from_s(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g,int is_signed){
struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
union Cyc_PrattProver_ShortestPathInfo _Tmp0=node->shortest_path_info;int _Tmp1;if(_Tmp0.Shortest.tag==2){_Tmp1=_Tmp0.Shortest.val;{int d=_Tmp1;
Cyc_PrattProver_set_dist(g,s,node,d,is_signed);goto _LL0;}}else{
goto _LL0;}_LL0:;}}
# 743
static void Cyc_PrattProver_dijkstra(struct Cyc_PrattProver_Node*s,struct Cyc_PrattProver_Graph*g){
# 749
Cyc_PrattProver_initialize_dist_set(s,g);
while(1){
{struct Cyc_PrattProver_Node*current_shortest_node=Cyc_PrattProver_extract_min(g);
# 753
if(current_shortest_node==0)break;else{
# 755
union Cyc_PrattProver_ShortestPathInfo _Tmp0=current_shortest_node->shortest_path_info;int _Tmp1;if(_Tmp0.Current.tag==3){_Tmp1=_Tmp0.Current.val;{int d=_Tmp1;
# 758
({union Cyc_PrattProver_ShortestPathInfo _Tmp2=({union Cyc_PrattProver_ShortestPathInfo _Tmp3;_Tmp3.Shortest.tag=2U,_Tmp3.Shortest.val=d;_Tmp3;});current_shortest_node->shortest_path_info=_Tmp2;});
goto _LL0;}}else{
# 761
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;});struct _fat_ptr _Tmp3=Cyc_aprintf(_tag_fat("current_shortest_node should always contain &Current(d)\n",sizeof(char),57U),_tag_fat(0U,sizeof(void*),0));_Tmp2(_Tmp3,_tag_fat(0U,sizeof(void*),0));});
goto _LL0;}_LL0:;}
# 765
Cyc_PrattProver_relaxation(current_shortest_node,g,0);}
# 751
1U;}
# 767
Cyc_PrattProver_set_shortest_dist_from_s(s,g,0);
# 770
Cyc_PrattProver_initialize_dist_set(s,g);
while(1){
{struct Cyc_PrattProver_Node*current_shortest_node=Cyc_PrattProver_extract_min(g);
# 774
if(current_shortest_node==0)break;else{
# 776
union Cyc_PrattProver_ShortestPathInfo _Tmp0=current_shortest_node->shortest_path_info;int _Tmp1;if(_Tmp0.Current.tag==3){_Tmp1=_Tmp0.Current.val;{int d=_Tmp1;
# 779
({union Cyc_PrattProver_ShortestPathInfo _Tmp2=({union Cyc_PrattProver_ShortestPathInfo _Tmp3;_Tmp3.Shortest.tag=2U,_Tmp3.Shortest.val=d;_Tmp3;});current_shortest_node->shortest_path_info=_Tmp2;});
goto _LL5;}}else{
# 782
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;});struct _fat_ptr _Tmp3=Cyc_aprintf(_tag_fat("current_shortest_node should always contain &Current(d)\n",sizeof(char),57U),_tag_fat(0U,sizeof(void*),0));_Tmp2(_Tmp3,_tag_fat(0U,sizeof(void*),0));});
goto _LL5;}_LL5:;}
# 786
Cyc_PrattProver_relaxation(current_shortest_node,g,1);}
# 772
1U;}
# 788
Cyc_PrattProver_set_shortest_dist_from_s(s,g,1);
return;}
# 799 "pratt_prover.cyc"
static void Cyc_PrattProver_johnson(int dummy,struct Cyc_PrattProver_Graph*g){
# 805
if(!g->changed)return;
# 813
Cyc_PrattProver_bellman_ford(0,g);
# 820
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
struct Cyc_PrattProver_Distance*pre_dists;struct Cyc_PrattProver_Distance*dists;
for((pre_dists=0,dists=node->unsigned_distances);dists!=0;(
pre_dists=dists,dists=dists->next)){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
# 830
dists->dist=(uv_unsigned + du_unsigned)- dv_unsigned;
# 832
if(dists->dist < 0){
# 835
if(pre_dists==0)
node->unsigned_distances=dists->next;else{
# 838
pre_dists->next=dists->next;}}}{
# 841
int du_signed=node->signeddistFromS;
for((pre_dists=0,dists=node->signed_distances);dists!=0;(
pre_dists=dists,dists=dists->next)){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 847
dists->dist=(uv_signed + du_signed)- dv_signed;
# 849
if(dists->dist < 0){
dists->dist=0;
if(pre_dists==0)
node->signed_distances=dists->next;else{
# 854
pre_dists->next=dists->next;}}}}}}
# 866
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
Cyc_PrattProver_dijkstra(node,g);}}
# 876
{struct Cyc_PrattProver_Node*node=g->rows;for(0;node!=0;node=node->next){
int du_unsigned=node->unsigneddistFromS;
{struct Cyc_PrattProver_Distance*dists=node->unsigned_distances;for(0;dists!=0;dists=dists->next){
int uv_unsigned=dists->dist;
int dv_unsigned=dists->target->unsigneddistFromS;
dists->dist=(uv_unsigned + dv_unsigned)- du_unsigned;}}{
# 883
int du_signed=node->signeddistFromS;
struct Cyc_PrattProver_Distance*dists=node->signed_distances;for(0;dists!=0;dists=dists->next){
int uv_signed=dists->dist;
int dv_signed=dists->target->signeddistFromS;
# 888
dists->dist=(uv_signed + dv_signed)- du_signed;}}}}
# 895
return;}
# 898
static void Cyc_PrattProver_set_distance(struct Cyc_PrattProver_Graph*g,void*it,void*jt,unsigned dist,int is_signed){
struct Cyc_PrattProver_Node*i=Cyc_PrattProver_term2node(g,it);
struct Cyc_PrattProver_Node*j=Cyc_PrattProver_term2node(g,jt);
Cyc_PrattProver_set_dist(g,i,j,(int)dist,is_signed);}
# 905
static struct Cyc_PrattProver_Distance*Cyc_PrattProver_copy_distances(struct Cyc_PrattProver_Graph*newg,struct Cyc_PrattProver_Distance*_ds){
struct Cyc_PrattProver_Distance*res=0;
struct Cyc_PrattProver_Distance*ds=_ds;
for(1;ds!=0;ds=ds->next){
void*t=ds->target->rep;
res=({struct Cyc_PrattProver_Distance*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Distance));_Tmp0->next=res,({struct Cyc_PrattProver_Node*_Tmp1=Cyc_PrattProver_term2node(newg,t);_Tmp0->target=_Tmp1;}),_Tmp0->dist=ds->dist;_Tmp0;});}
# 912
return res;}
# 916
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
# 922
1U;}
# 927
return first;}}
# 932
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_copy_graphs(struct Cyc_PrattProver_Graph*_gopt){
struct Cyc_PrattProver_Graph*res=0;
struct Cyc_PrattProver_Graph*gopt=_gopt;
for(1;gopt!=0;gopt=gopt->next){
++ Cyc_PrattProver_graphs_copied;{
struct Cyc_PrattProver_Graph*g=gopt;
# 939
struct Cyc_PrattProver_Graph*newg;newg=_cycalloc(sizeof(struct Cyc_PrattProver_Graph)),newg->next=res,newg->rows=0,({struct Cyc_Hashtable_Table*_Tmp0=Cyc_PrattProver_empty_map(17U);newg->map=_Tmp0;}),newg->changed=g->changed;
# 941
res=newg;{
# 943
struct Cyc_PrattProver_Node*newrs=0;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
newrs=({struct Cyc_PrattProver_Node*_Tmp0=_cycalloc(sizeof(struct Cyc_PrattProver_Node));_Tmp0->next=newrs,_Tmp0->rep=rs->rep,_Tmp0->broken_as_signed=rs->broken_as_signed,_Tmp0->broken_as_unsigned=rs->broken_as_unsigned,_Tmp0->unsigned_distances=0,_Tmp0->signed_distances=0,_Tmp0->signeddistFromS=0,_Tmp0->unsigneddistFromS=0,_Tmp0->shortest_path_info=Cyc_PrattProver_infinity;_Tmp0;});
# 947
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_PrattProver_Node*))Cyc_Hashtable_insert;_Tmp0;})(newg->map,rs->rep,newrs);}}
# 949
newrs=Cyc_PrattProver_revnodes(newrs);
newg->rows=newrs;{
# 952
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;(rs=rs->next,newrs=newrs->next)){
({struct Cyc_PrattProver_Distance*_Tmp0=Cyc_PrattProver_copy_distances(newg,rs->unsigned_distances);newrs->unsigned_distances=_Tmp0;});
({struct Cyc_PrattProver_Distance*_Tmp0=Cyc_PrattProver_copy_distances(newg,rs->signed_distances);newrs->signed_distances=_Tmp0;});}}}}}
# 957
return res;}
# 961
static int Cyc_PrattProver_graphs_change=0;
# 966
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_app_graphs(void(*f)(void*,struct Cyc_PrattProver_Graph*),void*e,struct Cyc_PrattProver_Graph*gs){
struct Cyc_PrattProver_Graph*prev=0;
{struct Cyc_PrattProver_Graph*g=gs;for(0;g!=0;g=g->next){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
f(e,g);
prev=g;
# 970
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_PrattProver_Inconsistent_exn_struct*)_Tmp2)->tag==Cyc_PrattProver_Inconsistent){
# 974
Cyc_PrattProver_graphs_change=1;
if(prev==0)
gs=g->next;else{
# 978
prev->next=g->next;}
# 980
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}}
# 983
return gs;}
# 986
static int Cyc_PrattProver_is_relation(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Gt:
 goto _LL4;case Cyc_Absyn_Lt: _LL4:
 goto _LL6;case Cyc_Absyn_Gte: _LL6:
 goto _LL8;case Cyc_Absyn_Lte: _LL8:
 return 1;default:
 return 0;};}
# 996
static unsigned Cyc_PrattProver_eval_binop(enum Cyc_Absyn_Primop p,unsigned c1,unsigned c2){
# 1000
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
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp1)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp1;});struct _fat_ptr _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_prim2string(p);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("Invalid binop %s during constant evaluation",sizeof(char),44U),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0(_Tmp1,_tag_fat(0U,sizeof(void*),0));});};
# 1015
return 0U;}
# 1018
static unsigned Cyc_PrattProver_eval_unop(enum Cyc_Absyn_Primop p,unsigned c){
switch((int)p){case Cyc_Absyn_Not:
 return(unsigned)!((int)c);case Cyc_Absyn_Bitnot:
 return ~ c;case Cyc_Absyn_Plus:
 return c;case Cyc_Absyn_Minus:
 return - c;default:
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp1)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp1;});struct _fat_ptr _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_prim2string(p);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(_tag_fat("Invalid unop %s during constant evaluation",sizeof(char),43U),_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp0(_Tmp1,_tag_fat(0U,sizeof(void*),0));});};
# 1026
return 0U;}struct _tuple14{int f0;unsigned f1;};
# 1029
static struct _tuple14 Cyc_PrattProver_eq_node_const(struct Cyc_PrattProver_Node*n,struct Cyc_PrattProver_Node*z,int is_signed){
int*n2z=Cyc_PrattProver_lookup_dist(n,z,is_signed);
int*z2n=Cyc_PrattProver_lookup_dist(z,n,is_signed);
if((n2z!=0 && z2n!=0)&&*z2n==*n2z){struct _tuple14 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=(unsigned)*n2z;return _Tmp0;}{
struct _tuple14 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=2989U;return _Tmp0;}}
# 1038
static struct _tuple14 Cyc_PrattProver_equal_node_const(struct Cyc_PrattProver_Node*n,struct Cyc_PrattProver_Node*z){
struct _tuple14 _Tmp0=Cyc_PrattProver_eq_node_const(n,z,1);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c=_Tmp1;
if(!ok)return Cyc_PrattProver_eq_node_const(n,z,0);else{
struct _tuple14 _Tmp3;_Tmp3.f0=ok,_Tmp3.f1=c;return _Tmp3;}}}
# 1045
static struct _tuple11 Cyc_PrattProver_subst_term_with_const(struct Cyc_PrattProver_Graph*g,void*t){
void*newterm=0;
{void*_Tmp0;void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;unsigned _Tmp4;switch(*((int*)t)){case 0: _Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned c=_Tmp4;
struct _tuple11 _Tmp5;_Tmp5.f0=1,_Tmp5.f1=c,_Tmp5.f2=0;return _Tmp5;}case 4: _Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp2;void*t2=_Tmp1;void*topt=_Tmp0;
# 1053
if(Cyc_PrattProver_is_relation(p))goto _LL0;{
struct _tuple11 _Tmp5=Cyc_PrattProver_subst_term_with_const(g,t1);void*_Tmp6;unsigned _Tmp7;int _Tmp8;_Tmp8=_Tmp5.f0;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{int ok1=_Tmp8;unsigned c1=_Tmp7;void*nt1=_Tmp6;
struct _tuple11 _Tmp9=Cyc_PrattProver_subst_term_with_const(g,t2);void*_TmpA;unsigned _TmpB;int _TmpC;_TmpC=_Tmp9.f0;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{int ok2=_TmpC;unsigned c2=_TmpB;void*nt2=_TmpA;
# 1058
if(nt1!=0){
newterm=Cyc_AssnDef_binop(p,nt1,t2,topt);
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1062
if(nt2!=0){
newterm=Cyc_AssnDef_binop(p,t1,nt2,topt);
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1066
if(ok1){
newterm=({enum Cyc_Absyn_Primop _TmpD=p;void*_TmpE=Cyc_AssnDef_uint(c1);void*_TmpF=t2;Cyc_AssnDef_binop(_TmpD,_TmpE,_TmpF,topt);});
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1070
if(ok2){
newterm=({enum Cyc_Absyn_Primop _TmpD=p;void*_TmpE=t1;void*_TmpF=Cyc_AssnDef_uint(c2);Cyc_AssnDef_binop(_TmpD,_TmpE,_TmpF,topt);});
Cyc_PrattProver_add_eq(g,t,newterm);}
# 1074
if(ok1 && ok2){
struct _tuple11 _TmpD;_TmpD.f0=1,({unsigned _TmpE=Cyc_PrattProver_eval_binop(p,c1,c2);_TmpD.f1=_TmpE;}),_TmpD.f2=0;return _TmpD;}
# 1077
goto _LL0;}}}}case 3: _Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp2;void*topt=_Tmp1;
# 1079
if((int)p==18)goto _LL0;{
struct _tuple11 _Tmp5=Cyc_PrattProver_subst_term_with_const(g,t1);unsigned _Tmp6;int _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{int ok1=_Tmp7;unsigned c1=_Tmp6;
if(ok1){
({struct Cyc_PrattProver_Graph*_Tmp8=g;void*_Tmp9=t;Cyc_PrattProver_add_eq(_Tmp8,_Tmp9,Cyc_AssnDef_uint(Cyc_PrattProver_eval_unop(p,c1)));});{
struct _tuple11 _Tmp8;_Tmp8.f0=1,({unsigned _Tmp9=Cyc_PrattProver_eval_unop(p,c1);_Tmp8.f1=_Tmp9;}),_Tmp8.f2=0;return _Tmp8;}}
# 1085
goto _LL0;}}}default:
 goto _LL0;}_LL0:;}
# 1088
if(t==newterm)newterm=0;{
struct Cyc_PrattProver_Node*n_node=Cyc_PrattProver_term2node(g,t);
struct Cyc_PrattProver_Node*z_node=({struct Cyc_PrattProver_Graph*_Tmp0=g;Cyc_PrattProver_term2node(_Tmp0,Cyc_AssnDef_zero());});
struct _tuple14 _Tmp0=Cyc_PrattProver_equal_node_const(n_node,z_node);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c=_Tmp1;
if(ok){struct _tuple11 _Tmp3;_Tmp3.f0=1,_Tmp3.f1=c,_Tmp3.f2=newterm;return _Tmp3;}{
struct _tuple11 _Tmp3;_Tmp3.f0=0,_Tmp3.f1=2989U,_Tmp3.f2=newterm;return _Tmp3;}}}}
# 1099
inline static int Cyc_PrattProver_better_term(void*t1,void*t2){
# 1101
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});int _Tmp1;int _Tmp2;if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2){_Tmp2=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f2;{int i=_Tmp2;int j=_Tmp1;
# 1104
return i < j;}}else{
# 1106
return 0;}}else{if(*((int*)_Tmp0.f1)==2)
# 1105
return 1;else{
# 1128 "pratt_prover.cyc"
return 0;}};}
# 1133
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
# 1144
for(1;ss!=0;ss=ss->tl){if(Cyc_PrattProver_occurs(t1,(void*)ss->hd))return 1;}
return 0;}case 9: _Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t2)->f1;{void*s=_Tmp2;
t2=s;goto loop;}case 10: _Tmp2=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t2)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t2)->f3;{void*s1=_Tmp2;void*s2=_Tmp1;
# 1148
if(Cyc_PrattProver_occurs(t1,s1))return 1;t2=s2;goto loop;}case 11: _Tmp2=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t2)->f2;{void*s=_Tmp2;
t2=s;goto loop;}case 12: _Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t2)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t2)->f4;{void*s1=_Tmp2;void*s2=_Tmp1;
# 1151
if(Cyc_PrattProver_occurs(t1,s1))return 1;t2=s2;goto loop;}case 13: _Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t2)->f1;{void*s=_Tmp2;
t2=s;goto loop;}case 14: _Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t2)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t2)->f2;{void*s1=_Tmp2;void*s2=_Tmp1;
if(Cyc_PrattProver_occurs(t1,s1))return 1;t2=s2;goto loop;}case 15: _Tmp2=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t2)->f1;{void*s=_Tmp2;
t2=s;goto loop;}default:
 return 0;};}}
# 1161
static void Cyc_PrattProver_subst_close_graph(int dummy,struct Cyc_PrattProver_Graph*g){
# 1167
struct Cyc_Hashtable_Table*eq=Cyc_AssnDef_empty_term_table();
int changed=0;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
void*t1=rs->rep;
void*best=t1;
# 1173
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
void*t2=ts->rep;
if((Cyc_PrattProver_better_term(t2,best)&& Cyc_PrattProver_eq_nodes(rs,ts,0))&& !Cyc_PrattProver_occurs(t1,t2))
best=t2;}}
# 1179
best=Cyc_AssnDef_subst_table(eq,best);
# 1182
if(!Cyc_PrattProver_occurs(t1,best)){
changed=1;
Cyc_Hashtable_insert(eq,t1,best);}}}
# 1187
if(!changed)return;{
# 1193
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
void*t=rs->rep;
void*t2=Cyc_AssnDef_subst_table(eq,t);
if(t!=t2)Cyc_PrattProver_add_eq(g,t,t2);}}}
# 1200
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_subst_close_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_subst_close_graph,0,gs);}
# 1204
static int Cyc_PrattProver_address_disjoint(struct Cyc_PrattProver_Graph*,void*,void*);
# 1207
static int Cyc_PrattProver_neq_terms(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;unsigned _Tmp2;unsigned _Tmp3;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp3;unsigned j=_Tmp2;
return i!=j;}}else{_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=_Tmp0.f1;_LL6: {unsigned i=_Tmp3;void*t=_Tmp1;
# 1215
struct Cyc_PrattProver_Node*n=Cyc_PrattProver_term2node(g,t);
struct Cyc_PrattProver_Node*z=({struct Cyc_PrattProver_Graph*_Tmp4=g;Cyc_PrattProver_term2node(_Tmp4,Cyc_AssnDef_zero());});
int*nzud=Cyc_PrattProver_lookup_dist(n,z,0);
if((i!=2147483648U && nzud!=0)&&*nzud < (int)i - 1)return 1;{
int*nzsd=Cyc_PrattProver_lookup_dist(n,z,1);
if((i!=2147483648U && nzsd!=0)&&*nzsd < (int)i - 1)return 1;{
int*znud=Cyc_PrattProver_lookup_dist(z,n,0);
if((i!=2147483647U && znud!=0)&&*znud < -((int)i + 1))return 1;{
int*znsd=Cyc_PrattProver_lookup_dist(z,n,1);
if((i!=2147483647U && znsd!=0)&&*znsd < -((int)i + 1))return 1;
return 0;}}}}}}else{if(*((int*)_Tmp0.f1)==0){_Tmp1=_Tmp0.f0;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*t=_Tmp1;unsigned i=_Tmp3;
# 1210
_Tmp3=i;_Tmp1=t;goto _LL6;}}else{
# 1231
struct Cyc_PrattProver_Node*n1=Cyc_PrattProver_term2node(g,t1);
struct Cyc_PrattProver_Node*n2=Cyc_PrattProver_term2node(g,t2);
int*n12ud=Cyc_PrattProver_lookup_dist(n1,n2,0);
if(n12ud!=0 &&*n12ud < -1)return 1;{
int*n21ud=Cyc_PrattProver_lookup_dist(n2,n1,0);
if(n21ud!=0 &&*n21ud < -1)return 1;{
int*n12sd=Cyc_PrattProver_lookup_dist(n1,n2,1);
if(n12sd!=0 &&*n12sd < -1)return 1;{
int*n21sd=Cyc_PrattProver_lookup_dist(n2,n1,1);
if(n21sd!=0 &&*n21sd < -1)return 1;
return Cyc_PrattProver_address_disjoint(g,t1,t2);}}}}};}
# 1251
static int Cyc_PrattProver_address_disjoint(struct Cyc_PrattProver_Graph*g,void*a1,void*a2){
LOOP: {
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;void*_Tmp6;void*_Tmp7;int _Tmp8;void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp0.f0)){case 11: switch(*((int*)_Tmp0.f1)){case 11: _TmpA=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl*x=_TmpA;struct Cyc_Absyn_Vardecl*y=_Tmp9;
return x!=y;}case 12:
 return 1;case 13: goto _LLD;case 14: goto _LL13;default: goto _LL15;}case 12: switch(*((int*)_Tmp0.f1)){case 11:
 return 1;case 12: _TmpA=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp8=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp6=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp3=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;int i1=_Tmp8;void*t1=_Tmp9;void*nv1=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;int i2=_Tmp5;void*t2=_Tmp4;void*nv2=_Tmp3;
# 1258
return(e1!=e2 || i1!=i2)|| Cyc_PrattProver_neq_terms(g,t1,t2);}case 13: goto _LLD;case 14: goto _LL13;default: goto _LL15;}case 13: if(*((int*)_Tmp0.f1)==13){_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*t1=_TmpA;unsigned i1=_Tmp2;void*t2=_Tmp9;unsigned i2=_Tmp1;
# 1260
if(Cyc_PrattProver_equal_terms(g,t1,t2)&& i1!=i2)return 1;
a1=t1;a2=t2;goto LOOP;}}else{_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;{void*t1=_TmpA;
a1=t1;goto LOOP;}}default: if(*((int*)_Tmp0.f1)==13){_LLD: _TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*t2=_TmpA;
a2=t2;goto LOOP;}}else{if(*((int*)_Tmp0.f0)==14){if(*((int*)_Tmp0.f1)==14){_TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*t1=_TmpA;void*t2=_Tmp9;void*t3=_Tmp7;void*t4=_Tmp6;
# 1265
if(Cyc_PrattProver_equal_terms(g,t1,t3)&& Cyc_PrattProver_neq_terms(g,t2,t4))return 1;
a1=t1;a2=t2;goto LOOP;}}else{_TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;{void*t1=_TmpA;
a1=t1;goto LOOP;}}}else{if(*((int*)_Tmp0.f1)==14){_LL13: _TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*t2=_TmpA;
a2=t2;goto LOOP;}}else{_LL15:
 return 0;}}}};}}
# 1275
static void Cyc_PrattProver_reduce_terms(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
void*t=rs->rep;
unsigned _Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)t)){case 6: if(*((int*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1)==7){_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*m=_Tmp5;void*a1=_Tmp4;void*v=_Tmp3;void*a2=_Tmp2;void*topt=_Tmp1;
# 1281
if(Cyc_PrattProver_equal_terms(g,a1,a2))
Cyc_PrattProver_add_eq(g,t,v);else{
# 1284
if(Cyc_PrattProver_address_disjoint(g,a1,a2))
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_select(m,a2,topt));});}
# 1287
goto _LL0;}}else{goto _LLB;}case 7: if(*((int*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1)==7){_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*m=_Tmp5;void*a1=_Tmp4;void*v1=_Tmp3;void*a2=_Tmp2;void*v2=_Tmp1;
# 1290
if(Cyc_PrattProver_equal_terms(g,a1,a2))
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_update(m,a2,v2));});else{
# 1294
if(Cyc_PrattProver_address_disjoint(g,a1,a2)&& Cyc_AssnDef_termcmp(a1,a2)< 0)
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,({void*_Tmp8=Cyc_AssnDef_update(m,a2,v2);void*_Tmp9=a1;Cyc_AssnDef_update(_Tmp8,_Tmp9,v1);}));});}
# 1297
goto _LL0;}}else{goto _LLB;}case 9: _Tmp5=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;unsigned i=_Tmp0;void*topt=_Tmp4;
# 1300
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
void*u=ts->rep;
if(*((int*)u)==8){
# 1304
if(Cyc_PrattProver_eq_nodes(rs,ts,0)){
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_proj(u,i,topt));});goto Found1;}
# 1307
goto _LLD;}else{
goto _LLD;}_LLD:;}}
# 1311
Found1:
 goto _LL0;}case 10: _Tmp5=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;unsigned i=_Tmp0;void*t2=_Tmp4;
# 1315
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
void*u=ts->rep;
if(*((int*)u)==8){
# 1319
if(Cyc_PrattProver_eq_nodes(rs,ts,0)){
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_aggr_update(u,i,t2));});goto Found2;}
# 1322
goto _LL12;}else{
goto _LL12;}_LL12:;}}
# 1326
Found2:
 goto _LL0;}case 15: _Tmp5=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp5;
# 1330
{struct Cyc_PrattProver_Node*ts=g->rows;for(0;ts!=0;ts=ts->next){
void*u=ts->rep;
if(*((int*)u)==8){
# 1334
if(Cyc_PrattProver_eq_nodes(rs,ts,0)){
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_tagof_tm(u));});goto Found3;}
# 1337
goto _LL17;}else{
goto _LL17;}_LL17:;}}
# 1341
Found3:
 goto _LL0;}default: _LLB:
 goto _LL0;}_LL0:;}}
# 1348
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_reduce_close_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_reduce_terms,0,gs);}struct _tuple15{int f0;void*f1;int f2;};
# 1353
static struct _tuple15 Cyc_PrattProver_break_term(struct Cyc_PrattProver_Graph*g,void*t,int is_signed){
{unsigned _Tmp0;void*_Tmp1;if(*((int*)t)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned c2=_Tmp0;
# 1356
if((int)c2 >= 0){
if(is_signed){
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=Cyc_PrattProver_term2node(g,t1);Cyc_PrattProver_lookup_dist(_Tmp2,({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());}),1);});
# 1360
if(dist!=0 &&*dist <= 2147483647 - (int)c2){
struct _tuple15 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;return _Tmp2;}}else{
# 1365
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=Cyc_PrattProver_term2node(g,t1);Cyc_PrattProver_lookup_dist(_Tmp2,({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());}),0);});
if(dist!=0){
struct _tuple15 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;return _Tmp2;}}}else{
# 1371
if(is_signed){
# 1373
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp2,Cyc_PrattProver_term2node(g,t1),1);});
if(dist!=0 &&*dist <= (int)(c2 - 2147483648U)){
struct _tuple15 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;return _Tmp2;}}else{
# 1379
int*dist=({struct Cyc_PrattProver_Node*_Tmp2=({struct Cyc_PrattProver_Graph*_Tmp3=g;Cyc_PrattProver_term2node(_Tmp3,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp2,Cyc_PrattProver_term2node(g,t1),0);});
if(dist!=0 &&*dist <= (int)c2){
struct _tuple15 _Tmp2;_Tmp2.f0=1,_Tmp2.f1=t1,_Tmp2.f2=(int)c2;return _Tmp2;}}}
# 1385
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
# 1387
 goto _LL0;}_LL0:;}{
# 1389
struct _tuple15 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=t,_Tmp0.f2=0;return _Tmp0;}}
# 1392
static void Cyc_PrattProver_break_term_in_graph(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
if(!rs->broken_as_signed){
struct _tuple15 _Tmp0=Cyc_PrattProver_break_term(g,rs->rep,1);int _Tmp1;void*_Tmp2;int _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int ok=_Tmp3;void*t1=_Tmp2;int c1=_Tmp1;
if(ok){
rs->broken_as_signed=1;
Cyc_PrattProver_set_distance(g,rs->rep,t1,(unsigned)c1,1);
if((unsigned)c1!=2147483648U)
Cyc_PrattProver_set_distance(g,t1,rs->rep,(unsigned)(- c1),1);}}}
# 1403
if(!rs->broken_as_unsigned){
struct _tuple15 _Tmp0=Cyc_PrattProver_break_term(g,rs->rep,0);int _Tmp1;void*_Tmp2;int _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{int ok=_Tmp3;void*t1=_Tmp2;int c1=_Tmp1;
if(ok){
rs->broken_as_unsigned=1;
Cyc_PrattProver_set_distance(g,rs->rep,t1,(unsigned)c1,0);
if((unsigned)c1!=2147483648U)
Cyc_PrattProver_set_distance(g,t1,rs->rep,(unsigned)(- c1),0);}}}{
# 1412
struct _tuple11 _Tmp0=Cyc_PrattProver_subst_term_with_const(g,rs->rep);unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int ok=_Tmp2;unsigned c1=_Tmp1;
# 1414
if(ok){
void*_Tmp3=rs->rep;unsigned _Tmp4;void*_Tmp5;if(*((int*)_Tmp3)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f3)==0){_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f2;_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp3)->f3)->f1;{void*t1=_Tmp5;unsigned c2=_Tmp4;
# 1417
rs->broken_as_signed=1;
rs->broken_as_unsigned=1;
({struct Cyc_PrattProver_Graph*_Tmp6=g;void*_Tmp7=t1;Cyc_PrattProver_add_eq(_Tmp6,_Tmp7,Cyc_AssnDef_uint(c1 - c2));});
goto _LL9;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC:
# 1422
 goto _LL9;}_LL9:;}}}}}
# 1428
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_break_term_in_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_break_term_in_graph,0,gs);}
# 1437
static int Cyc_PrattProver_range_of_term(struct Cyc_PrattProver_Graph*g,struct Cyc_PrattProver_Node*t){
struct Cyc_PrattProver_Node*zero_node=({struct Cyc_PrattProver_Graph*_Tmp0=g;Cyc_PrattProver_term2node(_Tmp0,Cyc_AssnDef_zero());});
# 1440
int*dist=Cyc_PrattProver_lookup_dist(zero_node,t,0);
if(dist!=0 &&(unsigned)*dist==2147483648U)
return 1;{
# 1444
int*dist=Cyc_PrattProver_lookup_dist(t,zero_node,0);
if(dist!=0 &&(unsigned)*dist <= 2147483647U)
return -1;{
# 1448
int*dist=Cyc_PrattProver_lookup_dist(zero_node,t,1);
if(dist!=0 &&*dist <= 0)
return 1;{
# 1452
int*dist=Cyc_PrattProver_lookup_dist(t,zero_node,1);
if(dist!=0 &&*dist <= -1)
return -1;
# 1456
return 0;}}}}
# 1467 "pratt_prover.cyc"
static void Cyc_PrattProver_associate_ud_sd_in_graph(int dummy,struct Cyc_PrattProver_Graph*g){
int range_of_src;int range_of_tgt;
{struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
range_of_src=Cyc_PrattProver_range_of_term(g,rs);
if(range_of_src!=0){
{struct Cyc_PrattProver_Distance*uds=rs->unsigned_distances;for(0;uds!=0;uds=uds->next){
range_of_tgt=Cyc_PrattProver_range_of_term(g,uds->target);
if(range_of_src==range_of_tgt)
Cyc_PrattProver_set_distance(g,rs->rep,uds->target->rep,(unsigned)uds->dist,1);}}{
# 1478
struct Cyc_PrattProver_Distance*sds=rs->signed_distances;for(0;sds!=0;sds=sds->next){
range_of_tgt=Cyc_PrattProver_range_of_term(g,sds->target);
if(range_of_src==range_of_tgt)
Cyc_PrattProver_set_distance(g,rs->rep,sds->target->rep,(unsigned)sds->dist,0);}}}}}
# 1486
return;}
# 1489
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_associate_ud_sd_in_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_associate_ud_sd_in_graph,0,gs);}
# 1496
static void Cyc_PrattProver_add_constraint(struct Cyc_PrattProver_Graph*g,void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
++ Cyc_PrattProver_constraints_added;
{struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned c1=_Tmp2;unsigned c2=_Tmp1;
# 1500
switch((int)p){case Cyc_AssnDef_ULt:
 if(c1 < c2)return;goto _LL9;case Cyc_AssnDef_ULte:
 if(c1 <= c2)return;goto _LL9;case Cyc_AssnDef_SLt:
 if((int)c1 < (int)c2)return;goto _LL9;case Cyc_AssnDef_SLte:
 if((int)c1 <= (int)c2)return;goto _LL9;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));}_LL9:;
# 1510
_throw(& Cyc_PrattProver_inconsistent);}}else{_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;{unsigned c1=_Tmp2;
# 1549
switch((int)p){case Cyc_AssnDef_ULt:
# 1552
 if(c1==4294967295U)
# 1556
_throw(& Cyc_PrattProver_inconsistent);
# 1558
c1=c1 + 1U;
goto _LL23;case Cyc_AssnDef_ULte: _LL23:
# 1562
 if(c1 > 2147483647U)
# 1565
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,t2,2147483648U,0);});else{
# 1570
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)(-(int)c1),0);});}
# 1572
return;case Cyc_AssnDef_SLt:
# 1575
 if(c1==2147483647U)
# 1579
_throw(& Cyc_PrattProver_inconsistent);
# 1581
c1=c1 + 1U;
goto _LL27;case Cyc_AssnDef_SLte: _LL27:
# 1586
 if(c1==2147483648U)return;{
struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_zero();void*_Tmp5=t2;return Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)(-(int)c1),1);}default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));};}}}else{if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned c2=_Tmp2;
# 1513
switch((int)p){case Cyc_AssnDef_ULt:
# 1516
 if(c2==0U)
# 1520
_throw(& Cyc_PrattProver_inconsistent);
# 1522
c2=c2 - 1U;
goto _LL18;case Cyc_AssnDef_ULte: _LL18:
# 1526
 if(c2 <= 2147483647U)
# 1528
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)((int)c2),0);});
# 1530
return;case Cyc_AssnDef_SLt:
# 1533
 if(c2==2147483648U)
# 1537
_throw(& Cyc_PrattProver_inconsistent);
# 1539
c2=c2 - 1U;
goto _LL1C;case Cyc_AssnDef_SLte: _LL1C:
# 1543
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=t1;void*_Tmp5=Cyc_AssnDef_zero();Cyc_PrattProver_set_distance(_Tmp3,_Tmp4,_Tmp5,(unsigned)((int)c2),1);});
return;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Vcgen: found bad primop in add_constraint",sizeof(char),42U),_tag_fat(0U,sizeof(void*),0));};}}else{
# 1590
goto _LL0;}}_LL0:;}
# 1594
switch((int)p){case Cyc_AssnDef_ULt:
# 1597
 return Cyc_PrattProver_set_distance(g,t1,t2,-1,0);case Cyc_AssnDef_ULte:
# 1600
 return Cyc_PrattProver_set_distance(g,t1,t2,0U,0);case Cyc_AssnDef_SLt:
# 1603
 return Cyc_PrattProver_set_distance(g,t1,t2,-1,1);case Cyc_AssnDef_SLte:
# 1606
 return Cyc_PrattProver_set_distance(g,t1,t2,0U,1);default:
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Vcgen:found bad primop in add_constraint",sizeof(char),41U),_tag_fat(0U,sizeof(void*),0));};}
# 1612
static void Cyc_PrattProver_add_eq(struct Cyc_PrattProver_Graph*g,void*t1,void*t2){
if(t1==t2)return;
Cyc_PrattProver_add_constraint(g,t1,5U,t2);
Cyc_PrattProver_add_constraint(g,t2,5U,t1);
Cyc_PrattProver_add_constraint(g,t1,3U,t2);
Cyc_PrattProver_add_constraint(g,t2,3U,t1);}struct _tuple16{void*f0;enum Cyc_AssnDef_Primreln f1;void*f2;};
# 1620
static void Cyc_PrattProver_add_prim(struct _tuple16*p,struct Cyc_PrattProver_Graph*g){
struct _tuple16 _Tmp0=*p;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
if((int)p==0)
Cyc_PrattProver_add_eq(g,t1,t2);else{
# 1625
Cyc_PrattProver_add_constraint(g,t1,p,t2);}}}char Cyc_PrattProver_TooLarge[9U]="TooLarge";struct Cyc_PrattProver_TooLarge_exn_struct{char*tag;};
# 1629
struct Cyc_PrattProver_TooLarge_exn_struct Cyc_PrattProver_too_large={Cyc_PrattProver_TooLarge};
# 1719 "pratt_prover.cyc"
static struct Cyc_Set_Set*Cyc_PrattProver_get_pointer_terms(void*t,struct Cyc_Set_Set*s){
LOOP: {
void*topt=Cyc_AssnDef_get_term_type(t);
if((topt!=0 && Cyc_Tcutil_is_nullable_pointer_type(topt,0))&&
 Cyc_Tcutil_get_type_bound(topt)!=0)
s=Cyc_Set_insert(s,t);
{void*_Tmp0;void*_Tmp1;switch(*((int*)t)){case 5: _Tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp1;
t=t1;goto LOOP;}case 3: _Tmp1=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp1;
t=t1;goto LOOP;}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 1729
s=Cyc_PrattProver_get_pointer_terms(t1,s);
t=t2;goto LOOP;}default:
 goto _LL0;}_LL0:;}
# 1733
return s;}}
# 1736
static struct Cyc_Set_Set*Cyc_PrattProver_get_graph_pointer_terms(struct Cyc_PrattProver_Graph*_g,struct Cyc_Set_Set*s){
# 1738
struct Cyc_PrattProver_Graph*g=_g;
for(1;g!=0;g=g->next){
struct Cyc_PrattProver_Node*rs=g->rows;for(0;rs!=0;rs=rs->next){
s=Cyc_PrattProver_get_pointer_terms(rs->rep,s);}}
return s;}struct _tuple17{void*f0;void*f1;void*f2;};
# 1745
static void Cyc_PrattProver_add_ptr_info(struct _tuple17*env,struct Cyc_PrattProver_Graph*g){
struct _tuple17 _Tmp0=*env;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t=_Tmp3;void*tnumelts=_Tmp2;void*bnd=_Tmp1;
# 1748
int*dist=({struct Cyc_PrattProver_Node*_Tmp4=({struct Cyc_PrattProver_Graph*_Tmp5=g;Cyc_PrattProver_term2node(_Tmp5,Cyc_AssnDef_zero());});Cyc_PrattProver_lookup_dist(_Tmp4,Cyc_PrattProver_term2node(g,t),0);});
if(dist!=0 &&*dist <= -1){
Cyc_PrattProver_add_constraint(g,bnd,5U,tnumelts);
Cyc_PrattProver_add_constraint(g,bnd,3U,tnumelts);}}}
# 1755
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_add_ptr_type_info(void*t,struct Cyc_PrattProver_Graph*gs){
void*type=_check_null(Cyc_AssnDef_get_term_type(t));
struct Cyc_Absyn_Exp*e=Cyc_PrattProver_strip_cast(_check_null(Cyc_Tcutil_get_type_bound(type)));
void*bnd=Cyc_AssnDef_cnst(e);
void*tnumelts=Cyc_AssnDef_numelts_term(t);
struct _tuple17 env=({struct _tuple17 _Tmp0;_Tmp0.f0=t,_Tmp0.f1=tnumelts,_Tmp0.f2=bnd;_Tmp0;});
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(struct _tuple17*,struct Cyc_PrattProver_Graph*),struct _tuple17*,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(struct _tuple17*,struct Cyc_PrattProver_Graph*),struct _tuple17*,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_add_ptr_info,& env,gs);}
# 1768
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_add_type_assns(struct Cyc_PrattProver_Graph*gs){
struct Cyc_Set_Set*ptrs=({struct Cyc_PrattProver_Graph*_Tmp0=gs;Cyc_PrattProver_get_graph_pointer_terms(_Tmp0,Cyc_Set_empty(Cyc_AssnDef_termcmp));});
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(struct Cyc_PrattProver_Graph*(*)(void*,struct Cyc_PrattProver_Graph*),struct Cyc_Set_Set*,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(struct Cyc_PrattProver_Graph*(*)(void*,struct Cyc_PrattProver_Graph*),struct Cyc_Set_Set*,struct Cyc_PrattProver_Graph*))Cyc_Set_fold;_Tmp0;})(Cyc_PrattProver_add_ptr_type_info,ptrs,gs);}
# 1774
static void Cyc_PrattProver_add_numelts_info(int dummy,struct Cyc_PrattProver_Graph*g){
struct Cyc_PrattProver_Node*zeronode=({struct Cyc_PrattProver_Graph*_Tmp0=g;Cyc_PrattProver_term2node(_Tmp0,Cyc_AssnDef_zero());});
# 1777
struct Cyc_PrattProver_Node*n=g->rows;for(0;n!=0;n=n->next){
void*_Tmp0=n->rep;unsigned _Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f1==Cyc_Absyn_Numelts){if(*((int*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f2)==14){if(*((int*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f2)->f2)==0){_Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f2)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0)->f2)->f2)->f1;{void*x=_Tmp2;unsigned uk=_Tmp1;
# 1780
int k=(int)uk;
if(k > 0){
# 1783
void*numelts_x=Cyc_AssnDef_numelts_term(x);
struct Cyc_PrattProver_Node*n_numelts_x=Cyc_PrattProver_term2node(g,numelts_x);
# 1786
int*dist_opt=Cyc_PrattProver_lookup_dist(zeronode,n_numelts_x,0);
if(dist_opt!=0){
int j=*dist_opt;
# 1790
int negj=- j;
# 1792
if(negj > 0){
int c=negj - 1;
# 1795
if(k <= c)
# 1797
({struct Cyc_PrattProver_Graph*_Tmp3=g;void*_Tmp4=Cyc_AssnDef_uint((unsigned)(c - k));Cyc_PrattProver_add_constraint(_Tmp3,_Tmp4,4U,n->rep);});}}}
# 1802
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}}
# 1808
static struct Cyc_PrattProver_Graph*Cyc_PrattProver_add_numelts_info_graphs(struct Cyc_PrattProver_Graph*gs){
return({struct Cyc_PrattProver_Graph*(*_Tmp0)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*)=(struct Cyc_PrattProver_Graph*(*)(void(*)(int,struct Cyc_PrattProver_Graph*),int,struct Cyc_PrattProver_Graph*))Cyc_PrattProver_app_graphs;_Tmp0;})(Cyc_PrattProver_add_numelts_info,0,gs);}struct _tuple18{struct Cyc_PrattProver_Graph*f0;struct Cyc_List_List*f1;};
# 1813
static int Cyc_PrattProver_inconsist(void*a){
# 1815
struct Cyc_PrattProver_Graph*g=Cyc_PrattProver_true_graph();
# 1818
struct Cyc_List_List*ands=0;
# 1821
struct Cyc_List_List*ors=0;
# 1823
int paths=0;
# 1828
while(1){
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
while(1){
{enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)a)){case 3: _Tmp4=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp4;void*a2=_Tmp3;
# 1837
a=a2;
ands=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=a1,_Tmp5->tl=ands;_Tmp5;});
continue;}case 4: _Tmp4=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp4;void*a2=_Tmp3;
# 1842
++ paths;
if((unsigned)paths > Cyc_Flags_max_vc_paths){
# 1845
a=Cyc_AssnDef_widen(a);
-- paths;
continue;}{
# 1852
struct Cyc_PrattProver_Graph*g2=Cyc_PrattProver_copy_graphs(g);
ors=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp6=({struct _tuple18*_Tmp7=_cycalloc(sizeof(struct _tuple18));_Tmp7->f0=g2,({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=a2,_Tmp9->tl=ands;_Tmp9;});_Tmp7->f1=_Tmp8;});_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=ors;_Tmp5;});
a=a1;
continue;}}case 2: if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1)->f1==0){if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){_Tmp4=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t2=_Tmp4;
_Tmp4=t2;goto _LL8;}}else{goto _LLB;}}else{if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)->f1==0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}else{goto _LLB;}}}else{if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2==Cyc_AssnDef_Neq){if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3)->f1==0){_LL7: _Tmp4=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_LL8: {void*t1=_Tmp4;
# 1861
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt==0 || Cyc_Tcutil_is_signed_type(topt)){
_Tmp4=t1;_Tmp3=Cyc_AssnDef_zero();goto _LLA;}
a=({void*_Tmp5=Cyc_AssnDef_zero();Cyc_AssnDef_ult(_Tmp5,t1);});
continue;}}else{goto _LL9;}}else{_LL9: _Tmp4=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;_LLA: {void*t1=_Tmp4;void*t2=_Tmp3;
# 1868
a=({void*_Tmp5=({void*_Tmp6=Cyc_AssnDef_slt(t1,t2);Cyc_AssnDef_or(_Tmp6,Cyc_AssnDef_slt(t2,t1));});Cyc_AssnDef_and(_Tmp5,({void*_Tmp6=Cyc_AssnDef_ult(t1,t2);Cyc_AssnDef_or(_Tmp6,Cyc_AssnDef_ult(t2,t1));}));});
continue;}}}else{_LLB: _Tmp4=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp4;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp3;
# 1871
if((int)p==0)
# 1874
Cyc_PrattProver_add_eq(g,t1,t2);else{
# 1878
Cyc_PrattProver_add_constraint(g,t1,p,t2);}
# 1881
goto _LLE;}}}case 0: _LLE:
# 1884
 if(ands!=0){
a=(void*)ands->hd;
ands=ands->tl;
continue;}
# 1898 "pratt_prover.cyc"
Cyc_PrattProver_associate_ud_sd_in_graph(0,g);
Cyc_PrattProver_break_term_in_graph(0,g);
Cyc_PrattProver_subst_close_graph(0,g);
Cyc_PrattProver_johnson(0,g);
Cyc_PrattProver_add_type_assns(g);
Cyc_PrattProver_add_numelts_info(0,g);
Cyc_PrattProver_break_term_in_graph(0,g);
Cyc_PrattProver_associate_ud_sd_in_graph(0,g);{
# 1908
int _Tmp5=0;_npop_handler(0);return _Tmp5;}default:
 _throw(& Cyc_PrattProver_inconsistent);};}
# 1831 "pratt_prover.cyc"
1U;}
# 1830
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_PrattProver_Inconsistent_exn_struct*)_Tmp2)->tag==Cyc_PrattProver_Inconsistent){
# 1917 "pratt_prover.cyc"
if(ors==0)
# 1919
return 1;{
# 1922
struct _tuple18*_Tmp4=(struct _tuple18*)ors->hd;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4->f0;_Tmp5=_Tmp4->f1;{struct Cyc_PrattProver_Graph*g2=_Tmp6;struct Cyc_List_List*ands2=_Tmp5;
g=g2;
ands=ands2;
a=(void*)& Cyc_AssnDef_true_assn;
ors=ors->tl;
goto _LL11;}}}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL11:;}}}
# 1829 "pratt_prover.cyc"
1U;}}
# 1932 "pratt_prover.cyc"
static int Cyc_PrattProver_paths_exhausted=0;
# 2003 "pratt_prover.cyc"
int Cyc_PrattProver_constraint_prove(void*ctxt,void*a){
# 2005
void*nota=Cyc_AssnDef_not(a);
if(Cyc_PrattProver_inconsist(({void*_Tmp0=Cyc_AssnDef_widen(ctxt);Cyc_AssnDef_and(_Tmp0,nota);})))return 1;{
void*b=Cyc_AssnDef_and(ctxt,nota);
int res=Cyc_PrattProver_inconsist(b);
if(!res && Cyc_Flags_print_unprovable){
Cyc_fprintf(Cyc_stderr,_tag_fat("unable to prove inconsistency of \n",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_AssnDef_assn2dag(b);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});
if(Cyc_PrattProver_paths_exhausted)
Cyc_fprintf(Cyc_stderr,_tag_fat("(paths exhausted)",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));
# 2015
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 2017
return res;}}

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
 struct Cyc___cycFILE;
# 106 "stdio.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 133
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 142
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 151
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 165
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 167
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 218
extern int Cyc_getc(struct Cyc___cycFILE*);
# 241
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 283
extern int Cyc_sprintf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
# 312 "stdio.h"
extern int Cyc_ungetc(int,struct Cyc___cycFILE*);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 154 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 47
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*,int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*);
# 100
extern void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(void*));struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 210
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 27 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple3{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 168
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple3*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 314 "absyn.h"
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct _tuple5{enum Cyc_Absyn_AggrKind f0;struct _tuple3*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple5 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 334
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple3*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 547 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct _tuple11{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple3*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple3*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple3*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple3*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple3*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple3*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Typedefdecl{struct _tuple3*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple3*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 912 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple3*,struct _tuple3*);
int Cyc_Absyn_hash_qvar(struct _tuple3*);
# 933
void*Cyc_Absyn_compress(void*);
# 943
void*Cyc_Absyn_app_type(void*,struct _fat_ptr);
# 968
extern void*Cyc_Absyn_typedef_type(struct _tuple3*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 999
extern void*Cyc_Absyn_fat_bound_type;
# 1003
void*Cyc_Absyn_bounds_one (void);
# 1007
void*Cyc_Absyn_thinconst (void);
# 1225
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 142 "cycboot.h"
extern int Cyc_getc(struct Cyc___cycFILE*);
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 222 "cycboot.h"
extern int Cyc_ungetc(int,struct Cyc___cycFILE*);
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Binding_resolve_all_interproc(struct Cyc_List_List*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cycinf_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 72
struct _fat_ptr Cyc_Absynpp_fullqvar2string(struct _tuple3*);
# 38 "string.h"
extern unsigned Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 105 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 131
extern struct _fat_ptr Cyc_strtok(struct _fat_ptr,struct _fat_ptr);
# 25 "cyclone.h"
struct Cyc_List_List*Cyc_Cyclone_parse_file(struct _fat_ptr);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 280 "tcutil.h"
int Cyc_Tcutil_is_cvar_type(void*);
# 283
void Cyc_Tcutil_introduce_cvars(void*);
void Cyc_Tcutil_replace_cvars(void*,void*);
int Cyc_Tcutil_is_main(struct _tuple3*);
# 33 "bansheeif.h"
void Cyc_BansheeIf_persist_cvarmap(struct Cyc___cycFILE*);
void Cyc_BansheeIf_reset_local_cvar(void*);
void Cyc_BansheeIf_register_toplevel_cvar(void*);
void Cyc_BansheeIf_init_toplevel_cvars(struct Cyc_List_List*);
# 38
void Cyc_BansheeIf_resolve_or_default(void*);
# 40
int Cyc_BansheeIf_add_all_constraints(struct Cyc_List_List*);
# 26 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_constraint_file(struct Cyc___cycFILE*);
# 41 "flags.h"
extern int Cyc_Flags_resolve;
extern int Cyc_Flags_no_merge;
# 22 "interproc.cyc"
void Cyc_Tcutil_deconstruct_type(void*);char Cyc_Interproc_Rebinding[10U]="Rebinding";struct Cyc_Interproc_Rebinding_exn_struct{char*tag;};char Cyc_Interproc_Unresolved_sym[15U]="Unresolved_sym";struct Cyc_Interproc_Unresolved_sym_exn_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 31
static struct Cyc_Interproc_Rebinding_exn_struct Cyc_Interproc_Rebinding_val={Cyc_Interproc_Rebinding};
# 42 "interproc.cyc"
static struct Cyc___cycFILE*Cyc_Interproc_tagsFile=0;
# 44
static struct Cyc___cycFILE*Cyc_Interproc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){{struct Cyc___cycFILE*_Tmp2=({const char*_Tmp3=(const char*)_untag_fat_ptr_check_bound(filename,sizeof(char),1U);Cyc_fopen(_Tmp3,(const char*)_untag_fat_ptr_check_bound(mode,sizeof(char),1U));});_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();
# 47
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=msg_part;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=filename;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("\nError: couldn't open %s %s\n",sizeof(char),29U),_tag_fat(_Tmp5,sizeof(void*),2));});
Cyc_fflush(Cyc_stderr);
return 0;;}}}
# 53
static struct _fat_ptr Cyc_Interproc_namespace2string(struct Cyc_List_List*ns){
struct _fat_ptr a=Cyc_str_sepstr(ns,_tag_fat("::",sizeof(char),3U));
if((int)Cyc_strlen(a))
return Cyc_strconcat(a,_tag_fat("::",sizeof(char),3U));
# 58
return a;}
# 61
static void Cyc_Interproc_dump_decl_tags(struct Cyc_List_List*ns,struct _fat_ptr sourcefile,struct Cyc_List_List*tds){
for(1;(unsigned)tds;tds=tds->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)tds->hd)->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: _Tmp2=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp2;
# 65
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Interproc_namespace2string(ns);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=Cyc_Absynpp_fullqvar2string(fd->name);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=sourcefile;_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_fprintf(Cyc_Interproc_tagsFile,_tag_fat("^%s%s^%s^function^0\n",sizeof(char),21U),_tag_fat(_Tmp6,sizeof(void*),3));});
# 68
goto _LL0;}case 9: _Tmp2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*n=_Tmp2;struct Cyc_List_List*l=_Tmp1;
# 70
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=ns;Cyc_List_append(_Tmp4,({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=n,_Tmp5->tl=0;_Tmp5;}));});struct _fat_ptr _Tmp4=sourcefile;Cyc_Interproc_dump_decl_tags(_Tmp3,_Tmp4,l);});
goto _LL0;}case 10: _Tmp2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _tuple3*n=_Tmp2;struct Cyc_List_List*l=_Tmp1;
# 73
Cyc_Interproc_dump_decl_tags(ns,sourcefile,l);
goto _LL0;}default:
# 76
 goto _LL0;}_LL0:;}}
# 82
void Cyc_Interproc_dump_tags(struct _fat_ptr sourcefile,struct Cyc_List_List*tds){
if(!((unsigned)Cyc_Interproc_tagsFile)){
Cyc_Interproc_tagsFile=Cyc_Interproc_try_file_open(_tag_fat("CYCTAGS",sizeof(char),8U),_tag_fat("w",sizeof(char),2U),_tag_fat("CYCTAGS",sizeof(char),8U));
if(!((unsigned)Cyc_Interproc_tagsFile)){
Cyc_Warn_err(0U,_tag_fat("Could not open TAGS file",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));
return;}}
# 90
Cyc_Interproc_dump_decl_tags(0,sourcefile,tds);}
# 92
void Cyc_Interproc_close_tags_file (void){
if((unsigned)Cyc_Interproc_tagsFile)
Cyc_fclose(Cyc_Interproc_tagsFile);}
# 102
static struct Cyc_Hashtable_Table*Cyc_Interproc_tagmap=0;struct _tuple14{struct _fat_ptr f0;struct Cyc_List_List*f1;};
# 104
static void Cyc_Interproc_print_filedecls(struct Cyc_List_List*fd){
for(1;(unsigned)fd;fd=fd->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=((struct _tuple14*)fd->hd)->f0;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("Decls for file %s::\n",sizeof(char),21U),_tag_fat(_Tmp1,sizeof(void*),1));});}}
# 110
static void Cyc_Interproc_skip_whitespace(struct Cyc___cycFILE*f){
int c=Cyc_getc(f);
while((c==32 || c==9)|| c==10){
c=Cyc_getc(f);1U;}
# 115
Cyc_ungetc(c,f);}
# 117
static struct _fat_ptr Cyc_Interproc_grabline(struct Cyc___cycFILE*f){
static char buf[1000U];
int c=0;
int i=0;
struct _fat_ptr bufp=_tag_fat(buf,sizeof(char),1000U);
Cyc_Interproc_skip_whitespace(f);
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
c=Cyc_getc(f);
while(c!=10 && c!=-1){
*((char*)_check_fat_subscript(bufp,sizeof(char),i ++))=(char)c;
c=Cyc_getc(f);
# 126
1U;}
# 129
if(c==-1){struct _fat_ptr _Tmp2=_tag_fat(0,0,0);_npop_handler(0);return _Tmp2;}
Cyc_ungetc(c,f);
*((char*)_check_fat_subscript(bufp,sizeof(char),i))='\000';{
struct _fat_ptr _Tmp2=Cyc_strdup(bufp);_npop_handler(0);return _Tmp2;}
# 124
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();
# 134
Cyc_fprintf(Cyc_stderr,_tag_fat("grabline failed\n",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));goto _LL0;_LL0:;}}}
# 136
return _tag_fat(0,0,0);}
# 139
static int Cyc_Interproc_mstrptrcmp(struct _fat_ptr*m1,struct _fat_ptr*m2){
return Cyc_strcmp(*m1,*m2);}
# 142
static int Cyc_Interproc_hash_mstringptr(struct _fat_ptr*m1){
return Cyc_Hashtable_hash_string(*m1);}
# 146
static void Cyc_Interproc_print_string(struct _fat_ptr*s){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=*s;_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,({unsigned long _Tmp3=(unsigned long)Cyc_Interproc_hash_mstringptr(s);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_printf(_tag_fat("<%s>:%d",sizeof(char),8U),_tag_fat(_Tmp2,sizeof(void*),2));});}
# 150
static int Cyc_Interproc_init_tagmap_from_decls(struct _fat_ptr*source,struct Cyc_List_List*ds){
for(1;(unsigned)ds;ds=ds->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 154
({void(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*)=({void(*_Tmp3)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*))Cyc_Hashtable_insert;_Tmp3;});struct Cyc_Hashtable_Table*_Tmp3=Cyc_Interproc_tagmap;struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({
struct _fat_ptr _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_fullqvar2string(fd->name);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp8,sizeof(void*),1));});*_Tmp5=_Tmp6;});_Tmp5;});
# 154
_Tmp2(_Tmp3,_Tmp4,source);});
# 157
goto _LL0;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*l=_Tmp1;
_Tmp1=l;goto _LL6;}case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL6: {struct Cyc_List_List*l=_Tmp1;
# 160
Cyc_Interproc_init_tagmap_from_decls(source,l);
goto _LL0;}default:
# 163
 goto _LL0;}_LL0:;}
# 166
return 1;}
# 170
static int Cyc_Interproc_init_tagmap(struct Cyc_List_List*ftds){
if((unsigned)Cyc_Interproc_tagmap)
return 1;{
struct Cyc___cycFILE*tags=Cyc_Interproc_try_file_open(_tag_fat("CYCTAGS",sizeof(char),8U),_tag_fat("r",sizeof(char),2U),_tag_fat("CYCTAGS",sizeof(char),8U));
if(!((unsigned)tags)){
if(!((unsigned)ftds))
return 0;
Cyc_Interproc_tagmap=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;_Tmp0;})(11,Cyc_Interproc_mstrptrcmp,Cyc_Interproc_hash_mstringptr);{
struct _fat_ptr*_Tmp0=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=((struct _tuple14*)ftds->hd)->f0;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp4,sizeof(void*),1));});*_Tmp1=_Tmp2;});_Tmp1;});return Cyc_Interproc_init_tagmap_from_decls(_Tmp0,((struct _tuple14*)ftds->hd)->f1);}}
# 180
Cyc_Interproc_tagmap=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;_Tmp0;})(11,Cyc_Interproc_mstrptrcmp,Cyc_Interproc_hash_mstringptr);{
struct _fat_ptr line;
while((unsigned)(line=Cyc_Interproc_grabline(tags)).curr){
if((int)*((char*)_check_fat_subscript(line,sizeof(char),0))!=94)
continue;{
struct _fat_ptr*symname;symname=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_strtok(line,_tag_fat("^@",sizeof(char),3U));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp2,sizeof(void*),1));});*symname=_Tmp0;});{
struct _fat_ptr*filename;filename=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_strtok(_tag_fat(0,0,0),_tag_fat("^@",sizeof(char),3U));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp2,sizeof(void*),1));});*filename=_Tmp0;});{
struct _fat_ptr symtype=Cyc_strtok(_tag_fat(0,0,0),_tag_fat("^@",sizeof(char),3U));
if(!Cyc_strcmp(symtype,_tag_fat("function",sizeof(char),9U)))
# 191
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*))Cyc_Hashtable_insert;_Tmp0;})(Cyc_Interproc_tagmap,symname,filename);}}}
# 183
1U;}
# 194
Cyc_fclose(tags);
return 1;}}}
# 199
static struct _fat_ptr Cyc_Interproc_locate_qvar(struct _tuple3*qv,struct Cyc_List_List*ftds){
if(!((unsigned)Cyc_Interproc_tagmap)&& !Cyc_Interproc_init_tagmap(ftds))
_throw(& Cyc_Core_Not_found_val);{
# 204
struct _fat_ptr*qvname;qvname=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Absynpp_fullqvar2string(qv);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp2,sizeof(void*),1));});*qvname=_Tmp0;});
return*({struct _fat_ptr*(*_Tmp0)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct _fat_ptr*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;_Tmp0;})(Cyc_Interproc_tagmap,qvname);}}struct _tuple15{struct _fat_ptr f0;void*f1;};
# 211
static int Cyc_Interproc_cmp_filenames(struct _fat_ptr*fname,struct _tuple15*ftd){
return !Cyc_strcmp(*fname,ftd->f0);}struct _tuple16{int f0;struct Cyc_List_List*f1;};
# 218
static struct _tuple16 Cyc_Interproc_add_filedecls(struct _fat_ptr filename,struct Cyc_List_List*ftds){
if(({int(*_Tmp0)(int(*)(struct _fat_ptr*,struct _tuple14*),struct _fat_ptr*,struct Cyc_List_List*)=(int(*)(int(*)(struct _fat_ptr*,struct _tuple14*),struct _fat_ptr*,struct Cyc_List_List*))Cyc_List_exists_c;_Tmp0;})(({int(*_Tmp0)(struct _fat_ptr*,struct _tuple14*)=(int(*)(struct _fat_ptr*,struct _tuple14*))Cyc_Interproc_cmp_filenames;_Tmp0;}),& filename,ftds)){
struct _tuple16 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=0;return _Tmp0;}{
struct Cyc_List_List*decls=Cyc_Cyclone_parse_file(filename);
if(!((unsigned)decls)){
struct _tuple16 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=0;return _Tmp0;}{
# 225
struct Cyc_List_List*newl;newl=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple14*_Tmp0=({struct _tuple14*_Tmp1=_cycalloc(sizeof(struct _tuple14));_Tmp1->f0=filename,_Tmp1->f1=decls;_Tmp1;});newl->hd=_Tmp0;}),newl->tl=0;
Cyc_List_imp_append(ftds,newl);{
# 230
struct _tuple16 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=newl;return _Tmp0;}}}}struct _tuple17{int f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 241 "interproc.cyc"
static struct _tuple17 Cyc_Interproc_locate_externs(struct Cyc_List_List*es,struct Cyc_List_List*ftds){
struct Cyc_List_List*newdecls=0;
struct Cyc_List_List*located=0;
for(1;es!=0;es=es->tl){
struct _fat_ptr filename=_tag_fat(0,0,0);
{void*_Tmp0=((struct Cyc_Absyn_Exp*)es->hd)->r;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
# 248
{void*_Tmp2;switch(*((int*)b)){case 0: _Tmp2=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)b)->f1;{struct _tuple3*q=_Tmp2;
# 250
if(Cyc_Flags_no_merge)
_throw((void*)({struct Cyc_Interproc_Unresolved_sym_exn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Interproc_Unresolved_sym_exn_struct));_Tmp3->tag=Cyc_Interproc_Unresolved_sym,_Tmp3->f1=(struct Cyc_Absyn_Exp*)es->hd;_Tmp3;}));
{struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){
filename=Cyc_Interproc_locate_qvar(q,ftds);
located=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=(struct Cyc_Absyn_Exp*)es->hd,_Tmp5->tl=located;_Tmp5;});
# 253
;_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp5)->tag==Cyc_Core_Not_found)
# 256
_throw((void*)({struct Cyc_Interproc_Unresolved_sym_exn_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Interproc_Unresolved_sym_exn_struct));_Tmp7->tag=Cyc_Interproc_Unresolved_sym,_Tmp7->f1=(struct Cyc_Absyn_Exp*)es->hd;_Tmp7;}));else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}};}}}
# 258
goto _LL5;}case 1: _Tmp2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 260
{struct _handler_cons _Tmp3;_push_handler(& _Tmp3);{int _Tmp4=0;if(setjmp(_Tmp3.handler))_Tmp4=1;if(!_Tmp4){
filename=Cyc_Interproc_locate_qvar(vd->name,ftds);
if(Cyc_Flags_no_merge){
if(!Cyc_strcmp(filename,((struct _tuple14*)ftds->hd)->f0)){
# 265
located=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=(struct Cyc_Absyn_Exp*)es->hd,_Tmp5->tl=located;_Tmp5;});
_npop_handler(0);continue;}else{
# 272
Cyc_Tcutil_introduce_cvars(vd->type);
filename=_tag_fat(0,0,0);
_npop_handler(0);continue;}}else{
# 278
located=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=(struct Cyc_Absyn_Exp*)es->hd,_Tmp5->tl=located;_Tmp5;});}
# 261
;_pop_handler();}else{void*_Tmp5=(void*)Cyc_Core_get_exn_thrown();void*_Tmp6;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp5)->tag==Cyc_Core_Not_found)
# 284
continue;else{_Tmp6=_Tmp5;{void*exn=_Tmp6;_rethrow(exn);}};}}}
# 286
goto _LL5;}default:
# 288
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("locate_externs: Unexpected expression",sizeof(char),38U);_Tmp4;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp5;_Tmp5.tag=4,_Tmp5.f1=(struct Cyc_Absyn_Exp*)es->hd;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL5;}_LL5:;}
# 291
goto _LL0;}}else{
# 293
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("locate_externs: Unexpected expression",sizeof(char),38U);_Tmp3;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp4;_Tmp4.tag=4,_Tmp4.f1=(struct Cyc_Absyn_Exp*)es->hd;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),2));});
goto _LL0;}_LL0:;}
# 297
(unsigned)filename.curr && !Cyc_Flags_no_merge?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("filename && !Flags::no_merge",sizeof(char),29U),_tag_fat("interproc.cyc",sizeof(char),14U),297U);{
struct _tuple16 _Tmp0=Cyc_Interproc_add_filedecls(filename,ftds);void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{int b=_Tmp2;struct Cyc_List_List*newl=_Tmp1;
if(!b){
struct _tuple17 _Tmp3;_Tmp3.f0=0,_Tmp3.f1=0,_Tmp3.f2=0;return _Tmp3;}
if(!((unsigned)newdecls))
newdecls=newl;}}}{
# 304
struct _tuple17 _Tmp0;_Tmp0.f0=1,_Tmp0.f1=newdecls,_Tmp0.f2=located;return _Tmp0;}}
# 315 "interproc.cyc"
static void Cyc_Interproc_build_fndecl_tab(struct Cyc_Hashtable_Table*fdt,struct Cyc_List_List*decls){
struct Cyc_List_List*ds=decls;for(0;(unsigned)ds;ds=ds->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 319
({void(*_Tmp2)(struct Cyc_Hashtable_Table*,struct _tuple3*,struct Cyc_Absyn_Fndecl*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple3*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_insert;_Tmp2;})(fdt,fd->name,fd);
goto _LL0;}case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*l=_Tmp1;
_Tmp1=l;goto _LL6;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL6: {struct Cyc_List_List*l=_Tmp1;
# 323
Cyc_Interproc_build_fndecl_tab(fdt,l);goto _LL0;}default:
# 325
 goto _LL0;}_LL0:;}}
# 330
static void Cyc_Interproc_remove_cvars(void*t){
void*_Tmp0;void*_Tmp1;switch(*((int*)t)){case 4: _Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp0=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;{void*ta=_Tmp1;void**b=(void**)_Tmp0;
# 333
if(Cyc_Tcutil_is_cvar_type(*b))
({void*_Tmp2=Cyc_Absyn_bounds_one();*b=_Tmp2;});
# 336
Cyc_Interproc_remove_cvars(ta);
return;}case 6: _Tmp1=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*i=_Tmp1;
# 339
Cyc_Interproc_remove_cvars(i->ret_type);
{struct Cyc_List_List*args=i->args;for(0;(unsigned)args;args=args->tl){
Cyc_Interproc_remove_cvars(((struct _tuple11*)args->hd)->f2);}}
# 343
return;}case 0: _Tmp1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2;{struct Cyc_List_List*ts=_Tmp1;
_Tmp1=ts;goto _LL8;}case 8: _Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f2;_LL8: {struct Cyc_List_List*ts=_Tmp1;
# 346
for(1;(unsigned)ts;ts=ts->tl){Cyc_Interproc_remove_cvars((void*)ts->hd);}
return;}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f2==1){_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)t)->f3;{struct Cyc_List_List*fields=_Tmp1;
# 349
for(1;(unsigned)fields;fields=fields->tl){
Cyc_Interproc_remove_cvars(((struct Cyc_Absyn_Aggrfield*)fields->hd)->type);}
# 352
return;}}else{goto _LLB;}default: _LLB:
# 354
 return;};}struct _tuple18{void*f0;void*f1;};
# 359
static void Cyc_Interproc_ensure_cvar_identity(unsigned loc,void*dest,void*src){
struct _tuple18 _Tmp0=({struct _tuple18 _Tmp1;_Tmp1.f0=dest,_Tmp1.f1=src;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_Tmp3=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.elt_type;_Tmp1=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1.ptr_atts.bounds;{void*tD=_Tmp4;void**bD=_Tmp3;void*tS=_Tmp2;void**bS=_Tmp1;
# 362
if(Cyc_Tcutil_is_cvar_type(*bS))
*bD=*bS;
Cyc_Interproc_ensure_cvar_identity(loc,tD,tS);
return;}}else{goto _LLB;}case 6: if(*((int*)_Tmp0.f1)==6){_Tmp4=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp3=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_FnInfo*iD=_Tmp4;struct Cyc_Absyn_FnInfo*iS=_Tmp3;
# 367
Cyc_Interproc_ensure_cvar_identity(loc,iD->ret_type,iS->ret_type);
if(({int _Tmp5=Cyc_List_length(iD->args);_Tmp5!=Cyc_List_length(iS->args);}))
_throw(& Cyc_Interproc_Rebinding_val);
{struct Cyc_List_List*argD=iD->args;struct Cyc_List_List*argS=iS->args;for(0;
(unsigned)argD &&(unsigned)argS;(argD=argD->tl,argS=argS->tl)){
Cyc_Interproc_ensure_cvar_identity(loc,((struct _tuple11*)argD->hd)->f2,((struct _tuple11*)argS->hd)->f2);}}
# 374
return;}}else{goto _LLB;}case 8: if(*((int*)_Tmp0.f1)==8){_Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f0)->f2;_Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0.f1)->f2;{struct _tuple3*nameD=_Tmp4;struct Cyc_List_List*tD=_Tmp3;struct _tuple3*nameS=_Tmp2;struct Cyc_List_List*tS=_Tmp1;
# 376
if(({int _Tmp5=Cyc_List_length(tD);_Tmp5!=Cyc_List_length(tS);})){
# 381
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("Typedef names could not be resolved yet; removing any constraint variables from ",sizeof(char),81U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)src;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_Warn_warn2(loc,_tag_fat(_Tmp7,sizeof(void*),2));});
for(1;(unsigned)tS;tS=tS->tl){
Cyc_Interproc_remove_cvars((void*)tS->hd);}
return;}
# 386
_Tmp4=tD;_Tmp3=tS;goto _LL8;}}else{goto _LLB;}case 0: if(*((int*)_Tmp0.f1)==0){_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f2;_LL8: {struct Cyc_List_List*tD=_Tmp4;struct Cyc_List_List*tS=_Tmp3;
# 388
if(({int _Tmp5=Cyc_List_length(tD);_Tmp5!=Cyc_List_length(tS);}))
_throw(& Cyc_Interproc_Rebinding_val);
for(1;(unsigned)tD &&(unsigned)tS;(tD=tD->tl,tS=tS->tl)){
Cyc_Interproc_ensure_cvar_identity(loc,(void*)tD->hd,(void*)tS->hd);}
return;}}else{goto _LLB;}case 7: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f0)->f2==1){if(*((int*)_Tmp0.f1)==7){if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f2==1){_Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f0)->f3;_Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0.f1)->f3;{struct Cyc_List_List*fieldD=_Tmp4;struct Cyc_List_List*fieldS=_Tmp3;
# 394
if(({int _Tmp5=Cyc_List_length(fieldD);_Tmp5!=Cyc_List_length(fieldS);}))
_throw(& Cyc_Interproc_Rebinding_val);
for(1;(unsigned)fieldD &&(unsigned)fieldS;(fieldD=fieldD->tl,fieldS=fieldS->tl)){
Cyc_Interproc_ensure_cvar_identity(loc,((struct Cyc_Absyn_Aggrfield*)fieldD->hd)->type,((struct Cyc_Absyn_Aggrfield*)fieldS->hd)->type);}
return;}}else{goto _LLB;}}else{goto _LLB;}}else{goto _LLB;}default: _LLB:
# 400
 return;};}
# 406
static void Cyc_Interproc_imp_update_prototypes(struct Cyc_List_List*decls,struct Cyc_Hashtable_Table*fndecl_tab){
# 408
for(1;(unsigned)decls;decls=decls->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)decls->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 411
struct Cyc_Absyn_Fndecl*fd;
{struct _handler_cons _Tmp2;_push_handler(& _Tmp2);{int _Tmp3=0;if(setjmp(_Tmp2.handler))_Tmp3=1;if(!_Tmp3){
fd=({struct Cyc_Absyn_Fndecl*(*_Tmp4)(struct Cyc_Hashtable_Table*,struct _tuple3*)=(struct Cyc_Absyn_Fndecl*(*)(struct Cyc_Hashtable_Table*,struct _tuple3*))Cyc_Hashtable_lookup;_Tmp4;})(fndecl_tab,vd->name);;_pop_handler();}else{void*_Tmp4=(void*)Cyc_Core_get_exn_thrown();void*_Tmp5;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp4)->tag==Cyc_Core_Not_found)
# 416
continue;else{_Tmp5=_Tmp4;{void*exn=_Tmp5;_rethrow(exn);}};}}}
# 418
{struct _handler_cons _Tmp2;_push_handler(& _Tmp2);{int _Tmp3=0;if(setjmp(_Tmp2.handler))_Tmp3=1;if(!_Tmp3){
({unsigned _Tmp4=((struct Cyc_Absyn_Decl*)decls->hd)->loc;void*_Tmp5=vd->type;Cyc_Interproc_ensure_cvar_identity(_Tmp4,_Tmp5,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp6->tag=6,_Tmp6->f1=fd->i;_Tmp6;}));});;_pop_handler();}else{void*_Tmp4=(void*)Cyc_Core_get_exn_thrown();void*_Tmp5;if(((struct Cyc_Interproc_Rebinding_exn_struct*)_Tmp4)->tag==Cyc_Interproc_Rebinding){
# 422
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("Incompatible types in declarations: ",sizeof(char),37U);_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)vd->type;_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,({typeof((void*)((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_TmpB->tag=6,_TmpB->f1=fd->i;_TmpB;})))_TmpA=(void*)((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_TmpB->tag=6,_TmpB->f1=fd->i;_TmpB;}));_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_Warn_err2(((struct Cyc_Absyn_Decl*)decls->hd)->loc,_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LLE;}else{_Tmp5=_Tmp4;{void*exn=_Tmp5;_rethrow(exn);}}_LLE:;}}}
# 425
goto _LL0;}case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*l=_Tmp1;
_Tmp1=l;goto _LL6;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL6: {struct Cyc_List_List*l=_Tmp1;
# 428
Cyc_Interproc_imp_update_prototypes(l,fndecl_tab);
goto _LL0;}default:
# 431
 goto _LL0;}_LL0:;}}
# 435
static struct Cyc_List_List*Cyc_Interproc_update_prototype_cvars(struct Cyc_List_List*exts,struct Cyc_List_List*decls){
struct _RegionHandle _Tmp0=_new_region(0U,"r");struct _RegionHandle*r=& _Tmp0;_push_region(r);
{struct Cyc_Hashtable_Table*fndecl_tab=({struct Cyc_Hashtable_Table*(*_Tmp1)(struct _RegionHandle*,int,int(*)(struct _tuple3*,struct _tuple3*),int(*)(struct _tuple3*))=(struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*,int,int(*)(struct _tuple3*,struct _tuple3*),int(*)(struct _tuple3*)))Cyc_Hashtable_rcreate;_Tmp1;})(r,101,Cyc_Absyn_qvar_cmp,Cyc_Absyn_hash_qvar);
Cyc_Interproc_build_fndecl_tab(fndecl_tab,decls);
# 440
Cyc_Interproc_imp_update_prototypes(decls,fndecl_tab);{
struct Cyc_List_List*_Tmp1=decls;_npop_handler(0);return _Tmp1;}}
# 437
;_pop_region();}
# 455 "interproc.cyc"
static char Cyc_Interproc__prefix[1024U];
static int Cyc_Interproc_myhash_stringptr(struct _fat_ptr*a){
return Cyc_Hashtable_hash_string(*a);}
# 459
static int Cyc_Interproc_mystrptrcmp(struct _fat_ptr*a,struct _fat_ptr*b){
return Cyc_strcmp(*a,*b);}
# 462
static void Cyc_Interproc_print_strptr(struct _fat_ptr*a){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=*a;_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,({unsigned long _Tmp3=(unsigned long)Cyc_Hashtable_hash_stringptr(a);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_printf(_tag_fat("%s(%d)",sizeof(char),7U),_tag_fat(_Tmp2,sizeof(void*),2));});}
# 465
static void Cyc_Interproc_print_type(void*a){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Absynpp_typ2string(a);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 468
static void Cyc_Interproc_dump_table(struct Cyc_Hashtable_Table*cvmap){
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void(*)(struct _fat_ptr*),void(*)(void*))=(void(*)(struct Cyc_Hashtable_Table*,void(*)(struct _fat_ptr*),void(*)(void*)))Cyc_Hashtable_print_table_map;_Tmp0;})(cvmap,Cyc_Interproc_print_strptr,Cyc_Interproc_print_type);}struct _tuple19{struct Cyc_Absyn_Tqual f0;void*f1;};
# 474
static struct Cyc_List_List*Cyc_Interproc_cvars_in_type(void*t,int index,int is_local,int pfxpos){
# 476
static struct Cyc_Hashtable_Table*cvmap=0;
if(!((unsigned)cvmap))
cvmap=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;_Tmp0;})(101,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);{
# 480
struct Cyc_List_List*ret=0;
{void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 4: _Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;_Tmp3=(void**)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.ptr_atts.bounds;{void*elt=_Tmp4;void**bd=_Tmp3;
# 483
ret=Cyc_Interproc_cvars_in_type(elt,index + 1,is_local,pfxpos);
{void*_Tmp5=*bd;void*_Tmp6;void*_Tmp7;if(*((int*)_Tmp5)==3){_Tmp7=(int*)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp5)->f3;_Tmp6=(const char**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp5)->f5;{int*id=(int*)_Tmp7;const char**name=(const char**)_Tmp6;
# 486
pfxpos < 1000?0:({int(*_Tmp8)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp8;})(_tag_fat("pfxpos < 1000",sizeof(char),14U),_tag_fat("interproc.cyc",sizeof(char),14U),486U);
({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)index;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({struct _fat_ptr _TmpA=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);struct _fat_ptr _TmpB=_tag_fat("_%d",sizeof(char),4U);Cyc_sprintf(_TmpA,_TmpB,_tag_fat(_Tmp9,sizeof(void*),1));});});{
struct _fat_ptr nn=({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp9,sizeof(void*),1));});
# 490
void**val=({void**(*_Tmp8)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(void**(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_lookup_other_opt;_Tmp8;})(cvmap,& nn,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);
if((unsigned)val)
*bd=*val;else{
# 495
({const char*_Tmp8=(const char*)_untag_fat_ptr_check_bound(nn,sizeof(char),1U);*name=_Tmp8;});
ret=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=*bd,_Tmp8->tl=ret;_Tmp8;});
if(!is_local)
*id=-*id;
({void(*_Tmp8)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=({void(*_Tmp9)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*))Cyc_Hashtable_insert;_Tmp9;});struct Cyc_Hashtable_Table*_Tmp9=cvmap;struct _fat_ptr*_TmpA=({struct _fat_ptr*_TmpB=_cycalloc(sizeof(struct _fat_ptr));*_TmpB=nn;_TmpB;});_Tmp8(_Tmp9,_TmpA,*bd);});}
# 501
({struct _fat_ptr _Tmp8=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);char _Tmp9=*((char*)_check_fat_subscript(_Tmp8,sizeof(char),0U));char _TmpA='\000';if(_get_fat_size(_Tmp8,sizeof(char))==1U &&(_Tmp9==0 && _TmpA!=0))_throw_arraybounds();*((char*)_Tmp8.curr)=_TmpA;});
goto _LL13;}}}else{
# 504
goto _LL13;}_LL13:;}
# 506
goto _LL0;}case 5: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*elt=_Tmp4;
# 508
ret=Cyc_Interproc_cvars_in_type(elt,index + 1,is_local,pfxpos);
goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{struct Cyc_List_List*af=_Tmp4;
# 511
int findex=0;
for(1;(unsigned)af;af=af->tl){
struct Cyc_Absyn_Aggrfield*_Tmp5=(struct Cyc_Absyn_Aggrfield*)af->hd;void*_Tmp6;_Tmp6=_Tmp5->type;{void*t=_Tmp6;
# 515
pfxpos < 980?0:({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp7;})(_tag_fat("pfxpos < 980",sizeof(char),13U),_tag_fat("interproc.cyc",sizeof(char),14U),515U);
({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)index;_Tmp8;});struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)findex ++;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;({struct _fat_ptr _TmpA=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);struct _fat_ptr _TmpB=_tag_fat("_%d_AA%d",sizeof(char),9U);Cyc_sprintf(_TmpA,_TmpB,_tag_fat(_Tmp9,sizeof(void*),2));});});
ret=({struct Cyc_List_List*_Tmp7=({void*_Tmp8=t;int _Tmp9=is_local;Cyc_Interproc_cvars_in_type(_Tmp8,0,_Tmp9,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});Cyc_List_append(_Tmp7,ret);});
goto _LL18;}_LL18:;}
# 521
({struct _fat_ptr _Tmp5=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);char _Tmp6=*((char*)_check_fat_subscript(_Tmp5,sizeof(char),0U));char _Tmp7='\000';if(_get_fat_size(_Tmp5,sizeof(char))==1U &&(_Tmp6==0 && _Tmp7!=0))_throw_arraybounds();*((char*)_Tmp5.curr)=_Tmp7;});
goto _LL0;}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 22: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatype.tag==2){_Tmp4=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownDatatype.val;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Datatypedecl*dtd=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 524
if((unsigned)((struct Cyc_List_List*)dtd->fields->v)){
struct Cyc_List_List*fs=(struct Cyc_List_List*)dtd->fields->v;
int findex=0;
struct _fat_ptr dtn=Cyc_Absynpp_fullqvar2string(dtd->name);
pfxpos < 900?0:({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp5;})(_tag_fat("pfxpos < 900",sizeof(char),13U),_tag_fat("interproc.cyc",sizeof(char),14U),528U);
for(1;(unsigned)fs;fs=fs->tl){
struct Cyc_List_List*ft=((struct Cyc_Absyn_Datatypefield*)fs->hd)->typs;
int finner=0;
for(1;(unsigned)ft;ft=ft->tl){
struct _tuple19*_Tmp5=(struct _tuple19*)ft->hd;void*_Tmp6;_Tmp6=_Tmp5->f1;{void*t=_Tmp6;
({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)index;_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=dtn;_Tmp9;});struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned long)findex;_TmpA;});struct Cyc_Int_pa_PrintArg_struct _TmpA=({struct Cyc_Int_pa_PrintArg_struct _TmpB;_TmpB.tag=1,_TmpB.f1=(unsigned long)finner ++;_TmpB;});void*_TmpB[4];_TmpB[0]=& _Tmp7,_TmpB[1]=& _Tmp8,_TmpB[2]=& _Tmp9,_TmpB[3]=& _TmpA;({struct _fat_ptr _TmpC=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);struct _fat_ptr _TmpD=_tag_fat("_%d_DD%s_DF%d_%d",sizeof(char),17U);Cyc_sprintf(_TmpC,_TmpD,_tag_fat(_TmpB,sizeof(void*),4));});});
# 536
ret=({struct Cyc_List_List*_Tmp7=({void*_Tmp8=t;int _Tmp9=is_local;Cyc_Interproc_cvars_in_type(_Tmp8,0,_Tmp9,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});Cyc_List_append(_Tmp7,ret);});}}
# 538
++ findex;}
# 540
({struct _fat_ptr _Tmp5=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);char _Tmp6=*((char*)_check_fat_subscript(_Tmp5,sizeof(char),0U));char _Tmp7='\000';if(_get_fat_size(_Tmp5,sizeof(char))==1U &&(_Tmp6==0 && _Tmp7!=0))_throw_arraybounds();*((char*)_Tmp5.curr)=_Tmp7;});}
# 542
goto _LL0;}}else{goto _LL11;}case 24: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.tag==2){_Tmp4=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1.KnownAggr.val;_Tmp3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 544
struct _fat_ptr adn=Cyc_Absynpp_fullqvar2string(ad->name);
int findex=0;
if((unsigned)ad->impl){
pfxpos < 900?0:({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp5;})(_tag_fat("pfxpos < 900",sizeof(char),13U),_tag_fat("interproc.cyc",sizeof(char),14U),547U);
{struct Cyc_List_List*fs=ad->impl->fields;for(0;(unsigned)fs;fs=fs->tl){
({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)index;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=adn;_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)findex ++;_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;({struct _fat_ptr _Tmp9=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);struct _fat_ptr _TmpA=_tag_fat("_%d_AD%s_AF%d",sizeof(char),14U);Cyc_sprintf(_Tmp9,_TmpA,_tag_fat(_Tmp8,sizeof(void*),3));});});
# 551
ret=({struct Cyc_List_List*_Tmp5=({void*_Tmp6=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;int _Tmp7=is_local;Cyc_Interproc_cvars_in_type(_Tmp6,0,_Tmp7,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});Cyc_List_append(_Tmp5,ret);});}}
# 553
({struct _fat_ptr _Tmp5=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);char _Tmp6=*((char*)_check_fat_subscript(_Tmp5,sizeof(char),0U));char _Tmp7='\000';if(_get_fat_size(_Tmp5,sizeof(char))==1U &&(_Tmp6==0 && _Tmp7!=0))_throw_arraybounds();*((char*)_Tmp5.curr)=_Tmp7;});}
# 555
goto _LL0;}}else{goto _LL11;}default: goto _LL11;}case 6: _Tmp4=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_FnInfo*fi=_Tmp4;
# 558
pfxpos < 900?0:({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp5;})(_tag_fat("pfxpos < 900",sizeof(char),13U),_tag_fat("interproc.cyc",sizeof(char),14U),558U);
({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)index;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({struct _fat_ptr _Tmp7=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);struct _fat_ptr _Tmp8=_tag_fat("_%d_FR",sizeof(char),7U);Cyc_sprintf(_Tmp7,_Tmp8,_tag_fat(_Tmp6,sizeof(void*),1));});});
ret=({struct Cyc_List_List*_Tmp5=({void*_Tmp6=fi->ret_type;int _Tmp7=is_local;Cyc_Interproc_cvars_in_type(_Tmp6,0,_Tmp7,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});Cyc_List_append(_Tmp5,ret);});{
int argix=0;
{struct Cyc_List_List*args=fi->args;for(0;(unsigned)args;args=args->tl){
struct _tuple11*_Tmp5=(struct _tuple11*)args->hd;void*_Tmp6;_Tmp6=_Tmp5->f2;{void*t=_Tmp6;
({struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)index;_Tmp8;});struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)argix ++;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;({struct _fat_ptr _TmpA=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);struct _fat_ptr _TmpB=_tag_fat("_%d_ARG%d",sizeof(char),10U);Cyc_sprintf(_TmpA,_TmpB,_tag_fat(_Tmp9,sizeof(void*),2));});});
# 566
ret=({struct Cyc_List_List*_Tmp7=({void*_Tmp8=t;int _Tmp9=is_local;Cyc_Interproc_cvars_in_type(_Tmp8,0,_Tmp9,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});Cyc_List_append(_Tmp7,ret);});}}}
# 568
({struct _fat_ptr _Tmp5=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);char _Tmp6=*((char*)_check_fat_subscript(_Tmp5,sizeof(char),0U));char _Tmp7='\000';if(_get_fat_size(_Tmp5,sizeof(char))==1U &&(_Tmp6==0 && _Tmp7!=0))_throw_arraybounds();*((char*)_Tmp5.curr)=_Tmp7;});
goto _LL0;}}case 8: _Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f3;_Tmp1=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f4;{struct _tuple3*tdn=_Tmp4;struct Cyc_List_List*ts=_Tmp3;struct Cyc_Absyn_Typedefdecl*tdef=_Tmp2;void*topt=_Tmp1;
# 571
if((unsigned)tdef &&(unsigned)tdef->defn){
pfxpos < 900?0:({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp5;})(_tag_fat("pfxpos < 900",sizeof(char),13U),_tag_fat("interproc.cyc",sizeof(char),14U),572U);
# 574
({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)index;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_fullqvar2string(tdn);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;({struct _fat_ptr _Tmp8=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);struct _fat_ptr _Tmp9=_tag_fat("_%d_TD%s",sizeof(char),9U);Cyc_sprintf(_Tmp8,_Tmp9,_tag_fat(_Tmp7,sizeof(void*),2));});});
ret=({struct Cyc_List_List*_Tmp5=({void*_Tmp6=tdef->defn;int _Tmp7=is_local;Cyc_Interproc_cvars_in_type(_Tmp6,0,_Tmp7,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});Cyc_List_append(_Tmp5,ret);});
# 577
({struct _fat_ptr _Tmp5=_fat_ptr_plus(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U),sizeof(char),pfxpos);char _Tmp6=*((char*)_check_fat_subscript(_Tmp5,sizeof(char),0U));char _Tmp7='\000';if(_get_fat_size(_Tmp5,sizeof(char))==1U &&(_Tmp6==0 && _Tmp7!=0))_throw_arraybounds();*((char*)_Tmp5.curr)=_Tmp7;});}
# 579
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_Absyn_TypeDecl*td=_Tmp4;void**to=_Tmp3;
# 581
{void*_Tmp5=td->r;void*_Tmp6;switch(*((int*)_Tmp5)){case 0: _Tmp6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp6;
# 583
ret=({struct Cyc_List_List*_Tmp7=({void*_Tmp8=({void*_Tmp9=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_TmpA->tag=24,({union Cyc_Absyn_AggrInfo _TmpB=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_TmpC=ad;_TmpC;}));_TmpA->f1=_TmpB;});_TmpA;});Cyc_Absyn_app_type(_Tmp9,_tag_fat(0U,sizeof(void*),0));});int _Tmp9=is_local;Cyc_Interproc_cvars_in_type(_Tmp8,0,_Tmp9,pfxpos);});Cyc_List_append(_Tmp7,ret);});
# 585
goto _LL21;}case 2: _Tmp6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_Tmp5)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp6;
# 587
ret=({struct Cyc_List_List*_Tmp7=({void*_Tmp8=({void*_Tmp9=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct));_TmpA->tag=22,({union Cyc_Absyn_DatatypeInfo _TmpB=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));*_TmpC=tud;_TmpC;}));_TmpA->f1=_TmpB;});_TmpA;});Cyc_Absyn_app_type(_Tmp9,_tag_fat(0U,sizeof(void*),0));});int _Tmp9=is_local;Cyc_Interproc_cvars_in_type(_Tmp8,0,_Tmp9,pfxpos);});Cyc_List_append(_Tmp7,ret);});
# 589
goto _LL21;}default:
# 591
 goto _LL21;}_LL21:;}
# 593
goto _LL0;}default: _LL11:
# 595
 goto _LL0;}_LL0:;}
# 598
return ret;}}
# 602
static int Cyc_Interproc_exp_noop(void*a,struct Cyc_Absyn_Exp*b){
{void*_Tmp0=b->r;void*_Tmp1;if(*((int*)_Tmp0)==14){_Tmp1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t=_Tmp1;
# 605
({void*_Tmp2=t;Cyc_Tcutil_replace_cvars(_Tmp2,Cyc_Absyn_bounds_one());});
goto _LL0;}}else{
# 608
goto _LL0;}_LL0:;}
# 610
return 1;}struct _tuple20{struct _fat_ptr f0;int f1;void(*f2)(struct Cyc_List_List*,void*);void*f3;};
# 615
static int Cyc_Interproc_cvars_in_local_decl(struct _tuple20*env,struct Cyc_Absyn_Stmt*s){
void*_Tmp0;void*_Tmp1;void*_Tmp2;struct _fat_ptr _Tmp3;_Tmp3=env->f0;_Tmp2=(int*)& env->f1;_Tmp1=env->f2;_Tmp0=(void*)env->f3;{struct _fat_ptr pfx=_Tmp3;int*count=(int*)_Tmp2;void(*fn)(struct Cyc_List_List*,void*)=_Tmp1;void*inner_env=_Tmp0;
{void*_Tmp4=s->r;void*_Tmp5;void*_Tmp6;if(*((int*)_Tmp4)==12){_Tmp6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp4)->f1;_Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp4)->f2;{struct Cyc_Absyn_Decl*d=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
# 619
{void*_Tmp7=d->r;void*_Tmp8;switch(*((int*)_Tmp7)){case 0: _Tmp8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp7)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp8;
# 621
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=pfx;_TmpA;});struct Cyc_Int_pa_PrintArg_struct _TmpA=({struct Cyc_Int_pa_PrintArg_struct _TmpB;_TmpB.tag=1,_TmpB.f1=(unsigned long)(*count)++;_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_sprintf(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U - 1U),_tag_fat("%s_%d",sizeof(char),6U),_tag_fat(_TmpB,sizeof(void*),2));});{
struct Cyc_List_List*cvars=({void*_Tmp9=vd->type;Cyc_Interproc_cvars_in_type(_Tmp9,0,1,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});
# 626
fn(cvars,inner_env);
goto _LL8;}}case 2: _Tmp8=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp7)->f1;{struct Cyc_Absyn_Pat*p=_Tmp8;
# 629
{void*_Tmp9=p->r;void*_TmpA;if(*((int*)_Tmp9)==2){_TmpA=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp9)->f2;{struct Cyc_Absyn_Vardecl*vd=_TmpA;
# 631
({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=pfx;_TmpC;});struct Cyc_Int_pa_PrintArg_struct _TmpC=({struct Cyc_Int_pa_PrintArg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=(unsigned long)(*count)++;_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;Cyc_sprintf(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U - 1U),_tag_fat("%s_%d",sizeof(char),6U),_tag_fat(_TmpD,sizeof(void*),2));});{
struct Cyc_List_List*cvars=({void*_TmpB=vd->type;Cyc_Interproc_cvars_in_type(_TmpB,0,1,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});
# 636
fn(cvars,inner_env);
goto _LLF;}}}else{
# 639
goto _LLF;}_LLF:;}
# 641
goto _LL8;}default:
# 643
 goto _LL8;}_LL8:;}
# 645
goto _LL3;}}else{
# 647
goto _LL3;}_LL3:;}
# 649
return 1;}}
# 655
static void Cyc_Interproc_remove_local_constraints(struct Cyc_List_List*cvars,int ignore){
for(1;(unsigned)cvars;cvars=cvars->tl){
Cyc_BansheeIf_reset_local_cvar((void*)cvars->hd);}}struct _tuple21{struct _fat_ptr f0;int f1;void(*f2)(struct Cyc_List_List*,int);int f3;};
# 667
static struct Cyc_List_List*Cyc_Interproc_assign_cvar_names(struct Cyc_List_List*ds){
struct Cyc_List_List*ret=0;
for(1;(unsigned)ds;ds=ds->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 9: _Tmp1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*l=_Tmp1;
_Tmp1=l;goto _LL4;}case 10: _Tmp1=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL4: {struct Cyc_List_List*l=_Tmp1;
ret=({struct Cyc_List_List*_Tmp2=ret;Cyc_List_append(_Tmp2,Cyc_Interproc_assign_cvar_names(l));});goto _LL0;}case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 674
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=Cyc_Absynpp_fullqvar2string(vd->name);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_sprintf(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U - 1U),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp3,sizeof(void*),1));});
ret=({struct Cyc_List_List*_Tmp2=({void*_Tmp3=vd->type;Cyc_Interproc_cvars_in_type(_Tmp3,0,0,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});Cyc_List_append(_Tmp2,ret);});
# 677
goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 679
struct _fat_ptr fdname=Cyc_Absynpp_fullqvar2string(fd->name);
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=fdname;_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_sprintf(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U - 1U),_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp3,sizeof(void*),1));});
if(Cyc_Tcutil_is_main(fd->name)){
struct Cyc_List_List*args=fd->i.args;
if((unsigned)args &&(unsigned)args->tl)
Cyc_Tcutil_replace_cvars(((struct _tuple11*)args->tl->hd)->f2,Cyc_Absyn_fat_bound_type);}else{
# 688
ret=({struct Cyc_List_List*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp4->tag=6,_Tmp4->f1=fd->i;_Tmp4;});Cyc_Interproc_cvars_in_type(_Tmp3,0,0,(int)Cyc_strlen(_tag_fat(Cyc_Interproc__prefix,sizeof(char),1024U)));});Cyc_List_append(_Tmp2,ret);});}{
# 692
struct _tuple21*env;env=_cycalloc(sizeof(struct _tuple21)),env->f0=fdname,env->f1=0,env->f2=Cyc_Interproc_remove_local_constraints,env->f3=0;
({void(*_Tmp2)(int(*)(struct _tuple21*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple21*,struct Cyc_Absyn_Stmt*),struct _tuple21*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct _tuple21*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple21*,struct Cyc_Absyn_Stmt*),struct _tuple21*,struct Cyc_Absyn_Stmt*))Cyc_Absyn_visit_stmt;_Tmp2;})(({int(*_Tmp2)(struct _tuple21*,struct Cyc_Absyn_Exp*)=(int(*)(struct _tuple21*,struct Cyc_Absyn_Exp*))Cyc_Interproc_exp_noop;_Tmp2;}),({int(*_Tmp2)(struct _tuple21*,struct Cyc_Absyn_Stmt*)=(int(*)(struct _tuple21*,struct Cyc_Absyn_Stmt*))Cyc_Interproc_cvars_in_local_decl;_Tmp2;}),env,fd->body);
goto _LL0;}}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 696
ret=({struct Cyc_List_List*_Tmp2=Cyc_Interproc_cvars_in_type(({void*_Tmp3=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_Tmp4->tag=24,({union Cyc_Absyn_AggrInfo _Tmp5=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp6=ad;_Tmp6;}));_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_app_type(_Tmp3,_tag_fat(0U,sizeof(void*),0));}),0,0,0);Cyc_List_append(_Tmp2,ret);});
# 698
goto _LL0;}case 6: _Tmp1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp1;
# 700
ret=({struct Cyc_List_List*_Tmp2=Cyc_Interproc_cvars_in_type(({void*_Tmp3=(void*)({struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct));_Tmp4->tag=22,({union Cyc_Absyn_DatatypeInfo _Tmp5=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));*_Tmp6=dd;_Tmp6;}));_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_app_type(_Tmp3,_tag_fat(0U,sizeof(void*),0));}),0,0,0);Cyc_List_append(_Tmp2,ret);});
# 702
goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 704
ret=({struct Cyc_List_List*_Tmp2=Cyc_Interproc_cvars_in_type(Cyc_Absyn_typedef_type(td->name,0,td,0),0,0,0);Cyc_List_append(_Tmp2,ret);});
# 706
goto _LL0;}default:
# 708
 goto _LL0;}_LL0:;}
# 711
return ret;}
# 714
static struct Cyc_List_List*Cyc_Interproc_parse_constraint_graph (void){
const char*cfilename="./.cych/cvars";
struct Cyc___cycFILE*file=Cyc_Interproc_try_file_open(_tag_fat(cfilename,sizeof(char),14U),_tag_fat("r",sizeof(char),2U),_tag_fat("constraint file",sizeof(char),16U));
if(!((unsigned)file)){
Cyc_Warn_warn(0U,_tag_fat("Unable to initialize constraint graph",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));
return 0;}
# 721
Cyc_Position_reset_position(_tag_fat(cfilename,sizeof(char),14U));{
struct Cyc_List_List*ret=Cyc_Parse_parse_constraint_file(file);
Cyc_fclose(file);
return ret;}}
# 726
static int Cyc_Interproc_read_constraint_graph (void){
struct Cyc_List_List*cg=Cyc_Interproc_parse_constraint_graph();
Cyc_BansheeIf_init_toplevel_cvars(cg);
return 0;}
# 732
static void Cyc_Interproc_print_cvarlist(struct Cyc_List_List*l){
for(1;(unsigned)l;l=l->tl){
Cyc_Tcutil_deconstruct_type((void*)l->hd);}}
# 740
static void Cyc_Interproc_bind_cvars(struct Cyc_List_List*ds){
if(!Cyc_Flags_no_merge || Cyc_Interproc_read_constraint_graph())
Cyc_BansheeIf_init_toplevel_cvars(0);{
# 744
struct Cyc_List_List*cvars=Cyc_Interproc_assign_cvar_names(ds);
for(1;(unsigned)cvars;cvars=cvars->tl){
void*_Tmp0=(void*)cvars->hd;const char*_Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp0)==3){_Tmp5=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0)->f3;_Tmp2=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0)->f4;_Tmp1=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0)->f5;{struct Cyc_Core_Opt*ok=_Tmp5;void**topt=(void**)_Tmp4;int id=_Tmp3;void**bv=(void**)_Tmp2;const char*name=_Tmp1;
# 748
*topt==0?0:({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp6;})(_tag_fat("*topt==0",sizeof(char),9U),_tag_fat("interproc.cyc",sizeof(char),14U),748U);
id > 0?0:({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp6;})(_tag_fat("id>0",sizeof(char),5U),_tag_fat("interproc.cyc",sizeof(char),14U),749U);
*bv==0?0:({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp6;})(_tag_fat("*bv==0",sizeof(char),7U),_tag_fat("interproc.cyc",sizeof(char),14U),750U);
# 754
Cyc_BansheeIf_register_toplevel_cvar((void*)cvars->hd);
goto _LL0;}}else{
# 757
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Unexpected type in cvar list",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}}}
# 765
void Cyc_Interproc_emit_decls(struct _fat_ptr filename,struct Cyc_List_List*ds){
# 767
struct Cyc___cycFILE*file=Cyc_Interproc_try_file_open(_tag_fat("./.cych/cvars",sizeof(char),14U),_tag_fat("w",sizeof(char),2U),_tag_fat("constraint file",sizeof(char),16U));
if(!((unsigned)file)){
Cyc_Warn_warn(0U,_tag_fat("Unable to open constrant file",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));
return;}
# 772
Cyc_BansheeIf_persist_cvarmap(file);
Cyc_fclose(file);{
# 775
struct _fat_ptr outname=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=filename;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("./.cych/%s",sizeof(char),11U),_tag_fat(_Tmp1,sizeof(void*),1));});
struct Cyc___cycFILE*outfile=Cyc_Interproc_try_file_open(outname,_tag_fat("w",sizeof(char),2U),_tag_fat("output file",sizeof(char),12U));
Cyc_Absynpp_set_params(& Cyc_Absynpp_cycinf_params_r);
Cyc_Absynpp_decllist2file(ds,_check_null(outfile));
return;}}
# 783
struct Cyc_List_List*Cyc_Interproc_binding_cvar_resolution(struct Cyc_List_List*tds){
if(!Cyc_Flags_resolve)
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Cvar resolution is only supported with --resolve",sizeof(char),49U),_tag_fat(0U,sizeof(void*),0));
# 787
if(Cyc_Interproc_read_constraint_graph())
Cyc_Warn_err(0U,_tag_fat("Failed to read constraint graph; cannot resolve Cvars",sizeof(char),54U),_tag_fat(0U,sizeof(void*),0));
# 790
Cyc_Binding_resolve_all(tds);
Cyc_BansheeIf_resolve_or_default(Cyc_Absyn_thinconst());
return tds;}struct _tuple22{const char*f0;const char*f1;};
# 795
int Cyc_Interproc_cmp_stats(struct _tuple22*a,struct _tuple22*b){
return Cyc_strcmp(({const char*_Tmp0=a->f1;_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}),({const char*_Tmp0=b->f1;_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}));}
# 800
struct Cyc_List_List*Cyc_Interproc_resolve_all(struct _fat_ptr filename,struct Cyc_List_List*tds){
struct Cyc_List_List*orig=tds;
struct Cyc_List_List*first=0;struct Cyc_List_List*worklist=0;
first=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp1=({struct _tuple14*_Tmp2=_cycalloc(sizeof(struct _tuple14));_Tmp2->f0=filename,_Tmp2->f1=tds;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});{
struct Cyc_List_List*all_located_fnsyms=0;
do{
if((unsigned)worklist){
tds=((struct _tuple14*)worklist->hd)->f1;
worklist=worklist->tl;}{
# 810
struct Cyc_List_List*externs=Cyc_Binding_resolve_all_interproc(tds);
if((unsigned)externs){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple17 _Tmp2=Cyc_Interproc_locate_externs(externs,first);void*_Tmp3;void*_Tmp4;int _Tmp5;_Tmp5=_Tmp2.f0;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{int b=_Tmp5;struct Cyc_List_List*nd=_Tmp4;struct Cyc_List_List*rext=_Tmp3;
if(!b){
struct Cyc_List_List*_Tmp6=tds;_npop_handler(0);return _Tmp6;}
if(!((unsigned)worklist))
worklist=nd;
# 819
all_located_fnsyms=Cyc_List_imp_append(rext,all_located_fnsyms);}}
# 813
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Interproc_Unresolved_sym_exn_struct*)_Tmp2)->tag==Cyc_Interproc_Unresolved_sym){_Tmp3=((struct Cyc_Interproc_Unresolved_sym_exn_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 822
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("Undefined symbol",sizeof(char),17U);_Tmp5;});struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Exp_Warn_Warg_struct _Tmp6;_Tmp6.tag=4,_Tmp6.f1=e;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_Warn_err2(e->loc,_tag_fat(_Tmp6,sizeof(void*),2));});
return 0;}}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}}}while((unsigned)worklist);{
# 827
struct Cyc_List_List*prev=((struct _tuple14*)first->hd)->f1;
struct Cyc_List_List*all_decls=orig;
{struct Cyc_List_List*it=first->tl;for(0;(unsigned)it;it=it->tl){
all_decls=Cyc_List_imp_append(all_decls,((struct _tuple14*)it->hd)->f1);}}
# 835
Cyc_Interproc_bind_cvars(all_decls);
return all_decls;}}}struct _tuple23{void*f0;struct Cyc_List_List*f1;};struct _tuple24{const char*f0;const char*f1;int f2;};
# 839
void Cyc_Interproc_inference_statistics (void){
struct Cyc_List_List*graph=Cyc_Interproc_parse_constraint_graph();
if(Cyc_BansheeIf_add_all_constraints(graph)){
Cyc_fprintf(Cyc_stderr,_tag_fat("Inconsistent constraint graph\n",sizeof(char),31U),_tag_fat(0U,sizeof(void*),0));
return;}{
# 845
struct Cyc_List_List*thins=0;
struct Cyc_List_List*fats=0;
for(1;(unsigned)graph;graph=graph->tl){
struct _tuple23*_Tmp0=(struct _tuple23*)graph->hd;void*_Tmp1;_Tmp1=_Tmp0->f0;{void*t=_Tmp1;
int _Tmp2;const char*_Tmp3;const char*_Tmp4;if(*((int*)t)==3){_Tmp4=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)t)->f5;_Tmp3=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)t)->f6;_Tmp2=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)t)->f7;{const char*name=_Tmp4;const char*loc=_Tmp3;int origfat=_Tmp2;
# 851
{void*_Tmp5=Cyc_Absyn_compress(t);switch(*((int*)_Tmp5)){case 3:
 goto _LLC;case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)){case 13: _LLC:
# 854
 thins=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp7=({struct _tuple24*_Tmp8=_cycalloc(sizeof(struct _tuple24));_Tmp8->f0=name,_Tmp8->f1=loc,_Tmp8->f2=origfat;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=thins;_Tmp6;});
goto _LL8;case 14:
# 857
 fats=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp7=({struct _tuple24*_Tmp8=_cycalloc(sizeof(struct _tuple24));_Tmp8->f0=name,_Tmp8->f1=loc,_Tmp8->f2=origfat;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=fats;_Tmp6;});
goto _LL8;default: goto _LLF;}default: _LLF:
# 860
 goto _LL8;}_LL8:;}
# 862
goto _LL3;}}else{
# 864
goto _LL3;}_LL3:;}}
# 867
fats=({struct Cyc_List_List*(*_Tmp0)(int(*)(struct _tuple24*,struct _tuple24*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple24*,struct _tuple24*),struct Cyc_List_List*))Cyc_List_merge_sort;_Tmp0;})(Cyc_Interproc_cmp_stats,fats);
thins=({struct Cyc_List_List*(*_Tmp0)(int(*)(struct _tuple24*,struct _tuple24*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple24*,struct _tuple24*),struct Cyc_List_List*))Cyc_List_merge_sort;_Tmp0;})(Cyc_Interproc_cmp_stats,thins);
Cyc_fprintf(Cyc_stderr,_tag_fat("******************************FAT POINTERS******************************\n",sizeof(char),74U),_tag_fat(0U,sizeof(void*),0));
for(1;(unsigned)fats;fats=fats->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=((struct _tuple24*)fats->hd)->f1;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3=((struct _tuple24*)fats->hd)->f0;_tag_fat((void*)_Tmp3,sizeof(char),_get_zero_arr_size_char((void*)_Tmp3,1U));});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("%s %50s",sizeof(char),8U),_tag_fat(_Tmp2,sizeof(void*),2));});
if(!((struct _tuple24*)fats->hd)->f2)
Cyc_fprintf(Cyc_stderr,_tag_fat(" --EXTRA",sizeof(char),9U),_tag_fat(0U,sizeof(void*),0));
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 876
Cyc_fprintf(Cyc_stderr,_tag_fat("******************************THIN POINTERS******************************\n",sizeof(char),75U),_tag_fat(0U,sizeof(void*),0));
for(1;(unsigned)thins;thins=thins->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=((struct _tuple24*)thins->hd)->f1;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3=((struct _tuple24*)thins->hd)->f0;_tag_fat((void*)_Tmp3,sizeof(char),_get_zero_arr_size_char((void*)_Tmp3,1U));});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("%s %50s",sizeof(char),8U),_tag_fat(_Tmp2,sizeof(void*),2));});
if(((struct _tuple24*)thins->hd)->f2)
Cyc_fprintf(Cyc_stderr,_tag_fat(" --ORIG-FAT",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));}
# 883
Cyc_fprintf(Cyc_stderr,_tag_fat("***********************************DONE*************************************\n",sizeof(char),78U),_tag_fat(0U,sizeof(void*),0));}}

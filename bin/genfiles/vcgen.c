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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f0;void*f1;};
# 112 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 125
int Cyc_Core_ptrcmp(void*,void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 76 "list.h"
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 145
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 220
extern struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 36 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 139 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 160
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;void*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;void*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;void*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 493 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 500
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 518
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 532
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 695 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 878
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 898
void*Cyc_Absyn_compress(void*);
# 916
extern void*Cyc_Absyn_uint_type;
# 921
void*Cyc_Absyn_gen_float_type(unsigned);
# 968
void*Cyc_Absyn_bounds_one (void);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
# 1170
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1179
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct _tuple13{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 59 "tcutil.h"
void*Cyc_Tcutil_pointer_elt_type(void*);
# 69
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 81
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 212
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 226
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 237
int Cyc_Tcutil_force_type2bool(int,void*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 92 "assndef.h"
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory;
# 96
extern void*Cyc_AssnDef_uint(unsigned);
extern void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
extern void*Cyc_AssnDef_zero (void);
extern void*Cyc_AssnDef_one (void);
# 101
extern void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*);
extern void*Cyc_AssnDef_addr(struct Cyc_Absyn_Vardecl*);
extern void*Cyc_AssnDef_select(void*,void*,void*);
extern void*Cyc_AssnDef_update(void*,void*,void*);
extern void*Cyc_AssnDef_logicvar(struct Cyc_Absyn_Vardecl*,int,void*);
# 107
extern void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
extern void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop,void*,void*);
extern void*Cyc_AssnDef_plus(void*,void*,void*);
extern void*Cyc_AssnDef_minus(void*,void*,void*);
extern void*Cyc_AssnDef_cast(void*,void*);
extern void*Cyc_AssnDef_proj(void*,unsigned,void*);
# 114
extern void*Cyc_AssnDef_struct_aggr(struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_datatype_aggr(unsigned,struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_union_aggr(unsigned,struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_tagof(void*);
extern void*Cyc_AssnDef_okderef(void*);
extern void*Cyc_AssnDef_numelts_term(void*);
# 123
extern void*Cyc_AssnDef_fresh_var(void*);
# 131
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 146
extern struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 150
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 159
extern void*Cyc_AssnDef_and(void*,void*);
extern void*Cyc_AssnDef_or(void*,void*);
extern void*Cyc_AssnDef_not(void*);
extern void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*,void*,void*);
# 164
extern void*Cyc_AssnDef_eq(void*,void*);
extern void*Cyc_AssnDef_neq(void*,void*);
extern void*Cyc_AssnDef_slt(void*,void*);
extern void*Cyc_AssnDef_slte(void*,void*);
extern void*Cyc_AssnDef_ult(void*,void*);
extern void*Cyc_AssnDef_ulte(void*,void*);
# 172
extern void*Cyc_AssnDef_reduce(void*);
extern void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
# 181
extern void*Cyc_AssnDef_widen(void*);
# 184
extern int Cyc_AssnDef_simple_prove(void*,void*);struct Cyc_Vcgen_ExpChecks{void*ctxt;void*bounds_check;void*null_check;};
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 73 "flags.h"
extern int Cyc_Flags_allpaths;struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 126 "dict.h"
extern int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict,void*,void**);
# 193
extern struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
# 159 "cf_flowinfo.h"
extern int Cyc_CfFlowInfo_get_field_index(void*,struct _fat_ptr*);struct Cyc_Vcgen_Env;
# 137 "vcgen.cyc"
static int Cyc_Vcgen_simple_implies(void**a1,void**a2){
({void*_Tmp0=Cyc_AssnDef_reduce(*a1);*a1=_Tmp0;});
({void*_Tmp0=Cyc_AssnDef_reduce(*a2);*a2=_Tmp0;});
return Cyc_AssnDef_simple_prove(*a1,*a2);}
# 144
static int Cyc_Vcgen_implies(void**a1,void**a2){
return Cyc_Vcgen_simple_implies(a1,a2)|| Cyc_PrattProver_constraint_prove(*a1,*a2);}
# 152
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Vardecl*);
# 155
static void*Cyc_Vcgen_kill_var(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
void*v2=Cyc_Vcgen_fresh_nonescvar(env,vd);
a=Cyc_AssnDef_subst(vd,v2,a);
return a;}
# 162
static void*Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,void*a,void*tgt,void*src){
void*_Tmp0;if(tgt!=0){if(*((int*)tgt)==2){_Tmp0=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)tgt)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
# 168
void*v2=Cyc_Vcgen_fresh_nonescvar(env,vd);
a=Cyc_AssnDef_subst(vd,v2,a);
src=src==0?0: Cyc_AssnDef_subst_term(src,vd,v2);
return({void*_Tmp1=a;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(tgt,src));});}}else{goto _LL3;}}else{_LL3: {
# 177
void*m=Cyc_Vcgen_fresh_nonescvar(env,Cyc_AssnDef_memory);
a=Cyc_AssnDef_subst(Cyc_AssnDef_memory,m,a);{
void*upd=0;
if(src!=0 && tgt!=0)
upd=({void*_Tmp1=m;void*_Tmp2=Cyc_AssnDef_subst_term(tgt,Cyc_AssnDef_memory,m);Cyc_AssnDef_update(_Tmp1,_Tmp2,Cyc_AssnDef_subst_term(src,Cyc_AssnDef_memory,m));});
return({void*_Tmp1=a;Cyc_AssnDef_and(_Tmp1,({void*_Tmp2=Cyc_AssnDef_var(Cyc_AssnDef_memory);Cyc_AssnDef_eq(_Tmp2,upd);}));});}}};}struct Cyc_Vcgen_SharedEnv{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;void**try_assn;void**exp_stmt;void*res_term;void**res_assn;int widen_paths;};struct Cyc_Vcgen_Env{struct Cyc_Vcgen_SharedEnv*shared;struct Cyc_Dict_Dict state_counter;struct Cyc_Hashtable_Table**assn_info;};
# 224 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 226
static void*Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*env,void*a){
return env->shared->widen_paths?Cyc_AssnDef_widen(a): a;}
# 230
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,void**res_assn,void**try_assn,void*res_term){
# 232
struct Cyc_Hashtable_Table*succ_table=({(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;})(tables->succ_tables,fd);
struct Cyc_Hashtable_Table*assn_table=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;})(57,({(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;}));
struct Cyc_Vcgen_SharedEnv*senv;senv=_cycalloc(sizeof(struct Cyc_Vcgen_SharedEnv)),senv->assn_table=assn_table,senv->succ_table=succ_table,senv->try_assn=try_assn,senv->exp_stmt=0,senv->res_term=res_term,senv->res_assn=res_assn,senv->widen_paths=!Cyc_Flags_allpaths;{
# 241
struct Cyc_Dict_Dict d=({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty;})(({(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp;}));({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int))Cyc_Dict_insert;})(_Tmp0,Cyc_AssnDef_memory,0);});
return({struct Cyc_Vcgen_Env*_Tmp0=_cycalloc(sizeof(struct Cyc_Vcgen_Env));_Tmp0->shared=senv,_Tmp0->state_counter=d,_Tmp0->assn_info=assn_info;_Tmp0;});}}
# 246
static struct Cyc_Vcgen_Env*Cyc_Vcgen_bogus_env (void){
# 248
static struct Cyc_Vcgen_SharedEnv*bogus_shared=0;
struct Cyc_Hashtable_Table**assn_info=0;
struct Cyc_Vcgen_SharedEnv*senv;
if(bogus_shared==0){
struct Cyc_Hashtable_Table*succ_table=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;})(1,({(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;}));
struct Cyc_Hashtable_Table*assn_table=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;})(1,({(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;}));
senv=({struct Cyc_Vcgen_SharedEnv*_Tmp0=_cycalloc(sizeof(struct Cyc_Vcgen_SharedEnv));_Tmp0->assn_table=assn_table,_Tmp0->succ_table=succ_table,_Tmp0->try_assn=0,_Tmp0->exp_stmt=0,_Tmp0->res_term=0,_Tmp0->res_assn=0,_Tmp0->widen_paths=0;_Tmp0;});
# 261
bogus_shared=senv;}else{
senv=_check_null(bogus_shared);}
if(assn_info==0)
assn_info=({struct Cyc_Hashtable_Table**_Tmp0=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));({struct Cyc_Hashtable_Table*_Tmp1=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;})(1,({(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr;}));*_Tmp0=_Tmp1;});_Tmp0;});{
struct Cyc_Dict_Dict d=({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty;})(({(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp;}));({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int))Cyc_Dict_insert;})(_Tmp0,Cyc_AssnDef_memory,0);});
return({struct Cyc_Vcgen_Env*_Tmp0=_cycalloc(sizeof(struct Cyc_Vcgen_Env));_Tmp0->shared=senv,_Tmp0->state_counter=d,_Tmp0->assn_info=assn_info;_Tmp0;});}}
# 269
static struct Cyc_Vcgen_Env*Cyc_Vcgen_copy_env(struct Cyc_Vcgen_Env*env){
return({struct Cyc_Vcgen_Env*_Tmp0=_cycalloc(sizeof(struct Cyc_Vcgen_Env));*_Tmp0=*env;_Tmp0;});}
# 275
static int Cyc_Vcgen_max_counter(int dummy,struct Cyc_Absyn_Vardecl*vd,int i,int j){
return i < j?j: i;}
# 279
static void Cyc_Vcgen_merge_env(struct Cyc_Vcgen_Env*old_env,struct Cyc_Vcgen_Env*new_env){
({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(int(*)(int,struct Cyc_Absyn_Vardecl*,int,int),int,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_union_two_c;})(Cyc_Vcgen_max_counter,0,old_env->state_counter,new_env->state_counter);old_env->state_counter=_Tmp0;});}
# 287
static void*Cyc_Vcgen_fresh_nonescvar(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){
int res=0;
({(int(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int*))Cyc_Dict_lookup_bool;})(env->state_counter,vd,& res);
({struct Cyc_Dict_Dict _Tmp0=({(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,int))Cyc_Dict_insert;})(env->state_counter,vd,res + 1);env->state_counter=_Tmp0;});
return Cyc_AssnDef_logicvar(vd,res,vd->type);}
# 296
static void*Cyc_Vcgen_pre_stmt_update(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***popt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->shared->assn_table,s);
if(popt!=0){
void**p=*popt;
void*new_precondition=Cyc_AssnDef_or(ain,*p);
*p=new_precondition;
return new_precondition;}else{
# 304
return ain;}}
# 308
static void**Cyc_Vcgen_get_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
static void*false_pointer=(void*)& Cyc_AssnDef_false_assn;
void***popt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->shared->assn_table,s);
if(popt!=0)
return*popt;else{
# 314
return& false_pointer;}}
# 319
static void*Cyc_Vcgen_loop_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->shared->assn_table,s);
if(aprevopt!=0){
void**aprev=*aprevopt;
# 324
ain=Cyc_AssnDef_widen(Cyc_AssnDef_or(ain,*aprev));
*aprev=ain;}else{
# 327
({struct Cyc_Hashtable_Table*_Tmp0=env->shared->assn_table;struct Cyc_Absyn_Stmt*_Tmp1=s;({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert;})(_Tmp0,_Tmp1,({void**_Tmp2=_cycalloc(sizeof(void*));*_Tmp2=ain;_Tmp2;}));});}
# 329
return ain;}
# 334
static void*Cyc_Vcgen_forward_assn(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
void***aprevopt=({(void***(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;})(env->shared->assn_table,s);
if(aprevopt!=0){
void**aprev=*aprevopt;
ain=({struct Cyc_Vcgen_Env*_Tmp0=env;Cyc_Vcgen_may_widen(_Tmp0,Cyc_AssnDef_or(ain,*aprev));});
*aprev=ain;}else{
# 341
({struct Cyc_Hashtable_Table*_Tmp0=env->shared->assn_table;struct Cyc_Absyn_Stmt*_Tmp1=s;({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,void**))Cyc_Hashtable_insert;})(_Tmp0,_Tmp1,({void**_Tmp2=_cycalloc(sizeof(void*));*_Tmp2=ain;_Tmp2;}));});}
# 343
return ain;}
# 348
static void Cyc_Vcgen_update_try_assn(struct Cyc_Vcgen_Env*env,void*a){
if(env->shared->try_assn==0)return;
a=({struct Cyc_Vcgen_Env*_Tmp0=env;Cyc_Vcgen_may_widen(_Tmp0,Cyc_AssnDef_or(*env->shared->try_assn,a));});
*env->shared->try_assn=a;}
# 355
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*t,void*a,void*topt){
struct Cyc_Vcgen_SharedEnv*shared=env->shared;
if(shared->res_assn==0)return;
if(shared->res_term!=0)
({void*_Tmp0=({void*_Tmp1=({void*_Tmp2=a;Cyc_AssnDef_and(_Tmp2,Cyc_AssnDef_eq(shared->res_term,t));});Cyc_AssnDef_or(_Tmp1,*shared->res_assn);});*shared->res_assn=_Tmp0;});else{
# 361
({void*_Tmp0=Cyc_AssnDef_or(a,*shared->res_assn);*shared->res_assn=_Tmp0;});}}
# 366
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void){
struct Cyc_Hashtable_Table*assn_info=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;})(57,({(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;}),({(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr;}));
return({struct Cyc_Hashtable_Table**_Tmp0=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));*_Tmp0=assn_info;_Tmp0;});}
# 371
struct Cyc_Vcgen_ExpChecks*Cyc_Vcgen_exp2ctxt_checks(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){
struct Cyc_Vcgen_ExpChecks**result=({(struct Cyc_Vcgen_ExpChecks**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_lookup_opt;})(*_check_null(assn_info),e);
if(result==0){
static struct Cyc_Vcgen_ExpChecks b={(void*)& Cyc_AssnDef_true_assn,(void*)& Cyc_AssnDef_false_assn,(void*)& Cyc_AssnDef_false_assn};
# 379
return& b;}else{
# 381
return*result;}}
# 385
static void Cyc_Vcgen_insert_exp_checks(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct Cyc_Vcgen_ExpChecks*exp_check){
# 394 "vcgen.cyc"
({(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*))Cyc_Hashtable_insert;})(*_check_null(assn_info),e,exp_check);
return;}
# 398
static struct Cyc_Absyn_Exp*Cyc_Vcgen_zero_exp (void){
struct Cyc_Absyn_Exp*ans=Cyc_Absyn_uint_exp(0U,0U);
ans->topt=Cyc_Absyn_uint_type;
return ans;}
# 413
static struct _tuple0 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 415
static struct _tuple0 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);
# 420
static struct _tuple0 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,void*);struct _tuple15{struct Cyc_List_List*f0;void*f1;};
# 429
static struct _tuple15 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,void*);
# 431
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*,void*,struct Cyc_List_List*,void*);
# 435
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Decl*,void*);
# 437
static struct _fat_ptr Cyc_Vcgen_stmt2shortstring(struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 4: _Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 440
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("if (%s) ...",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 5: _Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 442
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("while (%s) ...",sizeof(char),15U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 9: _Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;struct Cyc_Absyn_Exp*e3=_Tmp1;
# 444
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e2));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e3));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(_tag_fat("for (%s;%s;%s) ...",sizeof(char),19U),_tag_fat(_Tmp7,sizeof(void*),3));});}case 10: _Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 447
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("switch (%s) ...",sizeof(char),16U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 14: _Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 449
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("do ... while (%s)",sizeof(char),18U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 15: _Tmp3=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp3;
# 451
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Vcgen_stmt2shortstring(s));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("try %s catch ...",sizeof(char),17U),_tag_fat(_Tmp5,sizeof(void*),1));});}case 2: _Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp3;
return Cyc_Vcgen_stmt2shortstring(s);}default:  {
# 454
struct _fat_ptr _Tmp4=Cyc_Absynpp_stmt2string(s);struct _fat_ptr s=_Tmp4;
unsigned max=_get_fat_size(s,sizeof(char))< 11U?_get_fat_size(s,sizeof(char)): 11U;
struct _fat_ptr res=({unsigned _Tmp5=sizeof(char)* max;_tag_fat(_cycalloc_atomic(_Tmp5),1U,_Tmp5);});
{int i=0;for(0;(unsigned)i < max - 1U;++ i){
({char _Tmp5=*((const char*)_check_fat_subscript(s,sizeof(char),i));*((char*)_check_fat_subscript(res,sizeof(char),i))=_Tmp5;});}}
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)res);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("%s...",sizeof(char),6U),_tag_fat(_Tmp6,sizeof(void*),1));});}};}
# 515 "vcgen.cyc"
static struct Cyc_List_List*Cyc_Vcgen_heuristic_loop_invariants(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
struct Cyc_List_List*loop_invariants=0;
{void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 9: _Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Stmt*dummy2=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;struct Cyc_Absyn_Stmt*dummy3=_Tmp2;struct Cyc_Absyn_Stmt*s=_Tmp1;
# 519
{void*_Tmp7=e2->r;void*_Tmp8;if(*((int*)_Tmp7)==3){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp7)->f1==Cyc_Absyn_Lt){_Tmp8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_List_List*es=_Tmp8;
# 521
struct Cyc_Absyn_Exp*exp=Cyc_Absyn_primop_exp(10U,es,0U);
exp->topt=e2->topt;{
void*a=({struct Cyc_Vcgen_Env*_Tmp9=env;Cyc_Vcgen_vcgen_test(_Tmp9,Cyc_Absyn_primop_exp(10U,es,0U),(void*)& Cyc_AssnDef_true_assn);}).f0;
ain=Cyc_AssnDef_reduce(ain);
a=Cyc_AssnDef_reduce(a);
if(Cyc_Vcgen_implies(& ain,& a))loop_invariants=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=a,_Tmp9->tl=loop_invariants;_Tmp9;});
goto _LL9;}}}else{goto _LLC;}}else{_LLC:
 goto _LL9;}_LL9:;}
# 530
goto _LL0;}case 5: _Tmp6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f1;_Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*dummy=_Tmp5;struct Cyc_Absyn_Stmt*s=_Tmp4;
# 532
goto _LL0;}case 14: _Tmp6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp5;struct Cyc_Absyn_Stmt*dummy=_Tmp4;
# 534
goto _LL0;}default:
# 536
 goto _LL0;}_LL0:;}
# 538
return loop_invariants;}
# 541
static struct Cyc_Absyn_Exp*Cyc_Vcgen_find_assert_e(struct Cyc_Absyn_Stmt*s){
while(1){
void*_Tmp0=s->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1)->r)==41){_Tmp1=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1->r)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
return e;}}else{goto _LL5;}case 2: _Tmp1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*sa=_Tmp1;
s=sa;continue;}default: _LL5:
 return 0;};}}
# 559 "vcgen.cyc"
static void*Cyc_Vcgen_vcgen_stmt_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,void*ain){
# 562
LOOP:
 ain=Cyc_Vcgen_pre_stmt_update(env,s,ain);{
# 574 "vcgen.cyc"
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 0:
 return ain;case 1: _Tmp6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 577
struct _tuple0 _Tmp7=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*t=_Tmp9;void*aout=_Tmp8;
# 580
if(env->shared->exp_stmt!=0)
*env->shared->exp_stmt=t;
return aout;}}case 2: _Tmp6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_Absyn_Stmt*s2=_Tmp5;
# 584
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,ain);
s=s2;
goto LOOP;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1==0){
# 588
Cyc_Vcgen_update_return(env,0,ain,0);
return(void*)& Cyc_AssnDef_false_assn;}else{_Tmp6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 591
struct _tuple0 _Tmp7=Cyc_Vcgen_vcgen_rexp_nodebug(env,_check_null(e),ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*t=_Tmp9;void*aout=_Tmp8;
Cyc_Vcgen_update_return(env,t,aout,e->topt);
return(void*)& Cyc_AssnDef_false_assn;}}}case 4: _Tmp6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 595
struct _tuple0 _Tmp7=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*at=_Tmp9;void*af=_Tmp8;
struct Cyc_Vcgen_Env*env2=Cyc_Vcgen_copy_env(env);
at=Cyc_Vcgen_vcgen_stmt_nodebug(env,s1,at);
af=Cyc_Vcgen_vcgen_stmt_nodebug(env2,s2,af);
Cyc_Vcgen_merge_env(env,env2);
return({struct Cyc_Vcgen_Env*_TmpA=env;Cyc_Vcgen_may_widen(_TmpA,Cyc_AssnDef_or(at,af));});}}case 5: _Tmp6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f1;_Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*dummy=_Tmp5;struct Cyc_Absyn_Stmt*s=_Tmp4;
# 607
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_Tmp7=e->r;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp7)==7){_Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*eleft=_Tmp9;struct Cyc_Absyn_Exp*eright=_Tmp8;
# 612
{void*_TmpA=eleft->r;void*_TmpB;if(*((int*)_TmpA)==41){_TmpB=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpA)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpB;
# 614
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*tm=_TmpE;void*an=_TmpD;
loop_inv=Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn).f0;
e=eright;
ain=an;
goto _LL33;}}}else{
# 620
goto _LL33;}_LL33:;}
# 622
goto _LL2E;}}else{
# 624
goto _LL2E;}_LL2E:;}
# 626
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy,ain);
ain=Cyc_AssnDef_and(ain,loop_inv);{
struct _tuple0 _Tmp7=Cyc_Vcgen_vcgen_test(env,e,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*at=_Tmp9;void*af=_Tmp8;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 633
({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_TmpB.f1=_TmpC;});_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({struct _fat_ptr _TmpD=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_err(e->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_TmpC,sizeof(void*),2));});
return(void*)& Cyc_AssnDef_true_assn;}
# 637
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;}}}}case 7:
# 639
 goto _LL12;{
# 642
struct Cyc_Absyn_Stmt*dest=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(env->shared->succ_table,s);
Cyc_Vcgen_loop_assn(env,dest,ain);
return(void*)& Cyc_AssnDef_false_assn;}case 6: _LL12: {
# 649
struct Cyc_Absyn_Stmt*dest=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(env->shared->succ_table,s);
if(dest==0)
Cyc_Vcgen_update_return(env,0,ain,0);else{
# 653
Cyc_Vcgen_forward_assn(env,dest,ain);}
return(void*)& Cyc_AssnDef_false_assn;}case 9: _Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Stmt*dummy2=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;struct Cyc_Absyn_Stmt*dummy3=_Tmp2;struct Cyc_Absyn_Stmt*s=_Tmp1;
# 656
struct _tuple0 _Tmp7=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_Tmp8;_Tmp8=_Tmp7.f1;{void*a=_Tmp8;
ain=a;{
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_Tmp9=e2->r;void*_TmpA;void*_TmpB;if(*((int*)_Tmp9)==7){_TmpB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp9)->f1;_TmpA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp9)->f2;{struct Cyc_Absyn_Exp*eleft=_TmpB;struct Cyc_Absyn_Exp*eright=_TmpA;
# 663
{void*_TmpC=eleft->r;void*_TmpD;if(*((int*)_TmpC)==41){_TmpD=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpC)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpD;
# 665
struct _tuple0 _TmpE=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);void*_TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*tm=_Tmp10;void*an=_TmpF;
loop_inv=Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn).f0;
e2=eright;
ain=an;
goto _LL46;}}}else{
# 671
goto _LL46;}_LL46:;}
# 673
goto _LL41;}}else{
# 675
goto _LL41;}_LL41:;}{
# 677
struct Cyc_List_List*heuristic_inv=({struct Cyc_Vcgen_Env*_Tmp9=env;struct Cyc_Absyn_Stmt*_TmpA=Cyc_Absyn_for_stmt(e1,e2,e3,s,0U);Cyc_Vcgen_heuristic_loop_invariants(_Tmp9,_TmpA,ain);});
int right_loop_inv;
while(1){
ain=Cyc_Vcgen_loop_assn(env,dummy2,ain);
ain=Cyc_AssnDef_and(ain,loop_inv);
ain=Cyc_List_fold_left(Cyc_AssnDef_and,ain,heuristic_inv);{
struct _tuple0 _Tmp9=Cyc_Vcgen_vcgen_test(env,e2,ain);void*_TmpA;void*_TmpB;_TmpB=_Tmp9.f0;_TmpA=_Tmp9.f1;{void*at=_TmpB;void*af=_TmpA;
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,at);
# 686
ain=Cyc_Vcgen_pre_stmt_update(env,dummy3,ain);
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,ain).f1;{
# 689
struct Cyc_List_List*invs=heuristic_inv;
# 691
heuristic_inv=0;
right_loop_inv=1;
for(1;invs!=0;invs=invs->tl){
if(Cyc_Vcgen_implies(& ain,(void**)((void*)& invs->hd)))
heuristic_inv=({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));_TmpC->hd=(void*)invs->hd,_TmpC->tl=heuristic_inv;_TmpC;});else{
# 697
right_loop_inv=0;}}
# 699
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 701
({struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,({
struct _fat_ptr _TmpE=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_TmpD.f1=_TmpE;});_TmpD;});struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,({struct _fat_ptr _TmpF=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_TmpE.f1=_TmpF;});_TmpE;});void*_TmpE[2];_TmpE[0]=& _TmpC,_TmpE[1]=& _TmpD;Cyc_Warn_err(e2->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_TmpE,sizeof(void*),2));});
return(void*)& Cyc_AssnDef_true_assn;}
# 705
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy2))&&(
Cyc_Vcgen_implies(& ain,& loop_inv)&& right_loop_inv)){
loop_inv=Cyc_List_fold_left(Cyc_AssnDef_and,loop_inv,heuristic_inv);
return Cyc_AssnDef_and(loop_inv,af);}}}}}}}}}case 14: _Tmp6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp5;struct Cyc_Absyn_Stmt*dummy=_Tmp4;
# 713
void*loop_inv=(void*)& Cyc_AssnDef_true_assn;
{void*_Tmp7=e->r;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp7)==7){_Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*eleft=_Tmp9;struct Cyc_Absyn_Exp*eright=_Tmp8;
# 718
{void*_TmpA=eleft->r;void*_TmpB;if(*((int*)_TmpA)==41){_TmpB=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpA)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpB;
# 720
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,eleft,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*tm=_TmpE;void*an=_TmpD;
loop_inv=Cyc_Vcgen_vcgen_test(env,eassn,(void*)& Cyc_AssnDef_true_assn).f0;
e=eright;
ain=an;
goto _LL56;}}}}else{
# 727
goto _LL56;}_LL56:;}
# 729
goto _LL51;}}else{
# 731
goto _LL51;}_LL51:;}
# 733
while(1){
ain=Cyc_AssnDef_and(ain,loop_inv);
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);
if(!Cyc_Vcgen_implies(& ain,& loop_inv)){
# 738
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(loop_inv));_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(ain));_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err(e->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_Tmp9,sizeof(void*),2));});
return(void*)& Cyc_AssnDef_true_assn;}{
# 742
struct _tuple0 _Tmp7=Cyc_Vcgen_vcgen_test(env,e,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*at=_Tmp9;void*af=_Tmp8;
ain=({struct Cyc_Vcgen_Env*_TmpA=env;Cyc_Vcgen_may_widen(_TmpA,({void*_TmpB=*Cyc_Vcgen_get_assn(env,dummy);Cyc_AssnDef_or(_TmpB,ain);}));});
if(Cyc_Vcgen_simple_implies(& ain,Cyc_Vcgen_get_assn(env,dummy)))return af;
Cyc_Vcgen_loop_assn(env,dummy,ain);}}}}case 10: _Tmp6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_List_List*scs=_Tmp5;void*dtree=_Tmp4;
# 753
struct _tuple0 _Tmp7=Cyc_Vcgen_vcgen_rexp_always(env,e,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*t=_Tmp9;void*a=_Tmp8;
return Cyc_Vcgen_vcgen_switch(env,t,scs,a);}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2==0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("vcgen fallthru",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));else{_Tmp6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp6;struct Cyc_Absyn_Switch_clause*dest=_Tmp5;
# 758
struct _tuple15 _Tmp7=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{struct Cyc_List_List*ts=_Tmp9;void*a=_Tmp8;
if(ts!=0){
# 761
struct Cyc_List_List*x=Cyc_Tcutil_filter_nulls(Cyc_List_split((struct Cyc_List_List*)_check_null(dest->pat_vars)->v).f0);
# 763
for(1;x!=0;(x=x->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)x->hd;
if(vd!=0)
a=({struct Cyc_Vcgen_Env*_TmpA=env;void*_TmpB=a;void*_TmpC=Cyc_AssnDef_var(vd);Cyc_Vcgen_do_assign(_TmpA,_TmpB,_TmpC,(void*)_check_null(ts)->hd);});}}
# 769
Cyc_Vcgen_forward_assn(env,dest->body,a);
return(void*)& Cyc_AssnDef_false_assn;}}}case 12: _Tmp6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
# 772
ain=Cyc_Vcgen_vcgen_local_decl(env,d,ain);
s=s1;
goto LOOP;}case 13: _Tmp6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;
# 779
struct Cyc_Absyn_Exp*label_assn_opt=Cyc_Vcgen_find_assert_e(s1);
if(label_assn_opt==0)
# 785
ain=(void*)& Cyc_AssnDef_true_assn;
# 787
s=s1;
goto LOOP;}case 8:  {
# 792
struct Cyc_Absyn_Stmt*destopt=({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;})(env->shared->succ_table,s);
if(destopt!=0){
void*_Tmp7=destopt->r;void*_Tmp8;if(*((int*)_Tmp7)==13){_Tmp8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp8;
# 796
struct Cyc_Absyn_Exp*label_assn_opt=Cyc_Vcgen_find_assert_e(s1);
if(label_assn_opt==0)goto _LL67;{
struct Cyc_Absyn_Exp*label_assn=label_assn_opt;
struct _tuple0 _Tmp9=Cyc_Vcgen_vcgen_test(env,label_assn,(void*)& Cyc_AssnDef_true_assn);void*_TmpA;_TmpA=_Tmp9.f0;{void*at=_TmpA;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({
struct _fat_ptr _TmpD=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(at));_TmpC.f1=_TmpD;});_TmpC;});struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,({struct _fat_ptr _TmpE=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_TmpD.f1=_TmpE;});_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;Cyc_Warn_err(s->loc,_tag_fat("cannot prove @assert(%s)\nfrom %s",sizeof(char),33U),_tag_fat(_TmpD,sizeof(void*),2));});
goto _LL67;}}}}else{
goto _LL67;}_LL67:;}
# 807
return(void*)& Cyc_AssnDef_false_assn;}default: _Tmp6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_List_List*scs=_Tmp5;void*dtree=_Tmp4;
# 810
void**old_try_assn=env->shared->try_assn;
# 812
({void**_Tmp7=({void**_Tmp8=_cycalloc(sizeof(void*));*_Tmp8=(void*)& Cyc_AssnDef_false_assn;_Tmp8;});env->shared->try_assn=_Tmp7;});
# 815
ain=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);{
# 817
void*new_try_assn=*_check_null(env->shared->try_assn);
env->shared->try_assn=old_try_assn;
# 821
return({void*_Tmp7=ain;Cyc_AssnDef_or(_Tmp7,({struct Cyc_Vcgen_Env*_Tmp8=env;void*_Tmp9=Cyc_AssnDef_fresh_var(0);struct Cyc_List_List*_TmpA=scs;Cyc_Vcgen_vcgen_switch(_Tmp8,_Tmp9,_TmpA,new_try_assn);}));});}}};}}
# 825
static void*Cyc_Vcgen_initialize_var(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*vopt,void*a){
void*v;
if(vopt==0)
v=Cyc_AssnDef_fresh_var(vd->type);else{
# 830
v=vopt;}
if(vd->escapes)
a=({struct Cyc_Vcgen_Env*_Tmp0=env;void*_Tmp1=a;void*_Tmp2=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_Tmp0,_Tmp1,_Tmp2,v);});else{
# 834
a=({void*_Tmp0=Cyc_Vcgen_kill_var(env,vd,a);Cyc_AssnDef_and(_Tmp0,({void*_Tmp1=Cyc_AssnDef_var(vd);Cyc_AssnDef_eq(_Tmp1,v);}));});}
return a;}
# 838
static unsigned Cyc_Vcgen_datatype_constructor_index(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
# 840
unsigned i=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(dd->fields)->v;
for(1;fs!=0;(fs=fs->tl,++ i)){
if((struct Cyc_Absyn_Datatypefield*)fs->hd==df)return i;
if(Cyc_Absyn_qvar_cmp(df->name,((struct Cyc_Absyn_Datatypefield*)fs->hd)->name)==0)return i;}
# 846
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("vcgen: datatype_constructor_index",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));}struct _tuple16{int f0;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};
# 849
static struct _tuple16 Cyc_Vcgen_get_aggr_info(void*t){
# 851
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;int _Tmp2;enum Cyc_Absyn_AggrKind _Tmp3;union Cyc_Absyn_AggrInfo _Tmp4;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==25){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp4;
# 853
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
return({struct _tuple16 _Tmp5;_Tmp5.f0=_check_null(ad->impl)->tagged,_Tmp5.f1=ad->kind,_Tmp5.f2=ad->impl->fields;_Tmp5;});}}else{goto _LL5;}case 6: _Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{enum Cyc_Absyn_AggrKind ak=_Tmp3;int is_tuple=_Tmp2;struct Cyc_List_List*fs=_Tmp1;
# 856
return({struct _tuple16 _Tmp5;_Tmp5.f0=0,_Tmp5.f1=ak,_Tmp5.f2=fs;_Tmp5;});}default: _LL5:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("vcgen: bad type in get_aggr_info",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));};}struct _tuple17{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 861
static struct _tuple0 Cyc_Vcgen_vcgen_pat(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Pat*p,void*path,void*v,void*ain){
# 863
void*_Tmp0=p->r;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;void*_Tmp7;switch(*((int*)_Tmp0)){case 0:
 return({struct _tuple0 _Tmp8;_Tmp8.f0=ain,_Tmp8.f1=(void*)& Cyc_AssnDef_false_assn;_Tmp8;});case 1: _Tmp7=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;struct Cyc_Absyn_Pat*p2=_Tmp6;
# 866
struct _tuple0 _Tmp8=Cyc_Vcgen_vcgen_pat(env,p2,path,v,ain);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*at=_TmpA;void*af=_Tmp9;
return({struct _tuple0 _TmpB;({void*_TmpC=Cyc_Vcgen_initialize_var(env,vd,v,at);_TmpB.f0=_TmpC;}),_TmpB.f1=af;_TmpB;});}}case 2: _Tmp7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp7;struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 869
return({struct _tuple0 _Tmp8;({void*_Tmp9=Cyc_Vcgen_initialize_var(env,vd,v,ain);_Tmp8.f0=_Tmp9;}),_Tmp8.f1=(void*)& Cyc_AssnDef_false_assn;_Tmp8;});}case 3: _Tmp7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;struct Cyc_Absyn_Pat*p2=_Tmp6;
# 871
struct _tuple0 _Tmp8=Cyc_Vcgen_vcgen_pat(env,p2,path,v,ain);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*at=_TmpA;void*af=_Tmp9;
return({struct _tuple0 _TmpB;({void*_TmpC=Cyc_Vcgen_initialize_var(env,vd,path,at);_TmpB.f0=_TmpC;}),_TmpB.f1=af;_TmpB;});}}case 4: _Tmp7=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;
# 874
return({struct _tuple0 _Tmp8;({void*_Tmp9=Cyc_Vcgen_initialize_var(env,vd,v,ain);_Tmp8.f0=_Tmp9;}),_Tmp8.f1=(void*)& Cyc_AssnDef_false_assn;_Tmp8;});}case 8:  {
# 876
void*t=Cyc_AssnDef_zero();
return({struct _tuple0 _Tmp8;({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});_Tmp8;});}case 9: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
# 879
void*t=Cyc_AssnDef_uint((unsigned)i);
return({struct _tuple0 _Tmp8;({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});_Tmp8;});}case 10: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
# 882
void*t=Cyc_AssnDef_uint((unsigned)c);
return({struct _tuple0 _Tmp8;({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});_Tmp8;});}case 11: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr f=_Tmp2;int i=_Tmp4;
# 885
struct Cyc_Absyn_Exp*e=Cyc_Absyn_float_exp(f,i,0U);
({void*_Tmp8=Cyc_Absyn_gen_float_type((unsigned)i);e->topt=_Tmp8;});{
void*t=Cyc_AssnDef_cnst(e);
return({struct _tuple0 _Tmp8;({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});_Tmp8;});}}case 12: _Tmp7=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;struct Cyc_Absyn_Enumfield*ef=_Tmp6;
# 890
struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_Tmp8->tag=31,_Tmp8->f1=ed,_Tmp8->f2=ef;_Tmp8;}),0U);
e->topt=p->topt;{
void*t=Cyc_AssnDef_cnst(e);
return({struct _tuple0 _Tmp8;({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});_Tmp8;});}}case 13: _Tmp7=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{void*type=_Tmp7;struct Cyc_Absyn_Enumfield*ef=_Tmp6;
# 895
struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_Tmp8->tag=32,_Tmp8->f1=type,_Tmp8->f2=ef;_Tmp8;}),0U);
e->topt=p->topt;{
void*t=Cyc_AssnDef_cnst(e);
return({struct _tuple0 _Tmp8;({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({void*_Tmp9=({void*_TmpA=ain;Cyc_AssnDef_and(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});_Tmp8;});}}case 5: _Tmp7=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp7;
# 900
void*path2=v;
void*v2=({void*_Tmp8=Cyc_AssnDef_var(Cyc_AssnDef_memory);void*_Tmp9=v;Cyc_AssnDef_select(_Tmp8,_Tmp9,p->topt);});
struct _tuple0 _Tmp8=Cyc_Vcgen_vcgen_pat(env,p2,path2,v2,ain);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*at=_TmpA;void*af=_Tmp9;
return({struct _tuple0 _TmpB;({void*_TmpC=({void*_TmpD=({void*_TmpE=at;Cyc_AssnDef_and(_TmpE,({void*_TmpF=Cyc_AssnDef_zero();Cyc_AssnDef_ult(_TmpF,Cyc_AssnDef_okderef(v));}));});Cyc_AssnDef_and(_TmpD,({void*_TmpE=Cyc_AssnDef_zero();Cyc_AssnDef_neq(_TmpE,v);}));});_TmpB.f0=_TmpC;}),_TmpB.f1=af;_TmpB;});}}case 6: _Tmp7=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{void*topt=_Tmp7;struct Cyc_List_List*dlps=_Tmp6;
# 905
void*type=_check_null(topt);
struct _tuple16 _Tmp8=Cyc_Vcgen_get_aggr_info(type);void*_Tmp9;int _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f2;{int is_tagged=_TmpA;struct Cyc_List_List*fields=_Tmp9;
void*at=ain;
void*af=(void*)& Cyc_AssnDef_false_assn;
for(1;dlps!=0;dlps=dlps->tl){
struct _tuple17*tup=(struct _tuple17*)dlps->hd;
struct Cyc_Absyn_Pat*p2=tup->f1;
if(p2->r==(void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(tup->f0);
int i=Cyc_CfFlowInfo_get_field_index(type,f);
if(is_tagged){
# 918
void*tag_check=({void*_TmpB=Cyc_AssnDef_tagof(v);Cyc_AssnDef_eq(_TmpB,Cyc_AssnDef_uint((unsigned)i));});
at=Cyc_AssnDef_and(at,tag_check);
af=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_not(tag_check));});}{
# 922
void*member=Cyc_AssnDef_proj(v,(unsigned)i,p2->topt);
# 930
struct _tuple0 _TmpB=Cyc_Vcgen_vcgen_pat(env,p2,0,member,at);void*_TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*at2=_TmpD;void*af2=_TmpC;
at=at2;
af=Cyc_AssnDef_or(af,af2);}}}}
# 934
return({struct _tuple0 _TmpB;_TmpB.f0=at,_TmpB.f1=af;_TmpB;});}}case 7: _Tmp7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;struct Cyc_Absyn_Datatypefield*df=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 938
int i=(int)Cyc_Vcgen_datatype_constructor_index(dd,df);
void*tag_check=({void*_Tmp8=Cyc_AssnDef_tagof(v);Cyc_AssnDef_eq(_Tmp8,Cyc_AssnDef_uint((unsigned)i));});
void*at=Cyc_AssnDef_and(ain,tag_check);
void*af=({void*_Tmp8=ain;Cyc_AssnDef_and(_Tmp8,Cyc_AssnDef_not(tag_check));});
{unsigned j=0U;for(0;ps!=0;(ps=ps->tl,++ j)){
struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)ps->hd;
void*member=Cyc_AssnDef_proj(v,(unsigned)i,p2->topt);
# 947
struct _tuple0 _Tmp8=Cyc_Vcgen_vcgen_pat(env,p2,0,member,at);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*at2=_TmpA;void*af2=_Tmp9;
at=at2;
af=Cyc_AssnDef_or(af,af2);}}}
# 951
return({struct _tuple0 _Tmp8;_Tmp8.f0=at,_Tmp8.f1=af;_Tmp8;});}case 14:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("vcgen pat found bad pattern",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));};}
# 959
static void*Cyc_Vcgen_simplify_constants(void*a){
while(1){
void*_Tmp0;unsigned _Tmp1;void*_Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;if(*((int*)a)==3){if(*((int*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1)==3){if(*((int*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1)->f2)==2){if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1)->f2)->f1)==0){if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1)->f2)->f2==Cyc_AssnDef_Neq){if(*((int*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2)==2){if(*((int*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2)->f1)==0){if(((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2)->f2==Cyc_AssnDef_Eq){_Tmp5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1)->f1;_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1)->f2)->f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1)->f2)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2)->f1)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2)->f3;if(
(void*)_Tmp3==(void*)_Tmp0 &&(unsigned)_Tmp4!=(unsigned)_Tmp1){void*a2=_Tmp5;unsigned i=_Tmp4;void*x=_Tmp3;void*p=_Tmp2;unsigned j=_Tmp1;void*y=_Tmp0;
a=Cyc_AssnDef_and(a2,p);continue;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return a;};}}
# 1176 "vcgen.cyc"
static void*Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,void*v,struct Cyc_List_List*scs,void*ain){
# 1181
void*res=(void*)& Cyc_AssnDef_false_assn;
for(1;scs!=0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*sc=(struct Cyc_Absyn_Switch_clause*)scs->hd;
struct _tuple0 _Tmp0=Cyc_Vcgen_vcgen_pat(env,sc->pattern,0,v,ain);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*at=_Tmp2;void*af=_Tmp1;
if(sc->where_clause!=0){
struct _tuple0 _Tmp3=Cyc_Vcgen_vcgen_test(env,_check_null(sc->where_clause),at);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{void*at2=_Tmp5;void*af2=_Tmp4;
at=at2;
af=Cyc_AssnDef_or(af,af2);}}
# 1190
at=Cyc_Vcgen_simplify_constants(at);
res=({void*_Tmp3=res;Cyc_AssnDef_or(_Tmp3,Cyc_Vcgen_vcgen_stmt_nodebug(env,sc->body,at));});
if(!env->shared->widen_paths)
ain=af;}}
# 1195
return res;}
# 1198
static void*Cyc_Vcgen_add_vardecl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,void*a){
# 1201
void*tv;
if(vd->escapes)
tv=Cyc_AssnDef_addr(vd);else{
# 1205
tv=Cyc_AssnDef_var(vd);
a=Cyc_Vcgen_kill_var(env,vd,a);}
# 1208
if(vd->initializer!=0){
struct _tuple0 _Tmp0=Cyc_Vcgen_vcgen_rexp_nodebug(env,_check_null(vd->initializer),a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*t=_Tmp2;void*a1=_Tmp1;
a=Cyc_Vcgen_do_assign(env,a1,tv,t);}}
# 1212
return a;}
# 1215
static void*Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,void*a){
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
return Cyc_Vcgen_add_vardecl(env,vd,a);}case 2: _Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Pat*p=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;void*dtree=_Tmp1;
# 1219
struct _tuple0 _Tmp4=Cyc_Vcgen_vcgen_rexp_always(env,e,a);void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{void*v=_Tmp6;void*a=_Tmp5;
struct _tuple0 _Tmp7=Cyc_Vcgen_vcgen_pat(env,p,0,v,a);void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*at=_Tmp9;void*af=_Tmp8;
if(af!=(void*)& Cyc_AssnDef_false_assn)Cyc_Vcgen_update_try_assn(env,af);
return at;}}}case 3: _Tmp3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp3;
# 1224
for(1;vds!=0;vds=vds->tl){
a=Cyc_Vcgen_add_vardecl(env,(struct Cyc_Absyn_Vardecl*)vds->hd,a);}
return a;}case 4: _Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*eopt=_Tmp2;
# 1228
if(eopt!=0){
struct _tuple0 _Tmp4=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,a);void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{void*t=_Tmp6;void*a1=_Tmp5;
a=a1;}}
# 1233
a=Cyc_Vcgen_add_vardecl(env,vd,a);
return a;}default:
 return a;};}
# 1241
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});if(*((int*)_Tmp0.f0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1==Cyc_Absyn_Unsigned)
return 1;else{if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1==Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1==Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1==Cyc_Absyn_Unsigned){_LL3:
 return 1;}else{if(*((int*)_Tmp0.f0)==3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_Tmp0.f0)==3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}}};}
# 1257
static struct _tuple0 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_Tmp0=e->r;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 1260
struct _tuple0 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*a1t=_Tmp7;void*a1f=_Tmp6;
struct _tuple0 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1t);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*a2t=_TmpA;void*a2f=_Tmp9;
struct _tuple0 _TmpB=Cyc_Vcgen_vcgen_test(env,e3,a1f);void*_TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*a3t=_TmpD;void*a3f=_TmpC;
return({struct _tuple0 _TmpE;({void*_TmpF=Cyc_AssnDef_or(a2t,a3t);_TmpE.f0=_TmpF;}),({void*_TmpF=Cyc_AssnDef_or(a2f,a3f);_TmpE.f1=_TmpF;});_TmpE;});}}}}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1265
struct _tuple0 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*a1t=_Tmp7;void*a1f=_Tmp6;
struct _tuple0 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1t);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*a2t=_TmpA;void*a2f=_Tmp9;
return({struct _tuple0 _TmpB;_TmpB.f0=a2t,({void*_TmpC=Cyc_AssnDef_or(a1f,a2f);_TmpB.f1=_TmpC;});_TmpB;});}}}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1269
struct _tuple0 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*a1t=_Tmp7;void*a1f=_Tmp6;
struct _tuple0 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1f);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*a2t=_TmpA;void*a2f=_Tmp9;
return({struct _tuple0 _TmpB;({void*_TmpC=Cyc_AssnDef_or(a1t,a2t);_TmpB.f0=_TmpC;}),_TmpB.f1=a2f;_TmpB;});}}}case 9: _Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1273
struct _tuple0 _Tmp5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_Tmp6;_Tmp6=_Tmp5.f1;{void*ain=_Tmp6;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){_Tmp4=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;{struct Cyc_Absyn_Exp*e1=_Tmp4;
# 1276
struct _tuple0 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*at=_Tmp7;void*af=_Tmp6;
return({struct _tuple0 _Tmp8;_Tmp8.f0=af,_Tmp8.f1=at;_Tmp8;});}}}else{goto _LLD;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0){_Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;_Tmp3=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->tl->hd;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1279
struct _tuple0 _Tmp5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*t1=_Tmp7;void*a1=_Tmp6;
struct _tuple0 _Tmp8=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a1);void*_Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*t2=_TmpA;void*a2=_Tmp9;
void*at=a2;
void*af=a2;
switch((int)p){case Cyc_Absyn_Eq:
# 1285
 at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_eq(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_neq(t1,t2));});goto _LL30;case Cyc_Absyn_Neq:
# 1287
 at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_neq(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_eq(t1,t2));});goto _LL30;case Cyc_Absyn_Lt:
# 1289
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ult(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ulte(t2,t1));});}else{
# 1292
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slt(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slte(t2,t1));});}
# 1294
goto _LL30;case Cyc_Absyn_Lte:
# 1296
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ulte(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ult(t2,t1));});}else{
# 1299
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slte(t1,t2));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slt(t2,t1));});}
# 1301
goto _LL30;case Cyc_Absyn_Gt:
# 1303
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ult(t2,t1));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ulte(t1,t2));});}else{
# 1306
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slt(t2,t1));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slte(t1,t2));});}
# 1308
goto _LL30;case Cyc_Absyn_Gte:
# 1310
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ulte(t2,t1));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_ult(t1,t2));});}else{
# 1313
at=({void*_TmpB=at;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slte(t2,t1));});af=({void*_TmpB=af;Cyc_AssnDef_and(_TmpB,Cyc_AssnDef_slt(t1,t2));});}
# 1315
goto _LL30;default:
 goto _LL30;}_LL30:;
# 1318
return({struct _tuple0 _TmpB;_TmpB.f0=at,_TmpB.f1=af;_TmpB;});}}}}else{goto _LLD;}}else{goto _LLD;}}else{goto _LLD;}}default: _LLD: {
# 1320
struct _tuple0 _Tmp5=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*t=_Tmp7;void*aout=_Tmp6;
return({struct _tuple0 _Tmp8;({void*_Tmp9=({void*_TmpA=aout;Cyc_AssnDef_and(_TmpA,({void*_TmpB=Cyc_AssnDef_one();Cyc_AssnDef_ulte(_TmpB,t);}));});_Tmp8.f0=_Tmp9;}),({void*_Tmp9=({void*_TmpA=aout;Cyc_AssnDef_and(_TmpA,({void*_TmpB=t;Cyc_AssnDef_eq(_TmpB,Cyc_AssnDef_zero());}));});_Tmp8.f1=_Tmp9;});_Tmp8;});}}};}
# 1325
static void*Cyc_Vcgen_vcgen_rexps_nosave(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
for(1;es!=0;es=es->tl){
a=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a).f1;}
return a;}struct _tuple18{struct Cyc_List_List**f0;void*f1;};
# 1331
static struct _tuple18 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*a){
struct Cyc_List_List*res=0;
int valid=1;
for(1;es!=0;es=es->tl){
struct _tuple0 _Tmp0=Cyc_Vcgen_vcgen_rexp_nodebug(env,(struct Cyc_Absyn_Exp*)es->hd,a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*t=_Tmp2;void*aout=_Tmp1;
a=aout;
if(valid && t!=0)
res=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=t,_Tmp3->tl=res;_Tmp3;});else{
# 1340
valid=0;}}}
# 1343
return({struct _tuple18 _Tmp0;valid?({struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_cycalloc(sizeof(struct Cyc_List_List*));({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(res);*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0.f0=_Tmp1;}):(_Tmp0.f0=0),_Tmp0.f1=a;_Tmp0;});}
# 1346
static struct _tuple0 Cyc_Vcgen_vcgen_rexp_always(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
struct _tuple0 _Tmp0=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*topt=_Tmp2;void*a=_Tmp1;
return({struct _tuple0 _Tmp3;topt==0?({void*_Tmp4=Cyc_AssnDef_fresh_var(e->topt);_Tmp3.f0=_Tmp4;}):(_Tmp3.f0=topt),_Tmp3.f1=a;_Tmp3;});}}
# 1351
static struct _tuple15 Cyc_Vcgen_vcgen_rexps_always(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,void*ain){
struct Cyc_List_List*res=0;
for(1;es!=0;es=es->tl){
struct _tuple0 _Tmp0=Cyc_Vcgen_vcgen_rexp_always(env,(struct Cyc_Absyn_Exp*)es->hd,ain);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*t=_Tmp2;void*a=_Tmp1;
ain=a;
res=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=t,_Tmp3->tl=res;_Tmp3;});}}
# 1358
return({struct _tuple15 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res);_Tmp0.f0=_Tmp1;}),_Tmp0.f1=ain;_Tmp0;});}
# 1361
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_Tmp0;switch(*((int*)b)){case 1: _Tmp0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}case 2: _Tmp0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp0;
return fd->fn_vardecl;}case 0:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("vcgen bind2vardecl",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp0=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}case 4: _Tmp0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}default: _Tmp0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}};}
# 1372
static struct _tuple0 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp1;
# 1375
struct Cyc_Absyn_Vardecl*vdopt=Cyc_Vcgen_bind2vardecl(b);
if(vdopt==0)return({struct _tuple0 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=ain;_Tmp2;});{
struct Cyc_Absyn_Vardecl*vd=vdopt;
if(vd->escapes)
return({struct _tuple0 _Tmp2;({void*_Tmp3=Cyc_AssnDef_addr(vd);_Tmp2.f0=_Tmp3;}),_Tmp2.f1=ain;_Tmp2;});else{
# 1381
return({struct _tuple0 _Tmp2;({void*_Tmp3=Cyc_AssnDef_var(vd);_Tmp2.f0=_Tmp3;}),_Tmp2.f1=ain;_Tmp2;});}}}}else{
# 1383
return({struct _tuple0 _Tmp2;_Tmp2.f0=0,({void*_Tmp3=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f1;_Tmp2.f1=_Tmp3;});_Tmp2;});};}struct _tuple19{struct Cyc_List_List*f0;struct Cyc_List_List*f1;void*f2;void*f3;void*f4;struct Cyc_Absyn_Vardecl*f5;};
# 1389
static struct _tuple19 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
return({struct _tuple19 _Tmp0;_Tmp0.f0=fi->args,_Tmp0.f1=fi->arg_vardecls,_Tmp0.f2=fi->requires_assn,_Tmp0.f3=fi->ensures_assn,_Tmp0.f4=fi->throws_assn,_Tmp0.f5=fi->return_value;_Tmp0;});}
# 1394
static struct _tuple19 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt!=0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_Tmp0;if(*((int*)t)==5){_Tmp0=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_Tmp0;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{
goto _LL0;}_LL0:;}
# 1402
return({struct _tuple19 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=0,_Tmp0.f2=0,_Tmp0.f3=0,_Tmp0.f4=0,_Tmp0.f5=0;_Tmp0;});}
# 1405
static void Cyc_Vcgen_check_requires(unsigned loc,struct Cyc_Vcgen_Env*env,struct Cyc_List_List*inputs,struct Cyc_List_List*argvds,void*a,void*reqopt){
# 1409
if(reqopt!=0){
void*req=reqopt;
# 1413
for(1;argvds!=0 && inputs!=0;(argvds=argvds->tl,inputs=inputs->tl)){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)argvds->hd;
if(vd!=0)
a=({void*_Tmp0=a;Cyc_AssnDef_and(_Tmp0,({void*_Tmp1=Cyc_AssnDef_var(vd);Cyc_AssnDef_eq(_Tmp1,(void*)inputs->hd);}));});}
# 1419
if(!Cyc_Vcgen_implies(& a,& req))
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(req)));_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(a)));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err(loc,_tag_fat("cannot prove @requires %s from %s",sizeof(char),34U),_tag_fat(_Tmp2,sizeof(void*),2));});}}
# 1425
static void*Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,void*ain){
void*_Tmp0;if(lt!=0)switch(*((int*)lt)){case 2:
 return lt;case 3: _Tmp0=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)lt)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return({void*_Tmp1=Cyc_AssnDef_var(Cyc_AssnDef_memory);void*_Tmp2=lt;Cyc_AssnDef_select(_Tmp1,_Tmp2,vd->type);});}default: goto _LL5;}else{_LL5:
 return 0;};}
# 1433
static int Cyc_Vcgen_is_unsigned_int_exp(struct Cyc_Absyn_Exp*e){
void*t=e->topt;
if(t==0)return 0;{
enum Cyc_Absyn_Size_of _Tmp0;if(t!=0){if(*((int*)t)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1==Cyc_Absyn_Unsigned){_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Size_of sz=_Tmp0;
# 1439
if((int)sz==3 ||(int)sz==2)
return 1;else{
return 0;}}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}}struct _tuple20{int f0;void*f1;};
# 1446
static int Cyc_Vcgen_cmp_index(struct _tuple20*x,struct _tuple20*y){
return x->f0 - y->f0;}struct _tuple21{void*f0;void*f1;void*f2;};
# 1451
static struct _tuple21 Cyc_Vcgen_vcgen_deref(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*root,struct Cyc_Absyn_Exp*index,void*ain){
# 1453
struct _tuple0 _Tmp0=Cyc_Vcgen_vcgen_rexp_always(env,root,ain);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*t1=_Tmp2;void*a1=_Tmp1;
struct _tuple0 _Tmp3=Cyc_Vcgen_vcgen_rexp_nodebug(env,index,a1);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{void*t2=_Tmp5;void*a2=_Tmp4;
void*term_numelts;void*term_typebound;
# 1458
if(t2==0){
Cyc_Vcgen_update_try_assn(env,a2);
return({struct _tuple21 _Tmp6;_Tmp6.f0=t1,_Tmp6.f1=t2,_Tmp6.f2=a2;_Tmp6;});}{
# 1462
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(_check_null(root->topt));
void*bnds_check;
# 1465
if(eopt==0){
# 1467
term_numelts=Cyc_AssnDef_numelts_term(t1);
bnds_check=({void*_Tmp6=Cyc_AssnDef_zero();Cyc_AssnDef_ult(_Tmp6,Cyc_AssnDef_okderef(Cyc_AssnDef_plus(t1,t2,0)));});}else{
# 1471
struct Cyc_Absyn_Exp*e=eopt;
term_numelts=Cyc_Vcgen_vcgen_rexp_always(env,e,a2).f0;
bnds_check=Cyc_AssnDef_ult(t2,term_numelts);}{
# 1475
void*bnds_check_sufficient=Cyc_AssnDef_ult(t2,term_numelts);
# 1478
void*null_check=({void*_Tmp6=Cyc_AssnDef_zero();Cyc_AssnDef_neq(_Tmp6,t1);});
# 1481
({struct Cyc_Vcgen_Env*_Tmp6=env;Cyc_Vcgen_update_try_assn(_Tmp6,({void*_Tmp7=a2;Cyc_AssnDef_and(_Tmp7,({void*_Tmp8=Cyc_AssnDef_not(bnds_check);Cyc_AssnDef_or(_Tmp8,Cyc_AssnDef_not(null_check));}));}));});{
# 1483
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks)),exp_checks->ctxt=a2,({void*_Tmp6=Cyc_AssnDef_or(bnds_check,bnds_check_sufficient);exp_checks->bounds_check=_Tmp6;}),exp_checks->null_check=null_check;
# 1485
Cyc_Vcgen_insert_exp_checks(env->assn_info,root,exp_checks);
# 1487
a2=({void*_Tmp6=Cyc_AssnDef_and(a2,bnds_check);Cyc_AssnDef_and(_Tmp6,null_check);});
return({struct _tuple21 _Tmp6;_Tmp6.f0=t1,_Tmp6.f1=t2,_Tmp6.f2=a2;_Tmp6;});}}}}}}struct _tuple22{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple23{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 1491
static struct _tuple0 Cyc_Vcgen_vcgen_rexp_nodebug(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*ain){
if(Cyc_Tcutil_is_const_exp(e)){
# 1494
{void*_Tmp0=e->r;struct _fat_ptr _Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp1;
# 1496
if(Cyc_strcmp(s,_tag_fat("print_assn",sizeof(char),11U))==0){
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)((struct _fat_ptr)seg_str);_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_fprintf(Cyc_stderr,_tag_fat("%s: current assn is %s\n",sizeof(char),24U),_tag_fat(_Tmp4,sizeof(void*),2));});}
# 1500
if(Cyc_strcmp(s,_tag_fat("all_paths",sizeof(char),10U))==0)
env->shared->widen_paths=0;
# 1503
if(Cyc_strcmp(s,_tag_fat("one_path",sizeof(char),9U))==0)
env->shared->widen_paths=1;
# 1506
return({struct _tuple0 _Tmp2;({void*_Tmp3=Cyc_AssnDef_zero();_Tmp2.f0=_Tmp3;}),_Tmp2.f1=ain;_Tmp2;});}}else{
goto _LL0;}_LL0:;}{
# 1509
struct _tuple13 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{unsigned cn=_Tmp2;int known=_Tmp1;
if(known)return({struct _tuple0 _Tmp3;({void*_Tmp4=Cyc_AssnDef_uint(cn);_Tmp3.f0=_Tmp4;}),_Tmp3.f1=ain;_Tmp3;});}}}{
# 1512
void*_Tmp0=e->r;struct _fat_ptr _Tmp1;struct Cyc_Absyn_MallocInfo _Tmp2;int _Tmp3;int _Tmp4;enum Cyc_Absyn_Coercion _Tmp5;enum Cyc_Absyn_Incrementor _Tmp6;void*_Tmp7;void*_Tmp8;enum Cyc_Absyn_Primop _Tmp9;void*_TmpA;union Cyc_Absyn_Cnst _TmpB;switch(*((int*)_Tmp0)){case 0: _TmpB=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_TmpB;
# 1515
return({struct _tuple0 _TmpC;({void*_TmpD=Cyc_AssnDef_cnst(e);_TmpC.f0=_TmpD;}),_TmpC.f1=ain;_TmpC;});}case 1: _TmpA=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_TmpA;
# 1517
struct Cyc_Absyn_Vardecl*vdopt=Cyc_Vcgen_bind2vardecl(b);
if(vdopt==0)return({struct _tuple0 _TmpC;_TmpC.f0=0,_TmpC.f1=ain;_TmpC;});{
struct Cyc_Absyn_Vardecl*vd=vdopt;
if(vd->escapes)
return({struct _tuple0 _TmpC;({void*_TmpD=({void*_TmpE=Cyc_AssnDef_var(Cyc_AssnDef_memory);void*_TmpF=Cyc_AssnDef_addr(vd);Cyc_AssnDef_select(_TmpE,_TmpF,vd->type);});_TmpC.f0=_TmpD;}),_TmpC.f1=ain;_TmpC;});else{
# 1523
return({struct _tuple0 _TmpC;({void*_TmpD=Cyc_AssnDef_var(vd);_TmpC.f0=_TmpD;}),_TmpC.f1=ain;_TmpC;});}}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl==0){_Tmp9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;{enum Cyc_Absyn_Primop p=_Tmp9;struct Cyc_Absyn_Exp*e1=_TmpA;
# 1525
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*t=_TmpE;void*aout=_TmpD;
return({struct _tuple0 _TmpF;({void*_Tmp10=Cyc_AssnDef_unop(p,t,e->topt);_TmpF.f0=_Tmp10;}),_TmpF.f1=aout;_TmpF;});}}}else{_Tmp9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_TmpA=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;_Tmp8=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->tl->hd;{enum Cyc_Absyn_Primop p=_Tmp9;struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp8;
# 1528
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*t1=_TmpE;void*aout1=_TmpD;
struct _tuple0 _TmpF=Cyc_Vcgen_vcgen_rexp_always(env,e2,aout1);void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*t2=_Tmp11;void*aout=_Tmp10;
void*t=Cyc_AssnDef_binop(p,t1,t2,e->topt);
# 1533
if((int)p==4 && Cyc_Vcgen_is_unsigned_int_exp(e))
# 1535
aout=({void*_Tmp12=aout;Cyc_AssnDef_and(_Tmp12,Cyc_AssnDef_ult(t1,t2));});
# 1537
return({struct _tuple0 _Tmp12;_Tmp12.f0=t,_Tmp12.f1=aout;_Tmp12;});}}}}}else{
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("vcgen: bad primop",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));}case 4: _TmpA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Core_Opt*popt=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1540
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*t2=_TmpE;void*a1=_TmpD;
struct _tuple0 _TmpF=Cyc_Vcgen_vcgen_lexp(env,e1,a1);void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*lt=_Tmp11;void*a2=_Tmp10;
if(popt!=0){
void*t1=Cyc_Vcgen_deref_lterm(env,lt,a2);
if(t1==0 || t2==0)
t2=0;else{
# 1547
t2=Cyc_AssnDef_binop((enum Cyc_Absyn_Primop)popt->v,t1,t2,e->topt);}}{
# 1549
void*a=Cyc_Vcgen_do_assign(env,a2,lt,t2);
return({struct _tuple0 _Tmp12;_Tmp12.f0=lt,_Tmp12.f1=a;_Tmp12;});}}}}case 5: _TmpA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_TmpA;enum Cyc_Absyn_Incrementor i=_Tmp6;
# 1552
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_lexp(env,e,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*lt=_TmpE;void*a1=_TmpD;
void*t1=Cyc_Vcgen_deref_lterm(env,lt,a1);
void*res=0;
void*assn=0;
switch((int)i){case Cyc_Absyn_PreInc:
# 1558
 if(t1!=0)assn=({void*_TmpF=t1;void*_Tmp10=Cyc_AssnDef_one();Cyc_AssnDef_plus(_TmpF,_Tmp10,e->topt);});
res=assn;
goto _LL73;case Cyc_Absyn_PostInc:
# 1562
 res=t1;
if(t1!=0)assn=({void*_TmpF=t1;void*_Tmp10=Cyc_AssnDef_one();Cyc_AssnDef_plus(_TmpF,_Tmp10,e->topt);});
goto _LL73;case Cyc_Absyn_PreDec:
# 1566
 if(t1!=0)assn=({void*_TmpF=t1;void*_Tmp10=Cyc_AssnDef_one();Cyc_AssnDef_minus(_TmpF,_Tmp10,e->topt);});
res=assn;
goto _LL73;case Cyc_Absyn_PostDec:
# 1570
 res=t1;
if(t1!=0)assn=({void*_TmpF=t1;void*_Tmp10=Cyc_AssnDef_one();Cyc_AssnDef_minus(_TmpF,_Tmp10,e->topt);});
goto _LL73;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("vcgen vcgen_rexp increment_e",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}_LL73:;{
# 1575
void*a=Cyc_Vcgen_do_assign(env,a1,lt,assn);
return({struct _tuple0 _TmpF;_TmpF.f0=res,_TmpF.f1=a;_TmpF;});}}}case 6: _TmpA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp8;struct Cyc_Absyn_Exp*e3=_Tmp7;
# 1578
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*at=_TmpE;void*af=_TmpD;
struct _tuple0 _TmpF=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*t1=_Tmp11;void*at=_Tmp10;
struct _tuple0 _Tmp12=Cyc_Vcgen_vcgen_rexp_nodebug(env,e3,af);void*_Tmp13;void*_Tmp14;_Tmp14=_Tmp12.f0;_Tmp13=_Tmp12.f1;{void*t2=_Tmp14;void*af=_Tmp13;
if(t1!=0 && t2!=0){
void*v=Cyc_AssnDef_fresh_var(e->topt);
at=({void*_Tmp15=at;Cyc_AssnDef_and(_Tmp15,Cyc_AssnDef_eq(v,t1));});
af=({void*_Tmp15=af;Cyc_AssnDef_and(_Tmp15,Cyc_AssnDef_eq(v,t2));});
return({struct _tuple0 _Tmp15;_Tmp15.f0=v,({void*_Tmp16=({struct Cyc_Vcgen_Env*_Tmp17=env;Cyc_Vcgen_may_widen(_Tmp17,Cyc_AssnDef_or(at,af));});_Tmp15.f1=_Tmp16;});_Tmp15;});}else{
# 1587
return({struct _tuple0 _Tmp15;_Tmp15.f0=0,({void*_Tmp16=({struct Cyc_Vcgen_Env*_Tmp17=env;Cyc_Vcgen_may_widen(_Tmp17,Cyc_AssnDef_or(at,af));});_Tmp15.f1=_Tmp16;});_Tmp15;});}}}}}case 7: _TmpA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp8;
# 1589
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*at=_TmpE;void*af=_TmpD;
struct _tuple0 _TmpF=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*t=_Tmp11;void*at=_Tmp10;
void*v=Cyc_AssnDef_fresh_var(e->topt);
at=({void*_Tmp12=at;Cyc_AssnDef_and(_Tmp12,Cyc_AssnDef_eq(v,t));});
af=({void*_Tmp12=af;Cyc_AssnDef_and(_Tmp12,({void*_Tmp13=v;Cyc_AssnDef_eq(_Tmp13,Cyc_AssnDef_zero());}));});
return({struct _tuple0 _Tmp12;_Tmp12.f0=v,({void*_Tmp13=({struct Cyc_Vcgen_Env*_Tmp14=env;Cyc_Vcgen_may_widen(_Tmp14,Cyc_AssnDef_or(at,af));});_Tmp12.f1=_Tmp13;});_Tmp12;});}}}case 8: _TmpA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp8;
# 1596
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_test(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*at=_TmpE;void*af=_TmpD;
struct _tuple0 _TmpF=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,af);void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*t=_Tmp11;void*af=_Tmp10;
void*v=Cyc_AssnDef_fresh_var(e->topt);
at=({void*_Tmp12=at;Cyc_AssnDef_and(_Tmp12,({void*_Tmp13=v;Cyc_AssnDef_eq(_Tmp13,Cyc_AssnDef_one());}));});
af=({void*_Tmp12=af;Cyc_AssnDef_and(_Tmp12,Cyc_AssnDef_eq(v,t));});
return({struct _tuple0 _Tmp12;_Tmp12.f0=v,({void*_Tmp13=({struct Cyc_Vcgen_Env*_Tmp14=env;Cyc_Vcgen_may_widen(_Tmp14,Cyc_AssnDef_or(at,af));});_Tmp12.f1=_Tmp13;});_Tmp12;});}}}case 9: _TmpA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp8;
# 1603
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_TmpD;_TmpD=_TmpC.f1;{void*a=_TmpD;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,a);}}case 10: _TmpA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_List_List*es=_Tmp8;
# 1606
struct _tuple15 _TmpC=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{struct Cyc_List_List*ts=_TmpE;void*a=_TmpD;
struct _tuple0 _TmpF=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,a);void*_Tmp10;_Tmp10=_TmpF.f1;{void*a2=_Tmp10;
# 1609
struct _tuple19 _Tmp11=Cyc_Vcgen_get_requires_and_ensures(e1->topt);void*_Tmp12;void*_Tmp13;void*_Tmp14;void*_Tmp15;void*_Tmp16;void*_Tmp17;_Tmp17=_Tmp11.f0;_Tmp16=_Tmp11.f1;_Tmp15=_Tmp11.f2;_Tmp14=_Tmp11.f3;_Tmp13=_Tmp11.f4;_Tmp12=_Tmp11.f5;{struct Cyc_List_List*args=_Tmp17;struct Cyc_List_List*argvds=_Tmp16;void*reqopt=_Tmp15;void*ensopt=_Tmp14;void*thrwsopt=_Tmp13;struct Cyc_Absyn_Vardecl*ret_value=_Tmp12;
Cyc_Vcgen_check_requires(e->loc,env,ts,argvds,a2,reqopt);
# 1613
a2=({struct Cyc_Absyn_Vardecl*_Tmp18=Cyc_AssnDef_memory;void*_Tmp19=Cyc_Vcgen_fresh_nonescvar(env,Cyc_AssnDef_memory);Cyc_AssnDef_subst(_Tmp18,_Tmp19,a2);});
# 1615
if(thrwsopt!=0){
void*thrws=thrwsopt;
# 1618
struct Cyc_List_List*ts2=ts;
{struct Cyc_List_List*avs=argvds;for(0;avs!=0 && ts2!=0;(avs=avs->tl,ts2=ts2->tl)){
struct Cyc_Absyn_Vardecl*vdopt=(struct Cyc_Absyn_Vardecl*)avs->hd;
if(vdopt!=0){
struct Cyc_Absyn_Vardecl*vd=_check_null((struct Cyc_Absyn_Vardecl*)avs->hd);
thrws=Cyc_AssnDef_subst(vd,(void*)ts2->hd,thrws);}}}
# 1626
({struct Cyc_Vcgen_Env*_Tmp18=env;Cyc_Vcgen_update_try_assn(_Tmp18,Cyc_AssnDef_and(a2,thrws));});}else{
# 1628
Cyc_Vcgen_update_try_assn(env,a2);}
# 1630
if(Cyc_Atts_is_noreturn_fn_type(_check_null(e1->topt)))a2=(void*)& Cyc_AssnDef_false_assn;
if(ensopt==0)
return({struct _tuple0 _Tmp18;_Tmp18.f0=0,_Tmp18.f1=a2;_Tmp18;});{
# 1634
void*ensures=ensopt;
# 1636
for(1;argvds!=0 && ts!=0;(argvds=argvds->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*vdopt=(struct Cyc_Absyn_Vardecl*)argvds->hd;
if(vdopt!=0){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)argvds->hd;
ensures=Cyc_AssnDef_subst(_check_null(vd),(void*)ts->hd,ensures);}}
# 1644
if(ret_value!=0){
void*v=Cyc_AssnDef_fresh_var(ret_value->type);
# 1647
ensures=Cyc_AssnDef_subst(ret_value,v,ensures);
return({struct _tuple0 _Tmp18;_Tmp18.f0=v,({void*_Tmp19=Cyc_AssnDef_and(a2,ensures);_Tmp18.f1=_Tmp19;});_Tmp18;});}
# 1650
return({struct _tuple0 _Tmp18;_Tmp18.f0=0,_Tmp18.f1=a2;_Tmp18;});}}}}}case 11: _TmpA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 1652
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);void*_TmpD;_TmpD=_TmpC.f1;{void*a=_TmpD;
Cyc_Vcgen_update_try_assn(env,a);
return({struct _tuple0 _TmpE;_TmpE.f0=0,_TmpE.f1=(void*)& Cyc_AssnDef_false_assn;_TmpE;});}}case 12: _TmpA=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 13: _TmpA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 14: _TmpA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*tp=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp8;enum Cyc_Absyn_Coercion c=_Tmp5;
# 1660
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*topt=_TmpE;void*a=_TmpD;
# 1667
{struct _tuple0 _TmpF=({struct _tuple0 _Tmp10;({void*_Tmp11=Cyc_Absyn_compress(tp);_Tmp10.f0=_Tmp11;}),({void*_Tmp11=Cyc_Absyn_compress(_check_null(e->topt));_Tmp10.f1=_Tmp11;});_Tmp10;});struct Cyc_Absyn_PtrInfo _Tmp10;struct Cyc_Absyn_PtrInfo _Tmp11;enum Cyc_Absyn_Size_of _Tmp12;enum Cyc_Absyn_Sign _Tmp13;enum Cyc_Absyn_Size_of _Tmp14;enum Cyc_Absyn_Sign _Tmp15;switch(*((int*)_TmpF.f0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f0)->f1)==1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f0)->f1)->f2){case Cyc_Absyn_Int_sz: if(*((int*)_TmpF.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f1)->f1)){case 5:
# 1671
 return({struct _tuple0 _Tmp16;_Tmp16.f0=topt,_Tmp16.f1=a;_Tmp16;});case 1: goto _LLAA;default: goto _LLAE;}else{goto _LLAE;}case Cyc_Absyn_Long_sz: if(*((int*)_TmpF.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f1)->f1)){case 5:
# 1673
 return({struct _tuple0 _Tmp16;_Tmp16.f0=topt,_Tmp16.f1=a;_Tmp16;});case 1: goto _LLAA;default: goto _LLAE;}else{goto _LLAE;}default: if(*((int*)_TmpF.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f1)->f1)==1){_LLAA: _Tmp15=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f0)->f1)->f1;_Tmp14=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f0)->f1)->f2;_Tmp13=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f1)->f1)->f1;_Tmp12=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpF.f1)->f1)->f2;{enum Cyc_Absyn_Sign u1=_Tmp15;enum Cyc_Absyn_Size_of s1=_Tmp14;enum Cyc_Absyn_Sign u2=_Tmp13;enum Cyc_Absyn_Size_of s2=_Tmp12;
# 1679
s1=(int)s1==3?2U: s1;
s2=(int)s2==3?2U: s2;
if((int)s1==(int)s2)return({struct _tuple0 _Tmp16;_Tmp16.f0=topt,_Tmp16.f1=a;_Tmp16;});
goto _LLA5;}}else{goto _LLAE;}}else{goto _LLAE;}}else{goto _LLAE;}case 3: if(*((int*)_TmpF.f1)==3){_Tmp11=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpF.f0)->f1;_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpF.f1)->f1;{struct Cyc_Absyn_PtrInfo p2=_Tmp11;struct Cyc_Absyn_PtrInfo p1=_Tmp10;
# 1684
int n1=Cyc_Tcutil_force_type2bool(0,p1.ptr_atts.nullable);
int n2=Cyc_Tcutil_force_type2bool(0,p2.ptr_atts.nullable);
struct Cyc_Absyn_Exp*b1=({void*_Tmp16=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp16,p1.ptr_atts.bounds);});
struct Cyc_Absyn_Exp*b2=({void*_Tmp16=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp16,p2.ptr_atts.bounds);});
struct _tuple22 _Tmp16=({struct _tuple22 _Tmp17;_Tmp17.f0=b1,_Tmp17.f1=b2;_Tmp17;});if(_Tmp16.f0!=0){if(_Tmp16.f1!=0){
# 1693
if(n1 && !n2){
if(topt!=0){
void*term=topt;
void*null_check=({void*_Tmp17=Cyc_AssnDef_zero();Cyc_AssnDef_neq(_Tmp17,term);});
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks)),exp_checks->ctxt=a,exp_checks->bounds_check=(void*)& Cyc_AssnDef_true_assn,exp_checks->null_check=null_check;
# 1699
({struct Cyc_Vcgen_Env*_Tmp17=env;Cyc_Vcgen_update_try_assn(_Tmp17,({void*_Tmp18=a;Cyc_AssnDef_and(_Tmp18,({void*_Tmp19=term;Cyc_AssnDef_eq(_Tmp19,Cyc_AssnDef_zero());}));}));});
return({struct _tuple0 _Tmp17;_Tmp17.f0=term,({void*_Tmp18=Cyc_AssnDef_and(a,null_check);_Tmp17.f1=_Tmp18;});_Tmp17;});}else{
# 1702
Cyc_Vcgen_update_try_assn(env,a);}}
# 1705
return({struct _tuple0 _Tmp17;_Tmp17.f0=topt,_Tmp17.f1=a;_Tmp17;});}else{
# 1707
return({struct _tuple0 _Tmp17;_Tmp17.f0=topt,_Tmp17.f1=a;_Tmp17;});}}else{if(_Tmp16.f1!=0){
# 1711
if(topt!=0){
void*term=topt;
void*null_check=n2?(void*)& Cyc_AssnDef_true_assn:({void*_Tmp17=Cyc_AssnDef_zero();Cyc_AssnDef_neq(_Tmp17,term);});
struct _tuple0 _Tmp17=Cyc_Vcgen_vcgen_rexp_always(env,_check_null(b2),a);void*_Tmp18;_Tmp18=_Tmp17.f0;{void*b=_Tmp18;
void*bcheck=({void*_Tmp19=b;Cyc_AssnDef_ulte(_Tmp19,Cyc_AssnDef_numelts_term(term));});
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks)),exp_checks->ctxt=a,exp_checks->bounds_check=bcheck,exp_checks->null_check=null_check;{
# 1718
void*checks=Cyc_AssnDef_and(null_check,bcheck);
({struct Cyc_Vcgen_Env*_Tmp19=env;Cyc_Vcgen_update_try_assn(_Tmp19,({void*_Tmp1A=a;Cyc_AssnDef_and(_Tmp1A,Cyc_AssnDef_not(checks));}));});
return({struct _tuple0 _Tmp19;_Tmp19.f0=term,({void*_Tmp1A=Cyc_AssnDef_and(a,checks);_Tmp19.f1=_Tmp1A;});_Tmp19;});}}}else{
# 1722
Cyc_Vcgen_update_try_assn(env,a);}
# 1724
return({struct _tuple0 _Tmp17;_Tmp17.f0=topt,_Tmp17.f1=a;_Tmp17;});}else{
# 1726
return({struct _tuple0 _Tmp17;_Tmp17.f0=topt,_Tmp17.f1=a;_Tmp17;});}};}}else{goto _LLAE;}default: _LLAE:
# 1728
 goto _LLA5;}_LLA5:;}
# 1730
if((int)c==1){
if(topt==0)return({struct _tuple0 _TmpF;_TmpF.f0=0,_TmpF.f1=a;_TmpF;});{
void*t=topt;
return({struct _tuple0 _TmpF;({void*_Tmp10=Cyc_AssnDef_cast(tp,t);_TmpF.f0=_Tmp10;}),_TmpF.f1=a;_TmpF;});}}else{
# 1736
Cyc_Vcgen_update_try_assn(env,ain);
return({struct _tuple0 _TmpF;topt==0?_TmpF.f0=0:({void*_Tmp10=Cyc_AssnDef_cast(tp,topt);_TmpF.f0=_Tmp10;}),_TmpF.f1=a;_TmpF;});}}}case 16: _TmpA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*eopt=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*qopt=_Tmp7;
# 1742
if(eopt!=0)
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,eopt,ain).f1;
if(qopt!=0)
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,qopt,ain).f1;{
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*t=_TmpE;void*ain=_TmpD;
# 1749
void*res=Cyc_AssnDef_fresh_var(e->topt);
ain=({void*_TmpF=ain;Cyc_AssnDef_and(_TmpF,({void*_Tmp10=({void*_Tmp11=({void*_Tmp12=Cyc_AssnDef_var(Cyc_AssnDef_memory);void*_Tmp13=res;Cyc_AssnDef_select(_Tmp12,_Tmp13,e1->topt);});Cyc_AssnDef_eq(_Tmp11,t);});Cyc_AssnDef_and(_Tmp10,({
void*_Tmp11=res;Cyc_AssnDef_neq(_Tmp11,Cyc_AssnDef_zero());}));}));});
return({struct _tuple0 _TmpF;_TmpF.f0=res,_TmpF.f1=ain;_TmpF;});}}}case 31:
# 1755
 goto _LL2E;case 32: _LL2E:
 goto _LL30;case 17: _LL30:
 goto _LL32;case 18: _LL32:
 goto _LL34;case 38: _LL34:
 goto _LL36;case 19: _LL36:
 return({struct _tuple0 _TmpC;({void*_TmpD=Cyc_AssnDef_cnst(e);_TmpC.f0=_TmpD;}),_TmpC.f1=ain;_TmpC;});case 21: _TmpA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*f=_Tmp8;int is_tagged=_Tmp4;int is_read=_Tmp3;
# 1763
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*t=_TmpE;void*ain=_TmpD;
int i=Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),f);
# 1766
if(is_tagged && is_read){
# 1768
void*ck=({void*_TmpF=Cyc_AssnDef_tagof(t);Cyc_AssnDef_eq(_TmpF,Cyc_AssnDef_uint((unsigned)i));});
({struct Cyc_Vcgen_Env*_TmpF=env;Cyc_Vcgen_update_try_assn(_TmpF,({void*_Tmp10=ain;Cyc_AssnDef_and(_Tmp10,Cyc_AssnDef_not(ck));}));});
ain=Cyc_AssnDef_and(ain,ck);}
# 1772
return({struct _tuple0 _TmpF;({void*_Tmp10=Cyc_AssnDef_proj(t,(unsigned)i,e->topt);_TmpF.f0=_Tmp10;}),_TmpF.f1=ain;_TmpF;});}}case 37: _TmpA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 1774
_TmpA=e;goto _LL3C;}case 15: _TmpA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL3C: {struct Cyc_Absyn_Exp*e=_TmpA;
# 1776
return Cyc_Vcgen_vcgen_lexp(env,e,ain);}case 20: _TmpA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 1781
struct _tuple21 _TmpC=({struct Cyc_Vcgen_Env*_TmpD=env;struct Cyc_Absyn_Exp*_TmpE=e1;struct Cyc_Absyn_Exp*_TmpF=Cyc_Vcgen_zero_exp();Cyc_Vcgen_vcgen_deref(_TmpD,_TmpE,_TmpF,ain);});void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f2;{void*t1=_TmpE;void*ain=_TmpD;
if(t1==0)return({struct _tuple0 _TmpF;_TmpF.f0=0,_TmpF.f1=ain;_TmpF;});else{
return({struct _tuple0 _TmpF;({void*_Tmp10=({void*_Tmp11=Cyc_AssnDef_var(Cyc_AssnDef_memory);void*_Tmp12=t1;Cyc_AssnDef_select(_Tmp11,_Tmp12,e->topt);});_TmpF.f0=_Tmp10;}),_TmpF.f1=ain;_TmpF;});}}}case 22: _TmpA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_TmpA;struct _fat_ptr*f=_Tmp8;int is_tagged=_Tmp4;int is_read=_Tmp3;
# 1785
struct _tuple21 _TmpC=({struct Cyc_Vcgen_Env*_TmpD=env;struct Cyc_Absyn_Exp*_TmpE=e1;struct Cyc_Absyn_Exp*_TmpF=Cyc_Vcgen_zero_exp();Cyc_Vcgen_vcgen_deref(_TmpD,_TmpE,_TmpF,ain);});void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f2;{void*t1=_TmpE;void*ain=_TmpD;
int i=({void*_TmpF=Cyc_Tcutil_pointer_elt_type(_check_null(e1->topt));Cyc_CfFlowInfo_get_field_index(_TmpF,f);});
if(is_tagged && is_read){
void*ck=({void*_TmpF=Cyc_AssnDef_tagof(({void*_Tmp10=Cyc_AssnDef_var(Cyc_AssnDef_memory);Cyc_AssnDef_select(_Tmp10,(unsigned)t1?t1: Cyc_AssnDef_fresh_var(0),0);}));Cyc_AssnDef_eq(_TmpF,
Cyc_AssnDef_uint((unsigned)i));});
({struct Cyc_Vcgen_Env*_TmpF=env;Cyc_Vcgen_update_try_assn(_TmpF,({void*_Tmp10=ain;Cyc_AssnDef_and(_Tmp10,Cyc_AssnDef_not(ck));}));});
ain=Cyc_AssnDef_and(ain,ck);}{
# 1793
void*elt_type=Cyc_Tcutil_pointer_elt_type(_check_null(e1->topt));
return({struct _tuple0 _TmpF;({void*_Tmp10=({void*_Tmp11=({void*_Tmp12=Cyc_AssnDef_var(Cyc_AssnDef_memory);void*_Tmp13=_check_null(t1);Cyc_AssnDef_select(_Tmp12,_Tmp13,elt_type);});unsigned _Tmp12=(unsigned)i;Cyc_AssnDef_proj(_Tmp11,_Tmp12,e->topt);});_TmpF.f0=_Tmp10;}),_TmpF.f1=ain;_TmpF;});}}}case 23: _TmpA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp8;
# 1796
struct _tuple21 _TmpC=Cyc_Vcgen_vcgen_deref(env,e1,e2,ain);void*_TmpD;void*_TmpE;void*_TmpF;_TmpF=_TmpC.f0;_TmpE=_TmpC.f1;_TmpD=_TmpC.f2;{void*t1=_TmpF;void*t2=_TmpE;void*ain=_TmpD;
if(t1==0 || t2==0)return({struct _tuple0 _Tmp10;_Tmp10.f0=0,_Tmp10.f1=ain;_Tmp10;});else{
return({struct _tuple0 _Tmp10;({void*_Tmp11=({void*_Tmp12=Cyc_AssnDef_var(Cyc_AssnDef_memory);void*_Tmp13=Cyc_AssnDef_plus(t1,t2,0);Cyc_AssnDef_select(_Tmp12,_Tmp13,e->topt);});_Tmp10.f0=_Tmp11;}),_Tmp10.f1=ain;_Tmp10;});}}}case 30: _TmpA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_TmpA;struct Cyc_Absyn_Datatypedecl*dd=_Tmp8;struct Cyc_Absyn_Datatypefield*df=_Tmp7;
# 1801
struct _tuple15 _TmpC=Cyc_Vcgen_vcgen_rexps_always(env,es,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{struct Cyc_List_List*ts=_TmpE;void*ain=_TmpD;
unsigned i=Cyc_Vcgen_datatype_constructor_index(dd,df);
return({struct _tuple0 _TmpF;({void*_Tmp10=Cyc_AssnDef_datatype_aggr(i,ts,e->topt);_TmpF.f0=_Tmp10;}),_TmpF.f1=ain;_TmpF;});}}case 24: _TmpA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_TmpA;
# 1806
_TmpA=dles;goto _LL48;}case 25: _TmpA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL48: {struct Cyc_List_List*dles=_TmpA;
_TmpA=dles;goto _LL4A;}case 35: _TmpA=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL4A: {struct Cyc_List_List*dles=_TmpA;
# 1809
for(1;dles!=0;dles=dles->tl){
struct _tuple23*_TmpC=(struct _tuple23*)dles->hd;void*_TmpD;_TmpD=_TmpC->f1;{struct Cyc_Absyn_Exp*e=_TmpD;
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f1;}}
# 1813
return({struct _tuple0 _TmpC;_TmpC.f0=0,_TmpC.f1=ain;_TmpC;});}case 28: _TmpA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*dles=_TmpA;
# 1815
_TmpA=dles;goto _LL4E;}case 29: _TmpA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL4E: {struct Cyc_List_List*dles=_TmpA;
# 1817
void*atype=_check_null(e->topt);
struct _tuple16 _TmpC=Cyc_Vcgen_get_aggr_info(atype);enum Cyc_Absyn_AggrKind _TmpD;int _TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{int is_tagged=_TmpE;enum Cyc_Absyn_AggrKind ak=_TmpD;
struct Cyc_List_List*dlvs=0;
unsigned tag=0U;
for(1;dles!=0;dles=dles->tl){
struct _tuple23*_TmpF=(struct _tuple23*)dles->hd;void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF->f0;_Tmp10=_TmpF->f1;{struct Cyc_List_List*dl=_Tmp11;struct Cyc_Absyn_Exp*e=_Tmp10;
struct _tuple0 _Tmp12=Cyc_Vcgen_vcgen_rexp_always(env,e,ain);void*_Tmp13;void*_Tmp14;_Tmp14=_Tmp12.f0;_Tmp13=_Tmp12.f1;{void*v=_Tmp14;void*a=_Tmp13;
ain=a;{
void*_Tmp15;if(dl!=0){if(*((int*)((struct Cyc_List_List*)dl)->hd)==1){if(((struct Cyc_List_List*)dl)->tl==0){_Tmp15=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)dl->hd)->f1;{struct _fat_ptr*f=_Tmp15;
# 1827
int i=Cyc_CfFlowInfo_get_field_index(atype,f);
tag=(unsigned)i;
dlvs=({struct Cyc_List_List*_Tmp16=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple20*_Tmp17=({struct _tuple20*_Tmp18=_cycalloc(sizeof(struct _tuple20));_Tmp18->f0=i,_Tmp18->f1=v;_Tmp18;});_Tmp16->hd=_Tmp17;}),_Tmp16->tl=dlvs;_Tmp16;});
goto _LLDA;}}else{goto _LLDD;}}else{goto _LLDD;}}else{_LLDD:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("no field name in designator!",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}_LLDA:;}}}}
# 1834
dlvs=({(struct Cyc_List_List*(*)(int(*)(struct _tuple20*,struct _tuple20*),struct Cyc_List_List*))Cyc_List_rimp_merge_sort;})(Cyc_Vcgen_cmp_index,dlvs);{
struct Cyc_List_List*vs=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple20*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple20*))Cyc_Core_snd;}),dlvs);
void*res=(int)ak==1?Cyc_AssnDef_union_aggr(tag,vs,e->topt):
 Cyc_AssnDef_struct_aggr(vs,e->topt);
return({struct _tuple0 _TmpF;_TmpF.f0=res,_TmpF.f1=ain;_TmpF;});}}}case 26: _TmpA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_TmpA;struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 1842
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_always(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*t1=_TmpE;void*a=_TmpD;
void*v=Cyc_AssnDef_var(vd);
# 1845
a=({struct Cyc_Vcgen_Env*_TmpF=env;void*_Tmp10=a;void*_Tmp11=v;Cyc_Vcgen_do_assign(_TmpF,_Tmp10,_Tmp11,Cyc_AssnDef_zero());});
while(1){
void*at=({void*_TmpF=a;Cyc_AssnDef_and(_TmpF,Cyc_AssnDef_ult(v,t1));});
void*af=({void*_TmpF=a;Cyc_AssnDef_and(_TmpF,Cyc_AssnDef_ulte(t1,v));});
struct _tuple0 _TmpF=Cyc_Vcgen_vcgen_rexp_nodebug(env,e2,at);void*_Tmp10;_Tmp10=_TmpF.f1;{void*abody=_Tmp10;
abody=({struct Cyc_Vcgen_Env*_Tmp11=env;void*_Tmp12=abody;void*_Tmp13=v;Cyc_Vcgen_do_assign(_Tmp11,_Tmp12,_Tmp13,({void*_Tmp14=v;void*_Tmp15=Cyc_AssnDef_one();Cyc_AssnDef_plus(_Tmp14,_Tmp15,vd->type);}));});
if(Cyc_Vcgen_simple_implies(& abody,& a)){a=af;break;}
# 1853
a=Cyc_AssnDef_widen(Cyc_AssnDef_or(abody,a));}}{
# 1855
void*res=Cyc_AssnDef_fresh_var(e->topt);
# 1857
a=({void*_TmpF=a;Cyc_AssnDef_and(_TmpF,({void*_Tmp10=Cyc_AssnDef_numelts_term(res);Cyc_AssnDef_eq(_Tmp10,t1);}));});
return({struct _tuple0 _TmpF;_TmpF.f0=res,_TmpF.f1=a;_TmpF;});}}}case 27: _TmpA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
# 1860
return({struct _tuple0 _TmpC;_TmpC.f0=0,({void*_TmpD=Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain).f1;_TmpC.f1=_TmpD;});_TmpC;});}case 33: _Tmp2=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_MallocInfo mi=_Tmp2;
# 1862
if(mi.rgn!=0)
ain=Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.rgn,ain).f1;{
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_rexp_nodebug(env,mi.num_elts,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*t=_TmpE;void*ain=_TmpD;
void*res=Cyc_AssnDef_fresh_var(e->topt);
ain=({void*_TmpF=ain;Cyc_AssnDef_and(_TmpF,({void*_Tmp10=Cyc_AssnDef_numelts_term(res);Cyc_AssnDef_eq(_Tmp10,t);}));});
return({struct _tuple0 _TmpF;_TmpF.f0=res,_TmpF.f1=ain;_TmpF;});}}}case 34: _TmpA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp8;
# 1869
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_lexp(env,e1,ain);void*_TmpD;void*_TmpE;_TmpE=_TmpC.f0;_TmpD=_TmpC.f1;{void*lt1=_TmpE;void*a1=_TmpD;
Cyc_Vcgen_update_try_assn(env,a1);{
struct _tuple0 _TmpF=Cyc_Vcgen_vcgen_lexp(env,e2,a1);void*_Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*lt2=_Tmp11;void*a2=_Tmp10;
Cyc_Vcgen_update_try_assn(env,a2);{
void*t1=Cyc_Vcgen_deref_lterm(env,lt1,a2);
void*t2=Cyc_Vcgen_deref_lterm(env,lt2,a2);
a2=Cyc_Vcgen_do_assign(env,a2,lt1,t2);
a2=Cyc_Vcgen_do_assign(env,a2,lt2,t1);
return({struct _tuple0 _Tmp12;_Tmp12.f0=0,_Tmp12.f1=a2;_Tmp12;});}}}}}case 36: _TmpA=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_TmpA;
# 1879
void**old_exp_stmt=env->shared->exp_stmt;
({void**_TmpC=({void**_TmpD=_cycalloc(sizeof(void*));*_TmpD=0;_TmpD;});env->shared->exp_stmt=_TmpC;});{
void*aout=Cyc_Vcgen_vcgen_stmt_nodebug(env,s,ain);
void*t=*_check_null(env->shared->exp_stmt);
env->shared->exp_stmt=old_exp_stmt;
return({struct _tuple0 _TmpC;_TmpC.f0=t,_TmpC.f1=aout;_TmpC;});}}case 39:
 return({struct _tuple0 _TmpC;_TmpC.f0=0,_TmpC.f1=ain;_TmpC;});case 40: _TmpA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_TmpA;
return Cyc_Vcgen_vcgen_rexp_nodebug(env,e,ain);}case 2: _Tmp1=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp1;
return({struct _tuple0 _TmpC;({void*_TmpD=Cyc_AssnDef_zero();_TmpC.f0=_TmpD;}),_TmpC.f1=ain;_TmpC;});}default: _TmpA=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;
# 1889
struct _tuple0 _TmpC=Cyc_Vcgen_vcgen_test(env,e1,(void*)& Cyc_AssnDef_true_assn);void*_TmpD;_TmpD=_TmpC.f0;{void*at=_TmpD;
if(!Cyc_Vcgen_implies(& ain,& at))
({struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({
struct _fat_ptr _Tmp10=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(at));_TmpF.f1=_Tmp10;});_TmpF;});struct Cyc_String_pa_PrintArg_struct _TmpF=({struct Cyc_String_pa_PrintArg_struct _Tmp10;_Tmp10.tag=0,({struct _fat_ptr _Tmp11=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(ain)));_Tmp10.f1=_Tmp11;});_Tmp10;});void*_Tmp10[2];_Tmp10[0]=& _TmpE,_Tmp10[1]=& _TmpF;Cyc_Warn_err(e->loc,_tag_fat("cannot prove @assert(%s) \nfrom %s",sizeof(char),34U),_tag_fat(_Tmp10,sizeof(void*),2));});
return({struct _tuple0 _TmpE;({void*_TmpF=Cyc_AssnDef_one();_TmpE.f0=_TmpF;}),({void*_TmpF=Cyc_AssnDef_and(ain,at);_TmpE.f1=_TmpF;});_TmpE;});}}};}}
# 1899
void Cyc_Vcgen_vcgen_fundecl(unsigned loc,struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
struct _tuple19 _Tmp0=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp0.f0;_Tmp5=_Tmp0.f1;_Tmp4=_Tmp0.f2;_Tmp3=_Tmp0.f3;_Tmp2=_Tmp0.f4;_Tmp1=_Tmp0.f5;{struct Cyc_List_List*args=_Tmp6;struct Cyc_List_List*arvds=_Tmp5;void*reqopt=_Tmp4;void*ensopt=_Tmp3;void*thrwsopt=_Tmp2;struct Cyc_Absyn_Vardecl*ret_value=_Tmp1;
void*a=(void*)& Cyc_AssnDef_true_assn;
# 1903
if(reqopt!=0)
a=reqopt;{
# 1909
void**thrws_assn=0;
if(thrwsopt!=0)
thrws_assn=({void**_Tmp7=_cycalloc(sizeof(void*));*_Tmp7=(void*)& Cyc_AssnDef_false_assn;_Tmp7;});{
# 1913
void**result_assn=ensopt==0?0:({void**_Tmp7=_cycalloc(sizeof(void*));*_Tmp7=(void*)& Cyc_AssnDef_false_assn;_Tmp7;});
void*res_term=ret_value==0?0: Cyc_AssnDef_var(ret_value);
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_initial_env(tables,fd,assn_info,result_assn,thrws_assn,res_term);
Cyc_Vcgen_vcgen_stmt_nodebug(env,fd->body,a);
# 1918
if(ensopt!=0){
void*at=ensopt;
struct Cyc_Vcgen_SharedEnv*senv=env->shared;
void*res_assn_opt=*_check_null(senv->res_assn);
void*res_assn=(void*)& Cyc_AssnDef_false_assn;
if(res_assn_opt!=0)res_assn=res_assn_opt;
if(!Cyc_Vcgen_implies(& res_assn,& at))
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(at)));_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(res_assn)));_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err(loc,_tag_fat("cannot prove the @ensures clause %s from %s",sizeof(char),44U),_tag_fat(_Tmp9,sizeof(void*),2));});}
# 1929
if(thrwsopt!=0){
void*desired_throws_state=thrwsopt;
void*actual_throws_state=*_check_null(thrws_assn);
if(!Cyc_Vcgen_implies(& actual_throws_state,& desired_throws_state))
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(desired_throws_state)));_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_assn2string(Cyc_AssnDef_reduce(actual_throws_state)));_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err(loc,_tag_fat("cannot prove the @throws clause %s from %s",sizeof(char),43U),_tag_fat(_Tmp9,sizeof(void*),2));});}}}}}
# 1940
void*Cyc_Vcgen_clause2assn(struct Cyc_Absyn_Exp*e){
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_bogus_env();
return Cyc_Vcgen_vcgen_test(env,e,(void*)& Cyc_AssnDef_true_assn).f0;}

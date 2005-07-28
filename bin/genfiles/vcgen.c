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
# 115 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 220
extern struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 77 "flags.h"
extern int Cyc_Flags_allpaths;
# 36 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_AssnMap*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_AssnMap*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_AssnMap*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 542
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 706 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 890
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 905
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 911
void*Cyc_Absyn_compress(void*);
# 929
extern void*Cyc_Absyn_uint_type;
# 931
extern void*Cyc_Absyn_sint_type;
# 934
void*Cyc_Absyn_gen_float_type(unsigned);
# 938
extern void*Cyc_Absyn_heap_rgn_type;
# 940
extern void*Cyc_Absyn_al_qual_type;
# 944
extern void*Cyc_Absyn_false_type;
# 969
void*Cyc_Absyn_exn_type (void);
# 981
void*Cyc_Absyn_bounds_one (void);
# 991
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
# 1082
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1187
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1196
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;void*f2;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct Cyc_AssnDef_Alloc_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;void*f4;};struct Cyc_AssnDef_Offseti_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};
# 110 "assndef.h"
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory;
# 115
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_exception_vardecl (void);
# 118
extern void*Cyc_AssnDef_zero (void);
extern void*Cyc_AssnDef_one (void);
# 122
extern void*Cyc_AssnDef_uint(unsigned);
extern void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
extern void*Cyc_AssnDef_select(void*,void*,void*);
extern void*Cyc_AssnDef_update(void*,void*,void*);
# 127
extern void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop,void*,void*);
extern void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
extern void*Cyc_AssnDef_cast(void*,void*);
# 131
extern void*Cyc_AssnDef_tagof_tm(void*);
extern void*Cyc_AssnDef_proj(void*,unsigned,void*);
# 134
extern void*Cyc_AssnDef_addr(struct Cyc_Absyn_Vardecl*);
extern void*Cyc_AssnDef_alloc(struct Cyc_Absyn_Exp*,void*,void*);
extern void*Cyc_AssnDef_offsetf(void*,unsigned,void*);
extern void*Cyc_AssnDef_offseti(void*,void*,void*);
# 140
extern void*Cyc_AssnDef_numelts_term(void*);
extern void*Cyc_AssnDef_plus(void*,void*,void*);
# 143
extern void*Cyc_AssnDef_struct_aggr(struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_datatype_aggr(unsigned,struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_union_aggr(unsigned,struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_fresh_var(void*);struct _tuple13{void*f0;struct Cyc_List_List*f1;};
# 151
extern struct _tuple13 Cyc_AssnDef_split_addr(void*);
# 159
extern void*Cyc_AssnDef_apply_aggr_update(void*,struct Cyc_List_List*,void*);
# 161
extern void*Cyc_AssnDef_get_term_type(void*);struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};
# 189 "assndef.h"
extern struct _fat_ptr Cyc_AssnDef_assn2string(void*);
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 193
extern void*Cyc_AssnDef_and(void*,void*);
extern void*Cyc_AssnDef_or(void*,void*);
extern void*Cyc_AssnDef_not(void*);
# 197
extern void*Cyc_AssnDef_eq(void*,void*);
extern void*Cyc_AssnDef_neq(void*,void*);
extern void*Cyc_AssnDef_slt(void*,void*);
extern void*Cyc_AssnDef_slte(void*,void*);
extern void*Cyc_AssnDef_ult(void*,void*);
extern void*Cyc_AssnDef_ulte(void*,void*);struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};
# 217
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_false_assnmap (void);
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_true_assnmap (void);
# 220
extern struct _fat_ptr Cyc_AssnDef_assnmap2dag(struct Cyc_AssnDef_AssnMap);
extern struct _fat_ptr Cyc_AssnDef_assnmap2string(struct Cyc_AssnDef_AssnMap);struct _tuple14{void*f0;struct Cyc_AssnDef_AssnMap f1;};
# 224
extern struct _tuple14 Cyc_AssnDef_lookup_var_map(struct Cyc_Absyn_Vardecl*,struct Cyc_AssnDef_AssnMap);
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_update_var_map(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_AssnMap);struct _tuple15{void*f0;void*f1;struct Cyc_Dict_Dict f2;};
# 227
extern struct _tuple15 Cyc_AssnDef_unify_var_maps(struct Cyc_AssnDef_AssnMap,struct Cyc_AssnDef_AssnMap);
extern struct _tuple15 Cyc_AssnDef_unify_var_maps_subst(struct Cyc_AssnDef_AssnMap,struct Cyc_AssnDef_AssnMap);
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_canonical_assnmap(struct Cyc_AssnDef_AssnMap);
# 233
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assnmap(struct Cyc_AssnDef_AssnMap,struct Cyc_AssnDef_AssnMap);
# 235
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_widen_assnmap(struct Cyc_AssnDef_AssnMap);
# 238
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assn(struct Cyc_AssnDef_AssnMap,void*);
# 241
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_and_assnmap_assn(struct Cyc_AssnDef_AssnMap,void*);
# 243
extern int Cyc_AssnDef_simple_prove(void*,void*);
# 275 "assndef.h"
extern void*Cyc_AssnDef_subst_args(struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*,void*,void*,struct Cyc_AssnDef_AssnMap);struct Cyc_Vcgen_ExpChecks{struct Cyc_AssnDef_AssnMap ctxt;void*bounds_check;void*null_check;};
# 41 "tcutil.h"
int Cyc_Tcutil_is_pointer_type(void*);
# 47
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 51
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 69
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 81
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 222
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 236
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 247
int Cyc_Tcutil_force_type2bool(int,void*);struct _tuple16{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
# 159 "cf_flowinfo.h"
extern int Cyc_CfFlowInfo_get_field_index(void*,struct _fat_ptr*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc_Vcgen_Env{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table**assn_info;struct Cyc_AssnDef_AssnMap*try_assnmap;void**exp_stmt;struct Cyc_Absyn_Vardecl*result;struct Cyc_AssnDef_AssnMap*res_assnmap;struct Cyc_AssnDef_AssnMap*continue_assnmap;int widen_paths;int pure_exp;};
# 76 "vcgen.cyc"
static int Cyc_Vcgen_hash_ptr(void*s){return(int)s;}
# 80
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,struct Cyc_AssnDef_AssnMap*res_assnmap,struct Cyc_AssnDef_AssnMap*try_assnmap,struct Cyc_Absyn_Vardecl*result){
# 84
struct Cyc_Hashtable_Table*succ_table=({struct Cyc_Hashtable_Table*(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))Cyc_Hashtable_lookup;_Tmp0;})(tables->succ_tables,fd);
struct Cyc_Hashtable_Table*assn_table=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;_Tmp0;})(57,({int(*_Tmp0)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;_Tmp0;}),({int(*_Tmp0)(struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;_Tmp0;}));
struct Cyc_Vcgen_Env*env;env=_cycalloc(sizeof(struct Cyc_Vcgen_Env)),env->assn_table=assn_table,env->succ_table=succ_table,env->assn_info=assn_info,env->try_assnmap=try_assnmap,env->exp_stmt=0,env->result=result,env->res_assnmap=res_assnmap,env->continue_assnmap=0,env->widen_paths=!Cyc_Flags_allpaths,env->pure_exp=0;
# 96
return env;}
# 102
static struct Cyc_Vcgen_Env*Cyc_Vcgen_bogus_env (void){
struct Cyc_Hashtable_Table*succ_table=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;_Tmp0;})(1,({int(*_Tmp0)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;_Tmp0;}),({int(*_Tmp0)(struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;_Tmp0;}));
struct Cyc_Hashtable_Table*assn_table=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_create;_Tmp0;})(1,({int(*_Tmp0)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp;_Tmp0;}),({int(*_Tmp0)(struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;_Tmp0;}));
struct Cyc_Hashtable_Table**assn_info;assn_info=_cycalloc(sizeof(struct Cyc_Hashtable_Table*)),({struct Cyc_Hashtable_Table*_Tmp0=({struct Cyc_Hashtable_Table*(*_Tmp1)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;_Tmp1;})(1,({int(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;_Tmp1;}),({int(*_Tmp1)(struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr;_Tmp1;}));*assn_info=_Tmp0;});{
struct Cyc_Vcgen_Env*env;env=_cycalloc(sizeof(struct Cyc_Vcgen_Env)),env->assn_table=assn_table,env->succ_table=succ_table,env->assn_info=assn_info,env->try_assnmap=0,env->exp_stmt=0,env->result=0,env->res_assnmap=0,env->continue_assnmap=0,env->widen_paths=0,env->pure_exp=1;
# 116
return env;}}
# 120
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*env,struct Cyc_AssnDef_AssnMap am){
if(env->widen_paths)
return Cyc_AssnDef_widen_assnmap(am);
# 124
return am;}
# 131
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_lookup_stmt_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){
struct Cyc_AssnDef_AssnMap**preconopt=({struct Cyc_AssnDef_AssnMap**(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_AssnDef_AssnMap**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;_Tmp0;})(env->assn_table,s);
if(preconopt!=0)return*(*preconopt);else{
return Cyc_AssnDef_false_assnmap();}}
# 138
static void Cyc_Vcgen_update_stmt_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,struct Cyc_AssnDef_AssnMap newam){
struct Cyc_AssnDef_AssnMap**preconopt=({struct Cyc_AssnDef_AssnMap**(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_AssnDef_AssnMap**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup_opt;_Tmp0;})(env->assn_table,s);
if(preconopt!=0)({struct Cyc_AssnDef_AssnMap _Tmp0=({struct Cyc_Vcgen_Env*_Tmp1=env;Cyc_Vcgen_may_widen(_Tmp1,Cyc_AssnDef_or_assnmap_assnmap(*(*preconopt),newam));});*(*preconopt)=_Tmp0;});else{
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=env->assn_table;struct Cyc_Absyn_Stmt*_Tmp2=s;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_AssnMap*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));*_Tmp3=newam;_Tmp3;}));});}}
# 146
static void Cyc_Vcgen_replace_stmt_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,struct Cyc_AssnDef_AssnMap newam){
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=env->assn_table;struct Cyc_Absyn_Stmt*_Tmp2=s;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_AssnMap*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));*_Tmp3=newam;_Tmp3;}));});}
# 153
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_stmt(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap);
# 157
static struct _tuple14 Cyc_Vcgen_vcgen_rexp(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,struct Cyc_AssnDef_AssnMap);struct _tuple18{struct Cyc_List_List*f0;struct Cyc_AssnDef_AssnMap f1;};
# 160
static struct _tuple18 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,struct Cyc_AssnDef_AssnMap);
# 163
static struct _tuple14 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,struct Cyc_AssnDef_AssnMap);struct _tuple19{struct Cyc_AssnDef_AssnMap f0;struct Cyc_AssnDef_AssnMap f1;};
# 167
static struct _tuple19 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,struct Cyc_AssnDef_AssnMap);
# 172
static struct _tuple19 Cyc_Vcgen_vcgen_pat(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Pat*,void*,void*,struct Cyc_AssnDef_AssnMap);
# 175
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*,void*,struct Cyc_List_List*,struct Cyc_AssnDef_AssnMap);
# 178
static int Cyc_Vcgen_simple_implies(void*a1,void*a2){
return Cyc_AssnDef_simple_prove(a1,a2);}
# 182
static int Cyc_Vcgen_implies(void*a1,void*a2){
return Cyc_Vcgen_simple_implies(a1,a2)|| Cyc_PrattProver_constraint_prove(a1,a2);}
# 186
static int Cyc_Vcgen_implies_assnmap(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
# 191
struct _tuple15 _Tmp0=Cyc_AssnDef_unify_var_maps(am1,am2);struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*a1=_Tmp3;void*a2=_Tmp2;struct Cyc_Dict_Dict map=_Tmp1;
return Cyc_Vcgen_simple_implies(a1,a2)|| Cyc_PrattProver_constraint_prove(a1,a2);}}
# 196
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){
void*_Tmp0;switch(*((int*)b)){case 1: _Tmp0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}case 2: _Tmp0=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp0;
return fd->fn_vardecl;}case 0:
({int(*_Tmp1)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp1;})(_tag_fat("vcgen bind2vardecl",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp0=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}case 4: _Tmp0=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}default: _Tmp0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd;}};}
# 208
static int Cyc_Vcgen_is_unsigned_int_exp(struct Cyc_Absyn_Exp*e){
void*t=e->topt;
if(t==0)return 0;{
enum Cyc_Absyn_Size_of _Tmp0;if(t!=0){if(*((int*)t)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1==Cyc_Absyn_Unsigned){_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Size_of sz=_Tmp0;
# 214
if((int)sz==3 ||(int)sz==2)
return 1;else{
return 0;}}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 return 0;};}}
# 226
static struct _tuple14 Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,struct Cyc_AssnDef_AssnMap am){
void*_Tmp0;if(*((int*)lt)==11){_Tmp0=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)lt)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
# 229
if(!vd->escapes)
return Cyc_AssnDef_lookup_var_map(vd,am);
# 232
goto _LL4;}}else{_LL4: {
# 234
struct _tuple14 _Tmp1=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{void*mem=_Tmp3;struct Cyc_AssnDef_AssnMap am1=_Tmp2;
struct _tuple14 _Tmp4;({void*_Tmp5=Cyc_AssnDef_select(mem,lt,0);_Tmp4.f0=_Tmp5;}),_Tmp4.f1=am1;return _Tmp4;}}};}
# 242
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,struct Cyc_AssnDef_AssnMap am,void*lt,void*t){
struct _tuple13 _Tmp0=Cyc_AssnDef_split_addr(lt);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*root=_Tmp2;struct Cyc_List_List*fields=_Tmp1;
void*_Tmp3;if(*((int*)root)==11){_Tmp3=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)root)->f1;if(!((struct Cyc_Absyn_Vardecl*)_Tmp3)->escapes){struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 246
struct _tuple14 _Tmp4=Cyc_AssnDef_lookup_var_map(vd,am);struct Cyc_AssnDef_AssnMap _Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{void*oldv=_Tmp6;struct Cyc_AssnDef_AssnMap am1=_Tmp5;
struct Cyc_Absyn_Vardecl*_Tmp7=vd;void*_Tmp8=Cyc_AssnDef_apply_aggr_update(oldv,fields,t);return Cyc_AssnDef_update_var_map(_Tmp7,_Tmp8,am1);}}else{goto _LL6;}}else{_LL6: {
# 249
struct _tuple14 _Tmp4=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{void*mem=_Tmp6;struct Cyc_AssnDef_AssnMap am1=_Tmp5;
struct Cyc_Absyn_Vardecl*_Tmp7=Cyc_AssnDef_memory;void*_Tmp8=Cyc_AssnDef_update(mem,lt,t);return Cyc_AssnDef_update_var_map(_Tmp7,_Tmp8,am1);}}};}}
# 256
static void Cyc_Vcgen_update_try_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_AssnDef_AssnMap am,void*exn_value){
if(env->try_assnmap==0)return;
am=({struct Cyc_Vcgen_Env*_Tmp0=env;struct Cyc_AssnDef_AssnMap _Tmp1=am;void*_Tmp2=Cyc_AssnDef_addr(Cyc_AssnDef_exception_vardecl());Cyc_Vcgen_do_assign(_Tmp0,_Tmp1,_Tmp2,exn_value);});
am=({struct Cyc_Vcgen_Env*_Tmp0=env;Cyc_Vcgen_may_widen(_Tmp0,Cyc_AssnDef_or_assnmap_assnmap(*_check_null(env->try_assnmap),am));});
*_check_null(env->try_assnmap)=am;}
# 263
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,struct Cyc_AssnDef_AssnMap amin){
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 268
amin=({struct Cyc_Vcgen_Env*_Tmp4=env;struct Cyc_AssnDef_AssnMap _Tmp5=amin;void*_Tmp6=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_Tmp4,_Tmp5,_Tmp6,Cyc_AssnDef_fresh_var(vd->type));});
if(vd->initializer!=0){
struct _tuple14 _Tmp4=Cyc_Vcgen_vcgen_rexp(env,vd->initializer,amin);struct Cyc_AssnDef_AssnMap _Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{void*t=_Tmp6;struct Cyc_AssnDef_AssnMap am=_Tmp5;
am=({struct Cyc_Vcgen_Env*_Tmp7=env;struct Cyc_AssnDef_AssnMap _Tmp8=am;void*_Tmp9=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_Tmp7,_Tmp8,_Tmp9,t);});
return am;}}
# 274
return amin;}case 2: _Tmp3=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Pat*p=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;void*dtree=_Tmp1;
# 276
struct _tuple14 _Tmp4=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{void*v=_Tmp6;struct Cyc_AssnDef_AssnMap am=_Tmp5;
struct _tuple19 _Tmp7=Cyc_Vcgen_vcgen_pat(env,p,0,v,am);struct Cyc_AssnDef_AssnMap _Tmp8;struct Cyc_AssnDef_AssnMap _Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{struct Cyc_AssnDef_AssnMap amt=_Tmp9;struct Cyc_AssnDef_AssnMap amf=_Tmp8;
# 281
if(amf.assn!=(void*)& Cyc_AssnDef_false_assn)
({struct Cyc_Vcgen_Env*_TmpA=env;struct Cyc_AssnDef_AssnMap _TmpB=amf;Cyc_Vcgen_update_try_assnmap(_TmpA,_TmpB,Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type()));});
return amt;}}}case 3: _Tmp3=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*vds=_Tmp3;
# 287
return amin;}case 4: _Tmp3=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Exp*eopt=_Tmp2;
# 289
if(eopt!=0){
struct _tuple14 _Tmp4=Cyc_Vcgen_vcgen_rexp(env,eopt,amin);struct Cyc_AssnDef_AssnMap _Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{void*t=_Tmp6;struct Cyc_AssnDef_AssnMap am=_Tmp5;
am=({struct Cyc_Vcgen_Env*_Tmp7=env;struct Cyc_AssnDef_AssnMap _Tmp8=am;void*_Tmp9=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_Tmp7,_Tmp8,_Tmp9,t);});
return am;}}
# 294
return amin;}default:
 return amin;};}struct _tuple20{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_AssnDef_AssnMap*f2;struct Cyc_AssnDef_AssnMap*f3;struct Cyc_AssnDef_AssnMap*f4;struct Cyc_Absyn_Vardecl*f5;};
# 301
static struct _tuple20 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){
struct _tuple20 _Tmp0;_Tmp0.f0=fi->args,_Tmp0.f1=fi->arg_vardecls,_Tmp0.f2=fi->requires_assn,_Tmp0.f3=fi->ensures_assn,_Tmp0.f4=fi->throws_assn,_Tmp0.f5=fi->return_value;return _Tmp0;}
# 306
static struct _tuple20 Cyc_Vcgen_get_requires_and_ensures(void*topt){
if(topt!=0){
void*t=Cyc_Tcutil_pointer_elt_type(topt);
void*_Tmp0;if(*((int*)t)==5){_Tmp0=(struct Cyc_Absyn_FnInfo*)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_Tmp0;
return Cyc_Vcgen_get_requires_and_ensures_info(fi);}}else{
goto _LL0;}_LL0:;}{
# 314
struct _tuple20 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=0,_Tmp0.f2=0,_Tmp0.f3=0,_Tmp0.f4=0,_Tmp0.f5=0;return _Tmp0;}}
# 317
static unsigned Cyc_Vcgen_datatype_constructor_index(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
# 319
unsigned i=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(dd->fields)->v;
for(1;fs!=0;(fs=fs->tl,++ i)){
if((struct Cyc_Absyn_Datatypefield*)fs->hd==df)return i;
if(Cyc_Absyn_qvar_cmp(df->name,((struct Cyc_Absyn_Datatypefield*)fs->hd)->name)==0)return i;}
# 325
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("vcgen: datatype_constructor_index",sizeof(char),34U),_tag_fat(0U,sizeof(void*),0));}struct _tuple21{int f0;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};
# 328
static struct _tuple21 Cyc_Vcgen_get_aggr_info(void*t){
# 330
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;int _Tmp2;enum Cyc_Absyn_AggrKind _Tmp3;union Cyc_Absyn_AggrInfo _Tmp4;switch(*((int*)_Tmp0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==24){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp4;
# 332
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
struct _tuple21 _Tmp5;_Tmp5.f0=_check_null(ad->impl)->tagged,_Tmp5.f1=ad->kind,_Tmp5.f2=ad->impl->fields;return _Tmp5;}}else{goto _LL5;}case 6: _Tmp3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f3;{enum Cyc_Absyn_AggrKind ak=_Tmp3;int is_tuple=_Tmp2;struct Cyc_List_List*fs=_Tmp1;
# 335
struct _tuple21 _Tmp5;_Tmp5.f0=0,_Tmp5.f1=ak,_Tmp5.f2=fs;return _Tmp5;}default: _LL5:
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("vcgen: bad type in get_aggr_info",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));};}struct _tuple22{int f0;void*f1;};
# 340
static int Cyc_Vcgen_cmp_index(struct _tuple22*x,struct _tuple22*y){
return x->f0 - y->f0;}
# 345
struct Cyc_Hashtable_Table**Cyc_Vcgen_new_assn_info (void){
struct Cyc_Hashtable_Table*assn_info=({struct Cyc_Hashtable_Table*(*_Tmp0)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))Cyc_Hashtable_create;_Tmp0;})(57,({int(*_Tmp0)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Core_ptrcmp;_Tmp0;}),({int(*_Tmp0)(struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr;_Tmp0;}));
struct Cyc_Hashtable_Table**_Tmp0=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));*_Tmp0=assn_info;return _Tmp0;}
# 350
struct Cyc_Vcgen_ExpChecks*Cyc_Vcgen_exp2ctxt_checks(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){
struct Cyc_Vcgen_ExpChecks**result=({struct Cyc_Vcgen_ExpChecks**(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*)=(struct Cyc_Vcgen_ExpChecks**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))Cyc_Hashtable_lookup_opt;_Tmp0;})(*_check_null(assn_info),e);
if(result==0){
static struct Cyc_Vcgen_ExpChecks*b=0;
if(b==0)
b=({struct Cyc_Vcgen_ExpChecks*_Tmp0=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));({struct Cyc_AssnDef_AssnMap _Tmp1=Cyc_AssnDef_true_assnmap();_Tmp0->ctxt=_Tmp1;}),_Tmp0->bounds_check=(void*)& Cyc_AssnDef_false_assn,_Tmp0->null_check=(void*)& Cyc_AssnDef_false_assn;_Tmp0;});
# 361
return b;}else{
# 363
return*result;}}
# 367
static void Cyc_Vcgen_insert_exp_checks(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct Cyc_Vcgen_ExpChecks*exp_check){
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*))Cyc_Hashtable_insert;_Tmp0;})(*_check_null(assn_info),e,exp_check);
return;}
# 372
static struct Cyc_Absyn_Exp*Cyc_Vcgen_zero_exp (void){
struct Cyc_Absyn_Exp*ans=Cyc_Absyn_uint_exp(0U,0U);
ans->topt=Cyc_Absyn_uint_type;
return ans;}
# 378
static struct _tuple0 Cyc_Vcgen_okderef(void*root,void*i){
{void*_Tmp0;void*_Tmp1;switch(*((int*)root)){case 12: _Tmp1=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)root)->f2;{void*size=_Tmp1;
struct _tuple0 _Tmp2;({void*_Tmp3=Cyc_AssnDef_ult(i,size);_Tmp2.f0=_Tmp3;}),_Tmp2.f1=(void*)& Cyc_AssnDef_true_assn;return _Tmp2;}case 14: _Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)root)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)root)->f2;{void*r=_Tmp1;void*i0=_Tmp0;
# 382
struct _tuple0 _Tmp2=({void*_Tmp3=r;Cyc_Vcgen_okderef(_Tmp3,Cyc_AssnDef_plus(i,i0,0));});void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{void*bndck=_Tmp4;void*nullck=_Tmp3;
struct _tuple0 _Tmp5;({void*_Tmp6=({void*_Tmp7=({void*_Tmp8=i;Cyc_AssnDef_ult(_Tmp8,Cyc_AssnDef_numelts_term(root));});Cyc_AssnDef_or(_Tmp7,bndck);});_Tmp5.f0=_Tmp6;}),({
# 385
void*_Tmp6=({void*_Tmp7=({void*_Tmp8=root;Cyc_AssnDef_neq(_Tmp8,Cyc_AssnDef_zero());});Cyc_AssnDef_or(_Tmp7,nullck);});_Tmp5.f1=_Tmp6;});return _Tmp5;}}case 11: _Tmp1=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)root)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 387
{void*_Tmp2=Cyc_Absyn_compress(vd->type);struct Cyc_Absyn_ArrayInfo _Tmp3;if(*((int*)_Tmp2)==4){_Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp3;
# 389
if(ai.num_elts!=0){
struct _tuple14 _Tmp4=({struct Cyc_Vcgen_Env*_Tmp5=Cyc_Vcgen_bogus_env();struct Cyc_Absyn_Exp*_Tmp6=ai.num_elts;Cyc_Vcgen_vcgen_rexp(_Tmp5,_Tmp6,Cyc_AssnDef_true_assnmap());});void*_Tmp5;_Tmp5=_Tmp4.f0;{void*size=_Tmp5;
struct _tuple0 _Tmp6;({void*_Tmp7=Cyc_AssnDef_ult(i,size);_Tmp6.f0=_Tmp7;}),_Tmp6.f1=(void*)& Cyc_AssnDef_true_assn;return _Tmp6;}}
# 393
goto _LLC;}}else{
# 395
goto _LLC;}_LLC:;}{
# 397
struct _tuple0 _Tmp2;({void*_Tmp3=({void*_Tmp4=i;Cyc_AssnDef_ult(_Tmp4,Cyc_AssnDef_numelts_term(root));});_Tmp2.f0=_Tmp3;}),_Tmp2.f1=(void*)& Cyc_AssnDef_true_assn;return _Tmp2;}}default:  {
# 399
void*tp=Cyc_AssnDef_get_term_type(root);
if(tp!=0){
void*_Tmp2=Cyc_Absyn_compress(tp);struct Cyc_Absyn_PtrInfo _Tmp3;if(*((int*)_Tmp2)==3){_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1;{struct Cyc_Absyn_PtrInfo p1=_Tmp3;
# 403
struct Cyc_Absyn_Exp*b1=({void*_Tmp4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp4,p1.ptr_atts.bounds);});
int b2=Cyc_Tcutil_is_nullable_pointer_type(tp,0);
void*nullck=b2?({void*_Tmp4=root;Cyc_AssnDef_neq(_Tmp4,Cyc_AssnDef_zero());}):(void*)& Cyc_AssnDef_true_assn;
if(b1!=0){
# 413
struct _tuple0 _Tmp4;({void*_Tmp5=({void*_Tmp6=({void*_Tmp7=i;Cyc_AssnDef_ult(_Tmp7,Cyc_AssnDef_cnst(b1));});Cyc_AssnDef_or(_Tmp6,({
void*_Tmp7=i;Cyc_AssnDef_ult(_Tmp7,Cyc_AssnDef_numelts_term(root));}));});
# 413
_Tmp4.f0=_Tmp5;}),_Tmp4.f1=nullck;return _Tmp4;}
# 415
goto _LL14;}}else{
# 417
goto _LL14;}_LL14:;}
# 420
goto _LL0;}}_LL0:;}{
# 422
struct _tuple0 _Tmp0;({void*_Tmp1=({void*_Tmp2=i;Cyc_AssnDef_ult(_Tmp2,Cyc_AssnDef_numelts_term(root));});_Tmp0.f0=_Tmp1;}),({void*_Tmp1=({void*_Tmp2=root;Cyc_AssnDef_neq(_Tmp2,Cyc_AssnDef_zero());});_Tmp0.f1=_Tmp1;});return _Tmp0;}}struct _tuple23{void*f0;void*f1;struct Cyc_AssnDef_AssnMap f2;};
# 426
static struct _tuple23 Cyc_Vcgen_vcgen_deref(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*root,struct Cyc_Absyn_Exp*index,struct Cyc_AssnDef_AssnMap amin){
# 428
struct _tuple14 _Tmp0=Cyc_Vcgen_vcgen_rexp(env,root,amin);struct Cyc_AssnDef_AssnMap _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*t1=_Tmp2;struct Cyc_AssnDef_AssnMap am1=_Tmp1;
struct _tuple14 _Tmp3=Cyc_Vcgen_vcgen_rexp(env,index,am1);struct Cyc_AssnDef_AssnMap _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{void*t2=_Tmp5;struct Cyc_AssnDef_AssnMap am2=_Tmp4;
if(!env->pure_exp){
# 433
struct _tuple0 _Tmp6=Cyc_Vcgen_okderef(t1,t2);void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{void*bnds_check=_Tmp8;void*null_check=_Tmp7;
# 439
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(_check_null(root->topt));
void*term_numelts;
void*bnds_check_sufficient;
if(eopt==0){
# 444
term_numelts=Cyc_AssnDef_numelts_term(t1);
bnds_check_sufficient=Cyc_AssnDef_ult(t2,term_numelts);}else{
# 447
term_numelts=({struct Cyc_Vcgen_Env*_Tmp9=env;struct Cyc_Absyn_Exp*_TmpA=eopt;Cyc_Vcgen_vcgen_rexp(_Tmp9,_TmpA,Cyc_AssnDef_true_assnmap());}).f0;
bnds_check_sufficient=Cyc_AssnDef_ult(t2,term_numelts);}
# 450
({struct Cyc_Vcgen_Env*_Tmp9=env;struct Cyc_AssnDef_AssnMap _TmpA=({struct Cyc_AssnDef_AssnMap _TmpB=am2;Cyc_AssnDef_and_assnmap_assn(_TmpB,({void*_TmpC=Cyc_AssnDef_not(bnds_check_sufficient);Cyc_AssnDef_or(_TmpC,Cyc_AssnDef_not(null_check));}));});Cyc_Vcgen_update_try_assnmap(_Tmp9,_TmpA,Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type()));});{
# 452
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks)),exp_checks->ctxt=am2,exp_checks->bounds_check=bnds_check,exp_checks->null_check=null_check;
# 454
Cyc_Vcgen_insert_exp_checks(env->assn_info,root,exp_checks);
# 456
am2=({struct Cyc_AssnDef_AssnMap _Tmp9=am2;Cyc_AssnDef_and_assnmap_assn(_Tmp9,Cyc_AssnDef_and(bnds_check,null_check));});}}}{
# 458
struct _tuple23 _Tmp6;_Tmp6.f0=t1,_Tmp6.f1=t2,_Tmp6.f2=am2;return _Tmp6;}}}}
# 461
static struct _tuple14 Cyc_Vcgen_vcgen_comprehension(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,void*res_type,struct Cyc_AssnDef_AssnMap amin){
# 465
struct _tuple14 _Tmp0=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*t1=_Tmp2;struct Cyc_AssnDef_AssnMap amin=_Tmp1;
# 467
void*v=Cyc_AssnDef_zero();
amin=({struct Cyc_Vcgen_Env*_Tmp3=env;struct Cyc_AssnDef_AssnMap _Tmp4=amin;void*_Tmp5=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_Tmp3,_Tmp4,_Tmp5,v);});
while(1){
amin=Cyc_AssnDef_canonical_assnmap(amin);{
# 472
struct _tuple14 _Tmp3=Cyc_AssnDef_lookup_var_map(vd,amin);struct Cyc_AssnDef_AssnMap _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{void*v2=_Tmp5;struct Cyc_AssnDef_AssnMap am2=_Tmp4;
v=v2;amin=am2;{
struct Cyc_AssnDef_AssnMap amt=({struct Cyc_AssnDef_AssnMap _Tmp6=amin;Cyc_AssnDef_and_assnmap_assn(_Tmp6,Cyc_AssnDef_ult(v,t1));});
struct Cyc_AssnDef_AssnMap amf=({struct Cyc_AssnDef_AssnMap _Tmp6=amin;Cyc_AssnDef_and_assnmap_assn(_Tmp6,Cyc_AssnDef_ulte(t1,v));});
struct _tuple14 _Tmp6=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _Tmp7;_Tmp7=_Tmp6.f1;{struct Cyc_AssnDef_AssnMap ambody=_Tmp7;
# 478
struct _tuple14 _Tmp8=Cyc_AssnDef_lookup_var_map(vd,ambody);struct Cyc_AssnDef_AssnMap _Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*v2=_TmpA;struct Cyc_AssnDef_AssnMap am2=_Tmp9;
ambody=({struct Cyc_Vcgen_Env*_TmpB=env;struct Cyc_AssnDef_AssnMap _TmpC=am2;void*_TmpD=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_TmpB,_TmpC,_TmpD,({void*_TmpE=v2;void*_TmpF=Cyc_AssnDef_one();Cyc_AssnDef_plus(_TmpE,_TmpF,Cyc_Absyn_uint_type);}));});
ambody=Cyc_AssnDef_canonical_assnmap(ambody);{
# 482
struct _tuple15 _TmpB=Cyc_AssnDef_unify_var_maps_subst(amin,ambody);struct Cyc_Dict_Dict _TmpC;void*_TmpD;void*_TmpE;_TmpE=_TmpB.f0;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{void*a1=_TmpE;void*a2=_TmpD;struct Cyc_Dict_Dict map=_TmpC;
# 485
if(Cyc_Vcgen_implies(a2,a1)){
# 487
amin=amf;
break;}
# 490
amin=Cyc_AssnDef_widen_assnmap(({struct Cyc_AssnDef_AssnMap _TmpF;({void*_Tmp10=Cyc_AssnDef_or(a1,a2);_TmpF.assn=_Tmp10;}),_TmpF.map=map;_TmpF;}));}}}}}}}
# 470
1U;}{
# 493
void*res=Cyc_AssnDef_fresh_var(res_type);
# 495
amin=({struct Cyc_AssnDef_AssnMap _Tmp3=amin;Cyc_AssnDef_and_assnmap_assn(_Tmp3,({void*_Tmp4=Cyc_AssnDef_numelts_term(res);Cyc_AssnDef_eq(_Tmp4,t1);}));});{
struct _tuple14 _Tmp3;_Tmp3.f0=res,_Tmp3.f1=amin;return _Tmp3;}}}}
# 499
static struct _tuple14 Cyc_Vcgen_vcgen_binop(struct Cyc_Vcgen_Env*env,void*tp_opt,enum Cyc_Absyn_Primop p,void*t1,void*t2,struct Cyc_AssnDef_AssnMap am){
# 502
void*tp=_check_null(tp_opt);
if(Cyc_Tcutil_is_pointer_type(tp)){
switch((int)p){case Cyc_Absyn_Plus:
# 508
 if((!env->pure_exp && Cyc_Tcutil_is_zeroterm_pointer_type(tp))&& !
Cyc_Tcutil_is_fat_pointer_type(tp))
({struct Cyc_Vcgen_Env*_Tmp0=env;struct Cyc_AssnDef_AssnMap _Tmp1=am;Cyc_Vcgen_update_try_assnmap(_Tmp0,_Tmp1,Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type()));});{
struct _tuple14 _Tmp0;({void*_Tmp1=Cyc_AssnDef_offseti(t1,t2,tp);_Tmp0.f0=_Tmp1;}),_Tmp0.f1=am;return _Tmp0;}case Cyc_Absyn_Minus:  {
# 513
struct _tuple14 _Tmp0;({void*_Tmp1=({void*_Tmp2=t1;void*_Tmp3=Cyc_AssnDef_unop(2U,t2,Cyc_Absyn_sint_type);Cyc_AssnDef_offseti(_Tmp2,_Tmp3,tp);});_Tmp0.f0=_Tmp1;}),_Tmp0.f1=am;return _Tmp0;}default:
# 515
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("vcgen: result has pointer type but uses bad primop",sizeof(char),51U),_tag_fat(0U,sizeof(void*),0));};}{
# 519
void*t=Cyc_AssnDef_binop(p,t1,t2,tp);
# 522
if((!env->pure_exp &&(int)p==4)&& Cyc_Unify_unify(Cyc_Absyn_uint_type,tp))
am=({struct Cyc_AssnDef_AssnMap _Tmp0=am;Cyc_AssnDef_and_assnmap_assn(_Tmp0,Cyc_AssnDef_ult(t,t2));});{
struct _tuple14 _Tmp0;_Tmp0.f0=t,_Tmp0.f1=am;return _Tmp0;}}}struct _tuple24{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple25{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 527
static struct _tuple14 Cyc_Vcgen_vcgen_rexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap amin){
if(Cyc_Tcutil_is_const_exp(e)){
# 530
{void*_Tmp0=e->r;struct _fat_ptr _Tmp1;switch(*((int*)_Tmp0)){case 2: _Tmp1=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp1;
# 532
if(Cyc_strcmp(s,_tag_fat("print_assn",sizeof(char),11U))==0){
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)seg_str;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_fprintf(Cyc_stderr,_tag_fat("%s: %s\n",sizeof(char),8U),_tag_fat(_Tmp4,sizeof(void*),2));});}
# 536
if(Cyc_strcmp(s,_tag_fat("print_dag",sizeof(char),10U))==0){
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(e->loc);
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=(struct _fat_ptr)seg_str;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_assnmap2dag(amin);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_fprintf(Cyc_stderr,_tag_fat("%s: %s\n",sizeof(char),8U),_tag_fat(_Tmp4,sizeof(void*),2));});}
# 540
if(Cyc_strcmp(s,_tag_fat("all_paths",sizeof(char),10U))==0)
env->widen_paths=0;
# 543
if(Cyc_strcmp(s,_tag_fat("one_path",sizeof(char),9U))==0)
env->widen_paths=1;{
# 546
struct _tuple14 _Tmp2;({void*_Tmp3=Cyc_AssnDef_zero();_Tmp2.f0=_Tmp3;}),_Tmp2.f1=amin;return _Tmp2;}}case 28:
 goto _LL6;case 29: _LL6:
 goto _LL8;case 30: _LL8:
 goto _LLA;case 26: _LLA:
 goto _LLC;case 25: _LLC:
# 556
 goto ANALYZE;default:
 goto _LL0;}_LL0:;}{
# 559
struct _tuple16 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{unsigned cn=_Tmp2;int known=_Tmp1;
if(known){struct _tuple14 _Tmp3;({void*_Tmp4=Cyc_AssnDef_uint(cn);_Tmp3.f0=_Tmp4;}),_Tmp3.f1=amin;return _Tmp3;}else{
struct _tuple14 _Tmp3;({void*_Tmp4=Cyc_AssnDef_cnst(e);_Tmp3.f0=_Tmp4;}),_Tmp3.f1=amin;return _Tmp3;}}}}
# 563
ANALYZE: {
void*_Tmp0=e->r;struct Cyc_Absyn_MallocInfo _Tmp1;int _Tmp2;int _Tmp3;enum Cyc_Absyn_Coercion _Tmp4;enum Cyc_Absyn_Incrementor _Tmp5;void*_Tmp6;void*_Tmp7;enum Cyc_Absyn_Primop _Tmp8;void*_Tmp9;union Cyc_Absyn_Cnst _TmpA;switch(*((int*)_Tmp0)){case 0: _TmpA=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_TmpA;
goto _LL16;}case 2: _LL16:
# 567
({int(*_TmpB)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpB;})(_tag_fat("Const_e Pragma_e are const_exps\n",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));case 1: _Tmp9=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp9;
# 572
struct Cyc_Absyn_Vardecl*vdopt=Cyc_Vcgen_bind2vardecl(b);
if(vdopt==0){struct _tuple14 _TmpB;({void*_TmpC=Cyc_AssnDef_fresh_var(0);_TmpB.f0=_TmpC;}),_TmpB.f1=amin;return _TmpB;}{
struct Cyc_Absyn_Vardecl*vd=vdopt;
if(vd->escapes){
struct _tuple14 _TmpB=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*mem=_TmpD;struct Cyc_AssnDef_AssnMap am1=_TmpC;
struct _tuple14 _TmpE;({void*_TmpF=({void*_Tmp10=mem;void*_Tmp11=Cyc_AssnDef_addr(vd);Cyc_AssnDef_select(_Tmp10,_Tmp11,vd->type);});_TmpE.f0=_TmpF;}),_TmpE.f1=am1;return _TmpE;}}else{
# 580
{void*_TmpB=Cyc_Absyn_compress(vd->type);if(*((int*)_TmpB)==4){
# 582
struct _tuple14 _TmpC;({void*_TmpD=Cyc_AssnDef_addr(vd);_TmpC.f0=_TmpD;}),_TmpC.f1=amin;return _TmpC;}else{
goto _LL70;}_LL70:;}{
# 585
struct _tuple14 _TmpB=Cyc_AssnDef_lookup_var_map(vd,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t=_TmpD;struct Cyc_AssnDef_AssnMap am=_TmpC;
{struct _tuple14 _TmpE;_TmpE.f0=t,_TmpE.f1=am;return _TmpE;}
return Cyc_AssnDef_lookup_var_map(vd,amin);}}}}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl==0){_Tmp8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp9=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;{enum Cyc_Absyn_Primop p=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp9;
# 591
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t=_TmpD;struct Cyc_AssnDef_AssnMap am=_TmpC;
struct _tuple14 _TmpE;({void*_TmpF=Cyc_AssnDef_unop(p,t,e->topt);_TmpE.f0=_TmpF;}),_TmpE.f1=am;return _TmpE;}}}else{_Tmp8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp9=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;_Tmp7=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->tl->hd;{enum Cyc_Absyn_Primop p=_Tmp8;struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 595
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t1=_TmpD;struct Cyc_AssnDef_AssnMap am1=_TmpC;
struct _tuple14 _TmpE=Cyc_Vcgen_vcgen_rexp(env,e2,am1);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*t2=_Tmp10;struct Cyc_AssnDef_AssnMap am2=_TmpF;
return Cyc_Vcgen_vcgen_binop(env,e->topt,p,t1,t2,am2);}}}}}else{
({int(*_TmpB)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpB;})(_tag_fat("vcgen: bad primop",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));}case 4: _Tmp9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Core_Opt*popt=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 601
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e2,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t2=_TmpD;struct Cyc_AssnDef_AssnMap am1=_TmpC;
struct _tuple14 _TmpE=Cyc_Vcgen_vcgen_lexp(env,e1,am1);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*lt=_Tmp10;struct Cyc_AssnDef_AssnMap am2=_TmpF;
struct Cyc_AssnDef_AssnMap amout;
if(popt!=0){
struct _tuple14 _Tmp11=Cyc_Vcgen_deref_lterm(env,lt,am2);struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*t1=_Tmp13;struct Cyc_AssnDef_AssnMap am3=_Tmp12;
struct _tuple14 _Tmp14=Cyc_Vcgen_vcgen_binop(env,e->topt,(enum Cyc_Absyn_Primop)popt->v,t1,t2,am3);struct Cyc_AssnDef_AssnMap _Tmp15;void*_Tmp16;_Tmp16=_Tmp14.f0;_Tmp15=_Tmp14.f1;{void*t2=_Tmp16;struct Cyc_AssnDef_AssnMap am4=_Tmp15;
amout=Cyc_Vcgen_do_assign(env,am4,lt,t2);{
struct _tuple14 _Tmp17;_Tmp17.f0=t2,_Tmp17.f1=amout;return _Tmp17;}}}}else{
# 610
amout=Cyc_Vcgen_do_assign(env,am2,lt,t2);{
struct _tuple14 _Tmp11;_Tmp11.f0=t2,_Tmp11.f1=amout;return _Tmp11;}}}}}case 5: _Tmp9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp9;enum Cyc_Absyn_Incrementor i=_Tmp5;
# 614
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_lexp(env,e,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*lt=_TmpD;struct Cyc_AssnDef_AssnMap am1=_TmpC;
struct _tuple14 _TmpE=Cyc_Vcgen_deref_lterm(env,lt,am1);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*t1=_Tmp10;struct Cyc_AssnDef_AssnMap am2=_TmpF;
void*res;void*rvalue;
struct Cyc_AssnDef_AssnMap amout;
switch((int)i){case Cyc_Absyn_PostInc:  {
# 620
struct _tuple14 _Tmp11=({struct Cyc_Vcgen_Env*_Tmp12=env;void*_Tmp13=e->topt;void*_Tmp14=t1;void*_Tmp15=Cyc_AssnDef_one();Cyc_Vcgen_vcgen_binop(_Tmp12,_Tmp13,0U,_Tmp14,_Tmp15,am2);});struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*t=_Tmp13;struct Cyc_AssnDef_AssnMap am=_Tmp12;
res=t1;
rvalue=t;
amout=am;
goto _LL93;}}case Cyc_Absyn_PreInc:  {
# 626
struct _tuple14 _Tmp11=({struct Cyc_Vcgen_Env*_Tmp12=env;void*_Tmp13=e->topt;void*_Tmp14=t1;void*_Tmp15=Cyc_AssnDef_one();Cyc_Vcgen_vcgen_binop(_Tmp12,_Tmp13,0U,_Tmp14,_Tmp15,am2);});struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*t=_Tmp13;struct Cyc_AssnDef_AssnMap am=_Tmp12;
rvalue=t;
res=rvalue;
amout=am;
goto _LL93;}}case Cyc_Absyn_PostDec:  {
# 632
struct _tuple14 _Tmp11=({struct Cyc_Vcgen_Env*_Tmp12=env;void*_Tmp13=e->topt;void*_Tmp14=t1;void*_Tmp15=Cyc_AssnDef_one();Cyc_Vcgen_vcgen_binop(_Tmp12,_Tmp13,2U,_Tmp14,_Tmp15,am2);});struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*t=_Tmp13;struct Cyc_AssnDef_AssnMap am=_Tmp12;
res=t1;
rvalue=t;
amout=am;
goto _LL93;}}case Cyc_Absyn_PreDec:  {
# 638
struct _tuple14 _Tmp11=({struct Cyc_Vcgen_Env*_Tmp12=env;void*_Tmp13=e->topt;void*_Tmp14=t1;void*_Tmp15=Cyc_AssnDef_one();Cyc_Vcgen_vcgen_binop(_Tmp12,_Tmp13,2U,_Tmp14,_Tmp15,am2);});struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*t=_Tmp13;struct Cyc_AssnDef_AssnMap am=_Tmp12;
rvalue=t;
res=rvalue;
amout=am;
goto _LL93;}}default:
# 644
({int(*_Tmp11)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp11;})(_tag_fat("vcgen rexp increment_e",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));}_LL93:;
# 646
amout=Cyc_Vcgen_do_assign(env,amout,lt,rvalue);{
struct _tuple14 _Tmp11;_Tmp11.f0=res,_Tmp11.f1=amout;return _Tmp11;}}}}case 6: _Tmp9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp7;struct Cyc_Absyn_Exp*e3=_Tmp6;
# 649
struct _tuple19 _TmpB=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;struct Cyc_AssnDef_AssnMap _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct Cyc_AssnDef_AssnMap amt=_TmpD;struct Cyc_AssnDef_AssnMap amf=_TmpC;
struct _tuple14 _TmpE=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*t1=_Tmp10;struct Cyc_AssnDef_AssnMap amt=_TmpF;
struct _tuple14 _Tmp11=Cyc_Vcgen_vcgen_rexp(env,e3,amf);struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*t2=_Tmp13;struct Cyc_AssnDef_AssnMap amf=_Tmp12;
void*v=Cyc_AssnDef_fresh_var(e->topt);
amt=({struct Cyc_AssnDef_AssnMap _Tmp14=amt;Cyc_AssnDef_and_assnmap_assn(_Tmp14,Cyc_AssnDef_eq(v,t1));});
amf=({struct Cyc_AssnDef_AssnMap _Tmp14=amf;Cyc_AssnDef_and_assnmap_assn(_Tmp14,Cyc_AssnDef_eq(v,t2));});{
struct _tuple14 _Tmp14;_Tmp14.f0=v,({struct Cyc_AssnDef_AssnMap _Tmp15=({struct Cyc_Vcgen_Env*_Tmp16=env;Cyc_Vcgen_may_widen(_Tmp16,Cyc_AssnDef_or_assnmap_assnmap(amt,amf));});_Tmp14.f1=_Tmp15;});return _Tmp14;}}}}}case 7: _Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 657
struct _tuple19 _TmpB=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;struct Cyc_AssnDef_AssnMap _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct Cyc_AssnDef_AssnMap amt=_TmpD;struct Cyc_AssnDef_AssnMap amf=_TmpC;
struct _tuple14 _TmpE=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*t=_Tmp10;struct Cyc_AssnDef_AssnMap amt=_TmpF;
void*v=Cyc_AssnDef_fresh_var(e->topt);
amt=({struct Cyc_AssnDef_AssnMap _Tmp11=amt;Cyc_AssnDef_and_assnmap_assn(_Tmp11,Cyc_AssnDef_eq(v,t));});
amf=({struct Cyc_AssnDef_AssnMap _Tmp11=amf;Cyc_AssnDef_and_assnmap_assn(_Tmp11,({void*_Tmp12=v;Cyc_AssnDef_eq(_Tmp12,Cyc_AssnDef_zero());}));});{
struct _tuple14 _Tmp11;_Tmp11.f0=v,({struct Cyc_AssnDef_AssnMap _Tmp12=({struct Cyc_Vcgen_Env*_Tmp13=env;Cyc_Vcgen_may_widen(_Tmp13,Cyc_AssnDef_or_assnmap_assnmap(amt,amf));});_Tmp11.f1=_Tmp12;});return _Tmp11;}}}}case 8: _Tmp9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 664
struct _tuple19 _TmpB=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;struct Cyc_AssnDef_AssnMap _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct Cyc_AssnDef_AssnMap amt=_TmpD;struct Cyc_AssnDef_AssnMap amf=_TmpC;
struct _tuple14 _TmpE=Cyc_Vcgen_vcgen_rexp(env,e2,amf);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*t=_Tmp10;struct Cyc_AssnDef_AssnMap amf=_TmpF;
void*v=Cyc_AssnDef_fresh_var(e->topt);
amt=({struct Cyc_AssnDef_AssnMap _Tmp11=amt;Cyc_AssnDef_and_assnmap_assn(_Tmp11,({void*_Tmp12=v;Cyc_AssnDef_neq(_Tmp12,Cyc_AssnDef_zero());}));});
amf=({struct Cyc_AssnDef_AssnMap _Tmp11=amf;Cyc_AssnDef_and_assnmap_assn(_Tmp11,Cyc_AssnDef_eq(v,t));});{
struct _tuple14 _Tmp11;_Tmp11.f0=v,({struct Cyc_AssnDef_AssnMap _Tmp12=({struct Cyc_Vcgen_Env*_Tmp13=env;Cyc_Vcgen_may_widen(_Tmp13,Cyc_AssnDef_or_assnmap_assnmap(amt,amf));});_Tmp11.f1=_Tmp12;});return _Tmp11;}}}}case 9: _Tmp9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 671
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;_TmpC=_TmpB.f1;{struct Cyc_AssnDef_AssnMap am1=_TmpC;
return Cyc_Vcgen_vcgen_rexp(env,e2,am1);}}case 10: _Tmp9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_List_List*es=_Tmp7;
# 674
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*f=_TmpD;struct Cyc_AssnDef_AssnMap am1=_TmpC;
struct _tuple18 _TmpE=Cyc_Vcgen_vcgen_rexps(env,es,am1);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{struct Cyc_List_List*ts=_Tmp10;struct Cyc_AssnDef_AssnMap am2=_TmpF;
struct _tuple14 _Tmp11=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am2);struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*pre_memory=_Tmp13;struct Cyc_AssnDef_AssnMap am2=_Tmp12;
# 678
struct _tuple20 _Tmp14=Cyc_Vcgen_get_requires_and_ensures(e1->topt);void*_Tmp15;void*_Tmp16;void*_Tmp17;void*_Tmp18;void*_Tmp19;void*_Tmp1A;_Tmp1A=_Tmp14.f0;_Tmp19=_Tmp14.f1;_Tmp18=_Tmp14.f2;_Tmp17=_Tmp14.f3;_Tmp16=_Tmp14.f4;_Tmp15=_Tmp14.f5;{struct Cyc_List_List*args=_Tmp1A;struct Cyc_List_List*argvds=_Tmp19;struct Cyc_AssnDef_AssnMap*reqopt=_Tmp18;struct Cyc_AssnDef_AssnMap*ensopt=_Tmp17;struct Cyc_AssnDef_AssnMap*thrwsopt=_Tmp16;struct Cyc_Absyn_Vardecl*ret_value=_Tmp15;
# 681
void*actual_ret=Cyc_AssnDef_fresh_var(e->topt);
# 683
if(reqopt!=0){
# 685
void*req_assn=Cyc_AssnDef_subst_args(argvds,ret_value,ts,actual_ret,pre_memory,*reqopt);
if(!Cyc_Vcgen_implies(am2.assn,req_assn))
({struct Cyc_String_pa_PrintArg_struct _Tmp1B=({struct Cyc_String_pa_PrintArg_struct _Tmp1C;_Tmp1C.tag=0,({
struct _fat_ptr _Tmp1D=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(am2);_Tmp1C.f1=_Tmp1D;});_Tmp1C;});struct Cyc_String_pa_PrintArg_struct _Tmp1C=({struct Cyc_String_pa_PrintArg_struct _Tmp1D;_Tmp1D.tag=0,({struct _fat_ptr _Tmp1E=(struct _fat_ptr)Cyc_AssnDef_assn2string(req_assn);_Tmp1D.f1=_Tmp1E;});_Tmp1D;});void*_Tmp1D[2];_Tmp1D[0]=& _Tmp1B,_Tmp1D[1]=& _Tmp1C;Cyc_Warn_err(e->loc,_tag_fat("current assertion:\n%s\ndoes not satisfy the pre-condition of the function:\n%s\n",sizeof(char),78U),_tag_fat(_Tmp1D,sizeof(void*),2));});}{
# 694
void*new_memory=Cyc_AssnDef_fresh_var(0);
struct Cyc_AssnDef_AssnMap am3=Cyc_AssnDef_update_var_map(Cyc_AssnDef_memory,new_memory,am2);
# 698
if(thrwsopt!=0){
void*v=Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type());
void*throws_assn=({struct Cyc_List_List*_Tmp1B=argvds;struct Cyc_Absyn_Vardecl*_Tmp1C=Cyc_AssnDef_exception_vardecl();struct Cyc_List_List*_Tmp1D=ts;void*_Tmp1E=v;void*_Tmp1F=new_memory;Cyc_AssnDef_subst_args(_Tmp1B,_Tmp1C,_Tmp1D,_Tmp1E,_Tmp1F,*thrwsopt);});
({struct Cyc_Vcgen_Env*_Tmp1B=env;struct Cyc_AssnDef_AssnMap _Tmp1C=Cyc_AssnDef_and_assnmap_assn(am3,throws_assn);Cyc_Vcgen_update_try_assnmap(_Tmp1B,_Tmp1C,v);});}
# 704
if(ensopt!=0){
void*ens_assn=Cyc_AssnDef_subst_args(argvds,ret_value,ts,actual_ret,new_memory,*ensopt);
am3=Cyc_AssnDef_and_assnmap_assn(am3,ens_assn);}
# 710
if(Cyc_Atts_is_noreturn_fn_type(_check_null(e1->topt)))
am3.assn=(void*)& Cyc_AssnDef_false_assn;{
struct _tuple14 _Tmp1B;_Tmp1B.f0=actual_ret,_Tmp1B.f1=am3;return _Tmp1B;}}}}}}}case 11: _Tmp9=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 714
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t=_TmpD;struct Cyc_AssnDef_AssnMap am1=_TmpC;
Cyc_Vcgen_update_try_assnmap(env,am1,t);{
struct _tuple14 _TmpE;({void*_TmpF=Cyc_AssnDef_fresh_var(0);_TmpE.f0=_TmpF;}),({struct Cyc_AssnDef_AssnMap _TmpF=Cyc_AssnDef_false_assnmap();_TmpE.f1=_TmpF;});return _TmpE;}}}case 12: _Tmp9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
return Cyc_Vcgen_vcgen_rexp(env,e,amin);}case 13: _Tmp9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
return Cyc_Vcgen_vcgen_rexp(env,e,amin);}case 14: _Tmp9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*tp=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp7;enum Cyc_Absyn_Coercion c=_Tmp4;
# 723
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t=_TmpD;struct Cyc_AssnDef_AssnMap am=_TmpC;
# 730
{struct _tuple0 _TmpE=({struct _tuple0 _TmpF;({void*_Tmp10=Cyc_Absyn_compress(tp);_TmpF.f0=_Tmp10;}),({void*_Tmp10=Cyc_Absyn_compress(_check_null(e1->topt));_TmpF.f1=_Tmp10;});_TmpF;});struct Cyc_Absyn_PtrInfo _TmpF;struct Cyc_Absyn_PtrInfo _Tmp10;enum Cyc_Absyn_Size_of _Tmp11;enum Cyc_Absyn_Sign _Tmp12;enum Cyc_Absyn_Size_of _Tmp13;enum Cyc_Absyn_Sign _Tmp14;void*_Tmp15;switch(*((int*)_TmpE.f0)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f0)->f1)==1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f0)->f1)->f2){case Cyc_Absyn_Int_sz: if(*((int*)_TmpE.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f1)->f1)){case 5: _Tmp15=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f1)->f2;{struct Cyc_List_List*tagtps=_Tmp15;
# 734
struct _tuple14 _Tmp16;({void*_Tmp17=Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp((void*)_check_null(tagtps)->hd,e1->loc));_Tmp16.f0=_Tmp17;}),_Tmp16.f1=am;return _Tmp16;}case 1: goto _LLD9;default: goto _LLDD;}else{goto _LLDD;}case Cyc_Absyn_Long_sz: if(*((int*)_TmpE.f1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f1)->f1)){case 5: _Tmp15=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f1)->f2;{struct Cyc_List_List*tagtps=_Tmp15;
# 736
struct _tuple14 _Tmp16;({void*_Tmp17=Cyc_AssnDef_cnst(Cyc_Absyn_valueof_exp((void*)_check_null(tagtps)->hd,e1->loc));_Tmp16.f0=_Tmp17;}),_Tmp16.f1=am;return _Tmp16;}case 1: goto _LLD9;default: goto _LLDD;}else{goto _LLDD;}default: if(*((int*)_TmpE.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f1)->f1)==1){_LLD9: _Tmp14=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f0)->f1)->f1;_Tmp13=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f0)->f1)->f2;_Tmp12=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f1)->f1)->f1;_Tmp11=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TmpE.f1)->f1)->f2;{enum Cyc_Absyn_Sign u1=_Tmp14;enum Cyc_Absyn_Size_of s1=_Tmp13;enum Cyc_Absyn_Sign u2=_Tmp12;enum Cyc_Absyn_Size_of s2=_Tmp11;
# 743
s1=(int)s1==3?2U: s1;
s2=(int)s2==3?2U: s2;
if((int)s1==(int)s2){struct _tuple14 _Tmp16;_Tmp16.f0=t,_Tmp16.f1=am;return _Tmp16;}
goto _LLD4;}}else{goto _LLDD;}}else{goto _LLDD;}}else{goto _LLDD;}case 3: if(*((int*)_TmpE.f1)==3){_Tmp10=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpE.f0)->f1;_TmpF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TmpE.f1)->f1;{struct Cyc_Absyn_PtrInfo p2=_Tmp10;struct Cyc_Absyn_PtrInfo p1=_TmpF;
# 748
int n1=Cyc_Tcutil_force_type2bool(0,p1.ptr_atts.nullable);
int n2=Cyc_Tcutil_force_type2bool(0,p2.ptr_atts.nullable);
struct Cyc_Absyn_Exp*b1=({void*_Tmp16=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp16,p1.ptr_atts.bounds);});
struct Cyc_Absyn_Exp*b2=({void*_Tmp16=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_Tmp16,p2.ptr_atts.bounds);});
struct _tuple24 _Tmp16=({struct _tuple24 _Tmp17;_Tmp17.f0=b1,_Tmp17.f1=b2;_Tmp17;});if(_Tmp16.f0!=0){if(_Tmp16.f1!=0){if(!env->pure_exp){
# 757
void*null_check=({void*_Tmp17=Cyc_AssnDef_zero();Cyc_AssnDef_neq(_Tmp17,t);});
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks)),exp_checks->ctxt=am,exp_checks->bounds_check=(void*)& Cyc_AssnDef_true_assn,exp_checks->null_check=null_check;
# 760
Cyc_Vcgen_insert_exp_checks(env->assn_info,e1,exp_checks);
({struct Cyc_Vcgen_Env*_Tmp17=env;struct Cyc_AssnDef_AssnMap _Tmp18=({struct Cyc_AssnDef_AssnMap _Tmp19=am;Cyc_AssnDef_and_assnmap_assn(_Tmp19,({void*_Tmp1A=t;Cyc_AssnDef_eq(_Tmp1A,Cyc_AssnDef_zero());}));});Cyc_Vcgen_update_try_assnmap(_Tmp17,_Tmp18,Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type()));});{
struct _tuple14 _Tmp17;_Tmp17.f0=t,({struct Cyc_AssnDef_AssnMap _Tmp18=Cyc_AssnDef_and_assnmap_assn(am,null_check);_Tmp17.f1=_Tmp18;});return _Tmp17;}}else{goto _LLE6;}}else{
# 764
struct _tuple14 _Tmp17;_Tmp17.f0=t,_Tmp17.f1=am;return _Tmp17;}}else{if(_Tmp16.f1!=0){if(!env->pure_exp){
# 768
void*null_check=(env->pure_exp || n2)?(void*)& Cyc_AssnDef_true_assn:({
void*_Tmp17=Cyc_AssnDef_zero();Cyc_AssnDef_neq(_Tmp17,t);});
struct _tuple14 _Tmp17=Cyc_Vcgen_vcgen_rexp(env,_check_null(b2),am);void*_Tmp18;_Tmp18=_Tmp17.f0;{void*b=_Tmp18;
void*bcheck=({void*_Tmp19=b;Cyc_AssnDef_ulte(_Tmp19,Cyc_AssnDef_numelts_term(t));});
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks)),exp_checks->ctxt=am,exp_checks->bounds_check=bcheck,exp_checks->null_check=null_check;
# 774
Cyc_Vcgen_insert_exp_checks(env->assn_info,e1,exp_checks);{
void*checks=Cyc_AssnDef_and(null_check,bcheck);
({struct Cyc_Vcgen_Env*_Tmp19=env;struct Cyc_AssnDef_AssnMap _Tmp1A=({struct Cyc_AssnDef_AssnMap _Tmp1B=am;Cyc_AssnDef_and_assnmap_assn(_Tmp1B,Cyc_AssnDef_not(checks));});Cyc_Vcgen_update_try_assnmap(_Tmp19,_Tmp1A,Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type()));});{
struct _tuple14 _Tmp19;_Tmp19.f0=t,({struct Cyc_AssnDef_AssnMap _Tmp1A=Cyc_AssnDef_and_assnmap_assn(am,checks);_Tmp19.f1=_Tmp1A;});return _Tmp19;}}}}else{goto _LLE6;}}else{_LLE6: {
# 779
struct _tuple14 _Tmp17;_Tmp17.f0=t,_Tmp17.f1=am;return _Tmp17;}}};}}else{goto _LLDD;}default: _LLDD:
# 781
 goto _LLD4;}_LLD4:;}
# 783
if(env->pure_exp ||(int)c==1){
struct _tuple14 _TmpE;({void*_TmpF=Cyc_AssnDef_cast(tp,t);_TmpE.f0=_TmpF;}),_TmpE.f1=am;return _TmpE;}else{
# 787
({struct Cyc_Vcgen_Env*_TmpE=env;struct Cyc_AssnDef_AssnMap _TmpF=amin;Cyc_Vcgen_update_try_assnmap(_TmpE,_TmpF,Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type()));});{
struct _tuple14 _TmpE;_TmpE.f0=t,_TmpE.f1=am;return _TmpE;}}}}case 16: _Tmp9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*eopt=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp*qopt=_Tmp6;
# 791
if(eopt!=0)
amin=Cyc_Vcgen_vcgen_rexp(env,eopt,amin).f1;
if(qopt!=0)
amin=Cyc_Vcgen_vcgen_rexp(env,qopt,amin).f1;{
# 796
void*_TmpB=e1->r;void*_TmpC;void*_TmpD;void*_TmpE;switch(*((int*)_TmpB)){case 25: _TmpE=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_TmpB)->f1;{struct Cyc_List_List*dles=_TmpE;
# 801
struct _tuple14 _TmpF=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _Tmp10;_Tmp10=_TmpF.f1;{struct Cyc_AssnDef_AssnMap amin=_Tmp10;
void*size=Cyc_AssnDef_uint((unsigned)Cyc_List_length(dles));
void*res=({struct Cyc_Absyn_Exp*_Tmp11=e;void*_Tmp12=Cyc_AssnDef_uint((unsigned)Cyc_List_length(dles));Cyc_AssnDef_alloc(_Tmp11,_Tmp12,e->topt);});
amin=({struct Cyc_AssnDef_AssnMap _Tmp11=amin;Cyc_AssnDef_and_assnmap_assn(_Tmp11,({void*_Tmp12=({void*_Tmp13=res;Cyc_AssnDef_neq(_Tmp13,Cyc_AssnDef_zero());});Cyc_AssnDef_and(_Tmp12,({
void*_Tmp13=Cyc_AssnDef_numelts_term(res);Cyc_AssnDef_eq(_Tmp13,size);}));}));});{
# 808
struct _tuple14 _Tmp11;_Tmp11.f0=res,_Tmp11.f1=amin;return _Tmp11;}}}case 26: _TmpE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TmpB)->f1;_TmpD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TmpB)->f2;_TmpC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TmpB)->f3;{struct Cyc_Absyn_Vardecl*vd=_TmpE;struct Cyc_Absyn_Exp*e1=_TmpD;struct Cyc_Absyn_Exp*e2=_TmpC;
# 810
struct _tuple14 _TmpF=Cyc_Vcgen_vcgen_comprehension(env,vd,e1,e2,_check_null(e->topt),amin);struct Cyc_AssnDef_AssnMap _Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*res=_Tmp11;struct Cyc_AssnDef_AssnMap amin=_Tmp10;
amin=({struct Cyc_AssnDef_AssnMap _Tmp12=amin;Cyc_AssnDef_and_assnmap_assn(_Tmp12,({void*_Tmp13=res;Cyc_AssnDef_neq(_Tmp13,Cyc_AssnDef_zero());}));});{
struct _tuple14 _Tmp12;_Tmp12.f0=res,_Tmp12.f1=amin;return _Tmp12;}}}default:  {
# 814
struct _tuple14 _TmpF=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*t=_Tmp11;struct Cyc_AssnDef_AssnMap amin=_Tmp10;
void*res=({struct Cyc_Absyn_Exp*_Tmp12=e;void*_Tmp13=Cyc_AssnDef_one();Cyc_AssnDef_alloc(_Tmp12,_Tmp13,e->topt);});
struct _tuple14 _Tmp12=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _Tmp13;void*_Tmp14;_Tmp14=_Tmp12.f0;_Tmp13=_Tmp12.f1;{void*mem=_Tmp14;struct Cyc_AssnDef_AssnMap amin=_Tmp13;
amin=({struct Cyc_AssnDef_AssnMap _Tmp15=amin;Cyc_AssnDef_and_assnmap_assn(_Tmp15,({void*_Tmp16=({void*_Tmp17=Cyc_AssnDef_select(mem,res,e1->topt);Cyc_AssnDef_eq(_Tmp17,t);});Cyc_AssnDef_and(_Tmp16,({
void*_Tmp17=res;Cyc_AssnDef_neq(_Tmp17,Cyc_AssnDef_zero());}));}));});{
struct _tuple14 _Tmp15;_Tmp15.f0=res,_Tmp15.f1=amin;return _Tmp15;}}}}};}}case 31:
# 821
 goto _LL3A;case 32: _LL3A:
 goto _LL3C;case 17: _LL3C:
 goto _LL3E;case 18: _LL3E:
 goto _LL40;case 38: _LL40:
 goto _LL42;case 19: _LL42: {
struct _tuple14 _TmpB;({void*_TmpC=Cyc_AssnDef_cnst(e);_TmpB.f0=_TmpC;}),_TmpB.f1=amin;return _TmpB;}case 21: _Tmp9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*f=_Tmp7;int is_tagged=_Tmp3;int is_read=_Tmp2;
# 828
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t=_TmpD;struct Cyc_AssnDef_AssnMap amin=_TmpC;
int i=Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),f);
# 831
if((!env->pure_exp && is_tagged)&& is_read){
# 833
void*ck=({void*_TmpE=Cyc_AssnDef_tagof_tm(t);Cyc_AssnDef_eq(_TmpE,Cyc_AssnDef_uint((unsigned)i));});
({struct Cyc_Vcgen_Env*_TmpE=env;struct Cyc_AssnDef_AssnMap _TmpF=({struct Cyc_AssnDef_AssnMap _Tmp10=amin;Cyc_AssnDef_and_assnmap_assn(_Tmp10,Cyc_AssnDef_not(ck));});Cyc_Vcgen_update_try_assnmap(_TmpE,_TmpF,Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type()));});
amin=Cyc_AssnDef_and_assnmap_assn(amin,ck);}{
# 837
struct _tuple14 _TmpE;({void*_TmpF=Cyc_AssnDef_proj(t,(unsigned)i,e->topt);_TmpE.f0=_TmpF;}),_TmpE.f1=amin;return _TmpE;}}}case 37: _Tmp9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*f=_Tmp7;
# 840
int i=Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),f);
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t=_TmpD;struct Cyc_AssnDef_AssnMap am=_TmpC;
struct Cyc_Vcgen_Env*_TmpE=env;void*_TmpF=Cyc_Absyn_sint_type;void*_Tmp10=Cyc_AssnDef_tagof_tm(t);void*_Tmp11=Cyc_AssnDef_uint((unsigned)i);return Cyc_Vcgen_vcgen_binop(_TmpE,_TmpF,5U,_Tmp10,_Tmp11,am);}}case 15: _Tmp9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 844
return Cyc_Vcgen_vcgen_lexp(env,e1,amin);}case 20: _Tmp9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 846
struct _tuple23 _TmpB=({struct Cyc_Vcgen_Env*_TmpC=env;struct Cyc_Absyn_Exp*_TmpD=e1;struct Cyc_Absyn_Exp*_TmpE=Cyc_Vcgen_zero_exp();Cyc_Vcgen_vcgen_deref(_TmpC,_TmpD,_TmpE,amin);});struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f2;{void*t1=_TmpD;struct Cyc_AssnDef_AssnMap amin=_TmpC;
struct _tuple14 _TmpE=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*mem=_Tmp10;struct Cyc_AssnDef_AssnMap amin=_TmpF;
struct _tuple14 _Tmp11;({void*_Tmp12=Cyc_AssnDef_select(mem,t1,e->topt);_Tmp11.f0=_Tmp12;}),_Tmp11.f1=amin;return _Tmp11;}}}case 22: _Tmp9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct _fat_ptr*f=_Tmp7;int is_tagged=_Tmp3;int is_read=_Tmp2;
# 850
struct _tuple23 _TmpB=({struct Cyc_Vcgen_Env*_TmpC=env;struct Cyc_Absyn_Exp*_TmpD=e1;struct Cyc_Absyn_Exp*_TmpE=Cyc_Vcgen_zero_exp();Cyc_Vcgen_vcgen_deref(_TmpC,_TmpD,_TmpE,amin);});struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f2;{void*t1=_TmpD;struct Cyc_AssnDef_AssnMap amin=_TmpC;
int i=({void*_TmpE=Cyc_Tcutil_pointer_elt_type(_check_null(e1->topt));Cyc_CfFlowInfo_get_field_index(_TmpE,f);});
struct _tuple14 _TmpE=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*mem=_Tmp10;struct Cyc_AssnDef_AssnMap amin=_TmpF;
if((!env->pure_exp && is_tagged)&& is_read){
void*ck=({void*_Tmp11=Cyc_AssnDef_tagof_tm(Cyc_AssnDef_select(mem,t1,0));Cyc_AssnDef_eq(_Tmp11,
Cyc_AssnDef_uint((unsigned)i));});
({struct Cyc_Vcgen_Env*_Tmp11=env;struct Cyc_AssnDef_AssnMap _Tmp12=({struct Cyc_AssnDef_AssnMap _Tmp13=amin;Cyc_AssnDef_and_assnmap_assn(_Tmp13,Cyc_AssnDef_not(ck));});Cyc_Vcgen_update_try_assnmap(_Tmp11,_Tmp12,Cyc_AssnDef_fresh_var(Cyc_Absyn_exn_type()));});
amin=Cyc_AssnDef_and_assnmap_assn(amin,ck);}{
# 859
void*elt_type=Cyc_Tcutil_pointer_elt_type(_check_null(e1->topt));
struct _tuple14 _Tmp11;({void*_Tmp12=({void*_Tmp13=Cyc_AssnDef_select(mem,t1,elt_type);unsigned _Tmp14=(unsigned)i;Cyc_AssnDef_proj(_Tmp13,_Tmp14,e->topt);});_Tmp11.f0=_Tmp12;}),_Tmp11.f1=amin;return _Tmp11;}}}}case 23: _Tmp9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 862
struct _tuple23 _TmpB=Cyc_Vcgen_vcgen_deref(env,e1,e2,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;void*_TmpE;_TmpE=_TmpB.f0;_TmpD=_TmpB.f1;_TmpC=_TmpB.f2;{void*t1=_TmpE;void*t2=_TmpD;struct Cyc_AssnDef_AssnMap amin=_TmpC;
struct _tuple14 _TmpF=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _Tmp10;void*_Tmp11;_Tmp11=_TmpF.f0;_Tmp10=_TmpF.f1;{void*mem=_Tmp11;struct Cyc_AssnDef_AssnMap amin=_Tmp10;
struct _tuple14 _Tmp12;({void*_Tmp13=({void*_Tmp14=mem;void*_Tmp15=Cyc_AssnDef_plus(t1,t2,0);Cyc_AssnDef_select(_Tmp14,_Tmp15,e->topt);});_Tmp12.f0=_Tmp13;}),_Tmp12.f1=amin;return _Tmp12;}}}case 30: _Tmp9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*es=_Tmp9;struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;struct Cyc_Absyn_Datatypefield*df=_Tmp6;
# 866
struct _tuple18 _TmpB=Cyc_Vcgen_vcgen_rexps(env,es,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct Cyc_List_List*ts=_TmpD;struct Cyc_AssnDef_AssnMap amin=_TmpC;
unsigned i=Cyc_Vcgen_datatype_constructor_index(dd,df);
struct _tuple14 _TmpE;({void*_TmpF=Cyc_AssnDef_datatype_aggr(i,ts,e->topt);_TmpE.f0=_TmpF;}),_TmpE.f1=amin;return _TmpE;}}case 24: _Tmp9=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_Tmp9;
# 871
_Tmp9=dles;goto _LL54;}case 25: _Tmp9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL54: {struct Cyc_List_List*dles=_Tmp9;
_Tmp9=dles;goto _LL56;}case 35: _Tmp9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL56: {struct Cyc_List_List*dles=_Tmp9;
# 874
for(1;dles!=0;dles=dles->tl){
struct _tuple25*_TmpB=(struct _tuple25*)dles->hd;void*_TmpC;_TmpC=_TmpB->f1;{struct Cyc_Absyn_Exp*e=_TmpC;
amin=Cyc_Vcgen_vcgen_rexp(env,e,amin).f1;}}{
# 878
struct _tuple14 _TmpB;({void*_TmpC=Cyc_AssnDef_fresh_var(0);_TmpB.f0=_TmpC;}),_TmpB.f1=amin;return _TmpB;}}case 28: _Tmp9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*dles=_Tmp9;
_Tmp9=dles;goto _LL5A;}case 29: _Tmp9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LL5A: {struct Cyc_List_List*dles=_Tmp9;
# 881
void*atype=_check_null(e->topt);
struct _tuple21 _TmpB=Cyc_Vcgen_get_aggr_info(atype);enum Cyc_Absyn_AggrKind _TmpC;int _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{int is_tagged=_TmpD;enum Cyc_Absyn_AggrKind ak=_TmpC;
struct Cyc_List_List*dlvs=0;
unsigned tag=0U;
for(1;dles!=0;dles=dles->tl){
struct _tuple25*_TmpE=(struct _tuple25*)dles->hd;void*_TmpF;void*_Tmp10;_Tmp10=_TmpE->f0;_TmpF=_TmpE->f1;{struct Cyc_List_List*dl=_Tmp10;struct Cyc_Absyn_Exp*e=_TmpF;
struct _tuple14 _Tmp11=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*v=_Tmp13;struct Cyc_AssnDef_AssnMap a=_Tmp12;
amin=a;{
void*_Tmp14;if(dl!=0){if(*((int*)((struct Cyc_List_List*)dl)->hd)==1){if(((struct Cyc_List_List*)dl)->tl==0){_Tmp14=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)dl->hd)->f1;{struct _fat_ptr*f=_Tmp14;
# 891
int i=Cyc_CfFlowInfo_get_field_index(atype,f);
tag=(unsigned)i;
dlvs=({struct Cyc_List_List*_Tmp15=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple22*_Tmp16=({struct _tuple22*_Tmp17=_cycalloc(sizeof(struct _tuple22));_Tmp17->f0=i,_Tmp17->f1=v;_Tmp17;});_Tmp15->hd=_Tmp16;}),_Tmp15->tl=dlvs;_Tmp15;});
goto _LL125;}}else{goto _LL128;}}else{goto _LL128;}}else{_LL128:
({int(*_Tmp15)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp15;})(_tag_fat("no field name in designator!",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}_LL125:;}}}}
# 898
dlvs=({struct Cyc_List_List*(*_TmpE)(int(*)(struct _tuple22*,struct _tuple22*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple22*,struct _tuple22*),struct Cyc_List_List*))Cyc_List_rimp_merge_sort;_TmpE;})(Cyc_Vcgen_cmp_index,dlvs);{
struct Cyc_List_List*vs=({struct Cyc_List_List*(*_TmpE)(void*(*)(struct _tuple22*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple22*),struct Cyc_List_List*))Cyc_List_map;_TmpE;})(({void*(*_TmpE)(struct _tuple22*)=(void*(*)(struct _tuple22*))Cyc_Core_snd;_TmpE;}),dlvs);
void*res=(int)ak==1?Cyc_AssnDef_union_aggr(tag,vs,e->topt):
 Cyc_AssnDef_struct_aggr(vs,e->topt);
struct _tuple14 _TmpE;_TmpE.f0=res,_TmpE.f1=amin;return _TmpE;}}}case 26: _Tmp9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp9;struct Cyc_Absyn_Exp*e1=_Tmp7;struct Cyc_Absyn_Exp*e2=_Tmp6;
# 904
return Cyc_Vcgen_vcgen_comprehension(env,vd,e1,e2,_check_null(e->topt),amin);}case 27: _Tmp9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 906
struct _tuple14 _TmpB;({void*_TmpC=Cyc_AssnDef_fresh_var(0);_TmpB.f0=_TmpC;}),({struct Cyc_AssnDef_AssnMap _TmpC=Cyc_Vcgen_vcgen_rexp(env,e,amin).f1;_TmpB.f1=_TmpC;});return _TmpB;}case 33: _Tmp1=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_MallocInfo mi=_Tmp1;
# 908
if(mi.rgn!=0)
amin=Cyc_Vcgen_vcgen_rexp(env,mi.rgn,amin).f1;{
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_rexp(env,mi.num_elts,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t=_TmpD;struct Cyc_AssnDef_AssnMap am1=_TmpC;
void*res=Cyc_AssnDef_alloc(e,t,e->topt);
struct Cyc_AssnDef_AssnMap am2=({struct Cyc_AssnDef_AssnMap _TmpE=am1;Cyc_AssnDef_and_assnmap_assn(_TmpE,({void*_TmpF=({void*_Tmp10=res;Cyc_AssnDef_neq(_Tmp10,Cyc_AssnDef_zero());});Cyc_AssnDef_and(_TmpF,({void*_Tmp10=Cyc_AssnDef_numelts_term(res);Cyc_AssnDef_eq(_Tmp10,t);}));}));});
struct _tuple14 _TmpE;_TmpE.f0=res,_TmpE.f1=am2;return _TmpE;}}}case 34: _Tmp9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp9;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 916
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_lexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*lt1=_TmpD;struct Cyc_AssnDef_AssnMap am1=_TmpC;
struct _tuple14 _TmpE=Cyc_Vcgen_vcgen_lexp(env,e2,am1);struct Cyc_AssnDef_AssnMap _TmpF;void*_Tmp10;_Tmp10=_TmpE.f0;_TmpF=_TmpE.f1;{void*lt2=_Tmp10;struct Cyc_AssnDef_AssnMap am2=_TmpF;
struct _tuple14 _Tmp11=Cyc_Vcgen_deref_lterm(env,lt1,am2);struct Cyc_AssnDef_AssnMap _Tmp12;void*_Tmp13;_Tmp13=_Tmp11.f0;_Tmp12=_Tmp11.f1;{void*t1=_Tmp13;struct Cyc_AssnDef_AssnMap am3=_Tmp12;
struct _tuple14 _Tmp14=Cyc_Vcgen_deref_lterm(env,lt2,am3);struct Cyc_AssnDef_AssnMap _Tmp15;void*_Tmp16;_Tmp16=_Tmp14.f0;_Tmp15=_Tmp14.f1;{void*t2=_Tmp16;struct Cyc_AssnDef_AssnMap am4=_Tmp15;
struct Cyc_AssnDef_AssnMap am5=Cyc_Vcgen_do_assign(env,am4,lt1,t2);
struct Cyc_AssnDef_AssnMap am6=Cyc_Vcgen_do_assign(env,am5,lt2,t1);
struct _tuple14 _Tmp17;({void*_Tmp18=Cyc_AssnDef_fresh_var(0);_Tmp17.f0=_Tmp18;}),_Tmp17.f1=am6;return _Tmp17;}}}}}case 36: _Tmp9=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp9;
# 924
void**old_exp_stmt=env->exp_stmt;
({void**_TmpB=({void**_TmpC=_cycalloc(sizeof(void*));({void*_TmpD=Cyc_AssnDef_fresh_var(0);*_TmpC=_TmpD;});_TmpC;});env->exp_stmt=_TmpB;});{
struct Cyc_AssnDef_AssnMap amout=Cyc_Vcgen_vcgen_stmt(env,s,amin);
void*t=*_check_null(env->exp_stmt);
env->exp_stmt=old_exp_stmt;{
struct _tuple14 _TmpB;_TmpB.f0=t,_TmpB.f1=amout;return _TmpB;}}}case 39:  {
struct _tuple14 _TmpB;({void*_TmpC=Cyc_AssnDef_fresh_var(0);_TmpB.f0=_TmpC;}),_TmpB.f1=amin;return _TmpB;}case 40: _Tmp9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
return Cyc_Vcgen_vcgen_rexp(env,e,amin);}case 41: _Tmp9=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 933
int old_widen_paths=env->widen_paths;
int old_pure_exp=env->pure_exp;
env->widen_paths=0;
env->pure_exp=1;{
struct _tuple19 _TmpB=({struct Cyc_Vcgen_Env*_TmpC=env;struct Cyc_Absyn_Exp*_TmpD=e1;Cyc_Vcgen_vcgen_test(_TmpC,_TmpD,({struct Cyc_AssnDef_AssnMap _TmpE;_TmpE.assn=(void*)& Cyc_AssnDef_true_assn,_TmpE.map=amin.map;_TmpE;}));});struct Cyc_AssnDef_AssnMap _TmpC;_TmpC=_TmpB.f0;{struct Cyc_AssnDef_AssnMap amt=_TmpC;
env->widen_paths=old_widen_paths;
env->pure_exp=old_pure_exp;
if(!Cyc_Vcgen_implies(amin.assn,amt.assn))
({struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,({
struct _fat_ptr _TmpF=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amt);_TmpE.f1=_TmpF;});_TmpE;});struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({struct _fat_ptr _Tmp10=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_TmpF.f1=_Tmp10;});_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;Cyc_Warn_err(e->loc,_tag_fat("cannot prove @assert(%s)\nfrom\n %s",sizeof(char),34U),_tag_fat(_TmpF,sizeof(void*),2));});{
struct _tuple14 _TmpD;({void*_TmpE=Cyc_AssnDef_one();_TmpD.f0=_TmpE;}),({struct Cyc_AssnDef_AssnMap _TmpE=Cyc_AssnDef_and_assnmap_assn(amin,amt.assn);_TmpD.f1=_TmpE;});return _TmpD;}}}}default: _Tmp9=((struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp9;
# 945
int old_widen_paths=env->widen_paths;
int old_pure_exp=env->pure_exp;
env->widen_paths=0;
env->pure_exp=1;{
struct _tuple19 _TmpB=({struct Cyc_Vcgen_Env*_TmpC=env;struct Cyc_Absyn_Exp*_TmpD=e1;Cyc_Vcgen_vcgen_test(_TmpC,_TmpD,({struct Cyc_AssnDef_AssnMap _TmpE;_TmpE.assn=(void*)& Cyc_AssnDef_true_assn,_TmpE.map=amin.map;_TmpE;}));});struct Cyc_AssnDef_AssnMap _TmpC;_TmpC=_TmpB.f0;{struct Cyc_AssnDef_AssnMap amt=_TmpC;
env->widen_paths=old_widen_paths;
env->pure_exp=old_pure_exp;
if(Cyc_Vcgen_implies(amin.assn,amt.assn))
({struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,({
struct _fat_ptr _TmpF=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amt);_TmpE.f1=_TmpF;});_TmpE;});struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({struct _fat_ptr _Tmp10=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_TmpF.f1=_Tmp10;});_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;Cyc_Warn_err(e->loc,_tag_fat("proved @assert_false(%s)\nfrom\n %s",sizeof(char),34U),_tag_fat(_TmpF,sizeof(void*),2));});{
struct _tuple14 _TmpD;({void*_TmpE=Cyc_AssnDef_one();_TmpD.f0=_TmpE;}),_TmpD.f1=amin;return _TmpD;}}}}};}}
# 959
static struct _tuple18 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,struct Cyc_AssnDef_AssnMap amin){
struct Cyc_List_List*result=0;
for(1;es!=0;es=es->tl){
struct _tuple14 _Tmp0=Cyc_Vcgen_vcgen_rexp(env,(struct Cyc_Absyn_Exp*)es->hd,amin);struct Cyc_AssnDef_AssnMap _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{void*t=_Tmp2;struct Cyc_AssnDef_AssnMap am=_Tmp1;
result=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=t,_Tmp3->tl=result;_Tmp3;});
amin=am;}}{
# 966
struct _tuple18 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(result);_Tmp0.f0=_Tmp1;}),_Tmp0.f1=amin;return _Tmp0;}}
# 972
static struct _tuple14 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap amin){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)){case 1: _Tmp2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
_Tmp2=vd;goto _LL4;}case 4: _Tmp2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 977
void*_Tmp3=Cyc_Absyn_compress(vd->type);if(*((int*)_Tmp3)==4)
({int(*_Tmp4)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp4;})(_tag_fat("array type var is not a valid l-value",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));else{
struct _tuple14 _Tmp4;({void*_Tmp5=Cyc_AssnDef_addr(vd);_Tmp4.f0=_Tmp5;}),_Tmp4.f1=amin;return _Tmp4;};}case 3: _Tmp2=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 981
_Tmp2=vd;goto _LL8;}case 5: _Tmp2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1;_LL8: {struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 985
struct _tuple14 _Tmp3;({void*_Tmp4=Cyc_AssnDef_addr(vd);_Tmp3.f0=_Tmp4;}),_Tmp3.f1=amin;return _Tmp3;}default: goto _LL15;}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
# 990
struct _tuple23 _Tmp3=({struct Cyc_Vcgen_Env*_Tmp4=env;struct Cyc_Absyn_Exp*_Tmp5=e1;struct Cyc_Absyn_Exp*_Tmp6=Cyc_Vcgen_zero_exp();Cyc_Vcgen_vcgen_deref(_Tmp4,_Tmp5,_Tmp6,amin);});struct Cyc_AssnDef_AssnMap _Tmp4;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp3.f0;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{void*t1=_Tmp6;void*t2=_Tmp5;struct Cyc_AssnDef_AssnMap am1=_Tmp4;
struct _tuple14 _Tmp7;_Tmp7.f0=t1,_Tmp7.f1=am1;return _Tmp7;}}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 995
struct _tuple23 _Tmp3=Cyc_Vcgen_vcgen_deref(env,e1,e2,amin);struct Cyc_AssnDef_AssnMap _Tmp4;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp3.f0;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{void*t1=_Tmp6;void*t2=_Tmp5;struct Cyc_AssnDef_AssnMap am1=_Tmp4;
struct _tuple14 _Tmp7;({void*_Tmp8=Cyc_AssnDef_offseti(t1,t2,e1->topt);_Tmp7.f0=_Tmp8;}),_Tmp7.f1=am1;return _Tmp7;}}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 999
struct _tuple23 _Tmp3=({struct Cyc_Vcgen_Env*_Tmp4=env;struct Cyc_Absyn_Exp*_Tmp5=e1;struct Cyc_Absyn_Exp*_Tmp6=Cyc_Vcgen_zero_exp();Cyc_Vcgen_vcgen_deref(_Tmp4,_Tmp5,_Tmp6,amin);});struct Cyc_AssnDef_AssnMap _Tmp4;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp3.f0;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{void*t1=_Tmp6;void*t2=_Tmp5;struct Cyc_AssnDef_AssnMap am1=_Tmp4;
int i=({void*_Tmp7=Cyc_Tcutil_pointer_elt_type(_check_null(e1->topt));Cyc_CfFlowInfo_get_field_index(_Tmp7,f);});
# 1002
void*tp=({void*_Tmp7=_check_null(e->topt);void*_Tmp8=Cyc_Absyn_heap_rgn_type;void*_Tmp9=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _TmpA=Cyc_Absyn_empty_tqual(0U);void*_TmpB=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,_TmpB,Cyc_Absyn_false_type);});
struct _tuple14 _Tmp7;({void*_Tmp8=Cyc_AssnDef_offsetf(t1,(unsigned)i,tp);_Tmp7.f0=_Tmp8;}),_Tmp7.f1=am1;return _Tmp7;}}case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 1005
struct _tuple14 _Tmp3=Cyc_Vcgen_vcgen_lexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{void*t1=_Tmp5;struct Cyc_AssnDef_AssnMap am1=_Tmp4;
int i=Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),f);
# 1008
void*tp=({void*_Tmp6=_check_null(e->topt);void*_Tmp7=Cyc_Absyn_heap_rgn_type;void*_Tmp8=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp9=Cyc_Absyn_empty_tqual(0U);void*_TmpA=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Absyn_false_type);});
struct _tuple14 _Tmp6;({void*_Tmp7=Cyc_AssnDef_offsetf(t1,(unsigned)i,tp);_Tmp6.f0=_Tmp7;}),_Tmp6.f1=am1;return _Tmp6;}}case 13: _Tmp2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
_Tmp2=e1;goto _LL14;}case 12: _Tmp2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp2;
# 1012
return Cyc_Vcgen_vcgen_lexp(env,e1,amin);}default: _LL15: {
# 1015
void*tp=({void*_Tmp3=_check_null(e->topt);void*_Tmp4=Cyc_Absyn_heap_rgn_type;void*_Tmp5=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp3,_Tmp4,_Tmp5,_Tmp6,_Tmp7,Cyc_Absyn_false_type);});
struct _tuple14 _Tmp3;({void*_Tmp4=Cyc_AssnDef_fresh_var(tp);_Tmp3.f0=_Tmp4;}),_Tmp3.f1=amin;return _Tmp3;}};}
# 1022
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Absyn_compress(_check_null(e1->topt));
void*t2=Cyc_Absyn_compress(_check_null(e2->topt));
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});if(*((int*)_Tmp0.f0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f0)->f1)->f1==Cyc_Absyn_Unsigned)
return 1;else{if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1==Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1==Cyc_Absyn_Unsigned)goto _LL3;else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}}else{if(*((int*)_Tmp0.f1)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0.f1)->f1)->f1==Cyc_Absyn_Unsigned){_LL3:
 return 1;}else{if(*((int*)_Tmp0.f0)==3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_Tmp0.f0)==3)goto _LL7;else{goto _LL7;}}}else{if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3)
return 1;else{goto _LL7;}}else{_LL7:
 return 0;}}};}
# 1040 "vcgen.cyc"
static struct _tuple19 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap ain){
void*_Tmp0=e->r;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 1043
struct _tuple19 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _Tmp6;struct Cyc_AssnDef_AssnMap _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_AssnDef_AssnMap a1t=_Tmp7;struct Cyc_AssnDef_AssnMap a1f=_Tmp6;
struct _tuple19 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct Cyc_AssnDef_AssnMap _Tmp9;struct Cyc_AssnDef_AssnMap _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{struct Cyc_AssnDef_AssnMap a2t=_TmpA;struct Cyc_AssnDef_AssnMap a2f=_Tmp9;
struct _tuple19 _TmpB=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct Cyc_AssnDef_AssnMap _TmpC;struct Cyc_AssnDef_AssnMap _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct Cyc_AssnDef_AssnMap a3t=_TmpD;struct Cyc_AssnDef_AssnMap a3f=_TmpC;
struct _tuple19 _TmpE;({struct Cyc_AssnDef_AssnMap _TmpF=({struct Cyc_Vcgen_Env*_Tmp10=env;Cyc_Vcgen_may_widen(_Tmp10,Cyc_AssnDef_or_assnmap_assnmap(a2t,a3t));});_TmpE.f0=_TmpF;}),({
struct Cyc_AssnDef_AssnMap _TmpF=({struct Cyc_Vcgen_Env*_Tmp10=env;Cyc_Vcgen_may_widen(_Tmp10,Cyc_AssnDef_or_assnmap_assnmap(a2f,a3f));});_TmpE.f1=_TmpF;});return _TmpE;}}}}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1049
struct _tuple19 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _Tmp6;struct Cyc_AssnDef_AssnMap _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_AssnDef_AssnMap a1t=_Tmp7;struct Cyc_AssnDef_AssnMap a1f=_Tmp6;
struct _tuple19 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct Cyc_AssnDef_AssnMap _Tmp9;struct Cyc_AssnDef_AssnMap _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{struct Cyc_AssnDef_AssnMap a2t=_TmpA;struct Cyc_AssnDef_AssnMap a2f=_Tmp9;
struct _tuple19 _TmpB;_TmpB.f0=a2t,({struct Cyc_AssnDef_AssnMap _TmpC=({struct Cyc_Vcgen_Env*_TmpD=env;Cyc_Vcgen_may_widen(_TmpD,Cyc_AssnDef_or_assnmap_assnmap(a1f,a2f));});_TmpB.f1=_TmpC;});return _TmpB;}}}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1053
struct _tuple19 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _Tmp6;struct Cyc_AssnDef_AssnMap _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_AssnDef_AssnMap a1t=_Tmp7;struct Cyc_AssnDef_AssnMap a1f=_Tmp6;
struct _tuple19 _Tmp8=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct Cyc_AssnDef_AssnMap _Tmp9;struct Cyc_AssnDef_AssnMap _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{struct Cyc_AssnDef_AssnMap a2t=_TmpA;struct Cyc_AssnDef_AssnMap a2f=_Tmp9;
struct _tuple19 _TmpB;({struct Cyc_AssnDef_AssnMap _TmpC=({struct Cyc_Vcgen_Env*_TmpD=env;Cyc_Vcgen_may_widen(_TmpD,Cyc_AssnDef_or_assnmap_assnmap(a1t,a2t));});_TmpB.f0=_TmpC;}),_TmpB.f1=a2f;return _TmpB;}}}case 9: _Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1057
struct _tuple14 _Tmp5=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _Tmp6;_Tmp6=_Tmp5.f1;{struct Cyc_AssnDef_AssnMap ain=_Tmp6;
return Cyc_Vcgen_vcgen_test(env,e2,ain);}}case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1==Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){_Tmp4=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;{struct Cyc_Absyn_Exp*e1=_Tmp4;
# 1060
struct _tuple19 _Tmp5=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _Tmp6;struct Cyc_AssnDef_AssnMap _Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_AssnDef_AssnMap at=_Tmp7;struct Cyc_AssnDef_AssnMap af=_Tmp6;
struct _tuple19 _Tmp8;_Tmp8.f0=af,_Tmp8.f1=at;return _Tmp8;}}}else{goto _LLF;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2)->tl)->tl==0){_Tmp1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->hd;_Tmp3=(struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2->tl->hd;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1071
struct _tuple14 _Tmp5=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*t1=_Tmp7;struct Cyc_AssnDef_AssnMap a1=_Tmp6;
struct _tuple14 _Tmp8=Cyc_Vcgen_vcgen_rexp(env,e2,a1);struct Cyc_AssnDef_AssnMap _Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*t2=_TmpA;struct Cyc_AssnDef_AssnMap a2=_Tmp9;
struct Cyc_AssnDef_AssnMap at=a2;
struct Cyc_AssnDef_AssnMap af=a2;
switch((int)p){case Cyc_Absyn_Eq:
# 1077
 at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_eq(t1,t2));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_neq(t1,t2));});
goto _LL35;case Cyc_Absyn_Neq:
# 1080
 at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_neq(t1,t2));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_eq(t1,t2));});goto _LL35;case Cyc_Absyn_Lt:
# 1082
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_ult(t1,t2));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_ulte(t2,t1));});}else{
# 1085
at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_slt(t1,t2));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_slte(t2,t1));});}
# 1087
goto _LL35;case Cyc_Absyn_Lte:
# 1089
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_ulte(t1,t2));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_ult(t2,t1));});}else{
# 1092
at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_slte(t1,t2));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_slt(t2,t1));});}
# 1094
goto _LL35;case Cyc_Absyn_Gt:
# 1096
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_ult(t2,t1));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_ulte(t1,t2));});}else{
# 1099
at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_slt(t2,t1));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_slte(t1,t2));});}
# 1101
goto _LL35;case Cyc_Absyn_Gte:
# 1103
 if(Cyc_Vcgen_unsigned_comparison(e1,e2)){
at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_ulte(t2,t1));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_ult(t1,t2));});}else{
# 1106
at=({struct Cyc_AssnDef_AssnMap _TmpB=at;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_slte(t2,t1));});af=({struct Cyc_AssnDef_AssnMap _TmpB=af;Cyc_AssnDef_and_assnmap_assn(_TmpB,Cyc_AssnDef_slt(t1,t2));});}
# 1108
goto _LL35;default:  {
# 1110
struct _tuple14 _TmpB=Cyc_Vcgen_vcgen_binop(env,e->topt,p,t1,t2,at);struct Cyc_AssnDef_AssnMap _TmpC;void*_TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{void*t=_TmpD;struct Cyc_AssnDef_AssnMap am=_TmpC;
at=({struct Cyc_AssnDef_AssnMap _TmpE=at;Cyc_AssnDef_and_assnmap_assn(_TmpE,({void*_TmpF=Cyc_AssnDef_one();Cyc_AssnDef_ulte(_TmpF,t);}));});
af=({struct Cyc_AssnDef_AssnMap _TmpE=af;Cyc_AssnDef_and_assnmap_assn(_TmpE,({void*_TmpF=t;Cyc_AssnDef_eq(_TmpF,Cyc_AssnDef_zero());}));});
goto _LL35;}}}_LL35:;{
# 1115
struct _tuple19 _TmpB;_TmpB.f0=at,_TmpB.f1=af;return _TmpB;}}}}}else{goto _LLF;}}else{goto _LLF;}}else{goto _LLF;}}case 37: _Tmp4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct _fat_ptr*f=_Tmp3;
# 1064
void*i=Cyc_AssnDef_uint((unsigned)Cyc_CfFlowInfo_get_field_index(_check_null(e1->topt),f));
struct _tuple14 _Tmp5=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*t=_Tmp7;struct Cyc_AssnDef_AssnMap ain=_Tmp6;
void*tt=Cyc_AssnDef_tagof_tm(t);
struct Cyc_AssnDef_AssnMap at=({struct Cyc_AssnDef_AssnMap _Tmp8=ain;Cyc_AssnDef_and_assnmap_assn(_Tmp8,Cyc_AssnDef_eq(tt,i));});
struct Cyc_AssnDef_AssnMap af=({struct Cyc_AssnDef_AssnMap _Tmp8=ain;Cyc_AssnDef_and_assnmap_assn(_Tmp8,Cyc_AssnDef_neq(tt,i));});
struct _tuple19 _Tmp8;_Tmp8.f0=at,_Tmp8.f1=af;return _Tmp8;}}default: _LLF: {
# 1117
struct _tuple14 _Tmp5=Cyc_Vcgen_vcgen_rexp(env,e,ain);struct Cyc_AssnDef_AssnMap _Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*t=_Tmp7;struct Cyc_AssnDef_AssnMap aout=_Tmp6;
if(*((int*)t)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1==0){
# 1120
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_and_assnmap_assn(aout,(void*)& Cyc_AssnDef_false_assn);_Tmp8.f0=_Tmp9;}),_Tmp8.f1=aout;return _Tmp8;}else{
# 1122
struct _tuple19 _Tmp8;_Tmp8.f0=aout,({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_and_assnmap_assn(aout,(void*)& Cyc_AssnDef_false_assn);_Tmp8.f1=_Tmp9;});return _Tmp8;}}else{
# 1124
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=aout;Cyc_AssnDef_and_assnmap_assn(_TmpA,({void*_TmpB=Cyc_AssnDef_one();Cyc_AssnDef_ulte(_TmpB,t);}));});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=aout;Cyc_AssnDef_and_assnmap_assn(_TmpA,({void*_TmpB=t;Cyc_AssnDef_eq(_TmpB,Cyc_AssnDef_zero());}));});_Tmp8.f1=_Tmp9;});return _Tmp8;};}}};}
# 1130
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*topt,struct Cyc_AssnDef_AssnMap a,void*tp){
# 1132
if(a.assn==(void*)& Cyc_AssnDef_false_assn)return;{
void*t;
if(topt!=0)t=topt;else{
t=Cyc_AssnDef_fresh_var(tp);}
# 1137
if(env->res_assnmap==0)return;
if(env->result!=0)
a=({struct Cyc_Vcgen_Env*_Tmp0=env;struct Cyc_AssnDef_AssnMap _Tmp1=a;void*_Tmp2=Cyc_AssnDef_addr(env->result);Cyc_Vcgen_do_assign(_Tmp0,_Tmp1,_Tmp2,t);});
# 1141
a=({struct Cyc_Vcgen_Env*_Tmp0=env;Cyc_Vcgen_may_widen(_Tmp0,Cyc_AssnDef_or_assnmap_assnmap(*_check_null(env->res_assnmap),a));});
*_check_null(env->res_assnmap)=a;}}
# 1145
static struct Cyc_Absyn_Exp*Cyc_Vcgen_find_assert_e(struct Cyc_Absyn_Stmt*s){
while(1){
{void*_Tmp0=s->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1: if(*((int*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1)->r)==41){_Tmp1=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1->r)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
return e;}}else{goto _LL5;}case 2: _Tmp1=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*sa=_Tmp1;
s=sa;continue;}default: _LL5:
 return 0;};}
# 1147
1U;}}
# 1163 "vcgen.cyc"
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_stmt(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,struct Cyc_AssnDef_AssnMap amin){
LOOP: {
struct Cyc_AssnDef_AssnMap oldam=Cyc_Vcgen_lookup_stmt_assnmap(env,s);
amin=Cyc_AssnDef_or_assnmap_assnmap(oldam,amin);{
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 0:
 return amin;case 1: _Tmp6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 1170
struct _tuple14 _Tmp7=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*t=_Tmp9;struct Cyc_AssnDef_AssnMap amout=_Tmp8;
if(env->exp_stmt!=0)
*env->exp_stmt=t;
return amout;}}case 2: _Tmp6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_Absyn_Stmt*s2=_Tmp5;
# 1175
amin=Cyc_Vcgen_vcgen_stmt(env,s1,amin);
s=s2;
goto LOOP;}case 3: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1!=0){_Tmp6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 1179
struct _tuple14 _Tmp7=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*t=_Tmp9;struct Cyc_AssnDef_AssnMap amout=_Tmp8;
Cyc_Vcgen_update_return(env,t,amout,e->topt);
return Cyc_AssnDef_false_assnmap();}}}else{
# 1183
Cyc_Vcgen_update_return(env,0,amin,0);
return Cyc_AssnDef_false_assnmap();}case 4: _Tmp6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 1186
struct _tuple19 _Tmp7=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _Tmp8;struct Cyc_AssnDef_AssnMap _Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{struct Cyc_AssnDef_AssnMap amt=_Tmp9;struct Cyc_AssnDef_AssnMap amf=_Tmp8;
amt=Cyc_Vcgen_vcgen_stmt(env,s1,amt);
amf=Cyc_Vcgen_vcgen_stmt(env,s2,amf);{
struct Cyc_Vcgen_Env*_TmpA=env;return Cyc_Vcgen_may_widen(_TmpA,Cyc_AssnDef_or_assnmap_assnmap(amt,amf));}}}case 5: _Tmp6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f1;_Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*dummy=_Tmp5;struct Cyc_Absyn_Stmt*s1=_Tmp4;
# 1191
struct Cyc_AssnDef_AssnMap*old_continue_assnmap=env->continue_assnmap;
({struct Cyc_AssnDef_AssnMap*_Tmp7=({struct Cyc_AssnDef_AssnMap*_Tmp8=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_false_assnmap();*_Tmp8=_Tmp9;});_Tmp8;});env->continue_assnmap=_Tmp7;});{
struct Cyc_Absyn_Exp*loop_inv_exp=0;
struct Cyc_AssnDef_AssnMap loop_inv_assnmap;
{void*_Tmp7=e->r;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp7)==7){_Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*eleft=_Tmp9;struct Cyc_Absyn_Exp*eright=_Tmp8;
# 1197
{void*_TmpA=eleft->r;void*_TmpB;if(*((int*)_TmpA)==41){_TmpB=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpA)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpB;
# 1199
loop_inv_exp=eassn;
e=eright;
goto _LL33;}}else{
# 1203
goto _LL33;}_LL33:;}
# 1205
goto _LL2E;}}else{
# 1207
goto _LL2E;}_LL2E:;}
# 1209
if(loop_inv_exp!=0){
int old_widen_paths=env->widen_paths;
env->widen_paths=0;
loop_inv_assnmap=({struct Cyc_Vcgen_Env*_Tmp7=env;struct Cyc_Absyn_Exp*_Tmp8=loop_inv_exp;Cyc_Vcgen_vcgen_test(_Tmp7,_Tmp8,({struct Cyc_AssnDef_AssnMap _Tmp9;_Tmp9.assn=(void*)& Cyc_AssnDef_true_assn,_Tmp9.map=amin.map;_Tmp9;}));}).f0;
env->widen_paths=old_widen_paths;}else{
# 1216
loop_inv_assnmap=({struct Cyc_AssnDef_AssnMap _Tmp7;_Tmp7.assn=(void*)& Cyc_AssnDef_true_assn,_Tmp7.map=amin.map;_Tmp7;});}
if(!Cyc_Vcgen_implies(amin.assn,loop_inv_assnmap.assn)){
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(loop_inv_assnmap);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err(e->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_Tmp9,sizeof(void*),2));});
env->continue_assnmap=old_continue_assnmap;
return Cyc_AssnDef_true_assnmap();}
# 1223
amin=Cyc_AssnDef_canonical_assnmap(amin);
while(1){
# 1226
{struct Cyc_AssnDef_AssnMap amold=Cyc_Vcgen_lookup_stmt_assnmap(env,dummy);
amin=Cyc_AssnDef_widen_assnmap(Cyc_AssnDef_or_assnmap_assnmap(amold,amin));
Cyc_Vcgen_replace_stmt_assnmap(env,dummy,amin);
if(loop_inv_exp!=0)
amin=Cyc_Vcgen_vcgen_test(env,loop_inv_exp,amin).f0;{
# 1232
struct _tuple19 _Tmp7=Cyc_Vcgen_vcgen_test(env,e,amin);struct Cyc_AssnDef_AssnMap _Tmp8;struct Cyc_AssnDef_AssnMap _Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{struct Cyc_AssnDef_AssnMap amt=_Tmp9;struct Cyc_AssnDef_AssnMap amf=_Tmp8;
amin=Cyc_Vcgen_vcgen_stmt(env,s1,amt);
# 1235
amin=Cyc_AssnDef_or_assnmap_assnmap(amin,*_check_null(env->continue_assnmap));
if(loop_inv_exp!=0){
int old_widen_paths=env->widen_paths;
env->widen_paths=0;
loop_inv_assnmap=({struct Cyc_Vcgen_Env*_TmpA=env;struct Cyc_Absyn_Exp*_TmpB=loop_inv_exp;Cyc_Vcgen_vcgen_test(_TmpA,_TmpB,({struct Cyc_AssnDef_AssnMap _TmpC;_TmpC.assn=(void*)& Cyc_AssnDef_true_assn,_TmpC.map=amin.map;_TmpC;}));}).f0;
env->widen_paths=old_widen_paths;}else{
# 1243
loop_inv_assnmap=({struct Cyc_AssnDef_AssnMap _TmpA;_TmpA.assn=(void*)& Cyc_AssnDef_true_assn,_TmpA.map=amin.map;_TmpA;});}
if(!Cyc_Vcgen_implies(amin.assn,loop_inv_assnmap.assn)){
({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(loop_inv_assnmap);_TmpB.f1=_TmpC;});_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({struct _fat_ptr _TmpD=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_err(e->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_TmpC,sizeof(void*),2));});
env->continue_assnmap=old_continue_assnmap;
return Cyc_AssnDef_true_assnmap();}
# 1250
amin=Cyc_AssnDef_canonical_assnmap(amin);{
# 1253
struct _tuple15 _TmpA=({struct Cyc_AssnDef_AssnMap _TmpB=Cyc_Vcgen_lookup_stmt_assnmap(env,dummy);Cyc_AssnDef_unify_var_maps_subst(_TmpB,amin);});struct Cyc_Dict_Dict _TmpB;void*_TmpC;void*_TmpD;_TmpD=_TmpA.f0;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*a1=_TmpD;void*a2=_TmpC;struct Cyc_Dict_Dict map=_TmpB;
# 1259
amin=({struct Cyc_AssnDef_AssnMap _TmpE;_TmpE.assn=a2,_TmpE.map=map;_TmpE;});
if(Cyc_Vcgen_implies(a2,a1)){
env->continue_assnmap=old_continue_assnmap;
return amf;}}}}}}
# 1226
1U;}}}case 7:
# 1267
({struct Cyc_AssnDef_AssnMap _Tmp7=Cyc_AssnDef_or_assnmap_assnmap(*_check_null(env->continue_assnmap),amin);*_check_null(env->continue_assnmap)=_Tmp7;});
return Cyc_AssnDef_false_assnmap();case 6:  {
# 1270
struct Cyc_Absyn_Stmt*dest=({struct Cyc_Absyn_Stmt*(*_Tmp7)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;_Tmp7;})(env->succ_table,s);
if(dest==0)
Cyc_Vcgen_update_return(env,0,amin,0);else{
# 1274
Cyc_Vcgen_update_stmt_assnmap(env,dest,amin);}
return Cyc_AssnDef_false_assnmap();}case 9: _Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Stmt*dummy2=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;struct Cyc_Absyn_Stmt*dummy3=_Tmp2;struct Cyc_Absyn_Stmt*s1=_Tmp1;
# 1277
struct Cyc_AssnDef_AssnMap*old_continue_assnmap=env->continue_assnmap;
({struct Cyc_AssnDef_AssnMap*_Tmp7=({struct Cyc_AssnDef_AssnMap*_Tmp8=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_false_assnmap();*_Tmp8=_Tmp9;});_Tmp8;});env->continue_assnmap=_Tmp7;});{
struct Cyc_Absyn_Exp*loop_inv_exp=0;
struct Cyc_AssnDef_AssnMap loop_inv_assnmap;
{void*_Tmp7=e2->r;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp7)==7){_Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*eleft=_Tmp9;struct Cyc_Absyn_Exp*eright=_Tmp8;
# 1283
{void*_TmpA=eleft->r;void*_TmpB;if(*((int*)_TmpA)==41){_TmpB=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpA)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpB;
# 1285
loop_inv_exp=eassn;
e2=eright;
goto _LL43;}}else{
# 1289
goto _LL43;}_LL43:;}
# 1291
goto _LL3E;}}else{
# 1293
goto _LL3E;}_LL3E:;}{
# 1296
struct _tuple14 _Tmp7=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _Tmp8;_Tmp8=_Tmp7.f1;{struct Cyc_AssnDef_AssnMap amin=_Tmp8;
if(loop_inv_exp!=0){
int old_widen_paths=env->widen_paths;
env->widen_paths=0;
loop_inv_assnmap=({struct Cyc_Vcgen_Env*_Tmp9=env;struct Cyc_Absyn_Exp*_TmpA=loop_inv_exp;Cyc_Vcgen_vcgen_test(_Tmp9,_TmpA,({struct Cyc_AssnDef_AssnMap _TmpB;_TmpB.assn=(void*)& Cyc_AssnDef_true_assn,_TmpB.map=amin.map;_TmpB;}));}).f0;
env->widen_paths=old_widen_paths;}else{
# 1304
loop_inv_assnmap=({struct Cyc_AssnDef_AssnMap _Tmp9;_Tmp9.assn=(void*)& Cyc_AssnDef_true_assn,_Tmp9.map=amin.map;_Tmp9;});}
if(!Cyc_Vcgen_implies(amin.assn,loop_inv_assnmap.assn)){
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(loop_inv_assnmap);_TmpA.f1=_TmpB;});_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({struct _fat_ptr _TmpC=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_TmpB.f1=_TmpC;});_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_Warn_err(e2->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_TmpB,sizeof(void*),2));});
env->continue_assnmap=old_continue_assnmap;
return Cyc_AssnDef_true_assnmap();}
# 1311
amin=Cyc_AssnDef_canonical_assnmap(amin);
while(1){
# 1314
{struct Cyc_AssnDef_AssnMap amold=Cyc_Vcgen_lookup_stmt_assnmap(env,dummy2);
amin=Cyc_AssnDef_widen_assnmap(Cyc_AssnDef_or_assnmap_assnmap(amin,amold));
Cyc_Vcgen_replace_stmt_assnmap(env,dummy2,amin);
if(loop_inv_exp!=0)
amin=Cyc_Vcgen_vcgen_test(env,loop_inv_exp,amin).f0;{
# 1320
struct _tuple19 _Tmp9=Cyc_Vcgen_vcgen_test(env,e2,amin);struct Cyc_AssnDef_AssnMap _TmpA;struct Cyc_AssnDef_AssnMap _TmpB;_TmpB=_Tmp9.f0;_TmpA=_Tmp9.f1;{struct Cyc_AssnDef_AssnMap amt=_TmpB;struct Cyc_AssnDef_AssnMap amf=_TmpA;
# 1322
amin=Cyc_Vcgen_vcgen_stmt(env,s1,amt);{
struct _tuple14 _TmpC=Cyc_Vcgen_vcgen_rexp(env,e3,amin);struct Cyc_AssnDef_AssnMap _TmpD;_TmpD=_TmpC.f1;{struct Cyc_AssnDef_AssnMap am=_TmpD;
amin=am;
if(loop_inv_exp!=0){
int old_widen_paths=env->widen_paths;
env->widen_paths=0;
loop_inv_assnmap=({struct Cyc_Vcgen_Env*_TmpE=env;struct Cyc_Absyn_Exp*_TmpF=loop_inv_exp;Cyc_Vcgen_vcgen_test(_TmpE,_TmpF,({struct Cyc_AssnDef_AssnMap _Tmp10;_Tmp10.assn=(void*)& Cyc_AssnDef_true_assn,_Tmp10.map=amin.map;_Tmp10;}));}).f0;
env->widen_paths=old_widen_paths;}else{
# 1332
loop_inv_assnmap=({struct Cyc_AssnDef_AssnMap _TmpE;_TmpE.assn=(void*)& Cyc_AssnDef_true_assn,_TmpE.map=amin.map;_TmpE;});}
if(!Cyc_Vcgen_implies(amin.assn,loop_inv_assnmap.assn)){
({struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({
struct _fat_ptr _Tmp10=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(loop_inv_assnmap);_TmpF.f1=_Tmp10;});_TmpF;});struct Cyc_String_pa_PrintArg_struct _TmpF=({struct Cyc_String_pa_PrintArg_struct _Tmp10;_Tmp10.tag=0,({struct _fat_ptr _Tmp11=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_Tmp10.f1=_Tmp11;});_Tmp10;});void*_Tmp10[2];_Tmp10[0]=& _TmpE,_Tmp10[1]=& _TmpF;Cyc_Warn_err(e2->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_Tmp10,sizeof(void*),2));});
env->continue_assnmap=old_continue_assnmap;
return Cyc_AssnDef_true_assnmap();}
# 1339
amin=Cyc_AssnDef_canonical_assnmap(amin);{
struct _tuple15 _TmpE=({struct Cyc_AssnDef_AssnMap _TmpF=Cyc_Vcgen_lookup_stmt_assnmap(env,dummy2);Cyc_AssnDef_unify_var_maps_subst(_TmpF,amin);});struct Cyc_Dict_Dict _TmpF;void*_Tmp10;void*_Tmp11;_Tmp11=_TmpE.f0;_Tmp10=_TmpE.f1;_TmpF=_TmpE.f2;{void*a1=_Tmp11;void*a2=_Tmp10;struct Cyc_Dict_Dict map=_TmpF;
# 1344
amin=({struct Cyc_AssnDef_AssnMap _Tmp12;_Tmp12.assn=a2,_Tmp12.map=map;_Tmp12;});
if(Cyc_Vcgen_implies(a2,a1)){
env->continue_assnmap=old_continue_assnmap;
return amf;}}}}}}}}
# 1314
1U;}}}}}case 14: _Tmp6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp5;struct Cyc_Absyn_Stmt*dummy=_Tmp4;
# 1351
struct Cyc_AssnDef_AssnMap*old_continue_assnmap=env->continue_assnmap;
({struct Cyc_AssnDef_AssnMap*_Tmp7=({struct Cyc_AssnDef_AssnMap*_Tmp8=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_false_assnmap();*_Tmp8=_Tmp9;});_Tmp8;});env->continue_assnmap=_Tmp7;});
amin=Cyc_Vcgen_vcgen_stmt(env,s1,amin);{
struct Cyc_Absyn_Exp*loop_inv_exp=0;
struct Cyc_AssnDef_AssnMap loop_inv_assnmap;
{void*_Tmp7=e->r;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp7)==7){_Tmp9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;_Tmp8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Exp*eleft=_Tmp9;struct Cyc_Absyn_Exp*eright=_Tmp8;
# 1358
{void*_TmpA=eleft->r;void*_TmpB;if(*((int*)_TmpA)==41){_TmpB=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TmpA)->f1;{struct Cyc_Absyn_Exp*eassn=_TmpB;
# 1360
loop_inv_exp=eassn;
e=eright;
goto _LL59;}}else{
# 1364
goto _LL59;}_LL59:;}
# 1366
goto _LL54;}}else{
# 1368
goto _LL54;}_LL54:;}
# 1370
if(loop_inv_exp!=0){
int old_widen_paths=env->widen_paths;
env->widen_paths=0;
loop_inv_assnmap=({struct Cyc_Vcgen_Env*_Tmp7=env;struct Cyc_Absyn_Exp*_Tmp8=loop_inv_exp;Cyc_Vcgen_vcgen_test(_Tmp7,_Tmp8,({struct Cyc_AssnDef_AssnMap _Tmp9;_Tmp9.assn=(void*)& Cyc_AssnDef_true_assn,_Tmp9.map=amin.map;_Tmp9;}));}).f0;
env->widen_paths=old_widen_paths;}else{
# 1377
loop_inv_assnmap=({struct Cyc_AssnDef_AssnMap _Tmp7;_Tmp7.assn=(void*)& Cyc_AssnDef_true_assn,_Tmp7.map=amin.map;_Tmp7;});}
if(!Cyc_Vcgen_implies(amin.assn,loop_inv_assnmap.assn)){
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(loop_inv_assnmap);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err(e->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_Tmp9,sizeof(void*),2));});
env->continue_assnmap=old_continue_assnmap;
return Cyc_AssnDef_true_assnmap();}
# 1384
amin=Cyc_AssnDef_canonical_assnmap(amin);
while(1){
# 1387
{struct Cyc_AssnDef_AssnMap amold=Cyc_Vcgen_lookup_stmt_assnmap(env,dummy);
amin=Cyc_AssnDef_widen_assnmap(Cyc_AssnDef_or_assnmap_assnmap(amold,amin));
Cyc_Vcgen_replace_stmt_assnmap(env,dummy,amin);
if(loop_inv_exp!=0)
amin=Cyc_Vcgen_vcgen_test(env,loop_inv_exp,amin).f0;{
# 1393
struct _tuple19 _Tmp7=Cyc_Vcgen_vcgen_test(env,e,amin);struct Cyc_AssnDef_AssnMap _Tmp8;struct Cyc_AssnDef_AssnMap _Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{struct Cyc_AssnDef_AssnMap amt=_Tmp9;struct Cyc_AssnDef_AssnMap amf=_Tmp8;
amin=Cyc_Vcgen_vcgen_stmt(env,s1,amt);
# 1396
amin=Cyc_AssnDef_or_assnmap_assnmap(amin,*_check_null(env->continue_assnmap));
if(loop_inv_exp!=0){
int old_widen_paths=env->widen_paths;
env->widen_paths=0;
loop_inv_assnmap=({struct Cyc_Vcgen_Env*_TmpA=env;struct Cyc_Absyn_Exp*_TmpB=loop_inv_exp;Cyc_Vcgen_vcgen_test(_TmpA,_TmpB,({struct Cyc_AssnDef_AssnMap _TmpC;_TmpC.assn=(void*)& Cyc_AssnDef_true_assn,_TmpC.map=amin.map;_TmpC;}));}).f0;
env->widen_paths=old_widen_paths;}else{
# 1404
loop_inv_assnmap=({struct Cyc_AssnDef_AssnMap _TmpA;_TmpA.assn=(void*)& Cyc_AssnDef_true_assn,_TmpA.map=amin.map;_TmpA;});}
if(!Cyc_Vcgen_implies(amin.assn,loop_inv_assnmap.assn)){
({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(loop_inv_assnmap);_TmpB.f1=_TmpC;});_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({struct _fat_ptr _TmpD=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_err(e->loc,_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U),_tag_fat(_TmpC,sizeof(void*),2));});
env->continue_assnmap=old_continue_assnmap;
return Cyc_AssnDef_true_assnmap();}
# 1411
amin=Cyc_AssnDef_canonical_assnmap(amin);{
# 1414
struct _tuple15 _TmpA=({struct Cyc_AssnDef_AssnMap _TmpB=Cyc_Vcgen_lookup_stmt_assnmap(env,dummy);Cyc_AssnDef_unify_var_maps_subst(_TmpB,amin);});struct Cyc_Dict_Dict _TmpB;void*_TmpC;void*_TmpD;_TmpD=_TmpA.f0;_TmpC=_TmpA.f1;_TmpB=_TmpA.f2;{void*a1=_TmpD;void*a2=_TmpC;struct Cyc_Dict_Dict map=_TmpB;
# 1418
amin=({struct Cyc_AssnDef_AssnMap _TmpE;_TmpE.assn=a2,_TmpE.map=map;_TmpE;});
if(Cyc_Vcgen_implies(a2,a1)){
env->continue_assnmap=old_continue_assnmap;
return amf;}}}}}}
# 1387
1U;}}}case 10: _Tmp6=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_List_List*scs=_Tmp5;void*dtree=_Tmp4;
# 1425
struct _tuple14 _Tmp7=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*t=_Tmp9;struct Cyc_AssnDef_AssnMap am=_Tmp8;
return Cyc_Vcgen_vcgen_switch(env,t,scs,am);}}case 11: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2==0)
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("vcgen fallthru",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));else{_Tmp6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=*((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp6;struct Cyc_Absyn_Switch_clause*dest=_Tmp5;
# 1430
struct _tuple18 _Tmp7=Cyc_Vcgen_vcgen_rexps(env,es,amin);struct Cyc_AssnDef_AssnMap _Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{struct Cyc_List_List*ts=_Tmp9;struct Cyc_AssnDef_AssnMap am=_Tmp8;
if(ts!=0){
# 1433
struct Cyc_List_List*x=Cyc_Tcutil_filter_nulls(Cyc_List_split((struct Cyc_List_List*)_check_null(dest->pat_vars)->v).f0);
# 1435
for(1;x!=0;(x=x->tl,ts=ts->tl)){
struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)x->hd;
if(vd!=0)
am=({struct Cyc_Vcgen_Env*_TmpA=env;struct Cyc_AssnDef_AssnMap _TmpB=am;void*_TmpC=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_TmpA,_TmpB,_TmpC,(void*)_check_null(ts)->hd);});}}
# 1441
Cyc_Vcgen_update_stmt_assnmap(env,dest->body,am);
return Cyc_AssnDef_false_assnmap();}}}case 12: _Tmp6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;
# 1444
amin=Cyc_Vcgen_vcgen_local_decl(env,d,amin);
s=s1;
goto LOOP;}case 13: _Tmp6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;
# 1451
struct Cyc_Absyn_Exp*label_assn_opt=Cyc_Vcgen_find_assert_e(s1);
if(label_assn_opt==0)
# 1457
amin=Cyc_AssnDef_true_assnmap();else{
# 1460
struct _tuple19 _Tmp7=({struct Cyc_Vcgen_Env*_Tmp8=env;struct Cyc_Absyn_Exp*_Tmp9=label_assn_opt;Cyc_Vcgen_vcgen_test(_Tmp8,_Tmp9,({struct Cyc_AssnDef_AssnMap _TmpA;_TmpA.assn=(void*)& Cyc_AssnDef_true_assn,_TmpA.map=amin.map;_TmpA;}));});struct Cyc_AssnDef_AssnMap _Tmp8;_Tmp8=_Tmp7.f0;{struct Cyc_AssnDef_AssnMap amt=_Tmp8;
if(!Cyc_Vcgen_implies(amin.assn,amt.assn))
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_TmpA.f1=_TmpB;});_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({struct _fat_ptr _TmpC=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amt);_TmpB.f1=_TmpC;});_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_Warn_err(s->loc,_tag_fat("can not prove @assert(%s) \n from %s",sizeof(char),36U),_tag_fat(_TmpB,sizeof(void*),2));});{
# 1467
struct _tuple19 _Tmp9=({struct Cyc_Vcgen_Env*_TmpA=env;struct Cyc_Absyn_Exp*_TmpB=label_assn_opt;Cyc_Vcgen_vcgen_test(_TmpA,_TmpB,Cyc_AssnDef_true_assnmap());});struct Cyc_AssnDef_AssnMap _TmpA;_TmpA=_Tmp9.f0;{struct Cyc_AssnDef_AssnMap amt=_TmpA;
amin=amt;}}}}
# 1470
s=s1;
goto LOOP;}case 8:  {
# 1475
struct Cyc_Absyn_Stmt*destopt=({struct Cyc_Absyn_Stmt*(*_Tmp7)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))Cyc_Hashtable_lookup;_Tmp7;})(env->succ_table,s);
if(destopt!=0){
void*_Tmp7=destopt->r;void*_Tmp8;if(*((int*)_Tmp7)==13){_Tmp8=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp7)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp8;
# 1479
struct Cyc_Absyn_Exp*label_assn_opt=Cyc_Vcgen_find_assert_e(s1);
if(label_assn_opt==0)goto _LL70;{
struct Cyc_Absyn_Exp*label_assn=label_assn_opt;
struct _tuple19 _Tmp9=({struct Cyc_Vcgen_Env*_TmpA=env;struct Cyc_Absyn_Exp*_TmpB=label_assn;Cyc_Vcgen_vcgen_test(_TmpA,_TmpB,({struct Cyc_AssnDef_AssnMap _TmpC;_TmpC.assn=(void*)& Cyc_AssnDef_true_assn,_TmpC.map=amin.map;_TmpC;}));});struct Cyc_AssnDef_AssnMap _TmpA;_TmpA=_Tmp9.f0;{struct Cyc_AssnDef_AssnMap amt=_TmpA;
if(!Cyc_Vcgen_implies(amin.assn,amt.assn))
({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({
struct _fat_ptr _TmpD=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amt);_TmpC.f1=_TmpD;});_TmpC;});struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,({struct _fat_ptr _TmpE=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(amin);_TmpD.f1=_TmpE;});_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;Cyc_Warn_err(s->loc,_tag_fat("cannot prove @assert(%s)\nfrom %s",sizeof(char),33U),_tag_fat(_TmpD,sizeof(void*),2));});
goto _LL70;}}}}else{
({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp9;})(_tag_fat("vcgen goto",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));}_LL70:;}
# 1490
return Cyc_AssnDef_false_assnmap();}default: _Tmp6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_List_List*scs=_Tmp5;void*dtree=_Tmp4;
# 1492
struct Cyc_AssnDef_AssnMap*old_try_assnmap=env->try_assnmap;
# 1494
({struct Cyc_AssnDef_AssnMap*_Tmp7=({struct Cyc_AssnDef_AssnMap*_Tmp8=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_false_assnmap();*_Tmp8=_Tmp9;});_Tmp8;});env->try_assnmap=_Tmp7;});{
# 1497
struct Cyc_AssnDef_AssnMap am1=Cyc_Vcgen_vcgen_stmt(env,s,amin);
# 1499
struct Cyc_AssnDef_AssnMap new_try_assnmap=*_check_null(env->try_assnmap);
env->try_assnmap=old_try_assnmap;{
# 1503
struct _tuple14 _Tmp7=({struct Cyc_Absyn_Vardecl*_Tmp8=Cyc_AssnDef_exception_vardecl();Cyc_AssnDef_lookup_var_map(_Tmp8,am1);});struct Cyc_AssnDef_AssnMap _Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*exc=_Tmp9;struct Cyc_AssnDef_AssnMap am2=_Tmp8;
struct Cyc_AssnDef_AssnMap am3=Cyc_Vcgen_vcgen_switch(env,exc,scs,new_try_assnmap);
struct Cyc_Vcgen_Env*_TmpA=env;return Cyc_Vcgen_may_widen(_TmpA,Cyc_AssnDef_or_assnmap_assnmap(am1,am2));}}}}};}}}struct _tuple26{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 1509
static struct _tuple19 Cyc_Vcgen_vcgen_pat(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Pat*p,void*lvalue,void*v,struct Cyc_AssnDef_AssnMap amin){
# 1511
void*_Tmp0=p->r;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;void*_Tmp7;switch(*((int*)_Tmp0)){case 0:  {
# 1513
struct _tuple19 _Tmp8;_Tmp8.f0=amin,({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_false_assnmap();_Tmp8.f1=_Tmp9;});return _Tmp8;}case 1: _Tmp7=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;struct Cyc_Absyn_Pat*p2=_Tmp6;
# 1515
struct _tuple19 _Tmp8=Cyc_Vcgen_vcgen_pat(env,p2,lvalue,v,amin);struct Cyc_AssnDef_AssnMap _Tmp9;struct Cyc_AssnDef_AssnMap _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{struct Cyc_AssnDef_AssnMap amt=_TmpA;struct Cyc_AssnDef_AssnMap amf=_Tmp9;
struct _tuple19 _TmpB;({struct Cyc_AssnDef_AssnMap _TmpC=({struct Cyc_Vcgen_Env*_TmpD=env;struct Cyc_AssnDef_AssnMap _TmpE=amt;void*_TmpF=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_TmpD,_TmpE,_TmpF,v);});_TmpB.f0=_TmpC;}),_TmpB.f1=amf;return _TmpB;}}case 2: _Tmp7=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp7;struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 1518
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_Vcgen_Env*_TmpA=env;struct Cyc_AssnDef_AssnMap _TmpB=amin;void*_TmpC=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_TmpA,_TmpB,_TmpC,v);});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_false_assnmap();_Tmp8.f1=_Tmp9;});return _Tmp8;}case 3: _Tmp7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;struct Cyc_Absyn_Pat*p2=_Tmp6;
# 1520
struct _tuple19 _Tmp8=Cyc_Vcgen_vcgen_pat(env,p2,lvalue,v,amin);struct Cyc_AssnDef_AssnMap _Tmp9;struct Cyc_AssnDef_AssnMap _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{struct Cyc_AssnDef_AssnMap amt=_TmpA;struct Cyc_AssnDef_AssnMap amf=_Tmp9;
if(lvalue==0)({int(*_TmpB)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpB;})(_tag_fat("vcgen reference pattern matching",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple19 _TmpB;({struct Cyc_AssnDef_AssnMap _TmpC=({struct Cyc_Vcgen_Env*_TmpD=env;struct Cyc_AssnDef_AssnMap _TmpE=amt;void*_TmpF=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_TmpD,_TmpE,_TmpF,lvalue);});_TmpB.f0=_TmpC;}),_TmpB.f1=amf;return _TmpB;}}}case 4: _Tmp7=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;
# 1524
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_Vcgen_Env*_TmpA=env;struct Cyc_AssnDef_AssnMap _TmpB=amin;void*_TmpC=Cyc_AssnDef_addr(vd);Cyc_Vcgen_do_assign(_TmpA,_TmpB,_TmpC,v);});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=Cyc_AssnDef_false_assnmap();_Tmp8.f1=_Tmp9;});return _Tmp8;}case 8:  {
# 1526
void*t=Cyc_AssnDef_zero();
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});return _Tmp8;}case 9: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
# 1529
void*t=Cyc_AssnDef_uint((unsigned)i);
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});return _Tmp8;}case 10: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
# 1532
void*t=Cyc_AssnDef_uint((unsigned)c);
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});return _Tmp8;}case 11: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr f=_Tmp2;int i=_Tmp4;
# 1535
struct Cyc_Absyn_Exp*e=Cyc_Absyn_float_exp(f,i,0U);
({void*_Tmp8=Cyc_Absyn_gen_float_type((unsigned)i);e->topt=_Tmp8;});{
void*t=Cyc_AssnDef_cnst(e);
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});return _Tmp8;}}case 12: _Tmp7=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;struct Cyc_Absyn_Enumfield*ef=_Tmp6;
# 1540
struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_Tmp8->tag=31,_Tmp8->f1=ed,_Tmp8->f2=ef;_Tmp8;}),0U);
e->topt=p->topt;{
void*t=Cyc_AssnDef_cnst(e);
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});return _Tmp8;}}case 13: _Tmp7=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{void*type=_Tmp7;struct Cyc_Absyn_Enumfield*ef=_Tmp6;
# 1545
struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_Tmp8->tag=32,_Tmp8->f1=type,_Tmp8->f2=ef;_Tmp8;}),0U);
e->topt=p->topt;{
void*t=Cyc_AssnDef_cnst(e);
struct _tuple19 _Tmp8;({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_eq(v,t));});_Tmp8.f0=_Tmp9;}),({struct Cyc_AssnDef_AssnMap _Tmp9=({struct Cyc_AssnDef_AssnMap _TmpA=amin;Cyc_AssnDef_and_assnmap_assn(_TmpA,Cyc_AssnDef_neq(v,t));});_Tmp8.f1=_Tmp9;});return _Tmp8;}}case 5: _Tmp7=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp7;
# 1551
struct _tuple14 _Tmp8=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _Tmp9;void*_TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{void*mem=_TmpA;struct Cyc_AssnDef_AssnMap amin=_Tmp9;
void*v2=Cyc_AssnDef_select(mem,v,p->topt);
struct _tuple19 _TmpB=Cyc_Vcgen_vcgen_pat(env,p2,v,v2,amin);struct Cyc_AssnDef_AssnMap _TmpC;struct Cyc_AssnDef_AssnMap _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct Cyc_AssnDef_AssnMap amt=_TmpD;struct Cyc_AssnDef_AssnMap amf=_TmpC;
# 1555
struct _tuple19 _TmpE;({struct Cyc_AssnDef_AssnMap _TmpF=({struct Cyc_AssnDef_AssnMap _Tmp10=amt;Cyc_AssnDef_and_assnmap_assn(_Tmp10,({void*_Tmp11=Cyc_AssnDef_zero();Cyc_AssnDef_neq(_Tmp11,v);}));});_TmpE.f0=_TmpF;}),({struct Cyc_AssnDef_AssnMap _TmpF=({struct Cyc_AssnDef_AssnMap _Tmp10=amf;Cyc_AssnDef_or_assnmap_assn(_Tmp10,({void*_Tmp11=Cyc_AssnDef_zero();Cyc_AssnDef_eq(_Tmp11,v);}));});_TmpE.f1=_TmpF;});return _TmpE;}}}case 6: _Tmp7=(void*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{void*topt=_Tmp7;struct Cyc_List_List*dlps=_Tmp6;
# 1557
void*type=_check_null(topt);
struct _tuple21 _Tmp8=Cyc_Vcgen_get_aggr_info(type);void*_Tmp9;int _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f2;{int is_tagged=_TmpA;struct Cyc_List_List*fields=_Tmp9;
struct Cyc_AssnDef_AssnMap amt=amin;
struct Cyc_AssnDef_AssnMap amf=Cyc_AssnDef_false_assnmap();
for(1;dlps!=0;dlps=dlps->tl){
struct _tuple26*tup=(struct _tuple26*)dlps->hd;
struct Cyc_Absyn_Pat*p2=tup->f1;
if(p2->r==(void*)& Cyc_Absyn_Wild_p_val)continue;{
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(tup->f0);
int i=Cyc_CfFlowInfo_get_field_index(type,f);
if(is_tagged){
# 1570
void*tag_check=({void*_TmpB=Cyc_AssnDef_tagof_tm(v);Cyc_AssnDef_eq(_TmpB,Cyc_AssnDef_uint((unsigned)i));});
amt=Cyc_AssnDef_and_assnmap_assn(amt,tag_check);
amf=({struct Cyc_AssnDef_AssnMap _TmpB=amf;Cyc_AssnDef_or_assnmap_assn(_TmpB,Cyc_AssnDef_not(tag_check));});}{
# 1574
void*member=Cyc_AssnDef_proj(v,(unsigned)i,p2->topt);
void*path=0;
if(lvalue!=0)
path=Cyc_AssnDef_offsetf(lvalue,(unsigned)i,0);{
struct _tuple19 _TmpB=Cyc_Vcgen_vcgen_pat(env,p2,path,member,amt);struct Cyc_AssnDef_AssnMap _TmpC;struct Cyc_AssnDef_AssnMap _TmpD;_TmpD=_TmpB.f0;_TmpC=_TmpB.f1;{struct Cyc_AssnDef_AssnMap amt2=_TmpD;struct Cyc_AssnDef_AssnMap amf2=_TmpC;
amf=({struct Cyc_Vcgen_Env*_TmpE=env;Cyc_Vcgen_may_widen(_TmpE,Cyc_AssnDef_or_assnmap_assnmap(amf,amf2));});
amt=amt2;}}}}}{
# 1582
struct _tuple19 _TmpB;_TmpB.f0=amt,_TmpB.f1=amf;return _TmpB;}}}case 7: _Tmp7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;struct Cyc_Absyn_Datatypefield*df=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 1586
int i=(int)Cyc_Vcgen_datatype_constructor_index(dd,df);
void*tag_check=({void*_Tmp8=Cyc_AssnDef_tagof_tm(v);Cyc_AssnDef_eq(_Tmp8,Cyc_AssnDef_uint((unsigned)i));});
struct Cyc_AssnDef_AssnMap amt=Cyc_AssnDef_and_assnmap_assn(amin,tag_check);
struct Cyc_AssnDef_AssnMap amf=Cyc_AssnDef_false_assnmap();
{unsigned j=0U;for(0;ps!=0;(ps=ps->tl,++ j)){
struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)ps->hd;
void*member=Cyc_AssnDef_proj(v,j,p2->topt);
void*path=0;
if(lvalue!=0)
path=Cyc_AssnDef_offsetf(lvalue,j,0);{
# 1597
struct _tuple19 _Tmp8=Cyc_Vcgen_vcgen_pat(env,p2,path,member,amt);struct Cyc_AssnDef_AssnMap _Tmp9;struct Cyc_AssnDef_AssnMap _TmpA;_TmpA=_Tmp8.f0;_Tmp9=_Tmp8.f1;{struct Cyc_AssnDef_AssnMap amt2=_TmpA;struct Cyc_AssnDef_AssnMap amf2=_Tmp9;
amf=({struct Cyc_Vcgen_Env*_TmpB=env;Cyc_Vcgen_may_widen(_TmpB,Cyc_AssnDef_or_assnmap_assnmap(amf,amf2));});
amt=amt2;}}}}
# 1601
amf=({struct Cyc_AssnDef_AssnMap _Tmp8=amf;Cyc_AssnDef_or_assnmap_assn(_Tmp8,Cyc_AssnDef_not(tag_check));});{
struct _tuple19 _Tmp8;_Tmp8.f0=amt,_Tmp8.f1=amf;return _Tmp8;}}case 14:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
({int(*_Tmp8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp8;})(_tag_fat("vcgen pat found bad pattern",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));};}
# 1609
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,void*v,struct Cyc_List_List*scs,struct Cyc_AssnDef_AssnMap amin){
# 1614
struct Cyc_AssnDef_AssnMap res=Cyc_AssnDef_false_assnmap();
for(1;scs!=0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*sc=(struct Cyc_Absyn_Switch_clause*)scs->hd;
struct _tuple19 _Tmp0=Cyc_Vcgen_vcgen_pat(env,sc->pattern,0,v,amin);struct Cyc_AssnDef_AssnMap _Tmp1;struct Cyc_AssnDef_AssnMap _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_AssnDef_AssnMap amt=_Tmp2;struct Cyc_AssnDef_AssnMap amf=_Tmp1;
if(sc->where_clause!=0){
struct _tuple19 _Tmp3=Cyc_Vcgen_vcgen_test(env,sc->where_clause,amt);struct Cyc_AssnDef_AssnMap _Tmp4;struct Cyc_AssnDef_AssnMap _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_AssnDef_AssnMap amt2=_Tmp5;struct Cyc_AssnDef_AssnMap amf2=_Tmp4;
amt=amt2;
amf=({struct Cyc_Vcgen_Env*_Tmp6=env;Cyc_Vcgen_may_widen(_Tmp6,Cyc_AssnDef_or_assnmap_assnmap(amf,amf2));});}}
# 1623
res=({struct Cyc_Vcgen_Env*_Tmp3=env;Cyc_Vcgen_may_widen(_Tmp3,({struct Cyc_AssnDef_AssnMap _Tmp4=res;Cyc_AssnDef_or_assnmap_assnmap(_Tmp4,Cyc_Vcgen_vcgen_stmt(env,sc->body,amt));}));});
if(!env->widen_paths)
amin=amf;}}
# 1627
return res;}
# 1631
struct Cyc_AssnDef_AssnMap Cyc_Vcgen_clause2assn(struct Cyc_Absyn_Exp*e){
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_bogus_env();
struct Cyc_AssnDef_AssnMap am=({struct Cyc_Absyn_Vardecl*_Tmp0=Cyc_AssnDef_memory;void*_Tmp1=Cyc_AssnDef_fresh_var(0);Cyc_AssnDef_update_var_map(_Tmp0,_Tmp1,Cyc_AssnDef_true_assnmap());});
struct Cyc_AssnDef_AssnMap res=Cyc_Vcgen_vcgen_test(env,e,am).f0;
return res;}
# 1638
void Cyc_Vcgen_vcgen_fundecl(unsigned loc,struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info){
struct _tuple20 _Tmp0=Cyc_Vcgen_get_requires_and_ensures_info(& fd->i);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp0.f0;_Tmp5=_Tmp0.f1;_Tmp4=_Tmp0.f2;_Tmp3=_Tmp0.f3;_Tmp2=_Tmp0.f4;_Tmp1=_Tmp0.f5;{struct Cyc_List_List*args=_Tmp6;struct Cyc_List_List*arvds=_Tmp5;struct Cyc_AssnDef_AssnMap*reqopt=_Tmp4;struct Cyc_AssnDef_AssnMap*ensopt=_Tmp3;struct Cyc_AssnDef_AssnMap*thrwsopt=_Tmp2;struct Cyc_Absyn_Vardecl*ret_value=_Tmp1;
struct Cyc_AssnDef_AssnMap am=Cyc_AssnDef_true_assnmap();
# 1642
if(reqopt!=0)
am=*reqopt;{
# 1649
struct Cyc_List_List*actuals=0;
{struct Cyc_List_List*as=arvds;for(0;as!=0;as=as->tl){
struct Cyc_Absyn_Vardecl*vd=_check_null((struct Cyc_Absyn_Vardecl*)as->hd);
struct _tuple14 _Tmp7=Cyc_AssnDef_lookup_var_map(vd,am);struct Cyc_AssnDef_AssnMap _Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*actual=_Tmp9;struct Cyc_AssnDef_AssnMap am2=_Tmp8;
am=am2;
actuals=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));_TmpA->hd=actual,_TmpA->tl=actuals;_TmpA;});}}}
# 1656
actuals=Cyc_List_imp_rev(actuals);{
# 1658
struct _tuple14 _Tmp7=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f0;_Tmp8=_Tmp7.f1;{void*initmem=_Tmp9;struct Cyc_AssnDef_AssnMap am2=_Tmp8;
# 1663
struct Cyc_AssnDef_AssnMap*thrws_assn=0;
if(thrwsopt!=0)
thrws_assn=({struct Cyc_AssnDef_AssnMap*_TmpA=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));({struct Cyc_AssnDef_AssnMap _TmpB=Cyc_AssnDef_false_assnmap();*_TmpA=_TmpB;});_TmpA;});{
# 1668
struct Cyc_AssnDef_AssnMap*result_assn=ensopt==0?0:({struct Cyc_AssnDef_AssnMap*_TmpA=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));({struct Cyc_AssnDef_AssnMap _TmpB=Cyc_AssnDef_false_assnmap();*_TmpA=_TmpB;});_TmpA;});
# 1670
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_initial_env(tables,fd,assn_info,result_assn,thrws_assn,ret_value);
# 1673
struct Cyc_AssnDef_AssnMap amout=Cyc_Vcgen_vcgen_stmt(env,fd->body,am);
# 1676
Cyc_Vcgen_update_return(env,0,amout,0);
# 1679
if(ensopt!=0){
struct Cyc_AssnDef_AssnMap res=*_check_null(env->res_assnmap);
void*res_term=Cyc_AssnDef_fresh_var(0);
if(ret_value!=0){
struct _tuple14 _TmpA=Cyc_AssnDef_lookup_var_map(ret_value,res);struct Cyc_AssnDef_AssnMap _TmpB;void*_TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{void*r=_TmpC;struct Cyc_AssnDef_AssnMap a=_TmpB;
res_term=r;
res=a;}}{
# 1687
struct _tuple14 _TmpA=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,res);struct Cyc_AssnDef_AssnMap _TmpB;void*_TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{void*new_memory=_TmpC;struct Cyc_AssnDef_AssnMap res=_TmpB;
void*ens=Cyc_AssnDef_subst_args(arvds,ret_value,actuals,res_term,new_memory,*ensopt);
if(!Cyc_Vcgen_implies(res.assn,ens))
({struct Cyc_String_pa_PrintArg_struct _TmpD=({struct Cyc_String_pa_PrintArg_struct _TmpE;_TmpE.tag=0,({
struct _fat_ptr _TmpF=(struct _fat_ptr)Cyc_AssnDef_assn2string(ens);_TmpE.f1=_TmpF;});_TmpE;});struct Cyc_String_pa_PrintArg_struct _TmpE=({struct Cyc_String_pa_PrintArg_struct _TmpF;_TmpF.tag=0,({
struct _fat_ptr _Tmp10=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(*_check_null(env->res_assnmap));_TmpF.f1=_Tmp10;});_TmpF;});void*_TmpF[2];_TmpF[0]=& _TmpD,_TmpF[1]=& _TmpE;Cyc_Warn_err(loc,_tag_fat("cannot prove the @ensures clause\n\t%s\nfrom\n\t%s",sizeof(char),46U),_tag_fat(_TmpF,sizeof(void*),2));});}}}
# 1695
if(thrwsopt!=0){
struct Cyc_AssnDef_AssnMap res=*_check_null(env->try_assnmap);
struct _tuple14 _TmpA=({struct Cyc_Absyn_Vardecl*_TmpB=Cyc_AssnDef_exception_vardecl();Cyc_AssnDef_lookup_var_map(_TmpB,res);});struct Cyc_AssnDef_AssnMap _TmpB;void*_TmpC;_TmpC=_TmpA.f0;_TmpB=_TmpA.f1;{void*res_term=_TmpC;struct Cyc_AssnDef_AssnMap res=_TmpB;
struct _tuple14 _TmpD=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,res);struct Cyc_AssnDef_AssnMap _TmpE;void*_TmpF;_TmpF=_TmpD.f0;_TmpE=_TmpD.f1;{void*new_memory=_TmpF;struct Cyc_AssnDef_AssnMap res=_TmpE;
void*thr=({struct Cyc_List_List*_Tmp10=arvds;struct Cyc_Absyn_Vardecl*_Tmp11=Cyc_AssnDef_exception_vardecl();struct Cyc_List_List*_Tmp12=actuals;void*_Tmp13=res_term;void*_Tmp14=new_memory;Cyc_AssnDef_subst_args(_Tmp10,_Tmp11,_Tmp12,_Tmp13,_Tmp14,*thrwsopt);});
if(!Cyc_Vcgen_implies(res.assn,thr))
({struct Cyc_String_pa_PrintArg_struct _Tmp10=({struct Cyc_String_pa_PrintArg_struct _Tmp11;_Tmp11.tag=0,({
struct _fat_ptr _Tmp12=(struct _fat_ptr)Cyc_AssnDef_assn2string(thr);_Tmp11.f1=_Tmp12;});_Tmp11;});struct Cyc_String_pa_PrintArg_struct _Tmp11=({struct Cyc_String_pa_PrintArg_struct _Tmp12;_Tmp12.tag=0,({
struct _fat_ptr _Tmp13=(struct _fat_ptr)Cyc_AssnDef_assnmap2string(*_check_null(env->try_assnmap));_Tmp12.f1=_Tmp13;});_Tmp12;});void*_Tmp12[2];_Tmp12[0]=& _Tmp10,_Tmp12[1]=& _Tmp11;Cyc_Warn_err(loc,_tag_fat("cannot prove the @throws clause\n\t%s\nfrom\n\t%s",sizeof(char),45U),_tag_fat(_Tmp12,sizeof(void*),2));});}}}}}}}}}

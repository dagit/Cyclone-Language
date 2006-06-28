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
void* _throw_assert_fn(const char *,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw_assert() (_throw_assert_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
struct Cyc_Assert_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];
extern char Cyc_Assert[];

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
# 87 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 77 "flags.h"
extern int Cyc_Flags_warn_assert;
# 86
extern int Cyc_Flags_allpaths;
# 153 "flags.h"
extern unsigned Cyc_Flags_max_vc_summary;
# 37 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_SubsetType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_AssnDef_ExistAssnFn*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 533
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 551
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 565
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;int f3;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 733 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 920
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
int Cyc_Absyn_hash_qvar(struct _tuple2*);
# 928
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
# 935
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 941
void*Cyc_Absyn_compress(void*);
# 959
extern void*Cyc_Absyn_uint_type;
# 961
extern void*Cyc_Absyn_sint_type;
# 964
void*Cyc_Absyn_gen_float_type(unsigned);
# 968
extern void*Cyc_Absyn_heap_rgn_type;
# 970
extern void*Cyc_Absyn_al_qual_type;
# 974
extern void*Cyc_Absyn_false_type;
# 999
void*Cyc_Absyn_exn_type (void);
# 1011
void*Cyc_Absyn_bounds_one (void);
# 1025
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
# 1121
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1170
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple2*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1231
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1240
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1244
void Cyc_Absyn_visit_exp(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};struct Cyc_Set_Set;
# 77 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 96
extern int Cyc_Set_cardinality(struct Cyc_Set_Set*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;void*f2;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Alloc_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;void*f4;void*f5;};struct Cyc_AssnDef_Offseti_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};
# 151 "assndef.h"
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory;
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_pre_memory;
# 157
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_exception_vardecl (void);
# 160
extern void*Cyc_AssnDef_zero (void);
extern void*Cyc_AssnDef_one (void);
# 164
extern void*Cyc_AssnDef_uint(unsigned);
extern void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
extern void*Cyc_AssnDef_select(void*,void*,void*);
extern void*Cyc_AssnDef_update(void*,void*,void*);
# 169
extern void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop,void*,void*);
extern void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
extern void*Cyc_AssnDef_cast(void*,void*);
# 173
extern void*Cyc_AssnDef_tagof_tm(void*);
extern void*Cyc_AssnDef_proj(void*,unsigned,void*);
# 176
extern void*Cyc_AssnDef_addr(struct Cyc_Absyn_Vardecl*,void*);
extern void*Cyc_AssnDef_alloc(struct Cyc_Absyn_Exp*,void*,void*,void*);
# 179
extern void*Cyc_AssnDef_offsetf(void*,unsigned,void*);
extern void*Cyc_AssnDef_offseti(void*,void*,void*);
# 183
extern void*Cyc_AssnDef_numelts_term(void*);
extern void*Cyc_AssnDef_plus(void*,void*,void*);
# 186
extern void*Cyc_AssnDef_struct_aggr(struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_datatype_aggr(unsigned,struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_union_aggr(unsigned,struct Cyc_List_List*,void*);
extern void*Cyc_AssnDef_fresh_var(struct Cyc_Absyn_Vardecl*,void*);struct _tuple13{void*f0;struct Cyc_List_List*f1;};
# 194
extern struct _tuple13 Cyc_AssnDef_split_addr(void*);
# 202
extern void*Cyc_AssnDef_apply_aggr_update(void*,struct Cyc_List_List*,void*);
# 204
extern void*Cyc_AssnDef_get_term_type(void*);struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};
# 232 "assndef.h"
extern struct _fat_ptr Cyc_AssnDef_assn2string(void*);
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 236
extern void*Cyc_AssnDef_and(void*,void*);
extern void*Cyc_AssnDef_or(void*,void*);
extern void*Cyc_AssnDef_not(void*);
# 240
extern void*Cyc_AssnDef_eq(void*,void*);
extern void*Cyc_AssnDef_neq(void*,void*);
extern void*Cyc_AssnDef_slt(void*,void*);
extern void*Cyc_AssnDef_slte(void*,void*);
extern void*Cyc_AssnDef_ult(void*,void*);
extern void*Cyc_AssnDef_ulte(void*,void*);struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};
# 281
extern struct Cyc_AssnDef_ExistAssnFn*Cyc_AssnDef_bound_ts_in_assn(void*,struct Cyc_List_List*);
# 283
extern void*Cyc_AssnDef_existassnfn2assn(struct Cyc_AssnDef_ExistAssnFn*,struct Cyc_List_List*);
# 286
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_false_assnmap (void);
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_true_assnmap (void);
# 289
extern struct _fat_ptr Cyc_AssnDef_assnmap2dag(struct Cyc_AssnDef_AssnMap);
extern struct _fat_ptr Cyc_AssnDef_assnmap2string(struct Cyc_AssnDef_AssnMap);struct _tuple14{void*f0;struct Cyc_AssnDef_AssnMap f1;};
# 293
extern struct _tuple14 Cyc_AssnDef_lookup_var_map(struct Cyc_Absyn_Vardecl*,struct Cyc_AssnDef_AssnMap);
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_update_var_map(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_AssnMap);struct _tuple15{void*f0;void*f1;struct Cyc_Dict_Dict f2;};
# 297
extern struct _tuple15 Cyc_AssnDef_unify_var_maps_subst(struct Cyc_AssnDef_AssnMap,struct Cyc_AssnDef_AssnMap);
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_canonical_assnmap(struct Cyc_AssnDef_AssnMap);
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_force_canonical(struct Cyc_Set_Set*,struct Cyc_AssnDef_AssnMap);
# 303
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assnmap(struct Cyc_AssnDef_AssnMap,struct Cyc_AssnDef_AssnMap);
# 305
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_widen_assnmap(struct Cyc_AssnDef_AssnMap);
# 308
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assn(struct Cyc_AssnDef_AssnMap,void*);
# 311
extern struct Cyc_AssnDef_AssnMap Cyc_AssnDef_and_assnmap_assn(struct Cyc_AssnDef_AssnMap,void*);
# 315
extern int Cyc_AssnDef_simple_prove(void*,void*);
extern int Cyc_AssnDef_better_prove(void*,void*);
# 355 "assndef.h"
extern void*Cyc_AssnDef_widen(void*);
# 370
extern struct Cyc_Set_Set*Cyc_AssnDef_assn_fr_logicvar(void*);
# 374
extern struct Cyc_Set_Set*Cyc_AssnDef_calc_changed_vars_stmt(struct Cyc_Absyn_Stmt*);
extern struct Cyc_Set_Set*Cyc_AssnDef_calc_changed_vars_exp(struct Cyc_Absyn_Exp*);struct Cyc_Vcgen_ExpChecks{struct Cyc_AssnDef_AssnMap ctxt;void*bounds_check;void*null_check;};
# 41 "tcutil.h"
int Cyc_Tcutil_is_pointer_type(void*);
# 47
int Cyc_Tcutil_is_zeroterm_pointer_type(void*);
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 51
int Cyc_Tcutil_is_fat_pointer_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 70
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp_constrain(void*,void*,int);
# 73
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 226 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 242
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 246
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 253
int Cyc_Tcutil_force_type2bool(int,void*);struct _tuple16{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple16 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 43 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 160 "cf_flowinfo.h"
extern int Cyc_CfFlowInfo_get_field_index(void*,struct _fat_ptr*);
# 52 "attributes.h"
int Cyc_Atts_is_noreturn_fn_type(void*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 75 "vcgen.cyc"
static struct Cyc_Hashtable_Table*Cyc_Vcgen_fn_ens_info_table;
# 77
static void Cyc_Vcgen_insert_fn_info(struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List*pre_memory_and_actuals,void*result_term,void*final_memory,void*a){int _T0;unsigned _T1;unsigned _T2;struct Cyc_Hashtable_Table*(*_T3)(int,int(*)(struct _tuple2*,struct _tuple2*),int(*)(struct _tuple2*));struct Cyc_Hashtable_Table*(*_T4)(int,int(*)(void*,void*),int(*)(void*));int(*_T5)(struct _tuple2*,struct _tuple2*);int(*_T6)(struct _tuple2*);void(*_T7)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_AssnDef_ExistAssnFn*);void(*_T8)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T9;struct Cyc_Absyn_Fndecl*_TA;struct _tuple2*_TB;void*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_AssnDef_ExistAssnFn*_TF;
# 82
struct Cyc_Set_Set*fvs=Cyc_AssnDef_assn_fr_logicvar(a);_T0=
Cyc_Set_cardinality(fvs);_T1=(unsigned)_T0;_T2=Cyc_Flags_max_vc_summary;if(_T1 <= _T2)goto _TL0;
a=Cyc_AssnDef_widen(a);goto _TL1;_TL0: _TL1: {
# 86
struct Cyc_Hashtable_Table*h;
if(Cyc_Vcgen_fn_ens_info_table!=0)goto _TL2;_T4=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T10)(int,int(*)(struct _tuple2*,struct _tuple2*),int(*)(struct _tuple2*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple2*,struct _tuple2*),int(*)(struct _tuple2*)))_T4;_T3=_T10;}_T5=Cyc_Absyn_qvar_cmp;_T6=Cyc_Absyn_hash_qvar;h=_T3(37,_T5,_T6);
Cyc_Vcgen_fn_ens_info_table=h;goto _TL3;
_TL2: h=Cyc_Vcgen_fn_ens_info_table;_TL3: _T8=Cyc_Hashtable_insert;{
void(*_T10)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_AssnDef_ExistAssnFn*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_AssnDef_ExistAssnFn*))_T8;_T7=_T10;}_T9=h;_TA=fd;_TB=_TA->name;_TC=a;{struct Cyc_List_List*_T10=_cycalloc(sizeof(struct Cyc_List_List));_T10->hd=result_term;{struct Cyc_List_List*_T11=_cycalloc(sizeof(struct Cyc_List_List));_T11->hd=final_memory;_T11->tl=pre_memory_and_actuals;_TE=(struct Cyc_List_List*)_T11;}_T10->tl=_TE;_TD=(struct Cyc_List_List*)_T10;}_TF=Cyc_AssnDef_bound_ts_in_assn(_TC,_TD);_T7(_T9,_TB,_TF);}}
# 94
static void Cyc_Vcgen_insert_fn_precond_info(struct Cyc_Hashtable_Table*fn_precond_info,struct Cyc_Absyn_Exp*e,void*a,void*mem,struct Cyc_List_List*actuals){int _T0;unsigned _T1;unsigned _T2;void*_T3;struct Cyc_List_List*_T4;struct Cyc_Absyn_Exp*_T5;int*_T6;unsigned _T7;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T8;void*_T9;int*_TA;int _TB;void*_TC;struct Cyc_Absyn_Fndecl*_TD;int _TE;struct Cyc_List_List**(*_TF)(struct Cyc_Hashtable_Table*,struct _tuple2*);void**(*_T10)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_T11;struct Cyc_Absyn_Fndecl*_T12;struct _tuple2*_T13;void(*_T14)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_List_List*);void(*_T15)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T16;struct Cyc_Absyn_Fndecl*_T17;struct _tuple2*_T18;struct Cyc_List_List*_T19;void(*_T1A)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_List_List*);void(*_T1B)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T1C;struct Cyc_Absyn_Fndecl*_T1D;struct _tuple2*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List**_T20;
struct Cyc_Set_Set*fvs=Cyc_AssnDef_assn_fr_logicvar(a);_T0=
Cyc_Set_cardinality(fvs);_T1=(unsigned)_T0;_T2=Cyc_Flags_max_vc_summary;if(_T1 <= _T2)goto _TL4;
a=Cyc_AssnDef_widen(a);goto _TL5;_TL4: _TL5: _T3=a;{struct Cyc_List_List*_T21=_cycalloc(sizeof(struct Cyc_List_List));
# 99
_T21->hd=mem;_T21->tl=actuals;_T4=(struct Cyc_List_List*)_T21;}{struct Cyc_AssnDef_ExistAssnFn*eaf=Cyc_AssnDef_bound_ts_in_assn(_T3,_T4);
struct Cyc_Hashtable_Table*h=fn_precond_info;_T5=e;{
void*_T21=_T5->r;struct Cyc_Absyn_Exp*_T22;struct Cyc_Absyn_Fndecl*_T23;_T6=(int*)_T21;_T7=*_T6;switch(_T7){case 1: _T8=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T21;_T9=_T8->f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=2)goto _TL7;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T24=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T21;_TC=_T24->f1;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T25=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_TC;_T23=_T25->f1;}}{struct Cyc_Absyn_Fndecl*fd=_T23;_TD=fd;_TE=_TD->escapes;
# 104
if(!_TE)goto _TL9;goto _LL0;_TL9: _T10=Cyc_Hashtable_lookup_opt;{
struct Cyc_List_List**(*_T24)(struct Cyc_Hashtable_Table*,struct _tuple2*)=(struct Cyc_List_List**(*)(struct Cyc_Hashtable_Table*,struct _tuple2*))_T10;_TF=_T24;}_T11=h;_T12=fd;_T13=_T12->name;{struct Cyc_List_List**eafs_opt=_TF(_T11,_T13);
if(eafs_opt!=0)goto _TLB;_T15=Cyc_Hashtable_insert;{
void(*_T24)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_List_List*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_List_List*))_T15;_T14=_T24;}_T16=h;_T17=fd;_T18=_T17->name;{struct Cyc_List_List*_T24=_cycalloc(sizeof(struct Cyc_List_List));_T24->hd=eaf;_T24->tl=0;_T19=(struct Cyc_List_List*)_T24;}_T14(_T16,_T18,_T19);goto _TLC;
# 109
_TLB: _T1B=Cyc_Hashtable_insert;{void(*_T24)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_List_List*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple2*,struct Cyc_List_List*))_T1B;_T1A=_T24;}_T1C=h;_T1D=fd;_T1E=_T1D->name;{struct Cyc_List_List*_T24=_cycalloc(sizeof(struct Cyc_List_List));_T24->hd=eaf;_T20=eafs_opt;_T24->tl=*_T20;_T1F=(struct Cyc_List_List*)_T24;}_T1A(_T1C,_T1E,_T1F);_TLC: goto _LL0;}}_TL7: goto _LL7;case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T24=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T21;_T22=_T24->f1;}{struct Cyc_Absyn_Exp*e1=_T22;
# 111
Cyc_Vcgen_insert_fn_precond_info(h,e1,a,mem,actuals);goto _LL0;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T24=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T21;_T22=_T24->f1;}{struct Cyc_Absyn_Exp*e1=_T22;
Cyc_Vcgen_insert_fn_precond_info(h,e1,a,mem,actuals);goto _LL0;}default: _LL7: goto _LL0;}_LL0:;}}}
# 118
static void Cyc_Vcgen_add_free_var(struct Cyc_Dict_Dict*S,void*x){struct Cyc_Dict_Dict*_T0;struct Cyc_Dict_Dict*_T1;struct Cyc_Dict_Dict _T2;void*_T3;void*_T4;void*_T5;_T0=S;_T1=S;_T2=*_T1;_T3=x;_T4=
Cyc_AssnDef_get_term_type(x);_T5=Cyc_AssnDef_fresh_var(0,_T4);*_T0=Cyc_Dict_insert(_T2,_T3,_T5);}
# 122
static struct Cyc_AssnDef_ExistAssnFn*Cyc_Vcgen_lookup_fn_summary_info(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;struct Cyc_AssnDef_ExistAssnFn**(*_T8)(struct Cyc_Hashtable_Table*,struct _tuple2*);void**(*_T9)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TA;struct Cyc_Absyn_Fndecl*_TB;struct _tuple2*_TC;struct Cyc_AssnDef_ExistAssnFn**_TD;struct Cyc_AssnDef_ExistAssnFn*_TE;void*_TF;struct Cyc_AssnDef_ExistAssnFn**(*_T10)(struct Cyc_Hashtable_Table*,struct _tuple2*);void**(*_T11)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_T12;struct Cyc_Absyn_Vardecl*_T13;struct _tuple2*_T14;struct Cyc_AssnDef_ExistAssnFn**_T15;struct Cyc_AssnDef_ExistAssnFn*_T16;struct Cyc_AssnDef_ExistAssnFn*_T17;
if(Cyc_Vcgen_fn_ens_info_table!=0)goto _TLD;return 0;_TLD: {
struct Cyc_Hashtable_Table*h=Cyc_Vcgen_fn_ens_info_table;_T0=e;{
void*_T18=_T0->r;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Vardecl*_T1A;struct Cyc_Absyn_Fndecl*_T1B;_T1=(int*)_T18;_T2=*_T1;switch(_T2){case 1: _T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T18;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 2:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T18;_T7=_T1C->f1;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T1D=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T7;_T1B=_T1D->f1;}}{struct Cyc_Absyn_Fndecl*fd=_T1B;_T9=Cyc_Hashtable_lookup_opt;{
# 127
struct Cyc_AssnDef_ExistAssnFn**(*_T1C)(struct Cyc_Hashtable_Table*,struct _tuple2*)=(struct Cyc_AssnDef_ExistAssnFn**(*)(struct Cyc_Hashtable_Table*,struct _tuple2*))_T9;_T8=_T1C;}_TA=h;_TB=fd;_TC=_TB->name;{struct Cyc_AssnDef_ExistAssnFn**fensinfo_opt=_T8(_TA,_TC);
if(fensinfo_opt!=0)goto _TL11;return 0;
_TL11: _TD=fensinfo_opt;_TE=*_TD;return _TE;}}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T18;_TF=_T1C->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T1D=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_TF;_T1A=_T1D->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T1A;_T11=Cyc_Hashtable_lookup_opt;{
# 131
struct Cyc_AssnDef_ExistAssnFn**(*_T1C)(struct Cyc_Hashtable_Table*,struct _tuple2*)=(struct Cyc_AssnDef_ExistAssnFn**(*)(struct Cyc_Hashtable_Table*,struct _tuple2*))_T11;_T10=_T1C;}_T12=h;_T13=vd;_T14=_T13->name;{struct Cyc_AssnDef_ExistAssnFn**fensinfo_opt=_T10(_T12,_T14);
if(fensinfo_opt!=0)goto _TL13;return 0;
_TL13: _T15=fensinfo_opt;_T16=*_T15;return _T16;}}default: goto _LL7;};case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T18;_T19=_T1C->f1;}{struct Cyc_Absyn_Exp*e1=_T19;_T17=
Cyc_Vcgen_lookup_fn_summary_info(e1);return _T17;}default: _LL7:
 return 0;};}}}struct Cyc_Vcgen_Env{struct Cyc_Hashtable_Table*assn_table;struct Cyc_Hashtable_Table*succ_table;struct Cyc_Hashtable_Table**assn_info;struct Cyc_Hashtable_Table*fn_precond_info;struct Cyc_AssnDef_AssnMap*try_assnmap;void**exp_stmt;struct Cyc_Absyn_Vardecl*result;struct Cyc_AssnDef_AssnMap*res_assnmap;struct Cyc_AssnDef_AssnMap*continue_assnmap;int widen_paths;int pure_exp;void*invoke;};
# 175
static int Cyc_Vcgen_hash_ptr(void*s){void*_T0;int _T1;_T0=s;_T1=(int)_T0;return _T1;}
# 179
static struct Cyc_Vcgen_Env*Cyc_Vcgen_initial_env(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,struct Cyc_Hashtable_Table*fn_precond_info,struct Cyc_AssnDef_AssnMap*res_assnmap,struct Cyc_AssnDef_AssnMap*try_assnmap,struct Cyc_Absyn_Vardecl*result){struct Cyc_Hashtable_Table*(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*);void*(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_JumpAnalysis_Jump_Anal_Result*_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Fndecl*_T4;struct Cyc_Hashtable_Table*(*_T5)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*));struct Cyc_Hashtable_Table*(*_T6)(int,int(*)(void*,void*),int(*)(void*));int(*_T7)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*);int(*_T8)(void*,void*);int(*_T9)(struct Cyc_Absyn_Stmt*);struct Cyc_Vcgen_Env*_TA;struct Cyc_Vcgen_Env*_TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_Vcgen_Env*_TD;struct Cyc_Vcgen_Env*_TE;struct Cyc_Vcgen_Env*_TF;struct Cyc_Vcgen_Env*_T10;struct Cyc_Vcgen_Env*_T11;struct Cyc_Vcgen_Env*_T12;struct Cyc_Vcgen_Env*_T13;struct Cyc_Vcgen_Env*_T14;struct Cyc_Vcgen_Env*_T15;struct Cyc_Vcgen_Env*_T16;_T1=Cyc_Hashtable_lookup;{
# 184
struct Cyc_Hashtable_Table*(*_T17)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*)=(struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Fndecl*))_T1;_T0=_T17;}_T2=tables;_T3=_T2->succ_tables;_T4=fd;{struct Cyc_Hashtable_Table*succ_table=_T0(_T3,_T4);_T6=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T17)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))_T6;_T5=_T17;}_T8=Cyc_Core_ptrcmp;{int(*_T17)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))_T8;_T7=_T17;}{int(*_T17)(struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;_T9=_T17;}{struct Cyc_Hashtable_Table*assn_table=_T5(57,_T7,_T9);
struct Cyc_Vcgen_Env*env;env=_cycalloc(sizeof(struct Cyc_Vcgen_Env));_TA=env;_TA->assn_table=assn_table;_TB=env;
_TB->succ_table=succ_table;_TC=env;
_TC->assn_info=assn_info;_TD=env;
_TD->fn_precond_info=fn_precond_info;_TE=env;
_TE->try_assnmap=try_assnmap;_TF=env;
_TF->exp_stmt=0;_T10=env;
_T10->result=result;_T11=env;
_T11->res_assnmap=res_assnmap;_T12=env;
_T12->continue_assnmap=0;_T13=env;
_T13->widen_paths=!Cyc_Flags_allpaths;_T14=env;
_T14->pure_exp=0;_T15=env;
_T15->invoke=Cyc_AssnDef_fresh_var(0,0);_T16=env;
return _T16;}}}
# 204
static struct Cyc_Vcgen_Env*Cyc_Vcgen_bogus_env (void){struct Cyc_Hashtable_Table*(*_T0)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*));struct Cyc_Hashtable_Table*(*_T1)(int,int(*)(void*,void*),int(*)(void*));int(*_T2)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*);int(*_T3)(void*,void*);int(*_T4)(struct Cyc_Absyn_Stmt*);struct Cyc_Hashtable_Table*(*_T5)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*));struct Cyc_Hashtable_Table*(*_T6)(int,int(*)(void*,void*),int(*)(void*));int(*_T7)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*);int(*_T8)(void*,void*);int(*_T9)(struct Cyc_Absyn_Stmt*);struct Cyc_Hashtable_Table**_TA;struct Cyc_Hashtable_Table*(*_TB)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*));struct Cyc_Hashtable_Table*(*_TC)(int,int(*)(void*,void*),int(*)(void*));int(*_TD)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);int(*_TE)(void*,void*);int(*_TF)(struct Cyc_Absyn_Exp*);struct Cyc_Hashtable_Table*(*_T10)(int,int(*)(struct _tuple2*,struct _tuple2*),int(*)(struct _tuple2*));struct Cyc_Hashtable_Table*(*_T11)(int,int(*)(void*,void*),int(*)(void*));int(*_T12)(struct _tuple2*,struct _tuple2*);int(*_T13)(void*,void*);int(*_T14)(struct _tuple2*);struct Cyc_Vcgen_Env*_T15;struct Cyc_Vcgen_Env*_T16;struct Cyc_Vcgen_Env*_T17;struct Cyc_Vcgen_Env*_T18;struct Cyc_Vcgen_Env*_T19;struct Cyc_Vcgen_Env*_T1A;struct Cyc_Vcgen_Env*_T1B;struct Cyc_Vcgen_Env*_T1C;struct Cyc_Vcgen_Env*_T1D;struct Cyc_Vcgen_Env*_T1E;struct Cyc_Vcgen_Env*_T1F;struct Cyc_Vcgen_Env*_T20;struct Cyc_Vcgen_Env*_T21;_T1=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T22)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))_T1;_T0=_T22;}_T3=Cyc_Core_ptrcmp;{int(*_T22)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))_T3;_T2=_T22;}{int(*_T22)(struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;_T4=_T22;}{struct Cyc_Hashtable_Table*succ_table=_T0(1,_T2,_T4);_T6=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T22)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*)(struct Cyc_Absyn_Stmt*)))_T6;_T5=_T22;}_T8=Cyc_Core_ptrcmp;{int(*_T22)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))_T8;_T7=_T22;}{int(*_T22)(struct Cyc_Absyn_Stmt*)=(int(*)(struct Cyc_Absyn_Stmt*))Cyc_Vcgen_hash_ptr;_T9=_T22;}{struct Cyc_Hashtable_Table*assn_table=_T5(1,_T7,_T9);
struct Cyc_Hashtable_Table**assn_info;assn_info=_cycalloc(sizeof(struct Cyc_Hashtable_Table*));_TA=assn_info;_TC=Cyc_Hashtable_create;{struct Cyc_Hashtable_Table*(*_T22)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),int(*)(struct Cyc_Absyn_Exp*)))_TC;_TB=_T22;}_TE=Cyc_Core_ptrcmp;{int(*_T22)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))_TE;_TD=_T22;}{int(*_T22)(struct Cyc_Absyn_Exp*)=(int(*)(struct Cyc_Absyn_Exp*))Cyc_Vcgen_hash_ptr;_TF=_T22;}*_TA=_TB(1,_TD,_TF);_T11=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T22)(int,int(*)(struct _tuple2*,struct _tuple2*),int(*)(struct _tuple2*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _tuple2*,struct _tuple2*),int(*)(struct _tuple2*)))_T11;_T10=_T22;}_T13=Cyc_Core_ptrcmp;{int(*_T22)(struct _tuple2*,struct _tuple2*)=(int(*)(struct _tuple2*,struct _tuple2*))_T13;_T12=_T22;}{int(*_T22)(struct _tuple2*)=(int(*)(struct _tuple2*))Cyc_Vcgen_hash_ptr;_T14=_T22;}{struct Cyc_Hashtable_Table*fn_precond_info=_T10(1,_T12,_T14);
struct Cyc_Vcgen_Env*env;env=_cycalloc(sizeof(struct Cyc_Vcgen_Env));_T15=env;_T15->assn_table=assn_table;_T16=env;
_T16->succ_table=succ_table;_T17=env;
_T17->assn_info=assn_info;_T18=env;
_T18->fn_precond_info=fn_precond_info;_T19=env;
_T19->try_assnmap=0;_T1A=env;
_T1A->exp_stmt=0;_T1B=env;
_T1B->result=0;_T1C=env;
_T1C->res_assnmap=0;_T1D=env;
_T1D->continue_assnmap=0;_T1E=env;
_T1E->widen_paths=0;_T1F=env;
_T1F->pure_exp=1;_T20=env;
_T20->invoke=Cyc_AssnDef_fresh_var(0,0);_T21=env;
return _T21;}}}}
# 225
static void*Cyc_Vcgen_myaddr(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl*_T0;struct Cyc_Vcgen_Env*_T1;void*_T2;void*_T3;_T0=vd;_T1=env;_T2=_T1->invoke;_T3=
Cyc_AssnDef_addr(_T0,_T2);return _T3;}
# 228
static void*Cyc_Vcgen_myalloc(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,void*nelts,void*tp){struct Cyc_Absyn_Exp*_T0;void*_T1;struct Cyc_Vcgen_Env*_T2;void*_T3;void*_T4;void*_T5;_T0=e;_T1=nelts;_T2=env;_T3=_T2->invoke;_T4=tp;_T5=
Cyc_AssnDef_alloc(_T0,_T1,_T3,_T4);return _T5;}
# 233
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_may_widen(struct Cyc_Vcgen_Env*env,struct Cyc_AssnDef_AssnMap am){struct Cyc_Vcgen_Env*_T0;int _T1;struct Cyc_AssnDef_AssnMap _T2;struct Cyc_AssnDef_AssnMap _T3;_T0=env;_T1=_T0->widen_paths;
if(!_T1)goto _TL15;_T2=
Cyc_AssnDef_widen_assnmap(am);return _T2;_TL15: _T3=am;
# 237
return _T3;}
# 244
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_lookup_stmt_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s){struct Cyc_AssnDef_AssnMap**(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Vcgen_Env*_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Stmt*_T4;struct Cyc_AssnDef_AssnMap**_T5;struct Cyc_AssnDef_AssnMap*_T6;struct Cyc_AssnDef_AssnMap _T7;struct Cyc_AssnDef_AssnMap _T8;_T1=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnMap**(*_T9)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_AssnDef_AssnMap**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T9;}_T2=env;_T3=_T2->assn_table;_T4=s;{struct Cyc_AssnDef_AssnMap**preconopt=_T0(_T3,_T4);
if(preconopt==0)goto _TL17;_T5=preconopt;_T6=*_T5;_T7=*_T6;return _T7;
_TL17: _T8=Cyc_AssnDef_false_assnmap();return _T8;}}
# 251
static void Cyc_Vcgen_update_stmt_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,struct Cyc_AssnDef_AssnMap newam){struct Cyc_AssnDef_AssnMap**(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Vcgen_Env*_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Stmt*_T4;struct Cyc_AssnDef_AssnMap**_T5;struct Cyc_AssnDef_AssnMap*_T6;struct Cyc_Vcgen_Env*_T7;struct Cyc_AssnDef_AssnMap**_T8;struct Cyc_AssnDef_AssnMap*_T9;struct Cyc_AssnDef_AssnMap _TA;struct Cyc_AssnDef_AssnMap _TB;struct Cyc_AssnDef_AssnMap _TC;void(*_TD)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*);void(*_TE)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Vcgen_Env*_TF;struct Cyc_Hashtable_Table*_T10;struct Cyc_Absyn_Stmt*_T11;struct Cyc_AssnDef_AssnMap*_T12;_T1=Cyc_Hashtable_lookup_opt;{
struct Cyc_AssnDef_AssnMap**(*_T13)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_AssnDef_AssnMap**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T13;}_T2=env;_T3=_T2->assn_table;_T4=s;{struct Cyc_AssnDef_AssnMap**preconopt=_T0(_T3,_T4);
if(preconopt==0)goto _TL19;_T5=preconopt;_T6=*_T5;_T7=env;_T8=preconopt;_T9=*_T8;_TA=*_T9;_TB=newam;_TC=Cyc_AssnDef_or_assnmap_assnmap(_TA,_TB);*_T6=Cyc_Vcgen_may_widen(_T7,_TC);goto _TL1A;
_TL19: _TE=Cyc_Hashtable_insert;{void(*_T13)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*))_TE;_TD=_T13;}_TF=env;_T10=_TF->assn_table;_T11=s;{struct Cyc_AssnDef_AssnMap*_T13=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));*_T13=newam;_T12=(struct Cyc_AssnDef_AssnMap*)_T13;}_TD(_T10,_T11,_T12);_TL1A:;}}
# 259
static void Cyc_Vcgen_replace_stmt_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,struct Cyc_AssnDef_AssnMap newam){void(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*);void(*_T1)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Vcgen_Env*_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Stmt*_T4;struct Cyc_AssnDef_AssnMap*_T5;_T1=Cyc_Hashtable_insert;{
void(*_T6)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap*))_T1;_T0=_T6;}_T2=env;_T3=_T2->assn_table;_T4=s;{struct Cyc_AssnDef_AssnMap*_T6=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));*_T6=newam;_T5=(struct Cyc_AssnDef_AssnMap*)_T6;}_T0(_T3,_T4,_T5);}
# 266
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_stmt(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Stmt*,struct Cyc_AssnDef_AssnMap);
# 270
static struct _tuple14 Cyc_Vcgen_vcgen_rexp(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,struct Cyc_AssnDef_AssnMap);struct _tuple17{struct Cyc_List_List*f0;struct Cyc_AssnDef_AssnMap f1;};
# 273
static struct _tuple17 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*,struct Cyc_List_List*,struct Cyc_AssnDef_AssnMap);
# 276
static struct _tuple14 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,struct Cyc_AssnDef_AssnMap);struct _tuple18{struct Cyc_AssnDef_AssnMap f0;struct Cyc_AssnDef_AssnMap f1;};
# 280
static struct _tuple18 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Exp*,struct Cyc_AssnDef_AssnMap);
# 285
static struct _tuple18 Cyc_Vcgen_vcgen_pat(struct Cyc_Vcgen_Env*,struct Cyc_Absyn_Pat*,void*,void*,struct Cyc_AssnDef_AssnMap);
# 288
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*,void*,struct Cyc_List_List*,struct Cyc_AssnDef_AssnMap);
# 404 "vcgen.cyc"
static int Cyc_Vcgen_implies(void*a1,void*a2){int _T0;int _T1;_T1=
Cyc_AssnDef_simple_prove(a1,a2);if(!_T1)goto _TL1B;_T0=1;goto _TL1C;_TL1B: _T0=Cyc_AssnDef_better_prove(a1,a2);_TL1C: return _T0;}
# 409
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;void*_T5;struct Cyc_Absyn_Fndecl*_T6;struct Cyc_Absyn_Vardecl*_T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr);void*(*_T9)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TA;struct _fat_ptr _TB;void*_TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;struct Cyc_Absyn_Vardecl*_TF;void*_T10;struct Cyc_Absyn_Vardecl*_T11;struct Cyc_Absyn_Fndecl*_T12;struct Cyc_Absyn_Vardecl*_T13;_T0=b;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 1: _T3=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T3;_T13=_T14->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13;_T4=vd;
# 411
return _T4;}case 2: _T5=b;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T5;_T12=_T14->f1;}{struct Cyc_Absyn_Fndecl*fd=_T12;_T6=fd;_T7=_T6->fn_vardecl;
return _T7;}case 0: _T9=Cyc_Warn_impos;{
int(*_T14)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T9;_T8=_T14;}_TA=_tag_fat("vcgen bind2vardecl",sizeof(char),19U);_TB=_tag_fat(0U,sizeof(void*),0);_T8(_TA,_TB);case 3: _TC=b;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_TC;_T13=_T14->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13;_TD=vd;
return _TD;}case 4: _TE=b;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TE;_T13=_T14->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13;_TF=vd;
return _TF;}default: _T10=b;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T10;_T13=_T14->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13;_T11=vd;
return _T11;}};}
# 421
static int Cyc_Vcgen_is_unsigned_int_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;int _T8;void*_T9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA;void*_TB;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TC;enum Cyc_Absyn_Sign _TD;void*_TE;void*_TF;enum Cyc_Absyn_Size_of _T10;int _T11;enum Cyc_Absyn_Size_of _T12;int _T13;_T0=e;{
void*t=_T0->topt;
if(t!=0)goto _TL1E;return 0;_TL1E: {enum Cyc_Absyn_Size_of _T14;if(t==0)goto _TL20;_T1=t;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL22;_T4=t;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=1)goto _TL24;_T9=t;_TA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9;_TB=_TA->f1;_TC=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TB;_TD=_TC->f1;if(_TD!=Cyc_Absyn_Unsigned)goto _TL26;_TE=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE;_TF=_T15->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T16=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TF;_T14=_T16->f2;}}{enum Cyc_Absyn_Size_of sz=_T14;_T10=sz;_T11=(int)_T10;
# 427
if(_T11==3)goto _TL2A;else{goto _TL2B;}_TL2B: _T12=sz;_T13=(int)_T12;if(_T13==2)goto _TL2A;else{goto _TL28;}
_TL2A: return 1;
_TL28: return 0;}_TL26: goto _LL3;_TL24: goto _LL3;_TL22: goto _LL3;_TL20: _LL3:
 return 0;;}}}
# 439
static struct _tuple14 Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,struct Cyc_AssnDef_AssnMap am){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;int _T5;struct _tuple14 _T6;struct _tuple14 _T7;struct Cyc_Absyn_Vardecl*_T8;_T0=lt;_T1=(int*)_T0;_T2=*_T1;if(_T2!=11)goto _TL2C;_T3=lt;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T9=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T3;_T8=_T9->f1;}{struct Cyc_Absyn_Vardecl*vd=_T8;_T4=vd;_T5=_T4->escapes;
# 442
if(_T5)goto _TL2E;else{goto _TL30;}
_TL30: _T6=Cyc_AssnDef_lookup_var_map(vd,am);return _T6;_TL2E: goto _LL4;}_TL2C: _LL4:{
# 447
struct _tuple14 _T9=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _TA;void*_TB;_TB=_T9.f0;_TA=_T9.f1;{void*mem=_TB;struct Cyc_AssnDef_AssnMap am1=_TA;{struct _tuple14 _TC;
_TC.f0=Cyc_AssnDef_select(mem,lt,0);_TC.f1=am1;_T7=_TC;}return _T7;}};}
# 455
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,struct Cyc_AssnDef_AssnMap am,void*lt,void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;int _T5;struct Cyc_Absyn_Vardecl*_T6;void*_T7;struct Cyc_AssnDef_AssnMap _T8;struct Cyc_AssnDef_AssnMap _T9;struct Cyc_Absyn_Vardecl*_TA;void*_TB;struct Cyc_AssnDef_AssnMap _TC;struct Cyc_AssnDef_AssnMap _TD;
struct _tuple13 _TE=Cyc_AssnDef_split_addr(lt);struct Cyc_List_List*_TF;void*_T10;_T10=_TE.f0;_TF=_TE.f1;{void*root=_T10;struct Cyc_List_List*fields=_TF;struct Cyc_Absyn_Vardecl*_T11;_T0=root;_T1=(int*)_T0;_T2=*_T1;if(_T2!=11)goto _TL31;_T3=root;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T12=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T3;_T11=_T12->f1;}_T4=(struct Cyc_Absyn_Vardecl*)_T11;_T5=_T4->escapes;if(_T5)goto _TL33;else{goto _TL35;}_TL35:{struct Cyc_Absyn_Vardecl*vd=_T11;
# 459
struct _tuple14 _T12=Cyc_AssnDef_lookup_var_map(vd,am);struct Cyc_AssnDef_AssnMap _T13;void*_T14;_T14=_T12.f0;_T13=_T12.f1;{void*oldv=_T14;struct Cyc_AssnDef_AssnMap am1=_T13;_T6=vd;_T7=
Cyc_AssnDef_apply_aggr_update(oldv,fields,t);_T8=am1;_T9=Cyc_AssnDef_update_var_map(_T6,_T7,_T8);return _T9;}}_TL33: goto _LL6;_TL31: _LL6:{
# 462
struct _tuple14 _T12=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _T13;void*_T14;_T14=_T12.f0;_T13=_T12.f1;{void*mem=_T14;struct Cyc_AssnDef_AssnMap am1=_T13;_TA=Cyc_AssnDef_memory;_TB=
Cyc_AssnDef_update(mem,lt,t);_TC=am1;_TD=Cyc_AssnDef_update_var_map(_TA,_TB,_TC);return _TD;}};}}
# 469
static void Cyc_Vcgen_update_try_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_AssnDef_AssnMap am,void*exn_value){struct Cyc_Vcgen_Env*_T0;struct Cyc_AssnDef_AssnMap*_T1;struct Cyc_Vcgen_Env*_T2;struct Cyc_AssnDef_AssnMap _T3;struct Cyc_Vcgen_Env*_T4;struct Cyc_Absyn_Vardecl*_T5;void*_T6;void*_T7;struct Cyc_Vcgen_Env*_T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_AssnDef_AssnMap*_TA;struct Cyc_AssnDef_AssnMap*_TB;struct Cyc_AssnDef_AssnMap _TC;struct Cyc_AssnDef_AssnMap _TD;struct Cyc_AssnDef_AssnMap _TE;struct Cyc_Vcgen_Env*_TF;struct Cyc_AssnDef_AssnMap*_T10;struct Cyc_AssnDef_AssnMap*_T11;_T0=env;_T1=_T0->try_assnmap;
if(_T1!=0)goto _TL36;return;_TL36: _T2=env;_T3=am;_T4=env;_T5=
Cyc_AssnDef_exception_vardecl();_T6=Cyc_Vcgen_myaddr(_T4,_T5);_T7=exn_value;am=Cyc_Vcgen_do_assign(_T2,_T3,_T6,_T7);_T8=env;_T9=env;_TA=_T9->try_assnmap;_TB=
_check_null(_TA);_TC=*_TB;_TD=am;_TE=Cyc_AssnDef_or_assnmap_assnmap(_TC,_TD);am=Cyc_Vcgen_may_widen(_T8,_TE);_TF=env;_T10=_TF->try_assnmap;_T11=
_check_null(_T10);*_T11=am;}
# 476
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Decl*_T0;int*_T1;unsigned _T2;struct Cyc_Vcgen_Env*_T3;struct Cyc_AssnDef_AssnMap _T4;void*_T5;struct Cyc_Absyn_Vardecl*_T6;struct Cyc_Absyn_Vardecl*_T7;void*_T8;void*_T9;struct Cyc_Absyn_Vardecl*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_Absyn_Vardecl*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_AssnDef_AssnMap _TF;struct Cyc_Vcgen_Env*_T10;struct Cyc_AssnDef_AssnMap _T11;void*_T12;void*_T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_AssnMap _T15;void*_T16;struct Cyc_AssnDef_AssnMap _T17;void*_T18;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T19;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T1A;void*_T1B;struct Cyc_Vcgen_Env*_T1C;struct Cyc_AssnDef_AssnMap _T1D;void*_T1E;void*_T1F;struct Cyc_AssnDef_AssnMap _T20;struct Cyc_AssnDef_AssnMap _T21;struct Cyc_Vcgen_Env*_T22;struct Cyc_AssnDef_AssnMap _T23;void*_T24;void*_T25;struct Cyc_AssnDef_AssnMap _T26;struct Cyc_AssnDef_AssnMap _T27;struct Cyc_AssnDef_AssnMap _T28;_T0=d;{
void*_T29=_T0->r;struct Cyc_Absyn_Vardecl*_T2A;struct Cyc_List_List*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Pat*_T2D;void*_T2E;_T1=(int*)_T29;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T29;_T2E=_T2F->f1;}{struct Cyc_Absyn_Vardecl*vd=_T2E;_T3=env;_T4=amin;_T5=
# 481
Cyc_Vcgen_myaddr(env,vd);_T6=vd;_T7=vd;_T8=_T7->type;_T9=Cyc_AssnDef_fresh_var(_T6,_T8);amin=Cyc_Vcgen_do_assign(_T3,_T4,_T5,_T9);_TA=vd;_TB=_TA->initializer;
if(_TB==0)goto _TL39;_TC=env;_TD=vd;_TE=_TD->initializer;_TF=amin;{
struct _tuple14 _T2F=Cyc_Vcgen_vcgen_rexp(_TC,_TE,_TF);struct Cyc_AssnDef_AssnMap _T30;void*_T31;_T31=_T2F.f0;_T30=_T2F.f1;{void*t=_T31;struct Cyc_AssnDef_AssnMap am=_T30;_T10=env;_T11=am;_T12=
Cyc_Vcgen_myaddr(env,vd);_T13=t;am=Cyc_Vcgen_do_assign(_T10,_T11,_T12,_T13);_T14=am;
return _T14;}}_TL39: _T15=amin;
# 487
return _T15;}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T2F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T29;_T2D=_T2F->f1;_T2C=_T2F->f3;_T16=_T2F->f4;_T2E=(void*)_T16;}{struct Cyc_Absyn_Pat*p=_T2D;struct Cyc_Absyn_Exp*e=_T2C;void*dtree=_T2E;
# 489
struct _tuple14 _T2F=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T30;void*_T31;_T31=_T2F.f0;_T30=_T2F.f1;{void*v=_T31;struct Cyc_AssnDef_AssnMap am=_T30;
struct _tuple18 _T32=Cyc_Vcgen_vcgen_pat(env,p,0,v,am);struct Cyc_AssnDef_AssnMap _T33;struct Cyc_AssnDef_AssnMap _T34;_T34=_T32.f0;_T33=_T32.f1;{struct Cyc_AssnDef_AssnMap amt=_T34;struct Cyc_AssnDef_AssnMap amf=_T33;_T17=amf;_T18=_T17.assn;_T19=& Cyc_AssnDef_false_assn;_T1A=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T19;_T1B=(void*)_T1A;
# 494
if(_T18==_T1B)goto _TL3B;_T1C=env;_T1D=amf;_T1E=
Cyc_Absyn_exn_type();_T1F=Cyc_AssnDef_fresh_var(0,_T1E);Cyc_Vcgen_update_try_assnmap(_T1C,_T1D,_T1F);goto _TL3C;_TL3B: _TL3C: _T20=amt;
return _T20;}}}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T2F=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T29;_T2B=_T2F->f1;}{struct Cyc_List_List*vds=_T2B;_T21=amin;
# 500
return _T21;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T2F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T29;_T2A=_T2F->f2;_T2C=_T2F->f3;}{struct Cyc_Absyn_Vardecl*vd=_T2A;struct Cyc_Absyn_Exp*eopt=_T2C;
# 502
if(eopt==0)goto _TL3D;{
struct _tuple14 _T2F=Cyc_Vcgen_vcgen_rexp(env,eopt,amin);struct Cyc_AssnDef_AssnMap _T30;void*_T31;_T31=_T2F.f0;_T30=_T2F.f1;{void*t=_T31;struct Cyc_AssnDef_AssnMap am=_T30;_T22=env;_T23=am;_T24=
Cyc_Vcgen_myaddr(env,vd);_T25=t;am=Cyc_Vcgen_do_assign(_T22,_T23,_T24,_T25);_T26=am;
return _T26;}}_TL3D: _T27=amin;
# 507
return _T27;}default: _T28=amin;
return _T28;};}}struct _tuple19{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_AssnDef_ExistAssnFn*f2;struct Cyc_AssnDef_ExistAssnFn*f3;struct Cyc_AssnDef_ExistAssnFn*f4;struct Cyc_AssnDef_ExistAssnFn*f5;struct Cyc_Absyn_Vardecl*f6;};
# 514
static struct _tuple19 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){struct _tuple19 _T0;struct Cyc_Absyn_FnInfo*_T1;struct Cyc_Absyn_FnInfo*_T2;struct Cyc_Absyn_FnInfo*_T3;struct Cyc_Absyn_FnInfo*_T4;struct Cyc_Absyn_FnInfo*_T5;struct Cyc_Absyn_FnInfo*_T6;struct Cyc_Absyn_FnInfo*_T7;{struct _tuple19 _T8;_T1=fi;
_T8.f0=_T1->args;_T2=fi;_T8.f1=_T2->arg_vardecls;_T3=fi;_T8.f2=_T3->checks_assn;_T4=fi;_T8.f3=_T4->requires_assn;_T5=fi;
_T8.f4=_T5->ensures_assn;_T6=fi;_T8.f5=_T6->throws_assn;_T7=fi;_T8.f6=_T7->return_value;_T0=_T8;}
# 515
return _T0;}
# 519
static struct _tuple19 Cyc_Vcgen_get_requires_and_ensures(void*topt){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T5;struct Cyc_Absyn_FnInfo*_T6;struct _tuple19 _T7;struct _tuple19 _T8;
if(topt==0)goto _TL3F;{
void*t=Cyc_Tcutil_pointer_elt_type(topt);void*_T9;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=6)goto _TL41;_T3=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_TA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3;_T4=t;_T5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T4;_T6=& _T5->f1;_T9=(struct Cyc_Absyn_FnInfo*)_T6;}{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_T9;_T7=
# 523
Cyc_Vcgen_get_requires_and_ensures_info(fi);return _T7;}_TL41: goto _LL0;_LL0:;}goto _TL40;_TL3F: _TL40:{struct _tuple19 _T9;
# 527
_T9.f0=0;_T9.f1=0;_T9.f2=0;_T9.f3=0;_T9.f4=0;_T9.f5=0;_T9.f6=0;_T8=_T9;}return _T8;}
# 530
static unsigned Cyc_Vcgen_datatype_constructor_index(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){struct Cyc_Absyn_Datatypedecl*_T0;struct Cyc_Core_Opt*_T1;struct Cyc_Core_Opt*_T2;void*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_Absyn_Datatypefield*_T6;struct Cyc_Absyn_Datatypefield*_T7;unsigned _T8;struct Cyc_Absyn_Datatypefield*_T9;struct _tuple2*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_Absyn_Datatypefield*_TD;struct _tuple2*_TE;int _TF;unsigned _T10;struct Cyc_List_List*_T11;int(*_T12)(struct _fat_ptr,struct _fat_ptr);void*(*_T13)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T14;struct _fat_ptr _T15;
# 532
unsigned i=0U;_T0=dd;_T1=_T0->fields;_T2=
_check_null(_T1);_T3=_T2->v;{struct Cyc_List_List*fs=(struct Cyc_List_List*)_T3;
_TL46: if(fs!=0)goto _TL44;else{goto _TL45;}
_TL44: _T4=fs;_T5=_T4->hd;_T6=(struct Cyc_Absyn_Datatypefield*)_T5;_T7=df;if(_T6!=_T7)goto _TL47;_T8=i;return _T8;_TL47: _T9=df;_TA=_T9->name;_TB=fs;_TC=_TB->hd;_TD=(struct Cyc_Absyn_Datatypefield*)_TC;_TE=_TD->name;_TF=
Cyc_Absyn_qvar_cmp(_TA,_TE);if(_TF!=0)goto _TL49;_T10=i;return _T10;_TL49: _T11=fs;
# 534
fs=_T11->tl;i=i + 1;goto _TL46;_TL45: _T13=Cyc_Warn_impos;{
# 538
int(*_T16)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T13;_T12=_T16;}_T14=_tag_fat("vcgen: datatype_constructor_index",sizeof(char),34U);_T15=_tag_fat(0U,sizeof(void*),0);_T12(_T14,_T15);}}struct _tuple20{int f0;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};
# 541
static struct _tuple20 Cyc_Vcgen_get_aggr_info(void*t){int*_T0;unsigned _T1;struct Cyc_Absyn_Vardecl*_T2;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3;void*_T4;int*_T5;int _T6;void*_T7;struct _tuple20 _T8;struct Cyc_Absyn_Aggrdecl*_T9;struct Cyc_Absyn_AggrdeclImpl*_TA;struct Cyc_Absyn_AggrdeclImpl*_TB;struct Cyc_Absyn_Aggrdecl*_TC;struct Cyc_Absyn_Aggrdecl*_TD;struct Cyc_Absyn_AggrdeclImpl*_TE;struct _tuple20 _TF;int(*_T10)(struct _fat_ptr,struct _fat_ptr);void*(*_T11)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T12;struct _fat_ptr _T13;
# 543
loop: {
void*_T14=Cyc_Absyn_compress(t);struct Cyc_List_List*_T15;int _T16;enum Cyc_Absyn_AggrKind _T17;union Cyc_Absyn_AggrInfo _T18;struct Cyc_Absyn_Vardecl*_T19;_T0=(int*)_T14;_T1=*_T0;switch(_T1){case 12:{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T1A=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T14;_T19=_T1A->f1;}{struct Cyc_Absyn_Vardecl*vd=_T19;_T2=vd;
t=_T2->type;goto loop;}case 0: _T3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T14;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=24)goto _TL4C;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T14;_T7=_T1A->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T1B=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T7;_T18=_T1B->f1;}}{union Cyc_Absyn_AggrInfo info=_T18;
# 547
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);{struct _tuple20 _T1A;_T9=ad;_TA=_T9->impl;_TB=
_check_null(_TA);_T1A.f0=_TB->tagged;_TC=ad;_T1A.f1=_TC->kind;_TD=ad;_TE=_TD->impl;_T1A.f2=_TE->fields;_T8=_T1A;}return _T8;}_TL4C: goto _LL7;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T1A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T14;_T17=_T1A->f1;_T16=_T1A->f2;_T15=_T1A->f3;}{enum Cyc_Absyn_AggrKind ak=_T17;int is_tuple=_T16;struct Cyc_List_List*fs=_T15;{struct _tuple20 _T1A;
# 550
_T1A.f0=0;_T1A.f1=ak;_T1A.f2=fs;_TF=_T1A;}return _TF;}default: _LL7: _T11=Cyc_Warn_impos;{
int(*_T1A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T11;_T10=_T1A;}_T12=_tag_fat("vcgen: bad type in get_aggr_info",sizeof(char),33U);_T13=_tag_fat(0U,sizeof(void*),0);_T10(_T12,_T13);};}}struct _tuple21{int f0;void*f1;};
# 555
static int Cyc_Vcgen_cmp_index(struct _tuple21*x,struct _tuple21*y){struct _tuple21*_T0;int _T1;struct _tuple21*_T2;int _T3;int _T4;_T0=x;_T1=_T0->f0;_T2=y;_T3=_T2->f0;_T4=_T1 - _T3;
return _T4;}
# 559
struct Cyc_Vcgen_ExpChecks*Cyc_Vcgen_exp2ctxt_checks(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){struct Cyc_Vcgen_ExpChecks**(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table**_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Vcgen_ExpChecks*_T5;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T6;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T7;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T8;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T9;struct Cyc_Vcgen_ExpChecks*_TA;struct Cyc_Vcgen_ExpChecks**_TB;struct Cyc_Vcgen_ExpChecks*_TC;_T1=Cyc_Hashtable_lookup_opt;{
struct Cyc_Vcgen_ExpChecks**(*_TD)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*)=(struct Cyc_Vcgen_ExpChecks**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))_T1;_T0=_TD;}_T2=_check_null(assn_info);_T3=*_T2;_T4=e;{struct Cyc_Vcgen_ExpChecks**result=_T0(_T3,_T4);
if(result!=0)goto _TL4E;{
static struct Cyc_Vcgen_ExpChecks*b=0;
if(b!=0)goto _TL50;{struct Cyc_Vcgen_ExpChecks*_TD=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));
_TD->ctxt=Cyc_AssnDef_true_assnmap();_T6=& Cyc_AssnDef_false_assn;_T7=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T6;
_TD->bounds_check=(void*)_T7;_T8=& Cyc_AssnDef_false_assn;_T9=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T8;
_TD->null_check=(void*)_T9;_T5=(struct Cyc_Vcgen_ExpChecks*)_TD;}
# 564
b=_T5;goto _TL51;_TL50: _TL51: _TA=b;
# 570
return _TA;}
# 572
_TL4E: _TB=result;_TC=*_TB;return _TC;}}
# 576
static void Cyc_Vcgen_insert_exp_checks(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct Cyc_Vcgen_ExpChecks*exp_check){void(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*);void(*_T1)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table**_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Vcgen_ExpChecks*_T5;_T1=Cyc_Hashtable_insert;{
void(*_T6)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*))_T1;_T0=_T6;}_T2=_check_null(assn_info);_T3=*_T2;_T4=e;_T5=exp_check;_T0(_T3,_T4,_T5);
return;}
# 581
static struct Cyc_Absyn_Exp*Cyc_Vcgen_zero_exp (void){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp*_T1;
struct Cyc_Absyn_Exp*ans=Cyc_Absyn_uint_exp(0U,0U);_T0=ans;
_T0->topt=Cyc_Absyn_uint_type;_T1=ans;
return _T1;}
# 587
static struct _tuple0 Cyc_Vcgen_okderef(void*root,void*i){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;struct _tuple0 _T5;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T6;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T7;void*_T8;void*_T9;void*_TA;void*_TB;void*_TC;struct _tuple0 _TD;void*_TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;struct Cyc_Absyn_Vardecl*_T17;void*_T18;int*_T19;int _T1A;struct Cyc_Absyn_ArrayInfo _T1B;struct Cyc_Absyn_Exp*_T1C;struct Cyc_Vcgen_Env*_T1D;struct Cyc_Absyn_ArrayInfo _T1E;struct Cyc_Absyn_Exp*_T1F;struct Cyc_AssnDef_AssnMap _T20;struct _tuple0 _T21;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T22;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T23;struct _tuple0 _T24;void*_T25;void*_T26;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T27;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T28;int*_T29;int _T2A;void*_T2B;struct Cyc_Absyn_PtrInfo _T2C;struct Cyc_Absyn_PtrAtts _T2D;void*_T2E;void*_T2F;int _T30;void*_T31;void*_T32;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T33;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T34;struct _tuple0 _T35;void*_T36;void*_T37;void*_T38;void*_T39;void*_T3A;void*_T3B;struct _tuple0 _T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;{struct Cyc_Absyn_Vardecl*_T41;void*_T42;void*_T43;_T0=root;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 12: _T3=root;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T44=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T3;_T4=_T44->f3;_T43=(void*)_T4;}{void*size=_T43;{struct _tuple0 _T44;
# 589
_T44.f0=Cyc_AssnDef_ult(i,size);_T6=& Cyc_AssnDef_true_assn;_T7=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T6;_T44.f1=(void*)_T7;_T5=_T44;}return _T5;}case 14: _T8=root;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T44=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T8;_T9=_T44->f1;_T43=(void*)_T9;_TA=_T44->f2;_T42=(void*)_TA;}{void*r=_T43;void*i0=_T42;_TB=r;_TC=
# 591
Cyc_AssnDef_plus(i,i0,0);{struct _tuple0 _T44=Cyc_Vcgen_okderef(_TB,_TC);void*_T45;void*_T46;_T46=_T44.f0;_T45=_T44.f1;{void*bndck=_T46;void*nullck=_T45;{struct _tuple0 _T47;_TE=i;_TF=
Cyc_AssnDef_numelts_term(root);_T10=Cyc_AssnDef_ult(_TE,_TF);_T11=bndck;_T47.f0=Cyc_AssnDef_or(_T10,_T11);_T12=root;_T13=
# 594
Cyc_AssnDef_zero();_T14=Cyc_AssnDef_neq(_T12,_T13);_T15=nullck;_T47.f1=Cyc_AssnDef_or(_T14,_T15);_TD=_T47;}
# 592
return _TD;}}}case 11: _T16=root;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T44=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T16;_T41=_T44->f1;}{struct Cyc_Absyn_Vardecl*vd=_T41;_T17=vd;_T18=_T17->type;{
# 596
void*_T44=Cyc_Absyn_compress(_T18);struct Cyc_Absyn_ArrayInfo _T45;_T19=(int*)_T44;_T1A=*_T19;if(_T1A!=5)goto _TL53;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T46=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T44;_T45=_T46->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T45;_T1B=ai;_T1C=_T1B.num_elts;
# 598
if(_T1C==0)goto _TL55;_T1D=
Cyc_Vcgen_bogus_env();_T1E=ai;_T1F=_T1E.num_elts;_T20=Cyc_AssnDef_true_assnmap();{struct _tuple14 _T46=Cyc_Vcgen_vcgen_rexp(_T1D,_T1F,_T20);void*_T47;_T47=_T46.f0;{void*size=_T47;{struct _tuple0 _T48;
_T48.f0=Cyc_AssnDef_ult(i,size);_T22=& Cyc_AssnDef_true_assn;_T23=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T22;_T48.f1=(void*)_T23;_T21=_T48;}return _T21;}}_TL55: goto _LLC;}_TL53: goto _LLC;_LLC:;}{struct _tuple0 _T44;_T25=i;_T26=
# 606
Cyc_AssnDef_numelts_term(root);_T44.f0=Cyc_AssnDef_ult(_T25,_T26);_T27=& Cyc_AssnDef_true_assn;_T28=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T27;_T44.f1=(void*)_T28;_T24=_T44;}return _T24;}default:  {
# 608
void*tp=Cyc_AssnDef_get_term_type(root);
if(tp==0)goto _TL57;{
void*_T44=Cyc_Absyn_compress(tp);struct Cyc_Absyn_PtrInfo _T45;_T29=(int*)_T44;_T2A=*_T29;if(_T2A!=4)goto _TL59;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T46=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T44;_T45=_T46->f1;}{struct Cyc_Absyn_PtrInfo p1=_T45;_T2B=
# 612
Cyc_Absyn_bounds_one();_T2C=p1;_T2D=_T2C.ptr_atts;_T2E=_T2D.bounds;{struct Cyc_Absyn_Exp*b1=Cyc_Tcutil_get_bounds_exp_constrain(_T2B,_T2E,1);
int b2=Cyc_Tcutil_is_nullable_pointer_type(tp,0);_T30=b2;
if(!_T30)goto _TL5B;_T31=root;_T32=Cyc_AssnDef_zero();_T2F=Cyc_AssnDef_neq(_T31,_T32);goto _TL5C;_TL5B: _T33=& Cyc_AssnDef_true_assn;_T34=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T33;_T2F=(void*)_T34;_TL5C: {void*nullck=_T2F;
if(b1==0)goto _TL5D;{struct _tuple0 _T46;_T36=i;_T37=
# 622
Cyc_AssnDef_cnst(b1);_T38=Cyc_AssnDef_ult(_T36,_T37);_T39=i;_T3A=
Cyc_AssnDef_numelts_term(root);_T3B=Cyc_AssnDef_ult(_T39,_T3A);
# 622
_T46.f0=Cyc_AssnDef_or(_T38,_T3B);
_T46.f1=nullck;_T35=_T46;}
# 622
return _T35;_TL5D: goto _LL14;}}}_TL59: goto _LL14;_LL14:;}goto _TL58;_TL57: _TL58: goto _LL0;}}_LL0:;}{struct _tuple0 _T41;_T3D=i;_T3E=
# 631
Cyc_AssnDef_numelts_term(root);_T41.f0=Cyc_AssnDef_ult(_T3D,_T3E);_T3F=root;_T40=Cyc_AssnDef_zero();_T41.f1=Cyc_AssnDef_neq(_T3F,_T40);_T3C=_T41;}return _T3C;}struct _tuple22{void*f0;void*f1;struct Cyc_AssnDef_AssnMap f2;};
# 635
static struct _tuple22 Cyc_Vcgen_vcgen_deref(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*root,struct Cyc_Absyn_Exp*index,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Vcgen_Env*_T0;int _T1;struct Cyc_Absyn_Exp*_T2;void*_T3;void*_T4;struct Cyc_Vcgen_Env*_T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_AssnDef_AssnMap _T7;struct _tuple14 _T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_AssnDef_AssnMap _TA;void*_TB;void*_TC;void*_TD;struct Cyc_AssnDef_AssnMap _TE;void*_TF;void*_T10;struct Cyc_Vcgen_ExpChecks*_T11;struct Cyc_Vcgen_ExpChecks*_T12;struct Cyc_Vcgen_ExpChecks*_T13;struct Cyc_Vcgen_Env*_T14;struct Cyc_Hashtable_Table**_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Vcgen_ExpChecks*_T17;struct Cyc_AssnDef_AssnMap _T18;void*_T19;struct _tuple22 _T1A;
# 637
struct _tuple14 _T1B=Cyc_Vcgen_vcgen_rexp(env,root,amin);struct Cyc_AssnDef_AssnMap _T1C;void*_T1D;_T1D=_T1B.f0;_T1C=_T1B.f1;{void*t1=_T1D;struct Cyc_AssnDef_AssnMap am1=_T1C;
struct _tuple14 _T1E=Cyc_Vcgen_vcgen_rexp(env,index,am1);struct Cyc_AssnDef_AssnMap _T1F;void*_T20;_T20=_T1E.f0;_T1F=_T1E.f1;{void*t2=_T20;struct Cyc_AssnDef_AssnMap am2=_T1F;_T0=env;_T1=_T0->pure_exp;
if(_T1)goto _TL5F;else{goto _TL61;}
# 642
_TL61:{struct _tuple0 _T21=Cyc_Vcgen_okderef(t1,t2);void*_T22;void*_T23;_T23=_T21.f0;_T22=_T21.f1;{void*bnds_check=_T23;void*null_check=_T22;_T2=root;_T3=_T2->topt;_T4=
# 648
_check_null(_T3);{struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(_T4);
void*term_numelts;
void*bnds_check_sufficient;
if(eopt!=0)goto _TL62;
# 653
term_numelts=Cyc_AssnDef_numelts_term(t1);
bnds_check_sufficient=Cyc_AssnDef_ult(t2,term_numelts);goto _TL63;
# 656
_TL62: _T5=env;_T6=eopt;_T7=Cyc_AssnDef_true_assnmap();_T8=Cyc_Vcgen_vcgen_rexp(_T5,_T6,_T7);term_numelts=_T8.f0;
bnds_check_sufficient=Cyc_AssnDef_ult(t2,term_numelts);_TL63: _T9=env;_TA=am2;_TB=
# 659
Cyc_AssnDef_not(bnds_check_sufficient);_TC=Cyc_AssnDef_not(null_check);_TD=Cyc_AssnDef_or(_TB,_TC);_TE=Cyc_AssnDef_and_assnmap_assn(_TA,_TD);_TF=Cyc_Absyn_exn_type();_T10=Cyc_AssnDef_fresh_var(0,_TF);Cyc_Vcgen_update_try_assnmap(_T9,_TE,_T10);{
# 661
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));_T11=exp_checks;_T11->ctxt=am2;_T12=exp_checks;_T12->bounds_check=bnds_check;_T13=exp_checks;
_T13->null_check=null_check;_T14=env;_T15=_T14->assn_info;_T16=root;_T17=exp_checks;
Cyc_Vcgen_insert_exp_checks(_T15,_T16,_T17);_T18=am2;_T19=
# 665
Cyc_AssnDef_and(bnds_check,null_check);am2=Cyc_AssnDef_and_assnmap_assn(_T18,_T19);}}}}goto _TL60;_TL5F: _TL60:{struct _tuple22 _T21;
# 667
_T21.f0=t1;_T21.f1=t2;_T21.f2=am2;_T1A=_T21;}return _T1A;}}}
# 670
static struct _tuple14 Cyc_Vcgen_vcgen_comprehension(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,void*res_type,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Vcgen_Env*_T0;struct Cyc_AssnDef_AssnMap _T1;void*_T2;void*_T3;struct Cyc_AssnDef_AssnMap _T4;void*_T5;struct Cyc_AssnDef_AssnMap _T6;void*_T7;struct Cyc_Vcgen_Env*_T8;struct Cyc_AssnDef_AssnMap _T9;void*_TA;void*_TB;void*_TC;void*_TD;void*_TE;int _TF;struct Cyc_AssnDef_AssnMap _T10;struct Cyc_AssnDef_AssnMap _T11;void*_T12;void*_T13;void*_T14;struct _tuple14 _T15;
# 674
struct _tuple14 _T16=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T17;void*_T18;_T18=_T16.f0;_T17=_T16.f1;{void*t1=_T18;struct Cyc_AssnDef_AssnMap amin=_T17;
# 676
void*v=Cyc_AssnDef_zero();_T0=env;_T1=amin;_T2=
Cyc_Vcgen_myaddr(env,vd);_T3=v;amin=Cyc_Vcgen_do_assign(_T0,_T1,_T2,_T3);
_TL64: if(1)goto _TL65;else{goto _TL66;}
_TL65: amin=Cyc_AssnDef_canonical_assnmap(amin);{
# 681
struct _tuple14 _T19=Cyc_AssnDef_lookup_var_map(vd,amin);struct Cyc_AssnDef_AssnMap _T1A;void*_T1B;_T1B=_T19.f0;_T1A=_T19.f1;{void*v2=_T1B;struct Cyc_AssnDef_AssnMap am2=_T1A;
v=v2;amin=am2;_T4=amin;_T5=
Cyc_AssnDef_ult(v,t1);{struct Cyc_AssnDef_AssnMap amt=Cyc_AssnDef_and_assnmap_assn(_T4,_T5);_T6=amin;_T7=
Cyc_AssnDef_ulte(t1,v);{struct Cyc_AssnDef_AssnMap amf=Cyc_AssnDef_and_assnmap_assn(_T6,_T7);
struct _tuple14 _T1C=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _T1D;_T1D=_T1C.f1;{struct Cyc_AssnDef_AssnMap ambody=_T1D;
# 687
struct _tuple14 _T1E=Cyc_AssnDef_lookup_var_map(vd,ambody);struct Cyc_AssnDef_AssnMap _T1F;void*_T20;_T20=_T1E.f0;_T1F=_T1E.f1;{void*v2=_T20;struct Cyc_AssnDef_AssnMap am2=_T1F;_T8=env;_T9=am2;_TA=
Cyc_Vcgen_myaddr(env,vd);_TB=v2;_TC=Cyc_AssnDef_one();_TD=Cyc_Absyn_uint_type;_TE=Cyc_AssnDef_plus(_TB,_TC,_TD);ambody=Cyc_Vcgen_do_assign(_T8,_T9,_TA,_TE);
ambody=Cyc_AssnDef_canonical_assnmap(ambody);{
# 691
struct _tuple15 _T21=Cyc_AssnDef_unify_var_maps_subst(amin,ambody);struct Cyc_Dict_Dict _T22;void*_T23;void*_T24;_T24=_T21.f0;_T23=_T21.f1;_T22=_T21.f2;{void*a1=_T24;void*a2=_T23;struct Cyc_Dict_Dict map=_T22;_TF=
# 694
Cyc_Vcgen_implies(a2,a1);if(!_TF)goto _TL67;
# 696
amin=amf;goto _TL66;_TL67:{struct Cyc_AssnDef_AssnMap _T25;
# 699
_T25.assn=Cyc_AssnDef_or(a1,a2);_T25.map=map;_T10=_T25;}amin=Cyc_AssnDef_widen_assnmap(_T10);}}}}}}}}goto _TL64;_TL66: {
# 702
void*res=Cyc_AssnDef_fresh_var(0,res_type);_T11=amin;_T12=
# 704
Cyc_AssnDef_numelts_term(res);_T13=t1;_T14=Cyc_AssnDef_eq(_T12,_T13);amin=Cyc_AssnDef_and_assnmap_assn(_T11,_T14);{struct _tuple14 _T19;
_T19.f0=res;_T19.f1=amin;_T15=_T19;}return _T15;}}}
# 708
static struct _tuple14 Cyc_Vcgen_vcgen_binop(struct Cyc_Vcgen_Env*env,void*tp_opt,enum Cyc_Absyn_Primop p,void*t1,void*t2,struct Cyc_AssnDef_AssnMap am){int _T0;enum Cyc_Absyn_Primop _T1;int _T2;struct Cyc_Vcgen_Env*_T3;int _T4;int _T5;int _T6;struct Cyc_Vcgen_Env*_T7;struct Cyc_AssnDef_AssnMap _T8;void*_T9;void*_TA;struct _tuple14 _TB;struct _tuple14 _TC;void*_TD;void*_TE;void*_TF;int(*_T10)(struct _fat_ptr,struct _fat_ptr);void*(*_T11)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_Vcgen_Env*_T14;int _T15;enum Cyc_Absyn_Primop _T16;int _T17;int _T18;struct Cyc_AssnDef_AssnMap _T19;void*_T1A;struct _tuple14 _T1B;
# 711
void*tp=_check_null(tp_opt);_T0=
Cyc_Tcutil_is_pointer_type(tp);if(!_T0)goto _TL69;_T1=p;_T2=(int)_T1;switch(_T2){case Cyc_Absyn_Plus: _T3=env;_T4=_T3->pure_exp;
# 717
if(_T4)goto _TL6C;else{goto _TL6E;}_TL6E: _T5=Cyc_Tcutil_is_zeroterm_pointer_type(tp);if(!_T5)goto _TL6C;_T6=
Cyc_Tcutil_is_fat_pointer_type(tp);
# 717
if(_T6)goto _TL6C;else{goto _TL6F;}
# 719
_TL6F: _T7=env;_T8=am;_T9=Cyc_Absyn_exn_type();_TA=Cyc_AssnDef_fresh_var(0,_T9);Cyc_Vcgen_update_try_assnmap(_T7,_T8,_TA);goto _TL6D;_TL6C: _TL6D:{struct _tuple14 _T1C;
_T1C.f0=Cyc_AssnDef_offseti(t1,t2,tp);_T1C.f1=am;_TB=_T1C;}return _TB;case Cyc_Absyn_Minus:{struct _tuple14 _T1C;_TD=t1;_TE=
# 722
Cyc_AssnDef_unop(2U,t2,Cyc_Absyn_sint_type);_TF=tp;_T1C.f0=Cyc_AssnDef_offseti(_TD,_TE,_TF);_T1C.f1=am;_TC=_T1C;}return _TC;default: _T11=Cyc_Warn_impos;{
# 724
int(*_T1C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T11;_T10=_T1C;}_T12=_tag_fat("vcgen: result has pointer type but uses bad primop",sizeof(char),51U);_T13=_tag_fat(0U,sizeof(void*),0);_T10(_T12,_T13);}goto _TL6A;_TL69: _TL6A: {
# 728
void*t=Cyc_AssnDef_binop(p,t1,t2,tp);_T14=env;_T15=_T14->pure_exp;
# 731
if(_T15)goto _TL70;else{goto _TL72;}_TL72: _T16=p;_T17=(int)_T16;if(_T17!=21)goto _TL70;_T18=Cyc_Unify_unify(Cyc_Absyn_uint_type,tp);if(!_T18)goto _TL70;_T19=am;_T1A=
Cyc_AssnDef_ult(t,t2);am=Cyc_AssnDef_and_assnmap_assn(_T19,_T1A);goto _TL71;_TL70: _TL71:{struct _tuple14 _T1C;
_T1C.f0=t;_T1C.f1=am;_T1B=_T1C;}return _T1B;}}
# 736
static int Cyc_Vcgen_visit_pure_stmt(int*env,struct Cyc_Absyn_Stmt*s){int*_T0;unsigned _T1;struct Cyc_Absyn_Stmt*_T2;int*_T3;unsigned _T4;int*_T5;_T0=env;_T1=(unsigned)_T0;
if(_T1)goto _TL73;else{goto _TL75;}_TL75: return 0;_TL73: _T2=s;{
void*_T6=_T2->r;_T3=(int*)_T6;_T4=*_T3;switch(_T4){case 3: goto _LL4;case 5: _LL4: goto _LL6;case 6: _LL6: goto _LL8;case 7: _LL8: goto _LLA;case 8: _LLA: goto _LLC;case 9: _LLC: goto _LLE;case 14: _LLE: goto _LL10;case 15: _LL10: _T5=env;
# 746
*_T5=0;return 0;default:
 return 1;};}}
# 751
static int Cyc_Vcgen_visit_pure_exp(int*env,struct Cyc_Absyn_Exp*e){int*_T0;unsigned _T1;struct Cyc_Absyn_Exp*_T2;int*_T3;unsigned _T4;enum Cyc_Absyn_Primop _T5;int _T6;enum Cyc_Absyn_Primop _T7;int _T8;enum Cyc_Absyn_Coercion _T9;int _TA;int _TB;int*_TC;_T0=env;_T1=(unsigned)_T0;
if(_T1)goto _TL77;else{goto _TL79;}_TL79: return 0;_TL77: _T2=e;{
void*_TD=_T2->r;int _TE;enum Cyc_Absyn_Coercion _TF;enum Cyc_Absyn_Primop _T10;_T3=(int*)_TD;_T4=*_T3;switch(_T4){case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T11=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TD;_T10=_T11->f1;}{enum Cyc_Absyn_Primop p=_T10;_T5=p;_T6=(int)_T5;
# 755
if(_T6==3)goto _TL7D;else{goto _TL7E;}_TL7E: _T7=p;_T8=(int)_T7;if(_T8==4)goto _TL7D;else{goto _TL7B;}_TL7D: goto _LL0;_TL7B:
 return 1;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T11=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TD;_TF=_T11->f4;}{enum Cyc_Absyn_Coercion c=_TF;_T9=c;if(_T9!=Cyc_Absyn_No_coercion)goto _TL7F;
# 759
return 1;_TL7F: goto _LL1F;_LL1F: goto _LL0;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T11=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_TD;_TE=_T11->f3;}_TA=(int)_TE;if(!_TA)goto _TL81;{int is_tagged=_TE;goto _LL0;}_TL81: goto _LL1D;case 41:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T11=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TD;_TE=_T11->f2;}_TB=(int)_TE;if(_TB)goto _TL83;else{goto _TL85;}_TL85:{int static_only=_TE;goto _LL0;}_TL83: goto _LL1D;case 4: goto _LLC;case 5: _LLC: goto _LLE;case 10: _LLE: goto _LL10;case 11: _LL10: goto _LL12;case 20: _LL12: goto _LL14;case 22: _LL14: goto _LL16;case 23: _LL16: goto _LL18;case 34: _LL18: goto _LL1A;case 39: _LL1A: goto _LL1C;case 40: _LL1C: goto _LL0;default: _LL1D:
# 775
 return 1;}_LL0:;}_TC=env;
# 777
*_TC=0;
return 0;}
# 781
static int Cyc_Vcgen_is_pure_exp(struct Cyc_Absyn_Exp*e){void(*_T0)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Exp*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);int*_T2;struct Cyc_Absyn_Exp*_T3;int _T4;
int env=1;_T1=Cyc_Absyn_visit_exp;{
void(*_T5)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Exp*))_T1;_T0=_T5;}_T2=& env;_T3=e;_T0(Cyc_Vcgen_visit_pure_exp,Cyc_Vcgen_visit_pure_stmt,_T2,_T3);_T4=env;
return _T4;}
# 796 "vcgen.cyc"
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_loop(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e1infor,struct Cyc_Absyn_Stmt*s1indo,struct Cyc_Absyn_Exp*cond,struct Cyc_Absyn_Stmt*dummy,struct Cyc_Absyn_Stmt*body,struct Cyc_Absyn_Exp*e3infor,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Vcgen_Env*_T0;struct Cyc_Vcgen_Env*_T1;struct Cyc_AssnDef_AssnMap*_T2;struct Cyc_Absyn_Exp*_T3;int*_T4;int _T5;struct Cyc_Absyn_Exp*_T6;int*_T7;int _T8;int _T9;struct Cyc_Vcgen_Env*_TA;struct Cyc_Vcgen_Env*_TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_AssnDef_AssnMap _TE;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_TF;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T10;struct Cyc_AssnDef_AssnMap _T11;struct _tuple18 _T12;struct Cyc_Vcgen_Env*_T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T15;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T16;struct Cyc_AssnDef_AssnMap _T17;struct Cyc_AssnDef_AssnMap _T18;void*_T19;struct Cyc_AssnDef_AssnMap _T1A;void*_T1B;int _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc_AssnDef_AssnMap _T1E;void*_T1F;struct Cyc_String_pa_PrintArg_struct _T20;struct Cyc_Absyn_Exp*_T21;unsigned _T22;struct _fat_ptr _T23;struct _fat_ptr _T24;struct Cyc_Vcgen_Env*_T25;struct Cyc_AssnDef_AssnMap _T26;struct Cyc_Set_Set*_T27;struct Cyc_Set_Set*_T28;struct Cyc_Set_Set*_T29;struct Cyc_Set_Set*_T2A;struct _tuple18 _T2B;struct Cyc_AssnDef_AssnMap _T2C;struct Cyc_Vcgen_Env*_T2D;struct Cyc_AssnDef_AssnMap*_T2E;struct Cyc_AssnDef_AssnMap*_T2F;struct Cyc_AssnDef_AssnMap _T30;struct Cyc_Vcgen_Env*_T31;struct Cyc_Vcgen_Env*_T32;struct Cyc_Vcgen_Env*_T33;struct Cyc_Absyn_Exp*_T34;struct Cyc_AssnDef_AssnMap _T35;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T36;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T37;struct Cyc_AssnDef_AssnMap _T38;struct _tuple18 _T39;struct Cyc_Vcgen_Env*_T3A;struct Cyc_AssnDef_AssnMap _T3B;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T3C;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T3D;struct Cyc_AssnDef_AssnMap _T3E;struct Cyc_AssnDef_AssnMap _T3F;void*_T40;struct Cyc_AssnDef_AssnMap _T41;void*_T42;int _T43;struct Cyc_String_pa_PrintArg_struct _T44;struct Cyc_AssnDef_AssnMap _T45;void*_T46;struct Cyc_String_pa_PrintArg_struct _T47;struct Cyc_Absyn_Exp*_T48;unsigned _T49;struct _fat_ptr _T4A;struct _fat_ptr _T4B;struct Cyc_Vcgen_Env*_T4C;struct Cyc_AssnDef_AssnMap _T4D;struct Cyc_AssnDef_AssnMap _T4E;struct Cyc_AssnDef_AssnMap _T4F;struct Cyc_AssnDef_AssnMap _T50;int _T51;struct Cyc_Vcgen_Env*_T52;struct Cyc_AssnDef_AssnMap _T53;_T0=env;{
# 800
struct Cyc_AssnDef_AssnMap*old_continue_assnmap=_T0->continue_assnmap;_T1=env;{struct Cyc_AssnDef_AssnMap*_T54=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));
*_T54=Cyc_AssnDef_false_assnmap();_T2=(struct Cyc_AssnDef_AssnMap*)_T54;}_T1->continue_assnmap=_T2;
# 803
if(e1infor==0)goto _TL86;{
struct _tuple14 _T54=Cyc_Vcgen_vcgen_rexp(env,e1infor,amin);struct Cyc_AssnDef_AssnMap _T55;_T55=_T54.f1;{struct Cyc_AssnDef_AssnMap am=_T55;
amin=am;}}goto _TL87;_TL86: _TL87:
# 807
 if(s1indo==0)goto _TL88;
amin=Cyc_Vcgen_vcgen_stmt(env,s1indo,amin);goto _TL89;_TL88: _TL89: {
# 814
struct Cyc_Absyn_Exp*loop_inv_exp=0;
struct Cyc_AssnDef_AssnMap loop_inv_assnmap;_T3=cond;{
void*_T54=_T3->r;struct Cyc_Absyn_Exp*_T55;struct Cyc_Absyn_Exp*_T56;_T4=(int*)_T54;_T5=*_T4;if(_T5!=7)goto _TL8A;{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T57=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T54;_T56=_T57->f1;_T55=_T57->f2;}{struct Cyc_Absyn_Exp*eleft=_T56;struct Cyc_Absyn_Exp*eright=_T55;_T6=eleft;{
# 818
void*_T57=_T6->r;int _T58;struct Cyc_Absyn_Exp*_T59;_T7=(int*)_T57;_T8=*_T7;if(_T8!=41)goto _TL8C;{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T5A=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T57;_T59=_T5A->f1;_T58=_T5A->f2;}_T9=(int)_T58;if(!_T9)goto _TL8E;{struct Cyc_Absyn_Exp*eassn=_T59;int static_only=_T58;
# 820
loop_inv_exp=eassn;
cond=eright;goto _LL8;}_TL8E: goto _LLB;_TL8C: _LLB: goto _LL8;_LL8:;}goto _LL3;}_TL8A: goto _LL3;_LL3:;}
# 836
if(loop_inv_exp==0)goto _TL90;_TA=env;{
int old_widen_paths=_TA->widen_paths;_TB=env;
_TB->widen_paths=0;_TC=env;_TD=loop_inv_exp;{struct Cyc_AssnDef_AssnMap _T54;_TF=& Cyc_AssnDef_true_assn;_T10=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_TF;
_T54.assn=(void*)_T10;_T11=amin;_T54.map=_T11.map;_TE=_T54;}_T12=Cyc_Vcgen_vcgen_test(_TC,_TD,_TE);loop_inv_assnmap=_T12.f0;_T13=env;
_T13->widen_paths=old_widen_paths;}goto _TL91;
# 843
_TL90:{struct Cyc_AssnDef_AssnMap _T54;_T15=& Cyc_AssnDef_true_assn;_T16=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T15;_T54.assn=(void*)_T16;_T17=amin;_T54.map=_T17.map;_T14=_T54;}loop_inv_assnmap=_T14;_TL91: _T18=amin;_T19=_T18.assn;_T1A=loop_inv_assnmap;_T1B=_T1A.assn;_T1C=
# 846
Cyc_Vcgen_implies(_T19,_T1B);if(_T1C)goto _TL92;else{goto _TL94;}
_TL94:{struct Cyc_String_pa_PrintArg_struct _T54;_T54.tag=0;_T1E=loop_inv_assnmap;_T1F=_T1E.assn;
_T54.f1=Cyc_AssnDef_assn2string(_T1F);_T1D=_T54;}{struct Cyc_String_pa_PrintArg_struct _T54=_T1D;{struct Cyc_String_pa_PrintArg_struct _T55;_T55.tag=0;_T55.f1=Cyc_AssnDef_assnmap2string(amin);_T20=_T55;}{struct Cyc_String_pa_PrintArg_struct _T55=_T20;void*_T56[2];_T56[0]=& _T54;_T56[1]=& _T55;_T21=cond;_T22=_T21->loc;_T23=
# 847
_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U);_T24=_tag_fat(_T56,sizeof(void*),2);Cyc_Warn_err(_T22,_T23,_T24);}}_T25=env;
# 849
_T25->continue_assnmap=old_continue_assnmap;_T26=
Cyc_AssnDef_true_assnmap();return _T26;_TL92: {
# 857
struct Cyc_Set_Set*changed=Cyc_AssnDef_calc_changed_vars_exp(cond);_T27=changed;_T28=
Cyc_AssnDef_calc_changed_vars_stmt(body);changed=Cyc_Set_union_two(_T27,_T28);
if(e3infor==0)goto _TL95;_T29=changed;_T2A=
Cyc_AssnDef_calc_changed_vars_exp(e3infor);changed=Cyc_Set_union_two(_T29,_T2A);goto _TL96;_TL95: _TL96:
# 864
 amin=Cyc_AssnDef_force_canonical(changed,amin);
# 877 "vcgen.cyc"
_TL97: if(1)goto _TL98;else{goto _TL99;}
# 881
_TL98: amin=Cyc_AssnDef_canonical_assnmap(amin);{
# 886
struct Cyc_AssnDef_AssnMap amold=Cyc_Vcgen_lookup_stmt_assnmap(env,dummy);
# 890
amin=Cyc_AssnDef_or_assnmap_assnmap(amold,amin);
# 894
amin=Cyc_AssnDef_widen_assnmap(amin);
# 898
Cyc_Vcgen_replace_stmt_assnmap(env,dummy,amin);
# 900
if(loop_inv_exp==0)goto _TL9A;_T2B=
Cyc_Vcgen_vcgen_test(env,loop_inv_exp,amin);amin=_T2B.f0;goto _TL9B;_TL9A: _TL9B: {
# 904
struct _tuple18 _T54=Cyc_Vcgen_vcgen_test(env,cond,amin);struct Cyc_AssnDef_AssnMap _T55;struct Cyc_AssnDef_AssnMap _T56;_T56=_T54.f0;_T55=_T54.f1;{struct Cyc_AssnDef_AssnMap amt=_T56;struct Cyc_AssnDef_AssnMap amf=_T55;
amin=Cyc_Vcgen_vcgen_stmt(env,body,amt);_T2C=amin;_T2D=env;_T2E=_T2D->continue_assnmap;_T2F=
# 910
_check_null(_T2E);_T30=*_T2F;amin=Cyc_AssnDef_or_assnmap_assnmap(_T2C,_T30);
# 916
if(e3infor==0)goto _TL9C;{
struct _tuple14 _T57=Cyc_Vcgen_vcgen_rexp(env,e3infor,amin);struct Cyc_AssnDef_AssnMap _T58;_T58=_T57.f1;{struct Cyc_AssnDef_AssnMap am=_T58;
amin=am;}}goto _TL9D;_TL9C: _TL9D:
# 923
 if(loop_inv_exp==0)goto _TL9E;_T31=env;{
int old_widen_paths=_T31->widen_paths;_T32=env;
_T32->widen_paths=0;_T33=env;_T34=loop_inv_exp;{struct Cyc_AssnDef_AssnMap _T57;_T36=& Cyc_AssnDef_true_assn;_T37=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T36;
_T57.assn=(void*)_T37;_T38=amin;_T57.map=_T38.map;_T35=_T57;}_T39=Cyc_Vcgen_vcgen_test(_T33,_T34,_T35);loop_inv_assnmap=_T39.f0;_T3A=env;
_T3A->widen_paths=old_widen_paths;}goto _TL9F;
# 930
_TL9E:{struct Cyc_AssnDef_AssnMap _T57;_T3C=& Cyc_AssnDef_true_assn;_T3D=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T3C;_T57.assn=(void*)_T3D;_T3E=amin;_T57.map=_T3E.map;_T3B=_T57;}loop_inv_assnmap=_T3B;_TL9F: _T3F=amin;_T40=_T3F.assn;_T41=loop_inv_assnmap;_T42=_T41.assn;_T43=
Cyc_Vcgen_implies(_T40,_T42);if(_T43)goto _TLA0;else{goto _TLA2;}
_TLA2:{struct Cyc_String_pa_PrintArg_struct _T57;_T57.tag=0;_T45=loop_inv_assnmap;_T46=_T45.assn;
_T57.f1=Cyc_AssnDef_assn2string(_T46);_T44=_T57;}{struct Cyc_String_pa_PrintArg_struct _T57=_T44;{struct Cyc_String_pa_PrintArg_struct _T58;_T58.tag=0;_T58.f1=Cyc_AssnDef_assnmap2string(amin);_T47=_T58;}{struct Cyc_String_pa_PrintArg_struct _T58=_T47;void*_T59[2];_T59[0]=& _T57;_T59[1]=& _T58;_T48=cond;_T49=_T48->loc;_T4A=
# 932
_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U);_T4B=_tag_fat(_T59,sizeof(void*),2);Cyc_Warn_err(_T49,_T4A,_T4B);}}_T4C=env;
# 934
_T4C->continue_assnmap=old_continue_assnmap;_T4D=
Cyc_AssnDef_true_assnmap();return _T4D;_TLA0: _T4E=
# 938
Cyc_Vcgen_lookup_stmt_assnmap(env,dummy);_T4F=amin;{struct _tuple15 _T57=Cyc_AssnDef_unify_var_maps_subst(_T4E,_T4F);struct Cyc_Dict_Dict _T58;void*_T59;void*_T5A;_T5A=_T57.f0;_T59=_T57.f1;_T58=_T57.f2;{void*a1=_T5A;void*a2=_T59;struct Cyc_Dict_Dict map=_T58;{struct Cyc_AssnDef_AssnMap _T5B;
_T5B.assn=a2;_T5B.map=map;_T50=_T5B;}amin=_T50;_T51=
# 944
Cyc_Vcgen_implies(a2,a1);if(!_T51)goto _TLA3;_T52=env;
_T52->continue_assnmap=old_continue_assnmap;_T53=amf;
return _T53;_TLA3:;}}}}}goto _TL97;_TL99:;}}}}struct _tuple23{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 951
static struct _tuple14 Cyc_Vcgen_vcgen_rexp_inner(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;int _T3;struct _tuple14 _T4;struct _tuple14 _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;int _T8;struct Cyc_Absyn_Exp*_T9;unsigned _TA;struct Cyc_String_pa_PrintArg_struct _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc___cycFILE*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;int _T12;struct Cyc_Absyn_Exp*_T13;unsigned _T14;struct Cyc_String_pa_PrintArg_struct _T15;struct Cyc_String_pa_PrintArg_struct _T16;struct Cyc___cycFILE*_T17;struct _fat_ptr _T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;struct _fat_ptr _T1B;int _T1C;struct Cyc_Vcgen_Env*_T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;int _T20;struct Cyc_Vcgen_Env*_T21;struct _tuple14 _T22;void*_T23;struct _tuple14 _T24;struct Cyc_Absyn_Vardecl*_T25;int _T26;struct _tuple14 _T27;void*_T28;void*_T29;struct Cyc_Absyn_Vardecl*_T2A;void*_T2B;struct Cyc_Absyn_Vardecl*_T2C;void*_T2D;int*_T2E;int _T2F;struct _tuple14 _T30;struct _tuple14 _T31;struct _tuple14 _T32;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T33;struct Cyc_List_List*_T34;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T35;struct Cyc_List_List*_T36;struct Cyc_List_List*_T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;void*_T3A;struct _tuple14 _T3B;enum Cyc_Absyn_Primop _T3C;void*_T3D;struct Cyc_Absyn_Exp*_T3E;void*_T3F;struct Cyc_List_List*_T40;void*_T41;struct Cyc_List_List*_T42;void*_T43;struct Cyc_Vcgen_Env*_T44;struct Cyc_Absyn_Exp*_T45;void*_T46;enum Cyc_Absyn_Primop _T47;void*_T48;void*_T49;struct Cyc_AssnDef_AssnMap _T4A;struct _tuple14 _T4B;int(*_T4C)(struct _fat_ptr,struct _fat_ptr);void*(*_T4D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T4E;struct _fat_ptr _T4F;struct Cyc_Vcgen_Env*_T50;struct Cyc_Absyn_Exp*_T51;void*_T52;struct Cyc_Core_Opt*_T53;void*_T54;enum Cyc_Absyn_Primop _T55;void*_T56;void*_T57;struct Cyc_AssnDef_AssnMap _T58;struct _tuple14 _T59;struct _tuple14 _T5A;enum Cyc_Absyn_Incrementor _T5B;int _T5C;struct Cyc_Vcgen_Env*_T5D;struct Cyc_Absyn_Exp*_T5E;void*_T5F;void*_T60;void*_T61;struct Cyc_AssnDef_AssnMap _T62;struct Cyc_Vcgen_Env*_T63;struct Cyc_Absyn_Exp*_T64;void*_T65;void*_T66;void*_T67;struct Cyc_AssnDef_AssnMap _T68;struct Cyc_Vcgen_Env*_T69;struct Cyc_Absyn_Exp*_T6A;void*_T6B;void*_T6C;void*_T6D;struct Cyc_AssnDef_AssnMap _T6E;struct Cyc_Vcgen_Env*_T6F;struct Cyc_Absyn_Exp*_T70;void*_T71;void*_T72;void*_T73;struct Cyc_AssnDef_AssnMap _T74;int(*_T75)(struct _fat_ptr,struct _fat_ptr);void*(*_T76)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T77;struct _fat_ptr _T78;struct _tuple14 _T79;struct Cyc_Absyn_Exp*_T7A;void*_T7B;struct Cyc_AssnDef_AssnMap _T7C;void*_T7D;struct Cyc_AssnDef_AssnMap _T7E;void*_T7F;struct _tuple14 _T80;struct Cyc_Vcgen_Env*_T81;struct Cyc_AssnDef_AssnMap _T82;struct Cyc_Absyn_Exp*_T83;void*_T84;struct Cyc_AssnDef_AssnMap _T85;void*_T86;struct Cyc_AssnDef_AssnMap _T87;void*_T88;void*_T89;void*_T8A;struct _tuple14 _T8B;struct Cyc_Vcgen_Env*_T8C;struct Cyc_AssnDef_AssnMap _T8D;struct Cyc_Absyn_Exp*_T8E;void*_T8F;struct Cyc_AssnDef_AssnMap _T90;void*_T91;void*_T92;void*_T93;struct Cyc_AssnDef_AssnMap _T94;void*_T95;struct _tuple14 _T96;struct Cyc_Vcgen_Env*_T97;struct Cyc_AssnDef_AssnMap _T98;struct _tuple14 _T99;struct Cyc_Absyn_Exp*_T9A;void*_T9B;struct Cyc_Vcgen_Env*_T9C;struct Cyc_Hashtable_Table*_T9D;struct Cyc_Absyn_Exp*_T9E;struct Cyc_AssnDef_AssnMap _T9F;void*_TA0;void*_TA1;struct Cyc_List_List*_TA2;struct Cyc_Absyn_Exp*_TA3;void*_TA4;struct Cyc_List_List*_TA5;struct Cyc_List_List*_TA6;struct Cyc_AssnDef_AssnMap _TA7;void*_TA8;void*_TA9;int _TAA;struct Cyc_String_pa_PrintArg_struct _TAB;struct Cyc_String_pa_PrintArg_struct _TAC;struct Cyc_Absyn_Exp*_TAD;unsigned _TAE;struct _fat_ptr _TAF;struct _fat_ptr _TB0;void*_TB1;struct Cyc_AssnDef_ExistAssnFn*_TB2;struct Cyc_List_List*_TB3;struct Cyc_Vcgen_Env*_TB4;struct Cyc_AssnDef_AssnMap _TB5;void*_TB6;struct Cyc_AssnDef_ExistAssnFn*_TB7;struct Cyc_List_List*_TB8;struct Cyc_List_List*_TB9;struct Cyc_AssnDef_ExistAssnFn*_TBA;struct Cyc_AssnDef_AssnFn*_TBB;struct Cyc_List_List*_TBC;int _TBD;int _TBE;int _TBF;struct Cyc_AssnDef_ExistAssnFn*_TC0;struct Cyc_List_List*_TC1;struct Cyc_List_List*_TC2;struct Cyc_Absyn_Exp*_TC3;void*_TC4;void*_TC5;int _TC6;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TC7;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TC8;struct _tuple14 _TC9;struct _tuple14 _TCA;struct _tuple14 _TCB;struct _tuple14 _TCC;void*_TCD;int _TCE;int _TCF;struct _tuple14 _TD0;void*_TD1;struct Cyc_Absyn_Exp*_TD2;void*_TD3;void*_TD4;int _TD5;struct _tuple14 _TD6;struct _tuple0 _TD7;struct Cyc_Absyn_Exp*_TD8;void*_TD9;void*_TDA;void*_TDB;int*_TDC;int _TDD;void*_TDE;enum Cyc_Absyn_Coercion _TDF;int _TE0;struct _tuple14 _TE1;void*_TE2;int*_TE3;int _TE4;void*_TE5;struct Cyc_AssnDef_ExistAssnFn*_TE6;struct Cyc_List_List*_TE7;struct Cyc_AssnDef_AssnMap _TE8;void*_TE9;void*_TEA;int _TEB;struct Cyc_String_pa_PrintArg_struct _TEC;struct Cyc_String_pa_PrintArg_struct _TED;struct Cyc_Absyn_Exp*_TEE;unsigned _TEF;struct _fat_ptr _TF0;struct _fat_ptr _TF1;struct Cyc_AssnDef_AssnMap _TF2;void*_TF3;void*_TF4;struct _tuple14 _TF5;void*_TF6;int*_TF7;int _TF8;void*_TF9;enum Cyc_Absyn_Coercion _TFA;int _TFB;struct _tuple14 _TFC;void*_TFD;int*_TFE;unsigned _TFF;void*_T100;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T101;void*_T102;int*_T103;int _T104;void*_T105;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T106;void*_T107;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T108;enum Cyc_Absyn_Size_of _T109;int _T10A;void*_T10B;int*_T10C;unsigned _T10D;void*_T10E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10F;void*_T110;int*_T111;int _T112;void*_T113;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T114;void*_T115;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T116;enum Cyc_Absyn_Size_of _T117;int _T118;void*_T119;int*_T11A;unsigned _T11B;void*_T11C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11D;void*_T11E;int*_T11F;unsigned _T120;void*_T121;struct _tuple14 _T122;struct Cyc_List_List*_T123;void*_T124;struct Cyc_Absyn_Exp*_T125;unsigned _T126;struct Cyc_Absyn_Exp*_T127;void*_T128;void*_T129;int*_T12A;unsigned _T12B;void*_T12C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T12D;void*_T12E;int*_T12F;unsigned _T130;void*_T131;struct _tuple14 _T132;struct Cyc_List_List*_T133;void*_T134;struct Cyc_Absyn_Exp*_T135;unsigned _T136;struct Cyc_Absyn_Exp*_T137;void*_T138;void*_T139;struct Cyc_Absyn_PtrInfo _T13A;struct Cyc_Absyn_PtrAtts _T13B;void*_T13C;struct Cyc_Absyn_Exp*_T13D;struct _tuple14 _T13E;void*_T13F;int*_T140;int _T141;void*_T142;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T143;void*_T144;int*_T145;int _T146;void*_T147;void*_T148;void*_T149;void*_T14A;enum Cyc_Absyn_Size_of _T14B;enum Cyc_Absyn_Size_of _T14C;int _T14D;enum Cyc_Absyn_Size_of _T14E;enum Cyc_Absyn_Size_of _T14F;int _T150;enum Cyc_Absyn_Size_of _T151;int _T152;enum Cyc_Absyn_Size_of _T153;int _T154;struct _tuple14 _T155;void*_T156;int*_T157;int _T158;void*_T159;void*_T15A;struct Cyc_Absyn_PtrInfo _T15B;struct Cyc_Absyn_PtrAtts _T15C;void*_T15D;struct Cyc_Absyn_PtrInfo _T15E;struct Cyc_Absyn_PtrAtts _T15F;void*_T160;void*_T161;struct Cyc_Absyn_PtrInfo _T162;struct Cyc_Absyn_PtrAtts _T163;void*_T164;void*_T165;struct Cyc_Absyn_PtrInfo _T166;struct Cyc_Absyn_PtrAtts _T167;void*_T168;struct _tuple23 _T169;struct Cyc_Absyn_Exp*_T16A;struct Cyc_Absyn_Exp*_T16B;struct Cyc_Vcgen_Env*_T16C;int _T16D;int _T16E;int _T16F;void*_T170;void*_T171;struct Cyc_Vcgen_ExpChecks*_T172;struct Cyc_Vcgen_ExpChecks*_T173;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T174;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T175;struct Cyc_Vcgen_ExpChecks*_T176;struct Cyc_Vcgen_Env*_T177;struct Cyc_Hashtable_Table**_T178;struct Cyc_Absyn_Exp*_T179;struct Cyc_Vcgen_ExpChecks*_T17A;struct Cyc_Vcgen_Env*_T17B;struct Cyc_AssnDef_AssnMap _T17C;void*_T17D;void*_T17E;void*_T17F;struct Cyc_AssnDef_AssnMap _T180;void*_T181;void*_T182;struct _tuple14 _T183;struct _tuple14 _T184;struct _tuple14 _T185;struct Cyc_Absyn_Exp*_T186;struct Cyc_Vcgen_Env*_T187;int _T188;void*_T189;struct Cyc_Vcgen_Env*_T18A;int _T18B;int _T18C;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T18D;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T18E;void*_T18F;void*_T190;struct Cyc_Vcgen_Env*_T191;struct Cyc_Absyn_Exp*_T192;struct Cyc_AssnDef_AssnMap _T193;struct Cyc_Absyn_PtrInfo _T194;struct Cyc_Absyn_PtrAtts _T195;void*_T196;struct Cyc_Absyn_PtrInfo _T197;struct Cyc_Absyn_PtrAtts _T198;void*_T199;int _T19A;int _T19B;void*_T19C;void*_T19D;void*_T19E;void*_T19F;void*_T1A0;struct Cyc_Vcgen_ExpChecks*_T1A1;struct Cyc_Vcgen_ExpChecks*_T1A2;struct Cyc_Vcgen_ExpChecks*_T1A3;struct Cyc_Vcgen_Env*_T1A4;struct Cyc_Hashtable_Table**_T1A5;struct Cyc_Absyn_Exp*_T1A6;struct Cyc_Vcgen_ExpChecks*_T1A7;struct Cyc_Vcgen_Env*_T1A8;struct Cyc_AssnDef_AssnMap _T1A9;void*_T1AA;struct Cyc_AssnDef_AssnMap _T1AB;void*_T1AC;void*_T1AD;struct _tuple14 _T1AE;struct _tuple14 _T1AF;struct Cyc_Vcgen_Env*_T1B0;int _T1B1;enum Cyc_Absyn_Coercion _T1B2;int _T1B3;struct _tuple14 _T1B4;struct Cyc_Vcgen_Env*_T1B5;struct Cyc_AssnDef_AssnMap _T1B6;void*_T1B7;void*_T1B8;struct _tuple14 _T1B9;struct _tuple14 _T1BA;struct _tuple14 _T1BB;struct Cyc_Absyn_Exp*_T1BC;int*_T1BD;unsigned _T1BE;int _T1BF;unsigned _T1C0;struct Cyc_Vcgen_Env*_T1C1;struct Cyc_Absyn_Exp*_T1C2;int _T1C3;unsigned _T1C4;void*_T1C5;struct Cyc_Absyn_Exp*_T1C6;void*_T1C7;struct Cyc_AssnDef_AssnMap _T1C8;void*_T1C9;void*_T1CA;void*_T1CB;void*_T1CC;void*_T1CD;void*_T1CE;void*_T1CF;struct _tuple14 _T1D0;struct Cyc_Vcgen_Env*_T1D1;struct Cyc_Absyn_Vardecl*_T1D2;struct Cyc_Absyn_Exp*_T1D3;struct Cyc_Absyn_Exp*_T1D4;struct Cyc_Absyn_Exp*_T1D5;void*_T1D6;void*_T1D7;struct Cyc_AssnDef_AssnMap _T1D8;struct Cyc_AssnDef_AssnMap _T1D9;void*_T1DA;void*_T1DB;void*_T1DC;struct _tuple14 _T1DD;struct Cyc_Vcgen_Env*_T1DE;struct Cyc_Absyn_Exp*_T1DF;void*_T1E0;struct Cyc_Absyn_Exp*_T1E1;void*_T1E2;struct Cyc_AssnDef_AssnMap _T1E3;void*_T1E4;void*_T1E5;struct Cyc_Absyn_Exp*_T1E6;void*_T1E7;void*_T1E8;void*_T1E9;void*_T1EA;void*_T1EB;void*_T1EC;void*_T1ED;void*_T1EE;struct _tuple14 _T1EF;int _T1F0;struct _tuple14 _T1F1;struct _tuple14 _T1F2;struct Cyc_Absyn_Exp*_T1F3;void*_T1F4;void*_T1F5;struct _fat_ptr*_T1F6;struct Cyc_Vcgen_Env*_T1F7;int _T1F8;int _T1F9;int _T1FA;void*_T1FB;int _T1FC;unsigned _T1FD;void*_T1FE;struct Cyc_Vcgen_Env*_T1FF;struct Cyc_AssnDef_AssnMap _T200;void*_T201;struct Cyc_AssnDef_AssnMap _T202;void*_T203;void*_T204;struct _tuple14 _T205;void*_T206;int _T207;unsigned _T208;struct Cyc_Absyn_Exp*_T209;void*_T20A;struct Cyc_Absyn_Exp*_T20B;void*_T20C;void*_T20D;struct _fat_ptr*_T20E;struct Cyc_Vcgen_Env*_T20F;void*_T210;void*_T211;int _T212;unsigned _T213;void*_T214;struct Cyc_AssnDef_AssnMap _T215;struct _tuple14 _T216;struct _tuple14 _T217;struct Cyc_Vcgen_Env*_T218;struct Cyc_Absyn_Exp*_T219;struct Cyc_Absyn_Exp*_T21A;struct Cyc_AssnDef_AssnMap _T21B;struct _tuple14 _T21C;void*_T21D;void*_T21E;struct Cyc_Absyn_Exp*_T21F;void*_T220;struct Cyc_Vcgen_Env*_T221;struct Cyc_Absyn_Exp*_T222;struct Cyc_Absyn_Exp*_T223;struct Cyc_AssnDef_AssnMap _T224;struct Cyc_Absyn_Exp*_T225;void*_T226;void*_T227;void*_T228;struct _fat_ptr*_T229;struct Cyc_Vcgen_Env*_T22A;int _T22B;int _T22C;int _T22D;void*_T22E;void*_T22F;int _T230;unsigned _T231;void*_T232;struct Cyc_Vcgen_Env*_T233;struct Cyc_AssnDef_AssnMap _T234;void*_T235;struct Cyc_AssnDef_AssnMap _T236;void*_T237;void*_T238;struct Cyc_Absyn_Exp*_T239;void*_T23A;void*_T23B;struct _tuple14 _T23C;void*_T23D;int _T23E;unsigned _T23F;struct Cyc_Absyn_Exp*_T240;void*_T241;struct _tuple14 _T242;void*_T243;void*_T244;struct Cyc_Absyn_Exp*_T245;void*_T246;struct _tuple14 _T247;unsigned _T248;struct Cyc_List_List*_T249;struct Cyc_Absyn_Exp*_T24A;void*_T24B;struct Cyc_List_List*_T24C;void*_T24D;struct _tuple14 _T24E;struct Cyc_List_List*_T24F;struct _tuple14 _T250;struct Cyc_Absyn_Exp*_T251;void*_T252;struct Cyc_List_List*_T253;void*_T254;struct Cyc_List_List*_T255;struct Cyc_List_List*_T256;void*_T257;int*_T258;int _T259;struct Cyc_List_List*_T25A;struct Cyc_List_List*_T25B;struct Cyc_List_List*_T25C;struct Cyc_List_List*_T25D;void*_T25E;int _T25F;struct Cyc_List_List*_T260;struct _tuple21*_T261;int(*_T262)(struct _fat_ptr,struct _fat_ptr);void*(*_T263)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T264;struct _fat_ptr _T265;struct Cyc_List_List*_T266;struct Cyc_List_List*(*_T267)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*);struct Cyc_List_List*(*_T268)(int(*)(void*,void*),struct Cyc_List_List*);struct Cyc_List_List*_T269;struct Cyc_List_List*(*_T26A)(void*(*)(struct _tuple21*),struct Cyc_List_List*);struct Cyc_List_List*(*_T26B)(void*(*)(void*),struct Cyc_List_List*);void*(*_T26C)(struct _tuple21*);void*(*_T26D)(struct _tuple0*);struct Cyc_List_List*_T26E;void*_T26F;enum Cyc_Absyn_AggrKind _T270;int _T271;unsigned _T272;struct Cyc_List_List*_T273;struct Cyc_Absyn_Exp*_T274;void*_T275;struct Cyc_List_List*_T276;struct Cyc_Absyn_Exp*_T277;void*_T278;struct _tuple14 _T279;struct Cyc_Vcgen_Env*_T27A;struct Cyc_Absyn_Vardecl*_T27B;struct Cyc_Absyn_Exp*_T27C;struct Cyc_Absyn_Exp*_T27D;struct Cyc_Absyn_Exp*_T27E;void*_T27F;void*_T280;struct Cyc_AssnDef_AssnMap _T281;struct _tuple14 _T282;struct _tuple14 _T283;struct _tuple14 _T284;struct Cyc_Absyn_MallocInfo _T285;struct Cyc_Absyn_Exp*_T286;struct Cyc_Vcgen_Env*_T287;struct Cyc_Absyn_MallocInfo _T288;struct Cyc_Absyn_Exp*_T289;struct Cyc_AssnDef_AssnMap _T28A;struct _tuple14 _T28B;struct Cyc_Vcgen_Env*_T28C;struct Cyc_Absyn_MallocInfo _T28D;struct Cyc_Absyn_Exp*_T28E;struct Cyc_AssnDef_AssnMap _T28F;struct Cyc_Vcgen_Env*_T290;struct Cyc_Absyn_Exp*_T291;void*_T292;struct Cyc_Absyn_Exp*_T293;void*_T294;struct Cyc_AssnDef_AssnMap _T295;void*_T296;void*_T297;void*_T298;void*_T299;void*_T29A;void*_T29B;void*_T29C;struct _tuple14 _T29D;struct _tuple14 _T29E;struct Cyc_Vcgen_Env*_T29F;struct Cyc_Vcgen_Env*_T2A0;void**_T2A1;struct Cyc_Vcgen_Env*_T2A2;void**_T2A3;void**_T2A4;struct Cyc_Vcgen_Env*_T2A5;struct _tuple14 _T2A6;struct _tuple14 _T2A7;struct _tuple14 _T2A8;int _T2A9;struct Cyc_Vcgen_Env*_T2AA;struct Cyc_Vcgen_Env*_T2AB;struct Cyc_Vcgen_Env*_T2AC;struct Cyc_Vcgen_Env*_T2AD;struct Cyc_Vcgen_Env*_T2AE;struct Cyc_Absyn_Exp*_T2AF;struct Cyc_AssnDef_AssnMap _T2B0;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2B1;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2B2;struct Cyc_AssnDef_AssnMap _T2B3;struct Cyc_Vcgen_Env*_T2B4;struct Cyc_Vcgen_Env*_T2B5;struct Cyc_AssnDef_AssnMap _T2B6;void*_T2B7;struct Cyc_AssnDef_AssnMap _T2B8;void*_T2B9;int _T2BA;struct Cyc_String_pa_PrintArg_struct _T2BB;struct Cyc_AssnDef_AssnMap _T2BC;void*_T2BD;struct Cyc_String_pa_PrintArg_struct _T2BE;struct Cyc_Absyn_Exp*_T2BF;unsigned _T2C0;struct _fat_ptr _T2C1;struct _fat_ptr _T2C2;struct _tuple14 _T2C3;struct Cyc_AssnDef_AssnMap _T2C4;struct Cyc_AssnDef_AssnMap _T2C5;void*_T2C6;struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T2C7;int*_T2C8;struct Cyc_AssnDef_AssnMap _T2C9;void*_T2CA;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T2CB;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T2CC;void*_T2CD;int _T2CE;int _T2CF;int*_T2D0;int*_T2D1;int*_T2D2;int _T2D3;struct Cyc_Warn_String_Warn_Warg_struct _T2D4;struct Cyc_Warn_Exp_Warn_Warg_struct _T2D5;struct Cyc_Warn_String_Warn_Warg_struct _T2D6;struct Cyc_Absyn_Exp*_T2D7;unsigned _T2D8;struct _fat_ptr _T2D9;void*_T2DA;struct _tuple14 _T2DB;struct Cyc_Vcgen_Env*_T2DC;struct Cyc_Vcgen_Env*_T2DD;struct Cyc_Vcgen_Env*_T2DE;struct Cyc_Vcgen_Env*_T2DF;struct Cyc_Vcgen_Env*_T2E0;struct Cyc_Absyn_Exp*_T2E1;struct Cyc_AssnDef_AssnMap _T2E2;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2E3;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2E4;struct Cyc_AssnDef_AssnMap _T2E5;struct Cyc_Vcgen_Env*_T2E6;struct Cyc_Vcgen_Env*_T2E7;struct Cyc_AssnDef_AssnMap _T2E8;void*_T2E9;struct Cyc_AssnDef_AssnMap _T2EA;void*_T2EB;int _T2EC;struct Cyc_String_pa_PrintArg_struct _T2ED;struct Cyc_String_pa_PrintArg_struct _T2EE;struct Cyc_Absyn_Exp*_T2EF;unsigned _T2F0;struct _fat_ptr _T2F1;struct _fat_ptr _T2F2;struct _tuple14 _T2F3;_T0=e;{
# 994 "vcgen.cyc"
void*_T2F4=_T0->r;struct Cyc_Absyn_MallocInfo _T2F5;struct Cyc_Absyn_Datatypefield*_T2F6;struct Cyc_Absyn_Datatypedecl*_T2F7;int _T2F8;int _T2F9;struct _fat_ptr*_T2FA;enum Cyc_Absyn_Coercion _T2FB;struct Cyc_List_List*_T2FC;struct Cyc_Absyn_Exp*_T2FD;enum Cyc_Absyn_Incrementor _T2FE;struct Cyc_Core_Opt*_T2FF;struct Cyc_Absyn_Exp*_T300;enum Cyc_Absyn_Primop _T301;void*_T302;struct _fat_ptr _T303;union Cyc_Absyn_Cnst _T304;_T1=(int*)_T2F4;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T2F4;_T304=_T305->f1;}{union Cyc_Absyn_Cnst c=_T304;
# 996
struct _tuple16 _T305=Cyc_Evexp_eval_const_uint_exp(e);int _T306;unsigned _T307;_T307=_T305.f0;_T306=_T305.f1;{unsigned cn=_T307;int known=_T306;_T3=known;
if(!_T3)goto _TLA6;{struct _tuple14 _T308;_T308.f0=Cyc_AssnDef_uint(cn);_T308.f1=amin;_T4=_T308;}return _T4;
_TLA6:{struct _tuple14 _T308;_T308.f0=Cyc_AssnDef_cnst(e);_T308.f1=amin;_T5=_T308;}return _T5;}}case 2:{struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_T2F4;_T303=_T305->f1;}{struct _fat_ptr s=_T303;_T6=s;_T7=
# 1000
_tag_fat("print_assn",sizeof(char),11U);_T8=Cyc_strcmp(_T6,_T7);if(_T8!=0)goto _TLA8;_T9=e;_TA=_T9->loc;{
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(_TA);{struct Cyc_String_pa_PrintArg_struct _T305;_T305.tag=0;
_T305.f1=seg_str;_TB=_T305;}{struct Cyc_String_pa_PrintArg_struct _T305=_TB;{struct Cyc_String_pa_PrintArg_struct _T306;_T306.tag=0;_T306.f1=Cyc_AssnDef_assnmap2string(amin);_TC=_T306;}{struct Cyc_String_pa_PrintArg_struct _T306=_TC;void*_T307[2];_T307[0]=& _T305;_T307[1]=& _T306;_TD=Cyc_stderr;_TE=_tag_fat("%s: %s\n",sizeof(char),8U);_TF=_tag_fat(_T307,sizeof(void*),2);Cyc_fprintf(_TD,_TE,_TF);}}}goto _TLA9;_TLA8: _TLA9: _T10=s;_T11=
# 1004
_tag_fat("print_dag",sizeof(char),10U);_T12=Cyc_strcmp(_T10,_T11);if(_T12!=0)goto _TLAA;_T13=e;_T14=_T13->loc;{
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(_T14);{struct Cyc_String_pa_PrintArg_struct _T305;_T305.tag=0;
_T305.f1=seg_str;_T15=_T305;}{struct Cyc_String_pa_PrintArg_struct _T305=_T15;{struct Cyc_String_pa_PrintArg_struct _T306;_T306.tag=0;_T306.f1=Cyc_AssnDef_assnmap2dag(amin);_T16=_T306;}{struct Cyc_String_pa_PrintArg_struct _T306=_T16;void*_T307[2];_T307[0]=& _T305;_T307[1]=& _T306;_T17=Cyc_stderr;_T18=_tag_fat("%s: %s\n",sizeof(char),8U);_T19=_tag_fat(_T307,sizeof(void*),2);Cyc_fprintf(_T17,_T18,_T19);}}}goto _TLAB;_TLAA: _TLAB: _T1A=s;_T1B=
# 1008
_tag_fat("all_paths",sizeof(char),10U);_T1C=Cyc_strcmp(_T1A,_T1B);if(_T1C!=0)goto _TLAC;_T1D=env;
_T1D->widen_paths=0;goto _TLAD;_TLAC: _TLAD: _T1E=s;_T1F=
# 1011
_tag_fat("one_path",sizeof(char),9U);_T20=Cyc_strcmp(_T1E,_T1F);if(_T20!=0)goto _TLAE;_T21=env;
_T21->widen_paths=1;goto _TLAF;_TLAE: _TLAF:{struct _tuple14 _T305;
# 1014
_T305.f0=Cyc_AssnDef_zero();_T305.f1=amin;_T22=_T305;}return _T22;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T2F4;_T23=_T305->f1;_T302=(void*)_T23;}{void*b=_T302;
# 1019
struct Cyc_Absyn_Vardecl*vdopt=Cyc_Vcgen_bind2vardecl(b);
if(vdopt!=0)goto _TLB0;{struct _tuple14 _T305;_T305.f0=Cyc_AssnDef_fresh_var(0,0);_T305.f1=amin;_T24=_T305;}return _T24;_TLB0: {
struct Cyc_Absyn_Vardecl*vd=vdopt;_T25=vd;_T26=_T25->escapes;
if(!_T26)goto _TLB2;{
struct _tuple14 _T305=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*mem=_T307;struct Cyc_AssnDef_AssnMap am1=_T306;{struct _tuple14 _T308;_T28=mem;_T29=
Cyc_Vcgen_myaddr(env,vd);_T2A=vd;_T2B=_T2A->type;_T308.f0=Cyc_AssnDef_select(_T28,_T29,_T2B);_T308.f1=am1;_T27=_T308;}return _T27;}}
# 1027
_TLB2: _T2C=vd;_T2D=_T2C->type;{void*_T305=Cyc_Absyn_compress(_T2D);_T2E=(int*)_T305;_T2F=*_T2E;if(_T2F!=5)goto _TLB4;{struct _tuple14 _T306;
# 1029
_T306.f0=Cyc_Vcgen_myaddr(env,vd);_T306.f1=amin;_T30=_T306;}return _T30;_TLB4: goto _LL63;_LL63:;}{
# 1032
struct _tuple14 _T305=Cyc_AssnDef_lookup_var_map(vd,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t=_T307;struct Cyc_AssnDef_AssnMap am=_T306;{struct _tuple14 _T308;
_T308.f0=t;_T308.f1=am;_T31=_T308;}return _T31;_T32=
Cyc_AssnDef_lookup_var_map(vd,amin);return _T32;}}}}case 3: _T33=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2F4;_T34=_T33->f2;if(_T34==0)goto _TLB6;_T35=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2F4;_T36=_T35->f2;_T37=(struct Cyc_List_List*)_T36;_T38=_T37->tl;if(_T38!=0)goto _TLB8;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2F4;_T301=_T305->f1;_T39=_T305->f2;{struct Cyc_List_List _T306=*_T39;_T3A=_T306.hd;_T302=(struct Cyc_Absyn_Exp*)_T3A;}}{enum Cyc_Absyn_Primop p=_T301;struct Cyc_Absyn_Exp*e1=_T302;
# 1038
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t=_T307;struct Cyc_AssnDef_AssnMap am=_T306;{struct _tuple14 _T308;_T3C=p;_T3D=t;_T3E=e;_T3F=_T3E->topt;
_T308.f0=Cyc_AssnDef_unop(_T3C,_T3D,_T3F);_T308.f1=am;_T3B=_T308;}return _T3B;}}_TLB8:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2F4;_T301=_T305->f1;_T40=_T305->f2;{struct Cyc_List_List _T306=*_T40;_T41=_T306.hd;_T302=(struct Cyc_Absyn_Exp*)_T41;_T42=_T306.tl;{struct Cyc_List_List _T307=*_T42;_T43=_T307.hd;_T300=(struct Cyc_Absyn_Exp*)_T43;}}}{enum Cyc_Absyn_Primop p=_T301;struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_Absyn_Exp*e2=_T300;
# 1042
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t1=_T307;struct Cyc_AssnDef_AssnMap am1=_T306;
struct _tuple14 _T308=Cyc_Vcgen_vcgen_rexp(env,e2,am1);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*t2=_T30A;struct Cyc_AssnDef_AssnMap am2=_T309;_T44=env;_T45=e;_T46=_T45->topt;_T47=p;_T48=t1;_T49=t2;_T4A=am2;_T4B=
Cyc_Vcgen_vcgen_binop(_T44,_T46,_T47,_T48,_T49,_T4A);return _T4B;}}}_TLB6: _T4D=Cyc_Warn_impos;{
int(*_T305)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T4D;_T4C=_T305;}_T4E=_tag_fat("vcgen: bad primop",sizeof(char),18U);_T4F=_tag_fat(0U,sizeof(void*),0);_T4C(_T4E,_T4F);;case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T2FF=_T305->f2;_T300=_T305->f3;}{struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_Core_Opt*popt=_T2FF;struct Cyc_Absyn_Exp*e2=_T300;
# 1048
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e2,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t2=_T307;struct Cyc_AssnDef_AssnMap am1=_T306;
struct _tuple14 _T308=Cyc_Vcgen_vcgen_lexp(env,e1,am1);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*lt=_T30A;struct Cyc_AssnDef_AssnMap am2=_T309;
struct Cyc_AssnDef_AssnMap amout;
if(popt==0)goto _TLBA;{
struct _tuple14 _T30B=Cyc_Vcgen_deref_lterm(env,lt,am2);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*t1=_T30D;struct Cyc_AssnDef_AssnMap am3=_T30C;_T50=env;_T51=e;_T52=_T51->topt;_T53=popt;_T54=_T53->v;_T55=(enum Cyc_Absyn_Primop)_T54;_T56=t1;_T57=t2;_T58=am3;{
struct _tuple14 _T30E=Cyc_Vcgen_vcgen_binop(_T50,_T52,_T55,_T56,_T57,_T58);struct Cyc_AssnDef_AssnMap _T30F;void*_T310;_T310=_T30E.f0;_T30F=_T30E.f1;{void*t2=_T310;struct Cyc_AssnDef_AssnMap am4=_T30F;
amout=Cyc_Vcgen_do_assign(env,am4,lt,t2);{struct _tuple14 _T311;
_T311.f0=t2;_T311.f1=amout;_T59=_T311;}return _T59;}}}}
# 1057
_TLBA: amout=Cyc_Vcgen_do_assign(env,am2,lt,t2);{struct _tuple14 _T30B;
_T30B.f0=t2;_T30B.f1=amout;_T5A=_T30B;}return _T5A;}}}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T2FE=_T305->f2;}{struct Cyc_Absyn_Exp*e=_T302;enum Cyc_Absyn_Incrementor i=_T2FE;
# 1061
struct _tuple14 _T305=Cyc_Vcgen_vcgen_lexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*lt=_T307;struct Cyc_AssnDef_AssnMap am1=_T306;
struct _tuple14 _T308=Cyc_Vcgen_deref_lterm(env,lt,am1);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*t1=_T30A;struct Cyc_AssnDef_AssnMap am2=_T309;
void*res;void*rvalue;
struct Cyc_AssnDef_AssnMap amout;_T5B=i;_T5C=(int)_T5B;switch(_T5C){case Cyc_Absyn_PostInc: _T5D=env;_T5E=e;_T5F=_T5E->topt;_T60=t1;_T61=
# 1067
Cyc_AssnDef_one();_T62=am2;{struct _tuple14 _T30B=Cyc_Vcgen_vcgen_binop(_T5D,_T5F,0U,_T60,_T61,_T62);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*t=_T30D;struct Cyc_AssnDef_AssnMap am=_T30C;
res=t1;
rvalue=t;
amout=am;goto _LL86;}}case Cyc_Absyn_PreInc: _T63=env;_T64=e;_T65=_T64->topt;_T66=t1;_T67=
# 1073
Cyc_AssnDef_one();_T68=am2;{struct _tuple14 _T30B=Cyc_Vcgen_vcgen_binop(_T63,_T65,0U,_T66,_T67,_T68);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*t=_T30D;struct Cyc_AssnDef_AssnMap am=_T30C;
rvalue=t;
res=rvalue;
amout=am;goto _LL86;}}case Cyc_Absyn_PostDec: _T69=env;_T6A=e;_T6B=_T6A->topt;_T6C=t1;_T6D=
# 1079
Cyc_AssnDef_one();_T6E=am2;{struct _tuple14 _T30B=Cyc_Vcgen_vcgen_binop(_T69,_T6B,2U,_T6C,_T6D,_T6E);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*t=_T30D;struct Cyc_AssnDef_AssnMap am=_T30C;
res=t1;
rvalue=t;
amout=am;goto _LL86;}}case Cyc_Absyn_PreDec: _T6F=env;_T70=e;_T71=_T70->topt;_T72=t1;_T73=
# 1085
Cyc_AssnDef_one();_T74=am2;{struct _tuple14 _T30B=Cyc_Vcgen_vcgen_binop(_T6F,_T71,2U,_T72,_T73,_T74);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*t=_T30D;struct Cyc_AssnDef_AssnMap am=_T30C;
rvalue=t;
res=rvalue;
amout=am;goto _LL86;}}default: _T76=Cyc_Warn_impos;{
# 1091
int(*_T30B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T76;_T75=_T30B;}_T77=_tag_fat("vcgen rexp increment_e",sizeof(char),23U);_T78=_tag_fat(0U,sizeof(void*),0);_T75(_T77,_T78);}_LL86:
# 1093
 amout=Cyc_Vcgen_do_assign(env,amout,lt,rvalue);{struct _tuple14 _T30B;
_T30B.f0=res;_T30B.f1=amout;_T79=_T30B;}return _T79;}}}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T300=_T305->f2;_T2FD=_T305->f3;}{struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_Absyn_Exp*e2=_T300;struct Cyc_Absyn_Exp*e3=_T2FD;
# 1096
struct _tuple18 _T305=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;struct Cyc_AssnDef_AssnMap _T307;_T307=_T305.f0;_T306=_T305.f1;{struct Cyc_AssnDef_AssnMap amt=_T307;struct Cyc_AssnDef_AssnMap amf=_T306;
struct _tuple14 _T308=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*t1=_T30A;struct Cyc_AssnDef_AssnMap amt=_T309;
struct _tuple14 _T30B=Cyc_Vcgen_vcgen_rexp(env,e3,amf);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*t2=_T30D;struct Cyc_AssnDef_AssnMap amf=_T30C;_T7A=e;_T7B=_T7A->topt;{
void*v=Cyc_AssnDef_fresh_var(0,_T7B);_T7C=amt;_T7D=
Cyc_AssnDef_eq(v,t1);amt=Cyc_AssnDef_and_assnmap_assn(_T7C,_T7D);_T7E=amf;_T7F=
Cyc_AssnDef_eq(v,t2);amf=Cyc_AssnDef_and_assnmap_assn(_T7E,_T7F);{struct _tuple14 _T30E;
_T30E.f0=v;_T81=env;_T82=Cyc_AssnDef_or_assnmap_assnmap(amt,amf);_T30E.f1=Cyc_Vcgen_may_widen(_T81,_T82);_T80=_T30E;}return _T80;}}}}}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T300=_T305->f2;}{struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_Absyn_Exp*e2=_T300;
# 1104
struct _tuple18 _T305=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;struct Cyc_AssnDef_AssnMap _T307;_T307=_T305.f0;_T306=_T305.f1;{struct Cyc_AssnDef_AssnMap amt=_T307;struct Cyc_AssnDef_AssnMap amf=_T306;
struct _tuple14 _T308=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*t=_T30A;struct Cyc_AssnDef_AssnMap amt=_T309;_T83=e;_T84=_T83->topt;{
void*v=Cyc_AssnDef_fresh_var(0,_T84);_T85=amt;_T86=
Cyc_AssnDef_eq(v,t);amt=Cyc_AssnDef_and_assnmap_assn(_T85,_T86);_T87=amf;_T88=v;_T89=
Cyc_AssnDef_zero();_T8A=Cyc_AssnDef_eq(_T88,_T89);amf=Cyc_AssnDef_and_assnmap_assn(_T87,_T8A);{struct _tuple14 _T30B;
_T30B.f0=v;_T8C=env;_T8D=Cyc_AssnDef_or_assnmap_assnmap(amt,amf);_T30B.f1=Cyc_Vcgen_may_widen(_T8C,_T8D);_T8B=_T30B;}return _T8B;}}}}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T300=_T305->f2;}{struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_Absyn_Exp*e2=_T300;
# 1111
struct _tuple18 _T305=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;struct Cyc_AssnDef_AssnMap _T307;_T307=_T305.f0;_T306=_T305.f1;{struct Cyc_AssnDef_AssnMap amt=_T307;struct Cyc_AssnDef_AssnMap amf=_T306;
struct _tuple14 _T308=Cyc_Vcgen_vcgen_rexp(env,e2,amf);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*t=_T30A;struct Cyc_AssnDef_AssnMap amf=_T309;_T8E=e;_T8F=_T8E->topt;{
void*v=Cyc_AssnDef_fresh_var(0,_T8F);_T90=amt;_T91=v;_T92=
Cyc_AssnDef_zero();_T93=Cyc_AssnDef_neq(_T91,_T92);amt=Cyc_AssnDef_and_assnmap_assn(_T90,_T93);_T94=amf;_T95=
Cyc_AssnDef_eq(v,t);amf=Cyc_AssnDef_and_assnmap_assn(_T94,_T95);{struct _tuple14 _T30B;
_T30B.f0=v;_T97=env;_T98=Cyc_AssnDef_or_assnmap_assnmap(amt,amf);_T30B.f1=Cyc_Vcgen_may_widen(_T97,_T98);_T96=_T30B;}return _T96;}}}}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T300=_T305->f2;}{struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_Absyn_Exp*e2=_T300;
# 1118
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;_T306=_T305.f1;{struct Cyc_AssnDef_AssnMap am1=_T306;_T99=
Cyc_Vcgen_vcgen_rexp(env,e2,am1);return _T99;}}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T2FC=_T305->f2;}{struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_List_List*es=_T2FC;
# 1121
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*f=_T307;struct Cyc_AssnDef_AssnMap am1=_T306;
struct _tuple17 _T308=Cyc_Vcgen_vcgen_rexps(env,es,am1);struct Cyc_AssnDef_AssnMap _T309;struct Cyc_List_List*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{struct Cyc_List_List*ts=_T30A;struct Cyc_AssnDef_AssnMap am2=_T309;
struct _tuple14 _T30B=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am2);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*pre_memory=_T30D;struct Cyc_AssnDef_AssnMap am2=_T30C;_T9A=e1;_T9B=_T9A->topt;{
struct _tuple19 _T30E=Cyc_Vcgen_get_requires_and_ensures(_T9B);struct Cyc_Absyn_Vardecl*_T30F;struct Cyc_AssnDef_ExistAssnFn*_T310;struct Cyc_AssnDef_ExistAssnFn*_T311;struct Cyc_AssnDef_ExistAssnFn*_T312;struct Cyc_AssnDef_ExistAssnFn*_T313;struct Cyc_List_List*_T314;struct Cyc_List_List*_T315;_T315=_T30E.f0;_T314=_T30E.f1;_T313=_T30E.f2;_T312=_T30E.f3;_T311=_T30E.f4;_T310=_T30E.f5;_T30F=_T30E.f6;{struct Cyc_List_List*args=_T315;struct Cyc_List_List*argvds=_T314;struct Cyc_AssnDef_ExistAssnFn*checkopt=_T313;struct Cyc_AssnDef_ExistAssnFn*reqopt=_T312;struct Cyc_AssnDef_ExistAssnFn*ensopt=_T311;struct Cyc_AssnDef_ExistAssnFn*thrwsopt=_T310;struct Cyc_Absyn_Vardecl*ret_value=_T30F;_T9C=env;_T9D=_T9C->fn_precond_info;_T9E=e1;_T9F=am2;_TA0=_T9F.assn;_TA1=pre_memory;_TA2=ts;
# 1130
Cyc_Vcgen_insert_fn_precond_info(_T9D,_T9E,_TA0,_TA1,_TA2);{
struct Cyc_AssnDef_ExistAssnFn*fensinfo=Cyc_Vcgen_lookup_fn_summary_info(e1);_TA3=e;_TA4=_TA3->topt;{
# 1136
void*actual_ret=Cyc_AssnDef_fresh_var(0,_TA4);
# 1138
struct Cyc_List_List*pre_memory_and_ts;pre_memory_and_ts=_cycalloc(sizeof(struct Cyc_List_List));_TA5=pre_memory_and_ts;_TA5->hd=pre_memory;_TA6=pre_memory_and_ts;_TA6->tl=ts;
if(reqopt==0)goto _TLBD;{
# 1141
void*req_assn=Cyc_AssnDef_existassnfn2assn(reqopt,pre_memory_and_ts);_TA7=am2;_TA8=_TA7.assn;_TA9=req_assn;_TAA=
Cyc_Vcgen_implies(_TA8,_TA9);if(_TAA)goto _TLBF;else{goto _TLC1;}
_TLC1:{struct Cyc_String_pa_PrintArg_struct _T316;_T316.tag=0;
_T316.f1=Cyc_AssnDef_assnmap2string(am2);_TAB=_T316;}{struct Cyc_String_pa_PrintArg_struct _T316=_TAB;{struct Cyc_String_pa_PrintArg_struct _T317;_T317.tag=0;_T317.f1=Cyc_AssnDef_assn2string(req_assn);_TAC=_T317;}{struct Cyc_String_pa_PrintArg_struct _T317=_TAC;void*_T318[2];_T318[0]=& _T316;_T318[1]=& _T317;_TAD=e;_TAE=_TAD->loc;_TAF=
# 1143
_tag_fat("current assertion:\n%s\ndoes not satisfy the pre-condition of the function:\n%s\n",sizeof(char),78U);_TB0=_tag_fat(_T318,sizeof(void*),2);Cyc_Warn_err(_TAE,_TAF,_TB0);}}goto _TLC0;_TLBF: _TLC0:;}goto _TLBE;_TLBD: _TLBE: {
# 1150
void*new_memory=Cyc_AssnDef_fresh_var(Cyc_AssnDef_memory,0);
struct Cyc_AssnDef_AssnMap am3=Cyc_AssnDef_update_var_map(Cyc_AssnDef_memory,new_memory,am2);_TB1=
# 1154
Cyc_Absyn_exn_type();{void*v=Cyc_AssnDef_fresh_var(0,_TB1);
if(thrwsopt==0)goto _TLC2;_TB2=thrwsopt;{struct Cyc_List_List*_T316=_cycalloc(sizeof(struct Cyc_List_List));
_T316->hd=new_memory;_T316->tl=pre_memory_and_ts;_TB3=(struct Cyc_List_List*)_T316;}{void*throws_assn=Cyc_AssnDef_existassnfn2assn(_TB2,_TB3);_TB4=env;_TB5=
Cyc_AssnDef_and_assnmap_assn(am3,throws_assn);_TB6=v;Cyc_Vcgen_update_try_assnmap(_TB4,_TB5,_TB6);}goto _TLC3;
# 1159
_TLC2: Cyc_Vcgen_update_try_assnmap(env,am3,v);_TLC3:
# 1162
 if(ensopt==0)goto _TLC4;_TB7=ensopt;{struct Cyc_List_List*_T316=_cycalloc(sizeof(struct Cyc_List_List));
_T316->hd=actual_ret;{struct Cyc_List_List*_T317=_cycalloc(sizeof(struct Cyc_List_List));_T317->hd=new_memory;_T317->tl=pre_memory_and_ts;_TB9=(struct Cyc_List_List*)_T317;}_T316->tl=_TB9;_TB8=(struct Cyc_List_List*)_T316;}{void*ens_assn=Cyc_AssnDef_existassnfn2assn(_TB7,_TB8);
am3=Cyc_AssnDef_and_assnmap_assn(am3,ens_assn);}goto _TLC5;
_TLC4: if(fensinfo==0)goto _TLC6;_TBA=fensinfo;_TBB=_TBA->af;_TBC=_TBB->actuals;_TBD=
Cyc_List_length(_TBC);_TBE=Cyc_List_length(ts);_TBF=_TBE + 3;if(_TBD==_TBF)goto _TLC8;goto _TLC9;
# 1171
_TLC8: _TC0=fensinfo;{struct Cyc_List_List*_T316=_cycalloc(sizeof(struct Cyc_List_List));_T316->hd=actual_ret;{struct Cyc_List_List*_T317=_cycalloc(sizeof(struct Cyc_List_List));_T317->hd=new_memory;_T317->tl=pre_memory_and_ts;_TC2=(struct Cyc_List_List*)_T317;}_T316->tl=_TC2;_TC1=(struct Cyc_List_List*)_T316;}{void*ens_assn=Cyc_AssnDef_existassnfn2assn(_TC0,_TC1);
am3=Cyc_AssnDef_and_assnmap_assn(am3,ens_assn);}_TLC9: goto _TLC7;_TLC6: _TLC7: _TLC5: _TC3=e1;_TC4=_TC3->topt;_TC5=
# 1177
_check_null(_TC4);_TC6=Cyc_Atts_is_noreturn_fn_type(_TC5);if(!_TC6)goto _TLCA;_TC7=& Cyc_AssnDef_false_assn;_TC8=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_TC7;
am3.assn=(void*)_TC8;goto _TLCB;_TLCA: _TLCB:{struct _tuple14 _T316;
_T316.f0=actual_ret;_T316.f1=am3;_TC9=_T316;}return _TC9;}}}}}}}}}}case 11:{struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;}{struct Cyc_Absyn_Exp*e=_T302;
# 1181
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t=_T307;struct Cyc_AssnDef_AssnMap am1=_T306;
Cyc_Vcgen_update_try_assnmap(env,am1,t);{struct _tuple14 _T308;
_T308.f0=Cyc_AssnDef_fresh_var(0,0);_T308.f1=Cyc_AssnDef_false_assnmap();_TCA=_T308;}return _TCA;}}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;}{struct Cyc_Absyn_Exp*e=_T302;_TCB=
Cyc_Vcgen_vcgen_rexp(env,e,amin);return _TCB;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;}{struct Cyc_Absyn_Exp*e=_T302;_TCC=
Cyc_Vcgen_vcgen_rexp(env,e,amin);return _TCC;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T2F4;_TCD=_T305->f1;_T302=(void*)_TCD;_T300=_T305->f2;_T2FB=_T305->f4;}{void*tp=_T302;struct Cyc_Absyn_Exp*e1=_T300;enum Cyc_Absyn_Coercion c=_T2FB;_TCE=
# 1190
Cyc_Tcutil_is_const_exp(e1);if(!_TCE)goto _TLCC;{
struct _tuple16 _T305=Cyc_Evexp_eval_const_uint_exp(e);int _T306;unsigned _T307;_T307=_T305.f0;_T306=_T305.f1;{unsigned cn=_T307;int known=_T306;_TCF=known;
if(!_TCF)goto _TLCE;{struct _tuple14 _T308;_T308.f0=Cyc_AssnDef_uint(cn);_T308.f1=amin;_TD0=_T308;}return _TD0;_TLCE:;}}goto _TLCD;_TLCC: _TLCD: {
# 1194
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t=_T307;struct Cyc_AssnDef_AssnMap am=_T306;_TD1=tp;_TD2=e1;_TD3=_TD2->topt;_TD4=
# 1196
_check_null(_TD3);_TD5=Cyc_Unify_unify(_TD1,_TD4);if(!_TD5)goto _TLD0;{struct _tuple14 _T308;_T308.f0=t;_T308.f1=am;_TD6=_T308;}return _TD6;_TLD0:{struct _tuple0 _T308;
# 1203
_T308.f0=Cyc_Absyn_compress(tp);_TD8=e1;_TD9=_TD8->topt;_TDA=_check_null(_TD9);_T308.f1=Cyc_Absyn_compress(_TDA);_TD7=_T308;}{struct _tuple0 _T308=_TD7;struct Cyc_Absyn_PtrInfo _T309;enum Cyc_Absyn_Size_of _T30A;enum Cyc_Absyn_Sign _T30B;enum Cyc_Absyn_Size_of _T30C;enum Cyc_Absyn_Sign _T30D;struct Cyc_Absyn_PtrInfo _T30E;struct Cyc_List_List*_T30F;struct Cyc_Absyn_Vardecl*_T310;struct Cyc_AssnDef_ExistAssnFn*_T311;_TDB=_T308.f0;_TDC=(int*)_TDB;_TDD=*_TDC;if(_TDD!=12)goto _TLD2;_TDE=_T308.f0;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_TDE;_T311=_T312->f3;}_TDF=c;_TE0=(int)_TDF;if(_TE0!=1)goto _TLD4;{struct Cyc_AssnDef_ExistAssnFn*af=_T311;{struct _tuple14 _T312;
# 1207
_T312.f0=t;_T312.f1=am;_TE1=_T312;}return _TE1;}_TLD4: _TE2=_T308.f1;_TE3=(int*)_TE2;_TE4=*_TE3;if(_TE4!=12)goto _TLD6;goto _LLCD;_TLD6: _TE5=_T308.f0;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_TE5;_T310=_T312->f1;_T311=_T312->f3;}{struct Cyc_Absyn_Vardecl*vd=_T310;struct Cyc_AssnDef_ExistAssnFn*af=_T311;_TE6=
# 1215
_check_null(af);{struct Cyc_List_List*_T312=_cycalloc(sizeof(struct Cyc_List_List));_T312->hd=t;_T312->tl=0;_TE7=(struct Cyc_List_List*)_T312;}{void*check=Cyc_AssnDef_existassnfn2assn(_TE6,_TE7);_TE8=am;_TE9=_TE8.assn;_TEA=check;_TEB=
Cyc_Vcgen_implies(_TE9,_TEA);if(_TEB)goto _TLD8;else{goto _TLDA;}
_TLDA:{struct Cyc_String_pa_PrintArg_struct _T312;_T312.tag=0;_T312.f1=Cyc_AssnDef_assn2string(check);_TEC=_T312;}{struct Cyc_String_pa_PrintArg_struct _T312=_TEC;{struct Cyc_String_pa_PrintArg_struct _T313;_T313.tag=0;_T313.f1=Cyc_AssnDef_assnmap2string(amin);_TED=_T313;}{struct Cyc_String_pa_PrintArg_struct _T313=_TED;void*_T314[2];_T314[0]=& _T312;_T314[1]=& _T313;_TEE=e;_TEF=_TEE->loc;_TF0=_tag_fat("cannot prove that @subset predicate\n\t%s\nholds from\n\t%s",sizeof(char),55U);_TF1=_tag_fat(_T314,sizeof(void*),2);Cyc_Warn_err(_TEF,_TF0,_TF1);}}goto _TLD9;_TLD8: _TLD9: _TF2=am;_TF3=_TF2.assn;_TF4=check;
am.assn=Cyc_AssnDef_and(_TF3,_TF4);{struct _tuple14 _T312;
_T312.f0=t;_T312.f1=am;_TF5=_T312;}return _TF5;}}_TLD2: _TF6=_T308.f1;_TF7=(int*)_TF6;_TF8=*_TF7;if(_TF8!=12)goto _TLDB;_LLCD: _TF9=_T308.f1;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_TF9;_T311=_T312->f3;}_TFA=c;_TFB=(int)_TFA;if(_TFB!=1)goto _TLDD;{struct Cyc_AssnDef_ExistAssnFn*af=_T311;{struct _tuple14 _T312;
# 1210
_T312.f0=t;_T312.f1=am;_TFC=_T312;}return _TFC;}_TLDD: _TFD=_T308.f0;_TFE=(int*)_TFD;_TFF=*_TFE;switch(_TFF){case 0: _T100=_T308.f0;_T101=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T100;_T102=_T101->f1;_T103=(int*)_T102;_T104=*_T103;if(_T104!=1)goto _TLE0;_T105=_T308.f0;_T106=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T105;_T107=_T106->f1;_T108=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T107;_T109=_T108->f2;_T10A=(int)_T109;switch(_T10A){case Cyc_Absyn_Int_sz: goto _LLDD;case Cyc_Absyn_Long_sz: goto _LLDD;default: goto _LLDD;}goto _TLE1;_TLE0: goto _LLDD;_TLE1:;case 4: goto _LLDD;default: goto _LLDD;}goto _TLDC;_TLDB: _T10B=_T308.f0;_T10C=(int*)_T10B;_T10D=*_T10C;switch(_T10D){case 0: _T10E=_T308.f0;_T10F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10E;_T110=_T10F->f1;_T111=(int*)_T110;_T112=*_T111;if(_T112!=1)goto _TLE4;_T113=_T308.f0;_T114=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T113;_T115=_T114->f1;_T116=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T115;_T117=_T116->f2;_T118=(int)_T117;switch(_T118){case Cyc_Absyn_Int_sz: _T119=_T308.f1;_T11A=(int*)_T119;_T11B=*_T11A;switch(_T11B){case 0: _T11C=_T308.f1;_T11D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11C;_T11E=_T11D->f1;_T11F=(int*)_T11E;_T120=*_T11F;switch(_T120){case 5: _T121=_T308.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T121;_T30F=_T312->f2;}{struct Cyc_List_List*tagtps=_T30F;{struct _tuple14 _T312;_T123=
# 1223
_check_null(tagtps);_T124=_T123->hd;_T125=e1;_T126=_T125->loc;_T127=Cyc_Absyn_valueof_exp(_T124,_T126);_T312.f0=Cyc_AssnDef_cnst(_T127);_T312.f1=am;_T122=_T312;}return _T122;}case 1: goto _LLD5;default: goto _LLDD;};case 4: _T128=_T308.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T128;_T30E=_T312->f1;}{struct Cyc_Absyn_PtrInfo pi=_T30E;_T30E=pi;goto _LLDA;}default: goto _LLDD;};case Cyc_Absyn_Long_sz: _T129=_T308.f1;_T12A=(int*)_T129;_T12B=*_T12A;switch(_T12B){case 0: _T12C=_T308.f1;_T12D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T12C;_T12E=_T12D->f1;_T12F=(int*)_T12E;_T130=*_T12F;switch(_T130){case 5: _T131=_T308.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T131;_T30F=_T312->f2;}{struct Cyc_List_List*tagtps=_T30F;{struct _tuple14 _T312;_T133=
# 1225
_check_null(tagtps);_T134=_T133->hd;_T135=e1;_T136=_T135->loc;_T137=Cyc_Absyn_valueof_exp(_T134,_T136);_T312.f0=Cyc_AssnDef_cnst(_T137);_T312.f1=am;_T132=_T312;}return _T132;}case 1: goto _LLD5;default: goto _LLDD;};case 4: _T138=_T308.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T138;_T30E=_T312->f1;}_LLDA: {struct Cyc_Absyn_PtrInfo pi=_T30E;_T139=
# 1238
Cyc_Absyn_bounds_one();_T13A=pi;_T13B=_T13A.ptr_atts;_T13C=_T13B.bounds;_T13D=Cyc_Tcutil_get_bounds_exp_constrain(_T139,_T13C,1);if(_T13D==0)goto _TLEB;{struct _tuple14 _T312;
# 1242
_T312.f0=t;_T312.f1=am;_T13E=_T312;}return _T13E;_TLEB: goto _LLCA;}default: goto _LLDD;};default: _T13F=_T308.f1;_T140=(int*)_T13F;_T141=*_T140;if(_T141!=0)goto _TLED;_T142=_T308.f1;_T143=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T142;_T144=_T143->f1;_T145=(int*)_T144;_T146=*_T145;if(_T146!=1)goto _TLEF;_LLD5: _T147=_T308.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T147;_T148=_T312->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T313=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T148;_T30D=_T313->f1;_T30C=_T313->f2;}}_T149=_T308.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T149;_T14A=_T312->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T313=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T14A;_T30B=_T313->f1;_T30A=_T313->f2;}}{enum Cyc_Absyn_Sign u1=_T30D;enum Cyc_Absyn_Size_of s1=_T30C;enum Cyc_Absyn_Sign u2=_T30B;enum Cyc_Absyn_Size_of s2=_T30A;_T14C=s1;_T14D=(int)_T14C;
# 1232
if(_T14D!=3)goto _TLF1;_T14B=2U;goto _TLF2;_TLF1: _T14B=s1;_TLF2: s1=_T14B;_T14F=s2;_T150=(int)_T14F;
if(_T150!=3)goto _TLF3;_T14E=2U;goto _TLF4;_TLF3: _T14E=s2;_TLF4: s2=_T14E;_T151=s1;_T152=(int)_T151;_T153=s2;_T154=(int)_T153;
if(_T152!=_T154)goto _TLF5;{struct _tuple14 _T312;_T312.f0=t;_T312.f1=am;_T155=_T312;}return _T155;_TLF5: goto _LLCA;}_TLEF: goto _LLDD;_TLED: goto _LLDD;}goto _TLE5;_TLE4: goto _LLDD;_TLE5:;case 4: _T156=_T308.f1;_T157=(int*)_T156;_T158=*_T157;if(_T158!=4)goto _TLF7;_T159=_T308.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T159;_T30E=_T312->f1;}_T15A=_T308.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T312=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T15A;_T309=_T312->f1;}{struct Cyc_Absyn_PtrInfo p2=_T30E;struct Cyc_Absyn_PtrInfo p1=_T309;_T15B=p1;_T15C=_T15B.ptr_atts;_T15D=_T15C.nullable;{
# 1246
int n1=Cyc_Tcutil_force_type2bool(0,_T15D);_T15E=p2;_T15F=_T15E.ptr_atts;_T160=_T15F.nullable;{
int n2=Cyc_Tcutil_force_type2bool(0,_T160);_T161=
Cyc_Absyn_bounds_one();_T162=p1;_T163=_T162.ptr_atts;_T164=_T163.bounds;{struct Cyc_Absyn_Exp*b1=Cyc_Tcutil_get_bounds_exp_constrain(_T161,_T164,1);_T165=
Cyc_Absyn_bounds_one();_T166=p2;_T167=_T166.ptr_atts;_T168=_T167.bounds;{struct Cyc_Absyn_Exp*b2=Cyc_Tcutil_get_bounds_exp_constrain(_T165,_T168,1);{struct _tuple23 _T312;
_T312.f0=b1;_T312.f1=b2;_T169=_T312;}{struct _tuple23 _T312=_T169;_T16A=_T312.f0;if(_T16A==0)goto _TLF9;_T16B=_T312.f1;if(_T16B==0)goto _TLFB;_T16C=env;_T16D=_T16C->pure_exp;if(_T16D)goto _TLFD;else{goto _TLFF;}_TLFF: _T16E=n1;
# 1255
if(!_T16E)goto _TL100;_T16F=n2;if(_T16F)goto _TL100;else{goto _TL102;}
_TL102: _T170=Cyc_AssnDef_zero();_T171=t;{void*null_check=Cyc_AssnDef_neq(_T170,_T171);
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));_T172=exp_checks;_T172->ctxt=am;_T173=exp_checks;_T174=& Cyc_AssnDef_true_assn;_T175=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T174;_T173->bounds_check=(void*)_T175;_T176=exp_checks;
_T176->null_check=null_check;_T177=env;_T178=_T177->assn_info;_T179=e1;_T17A=exp_checks;
Cyc_Vcgen_insert_exp_checks(_T178,_T179,_T17A);_T17B=env;_T17C=am;_T17D=t;_T17E=
Cyc_AssnDef_zero();_T17F=Cyc_AssnDef_eq(_T17D,_T17E);_T180=Cyc_AssnDef_and_assnmap_assn(_T17C,_T17F);_T181=Cyc_Absyn_exn_type();_T182=Cyc_AssnDef_fresh_var(0,_T181);Cyc_Vcgen_update_try_assnmap(_T17B,_T180,_T182);{struct _tuple14 _T313;
_T313.f0=t;_T313.f1=Cyc_AssnDef_and_assnmap_assn(am,null_check);_T183=_T313;}return _T183;}
_TL100:{struct _tuple14 _T313;_T313.f0=t;_T313.f1=am;_T184=_T313;}return _T184;_TLFD: goto _LLE6;_TLFB:{struct _tuple14 _T313;
# 1264
_T313.f0=t;_T313.f1=am;_T185=_T313;}return _T185;_TLF9: _T186=_T312.f1;if(_T186==0)goto _TL103;_T187=env;_T188=_T187->pure_exp;if(_T188)goto _TL105;else{goto _TL107;}_TL107: _T18A=env;_T18B=_T18A->pure_exp;
# 1268
if(_T18B)goto _TL10A;else{goto _TL10B;}_TL10B: _T18C=n2;if(_T18C)goto _TL10A;else{goto _TL108;}_TL10A: _T18D=& Cyc_AssnDef_true_assn;_T18E=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T18D;_T189=(void*)_T18E;goto _TL109;_TL108: _T18F=
Cyc_AssnDef_zero();_T190=t;_T189=Cyc_AssnDef_neq(_T18F,_T190);_TL109:{
# 1268
void*null_check=_T189;_T191=env;_T192=
# 1270
_check_null(b2);_T193=am;{struct _tuple14 _T313=Cyc_Vcgen_vcgen_rexp(_T191,_T192,_T193);void*_T314;_T314=_T313.f0;{void*b=_T314;_T194=p1;_T195=_T194.ptr_atts;_T196=_T195.zero_term;{
# 1279 "vcgen.cyc"
int zt1=Cyc_Tcutil_force_type2bool(0,_T196);_T197=p2;_T198=_T197.ptr_atts;_T199=_T198.zero_term;{
int zt2=Cyc_Tcutil_force_type2bool(0,_T199);_T19A=zt1;
if(!_T19A)goto _TL10C;_T19B=zt2;if(_T19B)goto _TL10C;else{goto _TL10E;}
_TL10E: _T19C=b;_T19D=Cyc_AssnDef_one();_T19E=Cyc_AssnDef_get_term_type(b);b=Cyc_AssnDef_plus(_T19C,_T19D,_T19E);goto _TL10D;_TL10C: _TL10D: _T19F=b;_T1A0=
# 1284
Cyc_AssnDef_numelts_term(t);{void*bcheck=Cyc_AssnDef_ulte(_T19F,_T1A0);
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));_T1A1=exp_checks;_T1A1->ctxt=am;_T1A2=exp_checks;_T1A2->bounds_check=bcheck;_T1A3=exp_checks;
_T1A3->null_check=null_check;_T1A4=env;_T1A5=_T1A4->assn_info;_T1A6=e1;_T1A7=exp_checks;
Cyc_Vcgen_insert_exp_checks(_T1A5,_T1A6,_T1A7);{
void*checks=Cyc_AssnDef_and(null_check,bcheck);_T1A8=env;_T1A9=am;_T1AA=
Cyc_AssnDef_not(checks);_T1AB=Cyc_AssnDef_and_assnmap_assn(_T1A9,_T1AA);_T1AC=Cyc_Absyn_exn_type();_T1AD=Cyc_AssnDef_fresh_var(0,_T1AC);Cyc_Vcgen_update_try_assnmap(_T1A8,_T1AB,_T1AD);{struct _tuple14 _T315;
_T315.f0=t;_T315.f1=Cyc_AssnDef_and_assnmap_assn(am,checks);_T1AE=_T315;}return _T1AE;}}}}}}}_TL105: goto _LLE6;_TL103: _LLE6:{struct _tuple14 _T313;
# 1292
_T313.f0=t;_T313.f1=am;_T1AF=_T313;}return _T1AF;;}}}}}}goto _TLF8;_TLF7: goto _LLDD;_TLF8:;default: _LLDD: goto _LLCA;}_TLDC: _LLCA:;}_T1B0=env;_T1B1=_T1B0->pure_exp;
# 1296
if(_T1B1)goto _TL111;else{goto _TL112;}_TL112: _T1B2=c;_T1B3=(int)_T1B2;if(_T1B3==1)goto _TL111;else{goto _TL10F;}
_TL111:{struct _tuple14 _T308;_T308.f0=Cyc_AssnDef_cast(tp,t);_T308.f1=am;_T1B4=_T308;}return _T1B4;
# 1300
_TL10F: _T1B5=env;_T1B6=amin;_T1B7=Cyc_Absyn_exn_type();_T1B8=Cyc_AssnDef_fresh_var(0,_T1B7);Cyc_Vcgen_update_try_assnmap(_T1B5,_T1B6,_T1B8);{struct _tuple14 _T308;
_T308.f0=t;_T308.f1=am;_T1B9=_T308;}return _T1B9;}}}case 16:{struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T300=_T305->f2;_T2FD=_T305->f3;}{struct Cyc_Absyn_Exp*eopt=_T302;struct Cyc_Absyn_Exp*e1=_T300;struct Cyc_Absyn_Exp*qopt=_T2FD;
# 1304
if(eopt==0)goto _TL113;_T1BA=
Cyc_Vcgen_vcgen_rexp(env,eopt,amin);amin=_T1BA.f1;goto _TL114;_TL113: _TL114:
 if(qopt==0)goto _TL115;_T1BB=
Cyc_Vcgen_vcgen_rexp(env,qopt,amin);amin=_T1BB.f1;goto _TL116;_TL115: _TL116: _T1BC=e1;{
# 1309
void*_T305=_T1BC->r;struct Cyc_Absyn_Exp*_T306;struct Cyc_Absyn_Exp*_T307;struct Cyc_Absyn_Vardecl*_T308;struct Cyc_List_List*_T309;_T1BD=(int*)_T305;_T1BE=*_T1BD;switch(_T1BE){case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T30A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T305;_T309=_T30A->f1;}{struct Cyc_List_List*dles=_T309;
# 1314
struct _tuple14 _T30A=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T30B;_T30B=_T30A.f1;{struct Cyc_AssnDef_AssnMap amin=_T30B;_T1BF=
Cyc_List_length(dles);_T1C0=(unsigned)_T1BF;{void*size=Cyc_AssnDef_uint(_T1C0);_T1C1=env;_T1C2=e;_T1C3=
Cyc_List_length(dles);_T1C4=(unsigned)_T1C3;_T1C5=Cyc_AssnDef_uint(_T1C4);_T1C6=e;_T1C7=_T1C6->topt;{void*res=Cyc_Vcgen_myalloc(_T1C1,_T1C2,_T1C5,_T1C7);_T1C8=amin;_T1C9=res;_T1CA=
Cyc_AssnDef_zero();_T1CB=Cyc_AssnDef_neq(_T1C9,_T1CA);_T1CC=
Cyc_AssnDef_numelts_term(res);_T1CD=size;_T1CE=Cyc_AssnDef_eq(_T1CC,_T1CD);_T1CF=
# 1317
Cyc_AssnDef_and(_T1CB,_T1CE);amin=Cyc_AssnDef_and_assnmap_assn(_T1C8,_T1CF);{struct _tuple14 _T30C;
# 1321
_T30C.f0=res;_T30C.f1=amin;_T1D0=_T30C;}return _T1D0;}}}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T30A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T305;_T308=_T30A->f1;_T307=_T30A->f2;_T306=_T30A->f3;}{struct Cyc_Absyn_Vardecl*vd=_T308;struct Cyc_Absyn_Exp*e1=_T307;struct Cyc_Absyn_Exp*e2=_T306;_T1D1=env;_T1D2=vd;_T1D3=e1;_T1D4=e2;_T1D5=e;_T1D6=_T1D5->topt;_T1D7=
# 1323
_check_null(_T1D6);_T1D8=amin;{struct _tuple14 _T30A=Cyc_Vcgen_vcgen_comprehension(_T1D1,_T1D2,_T1D3,_T1D4,_T1D7,_T1D8);struct Cyc_AssnDef_AssnMap _T30B;void*_T30C;_T30C=_T30A.f0;_T30B=_T30A.f1;{void*res=_T30C;struct Cyc_AssnDef_AssnMap amin=_T30B;_T1D9=amin;_T1DA=res;_T1DB=
Cyc_AssnDef_zero();_T1DC=Cyc_AssnDef_neq(_T1DA,_T1DB);amin=Cyc_AssnDef_and_assnmap_assn(_T1D9,_T1DC);{struct _tuple14 _T30D;
_T30D.f0=res;_T30D.f1=amin;_T1DD=_T30D;}return _T1DD;}}}default:  {
# 1327
struct _tuple14 _T30A=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T30B;void*_T30C;_T30C=_T30A.f0;_T30B=_T30A.f1;{void*t=_T30C;struct Cyc_AssnDef_AssnMap amin=_T30B;_T1DE=env;_T1DF=e;_T1E0=
Cyc_AssnDef_one();_T1E1=e;_T1E2=_T1E1->topt;{void*res=Cyc_Vcgen_myalloc(_T1DE,_T1DF,_T1E0,_T1E2);
struct _tuple14 _T30D=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T30E;void*_T30F;_T30F=_T30D.f0;_T30E=_T30D.f1;{void*mem=_T30F;struct Cyc_AssnDef_AssnMap amin=_T30E;_T1E3=amin;_T1E4=mem;_T1E5=res;_T1E6=e1;_T1E7=_T1E6->topt;_T1E8=
Cyc_AssnDef_select(_T1E4,_T1E5,_T1E7);_T1E9=t;_T1EA=Cyc_AssnDef_eq(_T1E8,_T1E9);_T1EB=res;_T1EC=
Cyc_AssnDef_zero();_T1ED=Cyc_AssnDef_neq(_T1EB,_T1EC);_T1EE=
# 1330
Cyc_AssnDef_and(_T1EA,_T1ED);amin=Cyc_AssnDef_and_assnmap_assn(_T1E3,_T1EE);{struct _tuple14 _T310;
# 1332
_T310.f0=res;_T310.f1=amin;_T1EF=_T310;}return _T1EF;}}}}};}}case 31: goto _LL28;case 32: _LL28: goto _LL2A;case 17: _LL2A: goto _LL2C;case 18: _LL2C: goto _LL2E;case 38: _LL2E: goto _LL30;case 19: _LL30: {
# 1340
struct _tuple16 _T305=Cyc_Evexp_eval_const_uint_exp(e);int _T306;unsigned _T307;_T307=_T305.f0;_T306=_T305.f1;{unsigned cn=_T307;int known=_T306;_T1F0=known;
if(!_T1F0)goto _TL118;{struct _tuple14 _T308;_T308.f0=Cyc_AssnDef_uint(cn);_T308.f1=amin;_T1F1=_T308;}return _T1F1;_TL118:{struct _tuple14 _T308;
_T308.f0=Cyc_AssnDef_cnst(e);_T308.f1=amin;_T1F2=_T308;}return _T1F2;}}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T2FA=_T305->f2;_T2F9=_T305->f3;_T2F8=_T305->f4;}{struct Cyc_Absyn_Exp*e1=_T302;struct _fat_ptr*f=_T2FA;int is_tagged=_T2F9;int is_read=_T2F8;
# 1344
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t=_T307;struct Cyc_AssnDef_AssnMap amin=_T306;_T1F3=e1;_T1F4=_T1F3->topt;_T1F5=
_check_null(_T1F4);_T1F6=f;{int i=Cyc_CfFlowInfo_get_field_index(_T1F5,_T1F6);_T1F7=env;_T1F8=_T1F7->pure_exp;
# 1347
if(_T1F8)goto _TL11A;else{goto _TL11C;}_TL11C: _T1F9=is_tagged;if(!_T1F9)goto _TL11A;_T1FA=is_read;if(!_T1FA)goto _TL11A;_T1FB=
# 1349
Cyc_AssnDef_tagof_tm(t);_T1FC=i;_T1FD=(unsigned)_T1FC;_T1FE=Cyc_AssnDef_uint(_T1FD);{void*ck=Cyc_AssnDef_eq(_T1FB,_T1FE);_T1FF=env;_T200=amin;_T201=
Cyc_AssnDef_not(ck);_T202=Cyc_AssnDef_and_assnmap_assn(_T200,_T201);_T203=Cyc_Absyn_exn_type();_T204=Cyc_AssnDef_fresh_var(0,_T203);Cyc_Vcgen_update_try_assnmap(_T1FF,_T202,_T204);
amin=Cyc_AssnDef_and_assnmap_assn(amin,ck);}goto _TL11B;_TL11A: _TL11B:{struct _tuple14 _T308;_T206=t;_T207=i;_T208=(unsigned)_T207;_T209=e;_T20A=_T209->topt;
# 1353
_T308.f0=Cyc_AssnDef_proj(_T206,_T208,_T20A);_T308.f1=amin;_T205=_T308;}return _T205;}}}case 37:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T2FA=_T305->f2;}{struct Cyc_Absyn_Exp*e1=_T302;struct _fat_ptr*f=_T2FA;_T20B=e1;_T20C=_T20B->topt;_T20D=
# 1356
_check_null(_T20C);_T20E=f;{int i=Cyc_CfFlowInfo_get_field_index(_T20D,_T20E);
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t=_T307;struct Cyc_AssnDef_AssnMap am=_T306;_T20F=env;_T210=Cyc_Absyn_sint_type;_T211=
Cyc_AssnDef_tagof_tm(t);_T212=i;_T213=(unsigned)_T212;_T214=Cyc_AssnDef_uint(_T213);_T215=am;_T216=Cyc_Vcgen_vcgen_binop(_T20F,_T210,5U,_T211,_T214,_T215);return _T216;}}}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;}{struct Cyc_Absyn_Exp*e1=_T302;_T217=
# 1360
Cyc_Vcgen_vcgen_lexp(env,e1,amin);return _T217;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;}{struct Cyc_Absyn_Exp*e1=_T302;_T218=env;_T219=e1;_T21A=
# 1362
Cyc_Vcgen_zero_exp();_T21B=amin;{struct _tuple22 _T305=Cyc_Vcgen_vcgen_deref(_T218,_T219,_T21A,_T21B);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f2;{void*t1=_T307;struct Cyc_AssnDef_AssnMap amin=_T306;
struct _tuple14 _T308=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*mem=_T30A;struct Cyc_AssnDef_AssnMap amin=_T309;{struct _tuple14 _T30B;_T21D=mem;_T21E=t1;_T21F=e;_T220=_T21F->topt;
_T30B.f0=Cyc_AssnDef_select(_T21D,_T21E,_T220);_T30B.f1=amin;_T21C=_T30B;}return _T21C;}}}}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T2FA=_T305->f2;_T2F9=_T305->f3;_T2F8=_T305->f4;}{struct Cyc_Absyn_Exp*e1=_T302;struct _fat_ptr*f=_T2FA;int is_tagged=_T2F9;int is_read=_T2F8;_T221=env;_T222=e1;_T223=
# 1366
Cyc_Vcgen_zero_exp();_T224=amin;{struct _tuple22 _T305=Cyc_Vcgen_vcgen_deref(_T221,_T222,_T223,_T224);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f2;{void*t1=_T307;struct Cyc_AssnDef_AssnMap amin=_T306;_T225=e1;_T226=_T225->topt;_T227=
_check_null(_T226);_T228=Cyc_Tcutil_pointer_elt_type(_T227);_T229=f;{int i=Cyc_CfFlowInfo_get_field_index(_T228,_T229);
struct _tuple14 _T308=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*mem=_T30A;struct Cyc_AssnDef_AssnMap amin=_T309;_T22A=env;_T22B=_T22A->pure_exp;
if(_T22B)goto _TL11D;else{goto _TL11F;}_TL11F: _T22C=is_tagged;if(!_T22C)goto _TL11D;_T22D=is_read;if(!_T22D)goto _TL11D;_T22E=
Cyc_AssnDef_select(mem,t1,0);_T22F=Cyc_AssnDef_tagof_tm(_T22E);_T230=i;_T231=(unsigned)_T230;_T232=
Cyc_AssnDef_uint(_T231);{
# 1370
void*ck=Cyc_AssnDef_eq(_T22F,_T232);_T233=env;_T234=amin;_T235=
# 1372
Cyc_AssnDef_not(ck);_T236=Cyc_AssnDef_and_assnmap_assn(_T234,_T235);_T237=Cyc_Absyn_exn_type();_T238=Cyc_AssnDef_fresh_var(0,_T237);Cyc_Vcgen_update_try_assnmap(_T233,_T236,_T238);
amin=Cyc_AssnDef_and_assnmap_assn(amin,ck);}goto _TL11E;_TL11D: _TL11E: _T239=e1;_T23A=_T239->topt;_T23B=
# 1375
_check_null(_T23A);{void*elt_type=Cyc_Tcutil_pointer_elt_type(_T23B);{struct _tuple14 _T30B;_T23D=
Cyc_AssnDef_select(mem,t1,elt_type);_T23E=i;_T23F=(unsigned)_T23E;_T240=e;_T241=_T240->topt;_T30B.f0=Cyc_AssnDef_proj(_T23D,_T23F,_T241);_T30B.f1=amin;_T23C=_T30B;}return _T23C;}}}}}}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T300=_T305->f2;}{struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_Absyn_Exp*e2=_T300;
# 1378
struct _tuple22 _T305=Cyc_Vcgen_vcgen_deref(env,e1,e2,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;void*_T308;_T308=_T305.f0;_T307=_T305.f1;_T306=_T305.f2;{void*t1=_T308;void*t2=_T307;struct Cyc_AssnDef_AssnMap amin=_T306;
struct _tuple14 _T309=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T30A;void*_T30B;_T30B=_T309.f0;_T30A=_T309.f1;{void*mem=_T30B;struct Cyc_AssnDef_AssnMap amin=_T30A;{struct _tuple14 _T30C;_T243=mem;_T244=
Cyc_AssnDef_plus(t1,t2,0);_T245=e;_T246=_T245->topt;_T30C.f0=Cyc_AssnDef_select(_T243,_T244,_T246);_T30C.f1=amin;_T242=_T30C;}return _T242;}}}case 30:{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T2F7=_T305->f2;_T2F6=_T305->f3;}{struct Cyc_List_List*es=_T302;struct Cyc_Absyn_Datatypedecl*dd=_T2F7;struct Cyc_Absyn_Datatypefield*df=_T2F6;
# 1382
struct _tuple17 _T305=Cyc_Vcgen_vcgen_rexps(env,es,amin);struct Cyc_AssnDef_AssnMap _T306;struct Cyc_List_List*_T307;_T307=_T305.f0;_T306=_T305.f1;{struct Cyc_List_List*ts=_T307;struct Cyc_AssnDef_AssnMap amin=_T306;
unsigned i=Cyc_Vcgen_datatype_constructor_index(dd,df);{struct _tuple14 _T308;_T248=i;_T249=ts;_T24A=e;_T24B=_T24A->topt;
_T308.f0=Cyc_AssnDef_datatype_aggr(_T248,_T249,_T24B);_T308.f1=amin;_T247=_T308;}return _T247;}}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f2;}{struct Cyc_List_List*dles=_T302;_T302=dles;goto _LL42;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;}_LL42: {struct Cyc_List_List*dles=_T302;_T302=dles;goto _LL44;}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f2;}_LL44: {struct Cyc_List_List*dles=_T302;
# 1390
_TL123: if(dles!=0)goto _TL121;else{goto _TL122;}
_TL121: _T24C=dles;_T24D=_T24C->hd;{struct _tuple24*_T305=(struct _tuple24*)_T24D;struct Cyc_Absyn_Exp*_T306;{struct _tuple24 _T307=*_T305;_T306=_T307.f1;}{struct Cyc_Absyn_Exp*e=_T306;_T24E=
Cyc_Vcgen_vcgen_rexp(env,e,amin);amin=_T24E.f1;}}_T24F=dles;
# 1390
dles=_T24F->tl;goto _TL123;_TL122:{struct _tuple14 _T305;
# 1394
_T305.f0=Cyc_AssnDef_fresh_var(0,0);_T305.f1=amin;_T250=_T305;}return _T250;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f3;}{struct Cyc_List_List*dles=_T302;_T302=dles;goto _LL48;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f3;}_LL48: {struct Cyc_List_List*dles=_T302;_T251=e;_T252=_T251->topt;{
# 1397
void*atype=_check_null(_T252);
struct _tuple20 _T305=Cyc_Vcgen_get_aggr_info(atype);enum Cyc_Absyn_AggrKind _T306;int _T307;_T307=_T305.f0;_T306=_T305.f1;{int is_tagged=_T307;enum Cyc_Absyn_AggrKind ak=_T306;
struct Cyc_List_List*dlvs=0;
unsigned tag=0U;
_TL127: if(dles!=0)goto _TL125;else{goto _TL126;}
_TL125: _T253=dles;_T254=_T253->hd;{struct _tuple24*_T308=(struct _tuple24*)_T254;struct Cyc_Absyn_Exp*_T309;struct Cyc_List_List*_T30A;{struct _tuple24 _T30B=*_T308;_T30A=_T30B.f0;_T309=_T30B.f1;}{struct Cyc_List_List*dl=_T30A;struct Cyc_Absyn_Exp*e=_T309;
struct _tuple14 _T30B=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*v=_T30D;struct Cyc_AssnDef_AssnMap a=_T30C;
amin=a;{struct _fat_ptr*_T30E;if(dl==0)goto _TL128;_T255=dl;_T256=(struct Cyc_List_List*)_T255;_T257=_T256->hd;_T258=(int*)_T257;_T259=*_T258;if(_T259!=1)goto _TL12A;_T25A=dl;_T25B=(struct Cyc_List_List*)_T25A;_T25C=_T25B->tl;if(_T25C!=0)goto _TL12C;_T25D=dl;{struct Cyc_List_List _T30F=*_T25D;_T25E=_T30F.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T310=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T25E;_T30E=_T310->f1;}}{struct _fat_ptr*f=_T30E;
# 1407
int i=Cyc_CfFlowInfo_get_field_index(atype,f);_T25F=i;
tag=(unsigned)_T25F;{struct Cyc_List_List*_T30F=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple21*_T310=_cycalloc(sizeof(struct _tuple21));
_T310->f0=i;_T310->f1=v;_T261=(struct _tuple21*)_T310;}_T30F->hd=_T261;_T30F->tl=dlvs;_T260=(struct Cyc_List_List*)_T30F;}dlvs=_T260;goto _LL128;}_TL12C: goto _LL12B;_TL12A: goto _LL12B;_TL128: _LL12B: _T263=Cyc_Warn_impos;{
# 1411
int(*_T30F)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T263;_T262=_T30F;}_T264=_tag_fat("no field name in designator!",sizeof(char),29U);_T265=_tag_fat(0U,sizeof(void*),0);_T262(_T264,_T265);_LL128:;}}}}_T266=dles;
# 1401
dles=_T266->tl;goto _TL127;_TL126: _T268=Cyc_List_rimp_merge_sort;{
# 1414
struct Cyc_List_List*(*_T308)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*))_T268;_T267=_T308;}_T269=dlvs;dlvs=_T267(Cyc_Vcgen_cmp_index,_T269);_T26B=Cyc_List_map;{
struct Cyc_List_List*(*_T308)(void*(*)(struct _tuple21*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple21*),struct Cyc_List_List*))_T26B;_T26A=_T308;}_T26D=Cyc_Core_snd;{void*(*_T308)(struct _tuple21*)=(void*(*)(struct _tuple21*))_T26D;_T26C=_T308;}_T26E=dlvs;{struct Cyc_List_List*vs=_T26A(_T26C,_T26E);_T270=ak;_T271=(int)_T270;
if(_T271!=1)goto _TL12E;_T272=tag;_T273=vs;_T274=e;_T275=_T274->topt;_T26F=Cyc_AssnDef_union_aggr(_T272,_T273,_T275);goto _TL12F;_TL12E: _T276=vs;_T277=e;_T278=_T277->topt;_T26F=
Cyc_AssnDef_struct_aggr(_T276,_T278);_TL12F: {
# 1416
void*res=_T26F;{struct _tuple14 _T308;
# 1418
_T308.f0=res;_T308.f1=amin;_T279=_T308;}return _T279;}}}}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T300=_T305->f2;_T2FD=_T305->f3;}{struct Cyc_Absyn_Vardecl*vd=_T302;struct Cyc_Absyn_Exp*e1=_T300;struct Cyc_Absyn_Exp*e2=_T2FD;_T27A=env;_T27B=vd;_T27C=e1;_T27D=e2;_T27E=e;_T27F=_T27E->topt;_T280=
# 1420
_check_null(_T27F);_T281=amin;_T282=Cyc_Vcgen_vcgen_comprehension(_T27A,_T27B,_T27C,_T27D,_T280,_T281);return _T282;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;}{struct Cyc_Absyn_Exp*e=_T302;{struct _tuple14 _T305;
# 1422
_T305.f0=Cyc_AssnDef_fresh_var(0,0);_T284=Cyc_Vcgen_vcgen_rexp(env,e,amin);_T305.f1=_T284.f1;_T283=_T305;}return _T283;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T2F4;_T2F5=_T305->f1;}{struct Cyc_Absyn_MallocInfo mi=_T2F5;_T285=mi;_T286=_T285.rgn;
# 1424
if(_T286==0)goto _TL130;_T287=env;_T288=mi;_T289=_T288.rgn;_T28A=amin;_T28B=
Cyc_Vcgen_vcgen_rexp(_T287,_T289,_T28A);amin=_T28B.f1;goto _TL131;_TL130: _TL131: _T28C=env;_T28D=mi;_T28E=_T28D.num_elts;_T28F=amin;{
struct _tuple14 _T305=Cyc_Vcgen_vcgen_rexp(_T28C,_T28E,_T28F);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*t=_T307;struct Cyc_AssnDef_AssnMap am1=_T306;_T290=env;_T291=e;_T292=t;_T293=e;_T294=_T293->topt;{
void*res=Cyc_Vcgen_myalloc(_T290,_T291,_T292,_T294);_T295=am1;_T296=res;_T297=
Cyc_AssnDef_zero();_T298=Cyc_AssnDef_neq(_T296,_T297);_T299=Cyc_AssnDef_numelts_term(res);_T29A=t;_T29B=Cyc_AssnDef_eq(_T299,_T29A);_T29C=Cyc_AssnDef_and(_T298,_T29B);{struct Cyc_AssnDef_AssnMap am2=Cyc_AssnDef_and_assnmap_assn(_T295,_T29C);{struct _tuple14 _T308;
_T308.f0=res;_T308.f1=am2;_T29D=_T308;}return _T29D;}}}}}case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;_T300=_T305->f2;}{struct Cyc_Absyn_Exp*e1=_T302;struct Cyc_Absyn_Exp*e2=_T300;
# 1432
struct _tuple14 _T305=Cyc_Vcgen_vcgen_lexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;void*_T307;_T307=_T305.f0;_T306=_T305.f1;{void*lt1=_T307;struct Cyc_AssnDef_AssnMap am1=_T306;
struct _tuple14 _T308=Cyc_Vcgen_vcgen_lexp(env,e2,am1);struct Cyc_AssnDef_AssnMap _T309;void*_T30A;_T30A=_T308.f0;_T309=_T308.f1;{void*lt2=_T30A;struct Cyc_AssnDef_AssnMap am2=_T309;
struct _tuple14 _T30B=Cyc_Vcgen_deref_lterm(env,lt1,am2);struct Cyc_AssnDef_AssnMap _T30C;void*_T30D;_T30D=_T30B.f0;_T30C=_T30B.f1;{void*t1=_T30D;struct Cyc_AssnDef_AssnMap am3=_T30C;
struct _tuple14 _T30E=Cyc_Vcgen_deref_lterm(env,lt2,am3);struct Cyc_AssnDef_AssnMap _T30F;void*_T310;_T310=_T30E.f0;_T30F=_T30E.f1;{void*t2=_T310;struct Cyc_AssnDef_AssnMap am4=_T30F;
struct Cyc_AssnDef_AssnMap am5=Cyc_Vcgen_do_assign(env,am4,lt1,t2);
struct Cyc_AssnDef_AssnMap am6=Cyc_Vcgen_do_assign(env,am5,lt2,t1);{struct _tuple14 _T311;
_T311.f0=Cyc_AssnDef_fresh_var(0,0);_T311.f1=am6;_T29E=_T311;}return _T29E;}}}}}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T2F4;_T302=_T305->f1;}{struct Cyc_Absyn_Stmt*s=_T302;_T29F=env;{
# 1440
void**old_exp_stmt=_T29F->exp_stmt;_T2A0=env;{void**_T305=_cycalloc(sizeof(void*));
*_T305=Cyc_AssnDef_fresh_var(0,0);_T2A1=(void**)_T305;}_T2A0->exp_stmt=_T2A1;{
struct Cyc_AssnDef_AssnMap amout=Cyc_Vcgen_vcgen_stmt(env,s,amin);_T2A2=env;_T2A3=_T2A2->exp_stmt;_T2A4=
_check_null(_T2A3);{void*t=*_T2A4;_T2A5=env;
_T2A5->exp_stmt=old_exp_stmt;{struct _tuple14 _T305;
_T305.f0=t;_T305.f1=amout;_T2A6=_T305;}return _T2A6;}}}}case 39:{struct _tuple14 _T305;
_T305.f0=Cyc_AssnDef_fresh_var(0,0);_T305.f1=amin;_T2A7=_T305;}return _T2A7;case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T2F4;_T300=_T305->f1;}{struct Cyc_Absyn_Exp*e=_T300;_T2A8=
Cyc_Vcgen_vcgen_rexp(env,e,amin);return _T2A8;}case 41:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T2F4;_T300=_T305->f1;_T2F9=_T305->f2;}_T2A9=(int)_T2F9;if(!_T2A9)goto _TL132;{struct Cyc_Absyn_Exp*e1=_T300;int static_only=_T2F9;_T2AA=env;{
# 1449
int old_widen_paths=_T2AA->widen_paths;_T2AB=env;{
int old_pure_exp=_T2AB->pure_exp;_T2AC=env;
_T2AC->widen_paths=0;_T2AD=env;
_T2AD->pure_exp=1;_T2AE=env;_T2AF=e1;{struct Cyc_AssnDef_AssnMap _T305;_T2B1=& Cyc_AssnDef_true_assn;_T2B2=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T2B1;
_T305.assn=(void*)_T2B2;_T2B3=amin;_T305.map=_T2B3.map;_T2B0=_T305;}{struct _tuple18 _T305=Cyc_Vcgen_vcgen_test(_T2AE,_T2AF,_T2B0);struct Cyc_AssnDef_AssnMap _T306;_T306=_T305.f0;{struct Cyc_AssnDef_AssnMap amt=_T306;_T2B4=env;
_T2B4->widen_paths=old_widen_paths;_T2B5=env;
_T2B5->pure_exp=old_pure_exp;_T2B6=amin;_T2B7=_T2B6.assn;_T2B8=amt;_T2B9=_T2B8.assn;_T2BA=
Cyc_Vcgen_implies(_T2B7,_T2B9);if(_T2BA)goto _TL134;else{goto _TL136;}
_TL136:{struct Cyc_String_pa_PrintArg_struct _T307;_T307.tag=0;_T2BC=amt;_T2BD=_T2BC.assn;
_T307.f1=Cyc_AssnDef_assn2string(_T2BD);_T2BB=_T307;}{struct Cyc_String_pa_PrintArg_struct _T307=_T2BB;{struct Cyc_String_pa_PrintArg_struct _T308;_T308.tag=0;_T308.f1=Cyc_AssnDef_assnmap2string(amin);_T2BE=_T308;}{struct Cyc_String_pa_PrintArg_struct _T308=_T2BE;void*_T309[2];_T309[0]=& _T307;_T309[1]=& _T308;_T2BF=e;_T2C0=_T2BF->loc;_T2C1=
# 1457
_tag_fat("cannot prove @assert(%s)\nfrom\n %s",sizeof(char),34U);_T2C2=_tag_fat(_T309,sizeof(void*),2);Cyc_Warn_err(_T2C0,_T2C1,_T2C2);}}goto _TL135;_TL134: _TL135:{struct _tuple14 _T307;
# 1459
_T307.f0=Cyc_AssnDef_one();_T2C4=amin;_T2C5=amt;_T2C6=_T2C5.assn;_T307.f1=Cyc_AssnDef_and_assnmap_assn(_T2C4,_T2C6);_T2C3=_T307;}return _T2C3;}}}}}_TL132:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T2F4;_T300=_T305->f1;_T2C7=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T2F4;_T2C8=& _T2C7->f3;_T302=(int*)_T2C8;}{struct Cyc_Absyn_Exp*e1=_T300;int*do_check=(int*)_T302;
# 1463
struct _tuple18 _T305=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T306;struct Cyc_AssnDef_AssnMap _T307;_T307=_T305.f0;_T306=_T305.f1;{struct Cyc_AssnDef_AssnMap amt=_T307;struct Cyc_AssnDef_AssnMap amf=_T306;_T2C9=amf;_T2CA=_T2C9.assn;_T2CB=& Cyc_AssnDef_false_assn;_T2CC=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T2CB;_T2CD=(void*)_T2CC;_T2CE=
# 1465
Cyc_Vcgen_implies(_T2CA,_T2CD);if(!_T2CE)goto _TL137;_T2CF=
Cyc_Vcgen_is_pure_exp(e1);if(!_T2CF)goto _TL139;_T2D0=do_check;
*_T2D0=0;goto _TL13A;
# 1469
_TL139: _T2D1=do_check;*_T2D1=1;_TL13A: goto _TL138;
# 1471
_TL137: _T2D2=do_check;*_T2D2=1;_T2D3=Cyc_Flags_warn_assert;
if(!_T2D3)goto _TL13B;{struct Cyc_Warn_String_Warn_Warg_struct _T308;_T308.tag=0;
_T308.f1=_tag_fat("assert(",sizeof(char),8U);_T2D4=_T308;}{struct Cyc_Warn_String_Warn_Warg_struct _T308=_T2D4;{struct Cyc_Warn_Exp_Warn_Warg_struct _T309;_T309.tag=4;_T309.f1=e1;_T2D5=_T309;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T309=_T2D5;{struct Cyc_Warn_String_Warn_Warg_struct _T30A;_T30A.tag=0;_T30A.f1=_tag_fat(") might fail at run-time",sizeof(char),25U);_T2D6=_T30A;}{struct Cyc_Warn_String_Warn_Warg_struct _T30A=_T2D6;void*_T30B[3];_T30B[0]=& _T308;_T30B[1]=& _T309;_T30B[2]=& _T30A;_T2D7=e;_T2D8=_T2D7->loc;_T2D9=_tag_fat(_T30B,sizeof(void*),3);Cyc_Warn_warn2(_T2D8,_T2D9);}}}goto _TL13C;_TL13B: _TL13C: _T2DA=
Cyc_Absyn_exn_type();{void*v=Cyc_AssnDef_fresh_var(0,_T2DA);
Cyc_Vcgen_update_try_assnmap(env,amf,v);}_TL138:{struct _tuple14 _T308;
# 1477
_T308.f0=Cyc_AssnDef_one();_T308.f1=amt;_T2DB=_T308;}return _T2DB;}}default:{struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*_T305=(struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_T2F4;_T300=_T305->f1;}{struct Cyc_Absyn_Exp*e1=_T300;_T2DC=env;{
# 1479
int old_widen_paths=_T2DC->widen_paths;_T2DD=env;{
int old_pure_exp=_T2DD->pure_exp;_T2DE=env;
_T2DE->widen_paths=0;_T2DF=env;
_T2DF->pure_exp=1;_T2E0=env;_T2E1=e1;{struct Cyc_AssnDef_AssnMap _T305;_T2E3=& Cyc_AssnDef_true_assn;_T2E4=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T2E3;
_T305.assn=(void*)_T2E4;_T2E5=amin;_T305.map=_T2E5.map;_T2E2=_T305;}{struct _tuple18 _T305=Cyc_Vcgen_vcgen_test(_T2E0,_T2E1,_T2E2);struct Cyc_AssnDef_AssnMap _T306;_T306=_T305.f0;{struct Cyc_AssnDef_AssnMap amt=_T306;_T2E6=env;
_T2E6->widen_paths=old_widen_paths;_T2E7=env;
_T2E7->pure_exp=old_pure_exp;_T2E8=amin;_T2E9=_T2E8.assn;_T2EA=amt;_T2EB=_T2EA.assn;_T2EC=
Cyc_Vcgen_implies(_T2E9,_T2EB);if(!_T2EC)goto _TL13D;{struct Cyc_String_pa_PrintArg_struct _T307;_T307.tag=0;
# 1488
_T307.f1=Cyc_AssnDef_assnmap2string(amt);_T2ED=_T307;}{struct Cyc_String_pa_PrintArg_struct _T307=_T2ED;{struct Cyc_String_pa_PrintArg_struct _T308;_T308.tag=0;_T308.f1=Cyc_AssnDef_assnmap2string(amin);_T2EE=_T308;}{struct Cyc_String_pa_PrintArg_struct _T308=_T2EE;void*_T309[2];_T309[0]=& _T307;_T309[1]=& _T308;_T2EF=e;_T2F0=_T2EF->loc;_T2F1=
# 1487
_tag_fat("proved @assert_false(%s)\nfrom\n %s",sizeof(char),34U);_T2F2=_tag_fat(_T309,sizeof(void*),2);Cyc_Warn_err(_T2F0,_T2F1,_T2F2);}}goto _TL13E;_TL13D: _TL13E:{struct _tuple14 _T307;
# 1489
_T307.f0=Cyc_AssnDef_one();_T307.f1=amin;_T2F3=_T307;}return _T2F3;}}}}}};}}
# 1494
static struct _tuple14 Cyc_Vcgen_vcgen_rexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;int*_T3;int _T4;struct Cyc_List_List _T5;struct Cyc_AssnDef_ExistAssnFn*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_AssnDef_AssnMap _T9;void*_TA;void*_TB;struct _tuple14 _TC;
struct _tuple14 _TD=Cyc_Vcgen_vcgen_rexp_inner(env,e,amin);struct Cyc_AssnDef_AssnMap _TE;void*_TF;_TF=_TD.f0;_TE=_TD.f1;{void*t=_TF;struct Cyc_AssnDef_AssnMap am=_TE;_T0=e;_T1=_T0->topt;_T2=
_check_null(_T1);{void*_T10=Cyc_Absyn_compress(_T2);struct Cyc_AssnDef_ExistAssnFn*_T11;_T3=(int*)_T10;_T4=*_T3;if(_T4!=12)goto _TL13F;{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T12=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T10;_T11=_T12->f3;}{struct Cyc_AssnDef_ExistAssnFn*af=_T11;{struct Cyc_List_List _T12;
# 1498
_T12.hd=t;_T12.tl=0;_T5=_T12;}{struct Cyc_List_List terms=_T5;_T6=
_check_null(af);_T7=& terms;_T8=(struct Cyc_List_List*)_T7;{void*assn=Cyc_AssnDef_existassnfn2assn(_T6,_T8);_T9=am;_TA=_T9.assn;_TB=assn;
am.assn=Cyc_AssnDef_and(_TA,_TB);goto _LL3;}}}_TL13F: goto _LL3;_LL3:;}{struct _tuple14 _T10;
# 1504
_T10.f0=t;_T10.f1=am;_TC=_T10;}return _TC;}}
# 1507
static struct _tuple17 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Vcgen_Env*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_AssnDef_AssnMap _T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct _tuple17 _T7;
struct Cyc_List_List*result=0;
_TL144: if(es!=0)goto _TL142;else{goto _TL143;}
_TL142: _T0=env;_T1=es;_T2=_T1->hd;_T3=(struct Cyc_Absyn_Exp*)_T2;_T4=amin;{struct _tuple14 _T8=Cyc_Vcgen_vcgen_rexp(_T0,_T3,_T4);struct Cyc_AssnDef_AssnMap _T9;void*_TA;_TA=_T8.f0;_T9=_T8.f1;{void*t=_TA;struct Cyc_AssnDef_AssnMap am=_T9;{struct Cyc_List_List*_TB=_cycalloc(sizeof(struct Cyc_List_List));
_TB->hd=t;_TB->tl=result;_T5=(struct Cyc_List_List*)_TB;}result=_T5;
amin=am;}}_T6=es;
# 1509
es=_T6->tl;goto _TL144;_TL143:{struct _tuple17 _T8;
# 1514
_T8.f0=Cyc_List_imp_rev(result);_T8.f1=amin;_T7=_T8;}return _T7;}
# 1520
static struct _tuple14 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;struct Cyc_Absyn_Vardecl*_T9;void*_TA;int*_TB;int _TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;void*_TF;void*_T10;struct _tuple14 _T11;void*_T12;void*_T13;void*_T14;struct _tuple14 _T15;void*_T16;void*_T17;struct _tuple14 _T18;struct Cyc_Vcgen_Env*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_AssnDef_AssnMap _T1C;struct _tuple14 _T1D;struct _tuple14 _T1E;void*_T1F;void*_T20;struct Cyc_Absyn_Exp*_T21;void*_T22;struct Cyc_Vcgen_Env*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_Absyn_Exp*_T25;struct Cyc_AssnDef_AssnMap _T26;struct Cyc_Absyn_Exp*_T27;void*_T28;void*_T29;void*_T2A;struct _fat_ptr*_T2B;struct Cyc_Absyn_Exp*_T2C;void*_T2D;void*_T2E;void*_T2F;void*_T30;struct Cyc_Absyn_Tqual _T31;void*_T32;void*_T33;struct _tuple14 _T34;void*_T35;int _T36;unsigned _T37;void*_T38;struct Cyc_Absyn_Exp*_T39;void*_T3A;void*_T3B;struct _fat_ptr*_T3C;struct Cyc_Absyn_Exp*_T3D;void*_T3E;void*_T3F;void*_T40;void*_T41;struct Cyc_Absyn_Tqual _T42;void*_T43;void*_T44;struct _tuple14 _T45;void*_T46;int _T47;unsigned _T48;void*_T49;struct _tuple14 _T4A;struct Cyc_Absyn_Exp*_T4B;void*_T4C;void*_T4D;void*_T4E;void*_T4F;struct Cyc_Absyn_Tqual _T50;void*_T51;void*_T52;struct _tuple14 _T53;_T0=e;{
void*_T54=_T0->r;struct _fat_ptr*_T55;struct Cyc_Absyn_Exp*_T56;struct Cyc_Absyn_Exp*_T57;struct Cyc_Absyn_Vardecl*_T58;_T1=(int*)_T54;_T2=*_T1;switch(_T2){case 1: _T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T7=_T59->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T5A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T7;_T58=_T5A->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T58;_T58=vd;goto _LL4;}case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T8=_T59->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T5A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T8;_T58=_T5A->f1;}}_LL4: {struct Cyc_Absyn_Vardecl*vd=_T58;_T9=vd;_TA=_T9->type;{
# 1525
void*_T59=Cyc_Absyn_compress(_TA);struct Cyc_Absyn_ArrayInfo _T5A;_TB=(int*)_T59;_TC=*_TB;if(_TC!=5)goto _TL147;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T5B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T59;_T5A=_T5B->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T5A;_TD=vd;_TE=_TD->type;_TF=Cyc_Absyn_heap_rgn_type;_T10=Cyc_Absyn_al_qual_type;{
# 1528
void*p=Cyc_Tcutil_promote_array(_TE,_TF,_T10,0);{struct _tuple14 _T5B;_T12=
# 1530
Cyc_Vcgen_myaddr(env,vd);_T13=Cyc_AssnDef_zero();_T14=p;_T5B.f0=Cyc_AssnDef_offseti(_T12,_T13,_T14);_T5B.f1=amin;_T11=_T5B;}return _T11;}}_TL147:{struct _tuple14 _T5B;
_T5B.f0=Cyc_Vcgen_myaddr(env,vd);_T5B.f1=amin;_T15=_T5B;}return _T15;;}}case 3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T16=_T59->f1;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T5A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T16;_T58=_T5A->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T58;_T58=vd;goto _LL8;}case 5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T17=_T59->f1;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T5A=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T17;_T58=_T5A->f1;}}_LL8: {struct Cyc_Absyn_Vardecl*vd=_T58;{struct _tuple14 _T59;
# 1537
_T59.f0=Cyc_Vcgen_myaddr(env,vd);_T59.f1=amin;_T18=_T59;}return _T18;}default: goto _LL15;};case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;}{struct Cyc_Absyn_Exp*e1=_T57;_T19=env;_T1A=e1;_T1B=
# 1542
Cyc_Vcgen_zero_exp();_T1C=amin;{struct _tuple22 _T59=Cyc_Vcgen_vcgen_deref(_T19,_T1A,_T1B,_T1C);struct Cyc_AssnDef_AssnMap _T5A;void*_T5B;void*_T5C;_T5C=_T59.f0;_T5B=_T59.f1;_T5A=_T59.f2;{void*t1=_T5C;void*t2=_T5B;struct Cyc_AssnDef_AssnMap am1=_T5A;{struct _tuple14 _T5D;
_T5D.f0=t1;_T5D.f1=am1;_T1D=_T5D;}return _T1D;}}}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;_T56=_T59->f2;}{struct Cyc_Absyn_Exp*e1=_T57;struct Cyc_Absyn_Exp*e2=_T56;
# 1547
struct _tuple22 _T59=Cyc_Vcgen_vcgen_deref(env,e1,e2,amin);struct Cyc_AssnDef_AssnMap _T5A;void*_T5B;void*_T5C;_T5C=_T59.f0;_T5B=_T59.f1;_T5A=_T59.f2;{void*t1=_T5C;void*t2=_T5B;struct Cyc_AssnDef_AssnMap am1=_T5A;{struct _tuple14 _T5D;_T1F=t1;_T20=t2;_T21=e1;_T22=_T21->topt;
_T5D.f0=Cyc_AssnDef_offseti(_T1F,_T20,_T22);_T5D.f1=am1;_T1E=_T5D;}return _T1E;}}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;_T55=_T59->f2;}{struct Cyc_Absyn_Exp*e1=_T57;struct _fat_ptr*f=_T55;_T23=env;_T24=e1;_T25=
# 1551
Cyc_Vcgen_zero_exp();_T26=amin;{struct _tuple22 _T59=Cyc_Vcgen_vcgen_deref(_T23,_T24,_T25,_T26);struct Cyc_AssnDef_AssnMap _T5A;void*_T5B;void*_T5C;_T5C=_T59.f0;_T5B=_T59.f1;_T5A=_T59.f2;{void*t1=_T5C;void*t2=_T5B;struct Cyc_AssnDef_AssnMap am1=_T5A;_T27=e1;_T28=_T27->topt;_T29=
_check_null(_T28);_T2A=Cyc_Tcutil_pointer_elt_type(_T29);_T2B=f;{int i=Cyc_CfFlowInfo_get_field_index(_T2A,_T2B);_T2C=e;_T2D=_T2C->topt;_T2E=
# 1554
_check_null(_T2D);_T2F=Cyc_Absyn_heap_rgn_type;_T30=Cyc_Absyn_al_qual_type;_T31=Cyc_Absyn_empty_tqual(0U);_T32=Cyc_Absyn_false_type;_T33=Cyc_Absyn_false_type;{void*tp=Cyc_Absyn_at_type(_T2E,_T2F,_T30,_T31,_T32,_T33);{struct _tuple14 _T5D;_T35=t1;_T36=i;_T37=(unsigned)_T36;_T38=tp;
_T5D.f0=Cyc_AssnDef_offsetf(_T35,_T37,_T38);_T5D.f1=am1;_T34=_T5D;}return _T34;}}}}}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;_T55=_T59->f2;}{struct Cyc_Absyn_Exp*e1=_T57;struct _fat_ptr*f=_T55;
# 1557
struct _tuple14 _T59=Cyc_Vcgen_vcgen_lexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T5A;void*_T5B;_T5B=_T59.f0;_T5A=_T59.f1;{void*t1=_T5B;struct Cyc_AssnDef_AssnMap am1=_T5A;_T39=e1;_T3A=_T39->topt;_T3B=
_check_null(_T3A);_T3C=f;{int i=Cyc_CfFlowInfo_get_field_index(_T3B,_T3C);_T3D=e;_T3E=_T3D->topt;_T3F=
# 1560
_check_null(_T3E);_T40=Cyc_Absyn_heap_rgn_type;_T41=Cyc_Absyn_al_qual_type;_T42=Cyc_Absyn_empty_tqual(0U);_T43=Cyc_Absyn_false_type;_T44=Cyc_Absyn_false_type;{void*tp=Cyc_Absyn_at_type(_T3F,_T40,_T41,_T42,_T43,_T44);{struct _tuple14 _T5C;_T46=t1;_T47=i;_T48=(unsigned)_T47;_T49=tp;
_T5C.f0=Cyc_AssnDef_offsetf(_T46,_T48,_T49);_T5C.f1=am1;_T45=_T5C;}return _T45;}}}}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;}{struct Cyc_Absyn_Exp*e1=_T57;_T57=e1;goto _LL14;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;}_LL14: {struct Cyc_Absyn_Exp*e1=_T57;_T4A=
# 1564
Cyc_Vcgen_vcgen_lexp(env,e1,amin);return _T4A;}default: _LL15: _T4B=e;_T4C=_T4B->topt;_T4D=
# 1567
_check_null(_T4C);_T4E=Cyc_Absyn_heap_rgn_type;_T4F=Cyc_Absyn_al_qual_type;_T50=Cyc_Absyn_empty_tqual(0U);_T51=Cyc_Absyn_false_type;_T52=Cyc_Absyn_false_type;{void*tp=Cyc_Absyn_at_type(_T4D,_T4E,_T4F,_T50,_T51,_T52);{struct _tuple14 _T59;
_T59.f0=Cyc_AssnDef_fresh_var(0,tp);_T59.f1=amin;_T53=_T59;}return _T53;}};}}
# 1574
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;struct Cyc_Absyn_Exp*_T3;void*_T4;void*_T5;struct _tuple0 _T6;void*_T7;int*_T8;int _T9;void*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;void*_TC;int*_TD;int _TE;void*_TF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10;void*_T11;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T12;enum Cyc_Absyn_Sign _T13;void*_T14;int*_T15;int _T16;void*_T17;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18;void*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1D;void*_T1E;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T1F;enum Cyc_Absyn_Sign _T20;void*_T21;int*_T22;int _T23;void*_T24;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T25;void*_T26;int*_T27;int _T28;void*_T29;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2A;void*_T2B;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T2C;enum Cyc_Absyn_Sign _T2D;void*_T2E;int*_T2F;int _T30;void*_T31;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T32;void*_T33;int*_T34;int _T35;void*_T36;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T37;void*_T38;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T39;enum Cyc_Absyn_Sign _T3A;void*_T3B;int*_T3C;int _T3D;void*_T3E;int*_T3F;int _T40;void*_T41;int*_T42;int _T43;void*_T44;int*_T45;int _T46;_T0=e1;_T1=_T0->topt;_T2=
_check_null(_T1);{void*t1=Cyc_Absyn_compress(_T2);_T3=e2;_T4=_T3->topt;_T5=
_check_null(_T4);{void*t2=Cyc_Absyn_compress(_T5);{struct _tuple0 _T47;
_T47.f0=t1;_T47.f1=t2;_T6=_T47;}{struct _tuple0 _T47=_T6;_T7=_T47.f0;_T8=(int*)_T7;_T9=*_T8;if(_T9!=0)goto _TL149;_TA=_T47.f0;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA;_TC=_TB->f1;_TD=(int*)_TC;_TE=*_TD;if(_TE!=1)goto _TL14B;_TF=_T47.f0;_T10=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF;_T11=_T10->f1;_T12=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T11;_T13=_T12->f1;if(_T13!=Cyc_Absyn_Unsigned)goto _TL14D;
return 1;_TL14D: _T14=_T47.f1;_T15=(int*)_T14;_T16=*_T15;if(_T16!=0)goto _TL14F;_T17=_T47.f1;_T18=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T17;_T19=_T18->f1;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=1)goto _TL151;_T1C=_T47.f1;_T1D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1C;_T1E=_T1D->f1;_T1F=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T1E;_T20=_T1F->f1;if(_T20!=Cyc_Absyn_Unsigned)goto _TL153;goto _LL3;_TL153: goto _LL7;_TL151: goto _LL7;_TL14F: goto _LL7;_TL14B: _T21=_T47.f1;_T22=(int*)_T21;_T23=*_T22;if(_T23!=0)goto _TL155;_T24=_T47.f1;_T25=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T24;_T26=_T25->f1;_T27=(int*)_T26;_T28=*_T27;if(_T28!=1)goto _TL157;_T29=_T47.f1;_T2A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T29;_T2B=_T2A->f1;_T2C=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T2B;_T2D=_T2C->f1;if(_T2D!=Cyc_Absyn_Unsigned)goto _TL159;goto _LL3;_TL159: goto _LL7;_TL157: goto _LL7;_TL155: goto _LL7;_TL149: _T2E=_T47.f1;_T2F=(int*)_T2E;_T30=*_T2F;if(_T30!=0)goto _TL15B;_T31=_T47.f1;_T32=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T31;_T33=_T32->f1;_T34=(int*)_T33;_T35=*_T34;if(_T35!=1)goto _TL15D;_T36=_T47.f1;_T37=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T36;_T38=_T37->f1;_T39=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T38;_T3A=_T39->f1;if(_T3A!=Cyc_Absyn_Unsigned)goto _TL15F;_LL3:
 return 1;_TL15F: _T3B=_T47.f0;_T3C=(int*)_T3B;_T3D=*_T3C;if(_T3D!=4)goto _TL161;goto _LL7;_TL161: goto _LL7;_TL15D: _T3E=_T47.f0;_T3F=(int*)_T3E;_T40=*_T3F;if(_T40!=4)goto _TL163;goto _LL7;_TL163: goto _LL7;_TL15B: _T41=_T47.f0;_T42=(int*)_T41;_T43=*_T42;if(_T43!=4)goto _TL165;_T44=_T47.f1;_T45=(int*)_T44;_T46=*_T45;if(_T46!=4)goto _TL167;
return 1;_TL167: goto _LL7;_TL165: _LL7:
 return 0;;}}}}
# 1592 "vcgen.cyc"
static struct _tuple18 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap ain){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct _tuple18 _T3;struct Cyc_Vcgen_Env*_T4;struct Cyc_AssnDef_AssnMap _T5;struct Cyc_Vcgen_Env*_T6;struct Cyc_AssnDef_AssnMap _T7;struct _tuple18 _T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_AssnDef_AssnMap _TA;struct _tuple18 _TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_AssnDef_AssnMap _TD;struct _tuple18 _TE;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TF;enum Cyc_Absyn_Primop _T10;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;void*_T14;struct _tuple18 _T15;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T16;struct Cyc_List_List*_T17;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_List_List*_T24;void*_T25;enum Cyc_Absyn_Primop _T26;int _T27;struct Cyc_AssnDef_AssnMap _T28;void*_T29;struct Cyc_AssnDef_AssnMap _T2A;void*_T2B;struct Cyc_AssnDef_AssnMap _T2C;void*_T2D;struct Cyc_AssnDef_AssnMap _T2E;void*_T2F;struct Cyc_AssnDef_AssnMap _T30;void*_T31;struct Cyc_AssnDef_AssnMap _T32;void*_T33;struct Cyc_AssnDef_AssnMap _T34;void*_T35;struct Cyc_AssnDef_AssnMap _T36;void*_T37;struct Cyc_AssnDef_AssnMap _T38;void*_T39;struct Cyc_AssnDef_AssnMap _T3A;void*_T3B;struct Cyc_AssnDef_AssnMap _T3C;void*_T3D;struct Cyc_AssnDef_AssnMap _T3E;void*_T3F;struct Cyc_AssnDef_AssnMap _T40;void*_T41;struct Cyc_AssnDef_AssnMap _T42;void*_T43;struct Cyc_AssnDef_AssnMap _T44;void*_T45;struct Cyc_AssnDef_AssnMap _T46;void*_T47;struct Cyc_AssnDef_AssnMap _T48;void*_T49;struct Cyc_AssnDef_AssnMap _T4A;void*_T4B;struct Cyc_AssnDef_AssnMap _T4C;void*_T4D;struct Cyc_AssnDef_AssnMap _T4E;void*_T4F;struct Cyc_Vcgen_Env*_T50;struct Cyc_Absyn_Exp*_T51;void*_T52;enum Cyc_Absyn_Primop _T53;void*_T54;void*_T55;struct Cyc_AssnDef_AssnMap _T56;struct Cyc_AssnDef_AssnMap _T57;void*_T58;void*_T59;void*_T5A;struct Cyc_AssnDef_AssnMap _T5B;void*_T5C;void*_T5D;void*_T5E;struct _tuple18 _T5F;struct Cyc_Absyn_Exp*_T60;void*_T61;void*_T62;struct _fat_ptr*_T63;int _T64;unsigned _T65;struct Cyc_AssnDef_AssnMap _T66;void*_T67;struct Cyc_AssnDef_AssnMap _T68;void*_T69;struct _tuple18 _T6A;void*_T6B;struct _tuple0 _T6C;struct Cyc_Absyn_Exp*_T6D;void*_T6E;void*_T6F;void*_T70;int*_T71;int _T72;void*_T73;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T74;void*_T75;int*_T76;int _T77;void*_T78;int*_T79;int _T7A;void*_T7B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7C;void*_T7D;int*_T7E;int _T7F;void*_T80;void*_T81;void*_T82;void*_T83;enum Cyc_Absyn_Size_of _T84;enum Cyc_Absyn_Size_of _T85;int _T86;enum Cyc_Absyn_Size_of _T87;enum Cyc_Absyn_Size_of _T88;int _T89;enum Cyc_Absyn_Size_of _T8A;int _T8B;enum Cyc_Absyn_Size_of _T8C;int _T8D;struct _tuple18 _T8E;void*_T8F;int*_T90;int _T91;void*_T92;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T93;unsigned _T94;struct _tuple18 _T95;struct Cyc_AssnDef_AssnMap _T96;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T97;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T98;void*_T99;struct _tuple18 _T9A;struct Cyc_AssnDef_AssnMap _T9B;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T9C;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T9D;void*_T9E;struct _tuple18 _T9F;struct Cyc_AssnDef_AssnMap _TA0;void*_TA1;void*_TA2;void*_TA3;struct Cyc_AssnDef_AssnMap _TA4;void*_TA5;void*_TA6;void*_TA7;_T0=e;{
void*_TA8=_T0->r;struct _fat_ptr*_TA9;enum Cyc_Absyn_Primop _TAA;struct Cyc_Absyn_Exp*_TAB;struct Cyc_Absyn_Exp*_TAC;void*_TAD;_T1=(int*)_TA8;_T2=*_T1;switch(_T2){case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TAC=_TAE->f2;_TAB=_TAE->f3;}{struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;struct Cyc_Absyn_Exp*e3=_TAB;
# 1595
struct _tuple18 _TAE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;struct Cyc_AssnDef_AssnMap _TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap a1t=_TB0;struct Cyc_AssnDef_AssnMap a1f=_TAF;
struct _tuple18 _TB1=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct Cyc_AssnDef_AssnMap _TB2;struct Cyc_AssnDef_AssnMap _TB3;_TB3=_TB1.f0;_TB2=_TB1.f1;{struct Cyc_AssnDef_AssnMap a2t=_TB3;struct Cyc_AssnDef_AssnMap a2f=_TB2;
struct _tuple18 _TB4=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct Cyc_AssnDef_AssnMap _TB5;struct Cyc_AssnDef_AssnMap _TB6;_TB6=_TB4.f0;_TB5=_TB4.f1;{struct Cyc_AssnDef_AssnMap a3t=_TB6;struct Cyc_AssnDef_AssnMap a3f=_TB5;{struct _tuple18 _TB7;_T4=env;_T5=
Cyc_AssnDef_or_assnmap_assnmap(a2t,a3t);_TB7.f0=Cyc_Vcgen_may_widen(_T4,_T5);_T6=env;_T7=
Cyc_AssnDef_or_assnmap_assnmap(a2f,a3f);_TB7.f1=Cyc_Vcgen_may_widen(_T6,_T7);_T3=_TB7;}
# 1598
return _T3;}}}}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TAC=_TAE->f2;}{struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;
# 1601
struct _tuple18 _TAE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;struct Cyc_AssnDef_AssnMap _TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap a1t=_TB0;struct Cyc_AssnDef_AssnMap a1f=_TAF;
struct _tuple18 _TB1=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct Cyc_AssnDef_AssnMap _TB2;struct Cyc_AssnDef_AssnMap _TB3;_TB3=_TB1.f0;_TB2=_TB1.f1;{struct Cyc_AssnDef_AssnMap a2t=_TB3;struct Cyc_AssnDef_AssnMap a2f=_TB2;{struct _tuple18 _TB4;
_TB4.f0=a2t;_T9=env;_TA=Cyc_AssnDef_or_assnmap_assnmap(a1f,a2f);_TB4.f1=Cyc_Vcgen_may_widen(_T9,_TA);_T8=_TB4;}return _T8;}}}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TAC=_TAE->f2;}{struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;
# 1605
struct _tuple18 _TAE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;struct Cyc_AssnDef_AssnMap _TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap a1t=_TB0;struct Cyc_AssnDef_AssnMap a1f=_TAF;
struct _tuple18 _TB1=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct Cyc_AssnDef_AssnMap _TB2;struct Cyc_AssnDef_AssnMap _TB3;_TB3=_TB1.f0;_TB2=_TB1.f1;{struct Cyc_AssnDef_AssnMap a2t=_TB3;struct Cyc_AssnDef_AssnMap a2f=_TB2;{struct _tuple18 _TB4;_TC=env;_TD=
Cyc_AssnDef_or_assnmap_assnmap(a1t,a2t);_TB4.f0=Cyc_Vcgen_may_widen(_TC,_TD);_TB4.f1=a2f;_TB=_TB4;}return _TB;}}}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TAC=_TAE->f2;}{struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;
# 1609
struct _tuple14 _TAE=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap ain=_TAF;_TE=
Cyc_Vcgen_vcgen_test(env,e2,ain);return _TE;}}case 3: _TF=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T10=_TF->f1;if(_T10!=Cyc_Absyn_Not)goto _TL16A;_T11=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T12=_T11->f2;if(_T12==0)goto _TL16C;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T13=_TAE->f2;{struct Cyc_List_List _TAF=*_T13;_T14=_TAF.hd;_TAD=(struct Cyc_Absyn_Exp*)_T14;}}{struct Cyc_Absyn_Exp*e1=_TAD;
# 1612
struct _tuple18 _TAE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;struct Cyc_AssnDef_AssnMap _TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap at=_TB0;struct Cyc_AssnDef_AssnMap af=_TAF;{struct _tuple18 _TB1;
_TB1.f0=af;_TB1.f1=at;_T15=_TB1;}return _T15;}}_TL16C: goto _LL11;_TL16A: _T16=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T17=_T16->f2;if(_T17==0)goto _TL16E;_T18=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T19=_T18->f2;_T1A=(struct Cyc_List_List*)_T19;_T1B=_T1A->tl;if(_T1B==0)goto _TL170;_T1C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T1D=_T1C->f2;_T1E=(struct Cyc_List_List*)_T1D;_T1F=_T1E->tl;_T20=(struct Cyc_List_List*)_T1F;_T21=_T20->tl;if(_T21!=0)goto _TL172;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_TAA=_TAE->f1;_T22=_TAE->f2;{struct Cyc_List_List _TAF=*_T22;_T23=_TAF.hd;_TAD=(struct Cyc_Absyn_Exp*)_T23;_T24=_TAF.tl;{struct Cyc_List_List _TB0=*_T24;_T25=_TB0.hd;_TAC=(struct Cyc_Absyn_Exp*)_T25;}}}{enum Cyc_Absyn_Primop p=_TAA;struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;
# 1623
struct _tuple14 _TAE=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;void*_TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{void*t1=_TB0;struct Cyc_AssnDef_AssnMap a1=_TAF;
struct _tuple14 _TB1=Cyc_Vcgen_vcgen_rexp(env,e2,a1);struct Cyc_AssnDef_AssnMap _TB2;void*_TB3;_TB3=_TB1.f0;_TB2=_TB1.f1;{void*t2=_TB3;struct Cyc_AssnDef_AssnMap a2=_TB2;
struct Cyc_AssnDef_AssnMap at=a2;
struct Cyc_AssnDef_AssnMap af=a2;_T26=p;_T27=(int)_T26;switch(_T27){case Cyc_Absyn_Eq: _T28=at;_T29=
# 1629
Cyc_AssnDef_eq(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T28,_T29);_T2A=af;_T2B=Cyc_AssnDef_neq(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T2A,_T2B);goto _LL37;case Cyc_Absyn_Neq: _T2C=at;_T2D=
# 1632
Cyc_AssnDef_neq(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T2C,_T2D);_T2E=af;_T2F=Cyc_AssnDef_eq(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T2E,_T2F);goto _LL37;case Cyc_Absyn_ULt: _T30=at;_T31=
# 1634
Cyc_AssnDef_ult(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T30,_T31);_T32=af;_T33=Cyc_AssnDef_ulte(t2,t1);af=Cyc_AssnDef_and_assnmap_assn(_T32,_T33);goto _LL37;case Cyc_Absyn_Lt: _T34=at;_T35=
# 1636
Cyc_AssnDef_slt(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T34,_T35);_T36=af;_T37=Cyc_AssnDef_slte(t2,t1);af=Cyc_AssnDef_and_assnmap_assn(_T36,_T37);goto _LL37;case Cyc_Absyn_ULte: _T38=at;_T39=
# 1638
Cyc_AssnDef_ulte(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T38,_T39);_T3A=af;_T3B=Cyc_AssnDef_ult(t2,t1);af=Cyc_AssnDef_and_assnmap_assn(_T3A,_T3B);goto _LL37;case Cyc_Absyn_Lte: _T3C=at;_T3D=
# 1640
Cyc_AssnDef_slte(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T3C,_T3D);_T3E=af;_T3F=Cyc_AssnDef_slt(t2,t1);af=Cyc_AssnDef_and_assnmap_assn(_T3E,_T3F);goto _LL37;case Cyc_Absyn_UGt: _T40=at;_T41=
# 1642
Cyc_AssnDef_ult(t2,t1);at=Cyc_AssnDef_and_assnmap_assn(_T40,_T41);_T42=af;_T43=Cyc_AssnDef_ulte(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T42,_T43);goto _LL37;case Cyc_Absyn_Gt: _T44=at;_T45=
# 1644
Cyc_AssnDef_slt(t2,t1);at=Cyc_AssnDef_and_assnmap_assn(_T44,_T45);_T46=af;_T47=Cyc_AssnDef_slte(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T46,_T47);goto _LL37;case Cyc_Absyn_UGte: _T48=at;_T49=
# 1646
Cyc_AssnDef_ulte(t2,t1);at=Cyc_AssnDef_and_assnmap_assn(_T48,_T49);_T4A=af;_T4B=Cyc_AssnDef_ult(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T4A,_T4B);goto _LL37;case Cyc_Absyn_Gte: _T4C=at;_T4D=
# 1648
Cyc_AssnDef_slte(t2,t1);at=Cyc_AssnDef_and_assnmap_assn(_T4C,_T4D);_T4E=af;_T4F=Cyc_AssnDef_slt(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T4E,_T4F);goto _LL37;default: _T50=env;_T51=e;_T52=_T51->topt;_T53=p;_T54=t1;_T55=t2;_T56=at;{
# 1650
struct _tuple14 _TB4=Cyc_Vcgen_vcgen_binop(_T50,_T52,_T53,_T54,_T55,_T56);struct Cyc_AssnDef_AssnMap _TB5;void*_TB6;_TB6=_TB4.f0;_TB5=_TB4.f1;{void*t=_TB6;struct Cyc_AssnDef_AssnMap am=_TB5;_T57=at;_T58=
Cyc_AssnDef_one();_T59=t;_T5A=Cyc_AssnDef_ulte(_T58,_T59);at=Cyc_AssnDef_and_assnmap_assn(_T57,_T5A);_T5B=af;_T5C=t;_T5D=
Cyc_AssnDef_zero();_T5E=Cyc_AssnDef_eq(_T5C,_T5D);af=Cyc_AssnDef_and_assnmap_assn(_T5B,_T5E);goto _LL37;}}}_LL37:{struct _tuple18 _TB4;
# 1655
_TB4.f0=at;_TB4.f1=af;_T5F=_TB4;}return _T5F;}}}_TL172: goto _LL11;_TL170: goto _LL11;_TL16E: goto _LL11;case 37:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TA9=_TAE->f2;}{struct Cyc_Absyn_Exp*e1=_TAD;struct _fat_ptr*f=_TA9;_T60=e1;_T61=_T60->topt;_T62=
# 1616
_check_null(_T61);_T63=f;_T64=Cyc_CfFlowInfo_get_field_index(_T62,_T63);_T65=(unsigned)_T64;{void*i=Cyc_AssnDef_uint(_T65);
struct _tuple14 _TAE=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;void*_TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{void*t=_TB0;struct Cyc_AssnDef_AssnMap ain=_TAF;
void*tt=Cyc_AssnDef_tagof_tm(t);_T66=ain;_T67=
Cyc_AssnDef_eq(tt,i);{struct Cyc_AssnDef_AssnMap at=Cyc_AssnDef_and_assnmap_assn(_T66,_T67);_T68=ain;_T69=
Cyc_AssnDef_neq(tt,i);{struct Cyc_AssnDef_AssnMap af=Cyc_AssnDef_and_assnmap_assn(_T68,_T69);{struct _tuple18 _TB1;
_TB1.f0=at;_TB1.f1=af;_T6A=_TB1;}return _T6A;}}}}}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TA8;_T6B=_TAE->f1;_TAD=(void*)_T6B;_TAC=_TAE->f2;}{void*tp=_TAD;struct Cyc_Absyn_Exp*e1=_TAC;{struct _tuple0 _TAE;
# 1657
_TAE.f0=Cyc_Absyn_compress(tp);_T6D=e1;_T6E=_T6D->topt;_T6F=_check_null(_T6E);_TAE.f1=Cyc_Absyn_compress(_T6F);_T6C=_TAE;}{struct _tuple0 _TAE=_T6C;enum Cyc_Absyn_Size_of _TAF;enum Cyc_Absyn_Sign _TB0;enum Cyc_Absyn_Size_of _TB1;enum Cyc_Absyn_Sign _TB2;_T70=_TAE.f0;_T71=(int*)_T70;_T72=*_T71;if(_T72!=0)goto _TL175;_T73=_TAE.f0;_T74=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T73;_T75=_T74->f1;_T76=(int*)_T75;_T77=*_T76;if(_T77!=1)goto _TL177;_T78=_TAE.f1;_T79=(int*)_T78;_T7A=*_T79;if(_T7A!=0)goto _TL179;_T7B=_TAE.f1;_T7C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7B;_T7D=_T7C->f1;_T7E=(int*)_T7D;_T7F=*_T7E;if(_T7F!=1)goto _TL17B;_T80=_TAE.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T80;_T81=_TB3->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TB4=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T81;_TB2=_TB4->f1;_TB1=_TB4->f2;}}_T82=_TAE.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T82;_T83=_TB3->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TB4=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T83;_TB0=_TB4->f1;_TAF=_TB4->f2;}}{enum Cyc_Absyn_Sign u1=_TB2;enum Cyc_Absyn_Size_of s1=_TB1;enum Cyc_Absyn_Sign u2=_TB0;enum Cyc_Absyn_Size_of s2=_TAF;_T85=s1;_T86=(int)_T85;
# 1663
if(_T86!=3)goto _TL17D;_T84=2U;goto _TL17E;_TL17D: _T84=s1;_TL17E: s1=_T84;_T88=s2;_T89=(int)_T88;
if(_T89!=3)goto _TL17F;_T87=2U;goto _TL180;_TL17F: _T87=s2;_TL180: s2=_T87;_T8A=s1;_T8B=(int)_T8A;_T8C=s2;_T8D=(int)_T8C;
if(_T8B!=_T8D)goto _TL181;_T8E=Cyc_Vcgen_vcgen_test(env,e1,ain);return _T8E;_TL181: goto _LL51;}_TL17B: goto _LL54;_TL179: goto _LL54;_TL177: goto _LL54;_TL175: _LL54: goto _LL51;_LL51:;}goto _LL12;}default: _LL11: _LL12: {
# 1671
struct _tuple14 _TAE=Cyc_Vcgen_vcgen_rexp(env,e,ain);struct Cyc_AssnDef_AssnMap _TAF;void*_TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{void*t=_TB0;struct Cyc_AssnDef_AssnMap aout=_TAF;_T8F=t;_T90=(int*)_T8F;_T91=*_T90;if(_T91!=0)goto _TL183;_T92=t;_T93=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T92;_T94=_T93->f1;if(_T94!=0)goto _TL185;{struct _tuple18 _TB1;_T96=aout;_T97=& Cyc_AssnDef_false_assn;_T98=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T97;_T99=(void*)_T98;
# 1674
_TB1.f0=Cyc_AssnDef_and_assnmap_assn(_T96,_T99);_TB1.f1=aout;_T95=_TB1;}return _T95;_TL185:{struct _tuple18 _TB1;
# 1676
_TB1.f0=aout;_T9B=aout;_T9C=& Cyc_AssnDef_false_assn;_T9D=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T9C;_T9E=(void*)_T9D;_TB1.f1=Cyc_AssnDef_and_assnmap_assn(_T9B,_T9E);_T9A=_TB1;}return _T9A;_TL183:{struct _tuple18 _TB1;_TA0=aout;_TA1=
# 1678
Cyc_AssnDef_one();_TA2=t;_TA3=Cyc_AssnDef_ulte(_TA1,_TA2);_TB1.f0=Cyc_AssnDef_and_assnmap_assn(_TA0,_TA3);_TA4=aout;_TA5=t;_TA6=Cyc_AssnDef_zero();_TA7=Cyc_AssnDef_eq(_TA5,_TA6);_TB1.f1=Cyc_AssnDef_and_assnmap_assn(_TA4,_TA7);_T9F=_TB1;}return _T9F;;}}};}}
# 1684
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*topt,struct Cyc_AssnDef_AssnMap a,void*tp){struct Cyc_AssnDef_AssnMap _T0;void*_T1;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T2;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T3;void*_T4;struct Cyc_Vcgen_Env*_T5;struct Cyc_AssnDef_AssnMap*_T6;struct Cyc_Vcgen_Env*_T7;struct Cyc_Absyn_Vardecl*_T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_AssnDef_AssnMap _TA;struct Cyc_Vcgen_Env*_TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;void*_TF;struct Cyc_Vcgen_Env*_T10;struct Cyc_Vcgen_Env*_T11;struct Cyc_AssnDef_AssnMap*_T12;struct Cyc_AssnDef_AssnMap*_T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_AssnMap _T15;struct Cyc_AssnDef_AssnMap _T16;struct Cyc_Vcgen_Env*_T17;struct Cyc_AssnDef_AssnMap*_T18;struct Cyc_AssnDef_AssnMap*_T19;_T0=a;_T1=_T0.assn;_T2=& Cyc_AssnDef_false_assn;_T3=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T2;_T4=(void*)_T3;
# 1686
if(_T1!=_T4)goto _TL187;return;_TL187: {
void*t;
if(topt==0)goto _TL189;t=topt;goto _TL18A;
_TL189: t=Cyc_AssnDef_fresh_var(0,tp);_TL18A: _T5=env;_T6=_T5->res_assnmap;
# 1691
if(_T6!=0)goto _TL18B;return;_TL18B: _T7=env;_T8=_T7->result;
if(_T8==0)goto _TL18D;_T9=env;_TA=a;_TB=env;_TC=env;_TD=_TC->result;_TE=
Cyc_Vcgen_myaddr(_TB,_TD);_TF=t;a=Cyc_Vcgen_do_assign(_T9,_TA,_TE,_TF);goto _TL18E;_TL18D: _TL18E: _T10=env;_T11=env;_T12=_T11->res_assnmap;_T13=
# 1695
_check_null(_T12);_T14=*_T13;_T15=a;_T16=Cyc_AssnDef_or_assnmap_assnmap(_T14,_T15);a=Cyc_Vcgen_may_widen(_T10,_T16);_T17=env;_T18=_T17->res_assnmap;_T19=
_check_null(_T18);*_T19=a;}}
# 1699
static struct Cyc_Absyn_Exp*Cyc_Vcgen_find_assert_e(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Exp*_T5;void*_T6;int*_T7;int _T8;struct Cyc_Absyn_Exp*_T9;void*_TA;int _TB;struct Cyc_Absyn_Exp*_TC;
_TL18F: if(1)goto _TL190;else{goto _TL191;}
_TL190: _T0=s;{void*_TD=_T0->r;struct Cyc_Absyn_Stmt*_TE;int _TF;struct Cyc_Absyn_Exp*_T10;_T1=(int*)_TD;_T2=*_T1;switch(_T2){case 1: _T3=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_TD;_T4=_T3->f1;_T5=(struct Cyc_Absyn_Exp*)_T4;_T6=_T5->r;_T7=(int*)_T6;_T8=*_T7;if(_T8!=41)goto _TL193;{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T11=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_TD;_T9=_T11->f1;{struct Cyc_Absyn_Exp _T12=*_T9;_TA=_T12.r;{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T13=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TA;_T10=_T13->f1;_TF=_T13->f2;}}}_TB=(int)_TF;if(!_TB)goto _TL195;{struct Cyc_Absyn_Exp*e=_T10;int static_only=_TF;_TC=e;
return _TC;}_TL195: goto _LL5;_TL193: goto _LL5;case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T11=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_TD;_TE=_T11->f1;}{struct Cyc_Absyn_Stmt*sa=_TE;
s=sa;goto _TL18F;}default: _LL5:
 return 0;};}goto _TL18F;_TL191:;}
# 1717 "vcgen.cyc"
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_stmt(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_AssnDef_AssnMap _T3;struct Cyc_Vcgen_Env*_T4;void**_T5;struct Cyc_Vcgen_Env*_T6;void**_T7;struct Cyc_AssnDef_AssnMap _T8;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Vcgen_Env*_TB;void*_TC;struct Cyc_AssnDef_AssnMap _TD;struct Cyc_Absyn_Exp*_TE;void*_TF;struct Cyc_AssnDef_AssnMap _T10;struct Cyc_AssnDef_AssnMap _T11;struct Cyc_Vcgen_Env*_T12;struct Cyc_AssnDef_AssnMap _T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_Vcgen_Env*_T15;struct Cyc_AssnDef_AssnMap*_T16;struct Cyc_AssnDef_AssnMap*_T17;struct Cyc_Vcgen_Env*_T18;struct Cyc_AssnDef_AssnMap*_T19;struct Cyc_AssnDef_AssnMap*_T1A;struct Cyc_AssnDef_AssnMap _T1B;struct Cyc_AssnDef_AssnMap _T1C;struct Cyc_AssnDef_AssnMap _T1D;struct Cyc_Absyn_Stmt*(*_T1E)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T1F)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Vcgen_Env*_T20;struct Cyc_Hashtable_Table*_T21;struct Cyc_Absyn_Stmt*_T22;struct Cyc_AssnDef_AssnMap _T23;struct _tuple11 _T24;struct _tuple11 _T25;struct Cyc_AssnDef_AssnMap _T26;struct _tuple11 _T27;struct _tuple11 _T28;struct _tuple11 _T29;struct _tuple11 _T2A;struct Cyc_AssnDef_AssnMap _T2B;struct _tuple11 _T2C;struct _tuple11 _T2D;struct Cyc_AssnDef_AssnMap _T2E;void*_T2F;struct Cyc_AssnDef_AssnMap _T30;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T31;struct Cyc_Absyn_Switch_clause**_T32;int(*_T33)(struct _fat_ptr,struct _fat_ptr);void*(*_T34)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T35;struct _fat_ptr _T36;struct Cyc_Absyn_Switch_clause**_T37;struct Cyc_Absyn_Switch_clause*_T38;struct Cyc_Core_Opt*_T39;struct Cyc_Core_Opt*_T3A;void*_T3B;struct Cyc_List_List*_T3C;struct _tuple1 _T3D;struct Cyc_List_List*_T3E;struct Cyc_List_List*_T3F;void*_T40;struct Cyc_Vcgen_Env*_T41;struct Cyc_AssnDef_AssnMap _T42;void*_T43;struct Cyc_List_List*_T44;void*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;struct Cyc_Vcgen_Env*_T48;struct Cyc_Absyn_Switch_clause*_T49;struct Cyc_Absyn_Stmt*_T4A;struct Cyc_AssnDef_AssnMap _T4B;struct Cyc_AssnDef_AssnMap _T4C;struct Cyc_Vcgen_Env*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_AssnDef_AssnMap _T4F;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T50;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T51;struct Cyc_AssnDef_AssnMap _T52;struct Cyc_AssnDef_AssnMap _T53;void*_T54;struct Cyc_AssnDef_AssnMap _T55;void*_T56;int _T57;struct Cyc_String_pa_PrintArg_struct _T58;struct Cyc_String_pa_PrintArg_struct _T59;struct Cyc_Absyn_Stmt*_T5A;unsigned _T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;struct Cyc_Vcgen_Env*_T5E;struct Cyc_Absyn_Exp*_T5F;struct Cyc_AssnDef_AssnMap _T60;struct Cyc_Absyn_Stmt*(*_T61)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T62)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Vcgen_Env*_T63;struct Cyc_Hashtable_Table*_T64;struct Cyc_Absyn_Stmt*_T65;struct Cyc_Absyn_Stmt*_T66;int*_T67;int _T68;struct Cyc_Vcgen_Env*_T69;struct Cyc_Absyn_Exp*_T6A;struct Cyc_AssnDef_AssnMap _T6B;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T6C;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T6D;struct Cyc_AssnDef_AssnMap _T6E;struct Cyc_AssnDef_AssnMap _T6F;void*_T70;struct Cyc_AssnDef_AssnMap _T71;void*_T72;int _T73;struct Cyc_String_pa_PrintArg_struct _T74;struct Cyc_AssnDef_AssnMap _T75;void*_T76;struct Cyc_String_pa_PrintArg_struct _T77;struct Cyc_Absyn_Stmt*_T78;unsigned _T79;struct _fat_ptr _T7A;struct _fat_ptr _T7B;int(*_T7C)(struct _fat_ptr,struct _fat_ptr);void*(*_T7D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7E;struct _fat_ptr _T7F;struct Cyc_AssnDef_AssnMap _T80;void*_T81;struct Cyc_Vcgen_Env*_T82;struct Cyc_Vcgen_Env*_T83;struct Cyc_AssnDef_AssnMap*_T84;struct Cyc_Vcgen_Env*_T85;struct Cyc_AssnDef_AssnMap*_T86;struct Cyc_AssnDef_AssnMap*_T87;struct Cyc_Vcgen_Env*_T88;struct Cyc_Absyn_Vardecl*_T89;struct Cyc_AssnDef_AssnMap _T8A;struct Cyc_Vcgen_Env*_T8B;struct Cyc_AssnDef_AssnMap _T8C;struct Cyc_AssnDef_AssnMap _T8D;
LOOP: {
struct Cyc_AssnDef_AssnMap oldam=Cyc_Vcgen_lookup_stmt_assnmap(env,s);
amin=Cyc_AssnDef_or_assnmap_assnmap(oldam,amin);_T0=s;{
void*_T8E=_T0->r;struct Cyc_Absyn_Switch_clause*_T8F;struct Cyc_List_List*_T90;struct Cyc_Absyn_Stmt*_T91;struct Cyc_Absyn_Stmt*_T92;struct Cyc_Absyn_Exp*_T93;struct Cyc_Absyn_Stmt*_T94;void*_T95;void*_T96;_T1=(int*)_T8E;_T2=*_T1;switch(_T2){case 0: _T3=amin;
return _T3;case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;}{struct Cyc_Absyn_Exp*e=_T96;
# 1724
struct _tuple14 _T97=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T98;void*_T99;_T99=_T97.f0;_T98=_T97.f1;{void*t=_T99;struct Cyc_AssnDef_AssnMap amout=_T98;_T4=env;_T5=_T4->exp_stmt;
if(_T5==0)goto _TL198;_T6=env;_T7=_T6->exp_stmt;
*_T7=t;goto _TL199;_TL198: _TL199: _T8=amout;
return _T8;}}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T95=_T97->f2;}{struct Cyc_Absyn_Stmt*s1=_T96;struct Cyc_Absyn_Stmt*s2=_T95;
# 1729
amin=Cyc_Vcgen_vcgen_stmt(env,s1,amin);
s=s2;goto LOOP;}case 3: _T9=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T8E;_TA=_T9->f1;if(_TA==0)goto _TL19A;{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;}{struct Cyc_Absyn_Exp*e=_T96;
# 1733
struct _tuple14 _T97=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T98;void*_T99;_T99=_T97.f0;_T98=_T97.f1;{void*t=_T99;struct Cyc_AssnDef_AssnMap amout=_T98;_TB=env;_TC=t;_TD=amout;_TE=e;_TF=_TE->topt;
Cyc_Vcgen_update_return(_TB,_TC,_TD,_TF);_T10=
Cyc_AssnDef_false_assnmap();return _T10;}}_TL19A:
# 1737
 Cyc_Vcgen_update_return(env,0,amin,0);_T11=
Cyc_AssnDef_false_assnmap();return _T11;case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T95=_T97->f2;_T94=_T97->f3;}{struct Cyc_Absyn_Exp*e1=_T96;struct Cyc_Absyn_Stmt*s1=_T95;struct Cyc_Absyn_Stmt*s2=_T94;
# 1740
struct _tuple18 _T97=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T98;struct Cyc_AssnDef_AssnMap _T99;_T99=_T97.f0;_T98=_T97.f1;{struct Cyc_AssnDef_AssnMap amt=_T99;struct Cyc_AssnDef_AssnMap amf=_T98;
amt=Cyc_Vcgen_vcgen_stmt(env,s1,amt);
amf=Cyc_Vcgen_vcgen_stmt(env,s2,amf);_T12=env;_T13=
Cyc_AssnDef_or_assnmap_assnmap(amt,amf);_T14=Cyc_Vcgen_may_widen(_T12,_T13);return _T14;}}case 7: _T15=env;_T16=_T15->continue_assnmap;_T17=
# 1745
_check_null(_T16);_T18=env;_T19=_T18->continue_assnmap;_T1A=_check_null(_T19);_T1B=*_T1A;_T1C=amin;*_T17=Cyc_AssnDef_or_assnmap_assnmap(_T1B,_T1C);_T1D=
Cyc_AssnDef_false_assnmap();return _T1D;case 6: _T1F=Cyc_Hashtable_lookup;{
# 1748
struct Cyc_Absyn_Stmt*(*_T97)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T1F;_T1E=_T97;}_T20=env;_T21=_T20->succ_table;_T22=s;{struct Cyc_Absyn_Stmt*dest=_T1E(_T21,_T22);
if(dest!=0)goto _TL19C;
Cyc_Vcgen_update_return(env,0,amin,0);goto _TL19D;
# 1752
_TL19C: Cyc_Vcgen_update_stmt_assnmap(env,dest,amin);_TL19D: _T23=
Cyc_AssnDef_false_assnmap();return _T23;}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T8E;_T24=_T97->f1;_T96=_T24.f0;_T25=_T97->f1;_T95=_T25.f1;_T94=_T97->f2;}{struct Cyc_Absyn_Exp*e=_T96;struct Cyc_Absyn_Stmt*dummy=_T95;struct Cyc_Absyn_Stmt*s1=_T94;_T26=
# 1755
Cyc_Vcgen_vcgen_loop(env,0,0,e,dummy,s1,0,amin);return _T26;}case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T27=_T97->f2;_T95=_T27.f0;_T28=_T97->f2;_T94=_T28.f1;_T29=_T97->f3;_T93=_T29.f0;_T2A=_T97->f3;_T92=_T2A.f1;_T91=_T97->f4;}{struct Cyc_Absyn_Exp*e1=_T96;struct Cyc_Absyn_Exp*e2=_T95;struct Cyc_Absyn_Stmt*dummy2=_T94;struct Cyc_Absyn_Exp*e3=_T93;struct Cyc_Absyn_Stmt*dummy3=_T92;struct Cyc_Absyn_Stmt*s1=_T91;_T2B=
# 1757
Cyc_Vcgen_vcgen_loop(env,e1,0,e2,dummy2,s1,e3,amin);return _T2B;}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T2C=_T97->f2;_T95=_T2C.f0;_T2D=_T97->f2;_T94=_T2D.f1;}{struct Cyc_Absyn_Stmt*s1=_T96;struct Cyc_Absyn_Exp*e=_T95;struct Cyc_Absyn_Stmt*dummy=_T94;_T2E=
# 1759
Cyc_Vcgen_vcgen_loop(env,0,s1,e,dummy,s1,0,amin);return _T2E;}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T90=_T97->f2;_T2F=_T97->f3;_T95=(void*)_T2F;}{struct Cyc_Absyn_Exp*e=_T96;struct Cyc_List_List*scs=_T90;void*dtree=_T95;
# 1761
struct _tuple14 _T97=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T98;void*_T99;_T99=_T97.f0;_T98=_T97.f1;{void*t=_T99;struct Cyc_AssnDef_AssnMap am=_T98;_T30=
Cyc_Vcgen_vcgen_switch(env,t,scs,am);return _T30;}}case 11: _T31=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T8E;_T32=_T31->f2;if(_T32!=0)goto _TL19E;_T34=Cyc_Warn_impos;{
int(*_T97)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T34;_T33=_T97;}_T35=_tag_fat("vcgen fallthru",sizeof(char),15U);_T36=_tag_fat(0U,sizeof(void*),0);_T33(_T35,_T36);goto _TL19F;_TL19E:{struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T37=_T97->f2;{struct Cyc_Absyn_Switch_clause*_T98=*_T37;_T8F=_T98;}}{struct Cyc_List_List*es=_T96;struct Cyc_Absyn_Switch_clause*dest=_T8F;
# 1766
struct _tuple17 _T97=Cyc_Vcgen_vcgen_rexps(env,es,amin);struct Cyc_AssnDef_AssnMap _T98;struct Cyc_List_List*_T99;_T99=_T97.f0;_T98=_T97.f1;{struct Cyc_List_List*ts=_T99;struct Cyc_AssnDef_AssnMap am=_T98;
if(ts==0)goto _TL1A0;_T38=dest;_T39=_T38->pat_vars;_T3A=
# 1769
_check_null(_T39);_T3B=_T3A->v;_T3C=(struct Cyc_List_List*)_T3B;_T3D=Cyc_List_split(_T3C);_T3E=_T3D.f0;{struct Cyc_List_List*x=Cyc_Tcutil_filter_nulls(_T3E);
# 1771
_TL1A5: if(x!=0)goto _TL1A3;else{goto _TL1A4;}
_TL1A3: _T3F=x;_T40=_T3F->hd;{struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)_T40;
if(vd==0)goto _TL1A6;_T41=env;_T42=am;_T43=
Cyc_Vcgen_myaddr(env,vd);_T44=_check_null(ts);_T45=_T44->hd;am=Cyc_Vcgen_do_assign(_T41,_T42,_T43,_T45);goto _TL1A7;_TL1A6: _TL1A7:;}_T46=x;
# 1771
x=_T46->tl;_T47=ts;ts=_T47->tl;goto _TL1A5;_TL1A4:;}goto _TL1A1;_TL1A0: _TL1A1: _T48=env;_T49=dest;_T4A=_T49->body;_T4B=am;
# 1777
Cyc_Vcgen_update_stmt_assnmap(_T48,_T4A,_T4B);_T4C=
Cyc_AssnDef_false_assnmap();return _T4C;}}_TL19F:;case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T94=_T97->f2;}{struct Cyc_Absyn_Decl*d=_T96;struct Cyc_Absyn_Stmt*s1=_T94;
# 1780
amin=Cyc_Vcgen_vcgen_local_decl(env,d,amin);
s=s1;goto LOOP;}case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T8E;_T94=_T97->f2;}{struct Cyc_Absyn_Stmt*s1=_T94;
# 1787
struct Cyc_Absyn_Exp*label_assn_opt=Cyc_Vcgen_find_assert_e(s1);
if(label_assn_opt!=0)goto _TL1A8;
# 1793
amin=Cyc_AssnDef_true_assnmap();goto _TL1A9;
# 1796
_TL1A8: _T4D=env;_T4E=label_assn_opt;{struct Cyc_AssnDef_AssnMap _T97;_T50=& Cyc_AssnDef_true_assn;_T51=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T50;_T97.assn=(void*)_T51;_T52=amin;_T97.map=_T52.map;_T4F=_T97;}{struct _tuple18 _T97=Cyc_Vcgen_vcgen_test(_T4D,_T4E,_T4F);struct Cyc_AssnDef_AssnMap _T98;_T98=_T97.f0;{struct Cyc_AssnDef_AssnMap amt=_T98;_T53=amin;_T54=_T53.assn;_T55=amt;_T56=_T55.assn;_T57=
Cyc_Vcgen_implies(_T54,_T56);if(_T57)goto _TL1AA;else{goto _TL1AC;}
_TL1AC:{struct Cyc_String_pa_PrintArg_struct _T99;_T99.tag=0;
_T99.f1=Cyc_AssnDef_assnmap2string(amin);_T58=_T99;}{struct Cyc_String_pa_PrintArg_struct _T99=_T58;{struct Cyc_String_pa_PrintArg_struct _T9A;_T9A.tag=0;_T9A.f1=Cyc_AssnDef_assnmap2string(amt);_T59=_T9A;}{struct Cyc_String_pa_PrintArg_struct _T9A=_T59;void*_T9B[2];_T9B[0]=& _T99;_T9B[1]=& _T9A;_T5A=s;_T5B=_T5A->loc;_T5C=
# 1798
_tag_fat("can not prove @assert(%s) \n from %s",sizeof(char),36U);_T5D=_tag_fat(_T9B,sizeof(void*),2);Cyc_Warn_err(_T5B,_T5C,_T5D);}}goto _TL1AB;_TL1AA: _TL1AB: _T5E=env;_T5F=label_assn_opt;_T60=
# 1803
Cyc_AssnDef_true_assnmap();{struct _tuple18 _T99=Cyc_Vcgen_vcgen_test(_T5E,_T5F,_T60);struct Cyc_AssnDef_AssnMap _T9A;_T9A=_T99.f0;{struct Cyc_AssnDef_AssnMap amt=_T9A;
amin=amt;}}}}_TL1A9:
# 1806
 s=s1;goto LOOP;}case 8: _T62=Cyc_Hashtable_lookup;{
# 1811
struct Cyc_Absyn_Stmt*(*_T97)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T62;_T61=_T97;}_T63=env;_T64=_T63->succ_table;_T65=s;{struct Cyc_Absyn_Stmt*destopt=_T61(_T64,_T65);
if(destopt==0)goto _TL1AD;_T66=destopt;{
void*_T97=_T66->r;struct Cyc_Absyn_Stmt*_T98;_T67=(int*)_T97;_T68=*_T67;if(_T68!=13)goto _TL1AF;{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T99=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T97;_T98=_T99->f2;}{struct Cyc_Absyn_Stmt*s1=_T98;
# 1815
struct Cyc_Absyn_Exp*label_assn_opt=Cyc_Vcgen_find_assert_e(s1);
if(label_assn_opt!=0)goto _TL1B1;goto _LL3A;_TL1B1: {
struct Cyc_Absyn_Exp*label_assn=label_assn_opt;_T69=env;_T6A=label_assn;{struct Cyc_AssnDef_AssnMap _T99;_T6C=& Cyc_AssnDef_true_assn;_T6D=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T6C;
_T99.assn=(void*)_T6D;_T6E=amin;_T99.map=_T6E.map;_T6B=_T99;}{struct _tuple18 _T99=Cyc_Vcgen_vcgen_test(_T69,_T6A,_T6B);struct Cyc_AssnDef_AssnMap _T9A;_T9A=_T99.f0;{struct Cyc_AssnDef_AssnMap amt=_T9A;_T6F=amin;_T70=_T6F.assn;_T71=amt;_T72=_T71.assn;_T73=
Cyc_Vcgen_implies(_T70,_T72);if(_T73)goto _TL1B3;else{goto _TL1B5;}
_TL1B5:{struct Cyc_String_pa_PrintArg_struct _T9B;_T9B.tag=0;_T75=amt;_T76=_T75.assn;
_T9B.f1=Cyc_AssnDef_assn2string(_T76);_T74=_T9B;}{struct Cyc_String_pa_PrintArg_struct _T9B=_T74;{struct Cyc_String_pa_PrintArg_struct _T9C;_T9C.tag=0;_T9C.f1=Cyc_AssnDef_assnmap2string(amin);_T77=_T9C;}{struct Cyc_String_pa_PrintArg_struct _T9C=_T77;void*_T9D[2];_T9D[0]=& _T9B;_T9D[1]=& _T9C;_T78=s;_T79=_T78->loc;_T7A=
# 1820
_tag_fat("cannot prove @assert(%s)\nfrom %s",sizeof(char),33U);_T7B=_tag_fat(_T9D,sizeof(void*),2);Cyc_Warn_err(_T79,_T7A,_T7B);}}goto _TL1B4;_TL1B3: _TL1B4: goto _LL3A;}}}}_TL1AF: _T7D=Cyc_Warn_impos;{
# 1823
int(*_T99)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7D;_T7C=_T99;}_T7E=_tag_fat("vcgen goto",sizeof(char),11U);_T7F=_tag_fat(0U,sizeof(void*),0);_T7C(_T7E,_T7F);_LL3A:;}goto _TL1AE;_TL1AD: _TL1AE: _T80=
# 1826
Cyc_AssnDef_false_assnmap();return _T80;}default:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T8E;_T94=_T97->f1;_T90=_T97->f2;_T81=_T97->f3;_T96=(void*)_T81;}{struct Cyc_Absyn_Stmt*s=_T94;struct Cyc_List_List*scs=_T90;void*dtree=_T96;_T82=env;{
# 1844 "vcgen.cyc"
struct Cyc_AssnDef_AssnMap*old_try_assnmap=_T82->try_assnmap;_T83=env;{struct Cyc_AssnDef_AssnMap*_T97=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));
# 1846
*_T97=Cyc_AssnDef_false_assnmap();_T84=(struct Cyc_AssnDef_AssnMap*)_T97;}_T83->try_assnmap=_T84;{
# 1849
struct Cyc_AssnDef_AssnMap am1=Cyc_Vcgen_vcgen_stmt(env,s,amin);_T85=env;_T86=_T85->try_assnmap;_T87=
# 1851
_check_null(_T86);{struct Cyc_AssnDef_AssnMap new_try_assnmap=*_T87;_T88=env;
_T88->try_assnmap=old_try_assnmap;_T89=
# 1855
Cyc_AssnDef_exception_vardecl();_T8A=am1;{struct _tuple14 _T97=Cyc_AssnDef_lookup_var_map(_T89,_T8A);struct Cyc_AssnDef_AssnMap _T98;void*_T99;_T99=_T97.f0;_T98=_T97.f1;{void*exc=_T99;struct Cyc_AssnDef_AssnMap am2=_T98;
struct Cyc_AssnDef_AssnMap am3=Cyc_Vcgen_vcgen_switch(env,exc,scs,new_try_assnmap);_T8B=env;_T8C=
Cyc_AssnDef_or_assnmap_assnmap(am1,am2);_T8D=Cyc_Vcgen_may_widen(_T8B,_T8C);return _T8D;}}}}}}};}}}struct _tuple25{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 1862
static struct _tuple18 Cyc_Vcgen_vcgen_pat(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Pat*p,void*lvalue,void*v,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct _tuple18 _T3;struct _tuple18 _T4;struct Cyc_Vcgen_Env*_T5;struct Cyc_AssnDef_AssnMap _T6;void*_T7;void*_T8;struct _tuple18 _T9;struct Cyc_Vcgen_Env*_TA;struct Cyc_AssnDef_AssnMap _TB;void*_TC;void*_TD;int(*_TE)(struct _fat_ptr,struct _fat_ptr);void*(*_TF)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T10;struct _fat_ptr _T11;struct _tuple18 _T12;struct Cyc_Vcgen_Env*_T13;struct Cyc_AssnDef_AssnMap _T14;void*_T15;void*_T16;struct _tuple18 _T17;struct Cyc_Vcgen_Env*_T18;struct Cyc_AssnDef_AssnMap _T19;void*_T1A;void*_T1B;struct _tuple18 _T1C;struct Cyc_AssnDef_AssnMap _T1D;void*_T1E;struct Cyc_AssnDef_AssnMap _T1F;void*_T20;int _T21;unsigned _T22;struct _tuple18 _T23;struct Cyc_AssnDef_AssnMap _T24;void*_T25;struct Cyc_AssnDef_AssnMap _T26;void*_T27;char _T28;unsigned _T29;struct _tuple18 _T2A;struct Cyc_AssnDef_AssnMap _T2B;void*_T2C;struct Cyc_AssnDef_AssnMap _T2D;void*_T2E;struct Cyc_Absyn_Exp*_T2F;int _T30;unsigned _T31;struct _tuple18 _T32;struct Cyc_AssnDef_AssnMap _T33;void*_T34;struct Cyc_AssnDef_AssnMap _T35;void*_T36;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T37;void*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Pat*_T3A;struct _tuple18 _T3B;struct Cyc_AssnDef_AssnMap _T3C;void*_T3D;struct Cyc_AssnDef_AssnMap _T3E;void*_T3F;void*_T40;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T41;void*_T42;struct Cyc_Absyn_Exp*_T43;struct Cyc_Absyn_Pat*_T44;struct _tuple18 _T45;struct Cyc_AssnDef_AssnMap _T46;void*_T47;struct Cyc_AssnDef_AssnMap _T48;void*_T49;void*_T4A;void*_T4B;struct Cyc_Absyn_Pat*_T4C;void*_T4D;struct _tuple18 _T4E;struct Cyc_AssnDef_AssnMap _T4F;void*_T50;void*_T51;void*_T52;struct Cyc_AssnDef_AssnMap _T53;void*_T54;void*_T55;void*_T56;void*_T57;struct Cyc_List_List*_T58;void*_T59;struct _tuple25*_T5A;struct Cyc_Absyn_Pat*_T5B;void*_T5C;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T5D;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T5E;void*_T5F;struct _tuple25*_T60;struct Cyc_List_List*_T61;int _T62;void*_T63;int _T64;unsigned _T65;void*_T66;struct Cyc_AssnDef_AssnMap _T67;void*_T68;void*_T69;int _T6A;unsigned _T6B;struct Cyc_Absyn_Pat*_T6C;void*_T6D;void*_T6E;int _T6F;unsigned _T70;struct Cyc_Vcgen_Env*_T71;struct Cyc_AssnDef_AssnMap _T72;struct Cyc_List_List*_T73;struct _tuple18 _T74;unsigned _T75;void*_T76;int _T77;unsigned _T78;void*_T79;struct Cyc_List_List*_T7A;void*_T7B;void*_T7C;unsigned _T7D;struct Cyc_Absyn_Pat*_T7E;void*_T7F;struct Cyc_Vcgen_Env*_T80;struct Cyc_AssnDef_AssnMap _T81;struct Cyc_List_List*_T82;struct Cyc_AssnDef_AssnMap _T83;void*_T84;struct _tuple18 _T85;int(*_T86)(struct _fat_ptr,struct _fat_ptr);void*(*_T87)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T88;struct _fat_ptr _T89;_T0=p;{
# 1864
void*_T8A=_T0->r;struct Cyc_Absyn_Datatypefield*_T8B;struct Cyc_Absyn_Datatypedecl*_T8C;struct Cyc_List_List*_T8D;struct Cyc_Absyn_Enumfield*_T8E;struct _fat_ptr _T8F;char _T90;int _T91;enum Cyc_Absyn_Sign _T92;struct Cyc_Absyn_Vardecl*_T93;struct Cyc_Absyn_Pat*_T94;void*_T95;_T1=(int*)_T8A;_T2=*_T1;switch(_T2){case 0:{struct _tuple18 _T96;
# 1866
_T96.f0=amin;_T96.f1=Cyc_AssnDef_false_assnmap();_T3=_T96;}return _T3;case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;_T94=_T96->f2;}{struct Cyc_Absyn_Vardecl*vd=_T95;struct Cyc_Absyn_Pat*p2=_T94;
# 1868
struct _tuple18 _T96=Cyc_Vcgen_vcgen_pat(env,p2,lvalue,v,amin);struct Cyc_AssnDef_AssnMap _T97;struct Cyc_AssnDef_AssnMap _T98;_T98=_T96.f0;_T97=_T96.f1;{struct Cyc_AssnDef_AssnMap amt=_T98;struct Cyc_AssnDef_AssnMap amf=_T97;{struct _tuple18 _T99;_T5=env;_T6=amt;_T7=
Cyc_Vcgen_myaddr(env,vd);_T8=v;_T99.f0=Cyc_Vcgen_do_assign(_T5,_T6,_T7,_T8);_T99.f1=amf;_T4=_T99;}return _T4;}}case 2:{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;_T93=_T96->f2;}{struct Cyc_Absyn_Tvar*tv=_T95;struct Cyc_Absyn_Vardecl*vd=_T93;{struct _tuple18 _T96;_TA=env;_TB=amin;_TC=
# 1871
Cyc_Vcgen_myaddr(env,vd);_TD=v;_T96.f0=Cyc_Vcgen_do_assign(_TA,_TB,_TC,_TD);_T96.f1=Cyc_AssnDef_false_assnmap();_T9=_T96;}return _T9;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;_T94=_T96->f2;}{struct Cyc_Absyn_Vardecl*vd=_T95;struct Cyc_Absyn_Pat*p2=_T94;
# 1873
struct _tuple18 _T96=Cyc_Vcgen_vcgen_pat(env,p2,lvalue,v,amin);struct Cyc_AssnDef_AssnMap _T97;struct Cyc_AssnDef_AssnMap _T98;_T98=_T96.f0;_T97=_T96.f1;{struct Cyc_AssnDef_AssnMap amt=_T98;struct Cyc_AssnDef_AssnMap amf=_T97;
if(lvalue!=0)goto _TL1B7;_TF=Cyc_Warn_impos;{int(*_T99)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF;_TE=_T99;}_T10=_tag_fat("vcgen reference pattern matching",sizeof(char),33U);_T11=_tag_fat(0U,sizeof(void*),0);_TE(_T10,_T11);goto _TL1B8;_TL1B7: _TL1B8:{struct _tuple18 _T99;_T13=env;_T14=amt;_T15=
Cyc_Vcgen_myaddr(env,vd);_T16=lvalue;_T99.f0=Cyc_Vcgen_do_assign(_T13,_T14,_T15,_T16);_T99.f1=amf;_T12=_T99;}return _T12;}}case 4:{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f2;}{struct Cyc_Absyn_Vardecl*vd=_T95;{struct _tuple18 _T96;_T18=env;_T19=amin;_T1A=
# 1877
Cyc_Vcgen_myaddr(env,vd);_T1B=v;_T96.f0=Cyc_Vcgen_do_assign(_T18,_T19,_T1A,_T1B);_T96.f1=Cyc_AssnDef_false_assnmap();_T17=_T96;}return _T17;}case 8:  {
# 1879
void*t=Cyc_AssnDef_zero();{struct _tuple18 _T96;_T1D=amin;_T1E=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T1D,_T1E);_T1F=amin;_T20=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T1F,_T20);_T1C=_T96;}return _T1C;}case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T8A;_T92=_T96->f1;_T91=_T96->f2;}{enum Cyc_Absyn_Sign s=_T92;int i=_T91;_T21=i;_T22=(unsigned)_T21;{
# 1882
void*t=Cyc_AssnDef_uint(_T22);{struct _tuple18 _T96;_T24=amin;_T25=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T24,_T25);_T26=amin;_T27=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T26,_T27);_T23=_T96;}return _T23;}}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T8A;_T90=_T96->f1;}{char c=_T90;_T28=c;_T29=(unsigned)_T28;{
# 1885
void*t=Cyc_AssnDef_uint(_T29);{struct _tuple18 _T96;_T2B=amin;_T2C=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T2B,_T2C);_T2D=amin;_T2E=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T2D,_T2E);_T2A=_T96;}return _T2A;}}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T8A;_T8F=_T96->f1;_T91=_T96->f2;}{struct _fat_ptr f=_T8F;int i=_T91;
# 1888
struct Cyc_Absyn_Exp*e=Cyc_Absyn_float_exp(f,i,0U);_T2F=e;_T30=i;_T31=(unsigned)_T30;
_T2F->topt=Cyc_Absyn_gen_float_type(_T31);{
void*t=Cyc_AssnDef_cnst(e);{struct _tuple18 _T96;_T33=amin;_T34=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T33,_T34);_T35=amin;_T36=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T35,_T36);_T32=_T96;}return _T32;}}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;_T8E=_T96->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T95;struct Cyc_Absyn_Enumfield*ef=_T8E;{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T96=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_T96->tag=31;
# 1893
_T96->f1=ed;_T96->f2=ef;_T37=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_T96;}_T38=(void*)_T37;{struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp(_T38,0U);_T39=e;_T3A=p;
_T39->topt=_T3A->topt;{
void*t=Cyc_AssnDef_cnst(e);{struct _tuple18 _T96;_T3C=amin;_T3D=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T3C,_T3D);_T3E=amin;_T3F=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T3E,_T3F);_T3B=_T96;}return _T3B;}}}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T8A;_T40=_T96->f1;_T95=(void*)_T40;_T8E=_T96->f2;}{void*type=_T95;struct Cyc_Absyn_Enumfield*ef=_T8E;{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T96=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_T96->tag=32;
# 1898
_T96->f1=type;_T96->f2=ef;_T41=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_T96;}_T42=(void*)_T41;{struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp(_T42,0U);_T43=e;_T44=p;
_T43->topt=_T44->topt;{
void*t=Cyc_AssnDef_cnst(e);{struct _tuple18 _T96;_T46=amin;_T47=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T46,_T47);_T48=amin;_T49=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T48,_T49);_T45=_T96;}return _T45;}}}case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;}{struct Cyc_Absyn_Pat*p2=_T95;
# 1904
struct _tuple14 _T96=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T97;void*_T98;_T98=_T96.f0;_T97=_T96.f1;{void*mem=_T98;struct Cyc_AssnDef_AssnMap amin=_T97;_T4A=mem;_T4B=v;_T4C=p;_T4D=_T4C->topt;{
void*v2=Cyc_AssnDef_select(_T4A,_T4B,_T4D);
struct _tuple18 _T99=Cyc_Vcgen_vcgen_pat(env,p2,v,v2,amin);struct Cyc_AssnDef_AssnMap _T9A;struct Cyc_AssnDef_AssnMap _T9B;_T9B=_T99.f0;_T9A=_T99.f1;{struct Cyc_AssnDef_AssnMap amt=_T9B;struct Cyc_AssnDef_AssnMap amf=_T9A;{struct _tuple18 _T9C;_T4F=amt;_T50=
# 1908
Cyc_AssnDef_zero();_T51=v;_T52=Cyc_AssnDef_neq(_T50,_T51);_T9C.f0=Cyc_AssnDef_and_assnmap_assn(_T4F,_T52);_T53=amf;_T54=Cyc_AssnDef_zero();_T55=v;_T56=Cyc_AssnDef_eq(_T54,_T55);_T9C.f1=Cyc_AssnDef_or_assnmap_assn(_T53,_T56);_T4E=_T9C;}return _T4E;}}}}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T8A;_T57=_T96->f1;_T95=(void*)_T57;_T8D=_T96->f4;}{void*topt=_T95;struct Cyc_List_List*dlps=_T8D;
# 1910
void*type=_check_null(topt);
struct _tuple20 _T96=Cyc_Vcgen_get_aggr_info(type);struct Cyc_List_List*_T97;int _T98;_T98=_T96.f0;_T97=_T96.f2;{int is_tagged=_T98;struct Cyc_List_List*fields=_T97;
struct Cyc_AssnDef_AssnMap amt=amin;
struct Cyc_AssnDef_AssnMap amf=Cyc_AssnDef_false_assnmap();
_TL1BC: if(dlps!=0)goto _TL1BA;else{goto _TL1BB;}
_TL1BA: _T58=dlps;_T59=_T58->hd;{struct _tuple25*tup=(struct _tuple25*)_T59;_T5A=tup;{
struct Cyc_Absyn_Pat*p2=_T5A->f1;_T5B=p2;_T5C=_T5B->r;_T5D=& Cyc_Absyn_Wild_p_val;_T5E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T5D;_T5F=(void*)_T5E;
if(_T5C!=_T5F)goto _TL1BD;goto _TL1B9;_TL1BD: _T60=tup;_T61=_T60->f0;{
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(_T61);
int i=Cyc_CfFlowInfo_get_field_index(type,f);_T62=is_tagged;
if(!_T62)goto _TL1BF;_T63=
# 1923
Cyc_AssnDef_tagof_tm(v);_T64=i;_T65=(unsigned)_T64;_T66=Cyc_AssnDef_uint(_T65);{void*tag_check=Cyc_AssnDef_eq(_T63,_T66);
amt=Cyc_AssnDef_and_assnmap_assn(amt,tag_check);_T67=amf;_T68=
Cyc_AssnDef_not(tag_check);amf=Cyc_AssnDef_or_assnmap_assn(_T67,_T68);}goto _TL1C0;_TL1BF: _TL1C0: _T69=v;_T6A=i;_T6B=(unsigned)_T6A;_T6C=p2;_T6D=_T6C->topt;{
# 1927
void*member=Cyc_AssnDef_proj(_T69,_T6B,_T6D);
void*path=0;
if(lvalue==0)goto _TL1C1;_T6E=lvalue;_T6F=i;_T70=(unsigned)_T6F;
path=Cyc_AssnDef_offsetf(_T6E,_T70,0);goto _TL1C2;_TL1C1: _TL1C2: {
struct _tuple18 _T99=Cyc_Vcgen_vcgen_pat(env,p2,path,member,amt);struct Cyc_AssnDef_AssnMap _T9A;struct Cyc_AssnDef_AssnMap _T9B;_T9B=_T99.f0;_T9A=_T99.f1;{struct Cyc_AssnDef_AssnMap amt2=_T9B;struct Cyc_AssnDef_AssnMap amf2=_T9A;_T71=env;_T72=
Cyc_AssnDef_or_assnmap_assnmap(amf,amf2);amf=Cyc_Vcgen_may_widen(_T71,_T72);
amt=amt2;}}}}}}_TL1B9: _T73=dlps;
# 1914
dlps=_T73->tl;goto _TL1BC;_TL1BB:{struct _tuple18 _T99;
# 1935
_T99.f0=amt;_T99.f1=amf;_T74=_T99;}return _T74;}}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T8A;_T8C=_T96->f1;_T8B=_T96->f2;_T8D=_T96->f3;}{struct Cyc_Absyn_Datatypedecl*dd=_T8C;struct Cyc_Absyn_Datatypefield*df=_T8B;struct Cyc_List_List*ps=_T8D;_T75=
# 1939
Cyc_Vcgen_datatype_constructor_index(dd,df);{int i=(int)_T75;_T76=
Cyc_AssnDef_tagof_tm(v);_T77=i;_T78=(unsigned)_T77;_T79=Cyc_AssnDef_uint(_T78);{void*tag_check=Cyc_AssnDef_eq(_T76,_T79);
struct Cyc_AssnDef_AssnMap amt=Cyc_AssnDef_and_assnmap_assn(amin,tag_check);
struct Cyc_AssnDef_AssnMap amf=Cyc_AssnDef_false_assnmap();{
unsigned j=0U;_TL1C6: if(ps!=0)goto _TL1C4;else{goto _TL1C5;}
_TL1C4: _T7A=ps;_T7B=_T7A->hd;{struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)_T7B;_T7C=v;_T7D=j;_T7E=p2;_T7F=_T7E->topt;{
void*member=Cyc_AssnDef_proj(_T7C,_T7D,_T7F);
void*path=0;
if(lvalue==0)goto _TL1C7;
path=Cyc_AssnDef_offsetf(lvalue,j,0);goto _TL1C8;_TL1C7: _TL1C8: {
# 1950
struct _tuple18 _T96=Cyc_Vcgen_vcgen_pat(env,p2,path,member,amt);struct Cyc_AssnDef_AssnMap _T97;struct Cyc_AssnDef_AssnMap _T98;_T98=_T96.f0;_T97=_T96.f1;{struct Cyc_AssnDef_AssnMap amt2=_T98;struct Cyc_AssnDef_AssnMap amf2=_T97;_T80=env;_T81=
Cyc_AssnDef_or_assnmap_assnmap(amf,amf2);amf=Cyc_Vcgen_may_widen(_T80,_T81);
amt=amt2;}}}}_T82=ps;
# 1943
ps=_T82->tl;j=j + 1;goto _TL1C6;_TL1C5:;}_T83=amf;_T84=
# 1954
Cyc_AssnDef_not(tag_check);amf=Cyc_AssnDef_or_assnmap_assn(_T83,_T84);{struct _tuple18 _T96;
_T96.f0=amt;_T96.f1=amf;_T85=_T96;}return _T85;}}}case 14: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22: _T87=Cyc_Warn_impos;{
# 1958
int(*_T96)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T87;_T86=_T96;}_T88=_tag_fat("vcgen pat found bad pattern",sizeof(char),28U);_T89=_tag_fat(0U,sizeof(void*),0);_T86(_T88,_T89);};}}
# 1962
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,void*v,struct Cyc_List_List*scs,struct Cyc_AssnDef_AssnMap amin){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Vcgen_Env*_T2;struct Cyc_Absyn_Switch_clause*_T3;struct Cyc_Absyn_Pat*_T4;void*_T5;struct Cyc_AssnDef_AssnMap _T6;struct Cyc_Absyn_Switch_clause*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_Absyn_Switch_clause*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_AssnDef_AssnMap _TC;struct Cyc_Vcgen_Env*_TD;struct Cyc_AssnDef_AssnMap _TE;struct Cyc_Vcgen_Env*_TF;struct Cyc_AssnDef_AssnMap _T10;struct Cyc_Vcgen_Env*_T11;struct Cyc_Absyn_Switch_clause*_T12;struct Cyc_Absyn_Stmt*_T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_AssnMap _T15;struct Cyc_AssnDef_AssnMap _T16;struct Cyc_Vcgen_Env*_T17;int _T18;struct Cyc_List_List*_T19;struct Cyc_AssnDef_AssnMap _T1A;
# 1967
struct Cyc_AssnDef_AssnMap res=Cyc_AssnDef_false_assnmap();
_TL1CC: if(scs!=0)goto _TL1CA;else{goto _TL1CB;}
_TL1CA: _T0=scs;_T1=_T0->hd;{struct Cyc_Absyn_Switch_clause*sc=(struct Cyc_Absyn_Switch_clause*)_T1;_T2=env;_T3=sc;_T4=_T3->pattern;_T5=v;_T6=amin;{
struct _tuple18 _T1B=Cyc_Vcgen_vcgen_pat(_T2,_T4,0,_T5,_T6);struct Cyc_AssnDef_AssnMap _T1C;struct Cyc_AssnDef_AssnMap _T1D;_T1D=_T1B.f0;_T1C=_T1B.f1;{struct Cyc_AssnDef_AssnMap amt=_T1D;struct Cyc_AssnDef_AssnMap amf=_T1C;_T7=sc;_T8=_T7->where_clause;
if(_T8==0)goto _TL1CD;_T9=env;_TA=sc;_TB=_TA->where_clause;_TC=amt;{
struct _tuple18 _T1E=Cyc_Vcgen_vcgen_test(_T9,_TB,_TC);struct Cyc_AssnDef_AssnMap _T1F;struct Cyc_AssnDef_AssnMap _T20;_T20=_T1E.f0;_T1F=_T1E.f1;{struct Cyc_AssnDef_AssnMap amt2=_T20;struct Cyc_AssnDef_AssnMap amf2=_T1F;
amt=amt2;_TD=env;_TE=
Cyc_AssnDef_or_assnmap_assnmap(amf,amf2);amf=Cyc_Vcgen_may_widen(_TD,_TE);}}goto _TL1CE;_TL1CD: _TL1CE: _TF=env;_T10=res;_T11=env;_T12=sc;_T13=_T12->body;_T14=amt;_T15=
# 1976
Cyc_Vcgen_vcgen_stmt(_T11,_T13,_T14);_T16=Cyc_AssnDef_or_assnmap_assnmap(_T10,_T15);res=Cyc_Vcgen_may_widen(_TF,_T16);_T17=env;_T18=_T17->widen_paths;
if(_T18)goto _TL1CF;else{goto _TL1D1;}
_TL1D1: amin=amf;goto _TL1D0;_TL1CF: _TL1D0:;}}}_T19=scs;
# 1968
scs=_T19->tl;goto _TL1CC;_TL1CB: _T1A=res;
# 1980
return _T1A;}
# 1983
struct Cyc_AssnDef_AssnMap Cyc_Vcgen_clause2assn(struct Cyc_Absyn_Exp*e){struct _tuple18 _T0;struct Cyc_AssnDef_AssnMap _T1;
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_bogus_env();
# 1986
struct Cyc_AssnDef_AssnMap am=Cyc_AssnDef_true_assnmap();
struct _tuple14 _T2=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _T3;_T3=_T2.f1;{struct Cyc_AssnDef_AssnMap am=_T3;_T0=
Cyc_Vcgen_vcgen_test(env,e,am);{struct Cyc_AssnDef_AssnMap res=_T0.f0;_T1=res;
return _T1;}}}
# 2098 "vcgen.cyc"
void Cyc_Vcgen_vcgen_fundecl(unsigned loc,struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,struct Cyc_Hashtable_Table*fn_precond_info,int use_precond){struct Cyc_Absyn_Fndecl*_T0;struct Cyc_Absyn_FnInfo*_T1;struct Cyc_Absyn_FnInfo*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_AssnDef_AssnMap _TA;struct Cyc_AssnDef_AssnMap _TB;struct Cyc_AssnDef_AssnMap _TC;struct Cyc_AssnDef_AssnMap _TD;struct Cyc_AssnDef_AssnMap*_TE;struct Cyc_AssnDef_AssnMap*_TF;struct _tuple2*_T10;struct _tuple2*_T11;struct _fat_ptr*_T12;struct _tuple2*_T13;struct Cyc_Absyn_Fndecl*_T14;struct Cyc_Absyn_FnInfo _T15;void*_T16;struct Cyc_Vcgen_Env*_T17;struct Cyc_Absyn_Fndecl*_T18;struct Cyc_Absyn_Stmt*_T19;struct Cyc_AssnDef_AssnMap _T1A;struct Cyc_Vcgen_Env*_T1B;struct Cyc_AssnDef_AssnMap*_T1C;struct Cyc_AssnDef_AssnMap*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_AssnDef_ExistAssnFn*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;struct Cyc_AssnDef_AssnMap _T23;void*_T24;void*_T25;int _T26;struct Cyc_String_pa_PrintArg_struct _T27;struct Cyc_String_pa_PrintArg_struct _T28;struct Cyc_Vcgen_Env*_T29;struct Cyc_AssnDef_AssnMap*_T2A;struct Cyc_AssnDef_AssnMap*_T2B;struct Cyc_AssnDef_AssnMap _T2C;unsigned _T2D;struct _fat_ptr _T2E;struct _fat_ptr _T2F;struct Cyc_Absyn_Fndecl*_T30;struct Cyc_List_List*_T31;void*_T32;void*_T33;struct Cyc_AssnDef_AssnMap _T34;void*_T35;struct Cyc_Vcgen_Env*_T36;struct Cyc_AssnDef_AssnMap*_T37;struct Cyc_AssnDef_AssnMap*_T38;struct Cyc_AssnDef_ExistAssnFn*_T39;struct Cyc_List_List*_T3A;struct Cyc_AssnDef_AssnMap _T3B;void*_T3C;void*_T3D;int _T3E;struct Cyc_String_pa_PrintArg_struct _T3F;struct Cyc_String_pa_PrintArg_struct _T40;struct Cyc_Vcgen_Env*_T41;struct Cyc_AssnDef_AssnMap*_T42;struct Cyc_AssnDef_AssnMap*_T43;struct Cyc_AssnDef_AssnMap _T44;unsigned _T45;struct _fat_ptr _T46;struct _fat_ptr _T47;_T0=fd;_T1=& _T0->i;_T2=(struct Cyc_Absyn_FnInfo*)_T1;{
# 2104
struct _tuple19 _T48=Cyc_Vcgen_get_requires_and_ensures_info(_T2);struct Cyc_Absyn_Vardecl*_T49;struct Cyc_AssnDef_ExistAssnFn*_T4A;struct Cyc_AssnDef_ExistAssnFn*_T4B;struct Cyc_AssnDef_ExistAssnFn*_T4C;struct Cyc_AssnDef_ExistAssnFn*_T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;_T4F=_T48.f0;_T4E=_T48.f1;_T4D=_T48.f2;_T4C=_T48.f3;_T4B=_T48.f4;_T4A=_T48.f5;_T49=_T48.f6;{struct Cyc_List_List*args=_T4F;struct Cyc_List_List*arvds=_T4E;struct Cyc_AssnDef_ExistAssnFn*checkopt=_T4D;struct Cyc_AssnDef_ExistAssnFn*reqopt=_T4C;struct Cyc_AssnDef_ExistAssnFn*ensopt=_T4B;struct Cyc_AssnDef_ExistAssnFn*thrwsopt=_T4A;struct Cyc_Absyn_Vardecl*ret_value=_T49;
struct Cyc_AssnDef_AssnMap am=Cyc_AssnDef_true_assnmap();
# 2110
struct Cyc_List_List*actuals=0;{
struct Cyc_List_List*as=arvds;_TL1D5: if(as!=0)goto _TL1D3;else{goto _TL1D4;}
_TL1D3: _T3=as;_T4=_T3->hd;_T5=(struct Cyc_Absyn_Vardecl*)_T4;{struct Cyc_Absyn_Vardecl*vd=_check_null(_T5);
struct _tuple14 _T50=Cyc_AssnDef_lookup_var_map(vd,am);struct Cyc_AssnDef_AssnMap _T51;void*_T52;_T52=_T50.f0;_T51=_T50.f1;{void*actual=_T52;struct Cyc_AssnDef_AssnMap am2=_T51;
am=am2;{struct Cyc_List_List*_T53=_cycalloc(sizeof(struct Cyc_List_List));
_T53->hd=actual;_T53->tl=actuals;_T6=(struct Cyc_List_List*)_T53;}actuals=_T6;}}_T7=as;
# 2111
as=_T7->tl;goto _TL1D5;_TL1D4:;}
# 2117
actuals=Cyc_List_imp_rev(actuals);{
# 2119
struct _tuple14 _T50=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _T51;void*_T52;_T52=_T50.f0;_T51=_T50.f1;{void*initmem=_T52;struct Cyc_AssnDef_AssnMap am2=_T51;
struct Cyc_AssnDef_AssnMap _T53=Cyc_AssnDef_update_var_map(Cyc_AssnDef_pre_memory,initmem,am2);{struct Cyc_AssnDef_AssnMap am2=_T53;
am=am2;{
# 2123
struct Cyc_List_List*args;args=_cycalloc(sizeof(struct Cyc_List_List));_T8=args;_T8->hd=initmem;_T9=args;_T9->tl=actuals;
# 2125
if(checkopt==0)goto _TL1D6;{struct Cyc_AssnDef_AssnMap _T54;
_T54.assn=Cyc_AssnDef_existassnfn2assn(checkopt,args);_TB=am;_T54.map=_TB.map;_TA=_T54;}am=_TA;goto _TL1D7;_TL1D6: _TL1D7:
# 2129
 if(reqopt==0)goto _TL1D8;{struct Cyc_AssnDef_AssnMap _T54;
_T54.assn=Cyc_AssnDef_existassnfn2assn(reqopt,args);_TD=am;_T54.map=_TD.map;_TC=_T54;}am=_TC;goto _TL1D9;_TL1D8: _TL1D9: {
# 2157 "vcgen.cyc"
struct Cyc_AssnDef_AssnMap*thrws_assn=0;
if(thrwsopt==0)goto _TL1DA;{struct Cyc_AssnDef_AssnMap*_T54=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));
*_T54=Cyc_AssnDef_false_assnmap();_TE=(struct Cyc_AssnDef_AssnMap*)_T54;}thrws_assn=_TE;goto _TL1DB;_TL1DA: _TL1DB: {
# 2162
struct Cyc_AssnDef_AssnMap*result_assn;result_assn=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));_TF=result_assn;*_TF=Cyc_AssnDef_false_assnmap();
if(ret_value!=0)goto _TL1DC;{
struct _tuple2*v;v=_cycalloc(sizeof(struct _tuple2));_T10=v;_T10->f0=Cyc_Absyn_Loc_n();_T11=v;{struct _fat_ptr*_T54=_cycalloc(sizeof(struct _fat_ptr));*_T54=_tag_fat("return_value",sizeof(char),13U);_T12=(struct _fat_ptr*)_T54;}_T11->f1=_T12;_T13=v;_T14=fd;_T15=_T14->i;_T16=_T15.ret_type;
ret_value=Cyc_Absyn_new_vardecl(0U,_T13,_T16,0,0);}goto _TL1DD;_TL1DC: _TL1DD: {
# 2167
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_initial_env(tables,fd,assn_info,fn_precond_info,result_assn,thrws_assn,ret_value);_T17=env;_T18=fd;_T19=_T18->body;_T1A=am;{
# 2170
struct Cyc_AssnDef_AssnMap amout=Cyc_Vcgen_vcgen_stmt(_T17,_T19,_T1A);
# 2173
Cyc_Vcgen_update_return(env,0,amout,0);_T1B=env;_T1C=_T1B->res_assnmap;_T1D=
# 2176
_check_null(_T1C);{struct Cyc_AssnDef_AssnMap res=*_T1D;
void*res_term=Cyc_AssnDef_fresh_var(0,0);
if(ret_value==0)goto _TL1DE;{
struct _tuple14 _T54=Cyc_AssnDef_lookup_var_map(ret_value,res);struct Cyc_AssnDef_AssnMap _T55;void*_T56;_T56=_T54.f0;_T55=_T54.f1;{void*r=_T56;struct Cyc_AssnDef_AssnMap a=_T55;
res_term=r;
res=a;}}goto _TL1DF;_TL1DE: _TL1DF: {
# 2183
struct _tuple14 _T54=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,res);struct Cyc_AssnDef_AssnMap _T55;void*_T56;_T56=_T54.f0;_T55=_T54.f1;{void*new_memory=_T56;struct Cyc_AssnDef_AssnMap res=_T55;
struct _tuple14 _T57=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_pre_memory,res);struct Cyc_AssnDef_AssnMap _T58;void*_T59;_T59=_T57.f0;_T58=_T57.f1;{void*pre_mem=_T59;struct Cyc_AssnDef_AssnMap res=_T58;
struct Cyc_List_List*pre_memory_and_actuals;pre_memory_and_actuals=_cycalloc(sizeof(struct Cyc_List_List));_T1E=pre_memory_and_actuals;_T1E->hd=pre_mem;_T1F=pre_memory_and_actuals;_T1F->tl=actuals;
if(ensopt==0)goto _TL1E0;_T20=ensopt;{struct Cyc_List_List*_T5A=_cycalloc(sizeof(struct Cyc_List_List));
_T5A->hd=res_term;{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));_T5B->hd=new_memory;_T5B->tl=pre_memory_and_actuals;_T22=(struct Cyc_List_List*)_T5B;}_T5A->tl=_T22;_T21=(struct Cyc_List_List*)_T5A;}{void*ens=Cyc_AssnDef_existassnfn2assn(_T20,_T21);_T23=res;_T24=_T23.assn;_T25=ens;_T26=
Cyc_Vcgen_implies(_T24,_T25);if(_T26)goto _TL1E2;else{goto _TL1E4;}
_TL1E4:{struct Cyc_String_pa_PrintArg_struct _T5A;_T5A.tag=0;
_T5A.f1=Cyc_AssnDef_assn2string(ens);_T27=_T5A;}{struct Cyc_String_pa_PrintArg_struct _T5A=_T27;{struct Cyc_String_pa_PrintArg_struct _T5B;_T5B.tag=0;_T29=env;_T2A=_T29->res_assnmap;_T2B=
_check_null(_T2A);_T2C=*_T2B;_T5B.f1=Cyc_AssnDef_assnmap2string(_T2C);_T28=_T5B;}{struct Cyc_String_pa_PrintArg_struct _T5B=_T28;void*_T5C[2];_T5C[0]=& _T5A;_T5C[1]=& _T5B;_T2D=loc;_T2E=
# 2189
_tag_fat("cannot prove the @ensures clause\n\t%s\nfrom\n\t%s",sizeof(char),46U);_T2F=_tag_fat(_T5C,sizeof(void*),2);Cyc_Warn_err(_T2D,_T2E,_T2F);}}goto _TL1E3;_TL1E2: _TL1E3:;}goto _TL1E1;
# 2195
_TL1E0: _T30=fd;_T31=pre_memory_and_actuals;_T32=res_term;_T33=new_memory;_T34=res;_T35=_T34.assn;Cyc_Vcgen_insert_fn_info(_T30,_T31,_T32,_T33,_T35);_TL1E1:
# 2212 "vcgen.cyc"
 if(thrwsopt==0)goto _TL1E5;_T36=env;_T37=_T36->try_assnmap;_T38=
_check_null(_T37);{struct Cyc_AssnDef_AssnMap res=*_T38;
# 2215
struct _tuple14 _T5A=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,res);struct Cyc_AssnDef_AssnMap _T5B;void*_T5C;_T5C=_T5A.f0;_T5B=_T5A.f1;{void*new_memory=_T5C;struct Cyc_AssnDef_AssnMap res=_T5B;_T39=thrwsopt;{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));
_T5D->hd=new_memory;_T5D->tl=pre_memory_and_actuals;_T3A=(struct Cyc_List_List*)_T5D;}{void*thr=Cyc_AssnDef_existassnfn2assn(_T39,_T3A);_T3B=res;_T3C=_T3B.assn;_T3D=thr;_T3E=
Cyc_Vcgen_implies(_T3C,_T3D);if(_T3E)goto _TL1E7;else{goto _TL1E9;}
_TL1E9:{struct Cyc_String_pa_PrintArg_struct _T5D;_T5D.tag=0;
_T5D.f1=Cyc_AssnDef_assn2string(thr);_T3F=_T5D;}{struct Cyc_String_pa_PrintArg_struct _T5D=_T3F;{struct Cyc_String_pa_PrintArg_struct _T5E;_T5E.tag=0;_T41=env;_T42=_T41->try_assnmap;_T43=
_check_null(_T42);_T44=*_T43;_T5E.f1=Cyc_AssnDef_assnmap2string(_T44);_T40=_T5E;}{struct Cyc_String_pa_PrintArg_struct _T5E=_T40;void*_T5F[2];_T5F[0]=& _T5D;_T5F[1]=& _T5E;_T45=loc;_T46=
# 2218
_tag_fat("cannot prove the @throws clause\n\t%s\nfrom\n\t%s",sizeof(char),45U);_T47=_tag_fat(_T5F,sizeof(void*),2);Cyc_Warn_err(_T45,_T46,_T47);}}goto _TL1E8;_TL1E7: _TL1E8:;}}}goto _TL1E6;_TL1E5: _TL1E6:;}}}}}}}}}}}}}}}

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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 531
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 563
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;int f3;};struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 731 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 918
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
int Cyc_Absyn_hash_qvar(struct _tuple2*);
# 926
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
# 933
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 939
void*Cyc_Absyn_compress(void*);
# 957
extern void*Cyc_Absyn_uint_type;
# 959
extern void*Cyc_Absyn_sint_type;
# 962
void*Cyc_Absyn_gen_float_type(unsigned);
# 966
extern void*Cyc_Absyn_heap_rgn_type;
# 968
extern void*Cyc_Absyn_al_qual_type;
# 972
extern void*Cyc_Absyn_false_type;
# 997
void*Cyc_Absyn_exn_type (void);
# 1009
void*Cyc_Absyn_bounds_one (void);
# 1023
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
# 1119
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1168
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple2*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1229
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1238
struct _fat_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1242
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
# 313
extern int Cyc_AssnDef_simple_prove(void*,void*);
# 352 "assndef.h"
extern void*Cyc_AssnDef_widen(void*);
# 367
extern struct Cyc_Set_Set*Cyc_AssnDef_assn_fr_logicvar(void*);
# 371
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
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
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
# 402 "vcgen.cyc"
static int Cyc_Vcgen_simple_implies(void*a1,void*a2){int _T0;_T0=
Cyc_AssnDef_simple_prove(a1,a2);return _T0;}
# 409
static int Cyc_Vcgen_implies(void*a1,void*a2){int _T0;int _T1;_T1=
Cyc_Vcgen_simple_implies(a1,a2);if(!_T1)goto _TL1B;_T0=1;goto _TL1C;_TL1B: _T0=Cyc_PrattProver_constraint_prove(a1,a2);_TL1C: return _T0;}
# 423 "vcgen.cyc"
static struct Cyc_Absyn_Vardecl*Cyc_Vcgen_bind2vardecl(void*b){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;void*_T5;struct Cyc_Absyn_Fndecl*_T6;struct Cyc_Absyn_Vardecl*_T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr);void*(*_T9)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TA;struct _fat_ptr _TB;void*_TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;struct Cyc_Absyn_Vardecl*_TF;void*_T10;struct Cyc_Absyn_Vardecl*_T11;struct Cyc_Absyn_Fndecl*_T12;struct Cyc_Absyn_Vardecl*_T13;_T0=b;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 1: _T3=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T3;_T13=_T14->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13;_T4=vd;
# 425
return _T4;}case 2: _T5=b;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T5;_T12=_T14->f1;}{struct Cyc_Absyn_Fndecl*fd=_T12;_T6=fd;_T7=_T6->fn_vardecl;
return _T7;}case 0: _T9=Cyc_Warn_impos;{
int(*_T14)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T9;_T8=_T14;}_TA=_tag_fat("vcgen bind2vardecl",sizeof(char),19U);_TB=_tag_fat(0U,sizeof(void*),0);_T8(_TA,_TB);case 3: _TC=b;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_TC;_T13=_T14->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13;_TD=vd;
return _TD;}case 4: _TE=b;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TE;_T13=_T14->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13;_TF=vd;
return _TF;}default: _T10=b;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T14=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T10;_T13=_T14->f1;}{struct Cyc_Absyn_Vardecl*vd=_T13;_T11=vd;
return _T11;}};}
# 435
static int Cyc_Vcgen_is_unsigned_int_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;int _T8;void*_T9;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA;void*_TB;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TC;enum Cyc_Absyn_Sign _TD;void*_TE;void*_TF;enum Cyc_Absyn_Size_of _T10;int _T11;enum Cyc_Absyn_Size_of _T12;int _T13;_T0=e;{
void*t=_T0->topt;
if(t!=0)goto _TL1E;return 0;_TL1E: {enum Cyc_Absyn_Size_of _T14;if(t==0)goto _TL20;_T1=t;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL22;_T4=t;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=1)goto _TL24;_T9=t;_TA=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T9;_TB=_TA->f1;_TC=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TB;_TD=_TC->f1;if(_TD!=Cyc_Absyn_Unsigned)goto _TL26;_TE=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T15=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE;_TF=_T15->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T16=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TF;_T14=_T16->f2;}}{enum Cyc_Absyn_Size_of sz=_T14;_T10=sz;_T11=(int)_T10;
# 441
if(_T11==3)goto _TL2A;else{goto _TL2B;}_TL2B: _T12=sz;_T13=(int)_T12;if(_T13==2)goto _TL2A;else{goto _TL28;}
_TL2A: return 1;
_TL28: return 0;}_TL26: goto _LL3;_TL24: goto _LL3;_TL22: goto _LL3;_TL20: _LL3:
 return 0;;}}}
# 453
static struct _tuple14 Cyc_Vcgen_deref_lterm(struct Cyc_Vcgen_Env*env,void*lt,struct Cyc_AssnDef_AssnMap am){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;int _T5;struct _tuple14 _T6;struct _tuple14 _T7;struct Cyc_Absyn_Vardecl*_T8;_T0=lt;_T1=(int*)_T0;_T2=*_T1;if(_T2!=11)goto _TL2C;_T3=lt;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T9=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T3;_T8=_T9->f1;}{struct Cyc_Absyn_Vardecl*vd=_T8;_T4=vd;_T5=_T4->escapes;
# 456
if(_T5)goto _TL2E;else{goto _TL30;}
_TL30: _T6=Cyc_AssnDef_lookup_var_map(vd,am);return _T6;_TL2E: goto _LL4;}_TL2C: _LL4:{
# 461
struct _tuple14 _T9=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _TA;void*_TB;_TB=_T9.f0;_TA=_T9.f1;{void*mem=_TB;struct Cyc_AssnDef_AssnMap am1=_TA;{struct _tuple14 _TC;
_TC.f0=Cyc_AssnDef_select(mem,lt,0);_TC.f1=am1;_T7=_TC;}return _T7;}};}
# 469
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_do_assign(struct Cyc_Vcgen_Env*env,struct Cyc_AssnDef_AssnMap am,void*lt,void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;int _T5;struct Cyc_Absyn_Vardecl*_T6;void*_T7;struct Cyc_AssnDef_AssnMap _T8;struct Cyc_AssnDef_AssnMap _T9;struct Cyc_Absyn_Vardecl*_TA;void*_TB;struct Cyc_AssnDef_AssnMap _TC;struct Cyc_AssnDef_AssnMap _TD;
struct _tuple13 _TE=Cyc_AssnDef_split_addr(lt);struct Cyc_List_List*_TF;void*_T10;_T10=_TE.f0;_TF=_TE.f1;{void*root=_T10;struct Cyc_List_List*fields=_TF;struct Cyc_Absyn_Vardecl*_T11;_T0=root;_T1=(int*)_T0;_T2=*_T1;if(_T2!=11)goto _TL31;_T3=root;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T12=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T3;_T11=_T12->f1;}_T4=(struct Cyc_Absyn_Vardecl*)_T11;_T5=_T4->escapes;if(_T5)goto _TL33;else{goto _TL35;}_TL35:{struct Cyc_Absyn_Vardecl*vd=_T11;
# 473
struct _tuple14 _T12=Cyc_AssnDef_lookup_var_map(vd,am);struct Cyc_AssnDef_AssnMap _T13;void*_T14;_T14=_T12.f0;_T13=_T12.f1;{void*oldv=_T14;struct Cyc_AssnDef_AssnMap am1=_T13;_T6=vd;_T7=
Cyc_AssnDef_apply_aggr_update(oldv,fields,t);_T8=am1;_T9=Cyc_AssnDef_update_var_map(_T6,_T7,_T8);return _T9;}}_TL33: goto _LL6;_TL31: _LL6:{
# 476
struct _tuple14 _T12=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _T13;void*_T14;_T14=_T12.f0;_T13=_T12.f1;{void*mem=_T14;struct Cyc_AssnDef_AssnMap am1=_T13;_TA=Cyc_AssnDef_memory;_TB=
Cyc_AssnDef_update(mem,lt,t);_TC=am1;_TD=Cyc_AssnDef_update_var_map(_TA,_TB,_TC);return _TD;}};}}
# 483
static void Cyc_Vcgen_update_try_assnmap(struct Cyc_Vcgen_Env*env,struct Cyc_AssnDef_AssnMap am,void*exn_value){struct Cyc_Vcgen_Env*_T0;struct Cyc_AssnDef_AssnMap*_T1;struct Cyc_Vcgen_Env*_T2;struct Cyc_AssnDef_AssnMap _T3;struct Cyc_Vcgen_Env*_T4;struct Cyc_Absyn_Vardecl*_T5;void*_T6;void*_T7;struct Cyc_Vcgen_Env*_T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_AssnDef_AssnMap*_TA;struct Cyc_AssnDef_AssnMap*_TB;struct Cyc_AssnDef_AssnMap _TC;struct Cyc_AssnDef_AssnMap _TD;struct Cyc_AssnDef_AssnMap _TE;struct Cyc_Vcgen_Env*_TF;struct Cyc_AssnDef_AssnMap*_T10;struct Cyc_AssnDef_AssnMap*_T11;_T0=env;_T1=_T0->try_assnmap;
if(_T1!=0)goto _TL36;return;_TL36: _T2=env;_T3=am;_T4=env;_T5=
Cyc_AssnDef_exception_vardecl();_T6=Cyc_Vcgen_myaddr(_T4,_T5);_T7=exn_value;am=Cyc_Vcgen_do_assign(_T2,_T3,_T6,_T7);_T8=env;_T9=env;_TA=_T9->try_assnmap;_TB=
_check_null(_TA);_TC=*_TB;_TD=am;_TE=Cyc_AssnDef_or_assnmap_assnmap(_TC,_TD);am=Cyc_Vcgen_may_widen(_T8,_TE);_TF=env;_T10=_TF->try_assnmap;_T11=
_check_null(_T10);*_T11=am;}
# 490
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_local_decl(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Decl*d,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Decl*_T0;int*_T1;unsigned _T2;struct Cyc_Vcgen_Env*_T3;struct Cyc_AssnDef_AssnMap _T4;void*_T5;struct Cyc_Absyn_Vardecl*_T6;struct Cyc_Absyn_Vardecl*_T7;void*_T8;void*_T9;struct Cyc_Absyn_Vardecl*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_Absyn_Vardecl*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_AssnDef_AssnMap _TF;struct Cyc_Vcgen_Env*_T10;struct Cyc_AssnDef_AssnMap _T11;void*_T12;void*_T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_AssnMap _T15;void*_T16;struct Cyc_AssnDef_AssnMap _T17;void*_T18;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T19;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T1A;void*_T1B;struct Cyc_Vcgen_Env*_T1C;struct Cyc_AssnDef_AssnMap _T1D;void*_T1E;void*_T1F;struct Cyc_AssnDef_AssnMap _T20;struct Cyc_AssnDef_AssnMap _T21;struct Cyc_Vcgen_Env*_T22;struct Cyc_AssnDef_AssnMap _T23;void*_T24;void*_T25;struct Cyc_AssnDef_AssnMap _T26;struct Cyc_AssnDef_AssnMap _T27;struct Cyc_AssnDef_AssnMap _T28;_T0=d;{
void*_T29=_T0->r;struct Cyc_Absyn_Vardecl*_T2A;struct Cyc_List_List*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Pat*_T2D;void*_T2E;_T1=(int*)_T29;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2F=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T29;_T2E=_T2F->f1;}{struct Cyc_Absyn_Vardecl*vd=_T2E;_T3=env;_T4=amin;_T5=
# 495
Cyc_Vcgen_myaddr(env,vd);_T6=vd;_T7=vd;_T8=_T7->type;_T9=Cyc_AssnDef_fresh_var(_T6,_T8);amin=Cyc_Vcgen_do_assign(_T3,_T4,_T5,_T9);_TA=vd;_TB=_TA->initializer;
if(_TB==0)goto _TL39;_TC=env;_TD=vd;_TE=_TD->initializer;_TF=amin;{
struct _tuple14 _T2F=Cyc_Vcgen_vcgen_rexp(_TC,_TE,_TF);struct Cyc_AssnDef_AssnMap _T30;void*_T31;_T31=_T2F.f0;_T30=_T2F.f1;{void*t=_T31;struct Cyc_AssnDef_AssnMap am=_T30;_T10=env;_T11=am;_T12=
Cyc_Vcgen_myaddr(env,vd);_T13=t;am=Cyc_Vcgen_do_assign(_T10,_T11,_T12,_T13);_T14=am;
return _T14;}}_TL39: _T15=amin;
# 501
return _T15;}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T2F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T29;_T2D=_T2F->f1;_T2C=_T2F->f3;_T16=_T2F->f4;_T2E=(void*)_T16;}{struct Cyc_Absyn_Pat*p=_T2D;struct Cyc_Absyn_Exp*e=_T2C;void*dtree=_T2E;
# 503
struct _tuple14 _T2F=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T30;void*_T31;_T31=_T2F.f0;_T30=_T2F.f1;{void*v=_T31;struct Cyc_AssnDef_AssnMap am=_T30;
struct _tuple18 _T32=Cyc_Vcgen_vcgen_pat(env,p,0,v,am);struct Cyc_AssnDef_AssnMap _T33;struct Cyc_AssnDef_AssnMap _T34;_T34=_T32.f0;_T33=_T32.f1;{struct Cyc_AssnDef_AssnMap amt=_T34;struct Cyc_AssnDef_AssnMap amf=_T33;_T17=amf;_T18=_T17.assn;_T19=& Cyc_AssnDef_false_assn;_T1A=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T19;_T1B=(void*)_T1A;
# 508
if(_T18==_T1B)goto _TL3B;_T1C=env;_T1D=amf;_T1E=
Cyc_Absyn_exn_type();_T1F=Cyc_AssnDef_fresh_var(0,_T1E);Cyc_Vcgen_update_try_assnmap(_T1C,_T1D,_T1F);goto _TL3C;_TL3B: _TL3C: _T20=amt;
return _T20;}}}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T2F=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T29;_T2B=_T2F->f1;}{struct Cyc_List_List*vds=_T2B;_T21=amin;
# 514
return _T21;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T2F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T29;_T2A=_T2F->f2;_T2C=_T2F->f3;}{struct Cyc_Absyn_Vardecl*vd=_T2A;struct Cyc_Absyn_Exp*eopt=_T2C;
# 516
if(eopt==0)goto _TL3D;{
struct _tuple14 _T2F=Cyc_Vcgen_vcgen_rexp(env,eopt,amin);struct Cyc_AssnDef_AssnMap _T30;void*_T31;_T31=_T2F.f0;_T30=_T2F.f1;{void*t=_T31;struct Cyc_AssnDef_AssnMap am=_T30;_T22=env;_T23=am;_T24=
Cyc_Vcgen_myaddr(env,vd);_T25=t;am=Cyc_Vcgen_do_assign(_T22,_T23,_T24,_T25);_T26=am;
return _T26;}}_TL3D: _T27=amin;
# 521
return _T27;}default: _T28=amin;
return _T28;};}}struct _tuple19{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_AssnDef_ExistAssnFn*f2;struct Cyc_AssnDef_ExistAssnFn*f3;struct Cyc_AssnDef_ExistAssnFn*f4;struct Cyc_AssnDef_ExistAssnFn*f5;struct Cyc_Absyn_Vardecl*f6;};
# 528
static struct _tuple19 Cyc_Vcgen_get_requires_and_ensures_info(struct Cyc_Absyn_FnInfo*fi){struct _tuple19 _T0;struct Cyc_Absyn_FnInfo*_T1;struct Cyc_Absyn_FnInfo*_T2;struct Cyc_Absyn_FnInfo*_T3;struct Cyc_Absyn_FnInfo*_T4;struct Cyc_Absyn_FnInfo*_T5;struct Cyc_Absyn_FnInfo*_T6;struct Cyc_Absyn_FnInfo*_T7;{struct _tuple19 _T8;_T1=fi;
_T8.f0=_T1->args;_T2=fi;_T8.f1=_T2->arg_vardecls;_T3=fi;_T8.f2=_T3->checks_assn;_T4=fi;_T8.f3=_T4->requires_assn;_T5=fi;
_T8.f4=_T5->ensures_assn;_T6=fi;_T8.f5=_T6->throws_assn;_T7=fi;_T8.f6=_T7->return_value;_T0=_T8;}
# 529
return _T0;}
# 533
static struct _tuple19 Cyc_Vcgen_get_requires_and_ensures(void*topt){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T5;struct Cyc_Absyn_FnInfo*_T6;struct _tuple19 _T7;struct _tuple19 _T8;
if(topt==0)goto _TL3F;{
void*t=Cyc_Tcutil_pointer_elt_type(topt);void*_T9;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=6)goto _TL41;_T3=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_TA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3;_T4=t;_T5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T4;_T6=& _T5->f1;_T9=(struct Cyc_Absyn_FnInfo*)_T6;}{struct Cyc_Absyn_FnInfo*fi=(struct Cyc_Absyn_FnInfo*)_T9;_T7=
# 537
Cyc_Vcgen_get_requires_and_ensures_info(fi);return _T7;}_TL41: goto _LL0;_LL0:;}goto _TL40;_TL3F: _TL40:{struct _tuple19 _T9;
# 541
_T9.f0=0;_T9.f1=0;_T9.f2=0;_T9.f3=0;_T9.f4=0;_T9.f5=0;_T9.f6=0;_T8=_T9;}return _T8;}
# 544
static unsigned Cyc_Vcgen_datatype_constructor_index(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){struct Cyc_Absyn_Datatypedecl*_T0;struct Cyc_Core_Opt*_T1;struct Cyc_Core_Opt*_T2;void*_T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc_Absyn_Datatypefield*_T6;struct Cyc_Absyn_Datatypefield*_T7;unsigned _T8;struct Cyc_Absyn_Datatypefield*_T9;struct _tuple2*_TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_Absyn_Datatypefield*_TD;struct _tuple2*_TE;int _TF;unsigned _T10;struct Cyc_List_List*_T11;int(*_T12)(struct _fat_ptr,struct _fat_ptr);void*(*_T13)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T14;struct _fat_ptr _T15;
# 546
unsigned i=0U;_T0=dd;_T1=_T0->fields;_T2=
_check_null(_T1);_T3=_T2->v;{struct Cyc_List_List*fs=(struct Cyc_List_List*)_T3;
_TL46: if(fs!=0)goto _TL44;else{goto _TL45;}
_TL44: _T4=fs;_T5=_T4->hd;_T6=(struct Cyc_Absyn_Datatypefield*)_T5;_T7=df;if(_T6!=_T7)goto _TL47;_T8=i;return _T8;_TL47: _T9=df;_TA=_T9->name;_TB=fs;_TC=_TB->hd;_TD=(struct Cyc_Absyn_Datatypefield*)_TC;_TE=_TD->name;_TF=
Cyc_Absyn_qvar_cmp(_TA,_TE);if(_TF!=0)goto _TL49;_T10=i;return _T10;_TL49: _T11=fs;
# 548
fs=_T11->tl;i=i + 1;goto _TL46;_TL45: _T13=Cyc_Warn_impos;{
# 552
int(*_T16)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T13;_T12=_T16;}_T14=_tag_fat("vcgen: datatype_constructor_index",sizeof(char),34U);_T15=_tag_fat(0U,sizeof(void*),0);_T12(_T14,_T15);}}struct _tuple20{int f0;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};
# 555
static struct _tuple20 Cyc_Vcgen_get_aggr_info(void*t){int*_T0;unsigned _T1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2;void*_T3;int*_T4;int _T5;void*_T6;struct _tuple20 _T7;struct Cyc_Absyn_Aggrdecl*_T8;struct Cyc_Absyn_AggrdeclImpl*_T9;struct Cyc_Absyn_AggrdeclImpl*_TA;struct Cyc_Absyn_Aggrdecl*_TB;struct Cyc_Absyn_Aggrdecl*_TC;struct Cyc_Absyn_AggrdeclImpl*_TD;struct _tuple20 _TE;int(*_TF)(struct _fat_ptr,struct _fat_ptr);void*(*_T10)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T11;struct _fat_ptr _T12;
# 557
void*_T13=Cyc_Absyn_compress(t);struct Cyc_List_List*_T14;int _T15;enum Cyc_Absyn_AggrKind _T16;union Cyc_Absyn_AggrInfo _T17;_T0=(int*)_T13;_T1=*_T0;switch(_T1){case 0: _T2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13;_T3=_T2->f1;_T4=(int*)_T3;_T5=*_T4;if(_T5!=24)goto _TL4C;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T13;_T6=_T18->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T19=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6;_T17=_T19->f1;}}{union Cyc_Absyn_AggrInfo info=_T17;
# 559
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);{struct _tuple20 _T18;_T8=ad;_T9=_T8->impl;_TA=
_check_null(_T9);_T18.f0=_TA->tagged;_TB=ad;_T18.f1=_TB->kind;_TC=ad;_TD=_TC->impl;_T18.f2=_TD->fields;_T7=_T18;}return _T7;}_TL4C: goto _LL5;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T18=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T13;_T16=_T18->f1;_T15=_T18->f2;_T14=_T18->f3;}{enum Cyc_Absyn_AggrKind ak=_T16;int is_tuple=_T15;struct Cyc_List_List*fs=_T14;{struct _tuple20 _T18;
# 562
_T18.f0=0;_T18.f1=ak;_T18.f2=fs;_TE=_T18;}return _TE;}default: _LL5: _T10=Cyc_Warn_impos;{
int(*_T18)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T10;_TF=_T18;}_T11=_tag_fat("vcgen: bad type in get_aggr_info",sizeof(char),33U);_T12=_tag_fat(0U,sizeof(void*),0);_TF(_T11,_T12);};}struct _tuple21{int f0;void*f1;};
# 567
static int Cyc_Vcgen_cmp_index(struct _tuple21*x,struct _tuple21*y){struct _tuple21*_T0;int _T1;struct _tuple21*_T2;int _T3;int _T4;_T0=x;_T1=_T0->f0;_T2=y;_T3=_T2->f0;_T4=_T1 - _T3;
return _T4;}
# 571
struct Cyc_Vcgen_ExpChecks*Cyc_Vcgen_exp2ctxt_checks(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e){struct Cyc_Vcgen_ExpChecks**(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*);void**(*_T1)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table**_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Vcgen_ExpChecks*_T5;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T6;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T7;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T8;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T9;struct Cyc_Vcgen_ExpChecks*_TA;struct Cyc_Vcgen_ExpChecks**_TB;struct Cyc_Vcgen_ExpChecks*_TC;_T1=Cyc_Hashtable_lookup_opt;{
struct Cyc_Vcgen_ExpChecks**(*_TD)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*)=(struct Cyc_Vcgen_ExpChecks**(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*))_T1;_T0=_TD;}_T2=_check_null(assn_info);_T3=*_T2;_T4=e;{struct Cyc_Vcgen_ExpChecks**result=_T0(_T3,_T4);
if(result!=0)goto _TL4E;{
static struct Cyc_Vcgen_ExpChecks*b=0;
if(b!=0)goto _TL50;{struct Cyc_Vcgen_ExpChecks*_TD=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));
_TD->ctxt=Cyc_AssnDef_true_assnmap();_T6=& Cyc_AssnDef_false_assn;_T7=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T6;
_TD->bounds_check=(void*)_T7;_T8=& Cyc_AssnDef_false_assn;_T9=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T8;
_TD->null_check=(void*)_T9;_T5=(struct Cyc_Vcgen_ExpChecks*)_TD;}
# 576
b=_T5;goto _TL51;_TL50: _TL51: _TA=b;
# 582
return _TA;}
# 584
_TL4E: _TB=result;_TC=*_TB;return _TC;}}
# 588
static void Cyc_Vcgen_insert_exp_checks(struct Cyc_Hashtable_Table**assn_info,struct Cyc_Absyn_Exp*e,struct Cyc_Vcgen_ExpChecks*exp_check){void(*_T0)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*);void(*_T1)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table**_T2;struct Cyc_Hashtable_Table*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Vcgen_ExpChecks*_T5;_T1=Cyc_Hashtable_insert;{
void(*_T6)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*)=(void(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Exp*,struct Cyc_Vcgen_ExpChecks*))_T1;_T0=_T6;}_T2=_check_null(assn_info);_T3=*_T2;_T4=e;_T5=exp_check;_T0(_T3,_T4,_T5);
return;}
# 593
static struct Cyc_Absyn_Exp*Cyc_Vcgen_zero_exp (void){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp*_T1;
struct Cyc_Absyn_Exp*ans=Cyc_Absyn_uint_exp(0U,0U);_T0=ans;
_T0->topt=Cyc_Absyn_uint_type;_T1=ans;
return _T1;}
# 599
static struct _tuple0 Cyc_Vcgen_okderef(void*root,void*i){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;struct _tuple0 _T5;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T6;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T7;void*_T8;void*_T9;void*_TA;void*_TB;void*_TC;struct _tuple0 _TD;void*_TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;struct Cyc_Absyn_Vardecl*_T17;void*_T18;int*_T19;int _T1A;struct Cyc_Absyn_ArrayInfo _T1B;struct Cyc_Absyn_Exp*_T1C;struct Cyc_Vcgen_Env*_T1D;struct Cyc_Absyn_ArrayInfo _T1E;struct Cyc_Absyn_Exp*_T1F;struct Cyc_AssnDef_AssnMap _T20;struct _tuple0 _T21;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T22;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T23;struct _tuple0 _T24;void*_T25;void*_T26;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T27;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T28;int*_T29;int _T2A;void*_T2B;struct Cyc_Absyn_PtrInfo _T2C;struct Cyc_Absyn_PtrAtts _T2D;void*_T2E;void*_T2F;int _T30;void*_T31;void*_T32;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T33;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T34;struct _tuple0 _T35;void*_T36;void*_T37;void*_T38;void*_T39;void*_T3A;void*_T3B;struct _tuple0 _T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;{struct Cyc_Absyn_Vardecl*_T41;void*_T42;void*_T43;_T0=root;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 12: _T3=root;{struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_T44=(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_T3;_T4=_T44->f3;_T43=(void*)_T4;}{void*size=_T43;{struct _tuple0 _T44;
# 601
_T44.f0=Cyc_AssnDef_ult(i,size);_T6=& Cyc_AssnDef_true_assn;_T7=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T6;_T44.f1=(void*)_T7;_T5=_T44;}return _T5;}case 14: _T8=root;{struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_T44=(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_T8;_T9=_T44->f1;_T43=(void*)_T9;_TA=_T44->f2;_T42=(void*)_TA;}{void*r=_T43;void*i0=_T42;_TB=r;_TC=
# 603
Cyc_AssnDef_plus(i,i0,0);{struct _tuple0 _T44=Cyc_Vcgen_okderef(_TB,_TC);void*_T45;void*_T46;_T46=_T44.f0;_T45=_T44.f1;{void*bndck=_T46;void*nullck=_T45;{struct _tuple0 _T47;_TE=i;_TF=
Cyc_AssnDef_numelts_term(root);_T10=Cyc_AssnDef_ult(_TE,_TF);_T11=bndck;_T47.f0=Cyc_AssnDef_or(_T10,_T11);_T12=root;_T13=
# 606
Cyc_AssnDef_zero();_T14=Cyc_AssnDef_neq(_T12,_T13);_T15=nullck;_T47.f1=Cyc_AssnDef_or(_T14,_T15);_TD=_T47;}
# 604
return _TD;}}}case 11: _T16=root;{struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_T44=(struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_T16;_T41=_T44->f1;}{struct Cyc_Absyn_Vardecl*vd=_T41;_T17=vd;_T18=_T17->type;{
# 608
void*_T44=Cyc_Absyn_compress(_T18);struct Cyc_Absyn_ArrayInfo _T45;_T19=(int*)_T44;_T1A=*_T19;if(_T1A!=5)goto _TL53;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T46=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T44;_T45=_T46->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T45;_T1B=ai;_T1C=_T1B.num_elts;
# 610
if(_T1C==0)goto _TL55;_T1D=
Cyc_Vcgen_bogus_env();_T1E=ai;_T1F=_T1E.num_elts;_T20=Cyc_AssnDef_true_assnmap();{struct _tuple14 _T46=Cyc_Vcgen_vcgen_rexp(_T1D,_T1F,_T20);void*_T47;_T47=_T46.f0;{void*size=_T47;{struct _tuple0 _T48;
_T48.f0=Cyc_AssnDef_ult(i,size);_T22=& Cyc_AssnDef_true_assn;_T23=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T22;_T48.f1=(void*)_T23;_T21=_T48;}return _T21;}}_TL55: goto _LLC;}_TL53: goto _LLC;_LLC:;}{struct _tuple0 _T44;_T25=i;_T26=
# 618
Cyc_AssnDef_numelts_term(root);_T44.f0=Cyc_AssnDef_ult(_T25,_T26);_T27=& Cyc_AssnDef_true_assn;_T28=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T27;_T44.f1=(void*)_T28;_T24=_T44;}return _T24;}default:  {
# 620
void*tp=Cyc_AssnDef_get_term_type(root);
if(tp==0)goto _TL57;{
void*_T44=Cyc_Absyn_compress(tp);struct Cyc_Absyn_PtrInfo _T45;_T29=(int*)_T44;_T2A=*_T29;if(_T2A!=4)goto _TL59;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T46=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T44;_T45=_T46->f1;}{struct Cyc_Absyn_PtrInfo p1=_T45;_T2B=
# 624
Cyc_Absyn_bounds_one();_T2C=p1;_T2D=_T2C.ptr_atts;_T2E=_T2D.bounds;{struct Cyc_Absyn_Exp*b1=Cyc_Tcutil_get_bounds_exp_constrain(_T2B,_T2E,1);
int b2=Cyc_Tcutil_is_nullable_pointer_type(tp,0);_T30=b2;
if(!_T30)goto _TL5B;_T31=root;_T32=Cyc_AssnDef_zero();_T2F=Cyc_AssnDef_neq(_T31,_T32);goto _TL5C;_TL5B: _T33=& Cyc_AssnDef_true_assn;_T34=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T33;_T2F=(void*)_T34;_TL5C: {void*nullck=_T2F;
if(b1==0)goto _TL5D;{struct _tuple0 _T46;_T36=i;_T37=
# 634
Cyc_AssnDef_cnst(b1);_T38=Cyc_AssnDef_ult(_T36,_T37);_T39=i;_T3A=
Cyc_AssnDef_numelts_term(root);_T3B=Cyc_AssnDef_ult(_T39,_T3A);
# 634
_T46.f0=Cyc_AssnDef_or(_T38,_T3B);
_T46.f1=nullck;_T35=_T46;}
# 634
return _T35;_TL5D: goto _LL14;}}}_TL59: goto _LL14;_LL14:;}goto _TL58;_TL57: _TL58: goto _LL0;}}_LL0:;}{struct _tuple0 _T41;_T3D=i;_T3E=
# 643
Cyc_AssnDef_numelts_term(root);_T41.f0=Cyc_AssnDef_ult(_T3D,_T3E);_T3F=root;_T40=Cyc_AssnDef_zero();_T41.f1=Cyc_AssnDef_neq(_T3F,_T40);_T3C=_T41;}return _T3C;}struct _tuple22{void*f0;void*f1;struct Cyc_AssnDef_AssnMap f2;};
# 647
static struct _tuple22 Cyc_Vcgen_vcgen_deref(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*root,struct Cyc_Absyn_Exp*index,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Vcgen_Env*_T0;int _T1;struct Cyc_Absyn_Exp*_T2;void*_T3;void*_T4;struct Cyc_Vcgen_Env*_T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_AssnDef_AssnMap _T7;struct _tuple14 _T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_AssnDef_AssnMap _TA;void*_TB;void*_TC;void*_TD;struct Cyc_AssnDef_AssnMap _TE;void*_TF;void*_T10;struct Cyc_Vcgen_ExpChecks*_T11;struct Cyc_Vcgen_ExpChecks*_T12;struct Cyc_Vcgen_ExpChecks*_T13;struct Cyc_Vcgen_Env*_T14;struct Cyc_Hashtable_Table**_T15;struct Cyc_Absyn_Exp*_T16;struct Cyc_Vcgen_ExpChecks*_T17;struct Cyc_AssnDef_AssnMap _T18;void*_T19;struct _tuple22 _T1A;
# 649
struct _tuple14 _T1B=Cyc_Vcgen_vcgen_rexp(env,root,amin);struct Cyc_AssnDef_AssnMap _T1C;void*_T1D;_T1D=_T1B.f0;_T1C=_T1B.f1;{void*t1=_T1D;struct Cyc_AssnDef_AssnMap am1=_T1C;
struct _tuple14 _T1E=Cyc_Vcgen_vcgen_rexp(env,index,am1);struct Cyc_AssnDef_AssnMap _T1F;void*_T20;_T20=_T1E.f0;_T1F=_T1E.f1;{void*t2=_T20;struct Cyc_AssnDef_AssnMap am2=_T1F;_T0=env;_T1=_T0->pure_exp;
if(_T1)goto _TL5F;else{goto _TL61;}
# 654
_TL61:{struct _tuple0 _T21=Cyc_Vcgen_okderef(t1,t2);void*_T22;void*_T23;_T23=_T21.f0;_T22=_T21.f1;{void*bnds_check=_T23;void*null_check=_T22;_T2=root;_T3=_T2->topt;_T4=
# 660
_check_null(_T3);{struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(_T4);
void*term_numelts;
void*bnds_check_sufficient;
if(eopt!=0)goto _TL62;
# 665
term_numelts=Cyc_AssnDef_numelts_term(t1);
bnds_check_sufficient=Cyc_AssnDef_ult(t2,term_numelts);goto _TL63;
# 668
_TL62: _T5=env;_T6=eopt;_T7=Cyc_AssnDef_true_assnmap();_T8=Cyc_Vcgen_vcgen_rexp(_T5,_T6,_T7);term_numelts=_T8.f0;
bnds_check_sufficient=Cyc_AssnDef_ult(t2,term_numelts);_TL63: _T9=env;_TA=am2;_TB=
# 671
Cyc_AssnDef_not(bnds_check_sufficient);_TC=Cyc_AssnDef_not(null_check);_TD=Cyc_AssnDef_or(_TB,_TC);_TE=Cyc_AssnDef_and_assnmap_assn(_TA,_TD);_TF=Cyc_Absyn_exn_type();_T10=Cyc_AssnDef_fresh_var(0,_TF);Cyc_Vcgen_update_try_assnmap(_T9,_TE,_T10);{
# 673
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));_T11=exp_checks;_T11->ctxt=am2;_T12=exp_checks;_T12->bounds_check=bnds_check;_T13=exp_checks;
_T13->null_check=null_check;_T14=env;_T15=_T14->assn_info;_T16=root;_T17=exp_checks;
Cyc_Vcgen_insert_exp_checks(_T15,_T16,_T17);_T18=am2;_T19=
# 677
Cyc_AssnDef_and(bnds_check,null_check);am2=Cyc_AssnDef_and_assnmap_assn(_T18,_T19);}}}}goto _TL60;_TL5F: _TL60:{struct _tuple22 _T21;
# 679
_T21.f0=t1;_T21.f1=t2;_T21.f2=am2;_T1A=_T21;}return _T1A;}}}
# 682
static struct _tuple14 Cyc_Vcgen_vcgen_comprehension(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,void*res_type,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Vcgen_Env*_T0;struct Cyc_AssnDef_AssnMap _T1;void*_T2;void*_T3;struct Cyc_AssnDef_AssnMap _T4;void*_T5;struct Cyc_AssnDef_AssnMap _T6;void*_T7;struct Cyc_Vcgen_Env*_T8;struct Cyc_AssnDef_AssnMap _T9;void*_TA;void*_TB;void*_TC;void*_TD;void*_TE;int _TF;struct Cyc_AssnDef_AssnMap _T10;struct Cyc_AssnDef_AssnMap _T11;void*_T12;void*_T13;void*_T14;struct _tuple14 _T15;
# 686
struct _tuple14 _T16=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T17;void*_T18;_T18=_T16.f0;_T17=_T16.f1;{void*t1=_T18;struct Cyc_AssnDef_AssnMap amin=_T17;
# 688
void*v=Cyc_AssnDef_zero();_T0=env;_T1=amin;_T2=
Cyc_Vcgen_myaddr(env,vd);_T3=v;amin=Cyc_Vcgen_do_assign(_T0,_T1,_T2,_T3);
_TL64: if(1)goto _TL65;else{goto _TL66;}
_TL65: amin=Cyc_AssnDef_canonical_assnmap(amin);{
# 693
struct _tuple14 _T19=Cyc_AssnDef_lookup_var_map(vd,amin);struct Cyc_AssnDef_AssnMap _T1A;void*_T1B;_T1B=_T19.f0;_T1A=_T19.f1;{void*v2=_T1B;struct Cyc_AssnDef_AssnMap am2=_T1A;
v=v2;amin=am2;_T4=amin;_T5=
Cyc_AssnDef_ult(v,t1);{struct Cyc_AssnDef_AssnMap amt=Cyc_AssnDef_and_assnmap_assn(_T4,_T5);_T6=amin;_T7=
Cyc_AssnDef_ulte(t1,v);{struct Cyc_AssnDef_AssnMap amf=Cyc_AssnDef_and_assnmap_assn(_T6,_T7);
struct _tuple14 _T1C=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _T1D;_T1D=_T1C.f1;{struct Cyc_AssnDef_AssnMap ambody=_T1D;
# 699
struct _tuple14 _T1E=Cyc_AssnDef_lookup_var_map(vd,ambody);struct Cyc_AssnDef_AssnMap _T1F;void*_T20;_T20=_T1E.f0;_T1F=_T1E.f1;{void*v2=_T20;struct Cyc_AssnDef_AssnMap am2=_T1F;_T8=env;_T9=am2;_TA=
Cyc_Vcgen_myaddr(env,vd);_TB=v2;_TC=Cyc_AssnDef_one();_TD=Cyc_Absyn_uint_type;_TE=Cyc_AssnDef_plus(_TB,_TC,_TD);ambody=Cyc_Vcgen_do_assign(_T8,_T9,_TA,_TE);
ambody=Cyc_AssnDef_canonical_assnmap(ambody);{
# 703
struct _tuple15 _T21=Cyc_AssnDef_unify_var_maps_subst(amin,ambody);struct Cyc_Dict_Dict _T22;void*_T23;void*_T24;_T24=_T21.f0;_T23=_T21.f1;_T22=_T21.f2;{void*a1=_T24;void*a2=_T23;struct Cyc_Dict_Dict map=_T22;_TF=
# 706
Cyc_Vcgen_implies(a2,a1);if(!_TF)goto _TL67;
# 708
amin=amf;goto _TL66;_TL67:{struct Cyc_AssnDef_AssnMap _T25;
# 711
_T25.assn=Cyc_AssnDef_or(a1,a2);_T25.map=map;_T10=_T25;}amin=Cyc_AssnDef_widen_assnmap(_T10);}}}}}}}}goto _TL64;_TL66: {
# 714
void*res=Cyc_AssnDef_fresh_var(0,res_type);_T11=amin;_T12=
# 716
Cyc_AssnDef_numelts_term(res);_T13=t1;_T14=Cyc_AssnDef_eq(_T12,_T13);amin=Cyc_AssnDef_and_assnmap_assn(_T11,_T14);{struct _tuple14 _T19;
_T19.f0=res;_T19.f1=amin;_T15=_T19;}return _T15;}}}
# 720
static struct _tuple14 Cyc_Vcgen_vcgen_binop(struct Cyc_Vcgen_Env*env,void*tp_opt,enum Cyc_Absyn_Primop p,void*t1,void*t2,struct Cyc_AssnDef_AssnMap am){int _T0;enum Cyc_Absyn_Primop _T1;int _T2;struct Cyc_Vcgen_Env*_T3;int _T4;int _T5;int _T6;struct Cyc_Vcgen_Env*_T7;struct Cyc_AssnDef_AssnMap _T8;void*_T9;void*_TA;struct _tuple14 _TB;struct _tuple14 _TC;void*_TD;void*_TE;void*_TF;int(*_T10)(struct _fat_ptr,struct _fat_ptr);void*(*_T11)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_Vcgen_Env*_T14;int _T15;enum Cyc_Absyn_Primop _T16;int _T17;int _T18;struct Cyc_AssnDef_AssnMap _T19;void*_T1A;struct _tuple14 _T1B;
# 723
void*tp=_check_null(tp_opt);_T0=
Cyc_Tcutil_is_pointer_type(tp);if(!_T0)goto _TL69;_T1=p;_T2=(int)_T1;switch(_T2){case Cyc_Absyn_Plus: _T3=env;_T4=_T3->pure_exp;
# 729
if(_T4)goto _TL6C;else{goto _TL6E;}_TL6E: _T5=Cyc_Tcutil_is_zeroterm_pointer_type(tp);if(!_T5)goto _TL6C;_T6=
Cyc_Tcutil_is_fat_pointer_type(tp);
# 729
if(_T6)goto _TL6C;else{goto _TL6F;}
# 731
_TL6F: _T7=env;_T8=am;_T9=Cyc_Absyn_exn_type();_TA=Cyc_AssnDef_fresh_var(0,_T9);Cyc_Vcgen_update_try_assnmap(_T7,_T8,_TA);goto _TL6D;_TL6C: _TL6D:{struct _tuple14 _T1C;
_T1C.f0=Cyc_AssnDef_offseti(t1,t2,tp);_T1C.f1=am;_TB=_T1C;}return _TB;case Cyc_Absyn_Minus:{struct _tuple14 _T1C;_TD=t1;_TE=
# 734
Cyc_AssnDef_unop(2U,t2,Cyc_Absyn_sint_type);_TF=tp;_T1C.f0=Cyc_AssnDef_offseti(_TD,_TE,_TF);_T1C.f1=am;_TC=_T1C;}return _TC;default: _T11=Cyc_Warn_impos;{
# 736
int(*_T1C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T11;_T10=_T1C;}_T12=_tag_fat("vcgen: result has pointer type but uses bad primop",sizeof(char),51U);_T13=_tag_fat(0U,sizeof(void*),0);_T10(_T12,_T13);}goto _TL6A;_TL69: _TL6A: {
# 740
void*t=Cyc_AssnDef_binop(p,t1,t2,tp);_T14=env;_T15=_T14->pure_exp;
# 743
if(_T15)goto _TL70;else{goto _TL72;}_TL72: _T16=p;_T17=(int)_T16;if(_T17!=21)goto _TL70;_T18=Cyc_Unify_unify(Cyc_Absyn_uint_type,tp);if(!_T18)goto _TL70;_T19=am;_T1A=
Cyc_AssnDef_ult(t,t2);am=Cyc_AssnDef_and_assnmap_assn(_T19,_T1A);goto _TL71;_TL70: _TL71:{struct _tuple14 _T1C;
_T1C.f0=t;_T1C.f1=am;_T1B=_T1C;}return _T1B;}}
# 748
static int Cyc_Vcgen_visit_pure_stmt(int*env,struct Cyc_Absyn_Stmt*s){int*_T0;unsigned _T1;struct Cyc_Absyn_Stmt*_T2;int*_T3;unsigned _T4;int*_T5;_T0=env;_T1=(unsigned)_T0;
if(_T1)goto _TL73;else{goto _TL75;}_TL75: return 0;_TL73: _T2=s;{
void*_T6=_T2->r;_T3=(int*)_T6;_T4=*_T3;switch(_T4){case 3: goto _LL4;case 5: _LL4: goto _LL6;case 6: _LL6: goto _LL8;case 7: _LL8: goto _LLA;case 8: _LLA: goto _LLC;case 9: _LLC: goto _LLE;case 14: _LLE: goto _LL10;case 15: _LL10: _T5=env;
# 758
*_T5=0;return 0;default:
 return 1;};}}
# 763
static int Cyc_Vcgen_visit_pure_exp(int*env,struct Cyc_Absyn_Exp*e){int*_T0;unsigned _T1;struct Cyc_Absyn_Exp*_T2;int*_T3;unsigned _T4;enum Cyc_Absyn_Primop _T5;int _T6;enum Cyc_Absyn_Primop _T7;int _T8;enum Cyc_Absyn_Coercion _T9;int _TA;int _TB;int*_TC;_T0=env;_T1=(unsigned)_T0;
if(_T1)goto _TL77;else{goto _TL79;}_TL79: return 0;_TL77: _T2=e;{
void*_TD=_T2->r;int _TE;enum Cyc_Absyn_Coercion _TF;enum Cyc_Absyn_Primop _T10;_T3=(int*)_TD;_T4=*_T3;switch(_T4){case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T11=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TD;_T10=_T11->f1;}{enum Cyc_Absyn_Primop p=_T10;_T5=p;_T6=(int)_T5;
# 767
if(_T6==3)goto _TL7D;else{goto _TL7E;}_TL7E: _T7=p;_T8=(int)_T7;if(_T8==4)goto _TL7D;else{goto _TL7B;}_TL7D: goto _LL0;_TL7B:
 return 1;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T11=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TD;_TF=_T11->f4;}{enum Cyc_Absyn_Coercion c=_TF;_T9=c;if(_T9!=Cyc_Absyn_No_coercion)goto _TL7F;
# 771
return 1;_TL7F: goto _LL1F;_LL1F: goto _LL0;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T11=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_TD;_TE=_T11->f3;}_TA=(int)_TE;if(!_TA)goto _TL81;{int is_tagged=_TE;goto _LL0;}_TL81: goto _LL1D;case 41:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T11=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TD;_TE=_T11->f2;}_TB=(int)_TE;if(_TB)goto _TL83;else{goto _TL85;}_TL85:{int static_only=_TE;goto _LL0;}_TL83: goto _LL1D;case 4: goto _LLC;case 5: _LLC: goto _LLE;case 10: _LLE: goto _LL10;case 11: _LL10: goto _LL12;case 20: _LL12: goto _LL14;case 22: _LL14: goto _LL16;case 23: _LL16: goto _LL18;case 34: _LL18: goto _LL1A;case 39: _LL1A: goto _LL1C;case 40: _LL1C: goto _LL0;default: _LL1D:
# 787
 return 1;}_LL0:;}_TC=env;
# 789
*_TC=0;
return 0;}
# 793
static int Cyc_Vcgen_is_pure_exp(struct Cyc_Absyn_Exp*e){void(*_T0)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Exp*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Exp*);int*_T2;struct Cyc_Absyn_Exp*_T3;int _T4;
int env=1;_T1=Cyc_Absyn_visit_exp;{
void(*_T5)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Exp*)=(void(*)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Exp*))_T1;_T0=_T5;}_T2=& env;_T3=e;_T0(Cyc_Vcgen_visit_pure_exp,Cyc_Vcgen_visit_pure_stmt,_T2,_T3);_T4=env;
return _T4;}
# 808 "vcgen.cyc"
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_loop(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e1infor,struct Cyc_Absyn_Stmt*s1indo,struct Cyc_Absyn_Exp*cond,struct Cyc_Absyn_Stmt*dummy,struct Cyc_Absyn_Stmt*body,struct Cyc_Absyn_Exp*e3infor,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Vcgen_Env*_T0;struct Cyc_Vcgen_Env*_T1;struct Cyc_AssnDef_AssnMap*_T2;struct Cyc_Absyn_Exp*_T3;int*_T4;int _T5;struct Cyc_Absyn_Exp*_T6;int*_T7;int _T8;int _T9;struct Cyc_Vcgen_Env*_TA;struct Cyc_Vcgen_Env*_TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_AssnDef_AssnMap _TE;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_TF;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T10;struct Cyc_AssnDef_AssnMap _T11;struct _tuple18 _T12;struct Cyc_Vcgen_Env*_T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T15;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T16;struct Cyc_AssnDef_AssnMap _T17;struct Cyc_AssnDef_AssnMap _T18;void*_T19;struct Cyc_AssnDef_AssnMap _T1A;void*_T1B;int _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc_AssnDef_AssnMap _T1E;void*_T1F;struct Cyc_String_pa_PrintArg_struct _T20;struct Cyc_Absyn_Exp*_T21;unsigned _T22;struct _fat_ptr _T23;struct _fat_ptr _T24;struct Cyc_Vcgen_Env*_T25;struct Cyc_AssnDef_AssnMap _T26;struct Cyc_Set_Set*_T27;struct Cyc_Set_Set*_T28;struct Cyc_Set_Set*_T29;struct Cyc_Set_Set*_T2A;struct _tuple18 _T2B;struct Cyc_AssnDef_AssnMap _T2C;struct Cyc_Vcgen_Env*_T2D;struct Cyc_AssnDef_AssnMap*_T2E;struct Cyc_AssnDef_AssnMap*_T2F;struct Cyc_AssnDef_AssnMap _T30;struct Cyc_Vcgen_Env*_T31;struct Cyc_Vcgen_Env*_T32;struct Cyc_Vcgen_Env*_T33;struct Cyc_Absyn_Exp*_T34;struct Cyc_AssnDef_AssnMap _T35;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T36;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T37;struct Cyc_AssnDef_AssnMap _T38;struct _tuple18 _T39;struct Cyc_Vcgen_Env*_T3A;struct Cyc_AssnDef_AssnMap _T3B;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T3C;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T3D;struct Cyc_AssnDef_AssnMap _T3E;struct Cyc_AssnDef_AssnMap _T3F;void*_T40;struct Cyc_AssnDef_AssnMap _T41;void*_T42;int _T43;struct Cyc_String_pa_PrintArg_struct _T44;struct Cyc_AssnDef_AssnMap _T45;void*_T46;struct Cyc_String_pa_PrintArg_struct _T47;struct Cyc_Absyn_Exp*_T48;unsigned _T49;struct _fat_ptr _T4A;struct _fat_ptr _T4B;struct Cyc_Vcgen_Env*_T4C;struct Cyc_AssnDef_AssnMap _T4D;struct Cyc_AssnDef_AssnMap _T4E;struct Cyc_AssnDef_AssnMap _T4F;struct Cyc_AssnDef_AssnMap _T50;int _T51;struct Cyc_Vcgen_Env*_T52;struct Cyc_AssnDef_AssnMap _T53;_T0=env;{
# 812
struct Cyc_AssnDef_AssnMap*old_continue_assnmap=_T0->continue_assnmap;_T1=env;{struct Cyc_AssnDef_AssnMap*_T54=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));
*_T54=Cyc_AssnDef_false_assnmap();_T2=(struct Cyc_AssnDef_AssnMap*)_T54;}_T1->continue_assnmap=_T2;
# 815
if(e1infor==0)goto _TL86;{
struct _tuple14 _T54=Cyc_Vcgen_vcgen_rexp(env,e1infor,amin);struct Cyc_AssnDef_AssnMap _T55;_T55=_T54.f1;{struct Cyc_AssnDef_AssnMap am=_T55;
amin=am;}}goto _TL87;_TL86: _TL87:
# 819
 if(s1indo==0)goto _TL88;
amin=Cyc_Vcgen_vcgen_stmt(env,s1indo,amin);goto _TL89;_TL88: _TL89: {
# 826
struct Cyc_Absyn_Exp*loop_inv_exp=0;
struct Cyc_AssnDef_AssnMap loop_inv_assnmap;_T3=cond;{
void*_T54=_T3->r;struct Cyc_Absyn_Exp*_T55;struct Cyc_Absyn_Exp*_T56;_T4=(int*)_T54;_T5=*_T4;if(_T5!=7)goto _TL8A;{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T57=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T54;_T56=_T57->f1;_T55=_T57->f2;}{struct Cyc_Absyn_Exp*eleft=_T56;struct Cyc_Absyn_Exp*eright=_T55;_T6=eleft;{
# 830
void*_T57=_T6->r;int _T58;struct Cyc_Absyn_Exp*_T59;_T7=(int*)_T57;_T8=*_T7;if(_T8!=41)goto _TL8C;{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T5A=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T57;_T59=_T5A->f1;_T58=_T5A->f2;}_T9=(int)_T58;if(!_T9)goto _TL8E;{struct Cyc_Absyn_Exp*eassn=_T59;int static_only=_T58;
# 832
loop_inv_exp=eassn;
cond=eright;goto _LL8;}_TL8E: goto _LLB;_TL8C: _LLB: goto _LL8;_LL8:;}goto _LL3;}_TL8A: goto _LL3;_LL3:;}
# 848
if(loop_inv_exp==0)goto _TL90;_TA=env;{
int old_widen_paths=_TA->widen_paths;_TB=env;
_TB->widen_paths=0;_TC=env;_TD=loop_inv_exp;{struct Cyc_AssnDef_AssnMap _T54;_TF=& Cyc_AssnDef_true_assn;_T10=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_TF;
_T54.assn=(void*)_T10;_T11=amin;_T54.map=_T11.map;_TE=_T54;}_T12=Cyc_Vcgen_vcgen_test(_TC,_TD,_TE);loop_inv_assnmap=_T12.f0;_T13=env;
_T13->widen_paths=old_widen_paths;}goto _TL91;
# 855
_TL90:{struct Cyc_AssnDef_AssnMap _T54;_T15=& Cyc_AssnDef_true_assn;_T16=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T15;_T54.assn=(void*)_T16;_T17=amin;_T54.map=_T17.map;_T14=_T54;}loop_inv_assnmap=_T14;_TL91: _T18=amin;_T19=_T18.assn;_T1A=loop_inv_assnmap;_T1B=_T1A.assn;_T1C=
# 858
Cyc_Vcgen_implies(_T19,_T1B);if(_T1C)goto _TL92;else{goto _TL94;}
_TL94:{struct Cyc_String_pa_PrintArg_struct _T54;_T54.tag=0;_T1E=loop_inv_assnmap;_T1F=_T1E.assn;
_T54.f1=Cyc_AssnDef_assn2string(_T1F);_T1D=_T54;}{struct Cyc_String_pa_PrintArg_struct _T54=_T1D;{struct Cyc_String_pa_PrintArg_struct _T55;_T55.tag=0;_T55.f1=Cyc_AssnDef_assnmap2string(amin);_T20=_T55;}{struct Cyc_String_pa_PrintArg_struct _T55=_T20;void*_T56[2];_T56[0]=& _T54;_T56[1]=& _T55;_T21=cond;_T22=_T21->loc;_T23=
# 859
_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U);_T24=_tag_fat(_T56,sizeof(void*),2);Cyc_Warn_err(_T22,_T23,_T24);}}_T25=env;
# 861
_T25->continue_assnmap=old_continue_assnmap;_T26=
Cyc_AssnDef_true_assnmap();return _T26;_TL92: {
# 869
struct Cyc_Set_Set*changed=Cyc_AssnDef_calc_changed_vars_exp(cond);_T27=changed;_T28=
Cyc_AssnDef_calc_changed_vars_stmt(body);changed=Cyc_Set_union_two(_T27,_T28);
if(e3infor==0)goto _TL95;_T29=changed;_T2A=
Cyc_AssnDef_calc_changed_vars_exp(e3infor);changed=Cyc_Set_union_two(_T29,_T2A);goto _TL96;_TL95: _TL96:
# 876
 amin=Cyc_AssnDef_force_canonical(changed,amin);
# 889 "vcgen.cyc"
_TL97: if(1)goto _TL98;else{goto _TL99;}
# 893
_TL98: amin=Cyc_AssnDef_canonical_assnmap(amin);{
# 898
struct Cyc_AssnDef_AssnMap amold=Cyc_Vcgen_lookup_stmt_assnmap(env,dummy);
# 902
amin=Cyc_AssnDef_or_assnmap_assnmap(amold,amin);
# 906
amin=Cyc_AssnDef_widen_assnmap(amin);
# 910
Cyc_Vcgen_replace_stmt_assnmap(env,dummy,amin);
# 912
if(loop_inv_exp==0)goto _TL9A;_T2B=
Cyc_Vcgen_vcgen_test(env,loop_inv_exp,amin);amin=_T2B.f0;goto _TL9B;_TL9A: _TL9B: {
# 916
struct _tuple18 _T54=Cyc_Vcgen_vcgen_test(env,cond,amin);struct Cyc_AssnDef_AssnMap _T55;struct Cyc_AssnDef_AssnMap _T56;_T56=_T54.f0;_T55=_T54.f1;{struct Cyc_AssnDef_AssnMap amt=_T56;struct Cyc_AssnDef_AssnMap amf=_T55;
amin=Cyc_Vcgen_vcgen_stmt(env,body,amt);_T2C=amin;_T2D=env;_T2E=_T2D->continue_assnmap;_T2F=
# 922
_check_null(_T2E);_T30=*_T2F;amin=Cyc_AssnDef_or_assnmap_assnmap(_T2C,_T30);
# 928
if(e3infor==0)goto _TL9C;{
struct _tuple14 _T57=Cyc_Vcgen_vcgen_rexp(env,e3infor,amin);struct Cyc_AssnDef_AssnMap _T58;_T58=_T57.f1;{struct Cyc_AssnDef_AssnMap am=_T58;
amin=am;}}goto _TL9D;_TL9C: _TL9D:
# 935
 if(loop_inv_exp==0)goto _TL9E;_T31=env;{
int old_widen_paths=_T31->widen_paths;_T32=env;
_T32->widen_paths=0;_T33=env;_T34=loop_inv_exp;{struct Cyc_AssnDef_AssnMap _T57;_T36=& Cyc_AssnDef_true_assn;_T37=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T36;
_T57.assn=(void*)_T37;_T38=amin;_T57.map=_T38.map;_T35=_T57;}_T39=Cyc_Vcgen_vcgen_test(_T33,_T34,_T35);loop_inv_assnmap=_T39.f0;_T3A=env;
_T3A->widen_paths=old_widen_paths;}goto _TL9F;
# 942
_TL9E:{struct Cyc_AssnDef_AssnMap _T57;_T3C=& Cyc_AssnDef_true_assn;_T3D=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T3C;_T57.assn=(void*)_T3D;_T3E=amin;_T57.map=_T3E.map;_T3B=_T57;}loop_inv_assnmap=_T3B;_TL9F: _T3F=amin;_T40=_T3F.assn;_T41=loop_inv_assnmap;_T42=_T41.assn;_T43=
Cyc_Vcgen_implies(_T40,_T42);if(_T43)goto _TLA0;else{goto _TLA2;}
_TLA2:{struct Cyc_String_pa_PrintArg_struct _T57;_T57.tag=0;_T45=loop_inv_assnmap;_T46=_T45.assn;
_T57.f1=Cyc_AssnDef_assn2string(_T46);_T44=_T57;}{struct Cyc_String_pa_PrintArg_struct _T57=_T44;{struct Cyc_String_pa_PrintArg_struct _T58;_T58.tag=0;_T58.f1=Cyc_AssnDef_assnmap2string(amin);_T47=_T58;}{struct Cyc_String_pa_PrintArg_struct _T58=_T47;void*_T59[2];_T59[0]=& _T57;_T59[1]=& _T58;_T48=cond;_T49=_T48->loc;_T4A=
# 944
_tag_fat("cannot prove loop invariant %s from %s\n",sizeof(char),40U);_T4B=_tag_fat(_T59,sizeof(void*),2);Cyc_Warn_err(_T49,_T4A,_T4B);}}_T4C=env;
# 946
_T4C->continue_assnmap=old_continue_assnmap;_T4D=
Cyc_AssnDef_true_assnmap();return _T4D;_TLA0: _T4E=
# 950
Cyc_Vcgen_lookup_stmt_assnmap(env,dummy);_T4F=amin;{struct _tuple15 _T57=Cyc_AssnDef_unify_var_maps_subst(_T4E,_T4F);struct Cyc_Dict_Dict _T58;void*_T59;void*_T5A;_T5A=_T57.f0;_T59=_T57.f1;_T58=_T57.f2;{void*a1=_T5A;void*a2=_T59;struct Cyc_Dict_Dict map=_T58;{struct Cyc_AssnDef_AssnMap _T5B;
_T5B.assn=a2;_T5B.map=map;_T50=_T5B;}amin=_T50;_T51=
# 956
Cyc_Vcgen_implies(a2,a1);if(!_T51)goto _TLA3;_T52=env;
_T52->continue_assnmap=old_continue_assnmap;_T53=amf;
return _T53;_TLA3:;}}}}}goto _TL97;_TL99:;}}}}struct _tuple23{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 963
static struct _tuple14 Cyc_Vcgen_vcgen_rexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap amin){int _T0;struct Cyc_Absyn_Exp*_T1;int*_T2;unsigned _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;int _T6;struct Cyc_Absyn_Exp*_T7;unsigned _T8;struct Cyc_String_pa_PrintArg_struct _T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc___cycFILE*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;int _T10;struct Cyc_Absyn_Exp*_T11;unsigned _T12;struct Cyc_String_pa_PrintArg_struct _T13;struct Cyc_String_pa_PrintArg_struct _T14;struct Cyc___cycFILE*_T15;struct _fat_ptr _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct _fat_ptr _T19;int _T1A;struct Cyc_Vcgen_Env*_T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;int _T1E;struct Cyc_Vcgen_Env*_T1F;struct _tuple14 _T20;int _T21;struct _tuple14 _T22;struct _tuple14 _T23;struct Cyc_Absyn_Exp*_T24;int*_T25;unsigned _T26;int(*_T27)(struct _fat_ptr,struct _fat_ptr);void*(*_T28)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T29;struct _fat_ptr _T2A;void*_T2B;struct _tuple14 _T2C;struct Cyc_Absyn_Vardecl*_T2D;int _T2E;struct _tuple14 _T2F;void*_T30;void*_T31;struct Cyc_Absyn_Vardecl*_T32;void*_T33;struct Cyc_Absyn_Vardecl*_T34;void*_T35;int*_T36;int _T37;struct _tuple14 _T38;struct _tuple14 _T39;struct _tuple14 _T3A;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T3B;struct Cyc_List_List*_T3C;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T3D;struct Cyc_List_List*_T3E;struct Cyc_List_List*_T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*_T41;void*_T42;struct _tuple14 _T43;enum Cyc_Absyn_Primop _T44;void*_T45;struct Cyc_Absyn_Exp*_T46;void*_T47;struct Cyc_List_List*_T48;void*_T49;struct Cyc_List_List*_T4A;void*_T4B;struct Cyc_Vcgen_Env*_T4C;struct Cyc_Absyn_Exp*_T4D;void*_T4E;enum Cyc_Absyn_Primop _T4F;void*_T50;void*_T51;struct Cyc_AssnDef_AssnMap _T52;struct _tuple14 _T53;int(*_T54)(struct _fat_ptr,struct _fat_ptr);void*(*_T55)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T56;struct _fat_ptr _T57;struct Cyc_Vcgen_Env*_T58;struct Cyc_Absyn_Exp*_T59;void*_T5A;struct Cyc_Core_Opt*_T5B;void*_T5C;enum Cyc_Absyn_Primop _T5D;void*_T5E;void*_T5F;struct Cyc_AssnDef_AssnMap _T60;struct _tuple14 _T61;struct _tuple14 _T62;enum Cyc_Absyn_Incrementor _T63;int _T64;struct Cyc_Vcgen_Env*_T65;struct Cyc_Absyn_Exp*_T66;void*_T67;void*_T68;void*_T69;struct Cyc_AssnDef_AssnMap _T6A;struct Cyc_Vcgen_Env*_T6B;struct Cyc_Absyn_Exp*_T6C;void*_T6D;void*_T6E;void*_T6F;struct Cyc_AssnDef_AssnMap _T70;struct Cyc_Vcgen_Env*_T71;struct Cyc_Absyn_Exp*_T72;void*_T73;void*_T74;void*_T75;struct Cyc_AssnDef_AssnMap _T76;struct Cyc_Vcgen_Env*_T77;struct Cyc_Absyn_Exp*_T78;void*_T79;void*_T7A;void*_T7B;struct Cyc_AssnDef_AssnMap _T7C;int(*_T7D)(struct _fat_ptr,struct _fat_ptr);void*(*_T7E)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7F;struct _fat_ptr _T80;struct _tuple14 _T81;struct Cyc_Absyn_Exp*_T82;void*_T83;struct Cyc_AssnDef_AssnMap _T84;void*_T85;struct Cyc_AssnDef_AssnMap _T86;void*_T87;struct _tuple14 _T88;struct Cyc_Vcgen_Env*_T89;struct Cyc_AssnDef_AssnMap _T8A;struct Cyc_Absyn_Exp*_T8B;void*_T8C;struct Cyc_AssnDef_AssnMap _T8D;void*_T8E;struct Cyc_AssnDef_AssnMap _T8F;void*_T90;void*_T91;void*_T92;struct _tuple14 _T93;struct Cyc_Vcgen_Env*_T94;struct Cyc_AssnDef_AssnMap _T95;struct Cyc_Absyn_Exp*_T96;void*_T97;struct Cyc_AssnDef_AssnMap _T98;void*_T99;void*_T9A;void*_T9B;struct Cyc_AssnDef_AssnMap _T9C;void*_T9D;struct _tuple14 _T9E;struct Cyc_Vcgen_Env*_T9F;struct Cyc_AssnDef_AssnMap _TA0;struct _tuple14 _TA1;struct Cyc_Absyn_Exp*_TA2;void*_TA3;struct Cyc_Vcgen_Env*_TA4;struct Cyc_Hashtable_Table*_TA5;struct Cyc_Absyn_Exp*_TA6;struct Cyc_AssnDef_AssnMap _TA7;void*_TA8;void*_TA9;struct Cyc_List_List*_TAA;struct Cyc_Absyn_Exp*_TAB;void*_TAC;struct Cyc_List_List*_TAD;struct Cyc_List_List*_TAE;struct Cyc_AssnDef_AssnMap _TAF;void*_TB0;void*_TB1;int _TB2;struct Cyc_String_pa_PrintArg_struct _TB3;struct Cyc_String_pa_PrintArg_struct _TB4;struct Cyc_Absyn_Exp*_TB5;unsigned _TB6;struct _fat_ptr _TB7;struct _fat_ptr _TB8;void*_TB9;struct Cyc_AssnDef_ExistAssnFn*_TBA;struct Cyc_List_List*_TBB;struct Cyc_Vcgen_Env*_TBC;struct Cyc_AssnDef_AssnMap _TBD;void*_TBE;struct Cyc_AssnDef_ExistAssnFn*_TBF;struct Cyc_List_List*_TC0;struct Cyc_List_List*_TC1;struct Cyc_AssnDef_ExistAssnFn*_TC2;struct Cyc_AssnDef_AssnFn*_TC3;struct Cyc_List_List*_TC4;int _TC5;int _TC6;int _TC7;struct Cyc_AssnDef_ExistAssnFn*_TC8;struct Cyc_List_List*_TC9;struct Cyc_List_List*_TCA;struct Cyc_Absyn_Exp*_TCB;void*_TCC;void*_TCD;int _TCE;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TCF;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_TD0;struct _tuple14 _TD1;struct _tuple14 _TD2;struct _tuple14 _TD3;struct _tuple14 _TD4;void*_TD5;int _TD6;int _TD7;struct _tuple14 _TD8;struct _tuple0 _TD9;struct Cyc_Absyn_Exp*_TDA;void*_TDB;void*_TDC;void*_TDD;int*_TDE;unsigned _TDF;void*_TE0;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE1;void*_TE2;int*_TE3;int _TE4;void*_TE5;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE6;void*_TE7;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TE8;enum Cyc_Absyn_Size_of _TE9;int _TEA;void*_TEB;int*_TEC;unsigned _TED;void*_TEE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TEF;void*_TF0;int*_TF1;unsigned _TF2;void*_TF3;struct _tuple14 _TF4;struct Cyc_List_List*_TF5;void*_TF6;struct Cyc_Absyn_Exp*_TF7;unsigned _TF8;struct Cyc_Absyn_Exp*_TF9;void*_TFA;void*_TFB;int*_TFC;unsigned _TFD;void*_TFE;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TFF;void*_T100;int*_T101;unsigned _T102;void*_T103;struct _tuple14 _T104;struct Cyc_List_List*_T105;void*_T106;struct Cyc_Absyn_Exp*_T107;unsigned _T108;struct Cyc_Absyn_Exp*_T109;void*_T10A;void*_T10B;struct Cyc_Absyn_PtrInfo _T10C;struct Cyc_Absyn_PtrAtts _T10D;void*_T10E;struct Cyc_Absyn_Exp*_T10F;struct _tuple14 _T110;void*_T111;int*_T112;int _T113;void*_T114;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T115;void*_T116;int*_T117;int _T118;void*_T119;void*_T11A;void*_T11B;void*_T11C;enum Cyc_Absyn_Size_of _T11D;enum Cyc_Absyn_Size_of _T11E;int _T11F;enum Cyc_Absyn_Size_of _T120;enum Cyc_Absyn_Size_of _T121;int _T122;enum Cyc_Absyn_Size_of _T123;int _T124;enum Cyc_Absyn_Size_of _T125;int _T126;struct _tuple14 _T127;void*_T128;int*_T129;int _T12A;void*_T12B;void*_T12C;struct Cyc_Absyn_PtrInfo _T12D;struct Cyc_Absyn_PtrAtts _T12E;void*_T12F;struct Cyc_Absyn_PtrInfo _T130;struct Cyc_Absyn_PtrAtts _T131;void*_T132;void*_T133;struct Cyc_Absyn_PtrInfo _T134;struct Cyc_Absyn_PtrAtts _T135;void*_T136;void*_T137;struct Cyc_Absyn_PtrInfo _T138;struct Cyc_Absyn_PtrAtts _T139;void*_T13A;struct _tuple23 _T13B;struct Cyc_Absyn_Exp*_T13C;struct Cyc_Absyn_Exp*_T13D;struct Cyc_Vcgen_Env*_T13E;int _T13F;int _T140;int _T141;void*_T142;void*_T143;struct Cyc_Vcgen_ExpChecks*_T144;struct Cyc_Vcgen_ExpChecks*_T145;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T146;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T147;struct Cyc_Vcgen_ExpChecks*_T148;struct Cyc_Vcgen_Env*_T149;struct Cyc_Hashtable_Table**_T14A;struct Cyc_Absyn_Exp*_T14B;struct Cyc_Vcgen_ExpChecks*_T14C;struct Cyc_Vcgen_Env*_T14D;struct Cyc_AssnDef_AssnMap _T14E;void*_T14F;void*_T150;void*_T151;struct Cyc_AssnDef_AssnMap _T152;void*_T153;void*_T154;struct _tuple14 _T155;struct _tuple14 _T156;struct _tuple14 _T157;struct Cyc_Absyn_Exp*_T158;struct Cyc_Vcgen_Env*_T159;int _T15A;void*_T15B;struct Cyc_Vcgen_Env*_T15C;int _T15D;int _T15E;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T15F;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T160;void*_T161;void*_T162;struct Cyc_Vcgen_Env*_T163;struct Cyc_Absyn_Exp*_T164;struct Cyc_AssnDef_AssnMap _T165;struct Cyc_Absyn_PtrInfo _T166;struct Cyc_Absyn_PtrAtts _T167;void*_T168;struct Cyc_Absyn_PtrInfo _T169;struct Cyc_Absyn_PtrAtts _T16A;void*_T16B;int _T16C;int _T16D;void*_T16E;void*_T16F;void*_T170;void*_T171;void*_T172;struct Cyc_Vcgen_ExpChecks*_T173;struct Cyc_Vcgen_ExpChecks*_T174;struct Cyc_Vcgen_ExpChecks*_T175;struct Cyc_Vcgen_Env*_T176;struct Cyc_Hashtable_Table**_T177;struct Cyc_Absyn_Exp*_T178;struct Cyc_Vcgen_ExpChecks*_T179;struct Cyc_Vcgen_Env*_T17A;struct Cyc_AssnDef_AssnMap _T17B;void*_T17C;struct Cyc_AssnDef_AssnMap _T17D;void*_T17E;void*_T17F;struct _tuple14 _T180;struct _tuple14 _T181;struct Cyc_Vcgen_Env*_T182;int _T183;enum Cyc_Absyn_Coercion _T184;int _T185;struct _tuple14 _T186;struct Cyc_Vcgen_Env*_T187;struct Cyc_AssnDef_AssnMap _T188;void*_T189;void*_T18A;struct _tuple14 _T18B;struct _tuple14 _T18C;struct _tuple14 _T18D;struct Cyc_Absyn_Exp*_T18E;int*_T18F;unsigned _T190;int _T191;unsigned _T192;struct Cyc_Vcgen_Env*_T193;struct Cyc_Absyn_Exp*_T194;int _T195;unsigned _T196;void*_T197;struct Cyc_Absyn_Exp*_T198;void*_T199;struct Cyc_AssnDef_AssnMap _T19A;void*_T19B;void*_T19C;void*_T19D;void*_T19E;void*_T19F;void*_T1A0;void*_T1A1;struct _tuple14 _T1A2;struct Cyc_Vcgen_Env*_T1A3;struct Cyc_Absyn_Vardecl*_T1A4;struct Cyc_Absyn_Exp*_T1A5;struct Cyc_Absyn_Exp*_T1A6;struct Cyc_Absyn_Exp*_T1A7;void*_T1A8;void*_T1A9;struct Cyc_AssnDef_AssnMap _T1AA;struct Cyc_AssnDef_AssnMap _T1AB;void*_T1AC;void*_T1AD;void*_T1AE;struct _tuple14 _T1AF;struct Cyc_Vcgen_Env*_T1B0;struct Cyc_Absyn_Exp*_T1B1;void*_T1B2;struct Cyc_Absyn_Exp*_T1B3;void*_T1B4;struct Cyc_AssnDef_AssnMap _T1B5;void*_T1B6;void*_T1B7;struct Cyc_Absyn_Exp*_T1B8;void*_T1B9;void*_T1BA;void*_T1BB;void*_T1BC;void*_T1BD;void*_T1BE;void*_T1BF;void*_T1C0;struct _tuple14 _T1C1;struct _tuple14 _T1C2;struct Cyc_Absyn_Exp*_T1C3;void*_T1C4;void*_T1C5;struct _fat_ptr*_T1C6;struct Cyc_Vcgen_Env*_T1C7;int _T1C8;int _T1C9;int _T1CA;void*_T1CB;int _T1CC;unsigned _T1CD;void*_T1CE;struct Cyc_Vcgen_Env*_T1CF;struct Cyc_AssnDef_AssnMap _T1D0;void*_T1D1;struct Cyc_AssnDef_AssnMap _T1D2;void*_T1D3;void*_T1D4;struct _tuple14 _T1D5;void*_T1D6;int _T1D7;unsigned _T1D8;struct Cyc_Absyn_Exp*_T1D9;void*_T1DA;struct Cyc_Absyn_Exp*_T1DB;void*_T1DC;void*_T1DD;struct _fat_ptr*_T1DE;struct Cyc_Vcgen_Env*_T1DF;void*_T1E0;void*_T1E1;int _T1E2;unsigned _T1E3;void*_T1E4;struct Cyc_AssnDef_AssnMap _T1E5;struct _tuple14 _T1E6;struct _tuple14 _T1E7;struct Cyc_Vcgen_Env*_T1E8;struct Cyc_Absyn_Exp*_T1E9;struct Cyc_Absyn_Exp*_T1EA;struct Cyc_AssnDef_AssnMap _T1EB;struct _tuple14 _T1EC;void*_T1ED;void*_T1EE;struct Cyc_Absyn_Exp*_T1EF;void*_T1F0;struct Cyc_Vcgen_Env*_T1F1;struct Cyc_Absyn_Exp*_T1F2;struct Cyc_Absyn_Exp*_T1F3;struct Cyc_AssnDef_AssnMap _T1F4;struct Cyc_Absyn_Exp*_T1F5;void*_T1F6;void*_T1F7;void*_T1F8;struct _fat_ptr*_T1F9;struct Cyc_Vcgen_Env*_T1FA;int _T1FB;int _T1FC;int _T1FD;void*_T1FE;void*_T1FF;int _T200;unsigned _T201;void*_T202;struct Cyc_Vcgen_Env*_T203;struct Cyc_AssnDef_AssnMap _T204;void*_T205;struct Cyc_AssnDef_AssnMap _T206;void*_T207;void*_T208;struct Cyc_Absyn_Exp*_T209;void*_T20A;void*_T20B;struct _tuple14 _T20C;void*_T20D;int _T20E;unsigned _T20F;struct Cyc_Absyn_Exp*_T210;void*_T211;struct _tuple14 _T212;void*_T213;void*_T214;struct Cyc_Absyn_Exp*_T215;void*_T216;struct _tuple14 _T217;unsigned _T218;struct Cyc_List_List*_T219;struct Cyc_Absyn_Exp*_T21A;void*_T21B;struct Cyc_List_List*_T21C;void*_T21D;struct _tuple14 _T21E;struct Cyc_List_List*_T21F;struct _tuple14 _T220;struct Cyc_Absyn_Exp*_T221;void*_T222;struct Cyc_List_List*_T223;void*_T224;struct Cyc_List_List*_T225;struct Cyc_List_List*_T226;void*_T227;int*_T228;int _T229;struct Cyc_List_List*_T22A;struct Cyc_List_List*_T22B;struct Cyc_List_List*_T22C;struct Cyc_List_List*_T22D;void*_T22E;int _T22F;struct Cyc_List_List*_T230;struct _tuple21*_T231;int(*_T232)(struct _fat_ptr,struct _fat_ptr);void*(*_T233)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T234;struct _fat_ptr _T235;struct Cyc_List_List*_T236;struct Cyc_List_List*(*_T237)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*);struct Cyc_List_List*(*_T238)(int(*)(void*,void*),struct Cyc_List_List*);struct Cyc_List_List*_T239;struct Cyc_List_List*(*_T23A)(void*(*)(struct _tuple21*),struct Cyc_List_List*);struct Cyc_List_List*(*_T23B)(void*(*)(void*),struct Cyc_List_List*);void*(*_T23C)(struct _tuple21*);void*(*_T23D)(struct _tuple0*);struct Cyc_List_List*_T23E;void*_T23F;enum Cyc_Absyn_AggrKind _T240;int _T241;unsigned _T242;struct Cyc_List_List*_T243;struct Cyc_Absyn_Exp*_T244;void*_T245;struct Cyc_List_List*_T246;struct Cyc_Absyn_Exp*_T247;void*_T248;struct _tuple14 _T249;struct Cyc_Vcgen_Env*_T24A;struct Cyc_Absyn_Vardecl*_T24B;struct Cyc_Absyn_Exp*_T24C;struct Cyc_Absyn_Exp*_T24D;struct Cyc_Absyn_Exp*_T24E;void*_T24F;void*_T250;struct Cyc_AssnDef_AssnMap _T251;struct _tuple14 _T252;struct _tuple14 _T253;struct _tuple14 _T254;struct Cyc_Absyn_MallocInfo _T255;struct Cyc_Absyn_Exp*_T256;struct Cyc_Vcgen_Env*_T257;struct Cyc_Absyn_MallocInfo _T258;struct Cyc_Absyn_Exp*_T259;struct Cyc_AssnDef_AssnMap _T25A;struct _tuple14 _T25B;struct Cyc_Vcgen_Env*_T25C;struct Cyc_Absyn_MallocInfo _T25D;struct Cyc_Absyn_Exp*_T25E;struct Cyc_AssnDef_AssnMap _T25F;struct Cyc_Vcgen_Env*_T260;struct Cyc_Absyn_Exp*_T261;void*_T262;struct Cyc_Absyn_Exp*_T263;void*_T264;struct Cyc_AssnDef_AssnMap _T265;void*_T266;void*_T267;void*_T268;void*_T269;void*_T26A;void*_T26B;void*_T26C;struct _tuple14 _T26D;struct _tuple14 _T26E;struct Cyc_Vcgen_Env*_T26F;struct Cyc_Vcgen_Env*_T270;void**_T271;struct Cyc_Vcgen_Env*_T272;void**_T273;void**_T274;struct Cyc_Vcgen_Env*_T275;struct _tuple14 _T276;struct _tuple14 _T277;struct _tuple14 _T278;int _T279;struct Cyc_Vcgen_Env*_T27A;struct Cyc_Vcgen_Env*_T27B;struct Cyc_Vcgen_Env*_T27C;struct Cyc_Vcgen_Env*_T27D;struct Cyc_Vcgen_Env*_T27E;struct Cyc_Absyn_Exp*_T27F;struct Cyc_AssnDef_AssnMap _T280;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T281;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T282;struct Cyc_AssnDef_AssnMap _T283;struct Cyc_Vcgen_Env*_T284;struct Cyc_Vcgen_Env*_T285;struct Cyc_AssnDef_AssnMap _T286;void*_T287;struct Cyc_AssnDef_AssnMap _T288;void*_T289;int _T28A;struct Cyc_String_pa_PrintArg_struct _T28B;struct Cyc_AssnDef_AssnMap _T28C;void*_T28D;struct Cyc_String_pa_PrintArg_struct _T28E;struct Cyc_Absyn_Exp*_T28F;unsigned _T290;struct _fat_ptr _T291;struct _fat_ptr _T292;struct _tuple14 _T293;struct Cyc_AssnDef_AssnMap _T294;struct Cyc_AssnDef_AssnMap _T295;void*_T296;struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T297;int*_T298;struct Cyc_AssnDef_AssnMap _T299;void*_T29A;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T29B;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T29C;void*_T29D;int _T29E;int _T29F;int*_T2A0;int*_T2A1;int*_T2A2;int _T2A3;struct Cyc_Warn_String_Warn_Warg_struct _T2A4;struct Cyc_Warn_Exp_Warn_Warg_struct _T2A5;struct Cyc_Warn_String_Warn_Warg_struct _T2A6;struct Cyc_Absyn_Exp*_T2A7;unsigned _T2A8;struct _fat_ptr _T2A9;void*_T2AA;struct _tuple14 _T2AB;struct Cyc_Vcgen_Env*_T2AC;struct Cyc_Vcgen_Env*_T2AD;struct Cyc_Vcgen_Env*_T2AE;struct Cyc_Vcgen_Env*_T2AF;struct Cyc_Vcgen_Env*_T2B0;struct Cyc_Absyn_Exp*_T2B1;struct Cyc_AssnDef_AssnMap _T2B2;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2B3;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T2B4;struct Cyc_AssnDef_AssnMap _T2B5;struct Cyc_Vcgen_Env*_T2B6;struct Cyc_Vcgen_Env*_T2B7;struct Cyc_AssnDef_AssnMap _T2B8;void*_T2B9;struct Cyc_AssnDef_AssnMap _T2BA;void*_T2BB;int _T2BC;struct Cyc_String_pa_PrintArg_struct _T2BD;struct Cyc_String_pa_PrintArg_struct _T2BE;struct Cyc_Absyn_Exp*_T2BF;unsigned _T2C0;struct _fat_ptr _T2C1;struct _fat_ptr _T2C2;struct _tuple14 _T2C3;_T0=
Cyc_Tcutil_is_const_exp(e);if(!_T0)goto _TLA5;_T1=e;{
# 966
void*_T2C4=_T1->r;struct _fat_ptr _T2C5;_T2=(int*)_T2C4;_T3=*_T2;switch(_T3){case 2:{struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_T2C6=(struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_T2C4;_T2C5=_T2C6->f1;}{struct _fat_ptr s=_T2C5;_T4=s;_T5=
# 968
_tag_fat("print_assn",sizeof(char),11U);_T6=Cyc_strcmp(_T4,_T5);if(_T6!=0)goto _TLA8;_T7=e;_T8=_T7->loc;{
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(_T8);{struct Cyc_String_pa_PrintArg_struct _T2C6;_T2C6.tag=0;
_T2C6.f1=seg_str;_T9=_T2C6;}{struct Cyc_String_pa_PrintArg_struct _T2C6=_T9;{struct Cyc_String_pa_PrintArg_struct _T2C7;_T2C7.tag=0;_T2C7.f1=Cyc_AssnDef_assnmap2string(amin);_TA=_T2C7;}{struct Cyc_String_pa_PrintArg_struct _T2C7=_TA;void*_T2C8[2];_T2C8[0]=& _T2C6;_T2C8[1]=& _T2C7;_TB=Cyc_stderr;_TC=_tag_fat("%s: %s\n",sizeof(char),8U);_TD=_tag_fat(_T2C8,sizeof(void*),2);Cyc_fprintf(_TB,_TC,_TD);}}}goto _TLA9;_TLA8: _TLA9: _TE=s;_TF=
# 972
_tag_fat("print_dag",sizeof(char),10U);_T10=Cyc_strcmp(_TE,_TF);if(_T10!=0)goto _TLAA;_T11=e;_T12=_T11->loc;{
struct _fat_ptr seg_str=Cyc_Position_string_of_segment(_T12);{struct Cyc_String_pa_PrintArg_struct _T2C6;_T2C6.tag=0;
_T2C6.f1=seg_str;_T13=_T2C6;}{struct Cyc_String_pa_PrintArg_struct _T2C6=_T13;{struct Cyc_String_pa_PrintArg_struct _T2C7;_T2C7.tag=0;_T2C7.f1=Cyc_AssnDef_assnmap2dag(amin);_T14=_T2C7;}{struct Cyc_String_pa_PrintArg_struct _T2C7=_T14;void*_T2C8[2];_T2C8[0]=& _T2C6;_T2C8[1]=& _T2C7;_T15=Cyc_stderr;_T16=_tag_fat("%s: %s\n",sizeof(char),8U);_T17=_tag_fat(_T2C8,sizeof(void*),2);Cyc_fprintf(_T15,_T16,_T17);}}}goto _TLAB;_TLAA: _TLAB: _T18=s;_T19=
# 976
_tag_fat("all_paths",sizeof(char),10U);_T1A=Cyc_strcmp(_T18,_T19);if(_T1A!=0)goto _TLAC;_T1B=env;
_T1B->widen_paths=0;goto _TLAD;_TLAC: _TLAD: _T1C=s;_T1D=
# 979
_tag_fat("one_path",sizeof(char),9U);_T1E=Cyc_strcmp(_T1C,_T1D);if(_T1E!=0)goto _TLAE;_T1F=env;
_T1F->widen_paths=1;goto _TLAF;_TLAE: _TLAF:{struct _tuple14 _T2C6;
# 982
_T2C6.f0=Cyc_AssnDef_zero();_T2C6.f1=amin;_T20=_T2C6;}return _T20;}case 28: goto _LL6;case 29: _LL6: goto _LL8;case 30: _LL8: goto _LLA;case 26: _LLA: goto _LLC;case 25: _LLC: goto _LLE;case 14: _LLE: goto _LL10;case 15: _LL10: goto ANALYZE;default: goto _LL0;}_LL0:;}{
# 997
struct _tuple16 _T2C4=Cyc_Evexp_eval_const_uint_exp(e);int _T2C5;unsigned _T2C6;_T2C6=_T2C4.f0;_T2C5=_T2C4.f1;{unsigned cn=_T2C6;int known=_T2C5;_T21=known;
if(!_T21)goto _TLB0;{struct _tuple14 _T2C7;_T2C7.f0=Cyc_AssnDef_uint(cn);_T2C7.f1=amin;_T22=_T2C7;}return _T22;
_TLB0:{struct _tuple14 _T2C7;_T2C7.f0=Cyc_AssnDef_cnst(e);_T2C7.f1=amin;_T23=_T2C7;}return _T23;}}_TLA5:
# 1001
 ANALYZE: _T24=e;{
void*_T2C4=_T24->r;struct Cyc_Absyn_MallocInfo _T2C5;struct Cyc_Absyn_Datatypefield*_T2C6;struct Cyc_Absyn_Datatypedecl*_T2C7;int _T2C8;int _T2C9;struct _fat_ptr*_T2CA;enum Cyc_Absyn_Coercion _T2CB;struct Cyc_List_List*_T2CC;struct Cyc_Absyn_Exp*_T2CD;enum Cyc_Absyn_Incrementor _T2CE;struct Cyc_Core_Opt*_T2CF;struct Cyc_Absyn_Exp*_T2D0;enum Cyc_Absyn_Primop _T2D1;void*_T2D2;union Cyc_Absyn_Cnst _T2D3;_T25=(int*)_T2C4;_T26=*_T25;switch(_T26){case 0:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T2C4;_T2D3=_T2D4->f1;}{union Cyc_Absyn_Cnst c=_T2D3;goto _LL1A;}case 2: _LL1A: _T28=Cyc_Warn_impos;{
# 1005
int(*_T2D4)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T28;_T27=_T2D4;}_T29=_tag_fat("Const_e Pragma_e are const_exps\n",sizeof(char),33U);_T2A=_tag_fat(0U,sizeof(void*),0);_T27(_T29,_T2A);case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T2C4;_T2B=_T2D4->f1;_T2D2=(void*)_T2B;}{void*b=_T2D2;
# 1010
struct Cyc_Absyn_Vardecl*vdopt=Cyc_Vcgen_bind2vardecl(b);
if(vdopt!=0)goto _TLB3;{struct _tuple14 _T2D4;_T2D4.f0=Cyc_AssnDef_fresh_var(0,0);_T2D4.f1=amin;_T2C=_T2D4;}return _T2C;_TLB3: {
struct Cyc_Absyn_Vardecl*vd=vdopt;_T2D=vd;_T2E=_T2D->escapes;
if(!_T2E)goto _TLB5;{
struct _tuple14 _T2D4=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*mem=_T2D6;struct Cyc_AssnDef_AssnMap am1=_T2D5;{struct _tuple14 _T2D7;_T30=mem;_T31=
Cyc_Vcgen_myaddr(env,vd);_T32=vd;_T33=_T32->type;_T2D7.f0=Cyc_AssnDef_select(_T30,_T31,_T33);_T2D7.f1=am1;_T2F=_T2D7;}return _T2F;}}
# 1018
_TLB5: _T34=vd;_T35=_T34->type;{void*_T2D4=Cyc_Absyn_compress(_T35);_T36=(int*)_T2D4;_T37=*_T36;if(_T37!=5)goto _TLB7;{struct _tuple14 _T2D5;
# 1020
_T2D5.f0=Cyc_Vcgen_myaddr(env,vd);_T2D5.f1=amin;_T38=_T2D5;}return _T38;_TLB7: goto _LL76;_LL76:;}{
# 1023
struct _tuple14 _T2D4=Cyc_AssnDef_lookup_var_map(vd,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t=_T2D6;struct Cyc_AssnDef_AssnMap am=_T2D5;{struct _tuple14 _T2D7;
_T2D7.f0=t;_T2D7.f1=am;_T39=_T2D7;}return _T39;_T3A=
Cyc_AssnDef_lookup_var_map(vd,amin);return _T3A;}}}}case 3: _T3B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2C4;_T3C=_T3B->f2;if(_T3C==0)goto _TLB9;_T3D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2C4;_T3E=_T3D->f2;_T3F=(struct Cyc_List_List*)_T3E;_T40=_T3F->tl;if(_T40!=0)goto _TLBB;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2C4;_T2D1=_T2D4->f1;_T41=_T2D4->f2;{struct Cyc_List_List _T2D5=*_T41;_T42=_T2D5.hd;_T2D2=(struct Cyc_Absyn_Exp*)_T42;}}{enum Cyc_Absyn_Primop p=_T2D1;struct Cyc_Absyn_Exp*e1=_T2D2;
# 1029
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t=_T2D6;struct Cyc_AssnDef_AssnMap am=_T2D5;{struct _tuple14 _T2D7;_T44=p;_T45=t;_T46=e;_T47=_T46->topt;
_T2D7.f0=Cyc_AssnDef_unop(_T44,_T45,_T47);_T2D7.f1=am;_T43=_T2D7;}return _T43;}}_TLBB:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T2C4;_T2D1=_T2D4->f1;_T48=_T2D4->f2;{struct Cyc_List_List _T2D5=*_T48;_T49=_T2D5.hd;_T2D2=(struct Cyc_Absyn_Exp*)_T49;_T4A=_T2D5.tl;{struct Cyc_List_List _T2D6=*_T4A;_T4B=_T2D6.hd;_T2D0=(struct Cyc_Absyn_Exp*)_T4B;}}}{enum Cyc_Absyn_Primop p=_T2D1;struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_Absyn_Exp*e2=_T2D0;
# 1033
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t1=_T2D6;struct Cyc_AssnDef_AssnMap am1=_T2D5;
struct _tuple14 _T2D7=Cyc_Vcgen_vcgen_rexp(env,e2,am1);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*t2=_T2D9;struct Cyc_AssnDef_AssnMap am2=_T2D8;_T4C=env;_T4D=e;_T4E=_T4D->topt;_T4F=p;_T50=t1;_T51=t2;_T52=am2;_T53=
Cyc_Vcgen_vcgen_binop(_T4C,_T4E,_T4F,_T50,_T51,_T52);return _T53;}}}_TLB9: _T55=Cyc_Warn_impos;{
int(*_T2D4)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T55;_T54=_T2D4;}_T56=_tag_fat("vcgen: bad primop",sizeof(char),18U);_T57=_tag_fat(0U,sizeof(void*),0);_T54(_T56,_T57);;case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2CF=_T2D4->f2;_T2D0=_T2D4->f3;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_Core_Opt*popt=_T2CF;struct Cyc_Absyn_Exp*e2=_T2D0;
# 1039
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e2,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t2=_T2D6;struct Cyc_AssnDef_AssnMap am1=_T2D5;
struct _tuple14 _T2D7=Cyc_Vcgen_vcgen_lexp(env,e1,am1);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*lt=_T2D9;struct Cyc_AssnDef_AssnMap am2=_T2D8;
struct Cyc_AssnDef_AssnMap amout;
if(popt==0)goto _TLBD;{
struct _tuple14 _T2DA=Cyc_Vcgen_deref_lterm(env,lt,am2);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*t1=_T2DC;struct Cyc_AssnDef_AssnMap am3=_T2DB;_T58=env;_T59=e;_T5A=_T59->topt;_T5B=popt;_T5C=_T5B->v;_T5D=(enum Cyc_Absyn_Primop)_T5C;_T5E=t1;_T5F=t2;_T60=am3;{
struct _tuple14 _T2DD=Cyc_Vcgen_vcgen_binop(_T58,_T5A,_T5D,_T5E,_T5F,_T60);struct Cyc_AssnDef_AssnMap _T2DE;void*_T2DF;_T2DF=_T2DD.f0;_T2DE=_T2DD.f1;{void*t2=_T2DF;struct Cyc_AssnDef_AssnMap am4=_T2DE;
amout=Cyc_Vcgen_do_assign(env,am4,lt,t2);{struct _tuple14 _T2E0;
_T2E0.f0=t2;_T2E0.f1=amout;_T61=_T2E0;}return _T61;}}}}
# 1048
_TLBD: amout=Cyc_Vcgen_do_assign(env,am2,lt,t2);{struct _tuple14 _T2DA;
_T2DA.f0=t2;_T2DA.f1=amout;_T62=_T2DA;}return _T62;}}}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2CE=_T2D4->f2;}{struct Cyc_Absyn_Exp*e=_T2D2;enum Cyc_Absyn_Incrementor i=_T2CE;
# 1052
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_lexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*lt=_T2D6;struct Cyc_AssnDef_AssnMap am1=_T2D5;
struct _tuple14 _T2D7=Cyc_Vcgen_deref_lterm(env,lt,am1);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*t1=_T2D9;struct Cyc_AssnDef_AssnMap am2=_T2D8;
void*res;void*rvalue;
struct Cyc_AssnDef_AssnMap amout;_T63=i;_T64=(int)_T63;switch(_T64){case Cyc_Absyn_PostInc: _T65=env;_T66=e;_T67=_T66->topt;_T68=t1;_T69=
# 1058
Cyc_AssnDef_one();_T6A=am2;{struct _tuple14 _T2DA=Cyc_Vcgen_vcgen_binop(_T65,_T67,0U,_T68,_T69,_T6A);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*t=_T2DC;struct Cyc_AssnDef_AssnMap am=_T2DB;
res=t1;
rvalue=t;
amout=am;goto _LL99;}}case Cyc_Absyn_PreInc: _T6B=env;_T6C=e;_T6D=_T6C->topt;_T6E=t1;_T6F=
# 1064
Cyc_AssnDef_one();_T70=am2;{struct _tuple14 _T2DA=Cyc_Vcgen_vcgen_binop(_T6B,_T6D,0U,_T6E,_T6F,_T70);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*t=_T2DC;struct Cyc_AssnDef_AssnMap am=_T2DB;
rvalue=t;
res=rvalue;
amout=am;goto _LL99;}}case Cyc_Absyn_PostDec: _T71=env;_T72=e;_T73=_T72->topt;_T74=t1;_T75=
# 1070
Cyc_AssnDef_one();_T76=am2;{struct _tuple14 _T2DA=Cyc_Vcgen_vcgen_binop(_T71,_T73,2U,_T74,_T75,_T76);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*t=_T2DC;struct Cyc_AssnDef_AssnMap am=_T2DB;
res=t1;
rvalue=t;
amout=am;goto _LL99;}}case Cyc_Absyn_PreDec: _T77=env;_T78=e;_T79=_T78->topt;_T7A=t1;_T7B=
# 1076
Cyc_AssnDef_one();_T7C=am2;{struct _tuple14 _T2DA=Cyc_Vcgen_vcgen_binop(_T77,_T79,2U,_T7A,_T7B,_T7C);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*t=_T2DC;struct Cyc_AssnDef_AssnMap am=_T2DB;
rvalue=t;
res=rvalue;
amout=am;goto _LL99;}}default: _T7E=Cyc_Warn_impos;{
# 1082
int(*_T2DA)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7E;_T7D=_T2DA;}_T7F=_tag_fat("vcgen rexp increment_e",sizeof(char),23U);_T80=_tag_fat(0U,sizeof(void*),0);_T7D(_T7F,_T80);}_LL99:
# 1084
 amout=Cyc_Vcgen_do_assign(env,amout,lt,rvalue);{struct _tuple14 _T2DA;
_T2DA.f0=res;_T2DA.f1=amout;_T81=_T2DA;}return _T81;}}}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2D0=_T2D4->f2;_T2CD=_T2D4->f3;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_Absyn_Exp*e2=_T2D0;struct Cyc_Absyn_Exp*e3=_T2CD;
# 1087
struct _tuple18 _T2D4=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;struct Cyc_AssnDef_AssnMap _T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{struct Cyc_AssnDef_AssnMap amt=_T2D6;struct Cyc_AssnDef_AssnMap amf=_T2D5;
struct _tuple14 _T2D7=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*t1=_T2D9;struct Cyc_AssnDef_AssnMap amt=_T2D8;
struct _tuple14 _T2DA=Cyc_Vcgen_vcgen_rexp(env,e3,amf);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*t2=_T2DC;struct Cyc_AssnDef_AssnMap amf=_T2DB;_T82=e;_T83=_T82->topt;{
void*v=Cyc_AssnDef_fresh_var(0,_T83);_T84=amt;_T85=
Cyc_AssnDef_eq(v,t1);amt=Cyc_AssnDef_and_assnmap_assn(_T84,_T85);_T86=amf;_T87=
Cyc_AssnDef_eq(v,t2);amf=Cyc_AssnDef_and_assnmap_assn(_T86,_T87);{struct _tuple14 _T2DD;
_T2DD.f0=v;_T89=env;_T8A=Cyc_AssnDef_or_assnmap_assnmap(amt,amf);_T2DD.f1=Cyc_Vcgen_may_widen(_T89,_T8A);_T88=_T2DD;}return _T88;}}}}}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2D0=_T2D4->f2;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_Absyn_Exp*e2=_T2D0;
# 1095
struct _tuple18 _T2D4=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;struct Cyc_AssnDef_AssnMap _T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{struct Cyc_AssnDef_AssnMap amt=_T2D6;struct Cyc_AssnDef_AssnMap amf=_T2D5;
struct _tuple14 _T2D7=Cyc_Vcgen_vcgen_rexp(env,e2,amt);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*t=_T2D9;struct Cyc_AssnDef_AssnMap amt=_T2D8;_T8B=e;_T8C=_T8B->topt;{
void*v=Cyc_AssnDef_fresh_var(0,_T8C);_T8D=amt;_T8E=
Cyc_AssnDef_eq(v,t);amt=Cyc_AssnDef_and_assnmap_assn(_T8D,_T8E);_T8F=amf;_T90=v;_T91=
Cyc_AssnDef_zero();_T92=Cyc_AssnDef_eq(_T90,_T91);amf=Cyc_AssnDef_and_assnmap_assn(_T8F,_T92);{struct _tuple14 _T2DA;
_T2DA.f0=v;_T94=env;_T95=Cyc_AssnDef_or_assnmap_assnmap(amt,amf);_T2DA.f1=Cyc_Vcgen_may_widen(_T94,_T95);_T93=_T2DA;}return _T93;}}}}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2D0=_T2D4->f2;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_Absyn_Exp*e2=_T2D0;
# 1102
struct _tuple18 _T2D4=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;struct Cyc_AssnDef_AssnMap _T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{struct Cyc_AssnDef_AssnMap amt=_T2D6;struct Cyc_AssnDef_AssnMap amf=_T2D5;
struct _tuple14 _T2D7=Cyc_Vcgen_vcgen_rexp(env,e2,amf);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*t=_T2D9;struct Cyc_AssnDef_AssnMap amf=_T2D8;_T96=e;_T97=_T96->topt;{
void*v=Cyc_AssnDef_fresh_var(0,_T97);_T98=amt;_T99=v;_T9A=
Cyc_AssnDef_zero();_T9B=Cyc_AssnDef_neq(_T99,_T9A);amt=Cyc_AssnDef_and_assnmap_assn(_T98,_T9B);_T9C=amf;_T9D=
Cyc_AssnDef_eq(v,t);amf=Cyc_AssnDef_and_assnmap_assn(_T9C,_T9D);{struct _tuple14 _T2DA;
_T2DA.f0=v;_T9F=env;_TA0=Cyc_AssnDef_or_assnmap_assnmap(amt,amf);_T2DA.f1=Cyc_Vcgen_may_widen(_T9F,_TA0);_T9E=_T2DA;}return _T9E;}}}}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2D0=_T2D4->f2;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_Absyn_Exp*e2=_T2D0;
# 1109
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;_T2D5=_T2D4.f1;{struct Cyc_AssnDef_AssnMap am1=_T2D5;_TA1=
Cyc_Vcgen_vcgen_rexp(env,e2,am1);return _TA1;}}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2CC=_T2D4->f2;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_List_List*es=_T2CC;
# 1112
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*f=_T2D6;struct Cyc_AssnDef_AssnMap am1=_T2D5;
struct _tuple17 _T2D7=Cyc_Vcgen_vcgen_rexps(env,es,am1);struct Cyc_AssnDef_AssnMap _T2D8;struct Cyc_List_List*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{struct Cyc_List_List*ts=_T2D9;struct Cyc_AssnDef_AssnMap am2=_T2D8;
struct _tuple14 _T2DA=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am2);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*pre_memory=_T2DC;struct Cyc_AssnDef_AssnMap am2=_T2DB;_TA2=e1;_TA3=_TA2->topt;{
struct _tuple19 _T2DD=Cyc_Vcgen_get_requires_and_ensures(_TA3);struct Cyc_Absyn_Vardecl*_T2DE;struct Cyc_AssnDef_ExistAssnFn*_T2DF;struct Cyc_AssnDef_ExistAssnFn*_T2E0;struct Cyc_AssnDef_ExistAssnFn*_T2E1;struct Cyc_AssnDef_ExistAssnFn*_T2E2;struct Cyc_List_List*_T2E3;struct Cyc_List_List*_T2E4;_T2E4=_T2DD.f0;_T2E3=_T2DD.f1;_T2E2=_T2DD.f2;_T2E1=_T2DD.f3;_T2E0=_T2DD.f4;_T2DF=_T2DD.f5;_T2DE=_T2DD.f6;{struct Cyc_List_List*args=_T2E4;struct Cyc_List_List*argvds=_T2E3;struct Cyc_AssnDef_ExistAssnFn*checkopt=_T2E2;struct Cyc_AssnDef_ExistAssnFn*reqopt=_T2E1;struct Cyc_AssnDef_ExistAssnFn*ensopt=_T2E0;struct Cyc_AssnDef_ExistAssnFn*thrwsopt=_T2DF;struct Cyc_Absyn_Vardecl*ret_value=_T2DE;_TA4=env;_TA5=_TA4->fn_precond_info;_TA6=e1;_TA7=am2;_TA8=_TA7.assn;_TA9=pre_memory;_TAA=ts;
# 1121
Cyc_Vcgen_insert_fn_precond_info(_TA5,_TA6,_TA8,_TA9,_TAA);{
struct Cyc_AssnDef_ExistAssnFn*fensinfo=Cyc_Vcgen_lookup_fn_summary_info(e1);_TAB=e;_TAC=_TAB->topt;{
# 1127
void*actual_ret=Cyc_AssnDef_fresh_var(0,_TAC);
# 1129
struct Cyc_List_List*pre_memory_and_ts;pre_memory_and_ts=_cycalloc(sizeof(struct Cyc_List_List));_TAD=pre_memory_and_ts;_TAD->hd=pre_memory;_TAE=pre_memory_and_ts;_TAE->tl=ts;
if(reqopt==0)goto _TLC0;{
# 1132
void*req_assn=Cyc_AssnDef_existassnfn2assn(reqopt,pre_memory_and_ts);_TAF=am2;_TB0=_TAF.assn;_TB1=req_assn;_TB2=
Cyc_Vcgen_implies(_TB0,_TB1);if(_TB2)goto _TLC2;else{goto _TLC4;}
_TLC4:{struct Cyc_String_pa_PrintArg_struct _T2E5;_T2E5.tag=0;
_T2E5.f1=Cyc_AssnDef_assnmap2string(am2);_TB3=_T2E5;}{struct Cyc_String_pa_PrintArg_struct _T2E5=_TB3;{struct Cyc_String_pa_PrintArg_struct _T2E6;_T2E6.tag=0;_T2E6.f1=Cyc_AssnDef_assn2string(req_assn);_TB4=_T2E6;}{struct Cyc_String_pa_PrintArg_struct _T2E6=_TB4;void*_T2E7[2];_T2E7[0]=& _T2E5;_T2E7[1]=& _T2E6;_TB5=e;_TB6=_TB5->loc;_TB7=
# 1134
_tag_fat("current assertion:\n%s\ndoes not satisfy the pre-condition of the function:\n%s\n",sizeof(char),78U);_TB8=_tag_fat(_T2E7,sizeof(void*),2);Cyc_Warn_err(_TB6,_TB7,_TB8);}}goto _TLC3;_TLC2: _TLC3:;}goto _TLC1;_TLC0: _TLC1: {
# 1141
void*new_memory=Cyc_AssnDef_fresh_var(Cyc_AssnDef_memory,0);
struct Cyc_AssnDef_AssnMap am3=Cyc_AssnDef_update_var_map(Cyc_AssnDef_memory,new_memory,am2);_TB9=
# 1145
Cyc_Absyn_exn_type();{void*v=Cyc_AssnDef_fresh_var(0,_TB9);
if(thrwsopt==0)goto _TLC5;_TBA=thrwsopt;{struct Cyc_List_List*_T2E5=_cycalloc(sizeof(struct Cyc_List_List));
_T2E5->hd=new_memory;_T2E5->tl=pre_memory_and_ts;_TBB=(struct Cyc_List_List*)_T2E5;}{void*throws_assn=Cyc_AssnDef_existassnfn2assn(_TBA,_TBB);_TBC=env;_TBD=
Cyc_AssnDef_and_assnmap_assn(am3,throws_assn);_TBE=v;Cyc_Vcgen_update_try_assnmap(_TBC,_TBD,_TBE);}goto _TLC6;
# 1150
_TLC5: Cyc_Vcgen_update_try_assnmap(env,am3,v);_TLC6:
# 1153
 if(ensopt==0)goto _TLC7;_TBF=ensopt;{struct Cyc_List_List*_T2E5=_cycalloc(sizeof(struct Cyc_List_List));
_T2E5->hd=actual_ret;{struct Cyc_List_List*_T2E6=_cycalloc(sizeof(struct Cyc_List_List));_T2E6->hd=new_memory;_T2E6->tl=pre_memory_and_ts;_TC1=(struct Cyc_List_List*)_T2E6;}_T2E5->tl=_TC1;_TC0=(struct Cyc_List_List*)_T2E5;}{void*ens_assn=Cyc_AssnDef_existassnfn2assn(_TBF,_TC0);
am3=Cyc_AssnDef_and_assnmap_assn(am3,ens_assn);}goto _TLC8;
_TLC7: if(fensinfo==0)goto _TLC9;_TC2=fensinfo;_TC3=_TC2->af;_TC4=_TC3->actuals;_TC5=
Cyc_List_length(_TC4);_TC6=Cyc_List_length(ts);_TC7=_TC6 + 3;if(_TC5==_TC7)goto _TLCB;goto _TLCC;
# 1162
_TLCB: _TC8=fensinfo;{struct Cyc_List_List*_T2E5=_cycalloc(sizeof(struct Cyc_List_List));_T2E5->hd=actual_ret;{struct Cyc_List_List*_T2E6=_cycalloc(sizeof(struct Cyc_List_List));_T2E6->hd=new_memory;_T2E6->tl=pre_memory_and_ts;_TCA=(struct Cyc_List_List*)_T2E6;}_T2E5->tl=_TCA;_TC9=(struct Cyc_List_List*)_T2E5;}{void*ens_assn=Cyc_AssnDef_existassnfn2assn(_TC8,_TC9);
am3=Cyc_AssnDef_and_assnmap_assn(am3,ens_assn);}_TLCC: goto _TLCA;_TLC9: _TLCA: _TLC8: _TCB=e1;_TCC=_TCB->topt;_TCD=
# 1168
_check_null(_TCC);_TCE=Cyc_Atts_is_noreturn_fn_type(_TCD);if(!_TCE)goto _TLCD;_TCF=& Cyc_AssnDef_false_assn;_TD0=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_TCF;
am3.assn=(void*)_TD0;goto _TLCE;_TLCD: _TLCE:{struct _tuple14 _T2E5;
_T2E5.f0=actual_ret;_T2E5.f1=am3;_TD1=_T2E5;}return _TD1;}}}}}}}}}}case 11:{struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;}{struct Cyc_Absyn_Exp*e=_T2D2;
# 1172
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t=_T2D6;struct Cyc_AssnDef_AssnMap am1=_T2D5;
Cyc_Vcgen_update_try_assnmap(env,am1,t);{struct _tuple14 _T2D7;
_T2D7.f0=Cyc_AssnDef_fresh_var(0,0);_T2D7.f1=Cyc_AssnDef_false_assnmap();_TD2=_T2D7;}return _TD2;}}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;}{struct Cyc_Absyn_Exp*e=_T2D2;_TD3=
Cyc_Vcgen_vcgen_rexp(env,e,amin);return _TD3;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;}{struct Cyc_Absyn_Exp*e=_T2D2;_TD4=
Cyc_Vcgen_vcgen_rexp(env,e,amin);return _TD4;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T2C4;_TD5=_T2D4->f1;_T2D2=(void*)_TD5;_T2D0=_T2D4->f2;_T2CB=_T2D4->f4;}{void*tp=_T2D2;struct Cyc_Absyn_Exp*e1=_T2D0;enum Cyc_Absyn_Coercion c=_T2CB;_TD6=
# 1181
Cyc_Tcutil_is_const_exp(e1);if(!_TD6)goto _TLCF;{
struct _tuple16 _T2D4=Cyc_Evexp_eval_const_uint_exp(e);int _T2D5;unsigned _T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{unsigned cn=_T2D6;int known=_T2D5;_TD7=known;
if(!_TD7)goto _TLD1;{struct _tuple14 _T2D7;_T2D7.f0=Cyc_AssnDef_uint(cn);_T2D7.f1=amin;_TD8=_T2D7;}return _TD8;_TLD1:;}}goto _TLD0;_TLCF: _TLD0: {
# 1185
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t=_T2D6;struct Cyc_AssnDef_AssnMap am=_T2D5;{struct _tuple0 _T2D7;
# 1192
_T2D7.f0=Cyc_Absyn_compress(tp);_TDA=e1;_TDB=_TDA->topt;_TDC=_check_null(_TDB);_T2D7.f1=Cyc_Absyn_compress(_TDC);_TD9=_T2D7;}{struct _tuple0 _T2D7=_TD9;struct Cyc_Absyn_PtrInfo _T2D8;enum Cyc_Absyn_Size_of _T2D9;enum Cyc_Absyn_Sign _T2DA;enum Cyc_Absyn_Size_of _T2DB;enum Cyc_Absyn_Sign _T2DC;struct Cyc_Absyn_PtrInfo _T2DD;struct Cyc_List_List*_T2DE;_TDD=_T2D7.f0;_TDE=(int*)_TDD;_TDF=*_TDE;switch(_TDF){case 0: _TE0=_T2D7.f0;_TE1=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE0;_TE2=_TE1->f1;_TE3=(int*)_TE2;_TE4=*_TE3;if(_TE4!=1)goto _TLD4;_TE5=_T2D7.f0;_TE6=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TE5;_TE7=_TE6->f1;_TE8=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TE7;_TE9=_TE8->f2;_TEA=(int)_TE9;switch(_TEA){case Cyc_Absyn_Int_sz: _TEB=_T2D7.f1;_TEC=(int*)_TEB;_TED=*_TEC;switch(_TED){case 0: _TEE=_T2D7.f1;_TEF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TEE;_TF0=_TEF->f1;_TF1=(int*)_TF0;_TF2=*_TF1;switch(_TF2){case 5: _TF3=_T2D7.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2DF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF3;_T2DE=_T2DF->f2;}{struct Cyc_List_List*tagtps=_T2DE;{struct _tuple14 _T2DF;_TF5=
# 1196
_check_null(tagtps);_TF6=_TF5->hd;_TF7=e1;_TF8=_TF7->loc;_TF9=Cyc_Absyn_valueof_exp(_TF6,_TF8);_T2DF.f0=Cyc_AssnDef_cnst(_TF9);_T2DF.f1=am;_TF4=_T2DF;}return _TF4;}case 1: goto _LLE2;default: goto _LLEA;};case 4: _TFA=_T2D7.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2DF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TFA;_T2DD=_T2DF->f1;}{struct Cyc_Absyn_PtrInfo pi=_T2DD;_T2DD=pi;goto _LLE7;}default: goto _LLEA;};case Cyc_Absyn_Long_sz: _TFB=_T2D7.f1;_TFC=(int*)_TFB;_TFD=*_TFC;switch(_TFD){case 0: _TFE=_T2D7.f1;_TFF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TFE;_T100=_TFF->f1;_T101=(int*)_T100;_T102=*_T101;switch(_T102){case 5: _T103=_T2D7.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2DF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T103;_T2DE=_T2DF->f2;}{struct Cyc_List_List*tagtps=_T2DE;{struct _tuple14 _T2DF;_T105=
# 1198
_check_null(tagtps);_T106=_T105->hd;_T107=e1;_T108=_T107->loc;_T109=Cyc_Absyn_valueof_exp(_T106,_T108);_T2DF.f0=Cyc_AssnDef_cnst(_T109);_T2DF.f1=am;_T104=_T2DF;}return _T104;}case 1: goto _LLE2;default: goto _LLEA;};case 4: _T10A=_T2D7.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2DF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T10A;_T2DD=_T2DF->f1;}_LLE7: {struct Cyc_Absyn_PtrInfo pi=_T2DD;_T10B=
# 1211
Cyc_Absyn_bounds_one();_T10C=pi;_T10D=_T10C.ptr_atts;_T10E=_T10D.bounds;_T10F=Cyc_Tcutil_get_bounds_exp_constrain(_T10B,_T10E,1);if(_T10F==0)goto _TLDB;{struct _tuple14 _T2DF;
# 1215
_T2DF.f0=t;_T2DF.f1=am;_T110=_T2DF;}return _T110;_TLDB: goto _LLDD;}default: goto _LLEA;};default: _T111=_T2D7.f1;_T112=(int*)_T111;_T113=*_T112;if(_T113!=0)goto _TLDD;_T114=_T2D7.f1;_T115=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T114;_T116=_T115->f1;_T117=(int*)_T116;_T118=*_T117;if(_T118!=1)goto _TLDF;_LLE2: _T119=_T2D7.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2DF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T119;_T11A=_T2DF->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T2E0=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T11A;_T2DC=_T2E0->f1;_T2DB=_T2E0->f2;}}_T11B=_T2D7.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2DF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11B;_T11C=_T2DF->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T2E0=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T11C;_T2DA=_T2E0->f1;_T2D9=_T2E0->f2;}}{enum Cyc_Absyn_Sign u1=_T2DC;enum Cyc_Absyn_Size_of s1=_T2DB;enum Cyc_Absyn_Sign u2=_T2DA;enum Cyc_Absyn_Size_of s2=_T2D9;_T11E=s1;_T11F=(int)_T11E;
# 1205
if(_T11F!=3)goto _TLE1;_T11D=2U;goto _TLE2;_TLE1: _T11D=s1;_TLE2: s1=_T11D;_T121=s2;_T122=(int)_T121;
if(_T122!=3)goto _TLE3;_T120=2U;goto _TLE4;_TLE3: _T120=s2;_TLE4: s2=_T120;_T123=s1;_T124=(int)_T123;_T125=s2;_T126=(int)_T125;
if(_T124!=_T126)goto _TLE5;{struct _tuple14 _T2DF;_T2DF.f0=t;_T2DF.f1=am;_T127=_T2DF;}return _T127;_TLE5: goto _LLDD;}_TLDF: goto _LLEA;_TLDD: goto _LLEA;}goto _TLD5;_TLD4: goto _LLEA;_TLD5:;case 4: _T128=_T2D7.f1;_T129=(int*)_T128;_T12A=*_T129;if(_T12A!=4)goto _TLE7;_T12B=_T2D7.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2DF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T12B;_T2DD=_T2DF->f1;}_T12C=_T2D7.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2DF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T12C;_T2D8=_T2DF->f1;}{struct Cyc_Absyn_PtrInfo p2=_T2DD;struct Cyc_Absyn_PtrInfo p1=_T2D8;_T12D=p1;_T12E=_T12D.ptr_atts;_T12F=_T12E.nullable;{
# 1219
int n1=Cyc_Tcutil_force_type2bool(0,_T12F);_T130=p2;_T131=_T130.ptr_atts;_T132=_T131.nullable;{
int n2=Cyc_Tcutil_force_type2bool(0,_T132);_T133=
Cyc_Absyn_bounds_one();_T134=p1;_T135=_T134.ptr_atts;_T136=_T135.bounds;{struct Cyc_Absyn_Exp*b1=Cyc_Tcutil_get_bounds_exp_constrain(_T133,_T136,1);_T137=
Cyc_Absyn_bounds_one();_T138=p2;_T139=_T138.ptr_atts;_T13A=_T139.bounds;{struct Cyc_Absyn_Exp*b2=Cyc_Tcutil_get_bounds_exp_constrain(_T137,_T13A,1);{struct _tuple23 _T2DF;
_T2DF.f0=b1;_T2DF.f1=b2;_T13B=_T2DF;}{struct _tuple23 _T2DF=_T13B;_T13C=_T2DF.f0;if(_T13C==0)goto _TLE9;_T13D=_T2DF.f1;if(_T13D==0)goto _TLEB;_T13E=env;_T13F=_T13E->pure_exp;if(_T13F)goto _TLED;else{goto _TLEF;}_TLEF: _T140=n1;
# 1228
if(!_T140)goto _TLF0;_T141=n2;if(_T141)goto _TLF0;else{goto _TLF2;}
_TLF2: _T142=Cyc_AssnDef_zero();_T143=t;{void*null_check=Cyc_AssnDef_neq(_T142,_T143);
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));_T144=exp_checks;_T144->ctxt=am;_T145=exp_checks;_T146=& Cyc_AssnDef_true_assn;_T147=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T146;_T145->bounds_check=(void*)_T147;_T148=exp_checks;
_T148->null_check=null_check;_T149=env;_T14A=_T149->assn_info;_T14B=e1;_T14C=exp_checks;
Cyc_Vcgen_insert_exp_checks(_T14A,_T14B,_T14C);_T14D=env;_T14E=am;_T14F=t;_T150=
Cyc_AssnDef_zero();_T151=Cyc_AssnDef_eq(_T14F,_T150);_T152=Cyc_AssnDef_and_assnmap_assn(_T14E,_T151);_T153=Cyc_Absyn_exn_type();_T154=Cyc_AssnDef_fresh_var(0,_T153);Cyc_Vcgen_update_try_assnmap(_T14D,_T152,_T154);{struct _tuple14 _T2E0;
_T2E0.f0=t;_T2E0.f1=Cyc_AssnDef_and_assnmap_assn(am,null_check);_T155=_T2E0;}return _T155;}
_TLF0:{struct _tuple14 _T2E0;_T2E0.f0=t;_T2E0.f1=am;_T156=_T2E0;}return _T156;_TLED: goto _LLF3;_TLEB:{struct _tuple14 _T2E0;
# 1237
_T2E0.f0=t;_T2E0.f1=am;_T157=_T2E0;}return _T157;_TLE9: _T158=_T2DF.f1;if(_T158==0)goto _TLF3;_T159=env;_T15A=_T159->pure_exp;if(_T15A)goto _TLF5;else{goto _TLF7;}_TLF7: _T15C=env;_T15D=_T15C->pure_exp;
# 1241
if(_T15D)goto _TLFA;else{goto _TLFB;}_TLFB: _T15E=n2;if(_T15E)goto _TLFA;else{goto _TLF8;}_TLFA: _T15F=& Cyc_AssnDef_true_assn;_T160=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T15F;_T15B=(void*)_T160;goto _TLF9;_TLF8: _T161=
Cyc_AssnDef_zero();_T162=t;_T15B=Cyc_AssnDef_neq(_T161,_T162);_TLF9:{
# 1241
void*null_check=_T15B;_T163=env;_T164=
# 1243
_check_null(b2);_T165=am;{struct _tuple14 _T2E0=Cyc_Vcgen_vcgen_rexp(_T163,_T164,_T165);void*_T2E1;_T2E1=_T2E0.f0;{void*b=_T2E1;_T166=p1;_T167=_T166.ptr_atts;_T168=_T167.zero_term;{
# 1252 "vcgen.cyc"
int zt1=Cyc_Tcutil_force_type2bool(0,_T168);_T169=p2;_T16A=_T169.ptr_atts;_T16B=_T16A.zero_term;{
int zt2=Cyc_Tcutil_force_type2bool(0,_T16B);_T16C=zt1;
if(!_T16C)goto _TLFC;_T16D=zt2;if(_T16D)goto _TLFC;else{goto _TLFE;}
_TLFE: _T16E=b;_T16F=Cyc_AssnDef_one();_T170=Cyc_AssnDef_get_term_type(b);b=Cyc_AssnDef_plus(_T16E,_T16F,_T170);goto _TLFD;_TLFC: _TLFD: _T171=b;_T172=
# 1257
Cyc_AssnDef_numelts_term(t);{void*bcheck=Cyc_AssnDef_ulte(_T171,_T172);
struct Cyc_Vcgen_ExpChecks*exp_checks;exp_checks=_cycalloc(sizeof(struct Cyc_Vcgen_ExpChecks));_T173=exp_checks;_T173->ctxt=am;_T174=exp_checks;_T174->bounds_check=bcheck;_T175=exp_checks;
_T175->null_check=null_check;_T176=env;_T177=_T176->assn_info;_T178=e1;_T179=exp_checks;
Cyc_Vcgen_insert_exp_checks(_T177,_T178,_T179);{
void*checks=Cyc_AssnDef_and(null_check,bcheck);_T17A=env;_T17B=am;_T17C=
Cyc_AssnDef_not(checks);_T17D=Cyc_AssnDef_and_assnmap_assn(_T17B,_T17C);_T17E=Cyc_Absyn_exn_type();_T17F=Cyc_AssnDef_fresh_var(0,_T17E);Cyc_Vcgen_update_try_assnmap(_T17A,_T17D,_T17F);{struct _tuple14 _T2E2;
_T2E2.f0=t;_T2E2.f1=Cyc_AssnDef_and_assnmap_assn(am,checks);_T180=_T2E2;}return _T180;}}}}}}}_TLF5: goto _LLF3;_TLF3: _LLF3:{struct _tuple14 _T2E0;
# 1265
_T2E0.f0=t;_T2E0.f1=am;_T181=_T2E0;}return _T181;;}}}}}}goto _TLE8;_TLE7: goto _LLEA;_TLE8:;default: _LLEA: goto _LLDD;}_LLDD:;}_T182=env;_T183=_T182->pure_exp;
# 1269
if(_T183)goto _TL101;else{goto _TL102;}_TL102: _T184=c;_T185=(int)_T184;if(_T185==1)goto _TL101;else{goto _TLFF;}
_TL101:{struct _tuple14 _T2D7;_T2D7.f0=Cyc_AssnDef_cast(tp,t);_T2D7.f1=am;_T186=_T2D7;}return _T186;
# 1273
_TLFF: _T187=env;_T188=amin;_T189=Cyc_Absyn_exn_type();_T18A=Cyc_AssnDef_fresh_var(0,_T189);Cyc_Vcgen_update_try_assnmap(_T187,_T188,_T18A);{struct _tuple14 _T2D7;
_T2D7.f0=t;_T2D7.f1=am;_T18B=_T2D7;}return _T18B;}}}case 16:{struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2D0=_T2D4->f2;_T2CD=_T2D4->f3;}{struct Cyc_Absyn_Exp*eopt=_T2D2;struct Cyc_Absyn_Exp*e1=_T2D0;struct Cyc_Absyn_Exp*qopt=_T2CD;
# 1277
if(eopt==0)goto _TL103;_T18C=
Cyc_Vcgen_vcgen_rexp(env,eopt,amin);amin=_T18C.f1;goto _TL104;_TL103: _TL104:
 if(qopt==0)goto _TL105;_T18D=
Cyc_Vcgen_vcgen_rexp(env,qopt,amin);amin=_T18D.f1;goto _TL106;_TL105: _TL106: _T18E=e1;{
# 1282
void*_T2D4=_T18E->r;struct Cyc_Absyn_Exp*_T2D5;struct Cyc_Absyn_Exp*_T2D6;struct Cyc_Absyn_Vardecl*_T2D7;struct Cyc_List_List*_T2D8;_T18F=(int*)_T2D4;_T190=*_T18F;switch(_T190){case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T2D9=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T2D4;_T2D8=_T2D9->f1;}{struct Cyc_List_List*dles=_T2D8;
# 1287
struct _tuple14 _T2D9=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2DA;_T2DA=_T2D9.f1;{struct Cyc_AssnDef_AssnMap amin=_T2DA;_T191=
Cyc_List_length(dles);_T192=(unsigned)_T191;{void*size=Cyc_AssnDef_uint(_T192);_T193=env;_T194=e;_T195=
Cyc_List_length(dles);_T196=(unsigned)_T195;_T197=Cyc_AssnDef_uint(_T196);_T198=e;_T199=_T198->topt;{void*res=Cyc_Vcgen_myalloc(_T193,_T194,_T197,_T199);_T19A=amin;_T19B=res;_T19C=
Cyc_AssnDef_zero();_T19D=Cyc_AssnDef_neq(_T19B,_T19C);_T19E=
Cyc_AssnDef_numelts_term(res);_T19F=size;_T1A0=Cyc_AssnDef_eq(_T19E,_T19F);_T1A1=
# 1290
Cyc_AssnDef_and(_T19D,_T1A0);amin=Cyc_AssnDef_and_assnmap_assn(_T19A,_T1A1);{struct _tuple14 _T2DB;
# 1294
_T2DB.f0=res;_T2DB.f1=amin;_T1A2=_T2DB;}return _T1A2;}}}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T2D9=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T2D4;_T2D7=_T2D9->f1;_T2D6=_T2D9->f2;_T2D5=_T2D9->f3;}{struct Cyc_Absyn_Vardecl*vd=_T2D7;struct Cyc_Absyn_Exp*e1=_T2D6;struct Cyc_Absyn_Exp*e2=_T2D5;_T1A3=env;_T1A4=vd;_T1A5=e1;_T1A6=e2;_T1A7=e;_T1A8=_T1A7->topt;_T1A9=
# 1296
_check_null(_T1A8);_T1AA=amin;{struct _tuple14 _T2D9=Cyc_Vcgen_vcgen_comprehension(_T1A3,_T1A4,_T1A5,_T1A6,_T1A9,_T1AA);struct Cyc_AssnDef_AssnMap _T2DA;void*_T2DB;_T2DB=_T2D9.f0;_T2DA=_T2D9.f1;{void*res=_T2DB;struct Cyc_AssnDef_AssnMap amin=_T2DA;_T1AB=amin;_T1AC=res;_T1AD=
Cyc_AssnDef_zero();_T1AE=Cyc_AssnDef_neq(_T1AC,_T1AD);amin=Cyc_AssnDef_and_assnmap_assn(_T1AB,_T1AE);{struct _tuple14 _T2DC;
_T2DC.f0=res;_T2DC.f1=amin;_T1AF=_T2DC;}return _T1AF;}}}default:  {
# 1300
struct _tuple14 _T2D9=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2DA;void*_T2DB;_T2DB=_T2D9.f0;_T2DA=_T2D9.f1;{void*t=_T2DB;struct Cyc_AssnDef_AssnMap amin=_T2DA;_T1B0=env;_T1B1=e;_T1B2=
Cyc_AssnDef_one();_T1B3=e;_T1B4=_T1B3->topt;{void*res=Cyc_Vcgen_myalloc(_T1B0,_T1B1,_T1B2,_T1B4);
struct _tuple14 _T2DC=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T2DD;void*_T2DE;_T2DE=_T2DC.f0;_T2DD=_T2DC.f1;{void*mem=_T2DE;struct Cyc_AssnDef_AssnMap amin=_T2DD;_T1B5=amin;_T1B6=mem;_T1B7=res;_T1B8=e1;_T1B9=_T1B8->topt;_T1BA=
Cyc_AssnDef_select(_T1B6,_T1B7,_T1B9);_T1BB=t;_T1BC=Cyc_AssnDef_eq(_T1BA,_T1BB);_T1BD=res;_T1BE=
Cyc_AssnDef_zero();_T1BF=Cyc_AssnDef_neq(_T1BD,_T1BE);_T1C0=
# 1303
Cyc_AssnDef_and(_T1BC,_T1BF);amin=Cyc_AssnDef_and_assnmap_assn(_T1B5,_T1C0);{struct _tuple14 _T2DF;
# 1305
_T2DF.f0=res;_T2DF.f1=amin;_T1C1=_T2DF;}return _T1C1;}}}}};}}case 31: goto _LL3E;case 32: _LL3E: goto _LL40;case 17: _LL40: goto _LL42;case 18: _LL42: goto _LL44;case 38: _LL44: goto _LL46;case 19: _LL46:{struct _tuple14 _T2D4;
# 1312
_T2D4.f0=Cyc_AssnDef_cnst(e);_T2D4.f1=amin;_T1C2=_T2D4;}return _T1C2;case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2CA=_T2D4->f2;_T2C9=_T2D4->f3;_T2C8=_T2D4->f4;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct _fat_ptr*f=_T2CA;int is_tagged=_T2C9;int is_read=_T2C8;
# 1314
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t=_T2D6;struct Cyc_AssnDef_AssnMap amin=_T2D5;_T1C3=e1;_T1C4=_T1C3->topt;_T1C5=
_check_null(_T1C4);_T1C6=f;{int i=Cyc_CfFlowInfo_get_field_index(_T1C5,_T1C6);_T1C7=env;_T1C8=_T1C7->pure_exp;
# 1317
if(_T1C8)goto _TL108;else{goto _TL10A;}_TL10A: _T1C9=is_tagged;if(!_T1C9)goto _TL108;_T1CA=is_read;if(!_T1CA)goto _TL108;_T1CB=
# 1319
Cyc_AssnDef_tagof_tm(t);_T1CC=i;_T1CD=(unsigned)_T1CC;_T1CE=Cyc_AssnDef_uint(_T1CD);{void*ck=Cyc_AssnDef_eq(_T1CB,_T1CE);_T1CF=env;_T1D0=amin;_T1D1=
Cyc_AssnDef_not(ck);_T1D2=Cyc_AssnDef_and_assnmap_assn(_T1D0,_T1D1);_T1D3=Cyc_Absyn_exn_type();_T1D4=Cyc_AssnDef_fresh_var(0,_T1D3);Cyc_Vcgen_update_try_assnmap(_T1CF,_T1D2,_T1D4);
amin=Cyc_AssnDef_and_assnmap_assn(amin,ck);}goto _TL109;_TL108: _TL109:{struct _tuple14 _T2D7;_T1D6=t;_T1D7=i;_T1D8=(unsigned)_T1D7;_T1D9=e;_T1DA=_T1D9->topt;
# 1323
_T2D7.f0=Cyc_AssnDef_proj(_T1D6,_T1D8,_T1DA);_T2D7.f1=amin;_T1D5=_T2D7;}return _T1D5;}}}case 37:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2CA=_T2D4->f2;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct _fat_ptr*f=_T2CA;_T1DB=e1;_T1DC=_T1DB->topt;_T1DD=
# 1326
_check_null(_T1DC);_T1DE=f;{int i=Cyc_CfFlowInfo_get_field_index(_T1DD,_T1DE);
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t=_T2D6;struct Cyc_AssnDef_AssnMap am=_T2D5;_T1DF=env;_T1E0=Cyc_Absyn_sint_type;_T1E1=
Cyc_AssnDef_tagof_tm(t);_T1E2=i;_T1E3=(unsigned)_T1E2;_T1E4=Cyc_AssnDef_uint(_T1E3);_T1E5=am;_T1E6=Cyc_Vcgen_vcgen_binop(_T1DF,_T1E0,5U,_T1E1,_T1E4,_T1E5);return _T1E6;}}}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;}{struct Cyc_Absyn_Exp*e1=_T2D2;_T1E7=
# 1330
Cyc_Vcgen_vcgen_lexp(env,e1,amin);return _T1E7;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;}{struct Cyc_Absyn_Exp*e1=_T2D2;_T1E8=env;_T1E9=e1;_T1EA=
# 1332
Cyc_Vcgen_zero_exp();_T1EB=amin;{struct _tuple22 _T2D4=Cyc_Vcgen_vcgen_deref(_T1E8,_T1E9,_T1EA,_T1EB);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f2;{void*t1=_T2D6;struct Cyc_AssnDef_AssnMap amin=_T2D5;
struct _tuple14 _T2D7=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*mem=_T2D9;struct Cyc_AssnDef_AssnMap amin=_T2D8;{struct _tuple14 _T2DA;_T1ED=mem;_T1EE=t1;_T1EF=e;_T1F0=_T1EF->topt;
_T2DA.f0=Cyc_AssnDef_select(_T1ED,_T1EE,_T1F0);_T2DA.f1=amin;_T1EC=_T2DA;}return _T1EC;}}}}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2CA=_T2D4->f2;_T2C9=_T2D4->f3;_T2C8=_T2D4->f4;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct _fat_ptr*f=_T2CA;int is_tagged=_T2C9;int is_read=_T2C8;_T1F1=env;_T1F2=e1;_T1F3=
# 1336
Cyc_Vcgen_zero_exp();_T1F4=amin;{struct _tuple22 _T2D4=Cyc_Vcgen_vcgen_deref(_T1F1,_T1F2,_T1F3,_T1F4);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f2;{void*t1=_T2D6;struct Cyc_AssnDef_AssnMap amin=_T2D5;_T1F5=e1;_T1F6=_T1F5->topt;_T1F7=
_check_null(_T1F6);_T1F8=Cyc_Tcutil_pointer_elt_type(_T1F7);_T1F9=f;{int i=Cyc_CfFlowInfo_get_field_index(_T1F8,_T1F9);
struct _tuple14 _T2D7=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*mem=_T2D9;struct Cyc_AssnDef_AssnMap amin=_T2D8;_T1FA=env;_T1FB=_T1FA->pure_exp;
if(_T1FB)goto _TL10B;else{goto _TL10D;}_TL10D: _T1FC=is_tagged;if(!_T1FC)goto _TL10B;_T1FD=is_read;if(!_T1FD)goto _TL10B;_T1FE=
Cyc_AssnDef_select(mem,t1,0);_T1FF=Cyc_AssnDef_tagof_tm(_T1FE);_T200=i;_T201=(unsigned)_T200;_T202=
Cyc_AssnDef_uint(_T201);{
# 1340
void*ck=Cyc_AssnDef_eq(_T1FF,_T202);_T203=env;_T204=amin;_T205=
# 1342
Cyc_AssnDef_not(ck);_T206=Cyc_AssnDef_and_assnmap_assn(_T204,_T205);_T207=Cyc_Absyn_exn_type();_T208=Cyc_AssnDef_fresh_var(0,_T207);Cyc_Vcgen_update_try_assnmap(_T203,_T206,_T208);
amin=Cyc_AssnDef_and_assnmap_assn(amin,ck);}goto _TL10C;_TL10B: _TL10C: _T209=e1;_T20A=_T209->topt;_T20B=
# 1345
_check_null(_T20A);{void*elt_type=Cyc_Tcutil_pointer_elt_type(_T20B);{struct _tuple14 _T2DA;_T20D=
Cyc_AssnDef_select(mem,t1,elt_type);_T20E=i;_T20F=(unsigned)_T20E;_T210=e;_T211=_T210->topt;_T2DA.f0=Cyc_AssnDef_proj(_T20D,_T20F,_T211);_T2DA.f1=amin;_T20C=_T2DA;}return _T20C;}}}}}}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2D0=_T2D4->f2;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_Absyn_Exp*e2=_T2D0;
# 1348
struct _tuple22 _T2D4=Cyc_Vcgen_vcgen_deref(env,e1,e2,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;void*_T2D7;_T2D7=_T2D4.f0;_T2D6=_T2D4.f1;_T2D5=_T2D4.f2;{void*t1=_T2D7;void*t2=_T2D6;struct Cyc_AssnDef_AssnMap amin=_T2D5;
struct _tuple14 _T2D8=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T2D9;void*_T2DA;_T2DA=_T2D8.f0;_T2D9=_T2D8.f1;{void*mem=_T2DA;struct Cyc_AssnDef_AssnMap amin=_T2D9;{struct _tuple14 _T2DB;_T213=mem;_T214=
Cyc_AssnDef_plus(t1,t2,0);_T215=e;_T216=_T215->topt;_T2DB.f0=Cyc_AssnDef_select(_T213,_T214,_T216);_T2DB.f1=amin;_T212=_T2DB;}return _T212;}}}case 30:{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2C7=_T2D4->f2;_T2C6=_T2D4->f3;}{struct Cyc_List_List*es=_T2D2;struct Cyc_Absyn_Datatypedecl*dd=_T2C7;struct Cyc_Absyn_Datatypefield*df=_T2C6;
# 1352
struct _tuple17 _T2D4=Cyc_Vcgen_vcgen_rexps(env,es,amin);struct Cyc_AssnDef_AssnMap _T2D5;struct Cyc_List_List*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{struct Cyc_List_List*ts=_T2D6;struct Cyc_AssnDef_AssnMap amin=_T2D5;
unsigned i=Cyc_Vcgen_datatype_constructor_index(dd,df);{struct _tuple14 _T2D7;_T218=i;_T219=ts;_T21A=e;_T21B=_T21A->topt;
_T2D7.f0=Cyc_AssnDef_datatype_aggr(_T218,_T219,_T21B);_T2D7.f1=amin;_T217=_T2D7;}return _T217;}}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f2;}{struct Cyc_List_List*dles=_T2D2;_T2D2=dles;goto _LL58;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;}_LL58: {struct Cyc_List_List*dles=_T2D2;_T2D2=dles;goto _LL5A;}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f2;}_LL5A: {struct Cyc_List_List*dles=_T2D2;
# 1360
_TL111: if(dles!=0)goto _TL10F;else{goto _TL110;}
_TL10F: _T21C=dles;_T21D=_T21C->hd;{struct _tuple24*_T2D4=(struct _tuple24*)_T21D;struct Cyc_Absyn_Exp*_T2D5;{struct _tuple24 _T2D6=*_T2D4;_T2D5=_T2D6.f1;}{struct Cyc_Absyn_Exp*e=_T2D5;_T21E=
Cyc_Vcgen_vcgen_rexp(env,e,amin);amin=_T21E.f1;}}_T21F=dles;
# 1360
dles=_T21F->tl;goto _TL111;_TL110:{struct _tuple14 _T2D4;
# 1364
_T2D4.f0=Cyc_AssnDef_fresh_var(0,0);_T2D4.f1=amin;_T220=_T2D4;}return _T220;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f3;}{struct Cyc_List_List*dles=_T2D2;_T2D2=dles;goto _LL5E;}case 29:{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f3;}_LL5E: {struct Cyc_List_List*dles=_T2D2;_T221=e;_T222=_T221->topt;{
# 1367
void*atype=_check_null(_T222);
struct _tuple20 _T2D4=Cyc_Vcgen_get_aggr_info(atype);enum Cyc_Absyn_AggrKind _T2D5;int _T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{int is_tagged=_T2D6;enum Cyc_Absyn_AggrKind ak=_T2D5;
struct Cyc_List_List*dlvs=0;
unsigned tag=0U;
_TL115: if(dles!=0)goto _TL113;else{goto _TL114;}
_TL113: _T223=dles;_T224=_T223->hd;{struct _tuple24*_T2D7=(struct _tuple24*)_T224;struct Cyc_Absyn_Exp*_T2D8;struct Cyc_List_List*_T2D9;{struct _tuple24 _T2DA=*_T2D7;_T2D9=_T2DA.f0;_T2D8=_T2DA.f1;}{struct Cyc_List_List*dl=_T2D9;struct Cyc_Absyn_Exp*e=_T2D8;
struct _tuple14 _T2DA=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*v=_T2DC;struct Cyc_AssnDef_AssnMap a=_T2DB;
amin=a;{struct _fat_ptr*_T2DD;if(dl==0)goto _TL116;_T225=dl;_T226=(struct Cyc_List_List*)_T225;_T227=_T226->hd;_T228=(int*)_T227;_T229=*_T228;if(_T229!=1)goto _TL118;_T22A=dl;_T22B=(struct Cyc_List_List*)_T22A;_T22C=_T22B->tl;if(_T22C!=0)goto _TL11A;_T22D=dl;{struct Cyc_List_List _T2DE=*_T22D;_T22E=_T2DE.hd;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T2DF=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T22E;_T2DD=_T2DF->f1;}}{struct _fat_ptr*f=_T2DD;
# 1377
int i=Cyc_CfFlowInfo_get_field_index(atype,f);_T22F=i;
tag=(unsigned)_T22F;{struct Cyc_List_List*_T2DE=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple21*_T2DF=_cycalloc(sizeof(struct _tuple21));
_T2DF->f0=i;_T2DF->f1=v;_T231=(struct _tuple21*)_T2DF;}_T2DE->hd=_T231;_T2DE->tl=dlvs;_T230=(struct Cyc_List_List*)_T2DE;}dlvs=_T230;goto _LL132;}_TL11A: goto _LL135;_TL118: goto _LL135;_TL116: _LL135: _T233=Cyc_Warn_impos;{
# 1381
int(*_T2DE)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T233;_T232=_T2DE;}_T234=_tag_fat("no field name in designator!",sizeof(char),29U);_T235=_tag_fat(0U,sizeof(void*),0);_T232(_T234,_T235);_LL132:;}}}}_T236=dles;
# 1371
dles=_T236->tl;goto _TL115;_TL114: _T238=Cyc_List_rimp_merge_sort;{
# 1384
struct Cyc_List_List*(*_T2D7)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*))_T238;_T237=_T2D7;}_T239=dlvs;dlvs=_T237(Cyc_Vcgen_cmp_index,_T239);_T23B=Cyc_List_map;{
struct Cyc_List_List*(*_T2D7)(void*(*)(struct _tuple21*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct _tuple21*),struct Cyc_List_List*))_T23B;_T23A=_T2D7;}_T23D=Cyc_Core_snd;{void*(*_T2D7)(struct _tuple21*)=(void*(*)(struct _tuple21*))_T23D;_T23C=_T2D7;}_T23E=dlvs;{struct Cyc_List_List*vs=_T23A(_T23C,_T23E);_T240=ak;_T241=(int)_T240;
if(_T241!=1)goto _TL11C;_T242=tag;_T243=vs;_T244=e;_T245=_T244->topt;_T23F=Cyc_AssnDef_union_aggr(_T242,_T243,_T245);goto _TL11D;_TL11C: _T246=vs;_T247=e;_T248=_T247->topt;_T23F=
Cyc_AssnDef_struct_aggr(_T246,_T248);_TL11D: {
# 1386
void*res=_T23F;{struct _tuple14 _T2D7;
# 1388
_T2D7.f0=res;_T2D7.f1=amin;_T249=_T2D7;}return _T249;}}}}}case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2D0=_T2D4->f2;_T2CD=_T2D4->f3;}{struct Cyc_Absyn_Vardecl*vd=_T2D2;struct Cyc_Absyn_Exp*e1=_T2D0;struct Cyc_Absyn_Exp*e2=_T2CD;_T24A=env;_T24B=vd;_T24C=e1;_T24D=e2;_T24E=e;_T24F=_T24E->topt;_T250=
# 1390
_check_null(_T24F);_T251=amin;_T252=Cyc_Vcgen_vcgen_comprehension(_T24A,_T24B,_T24C,_T24D,_T250,_T251);return _T252;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;}{struct Cyc_Absyn_Exp*e=_T2D2;{struct _tuple14 _T2D4;
# 1392
_T2D4.f0=Cyc_AssnDef_fresh_var(0,0);_T254=Cyc_Vcgen_vcgen_rexp(env,e,amin);_T2D4.f1=_T254.f1;_T253=_T2D4;}return _T253;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T2C4;_T2C5=_T2D4->f1;}{struct Cyc_Absyn_MallocInfo mi=_T2C5;_T255=mi;_T256=_T255.rgn;
# 1394
if(_T256==0)goto _TL11E;_T257=env;_T258=mi;_T259=_T258.rgn;_T25A=amin;_T25B=
Cyc_Vcgen_vcgen_rexp(_T257,_T259,_T25A);amin=_T25B.f1;goto _TL11F;_TL11E: _TL11F: _T25C=env;_T25D=mi;_T25E=_T25D.num_elts;_T25F=amin;{
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_rexp(_T25C,_T25E,_T25F);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*t=_T2D6;struct Cyc_AssnDef_AssnMap am1=_T2D5;_T260=env;_T261=e;_T262=t;_T263=e;_T264=_T263->topt;{
void*res=Cyc_Vcgen_myalloc(_T260,_T261,_T262,_T264);_T265=am1;_T266=res;_T267=
Cyc_AssnDef_zero();_T268=Cyc_AssnDef_neq(_T266,_T267);_T269=Cyc_AssnDef_numelts_term(res);_T26A=t;_T26B=Cyc_AssnDef_eq(_T269,_T26A);_T26C=Cyc_AssnDef_and(_T268,_T26B);{struct Cyc_AssnDef_AssnMap am2=Cyc_AssnDef_and_assnmap_assn(_T265,_T26C);{struct _tuple14 _T2D7;
_T2D7.f0=res;_T2D7.f1=am2;_T26D=_T2D7;}return _T26D;}}}}}case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;_T2D0=_T2D4->f2;}{struct Cyc_Absyn_Exp*e1=_T2D2;struct Cyc_Absyn_Exp*e2=_T2D0;
# 1402
struct _tuple14 _T2D4=Cyc_Vcgen_vcgen_lexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;void*_T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{void*lt1=_T2D6;struct Cyc_AssnDef_AssnMap am1=_T2D5;
struct _tuple14 _T2D7=Cyc_Vcgen_vcgen_lexp(env,e2,am1);struct Cyc_AssnDef_AssnMap _T2D8;void*_T2D9;_T2D9=_T2D7.f0;_T2D8=_T2D7.f1;{void*lt2=_T2D9;struct Cyc_AssnDef_AssnMap am2=_T2D8;
struct _tuple14 _T2DA=Cyc_Vcgen_deref_lterm(env,lt1,am2);struct Cyc_AssnDef_AssnMap _T2DB;void*_T2DC;_T2DC=_T2DA.f0;_T2DB=_T2DA.f1;{void*t1=_T2DC;struct Cyc_AssnDef_AssnMap am3=_T2DB;
struct _tuple14 _T2DD=Cyc_Vcgen_deref_lterm(env,lt2,am3);struct Cyc_AssnDef_AssnMap _T2DE;void*_T2DF;_T2DF=_T2DD.f0;_T2DE=_T2DD.f1;{void*t2=_T2DF;struct Cyc_AssnDef_AssnMap am4=_T2DE;
struct Cyc_AssnDef_AssnMap am5=Cyc_Vcgen_do_assign(env,am4,lt1,t2);
struct Cyc_AssnDef_AssnMap am6=Cyc_Vcgen_do_assign(env,am5,lt2,t1);{struct _tuple14 _T2E0;
_T2E0.f0=Cyc_AssnDef_fresh_var(0,0);_T2E0.f1=am6;_T26E=_T2E0;}return _T26E;}}}}}case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_T2C4;_T2D2=_T2D4->f1;}{struct Cyc_Absyn_Stmt*s=_T2D2;_T26F=env;{
# 1410
void**old_exp_stmt=_T26F->exp_stmt;_T270=env;{void**_T2D4=_cycalloc(sizeof(void*));
*_T2D4=Cyc_AssnDef_fresh_var(0,0);_T271=(void**)_T2D4;}_T270->exp_stmt=_T271;{
struct Cyc_AssnDef_AssnMap amout=Cyc_Vcgen_vcgen_stmt(env,s,amin);_T272=env;_T273=_T272->exp_stmt;_T274=
_check_null(_T273);{void*t=*_T274;_T275=env;
_T275->exp_stmt=old_exp_stmt;{struct _tuple14 _T2D4;
_T2D4.f0=t;_T2D4.f1=amout;_T276=_T2D4;}return _T276;}}}}case 39:{struct _tuple14 _T2D4;
_T2D4.f0=Cyc_AssnDef_fresh_var(0,0);_T2D4.f1=amin;_T277=_T2D4;}return _T277;case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_T2C4;_T2D0=_T2D4->f1;}{struct Cyc_Absyn_Exp*e=_T2D0;_T278=
Cyc_Vcgen_vcgen_rexp(env,e,amin);return _T278;}case 41:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T2C4;_T2D0=_T2D4->f1;_T2C9=_T2D4->f2;}_T279=(int)_T2C9;if(!_T279)goto _TL120;{struct Cyc_Absyn_Exp*e1=_T2D0;int static_only=_T2C9;_T27A=env;{
# 1419
int old_widen_paths=_T27A->widen_paths;_T27B=env;{
int old_pure_exp=_T27B->pure_exp;_T27C=env;
_T27C->widen_paths=0;_T27D=env;
_T27D->pure_exp=1;_T27E=env;_T27F=e1;{struct Cyc_AssnDef_AssnMap _T2D4;_T281=& Cyc_AssnDef_true_assn;_T282=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T281;
_T2D4.assn=(void*)_T282;_T283=amin;_T2D4.map=_T283.map;_T280=_T2D4;}{struct _tuple18 _T2D4=Cyc_Vcgen_vcgen_test(_T27E,_T27F,_T280);struct Cyc_AssnDef_AssnMap _T2D5;_T2D5=_T2D4.f0;{struct Cyc_AssnDef_AssnMap amt=_T2D5;_T284=env;
_T284->widen_paths=old_widen_paths;_T285=env;
_T285->pure_exp=old_pure_exp;_T286=amin;_T287=_T286.assn;_T288=amt;_T289=_T288.assn;_T28A=
Cyc_Vcgen_implies(_T287,_T289);if(_T28A)goto _TL122;else{goto _TL124;}
_TL124:{struct Cyc_String_pa_PrintArg_struct _T2D6;_T2D6.tag=0;_T28C=amt;_T28D=_T28C.assn;
_T2D6.f1=Cyc_AssnDef_assn2string(_T28D);_T28B=_T2D6;}{struct Cyc_String_pa_PrintArg_struct _T2D6=_T28B;{struct Cyc_String_pa_PrintArg_struct _T2D7;_T2D7.tag=0;_T2D7.f1=Cyc_AssnDef_assnmap2string(amin);_T28E=_T2D7;}{struct Cyc_String_pa_PrintArg_struct _T2D7=_T28E;void*_T2D8[2];_T2D8[0]=& _T2D6;_T2D8[1]=& _T2D7;_T28F=e;_T290=_T28F->loc;_T291=
# 1427
_tag_fat("cannot prove @assert(%s)\nfrom\n %s",sizeof(char),34U);_T292=_tag_fat(_T2D8,sizeof(void*),2);Cyc_Warn_err(_T290,_T291,_T292);}}goto _TL123;_TL122: _TL123:{struct _tuple14 _T2D6;
# 1429
_T2D6.f0=Cyc_AssnDef_one();_T294=amin;_T295=amt;_T296=_T295.assn;_T2D6.f1=Cyc_AssnDef_and_assnmap_assn(_T294,_T296);_T293=_T2D6;}return _T293;}}}}}_TL120:{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T2C4;_T2D0=_T2D4->f1;_T297=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_T2C4;_T298=& _T297->f3;_T2D2=(int*)_T298;}{struct Cyc_Absyn_Exp*e1=_T2D0;int*do_check=(int*)_T2D2;
# 1433
struct _tuple18 _T2D4=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T2D5;struct Cyc_AssnDef_AssnMap _T2D6;_T2D6=_T2D4.f0;_T2D5=_T2D4.f1;{struct Cyc_AssnDef_AssnMap amt=_T2D6;struct Cyc_AssnDef_AssnMap amf=_T2D5;_T299=amf;_T29A=_T299.assn;_T29B=& Cyc_AssnDef_false_assn;_T29C=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T29B;_T29D=(void*)_T29C;_T29E=
# 1435
Cyc_Vcgen_implies(_T29A,_T29D);if(!_T29E)goto _TL125;_T29F=
Cyc_Vcgen_is_pure_exp(e1);if(!_T29F)goto _TL127;_T2A0=do_check;
*_T2A0=0;goto _TL128;
# 1439
_TL127: _T2A1=do_check;*_T2A1=1;_TL128: goto _TL126;
# 1441
_TL125: _T2A2=do_check;*_T2A2=1;_T2A3=Cyc_Flags_warn_assert;
if(!_T2A3)goto _TL129;{struct Cyc_Warn_String_Warn_Warg_struct _T2D7;_T2D7.tag=0;
_T2D7.f1=_tag_fat("assert(",sizeof(char),8U);_T2A4=_T2D7;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D7=_T2A4;{struct Cyc_Warn_Exp_Warn_Warg_struct _T2D8;_T2D8.tag=4;_T2D8.f1=e1;_T2A5=_T2D8;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T2D8=_T2A5;{struct Cyc_Warn_String_Warn_Warg_struct _T2D9;_T2D9.tag=0;_T2D9.f1=_tag_fat(") might fail at run-time",sizeof(char),25U);_T2A6=_T2D9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D9=_T2A6;void*_T2DA[3];_T2DA[0]=& _T2D7;_T2DA[1]=& _T2D8;_T2DA[2]=& _T2D9;_T2A7=e;_T2A8=_T2A7->loc;_T2A9=_tag_fat(_T2DA,sizeof(void*),3);Cyc_Warn_warn2(_T2A8,_T2A9);}}}goto _TL12A;_TL129: _TL12A: _T2AA=
Cyc_Absyn_exn_type();{void*v=Cyc_AssnDef_fresh_var(0,_T2AA);
Cyc_Vcgen_update_try_assnmap(env,amf,v);}_TL126:{struct _tuple14 _T2D7;
# 1447
_T2D7.f0=Cyc_AssnDef_one();_T2D7.f1=amt;_T2AB=_T2D7;}return _T2AB;}}default:{struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*_T2D4=(struct Cyc_Absyn_Assert_false_e_Absyn_Raw_exp_struct*)_T2C4;_T2D0=_T2D4->f1;}{struct Cyc_Absyn_Exp*e1=_T2D0;_T2AC=env;{
# 1449
int old_widen_paths=_T2AC->widen_paths;_T2AD=env;{
int old_pure_exp=_T2AD->pure_exp;_T2AE=env;
_T2AE->widen_paths=0;_T2AF=env;
_T2AF->pure_exp=1;_T2B0=env;_T2B1=e1;{struct Cyc_AssnDef_AssnMap _T2D4;_T2B3=& Cyc_AssnDef_true_assn;_T2B4=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T2B3;
_T2D4.assn=(void*)_T2B4;_T2B5=amin;_T2D4.map=_T2B5.map;_T2B2=_T2D4;}{struct _tuple18 _T2D4=Cyc_Vcgen_vcgen_test(_T2B0,_T2B1,_T2B2);struct Cyc_AssnDef_AssnMap _T2D5;_T2D5=_T2D4.f0;{struct Cyc_AssnDef_AssnMap amt=_T2D5;_T2B6=env;
_T2B6->widen_paths=old_widen_paths;_T2B7=env;
_T2B7->pure_exp=old_pure_exp;_T2B8=amin;_T2B9=_T2B8.assn;_T2BA=amt;_T2BB=_T2BA.assn;_T2BC=
Cyc_Vcgen_implies(_T2B9,_T2BB);if(!_T2BC)goto _TL12B;{struct Cyc_String_pa_PrintArg_struct _T2D6;_T2D6.tag=0;
# 1458
_T2D6.f1=Cyc_AssnDef_assnmap2string(amt);_T2BD=_T2D6;}{struct Cyc_String_pa_PrintArg_struct _T2D6=_T2BD;{struct Cyc_String_pa_PrintArg_struct _T2D7;_T2D7.tag=0;_T2D7.f1=Cyc_AssnDef_assnmap2string(amin);_T2BE=_T2D7;}{struct Cyc_String_pa_PrintArg_struct _T2D7=_T2BE;void*_T2D8[2];_T2D8[0]=& _T2D6;_T2D8[1]=& _T2D7;_T2BF=e;_T2C0=_T2BF->loc;_T2C1=
# 1457
_tag_fat("proved @assert_false(%s)\nfrom\n %s",sizeof(char),34U);_T2C2=_tag_fat(_T2D8,sizeof(void*),2);Cyc_Warn_err(_T2C0,_T2C1,_T2C2);}}goto _TL12C;_TL12B: _TL12C:{struct _tuple14 _T2D6;
# 1459
_T2D6.f0=Cyc_AssnDef_one();_T2D6.f1=amin;_T2C3=_T2D6;}return _T2C3;}}}}}};}}
# 1463
static struct _tuple17 Cyc_Vcgen_vcgen_rexps(struct Cyc_Vcgen_Env*env,struct Cyc_List_List*es,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Vcgen_Env*_T0;struct Cyc_List_List*_T1;void*_T2;struct Cyc_Absyn_Exp*_T3;struct Cyc_AssnDef_AssnMap _T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct _tuple17 _T7;
struct Cyc_List_List*result=0;
_TL130: if(es!=0)goto _TL12E;else{goto _TL12F;}
_TL12E: _T0=env;_T1=es;_T2=_T1->hd;_T3=(struct Cyc_Absyn_Exp*)_T2;_T4=amin;{struct _tuple14 _T8=Cyc_Vcgen_vcgen_rexp(_T0,_T3,_T4);struct Cyc_AssnDef_AssnMap _T9;void*_TA;_TA=_T8.f0;_T9=_T8.f1;{void*t=_TA;struct Cyc_AssnDef_AssnMap am=_T9;{struct Cyc_List_List*_TB=_cycalloc(sizeof(struct Cyc_List_List));
_TB->hd=t;_TB->tl=result;_T5=(struct Cyc_List_List*)_TB;}result=_T5;
amin=am;}}_T6=es;
# 1465
es=_T6->tl;goto _TL130;_TL12F:{struct _tuple17 _T8;
# 1470
_T8.f0=Cyc_List_imp_rev(result);_T8.f1=amin;_T7=_T8;}return _T7;}
# 1476
static struct _tuple14 Cyc_Vcgen_vcgen_lexp(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;unsigned _T6;void*_T7;void*_T8;struct Cyc_Absyn_Vardecl*_T9;void*_TA;int*_TB;int _TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;void*_TF;void*_T10;struct _tuple14 _T11;void*_T12;void*_T13;void*_T14;struct _tuple14 _T15;void*_T16;void*_T17;struct _tuple14 _T18;struct Cyc_Vcgen_Env*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Exp*_T1B;struct Cyc_AssnDef_AssnMap _T1C;struct _tuple14 _T1D;struct _tuple14 _T1E;void*_T1F;void*_T20;struct Cyc_Absyn_Exp*_T21;void*_T22;struct Cyc_Vcgen_Env*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_Absyn_Exp*_T25;struct Cyc_AssnDef_AssnMap _T26;struct Cyc_Absyn_Exp*_T27;void*_T28;void*_T29;void*_T2A;struct _fat_ptr*_T2B;struct Cyc_Absyn_Exp*_T2C;void*_T2D;void*_T2E;void*_T2F;void*_T30;struct Cyc_Absyn_Tqual _T31;void*_T32;void*_T33;struct _tuple14 _T34;void*_T35;int _T36;unsigned _T37;void*_T38;struct Cyc_Absyn_Exp*_T39;void*_T3A;void*_T3B;struct _fat_ptr*_T3C;struct Cyc_Absyn_Exp*_T3D;void*_T3E;void*_T3F;void*_T40;void*_T41;struct Cyc_Absyn_Tqual _T42;void*_T43;void*_T44;struct _tuple14 _T45;void*_T46;int _T47;unsigned _T48;void*_T49;struct _tuple14 _T4A;struct Cyc_Absyn_Exp*_T4B;void*_T4C;void*_T4D;void*_T4E;void*_T4F;struct Cyc_Absyn_Tqual _T50;void*_T51;void*_T52;struct _tuple14 _T53;_T0=e;{
void*_T54=_T0->r;struct _fat_ptr*_T55;struct Cyc_Absyn_Exp*_T56;struct Cyc_Absyn_Exp*_T57;struct Cyc_Absyn_Vardecl*_T58;_T1=(int*)_T54;_T2=*_T1;switch(_T2){case 1: _T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T7=_T59->f1;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T5A=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T7;_T58=_T5A->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T58;_T58=vd;goto _LL4;}case 4:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T8=_T59->f1;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T5A=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T8;_T58=_T5A->f1;}}_LL4: {struct Cyc_Absyn_Vardecl*vd=_T58;_T9=vd;_TA=_T9->type;{
# 1481
void*_T59=Cyc_Absyn_compress(_TA);struct Cyc_Absyn_ArrayInfo _T5A;_TB=(int*)_T59;_TC=*_TB;if(_TC!=5)goto _TL133;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T5B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T59;_T5A=_T5B->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T5A;_TD=vd;_TE=_TD->type;_TF=Cyc_Absyn_heap_rgn_type;_T10=Cyc_Absyn_al_qual_type;{
# 1484
void*p=Cyc_Tcutil_promote_array(_TE,_TF,_T10,0);{struct _tuple14 _T5B;_T12=
# 1486
Cyc_Vcgen_myaddr(env,vd);_T13=Cyc_AssnDef_zero();_T14=p;_T5B.f0=Cyc_AssnDef_offseti(_T12,_T13,_T14);_T5B.f1=amin;_T11=_T5B;}return _T11;}}_TL133:{struct _tuple14 _T5B;
_T5B.f0=Cyc_Vcgen_myaddr(env,vd);_T5B.f1=amin;_T15=_T5B;}return _T15;;}}case 3:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T16=_T59->f1;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T5A=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T16;_T58=_T5A->f1;}}{struct Cyc_Absyn_Vardecl*vd=_T58;_T58=vd;goto _LL8;}case 5:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T54;_T17=_T59->f1;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T5A=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_T17;_T58=_T5A->f1;}}_LL8: {struct Cyc_Absyn_Vardecl*vd=_T58;{struct _tuple14 _T59;
# 1493
_T59.f0=Cyc_Vcgen_myaddr(env,vd);_T59.f1=amin;_T18=_T59;}return _T18;}default: goto _LL15;};case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;}{struct Cyc_Absyn_Exp*e1=_T57;_T19=env;_T1A=e1;_T1B=
# 1498
Cyc_Vcgen_zero_exp();_T1C=amin;{struct _tuple22 _T59=Cyc_Vcgen_vcgen_deref(_T19,_T1A,_T1B,_T1C);struct Cyc_AssnDef_AssnMap _T5A;void*_T5B;void*_T5C;_T5C=_T59.f0;_T5B=_T59.f1;_T5A=_T59.f2;{void*t1=_T5C;void*t2=_T5B;struct Cyc_AssnDef_AssnMap am1=_T5A;{struct _tuple14 _T5D;
_T5D.f0=t1;_T5D.f1=am1;_T1D=_T5D;}return _T1D;}}}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;_T56=_T59->f2;}{struct Cyc_Absyn_Exp*e1=_T57;struct Cyc_Absyn_Exp*e2=_T56;
# 1503
struct _tuple22 _T59=Cyc_Vcgen_vcgen_deref(env,e1,e2,amin);struct Cyc_AssnDef_AssnMap _T5A;void*_T5B;void*_T5C;_T5C=_T59.f0;_T5B=_T59.f1;_T5A=_T59.f2;{void*t1=_T5C;void*t2=_T5B;struct Cyc_AssnDef_AssnMap am1=_T5A;{struct _tuple14 _T5D;_T1F=t1;_T20=t2;_T21=e1;_T22=_T21->topt;
_T5D.f0=Cyc_AssnDef_offseti(_T1F,_T20,_T22);_T5D.f1=am1;_T1E=_T5D;}return _T1E;}}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;_T55=_T59->f2;}{struct Cyc_Absyn_Exp*e1=_T57;struct _fat_ptr*f=_T55;_T23=env;_T24=e1;_T25=
# 1507
Cyc_Vcgen_zero_exp();_T26=amin;{struct _tuple22 _T59=Cyc_Vcgen_vcgen_deref(_T23,_T24,_T25,_T26);struct Cyc_AssnDef_AssnMap _T5A;void*_T5B;void*_T5C;_T5C=_T59.f0;_T5B=_T59.f1;_T5A=_T59.f2;{void*t1=_T5C;void*t2=_T5B;struct Cyc_AssnDef_AssnMap am1=_T5A;_T27=e1;_T28=_T27->topt;_T29=
_check_null(_T28);_T2A=Cyc_Tcutil_pointer_elt_type(_T29);_T2B=f;{int i=Cyc_CfFlowInfo_get_field_index(_T2A,_T2B);_T2C=e;_T2D=_T2C->topt;_T2E=
# 1510
_check_null(_T2D);_T2F=Cyc_Absyn_heap_rgn_type;_T30=Cyc_Absyn_al_qual_type;_T31=Cyc_Absyn_empty_tqual(0U);_T32=Cyc_Absyn_false_type;_T33=Cyc_Absyn_false_type;{void*tp=Cyc_Absyn_at_type(_T2E,_T2F,_T30,_T31,_T32,_T33);{struct _tuple14 _T5D;_T35=t1;_T36=i;_T37=(unsigned)_T36;_T38=tp;
_T5D.f0=Cyc_AssnDef_offsetf(_T35,_T37,_T38);_T5D.f1=am1;_T34=_T5D;}return _T34;}}}}}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;_T55=_T59->f2;}{struct Cyc_Absyn_Exp*e1=_T57;struct _fat_ptr*f=_T55;
# 1513
struct _tuple14 _T59=Cyc_Vcgen_vcgen_lexp(env,e1,amin);struct Cyc_AssnDef_AssnMap _T5A;void*_T5B;_T5B=_T59.f0;_T5A=_T59.f1;{void*t1=_T5B;struct Cyc_AssnDef_AssnMap am1=_T5A;_T39=e1;_T3A=_T39->topt;_T3B=
_check_null(_T3A);_T3C=f;{int i=Cyc_CfFlowInfo_get_field_index(_T3B,_T3C);_T3D=e;_T3E=_T3D->topt;_T3F=
# 1516
_check_null(_T3E);_T40=Cyc_Absyn_heap_rgn_type;_T41=Cyc_Absyn_al_qual_type;_T42=Cyc_Absyn_empty_tqual(0U);_T43=Cyc_Absyn_false_type;_T44=Cyc_Absyn_false_type;{void*tp=Cyc_Absyn_at_type(_T3F,_T40,_T41,_T42,_T43,_T44);{struct _tuple14 _T5C;_T46=t1;_T47=i;_T48=(unsigned)_T47;_T49=tp;
_T5C.f0=Cyc_AssnDef_offsetf(_T46,_T48,_T49);_T5C.f1=am1;_T45=_T5C;}return _T45;}}}}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;}{struct Cyc_Absyn_Exp*e1=_T57;_T57=e1;goto _LL14;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T54;_T57=_T59->f1;}_LL14: {struct Cyc_Absyn_Exp*e1=_T57;_T4A=
# 1520
Cyc_Vcgen_vcgen_lexp(env,e1,amin);return _T4A;}default: _LL15: _T4B=e;_T4C=_T4B->topt;_T4D=
# 1523
_check_null(_T4C);_T4E=Cyc_Absyn_heap_rgn_type;_T4F=Cyc_Absyn_al_qual_type;_T50=Cyc_Absyn_empty_tqual(0U);_T51=Cyc_Absyn_false_type;_T52=Cyc_Absyn_false_type;{void*tp=Cyc_Absyn_at_type(_T4D,_T4E,_T4F,_T50,_T51,_T52);{struct _tuple14 _T59;
_T59.f0=Cyc_AssnDef_fresh_var(0,tp);_T59.f1=amin;_T53=_T59;}return _T53;}};}}
# 1530
static int Cyc_Vcgen_unsigned_comparison(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;void*_T1;void*_T2;struct Cyc_Absyn_Exp*_T3;void*_T4;void*_T5;struct _tuple0 _T6;void*_T7;int*_T8;int _T9;void*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;void*_TC;int*_TD;int _TE;void*_TF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T10;void*_T11;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T12;enum Cyc_Absyn_Sign _T13;void*_T14;int*_T15;int _T16;void*_T17;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18;void*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1D;void*_T1E;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T1F;enum Cyc_Absyn_Sign _T20;void*_T21;int*_T22;int _T23;void*_T24;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T25;void*_T26;int*_T27;int _T28;void*_T29;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2A;void*_T2B;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T2C;enum Cyc_Absyn_Sign _T2D;void*_T2E;int*_T2F;int _T30;void*_T31;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T32;void*_T33;int*_T34;int _T35;void*_T36;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T37;void*_T38;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T39;enum Cyc_Absyn_Sign _T3A;void*_T3B;int*_T3C;int _T3D;void*_T3E;int*_T3F;int _T40;void*_T41;int*_T42;int _T43;void*_T44;int*_T45;int _T46;_T0=e1;_T1=_T0->topt;_T2=
_check_null(_T1);{void*t1=Cyc_Absyn_compress(_T2);_T3=e2;_T4=_T3->topt;_T5=
_check_null(_T4);{void*t2=Cyc_Absyn_compress(_T5);{struct _tuple0 _T47;
_T47.f0=t1;_T47.f1=t2;_T6=_T47;}{struct _tuple0 _T47=_T6;_T7=_T47.f0;_T8=(int*)_T7;_T9=*_T8;if(_T9!=0)goto _TL135;_TA=_T47.f0;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TA;_TC=_TB->f1;_TD=(int*)_TC;_TE=*_TD;if(_TE!=1)goto _TL137;_TF=_T47.f0;_T10=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TF;_T11=_T10->f1;_T12=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T11;_T13=_T12->f1;if(_T13!=Cyc_Absyn_Unsigned)goto _TL139;
return 1;_TL139: _T14=_T47.f1;_T15=(int*)_T14;_T16=*_T15;if(_T16!=0)goto _TL13B;_T17=_T47.f1;_T18=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T17;_T19=_T18->f1;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=1)goto _TL13D;_T1C=_T47.f1;_T1D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1C;_T1E=_T1D->f1;_T1F=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T1E;_T20=_T1F->f1;if(_T20!=Cyc_Absyn_Unsigned)goto _TL13F;goto _LL3;_TL13F: goto _LL7;_TL13D: goto _LL7;_TL13B: goto _LL7;_TL137: _T21=_T47.f1;_T22=(int*)_T21;_T23=*_T22;if(_T23!=0)goto _TL141;_T24=_T47.f1;_T25=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T24;_T26=_T25->f1;_T27=(int*)_T26;_T28=*_T27;if(_T28!=1)goto _TL143;_T29=_T47.f1;_T2A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T29;_T2B=_T2A->f1;_T2C=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T2B;_T2D=_T2C->f1;if(_T2D!=Cyc_Absyn_Unsigned)goto _TL145;goto _LL3;_TL145: goto _LL7;_TL143: goto _LL7;_TL141: goto _LL7;_TL135: _T2E=_T47.f1;_T2F=(int*)_T2E;_T30=*_T2F;if(_T30!=0)goto _TL147;_T31=_T47.f1;_T32=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T31;_T33=_T32->f1;_T34=(int*)_T33;_T35=*_T34;if(_T35!=1)goto _TL149;_T36=_T47.f1;_T37=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T36;_T38=_T37->f1;_T39=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T38;_T3A=_T39->f1;if(_T3A!=Cyc_Absyn_Unsigned)goto _TL14B;_LL3:
 return 1;_TL14B: _T3B=_T47.f0;_T3C=(int*)_T3B;_T3D=*_T3C;if(_T3D!=4)goto _TL14D;goto _LL7;_TL14D: goto _LL7;_TL149: _T3E=_T47.f0;_T3F=(int*)_T3E;_T40=*_T3F;if(_T40!=4)goto _TL14F;goto _LL7;_TL14F: goto _LL7;_TL147: _T41=_T47.f0;_T42=(int*)_T41;_T43=*_T42;if(_T43!=4)goto _TL151;_T44=_T47.f1;_T45=(int*)_T44;_T46=*_T45;if(_T46!=4)goto _TL153;
return 1;_TL153: goto _LL7;_TL151: _LL7:
 return 0;;}}}}
# 1548 "vcgen.cyc"
static struct _tuple18 Cyc_Vcgen_vcgen_test(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Exp*e,struct Cyc_AssnDef_AssnMap ain){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct _tuple18 _T3;struct Cyc_Vcgen_Env*_T4;struct Cyc_AssnDef_AssnMap _T5;struct Cyc_Vcgen_Env*_T6;struct Cyc_AssnDef_AssnMap _T7;struct _tuple18 _T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_AssnDef_AssnMap _TA;struct _tuple18 _TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_AssnDef_AssnMap _TD;struct _tuple18 _TE;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TF;enum Cyc_Absyn_Primop _T10;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;void*_T14;struct _tuple18 _T15;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T16;struct Cyc_List_List*_T17;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_List_List*_T24;void*_T25;enum Cyc_Absyn_Primop _T26;int _T27;struct Cyc_AssnDef_AssnMap _T28;void*_T29;struct Cyc_AssnDef_AssnMap _T2A;void*_T2B;struct Cyc_AssnDef_AssnMap _T2C;void*_T2D;struct Cyc_AssnDef_AssnMap _T2E;void*_T2F;struct Cyc_AssnDef_AssnMap _T30;void*_T31;struct Cyc_AssnDef_AssnMap _T32;void*_T33;struct Cyc_AssnDef_AssnMap _T34;void*_T35;struct Cyc_AssnDef_AssnMap _T36;void*_T37;struct Cyc_AssnDef_AssnMap _T38;void*_T39;struct Cyc_AssnDef_AssnMap _T3A;void*_T3B;struct Cyc_AssnDef_AssnMap _T3C;void*_T3D;struct Cyc_AssnDef_AssnMap _T3E;void*_T3F;struct Cyc_AssnDef_AssnMap _T40;void*_T41;struct Cyc_AssnDef_AssnMap _T42;void*_T43;struct Cyc_AssnDef_AssnMap _T44;void*_T45;struct Cyc_AssnDef_AssnMap _T46;void*_T47;struct Cyc_AssnDef_AssnMap _T48;void*_T49;struct Cyc_AssnDef_AssnMap _T4A;void*_T4B;struct Cyc_AssnDef_AssnMap _T4C;void*_T4D;struct Cyc_AssnDef_AssnMap _T4E;void*_T4F;struct Cyc_Vcgen_Env*_T50;struct Cyc_Absyn_Exp*_T51;void*_T52;enum Cyc_Absyn_Primop _T53;void*_T54;void*_T55;struct Cyc_AssnDef_AssnMap _T56;struct Cyc_AssnDef_AssnMap _T57;void*_T58;void*_T59;void*_T5A;struct Cyc_AssnDef_AssnMap _T5B;void*_T5C;void*_T5D;void*_T5E;struct _tuple18 _T5F;struct Cyc_Absyn_Exp*_T60;void*_T61;void*_T62;struct _fat_ptr*_T63;int _T64;unsigned _T65;struct Cyc_AssnDef_AssnMap _T66;void*_T67;struct Cyc_AssnDef_AssnMap _T68;void*_T69;struct _tuple18 _T6A;void*_T6B;struct _tuple0 _T6C;struct Cyc_Absyn_Exp*_T6D;void*_T6E;void*_T6F;void*_T70;int*_T71;int _T72;void*_T73;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T74;void*_T75;int*_T76;int _T77;void*_T78;int*_T79;int _T7A;void*_T7B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7C;void*_T7D;int*_T7E;int _T7F;void*_T80;void*_T81;void*_T82;void*_T83;enum Cyc_Absyn_Size_of _T84;enum Cyc_Absyn_Size_of _T85;int _T86;enum Cyc_Absyn_Size_of _T87;enum Cyc_Absyn_Size_of _T88;int _T89;enum Cyc_Absyn_Size_of _T8A;int _T8B;enum Cyc_Absyn_Size_of _T8C;int _T8D;struct _tuple18 _T8E;void*_T8F;int*_T90;int _T91;void*_T92;struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_T93;unsigned _T94;struct _tuple18 _T95;struct Cyc_AssnDef_AssnMap _T96;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T97;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T98;void*_T99;struct _tuple18 _T9A;struct Cyc_AssnDef_AssnMap _T9B;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T9C;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T9D;void*_T9E;struct _tuple18 _T9F;struct Cyc_AssnDef_AssnMap _TA0;void*_TA1;void*_TA2;void*_TA3;struct Cyc_AssnDef_AssnMap _TA4;void*_TA5;void*_TA6;void*_TA7;_T0=e;{
void*_TA8=_T0->r;struct _fat_ptr*_TA9;enum Cyc_Absyn_Primop _TAA;struct Cyc_Absyn_Exp*_TAB;struct Cyc_Absyn_Exp*_TAC;void*_TAD;_T1=(int*)_TA8;_T2=*_T1;switch(_T2){case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TAC=_TAE->f2;_TAB=_TAE->f3;}{struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;struct Cyc_Absyn_Exp*e3=_TAB;
# 1551
struct _tuple18 _TAE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;struct Cyc_AssnDef_AssnMap _TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap a1t=_TB0;struct Cyc_AssnDef_AssnMap a1f=_TAF;
struct _tuple18 _TB1=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct Cyc_AssnDef_AssnMap _TB2;struct Cyc_AssnDef_AssnMap _TB3;_TB3=_TB1.f0;_TB2=_TB1.f1;{struct Cyc_AssnDef_AssnMap a2t=_TB3;struct Cyc_AssnDef_AssnMap a2f=_TB2;
struct _tuple18 _TB4=Cyc_Vcgen_vcgen_test(env,e3,a1f);struct Cyc_AssnDef_AssnMap _TB5;struct Cyc_AssnDef_AssnMap _TB6;_TB6=_TB4.f0;_TB5=_TB4.f1;{struct Cyc_AssnDef_AssnMap a3t=_TB6;struct Cyc_AssnDef_AssnMap a3f=_TB5;{struct _tuple18 _TB7;_T4=env;_T5=
Cyc_AssnDef_or_assnmap_assnmap(a2t,a3t);_TB7.f0=Cyc_Vcgen_may_widen(_T4,_T5);_T6=env;_T7=
Cyc_AssnDef_or_assnmap_assnmap(a2f,a3f);_TB7.f1=Cyc_Vcgen_may_widen(_T6,_T7);_T3=_TB7;}
# 1554
return _T3;}}}}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TAC=_TAE->f2;}{struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;
# 1557
struct _tuple18 _TAE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;struct Cyc_AssnDef_AssnMap _TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap a1t=_TB0;struct Cyc_AssnDef_AssnMap a1f=_TAF;
struct _tuple18 _TB1=Cyc_Vcgen_vcgen_test(env,e2,a1t);struct Cyc_AssnDef_AssnMap _TB2;struct Cyc_AssnDef_AssnMap _TB3;_TB3=_TB1.f0;_TB2=_TB1.f1;{struct Cyc_AssnDef_AssnMap a2t=_TB3;struct Cyc_AssnDef_AssnMap a2f=_TB2;{struct _tuple18 _TB4;
_TB4.f0=a2t;_T9=env;_TA=Cyc_AssnDef_or_assnmap_assnmap(a1f,a2f);_TB4.f1=Cyc_Vcgen_may_widen(_T9,_TA);_T8=_TB4;}return _T8;}}}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TAC=_TAE->f2;}{struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;
# 1561
struct _tuple18 _TAE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;struct Cyc_AssnDef_AssnMap _TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap a1t=_TB0;struct Cyc_AssnDef_AssnMap a1f=_TAF;
struct _tuple18 _TB1=Cyc_Vcgen_vcgen_test(env,e2,a1f);struct Cyc_AssnDef_AssnMap _TB2;struct Cyc_AssnDef_AssnMap _TB3;_TB3=_TB1.f0;_TB2=_TB1.f1;{struct Cyc_AssnDef_AssnMap a2t=_TB3;struct Cyc_AssnDef_AssnMap a2f=_TB2;{struct _tuple18 _TB4;_TC=env;_TD=
Cyc_AssnDef_or_assnmap_assnmap(a1t,a2t);_TB4.f0=Cyc_Vcgen_may_widen(_TC,_TD);_TB4.f1=a2f;_TB=_TB4;}return _TB;}}}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TAC=_TAE->f2;}{struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;
# 1565
struct _tuple14 _TAE=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap ain=_TAF;_TE=
Cyc_Vcgen_vcgen_test(env,e2,ain);return _TE;}}case 3: _TF=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T10=_TF->f1;if(_T10!=Cyc_Absyn_Not)goto _TL156;_T11=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T12=_T11->f2;if(_T12==0)goto _TL158;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T13=_TAE->f2;{struct Cyc_List_List _TAF=*_T13;_T14=_TAF.hd;_TAD=(struct Cyc_Absyn_Exp*)_T14;}}{struct Cyc_Absyn_Exp*e1=_TAD;
# 1568
struct _tuple18 _TAE=Cyc_Vcgen_vcgen_test(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;struct Cyc_AssnDef_AssnMap _TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{struct Cyc_AssnDef_AssnMap at=_TB0;struct Cyc_AssnDef_AssnMap af=_TAF;{struct _tuple18 _TB1;
_TB1.f0=af;_TB1.f1=at;_T15=_TB1;}return _T15;}}_TL158: goto _LL11;_TL156: _T16=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T17=_T16->f2;if(_T17==0)goto _TL15A;_T18=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T19=_T18->f2;_T1A=(struct Cyc_List_List*)_T19;_T1B=_T1A->tl;if(_T1B==0)goto _TL15C;_T1C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_T1D=_T1C->f2;_T1E=(struct Cyc_List_List*)_T1D;_T1F=_T1E->tl;_T20=(struct Cyc_List_List*)_T1F;_T21=_T20->tl;if(_T21!=0)goto _TL15E;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TA8;_TAA=_TAE->f1;_T22=_TAE->f2;{struct Cyc_List_List _TAF=*_T22;_T23=_TAF.hd;_TAD=(struct Cyc_Absyn_Exp*)_T23;_T24=_TAF.tl;{struct Cyc_List_List _TB0=*_T24;_T25=_TB0.hd;_TAC=(struct Cyc_Absyn_Exp*)_T25;}}}{enum Cyc_Absyn_Primop p=_TAA;struct Cyc_Absyn_Exp*e1=_TAD;struct Cyc_Absyn_Exp*e2=_TAC;
# 1579
struct _tuple14 _TAE=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;void*_TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{void*t1=_TB0;struct Cyc_AssnDef_AssnMap a1=_TAF;
struct _tuple14 _TB1=Cyc_Vcgen_vcgen_rexp(env,e2,a1);struct Cyc_AssnDef_AssnMap _TB2;void*_TB3;_TB3=_TB1.f0;_TB2=_TB1.f1;{void*t2=_TB3;struct Cyc_AssnDef_AssnMap a2=_TB2;
struct Cyc_AssnDef_AssnMap at=a2;
struct Cyc_AssnDef_AssnMap af=a2;_T26=p;_T27=(int)_T26;switch(_T27){case Cyc_Absyn_Eq: _T28=at;_T29=
# 1585
Cyc_AssnDef_eq(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T28,_T29);_T2A=af;_T2B=Cyc_AssnDef_neq(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T2A,_T2B);goto _LL37;case Cyc_Absyn_Neq: _T2C=at;_T2D=
# 1588
Cyc_AssnDef_neq(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T2C,_T2D);_T2E=af;_T2F=Cyc_AssnDef_eq(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T2E,_T2F);goto _LL37;case Cyc_Absyn_ULt: _T30=at;_T31=
# 1590
Cyc_AssnDef_ult(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T30,_T31);_T32=af;_T33=Cyc_AssnDef_ulte(t2,t1);af=Cyc_AssnDef_and_assnmap_assn(_T32,_T33);goto _LL37;case Cyc_Absyn_Lt: _T34=at;_T35=
# 1592
Cyc_AssnDef_slt(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T34,_T35);_T36=af;_T37=Cyc_AssnDef_slte(t2,t1);af=Cyc_AssnDef_and_assnmap_assn(_T36,_T37);goto _LL37;case Cyc_Absyn_ULte: _T38=at;_T39=
# 1594
Cyc_AssnDef_ulte(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T38,_T39);_T3A=af;_T3B=Cyc_AssnDef_ult(t2,t1);af=Cyc_AssnDef_and_assnmap_assn(_T3A,_T3B);goto _LL37;case Cyc_Absyn_Lte: _T3C=at;_T3D=
# 1596
Cyc_AssnDef_slte(t1,t2);at=Cyc_AssnDef_and_assnmap_assn(_T3C,_T3D);_T3E=af;_T3F=Cyc_AssnDef_slt(t2,t1);af=Cyc_AssnDef_and_assnmap_assn(_T3E,_T3F);goto _LL37;case Cyc_Absyn_UGt: _T40=at;_T41=
# 1598
Cyc_AssnDef_ult(t2,t1);at=Cyc_AssnDef_and_assnmap_assn(_T40,_T41);_T42=af;_T43=Cyc_AssnDef_ulte(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T42,_T43);goto _LL37;case Cyc_Absyn_Gt: _T44=at;_T45=
# 1600
Cyc_AssnDef_slt(t2,t1);at=Cyc_AssnDef_and_assnmap_assn(_T44,_T45);_T46=af;_T47=Cyc_AssnDef_slte(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T46,_T47);goto _LL37;case Cyc_Absyn_UGte: _T48=at;_T49=
# 1602
Cyc_AssnDef_ulte(t2,t1);at=Cyc_AssnDef_and_assnmap_assn(_T48,_T49);_T4A=af;_T4B=Cyc_AssnDef_ult(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T4A,_T4B);goto _LL37;case Cyc_Absyn_Gte: _T4C=at;_T4D=
# 1604
Cyc_AssnDef_slte(t2,t1);at=Cyc_AssnDef_and_assnmap_assn(_T4C,_T4D);_T4E=af;_T4F=Cyc_AssnDef_slt(t1,t2);af=Cyc_AssnDef_and_assnmap_assn(_T4E,_T4F);goto _LL37;default: _T50=env;_T51=e;_T52=_T51->topt;_T53=p;_T54=t1;_T55=t2;_T56=at;{
# 1606
struct _tuple14 _TB4=Cyc_Vcgen_vcgen_binop(_T50,_T52,_T53,_T54,_T55,_T56);struct Cyc_AssnDef_AssnMap _TB5;void*_TB6;_TB6=_TB4.f0;_TB5=_TB4.f1;{void*t=_TB6;struct Cyc_AssnDef_AssnMap am=_TB5;_T57=at;_T58=
Cyc_AssnDef_one();_T59=t;_T5A=Cyc_AssnDef_ulte(_T58,_T59);at=Cyc_AssnDef_and_assnmap_assn(_T57,_T5A);_T5B=af;_T5C=t;_T5D=
Cyc_AssnDef_zero();_T5E=Cyc_AssnDef_eq(_T5C,_T5D);af=Cyc_AssnDef_and_assnmap_assn(_T5B,_T5E);goto _LL37;}}}_LL37:{struct _tuple18 _TB4;
# 1611
_TB4.f0=at;_TB4.f1=af;_T5F=_TB4;}return _T5F;}}}_TL15E: goto _LL11;_TL15C: goto _LL11;_TL15A: goto _LL11;case 37:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_TA8;_TAD=_TAE->f1;_TA9=_TAE->f2;}{struct Cyc_Absyn_Exp*e1=_TAD;struct _fat_ptr*f=_TA9;_T60=e1;_T61=_T60->topt;_T62=
# 1572
_check_null(_T61);_T63=f;_T64=Cyc_CfFlowInfo_get_field_index(_T62,_T63);_T65=(unsigned)_T64;{void*i=Cyc_AssnDef_uint(_T65);
struct _tuple14 _TAE=Cyc_Vcgen_vcgen_rexp(env,e1,ain);struct Cyc_AssnDef_AssnMap _TAF;void*_TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{void*t=_TB0;struct Cyc_AssnDef_AssnMap ain=_TAF;
void*tt=Cyc_AssnDef_tagof_tm(t);_T66=ain;_T67=
Cyc_AssnDef_eq(tt,i);{struct Cyc_AssnDef_AssnMap at=Cyc_AssnDef_and_assnmap_assn(_T66,_T67);_T68=ain;_T69=
Cyc_AssnDef_neq(tt,i);{struct Cyc_AssnDef_AssnMap af=Cyc_AssnDef_and_assnmap_assn(_T68,_T69);{struct _tuple18 _TB1;
_TB1.f0=at;_TB1.f1=af;_T6A=_TB1;}return _T6A;}}}}}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TAE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TA8;_T6B=_TAE->f1;_TAD=(void*)_T6B;_TAC=_TAE->f2;}{void*tp=_TAD;struct Cyc_Absyn_Exp*e1=_TAC;{struct _tuple0 _TAE;
# 1613
_TAE.f0=Cyc_Absyn_compress(tp);_T6D=e1;_T6E=_T6D->topt;_T6F=_check_null(_T6E);_TAE.f1=Cyc_Absyn_compress(_T6F);_T6C=_TAE;}{struct _tuple0 _TAE=_T6C;enum Cyc_Absyn_Size_of _TAF;enum Cyc_Absyn_Sign _TB0;enum Cyc_Absyn_Size_of _TB1;enum Cyc_Absyn_Sign _TB2;_T70=_TAE.f0;_T71=(int*)_T70;_T72=*_T71;if(_T72!=0)goto _TL161;_T73=_TAE.f0;_T74=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T73;_T75=_T74->f1;_T76=(int*)_T75;_T77=*_T76;if(_T77!=1)goto _TL163;_T78=_TAE.f1;_T79=(int*)_T78;_T7A=*_T79;if(_T7A!=0)goto _TL165;_T7B=_TAE.f1;_T7C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7B;_T7D=_T7C->f1;_T7E=(int*)_T7D;_T7F=*_T7E;if(_T7F!=1)goto _TL167;_T80=_TAE.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T80;_T81=_TB3->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TB4=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T81;_TB2=_TB4->f1;_TB1=_TB4->f2;}}_T82=_TAE.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB3=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T82;_T83=_TB3->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TB4=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T83;_TB0=_TB4->f1;_TAF=_TB4->f2;}}{enum Cyc_Absyn_Sign u1=_TB2;enum Cyc_Absyn_Size_of s1=_TB1;enum Cyc_Absyn_Sign u2=_TB0;enum Cyc_Absyn_Size_of s2=_TAF;_T85=s1;_T86=(int)_T85;
# 1619
if(_T86!=3)goto _TL169;_T84=2U;goto _TL16A;_TL169: _T84=s1;_TL16A: s1=_T84;_T88=s2;_T89=(int)_T88;
if(_T89!=3)goto _TL16B;_T87=2U;goto _TL16C;_TL16B: _T87=s2;_TL16C: s2=_T87;_T8A=s1;_T8B=(int)_T8A;_T8C=s2;_T8D=(int)_T8C;
if(_T8B!=_T8D)goto _TL16D;_T8E=Cyc_Vcgen_vcgen_test(env,e1,ain);return _T8E;_TL16D: goto _LL51;}_TL167: goto _LL54;_TL165: goto _LL54;_TL163: goto _LL54;_TL161: _LL54: goto _LL51;_LL51:;}goto _LL12;}default: _LL11: _LL12: {
# 1627
struct _tuple14 _TAE=Cyc_Vcgen_vcgen_rexp(env,e,ain);struct Cyc_AssnDef_AssnMap _TAF;void*_TB0;_TB0=_TAE.f0;_TAF=_TAE.f1;{void*t=_TB0;struct Cyc_AssnDef_AssnMap aout=_TAF;_T8F=t;_T90=(int*)_T8F;_T91=*_T90;if(_T91!=0)goto _TL16F;_T92=t;_T93=(struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_T92;_T94=_T93->f1;if(_T94!=0)goto _TL171;{struct _tuple18 _TB1;_T96=aout;_T97=& Cyc_AssnDef_false_assn;_T98=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T97;_T99=(void*)_T98;
# 1630
_TB1.f0=Cyc_AssnDef_and_assnmap_assn(_T96,_T99);_TB1.f1=aout;_T95=_TB1;}return _T95;_TL171:{struct _tuple18 _TB1;
# 1632
_TB1.f0=aout;_T9B=aout;_T9C=& Cyc_AssnDef_false_assn;_T9D=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T9C;_T9E=(void*)_T9D;_TB1.f1=Cyc_AssnDef_and_assnmap_assn(_T9B,_T9E);_T9A=_TB1;}return _T9A;_TL16F:{struct _tuple18 _TB1;_TA0=aout;_TA1=
# 1634
Cyc_AssnDef_one();_TA2=t;_TA3=Cyc_AssnDef_ulte(_TA1,_TA2);_TB1.f0=Cyc_AssnDef_and_assnmap_assn(_TA0,_TA3);_TA4=aout;_TA5=t;_TA6=Cyc_AssnDef_zero();_TA7=Cyc_AssnDef_eq(_TA5,_TA6);_TB1.f1=Cyc_AssnDef_and_assnmap_assn(_TA4,_TA7);_T9F=_TB1;}return _T9F;;}}};}}
# 1640
static void Cyc_Vcgen_update_return(struct Cyc_Vcgen_Env*env,void*topt,struct Cyc_AssnDef_AssnMap a,void*tp){struct Cyc_AssnDef_AssnMap _T0;void*_T1;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T2;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T3;void*_T4;struct Cyc_Vcgen_Env*_T5;struct Cyc_AssnDef_AssnMap*_T6;struct Cyc_Vcgen_Env*_T7;struct Cyc_Absyn_Vardecl*_T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_AssnDef_AssnMap _TA;struct Cyc_Vcgen_Env*_TB;struct Cyc_Vcgen_Env*_TC;struct Cyc_Absyn_Vardecl*_TD;void*_TE;void*_TF;struct Cyc_Vcgen_Env*_T10;struct Cyc_Vcgen_Env*_T11;struct Cyc_AssnDef_AssnMap*_T12;struct Cyc_AssnDef_AssnMap*_T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_AssnMap _T15;struct Cyc_AssnDef_AssnMap _T16;struct Cyc_Vcgen_Env*_T17;struct Cyc_AssnDef_AssnMap*_T18;struct Cyc_AssnDef_AssnMap*_T19;_T0=a;_T1=_T0.assn;_T2=& Cyc_AssnDef_false_assn;_T3=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T2;_T4=(void*)_T3;
# 1642
if(_T1!=_T4)goto _TL173;return;_TL173: {
void*t;
if(topt==0)goto _TL175;t=topt;goto _TL176;
_TL175: t=Cyc_AssnDef_fresh_var(0,tp);_TL176: _T5=env;_T6=_T5->res_assnmap;
# 1647
if(_T6!=0)goto _TL177;return;_TL177: _T7=env;_T8=_T7->result;
if(_T8==0)goto _TL179;_T9=env;_TA=a;_TB=env;_TC=env;_TD=_TC->result;_TE=
Cyc_Vcgen_myaddr(_TB,_TD);_TF=t;a=Cyc_Vcgen_do_assign(_T9,_TA,_TE,_TF);goto _TL17A;_TL179: _TL17A: _T10=env;_T11=env;_T12=_T11->res_assnmap;_T13=
# 1651
_check_null(_T12);_T14=*_T13;_T15=a;_T16=Cyc_AssnDef_or_assnmap_assnmap(_T14,_T15);a=Cyc_Vcgen_may_widen(_T10,_T16);_T17=env;_T18=_T17->res_assnmap;_T19=
_check_null(_T18);*_T19=a;}}
# 1655
static struct Cyc_Absyn_Exp*Cyc_Vcgen_find_assert_e(struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T3;struct Cyc_Absyn_Exp*_T4;struct Cyc_Absyn_Exp*_T5;void*_T6;int*_T7;int _T8;struct Cyc_Absyn_Exp*_T9;void*_TA;int _TB;struct Cyc_Absyn_Exp*_TC;
_TL17B: if(1)goto _TL17C;else{goto _TL17D;}
_TL17C: _T0=s;{void*_TD=_T0->r;struct Cyc_Absyn_Stmt*_TE;int _TF;struct Cyc_Absyn_Exp*_T10;_T1=(int*)_TD;_T2=*_T1;switch(_T2){case 1: _T3=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_TD;_T4=_T3->f1;_T5=(struct Cyc_Absyn_Exp*)_T4;_T6=_T5->r;_T7=(int*)_T6;_T8=*_T7;if(_T8!=41)goto _TL17F;{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T11=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_TD;_T9=_T11->f1;{struct Cyc_Absyn_Exp _T12=*_T9;_TA=_T12.r;{struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*_T13=(struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_TA;_T10=_T13->f1;_TF=_T13->f2;}}}_TB=(int)_TF;if(!_TB)goto _TL181;{struct Cyc_Absyn_Exp*e=_T10;int static_only=_TF;_TC=e;
return _TC;}_TL181: goto _LL5;_TL17F: goto _LL5;case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T11=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_TD;_TE=_T11->f1;}{struct Cyc_Absyn_Stmt*sa=_TE;
s=sa;goto _TL17B;}default: _LL5:
 return 0;};}goto _TL17B;_TL17D:;}
# 1673 "vcgen.cyc"
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_stmt(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Stmt*s,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;struct Cyc_AssnDef_AssnMap _T3;struct Cyc_Vcgen_Env*_T4;void**_T5;struct Cyc_Vcgen_Env*_T6;void**_T7;struct Cyc_AssnDef_AssnMap _T8;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Vcgen_Env*_TB;void*_TC;struct Cyc_AssnDef_AssnMap _TD;struct Cyc_Absyn_Exp*_TE;void*_TF;struct Cyc_AssnDef_AssnMap _T10;struct Cyc_AssnDef_AssnMap _T11;struct Cyc_Vcgen_Env*_T12;struct Cyc_AssnDef_AssnMap _T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_Vcgen_Env*_T15;struct Cyc_AssnDef_AssnMap*_T16;struct Cyc_AssnDef_AssnMap*_T17;struct Cyc_Vcgen_Env*_T18;struct Cyc_AssnDef_AssnMap*_T19;struct Cyc_AssnDef_AssnMap*_T1A;struct Cyc_AssnDef_AssnMap _T1B;struct Cyc_AssnDef_AssnMap _T1C;struct Cyc_AssnDef_AssnMap _T1D;struct Cyc_Absyn_Stmt*(*_T1E)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T1F)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Vcgen_Env*_T20;struct Cyc_Hashtable_Table*_T21;struct Cyc_Absyn_Stmt*_T22;struct Cyc_AssnDef_AssnMap _T23;struct _tuple11 _T24;struct _tuple11 _T25;struct Cyc_AssnDef_AssnMap _T26;struct _tuple11 _T27;struct _tuple11 _T28;struct _tuple11 _T29;struct _tuple11 _T2A;struct Cyc_AssnDef_AssnMap _T2B;struct _tuple11 _T2C;struct _tuple11 _T2D;struct Cyc_AssnDef_AssnMap _T2E;void*_T2F;struct Cyc_AssnDef_AssnMap _T30;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T31;struct Cyc_Absyn_Switch_clause**_T32;int(*_T33)(struct _fat_ptr,struct _fat_ptr);void*(*_T34)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T35;struct _fat_ptr _T36;struct Cyc_Absyn_Switch_clause**_T37;struct Cyc_Absyn_Switch_clause*_T38;struct Cyc_Core_Opt*_T39;struct Cyc_Core_Opt*_T3A;void*_T3B;struct Cyc_List_List*_T3C;struct _tuple1 _T3D;struct Cyc_List_List*_T3E;struct Cyc_List_List*_T3F;void*_T40;struct Cyc_Vcgen_Env*_T41;struct Cyc_AssnDef_AssnMap _T42;void*_T43;struct Cyc_List_List*_T44;void*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;struct Cyc_Vcgen_Env*_T48;struct Cyc_Absyn_Switch_clause*_T49;struct Cyc_Absyn_Stmt*_T4A;struct Cyc_AssnDef_AssnMap _T4B;struct Cyc_AssnDef_AssnMap _T4C;struct Cyc_Vcgen_Env*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_AssnDef_AssnMap _T4F;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T50;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T51;struct Cyc_AssnDef_AssnMap _T52;struct Cyc_AssnDef_AssnMap _T53;void*_T54;struct Cyc_AssnDef_AssnMap _T55;void*_T56;int _T57;struct Cyc_String_pa_PrintArg_struct _T58;struct Cyc_String_pa_PrintArg_struct _T59;struct Cyc_Absyn_Stmt*_T5A;unsigned _T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;struct Cyc_Vcgen_Env*_T5E;struct Cyc_Absyn_Exp*_T5F;struct Cyc_AssnDef_AssnMap _T60;struct Cyc_Absyn_Stmt*(*_T61)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*);void*(*_T62)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Vcgen_Env*_T63;struct Cyc_Hashtable_Table*_T64;struct Cyc_Absyn_Stmt*_T65;struct Cyc_Absyn_Stmt*_T66;int*_T67;int _T68;struct Cyc_Vcgen_Env*_T69;struct Cyc_Absyn_Exp*_T6A;struct Cyc_AssnDef_AssnMap _T6B;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T6C;struct Cyc_AssnDef_True_AssnDef_Assn_struct*_T6D;struct Cyc_AssnDef_AssnMap _T6E;struct Cyc_AssnDef_AssnMap _T6F;void*_T70;struct Cyc_AssnDef_AssnMap _T71;void*_T72;int _T73;struct Cyc_String_pa_PrintArg_struct _T74;struct Cyc_AssnDef_AssnMap _T75;void*_T76;struct Cyc_String_pa_PrintArg_struct _T77;struct Cyc_Absyn_Stmt*_T78;unsigned _T79;struct _fat_ptr _T7A;struct _fat_ptr _T7B;int(*_T7C)(struct _fat_ptr,struct _fat_ptr);void*(*_T7D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7E;struct _fat_ptr _T7F;struct Cyc_AssnDef_AssnMap _T80;void*_T81;struct Cyc_Vcgen_Env*_T82;struct Cyc_Vcgen_Env*_T83;struct Cyc_AssnDef_AssnMap*_T84;struct Cyc_Vcgen_Env*_T85;struct Cyc_AssnDef_AssnMap*_T86;struct Cyc_AssnDef_AssnMap*_T87;struct Cyc_Vcgen_Env*_T88;struct Cyc_Absyn_Vardecl*_T89;struct Cyc_AssnDef_AssnMap _T8A;struct Cyc_Vcgen_Env*_T8B;struct Cyc_AssnDef_AssnMap _T8C;struct Cyc_AssnDef_AssnMap _T8D;
LOOP: {
struct Cyc_AssnDef_AssnMap oldam=Cyc_Vcgen_lookup_stmt_assnmap(env,s);
amin=Cyc_AssnDef_or_assnmap_assnmap(oldam,amin);_T0=s;{
void*_T8E=_T0->r;struct Cyc_Absyn_Switch_clause*_T8F;struct Cyc_List_List*_T90;struct Cyc_Absyn_Stmt*_T91;struct Cyc_Absyn_Stmt*_T92;struct Cyc_Absyn_Exp*_T93;struct Cyc_Absyn_Stmt*_T94;void*_T95;void*_T96;_T1=(int*)_T8E;_T2=*_T1;switch(_T2){case 0: _T3=amin;
return _T3;case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;}{struct Cyc_Absyn_Exp*e=_T96;
# 1680
struct _tuple14 _T97=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T98;void*_T99;_T99=_T97.f0;_T98=_T97.f1;{void*t=_T99;struct Cyc_AssnDef_AssnMap amout=_T98;_T4=env;_T5=_T4->exp_stmt;
if(_T5==0)goto _TL184;_T6=env;_T7=_T6->exp_stmt;
*_T7=t;goto _TL185;_TL184: _TL185: _T8=amout;
return _T8;}}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T95=_T97->f2;}{struct Cyc_Absyn_Stmt*s1=_T96;struct Cyc_Absyn_Stmt*s2=_T95;
# 1685
amin=Cyc_Vcgen_vcgen_stmt(env,s1,amin);
s=s2;goto LOOP;}case 3: _T9=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T8E;_TA=_T9->f1;if(_TA==0)goto _TL186;{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;}{struct Cyc_Absyn_Exp*e=_T96;
# 1689
struct _tuple14 _T97=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T98;void*_T99;_T99=_T97.f0;_T98=_T97.f1;{void*t=_T99;struct Cyc_AssnDef_AssnMap amout=_T98;_TB=env;_TC=t;_TD=amout;_TE=e;_TF=_TE->topt;
Cyc_Vcgen_update_return(_TB,_TC,_TD,_TF);_T10=
Cyc_AssnDef_false_assnmap();return _T10;}}_TL186:
# 1693
 Cyc_Vcgen_update_return(env,0,amin,0);_T11=
Cyc_AssnDef_false_assnmap();return _T11;case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T95=_T97->f2;_T94=_T97->f3;}{struct Cyc_Absyn_Exp*e1=_T96;struct Cyc_Absyn_Stmt*s1=_T95;struct Cyc_Absyn_Stmt*s2=_T94;
# 1696
struct _tuple18 _T97=Cyc_Vcgen_vcgen_test(env,e1,amin);struct Cyc_AssnDef_AssnMap _T98;struct Cyc_AssnDef_AssnMap _T99;_T99=_T97.f0;_T98=_T97.f1;{struct Cyc_AssnDef_AssnMap amt=_T99;struct Cyc_AssnDef_AssnMap amf=_T98;
amt=Cyc_Vcgen_vcgen_stmt(env,s1,amt);
amf=Cyc_Vcgen_vcgen_stmt(env,s2,amf);_T12=env;_T13=
Cyc_AssnDef_or_assnmap_assnmap(amt,amf);_T14=Cyc_Vcgen_may_widen(_T12,_T13);return _T14;}}case 7: _T15=env;_T16=_T15->continue_assnmap;_T17=
# 1701
_check_null(_T16);_T18=env;_T19=_T18->continue_assnmap;_T1A=_check_null(_T19);_T1B=*_T1A;_T1C=amin;*_T17=Cyc_AssnDef_or_assnmap_assnmap(_T1B,_T1C);_T1D=
Cyc_AssnDef_false_assnmap();return _T1D;case 6: _T1F=Cyc_Hashtable_lookup;{
# 1704
struct Cyc_Absyn_Stmt*(*_T97)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T1F;_T1E=_T97;}_T20=env;_T21=_T20->succ_table;_T22=s;{struct Cyc_Absyn_Stmt*dest=_T1E(_T21,_T22);
if(dest!=0)goto _TL188;
Cyc_Vcgen_update_return(env,0,amin,0);goto _TL189;
# 1708
_TL188: Cyc_Vcgen_update_stmt_assnmap(env,dest,amin);_TL189: _T23=
Cyc_AssnDef_false_assnmap();return _T23;}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T8E;_T24=_T97->f1;_T96=_T24.f0;_T25=_T97->f1;_T95=_T25.f1;_T94=_T97->f2;}{struct Cyc_Absyn_Exp*e=_T96;struct Cyc_Absyn_Stmt*dummy=_T95;struct Cyc_Absyn_Stmt*s1=_T94;_T26=
# 1711
Cyc_Vcgen_vcgen_loop(env,0,0,e,dummy,s1,0,amin);return _T26;}case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T27=_T97->f2;_T95=_T27.f0;_T28=_T97->f2;_T94=_T28.f1;_T29=_T97->f3;_T93=_T29.f0;_T2A=_T97->f3;_T92=_T2A.f1;_T91=_T97->f4;}{struct Cyc_Absyn_Exp*e1=_T96;struct Cyc_Absyn_Exp*e2=_T95;struct Cyc_Absyn_Stmt*dummy2=_T94;struct Cyc_Absyn_Exp*e3=_T93;struct Cyc_Absyn_Stmt*dummy3=_T92;struct Cyc_Absyn_Stmt*s1=_T91;_T2B=
# 1713
Cyc_Vcgen_vcgen_loop(env,e1,0,e2,dummy2,s1,e3,amin);return _T2B;}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T2C=_T97->f2;_T95=_T2C.f0;_T2D=_T97->f2;_T94=_T2D.f1;}{struct Cyc_Absyn_Stmt*s1=_T96;struct Cyc_Absyn_Exp*e=_T95;struct Cyc_Absyn_Stmt*dummy=_T94;_T2E=
# 1715
Cyc_Vcgen_vcgen_loop(env,0,s1,e,dummy,s1,0,amin);return _T2E;}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T90=_T97->f2;_T2F=_T97->f3;_T95=(void*)_T2F;}{struct Cyc_Absyn_Exp*e=_T96;struct Cyc_List_List*scs=_T90;void*dtree=_T95;
# 1717
struct _tuple14 _T97=Cyc_Vcgen_vcgen_rexp(env,e,amin);struct Cyc_AssnDef_AssnMap _T98;void*_T99;_T99=_T97.f0;_T98=_T97.f1;{void*t=_T99;struct Cyc_AssnDef_AssnMap am=_T98;_T30=
Cyc_Vcgen_vcgen_switch(env,t,scs,am);return _T30;}}case 11: _T31=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T8E;_T32=_T31->f2;if(_T32!=0)goto _TL18A;_T34=Cyc_Warn_impos;{
int(*_T97)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T34;_T33=_T97;}_T35=_tag_fat("vcgen fallthru",sizeof(char),15U);_T36=_tag_fat(0U,sizeof(void*),0);_T33(_T35,_T36);goto _TL18B;_TL18A:{struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T37=_T97->f2;{struct Cyc_Absyn_Switch_clause*_T98=*_T37;_T8F=_T98;}}{struct Cyc_List_List*es=_T96;struct Cyc_Absyn_Switch_clause*dest=_T8F;
# 1722
struct _tuple17 _T97=Cyc_Vcgen_vcgen_rexps(env,es,amin);struct Cyc_AssnDef_AssnMap _T98;struct Cyc_List_List*_T99;_T99=_T97.f0;_T98=_T97.f1;{struct Cyc_List_List*ts=_T99;struct Cyc_AssnDef_AssnMap am=_T98;
if(ts==0)goto _TL18C;_T38=dest;_T39=_T38->pat_vars;_T3A=
# 1725
_check_null(_T39);_T3B=_T3A->v;_T3C=(struct Cyc_List_List*)_T3B;_T3D=Cyc_List_split(_T3C);_T3E=_T3D.f0;{struct Cyc_List_List*x=Cyc_Tcutil_filter_nulls(_T3E);
# 1727
_TL191: if(x!=0)goto _TL18F;else{goto _TL190;}
_TL18F: _T3F=x;_T40=_T3F->hd;{struct Cyc_Absyn_Vardecl*vd=(struct Cyc_Absyn_Vardecl*)_T40;
if(vd==0)goto _TL192;_T41=env;_T42=am;_T43=
Cyc_Vcgen_myaddr(env,vd);_T44=_check_null(ts);_T45=_T44->hd;am=Cyc_Vcgen_do_assign(_T41,_T42,_T43,_T45);goto _TL193;_TL192: _TL193:;}_T46=x;
# 1727
x=_T46->tl;_T47=ts;ts=_T47->tl;goto _TL191;_TL190:;}goto _TL18D;_TL18C: _TL18D: _T48=env;_T49=dest;_T4A=_T49->body;_T4B=am;
# 1733
Cyc_Vcgen_update_stmt_assnmap(_T48,_T4A,_T4B);_T4C=
Cyc_AssnDef_false_assnmap();return _T4C;}}_TL18B:;case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T8E;_T96=_T97->f1;_T94=_T97->f2;}{struct Cyc_Absyn_Decl*d=_T96;struct Cyc_Absyn_Stmt*s1=_T94;
# 1736
amin=Cyc_Vcgen_vcgen_local_decl(env,d,amin);
s=s1;goto LOOP;}case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T8E;_T94=_T97->f2;}{struct Cyc_Absyn_Stmt*s1=_T94;
# 1743
struct Cyc_Absyn_Exp*label_assn_opt=Cyc_Vcgen_find_assert_e(s1);
if(label_assn_opt!=0)goto _TL194;
# 1749
amin=Cyc_AssnDef_true_assnmap();goto _TL195;
# 1752
_TL194: _T4D=env;_T4E=label_assn_opt;{struct Cyc_AssnDef_AssnMap _T97;_T50=& Cyc_AssnDef_true_assn;_T51=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T50;_T97.assn=(void*)_T51;_T52=amin;_T97.map=_T52.map;_T4F=_T97;}{struct _tuple18 _T97=Cyc_Vcgen_vcgen_test(_T4D,_T4E,_T4F);struct Cyc_AssnDef_AssnMap _T98;_T98=_T97.f0;{struct Cyc_AssnDef_AssnMap amt=_T98;_T53=amin;_T54=_T53.assn;_T55=amt;_T56=_T55.assn;_T57=
Cyc_Vcgen_implies(_T54,_T56);if(_T57)goto _TL196;else{goto _TL198;}
_TL198:{struct Cyc_String_pa_PrintArg_struct _T99;_T99.tag=0;
_T99.f1=Cyc_AssnDef_assnmap2string(amin);_T58=_T99;}{struct Cyc_String_pa_PrintArg_struct _T99=_T58;{struct Cyc_String_pa_PrintArg_struct _T9A;_T9A.tag=0;_T9A.f1=Cyc_AssnDef_assnmap2string(amt);_T59=_T9A;}{struct Cyc_String_pa_PrintArg_struct _T9A=_T59;void*_T9B[2];_T9B[0]=& _T99;_T9B[1]=& _T9A;_T5A=s;_T5B=_T5A->loc;_T5C=
# 1754
_tag_fat("can not prove @assert(%s) \n from %s",sizeof(char),36U);_T5D=_tag_fat(_T9B,sizeof(void*),2);Cyc_Warn_err(_T5B,_T5C,_T5D);}}goto _TL197;_TL196: _TL197: _T5E=env;_T5F=label_assn_opt;_T60=
# 1759
Cyc_AssnDef_true_assnmap();{struct _tuple18 _T99=Cyc_Vcgen_vcgen_test(_T5E,_T5F,_T60);struct Cyc_AssnDef_AssnMap _T9A;_T9A=_T99.f0;{struct Cyc_AssnDef_AssnMap amt=_T9A;
amin=amt;}}}}_TL195:
# 1762
 s=s1;goto LOOP;}case 8: _T62=Cyc_Hashtable_lookup;{
# 1767
struct Cyc_Absyn_Stmt*(*_T97)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Hashtable_Table*,struct Cyc_Absyn_Stmt*))_T62;_T61=_T97;}_T63=env;_T64=_T63->succ_table;_T65=s;{struct Cyc_Absyn_Stmt*destopt=_T61(_T64,_T65);
if(destopt==0)goto _TL199;_T66=destopt;{
void*_T97=_T66->r;struct Cyc_Absyn_Stmt*_T98;_T67=(int*)_T97;_T68=*_T67;if(_T68!=13)goto _TL19B;{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T99=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T97;_T98=_T99->f2;}{struct Cyc_Absyn_Stmt*s1=_T98;
# 1771
struct Cyc_Absyn_Exp*label_assn_opt=Cyc_Vcgen_find_assert_e(s1);
if(label_assn_opt!=0)goto _TL19D;goto _LL3A;_TL19D: {
struct Cyc_Absyn_Exp*label_assn=label_assn_opt;_T69=env;_T6A=label_assn;{struct Cyc_AssnDef_AssnMap _T99;_T6C=& Cyc_AssnDef_true_assn;_T6D=(struct Cyc_AssnDef_True_AssnDef_Assn_struct*)_T6C;
_T99.assn=(void*)_T6D;_T6E=amin;_T99.map=_T6E.map;_T6B=_T99;}{struct _tuple18 _T99=Cyc_Vcgen_vcgen_test(_T69,_T6A,_T6B);struct Cyc_AssnDef_AssnMap _T9A;_T9A=_T99.f0;{struct Cyc_AssnDef_AssnMap amt=_T9A;_T6F=amin;_T70=_T6F.assn;_T71=amt;_T72=_T71.assn;_T73=
Cyc_Vcgen_implies(_T70,_T72);if(_T73)goto _TL19F;else{goto _TL1A1;}
_TL1A1:{struct Cyc_String_pa_PrintArg_struct _T9B;_T9B.tag=0;_T75=amt;_T76=_T75.assn;
_T9B.f1=Cyc_AssnDef_assn2string(_T76);_T74=_T9B;}{struct Cyc_String_pa_PrintArg_struct _T9B=_T74;{struct Cyc_String_pa_PrintArg_struct _T9C;_T9C.tag=0;_T9C.f1=Cyc_AssnDef_assnmap2string(amin);_T77=_T9C;}{struct Cyc_String_pa_PrintArg_struct _T9C=_T77;void*_T9D[2];_T9D[0]=& _T9B;_T9D[1]=& _T9C;_T78=s;_T79=_T78->loc;_T7A=
# 1776
_tag_fat("cannot prove @assert(%s)\nfrom %s",sizeof(char),33U);_T7B=_tag_fat(_T9D,sizeof(void*),2);Cyc_Warn_err(_T79,_T7A,_T7B);}}goto _TL1A0;_TL19F: _TL1A0: goto _LL3A;}}}}_TL19B: _T7D=Cyc_Warn_impos;{
# 1779
int(*_T99)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7D;_T7C=_T99;}_T7E=_tag_fat("vcgen goto",sizeof(char),11U);_T7F=_tag_fat(0U,sizeof(void*),0);_T7C(_T7E,_T7F);_LL3A:;}goto _TL19A;_TL199: _TL19A: _T80=
# 1782
Cyc_AssnDef_false_assnmap();return _T80;}default:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T97=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T8E;_T94=_T97->f1;_T90=_T97->f2;_T81=_T97->f3;_T96=(void*)_T81;}{struct Cyc_Absyn_Stmt*s=_T94;struct Cyc_List_List*scs=_T90;void*dtree=_T96;_T82=env;{
# 1800 "vcgen.cyc"
struct Cyc_AssnDef_AssnMap*old_try_assnmap=_T82->try_assnmap;_T83=env;{struct Cyc_AssnDef_AssnMap*_T97=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));
# 1802
*_T97=Cyc_AssnDef_false_assnmap();_T84=(struct Cyc_AssnDef_AssnMap*)_T97;}_T83->try_assnmap=_T84;{
# 1805
struct Cyc_AssnDef_AssnMap am1=Cyc_Vcgen_vcgen_stmt(env,s,amin);_T85=env;_T86=_T85->try_assnmap;_T87=
# 1807
_check_null(_T86);{struct Cyc_AssnDef_AssnMap new_try_assnmap=*_T87;_T88=env;
_T88->try_assnmap=old_try_assnmap;_T89=
# 1811
Cyc_AssnDef_exception_vardecl();_T8A=am1;{struct _tuple14 _T97=Cyc_AssnDef_lookup_var_map(_T89,_T8A);struct Cyc_AssnDef_AssnMap _T98;void*_T99;_T99=_T97.f0;_T98=_T97.f1;{void*exc=_T99;struct Cyc_AssnDef_AssnMap am2=_T98;
struct Cyc_AssnDef_AssnMap am3=Cyc_Vcgen_vcgen_switch(env,exc,scs,new_try_assnmap);_T8B=env;_T8C=
Cyc_AssnDef_or_assnmap_assnmap(am1,am2);_T8D=Cyc_Vcgen_may_widen(_T8B,_T8C);return _T8D;}}}}}}};}}}struct _tuple25{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};
# 1818
static struct _tuple18 Cyc_Vcgen_vcgen_pat(struct Cyc_Vcgen_Env*env,struct Cyc_Absyn_Pat*p,void*lvalue,void*v,struct Cyc_AssnDef_AssnMap amin){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct _tuple18 _T3;struct _tuple18 _T4;struct Cyc_Vcgen_Env*_T5;struct Cyc_AssnDef_AssnMap _T6;void*_T7;void*_T8;struct _tuple18 _T9;struct Cyc_Vcgen_Env*_TA;struct Cyc_AssnDef_AssnMap _TB;void*_TC;void*_TD;int(*_TE)(struct _fat_ptr,struct _fat_ptr);void*(*_TF)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T10;struct _fat_ptr _T11;struct _tuple18 _T12;struct Cyc_Vcgen_Env*_T13;struct Cyc_AssnDef_AssnMap _T14;void*_T15;void*_T16;struct _tuple18 _T17;struct Cyc_Vcgen_Env*_T18;struct Cyc_AssnDef_AssnMap _T19;void*_T1A;void*_T1B;struct _tuple18 _T1C;struct Cyc_AssnDef_AssnMap _T1D;void*_T1E;struct Cyc_AssnDef_AssnMap _T1F;void*_T20;int _T21;unsigned _T22;struct _tuple18 _T23;struct Cyc_AssnDef_AssnMap _T24;void*_T25;struct Cyc_AssnDef_AssnMap _T26;void*_T27;char _T28;unsigned _T29;struct _tuple18 _T2A;struct Cyc_AssnDef_AssnMap _T2B;void*_T2C;struct Cyc_AssnDef_AssnMap _T2D;void*_T2E;struct Cyc_Absyn_Exp*_T2F;int _T30;unsigned _T31;struct _tuple18 _T32;struct Cyc_AssnDef_AssnMap _T33;void*_T34;struct Cyc_AssnDef_AssnMap _T35;void*_T36;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T37;void*_T38;struct Cyc_Absyn_Exp*_T39;struct Cyc_Absyn_Pat*_T3A;struct _tuple18 _T3B;struct Cyc_AssnDef_AssnMap _T3C;void*_T3D;struct Cyc_AssnDef_AssnMap _T3E;void*_T3F;void*_T40;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T41;void*_T42;struct Cyc_Absyn_Exp*_T43;struct Cyc_Absyn_Pat*_T44;struct _tuple18 _T45;struct Cyc_AssnDef_AssnMap _T46;void*_T47;struct Cyc_AssnDef_AssnMap _T48;void*_T49;void*_T4A;void*_T4B;struct Cyc_Absyn_Pat*_T4C;void*_T4D;struct _tuple18 _T4E;struct Cyc_AssnDef_AssnMap _T4F;void*_T50;void*_T51;void*_T52;struct Cyc_AssnDef_AssnMap _T53;void*_T54;void*_T55;void*_T56;void*_T57;struct Cyc_List_List*_T58;void*_T59;struct _tuple25*_T5A;struct Cyc_Absyn_Pat*_T5B;void*_T5C;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T5D;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T5E;void*_T5F;struct _tuple25*_T60;struct Cyc_List_List*_T61;int _T62;void*_T63;int _T64;unsigned _T65;void*_T66;struct Cyc_AssnDef_AssnMap _T67;void*_T68;void*_T69;int _T6A;unsigned _T6B;struct Cyc_Absyn_Pat*_T6C;void*_T6D;void*_T6E;int _T6F;unsigned _T70;struct Cyc_Vcgen_Env*_T71;struct Cyc_AssnDef_AssnMap _T72;struct Cyc_List_List*_T73;struct _tuple18 _T74;unsigned _T75;void*_T76;int _T77;unsigned _T78;void*_T79;struct Cyc_List_List*_T7A;void*_T7B;void*_T7C;unsigned _T7D;struct Cyc_Absyn_Pat*_T7E;void*_T7F;struct Cyc_Vcgen_Env*_T80;struct Cyc_AssnDef_AssnMap _T81;struct Cyc_List_List*_T82;struct Cyc_AssnDef_AssnMap _T83;void*_T84;struct _tuple18 _T85;int(*_T86)(struct _fat_ptr,struct _fat_ptr);void*(*_T87)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T88;struct _fat_ptr _T89;_T0=p;{
# 1820
void*_T8A=_T0->r;struct Cyc_Absyn_Datatypefield*_T8B;struct Cyc_Absyn_Datatypedecl*_T8C;struct Cyc_List_List*_T8D;struct Cyc_Absyn_Enumfield*_T8E;struct _fat_ptr _T8F;char _T90;int _T91;enum Cyc_Absyn_Sign _T92;struct Cyc_Absyn_Vardecl*_T93;struct Cyc_Absyn_Pat*_T94;void*_T95;_T1=(int*)_T8A;_T2=*_T1;switch(_T2){case 0:{struct _tuple18 _T96;
# 1822
_T96.f0=amin;_T96.f1=Cyc_AssnDef_false_assnmap();_T3=_T96;}return _T3;case 1:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;_T94=_T96->f2;}{struct Cyc_Absyn_Vardecl*vd=_T95;struct Cyc_Absyn_Pat*p2=_T94;
# 1824
struct _tuple18 _T96=Cyc_Vcgen_vcgen_pat(env,p2,lvalue,v,amin);struct Cyc_AssnDef_AssnMap _T97;struct Cyc_AssnDef_AssnMap _T98;_T98=_T96.f0;_T97=_T96.f1;{struct Cyc_AssnDef_AssnMap amt=_T98;struct Cyc_AssnDef_AssnMap amf=_T97;{struct _tuple18 _T99;_T5=env;_T6=amt;_T7=
Cyc_Vcgen_myaddr(env,vd);_T8=v;_T99.f0=Cyc_Vcgen_do_assign(_T5,_T6,_T7,_T8);_T99.f1=amf;_T4=_T99;}return _T4;}}case 2:{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;_T93=_T96->f2;}{struct Cyc_Absyn_Tvar*tv=_T95;struct Cyc_Absyn_Vardecl*vd=_T93;{struct _tuple18 _T96;_TA=env;_TB=amin;_TC=
# 1827
Cyc_Vcgen_myaddr(env,vd);_TD=v;_T96.f0=Cyc_Vcgen_do_assign(_TA,_TB,_TC,_TD);_T96.f1=Cyc_AssnDef_false_assnmap();_T9=_T96;}return _T9;}case 3:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;_T94=_T96->f2;}{struct Cyc_Absyn_Vardecl*vd=_T95;struct Cyc_Absyn_Pat*p2=_T94;
# 1829
struct _tuple18 _T96=Cyc_Vcgen_vcgen_pat(env,p2,lvalue,v,amin);struct Cyc_AssnDef_AssnMap _T97;struct Cyc_AssnDef_AssnMap _T98;_T98=_T96.f0;_T97=_T96.f1;{struct Cyc_AssnDef_AssnMap amt=_T98;struct Cyc_AssnDef_AssnMap amf=_T97;
if(lvalue!=0)goto _TL1A3;_TF=Cyc_Warn_impos;{int(*_T99)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF;_TE=_T99;}_T10=_tag_fat("vcgen reference pattern matching",sizeof(char),33U);_T11=_tag_fat(0U,sizeof(void*),0);_TE(_T10,_T11);goto _TL1A4;_TL1A3: _TL1A4:{struct _tuple18 _T99;_T13=env;_T14=amt;_T15=
Cyc_Vcgen_myaddr(env,vd);_T16=lvalue;_T99.f0=Cyc_Vcgen_do_assign(_T13,_T14,_T15,_T16);_T99.f1=amf;_T12=_T99;}return _T12;}}case 4:{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f2;}{struct Cyc_Absyn_Vardecl*vd=_T95;{struct _tuple18 _T96;_T18=env;_T19=amin;_T1A=
# 1833
Cyc_Vcgen_myaddr(env,vd);_T1B=v;_T96.f0=Cyc_Vcgen_do_assign(_T18,_T19,_T1A,_T1B);_T96.f1=Cyc_AssnDef_false_assnmap();_T17=_T96;}return _T17;}case 8:  {
# 1835
void*t=Cyc_AssnDef_zero();{struct _tuple18 _T96;_T1D=amin;_T1E=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T1D,_T1E);_T1F=amin;_T20=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T1F,_T20);_T1C=_T96;}return _T1C;}case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T8A;_T92=_T96->f1;_T91=_T96->f2;}{enum Cyc_Absyn_Sign s=_T92;int i=_T91;_T21=i;_T22=(unsigned)_T21;{
# 1838
void*t=Cyc_AssnDef_uint(_T22);{struct _tuple18 _T96;_T24=amin;_T25=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T24,_T25);_T26=amin;_T27=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T26,_T27);_T23=_T96;}return _T23;}}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T8A;_T90=_T96->f1;}{char c=_T90;_T28=c;_T29=(unsigned)_T28;{
# 1841
void*t=Cyc_AssnDef_uint(_T29);{struct _tuple18 _T96;_T2B=amin;_T2C=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T2B,_T2C);_T2D=amin;_T2E=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T2D,_T2E);_T2A=_T96;}return _T2A;}}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T8A;_T8F=_T96->f1;_T91=_T96->f2;}{struct _fat_ptr f=_T8F;int i=_T91;
# 1844
struct Cyc_Absyn_Exp*e=Cyc_Absyn_float_exp(f,i,0U);_T2F=e;_T30=i;_T31=(unsigned)_T30;
_T2F->topt=Cyc_Absyn_gen_float_type(_T31);{
void*t=Cyc_AssnDef_cnst(e);{struct _tuple18 _T96;_T33=amin;_T34=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T33,_T34);_T35=amin;_T36=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T35,_T36);_T32=_T96;}return _T32;}}case 12:{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;_T8E=_T96->f2;}{struct Cyc_Absyn_Enumdecl*ed=_T95;struct Cyc_Absyn_Enumfield*ef=_T8E;{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T96=_cycalloc(sizeof(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct));_T96->tag=31;
# 1849
_T96->f1=ed;_T96->f2=ef;_T37=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_T96;}_T38=(void*)_T37;{struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp(_T38,0U);_T39=e;_T3A=p;
_T39->topt=_T3A->topt;{
void*t=Cyc_AssnDef_cnst(e);{struct _tuple18 _T96;_T3C=amin;_T3D=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T3C,_T3D);_T3E=amin;_T3F=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T3E,_T3F);_T3B=_T96;}return _T3B;}}}case 13:{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_T8A;_T40=_T96->f1;_T95=(void*)_T40;_T8E=_T96->f2;}{void*type=_T95;struct Cyc_Absyn_Enumfield*ef=_T8E;{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T96=_cycalloc(sizeof(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct));_T96->tag=32;
# 1854
_T96->f1=type;_T96->f2=ef;_T41=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_T96;}_T42=(void*)_T41;{struct Cyc_Absyn_Exp*e=Cyc_Absyn_new_exp(_T42,0U);_T43=e;_T44=p;
_T43->topt=_T44->topt;{
void*t=Cyc_AssnDef_cnst(e);{struct _tuple18 _T96;_T46=amin;_T47=
Cyc_AssnDef_eq(v,t);_T96.f0=Cyc_AssnDef_and_assnmap_assn(_T46,_T47);_T48=amin;_T49=Cyc_AssnDef_neq(v,t);_T96.f1=Cyc_AssnDef_and_assnmap_assn(_T48,_T49);_T45=_T96;}return _T45;}}}case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T8A;_T95=_T96->f1;}{struct Cyc_Absyn_Pat*p2=_T95;
# 1860
struct _tuple14 _T96=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,amin);struct Cyc_AssnDef_AssnMap _T97;void*_T98;_T98=_T96.f0;_T97=_T96.f1;{void*mem=_T98;struct Cyc_AssnDef_AssnMap amin=_T97;_T4A=mem;_T4B=v;_T4C=p;_T4D=_T4C->topt;{
void*v2=Cyc_AssnDef_select(_T4A,_T4B,_T4D);
struct _tuple18 _T99=Cyc_Vcgen_vcgen_pat(env,p2,v,v2,amin);struct Cyc_AssnDef_AssnMap _T9A;struct Cyc_AssnDef_AssnMap _T9B;_T9B=_T99.f0;_T9A=_T99.f1;{struct Cyc_AssnDef_AssnMap amt=_T9B;struct Cyc_AssnDef_AssnMap amf=_T9A;{struct _tuple18 _T9C;_T4F=amt;_T50=
# 1864
Cyc_AssnDef_zero();_T51=v;_T52=Cyc_AssnDef_neq(_T50,_T51);_T9C.f0=Cyc_AssnDef_and_assnmap_assn(_T4F,_T52);_T53=amf;_T54=Cyc_AssnDef_zero();_T55=v;_T56=Cyc_AssnDef_eq(_T54,_T55);_T9C.f1=Cyc_AssnDef_or_assnmap_assn(_T53,_T56);_T4E=_T9C;}return _T4E;}}}}case 6:{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T8A;_T57=_T96->f1;_T95=(void*)_T57;_T8D=_T96->f4;}{void*topt=_T95;struct Cyc_List_List*dlps=_T8D;
# 1866
void*type=_check_null(topt);
struct _tuple20 _T96=Cyc_Vcgen_get_aggr_info(type);struct Cyc_List_List*_T97;int _T98;_T98=_T96.f0;_T97=_T96.f2;{int is_tagged=_T98;struct Cyc_List_List*fields=_T97;
struct Cyc_AssnDef_AssnMap amt=amin;
struct Cyc_AssnDef_AssnMap amf=Cyc_AssnDef_false_assnmap();
_TL1A8: if(dlps!=0)goto _TL1A6;else{goto _TL1A7;}
_TL1A6: _T58=dlps;_T59=_T58->hd;{struct _tuple25*tup=(struct _tuple25*)_T59;_T5A=tup;{
struct Cyc_Absyn_Pat*p2=_T5A->f1;_T5B=p2;_T5C=_T5B->r;_T5D=& Cyc_Absyn_Wild_p_val;_T5E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T5D;_T5F=(void*)_T5E;
if(_T5C!=_T5F)goto _TL1A9;goto _TL1A5;_TL1A9: _T60=tup;_T61=_T60->f0;{
struct _fat_ptr*f=Cyc_Absyn_designatorlist_to_fieldname(_T61);
int i=Cyc_CfFlowInfo_get_field_index(type,f);_T62=is_tagged;
if(!_T62)goto _TL1AB;_T63=
# 1879
Cyc_AssnDef_tagof_tm(v);_T64=i;_T65=(unsigned)_T64;_T66=Cyc_AssnDef_uint(_T65);{void*tag_check=Cyc_AssnDef_eq(_T63,_T66);
amt=Cyc_AssnDef_and_assnmap_assn(amt,tag_check);_T67=amf;_T68=
Cyc_AssnDef_not(tag_check);amf=Cyc_AssnDef_or_assnmap_assn(_T67,_T68);}goto _TL1AC;_TL1AB: _TL1AC: _T69=v;_T6A=i;_T6B=(unsigned)_T6A;_T6C=p2;_T6D=_T6C->topt;{
# 1883
void*member=Cyc_AssnDef_proj(_T69,_T6B,_T6D);
void*path=0;
if(lvalue==0)goto _TL1AD;_T6E=lvalue;_T6F=i;_T70=(unsigned)_T6F;
path=Cyc_AssnDef_offsetf(_T6E,_T70,0);goto _TL1AE;_TL1AD: _TL1AE: {
struct _tuple18 _T99=Cyc_Vcgen_vcgen_pat(env,p2,path,member,amt);struct Cyc_AssnDef_AssnMap _T9A;struct Cyc_AssnDef_AssnMap _T9B;_T9B=_T99.f0;_T9A=_T99.f1;{struct Cyc_AssnDef_AssnMap amt2=_T9B;struct Cyc_AssnDef_AssnMap amf2=_T9A;_T71=env;_T72=
Cyc_AssnDef_or_assnmap_assnmap(amf,amf2);amf=Cyc_Vcgen_may_widen(_T71,_T72);
amt=amt2;}}}}}}_TL1A5: _T73=dlps;
# 1870
dlps=_T73->tl;goto _TL1A8;_TL1A7:{struct _tuple18 _T99;
# 1891
_T99.f0=amt;_T99.f1=amf;_T74=_T99;}return _T74;}}case 7:{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_T96=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_T8A;_T8C=_T96->f1;_T8B=_T96->f2;_T8D=_T96->f3;}{struct Cyc_Absyn_Datatypedecl*dd=_T8C;struct Cyc_Absyn_Datatypefield*df=_T8B;struct Cyc_List_List*ps=_T8D;_T75=
# 1895
Cyc_Vcgen_datatype_constructor_index(dd,df);{int i=(int)_T75;_T76=
Cyc_AssnDef_tagof_tm(v);_T77=i;_T78=(unsigned)_T77;_T79=Cyc_AssnDef_uint(_T78);{void*tag_check=Cyc_AssnDef_eq(_T76,_T79);
struct Cyc_AssnDef_AssnMap amt=Cyc_AssnDef_and_assnmap_assn(amin,tag_check);
struct Cyc_AssnDef_AssnMap amf=Cyc_AssnDef_false_assnmap();{
unsigned j=0U;_TL1B2: if(ps!=0)goto _TL1B0;else{goto _TL1B1;}
_TL1B0: _T7A=ps;_T7B=_T7A->hd;{struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)_T7B;_T7C=v;_T7D=j;_T7E=p2;_T7F=_T7E->topt;{
void*member=Cyc_AssnDef_proj(_T7C,_T7D,_T7F);
void*path=0;
if(lvalue==0)goto _TL1B3;
path=Cyc_AssnDef_offsetf(lvalue,j,0);goto _TL1B4;_TL1B3: _TL1B4: {
# 1906
struct _tuple18 _T96=Cyc_Vcgen_vcgen_pat(env,p2,path,member,amt);struct Cyc_AssnDef_AssnMap _T97;struct Cyc_AssnDef_AssnMap _T98;_T98=_T96.f0;_T97=_T96.f1;{struct Cyc_AssnDef_AssnMap amt2=_T98;struct Cyc_AssnDef_AssnMap amf2=_T97;_T80=env;_T81=
Cyc_AssnDef_or_assnmap_assnmap(amf,amf2);amf=Cyc_Vcgen_may_widen(_T80,_T81);
amt=amt2;}}}}_T82=ps;
# 1899
ps=_T82->tl;j=j + 1;goto _TL1B2;_TL1B1:;}_T83=amf;_T84=
# 1910
Cyc_AssnDef_not(tag_check);amf=Cyc_AssnDef_or_assnmap_assn(_T83,_T84);{struct _tuple18 _T96;
_T96.f0=amt;_T96.f1=amf;_T85=_T96;}return _T85;}}}case 14: goto _LL20;case 15: _LL20: goto _LL22;default: _LL22: _T87=Cyc_Warn_impos;{
# 1914
int(*_T96)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T87;_T86=_T96;}_T88=_tag_fat("vcgen pat found bad pattern",sizeof(char),28U);_T89=_tag_fat(0U,sizeof(void*),0);_T86(_T88,_T89);};}}
# 1918
static struct Cyc_AssnDef_AssnMap Cyc_Vcgen_vcgen_switch(struct Cyc_Vcgen_Env*env,void*v,struct Cyc_List_List*scs,struct Cyc_AssnDef_AssnMap amin){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Vcgen_Env*_T2;struct Cyc_Absyn_Switch_clause*_T3;struct Cyc_Absyn_Pat*_T4;void*_T5;struct Cyc_AssnDef_AssnMap _T6;struct Cyc_Absyn_Switch_clause*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Vcgen_Env*_T9;struct Cyc_Absyn_Switch_clause*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_AssnDef_AssnMap _TC;struct Cyc_Vcgen_Env*_TD;struct Cyc_AssnDef_AssnMap _TE;struct Cyc_Vcgen_Env*_TF;struct Cyc_AssnDef_AssnMap _T10;struct Cyc_Vcgen_Env*_T11;struct Cyc_Absyn_Switch_clause*_T12;struct Cyc_Absyn_Stmt*_T13;struct Cyc_AssnDef_AssnMap _T14;struct Cyc_AssnDef_AssnMap _T15;struct Cyc_AssnDef_AssnMap _T16;struct Cyc_Vcgen_Env*_T17;int _T18;struct Cyc_List_List*_T19;struct Cyc_AssnDef_AssnMap _T1A;
# 1923
struct Cyc_AssnDef_AssnMap res=Cyc_AssnDef_false_assnmap();
_TL1B8: if(scs!=0)goto _TL1B6;else{goto _TL1B7;}
_TL1B6: _T0=scs;_T1=_T0->hd;{struct Cyc_Absyn_Switch_clause*sc=(struct Cyc_Absyn_Switch_clause*)_T1;_T2=env;_T3=sc;_T4=_T3->pattern;_T5=v;_T6=amin;{
struct _tuple18 _T1B=Cyc_Vcgen_vcgen_pat(_T2,_T4,0,_T5,_T6);struct Cyc_AssnDef_AssnMap _T1C;struct Cyc_AssnDef_AssnMap _T1D;_T1D=_T1B.f0;_T1C=_T1B.f1;{struct Cyc_AssnDef_AssnMap amt=_T1D;struct Cyc_AssnDef_AssnMap amf=_T1C;_T7=sc;_T8=_T7->where_clause;
if(_T8==0)goto _TL1B9;_T9=env;_TA=sc;_TB=_TA->where_clause;_TC=amt;{
struct _tuple18 _T1E=Cyc_Vcgen_vcgen_test(_T9,_TB,_TC);struct Cyc_AssnDef_AssnMap _T1F;struct Cyc_AssnDef_AssnMap _T20;_T20=_T1E.f0;_T1F=_T1E.f1;{struct Cyc_AssnDef_AssnMap amt2=_T20;struct Cyc_AssnDef_AssnMap amf2=_T1F;
amt=amt2;_TD=env;_TE=
Cyc_AssnDef_or_assnmap_assnmap(amf,amf2);amf=Cyc_Vcgen_may_widen(_TD,_TE);}}goto _TL1BA;_TL1B9: _TL1BA: _TF=env;_T10=res;_T11=env;_T12=sc;_T13=_T12->body;_T14=amt;_T15=
# 1932
Cyc_Vcgen_vcgen_stmt(_T11,_T13,_T14);_T16=Cyc_AssnDef_or_assnmap_assnmap(_T10,_T15);res=Cyc_Vcgen_may_widen(_TF,_T16);_T17=env;_T18=_T17->widen_paths;
if(_T18)goto _TL1BB;else{goto _TL1BD;}
_TL1BD: amin=amf;goto _TL1BC;_TL1BB: _TL1BC:;}}}_T19=scs;
# 1924
scs=_T19->tl;goto _TL1B8;_TL1B7: _T1A=res;
# 1936
return _T1A;}
# 1939
struct Cyc_AssnDef_AssnMap Cyc_Vcgen_clause2assn(struct Cyc_Absyn_Exp*e){struct _tuple18 _T0;struct Cyc_AssnDef_AssnMap _T1;
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_bogus_env();
# 1942
struct Cyc_AssnDef_AssnMap am=Cyc_AssnDef_true_assnmap();
struct _tuple14 _T2=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _T3;_T3=_T2.f1;{struct Cyc_AssnDef_AssnMap am=_T3;_T0=
Cyc_Vcgen_vcgen_test(env,e,am);{struct Cyc_AssnDef_AssnMap res=_T0.f0;_T1=res;
return _T1;}}}
# 2054 "vcgen.cyc"
void Cyc_Vcgen_vcgen_fundecl(unsigned loc,struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_Absyn_Fndecl*fd,struct Cyc_Hashtable_Table**assn_info,struct Cyc_Hashtable_Table*fn_precond_info,int use_precond){struct Cyc_Absyn_Fndecl*_T0;struct Cyc_Absyn_FnInfo*_T1;struct Cyc_Absyn_FnInfo*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_AssnDef_AssnMap _TA;struct Cyc_AssnDef_AssnMap _TB;struct Cyc_AssnDef_AssnMap _TC;struct Cyc_AssnDef_AssnMap _TD;struct Cyc_AssnDef_AssnMap*_TE;struct Cyc_AssnDef_AssnMap*_TF;struct _tuple2*_T10;struct _tuple2*_T11;struct _fat_ptr*_T12;struct _tuple2*_T13;struct Cyc_Absyn_Fndecl*_T14;struct Cyc_Absyn_FnInfo _T15;void*_T16;struct Cyc_Vcgen_Env*_T17;struct Cyc_Absyn_Fndecl*_T18;struct Cyc_Absyn_Stmt*_T19;struct Cyc_AssnDef_AssnMap _T1A;struct Cyc_Vcgen_Env*_T1B;struct Cyc_AssnDef_AssnMap*_T1C;struct Cyc_AssnDef_AssnMap*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_AssnDef_ExistAssnFn*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;struct Cyc_AssnDef_AssnMap _T23;void*_T24;void*_T25;int _T26;struct Cyc_String_pa_PrintArg_struct _T27;struct Cyc_String_pa_PrintArg_struct _T28;struct Cyc_Vcgen_Env*_T29;struct Cyc_AssnDef_AssnMap*_T2A;struct Cyc_AssnDef_AssnMap*_T2B;struct Cyc_AssnDef_AssnMap _T2C;unsigned _T2D;struct _fat_ptr _T2E;struct _fat_ptr _T2F;struct Cyc_Absyn_Fndecl*_T30;struct Cyc_List_List*_T31;void*_T32;void*_T33;struct Cyc_AssnDef_AssnMap _T34;void*_T35;struct Cyc_Vcgen_Env*_T36;struct Cyc_AssnDef_AssnMap*_T37;struct Cyc_AssnDef_AssnMap*_T38;struct Cyc_AssnDef_ExistAssnFn*_T39;struct Cyc_List_List*_T3A;struct Cyc_AssnDef_AssnMap _T3B;void*_T3C;void*_T3D;int _T3E;struct Cyc_String_pa_PrintArg_struct _T3F;struct Cyc_String_pa_PrintArg_struct _T40;struct Cyc_Vcgen_Env*_T41;struct Cyc_AssnDef_AssnMap*_T42;struct Cyc_AssnDef_AssnMap*_T43;struct Cyc_AssnDef_AssnMap _T44;unsigned _T45;struct _fat_ptr _T46;struct _fat_ptr _T47;_T0=fd;_T1=& _T0->i;_T2=(struct Cyc_Absyn_FnInfo*)_T1;{
# 2060
struct _tuple19 _T48=Cyc_Vcgen_get_requires_and_ensures_info(_T2);struct Cyc_Absyn_Vardecl*_T49;struct Cyc_AssnDef_ExistAssnFn*_T4A;struct Cyc_AssnDef_ExistAssnFn*_T4B;struct Cyc_AssnDef_ExistAssnFn*_T4C;struct Cyc_AssnDef_ExistAssnFn*_T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;_T4F=_T48.f0;_T4E=_T48.f1;_T4D=_T48.f2;_T4C=_T48.f3;_T4B=_T48.f4;_T4A=_T48.f5;_T49=_T48.f6;{struct Cyc_List_List*args=_T4F;struct Cyc_List_List*arvds=_T4E;struct Cyc_AssnDef_ExistAssnFn*checkopt=_T4D;struct Cyc_AssnDef_ExistAssnFn*reqopt=_T4C;struct Cyc_AssnDef_ExistAssnFn*ensopt=_T4B;struct Cyc_AssnDef_ExistAssnFn*thrwsopt=_T4A;struct Cyc_Absyn_Vardecl*ret_value=_T49;
struct Cyc_AssnDef_AssnMap am=Cyc_AssnDef_true_assnmap();
# 2066
struct Cyc_List_List*actuals=0;{
struct Cyc_List_List*as=arvds;_TL1C1: if(as!=0)goto _TL1BF;else{goto _TL1C0;}
_TL1BF: _T3=as;_T4=_T3->hd;_T5=(struct Cyc_Absyn_Vardecl*)_T4;{struct Cyc_Absyn_Vardecl*vd=_check_null(_T5);
struct _tuple14 _T50=Cyc_AssnDef_lookup_var_map(vd,am);struct Cyc_AssnDef_AssnMap _T51;void*_T52;_T52=_T50.f0;_T51=_T50.f1;{void*actual=_T52;struct Cyc_AssnDef_AssnMap am2=_T51;
am=am2;{struct Cyc_List_List*_T53=_cycalloc(sizeof(struct Cyc_List_List));
_T53->hd=actual;_T53->tl=actuals;_T6=(struct Cyc_List_List*)_T53;}actuals=_T6;}}_T7=as;
# 2067
as=_T7->tl;goto _TL1C1;_TL1C0:;}
# 2073
actuals=Cyc_List_imp_rev(actuals);{
# 2075
struct _tuple14 _T50=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,am);struct Cyc_AssnDef_AssnMap _T51;void*_T52;_T52=_T50.f0;_T51=_T50.f1;{void*initmem=_T52;struct Cyc_AssnDef_AssnMap am2=_T51;
struct Cyc_AssnDef_AssnMap _T53=Cyc_AssnDef_update_var_map(Cyc_AssnDef_pre_memory,initmem,am2);{struct Cyc_AssnDef_AssnMap am2=_T53;
am=am2;{
# 2079
struct Cyc_List_List*args;args=_cycalloc(sizeof(struct Cyc_List_List));_T8=args;_T8->hd=initmem;_T9=args;_T9->tl=actuals;
# 2081
if(checkopt==0)goto _TL1C2;{struct Cyc_AssnDef_AssnMap _T54;
_T54.assn=Cyc_AssnDef_existassnfn2assn(checkopt,args);_TB=am;_T54.map=_TB.map;_TA=_T54;}am=_TA;goto _TL1C3;_TL1C2: _TL1C3:
# 2085
 if(reqopt==0)goto _TL1C4;{struct Cyc_AssnDef_AssnMap _T54;
_T54.assn=Cyc_AssnDef_existassnfn2assn(reqopt,args);_TD=am;_T54.map=_TD.map;_TC=_T54;}am=_TC;goto _TL1C5;_TL1C4: _TL1C5: {
# 2113 "vcgen.cyc"
struct Cyc_AssnDef_AssnMap*thrws_assn=0;
if(thrwsopt==0)goto _TL1C6;{struct Cyc_AssnDef_AssnMap*_T54=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));
*_T54=Cyc_AssnDef_false_assnmap();_TE=(struct Cyc_AssnDef_AssnMap*)_T54;}thrws_assn=_TE;goto _TL1C7;_TL1C6: _TL1C7: {
# 2118
struct Cyc_AssnDef_AssnMap*result_assn;result_assn=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));_TF=result_assn;*_TF=Cyc_AssnDef_false_assnmap();
if(ret_value!=0)goto _TL1C8;{
struct _tuple2*v;v=_cycalloc(sizeof(struct _tuple2));_T10=v;_T10->f0=Cyc_Absyn_Loc_n();_T11=v;{struct _fat_ptr*_T54=_cycalloc(sizeof(struct _fat_ptr));*_T54=_tag_fat("return_value",sizeof(char),13U);_T12=(struct _fat_ptr*)_T54;}_T11->f1=_T12;_T13=v;_T14=fd;_T15=_T14->i;_T16=_T15.ret_type;
ret_value=Cyc_Absyn_new_vardecl(0U,_T13,_T16,0,0);}goto _TL1C9;_TL1C8: _TL1C9: {
# 2123
struct Cyc_Vcgen_Env*env=Cyc_Vcgen_initial_env(tables,fd,assn_info,fn_precond_info,result_assn,thrws_assn,ret_value);_T17=env;_T18=fd;_T19=_T18->body;_T1A=am;{
# 2126
struct Cyc_AssnDef_AssnMap amout=Cyc_Vcgen_vcgen_stmt(_T17,_T19,_T1A);
# 2129
Cyc_Vcgen_update_return(env,0,amout,0);_T1B=env;_T1C=_T1B->res_assnmap;_T1D=
# 2132
_check_null(_T1C);{struct Cyc_AssnDef_AssnMap res=*_T1D;
void*res_term=Cyc_AssnDef_fresh_var(0,0);
if(ret_value==0)goto _TL1CA;{
struct _tuple14 _T54=Cyc_AssnDef_lookup_var_map(ret_value,res);struct Cyc_AssnDef_AssnMap _T55;void*_T56;_T56=_T54.f0;_T55=_T54.f1;{void*r=_T56;struct Cyc_AssnDef_AssnMap a=_T55;
res_term=r;
res=a;}}goto _TL1CB;_TL1CA: _TL1CB: {
# 2139
struct _tuple14 _T54=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,res);struct Cyc_AssnDef_AssnMap _T55;void*_T56;_T56=_T54.f0;_T55=_T54.f1;{void*new_memory=_T56;struct Cyc_AssnDef_AssnMap res=_T55;
struct _tuple14 _T57=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_pre_memory,res);struct Cyc_AssnDef_AssnMap _T58;void*_T59;_T59=_T57.f0;_T58=_T57.f1;{void*pre_mem=_T59;struct Cyc_AssnDef_AssnMap res=_T58;
struct Cyc_List_List*pre_memory_and_actuals;pre_memory_and_actuals=_cycalloc(sizeof(struct Cyc_List_List));_T1E=pre_memory_and_actuals;_T1E->hd=pre_mem;_T1F=pre_memory_and_actuals;_T1F->tl=actuals;
if(ensopt==0)goto _TL1CC;_T20=ensopt;{struct Cyc_List_List*_T5A=_cycalloc(sizeof(struct Cyc_List_List));
_T5A->hd=res_term;{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));_T5B->hd=new_memory;_T5B->tl=pre_memory_and_actuals;_T22=(struct Cyc_List_List*)_T5B;}_T5A->tl=_T22;_T21=(struct Cyc_List_List*)_T5A;}{void*ens=Cyc_AssnDef_existassnfn2assn(_T20,_T21);_T23=res;_T24=_T23.assn;_T25=ens;_T26=
Cyc_Vcgen_implies(_T24,_T25);if(_T26)goto _TL1CE;else{goto _TL1D0;}
_TL1D0:{struct Cyc_String_pa_PrintArg_struct _T5A;_T5A.tag=0;
_T5A.f1=Cyc_AssnDef_assn2string(ens);_T27=_T5A;}{struct Cyc_String_pa_PrintArg_struct _T5A=_T27;{struct Cyc_String_pa_PrintArg_struct _T5B;_T5B.tag=0;_T29=env;_T2A=_T29->res_assnmap;_T2B=
_check_null(_T2A);_T2C=*_T2B;_T5B.f1=Cyc_AssnDef_assnmap2string(_T2C);_T28=_T5B;}{struct Cyc_String_pa_PrintArg_struct _T5B=_T28;void*_T5C[2];_T5C[0]=& _T5A;_T5C[1]=& _T5B;_T2D=loc;_T2E=
# 2145
_tag_fat("cannot prove the @ensures clause\n\t%s\nfrom\n\t%s",sizeof(char),46U);_T2F=_tag_fat(_T5C,sizeof(void*),2);Cyc_Warn_err(_T2D,_T2E,_T2F);}}goto _TL1CF;_TL1CE: _TL1CF:;}goto _TL1CD;
# 2151
_TL1CC: _T30=fd;_T31=pre_memory_and_actuals;_T32=res_term;_T33=new_memory;_T34=res;_T35=_T34.assn;Cyc_Vcgen_insert_fn_info(_T30,_T31,_T32,_T33,_T35);_TL1CD:
# 2168 "vcgen.cyc"
 if(thrwsopt==0)goto _TL1D1;_T36=env;_T37=_T36->try_assnmap;_T38=
_check_null(_T37);{struct Cyc_AssnDef_AssnMap res=*_T38;
# 2171
struct _tuple14 _T5A=Cyc_AssnDef_lookup_var_map(Cyc_AssnDef_memory,res);struct Cyc_AssnDef_AssnMap _T5B;void*_T5C;_T5C=_T5A.f0;_T5B=_T5A.f1;{void*new_memory=_T5C;struct Cyc_AssnDef_AssnMap res=_T5B;_T39=thrwsopt;{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));
_T5D->hd=new_memory;_T5D->tl=pre_memory_and_actuals;_T3A=(struct Cyc_List_List*)_T5D;}{void*thr=Cyc_AssnDef_existassnfn2assn(_T39,_T3A);_T3B=res;_T3C=_T3B.assn;_T3D=thr;_T3E=
Cyc_Vcgen_implies(_T3C,_T3D);if(_T3E)goto _TL1D3;else{goto _TL1D5;}
_TL1D5:{struct Cyc_String_pa_PrintArg_struct _T5D;_T5D.tag=0;
_T5D.f1=Cyc_AssnDef_assn2string(thr);_T3F=_T5D;}{struct Cyc_String_pa_PrintArg_struct _T5D=_T3F;{struct Cyc_String_pa_PrintArg_struct _T5E;_T5E.tag=0;_T41=env;_T42=_T41->try_assnmap;_T43=
_check_null(_T42);_T44=*_T43;_T5E.f1=Cyc_AssnDef_assnmap2string(_T44);_T40=_T5E;}{struct Cyc_String_pa_PrintArg_struct _T5E=_T40;void*_T5F[2];_T5F[0]=& _T5D;_T5F[1]=& _T5E;_T45=loc;_T46=
# 2174
_tag_fat("cannot prove the @throws clause\n\t%s\nfrom\n\t%s",sizeof(char),45U);_T47=_tag_fat(_T5F,sizeof(void*),2);Cyc_Warn_err(_T45,_T46,_T47);}}goto _TL1D4;_TL1D3: _TL1D4:;}}}goto _TL1D2;_TL1D1: _TL1D2:;}}}}}}}}}}}}}}}

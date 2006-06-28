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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 210
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 38 "position.h"
extern struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
# 48
extern int Cyc_Position_use_gcc_style_location;struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 533
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 551
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 565
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 920 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 928
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
# 935
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 941
void*Cyc_Absyn_compress(void*);
# 956
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 961
extern void*Cyc_Absyn_sint_type;
# 970
extern void*Cyc_Absyn_al_qual_type;
# 974
extern void*Cyc_Absyn_false_type;
# 976
extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);
# 1004
void*Cyc_Absyn_string_type(void*,void*);
# 1007
extern void*Cyc_Absyn_fat_bound_type;
# 1029
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1237
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 69 "tcutil.h"
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 82
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 253 "tcutil.h"
int Cyc_Tcutil_force_type2bool(int,void*);
# 256
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 48 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 67
extern int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*,void*,void**);struct Cyc_Port_Edit{unsigned loc;struct _fat_ptr old_string;struct _fat_ptr new_string;};
# 90 "port.cyc"
int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){struct Cyc_Port_Edit*_T0;int _T1;struct Cyc_Port_Edit*_T2;int _T3;int _T4;_T0=e1;_T1=(int)_T0;_T2=e2;_T3=(int)_T2;_T4=_T1 - _T3;
return _T4;}
# 93
static unsigned Cyc_Port_get_seg(struct Cyc_Port_Edit*e){struct Cyc_Port_Edit*_T0;unsigned _T1;_T0=e;_T1=_T0->loc;
return _T1;}
# 96
int Cyc_Port_cmp_seg_t(unsigned loc1,unsigned loc2){unsigned _T0;int _T1;_T0=loc1 - loc2;_T1=(int)_T0;
return _T1;}
# 99
int Cyc_Port_hash_seg_t(unsigned loc){unsigned _T0;int _T1;_T0=loc;_T1=(int)_T0;
return _T1;}struct Cyc_Port_VarUsage{int address_taken;struct Cyc_Absyn_Exp*init;struct Cyc_List_List*usage_locs;};struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};struct Cyc_Port_Cfield{void*qual;struct _fat_ptr*f;void*type;};struct Cyc_Port_Const_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;};struct _tuple11{struct Cyc_Absyn_Aggrdecl*f0;struct Cyc_List_List*f1;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;struct _tuple11*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Var_ct_Port_Ctype_struct{int tag;struct Cyc_Port_Cvar*f1;};
# 155
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0};
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1};
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2};
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3};
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4};
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5};
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6};
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7};
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8};
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9};
# 168
static struct _fat_ptr Cyc_Port_ctypes2string(int,struct Cyc_List_List*);
static struct _fat_ptr Cyc_Port_cfields2string(int,struct Cyc_List_List*);
static struct _fat_ptr Cyc_Port_ctype2string(int deep,void*t){void*_T0;int*_T1;unsigned _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;void*_TD;struct _fat_ptr _TE;struct Cyc_String_pa_PrintArg_struct _TF;struct _fat_ptr*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;void*_T18;struct _fat_ptr _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct Cyc_String_pa_PrintArg_struct _T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc_String_pa_PrintArg_struct _T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;void*_T21;void*_T22;void*_T23;void*_T24;struct _fat_ptr _T25;struct Cyc_String_pa_PrintArg_struct _T26;struct Cyc_String_pa_PrintArg_struct _T27;struct Cyc_String_pa_PrintArg_struct _T28;struct _fat_ptr _T29;struct _fat_ptr _T2A;void*_T2B;struct _tuple11*_T2C;struct _fat_ptr _T2D;struct Cyc_Absyn_Aggrdecl*_T2E;enum Cyc_Absyn_AggrKind _T2F;int _T30;int _T31;struct _fat_ptr _T32;struct Cyc_String_pa_PrintArg_struct _T33;struct Cyc_String_pa_PrintArg_struct _T34;struct Cyc_Absyn_Aggrdecl*_T35;struct _tuple0*_T36;struct _fat_ptr _T37;struct _fat_ptr _T38;struct _fat_ptr _T39;struct Cyc_String_pa_PrintArg_struct _T3A;struct Cyc_String_pa_PrintArg_struct _T3B;struct Cyc_Absyn_Aggrdecl*_T3C;struct _tuple0*_T3D;struct Cyc_String_pa_PrintArg_struct _T3E;struct _fat_ptr _T3F;struct _fat_ptr _T40;void*_T41;struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T42;void**_T43;void*_T44;void**_T45;struct _fat_ptr _T46;void*_T47;struct _fat_ptr _T48;struct Cyc_String_pa_PrintArg_struct _T49;struct _fat_ptr _T4A;struct _fat_ptr _T4B;void*_T4C;void*_T4D;struct _fat_ptr _T4E;struct Cyc_String_pa_PrintArg_struct _T4F;struct Cyc_String_pa_PrintArg_struct _T50;struct _fat_ptr _T51;struct _fat_ptr _T52;void*_T53;struct Cyc_Port_Cvar*_T54;void**_T55;unsigned _T56;int _T57;struct Cyc_Port_Cvar*_T58;void**_T59;void*_T5A;struct _fat_ptr _T5B;int _T5C;struct Cyc_Port_Cvar*_T5D;struct Cyc_List_List*_T5E;struct Cyc_Port_Cvar*_T5F;struct Cyc_List_List*_T60;struct _fat_ptr _T61;struct Cyc_Int_pa_PrintArg_struct _T62;struct Cyc_Port_Cvar*_T63;int _T64;struct _fat_ptr _T65;struct _fat_ptr _T66;struct _fat_ptr _T67;struct Cyc_String_pa_PrintArg_struct _T68;struct Cyc_Port_Cvar*_T69;struct Cyc_List_List*_T6A;struct Cyc_Int_pa_PrintArg_struct _T6B;struct Cyc_Port_Cvar*_T6C;int _T6D;struct Cyc_String_pa_PrintArg_struct _T6E;struct Cyc_Port_Cvar*_T6F;struct Cyc_List_List*_T70;struct _fat_ptr _T71;struct _fat_ptr _T72;struct Cyc_Port_Cvar*_T73;struct Cyc_List_List*_T74;void*_T75;void*_T76;void*_T77;void*_T78;void*_T79;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=
# 172
_tag_fat("const",sizeof(char),6U);return _T3;case 1: _T4=
_tag_fat("notconst",sizeof(char),9U);return _T4;case 2: _T5=
_tag_fat("thin",sizeof(char),5U);return _T5;case 3: _T6=
_tag_fat("fat",sizeof(char),4U);return _T6;case 4: _T7=
_tag_fat("void",sizeof(char),5U);return _T7;case 5: _T8=
_tag_fat("zero",sizeof(char),5U);return _T8;case 6: _T9=
_tag_fat("arith",sizeof(char),6U);return _T9;case 7: _TA=
_tag_fat("heap",sizeof(char),5U);return _TA;case 8: _TB=
_tag_fat("ZT",sizeof(char),3U);return _TB;case 9: _TC=
_tag_fat("NZT",sizeof(char),4U);return _TC;case 10: _TD=t;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T7A=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_TD;_T79=_T7A->f1;}{struct _fat_ptr*n=_T79;{struct Cyc_String_pa_PrintArg_struct _T7A;_T7A.tag=0;_T10=n;
_T7A.f1=*_T10;_TF=_T7A;}{struct Cyc_String_pa_PrintArg_struct _T7A=_TF;void*_T7B[1];_T7B[0]=& _T7A;_T11=_tag_fat("%s",sizeof(char),3U);_T12=_tag_fat(_T7B,sizeof(void*),1);_TE=Cyc_aprintf(_T11,_T12);}return _TE;}case 11: _T13=t;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T7A=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T13;_T14=_T7A->f1;_T79=(void*)_T14;_T15=_T7A->f2;_T78=(void*)_T15;_T16=_T7A->f3;_T77=(void*)_T16;_T17=_T7A->f4;_T76=(void*)_T17;_T18=_T7A->f5;_T75=(void*)_T18;}{void*elt=_T79;void*qual=_T78;void*k=_T77;void*rgn=_T76;void*zt=_T75;{struct Cyc_String_pa_PrintArg_struct _T7A;_T7A.tag=0;
# 184
_T7A.f1=Cyc_Port_ctype2string(deep,elt);_T1A=_T7A;}{struct Cyc_String_pa_PrintArg_struct _T7A=_T1A;{struct Cyc_String_pa_PrintArg_struct _T7B;_T7B.tag=0;
_T7B.f1=Cyc_Port_ctype2string(deep,qual);_T1B=_T7B;}{struct Cyc_String_pa_PrintArg_struct _T7B=_T1B;{struct Cyc_String_pa_PrintArg_struct _T7C;_T7C.tag=0;_T7C.f1=Cyc_Port_ctype2string(deep,k);_T1C=_T7C;}{struct Cyc_String_pa_PrintArg_struct _T7C=_T1C;{struct Cyc_String_pa_PrintArg_struct _T7D;_T7D.tag=0;
_T7D.f1=Cyc_Port_ctype2string(deep,rgn);_T1D=_T7D;}{struct Cyc_String_pa_PrintArg_struct _T7D=_T1D;{struct Cyc_String_pa_PrintArg_struct _T7E;_T7E.tag=0;_T7E.f1=Cyc_Port_ctype2string(deep,zt);_T1E=_T7E;}{struct Cyc_String_pa_PrintArg_struct _T7E=_T1E;void*_T7F[5];_T7F[0]=& _T7A;_T7F[1]=& _T7B;_T7F[2]=& _T7C;_T7F[3]=& _T7D;_T7F[4]=& _T7E;_T1F=
# 184
_tag_fat("ptr(%s,%s,%s,%s,%s)",sizeof(char),20U);_T20=_tag_fat(_T7F,sizeof(void*),5);_T19=Cyc_aprintf(_T1F,_T20);}}}}}return _T19;}case 12: _T21=t;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T7A=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T21;_T22=_T7A->f1;_T79=(void*)_T22;_T23=_T7A->f2;_T78=(void*)_T23;_T24=_T7A->f3;_T77=(void*)_T24;}{void*elt=_T79;void*qual=_T78;void*zt=_T77;{struct Cyc_String_pa_PrintArg_struct _T7A;_T7A.tag=0;
# 188
_T7A.f1=Cyc_Port_ctype2string(deep,elt);_T26=_T7A;}{struct Cyc_String_pa_PrintArg_struct _T7A=_T26;{struct Cyc_String_pa_PrintArg_struct _T7B;_T7B.tag=0;
_T7B.f1=Cyc_Port_ctype2string(deep,qual);_T27=_T7B;}{struct Cyc_String_pa_PrintArg_struct _T7B=_T27;{struct Cyc_String_pa_PrintArg_struct _T7C;_T7C.tag=0;_T7C.f1=Cyc_Port_ctype2string(deep,zt);_T28=_T7C;}{struct Cyc_String_pa_PrintArg_struct _T7C=_T28;void*_T7D[3];_T7D[0]=& _T7A;_T7D[1]=& _T7B;_T7D[2]=& _T7C;_T29=
# 188
_tag_fat("array(%s,%s,%s)",sizeof(char),16U);_T2A=_tag_fat(_T7D,sizeof(void*),3);_T25=Cyc_aprintf(_T29,_T2A);}}}return _T25;}case 13: _T2B=t;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_T7A=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_T2B;_T2C=_T7A->f1;{struct _tuple11 _T7B=*_T2C;_T79=_T7B.f0;_T78=_T7B.f1;}}{struct Cyc_Absyn_Aggrdecl*ad=_T79;struct Cyc_List_List*cfs=_T78;_T2E=ad;_T2F=_T2E->kind;_T30=(int)_T2F;
# 191
if(_T30!=0)goto _TL1;_T2D=_tag_fat("struct",sizeof(char),7U);goto _TL2;_TL1: _T2D=_tag_fat("union",sizeof(char),6U);_TL2: {struct _fat_ptr s=_T2D;_T31=deep;
if(_T31)goto _TL3;else{goto _TL5;}
_TL5:{struct Cyc_String_pa_PrintArg_struct _T7A;_T7A.tag=0;_T7A.f1=s;_T33=_T7A;}{struct Cyc_String_pa_PrintArg_struct _T7A=_T33;{struct Cyc_String_pa_PrintArg_struct _T7B;_T7B.tag=0;_T35=ad;_T36=_T35->name;_T7B.f1=Cyc_Absynpp_qvar2string(_T36);_T34=_T7B;}{struct Cyc_String_pa_PrintArg_struct _T7B=_T34;void*_T7C[2];_T7C[0]=& _T7A;_T7C[1]=& _T7B;_T37=_tag_fat("%s %s",sizeof(char),6U);_T38=_tag_fat(_T7C,sizeof(void*),2);_T32=Cyc_aprintf(_T37,_T38);}}return _T32;
# 195
_TL3:{struct Cyc_String_pa_PrintArg_struct _T7A;_T7A.tag=0;_T7A.f1=s;_T3A=_T7A;}{struct Cyc_String_pa_PrintArg_struct _T7A=_T3A;{struct Cyc_String_pa_PrintArg_struct _T7B;_T7B.tag=0;_T3C=ad;_T3D=_T3C->name;_T7B.f1=Cyc_Absynpp_qvar2string(_T3D);_T3B=_T7B;}{struct Cyc_String_pa_PrintArg_struct _T7B=_T3B;{struct Cyc_String_pa_PrintArg_struct _T7C;_T7C.tag=0;
_T7C.f1=Cyc_Port_cfields2string(0,cfs);_T3E=_T7C;}{struct Cyc_String_pa_PrintArg_struct _T7C=_T3E;void*_T7D[3];_T7D[0]=& _T7A;_T7D[1]=& _T7B;_T7D[2]=& _T7C;_T3F=
# 195
_tag_fat("%s %s {%s}",sizeof(char),11U);_T40=_tag_fat(_T7D,sizeof(void*),3);_T39=Cyc_aprintf(_T3F,_T40);}}}return _T39;}}case 14: _T41=t;_T42=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T41;_T43=_T42->f2;if(_T43==0)goto _TL6;_T44=t;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T7A=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T44;_T79=_T7A->f1;_T45=_T7A->f2;{void*_T7B=*_T45;_T78=_T7B;}}{struct Cyc_List_List*cfs=_T79;void*eq=_T78;_T46=
# 197
Cyc_Port_ctype2string(deep,eq);return _T46;}_TL6: _T47=t;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T7A=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T47;_T79=_T7A->f1;}{struct Cyc_List_List*cfs=_T79;{struct Cyc_String_pa_PrintArg_struct _T7A;_T7A.tag=0;
# 199
_T7A.f1=Cyc_Port_cfields2string(deep,cfs);_T49=_T7A;}{struct Cyc_String_pa_PrintArg_struct _T7A=_T49;void*_T7B[1];_T7B[0]=& _T7A;_T4A=_tag_fat("aggr {%s}",sizeof(char),10U);_T4B=_tag_fat(_T7B,sizeof(void*),1);_T48=Cyc_aprintf(_T4A,_T4B);}return _T48;}case 15: _T4C=t;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T7A=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T4C;_T4D=_T7A->f1;_T79=(void*)_T4D;_T74=_T7A->f2;}{void*t=_T79;struct Cyc_List_List*ts=_T74;{struct Cyc_String_pa_PrintArg_struct _T7A;_T7A.tag=0;
# 201
_T7A.f1=Cyc_Port_ctypes2string(deep,ts);_T4F=_T7A;}{struct Cyc_String_pa_PrintArg_struct _T7A=_T4F;{struct Cyc_String_pa_PrintArg_struct _T7B;_T7B.tag=0;_T7B.f1=Cyc_Port_ctype2string(deep,t);_T50=_T7B;}{struct Cyc_String_pa_PrintArg_struct _T7B=_T50;void*_T7C[2];_T7C[0]=& _T7A;_T7C[1]=& _T7B;_T51=_tag_fat("fn(%s)->%s",sizeof(char),11U);_T52=_tag_fat(_T7C,sizeof(void*),2);_T4E=Cyc_aprintf(_T51,_T52);}}return _T4E;}default: _T53=t;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T7A=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T53;_T73=_T7A->f1;}{struct Cyc_Port_Cvar*cv=_T73;_T54=cv;_T55=_T54->eq;_T56=(unsigned)_T55;
# 203
if(!_T56)goto _TL8;_T57=deep;_T58=cv;_T59=_T58->eq;_T5A=*_T59;_T5B=
Cyc_Port_ctype2string(_T57,_T5A);return _T5B;
_TL8: _T5C=deep;if(_T5C)goto _TLD;else{goto _TLC;}_TLD: _T5D=cv;_T5E=_T5D->uppers;if(_T5E==0)goto _TLE;else{goto _TLA;}_TLE: _T5F=cv;_T60=_T5F->lowers;if(_T60==0)goto _TLC;else{goto _TLA;}
_TLC:{struct Cyc_Int_pa_PrintArg_struct _T7A;_T7A.tag=1;_T63=cv;_T64=_T63->id;_T7A.f1=(unsigned long)_T64;_T62=_T7A;}{struct Cyc_Int_pa_PrintArg_struct _T7A=_T62;void*_T7B[1];_T7B[0]=& _T7A;_T65=_tag_fat("var(%d)",sizeof(char),8U);_T66=_tag_fat(_T7B,sizeof(void*),1);_T61=Cyc_aprintf(_T65,_T66);}return _T61;
_TLA:{struct Cyc_String_pa_PrintArg_struct _T7A;_T7A.tag=0;_T69=cv;_T6A=_T69->lowers;
_T7A.f1=Cyc_Port_ctypes2string(0,_T6A);_T68=_T7A;}{struct Cyc_String_pa_PrintArg_struct _T7A=_T68;{struct Cyc_Int_pa_PrintArg_struct _T7B;_T7B.tag=1;_T6C=cv;_T6D=_T6C->id;
_T7B.f1=(unsigned long)_T6D;_T6B=_T7B;}{struct Cyc_Int_pa_PrintArg_struct _T7B=_T6B;{struct Cyc_String_pa_PrintArg_struct _T7C;_T7C.tag=0;_T6F=cv;_T70=_T6F->uppers;
_T7C.f1=Cyc_Port_ctypes2string(0,_T70);_T6E=_T7C;}{struct Cyc_String_pa_PrintArg_struct _T7C=_T6E;void*_T7D[3];_T7D[0]=& _T7A;_T7D[1]=& _T7B;_T7D[2]=& _T7C;_T71=
# 207
_tag_fat("var([%s]<=%d<=[%s])",sizeof(char),20U);_T72=_tag_fat(_T7D,sizeof(void*),3);_T67=Cyc_aprintf(_T71,_T72);}}}return _T67;}};}
# 213
static struct _fat_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){struct _fat_ptr*_T0;{struct _fat_ptr*_T1=_cycalloc(sizeof(struct _fat_ptr));*_T1=Cyc_Port_ctype2string(deep,t);_T0=(struct _fat_ptr*)_T1;}return _T0;}
struct Cyc_List_List*Cyc_Port_sep(struct _fat_ptr s,struct Cyc_List_List*xs){struct _fat_ptr*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;
struct _fat_ptr*sptr;sptr=_cycalloc(sizeof(struct _fat_ptr));_T0=sptr;*_T0=s;
if(xs!=0)goto _TLF;_T1=xs;return _T1;_TLF: {
struct Cyc_List_List*prev=xs;_T2=xs;{
struct Cyc_List_List*curr=_T2->tl;
_TL14: if(curr!=0)goto _TL12;else{goto _TL13;}
_TL12: _T3=prev;{struct Cyc_List_List*_T7=_cycalloc(sizeof(struct Cyc_List_List));_T7->hd=sptr;_T7->tl=curr;_T4=(struct Cyc_List_List*)_T7;}_T3->tl=_T4;
# 219
prev=curr;_T5=curr;curr=_T5->tl;goto _TL14;_TL13: _T6=xs;
# 222
return _T6;}}}
# 224
static struct _fat_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;int _T2;struct Cyc_Port_Cfield*_T3;void*_T4;struct Cyc_String_pa_PrintArg_struct _T5;struct Cyc_Port_Cfield*_T6;struct _fat_ptr*_T7;struct Cyc_String_pa_PrintArg_struct _T8;int _T9;struct Cyc_Port_Cfield*_TA;void*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct _fat_ptr*_TE;{struct Cyc_String_pa_PrintArg_struct _TF;_TF.tag=0;_T2=deep;_T3=f;_T4=_T3->qual;
# 226
_TF.f1=Cyc_Port_ctype2string(_T2,_T4);_T1=_TF;}{struct Cyc_String_pa_PrintArg_struct _TF=_T1;{struct Cyc_String_pa_PrintArg_struct _T10;_T10.tag=0;_T6=f;_T7=_T6->f;_T10.f1=*_T7;_T5=_T10;}{struct Cyc_String_pa_PrintArg_struct _T10=_T5;{struct Cyc_String_pa_PrintArg_struct _T11;_T11.tag=0;_T9=deep;_TA=f;_TB=_TA->type;_T11.f1=Cyc_Port_ctype2string(_T9,_TB);_T8=_T11;}{struct Cyc_String_pa_PrintArg_struct _T11=_T8;void*_T12[3];_T12[0]=& _TF;_T12[1]=& _T10;_T12[2]=& _T11;_TC=_tag_fat("%s %s: %s",sizeof(char),10U);_TD=_tag_fat(_T12,sizeof(void*),3);_T0=Cyc_aprintf(_TC,_TD);}}}{
# 225
struct _fat_ptr s=_T0;{struct _fat_ptr*_TF=_cycalloc(sizeof(struct _fat_ptr));
# 227
*_TF=s;_TE=(struct _fat_ptr*)_TF;}return _TE;}}
# 229
static struct _fat_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){struct _fat_ptr _T0;struct Cyc_List_List*(*_T1)(struct _fat_ptr*(*)(int,void*),int,struct Cyc_List_List*);struct Cyc_List_List*(*_T2)(void*(*)(void*,void*),void*,struct Cyc_List_List*);int _T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct _fat_ptr _T7;_T0=
_tag_fat(",",sizeof(char),2U);_T2=Cyc_List_map_c;{struct Cyc_List_List*(*_T8)(struct _fat_ptr*(*)(int,void*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(int,void*),int,struct Cyc_List_List*))_T2;_T1=_T8;}_T3=deep;_T4=ts;_T5=_T1(Cyc_Port_ctype2stringptr,_T3,_T4);_T6=Cyc_Port_sep(_T0,_T5);_T7=Cyc_strconcat_l(_T6);return _T7;}
# 232
static struct _fat_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){struct _fat_ptr _T0;struct Cyc_List_List*(*_T1)(struct _fat_ptr*(*)(int,struct Cyc_Port_Cfield*),int,struct Cyc_List_List*);struct Cyc_List_List*(*_T2)(void*(*)(void*,void*),void*,struct Cyc_List_List*);int _T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct _fat_ptr _T7;_T0=
_tag_fat(";",sizeof(char),2U);_T2=Cyc_List_map_c;{struct Cyc_List_List*(*_T8)(struct _fat_ptr*(*)(int,struct Cyc_Port_Cfield*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(int,struct Cyc_Port_Cfield*),int,struct Cyc_List_List*))_T2;_T1=_T8;}_T3=deep;_T4=fs;_T5=_T1(Cyc_Port_cfield2stringptr,_T3,_T4);_T6=Cyc_Port_sep(_T0,_T5);_T7=Cyc_strconcat_l(_T6);return _T7;}
# 238
static void*Cyc_Port_notconst_ct (void){struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Notconst_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Notconst_ct_val;_T1=(struct Cyc_Port_Notconst_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_const_ct (void){struct Cyc_Port_Const_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Const_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Const_ct_val;_T1=(struct Cyc_Port_Const_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_thin_ct (void){struct Cyc_Port_Thin_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Thin_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Thin_ct_val;_T1=(struct Cyc_Port_Thin_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_fat_ct (void){struct Cyc_Port_Fat_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Fat_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Fat_ct_val;_T1=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_void_ct (void){struct Cyc_Port_Void_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Void_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Void_ct_val;_T1=(struct Cyc_Port_Void_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_zero_ct (void){struct Cyc_Port_Zero_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Zero_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Zero_ct_val;_T1=(struct Cyc_Port_Zero_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_arith_ct (void){struct Cyc_Port_Arith_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Arith_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Arith_ct_val;_T1=(struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_heap_ct (void){struct Cyc_Port_Heap_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Heap_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Heap_ct_val;_T1=(struct Cyc_Port_Heap_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_zterm_ct (void){struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Zterm_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Zterm_ct_val;_T1=(struct Cyc_Port_Zterm_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_nozterm_ct (void){struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*_T1;void*_T2;_T0=& Cyc_Port_Nozterm_ct_val;_T1=(struct Cyc_Port_Nozterm_ct_Port_Ctype_struct*)_T0;_T2=(void*)_T1;return _T2;}
static void*Cyc_Port_rgnvar_ct(struct _fat_ptr*n){struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T0;void*_T1;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T2=_cycalloc(sizeof(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct));_T2->tag=10;_T2->f1=n;_T0=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_T2;}_T1=(void*)_T0;return _T1;}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T0;void*_T1;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T2=_cycalloc(sizeof(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct));_T2->tag=14;
_T2->f1=fs;_T2->f2=0;_T0=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 252
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_T0;void*_T1;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_T2=_cycalloc(sizeof(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct));_T2->tag=13;
_T2->f1=p;_T0=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 255
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T0;void*_T1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2=_cycalloc(sizeof(struct Cyc_Port_Ptr_ct_Port_Ctype_struct));_T2->tag=11;
# 257
_T2->f1=elt;_T2->f2=qual;_T2->f3=ptr_kind;_T2->f4=r;_T2->f5=zt;_T0=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 259
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){struct Cyc_Port_Array_ct_Port_Ctype_struct*_T0;void*_T1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T2=_cycalloc(sizeof(struct Cyc_Port_Array_ct_Port_Ctype_struct));_T2->tag=12;
_T2->f1=elt;_T2->f2=qual;_T2->f3=zt;_T0=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 262
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T0;void*_T1;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T2=_cycalloc(sizeof(struct Cyc_Port_Fn_ct_Port_Ctype_struct));_T2->tag=15;
_T2->f1=return_type;_T2->f2=args;_T0=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 265
static void*Cyc_Port_var (void){struct Cyc_Port_Var_ct_Port_Ctype_struct*_T0;struct Cyc_Port_Cvar*_T1;int _T2;void*_T3;
static int counter=0;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T4=_cycalloc(sizeof(struct Cyc_Port_Var_ct_Port_Ctype_struct));_T4->tag=16;{struct Cyc_Port_Cvar*_T5=_cycalloc(sizeof(struct Cyc_Port_Cvar));_T2=counter;
counter=_T2 + 1;_T5->id=_T2;_T5->eq=0;_T5->uppers=0;_T5->lowers=0;_T1=(struct Cyc_Port_Cvar*)_T5;}_T4->f1=_T1;_T0=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T4;}_T3=(void*)_T0;return _T3;}
# 269
static void*Cyc_Port_new_var(void*x){void*_T0;_T0=
Cyc_Port_var();return _T0;}
# 272
static struct _fat_ptr*Cyc_Port_new_region_var (void){struct _fat_ptr _T0;struct Cyc_Int_pa_PrintArg_struct _T1;int _T2;int _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr*_T6;
static int counter=0;{struct Cyc_Int_pa_PrintArg_struct _T7;_T7.tag=1;_T2=counter;
counter=_T2 + 1;_T3=_T2;_T7.f1=(unsigned long)_T3;_T1=_T7;}{struct Cyc_Int_pa_PrintArg_struct _T7=_T1;void*_T8[1];_T8[0]=& _T7;_T4=_tag_fat("`R%d",sizeof(char),5U);_T5=_tag_fat(_T8,sizeof(void*),1);_T0=Cyc_aprintf(_T4,_T5);}{struct _fat_ptr s=_T0;{struct _fat_ptr*_T7=_cycalloc(sizeof(struct _fat_ptr));
*_T7=s;_T6=(struct _fat_ptr*)_T7;}return _T6;}}
# 280
static int Cyc_Port_unifies(void*,void*);
# 282
static void*Cyc_Port_compress_ct(void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Port_Cvar*_T4;void*_T5;struct Cyc_Port_Var_ct_Port_Ctype_struct*_T6;struct Cyc_Port_Cvar*_T7;void***_T8;void***_T9;void**_TA;unsigned _TB;void**_TC;void*_TD;void**_TE;void*_TF;void*_T10;void***_T11;void**_T12;void*_T13;struct Cyc_List_List*_T14;int*_T15;unsigned _T16;void***_T17;void**_T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;void*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;int*_T1E;unsigned _T1F;void***_T20;void**_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_List_List*_T25;void*_T26;void*_T27;void**_T28;unsigned _T29;void**_T2A;void*_T2B;void*_T2C;void*_T2D;void*_T2E;void**_T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;void*_T32;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 16: _T3=t;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T33=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T3;_T4=_T33->f1;{struct Cyc_Port_Cvar _T34=*_T4;_T5=t;_T6=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T5;_T7=_T6->f1;_T8=& _T7->eq;_T32=(void***)_T8;_T31=_T34.uppers;_T30=_T34.lowers;}}{void***eqp=(void***)_T32;struct Cyc_List_List*us=_T31;struct Cyc_List_List*ls=_T30;_T9=eqp;{
# 285
void**eq=*_T9;_TA=eq;_TB=(unsigned)_TA;
if(!_TB)goto _TL16;_TC=eq;_TD=*_TC;{
# 289
void*r=Cyc_Port_compress_ct(_TD);_TE=eq;_TF=*_TE;_T10=r;
if(_TF==_T10)goto _TL18;_T11=eqp;{void**_T33=_cycalloc(sizeof(void*));*_T33=r;_T12=(void**)_T33;}*_T11=_T12;goto _TL19;_TL18: _TL19: _T13=r;
return _T13;}_TL16:
# 296
 _TL1D: if(ls!=0)goto _TL1B;else{goto _TL1C;}
_TL1B: _T14=ls;{void*_T33=_T14->hd;_T15=(int*)_T33;_T16=*_T15;switch(_T16){case 0: goto _LLB;case 9: _LLB: goto _LLD;case 4: _LLD: goto _LLF;case 13: _LLF: goto _LL11;case 15: _LL11: _T17=eqp;{void**_T34=_cycalloc(sizeof(void*));_T19=ls;
# 303
*_T34=_T19->hd;_T18=(void**)_T34;}*_T17=_T18;_T1A=ls;_T1B=_T1A->hd;
return _T1B;default: goto _LL7;}_LL7:;}_T1C=ls;
# 296
ls=_T1C->tl;goto _TL1D;_TL1C:
# 309
 _TL22: if(us!=0)goto _TL20;else{goto _TL21;}
_TL20: _T1D=us;{void*_T33=_T1D->hd;_T1E=(int*)_T33;_T1F=*_T1E;switch(_T1F){case 1: goto _LL18;case 8: _LL18: goto _LL1A;case 5: _LL1A: goto _LL1C;case 13: _LL1C: goto _LL1E;case 15: _LL1E: _T20=eqp;{void**_T34=_cycalloc(sizeof(void*));_T22=us;
# 316
*_T34=_T22->hd;_T21=(void**)_T34;}*_T20=_T21;_T23=us;_T24=_T23->hd;
return _T24;default: goto _LL14;}_LL14:;}_T25=us;
# 309
us=_T25->tl;goto _TL22;_TL21: _T26=t;
# 322
return _T26;}}case 14: _T27=t;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T33=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T27;_T2F=_T33->f2;}{void**eq=_T2F;_T28=eq;_T29=(unsigned)_T28;
# 325
if(!_T29)goto _TL24;_T2A=eq;_T2B=*_T2A;_T2C=Cyc_Port_compress_ct(_T2B);return _T2C;
_TL24: _T2D=t;return _T2D;}default: _T2E=t;
# 329
return _T2E;};}struct _tuple12{void*f0;void*f1;};
# 335
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){int*_T0;unsigned _T1;void*_T2;int(*_T3)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T4)(struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict*_T5;struct Cyc_Dict_Dict _T6;struct _fat_ptr*_T7;int _T8;struct Cyc_Dict_Dict*_T9;struct Cyc_Dict_Dict(*_TA)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*);struct Cyc_Dict_Dict(*_TB)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_TC;struct Cyc_Dict_Dict _TD;struct _fat_ptr*_TE;void*_TF;void*(*_T10)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T11)(struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict*_T12;struct Cyc_Dict_Dict _T13;struct _fat_ptr*_T14;void*_T15;void*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;struct _tuple12 _T1B;void*_T1C;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;struct Cyc_Port_Array_ct_Port_Ctype_struct*_T23;void*_T24;void*_T25;struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T26;struct Cyc_List_List*(*_T27)(void*(*)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*,struct Cyc_List_List*);struct Cyc_List_List*(*_T28)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Dict_Dict*_T29;struct Cyc_List_List*_T2A;void*_T2B;
void*_T2C=Cyc_Port_compress_ct(t);struct Cyc_List_List*_T2D;void*_T2E;void*_T2F;void*_T30;void*_T31;void*_T32;_T0=(int*)_T2C;_T1=*_T0;switch(_T1){case 0: goto _LL4;case 1: _LL4: goto _LL6;case 2: _LL6: goto _LL8;case 3: _LL8: goto _LLA;case 4: _LLA: goto _LLC;case 5: _LLC: goto _LLE;case 6: _LLE: goto _LL10;case 8: _LL10: goto _LL12;case 9: _LL12: goto _LL14;case 14: _LL14: goto _LL16;case 13: _LL16: goto _LL18;case 16: _LL18: goto _LL1A;case 7: _LL1A: _T2=t;
# 349
return _T2;case 10:{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T33=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_T2C;_T32=_T33->f1;}{struct _fat_ptr*n=_T32;_T4=Cyc_Dict_member;{
# 351
int(*_T33)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T4;_T3=_T33;}_T5=instenv;_T6=*_T5;_T7=n;_T8=_T3(_T6,_T7);if(_T8)goto _TL27;else{goto _TL29;}
_TL29: _T9=instenv;_TB=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T33)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))_TB;_TA=_T33;}_TC=instenv;_TD=*_TC;_TE=n;_TF=Cyc_Port_var();*_T9=_TA(_TD,_TE,_TF);goto _TL28;_TL27: _TL28: _T11=Cyc_Dict_lookup;{
void*(*_T33)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T11;_T10=_T33;}_T12=instenv;_T13=*_T12;_T14=n;_T15=_T10(_T13,_T14);return _T15;}case 11:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T33=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T2C;_T16=_T33->f1;_T32=(void*)_T16;_T17=_T33->f2;_T31=(void*)_T17;_T18=_T33->f3;_T30=(void*)_T18;_T19=_T33->f4;_T2F=(void*)_T19;_T1A=_T33->f5;_T2E=(void*)_T1A;}{void*t1=_T32;void*t2=_T31;void*t3=_T30;void*t4=_T2F;void*zt=_T2E;{struct _tuple12 _T33;
# 355
_T33.f0=Cyc_Port_inst(instenv,t1);_T33.f1=Cyc_Port_inst(instenv,t4);_T1B=_T33;}{struct _tuple12 _T33=_T1B;void*_T34;void*_T35;_T35=_T33.f0;_T34=_T33.f1;{void*nt1=_T35;void*nt4=_T34;
if(nt1!=t1)goto _TL2A;if(nt4!=t4)goto _TL2A;_T1C=t;return _T1C;_TL2A:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T36=_cycalloc(sizeof(struct Cyc_Port_Ptr_ct_Port_Ctype_struct));_T36->tag=11;
_T36->f1=nt1;_T36->f2=t2;_T36->f3=t3;_T36->f4=nt4;_T36->f5=zt;_T1D=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T36;}_T1E=(void*)_T1D;return _T1E;}}}case 12:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T33=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T2C;_T1F=_T33->f1;_T32=(void*)_T1F;_T20=_T33->f2;_T31=(void*)_T20;_T21=_T33->f3;_T30=(void*)_T21;}{void*t1=_T32;void*t2=_T31;void*zt=_T30;
# 359
void*nt1=Cyc_Port_inst(instenv,t1);
if(nt1!=t1)goto _TL2C;_T22=t;return _T22;_TL2C:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T33=_cycalloc(sizeof(struct Cyc_Port_Array_ct_Port_Ctype_struct));_T33->tag=12;
_T33->f1=nt1;_T33->f2=t2;_T33->f3=zt;_T23=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T33;}_T24=(void*)_T23;return _T24;}default:{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T33=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T2C;_T25=_T33->f1;_T32=(void*)_T25;_T2D=_T33->f2;}{void*t1=_T32;struct Cyc_List_List*ts=_T2D;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T33=_cycalloc(sizeof(struct Cyc_Port_Fn_ct_Port_Ctype_struct));_T33->tag=15;
# 363
_T33->f1=Cyc_Port_inst(instenv,t1);_T28=Cyc_List_map_c;{struct Cyc_List_List*(*_T34)(void*(*)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*,struct Cyc_List_List*))_T28;_T27=_T34;}_T29=instenv;_T2A=ts;_T33->f2=_T27(Cyc_Port_inst,_T29,_T2A);_T26=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T33;}_T2B=(void*)_T26;return _T2B;}};}
# 367
void*Cyc_Port_instantiate(void*t){struct Cyc_Dict_Dict*_T0;struct Cyc_Dict_Dict(*_T1)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T2)(int(*)(void*,void*));int(*_T3)(struct _fat_ptr*,struct _fat_ptr*);void*_T4;void*_T5;{struct Cyc_Dict_Dict*_T6=_cycalloc(sizeof(struct Cyc_Dict_Dict));_T2=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T7)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T2;_T1=_T7;}_T3=Cyc_strptrcmp;*_T6=_T1(_T3);_T0=(struct Cyc_Dict_Dict*)_T6;}_T4=t;_T5=Cyc_Port_inst(_T0,_T4);return _T5;}
# 374
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;int _T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_List_List*_T8;void*_T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;
int found=0;{
struct Cyc_List_List*xs=ts;_TL31: _T0=xs;_T1=(unsigned)_T0;if(_T1)goto _TL2F;else{goto _TL30;}
_TL2F: _T2=xs;_T3=_T2->hd;{void*t2=Cyc_Port_compress_ct(_T3);
if(t!=t2)goto _TL32;found=1;goto _TL33;_TL32: _TL33:;}_T4=xs;
# 376
xs=_T4->tl;goto _TL31;_TL30:;}_T5=found;
# 380
if(_T5)goto _TL34;else{goto _TL36;}_TL36: _T6=ts;return _T6;_TL34: {
struct Cyc_List_List*res=0;
_TL3A: if(ts!=0)goto _TL38;else{goto _TL39;}
_TL38: _T7=t;_T8=ts;_T9=_T8->hd;_TA=Cyc_Port_compress_ct(_T9);if(_T7!=_TA)goto _TL3B;goto _TL37;_TL3B:{struct Cyc_List_List*_TF=_cycalloc(sizeof(struct Cyc_List_List));_TC=ts;
_TF->hd=_TC->hd;_TF->tl=res;_TB=(struct Cyc_List_List*)_TF;}res=_TB;_TL37: _TD=ts;
# 382
ts=_TD->tl;goto _TL3A;_TL39: _TE=res;
# 386
return _TE;}}
# 391
void Cyc_Port_generalize(int is_rgn,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Port_Cvar*_T4;void*_T5;struct Cyc_Port_Cvar*_T6;struct Cyc_List_List*_T7;struct Cyc_Port_Cvar*_T8;void*_T9;struct Cyc_Port_Cvar*_TA;struct Cyc_List_List*_TB;int _TC;struct Cyc_Port_Cvar*_TD;struct Cyc_List_List*_TE;struct Cyc_Port_Cvar*_TF;struct Cyc_List_List*_T10;void*_T11;struct _fat_ptr*_T12;void*_T13;struct Cyc_Port_Cvar*_T14;struct Cyc_List_List*_T15;unsigned _T16;void*_T17;struct Cyc_Port_Cvar*_T18;struct Cyc_List_List*_T19;void*_T1A;void*_T1B;struct Cyc_Port_Cvar*_T1C;struct Cyc_List_List*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;void*_T27;void*_T28;void*_T29;void*_T2A;void(*_T2B)(void(*)(int,void*),int,struct Cyc_List_List*);void(*_T2C)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T2D;
t=Cyc_Port_compress_ct(t);{struct Cyc_List_List*_T2E;void*_T2F;void*_T30;void*_T31;void*_T32;void*_T33;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 16: _T3=t;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T34=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T3;_T33=_T34->f1;}{struct Cyc_Port_Cvar*cv=_T33;_T4=cv;_T5=t;_T6=cv;_T7=_T6->uppers;
# 396
_T4->uppers=Cyc_Port_filter_self(_T5,_T7);_T8=cv;_T9=t;_TA=cv;_TB=_TA->lowers;
_T8->lowers=Cyc_Port_filter_self(_T9,_TB);_TC=is_rgn;
if(!_TC)goto _TL3E;_TD=cv;_TE=_TD->uppers;
# 401
if(_TE!=0)goto _TL40;_TF=cv;_T10=_TF->lowers;if(_T10!=0)goto _TL40;_T11=t;_T12=
Cyc_Port_new_region_var();_T13=Cyc_Port_rgnvar_ct(_T12);Cyc_Port_unifies(_T11,_T13);
return;_TL40: _T14=cv;_T15=_T14->uppers;_T16=(unsigned)_T15;
# 406
if(!_T16)goto _TL42;_T17=t;_T18=cv;_T19=_T18->uppers;_T1A=_T19->hd;
Cyc_Port_unifies(_T17,_T1A);
Cyc_Port_generalize(1,t);goto _TL43;
# 410
_TL42: _T1B=t;_T1C=cv;_T1D=_T1C->lowers;_T1E=_T1D->hd;Cyc_Port_unifies(_T1B,_T1E);
Cyc_Port_generalize(1,t);_TL43: goto _TL3F;_TL3E: _TL3F:
# 414
 return;}case 0: goto _LL6;case 1: _LL6: goto _LL8;case 2: _LL8: goto _LLA;case 3: _LLA: goto _LLC;case 4: _LLC: goto _LLE;case 5: _LLE: goto _LL10;case 6: _LL10: goto _LL12;case 14: _LL12: goto _LL14;case 13: _LL14: goto _LL16;case 10: _LL16: goto _LL18;case 9: _LL18: goto _LL1A;case 8: _LL1A: goto _LL1C;case 7: _LL1C:
# 427
 return;case 11: _T1F=t;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T34=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T1F;_T20=_T34->f1;_T33=(void*)_T20;_T21=_T34->f2;_T32=(void*)_T21;_T22=_T34->f3;_T31=(void*)_T22;_T23=_T34->f4;_T30=(void*)_T23;_T24=_T34->f5;_T2F=(void*)_T24;}{void*t1=_T33;void*t2=_T32;void*t3=_T31;void*t4=_T30;void*t5=_T2F;
# 431
Cyc_Port_generalize(0,t1);Cyc_Port_generalize(1,t4);goto _LL0;}case 12: _T25=t;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T34=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T25;_T26=_T34->f1;_T33=(void*)_T26;_T27=_T34->f2;_T32=(void*)_T27;_T28=_T34->f3;_T31=(void*)_T28;}{void*t1=_T33;void*t2=_T32;void*t3=_T31;
# 433
Cyc_Port_generalize(0,t1);Cyc_Port_generalize(0,t2);goto _LL0;}default: _T29=t;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T34=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T29;_T2A=_T34->f1;_T33=(void*)_T2A;_T2E=_T34->f2;}{void*t1=_T33;struct Cyc_List_List*ts=_T2E;
# 435
Cyc_Port_generalize(0,t1);_T2C=Cyc_List_iter_c;{void(*_T34)(void(*)(int,void*),int,struct Cyc_List_List*)=(void(*)(void(*)(int,void*),int,struct Cyc_List_List*))_T2C;_T2B=_T34;}_T2D=ts;_T2B(Cyc_Port_generalize,0,_T2D);goto _LL0;}}_LL0:;}}
# 441
static int Cyc_Port_occurs(void*v,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;void*_T8;int _T9;int _TA;int _TB;int _TC;int _TD;void*_TE;void*_TF;void*_T10;void*_T11;int _T12;int _T13;int _T14;void*_T15;void*_T16;int _T17;struct Cyc_List_List*_T18;unsigned _T19;void*_T1A;struct Cyc_List_List*_T1B;void*_T1C;int _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct _tuple11*_T20;void*_T21;struct Cyc_List_List*_T22;unsigned _T23;void*_T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_Port_Cfield*_T27;void*_T28;int _T29;void*_T2A;struct Cyc_List_List*_T2B;void*_T2C;struct Cyc_Port_Cfield*_T2D;void*_T2E;int _T2F;struct Cyc_List_List*_T30;
t=Cyc_Port_compress_ct(t);
if(t!=v)goto _TL44;return 1;_TL44: {struct Cyc_List_List*_T31;void*_T32;void*_T33;void*_T34;void*_T35;void*_T36;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 11: _T3=t;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T37=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T3;_T4=_T37->f1;_T36=(void*)_T4;_T5=_T37->f2;_T35=(void*)_T5;_T6=_T37->f3;_T34=(void*)_T6;_T7=_T37->f4;_T33=(void*)_T7;_T8=_T37->f5;_T32=(void*)_T8;}{void*t1=_T36;void*t2=_T35;void*t3=_T34;void*t4=_T33;void*t5=_T32;_TA=
# 446
Cyc_Port_occurs(v,t1);if(_TA)goto _TL49;else{goto _TL4C;}_TL4C: _TB=Cyc_Port_occurs(v,t2);if(_TB)goto _TL49;else{goto _TL4B;}_TL4B: _TC=Cyc_Port_occurs(v,t3);if(_TC)goto _TL49;else{goto _TL4A;}_TL4A: _TD=Cyc_Port_occurs(v,t4);if(_TD)goto _TL49;else{goto _TL47;}_TL49: _T9=1;goto _TL48;_TL47: _T9=
Cyc_Port_occurs(v,t5);_TL48:
# 446
 return _T9;}case 12: _TE=t;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T37=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_TE;_TF=_T37->f1;_T36=(void*)_TF;_T10=_T37->f2;_T35=(void*)_T10;_T11=_T37->f3;_T34=(void*)_T11;}{void*t1=_T36;void*t2=_T35;void*t3=_T34;_T13=
# 449
Cyc_Port_occurs(v,t1);if(_T13)goto _TL4F;else{goto _TL50;}_TL50: _T14=Cyc_Port_occurs(v,t2);if(_T14)goto _TL4F;else{goto _TL4D;}_TL4F: _T12=1;goto _TL4E;_TL4D: _T12=Cyc_Port_occurs(v,t3);_TL4E: return _T12;}case 15: _T15=t;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T37=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T15;_T16=_T37->f1;_T36=(void*)_T16;_T31=_T37->f2;}{void*t=_T36;struct Cyc_List_List*ts=_T31;_T17=
# 451
Cyc_Port_occurs(v,t);if(!_T17)goto _TL51;return 1;_TL51:
 _TL56: _T18=ts;_T19=(unsigned)_T18;if(_T19)goto _TL54;else{goto _TL55;}
_TL54: _T1A=v;_T1B=ts;_T1C=_T1B->hd;_T1D=Cyc_Port_occurs(_T1A,_T1C);if(!_T1D)goto _TL57;return 1;_TL57: _T1E=ts;
# 452
ts=_T1E->tl;goto _TL56;_TL55:
# 454
 return 0;}case 13: _T1F=t;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_T37=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_T1F;_T20=_T37->f1;{struct _tuple11 _T38=*_T20;_T31=_T38.f1;}}{struct Cyc_List_List*fs=_T31;
return 0;}case 14: _T21=t;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T37=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T21;_T31=_T37->f1;}{struct Cyc_List_List*fs=_T31;
# 457
_TL5C: _T22=fs;_T23=(unsigned)_T22;if(_T23)goto _TL5A;else{goto _TL5B;}
_TL5A: _T24=v;_T25=fs;_T26=_T25->hd;_T27=(struct Cyc_Port_Cfield*)_T26;_T28=_T27->qual;_T29=Cyc_Port_occurs(_T24,_T28);if(_T29)goto _TL5F;else{goto _TL60;}_TL60: _T2A=v;_T2B=fs;_T2C=_T2B->hd;_T2D=(struct Cyc_Port_Cfield*)_T2C;_T2E=_T2D->type;_T2F=Cyc_Port_occurs(_T2A,_T2E);if(_T2F)goto _TL5F;else{goto _TL5D;}_TL5F: return 1;_TL5D: _T30=fs;
# 457
fs=_T30->tl;goto _TL5C;_TL5B:
# 459
 return 0;}default:
 return 0;};}}char Cyc_Port_Unify_ct[9U]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 469
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 471
static int Cyc_Port_leq(void*,void*);
static void Cyc_Port_unify_cts(struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*,struct Cyc_List_List*,int);
# 475
static void Cyc_Port_unify_ct(void*t1,void*t2){struct _tuple12 _T0;void*_T1;int*_T2;int _T3;void*_T4;int _T5;void*_T6;struct Cyc_Port_Cvar*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;unsigned _TA;void*_TB;struct Cyc_List_List*_TC;void*_TD;int _TE;struct Cyc_Port_Unify_ct_exn_struct*_TF;struct Cyc_Port_Unify_ct_exn_struct*_T10;struct Cyc_List_List*_T11;void*_T12;struct Cyc_Port_Cvar*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;unsigned _T16;struct Cyc_List_List*_T17;void*_T18;void*_T19;int _T1A;struct Cyc_Port_Unify_ct_exn_struct*_T1B;struct Cyc_Port_Unify_ct_exn_struct*_T1C;struct Cyc_List_List*_T1D;struct Cyc_Port_Cvar*_T1E;void**_T1F;struct Cyc_Port_Unify_ct_exn_struct*_T20;struct Cyc_Port_Unify_ct_exn_struct*_T21;void*_T22;int*_T23;int _T24;void*_T25;void*_T26;int*_T27;unsigned _T28;void*_T29;int*_T2A;int _T2B;void*_T2C;void*_T2D;void*_T2E;void*_T2F;void*_T30;void*_T31;void*_T32;void*_T33;void*_T34;void*_T35;void*_T36;void*_T37;void*_T38;int*_T39;int _T3A;void*_T3B;struct _fat_ptr*_T3C;void*_T3D;struct _fat_ptr*_T3E;int _T3F;struct Cyc_Port_Unify_ct_exn_struct*_T40;struct Cyc_Port_Unify_ct_exn_struct*_T41;void*_T42;int*_T43;int _T44;void*_T45;void*_T46;void*_T47;void*_T48;void*_T49;void*_T4A;void*_T4B;void*_T4C;void*_T4D;int*_T4E;int _T4F;void*_T50;void*_T51;void*_T52;void*_T53;void*_T54;int*_T55;unsigned _T56;void*_T57;void*_T58;struct Cyc_Port_Unify_ct_exn_struct*_T59;struct Cyc_Port_Unify_ct_exn_struct*_T5A;void*_T5B;struct _tuple11*_T5C;void*_T5D;void*_T5E;struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T5F;void***_T60;void***_T61;void**_T62;void*_T63;int*_T64;unsigned _T65;void*_T66;void*_T67;struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T68;void***_T69;void*_T6A;void*_T6B;struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T6C;void***_T6D;struct Cyc_List_List*_T6E;void***_T6F;void***_T70;void**_T71;void*_T72;void*_T73;struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T74;void***_T75;void*_T76;struct _tuple11*_T77;void***_T78;void**_T79;struct Cyc_Port_Unify_ct_exn_struct*_T7A;struct Cyc_Port_Unify_ct_exn_struct*_T7B;
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);
if(t1!=t2)goto _TL61;return;_TL61:{struct _tuple12 _T7C;
_T7C.f0=t1;_T7C.f1=t2;_T0=_T7C;}{struct _tuple12 _T7C=_T0;struct Cyc_Absyn_Aggrdecl*_T7D;struct Cyc_List_List*_T7E;struct Cyc_List_List*_T7F;struct _fat_ptr _T80;struct _fat_ptr _T81;void*_T82;void*_T83;void*_T84;void*_T85;void*_T86;void*_T87;void*_T88;void*_T89;void*_T8A;void*_T8B;_T1=_T7C.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=16)goto _TL63;_T4=_T7C.f0;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T4;_T8B=_T8C->f1;}{struct Cyc_Port_Cvar*v1=_T8B;_T5=
# 481
Cyc_Port_occurs(t1,t2);if(_T5)goto _TL65;else{goto _TL67;}
# 484
_TL67: _T6=t1;_T7=v1;_T8=_T7->uppers;{struct Cyc_List_List*us=Cyc_Port_filter_self(_T6,_T8);_TL6B: _T9=us;_TA=(unsigned)_T9;if(_TA)goto _TL69;else{goto _TL6A;}
_TL69: _TB=t2;_TC=us;_TD=_TC->hd;_TE=Cyc_Port_leq(_TB,_TD);if(_TE)goto _TL6C;else{goto _TL6E;}_TL6E: _TF=& Cyc_Port_Unify_ct_val;_T10=(struct Cyc_Port_Unify_ct_exn_struct*)_TF;_throw(_T10);goto _TL6D;_TL6C: _TL6D: _T11=us;
# 484
us=_T11->tl;goto _TL6B;_TL6A:;}_T12=t1;_T13=v1;_T14=_T13->lowers;{
# 486
struct Cyc_List_List*ls=Cyc_Port_filter_self(_T12,_T14);_TL72: _T15=ls;_T16=(unsigned)_T15;if(_T16)goto _TL70;else{goto _TL71;}
_TL70: _T17=ls;_T18=_T17->hd;_T19=t2;_T1A=Cyc_Port_leq(_T18,_T19);if(_T1A)goto _TL73;else{goto _TL75;}_TL75: _T1B=& Cyc_Port_Unify_ct_val;_T1C=(struct Cyc_Port_Unify_ct_exn_struct*)_T1B;_throw(_T1C);goto _TL74;_TL73: _TL74: _T1D=ls;
# 486
ls=_T1D->tl;goto _TL72;_TL71:;}_T1E=v1;{void**_T8C=_cycalloc(sizeof(void*));
# 488
*_T8C=t2;_T1F=(void**)_T8C;}_T1E->eq=_T1F;
return;
_TL65: _T20=& Cyc_Port_Unify_ct_val;_T21=(struct Cyc_Port_Unify_ct_exn_struct*)_T20;_throw(_T21);;}goto _TL64;_TL63: _T22=_T7C.f1;_T23=(int*)_T22;_T24=*_T23;if(_T24!=16)goto _TL76;_T25=_T7C.f1;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T25;_T8B=_T8C->f1;}{struct Cyc_Port_Cvar*v1=_T8B;
Cyc_Port_unify_ct(t2,t1);return;}_TL76: _T26=_T7C.f0;_T27=(int*)_T26;_T28=*_T27;switch(_T28){case 11: _T29=_T7C.f1;_T2A=(int*)_T29;_T2B=*_T2A;if(_T2B!=11)goto _TL79;_T2C=_T7C.f0;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T2C;_T2D=_T8C->f1;_T8B=(void*)_T2D;_T2E=_T8C->f2;_T8A=(void*)_T2E;_T2F=_T8C->f3;_T89=(void*)_T2F;_T30=_T8C->f4;_T88=(void*)_T30;_T31=_T8C->f5;_T87=(void*)_T31;}_T32=_T7C.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T32;_T33=_T8C->f1;_T86=(void*)_T33;_T34=_T8C->f2;_T85=(void*)_T34;_T35=_T8C->f3;_T84=(void*)_T35;_T36=_T8C->f4;_T83=(void*)_T36;_T37=_T8C->f5;_T82=(void*)_T37;}{void*e1=_T8B;void*q1=_T8A;void*k1=_T89;void*r1=_T88;void*zt1=_T87;void*e2=_T86;void*q2=_T85;void*k2=_T84;void*r2=_T83;void*zt2=_T82;
# 493
Cyc_Port_unify_ct(e1,e2);Cyc_Port_unify_ct(q1,q2);Cyc_Port_unify_ct(k1,k2);Cyc_Port_unify_ct(r1,r2);
Cyc_Port_unify_ct(zt1,zt2);
return;}_TL79: goto _LL15;case 10: _T38=_T7C.f1;_T39=(int*)_T38;_T3A=*_T39;if(_T3A!=10)goto _TL7B;_T3B=_T7C.f0;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_T3B;_T3C=_T8C->f1;{struct _fat_ptr _T8D=*_T3C;_T81=_T8D;}}_T3D=_T7C.f1;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_T3D;_T3E=_T8C->f1;{struct _fat_ptr _T8D=*_T3E;_T80=_T8D;}}{struct _fat_ptr n1=_T81;struct _fat_ptr n2=_T80;_T3F=
# 497
Cyc_strcmp(n1,n2);if(_T3F==0)goto _TL7D;_T40=& Cyc_Port_Unify_ct_val;_T41=(struct Cyc_Port_Unify_ct_exn_struct*)_T40;_throw(_T41);goto _TL7E;_TL7D: _TL7E:
 return;}_TL7B: goto _LL15;case 12: _T42=_T7C.f1;_T43=(int*)_T42;_T44=*_T43;if(_T44!=12)goto _TL7F;_T45=_T7C.f0;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T45;_T46=_T8C->f1;_T8B=(void*)_T46;_T47=_T8C->f2;_T8A=(void*)_T47;_T48=_T8C->f3;_T89=(void*)_T48;}_T49=_T7C.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T49;_T4A=_T8C->f1;_T88=(void*)_T4A;_T4B=_T8C->f2;_T87=(void*)_T4B;_T4C=_T8C->f3;_T86=(void*)_T4C;}{void*e1=_T8B;void*q1=_T8A;void*zt1=_T89;void*e2=_T88;void*q2=_T87;void*zt2=_T86;
# 500
Cyc_Port_unify_ct(e1,e2);Cyc_Port_unify_ct(q1,q2);Cyc_Port_unify_ct(zt1,zt2);return;}_TL7F: goto _LL15;case 15: _T4D=_T7C.f1;_T4E=(int*)_T4D;_T4F=*_T4E;if(_T4F!=15)goto _TL81;_T50=_T7C.f0;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T50;_T51=_T8C->f1;_T8B=(void*)_T51;_T8A=_T8C->f2;}_T52=_T7C.f1;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T52;_T53=_T8C->f1;_T89=(void*)_T53;_T7F=_T8C->f2;}{void*t1=_T8B;struct Cyc_List_List*ts1=_T8A;void*t2=_T89;struct Cyc_List_List*ts2=_T7F;
# 502
Cyc_Port_unify_ct(t1,t2);Cyc_Port_unify_cts(ts1,ts2);return;}_TL81: goto _LL15;case 13: _T54=_T7C.f1;_T55=(int*)_T54;_T56=*_T55;switch(_T56){case 13: _T57=_T7C.f0;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_T57;_T8B=_T8C->f1;}_T58=_T7C.f1;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_T58;_T8A=_T8C->f1;}{struct _tuple11*p1=_T8B;struct _tuple11*p2=_T8A;
# 504
if(p1!=p2)goto _TL84;return;_TL84: _T59=& Cyc_Port_Unify_ct_val;_T5A=(struct Cyc_Port_Unify_ct_exn_struct*)_T59;_throw(_T5A);;}case 14: _T5B=_T7C.f0;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_T5B;_T5C=_T8C->f1;{struct _tuple11 _T8D=*_T5C;_T8B=_T8D.f0;_T7F=_T8D.f1;}}_T5D=_T7C.f1;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T5D;_T7E=_T8C->f1;_T5E=_T7C.f1;_T5F=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T5E;_T60=& _T5F->f2;_T8A=(void***)_T60;}{struct Cyc_Absyn_Aggrdecl*ad=_T8B;struct Cyc_List_List*fs2=_T7F;struct Cyc_List_List*fs1=_T7E;void***eq=_T8A;
# 514
Cyc_Port_merge_fields(fs2,fs1,0);_T61=eq;{void**_T8C=_cycalloc(sizeof(void*));
*_T8C=t1;_T62=(void**)_T8C;}*_T61=_T62;
return;}default: goto _LL15;};case 14: _T63=_T7C.f1;_T64=(int*)_T63;_T65=*_T64;switch(_T65){case 14: _T66=_T7C.f0;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T66;_T7F=_T8C->f1;_T67=_T7C.f0;_T68=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T67;_T69=& _T68->f2;_T8B=(void***)_T69;}_T6A=_T7C.f1;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T6A;_T7E=_T8C->f1;_T6B=_T7C.f1;_T6C=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T6B;_T6D=& _T6C->f2;_T8A=(void***)_T6D;}{struct Cyc_List_List*fs1=_T7F;void***eq1=_T8B;struct Cyc_List_List*fs2=_T7E;void***eq2=(void***)_T8A;_T6E=
# 506
Cyc_Port_merge_fields(fs1,fs2,1);{void*t=Cyc_Port_unknown_aggr_ct(_T6E);_T6F=eq1;_T70=eq2;{void**_T8C=_cycalloc(sizeof(void*));
*_T8C=t;_T71=(void**)_T8C;}*_T70=_T71;*_T6F=*_T70;
return;}}case 13: _T72=_T7C.f0;{struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T72;_T7F=_T8C->f1;_T73=_T7C.f0;_T74=(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_T73;_T75=& _T74->f2;_T8B=(void***)_T75;}_T76=_T7C.f1;{struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_T8C=(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_T76;_T77=_T8C->f1;{struct _tuple11 _T8D=*_T77;_T7D=_T8D.f0;_T7E=_T8D.f1;}}{struct Cyc_List_List*fs1=_T7F;void***eq=(void***)_T8B;struct Cyc_Absyn_Aggrdecl*ad=_T7D;struct Cyc_List_List*fs2=_T7E;
# 510
Cyc_Port_merge_fields(fs2,fs1,0);_T78=eq;{void**_T8C=_cycalloc(sizeof(void*));
*_T8C=t2;_T79=(void**)_T8C;}*_T78=_T79;
return;}default: goto _LL15;};default: _LL15: _T7A=& Cyc_Port_Unify_ct_val;_T7B=(struct Cyc_Port_Unify_ct_exn_struct*)_T7A;_throw(_T7B);}_TL64:;}}
# 521
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_Port_Unify_ct_exn_struct*_T6;struct Cyc_Port_Unify_ct_exn_struct*_T7;
_TL8A: if(t1!=0)goto _TL8B;else{goto _TL89;}_TL8B: if(t2!=0)goto _TL88;else{goto _TL89;}
_TL88: _T0=t1;_T1=_T0->hd;_T2=t2;_T3=_T2->hd;Cyc_Port_unify_ct(_T1,_T3);_T4=t1;
# 522
t1=_T4->tl;_T5=t2;t2=_T5->tl;goto _TL8A;_TL89:
# 525
 if(t1!=0)goto _TL8E;else{goto _TL8F;}_TL8F: if(t2!=0)goto _TL8E;else{goto _TL8C;}
_TL8E: _T6=& Cyc_Port_Unify_ct_val;_T7=(struct Cyc_Port_Unify_ct_exn_struct*)_T6;_throw(_T7);goto _TL8D;_TL8C: _TL8D:;}
# 531
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;unsigned _T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_Port_Cfield*_T8;struct _fat_ptr*_T9;struct Cyc_Port_Cfield*_TA;struct _fat_ptr*_TB;int _TC;struct Cyc_List_List*_TD;struct Cyc_Port_Cfield*_TE;void*_TF;struct Cyc_Port_Cfield*_T10;void*_T11;struct Cyc_Port_Cfield*_T12;void*_T13;struct Cyc_Port_Cfield*_T14;void*_T15;struct Cyc_List_List*_T16;int _T17;int _T18;struct Cyc_List_List*_T19;struct Cyc_Port_Unify_ct_exn_struct*_T1A;struct Cyc_Port_Unify_ct_exn_struct*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;unsigned _T1E;struct Cyc_List_List*_T1F;void*_T20;struct Cyc_List_List*_T21;unsigned _T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Port_Cfield*_T25;struct _fat_ptr*_T26;struct Cyc_Port_Cfield*_T27;struct _fat_ptr*_T28;int _T29;struct Cyc_List_List*_T2A;int _T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;
# 533
struct Cyc_List_List*common=0;{
struct Cyc_List_List*xs2=fs2;_TL93: _T0=xs2;_T1=(unsigned)_T0;if(_T1)goto _TL91;else{goto _TL92;}
_TL91: _T2=xs2;_T3=_T2->hd;{struct Cyc_Port_Cfield*f2=(struct Cyc_Port_Cfield*)_T3;
int found=0;{
struct Cyc_List_List*xs1=fs1;_TL97: _T4=xs1;_T5=(unsigned)_T4;if(_T5)goto _TL95;else{goto _TL96;}
_TL95: _T6=xs1;_T7=_T6->hd;{struct Cyc_Port_Cfield*f1=(struct Cyc_Port_Cfield*)_T7;_T8=f1;_T9=_T8->f;_TA=f2;_TB=_TA->f;_TC=
Cyc_strptrcmp(_T9,_TB);if(_TC!=0)goto _TL98;{struct Cyc_List_List*_T2F=_cycalloc(sizeof(struct Cyc_List_List));
_T2F->hd=f1;_T2F->tl=common;_TD=(struct Cyc_List_List*)_T2F;}common=_TD;_TE=f1;_TF=_TE->qual;_T10=f2;_T11=_T10->qual;
Cyc_Port_unify_ct(_TF,_T11);_T12=f1;_T13=_T12->type;_T14=f2;_T15=_T14->type;
Cyc_Port_unify_ct(_T13,_T15);
found=1;goto _TL96;_TL98:;}_T16=xs1;
# 537
xs1=_T16->tl;goto _TL97;_TL96:;}_T17=found;
# 547
if(_T17)goto _TL9A;else{goto _TL9C;}
_TL9C: _T18=allow_f1_subset_f2;if(!_T18)goto _TL9D;{struct Cyc_List_List*_T2F=_cycalloc(sizeof(struct Cyc_List_List));
_T2F->hd=f2;_T2F->tl=common;_T19=(struct Cyc_List_List*)_T2F;}common=_T19;goto _TL9E;
_TL9D: _T1A=& Cyc_Port_Unify_ct_val;_T1B=(struct Cyc_Port_Unify_ct_exn_struct*)_T1A;_throw(_T1B);_TL9E: goto _TL9B;_TL9A: _TL9B:;}_T1C=xs2;
# 534
xs2=_T1C->tl;goto _TL93;_TL92:;}{
# 553
struct Cyc_List_List*xs1=fs1;_TLA2: _T1D=xs1;_T1E=(unsigned)_T1D;if(_T1E)goto _TLA0;else{goto _TLA1;}
_TLA0: _T1F=xs1;_T20=_T1F->hd;{struct Cyc_Port_Cfield*f1=(struct Cyc_Port_Cfield*)_T20;
int found=0;{
struct Cyc_List_List*xs2=fs2;_TLA6: _T21=xs2;_T22=(unsigned)_T21;if(_T22)goto _TLA4;else{goto _TLA5;}
_TLA4: _T23=xs2;_T24=_T23->hd;{struct Cyc_Port_Cfield*f2=(struct Cyc_Port_Cfield*)_T24;_T25=f1;_T26=_T25->f;_T27=f2;_T28=_T27->f;_T29=
Cyc_strptrcmp(_T26,_T28);if(!_T29)goto _TLA7;found=1;goto _TLA8;_TLA7: _TLA8:;}_T2A=xs2;
# 556
xs2=_T2A->tl;goto _TLA6;_TLA5:;}_T2B=found;
# 560
if(_T2B)goto _TLA9;else{goto _TLAB;}
_TLAB:{struct Cyc_List_List*_T2F=_cycalloc(sizeof(struct Cyc_List_List));_T2F->hd=f1;_T2F->tl=common;_T2C=(struct Cyc_List_List*)_T2F;}common=_T2C;goto _TLAA;_TLA9: _TLAA:;}_T2D=xs1;
# 553
xs1=_T2D->tl;goto _TLA2;_TLA1:;}_T2E=common;
# 563
return _T2E;}
# 566
static int Cyc_Port_unifies(void*t1,void*t2){struct _handler_cons*_T0;int _T1;void*_T2;struct Cyc_Port_Unify_ct_exn_struct*_T3;char*_T4;char*_T5;{struct _handler_cons _T6;_T0=& _T6;_push_handler(_T0);{int _T7=0;_T1=setjmp(_T6.handler);if(!_T1)goto _TLAC;_T7=1;goto _TLAD;_TLAC: _TLAD: if(_T7)goto _TLAE;else{goto _TLB0;}_TLB0:
# 573
 Cyc_Port_unify_ct(t1,t2);_pop_handler();goto _TLAF;_TLAE: _T2=Cyc_Core_get_exn_thrown();{void*_T8=(void*)_T2;void*_T9;_T3=(struct Cyc_Port_Unify_ct_exn_struct*)_T8;_T4=_T3->tag;_T5=Cyc_Port_Unify_ct;if(_T4!=_T5)goto _TLB1;
# 580
return 0;_TLB1: _T9=_T8;{void*exn=_T9;_rethrow(exn);};}_TLAF:;}}
# 582
return 1;}struct _tuple13{void*f0;void*f1;void*f2;void*f3;void*f4;};
# 588
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){void*_T0;int*_T1;unsigned _T2;struct Cyc_List_List**_T3;struct Cyc_List_List**_T4;struct Cyc_List_List*_T5;struct Cyc_List_List**_T6;struct Cyc_List_List*_T7;struct Cyc_List_List**_T8;struct Cyc_List_List*_T9;unsigned _TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_List_List**_TD;struct Cyc_List_List*_TE;struct _tuple12 _TF;void*_T10;int*_T11;unsigned _T12;void*_T13;int*_T14;unsigned _T15;struct Cyc_List_List**_T16;struct Cyc_List_List*_T17;void*_T18;int*_T19;int _T1A;void*_T1B;void*_T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;struct _tuple13 _T27;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T28;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T29;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2A;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2B;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2C;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2D;struct Cyc_List_List*_T2E;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2F;void*_T30;struct Cyc_List_List**_T31;struct Cyc_List_List*_T32;struct Cyc_List_List*_T33;struct Cyc_List_List*_T34;struct Cyc_List_List**_T35;
# 590
t=Cyc_Port_compress_ct(t);_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 1: goto _LL4;case 8: _LL4: goto _LL6;case 5: _LL6: goto _LL8;case 2: _LL8: goto _LLA;case 3: _LLA: goto _LLC;case 12: _LLC: goto _LLE;case 13: _LLE: goto _LL10;case 15: _LL10: goto _LL12;case 7: _LL12: _T3=uppers;
# 605
*_T3=0;
Cyc_Port_unifies(v,t);_T4=uppers;_T5=*_T4;
return _T5;case 4: goto _LL16;case 0: _LL16: goto _LL18;case 9: _LL18: _T6=uppers;_T7=*_T6;
# 613
return _T7;default: goto _LL0;}_LL0: _T8=uppers;{
# 617
struct Cyc_List_List*us=*_T8;_TLB7: _T9=us;_TA=(unsigned)_T9;if(_TA)goto _TLB5;else{goto _TLB6;}
_TLB5: _TB=us;_TC=_TB->hd;{void*t2=Cyc_Port_compress_ct(_TC);
# 620
if(t!=t2)goto _TLB8;_TD=uppers;_TE=*_TD;return _TE;_TLB8:{struct _tuple12 _T36;
_T36.f0=t;_T36.f1=t2;_TF=_T36;}{struct _tuple12 _T36=_TF;void*_T37;void*_T38;void*_T39;void*_T3A;void*_T3B;void*_T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;_T10=_T36.f0;_T11=(int*)_T10;_T12=*_T11;switch(_T12){case 6: _T13=_T36.f1;_T14=(int*)_T13;_T15=*_T14;switch(_T15){case 11: goto _LL1F;case 5: _LL1F: goto _LL21;case 12: _LL21: _T16=uppers;_T17=*_T16;
# 628
return _T17;default: goto _LL24;};case 11: _T18=_T36.f1;_T19=(int*)_T18;_T1A=*_T19;if(_T1A!=11)goto _TLBC;_T1B=_T36.f0;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T41=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T1B;_T1C=_T41->f1;_T40=(void*)_T1C;_T1D=_T41->f2;_T3F=(void*)_T1D;_T1E=_T41->f3;_T3E=(void*)_T1E;_T1F=_T41->f4;_T3D=(void*)_T1F;_T20=_T41->f5;_T3C=(void*)_T20;}_T21=_T36.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T41=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T21;_T22=_T41->f1;_T3B=(void*)_T22;_T23=_T41->f2;_T3A=(void*)_T23;_T24=_T41->f3;_T39=(void*)_T24;_T25=_T41->f4;_T38=(void*)_T25;_T26=_T41->f5;_T37=(void*)_T26;}{void*ta=_T40;void*qa=_T3F;void*ka=_T3E;void*ra=_T3D;void*za=_T3C;void*tb=_T3B;void*qb=_T3A;void*kb=_T39;void*rb=_T38;void*zb=_T37;{struct _tuple13 _T41;
# 633
_T41.f0=Cyc_Port_var();_T41.f1=Cyc_Port_var();_T41.f2=Cyc_Port_var();_T41.f3=Cyc_Port_var();_T41.f4=Cyc_Port_var();_T27=_T41;}{struct _tuple13 _T41=_T27;void*_T42;void*_T43;void*_T44;void*_T45;void*_T46;_T46=_T41.f0;_T45=_T41.f1;_T44=_T41.f2;_T43=_T41.f3;_T42=_T41.f4;{void*tc=_T46;void*qc=_T45;void*kc=_T44;void*rc=_T43;void*zc=_T42;
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*p;p=_cycalloc(sizeof(struct Cyc_Port_Ptr_ct_Port_Ctype_struct));_T28=p;_T28->tag=11;_T29=p;_T29->f1=tc;_T2A=p;_T2A->f2=qc;_T2B=p;_T2B->f3=kc;_T2C=p;_T2C->f4=rc;_T2D=p;_T2D->f5=zc;
Cyc_Port_leq(tc,ta);Cyc_Port_leq(tc,tb);
Cyc_Port_leq(qc,qa);Cyc_Port_leq(qc,qb);
Cyc_Port_leq(kc,ka);Cyc_Port_leq(kc,qb);
Cyc_Port_leq(rc,ra);Cyc_Port_leq(rc,rb);
Cyc_Port_leq(zc,za);Cyc_Port_leq(zc,zb);_T2E=us;_T2F=p;_T30=(void*)_T2F;
_T2E->hd=(void*)_T30;_T31=uppers;_T32=*_T31;
return _T32;}}}_TLBC: goto _LL24;default: _LL24: goto _LL1B;}_LL1B:;}}_T33=us;
# 617
us=_T33->tl;goto _TLB7;_TLB6:;}{struct Cyc_List_List*_T36=_cycalloc(sizeof(struct Cyc_List_List));
# 645
_T36->hd=t;_T35=uppers;_T36->tl=*_T35;_T34=(struct Cyc_List_List*)_T36;}return _T34;}
# 650
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){void*_T0;int*_T1;unsigned _T2;struct Cyc_List_List**_T3;struct Cyc_List_List**_T4;struct Cyc_List_List*_T5;struct Cyc_List_List**_T6;struct Cyc_List_List*_T7;struct Cyc_List_List**_T8;struct Cyc_List_List*_T9;unsigned _TA;struct Cyc_List_List*_TB;void*_TC;struct Cyc_List_List**_TD;struct Cyc_List_List*_TE;struct _tuple12 _TF;void*_T10;int*_T11;int _T12;void*_T13;int*_T14;unsigned _T15;void*_T16;int*_T17;unsigned _T18;void*_T19;int*_T1A;unsigned _T1B;void*_T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;void*_T25;void*_T26;void*_T27;struct _tuple13 _T28;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T29;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2A;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2B;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2C;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2D;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2E;struct Cyc_List_List*_T2F;struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T30;void*_T31;struct Cyc_List_List**_T32;struct Cyc_List_List*_T33;void*_T34;int*_T35;int _T36;struct Cyc_List_List**_T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;struct Cyc_List_List*_T3A;struct Cyc_List_List**_T3B;
# 652
t=Cyc_Port_compress_ct(t);_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: goto _LL4;case 8: _LL4: goto _LL6;case 2: _LL6: goto _LL8;case 3: _LL8: goto _LLA;case 4: _LLA: goto _LLC;case 13: _LLC: goto _LLE;case 15: _LLE: goto _LL10;case 10: _LL10: _T3=lowers;
# 662
*_T3=0;
Cyc_Port_unifies(v,t);_T4=lowers;_T5=*_T4;
return _T5;case 7: goto _LL14;case 1: _LL14: goto _LL16;case 9: _LL16: _T6=lowers;_T7=*_T6;
# 668
return _T7;default: goto _LL0;}_LL0: _T8=lowers;{
# 672
struct Cyc_List_List*ls=*_T8;_TLC2: _T9=ls;_TA=(unsigned)_T9;if(_TA)goto _TLC0;else{goto _TLC1;}
_TLC0: _TB=ls;_TC=_TB->hd;{void*t2=Cyc_Port_compress_ct(_TC);
if(t!=t2)goto _TLC3;_TD=lowers;_TE=*_TD;return _TE;_TLC3:{struct _tuple12 _T3C;
_T3C.f0=t;_T3C.f1=t2;_TF=_T3C;}{struct _tuple12 _T3C=_TF;void*_T3D;void*_T3E;void*_T3F;void*_T40;void*_T41;void*_T42;void*_T43;void*_T44;void*_T45;void*_T46;_T10=_T3C.f1;_T11=(int*)_T10;_T12=*_T11;if(_T12!=4)goto _TLC5;goto _LL1D;_TLC5: _T13=_T3C.f0;_T14=(int*)_T13;_T15=*_T14;switch(_T15){case 5: _T16=_T3C.f1;_T17=(int*)_T16;_T18=*_T17;switch(_T18){case 6: _LL1D: goto _LL1F;case 11: _LL1F: goto _LL21;default: goto _LL26;};case 11: _T19=_T3C.f1;_T1A=(int*)_T19;_T1B=*_T1A;switch(_T1B){case 6: _LL21: goto _LL23;case 11: _T1C=_T3C.f0;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T47=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T1C;_T1D=_T47->f1;_T46=(void*)_T1D;_T1E=_T47->f2;_T45=(void*)_T1E;_T1F=_T47->f3;_T44=(void*)_T1F;_T20=_T47->f4;_T43=(void*)_T20;_T21=_T47->f5;_T42=(void*)_T21;}_T22=_T3C.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T47=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T22;_T23=_T47->f1;_T41=(void*)_T23;_T24=_T47->f2;_T40=(void*)_T24;_T25=_T47->f3;_T3F=(void*)_T25;_T26=_T47->f4;_T3E=(void*)_T26;_T27=_T47->f5;_T3D=(void*)_T27;}{void*ta=_T46;void*qa=_T45;void*ka=_T44;void*ra=_T43;void*za=_T42;void*tb=_T41;void*qb=_T40;void*kb=_T3F;void*rb=_T3E;void*zb=_T3D;{struct _tuple13 _T47;
# 686
_T47.f0=Cyc_Port_var();_T47.f1=Cyc_Port_var();_T47.f2=Cyc_Port_var();_T47.f3=Cyc_Port_var();_T47.f4=Cyc_Port_var();_T28=_T47;}{struct _tuple13 _T47=_T28;void*_T48;void*_T49;void*_T4A;void*_T4B;void*_T4C;_T4C=_T47.f0;_T4B=_T47.f1;_T4A=_T47.f2;_T49=_T47.f3;_T48=_T47.f4;{void*tc=_T4C;void*qc=_T4B;void*kc=_T4A;void*rc=_T49;void*zc=_T48;
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*p;p=_cycalloc(sizeof(struct Cyc_Port_Ptr_ct_Port_Ctype_struct));_T29=p;_T29->tag=11;_T2A=p;_T2A->f1=tc;_T2B=p;_T2B->f2=qc;_T2C=p;_T2C->f3=kc;_T2D=p;_T2D->f4=rc;_T2E=p;_T2E->f5=zc;
Cyc_Port_leq(ta,tc);Cyc_Port_leq(tb,tc);
Cyc_Port_leq(qa,qc);Cyc_Port_leq(qb,qc);
Cyc_Port_leq(ka,kc);Cyc_Port_leq(qb,kc);
Cyc_Port_leq(ra,rc);Cyc_Port_leq(rb,rc);
Cyc_Port_leq(za,zc);Cyc_Port_leq(zb,zc);_T2F=ls;_T30=p;_T31=(void*)_T30;
_T2F->hd=(void*)_T31;_T32=lowers;_T33=*_T32;
return _T33;}}}default: goto _LL26;};case 12: _T34=_T3C.f1;_T35=(int*)_T34;_T36=*_T35;if(_T36!=6)goto _TLCA;_LL23: _T37=lowers;_T38=*_T37;
# 681
return _T38;_TLCA: goto _LL26;default: _LL26: goto _LL19;}_LL19:;}}_T39=ls;
# 672
ls=_T39->tl;goto _TLC2;_TLC1:;}{struct Cyc_List_List*_T3C=_cycalloc(sizeof(struct Cyc_List_List));
# 698
_T3C->hd=t;_T3B=lowers;_T3C->tl=*_T3B;_T3A=(struct Cyc_List_List*)_T3C;}return _T3A;}
# 705
static int Cyc_Port_leq(void*t1,void*t2){struct _tuple12 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;unsigned _T6;void*_T7;struct _fat_ptr*_T8;void*_T9;struct _fat_ptr*_TA;int _TB;int _TC;void*_TD;struct _fat_ptr*_TE;void*_TF;int*_T10;unsigned _T11;void*_T12;int*_T13;unsigned _T14;void*_T15;int*_T16;unsigned _T17;void*_T18;int*_T19;unsigned _T1A;void*_T1B;int*_T1C;unsigned _T1D;void*_T1E;void*_T1F;struct Cyc_Port_Cvar*_T20;void*_T21;struct Cyc_Port_Cvar*_T22;struct Cyc_List_List*_T23;struct Cyc_Port_Cvar*_T24;void*_T25;struct Cyc_Port_Cvar*_T26;struct Cyc_List_List*_T27;struct Cyc_Port_Cvar*_T28;void*_T29;void*_T2A;struct Cyc_Port_Cvar*_T2B;struct Cyc_List_List**_T2C;struct Cyc_Port_Cvar*_T2D;void*_T2E;void*_T2F;struct Cyc_Port_Cvar*_T30;struct Cyc_List_List**_T31;void*_T32;struct Cyc_Port_Cvar*_T33;void*_T34;struct Cyc_Port_Cvar*_T35;struct Cyc_List_List*_T36;struct Cyc_Port_Cvar*_T37;void*_T38;void*_T39;struct Cyc_Port_Cvar*_T3A;struct Cyc_List_List**_T3B;void*_T3C;int*_T3D;unsigned _T3E;void*_T3F;int*_T40;unsigned _T41;void*_T42;void*_T43;void*_T44;void*_T45;void*_T46;void*_T47;void*_T48;void*_T49;void*_T4A;void*_T4B;void*_T4C;void*_T4D;int _T4E;int _T4F;int _T50;int _T51;int _T52;void*_T53;int*_T54;unsigned _T55;void*_T56;void*_T57;void*_T58;void*_T59;void*_T5A;void*_T5B;void*_T5C;void*_T5D;int _T5E;int _T5F;int _T60;void*_T61;void*_T62;void*_T63;void*_T64;void*_T65;void*_T66;void*_T67;void*_T68;void*_T69;int _T6A;int _T6B;int _T6C;struct Cyc_Port_Fat_ct_Port_Ctype_struct*_T6D;struct Cyc_Port_Fat_ct_Port_Ctype_struct*_T6E;void*_T6F;void*_T70;int _T71;void*_T72;int*_T73;int _T74;void*_T75;struct Cyc_Port_Cvar*_T76;void*_T77;struct Cyc_Port_Cvar*_T78;struct Cyc_List_List*_T79;struct Cyc_Port_Cvar*_T7A;void*_T7B;void*_T7C;struct Cyc_Port_Cvar*_T7D;struct Cyc_List_List**_T7E;int _T7F;
# 711
if(t1!=t2)goto _TLCC;return 1;_TLCC:
 t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);{struct _tuple12 _T80;
_T80.f0=t1;_T80.f1=t2;_T0=_T80;}{struct _tuple12 _T80=_T0;struct Cyc_Port_Cvar*_T81;void*_T82;void*_T83;void*_T84;void*_T85;void*_T86;void*_T87;void*_T88;void*_T89;void*_T8A;void*_T8B;struct _fat_ptr _T8C;struct _fat_ptr _T8D;_T1=_T80.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 7:
 return 1;case 10: _T4=_T80.f1;_T5=(int*)_T4;_T6=*_T5;switch(_T6){case 10: _T7=_T80.f0;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_T7;_T8=_T8E->f1;{struct _fat_ptr _T8F=*_T8;_T8D=_T8F;}}_T9=_T80.f1;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_T9;_TA=_T8E->f1;{struct _fat_ptr _T8F=*_TA;_T8C=_T8F;}}{struct _fat_ptr n1=_T8D;struct _fat_ptr n2=_T8C;_TB=
Cyc_strcmp(n1,n2);_TC=_TB==0;return _TC;}case 7: _TD=_T80.f0;{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_TD;_TE=_T8E->f1;{struct _fat_ptr _T8F=*_TE;_T8D=_T8F;}}{struct _fat_ptr n1=_T8D;
return 0;}case 16: goto _LL2D;default: goto _LL2F;};case 1: _TF=_T80.f1;_T10=(int*)_TF;_T11=*_T10;switch(_T11){case 0:
 return 1;case 16: goto _LL2D;default: goto _LL2F;};case 0: _T12=_T80.f1;_T13=(int*)_T12;_T14=*_T13;switch(_T14){case 1:
 return 0;case 16: goto _LL2D;default: goto _LL2F;};case 9: _T15=_T80.f1;_T16=(int*)_T15;_T17=*_T16;switch(_T17){case 8:
 return 0;case 16: goto _LL2D;default: goto _LL2F;};case 8: _T18=_T80.f1;_T19=(int*)_T18;_T1A=*_T19;switch(_T1A){case 9:
 return 1;case 16: goto _LL2D;default: goto _LL2F;};case 16: _T1B=_T80.f1;_T1C=(int*)_T1B;_T1D=*_T1C;switch(_T1D){case 0:
 return 1;case 4:
 return 1;case 16: _T1E=_T80.f0;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T1E;_T8B=_T8E->f1;}_T1F=_T80.f1;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T1F;_T8A=_T8E->f1;}{struct Cyc_Port_Cvar*v1=_T8B;struct Cyc_Port_Cvar*v2=_T8A;_T20=v1;_T21=t1;_T22=v1;_T23=_T22->uppers;
# 741
_T20->uppers=Cyc_Port_filter_self(_T21,_T23);_T24=v2;_T25=t2;_T26=v2;_T27=_T26->lowers;
_T24->lowers=Cyc_Port_filter_self(_T25,_T27);_T28=v1;_T29=t1;_T2A=t2;_T2B=v1;_T2C=& _T2B->uppers;
_T28->uppers=Cyc_Port_insert_upper(_T29,_T2A,_T2C);_T2D=v2;_T2E=t2;_T2F=t1;_T30=v2;_T31=& _T30->lowers;
_T2D->lowers=Cyc_Port_insert_lower(_T2E,_T2F,_T31);
return 1;}default: _T32=_T80.f0;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T32;_T8B=_T8E->f1;}{struct Cyc_Port_Cvar*v1=_T8B;_T33=v1;_T34=t1;_T35=v1;_T36=_T35->uppers;
# 747
_T33->uppers=Cyc_Port_filter_self(_T34,_T36);_T37=v1;_T38=t1;_T39=t2;_T3A=v1;_T3B=& _T3A->uppers;
_T37->uppers=Cyc_Port_insert_upper(_T38,_T39,_T3B);
return 1;}};case 4:
# 724
 return 0;case 5: _T3C=_T80.f1;_T3D=(int*)_T3C;_T3E=*_T3D;switch(_T3E){case 6:
 return 1;case 11:
 return 1;case 4:
 return 1;case 16: goto _LL2D;default: goto _LL2F;};case 11: _T3F=_T80.f1;_T40=(int*)_T3F;_T41=*_T40;switch(_T41){case 6:
 return 1;case 4:
 return 1;case 11: _T42=_T80.f0;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T42;_T43=_T8E->f1;_T8B=(void*)_T43;_T44=_T8E->f2;_T8A=(void*)_T44;_T45=_T8E->f3;_T89=(void*)_T45;_T46=_T8E->f4;_T88=(void*)_T46;_T47=_T8E->f5;_T87=(void*)_T47;}_T48=_T80.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T48;_T49=_T8E->f1;_T86=(void*)_T49;_T4A=_T8E->f2;_T85=(void*)_T4A;_T4B=_T8E->f3;_T84=(void*)_T4B;_T4C=_T8E->f4;_T83=(void*)_T4C;_T4D=_T8E->f5;_T82=(void*)_T4D;}{void*t1=_T8B;void*q1=_T8A;void*k1=_T89;void*r1=_T88;void*z1=_T87;void*t2=_T86;void*q2=_T85;void*k2=_T84;void*r2=_T83;void*z2=_T82;_T4F=
# 733
Cyc_Port_leq(t1,t2);if(!_T4F)goto _TLD7;_T50=Cyc_Port_leq(q1,q2);if(!_T50)goto _TLD7;_T51=Cyc_Port_unifies(k1,k2);if(!_T51)goto _TLD7;_T52=Cyc_Port_leq(r1,r2);if(!_T52)goto _TLD7;_T4E=
Cyc_Port_leq(z1,z2);goto _TLD8;_TLD7: _T4E=0;_TLD8:
# 733
 return _T4E;}case 16: goto _LL2D;default: goto _LL2F;};case 12: _T53=_T80.f1;_T54=(int*)_T53;_T55=*_T54;switch(_T55){case 6:
# 730
 return 1;case 4:
 return 1;case 12: _T56=_T80.f0;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T56;_T57=_T8E->f1;_T8B=(void*)_T57;_T58=_T8E->f2;_T8A=(void*)_T58;_T59=_T8E->f3;_T89=(void*)_T59;}_T5A=_T80.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T5A;_T5B=_T8E->f1;_T88=(void*)_T5B;_T5C=_T8E->f2;_T87=(void*)_T5C;_T5D=_T8E->f3;_T86=(void*)_T5D;}{void*t1=_T8B;void*q1=_T8A;void*z1=_T89;void*t2=_T88;void*q2=_T87;void*z2=_T86;_T5F=
# 736
Cyc_Port_leq(t1,t2);if(!_T5F)goto _TLDA;_T60=Cyc_Port_leq(q1,q2);if(!_T60)goto _TLDA;_T5E=Cyc_Port_leq(z1,z2);goto _TLDB;_TLDA: _T5E=0;_TLDB: return _T5E;}case 11: _T61=_T80.f0;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T61;_T62=_T8E->f1;_T8B=(void*)_T62;_T63=_T8E->f2;_T8A=(void*)_T63;_T64=_T8E->f3;_T89=(void*)_T64;}_T65=_T80.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T65;_T66=_T8E->f1;_T88=(void*)_T66;_T67=_T8E->f2;_T87=(void*)_T67;_T68=_T8E->f3;_T86=(void*)_T68;_T69=_T8E->f5;_T85=(void*)_T69;}{void*t1=_T8B;void*q1=_T8A;void*z1=_T89;void*t2=_T88;void*q2=_T87;void*k2=_T86;void*z2=_T85;_T6B=
# 738
Cyc_Port_leq(t1,t2);if(!_T6B)goto _TLDC;_T6C=Cyc_Port_leq(q1,q2);if(!_T6C)goto _TLDC;_T6D=& Cyc_Port_Fat_ct_val;_T6E=(struct Cyc_Port_Fat_ct_Port_Ctype_struct*)_T6D;_T6F=(void*)_T6E;_T70=k2;_T71=Cyc_Port_unifies(_T6F,_T70);if(!_T71)goto _TLDC;_T6A=
Cyc_Port_leq(z1,z2);goto _TLDD;_TLDC: _T6A=0;_TLDD:
# 738
 return _T6A;}case 16: goto _LL2D;default: goto _LL2F;};default: _T72=_T80.f1;_T73=(int*)_T72;_T74=*_T73;if(_T74!=16)goto _TLDE;_LL2D: _T75=_T80.f1;{struct Cyc_Port_Var_ct_Port_Ctype_struct*_T8E=(struct Cyc_Port_Var_ct_Port_Ctype_struct*)_T75;_T81=_T8E->f1;}{struct Cyc_Port_Cvar*v2=_T81;_T76=v2;_T77=t2;_T78=v2;_T79=_T78->lowers;
# 751
_T76->lowers=Cyc_Port_filter_self(_T77,_T79);_T7A=v2;_T7B=t2;_T7C=t1;_T7D=v2;_T7E=& _T7D->lowers;
_T7A->lowers=Cyc_Port_insert_lower(_T7B,_T7C,_T7E);
return 1;}_TLDE: _LL2F: _T7F=
Cyc_Port_unifies(t1,t2);return _T7F;};}}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*vardecl_locs;struct Cyc_Hashtable_Table*varusage_tab;struct Cyc_List_List*edits;int porting;};
# 785
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos =0U,Cyc_Port_FnArg_pos =1U,Cyc_Port_FnBody_pos =2U,Cyc_Port_Toplevel_pos =3U};struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 798
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv (void){struct Cyc_Port_GlobalCenv*_T0;struct Cyc_Dict_Dict(*_T1)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T2)(int(*)(void*,void*));int(*_T3)(struct _tuple0*,struct _tuple0*);struct Cyc_Port_GlobalCenv*_T4;struct Cyc_Dict_Dict(*_T5)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T6)(int(*)(void*,void*));int(*_T7)(struct _tuple0*,struct _tuple0*);struct Cyc_Port_GlobalCenv*_T8;struct Cyc_Dict_Dict(*_T9)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_TA)(int(*)(void*,void*));int(*_TB)(struct _tuple0*,struct _tuple0*);struct Cyc_Port_GlobalCenv*_TC;struct Cyc_Port_GlobalCenv*_TD;struct Cyc_Port_GlobalCenv*_TE;struct Cyc_Port_GlobalCenv*_TF;struct Cyc_Port_GlobalCenv*_T10;struct Cyc_Hashtable_Table*(*_T11)(int,int(*)(unsigned,unsigned),int(*)(unsigned));struct Cyc_Hashtable_Table*(*_T12)(int,int(*)(void*,void*),int(*)(void*));struct Cyc_Port_GlobalCenv*_T13;struct Cyc_Port_GlobalCenv*_T14;struct Cyc_Port_GlobalCenv*_T15;struct Cyc_Port_Cenv*_T16;struct Cyc_Dict_Dict(*_T17)(int(*)(struct _tuple0*,struct _tuple0*));struct Cyc_Dict_Dict(*_T18)(int(*)(void*,void*));int(*_T19)(struct _tuple0*,struct _tuple0*);
struct Cyc_Port_GlobalCenv*g;g=_cycalloc(sizeof(struct Cyc_Port_GlobalCenv));_T0=g;_T2=Cyc_Dict_empty;{struct Cyc_Dict_Dict(*_T1A)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T2;_T1=_T1A;}_T3=Cyc_Absyn_qvar_cmp;_T0->typedef_dict=_T1(_T3);_T4=g;_T6=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T1A)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T6;_T5=_T1A;}_T7=Cyc_Absyn_qvar_cmp;_T4->struct_dict=_T5(_T7);_T8=g;_TA=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T1A)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_TA;_T9=_T1A;}_TB=Cyc_Absyn_qvar_cmp;_T8->union_dict=_T9(_TB);_TC=g;
_TC->qualifier_edits=0;_TD=g;
_TD->pointer_edits=0;_TE=g;
_TE->zeroterm_edits=0;_TF=g;
_TF->vardecl_locs=0;_T10=g;_T12=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T1A)(int,int(*)(unsigned,unsigned),int(*)(unsigned))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(unsigned,unsigned),int(*)(unsigned)))_T12;_T11=_T1A;}_T10->varusage_tab=_T11(128,Cyc_Port_cmp_seg_t,Cyc_Port_hash_seg_t);_T13=g;
_T13->edits=0;_T14=g;
_T14->porting=0;_T15=g;
_T15->return_type=Cyc_Port_void_ct();{struct Cyc_Port_Cenv*_T1A=_cycalloc(sizeof(struct Cyc_Port_Cenv));
_T1A->gcenv=g;
_T1A->cpos=3U;_T18=Cyc_Dict_empty;{
struct Cyc_Dict_Dict(*_T1B)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))_T18;_T17=_T1B;}_T19=Cyc_Absyn_qvar_cmp;_T1A->var_dict=_T17(_T19);_T16=(struct Cyc_Port_Cenv*)_T1A;}
# 810
return _T16;}
# 818
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){struct Cyc_Port_Cenv*_T0;enum Cyc_Port_CPos _T1;int _T2;int _T3;_T0=env;_T1=_T0->cpos;_T2=(int)_T1;_T3=_T2==1;
return _T3;}
# 821
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){struct Cyc_Port_Cenv*_T0;enum Cyc_Port_CPos _T1;int _T2;int _T3;_T0=env;_T1=_T0->cpos;_T2=(int)_T1;_T3=_T2==0;
return _T3;}
# 824
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){struct Cyc_Port_Cenv*_T0;enum Cyc_Port_CPos _T1;int _T2;int _T3;_T0=env;_T1=_T0->cpos;_T2=(int)_T1;_T3=_T2==3;
return _T3;}
# 827
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_GlobalCenv*_T1;void*_T2;_T0=env;_T1=_T0->gcenv;_T2=_T1->return_type;
return _T2;}
# 830
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _handler_cons*_T0;int _T1;struct _tuple12*(*_T2)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T3)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T4;struct Cyc_Port_GlobalCenv*_T5;struct Cyc_Dict_Dict _T6;struct _tuple0*_T7;struct _tuple12*_T8;void*_T9;struct Cyc_Dict_Absent_exn_struct*_TA;char*_TB;char*_TC;void*_TD;struct _handler_cons _TE;_T0=& _TE;_push_handler(_T0);{int _TF=0;_T1=setjmp(_TE.handler);if(!_T1)goto _TLE0;_TF=1;goto _TLE1;_TLE0: _TLE1: if(_TF)goto _TLE2;else{goto _TLE4;}_TLE4: _T3=Cyc_Dict_lookup;{
# 832
struct _tuple12*(*_T10)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple12*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T3;_T2=_T10;}_T4=env;_T5=_T4->gcenv;_T6=_T5->typedef_dict;_T7=n;_T8=_T2(_T6,_T7);{struct _tuple12 _T10=*_T8;void*_T11;_T11=_T10.f0;{void*t=_T11;void*_T12=t;_npop_handler(0);return _T12;}}_pop_handler();goto _TLE3;_TLE2: _T9=Cyc_Core_get_exn_thrown();{void*_T10=(void*)_T9;void*_T11;_TA=(struct Cyc_Dict_Absent_exn_struct*)_T10;_TB=_TA->tag;_TC=Cyc_Dict_Absent;if(_TB!=_TC)goto _TLE5;_TD=Cyc_Absyn_sint_type;
# 839
return _TD;_TLE5: _T11=_T10;{void*exn=_T11;_rethrow(exn);};}_TLE3:;}}
# 844
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _handler_cons*_T0;int _T1;struct _tuple12*(*_T2)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T3)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T4;struct Cyc_Port_GlobalCenv*_T5;struct Cyc_Dict_Dict _T6;struct _tuple0*_T7;struct _tuple12*_T8;void*_T9;struct Cyc_Dict_Absent_exn_struct*_TA;char*_TB;char*_TC;void*_TD;struct _handler_cons _TE;_T0=& _TE;_push_handler(_T0);{int _TF=0;_T1=setjmp(_TE.handler);if(!_T1)goto _TLE7;_TF=1;goto _TLE8;_TLE7: _TLE8: if(_TF)goto _TLE9;else{goto _TLEB;}_TLEB: _T3=Cyc_Dict_lookup;{
# 846
struct _tuple12*(*_T10)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple12*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T3;_T2=_T10;}_T4=env;_T5=_T4->gcenv;_T6=_T5->typedef_dict;_T7=n;_T8=_T2(_T6,_T7);{struct _tuple12 _T10=*_T8;void*_T11;_T11=_T10.f1;{void*ct=_T11;void*_T12=ct;_npop_handler(0);return _T12;}}_pop_handler();goto _TLEA;_TLE9: _T9=Cyc_Core_get_exn_thrown();{void*_T10=(void*)_T9;void*_T11;_TA=(struct Cyc_Dict_Absent_exn_struct*)_T10;_TB=_TA->tag;_TC=Cyc_Dict_Absent;if(_TB!=_TC)goto _TLEC;_TD=
# 853
Cyc_Port_var();return _TD;_TLEC: _T11=_T10;{void*exn=_T11;_rethrow(exn);};}_TLEA:;}}
# 859
static struct _tuple11*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _tuple11**(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);void**(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T2;struct Cyc_Port_GlobalCenv*_T3;struct Cyc_Dict_Dict _T4;struct _tuple0*_T5;struct _tuple11**_T6;unsigned _T7;struct _tuple11**_T8;struct _tuple11*_T9;struct Cyc_Absyn_Aggrdecl*_TA;struct Cyc_Absyn_Aggrdecl*_TB;struct Cyc_Absyn_Aggrdecl*_TC;struct Cyc_Absyn_Aggrdecl*_TD;struct Cyc_Absyn_Aggrdecl*_TE;struct Cyc_Absyn_Aggrdecl*_TF;struct Cyc_Absyn_Aggrdecl*_T10;struct _tuple11*_T11;struct _tuple11*_T12;struct Cyc_Port_Cenv*_T13;struct Cyc_Port_GlobalCenv*_T14;struct Cyc_Dict_Dict(*_T15)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*);struct Cyc_Dict_Dict(*_T16)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Port_Cenv*_T17;struct Cyc_Port_GlobalCenv*_T18;struct Cyc_Dict_Dict _T19;struct _tuple0*_T1A;struct _tuple11*_T1B;struct _tuple11*_T1C;_T1=Cyc_Dict_lookup_opt;{
# 861
struct _tuple11**(*_T1D)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple11**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_T1D;}_T2=env;_T3=_T2->gcenv;_T4=_T3->struct_dict;_T5=n;{struct _tuple11**popt=_T0(_T4,_T5);_T6=popt;_T7=(unsigned)_T6;
if(!_T7)goto _TLEE;_T8=popt;_T9=*_T8;
return _T9;
# 865
_TLEE: {struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_TA=ad;_TA->kind=0U;_TB=ad;_TB->sc=2U;_TC=ad;_TC->name=n;_TD=ad;
_TD->tvs=0;_TE=ad;_TE->impl=0;_TF=ad;_TF->attributes=0;_T10=ad;
_T10->expected_mem_kind=0;{
struct _tuple11*p;p=_cycalloc(sizeof(struct _tuple11));_T11=p;_T11->f0=ad;_T12=p;_T12->f1=0;_T13=env;_T14=_T13->gcenv;_T16=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1D)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*))_T16;_T15=_T1D;}_T17=env;_T18=_T17->gcenv;_T19=_T18->struct_dict;_T1A=n;_T1B=p;_T14->struct_dict=_T15(_T19,_T1A,_T1B);_T1C=p;
return _T1C;}}}}
# 876
static struct _tuple11*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){struct _tuple11**(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);void**(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T2;struct Cyc_Port_GlobalCenv*_T3;struct Cyc_Dict_Dict _T4;struct _tuple0*_T5;struct _tuple11**_T6;unsigned _T7;struct _tuple11**_T8;struct _tuple11*_T9;struct Cyc_Absyn_Aggrdecl*_TA;struct Cyc_Absyn_Aggrdecl*_TB;struct Cyc_Absyn_Aggrdecl*_TC;struct Cyc_Absyn_Aggrdecl*_TD;struct Cyc_Absyn_Aggrdecl*_TE;struct Cyc_Absyn_Aggrdecl*_TF;struct Cyc_Absyn_Aggrdecl*_T10;struct _tuple11*_T11;struct _tuple11*_T12;struct Cyc_Port_Cenv*_T13;struct Cyc_Port_GlobalCenv*_T14;struct Cyc_Dict_Dict(*_T15)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*);struct Cyc_Dict_Dict(*_T16)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Port_Cenv*_T17;struct Cyc_Port_GlobalCenv*_T18;struct Cyc_Dict_Dict _T19;struct _tuple0*_T1A;struct _tuple11*_T1B;struct _tuple11*_T1C;_T1=Cyc_Dict_lookup_opt;{
# 878
struct _tuple11**(*_T1D)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple11**(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_T1D;}_T2=env;_T3=_T2->gcenv;_T4=_T3->union_dict;_T5=n;{struct _tuple11**popt=_T0(_T4,_T5);_T6=popt;_T7=(unsigned)_T6;
if(!_T7)goto _TLF0;_T8=popt;_T9=*_T8;
return _T9;
# 882
_TLF0: {struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_TA=ad;_TA->kind=1U;_TB=ad;_TB->sc=2U;_TC=ad;_TC->name=n;_TD=ad;
_TD->tvs=0;_TE=ad;_TE->impl=0;_TF=ad;_TF->attributes=0;_T10=ad;
_T10->expected_mem_kind=0;{
struct _tuple11*p;p=_cycalloc(sizeof(struct _tuple11));_T11=p;_T11->f0=ad;_T12=p;_T12->f1=0;_T13=env;_T14=_T13->gcenv;_T16=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T1D)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*))_T16;_T15=_T1D;}_T17=env;_T18=_T17->gcenv;_T19=_T18->union_dict;_T1A=n;_T1B=p;_T14->union_dict=_T15(_T19,_T1A,_T1B);_T1C=p;
return _T1C;}}}}struct _tuple14{struct _tuple12 f0;unsigned f1;};struct _tuple15{void*f0;struct _tuple12*f1;unsigned f2;};
# 892
static struct _tuple14 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons*_T0;int _T1;struct _tuple15*(*_T2)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T3)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T4;struct Cyc_Dict_Dict _T5;struct _tuple0*_T6;struct _tuple15*_T7;struct _tuple14 _T8;struct _tuple12*_T9;void*_TA;struct Cyc_Dict_Absent_exn_struct*_TB;char*_TC;char*_TD;struct _tuple14 _TE;struct _handler_cons _TF;_T0=& _TF;_push_handler(_T0);{int _T10=0;_T1=setjmp(_TF.handler);if(!_T1)goto _TLF2;_T10=1;goto _TLF3;_TLF2: _TLF3: if(_T10)goto _TLF4;else{goto _TLF6;}_TLF6: _T3=Cyc_Dict_lookup;{
# 894
struct _tuple15*(*_T11)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T3;_T2=_T11;}_T4=env;_T5=_T4->var_dict;_T6=x;_T7=_T2(_T5,_T6);{struct _tuple15 _T11=*_T7;unsigned _T12;struct _tuple12*_T13;_T13=_T11.f1;_T12=_T11.f2;{struct _tuple12*p=_T13;unsigned loc=_T12;{struct _tuple14 _T14;_T9=p;
_T14.f0=*_T9;_T14.f1=loc;_T8=_T14;}{struct _tuple14 _T14=_T8;_npop_handler(0);return _T14;}}}_pop_handler();goto _TLF5;_TLF4: _TA=Cyc_Core_get_exn_thrown();{void*_T11=(void*)_TA;void*_T12;_TB=(struct Cyc_Dict_Absent_exn_struct*)_T11;_TC=_TB->tag;_TD=Cyc_Dict_Absent;if(_TC!=_TD)goto _TLF7;{struct _tuple14 _T13;
# 901
_T13.f0.f0=Cyc_Port_var();_T13.f0.f1=Cyc_Port_var();_T13.f1=0U;_TE=_T13;}return _TE;_TLF7: _T12=_T11;{void*exn=_T12;_rethrow(exn);};}_TLF5:;}}
# 904
static struct _tuple15*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _tuple15*(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T2;struct Cyc_Dict_Dict _T3;struct _tuple0*_T4;struct _tuple15*_T5;_T1=Cyc_Dict_lookup;{
struct _tuple15*(*_T6)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_T6;}_T2=env;_T3=_T2->var_dict;_T4=x;_T5=_T0(_T3,_T4);return _T5;}
# 908
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){int(*_T0)(struct Cyc_Dict_Dict,struct _tuple0*);int(*_T1)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T2;struct Cyc_Dict_Dict _T3;struct _tuple0*_T4;int _T5;_T1=Cyc_Dict_member;{
int(*_T6)(struct Cyc_Dict_Dict,struct _tuple0*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T1;_T0=_T6;}_T2=env;_T3=_T2->var_dict;_T4=x;_T5=_T0(_T3,_T4);return _T5;}
# 912
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons*_T0;int _T1;struct _tuple15*(*_T2)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T3)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T4;struct Cyc_Dict_Dict _T5;struct _tuple0*_T6;struct _tuple15*_T7;void*_T8;int*_T9;unsigned _TA;void*_TB;void*_TC;struct Cyc_Dict_Absent_exn_struct*_TD;char*_TE;char*_TF;struct _handler_cons _T10;_T0=& _T10;_push_handler(_T0);{int _T11=0;_T1=setjmp(_T10.handler);if(!_T1)goto _TLF9;_T11=1;goto _TLFA;_TLF9: _TLFA: if(_T11)goto _TLFB;else{goto _TLFD;}_TLFD: _T3=Cyc_Dict_lookup;{
# 914
struct _tuple15*(*_T12)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T3;_T2=_T12;}_T4=env;_T5=_T4->var_dict;_T6=x;_T7=_T2(_T5,_T6);{struct _tuple15 _T12=*_T7;void*_T13;_T13=_T12.f0;{void*t=_T13;
LOOP: {struct _tuple0*_T14;_T8=t;_T9=(int*)_T8;_TA=*_T9;switch(_TA){case 8: _TB=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T15=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TB;_T14=_T15->f1;}{struct _tuple0*n=_T14;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 6:  {int _T15=1;_npop_handler(0);return _T15;}default:  {int _T15=0;_npop_handler(0);return _T15;}};}}}_pop_handler();goto _TLFC;_TLFB: _TC=Cyc_Core_get_exn_thrown();{void*_T12=(void*)_TC;void*_T13;_TD=(struct Cyc_Dict_Absent_exn_struct*)_T12;_TE=_TD->tag;_TF=Cyc_Dict_Absent;if(_TE!=_TF)goto _TLFF;
# 925
return 0;_TLFF: _T13=_T12;{void*exn=_T13;_rethrow(exn);};}_TLFC:;}}
# 929
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){struct _handler_cons*_T0;int _T1;struct _tuple15*(*_T2)(struct Cyc_Dict_Dict,struct _tuple0*);void*(*_T3)(struct Cyc_Dict_Dict,void*);struct Cyc_Port_Cenv*_T4;struct Cyc_Dict_Dict _T5;struct _tuple0*_T6;struct _tuple15*_T7;void*_T8;int*_T9;unsigned _TA;void*_TB;void*_TC;struct Cyc_Dict_Absent_exn_struct*_TD;char*_TE;char*_TF;struct _handler_cons _T10;_T0=& _T10;_push_handler(_T0);{int _T11=0;_T1=setjmp(_T10.handler);if(!_T1)goto _TL101;_T11=1;goto _TL102;_TL101: _TL102: if(_T11)goto _TL103;else{goto _TL105;}_TL105: _T3=Cyc_Dict_lookup;{
# 931
struct _tuple15*(*_T12)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))_T3;_T2=_T12;}_T4=env;_T5=_T4->var_dict;_T6=x;_T7=_T2(_T5,_T6);{struct _tuple15 _T12=*_T7;void*_T13;_T13=_T12.f0;{void*t=_T13;
LOOP: {struct _tuple0*_T14;_T8=t;_T9=(int*)_T8;_TA=*_T9;switch(_TA){case 8: _TB=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T15=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_TB;_T14=_T15->f1;}{struct _tuple0*n=_T14;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 5:  {int _T15=1;_npop_handler(0);return _T15;}default:  {int _T15=0;_npop_handler(0);return _T15;}};}}}_pop_handler();goto _TL104;_TL103: _TC=Cyc_Core_get_exn_thrown();{void*_T12=(void*)_TC;void*_T13;_TD=(struct Cyc_Dict_Absent_exn_struct*)_T12;_TE=_TD->tag;_TF=Cyc_Dict_Absent;if(_TE!=_TF)goto _TL107;
# 942
return 0;_TL107: _T13=_T12;{void*exn=_T13;_rethrow(exn);};}_TL104:;}}
# 948
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos cpos){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_Cenv*_T1;struct Cyc_Port_Cenv*_T2;{struct Cyc_Port_Cenv*_T3=_cycalloc(sizeof(struct Cyc_Port_Cenv));_T1=env;
_T3->gcenv=_T1->gcenv;_T2=env;_T3->var_dict=_T2->var_dict;_T3->cpos=cpos;_T0=(struct Cyc_Port_Cenv*)_T3;}return _T0;}
# 953
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_GlobalCenv*_T1;_T0=env;_T1=_T0->gcenv;
_T1->return_type=t;}
# 958
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*ctype,unsigned loc){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_Cenv*_T1;struct Cyc_Dict_Dict(*_T2)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple15*);struct Cyc_Dict_Dict(*_T3)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Port_Cenv*_T4;struct Cyc_Dict_Dict _T5;struct _tuple0*_T6;struct _tuple15*_T7;struct _tuple12*_T8;struct Cyc_Port_Cenv*_T9;{struct Cyc_Port_Cenv*_TA=_cycalloc(sizeof(struct Cyc_Port_Cenv));_T1=env;
# 960
_TA->gcenv=_T1->gcenv;_T3=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_TB)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple15*))_T3;_T2=_TB;}_T4=env;_T5=_T4->var_dict;_T6=x;{struct _tuple15*_TB=_cycalloc(sizeof(struct _tuple15));_TB->f0=t;{struct _tuple12*_TC=_cycalloc(sizeof(struct _tuple12));_TC->f0=qual;_TC->f1=ctype;_T8=(struct _tuple12*)_TC;}_TB->f1=_T8;_TB->f2=loc;_T7=(struct _tuple15*)_TB;}_TA->var_dict=_T2(_T5,_T6,_T7);_T9=env;
_TA->cpos=_T9->cpos;_T0=(struct Cyc_Port_Cenv*)_TA;}
# 960
return _T0;}
# 966
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_GlobalCenv*_T1;struct Cyc_Dict_Dict(*_T2)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple12*);struct Cyc_Dict_Dict(*_T3)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Port_Cenv*_T4;struct Cyc_Port_GlobalCenv*_T5;struct Cyc_Dict_Dict _T6;struct _tuple0*_T7;struct _tuple12*_T8;_T0=env;_T1=_T0->gcenv;_T3=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T9)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple12*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple12*))_T3;_T2=_T9;}_T4=env;_T5=_T4->gcenv;_T6=_T5->typedef_dict;_T7=n;{struct _tuple12*_T9=_cycalloc(sizeof(struct _tuple12));
_T9->f0=t;_T9->f1=ct;_T8=(struct _tuple12*)_T9;}
# 967
_T1->typedef_dict=_T2(_T6,_T7,_T8);}struct _tuple16{struct _tuple0*f0;unsigned f1;void*f2;};
# 970
static void Cyc_Port_register_localvar_decl(struct Cyc_Port_Cenv*env,struct _tuple0*x,unsigned loc,void*type,struct Cyc_Absyn_Exp*init){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_GlobalCenv*_T1;struct Cyc_List_List*_T2;struct _tuple16*_T3;struct Cyc_Port_Cenv*_T4;struct Cyc_Port_GlobalCenv*_T5;void(*_T6)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage*);void(*_T7)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Port_Cenv*_T8;struct Cyc_Port_GlobalCenv*_T9;struct Cyc_Hashtable_Table*_TA;unsigned _TB;struct Cyc_Port_VarUsage*_TC;_T0=env;_T1=_T0->gcenv;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple16*_TE=_cycalloc(sizeof(struct _tuple16));
_TE->f0=x;_TE->f1=loc;_TE->f2=type;_T3=(struct _tuple16*)_TE;}_TD->hd=_T3;_T4=env;_T5=_T4->gcenv;_TD->tl=_T5->vardecl_locs;_T2=(struct Cyc_List_List*)_TD;}_T1->vardecl_locs=_T2;_T7=Cyc_Hashtable_insert;{
void(*_TD)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage*)=(void(*)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage*))_T7;_T6=_TD;}_T8=env;_T9=_T8->gcenv;_TA=_T9->varusage_tab;_TB=loc;{struct Cyc_Port_VarUsage*_TD=_cycalloc(sizeof(struct Cyc_Port_VarUsage));_TD->address_taken=0;_TD->init=init;_TD->usage_locs=0;_TC=(struct Cyc_Port_VarUsage*)_TD;}_T6(_TA,_TB,_TC);}
# 974
static void Cyc_Port_register_localvar_usage(struct Cyc_Port_Cenv*env,unsigned declloc,unsigned useloc,int takeaddress){int(*_T0)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage**);int(*_T1)(struct Cyc_Hashtable_Table*,void*,void**);struct Cyc_Port_Cenv*_T2;struct Cyc_Port_GlobalCenv*_T3;struct Cyc_Hashtable_Table*_T4;unsigned _T5;struct Cyc_Port_VarUsage**_T6;int _T7;int _T8;struct Cyc_Port_VarUsage*_T9;struct Cyc_Port_VarUsage*_TA;struct Cyc_Port_VarUsage*_TB;struct Cyc_List_List*_TC;unsigned _TD;
struct Cyc_Port_VarUsage*varusage=0;_T1=Cyc_Hashtable_try_lookup;{
int(*_TE)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage**)=(int(*)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage**))_T1;_T0=_TE;}_T2=env;_T3=_T2->gcenv;_T4=_T3->varusage_tab;_T5=declloc;_T6=& varusage;_T7=_T0(_T4,_T5,_T6);if(!_T7)goto _TL109;_T8=takeaddress;
if(!_T8)goto _TL10B;_T9=_check_null(varusage);_T9->address_taken=1;goto _TL10C;_TL10B: _TL10C: _TA=
_check_null(varusage);{struct Cyc_List_List*l=_TA->usage_locs;_TB=varusage;{struct Cyc_List_List*_TE=_cycalloc(sizeof(struct Cyc_List_List));_TD=useloc;
_TE->hd=(void*)_TD;_TE->tl=l;_TC=(struct Cyc_List_List*)_TE;}_TB->usage_locs=_TC;}goto _TL10A;_TL109: _TL10A:;}struct _tuple17{void*f0;void*f1;unsigned f2;};
# 985
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned loc){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_GlobalCenv*_T1;struct Cyc_List_List*_T2;struct _tuple17*_T3;struct Cyc_Port_Cenv*_T4;struct Cyc_Port_GlobalCenv*_T5;_T0=env;_T1=_T0->gcenv;{struct Cyc_List_List*_T6=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple17*_T7=_cycalloc(sizeof(struct _tuple17));
# 987
_T7->f0=new_qual;_T7->f1=orig_qual;_T7->f2=loc;_T3=(struct _tuple17*)_T7;}_T6->hd=_T3;_T4=env;_T5=_T4->gcenv;
_T6->tl=_T5->qualifier_edits;_T2=(struct Cyc_List_List*)_T6;}
# 987
_T1->qualifier_edits=_T2;}
# 993
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned loc){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_GlobalCenv*_T1;struct Cyc_List_List*_T2;struct _tuple17*_T3;struct Cyc_Port_Cenv*_T4;struct Cyc_Port_GlobalCenv*_T5;_T0=env;_T1=_T0->gcenv;{struct Cyc_List_List*_T6=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple17*_T7=_cycalloc(sizeof(struct _tuple17));
# 995
_T7->f0=new_ptr;_T7->f1=orig_ptr;_T7->f2=loc;_T3=(struct _tuple17*)_T7;}_T6->hd=_T3;_T4=env;_T5=_T4->gcenv;
_T6->tl=_T5->pointer_edits;_T2=(struct Cyc_List_List*)_T6;}
# 995
_T1->pointer_edits=_T2;}
# 999
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned loc){struct Cyc_Port_Cenv*_T0;struct Cyc_Port_GlobalCenv*_T1;struct Cyc_List_List*_T2;struct _tuple17*_T3;struct Cyc_Port_Cenv*_T4;struct Cyc_Port_GlobalCenv*_T5;_T0=env;_T1=_T0->gcenv;{struct Cyc_List_List*_T6=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple17*_T7=_cycalloc(sizeof(struct _tuple17));
# 1001
_T7->f0=new_zt;_T7->f1=orig_zt;_T7->f2=loc;_T3=(struct _tuple17*)_T7;}_T6->hd=_T3;_T4=env;_T5=_T4->gcenv;
_T6->tl=_T5->zeroterm_edits;_T2=(struct Cyc_List_List*)_T6;}
# 1001
_T1->zeroterm_edits=_T2;}
# 1007
static void*Cyc_Port_main_type (void){struct _tuple8*_T0;struct _tuple8*_T1;struct _tuple8*_T2;struct _tuple8*_T3;struct _tuple8*_T4;struct _tuple8*_T5;void*_T6;void*_T7;void*_T8;void*_T9;void*_TA;struct Cyc_Absyn_Tqual _TB;void*_TC;void*_TD;struct Cyc_Absyn_FnType_Absyn_Type_struct*_TE;struct Cyc_List_List*_TF;struct _fat_ptr _T10;void*_T11;
struct _tuple8*arg1;arg1=_cycalloc(sizeof(struct _tuple8));_T0=arg1;
_T0->f0=0;_T1=arg1;_T1->f1=Cyc_Absyn_empty_tqual(0U);_T2=arg1;_T2->f2=Cyc_Absyn_sint_type;{
struct _tuple8*arg2;arg2=_cycalloc(sizeof(struct _tuple8));_T3=arg2;
_T3->f0=0;_T4=arg2;_T4->f1=Cyc_Absyn_empty_tqual(0U);_T5=arg2;_T6=
Cyc_Absyn_wildtyp(0);_T7=Cyc_Absyn_al_qual_type;_T8=Cyc_Absyn_string_type(_T6,_T7);_T9=Cyc_Absyn_wildtyp(0);_TA=Cyc_Absyn_al_qual_type;_TB=
Cyc_Absyn_empty_tqual(0U);_TC=Cyc_Tcutil_any_bool(0);_TD=Cyc_Absyn_false_type;
# 1012
_T5->f2=Cyc_Absyn_fatptr_type(_T8,_T9,_TA,_TB,_TC,_TD);{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T12=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T12->tag=6;
# 1014
_T12->f1.tvars=0;_T12->f1.effect=0;
_T12->f1.ret_tqual=Cyc_Absyn_empty_tqual(0U);
_T12->f1.ret_type=Cyc_Absyn_sint_type;{struct _tuple8*_T13[2];
_T13[0]=arg1;_T13[1]=arg2;_T10=_tag_fat(_T13,sizeof(struct _tuple8*),2);_TF=Cyc_List_list(_T10);}_T12->f1.args=_TF;_T12->f1.c_varargs=0;
_T12->f1.cyc_varargs=0;_T12->f1.qual_bnd=0;
_T12->f1.attributes=0;
_T12->f1.checks_clause=0;
_T12->f1.checks_assn=0;
_T12->f1.requires_clause=0;
_T12->f1.requires_assn=0;
_T12->f1.ensures_clause=0;_T12->f1.arg_vardecls=0;
_T12->f1.ensures_assn=0;_T12->f1.throws_clause=0;
_T12->f1.throws_assn=0;
_T12->f1.return_value=0;_T12->f1.effconstr=0;_TE=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T12;}_T11=(void*)_TE;
# 1014
return _T11;}}
# 1031
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*,void*);
# 1034
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv (void){struct Cyc_Port_Cenv*_T0;struct _tuple0*_T1;struct _fat_ptr*_T2;void*_T3;void*_T4;struct Cyc_Port_Cenv*_T5;void*_T6;void*_T7;struct Cyc_Port_Cenv*_T8;
struct Cyc_Port_Cenv*e=Cyc_Port_empty_cenv();_T0=e;{struct _tuple0*_T9=_cycalloc(sizeof(struct _tuple0));
# 1037
_T9->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_TA=_cycalloc(sizeof(struct _fat_ptr));*_TA=_tag_fat("main",sizeof(char),5U);_T2=(struct _fat_ptr*)_TA;}_T9->f1=_T2;_T1=(struct _tuple0*)_T9;}_T3=Cyc_Port_main_type();_T4=Cyc_Port_const_ct();_T5=e;_T6=
Cyc_Port_main_type();_T7=Cyc_Port_type_to_ctype(_T5,_T6);
# 1037
e=Cyc_Port_add_var(_T0,_T1,_T3,_T4,_T7,0U);_T8=e;
# 1039
return _T8;}
# 1045
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){int*_T0;unsigned _T1;int(*_T2)(struct Cyc_Dict_Dict,struct _fat_ptr*);int(*_T3)(struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict*_T4;struct Cyc_Dict_Dict _T5;struct _fat_ptr*_T6;int _T7;struct Cyc_Dict_Dict*_T8;struct Cyc_Dict_Dict(*_T9)(struct Cyc_Dict_Dict,struct _fat_ptr*,int*);struct Cyc_Dict_Dict(*_TA)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Dict_Dict*_TB;struct Cyc_Dict_Dict _TC;struct _fat_ptr*_TD;int*_TE;int*(*_TF)(struct Cyc_Dict_Dict,struct _fat_ptr*);void*(*_T10)(struct Cyc_Dict_Dict,void*);struct Cyc_Dict_Dict*_T11;struct Cyc_Dict_Dict _T12;struct _fat_ptr*_T13;int*_T14;int*_T15;int _T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;void*_T1E;void*_T1F;struct Cyc_List_List*_T20;unsigned _T21;struct Cyc_Dict_Dict*_T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_List_List*_T25;
void*_T26=Cyc_Port_compress_ct(t);struct Cyc_List_List*_T27;void*_T28;void*_T29;void*_T2A;void*_T2B;void*_T2C;_T0=(int*)_T26;_T1=*_T0;switch(_T1){case 0: goto _LL4;case 1: _LL4: goto _LL6;case 2: _LL6: goto _LL8;case 3: _LL8: goto _LLA;case 4: _LLA: goto _LLC;case 5: _LLC: goto _LLE;case 6: _LLE: goto _LL10;case 14: _LL10: goto _LL12;case 13: _LL12: goto _LL14;case 16: _LL14: goto _LL16;case 8: _LL16: goto _LL18;case 9: _LL18: goto _LL1A;case 7: _LL1A:
# 1059
 return;case 10:{struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_T2D=(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_T26;_T2C=_T2D->f1;}{struct _fat_ptr*n=_T2C;_T3=Cyc_Dict_member;{
# 1061
int(*_T2D)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T3;_T2=_T2D;}_T4=cts;_T5=*_T4;_T6=n;_T7=_T2(_T5,_T6);if(_T7)goto _TL10E;else{goto _TL110;}
_TL110: _T8=cts;_TA=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T2D)(struct Cyc_Dict_Dict,struct _fat_ptr*,int*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,int*))_TA;_T9=_T2D;}_TB=cts;_TC=*_TB;_TD=n;{int*_T2D=_cycalloc_atomic(sizeof(int));*_T2D=0;_TE=(int*)_T2D;}*_T8=_T9(_TC,_TD,_TE);goto _TL10F;_TL10E: _TL10F: _T10=Cyc_Dict_lookup;{
int*(*_T2D)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))_T10;_TF=_T2D;}_T11=cts;_T12=*_T11;_T13=n;{int*p=_TF(_T12,_T13);_T14=p;_T15=p;_T16=*_T15;
*_T14=_T16 + 1;
return;}}case 11:{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T2D=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_T26;_T17=_T2D->f1;_T2C=(void*)_T17;_T18=_T2D->f2;_T2B=(void*)_T18;_T19=_T2D->f3;_T2A=(void*)_T19;_T1A=_T2D->f4;_T29=(void*)_T1A;_T1B=_T2D->f5;_T28=(void*)_T1B;}{void*t1=_T2C;void*t2=_T2B;void*t3=_T2A;void*t4=_T29;void*zt=_T28;
# 1067
Cyc_Port_region_counts(cts,t1);
Cyc_Port_region_counts(cts,t4);
return;}case 12:{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T2D=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T26;_T1C=_T2D->f1;_T2C=(void*)_T1C;_T1D=_T2D->f2;_T2B=(void*)_T1D;_T1E=_T2D->f3;_T2A=(void*)_T1E;}{void*t1=_T2C;void*t2=_T2B;void*zt=_T2A;
# 1071
Cyc_Port_region_counts(cts,t1);
return;}default:{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T2D=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T26;_T1F=_T2D->f1;_T2C=(void*)_T1F;_T27=_T2D->f2;}{void*t1=_T2C;struct Cyc_List_List*ts=_T27;
# 1074
Cyc_Port_region_counts(cts,t1);
_TL114: _T20=ts;_T21=(unsigned)_T20;if(_T21)goto _TL112;else{goto _TL113;}_TL112: _T22=cts;_T23=ts;_T24=_T23->hd;Cyc_Port_region_counts(_T22,_T24);_T25=ts;ts=_T25->tl;goto _TL114;_TL113:
 return;}};}
# 1085
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){struct _tuple12 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;int _T6;void*_T7;struct Cyc_Absyn_PtrInfo _T8;struct Cyc_Absyn_PtrInfo _T9;struct Cyc_Absyn_PtrAtts _TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrAtts _TC;void*_TD;void*_TE;void*_TF;unsigned _T10;struct Cyc_Absyn_PtrLoc*_T11;unsigned _T12;struct Cyc_Absyn_PtrLoc*_T13;struct Cyc_Port_Cenv*_T14;struct Cyc_Port_GlobalCenv*_T15;struct Cyc_List_List*_T16;struct Cyc_Port_Edit*_T17;struct Cyc_Port_Cenv*_T18;struct Cyc_Port_GlobalCenv*_T19;void*_T1A;int*_T1B;int _T1C;void*_T1D;struct Cyc_Absyn_ArrayInfo _T1E;void*_T1F;void*_T20;void*_T21;int*_T22;int _T23;void*_T24;struct Cyc_Absyn_FnInfo _T25;struct Cyc_Absyn_FnInfo _T26;void*_T27;void*_T28;struct Cyc_List_List*_T29;void*_T2A;struct _tuple8*_T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;
# 1087
c=Cyc_Port_compress_ct(c);{struct _tuple12 _T2F;
_T2F.f0=t;_T2F.f1=c;_T0=_T2F;}{struct _tuple12 _T2F=_T0;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;void*_T32;void*_T33;struct Cyc_Absyn_PtrLoc*_T34;void*_T35;void*_T36;_T1=_T2F.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 4: _T4=_T2F.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=11)goto _TL116;_T7=_T2F.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T37=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T7;_T8=_T37->f1;_T36=_T8.elt_type;_T9=_T37->f1;_TA=_T9.ptr_atts;_T35=_TA.eff;_TB=_T37->f1;_TC=_TB.ptr_atts;_T34=_TC.ptrloc;}_TD=_T2F.f1;{struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_T37=(struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_TD;_TE=_T37->f1;_T33=(void*)_TE;_TF=_T37->f4;_T32=(void*)_TF;}{void*et=_T36;void*rt=_T35;struct Cyc_Absyn_PtrLoc*ploc=_T34;void*ec=_T33;void*rc=_T32;
# 1090
Cyc_Port_register_rgns(env,counts,fn_res,et,ec);_T11=ploc;_T12=(unsigned)_T11;
if(!_T12)goto _TL118;_T13=ploc;_T10=_T13->rgn_loc;goto _TL119;_TL118: _T10=0U;_TL119: {unsigned loc=_T10;
rc=Cyc_Port_compress_ct(rc);_T14=env;_T15=_T14->gcenv;{struct Cyc_List_List*_T37=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Port_Edit*_T38=_cycalloc(sizeof(struct Cyc_Port_Edit));
# 1108 "port.cyc"
_T38->loc=loc;_T38->old_string=_tag_fat("`H ",sizeof(char),4U);_T38->new_string=_tag_fat("",sizeof(char),1U);_T17=(struct Cyc_Port_Edit*)_T38;}_T37->hd=_T17;_T18=env;_T19=_T18->gcenv;_T37->tl=_T19->edits;_T16=(struct Cyc_List_List*)_T37;}
# 1107
_T15->edits=_T16;goto _LL0;}}_TL116: goto _LL7;case 5: _T1A=_T2F.f1;_T1B=(int*)_T1A;_T1C=*_T1B;if(_T1C!=12)goto _TL11A;_T1D=_T2F.f0;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T37=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T1D;_T1E=_T37->f1;_T36=_T1E.elt_type;}_T1F=_T2F.f1;{struct Cyc_Port_Array_ct_Port_Ctype_struct*_T37=(struct Cyc_Port_Array_ct_Port_Ctype_struct*)_T1F;_T20=_T37->f1;_T35=(void*)_T20;}{void*et=_T36;void*ec=_T35;
# 1112
Cyc_Port_register_rgns(env,counts,fn_res,et,ec);goto _LL0;}_TL11A: goto _LL7;case 6: _T21=_T2F.f1;_T22=(int*)_T21;_T23=*_T22;if(_T23!=15)goto _TL11C;_T24=_T2F.f0;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T37=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T24;_T25=_T37->f1;_T36=_T25.ret_type;_T26=_T37->f1;_T31=_T26.args;}_T27=_T2F.f1;{struct Cyc_Port_Fn_ct_Port_Ctype_struct*_T37=(struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_T27;_T28=_T37->f1;_T35=(void*)_T28;_T30=_T37->f2;}{void*rt=_T36;struct Cyc_List_List*argst=_T31;void*rc=_T35;struct Cyc_List_List*argsc=_T30;
# 1115
Cyc_Port_register_rgns(env,counts,1,rt,rc);
_TL121: if(argst!=0)goto _TL122;else{goto _TL120;}_TL122: if(argsc!=0)goto _TL11F;else{goto _TL120;}
_TL11F: _T29=argst;_T2A=_T29->hd;_T2B=(struct _tuple8*)_T2A;{struct _tuple8 _T37=*_T2B;void*_T38;_T38=_T37.f2;{void*at=_T38;_T2C=argsc;{
void*ac=_T2C->hd;
Cyc_Port_register_rgns(env,counts,0,at,ac);}}}_T2D=argst;
# 1116
argst=_T2D->tl;_T2E=argsc;argsc=_T2E->tl;goto _TL121;_TL120: goto _LL0;}_TL11C: goto _LL7;default: _LL7: goto _LL0;}_LL0:;}}
# 1128
static void*Cyc_Port_gen_exp(int,struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*);
# 1130
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Decl*,int);
# 1133
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct _tuple0*_T4;struct Cyc_Port_Cenv*_T5;void*_T6;int _T7;void*_T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;void*_TA;int*_TB;int _TC;void*_TD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE;void*_TF;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T10;enum Cyc_Absyn_Size_of _T11;struct _tuple0*_T12;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 8: _T3=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T13=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3;_T12=_T13->f1;}{struct _tuple0*n=_T12;_T4=n;
# 1136
(*_T4).f0=Cyc_Absyn_Loc_n();_T5=env;_T6=
Cyc_Port_lookup_typedef(env,n);_T7=Cyc_Port_is_char(_T5,_T6);return _T7;}case 0: _T8=t;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T8;_TA=_T9->f1;_TB=(int*)_TA;_TC=*_TB;if(_TC!=1)goto _TL124;_TD=t;_TE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD;_TF=_TE->f1;_T10=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TF;_T11=_T10->f2;if(_T11!=Cyc_Absyn_Char_sz)goto _TL126;
return 1;_TL126: goto _LL5;_TL124: goto _LL5;default: _LL5:
 return 0;};}
# 1144
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct _tuple0*_T4;void*_T5;void*_T6;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7;void*_T8;int*_T9;unsigned _TA;void*_TB;void*_TC;void*_TD;void*_TE;union Cyc_Absyn_AggrInfo _TF;struct _union_AggrInfo_UnknownAggr _T10;unsigned _T11;union Cyc_Absyn_AggrInfo _T12;struct _union_AggrInfo_UnknownAggr _T13;struct _tuple2 _T14;enum Cyc_Absyn_AggrKind _T15;union Cyc_Absyn_AggrInfo _T16;struct _union_AggrInfo_UnknownAggr _T17;struct _tuple2 _T18;struct _tuple0*_T19;void*_T1A;union Cyc_Absyn_AggrInfo _T1B;struct _union_AggrInfo_UnknownAggr _T1C;struct _tuple2 _T1D;struct _tuple0*_T1E;void*_T1F;union Cyc_Absyn_AggrInfo _T20;struct _union_AggrInfo_KnownAggr _T21;struct Cyc_Absyn_Aggrdecl**_T22;struct Cyc_Absyn_Aggrdecl*_T23;struct Cyc_Port_Cenv*_T24;struct Cyc_Absyn_Aggrdecl*_T25;struct _tuple0*_T26;void*_T27;struct Cyc_Port_Cenv*_T28;struct Cyc_Absyn_Aggrdecl*_T29;struct _tuple0*_T2A;void*_T2B;void*_T2C;void*_T2D;void*_T2E;struct Cyc_List_List*_T2F;unsigned _T30;struct Cyc_List_List*_T31;void*_T32;struct Cyc_Absyn_Enumfield*_T33;struct _tuple0*_T34;struct Cyc_Port_Cenv*_T35;struct Cyc_List_List*_T36;void*_T37;struct Cyc_Absyn_Enumfield*_T38;struct _tuple0*_T39;void*_T3A;void*_T3B;void*_T3C;struct Cyc_List_List*_T3D;void*_T3E;void*_T3F;struct Cyc_Absyn_PtrInfo _T40;struct Cyc_Absyn_PtrInfo _T41;struct Cyc_Absyn_PtrInfo _T42;struct Cyc_Absyn_PtrAtts _T43;struct Cyc_Absyn_PtrInfo _T44;struct Cyc_Absyn_PtrAtts _T45;struct Cyc_Absyn_PtrInfo _T46;struct Cyc_Absyn_PtrAtts _T47;struct Cyc_Absyn_PtrInfo _T48;struct Cyc_Absyn_PtrAtts _T49;struct Cyc_Absyn_PtrInfo _T4A;struct Cyc_Absyn_PtrAtts _T4B;unsigned _T4C;struct Cyc_Absyn_PtrLoc*_T4D;unsigned _T4E;struct Cyc_Absyn_PtrLoc*_T4F;unsigned _T50;struct Cyc_Absyn_PtrLoc*_T51;unsigned _T52;struct Cyc_Absyn_PtrLoc*_T53;unsigned _T54;struct Cyc_Absyn_PtrLoc*_T55;unsigned _T56;struct Cyc_Absyn_PtrLoc*_T57;void*_T58;int*_T59;unsigned _T5A;void*_T5B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5C;void*_T5D;int*_T5E;int _T5F;void*_T60;struct Cyc_Absyn_Tvar*_T61;int _T62;struct _fat_ptr*_T63;int _T64;int _T65;struct Cyc_Absyn_Exp*_T66;int*_T67;int _T68;struct Cyc_Port_Cenv*_T69;struct Cyc_Port_GlobalCenv*_T6A;int _T6B;struct Cyc_Port_Cenv*_T6C;void*_T6D;void*_T6E;struct Cyc_Absyn_Tqual _T6F;int _T70;struct Cyc_Absyn_Tqual _T71;unsigned _T72;struct Cyc_Absyn_Tqual _T73;int _T74;void*_T75;void*_T76;struct Cyc_Port_Cenv*_T77;void*_T78;void*_T79;int _T7A;unsigned _T7B;int _T7C;void*_T7D;void*_T7E;struct Cyc_Port_Cenv*_T7F;void*_T80;void*_T81;int _T82;unsigned _T83;int*_T84;int _T85;void*_T86;void*_T87;int _T88;void*_T89;void*_T8A;void*_T8B;struct Cyc_Absyn_Tqual _T8C;int _T8D;void*_T8E;int _T8F;void*_T90;void*_T91;int _T92;void*_T93;void*_T94;struct Cyc_Absyn_ArrayInfo _T95;struct Cyc_Absyn_ArrayInfo _T96;struct Cyc_Absyn_ArrayInfo _T97;struct Cyc_Absyn_ArrayInfo _T98;int*_T99;int _T9A;struct Cyc_Port_Cenv*_T9B;struct Cyc_Port_GlobalCenv*_T9C;int _T9D;struct Cyc_Port_Cenv*_T9E;void*_T9F;void*_TA0;struct Cyc_Absyn_Tqual _TA1;int _TA2;struct Cyc_Absyn_Tqual _TA3;unsigned _TA4;struct Cyc_Port_Cenv*_TA5;void*_TA6;void*_TA7;int _TA8;unsigned _TA9;int*_TAA;int _TAB;void*_TAC;void*_TAD;int _TAE;void*_TAF;void*_TB0;void*_TB1;struct Cyc_Absyn_Tqual _TB2;int _TB3;void*_TB4;int _TB5;void*_TB6;void*_TB7;struct Cyc_Absyn_FnInfo _TB8;struct Cyc_Absyn_FnInfo _TB9;struct Cyc_Port_Cenv*_TBA;void*_TBB;struct Cyc_List_List*_TBC;void*_TBD;struct _tuple8*_TBE;struct Cyc_List_List*_TBF;struct Cyc_List_List*_TC0;void*_TC1;struct Cyc_List_List*_TC2;void*_TC3;void*_TC4;struct Cyc_List_List*_TC5;unsigned _TC6;struct Cyc_Absyn_PtrLoc*_TC7;void*_TC8;void*_TC9;void*_TCA;void*_TCB;struct Cyc_Absyn_Tqual _TCC;union Cyc_Absyn_AggrInfo _TCD;void*_TCE;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 8: _T3=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_TCF=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T3;_TCE=_TCF->f1;}{struct _tuple0*n=_TCE;_T4=n;
# 1147
(*_T4).f0=Cyc_Absyn_Loc_n();_T5=
Cyc_Port_lookup_typedef_ctype(env,n);return _T5;}case 0: _T6=t;_T7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6;_T8=_T7->f1;_T9=(int*)_T8;_TA=*_T9;switch(_TA){case 0: _TB=
Cyc_Port_void_ct();return _TB;case 1: goto _LLA;case 2: _LLA: _TC=
# 1227
Cyc_Port_arith_ct();return _TC;case 24: _TD=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TCF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD;_TE=_TCF->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TD0=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TE;_TCD=_TD0->f1;}}{union Cyc_Absyn_AggrInfo ai=_TCD;struct Cyc_Absyn_Aggrdecl**_TCF;struct _tuple0*_TD0;_TF=ai;_T10=_TF.UnknownAggr;_T11=_T10.tag;if(_T11!=1)goto _TL12A;_T12=ai;_T13=_T12.UnknownAggr;_T14=_T13.val;_T15=_T14.f0;if(_T15!=Cyc_Absyn_StructA)goto _TL12C;_T16=ai;_T17=_T16.UnknownAggr;_T18=_T17.val;_TD0=_T18.f1;{struct _tuple0*n=_TD0;_T19=n;
# 1271
(*_T19).f0=Cyc_Absyn_Loc_n();{
struct _tuple11*p=Cyc_Port_lookup_struct_decl(env,n);_T1A=
Cyc_Port_known_aggr_ct(p);return _T1A;}}_TL12C: _T1B=ai;_T1C=_T1B.UnknownAggr;_T1D=_T1C.val;_TD0=_T1D.f1;{struct _tuple0*n=_TD0;_T1E=n;
# 1275
(*_T1E).f0=Cyc_Absyn_Loc_n();{
struct _tuple11*p=Cyc_Port_lookup_union_decl(env,n);_T1F=
Cyc_Port_known_aggr_ct(p);return _T1F;}}_TL12A: _T20=ai;_T21=_T20.KnownAggr;_TCF=_T21.val;{struct Cyc_Absyn_Aggrdecl**adp=_TCF;_T22=adp;{
# 1279
struct Cyc_Absyn_Aggrdecl*ad=*_T22;_T23=ad;{
enum Cyc_Absyn_AggrKind _TD1=_T23->kind;if(_TD1!=Cyc_Absyn_StructA)goto _TL12E;_T24=env;_T25=ad;_T26=_T25->name;{
# 1282
struct _tuple11*p=Cyc_Port_lookup_struct_decl(_T24,_T26);_T27=
Cyc_Port_known_aggr_ct(p);return _T27;}_TL12E: _T28=env;_T29=ad;_T2A=_T29->name;{
# 1285
struct _tuple11*p=Cyc_Port_lookup_union_decl(_T28,_T2A);_T2B=
Cyc_Port_known_aggr_ct(p);return _T2B;};}}};}case 19: _T2C=
# 1289
Cyc_Port_arith_ct();return _T2C;case 20: _T2D=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TCF=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2D;_T2E=_TCF->f1;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_TD0=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_T2E;_TCE=_TD0->f1;}}{struct Cyc_List_List*fs=_TCE;
# 1292
_TL133: _T2F=fs;_T30=(unsigned)_T2F;if(_T30)goto _TL131;else{goto _TL132;}
_TL131: _T31=fs;_T32=_T31->hd;_T33=(struct Cyc_Absyn_Enumfield*)_T32;_T34=_T33->name;(*_T34).f0=Cyc_Absyn_Loc_n();_T35=env;_T36=fs;_T37=_T36->hd;_T38=(struct Cyc_Absyn_Enumfield*)_T37;_T39=_T38->name;_T3A=Cyc_Absyn_sint_type;_T3B=
Cyc_Port_const_ct();_T3C=Cyc_Port_arith_ct();env=Cyc_Port_add_var(_T35,_T39,_T3A,_T3B,_T3C,0U);_T3D=fs;
# 1292
fs=_T3D->tl;goto _TL133;_TL132: _T3E=
# 1296
Cyc_Port_arith_ct();return _T3E;}default: goto _LL15;};case 4: _T3F=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TCF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3F;_T40=_TCF->f1;_TCE=_T40.elt_type;_T41=_TCF->f1;_TCC=_T41.elt_tq;_T42=_TCF->f1;_T43=_T42.ptr_atts;_TCB=_T43.eff;_T44=_TCF->f1;_T45=_T44.ptr_atts;_TCA=_T45.nullable;_T46=_TCF->f1;_T47=_T46.ptr_atts;_TC9=_T47.bounds;_T48=_TCF->f1;_T49=_T48.ptr_atts;_TC8=_T49.zero_term;_T4A=_TCF->f1;_T4B=_T4A.ptr_atts;_TC7=_T4B.ptrloc;}{void*et=_TCE;struct Cyc_Absyn_Tqual tq=_TCC;void*r=_TCB;void*n=_TCA;void*b=_TC9;void*zt=_TC8;struct Cyc_Absyn_PtrLoc*loc=_TC7;_T4D=loc;_T4E=(unsigned)_T4D;
# 1151
if(!_T4E)goto _TL134;_T4F=loc;_T4C=_T4F->ptr_loc;goto _TL135;_TL134: _T4C=0U;_TL135: {unsigned ptr_loc=_T4C;_T51=loc;_T52=(unsigned)_T51;
if(!_T52)goto _TL136;_T53=loc;_T50=_T53->rgn_loc;goto _TL137;_TL136: _T50=0U;_TL137: {unsigned rgn_loc=_T50;_T55=loc;_T56=(unsigned)_T55;
if(!_T56)goto _TL138;_T57=loc;_T54=_T57->zt_loc;goto _TL139;_TL138: _T54=0U;_TL139: {unsigned zt_loc=_T54;
# 1157
void*cet=Cyc_Port_type_to_ctype(env,et);
void*new_rgn;{struct _fat_ptr*_TCF;_T58=r;_T59=(int*)_T58;_T5A=*_T59;switch(_T5A){case 0: _T5B=r;_T5C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T5B;_T5D=_T5C->f1;_T5E=(int*)_T5D;_T5F=*_T5E;if(_T5F!=6)goto _TL13B;
# 1162
new_rgn=Cyc_Port_heap_ct();goto _LL17;_TL13B: goto _LL1E;case 2: _T60=r;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T60;_T61=_TD0->f1;{struct Cyc_Absyn_Tvar _TD1=*_T61;_TCF=_TD1.name;}}{struct _fat_ptr*name=_TCF;
# 1164
new_rgn=Cyc_Port_rgnvar_ct(name);goto _LL17;}case 1: _T62=
# 1168
Cyc_Port_in_fn_arg(env);if(!_T62)goto _TL13D;_T63=
Cyc_Port_new_region_var();new_rgn=Cyc_Port_rgnvar_ct(_T63);goto _TL13E;
# 1171
_TL13D: _T64=Cyc_Port_in_fn_res(env);if(_T64)goto _TL141;else{goto _TL142;}_TL142: _T65=Cyc_Port_in_toplevel(env);if(_T65)goto _TL141;else{goto _TL13F;}
_TL141: new_rgn=Cyc_Port_heap_ct();goto _TL140;
_TL13F: new_rgn=Cyc_Port_var();_TL140: _TL13E: goto _LL17;default: _LL1E:
# 1176
 new_rgn=Cyc_Port_heap_ct();goto _LL17;}_LL17:;}_T66=
# 1178
Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b);{int orig_fat=_T66==0;
int orig_zt;{
void*_TCF=Cyc_Absyn_compress(zt);_T67=(int*)_TCF;_T68=*_T67;if(_T68!=1)goto _TL143;
orig_zt=Cyc_Port_is_char(env,et);goto _LL20;_TL143:
 orig_zt=Cyc_Tcutil_force_type2bool(0,zt);goto _LL20;_LL20:;}_T69=env;_T6A=_T69->gcenv;_T6B=_T6A->porting;
# 1184
if(!_T6B)goto _TL145;{
void*cqv=Cyc_Port_var();_T6C=env;_T6D=cqv;_T6F=tq;_T70=_T6F.print_const;
# 1189
if(!_T70)goto _TL147;_T6E=Cyc_Port_const_ct();goto _TL148;_TL147: _T6E=Cyc_Port_notconst_ct();_TL148: _T71=tq;_T72=_T71.loc;Cyc_Port_register_const_cvar(_T6C,_T6D,_T6E,_T72);_T73=tq;_T74=_T73.print_const;
# 1192
if(!_T74)goto _TL149;_T75=cqv;_T76=Cyc_Port_const_ct();Cyc_Port_unify_ct(_T75,_T76);goto _TL14A;_TL149: _TL14A: {
# 1198
void*cbv=Cyc_Port_var();_T77=env;_T78=cbv;_T7A=orig_fat;
# 1201
if(!_T7A)goto _TL14B;_T79=Cyc_Port_fat_ct();goto _TL14C;_TL14B: _T79=Cyc_Port_thin_ct();_TL14C: _T7B=ptr_loc;Cyc_Port_register_ptr_cvars(_T77,_T78,_T79,_T7B);_T7C=orig_fat;
# 1203
if(!_T7C)goto _TL14D;_T7D=cbv;_T7E=Cyc_Port_fat_ct();Cyc_Port_unify_ct(_T7D,_T7E);goto _TL14E;_TL14D: _TL14E: {
void*czv=Cyc_Port_var();_T7F=env;_T80=czv;_T82=orig_zt;
# 1207
if(!_T82)goto _TL14F;_T81=Cyc_Port_zterm_ct();goto _TL150;_TL14F: _T81=Cyc_Port_nozterm_ct();_TL150: _T83=zt_loc;Cyc_Port_register_zeroterm_cvars(_T7F,_T80,_T81,_T83);{
# 1209
void*_TCF=Cyc_Absyn_compress(zt);_T84=(int*)_TCF;_T85=*_T84;if(_T85!=1)goto _TL151;goto _LL25;_TL151: _T86=czv;_T88=
# 1212
Cyc_Tcutil_force_type2bool(0,zt);if(!_T88)goto _TL153;_T87=Cyc_Port_zterm_ct();goto _TL154;_TL153: _T87=Cyc_Port_nozterm_ct();_TL154: Cyc_Port_unify_ct(_T86,_T87);goto _LL25;_LL25:;}_T89=
# 1218
Cyc_Port_ptr_ct(cet,cqv,cbv,new_rgn,czv);return _T89;}}}
# 1222
_TL145: _T8A=cet;_T8C=tq;_T8D=_T8C.print_const;if(!_T8D)goto _TL155;_T8B=Cyc_Port_const_ct();goto _TL156;_TL155: _T8B=Cyc_Port_notconst_ct();_TL156: _T8F=orig_fat;
if(!_T8F)goto _TL157;_T8E=Cyc_Port_fat_ct();goto _TL158;_TL157: _T8E=Cyc_Port_thin_ct();_TL158: _T90=new_rgn;_T92=orig_zt;
if(!_T92)goto _TL159;_T91=Cyc_Port_zterm_ct();goto _TL15A;_TL159: _T91=Cyc_Port_nozterm_ct();_TL15A: _T93=
# 1222
Cyc_Port_ptr_ct(_T8A,_T8B,_T8E,_T90,_T91);return _T93;}}}}}case 5: _T94=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TCF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T94;_T95=_TCF->f1;_TCE=_T95.elt_type;_T96=_TCF->f1;_TCC=_T96.tq;_T97=_TCF->f1;_TCB=_T97.zero_term;_T98=_TCF->f1;_TC6=_T98.zt_loc;}{void*et=_TCE;struct Cyc_Absyn_Tqual tq=_TCC;void*zt=_TCB;unsigned ztloc=_TC6;
# 1230
void*cet=Cyc_Port_type_to_ctype(env,et);
int orig_zt;{
void*_TCF=Cyc_Absyn_compress(zt);_T99=(int*)_TCF;_T9A=*_T99;if(_T9A!=1)goto _TL15B;
orig_zt=0;goto _LL2A;_TL15B:
 orig_zt=Cyc_Tcutil_force_type2bool(0,zt);goto _LL2A;_LL2A:;}_T9B=env;_T9C=_T9B->gcenv;_T9D=_T9C->porting;
# 1236
if(!_T9D)goto _TL15D;{
void*cqv=Cyc_Port_var();_T9E=env;_T9F=cqv;_TA1=tq;_TA2=_TA1.print_const;
if(!_TA2)goto _TL15F;_TA0=Cyc_Port_const_ct();goto _TL160;_TL15F: _TA0=Cyc_Port_notconst_ct();_TL160: _TA3=tq;_TA4=_TA3.loc;Cyc_Port_register_const_cvar(_T9E,_T9F,_TA0,_TA4);{
# 1245
void*czv=Cyc_Port_var();_TA5=env;_TA6=czv;_TA8=orig_zt;
if(!_TA8)goto _TL161;_TA7=Cyc_Port_zterm_ct();goto _TL162;_TL161: _TA7=Cyc_Port_nozterm_ct();_TL162: _TA9=ztloc;Cyc_Port_register_zeroterm_cvars(_TA5,_TA6,_TA7,_TA9);{
# 1248
void*_TCF=Cyc_Absyn_compress(zt);_TAA=(int*)_TCF;_TAB=*_TAA;if(_TAB!=1)goto _TL163;goto _LL2F;_TL163: _TAC=czv;_TAE=
# 1251
Cyc_Tcutil_force_type2bool(0,zt);if(!_TAE)goto _TL165;_TAD=Cyc_Port_zterm_ct();goto _TL166;_TL165: _TAD=Cyc_Port_nozterm_ct();_TL166: Cyc_Port_unify_ct(_TAC,_TAD);goto _LL2F;_LL2F:;}_TAF=
# 1254
Cyc_Port_array_ct(cet,cqv,czv);return _TAF;}}
# 1256
_TL15D: _TB0=cet;_TB2=tq;_TB3=_TB2.print_const;if(!_TB3)goto _TL167;_TB1=Cyc_Port_const_ct();goto _TL168;_TL167: _TB1=Cyc_Port_notconst_ct();_TL168: _TB5=orig_zt;
if(!_TB5)goto _TL169;_TB4=Cyc_Port_zterm_ct();goto _TL16A;_TL169: _TB4=Cyc_Port_nozterm_ct();_TL16A: _TB6=
# 1256
Cyc_Port_array_ct(_TB0,_TB1,_TB4);return _TB6;}case 6: _TB7=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_TCF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TB7;_TB8=_TCF->f1;_TCE=_TB8.ret_type;_TB9=_TCF->f1;_TC5=_TB9.args;}{void*rt=_TCE;struct Cyc_List_List*args=_TC5;_TBA=
# 1260
Cyc_Port_set_cpos(env,0U);_TBB=rt;{void*crt=Cyc_Port_type_to_ctype(_TBA,_TBB);
struct Cyc_Port_Cenv*env_arg=Cyc_Port_set_cpos(env,1U);
struct Cyc_List_List*cargs=0;
_TL16E: if(args!=0)goto _TL16C;else{goto _TL16D;}
_TL16C: _TBC=args;_TBD=_TBC->hd;_TBE=(struct _tuple8*)_TBD;{struct _tuple8 _TCF=*_TBE;void*_TD0;struct Cyc_Absyn_Tqual _TD1;_TD1=_TCF.f1;_TD0=_TCF.f2;{struct Cyc_Absyn_Tqual tq=_TD1;void*t=_TD0;{struct Cyc_List_List*_TD2=_cycalloc(sizeof(struct Cyc_List_List));
_TD2->hd=Cyc_Port_type_to_ctype(env_arg,t);_TD2->tl=cargs;_TBF=(struct Cyc_List_List*)_TD2;}cargs=_TBF;}}_TC0=args;
# 1263
args=_TC0->tl;goto _TL16E;_TL16D: _TC1=crt;_TC2=
# 1267
Cyc_List_imp_rev(cargs);_TC3=Cyc_Port_fn_ct(_TC1,_TC2);return _TC3;}}default: _LL15: _TC4=
# 1297
Cyc_Port_arith_ct();return _TC4;};}
# 1302
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;enum Cyc_Absyn_Primop _T3;int _T4;struct Cyc_List_List*_T5;void*_T6;void*_T7;void*_T8;void*_T9;void*_TA;void*_TB;void*_TC;void*_TD;int _TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;void*_T18;int _T19;void*_T1A;void*_T1B;void*_T1C;void*_T1D;void*_T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;struct Cyc_List_List*_T25;void*_T26;void*_T27;void*_T28;void*_T29;void*_T2A;void*_T2B;void*_T2C;void*_T2D;int _T2E;void*_T2F;void*_T30;void*_T31;void*_T32;void*_T33;void*_T34;void*_T35;int _T36;void*_T37;void*_T38;void*_T39;void*_T3A;void*_T3B;void*_T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;void*_T41;struct Cyc_List_List*_T42;void*_T43;void*_T44;void*_T45;void*_T46;void*_T47;void*_T48;int(*_T49)(struct _fat_ptr,struct _fat_ptr);void*(*_T4A)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T4B;struct _fat_ptr _T4C;_T0=
_check_null(args);_T1=_T0->hd;{void*arg1=Cyc_Port_compress_ct(_T1);_T2=args;{
struct Cyc_List_List*arg2s=_T2->tl;_T3=p;_T4=(int)_T3;switch(_T4){case Cyc_Absyn_Plus: _T5=
# 1310
_check_null(arg2s);_T6=_T5->hd;{void*arg2=Cyc_Port_compress_ct(_T6);_T7=arg1;_T8=
Cyc_Port_var();_T9=Cyc_Port_var();_TA=Cyc_Port_fat_ct();_TB=Cyc_Port_var();_TC=Cyc_Port_var();_TD=Cyc_Port_ptr_ct(_T8,_T9,_TA,_TB,_TC);_TE=Cyc_Port_leq(_T7,_TD);if(!_TE)goto _TL170;_TF=arg2;_T10=
Cyc_Port_arith_ct();Cyc_Port_leq(_TF,_T10);_T11=arg1;
return _T11;
_TL170: _T12=arg2;_T13=Cyc_Port_var();_T14=Cyc_Port_var();_T15=Cyc_Port_fat_ct();_T16=Cyc_Port_var();_T17=Cyc_Port_var();_T18=Cyc_Port_ptr_ct(_T13,_T14,_T15,_T16,_T17);_T19=Cyc_Port_leq(_T12,_T18);if(!_T19)goto _TL172;_T1A=arg1;_T1B=
Cyc_Port_arith_ct();Cyc_Port_leq(_T1A,_T1B);_T1C=arg2;
return _T1C;
# 1318
_TL172: _T1D=arg1;_T1E=Cyc_Port_arith_ct();Cyc_Port_leq(_T1D,_T1E);_T1F=arg2;_T20=
Cyc_Port_arith_ct();Cyc_Port_leq(_T1F,_T20);_T21=
Cyc_Port_arith_ct();return _T21;}case Cyc_Absyn_Minus:
# 1327
 if(arg2s!=0)goto _TL174;_T22=arg1;_T23=
# 1329
Cyc_Port_arith_ct();Cyc_Port_leq(_T22,_T23);_T24=
Cyc_Port_arith_ct();return _T24;
# 1332
_TL174: _T25=arg2s;_T26=_T25->hd;{void*arg2=Cyc_Port_compress_ct(_T26);_T27=arg1;_T28=
Cyc_Port_var();_T29=Cyc_Port_var();_T2A=Cyc_Port_fat_ct();_T2B=Cyc_Port_var();_T2C=Cyc_Port_var();_T2D=Cyc_Port_ptr_ct(_T28,_T29,_T2A,_T2B,_T2C);_T2E=Cyc_Port_leq(_T27,_T2D);if(!_T2E)goto _TL176;_T2F=arg2;_T30=
Cyc_Port_var();_T31=Cyc_Port_var();_T32=Cyc_Port_fat_ct();_T33=Cyc_Port_var();_T34=Cyc_Port_var();_T35=Cyc_Port_ptr_ct(_T30,_T31,_T32,_T33,_T34);_T36=Cyc_Port_leq(_T2F,_T35);if(!_T36)goto _TL178;_T37=
Cyc_Port_arith_ct();return _T37;
# 1337
_TL178: _T38=arg2;_T39=Cyc_Port_arith_ct();Cyc_Port_leq(_T38,_T39);_T3A=arg1;
return _T3A;
# 1341
_TL176: _T3B=arg1;_T3C=Cyc_Port_arith_ct();Cyc_Port_leq(_T3B,_T3C);_T3D=arg1;_T3E=
Cyc_Port_arith_ct();Cyc_Port_leq(_T3D,_T3E);_T3F=
Cyc_Port_arith_ct();return _T3F;}case Cyc_Absyn_Times: goto _LL8;case Cyc_Absyn_Div: _LL8: goto _LLA;case Cyc_Absyn_Mod: _LLA: goto _LLC;case Cyc_Absyn_Eq: _LLC: goto _LLE;case Cyc_Absyn_Neq: _LLE: goto _LL10;case Cyc_Absyn_Lt: _LL10: goto _LL12;case Cyc_Absyn_Gt: _LL12: goto _LL14;case Cyc_Absyn_Lte: _LL14: goto _LL16;case Cyc_Absyn_Gte: _LL16: goto _LL18;case Cyc_Absyn_Bitand: _LL18: goto _LL1A;case Cyc_Absyn_Bitor: _LL1A: goto _LL1C;case Cyc_Absyn_Bitxor: _LL1C: goto _LL1E;case Cyc_Absyn_Bitlshift: _LL1E: goto _LL20;case Cyc_Absyn_Bitlrshift: _LL20: goto _LL22;case Cyc_Absyn_UDiv: _LL22: goto _LL24;case Cyc_Absyn_UMod: _LL24: goto _LL26;case Cyc_Absyn_ULt: _LL26: goto _LL28;case Cyc_Absyn_UGt: _LL28: goto _LL2A;case Cyc_Absyn_ULte: _LL2A: goto _LL2C;case Cyc_Absyn_UGte: _LL2C: _T40=arg1;_T41=
# 1350
Cyc_Port_arith_ct();Cyc_Port_leq(_T40,_T41);_T42=
_check_null(arg2s);_T43=_T42->hd;_T44=Cyc_Port_arith_ct();Cyc_Port_leq(_T43,_T44);_T45=
Cyc_Port_arith_ct();return _T45;case Cyc_Absyn_Not: goto _LL30;case Cyc_Absyn_Bitnot: _LL30: _T46=arg1;_T47=
# 1355
Cyc_Port_arith_ct();Cyc_Port_leq(_T46,_T47);_T48=
Cyc_Port_arith_ct();return _T48;default: _T4A=Cyc_Warn_impos;{
int(*_T4D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T4A;_T49=_T4D;}_T4B=_tag_fat("Port: unknown primop",sizeof(char),21U);_T4C=_tag_fat(0U,sizeof(void*),0);_T49(_T4B,_T4C);};}}}
# 1362
static struct _tuple12 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e,int takeaddress){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;void*_T3;struct _tuple0*_T4;struct Cyc_Port_Cenv*_T5;unsigned _T6;struct Cyc_Absyn_Exp*_T7;unsigned _T8;int _T9;struct _tuple12 _TA;void*_TB;void*_TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;struct _tuple12 _T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;void*_T18;void*_T19;struct _tuple12 _T1A;void*_T1B;struct Cyc_List_List*_T1C;struct Cyc_Port_Cfield*_T1D;struct _fat_ptr _T1E;void*_T1F;struct _tuple12 _T20;void*_T21;struct Cyc_List_List*_T22;struct Cyc_Port_Cfield*_T23;struct _fat_ptr _T24;void*_T25;void*_T26;void*_T27;void*_T28;void*_T29;void*_T2A;struct _tuple12 _T2B;int(*_T2C)(struct _fat_ptr,struct _fat_ptr);void*(*_T2D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2E;struct Cyc_String_pa_PrintArg_struct _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct _fat_ptr _T32;_T0=e;{
void*_T33=_T0->r;struct _fat_ptr*_T34;struct Cyc_Absyn_Exp*_T35;struct Cyc_Absyn_Exp*_T36;void*_T37;_T1=(int*)_T33;_T2=*_T1;switch(_T2){case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T38=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T33;_T3=_T38->f1;_T37=(void*)_T3;}{void*b=_T37;
# 1365
struct _tuple0*x=Cyc_Absyn_binding2qvar(b);_T4=x;
(*_T4).f0=Cyc_Absyn_Loc_n();{
struct _tuple14 _T38=Cyc_Port_lookup_var(env,x);unsigned _T39;struct _tuple12 _T3A;_T3A=_T38.f0;_T39=_T38.f1;{struct _tuple12 p=_T3A;unsigned loc=_T39;_T5=env;_T6=loc;_T7=e;_T8=_T7->loc;_T9=takeaddress;
Cyc_Port_register_localvar_usage(_T5,_T6,_T8,_T9);_TA=p;
return _TA;}}}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T38=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_T33;_T36=_T38->f1;_T35=_T38->f2;}{struct Cyc_Absyn_Exp*e1=_T36;struct Cyc_Absyn_Exp*e2=_T35;
# 1371
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*t1=Cyc_Port_gen_exp(0,env,e1);_TB=
Cyc_Port_gen_exp(0,env,e2);_TC=Cyc_Port_arith_ct();Cyc_Port_leq(_TB,_TC);_TD=v;_TE=q;_TF=
Cyc_Port_fat_ct();_T10=Cyc_Port_var();_T11=Cyc_Port_var();{void*p=Cyc_Port_ptr_ct(_TD,_TE,_TF,_T10,_T11);
Cyc_Port_leq(t1,p);{struct _tuple12 _T38;
_T38.f0=q;_T38.f1=v;_T12=_T38;}return _T12;}}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T38=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T33;_T36=_T38->f1;}{struct Cyc_Absyn_Exp*e1=_T36;
# 1379
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();_T13=v;_T14=q;_T15=
Cyc_Port_var();_T16=Cyc_Port_var();_T17=Cyc_Port_var();{void*p=Cyc_Port_ptr_ct(_T13,_T14,_T15,_T16,_T17);_T18=
Cyc_Port_gen_exp(0,env,e1);_T19=p;Cyc_Port_leq(_T18,_T19);{struct _tuple12 _T38;
_T38.f0=q;_T38.f1=v;_T1A=_T38;}return _T1A;}}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T38=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_T33;_T36=_T38->f1;_T34=_T38->f2;}{struct Cyc_Absyn_Exp*e1=_T36;struct _fat_ptr*f=_T34;
# 1385
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*t1=Cyc_Port_gen_exp(takeaddress,env,e1);_T1B=t1;{struct Cyc_Port_Cfield*_T38[1];{struct Cyc_Port_Cfield*_T39=_cycalloc(sizeof(struct Cyc_Port_Cfield));
_T39->qual=q;_T39->f=f;_T39->type=v;_T1D=(struct Cyc_Port_Cfield*)_T39;}_T38[0]=_T1D;_T1E=_tag_fat(_T38,sizeof(struct Cyc_Port_Cfield*),1);_T1C=Cyc_List_list(_T1E);}_T1F=Cyc_Port_unknown_aggr_ct(_T1C);Cyc_Port_leq(_T1B,_T1F);{struct _tuple12 _T38;
_T38.f0=q;_T38.f1=v;_T20=_T38;}return _T20;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T38=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T33;_T36=_T38->f1;_T34=_T38->f2;}{struct Cyc_Absyn_Exp*e1=_T36;struct _fat_ptr*f=_T34;
# 1391
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*t1=Cyc_Port_gen_exp(0,env,e1);_T21=t1;{struct Cyc_Port_Cfield*_T38[1];{struct Cyc_Port_Cfield*_T39=_cycalloc(sizeof(struct Cyc_Port_Cfield));
_T39->qual=q;_T39->f=f;_T39->type=v;_T23=(struct Cyc_Port_Cfield*)_T39;}_T38[0]=_T23;_T24=_tag_fat(_T38,sizeof(struct Cyc_Port_Cfield*),1);_T22=Cyc_List_list(_T24);}_T25=Cyc_Port_unknown_aggr_ct(_T22);_T26=
Cyc_Port_var();_T27=Cyc_Port_var();_T28=Cyc_Port_var();_T29=Cyc_Port_var();_T2A=
# 1394
Cyc_Port_ptr_ct(_T25,_T26,_T27,_T28,_T29);Cyc_Port_leq(_T21,_T2A);{struct _tuple12 _T38;
# 1396
_T38.f0=q;_T38.f1=v;_T2B=_T38;}return _T2B;}default: _T2D=Cyc_Warn_impos;{
int(*_T38)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2D;_T2C=_T38;}{struct Cyc_String_pa_PrintArg_struct _T38;_T38.tag=0;_T38.f1=Cyc_Absynpp_exp2string(e);_T2F=_T38;}{struct Cyc_String_pa_PrintArg_struct _T38=_T2F;void*_T39[1];_T39[0]=& _T38;_T30=_tag_fat("gen_lhs: %s",sizeof(char),12U);_T31=_tag_fat(_T39,sizeof(void*),1);_T2E=Cyc_aprintf(_T30,_T31);}_T32=_tag_fat(0U,sizeof(void*),0);_T2C(_T2E,_T32);};}}
# 1402
static void*Cyc_Port_gen_exp_false(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){void*_T0;_T0=
Cyc_Port_gen_exp(0,env,e);return _T0;}
# 1405
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_Wstring_c _T5;unsigned _T6;void*_T7;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T8;union Cyc_Absyn_Cnst _T9;struct _union_Cnst_Int_c _TA;struct _tuple5 _TB;int _TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;void*_T1C;struct _tuple0*_T1D;struct _tuple12 _T1E;int _T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;void*_T25;int _T26;void*_T27;void*_T28;void*_T29;void*_T2A;void*_T2B;void*_T2C;struct Cyc_Port_Cenv*_T2D;unsigned _T2E;struct Cyc_Absyn_Exp*_T2F;unsigned _T30;int _T31;void*_T32;struct Cyc_Port_Cenv*_T33;enum Cyc_Absyn_Primop _T34;struct Cyc_List_List*(*_T35)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*);struct Cyc_List_List*(*_T36)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Port_Cenv*_T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;void*_T3A;void*_T3B;void*_T3C;struct Cyc_Core_Opt*_T3D;unsigned _T3E;struct Cyc_List_List _T3F;struct Cyc_List_List _T40;struct Cyc_List_List*_T41;struct Cyc_Port_Cenv*_T42;struct Cyc_Core_Opt*_T43;void*_T44;enum Cyc_Absyn_Primop _T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;void*_T48;void*_T49;void*_T4A;void*_T4B;void*_T4C;void*_T4D;void*_T4E;void*_T4F;void*_T50;void*_T51;void*_T52;void*_T53;void*_T54;void*_T55;void*_T56;void*_T57;void*_T58;void*_T59;void*_T5A;void*_T5B;void*_T5C;void*_T5D;void*_T5E;void*_T5F;void*_T60;void*_T61;void*_T62;struct Cyc_List_List*(*_T63)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*);struct Cyc_List_List*(*_T64)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Port_Cenv*_T65;struct Cyc_List_List*_T66;struct Cyc_List_List*_T67;void*_T68;void*_T69;void*_T6A;void*_T6B;void*_T6C;void*_T6D;void*_T6E;struct Cyc_List_List*_T6F;void*_T70;struct Cyc_List_List*_T71;void*_T72;struct Cyc_List_List*_T73;struct Cyc_List_List*_T74;void*_T75;void*_T76;void*_T77;void*_T78;void*_T79;void*_T7A;void*_T7B;void*_T7C;void*_T7D;void*_T7E;void*_T7F;void*_T80;void*_T81;void*_T82;void*_T83;void*_T84;void*_T85;void*_T86;void*_T87;void*_T88;void*_T89;void*_T8A;void*_T8B;void*_T8C;void*_T8D;void*_T8E;void*_T8F;void*_T90;void*_T91;void*_T92;struct Cyc_List_List*_T93;struct Cyc_Port_Cfield*_T94;struct _fat_ptr _T95;void*_T96;void*_T97;void*_T98;struct Cyc_List_List*_T99;struct Cyc_Port_Cfield*_T9A;struct _fat_ptr _T9B;void*_T9C;void*_T9D;void*_T9E;void*_T9F;void*_TA0;void*_TA1;void*_TA2;struct Cyc_Absyn_MallocInfo _TA3;struct Cyc_Absyn_MallocInfo _TA4;void*_TA5;void*_TA6;void*_TA7;void**_TA8;unsigned _TA9;struct Cyc_Port_Cenv*_TAA;void**_TAB;void*_TAC;void*_TAD;void*_TAE;void*_TAF;void*_TB0;void*_TB1;void*_TB2;int(*_TB3)(struct _fat_ptr,struct _fat_ptr);void*(*_TB4)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TB5;struct _fat_ptr _TB6;int(*_TB7)(struct _fat_ptr,struct _fat_ptr);void*(*_TB8)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TB9;struct _fat_ptr _TBA;int(*_TBB)(struct _fat_ptr,struct _fat_ptr);void*(*_TBC)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TBD;struct _fat_ptr _TBE;int(*_TBF)(struct _fat_ptr,struct _fat_ptr);void*(*_TC0)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC1;struct _fat_ptr _TC2;int(*_TC3)(struct _fat_ptr,struct _fat_ptr);void*(*_TC4)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC5;struct _fat_ptr _TC6;int(*_TC7)(struct _fat_ptr,struct _fat_ptr);void*(*_TC8)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC9;struct _fat_ptr _TCA;int(*_TCB)(struct _fat_ptr,struct _fat_ptr);void*(*_TCC)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TCD;struct _fat_ptr _TCE;int(*_TCF)(struct _fat_ptr,struct _fat_ptr);void*(*_TD0)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TD1;struct _fat_ptr _TD2;int(*_TD3)(struct _fat_ptr,struct _fat_ptr);void*(*_TD4)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TD5;struct _fat_ptr _TD6;int(*_TD7)(struct _fat_ptr,struct _fat_ptr);void*(*_TD8)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TD9;struct _fat_ptr _TDA;int(*_TDB)(struct _fat_ptr,struct _fat_ptr);void*(*_TDC)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TDD;struct _fat_ptr _TDE;int(*_TDF)(struct _fat_ptr,struct _fat_ptr);void*(*_TE0)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TE1;struct _fat_ptr _TE2;int(*_TE3)(struct _fat_ptr,struct _fat_ptr);void*(*_TE4)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TE5;struct _fat_ptr _TE6;int(*_TE7)(struct _fat_ptr,struct _fat_ptr);void*(*_TE8)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TE9;struct _fat_ptr _TEA;int(*_TEB)(struct _fat_ptr,struct _fat_ptr);void*(*_TEC)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TED;struct _fat_ptr _TEE;int(*_TEF)(struct _fat_ptr,struct _fat_ptr);void*(*_TF0)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TF1;struct _fat_ptr _TF2;int(*_TF3)(struct _fat_ptr,struct _fat_ptr);void*(*_TF4)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TF5;struct _fat_ptr _TF6;int(*_TF7)(struct _fat_ptr,struct _fat_ptr);void*(*_TF8)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TF9;struct _fat_ptr _TFA;int(*_TFB)(struct _fat_ptr,struct _fat_ptr);void*(*_TFC)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TFD;struct _fat_ptr _TFE;_T0=e;{
void*_TFF=_T0->r;struct Cyc_Absyn_Stmt*_T100;void**_T101;struct _fat_ptr*_T102;struct Cyc_List_List*_T103;struct Cyc_Absyn_Exp*_T104;enum Cyc_Absyn_Incrementor _T105;struct Cyc_Absyn_Exp*_T106;struct Cyc_Core_Opt*_T107;enum Cyc_Absyn_Primop _T108;void*_T109;_T1=(int*)_TFF;_T2=*_T1;switch(_T2){case 0: _T3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TFF;_T4=_T3->f1;_T5=_T4.Wstring_c;_T6=_T5.tag;switch(_T6){case 2: goto _LL4;case 3: _LL4: goto _LL6;case 4: _LL6: goto _LL8;case 6: _LL8: goto _LLA;case 7: _LL14: _T7=
# 1416
Cyc_Port_arith_ct();return _T7;case 5: _T8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TFF;_T9=_T8->f1;_TA=_T9.Int_c;_TB=_TA.val;_TC=_TB.f1;if(_TC!=0)goto _TL17D;_TD=
Cyc_Port_zero_ct();return _TD;_TL17D: _TE=
Cyc_Port_arith_ct();return _TE;case 8: _TF=
# 1420
Cyc_Port_arith_ct();_T10=Cyc_Port_const_ct();_T11=Cyc_Port_fat_ct();_T12=Cyc_Port_heap_ct();_T13=Cyc_Port_zterm_ct();_T14=Cyc_Port_ptr_ct(_TF,_T10,_T11,_T12,_T13);return _T14;case 9: _T15=
# 1422
Cyc_Port_arith_ct();_T16=Cyc_Port_const_ct();_T17=Cyc_Port_fat_ct();_T18=Cyc_Port_heap_ct();_T19=Cyc_Port_zterm_ct();_T1A=Cyc_Port_ptr_ct(_T15,_T16,_T17,_T18,_T19);return _T1A;default: _T1B=
Cyc_Port_zero_ct();return _T1B;};case 17: _LLA: goto _LLC;case 18: _LLC: goto _LLE;case 19: _LLE: goto _LL10;case 31: _LL10: goto _LL12;case 32: _LL12: goto _LL14;case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_TFF;_T1C=_T10A->f1;_T109=(void*)_T1C;}{void*b=_T109;
# 1425
struct _tuple0*x=Cyc_Absyn_binding2qvar(b);_T1D=x;
(*_T1D).f0=Cyc_Absyn_Loc_n();{
struct _tuple14 _T10A=Cyc_Port_lookup_var(env,x);unsigned _T10B;struct _tuple12 _T10C;_T10C=_T10A.f0;_T10B=_T10A.f1;{struct _tuple12 p=_T10C;unsigned loc=_T10B;void*_T10D;_T1E=p;_T10D=_T1E.f1;{void*t=_T10D;_T1F=
# 1429
Cyc_Port_isfn(env,x);if(!_T1F)goto _TL17F;
t=Cyc_Port_instantiate(t);_T20=t;_T21=
Cyc_Port_var();_T22=Cyc_Port_var();_T23=Cyc_Port_heap_ct();_T24=Cyc_Port_nozterm_ct();_T25=Cyc_Port_ptr_ct(_T20,_T21,_T22,_T23,_T24);return _T25;
# 1433
_TL17F: _T26=Cyc_Port_isarray(env,x);if(!_T26)goto _TL181;{
void*elt_type=Cyc_Port_var();
void*qual=Cyc_Port_var();
void*zt=Cyc_Port_var();
void*array_type=Cyc_Port_array_ct(elt_type,qual,zt);
Cyc_Port_unifies(t,array_type);_T27=elt_type;_T28=qual;_T29=
Cyc_Port_fat_ct();_T2A=Cyc_Port_var();_T2B=zt;{void*ptr_type=Cyc_Port_ptr_ct(_T27,_T28,_T29,_T2A,_T2B);_T2C=ptr_type;
return _T2C;}}
# 1442
_TL181: _T2D=env;_T2E=loc;_T2F=e;_T30=_T2F->loc;_T31=takeaddress;Cyc_Port_register_localvar_usage(_T2D,_T2E,_T30,_T31);_T32=t;
return _T32;}}}}case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_TFF;_T108=_T10A->f1;_T109=_T10A->f2;}{enum Cyc_Absyn_Primop p=_T108;struct Cyc_List_List*es=_T109;_T33=env;_T34=p;_T36=Cyc_List_map_c;{
# 1445
struct Cyc_List_List*(*_T10A)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*))_T36;_T35=_T10A;}_T37=env;_T38=es;_T39=_T35(Cyc_Port_gen_exp_false,_T37,_T38);_T3A=Cyc_Port_gen_primop(_T33,_T34,_T39);return _T3A;}case 4:{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;_T107=_T10A->f2;_T106=_T10A->f3;}{struct Cyc_Absyn_Exp*e1=_T109;struct Cyc_Core_Opt*popt=_T107;struct Cyc_Absyn_Exp*e2=_T106;
# 1447
struct _tuple12 _T10A=Cyc_Port_gen_lhs(env,e1,0);void*_T10B;void*_T10C;_T10C=_T10A.f0;_T10B=_T10A.f1;{void*q=_T10C;void*t1=_T10B;_T3B=q;_T3C=
Cyc_Port_notconst_ct();Cyc_Port_unifies(_T3B,_T3C);{
void*t2=Cyc_Port_gen_exp(0,env,e2);_T3D=popt;_T3E=(unsigned)_T3D;
if(!_T3E)goto _TL183;{struct Cyc_List_List _T10D;
_T10D.hd=t2;_T10D.tl=0;_T3F=_T10D;}{struct Cyc_List_List x2=_T3F;{struct Cyc_List_List _T10D;
_T10D.hd=t1;_T41=& x2;_T10D.tl=(struct Cyc_List_List*)_T41;_T40=_T10D;}{struct Cyc_List_List x1=_T40;_T42=env;_T43=popt;_T44=_T43->v;_T45=(enum Cyc_Absyn_Primop)_T44;_T46=& x1;_T47=(struct Cyc_List_List*)_T46;{
void*t3=Cyc_Port_gen_primop(_T42,_T45,_T47);
Cyc_Port_leq(t3,t1);_T48=t1;
return _T48;}}}
# 1457
_TL183: Cyc_Port_leq(t2,t1);_T49=t1;
return _T49;}}}case 5:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;_T105=_T10A->f2;}{struct Cyc_Absyn_Exp*e1=_T109;enum Cyc_Absyn_Incrementor inc=_T105;
# 1461
struct _tuple12 _T10A=Cyc_Port_gen_lhs(env,e1,0);void*_T10B;void*_T10C;_T10C=_T10A.f0;_T10B=_T10A.f1;{void*q=_T10C;void*t=_T10B;_T4A=q;_T4B=
Cyc_Port_notconst_ct();Cyc_Port_unifies(_T4A,_T4B);_T4C=t;_T4D=
# 1464
Cyc_Port_var();_T4E=Cyc_Port_var();_T4F=Cyc_Port_fat_ct();_T50=Cyc_Port_var();_T51=Cyc_Port_var();_T52=Cyc_Port_ptr_ct(_T4D,_T4E,_T4F,_T50,_T51);Cyc_Port_leq(_T4C,_T52);_T53=t;_T54=
Cyc_Port_arith_ct();Cyc_Port_leq(_T53,_T54);_T55=t;
return _T55;}}case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;_T106=_T10A->f2;_T104=_T10A->f3;}{struct Cyc_Absyn_Exp*e1=_T109;struct Cyc_Absyn_Exp*e2=_T106;struct Cyc_Absyn_Exp*e3=_T104;
# 1468
void*v=Cyc_Port_var();_T56=
Cyc_Port_gen_exp(0,env,e1);_T57=Cyc_Port_arith_ct();Cyc_Port_leq(_T56,_T57);_T58=
Cyc_Port_gen_exp(0,env,e2);_T59=v;Cyc_Port_leq(_T58,_T59);_T5A=
Cyc_Port_gen_exp(0,env,e3);_T5B=v;Cyc_Port_leq(_T5A,_T5B);_T5C=v;
return _T5C;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;_T106=_T10A->f2;}{struct Cyc_Absyn_Exp*e1=_T109;struct Cyc_Absyn_Exp*e2=_T106;_T109=e1;_T106=e2;goto _LL2C;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;_T106=_T10A->f2;}_LL2C: {struct Cyc_Absyn_Exp*e1=_T109;struct Cyc_Absyn_Exp*e2=_T106;_T5D=
# 1475
Cyc_Port_gen_exp(0,env,e1);_T5E=Cyc_Port_arith_ct();Cyc_Port_leq(_T5D,_T5E);_T5F=
Cyc_Port_gen_exp(0,env,e2);_T60=Cyc_Port_arith_ct();Cyc_Port_leq(_T5F,_T60);_T61=
Cyc_Port_arith_ct();return _T61;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;_T106=_T10A->f2;}{struct Cyc_Absyn_Exp*e1=_T109;struct Cyc_Absyn_Exp*e2=_T106;
# 1479
Cyc_Port_gen_exp(0,env,e1);_T62=
Cyc_Port_gen_exp(0,env,e2);return _T62;}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;_T103=_T10A->f2;}{struct Cyc_Absyn_Exp*e1=_T109;struct Cyc_List_List*es=_T103;
# 1482
void*v=Cyc_Port_var();
void*t1=Cyc_Port_gen_exp(0,env,e1);_T64=Cyc_List_map_c;{
struct Cyc_List_List*(*_T10A)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*))_T64;_T63=_T10A;}_T65=env;_T66=es;{struct Cyc_List_List*ts=_T63(Cyc_Port_gen_exp_false,_T65,_T66);_T67=ts;{
struct Cyc_List_List*vs=Cyc_List_map(Cyc_Port_new_var,_T67);_T68=t1;_T69=
Cyc_Port_fn_ct(v,vs);_T6A=Cyc_Port_var();_T6B=Cyc_Port_var();_T6C=Cyc_Port_var();_T6D=Cyc_Port_nozterm_ct();_T6E=Cyc_Port_ptr_ct(_T69,_T6A,_T6B,_T6C,_T6D);Cyc_Port_unifies(_T68,_T6E);
_TL188: if(ts!=0)goto _TL186;else{goto _TL187;}
_TL186: _T6F=ts;_T70=_T6F->hd;_T71=_check_null(vs);_T72=_T71->hd;Cyc_Port_leq(_T70,_T72);_T73=ts;
# 1487
ts=_T73->tl;_T74=vs;vs=_T74->tl;goto _TL188;_TL187: _T75=v;
# 1489
return _T75;}}}case 40:{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;}{struct Cyc_Absyn_Exp*e1=_T109;_T76=
Cyc_Port_gen_exp(0,env,e1);return _T76;}case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_TFF;_T109=_T10A->f1;}{struct Cyc_Absyn_Exp*e1=_T109;_T77=
Cyc_Port_gen_exp(0,env,e1);return _T77;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TFF;_T78=_T10A->f1;_T109=(void*)_T78;_T106=_T10A->f2;}{void*t=_T109;struct Cyc_Absyn_Exp*e1=_T106;
# 1493
void*t1=Cyc_Port_type_to_ctype(env,t);
void*t2=Cyc_Port_gen_exp(0,env,e1);
Cyc_Port_leq(t1,t2);_T79=t2;
return _T79;}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_TFF;_T106=_T10A->f1;}{struct Cyc_Absyn_Exp*e1=_T106;
# 1498
struct _tuple12 _T10A=Cyc_Port_gen_lhs(env,e1,1);void*_T10B;void*_T10C;_T10C=_T10A.f0;_T10B=_T10A.f1;{void*q=_T10C;void*t=_T10B;_T7A=t;_T7B=q;_T7C=
Cyc_Port_var();_T7D=Cyc_Port_var();_T7E=Cyc_Port_var();_T7F=Cyc_Port_ptr_ct(_T7A,_T7B,_T7C,_T7D,_T7E);return _T7F;}}case 23:{struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_TFF;_T106=_T10A->f1;_T104=_T10A->f2;}{struct Cyc_Absyn_Exp*e1=_T106;struct Cyc_Absyn_Exp*e2=_T104;
# 1501
void*v=Cyc_Port_var();_T80=
Cyc_Port_gen_exp(0,env,e2);_T81=Cyc_Port_arith_ct();Cyc_Port_leq(_T80,_T81);{
void*t=Cyc_Port_gen_exp(0,env,e1);_T82=t;_T83=v;_T84=
Cyc_Port_var();_T85=Cyc_Port_fat_ct();_T86=Cyc_Port_var();_T87=Cyc_Port_var();_T88=Cyc_Port_ptr_ct(_T83,_T84,_T85,_T86,_T87);Cyc_Port_leq(_T82,_T88);_T89=v;
return _T89;}}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_TFF;_T106=_T10A->f1;}{struct Cyc_Absyn_Exp*e1=_T106;
# 1507
void*v=Cyc_Port_var();_T8A=
Cyc_Port_gen_exp(0,env,e1);_T8B=v;_T8C=Cyc_Port_var();_T8D=Cyc_Port_var();_T8E=Cyc_Port_var();_T8F=Cyc_Port_var();_T90=Cyc_Port_ptr_ct(_T8B,_T8C,_T8D,_T8E,_T8F);Cyc_Port_leq(_T8A,_T90);_T91=v;
return _T91;}case 21:{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_TFF;_T106=_T10A->f1;_T102=_T10A->f2;}{struct Cyc_Absyn_Exp*e1=_T106;struct _fat_ptr*f=_T102;
# 1511
void*v=Cyc_Port_var();
void*t=Cyc_Port_gen_exp(takeaddress,env,e1);_T92=t;{struct Cyc_Port_Cfield*_T10A[1];{struct Cyc_Port_Cfield*_T10B=_cycalloc(sizeof(struct Cyc_Port_Cfield));
_T10B->qual=Cyc_Port_var();_T10B->f=f;_T10B->type=v;_T94=(struct Cyc_Port_Cfield*)_T10B;}_T10A[0]=_T94;_T95=_tag_fat(_T10A,sizeof(struct Cyc_Port_Cfield*),1);_T93=Cyc_List_list(_T95);}_T96=Cyc_Port_unknown_aggr_ct(_T93);Cyc_Port_leq(_T92,_T96);_T97=v;
return _T97;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_TFF;_T106=_T10A->f1;_T102=_T10A->f2;}{struct Cyc_Absyn_Exp*e1=_T106;struct _fat_ptr*f=_T102;
# 1516
void*v=Cyc_Port_var();
void*t=Cyc_Port_gen_exp(0,env,e1);_T98=
Cyc_Port_gen_exp(0,env,e1);{struct Cyc_Port_Cfield*_T10A[1];{struct Cyc_Port_Cfield*_T10B=_cycalloc(sizeof(struct Cyc_Port_Cfield));_T10B->qual=Cyc_Port_var();_T10B->f=f;_T10B->type=v;_T9A=(struct Cyc_Port_Cfield*)_T10B;}_T10A[0]=_T9A;_T9B=_tag_fat(_T10A,sizeof(struct Cyc_Port_Cfield*),1);_T99=Cyc_List_list(_T9B);}_T9C=Cyc_Port_unknown_aggr_ct(_T99);_T9D=
Cyc_Port_var();_T9E=Cyc_Port_var();_T9F=Cyc_Port_var();_TA0=Cyc_Port_var();_TA1=
# 1518
Cyc_Port_ptr_ct(_T9C,_T9D,_T9E,_T9F,_TA0);Cyc_Port_leq(_T98,_TA1);_TA2=v;
# 1520
return _TA2;}case 33:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_TFF;_TA3=_T10A->f1;_T101=_TA3.elt_type;_TA4=_T10A->f1;_T106=_TA4.num_elts;}{void**topt=_T101;struct Cyc_Absyn_Exp*e1=_T106;_TA5=
# 1522
Cyc_Port_gen_exp(0,env,e1);_TA6=Cyc_Port_arith_ct();Cyc_Port_leq(_TA5,_TA6);_TA8=topt;_TA9=(unsigned)_TA8;
if(!_TA9)goto _TL189;_TAA=env;_TAB=topt;_TAC=*_TAB;_TA7=Cyc_Port_type_to_ctype(_TAA,_TAC);goto _TL18A;_TL189: _TA7=Cyc_Port_var();_TL18A: {void*t=_TA7;_TAD=t;_TAE=
Cyc_Port_var();_TAF=Cyc_Port_fat_ct();_TB0=Cyc_Port_heap_ct();_TB1=Cyc_Port_var();_TB2=Cyc_Port_ptr_ct(_TAD,_TAE,_TAF,_TB0,_TB1);return _TB2;}}case 41: _TB4=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB4;_TB3=_T10A;}_TB5=_tag_fat("@assert",sizeof(char),8U);_TB6=_tag_fat(0U,sizeof(void*),0);_TB3(_TB5,_TB6);case 42: _TB8=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB8;_TB7=_T10A;}_TB9=_tag_fat("@assert_false",sizeof(char),14U);_TBA=_tag_fat(0U,sizeof(void*),0);_TB7(_TB9,_TBA);case 11: _TBC=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TBC;_TBB=_T10A;}_TBD=_tag_fat("throw",sizeof(char),6U);_TBE=_tag_fat(0U,sizeof(void*),0);_TBB(_TBD,_TBE);case 13: _TC0=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TC0;_TBF=_T10A;}_TC1=_tag_fat("instantiate",sizeof(char),12U);_TC2=_tag_fat(0U,sizeof(void*),0);_TBF(_TC1,_TC2);case 2: _TC4=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TC4;_TC3=_T10A;}_TC5=_tag_fat("Pragma_e",sizeof(char),9U);_TC6=_tag_fat(0U,sizeof(void*),0);_TC3(_TC5,_TC6);case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_TFF;_T106=_T10A->f1;_T104=_T10A->f2;}{struct Cyc_Absyn_Exp*e1=_T106;struct Cyc_Absyn_Exp*e2=_T104;_TC8=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TC8;_TC7=_T10A;}_TC9=_tag_fat("Swap_e",sizeof(char),7U);_TCA=_tag_fat(0U,sizeof(void*),0);_TC7(_TC9,_TCA);}case 16: _TCC=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TCC;_TCB=_T10A;}_TCD=_tag_fat("New_e",sizeof(char),6U);_TCE=_tag_fat(0U,sizeof(void*),0);_TCB(_TCD,_TCE);case 24: _TD0=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TD0;_TCF=_T10A;}_TD1=_tag_fat("CompoundLit_e",sizeof(char),14U);_TD2=_tag_fat(0U,sizeof(void*),0);_TCF(_TD1,_TD2);case 25: _TD4=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TD4;_TD3=_T10A;}_TD5=_tag_fat("Array_e",sizeof(char),8U);_TD6=_tag_fat(0U,sizeof(void*),0);_TD3(_TD5,_TD6);case 26: _TD8=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TD8;_TD7=_T10A;}_TD9=_tag_fat("Comprehension_e",sizeof(char),16U);_TDA=_tag_fat(0U,sizeof(void*),0);_TD7(_TD9,_TDA);case 27: _TDC=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TDC;_TDB=_T10A;}_TDD=_tag_fat("ComprehensionNoinit_e",sizeof(char),22U);_TDE=_tag_fat(0U,sizeof(void*),0);_TDB(_TDD,_TDE);case 28: _TE0=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TE0;_TDF=_T10A;}_TE1=_tag_fat("Aggregate_e",sizeof(char),12U);_TE2=_tag_fat(0U,sizeof(void*),0);_TDF(_TE1,_TE2);case 29: _TE4=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TE4;_TE3=_T10A;}_TE5=_tag_fat("AnonStruct_e",sizeof(char),13U);_TE6=_tag_fat(0U,sizeof(void*),0);_TE3(_TE5,_TE6);case 30: _TE8=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TE8;_TE7=_T10A;}_TE9=_tag_fat("Datatype_e",sizeof(char),11U);_TEA=_tag_fat(0U,sizeof(void*),0);_TE7(_TE9,_TEA);case 35: _TEC=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TEC;_TEB=_T10A;}_TED=_tag_fat("UnresolvedMem_e",sizeof(char),16U);_TEE=_tag_fat(0U,sizeof(void*),0);_TEB(_TED,_TEE);case 36:{struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_T10A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_TFF;_T100=_T10A->f1;}{struct Cyc_Absyn_Stmt*s=_T100;_TF0=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF0;_TEF=_T10A;}_TF1=_tag_fat("StmtExp_e",sizeof(char),10U);_TF2=_tag_fat(0U,sizeof(void*),0);_TEF(_TF1,_TF2);}case 38: _TF4=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF4;_TF3=_T10A;}_TF5=_tag_fat("Valueof_e",sizeof(char),10U);_TF6=_tag_fat(0U,sizeof(void*),0);_TF3(_TF5,_TF6);case 39: _TF8=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF8;_TF7=_T10A;}_TF9=_tag_fat("Asm_e",sizeof(char),6U);_TFA=_tag_fat(0U,sizeof(void*),0);_TF7(_TF9,_TFA);default: _TFC=Cyc_Warn_impos;{
int(*_T10A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TFC;_TFB=_T10A;}_TFD=_tag_fat("Tagcheck_e",sizeof(char),11U);_TFE=_tag_fat(0U,sizeof(void*),0);_TFB(_TFD,_TFE);};}}
# 1548
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_Exp*_T4;unsigned _T5;void*_T6;void*_T7;struct _tuple9 _T8;void*_T9;void*_TA;struct _tuple9 _TB;struct _tuple9 _TC;void*_TD;void*_TE;void*_TF;void*_T10;struct Cyc_List_List*_T11;unsigned _T12;struct Cyc_Port_Cenv*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_Absyn_Switch_clause*_T16;struct Cyc_Absyn_Stmt*_T17;struct Cyc_List_List*_T18;struct _tuple9 _T19;void*_T1A;void*_T1B;int(*_T1C)(struct _fat_ptr,struct _fat_ptr);void*(*_T1D)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T1E;struct _fat_ptr _T1F;_T0=s;{
void*_T20=_T0->r;struct Cyc_Absyn_Decl*_T21;struct Cyc_List_List*_T22;struct Cyc_Absyn_Exp*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_Absyn_Stmt*_T25;struct Cyc_Absyn_Stmt*_T26;struct Cyc_Absyn_Exp*_T27;_T1=(int*)_T20;_T2=*_T1;switch(_T2){case 0:
 return;case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T20;_T27=_T28->f1;}{struct Cyc_Absyn_Exp*e=_T27;
Cyc_Port_gen_exp(0,env,e);return;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T20;_T26=_T28->f1;_T25=_T28->f2;}{struct Cyc_Absyn_Stmt*s1=_T26;struct Cyc_Absyn_Stmt*s2=_T25;
Cyc_Port_gen_stmt(env,s1);Cyc_Port_gen_stmt(env,s2);return;}case 3:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T20;_T27=_T28->f1;}{struct Cyc_Absyn_Exp*eopt=_T27;
# 1554
void*v=Cyc_Port_lookup_return_type(env);_T4=eopt;_T5=(unsigned)_T4;
if(!_T5)goto _TL18C;_T3=Cyc_Port_gen_exp(0,env,eopt);goto _TL18D;_TL18C: _T3=Cyc_Port_void_ct();_TL18D: {void*t=_T3;
Cyc_Port_leq(t,v);
return;}}case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T20;_T27=_T28->f1;_T26=_T28->f2;_T25=_T28->f3;}{struct Cyc_Absyn_Exp*e=_T27;struct Cyc_Absyn_Stmt*s1=_T26;struct Cyc_Absyn_Stmt*s2=_T25;_T6=
# 1559
Cyc_Port_gen_exp(0,env,e);_T7=Cyc_Port_arith_ct();Cyc_Port_leq(_T6,_T7);
Cyc_Port_gen_stmt(env,s1);
Cyc_Port_gen_stmt(env,s2);
return;}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T20;_T8=_T28->f1;_T27=_T8.f0;_T26=_T28->f2;}{struct Cyc_Absyn_Exp*e=_T27;struct Cyc_Absyn_Stmt*s=_T26;_T9=
# 1564
Cyc_Port_gen_exp(0,env,e);_TA=Cyc_Port_arith_ct();Cyc_Port_leq(_T9,_TA);
Cyc_Port_gen_stmt(env,s);
return;}case 6: goto _LL10;case 7: _LL10: goto _LL12;case 11: _LL12: goto _LL14;case 8: _LL14:
# 1570
 return;case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T20;_T27=_T28->f1;_TB=_T28->f2;_T24=_TB.f0;_TC=_T28->f3;_T23=_TC.f0;_T26=_T28->f4;}{struct Cyc_Absyn_Exp*e1=_T27;struct Cyc_Absyn_Exp*e2=_T24;struct Cyc_Absyn_Exp*e3=_T23;struct Cyc_Absyn_Stmt*s=_T26;
# 1572
Cyc_Port_gen_exp(0,env,e1);_TD=
Cyc_Port_gen_exp(0,env,e2);_TE=Cyc_Port_arith_ct();Cyc_Port_leq(_TD,_TE);
Cyc_Port_gen_exp(0,env,e3);
Cyc_Port_gen_stmt(env,s);
return;}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T20;_T27=_T28->f1;_T22=_T28->f2;}{struct Cyc_Absyn_Exp*e=_T27;struct Cyc_List_List*scs=_T22;_TF=
# 1578
Cyc_Port_gen_exp(0,env,e);_T10=Cyc_Port_arith_ct();Cyc_Port_leq(_TF,_T10);
_TL191: _T11=scs;_T12=(unsigned)_T11;if(_T12)goto _TL18F;else{goto _TL190;}
_TL18F: _T13=env;_T14=scs;_T15=_T14->hd;_T16=(struct Cyc_Absyn_Switch_clause*)_T15;_T17=_T16->body;Cyc_Port_gen_stmt(_T13,_T17);_T18=scs;
# 1579
scs=_T18->tl;goto _TL191;_TL190:
# 1581
 return;}case 12:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T20;_T21=_T28->f1;_T26=_T28->f2;}{struct Cyc_Absyn_Decl*d=_T21;struct Cyc_Absyn_Stmt*s=_T26;
# 1583
env=Cyc_Port_gen_localdecl(env,d,0);Cyc_Port_gen_stmt(env,s);return;}case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T20;_T26=_T28->f2;}{struct Cyc_Absyn_Stmt*s=_T26;
Cyc_Port_gen_stmt(env,s);return;}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T28=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T20;_T26=_T28->f1;_T19=_T28->f2;_T27=_T19.f0;}{struct Cyc_Absyn_Stmt*s=_T26;struct Cyc_Absyn_Exp*e=_T27;
# 1586
Cyc_Port_gen_stmt(env,s);_T1A=
Cyc_Port_gen_exp(0,env,e);_T1B=Cyc_Port_arith_ct();Cyc_Port_leq(_T1A,_T1B);
return;}default: _T1D=Cyc_Warn_impos;{
int(*_T28)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T1D;_T1C=_T28;}_T1E=_tag_fat("try/catch",sizeof(char),10U);_T1F=_tag_fat(0U,sizeof(void*),0);_T1C(_T1E,_T1F);};}}struct _tuple18{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 1594
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;void*_T3;int*_T4;unsigned _T5;void*_T6;struct _tuple0*_T7;void*_T8;struct Cyc_Absyn_ArrayInfo _T9;struct Cyc_Absyn_ArrayInfo _TA;struct Cyc_Absyn_ArrayInfo _TB;struct Cyc_List_List*_TC;void*_TD;struct _tuple18*_TE;struct Cyc_List_List*_TF;unsigned _T10;int(*_T11)(struct _fat_ptr,struct _fat_ptr);void*(*_T12)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T13;struct _fat_ptr _T14;struct Cyc_List_List*_T15;void*_T16;void*_T17;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T18;void*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1D;void*_T1E;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T1F;union Cyc_Absyn_AggrInfo _T20;struct _union_AggrInfo_UnknownAggr _T21;unsigned _T22;void*_T23;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T24;void*_T25;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T26;union Cyc_Absyn_AggrInfo _T27;struct _union_AggrInfo_UnknownAggr _T28;struct _tuple2 _T29;enum Cyc_Absyn_AggrKind _T2A;void*_T2B;void*_T2C;union Cyc_Absyn_AggrInfo _T2D;struct _union_AggrInfo_UnknownAggr _T2E;struct _tuple2 _T2F;struct _tuple0*_T30;struct _tuple11*_T31;int(*_T32)(struct _fat_ptr,struct _fat_ptr);void*(*_T33)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T34;struct _fat_ptr _T35;void*_T36;void*_T37;union Cyc_Absyn_AggrInfo _T38;struct _union_AggrInfo_KnownAggr _T39;struct Cyc_Absyn_Aggrdecl**_T3A;struct Cyc_Absyn_Aggrdecl*_T3B;struct Cyc_Absyn_AggrdeclImpl*_T3C;struct Cyc_Absyn_AggrdeclImpl*_T3D;struct Cyc_List_List*_T3E;void*_T3F;struct _tuple18*_T40;struct Cyc_List_List*_T41;unsigned _T42;int(*_T43)(struct _fat_ptr,struct _fat_ptr);void*(*_T44)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T45;struct _fat_ptr _T46;struct Cyc_List_List*_T47;void*_T48;struct Cyc_List_List*_T49;struct Cyc_Port_Cenv*_T4A;struct Cyc_Absyn_Aggrfield*_T4B;void*_T4C;struct Cyc_Absyn_Exp*_T4D;struct Cyc_List_List*_T4E;void*_T4F;int(*_T50)(struct _fat_ptr,struct _fat_ptr);void*(*_T51)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T52;struct _fat_ptr _T53;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T54;union Cyc_Absyn_Cnst _T55;struct _union_Cnst_Wstring_c _T56;unsigned _T57;void*_T58;int*_T59;unsigned _T5A;void*_T5B;struct _tuple0*_T5C;void*_T5D;void*_T5E;void*_T5F;void*_T60;void*_T61;void*_T62;_T0=e;{
void*_T63=_T0->r;struct Cyc_List_List*_T64;_T1=(int*)_T63;_T2=*_T1;switch(_T2){case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T65=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T63;_T64=_T65->f2;}{struct Cyc_List_List*dles=_T64;_T64=dles;goto _LL4;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T65=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T63;_T64=_T65->f1;}_LL4: {struct Cyc_List_List*dles=_T64;_T64=dles;goto _LL6;}case 24:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T65=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T63;_T64=_T65->f2;}_LL6: {struct Cyc_List_List*dles=_T64;
# 1599
LOOP: {struct Cyc_Absyn_Aggrdecl*_T65;struct _tuple0*_T66;unsigned _T67;void*_T68;void*_T69;_T3=t;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 8: _T6=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T6A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T6;_T69=_T6A->f1;}{struct _tuple0*n=_T69;_T7=n;
# 1602
(*_T7).f0=Cyc_Absyn_Loc_n();
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 5: _T8=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T6A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T8;_T9=_T6A->f1;_T69=_T9.elt_type;_TA=_T6A->f1;_T68=_TA.zero_term;_TB=_T6A->f1;_T67=_TB.zt_loc;}{void*et=_T69;void*zt=_T68;unsigned ztl=_T67;
# 1605
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*z=Cyc_Port_var();
void*t=Cyc_Port_type_to_ctype(env,et);
_TL197: if(dles!=0)goto _TL195;else{goto _TL196;}
_TL195: _TC=dles;_TD=_TC->hd;_TE=(struct _tuple18*)_TD;{struct _tuple18 _T6A=*_TE;struct Cyc_Absyn_Exp*_T6B;struct Cyc_List_List*_T6C;_T6C=_T6A.f0;_T6B=_T6A.f1;{struct Cyc_List_List*d=_T6C;struct Cyc_Absyn_Exp*e=_T6B;_TF=d;_T10=(unsigned)_TF;
if(!_T10)goto _TL198;_T12=Cyc_Warn_impos;{int(*_T6D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T12;_T11=_T6D;}_T13=_tag_fat("designators in initializers",sizeof(char),28U);_T14=_tag_fat(0U,sizeof(void*),0);_T11(_T13,_T14);goto _TL199;_TL198: _TL199: {
void*te=Cyc_Port_gen_initializer(env,et,e);
Cyc_Port_leq(te,v);}}}_T15=dles;
# 1609
dles=_T15->tl;goto _TL197;_TL196: _T16=
# 1615
Cyc_Port_array_ct(v,q,z);return _T16;}case 0: _T17=t;_T18=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T17;_T19=_T18->f1;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=24)goto _TL19A;_T1C=t;_T1D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1C;_T1E=_T1D->f1;_T1F=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T1E;_T20=_T1F->f1;_T21=_T20.UnknownAggr;_T22=_T21.tag;if(_T22!=1)goto _TL19C;_T23=t;_T24=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T23;_T25=_T24->f1;_T26=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T25;_T27=_T26->f1;_T28=_T27.UnknownAggr;_T29=_T28.val;_T2A=_T29.f0;if(_T2A!=Cyc_Absyn_StructA)goto _TL19E;_T2B=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2B;_T2C=_T6A->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T6B=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T2C;_T2D=_T6B->f1;_T2E=_T2D.UnknownAggr;_T2F=_T2E.val;_T66=_T2F.f1;}}{struct _tuple0*n=_T66;_T30=n;
# 1617
(*_T30).f0=Cyc_Absyn_Loc_n();_T31=
Cyc_Port_lookup_struct_decl(env,n);{struct _tuple11 _T6A=*_T31;struct Cyc_Absyn_Aggrdecl*_T6B;_T6B=_T6A.f0;{struct Cyc_Absyn_Aggrdecl*ad=_T6B;
if(ad!=0)goto _TL1A0;_T33=Cyc_Warn_impos;{int(*_T6C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T33;_T32=_T6C;}_T34=_tag_fat("struct is not yet defined",sizeof(char),26U);_T35=_tag_fat(0U,sizeof(void*),0);_T32(_T34,_T35);goto _TL1A1;_TL1A0: _TL1A1: _T65=ad;goto _LL15;}}}_TL19E: goto _LL16;_TL19C: _T36=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T36;_T37=_T6A->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T6B=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T37;_T38=_T6B->f1;_T39=_T38.KnownAggr;_T3A=_T39.val;{struct Cyc_Absyn_Aggrdecl*_T6C=*_T3A;_T65=_T6C;}}}_LL15:{struct Cyc_Absyn_Aggrdecl*ad=_T65;_T3B=ad;_T3C=_T3B->impl;_T3D=
# 1622
_check_null(_T3C);{struct Cyc_List_List*fields=_T3D->fields;
_TL1A5: if(dles!=0)goto _TL1A3;else{goto _TL1A4;}
_TL1A3: _T3E=dles;_T3F=_T3E->hd;_T40=(struct _tuple18*)_T3F;{struct _tuple18 _T6A=*_T40;struct Cyc_Absyn_Exp*_T6B;struct Cyc_List_List*_T6C;_T6C=_T6A.f0;_T6B=_T6A.f1;{struct Cyc_List_List*d=_T6C;struct Cyc_Absyn_Exp*e=_T6B;_T41=d;_T42=(unsigned)_T41;
if(!_T42)goto _TL1A6;_T44=Cyc_Warn_impos;{int(*_T6D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T44;_T43=_T6D;}_T45=_tag_fat("designators in initializers",sizeof(char),28U);_T46=_tag_fat(0U,sizeof(void*),0);_T43(_T45,_T46);goto _TL1A7;_TL1A6: _TL1A7: _T47=
_check_null(fields);_T48=_T47->hd;{struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)_T48;_T49=fields;
fields=_T49->tl;_T4A=env;_T4B=f;_T4C=_T4B->type;_T4D=e;{
void*te=Cyc_Port_gen_initializer(_T4A,_T4C,_T4D);;}}}}_T4E=dles;
# 1623
dles=_T4E->tl;goto _TL1A5;_TL1A4: _T4F=
# 1630
Cyc_Port_type_to_ctype(env,t);return _T4F;}}_TL19A: goto _LL16;default: _LL16: _T51=Cyc_Warn_impos;{
int(*_T6A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T51;_T50=_T6A;}_T52=_tag_fat("bad type for aggregate initializer",sizeof(char),35U);_T53=_tag_fat(0U,sizeof(void*),0);_T50(_T52,_T53);};}}case 0: _T54=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T63;_T55=_T54->f1;_T56=_T55.Wstring_c;_T57=_T56.tag;switch(_T57){case 8: goto _LLA;case 9: _LLA:
# 1635
 LOOP2: {struct _tuple0*_T65;_T58=t;_T59=(int*)_T58;_T5A=*_T59;switch(_T5A){case 8: _T5B=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T66=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T5B;_T65=_T66->f1;}{struct _tuple0*n=_T65;_T5C=n;
# 1638
(*_T5C).f0=Cyc_Absyn_Loc_n();
t=Cyc_Port_lookup_typedef(env,n);goto LOOP2;}case 5: _T5D=
Cyc_Port_arith_ct();_T5E=Cyc_Port_const_ct();_T5F=Cyc_Port_zterm_ct();_T60=Cyc_Port_array_ct(_T5D,_T5E,_T5F);return _T60;default: _T61=
Cyc_Port_gen_exp(0,env,e);return _T61;};}default: goto _LLB;};default: _LLB: _T62=
# 1643
Cyc_Port_gen_exp(0,env,e);return _T62;};}}
# 1648
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int fromglobal){struct Cyc_Absyn_Decl*_T0;int*_T1;int _T2;int _T3;struct Cyc_Port_Cenv*_T4;struct Cyc_Absyn_Vardecl*_T5;struct _tuple0*_T6;struct Cyc_Absyn_Vardecl*_T7;unsigned _T8;struct Cyc_Absyn_Vardecl*_T9;void*_TA;struct Cyc_Absyn_Vardecl*_TB;struct Cyc_Absyn_Exp*_TC;struct Cyc_Port_Cenv*_TD;struct Cyc_Port_GlobalCenv*_TE;int _TF;struct Cyc_Port_Cenv*_T10;void*_T11;void*_T12;struct Cyc_Absyn_Vardecl*_T13;struct Cyc_Absyn_Tqual _T14;int _T15;struct Cyc_Absyn_Vardecl*_T16;struct Cyc_Absyn_Tqual _T17;unsigned _T18;void*_T19;struct Cyc_Absyn_Vardecl*_T1A;struct Cyc_Absyn_Tqual _T1B;int _T1C;struct Cyc_Absyn_Vardecl*_T1D;struct _tuple0*_T1E;struct Cyc_Port_Cenv*_T1F;struct Cyc_Absyn_Vardecl*_T20;struct _tuple0*_T21;struct Cyc_Absyn_Vardecl*_T22;void*_T23;void*_T24;void*_T25;struct Cyc_Absyn_Vardecl*_T26;unsigned _T27;void*_T28;struct Cyc_Port_Cenv*_T29;struct Cyc_Absyn_Vardecl*_T2A;void*_T2B;void*_T2C;struct Cyc_Absyn_Vardecl*_T2D;struct Cyc_Absyn_Exp*_T2E;unsigned _T2F;struct Cyc_Absyn_Vardecl*_T30;struct Cyc_Port_Cenv*_T31;struct Cyc_Absyn_Vardecl*_T32;void*_T33;struct Cyc_Absyn_Exp*_T34;struct Cyc_Port_Cenv*_T35;int(*_T36)(struct _fat_ptr,struct _fat_ptr);void*(*_T37)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T38;struct _fat_ptr _T39;_T0=d;{
void*_T3A=_T0->r;struct Cyc_Absyn_Vardecl*_T3B;_T1=(int*)_T3A;_T2=*_T1;if(_T2!=0)goto _TL1AA;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T3C=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T3A;_T3B=_T3C->f1;}{struct Cyc_Absyn_Vardecl*vd=_T3B;_T3=fromglobal;
# 1651
if(_T3)goto _TL1AC;else{goto _TL1AE;}_TL1AE: _T4=env;_T5=vd;_T6=_T5->name;_T7=vd;_T8=_T7->varloc;_T9=vd;_TA=_T9->type;_TB=vd;_TC=_TB->initializer;Cyc_Port_register_localvar_decl(_T4,_T6,_T8,_TA,_TC);goto _TL1AD;_TL1AC: _TL1AD: {
void*t=Cyc_Port_var();
void*q;_TD=env;_TE=_TD->gcenv;_TF=_TE->porting;
if(!_TF)goto _TL1AF;
q=Cyc_Port_var();_T10=env;_T11=q;_T13=vd;_T14=_T13->tq;_T15=_T14.print_const;
# 1657
if(!_T15)goto _TL1B1;_T12=Cyc_Port_const_ct();goto _TL1B2;_TL1B1: _T12=Cyc_Port_notconst_ct();_TL1B2: _T16=vd;_T17=_T16->tq;_T18=_T17.loc;
# 1656
Cyc_Port_register_const_cvar(_T10,_T11,_T12,_T18);goto _TL1B0;
# 1665
_TL1AF: _T1A=vd;_T1B=_T1A->tq;_T1C=_T1B.print_const;if(!_T1C)goto _TL1B3;_T19=Cyc_Port_const_ct();goto _TL1B4;_TL1B3: _T19=Cyc_Port_notconst_ct();_TL1B4: q=_T19;_TL1B0: _T1D=vd;_T1E=_T1D->name;
# 1667
(*_T1E).f0=Cyc_Absyn_Loc_n();_T1F=env;_T20=vd;_T21=_T20->name;_T22=vd;_T23=_T22->type;_T24=q;_T25=t;_T26=vd;_T27=_T26->varloc;
env=Cyc_Port_add_var(_T1F,_T21,_T23,_T24,_T25,_T27);_T28=t;_T29=env;_T2A=vd;_T2B=_T2A->type;_T2C=
Cyc_Port_type_to_ctype(_T29,_T2B);Cyc_Port_unifies(_T28,_T2C);_T2D=vd;_T2E=_T2D->initializer;_T2F=(unsigned)_T2E;
if(!_T2F)goto _TL1B5;_T30=vd;{
struct Cyc_Absyn_Exp*e=_T30->initializer;_T31=env;_T32=vd;_T33=_T32->type;_T34=e;{
void*t2=Cyc_Port_gen_initializer(_T31,_T33,_T34);
Cyc_Port_leq(t2,t);}}goto _TL1B6;_TL1B5: _TL1B6: _T35=env;
# 1675
return _T35;}}_TL1AA: _T37=Cyc_Warn_impos;{
int(*_T3C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T37;_T36=_T3C;}_T38=_tag_fat("non-local decl that isn't a variable",sizeof(char),37U);_T39=_tag_fat(0U,sizeof(void*),0);_T36(_T38,_T39);;}}
# 1680
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){struct _tuple8*_T0;struct _tuple8*_T1;_T0=arg;{
struct _tuple8 _T2=*_T0;void*_T3;struct Cyc_Absyn_Tqual _T4;struct _fat_ptr*_T5;_T5=_T2.f0;_T4=_T2.f1;_T3=_T2.f2;{struct _fat_ptr*x=_T5;struct Cyc_Absyn_Tqual q=_T4;void*t=_T3;{struct _tuple8*_T6=_cycalloc(sizeof(struct _tuple8));
_T6->f0=0;_T6->f1=q;_T6->f2=t;_T1=(struct _tuple8*)_T6;}return _T1;}}}struct _tuple19{struct _fat_ptr*f0;void*f1;void*f2;void*f3;};
# 1687
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;int*_T1;unsigned _T2;struct Cyc_Port_Cenv*_T3;struct Cyc_Port_GlobalCenv*_T4;struct Cyc_Port_Cenv*_T5;struct Cyc_Port_Cenv*_T6;struct Cyc_Port_GlobalCenv*_T7;struct Cyc_Port_Cenv*_T8;struct Cyc_Absyn_Vardecl*_T9;struct _tuple0*_TA;struct Cyc_Port_Cenv*_TB;struct Cyc_Absyn_Vardecl*_TC;struct _tuple0*_TD;int _TE;struct Cyc_Port_Cenv*_TF;struct Cyc_Absyn_Vardecl*_T10;struct _tuple0*_T11;struct _tuple12 _T12;struct _tuple12 _T13;struct Cyc_Port_Cenv*_T14;struct Cyc_Port_GlobalCenv*_T15;int _T16;struct Cyc_Port_Cenv*_T17;struct Cyc_Absyn_Vardecl*_T18;struct _tuple0*_T19;int _T1A;struct Cyc_Port_Cenv*_T1B;void*_T1C;void*_T1D;struct Cyc_Absyn_Vardecl*_T1E;struct Cyc_Absyn_Tqual _T1F;int _T20;struct Cyc_Absyn_Vardecl*_T21;struct Cyc_Absyn_Tqual _T22;unsigned _T23;void*_T24;struct Cyc_Absyn_Vardecl*_T25;struct Cyc_Absyn_Tqual _T26;int _T27;void*_T28;struct Cyc_Port_Cenv*_T29;struct Cyc_Absyn_Vardecl*_T2A;void*_T2B;void*_T2C;struct Cyc_Absyn_Vardecl*_T2D;struct Cyc_Absyn_Exp*_T2E;unsigned _T2F;struct Cyc_Absyn_Vardecl*_T30;struct Cyc_Port_Cenv*_T31;struct Cyc_Absyn_Vardecl*_T32;void*_T33;struct Cyc_Absyn_Exp*_T34;void*_T35;void*_T36;struct Cyc_Port_Cenv*_T37;struct Cyc_Port_Cenv*_T38;struct Cyc_Absyn_Fndecl*_T39;struct _tuple0*_T3A;struct Cyc_Port_Cenv*_T3B;struct Cyc_Absyn_Fndecl*_T3C;struct _tuple0*_T3D;int _T3E;struct Cyc_Port_Cenv*_T3F;struct Cyc_Absyn_Fndecl*_T40;struct _tuple0*_T41;struct Cyc_Absyn_Fndecl*_T42;struct Cyc_Absyn_FnInfo _T43;struct Cyc_Absyn_Fndecl*_T44;struct Cyc_Absyn_FnInfo _T45;struct Cyc_List_List*(*_T46)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*);struct Cyc_List_List*(*_T47)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T48;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T49;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T50;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T51;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T52;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T53;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T54;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T55;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T56;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T57;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T58;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T59;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T5A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T5B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T5C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T5D;struct Cyc_List_List*_T5E;unsigned _T5F;struct Cyc_List_List*_T60;void*_T61;struct _tuple8*_T62;struct Cyc_Port_Cenv*_T63;struct Cyc_Port_GlobalCenv*_T64;int _T65;struct Cyc_Port_Cenv*_T66;void*_T67;void*_T68;struct Cyc_Absyn_Tqual _T69;int _T6A;struct Cyc_Absyn_Tqual _T6B;unsigned _T6C;void*_T6D;struct Cyc_Absyn_Tqual _T6E;int _T6F;struct Cyc_List_List*_T70;struct _tuple19*_T71;struct Cyc_List_List*_T72;struct Cyc_List_List*_T73;struct Cyc_Absyn_Fndecl*_T74;struct _tuple0*_T75;struct Cyc_Port_Cenv*_T76;struct Cyc_Absyn_Fndecl*_T77;struct _tuple0*_T78;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T79;void*_T7A;void*_T7B;void*_T7C;struct Cyc_List_List*_T7D;unsigned _T7E;struct Cyc_List_List*_T7F;void*_T80;struct _tuple19*_T81;struct Cyc_Port_Cenv*_T82;struct _tuple0*_T83;void*_T84;void*_T85;void*_T86;struct Cyc_List_List*_T87;struct Cyc_Port_Cenv*_T88;struct Cyc_Absyn_Fndecl*_T89;struct Cyc_Absyn_Stmt*_T8A;struct Cyc_Dict_Dict(*_T8B)(int(*)(struct _fat_ptr*,struct _fat_ptr*));struct Cyc_Dict_Dict(*_T8C)(int(*)(void*,void*));int(*_T8D)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_Dict_Dict*_T8E;struct Cyc_Dict_Dict*_T8F;void*_T90;struct Cyc_Port_Cenv*_T91;struct Cyc_Dict_Dict _T92;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T93;void*_T94;void*_T95;struct Cyc_Port_Cenv*_T96;struct Cyc_Absyn_Fndecl*_T97;struct _tuple0*_T98;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T99;void*_T9A;void*_T9B;void*_T9C;struct _tuple15*_T9D;unsigned _T9E;struct _tuple15*_T9F;struct _tuple12*_TA0;void*_TA1;void*_TA2;void*_TA3;void*_TA4;struct Cyc_Port_Cenv*_TA5;struct Cyc_Absyn_Typedefdecl*_TA6;void*_TA7;struct Cyc_Absyn_Typedefdecl*_TA8;struct _tuple0*_TA9;struct Cyc_Port_Cenv*_TAA;struct Cyc_Absyn_Typedefdecl*_TAB;struct _tuple0*_TAC;void*_TAD;void*_TAE;struct Cyc_Port_Cenv*_TAF;struct Cyc_Absyn_Aggrdecl*_TB0;struct Cyc_Absyn_Aggrdecl*_TB1;struct _tuple0*_TB2;struct Cyc_Absyn_Aggrdecl*_TB3;struct Cyc_Absyn_Aggrdecl*_TB4;struct Cyc_Absyn_AggrdeclImpl*_TB5;unsigned _TB6;struct _tuple11*_TB7;struct _tuple11 _TB8;struct Cyc_Absyn_Aggrdecl*_TB9;struct Cyc_Absyn_AggrdeclImpl*_TBA;struct Cyc_List_List*_TBB;unsigned _TBC;struct Cyc_List_List*_TBD;void*_TBE;struct Cyc_Port_Cenv*_TBF;struct Cyc_Port_GlobalCenv*_TC0;int _TC1;struct Cyc_Port_Cenv*_TC2;void*_TC3;void*_TC4;struct Cyc_Absyn_Aggrfield*_TC5;struct Cyc_Absyn_Tqual _TC6;int _TC7;struct Cyc_Absyn_Aggrfield*_TC8;struct Cyc_Absyn_Tqual _TC9;unsigned _TCA;void*_TCB;struct Cyc_Absyn_Aggrfield*_TCC;struct Cyc_Absyn_Tqual _TCD;int _TCE;struct Cyc_Port_Cenv*_TCF;struct Cyc_Absyn_Aggrfield*_TD0;void*_TD1;struct Cyc_List_List*_TD2;void*_TD3;struct Cyc_Port_Cfield*_TD4;struct Cyc_List_List*_TD5;struct Cyc_List_List*_TD6;struct Cyc_Port_Cfield*_TD7;struct Cyc_Absyn_Aggrfield*_TD8;struct Cyc_List_List*_TD9;struct _tuple11*_TDA;struct _tuple11 _TDB;struct Cyc_List_List*_TDC;struct _tuple11*_TDD;struct Cyc_Port_Cenv*_TDE;struct Cyc_Absyn_Enumdecl*_TDF;struct _tuple0*_TE0;struct Cyc_Absyn_Enumdecl*_TE1;struct Cyc_Core_Opt*_TE2;unsigned _TE3;struct Cyc_Absyn_Enumdecl*_TE4;struct Cyc_Core_Opt*_TE5;void*_TE6;struct Cyc_List_List*_TE7;unsigned _TE8;struct Cyc_List_List*_TE9;void*_TEA;struct Cyc_Absyn_Enumfield*_TEB;struct _tuple0*_TEC;struct Cyc_Port_Cenv*_TED;struct Cyc_List_List*_TEE;void*_TEF;struct Cyc_Absyn_Enumfield*_TF0;struct _tuple0*_TF1;struct Cyc_Absyn_Enumdecl*_TF2;struct _tuple0*_TF3;struct Cyc_Absyn_Enumdecl*_TF4;void*_TF5;void*_TF6;void*_TF7;struct Cyc_List_List*_TF8;struct Cyc_Port_Cenv*_TF9;struct Cyc_Port_Cenv*_TFA;struct Cyc_Port_GlobalCenv*_TFB;int _TFC;struct Cyc___cycFILE*_TFD;struct _fat_ptr _TFE;struct _fat_ptr _TFF;struct Cyc_Port_Cenv*_T100;_T0=d;{
void*_T101=_T0->r;struct Cyc_Absyn_Enumdecl*_T102;struct Cyc_Absyn_Aggrdecl*_T103;struct Cyc_Absyn_Typedefdecl*_T104;struct Cyc_Absyn_Fndecl*_T105;struct Cyc_Absyn_Vardecl*_T106;_T1=(int*)_T101;_T2=*_T1;switch(_T2){case 13: _T3=env;_T4=_T3->gcenv;
# 1690
_T4->porting=1;_T5=env;
return _T5;case 14: _T6=env;_T7=_T6->gcenv;
# 1693
_T7->porting=0;_T8=env;
return _T8;case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T107=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T101;_T106=_T107->f1;}{struct Cyc_Absyn_Vardecl*vd=_T106;_T9=vd;_TA=_T9->name;
# 1696
(*_TA).f0=Cyc_Absyn_Loc_n();_TB=env;_TC=vd;_TD=_TC->name;_TE=
# 1700
Cyc_Port_declared_var(_TB,_TD);if(!_TE)goto _TL1B8;_TF=env;_T10=vd;_T11=_T10->name;{
struct _tuple14 _T107=Cyc_Port_lookup_var(_TF,_T11);unsigned _T108;struct _tuple12 _T109;_T109=_T107.f0;_T108=_T107.f1;{struct _tuple12 p=_T109;unsigned loc=_T108;void*_T10A;void*_T10B;_T12=p;_T10B=_T12.f0;_T13=p;_T10A=_T13.f1;{void*q=_T10B;void*t=_T10A;
# 1703
void*q2;_T14=env;_T15=_T14->gcenv;_T16=_T15->porting;
if(!_T16)goto _TL1BA;_T17=env;_T18=vd;_T19=_T18->name;_T1A=Cyc_Port_isfn(_T17,_T19);if(_T1A)goto _TL1BA;else{goto _TL1BC;}
_TL1BC: q2=Cyc_Port_var();_T1B=env;_T1C=q2;_T1E=vd;_T1F=_T1E->tq;_T20=_T1F.print_const;
# 1707
if(!_T20)goto _TL1BD;_T1D=Cyc_Port_const_ct();goto _TL1BE;_TL1BD: _T1D=Cyc_Port_notconst_ct();_TL1BE: _T21=vd;_T22=_T21->tq;_T23=_T22.loc;
# 1706
Cyc_Port_register_const_cvar(_T1B,_T1C,_T1D,_T23);goto _TL1BB;
# 1715
_TL1BA: _T25=vd;_T26=_T25->tq;_T27=_T26.print_const;if(!_T27)goto _TL1BF;_T24=Cyc_Port_const_ct();goto _TL1C0;_TL1BF: _T24=Cyc_Port_notconst_ct();_TL1C0: q2=_T24;_TL1BB:
# 1717
 Cyc_Port_unifies(q,q2);_T28=t;_T29=env;_T2A=vd;_T2B=_T2A->type;_T2C=
Cyc_Port_type_to_ctype(_T29,_T2B);Cyc_Port_unifies(_T28,_T2C);_T2D=vd;_T2E=_T2D->initializer;_T2F=(unsigned)_T2E;
if(!_T2F)goto _TL1C1;_T30=vd;{
struct Cyc_Absyn_Exp*e=_T30->initializer;_T31=env;_T32=vd;_T33=_T32->type;_T34=e;_T35=
Cyc_Port_gen_initializer(_T31,_T33,_T34);_T36=t;Cyc_Port_leq(_T35,_T36);}goto _TL1C2;_TL1C1: _TL1C2: _T37=env;
# 1723
return _T37;}}}
# 1725
_TL1B8: _T38=Cyc_Port_gen_localdecl(env,d,1);return _T38;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T107=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T101;_T105=_T107->f1;}{struct Cyc_Absyn_Fndecl*fd=_T105;_T39=fd;_T3A=_T39->name;
# 1731
(*_T3A).f0=Cyc_Absyn_Loc_n();{
struct _tuple15*predeclared=0;_T3B=env;_T3C=fd;_T3D=_T3C->name;_T3E=
# 1734
Cyc_Port_declared_var(_T3B,_T3D);if(!_T3E)goto _TL1C3;_T3F=env;_T40=fd;_T41=_T40->name;
predeclared=Cyc_Port_lookup_full_var(_T3F,_T41);goto _TL1C4;_TL1C3: _TL1C4: _T42=fd;_T43=_T42->i;{
# 1737
void*rettype=_T43.ret_type;_T44=fd;_T45=_T44->i;{
struct Cyc_List_List*args=_T45.args;_T47=Cyc_List_map;{
struct Cyc_List_List*(*_T107)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*))_T47;_T46=_T107;}_T48=args;{struct Cyc_List_List*targs=_T46(Cyc_Port_make_targ,_T48);
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype;fntype=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T49=fntype;_T49->tag=6;_T4A=fntype;
_T4A->f1.tvars=0;_T4B=fntype;_T4B->f1.effect=0;_T4C=fntype;_T4C->f1.ret_tqual=Cyc_Absyn_empty_tqual(0U);_T4D=fntype;_T4D->f1.ret_type=rettype;_T4E=fntype;_T4E->f1.args=targs;_T4F=fntype;_T4F->f1.c_varargs=0;_T50=fntype;_T50->f1.cyc_varargs=0;_T51=fntype;_T51->f1.qual_bnd=0;_T52=fntype;_T52->f1.attributes=0;_T53=fntype;_T53->f1.checks_clause=0;_T54=fntype;_T54->f1.checks_assn=0;_T55=fntype;_T55->f1.requires_clause=0;_T56=fntype;_T56->f1.requires_assn=0;_T57=fntype;_T57->f1.ensures_clause=0;_T58=fntype;_T58->f1.ensures_assn=0;_T59=fntype;_T59->f1.throws_clause=0;_T5A=fntype;_T5A->f1.throws_assn=0;_T5B=fntype;_T5B->f1.return_value=0;_T5C=fntype;_T5C->f1.arg_vardecls=0;_T5D=fntype;_T5D->f1.effconstr=0;{
# 1744
struct Cyc_Port_Cenv*fn_env=Cyc_Port_set_cpos(env,2U);
void*c_rettype=Cyc_Port_type_to_ctype(fn_env,rettype);
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;{
struct Cyc_List_List*xs=args;_TL1C8: _T5E=xs;_T5F=(unsigned)_T5E;if(_T5F)goto _TL1C6;else{goto _TL1C7;}
_TL1C6: _T60=xs;_T61=_T60->hd;_T62=(struct _tuple8*)_T61;{struct _tuple8 _T107=*_T62;void*_T108;struct Cyc_Absyn_Tqual _T109;struct _fat_ptr*_T10A;_T10A=_T107.f0;_T109=_T107.f1;_T108=_T107.f2;{struct _fat_ptr*x=_T10A;struct Cyc_Absyn_Tqual tq=_T109;void*t=_T108;
# 1752
void*ct=Cyc_Port_type_to_ctype(fn_env,t);
void*tqv;_T63=env;_T64=_T63->gcenv;_T65=_T64->porting;
if(!_T65)goto _TL1C9;
tqv=Cyc_Port_var();_T66=env;_T67=tqv;_T69=tq;_T6A=_T69.print_const;
if(!_T6A)goto _TL1CB;_T68=Cyc_Port_const_ct();goto _TL1CC;_TL1CB: _T68=Cyc_Port_notconst_ct();_TL1CC: _T6B=tq;_T6C=_T6B.loc;Cyc_Port_register_const_cvar(_T66,_T67,_T68,_T6C);goto _TL1CA;
# 1764
_TL1C9: _T6E=tq;_T6F=_T6E.print_const;if(!_T6F)goto _TL1CD;_T6D=Cyc_Port_const_ct();goto _TL1CE;_TL1CD: _T6D=Cyc_Port_notconst_ct();_TL1CE: tqv=_T6D;_TL1CA:{struct Cyc_List_List*_T10B=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple19*_T10C=_cycalloc(sizeof(struct _tuple19));
# 1766
_T10C->f0=x;_T10C->f1=t;_T10C->f2=tqv;_T10C->f3=ct;_T71=(struct _tuple19*)_T10C;}_T10B->hd=_T71;_T10B->tl=c_args;_T70=(struct Cyc_List_List*)_T10B;}c_args=_T70;{struct Cyc_List_List*_T10B=_cycalloc(sizeof(struct Cyc_List_List));
_T10B->hd=ct;_T10B->tl=c_arg_types;_T72=(struct Cyc_List_List*)_T10B;}c_arg_types=_T72;}}_T73=xs;
# 1748
xs=_T73->tl;goto _TL1C8;_TL1C7:;}
# 1769
c_args=Cyc_List_imp_rev(c_args);
c_arg_types=Cyc_List_imp_rev(c_arg_types);{
void*ctype=Cyc_Port_fn_ct(c_rettype,c_arg_types);_T74=fd;_T75=_T74->name;
# 1775
(*_T75).f0=Cyc_Absyn_Loc_n();_T76=fn_env;_T77=fd;_T78=_T77->name;_T79=fntype;_T7A=(void*)_T79;_T7B=
Cyc_Port_const_ct();_T7C=ctype;fn_env=Cyc_Port_add_var(_T76,_T78,_T7A,_T7B,_T7C,0U);
Cyc_Port_add_return_type(fn_env,c_rettype);{
struct Cyc_List_List*xs=c_args;_TL1D2: _T7D=xs;_T7E=(unsigned)_T7D;if(_T7E)goto _TL1D0;else{goto _TL1D1;}
_TL1D0: _T7F=xs;_T80=_T7F->hd;_T81=(struct _tuple19*)_T80;{struct _tuple19 _T107=*_T81;void*_T108;void*_T109;void*_T10A;struct _fat_ptr*_T10B;_T10B=_T107.f0;_T10A=_T107.f1;_T109=_T107.f2;_T108=_T107.f3;{struct _fat_ptr*x=_T10B;void*t=_T10A;void*q=_T109;void*ct=_T108;_T82=fn_env;{struct _tuple0*_T10C=_cycalloc(sizeof(struct _tuple0));
_T10C->f0=Cyc_Absyn_Loc_n();_T10C->f1=_check_null(x);_T83=(struct _tuple0*)_T10C;}_T84=t;_T85=q;_T86=ct;fn_env=Cyc_Port_add_var(_T82,_T83,_T84,_T85,_T86,0U);}}_T87=xs;
# 1778
xs=_T87->tl;goto _TL1D2;_TL1D1:;}_T88=fn_env;_T89=fd;_T8A=_T89->body;
# 1782
Cyc_Port_gen_stmt(_T88,_T8A);
# 1787
Cyc_Port_generalize(0,ctype);_T8C=Cyc_Dict_empty;{
# 1794
struct Cyc_Dict_Dict(*_T107)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))_T8C;_T8B=_T107;}_T8D=Cyc_strptrcmp;{struct Cyc_Dict_Dict counts=_T8B(_T8D);_T8E=& counts;_T8F=(struct Cyc_Dict_Dict*)_T8E;_T90=ctype;
Cyc_Port_region_counts(_T8F,_T90);_T91=env;_T92=counts;_T93=fntype;_T94=(void*)_T93;_T95=ctype;
# 1798
Cyc_Port_register_rgns(_T91,_T92,1,_T94,_T95);_T96=fn_env;_T97=fd;_T98=_T97->name;_T99=fntype;_T9A=(void*)_T99;_T9B=
Cyc_Port_const_ct();_T9C=ctype;env=Cyc_Port_add_var(_T96,_T98,_T9A,_T9B,_T9C,0U);_T9D=predeclared;_T9E=(unsigned)_T9D;
if(!_T9E)goto _TL1D3;_T9F=predeclared;{
# 1807
struct _tuple15 _T107=*_T9F;void*_T108;void*_T109;void*_T10A;_T10A=_T107.f0;_TA0=_T107.f1;{struct _tuple12 _T10B=*_TA0;_T109=_T10B.f0;_T108=_T10B.f1;}{void*orig_type=_T10A;void*q2=_T109;void*t2=_T108;_TA1=q2;_TA2=
Cyc_Port_const_ct();Cyc_Port_unifies(_TA1,_TA2);_TA3=t2;_TA4=
Cyc_Port_instantiate(ctype);Cyc_Port_unifies(_TA3,_TA4);
# 1811
Cyc_Port_register_rgns(env,counts,1,orig_type,ctype);}}goto _TL1D4;_TL1D3: _TL1D4: _TA5=env;
# 1813
return _TA5;}}}}}}}}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T107=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T101;_T104=_T107->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T104;_TA6=td;_TA7=_TA6->defn;{
# 1819
void*t=_check_null(_TA7);
void*ct=Cyc_Port_type_to_ctype(env,t);_TA8=td;_TA9=_TA8->name;
(*_TA9).f0=Cyc_Absyn_Loc_n();_TAA=env;_TAB=td;_TAC=_TAB->name;_TAD=t;_TAE=ct;
Cyc_Port_add_typedef(_TAA,_TAC,_TAD,_TAE);_TAF=env;
return _TAF;}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T107=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T101;_T103=_T107->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T103;_TB0=ad;{
# 1829
struct _tuple0*name=_TB0->name;_TB1=ad;_TB2=_TB1->name;
(*_TB2).f0=Cyc_Absyn_Loc_n();{
struct _tuple11*previous;
struct Cyc_List_List*curr=0;_TB3=ad;{
enum Cyc_Absyn_AggrKind _T107=_TB3->kind;if(_T107!=Cyc_Absyn_StructA)goto _TL1D5;
# 1835
previous=Cyc_Port_lookup_struct_decl(env,name);goto _LL20;_TL1D5:
# 1838
 previous=Cyc_Port_lookup_union_decl(env,name);goto _LL20;_LL20:;}_TB4=ad;_TB5=_TB4->impl;_TB6=(unsigned)_TB5;
# 1841
if(!_TB6)goto _TL1D7;_TB7=previous;_TB8=*_TB7;{
struct Cyc_List_List*cf=_TB8.f1;_TB9=ad;_TBA=_TB9->impl;{
struct Cyc_List_List*fields=_TBA->fields;_TL1DC: _TBB=fields;_TBC=(unsigned)_TBB;if(_TBC)goto _TL1DA;else{goto _TL1DB;}
_TL1DA: _TBD=fields;_TBE=_TBD->hd;{struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)_TBE;
void*qv;_TBF=env;_TC0=_TBF->gcenv;_TC1=_TC0->porting;
if(!_TC1)goto _TL1DD;
qv=Cyc_Port_var();_TC2=env;_TC3=qv;_TC5=f;_TC6=_TC5->tq;_TC7=_TC6.print_const;
# 1849
if(!_TC7)goto _TL1DF;_TC4=Cyc_Port_const_ct();goto _TL1E0;_TL1DF: _TC4=Cyc_Port_notconst_ct();_TL1E0: _TC8=f;_TC9=_TC8->tq;_TCA=_TC9.loc;
# 1848
Cyc_Port_register_const_cvar(_TC2,_TC3,_TC4,_TCA);goto _TL1DE;
# 1857
_TL1DD: _TCC=f;_TCD=_TCC->tq;_TCE=_TCD.print_const;if(!_TCE)goto _TL1E1;_TCB=Cyc_Port_const_ct();goto _TL1E2;_TL1E1: _TCB=Cyc_Port_notconst_ct();_TL1E2: qv=_TCB;_TL1DE: _TCF=env;_TD0=f;_TD1=_TD0->type;{
# 1859
void*ct=Cyc_Port_type_to_ctype(_TCF,_TD1);
if(cf==0)goto _TL1E3;_TD2=cf;_TD3=_TD2->hd;_TD4=(struct Cyc_Port_Cfield*)_TD3;{
struct Cyc_Port_Cfield _T107=*_TD4;void*_T108;void*_T109;_T109=_T107.qual;_T108=_T107.type;{void*qv2=_T109;void*ct2=_T108;_TD5=cf;
cf=_TD5->tl;
Cyc_Port_unifies(qv,qv2);
Cyc_Port_unifies(ct,ct2);}}goto _TL1E4;_TL1E3: _TL1E4:{struct Cyc_List_List*_T107=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Port_Cfield*_T108=_cycalloc(sizeof(struct Cyc_Port_Cfield));
# 1866
_T108->qual=qv;_TD8=f;_T108->f=_TD8->name;_T108->type=ct;_TD7=(struct Cyc_Port_Cfield*)_T108;}_T107->hd=_TD7;_T107->tl=curr;_TD6=(struct Cyc_List_List*)_T107;}curr=_TD6;}}_TD9=fields;
# 1843
fields=_TD9->tl;goto _TL1DC;_TL1DB:;}
# 1868
curr=Cyc_List_imp_rev(curr);_TDA=previous;_TDB=*_TDA;_TDC=_TDB.f1;
if(_TDC!=0)goto _TL1E5;_TDD=previous;
(*_TDD).f1=curr;goto _TL1E6;_TL1E5: _TL1E6:;}goto _TL1D8;_TL1D7: _TL1D8: _TDE=env;
# 1873
return _TDE;}}}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T107=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T101;_T102=_T107->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T102;_TDF=ed;_TE0=_TDF->name;
# 1878
(*_TE0).f0=Cyc_Absyn_Loc_n();_TE1=ed;_TE2=_TE1->fields;_TE3=(unsigned)_TE2;
# 1880
if(!_TE3)goto _TL1E7;_TE4=ed;_TE5=_TE4->fields;_TE6=_TE5->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_TE6;_TL1EC: _TE7=fs;_TE8=(unsigned)_TE7;if(_TE8)goto _TL1EA;else{goto _TL1EB;}
_TL1EA: _TE9=fs;_TEA=_TE9->hd;_TEB=(struct Cyc_Absyn_Enumfield*)_TEA;_TEC=_TEB->name;(*_TEC).f0=Cyc_Absyn_Loc_n();_TED=env;_TEE=fs;_TEF=_TEE->hd;_TF0=(struct Cyc_Absyn_Enumfield*)_TEF;_TF1=_TF0->name;_TF2=ed;_TF3=_TF2->name;_TF4=ed;_TF5=
Cyc_Absyn_enum_type(_TF3,_TF4);_TF6=
Cyc_Port_const_ct();_TF7=Cyc_Port_arith_ct();
# 1883
env=Cyc_Port_add_var(_TED,_TF1,_TF5,_TF6,_TF7,0U);_TF8=fs;
# 1881
fs=_TF8->tl;goto _TL1EC;_TL1EB:;}goto _TL1E8;_TL1E7: _TL1E8: _TF9=env;
# 1886
return _TF9;}default: _TFA=env;_TFB=_TFA->gcenv;_TFC=_TFB->porting;
# 1888
if(!_TFC)goto _TL1ED;_TFD=Cyc_stderr;_TFE=
_tag_fat("Warning: Cyclone declaration found in code to be ported -- skipping.",sizeof(char),69U);_TFF=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TFD,_TFE,_TFF);goto _TL1EE;_TL1ED: _TL1EE: _T100=env;
return _T100;};}}
# 1895
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_Port_Cenv*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_Absyn_Decl*_T5;struct Cyc_List_List*_T6;struct Cyc_Port_Cenv*_T7;
_TL1F2: _T0=ds;_T1=(unsigned)_T0;if(_T1)goto _TL1F0;else{goto _TL1F1;}
_TL1F0: _T2=env;_T3=ds;_T4=_T3->hd;_T5=(struct Cyc_Absyn_Decl*)_T4;env=Cyc_Port_gen_topdecl(_T2,_T5);_T6=ds;
# 1896
ds=_T6->tl;goto _TL1F2;_TL1F1: _T7=env;
# 1898
return _T7;}
# 1902
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){struct Cyc_Port_Cenv*_T0;struct Cyc_List_List*_T1;struct Cyc_Port_Cenv*_T2;struct Cyc_Port_GlobalCenv*_T3;struct Cyc_Port_Cenv*_T4;struct Cyc_Port_GlobalCenv*_T5;struct Cyc_Port_Cenv*_T6;struct Cyc_Port_GlobalCenv*_T7;struct Cyc_Port_Cenv*_T8;struct Cyc_Port_GlobalCenv*_T9;struct Cyc_Port_Cenv*_TA;struct Cyc_Port_GlobalCenv*_TB;struct Cyc_List_List*_TC;unsigned _TD;struct Cyc_List_List*_TE;void*_TF;struct _tuple17*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;unsigned _T13;struct Cyc_List_List*_T14;void*_T15;struct _tuple17*_T16;struct Cyc_List_List*_T17;struct Cyc_List_List*_T18;unsigned _T19;struct Cyc_List_List*_T1A;void*_T1B;struct _tuple17*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;unsigned _T1F;struct Cyc_List_List*_T20;void*_T21;struct _tuple17*_T22;int _T23;unsigned _T24;int _T25;int*_T26;unsigned _T27;struct Cyc_List_List*_T28;struct Cyc_Port_Edit*_T29;struct Cyc_List_List*_T2A;struct Cyc_Port_Edit*_T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;unsigned _T2E;struct Cyc_List_List*_T2F;void*_T30;struct _tuple17*_T31;int _T32;unsigned _T33;int _T34;int*_T35;unsigned _T36;struct Cyc_List_List*_T37;struct Cyc_Port_Edit*_T38;struct Cyc_List_List*_T39;struct Cyc_Port_Edit*_T3A;struct Cyc_List_List*_T3B;struct Cyc_List_List*_T3C;unsigned _T3D;struct Cyc_List_List*_T3E;void*_T3F;struct _tuple17*_T40;int _T41;unsigned _T42;int _T43;int*_T44;unsigned _T45;struct Cyc_List_List*_T46;struct Cyc_Port_Edit*_T47;struct Cyc_List_List*_T48;struct Cyc_Port_Edit*_T49;struct Cyc_List_List*_T4A;struct Cyc_List_List*(*_T4B)(int(*)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*);struct Cyc_List_List*(*_T4C)(int(*)(void*,void*),struct Cyc_List_List*);struct Cyc_List_List*_T4D;struct Cyc_List_List*_T4E;unsigned _T4F;struct Cyc_List_List*_T50;void*_T51;struct Cyc_Port_Edit*_T52;unsigned _T53;struct Cyc_List_List*_T54;struct Cyc_List_List*_T55;_T0=
# 1904
Cyc_Port_initial_cenv();_T1=ds;{struct Cyc_Port_Cenv*env=Cyc_Port_gen_topdecls(_T0,_T1);_T2=env;_T3=_T2->gcenv;{
# 1906
struct Cyc_List_List*ptrs=_T3->pointer_edits;_T4=env;_T5=_T4->gcenv;{
struct Cyc_List_List*consts=_T5->qualifier_edits;_T6=env;_T7=_T6->gcenv;{
struct Cyc_List_List*zts=_T7->zeroterm_edits;_T8=env;_T9=_T8->gcenv;{
struct Cyc_List_List*edits=_T9->edits;_TA=env;_TB=_TA->gcenv;{
struct Cyc_List_List*localvars=_TB->vardecl_locs;{
# 1935 "port.cyc"
struct Cyc_List_List*ps=ptrs;_TL1F6: _TC=ps;_TD=(unsigned)_TC;if(_TD)goto _TL1F4;else{goto _TL1F5;}
_TL1F4: _TE=ps;_TF=_TE->hd;_T10=(struct _tuple17*)_TF;{struct _tuple17 _T56=*_T10;unsigned _T57;void*_T58;void*_T59;_T59=_T56.f0;_T58=_T56.f1;_T57=_T56.f2;{void*new_ctype=_T59;void*orig_ctype=_T58;unsigned loc=_T57;
Cyc_Port_unifies(new_ctype,orig_ctype);}}_T11=ps;
# 1935
ps=_T11->tl;goto _TL1F6;_TL1F5:;}{
# 1939
struct Cyc_List_List*cs=consts;_TL1FA: _T12=cs;_T13=(unsigned)_T12;if(_T13)goto _TL1F8;else{goto _TL1F9;}
_TL1F8: _T14=cs;_T15=_T14->hd;_T16=(struct _tuple17*)_T15;{struct _tuple17 _T56=*_T16;unsigned _T57;void*_T58;void*_T59;_T59=_T56.f0;_T58=_T56.f1;_T57=_T56.f2;{void*new_qual=_T59;void*old_qual=_T58;unsigned loc=_T57;
Cyc_Port_unifies(new_qual,old_qual);}}_T17=cs;
# 1939
cs=_T17->tl;goto _TL1FA;_TL1F9:;}{
# 1943
struct Cyc_List_List*zs=zts;_TL1FE: _T18=zs;_T19=(unsigned)_T18;if(_T19)goto _TL1FC;else{goto _TL1FD;}
_TL1FC: _T1A=zs;_T1B=_T1A->hd;_T1C=(struct _tuple17*)_T1B;{struct _tuple17 _T56=*_T1C;unsigned _T57;void*_T58;void*_T59;_T59=_T56.f0;_T58=_T56.f1;_T57=_T56.f2;{void*new_zt=_T59;void*old_zt=_T58;unsigned loc=_T57;
Cyc_Port_unifies(new_zt,old_zt);}}_T1D=zs;
# 1943
zs=_T1D->tl;goto _TL1FE;_TL1FD:;}
# 1949
_TL202: _T1E=ptrs;_T1F=(unsigned)_T1E;if(_T1F)goto _TL200;else{goto _TL201;}
_TL200: _T20=ptrs;_T21=_T20->hd;_T22=(struct _tuple17*)_T21;{struct _tuple17 _T56=*_T22;unsigned _T57;void*_T58;void*_T59;_T59=_T56.f0;_T58=_T56.f1;_T57=_T56.f2;{void*new_ctype=_T59;void*orig_ctype=_T58;unsigned loc=_T57;_T23=
Cyc_Port_unifies(new_ctype,orig_ctype);if(_T23)goto _TL203;else{goto _TL205;}_TL205: _T24=loc;_T25=(int)_T24;if(!_T25)goto _TL203;{
void*_T5A=Cyc_Port_compress_ct(orig_ctype);_T26=(int*)_T5A;_T27=*_T26;switch(_T27){case 2:{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Port_Edit*_T5C=_cycalloc(sizeof(struct Cyc_Port_Edit));
# 1954
_T5C->loc=loc;_T5C->old_string=_tag_fat("?",sizeof(char),2U);_T5C->new_string=_tag_fat("*",sizeof(char),2U);_T29=(struct Cyc_Port_Edit*)_T5C;}_T5B->hd=_T29;_T5B->tl=edits;_T28=(struct Cyc_List_List*)_T5B;}edits=_T28;goto _LLC;case 3:{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Port_Edit*_T5C=_cycalloc(sizeof(struct Cyc_Port_Edit));
# 1957
_T5C->loc=loc;_T5C->old_string=_tag_fat("",sizeof(char),1U);_T5C->new_string=_tag_fat("@fat ",sizeof(char),6U);_T2B=(struct Cyc_Port_Edit*)_T5C;}_T5B->hd=_T2B;_T5B->tl=edits;_T2A=(struct Cyc_List_List*)_T5B;}edits=_T2A;goto _LLC;default: goto _LLC;}_LLC:;}goto _TL204;_TL203: _TL204:;}}_T2C=ptrs;
# 1949
ptrs=_T2C->tl;goto _TL202;_TL201:
# 1964
 _TL20A: _T2D=consts;_T2E=(unsigned)_T2D;if(_T2E)goto _TL208;else{goto _TL209;}
_TL208: _T2F=consts;_T30=_T2F->hd;_T31=(struct _tuple17*)_T30;{struct _tuple17 _T56=*_T31;unsigned _T57;void*_T58;void*_T59;_T59=_T56.f0;_T58=_T56.f1;_T57=_T56.f2;{void*new_qual=_T59;void*old_qual=_T58;unsigned loc=_T57;_T32=
Cyc_Port_unifies(new_qual,old_qual);if(_T32)goto _TL20B;else{goto _TL20D;}_TL20D: _T33=loc;_T34=(int)_T33;if(!_T34)goto _TL20B;{
void*_T5A=Cyc_Port_compress_ct(old_qual);_T35=(int*)_T5A;_T36=*_T35;switch(_T36){case 1:{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Port_Edit*_T5C=_cycalloc(sizeof(struct Cyc_Port_Edit));
# 1969
_T5C->loc=loc;_T5C->old_string=_tag_fat("const ",sizeof(char),7U);_T5C->new_string=_tag_fat("",sizeof(char),1U);_T38=(struct Cyc_Port_Edit*)_T5C;}_T5B->hd=_T38;_T5B->tl=edits;_T37=(struct Cyc_List_List*)_T5B;}edits=_T37;goto _LL16;case 0:{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Port_Edit*_T5C=_cycalloc(sizeof(struct Cyc_Port_Edit));
# 1971
_T5C->loc=loc;_T5C->old_string=_tag_fat("",sizeof(char),1U);_T5C->new_string=_tag_fat("const ",sizeof(char),7U);_T3A=(struct Cyc_Port_Edit*)_T5C;}_T5B->hd=_T3A;_T5B->tl=edits;_T39=(struct Cyc_List_List*)_T5B;}edits=_T39;goto _LL16;default: goto _LL16;}_LL16:;}goto _TL20C;_TL20B: _TL20C:;}}_T3B=consts;
# 1964
consts=_T3B->tl;goto _TL20A;_TL209:
# 1977
 _TL212: _T3C=zts;_T3D=(unsigned)_T3C;if(_T3D)goto _TL210;else{goto _TL211;}
_TL210: _T3E=zts;_T3F=_T3E->hd;_T40=(struct _tuple17*)_T3F;{struct _tuple17 _T56=*_T40;unsigned _T57;void*_T58;void*_T59;_T59=_T56.f0;_T58=_T56.f1;_T57=_T56.f2;{void*new_zt=_T59;void*old_zt=_T58;unsigned loc=_T57;_T41=
Cyc_Port_unifies(new_zt,old_zt);if(_T41)goto _TL213;else{goto _TL215;}_TL215: _T42=loc;_T43=(int)_T42;if(!_T43)goto _TL213;{
void*_T5A=Cyc_Port_compress_ct(old_zt);_T44=(int*)_T5A;_T45=*_T44;switch(_T45){case 8:{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Port_Edit*_T5C=_cycalloc(sizeof(struct Cyc_Port_Edit));
# 1982
_T5C->loc=loc;_T5C->old_string=_tag_fat("@nozeroterm ",sizeof(char),13U);_T5C->new_string=_tag_fat("",sizeof(char),1U);_T47=(struct Cyc_Port_Edit*)_T5C;}_T5B->hd=_T47;_T5B->tl=edits;_T46=(struct Cyc_List_List*)_T5B;}edits=_T46;goto _LL20;case 9:{struct Cyc_List_List*_T5B=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Port_Edit*_T5C=_cycalloc(sizeof(struct Cyc_Port_Edit));
# 1984
_T5C->loc=loc;_T5C->old_string=_tag_fat("@zeroterm ",sizeof(char),11U);_T5C->new_string=_tag_fat("",sizeof(char),1U);_T49=(struct Cyc_Port_Edit*)_T5C;}_T5B->hd=_T49;_T5B->tl=edits;_T48=(struct Cyc_List_List*)_T5B;}edits=_T48;goto _LL20;default: goto _LL20;}_LL20:;}goto _TL214;_TL213: _TL214:;}}_T4A=zts;
# 1977
zts=_T4A->tl;goto _TL212;_TL211: _T4C=Cyc_List_merge_sort;{
# 1991
struct Cyc_List_List*(*_T56)(int(*)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*))_T4C;_T4B=_T56;}_T4D=edits;edits=_T4B(Cyc_Port_cmp_edit,_T4D);
# 1993
_TL217: _T4E=edits;_T4F=(unsigned)_T4E;if(_T4F)goto _TL21A;else{goto _TL219;}_TL21A: _T50=edits;_T51=_T50->hd;_T52=(struct Cyc_Port_Edit*)_T51;_T53=_T52->loc;if(_T53==0U)goto _TL218;else{goto _TL219;}
# 1997
_TL218: _T54=edits;edits=_T54->tl;goto _TL217;_TL219: _T55=edits;
# 1999
return _T55;}}}}}}}
# 2004
void Cyc_Port_port(struct Cyc_List_List*ds){struct Cyc_List_List*(*_T0)(unsigned(*)(struct Cyc_Port_Edit*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;unsigned _T5;struct Cyc_List_List*_T6;void*_T7;struct Cyc_Port_Edit*_T8;struct Cyc_List_List*_T9;void*_TA;struct _fat_ptr*_TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_String_pa_PrintArg_struct _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;
struct Cyc_List_List*edits=Cyc_Port_gen_edits(ds);_T1=Cyc_List_map;{
struct Cyc_List_List*(*_T13)(unsigned(*)(struct Cyc_Port_Edit*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(unsigned(*)(struct Cyc_Port_Edit*),struct Cyc_List_List*))_T1;_T0=_T13;}_T2=edits;{struct Cyc_List_List*locs=_T0(Cyc_Port_get_seg,_T2);
Cyc_Position_use_gcc_style_location=0;_T3=
Cyc_Position_strings_of_segments(locs);{struct Cyc_List_List*slocs=Cyc_List_imp_rev(_T3);
_TL21E: _T4=edits;_T5=(unsigned)_T4;if(_T5)goto _TL21C;else{goto _TL21D;}
_TL21C: _T6=edits;_T7=_T6->hd;_T8=(struct Cyc_Port_Edit*)_T7;{struct Cyc_Port_Edit _T13=*_T8;struct _fat_ptr _T14;struct _fat_ptr _T15;unsigned _T16;_T16=_T13.loc;_T15=_T13.old_string;_T14=_T13.new_string;{unsigned loc=_T16;struct _fat_ptr s1=_T15;struct _fat_ptr s2=_T14;_T9=
_check_null(slocs);_TA=_T9->hd;_TB=(struct _fat_ptr*)_TA;{struct _fat_ptr sloc=*_TB;{struct Cyc_String_pa_PrintArg_struct _T17;_T17.tag=0;
_T17.f1=sloc;_TC=_T17;}{struct Cyc_String_pa_PrintArg_struct _T17=_TC;{struct Cyc_String_pa_PrintArg_struct _T18;_T18.tag=0;_T18.f1=s1;_TD=_T18;}{struct Cyc_String_pa_PrintArg_struct _T18=_TD;{struct Cyc_String_pa_PrintArg_struct _T19;_T19.tag=0;_T19.f1=s2;_TE=_T19;}{struct Cyc_String_pa_PrintArg_struct _T19=_TE;void*_T1A[3];_T1A[0]=& _T17;_T1A[1]=& _T18;_T1A[2]=& _T19;_TF=_tag_fat("%s: insert `%s' for `%s'\n",sizeof(char),26U);_T10=_tag_fat(_T1A,sizeof(void*),3);Cyc_printf(_TF,_T10);}}}}}}_T11=edits;
# 2009
edits=_T11->tl;_T12=slocs;slocs=_T12->tl;goto _TL21E;_TL21D:;}}}

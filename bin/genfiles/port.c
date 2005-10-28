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
# 37 "position.h"
extern struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);
# 45
extern int Cyc_Position_use_gcc_style_location;struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 542
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 890 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 898
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 905
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 911
void*Cyc_Absyn_compress(void*);
# 926
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 931
extern void*Cyc_Absyn_sint_type;
# 940
extern void*Cyc_Absyn_al_qual_type;
# 944
extern void*Cyc_Absyn_false_type;
# 946
extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);
# 974
void*Cyc_Absyn_string_type(void*,void*);
# 977
extern void*Cyc_Absyn_fat_bound_type;
# 995
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1193
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 69 "tcutil.h"
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 81
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*,void*);
# 250
int Cyc_Tcutil_force_type2bool(int,void*);
# 253
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 40 "warn.h"
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
int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){
return(int)e1 - (int)e2;}
# 93
static unsigned Cyc_Port_get_seg(struct Cyc_Port_Edit*e){
return e->loc;}
# 96
int Cyc_Port_cmp_seg_t(unsigned loc1,unsigned loc2){
return(int)(loc1 - loc2);}
# 99
int Cyc_Port_hash_seg_t(unsigned loc){
return(int)loc;}struct Cyc_Port_VarUsage{int address_taken;struct Cyc_Absyn_Exp*init;struct Cyc_List_List*usage_locs;};struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};struct Cyc_Port_Cfield{void*qual;struct _fat_ptr*f;void*type;};struct Cyc_Port_Const_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;};struct _tuple11{struct Cyc_Absyn_Aggrdecl*f0;struct Cyc_List_List*f1;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;struct _tuple11*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Var_ct_Port_Ctype_struct{int tag;struct Cyc_Port_Cvar*f1;};
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
static struct _fat_ptr Cyc_Port_ctype2string(int deep,void*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)t)){case 0:
 return _tag_fat("const",sizeof(char),6U);case 1:
 return _tag_fat("notconst",sizeof(char),9U);case 2:
 return _tag_fat("thin",sizeof(char),5U);case 3:
 return _tag_fat("fat",sizeof(char),4U);case 4:
 return _tag_fat("void",sizeof(char),5U);case 5:
 return _tag_fat("zero",sizeof(char),5U);case 6:
 return _tag_fat("arith",sizeof(char),6U);case 7:
 return _tag_fat("heap",sizeof(char),5U);case 8:
 return _tag_fat("ZT",sizeof(char),3U);case 9:
 return _tag_fat("NZT",sizeof(char),4U);case 10: _Tmp4=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)t)->f1;{struct _fat_ptr*n=_Tmp4;
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)*n;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp6,sizeof(void*),1));});}case 11: _Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f3;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f4;_Tmp0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f5;{void*elt=_Tmp4;void*qual=_Tmp3;void*k=_Tmp2;void*rgn=_Tmp1;void*zt=_Tmp0;
# 184
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_Port_ctype2string(deep,elt);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Port_ctype2string(deep,qual);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_Port_ctype2string(deep,k);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_Port_ctype2string(deep,rgn);_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=(struct _fat_ptr)Cyc_Port_ctype2string(deep,zt);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[5];_TmpA[0]=& _Tmp5,_TmpA[1]=& _Tmp6,_TmpA[2]=& _Tmp7,_TmpA[3]=& _Tmp8,_TmpA[4]=& _Tmp9;Cyc_aprintf(_tag_fat("ptr(%s,%s,%s,%s,%s)",sizeof(char),20U),_tag_fat(_TmpA,sizeof(void*),5));});}case 12: _Tmp4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f3;{void*elt=_Tmp4;void*qual=_Tmp3;void*zt=_Tmp2;
# 188
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_Port_ctype2string(deep,elt);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({
struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Port_ctype2string(deep,qual);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_Port_ctype2string(deep,zt);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_aprintf(_tag_fat("array(%s,%s,%s)",sizeof(char),16U),_tag_fat(_Tmp8,sizeof(void*),3));});}case 13: _Tmp4=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)t)->f1->f0;_Tmp3=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)t)->f1->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;struct Cyc_List_List*cfs=_Tmp3;
# 191
struct _fat_ptr s=(int)ad->kind==0?_tag_fat("struct",sizeof(char),7U): _tag_fat("union",sizeof(char),6U);
if(!deep)
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)s;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("%s %s",sizeof(char),6U),_tag_fat(_Tmp7,sizeof(void*),2));});else{
# 195
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)s;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_qvar2string(ad->name);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_Port_cfields2string(0,cfs);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_aprintf(_tag_fat("%s %s {%s}",sizeof(char),11U),_tag_fat(_Tmp8,sizeof(void*),3));});}}case 14: if(((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)t)->f2!=0){_Tmp4=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)t)->f1;_Tmp3=*((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)t)->f2;{struct Cyc_List_List*cfs=_Tmp4;void*eq=_Tmp3;
return Cyc_Port_ctype2string(deep,eq);}}else{_Tmp4=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)t)->f1;{struct Cyc_List_List*cfs=_Tmp4;
# 199
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_Port_cfields2string(deep,cfs);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("aggr {%s}",sizeof(char),10U),_tag_fat(_Tmp6,sizeof(void*),1));});}}case 15: _Tmp4=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)t)->f1;_Tmp3=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)t)->f2;{void*t=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 201
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_Port_ctypes2string(deep,ts);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Port_ctype2string(deep,t);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("fn(%s)->%s",sizeof(char),11U),_tag_fat(_Tmp7,sizeof(void*),2));});}default: _Tmp4=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)t)->f1;{struct Cyc_Port_Cvar*cv=_Tmp4;
# 203
if((unsigned)cv->eq)
return Cyc_Port_ctype2string(deep,*cv->eq);else{
if(!deep || cv->uppers==0 && cv->lowers==0)
return({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)cv->id;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("var(%d)",sizeof(char),8U),_tag_fat(_Tmp6,sizeof(void*),1));});else{
return({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_Port_ctypes2string(0,cv->lowers);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)cv->id;_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
# 210
struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_Port_ctypes2string(0,cv->uppers);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_aprintf(_tag_fat("var([%s]<=%d<=[%s])",sizeof(char),20U),_tag_fat(_Tmp8,sizeof(void*),3));});}}}};}
# 213
static struct _fat_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=Cyc_Port_ctype2string(deep,t);*_Tmp0=_Tmp1;});return _Tmp0;}
struct Cyc_List_List*Cyc_Port_sep(struct _fat_ptr s,struct Cyc_List_List*xs){
struct _fat_ptr*sptr;sptr=_cycalloc(sizeof(struct _fat_ptr)),*sptr=s;
if(xs==0)return xs;{
struct Cyc_List_List*prev=xs;
struct Cyc_List_List*curr=xs->tl;
for(1;curr!=0;(prev=curr,curr=curr->tl)){
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=sptr,_Tmp1->tl=curr;_Tmp1;});prev->tl=_Tmp0;});}
# 222
return xs;}}
# 224
static struct _fat_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
struct _fat_ptr s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({
struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_Port_ctype2string(deep,f->qual);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)*f->f;_Tmp2;});struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,({struct _fat_ptr _Tmp4=(struct _fat_ptr)Cyc_Port_ctype2string(deep,f->type);_Tmp3.f1=_Tmp4;});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_aprintf(_tag_fat("%s %s: %s",sizeof(char),10U),_tag_fat(_Tmp3,sizeof(void*),3));});
struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;return _Tmp0;}
# 229
static struct _fat_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
return Cyc_strconcat_l(({struct _fat_ptr _Tmp0=_tag_fat(",",sizeof(char),2U);Cyc_Port_sep(_Tmp0,({struct Cyc_List_List*(*_Tmp1)(struct _fat_ptr*(*)(int,void*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(int,void*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp1;})(Cyc_Port_ctype2stringptr,deep,ts));}));}
# 232
static struct _fat_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
return Cyc_strconcat_l(({struct _fat_ptr _Tmp0=_tag_fat(";",sizeof(char),2U);Cyc_Port_sep(_Tmp0,({struct Cyc_List_List*(*_Tmp1)(struct _fat_ptr*(*)(int,struct Cyc_Port_Cfield*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(int,struct Cyc_Port_Cfield*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp1;})(Cyc_Port_cfield2stringptr,deep,fs));}));}
# 238
static void*Cyc_Port_notconst_ct (void){return(void*)& Cyc_Port_Notconst_ct_val;}
static void*Cyc_Port_const_ct (void){return(void*)& Cyc_Port_Const_ct_val;}
static void*Cyc_Port_thin_ct (void){return(void*)& Cyc_Port_Thin_ct_val;}
static void*Cyc_Port_fat_ct (void){return(void*)& Cyc_Port_Fat_ct_val;}
static void*Cyc_Port_void_ct (void){return(void*)& Cyc_Port_Void_ct_val;}
static void*Cyc_Port_zero_ct (void){return(void*)& Cyc_Port_Zero_ct_val;}
static void*Cyc_Port_arith_ct (void){return(void*)& Cyc_Port_Arith_ct_val;}
static void*Cyc_Port_heap_ct (void){return(void*)& Cyc_Port_Heap_ct_val;}
static void*Cyc_Port_zterm_ct (void){return(void*)& Cyc_Port_Zterm_ct_val;}
static void*Cyc_Port_nozterm_ct (void){return(void*)& Cyc_Port_Nozterm_ct_val;}
static void*Cyc_Port_rgnvar_ct(struct _fat_ptr*n){return(void*)({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_RgnVar_ct_Port_Ctype_struct));_Tmp0->tag=10,_Tmp0->f1=n;_Tmp0;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct));_Tmp0->tag=14,_Tmp0->f1=fs,_Tmp0->f2=0;_Tmp0;});}
# 252
static void*Cyc_Port_known_aggr_ct(struct _tuple11*p){
return(void*)({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct));_Tmp0->tag=13,_Tmp0->f1=p;_Tmp0;});}
# 255
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 257
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_Ptr_ct_Port_Ctype_struct));_Tmp0->tag=11,_Tmp0->f1=elt,_Tmp0->f2=qual,_Tmp0->f3=ptr_kind,_Tmp0->f4=r,_Tmp0->f5=zt;_Tmp0;});}
# 259
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_Array_ct_Port_Ctype_struct));_Tmp0->tag=12,_Tmp0->f1=elt,_Tmp0->f2=qual,_Tmp0->f3=zt;_Tmp0;});}
# 262
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_Fn_ct_Port_Ctype_struct));_Tmp0->tag=15,_Tmp0->f1=return_type,_Tmp0->f2=args;_Tmp0;});}
# 265
static void*Cyc_Port_var (void){
static int counter=0;
return(void*)({struct Cyc_Port_Var_ct_Port_Ctype_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_Var_ct_Port_Ctype_struct));_Tmp0->tag=16,({struct Cyc_Port_Cvar*_Tmp1=({struct Cyc_Port_Cvar*_Tmp2=_cycalloc(sizeof(struct Cyc_Port_Cvar));_Tmp2->id=counter ++,_Tmp2->eq=0,_Tmp2->uppers=0,_Tmp2->lowers=0;_Tmp2;});_Tmp0->f1=_Tmp1;});_Tmp0;});}
# 269
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 272
static struct _fat_ptr*Cyc_Port_new_region_var (void){
static int counter=0;
struct _fat_ptr s=({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)counter ++;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("`R%d",sizeof(char),5U),_tag_fat(_Tmp1,sizeof(void*),1));});
struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;return _Tmp0;}
# 280
static int Cyc_Port_unifies(void*,void*);
# 282
static void*Cyc_Port_compress_ct(void*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)t)){case 16: _Tmp2=(void***)&((struct Cyc_Port_Var_ct_Port_Ctype_struct*)t)->f1->eq;_Tmp1=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)t)->f1->uppers;_Tmp0=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)t)->f1->lowers;{void***eqp=_Tmp2;struct Cyc_List_List*us=_Tmp1;struct Cyc_List_List*ls=_Tmp0;
# 285
void**eq=*eqp;
if((unsigned)eq){
# 289
void*r=Cyc_Port_compress_ct(*eq);
if(*eq!=r)({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=r;_Tmp4;});*eqp=_Tmp3;});
return r;}
# 296
for(1;ls!=0;ls=ls->tl){
void*_Tmp3=(void*)ls->hd;switch(*((int*)_Tmp3)){case 0:
 goto _LLB;case 9: _LLB:
 goto _LLD;case 4: _LLD:
 goto _LLF;case 13: _LLF:
 goto _LL11;case 15: _LL11:
# 303
({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=(void*)ls->hd;_Tmp5;});*eqp=_Tmp4;});
return(void*)ls->hd;default:
# 306
 goto _LL7;}_LL7:;}
# 309
for(1;us!=0;us=us->tl){
void*_Tmp3=(void*)us->hd;switch(*((int*)_Tmp3)){case 1:
 goto _LL18;case 8: _LL18:
 goto _LL1A;case 5: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 15: _LL1E:
# 316
({void**_Tmp4=({void**_Tmp5=_cycalloc(sizeof(void*));*_Tmp5=(void*)us->hd;_Tmp5;});*eqp=_Tmp4;});
return(void*)us->hd;default:
# 319
 goto _LL14;}_LL14:;}
# 322
return t;}case 14: _Tmp2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)t)->f2;{void**eq=_Tmp2;
# 325
if((unsigned)eq)return Cyc_Port_compress_ct(*eq);else{
return t;}}default:
# 329
 return t;};}struct _tuple12{void*f0;void*f1;};
# 335
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_Tmp0=Cyc_Port_compress_ct(t);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 5: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 8: _LL10:
 goto _LL12;case 9: _LL12:
 goto _LL14;case 14: _LL14:
 goto _LL16;case 13: _LL16:
 goto _LL18;case 16: _LL18:
 goto _LL1A;case 7: _LL1A:
 return t;case 10: _Tmp5=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_Tmp0)->f1;{struct _fat_ptr*n=_Tmp5;
# 351
if(!({int(*_Tmp6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;_Tmp6;})(*instenv,n))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,void*))Cyc_Dict_insert;_Tmp8;});struct Cyc_Dict_Dict _Tmp8=*instenv;struct _fat_ptr*_Tmp9=n;_Tmp7(_Tmp8,_Tmp9,Cyc_Port_var());});*instenv=_Tmp6;});
return({void*(*_Tmp6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(void*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;_Tmp6;})(*instenv,n);}case 11: _Tmp5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f1;_Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f2;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f3;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f4;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f5;{void*t1=_Tmp5;void*t2=_Tmp4;void*t3=_Tmp3;void*t4=_Tmp2;void*zt=_Tmp1;
# 355
struct _tuple12 _Tmp6=({struct _tuple12 _Tmp7;({void*_Tmp8=Cyc_Port_inst(instenv,t1);_Tmp7.f0=_Tmp8;}),({void*_Tmp8=Cyc_Port_inst(instenv,t4);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{void*nt1=_Tmp8;void*nt4=_Tmp7;
if(nt1==t1 && nt4==t4)return t;
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Port_Ptr_ct_Port_Ctype_struct));_Tmp9->tag=11,_Tmp9->f1=nt1,_Tmp9->f2=t2,_Tmp9->f3=t3,_Tmp9->f4=nt4,_Tmp9->f5=zt;_Tmp9;});}}case 12: _Tmp5=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0)->f1;_Tmp4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0)->f2;_Tmp3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0)->f3;{void*t1=_Tmp5;void*t2=_Tmp4;void*zt=_Tmp3;
# 359
void*nt1=Cyc_Port_inst(instenv,t1);
if(nt1==t1)return t;
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Port_Array_ct_Port_Ctype_struct));_Tmp6->tag=12,_Tmp6->f1=nt1,_Tmp6->f2=t2,_Tmp6->f3=zt;_Tmp6;});}default: _Tmp5=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0)->f2;{void*t1=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
# 363
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Port_Fn_ct_Port_Ctype_struct));_Tmp6->tag=15,({void*_Tmp7=Cyc_Port_inst(instenv,t1);_Tmp6->f1=_Tmp7;}),({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*(*_Tmp8)(void*(*)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Port_inst,instenv,ts);_Tmp6->f2=_Tmp7;});_Tmp6;});}};}
# 367
void*Cyc_Port_instantiate(void*t){
struct Cyc_Dict_Dict*_Tmp0=({struct Cyc_Dict_Dict*_Tmp1=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp2=({struct Cyc_Dict_Dict(*_Tmp3)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;_Tmp3;})(Cyc_strptrcmp);*_Tmp1=_Tmp2;});_Tmp1;});return Cyc_Port_inst(_Tmp0,t);}
# 374
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
int found=0;
{struct Cyc_List_List*xs=ts;for(0;(unsigned)xs;xs=xs->tl){
void*t2=Cyc_Port_compress_ct((void*)xs->hd);
if(t==t2)found=1;}}
# 380
if(!found)return ts;{
struct Cyc_List_List*res=0;
for(1;ts!=0;ts=ts->tl){
if(({void*_Tmp0=t;_Tmp0==Cyc_Port_compress_ct((void*)ts->hd);}))continue;
res=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)ts->hd,_Tmp0->tl=res;_Tmp0;});}
# 386
return res;}}
# 391
void Cyc_Port_generalize(int is_rgn,void*t){
t=Cyc_Port_compress_ct(t);{
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)t)){case 16: _Tmp4=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)t)->f1;{struct Cyc_Port_Cvar*cv=_Tmp4;
# 396
({struct Cyc_List_List*_Tmp5=Cyc_Port_filter_self(t,cv->uppers);cv->uppers=_Tmp5;});
({struct Cyc_List_List*_Tmp5=Cyc_Port_filter_self(t,cv->lowers);cv->lowers=_Tmp5;});
if(is_rgn){
# 401
if(cv->uppers==0 && cv->lowers==0){
({void*_Tmp5=t;Cyc_Port_unifies(_Tmp5,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));});
return;}
# 406
if((unsigned)cv->uppers){
Cyc_Port_unifies(t,(void*)cv->uppers->hd);
Cyc_Port_generalize(1,t);}else{
# 410
Cyc_Port_unifies(t,(void*)cv->lowers->hd);
Cyc_Port_generalize(1,t);}}
# 414
return;}case 0:
 goto _LL6;case 1: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 3: _LLA:
 goto _LLC;case 4: _LLC:
 goto _LLE;case 5: _LLE:
 goto _LL10;case 6: _LL10:
 goto _LL12;case 14: _LL12:
 goto _LL14;case 13: _LL14:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 9: _LL18:
 goto _LL1A;case 8: _LL1A:
 goto _LL1C;case 7: _LL1C:
 return;case 11: _Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f3;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f4;_Tmp0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f5;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;void*t4=_Tmp1;void*t5=_Tmp0;
# 431
Cyc_Port_generalize(0,t1);Cyc_Port_generalize(1,t4);goto _LL0;}case 12: _Tmp4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;
# 433
Cyc_Port_generalize(0,t1);Cyc_Port_generalize(0,t2);goto _LL0;}default: _Tmp4=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)t)->f1;_Tmp3=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)t)->f2;{void*t1=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 435
Cyc_Port_generalize(0,t1);({void(*_Tmp5)(void(*)(int,void*),int,struct Cyc_List_List*)=(void(*)(void(*)(int,void*),int,struct Cyc_List_List*))Cyc_List_iter_c;_Tmp5;})(Cyc_Port_generalize,0,ts);goto _LL0;}}_LL0:;}}
# 441
static int Cyc_Port_occurs(void*v,void*t){
t=Cyc_Port_compress_ct(t);
if(t==v)return 1;{
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)t)){case 11: _Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f3;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f4;_Tmp0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)t)->f5;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;void*t4=_Tmp1;void*t5=_Tmp0;
# 446
return(((Cyc_Port_occurs(v,t1)|| Cyc_Port_occurs(v,t2))|| Cyc_Port_occurs(v,t3))|| Cyc_Port_occurs(v,t4))||
 Cyc_Port_occurs(v,t5);}case 12: _Tmp4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;
# 449
return(Cyc_Port_occurs(v,t1)|| Cyc_Port_occurs(v,t2))|| Cyc_Port_occurs(v,t3);}case 15: _Tmp4=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)t)->f1;_Tmp3=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)t)->f2;{void*t=_Tmp4;struct Cyc_List_List*ts=_Tmp3;
# 451
if(Cyc_Port_occurs(v,t))return 1;
for(1;(unsigned)ts;ts=ts->tl){
if(Cyc_Port_occurs(v,(void*)ts->hd))return 1;}
return 0;}case 13: _Tmp4=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)t)->f1->f1;{struct Cyc_List_List*fs=_Tmp4;
return 0;}case 14: _Tmp4=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)t)->f1;{struct Cyc_List_List*fs=_Tmp4;
# 457
for(1;(unsigned)fs;fs=fs->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)fs->hd)->qual)|| Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)fs->hd)->type))return 1;}
return 0;}default:
 return 0;};}}char Cyc_Port_Unify_ct[9U]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 469
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 471
static int Cyc_Port_leq(void*,void*);
static void Cyc_Port_unify_cts(struct Cyc_List_List*,struct Cyc_List_List*);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*,struct Cyc_List_List*,int);
# 475
static void Cyc_Port_unify_ct(void*t1,void*t2){
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);
if(t1==t2)return;{
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});struct _fat_ptr _Tmp1;struct _fat_ptr _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;if(*((int*)_Tmp0.f0)==16){_TmpC=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;{struct Cyc_Port_Cvar*v1=_TmpC;
# 481
if(!Cyc_Port_occurs(t1,t2)){
# 484
{struct Cyc_List_List*us=Cyc_Port_filter_self(t1,v1->uppers);for(0;(unsigned)us;us=us->tl){
if(!Cyc_Port_leq(t2,(void*)us->hd))_throw(& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*ls=Cyc_Port_filter_self(t1,v1->lowers);for(0;(unsigned)ls;ls=ls->tl){
if(!Cyc_Port_leq((void*)ls->hd,t2))_throw(& Cyc_Port_Unify_ct_val);}}
({void**_TmpD=({void**_TmpE=_cycalloc(sizeof(void*));*_TmpE=t2;_TmpE;});v1->eq=_TmpD;});
return;}else{
_throw(& Cyc_Port_Unify_ct_val);}}}else{if(*((int*)_Tmp0.f1)==16){_TmpC=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;{struct Cyc_Port_Cvar*v1=_TmpC;
Cyc_Port_unify_ct(t2,t1);return;}}else{switch(*((int*)_Tmp0.f0)){case 11: if(*((int*)_Tmp0.f1)==11){_TmpC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_TmpB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f3;_Tmp9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f4;_Tmp8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f5;_Tmp7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f3;_Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f4;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f5;{void*e1=_TmpC;void*q1=_TmpB;void*k1=_TmpA;void*r1=_Tmp9;void*zt1=_Tmp8;void*e2=_Tmp7;void*q2=_Tmp6;void*k2=_Tmp5;void*r2=_Tmp4;void*zt2=_Tmp3;
# 493
Cyc_Port_unify_ct(e1,e2);Cyc_Port_unify_ct(q1,q2);Cyc_Port_unify_ct(k1,k2);Cyc_Port_unify_ct(r1,r2);
Cyc_Port_unify_ct(zt1,zt2);
return;}}else{goto _LL15;}case 10: if(*((int*)_Tmp0.f1)==10){_Tmp2=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_Tmp1=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;{struct _fat_ptr n1=_Tmp2;struct _fat_ptr n2=_Tmp1;
# 497
if(Cyc_strcmp(n1,n2)!=0)_throw(& Cyc_Port_Unify_ct_val);
return;}}else{goto _LL15;}case 12: if(*((int*)_Tmp0.f1)==12){_TmpC=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_TmpB=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f3;_Tmp9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp8=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f1)->f3;{void*e1=_TmpC;void*q1=_TmpB;void*zt1=_TmpA;void*e2=_Tmp9;void*q2=_Tmp8;void*zt2=_Tmp7;
# 500
Cyc_Port_unify_ct(e1,e2);Cyc_Port_unify_ct(q1,q2);Cyc_Port_unify_ct(zt1,zt2);return;}}else{goto _LL15;}case 15: if(*((int*)_Tmp0.f1)==15){_TmpC=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_TmpB=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;{void*t1=_TmpC;struct Cyc_List_List*ts1=_TmpB;void*t2=_TmpA;struct Cyc_List_List*ts2=_Tmp9;
# 502
Cyc_Port_unify_ct(t1,t2);Cyc_Port_unify_cts(ts1,ts2);return;}}else{goto _LL15;}case 13: switch(*((int*)_Tmp0.f1)){case 13: _TmpC=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_TmpB=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;{struct _tuple11*p1=_TmpC;struct _tuple11*p2=_TmpB;
# 504
if(p1==p2)return;else{_throw(& Cyc_Port_Unify_ct_val);}}case 14: _TmpC=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1->f0;_TmpB=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1->f1;_TmpA=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp9=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Aggrdecl*ad=_TmpC;struct Cyc_List_List*fs2=_TmpB;struct Cyc_List_List*fs1=_TmpA;void***eq=_Tmp9;
# 514
Cyc_Port_merge_fields(fs2,fs1,0);
({void**_TmpD=({void**_TmpE=_cycalloc(sizeof(void*));*_TmpE=t1;_TmpE;});*eq=_TmpD;});
return;}default: goto _LL15;}case 14: switch(*((int*)_Tmp0.f1)){case 14: _TmpC=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_TmpB=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_TmpA=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp9=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;{struct Cyc_List_List*fs1=_TmpC;void***eq1=_TmpB;struct Cyc_List_List*fs2=_TmpA;void***eq2=_Tmp9;
# 506
void*t=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(fs1,fs2,1));
({void**_TmpD=({void**_TmpE=({void**_TmpF=_cycalloc(sizeof(void*));*_TmpF=t;_TmpF;});*eq2=_TmpE;});*eq1=_TmpD;});
return;}case 13: _TmpC=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_TmpB=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_TmpA=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1->f0;_Tmp9=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1->f1;{struct Cyc_List_List*fs1=_TmpC;void***eq=(void***)_TmpB;struct Cyc_Absyn_Aggrdecl*ad=_TmpA;struct Cyc_List_List*fs2=_Tmp9;
# 510
Cyc_Port_merge_fields(fs2,fs1,0);
({void**_TmpD=({void**_TmpE=_cycalloc(sizeof(void*));*_TmpE=t2;_TmpE;});*eq=_TmpD;});
return;}default: goto _LL15;}default: _LL15:
# 517
 _throw(& Cyc_Port_Unify_ct_val);}}};}}
# 521
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(1;t1!=0 && t2!=0;(t1=t1->tl,t2=t2->tl)){
Cyc_Port_unify_ct((void*)t1->hd,(void*)t2->hd);}
# 525
if(t1!=0 || t2!=0)
_throw(& Cyc_Port_Unify_ct_val);}
# 531
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){
# 533
struct Cyc_List_List*common=0;
{struct Cyc_List_List*xs2=fs2;for(0;(unsigned)xs2;xs2=xs2->tl){
struct Cyc_Port_Cfield*f2=(struct Cyc_Port_Cfield*)xs2->hd;
int found=0;
{struct Cyc_List_List*xs1=fs1;for(0;(unsigned)xs1;xs1=xs1->tl){
struct Cyc_Port_Cfield*f1=(struct Cyc_Port_Cfield*)xs1->hd;
if(Cyc_strptrcmp(f1->f,f2->f)==0){
common=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=f1,_Tmp0->tl=common;_Tmp0;});
Cyc_Port_unify_ct(f1->qual,f2->qual);
Cyc_Port_unify_ct(f1->type,f2->type);
found=1;
break;}}}
# 547
if(!found){
if(allow_f1_subset_f2)
common=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=f2,_Tmp0->tl=common;_Tmp0;});else{
_throw(& Cyc_Port_Unify_ct_val);}}}}
# 553
{struct Cyc_List_List*xs1=fs1;for(0;(unsigned)xs1;xs1=xs1->tl){
struct Cyc_Port_Cfield*f1=(struct Cyc_Port_Cfield*)xs1->hd;
int found=0;
{struct Cyc_List_List*xs2=fs2;for(0;(unsigned)xs2;xs2=xs2->tl){
struct Cyc_Port_Cfield*f2=(struct Cyc_Port_Cfield*)xs2->hd;
if(Cyc_strptrcmp(f1->f,f2->f))found=1;}}
# 560
if(!found)
common=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=f1,_Tmp0->tl=common;_Tmp0;});}}
# 563
return common;}
# 566
static int Cyc_Port_unifies(void*t1,void*t2){
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
# 573
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Port_Unify_ct_exn_struct*)_Tmp2)->tag==Cyc_Port_Unify_ct)
# 580
return 0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 582
return 1;}struct _tuple13{void*f0;void*f1;void*f2;void*f3;void*f4;};
# 588
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){
# 590
t=Cyc_Port_compress_ct(t);
switch(*((int*)t)){case 1:
# 594
 goto _LL4;case 8: _LL4:
 goto _LL6;case 5: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 3: _LLA:
 goto _LLC;case 12: _LLC:
 goto _LLE;case 13: _LLE:
 goto _LL10;case 15: _LL10:
 goto _LL12;case 7: _LL12:
# 605
*uppers=0;
Cyc_Port_unifies(v,t);
return*uppers;case 4:
# 610
 goto _LL16;case 0: _LL16:
 goto _LL18;case 9: _LL18:
# 613
 return*uppers;default:
 goto _LL0;}_LL0:;
# 617
{struct Cyc_List_List*us=*uppers;for(0;(unsigned)us;us=us->tl){
void*t2=Cyc_Port_compress_ct((void*)us->hd);
# 620
if(t==t2)return*uppers;{
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp0.f0)){case 6: switch(*((int*)_Tmp0.f1)){case 11:
# 625
 goto _LL1F;case 5: _LL1F:
 goto _LL21;case 12: _LL21:
# 628
 return*uppers;default: goto _LL24;}case 11: if(*((int*)_Tmp0.f1)==11){_TmpA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f4;_Tmp6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f5;_Tmp5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f3;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f4;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f5;{void*ta=_TmpA;void*qa=_Tmp9;void*ka=_Tmp8;void*ra=_Tmp7;void*za=_Tmp6;void*tb=_Tmp5;void*qb=_Tmp4;void*kb=_Tmp3;void*rb=_Tmp2;void*zb=_Tmp1;
# 633
struct _tuple13 _TmpB=({struct _tuple13 _TmpC;({void*_TmpD=Cyc_Port_var();_TmpC.f0=_TmpD;}),({void*_TmpD=Cyc_Port_var();_TmpC.f1=_TmpD;}),({void*_TmpD=Cyc_Port_var();_TmpC.f2=_TmpD;}),({void*_TmpD=Cyc_Port_var();_TmpC.f3=_TmpD;}),({void*_TmpD=Cyc_Port_var();_TmpC.f4=_TmpD;});_TmpC;});void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;_Tmp10=_TmpB.f0;_TmpF=_TmpB.f1;_TmpE=_TmpB.f2;_TmpD=_TmpB.f3;_TmpC=_TmpB.f4;{void*tc=_Tmp10;void*qc=_TmpF;void*kc=_TmpE;void*rc=_TmpD;void*zc=_TmpC;
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*p;p=_cycalloc(sizeof(struct Cyc_Port_Ptr_ct_Port_Ctype_struct)),p->tag=11,p->f1=tc,p->f2=qc,p->f3=kc,p->f4=rc,p->f5=zc;
Cyc_Port_leq(tc,ta);Cyc_Port_leq(tc,tb);
Cyc_Port_leq(qc,qa);Cyc_Port_leq(qc,qb);
Cyc_Port_leq(kc,ka);Cyc_Port_leq(kc,qb);
Cyc_Port_leq(rc,ra);Cyc_Port_leq(rc,rb);
Cyc_Port_leq(zc,za);Cyc_Port_leq(zc,zb);
us->hd=(void*)((void*)p);
return*uppers;}}}else{goto _LL24;}default: _LL24:
 goto _LL1B;}_LL1B:;}}}{
# 645
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=t,_Tmp0->tl=*uppers;return _Tmp0;}}
# 650
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){
# 652
t=Cyc_Port_compress_ct(t);
switch(*((int*)t)){case 0:
 goto _LL4;case 8: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 13: _LLC:
 goto _LLE;case 15: _LLE:
 goto _LL10;case 10: _LL10:
# 662
*lowers=0;
Cyc_Port_unifies(v,t);
return*lowers;case 7:
 goto _LL14;case 1: _LL14:
 goto _LL16;case 9: _LL16:
# 668
 return*lowers;default:
# 670
 goto _LL0;}_LL0:;
# 672
{struct Cyc_List_List*ls=*lowers;for(0;(unsigned)ls;ls=ls->tl){
void*t2=Cyc_Port_compress_ct((void*)ls->hd);
if(t==t2)return*lowers;{
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;if(*((int*)_Tmp0.f1)==4)
goto _LL1D;else{switch(*((int*)_Tmp0.f0)){case 5: switch(*((int*)_Tmp0.f1)){case 6: _LL1D:
 goto _LL1F;case 11: _LL1F:
 goto _LL21;default: goto _LL26;}case 11: switch(*((int*)_Tmp0.f1)){case 6: _LL21:
 goto _LL23;case 11: _TmpA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f4;_Tmp6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f5;_Tmp5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f3;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f4;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f5;{void*ta=_TmpA;void*qa=_Tmp9;void*ka=_Tmp8;void*ra=_Tmp7;void*za=_Tmp6;void*tb=_Tmp5;void*qb=_Tmp4;void*kb=_Tmp3;void*rb=_Tmp2;void*zb=_Tmp1;
# 686
struct _tuple13 _TmpB=({struct _tuple13 _TmpC;({void*_TmpD=Cyc_Port_var();_TmpC.f0=_TmpD;}),({void*_TmpD=Cyc_Port_var();_TmpC.f1=_TmpD;}),({void*_TmpD=Cyc_Port_var();_TmpC.f2=_TmpD;}),({void*_TmpD=Cyc_Port_var();_TmpC.f3=_TmpD;}),({void*_TmpD=Cyc_Port_var();_TmpC.f4=_TmpD;});_TmpC;});void*_TmpC;void*_TmpD;void*_TmpE;void*_TmpF;void*_Tmp10;_Tmp10=_TmpB.f0;_TmpF=_TmpB.f1;_TmpE=_TmpB.f2;_TmpD=_TmpB.f3;_TmpC=_TmpB.f4;{void*tc=_Tmp10;void*qc=_TmpF;void*kc=_TmpE;void*rc=_TmpD;void*zc=_TmpC;
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*p;p=_cycalloc(sizeof(struct Cyc_Port_Ptr_ct_Port_Ctype_struct)),p->tag=11,p->f1=tc,p->f2=qc,p->f3=kc,p->f4=rc,p->f5=zc;
Cyc_Port_leq(ta,tc);Cyc_Port_leq(tb,tc);
Cyc_Port_leq(qa,qc);Cyc_Port_leq(qb,qc);
Cyc_Port_leq(ka,kc);Cyc_Port_leq(qb,kc);
Cyc_Port_leq(ra,rc);Cyc_Port_leq(rb,rc);
Cyc_Port_leq(za,zc);Cyc_Port_leq(zb,zc);
ls->hd=(void*)((void*)p);
return*lowers;}}default: goto _LL26;}case 12: if(*((int*)_Tmp0.f1)==6){_LL23:
# 681
 return*lowers;}else{goto _LL26;}default: _LL26:
# 695
 goto _LL19;}}_LL19:;}}}{
# 698
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=t,_Tmp0->tl=*lowers;return _Tmp0;}}
# 705
static int Cyc_Port_leq(void*t1,void*t2){
# 711
if(t1==t2)return 1;
t1=Cyc_Port_compress_ct(t1);
t2=Cyc_Port_compress_ct(t2);{
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;struct _fat_ptr _TmpB;struct _fat_ptr _TmpC;switch(*((int*)_Tmp0.f0)){case 7:
 return 1;case 10: switch(*((int*)_Tmp0.f1)){case 10: _TmpC=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_TmpB=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;{struct _fat_ptr n1=_TmpC;struct _fat_ptr n2=_TmpB;
return Cyc_strcmp(n1,n2)==0;}case 7: _TmpC=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;{struct _fat_ptr n1=_TmpC;
return 0;}case 16: goto _LL2D;default: goto _LL2F;}case 1: switch(*((int*)_Tmp0.f1)){case 0:
 return 1;case 16: goto _LL2D;default: goto _LL2F;}case 0: switch(*((int*)_Tmp0.f1)){case 1:
 return 0;case 16: goto _LL2D;default: goto _LL2F;}case 9: switch(*((int*)_Tmp0.f1)){case 8:
 return 0;case 16: goto _LL2D;default: goto _LL2F;}case 8: switch(*((int*)_Tmp0.f1)){case 9:
 return 1;case 16: goto _LL2D;default: goto _LL2F;}case 16: switch(*((int*)_Tmp0.f1)){case 0:
 return 1;case 4:
 return 1;case 16: _TmpA=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;{struct Cyc_Port_Cvar*v1=_TmpA;struct Cyc_Port_Cvar*v2=_Tmp9;
# 741
({struct Cyc_List_List*_TmpD=Cyc_Port_filter_self(t1,v1->uppers);v1->uppers=_TmpD;});
({struct Cyc_List_List*_TmpD=Cyc_Port_filter_self(t2,v2->lowers);v2->lowers=_TmpD;});
({struct Cyc_List_List*_TmpD=Cyc_Port_insert_upper(t1,t2,& v1->uppers);v1->uppers=_TmpD;});
({struct Cyc_List_List*_TmpD=Cyc_Port_insert_lower(t2,t1,& v2->lowers);v2->lowers=_TmpD;});
return 1;}default: _TmpA=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;{struct Cyc_Port_Cvar*v1=_TmpA;
# 747
({struct Cyc_List_List*_TmpD=Cyc_Port_filter_self(t1,v1->uppers);v1->uppers=_TmpD;});
({struct Cyc_List_List*_TmpD=Cyc_Port_insert_upper(t1,t2,& v1->uppers);v1->uppers=_TmpD;});
return 1;}}case 4:
# 724
 return 0;case 5: switch(*((int*)_Tmp0.f1)){case 6:
 return 1;case 11:
 return 1;case 4:
 return 1;case 16: goto _LL2D;default: goto _LL2F;}case 11: switch(*((int*)_Tmp0.f1)){case 6:
 return 1;case 4:
 return 1;case 11: _TmpA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f4;_Tmp6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f0)->f5;_Tmp5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f3;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f4;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f5;{void*t1=_TmpA;void*q1=_Tmp9;void*k1=_Tmp8;void*r1=_Tmp7;void*z1=_Tmp6;void*t2=_Tmp5;void*q2=_Tmp4;void*k2=_Tmp3;void*r2=_Tmp2;void*z2=_Tmp1;
# 733
return(((Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_unifies(k1,k2))&& Cyc_Port_leq(r1,r2))&&
 Cyc_Port_leq(z1,z2);}case 16: goto _LL2D;default: goto _LL2F;}case 12: switch(*((int*)_Tmp0.f1)){case 6:
# 730
 return 1;case 4:
 return 1;case 12: _TmpA=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f1)->f3;{void*t1=_TmpA;void*q1=_Tmp9;void*z1=_Tmp8;void*t2=_Tmp7;void*q2=_Tmp6;void*z2=_Tmp5;
# 736
return(Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_leq(z1,z2);}case 11: _TmpA=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f0)->f3;_Tmp7=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp6=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f3;_Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f5;{void*t1=_TmpA;void*q1=_Tmp9;void*z1=_Tmp8;void*t2=_Tmp7;void*q2=_Tmp6;void*k2=_Tmp5;void*z2=_Tmp4;
# 738
return((Cyc_Port_leq(t1,t2)&& Cyc_Port_leq(q1,q2))&& Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,k2))&&
 Cyc_Port_leq(z1,z2);}case 16: goto _LL2D;default: goto _LL2F;}default: if(*((int*)_Tmp0.f1)==16){_LL2D: _TmpA=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;{struct Cyc_Port_Cvar*v2=_TmpA;
# 751
({struct Cyc_List_List*_TmpD=Cyc_Port_filter_self(t2,v2->lowers);v2->lowers=_TmpD;});
({struct Cyc_List_List*_TmpD=Cyc_Port_insert_lower(t2,t1,& v2->lowers);v2->lowers=_TmpD;});
return 1;}}else{_LL2F:
 return Cyc_Port_unifies(t1,t2);}};}}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*vardecl_locs;struct Cyc_Hashtable_Table*varusage_tab;struct Cyc_List_List*edits;int porting;};
# 785
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos =0U,Cyc_Port_FnArg_pos =1U,Cyc_Port_FnBody_pos =2U,Cyc_Port_Toplevel_pos =3U};struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 798
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv (void){
struct Cyc_Port_GlobalCenv*g;g=_cycalloc(sizeof(struct Cyc_Port_GlobalCenv)),({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp1;})(Cyc_Absyn_qvar_cmp);g->typedef_dict=_Tmp0;}),({
struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp1;})(Cyc_Absyn_qvar_cmp);g->struct_dict=_Tmp0;}),({
struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp1;})(Cyc_Absyn_qvar_cmp);g->union_dict=_Tmp0;}),g->qualifier_edits=0,g->pointer_edits=0,g->zeroterm_edits=0,g->vardecl_locs=0,({
# 806
struct Cyc_Hashtable_Table*_Tmp0=({struct Cyc_Hashtable_Table*(*_Tmp1)(int,int(*)(unsigned,unsigned),int(*)(unsigned))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(unsigned,unsigned),int(*)(unsigned)))Cyc_Hashtable_create;_Tmp1;})(128,Cyc_Port_cmp_seg_t,Cyc_Port_hash_seg_t);g->varusage_tab=_Tmp0;}),g->edits=0,g->porting=0,({
# 809
void*_Tmp0=Cyc_Port_void_ct();g->return_type=_Tmp0;});{
struct Cyc_Port_Cenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_Cenv));_Tmp0->gcenv=g,_Tmp0->cpos=3U,({
# 812
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _tuple0*,struct _tuple0*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty;_Tmp2;})(Cyc_Absyn_qvar_cmp);_Tmp0->var_dict=_Tmp1;});return _Tmp0;}}
# 818
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){
return(int)env->cpos==1;}
# 821
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return(int)env->cpos==0;}
# 824
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){
return(int)env->cpos==3;}
# 827
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){
return env->gcenv->return_type;}
# 830
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple12 _Tmp2=*({struct _tuple12*(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple12*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp3;})(env->gcenv->typedef_dict,n);void*_Tmp3;_Tmp3=_Tmp2.f0;{void*t=_Tmp3;
void*_Tmp4=t;_npop_handler(0);return _Tmp4;}}
# 832
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent)
# 839
return Cyc_Absyn_sint_type;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 844
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple12 _Tmp2=*({struct _tuple12*(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple12*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp3;})(env->gcenv->typedef_dict,n);void*_Tmp3;_Tmp3=_Tmp2.f1;{void*ct=_Tmp3;
void*_Tmp4=ct;_npop_handler(0);return _Tmp4;}}
# 846
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent)
# 853
return Cyc_Port_var();else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 859
static struct _tuple11*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 861
struct _tuple11**popt=({struct _tuple11**(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple11**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt;_Tmp0;})(env->gcenv->struct_dict,n);
if((unsigned)popt)
return*popt;else{
# 865
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),ad->kind=0U,ad->sc=2U,ad->name=n,ad->tvs=0,ad->impl=0,ad->attributes=0,ad->expected_mem_kind=0;{
# 868
struct _tuple11*p;p=_cycalloc(sizeof(struct _tuple11)),p->f0=ad,p->f1=0;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*))Cyc_Dict_insert;_Tmp1;})(env->gcenv->struct_dict,n,p);env->gcenv->struct_dict=_Tmp0;});
return p;}}}
# 876
static struct _tuple11*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 878
struct _tuple11**popt=({struct _tuple11**(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple11**(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup_opt;_Tmp0;})(env->gcenv->union_dict,n);
if((unsigned)popt)
return*popt;else{
# 882
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),ad->kind=1U,ad->sc=2U,ad->name=n,ad->tvs=0,ad->impl=0,ad->attributes=0,ad->expected_mem_kind=0;{
# 885
struct _tuple11*p;p=_cycalloc(sizeof(struct _tuple11)),p->f0=ad,p->f1=0;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple11*))Cyc_Dict_insert;_Tmp1;})(env->gcenv->union_dict,n,p);env->gcenv->union_dict=_Tmp0;});
return p;}}}struct _tuple14{struct _tuple12 f0;unsigned f1;};struct _tuple15{void*f0;struct _tuple12*f1;unsigned f2;};
# 892
static struct _tuple14 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple15 _Tmp2=*({struct _tuple15*(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp3;})(env->var_dict,x);unsigned _Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct _tuple12*p=_Tmp4;unsigned loc=_Tmp3;
struct _tuple14 _Tmp5=({struct _tuple14 _Tmp6;_Tmp6.f0=*p,_Tmp6.f1=loc;_Tmp6;});_npop_handler(0);return _Tmp5;}}
# 894
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 901
struct _tuple14 _Tmp4;({void*_Tmp5=Cyc_Port_var();_Tmp4.f0.f0=_Tmp5;}),({void*_Tmp5=Cyc_Port_var();_Tmp4.f0.f1=_Tmp5;}),_Tmp4.f1=0U;return _Tmp4;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 904
static struct _tuple15*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return({struct _tuple15*(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp0;})(env->var_dict,x);}
# 908
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return({int(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple0*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_member;_Tmp0;})(env->var_dict,x);}
# 912
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple15 _Tmp2=*({struct _tuple15*(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp3;})(env->var_dict,x);void*_Tmp3;_Tmp3=_Tmp2.f0;{void*t=_Tmp3;
LOOP: {void*_Tmp4;switch(*((int*)t)){case 7: _Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;{struct _tuple0*n=_Tmp4;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 5:  {
int _Tmp5=1;_npop_handler(0);return _Tmp5;}default:  {
int _Tmp5=0;_npop_handler(0);return _Tmp5;}};}}}
# 914
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent)
# 925
return 0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 929
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple15 _Tmp2=*({struct _tuple15*(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple0*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple0*))Cyc_Dict_lookup;_Tmp3;})(env->var_dict,x);void*_Tmp3;_Tmp3=_Tmp2.f0;{void*t=_Tmp3;
LOOP: {void*_Tmp4;switch(*((int*)t)){case 7: _Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;{struct _tuple0*n=_Tmp4;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 4:  {
int _Tmp5=1;_npop_handler(0);return _Tmp5;}default:  {
int _Tmp5=0;_npop_handler(0);return _Tmp5;}};}}}
# 931
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent)
# 942
return 0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 948
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos cpos){
struct Cyc_Port_Cenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_Cenv));_Tmp0->gcenv=env->gcenv,_Tmp0->var_dict=env->var_dict,_Tmp0->cpos=cpos;return _Tmp0;}
# 953
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){
env->gcenv->return_type=t;}
# 958
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*ctype,unsigned loc){
# 960
struct Cyc_Port_Cenv*_Tmp0=_cycalloc(sizeof(struct Cyc_Port_Cenv));_Tmp0->gcenv=env->gcenv,({
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple15*)=({struct Cyc_Dict_Dict(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple15*))Cyc_Dict_insert;_Tmp3;});struct Cyc_Dict_Dict _Tmp3=env->var_dict;struct _tuple0*_Tmp4=x;_Tmp2(_Tmp3,_Tmp4,({struct _tuple15*_Tmp5=_cycalloc(sizeof(struct _tuple15));_Tmp5->f0=t,({struct _tuple12*_Tmp6=({struct _tuple12*_Tmp7=_cycalloc(sizeof(struct _tuple12));_Tmp7->f0=qual,_Tmp7->f1=ctype;_Tmp7;});_Tmp5->f1=_Tmp6;}),_Tmp5->f2=loc;_Tmp5;}));});_Tmp0->var_dict=_Tmp1;}),_Tmp0->cpos=env->cpos;return _Tmp0;}
# 966
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple12*)=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple12*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple0*,struct _tuple12*))Cyc_Dict_insert;_Tmp2;});struct Cyc_Dict_Dict _Tmp2=env->gcenv->typedef_dict;struct _tuple0*_Tmp3=n;_Tmp1(_Tmp2,_Tmp3,({struct _tuple12*_Tmp4=_cycalloc(sizeof(struct _tuple12));
_Tmp4->f0=t,_Tmp4->f1=ct;_Tmp4;}));});
# 967
env->gcenv->typedef_dict=_Tmp0;});}struct _tuple16{struct _tuple0*f0;unsigned f1;void*f2;};
# 970
static void Cyc_Port_register_localvar_decl(struct Cyc_Port_Cenv*env,struct _tuple0*x,unsigned loc,void*type,struct Cyc_Absyn_Exp*init){
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp2=({struct _tuple16*_Tmp3=_cycalloc(sizeof(struct _tuple16));_Tmp3->f0=x,_Tmp3->f1=loc,_Tmp3->f2=type;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=env->gcenv->vardecl_locs;_Tmp1;});env->gcenv->vardecl_locs=_Tmp0;});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage*)=(void(*)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=env->gcenv->varusage_tab;unsigned _Tmp2=loc;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_Port_VarUsage*_Tmp3=_cycalloc(sizeof(struct Cyc_Port_VarUsage));_Tmp3->address_taken=0,_Tmp3->init=init,_Tmp3->usage_locs=0;_Tmp3;}));});}
# 974
static void Cyc_Port_register_localvar_usage(struct Cyc_Port_Cenv*env,unsigned declloc,unsigned useloc,int takeaddress){
struct Cyc_Port_VarUsage*varusage=0;
if(({int(*_Tmp0)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage**)=(int(*)(struct Cyc_Hashtable_Table*,unsigned,struct Cyc_Port_VarUsage**))Cyc_Hashtable_try_lookup;_Tmp0;})(env->gcenv->varusage_tab,declloc,& varusage)){
if(takeaddress)_check_null(varusage)->address_taken=1;{
struct Cyc_List_List*l=_check_null(varusage)->usage_locs;
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)useloc,_Tmp1->tl=l;_Tmp1;});varusage->usage_locs=_Tmp0;});}}}struct _tuple17{void*f0;void*f1;unsigned f2;};
# 985
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned loc){
# 987
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp2=({struct _tuple17*_Tmp3=_cycalloc(sizeof(struct _tuple17));_Tmp3->f0=new_qual,_Tmp3->f1=orig_qual,_Tmp3->f2=loc;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=env->gcenv->qualifier_edits;_Tmp1;});env->gcenv->qualifier_edits=_Tmp0;});}
# 993
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned loc){
# 995
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp2=({struct _tuple17*_Tmp3=_cycalloc(sizeof(struct _tuple17));_Tmp3->f0=new_ptr,_Tmp3->f1=orig_ptr,_Tmp3->f2=loc;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=env->gcenv->pointer_edits;_Tmp1;});env->gcenv->pointer_edits=_Tmp0;});}
# 999
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned loc){
# 1001
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp2=({struct _tuple17*_Tmp3=_cycalloc(sizeof(struct _tuple17));_Tmp3->f0=new_zt,_Tmp3->f1=orig_zt,_Tmp3->f2=loc;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=env->gcenv->zeroterm_edits;_Tmp1;});env->gcenv->zeroterm_edits=_Tmp0;});}
# 1007
static void*Cyc_Port_main_type (void){
struct _tuple8*arg1;
arg1=_cycalloc(sizeof(struct _tuple8)),arg1->f0=0,({struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);arg1->f1=_Tmp0;}),arg1->f2=Cyc_Absyn_sint_type;{
struct _tuple8*arg2;
arg2=_cycalloc(sizeof(struct _tuple8)),arg2->f0=0,({struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);arg2->f1=_Tmp0;}),({
void*_Tmp0=({void*_Tmp1=({void*_Tmp2=Cyc_Absyn_wildtyp(0);Cyc_Absyn_string_type(_Tmp2,Cyc_Absyn_al_qual_type);});void*_Tmp2=Cyc_Absyn_wildtyp(0);void*_Tmp3=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp4=
Cyc_Absyn_empty_tqual(0U);
# 1012
void*_Tmp5=
Cyc_Tcutil_any_bool(0);
# 1012
Cyc_Absyn_fatptr_type(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,Cyc_Absyn_false_type);});arg2->f2=_Tmp0;});
# 1014
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_Tmp0->tag=5,_Tmp0->f1.tvars=0,_Tmp0->f1.effect=0,({
struct Cyc_Absyn_Tqual _Tmp1=Cyc_Absyn_empty_tqual(0U);_Tmp0->f1.ret_tqual=_Tmp1;}),_Tmp0->f1.ret_type=Cyc_Absyn_sint_type,({
# 1017
struct Cyc_List_List*_Tmp1=({struct _tuple8*_Tmp2[2];_Tmp2[0]=arg1,_Tmp2[1]=arg2;Cyc_List_list(_tag_fat(_Tmp2,sizeof(struct _tuple8*),2));});_Tmp0->f1.args=_Tmp1;}),_Tmp0->f1.c_varargs=0,_Tmp0->f1.cyc_varargs=0,_Tmp0->f1.qual_bnd=0,_Tmp0->f1.attributes=0,_Tmp0->f1.requires_clause=0,_Tmp0->f1.requires_assn=0,_Tmp0->f1.ensures_clause=0,_Tmp0->f1.arg_vardecls=0,_Tmp0->f1.ensures_assn=0,_Tmp0->f1.throws_clause=0,_Tmp0->f1.throws_assn=0,_Tmp0->f1.return_value=0,_Tmp0->f1.effconstr=0;_Tmp0;});}}
# 1028
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*,void*);
# 1031
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv (void){
struct Cyc_Port_Cenv*e=Cyc_Port_empty_cenv();
# 1034
e=({struct Cyc_Port_Cenv*_Tmp0=e;struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));_Tmp2->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));*_Tmp4=_tag_fat("main",sizeof(char),5U);_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});void*_Tmp2=Cyc_Port_main_type();void*_Tmp3=Cyc_Port_const_ct();Cyc_Port_add_var(_Tmp0,_Tmp1,_Tmp2,_Tmp3,({
struct Cyc_Port_Cenv*_Tmp4=e;Cyc_Port_type_to_ctype(_Tmp4,Cyc_Port_main_type());}),0U);});
return e;}
# 1042
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_Tmp0=Cyc_Port_compress_ct(t);void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 2: _LL6:
 goto _LL8;case 3: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 5: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 14: _LL10:
 goto _LL12;case 13: _LL12:
 goto _LL14;case 16: _LL14:
 goto _LL16;case 8: _LL16:
 goto _LL18;case 9: _LL18:
 goto _LL1A;case 7: _LL1A:
 return;case 10: _Tmp5=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_Tmp0)->f1;{struct _fat_ptr*n=_Tmp5;
# 1058
if(!({int(*_Tmp6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_member;_Tmp6;})(*cts,n))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _fat_ptr*,int*)=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _fat_ptr*,int*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _fat_ptr*,int*))Cyc_Dict_insert;_Tmp8;});struct Cyc_Dict_Dict _Tmp8=*cts;struct _fat_ptr*_Tmp9=n;_Tmp7(_Tmp8,_Tmp9,({int*_TmpA=_cycalloc_atomic(sizeof(int));*_TmpA=0;_TmpA;}));});*cts=_Tmp6;});{
int*p=({int*(*_Tmp6)(struct Cyc_Dict_Dict,struct _fat_ptr*)=(int*(*)(struct Cyc_Dict_Dict,struct _fat_ptr*))Cyc_Dict_lookup;_Tmp6;})(*cts,n);
*p=*p + 1;
return;}}case 11: _Tmp5=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f1;_Tmp4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f2;_Tmp3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f3;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f4;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0)->f5;{void*t1=_Tmp5;void*t2=_Tmp4;void*t3=_Tmp3;void*t4=_Tmp2;void*zt=_Tmp1;
# 1064
Cyc_Port_region_counts(cts,t1);
Cyc_Port_region_counts(cts,t4);
return;}case 12: _Tmp5=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0)->f1;_Tmp4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0)->f2;_Tmp3=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0)->f3;{void*t1=_Tmp5;void*t2=_Tmp4;void*zt=_Tmp3;
# 1068
Cyc_Port_region_counts(cts,t1);
return;}default: _Tmp5=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0)->f2;{void*t1=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
# 1071
Cyc_Port_region_counts(cts,t1);
for(1;(unsigned)ts;ts=ts->tl){Cyc_Port_region_counts(cts,(void*)ts->hd);}
return;}};}
# 1082
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){
# 1084
c=Cyc_Port_compress_ct(c);{
struct _tuple12 _Tmp0=({struct _tuple12 _Tmp1;_Tmp1.f0=t,_Tmp1.f1=c;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0.f0)){case 3: if(*((int*)_Tmp0.f1)==11){_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.eff;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f0)->f1.ptr_atts.ptrloc;_Tmp2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_Tmp0.f1)->f4;{void*et=_Tmp5;void*rt=_Tmp4;struct Cyc_Absyn_PtrLoc*ploc=_Tmp3;void*ec=_Tmp2;void*rc=_Tmp1;
# 1087
Cyc_Port_register_rgns(env,counts,fn_res,et,ec);{
unsigned loc=(unsigned)ploc?ploc->rgn_loc: 0U;
rc=Cyc_Port_compress_ct(rc);
# 1104 "port.cyc"
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));
({struct Cyc_Port_Edit*_Tmp8=({struct Cyc_Port_Edit*_Tmp9=_cycalloc(sizeof(struct Cyc_Port_Edit));_Tmp9->loc=loc,_Tmp9->old_string=_tag_fat("`H ",sizeof(char),4U),_Tmp9->new_string=_tag_fat("",sizeof(char),1U);_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=env->gcenv->edits;_Tmp7;});
# 1104
env->gcenv->edits=_Tmp6;});
# 1107
goto _LL0;}}}else{goto _LL7;}case 4: if(*((int*)_Tmp0.f1)==12){_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0.f0)->f1.elt_type;_Tmp4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;{void*et=_Tmp5;void*ec=_Tmp4;
# 1109
Cyc_Port_register_rgns(env,counts,fn_res,et,ec);
goto _LL0;}}else{goto _LL7;}case 5: if(*((int*)_Tmp0.f1)==15){_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f0)->f1.ret_type;_Tmp4=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0.f0)->f1.args;_Tmp3=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0.f1)->f1;_Tmp2=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_Tmp0.f1)->f2;{void*rt=_Tmp5;struct Cyc_List_List*argst=_Tmp4;void*rc=_Tmp3;struct Cyc_List_List*argsc=_Tmp2;
# 1112
Cyc_Port_register_rgns(env,counts,1,rt,rc);
for(1;argst!=0 && argsc!=0;(argst=argst->tl,argsc=argsc->tl)){
struct _tuple8 _Tmp6=*((struct _tuple8*)argst->hd);void*_Tmp7;_Tmp7=_Tmp6.f2;{void*at=_Tmp7;
void*ac=(void*)argsc->hd;
Cyc_Port_register_rgns(env,counts,0,at,ac);}}
# 1118
goto _LL0;}}else{goto _LL7;}default: _LL7:
 goto _LL0;}_LL0:;}}
# 1125
static void*Cyc_Port_gen_exp(int,struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*);
# 1127
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Decl*,int);
# 1130
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_Tmp0;switch(*((int*)t)){case 7: _Tmp0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;{struct _tuple0*n=_Tmp0;
# 1133
(*n).f0=Cyc_Absyn_Loc_n;{
struct Cyc_Port_Cenv*_Tmp1=env;return Cyc_Port_is_char(_Tmp1,Cyc_Port_lookup_typedef(env,n));}}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==1){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2==Cyc_Absyn_Char_sz)
return 1;else{goto _LL5;}}else{goto _LL5;}default: _LL5:
 return 0;};}
# 1141
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
unsigned _Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;union Cyc_Absyn_AggrInfo _Tmp7;void*_Tmp8;switch(*((int*)t)){case 7: _Tmp8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;{struct _tuple0*n=_Tmp8;
# 1144
(*n).f0=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,n);}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 0:
 return Cyc_Port_void_ct();case 1:
# 1223
 goto _LLA;case 2: _LLA:
 return Cyc_Port_arith_ct();case 24: _Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{union Cyc_Absyn_AggrInfo ai=_Tmp7;
# 1266
void*_Tmp9;if(ai.UnknownAggr.tag==1){if(ai.UnknownAggr.val.f0==Cyc_Absyn_StructA){_Tmp9=ai.UnknownAggr.val.f1;{struct _tuple0*n=_Tmp9;
# 1268
(*n).f0=Cyc_Absyn_Loc_n;{
struct _tuple11*p=Cyc_Port_lookup_struct_decl(env,n);
return Cyc_Port_known_aggr_ct(p);}}}else{_Tmp9=ai.UnknownAggr.val.f1;{struct _tuple0*n=_Tmp9;
# 1272
(*n).f0=Cyc_Absyn_Loc_n;{
struct _tuple11*p=Cyc_Port_lookup_union_decl(env,n);
return Cyc_Port_known_aggr_ct(p);}}}}else{_Tmp9=ai.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp9;
# 1276
struct Cyc_Absyn_Aggrdecl*ad=*adp;
enum Cyc_Absyn_AggrKind _TmpA=ad->kind;if(_TmpA==Cyc_Absyn_StructA){
# 1279
struct _tuple11*p=Cyc_Port_lookup_struct_decl(env,ad->name);
return Cyc_Port_known_aggr_ct(p);}else{
# 1282
struct _tuple11*p=Cyc_Port_lookup_union_decl(env,ad->name);
return Cyc_Port_known_aggr_ct(p);};}};}case 19:
# 1286
 return Cyc_Port_arith_ct();case 20: _Tmp8=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;{struct Cyc_List_List*fs=_Tmp8;
# 1289
for(1;(unsigned)fs;fs=fs->tl){
(*((struct Cyc_Absyn_Enumfield*)fs->hd)->name).f0=Cyc_Absyn_Loc_n;
env=({struct Cyc_Port_Cenv*_Tmp9=env;struct _tuple0*_TmpA=((struct Cyc_Absyn_Enumfield*)fs->hd)->name;void*_TmpB=Cyc_Absyn_sint_type;void*_TmpC=Cyc_Port_const_ct();Cyc_Port_add_var(_Tmp9,_TmpA,_TmpB,_TmpC,Cyc_Port_arith_ct(),0U);});}
# 1293
return Cyc_Port_arith_ct();}default: goto _LL15;}case 3: _Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.eff;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.ptrloc;{void*et=_Tmp8;struct Cyc_Absyn_Tqual tq=_Tmp6;void*r=_Tmp5;void*n=_Tmp4;void*b=_Tmp3;void*zt=_Tmp2;struct Cyc_Absyn_PtrLoc*loc=_Tmp1;
# 1148
unsigned ptr_loc=(unsigned)loc?loc->ptr_loc: 0U;
unsigned rgn_loc=(unsigned)loc?loc->rgn_loc: 0U;
unsigned zt_loc=(unsigned)loc?loc->zt_loc: 0U;
# 1154
void*cet=Cyc_Port_type_to_ctype(env,et);
void*new_rgn;
# 1157
{void*_Tmp9;switch(*((int*)r)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)r)->f1)==6){
# 1159
new_rgn=Cyc_Port_heap_ct();goto _LL17;}else{goto _LL1E;}case 2: _Tmp9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)r)->f1->name;{struct _fat_ptr*name=_Tmp9;
# 1161
new_rgn=Cyc_Port_rgnvar_ct(name);goto _LL17;}case 1:
# 1165
 if(Cyc_Port_in_fn_arg(env))
new_rgn=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());else{
# 1168
if(Cyc_Port_in_fn_res(env)|| Cyc_Port_in_toplevel(env))
new_rgn=Cyc_Port_heap_ct();else{
new_rgn=Cyc_Port_var();}}
goto _LL17;default: _LL1E:
# 1173
 new_rgn=Cyc_Port_heap_ct();goto _LL17;}_LL17:;}{
# 1175
int orig_fat=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,b)==0;
int orig_zt;
{void*_Tmp9=Cyc_Absyn_compress(zt);if(*((int*)_Tmp9)==1){
orig_zt=Cyc_Port_is_char(env,et);goto _LL20;}else{
orig_zt=Cyc_Tcutil_force_type2bool(0,zt);goto _LL20;}_LL20:;}
# 1181
if(env->gcenv->porting){
void*cqv=Cyc_Port_var();
# 1186
({struct Cyc_Port_Cenv*_Tmp9=env;void*_TmpA=cqv;void*_TmpB=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_Tmp9,_TmpA,_TmpB,tq.loc);});
# 1189
if(tq.print_const)({void*_Tmp9=cqv;Cyc_Port_unify_ct(_Tmp9,Cyc_Port_const_ct());});{
# 1195
void*cbv=Cyc_Port_var();
# 1198
({struct Cyc_Port_Cenv*_Tmp9=env;void*_TmpA=cbv;void*_TmpB=orig_fat?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();Cyc_Port_register_ptr_cvars(_Tmp9,_TmpA,_TmpB,ptr_loc);});
# 1200
if(orig_fat)({void*_Tmp9=cbv;Cyc_Port_unify_ct(_Tmp9,Cyc_Port_fat_ct());});{
void*czv=Cyc_Port_var();
# 1204
({struct Cyc_Port_Cenv*_Tmp9=env;void*_TmpA=czv;void*_TmpB=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_Tmp9,_TmpA,_TmpB,zt_loc);});
# 1206
{void*_Tmp9=Cyc_Absyn_compress(zt);if(*((int*)_Tmp9)==1)
goto _LL25;else{
# 1209
({void*_TmpA=czv;Cyc_Port_unify_ct(_TmpA,Cyc_Tcutil_force_type2bool(0,zt)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL25;}_LL25:;}
# 1215
return Cyc_Port_ptr_ct(cet,cqv,cbv,new_rgn,czv);}}}else{
# 1219
void*_Tmp9=cet;void*_TmpA=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();void*_TmpB=
orig_fat?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();
# 1219
void*_TmpC=new_rgn;return Cyc_Port_ptr_ct(_Tmp9,_TmpA,_TmpB,_TmpC,
# 1221
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}}case 4: _Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;_Tmp0=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*et=_Tmp8;struct Cyc_Absyn_Tqual tq=_Tmp6;void*zt=_Tmp5;unsigned ztloc=_Tmp0;
# 1227
void*cet=Cyc_Port_type_to_ctype(env,et);
int orig_zt;
{void*_Tmp9=Cyc_Absyn_compress(zt);if(*((int*)_Tmp9)==1){
orig_zt=0;goto _LL2A;}else{
orig_zt=Cyc_Tcutil_force_type2bool(0,zt);goto _LL2A;}_LL2A:;}
# 1233
if(env->gcenv->porting){
void*cqv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_Tmp9=env;void*_TmpA=cqv;void*_TmpB=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_Tmp9,_TmpA,_TmpB,tq.loc);});{
# 1242
void*czv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_Tmp9=env;void*_TmpA=czv;void*_TmpB=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_Tmp9,_TmpA,_TmpB,ztloc);});
# 1245
{void*_Tmp9=Cyc_Absyn_compress(zt);if(*((int*)_Tmp9)==1)
goto _LL2F;else{
# 1248
({void*_TmpA=czv;Cyc_Port_unify_ct(_TmpA,Cyc_Tcutil_force_type2bool(0,zt)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL2F;}_LL2F:;}
# 1251
return Cyc_Port_array_ct(cet,cqv,czv);}}else{
# 1253
void*_Tmp9=cet;void*_TmpA=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();return Cyc_Port_array_ct(_Tmp9,_TmpA,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());}}case 5: _Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.ret_type;_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)t)->f1.args;{void*rt=_Tmp8;struct Cyc_List_List*args=_Tmp5;
# 1257
void*crt=({struct Cyc_Port_Cenv*_Tmp9=Cyc_Port_set_cpos(env,0U);Cyc_Port_type_to_ctype(_Tmp9,rt);});
struct Cyc_Port_Cenv*env_arg=Cyc_Port_set_cpos(env,1U);
struct Cyc_List_List*cargs=0;
for(1;args!=0;args=args->tl){
struct _tuple8 _Tmp9=*((struct _tuple8*)args->hd);void*_TmpA;struct Cyc_Absyn_Tqual _TmpB;_TmpB=_Tmp9.f1;_TmpA=_Tmp9.f2;{struct Cyc_Absyn_Tqual tq=_TmpB;void*t=_TmpA;
cargs=({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpD=Cyc_Port_type_to_ctype(env_arg,t);_TmpC->hd=_TmpD;}),_TmpC->tl=cargs;_TmpC;});}}{
# 1264
void*_Tmp9=crt;return Cyc_Port_fn_ct(_Tmp9,Cyc_List_imp_rev(cargs));}}default: _LL15:
# 1294
 return Cyc_Port_arith_ct();};}
# 1299
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){
void*arg1=Cyc_Port_compress_ct((void*)_check_null(args)->hd);
struct Cyc_List_List*arg2s=args->tl;
switch((int)p){case Cyc_Absyn_Plus:  {
# 1307
void*arg2=Cyc_Port_compress_ct((void*)_check_null(arg2s)->hd);
if(({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,({void*_Tmp1=Cyc_Port_var();void*_Tmp2=Cyc_Port_var();void*_Tmp3=Cyc_Port_fat_ct();void*_Tmp4=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp1,_Tmp2,_Tmp3,_Tmp4,Cyc_Port_var());}));})){
({void*_Tmp0=arg2;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
return arg1;}else{
if(({void*_Tmp0=arg2;Cyc_Port_leq(_Tmp0,({void*_Tmp1=Cyc_Port_var();void*_Tmp2=Cyc_Port_var();void*_Tmp3=Cyc_Port_fat_ct();void*_Tmp4=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp1,_Tmp2,_Tmp3,_Tmp4,Cyc_Port_var());}));})){
({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
return arg2;}else{
# 1315
({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
({void*_Tmp0=arg2;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}}case Cyc_Absyn_Minus:
# 1324
 if(arg2s==0){
# 1326
({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}else{
# 1329
void*arg2=Cyc_Port_compress_ct((void*)arg2s->hd);
if(({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,({void*_Tmp1=Cyc_Port_var();void*_Tmp2=Cyc_Port_var();void*_Tmp3=Cyc_Port_fat_ct();void*_Tmp4=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp1,_Tmp2,_Tmp3,_Tmp4,Cyc_Port_var());}));})){
if(({void*_Tmp0=arg2;Cyc_Port_leq(_Tmp0,({void*_Tmp1=Cyc_Port_var();void*_Tmp2=Cyc_Port_var();void*_Tmp3=Cyc_Port_fat_ct();void*_Tmp4=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp1,_Tmp2,_Tmp3,_Tmp4,Cyc_Port_var());}));}))
return Cyc_Port_arith_ct();else{
# 1334
({void*_Tmp0=arg2;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
return arg1;}}else{
# 1338
({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}case Cyc_Absyn_Times:
# 1343
 goto _LL8;case Cyc_Absyn_Div: _LL8: goto _LLA;case Cyc_Absyn_Mod: _LLA: goto _LLC;case Cyc_Absyn_Eq: _LLC: goto _LLE;case Cyc_Absyn_Neq: _LLE: goto _LL10;case Cyc_Absyn_Lt: _LL10: goto _LL12;case Cyc_Absyn_Gt: _LL12: goto _LL14;case Cyc_Absyn_Lte: _LL14:
 goto _LL16;case Cyc_Absyn_Gte: _LL16: goto _LL18;case Cyc_Absyn_Bitand: _LL18: goto _LL1A;case Cyc_Absyn_Bitor: _LL1A: goto _LL1C;case Cyc_Absyn_Bitxor: _LL1C: goto _LL1E;case Cyc_Absyn_Bitlshift: _LL1E: goto _LL20;case Cyc_Absyn_Bitlrshift: _LL20:
# 1346
({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
({void*_Tmp0=(void*)_check_null(arg2s)->hd;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case Cyc_Absyn_Not:
 goto _LL24;case Cyc_Absyn_Bitnot: _LL24:
# 1351
({void*_Tmp0=arg1;Cyc_Port_leq(_Tmp0,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();default:
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Port: unknown primop",sizeof(char),21U),_tag_fat(0U,sizeof(void*),0));};}
# 1358
static struct _tuple12 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e,int takeaddress){
void*_Tmp0=e->r;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: _Tmp2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp2;
# 1361
struct _tuple0*x=Cyc_Absyn_binding2qvar(b);
(*x).f0=Cyc_Absyn_Loc_n;{
struct _tuple14 _Tmp3=Cyc_Port_lookup_var(env,x);unsigned _Tmp4;struct _tuple12 _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct _tuple12 p=_Tmp5;unsigned loc=_Tmp4;
Cyc_Port_register_localvar_usage(env,loc,e->loc,takeaddress);
return p;}}}case 23: _Tmp2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1367
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*t1=Cyc_Port_gen_exp(0,env,e1);
({void*_Tmp3=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_Tmp3,Cyc_Port_arith_ct());});{
void*p=({void*_Tmp3=v;void*_Tmp4=q;void*_Tmp5=Cyc_Port_fat_ct();void*_Tmp6=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp3,_Tmp4,_Tmp5,_Tmp6,Cyc_Port_var());});
Cyc_Port_leq(t1,p);{
struct _tuple12 _Tmp3;_Tmp3.f0=q,_Tmp3.f1=v;return _Tmp3;}}}case 20: _Tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp2;
# 1375
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*p=({void*_Tmp3=v;void*_Tmp4=q;void*_Tmp5=Cyc_Port_var();void*_Tmp6=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp3,_Tmp4,_Tmp5,_Tmp6,Cyc_Port_var());});
({void*_Tmp3=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_Tmp3,p);});{
struct _tuple12 _Tmp3;_Tmp3.f0=q,_Tmp3.f1=v;return _Tmp3;}}case 21: _Tmp2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 1381
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*t1=Cyc_Port_gen_exp(takeaddress,env,e1);
({void*_Tmp3=t1;Cyc_Port_leq(_Tmp3,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_Tmp4[1];({struct Cyc_Port_Cfield*_Tmp5=({struct Cyc_Port_Cfield*_Tmp6=_cycalloc(sizeof(struct Cyc_Port_Cfield));_Tmp6->qual=q,_Tmp6->f=f,_Tmp6->type=v;_Tmp6;});_Tmp4[0]=_Tmp5;});Cyc_List_list(_tag_fat(_Tmp4,sizeof(struct Cyc_Port_Cfield*),1));})));});{
struct _tuple12 _Tmp3;_Tmp3.f0=q,_Tmp3.f1=v;return _Tmp3;}}case 22: _Tmp2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct _fat_ptr*f=_Tmp1;
# 1387
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*t1=Cyc_Port_gen_exp(0,env,e1);
({void*_Tmp3=t1;Cyc_Port_leq(_Tmp3,({void*_Tmp4=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_Tmp5[1];({struct Cyc_Port_Cfield*_Tmp6=({struct Cyc_Port_Cfield*_Tmp7=_cycalloc(sizeof(struct Cyc_Port_Cfield));_Tmp7->qual=q,_Tmp7->f=f,_Tmp7->type=v;_Tmp7;});_Tmp5[0]=_Tmp6;});Cyc_List_list(_tag_fat(_Tmp5,sizeof(struct Cyc_Port_Cfield*),1));}));void*_Tmp5=
Cyc_Port_var();
# 1390
void*_Tmp6=
Cyc_Port_var();
# 1390
void*_Tmp7=
Cyc_Port_var();
# 1390
Cyc_Port_ptr_ct(_Tmp4,_Tmp5,_Tmp6,_Tmp7,
Cyc_Port_var());}));});{
struct _tuple12 _Tmp3;_Tmp3.f0=q,_Tmp3.f1=v;return _Tmp3;}}default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=({int(*_Tmp4)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp4;});struct _fat_ptr _Tmp4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_Absynpp_exp2string(e);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("gen_lhs: %s",sizeof(char),12U),_tag_fat(_Tmp6,sizeof(void*),1));});_Tmp3(_Tmp4,_tag_fat(0U,sizeof(void*),0));});};}
# 1398
static void*Cyc_Port_gen_exp_false(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
return Cyc_Port_gen_exp(0,env,e);}
# 1401
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;enum Cyc_Absyn_Incrementor _Tmp1;void*_Tmp2;void*_Tmp3;enum Cyc_Absyn_Primop _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wstring_c.tag){case 2:
 goto _LL4;case 3: _LL4:
 goto _LL6;case 4: _LL6:
 goto _LL8;case 6: _LL8:
 goto _LLA;case 7: _LL14:
# 1412
 return Cyc_Port_arith_ct();case 5: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f1==0)
return Cyc_Port_zero_ct();else{
return Cyc_Port_arith_ct();}case 8:  {
# 1416
void*_Tmp6=Cyc_Port_arith_ct();void*_Tmp7=Cyc_Port_const_ct();void*_Tmp8=Cyc_Port_fat_ct();void*_Tmp9=Cyc_Port_heap_ct();return Cyc_Port_ptr_ct(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Port_zterm_ct());}case 9:  {
# 1418
void*_Tmp6=Cyc_Port_arith_ct();void*_Tmp7=Cyc_Port_const_ct();void*_Tmp8=Cyc_Port_fat_ct();void*_Tmp9=Cyc_Port_heap_ct();return Cyc_Port_ptr_ct(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Port_zterm_ct());}default:
 return Cyc_Port_zero_ct();}case 17: _LLA:
# 1407
 goto _LLC;case 18: _LLC:
 goto _LLE;case 19: _LLE:
 goto _LL10;case 31: _LL10:
 goto _LL12;case 32: _LL12:
 goto _LL14;case 1: _Tmp5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp5;
# 1421
struct _tuple0*x=Cyc_Absyn_binding2qvar(b);
(*x).f0=Cyc_Absyn_Loc_n;{
struct _tuple14 _Tmp6=Cyc_Port_lookup_var(env,x);unsigned _Tmp7;struct _tuple12 _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{struct _tuple12 p=_Tmp8;unsigned loc=_Tmp7;
void*_Tmp9;_Tmp9=p.f1;{void*t=_Tmp9;
if(Cyc_Port_isfn(env,x)){
t=Cyc_Port_instantiate(t);{
void*_TmpA=t;void*_TmpB=Cyc_Port_var();void*_TmpC=Cyc_Port_var();void*_TmpD=Cyc_Port_heap_ct();return Cyc_Port_ptr_ct(_TmpA,_TmpB,_TmpC,_TmpD,Cyc_Port_nozterm_ct());}}else{
# 1429
if(Cyc_Port_isarray(env,x)){
void*elt_type=Cyc_Port_var();
void*qual=Cyc_Port_var();
void*zt=Cyc_Port_var();
void*array_type=Cyc_Port_array_ct(elt_type,qual,zt);
Cyc_Port_unifies(t,array_type);{
void*ptr_type=({void*_TmpA=elt_type;void*_TmpB=qual;void*_TmpC=Cyc_Port_fat_ct();void*_TmpD=Cyc_Port_var();Cyc_Port_ptr_ct(_TmpA,_TmpB,_TmpC,_TmpD,zt);});
return ptr_type;}}else{
# 1438
Cyc_Port_register_localvar_usage(env,loc,e->loc,takeaddress);
return t;}}}}}}case 3: _Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Primop p=_Tmp4;struct Cyc_List_List*es=_Tmp5;
# 1441
struct Cyc_Port_Cenv*_Tmp6=env;enum Cyc_Absyn_Primop _Tmp7=p;return Cyc_Port_gen_primop(_Tmp6,_Tmp7,({struct Cyc_List_List*(*_Tmp8)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Port_gen_exp_false,env,es));}case 4: _Tmp5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Core_Opt*popt=_Tmp3;struct Cyc_Absyn_Exp*e2=_Tmp2;
# 1443
struct _tuple12 _Tmp6=Cyc_Port_gen_lhs(env,e1,0);void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{void*q=_Tmp8;void*t1=_Tmp7;
({void*_Tmp9=q;Cyc_Port_unifies(_Tmp9,Cyc_Port_notconst_ct());});{
void*t2=Cyc_Port_gen_exp(0,env,e2);
if((unsigned)popt){
struct Cyc_List_List x2=({struct Cyc_List_List _Tmp9;_Tmp9.hd=t2,_Tmp9.tl=0;_Tmp9;});
struct Cyc_List_List x1=({struct Cyc_List_List _Tmp9;_Tmp9.hd=t1,_Tmp9.tl=& x2;_Tmp9;});
void*t3=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)popt->v,& x1);
Cyc_Port_leq(t3,t1);
return t1;}else{
# 1453
Cyc_Port_leq(t2,t1);
return t1;}}}}case 5: _Tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;enum Cyc_Absyn_Incrementor inc=_Tmp1;
# 1457
struct _tuple12 _Tmp6=Cyc_Port_gen_lhs(env,e1,0);void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{void*q=_Tmp8;void*t=_Tmp7;
({void*_Tmp9=q;Cyc_Port_unifies(_Tmp9,Cyc_Port_notconst_ct());});
# 1460
({void*_Tmp9=t;Cyc_Port_leq(_Tmp9,({void*_TmpA=Cyc_Port_var();void*_TmpB=Cyc_Port_var();void*_TmpC=Cyc_Port_fat_ct();void*_TmpD=Cyc_Port_var();Cyc_Port_ptr_ct(_TmpA,_TmpB,_TmpC,_TmpD,Cyc_Port_var());}));});
({void*_Tmp9=t;Cyc_Port_leq(_Tmp9,Cyc_Port_arith_ct());});
return t;}}case 6: _Tmp5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 1464
void*v=Cyc_Port_var();
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_Tmp6,Cyc_Port_arith_ct());});
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_Tmp6,v);});
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e3);Cyc_Port_leq(_Tmp6,v);});
return v;}case 7: _Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp5=e1;_Tmp3=e2;goto _LL2C;}case 8: _Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1471
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_Tmp6,Cyc_Port_arith_ct());});
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_Tmp6,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}case 9: _Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1475
Cyc_Port_gen_exp(0,env,e1);
return Cyc_Port_gen_exp(0,env,e2);}case 10: _Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_List_List*es=_Tmp3;
# 1478
void*v=Cyc_Port_var();
void*t1=Cyc_Port_gen_exp(0,env,e1);
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp6)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp6;})(Cyc_Port_gen_exp_false,env,es);
struct Cyc_List_List*vs=Cyc_List_map(Cyc_Port_new_var,ts);
({void*_Tmp6=t1;Cyc_Port_unifies(_Tmp6,({void*_Tmp7=Cyc_Port_fn_ct(v,vs);void*_Tmp8=Cyc_Port_var();void*_Tmp9=Cyc_Port_var();void*_TmpA=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Port_nozterm_ct());}));});
for(1;ts!=0;(ts=ts->tl,vs=vs->tl)){
Cyc_Port_leq((void*)ts->hd,(void*)_check_null(vs)->hd);}
return v;}case 40: _Tmp5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
return Cyc_Port_gen_exp(0,env,e1);}case 12: _Tmp5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
return Cyc_Port_gen_exp(0,env,e1);}case 14: _Tmp5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp3;
# 1489
void*t1=Cyc_Port_type_to_ctype(env,t);
void*t2=Cyc_Port_gen_exp(0,env,e1);
Cyc_Port_leq(t1,t2);
return t2;}case 15: _Tmp5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 1494
struct _tuple12 _Tmp6=Cyc_Port_gen_lhs(env,e1,1);void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{void*q=_Tmp8;void*t=_Tmp7;
void*_Tmp9=t;void*_TmpA=q;void*_TmpB=Cyc_Port_var();void*_TmpC=Cyc_Port_var();return Cyc_Port_ptr_ct(_Tmp9,_TmpA,_TmpB,_TmpC,Cyc_Port_var());}}case 23: _Tmp5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 1497
void*v=Cyc_Port_var();
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_Tmp6,Cyc_Port_arith_ct());});{
void*t=Cyc_Port_gen_exp(0,env,e1);
({void*_Tmp6=t;Cyc_Port_leq(_Tmp6,({void*_Tmp7=v;void*_Tmp8=Cyc_Port_var();void*_Tmp9=Cyc_Port_fat_ct();void*_TmpA=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Port_var());}));});
return v;}}case 20: _Tmp5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 1503
void*v=Cyc_Port_var();
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_Tmp6,({void*_Tmp7=v;void*_Tmp8=Cyc_Port_var();void*_Tmp9=Cyc_Port_var();void*_TmpA=Cyc_Port_var();Cyc_Port_ptr_ct(_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Port_var());}));});
return v;}case 21: _Tmp5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct _fat_ptr*f=_Tmp3;
# 1507
void*v=Cyc_Port_var();
void*t=Cyc_Port_gen_exp(takeaddress,env,e1);
({void*_Tmp6=t;Cyc_Port_leq(_Tmp6,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_Tmp7[1];({struct Cyc_Port_Cfield*_Tmp8=({struct Cyc_Port_Cfield*_Tmp9=_cycalloc(sizeof(struct Cyc_Port_Cfield));({void*_TmpA=Cyc_Port_var();_Tmp9->qual=_TmpA;}),_Tmp9->f=f,_Tmp9->type=v;_Tmp9;});_Tmp7[0]=_Tmp8;});Cyc_List_list(_tag_fat(_Tmp7,sizeof(struct Cyc_Port_Cfield*),1));})));});
return v;}case 22: _Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct _fat_ptr*f=_Tmp3;
# 1512
void*v=Cyc_Port_var();
void*t=Cyc_Port_gen_exp(0,env,e1);
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_Tmp6,({void*_Tmp7=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_Tmp8[1];({struct Cyc_Port_Cfield*_Tmp9=({struct Cyc_Port_Cfield*_TmpA=_cycalloc(sizeof(struct Cyc_Port_Cfield));({void*_TmpB=Cyc_Port_var();_TmpA->qual=_TmpB;}),_TmpA->f=f,_TmpA->type=v;_TmpA;});_Tmp8[0]=_Tmp9;});Cyc_List_list(_tag_fat(_Tmp8,sizeof(struct Cyc_Port_Cfield*),1));}));void*_Tmp8=
Cyc_Port_var();
# 1514
void*_Tmp9=
Cyc_Port_var();
# 1514
void*_TmpA=
Cyc_Port_var();
# 1514
Cyc_Port_ptr_ct(_Tmp7,_Tmp8,_Tmp9,_TmpA,
Cyc_Port_var());}));});
return v;}case 33: _Tmp5=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.elt_type;_Tmp3=((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.num_elts;{void**topt=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp3;
# 1518
({void*_Tmp6=Cyc_Port_gen_exp(0,env,e1);Cyc_Port_leq(_Tmp6,Cyc_Port_arith_ct());});{
void*t=(unsigned)topt?Cyc_Port_type_to_ctype(env,*topt): Cyc_Port_var();
void*_Tmp6=t;void*_Tmp7=Cyc_Port_var();void*_Tmp8=Cyc_Port_fat_ct();void*_Tmp9=Cyc_Port_heap_ct();return Cyc_Port_ptr_ct(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_Port_var());}}case 41:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("@assert",sizeof(char),8U),_tag_fat(0U,sizeof(void*),0));case 42:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("@assert_false",sizeof(char),14U),_tag_fat(0U,sizeof(void*),0));case 11:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("throw",sizeof(char),6U),_tag_fat(0U,sizeof(void*),0));case 13:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("instantiate",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));case 2:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Pragma_e",sizeof(char),9U),_tag_fat(0U,sizeof(void*),0));case 34: _Tmp5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp3;
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Swap_e",sizeof(char),7U),_tag_fat(0U,sizeof(void*),0));}case 16:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("New_e",sizeof(char),6U),_tag_fat(0U,sizeof(void*),0));case 24:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("CompoundLit_e",sizeof(char),14U),_tag_fat(0U,sizeof(void*),0));case 25:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Array_e",sizeof(char),8U),_tag_fat(0U,sizeof(void*),0));case 26:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Comprehension_e",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));case 27:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("ComprehensionNoinit_e",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));case 28:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Aggregate_e",sizeof(char),12U),_tag_fat(0U,sizeof(void*),0));case 29:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("AnonStruct_e",sizeof(char),13U),_tag_fat(0U,sizeof(void*),0));case 30:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Datatype_e",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));case 35:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("UnresolvedMem_e",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));case 36: _Tmp5=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Stmt*s=_Tmp5;
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("StmtExp_e",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));}case 38:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Valueof_e",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));case 39:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Asm_e",sizeof(char),6U),_tag_fat(0U,sizeof(void*),0));default:
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Tagcheck_e",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));};}
# 1544
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
 return;case 1: _Tmp4=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
Cyc_Port_gen_exp(0,env,e);return;}case 2: _Tmp4=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp4;struct Cyc_Absyn_Stmt*s2=_Tmp3;
Cyc_Port_gen_stmt(env,s1);Cyc_Port_gen_stmt(env,s2);return;}case 3: _Tmp4=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp4;
# 1550
void*v=Cyc_Port_lookup_return_type(env);
void*t=(unsigned)eopt?Cyc_Port_gen_exp(0,env,eopt): Cyc_Port_void_ct();
Cyc_Port_leq(t,v);
return;}case 4: _Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 1555
({void*_Tmp5=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_Tmp5,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,s1);
Cyc_Port_gen_stmt(env,s2);
return;}case 5: _Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp3=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_Absyn_Stmt*s=_Tmp3;
# 1560
({void*_Tmp5=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_Tmp5,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,s);
return;}case 6:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 11: _LL12:
 goto _LL14;case 8: _LL14:
 return;case 9: _Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;struct Cyc_Absyn_Stmt*s=_Tmp1;
# 1568
Cyc_Port_gen_exp(0,env,e1);
({void*_Tmp5=Cyc_Port_gen_exp(0,env,e2);Cyc_Port_leq(_Tmp5,Cyc_Port_arith_ct());});
Cyc_Port_gen_exp(0,env,e3);
Cyc_Port_gen_stmt(env,s);
return;}case 10: _Tmp4=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp4;struct Cyc_List_List*scs=_Tmp3;
# 1574
({void*_Tmp5=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_Tmp5,Cyc_Port_arith_ct());});
for(1;(unsigned)scs;scs=scs->tl){
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body);}
return;}case 12: _Tmp4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp4;struct Cyc_Absyn_Stmt*s=_Tmp3;
# 1579
env=Cyc_Port_gen_localdecl(env,d,0);Cyc_Port_gen_stmt(env,s);return;}case 13: _Tmp4=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s=_Tmp4;
Cyc_Port_gen_stmt(env,s);return;}case 14: _Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;{struct Cyc_Absyn_Stmt*s=_Tmp4;struct Cyc_Absyn_Exp*e=_Tmp3;
# 1582
Cyc_Port_gen_stmt(env,s);
({void*_Tmp5=Cyc_Port_gen_exp(0,env,e);Cyc_Port_leq(_Tmp5,Cyc_Port_arith_ct());});
return;}default:
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("try/catch",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));};}struct _tuple18{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 1590
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 35: _Tmp1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LL4;}case 25: _Tmp1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL4: {struct Cyc_List_List*dles=_Tmp1;
_Tmp1=dles;goto _LL6;}case 24: _Tmp1=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL6: {struct Cyc_List_List*dles=_Tmp1;
# 1595
LOOP: {
unsigned _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)t)){case 7: _Tmp4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;{struct _tuple0*n=_Tmp4;
# 1598
(*n).f0=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP;}case 4: _Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp3=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;_Tmp2=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*et=_Tmp4;void*zt=_Tmp3;unsigned ztl=_Tmp2;
# 1601
void*v=Cyc_Port_var();
void*q=Cyc_Port_var();
void*z=Cyc_Port_var();
void*t=Cyc_Port_type_to_ctype(env,et);
for(1;dles!=0;dles=dles->tl){
struct _tuple18 _Tmp5=*((struct _tuple18*)dles->hd);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*d=_Tmp7;struct Cyc_Absyn_Exp*e=_Tmp6;
if((unsigned)d)({int(*_Tmp8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp8;})(_tag_fat("designators in initializers",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));{
void*te=Cyc_Port_gen_initializer(env,et,e);
Cyc_Port_leq(te,v);}}}
# 1611
return Cyc_Port_array_ct(v,q,z);}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==24){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.UnknownAggr.tag==1){if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.UnknownAggr.val.f0==Cyc_Absyn_StructA){_Tmp4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.UnknownAggr.val.f1;{struct _tuple0*n=_Tmp4;
# 1613
(*n).f0=Cyc_Absyn_Loc_n;{
struct _tuple11 _Tmp5=*Cyc_Port_lookup_struct_decl(env,n);void*_Tmp6;_Tmp6=_Tmp5.f0;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp6;
if(ad==0)({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("struct is not yet defined",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
_Tmp4=ad;goto _LL15;}}}}else{goto _LL16;}}else{_Tmp4=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1.KnownAggr.val;_LL15: {struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 1618
struct Cyc_List_List*fields=_check_null(ad->impl)->fields;
for(1;dles!=0;dles=dles->tl){
struct _tuple18 _Tmp5=*((struct _tuple18*)dles->hd);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*d=_Tmp7;struct Cyc_Absyn_Exp*e=_Tmp6;
if((unsigned)d)({int(*_Tmp8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp8;})(_tag_fat("designators in initializers",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)_check_null(fields)->hd;
fields=fields->tl;{
void*te=Cyc_Port_gen_initializer(env,f->type,e);;}}}}
# 1626
return Cyc_Port_type_to_ctype(env,t);}}}else{goto _LL16;}default: _LL16:
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("bad type for aggregate initializer",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));};}}case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Wstring_c.tag){case 8:
# 1629
 goto _LLA;case 9: _LLA:
# 1631
 LOOP2: {
void*_Tmp2;switch(*((int*)t)){case 7: _Tmp2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)t)->f1;{struct _tuple0*n=_Tmp2;
# 1634
(*n).f0=Cyc_Absyn_Loc_n;
t=Cyc_Port_lookup_typedef(env,n);goto LOOP2;}case 4:  {
void*_Tmp3=Cyc_Port_arith_ct();void*_Tmp4=Cyc_Port_const_ct();return Cyc_Port_array_ct(_Tmp3,_Tmp4,Cyc_Port_zterm_ct());}default:
 return Cyc_Port_gen_exp(0,env,e);};}default: goto _LLB;}default: _LLB:
# 1639
 return Cyc_Port_gen_exp(0,env,e);};}
# 1644
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int fromglobal){
void*_Tmp0=d->r;void*_Tmp1;if(*((int*)_Tmp0)==0){_Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1647
if(!fromglobal)Cyc_Port_register_localvar_decl(env,vd->name,vd->varloc,vd->type,vd->initializer);{
void*t=Cyc_Port_var();
void*q;
if(env->gcenv->porting){
q=Cyc_Port_var();
({struct Cyc_Port_Cenv*_Tmp2=env;void*_Tmp3=q;void*_Tmp4=
vd->tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1652
Cyc_Port_register_const_cvar(_Tmp2,_Tmp3,_Tmp4,vd->tq.loc);});}else{
# 1661
q=vd->tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1663
(*vd->name).f0=Cyc_Absyn_Loc_n;
env=Cyc_Port_add_var(env,vd->name,vd->type,q,t,vd->varloc);
({void*_Tmp2=t;Cyc_Port_unifies(_Tmp2,Cyc_Port_type_to_ctype(env,vd->type));});
if((unsigned)vd->initializer){
struct Cyc_Absyn_Exp*e=vd->initializer;
void*t2=Cyc_Port_gen_initializer(env,vd->type,e);
Cyc_Port_leq(t2,t);}
# 1671
return env;}}}else{
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("non-local decl that isn't a variable",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));};}
# 1676
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _Tmp0=*arg;void*_Tmp1;struct Cyc_Absyn_Tqual _Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct _fat_ptr*x=_Tmp3;struct Cyc_Absyn_Tqual q=_Tmp2;void*t=_Tmp1;
struct _tuple8*_Tmp4=_cycalloc(sizeof(struct _tuple8));_Tmp4->f0=0,_Tmp4->f1=q,_Tmp4->f2=t;return _Tmp4;}}struct _tuple19{struct _fat_ptr*f0;void*f1;void*f2;void*f3;};
# 1683
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 13:
# 1686
 env->gcenv->porting=1;
return env;case 14:
# 1689
 env->gcenv->porting=0;
return env;case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 1692
(*vd->name).f0=Cyc_Absyn_Loc_n;
# 1696
if(Cyc_Port_declared_var(env,vd->name)){
struct _tuple14 _Tmp2=Cyc_Port_lookup_var(env,vd->name);unsigned _Tmp3;struct _tuple12 _Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct _tuple12 p=_Tmp4;unsigned loc=_Tmp3;
void*_Tmp5;void*_Tmp6;_Tmp6=p.f0;_Tmp5=p.f1;{void*q=_Tmp6;void*t=_Tmp5;
void*q2;
if(env->gcenv->porting && !Cyc_Port_isfn(env,vd->name)){
q2=Cyc_Port_var();
({struct Cyc_Port_Cenv*_Tmp7=env;void*_Tmp8=q2;void*_Tmp9=
vd->tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1702
Cyc_Port_register_const_cvar(_Tmp7,_Tmp8,_Tmp9,vd->tq.loc);});}else{
# 1711
q2=vd->tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1713
Cyc_Port_unifies(q,q2);
({void*_Tmp7=t;Cyc_Port_unifies(_Tmp7,Cyc_Port_type_to_ctype(env,vd->type));});
if((unsigned)vd->initializer){
struct Cyc_Absyn_Exp*e=vd->initializer;
({void*_Tmp7=Cyc_Port_gen_initializer(env,vd->type,e);Cyc_Port_leq(_Tmp7,t);});}
# 1719
return env;}}}else{
# 1721
return Cyc_Port_gen_localdecl(env,d,1);}}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
# 1727
(*fd->name).f0=Cyc_Absyn_Loc_n;{
struct _tuple15*predeclared=0;
# 1730
if(Cyc_Port_declared_var(env,fd->name))
predeclared=Cyc_Port_lookup_full_var(env,fd->name);{
# 1733
void*rettype=fd->i.ret_type;
struct Cyc_List_List*args=fd->i.args;
struct Cyc_List_List*targs=({struct Cyc_List_List*(*_Tmp2)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple8*(*)(struct _tuple8*),struct Cyc_List_List*))Cyc_List_map;_Tmp2;})(Cyc_Port_make_targ,args);
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype;
fntype=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct)),fntype->tag=5,fntype->f1.tvars=0,fntype->f1.effect=0,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(0U);fntype->f1.ret_tqual=_Tmp2;}),fntype->f1.ret_type=rettype,fntype->f1.args=targs,fntype->f1.c_varargs=0,fntype->f1.cyc_varargs=0,fntype->f1.qual_bnd=0,fntype->f1.attributes=0,fntype->f1.requires_clause=0,fntype->f1.requires_assn=0,fntype->f1.ensures_clause=0,fntype->f1.ensures_assn=0,fntype->f1.throws_clause=0,fntype->f1.throws_assn=0,fntype->f1.return_value=0,fntype->f1.arg_vardecls=0,fntype->f1.effconstr=0;{
# 1740
struct Cyc_Port_Cenv*fn_env=Cyc_Port_set_cpos(env,2U);
void*c_rettype=Cyc_Port_type_to_ctype(fn_env,rettype);
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*xs=args;for(0;(unsigned)xs;xs=xs->tl){
struct _tuple8 _Tmp2=*((struct _tuple8*)xs->hd);void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;void*_Tmp5;_Tmp5=_Tmp2.f0;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct _fat_ptr*x=_Tmp5;struct Cyc_Absyn_Tqual tq=_Tmp4;void*t=_Tmp3;
# 1748
void*ct=Cyc_Port_type_to_ctype(fn_env,t);
void*tqv;
if(env->gcenv->porting){
tqv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_Tmp6=env;void*_Tmp7=tqv;void*_Tmp8=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_Tmp6,_Tmp7,_Tmp8,tq.loc);});}else{
# 1760
tqv=tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}
# 1762
c_args=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp7=({struct _tuple19*_Tmp8=_cycalloc(sizeof(struct _tuple19));_Tmp8->f0=x,_Tmp8->f1=t,_Tmp8->f2=tqv,_Tmp8->f3=ct;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=c_args;_Tmp6;});
c_arg_types=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=ct,_Tmp6->tl=c_arg_types;_Tmp6;});}}}
# 1765
c_args=Cyc_List_imp_rev(c_args);
c_arg_types=Cyc_List_imp_rev(c_arg_types);{
void*ctype=Cyc_Port_fn_ct(c_rettype,c_arg_types);
# 1771
(*fd->name).f0=Cyc_Absyn_Loc_n;
fn_env=({struct Cyc_Port_Cenv*_Tmp2=fn_env;struct _tuple0*_Tmp3=fd->name;void*_Tmp4=(void*)fntype;void*_Tmp5=Cyc_Port_const_ct();Cyc_Port_add_var(_Tmp2,_Tmp3,_Tmp4,_Tmp5,ctype,0U);});
Cyc_Port_add_return_type(fn_env,c_rettype);
{struct Cyc_List_List*xs=c_args;for(0;(unsigned)xs;xs=xs->tl){
struct _tuple19 _Tmp2=*((struct _tuple19*)xs->hd);void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp2.f0;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{struct _fat_ptr*x=_Tmp6;void*t=_Tmp5;void*q=_Tmp4;void*ct=_Tmp3;
fn_env=({struct Cyc_Port_Cenv*_Tmp7=fn_env;struct _tuple0*_Tmp8=({struct _tuple0*_Tmp9=_cycalloc(sizeof(struct _tuple0));_Tmp9->f0=Cyc_Absyn_Loc_n,_Tmp9->f1=_check_null(x);_Tmp9;});void*_Tmp9=t;void*_TmpA=q;Cyc_Port_add_var(_Tmp7,_Tmp8,_Tmp9,_TmpA,ct,0U);});}}}
# 1778
Cyc_Port_gen_stmt(fn_env,fd->body);
# 1783
Cyc_Port_generalize(0,ctype);{
# 1790
struct Cyc_Dict_Dict counts=({struct Cyc_Dict_Dict(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Dict_Dict(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty;_Tmp2;})(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,ctype);
# 1794
Cyc_Port_register_rgns(env,counts,1,(void*)fntype,ctype);
env=({struct Cyc_Port_Cenv*_Tmp2=fn_env;struct _tuple0*_Tmp3=fd->name;void*_Tmp4=(void*)fntype;void*_Tmp5=Cyc_Port_const_ct();Cyc_Port_add_var(_Tmp2,_Tmp3,_Tmp4,_Tmp5,ctype,0U);});
if((unsigned)predeclared){
# 1803
struct _tuple15 _Tmp2=*predeclared;void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp2.f0;_Tmp4=_Tmp2.f1->f0;_Tmp3=_Tmp2.f1->f1;{void*orig_type=_Tmp5;void*q2=_Tmp4;void*t2=_Tmp3;
({void*_Tmp6=q2;Cyc_Port_unifies(_Tmp6,Cyc_Port_const_ct());});
({void*_Tmp6=t2;Cyc_Port_unifies(_Tmp6,Cyc_Port_instantiate(ctype));});
# 1807
Cyc_Port_register_rgns(env,counts,1,orig_type,ctype);}}
# 1809
return env;}}}}}}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 1815
void*t=_check_null(td->defn);
void*ct=Cyc_Port_type_to_ctype(env,t);
(*td->name).f0=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,td->name,t,ct);
return env;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 1825
struct _tuple0*name=ad->name;
(*ad->name).f0=Cyc_Absyn_Loc_n;{
struct _tuple11*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _Tmp2=ad->kind;if(_Tmp2==Cyc_Absyn_StructA){
# 1831
previous=Cyc_Port_lookup_struct_decl(env,name);
goto _LL20;}else{
# 1834
previous=Cyc_Port_lookup_union_decl(env,name);
goto _LL20;}_LL20:;}
# 1837
if((unsigned)ad->impl){
struct Cyc_List_List*cf=(*previous).f1;
{struct Cyc_List_List*fields=ad->impl->fields;for(0;(unsigned)fields;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)fields->hd;
void*qv;
if(env->gcenv->porting){
qv=Cyc_Port_var();
({struct Cyc_Port_Cenv*_Tmp2=env;void*_Tmp3=qv;void*_Tmp4=
f->tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1844
Cyc_Port_register_const_cvar(_Tmp2,_Tmp3,_Tmp4,f->tq.loc);});}else{
# 1853
qv=f->tq.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();}{
# 1855
void*ct=Cyc_Port_type_to_ctype(env,f->type);
if(cf!=0){
struct Cyc_Port_Cfield _Tmp2=*((struct Cyc_Port_Cfield*)cf->hd);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.qual;_Tmp3=_Tmp2.type;{void*qv2=_Tmp4;void*ct2=_Tmp3;
cf=cf->tl;
Cyc_Port_unifies(qv,qv2);
Cyc_Port_unifies(ct,ct2);}}
# 1862
curr=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Port_Cfield*_Tmp3=({struct Cyc_Port_Cfield*_Tmp4=_cycalloc(sizeof(struct Cyc_Port_Cfield));_Tmp4->qual=qv,_Tmp4->f=f->name,_Tmp4->type=ct;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=curr;_Tmp2;});}}}
# 1864
curr=Cyc_List_imp_rev(curr);
if((*previous).f1==0)
(*previous).f1=curr;}
# 1869
return env;}}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
# 1874
(*ed->name).f0=Cyc_Absyn_Loc_n;
# 1876
if((unsigned)ed->fields){
struct Cyc_List_List*fs=(struct Cyc_List_List*)ed->fields->v;for(0;(unsigned)fs;fs=fs->tl){
(*((struct Cyc_Absyn_Enumfield*)fs->hd)->name).f0=Cyc_Absyn_Loc_n;
env=({struct Cyc_Port_Cenv*_Tmp2=env;struct _tuple0*_Tmp3=((struct Cyc_Absyn_Enumfield*)fs->hd)->name;void*_Tmp4=Cyc_Absyn_enum_type(ed->name,ed);void*_Tmp5=
Cyc_Port_const_ct();
# 1879
Cyc_Port_add_var(_Tmp2,_Tmp3,_Tmp4,_Tmp5,
Cyc_Port_arith_ct(),0U);});}}
# 1882
return env;}default:
# 1884
 if(env->gcenv->porting)
Cyc_fprintf(Cyc_stderr,_tag_fat("Warning: Cyclone declaration found in code to be ported -- skipping.",sizeof(char),69U),_tag_fat(0U,sizeof(void*),0));
return env;};}
# 1891
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(1;(unsigned)ds;ds=ds->tl){
env=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);}
return env;}
# 1898
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1900
struct Cyc_Port_Cenv*env=({struct Cyc_Port_Cenv*_Tmp0=Cyc_Port_initial_cenv();Cyc_Port_gen_topdecls(_Tmp0,ds);});
# 1902
struct Cyc_List_List*ptrs=env->gcenv->pointer_edits;
struct Cyc_List_List*consts=env->gcenv->qualifier_edits;
struct Cyc_List_List*zts=env->gcenv->zeroterm_edits;
struct Cyc_List_List*edits=env->gcenv->edits;
struct Cyc_List_List*localvars=env->gcenv->vardecl_locs;
# 1931 "port.cyc"
{struct Cyc_List_List*ps=ptrs;for(0;(unsigned)ps;ps=ps->tl){
struct _tuple17 _Tmp0=*((struct _tuple17*)ps->hd);unsigned _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*new_ctype=_Tmp3;void*orig_ctype=_Tmp2;unsigned loc=_Tmp1;
Cyc_Port_unifies(new_ctype,orig_ctype);}}}
# 1935
{struct Cyc_List_List*cs=consts;for(0;(unsigned)cs;cs=cs->tl){
struct _tuple17 _Tmp0=*((struct _tuple17*)cs->hd);unsigned _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*new_qual=_Tmp3;void*old_qual=_Tmp2;unsigned loc=_Tmp1;
Cyc_Port_unifies(new_qual,old_qual);}}}
# 1939
{struct Cyc_List_List*zs=zts;for(0;(unsigned)zs;zs=zs->tl){
struct _tuple17 _Tmp0=*((struct _tuple17*)zs->hd);unsigned _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*new_zt=_Tmp3;void*old_zt=_Tmp2;unsigned loc=_Tmp1;
Cyc_Port_unifies(new_zt,old_zt);}}}
# 1945
for(1;(unsigned)ptrs;ptrs=ptrs->tl){
struct _tuple17 _Tmp0=*((struct _tuple17*)ptrs->hd);unsigned _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*new_ctype=_Tmp3;void*orig_ctype=_Tmp2;unsigned loc=_Tmp1;
if(!Cyc_Port_unifies(new_ctype,orig_ctype)&&(int)loc){
void*_Tmp4=Cyc_Port_compress_ct(orig_ctype);switch(*((int*)_Tmp4)){case 2:
# 1950
 edits=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Port_Edit*_Tmp6=({struct Cyc_Port_Edit*_Tmp7=_cycalloc(sizeof(struct Cyc_Port_Edit));_Tmp7->loc=loc,_Tmp7->old_string=_tag_fat("?",sizeof(char),2U),_Tmp7->new_string=_tag_fat("*",sizeof(char),2U);_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=edits;_Tmp5;});
goto _LLC;case 3:
# 1953
 edits=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Port_Edit*_Tmp6=({struct Cyc_Port_Edit*_Tmp7=_cycalloc(sizeof(struct Cyc_Port_Edit));_Tmp7->loc=loc,_Tmp7->old_string=_tag_fat("",sizeof(char),1U),_Tmp7->new_string=_tag_fat("@fat ",sizeof(char),6U);_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=edits;_Tmp5;});
goto _LLC;default:
 goto _LLC;}_LLC:;}}}
# 1960
for(1;(unsigned)consts;consts=consts->tl){
struct _tuple17 _Tmp0=*((struct _tuple17*)consts->hd);unsigned _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*new_qual=_Tmp3;void*old_qual=_Tmp2;unsigned loc=_Tmp1;
if(!Cyc_Port_unifies(new_qual,old_qual)&&(int)loc){
void*_Tmp4=Cyc_Port_compress_ct(old_qual);switch(*((int*)_Tmp4)){case 1:
# 1965
 edits=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Port_Edit*_Tmp6=({struct Cyc_Port_Edit*_Tmp7=_cycalloc(sizeof(struct Cyc_Port_Edit));_Tmp7->loc=loc,_Tmp7->old_string=_tag_fat("const ",sizeof(char),7U),_Tmp7->new_string=_tag_fat("",sizeof(char),1U);_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=edits;_Tmp5;});goto _LL16;case 0:
# 1967
 edits=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Port_Edit*_Tmp6=({struct Cyc_Port_Edit*_Tmp7=_cycalloc(sizeof(struct Cyc_Port_Edit));_Tmp7->loc=loc,_Tmp7->old_string=_tag_fat("",sizeof(char),1U),_Tmp7->new_string=_tag_fat("const ",sizeof(char),7U);_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=edits;_Tmp5;});goto _LL16;default:
 goto _LL16;}_LL16:;}}}
# 1973
for(1;(unsigned)zts;zts=zts->tl){
struct _tuple17 _Tmp0=*((struct _tuple17*)zts->hd);unsigned _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*new_zt=_Tmp3;void*old_zt=_Tmp2;unsigned loc=_Tmp1;
if(!Cyc_Port_unifies(new_zt,old_zt)&&(int)loc){
void*_Tmp4=Cyc_Port_compress_ct(old_zt);switch(*((int*)_Tmp4)){case 8:
# 1978
 edits=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Port_Edit*_Tmp6=({struct Cyc_Port_Edit*_Tmp7=_cycalloc(sizeof(struct Cyc_Port_Edit));_Tmp7->loc=loc,_Tmp7->old_string=_tag_fat("@nozeroterm ",sizeof(char),13U),_Tmp7->new_string=_tag_fat("",sizeof(char),1U);_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=edits;_Tmp5;});goto _LL20;case 9:
# 1980
 edits=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Port_Edit*_Tmp6=({struct Cyc_Port_Edit*_Tmp7=_cycalloc(sizeof(struct Cyc_Port_Edit));_Tmp7->loc=loc,_Tmp7->old_string=_tag_fat("@zeroterm ",sizeof(char),11U),_Tmp7->new_string=_tag_fat("",sizeof(char),1U);_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=edits;_Tmp5;});goto _LL20;default:
 goto _LL20;}_LL20:;}}}
# 1987
edits=({struct Cyc_List_List*(*_Tmp0)(int(*)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*))Cyc_List_merge_sort;_Tmp0;})(Cyc_Port_cmp_edit,edits);
# 1989
while((unsigned)edits &&((struct Cyc_Port_Edit*)edits->hd)->loc==0U){
# 1993
edits=edits->tl;1U;}
# 1995
return edits;}
# 2000
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*edits=Cyc_Port_gen_edits(ds);
struct Cyc_List_List*locs=({struct Cyc_List_List*(*_Tmp0)(unsigned(*)(struct Cyc_Port_Edit*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(unsigned(*)(struct Cyc_Port_Edit*),struct Cyc_List_List*))Cyc_List_map;_Tmp0;})(Cyc_Port_get_seg,edits);
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*slocs=Cyc_List_imp_rev(Cyc_Position_strings_of_segments(locs));
for(1;(unsigned)edits;(edits=edits->tl,slocs=slocs->tl)){
struct Cyc_Port_Edit _Tmp0=*((struct Cyc_Port_Edit*)edits->hd);struct _fat_ptr _Tmp1;struct _fat_ptr _Tmp2;unsigned _Tmp3;_Tmp3=_Tmp0.loc;_Tmp2=_Tmp0.old_string;_Tmp1=_Tmp0.new_string;{unsigned loc=_Tmp3;struct _fat_ptr s1=_Tmp2;struct _fat_ptr s2=_Tmp1;
struct _fat_ptr sloc=*((struct _fat_ptr*)_check_null(slocs)->hd);
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)sloc;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)s1;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)s2;_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_printf(_tag_fat("%s: insert `%s' for `%s'\n",sizeof(char),26U),_tag_fat(_Tmp7,sizeof(void*),3));});}}}}

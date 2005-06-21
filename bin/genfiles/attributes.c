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

# 118 "core.h"
 int Cyc_Core_intcmp(int,int);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 70
extern struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 52
extern int Cyc_zstrcmp(struct _fat_ptr,struct _fat_ptr);
# 109 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_AssnMap*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_AssnMap*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_AssnMap*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};
# 911 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 49 "tcutil.h"
int Cyc_Tcutil_is_bound_one(void*);
# 192
int Cyc_Tcutil_is_noalias_pointer(struct Cyc_List_List*,void*,int);
# 247
int Cyc_Tcutil_force_type2bool(int,void*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 25 "attributes.h"
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Atts_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Atts_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Atts_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Atts_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Atts_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Atts_Packed_att_val;
# 32
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Atts_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Atts_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Atts_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Atts_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Atts_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Atts_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Atts_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Atts_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Atts_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Atts_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Atts_Always_inline_att_val;
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 68
int Cyc_Atts_fntype_att(void*);
# 77
int Cyc_Atts_equal_att(void*,void*);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);
# 28 "attributes.cyc"
struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Atts_Stdcall_att_val={1};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Atts_Cdecl_att_val={2};
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Atts_Fastcall_att_val={3};
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Atts_Noreturn_att_val={4};
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Atts_Const_att_val={5};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Atts_Packed_att_val={7};
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Atts_Nocommon_att_val={9};
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Atts_Shared_att_val={10};
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Atts_Unused_att_val={11};
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Atts_Weak_att_val={12};
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Atts_Dllimport_att_val={13};
struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Atts_Dllexport_att_val={14};
struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Atts_No_instrument_function_att_val={15};
struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Atts_Constructor_att_val={16};
struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Atts_Destructor_att_val={17};
struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Atts_No_check_memory_usage_att_val={18};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Atts_Pure_att_val={23};
struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Atts_Always_inline_att_val={26};
struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val={27};
# 48
static void*Cyc_Atts_bad_attribute(unsigned loc){
Cyc_Warn_err(loc,_tag_fat("unrecognized attribute",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));
return(void*)& Cyc_Atts_Cdecl_att_val;}struct _tuple11{struct _fat_ptr f0;void*f1;};static char _TmpG0[8U]="stdcall";static char _TmpG1[6U]="cdecl";static char _TmpG2[9U]="fastcall";static char _TmpG3[9U]="noreturn";static char _TmpG4[6U]="const";static char _TmpG5[8U]="aligned";static char _TmpG6[7U]="packed";static char _TmpG7[7U]="shared";static char _TmpG8[7U]="unused";static char _TmpG9[5U]="weak";static char _TmpGA[10U]="dllimport";static char _TmpGB[10U]="dllexport";static char _TmpGC[23U]="no_instrument_function";static char _TmpGD[12U]="constructor";static char _TmpGE[11U]="destructor";static char _TmpGF[22U]="no_check_memory_usage";static char _TmpG10[5U]="pure";static char _TmpG11[14U]="always_inline";static char _TmpG12[9U]="no_throw";
# 53
void*Cyc_Atts_parse_nullary_att(unsigned loc,struct _fat_ptr s){
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,0};
static struct _tuple11 att_map[19U]={{.f0={_TmpG0,_TmpG0,_TmpG0 + 8U},.f1=(void*)& Cyc_Atts_Stdcall_att_val},{.f0={_TmpG1,_TmpG1,_TmpG1 + 6U},.f1=(void*)& Cyc_Atts_Cdecl_att_val},{.f0={_TmpG2,_TmpG2,_TmpG2 + 9U},.f1=(void*)& Cyc_Atts_Fastcall_att_val},{.f0={_TmpG3,_TmpG3,_TmpG3 + 9U},.f1=(void*)& Cyc_Atts_Noreturn_att_val},{.f0={_TmpG4,_TmpG4,_TmpG4 + 6U},.f1=(void*)& Cyc_Atts_Const_att_val},{.f0={_TmpG5,_TmpG5,_TmpG5 + 8U},.f1=(void*)& att_aligned},{.f0={_TmpG6,_TmpG6,_TmpG6 + 7U},.f1=(void*)& Cyc_Atts_Packed_att_val},{.f0={_TmpG7,_TmpG7,_TmpG7 + 7U},.f1=(void*)& Cyc_Atts_Shared_att_val},{.f0={_TmpG8,_TmpG8,_TmpG8 + 7U},.f1=(void*)& Cyc_Atts_Unused_att_val},{.f0={_TmpG9,_TmpG9,_TmpG9 + 5U},.f1=(void*)& Cyc_Atts_Weak_att_val},{.f0={_TmpGA,_TmpGA,_TmpGA + 10U},.f1=(void*)& Cyc_Atts_Dllimport_att_val},{.f0={_TmpGB,_TmpGB,_TmpGB + 10U},.f1=(void*)& Cyc_Atts_Dllexport_att_val},{.f0={_TmpGC,_TmpGC,_TmpGC + 23U},.f1=(void*)& Cyc_Atts_No_instrument_function_att_val},{.f0={_TmpGD,_TmpGD,_TmpGD + 12U},.f1=(void*)& Cyc_Atts_Constructor_att_val},{.f0={_TmpGE,_TmpGE,_TmpGE + 11U},.f1=(void*)& Cyc_Atts_Destructor_att_val},{.f0={_TmpGF,_TmpGF,_TmpGF + 22U},.f1=(void*)& Cyc_Atts_No_check_memory_usage_att_val},{.f0={_TmpG10,_TmpG10,_TmpG10 + 5U},.f1=(void*)& Cyc_Atts_Pure_att_val},{.f0={_TmpG11,_TmpG11,_TmpG11 + 14U},.f1=(void*)& Cyc_Atts_Always_inline_att_val},{.f0={_TmpG12,_TmpG12,_TmpG12 + 9U},.f1=(void*)& Cyc_Atts_No_throw_att_val}};
# 77
if((((_get_fat_size(s,sizeof(char))> 4U &&(int)((const char*)s.curr)[0]==95)&&(int)((const char*)s.curr)[1]==95)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- 2U)))==95)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- 3U)))==95)
# 79
s=Cyc_substring(s,2,_get_fat_size(s,sizeof(char))- 5U);{
int i=0;
for(1;(unsigned)i < 19U;++ i){
if(Cyc_strcmp(s,att_map[i].f0)==0)
return att_map[i].f1;}
return Cyc_Atts_bad_attribute(loc);}}
# 87
static int Cyc_Atts_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;int _Tmp1;if(*((int*)_Tmp0)==0){if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.tag==5){_Tmp1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.Int_c.val.f1;{int i=_Tmp1;
return i;}}else{goto _LL3;}}else{_LL3:
# 91
 Cyc_Warn_err(loc,_tag_fat("expecting integer constant",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
return 0;};}
# 95
static struct _fat_ptr Cyc_Atts_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;struct _fat_ptr _Tmp1;if(*((int*)_Tmp0)==0){if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.tag==8){_Tmp1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1.String_c.val;{struct _fat_ptr s=_Tmp1;
return s;}}else{goto _LL3;}}else{_LL3:
# 99
 Cyc_Warn_err(loc,_tag_fat("expecting string constant",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
return _tag_fat(0,0,0);};}
# 104
void*Cyc_Atts_parse_unary_att(unsigned sloc,struct _fat_ptr s,unsigned eloc,struct Cyc_Absyn_Exp*e){
void*a;
if(Cyc_zstrcmp(s,_tag_fat("aligned",sizeof(char),8U))==0 || Cyc_zstrcmp(s,_tag_fat("__aligned__",sizeof(char),12U))==0)
return(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct));_Tmp0->tag=6,_Tmp0->f1=e;_Tmp0;});
if(Cyc_zstrcmp(s,_tag_fat("section",sizeof(char),8U))==0 || Cyc_zstrcmp(s,_tag_fat("__section__",sizeof(char),12U))==0)
return(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct));_Tmp0->tag=8,({struct _fat_ptr _Tmp1=Cyc_Atts_exp2string(eloc,e);_Tmp0->f1=_Tmp1;});_Tmp0;});
if(Cyc_zstrcmp(s,_tag_fat("__mode__",sizeof(char),9U))==0)
return(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct));_Tmp0->tag=24,({struct _fat_ptr _Tmp1=Cyc_Atts_exp2string(eloc,e);_Tmp0->f1=_Tmp1;});_Tmp0;});
if(Cyc_zstrcmp(s,_tag_fat("alias",sizeof(char),6U))==0)
return(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct));_Tmp0->tag=25,({struct _fat_ptr _Tmp1=Cyc_Atts_exp2string(eloc,e);_Tmp0->f1=_Tmp1;});_Tmp0;});{
int n=Cyc_Atts_exp2int(eloc,e);
if(Cyc_zstrcmp(s,_tag_fat("regparm",sizeof(char),8U))==0 || Cyc_zstrcmp(s,_tag_fat("__regparm__",sizeof(char),12U))==0){
if(n < 0 || n > 3)
Cyc_Warn_err(eloc,_tag_fat("regparm requires value between 0 and 3",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));
return(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct));_Tmp0->tag=0,_Tmp0->f1=n;_Tmp0;});}
# 120
if(Cyc_zstrcmp(s,_tag_fat("initializes",sizeof(char),12U))==0 || Cyc_zstrcmp(s,_tag_fat("__initializes__",sizeof(char),16U))==0)
return(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct));_Tmp0->tag=20,_Tmp0->f1=n;_Tmp0;});
if(Cyc_zstrcmp(s,_tag_fat("noliveunique",sizeof(char),13U))==0 || Cyc_zstrcmp(s,_tag_fat("__noliveunique__",sizeof(char),17U))==0)
return(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct));_Tmp0->tag=21,_Tmp0->f1=n;_Tmp0;});
if(Cyc_zstrcmp(s,_tag_fat("consume",sizeof(char),8U))==0 || Cyc_zstrcmp(s,_tag_fat("__consume__",sizeof(char),12U))==0)
return(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct));_Tmp0->tag=22,_Tmp0->f1=n;_Tmp0;});
Cyc_Warn_err(sloc,_tag_fat("unrecognized attribute",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));
return Cyc_Atts_bad_attribute(sloc);}}
# 130
void*Cyc_Atts_parse_format_att(unsigned loc,unsigned s2loc,struct _fat_ptr s1,struct _fat_ptr s2,unsigned u1,unsigned u2){
# 132
if(!(Cyc_zstrcmp(s1,_tag_fat("format",sizeof(char),7U))==0)&& !(Cyc_zstrcmp(s1,_tag_fat("__format__",sizeof(char),11U))==0))
return Cyc_Atts_bad_attribute(loc);
if(Cyc_zstrcmp(s2,_tag_fat("printf",sizeof(char),7U))==0 || Cyc_zstrcmp(s2,_tag_fat("__printf__",sizeof(char),11U))==0)
return(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct));_Tmp0->tag=19,_Tmp0->f1=Cyc_Absyn_Printf_ft,_Tmp0->f2=(int)u1,_Tmp0->f3=(int)u2;_Tmp0;});
if(Cyc_zstrcmp(s2,_tag_fat("scanf",sizeof(char),6U))==0 || Cyc_zstrcmp(s2,_tag_fat("__scanf__",sizeof(char),10U))==0)
return(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct));_Tmp0->tag=19,_Tmp0->f1=Cyc_Absyn_Scanf_ft,_Tmp0->f2=(int)u1,_Tmp0->f3=(int)u2;_Tmp0;});
Cyc_Warn_err(loc,_tag_fat("unrecognized format type",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));
return(void*)& Cyc_Atts_Cdecl_att_val;}
# 144
int Cyc_Atts_is_noreturn_fn_type(void*t){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;switch(*((int*)_Tmp0)){case 3: _Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1.elt_type;{void*elt=_Tmp1;
return Cyc_Atts_is_noreturn_fn_type(elt);}case 5: _Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.attributes;{struct Cyc_List_List*atts=_Tmp1;
# 148
for(1;atts!=0;atts=atts->tl){
void*_Tmp2=(void*)atts->hd;if(*((int*)_Tmp2)==4)
return 1;else{
continue;};}
# 153
return 0;}default:
 return 0;};}
# 158
int Cyc_Atts_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a!=0;a=a->tl){
if(!Cyc_List_exists_c(Cyc_Atts_equal_att,(void*)a->hd,a2))
return 0;}}
{struct Cyc_List_List*a=a2;for(0;a!=0;a=a->tl){
if(!Cyc_List_exists_c(Cyc_Atts_equal_att,(void*)a->hd,a1))
return 0;}}
return 1;}
# 168
static int Cyc_Atts_is_regparm0_att(void*a){
if(*((int*)a)==0){if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)a)->f1==0)
return 1;else{goto _LL3;}}else{_LL3:
 return 0;};}
# 175
int Cyc_Atts_equiv_fn_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a!=0;a=a->tl){
if(!Cyc_List_exists_c(Cyc_Atts_equal_att,(void*)a->hd,a2)&& !Cyc_Atts_is_regparm0_att((void*)a->hd))
return 0;}}
{struct Cyc_List_List*a=a2;for(0;a!=0;a=a->tl){
if(!Cyc_List_exists_c(Cyc_Atts_equal_att,(void*)a->hd,a1)&& !Cyc_Atts_is_regparm0_att((void*)a->hd))
return 0;}}
return 1;}
# 187
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.attributes;{struct Cyc_List_List**fnatts=(struct Cyc_List_List**)_Tmp1;
# 190
struct Cyc_List_List*res_atts=0;
for(1;atts!=0;atts=atts->tl){
if(!Cyc_Atts_fntype_att((void*)atts->hd))
res_atts=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)atts->hd,_Tmp2->tl=res_atts;_Tmp2;});else{
if(!Cyc_List_mem(Cyc_Atts_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=(void*)atts->hd,_Tmp3->tl=*fnatts;_Tmp3;});*fnatts=_Tmp2;});}}
return res_atts;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("transfer_fn_type_atts",sizeof(char),22U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});};}
# 201
static int Cyc_Atts_fnTypeAttsRangeOK(unsigned loc,int i,int nargs,void*att){
if(i < 1 || i > nargs){
({struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp1;_Tmp1.tag=10,_Tmp1.f1=(void*)att;_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat(" has an out-of-range index",sizeof(char),27U);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return 0;}
# 206
return 1;}struct _tuple12{struct Cyc_List_List*f0;struct _fat_ptr f1;};
# 208
static void Cyc_Atts_fnTypeAttsOverlap(unsigned loc,int i,struct _tuple12 lst1,struct _tuple12 lst2){
# 211
if(({int(*_Tmp0)(struct Cyc_List_List*,int)=(int(*)(struct Cyc_List_List*,int))Cyc_List_memq;_Tmp0;})(lst2.f0,i))
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)lst1.f1;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)lst2.f1;_Tmp2;});struct Cyc_Int_pa_PrintArg_struct _Tmp2=({struct Cyc_Int_pa_PrintArg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=(unsigned long)i;_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_err(loc,_tag_fat("incompatible %s() and %s() attributes on parameter %d",sizeof(char),54U),_tag_fat(_Tmp3,sizeof(void*),3));});}
# 215
void Cyc_Atts_fnTypeAttsOK(unsigned loc,void*t){
struct _tuple12 init_params=({struct _tuple12 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=_tag_fat("initializes",sizeof(char),12U);_Tmp0;});
struct _tuple12 nolive_unique_params=({struct _tuple12 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=_tag_fat("noliveunique",sizeof(char),13U);_Tmp0;});
struct _tuple12 consume_params=({struct _tuple12 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=_tag_fat("consume",sizeof(char),8U);_Tmp0;});
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)_Tmp0)==5){_Tmp3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.attributes;_Tmp2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.args;_Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1.qual_bnd;{struct Cyc_List_List*atts=_Tmp3;struct Cyc_List_List*args=_Tmp2;struct Cyc_List_List*aqb=_Tmp1;
# 221
int nargs=Cyc_List_length(args);
for(1;atts!=0;atts=atts->tl){
void*_Tmp4=(void*)atts->hd;int _Tmp5;switch(*((int*)_Tmp4)){case 20: _Tmp5=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_Tmp4)->f1;{int i=_Tmp5;
# 225
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))
goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,init_params,nolive_unique_params);
Cyc_Atts_fnTypeAttsOverlap(loc,i,init_params,consume_params);{
struct _fat_ptr s=_tag_fat("initializes attribute allowed only on",sizeof(char),38U);
{void*_Tmp6=Cyc_Absyn_compress((*({struct _tuple8*(*_Tmp7)(struct Cyc_List_List*,int)=(struct _tuple8*(*)(struct Cyc_List_List*,int))Cyc_List_nth;_Tmp7;})(args,i - 1)).f2);void*_Tmp7;void*_Tmp8;void*_Tmp9;if(*((int*)_Tmp6)==3){_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1.ptr_atts.nullable;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1.ptr_atts.bounds;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1.ptr_atts.zero_term;{void*nullable=_Tmp9;void*bd=_Tmp8;void*zt=_Tmp7;
# 232
if(Cyc_Tcutil_force_type2bool(0,nullable))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=s;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" non-null pointers",sizeof(char),19U);_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),2));});
if(!Cyc_Tcutil_is_bound_one(bd))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=s;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" pointers of size 1",sizeof(char),20U);_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),2));});
if(Cyc_Tcutil_force_type2bool(0,zt))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=s;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" pointers to non-zero-terminated arrays",sizeof(char),40U);_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),2));});
goto _LLE;}}else{
# 240
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=s;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" pointers",sizeof(char),10U);_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;Cyc_Warn_err2(loc,_tag_fat(_TmpC,sizeof(void*),2));});}_LLE:;}
# 242
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=(void*)i,_Tmp7->tl=init_params.f0;_Tmp7;});init_params.f0=_Tmp6;});
goto _LL5;}}case 21: _Tmp5=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_Tmp4)->f1;{int i=_Tmp5;
# 245
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,nolive_unique_params,init_params);{
# 248
struct _tuple8*_Tmp6=({struct _tuple8*(*_Tmp7)(struct Cyc_List_List*,int)=(struct _tuple8*(*)(struct Cyc_List_List*,int))Cyc_List_nth;_Tmp7;})(args,i - 1);void*_Tmp7;_Tmp7=_Tmp6->f2;{void*t=_Tmp7;
if(!Cyc_Tcutil_is_noalias_pointer(aqb,t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("noliveunique allowed only on unique pointers",sizeof(char),45U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),1));});
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=(void*)i,_Tmp9->tl=nolive_unique_params.f0;_Tmp9;});nolive_unique_params.f0=_Tmp8;});
goto _LL5;}}}case 22: _Tmp5=((struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*)_Tmp4)->f1;{int i=_Tmp5;
# 254
if(!Cyc_Atts_fnTypeAttsRangeOK(loc,i,nargs,(void*)atts->hd))goto _LL5;
Cyc_Atts_fnTypeAttsOverlap(loc,i,consume_params,init_params);{
# 257
struct _tuple8*_Tmp6=({struct _tuple8*(*_Tmp7)(struct Cyc_List_List*,int)=(struct _tuple8*(*)(struct Cyc_List_List*,int))Cyc_List_nth;_Tmp7;})(args,i - 1);void*_Tmp7;_Tmp7=_Tmp6->f2;{void*t=_Tmp7;
if(!Cyc_Tcutil_is_noalias_pointer(aqb,t,0))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("consume allowed only on unique pointers",sizeof(char),40U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),1));});
({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=(void*)i,_Tmp9->tl=consume_params.f0;_Tmp9;});consume_params.f0=_Tmp8;});
goto _LL5;}}}default:
 goto _LL5;}_LL5:;}
# 265
goto _LL0;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("fnTypeAttsOK: not a function type",sizeof(char),34U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});}_LL0:;}
# 271
int Cyc_Atts_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t!=0;t=t->tl){
void*_Tmp0=(void*)t->hd;switch(*((int*)_Tmp0)){case 23:
 goto _LL4;case 4: _LL4:
 goto _LL6;case 20: _LL6:
 continue;default:
 if(!Cyc_List_exists_c(Cyc_Atts_equal_att,(void*)t->hd,a2))return 0;};}}
# 280
for(1;a2!=0;a2=a2->tl){
if(!Cyc_List_exists_c(Cyc_Atts_equal_att,(void*)a2->hd,a1)){
void*_Tmp0=(void*)a2->hd;if(*((int*)_Tmp0)==22)
# 284
goto _LL9;else{
# 286
return 0;}_LL9:;}}
# 289
return 1;}
# 292
static int Cyc_Atts_attribute_case_number(void*att){
switch(*((int*)att)){case 0:
 return 0;case 1:
 return 1;case 2:
 return 2;case 3:
 return 3;case 4:
 return 4;case 5:
 return 5;case 6:
 return 6;case 7:
 return 7;case 8:
 return 8;case 9:
 return 9;case 10:
 return 10;case 11:
 return 11;case 12:
 return 12;case 13:
 return 13;case 14:
 return 14;case 15:
 return 15;case 16:
 return 16;case 17:
 return 17;case 18:
 return 18;case 19:
 return 19;case 20:
 return 20;case 21:
 return 21;case 22:
 return 22;case 23:
 return 23;case 24:
 return 24;case 25:
 return 25;case 26:
 return 26;default:
 return 27;};}struct _tuple13{void*f0;void*f1;};
# 324
int Cyc_Atts_attribute_cmp(void*att1,void*att2){
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=att1,_Tmp1.f1=att2;_Tmp1;});int _Tmp1;int _Tmp2;enum Cyc_Absyn_Format_Type _Tmp3;enum Cyc_Absyn_Format_Type _Tmp4;struct _fat_ptr _Tmp5;struct _fat_ptr _Tmp6;void*_Tmp7;void*_Tmp8;int _Tmp9;int _TmpA;switch(*((int*)_Tmp0.f0)){case 0: if(*((int*)_Tmp0.f1)==0){_TmpA=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_Tmp0.f1)->f1;{int i1=_TmpA;int i2=_Tmp9;
_TmpA=i1;_Tmp9=i2;goto _LL4;}}else{goto _LLB;}case 20: if(*((int*)_Tmp0.f1)==20){_TmpA=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_Tmp0.f1)->f1;_LL4: {int i1=_TmpA;int i2=_Tmp9;
return Cyc_Core_intcmp(i1,i2);}}else{goto _LLB;}case 6: if(*((int*)_Tmp0.f1)==6){_Tmp8=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_Tmp0.f0)->f1;_Tmp7=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
# 329
if(e1==e2)return 0;
if(e1==0)return -1;
if(e2==0)return 1;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LLB;}case 8: if(*((int*)_Tmp0.f1)==8){_Tmp6=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_Tmp0.f0)->f1;_Tmp5=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_Tmp0.f1)->f1;{struct _fat_ptr s1=_Tmp6;struct _fat_ptr s2=_Tmp5;
return Cyc_strcmp(s1,s2);}}else{goto _LLB;}case 19: if(*((int*)_Tmp0.f1)==19){_Tmp4=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0.f0)->f1;_TmpA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0.f0)->f2;_Tmp9=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0.f0)->f3;_Tmp3=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0.f1)->f1;_Tmp2=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0.f1)->f2;_Tmp1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_Tmp0.f1)->f3;{enum Cyc_Absyn_Format_Type ft1=_Tmp4;int i1=_TmpA;int j1=_Tmp9;enum Cyc_Absyn_Format_Type ft2=_Tmp3;int i2=_Tmp2;int j2=_Tmp1;
# 335
int ftc=Cyc_Core_intcmp((int)((unsigned)ft1),(int)((unsigned)ft2));
if(ftc!=0)return ftc;{
int ic=Cyc_Core_intcmp(i1,i2);
if(ic!=0)return ic;
return Cyc_Core_intcmp(j1,j2);}}}else{goto _LLB;}default: _LLB:
# 341
 return({int _TmpB=Cyc_Atts_attribute_case_number(att1);Cyc_Core_intcmp(_TmpB,
Cyc_Atts_attribute_case_number(att2));});};}
# 345
int Cyc_Atts_equal_att(void*a1,void*a2){
return Cyc_Atts_attribute_cmp(a1,a2)==0;}
# 350
int Cyc_Atts_fntype_att(void*a){
int _Tmp0;switch(*((int*)a)){case 0: _Tmp0=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)a)->f1;{int i=_Tmp0;
goto _LL4;}case 3: _LL4:
 goto _LL6;case 1: _LL6:
 goto _LL8;case 2: _LL8:
 goto _LLA;case 4: _LLA:
 goto _LLC;case 23: _LLC:
 goto _LLE;case 26: _LLE:
 goto _LL10;case 19: _LL10:
 goto _LL12;case 5: _LL12:
 goto _LL14;case 21: _LL14:
 goto _LL16;case 20: _LL16:
 goto _LL18;case 22: _LL18:
 goto _LL1A;case 27: _LL1A:
 return 1;default:
 return 0;};}
# 369
void Cyc_Atts_check_fndecl_atts(unsigned loc,struct Cyc_List_List*atts,int is_defn){
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 6:
 goto _LL4;case 7: _LL4:
# 374
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("bad attribute ",sizeof(char),15U);_Tmp2;});struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp3;_Tmp3.tag=10,_Tmp3.f1=(void*)atts->hd;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" in function declaration",sizeof(char),25U);_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),3));});
return;default:
# 377
 if(!is_defn && Cyc_Atts_fntype_att((void*)atts->hd))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("function-type att in function variable declaration",sizeof(char),51U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;({int(*_Tmp3)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp3;})(_tag_fat(_Tmp2,sizeof(void*),1));});};}}
# 382
void Cyc_Atts_check_variable_atts(unsigned loc,struct Cyc_Absyn_Vardecl*vd,struct Cyc_List_List*atts){
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 6:
 goto _LL4;case 8: _LL4:
# 388
 goto _LL6;case 9: _LL6:
 goto _LL8;case 10: _LL8:
 goto _LLA;case 11: _LLA:
 goto _LLC;case 12: _LLC:
 goto _LLE;case 13: _LLE:
 goto _LL10;case 14: _LL10:
 continue;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("bad attribute ",sizeof(char),15U);_Tmp2;});struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp3;_Tmp3.tag=10,_Tmp3.f1=(void*)atts->hd;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" for variable ",sizeof(char),15U);_Tmp4;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp5;_Tmp5.tag=11,_Tmp5.f1=vd;_Tmp5;});void*_Tmp5[4];_Tmp5[0]=& _Tmp1,_Tmp5[1]=& _Tmp2,_Tmp5[2]=& _Tmp3,_Tmp5[3]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),4));});};}}
# 399
void Cyc_Atts_check_field_atts(unsigned loc,struct _fat_ptr*fn,struct Cyc_List_List*atts){
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 7:
 goto _LL4;case 6: _LL4:
 continue;default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("bad attribute ",sizeof(char),15U);_Tmp2;});struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Attribute_Warn_Warg_struct _Tmp3;_Tmp3.tag=10,_Tmp3.f1=(void*)atts->hd;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" on ",sizeof(char),5U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*fn;_Tmp5;});void*_Tmp5[4];_Tmp5[0]=& _Tmp1,_Tmp5[1]=& _Tmp2,_Tmp5[2]=& _Tmp3,_Tmp5[3]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),4));});};}}
# 409
struct Cyc_List_List*Cyc_Atts_merge_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
struct Cyc_List_List*x=0;
{struct Cyc_List_List*a=a1;for(0;a!=0;a=a->tl){
if(!Cyc_List_exists_c(Cyc_Atts_equal_att,(void*)a->hd,a2))
x=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)a->hd,_Tmp0->tl=x;_Tmp0;});}}
return({struct Cyc_List_List*_Tmp0=x;Cyc_List_append(_Tmp0,Cyc_List_copy(a2));});}
# 417
struct Cyc_List_List*Cyc_Atts_atts2c(struct Cyc_List_List*atts){
# 420
struct Cyc_List_List*ans=0;
for(1;atts!=0;atts=atts->tl){
void*_Tmp0=(void*)atts->hd;switch(*((int*)_Tmp0)){case 4:
 goto _LL4;case 5: _LL4:
 goto _LL6;case 19: _LL6:
 continue;case 22:
 continue;case 21:
 continue;case 20:
 continue;case 27:
 continue;default:
 ans=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)atts->hd,_Tmp1->tl=ans;_Tmp1;});goto _LL0;}_LL0:;}
# 432
return Cyc_List_imp_rev(ans);}

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
 extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 27 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);
# 37
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);
extern struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 43
extern struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned,struct _fat_ptr);
extern unsigned Cyc_Position_get_seg(struct Cyc_Position_Error*);
extern struct _fat_ptr Cyc_Position_get_desc(struct Cyc_Position_Error*);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 224 "cycboot.h"
extern int Cyc_vfprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 232
extern struct _fat_ptr Cyc_vrprintf(struct _RegionHandle*,struct _fat_ptr,struct _fat_ptr);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 62 "string.h"
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _fat_ptr Cyc_Absynpp_attribute2string(void*);
struct _fat_ptr Cyc_Absynpp_kindbound2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 28 "warn.h"
extern int Cyc_Warn_print_warnings;
# 30
extern int Cyc_Warn_num_errors;
extern int Cyc_Warn_max_errors;struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 32 "warn.cyc"
int Cyc_Warn_print_warnings=1;
static int Cyc_Warn_pr(struct _fat_ptr fmt,struct _fat_ptr ap){int _T0;int _T1;_T0=Cyc_Warn_print_warnings;
# 36
if(!_T0)goto _TL0;_T1=
Cyc_vfprintf(Cyc_stderr,fmt,ap);return _T1;
# 39
_TL0: return 0;}
# 41
static int Cyc_Warn_flush (void){int _T0;_T0=
Cyc_fflush(Cyc_stderr);return _T0;}
# 45
static struct Cyc_List_List*Cyc_Warn_warning_segs=0;
static struct Cyc_List_List*Cyc_Warn_warning_msgs=0;
# 66
void Cyc_Warn_vwarn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;struct _fat_ptr*_T3;
struct _fat_ptr msg=Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));_T1=loc;
_T4->hd=(void*)_T1;_T4->tl=Cyc_Warn_warning_segs;_T0=(struct Cyc_List_List*)_T4;}Cyc_Warn_warning_segs=_T0;{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T5=_cycalloc(sizeof(struct _fat_ptr));
*_T5=msg;_T3=(struct _fat_ptr*)_T5;}_T4->hd=_T3;_T4->tl=Cyc_Warn_warning_msgs;_T2=(struct Cyc_List_List*)_T4;}Cyc_Warn_warning_msgs=_T2;}
# 72
void Cyc_Warn_warn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 74
Cyc_Warn_vwarn(loc,fmt,ap);}
# 76
void Cyc_Warn_flush_warnings (void){struct _fat_ptr _T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_List_List*_T3;void*_T4;struct _fat_ptr*_T5;struct Cyc_String_pa_PrintArg_struct _T6;struct Cyc_List_List*_T7;void*_T8;struct _fat_ptr*_T9;void**_TA;void**_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;
if(Cyc_Warn_warning_segs!=0)goto _TL2;
return;_TL2: _T0=
_tag_fat("***Warnings***\n",sizeof(char),16U);_T1=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_pr(_T0,_T1);{
struct Cyc_List_List*seg_strs=Cyc_Position_strings_of_segments(Cyc_Warn_warning_segs);
Cyc_Warn_warning_segs=0;
Cyc_Warn_warning_msgs=Cyc_List_imp_rev(Cyc_Warn_warning_msgs);
_TL4: if(Cyc_Warn_warning_msgs!=0)goto _TL5;else{goto _TL6;}
_TL5:{struct Cyc_String_pa_PrintArg_struct _T12;_T12.tag=0;_T3=_check_null(seg_strs);_T4=_T3->hd;_T5=(struct _fat_ptr*)_T4;_T12.f1=*_T5;_T2=_T12;}{struct Cyc_String_pa_PrintArg_struct _T12=_T2;{struct Cyc_String_pa_PrintArg_struct _T13;_T13.tag=0;_T7=Cyc_Warn_warning_msgs;_T8=_T7->hd;_T9=(struct _fat_ptr*)_T8;_T13.f1=*_T9;_T6=_T13;}{struct Cyc_String_pa_PrintArg_struct _T13=_T6;void*_T14[2];_TA=_T14 + 0;*_TA=& _T12;_TB=_T14 + 1;*_TB=& _T13;_TC=_tag_fat("%s: %s\n",sizeof(char),8U);_TD=_tag_fat(_T14,sizeof(void*),2);Cyc_Warn_pr(_TC,_TD);}}_TE=seg_strs;
seg_strs=_TE->tl;_TF=
_check_null(Cyc_Warn_warning_msgs);Cyc_Warn_warning_msgs=_TF->tl;goto _TL4;_TL6: _T10=
# 88
_tag_fat("**************\n",sizeof(char),16U);_T11=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_pr(_T10,_T11);
Cyc_Warn_flush();}}
# 92
static int Cyc_Warn_error_b=0;
# 94
void Cyc_Warn_reset(struct _fat_ptr f){
Cyc_Position_reset_position(f);
Cyc_Warn_error_b=0;}
# 99
int Cyc_Warn_error_p (void){int _T0;_T0=Cyc_Warn_error_b;return _T0;}
# 101
int Cyc_Warn_first_error=1;
# 103
int Cyc_Warn_num_errors=0;
int Cyc_Warn_max_errors=10;
# 106
void Cyc_Warn_post_error(struct Cyc_Position_Error*e){int _T0;int _T1;struct Cyc___cycFILE*_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc_String_pa_PrintArg_struct _T5;unsigned _T6;struct Cyc_String_pa_PrintArg_struct _T7;void**_T8;void**_T9;struct Cyc___cycFILE*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc___cycFILE*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;_T0=Cyc_Warn_print_warnings;
if(_T0)goto _TL7;else{goto _TL9;}_TL9: return;_TL7:
 Cyc_Warn_error_b=1;
Cyc_fflush(Cyc_stdout);_T1=Cyc_Warn_first_error;
if(!_T1)goto _TLA;_T2=Cyc_stderr;_T3=
_tag_fat("\n",sizeof(char),2U);_T4=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2,_T3,_T4);
Cyc_Warn_first_error=0;goto _TLB;_TLA: _TLB:
# 115
 if(Cyc_Warn_num_errors > Cyc_Warn_max_errors)goto _TLC;{struct Cyc_String_pa_PrintArg_struct _T10;_T10.tag=0;_T6=
Cyc_Position_get_seg(e);_T10.f1=Cyc_Position_string_of_segment(_T6);_T5=_T10;}{struct Cyc_String_pa_PrintArg_struct _T10=_T5;{struct Cyc_String_pa_PrintArg_struct _T11;_T11.tag=0;_T11.f1=Cyc_Position_get_desc(e);_T7=_T11;}{struct Cyc_String_pa_PrintArg_struct _T11=_T7;void*_T12[2];_T8=_T12 + 0;*_T8=& _T10;_T9=_T12 + 1;*_T9=& _T11;_TA=Cyc_stderr;_TB=_tag_fat("%s: %s\n",sizeof(char),8U);_TC=_tag_fat(_T12,sizeof(void*),2);Cyc_fprintf(_TA,_TB,_TC);}}goto _TLD;_TLC: _TLD:
 if(Cyc_Warn_num_errors!=Cyc_Warn_max_errors)goto _TLE;_TD=Cyc_stderr;_TE=
_tag_fat("Too many error messages!\n",sizeof(char),26U);_TF=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TD,_TE,_TF);goto _TLF;_TLE: _TLF:
 Cyc_fflush(Cyc_stderr);
Cyc_Warn_num_errors=Cyc_Warn_num_errors + 1;}
# 123
void Cyc_Warn_verr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){unsigned _T0;struct _fat_ptr _T1;struct Cyc_Position_Error*_T2;_T0=loc;_T1=
Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);_T2=Cyc_Position_mk_err(_T0,_T1);Cyc_Warn_post_error(_T2);}
# 127
void Cyc_Warn_err(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 129
Cyc_Warn_verr(loc,fmt,ap);}
# 132
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap){int _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc_Core_Impossible_exn_struct*_T5;void*_T6;
struct _fat_ptr msg=Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);_T0=
Cyc_Warn_error_p();if(_T0)goto _TL10;else{goto _TL12;}
_TL12:{struct Cyc_String_pa_PrintArg_struct _T7;_T7.tag=0;_T7.f1=msg;_T1=_T7;}{struct Cyc_String_pa_PrintArg_struct _T7=_T1;void*_T8[1];_T2=_T8 + 0;*_T2=& _T7;_T3=_tag_fat("Compiler Error: %s\n",sizeof(char),20U);_T4=_tag_fat(_T8,sizeof(void*),1);Cyc_Warn_pr(_T3,_T4);}
Cyc_Warn_flush();goto _TL11;_TL10: _TL11:{struct Cyc_Core_Impossible_exn_struct*_T7=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T7->tag=Cyc_Core_Impossible;
# 138
_T7->f1=msg;_T5=(struct Cyc_Core_Impossible_exn_struct*)_T7;}_T6=(void*)_T5;_throw(_T6);}
# 141
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 143
Cyc_Warn_vimpos(fmt,ap);}
# 146
void*Cyc_Warn_vimpos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){int _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;struct Cyc_Core_Impossible_exn_struct*_T3;void*_T4;_T0=
Cyc_Warn_error_p();if(_T0)goto _TL13;else{goto _TL15;}
_TL15: _T1=_tag_fat("Compiler Error: \n",sizeof(char),18U);_T2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_pr(_T1,_T2);
Cyc_Warn_verr(loc,fmt,ap);goto _TL14;_TL13: _TL14:{struct Cyc_Core_Impossible_exn_struct*_T5=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T5->tag=Cyc_Core_Impossible;
# 151
_T5->f1=_tag_fat("Compiler Error",sizeof(char),15U);_T3=(struct Cyc_Core_Impossible_exn_struct*)_T5;}_T4=(void*)_T3;_throw(_T4);}
# 154
void*Cyc_Warn_impos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 156
Cyc_Warn_vimpos_loc(loc,fmt,ap);}
# 159
static struct _fat_ptr Cyc_Warn_args2string(struct _fat_ptr args){int _T0;unsigned _T1;struct _fat_ptr _T2;unsigned _T3;struct _fat_ptr _T4;unsigned char*_T5;void**_T6;int _T7;int*_T8;unsigned _T9;void*_TA;void*_TB;unsigned _TC;void*_TD;struct _fat_ptr _TE;struct Cyc_Absyn_Aggrdecl*_TF;enum Cyc_Absyn_AggrKind _T10;int _T11;struct Cyc_Absyn_Aggrdecl*_T12;struct _tuple0*_T13;struct _fat_ptr _T14;struct Cyc_Absyn_Tvar*_T15;struct _fat_ptr*_T16;void*_T17;void*_T18;struct Cyc_Absyn_Vardecl*_T19;struct _tuple0*_T1A;struct _fat_ptr _T1B;struct Cyc_Int_pa_PrintArg_struct _T1C;int _T1D;void**_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;unsigned long _T21;struct Cyc_List_List*_T22;struct _fat_ptr**_T23;struct _fat_ptr*_T24;struct _fat_ptr**_T25;struct _fat_ptr*_T26;struct _fat_ptr**_T27;struct _fat_ptr*_T28;struct _fat_ptr _T29;struct Cyc_List_List*_T2A;int _T2B;struct Cyc_List_List*_T2C;struct _fat_ptr**_T2D;struct _fat_ptr*_T2E;struct _fat_ptr**_T2F;struct _fat_ptr*_T30;struct _fat_ptr _T31;struct Cyc_List_List*_T32;struct Cyc_List_List*_T33;struct _fat_ptr*_T34;int _T35;struct Cyc_List_List*_T36;struct _fat_ptr _T37;
struct Cyc_List_List*lst=0;
int curr_line_len=0;{
int i=0;_TL19: _T0=i;_T1=(unsigned)_T0;_T2=args;_T3=_get_fat_size(_T2,sizeof(void*));if(_T1 < _T3)goto _TL17;else{goto _TL18;}
_TL17:{struct _fat_ptr s;_T4=args;_T5=_T4.curr;_T6=(void**)_T5;_T7=i;{
void*_T38=_T6[_T7];int _T39;struct Cyc_Absyn_Vardecl*_T3A;void*_T3B;struct _fat_ptr _T3C;_T8=(int*)_T38;_T9=*_T8;switch(_T9){case 0:{struct Cyc_Warn_String_Warn_Warg_struct*_T3D=(struct Cyc_Warn_String_Warn_Warg_struct*)_T38;_T3C=_T3D->f1;}{struct _fat_ptr s2=_T3C;
s=s2;goto _LL0;}case 4:{struct Cyc_Warn_Exp_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Exp_Warn_Warg_struct*)_T38;_T3B=_T3D->f1;}{struct Cyc_Absyn_Exp*e=_T3B;
s=Cyc_Absynpp_exp2string(e);goto _LL0;}case 3:{struct Cyc_Warn_TypOpt_Warn_Warg_struct*_T3D=(struct Cyc_Warn_TypOpt_Warn_Warg_struct*)_T38;_TA=_T3D->f1;_T3B=(void*)_TA;}{void*t=_T3B;_TB=t;_TC=(unsigned)_TB;
if(!_TC)goto _TL1B;_T3B=t;goto _LL8;_TL1B: s=_tag_fat("<?>",sizeof(char),4U);goto _LL0;}case 2:{struct Cyc_Warn_Typ_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Typ_Warn_Warg_struct*)_T38;_TD=_T3D->f1;_T3B=(void*)_TD;}_LL8: {void*t=_T3B;
s=Cyc_Absynpp_typ2string(t);goto _LL0;}case 1:{struct Cyc_Warn_Qvar_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Qvar_Warn_Warg_struct*)_T38;_T3B=_T3D->f1;}{struct _tuple0*qv=_T3B;
s=Cyc_Absynpp_qvar2string(qv);goto _LL0;}case 5:{struct Cyc_Warn_Stmt_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Stmt_Warn_Warg_struct*)_T38;_T3B=_T3D->f1;}{struct Cyc_Absyn_Stmt*s2=_T3B;
s=Cyc_Absynpp_stmt2string(s2);goto _LL0;}case 6:{struct Cyc_Warn_Aggrdecl_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Aggrdecl_Warn_Warg_struct*)_T38;_T3B=_T3D->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T3B;_TF=ad;_T10=_TF->kind;_T11=(int)_T10;
if(_T11!=1)goto _TL1D;_TE=_tag_fat("union ",sizeof(char),7U);goto _TL1E;_TL1D: _TE=_tag_fat("struct ",sizeof(char),8U);_TL1E: _T12=ad;_T13=_T12->name;_T14=
Cyc_Absynpp_qvar2string(_T13);
# 171
s=Cyc_strconcat(_TE,_T14);goto _LL0;}case 7:{struct Cyc_Warn_Tvar_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Tvar_Warn_Warg_struct*)_T38;_T3B=_T3D->f1;}{struct Cyc_Absyn_Tvar*tv=_T3B;_T15=tv;_T16=_T15->name;
# 174
s=*_T16;goto _LL0;}case 9:{struct Cyc_Warn_Kind_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Kind_Warn_Warg_struct*)_T38;_T3B=_T3D->f1;}{struct Cyc_Absyn_Kind*k=_T3B;
s=Cyc_Absynpp_kind2string(k);goto _LL0;}case 8:{struct Cyc_Warn_KindBound_Warn_Warg_struct*_T3D=(struct Cyc_Warn_KindBound_Warn_Warg_struct*)_T38;_T17=_T3D->f1;_T3B=(void*)_T17;}{void*kb=_T3B;
s=Cyc_Absynpp_kindbound2string(kb);goto _LL0;}case 10:{struct Cyc_Warn_Attribute_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Attribute_Warn_Warg_struct*)_T38;_T18=_T3D->f1;_T3B=(void*)_T18;}{void*a=_T3B;
s=Cyc_Absynpp_attribute2string(a);goto _LL0;}case 11:{struct Cyc_Warn_Vardecl_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Vardecl_Warn_Warg_struct*)_T38;_T3A=_T3D->f1;}{struct Cyc_Absyn_Vardecl*vd=_T3A;_T19=vd;_T1A=_T19->name;
s=Cyc_Absynpp_qvar2string(_T1A);goto _LL0;}default:{struct Cyc_Warn_Int_Warn_Warg_struct*_T3D=(struct Cyc_Warn_Int_Warn_Warg_struct*)_T38;_T39=_T3D->f1;}{int i=_T39;{struct Cyc_Int_pa_PrintArg_struct _T3D;_T3D.tag=1;_T1D=i;
_T3D.f1=(unsigned long)_T1D;_T1C=_T3D;}{struct Cyc_Int_pa_PrintArg_struct _T3D=_T1C;void*_T3E[1];_T1E=_T3E + 0;*_T1E=& _T3D;_T1F=_tag_fat("%d",sizeof(char),3U);_T20=_tag_fat(_T3E,sizeof(void*),1);_T1B=Cyc_aprintf(_T1F,_T20);}s=_T1B;goto _LL0;}}_LL0:;}_T21=
# 181
Cyc_strlen(s);{int s_len=(int)_T21;
if(s_len < 80)goto _TL1F;{struct _fat_ptr*_T38[3];_T23=_T38 + 0;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));
*_T39=_tag_fat("\n",sizeof(char),2U);_T24=(struct _fat_ptr*)_T39;}*_T23=_T24;_T25=_T38 + 1;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));*_T39=s;_T26=(struct _fat_ptr*)_T39;}*_T25=_T26;_T27=_T38 + 2;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));*_T39=_tag_fat("\n",sizeof(char),2U);_T28=(struct _fat_ptr*)_T39;}*_T27=_T28;_T29=_tag_fat(_T38,sizeof(struct _fat_ptr*),3);_T22=Cyc_List_list(_T29);}_T2A=lst;lst=Cyc_List_append(_T22,_T2A);
curr_line_len=0;goto _TL20;
_TL1F: _T2B=curr_line_len + s_len;if(_T2B < 80)goto _TL21;{struct _fat_ptr*_T38[2];_T2D=_T38 + 0;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));
*_T39=_tag_fat("\n",sizeof(char),2U);_T2E=(struct _fat_ptr*)_T39;}*_T2D=_T2E;_T2F=_T38 + 1;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));*_T39=s;_T30=(struct _fat_ptr*)_T39;}*_T2F=_T30;_T31=_tag_fat(_T38,sizeof(struct _fat_ptr*),2);_T2C=Cyc_List_list(_T31);}_T32=lst;lst=Cyc_List_append(_T2C,_T32);
curr_line_len=s_len;goto _TL22;
# 189
_TL21:{struct Cyc_List_List*_T38=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));*_T39=s;_T34=(struct _fat_ptr*)_T39;}_T38->hd=_T34;_T38->tl=lst;_T33=(struct Cyc_List_List*)_T38;}lst=_T33;_T35=s_len;
curr_line_len=curr_line_len + _T35;_TL22: _TL20:;}}
# 162
i=i + 1;goto _TL19;_TL18:;}_T36=
# 193
Cyc_List_imp_rev(lst);_T37=Cyc_strconcat_l(_T36);return _T37;}
# 195
void Cyc_Warn_verr2(unsigned loc,struct _fat_ptr args){unsigned _T0;struct _fat_ptr _T1;struct Cyc_Position_Error*_T2;_T0=loc;_T1=
Cyc_Warn_args2string(args);_T2=Cyc_Position_mk_err(_T0,_T1);Cyc_Warn_post_error(_T2);}
# 198
void Cyc_Warn_err2(unsigned loc,struct _fat_ptr args){
Cyc_Warn_verr2(loc,args);}
# 201
void Cyc_Warn_vwarn2(unsigned loc,struct _fat_ptr args){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;struct _fat_ptr*_T3;
struct _fat_ptr msg=Cyc_Warn_args2string(args);{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));_T1=loc;
_T4->hd=(void*)_T1;_T4->tl=Cyc_Warn_warning_segs;_T0=(struct Cyc_List_List*)_T4;}Cyc_Warn_warning_segs=_T0;{struct Cyc_List_List*_T4=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T5=_cycalloc(sizeof(struct _fat_ptr));
*_T5=msg;_T3=(struct _fat_ptr*)_T5;}_T4->hd=_T3;_T4->tl=Cyc_Warn_warning_msgs;_T2=(struct Cyc_List_List*)_T4;}Cyc_Warn_warning_msgs=_T2;}
# 206
void Cyc_Warn_warn2(unsigned loc,struct _fat_ptr args){
Cyc_Warn_vwarn2(loc,args);}
# 209
void*Cyc_Warn_vimpos2(struct _fat_ptr args){int _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc_Core_Impossible_exn_struct*_T5;void*_T6;
struct _fat_ptr msg=Cyc_Warn_args2string(args);_T0=
Cyc_Warn_error_p();if(_T0)goto _TL23;else{goto _TL25;}
_TL25:{struct Cyc_String_pa_PrintArg_struct _T7;_T7.tag=0;_T7.f1=msg;_T1=_T7;}{struct Cyc_String_pa_PrintArg_struct _T7=_T1;void*_T8[1];_T2=_T8 + 0;*_T2=& _T7;_T3=_tag_fat("Compiler Error: %s\n",sizeof(char),20U);_T4=_tag_fat(_T8,sizeof(void*),1);Cyc_Warn_pr(_T3,_T4);}
Cyc_Warn_flush();goto _TL24;_TL23: _TL24:{struct Cyc_Core_Impossible_exn_struct*_T7=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T7->tag=Cyc_Core_Impossible;
# 215
_T7->f1=msg;_T5=(struct Cyc_Core_Impossible_exn_struct*)_T7;}_T6=(void*)_T5;_throw(_T6);}
# 217
void*Cyc_Warn_impos2(struct _fat_ptr args){
Cyc_Warn_vimpos2(args);}
# 220
void*Cyc_Warn_vimpos_loc2(unsigned loc,struct _fat_ptr args){int _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;struct Cyc_Core_Impossible_exn_struct*_T3;void*_T4;
struct _fat_ptr msg=Cyc_Warn_args2string(args);_T0=
Cyc_Warn_error_p();if(_T0)goto _TL26;else{goto _TL28;}
_TL28: _T1=_tag_fat("Compiler Error: \n",sizeof(char),18U);_T2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_pr(_T1,_T2);
Cyc_Warn_verr2(loc,args);
Cyc_Warn_flush();goto _TL27;_TL26: _TL27:{struct Cyc_Core_Impossible_exn_struct*_T5=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T5->tag=Cyc_Core_Impossible;
# 227
_T5->f1=msg;_T3=(struct Cyc_Core_Impossible_exn_struct*)_T5;}_T4=(void*)_T3;_throw(_T4);}
# 229
void*Cyc_Warn_impos_loc2(unsigned loc,struct _fat_ptr args){
# 231
Cyc_Warn_vimpos_loc2(loc,args);}

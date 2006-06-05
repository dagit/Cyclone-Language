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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);
# 89
extern void Cyc_Hashtable_iter(void(*)(void*,void*),struct Cyc_Hashtable_Table*);
# 92
extern void Cyc_Hashtable_iter_c(void(*)(void*,void*,void*),struct Cyc_Hashtable_Table*,void*);
# 100
extern void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(void*));
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);
# 354
extern struct Cyc_List_List*Cyc_List_delete_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 36 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_loc(unsigned);
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};
# 939 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 974
extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);
# 1013
void*Cyc_Absyn_thinconst (void);
# 48 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 30 "bansheeif.h"
int Cyc_BansheeIf_resolve(void*);
# 277 "tcutil.h"
int Cyc_Tcutil_is_cvar_type(void*);
# 26 "flags.h"
extern int Cyc_Flags_verbose;
# 41
extern int Cyc_Flags_resolve;struct ptrbnd_;struct effect_;struct intbound_;struct list;
# 86 "bansheeif.cyc"
extern void cycinf_init (void);
# 94
extern struct ptrbnd_*ptrbnd_fresh(const char*);
extern struct ptrbnd_*ptrbnd_constant(const char*);
# 97
extern void ptrbnd_unify(struct ptrbnd_*,struct ptrbnd_*);
extern void ptrbnd_cunify(struct ptrbnd_*,struct ptrbnd_*);
extern struct ptrbnd_*ptrbnd_ecr(struct ptrbnd_*);
# 101
extern int ptrbnd_eq(struct ptrbnd_*,struct ptrbnd_*);struct numelts_decon{struct intbound_*f0;};
# 117
extern struct effect_*effect_list_head(struct list*);
extern struct list*effect_list_tail(struct list*);
# 122
extern struct effect_*effect_fresh(const char*);
# 125
extern struct effect_*effect_constant(const char*);
extern int effect_eq(struct effect_*,struct effect_*);
# 131
extern struct list*effect_tlb(struct effect_*);struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct{int tag;struct ptrbnd_*f1;};struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct{int tag;struct effect_*f1;};
# 159
static struct Cyc_List_List*Cyc_BansheeIf_pbconsts;
static struct Cyc_List_List*Cyc_BansheeIf_effconsts;
# 162
int Cyc_BansheeIf_banshee_init (void){
cycinf_init();
return 1;}
# 167
void Cyc_BansheeIf_add_variable(void*cvar){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T5;void**_T6;void**_T7;void*_T8;unsigned _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;int(*_TB)(struct _fat_ptr);void*(*_TC)(struct _fat_ptr);struct _fat_ptr _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;int(*_TF)(struct _fat_ptr);void*(*_T10)(struct _fat_ptr);struct _fat_ptr _T11;struct Cyc_Core_Opt*_T12;void*_T13;struct Cyc_Absyn_Kind*_T14;enum Cyc_Absyn_KindQual _T15;int _T16;void**_T17;struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T18;void**_T19;struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*_T1A;struct Cyc_Warn_String_Warn_Warg_struct _T1B;int(*_T1C)(struct _fat_ptr);void*(*_T1D)(struct _fat_ptr);struct _fat_ptr _T1E;struct Cyc_Warn_String_Warn_Warg_struct _T1F;struct Cyc_Warn_Typ_Warn_Warg_struct _T20;int(*_T21)(struct _fat_ptr);void*(*_T22)(struct _fat_ptr);struct _fat_ptr _T23;const char*_T24;void*_T25;struct Cyc_Core_Opt*_T26;_T0=cvar;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TL0;_T3=cvar;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T27=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T26=_T27->f1;_T4=cvar;_T5=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T4;_T6=& _T5->f4;_T25=(void**)_T6;_T24=_T27->f5;}{struct Cyc_Core_Opt*ok=_T26;void**bv=(void**)_T25;const char*name=_T24;_T7=bv;_T8=*_T7;_T9=(unsigned)_T8;
# 170
if(!_T9)goto _TL2;{struct Cyc_Warn_String_Warn_Warg_struct _T27;_T27.tag=0;
_T27.f1=_tag_fat("add_variable: Multiple add",sizeof(char),27U);_TA=_T27;}{struct Cyc_Warn_String_Warn_Warg_struct _T27=_TA;void*_T28[1];_T28[0]=& _T27;_TC=Cyc_Warn_impos2;{int(*_T29)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TC;_TB=_T29;}_TD=_tag_fat(_T28,sizeof(void*),1);_TB(_TD);}goto _TL3;_TL2: _TL3:
 if(name!=0)goto _TL4;{struct Cyc_Warn_String_Warn_Warg_struct _T27;_T27.tag=0;
_T27.f1=_tag_fat("add_variable: empty name",sizeof(char),25U);_TE=_T27;}{struct Cyc_Warn_String_Warn_Warg_struct _T27=_TE;void*_T28[1];_T28[0]=& _T27;_T10=Cyc_Warn_impos2;{int(*_T29)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T10;_TF=_T29;}_T11=_tag_fat(_T28,sizeof(void*),1);_TF(_T11);}goto _TL5;_TL4: _TL5: _T12=
_check_null(ok);_T13=_T12->v;{struct Cyc_Absyn_Kind*_T27=(struct Cyc_Absyn_Kind*)_T13;_T14=(struct Cyc_Absyn_Kind*)_T27;_T15=_T14->kind;_T16=(int)_T15;switch(_T16){case Cyc_Absyn_PtrBndKind: _T17=bv;{struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T28=_cycalloc(sizeof(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct));_T28->tag=0;
# 176
_T28->f1=ptrbnd_fresh(name);_T18=(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_T28;}*_T17=(void*)_T18;goto _LL5;case Cyc_Absyn_EffKind: _T19=bv;{struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*_T28=_cycalloc(sizeof(struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct));_T28->tag=1;
# 179
_T28->f1=effect_fresh(name);_T1A=(struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*)_T28;}*_T19=(void*)_T1A;goto _LL5;default:{struct Cyc_Warn_String_Warn_Warg_struct _T28;_T28.tag=0;
# 185
_T28.f1=_tag_fat("add_variable: Unsupported or null cvar kind",sizeof(char),44U);_T1B=_T28;}{struct Cyc_Warn_String_Warn_Warg_struct _T28=_T1B;void*_T29[1];_T29[0]=& _T28;_T1D=Cyc_Warn_impos2;{int(*_T2A)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1D;_T1C=_T2A;}_T1E=_tag_fat(_T29,sizeof(void*),1);_T1C(_T1E);}}_LL5:;}goto _LL0;}_TL0:{struct Cyc_Warn_String_Warn_Warg_struct _T27;_T27.tag=0;
# 189
_T27.f1=_tag_fat("add_variable: Bad type",sizeof(char),23U);_T1F=_T27;}{struct Cyc_Warn_String_Warn_Warg_struct _T27=_T1F;{struct Cyc_Warn_Typ_Warn_Warg_struct _T28;_T28.tag=2;_T28.f1=cvar;_T20=_T28;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T28=_T20;void*_T29[2];_T29[0]=& _T27;_T29[1]=& _T28;_T22=Cyc_Warn_impos2;{int(*_T2A)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T22;_T21=_T2A;}_T23=_tag_fat(_T29,sizeof(void*),2);_T21(_T23);}}_LL0:;}struct _tuple11{void*f0;struct ptrbnd_*f1;};struct _tuple12{void*f0;struct effect_*f1;};
# 193
void Cyc_BansheeIf_add_constant(struct _fat_ptr name,void*cvar){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T5;void**_T6;void**_T7;void*_T8;unsigned _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;int(*_TB)(struct _fat_ptr);void*(*_TC)(struct _fat_ptr);struct _fat_ptr _TD;struct Cyc_Core_Opt*_TE;void*_TF;struct Cyc_Absyn_Kind*_T10;enum Cyc_Absyn_KindQual _T11;int _T12;struct _fat_ptr _T13;char*_T14;const char*_T15;void**_T16;struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T17;struct Cyc_List_List*_T18;struct _tuple11*_T19;struct _fat_ptr _T1A;char*_T1B;const char*_T1C;void**_T1D;struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*_T1E;struct Cyc_List_List*_T1F;struct _tuple12*_T20;struct Cyc_Warn_String_Warn_Warg_struct _T21;int(*_T22)(struct _fat_ptr);void*(*_T23)(struct _fat_ptr);struct _fat_ptr _T24;struct Cyc_Warn_String_Warn_Warg_struct _T25;struct Cyc_Warn_Typ_Warn_Warg_struct _T26;int(*_T27)(struct _fat_ptr);void*(*_T28)(struct _fat_ptr);struct _fat_ptr _T29;void*_T2A;struct Cyc_Core_Opt*_T2B;_T0=cvar;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TL7;_T3=cvar;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T2B=_T2C->f1;_T4=cvar;_T5=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T4;_T6=& _T5->f4;_T2A=(void**)_T6;}{struct Cyc_Core_Opt*ok=_T2B;void**bv=(void**)_T2A;_T7=bv;_T8=*_T7;_T9=(unsigned)_T8;
# 196
if(!_T9)goto _TL9;{struct Cyc_Warn_String_Warn_Warg_struct _T2C;_T2C.tag=0;
_T2C.f1=_tag_fat("add_constant: Multiple add",sizeof(char),27U);_TA=_T2C;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C=_TA;void*_T2D[1];_T2D[0]=& _T2C;_TC=Cyc_Warn_impos2;{int(*_T2E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TC;_TB=_T2E;}_TD=_tag_fat(_T2D,sizeof(void*),1);_TB(_TD);}goto _TLA;_TL9: _TLA: _TE=
_check_null(ok);_TF=_TE->v;{struct Cyc_Absyn_Kind*_T2C=(struct Cyc_Absyn_Kind*)_TF;_T10=(struct Cyc_Absyn_Kind*)_T2C;_T11=_T10->kind;_T12=(int)_T11;switch(_T12){case Cyc_Absyn_PtrBndKind: _T13=name;_T14=_untag_fat_ptr_check_bound(_T13,sizeof(char),1U);_T15=(const char*)_T14;{
# 200
struct ptrbnd_*pbc=ptrbnd_constant(_T15);_T16=bv;{struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T2D=_cycalloc(sizeof(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct));_T2D->tag=0;
_T2D->f1=pbc;_T17=(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_T2D;}*_T16=(void*)_T17;{struct Cyc_List_List*_T2D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_T2E=_cycalloc(sizeof(struct _tuple11));
_T2E->f0=cvar;_T2E->f1=pbc;_T19=(struct _tuple11*)_T2E;}_T2D->hd=_T19;_T2D->tl=Cyc_BansheeIf_pbconsts;_T18=(struct Cyc_List_List*)_T2D;}Cyc_BansheeIf_pbconsts=_T18;goto _LL5;}case Cyc_Absyn_EffKind: _T1A=name;_T1B=_untag_fat_ptr_check_bound(_T1A,sizeof(char),1U);_T1C=(const char*)_T1B;{
# 205
struct effect_*effc=effect_constant(_T1C);_T1D=bv;{struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*_T2D=_cycalloc(sizeof(struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct));_T2D->tag=1;
_T2D->f1=effc;_T1E=(struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*)_T2D;}*_T1D=(void*)_T1E;{struct Cyc_List_List*_T2D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple12*_T2E=_cycalloc(sizeof(struct _tuple12));
_T2E->f0=cvar;_T2E->f1=effc;_T20=(struct _tuple12*)_T2E;}_T2D->hd=_T20;_T2D->tl=Cyc_BansheeIf_effconsts;_T1F=(struct Cyc_List_List*)_T2D;}Cyc_BansheeIf_effconsts=_T1F;goto _LL5;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T2D;_T2D.tag=0;
# 213
_T2D.f1=_tag_fat("add_constant: Unsupported or null cvar kind",sizeof(char),44U);_T21=_T2D;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D=_T21;void*_T2E[1];_T2E[0]=& _T2D;_T23=Cyc_Warn_impos2;{int(*_T2F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T23;_T22=_T2F;}_T24=_tag_fat(_T2E,sizeof(void*),1);_T22(_T24);}}_LL5:;}goto _LL0;}_TL7:{struct Cyc_Warn_String_Warn_Warg_struct _T2C;_T2C.tag=0;
# 217
_T2C.f1=_tag_fat("add_constant: Bad type",sizeof(char),23U);_T25=_T2C;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C=_T25;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2D;_T2D.tag=2;_T2D.f1=cvar;_T26=_T2D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2D=_T26;void*_T2E[2];_T2E[0]=& _T2C;_T2E[1]=& _T2D;_T28=Cyc_Warn_impos2;{int(*_T2F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T28;_T27=_T2F;}_T29=_tag_fat(_T2E,sizeof(void*),2);_T27(_T29);}}_LL0:;}
# 221
enum Cyc_BansheeIf_ConstraintOperator{Cyc_BansheeIf_AndOp =0U,Cyc_BansheeIf_OrOp =1U,Cyc_BansheeIf_NegOp =2U,Cyc_BansheeIf_EqOp =3U,Cyc_BansheeIf_SubsetOp =4U};struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct{int tag;void*f1;struct _fat_ptr f2;};struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct{int tag;enum Cyc_BansheeIf_ConstraintOperator f1;void*f2;void*f3;struct _fat_ptr f4;};struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct{int tag;void*f1;void*f2;int f3;struct _fat_ptr f4;};struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct{int tag;void*f1;void*f2;struct _fat_ptr f3;};struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct{int tag;void*f1;void*f2;struct _fat_ptr f3;};struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct{int tag;enum Cyc_BansheeIf_ConstraintOperator f1;void*f2;void*f3;struct _fat_ptr f4;};
# 241
void*Cyc_BansheeIf_check_constraint(void*c){struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T0;void*_T1;{struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T2=_cycalloc(sizeof(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct));_T2->tag=0;
_T2->f1=c;_T2->f2=_tag_fat(0,0,0);_T0=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T2;}_T1=(void*)_T0;return _T1;}struct _tuple13{void*f0;void*f1;};
# 245
void*Cyc_BansheeIf_cmpeq_constraint(void*t1,void*t2){struct _tuple13 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T9;void**_TA;void*_TB;void*_TC;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_TD;void**_TE;void**_TF;void*_T10;void**_T11;void*_T12;struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T13;void*_T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_Typ_Warn_Warg_struct _T16;struct Cyc_Warn_Typ_Warn_Warg_struct _T17;int(*_T18)(struct _fat_ptr);void*(*_T19)(struct _fat_ptr);struct _fat_ptr _T1A;{struct _tuple13 _T1B;
_T1B.f0=t1;_T1B.f1=t2;_T0=_T1B;}{struct _tuple13 _T1B=_T0;void*_T1C;void*_T1D;_T1=_T1B.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=3)goto _TLC;_T4=_T1B.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=3)goto _TLE;_T7=_T1B.f0;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1E=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T8=_T1B.f0;_T9=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T8;_TA=& _T9->f4;_T1D=(void**)_TA;}_TB=_T1B.f1;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1E=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TB;_TC=_T1B.f1;_TD=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TC;_TE=& _TD->f4;_T1C=(void**)_TE;}{void**bv1=(void**)_T1D;void**bv2=(void**)_T1C;_TF=bv1;_T10=*_TF;
# 248
if(_T10!=0)goto _TL10;Cyc_BansheeIf_add_variable(t1);goto _TL11;_TL10: _TL11: _T11=bv2;_T12=*_T11;
if(_T12!=0)goto _TL12;Cyc_BansheeIf_add_variable(t2);goto _TL13;_TL12: _TL13:{struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T1E=_cycalloc(sizeof(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct));_T1E->tag=1;
_T1E->f1=3U;_T1E->f2=t1;_T1E->f3=t2;_T1E->f4=_tag_fat(0,0,0);_T13=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T1E;}_T14=(void*)_T13;return _T14;}_TLE: goto _LL3;_TLC: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _T1E;_T1E.tag=0;
# 252
_T1E.f1=_tag_fat("cmpeq_constraint: Bad types for constraint - ",sizeof(char),46U);_T15=_T1E;}{struct Cyc_Warn_String_Warn_Warg_struct _T1E=_T15;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1F;_T1F.tag=2;_T1F.f1=t1;_T16=_T1F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1F=_T16;{struct Cyc_Warn_Typ_Warn_Warg_struct _T20;_T20.tag=2;_T20.f1=t2;_T17=_T20;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T20=_T17;void*_T21[3];_T21[0]=& _T1E;_T21[1]=& _T1F;_T21[2]=& _T20;_T19=Cyc_Warn_impos2;{int(*_T22)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T19;_T18=_T22;}_T1A=_tag_fat(_T21,sizeof(void*),3);_T18(_T1A);}}};}}
# 255
void*Cyc_BansheeIf_cmpsubset_constraint(void*t1,void*t2){struct Cyc_Warn_String_Warn_Warg_struct _T0;int(*_T1)(struct _fat_ptr);void*(*_T2)(struct _fat_ptr);struct _fat_ptr _T3;struct _tuple13 _T4;void*_T5;int*_T6;int _T7;void*_T8;int*_T9;int _TA;void*_TB;void*_TC;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_TD;void**_TE;void*_TF;void*_T10;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T11;void**_T12;void**_T13;void*_T14;void**_T15;void*_T16;struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T17;void*_T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;struct Cyc_Warn_Typ_Warn_Warg_struct _T1A;struct Cyc_Warn_Typ_Warn_Warg_struct _T1B;int(*_T1C)(struct _fat_ptr);void*(*_T1D)(struct _fat_ptr);struct _fat_ptr _T1E;{struct Cyc_Warn_String_Warn_Warg_struct _T1F;_T1F.tag=0;
_T1F.f1=_tag_fat("cmpsubset_constraint: Not yet supported",sizeof(char),40U);_T0=_T1F;}{struct Cyc_Warn_String_Warn_Warg_struct _T1F=_T0;void*_T20[1];_T20[0]=& _T1F;_T2=Cyc_Warn_impos2;{int(*_T21)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T2;_T1=_T21;}_T3=_tag_fat(_T20,sizeof(void*),1);_T1(_T3);}{struct _tuple13 _T1F;
_T1F.f0=t1;_T1F.f1=t2;_T4=_T1F;}{struct _tuple13 _T1F=_T4;void*_T20;void*_T21;_T5=_T1F.f0;_T6=(int*)_T5;_T7=*_T6;if(_T7!=3)goto _TL14;_T8=_T1F.f1;_T9=(int*)_T8;_TA=*_T9;if(_TA!=3)goto _TL16;_TB=_T1F.f0;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T22=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TB;_TC=_T1F.f0;_TD=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TC;_TE=& _TD->f4;_T21=(void**)_TE;}_TF=_T1F.f1;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T22=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TF;_T10=_T1F.f1;_T11=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T10;_T12=& _T11->f4;_T20=(void**)_T12;}{void**bv1=(void**)_T21;void**bv2=(void**)_T20;_T13=bv1;_T14=*_T13;
# 259
if(_T14!=0)goto _TL18;Cyc_BansheeIf_add_variable(t1);goto _TL19;_TL18: _TL19: _T15=bv2;_T16=*_T15;
if(_T16!=0)goto _TL1A;Cyc_BansheeIf_add_variable(t2);goto _TL1B;_TL1A: _TL1B:{struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T22=_cycalloc(sizeof(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct));_T22->tag=1;
_T22->f1=4U;_T22->f2=t1;_T22->f3=t2;_T22->f4=_tag_fat(0,0,0);_T17=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T22;}_T18=(void*)_T17;return _T18;}_TL16: goto _LL3;_TL14: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _T22;_T22.tag=0;
# 263
_T22.f1=_tag_fat("cmpsubset_constraint: Bad types for constraint - ",sizeof(char),50U);_T19=_T22;}{struct Cyc_Warn_String_Warn_Warg_struct _T22=_T19;{struct Cyc_Warn_Typ_Warn_Warg_struct _T23;_T23.tag=2;_T23.f1=t1;_T1A=_T23;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T23=_T1A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T24;_T24.tag=2;_T24.f1=t2;_T1B=_T24;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T24=_T1B;void*_T25[3];_T25[0]=& _T22;_T25[1]=& _T23;_T25[2]=& _T24;_T1D=Cyc_Warn_impos2;{int(*_T26)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T1D;_T1C=_T26;}_T1E=_tag_fat(_T25,sizeof(void*),3);_T1C(_T1E);}}};}}
# 267
static void*Cyc_BansheeIf_internal_equality_constraint(void*t1,void*t2,int c){struct _tuple13 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T9;void**_TA;void*_TB;void*_TC;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_TD;void**_TE;void**_TF;void*_T10;void**_T11;void*_T12;struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T13;void*_T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_Typ_Warn_Warg_struct _T16;struct Cyc_Warn_Typ_Warn_Warg_struct _T17;int(*_T18)(struct _fat_ptr);void*(*_T19)(struct _fat_ptr);struct _fat_ptr _T1A;{struct _tuple13 _T1B;
_T1B.f0=t1;_T1B.f1=t2;_T0=_T1B;}{struct _tuple13 _T1B=_T0;void*_T1C;void*_T1D;_T1=_T1B.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=3)goto _TL1C;_T4=_T1B.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=3)goto _TL1E;_T7=_T1B.f0;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1E=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T8=_T1B.f0;_T9=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T8;_TA=& _T9->f4;_T1D=(void**)_TA;}_TB=_T1B.f1;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1E=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TB;_TC=_T1B.f1;_TD=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TC;_TE=& _TD->f4;_T1C=(void**)_TE;}{void**bv1=(void**)_T1D;void**bv2=(void**)_T1C;_TF=bv1;_T10=*_TF;
# 270
if(_T10!=0)goto _TL20;Cyc_BansheeIf_add_variable(t1);goto _TL21;_TL20: _TL21: _T11=bv2;_T12=*_T11;
if(_T12!=0)goto _TL22;Cyc_BansheeIf_add_variable(t2);goto _TL23;_TL22: _TL23:{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T1E=_cycalloc(sizeof(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct));_T1E->tag=2;
_T1E->f1=t1;_T1E->f2=t2;_T1E->f3=c;_T1E->f4=_tag_fat(0,0,0);_T13=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T1E;}_T14=(void*)_T13;return _T14;}_TL1E: goto _LL3;_TL1C: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _T1E;_T1E.tag=0;
# 274
_T1E.f1=_tag_fat("equality_constraint: Bad types for constraint - ",sizeof(char),49U);_T15=_T1E;}{struct Cyc_Warn_String_Warn_Warg_struct _T1E=_T15;{struct Cyc_Warn_Typ_Warn_Warg_struct _T1F;_T1F.tag=2;_T1F.f1=t1;_T16=_T1F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T1F=_T16;{struct Cyc_Warn_Typ_Warn_Warg_struct _T20;_T20.tag=2;_T20.f1=t2;_T17=_T20;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T20=_T17;void*_T21[3];_T21[0]=& _T1E;_T21[1]=& _T1F;_T21[2]=& _T20;_T19=Cyc_Warn_impos2;{int(*_T22)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T19;_T18=_T22;}_T1A=_tag_fat(_T21,sizeof(void*),3);_T18(_T1A);}}};}}
# 278
void*Cyc_BansheeIf_equality_constraint(void*t1,void*t2){void*_T0;_T0=
Cyc_BansheeIf_internal_equality_constraint(t1,t2,0);return _T0;}
# 282
void*Cyc_BansheeIf_cond_equality_constraint(void*t1,void*t2){void*_T0;_T0=
Cyc_BansheeIf_internal_equality_constraint(t1,t2,1);return _T0;}
# 286
void*Cyc_BansheeIf_inclusion_constraint(void*t1,void*t2){struct _tuple13 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T9;void**_TA;void*_TB;void*_TC;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_TD;void**_TE;void**_TF;void*_T10;void**_T11;void*_T12;struct _tuple13 _T13;void**_T14;void**_T15;void*_T16;void*_T17;int*_T18;int _T19;void*_T1A;void*_T1B;int*_T1C;int _T1D;struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T1E;void*_T1F;struct Cyc_Warn_String_Warn_Warg_struct _T20;int(*_T21)(struct _fat_ptr);void*(*_T22)(struct _fat_ptr);struct _fat_ptr _T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;struct Cyc_Warn_Typ_Warn_Warg_struct _T25;struct Cyc_Warn_Typ_Warn_Warg_struct _T26;int(*_T27)(struct _fat_ptr);void*(*_T28)(struct _fat_ptr);struct _fat_ptr _T29;{struct _tuple13 _T2A;
_T2A.f0=t1;_T2A.f1=t2;_T0=_T2A;}{struct _tuple13 _T2A=_T0;void*_T2B;void*_T2C;_T1=_T2A.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=3)goto _TL24;_T4=_T2A.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=3)goto _TL26;_T7=_T2A.f0;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T2D=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T8=_T2A.f0;_T9=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T8;_TA=& _T9->f4;_T2C=(void**)_TA;}_TB=_T2A.f1;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T2D=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TB;_TC=_T2A.f1;_TD=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TC;_TE=& _TD->f4;_T2B=(void**)_TE;}{void**bv1=(void**)_T2C;void**bv2=(void**)_T2B;_TF=bv1;_T10=*_TF;
# 289
if(_T10!=0)goto _TL28;Cyc_BansheeIf_add_variable(t1);goto _TL29;_TL28: _TL29: _T11=bv2;_T12=*_T11;
if(_T12!=0)goto _TL2A;Cyc_BansheeIf_add_variable(t2);goto _TL2B;_TL2A: _TL2B:{struct _tuple13 _T2D;_T14=bv1;
_T2D.f0=*_T14;_T15=bv2;_T2D.f1=*_T15;_T13=_T2D;}{struct _tuple13 _T2D=_T13;_T16=_T2D.f0;if(_T16==0)goto _TL2C;_T17=_T2D.f0;_T18=(int*)_T17;_T19=*_T18;if(_T19!=1)goto _TL2E;_T1A=_T2D.f1;if(_T1A==0)goto _TL30;_T1B=_T2D.f1;_T1C=(int*)_T1B;_T1D=*_T1C;if(_T1D!=1)goto _TL32;{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T2E=_cycalloc(sizeof(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct));_T2E->tag=2;
# 293
_T2E->f1=t1;_T2E->f2=t2;_T2E->f3=0;_T2E->f4=_tag_fat(0,0,0);_T1E=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T2E;}_T1F=(void*)_T1E;return _T1F;_TL32: goto _LL8;_TL30: goto _LL8;_TL2E: goto _LL8;_TL2C: _LL8:{struct Cyc_Warn_String_Warn_Warg_struct _T2E;_T2E.tag=0;
# 295
_T2E.f1=_tag_fat("inclusion_constraint: Term sort doesn't permit inclusion constraints",sizeof(char),69U);_T20=_T2E;}{struct Cyc_Warn_String_Warn_Warg_struct _T2E=_T20;void*_T2F[1];_T2F[0]=& _T2E;_T22=Cyc_Warn_impos2;{int(*_T30)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T22;_T21=_T30;}_T23=_tag_fat(_T2F,sizeof(void*),1);_T21(_T23);};}}goto _TL27;_TL26: goto _LL3;_TL27: goto _TL25;_TL24: _LL3:{struct Cyc_Warn_String_Warn_Warg_struct _T2D;_T2D.tag=0;
# 298
_T2D.f1=_tag_fat("inclusion_constraint: Bad types for constraint - ",sizeof(char),50U);_T24=_T2D;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D=_T24;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2E;_T2E.tag=2;_T2E.f1=t1;_T25=_T2E;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2E=_T25;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2F;_T2F.tag=2;_T2F.f1=t2;_T26=_T2F;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2F=_T26;void*_T30[3];_T30[0]=& _T2D;_T30[1]=& _T2E;_T30[2]=& _T2F;_T28=Cyc_Warn_impos2;{int(*_T31)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T28;_T27=_T31;}_T29=_tag_fat(_T30,sizeof(void*),3);_T27(_T29);}}}_TL25:;}}
# 302
void*Cyc_BansheeIf_implication_constraint(void*t1,void*t2){struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T0;void*_T1;{struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T2=_cycalloc(sizeof(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct));_T2->tag=4;
_T2->f1=t1;_T2->f2=t2;_T2->f3=_tag_fat(0,0,0);_T0=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T2;}_T1=(void*)_T0;return _T1;}
# 306
static void*Cyc_BansheeIf_composite_constraint(enum Cyc_BansheeIf_ConstraintOperator op,void*t1,void*t2){enum Cyc_BansheeIf_ConstraintOperator _T0;struct Cyc_Warn_String_Warn_Warg_struct _T1;int(*_T2)(struct _fat_ptr);void*(*_T3)(struct _fat_ptr);struct _fat_ptr _T4;struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T5;void*_T6;void*_T7;unsigned _T8;void*_T9;unsigned _TA;struct Cyc_Warn_String_Warn_Warg_struct _TB;int(*_TC)(struct _fat_ptr);void*(*_TD)(struct _fat_ptr);struct _fat_ptr _TE;struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_TF;void*_T10;_T0=op;if(_T0!=Cyc_BansheeIf_NegOp)goto _TL34;
# 309
if(t2==0)goto _TL36;{struct Cyc_Warn_String_Warn_Warg_struct _T11;_T11.tag=0;
_T11.f1=_tag_fat("composite_constraint: Neg constraint is unary",sizeof(char),46U);_T1=_T11;}{struct Cyc_Warn_String_Warn_Warg_struct _T11=_T1;void*_T12[1];_T12[0]=& _T11;_T3=Cyc_Warn_impos2;{int(*_T13)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T3;_T2=_T13;}_T4=_tag_fat(_T12,sizeof(void*),1);_T2(_T4);}goto _TL37;_TL36: _TL37:{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T11=_cycalloc(sizeof(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct));_T11->tag=5;
_T11->f1=op;_T11->f2=t1;_T11->f3=0;_T11->f4=_tag_fat(0,0,0);_T5=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T11;}_T6=(void*)_T5;return _T6;_TL34: _T7=t1;_T8=(unsigned)_T7;
# 313
if(_T8)goto _TL3B;else{goto _TL3A;}_TL3B: _T9=t2;_TA=(unsigned)_T9;if(_TA)goto _TL38;else{goto _TL3A;}
_TL3A:{struct Cyc_Warn_String_Warn_Warg_struct _T11;_T11.tag=0;_T11.f1=_tag_fat("composite_constraint: Binary constraint missing argument",sizeof(char),57U);_TB=_T11;}{struct Cyc_Warn_String_Warn_Warg_struct _T11=_TB;void*_T12[1];_T12[0]=& _T11;_TD=Cyc_Warn_impos2;{int(*_T13)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TD;_TC=_T13;}_TE=_tag_fat(_T12,sizeof(void*),1);_TC(_TE);}goto _TL39;_TL38: _TL39:{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T11=_cycalloc(sizeof(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct));_T11->tag=5;
_T11->f1=op;_T11->f2=t1;_T11->f3=t2;_T11->f4=_tag_fat(0,0,0);_TF=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T11;}_T10=(void*)_TF;return _T10;;}
# 319
void*Cyc_BansheeIf_and_constraint(void*c1,void*c2){void*_T0;_T0=
Cyc_BansheeIf_composite_constraint(0U,c1,c2);return _T0;}
# 322
void*Cyc_BansheeIf_not_constraint(void*b){void*_T0;_T0=
Cyc_BansheeIf_composite_constraint(2U,b,0);return _T0;}
# 325
void*Cyc_BansheeIf_or_constraint(void*c1,void*c2){void*_T0;_T0=
Cyc_BansheeIf_composite_constraint(1U,c1,c2);return _T0;}
# 329
void*Cyc_BansheeIf_neg_constraint(void*c1){void*_T0;_T0=
Cyc_BansheeIf_composite_constraint(2U,c1,0);return _T0;}
# 333
static void*Cyc_BansheeIf_match_ptrbnd_constant(struct ptrbnd_*pb){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct _tuple11*_T4;struct ptrbnd_*_T5;struct ptrbnd_*_T6;int _T7;struct Cyc_List_List*_T8;void*_T9;struct _tuple11*_TA;void*_TB;struct Cyc_List_List*_TC;{
struct Cyc_List_List*l=Cyc_BansheeIf_pbconsts;_TL3F: _T0=l;_T1=(unsigned)_T0;if(_T1)goto _TL3D;else{goto _TL3E;}
_TL3D: _T2=l;_T3=_T2->hd;_T4=(struct _tuple11*)_T3;_T5=_T4->f1;_T6=pb;_T7=ptrbnd_eq(_T5,_T6);if(!_T7)goto _TL40;_T8=l;_T9=_T8->hd;_TA=(struct _tuple11*)_T9;_TB=_TA->f0;
return _TB;_TL40: _TC=l;
# 334
l=_TC->tl;goto _TL3F;_TL3E:;}
# 339
return 0;}
# 341
static int Cyc_BansheeIf_is_constant(void*t){void*_T0;int*_T1;int _T2;void*_T3;int _T4;int _T5;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TL42;_T3=t;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T6=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T5=_T6->f3;}{int id=_T5;
# 344
if(id!=1)goto _TL44;_T4=1;goto _TL45;_TL44: _T4=id==2;_TL45: return _T4;}_TL42:
# 346
 return 0;;}
# 351
static struct _fat_ptr Cyc_BansheeIf_op2string(enum Cyc_BansheeIf_ConstraintOperator op){enum Cyc_BansheeIf_ConstraintOperator _T0;int _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;_T0=op;_T1=(int)_T0;switch(_T1){case Cyc_BansheeIf_AndOp: _T2=
# 353
_tag_fat("A",sizeof(char),2U);return _T2;case Cyc_BansheeIf_OrOp: _T3=
_tag_fat("V",sizeof(char),2U);return _T3;case Cyc_BansheeIf_NegOp: _T4=
_tag_fat("!",sizeof(char),2U);return _T4;case Cyc_BansheeIf_EqOp: _T5=
_tag_fat("=",sizeof(char),2U);return _T5;case Cyc_BansheeIf_SubsetOp: _T6=
_tag_fat("<",sizeof(char),2U);return _T6;default: _T7=
_tag_fat("ERR",sizeof(char),4U);return _T7;};}
# 381 "bansheeif.cyc"
static struct _fat_ptr Cyc_BansheeIf_cvar2string(void*t1,int xc){int _T0;int _T1;int _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc_String_pa_PrintArg_struct _T5;void*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;_T0=
Cyc_Tcutil_is_cvar_type(t1);if(_T0)goto _TL49;else{goto _TL4A;}_TL4A: _T1=xc;if(_T1)goto _TL47;else{goto _TL4B;}_TL4B: _T2=Cyc_BansheeIf_is_constant(t1);if(_T2)goto _TL47;else{goto _TL49;}
_TL49: _T3=Cyc_Absynpp_typ2string(t1);return _T3;_TL47:{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;_T6=
Cyc_Absyn_compress(t1);_T9.f1=Cyc_Absynpp_typ2string(_T6);_T5=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T5;void*_TA[1];_TA[0]=& _T9;_T7=_tag_fat("\"%s\"",sizeof(char),5U);_T8=_tag_fat(_TA,sizeof(void*),1);_T4=Cyc_aprintf(_T7,_T8);}return _T4;}
# 387
static const char*Cyc_BansheeIf_cvarloc(void*cv){void*_T0;int*_T1;int _T2;void*_T3;const char*_T4;const char*_T5;_T0=cv;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TL4C;_T3=cv;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T6=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T5=_T6->f6;}{const char*l=_T5;_T4=l;
# 389
return _T4;}_TL4C:
 return 0;;}
# 394
static const char*Cyc_BansheeIf_cvarorigfat(void*cv){void*_T0;int*_T1;int _T2;void*_T3;const char*_T4;int _T5;int _T6;_T0=cv;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TL4E;_T3=cv;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T7=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T6=_T7->f7;}{int o=_T6;_T5=o;
# 396
if(!_T5)goto _TL50;_T4="true";goto _TL51;_TL50: _T4="false";_TL51: return _T4;}_TL4E:
 return 0;;}
# 401
static struct _fat_ptr Cyc_BansheeIf__constraint2string(void*c,int xcvar){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;struct _fat_ptr _T5;struct Cyc_String_pa_PrintArg_struct _T6;struct Cyc_String_pa_PrintArg_struct _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;void*_TA;void*_TB;void*_TC;struct _fat_ptr _TD;struct Cyc_String_pa_PrintArg_struct _TE;struct Cyc_String_pa_PrintArg_struct _TF;struct Cyc_String_pa_PrintArg_struct _T10;struct Cyc_String_pa_PrintArg_struct _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;void*_T14;void*_T15;void*_T16;int _T17;struct _fat_ptr _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct Cyc_String_pa_PrintArg_struct _T1A;struct Cyc_String_pa_PrintArg_struct _T1B;struct _fat_ptr _T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;struct Cyc_String_pa_PrintArg_struct _T1F;struct Cyc_String_pa_PrintArg_struct _T20;struct Cyc_String_pa_PrintArg_struct _T21;struct _fat_ptr _T22;struct _fat_ptr _T23;void*_T24;void*_T25;void*_T26;struct _fat_ptr _T27;struct Cyc_String_pa_PrintArg_struct _T28;struct Cyc_String_pa_PrintArg_struct _T29;struct Cyc_String_pa_PrintArg_struct _T2A;struct _fat_ptr _T2B;struct _fat_ptr _T2C;void*_T2D;void*_T2E;void*_T2F;struct _fat_ptr _T30;struct Cyc_String_pa_PrintArg_struct _T31;struct Cyc_String_pa_PrintArg_struct _T32;struct Cyc_String_pa_PrintArg_struct _T33;struct _fat_ptr _T34;struct _fat_ptr _T35;void*_T36;void*_T37;void*_T38;struct _fat_ptr _T39;struct Cyc_String_pa_PrintArg_struct _T3A;struct Cyc_String_pa_PrintArg_struct _T3B;struct Cyc_String_pa_PrintArg_struct _T3C;struct Cyc_String_pa_PrintArg_struct _T3D;void*_T3E;int _T3F;struct _fat_ptr _T40;struct _fat_ptr _T41;int _T42;void*_T43;enum Cyc_BansheeIf_ConstraintOperator _T44;struct _fat_ptr _T45;void*_T46;_T0=c;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=c;{struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T47=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T3;_T4=_T47->f1;_T46=(void*)_T4;_T45=_T47->f2;}{void*c1=_T46;struct _fat_ptr loc=_T45;{struct Cyc_String_pa_PrintArg_struct _T47;_T47.tag=0;
# 403
_T47.f1=loc;_T6=_T47;}{struct Cyc_String_pa_PrintArg_struct _T47=_T6;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;_T48.f1=Cyc_BansheeIf__constraint2string(c1,xcvar);_T7=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T7;void*_T49[2];_T49[0]=& _T47;_T49[1]=& _T48;_T8=_tag_fat("\"%s\"!(%s)",sizeof(char),10U);_T9=_tag_fat(_T49,sizeof(void*),2);_T5=Cyc_aprintf(_T8,_T9);}}return _T5;}case 1: _TA=c;{struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T47=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_TA;_T44=_T47->f1;_TB=_T47->f2;_T46=(void*)_TB;_TC=_T47->f3;_T43=(void*)_TC;_T45=_T47->f4;}{enum Cyc_BansheeIf_ConstraintOperator op=_T44;void*t1=_T46;void*t2=_T43;struct _fat_ptr loc=_T45;{struct Cyc_String_pa_PrintArg_struct _T47;_T47.tag=0;
_T47.f1=loc;_TE=_T47;}{struct Cyc_String_pa_PrintArg_struct _T47=_TE;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;_T48.f1=Cyc_BansheeIf_op2string(op);_TF=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_TF;{struct Cyc_String_pa_PrintArg_struct _T49;_T49.tag=0;_T49.f1=Cyc_BansheeIf_cvar2string(t1,xcvar);_T10=_T49;}{struct Cyc_String_pa_PrintArg_struct _T49=_T10;{struct Cyc_String_pa_PrintArg_struct _T4A;_T4A.tag=0;_T4A.f1=Cyc_BansheeIf_cvar2string(t2,xcvar);_T11=_T4A;}{struct Cyc_String_pa_PrintArg_struct _T4A=_T11;void*_T4B[4];_T4B[0]=& _T47;_T4B[1]=& _T48;_T4B[2]=& _T49;_T4B[3]=& _T4A;_T12=_tag_fat("\"%s\"^(%s,%s,%s)",sizeof(char),16U);_T13=_tag_fat(_T4B,sizeof(void*),4);_TD=Cyc_aprintf(_T12,_T13);}}}}return _TD;}case 2: _T14=c;{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T47=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T14;_T15=_T47->f1;_T46=(void*)_T15;_T16=_T47->f2;_T43=(void*)_T16;_T42=_T47->f3;_T45=_T47->f4;}{void*t1=_T46;void*t2=_T43;int b=_T42;struct _fat_ptr loc=_T45;_T17=b;
# 406
if(!_T17)goto _TL53;{struct Cyc_String_pa_PrintArg_struct _T47;_T47.tag=0;
_T47.f1=loc;_T19=_T47;}{struct Cyc_String_pa_PrintArg_struct _T47=_T19;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;_T48.f1=Cyc_BansheeIf_cvar2string(t1,xcvar);_T1A=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T1A;{struct Cyc_String_pa_PrintArg_struct _T49;_T49.tag=0;_T49.f1=Cyc_BansheeIf_cvar2string(t2,xcvar);_T1B=_T49;}{struct Cyc_String_pa_PrintArg_struct _T49=_T1B;void*_T4A[3];_T4A[0]=& _T47;_T4A[1]=& _T48;_T4A[2]=& _T49;_T1C=_tag_fat("\"%s\"?(%s,%s)",sizeof(char),13U);_T1D=_tag_fat(_T4A,sizeof(void*),3);_T18=Cyc_aprintf(_T1C,_T1D);}}}return _T18;
# 410
_TL53:{struct Cyc_String_pa_PrintArg_struct _T47;_T47.tag=0;_T47.f1=loc;_T1F=_T47;}{struct Cyc_String_pa_PrintArg_struct _T47=_T1F;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;_T48.f1=Cyc_BansheeIf_cvar2string(t1,xcvar);_T20=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T20;{struct Cyc_String_pa_PrintArg_struct _T49;_T49.tag=0;_T49.f1=Cyc_BansheeIf_cvar2string(t2,xcvar);_T21=_T49;}{struct Cyc_String_pa_PrintArg_struct _T49=_T21;void*_T4A[3];_T4A[0]=& _T47;_T4A[1]=& _T48;_T4A[2]=& _T49;_T22=_tag_fat("\"%s\"=(%s,%s)",sizeof(char),13U);_T23=_tag_fat(_T4A,sizeof(void*),3);_T1E=Cyc_aprintf(_T22,_T23);}}}return _T1E;}case 3: _T24=c;{struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*_T47=(struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_T24;_T25=_T47->f1;_T46=(void*)_T25;_T26=_T47->f2;_T43=(void*)_T26;_T45=_T47->f3;}{void*t1=_T46;void*t2=_T43;struct _fat_ptr loc=_T45;{struct Cyc_String_pa_PrintArg_struct _T47;_T47.tag=0;
_T47.f1=loc;_T28=_T47;}{struct Cyc_String_pa_PrintArg_struct _T47=_T28;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;
_T48.f1=Cyc_BansheeIf_cvar2string(t1,xcvar);_T29=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T29;{struct Cyc_String_pa_PrintArg_struct _T49;_T49.tag=0;_T49.f1=Cyc_BansheeIf_cvar2string(t2,xcvar);_T2A=_T49;}{struct Cyc_String_pa_PrintArg_struct _T49=_T2A;void*_T4A[3];_T4A[0]=& _T47;_T4A[1]=& _T48;_T4A[2]=& _T49;_T2B=
# 411
_tag_fat("\"%s\"<(%s,%s)",sizeof(char),13U);_T2C=_tag_fat(_T4A,sizeof(void*),3);_T27=Cyc_aprintf(_T2B,_T2C);}}}return _T27;}case 4: _T2D=c;{struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T47=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T2D;_T2E=_T47->f1;_T46=(void*)_T2E;_T2F=_T47->f2;_T43=(void*)_T2F;_T45=_T47->f3;}{void*c1=_T46;void*c2=_T43;struct _fat_ptr loc=_T45;{struct Cyc_String_pa_PrintArg_struct _T47;_T47.tag=0;
# 413
_T47.f1=loc;_T31=_T47;}{struct Cyc_String_pa_PrintArg_struct _T47=_T31;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;_T48.f1=Cyc_BansheeIf__constraint2string(c1,xcvar);_T32=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T32;{struct Cyc_String_pa_PrintArg_struct _T49;_T49.tag=0;_T49.f1=Cyc_BansheeIf__constraint2string(c2,xcvar);_T33=_T49;}{struct Cyc_String_pa_PrintArg_struct _T49=_T33;void*_T4A[3];_T4A[0]=& _T47;_T4A[1]=& _T48;_T4A[2]=& _T49;_T34=_tag_fat("\"%s\">(%s,%s)",sizeof(char),13U);_T35=_tag_fat(_T4A,sizeof(void*),3);_T30=Cyc_aprintf(_T34,_T35);}}}return _T30;}default: _T36=c;{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T47=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T36;_T44=_T47->f1;_T37=_T47->f2;_T46=(void*)_T37;_T38=_T47->f3;_T43=(void*)_T38;_T45=_T47->f4;}{enum Cyc_BansheeIf_ConstraintOperator op=_T44;void*c1=_T46;void*c2=_T43;struct _fat_ptr loc=_T45;{struct Cyc_String_pa_PrintArg_struct _T47;_T47.tag=0;
# 415
_T47.f1=loc;_T3A=_T47;}{struct Cyc_String_pa_PrintArg_struct _T47=_T3A;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;
_T48.f1=Cyc_BansheeIf__constraint2string(c1,xcvar);_T3B=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T3B;{struct Cyc_String_pa_PrintArg_struct _T49;_T49.tag=0;
_T49.f1=Cyc_BansheeIf_op2string(op);_T3C=_T49;}{struct Cyc_String_pa_PrintArg_struct _T49=_T3C;{struct Cyc_String_pa_PrintArg_struct _T4A;_T4A.tag=0;_T3E=_check_null(c2);_T3F=xcvar;_T4A.f1=Cyc_BansheeIf__constraint2string(_T3E,_T3F);_T3D=_T4A;}{struct Cyc_String_pa_PrintArg_struct _T4A=_T3D;void*_T4B[4];_T4B[0]=& _T47;_T4B[1]=& _T48;_T4B[2]=& _T49;_T4B[3]=& _T4A;_T40=
# 415
_tag_fat("\"%s\"+(%s,%s,%s)",sizeof(char),16U);_T41=_tag_fat(_T4B,sizeof(void*),4);_T39=Cyc_aprintf(_T40,_T41);}}}}return _T39;}};}
# 420
struct _fat_ptr Cyc_BansheeIf_constraint2string(void*c){struct _fat_ptr _T0;_T0=
Cyc_BansheeIf__constraint2string(c,1);return _T0;}
# 424
static void Cyc_BansheeIf_print_constraints(struct Cyc_List_List*c){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc___cycFILE*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_List_List*_T8;
_TL58: _T0=c;_T1=(unsigned)_T0;if(_T1)goto _TL56;else{goto _TL57;}
_TL56:{struct Cyc_String_pa_PrintArg_struct _T9;_T9.tag=0;_T3=c;_T4=_T3->hd;_T9.f1=Cyc_BansheeIf_constraint2string(_T4);_T2=_T9;}{struct Cyc_String_pa_PrintArg_struct _T9=_T2;void*_TA[1];_TA[0]=& _T9;_T5=Cyc_stderr;_T6=_tag_fat("%s\n",sizeof(char),4U);_T7=_tag_fat(_TA,sizeof(void*),1);Cyc_fprintf(_T5,_T6,_T7);}_T8=c;
# 425
c=_T8->tl;goto _TL58;_TL57:;}
# 435
static int Cyc_BansheeIf_nodelay_add_constraint(void*c){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T9;void*_TA;int*_TB;int _TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;struct _tuple13 _T12;void*_T13;void*_T14;int*_T15;int _T16;void*_T17;void*_T18;int*_T19;int _T1A;void*_T1B;void*_T1C;int _T1D;void*_T1E;unsigned _T1F;void*_T20;unsigned _T21;struct Cyc_Warn_String_Warn_Warg_struct _T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct _fat_ptr _T24;int _T25;struct Cyc___cycFILE*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;int _T29;int(*_T2A)(struct _fat_ptr,struct _fat_ptr);void*(*_T2B)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2C;struct _fat_ptr _T2D;void*_T2E;void*_T2F;void*_T30;enum Cyc_BansheeIf_ConstraintOperator _T31;int _T32;int(*_T33)(struct _fat_ptr,struct _fat_ptr);void*(*_T34)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T35;struct _fat_ptr _T36;int _T37;int _T38;void*_T39;void*_T3A;void*_T3B;void*_T3C;int(*_T3D)(struct _fat_ptr,struct _fat_ptr);void*(*_T3E)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T3F;struct _fat_ptr _T40;int(*_T41)(struct _fat_ptr,struct _fat_ptr);void*(*_T42)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T43;struct _fat_ptr _T44;enum Cyc_BansheeIf_ConstraintOperator _T45;int _T46;void*_T47;void*_T48;void*_T49;void*_T4A;_T0=c;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 2: _T3=c;_T4=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T3;_T5=_T4->f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=3)goto _TL5A;_T8=c;_T9=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T8;_TA=_T9->f2;_TB=(int*)_TA;_TC=*_TB;if(_TC!=3)goto _TL5C;_TD=c;{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T4B=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_TD;_TE=_T4B->f1;_T4A=(void*)_TE;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T4A;_TF=_T4C->f4;_T49=(void*)_TF;}_T10=_T4B->f2;_T48=(void*)_T10;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T48;_T11=_T4C->f4;_T47=(void*)_T11;}_T46=_T4B->f3;}{void*t1=_T4A;void*bv1=_T49;void*t2=_T48;void*bv2=_T47;int b=_T46;{struct _tuple13 _T4B;
# 438
_T4B.f0=bv1;_T4B.f1=bv2;_T12=_T4B;}{struct _tuple13 _T4B=_T12;struct ptrbnd_*_T4C;struct ptrbnd_*_T4D;_T13=_T4B.f0;if(_T13==0)goto _TL5E;_T14=_T4B.f0;_T15=(int*)_T14;_T16=*_T15;if(_T16!=0)goto _TL60;_T17=_T4B.f1;if(_T17==0)goto _TL62;_T18=_T4B.f1;_T19=(int*)_T18;_T1A=*_T19;if(_T1A!=0)goto _TL64;_T1B=_T4B.f0;{struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T4E=(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_T1B;_T4D=_T4E->f1;}_T1C=_T4B.f1;{struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T4E=(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_T1C;_T4C=_T4E->f1;}{struct ptrbnd_*pb1=_T4D;struct ptrbnd_*pb2=_T4C;
# 440
struct ptrbnd_*ecr1=ptrbnd_ecr(pb1);
struct ptrbnd_*ecr2=ptrbnd_ecr(pb2);_T1D=
ptrbnd_eq(ecr1,ecr2);if(_T1D)goto _TL66;else{goto _TL68;}_TL68: _T1E=
Cyc_BansheeIf_match_ptrbnd_constant(ecr1);_T1F=(unsigned)_T1E;
# 442
if(!_T1F)goto _TL66;_T20=
# 444
Cyc_BansheeIf_match_ptrbnd_constant(ecr2);_T21=(unsigned)_T20;
# 442
if(!_T21)goto _TL66;
# 446
Cyc_BansheeIf_resolve(t1);
Cyc_BansheeIf_resolve(t2);{struct Cyc_Warn_String_Warn_Warg_struct _T4E;_T4E.tag=0;
# 449
_T4E.f1=_tag_fat("Unsatisfiable constraint ",sizeof(char),26U);_T22=_T4E;}{struct Cyc_Warn_String_Warn_Warg_struct _T4E=_T22;{struct Cyc_Warn_String_Warn_Warg_struct _T4F;_T4F.tag=0;_T4F.f1=Cyc_BansheeIf__constraint2string(c,0);_T23=_T4F;}{struct Cyc_Warn_String_Warn_Warg_struct _T4F=_T23;void*_T50[2];_T50[0]=& _T4E;_T50[1]=& _T4F;_T24=_tag_fat(_T50,sizeof(void*),2);Cyc_Warn_err2(0U,_T24);}}
# 453
return 0;_TL66: _T25=Cyc_Flags_verbose;
# 455
if(!_T25)goto _TL69;_T26=Cyc_stderr;_T27=
_tag_fat("***Added\n",sizeof(char),10U);_T28=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T26,_T27,_T28);goto _TL6A;_TL69: _TL6A: _T29=b;
if(!_T29)goto _TL6B;
ptrbnd_cunify(pb1,pb2);goto _TL6C;
# 461
_TL6B: ptrbnd_unify(pb1,pb2);_TL6C:
# 463
 return 1;}_TL64: goto _LLC;_TL62: goto _LLC;_TL60: goto _LLC;_TL5E: _LLC: _T2B=Cyc_Warn_impos;{
# 465
int(*_T4E)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2B;_T2A=_T4E;}_T2C=_tag_fat("Kind not yet supported",sizeof(char),23U);_T2D=_tag_fat(0U,sizeof(void*),0);_T2A(_T2C,_T2D);;}goto _LL0;}_TL5C: goto _LL7;_TL5A: goto _LL7;case 5: _T2E=c;{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T4B=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T2E;_T45=_T4B->f1;_T2F=_T4B->f2;_T4A=(void*)_T2F;_T30=_T4B->f3;_T49=(void*)_T30;}{enum Cyc_BansheeIf_ConstraintOperator op=_T45;void*c1=_T4A;void*c2=_T49;_T31=op;_T32=(int)_T31;
# 469
if(_T32==0)goto _TL6D;_T34=Cyc_Warn_impos;{
int(*_T4B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T34;_T33=_T4B;}_T35=_tag_fat("Disjunctions and negations require a delay",sizeof(char),43U);_T36=_tag_fat(0U,sizeof(void*),0);_T33(_T35,_T36);goto _TL6E;_TL6D: _TL6E: _T38=
Cyc_BansheeIf_nodelay_add_constraint(c1);if(!_T38)goto _TL6F;_T39=
_check_null(c2);_T37=Cyc_BansheeIf_nodelay_add_constraint(_T39);goto _TL70;_TL6F: _T37=0;_TL70:
# 471
 return _T37;}case 3: _T3A=c;{struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*_T4B=(struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_T3A;_T3B=_T4B->f1;_T4A=(void*)_T3B;_T3C=_T4B->f2;_T49=(void*)_T3C;}{void*bv1=_T4A;void*bv2=_T49;_T3E=Cyc_Warn_impos;{
# 474
int(*_T4B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T3E;_T3D=_T4B;}_T3F=_tag_fat("setIF sort not yet supported",sizeof(char),29U);_T40=_tag_fat(0U,sizeof(void*),0);_T3D(_T3F,_T40);}default: _LL7: _T42=Cyc_Warn_impos;{
# 476
int(*_T4B)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T42;_T41=_T4B;}_T43=_tag_fat("No implications allowed",sizeof(char),24U);_T44=_tag_fat(0U,sizeof(void*),0);_T41(_T43,_T44);}_LL0:;}
# 480
static int Cyc_BansheeIf_requires_delay(void*c){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;int _T6;enum Cyc_BansheeIf_ConstraintOperator _T7;int _T8;int _T9;void*_TA;void*_TB;void*_TC;enum Cyc_BansheeIf_ConstraintOperator _TD;_T0=c;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: goto _LL4;case 1: _LL4: goto _LL6;case 4: _LL6:
# 485
 return 1;case 5: _T3=c;{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_TE=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T3;_TD=_TE->f1;_T4=_TE->f2;_TC=(void*)_T4;_T5=_TE->f3;_TB=(void*)_T5;}{enum Cyc_BansheeIf_ConstraintOperator op=_TD;void*c1=_TC;void*c2=_TB;_T7=op;_T8=(int)_T7;
# 487
if(_T8!=0)goto _TL74;else{goto _TL75;}_TL75: _T9=Cyc_BansheeIf_requires_delay(c1);if(_T9)goto _TL74;else{goto _TL72;}_TL74: _T6=1;goto _TL73;_TL72: _TA=_check_null(c2);_T6=Cyc_BansheeIf_requires_delay(_TA);_TL73: return _T6;}default:
# 489
 return 0;};}
# 493
static struct Cyc_List_List*Cyc_BansheeIf_delayed_constraints;
# 497
static int Cyc_BansheeIf_internal_add_constraint(void*c){int _T0;struct Cyc_List_List*_T1;int _T2;_T0=
Cyc_BansheeIf_requires_delay(c);if(!_T0)goto _TL76;{struct Cyc_List_List*_T3=_cycalloc(sizeof(struct Cyc_List_List));
_T3->hd=c;_T3->tl=Cyc_BansheeIf_delayed_constraints;_T1=(struct Cyc_List_List*)_T3;}Cyc_BansheeIf_delayed_constraints=_T1;
return 2;_TL76: _T2=
# 502
Cyc_BansheeIf_nodelay_add_constraint(c);return _T2;}
# 505
static int Cyc_BansheeIf_insert_into_map(void*);
# 507
void*Cyc_BansheeIf_add_location(struct _fat_ptr loc,void*c){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T5;struct _fat_ptr*_T6;void*_T7;void*_T8;struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T9;struct _fat_ptr*_TA;void*_TB;void*_TC;struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_TD;struct _fat_ptr*_TE;void*_TF;void*_T10;struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*_T11;struct _fat_ptr*_T12;void*_T13;void*_T14;struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T15;struct _fat_ptr*_T16;void*_T17;void*_T18;struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T19;struct _fat_ptr*_T1A;struct _fat_ptr*_T1B;void*_T1C;{void*_T1D;_T0=c;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=c;{struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T1E=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T3;_T4=c;_T5=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T4;_T6=& _T5->f2;_T1D=(struct _fat_ptr*)_T6;}{struct _fat_ptr*l=_T1D;_T1D=l;goto _LL4;}case 1: _T7=c;{struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T1E=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T7;_T8=c;_T9=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T8;_TA=& _T9->f4;_T1D=(struct _fat_ptr*)_TA;}_LL4: {struct _fat_ptr*l=_T1D;_T1D=l;goto _LL6;}case 2: _TB=c;{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T1E=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_TB;_TC=c;_TD=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_TC;_TE=& _TD->f4;_T1D=(struct _fat_ptr*)_TE;}_LL6: {struct _fat_ptr*l=_T1D;_T1D=l;goto _LL8;}case 3: _TF=c;{struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*_T1E=(struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_TF;_T10=c;_T11=(struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_T10;_T12=& _T11->f3;_T1D=(struct _fat_ptr*)_T12;}_LL8: {struct _fat_ptr*l=_T1D;_T1D=l;goto _LLA;}case 4: _T13=c;{struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T1E=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T13;_T14=c;_T15=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T14;_T16=& _T15->f3;_T1D=(struct _fat_ptr*)_T16;}_LLA: {struct _fat_ptr*l=_T1D;_T1D=l;goto _LLC;}default: _T17=c;{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T1E=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T17;_T18=c;_T19=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T18;_T1A=& _T19->f4;_T1D=(struct _fat_ptr*)_T1A;}_LLC: {struct _fat_ptr*l=(struct _fat_ptr*)_T1D;_T1B=l;
# 514
*_T1B=loc;}};}_T1C=c;
# 516
return _T1C;}
# 519
int Cyc_BansheeIf_add_constraint(unsigned loc,void*c){struct _fat_ptr _T0;void*_T1;int _T2;struct Cyc_String_pa_PrintArg_struct _T3;struct Cyc_String_pa_PrintArg_struct _T4;struct Cyc___cycFILE*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;int _T8;int _T9;struct Cyc___cycFILE*_TA;struct _fat_ptr _TB;struct _fat_ptr _TC;int _TD;_T0=
Cyc_Position_string_of_loc(loc);_T1=c;Cyc_BansheeIf_add_location(_T0,_T1);
Cyc_BansheeIf_insert_into_map(c);_T2=Cyc_Flags_verbose;
if(!_T2)goto _TL79;{struct Cyc_String_pa_PrintArg_struct _TE;_TE.tag=0;
_TE.f1=Cyc_Position_string_of_loc(loc);_T3=_TE;}{struct Cyc_String_pa_PrintArg_struct _TE=_T3;{struct Cyc_String_pa_PrintArg_struct _TF;_TF.tag=0;_TF.f1=Cyc_BansheeIf__constraint2string(c,0);_T4=_TF;}{struct Cyc_String_pa_PrintArg_struct _TF=_T4;void*_T10[2];_T10[0]=& _TE;_T10[1]=& _TF;_T5=Cyc_stderr;_T6=_tag_fat("-- (%s) Adding constraint : %s \t",sizeof(char),33U);_T7=_tag_fat(_T10,sizeof(void*),2);Cyc_fprintf(_T5,_T6,_T7);}}goto _TL7A;_TL79: _TL7A: _T8=
Cyc_BansheeIf_internal_add_constraint(c);{int ret=_T8!=0;_T9=Cyc_Flags_verbose;
if(!_T9)goto _TL7B;_TA=Cyc_stderr;_TB=
_tag_fat("\n",sizeof(char),2U);_TC=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TA,_TB,_TC);goto _TL7C;_TL7B: _TL7C: _TD=ret;
return _TD;}}
# 530
static int Cyc_BansheeIf_eval_guard(void*g){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T9;void*_TA;int*_TB;int _TC;void*_TD;void*_TE;void*_TF;void*_T10;void*_T11;enum Cyc_BansheeIf_ConstraintOperator _T12;int _T13;int(*_T14)(struct _fat_ptr,struct _fat_ptr);void*(*_T15)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T16;struct _fat_ptr _T17;struct _tuple13 _T18;void*_T19;void*_T1A;int*_T1B;int _T1C;void*_T1D;void*_T1E;int*_T1F;int _T20;void*_T21;void*_T22;int _T23;void*_T24;void*_T25;void*_T26;int _T27;enum Cyc_BansheeIf_ConstraintOperator _T28;int _T29;enum Cyc_BansheeIf_ConstraintOperator _T2A;int _T2B;void*_T2C;enum Cyc_BansheeIf_ConstraintOperator _T2D;int _T2E;int _T2F;int _T30;int _T31;int _T32;int _T33;void*_T34;void*_T35;void*_T36;int _T37;int _T38;void*_T39;void*_T3A;enum Cyc_BansheeIf_ConstraintOperator _T3B;_T0=g;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 1: _T3=g;_T4=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T3;_T5=_T4->f2;_T6=(int*)_T5;_T7=*_T6;if(_T7!=3)goto _TL7E;_T8=g;_T9=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T8;_TA=_T9->f3;_TB=(int*)_TA;_TC=*_TB;if(_TC!=3)goto _TL80;_TD=g;{struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T3C=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_TD;_T3B=_T3C->f1;_TE=_T3C->f2;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T3D=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TE;_TF=_T3D->f4;_T3A=(void*)_TF;}_T10=_T3C->f3;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T3D=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T10;_T11=_T3D->f4;_T39=(void*)_T11;}}{enum Cyc_BansheeIf_ConstraintOperator op=_T3B;void*bv1=_T3A;void*bv2=_T39;_T12=op;_T13=(int)_T12;
# 533
if(_T13==3)goto _TL82;_T15=Cyc_Warn_impos;{
int(*_T3C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T15;_T14=_T3C;}_T16=_tag_fat("Unsupported comparison operator",sizeof(char),32U);_T17=_tag_fat(0U,sizeof(void*),0);_T14(_T16,_T17);goto _TL83;_TL82: _TL83:{struct _tuple13 _T3C;
_T3C.f0=bv1;_T3C.f1=bv2;_T18=_T3C;}{struct _tuple13 _T3C=_T18;struct ptrbnd_*_T3D;struct ptrbnd_*_T3E;_T19=_T3C.f0;if(_T19==0)goto _TL84;_T1A=_T3C.f0;_T1B=(int*)_T1A;_T1C=*_T1B;if(_T1C!=0)goto _TL86;_T1D=_T3C.f1;if(_T1D==0)goto _TL88;_T1E=_T3C.f1;_T1F=(int*)_T1E;_T20=*_T1F;if(_T20!=0)goto _TL8A;_T21=_T3C.f0;{struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T3F=(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_T21;_T3E=_T3F->f1;}_T22=_T3C.f1;{struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T3F=(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_T22;_T3D=_T3F->f1;}{struct ptrbnd_*pb1=_T3E;struct ptrbnd_*pb2=_T3D;_T23=
# 537
ptrbnd_eq(pb1,pb2);return _T23;}_TL8A: goto _LLC;_TL88: goto _LLC;_TL86: goto _LLC;_TL84: _LLC:
# 539
 return 0;;}}goto _TL81;_TL80: goto _LL7;_TL81: goto _TL7F;_TL7E: goto _LL7;_TL7F:;case 5: _T24=g;{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T3C=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T24;_T3B=_T3C->f1;_T25=_T3C->f2;_T3A=(void*)_T25;_T26=_T3C->f3;_T39=(void*)_T26;}{enum Cyc_BansheeIf_ConstraintOperator op=_T3B;void*c1=_T3A;void*c2=_T39;
# 542
int c1g=Cyc_BansheeIf_eval_guard(c1);_T28=op;_T29=(int)_T28;
if(_T29==0)goto _TL8C;_T2A=op;_T2B=(int)_T2A;if(_T2B==1)goto _TL8C;_T27=0;goto _TL8D;_TL8C: _T2C=_check_null(c2);_T27=Cyc_BansheeIf_eval_guard(_T2C);_TL8D: {int c2g=_T27;_T2D=op;_T2E=(int)_T2D;switch(_T2E){case Cyc_BansheeIf_AndOp: _T30=c1g;
# 545
if(!_T30)goto _TL8F;_T2F=c2g;goto _TL90;_TL8F: _T2F=0;_TL90: return _T2F;case Cyc_BansheeIf_OrOp: _T32=c1g;
if(!_T32)goto _TL91;_T31=1;goto _TL92;_TL91: _T31=c2g;_TL92: return _T31;case Cyc_BansheeIf_NegOp: _T33=!c1g;
return _T33;default:
 return 0;};}}case 4: _T34=g;{struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T3C=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T34;_T35=_T3C->f1;_T3A=(void*)_T35;_T36=_T3C->f2;_T39=(void*)_T36;}{void*c1=_T3A;void*c2=_T39;_T37=
# 551
Cyc_BansheeIf_eval_guard(c1);if(_T37)goto _TL93;else{goto _TL95;}
_TL95: return 1;_TL93: _T38=
Cyc_BansheeIf_eval_guard(c2);return _T38;}default: _LL7:
# 555
 return 0;};}
# 559
int Cyc_BansheeIf_discharge_implications (void){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;int*_T3;unsigned _T4;void*_T5;void*_T6;int _T7;int _T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;unsigned _TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_List_List*_T16;int(*_T17)(struct _fat_ptr,struct _fat_ptr);void*(*_T18)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T19;struct _fat_ptr _T1A;int _T1B;struct Cyc_List_List*_T1C;unsigned _T1D;struct Cyc_List_List*_T1E;int*_T1F;int _T20;void*_T21;int _T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;struct Cyc_List_List*_T25;void*_T26;struct _fat_ptr _T27;struct Cyc_List_List*_T28;
int changed=0;
# 562
_TL96:{struct Cyc_List_List*prev=0;
struct Cyc_List_List*l=Cyc_BansheeIf_delayed_constraints;
int first=1;
changed=0;
_TL98: _T0=l;_T1=(unsigned)_T0;if(_T1)goto _TL99;else{goto _TL9A;}
_TL99: _T2=l;{void*_T29=_T2->hd;void*_T2A;void*_T2B;_T3=(int*)_T29;_T4=*_T3;switch(_T4){case 4:{struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T2C=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T29;_T5=_T2C->f1;_T2B=(void*)_T5;_T6=_T2C->f2;_T2A=(void*)_T6;}{void*c1=_T2B;void*c2=_T2A;_T7=
# 569
Cyc_BansheeIf_eval_guard(c1);if(!_T7)goto _TL9C;{
int result=Cyc_BansheeIf_internal_add_constraint(c2);
if(result!=0)goto _TL9E;return 0;_TL9E:
 if(result!=1)goto _TLA0;changed=1;goto _TLA1;_TLA0: _TLA1: _T8=first;
# 574
if(!_T8)goto _TLA2;
if(result!=2)goto _TLA4;_T9=Cyc_BansheeIf_delayed_constraints;_TA=
_check_null(Cyc_BansheeIf_delayed_constraints);_TB=_TA->tl;_TC=_check_null(_TB);_T9->tl=_TC->tl;
first=0;goto _TLA5;
# 580
_TLA4: _TD=_check_null(Cyc_BansheeIf_delayed_constraints);Cyc_BansheeIf_delayed_constraints=_TD->tl;_TLA5: goto _TLA3;
# 582
_TLA2: _TE=prev;_TF=(unsigned)_TE;if(!_TF)goto _TLA6;_T10=prev;_T11=l;
_T10->tl=_T11->tl;goto _TLA7;_TLA6: _TLA7: _TLA3: {
struct Cyc_List_List*tmp=l;_T12=l;
l=_T12->tl;_T13=tmp;
_T13->tl=0;}}goto _TL9D;
# 589
_TL9C: prev=l;_T14=l;
l=_T14->tl;
first=0;_TL9D: goto _LL0;}case 0:{struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T2C=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T29;_T15=_T2C->f1;_T2B=(void*)_T15;}{void*c1=_T2B;
# 596
first=0;
prev=l;_T16=l;
l=_T16->tl;goto _LL0;}default: _T18=Cyc_Warn_impos;{
# 609 "bansheeif.cyc"
int(*_T2C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T18;_T17=_T2C;}_T19=_tag_fat("Malformed delayed constraint; only implications permitted",sizeof(char),58U);_T1A=_tag_fat(0U,sizeof(void*),0);_T17(_T19,_T1A);}_LL0:;}goto _TL98;_TL9A:;}_T1B=changed;
# 561 "bansheeif.cyc"
if(_T1B)goto _TL96;else{goto _TL97;}_TL97:{
# 614 "bansheeif.cyc"
struct Cyc_List_List*dc=Cyc_BansheeIf_delayed_constraints;_TLAB: _T1C=dc;_T1D=(unsigned)_T1C;if(_T1D)goto _TLA9;else{goto _TLAA;}
_TLA9: _T1E=dc;{void*_T29=_T1E->hd;void*_T2A;_T1F=(int*)_T29;_T20=*_T1F;if(_T20!=0)goto _TLAC;{struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T2B=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T29;_T21=_T2B->f1;_T2A=(void*)_T21;}{void*c1=_T2A;_T22=
# 617
Cyc_BansheeIf_eval_guard(c1);if(_T22)goto _TLAE;else{goto _TLB0;}
_TLB0:{struct Cyc_Warn_String_Warn_Warg_struct _T2B;_T2B.tag=0;_T2B.f1=_tag_fat("Constraint could not be proved: ",sizeof(char),33U);_T23=_T2B;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B=_T23;{struct Cyc_Warn_String_Warn_Warg_struct _T2C;_T2C.tag=0;_T25=dc;_T26=_T25->hd;_T2C.f1=Cyc_BansheeIf_constraint2string(_T26);_T24=_T2C;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C=_T24;void*_T2D[2];_T2D[0]=& _T2B;_T2D[1]=& _T2C;_T27=_tag_fat(_T2D,sizeof(void*),2);Cyc_Warn_err2(0U,_T27);}}goto _TLAF;_TLAE: _TLAF: goto _LL7;}_TLAC: goto _LL7;_LL7:;}_T28=dc;
# 614
dc=_T28->tl;goto _TLAB;_TLAA:;}
# 625
return 1;}
# 628
int Cyc_BansheeIf_resolve(void*cvar){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T5;void**_T6;void*_T7;void**_T8;void*_T9;unsigned _TA;void*_TB;int*_TC;unsigned _TD;void*_TE;void*_TF;unsigned _T10;void**_T11;void*_T12;struct list*_T13;unsigned _T14;struct Cyc_List_List*_T15;unsigned _T16;struct Cyc_List_List*_T17;void*_T18;struct _tuple12*_T19;struct effect_*_T1A;struct effect_*_T1B;int _T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;void*_T1F;struct _tuple12*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;unsigned _T23;void**_T24;int(*_T25)(struct _fat_ptr,struct _fat_ptr);void*(*_T26)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;struct Cyc_Warn_Typ_Warn_Warg_struct _T2A;int(*_T2B)(struct _fat_ptr);void*(*_T2C)(struct _fat_ptr);struct _fat_ptr _T2D;void*_T2E;void*_T2F;_T0=cvar;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TLB1;_T3=cvar;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T30=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T4=cvar;_T5=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T4;_T6=& _T5->f2;_T2F=(void**)_T6;_T7=_T30->f4;_T2E=(void*)_T7;}{void**t=(void**)_T2F;void*bv=_T2E;_T8=t;_T9=*_T8;_TA=(unsigned)_T9;
# 631
if(!_TA)goto _TLB3;
return 1;_TLB3:{struct effect_*_T30;struct ptrbnd_*_T31;if(bv==0)goto _TLB5;_TB=bv;_TC=(int*)_TB;_TD=*_TC;switch(_TD){case 0: _TE=bv;{struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_T32=(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_TE;_T31=_T32->f1;}{struct ptrbnd_*pb=_T31;
# 635
struct ptrbnd_*pb2=ptrbnd_ecr(pb);
void*tmp=Cyc_BansheeIf_match_ptrbnd_constant(pb2);_TF=tmp;_T10=(unsigned)_TF;
if(!_T10)goto _TLB8;_T11=t;
*_T11=tmp;
return 1;_TLB8:
# 641
 return 0;}case 1: _T12=bv;{struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*_T32=(struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*)_T12;_T30=_T32->f1;}{struct effect_*ef=_T30;
# 643
struct list*efl=effect_tlb(ef);
struct Cyc_List_List*set=0;
_TLBD: _T13=efl;_T14=(unsigned)_T13;if(_T14)goto _TLBB;else{goto _TLBC;}
_TLBB:{struct Cyc_List_List*l=Cyc_BansheeIf_effconsts;_TLC1: _T15=l;_T16=(unsigned)_T15;if(_T16)goto _TLBF;else{goto _TLC0;}
_TLBF: _T17=l;_T18=_T17->hd;_T19=(struct _tuple12*)_T18;_T1A=_T19->f1;_T1B=effect_list_head(efl);_T1C=effect_eq(_T1A,_T1B);if(!_T1C)goto _TLC2;{struct Cyc_List_List*_T32=_cycalloc(sizeof(struct Cyc_List_List));_T1E=l;_T1F=_T1E->hd;_T20=(struct _tuple12*)_T1F;
_T32->hd=_T20->f0;_T32->tl=set;_T1D=(struct Cyc_List_List*)_T32;}set=_T1D;goto _TLC3;_TLC2: _TLC3: _T21=l;
# 646
l=_T21->tl;goto _TLC1;_TLC0:;}
# 645
efl=effect_list_tail(efl);goto _TLBD;_TLBC: _T22=set;_T23=(unsigned)_T22;
# 652
if(!_T23)goto _TLC4;_T24=t;
*_T24=Cyc_Absyn_join_eff(set);goto _TLC5;
# 655
_TLC4: return 0;_TLC5:
 return 1;}default: goto _LLA;}goto _TLB6;_TLB5: _LLA: _T26=Cyc_Warn_impos;{
# 658
int(*_T32)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T26;_T25=_T32;}_T27=_tag_fat("Attempt to resolve a variable that was never added",sizeof(char),51U);_T28=_tag_fat(0U,sizeof(void*),0);_T25(_T27,_T28);_TLB6:;}goto _LL0;}_TLB1:{struct Cyc_Warn_String_Warn_Warg_struct _T30;_T30.tag=0;
# 662
_T30.f1=_tag_fat("Bad type to resolve; expect Cvar only",sizeof(char),38U);_T29=_T30;}{struct Cyc_Warn_String_Warn_Warg_struct _T30=_T29;{struct Cyc_Warn_Typ_Warn_Warg_struct _T31;_T31.tag=2;_T31.f1=cvar;_T2A=_T31;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T31=_T2A;void*_T32[2];_T32[0]=& _T30;_T32[1]=& _T31;_T2C=Cyc_Warn_impos2;{int(*_T33)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T2C;_T2B=_T33;}_T2D=_tag_fat(_T32,sizeof(void*),2);_T2B(_T2D);}}_LL0:;}
# 671
enum Cyc_BansheeIf_CvarRecordState{Cyc_BansheeIf_LOCAL =0U,Cyc_BansheeIf_PERSIST_INIT =1U,Cyc_BansheeIf_PERSIST_INPROCESS =2U,Cyc_BansheeIf_PERSIST_REINTRO =3U,Cyc_BansheeIf_PERSIST_DONE =4U};
# 679
static struct Cyc_Hashtable_Table*Cyc_BansheeIf_cvarmap;struct _tuple14{void*f0;struct Cyc_List_List*f1;enum Cyc_BansheeIf_CvarRecordState f2;};
static struct Cyc_List_List*Cyc_BansheeIf_lookup_cvar_constraints(void*t){struct Cyc_Hashtable_Table*_T0;unsigned _T1;struct _handler_cons*_T2;int*_T3;int _T4;struct _tuple14*(*_T5)(struct Cyc_Hashtable_Table*,void*);void*(*_T6)(struct Cyc_Hashtable_Table*,void*);struct _tuple14*_T7;void*_T8;struct Cyc_Core_Not_found_exn_struct*_T9;char*_TA;char*_TB;_T0=Cyc_BansheeIf_cvarmap;_T1=(unsigned)_T0;
if(!_T1)goto _TLC6;{struct _handler_cons _TC;_T2=& _TC;_push_handler(_T2);{int _TD=0;_T3=_TC.handler;_T4=setjmp(_T3);if(!_T4)goto _TLC8;_TD=1;goto _TLC9;_TLC8: _TLC9: if(_TD)goto _TLCA;else{goto _TLCC;}_TLCC: _T6=Cyc_Hashtable_lookup;{
# 683
struct _tuple14*(*_TE)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple14*(*)(struct Cyc_Hashtable_Table*,void*))_T6;_T5=_TE;}_T7=_T5(Cyc_BansheeIf_cvarmap,t);{struct Cyc_List_List*_TE=_T7->f1;_npop_handler(0);return _TE;}_pop_handler();goto _TLCB;_TLCA: _T8=Cyc_Core_get_exn_thrown();{void*_TE=(void*)_T8;void*_TF;_T9=(struct Cyc_Core_Not_found_exn_struct*)_TE;_TA=_T9->tag;_TB=Cyc_Core_Not_found;if(_TA!=_TB)goto _TLCD;
# 686
return 0;_TLCD: _TF=_TE;{void*exn=_TF;_rethrow(exn);};}_TLCB:;}}goto _TLC7;_TLC6: _TLC7:
# 689
 return 0;}
# 692
static int Cyc_BansheeIf_cvarcmp(void*c1,void*c2){struct _tuple13 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;struct _fat_ptr _T9;void*_TA;void*_TB;unsigned _TC;struct _fat_ptr _TD;void*_TE;void*_TF;unsigned _T10;int _T11;int(*_T12)(struct _fat_ptr,struct _fat_ptr);void*(*_T13)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T14;struct _fat_ptr _T15;{struct _tuple13 _T16;
_T16.f0=c1;_T16.f1=c2;_T0=_T16;}{struct _tuple13 _T16=_T0;const char*_T17;const char*_T18;_T1=_T16.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=3)goto _TLCF;_T4=_T16.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=3)goto _TLD1;_T7=_T16.f0;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T19=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T18=_T19->f5;}_T8=_T16.f1;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T19=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T8;_T17=_T19->f5;}{const char*n1=_T18;const char*n2=_T17;{const char*_T19=n1;_TA=(void*)_T19;_TB=(void*)_T19;_TC=_get_zero_arr_size_char(_TB,1U);_T9=_tag_fat(_TA,sizeof(char),_TC);}{const char*_T19=n2;_TE=(void*)_T19;_TF=(void*)_T19;_T10=_get_zero_arr_size_char(_TF,1U);_TD=_tag_fat(_TE,sizeof(char),_T10);}_T11=
# 695
Cyc_strcmp(_T9,_TD);return _T11;}_TLD1: goto _LL3;_TLCF: _LL3: _T13=Cyc_Warn_impos;{
# 697
int(*_T19)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T13;_T12=_T19;}_T14=_tag_fat("Expecting cvar types only",sizeof(char),26U);_T15=_tag_fat(0U,sizeof(void*),0);_T12(_T14,_T15);;}}
# 701
static int Cyc_BansheeIf_cvareq(void*c1,void*c2){struct _tuple13 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;void*_T9;void*_TA;struct _fat_ptr _TB;void*_TC;void*_TD;unsigned _TE;struct _fat_ptr _TF;void*_T10;void*_T11;unsigned _T12;int _T13;int _T14;int(*_T15)(struct _fat_ptr,struct _fat_ptr);void*(*_T16)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T17;struct _fat_ptr _T18;{struct _tuple13 _T19;
_T19.f0=c1;_T19.f1=c2;_T0=_T19;}{struct _tuple13 _T19=_T0;const char*_T1A;void*_T1B;const char*_T1C;void*_T1D;_T1=_T19.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=3)goto _TLD3;_T4=_T19.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=3)goto _TLD5;_T7=_T19.f0;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1E=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T8=_T1E->f4;_T1D=(void*)_T8;_T1C=_T1E->f5;}_T9=_T19.f1;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1E=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T9;_TA=_T1E->f4;_T1B=(void*)_TA;_T1A=_T1E->f5;}{void*b1=_T1D;const char*n1=_T1C;void*b2=_T1B;const char*n2=_T1A;{const char*_T1E=n1;_TC=(void*)_T1E;_TD=(void*)_T1E;_TE=_get_zero_arr_size_char(_TD,1U);_TB=_tag_fat(_TC,sizeof(char),_TE);}{const char*_T1E=n2;_T10=(void*)_T1E;_T11=(void*)_T1E;_T12=_get_zero_arr_size_char(_T11,1U);_TF=_tag_fat(_T10,sizeof(char),_T12);}_T13=
# 704
Cyc_strcmp(_TB,_TF);_T14=!_T13;return _T14;}_TLD5: goto _LL3;_TLD3: _LL3: _T16=Cyc_Warn_impos;{
# 706
int(*_T1E)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T16;_T15=_T1E;}_T17=_tag_fat("Expecting cvar types only",sizeof(char),26U);_T18=_tag_fat(0U,sizeof(void*),0);_T15(_T17,_T18);;}}
# 710
static int Cyc_BansheeIf_cvarhash(void*c){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct _fat_ptr _T5;void*_T6;void*_T7;unsigned _T8;int _T9;int(*_TA)(struct _fat_ptr,struct _fat_ptr);void*(*_TB)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TC;struct _fat_ptr _TD;const char*_TE;void*_TF;_T0=c;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TLD7;_T3=c;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T10=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T4=_T10->f4;_TF=(void*)_T4;_TE=_T10->f5;}{void*b1=_TF;const char*name=_TE;{const char*_T10=name;_T6=(void*)_T10;_T7=(void*)_T10;_T8=_get_zero_arr_size_char(_T7,1U);_T5=_tag_fat(_T6,sizeof(char),_T8);}_T9=
# 714
Cyc_Hashtable_hash_string(_T5);return _T9;}_TLD7: _TB=Cyc_Warn_impos;{
# 716
int(*_T10)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TB;_TA=_T10;}_TC=_tag_fat("Expecting cvar types only",sizeof(char),26U);_TD=_tag_fat(0U,sizeof(void*),0);_TA(_TC,_TD);;}
# 720
static int Cyc_BansheeIf_not_in_list(struct Cyc_List_List*l,void*e){void*_T0;struct Cyc_List_List*_T1;int _T2;int _T3;_T0=e;_T1=l;_T2=
Cyc_List_exists_c(Cyc_BansheeIf_cvareq,_T0,_T1);_T3=!_T2;return _T3;}
# 724
static struct Cyc_List_List*Cyc_BansheeIf_cvars_in_constraint(void*c){void*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;void*_T8;void*_T9;void*_TA;void*_TB;int _TC;struct Cyc_List_List*_TD;struct _fat_ptr _TE;int _TF;int _T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;void*_T13;void*_T14;struct Cyc_List_List*_T15;void*_T16;void*_T17;void*_T18;void*_T19;void*_T1A;void*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*(*_T1D)(int(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*(*_T1E)(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T1F;void*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;void*_T24;void*_T25;_T0=c;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 1: _T3=c;{struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T26=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T3;_T4=_T26->f2;_T25=(void*)_T4;_T5=_T26->f3;_T24=(void*)_T5;}{void*cv1=_T25;void*cv2=_T24;_T25=cv1;_T24=cv2;goto _LL4;}case 2: _T6=c;{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T26=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T6;_T7=_T26->f1;_T25=(void*)_T7;_T8=_T26->f2;_T24=(void*)_T8;}_LL4: {void*cv1=_T25;void*cv2=_T24;_T25=cv1;_T24=cv2;goto _LL6;}case 3: _T9=c;{struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*_T26=(struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_T9;_TA=_T26->f1;_T25=(void*)_TA;_TB=_T26->f2;_T24=(void*)_TB;}_LL6: {void*cv1=_T25;void*cv2=_T24;
# 729
struct Cyc_List_List*ret=0;_TC=
Cyc_BansheeIf_is_constant(cv1);if(_TC)goto _TLDA;else{goto _TLDC;}
_TLDC:{void*_T26[1];_T26[0]=cv1;_TE=_tag_fat(_T26,sizeof(void*),1);_TD=Cyc_List_list(_TE);}ret=_TD;goto _TLDB;_TLDA: _TLDB: _TF=
Cyc_BansheeIf_cvarcmp(cv1,cv2);if(!_TF)goto _TLDD;_T10=Cyc_BansheeIf_is_constant(cv2);if(_T10)goto _TLDD;else{goto _TLDF;}
_TLDF:{struct Cyc_List_List*_T26=_cycalloc(sizeof(struct Cyc_List_List));_T26->hd=cv2;_T26->tl=ret;_T11=(struct Cyc_List_List*)_T26;}ret=_T11;goto _TLDE;_TLDD: _TLDE: _T12=ret;
return _T12;}case 0: _T13=c;{struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T26=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T13;_T14=_T26->f1;_T25=(void*)_T14;}{void*c1=_T25;_T15=
Cyc_BansheeIf_cvars_in_constraint(c1);return _T15;}case 4: _T16=c;{struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T26=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T16;_T17=_T26->f1;_T25=(void*)_T17;_T18=_T26->f2;_T24=(void*)_T18;}{void*c1=_T25;void*c2=_T24;_T25=c1;_T24=c2;goto _LLC;}default: _T19=c;{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T26=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T19;_T1A=_T26->f2;_T25=(void*)_T1A;_T1B=_T26->f3;_T24=(void*)_T1B;}_LLC: {void*c1=_T25;void*c2=_T24;
# 738
struct Cyc_List_List*l1=Cyc_BansheeIf_cvars_in_constraint(c1);_T1C=l1;_T1E=Cyc_List_filter_c;{
struct Cyc_List_List*(*_T26)(int(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*))_T1E;_T1D=_T26;}_T1F=l1;_T20=_check_null(c2);_T21=Cyc_BansheeIf_cvars_in_constraint(_T20);_T22=_T1D(Cyc_BansheeIf_not_in_list,_T1F,_T21);_T23=Cyc_List_append(_T1C,_T22);return _T23;}};}
# 743
static void Cyc_BansheeIf_init_map (void){struct Cyc_Hashtable_Table*_T0;unsigned _T1;_T0=Cyc_BansheeIf_cvarmap;_T1=(unsigned)_T0;
if(_T1)goto _TLE0;else{goto _TLE2;}
_TLE2: Cyc_BansheeIf_cvarmap=Cyc_Hashtable_create(101,Cyc_BansheeIf_cvarcmp,Cyc_BansheeIf_cvarhash);goto _TLE1;_TLE0: _TLE1:;}
# 749
static int Cyc_BansheeIf_constraint_eq(void*c1,void*c2){struct _tuple13 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;void*_T9;void*_TA;void*_TB;void*_TC;int _TD;int _TE;void*_TF;int*_T10;int _T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;void*_T17;enum Cyc_BansheeIf_ConstraintOperator _T18;int _T19;enum Cyc_BansheeIf_ConstraintOperator _T1A;int _T1B;void*_T1C;int*_T1D;int _T1E;void*_T1F;void*_T20;void*_T21;void*_T22;void*_T23;void*_T24;int _T25;int _T26;void*_T27;int*_T28;int _T29;void*_T2A;void*_T2B;void*_T2C;void*_T2D;int _T2E;void*_T2F;int*_T30;int _T31;void*_T32;void*_T33;void*_T34;void*_T35;void*_T36;void*_T37;int _T38;int _T39;void*_T3A;int*_T3B;int _T3C;void*_T3D;void*_T3E;void*_T3F;void*_T40;void*_T41;void*_T42;int _T43;enum Cyc_BansheeIf_ConstraintOperator _T44;int _T45;enum Cyc_BansheeIf_ConstraintOperator _T46;int _T47;int _T48;int _T49;void*_T4A;unsigned _T4B;void*_T4C;void*_T4D;{struct _tuple13 _T4E;
_T4E.f0=c1;_T4E.f1=c2;_T0=_T4E;}{struct _tuple13 _T4E=_T0;enum Cyc_BansheeIf_ConstraintOperator _T4F;enum Cyc_BansheeIf_ConstraintOperator _T50;int _T51;void*_T52;void*_T53;int _T54;void*_T55;void*_T56;_T1=_T4E.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 2: _T4=_T4E.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=2)goto _TLE4;_T7=_T4E.f0;{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T7;_T8=_T57->f1;_T56=(void*)_T8;_T9=_T57->f2;_T55=(void*)_T9;_T54=_T57->f3;}_TA=_T4E.f1;{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_TA;_TB=_T57->f1;_T53=(void*)_TB;_TC=_T57->f2;_T52=(void*)_TC;_T51=_T57->f3;}{void*cv11=_T56;void*cv12=_T55;int f1=_T54;void*cv21=_T53;void*cv22=_T52;int f2=_T51;
# 752
if(f1!=f2)goto _TLE6;_TE=
Cyc_BansheeIf_cvareq(cv11,cv21);if(!_TE)goto _TLE8;_TD=Cyc_BansheeIf_cvareq(cv12,cv22);goto _TLE9;_TLE8: _TD=0;_TLE9: return _TD;_TLE6:
 return 0;}_TLE4: goto _LLD;case 1: _TF=_T4E.f1;_T10=(int*)_TF;_T11=*_T10;if(_T11!=1)goto _TLEA;_T12=_T4E.f0;{struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T12;_T50=_T57->f1;_T13=_T57->f2;_T56=(void*)_T13;_T14=_T57->f3;_T55=(void*)_T14;}_T15=_T4E.f1;{struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_T15;_T4F=_T57->f1;_T16=_T57->f2;_T53=(void*)_T16;_T17=_T57->f3;_T52=(void*)_T17;}{enum Cyc_BansheeIf_ConstraintOperator op1=_T50;void*cv11=_T56;void*cv12=_T55;enum Cyc_BansheeIf_ConstraintOperator op2=_T4F;void*cv21=_T53;void*cv22=_T52;_T18=op1;_T19=(int)_T18;_T1A=op2;_T1B=(int)_T1A;
# 756
if(_T19!=_T1B)goto _TLEC;_T56=cv11;_T55=cv12;_T53=cv21;_T52=cv22;goto _LL6;_TLEC:
# 758
 return 0;}_TLEA: goto _LLD;case 3: _T1C=_T4E.f1;_T1D=(int*)_T1C;_T1E=*_T1D;if(_T1E!=3)goto _TLEE;_T1F=_T4E.f0;{struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_T1F;_T20=_T57->f1;_T56=(void*)_T20;_T21=_T57->f2;_T55=(void*)_T21;}_T22=_T4E.f1;{struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_T22;_T23=_T57->f1;_T53=(void*)_T23;_T24=_T57->f2;_T52=(void*)_T24;}_LL6:{void*cv11=_T56;void*cv12=_T55;void*cv21=_T53;void*cv22=_T52;_T26=
# 760
Cyc_BansheeIf_cvareq(cv11,cv21);if(!_T26)goto _TLF0;_T25=Cyc_BansheeIf_cvareq(cv12,cv22);goto _TLF1;_TLF0: _T25=0;_TLF1: return _T25;}_TLEE: goto _LLD;case 0: _T27=_T4E.f1;_T28=(int*)_T27;_T29=*_T28;if(_T29!=0)goto _TLF2;_T2A=_T4E.f0;{struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T2A;_T2B=_T57->f1;_T56=(void*)_T2B;}_T2C=_T4E.f1;{struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_T2C;_T2D=_T57->f1;_T55=(void*)_T2D;}{void*c11=_T56;void*c21=_T55;_T2E=
# 762
Cyc_BansheeIf_constraint_eq(c11,c21);return _T2E;}_TLF2: goto _LLD;case 4: _T2F=_T4E.f1;_T30=(int*)_T2F;_T31=*_T30;if(_T31!=4)goto _TLF4;_T32=_T4E.f0;{struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T32;_T33=_T57->f1;_T56=(void*)_T33;_T34=_T57->f2;_T55=(void*)_T34;}_T35=_T4E.f1;{struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_T35;_T36=_T57->f1;_T53=(void*)_T36;_T37=_T57->f2;_T52=(void*)_T37;}{void*c11=_T56;void*c12=_T55;void*c21=_T53;void*c22=_T52;_T39=
# 764
Cyc_BansheeIf_constraint_eq(c11,c21);if(!_T39)goto _TLF6;_T38=Cyc_BansheeIf_constraint_eq(c12,c22);goto _TLF7;_TLF6: _T38=0;_TLF7: return _T38;}_TLF4: goto _LLD;default: _T3A=_T4E.f1;_T3B=(int*)_T3A;_T3C=*_T3B;if(_T3C!=5)goto _TLF8;_T3D=_T4E.f0;{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T3D;_T50=_T57->f1;_T3E=_T57->f2;_T56=(void*)_T3E;_T3F=_T57->f3;_T55=(void*)_T3F;}_T40=_T4E.f1;{struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_T57=(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_T40;_T4F=_T57->f1;_T41=_T57->f2;_T53=(void*)_T41;_T42=_T57->f3;_T52=(void*)_T42;}{enum Cyc_BansheeIf_ConstraintOperator op1=_T50;void*c11=_T56;void*c12=_T55;enum Cyc_BansheeIf_ConstraintOperator op2=_T4F;void*c21=_T53;void*c22=_T52;_T44=op1;_T45=(int)_T44;_T46=op2;_T47=(int)_T46;
# 766
if(_T45!=_T47)goto _TLFA;_T48=Cyc_BansheeIf_constraint_eq(c11,c21);if(!_T48)goto _TLFA;_T4A=c12;_T4B=(unsigned)_T4A;if(!_T4B)goto _TLFC;_T4C=c12;_T4D=_check_null(c22);_T49=Cyc_BansheeIf_constraint_eq(_T4C,_T4D);goto _TLFD;_TLFC: _T49=c22==0;_TLFD: _T43=_T49;goto _TLFB;_TLFA: _T43=0;_TLFB: return _T43;}_TLF8: _LLD:
# 768
 return 0;};}}
# 772
static int Cyc_BansheeIf_constraint_cmp(void*c1,void*c2){int _T0;int _T1;_T0=
Cyc_BansheeIf_constraint_eq(c1,c2);_T1=!_T0;return _T1;}
# 775
static int Cyc_BansheeIf_hash_constraint(void*c){struct _fat_ptr _T0;int _T1;_T0=
Cyc_BansheeIf_constraint2string(c);_T1=Cyc_Hashtable_hash_string(_T0);return _T1;}
# 779
static int Cyc_BansheeIf_insert_into_map(void*c){struct Cyc_List_List*_T0;unsigned _T1;struct _handler_cons*_T2;int*_T3;int _T4;struct _tuple14*(*_T5)(struct Cyc_Hashtable_Table*,void*);void*(*_T6)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_List_List**_TA;void*_TB;struct Cyc_List_List**_TC;struct Cyc_List_List*_TD;int _TE;struct Cyc_List_List**_TF;struct Cyc_List_List*_T10;struct Cyc_List_List**_T11;void*_T12;struct Cyc_Core_Not_found_exn_struct*_T13;char*_T14;char*_T15;struct Cyc_List_List*_T16;int*_T17;int _T18;int(*_T19)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T1A)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T1B;struct _fat_ptr _T1C;int(*_T1D)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T1E)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T1F;struct _fat_ptr _T20;void(*_T21)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*);void(*_T22)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T23;struct Cyc_List_List*_T24;void*_T25;struct _tuple14*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;int _T2A;
Cyc_BansheeIf_init_map();{
int added=0;{
struct Cyc_List_List*cvars=Cyc_BansheeIf_cvars_in_constraint(c);_TL101: _T0=cvars;_T1=(unsigned)_T0;if(_T1)goto _TLFF;else{goto _TL100;}
_TLFF:{struct _handler_cons _T2B;_T2=& _T2B;_push_handler(_T2);{int _T2C=0;_T3=_T2B.handler;_T4=setjmp(_T3);if(!_T4)goto _TL102;_T2C=1;goto _TL103;_TL102: _TL103: if(_T2C)goto _TL104;else{goto _TL106;}_TL106: _T6=Cyc_Hashtable_lookup;{
struct _tuple14*(*_T2D)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple14*(*)(struct Cyc_Hashtable_Table*,void*))_T6;_T5=_T2D;}_T7=_check_null(Cyc_BansheeIf_cvarmap);_T8=cvars;_T9=_T8->hd;{struct _tuple14*_T2D=_T5(_T7,_T9);void*_T2E;{struct _tuple14 _T2F=*_T2D;_TA=& _T2D->f1;_T2E=(struct Cyc_List_List**)_TA;}{struct Cyc_List_List**oc=(struct Cyc_List_List**)_T2E;_TB=c;_TC=oc;_TD=*_TC;_TE=
Cyc_List_exists_c(Cyc_BansheeIf_constraint_eq,_TB,_TD);if(_TE)goto _TL107;else{goto _TL109;}
_TL109: _TF=oc;{struct Cyc_List_List*_T2F=_cycalloc(sizeof(struct Cyc_List_List));_T2F->hd=c;_T11=oc;_T2F->tl=*_T11;_T10=(struct Cyc_List_List*)_T2F;}*_TF=_T10;
added=added + 1;goto _TL108;_TL107: _TL108:;}}_pop_handler();goto _TL105;_TL104: _T12=Cyc_Core_get_exn_thrown();{void*_T2D=(void*)_T12;void*_T2E;_T13=(struct Cyc_Core_Not_found_exn_struct*)_T2D;_T14=_T13->tag;_T15=Cyc_Core_Not_found;if(_T14!=_T15)goto _TL10A;_T16=cvars;{
# 791
void*_T2F=_T16->hd;int _T30;_T17=(int*)_T2F;_T18=*_T17;if(_T18!=3)goto _TL10C;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T31=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T2F;_T30=_T31->f3;}{int id=_T30;
# 793
if(id!=0)goto _TL10E;goto _TL10F;_TL10E: _T1A=Cyc___assert_fail;{int(*_T31)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T1A;_T19=_T31;}_T1B=_tag_fat("id == 0",sizeof(char),8U);_T1C=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T19(_T1B,_T1C,793U);_TL10F: goto _LL8;}_TL10C:
# 796
 if(!0)goto _TL110;goto _TL111;_TL110: _T1E=Cyc___assert_fail;{int(*_T31)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T1E;_T1D=_T31;}_T1F=_tag_fat("0",sizeof(char),2U);_T20=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T1D(_T1F,_T20,796U);_TL111: goto _LL8;_LL8:;}_T22=Cyc_Hashtable_insert;{
# 799
void(*_T2F)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*))_T22;_T21=_T2F;}_T23=_check_null(Cyc_BansheeIf_cvarmap);_T24=cvars;_T25=_T24->hd;{struct _tuple14*_T2F=_cycalloc(sizeof(struct _tuple14));_T27=cvars;_T2F->f0=_T27->hd;{struct Cyc_List_List*_T30=_cycalloc(sizeof(struct Cyc_List_List));_T30->hd=c;_T30->tl=0;_T28=(struct Cyc_List_List*)_T30;}_T2F->f1=_T28;_T2F->f2=0U;_T26=(struct _tuple14*)_T2F;}_T21(_T23,_T25,_T26);goto _LL3;_TL10A: _T2E=_T2D;{void*exn=_T2E;_rethrow(exn);}_LL3:;}_TL105:;}}_T29=cvars;
# 782
cvars=_T29->tl;goto _TL101;_TL100:;}_T2A=added;
# 803
return _T2A;}}
# 807
static void*Cyc_BansheeIf_eval_equality_constraint(void*cvar,struct Cyc_List_List*lc){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;unsigned _T4;struct Cyc_List_List*_T5;int*_T6;int _T7;struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T8;int _T9;void*_TA;void*_TB;int _TC;void*_TD;int _TE;void*_TF;_T0=lc;_T1=(unsigned)_T0;
if(!_T1)goto _TL112;_T2=lc;_T3=_T2->tl;_T4=(unsigned)_T3;if(_T4)goto _TL112;else{goto _TL114;}
_TL114: _T5=lc;{void*_T10=_T5->hd;void*_T11;void*_T12;_T6=(int*)_T10;_T7=*_T6;if(_T7!=2)goto _TL115;_T8=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T10;_T9=_T8->f3;if(_T9!=0)goto _TL117;{struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_T13=(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_T10;_TA=_T13->f1;_T12=(void*)_TA;_TB=_T13->f2;_T11=(void*)_TB;}{void*c1=_T12;void*c2=_T11;_TC=
# 811
Cyc_BansheeIf_cvarcmp(cvar,c1);if(_TC)goto _TL119;else{goto _TL11B;}
_TL11B: _TD=Cyc_Absyn_compress(c2);return _TD;_TL119: _TE=
Cyc_BansheeIf_cvarcmp(cvar,c2);if(_TE)goto _TL11C;else{goto _TL11E;}
_TL11E: _TF=Cyc_Absyn_compress(c1);return _TF;_TL11C: goto _LL4;}_TL117: goto _LL3;_TL115: _LL3: _LL4:
# 817
 return 0;;}goto _TL113;_TL112: _TL113:
# 820
 return 0;}
# 824
static void Cyc_BansheeIf_reintroduce_cvar(void*_cvar){struct _handler_cons*_T0;int*_T1;int _T2;struct _tuple14*(*_T3)(struct Cyc_Hashtable_Table*,void*);void*(*_T4)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_T5;void*_T6;void*_T7;struct Cyc_Core_Not_found_exn_struct*_T8;char*_T9;char*_TA;struct Cyc_Warn_String_Warn_Warg_struct _TB;struct Cyc_Warn_Typ_Warn_Warg_struct _TC;int(*_TD)(struct _fat_ptr);void*(*_TE)(struct _fat_ptr);struct _fat_ptr _TF;struct _tuple14*_T10;struct _tuple14*_T11;enum Cyc_BansheeIf_CvarRecordState*_T12;enum Cyc_BansheeIf_CvarRecordState*_T13;enum Cyc_BansheeIf_CvarRecordState _T14;int _T15;enum Cyc_BansheeIf_CvarRecordState*_T16;void*_T17;int*_T18;int _T19;void*_T1A;void*_T1B;void*_T1C;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1D;void**_T1E;void**_T1F;void*_T20;unsigned _T21;int(*_T22)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T23)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_List_List*_T26;unsigned _T27;struct Cyc_List_List*_T28;void*_T29;int _T2A;int(*_T2B)(struct _fat_ptr,struct _fat_ptr);void*(*_T2C)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2D;struct _fat_ptr _T2E;int _T2F;struct Cyc_String_pa_PrintArg_struct _T30;struct Cyc_List_List*_T31;void*_T32;struct Cyc___cycFILE*_T33;struct _fat_ptr _T34;struct _fat_ptr _T35;struct Cyc_List_List*_T36;void*_T37;struct Cyc_List_List*_T38;struct Cyc_List_List*_T39;enum Cyc_BansheeIf_CvarRecordState*_T3A;int(*_T3B)(struct _fat_ptr,struct _fat_ptr);void*(*_T3C)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T3D;struct _fat_ptr _T3E;
struct _tuple14*val;{struct _handler_cons _T3F;_T0=& _T3F;_push_handler(_T0);{int _T40=0;_T1=_T3F.handler;_T2=setjmp(_T1);if(!_T2)goto _TL11F;_T40=1;goto _TL120;_TL11F: _TL120: if(_T40)goto _TL121;else{goto _TL123;}_TL123: _T4=Cyc_Hashtable_lookup;{
# 827
struct _tuple14*(*_T41)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple14*(*)(struct Cyc_Hashtable_Table*,void*))_T4;_T3=_T41;}_T5=_check_null(Cyc_BansheeIf_cvarmap);_T6=_cvar;val=_T3(_T5,_T6);_pop_handler();goto _TL122;_TL121: _T7=Cyc_Core_get_exn_thrown();{void*_T41=(void*)_T7;void*_T42;_T8=(struct Cyc_Core_Not_found_exn_struct*)_T41;_T9=_T8->tag;_TA=Cyc_Core_Not_found;if(_T9!=_TA)goto _TL124;{struct Cyc_Warn_String_Warn_Warg_struct _T43;_T43.tag=0;
# 830
_T43.f1=_tag_fat("Constraint graph refers to an undefined constraint variable",sizeof(char),60U);_TB=_T43;}{struct Cyc_Warn_String_Warn_Warg_struct _T43=_TB;{struct Cyc_Warn_Typ_Warn_Warg_struct _T44;_T44.tag=2;_T44.f1=_cvar;_TC=_T44;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T44=_TC;void*_T45[2];_T45[0]=& _T43;_T45[1]=& _T44;_TE=Cyc_Warn_impos2;{int(*_T46)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TE;_TD=_T46;}_TF=_tag_fat(_T45,sizeof(void*),2);_TD(_TF);}}goto _TL125;_TL124: _T42=_T41;{void*exn=_T42;_rethrow(exn);}_TL125:;}_TL122:;}}{void*_T3F;struct Cyc_List_List*_T40;void*_T41;_T10=val;{struct _tuple14 _T42=*_T10;_T41=_T42.f0;_T40=_T42.f1;_T11=val;_T12=& _T11->f2;_T3F=(enum Cyc_BansheeIf_CvarRecordState*)_T12;}{void*cv=_T41;struct Cyc_List_List*cnstr=_T40;enum Cyc_BansheeIf_CvarRecordState*state=(enum Cyc_BansheeIf_CvarRecordState*)_T3F;_T13=state;_T14=*_T13;_T15=(int)_T14;
# 835
if(_T15==1)goto _TL126;return;_TL126: _T16=state;
*_T16=2U;{const char*_T42;const char*_T43;void*_T44;int _T45;void*_T46;struct Cyc_Core_Opt*_T47;_T17=cv;_T18=(int*)_T17;_T19=*_T18;if(_T19!=3)goto _TL128;_T1A=cv;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T48=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T1A;_T47=_T48->f1;_T1B=_T48->f2;_T46=(void*)_T1B;_T45=_T48->f3;_T1C=cv;_T1D=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T1C;_T1E=& _T1D->f4;_T44=(void**)_T1E;_T43=_T48->f5;_T42=_T48->f6;}{struct Cyc_Core_Opt*ok=_T47;void*topt=_T46;int id=_T45;void**bv=(void**)_T44;const char*name=_T43;const char*pos=_T42;_T1F=bv;_T20=*_T1F;_T21=(unsigned)_T20;
# 839
if(_T21)goto _TL12A;else{goto _TL12C;}
_TL12C: if(cnstr!=0)goto _TL12D;goto _TL12E;_TL12D: _T23=Cyc___assert_fail;{int(*_T48)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T23;_T22=_T48;}_T24=_tag_fat("cnstr==0",sizeof(char),9U);_T25=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T22(_T24,_T25,840U);_TL12E:
 Cyc_BansheeIf_add_variable(cv);goto _TL12B;_TL12A: _TL12B:
# 845
 _TL132: _T26=cnstr;_T27=(unsigned)_T26;if(_T27)goto _TL130;else{goto _TL131;}
_TL130: _T28=cnstr;_T29=_T28->hd;_T2A=Cyc_BansheeIf_internal_add_constraint(_T29);if(_T2A!=0)goto _TL133;_T2C=Cyc_Warn_impos;{
int(*_T48)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2C;_T2B=_T48;}_T2D=_tag_fat("Failed to reintroduce a constraint",sizeof(char),35U);_T2E=_tag_fat(0U,sizeof(void*),0);_T2B(_T2D,_T2E);goto _TL134;_TL133: _TL134: _T2F=Cyc_Flags_verbose;
if(!_T2F)goto _TL135;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;_T31=cnstr;_T32=_T31->hd;
_T48.f1=Cyc_BansheeIf__constraint2string(_T32,0);_T30=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T30;void*_T49[1];_T49[0]=& _T48;_T33=Cyc_stderr;_T34=_tag_fat("-- Reintro'd constraint : %s \n",sizeof(char),31U);_T35=_tag_fat(_T49,sizeof(void*),1);Cyc_fprintf(_T33,_T34,_T35);}goto _TL136;_TL135: _TL136: _T36=cnstr;_T37=_T36->hd;{
# 851
struct Cyc_List_List*cvars=Cyc_BansheeIf_cvars_in_constraint(_T37);_T38=cvars;
Cyc_List_iter(Cyc_BansheeIf_reintroduce_cvar,_T38);}_T39=cnstr;
# 845
cnstr=_T39->tl;goto _TL132;_TL131: _T3A=state;
# 854
*_T3A=3U;goto _LL8;}_TL128: _T3C=Cyc_Warn_impos;{
# 857
int(*_T48)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T3C;_T3B=_T48;}_T3D=_tag_fat("Unexpected type",sizeof(char),16U);_T3E=_tag_fat(0U,sizeof(void*),0);_T3B(_T3D,_T3E);_LL8:;}
# 859
return;}}}
# 862
static void Cyc_BansheeIf_copy_cvar_fields(void*src,void*dest){struct _tuple13 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;void*_T9;void*_TA;void*_TB;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_TC;struct Cyc_Core_Opt**_TD;void*_TE;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_TF;void**_T10;void*_T11;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T12;int*_T13;void*_T14;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T15;void**_T16;void*_T17;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T18;const char**_T19;void*_T1A;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1B;const char**_T1C;void*_T1D;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T1E;int*_T1F;struct Cyc_Core_Opt**_T20;void**_T21;int*_T22;void**_T23;const char**_T24;const char**_T25;int*_T26;{struct _tuple13 _T27;
_T27.f0=src;_T27.f1=dest;_T0=_T27;}{struct _tuple13 _T27=_T0;void*_T28;void*_T29;void*_T2A;void*_T2B;void*_T2C;void*_T2D;void*_T2E;int _T2F;const char*_T30;const char*_T31;void*_T32;int _T33;void*_T34;struct Cyc_Core_Opt*_T35;_T1=_T27.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=3)goto _TL137;_T4=_T27.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=3)goto _TL139;_T7=_T27.f0;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T36=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T35=_T36->f1;_T8=_T36->f2;_T34=(void*)_T8;_T33=_T36->f3;_T9=_T36->f4;_T32=(void*)_T9;_T31=_T36->f5;_T30=_T36->f6;_T2F=_T36->f7;}_TA=_T27.f1;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T36=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TA;_TB=_T27.f1;_TC=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TB;_TD=& _TC->f1;_T2E=(struct Cyc_Core_Opt**)_TD;_TE=_T27.f1;_TF=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TE;_T10=& _TF->f2;_T2D=(void**)_T10;_T11=_T27.f1;_T12=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T11;_T13=& _T12->f3;_T2C=(int*)_T13;_T14=_T27.f1;_T15=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T14;_T16=& _T15->f4;_T2B=(void**)_T16;_T17=_T27.f1;_T18=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T17;_T19=& _T18->f5;_T2A=(const char**)_T19;_T1A=_T27.f1;_T1B=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T1A;_T1C=& _T1B->f6;_T29=(const char**)_T1C;_T1D=_T27.f1;_T1E=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T1D;_T1F=& _T1E->f7;_T28=(int*)_T1F;}{struct Cyc_Core_Opt*ok=_T35;void*topt=_T34;int id=_T33;void*bv=_T32;const char*name=_T31;const char*pos=_T30;int of=_T2F;struct Cyc_Core_Opt**okd=(struct Cyc_Core_Opt**)_T2E;void**toptd=(void**)_T2D;int*idd=(int*)_T2C;void**bvd=(void**)_T2B;const char**named=(const char**)_T2A;const char**posd=(const char**)_T29;int*ofd=(int*)_T28;_T20=okd;
# 865
*_T20=ok;_T21=toptd;*_T21=topt;_T22=idd;*_T22=id;_T23=bvd;*_T23=bv;_T24=named;*_T24=name;_T25=posd;*_T25=pos;_T26=ofd;*_T26=of;goto _LL0;}_TL139: goto _LL3;_TL137: _LL3: goto _LL0;_LL0:;}}struct _tuple15{struct Cyc___cycFILE*f0;int f1;};
# 872
static void Cyc_BansheeIf_persist_cvar_entry(void*ig,struct _tuple14*entry,struct _tuple15*env){struct _tuple15*_T0;struct _tuple15*_T1;int*_T2;struct _tuple14*_T3;struct _tuple14*_T4;enum Cyc_BansheeIf_CvarRecordState*_T5;enum Cyc_BansheeIf_CvarRecordState*_T6;enum Cyc_BansheeIf_CvarRecordState _T7;int _T8;int(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_TA)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TB;struct _fat_ptr _TC;enum Cyc_BansheeIf_CvarRecordState*_TD;enum Cyc_BansheeIf_CvarRecordState _TE;int _TF;enum Cyc_BansheeIf_CvarRecordState*_T10;enum Cyc_BansheeIf_CvarRecordState _T11;int _T12;struct Cyc_String_pa_PrintArg_struct _T13;struct Cyc_String_pa_PrintArg_struct _T14;struct _fat_ptr _T15;void*_T16;void*_T17;unsigned _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct _fat_ptr _T1A;void*_T1B;void*_T1C;unsigned _T1D;struct Cyc___cycFILE*_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct Cyc_List_List*_T21;unsigned _T22;struct Cyc_String_pa_PrintArg_struct _T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc___cycFILE*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;unsigned _T2B;struct Cyc___cycFILE*_T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;struct Cyc_List_List*_T2F;void*_T30;struct Cyc_List_List*_T31;unsigned _T32;struct _handler_cons*_T33;int*_T34;int _T35;struct _tuple14*(*_T36)(struct Cyc_Hashtable_Table*,void*);void*(*_T37)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_T38;struct Cyc_List_List*_T39;void*_T3A;enum Cyc_BansheeIf_CvarRecordState*_T3B;enum Cyc_BansheeIf_CvarRecordState*_T3C;enum Cyc_BansheeIf_CvarRecordState _T3D;int _T3E;enum Cyc_BansheeIf_CvarRecordState*_T3F;int*_T40;void*_T41;struct Cyc_Core_Not_found_exn_struct*_T42;char*_T43;char*_T44;int(*_T45)(struct _fat_ptr,struct _fat_ptr);void*(*_T46)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T47;struct _fat_ptr _T48;struct Cyc_List_List*_T49;struct Cyc_List_List*_T4A;struct Cyc___cycFILE*_T4B;struct _fat_ptr _T4C;struct _fat_ptr _T4D;enum Cyc_BansheeIf_CvarRecordState*_T4E;void*_T4F;struct Cyc___cycFILE*_T50;_T0=env;{struct _tuple15 _T51=*_T0;_T50=_T51.f0;_T1=env;_T2=& _T1->f1;_T4F=(int*)_T2;}{struct Cyc___cycFILE*f=_T50;int*changed=(int*)_T4F;void*_T51;struct Cyc_List_List*_T52;void*_T53;_T3=entry;{struct _tuple14 _T54=*_T3;_T53=_T54.f0;_T52=_T54.f1;_T4=entry;_T5=& _T4->f2;_T51=(enum Cyc_BansheeIf_CvarRecordState*)_T5;}{void*cv=_T53;struct Cyc_List_List*cs=_T52;enum Cyc_BansheeIf_CvarRecordState*state=(enum Cyc_BansheeIf_CvarRecordState*)_T51;_T6=state;_T7=*_T6;_T8=(int)_T7;
# 875
if(_T8==2)goto _TL13B;goto _TL13C;_TL13B: _TA=Cyc___assert_fail;{int(*_T54)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_TA;_T9=_T54;}_TB=_tag_fat("*state != PERSIST_INPROCESS",sizeof(char),28U);_TC=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T9(_TB,_TC,875U);_TL13C: _TD=state;_TE=*_TD;_TF=(int)_TE;
if(_TF==0)goto _TL13F;else{goto _TL140;}_TL140: _T10=state;_T11=*_T10;_T12=(int)_T11;if(_T12==4)goto _TL13F;else{goto _TL13D;}
_TL13F: return;_TL13D:{struct Cyc_String_pa_PrintArg_struct _T54;_T54.tag=0;
# 879
_T54.f1=Cyc_BansheeIf_cvar2string(cv,0);_T13=_T54;}{struct Cyc_String_pa_PrintArg_struct _T54=_T13;{struct Cyc_String_pa_PrintArg_struct _T55;_T55.tag=0;{const char*_T56=Cyc_BansheeIf_cvarloc(cv);_T16=(void*)_T56;_T17=(void*)_T56;_T18=_get_zero_arr_size_char(_T17,1U);_T15=_tag_fat(_T16,sizeof(char),_T18);}_T55.f1=_T15;_T14=_T55;}{struct Cyc_String_pa_PrintArg_struct _T55=_T14;{struct Cyc_String_pa_PrintArg_struct _T56;_T56.tag=0;{const char*_T57=Cyc_BansheeIf_cvarorigfat(cv);_T1B=(void*)_T57;_T1C=(void*)_T57;_T1D=_get_zero_arr_size_char(_T1C,1U);_T1A=_tag_fat(_T1B,sizeof(char),_T1D);}_T56.f1=_T1A;_T19=_T56;}{struct Cyc_String_pa_PrintArg_struct _T56=_T19;void*_T57[3];_T57[0]=& _T54;_T57[1]=& _T55;_T57[2]=& _T56;_T1E=f;_T1F=_tag_fat("%s \"%s\", \"%s\" (",sizeof(char),16U);_T20=_tag_fat(_T57,sizeof(void*),3);Cyc_fprintf(_T1E,_T1F,_T20);}}}
# 881
_TL144: _T21=cs;_T22=(unsigned)_T21;if(_T22)goto _TL142;else{goto _TL143;}
# 883
_TL142:{struct Cyc_String_pa_PrintArg_struct _T54;_T54.tag=0;_T24=cs;_T25=_T24->hd;_T54.f1=Cyc_BansheeIf__constraint2string(_T25,0);_T23=_T54;}{struct Cyc_String_pa_PrintArg_struct _T54=_T23;void*_T55[1];_T55[0]=& _T54;_T26=f;_T27=_tag_fat("%s",sizeof(char),3U);_T28=_tag_fat(_T55,sizeof(void*),1);Cyc_fprintf(_T26,_T27,_T28);}_T29=cs;_T2A=_T29->tl;_T2B=(unsigned)_T2A;
if(!_T2B)goto _TL145;_T2C=f;_T2D=
_tag_fat(";",sizeof(char),2U);_T2E=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2C,_T2D,_T2E);goto _TL146;_TL145: _TL146: _T2F=cs;_T30=_T2F->hd;{
# 887
struct Cyc_List_List*cvars=Cyc_BansheeIf_cvars_in_constraint(_T30);
_TL14A: _T31=cvars;_T32=(unsigned)_T31;if(_T32)goto _TL148;else{goto _TL149;}
_TL148:{struct _handler_cons _T54;_T33=& _T54;_push_handler(_T33);{int _T55=0;_T34=_T54.handler;_T35=setjmp(_T34);if(!_T35)goto _TL14B;_T55=1;goto _TL14C;_TL14B: _TL14C: if(_T55)goto _TL14D;else{goto _TL14F;}_TL14F: _T37=Cyc_Hashtable_lookup;{
struct _tuple14*(*_T56)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple14*(*)(struct Cyc_Hashtable_Table*,void*))_T37;_T36=_T56;}_T38=_check_null(Cyc_BansheeIf_cvarmap);_T39=cvars;_T3A=_T39->hd;{struct _tuple14*_T56=_T36(_T38,_T3A);void*_T57;struct Cyc_List_List*_T58;void*_T59;{struct _tuple14 _T5A=*_T56;_T59=_T5A.f0;_T58=_T5A.f1;_T3B=& _T56->f2;_T57=(enum Cyc_BansheeIf_CvarRecordState*)_T3B;}{void*cv=_T59;struct Cyc_List_List*cs=_T58;enum Cyc_BansheeIf_CvarRecordState*state=(enum Cyc_BansheeIf_CvarRecordState*)_T57;_T3C=state;_T3D=*_T3C;_T3E=(int)_T3D;
if(_T3E!=0)goto _TL150;_T3F=state;
*_T3F=1U;_T40=changed;
*_T40=1;goto _TL151;_TL150: _TL151:;}}_pop_handler();goto _TL14E;_TL14D: _T41=Cyc_Core_get_exn_thrown();{void*_T56=(void*)_T41;void*_T57;_T42=(struct Cyc_Core_Not_found_exn_struct*)_T56;_T43=_T42->tag;_T44=Cyc_Core_Not_found;if(_T43!=_T44)goto _TL152;_T46=Cyc_Warn_impos;{
# 896
int(*_T58)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T46;_T45=_T58;}_T47=_tag_fat("Constraint refers to an undeclared Cvar",sizeof(char),40U);_T48=_tag_fat(0U,sizeof(void*),0);_T45(_T47,_T48);goto _TL153;_TL152: _T57=_T56;{void*exn=_T57;_rethrow(exn);}_TL153:;}_TL14E:;}}_T49=cvars;
# 888
cvars=_T49->tl;goto _TL14A;_TL149:;}_T4A=cs;
# 881
cs=_T4A->tl;goto _TL144;_TL143: _T4B=f;_T4C=
# 911 "bansheeif.cyc"
_tag_fat(")\n",sizeof(char),3U);_T4D=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4B,_T4C,_T4D);_T4E=state;
*_T4E=4U;}}}
# 915
static void Cyc_BansheeIf_constrain_locals(void*cvar,struct _tuple14*val){struct _tuple14*_T0;enum Cyc_BansheeIf_CvarRecordState _T1;int _T2;int _T3;void*_T4;void*_T5;void*_T6;int _T7;int(*_T8)(struct _fat_ptr,struct _fat_ptr);void*(*_T9)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TA;struct _fat_ptr _TB;_T0=val;_T1=_T0->f2;_T2=(int)_T1;
if(_T2==0)goto _TL154;
return;_TL154: _T3=
Cyc_Tcutil_is_cvar_type(cvar);if(!_T3)goto _TL156;_T4=cvar;_T5=
Cyc_Absyn_thinconst();_T6=Cyc_BansheeIf_equality_constraint(_T4,_T5);_T7=Cyc_BansheeIf_internal_add_constraint(_T6);if(_T7!=0)goto _TL158;_T9=Cyc_Warn_impos;{
int(*_TC)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T9;_T8=_TC;}_TA=_tag_fat("Failed to add a default constraint on a local constraint variable",sizeof(char),66U);_TB=_tag_fat(0U,sizeof(void*),0);_T8(_TA,_TB);goto _TL159;_TL158: _TL159: goto _TL157;_TL156: _TL157:;}
# 926
void Cyc_BansheeIf_persist_cvarmap(struct Cyc___cycFILE*f){int(*_T0)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T1)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _tuple15*_T7;struct _tuple15*_T8;struct _tuple15*_T9;void(*_TA)(void(*)(void*,struct _tuple14*,struct _tuple15*),struct Cyc_Hashtable_Table*,struct _tuple15*);void(*_TB)(void(*)(void*,void*,void*),struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TC;struct _tuple15*_TD;struct _tuple15*_TE;int _TF;void(*_T10)(void(*)(void*,struct _tuple14*),struct Cyc_Hashtable_Table*);void(*_T11)(void(*)(void*,void*),struct Cyc_Hashtable_Table*);struct Cyc_Hashtable_Table*_T12;
Cyc_BansheeIf_discharge_implications();
if(Cyc_BansheeIf_cvarmap==0)goto _TL15A;goto _TL15B;_TL15A: _T1=Cyc___assert_fail;{int(*_T13)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T1;_T0=_T13;}_T2=_tag_fat("cvarmap != 0",sizeof(char),13U);_T3=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T0(_T2,_T3,928U);_TL15B: _T4=f;_T5=
_tag_fat("# 1 \"cvars\"\n",sizeof(char),13U);_T6=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4,_T5,_T6);{
struct _tuple15*env;env=_cycalloc(sizeof(struct _tuple15));_T7=env;_T7->f0=f;_T8=env;_T8->f1=0;
# 932
_TL15C: _T9=env;_T9->f1=0;_TB=Cyc_Hashtable_iter_c;{
void(*_T13)(void(*)(void*,struct _tuple14*,struct _tuple15*),struct Cyc_Hashtable_Table*,struct _tuple15*)=(void(*)(void(*)(void*,struct _tuple14*,struct _tuple15*),struct Cyc_Hashtable_Table*,struct _tuple15*))_TB;_TA=_T13;}_TC=_check_null(Cyc_BansheeIf_cvarmap);_TD=env;_TA(Cyc_BansheeIf_persist_cvar_entry,_TC,_TD);_TE=env;_TF=_TE->f1;
# 931
if(_TF)goto _TL15C;else{goto _TL15D;}_TL15D: _T11=Cyc_Hashtable_iter;{
# 936
void(*_T13)(void(*)(void*,struct _tuple14*),struct Cyc_Hashtable_Table*)=(void(*)(void(*)(void*,struct _tuple14*),struct Cyc_Hashtable_Table*))_T11;_T10=_T13;}_T12=_check_null(Cyc_BansheeIf_cvarmap);_T10(Cyc_BansheeIf_constrain_locals,_T12);}}
# 939
static void Cyc_BansheeIf_remove_constraint(void*c){struct Cyc_List_List*_T0;unsigned _T1;struct _handler_cons*_T2;int*_T3;int _T4;struct _tuple14*(*_T5)(struct Cyc_Hashtable_Table*,void*);void*(*_T6)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_List_List**_TA;struct Cyc_List_List**_TB;struct Cyc_List_List**_TC;struct Cyc_List_List*_TD;void*_TE;void*_TF;struct Cyc_Core_Not_found_exn_struct*_T10;char*_T11;char*_T12;int _T13;int(*_T14)(struct _fat_ptr,struct _fat_ptr);void*(*_T15)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T16;struct _fat_ptr _T17;struct Cyc_List_List*_T18;
struct Cyc_List_List*cvars=Cyc_BansheeIf_cvars_in_constraint(c);
int tabOk=0;
_TL161: _T0=cvars;_T1=(unsigned)_T0;if(_T1)goto _TL15F;else{goto _TL160;}
_TL15F: tabOk=0;{struct _handler_cons _T19;_T2=& _T19;_push_handler(_T2);{int _T1A=0;_T3=_T19.handler;_T4=setjmp(_T3);if(!_T4)goto _TL162;_T1A=1;goto _TL163;_TL162: _TL163: if(_T1A)goto _TL164;else{goto _TL166;}_TL166: _T6=Cyc_Hashtable_lookup;{
# 945
struct _tuple14*(*_T1B)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple14*(*)(struct Cyc_Hashtable_Table*,void*))_T6;_T5=_T1B;}_T7=_check_null(Cyc_BansheeIf_cvarmap);_T8=cvars;_T9=_T8->hd;{struct _tuple14*_T1B=_T5(_T7,_T9);void*_T1C;{struct _tuple14 _T1D=*_T1B;_TA=& _T1B->f1;_T1C=(struct Cyc_List_List**)_TA;}{struct Cyc_List_List**constr=(struct Cyc_List_List**)_T1C;
tabOk=1;_TB=constr;_TC=constr;_TD=*_TC;_TE=c;
*_TB=Cyc_List_delete_cmp(Cyc_BansheeIf_constraint_cmp,_TD,_TE);}}_pop_handler();goto _TL165;_TL164: _TF=Cyc_Core_get_exn_thrown();{void*_T1B=(void*)_TF;void*_T1C;_T10=(struct Cyc_Core_Not_found_exn_struct*)_T1B;_T11=_T10->tag;_T12=Cyc_Core_Not_found;if(_T11!=_T12)goto _TL167;_T13=tabOk;
# 950
if(_T13)goto _TL169;else{goto _TL16B;}
_TL16B: _T15=Cyc_Warn_impos;{int(*_T1D)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T15;_T14=_T1D;}_T16=_tag_fat("remove_constraint: Constraint refers to an undeclared Cvar",sizeof(char),59U);_T17=_tag_fat(0U,sizeof(void*),0);_T14(_T16,_T17);goto _TL16A;_TL169: _TL16A: goto _LL3;_TL167: _T1C=_T1B;{void*exn=_T1C;_rethrow(exn);}_LL3:;}_TL165:;}}_T18=cvars;
# 942
cvars=_T18->tl;goto _TL161;_TL160:;}
# 957
void Cyc_BansheeIf_reset_local_cvar(void*cvar){int(*_T0)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T1)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T2;struct _fat_ptr _T3;struct _handler_cons*_T4;int*_T5;int _T6;struct _tuple14*(*_T7)(struct Cyc_Hashtable_Table*,void*);void*(*_T8)(struct Cyc_Hashtable_Table*,void*);struct Cyc_List_List**_T9;struct Cyc_List_List**_TA;struct Cyc_List_List**_TB;struct Cyc_List_List*_TC;unsigned _TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_List_List*_T10;void*_T11;struct Cyc_Core_Not_found_exn_struct*_T12;char*_T13;char*_T14;void(*_T15)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*);void(*_T16)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T17;void*_T18;struct _tuple14*_T19;
if(Cyc_BansheeIf_cvarmap==0)goto _TL16C;goto _TL16D;_TL16C: _T1=Cyc___assert_fail;{int(*_T1A)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T1;_T0=_T1A;}_T2=_tag_fat("cvarmap != 0",sizeof(char),13U);_T3=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T0(_T2,_T3,958U);_TL16D: {struct _handler_cons _T1A;_T4=& _T1A;_push_handler(_T4);{int _T1B=0;_T5=_T1A.handler;_T6=setjmp(_T5);if(!_T6)goto _TL16E;_T1B=1;goto _TL16F;_TL16E: _TL16F: if(_T1B)goto _TL170;else{goto _TL172;}_TL172: _T8=Cyc_Hashtable_lookup;{
# 960
struct _tuple14*(*_T1C)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple14*(*)(struct Cyc_Hashtable_Table*,void*))_T8;_T7=_T1C;}{struct _tuple14*_T1C=_T7(Cyc_BansheeIf_cvarmap,cvar);void*_T1D;{struct _tuple14 _T1E=*_T1C;_T9=& _T1C->f1;_T1D=(struct Cyc_List_List**)_T9;}{struct Cyc_List_List**constr=(struct Cyc_List_List**)_T1D;_TA=constr;{
struct Cyc_List_List*l=*_TA;_TB=constr;*_TB=0;
_TL176: _TC=l;_TD=(unsigned)_TC;if(_TD)goto _TL174;else{goto _TL175;}
_TL174: _TE=l;_TF=_TE->hd;Cyc_BansheeIf_remove_constraint(_TF);_T10=l;
# 962
l=_T10->tl;goto _TL176;_TL175:;}}}_pop_handler();goto _TL171;_TL170: _T11=Cyc_Core_get_exn_thrown();{void*_T1C=(void*)_T11;void*_T1D;_T12=(struct Cyc_Core_Not_found_exn_struct*)_T1C;_T13=_T12->tag;_T14=Cyc_Core_Not_found;if(_T13!=_T14)goto _TL177;_T16=Cyc_Hashtable_insert;{
# 967
void(*_T1E)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*))_T16;_T15=_T1E;}_T17=_check_null(Cyc_BansheeIf_cvarmap);_T18=cvar;{struct _tuple14*_T1E=_cycalloc(sizeof(struct _tuple14));_T1E->f0=cvar;_T1E->f1=0;_T1E->f2=0U;_T19=(struct _tuple14*)_T1E;}_T15(_T17,_T18,_T19);goto _LL3;_TL177: _T1D=_T1C;{void*exn=_T1D;_rethrow(exn);}_LL3:;}_TL171:;}}}
# 976
void Cyc_BansheeIf_register_toplevel_cvar(void*cvar){int(*_T0)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T1)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T2;struct _fat_ptr _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T9;void**_TA;void*_TB;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_TC;void**_TD;struct _handler_cons*_TE;int*_TF;int _T10;struct _tuple14*(*_T11)(struct Cyc_Hashtable_Table*,void*);void*(*_T12)(struct Cyc_Hashtable_Table*,void*);enum Cyc_BansheeIf_CvarRecordState _T13;int _T14;int(*_T15)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T16)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T17;struct _fat_ptr _T18;int _T19;void*_T1A;struct Cyc_Core_Not_found_exn_struct*_T1B;char*_T1C;char*_T1D;void(*_T1E)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*);void(*_T1F)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T20;void*_T21;struct _tuple14*_T22;int(*_T23)(struct _fat_ptr,struct _fat_ptr);void*(*_T24)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T25;struct _fat_ptr _T26;
if(Cyc_BansheeIf_cvarmap==0)goto _TL179;goto _TL17A;_TL179: _T1=Cyc___assert_fail;{int(*_T27)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T1;_T0=_T27;}_T2=_tag_fat("cvarmap != 0",sizeof(char),13U);_T3=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T0(_T2,_T3,977U);_TL17A: {const char*_T27;void*_T28;int _T29;void*_T2A;struct Cyc_Core_Opt*_T2B;_T4=cvar;_T5=(int*)_T4;_T6=*_T5;if(_T6!=3)goto _TL17B;_T7=cvar;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T2C=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T2B=_T2C->f1;_T8=cvar;_T9=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T8;_TA=& _T9->f2;_T2A=(void**)_TA;_T29=_T2C->f3;_TB=cvar;_TC=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TB;_TD=& _TC->f4;_T28=(void**)_TD;_T27=_T2C->f5;}{struct Cyc_Core_Opt*ok=_T2B;void**topt=(void**)_T2A;int id=_T29;void**bv=(void**)_T28;const char*name=_T27;{struct _handler_cons _T2C;_TE=& _T2C;_push_handler(_TE);{int _T2D=0;_TF=_T2C.handler;_T10=setjmp(_TF);if(!_T10)goto _TL17D;_T2D=1;goto _TL17E;_TL17D: _TL17E: if(_T2D)goto _TL17F;else{goto _TL181;}_TL181: _T12=Cyc_Hashtable_lookup;{
# 981
struct _tuple14*(*_T2E)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple14*(*)(struct Cyc_Hashtable_Table*,void*))_T12;_T11=_T2E;}{struct _tuple14*_T2E=_T11(Cyc_BansheeIf_cvarmap,cvar);enum Cyc_BansheeIf_CvarRecordState _T2F;struct Cyc_List_List*_T30;void*_T31;{struct _tuple14 _T32=*_T2E;_T31=_T32.f0;_T30=_T32.f1;_T2F=_T32.f2;}{void*cv=_T31;struct Cyc_List_List*constr=_T30;enum Cyc_BansheeIf_CvarRecordState p=_T2F;_T13=p;_T14=(int)_T13;
if(!_T14)goto _TL182;goto _TL183;_TL182: _T16=Cyc___assert_fail;{int(*_T32)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T16;_T15=_T32;}_T17=_tag_fat("p",sizeof(char),2U);_T18=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T15(_T17,_T18,982U);_TL183: _T19=Cyc_Flags_resolve;
if(!_T19)goto _TL184;
# 987
Cyc_BansheeIf_reintroduce_cvar(cv);goto _TL185;_TL184: _TL185:
# 989
 Cyc_BansheeIf_copy_cvar_fields(cv,cvar);}}_pop_handler();goto _TL180;_TL17F: _T1A=Cyc_Core_get_exn_thrown();{void*_T2E=(void*)_T1A;void*_T2F;_T1B=(struct Cyc_Core_Not_found_exn_struct*)_T2E;_T1C=_T1B->tag;_T1D=Cyc_Core_Not_found;if(_T1C!=_T1D)goto _TL186;
# 992
Cyc_BansheeIf_add_variable(cvar);_T1F=Cyc_Hashtable_insert;{
void(*_T30)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*))_T1F;_T1E=_T30;}_T20=_check_null(Cyc_BansheeIf_cvarmap);_T21=cvar;{struct _tuple14*_T30=_cycalloc(sizeof(struct _tuple14));_T30->f0=cvar;_T30->f1=0;_T30->f2=1U;_T22=(struct _tuple14*)_T30;}_T1E(_T20,_T21,_T22);goto _LL8;_TL186: _T2F=_T2E;{void*exn=_T2F;_rethrow(exn);}_LL8:;}_TL180:;}}goto _LL0;}_TL17B: _T24=Cyc_Warn_impos;{
# 998
int(*_T2C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T24;_T23=_T2C;}_T25=_tag_fat("expected cvar only",sizeof(char),19U);_T26=_tag_fat(0U,sizeof(void*),0);_T23(_T25,_T26);_LL0:;}}
# 1002
static void Cyc_BansheeIf_default_unconstrained_cvar(void*cv,struct _tuple14*entry,void*def){struct _tuple14*_T0;enum Cyc_BansheeIf_CvarRecordState _T1;int _T2;int _T3;void*_T4;int _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;struct Cyc_Warn_Typ_Warn_Warg_struct _T7;struct _fat_ptr _T8;_T0=entry;_T1=_T0->f2;_T2=(int)_T1;
if(_T2!=3)goto _TL188;_T3=
Cyc_Tcutil_is_cvar_type(cv);if(!_T3)goto _TL18A;_T4=
Cyc_BansheeIf_equality_constraint(cv,def);_T5=Cyc_BansheeIf_internal_add_constraint(_T4);if(_T5!=0)goto _TL18C;{struct Cyc_Warn_String_Warn_Warg_struct _T9;_T9.tag=0;
_T9.f1=_tag_fat("Resolution failed while applying default to Cvar: ",sizeof(char),51U);_T6=_T9;}{struct Cyc_Warn_String_Warn_Warg_struct _T9=_T6;{struct Cyc_Warn_Typ_Warn_Warg_struct _TA;_TA.tag=2;_TA.f1=cv;_T7=_TA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TA=_T7;void*_TB[2];_TB[0]=& _T9;_TB[1]=& _TA;_T8=_tag_fat(_TB,sizeof(void*),2);Cyc_Warn_err2(0U,_T8);}}goto _TL18D;_TL18C: _TL18D: goto _TL18B;_TL18A: _TL18B: goto _TL189;_TL188: _TL189:;}
# 1011
void Cyc_BansheeIf_resolve_or_default(void*def){int _T0;void(*_T1)(void(*)(void*,struct _tuple14*,void*),struct Cyc_Hashtable_Table*,void*);void(*_T2)(void(*)(void*,void*,void*),struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_T3;void*_T4;_T0=
Cyc_BansheeIf_discharge_implications();if(!_T0)goto _TL18E;
return;_TL18E: _T2=Cyc_Hashtable_iter_c;{
void(*_T5)(void(*)(void*,struct _tuple14*,void*),struct Cyc_Hashtable_Table*,void*)=(void(*)(void(*)(void*,struct _tuple14*,void*),struct Cyc_Hashtable_Table*,void*))_T2;_T1=_T5;}_T3=_check_null(Cyc_BansheeIf_cvarmap);_T4=def;_T1(Cyc_BansheeIf_default_unconstrained_cvar,_T3,_T4);
Cyc_BansheeIf_discharge_implications();}struct _tuple16{void*f0;struct Cyc_List_List*f1;};
# 1021
void Cyc_BansheeIf_init_toplevel_cvars(struct Cyc_List_List*assoc){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;void(*_T4)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*);void(*_T5)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T6;void*_T7;struct _tuple14*_T8;struct Cyc_List_List*_T9;
Cyc_BansheeIf_init_map();
_TL193: _T0=assoc;_T1=(unsigned)_T0;if(_T1)goto _TL191;else{goto _TL192;}
_TL191: _T2=assoc;_T3=_T2->hd;{struct _tuple16*_TA=(struct _tuple16*)_T3;struct Cyc_List_List*_TB;void*_TC;{struct _tuple16 _TD=*_TA;_TC=_TD.f0;_TB=_TD.f1;}{void*cvar=_TC;struct Cyc_List_List*lc=_TB;_T5=Cyc_Hashtable_insert;{
# 1027
void(*_TD)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple14*))_T5;_T4=_TD;}_T6=_check_null(Cyc_BansheeIf_cvarmap);_T7=cvar;{struct _tuple14*_TD=_cycalloc(sizeof(struct _tuple14));_TD->f0=cvar;_TD->f1=lc;_TD->f2=1U;_T8=(struct _tuple14*)_TD;}_T4(_T6,_T7,_T8);}}_T9=assoc;
# 1023
assoc=_T9->tl;goto _TL193;_TL192:;}
# 1031
int Cyc_BansheeIf_add_all_constraints(struct Cyc_List_List*g){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;unsigned _T5;struct Cyc_List_List*_T6;void*_T7;int _T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;int _TC;int _TD;
_TL197: _T0=g;_T1=(unsigned)_T0;if(_T1)goto _TL195;else{goto _TL196;}
_TL195: _T2=g;_T3=_T2->hd;{struct _tuple16*_TE=(struct _tuple16*)_T3;struct Cyc_List_List*_TF;{struct _tuple16 _T10=*_TE;_TF=_T10.f1;}{struct Cyc_List_List*c=_TF;
_TL19B: _T4=c;_T5=(unsigned)_T4;if(_T5)goto _TL199;else{goto _TL19A;}
_TL199: _T6=c;_T7=_T6->hd;_T8=Cyc_BansheeIf_internal_add_constraint(_T7);if(_T8!=0)goto _TL19C;_T9=- 1;
return _T9;_TL19C: _TA=c;
# 1034
c=_TA->tl;goto _TL19B;_TL19A:;}}_TB=g;
# 1032
g=_TB->tl;goto _TL197;_TL196: _TC=
# 1039
Cyc_BansheeIf_discharge_implications();if(_TC!=0)goto _TL19E;_TD=- 1;
return _TD;_TL19E:
 return 0;}
# 1044
static void Cyc_BansheeIf_print_cvar(void*c){struct Cyc_String_pa_PrintArg_struct _T0;struct Cyc___cycFILE*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;{struct Cyc_String_pa_PrintArg_struct _T4;_T4.tag=0;
_T4.f1=Cyc_BansheeIf_cvar2string(c,0);_T0=_T4;}{struct Cyc_String_pa_PrintArg_struct _T4=_T0;void*_T5[1];_T5[0]=& _T4;_T1=Cyc_stderr;_T2=_tag_fat("%s",sizeof(char),3U);_T3=_tag_fat(_T5,sizeof(void*),1);Cyc_fprintf(_T1,_T2,_T3);}}
# 1047
static void Cyc_BansheeIf_print_cvarmapval(struct _tuple14*val){struct _tuple14*_T0;struct Cyc_List_List*_T1;unsigned _T2;struct Cyc_String_pa_PrintArg_struct _T3;struct Cyc_List_List*_T4;void*_T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_List_List*_T9;_T0=val;{
struct Cyc_List_List*l=_T0->f1;_TL1A3: _T1=l;_T2=(unsigned)_T1;if(_T2)goto _TL1A1;else{goto _TL1A2;}
_TL1A1:{struct Cyc_String_pa_PrintArg_struct _TA;_TA.tag=0;_T4=l;_T5=_T4->hd;_TA.f1=Cyc_BansheeIf_constraint2string(_T5);_T3=_TA;}{struct Cyc_String_pa_PrintArg_struct _TA=_T3;void*_TB[1];_TB[0]=& _TA;_T6=Cyc_stderr;_T7=_tag_fat("%s, ",sizeof(char),5U);_T8=_tag_fat(_TB,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}_T9=l;
# 1048
l=_T9->tl;goto _TL1A3;_TL1A2:;}}
# 1053
static void Cyc_BansheeIf_dump_cvarmap (void){struct Cyc_Hashtable_Table*_T0;unsigned _T1;int(*_T2)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T3)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4;struct _fat_ptr _T5;void(*_T6)(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(struct _tuple14*));void(*_T7)(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(void*));struct Cyc_Hashtable_Table*_T8;_T0=Cyc_BansheeIf_cvarmap;_T1=(unsigned)_T0;
if(!_T1)goto _TL1A4;goto _TL1A5;_TL1A4: _T3=Cyc___assert_fail;{int(*_T9)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T3;_T2=_T9;}_T4=_tag_fat("cvarmap",sizeof(char),8U);_T5=_tag_fat("bansheeif.cyc",sizeof(char),14U);_T2(_T4,_T5,1054U);_TL1A5: _T7=Cyc_Hashtable_print_table_map;{
void(*_T9)(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(struct _tuple14*))=(void(*)(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(struct _tuple14*)))_T7;_T6=_T9;}_T8=Cyc_BansheeIf_cvarmap;_T6(_T8,Cyc_BansheeIf_print_cvar,Cyc_BansheeIf_print_cvarmapval);}
# 1058
static void Cyc_BansheeIf_flush_one_variable(void*cvar,struct _tuple14*val){struct _tuple14*_T0;struct Cyc_List_List*_T1;unsigned _T2;struct Cyc_List_List*_T3;void*_T4;int _T5;int(*_T6)(struct _fat_ptr,struct _fat_ptr);void*(*_T7)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;_T0=val;{struct _tuple14 _TC=*_T0;_TB=_TC.f1;}{struct Cyc_List_List*l=_TB;
# 1060
_TL1A9: _T1=l;_T2=(unsigned)_T1;if(_T2)goto _TL1A7;else{goto _TL1A8;}
_TL1A7: _T3=l;_T4=_T3->hd;_T5=Cyc_BansheeIf_internal_add_constraint(_T4);if(_T5!=0)goto _TL1AA;_T7=Cyc_Warn_impos;{
int(*_TC)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T7;_T6=_TC;}_T8=_tag_fat("Inconsistent constraint graph while flushing",sizeof(char),45U);_T9=_tag_fat(0U,sizeof(void*),0);_T6(_T8,_T9);goto _TL1AB;_TL1AA: _TL1AB: _TA=l;
# 1060
l=_TA->tl;goto _TL1A9;_TL1A8:;}}
# 1067
void Cyc_BansheeIf_flush_all_constraints (void){struct Cyc_Hashtable_Table*_T0;unsigned _T1;void(*_T2)(void(*)(void*,struct _tuple14*),struct Cyc_Hashtable_Table*);void(*_T3)(void(*)(void*,void*),struct Cyc_Hashtable_Table*);struct Cyc_Hashtable_Table*_T4;_T0=Cyc_BansheeIf_cvarmap;_T1=(unsigned)_T0;
if(_T1)goto _TL1AC;else{goto _TL1AE;}
_TL1AE: return;_TL1AC: _T3=Cyc_Hashtable_iter;{
# 1071
void(*_T5)(void(*)(void*,struct _tuple14*),struct Cyc_Hashtable_Table*)=(void(*)(void(*)(void*,struct _tuple14*),struct Cyc_Hashtable_Table*))_T3;_T2=_T5;}_T4=Cyc_BansheeIf_cvarmap;_T2(Cyc_BansheeIf_flush_one_variable,_T4);
Cyc_BansheeIf_discharge_implications();}

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
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 133
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 167
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_Hashtable_Table;
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
extern struct Cyc___cycFILE*Cyc_stderr;
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 36 "position.h"
extern struct _fat_ptr Cyc_Position_string_of_loc(unsigned);
# 186 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};
# 933 "absyn.h"
void*Cyc_Absyn_compress(void*);
# 968
extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);
# 1007
void*Cyc_Absyn_thinconst (void);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 30 "bansheeif.h"
int Cyc_BansheeIf_resolve(void*);
# 280 "tcutil.h"
int Cyc_Tcutil_is_cvar_type(void*);
# 26 "flags.h"
extern int Cyc_Flags_verbose;
# 41
extern int Cyc_Flags_resolve;struct list;
# 81 "bansheeif.cyc"
extern void cycinf_init (void);
# 89
extern struct ptrbnd_*ptrbnd_fresh(const char*);
extern struct ptrbnd_*ptrbnd_constant(const char*);
# 92
extern void ptrbnd_unify(struct ptrbnd_*,struct ptrbnd_*);
extern void ptrbnd_cunify(struct ptrbnd_*,struct ptrbnd_*);
extern struct ptrbnd_*ptrbnd_ecr(struct ptrbnd_*);
# 96
extern int ptrbnd_eq(struct ptrbnd_*,struct ptrbnd_*);struct numelts_decon{struct intbound_*f0;};
# 112
extern struct effect_*effect_list_head(struct list*);
extern struct list*effect_list_tail(struct list*);
# 117
extern struct effect_*effect_fresh(const char*);
# 120
extern struct effect_*effect_constant(const char*);
extern int effect_eq(struct effect_*,struct effect_*);
# 126
extern struct list*effect_tlb(struct effect_*);struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct{int tag;struct ptrbnd_*f1;};struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct{int tag;struct effect_*f1;};
# 154
static struct Cyc_List_List*Cyc_BansheeIf_pbconsts;
static struct Cyc_List_List*Cyc_BansheeIf_effconsts;
# 157
int Cyc_BansheeIf_banshee_init (void){
cycinf_init();
return 1;}
# 162
void Cyc_BansheeIf_add_variable(void*cvar){
const char*_Tmp0;void*_Tmp1;void*_Tmp2;if(*((int*)cvar)==3){_Tmp2=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f1;_Tmp1=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f4;_Tmp0=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f5;{struct Cyc_Core_Opt*ok=_Tmp2;void**bv=(void**)_Tmp1;const char*name=_Tmp0;
# 165
if((unsigned)*bv)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("add_variable: Multiple add",sizeof(char),27U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});
{struct Cyc_Absyn_Kind*_Tmp3=(struct Cyc_Absyn_Kind*)_check_null(ok)->v;switch((int)((struct Cyc_Absyn_Kind*)_Tmp3)->kind){case Cyc_Absyn_PtrBndKind:
# 169
({void*_Tmp4=(void*)({struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct));_Tmp5->tag=0,({struct ptrbnd_*_Tmp6=ptrbnd_fresh(name);_Tmp5->f1=_Tmp6;});_Tmp5;});*bv=_Tmp4;});
goto _LL5;case Cyc_Absyn_EffKind:
# 172
({void*_Tmp4=(void*)({struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct));_Tmp5->tag=1,({struct effect_*_Tmp6=effect_fresh(name);_Tmp5->f1=_Tmp6;});_Tmp5;});*bv=_Tmp4;});
goto _LL5;default:
# 178
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("add_variable: Unsupported or null cvar kind",sizeof(char),44U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});}_LL5:;}
# 180
goto _LL0;}}else{
# 182
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("add_variable: Bad type",sizeof(char),23U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)cvar;_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),2));});}_LL0:;}struct _tuple14{void*f0;struct ptrbnd_*f1;};struct _tuple15{void*f0;struct effect_*f1;};
# 186
void Cyc_BansheeIf_add_constant(struct _fat_ptr name,void*cvar){
void*_Tmp0;void*_Tmp1;if(*((int*)cvar)==3){_Tmp1=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f1;_Tmp0=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f4;{struct Cyc_Core_Opt*ok=_Tmp1;void**bv=(void**)_Tmp0;
# 189
if((unsigned)*bv)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("add_constant: Multiple add",sizeof(char),27U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;({int(*_Tmp4)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp4;})(_tag_fat(_Tmp3,sizeof(void*),1));});
{struct Cyc_Absyn_Kind*_Tmp2=(struct Cyc_Absyn_Kind*)_check_null(ok)->v;switch((int)((struct Cyc_Absyn_Kind*)_Tmp2)->kind){case Cyc_Absyn_PtrBndKind:  {
# 193
struct ptrbnd_*pbc=ptrbnd_constant((const char*)_untag_fat_ptr_check_bound(name,sizeof(char),1U));
({void*_Tmp3=(void*)({struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct));_Tmp4->tag=0,_Tmp4->f1=pbc;_Tmp4;});*bv=_Tmp3;});
Cyc_BansheeIf_pbconsts=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple14*_Tmp4=({struct _tuple14*_Tmp5=_cycalloc(sizeof(struct _tuple14));_Tmp5->f0=cvar,_Tmp5->f1=pbc;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=Cyc_BansheeIf_pbconsts;_Tmp3;});
goto _LL5;}case Cyc_Absyn_EffKind:  {
# 198
struct effect_*effc=effect_constant((const char*)_untag_fat_ptr_check_bound(name,sizeof(char),1U));
({void*_Tmp3=(void*)({struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct));_Tmp4->tag=1,_Tmp4->f1=effc;_Tmp4;});*bv=_Tmp3;});
Cyc_BansheeIf_effconsts=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp4=({struct _tuple15*_Tmp5=_cycalloc(sizeof(struct _tuple15));_Tmp5->f0=cvar,_Tmp5->f1=effc;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=Cyc_BansheeIf_effconsts;_Tmp3;});
goto _LL5;}default:
# 206
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("add_constant: Unsupported or null cvar kind",sizeof(char),44U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),1));});}_LL5:;}
# 208
goto _LL0;}}else{
# 210
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("add_constant: Bad type",sizeof(char),23U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)cvar;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}
# 214
enum Cyc_BansheeIf_ConstraintOperator{Cyc_BansheeIf_AndOp =0U,Cyc_BansheeIf_OrOp =1U,Cyc_BansheeIf_NegOp =2U,Cyc_BansheeIf_EqOp =3U,Cyc_BansheeIf_SubsetOp =4U};struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct{int tag;void*f1;unsigned f2;};struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct{int tag;enum Cyc_BansheeIf_ConstraintOperator f1;void*f2;void*f3;unsigned f4;};struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct{int tag;void*f1;void*f2;int f3;unsigned f4;};struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct{int tag;void*f1;void*f2;unsigned f3;};struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct{int tag;void*f1;void*f2;unsigned f3;};struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct{int tag;enum Cyc_BansheeIf_ConstraintOperator f1;void*f2;void*f3;unsigned f4;};
# 234
void*Cyc_BansheeIf_check_constraint(void*c){
return(void*)({struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct));_Tmp0->tag=0,_Tmp0->f1=c,_Tmp0->f2=0U;_Tmp0;});}struct _tuple16{void*f0;void*f1;};
# 238
void*Cyc_BansheeIf_cmpeq_constraint(void*t1,void*t2){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp2=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f4;_Tmp1=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f4;{void**bv1=(void**)_Tmp2;void**bv2=(void**)_Tmp1;
# 241
if(*bv1==0)Cyc_BansheeIf_add_variable(t1);
if(*bv2==0)Cyc_BansheeIf_add_variable(t2);
return(void*)({struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct));_Tmp3->tag=1,_Tmp3->f1=3U,_Tmp3->f2=t1,_Tmp3->f3=t2,_Tmp3->f4=0U;_Tmp3;});}}else{goto _LL3;}}else{_LL3:
# 245
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("cmpeq_constraint: Bad types for constraint - ",sizeof(char),46U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t1;_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t2;_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),3));});};}
# 248
void*Cyc_BansheeIf_cmpsubset_constraint(void*t1,void*t2){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("cmpsubset_constraint: Not yet supported",sizeof(char),40U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});{
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp2=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f4;_Tmp1=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f4;{void**bv1=(void**)_Tmp2;void**bv2=(void**)_Tmp1;
# 252
if(*bv1==0)Cyc_BansheeIf_add_variable(t1);
if(*bv2==0)Cyc_BansheeIf_add_variable(t2);
return(void*)({struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct));_Tmp3->tag=1,_Tmp3->f1=4U,_Tmp3->f2=t1,_Tmp3->f3=t2,_Tmp3->f4=0U;_Tmp3;});}}else{goto _LL3;}}else{_LL3:
# 256
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("cmpsubset_constraint: Bad types for constraint - ",sizeof(char),50U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t1;_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t2;_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),3));});};}}
# 260
static void*Cyc_BansheeIf_internal_equality_constraint(void*t1,void*t2,int c){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp2=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f4;_Tmp1=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f4;{void**bv1=(void**)_Tmp2;void**bv2=(void**)_Tmp1;
# 263
if(*bv1==0)Cyc_BansheeIf_add_variable(t1);
if(*bv2==0)Cyc_BansheeIf_add_variable(t2);
return(void*)({struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=t2,_Tmp3->f3=c,_Tmp3->f4=0U;_Tmp3;});}}else{goto _LL3;}}else{_LL3:
# 267
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("equality_constraint: Bad types for constraint - ",sizeof(char),49U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t1;_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t2;_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),3));});};}
# 271
void*Cyc_BansheeIf_equality_constraint(void*t1,void*t2){
return Cyc_BansheeIf_internal_equality_constraint(t1,t2,0);}
# 275
void*Cyc_BansheeIf_cond_equality_constraint(void*t1,void*t2){
return Cyc_BansheeIf_internal_equality_constraint(t1,t2,1);}
# 279
void*Cyc_BansheeIf_inclusion_constraint(void*t1,void*t2){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp2=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f4;_Tmp1=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f4;{void**bv1=(void**)_Tmp2;void**bv2=(void**)_Tmp1;
# 282
if(*bv1==0)Cyc_BansheeIf_add_variable(t1);
if(*bv2==0)Cyc_BansheeIf_add_variable(t2);{
struct _tuple16 _Tmp3=({struct _tuple16 _Tmp4;_Tmp4.f0=*bv1,_Tmp4.f1=*bv2;_Tmp4;});if(_Tmp3.f0!=0){if(*((int*)_Tmp3.f0)==1){if(_Tmp3.f1!=0){if(*((int*)_Tmp3.f1)==1)
# 286
return(void*)({struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct));_Tmp4->tag=2,_Tmp4->f1=t1,_Tmp4->f2=t2,_Tmp4->f3=0,_Tmp4->f4=0U;_Tmp4;});else{goto _LL8;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8:
# 288
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("inclusion_constraint: Term sort doesn't permit inclusion constraints",sizeof(char),69U);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({int(*_Tmp6)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp6;})(_tag_fat(_Tmp5,sizeof(void*),1));});};}}}else{goto _LL3;}}else{_LL3:
# 291
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("inclusion_constraint: Bad types for constraint - ",sizeof(char),50U);_Tmp4;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=(void*)t1;_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t2;_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),3));});};}
# 295
void*Cyc_BansheeIf_implication_constraint(void*t1,void*t2){
return(void*)({struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct));_Tmp0->tag=4,_Tmp0->f1=t1,_Tmp0->f2=t2,_Tmp0->f3=0U;_Tmp0;});}
# 299
static void*Cyc_BansheeIf_composite_constraint(enum Cyc_BansheeIf_ConstraintOperator op,void*t1,void*t2){
if(op==Cyc_BansheeIf_NegOp){
# 302
if(t2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("composite_constraint: Neg constraint is unary",sizeof(char),46U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
return(void*)({struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct));_Tmp0->tag=5,_Tmp0->f1=op,_Tmp0->f2=t1,_Tmp0->f3=0,_Tmp0->f4=0U;_Tmp0;});}else{
# 306
if(!((unsigned)t1)|| !((unsigned)t2))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("composite_constraint: Binary constraint missing argument",sizeof(char),57U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});
return(void*)({struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct));_Tmp0->tag=5,_Tmp0->f1=op,_Tmp0->f2=t1,_Tmp0->f3=t2,_Tmp0->f4=0U;_Tmp0;});};}
# 312
void*Cyc_BansheeIf_and_constraint(void*c1,void*c2){
return Cyc_BansheeIf_composite_constraint(0U,c1,c2);}
# 315
void*Cyc_BansheeIf_not_constraint(void*b){
return Cyc_BansheeIf_composite_constraint(2U,b,0);}
# 318
void*Cyc_BansheeIf_or_constraint(void*c1,void*c2){
return Cyc_BansheeIf_composite_constraint(1U,c1,c2);}
# 322
void*Cyc_BansheeIf_neg_constraint(void*c1){
return Cyc_BansheeIf_composite_constraint(2U,c1,0);}
# 326
static void*Cyc_BansheeIf_match_ptrbnd_constant(struct ptrbnd_*pb){
{struct Cyc_List_List*l=Cyc_BansheeIf_pbconsts;for(0;(unsigned)l;l=l->tl){
if(ptrbnd_eq(((struct _tuple14*)l->hd)->f1,pb))
return((struct _tuple14*)l->hd)->f0;}}
# 332
return 0;}
# 334
static int Cyc_BansheeIf_is_constant(void*t){
int _Tmp0;if(*((int*)t)==3){_Tmp0=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)t)->f3;{int id=_Tmp0;
# 337
return id==0;}}else{
# 339
return 0;};}
# 344
static struct _fat_ptr Cyc_BansheeIf_op2string(enum Cyc_BansheeIf_ConstraintOperator op){
switch((int)op){case Cyc_BansheeIf_AndOp:
 return _tag_fat("A",sizeof(char),2U);case Cyc_BansheeIf_OrOp:
 return _tag_fat("V",sizeof(char),2U);case Cyc_BansheeIf_NegOp:
 return _tag_fat("!",sizeof(char),2U);case Cyc_BansheeIf_EqOp:
 return _tag_fat("=",sizeof(char),2U);case Cyc_BansheeIf_SubsetOp:
 return _tag_fat("<",sizeof(char),2U);default:
 return _tag_fat("ERR",sizeof(char),4U);};}
# 374 "bansheeif.cyc"
static struct _fat_ptr Cyc_BansheeIf_cvar2string(void*t1,int xc){
if(Cyc_Tcutil_is_cvar_type(t1)|| !xc && !Cyc_BansheeIf_is_constant(t1))
return Cyc_Absynpp_typ2string(t1);{
struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Absynpp_typ2string(Cyc_Absyn_compress(t1));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;return Cyc_aprintf(_tag_fat("\"%s\"",sizeof(char),5U),_tag_fat(_Tmp1,sizeof(void*),1));}}
# 380
static const char*Cyc_BansheeIf_cvarloc(void*cv){
const char*_Tmp0;if(*((int*)cv)==3){_Tmp0=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f6;{const char*l=_Tmp0;
return l;}}else{
return 0;};}
# 387
static const char*Cyc_BansheeIf_cvarorigfat(void*cv){
int _Tmp0;if(*((int*)cv)==3){_Tmp0=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f7;{int o=_Tmp0;
if(o)return"true";else{return"false";}}}else{
return 0;};}
# 394
static struct _fat_ptr Cyc_BansheeIf__constraint2string(void*c,int xcvar){
int _Tmp0;void*_Tmp1;enum Cyc_BansheeIf_ConstraintOperator _Tmp2;void*_Tmp3;switch(*((int*)c)){case 0: _Tmp3=(void*)((struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)c)->f1;{void*c1=_Tmp3;
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_BansheeIf__constraint2string(c1,xcvar);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;return Cyc_aprintf(_tag_fat("!(%s)",sizeof(char),6U),_tag_fat(_Tmp5,sizeof(void*),1));}case 1: _Tmp2=((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp3=(void*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)c)->f2;_Tmp1=(void*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)c)->f3;{enum Cyc_BansheeIf_ConstraintOperator op=_Tmp2;void*t1=_Tmp3;void*t2=_Tmp1;
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_BansheeIf_op2string(op);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_BansheeIf_cvar2string(t1,xcvar);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_BansheeIf_cvar2string(t2,xcvar);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;return Cyc_aprintf(_tag_fat("^(%s, %s, %s)",sizeof(char),14U),_tag_fat(_Tmp7,sizeof(void*),3));}case 2: _Tmp3=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp1=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f2;_Tmp0=((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f3;{void*t1=_Tmp3;void*t2=_Tmp1;int b=_Tmp0;
# 399
if(b){
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_BansheeIf_cvar2string(t1,xcvar);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_BansheeIf_cvar2string(t2,xcvar);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;return Cyc_aprintf(_tag_fat("?(%s,%s)",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));}else{
# 403
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_BansheeIf_cvar2string(t1,xcvar);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_BansheeIf_cvar2string(t2,xcvar);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;return Cyc_aprintf(_tag_fat("=(%s, %s)",sizeof(char),10U),_tag_fat(_Tmp6,sizeof(void*),2));}}case 3: _Tmp3=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp1=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)c)->f2;{void*t1=_Tmp3;void*t2=_Tmp1;
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_BansheeIf_cvar2string(t1,xcvar);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_BansheeIf_cvar2string(t2,xcvar);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;return Cyc_aprintf(_tag_fat("<(%s,%s)",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));}case 4: _Tmp3=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp1=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)c)->f2;{void*c1=_Tmp3;void*c2=_Tmp1;
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_BansheeIf__constraint2string(c1,xcvar);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_BansheeIf__constraint2string(c2,xcvar);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;return Cyc_aprintf(_tag_fat(">(%s,%s)",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));}default: _Tmp2=((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp3=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f2;_Tmp1=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f3;{enum Cyc_BansheeIf_ConstraintOperator op=_Tmp2;void*c1=_Tmp3;void*c2=_Tmp1;
struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_BansheeIf__constraint2string(c1,xcvar);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=Cyc_BansheeIf_op2string(op);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_BansheeIf__constraint2string(c2,xcvar);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;return Cyc_aprintf(_tag_fat("+(%s,%s,%s)",sizeof(char),12U),_tag_fat(_Tmp7,sizeof(void*),3));}};}
# 410
struct _fat_ptr Cyc_BansheeIf_constraint2string(void*c){
return Cyc_BansheeIf__constraint2string(c,1);}
# 414
static void Cyc_BansheeIf_print_constraints(struct Cyc_List_List*c){
for(1;(unsigned)c;c=c->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_BansheeIf_constraint2string((void*)c->hd);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s\n",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});}}
# 425
static int Cyc_BansheeIf_nodelay_add_constraint(void*c){
enum Cyc_BansheeIf_ConstraintOperator _Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)c)){case 2: if(*((int*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f1)==3){if(*((int*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f2)==3){_Tmp5=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp4=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f1)->f4;_Tmp3=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f2;_Tmp2=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f2)->f4;_Tmp1=((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f3;{void*t1=_Tmp5;void*bv1=_Tmp4;void*t2=_Tmp3;void*bv2=_Tmp2;int b=_Tmp1;
# 428
{struct _tuple16 _Tmp6=({struct _tuple16 _Tmp7;_Tmp7.f0=bv1,_Tmp7.f1=bv2;_Tmp7;});void*_Tmp7;void*_Tmp8;if(_Tmp6.f0!=0){if(*((int*)_Tmp6.f0)==0){if(_Tmp6.f1!=0){if(*((int*)_Tmp6.f1)==0){_Tmp8=((struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_Tmp6.f0)->f1;_Tmp7=((struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_Tmp6.f1)->f1;{struct ptrbnd_*pb1=_Tmp8;struct ptrbnd_*pb2=_Tmp7;
# 430
struct ptrbnd_*ecr1=ptrbnd_ecr(pb1);
struct ptrbnd_*ecr2=ptrbnd_ecr(pb2);
if((!ptrbnd_eq(ecr1,ecr2)&&(unsigned)
Cyc_BansheeIf_match_ptrbnd_constant(ecr1))&&(unsigned)
Cyc_BansheeIf_match_ptrbnd_constant(ecr2)){
# 436
Cyc_BansheeIf_resolve(t1);
Cyc_BansheeIf_resolve(t2);
# 439
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat("Unsatisfiable constraint ",sizeof(char),26U);_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,({struct _fat_ptr _TmpC=Cyc_BansheeIf__constraint2string(c,0);_TmpB.f1=_TmpC;});_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_Warn_err2(0U,_tag_fat(_TmpB,sizeof(void*),2));});
# 443
return 0;}
# 445
if(Cyc_Flags_verbose)
Cyc_fprintf(Cyc_stderr,_tag_fat("***Added\n",sizeof(char),10U),_tag_fat(0U,sizeof(void*),0));
if(b)
ptrbnd_cunify(pb1,pb2);else{
# 451
ptrbnd_unify(pb1,pb2);}
# 453
return 1;}}else{goto _LLC;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC:
# 455
({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp9;})(_tag_fat("Kind not yet supported",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));};}
# 457
goto _LL0;}}else{goto _LL7;}}else{goto _LL7;}case 5: _Tmp0=((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp5=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f2;_Tmp4=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f3;{enum Cyc_BansheeIf_ConstraintOperator op=_Tmp0;void*c1=_Tmp5;void*c2=_Tmp4;
# 459
if((int)op!=0)
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("Disjunctions and negations require a delay",sizeof(char),43U),_tag_fat(0U,sizeof(void*),0));
return Cyc_BansheeIf_nodelay_add_constraint(c1)&&
 Cyc_BansheeIf_nodelay_add_constraint(c2);}case 3: _Tmp5=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp4=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)c)->f2;{void*bv1=_Tmp5;void*bv2=_Tmp4;
# 464
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("setIF sort not yet supported",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}default: _LL7:
# 466
({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp6;})(_tag_fat("No implications allowed",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 470
static int Cyc_BansheeIf_requires_delay(void*c){
void*_Tmp0;void*_Tmp1;enum Cyc_BansheeIf_ConstraintOperator _Tmp2;switch(*((int*)c)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 4: _LL6:
# 475
 return 1;case 5: _Tmp2=((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp1=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f2;_Tmp0=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f3;{enum Cyc_BansheeIf_ConstraintOperator op=_Tmp2;void*c1=_Tmp1;void*c2=_Tmp0;
# 477
return((int)op!=0 || Cyc_BansheeIf_requires_delay(c1))|| Cyc_BansheeIf_requires_delay(c2);}default:
# 479
 return 0;};}
# 483
static struct Cyc_List_List*Cyc_BansheeIf_delayed_constraints;
# 487
static int Cyc_BansheeIf_internal_add_constraint(void*c){
if(Cyc_BansheeIf_requires_delay(c)){
Cyc_BansheeIf_delayed_constraints=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=c,_Tmp0->tl=Cyc_BansheeIf_delayed_constraints;_Tmp0;});
return 2;}
# 492
return Cyc_BansheeIf_nodelay_add_constraint(c);}
# 495
static int Cyc_BansheeIf_insert_into_map(void*);
static void Cyc_BansheeIf_add_location(unsigned loc,void*c){
void*_Tmp0;switch(*((int*)c)){case 0: _Tmp0=(unsigned*)&((struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)c)->f2;{unsigned*l=_Tmp0;
_Tmp0=l;goto _LL4;}case 1: _Tmp0=(unsigned*)&((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)c)->f4;_LL4: {unsigned*l=_Tmp0;
_Tmp0=l;goto _LL6;}case 2: _Tmp0=(unsigned*)&((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f4;_LL6: {unsigned*l=_Tmp0;
_Tmp0=l;goto _LL8;}case 3: _Tmp0=(unsigned*)&((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)c)->f3;_LL8: {unsigned*l=_Tmp0;
_Tmp0=l;goto _LLA;}case 4: _Tmp0=(unsigned*)&((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)c)->f3;_LLA: {unsigned*l=_Tmp0;
_Tmp0=l;goto _LLC;}default: _Tmp0=(unsigned*)&((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f4;_LLC: {unsigned*l=(unsigned*)_Tmp0;
*l=loc;}};}
# 506
int Cyc_BansheeIf_add_constraint(unsigned loc,void*c){
Cyc_BansheeIf_add_location(loc,c);
Cyc_BansheeIf_insert_into_map(c);
if(Cyc_Flags_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Position_string_of_loc(loc);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_BansheeIf__constraint2string(c,0);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_fprintf(Cyc_stderr,_tag_fat("-- (%s) Adding constraint : %s \t",sizeof(char),33U),_tag_fat(_Tmp2,sizeof(void*),2));});{
int ret=Cyc_BansheeIf_internal_add_constraint(c)!=0;
if(Cyc_Flags_verbose)
Cyc_fprintf(Cyc_stderr,_tag_fat("\n",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));
return ret;}}
# 517
static int Cyc_BansheeIf_eval_guard(void*g){
void*_Tmp0;void*_Tmp1;enum Cyc_BansheeIf_ConstraintOperator _Tmp2;switch(*((int*)g)){case 1: if(*((int*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)g)->f2)==3){if(*((int*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)g)->f3)==3){_Tmp2=((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)g)->f1;_Tmp1=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)g)->f2)->f4;_Tmp0=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)g)->f3)->f4;{enum Cyc_BansheeIf_ConstraintOperator op=_Tmp2;void*bv1=_Tmp1;void*bv2=_Tmp0;
# 520
if((int)op!=3)
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Unsupported comparison operator",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple16 _Tmp3=({struct _tuple16 _Tmp4;_Tmp4.f0=bv1,_Tmp4.f1=bv2;_Tmp4;});void*_Tmp4;void*_Tmp5;if(_Tmp3.f0!=0){if(*((int*)_Tmp3.f0)==0){if(_Tmp3.f1!=0){if(*((int*)_Tmp3.f1)==0){_Tmp5=((struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_Tmp3.f0)->f1;_Tmp4=((struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)_Tmp3.f1)->f1;{struct ptrbnd_*pb1=_Tmp5;struct ptrbnd_*pb2=_Tmp4;
# 524
return ptrbnd_eq(pb1,pb2);}}else{goto _LLC;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC:
# 526
 return 0;};}}}else{goto _LL7;}}else{goto _LL7;}case 5: _Tmp2=((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)g)->f1;_Tmp1=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)g)->f2;_Tmp0=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)g)->f3;{enum Cyc_BansheeIf_ConstraintOperator op=_Tmp2;void*c1=_Tmp1;void*c2=_Tmp0;
# 529
int c1g=Cyc_BansheeIf_eval_guard(c1);
int c2g=(int)op!=0 &&(int)op!=1?0: Cyc_BansheeIf_eval_guard(c2);
switch((int)op){case Cyc_BansheeIf_AndOp:
 return c1g && c2g;case Cyc_BansheeIf_OrOp:
 return c1g || c2g;case Cyc_BansheeIf_NegOp:
 return !c1g;default:
 return 0;};}case 4: _Tmp1=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)g)->f1;_Tmp0=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)g)->f2;{void*c1=_Tmp1;void*c2=_Tmp0;
# 538
if(!Cyc_BansheeIf_eval_guard(c1))
return 1;
return Cyc_BansheeIf_eval_guard(c2);}default: _LL7:
# 542
 return 0;};}
# 546
int Cyc_BansheeIf_discharge_implications (void){
int changed=0;
do{
struct Cyc_List_List*prev=0;
struct Cyc_List_List*l=Cyc_BansheeIf_delayed_constraints;
int first=1;
changed=0;
while((unsigned)l){
{void*_Tmp0=(void*)l->hd;void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 4: _Tmp2=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_Tmp0)->f2;{void*c1=_Tmp2;void*c2=_Tmp1;
# 556
if(Cyc_BansheeIf_eval_guard(c1)){
int result=Cyc_BansheeIf_internal_add_constraint(c2);
if(result==0)return 0;
if(result==1)changed=1;
# 561
if(first){
if(result==2){
Cyc_BansheeIf_delayed_constraints->tl=_check_null(_check_null(Cyc_BansheeIf_delayed_constraints)->tl)->tl;
first=0;}else{
# 567
Cyc_BansheeIf_delayed_constraints=_check_null(Cyc_BansheeIf_delayed_constraints)->tl;}}else{
# 569
if((unsigned)prev)
prev->tl=l->tl;}{
struct Cyc_List_List*tmp=l;
l=l->tl;
tmp->tl=0;}}else{
# 576
prev=l;
l=l->tl;
first=0;}
# 580
goto _LL0;}case 0: _Tmp2=(void*)((struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_Tmp0)->f1;{void*c1=_Tmp2;
# 583
first=0;
prev=l;
l=l->tl;
# 594 "bansheeif.cyc"
goto _LL0;}default:
# 596
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Malformed delayed constraint; only implications permitted",sizeof(char),58U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}
# 554 "bansheeif.cyc"
1U;}}while(changed);
# 601 "bansheeif.cyc"
{struct Cyc_List_List*dc=Cyc_BansheeIf_delayed_constraints;for(0;(unsigned)dc;dc=dc->tl){
void*_Tmp0=(void*)dc->hd;void*_Tmp1;if(*((int*)_Tmp0)==0){_Tmp1=(void*)((struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_Tmp0)->f1;{void*c1=_Tmp1;
# 604
if(!Cyc_BansheeIf_eval_guard(c1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("Constraint could not be proved: ",sizeof(char),33U);_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_BansheeIf_constraint2string((void*)dc->hd);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(0U,_tag_fat(_Tmp4,sizeof(void*),2));});
# 607
goto _LL7;}}else{
# 609
goto _LL7;}_LL7:;}}
# 612
return 1;}
# 615
int Cyc_BansheeIf_resolve(void*cvar){
void*_Tmp0;void*_Tmp1;if(*((int*)cvar)==3){_Tmp1=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f2;_Tmp0=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f4;{void**t=(void**)_Tmp1;void*bv=_Tmp0;
# 618
if((unsigned)*t)
return 1;
{void*_Tmp2;if(bv!=0)switch(*((int*)bv)){case 0: _Tmp2=((struct Cyc_BansheeIf_BvarPtrBnd_BansheeIf_BansheeVar_struct*)bv)->f1;{struct ptrbnd_*pb=_Tmp2;
# 622
struct ptrbnd_*pb2=ptrbnd_ecr(pb);
void*tmp=Cyc_BansheeIf_match_ptrbnd_constant(pb2);
if((unsigned)tmp){
*t=tmp;
return 1;}
# 628
return 0;}case 1: _Tmp2=((struct Cyc_BansheeIf_BvarEffect_BansheeIf_BansheeVar_struct*)bv)->f1;{struct effect_*ef=_Tmp2;
# 630
struct list*efl=effect_tlb(ef);
struct Cyc_List_List*set=0;
for(1;(unsigned)efl;efl=effect_list_tail(efl)){
struct Cyc_List_List*l=Cyc_BansheeIf_effconsts;for(0;(unsigned)l;l=l->tl){
if(({struct effect_*_Tmp3=((struct _tuple15*)l->hd)->f1;effect_eq(_Tmp3,effect_list_head(efl));}))
set=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=((struct _tuple15*)l->hd)->f0,_Tmp3->tl=set;_Tmp3;});}}
# 639
if((unsigned)set)
({void*_Tmp3=Cyc_Absyn_join_eff(set);*t=_Tmp3;});else{
# 642
return 0;}
goto _LL5;}default: goto _LLA;}else{_LLA:
# 645
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Attempt to resolve a variable that was never added",sizeof(char),51U),_tag_fat(0U,sizeof(void*),0));}_LL5:;}
# 647
goto _LL0;}}else{
# 649
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("Bad type to resolve; expect Cvar only",sizeof(char),38U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)cvar;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;({int(*_Tmp5)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp5;})(_tag_fat(_Tmp4,sizeof(void*),2));});}_LL0:;}
# 658
enum Cyc_BansheeIf_CvarRecordState{Cyc_BansheeIf_LOCAL =0U,Cyc_BansheeIf_PERSIST_INIT =1U,Cyc_BansheeIf_PERSIST_INPROCESS =2U,Cyc_BansheeIf_PERSIST_REINTRO =3U,Cyc_BansheeIf_PERSIST_DONE =4U};
# 666
static struct Cyc_Hashtable_Table*Cyc_BansheeIf_cvarmap;struct _tuple17{void*f0;struct Cyc_List_List*f1;enum Cyc_BansheeIf_CvarRecordState f2;};
static struct Cyc_List_List*Cyc_BansheeIf_lookup_cvar_constraints(void*t){
if((unsigned)Cyc_BansheeIf_cvarmap){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_List_List*_Tmp2=({struct _tuple17*(*_Tmp3)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple17*(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup;_Tmp3;})(Cyc_BansheeIf_cvarmap,t)->f1;_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found)
# 673
return 0;else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}
# 676
return 0;}
# 679
static int Cyc_BansheeIf_cvarcmp(void*c1,void*c2){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=c1,_Tmp1.f1=c2;_Tmp1;});const char*_Tmp1;const char*_Tmp2;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp2=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f5;_Tmp1=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f5;{const char*n1=_Tmp2;const char*n2=_Tmp1;
# 682
return Cyc_strcmp(({const char*_Tmp3=n1;_tag_fat((void*)_Tmp3,sizeof(char),_get_zero_arr_size_char((void*)_Tmp3,1U));}),({const char*_Tmp3=n2;_tag_fat((void*)_Tmp3,sizeof(char),_get_zero_arr_size_char((void*)_Tmp3,1U));}));}}else{goto _LL3;}}else{_LL3:
# 684
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("Expecting cvar types only",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}
# 688
static int Cyc_BansheeIf_cvareq(void*c1,void*c2){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=c1,_Tmp1.f1=c2;_Tmp1;});const char*_Tmp1;void*_Tmp2;const char*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_Tmp4=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f4;_Tmp3=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f5;_Tmp2=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f4;_Tmp1=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f5;{void*b1=_Tmp4;const char*n1=_Tmp3;void*b2=_Tmp2;const char*n2=_Tmp1;
# 691
return !Cyc_strcmp(({const char*_Tmp5=n1;_tag_fat((void*)_Tmp5,sizeof(char),_get_zero_arr_size_char((void*)_Tmp5,1U));}),({const char*_Tmp5=n2;_tag_fat((void*)_Tmp5,sizeof(char),_get_zero_arr_size_char((void*)_Tmp5,1U));}));}}else{goto _LL3;}}else{_LL3:
# 693
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("Expecting cvar types only",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}
# 697
static int Cyc_BansheeIf_cvarhash(void*c){
const char*_Tmp0;void*_Tmp1;if(*((int*)c)==3){_Tmp1=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)c)->f4;_Tmp0=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)c)->f5;{void*b1=_Tmp1;const char*name=_Tmp0;
# 701
return Cyc_Hashtable_hash_string(({const char*_Tmp2=name;_tag_fat((void*)_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));}));}}else{
# 703
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("Expecting cvar types only",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}
# 707
static int Cyc_BansheeIf_not_in_list(struct Cyc_List_List*l,void*e){
return !Cyc_List_exists_c(Cyc_BansheeIf_cvareq,e,l);}
# 711
static struct Cyc_List_List*Cyc_BansheeIf_cvars_in_constraint(void*c){
void*_Tmp0;void*_Tmp1;switch(*((int*)c)){case 1: _Tmp1=(void*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)c)->f2;_Tmp0=(void*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)c)->f3;{void*cv1=_Tmp1;void*cv2=_Tmp0;
_Tmp1=cv1;_Tmp0=cv2;goto _LL4;}case 2: _Tmp1=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp0=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)c)->f2;_LL4: {void*cv1=_Tmp1;void*cv2=_Tmp0;
_Tmp1=cv1;_Tmp0=cv2;goto _LL6;}case 3: _Tmp1=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp0=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)c)->f2;_LL6: {void*cv1=_Tmp1;void*cv2=_Tmp0;
# 716
struct Cyc_List_List*ret=0;
if(!Cyc_BansheeIf_is_constant(cv1))
ret=({void*_Tmp2[1];_Tmp2[0]=cv1;Cyc_List_list(_tag_fat(_Tmp2,sizeof(void*),1));});
if(Cyc_BansheeIf_cvarcmp(cv1,cv2)&& !Cyc_BansheeIf_is_constant(cv2))
ret=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=cv2,_Tmp2->tl=ret;_Tmp2;});
return ret;}case 0: _Tmp1=(void*)((struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)c)->f1;{void*c1=_Tmp1;
return Cyc_BansheeIf_cvars_in_constraint(c1);}case 4: _Tmp1=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)c)->f1;_Tmp0=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)c)->f2;{void*c1=_Tmp1;void*c2=_Tmp0;
_Tmp1=c1;_Tmp0=c2;goto _LLC;}default: _Tmp1=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f2;_Tmp0=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)c)->f3;_LLC: {void*c1=_Tmp1;void*c2=_Tmp0;
# 725
struct Cyc_List_List*l1=Cyc_BansheeIf_cvars_in_constraint(c1);
struct Cyc_List_List*_Tmp2=l1;return Cyc_List_append(_Tmp2,({struct Cyc_List_List*(*_Tmp3)(int(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp4)(int(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_filter_c;_Tmp4;});struct Cyc_List_List*_Tmp4=l1;_Tmp3(Cyc_BansheeIf_not_in_list,_Tmp4,Cyc_BansheeIf_cvars_in_constraint(c2));}));}};}
# 730
static void Cyc_BansheeIf_init_map (void){
if(!((unsigned)Cyc_BansheeIf_cvarmap))
Cyc_BansheeIf_cvarmap=Cyc_Hashtable_create(101,Cyc_BansheeIf_cvarcmp,Cyc_BansheeIf_cvarhash);}
# 736
static int Cyc_BansheeIf_constraint_eq(void*c1,void*c2){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=c1,_Tmp1.f1=c2;_Tmp1;});enum Cyc_BansheeIf_ConstraintOperator _Tmp1;enum Cyc_BansheeIf_ConstraintOperator _Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;switch(*((int*)_Tmp0.f0)){case 2: if(*((int*)_Tmp0.f1)==2){_Tmp8=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f2;_Tmp6=((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f3;_Tmp5=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f1;_Tmp4=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f2;_Tmp3=((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f3;{void*cv11=_Tmp8;void*cv12=_Tmp7;int f1=_Tmp6;void*cv21=_Tmp5;void*cv22=_Tmp4;int f2=_Tmp3;
# 739
if(f1==f2)
return Cyc_BansheeIf_cvareq(cv11,cv21)&& Cyc_BansheeIf_cvareq(cv12,cv22);
return 0;}}else{goto _LLD;}case 1: if(*((int*)_Tmp0.f1)==1){_Tmp2=((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f1;_Tmp8=(void*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f2;_Tmp7=(void*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f2;_Tmp4=(void*)((struct Cyc_BansheeIf_ComparisonConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f3;{enum Cyc_BansheeIf_ConstraintOperator op1=_Tmp2;void*cv11=_Tmp8;void*cv12=_Tmp7;enum Cyc_BansheeIf_ConstraintOperator op2=_Tmp1;void*cv21=_Tmp5;void*cv22=_Tmp4;
# 743
if((int)op1==(int)op2){
_Tmp8=cv11;_Tmp7=cv12;_Tmp5=cv21;_Tmp4=cv22;goto _LL6;}
return 0;}}else{goto _LLD;}case 3: if(*((int*)_Tmp0.f1)==3){_Tmp8=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f2;_Tmp5=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f1;_Tmp4=(void*)((struct Cyc_BansheeIf_InclusionConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f2;_LL6: {void*cv11=_Tmp8;void*cv12=_Tmp7;void*cv21=_Tmp5;void*cv22=_Tmp4;
# 747
return Cyc_BansheeIf_cvareq(cv11,cv21)&& Cyc_BansheeIf_cvareq(cv12,cv22);}}else{goto _LLD;}case 0: if(*((int*)_Tmp0.f1)==0){_Tmp8=(void*)((struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_BansheeIf_CheckConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f1;{void*c11=_Tmp8;void*c21=_Tmp7;
# 749
return Cyc_BansheeIf_constraint_eq(c11,c21);}}else{goto _LLD;}case 4: if(*((int*)_Tmp0.f1)==4){_Tmp8=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f2;_Tmp5=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f1;_Tmp4=(void*)((struct Cyc_BansheeIf_ImplicationConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f2;{void*c11=_Tmp8;void*c12=_Tmp7;void*c21=_Tmp5;void*c22=_Tmp4;
# 751
return Cyc_BansheeIf_constraint_eq(c11,c21)&& Cyc_BansheeIf_constraint_eq(c12,c22);}}else{goto _LLD;}default: if(*((int*)_Tmp0.f1)==5){_Tmp2=((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f1;_Tmp8=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f2;_Tmp7=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f2;_Tmp4=(void*)((struct Cyc_BansheeIf_CompositeConstraint_BansheeIf_Constraint_struct*)_Tmp0.f1)->f3;{enum Cyc_BansheeIf_ConstraintOperator op1=_Tmp2;void*c11=_Tmp8;void*c12=_Tmp7;enum Cyc_BansheeIf_ConstraintOperator op2=_Tmp1;void*c21=_Tmp5;void*c22=_Tmp4;
# 753
return((int)op1==(int)op2 && Cyc_BansheeIf_constraint_eq(c11,c21))&&((unsigned)c12?Cyc_BansheeIf_constraint_eq(c12,_check_null(c22)): c22==0);}}else{_LLD:
# 755
 return 0;}};}
# 759
static int Cyc_BansheeIf_constraint_cmp(void*c1,void*c2){
return !Cyc_BansheeIf_constraint_eq(c1,c2);}
# 762
static int Cyc_BansheeIf_hash_constraint(void*c){
return Cyc_Hashtable_hash_string(Cyc_BansheeIf_constraint2string(c));}
# 766
static int Cyc_BansheeIf_insert_into_map(void*c){
Cyc_BansheeIf_init_map();{
int added=0;
{struct Cyc_List_List*cvars=Cyc_BansheeIf_cvars_in_constraint(c);for(0;(unsigned)cvars;cvars=cvars->tl){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple17*_Tmp2=({struct _tuple17*(*_Tmp3)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple17*(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup;_Tmp3;})(Cyc_BansheeIf_cvarmap,(void*)cvars->hd);void*_Tmp3;_Tmp3=(struct Cyc_List_List**)& _Tmp2->f1;{struct Cyc_List_List**oc=(struct Cyc_List_List**)_Tmp3;
if(!Cyc_List_exists_c(Cyc_BansheeIf_constraint_eq,c,*oc)){
({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=c,_Tmp5->tl=*oc;_Tmp5;});*oc=_Tmp4;});
++ added;}}}
# 771
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 778
{void*_Tmp4=(void*)cvars->hd;int _Tmp5;if(*((int*)_Tmp4)==3){_Tmp5=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp4)->f3;{int id=_Tmp5;
# 780
id < 0?0:({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp6;})(_tag_fat("id < 0",sizeof(char),7U),_tag_fat("bansheeif.cyc",sizeof(char),14U),780U);
goto _LL8;}}else{
# 783
0?0:({int(*_Tmp6)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp6;})(_tag_fat("0",sizeof(char),2U),_tag_fat("bansheeif.cyc",sizeof(char),14U),783U);
goto _LL8;}_LL8:;}
# 786
({void(*_Tmp4)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*)=({void(*_Tmp5)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*))Cyc_Hashtable_insert;_Tmp5;});struct Cyc_Hashtable_Table*_Tmp5=Cyc_BansheeIf_cvarmap;void*_Tmp6=(void*)cvars->hd;_Tmp4(_Tmp5,_Tmp6,({struct _tuple17*_Tmp7=_cycalloc(sizeof(struct _tuple17));_Tmp7->f0=(void*)cvars->hd,({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=c,_Tmp9->tl=0;_Tmp9;});_Tmp7->f1=_Tmp8;}),_Tmp7->f2=0U;_Tmp7;}));});
goto _LL3;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL3:;}}}}
# 790
return added;}}
# 794
static void*Cyc_BansheeIf_eval_equality_constraint(void*cvar,struct Cyc_List_List*lc){
if((unsigned)lc && !((unsigned)lc->tl)){
void*_Tmp0=(void*)lc->hd;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==2){if(((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0)->f3==0){_Tmp2=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_BansheeIf_EqualityConstraint_BansheeIf_Constraint_struct*)_Tmp0)->f2;{void*c1=_Tmp2;void*c2=_Tmp1;
# 798
if(!Cyc_BansheeIf_cvarcmp(cvar,c1))
return Cyc_Absyn_compress(c2);
if(!Cyc_BansheeIf_cvarcmp(cvar,c2))
return Cyc_Absyn_compress(c1);
goto _LL4;}}else{goto _LL3;}}else{_LL3: _LL4:
# 804
 return 0;};}
# 807
return 0;}
# 811
static void Cyc_BansheeIf_reintroduce_cvar(void*_cvar){
struct _tuple17*val;
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
val=({struct _tuple17*(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple17*(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup;_Tmp2;})(_check_null(Cyc_BansheeIf_cvarmap),_cvar);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found)
# 817
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("Constraint graph refers to an undefined constraint variable",sizeof(char),60U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)_cvar;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;({int(*_Tmp7)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp7;})(_tag_fat(_Tmp6,sizeof(void*),2));});else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}{
# 819
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=val->f0;_Tmp1=val->f1;_Tmp0=(enum Cyc_BansheeIf_CvarRecordState*)& val->f2;{void*cv=_Tmp2;struct Cyc_List_List*cnstr=_Tmp1;enum Cyc_BansheeIf_CvarRecordState*state=(enum Cyc_BansheeIf_CvarRecordState*)_Tmp0;
# 822
if((int)*state!=1)return;
*state=2U;
{const char*_Tmp3;const char*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;if(*((int*)cv)==3){_Tmp8=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f1;_Tmp7=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f2;_Tmp6=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f3;_Tmp5=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f4;_Tmp4=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f5;_Tmp3=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cv)->f6;{struct Cyc_Core_Opt*ok=_Tmp8;void*topt=_Tmp7;int id=_Tmp6;void**bv=(void**)_Tmp5;const char*name=_Tmp4;const char*pos=_Tmp3;
# 826
if(!((unsigned)*bv)){
cnstr==0?0:({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp9;})(_tag_fat("cnstr==0",sizeof(char),9U),_tag_fat("bansheeif.cyc",sizeof(char),14U),827U);
Cyc_BansheeIf_add_variable(cv);}
# 832
for(1;(unsigned)cnstr;cnstr=cnstr->tl){
if(Cyc_BansheeIf_internal_add_constraint((void*)cnstr->hd)==0)
({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp9;})(_tag_fat("Failed to reintroduce a constraint",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_Flags_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_BansheeIf__constraint2string((void*)cnstr->hd,0);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_fprintf(Cyc_stderr,_tag_fat("-- Reintro'd constraint : %s \n",sizeof(char),31U),_tag_fat(_TmpA,sizeof(void*),1));});{
# 838
struct Cyc_List_List*cvars=Cyc_BansheeIf_cvars_in_constraint((void*)cnstr->hd);
Cyc_List_iter(Cyc_BansheeIf_reintroduce_cvar,cvars);}}
# 841
*state=3U;
goto _LL8;}}else{
# 844
({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp9;})(_tag_fat("Unexpected type",sizeof(char),16U),_tag_fat(0U,sizeof(void*),0));}_LL8:;}
# 846
return;}}}
# 849
static void Cyc_BansheeIf_copy_cvar_fields(void*src,void*dest){
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=src,_Tmp1.f1=dest;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;int _Tmp8;const char*_Tmp9;const char*_TmpA;void*_TmpB;int _TmpC;void*_TmpD;void*_TmpE;if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_TmpE=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f1;_TmpD=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f2;_TmpC=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f3;_TmpB=(void*)((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f4;_TmpA=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f5;_Tmp9=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f6;_Tmp8=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f0)->f7;_Tmp7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp6=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f2;_Tmp5=(int*)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f3;_Tmp4=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f4;_Tmp3=(const char**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f5;_Tmp2=(const char**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f6;_Tmp1=(int*)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_Tmp0.f1)->f7;{struct Cyc_Core_Opt*ok=_TmpE;void*topt=_TmpD;int id=_TmpC;void*bv=_TmpB;const char*name=_TmpA;const char*pos=_Tmp9;int of=_Tmp8;struct Cyc_Core_Opt**okd=(struct Cyc_Core_Opt**)_Tmp7;void**toptd=(void**)_Tmp6;int*idd=(int*)_Tmp5;void**bvd=(void**)_Tmp4;const char**named=(const char**)_Tmp3;const char**posd=(const char**)_Tmp2;int*ofd=(int*)_Tmp1;
# 852
*okd=ok;*toptd=topt;*idd=id;*bvd=bv;*named=name;*posd=pos;*ofd=of;
goto _LL0;}}else{goto _LL3;}}else{_LL3:
# 855
 goto _LL0;}_LL0:;}struct _tuple18{struct Cyc___cycFILE*f0;int f1;};
# 859
static void Cyc_BansheeIf_persist_cvar_entry(void*ig,struct _tuple17*entry,struct _tuple18*env){
void*_Tmp0;void*_Tmp1;_Tmp1=env->f0;_Tmp0=(int*)& env->f1;{struct Cyc___cycFILE*f=_Tmp1;int*changed=(int*)_Tmp0;
void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=entry->f0;_Tmp3=entry->f1;_Tmp2=(enum Cyc_BansheeIf_CvarRecordState*)& entry->f2;{void*cv=_Tmp4;struct Cyc_List_List*cs=_Tmp3;enum Cyc_BansheeIf_CvarRecordState*state=(enum Cyc_BansheeIf_CvarRecordState*)_Tmp2;
(int)*state!=2?0:({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp5;})(_tag_fat("*state != PERSIST_INPROCESS",sizeof(char),28U),_tag_fat("bansheeif.cyc",sizeof(char),14U),862U);
if((int)*state==0 ||(int)*state==4)
return;
# 866
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_BansheeIf_cvar2string(cv,0);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=({const char*_Tmp9=Cyc_BansheeIf_cvarloc(cv);_tag_fat((void*)_Tmp9,sizeof(char),_get_zero_arr_size_char((void*)_Tmp9,1U));});_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=({const char*_TmpA=Cyc_BansheeIf_cvarorigfat(cv);_tag_fat((void*)_TmpA,sizeof(char),_get_zero_arr_size_char((void*)_TmpA,1U));});_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_fprintf(f,_tag_fat("%s \"%s\", \"%s\" (",sizeof(char),16U),_tag_fat(_Tmp8,sizeof(void*),3));});
# 868
for(1;(unsigned)cs;cs=cs->tl){
# 870
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_BansheeIf__constraint2string((void*)cs->hd,0);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(f,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp6,sizeof(void*),1));});
if((unsigned)cs->tl)
Cyc_fprintf(f,_tag_fat(";",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));{
# 874
struct Cyc_List_List*cvars=Cyc_BansheeIf_cvars_in_constraint((void*)cs->hd);
for(1;(unsigned)cvars;cvars=cvars->tl){
struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){
{struct _tuple17*_Tmp7=({struct _tuple17*(*_Tmp8)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple17*(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup;_Tmp8;})(_check_null(Cyc_BansheeIf_cvarmap),(void*)cvars->hd);void*_Tmp8;void*_Tmp9;void*_TmpA;_TmpA=_Tmp7->f0;_Tmp9=_Tmp7->f1;_Tmp8=(enum Cyc_BansheeIf_CvarRecordState*)& _Tmp7->f2;{void*cv=_TmpA;struct Cyc_List_List*cs=_Tmp9;enum Cyc_BansheeIf_CvarRecordState*state=(enum Cyc_BansheeIf_CvarRecordState*)_Tmp8;
if((int)*state==0){
*state=1U;
*changed=1;}}}
# 877
;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp7)->tag==Cyc_Core_Not_found)
# 883
({int(*_Tmp9)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp9;})(_tag_fat("Constraint refers to an undeclared Cvar",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}};}}}}}
# 898 "bansheeif.cyc"
Cyc_fprintf(f,_tag_fat(")\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));
*state=4U;}}}
# 902
static void Cyc_BansheeIf_constrain_locals(void*cvar,struct _tuple17*val){
if((int)val->f2!=0)
return;
if(Cyc_Tcutil_is_cvar_type(cvar)){
if(Cyc_BansheeIf_internal_add_constraint(({void*_Tmp0=cvar;Cyc_BansheeIf_equality_constraint(_Tmp0,Cyc_Absyn_thinconst());}))==0)
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Failed to add a default constraint on a local constraint variable",sizeof(char),66U),_tag_fat(0U,sizeof(void*),0));}}
# 913
void Cyc_BansheeIf_persist_cvarmap(struct Cyc___cycFILE*f){
Cyc_BansheeIf_discharge_implications();
Cyc_BansheeIf_cvarmap!=0?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("cvarmap != 0",sizeof(char),13U),_tag_fat("bansheeif.cyc",sizeof(char),14U),915U);
Cyc_fprintf(f,_tag_fat("# 1 \"cvars\"\n",sizeof(char),13U),_tag_fat(0U,sizeof(void*),0));{
struct _tuple18*env;env=_cycalloc(sizeof(struct _tuple18)),env->f0=f,env->f1=0;
do{
env->f1=0;
({void(*_Tmp0)(void(*)(void*,struct _tuple17*,struct _tuple18*),struct Cyc_Hashtable_Table*,struct _tuple18*)=(void(*)(void(*)(void*,struct _tuple17*,struct _tuple18*),struct Cyc_Hashtable_Table*,struct _tuple18*))Cyc_Hashtable_iter_c;_Tmp0;})(Cyc_BansheeIf_persist_cvar_entry,_check_null(Cyc_BansheeIf_cvarmap),env);}while(env->f1);
# 923
({void(*_Tmp0)(void(*)(void*,struct _tuple17*),struct Cyc_Hashtable_Table*)=(void(*)(void(*)(void*,struct _tuple17*),struct Cyc_Hashtable_Table*))Cyc_Hashtable_iter;_Tmp0;})(Cyc_BansheeIf_constrain_locals,_check_null(Cyc_BansheeIf_cvarmap));}}
# 926
static void Cyc_BansheeIf_remove_constraint(void*c){
struct Cyc_List_List*cvars=Cyc_BansheeIf_cvars_in_constraint(c);
int tabOk=0;
for(1;(unsigned)cvars;cvars=cvars->tl){
tabOk=0;{
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple17*_Tmp2=({struct _tuple17*(*_Tmp3)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple17*(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup;_Tmp3;})(Cyc_BansheeIf_cvarmap,(void*)cvars->hd);void*_Tmp3;_Tmp3=(struct Cyc_List_List**)& _Tmp2->f1;{struct Cyc_List_List**constr=(struct Cyc_List_List**)_Tmp3;
tabOk=1;
({struct Cyc_List_List*_Tmp4=Cyc_List_delete_cmp(Cyc_BansheeIf_constraint_cmp,*constr,c);*constr=_Tmp4;});}}
# 932
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 937
if(!tabOk)
({int(*_Tmp4)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp4;})(_tag_fat("remove_constraint: Constraint refers to an undeclared Cvar",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));
goto _LL3;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL3:;}}}}}
# 944
void Cyc_BansheeIf_reset_local_cvar(void*cvar){
Cyc_BansheeIf_cvarmap!=0?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("cvarmap != 0",sizeof(char),13U),_tag_fat("bansheeif.cyc",sizeof(char),14U),945U);{
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple17*_Tmp2=({struct _tuple17*(*_Tmp3)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple17*(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup;_Tmp3;})(Cyc_BansheeIf_cvarmap,cvar);void*_Tmp3;_Tmp3=(struct Cyc_List_List**)& _Tmp2->f1;{struct Cyc_List_List**constr=(struct Cyc_List_List**)_Tmp3;
struct Cyc_List_List*l=*constr;*constr=0;
for(1;(unsigned)l;l=l->tl){
Cyc_BansheeIf_remove_constraint((void*)l->hd);}}}
# 947
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 954
({void(*_Tmp4)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*)=({void(*_Tmp5)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*))Cyc_Hashtable_insert;_Tmp5;});struct Cyc_Hashtable_Table*_Tmp5=_check_null(Cyc_BansheeIf_cvarmap);void*_Tmp6=cvar;_Tmp4(_Tmp5,_Tmp6,({struct _tuple17*_Tmp7=_cycalloc(sizeof(struct _tuple17));_Tmp7->f0=cvar,_Tmp7->f1=0,_Tmp7->f2=0U;_Tmp7;}));});
goto _LL3;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL3:;}}}}
# 963
void Cyc_BansheeIf_register_toplevel_cvar(void*cvar){
Cyc_BansheeIf_cvarmap!=0?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("cvarmap != 0",sizeof(char),13U),_tag_fat("bansheeif.cyc",sizeof(char),14U),964U);{
const char*_Tmp0;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)cvar)==3){_Tmp4=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f1;_Tmp3=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f2;_Tmp2=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f3;_Tmp1=(void**)&((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f4;_Tmp0=((struct Cyc_Absyn_Cvar_Absyn_Type_struct*)cvar)->f5;{struct Cyc_Core_Opt*ok=_Tmp4;void**topt=(void**)_Tmp3;int id=_Tmp2;void**bv=(void**)_Tmp1;const char*name=_Tmp0;
# 967
{struct _handler_cons _Tmp5;_push_handler(& _Tmp5);{int _Tmp6=0;if(setjmp(_Tmp5.handler))_Tmp6=1;if(!_Tmp6){
{struct _tuple17*_Tmp7=({struct _tuple17*(*_Tmp8)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple17*(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup;_Tmp8;})(Cyc_BansheeIf_cvarmap,cvar);enum Cyc_BansheeIf_CvarRecordState _Tmp8;void*_Tmp9;void*_TmpA;_TmpA=_Tmp7->f0;_Tmp9=_Tmp7->f1;_Tmp8=_Tmp7->f2;{void*cv=_TmpA;struct Cyc_List_List*constr=_Tmp9;enum Cyc_BansheeIf_CvarRecordState p=_Tmp8;
(int)p?0:({int(*_TmpB)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_TmpB;})(_tag_fat("p",sizeof(char),2U),_tag_fat("bansheeif.cyc",sizeof(char),14U),969U);
if(Cyc_Flags_resolve)
# 974
Cyc_BansheeIf_reintroduce_cvar(cv);
# 976
Cyc_BansheeIf_copy_cvar_fields(cv,cvar);}}
# 968
;_pop_handler();}else{void*_Tmp7=(void*)Cyc_Core_get_exn_thrown();void*_Tmp8;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp7)->tag==Cyc_Core_Not_found){
# 979
Cyc_BansheeIf_add_variable(cvar);
({void(*_Tmp9)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*)=({void(*_TmpA)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*))Cyc_Hashtable_insert;_TmpA;});struct Cyc_Hashtable_Table*_TmpA=_check_null(Cyc_BansheeIf_cvarmap);void*_TmpB=cvar;_Tmp9(_TmpA,_TmpB,({struct _tuple17*_TmpC=_cycalloc(sizeof(struct _tuple17));_TmpC->f0=cvar,_TmpC->f1=0,_TmpC->f2=1U;_TmpC;}));});
goto _LL8;}else{_Tmp8=_Tmp7;{void*exn=_Tmp8;_rethrow(exn);}}_LL8:;}}}
# 983
goto _LL0;}}else{
# 985
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("expected cvar only",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));}_LL0:;}}
# 989
static void Cyc_BansheeIf_default_unconstrained_cvar(void*cv,struct _tuple17*entry,void*def){
if((int)entry->f2==3){
if(Cyc_Tcutil_is_cvar_type(cv)){
if(Cyc_BansheeIf_internal_add_constraint(Cyc_BansheeIf_equality_constraint(cv,def))==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("Resolution failed while applying default to Cvar: ",sizeof(char),51U);_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)cv;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(0U,_tag_fat(_Tmp2,sizeof(void*),2));});}}}
# 998
void Cyc_BansheeIf_resolve_or_default(void*def){
if(Cyc_BansheeIf_discharge_implications())
return;
({void(*_Tmp0)(void(*)(void*,struct _tuple17*,void*),struct Cyc_Hashtable_Table*,void*)=(void(*)(void(*)(void*,struct _tuple17*,void*),struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_iter_c;_Tmp0;})(Cyc_BansheeIf_default_unconstrained_cvar,_check_null(Cyc_BansheeIf_cvarmap),def);
Cyc_BansheeIf_discharge_implications();}struct _tuple19{void*f0;struct Cyc_List_List*f1;};
# 1008
void Cyc_BansheeIf_init_toplevel_cvars(struct Cyc_List_List*assoc){
Cyc_BansheeIf_init_map();
for(1;(unsigned)assoc;assoc=assoc->tl){
struct _tuple19*_Tmp0=(struct _tuple19*)assoc->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*cvar=_Tmp2;struct Cyc_List_List*lc=_Tmp1;
# 1014
({void(*_Tmp3)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*)=({void(*_Tmp4)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple17*))Cyc_Hashtable_insert;_Tmp4;});struct Cyc_Hashtable_Table*_Tmp4=_check_null(Cyc_BansheeIf_cvarmap);void*_Tmp5=cvar;_Tmp3(_Tmp4,_Tmp5,({struct _tuple17*_Tmp6=_cycalloc(sizeof(struct _tuple17));_Tmp6->f0=cvar,_Tmp6->f1=lc,_Tmp6->f2=1U;_Tmp6;}));});}}}
# 1018
int Cyc_BansheeIf_add_all_constraints(struct Cyc_List_List*g){
for(1;(unsigned)g;g=g->tl){
struct _tuple19*_Tmp0=(struct _tuple19*)g->hd;void*_Tmp1;_Tmp1=_Tmp0->f1;{struct Cyc_List_List*c=_Tmp1;
for(1;(unsigned)c;c=c->tl){
if(Cyc_BansheeIf_internal_add_constraint((void*)c->hd)==0)
return -1;}}}
# 1026
if(Cyc_BansheeIf_discharge_implications()==0)
return -1;
return 0;}
# 1031
static void Cyc_BansheeIf_print_cvar(void*c){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_BansheeIf_cvar2string(c,0);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 1034
static void Cyc_BansheeIf_print_cvarmapval(struct _tuple17*val){
struct Cyc_List_List*l=val->f1;for(0;(unsigned)l;l=l->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_BansheeIf_constraint2string((void*)l->hd);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,_tag_fat("%s, ",sizeof(char),5U),_tag_fat(_Tmp1,sizeof(void*),1));});}}
# 1040
static void Cyc_BansheeIf_dump_cvarmap (void){
(unsigned)Cyc_BansheeIf_cvarmap?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("cvarmap",sizeof(char),8U),_tag_fat("bansheeif.cyc",sizeof(char),14U),1041U);
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(struct _tuple17*))=(void(*)(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(struct _tuple17*)))Cyc_Hashtable_print_table_map;_Tmp0;})(Cyc_BansheeIf_cvarmap,Cyc_BansheeIf_print_cvar,Cyc_BansheeIf_print_cvarmapval);}
# 1045
static void Cyc_BansheeIf_flush_one_variable(void*cvar,struct _tuple17*val){
void*_Tmp0;_Tmp0=val->f1;{struct Cyc_List_List*l=_Tmp0;
for(1;(unsigned)l;l=l->tl){
if(Cyc_BansheeIf_internal_add_constraint((void*)l->hd)==0)
({int(*_Tmp1)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp1;})(_tag_fat("Inconsistent constraint graph while flushing",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));}}}
# 1054
void Cyc_BansheeIf_flush_all_constraints (void){
if(!((unsigned)Cyc_BansheeIf_cvarmap))
return;
# 1058
({void(*_Tmp0)(void(*)(void*,struct _tuple17*),struct Cyc_Hashtable_Table*)=(void(*)(void(*)(void*,struct _tuple17*),struct Cyc_Hashtable_Table*))Cyc_Hashtable_iter;_Tmp0;})(Cyc_BansheeIf_flush_one_variable,Cyc_BansheeIf_cvarmap);
Cyc_BansheeIf_discharge_implications();}

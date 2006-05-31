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
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 290 "cycboot.h"
extern int isdigit(int);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 137 "string.h"
extern struct _fat_ptr Cyc_implode(struct Cyc_List_List*);struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 932 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 957
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;
# 959
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;
# 961
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 968
extern void*Cyc_Absyn_al_qual_type;
# 972
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 1023
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1027
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 41 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 256 "tcutil.h"
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 57 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_eko;struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 98 "tcenv.h"
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 31 "formatstr.cyc"
static void*Cyc_Formatstr_err_null(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 33
Cyc_Warn_verr(loc,fmt,ap);
return 0;}struct _tuple11{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;char f4;int f5;};
# 43
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _RegionHandle*r,struct _fat_ptr s,int i){int _T0;unsigned long _T1;unsigned long _T2;int _T3;unsigned long _T4;unsigned long _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;int _T9;char _TA;int _TB;struct Cyc_List_List*_TC;struct _RegionHandle*_TD;char _TE;int _TF;unsigned long _T10;unsigned long _T11;struct _fat_ptr _T12;unsigned char*_T13;const char*_T14;int _T15;char _T16;int _T17;struct Cyc_List_List*_T18;struct _RegionHandle*_T19;char _T1A;int _T1B;unsigned long _T1C;unsigned long _T1D;struct _fat_ptr _T1E;unsigned char*_T1F;const char*_T20;int _T21;char _T22;int _T23;int _T24;struct Cyc_List_List*_T25;struct _RegionHandle*_T26;char _T27;int _T28;unsigned long _T29;unsigned long _T2A;struct _fat_ptr _T2B;unsigned char*_T2C;const char*_T2D;int _T2E;char _T2F;int _T30;struct Cyc_List_List*_T31;struct _RegionHandle*_T32;char _T33;int _T34;unsigned long _T35;unsigned long _T36;struct _fat_ptr _T37;unsigned char*_T38;const char*_T39;int _T3A;char _T3B;int _T3C;struct Cyc_List_List*_T3D;struct _RegionHandle*_T3E;char _T3F;int _T40;unsigned long _T41;unsigned long _T42;struct _fat_ptr _T43;unsigned char*_T44;const char*_T45;int _T46;char _T47;int _T48;int _T49;struct Cyc_List_List*_T4A;struct _RegionHandle*_T4B;char _T4C;int _T4D;unsigned long _T4E;unsigned long _T4F;struct _fat_ptr _T50;unsigned char*_T51;const char*_T52;int _T53;char _T54;int _T55;struct Cyc_List_List*_T56;struct _RegionHandle*_T57;char _T58;int _T59;unsigned long _T5A;unsigned long _T5B;struct _fat_ptr _T5C;unsigned char*_T5D;const char*_T5E;int _T5F;char _T60;int _T61;struct Cyc_List_List*_T62;struct _RegionHandle*_T63;char _T64;struct Cyc_List_List*_T65;struct _RegionHandle*_T66;char _T67;int _T68;unsigned long _T69;unsigned long _T6A;struct _fat_ptr _T6B;unsigned char*_T6C;const char*_T6D;int _T6E;char _T6F;int _T70;struct Cyc_List_List*_T71;struct _RegionHandle*_T72;char _T73;struct Cyc_List_List*_T74;struct _RegionHandle*_T75;char _T76;int _T77;unsigned long _T78;unsigned long _T79;struct _fat_ptr _T7A;unsigned char*_T7B;const char*_T7C;int _T7D;char _T7E;int _T7F;struct Cyc_Core_Opt*_T80;struct _RegionHandle*_T81;struct _tuple11*_T82;struct _RegionHandle*_T83;
# 47
unsigned long len=Cyc_strlen(s);
if(i < 0)goto _TL2;else{goto _TL3;}_TL3: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 >= _T2)goto _TL2;else{goto _TL0;}_TL2: return 0;_TL0: {
# 51
struct Cyc_List_List*flags=0;
char c=' ';
_TL7: _T3=i;_T4=(unsigned long)_T3;_T5=len;if(_T4 < _T5)goto _TL5;else{goto _TL6;}
_TL5: _T6=s;_T7=_T6.curr;_T8=(const char*)_T7;_T9=i;c=_T8[_T9];_TA=c;_TB=(int)_TA;switch(_TB){case 43: goto _LL4;case 45: _LL4: goto _LL6;case 32: _LL6: goto _LL8;case 35: _LL8: goto _LLA;case 48: _LLA: _TD=r;{struct Cyc_List_List*_T84=_region_malloc(_TD,0U,sizeof(struct Cyc_List_List));_TE=c;
# 60
_T84->hd=(void*)_TE;_T84->tl=flags;_TC=(struct Cyc_List_List*)_T84;}flags=_TC;goto _TL4;default: goto _LL0;}_LL0: goto _TL6;_TL4:
# 53
 i=i + 1;goto _TL7;_TL6: _TF=i;_T10=(unsigned long)_TF;_T11=len;
# 65
if(_T10 < _T11)goto _TL9;return 0;_TL9:
 flags=Cyc_List_imp_rev(flags);{
# 69
struct Cyc_List_List*width=0;_T12=s;_T13=_T12.curr;_T14=(const char*)_T13;_T15=i;
c=_T14[_T15];_T16=c;_T17=(int)_T16;
if(_T17!=42)goto _TLB;_T19=r;{struct Cyc_List_List*_T84=_region_malloc(_T19,0U,sizeof(struct Cyc_List_List));_T1A=c;
_T84->hd=(void*)_T1A;_T84->tl=width;_T18=(struct Cyc_List_List*)_T84;}width=_T18;
i=i + 1;goto _TLC;
# 75
_TLB: _TL10: _T1B=i;_T1C=(unsigned long)_T1B;_T1D=len;if(_T1C < _T1D)goto _TLE;else{goto _TLF;}
_TLE: _T1E=s;_T1F=_T1E.curr;_T20=(const char*)_T1F;_T21=i;c=_T20[_T21];_T22=c;_T23=(int)_T22;_T24=
isdigit(_T23);if(!_T24)goto _TL11;_T26=r;{struct Cyc_List_List*_T84=_region_malloc(_T26,0U,sizeof(struct Cyc_List_List));_T27=c;_T84->hd=(void*)_T27;_T84->tl=width;_T25=(struct Cyc_List_List*)_T84;}width=_T25;goto _TL12;
_TL11: goto _TLF;_TL12:
# 75
 i=i + 1;goto _TL10;_TLF: _TLC: _T28=i;_T29=(unsigned long)_T28;_T2A=len;
# 81
if(_T29 < _T2A)goto _TL13;return 0;_TL13:
 width=Cyc_List_imp_rev(width);{
# 85
struct Cyc_List_List*precision=0;_T2B=s;_T2C=_T2B.curr;_T2D=(const char*)_T2C;_T2E=i;
c=_T2D[_T2E];_T2F=c;_T30=(int)_T2F;
if(_T30!=46)goto _TL15;_T32=r;{struct Cyc_List_List*_T84=_region_malloc(_T32,0U,sizeof(struct Cyc_List_List));_T33=c;
_T84->hd=(void*)_T33;_T84->tl=precision;_T31=(struct Cyc_List_List*)_T84;}precision=_T31;
i=i + 1;_T34=i;_T35=(unsigned long)_T34;_T36=len;if(_T35 < _T36)goto _TL17;return 0;_TL17: _T37=s;_T38=_T37.curr;_T39=(const char*)_T38;_T3A=i;
c=_T39[_T3A];_T3B=c;_T3C=(int)_T3B;
if(_T3C!=42)goto _TL19;_T3E=r;{struct Cyc_List_List*_T84=_region_malloc(_T3E,0U,sizeof(struct Cyc_List_List));_T3F=c;
_T84->hd=(void*)_T3F;_T84->tl=precision;_T3D=(struct Cyc_List_List*)_T84;}precision=_T3D;
i=i + 1;goto _TL1A;
# 95
_TL19: _TL1E: _T40=i;_T41=(unsigned long)_T40;_T42=len;if(_T41 < _T42)goto _TL1C;else{goto _TL1D;}
_TL1C: _T43=s;_T44=_T43.curr;_T45=(const char*)_T44;_T46=i;c=_T45[_T46];_T47=c;_T48=(int)_T47;_T49=
isdigit(_T48);if(!_T49)goto _TL1F;_T4B=r;{struct Cyc_List_List*_T84=_region_malloc(_T4B,0U,sizeof(struct Cyc_List_List));_T4C=c;_T84->hd=(void*)_T4C;_T84->tl=precision;_T4A=(struct Cyc_List_List*)_T84;}precision=_T4A;goto _TL20;
_TL1F: goto _TL1D;_TL20:
# 95
 i=i + 1;goto _TL1E;_TL1D: _TL1A: goto _TL16;_TL15: _TL16: _T4D=i;_T4E=(unsigned long)_T4D;_T4F=len;
# 101
if(_T4E < _T4F)goto _TL21;return 0;_TL21:
 precision=Cyc_List_imp_rev(precision);{
# 106
struct Cyc_List_List*lenmod=0;_T50=s;_T51=_T50.curr;_T52=(const char*)_T51;_T53=i;
c=_T52[_T53];_T54=c;_T55=(int)_T54;switch(_T55){case 104: _T57=r;{struct Cyc_List_List*_T84=_region_malloc(_T57,0U,sizeof(struct Cyc_List_List));_T58=c;
# 110
_T84->hd=(void*)_T58;_T84->tl=lenmod;_T56=(struct Cyc_List_List*)_T84;}lenmod=_T56;
i=i + 1;_T59=i;_T5A=(unsigned long)_T59;_T5B=len;if(_T5A < _T5B)goto _TL24;return 0;_TL24: _T5C=s;_T5D=_T5C.curr;_T5E=(const char*)_T5D;_T5F=i;
c=_T5E[_T5F];_T60=c;_T61=(int)_T60;
if(_T61!=104)goto _TL26;_T63=r;{struct Cyc_List_List*_T84=_region_malloc(_T63,0U,sizeof(struct Cyc_List_List));_T64=c;_T84->hd=(void*)_T64;_T84->tl=lenmod;_T62=(struct Cyc_List_List*)_T84;}lenmod=_T62;i=i + 1;goto _TL27;_TL26: _TL27: goto _LLD;case 108: _T66=r;{struct Cyc_List_List*_T84=_region_malloc(_T66,0U,sizeof(struct Cyc_List_List));_T67=c;
# 116
_T84->hd=(void*)_T67;_T84->tl=lenmod;_T65=(struct Cyc_List_List*)_T84;}lenmod=_T65;
i=i + 1;_T68=i;_T69=(unsigned long)_T68;_T6A=len;if(_T69 < _T6A)goto _TL28;return 0;_TL28: _T6B=s;_T6C=_T6B.curr;_T6D=(const char*)_T6C;_T6E=i;
c=_T6D[_T6E];_T6F=c;_T70=(int)_T6F;
if(_T70!=108)goto _TL2A;_T72=r;{struct Cyc_List_List*_T84=_region_malloc(_T72,0U,sizeof(struct Cyc_List_List));_T73=c;_T84->hd=(void*)_T73;_T84->tl=lenmod;_T71=(struct Cyc_List_List*)_T84;}lenmod=_T71;i=i + 1;goto _TL2B;_TL2A: _TL2B: goto _LLD;case 106: goto _LL15;case 122: _LL15: goto _LL17;case 116: _LL17: goto _LL19;case 76: _LL19: _T75=r;{struct Cyc_List_List*_T84=_region_malloc(_T75,0U,sizeof(struct Cyc_List_List));_T76=c;
# 125
_T84->hd=(void*)_T76;_T84->tl=lenmod;_T74=(struct Cyc_List_List*)_T84;}lenmod=_T74;
i=i + 1;goto _LLD;default: goto _LLD;}_LLD: _T77=i;_T78=(unsigned long)_T77;_T79=len;
# 130
if(_T78 < _T79)goto _TL2C;return 0;_TL2C:
 lenmod=Cyc_List_imp_rev(lenmod);_T7A=s;_T7B=_T7A.curr;_T7C=(const char*)_T7B;_T7D=i;
# 134
c=_T7C[_T7D];_T7E=c;_T7F=(int)_T7E;switch(_T7F){case 100: goto _LL20;case 105: _LL20: goto _LL22;case 111: _LL22: goto _LL24;case 117: _LL24: goto _LL26;case 120: _LL26: goto _LL28;case 88: _LL28: goto _LL2A;case 102: _LL2A: goto _LL2C;case 70: _LL2C: goto _LL2E;case 101: _LL2E: goto _LL30;case 69: _LL30: goto _LL32;case 103: _LL32: goto _LL34;case 71: _LL34: goto _LL36;case 97: _LL36: goto _LL38;case 65: _LL38: goto _LL3A;case 99: _LL3A: goto _LL3C;case 115: _LL3C: goto _LL3E;case 112: _LL3E: goto _LL40;case 110: _LL40: goto _LL42;case 37: _LL42: goto _LL1C;default:
# 155
 return 0;}_LL1C: _T81=r;{struct Cyc_Core_Opt*_T84=_region_malloc(_T81,0U,sizeof(struct Cyc_Core_Opt));_T83=r;{struct _tuple11*_T85=_region_malloc(_T83,0U,sizeof(struct _tuple11));
# 166 "formatstr.cyc"
_T85->f0=flags;_T85->f1=width;_T85->f2=precision;_T85->f3=lenmod;_T85->f4=c;_T85->f5=i + 1;_T82=(struct _tuple11*)_T85;}_T84->v=_T82;_T80=(struct Cyc_Core_Opt*)_T84;}return _T80;}}}}}
# 169
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;int _T6;char _T7;int _T8;struct _RegionHandle*_T9;struct _fat_ptr _TA;int _TB;struct Cyc_List_List*(*_TC)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Core_Opt*_T10;void*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;int _T17;int _T18;int _T19;struct Cyc_List_List*_T1A;struct Cyc_Int_pa_PrintArg_struct _T1B;int _T1C;void**_T1D;struct Cyc_List_List*(*_T1E)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;void*_T26;int _T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;struct Cyc_List_List*_T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;void*_T32;struct Cyc_List_List*_T33;void*_T34;int _T35;int _T36;struct Cyc_List_List*_T37;char _T38;int _T39;struct Cyc_List_List*_T3A;void*_T3B;int _T3C;struct Cyc_List_List*_T3D;struct Cyc_Int_pa_PrintArg_struct _T3E;char _T3F;int _T40;void**_T41;struct Cyc_List_List*(*_T42)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T43;struct _fat_ptr _T44;struct _fat_ptr _T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;struct Cyc_List_List*_T48;struct Cyc_List_List*_T49;struct Cyc_List_List*_T4A;void*_T4B;int _T4C;struct Cyc_List_List*_T4D;void*_T4E;int _T4F;struct Cyc_List_List*_T50;struct Cyc_List_List*_T51;struct Cyc_List_List*_T52;struct Cyc_List_List*_T53;struct Cyc_List_List*_T54;struct Cyc_List_List*_T55;void*_T56;struct Cyc_List_List*_T57;void*_T58;int _T59;int _T5A;struct Cyc_List_List*_T5B;struct Cyc_String_pa_PrintArg_struct _T5C;struct Cyc_Int_pa_PrintArg_struct _T5D;char _T5E;int _T5F;void**_T60;void**_T61;struct Cyc_List_List*(*_T62)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T63;struct _fat_ptr _T64;struct _fat_ptr _T65;struct Cyc_List_List*_T66;struct Cyc_List_List*_T67;void*_T68;int _T69;struct Cyc_List_List*(*_T6A)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T6B;struct _fat_ptr _T6C;struct _fat_ptr _T6D;struct Cyc_List_List*_T6E;struct Cyc_List_List*_T6F;struct Cyc_List_List*_T70;struct Cyc_List_List*_T71;struct Cyc_List_List*_T72;void*_T73;int _T74;struct Cyc_List_List*_T75;void*_T76;int _T77;struct Cyc_List_List*_T78;struct Cyc_List_List*_T79;struct Cyc_List_List*_T7A;struct Cyc_List_List*_T7B;struct Cyc_List_List*_T7C;struct Cyc_List_List*_T7D;void*_T7E;struct Cyc_List_List*_T7F;void*_T80;int _T81;int _T82;struct Cyc_List_List*_T83;struct Cyc_String_pa_PrintArg_struct _T84;struct Cyc_Int_pa_PrintArg_struct _T85;char _T86;int _T87;void**_T88;void**_T89;struct Cyc_List_List*(*_T8A)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T8B;struct _fat_ptr _T8C;struct _fat_ptr _T8D;struct Cyc_List_List*_T8E;struct Cyc_List_List*_T8F;struct Cyc_List_List*_T90;struct Cyc_List_List*_T91;struct Cyc_List_List*_T92;struct Cyc_List_List*_T93;void*_T94;int _T95;struct Cyc_List_List*_T96;struct Cyc_List_List*_T97;struct Cyc_String_pa_PrintArg_struct _T98;struct Cyc_Int_pa_PrintArg_struct _T99;char _T9A;int _T9B;void**_T9C;void**_T9D;struct Cyc_List_List*(*_T9E)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T9F;struct _fat_ptr _TA0;struct _fat_ptr _TA1;struct Cyc_List_List*_TA2;void*_TA3;int _TA4;struct Cyc_List_List*_TA5;void*_TA6;int _TA7;struct Cyc_List_List*_TA8;struct Cyc_Int_pa_PrintArg_struct _TA9;struct Cyc_List_List*_TAA;void*_TAB;int _TAC;void**_TAD;struct Cyc_List_List*(*_TAE)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TAF;struct _fat_ptr _TB0;struct _fat_ptr _TB1;struct Cyc_List_List*_TB2;struct Cyc_List_List*_TB3;struct Cyc_String_pa_PrintArg_struct _TB4;void**_TB5;struct Cyc_List_List*(*_TB6)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TB7;struct _fat_ptr _TB8;struct _fat_ptr _TB9;struct Cyc_List_List*_TBA;struct Cyc_String_pa_PrintArg_struct _TBB;void**_TBC;struct Cyc_List_List*(*_TBD)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TBE;struct _fat_ptr _TBF;struct _fat_ptr _TC0;struct Cyc_List_List*_TC1;struct Cyc_List_List*_TC2;void*_TC3;int _TC4;struct Cyc_List_List*(*_TC5)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TC6;struct _fat_ptr _TC7;struct _fat_ptr _TC8;struct Cyc_List_List*_TC9;struct Cyc_List_List*(*_TCA)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TCB;struct _fat_ptr _TCC;struct _fat_ptr _TCD;int _TCE;void*_TCF;struct Cyc_Core_Opt*_TD0;struct Cyc_Core_Opt*_TD1;struct Cyc_Core_Opt*_TD2;void*_TD3;void*_TD4;struct Cyc_Absyn_Tqual _TD5;void*_TD6;void*_TD7;void*_TD8;struct Cyc_Core_Opt*_TD9;struct Cyc_Core_Opt*_TDA;struct Cyc_Core_Opt*_TDB;void*_TDC;void*_TDD;struct Cyc_Absyn_Tqual _TDE;void*_TDF;void*_TE0;struct Cyc_List_List*_TE1;struct Cyc_List_List*_TE2;struct Cyc_List_List*_TE3;void*_TE4;int _TE5;struct Cyc_List_List*_TE6;void*_TE7;int _TE8;struct Cyc_List_List*_TE9;struct Cyc_Int_pa_PrintArg_struct _TEA;struct Cyc_List_List*_TEB;void*_TEC;int _TED;void**_TEE;struct Cyc_List_List*(*_TEF)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TF0;struct _fat_ptr _TF1;struct _fat_ptr _TF2;struct Cyc_List_List*_TF3;struct Cyc_List_List*_TF4;struct Cyc_String_pa_PrintArg_struct _TF5;void**_TF6;struct Cyc_List_List*(*_TF7)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TF8;struct _fat_ptr _TF9;struct _fat_ptr _TFA;struct Cyc_List_List*_TFB;struct Cyc_List_List*_TFC;struct Cyc_List_List*_TFD;struct Cyc_List_List*_TFE;void*_TFF;int _T100;struct Cyc_List_List*_T101;void*_T102;int _T103;struct Cyc_List_List*_T104;struct Cyc_List_List*_T105;struct Cyc_List_List*_T106;struct Cyc_List_List*_T107;struct Cyc_List_List*_T108;struct Cyc_List_List*_T109;void*_T10A;struct Cyc_List_List*_T10B;void*_T10C;int _T10D;int _T10E;struct Cyc_List_List*_T10F;struct Cyc_String_pa_PrintArg_struct _T110;struct Cyc_Int_pa_PrintArg_struct _T111;char _T112;int _T113;void**_T114;void**_T115;struct Cyc_List_List*(*_T116)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T117;struct _fat_ptr _T118;struct _fat_ptr _T119;void*_T11A;struct Cyc_Core_Opt*_T11B;struct Cyc_Core_Opt*_T11C;struct Cyc_Core_Opt*_T11D;void*_T11E;void*_T11F;struct Cyc_Absyn_Tqual _T120;void*_T121;void*_T122;struct Cyc_List_List*_T123;struct Cyc_List_List*_T124;struct Cyc_String_pa_PrintArg_struct _T125;void**_T126;struct Cyc_List_List*(*_T127)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T128;struct _fat_ptr _T129;struct _fat_ptr _T12A;struct Cyc_List_List*_T12B;struct Cyc_String_pa_PrintArg_struct _T12C;void**_T12D;struct Cyc_List_List*(*_T12E)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T12F;struct _fat_ptr _T130;struct _fat_ptr _T131;struct Cyc_List_List*_T132;struct Cyc_String_pa_PrintArg_struct _T133;void**_T134;struct Cyc_List_List*(*_T135)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T136;struct _fat_ptr _T137;struct _fat_ptr _T138;struct Cyc_List_List*_T139;struct Cyc_String_pa_PrintArg_struct _T13A;void**_T13B;struct Cyc_List_List*(*_T13C)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T13D;struct _fat_ptr _T13E;struct _fat_ptr _T13F;
# 172
unsigned long len=Cyc_strlen(s);
struct Cyc_List_List*typs=0;
int i;struct _RegionHandle _T140=_new_region(0U,"temp");struct _RegionHandle*temp=& _T140;_push_region(temp);
# 176
i=0;_TL32: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 < _T2)goto _TL30;else{goto _TL31;}
_TL30: _T3=s;_T4=_T3.curr;_T5=(const char*)_T4;_T6=i;_T7=_T5[_T6];_T8=(int)_T7;if(_T8==37)goto _TL33;goto _TL2F;_TL33: _T9=temp;_TA=s;_TB=i + 1;{
struct Cyc_Core_Opt*cs=Cyc_Formatstr_parse_conversionspecification(_T9,_TA,_TB);
if(cs!=0)goto _TL35;{
struct Cyc_List_List*(*_T141)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TC=_T141;}_TD=loc;_TE=_tag_fat("bad format string",sizeof(char),18U);_TF=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T141=_TC(_TD,_TE,_TF);_npop_handler(0);return _T141;}_TL35: _T10=cs;_T11=_T10->v;{
struct _tuple11*_T141=(struct _tuple11*)_T11;int _T142;char _T143;struct Cyc_List_List*_T144;struct Cyc_List_List*_T145;struct Cyc_List_List*_T146;struct Cyc_List_List*_T147;{struct _tuple11 _T148=*_T141;_T147=_T148.f0;_T146=_T148.f1;_T145=_T148.f2;_T144=_T148.f3;_T143=_T148.f4;_T142=_T148.f5;}{struct Cyc_List_List*flags=_T147;struct Cyc_List_List*width=_T146;struct Cyc_List_List*precision=_T145;struct Cyc_List_List*lenmod=_T144;char c=_T143;int j=_T142;
i=j - 1;{int _T148;if(lenmod==0)goto _TL37;_T12=lenmod;_T13=(struct Cyc_List_List*)_T12;_T14=_T13->tl;if(_T14!=0)goto _TL39;_T15=lenmod;{struct Cyc_List_List _T149=*_T15;_T16=_T149.hd;_T148=(int)_T16;}_T17=(int)_T148;if(_T17==106)goto _TL3D;else{goto _TL3F;}_TL3F: _T18=(int)_T148;if(_T18==122)goto _TL3D;else{goto _TL3E;}_TL3E: _T19=(int)_T148;if(_T19==116)goto _TL3D;else{goto _TL3B;}_TL3D:{int x=_T148;{struct Cyc_Int_pa_PrintArg_struct _T149;_T149.tag=1;_T1C=x;
# 187
_T149.f1=(unsigned long)_T1C;_T1B=_T149;}{struct Cyc_Int_pa_PrintArg_struct _T149=_T1B;void*_T14A[1];_T1D=_T14A + 0;*_T1D=& _T149;{struct Cyc_List_List*(*_T14B)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T1E=_T14B;}_T1F=loc;_T20=_tag_fat("length modifier '%c' is not supported",sizeof(char),38U);_T21=_tag_fat(_T14A,sizeof(void*),1);_T1A=_T1E(_T1F,_T20,_T21);}{struct Cyc_List_List*_T149=_T1A;_npop_handler(0);return _T149;}}_TL3B: goto _LL6;_TL39: goto _LL6;_TL37: _LL6: goto _LL3;_LL3:;}{int _T148;if(width==0)goto _TL40;_T22=width;_T23=(struct Cyc_List_List*)_T22;_T24=_T23->tl;if(_T24!=0)goto _TL42;_T25=width;{struct Cyc_List_List _T149=*_T25;_T26=_T149.hd;_T148=(int)_T26;}_T27=(int)_T148;if(_T27!=42)goto _TL44;{int x=_T148;{struct Cyc_List_List*_T149=_cycalloc(sizeof(struct Cyc_List_List));
# 191
_T149->hd=Cyc_Absyn_sint_type;_T149->tl=typs;_T28=(struct Cyc_List_List*)_T149;}typs=_T28;goto _LL8;}_TL44: goto _LLB;_TL42: goto _LLB;_TL40: _LLB: goto _LL8;_LL8:;}{int _T148;int _T149;if(precision==0)goto _TL46;_T29=precision;_T2A=(struct Cyc_List_List*)_T29;_T2B=_T2A->tl;if(_T2B==0)goto _TL48;_T2C=precision;_T2D=(struct Cyc_List_List*)_T2C;_T2E=_T2D->tl;_T2F=(struct Cyc_List_List*)_T2E;_T30=_T2F->tl;if(_T30!=0)goto _TL4A;_T31=precision;{struct Cyc_List_List _T14A=*_T31;_T32=_T14A.hd;_T149=(int)_T32;_T33=_T14A.tl;{struct Cyc_List_List _T14B=*_T33;_T34=_T14B.hd;_T148=(int)_T34;}}_T35=(int)_T149;if(_T35!=46)goto _TL4C;_T36=(int)_T148;if(_T36!=42)goto _TL4C;{int x=_T149;int y=_T148;{struct Cyc_List_List*_T14A=_cycalloc(sizeof(struct Cyc_List_List));
# 196
_T14A->hd=Cyc_Absyn_sint_type;_T14A->tl=typs;_T37=(struct Cyc_List_List*)_T14A;}typs=_T37;goto _LLD;}_TL4C: goto _LL10;_TL4A: goto _LL10;_TL48: goto _LL10;_TL46: _LL10: goto _LLD;_LLD:;}{
# 199
void*t;_T38=c;_T39=(int)_T38;switch(_T39){case 100: goto _LL16;case 105: _LL16:{
# 203
struct Cyc_List_List*f=flags;_TL52: if(f!=0)goto _TL50;else{goto _TL51;}
_TL50: _T3A=f;_T3B=_T3A->hd;_T3C=(int)_T3B;if(_T3C!=35)goto _TL53;{struct Cyc_Int_pa_PrintArg_struct _T148;_T148.tag=1;_T3F=c;_T40=(int)_T3F;
_T148.f1=(unsigned long)_T40;_T3E=_T148;}{struct Cyc_Int_pa_PrintArg_struct _T148=_T3E;void*_T149[1];_T41=_T149 + 0;*_T41=& _T148;{struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T42=_T14A;}_T43=loc;_T44=_tag_fat("flag '#' is not valid with %%%c",sizeof(char),32U);_T45=_tag_fat(_T149,sizeof(void*),1);_T3D=_T42(_T43,_T44,_T45);}{struct Cyc_List_List*_T148=_T3D;_npop_handler(0);return _T148;}_TL53: _T46=f;
# 203
f=_T46->tl;goto _TL52;_TL51:;}{int _T148;int _T149;if(lenmod!=0)goto _TL55;
# 207
t=Cyc_Absyn_sint_type;goto _LL3B;_TL55: _T47=lenmod;_T48=(struct Cyc_List_List*)_T47;_T49=_T48->tl;if(_T49!=0)goto _TL57;_T4A=lenmod;{struct Cyc_List_List _T14A=*_T4A;_T4B=_T14A.hd;_T149=(int)_T4B;}_T4C=(int)_T149;if(_T4C!=108)goto _TL59;{int x=_T149;
t=Cyc_Absyn_slong_type;goto _LL3B;}_TL59: _T4D=lenmod;{struct Cyc_List_List _T14A=*_T4D;_T4E=_T14A.hd;_T149=(int)_T4E;}_T4F=(int)_T149;if(_T4F!=104)goto _TL5B;{int x=_T149;
t=Cyc_Absyn_sshort_type;goto _LL3B;}_TL5B: goto _LL44;_TL57: _T50=lenmod;_T51=(struct Cyc_List_List*)_T50;_T52=_T51->tl;_T53=(struct Cyc_List_List*)_T52;_T54=_T53->tl;if(_T54!=0)goto _TL5D;_T55=lenmod;{struct Cyc_List_List _T14A=*_T55;_T56=_T14A.hd;_T149=(int)_T56;_T57=_T14A.tl;{struct Cyc_List_List _T14B=*_T57;_T58=_T14B.hd;_T148=(int)_T58;}}_T59=(int)_T149;if(_T59!=104)goto _TL5F;_T5A=(int)_T148;if(_T5A!=104)goto _TL5F;{int x=_T149;int y=_T148;
# 211
t=Cyc_Absyn_schar_type;goto _LL3B;}_TL5F: goto _LL44;_TL5D: _LL44:{struct Cyc_String_pa_PrintArg_struct _T14A;_T14A.tag=0;
# 214
_T14A.f1=Cyc_implode(lenmod);_T5C=_T14A;}{struct Cyc_String_pa_PrintArg_struct _T14A=_T5C;{struct Cyc_Int_pa_PrintArg_struct _T14B;_T14B.tag=1;_T5E=c;_T5F=(int)_T5E;_T14B.f1=(unsigned long)_T5F;_T5D=_T14B;}{struct Cyc_Int_pa_PrintArg_struct _T14B=_T5D;void*_T14C[2];_T60=_T14C + 0;*_T60=& _T14A;_T61=_T14C + 1;*_T61=& _T14B;{
# 213
struct Cyc_List_List*(*_T14D)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T62=_T14D;}_T63=loc;_T64=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T65=_tag_fat(_T14C,sizeof(void*),2);_T5B=_T62(_T63,_T64,_T65);}}{struct Cyc_List_List*_T14A=_T5B;_npop_handler(0);return _T14A;}_LL3B:;}{struct Cyc_List_List*_T148=_cycalloc(sizeof(struct Cyc_List_List));
# 216
_T148->hd=t;_T148->tl=typs;_T66=(struct Cyc_List_List*)_T148;}typs=_T66;goto _LL12;case 117:{
# 219
struct Cyc_List_List*f=flags;_TL64: if(f!=0)goto _TL62;else{goto _TL63;}
_TL62: _T67=f;_T68=_T67->hd;_T69=(int)_T68;if(_T69!=35)goto _TL65;{
struct Cyc_List_List*(*_T148)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T6A=_T148;}_T6B=loc;_T6C=_tag_fat("Flag '#' not valid with %%u",sizeof(char),28U);_T6D=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T148=_T6A(_T6B,_T6C,_T6D);_npop_handler(0);return _T148;}_TL65: _T6E=f;
# 219
f=_T6E->tl;goto _TL64;_TL63:;}goto _LL1A;case 111: _LL1A: goto _LL1C;case 120: _LL1C: goto _LL1E;case 88: _LL1E:{int _T148;int _T149;if(lenmod!=0)goto _TL67;
# 227
t=Cyc_Absyn_uint_type;goto _LL46;_TL67: _T6F=lenmod;_T70=(struct Cyc_List_List*)_T6F;_T71=_T70->tl;if(_T71!=0)goto _TL69;_T72=lenmod;{struct Cyc_List_List _T14A=*_T72;_T73=_T14A.hd;_T149=(int)_T73;}_T74=(int)_T149;if(_T74!=108)goto _TL6B;{int x=_T149;
t=Cyc_Absyn_ulong_type;goto _LL46;}_TL6B: _T75=lenmod;{struct Cyc_List_List _T14A=*_T75;_T76=_T14A.hd;_T149=(int)_T76;}_T77=(int)_T149;if(_T77!=104)goto _TL6D;{int x=_T149;
t=Cyc_Absyn_ushort_type;goto _LL46;}_TL6D: goto _LL4F;_TL69: _T78=lenmod;_T79=(struct Cyc_List_List*)_T78;_T7A=_T79->tl;_T7B=(struct Cyc_List_List*)_T7A;_T7C=_T7B->tl;if(_T7C!=0)goto _TL6F;_T7D=lenmod;{struct Cyc_List_List _T14A=*_T7D;_T7E=_T14A.hd;_T149=(int)_T7E;_T7F=_T14A.tl;{struct Cyc_List_List _T14B=*_T7F;_T80=_T14B.hd;_T148=(int)_T80;}}_T81=(int)_T149;if(_T81!=104)goto _TL71;_T82=(int)_T148;if(_T82!=104)goto _TL71;{int x=_T149;int y=_T148;
# 231
t=Cyc_Absyn_uchar_type;goto _LL46;}_TL71: goto _LL4F;_TL6F: _LL4F:{struct Cyc_String_pa_PrintArg_struct _T14A;_T14A.tag=0;
# 235
_T14A.f1=Cyc_implode(lenmod);_T84=_T14A;}{struct Cyc_String_pa_PrintArg_struct _T14A=_T84;{struct Cyc_Int_pa_PrintArg_struct _T14B;_T14B.tag=1;_T86=c;_T87=(int)_T86;_T14B.f1=(unsigned long)_T87;_T85=_T14B;}{struct Cyc_Int_pa_PrintArg_struct _T14B=_T85;void*_T14C[2];_T88=_T14C + 0;*_T88=& _T14A;_T89=_T14C + 1;*_T89=& _T14B;{
# 234
struct Cyc_List_List*(*_T14D)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T8A=_T14D;}_T8B=loc;_T8C=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T8D=_tag_fat(_T14C,sizeof(void*),2);_T83=_T8A(_T8B,_T8C,_T8D);}}{struct Cyc_List_List*_T14A=_T83;_npop_handler(0);return _T14A;}_LL46:;}{struct Cyc_List_List*_T148=_cycalloc(sizeof(struct Cyc_List_List));
# 237
_T148->hd=t;_T148->tl=typs;_T8E=(struct Cyc_List_List*)_T148;}typs=_T8E;goto _LL12;case 102: goto _LL22;case 70: _LL22: goto _LL24;case 101: _LL24: goto _LL26;case 69: _LL26: goto _LL28;case 103: _LL28: goto _LL2A;case 71: _LL2A: goto _LL2C;case 97: _LL2C: goto _LL2E;case 65: _LL2E:{int _T148;if(lenmod!=0)goto _TL73;{struct Cyc_List_List*_T149=_cycalloc(sizeof(struct Cyc_List_List));
# 255
_T149->hd=Cyc_Absyn_double_type;_T149->tl=typs;_T8F=(struct Cyc_List_List*)_T149;}typs=_T8F;goto _LL51;_TL73: _T90=lenmod;_T91=(struct Cyc_List_List*)_T90;_T92=_T91->tl;if(_T92!=0)goto _TL75;_T93=lenmod;{struct Cyc_List_List _T149=*_T93;_T94=_T149.hd;_T148=(int)_T94;}_T95=(int)_T148;if(_T95!=108)goto _TL77;{int x=_T148;{struct Cyc_List_List*_T149=_cycalloc(sizeof(struct Cyc_List_List));
# 257
_T149->hd=Cyc_Absyn_long_double_type;_T149->tl=typs;_T96=(struct Cyc_List_List*)_T149;}typs=_T96;goto _LL51;}_TL77: goto _LL56;_TL75: _LL56:{struct Cyc_String_pa_PrintArg_struct _T149;_T149.tag=0;
# 260
_T149.f1=Cyc_implode(lenmod);_T98=_T149;}{struct Cyc_String_pa_PrintArg_struct _T149=_T98;{struct Cyc_Int_pa_PrintArg_struct _T14A;_T14A.tag=1;_T9A=c;_T9B=(int)_T9A;_T14A.f1=(unsigned long)_T9B;_T99=_T14A;}{struct Cyc_Int_pa_PrintArg_struct _T14A=_T99;void*_T14B[2];_T9C=_T14B + 0;*_T9C=& _T149;_T9D=_T14B + 1;*_T9D=& _T14A;{
# 259
struct Cyc_List_List*(*_T14C)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T9E=_T14C;}_T9F=loc;_TA0=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_TA1=_tag_fat(_T14B,sizeof(void*),2);_T97=_T9E(_T9F,_TA0,_TA1);}}{struct Cyc_List_List*_T149=_T97;_npop_handler(0);return _T149;}_LL51:;}goto _LL12;case 99:{
# 264
struct Cyc_List_List*f=flags;_TL7C: if(f!=0)goto _TL7A;else{goto _TL7B;}
_TL7A: _TA2=f;_TA3=_TA2->hd;_TA4=(int)_TA3;if(_TA4==35)goto _TL7F;else{goto _TL80;}_TL80: _TA5=f;_TA6=_TA5->hd;_TA7=(int)_TA6;if(_TA7==48)goto _TL7F;else{goto _TL7D;}
_TL7F:{struct Cyc_Int_pa_PrintArg_struct _T148;_T148.tag=1;_TAA=f;_TAB=_TAA->hd;_TAC=(int)_TAB;_T148.f1=(unsigned long)_TAC;_TA9=_T148;}{struct Cyc_Int_pa_PrintArg_struct _T148=_TA9;void*_T149[1];_TAD=_T149 + 0;*_TAD=& _T148;{struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TAE=_T14A;}_TAF=loc;_TB0=_tag_fat("flag '%c' not allowed with %%c",sizeof(char),31U);_TB1=_tag_fat(_T149,sizeof(void*),1);_TA8=_TAE(_TAF,_TB0,_TB1);}{struct Cyc_List_List*_T148=_TA8;_npop_handler(0);return _T148;}_TL7D: _TB2=f;
# 264
f=_TB2->tl;goto _TL7C;_TL7B:;}
# 269
if(lenmod==0)goto _TL81;{struct Cyc_String_pa_PrintArg_struct _T148;_T148.tag=0;
# 271
_T148.f1=Cyc_implode(lenmod);_TB4=_T148;}{struct Cyc_String_pa_PrintArg_struct _T148=_TB4;void*_T149[1];_TB5=_T149 + 0;*_TB5=& _T148;{
# 270
struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TB6=_T14A;}_TB7=loc;_TB8=_tag_fat("length modifier '%s' not allowed with %%c",sizeof(char),42U);_TB9=_tag_fat(_T149,sizeof(void*),1);_TB3=_TB6(_TB7,_TB8,_TB9);}{struct Cyc_List_List*_T148=_TB3;_npop_handler(0);return _T148;}_TL81:
# 272
 if(precision==0)goto _TL83;{struct Cyc_String_pa_PrintArg_struct _T148;_T148.tag=0;
# 274
_T148.f1=Cyc_implode(precision);_TBB=_T148;}{struct Cyc_String_pa_PrintArg_struct _T148=_TBB;void*_T149[1];_TBC=_T149 + 0;*_TBC=& _T148;{
# 273
struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TBD=_T14A;}_TBE=loc;_TBF=_tag_fat("precision '%s' not allowed with %%c",sizeof(char),36U);_TC0=_tag_fat(_T149,sizeof(void*),1);_TBA=_TBD(_TBE,_TBF,_TC0);}{struct Cyc_List_List*_T148=_TBA;_npop_handler(0);return _T148;}_TL83:{struct Cyc_List_List*_T148=_cycalloc(sizeof(struct Cyc_List_List));
# 275
_T148->hd=Cyc_Absyn_sint_type;_T148->tl=typs;_TC1=(struct Cyc_List_List*)_T148;}typs=_TC1;goto _LL12;case 115:{
# 279
struct Cyc_List_List*f=flags;_TL88: if(f!=0)goto _TL86;else{goto _TL87;}
_TL86: _TC2=f;_TC3=_TC2->hd;_TC4=(int)_TC3;if(_TC4==45)goto _TL89;{
struct Cyc_List_List*(*_T148)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TC5=_T148;}_TC6=loc;_TC7=_tag_fat("a flag not allowed with %%s",sizeof(char),28U);_TC8=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T148=_TC5(_TC6,_TC7,_TC8);_npop_handler(0);return _T148;}_TL89: _TC9=f;
# 279
f=_TC9->tl;goto _TL88;_TL87:;}
# 284
if(lenmod==0)goto _TL8B;{
struct Cyc_List_List*(*_T148)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TCA=_T148;}_TCB=loc;_TCC=_tag_fat("length modifiers not allowed with %%s",sizeof(char),38U);_TCD=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T148=_TCA(_TCB,_TCC,_TCD);_npop_handler(0);return _T148;}_TL8B: {
# 288
void*ptr;
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);_TCE=isCproto;
if(_TCE)goto _TL8D;else{goto _TL8F;}
_TL8F: _TCF=Cyc_Absyn_char_type;_TD0=& Cyc_Kinds_eko;_TD1=(struct Cyc_Core_Opt*)_TD0;{struct Cyc_Core_Opt*_T148=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 293
_T148->v=tvs;_TD2=(struct Cyc_Core_Opt*)_T148;}_TD3=
# 292
Cyc_Absyn_new_evar(_TD1,_TD2);_TD4=Cyc_Absyn_al_qual_type;_TD5=
# 294
Cyc_Absyn_const_tqual(0U);_TD6=Cyc_Absyn_false_type;_TD7=Cyc_Tcutil_any_bool(tvs);
# 291
ptr=Cyc_Absyn_fatptr_type(_TCF,_TD3,_TD4,_TD5,_TD6,_TD7);goto _TL8E;
# 296
_TL8D: _TD8=Cyc_Absyn_char_type;_TD9=& Cyc_Kinds_eko;_TDA=(struct Cyc_Core_Opt*)_TD9;{struct Cyc_Core_Opt*_T148=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 298
_T148->v=tvs;_TDB=(struct Cyc_Core_Opt*)_T148;}_TDC=
# 297
Cyc_Absyn_new_evar(_TDA,_TDB);_TDD=Cyc_Absyn_al_qual_type;_TDE=
# 299
Cyc_Absyn_const_tqual(0U);_TDF=Cyc_Absyn_true_type;_TE0=Cyc_Tcutil_any_bool(tvs);
# 296
ptr=Cyc_Absyn_at_type(_TD8,_TDC,_TDD,_TDE,_TDF,_TE0);_TL8E:{struct Cyc_List_List*_T148=_cycalloc(sizeof(struct Cyc_List_List));
# 300
_T148->hd=ptr;_T148->tl=typs;_TE1=(struct Cyc_List_List*)_T148;}typs=_TE1;goto _LL12;}case 112:{struct Cyc_List_List*_T148=_cycalloc(sizeof(struct Cyc_List_List));
# 304
_T148->hd=Cyc_Absyn_uint_type;_T148->tl=typs;_TE2=(struct Cyc_List_List*)_T148;}typs=_TE2;goto _LL12;case 110:{
# 307
struct Cyc_List_List*f=flags;_TL93: if(f!=0)goto _TL91;else{goto _TL92;}
_TL91: _TE3=f;_TE4=_TE3->hd;_TE5=(int)_TE4;if(_TE5==35)goto _TL96;else{goto _TL97;}_TL97: _TE6=f;_TE7=_TE6->hd;_TE8=(int)_TE7;if(_TE8==48)goto _TL96;else{goto _TL94;}
_TL96:{struct Cyc_Int_pa_PrintArg_struct _T148;_T148.tag=1;_TEB=f;_TEC=_TEB->hd;_TED=(int)_TEC;_T148.f1=(unsigned long)_TED;_TEA=_T148;}{struct Cyc_Int_pa_PrintArg_struct _T148=_TEA;void*_T149[1];_TEE=_T149 + 0;*_TEE=& _T148;{struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TEF=_T14A;}_TF0=loc;_TF1=_tag_fat("flag '%c' not allowed with %%n",sizeof(char),31U);_TF2=_tag_fat(_T149,sizeof(void*),1);_TE9=_TEF(_TF0,_TF1,_TF2);}{struct Cyc_List_List*_T148=_TE9;_npop_handler(0);return _T148;}_TL94: _TF3=f;
# 307
f=_TF3->tl;goto _TL93;_TL92:;}
# 310
if(precision==0)goto _TL98;{struct Cyc_String_pa_PrintArg_struct _T148;_T148.tag=0;
# 312
_T148.f1=Cyc_implode(precision);_TF5=_T148;}{struct Cyc_String_pa_PrintArg_struct _T148=_TF5;void*_T149[1];_TF6=_T149 + 0;*_TF6=& _T148;{
# 311
struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TF7=_T14A;}_TF8=loc;_TF9=_tag_fat("precision '%s' not allowed with %%n",sizeof(char),36U);_TFA=_tag_fat(_T149,sizeof(void*),1);_TF4=_TF7(_TF8,_TF9,_TFA);}{struct Cyc_List_List*_T148=_TF4;_npop_handler(0);return _T148;}_TL98: {
# 313
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _T148;int _T149;if(lenmod!=0)goto _TL9A;
# 315
t=Cyc_Absyn_sint_type;goto _LL58;_TL9A: _TFB=lenmod;_TFC=(struct Cyc_List_List*)_TFB;_TFD=_TFC->tl;if(_TFD!=0)goto _TL9C;_TFE=lenmod;{struct Cyc_List_List _T14A=*_TFE;_TFF=_T14A.hd;_T149=(int)_TFF;}_T100=(int)_T149;if(_T100!=108)goto _TL9E;{int x=_T149;
# 317
t=Cyc_Absyn_ulong_type;goto _LL58;}_TL9E: _T101=lenmod;{struct Cyc_List_List _T14A=*_T101;_T102=_T14A.hd;_T149=(int)_T102;}_T103=(int)_T149;if(_T103!=104)goto _TLA0;{int x=_T149;
t=Cyc_Absyn_sshort_type;goto _LL58;}_TLA0: goto _LL61;_TL9C: _T104=lenmod;_T105=(struct Cyc_List_List*)_T104;_T106=_T105->tl;_T107=(struct Cyc_List_List*)_T106;_T108=_T107->tl;if(_T108!=0)goto _TLA2;_T109=lenmod;{struct Cyc_List_List _T14A=*_T109;_T10A=_T14A.hd;_T149=(int)_T10A;_T10B=_T14A.tl;{struct Cyc_List_List _T14B=*_T10B;_T10C=_T14B.hd;_T148=(int)_T10C;}}_T10D=(int)_T149;if(_T10D!=104)goto _TLA4;_T10E=(int)_T148;if(_T10E!=104)goto _TLA4;{int x=_T149;int y=_T148;
# 320
t=Cyc_Absyn_schar_type;goto _LL58;}_TLA4: goto _LL61;_TLA2: _LL61:{struct Cyc_String_pa_PrintArg_struct _T14A;_T14A.tag=0;
# 323
_T14A.f1=Cyc_implode(lenmod);_T110=_T14A;}{struct Cyc_String_pa_PrintArg_struct _T14A=_T110;{struct Cyc_Int_pa_PrintArg_struct _T14B;_T14B.tag=1;_T112=c;_T113=(int)_T112;_T14B.f1=(unsigned long)_T113;_T111=_T14B;}{struct Cyc_Int_pa_PrintArg_struct _T14B=_T111;void*_T14C[2];_T114=_T14C + 0;*_T114=& _T14A;_T115=_T14C + 1;*_T115=& _T14B;{
# 322
struct Cyc_List_List*(*_T14D)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T116=_T14D;}_T117=loc;_T118=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T119=_tag_fat(_T14C,sizeof(void*),2);_T10F=_T116(_T117,_T118,_T119);}}{struct Cyc_List_List*_T14A=_T10F;_npop_handler(0);return _T14A;}_LL58:;}_T11A=t;_T11B=& Cyc_Kinds_eko;_T11C=(struct Cyc_Core_Opt*)_T11B;{struct Cyc_Core_Opt*_T148=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 325
_T148->v=tvs;_T11D=(struct Cyc_Core_Opt*)_T148;}_T11E=Cyc_Absyn_new_evar(_T11C,_T11D);_T11F=Cyc_Absyn_al_qual_type;_T120=Cyc_Absyn_empty_tqual(0U);_T121=Cyc_Absyn_false_type;_T122=
Cyc_Tcutil_any_bool(tvs);
# 325
t=Cyc_Absyn_at_type(_T11A,_T11E,_T11F,_T120,_T121,_T122);{struct Cyc_List_List*_T148=_cycalloc(sizeof(struct Cyc_List_List));
# 327
_T148->hd=t;_T148->tl=typs;_T123=(struct Cyc_List_List*)_T148;}typs=_T123;goto _LL12;}case 37:
# 330
 if(flags==0)goto _TLA6;{struct Cyc_String_pa_PrintArg_struct _T148;_T148.tag=0;
# 332
_T148.f1=Cyc_implode(flags);_T125=_T148;}{struct Cyc_String_pa_PrintArg_struct _T148=_T125;void*_T149[1];_T126=_T149 + 0;*_T126=& _T148;{
# 331
struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T127=_T14A;}_T128=loc;_T129=_tag_fat("flags '%s' not allowed with %%%%",sizeof(char),33U);_T12A=_tag_fat(_T149,sizeof(void*),1);_T124=_T127(_T128,_T129,_T12A);}{struct Cyc_List_List*_T148=_T124;_npop_handler(0);return _T148;}_TLA6:
# 333
 if(width==0)goto _TLA8;{struct Cyc_String_pa_PrintArg_struct _T148;_T148.tag=0;
# 335
_T148.f1=Cyc_implode(width);_T12C=_T148;}{struct Cyc_String_pa_PrintArg_struct _T148=_T12C;void*_T149[1];_T12D=_T149 + 0;*_T12D=& _T148;{
# 334
struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T12E=_T14A;}_T12F=loc;_T130=_tag_fat("width '%s' not allowed with %%%%",sizeof(char),33U);_T131=_tag_fat(_T149,sizeof(void*),1);_T12B=_T12E(_T12F,_T130,_T131);}{struct Cyc_List_List*_T148=_T12B;_npop_handler(0);return _T148;}_TLA8:
# 336
 if(precision==0)goto _TLAA;{struct Cyc_String_pa_PrintArg_struct _T148;_T148.tag=0;
# 338
_T148.f1=Cyc_implode(precision);_T133=_T148;}{struct Cyc_String_pa_PrintArg_struct _T148=_T133;void*_T149[1];_T134=_T149 + 0;*_T134=& _T148;{
# 337
struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T135=_T14A;}_T136=loc;_T137=_tag_fat("precision '%s' not allowed with %%%%",sizeof(char),37U);_T138=_tag_fat(_T149,sizeof(void*),1);_T132=_T135(_T136,_T137,_T138);}{struct Cyc_List_List*_T148=_T132;_npop_handler(0);return _T148;}_TLAA:
# 339
 if(lenmod==0)goto _TLAC;{struct Cyc_String_pa_PrintArg_struct _T148;_T148.tag=0;
# 341
_T148.f1=Cyc_implode(lenmod);_T13A=_T148;}{struct Cyc_String_pa_PrintArg_struct _T148=_T13A;void*_T149[1];_T13B=_T149 + 0;*_T13B=& _T148;{
# 340
struct Cyc_List_List*(*_T14A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T13C=_T14A;}_T13D=loc;_T13E=_tag_fat("length modifier '%s' not allowed with %%%%",sizeof(char),43U);_T13F=_tag_fat(_T149,sizeof(void*),1);_T139=_T13C(_T13D,_T13E,_T13F);}{struct Cyc_List_List*_T148=_T139;_npop_handler(0);return _T148;}_TLAC: goto _LL12;default:  {struct Cyc_List_List*_T148=0;_npop_handler(0);return _T148;}}_LL12:;}}}}_TL2F:
# 176
 i=i + 1;goto _TL32;_TL31:{struct Cyc_List_List*_T141=
# 346
Cyc_List_imp_rev(typs);_npop_handler(0);return _T141;}_pop_region();}struct _tuple12{int f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;char f3;int f4;};
# 357 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct _fat_ptr s,int i){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;int _T6;char _T7;int _T8;int _T9;unsigned long _TA;unsigned long _TB;int _TC;unsigned long _TD;unsigned long _TE;struct _fat_ptr _TF;unsigned char*_T10;const char*_T11;int _T12;char _T13;int _T14;int _T15;struct Cyc_List_List*_T16;struct _RegionHandle*_T17;char _T18;int _T19;unsigned long _T1A;unsigned long _T1B;struct _fat_ptr _T1C;unsigned char*_T1D;const char*_T1E;int _T1F;char _T20;int _T21;struct Cyc_List_List*_T22;struct _RegionHandle*_T23;char _T24;int _T25;unsigned long _T26;unsigned long _T27;struct _fat_ptr _T28;unsigned char*_T29;const char*_T2A;int _T2B;char _T2C;int _T2D;struct Cyc_List_List*_T2E;struct _RegionHandle*_T2F;char _T30;struct Cyc_List_List*_T31;struct _RegionHandle*_T32;char _T33;int _T34;unsigned long _T35;unsigned long _T36;struct _fat_ptr _T37;unsigned char*_T38;const char*_T39;int _T3A;char _T3B;int _T3C;struct Cyc_List_List*_T3D;struct _RegionHandle*_T3E;char _T3F;struct Cyc_List_List*_T40;struct _RegionHandle*_T41;char _T42;int _T43;unsigned long _T44;unsigned long _T45;struct _fat_ptr _T46;unsigned char*_T47;const char*_T48;int _T49;char _T4A;int _T4B;struct Cyc_Core_Opt*_T4C;struct _RegionHandle*_T4D;struct _tuple12*_T4E;struct _RegionHandle*_T4F;
# 359
unsigned long len=Cyc_strlen(s);
if(i < 0)goto _TLB0;else{goto _TLB1;}_TLB1: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 >= _T2)goto _TLB0;else{goto _TLAE;}_TLB0: return 0;_TLAE: {
# 362
int suppress=0;_T3=s;_T4=_T3.curr;_T5=(const char*)_T4;_T6=i;{
char c=_T5[_T6];_T7=c;_T8=(int)_T7;
if(_T8!=42)goto _TLB2;
suppress=1;
i=i + 1;_T9=i;_TA=(unsigned long)_T9;_TB=len;
if(_TA < _TB)goto _TLB4;return 0;_TLB4: goto _TLB3;_TLB2: _TLB3: {
# 370
struct Cyc_List_List*width=0;
_TLB9: _TC=i;_TD=(unsigned long)_TC;_TE=len;if(_TD < _TE)goto _TLB7;else{goto _TLB8;}
_TLB7: _TF=s;_T10=_TF.curr;_T11=(const char*)_T10;_T12=i;c=_T11[_T12];_T13=c;_T14=(int)_T13;_T15=
isdigit(_T14);if(!_T15)goto _TLBA;_T17=r;{struct Cyc_List_List*_T50=_region_malloc(_T17,0U,sizeof(struct Cyc_List_List));_T18=c;_T50->hd=(void*)_T18;_T50->tl=width;_T16=(struct Cyc_List_List*)_T50;}width=_T16;goto _TLBB;
_TLBA: goto _TLB8;_TLBB:
# 371
 i=i + 1;goto _TLB9;_TLB8: _T19=i;_T1A=(unsigned long)_T19;_T1B=len;
# 376
if(_T1A < _T1B)goto _TLBC;return 0;_TLBC:
 width=Cyc_List_imp_rev(width);{
# 381
struct Cyc_List_List*lenmod=0;_T1C=s;_T1D=_T1C.curr;_T1E=(const char*)_T1D;_T1F=i;
c=_T1E[_T1F];_T20=c;_T21=(int)_T20;switch(_T21){case 104: _T23=r;{struct Cyc_List_List*_T50=_region_malloc(_T23,0U,sizeof(struct Cyc_List_List));_T24=c;
# 385
_T50->hd=(void*)_T24;_T50->tl=lenmod;_T22=(struct Cyc_List_List*)_T50;}lenmod=_T22;
i=i + 1;_T25=i;_T26=(unsigned long)_T25;_T27=len;
if(_T26 < _T27)goto _TLBF;return 0;_TLBF: _T28=s;_T29=_T28.curr;_T2A=(const char*)_T29;_T2B=i;
c=_T2A[_T2B];_T2C=c;_T2D=(int)_T2C;
if(_T2D!=104)goto _TLC1;_T2F=r;{struct Cyc_List_List*_T50=_region_malloc(_T2F,0U,sizeof(struct Cyc_List_List));_T30=c;_T50->hd=(void*)_T30;_T50->tl=lenmod;_T2E=(struct Cyc_List_List*)_T50;}lenmod=_T2E;i=i + 1;goto _TLC2;_TLC1: _TLC2: goto _LL0;case 108: _T32=r;{struct Cyc_List_List*_T50=_region_malloc(_T32,0U,sizeof(struct Cyc_List_List));_T33=c;
# 392
_T50->hd=(void*)_T33;_T50->tl=lenmod;_T31=(struct Cyc_List_List*)_T50;}lenmod=_T31;
i=i + 1;_T34=i;_T35=(unsigned long)_T34;_T36=len;
if(_T35 < _T36)goto _TLC3;return 0;_TLC3: _T37=s;_T38=_T37.curr;_T39=(const char*)_T38;_T3A=i;
c=_T39[_T3A];_T3B=c;_T3C=(int)_T3B;
if(_T3C!=108)goto _TLC5;_T3E=r;{struct Cyc_List_List*_T50=_region_malloc(_T3E,0U,sizeof(struct Cyc_List_List));_T3F=c;_T50->hd=(void*)_T3F;_T50->tl=lenmod;_T3D=(struct Cyc_List_List*)_T50;}lenmod=_T3D;i=i + 1;goto _TLC6;_TLC5: _TLC6: goto _LL0;case 106: goto _LL8;case 122: _LL8: goto _LLA;case 116: _LLA: goto _LLC;case 76: _LLC: _T41=r;{struct Cyc_List_List*_T50=_region_malloc(_T41,0U,sizeof(struct Cyc_List_List));_T42=c;
# 402
_T50->hd=(void*)_T42;_T50->tl=lenmod;_T40=(struct Cyc_List_List*)_T50;}lenmod=_T40;
i=i + 1;goto _LL0;default: goto _LL0;}_LL0: _T43=i;_T44=(unsigned long)_T43;_T45=len;
# 407
if(_T44 < _T45)goto _TLC7;return 0;_TLC7:
 lenmod=Cyc_List_imp_rev(lenmod);_T46=s;_T47=_T46.curr;_T48=(const char*)_T47;_T49=i;
# 411
c=_T48[_T49];_T4A=c;_T4B=(int)_T4A;switch(_T4B){case 100: goto _LL13;case 105: _LL13: goto _LL15;case 111: _LL15: goto _LL17;case 117: _LL17: goto _LL19;case 120: _LL19: goto _LL1B;case 88: _LL1B: goto _LL1D;case 102: _LL1D: goto _LL1F;case 70: _LL1F: goto _LL21;case 101: _LL21: goto _LL23;case 69: _LL23: goto _LL25;case 103: _LL25: goto _LL27;case 71: _LL27: goto _LL29;case 97: _LL29: goto _LL2B;case 65: _LL2B: goto _LL2D;case 99: _LL2D: goto _LL2F;case 115: _LL2F: goto _LL31;case 112: _LL31: goto _LL33;case 110: _LL33: goto _LL35;case 37: _LL35: goto _LLF;default:
# 432
 return 0;}_LLF: _T4D=r;{struct Cyc_Core_Opt*_T50=_region_malloc(_T4D,0U,sizeof(struct Cyc_Core_Opt));_T4F=r;{struct _tuple12*_T51=_region_malloc(_T4F,0U,sizeof(struct _tuple12));
# 434
_T51->f0=suppress;_T51->f1=width;_T51->f2=lenmod;_T51->f3=c;_T51->f4=i + 1;_T4E=(struct _tuple12*)_T51;}_T50->v=_T4E;_T4C=(struct Cyc_Core_Opt*)_T50;}return _T4C;}}}}}
# 436
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;int _T4;char*_T5;const char*_T6;char _T7;int _T8;struct _RegionHandle*_T9;struct _fat_ptr _TA;int _TB;struct Cyc_List_List*(*_TC)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Core_Opt*_T10;void*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;int _T17;int _T18;int _T19;struct Cyc_List_List*_T1A;struct Cyc_Int_pa_PrintArg_struct _T1B;int _T1C;void**_T1D;struct Cyc_List_List*(*_T1E)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;int _T22;char _T23;int _T24;struct Cyc_List_List*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;void*_T29;int _T2A;struct Cyc_List_List*_T2B;void*_T2C;int _T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;struct Cyc_List_List*_T32;struct Cyc_List_List*_T33;void*_T34;struct Cyc_List_List*_T35;void*_T36;int _T37;int _T38;struct Cyc_List_List*_T39;struct Cyc_String_pa_PrintArg_struct _T3A;struct Cyc_Int_pa_PrintArg_struct _T3B;char _T3C;int _T3D;void**_T3E;void**_T3F;struct Cyc_List_List*(*_T40)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T41;struct _fat_ptr _T42;struct _fat_ptr _T43;void*_T44;struct Cyc_Core_Opt*_T45;struct Cyc_Core_Opt*_T46;struct Cyc_Core_Opt*_T47;void*_T48;void*_T49;struct Cyc_Absyn_Tqual _T4A;void*_T4B;void*_T4C;struct Cyc_List_List*_T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;struct Cyc_List_List*_T50;struct Cyc_List_List*_T51;void*_T52;int _T53;struct Cyc_List_List*_T54;void*_T55;int _T56;struct Cyc_List_List*_T57;struct Cyc_List_List*_T58;struct Cyc_List_List*_T59;struct Cyc_List_List*_T5A;struct Cyc_List_List*_T5B;struct Cyc_List_List*_T5C;void*_T5D;struct Cyc_List_List*_T5E;void*_T5F;int _T60;int _T61;struct Cyc_List_List*_T62;struct Cyc_String_pa_PrintArg_struct _T63;struct Cyc_Int_pa_PrintArg_struct _T64;char _T65;int _T66;void**_T67;void**_T68;struct Cyc_List_List*(*_T69)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T6A;struct _fat_ptr _T6B;struct _fat_ptr _T6C;void*_T6D;struct Cyc_Core_Opt*_T6E;struct Cyc_Core_Opt*_T6F;struct Cyc_Core_Opt*_T70;void*_T71;void*_T72;struct Cyc_Absyn_Tqual _T73;void*_T74;void*_T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;struct Cyc_List_List*_T78;struct Cyc_List_List*_T79;struct Cyc_List_List*_T7A;void*_T7B;int _T7C;struct Cyc_List_List*_T7D;struct Cyc_String_pa_PrintArg_struct _T7E;struct Cyc_Int_pa_PrintArg_struct _T7F;char _T80;int _T81;void**_T82;void**_T83;struct Cyc_List_List*(*_T84)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T85;struct _fat_ptr _T86;struct _fat_ptr _T87;void*_T88;struct Cyc_Core_Opt*_T89;struct Cyc_Core_Opt*_T8A;struct Cyc_Core_Opt*_T8B;void*_T8C;void*_T8D;struct Cyc_Absyn_Tqual _T8E;void*_T8F;void*_T90;struct Cyc_List_List*_T91;int _T92;void*_T93;struct Cyc_Core_Opt*_T94;struct Cyc_Core_Opt*_T95;struct Cyc_Core_Opt*_T96;void*_T97;void*_T98;struct Cyc_Absyn_Tqual _T99;void*_T9A;void*_T9B;void*_T9C;struct Cyc_Core_Opt*_T9D;struct Cyc_Core_Opt*_T9E;struct Cyc_Core_Opt*_T9F;void*_TA0;void*_TA1;struct Cyc_Absyn_Tqual _TA2;void*_TA3;void*_TA4;struct Cyc_List_List*_TA5;int _TA6;void*_TA7;struct Cyc_Core_Opt*_TA8;struct Cyc_Core_Opt*_TA9;struct Cyc_Core_Opt*_TAA;void*_TAB;void*_TAC;struct Cyc_Absyn_Tqual _TAD;void*_TAE;void*_TAF;void*_TB0;struct Cyc_Core_Opt*_TB1;struct Cyc_Core_Opt*_TB2;struct Cyc_Core_Opt*_TB3;void*_TB4;void*_TB5;struct Cyc_Absyn_Tqual _TB6;void*_TB7;void*_TB8;struct Cyc_List_List*_TB9;struct Cyc_List_List*_TBA;struct Cyc_Int_pa_PrintArg_struct _TBB;char _TBC;int _TBD;void**_TBE;struct Cyc_List_List*(*_TBF)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TC0;struct _fat_ptr _TC1;struct _fat_ptr _TC2;struct Cyc_List_List*_TC3;struct Cyc_List_List*_TC4;struct Cyc_List_List*_TC5;struct Cyc_List_List*_TC6;void*_TC7;int _TC8;struct Cyc_List_List*_TC9;void*_TCA;int _TCB;struct Cyc_List_List*_TCC;struct Cyc_List_List*_TCD;struct Cyc_List_List*_TCE;struct Cyc_List_List*_TCF;struct Cyc_List_List*_TD0;struct Cyc_List_List*_TD1;void*_TD2;struct Cyc_List_List*_TD3;void*_TD4;int _TD5;int _TD6;struct Cyc_List_List*_TD7;struct Cyc_String_pa_PrintArg_struct _TD8;struct Cyc_Int_pa_PrintArg_struct _TD9;char _TDA;int _TDB;void**_TDC;void**_TDD;struct Cyc_List_List*(*_TDE)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TDF;struct _fat_ptr _TE0;struct _fat_ptr _TE1;void*_TE2;struct Cyc_Core_Opt*_TE3;struct Cyc_Core_Opt*_TE4;struct Cyc_Core_Opt*_TE5;void*_TE6;void*_TE7;struct Cyc_Absyn_Tqual _TE8;void*_TE9;void*_TEA;struct Cyc_List_List*_TEB;int _TEC;struct Cyc_List_List*(*_TED)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TEE;struct _fat_ptr _TEF;struct _fat_ptr _TF0;struct Cyc_List_List*_TF1;struct Cyc_String_pa_PrintArg_struct _TF2;void**_TF3;struct Cyc_List_List*(*_TF4)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TF5;struct _fat_ptr _TF6;struct _fat_ptr _TF7;struct Cyc_List_List*_TF8;struct Cyc_String_pa_PrintArg_struct _TF9;void**_TFA;struct Cyc_List_List*(*_TFB)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TFC;struct _fat_ptr _TFD;struct _fat_ptr _TFE;struct Cyc_List_List*_TFF;
# 439
unsigned long len=Cyc_strlen(s);
struct Cyc_List_List*typs=0;
int i;{struct _RegionHandle _T100=_new_region(0U,"temp");struct _RegionHandle*temp=& _T100;_push_region(temp);
# 443
i=0;_TLCD: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 < _T2)goto _TLCB;else{goto _TLCC;}
_TLCB: _T3=s;_T4=i;_T5=_check_fat_subscript(_T3,sizeof(char),_T4);_T6=(const char*)_T5;_T7=*_T6;_T8=(int)_T7;if(_T8==37)goto _TLCE;goto _TLCA;_TLCE: _T9=temp;_TA=s;_TB=i + 1;{
struct Cyc_Core_Opt*x=Cyc_Formatstr_parse_inputformat(_T9,_TA,_TB);
if(x!=0)goto _TLD0;{
struct Cyc_List_List*(*_T101)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TC=_T101;}_TD=loc;_TE=_tag_fat("bad format string",sizeof(char),18U);_TF=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T101=_TC(_TD,_TE,_TF);_npop_handler(0);return _T101;}_TLD0: _T10=x;_T11=_T10->v;{
struct _tuple12*_T101=(struct _tuple12*)_T11;int _T102;char _T103;struct Cyc_List_List*_T104;struct Cyc_List_List*_T105;int _T106;{struct _tuple12 _T107=*_T101;_T106=_T107.f0;_T105=_T107.f1;_T104=_T107.f2;_T103=_T107.f3;_T102=_T107.f4;}{int suppress=_T106;struct Cyc_List_List*width=_T105;struct Cyc_List_List*lenmod=_T104;char c=_T103;int j=_T102;
i=j - 1;{int _T107;if(lenmod==0)goto _TLD2;_T12=lenmod;_T13=(struct Cyc_List_List*)_T12;_T14=_T13->tl;if(_T14!=0)goto _TLD4;_T15=lenmod;{struct Cyc_List_List _T108=*_T15;_T16=_T108.hd;_T107=(int)_T16;}_T17=(int)_T107;if(_T17==106)goto _TLD8;else{goto _TLDA;}_TLDA: _T18=(int)_T107;if(_T18==122)goto _TLD8;else{goto _TLD9;}_TLD9: _T19=(int)_T107;if(_T19==116)goto _TLD8;else{goto _TLD6;}_TLD8:{int x=_T107;{struct Cyc_Int_pa_PrintArg_struct _T108;_T108.tag=1;_T1C=x;
# 453
_T108.f1=(unsigned long)_T1C;_T1B=_T108;}{struct Cyc_Int_pa_PrintArg_struct _T108=_T1B;void*_T109[1];_T1D=_T109 + 0;*_T1D=& _T108;{struct Cyc_List_List*(*_T10A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T1E=_T10A;}_T1F=loc;_T20=_tag_fat("length modifier '%c' is not supported",sizeof(char),38U);_T21=_tag_fat(_T109,sizeof(void*),1);_T1A=_T1E(_T1F,_T20,_T21);}{struct Cyc_List_List*_T108=_T1A;_npop_handler(0);return _T108;}}_TLD6: goto _LL6;_TLD4: goto _LL6;_TLD2: _LL6: goto _LL3;_LL3:;}_T22=suppress;
# 456
if(!_T22)goto _TLDB;goto _TLCA;_TLDB: {
void*t;_T23=c;_T24=(int)_T23;switch(_T24){case 100: goto _LLC;case 105: _LLC: {
# 461
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _T107;int _T108;if(lenmod!=0)goto _TLDE;
# 463
t=Cyc_Absyn_sint_type;goto _LL33;_TLDE: _T25=lenmod;_T26=(struct Cyc_List_List*)_T25;_T27=_T26->tl;if(_T27!=0)goto _TLE0;_T28=lenmod;{struct Cyc_List_List _T109=*_T28;_T29=_T109.hd;_T108=(int)_T29;}_T2A=(int)_T108;if(_T2A!=108)goto _TLE2;{int x=_T108;
t=Cyc_Absyn_slong_type;goto _LL33;}_TLE2: _T2B=lenmod;{struct Cyc_List_List _T109=*_T2B;_T2C=_T109.hd;_T108=(int)_T2C;}_T2D=(int)_T108;if(_T2D!=104)goto _TLE4;{int x=_T108;
t=Cyc_Absyn_sshort_type;goto _LL33;}_TLE4: goto _LL3C;_TLE0: _T2E=lenmod;_T2F=(struct Cyc_List_List*)_T2E;_T30=_T2F->tl;_T31=(struct Cyc_List_List*)_T30;_T32=_T31->tl;if(_T32!=0)goto _TLE6;_T33=lenmod;{struct Cyc_List_List _T109=*_T33;_T34=_T109.hd;_T108=(int)_T34;_T35=_T109.tl;{struct Cyc_List_List _T10A=*_T35;_T36=_T10A.hd;_T107=(int)_T36;}}_T37=(int)_T108;if(_T37!=104)goto _TLE8;_T38=(int)_T107;if(_T38!=104)goto _TLE8;{int x=_T108;int y=_T107;
t=Cyc_Absyn_schar_type;goto _LL33;}_TLE8: goto _LL3C;_TLE6: _LL3C:{struct Cyc_String_pa_PrintArg_struct _T109;_T109.tag=0;
# 469
_T109.f1=Cyc_implode(lenmod);_T3A=_T109;}{struct Cyc_String_pa_PrintArg_struct _T109=_T3A;{struct Cyc_Int_pa_PrintArg_struct _T10A;_T10A.tag=1;_T3C=c;_T3D=(int)_T3C;_T10A.f1=(unsigned long)_T3D;_T3B=_T10A;}{struct Cyc_Int_pa_PrintArg_struct _T10A=_T3B;void*_T10B[2];_T3E=_T10B + 0;*_T3E=& _T109;_T3F=_T10B + 1;*_T3F=& _T10A;{
# 468
struct Cyc_List_List*(*_T10C)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T40=_T10C;}_T41=loc;_T42=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T43=_tag_fat(_T10B,sizeof(void*),2);_T39=_T40(_T41,_T42,_T43);}}{struct Cyc_List_List*_T109=_T39;_npop_handler(0);return _T109;}_LL33:;}_T44=t;_T45=& Cyc_Kinds_eko;_T46=(struct Cyc_Core_Opt*)_T45;{struct Cyc_Core_Opt*_T107=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 471
_T107->v=tvs;_T47=(struct Cyc_Core_Opt*)_T107;}_T48=Cyc_Absyn_new_evar(_T46,_T47);_T49=Cyc_Absyn_al_qual_type;_T4A=Cyc_Absyn_empty_tqual(0U);_T4B=Cyc_Absyn_false_type;_T4C=
Cyc_Tcutil_any_bool(tvs);
# 471
t=Cyc_Absyn_at_type(_T44,_T48,_T49,_T4A,_T4B,_T4C);{struct Cyc_List_List*_T107=_cycalloc(sizeof(struct Cyc_List_List));
# 473
_T107->hd=t;_T107->tl=typs;_T4D=(struct Cyc_List_List*)_T107;}typs=_T4D;goto _LL8;}case 117: goto _LL10;case 111: _LL10: goto _LL12;case 120: _LL12: goto _LL14;case 88: _LL14: {
# 479
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _T107;int _T108;if(lenmod!=0)goto _TLEA;
# 481
t=Cyc_Absyn_uint_type;goto _LL3E;_TLEA: _T4E=lenmod;_T4F=(struct Cyc_List_List*)_T4E;_T50=_T4F->tl;if(_T50!=0)goto _TLEC;_T51=lenmod;{struct Cyc_List_List _T109=*_T51;_T52=_T109.hd;_T108=(int)_T52;}_T53=(int)_T108;if(_T53!=108)goto _TLEE;{int x=_T108;
t=Cyc_Absyn_ulong_type;goto _LL3E;}_TLEE: _T54=lenmod;{struct Cyc_List_List _T109=*_T54;_T55=_T109.hd;_T108=(int)_T55;}_T56=(int)_T108;if(_T56!=104)goto _TLF0;{int x=_T108;
t=Cyc_Absyn_ushort_type;goto _LL3E;}_TLF0: goto _LL47;_TLEC: _T57=lenmod;_T58=(struct Cyc_List_List*)_T57;_T59=_T58->tl;_T5A=(struct Cyc_List_List*)_T59;_T5B=_T5A->tl;if(_T5B!=0)goto _TLF2;_T5C=lenmod;{struct Cyc_List_List _T109=*_T5C;_T5D=_T109.hd;_T108=(int)_T5D;_T5E=_T109.tl;{struct Cyc_List_List _T10A=*_T5E;_T5F=_T10A.hd;_T107=(int)_T5F;}}_T60=(int)_T108;if(_T60!=104)goto _TLF4;_T61=(int)_T107;if(_T61!=104)goto _TLF4;{int x=_T108;int y=_T107;
t=Cyc_Absyn_uchar_type;goto _LL3E;}_TLF4: goto _LL47;_TLF2: _LL47:{struct Cyc_String_pa_PrintArg_struct _T109;_T109.tag=0;
# 487
_T109.f1=Cyc_implode(lenmod);_T63=_T109;}{struct Cyc_String_pa_PrintArg_struct _T109=_T63;{struct Cyc_Int_pa_PrintArg_struct _T10A;_T10A.tag=1;_T65=c;_T66=(int)_T65;_T10A.f1=(unsigned long)_T66;_T64=_T10A;}{struct Cyc_Int_pa_PrintArg_struct _T10A=_T64;void*_T10B[2];_T67=_T10B + 0;*_T67=& _T109;_T68=_T10B + 1;*_T68=& _T10A;{
# 486
struct Cyc_List_List*(*_T10C)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T69=_T10C;}_T6A=loc;_T6B=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T6C=_tag_fat(_T10B,sizeof(void*),2);_T62=_T69(_T6A,_T6B,_T6C);}}{struct Cyc_List_List*_T109=_T62;_npop_handler(0);return _T109;}_LL3E:;}_T6D=t;_T6E=& Cyc_Kinds_eko;_T6F=(struct Cyc_Core_Opt*)_T6E;{struct Cyc_Core_Opt*_T107=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 489
_T107->v=tvs;_T70=(struct Cyc_Core_Opt*)_T107;}_T71=Cyc_Absyn_new_evar(_T6F,_T70);_T72=Cyc_Absyn_al_qual_type;_T73=Cyc_Absyn_empty_tqual(0U);_T74=Cyc_Absyn_false_type;_T75=
Cyc_Tcutil_any_bool(tvs);
# 489
t=Cyc_Absyn_at_type(_T6D,_T71,_T72,_T73,_T74,_T75);{struct Cyc_List_List*_T107=_cycalloc(sizeof(struct Cyc_List_List));
# 491
_T107->hd=t;_T107->tl=typs;_T76=(struct Cyc_List_List*)_T107;}typs=_T76;goto _LL8;}case 102: goto _LL18;case 70: _LL18: goto _LL1A;case 101: _LL1A: goto _LL1C;case 69: _LL1C: goto _LL1E;case 103: _LL1E: goto _LL20;case 71: _LL20: goto _LL22;case 97: _LL22: goto _LL24;case 65: _LL24: {
# 501
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _T107;if(lenmod!=0)goto _TLF6;
# 503
t=Cyc_Absyn_float_type;goto _LL49;_TLF6: _T77=lenmod;_T78=(struct Cyc_List_List*)_T77;_T79=_T78->tl;if(_T79!=0)goto _TLF8;_T7A=lenmod;{struct Cyc_List_List _T108=*_T7A;_T7B=_T108.hd;_T107=(int)_T7B;}_T7C=(int)_T107;if(_T7C!=108)goto _TLFA;{int x=_T107;
# 505
t=Cyc_Absyn_double_type;goto _LL49;}_TLFA: goto _LL4E;_TLF8: _LL4E:{struct Cyc_String_pa_PrintArg_struct _T108;_T108.tag=0;
# 508
_T108.f1=Cyc_implode(lenmod);_T7E=_T108;}{struct Cyc_String_pa_PrintArg_struct _T108=_T7E;{struct Cyc_Int_pa_PrintArg_struct _T109;_T109.tag=1;_T80=c;_T81=(int)_T80;_T109.f1=(unsigned long)_T81;_T7F=_T109;}{struct Cyc_Int_pa_PrintArg_struct _T109=_T7F;void*_T10A[2];_T82=_T10A + 0;*_T82=& _T108;_T83=_T10A + 1;*_T83=& _T109;{
# 507
struct Cyc_List_List*(*_T10B)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T84=_T10B;}_T85=loc;_T86=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T87=_tag_fat(_T10A,sizeof(void*),2);_T7D=_T84(_T85,_T86,_T87);}}{struct Cyc_List_List*_T108=_T7D;_npop_handler(0);return _T108;}_LL49:;}_T88=t;_T89=& Cyc_Kinds_eko;_T8A=(struct Cyc_Core_Opt*)_T89;{struct Cyc_Core_Opt*_T107=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 510
_T107->v=tvs;_T8B=(struct Cyc_Core_Opt*)_T107;}_T8C=Cyc_Absyn_new_evar(_T8A,_T8B);_T8D=Cyc_Absyn_al_qual_type;_T8E=Cyc_Absyn_empty_tqual(0U);_T8F=Cyc_Absyn_false_type;_T90=
Cyc_Tcutil_any_bool(tvs);
# 510
t=Cyc_Absyn_at_type(_T88,_T8C,_T8D,_T8E,_T8F,_T90);{struct Cyc_List_List*_T107=_cycalloc(sizeof(struct Cyc_List_List));
# 512
_T107->hd=t;_T107->tl=typs;_T91=(struct Cyc_List_List*)_T107;}typs=_T91;goto _LL8;}case 99:  {
# 516
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
void*ptr;_T92=isCproto;
if(_T92)goto _TLFC;else{goto _TLFE;}
_TLFE: _T93=Cyc_Absyn_char_type;_T94=& Cyc_Kinds_eko;_T95=(struct Cyc_Core_Opt*)_T94;{struct Cyc_Core_Opt*_T107=_cycalloc(sizeof(struct Cyc_Core_Opt));_T107->v=tvs;_T96=(struct Cyc_Core_Opt*)_T107;}_T97=Cyc_Absyn_new_evar(_T95,_T96);_T98=Cyc_Absyn_al_qual_type;_T99=
Cyc_Absyn_empty_tqual(0U);_T9A=Cyc_Absyn_false_type;_T9B=
Cyc_Tcutil_any_bool(tvs);
# 519
ptr=Cyc_Absyn_fatptr_type(_T93,_T97,_T98,_T99,_T9A,_T9B);goto _TLFD;
# 523
_TLFC: _T9C=Cyc_Absyn_char_type;_T9D=& Cyc_Kinds_eko;_T9E=(struct Cyc_Core_Opt*)_T9D;{struct Cyc_Core_Opt*_T107=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T107->v=tvs;_T9F=(struct Cyc_Core_Opt*)_T107;}_TA0=Cyc_Absyn_new_evar(_T9E,_T9F);_TA1=Cyc_Absyn_al_qual_type;_TA2=
Cyc_Absyn_empty_tqual(0U);_TA3=Cyc_Absyn_false_type;_TA4=Cyc_Tcutil_any_bool(tvs);
# 523
ptr=Cyc_Absyn_at_type(_T9C,_TA0,_TA1,_TA2,_TA3,_TA4);_TLFD:{struct Cyc_List_List*_T107=_cycalloc(sizeof(struct Cyc_List_List));
# 526
_T107->hd=ptr;_T107->tl=typs;_TA5=(struct Cyc_List_List*)_T107;}typs=_TA5;goto _LL8;}case 115:  {
# 529
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
# 531
void*ptr;_TA6=isCproto;
if(_TA6)goto _TLFF;else{goto _TL101;}
_TL101: _TA7=Cyc_Absyn_char_type;_TA8=& Cyc_Kinds_eko;_TA9=(struct Cyc_Core_Opt*)_TA8;{struct Cyc_Core_Opt*_T107=_cycalloc(sizeof(struct Cyc_Core_Opt));_T107->v=tvs;_TAA=(struct Cyc_Core_Opt*)_T107;}_TAB=Cyc_Absyn_new_evar(_TA9,_TAA);_TAC=Cyc_Absyn_al_qual_type;_TAD=
Cyc_Absyn_empty_tqual(0U);_TAE=Cyc_Absyn_false_type;_TAF=Cyc_Tcutil_any_bool(tvs);
# 533
ptr=Cyc_Absyn_fatptr_type(_TA7,_TAB,_TAC,_TAD,_TAE,_TAF);goto _TL100;
# 536
_TLFF: _TB0=Cyc_Absyn_char_type;_TB1=& Cyc_Kinds_eko;_TB2=(struct Cyc_Core_Opt*)_TB1;{struct Cyc_Core_Opt*_T107=_cycalloc(sizeof(struct Cyc_Core_Opt));_T107->v=tvs;_TB3=(struct Cyc_Core_Opt*)_T107;}_TB4=Cyc_Absyn_new_evar(_TB2,_TB3);_TB5=Cyc_Absyn_al_qual_type;_TB6=
Cyc_Absyn_empty_tqual(0U);_TB7=Cyc_Absyn_true_type;_TB8=Cyc_Tcutil_any_bool(tvs);
# 536
ptr=Cyc_Absyn_at_type(_TB0,_TB4,_TB5,_TB6,_TB7,_TB8);_TL100:{struct Cyc_List_List*_T107=_cycalloc(sizeof(struct Cyc_List_List));
# 538
_T107->hd=ptr;_T107->tl=typs;_TB9=(struct Cyc_List_List*)_T107;}typs=_TB9;goto _LL8;}case 91: goto _LL2C;case 112: _LL2C:{struct Cyc_Int_pa_PrintArg_struct _T107;_T107.tag=1;_TBC=c;_TBD=(int)_TBC;
# 542
_T107.f1=(unsigned long)_TBD;_TBB=_T107;}{struct Cyc_Int_pa_PrintArg_struct _T107=_TBB;void*_T108[1];_TBE=_T108 + 0;*_TBE=& _T107;{struct Cyc_List_List*(*_T109)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TBF=_T109;}_TC0=loc;_TC1=_tag_fat("%%%c is not supported",sizeof(char),22U);_TC2=_tag_fat(_T108,sizeof(void*),1);_TBA=_TBF(_TC0,_TC1,_TC2);}{struct Cyc_List_List*_T107=_TBA;_npop_handler(0);return _T107;}case 110:  {
# 544
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _T107;int _T108;if(lenmod!=0)goto _TL102;
# 546
t=Cyc_Absyn_sint_type;goto _LL50;_TL102: _TC3=lenmod;_TC4=(struct Cyc_List_List*)_TC3;_TC5=_TC4->tl;if(_TC5!=0)goto _TL104;_TC6=lenmod;{struct Cyc_List_List _T109=*_TC6;_TC7=_T109.hd;_T108=(int)_TC7;}_TC8=(int)_T108;if(_TC8!=108)goto _TL106;{int x=_T108;
t=Cyc_Absyn_ulong_type;goto _LL50;}_TL106: _TC9=lenmod;{struct Cyc_List_List _T109=*_TC9;_TCA=_T109.hd;_T108=(int)_TCA;}_TCB=(int)_T108;if(_TCB!=104)goto _TL108;{int x=_T108;
t=Cyc_Absyn_sshort_type;goto _LL50;}_TL108: goto _LL59;_TL104: _TCC=lenmod;_TCD=(struct Cyc_List_List*)_TCC;_TCE=_TCD->tl;_TCF=(struct Cyc_List_List*)_TCE;_TD0=_TCF->tl;if(_TD0!=0)goto _TL10A;_TD1=lenmod;{struct Cyc_List_List _T109=*_TD1;_TD2=_T109.hd;_T108=(int)_TD2;_TD3=_T109.tl;{struct Cyc_List_List _T10A=*_TD3;_TD4=_T10A.hd;_T107=(int)_TD4;}}_TD5=(int)_T108;if(_TD5!=104)goto _TL10C;_TD6=(int)_T107;if(_TD6!=104)goto _TL10C;{int x=_T108;int y=_T107;
t=Cyc_Absyn_schar_type;goto _LL50;}_TL10C: goto _LL59;_TL10A: _LL59:{struct Cyc_String_pa_PrintArg_struct _T109;_T109.tag=0;
# 552
_T109.f1=Cyc_implode(lenmod);_TD8=_T109;}{struct Cyc_String_pa_PrintArg_struct _T109=_TD8;{struct Cyc_Int_pa_PrintArg_struct _T10A;_T10A.tag=1;_TDA=c;_TDB=(int)_TDA;_T10A.f1=(unsigned long)_TDB;_TD9=_T10A;}{struct Cyc_Int_pa_PrintArg_struct _T10A=_TD9;void*_T10B[2];_TDC=_T10B + 0;*_TDC=& _T109;_TDD=_T10B + 1;*_TDD=& _T10A;{
# 551
struct Cyc_List_List*(*_T10C)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TDE=_T10C;}_TDF=loc;_TE0=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_TE1=_tag_fat(_T10B,sizeof(void*),2);_TD7=_TDE(_TDF,_TE0,_TE1);}}{struct Cyc_List_List*_T109=_TD7;_npop_handler(0);return _T109;}_LL50:;}_TE2=t;_TE3=& Cyc_Kinds_eko;_TE4=(struct Cyc_Core_Opt*)_TE3;{struct Cyc_Core_Opt*_T107=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 554
_T107->v=tvs;_TE5=(struct Cyc_Core_Opt*)_T107;}_TE6=Cyc_Absyn_new_evar(_TE4,_TE5);_TE7=Cyc_Absyn_al_qual_type;_TE8=
Cyc_Absyn_empty_tqual(0U);_TE9=Cyc_Absyn_false_type;_TEA=Cyc_Tcutil_any_bool(tvs);
# 554
t=Cyc_Absyn_at_type(_TE2,_TE6,_TE7,_TE8,_TE9,_TEA);{struct Cyc_List_List*_T107=_cycalloc(sizeof(struct Cyc_List_List));
# 557
_T107->hd=t;_T107->tl=typs;_TEB=(struct Cyc_List_List*)_T107;}typs=_TEB;goto _LL8;}case 37: _TEC=suppress;
# 560
if(!_TEC)goto _TL10E;{
struct Cyc_List_List*(*_T107)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TED=_T107;}_TEE=loc;_TEF=_tag_fat("Assignment suppression (*) is not allowed with %%%%",sizeof(char),52U);_TF0=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T107=_TED(_TEE,_TEF,_TF0);_npop_handler(0);return _T107;}_TL10E:
 if(width==0)goto _TL110;{struct Cyc_String_pa_PrintArg_struct _T107;_T107.tag=0;
_T107.f1=Cyc_implode(width);_TF2=_T107;}{struct Cyc_String_pa_PrintArg_struct _T107=_TF2;void*_T108[1];_TF3=_T108 + 0;*_TF3=& _T107;{struct Cyc_List_List*(*_T109)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TF4=_T109;}_TF5=loc;_TF6=_tag_fat("width '%s' not allowed with %%%%",sizeof(char),33U);_TF7=_tag_fat(_T108,sizeof(void*),1);_TF1=_TF4(_TF5,_TF6,_TF7);}{struct Cyc_List_List*_T107=_TF1;_npop_handler(0);return _T107;}_TL110:
 if(lenmod==0)goto _TL112;{struct Cyc_String_pa_PrintArg_struct _T107;_T107.tag=0;
# 566
_T107.f1=Cyc_implode(lenmod);_TF9=_T107;}{struct Cyc_String_pa_PrintArg_struct _T107=_TF9;void*_T108[1];_TFA=_T108 + 0;*_TFA=& _T107;{
# 565
struct Cyc_List_List*(*_T109)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TFB=_T109;}_TFC=loc;_TFD=_tag_fat("length modifier '%s' not allowed with %%%%",sizeof(char),43U);_TFE=_tag_fat(_T108,sizeof(void*),1);_TF8=_TFB(_TFC,_TFD,_TFE);}{struct Cyc_List_List*_T107=_TF8;_npop_handler(0);return _T107;}_TL112: goto _LL8;default:  {struct Cyc_List_List*_T107=0;_npop_handler(0);return _T107;}}_LL8:;}}}}_TLCA:
# 443
 i=i + 1;goto _TLCD;_TLCC: _pop_region();}_TFF=
# 572
Cyc_List_imp_rev(typs);return _TFF;}

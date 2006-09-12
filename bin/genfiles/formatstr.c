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
# 935 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 955
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 960
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;
# 962
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;
# 964
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 971
extern void*Cyc_Absyn_al_qual_type;
# 975
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 1026
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1030
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
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _RegionHandle*r,struct _fat_ptr s,int i){int _T0;unsigned long _T1;unsigned long _T2;int _T3;unsigned long _T4;unsigned long _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;const char*_T9;int _TA;char _TB;int _TC;struct Cyc_List_List*_TD;struct _RegionHandle*_TE;char _TF;int _T10;unsigned long _T11;unsigned long _T12;struct _fat_ptr _T13;unsigned char*_T14;const char*_T15;int _T16;char _T17;int _T18;struct Cyc_List_List*_T19;struct _RegionHandle*_T1A;char _T1B;int _T1C;unsigned long _T1D;unsigned long _T1E;struct _fat_ptr _T1F;unsigned char*_T20;const char*_T21;int _T22;char _T23;int _T24;int _T25;struct Cyc_List_List*_T26;struct _RegionHandle*_T27;char _T28;int _T29;unsigned long _T2A;unsigned long _T2B;struct _fat_ptr _T2C;unsigned char*_T2D;const char*_T2E;int _T2F;char _T30;int _T31;struct Cyc_List_List*_T32;struct _RegionHandle*_T33;char _T34;int _T35;unsigned long _T36;unsigned long _T37;struct _fat_ptr _T38;unsigned char*_T39;const char*_T3A;int _T3B;char _T3C;int _T3D;struct Cyc_List_List*_T3E;struct _RegionHandle*_T3F;char _T40;int _T41;unsigned long _T42;unsigned long _T43;struct _fat_ptr _T44;unsigned char*_T45;const char*_T46;int _T47;char _T48;int _T49;int _T4A;struct Cyc_List_List*_T4B;struct _RegionHandle*_T4C;char _T4D;int _T4E;unsigned long _T4F;unsigned long _T50;struct _fat_ptr _T51;unsigned char*_T52;const char*_T53;int _T54;char _T55;int _T56;struct Cyc_List_List*_T57;struct _RegionHandle*_T58;char _T59;int _T5A;unsigned long _T5B;unsigned long _T5C;struct _fat_ptr _T5D;unsigned char*_T5E;const char*_T5F;int _T60;char _T61;int _T62;struct Cyc_List_List*_T63;struct _RegionHandle*_T64;char _T65;struct Cyc_List_List*_T66;struct _RegionHandle*_T67;char _T68;int _T69;unsigned long _T6A;unsigned long _T6B;struct _fat_ptr _T6C;unsigned char*_T6D;const char*_T6E;int _T6F;char _T70;int _T71;struct Cyc_List_List*_T72;struct _RegionHandle*_T73;char _T74;struct Cyc_List_List*_T75;struct _RegionHandle*_T76;char _T77;int _T78;unsigned long _T79;unsigned long _T7A;struct _fat_ptr _T7B;unsigned char*_T7C;const char*_T7D;int _T7E;char _T7F;int _T80;struct Cyc_Core_Opt*_T81;struct _RegionHandle*_T82;struct _tuple11*_T83;struct _RegionHandle*_T84;
# 47
unsigned long len=Cyc_strlen(s);
if(i < 0)goto _TL2;else{goto _TL3;}_TL3: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 >= _T2)goto _TL2;else{goto _TL0;}_TL2: return 0;_TL0: {
# 51
struct Cyc_List_List*flags=0;
char c=' ';
_TL7: _T3=i;_T4=(unsigned long)_T3;_T5=len;if(_T4 < _T5)goto _TL5;else{goto _TL6;}
_TL5: _T6=s;_T7=_T6.curr;_T8=(const char*)_T7;_T9=_check_null(_T8);_TA=i;c=_T9[_TA];_TB=c;_TC=(int)_TB;switch(_TC){case 43: goto _LL4;case 45: _LL4: goto _LL6;case 32: _LL6: goto _LL8;case 35: _LL8: goto _LLA;case 48: _LLA: _TE=r;{struct Cyc_List_List*_T85=_region_malloc(_TE,0U,sizeof(struct Cyc_List_List));_TF=c;
# 60
_T85->hd=(void*)_TF;_T85->tl=flags;_TD=(struct Cyc_List_List*)_T85;}flags=_TD;goto _TL4;default: goto _LL0;}_LL0: goto _TL6;_TL4:
# 53
 i=i + 1;goto _TL7;_TL6: _T10=i;_T11=(unsigned long)_T10;_T12=len;
# 65
if(_T11 < _T12)goto _TL9;return 0;_TL9:
 flags=Cyc_List_imp_rev(flags);{
# 69
struct Cyc_List_List*width=0;_T13=s;_T14=_T13.curr;_T15=(const char*)_T14;_T16=i;
c=_T15[_T16];_T17=c;_T18=(int)_T17;
if(_T18!=42)goto _TLB;_T1A=r;{struct Cyc_List_List*_T85=_region_malloc(_T1A,0U,sizeof(struct Cyc_List_List));_T1B=c;
_T85->hd=(void*)_T1B;_T85->tl=width;_T19=(struct Cyc_List_List*)_T85;}width=_T19;
i=i + 1;goto _TLC;
# 75
_TLB: _TL10: _T1C=i;_T1D=(unsigned long)_T1C;_T1E=len;if(_T1D < _T1E)goto _TLE;else{goto _TLF;}
_TLE: _T1F=s;_T20=_T1F.curr;_T21=(const char*)_T20;_T22=i;c=_T21[_T22];_T23=c;_T24=(int)_T23;_T25=
isdigit(_T24);if(!_T25)goto _TL11;_T27=r;{struct Cyc_List_List*_T85=_region_malloc(_T27,0U,sizeof(struct Cyc_List_List));_T28=c;_T85->hd=(void*)_T28;_T85->tl=width;_T26=(struct Cyc_List_List*)_T85;}width=_T26;goto _TL12;
_TL11: goto _TLF;_TL12:
# 75
 i=i + 1;goto _TL10;_TLF: _TLC: _T29=i;_T2A=(unsigned long)_T29;_T2B=len;
# 81
if(_T2A < _T2B)goto _TL13;return 0;_TL13:
 width=Cyc_List_imp_rev(width);{
# 85
struct Cyc_List_List*precision=0;_T2C=s;_T2D=_T2C.curr;_T2E=(const char*)_T2D;_T2F=i;
c=_T2E[_T2F];_T30=c;_T31=(int)_T30;
if(_T31!=46)goto _TL15;_T33=r;{struct Cyc_List_List*_T85=_region_malloc(_T33,0U,sizeof(struct Cyc_List_List));_T34=c;
_T85->hd=(void*)_T34;_T85->tl=precision;_T32=(struct Cyc_List_List*)_T85;}precision=_T32;
i=i + 1;_T35=i;_T36=(unsigned long)_T35;_T37=len;if(_T36 < _T37)goto _TL17;return 0;_TL17: _T38=s;_T39=_T38.curr;_T3A=(const char*)_T39;_T3B=i;
c=_T3A[_T3B];_T3C=c;_T3D=(int)_T3C;
if(_T3D!=42)goto _TL19;_T3F=r;{struct Cyc_List_List*_T85=_region_malloc(_T3F,0U,sizeof(struct Cyc_List_List));_T40=c;
_T85->hd=(void*)_T40;_T85->tl=precision;_T3E=(struct Cyc_List_List*)_T85;}precision=_T3E;
i=i + 1;goto _TL1A;
# 95
_TL19: _TL1E: _T41=i;_T42=(unsigned long)_T41;_T43=len;if(_T42 < _T43)goto _TL1C;else{goto _TL1D;}
_TL1C: _T44=s;_T45=_T44.curr;_T46=(const char*)_T45;_T47=i;c=_T46[_T47];_T48=c;_T49=(int)_T48;_T4A=
isdigit(_T49);if(!_T4A)goto _TL1F;_T4C=r;{struct Cyc_List_List*_T85=_region_malloc(_T4C,0U,sizeof(struct Cyc_List_List));_T4D=c;_T85->hd=(void*)_T4D;_T85->tl=precision;_T4B=(struct Cyc_List_List*)_T85;}precision=_T4B;goto _TL20;
_TL1F: goto _TL1D;_TL20:
# 95
 i=i + 1;goto _TL1E;_TL1D: _TL1A: goto _TL16;_TL15: _TL16: _T4E=i;_T4F=(unsigned long)_T4E;_T50=len;
# 101
if(_T4F < _T50)goto _TL21;return 0;_TL21:
 precision=Cyc_List_imp_rev(precision);{
# 106
struct Cyc_List_List*lenmod=0;_T51=s;_T52=_T51.curr;_T53=(const char*)_T52;_T54=i;
c=_T53[_T54];_T55=c;_T56=(int)_T55;switch(_T56){case 104: _T58=r;{struct Cyc_List_List*_T85=_region_malloc(_T58,0U,sizeof(struct Cyc_List_List));_T59=c;
# 110
_T85->hd=(void*)_T59;_T85->tl=lenmod;_T57=(struct Cyc_List_List*)_T85;}lenmod=_T57;
i=i + 1;_T5A=i;_T5B=(unsigned long)_T5A;_T5C=len;if(_T5B < _T5C)goto _TL24;return 0;_TL24: _T5D=s;_T5E=_T5D.curr;_T5F=(const char*)_T5E;_T60=i;
c=_T5F[_T60];_T61=c;_T62=(int)_T61;
if(_T62!=104)goto _TL26;_T64=r;{struct Cyc_List_List*_T85=_region_malloc(_T64,0U,sizeof(struct Cyc_List_List));_T65=c;_T85->hd=(void*)_T65;_T85->tl=lenmod;_T63=(struct Cyc_List_List*)_T85;}lenmod=_T63;i=i + 1;goto _TL27;_TL26: _TL27: goto _LLD;case 108: _T67=r;{struct Cyc_List_List*_T85=_region_malloc(_T67,0U,sizeof(struct Cyc_List_List));_T68=c;
# 116
_T85->hd=(void*)_T68;_T85->tl=lenmod;_T66=(struct Cyc_List_List*)_T85;}lenmod=_T66;
i=i + 1;_T69=i;_T6A=(unsigned long)_T69;_T6B=len;if(_T6A < _T6B)goto _TL28;return 0;_TL28: _T6C=s;_T6D=_T6C.curr;_T6E=(const char*)_T6D;_T6F=i;
c=_T6E[_T6F];_T70=c;_T71=(int)_T70;
if(_T71!=108)goto _TL2A;_T73=r;{struct Cyc_List_List*_T85=_region_malloc(_T73,0U,sizeof(struct Cyc_List_List));_T74=c;_T85->hd=(void*)_T74;_T85->tl=lenmod;_T72=(struct Cyc_List_List*)_T85;}lenmod=_T72;i=i + 1;goto _TL2B;_TL2A: _TL2B: goto _LLD;case 106: goto _LL15;case 122: _LL15: goto _LL17;case 116: _LL17: goto _LL19;case 76: _LL19: _T76=r;{struct Cyc_List_List*_T85=_region_malloc(_T76,0U,sizeof(struct Cyc_List_List));_T77=c;
# 125
_T85->hd=(void*)_T77;_T85->tl=lenmod;_T75=(struct Cyc_List_List*)_T85;}lenmod=_T75;
i=i + 1;goto _LLD;default: goto _LLD;}_LLD: _T78=i;_T79=(unsigned long)_T78;_T7A=len;
# 130
if(_T79 < _T7A)goto _TL2C;return 0;_TL2C:
 lenmod=Cyc_List_imp_rev(lenmod);_T7B=s;_T7C=_T7B.curr;_T7D=(const char*)_T7C;_T7E=i;
# 134
c=_T7D[_T7E];_T7F=c;_T80=(int)_T7F;switch(_T80){case 100: goto _LL20;case 105: _LL20: goto _LL22;case 111: _LL22: goto _LL24;case 117: _LL24: goto _LL26;case 120: _LL26: goto _LL28;case 88: _LL28: goto _LL2A;case 102: _LL2A: goto _LL2C;case 70: _LL2C: goto _LL2E;case 101: _LL2E: goto _LL30;case 69: _LL30: goto _LL32;case 103: _LL32: goto _LL34;case 71: _LL34: goto _LL36;case 97: _LL36: goto _LL38;case 65: _LL38: goto _LL3A;case 99: _LL3A: goto _LL3C;case 115: _LL3C: goto _LL3E;case 112: _LL3E: goto _LL40;case 110: _LL40: goto _LL42;case 37: _LL42: goto _LL1C;default:
# 155
 return 0;}_LL1C: _T82=r;{struct Cyc_Core_Opt*_T85=_region_malloc(_T82,0U,sizeof(struct Cyc_Core_Opt));_T84=r;{struct _tuple11*_T86=_region_malloc(_T84,0U,sizeof(struct _tuple11));
# 166 "formatstr.cyc"
_T86->f0=flags;_T86->f1=width;_T86->f2=precision;_T86->f3=lenmod;_T86->f4=c;_T86->f5=i + 1;_T83=(struct _tuple11*)_T86;}_T85->v=_T83;_T81=(struct Cyc_Core_Opt*)_T85;}return _T81;}}}}}
# 169
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;const char*_T6;int _T7;char _T8;int _T9;struct _RegionHandle*_TA;struct _fat_ptr _TB;int _TC;struct Cyc_List_List*(*_TD)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_Core_Opt*_T11;void*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;void*_T17;int _T18;int _T19;int _T1A;struct Cyc_List_List*_T1B;struct Cyc_Int_pa_PrintArg_struct _T1C;int _T1D;struct Cyc_List_List*(*_T1E)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;void*_T26;int _T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;struct Cyc_List_List*_T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;void*_T32;struct Cyc_List_List*_T33;void*_T34;int _T35;int _T36;struct Cyc_List_List*_T37;char _T38;int _T39;struct Cyc_List_List*_T3A;void*_T3B;int _T3C;struct Cyc_List_List*_T3D;struct Cyc_Int_pa_PrintArg_struct _T3E;char _T3F;int _T40;struct Cyc_List_List*(*_T41)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T42;struct _fat_ptr _T43;struct _fat_ptr _T44;struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;struct Cyc_List_List*_T48;struct Cyc_List_List*_T49;void*_T4A;int _T4B;struct Cyc_List_List*_T4C;void*_T4D;int _T4E;struct Cyc_List_List*_T4F;struct Cyc_List_List*_T50;struct Cyc_List_List*_T51;struct Cyc_List_List*_T52;struct Cyc_List_List*_T53;struct Cyc_List_List*_T54;void*_T55;struct Cyc_List_List*_T56;void*_T57;int _T58;int _T59;struct Cyc_List_List*_T5A;struct Cyc_String_pa_PrintArg_struct _T5B;struct Cyc_Int_pa_PrintArg_struct _T5C;char _T5D;int _T5E;struct Cyc_List_List*(*_T5F)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T60;struct _fat_ptr _T61;struct _fat_ptr _T62;struct Cyc_List_List*_T63;struct Cyc_List_List*_T64;void*_T65;int _T66;struct Cyc_List_List*(*_T67)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T68;struct _fat_ptr _T69;struct _fat_ptr _T6A;struct Cyc_List_List*_T6B;struct Cyc_List_List*_T6C;struct Cyc_List_List*_T6D;struct Cyc_List_List*_T6E;struct Cyc_List_List*_T6F;void*_T70;int _T71;struct Cyc_List_List*_T72;void*_T73;int _T74;struct Cyc_List_List*_T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;struct Cyc_List_List*_T78;struct Cyc_List_List*_T79;struct Cyc_List_List*_T7A;void*_T7B;struct Cyc_List_List*_T7C;void*_T7D;int _T7E;int _T7F;struct Cyc_List_List*_T80;struct Cyc_String_pa_PrintArg_struct _T81;struct Cyc_Int_pa_PrintArg_struct _T82;char _T83;int _T84;struct Cyc_List_List*(*_T85)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T86;struct _fat_ptr _T87;struct _fat_ptr _T88;struct Cyc_List_List*_T89;struct Cyc_List_List*_T8A;struct Cyc_List_List*_T8B;struct Cyc_List_List*_T8C;struct Cyc_List_List*_T8D;struct Cyc_List_List*_T8E;void*_T8F;int _T90;struct Cyc_List_List*_T91;struct Cyc_List_List*_T92;struct Cyc_String_pa_PrintArg_struct _T93;struct Cyc_Int_pa_PrintArg_struct _T94;char _T95;int _T96;struct Cyc_List_List*(*_T97)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T98;struct _fat_ptr _T99;struct _fat_ptr _T9A;struct Cyc_List_List*_T9B;void*_T9C;int _T9D;struct Cyc_List_List*_T9E;void*_T9F;int _TA0;struct Cyc_List_List*_TA1;struct Cyc_Int_pa_PrintArg_struct _TA2;struct Cyc_List_List*_TA3;void*_TA4;int _TA5;struct Cyc_List_List*(*_TA6)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TA7;struct _fat_ptr _TA8;struct _fat_ptr _TA9;struct Cyc_List_List*_TAA;struct Cyc_List_List*_TAB;struct Cyc_String_pa_PrintArg_struct _TAC;struct Cyc_List_List*(*_TAD)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TAE;struct _fat_ptr _TAF;struct _fat_ptr _TB0;struct Cyc_List_List*_TB1;struct Cyc_String_pa_PrintArg_struct _TB2;struct Cyc_List_List*(*_TB3)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TB4;struct _fat_ptr _TB5;struct _fat_ptr _TB6;struct Cyc_List_List*_TB7;struct Cyc_List_List*_TB8;void*_TB9;int _TBA;struct Cyc_List_List*(*_TBB)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TBC;struct _fat_ptr _TBD;struct _fat_ptr _TBE;struct Cyc_List_List*_TBF;struct Cyc_List_List*(*_TC0)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TC1;struct _fat_ptr _TC2;struct _fat_ptr _TC3;int _TC4;void*_TC5;struct Cyc_Core_Opt*_TC6;struct Cyc_Core_Opt*_TC7;struct Cyc_Core_Opt*_TC8;void*_TC9;void*_TCA;struct Cyc_Absyn_Tqual _TCB;void*_TCC;void*_TCD;void*_TCE;struct Cyc_Core_Opt*_TCF;struct Cyc_Core_Opt*_TD0;struct Cyc_Core_Opt*_TD1;void*_TD2;void*_TD3;struct Cyc_Absyn_Tqual _TD4;void*_TD5;void*_TD6;struct Cyc_List_List*_TD7;struct Cyc_List_List*_TD8;struct Cyc_List_List*_TD9;void*_TDA;int _TDB;struct Cyc_List_List*_TDC;void*_TDD;int _TDE;struct Cyc_List_List*_TDF;struct Cyc_Int_pa_PrintArg_struct _TE0;struct Cyc_List_List*_TE1;void*_TE2;int _TE3;struct Cyc_List_List*(*_TE4)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TE5;struct _fat_ptr _TE6;struct _fat_ptr _TE7;struct Cyc_List_List*_TE8;struct Cyc_List_List*_TE9;struct Cyc_String_pa_PrintArg_struct _TEA;struct Cyc_List_List*(*_TEB)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TEC;struct _fat_ptr _TED;struct _fat_ptr _TEE;struct Cyc_List_List*_TEF;struct Cyc_List_List*_TF0;struct Cyc_List_List*_TF1;struct Cyc_List_List*_TF2;void*_TF3;int _TF4;struct Cyc_List_List*_TF5;void*_TF6;int _TF7;struct Cyc_List_List*_TF8;struct Cyc_List_List*_TF9;struct Cyc_List_List*_TFA;struct Cyc_List_List*_TFB;struct Cyc_List_List*_TFC;struct Cyc_List_List*_TFD;void*_TFE;struct Cyc_List_List*_TFF;void*_T100;int _T101;int _T102;struct Cyc_List_List*_T103;struct Cyc_String_pa_PrintArg_struct _T104;struct Cyc_Int_pa_PrintArg_struct _T105;char _T106;int _T107;struct Cyc_List_List*(*_T108)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T109;struct _fat_ptr _T10A;struct _fat_ptr _T10B;void*_T10C;struct Cyc_Core_Opt*_T10D;struct Cyc_Core_Opt*_T10E;struct Cyc_Core_Opt*_T10F;void*_T110;void*_T111;struct Cyc_Absyn_Tqual _T112;void*_T113;void*_T114;struct Cyc_List_List*_T115;struct Cyc_List_List*_T116;struct Cyc_String_pa_PrintArg_struct _T117;struct Cyc_List_List*(*_T118)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T119;struct _fat_ptr _T11A;struct _fat_ptr _T11B;struct Cyc_List_List*_T11C;struct Cyc_String_pa_PrintArg_struct _T11D;struct Cyc_List_List*(*_T11E)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T11F;struct _fat_ptr _T120;struct _fat_ptr _T121;struct Cyc_List_List*_T122;struct Cyc_String_pa_PrintArg_struct _T123;struct Cyc_List_List*(*_T124)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T125;struct _fat_ptr _T126;struct _fat_ptr _T127;struct Cyc_List_List*_T128;struct Cyc_String_pa_PrintArg_struct _T129;struct Cyc_List_List*(*_T12A)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T12B;struct _fat_ptr _T12C;struct _fat_ptr _T12D;
# 172
unsigned long len=Cyc_strlen(s);
struct Cyc_List_List*typs=0;
int i;struct _RegionHandle _T12E=_new_region(0U,"temp");struct _RegionHandle*temp=& _T12E;_push_region(temp);
# 176
i=0;_TL32: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 < _T2)goto _TL30;else{goto _TL31;}
_TL30: _T3=s;_T4=_T3.curr;_T5=(const char*)_T4;_T6=_check_null(_T5);_T7=i;_T8=_T6[_T7];_T9=(int)_T8;if(_T9==37)goto _TL33;goto _TL2F;_TL33: _TA=temp;_TB=s;_TC=i + 1;{
struct Cyc_Core_Opt*cs=Cyc_Formatstr_parse_conversionspecification(_TA,_TB,_TC);
if(cs!=0)goto _TL35;{
struct Cyc_List_List*(*_T12F)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TD=_T12F;}_TE=loc;_TF=_tag_fat("bad format string",sizeof(char),18U);_T10=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T12F=_TD(_TE,_TF,_T10);_npop_handler(0);return _T12F;}_TL35: _T11=cs;_T12=_T11->v;{
struct _tuple11*_T12F=(struct _tuple11*)_T12;int _T130;char _T131;struct Cyc_List_List*_T132;struct Cyc_List_List*_T133;struct Cyc_List_List*_T134;struct Cyc_List_List*_T135;{struct _tuple11 _T136=*_T12F;_T135=_T136.f0;_T134=_T136.f1;_T133=_T136.f2;_T132=_T136.f3;_T131=_T136.f4;_T130=_T136.f5;}{struct Cyc_List_List*flags=_T135;struct Cyc_List_List*width=_T134;struct Cyc_List_List*precision=_T133;struct Cyc_List_List*lenmod=_T132;char c=_T131;int j=_T130;
i=j - 1;{int _T136;if(lenmod==0)goto _TL37;_T13=lenmod;_T14=(struct Cyc_List_List*)_T13;_T15=_T14->tl;if(_T15!=0)goto _TL39;_T16=lenmod;{struct Cyc_List_List _T137=*_T16;_T17=_T137.hd;_T136=(int)_T17;}_T18=(int)_T136;if(_T18==106)goto _TL3D;else{goto _TL3F;}_TL3F: _T19=(int)_T136;if(_T19==122)goto _TL3D;else{goto _TL3E;}_TL3E: _T1A=(int)_T136;if(_T1A==116)goto _TL3D;else{goto _TL3B;}_TL3D:{int x=_T136;{struct Cyc_Int_pa_PrintArg_struct _T137;_T137.tag=1;_T1D=x;
# 187
_T137.f1=(unsigned long)_T1D;_T1C=_T137;}{struct Cyc_Int_pa_PrintArg_struct _T137=_T1C;void*_T138[1];_T138[0]=& _T137;{struct Cyc_List_List*(*_T139)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T1E=_T139;}_T1F=loc;_T20=_tag_fat("length modifier '%c' is not supported",sizeof(char),38U);_T21=_tag_fat(_T138,sizeof(void*),1);_T1B=_T1E(_T1F,_T20,_T21);}{struct Cyc_List_List*_T137=_T1B;_npop_handler(0);return _T137;}}_TL3B: goto _LL6;_TL39: goto _LL6;_TL37: _LL6: goto _LL3;_LL3:;}{int _T136;if(width==0)goto _TL40;_T22=width;_T23=(struct Cyc_List_List*)_T22;_T24=_T23->tl;if(_T24!=0)goto _TL42;_T25=width;{struct Cyc_List_List _T137=*_T25;_T26=_T137.hd;_T136=(int)_T26;}_T27=(int)_T136;if(_T27!=42)goto _TL44;{int x=_T136;{struct Cyc_List_List*_T137=_cycalloc(sizeof(struct Cyc_List_List));
# 191
_T137->hd=Cyc_Absyn_sint_type;_T137->tl=typs;_T28=(struct Cyc_List_List*)_T137;}typs=_T28;goto _LL8;}_TL44: goto _LLB;_TL42: goto _LLB;_TL40: _LLB: goto _LL8;_LL8:;}{int _T136;int _T137;if(precision==0)goto _TL46;_T29=precision;_T2A=(struct Cyc_List_List*)_T29;_T2B=_T2A->tl;if(_T2B==0)goto _TL48;_T2C=precision;_T2D=(struct Cyc_List_List*)_T2C;_T2E=_T2D->tl;_T2F=(struct Cyc_List_List*)_T2E;_T30=_T2F->tl;if(_T30!=0)goto _TL4A;_T31=precision;{struct Cyc_List_List _T138=*_T31;_T32=_T138.hd;_T137=(int)_T32;_T33=_T138.tl;{struct Cyc_List_List _T139=*_T33;_T34=_T139.hd;_T136=(int)_T34;}}_T35=(int)_T137;if(_T35!=46)goto _TL4C;_T36=(int)_T136;if(_T36!=42)goto _TL4C;{int x=_T137;int y=_T136;{struct Cyc_List_List*_T138=_cycalloc(sizeof(struct Cyc_List_List));
# 196
_T138->hd=Cyc_Absyn_sint_type;_T138->tl=typs;_T37=(struct Cyc_List_List*)_T138;}typs=_T37;goto _LLD;}_TL4C: goto _LL10;_TL4A: goto _LL10;_TL48: goto _LL10;_TL46: _LL10: goto _LLD;_LLD:;}{
# 199
void*t;_T38=c;_T39=(int)_T38;switch(_T39){case 100: goto _LL16;case 105: _LL16:{
# 203
struct Cyc_List_List*f=flags;_TL52: if(f!=0)goto _TL50;else{goto _TL51;}
_TL50: _T3A=f;_T3B=_T3A->hd;_T3C=(int)_T3B;if(_T3C!=35)goto _TL53;{struct Cyc_Int_pa_PrintArg_struct _T136;_T136.tag=1;_T3F=c;_T40=(int)_T3F;
_T136.f1=(unsigned long)_T40;_T3E=_T136;}{struct Cyc_Int_pa_PrintArg_struct _T136=_T3E;void*_T137[1];_T137[0]=& _T136;{struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T41=_T138;}_T42=loc;_T43=_tag_fat("flag '#' is not valid with %%%c",sizeof(char),32U);_T44=_tag_fat(_T137,sizeof(void*),1);_T3D=_T41(_T42,_T43,_T44);}{struct Cyc_List_List*_T136=_T3D;_npop_handler(0);return _T136;}_TL53: _T45=f;
# 203
f=_T45->tl;goto _TL52;_TL51:;}{int _T136;int _T137;if(lenmod!=0)goto _TL55;
# 207
t=Cyc_Absyn_sint_type;goto _LL3B;_TL55: _T46=lenmod;_T47=(struct Cyc_List_List*)_T46;_T48=_T47->tl;if(_T48!=0)goto _TL57;_T49=lenmod;{struct Cyc_List_List _T138=*_T49;_T4A=_T138.hd;_T137=(int)_T4A;}_T4B=(int)_T137;if(_T4B!=108)goto _TL59;{int x=_T137;
t=Cyc_Absyn_slong_type;goto _LL3B;}_TL59: _T4C=lenmod;{struct Cyc_List_List _T138=*_T4C;_T4D=_T138.hd;_T137=(int)_T4D;}_T4E=(int)_T137;if(_T4E!=104)goto _TL5B;{int x=_T137;
t=Cyc_Absyn_sshort_type;goto _LL3B;}_TL5B: goto _LL44;_TL57: _T4F=lenmod;_T50=(struct Cyc_List_List*)_T4F;_T51=_T50->tl;_T52=(struct Cyc_List_List*)_T51;_T53=_T52->tl;if(_T53!=0)goto _TL5D;_T54=lenmod;{struct Cyc_List_List _T138=*_T54;_T55=_T138.hd;_T137=(int)_T55;_T56=_T138.tl;{struct Cyc_List_List _T139=*_T56;_T57=_T139.hd;_T136=(int)_T57;}}_T58=(int)_T137;if(_T58!=104)goto _TL5F;_T59=(int)_T136;if(_T59!=104)goto _TL5F;{int x=_T137;int y=_T136;
# 211
t=Cyc_Absyn_schar_type;goto _LL3B;}_TL5F: goto _LL44;_TL5D: _LL44:{struct Cyc_String_pa_PrintArg_struct _T138;_T138.tag=0;
# 214
_T138.f1=Cyc_implode(lenmod);_T5B=_T138;}{struct Cyc_String_pa_PrintArg_struct _T138=_T5B;{struct Cyc_Int_pa_PrintArg_struct _T139;_T139.tag=1;_T5D=c;_T5E=(int)_T5D;_T139.f1=(unsigned long)_T5E;_T5C=_T139;}{struct Cyc_Int_pa_PrintArg_struct _T139=_T5C;void*_T13A[2];_T13A[0]=& _T138;_T13A[1]=& _T139;{
# 213
struct Cyc_List_List*(*_T13B)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T5F=_T13B;}_T60=loc;_T61=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T62=_tag_fat(_T13A,sizeof(void*),2);_T5A=_T5F(_T60,_T61,_T62);}}{struct Cyc_List_List*_T138=_T5A;_npop_handler(0);return _T138;}_LL3B:;}{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 216
_T136->hd=t;_T136->tl=typs;_T63=(struct Cyc_List_List*)_T136;}typs=_T63;goto _LL12;case 117:{
# 219
struct Cyc_List_List*f=flags;_TL64: if(f!=0)goto _TL62;else{goto _TL63;}
_TL62: _T64=f;_T65=_T64->hd;_T66=(int)_T65;if(_T66!=35)goto _TL65;{
struct Cyc_List_List*(*_T136)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T67=_T136;}_T68=loc;_T69=_tag_fat("Flag '#' not valid with %%u",sizeof(char),28U);_T6A=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T136=_T67(_T68,_T69,_T6A);_npop_handler(0);return _T136;}_TL65: _T6B=f;
# 219
f=_T6B->tl;goto _TL64;_TL63:;}goto _LL1A;case 111: _LL1A: goto _LL1C;case 120: _LL1C: goto _LL1E;case 88: _LL1E:{int _T136;int _T137;if(lenmod!=0)goto _TL67;
# 227
t=Cyc_Absyn_uint_type;goto _LL46;_TL67: _T6C=lenmod;_T6D=(struct Cyc_List_List*)_T6C;_T6E=_T6D->tl;if(_T6E!=0)goto _TL69;_T6F=lenmod;{struct Cyc_List_List _T138=*_T6F;_T70=_T138.hd;_T137=(int)_T70;}_T71=(int)_T137;if(_T71!=108)goto _TL6B;{int x=_T137;
t=Cyc_Absyn_ulong_type;goto _LL46;}_TL6B: _T72=lenmod;{struct Cyc_List_List _T138=*_T72;_T73=_T138.hd;_T137=(int)_T73;}_T74=(int)_T137;if(_T74!=104)goto _TL6D;{int x=_T137;
t=Cyc_Absyn_ushort_type;goto _LL46;}_TL6D: goto _LL4F;_TL69: _T75=lenmod;_T76=(struct Cyc_List_List*)_T75;_T77=_T76->tl;_T78=(struct Cyc_List_List*)_T77;_T79=_T78->tl;if(_T79!=0)goto _TL6F;_T7A=lenmod;{struct Cyc_List_List _T138=*_T7A;_T7B=_T138.hd;_T137=(int)_T7B;_T7C=_T138.tl;{struct Cyc_List_List _T139=*_T7C;_T7D=_T139.hd;_T136=(int)_T7D;}}_T7E=(int)_T137;if(_T7E!=104)goto _TL71;_T7F=(int)_T136;if(_T7F!=104)goto _TL71;{int x=_T137;int y=_T136;
# 231
t=Cyc_Absyn_uchar_type;goto _LL46;}_TL71: goto _LL4F;_TL6F: _LL4F:{struct Cyc_String_pa_PrintArg_struct _T138;_T138.tag=0;
# 235
_T138.f1=Cyc_implode(lenmod);_T81=_T138;}{struct Cyc_String_pa_PrintArg_struct _T138=_T81;{struct Cyc_Int_pa_PrintArg_struct _T139;_T139.tag=1;_T83=c;_T84=(int)_T83;_T139.f1=(unsigned long)_T84;_T82=_T139;}{struct Cyc_Int_pa_PrintArg_struct _T139=_T82;void*_T13A[2];_T13A[0]=& _T138;_T13A[1]=& _T139;{
# 234
struct Cyc_List_List*(*_T13B)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T85=_T13B;}_T86=loc;_T87=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T88=_tag_fat(_T13A,sizeof(void*),2);_T80=_T85(_T86,_T87,_T88);}}{struct Cyc_List_List*_T138=_T80;_npop_handler(0);return _T138;}_LL46:;}{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 237
_T136->hd=t;_T136->tl=typs;_T89=(struct Cyc_List_List*)_T136;}typs=_T89;goto _LL12;case 102: goto _LL22;case 70: _LL22: goto _LL24;case 101: _LL24: goto _LL26;case 69: _LL26: goto _LL28;case 103: _LL28: goto _LL2A;case 71: _LL2A: goto _LL2C;case 97: _LL2C: goto _LL2E;case 65: _LL2E:{int _T136;if(lenmod!=0)goto _TL73;{struct Cyc_List_List*_T137=_cycalloc(sizeof(struct Cyc_List_List));
# 255
_T137->hd=Cyc_Absyn_double_type;_T137->tl=typs;_T8A=(struct Cyc_List_List*)_T137;}typs=_T8A;goto _LL51;_TL73: _T8B=lenmod;_T8C=(struct Cyc_List_List*)_T8B;_T8D=_T8C->tl;if(_T8D!=0)goto _TL75;_T8E=lenmod;{struct Cyc_List_List _T137=*_T8E;_T8F=_T137.hd;_T136=(int)_T8F;}_T90=(int)_T136;if(_T90!=108)goto _TL77;{int x=_T136;{struct Cyc_List_List*_T137=_cycalloc(sizeof(struct Cyc_List_List));
# 257
_T137->hd=Cyc_Absyn_long_double_type;_T137->tl=typs;_T91=(struct Cyc_List_List*)_T137;}typs=_T91;goto _LL51;}_TL77: goto _LL56;_TL75: _LL56:{struct Cyc_String_pa_PrintArg_struct _T137;_T137.tag=0;
# 260
_T137.f1=Cyc_implode(lenmod);_T93=_T137;}{struct Cyc_String_pa_PrintArg_struct _T137=_T93;{struct Cyc_Int_pa_PrintArg_struct _T138;_T138.tag=1;_T95=c;_T96=(int)_T95;_T138.f1=(unsigned long)_T96;_T94=_T138;}{struct Cyc_Int_pa_PrintArg_struct _T138=_T94;void*_T139[2];_T139[0]=& _T137;_T139[1]=& _T138;{
# 259
struct Cyc_List_List*(*_T13A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T97=_T13A;}_T98=loc;_T99=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T9A=_tag_fat(_T139,sizeof(void*),2);_T92=_T97(_T98,_T99,_T9A);}}{struct Cyc_List_List*_T137=_T92;_npop_handler(0);return _T137;}_LL51:;}goto _LL12;case 99:{
# 264
struct Cyc_List_List*f=flags;_TL7C: if(f!=0)goto _TL7A;else{goto _TL7B;}
_TL7A: _T9B=f;_T9C=_T9B->hd;_T9D=(int)_T9C;if(_T9D==35)goto _TL7F;else{goto _TL80;}_TL80: _T9E=f;_T9F=_T9E->hd;_TA0=(int)_T9F;if(_TA0==48)goto _TL7F;else{goto _TL7D;}
_TL7F:{struct Cyc_Int_pa_PrintArg_struct _T136;_T136.tag=1;_TA3=f;_TA4=_TA3->hd;_TA5=(int)_TA4;_T136.f1=(unsigned long)_TA5;_TA2=_T136;}{struct Cyc_Int_pa_PrintArg_struct _T136=_TA2;void*_T137[1];_T137[0]=& _T136;{struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TA6=_T138;}_TA7=loc;_TA8=_tag_fat("flag '%c' not allowed with %%c",sizeof(char),31U);_TA9=_tag_fat(_T137,sizeof(void*),1);_TA1=_TA6(_TA7,_TA8,_TA9);}{struct Cyc_List_List*_T136=_TA1;_npop_handler(0);return _T136;}_TL7D: _TAA=f;
# 264
f=_TAA->tl;goto _TL7C;_TL7B:;}
# 269
if(lenmod==0)goto _TL81;{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;
# 271
_T136.f1=Cyc_implode(lenmod);_TAC=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_TAC;void*_T137[1];_T137[0]=& _T136;{
# 270
struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TAD=_T138;}_TAE=loc;_TAF=_tag_fat("length modifier '%s' not allowed with %%c",sizeof(char),42U);_TB0=_tag_fat(_T137,sizeof(void*),1);_TAB=_TAD(_TAE,_TAF,_TB0);}{struct Cyc_List_List*_T136=_TAB;_npop_handler(0);return _T136;}_TL81:
# 272
 if(precision==0)goto _TL83;{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;
# 274
_T136.f1=Cyc_implode(precision);_TB2=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_TB2;void*_T137[1];_T137[0]=& _T136;{
# 273
struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TB3=_T138;}_TB4=loc;_TB5=_tag_fat("precision '%s' not allowed with %%c",sizeof(char),36U);_TB6=_tag_fat(_T137,sizeof(void*),1);_TB1=_TB3(_TB4,_TB5,_TB6);}{struct Cyc_List_List*_T136=_TB1;_npop_handler(0);return _T136;}_TL83:{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 275
_T136->hd=Cyc_Absyn_sint_type;_T136->tl=typs;_TB7=(struct Cyc_List_List*)_T136;}typs=_TB7;goto _LL12;case 115:{
# 279
struct Cyc_List_List*f=flags;_TL88: if(f!=0)goto _TL86;else{goto _TL87;}
_TL86: _TB8=f;_TB9=_TB8->hd;_TBA=(int)_TB9;if(_TBA==45)goto _TL89;{
struct Cyc_List_List*(*_T136)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TBB=_T136;}_TBC=loc;_TBD=_tag_fat("a flag not allowed with %%s",sizeof(char),28U);_TBE=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T136=_TBB(_TBC,_TBD,_TBE);_npop_handler(0);return _T136;}_TL89: _TBF=f;
# 279
f=_TBF->tl;goto _TL88;_TL87:;}
# 284
if(lenmod==0)goto _TL8B;{
struct Cyc_List_List*(*_T136)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TC0=_T136;}_TC1=loc;_TC2=_tag_fat("length modifiers not allowed with %%s",sizeof(char),38U);_TC3=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T136=_TC0(_TC1,_TC2,_TC3);_npop_handler(0);return _T136;}_TL8B: {
# 288
void*ptr;
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);_TC4=isCproto;
if(_TC4)goto _TL8D;else{goto _TL8F;}
_TL8F: _TC5=Cyc_Absyn_char_type;_TC6=& Cyc_Kinds_eko;_TC7=(struct Cyc_Core_Opt*)_TC6;{struct Cyc_Core_Opt*_T136=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 293
_T136->v=tvs;_TC8=(struct Cyc_Core_Opt*)_T136;}_TC9=
# 292
Cyc_Absyn_new_evar(_TC7,_TC8);_TCA=Cyc_Absyn_al_qual_type;_TCB=
# 294
Cyc_Absyn_const_tqual(0U);_TCC=Cyc_Absyn_false_type;_TCD=Cyc_Tcutil_any_bool(tvs);
# 291
ptr=Cyc_Absyn_fatptr_type(_TC5,_TC9,_TCA,_TCB,_TCC,_TCD);goto _TL8E;
# 296
_TL8D: _TCE=Cyc_Absyn_char_type;_TCF=& Cyc_Kinds_eko;_TD0=(struct Cyc_Core_Opt*)_TCF;{struct Cyc_Core_Opt*_T136=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 298
_T136->v=tvs;_TD1=(struct Cyc_Core_Opt*)_T136;}_TD2=
# 297
Cyc_Absyn_new_evar(_TD0,_TD1);_TD3=Cyc_Absyn_al_qual_type;_TD4=
# 299
Cyc_Absyn_const_tqual(0U);_TD5=Cyc_Absyn_true_type;_TD6=Cyc_Tcutil_any_bool(tvs);
# 296
ptr=Cyc_Absyn_at_type(_TCE,_TD2,_TD3,_TD4,_TD5,_TD6);_TL8E:{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 300
_T136->hd=ptr;_T136->tl=typs;_TD7=(struct Cyc_List_List*)_T136;}typs=_TD7;goto _LL12;}case 112:{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 304
_T136->hd=Cyc_Absyn_uint_type;_T136->tl=typs;_TD8=(struct Cyc_List_List*)_T136;}typs=_TD8;goto _LL12;case 110:{
# 307
struct Cyc_List_List*f=flags;_TL93: if(f!=0)goto _TL91;else{goto _TL92;}
_TL91: _TD9=f;_TDA=_TD9->hd;_TDB=(int)_TDA;if(_TDB==35)goto _TL96;else{goto _TL97;}_TL97: _TDC=f;_TDD=_TDC->hd;_TDE=(int)_TDD;if(_TDE==48)goto _TL96;else{goto _TL94;}
_TL96:{struct Cyc_Int_pa_PrintArg_struct _T136;_T136.tag=1;_TE1=f;_TE2=_TE1->hd;_TE3=(int)_TE2;_T136.f1=(unsigned long)_TE3;_TE0=_T136;}{struct Cyc_Int_pa_PrintArg_struct _T136=_TE0;void*_T137[1];_T137[0]=& _T136;{struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TE4=_T138;}_TE5=loc;_TE6=_tag_fat("flag '%c' not allowed with %%n",sizeof(char),31U);_TE7=_tag_fat(_T137,sizeof(void*),1);_TDF=_TE4(_TE5,_TE6,_TE7);}{struct Cyc_List_List*_T136=_TDF;_npop_handler(0);return _T136;}_TL94: _TE8=f;
# 307
f=_TE8->tl;goto _TL93;_TL92:;}
# 310
if(precision==0)goto _TL98;{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;
# 312
_T136.f1=Cyc_implode(precision);_TEA=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_TEA;void*_T137[1];_T137[0]=& _T136;{
# 311
struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TEB=_T138;}_TEC=loc;_TED=_tag_fat("precision '%s' not allowed with %%n",sizeof(char),36U);_TEE=_tag_fat(_T137,sizeof(void*),1);_TE9=_TEB(_TEC,_TED,_TEE);}{struct Cyc_List_List*_T136=_TE9;_npop_handler(0);return _T136;}_TL98: {
# 313
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _T136;int _T137;if(lenmod!=0)goto _TL9A;
# 315
t=Cyc_Absyn_sint_type;goto _LL58;_TL9A: _TEF=lenmod;_TF0=(struct Cyc_List_List*)_TEF;_TF1=_TF0->tl;if(_TF1!=0)goto _TL9C;_TF2=lenmod;{struct Cyc_List_List _T138=*_TF2;_TF3=_T138.hd;_T137=(int)_TF3;}_TF4=(int)_T137;if(_TF4!=108)goto _TL9E;{int x=_T137;
# 317
t=Cyc_Absyn_ulong_type;goto _LL58;}_TL9E: _TF5=lenmod;{struct Cyc_List_List _T138=*_TF5;_TF6=_T138.hd;_T137=(int)_TF6;}_TF7=(int)_T137;if(_TF7!=104)goto _TLA0;{int x=_T137;
t=Cyc_Absyn_sshort_type;goto _LL58;}_TLA0: goto _LL61;_TL9C: _TF8=lenmod;_TF9=(struct Cyc_List_List*)_TF8;_TFA=_TF9->tl;_TFB=(struct Cyc_List_List*)_TFA;_TFC=_TFB->tl;if(_TFC!=0)goto _TLA2;_TFD=lenmod;{struct Cyc_List_List _T138=*_TFD;_TFE=_T138.hd;_T137=(int)_TFE;_TFF=_T138.tl;{struct Cyc_List_List _T139=*_TFF;_T100=_T139.hd;_T136=(int)_T100;}}_T101=(int)_T137;if(_T101!=104)goto _TLA4;_T102=(int)_T136;if(_T102!=104)goto _TLA4;{int x=_T137;int y=_T136;
# 320
t=Cyc_Absyn_schar_type;goto _LL58;}_TLA4: goto _LL61;_TLA2: _LL61:{struct Cyc_String_pa_PrintArg_struct _T138;_T138.tag=0;
# 323
_T138.f1=Cyc_implode(lenmod);_T104=_T138;}{struct Cyc_String_pa_PrintArg_struct _T138=_T104;{struct Cyc_Int_pa_PrintArg_struct _T139;_T139.tag=1;_T106=c;_T107=(int)_T106;_T139.f1=(unsigned long)_T107;_T105=_T139;}{struct Cyc_Int_pa_PrintArg_struct _T139=_T105;void*_T13A[2];_T13A[0]=& _T138;_T13A[1]=& _T139;{
# 322
struct Cyc_List_List*(*_T13B)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T108=_T13B;}_T109=loc;_T10A=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T10B=_tag_fat(_T13A,sizeof(void*),2);_T103=_T108(_T109,_T10A,_T10B);}}{struct Cyc_List_List*_T138=_T103;_npop_handler(0);return _T138;}_LL58:;}_T10C=t;_T10D=& Cyc_Kinds_eko;_T10E=(struct Cyc_Core_Opt*)_T10D;{struct Cyc_Core_Opt*_T136=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 325
_T136->v=tvs;_T10F=(struct Cyc_Core_Opt*)_T136;}_T110=Cyc_Absyn_new_evar(_T10E,_T10F);_T111=Cyc_Absyn_al_qual_type;_T112=Cyc_Absyn_empty_tqual(0U);_T113=Cyc_Absyn_false_type;_T114=
Cyc_Tcutil_any_bool(tvs);
# 325
t=Cyc_Absyn_at_type(_T10C,_T110,_T111,_T112,_T113,_T114);{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 327
_T136->hd=t;_T136->tl=typs;_T115=(struct Cyc_List_List*)_T136;}typs=_T115;goto _LL12;}case 37:
# 330
 if(flags==0)goto _TLA6;{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;
# 332
_T136.f1=Cyc_implode(flags);_T117=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_T117;void*_T137[1];_T137[0]=& _T136;{
# 331
struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T118=_T138;}_T119=loc;_T11A=_tag_fat("flags '%s' not allowed with %%%%",sizeof(char),33U);_T11B=_tag_fat(_T137,sizeof(void*),1);_T116=_T118(_T119,_T11A,_T11B);}{struct Cyc_List_List*_T136=_T116;_npop_handler(0);return _T136;}_TLA6:
# 333
 if(width==0)goto _TLA8;{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;
# 335
_T136.f1=Cyc_implode(width);_T11D=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_T11D;void*_T137[1];_T137[0]=& _T136;{
# 334
struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T11E=_T138;}_T11F=loc;_T120=_tag_fat("width '%s' not allowed with %%%%",sizeof(char),33U);_T121=_tag_fat(_T137,sizeof(void*),1);_T11C=_T11E(_T11F,_T120,_T121);}{struct Cyc_List_List*_T136=_T11C;_npop_handler(0);return _T136;}_TLA8:
# 336
 if(precision==0)goto _TLAA;{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;
# 338
_T136.f1=Cyc_implode(precision);_T123=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_T123;void*_T137[1];_T137[0]=& _T136;{
# 337
struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T124=_T138;}_T125=loc;_T126=_tag_fat("precision '%s' not allowed with %%%%",sizeof(char),37U);_T127=_tag_fat(_T137,sizeof(void*),1);_T122=_T124(_T125,_T126,_T127);}{struct Cyc_List_List*_T136=_T122;_npop_handler(0);return _T136;}_TLAA:
# 339
 if(lenmod==0)goto _TLAC;{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;
# 341
_T136.f1=Cyc_implode(lenmod);_T129=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_T129;void*_T137[1];_T137[0]=& _T136;{
# 340
struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T12A=_T138;}_T12B=loc;_T12C=_tag_fat("length modifier '%s' not allowed with %%%%",sizeof(char),43U);_T12D=_tag_fat(_T137,sizeof(void*),1);_T128=_T12A(_T12B,_T12C,_T12D);}{struct Cyc_List_List*_T136=_T128;_npop_handler(0);return _T136;}_TLAC: goto _LL12;default:  {struct Cyc_List_List*_T136=0;_npop_handler(0);return _T136;}}_LL12:;}}}}_TL2F:
# 176
 i=i + 1;goto _TL32;_TL31:{struct Cyc_List_List*_T12F=
# 346
Cyc_List_imp_rev(typs);_npop_handler(0);return _T12F;}_pop_region();}struct _tuple12{int f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;char f3;int f4;};
# 357 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct _fat_ptr s,int i){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;const char*_T6;int _T7;char _T8;int _T9;int _TA;unsigned long _TB;unsigned long _TC;int _TD;unsigned long _TE;unsigned long _TF;struct _fat_ptr _T10;unsigned char*_T11;const char*_T12;int _T13;char _T14;int _T15;int _T16;struct Cyc_List_List*_T17;struct _RegionHandle*_T18;char _T19;int _T1A;unsigned long _T1B;unsigned long _T1C;struct _fat_ptr _T1D;unsigned char*_T1E;const char*_T1F;int _T20;char _T21;int _T22;struct Cyc_List_List*_T23;struct _RegionHandle*_T24;char _T25;int _T26;unsigned long _T27;unsigned long _T28;struct _fat_ptr _T29;unsigned char*_T2A;const char*_T2B;int _T2C;char _T2D;int _T2E;struct Cyc_List_List*_T2F;struct _RegionHandle*_T30;char _T31;struct Cyc_List_List*_T32;struct _RegionHandle*_T33;char _T34;int _T35;unsigned long _T36;unsigned long _T37;struct _fat_ptr _T38;unsigned char*_T39;const char*_T3A;int _T3B;char _T3C;int _T3D;struct Cyc_List_List*_T3E;struct _RegionHandle*_T3F;char _T40;struct Cyc_List_List*_T41;struct _RegionHandle*_T42;char _T43;int _T44;unsigned long _T45;unsigned long _T46;struct _fat_ptr _T47;unsigned char*_T48;const char*_T49;int _T4A;char _T4B;int _T4C;struct Cyc_Core_Opt*_T4D;struct _RegionHandle*_T4E;struct _tuple12*_T4F;struct _RegionHandle*_T50;
# 359
unsigned long len=Cyc_strlen(s);
if(i < 0)goto _TLB0;else{goto _TLB1;}_TLB1: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 >= _T2)goto _TLB0;else{goto _TLAE;}_TLB0: return 0;_TLAE: {
# 362
int suppress=0;_T3=s;_T4=_T3.curr;_T5=(const char*)_T4;_T6=
_check_null(_T5);_T7=i;{char c=_T6[_T7];_T8=c;_T9=(int)_T8;
if(_T9!=42)goto _TLB2;
suppress=1;
i=i + 1;_TA=i;_TB=(unsigned long)_TA;_TC=len;
if(_TB < _TC)goto _TLB4;return 0;_TLB4: goto _TLB3;_TLB2: _TLB3: {
# 370
struct Cyc_List_List*width=0;
_TLB9: _TD=i;_TE=(unsigned long)_TD;_TF=len;if(_TE < _TF)goto _TLB7;else{goto _TLB8;}
_TLB7: _T10=s;_T11=_T10.curr;_T12=(const char*)_T11;_T13=i;c=_T12[_T13];_T14=c;_T15=(int)_T14;_T16=
isdigit(_T15);if(!_T16)goto _TLBA;_T18=r;{struct Cyc_List_List*_T51=_region_malloc(_T18,0U,sizeof(struct Cyc_List_List));_T19=c;_T51->hd=(void*)_T19;_T51->tl=width;_T17=(struct Cyc_List_List*)_T51;}width=_T17;goto _TLBB;
_TLBA: goto _TLB8;_TLBB:
# 371
 i=i + 1;goto _TLB9;_TLB8: _T1A=i;_T1B=(unsigned long)_T1A;_T1C=len;
# 376
if(_T1B < _T1C)goto _TLBC;return 0;_TLBC:
 width=Cyc_List_imp_rev(width);{
# 381
struct Cyc_List_List*lenmod=0;_T1D=s;_T1E=_T1D.curr;_T1F=(const char*)_T1E;_T20=i;
c=_T1F[_T20];_T21=c;_T22=(int)_T21;switch(_T22){case 104: _T24=r;{struct Cyc_List_List*_T51=_region_malloc(_T24,0U,sizeof(struct Cyc_List_List));_T25=c;
# 385
_T51->hd=(void*)_T25;_T51->tl=lenmod;_T23=(struct Cyc_List_List*)_T51;}lenmod=_T23;
i=i + 1;_T26=i;_T27=(unsigned long)_T26;_T28=len;
if(_T27 < _T28)goto _TLBF;return 0;_TLBF: _T29=s;_T2A=_T29.curr;_T2B=(const char*)_T2A;_T2C=i;
c=_T2B[_T2C];_T2D=c;_T2E=(int)_T2D;
if(_T2E!=104)goto _TLC1;_T30=r;{struct Cyc_List_List*_T51=_region_malloc(_T30,0U,sizeof(struct Cyc_List_List));_T31=c;_T51->hd=(void*)_T31;_T51->tl=lenmod;_T2F=(struct Cyc_List_List*)_T51;}lenmod=_T2F;i=i + 1;goto _TLC2;_TLC1: _TLC2: goto _LL0;case 108: _T33=r;{struct Cyc_List_List*_T51=_region_malloc(_T33,0U,sizeof(struct Cyc_List_List));_T34=c;
# 392
_T51->hd=(void*)_T34;_T51->tl=lenmod;_T32=(struct Cyc_List_List*)_T51;}lenmod=_T32;
i=i + 1;_T35=i;_T36=(unsigned long)_T35;_T37=len;
if(_T36 < _T37)goto _TLC3;return 0;_TLC3: _T38=s;_T39=_T38.curr;_T3A=(const char*)_T39;_T3B=i;
c=_T3A[_T3B];_T3C=c;_T3D=(int)_T3C;
if(_T3D!=108)goto _TLC5;_T3F=r;{struct Cyc_List_List*_T51=_region_malloc(_T3F,0U,sizeof(struct Cyc_List_List));_T40=c;_T51->hd=(void*)_T40;_T51->tl=lenmod;_T3E=(struct Cyc_List_List*)_T51;}lenmod=_T3E;i=i + 1;goto _TLC6;_TLC5: _TLC6: goto _LL0;case 106: goto _LL8;case 122: _LL8: goto _LLA;case 116: _LLA: goto _LLC;case 76: _LLC: _T42=r;{struct Cyc_List_List*_T51=_region_malloc(_T42,0U,sizeof(struct Cyc_List_List));_T43=c;
# 402
_T51->hd=(void*)_T43;_T51->tl=lenmod;_T41=(struct Cyc_List_List*)_T51;}lenmod=_T41;
i=i + 1;goto _LL0;default: goto _LL0;}_LL0: _T44=i;_T45=(unsigned long)_T44;_T46=len;
# 407
if(_T45 < _T46)goto _TLC7;return 0;_TLC7:
 lenmod=Cyc_List_imp_rev(lenmod);_T47=s;_T48=_T47.curr;_T49=(const char*)_T48;_T4A=i;
# 411
c=_T49[_T4A];_T4B=c;_T4C=(int)_T4B;switch(_T4C){case 100: goto _LL13;case 105: _LL13: goto _LL15;case 111: _LL15: goto _LL17;case 117: _LL17: goto _LL19;case 120: _LL19: goto _LL1B;case 88: _LL1B: goto _LL1D;case 102: _LL1D: goto _LL1F;case 70: _LL1F: goto _LL21;case 101: _LL21: goto _LL23;case 69: _LL23: goto _LL25;case 103: _LL25: goto _LL27;case 71: _LL27: goto _LL29;case 97: _LL29: goto _LL2B;case 65: _LL2B: goto _LL2D;case 99: _LL2D: goto _LL2F;case 115: _LL2F: goto _LL31;case 112: _LL31: goto _LL33;case 110: _LL33: goto _LL35;case 37: _LL35: goto _LLF;default:
# 432
 return 0;}_LLF: _T4E=r;{struct Cyc_Core_Opt*_T51=_region_malloc(_T4E,0U,sizeof(struct Cyc_Core_Opt));_T50=r;{struct _tuple12*_T52=_region_malloc(_T50,0U,sizeof(struct _tuple12));
# 434
_T52->f0=suppress;_T52->f1=width;_T52->f2=lenmod;_T52->f3=c;_T52->f4=i + 1;_T4F=(struct _tuple12*)_T52;}_T51->v=_T4F;_T4D=(struct Cyc_Core_Opt*)_T51;}return _T4D;}}}}}
# 436
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;int _T4;unsigned char*_T5;const char*_T6;char _T7;int _T8;struct _RegionHandle*_T9;struct _fat_ptr _TA;int _TB;struct Cyc_List_List*(*_TC)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Core_Opt*_T10;void*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;int _T17;int _T18;int _T19;struct Cyc_List_List*_T1A;struct Cyc_Int_pa_PrintArg_struct _T1B;int _T1C;struct Cyc_List_List*(*_T1D)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;int _T21;char _T22;int _T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;void*_T28;int _T29;struct Cyc_List_List*_T2A;void*_T2B;int _T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;struct Cyc_List_List*_T32;void*_T33;struct Cyc_List_List*_T34;void*_T35;int _T36;int _T37;struct Cyc_List_List*_T38;struct Cyc_String_pa_PrintArg_struct _T39;struct Cyc_Int_pa_PrintArg_struct _T3A;char _T3B;int _T3C;struct Cyc_List_List*(*_T3D)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T3E;struct _fat_ptr _T3F;struct _fat_ptr _T40;void*_T41;struct Cyc_Core_Opt*_T42;struct Cyc_Core_Opt*_T43;struct Cyc_Core_Opt*_T44;void*_T45;void*_T46;struct Cyc_Absyn_Tqual _T47;void*_T48;void*_T49;struct Cyc_List_List*_T4A;struct Cyc_List_List*_T4B;struct Cyc_List_List*_T4C;struct Cyc_List_List*_T4D;struct Cyc_List_List*_T4E;void*_T4F;int _T50;struct Cyc_List_List*_T51;void*_T52;int _T53;struct Cyc_List_List*_T54;struct Cyc_List_List*_T55;struct Cyc_List_List*_T56;struct Cyc_List_List*_T57;struct Cyc_List_List*_T58;struct Cyc_List_List*_T59;void*_T5A;struct Cyc_List_List*_T5B;void*_T5C;int _T5D;int _T5E;struct Cyc_List_List*_T5F;struct Cyc_String_pa_PrintArg_struct _T60;struct Cyc_Int_pa_PrintArg_struct _T61;char _T62;int _T63;struct Cyc_List_List*(*_T64)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T65;struct _fat_ptr _T66;struct _fat_ptr _T67;void*_T68;struct Cyc_Core_Opt*_T69;struct Cyc_Core_Opt*_T6A;struct Cyc_Core_Opt*_T6B;void*_T6C;void*_T6D;struct Cyc_Absyn_Tqual _T6E;void*_T6F;void*_T70;struct Cyc_List_List*_T71;struct Cyc_List_List*_T72;struct Cyc_List_List*_T73;struct Cyc_List_List*_T74;struct Cyc_List_List*_T75;void*_T76;int _T77;struct Cyc_List_List*_T78;struct Cyc_String_pa_PrintArg_struct _T79;struct Cyc_Int_pa_PrintArg_struct _T7A;char _T7B;int _T7C;struct Cyc_List_List*(*_T7D)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T7E;struct _fat_ptr _T7F;struct _fat_ptr _T80;void*_T81;struct Cyc_Core_Opt*_T82;struct Cyc_Core_Opt*_T83;struct Cyc_Core_Opt*_T84;void*_T85;void*_T86;struct Cyc_Absyn_Tqual _T87;void*_T88;void*_T89;struct Cyc_List_List*_T8A;int _T8B;void*_T8C;struct Cyc_Core_Opt*_T8D;struct Cyc_Core_Opt*_T8E;struct Cyc_Core_Opt*_T8F;void*_T90;void*_T91;struct Cyc_Absyn_Tqual _T92;void*_T93;void*_T94;void*_T95;struct Cyc_Core_Opt*_T96;struct Cyc_Core_Opt*_T97;struct Cyc_Core_Opt*_T98;void*_T99;void*_T9A;struct Cyc_Absyn_Tqual _T9B;void*_T9C;void*_T9D;struct Cyc_List_List*_T9E;int _T9F;void*_TA0;struct Cyc_Core_Opt*_TA1;struct Cyc_Core_Opt*_TA2;struct Cyc_Core_Opt*_TA3;void*_TA4;void*_TA5;struct Cyc_Absyn_Tqual _TA6;void*_TA7;void*_TA8;void*_TA9;struct Cyc_Core_Opt*_TAA;struct Cyc_Core_Opt*_TAB;struct Cyc_Core_Opt*_TAC;void*_TAD;void*_TAE;struct Cyc_Absyn_Tqual _TAF;void*_TB0;void*_TB1;struct Cyc_List_List*_TB2;struct Cyc_List_List*_TB3;struct Cyc_Int_pa_PrintArg_struct _TB4;char _TB5;int _TB6;struct Cyc_List_List*(*_TB7)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TB8;struct _fat_ptr _TB9;struct _fat_ptr _TBA;struct Cyc_List_List*_TBB;struct Cyc_List_List*_TBC;struct Cyc_List_List*_TBD;struct Cyc_List_List*_TBE;void*_TBF;int _TC0;struct Cyc_List_List*_TC1;void*_TC2;int _TC3;struct Cyc_List_List*_TC4;struct Cyc_List_List*_TC5;struct Cyc_List_List*_TC6;struct Cyc_List_List*_TC7;struct Cyc_List_List*_TC8;struct Cyc_List_List*_TC9;void*_TCA;struct Cyc_List_List*_TCB;void*_TCC;int _TCD;int _TCE;struct Cyc_List_List*_TCF;struct Cyc_String_pa_PrintArg_struct _TD0;struct Cyc_Int_pa_PrintArg_struct _TD1;char _TD2;int _TD3;struct Cyc_List_List*(*_TD4)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TD5;struct _fat_ptr _TD6;struct _fat_ptr _TD7;void*_TD8;struct Cyc_Core_Opt*_TD9;struct Cyc_Core_Opt*_TDA;struct Cyc_Core_Opt*_TDB;void*_TDC;void*_TDD;struct Cyc_Absyn_Tqual _TDE;void*_TDF;void*_TE0;struct Cyc_List_List*_TE1;int _TE2;struct Cyc_List_List*(*_TE3)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TE4;struct _fat_ptr _TE5;struct _fat_ptr _TE6;struct Cyc_List_List*_TE7;struct Cyc_String_pa_PrintArg_struct _TE8;struct Cyc_List_List*(*_TE9)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TEA;struct _fat_ptr _TEB;struct _fat_ptr _TEC;struct Cyc_List_List*_TED;struct Cyc_String_pa_PrintArg_struct _TEE;struct Cyc_List_List*(*_TEF)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TF0;struct _fat_ptr _TF1;struct _fat_ptr _TF2;struct Cyc_List_List*_TF3;
# 439
unsigned long len=Cyc_strlen(s);
struct Cyc_List_List*typs=0;
int i;{struct _RegionHandle _TF4=_new_region(0U,"temp");struct _RegionHandle*temp=& _TF4;_push_region(temp);
# 443
i=0;_TLCD: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 < _T2)goto _TLCB;else{goto _TLCC;}
_TLCB: _T3=s;_T4=i;_T5=_check_fat_subscript(_T3,sizeof(char),_T4);_T6=(const char*)_T5;_T7=*_T6;_T8=(int)_T7;if(_T8==37)goto _TLCE;goto _TLCA;_TLCE: _T9=temp;_TA=s;_TB=i + 1;{
struct Cyc_Core_Opt*x=Cyc_Formatstr_parse_inputformat(_T9,_TA,_TB);
if(x!=0)goto _TLD0;{
struct Cyc_List_List*(*_TF5)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TC=_TF5;}_TD=loc;_TE=_tag_fat("bad format string",sizeof(char),18U);_TF=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_TF5=_TC(_TD,_TE,_TF);_npop_handler(0);return _TF5;}_TLD0: _T10=x;_T11=_T10->v;{
struct _tuple12*_TF5=(struct _tuple12*)_T11;int _TF6;char _TF7;struct Cyc_List_List*_TF8;struct Cyc_List_List*_TF9;int _TFA;{struct _tuple12 _TFB=*_TF5;_TFA=_TFB.f0;_TF9=_TFB.f1;_TF8=_TFB.f2;_TF7=_TFB.f3;_TF6=_TFB.f4;}{int suppress=_TFA;struct Cyc_List_List*width=_TF9;struct Cyc_List_List*lenmod=_TF8;char c=_TF7;int j=_TF6;
i=j - 1;{int _TFB;if(lenmod==0)goto _TLD2;_T12=lenmod;_T13=(struct Cyc_List_List*)_T12;_T14=_T13->tl;if(_T14!=0)goto _TLD4;_T15=lenmod;{struct Cyc_List_List _TFC=*_T15;_T16=_TFC.hd;_TFB=(int)_T16;}_T17=(int)_TFB;if(_T17==106)goto _TLD8;else{goto _TLDA;}_TLDA: _T18=(int)_TFB;if(_T18==122)goto _TLD8;else{goto _TLD9;}_TLD9: _T19=(int)_TFB;if(_T19==116)goto _TLD8;else{goto _TLD6;}_TLD8:{int x=_TFB;{struct Cyc_Int_pa_PrintArg_struct _TFC;_TFC.tag=1;_T1C=x;
# 453
_TFC.f1=(unsigned long)_T1C;_T1B=_TFC;}{struct Cyc_Int_pa_PrintArg_struct _TFC=_T1B;void*_TFD[1];_TFD[0]=& _TFC;{struct Cyc_List_List*(*_TFE)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T1D=_TFE;}_T1E=loc;_T1F=_tag_fat("length modifier '%c' is not supported",sizeof(char),38U);_T20=_tag_fat(_TFD,sizeof(void*),1);_T1A=_T1D(_T1E,_T1F,_T20);}{struct Cyc_List_List*_TFC=_T1A;_npop_handler(0);return _TFC;}}_TLD6: goto _LL6;_TLD4: goto _LL6;_TLD2: _LL6: goto _LL3;_LL3:;}_T21=suppress;
# 456
if(!_T21)goto _TLDB;goto _TLCA;_TLDB: {
void*t;_T22=c;_T23=(int)_T22;switch(_T23){case 100: goto _LLC;case 105: _LLC: {
# 461
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _TFB;int _TFC;if(lenmod!=0)goto _TLDE;
# 463
t=Cyc_Absyn_sint_type;goto _LL33;_TLDE: _T24=lenmod;_T25=(struct Cyc_List_List*)_T24;_T26=_T25->tl;if(_T26!=0)goto _TLE0;_T27=lenmod;{struct Cyc_List_List _TFD=*_T27;_T28=_TFD.hd;_TFC=(int)_T28;}_T29=(int)_TFC;if(_T29!=108)goto _TLE2;{int x=_TFC;
t=Cyc_Absyn_slong_type;goto _LL33;}_TLE2: _T2A=lenmod;{struct Cyc_List_List _TFD=*_T2A;_T2B=_TFD.hd;_TFC=(int)_T2B;}_T2C=(int)_TFC;if(_T2C!=104)goto _TLE4;{int x=_TFC;
t=Cyc_Absyn_sshort_type;goto _LL33;}_TLE4: goto _LL3C;_TLE0: _T2D=lenmod;_T2E=(struct Cyc_List_List*)_T2D;_T2F=_T2E->tl;_T30=(struct Cyc_List_List*)_T2F;_T31=_T30->tl;if(_T31!=0)goto _TLE6;_T32=lenmod;{struct Cyc_List_List _TFD=*_T32;_T33=_TFD.hd;_TFC=(int)_T33;_T34=_TFD.tl;{struct Cyc_List_List _TFE=*_T34;_T35=_TFE.hd;_TFB=(int)_T35;}}_T36=(int)_TFC;if(_T36!=104)goto _TLE8;_T37=(int)_TFB;if(_T37!=104)goto _TLE8;{int x=_TFC;int y=_TFB;
t=Cyc_Absyn_schar_type;goto _LL33;}_TLE8: goto _LL3C;_TLE6: _LL3C:{struct Cyc_String_pa_PrintArg_struct _TFD;_TFD.tag=0;
# 469
_TFD.f1=Cyc_implode(lenmod);_T39=_TFD;}{struct Cyc_String_pa_PrintArg_struct _TFD=_T39;{struct Cyc_Int_pa_PrintArg_struct _TFE;_TFE.tag=1;_T3B=c;_T3C=(int)_T3B;_TFE.f1=(unsigned long)_T3C;_T3A=_TFE;}{struct Cyc_Int_pa_PrintArg_struct _TFE=_T3A;void*_TFF[2];_TFF[0]=& _TFD;_TFF[1]=& _TFE;{
# 468
struct Cyc_List_List*(*_T100)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T3D=_T100;}_T3E=loc;_T3F=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T40=_tag_fat(_TFF,sizeof(void*),2);_T38=_T3D(_T3E,_T3F,_T40);}}{struct Cyc_List_List*_TFD=_T38;_npop_handler(0);return _TFD;}_LL33:;}_T41=t;_T42=& Cyc_Kinds_eko;_T43=(struct Cyc_Core_Opt*)_T42;{struct Cyc_Core_Opt*_TFB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 471
_TFB->v=tvs;_T44=(struct Cyc_Core_Opt*)_TFB;}_T45=Cyc_Absyn_new_evar(_T43,_T44);_T46=Cyc_Absyn_al_qual_type;_T47=Cyc_Absyn_empty_tqual(0U);_T48=Cyc_Absyn_false_type;_T49=
Cyc_Tcutil_any_bool(tvs);
# 471
t=Cyc_Absyn_at_type(_T41,_T45,_T46,_T47,_T48,_T49);{struct Cyc_List_List*_TFB=_cycalloc(sizeof(struct Cyc_List_List));
# 473
_TFB->hd=t;_TFB->tl=typs;_T4A=(struct Cyc_List_List*)_TFB;}typs=_T4A;goto _LL8;}case 117: goto _LL10;case 111: _LL10: goto _LL12;case 120: _LL12: goto _LL14;case 88: _LL14: {
# 479
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _TFB;int _TFC;if(lenmod!=0)goto _TLEA;
# 481
t=Cyc_Absyn_uint_type;goto _LL3E;_TLEA: _T4B=lenmod;_T4C=(struct Cyc_List_List*)_T4B;_T4D=_T4C->tl;if(_T4D!=0)goto _TLEC;_T4E=lenmod;{struct Cyc_List_List _TFD=*_T4E;_T4F=_TFD.hd;_TFC=(int)_T4F;}_T50=(int)_TFC;if(_T50!=108)goto _TLEE;{int x=_TFC;
t=Cyc_Absyn_ulong_type;goto _LL3E;}_TLEE: _T51=lenmod;{struct Cyc_List_List _TFD=*_T51;_T52=_TFD.hd;_TFC=(int)_T52;}_T53=(int)_TFC;if(_T53!=104)goto _TLF0;{int x=_TFC;
t=Cyc_Absyn_ushort_type;goto _LL3E;}_TLF0: goto _LL47;_TLEC: _T54=lenmod;_T55=(struct Cyc_List_List*)_T54;_T56=_T55->tl;_T57=(struct Cyc_List_List*)_T56;_T58=_T57->tl;if(_T58!=0)goto _TLF2;_T59=lenmod;{struct Cyc_List_List _TFD=*_T59;_T5A=_TFD.hd;_TFC=(int)_T5A;_T5B=_TFD.tl;{struct Cyc_List_List _TFE=*_T5B;_T5C=_TFE.hd;_TFB=(int)_T5C;}}_T5D=(int)_TFC;if(_T5D!=104)goto _TLF4;_T5E=(int)_TFB;if(_T5E!=104)goto _TLF4;{int x=_TFC;int y=_TFB;
t=Cyc_Absyn_uchar_type;goto _LL3E;}_TLF4: goto _LL47;_TLF2: _LL47:{struct Cyc_String_pa_PrintArg_struct _TFD;_TFD.tag=0;
# 487
_TFD.f1=Cyc_implode(lenmod);_T60=_TFD;}{struct Cyc_String_pa_PrintArg_struct _TFD=_T60;{struct Cyc_Int_pa_PrintArg_struct _TFE;_TFE.tag=1;_T62=c;_T63=(int)_T62;_TFE.f1=(unsigned long)_T63;_T61=_TFE;}{struct Cyc_Int_pa_PrintArg_struct _TFE=_T61;void*_TFF[2];_TFF[0]=& _TFD;_TFF[1]=& _TFE;{
# 486
struct Cyc_List_List*(*_T100)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T64=_T100;}_T65=loc;_T66=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T67=_tag_fat(_TFF,sizeof(void*),2);_T5F=_T64(_T65,_T66,_T67);}}{struct Cyc_List_List*_TFD=_T5F;_npop_handler(0);return _TFD;}_LL3E:;}_T68=t;_T69=& Cyc_Kinds_eko;_T6A=(struct Cyc_Core_Opt*)_T69;{struct Cyc_Core_Opt*_TFB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 489
_TFB->v=tvs;_T6B=(struct Cyc_Core_Opt*)_TFB;}_T6C=Cyc_Absyn_new_evar(_T6A,_T6B);_T6D=Cyc_Absyn_al_qual_type;_T6E=Cyc_Absyn_empty_tqual(0U);_T6F=Cyc_Absyn_false_type;_T70=
Cyc_Tcutil_any_bool(tvs);
# 489
t=Cyc_Absyn_at_type(_T68,_T6C,_T6D,_T6E,_T6F,_T70);{struct Cyc_List_List*_TFB=_cycalloc(sizeof(struct Cyc_List_List));
# 491
_TFB->hd=t;_TFB->tl=typs;_T71=(struct Cyc_List_List*)_TFB;}typs=_T71;goto _LL8;}case 102: goto _LL18;case 70: _LL18: goto _LL1A;case 101: _LL1A: goto _LL1C;case 69: _LL1C: goto _LL1E;case 103: _LL1E: goto _LL20;case 71: _LL20: goto _LL22;case 97: _LL22: goto _LL24;case 65: _LL24: {
# 501
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _TFB;if(lenmod!=0)goto _TLF6;
# 503
t=Cyc_Absyn_float_type;goto _LL49;_TLF6: _T72=lenmod;_T73=(struct Cyc_List_List*)_T72;_T74=_T73->tl;if(_T74!=0)goto _TLF8;_T75=lenmod;{struct Cyc_List_List _TFC=*_T75;_T76=_TFC.hd;_TFB=(int)_T76;}_T77=(int)_TFB;if(_T77!=108)goto _TLFA;{int x=_TFB;
# 505
t=Cyc_Absyn_double_type;goto _LL49;}_TLFA: goto _LL4E;_TLF8: _LL4E:{struct Cyc_String_pa_PrintArg_struct _TFC;_TFC.tag=0;
# 508
_TFC.f1=Cyc_implode(lenmod);_T79=_TFC;}{struct Cyc_String_pa_PrintArg_struct _TFC=_T79;{struct Cyc_Int_pa_PrintArg_struct _TFD;_TFD.tag=1;_T7B=c;_T7C=(int)_T7B;_TFD.f1=(unsigned long)_T7C;_T7A=_TFD;}{struct Cyc_Int_pa_PrintArg_struct _TFD=_T7A;void*_TFE[2];_TFE[0]=& _TFC;_TFE[1]=& _TFD;{
# 507
struct Cyc_List_List*(*_TFF)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T7D=_TFF;}_T7E=loc;_T7F=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T80=_tag_fat(_TFE,sizeof(void*),2);_T78=_T7D(_T7E,_T7F,_T80);}}{struct Cyc_List_List*_TFC=_T78;_npop_handler(0);return _TFC;}_LL49:;}_T81=t;_T82=& Cyc_Kinds_eko;_T83=(struct Cyc_Core_Opt*)_T82;{struct Cyc_Core_Opt*_TFB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 510
_TFB->v=tvs;_T84=(struct Cyc_Core_Opt*)_TFB;}_T85=Cyc_Absyn_new_evar(_T83,_T84);_T86=Cyc_Absyn_al_qual_type;_T87=Cyc_Absyn_empty_tqual(0U);_T88=Cyc_Absyn_false_type;_T89=
Cyc_Tcutil_any_bool(tvs);
# 510
t=Cyc_Absyn_at_type(_T81,_T85,_T86,_T87,_T88,_T89);{struct Cyc_List_List*_TFB=_cycalloc(sizeof(struct Cyc_List_List));
# 512
_TFB->hd=t;_TFB->tl=typs;_T8A=(struct Cyc_List_List*)_TFB;}typs=_T8A;goto _LL8;}case 99:  {
# 516
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
void*ptr;_T8B=isCproto;
if(_T8B)goto _TLFC;else{goto _TLFE;}
_TLFE: _T8C=Cyc_Absyn_char_type;_T8D=& Cyc_Kinds_eko;_T8E=(struct Cyc_Core_Opt*)_T8D;{struct Cyc_Core_Opt*_TFB=_cycalloc(sizeof(struct Cyc_Core_Opt));_TFB->v=tvs;_T8F=(struct Cyc_Core_Opt*)_TFB;}_T90=Cyc_Absyn_new_evar(_T8E,_T8F);_T91=Cyc_Absyn_al_qual_type;_T92=
Cyc_Absyn_empty_tqual(0U);_T93=Cyc_Absyn_false_type;_T94=
Cyc_Tcutil_any_bool(tvs);
# 519
ptr=Cyc_Absyn_fatptr_type(_T8C,_T90,_T91,_T92,_T93,_T94);goto _TLFD;
# 523
_TLFC: _T95=Cyc_Absyn_char_type;_T96=& Cyc_Kinds_eko;_T97=(struct Cyc_Core_Opt*)_T96;{struct Cyc_Core_Opt*_TFB=_cycalloc(sizeof(struct Cyc_Core_Opt));
_TFB->v=tvs;_T98=(struct Cyc_Core_Opt*)_TFB;}_T99=Cyc_Absyn_new_evar(_T97,_T98);_T9A=Cyc_Absyn_al_qual_type;_T9B=
Cyc_Absyn_empty_tqual(0U);_T9C=Cyc_Absyn_false_type;_T9D=Cyc_Tcutil_any_bool(tvs);
# 523
ptr=Cyc_Absyn_at_type(_T95,_T99,_T9A,_T9B,_T9C,_T9D);_TLFD:{struct Cyc_List_List*_TFB=_cycalloc(sizeof(struct Cyc_List_List));
# 526
_TFB->hd=ptr;_TFB->tl=typs;_T9E=(struct Cyc_List_List*)_TFB;}typs=_T9E;goto _LL8;}case 115:  {
# 529
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
# 531
void*ptr;_T9F=isCproto;
if(_T9F)goto _TLFF;else{goto _TL101;}
_TL101: _TA0=Cyc_Absyn_char_type;_TA1=& Cyc_Kinds_eko;_TA2=(struct Cyc_Core_Opt*)_TA1;{struct Cyc_Core_Opt*_TFB=_cycalloc(sizeof(struct Cyc_Core_Opt));_TFB->v=tvs;_TA3=(struct Cyc_Core_Opt*)_TFB;}_TA4=Cyc_Absyn_new_evar(_TA2,_TA3);_TA5=Cyc_Absyn_al_qual_type;_TA6=
Cyc_Absyn_empty_tqual(0U);_TA7=Cyc_Absyn_false_type;_TA8=Cyc_Tcutil_any_bool(tvs);
# 533
ptr=Cyc_Absyn_fatptr_type(_TA0,_TA4,_TA5,_TA6,_TA7,_TA8);goto _TL100;
# 536
_TLFF: _TA9=Cyc_Absyn_char_type;_TAA=& Cyc_Kinds_eko;_TAB=(struct Cyc_Core_Opt*)_TAA;{struct Cyc_Core_Opt*_TFB=_cycalloc(sizeof(struct Cyc_Core_Opt));_TFB->v=tvs;_TAC=(struct Cyc_Core_Opt*)_TFB;}_TAD=Cyc_Absyn_new_evar(_TAB,_TAC);_TAE=Cyc_Absyn_al_qual_type;_TAF=
Cyc_Absyn_empty_tqual(0U);_TB0=Cyc_Absyn_true_type;_TB1=Cyc_Tcutil_any_bool(tvs);
# 536
ptr=Cyc_Absyn_at_type(_TA9,_TAD,_TAE,_TAF,_TB0,_TB1);_TL100:{struct Cyc_List_List*_TFB=_cycalloc(sizeof(struct Cyc_List_List));
# 538
_TFB->hd=ptr;_TFB->tl=typs;_TB2=(struct Cyc_List_List*)_TFB;}typs=_TB2;goto _LL8;}case 91: goto _LL2C;case 112: _LL2C:{struct Cyc_Int_pa_PrintArg_struct _TFB;_TFB.tag=1;_TB5=c;_TB6=(int)_TB5;
# 542
_TFB.f1=(unsigned long)_TB6;_TB4=_TFB;}{struct Cyc_Int_pa_PrintArg_struct _TFB=_TB4;void*_TFC[1];_TFC[0]=& _TFB;{struct Cyc_List_List*(*_TFD)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TB7=_TFD;}_TB8=loc;_TB9=_tag_fat("%%%c is not supported",sizeof(char),22U);_TBA=_tag_fat(_TFC,sizeof(void*),1);_TB3=_TB7(_TB8,_TB9,_TBA);}{struct Cyc_List_List*_TFB=_TB3;_npop_handler(0);return _TFB;}case 110:  {
# 544
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _TFB;int _TFC;if(lenmod!=0)goto _TL102;
# 546
t=Cyc_Absyn_sint_type;goto _LL50;_TL102: _TBB=lenmod;_TBC=(struct Cyc_List_List*)_TBB;_TBD=_TBC->tl;if(_TBD!=0)goto _TL104;_TBE=lenmod;{struct Cyc_List_List _TFD=*_TBE;_TBF=_TFD.hd;_TFC=(int)_TBF;}_TC0=(int)_TFC;if(_TC0!=108)goto _TL106;{int x=_TFC;
t=Cyc_Absyn_ulong_type;goto _LL50;}_TL106: _TC1=lenmod;{struct Cyc_List_List _TFD=*_TC1;_TC2=_TFD.hd;_TFC=(int)_TC2;}_TC3=(int)_TFC;if(_TC3!=104)goto _TL108;{int x=_TFC;
t=Cyc_Absyn_sshort_type;goto _LL50;}_TL108: goto _LL59;_TL104: _TC4=lenmod;_TC5=(struct Cyc_List_List*)_TC4;_TC6=_TC5->tl;_TC7=(struct Cyc_List_List*)_TC6;_TC8=_TC7->tl;if(_TC8!=0)goto _TL10A;_TC9=lenmod;{struct Cyc_List_List _TFD=*_TC9;_TCA=_TFD.hd;_TFC=(int)_TCA;_TCB=_TFD.tl;{struct Cyc_List_List _TFE=*_TCB;_TCC=_TFE.hd;_TFB=(int)_TCC;}}_TCD=(int)_TFC;if(_TCD!=104)goto _TL10C;_TCE=(int)_TFB;if(_TCE!=104)goto _TL10C;{int x=_TFC;int y=_TFB;
t=Cyc_Absyn_schar_type;goto _LL50;}_TL10C: goto _LL59;_TL10A: _LL59:{struct Cyc_String_pa_PrintArg_struct _TFD;_TFD.tag=0;
# 552
_TFD.f1=Cyc_implode(lenmod);_TD0=_TFD;}{struct Cyc_String_pa_PrintArg_struct _TFD=_TD0;{struct Cyc_Int_pa_PrintArg_struct _TFE;_TFE.tag=1;_TD2=c;_TD3=(int)_TD2;_TFE.f1=(unsigned long)_TD3;_TD1=_TFE;}{struct Cyc_Int_pa_PrintArg_struct _TFE=_TD1;void*_TFF[2];_TFF[0]=& _TFD;_TFF[1]=& _TFE;{
# 551
struct Cyc_List_List*(*_T100)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TD4=_T100;}_TD5=loc;_TD6=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_TD7=_tag_fat(_TFF,sizeof(void*),2);_TCF=_TD4(_TD5,_TD6,_TD7);}}{struct Cyc_List_List*_TFD=_TCF;_npop_handler(0);return _TFD;}_LL50:;}_TD8=t;_TD9=& Cyc_Kinds_eko;_TDA=(struct Cyc_Core_Opt*)_TD9;{struct Cyc_Core_Opt*_TFB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 554
_TFB->v=tvs;_TDB=(struct Cyc_Core_Opt*)_TFB;}_TDC=Cyc_Absyn_new_evar(_TDA,_TDB);_TDD=Cyc_Absyn_al_qual_type;_TDE=
Cyc_Absyn_empty_tqual(0U);_TDF=Cyc_Absyn_false_type;_TE0=Cyc_Tcutil_any_bool(tvs);
# 554
t=Cyc_Absyn_at_type(_TD8,_TDC,_TDD,_TDE,_TDF,_TE0);{struct Cyc_List_List*_TFB=_cycalloc(sizeof(struct Cyc_List_List));
# 557
_TFB->hd=t;_TFB->tl=typs;_TE1=(struct Cyc_List_List*)_TFB;}typs=_TE1;goto _LL8;}case 37: _TE2=suppress;
# 560
if(!_TE2)goto _TL10E;{
struct Cyc_List_List*(*_TFB)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TE3=_TFB;}_TE4=loc;_TE5=_tag_fat("Assignment suppression (*) is not allowed with %%%%",sizeof(char),52U);_TE6=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_TFB=_TE3(_TE4,_TE5,_TE6);_npop_handler(0);return _TFB;}_TL10E:
 if(width==0)goto _TL110;{struct Cyc_String_pa_PrintArg_struct _TFB;_TFB.tag=0;
_TFB.f1=Cyc_implode(width);_TE8=_TFB;}{struct Cyc_String_pa_PrintArg_struct _TFB=_TE8;void*_TFC[1];_TFC[0]=& _TFB;{struct Cyc_List_List*(*_TFD)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TE9=_TFD;}_TEA=loc;_TEB=_tag_fat("width '%s' not allowed with %%%%",sizeof(char),33U);_TEC=_tag_fat(_TFC,sizeof(void*),1);_TE7=_TE9(_TEA,_TEB,_TEC);}{struct Cyc_List_List*_TFB=_TE7;_npop_handler(0);return _TFB;}_TL110:
 if(lenmod==0)goto _TL112;{struct Cyc_String_pa_PrintArg_struct _TFB;_TFB.tag=0;
# 566
_TFB.f1=Cyc_implode(lenmod);_TEE=_TFB;}{struct Cyc_String_pa_PrintArg_struct _TFB=_TEE;void*_TFC[1];_TFC[0]=& _TFB;{
# 565
struct Cyc_List_List*(*_TFD)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TEF=_TFD;}_TF0=loc;_TF1=_tag_fat("length modifier '%s' not allowed with %%%%",sizeof(char),43U);_TF2=_tag_fat(_TFC,sizeof(void*),1);_TED=_TEF(_TF0,_TF1,_TF2);}{struct Cyc_List_List*_TFB=_TED;_npop_handler(0);return _TFB;}_TL112: goto _LL8;default:  {struct Cyc_List_List*_TFB=0;_npop_handler(0);return _TFB;}}_LL8:;}}}}_TLCA:
# 443
 i=i + 1;goto _TLCD;_TLCC: _pop_region();}_TF3=
# 572
Cyc_List_imp_rev(typs);return _TF3;}

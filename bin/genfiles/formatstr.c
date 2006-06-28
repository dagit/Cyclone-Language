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
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){int _T0;unsigned long _T1;unsigned long _T2;struct _fat_ptr _T3;unsigned char*_T4;const char*_T5;int _T6;char _T7;int _T8;struct _RegionHandle*_T9;struct _fat_ptr _TA;int _TB;struct Cyc_List_List*(*_TC)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Core_Opt*_T10;void*_T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;int _T17;int _T18;int _T19;struct Cyc_List_List*_T1A;struct Cyc_Int_pa_PrintArg_struct _T1B;int _T1C;struct Cyc_List_List*(*_T1D)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;void*_T25;int _T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;struct Cyc_List_List*_T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*_T30;void*_T31;struct Cyc_List_List*_T32;void*_T33;int _T34;int _T35;struct Cyc_List_List*_T36;char _T37;int _T38;struct Cyc_List_List*_T39;void*_T3A;int _T3B;struct Cyc_List_List*_T3C;struct Cyc_Int_pa_PrintArg_struct _T3D;char _T3E;int _T3F;struct Cyc_List_List*(*_T40)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T41;struct _fat_ptr _T42;struct _fat_ptr _T43;struct Cyc_List_List*_T44;struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;struct Cyc_List_List*_T48;void*_T49;int _T4A;struct Cyc_List_List*_T4B;void*_T4C;int _T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;struct Cyc_List_List*_T50;struct Cyc_List_List*_T51;struct Cyc_List_List*_T52;struct Cyc_List_List*_T53;void*_T54;struct Cyc_List_List*_T55;void*_T56;int _T57;int _T58;struct Cyc_List_List*_T59;struct Cyc_String_pa_PrintArg_struct _T5A;struct Cyc_Int_pa_PrintArg_struct _T5B;char _T5C;int _T5D;struct Cyc_List_List*(*_T5E)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T5F;struct _fat_ptr _T60;struct _fat_ptr _T61;struct Cyc_List_List*_T62;struct Cyc_List_List*_T63;void*_T64;int _T65;struct Cyc_List_List*(*_T66)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T67;struct _fat_ptr _T68;struct _fat_ptr _T69;struct Cyc_List_List*_T6A;struct Cyc_List_List*_T6B;struct Cyc_List_List*_T6C;struct Cyc_List_List*_T6D;struct Cyc_List_List*_T6E;void*_T6F;int _T70;struct Cyc_List_List*_T71;void*_T72;int _T73;struct Cyc_List_List*_T74;struct Cyc_List_List*_T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;struct Cyc_List_List*_T78;struct Cyc_List_List*_T79;void*_T7A;struct Cyc_List_List*_T7B;void*_T7C;int _T7D;int _T7E;struct Cyc_List_List*_T7F;struct Cyc_String_pa_PrintArg_struct _T80;struct Cyc_Int_pa_PrintArg_struct _T81;char _T82;int _T83;struct Cyc_List_List*(*_T84)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T85;struct _fat_ptr _T86;struct _fat_ptr _T87;struct Cyc_List_List*_T88;struct Cyc_List_List*_T89;struct Cyc_List_List*_T8A;struct Cyc_List_List*_T8B;struct Cyc_List_List*_T8C;struct Cyc_List_List*_T8D;void*_T8E;int _T8F;struct Cyc_List_List*_T90;struct Cyc_List_List*_T91;struct Cyc_String_pa_PrintArg_struct _T92;struct Cyc_Int_pa_PrintArg_struct _T93;char _T94;int _T95;struct Cyc_List_List*(*_T96)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T97;struct _fat_ptr _T98;struct _fat_ptr _T99;struct Cyc_List_List*_T9A;void*_T9B;int _T9C;struct Cyc_List_List*_T9D;void*_T9E;int _T9F;struct Cyc_List_List*_TA0;struct Cyc_Int_pa_PrintArg_struct _TA1;struct Cyc_List_List*_TA2;void*_TA3;int _TA4;struct Cyc_List_List*(*_TA5)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TA6;struct _fat_ptr _TA7;struct _fat_ptr _TA8;struct Cyc_List_List*_TA9;struct Cyc_List_List*_TAA;struct Cyc_String_pa_PrintArg_struct _TAB;struct Cyc_List_List*(*_TAC)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TAD;struct _fat_ptr _TAE;struct _fat_ptr _TAF;struct Cyc_List_List*_TB0;struct Cyc_String_pa_PrintArg_struct _TB1;struct Cyc_List_List*(*_TB2)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TB3;struct _fat_ptr _TB4;struct _fat_ptr _TB5;struct Cyc_List_List*_TB6;struct Cyc_List_List*_TB7;void*_TB8;int _TB9;struct Cyc_List_List*(*_TBA)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TBB;struct _fat_ptr _TBC;struct _fat_ptr _TBD;struct Cyc_List_List*_TBE;struct Cyc_List_List*(*_TBF)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TC0;struct _fat_ptr _TC1;struct _fat_ptr _TC2;int _TC3;void*_TC4;struct Cyc_Core_Opt*_TC5;struct Cyc_Core_Opt*_TC6;struct Cyc_Core_Opt*_TC7;void*_TC8;void*_TC9;struct Cyc_Absyn_Tqual _TCA;void*_TCB;void*_TCC;void*_TCD;struct Cyc_Core_Opt*_TCE;struct Cyc_Core_Opt*_TCF;struct Cyc_Core_Opt*_TD0;void*_TD1;void*_TD2;struct Cyc_Absyn_Tqual _TD3;void*_TD4;void*_TD5;struct Cyc_List_List*_TD6;struct Cyc_List_List*_TD7;struct Cyc_List_List*_TD8;void*_TD9;int _TDA;struct Cyc_List_List*_TDB;void*_TDC;int _TDD;struct Cyc_List_List*_TDE;struct Cyc_Int_pa_PrintArg_struct _TDF;struct Cyc_List_List*_TE0;void*_TE1;int _TE2;struct Cyc_List_List*(*_TE3)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TE4;struct _fat_ptr _TE5;struct _fat_ptr _TE6;struct Cyc_List_List*_TE7;struct Cyc_List_List*_TE8;struct Cyc_String_pa_PrintArg_struct _TE9;struct Cyc_List_List*(*_TEA)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _TEB;struct _fat_ptr _TEC;struct _fat_ptr _TED;struct Cyc_List_List*_TEE;struct Cyc_List_List*_TEF;struct Cyc_List_List*_TF0;struct Cyc_List_List*_TF1;void*_TF2;int _TF3;struct Cyc_List_List*_TF4;void*_TF5;int _TF6;struct Cyc_List_List*_TF7;struct Cyc_List_List*_TF8;struct Cyc_List_List*_TF9;struct Cyc_List_List*_TFA;struct Cyc_List_List*_TFB;struct Cyc_List_List*_TFC;void*_TFD;struct Cyc_List_List*_TFE;void*_TFF;int _T100;int _T101;struct Cyc_List_List*_T102;struct Cyc_String_pa_PrintArg_struct _T103;struct Cyc_Int_pa_PrintArg_struct _T104;char _T105;int _T106;struct Cyc_List_List*(*_T107)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T108;struct _fat_ptr _T109;struct _fat_ptr _T10A;void*_T10B;struct Cyc_Core_Opt*_T10C;struct Cyc_Core_Opt*_T10D;struct Cyc_Core_Opt*_T10E;void*_T10F;void*_T110;struct Cyc_Absyn_Tqual _T111;void*_T112;void*_T113;struct Cyc_List_List*_T114;struct Cyc_List_List*_T115;struct Cyc_String_pa_PrintArg_struct _T116;struct Cyc_List_List*(*_T117)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T118;struct _fat_ptr _T119;struct _fat_ptr _T11A;struct Cyc_List_List*_T11B;struct Cyc_String_pa_PrintArg_struct _T11C;struct Cyc_List_List*(*_T11D)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T11E;struct _fat_ptr _T11F;struct _fat_ptr _T120;struct Cyc_List_List*_T121;struct Cyc_String_pa_PrintArg_struct _T122;struct Cyc_List_List*(*_T123)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T124;struct _fat_ptr _T125;struct _fat_ptr _T126;struct Cyc_List_List*_T127;struct Cyc_String_pa_PrintArg_struct _T128;struct Cyc_List_List*(*_T129)(unsigned,struct _fat_ptr,struct _fat_ptr ap);unsigned _T12A;struct _fat_ptr _T12B;struct _fat_ptr _T12C;
# 172
unsigned long len=Cyc_strlen(s);
struct Cyc_List_List*typs=0;
int i;struct _RegionHandle _T12D=_new_region(0U,"temp");struct _RegionHandle*temp=& _T12D;_push_region(temp);
# 176
i=0;_TL32: _T0=i;_T1=(unsigned long)_T0;_T2=len;if(_T1 < _T2)goto _TL30;else{goto _TL31;}
_TL30: _T3=s;_T4=_T3.curr;_T5=(const char*)_T4;_T6=i;_T7=_T5[_T6];_T8=(int)_T7;if(_T8==37)goto _TL33;goto _TL2F;_TL33: _T9=temp;_TA=s;_TB=i + 1;{
struct Cyc_Core_Opt*cs=Cyc_Formatstr_parse_conversionspecification(_T9,_TA,_TB);
if(cs!=0)goto _TL35;{
struct Cyc_List_List*(*_T12E)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TC=_T12E;}_TD=loc;_TE=_tag_fat("bad format string",sizeof(char),18U);_TF=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T12E=_TC(_TD,_TE,_TF);_npop_handler(0);return _T12E;}_TL35: _T10=cs;_T11=_T10->v;{
struct _tuple11*_T12E=(struct _tuple11*)_T11;int _T12F;char _T130;struct Cyc_List_List*_T131;struct Cyc_List_List*_T132;struct Cyc_List_List*_T133;struct Cyc_List_List*_T134;{struct _tuple11 _T135=*_T12E;_T134=_T135.f0;_T133=_T135.f1;_T132=_T135.f2;_T131=_T135.f3;_T130=_T135.f4;_T12F=_T135.f5;}{struct Cyc_List_List*flags=_T134;struct Cyc_List_List*width=_T133;struct Cyc_List_List*precision=_T132;struct Cyc_List_List*lenmod=_T131;char c=_T130;int j=_T12F;
i=j - 1;{int _T135;if(lenmod==0)goto _TL37;_T12=lenmod;_T13=(struct Cyc_List_List*)_T12;_T14=_T13->tl;if(_T14!=0)goto _TL39;_T15=lenmod;{struct Cyc_List_List _T136=*_T15;_T16=_T136.hd;_T135=(int)_T16;}_T17=(int)_T135;if(_T17==106)goto _TL3D;else{goto _TL3F;}_TL3F: _T18=(int)_T135;if(_T18==122)goto _TL3D;else{goto _TL3E;}_TL3E: _T19=(int)_T135;if(_T19==116)goto _TL3D;else{goto _TL3B;}_TL3D:{int x=_T135;{struct Cyc_Int_pa_PrintArg_struct _T136;_T136.tag=1;_T1C=x;
# 187
_T136.f1=(unsigned long)_T1C;_T1B=_T136;}{struct Cyc_Int_pa_PrintArg_struct _T136=_T1B;void*_T137[1];_T137[0]=& _T136;{struct Cyc_List_List*(*_T138)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T1D=_T138;}_T1E=loc;_T1F=_tag_fat("length modifier '%c' is not supported",sizeof(char),38U);_T20=_tag_fat(_T137,sizeof(void*),1);_T1A=_T1D(_T1E,_T1F,_T20);}{struct Cyc_List_List*_T136=_T1A;_npop_handler(0);return _T136;}}_TL3B: goto _LL6;_TL39: goto _LL6;_TL37: _LL6: goto _LL3;_LL3:;}{int _T135;if(width==0)goto _TL40;_T21=width;_T22=(struct Cyc_List_List*)_T21;_T23=_T22->tl;if(_T23!=0)goto _TL42;_T24=width;{struct Cyc_List_List _T136=*_T24;_T25=_T136.hd;_T135=(int)_T25;}_T26=(int)_T135;if(_T26!=42)goto _TL44;{int x=_T135;{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 191
_T136->hd=Cyc_Absyn_sint_type;_T136->tl=typs;_T27=(struct Cyc_List_List*)_T136;}typs=_T27;goto _LL8;}_TL44: goto _LLB;_TL42: goto _LLB;_TL40: _LLB: goto _LL8;_LL8:;}{int _T135;int _T136;if(precision==0)goto _TL46;_T28=precision;_T29=(struct Cyc_List_List*)_T28;_T2A=_T29->tl;if(_T2A==0)goto _TL48;_T2B=precision;_T2C=(struct Cyc_List_List*)_T2B;_T2D=_T2C->tl;_T2E=(struct Cyc_List_List*)_T2D;_T2F=_T2E->tl;if(_T2F!=0)goto _TL4A;_T30=precision;{struct Cyc_List_List _T137=*_T30;_T31=_T137.hd;_T136=(int)_T31;_T32=_T137.tl;{struct Cyc_List_List _T138=*_T32;_T33=_T138.hd;_T135=(int)_T33;}}_T34=(int)_T136;if(_T34!=46)goto _TL4C;_T35=(int)_T135;if(_T35!=42)goto _TL4C;{int x=_T136;int y=_T135;{struct Cyc_List_List*_T137=_cycalloc(sizeof(struct Cyc_List_List));
# 196
_T137->hd=Cyc_Absyn_sint_type;_T137->tl=typs;_T36=(struct Cyc_List_List*)_T137;}typs=_T36;goto _LLD;}_TL4C: goto _LL10;_TL4A: goto _LL10;_TL48: goto _LL10;_TL46: _LL10: goto _LLD;_LLD:;}{
# 199
void*t;_T37=c;_T38=(int)_T37;switch(_T38){case 100: goto _LL16;case 105: _LL16:{
# 203
struct Cyc_List_List*f=flags;_TL52: if(f!=0)goto _TL50;else{goto _TL51;}
_TL50: _T39=f;_T3A=_T39->hd;_T3B=(int)_T3A;if(_T3B!=35)goto _TL53;{struct Cyc_Int_pa_PrintArg_struct _T135;_T135.tag=1;_T3E=c;_T3F=(int)_T3E;
_T135.f1=(unsigned long)_T3F;_T3D=_T135;}{struct Cyc_Int_pa_PrintArg_struct _T135=_T3D;void*_T136[1];_T136[0]=& _T135;{struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T40=_T137;}_T41=loc;_T42=_tag_fat("flag '#' is not valid with %%%c",sizeof(char),32U);_T43=_tag_fat(_T136,sizeof(void*),1);_T3C=_T40(_T41,_T42,_T43);}{struct Cyc_List_List*_T135=_T3C;_npop_handler(0);return _T135;}_TL53: _T44=f;
# 203
f=_T44->tl;goto _TL52;_TL51:;}{int _T135;int _T136;if(lenmod!=0)goto _TL55;
# 207
t=Cyc_Absyn_sint_type;goto _LL3B;_TL55: _T45=lenmod;_T46=(struct Cyc_List_List*)_T45;_T47=_T46->tl;if(_T47!=0)goto _TL57;_T48=lenmod;{struct Cyc_List_List _T137=*_T48;_T49=_T137.hd;_T136=(int)_T49;}_T4A=(int)_T136;if(_T4A!=108)goto _TL59;{int x=_T136;
t=Cyc_Absyn_slong_type;goto _LL3B;}_TL59: _T4B=lenmod;{struct Cyc_List_List _T137=*_T4B;_T4C=_T137.hd;_T136=(int)_T4C;}_T4D=(int)_T136;if(_T4D!=104)goto _TL5B;{int x=_T136;
t=Cyc_Absyn_sshort_type;goto _LL3B;}_TL5B: goto _LL44;_TL57: _T4E=lenmod;_T4F=(struct Cyc_List_List*)_T4E;_T50=_T4F->tl;_T51=(struct Cyc_List_List*)_T50;_T52=_T51->tl;if(_T52!=0)goto _TL5D;_T53=lenmod;{struct Cyc_List_List _T137=*_T53;_T54=_T137.hd;_T136=(int)_T54;_T55=_T137.tl;{struct Cyc_List_List _T138=*_T55;_T56=_T138.hd;_T135=(int)_T56;}}_T57=(int)_T136;if(_T57!=104)goto _TL5F;_T58=(int)_T135;if(_T58!=104)goto _TL5F;{int x=_T136;int y=_T135;
# 211
t=Cyc_Absyn_schar_type;goto _LL3B;}_TL5F: goto _LL44;_TL5D: _LL44:{struct Cyc_String_pa_PrintArg_struct _T137;_T137.tag=0;
# 214
_T137.f1=Cyc_implode(lenmod);_T5A=_T137;}{struct Cyc_String_pa_PrintArg_struct _T137=_T5A;{struct Cyc_Int_pa_PrintArg_struct _T138;_T138.tag=1;_T5C=c;_T5D=(int)_T5C;_T138.f1=(unsigned long)_T5D;_T5B=_T138;}{struct Cyc_Int_pa_PrintArg_struct _T138=_T5B;void*_T139[2];_T139[0]=& _T137;_T139[1]=& _T138;{
# 213
struct Cyc_List_List*(*_T13A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T5E=_T13A;}_T5F=loc;_T60=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T61=_tag_fat(_T139,sizeof(void*),2);_T59=_T5E(_T5F,_T60,_T61);}}{struct Cyc_List_List*_T137=_T59;_npop_handler(0);return _T137;}_LL3B:;}{struct Cyc_List_List*_T135=_cycalloc(sizeof(struct Cyc_List_List));
# 216
_T135->hd=t;_T135->tl=typs;_T62=(struct Cyc_List_List*)_T135;}typs=_T62;goto _LL12;case 117:{
# 219
struct Cyc_List_List*f=flags;_TL64: if(f!=0)goto _TL62;else{goto _TL63;}
_TL62: _T63=f;_T64=_T63->hd;_T65=(int)_T64;if(_T65!=35)goto _TL65;{
struct Cyc_List_List*(*_T135)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T66=_T135;}_T67=loc;_T68=_tag_fat("Flag '#' not valid with %%u",sizeof(char),28U);_T69=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T135=_T66(_T67,_T68,_T69);_npop_handler(0);return _T135;}_TL65: _T6A=f;
# 219
f=_T6A->tl;goto _TL64;_TL63:;}goto _LL1A;case 111: _LL1A: goto _LL1C;case 120: _LL1C: goto _LL1E;case 88: _LL1E:{int _T135;int _T136;if(lenmod!=0)goto _TL67;
# 227
t=Cyc_Absyn_uint_type;goto _LL46;_TL67: _T6B=lenmod;_T6C=(struct Cyc_List_List*)_T6B;_T6D=_T6C->tl;if(_T6D!=0)goto _TL69;_T6E=lenmod;{struct Cyc_List_List _T137=*_T6E;_T6F=_T137.hd;_T136=(int)_T6F;}_T70=(int)_T136;if(_T70!=108)goto _TL6B;{int x=_T136;
t=Cyc_Absyn_ulong_type;goto _LL46;}_TL6B: _T71=lenmod;{struct Cyc_List_List _T137=*_T71;_T72=_T137.hd;_T136=(int)_T72;}_T73=(int)_T136;if(_T73!=104)goto _TL6D;{int x=_T136;
t=Cyc_Absyn_ushort_type;goto _LL46;}_TL6D: goto _LL4F;_TL69: _T74=lenmod;_T75=(struct Cyc_List_List*)_T74;_T76=_T75->tl;_T77=(struct Cyc_List_List*)_T76;_T78=_T77->tl;if(_T78!=0)goto _TL6F;_T79=lenmod;{struct Cyc_List_List _T137=*_T79;_T7A=_T137.hd;_T136=(int)_T7A;_T7B=_T137.tl;{struct Cyc_List_List _T138=*_T7B;_T7C=_T138.hd;_T135=(int)_T7C;}}_T7D=(int)_T136;if(_T7D!=104)goto _TL71;_T7E=(int)_T135;if(_T7E!=104)goto _TL71;{int x=_T136;int y=_T135;
# 231
t=Cyc_Absyn_uchar_type;goto _LL46;}_TL71: goto _LL4F;_TL6F: _LL4F:{struct Cyc_String_pa_PrintArg_struct _T137;_T137.tag=0;
# 235
_T137.f1=Cyc_implode(lenmod);_T80=_T137;}{struct Cyc_String_pa_PrintArg_struct _T137=_T80;{struct Cyc_Int_pa_PrintArg_struct _T138;_T138.tag=1;_T82=c;_T83=(int)_T82;_T138.f1=(unsigned long)_T83;_T81=_T138;}{struct Cyc_Int_pa_PrintArg_struct _T138=_T81;void*_T139[2];_T139[0]=& _T137;_T139[1]=& _T138;{
# 234
struct Cyc_List_List*(*_T13A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T84=_T13A;}_T85=loc;_T86=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T87=_tag_fat(_T139,sizeof(void*),2);_T7F=_T84(_T85,_T86,_T87);}}{struct Cyc_List_List*_T137=_T7F;_npop_handler(0);return _T137;}_LL46:;}{struct Cyc_List_List*_T135=_cycalloc(sizeof(struct Cyc_List_List));
# 237
_T135->hd=t;_T135->tl=typs;_T88=(struct Cyc_List_List*)_T135;}typs=_T88;goto _LL12;case 102: goto _LL22;case 70: _LL22: goto _LL24;case 101: _LL24: goto _LL26;case 69: _LL26: goto _LL28;case 103: _LL28: goto _LL2A;case 71: _LL2A: goto _LL2C;case 97: _LL2C: goto _LL2E;case 65: _LL2E:{int _T135;if(lenmod!=0)goto _TL73;{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 255
_T136->hd=Cyc_Absyn_double_type;_T136->tl=typs;_T89=(struct Cyc_List_List*)_T136;}typs=_T89;goto _LL51;_TL73: _T8A=lenmod;_T8B=(struct Cyc_List_List*)_T8A;_T8C=_T8B->tl;if(_T8C!=0)goto _TL75;_T8D=lenmod;{struct Cyc_List_List _T136=*_T8D;_T8E=_T136.hd;_T135=(int)_T8E;}_T8F=(int)_T135;if(_T8F!=108)goto _TL77;{int x=_T135;{struct Cyc_List_List*_T136=_cycalloc(sizeof(struct Cyc_List_List));
# 257
_T136->hd=Cyc_Absyn_long_double_type;_T136->tl=typs;_T90=(struct Cyc_List_List*)_T136;}typs=_T90;goto _LL51;}_TL77: goto _LL56;_TL75: _LL56:{struct Cyc_String_pa_PrintArg_struct _T136;_T136.tag=0;
# 260
_T136.f1=Cyc_implode(lenmod);_T92=_T136;}{struct Cyc_String_pa_PrintArg_struct _T136=_T92;{struct Cyc_Int_pa_PrintArg_struct _T137;_T137.tag=1;_T94=c;_T95=(int)_T94;_T137.f1=(unsigned long)_T95;_T93=_T137;}{struct Cyc_Int_pa_PrintArg_struct _T137=_T93;void*_T138[2];_T138[0]=& _T136;_T138[1]=& _T137;{
# 259
struct Cyc_List_List*(*_T139)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T96=_T139;}_T97=loc;_T98=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T99=_tag_fat(_T138,sizeof(void*),2);_T91=_T96(_T97,_T98,_T99);}}{struct Cyc_List_List*_T136=_T91;_npop_handler(0);return _T136;}_LL51:;}goto _LL12;case 99:{
# 264
struct Cyc_List_List*f=flags;_TL7C: if(f!=0)goto _TL7A;else{goto _TL7B;}
_TL7A: _T9A=f;_T9B=_T9A->hd;_T9C=(int)_T9B;if(_T9C==35)goto _TL7F;else{goto _TL80;}_TL80: _T9D=f;_T9E=_T9D->hd;_T9F=(int)_T9E;if(_T9F==48)goto _TL7F;else{goto _TL7D;}
_TL7F:{struct Cyc_Int_pa_PrintArg_struct _T135;_T135.tag=1;_TA2=f;_TA3=_TA2->hd;_TA4=(int)_TA3;_T135.f1=(unsigned long)_TA4;_TA1=_T135;}{struct Cyc_Int_pa_PrintArg_struct _T135=_TA1;void*_T136[1];_T136[0]=& _T135;{struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TA5=_T137;}_TA6=loc;_TA7=_tag_fat("flag '%c' not allowed with %%c",sizeof(char),31U);_TA8=_tag_fat(_T136,sizeof(void*),1);_TA0=_TA5(_TA6,_TA7,_TA8);}{struct Cyc_List_List*_T135=_TA0;_npop_handler(0);return _T135;}_TL7D: _TA9=f;
# 264
f=_TA9->tl;goto _TL7C;_TL7B:;}
# 269
if(lenmod==0)goto _TL81;{struct Cyc_String_pa_PrintArg_struct _T135;_T135.tag=0;
# 271
_T135.f1=Cyc_implode(lenmod);_TAB=_T135;}{struct Cyc_String_pa_PrintArg_struct _T135=_TAB;void*_T136[1];_T136[0]=& _T135;{
# 270
struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TAC=_T137;}_TAD=loc;_TAE=_tag_fat("length modifier '%s' not allowed with %%c",sizeof(char),42U);_TAF=_tag_fat(_T136,sizeof(void*),1);_TAA=_TAC(_TAD,_TAE,_TAF);}{struct Cyc_List_List*_T135=_TAA;_npop_handler(0);return _T135;}_TL81:
# 272
 if(precision==0)goto _TL83;{struct Cyc_String_pa_PrintArg_struct _T135;_T135.tag=0;
# 274
_T135.f1=Cyc_implode(precision);_TB1=_T135;}{struct Cyc_String_pa_PrintArg_struct _T135=_TB1;void*_T136[1];_T136[0]=& _T135;{
# 273
struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TB2=_T137;}_TB3=loc;_TB4=_tag_fat("precision '%s' not allowed with %%c",sizeof(char),36U);_TB5=_tag_fat(_T136,sizeof(void*),1);_TB0=_TB2(_TB3,_TB4,_TB5);}{struct Cyc_List_List*_T135=_TB0;_npop_handler(0);return _T135;}_TL83:{struct Cyc_List_List*_T135=_cycalloc(sizeof(struct Cyc_List_List));
# 275
_T135->hd=Cyc_Absyn_sint_type;_T135->tl=typs;_TB6=(struct Cyc_List_List*)_T135;}typs=_TB6;goto _LL12;case 115:{
# 279
struct Cyc_List_List*f=flags;_TL88: if(f!=0)goto _TL86;else{goto _TL87;}
_TL86: _TB7=f;_TB8=_TB7->hd;_TB9=(int)_TB8;if(_TB9==45)goto _TL89;{
struct Cyc_List_List*(*_T135)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TBA=_T135;}_TBB=loc;_TBC=_tag_fat("a flag not allowed with %%s",sizeof(char),28U);_TBD=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T135=_TBA(_TBB,_TBC,_TBD);_npop_handler(0);return _T135;}_TL89: _TBE=f;
# 279
f=_TBE->tl;goto _TL88;_TL87:;}
# 284
if(lenmod==0)goto _TL8B;{
struct Cyc_List_List*(*_T135)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TBF=_T135;}_TC0=loc;_TC1=_tag_fat("length modifiers not allowed with %%s",sizeof(char),38U);_TC2=_tag_fat(0U,sizeof(void*),0);{struct Cyc_List_List*_T135=_TBF(_TC0,_TC1,_TC2);_npop_handler(0);return _T135;}_TL8B: {
# 288
void*ptr;
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);_TC3=isCproto;
if(_TC3)goto _TL8D;else{goto _TL8F;}
_TL8F: _TC4=Cyc_Absyn_char_type;_TC5=& Cyc_Kinds_eko;_TC6=(struct Cyc_Core_Opt*)_TC5;{struct Cyc_Core_Opt*_T135=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 293
_T135->v=tvs;_TC7=(struct Cyc_Core_Opt*)_T135;}_TC8=
# 292
Cyc_Absyn_new_evar(_TC6,_TC7);_TC9=Cyc_Absyn_al_qual_type;_TCA=
# 294
Cyc_Absyn_const_tqual(0U);_TCB=Cyc_Absyn_false_type;_TCC=Cyc_Tcutil_any_bool(tvs);
# 291
ptr=Cyc_Absyn_fatptr_type(_TC4,_TC8,_TC9,_TCA,_TCB,_TCC);goto _TL8E;
# 296
_TL8D: _TCD=Cyc_Absyn_char_type;_TCE=& Cyc_Kinds_eko;_TCF=(struct Cyc_Core_Opt*)_TCE;{struct Cyc_Core_Opt*_T135=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 298
_T135->v=tvs;_TD0=(struct Cyc_Core_Opt*)_T135;}_TD1=
# 297
Cyc_Absyn_new_evar(_TCF,_TD0);_TD2=Cyc_Absyn_al_qual_type;_TD3=
# 299
Cyc_Absyn_const_tqual(0U);_TD4=Cyc_Absyn_true_type;_TD5=Cyc_Tcutil_any_bool(tvs);
# 296
ptr=Cyc_Absyn_at_type(_TCD,_TD1,_TD2,_TD3,_TD4,_TD5);_TL8E:{struct Cyc_List_List*_T135=_cycalloc(sizeof(struct Cyc_List_List));
# 300
_T135->hd=ptr;_T135->tl=typs;_TD6=(struct Cyc_List_List*)_T135;}typs=_TD6;goto _LL12;}case 112:{struct Cyc_List_List*_T135=_cycalloc(sizeof(struct Cyc_List_List));
# 304
_T135->hd=Cyc_Absyn_uint_type;_T135->tl=typs;_TD7=(struct Cyc_List_List*)_T135;}typs=_TD7;goto _LL12;case 110:{
# 307
struct Cyc_List_List*f=flags;_TL93: if(f!=0)goto _TL91;else{goto _TL92;}
_TL91: _TD8=f;_TD9=_TD8->hd;_TDA=(int)_TD9;if(_TDA==35)goto _TL96;else{goto _TL97;}_TL97: _TDB=f;_TDC=_TDB->hd;_TDD=(int)_TDC;if(_TDD==48)goto _TL96;else{goto _TL94;}
_TL96:{struct Cyc_Int_pa_PrintArg_struct _T135;_T135.tag=1;_TE0=f;_TE1=_TE0->hd;_TE2=(int)_TE1;_T135.f1=(unsigned long)_TE2;_TDF=_T135;}{struct Cyc_Int_pa_PrintArg_struct _T135=_TDF;void*_T136[1];_T136[0]=& _T135;{struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TE3=_T137;}_TE4=loc;_TE5=_tag_fat("flag '%c' not allowed with %%n",sizeof(char),31U);_TE6=_tag_fat(_T136,sizeof(void*),1);_TDE=_TE3(_TE4,_TE5,_TE6);}{struct Cyc_List_List*_T135=_TDE;_npop_handler(0);return _T135;}_TL94: _TE7=f;
# 307
f=_TE7->tl;goto _TL93;_TL92:;}
# 310
if(precision==0)goto _TL98;{struct Cyc_String_pa_PrintArg_struct _T135;_T135.tag=0;
# 312
_T135.f1=Cyc_implode(precision);_TE9=_T135;}{struct Cyc_String_pa_PrintArg_struct _T135=_TE9;void*_T136[1];_T136[0]=& _T135;{
# 311
struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_TEA=_T137;}_TEB=loc;_TEC=_tag_fat("precision '%s' not allowed with %%n",sizeof(char),36U);_TED=_tag_fat(_T136,sizeof(void*),1);_TE8=_TEA(_TEB,_TEC,_TED);}{struct Cyc_List_List*_T135=_TE8;_npop_handler(0);return _T135;}_TL98: {
# 313
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);{int _T135;int _T136;if(lenmod!=0)goto _TL9A;
# 315
t=Cyc_Absyn_sint_type;goto _LL58;_TL9A: _TEE=lenmod;_TEF=(struct Cyc_List_List*)_TEE;_TF0=_TEF->tl;if(_TF0!=0)goto _TL9C;_TF1=lenmod;{struct Cyc_List_List _T137=*_TF1;_TF2=_T137.hd;_T136=(int)_TF2;}_TF3=(int)_T136;if(_TF3!=108)goto _TL9E;{int x=_T136;
# 317
t=Cyc_Absyn_ulong_type;goto _LL58;}_TL9E: _TF4=lenmod;{struct Cyc_List_List _T137=*_TF4;_TF5=_T137.hd;_T136=(int)_TF5;}_TF6=(int)_T136;if(_TF6!=104)goto _TLA0;{int x=_T136;
t=Cyc_Absyn_sshort_type;goto _LL58;}_TLA0: goto _LL61;_TL9C: _TF7=lenmod;_TF8=(struct Cyc_List_List*)_TF7;_TF9=_TF8->tl;_TFA=(struct Cyc_List_List*)_TF9;_TFB=_TFA->tl;if(_TFB!=0)goto _TLA2;_TFC=lenmod;{struct Cyc_List_List _T137=*_TFC;_TFD=_T137.hd;_T136=(int)_TFD;_TFE=_T137.tl;{struct Cyc_List_List _T138=*_TFE;_TFF=_T138.hd;_T135=(int)_TFF;}}_T100=(int)_T136;if(_T100!=104)goto _TLA4;_T101=(int)_T135;if(_T101!=104)goto _TLA4;{int x=_T136;int y=_T135;
# 320
t=Cyc_Absyn_schar_type;goto _LL58;}_TLA4: goto _LL61;_TLA2: _LL61:{struct Cyc_String_pa_PrintArg_struct _T137;_T137.tag=0;
# 323
_T137.f1=Cyc_implode(lenmod);_T103=_T137;}{struct Cyc_String_pa_PrintArg_struct _T137=_T103;{struct Cyc_Int_pa_PrintArg_struct _T138;_T138.tag=1;_T105=c;_T106=(int)_T105;_T138.f1=(unsigned long)_T106;_T104=_T138;}{struct Cyc_Int_pa_PrintArg_struct _T138=_T104;void*_T139[2];_T139[0]=& _T137;_T139[1]=& _T138;{
# 322
struct Cyc_List_List*(*_T13A)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T107=_T13A;}_T108=loc;_T109=_tag_fat("length modifier '%s' is not allowed with %%%c",sizeof(char),46U);_T10A=_tag_fat(_T139,sizeof(void*),2);_T102=_T107(_T108,_T109,_T10A);}}{struct Cyc_List_List*_T137=_T102;_npop_handler(0);return _T137;}_LL58:;}_T10B=t;_T10C=& Cyc_Kinds_eko;_T10D=(struct Cyc_Core_Opt*)_T10C;{struct Cyc_Core_Opt*_T135=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 325
_T135->v=tvs;_T10E=(struct Cyc_Core_Opt*)_T135;}_T10F=Cyc_Absyn_new_evar(_T10D,_T10E);_T110=Cyc_Absyn_al_qual_type;_T111=Cyc_Absyn_empty_tqual(0U);_T112=Cyc_Absyn_false_type;_T113=
Cyc_Tcutil_any_bool(tvs);
# 325
t=Cyc_Absyn_at_type(_T10B,_T10F,_T110,_T111,_T112,_T113);{struct Cyc_List_List*_T135=_cycalloc(sizeof(struct Cyc_List_List));
# 327
_T135->hd=t;_T135->tl=typs;_T114=(struct Cyc_List_List*)_T135;}typs=_T114;goto _LL12;}case 37:
# 330
 if(flags==0)goto _TLA6;{struct Cyc_String_pa_PrintArg_struct _T135;_T135.tag=0;
# 332
_T135.f1=Cyc_implode(flags);_T116=_T135;}{struct Cyc_String_pa_PrintArg_struct _T135=_T116;void*_T136[1];_T136[0]=& _T135;{
# 331
struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T117=_T137;}_T118=loc;_T119=_tag_fat("flags '%s' not allowed with %%%%",sizeof(char),33U);_T11A=_tag_fat(_T136,sizeof(void*),1);_T115=_T117(_T118,_T119,_T11A);}{struct Cyc_List_List*_T135=_T115;_npop_handler(0);return _T135;}_TLA6:
# 333
 if(width==0)goto _TLA8;{struct Cyc_String_pa_PrintArg_struct _T135;_T135.tag=0;
# 335
_T135.f1=Cyc_implode(width);_T11C=_T135;}{struct Cyc_String_pa_PrintArg_struct _T135=_T11C;void*_T136[1];_T136[0]=& _T135;{
# 334
struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T11D=_T137;}_T11E=loc;_T11F=_tag_fat("width '%s' not allowed with %%%%",sizeof(char),33U);_T120=_tag_fat(_T136,sizeof(void*),1);_T11B=_T11D(_T11E,_T11F,_T120);}{struct Cyc_List_List*_T135=_T11B;_npop_handler(0);return _T135;}_TLA8:
# 336
 if(precision==0)goto _TLAA;{struct Cyc_String_pa_PrintArg_struct _T135;_T135.tag=0;
# 338
_T135.f1=Cyc_implode(precision);_T122=_T135;}{struct Cyc_String_pa_PrintArg_struct _T135=_T122;void*_T136[1];_T136[0]=& _T135;{
# 337
struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T123=_T137;}_T124=loc;_T125=_tag_fat("precision '%s' not allowed with %%%%",sizeof(char),37U);_T126=_tag_fat(_T136,sizeof(void*),1);_T121=_T123(_T124,_T125,_T126);}{struct Cyc_List_List*_T135=_T121;_npop_handler(0);return _T135;}_TLAA:
# 339
 if(lenmod==0)goto _TLAC;{struct Cyc_String_pa_PrintArg_struct _T135;_T135.tag=0;
# 341
_T135.f1=Cyc_implode(lenmod);_T128=_T135;}{struct Cyc_String_pa_PrintArg_struct _T135=_T128;void*_T136[1];_T136[0]=& _T135;{
# 340
struct Cyc_List_List*(*_T137)(unsigned,struct _fat_ptr,struct _fat_ptr ap)=(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;_T129=_T137;}_T12A=loc;_T12B=_tag_fat("length modifier '%s' not allowed with %%%%",sizeof(char),43U);_T12C=_tag_fat(_T136,sizeof(void*),1);_T127=_T129(_T12A,_T12B,_T12C);}{struct Cyc_List_List*_T135=_T127;_npop_handler(0);return _T135;}_TLAC: goto _LL12;default:  {struct Cyc_List_List*_T135=0;_npop_handler(0);return _T135;}}_LL12:;}}}}_TL2F:
# 176
 i=i + 1;goto _TL32;_TL31:{struct Cyc_List_List*_T12E=
# 346
Cyc_List_imp_rev(typs);_npop_handler(0);return _T12E;}_pop_region();}struct _tuple12{int f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;char f3;int f4;};
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

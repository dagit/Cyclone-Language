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
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, unsigned orig_s) {  
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
  return _region_malloc(r,orig_s); 
}

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 290 "cycboot.h"
extern int isdigit(int);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 136 "string.h"
extern struct _fat_ptr Cyc_implode(struct Cyc_List_List*);struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 847 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 867
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 872
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uchar_type;extern void*Cyc_Absyn_ushort_type;extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;
# 874
extern void*Cyc_Absyn_schar_type;extern void*Cyc_Absyn_sshort_type;extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;
# 876
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 883
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 924
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 928
void*Cyc_Absyn_fatptr_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 33 "warn.h"
void Cyc_Warn_verr(unsigned,struct _fat_ptr,struct _fat_ptr);
# 238 "tcutil.h"
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 47 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_rko;struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 99 "tcenv.h"
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 31 "formatstr.cyc"
static void*Cyc_Formatstr_err_null(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 33
Cyc_Warn_verr(loc,fmt,ap);
return 0;}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;char f5;int f6;};
# 43
struct Cyc_Core_Opt*Cyc_Formatstr_parse_conversionspecification(struct _RegionHandle*r,struct _fat_ptr s,int i){
# 47
unsigned long len=Cyc_strlen(s);
if(i < 0 ||(unsigned long)i >= len)return 0;{
# 51
struct Cyc_List_List*flags=0;
char c=' ';
for(1;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
switch((int)c){case 43:
 goto _LL4;case 45: _LL4:
 goto _LL6;case 32: _LL6:
 goto _LL8;case 35: _LL8:
 goto _LLA;case 48: _LLA:
 flags=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=flags;_Tmp0;});continue;default:
 goto _LL0;}_LL0:;
# 63
break;}
# 65
if((unsigned long)i >= len)return 0;
flags=Cyc_List_imp_rev(flags);{
# 69
struct Cyc_List_List*width=0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c==42){
width=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=width;_Tmp0;});
++ i;}else{
# 75
for(1;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))width=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=width;_Tmp0;});else{
break;}}}
# 81
if((unsigned long)i >= len)return 0;
width=Cyc_List_imp_rev(width);{
# 85
struct Cyc_List_List*precision=0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c==46){
precision=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=precision;_Tmp0;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c==42){
precision=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=precision;_Tmp0;});
++ i;}else{
# 95
for(1;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))precision=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=precision;_Tmp0;});else{
break;}}}}
# 101
if((unsigned long)i >= len)return 0;
precision=Cyc_List_imp_rev(precision);{
# 106
struct Cyc_List_List*lenmod=0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
switch((int)c){case 104:
# 110
 lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c==104){lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});++ i;}
goto _LLD;case 108:
# 116
 lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});
if((unsigned long)++ i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c==108){lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});++ i;}
goto _LLD;case 106:
 goto _LL15;case 122: _LL15:
 goto _LL17;case 116: _LL17:
 goto _LL19;case 76: _LL19:
# 125
 lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});
++ i;
goto _LLD;default:
 goto _LLD;}_LLD:;
# 130
if((unsigned long)i >= len)return 0;
lenmod=Cyc_List_imp_rev(lenmod);
# 134
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
switch((int)c){case 100:
 goto _LL20;case 105: _LL20:
 goto _LL22;case 111: _LL22:
 goto _LL24;case 117: _LL24:
 goto _LL26;case 120: _LL26:
 goto _LL28;case 88: _LL28:
 goto _LL2A;case 102: _LL2A:
 goto _LL2C;case 70: _LL2C:
 goto _LL2E;case 101: _LL2E:
 goto _LL30;case 69: _LL30:
 goto _LL32;case 103: _LL32:
 goto _LL34;case 71: _LL34:
 goto _LL36;case 97: _LL36:
 goto _LL38;case 65: _LL38:
 goto _LL3A;case 99: _LL3A:
 goto _LL3C;case 115: _LL3C:
 goto _LL3E;case 112: _LL3E:
 goto _LL40;case 110: _LL40:
 goto _LL42;case 37: _LL42:
 goto _LL1C;default:
 return 0;}_LL1C:;
# 166 "formatstr.cyc"
return({struct Cyc_Core_Opt*_Tmp0=_region_malloc(r,sizeof(struct Cyc_Core_Opt));({struct _tuple11*_Tmp1=({struct _tuple11*_Tmp2=_region_malloc(r,sizeof(struct _tuple11));_Tmp2->f1=flags,_Tmp2->f2=width,_Tmp2->f3=precision,_Tmp2->f4=lenmod,_Tmp2->f5=c,_Tmp2->f6=i + 1;_Tmp2;});_Tmp0->v=_Tmp1;});_Tmp0;});}}}}}
# 169
struct Cyc_List_List*Cyc_Formatstr_get_format_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){
# 172
unsigned long len=Cyc_strlen(s);
struct Cyc_List_List*typs=0;
int i;
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
for(i=0;(unsigned long)i < len;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!=37)continue;{
struct Cyc_Core_Opt*cs=Cyc_Formatstr_parse_conversionspecification(temp,s,i + 1);
if(cs==0){
struct Cyc_List_List*_Tmp1=({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_Tmp2="bad format string";_tag_fat(_Tmp2,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));_npop_handler(0);return _Tmp1;}{
struct _tuple11*_Tmp1=(struct _tuple11*)cs->v;int _Tmp2;char _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f1;_Tmp6=_Tmp1->f2;_Tmp5=_Tmp1->f3;_Tmp4=_Tmp1->f4;_Tmp3=_Tmp1->f5;_Tmp2=_Tmp1->f6;{struct Cyc_List_List*flags=_Tmp7;struct Cyc_List_List*width=_Tmp6;struct Cyc_List_List*precision=_Tmp5;struct Cyc_List_List*lenmod=_Tmp4;char c=_Tmp3;int j=_Tmp2;
i=j - 1;
{int _Tmp8;if(lenmod!=0){if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp8=(int)lenmod->hd;if(
((int)_Tmp8==106 ||(int)_Tmp8==122)||(int)_Tmp8==116){int x=_Tmp8;
# 187
struct Cyc_List_List*_Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA=({struct Cyc_Int_pa_PrintArg_struct _TmpB;_TmpB.tag=1,_TmpB.f1=(unsigned long)x;_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpC="length modifier '%c' is not supported";_tag_fat(_TmpC,sizeof(char),38U);}),_tag_fat(_TmpB,sizeof(void*),1));});_npop_handler(0);return _Tmp9;}else{goto _LL6;}}else{goto _LL6;}}else{_LL6:
 goto _LL3;}_LL3:;}
# 190
{int _Tmp8;if(width!=0){if(((struct Cyc_List_List*)width)->tl==0){_Tmp8=(int)width->hd;if((int)_Tmp8==42){int x=_Tmp8;
typs=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=Cyc_Absyn_sint_type,_Tmp9->tl=typs;_Tmp9;});goto _LL8;}else{goto _LLB;}}else{goto _LLB;}}else{_LLB:
 goto _LL8;}_LL8:;}
# 194
{int _Tmp8;int _Tmp9;if(precision!=0){if(((struct Cyc_List_List*)precision)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)precision)->tl)->tl==0){_Tmp9=(int)precision->hd;_Tmp8=(int)precision->tl->hd;if(
(int)_Tmp9==46 &&(int)_Tmp8==42){int x=_Tmp9;int y=_Tmp8;
typs=({struct Cyc_List_List*_TmpA=_cycalloc(sizeof(struct Cyc_List_List));_TmpA->hd=Cyc_Absyn_sint_type,_TmpA->tl=typs;_TmpA;});goto _LLD;}else{goto _LL10;}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10:
 goto _LLD;}_LLD:;}{
# 199
void*t;
switch((int)c){case 100:
 goto _LL16;case 105: _LL16:
# 203
{struct Cyc_List_List*f=flags;for(0;f!=0;f=f->tl){
if((int)f->hd==35){
struct Cyc_List_List*_Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned long)((int)c);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="flag '#' is not valid with %%%c";_tag_fat(_TmpB,sizeof(char),32U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}}}
{int _Tmp8;int _Tmp9;if(lenmod==0){
t=Cyc_Absyn_sint_type;goto _LL3B;}else{if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp9=(int)lenmod->hd;if((int)_Tmp9==108){int x=_Tmp9;
t=Cyc_Absyn_slong_type;goto _LL3B;}else{_Tmp9=(int)lenmod->hd;if((int)_Tmp9==104){int x=_Tmp9;
t=Cyc_Absyn_sshort_type;goto _LL3B;}else{goto _LL44;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)lenmod)->tl)->tl==0){_Tmp9=(int)lenmod->hd;_Tmp8=(int)lenmod->tl->hd;if(
(int)_Tmp9==104 &&(int)_Tmp8==104){int x=_Tmp9;int y=_Tmp8;
t=Cyc_Absyn_schar_type;goto _LL3B;}else{goto _LL44;}}else{_LL44: {
# 213
struct Cyc_List_List*_TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({
struct _fat_ptr _TmpD=Cyc_implode(lenmod);_TmpC.f1=_TmpD;});_TmpC;});struct Cyc_Int_pa_PrintArg_struct _TmpC=({struct Cyc_Int_pa_PrintArg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=(unsigned long)((int)c);_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;({
# 213
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpE="length modifier '%s' is not allowed with %%%c";_tag_fat(_TmpE,sizeof(char),46U);}),_tag_fat(_TmpD,sizeof(void*),2));});_npop_handler(0);return _TmpA;}}}}_LL3B:;}
# 216
typs=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=t,_Tmp8->tl=typs;_Tmp8;});
goto _LL12;case 117:
# 219
{struct Cyc_List_List*f=flags;for(0;f!=0;f=f->tl){
if((int)f->hd==35){
struct Cyc_List_List*_Tmp8=({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_Tmp9="Flag '#' not valid with %%u";_tag_fat(_Tmp9,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));_npop_handler(0);return _Tmp8;}}}
goto _LL1A;case 111: _LL1A:
 goto _LL1C;case 120: _LL1C:
 goto _LL1E;case 88: _LL1E:
# 226
{int _Tmp8;int _Tmp9;if(lenmod==0){
t=Cyc_Absyn_uint_type;goto _LL46;}else{if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp9=(int)lenmod->hd;if((int)_Tmp9==108){int x=_Tmp9;
t=Cyc_Absyn_ulong_type;goto _LL46;}else{_Tmp9=(int)lenmod->hd;if((int)_Tmp9==104){int x=_Tmp9;
t=Cyc_Absyn_ushort_type;goto _LL46;}else{goto _LL4F;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)lenmod)->tl)->tl==0){_Tmp9=(int)lenmod->hd;_Tmp8=(int)lenmod->tl->hd;if(
(int)_Tmp9==104 &&(int)_Tmp8==104){int x=_Tmp9;int y=_Tmp8;
t=Cyc_Absyn_uchar_type;goto _LL46;}else{goto _LL4F;}}else{_LL4F: {
# 234
struct Cyc_List_List*_TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({
struct _fat_ptr _TmpD=Cyc_implode(lenmod);_TmpC.f1=_TmpD;});_TmpC;});struct Cyc_Int_pa_PrintArg_struct _TmpC=({struct Cyc_Int_pa_PrintArg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=(unsigned long)((int)c);_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;({
# 234
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpE="length modifier '%s' is not allowed with %%%c";_tag_fat(_TmpE,sizeof(char),46U);}),_tag_fat(_TmpD,sizeof(void*),2));});_npop_handler(0);return _TmpA;}}}}_LL46:;}
# 237
typs=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=t,_Tmp8->tl=typs;_Tmp8;});
goto _LL12;case 102:
 goto _LL22;case 70: _LL22:
 goto _LL24;case 101: _LL24:
 goto _LL26;case 69: _LL26:
 goto _LL28;case 103: _LL28:
 goto _LL2A;case 71: _LL2A:
 goto _LL2C;case 97: _LL2C:
 goto _LL2E;case 65: _LL2E:
# 253
{int _Tmp8;if(lenmod==0){
# 255
typs=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=Cyc_Absyn_double_type,_Tmp9->tl=typs;_Tmp9;});goto _LL51;}else{if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp8=(int)lenmod->hd;if((int)_Tmp8==108){int x=_Tmp8;
# 257
typs=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=Cyc_Absyn_long_double_type,_Tmp9->tl=typs;_Tmp9;});goto _LL51;}else{goto _LL56;}}else{_LL56: {
# 259
struct Cyc_List_List*_Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_implode(lenmod);_TmpB.f1=_TmpC;});_TmpB;});struct Cyc_Int_pa_PrintArg_struct _TmpB=({struct Cyc_Int_pa_PrintArg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=(unsigned long)((int)c);_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;({
# 259
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpD="length modifier '%s' is not allowed with %%%c";_tag_fat(_TmpD,sizeof(char),46U);}),_tag_fat(_TmpC,sizeof(void*),2));});_npop_handler(0);return _Tmp9;}}}_LL51:;}
# 262
goto _LL12;case 99:
# 264
{struct Cyc_List_List*f=flags;for(0;f!=0;f=f->tl){
if((int)f->hd==35 ||(int)f->hd==48){
struct Cyc_List_List*_Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned long)((int)f->hd);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="flag '%c' not allowed with %%c";_tag_fat(_TmpB,sizeof(char),31U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}}}
# 269
if(lenmod!=0){
struct Cyc_List_List*_Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=Cyc_implode(lenmod);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({
# 270
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="length modifier '%s' not allowed with %%c";_tag_fat(_TmpB,sizeof(char),42U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}
# 272
if(precision!=0){
struct Cyc_List_List*_Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=Cyc_implode(precision);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({
# 273
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="precision '%s' not allowed with %%c";_tag_fat(_TmpB,sizeof(char),36U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}
# 275
typs=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=Cyc_Absyn_sint_type,_Tmp8->tl=typs;_Tmp8;});
goto _LL12;case 115:
# 279
{struct Cyc_List_List*f=flags;for(0;f!=0;f=f->tl){
if((int)f->hd!=45){
struct Cyc_List_List*_Tmp8=({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_Tmp9="a flag not allowed with %%s";_tag_fat(_Tmp9,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));_npop_handler(0);return _Tmp8;}}}
# 284
if(lenmod!=0){
struct Cyc_List_List*_Tmp8=({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_Tmp9="length modifiers not allowed with %%s";_tag_fat(_Tmp9,sizeof(char),38U);}),_tag_fat(0U,sizeof(void*),0));_npop_handler(0);return _Tmp8;}{
# 288
void*ptr;
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
if(!isCproto)
ptr=({void*_Tmp8=Cyc_Absyn_char_type;void*_Tmp9=
Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_TmpA=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpA->v=tvs;_TmpA;}));
# 291
struct Cyc_Absyn_Tqual _TmpA=
# 294
Cyc_Absyn_const_tqual(0U);
# 291
void*_TmpB=Cyc_Absyn_false_type;Cyc_Absyn_fatptr_type(_Tmp8,_Tmp9,_TmpA,_TmpB,
# 294
Cyc_Tcutil_any_bool(tvs));});else{
# 296
ptr=({void*_Tmp8=Cyc_Absyn_char_type;void*_Tmp9=
Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_TmpA=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpA->v=tvs;_TmpA;}));
# 296
struct Cyc_Absyn_Tqual _TmpA=
# 299
Cyc_Absyn_const_tqual(0U);
# 296
void*_TmpB=Cyc_Absyn_true_type;Cyc_Absyn_at_type(_Tmp8,_Tmp9,_TmpA,_TmpB,
# 299
Cyc_Tcutil_any_bool(tvs));});}
typs=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=ptr,_Tmp8->tl=typs;_Tmp8;});
goto _LL12;}case 112:
# 304
 typs=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=Cyc_Absyn_uint_type,_Tmp8->tl=typs;_Tmp8;});
goto _LL12;case 110:
# 307
{struct Cyc_List_List*f=flags;for(0;f!=0;f=f->tl){
if((int)f->hd==35 ||(int)f->hd==48){
struct Cyc_List_List*_Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned long)((int)f->hd);_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="flag '%c' not allowed with %%n";_tag_fat(_TmpB,sizeof(char),31U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}}}
if(precision!=0){
struct Cyc_List_List*_Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=Cyc_implode(precision);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({
# 311
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="precision '%s' not allowed with %%n";_tag_fat(_TmpB,sizeof(char),36U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}{
# 313
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
{int _Tmp8;int _Tmp9;if(lenmod==0){
t=Cyc_Absyn_sint_type;goto _LL58;}else{if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp9=(int)lenmod->hd;if((int)_Tmp9==108){int x=_Tmp9;
# 317
t=Cyc_Absyn_ulong_type;goto _LL58;}else{_Tmp9=(int)lenmod->hd;if((int)_Tmp9==104){int x=_Tmp9;
t=Cyc_Absyn_sshort_type;goto _LL58;}else{goto _LL61;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)lenmod)->tl)->tl==0){_Tmp9=(int)lenmod->hd;_Tmp8=(int)lenmod->tl->hd;if(
(int)_Tmp9==104 &&(int)_Tmp8==104){int x=_Tmp9;int y=_Tmp8;
t=Cyc_Absyn_schar_type;goto _LL58;}else{goto _LL61;}}else{_LL61: {
# 322
struct Cyc_List_List*_TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({
struct _fat_ptr _TmpD=Cyc_implode(lenmod);_TmpC.f1=_TmpD;});_TmpC;});struct Cyc_Int_pa_PrintArg_struct _TmpC=({struct Cyc_Int_pa_PrintArg_struct _TmpD;_TmpD.tag=1,_TmpD.f1=(unsigned long)((int)c);_TmpD;});void*_TmpD[2];_TmpD[0]=& _TmpB,_TmpD[1]=& _TmpC;({
# 322
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpE="length modifier '%s' is not allowed with %%%c";_tag_fat(_TmpE,sizeof(char),46U);}),_tag_fat(_TmpD,sizeof(void*),2));});_npop_handler(0);return _TmpA;}}}}_LL58:;}
# 325
t=({void*_Tmp8=t;void*_Tmp9=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_TmpA=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpA->v=tvs;_TmpA;}));struct Cyc_Absyn_Tqual _TmpA=Cyc_Absyn_empty_tqual(0U);void*_TmpB=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp8,_Tmp9,_TmpA,_TmpB,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=t,_Tmp8->tl=typs;_Tmp8;});
goto _LL12;}case 37:
# 330
 if(flags!=0){
struct Cyc_List_List*_Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=Cyc_implode(flags);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({
# 331
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="flags '%s' not allowed with %%%%";_tag_fat(_TmpB,sizeof(char),33U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}
# 333
if(width!=0){
struct Cyc_List_List*_Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=Cyc_implode(width);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({
# 334
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="width '%s' not allowed with %%%%";_tag_fat(_TmpB,sizeof(char),33U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}
# 336
if(precision!=0){
struct Cyc_List_List*_Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=Cyc_implode(precision);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({
# 337
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="precision '%s' not allowed with %%%%";_tag_fat(_TmpB,sizeof(char),37U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}
# 339
if(lenmod!=0){
struct Cyc_List_List*_Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=Cyc_implode(lenmod);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({
# 340
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="length modifier '%s' not allowed with %%%%";_tag_fat(_TmpB,sizeof(char),43U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}
# 342
goto _LL12;default:  {
struct Cyc_List_List*_Tmp8=0;_npop_handler(0);return _Tmp8;}}_LL12:;}}}}}{
# 346
struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(typs);_npop_handler(0);return _Tmp1;}
# 176
;_pop_region();}struct _tuple12{int f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;char f4;int f5;};
# 357 "formatstr.cyc"
struct Cyc_Core_Opt*Cyc_Formatstr_parse_inputformat(struct _RegionHandle*r,struct _fat_ptr s,int i){
# 359
unsigned long len=Cyc_strlen(s);
if(i < 0 ||(unsigned long)i >= len)return 0;{
# 362
int suppress=0;
char c=((const char*)s.curr)[i];
if((int)c==42){
suppress=1;
++ i;
if((unsigned long)i >= len)return 0;}{
# 370
struct Cyc_List_List*width=0;
for(1;(unsigned long)i < len;++ i){
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if(isdigit((int)c))width=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=width;_Tmp0;});else{
break;}}
# 376
if((unsigned long)i >= len)return 0;
width=Cyc_List_imp_rev(width);{
# 381
struct Cyc_List_List*lenmod=0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
switch((int)c){case 104:
# 385
 lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});
++ i;
if((unsigned long)i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c==104){lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});++ i;}
goto _LL0;case 108:
# 392
 lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});
++ i;
if((unsigned long)i >= len)return 0;
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c==108){lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});++ i;}
goto _LL0;case 106:
 goto _LL8;case 122: _LL8:
 goto _LLA;case 116: _LLA:
 goto _LLC;case 76: _LLC:
# 402
 lenmod=({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)((int)c),_Tmp0->tl=lenmod;_Tmp0;});
++ i;
goto _LL0;default:
 goto _LL0;}_LL0:;
# 407
if((unsigned long)i >= len)return 0;
lenmod=Cyc_List_imp_rev(lenmod);
# 411
c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
switch((int)c){case 100:
 goto _LL13;case 105: _LL13:
 goto _LL15;case 111: _LL15:
 goto _LL17;case 117: _LL17:
 goto _LL19;case 120: _LL19:
 goto _LL1B;case 88: _LL1B:
 goto _LL1D;case 102: _LL1D:
 goto _LL1F;case 70: _LL1F:
 goto _LL21;case 101: _LL21:
 goto _LL23;case 69: _LL23:
 goto _LL25;case 103: _LL25:
 goto _LL27;case 71: _LL27:
 goto _LL29;case 97: _LL29:
 goto _LL2B;case 65: _LL2B:
 goto _LL2D;case 99: _LL2D:
 goto _LL2F;case 115: _LL2F:
 goto _LL31;case 112: _LL31:
 goto _LL33;case 110: _LL33:
 goto _LL35;case 37: _LL35:
 goto _LLF;default:
 return 0;}_LLF:;
# 434
return({struct Cyc_Core_Opt*_Tmp0=_region_malloc(r,sizeof(struct Cyc_Core_Opt));({struct _tuple12*_Tmp1=({struct _tuple12*_Tmp2=_region_malloc(r,sizeof(struct _tuple12));_Tmp2->f1=suppress,_Tmp2->f2=width,_Tmp2->f3=lenmod,_Tmp2->f4=c,_Tmp2->f5=i + 1;_Tmp2;});_Tmp0->v=_Tmp1;});_Tmp0;});}}}}
# 436
struct Cyc_List_List*Cyc_Formatstr_get_scanf_types(struct Cyc_Tcenv_Tenv*te,struct _fat_ptr s,int isCproto,unsigned loc){
# 439
unsigned long len=Cyc_strlen(s);
struct Cyc_List_List*typs=0;
int i;
{struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);
for(i=0;(unsigned long)i < len;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!=37)continue;{
struct Cyc_Core_Opt*x=Cyc_Formatstr_parse_inputformat(temp,s,i + 1);
if(x==0){
struct Cyc_List_List*_Tmp1=({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_Tmp2="bad format string";_tag_fat(_Tmp2,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));_npop_handler(0);return _Tmp1;}{
struct _tuple12*_Tmp1=(struct _tuple12*)x->v;int _Tmp2;char _Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{int suppress=_Tmp6;struct Cyc_List_List*width=_Tmp5;struct Cyc_List_List*lenmod=_Tmp4;char c=_Tmp3;int j=_Tmp2;
i=j - 1;
{int _Tmp7;if(lenmod!=0){if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp7=(int)lenmod->hd;if(
((int)_Tmp7==106 ||(int)_Tmp7==122)||(int)_Tmp7==116){int x=_Tmp7;
# 453
struct Cyc_List_List*_Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9=({struct Cyc_Int_pa_PrintArg_struct _TmpA;_TmpA.tag=1,_TmpA.f1=(unsigned long)x;_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpB="length modifier '%c' is not supported";_tag_fat(_TmpB,sizeof(char),38U);}),_tag_fat(_TmpA,sizeof(void*),1));});_npop_handler(0);return _Tmp8;}else{goto _LL6;}}else{goto _LL6;}}else{_LL6:
 goto _LL3;}_LL3:;}
# 456
if(suppress)continue;{
void*t;
switch((int)c){case 100:
 goto _LLC;case 105: _LLC: {
# 461
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
{int _Tmp7;int _Tmp8;if(lenmod==0){
t=Cyc_Absyn_sint_type;goto _LL33;}else{if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp8=(int)lenmod->hd;if((int)_Tmp8==108){int x=_Tmp8;
t=Cyc_Absyn_slong_type;goto _LL33;}else{_Tmp8=(int)lenmod->hd;if((int)_Tmp8==104){int x=_Tmp8;
t=Cyc_Absyn_sshort_type;goto _LL33;}else{goto _LL3C;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)lenmod)->tl)->tl==0){_Tmp8=(int)lenmod->hd;_Tmp7=(int)lenmod->tl->hd;if(
(int)_Tmp8==104 &&(int)_Tmp7==104){int x=_Tmp8;int y=_Tmp7;t=Cyc_Absyn_schar_type;goto _LL33;}else{goto _LL3C;}}else{_LL3C: {
# 468
struct Cyc_List_List*_Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_implode(lenmod);_TmpB.f1=_TmpC;});_TmpB;});struct Cyc_Int_pa_PrintArg_struct _TmpB=({struct Cyc_Int_pa_PrintArg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=(unsigned long)((int)c);_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;({
# 468
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpD="length modifier '%s' is not allowed with %%%c";_tag_fat(_TmpD,sizeof(char),46U);}),_tag_fat(_TmpC,sizeof(void*),2));});_npop_handler(0);return _Tmp9;}}}}_LL33:;}
# 471
t=({void*_Tmp7=t;void*_Tmp8=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=tvs;_Tmp9;}));struct Cyc_Absyn_Tqual _Tmp9=Cyc_Absyn_empty_tqual(0U);void*_TmpA=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=t,_Tmp7->tl=typs;_Tmp7;});
goto _LL8;}case 117:
 goto _LL10;case 111: _LL10:
 goto _LL12;case 120: _LL12:
 goto _LL14;case 88: _LL14: {
# 479
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
{int _Tmp7;int _Tmp8;if(lenmod==0){
t=Cyc_Absyn_uint_type;goto _LL3E;}else{if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp8=(int)lenmod->hd;if((int)_Tmp8==108){int x=_Tmp8;
t=Cyc_Absyn_ulong_type;goto _LL3E;}else{_Tmp8=(int)lenmod->hd;if((int)_Tmp8==104){int x=_Tmp8;
t=Cyc_Absyn_ushort_type;goto _LL3E;}else{goto _LL47;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)lenmod)->tl)->tl==0){_Tmp8=(int)lenmod->hd;_Tmp7=(int)lenmod->tl->hd;if(
(int)_Tmp8==104 &&(int)_Tmp7==104){int x=_Tmp8;int y=_Tmp7;t=Cyc_Absyn_uchar_type;goto _LL3E;}else{goto _LL47;}}else{_LL47: {
# 486
struct Cyc_List_List*_Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_implode(lenmod);_TmpB.f1=_TmpC;});_TmpB;});struct Cyc_Int_pa_PrintArg_struct _TmpB=({struct Cyc_Int_pa_PrintArg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=(unsigned long)((int)c);_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;({
# 486
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpD="length modifier '%s' is not allowed with %%%c";_tag_fat(_TmpD,sizeof(char),46U);}),_tag_fat(_TmpC,sizeof(void*),2));});_npop_handler(0);return _Tmp9;}}}}_LL3E:;}
# 489
t=({void*_Tmp7=t;void*_Tmp8=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=tvs;_Tmp9;}));struct Cyc_Absyn_Tqual _Tmp9=Cyc_Absyn_empty_tqual(0U);void*_TmpA=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=t,_Tmp7->tl=typs;_Tmp7;});
goto _LL8;}case 102:
 goto _LL18;case 70: _LL18:
 goto _LL1A;case 101: _LL1A:
 goto _LL1C;case 69: _LL1C:
 goto _LL1E;case 103: _LL1E:
 goto _LL20;case 71: _LL20:
 goto _LL22;case 97: _LL22:
 goto _LL24;case 65: _LL24: {
# 501
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
{int _Tmp7;if(lenmod==0){
t=Cyc_Absyn_float_type;goto _LL49;}else{if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp7=(int)lenmod->hd;if((int)_Tmp7==108){int x=_Tmp7;
# 505
t=Cyc_Absyn_double_type;goto _LL49;}else{goto _LL4E;}}else{_LL4E: {
# 507
struct Cyc_List_List*_Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({
struct _fat_ptr _TmpB=Cyc_implode(lenmod);_TmpA.f1=_TmpB;});_TmpA;});struct Cyc_Int_pa_PrintArg_struct _TmpA=({struct Cyc_Int_pa_PrintArg_struct _TmpB;_TmpB.tag=1,_TmpB.f1=(unsigned long)((int)c);_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;({
# 507
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpC="length modifier '%s' is not allowed with %%%c";_tag_fat(_TmpC,sizeof(char),46U);}),_tag_fat(_TmpB,sizeof(void*),2));});_npop_handler(0);return _Tmp8;}}}_LL49:;}
# 510
t=({void*_Tmp7=t;void*_Tmp8=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=tvs;_Tmp9;}));struct Cyc_Absyn_Tqual _Tmp9=Cyc_Absyn_empty_tqual(0U);void*_TmpA=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=t,_Tmp7->tl=typs;_Tmp7;});
goto _LL8;}case 99:  {
# 516
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
void*ptr;
if(!isCproto)
ptr=({void*_Tmp7=Cyc_Absyn_char_type;void*_Tmp8=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=tvs;_Tmp9;}));struct Cyc_Absyn_Tqual _Tmp9=
Cyc_Absyn_empty_tqual(0U);
# 519
void*_TmpA=Cyc_Absyn_false_type;Cyc_Absyn_fatptr_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,
# 521
Cyc_Tcutil_any_bool(tvs));});else{
# 523
ptr=({void*_Tmp7=Cyc_Absyn_char_type;void*_Tmp8=
Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=tvs;_Tmp9;}));
# 523
struct Cyc_Absyn_Tqual _Tmp9=
# 525
Cyc_Absyn_empty_tqual(0U);
# 523
void*_TmpA=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,
# 525
Cyc_Tcutil_any_bool(tvs));});}
typs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=ptr,_Tmp7->tl=typs;_Tmp7;});
goto _LL8;}case 115:  {
# 529
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
# 531
void*ptr;
if(!isCproto)
ptr=({void*_Tmp7=Cyc_Absyn_char_type;void*_Tmp8=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=tvs;_Tmp9;}));struct Cyc_Absyn_Tqual _Tmp9=
Cyc_Absyn_empty_tqual(0U);
# 533
void*_TmpA=Cyc_Absyn_false_type;Cyc_Absyn_fatptr_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,
Cyc_Tcutil_any_bool(tvs));});else{
# 536
ptr=({void*_Tmp7=Cyc_Absyn_char_type;void*_Tmp8=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=tvs;_Tmp9;}));struct Cyc_Absyn_Tqual _Tmp9=
Cyc_Absyn_empty_tqual(0U);
# 536
void*_TmpA=Cyc_Absyn_true_type;Cyc_Absyn_at_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,
Cyc_Tcutil_any_bool(tvs));});}
typs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=ptr,_Tmp7->tl=typs;_Tmp7;});
goto _LL8;}case 91:
 goto _LL2C;case 112: _LL2C: {
# 542
struct Cyc_List_List*_Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)((int)c);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpA="%%%c is not supported";_tag_fat(_TmpA,sizeof(char),22U);}),_tag_fat(_Tmp9,sizeof(void*),1));});_npop_handler(0);return _Tmp7;}case 110:  {
# 544
struct Cyc_List_List*tvs=Cyc_Tcenv_lookup_type_vars(te);
{int _Tmp7;int _Tmp8;if(lenmod==0){
t=Cyc_Absyn_sint_type;goto _LL50;}else{if(((struct Cyc_List_List*)lenmod)->tl==0){_Tmp8=(int)lenmod->hd;if((int)_Tmp8==108){int x=_Tmp8;
t=Cyc_Absyn_ulong_type;goto _LL50;}else{_Tmp8=(int)lenmod->hd;if((int)_Tmp8==104){int x=_Tmp8;
t=Cyc_Absyn_sshort_type;goto _LL50;}else{goto _LL59;}}}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)lenmod)->tl)->tl==0){_Tmp8=(int)lenmod->hd;_Tmp7=(int)lenmod->tl->hd;if(
(int)_Tmp8==104 &&(int)_Tmp7==104){int x=_Tmp8;int y=_Tmp7;t=Cyc_Absyn_schar_type;goto _LL50;}else{goto _LL59;}}else{_LL59: {
# 551
struct Cyc_List_List*_Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,({
struct _fat_ptr _TmpC=Cyc_implode(lenmod);_TmpB.f1=_TmpC;});_TmpB;});struct Cyc_Int_pa_PrintArg_struct _TmpB=({struct Cyc_Int_pa_PrintArg_struct _TmpC;_TmpC.tag=1,_TmpC.f1=(unsigned long)((int)c);_TmpC;});void*_TmpC[2];_TmpC[0]=& _TmpA,_TmpC[1]=& _TmpB;({
# 551
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpD="length modifier '%s' is not allowed with %%%c";_tag_fat(_TmpD,sizeof(char),46U);}),_tag_fat(_TmpC,sizeof(void*),2));});_npop_handler(0);return _Tmp9;}}}}_LL50:;}
# 554
t=({void*_Tmp7=t;void*_Tmp8=Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=tvs;_Tmp9;}));struct Cyc_Absyn_Tqual _Tmp9=Cyc_Absyn_empty_tqual(0U);void*_TmpA=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_Tmp7,_Tmp8,_Tmp9,_TmpA,
Cyc_Tcutil_any_bool(tvs));});
typs=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=t,_Tmp7->tl=typs;_Tmp7;});
goto _LL8;}case 37:
# 559
 if(suppress){
struct Cyc_List_List*_Tmp7=({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_Tmp8="Assignment suppression (*) is not allowed with %%%%";_tag_fat(_Tmp8,sizeof(char),52U);}),_tag_fat(0U,sizeof(void*),0));_npop_handler(0);return _Tmp7;}
if(width!=0){
struct Cyc_List_List*_Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_implode(width);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpA="width '%s' not allowed with %%%%";_tag_fat(_TmpA,sizeof(char),33U);}),_tag_fat(_Tmp9,sizeof(void*),1));});_npop_handler(0);return _Tmp7;}
if(lenmod!=0){
struct Cyc_List_List*_Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=Cyc_implode(lenmod);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({
# 564
(struct Cyc_List_List*(*)(unsigned,struct _fat_ptr,struct _fat_ptr ap))Cyc_Formatstr_err_null;})(loc,({const char*_TmpA="length modifier '%s' not allowed with %%%%";_tag_fat(_TmpA,sizeof(char),43U);}),_tag_fat(_Tmp9,sizeof(void*),1));});_npop_handler(0);return _Tmp7;}
# 566
goto _LL8;default:  {
struct Cyc_List_List*_Tmp7=0;_npop_handler(0);return _Tmp7;}}_LL8:;}}}}}
# 443
;_pop_region();}
# 571
return Cyc_List_imp_rev(typs);}

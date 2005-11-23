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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 173
extern struct _RegionHandle*Cyc_Core_heap_region;
# 180
extern unsigned Cyc_Core_unique_qual;
# 193 "core.h"
void Cyc_Core_ufree(void*);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 269 "core.h"
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_ukey(unsigned,const char*,const char*,int);
# 299 "core.h"
void Cyc_Core_free_ukey(struct Cyc_Core_DynamicRegion*);
# 309
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*,void*,void*(*)(struct _RegionHandle*,void*));struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 197 "cycboot.h"
extern int Cyc_sscanf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 82 "lexing.h"
extern struct _fat_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
extern char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 178 "list.h"
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 65
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 129
extern void Cyc_Set_iter(void(*)(void*),struct Cyc_Set_Set*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 51
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 54
extern int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 73
extern struct _fat_ptr Cyc_zstrncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 110 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 42 "xarray.h"
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 51
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 66
extern void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
extern int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};
# 902 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 904
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Complex_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Parse_Declarator f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
# 55 "parse.h"
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U,Cyc_Parse_None_sc =7U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned f0;void*f1;void*f2;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f0;int f1;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple17{int f0;enum Cyc_Absyn_AggrKind f1;};struct _union_YYSTYPE_YY23{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _union_YYSTYPE_YY29{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f0;struct Cyc_Parse_Type_specifier f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY36{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f0;int f1;struct Cyc_Absyn_VarargInfo*f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _tuple20{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _union_YYSTYPE_YY51{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _tuple21{void*f0;void*f1;};struct _union_YYSTYPE_YY53{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _tuple22{struct Cyc_List_List*f0;unsigned f1;};struct _union_YYSTYPE_YY56{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _union_YYSTYPE_YY59{int tag;void*val;};struct _union_YYSTYPE_YY60{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_Absyn_Exp*val;};struct _tuple23{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY62{int tag;struct _tuple23 val;};struct _union_YYSTYPE_YY63{int tag;void*val;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY64{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY65{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY67{int tag;struct Cyc_List_List*val;};struct _tuple25{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};struct _union_YYSTYPE_YY68{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY69{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YY67 YY67;struct _union_YYSTYPE_YY68 YY68;struct _union_YYSTYPE_YY69 YY69;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 95 "parse_tab.h"
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 110 "dict.h"
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 45 "binding.h"
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,void*,void*(*)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned,struct Cyc_Binding_NSCtxt*,struct _tuple0*);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 136 "flags.h"
extern int Cyc_Flags_compile_for_boot;struct Cyc_Lex_Trie;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple26{int f0;struct Cyc_Lex_Trie*f1;};struct _union_TrieChildren_One{int tag;struct _tuple26 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 50 "lex.cyl"
union Cyc_Lex_TrieChildren Cyc_Lex_Zero (void){union Cyc_Lex_TrieChildren _Tmp0;_Tmp0.Zero.tag=1U,_Tmp0.Zero.val=0;return _Tmp0;}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
union Cyc_Lex_TrieChildren _Tmp0;_Tmp0.One.tag=2U,_Tmp0.One.val.f0=i,_Tmp0.One.val.f1=t;return _Tmp0;}
# 54
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
union Cyc_Lex_TrieChildren _Tmp0;_Tmp0.Many.tag=3U,_Tmp0.Many.val=ts;return _Tmp0;}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};struct Cyc_Lex_DynTrie{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;};struct Cyc_Lex_DynTrieSymbols{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
# 76
static int Cyc_Lex_num_kws=0;struct Cyc_Lex_KeyWordInfo{int token_index;int is_c_keyword;};
# 78
static struct _fat_ptr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};
# 80
int Cyc_Lex_expand_specials=0;
# 82
static int Cyc_Lex_in_extern_c=0;
static struct Cyc_List_List*Cyc_Lex_prev_extern_c=0;
void Cyc_Lex_enter_extern_c (void){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_Tmp0=x;struct Cyc_List_List*_Tmp1=Cyc_Lex_prev_extern_c;x=_Tmp1;Cyc_Lex_prev_extern_c=_Tmp0;});
Cyc_Lex_prev_extern_c=({struct Cyc_List_List*_Tmp0=_aqual_malloc(Cyc_Core_unique_qual,sizeof(struct Cyc_List_List));_Tmp0->hd=(void*)Cyc_Lex_in_extern_c,_Tmp0->tl=x;_Tmp0;});
Cyc_Lex_in_extern_c=1;}
# 90
void Cyc_Lex_leave_extern_c (void){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_Tmp0=x;struct Cyc_List_List*_Tmp1=Cyc_Lex_prev_extern_c;x=_Tmp1;Cyc_Lex_prev_extern_c=_Tmp0;});
Cyc_Lex_in_extern_c=(int)_check_null(x)->hd;
Cyc_Lex_prev_extern_c=x->tl;
({void(*_Tmp0)(struct Cyc_List_List*)=(void(*)(struct Cyc_List_List*))Cyc_Core_ufree;_Tmp0;})(x);}
# 98
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;
static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;
# 101
static int Cyc_Lex_comment_depth=0;
# 104
static union Cyc_Absyn_Cnst Cyc_Lex_token_int={.Int_c={5,{.f0=Cyc_Absyn_Signed,.f1=0}}};static char _TmpG0[8U]="*bogus*";
static struct _fat_ptr Cyc_Lex_bogus_string={_TmpG0,_TmpG0,_TmpG0 + 8U};
static struct _tuple0 Cyc_Lex_token_id_pair={.f0={.Loc_n={4,0}},.f1=& Cyc_Lex_bogus_string};
static char Cyc_Lex_token_char='\000';static char _TmpG1[1U]="";
struct _fat_ptr Cyc_Lex_token_string={_TmpG1,_TmpG1,_TmpG1 + 1U};
struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;
# 111
static int Cyc_Lex_runaway_start=0;
# 113
static void Cyc_Lex_err(struct _fat_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned s=(unsigned)Cyc_Lexing_lexeme_start(lb);
Cyc_Warn_err(s,msg,_tag_fat(0U,sizeof(void*),0));}
# 117
static void Cyc_Lex_runaway_err(struct _fat_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned s=(unsigned)Cyc_Lex_runaway_start;
Cyc_Warn_err(s,msg,_tag_fat(0U,sizeof(void*),0));}
# 121
static char Cyc_Lex_cnst2char(union Cyc_Absyn_Cnst cnst,struct Cyc_Lexing_lexbuf*lb){
char _Tmp0;long long _Tmp1;int _Tmp2;switch(cnst.Char_c.tag){case 5: _Tmp2=cnst.Int_c.val.f1;{int i=_Tmp2;
return(char)i;}case 6: _Tmp1=cnst.LongLong_c.val.f1;{long long i=_Tmp1;
return(char)i;}case 2: _Tmp0=cnst.Char_c.val.f1;{char c=_Tmp0;
return c;}default:
 Cyc_Lex_err(_tag_fat("couldn't convert constant to char!",sizeof(char),35U),lb);return'\000';};}static char _TmpG2[10U]="ALIASABLE";struct _tuple27{struct _fat_ptr f0;short f1;int f2;};static char _TmpG3[8U]="aqual_t";static char _TmpG4[7U]="aquals";static char _TmpG5[14U]="__attribute__";static char _TmpG6[12U]="__attribute";static char _TmpG7[8U]="__asm__";static char _TmpG8[6U]="__asm";static char _TmpG9[4U]="asm";static char _TmpGA[9U]="abstract";static char _TmpGB[5U]="auto";static char _TmpGC[6U]="break";static char _TmpGD[18U]="__builtin_va_list";static char _TmpGE[7U]="calloc";static char _TmpGF[5U]="case";static char _TmpG10[6U]="catch";static char _TmpG11[5U]="char";static char _TmpG12[9U]="_Complex";static char _TmpG13[12U]="__complex__";static char _TmpG14[6U]="const";static char _TmpG15[10U]="__const__";static char _TmpG16[9U]="continue";static char _TmpG17[17U]="cyclone_override";static char _TmpG18[9U]="datatype";static char _TmpG19[8U]="default";static char _TmpG1A[3U]="do";static char _TmpG1B[7U]="double";static char _TmpG1C[5U]="else";static char _TmpG1D[5U]="enum";static char _TmpG1E[7U]="export";static char _TmpG1F[13U]="cyclone_hide";static char _TmpG20[14U]="__extension__";static char _TmpG21[7U]="extern";static char _TmpG22[9U]="fallthru";static char _TmpG23[6U]="float";static char _TmpG24[4U]="for";static char _TmpG25[5U]="goto";static char _TmpG26[3U]="if";static char _TmpG27[7U]="inline";static char _TmpG28[11U]="__inline__";static char _TmpG29[9U]="__inline";static char _TmpG2A[4U]="int";static char _TmpG2B[4U]="let";static char _TmpG2C[5U]="long";static char _TmpG2D[7U]="malloc";static char _TmpG2E[10U]="namespace";static char _TmpG2F[4U]="new";static char _TmpG30[5U]="NULL";static char _TmpG31[8U]="numelts";static char _TmpG32[9U]="offsetof";static char _TmpG33[20U]="__cyclone_port_on__";static char _TmpG34[21U]="__cyclone_port_off__";static char _TmpG35[19U]="__cyclone_pragma__";static char _TmpG36[8U]="qcalloc";static char _TmpG37[8U]="qmalloc";static char _TmpG38[5U]="qnew";static char _TmpG39[8U]="rcalloc";static char _TmpG3A[7U]="REFCNT";static char _TmpG3B[9U]="region_t";static char _TmpG3C[7U]="region";static char _TmpG3D[8U]="regions";static char _TmpG3E[9U]="register";static char _TmpG3F[9U]="restrict";static char _TmpG40[11U]="__restrict";static char _TmpG41[11U]="RESTRICTED";static char _TmpG42[7U]="return";static char _TmpG43[8U]="rmalloc";static char _TmpG44[15U]="rmalloc_inline";static char _TmpG45[9U]="rvmalloc";static char _TmpG46[5U]="rnew";static char _TmpG47[6U]="short";static char _TmpG48[7U]="signed";static char _TmpG49[11U]="__signed__";static char _TmpG4A[7U]="sizeof";static char _TmpG4B[7U]="static";static char _TmpG4C[7U]="struct";static char _TmpG4D[7U]="switch";static char _TmpG4E[9U]="tagcheck";static char _TmpG4F[6U]="tagof";static char _TmpG50[6U]="tag_t";static char _TmpG51[15U]="__tempest_on__";static char _TmpG52[16U]="__tempest_off__";static char _TmpG53[6U]="throw";static char _TmpG54[4U]="try";static char _TmpG55[8U]="typedef";static char _TmpG56[7U]="typeof";static char _TmpG57[11U]="__typeof__";static char _TmpG58[7U]="UNIQUE";static char _TmpG59[6U]="union";static char _TmpG5A[9U]="unsigned";static char _TmpG5B[13U]="__unsigned__";static char _TmpG5C[6U]="using";static char _TmpG5D[8U]="valueof";static char _TmpG5E[10U]="valueof_t";static char _TmpG5F[5U]="void";static char _TmpG60[9U]="volatile";static char _TmpG61[13U]="__volatile__";static char _TmpG62[6U]="while";
# 132
static struct _tuple27 Cyc_Lex_rw_array[97U]={{.f0={_TmpG2,_TmpG2,_TmpG2 + 10U},.f1=331,.f2=0},{.f0={_TmpG3,_TmpG3,_TmpG3 + 8U},.f1=335,.f2=0},{.f0={_TmpG4,_TmpG4,_TmpG4 + 7U},.f1=357,.f2=0},{.f0={_TmpG5,_TmpG5,_TmpG5 + 14U},.f1=396,.f2=1},{.f0={_TmpG6,_TmpG6,_TmpG6 + 12U},.f1=396,.f2=1},{.f0={_TmpG7,_TmpG7,_TmpG7 + 8U},.f1=397,.f2=1},{.f0={_TmpG8,_TmpG8,_TmpG8 + 6U},.f1=397,.f2=1},{.f0={_TmpG9,_TmpG9,_TmpG9 + 4U},.f1=397,.f2=1},{.f0={_TmpGA,_TmpGA,_TmpGA + 9U},.f1=307,.f2=0},{.f0={_TmpGB,_TmpGB,_TmpGB + 5U},.f1=258,.f2=1},{.f0={_TmpGC,_TmpGC,_TmpGC + 6U},.f1=290,.f2=1},{.f0={_TmpGD,_TmpGD,_TmpGD + 18U},.f1=294,.f2=1},{.f0={_TmpGE,_TmpGE,_TmpGE + 7U},.f1=317,.f2=0},{.f0={_TmpGF,_TmpGF,_TmpGF + 5U},.f1=277,.f2=1},{.f0={_TmpG10,_TmpG10,_TmpG10 + 6U},.f1=301,.f2=1},{.f0={_TmpG11,_TmpG11,_TmpG11 + 5U},.f1=264,.f2=1},{.f0={_TmpG12,_TmpG12,_TmpG12 + 9U},.f1=296,.f2=1},{.f0={_TmpG13,_TmpG13,_TmpG13 + 12U},.f1=296,.f2=1},{.f0={_TmpG14,_TmpG14,_TmpG14 + 6U},.f1=272,.f2=1},{.f0={_TmpG15,_TmpG15,_TmpG15 + 10U},.f1=272,.f2=1},{.f0={_TmpG16,_TmpG16,_TmpG16 + 9U},.f1=289,.f2=1},{.f0={_TmpG17,_TmpG17,_TmpG17 + 17U},.f1=303,.f2=0},{.f0={_TmpG18,_TmpG18,_TmpG18 + 9U},.f1=311,.f2=0},{.f0={_TmpG19,_TmpG19,_TmpG19 + 8U},.f1=278,.f2=1},{.f0={_TmpG1A,_TmpG1A,_TmpG1A + 3U},.f1=286,.f2=1},{.f0={_TmpG1B,_TmpG1B,_TmpG1B + 7U},.f1=269,.f2=1},{.f0={_TmpG1C,_TmpG1C,_TmpG1C + 5U},.f1=283,.f2=1},{.f0={_TmpG1D,_TmpG1D,_TmpG1D + 5U},.f1=292,.f2=1},{.f0={_TmpG1E,_TmpG1E,_TmpG1E + 7U},.f1=302,.f2=0},{.f0={_TmpG1F,_TmpG1F,_TmpG1F + 13U},.f1=304,.f2=0},{.f0={_TmpG20,_TmpG20,_TmpG20 + 14U},.f1=295,.f2=1},{.f0={_TmpG21,_TmpG21,_TmpG21 + 7U},.f1=261,.f2=1},{.f0={_TmpG22,_TmpG22,_TmpG22 + 9U},.f1=308,.f2=0},{.f0={_TmpG23,_TmpG23,_TmpG23 + 6U},.f1=268,.f2=1},{.f0={_TmpG24,_TmpG24,_TmpG24 + 4U},.f1=287,.f2=1},{.f0={_TmpG25,_TmpG25,_TmpG25 + 5U},.f1=288,.f2=1},{.f0={_TmpG26,_TmpG26,_TmpG26 + 3U},.f1=282,.f2=1},{.f0={_TmpG27,_TmpG27,_TmpG27 + 7U},.f1=279,.f2=1},{.f0={_TmpG28,_TmpG28,_TmpG28 + 11U},.f1=279,.f2=1},{.f0={_TmpG29,_TmpG29,_TmpG29 + 9U},.f1=279,.f2=1},{.f0={_TmpG2A,_TmpG2A,_TmpG2A + 4U},.f1=266,.f2=1},{.f0={_TmpG2B,_TmpG2B,_TmpG2B + 4U},.f1=298,.f2=0},{.f0={_TmpG2C,_TmpG2C,_TmpG2C + 5U},.f1=267,.f2=1},{.f0={_TmpG2D,_TmpG2D,_TmpG2D + 7U},.f1=312,.f2=0},{.f0={_TmpG2E,_TmpG2E,_TmpG2E + 10U},.f1=310,.f2=0},{.f0={_TmpG2F,_TmpG2F,_TmpG2F + 4U},.f1=305,.f2=0},{.f0={_TmpG30,_TmpG30,_TmpG30 + 5U},.f1=297,.f2=0},{.f0={_TmpG31,_TmpG31,_TmpG31 + 8U},.f1=336,.f2=0},{.f0={_TmpG32,_TmpG32,_TmpG32 + 9U},.f1=281,.f2=1},{.f0={_TmpG33,_TmpG33,_TmpG33 + 20U},.f1=326,.f2=0},{.f0={_TmpG34,_TmpG34,_TmpG34 + 21U},.f1=327,.f2=0},{.f0={_TmpG35,_TmpG35,_TmpG35 + 19U},.f1=328,.f2=0},{.f0={_TmpG36,_TmpG36,_TmpG36 + 8U},.f1=318,.f2=0},{.f0={_TmpG37,_TmpG37,_TmpG37 + 8U},.f1=316,.f2=0},{.f0={_TmpG38,_TmpG38,_TmpG38 + 5U},.f1=306,.f2=0},{.f0={_TmpG39,_TmpG39,_TmpG39 + 8U},.f1=319,.f2=0},{.f0={_TmpG3A,_TmpG3A,_TmpG3A + 7U},.f1=332,.f2=0},{.f0={_TmpG3B,_TmpG3B,_TmpG3B + 9U},.f1=321,.f2=0},{.f0={_TmpG3C,_TmpG3C,_TmpG3C + 7U},.f1=323,.f2=0},{.f0={_TmpG3D,_TmpG3D,_TmpG3D + 8U},.f1=325,.f2=0},{.f0={_TmpG3E,_TmpG3E,_TmpG3E + 9U},.f1=259,.f2=1},{.f0={_TmpG3F,_TmpG3F,_TmpG3F + 9U},.f1=274,.f2=1},{.f0={_TmpG40,_TmpG40,_TmpG40 + 11U},.f1=274,.f2=1},{.f0={_TmpG41,_TmpG41,_TmpG41 + 11U},.f1=333,.f2=0},{.f0={_TmpG42,_TmpG42,_TmpG42 + 7U},.f1=291,.f2=1},{.f0={_TmpG43,_TmpG43,_TmpG43 + 8U},.f1=313,.f2=0},{.f0={_TmpG44,_TmpG44,_TmpG44 + 15U},.f1=315,.f2=0},{.f0={_TmpG45,_TmpG45,_TmpG45 + 9U},.f1=314,.f2=0},{.f0={_TmpG46,_TmpG46,_TmpG46 + 5U},.f1=324,.f2=0},{.f0={_TmpG47,_TmpG47,_TmpG47 + 6U},.f1=265,.f2=1},{.f0={_TmpG48,_TmpG48,_TmpG48 + 7U},.f1=270,.f2=1},{.f0={_TmpG49,_TmpG49,_TmpG49 + 11U},.f1=270,.f2=1},{.f0={_TmpG4A,_TmpG4A,_TmpG4A + 7U},.f1=280,.f2=1},{.f0={_TmpG4B,_TmpG4B,_TmpG4B + 7U},.f1=260,.f2=1},{.f0={_TmpG4C,_TmpG4C,_TmpG4C + 7U},.f1=275,.f2=1},{.f0={_TmpG4D,_TmpG4D,_TmpG4D + 7U},.f1=284,.f2=1},{.f0={_TmpG4E,_TmpG4E,_TmpG4E + 9U},.f1=340,.f2=0},{.f0={_TmpG4F,_TmpG4F,_TmpG4F + 6U},.f1=337,.f2=0},{.f0={_TmpG50,_TmpG50,_TmpG50 + 6U},.f1=322,.f2=0},{.f0={_TmpG51,_TmpG51,_TmpG51 + 15U},.f1=329,.f2=0},{.f0={_TmpG52,_TmpG52,_TmpG52 + 16U},.f1=330,.f2=0},{.f0={_TmpG53,_TmpG53,_TmpG53 + 6U},.f1=299,.f2=0},{.f0={_TmpG54,_TmpG54,_TmpG54 + 4U},.f1=300,.f2=0},{.f0={_TmpG55,_TmpG55,_TmpG55 + 8U},.f1=262,.f2=1},{.f0={_TmpG56,_TmpG56,_TmpG56 + 7U},.f1=293,.f2=1},{.f0={_TmpG57,_TmpG57,_TmpG57 + 11U},.f1=293,.f2=1},{.f0={_TmpG58,_TmpG58,_TmpG58 + 7U},.f1=334,.f2=0},{.f0={_TmpG59,_TmpG59,_TmpG59 + 6U},.f1=276,.f2=1},{.f0={_TmpG5A,_TmpG5A,_TmpG5A + 9U},.f1=271,.f2=1},{.f0={_TmpG5B,_TmpG5B,_TmpG5B + 13U},.f1=271,.f2=1},{.f0={_TmpG5C,_TmpG5C,_TmpG5C + 6U},.f1=309,.f2=0},{.f0={_TmpG5D,_TmpG5D,_TmpG5D + 8U},.f1=338,.f2=0},{.f0={_TmpG5E,_TmpG5E,_TmpG5E + 10U},.f1=339,.f2=0},{.f0={_TmpG5F,_TmpG5F,_TmpG5F + 5U},.f1=263,.f2=1},{.f0={_TmpG60,_TmpG60,_TmpG60 + 9U},.f1=273,.f2=1},{.f0={_TmpG61,_TmpG61,_TmpG61 + 13U},.f1=273,.f2=1},{.f0={_TmpG62,_TmpG62,_TmpG62 + 6U},.f1=285,.f2=1}};
# 232
static int Cyc_Lex_num_keywords(int include_cyclone_keywords){
int sum=0;
{unsigned i=0U;for(0;i < 97U;++ i){
if(include_cyclone_keywords ||(*((struct _tuple27*)_check_known_subscript_notnull(Cyc_Lex_rw_array,97U,sizeof(struct _tuple27),(int)i))).f2)
++ sum;}}
return sum;}
# 243
static int Cyc_Lex_do_possible_qualifier(struct Cyc_Lexing_lexbuf*lexbuf){
struct _fat_ptr b=_fat_ptr_plus(_fat_ptr_plus(lexbuf->lex_buffer,sizeof(char),lexbuf->lex_start_pos),sizeof(char),1);
unsigned len=(unsigned)(lexbuf->lex_curr_pos - (lexbuf->lex_start_pos + 1));
{char _Tmp0=*((const char*)_check_fat_subscript(b,sizeof(char),0));switch((int)_Tmp0){case 116:
# 248
 if(len==6U && Cyc_strncmp(b,_tag_fat("tagged",sizeof(char),7U),6U)==0)return 353;
if(len==4U && Cyc_strncmp(b,_tag_fat("thin",sizeof(char),5U),4U)==0)return 342;
if(len==6U && Cyc_strncmp(b,_tag_fat("throws",sizeof(char),7U),6U)==0)return 349;
goto _LL0;case 110:
# 253
 if(len==7U && Cyc_strncmp(b,_tag_fat("notnull",sizeof(char),8U),7U)==0)return 344;
if(len==7U && Cyc_strncmp(b,_tag_fat("numelts",sizeof(char),8U),7U)==0)return 341;
if(len==8U && Cyc_strncmp(b,_tag_fat("nullable",sizeof(char),9U),8U)==0)return 345;
if(len==10U && Cyc_strncmp(b,_tag_fat("nozeroterm",sizeof(char),11U),10U)==0)return 351;
goto _LL0;case 122:
# 259
 if(len==8U && Cyc_strncmp(b,_tag_fat("zeroterm",sizeof(char),9U),8U)==0)return 352;
goto _LL0;case 101:
# 262
 if(len==6U && Cyc_strncmp(b,_tag_fat("effect",sizeof(char),7U),6U)==0)return 348;
if(len==7U && Cyc_strncmp(b,_tag_fat("ensures",sizeof(char),8U),7U)==0)return 347;
if(len==10U && Cyc_strncmp(b,_tag_fat("extensible",sizeof(char),11U),10U)==0)return 358;
goto _LL0;case 114:
# 267
 if(len==8U && Cyc_strncmp(b,_tag_fat("requires",sizeof(char),9U),8U)==0)return 346;
if(len==6U && Cyc_strncmp(b,_tag_fat("region",sizeof(char),7U),6U)==0)return 350;
goto _LL0;case 102:
# 271
 if(len==3U && Cyc_strncmp(b,_tag_fat("fat",sizeof(char),4U),3U)==0)return 343;
goto _LL0;case 97:
# 274
 if(len==5U && Cyc_strncmp(b,_tag_fat("aqual",sizeof(char),6U),5U)==0)return 356;
if(len==6U && Cyc_strncmp(b,_tag_fat("assert",sizeof(char),7U),6U)==0)return 354;
if(len==8U && Cyc_strncmp(b,_tag_fat("assert__",sizeof(char),9U),8U)==0)return 354;
if(len==12U && Cyc_strncmp(b,_tag_fat("assert_false",sizeof(char),13U),12U)==0)return 355;
if(len==12U && Cyc_strncmp(b,_tag_fat("autoreleased",sizeof(char),13U),12U)==0)return 359;
goto _LL0;default:
 goto _LL0;}_LL0:;}
# 282
lexbuf->lex_curr_pos=lexbuf->lex_start_pos + 1;
return 64;}
# 287
static struct Cyc_Lex_Trie*Cyc_Lex_empty_trie(struct _RegionHandle*d,int dummy){
struct Cyc_Lex_Trie*_Tmp0=_region_malloc(d,0U,sizeof(struct Cyc_Lex_Trie));({union Cyc_Lex_TrieChildren _Tmp1=Cyc_Lex_Zero();_Tmp0->children=_Tmp1;}),_Tmp0->shared_str=0;return _Tmp0;}
# 295
static int Cyc_Lex_trie_char(int c){
# 297
if(c==92)return 37;
if(c >= 95)return c - 58;
if(c > 64)return c - 55;
return c - 48;}
# 303
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _fat_ptr buff,int offset,unsigned len){
# 305
unsigned i=0U;
buff=_fat_ptr_plus(buff,sizeof(char),offset);
if(!(len < _get_fat_size(buff,sizeof(char))))
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("array bounds in trie_lookup",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));
while(i < len){
{union Cyc_Lex_TrieChildren _Tmp0=t->children;int _Tmp1;void*_Tmp2;switch(_Tmp0.One.tag){case 3: _Tmp2=_Tmp0.Many.val;{struct Cyc_Lex_Trie**arr=_Tmp2;
# 313
int ch=Cyc_Lex_trie_char((int)*((const char*)_check_fat_subscript(buff,sizeof(char),(int)i)));
if(*((struct Cyc_Lex_Trie**)_check_known_subscript_notnull(arr,65U,sizeof(struct Cyc_Lex_Trie*),ch))==0)
({struct Cyc_Lex_Trie*_Tmp3=({struct Cyc_Lex_Trie*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_Lex_Trie));({union Cyc_Lex_TrieChildren _Tmp5=Cyc_Lex_Zero();_Tmp4->children=_Tmp5;}),_Tmp4->shared_str=0;_Tmp4;});arr[ch]=_Tmp3;});
t=arr[ch];
++ i;
goto _LL0;}case 2: _Tmp1=_Tmp0.One.val.f0;_Tmp2=_Tmp0.One.val.f1;{int one_ch=_Tmp1;struct Cyc_Lex_Trie*one_trie=_Tmp2;
# 321
if(one_ch==(int)*((const char*)_check_fat_subscript(buff,sizeof(char),(int)i)))
t=one_trie;else{
# 324
struct Cyc_Lex_Trie**arr=({unsigned _Tmp3=65U;({struct Cyc_Lex_Trie**_Tmp4=({struct _RegionHandle*_Tmp5=r;_region_malloc(_Tmp5,0U,_check_times(_Tmp3,sizeof(struct Cyc_Lex_Trie*)));});({{unsigned _Tmp5=65U;unsigned j;for(j=0;j < _Tmp5;++ j){_Tmp4[j]=0;}}0;});_Tmp4;});});
*((struct Cyc_Lex_Trie**)({typeof(arr )_Tmp3=arr;_check_known_subscript_notnull(_Tmp3,65U,sizeof(struct Cyc_Lex_Trie*),Cyc_Lex_trie_char(one_ch));}))=one_trie;{
int ch=Cyc_Lex_trie_char((int)((const char*)buff.curr)[(int)i]);
({struct Cyc_Lex_Trie*_Tmp3=({struct Cyc_Lex_Trie*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_Lex_Trie));({union Cyc_Lex_TrieChildren _Tmp5=Cyc_Lex_Zero();_Tmp4->children=_Tmp5;}),_Tmp4->shared_str=0;_Tmp4;});*((struct Cyc_Lex_Trie**)_check_known_subscript_notnull(arr,65U,sizeof(struct Cyc_Lex_Trie*),ch))=_Tmp3;});
({union Cyc_Lex_TrieChildren _Tmp3=Cyc_Lex_Many(arr);t->children=_Tmp3;});
t=arr[ch];}}
# 331
++ i;
goto _LL0;}default:
# 335
 while(i < len){
{struct Cyc_Lex_Trie*next;next=_region_malloc(r,0U,sizeof(struct Cyc_Lex_Trie)),({union Cyc_Lex_TrieChildren _Tmp3=Cyc_Lex_Zero();next->children=_Tmp3;}),next->shared_str=0;
({union Cyc_Lex_TrieChildren _Tmp3=Cyc_Lex_One((int)*((const char*)_check_fat_subscript(buff,sizeof(char),(int)i ++)),next);t->children=_Tmp3;});
t=next;}
# 336
1U;}
# 340
return t;}_LL0:;}
# 310
1U;}
# 342
return t;}struct _tuple28{struct Cyc_Lex_Trie*f0;struct Cyc_Xarray_Xarray*f1;struct _fat_ptr f2;int f3;int f4;};
# 345
static int Cyc_Lex_str_index_body(struct _RegionHandle*d,struct _tuple28*env){
# 348
struct _tuple28 _Tmp0=*env;int _Tmp1;int _Tmp2;struct _fat_ptr _Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp0.f0;_Tmp4=_Tmp0.f1;_Tmp3=_Tmp0.f2;_Tmp2=_Tmp0.f3;_Tmp1=_Tmp0.f4;{struct Cyc_Lex_Trie*tree=_Tmp5;struct Cyc_Xarray_Xarray*symbols=_Tmp4;struct _fat_ptr buff=_Tmp3;int offset=_Tmp2;int len=_Tmp1;
struct Cyc_Lex_Trie*t=Cyc_Lex_trie_lookup(d,tree,buff,offset,(unsigned)len);
# 351
if(_check_null(t)->shared_str==0){
struct _fat_ptr newstr=Cyc_Core_new_string((unsigned)(len + 1));
({struct _fat_ptr _Tmp6=_fat_ptr_decrease_size(newstr,sizeof(char),1U);struct _fat_ptr _Tmp7=(struct _fat_ptr)_fat_ptr_plus(buff,sizeof(char),offset);Cyc_zstrncpy(_Tmp6,_Tmp7,(unsigned long)len);});{
int ans=({int(*_Tmp6)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*)=({int(*_Tmp7)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*)=(int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind;_Tmp7;});struct Cyc_Xarray_Xarray*_Tmp7=symbols;_Tmp6(_Tmp7,({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));*_Tmp8=newstr;_Tmp8;}));});
t->shared_str=ans;}}
# 357
return t->shared_str;}}
# 360
static int Cyc_Lex_str_index(struct _fat_ptr buff,int offset,int len){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_Tmp0=idt;struct Cyc_Lex_DynTrieSymbols*_Tmp1=Cyc_Lex_ids_trie;idt=_Tmp1;Cyc_Lex_ids_trie=_Tmp0;});{
struct Cyc_Lex_DynTrieSymbols _Tmp0=*idt;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.dyn;_Tmp2=_Tmp0.t;_Tmp1=_Tmp0.symbols;{struct Cyc_Core_DynamicRegion*dyn=_Tmp3;struct Cyc_Lex_Trie*t=_Tmp2;struct Cyc_Xarray_Xarray*symbols=_Tmp1;
struct _tuple28 env=({struct _tuple28 _Tmp4;_Tmp4.f0=t,_Tmp4.f1=symbols,_Tmp4.f2=buff,_Tmp4.f3=offset,_Tmp4.f4=len;_Tmp4;});
int res=({int(*_Tmp4)(struct Cyc_Core_DynamicRegion*,struct _tuple28*,int(*)(struct _RegionHandle*,struct _tuple28*))=(int(*)(struct Cyc_Core_DynamicRegion*,struct _tuple28*,int(*)(struct _RegionHandle*,struct _tuple28*)))Cyc_Core_open_region;_Tmp4;})(dyn,& env,Cyc_Lex_str_index_body);
({struct Cyc_Lex_DynTrieSymbols _Tmp4=({struct Cyc_Lex_DynTrieSymbols _Tmp5;_Tmp5.dyn=dyn,_Tmp5.t=t,_Tmp5.symbols=symbols;_Tmp5;});*idt=_Tmp4;});
({struct Cyc_Lex_DynTrieSymbols*_Tmp4=idt;struct Cyc_Lex_DynTrieSymbols*_Tmp5=Cyc_Lex_ids_trie;idt=_Tmp5;Cyc_Lex_ids_trie=_Tmp4;});
return res;}}}
# 371
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index(lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}struct _tuple29{struct Cyc_Lex_Trie*f0;struct _fat_ptr f1;};
# 377
static int Cyc_Lex_insert_typedef_body(struct _RegionHandle*h,struct _tuple29*arg){
struct _tuple29 _Tmp0=*arg;struct _fat_ptr _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_Lex_Trie*t=_Tmp2;struct _fat_ptr s=_Tmp1;
struct Cyc_Lex_Trie*t_node=Cyc_Lex_trie_lookup(h,t,s,0,_get_fat_size(s,sizeof(char))- 1U);
_check_null(t_node)->shared_str=1;
return 0;}}
# 384
static void Cyc_Lex_insert_typedef(struct _fat_ptr*sptr){
struct _fat_ptr s=*sptr;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_Tmp0=tdefs;struct Cyc_Lex_DynTrie*_Tmp1=Cyc_Lex_typedefs_trie;tdefs=_Tmp1;Cyc_Lex_typedefs_trie=_Tmp0;});{
struct Cyc_Lex_DynTrie _Tmp0=*_check_null(tdefs);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.dyn;_Tmp1=_Tmp0.t;{struct Cyc_Core_DynamicRegion*dyn=_Tmp2;struct Cyc_Lex_Trie*t=_Tmp1;
struct _tuple29 env=({struct _tuple29 _Tmp3;_Tmp3.f0=t,_Tmp3.f1=s;_Tmp3;});
({int(*_Tmp3)(struct Cyc_Core_DynamicRegion*,struct _tuple29*,int(*)(struct _RegionHandle*,struct _tuple29*))=(int(*)(struct Cyc_Core_DynamicRegion*,struct _tuple29*,int(*)(struct _RegionHandle*,struct _tuple29*)))Cyc_Core_open_region;_Tmp3;})(dyn,& env,Cyc_Lex_insert_typedef_body);
({struct Cyc_Lex_DynTrie _Tmp3=({struct Cyc_Lex_DynTrie _Tmp4;_Tmp4.dyn=dyn,_Tmp4.t=t;_Tmp4;});*tdefs=_Tmp3;});
({struct Cyc_Lex_DynTrie*_Tmp3=tdefs;struct Cyc_Lex_DynTrie*_Tmp4=Cyc_Lex_typedefs_trie;tdefs=_Tmp4;Cyc_Lex_typedefs_trie=_Tmp3;});
return;}}}struct _tuple30{struct Cyc_Lex_Trie*f0;struct Cyc_Xarray_Xarray*f1;int f2;};
# 396
static struct _fat_ptr*Cyc_Lex_get_symbol_body(struct _RegionHandle*dyn,struct _tuple30*env){
struct _tuple30 _Tmp0=*env;int _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_Lex_Trie*t=_Tmp3;struct Cyc_Xarray_Xarray*symbols=_Tmp2;int symbol_num=_Tmp1;
return({struct _fat_ptr*(*_Tmp4)(struct Cyc_Xarray_Xarray*,int)=(struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get;_Tmp4;})(symbols,symbol_num);}}
# 401
static struct _fat_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_Tmp0=idt;struct Cyc_Lex_DynTrieSymbols*_Tmp1=Cyc_Lex_ids_trie;idt=_Tmp1;Cyc_Lex_ids_trie=_Tmp0;});{
struct Cyc_Lex_DynTrieSymbols _Tmp0=*idt;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.dyn;_Tmp2=_Tmp0.t;_Tmp1=_Tmp0.symbols;{struct Cyc_Core_DynamicRegion*dyn=_Tmp3;struct Cyc_Lex_Trie*t=_Tmp2;struct Cyc_Xarray_Xarray*symbols=_Tmp1;
struct _tuple30 env=({struct _tuple30 _Tmp4;_Tmp4.f0=t,_Tmp4.f1=symbols,_Tmp4.f2=symbol_num;_Tmp4;});
struct _fat_ptr*res=({struct _fat_ptr*(*_Tmp4)(struct Cyc_Core_DynamicRegion*,struct _tuple30*,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple30*))=(struct _fat_ptr*(*)(struct Cyc_Core_DynamicRegion*,struct _tuple30*,struct _fat_ptr*(*)(struct _RegionHandle*,struct _tuple30*)))Cyc_Core_open_region;_Tmp4;})(dyn,& env,Cyc_Lex_get_symbol_body);
({struct Cyc_Lex_DynTrieSymbols _Tmp4=({struct Cyc_Lex_DynTrieSymbols _Tmp5;_Tmp5.dyn=dyn,_Tmp5.t=t,_Tmp5.symbols=symbols;_Tmp5;});*idt=_Tmp4;});
({struct Cyc_Lex_DynTrieSymbols*_Tmp4=idt;struct Cyc_Lex_DynTrieSymbols*_Tmp5=Cyc_Lex_ids_trie;idt=_Tmp5;Cyc_Lex_ids_trie=_Tmp4;});
return res;}}}
# 420 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if(48 <= (int)c &&(int)c <= 57)return(int)c - 48;
if(97 <= (int)c &&(int)c <= 102)return(10 + (int)c)- 97;
if(65 <= (int)c &&(int)c <= 70)return(10 + (int)c)- 65;
_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=_tag_fat("string to integer conversion",sizeof(char),29U);_Tmp0;}));}
# 428
static union Cyc_Absyn_Cnst Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
enum Cyc_Absyn_Sign sn=2U;
start +=lbuf->lex_start_pos;{
struct _fat_ptr buff=_fat_ptr_plus(lbuf->lex_buffer,sizeof(char),start);
int end2=lbuf->lex_curr_pos - end;
int len=end2 - start;
enum Cyc_Absyn_Size_of size=2U;
int declared_size=0;
union Cyc_Absyn_Cnst res;
if(len >= 1 &&((int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))==108 ||(int)((char*)buff.curr)[len - 1]==76)){
# 439
len -=1;
declared_size=1;
if(len >= 1 &&((int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))==108 ||(int)((char*)buff.curr)[len - 1]==76)){
len -=1;
size=4U;}}
# 446
if(len >= 1 &&((int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))==117 ||(int)((char*)buff.curr)[len - 1]==85)){
len -=1;
sn=1U;}
# 450
if((int)sn==1){
unsigned long long n=0U;
{int i=0;for(0;i < len;++ i){
n=({unsigned long long _Tmp0=n * (unsigned long long)base;_Tmp0 + (unsigned long long)Cyc_Lex_int_of_char(*((char*)_check_fat_subscript(buff,sizeof(char),i)));});}}
if(n > 4294967295U){
if(declared_size &&(int)size==2)
Cyc_Lex_err(_tag_fat("integer constant too large",sizeof(char),27U),lbuf);
size=4U;}
# 459
if((int)size==2)
res=({union Cyc_Absyn_Cnst _Tmp0;_Tmp0.Int_c.tag=5U,_Tmp0.Int_c.val.f0=sn,_Tmp0.Int_c.val.f1=(int)((unsigned)n);_Tmp0;});else{
# 462
res=({union Cyc_Absyn_Cnst _Tmp0;_Tmp0.LongLong_c.tag=6U,_Tmp0.LongLong_c.val.f0=sn,_Tmp0.LongLong_c.val.f1=(long long)n;_Tmp0;});}}else{
# 464
long long n=0;
{int i=0;for(0;i < len;++ i){
n=({long long _Tmp0=n * (long long)base;_Tmp0 + (long long)Cyc_Lex_int_of_char(*((char*)_check_fat_subscript(buff,sizeof(char),i)));});}}{
unsigned long long x=(unsigned long long)n >> 32U;
if(x!=4294967295U && x!=0U){
if(declared_size &&(int)size==2)
Cyc_Lex_err(_tag_fat("integer constant too large",sizeof(char),27U),lbuf);
size=4U;}
# 473
if((int)size==2)
res=({union Cyc_Absyn_Cnst _Tmp0;_Tmp0.Int_c.tag=5U,_Tmp0.Int_c.val.f0=sn,_Tmp0.Int_c.val.f1=(int)n;_Tmp0;});else{
# 476
res=({union Cyc_Absyn_Cnst _Tmp0;_Tmp0.LongLong_c.tag=6U,_Tmp0.LongLong_c.val.f0=sn,_Tmp0.LongLong_c.val.f1=n;_Tmp0;});}}}
# 478
return res;}}
# 483
char Cyc_Lex_string_buffer_v[11U]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _fat_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11U)};
int Cyc_Lex_string_pos=0;
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_fat_size(Cyc_Lex_string_buffer,sizeof(char))- 1U);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;1U;}{
struct _fat_ptr str=({unsigned _Tmp0=(unsigned)newsz + 1U;_tag_fat(({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=(unsigned)newsz;unsigned i;for(i=0;i < _Tmp2;++ i){i < (unsigned)sz?_Tmp1[i]=*((char*)_check_fat_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):(_Tmp1[i]='\000');}_Tmp1[_Tmp2]=0;}0;});_Tmp1;}),sizeof(char),_Tmp0);});
Cyc_Lex_string_buffer=str;}}
# 494
({struct _fat_ptr _Tmp0=_fat_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
++ Cyc_Lex_string_pos;}
# 497
void Cyc_Lex_store_string(struct _fat_ptr s){
int sz=(int)Cyc_strlen(s);
int i=0;for(0;i < sz;++ i){
Cyc_Lex_store_string_char(*((const char*)_check_fat_subscript(s,sizeof(char),i)));}}
# 502
struct _fat_ptr Cyc_Lex_get_stored_string (void){
struct _fat_ptr str=Cyc_substring(Cyc_Lex_string_buffer,0,(unsigned long)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;
return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;};
# 520 "lex.cyl"
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;
# 522
static struct Cyc_Lex_Ldecls*Cyc_Lex_mk_empty_ldecls(int ignore){
struct Cyc_Lex_Ldecls*_Tmp0=_cycalloc(sizeof(struct Cyc_Lex_Ldecls));({struct Cyc_Set_Set*_Tmp1=({struct Cyc_Set_Set*(*_Tmp2)(int(*)(struct _fat_ptr*,struct _fat_ptr*))=(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;_Tmp2;})(Cyc_zstrptrcmp);_Tmp0->typedefs=_Tmp1;});return _Tmp0;}
# 526
static void Cyc_Lex_typedef_init (void){
Cyc_Lex_lstate=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_Binding_NSCtxt*_Tmp1=({struct Cyc_Binding_NSCtxt*(*_Tmp2)(int,struct Cyc_Lex_Ldecls*(*)(int))=(struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Lex_Ldecls*(*)(int)))Cyc_Binding_mt_nsctxt;_Tmp2;})(1,Cyc_Lex_mk_empty_ldecls);_Tmp0->v=_Tmp1;});_Tmp0;});}
# 530
static void Cyc_Lex_recompute_typedefs (void){
# 532
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_Tmp0=tdefs;struct Cyc_Lex_DynTrie*_Tmp1=Cyc_Lex_typedefs_trie;tdefs=_Tmp1;Cyc_Lex_typedefs_trie=_Tmp0;});{
struct Cyc_Lex_DynTrie _Tmp0=*tdefs;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.dyn;_Tmp1=_Tmp0.t;{struct Cyc_Core_DynamicRegion*dyn=_Tmp2;struct Cyc_Lex_Trie*t=_Tmp1;
Cyc_Core_free_ukey(dyn);{
struct Cyc_Core_NewDynamicRegion _Tmp3=Cyc_Core__new_ukey(0U,"internal-error","internal-error",0);void*_Tmp4;_Tmp4=_Tmp3.key;{struct Cyc_Core_DynamicRegion*dyn2=_Tmp4;
struct Cyc_Lex_Trie*t2=({struct Cyc_Lex_Trie*(*_Tmp5)(struct Cyc_Core_DynamicRegion*,int,struct Cyc_Lex_Trie*(*)(struct _RegionHandle*,int))=(struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*,int,struct Cyc_Lex_Trie*(*)(struct _RegionHandle*,int)))Cyc_Core_open_region;_Tmp5;})(dyn2,0,Cyc_Lex_empty_trie);
({struct Cyc_Lex_DynTrie _Tmp5=({struct Cyc_Lex_DynTrie _Tmp6;_Tmp6.dyn=dyn2,_Tmp6.t=t2;_Tmp6;});*tdefs=_Tmp5;});
({struct Cyc_Lex_DynTrie*_Tmp5=Cyc_Lex_typedefs_trie;struct Cyc_Lex_DynTrie*_Tmp6=tdefs;Cyc_Lex_typedefs_trie=_Tmp6;tdefs=_Tmp5;});{
# 542
struct Cyc_List_List*as=((struct Cyc_Binding_NSCtxt*)Cyc_Lex_lstate->v)->availables;for(0;as!=0;as=as->tl){
void*_Tmp5=(void*)as->hd;void*_Tmp6;if(*((int*)_Tmp5)==1){_Tmp6=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_Tmp5)->f1;{struct Cyc_List_List*ns=_Tmp6;
_Tmp6=ns;goto _LLA;}}else{_Tmp6=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_Tmp5)->f1;_LLA: {struct Cyc_List_List*ns=_Tmp6;
# 546
struct Cyc_Lex_Ldecls*ts=({struct Cyc_Lex_Ldecls*(*_Tmp7)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;_Tmp7;})(((struct Cyc_Binding_NSCtxt*)Cyc_Lex_lstate->v)->ns_data,ns);
({void(*_Tmp7)(void(*)(struct _fat_ptr*),struct Cyc_Set_Set*)=(void(*)(void(*)(struct _fat_ptr*),struct Cyc_Set_Set*))Cyc_Set_iter;_Tmp7;})(Cyc_Lex_insert_typedef,ts->typedefs);
goto _LL6;}}_LL6:;}}}}}}}
# 554
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _fat_ptr*v){
struct Cyc_List_List*ans=Cyc_Binding_resolve_rel_ns(0U,(struct Cyc_Binding_NSCtxt*)Cyc_Lex_lstate->v,ns);
struct Cyc_Lex_Ldecls*ts=({struct Cyc_Lex_Ldecls*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;_Tmp0;})(((struct Cyc_Binding_NSCtxt*)Cyc_Lex_lstate->v)->ns_data,ans);
return({int(*_Tmp0)(struct Cyc_Set_Set*,struct _fat_ptr*)=(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;_Tmp0;})(ts->typedefs,v);}struct _tuple31{struct Cyc_List_List*f0;struct Cyc_Lex_Trie*f1;struct _fat_ptr f2;};
# 560
static int Cyc_Lex_is_typedef_body(struct _RegionHandle*d,struct _tuple31*env){
# 563
struct _tuple31 _Tmp0=*env;struct _fat_ptr _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*ns=_Tmp3;struct Cyc_Lex_Trie*t=_Tmp2;struct _fat_ptr s=_Tmp1;
int len=(int)(_get_fat_size(s,sizeof(char))- 1U);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _Tmp4=_check_null(t)->children;void*_Tmp5;int _Tmp6;switch(_Tmp4.One.tag){case 1:
 return 0;case 2: _Tmp6=_Tmp4.One.val.f0;_Tmp5=_Tmp4.One.val.f1;if((int)_Tmp6!=(int)((const char*)s.curr)[i]){int one_ch=_Tmp6;struct Cyc_Lex_Trie*one_trie=_Tmp5;
return 0;}else{_Tmp5=_Tmp4.One.val.f1;{struct Cyc_Lex_Trie*one_trie=_Tmp5;
t=one_trie;goto _LL3;}}default: _Tmp5=_Tmp4.Many.val;{struct Cyc_Lex_Trie**arr=_Tmp5;
# 571
struct Cyc_Lex_Trie*next=*((struct Cyc_Lex_Trie**)({typeof(arr )_Tmp7=arr;_check_known_subscript_notnull(_Tmp7,65U,sizeof(struct Cyc_Lex_Trie*),Cyc_Lex_trie_char((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))));}));
if(next==0)
return 0;
t=next;
goto _LL3;}}_LL3:;}}
# 577
return _check_null(t)->shared_str;}}
# 579
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _fat_ptr*v){
if(ns!=0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 584
struct _fat_ptr s=*v;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_Tmp0=tdefs;struct Cyc_Lex_DynTrie*_Tmp1=Cyc_Lex_typedefs_trie;tdefs=_Tmp1;Cyc_Lex_typedefs_trie=_Tmp0;});{
struct Cyc_Lex_DynTrie _Tmp0=*tdefs;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.dyn;_Tmp1=_Tmp0.t;{struct Cyc_Core_DynamicRegion*dyn=_Tmp2;struct Cyc_Lex_Trie*t=_Tmp1;
struct _tuple31 env=({struct _tuple31 _Tmp3;_Tmp3.f0=ns,_Tmp3.f1=t,_Tmp3.f2=s;_Tmp3;});
int res=({int(*_Tmp3)(struct Cyc_Core_DynamicRegion*,struct _tuple31*,int(*)(struct _RegionHandle*,struct _tuple31*))=(int(*)(struct Cyc_Core_DynamicRegion*,struct _tuple31*,int(*)(struct _RegionHandle*,struct _tuple31*)))Cyc_Core_open_region;_Tmp3;})(dyn,& env,Cyc_Lex_is_typedef_body);
({struct Cyc_Lex_DynTrie _Tmp3=({struct Cyc_Lex_DynTrie _Tmp4;_Tmp4.dyn=dyn,_Tmp4.t=t;_Tmp4;});*tdefs=_Tmp3;});
({struct Cyc_Lex_DynTrie*_Tmp3=tdefs;struct Cyc_Lex_DynTrie*_Tmp4=Cyc_Lex_typedefs_trie;tdefs=_Tmp4;Cyc_Lex_typedefs_trie=_Tmp3;});
return res;}}}}
# 595
void Cyc_Lex_enter_namespace(struct _fat_ptr*s){
({void(*_Tmp0)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Lex_Ldecls*(*)(int))=(void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Lex_Ldecls*(*)(int)))Cyc_Binding_enter_ns;_Tmp0;})((struct Cyc_Binding_NSCtxt*)_check_null(Cyc_Lex_lstate)->v,s,1,Cyc_Lex_mk_empty_ldecls);{
struct Cyc_Lex_Ldecls*ts=({struct Cyc_Lex_Ldecls*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;_Tmp0;})(((struct Cyc_Binding_NSCtxt*)_check_null(Cyc_Lex_lstate)->v)->ns_data,((struct Cyc_Binding_NSCtxt*)Cyc_Lex_lstate->v)->curr_ns);
# 600
({void(*_Tmp0)(void(*)(struct _fat_ptr*),struct Cyc_Set_Set*)=(void(*)(void(*)(struct _fat_ptr*),struct Cyc_Set_Set*))Cyc_Set_iter;_Tmp0;})(Cyc_Lex_insert_typedef,ts->typedefs);}}
# 602
void Cyc_Lex_leave_namespace (void){
Cyc_Binding_leave_ns((struct Cyc_Binding_NSCtxt*)_check_null(Cyc_Lex_lstate)->v);
Cyc_Lex_recompute_typedefs();}
# 606
void Cyc_Lex_enter_using(struct _tuple0*q){
struct Cyc_List_List*ns=Cyc_Binding_enter_using(0U,(struct Cyc_Binding_NSCtxt*)_check_null(Cyc_Lex_lstate)->v,q);
struct Cyc_Lex_Ldecls*ts=({struct Cyc_Lex_Ldecls*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;_Tmp0;})(((struct Cyc_Binding_NSCtxt*)_check_null(Cyc_Lex_lstate)->v)->ns_data,ns);
# 611
({void(*_Tmp0)(void(*)(struct _fat_ptr*),struct Cyc_Set_Set*)=(void(*)(void(*)(struct _fat_ptr*),struct Cyc_Set_Set*))Cyc_Set_iter;_Tmp0;})(Cyc_Lex_insert_typedef,ts->typedefs);}
# 613
void Cyc_Lex_leave_using (void){
Cyc_Binding_leave_using((struct Cyc_Binding_NSCtxt*)_check_null(Cyc_Lex_lstate)->v);
# 616
Cyc_Lex_recompute_typedefs();}
# 619
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 621
struct Cyc_Lex_Ldecls*ts=({struct Cyc_Lex_Ldecls*(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_lookup;_Tmp0;})(((struct Cyc_Binding_NSCtxt*)_check_null(Cyc_Lex_lstate)->v)->ns_data,((struct Cyc_Binding_NSCtxt*)Cyc_Lex_lstate->v)->curr_ns);
({struct Cyc_Set_Set*_Tmp0=({struct Cyc_Set_Set*(*_Tmp1)(struct Cyc_Set_Set*,struct _fat_ptr*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;_Tmp1;})(ts->typedefs,(*q).f1);ts->typedefs=_Tmp0;});
# 626
Cyc_Lex_insert_typedef((*q).f1);}
# 629
static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*lbuf){
int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);
# 633
if(symbol_num <= Cyc_Lex_num_kws){
# 635
if(!Cyc_Lex_in_extern_c ||(*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).is_c_keyword){
short res=(short)(*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).token_index;
return res;}}{
# 641
struct _fat_ptr*s=Cyc_Lex_get_symbol(symbol_num);
Cyc_Lex_token_string=*s;
if(Cyc_Lex_is_typedef(0,s))return 392;else{return 384;}}}
# 646
static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
if(Cyc_Lex_in_extern_c)
Cyc_Lex_err(_tag_fat("qualified identifiers not allowed in C code",sizeof(char),44U),lbuf);{
int i=lbuf->lex_start_pos;
int end=lbuf->lex_curr_pos;
struct _fat_ptr s=lbuf->lex_buffer;
# 653
struct _fat_ptr*v=0;
struct Cyc_List_List*rev_vs=0;
# 656
while(i < end){
{int start=i;
for(1;i < end &&(int)*((char*)_check_fat_subscript(s,sizeof(char),i))!=58;++ i){
;}
if(start==i)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad namespace",sizeof(char),14U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_impos2(_tag_fat(_Tmp1,sizeof(void*),1));});{
int vlen=i - start;
if(v!=0)
rev_vs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=v,_Tmp0->tl=rev_vs;_Tmp0;});
v=Cyc_Lex_get_symbol(Cyc_Lex_str_index(s,start,vlen));
i +=2;}}
# 657
1U;}
# 668
if(v==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad namespace",sizeof(char),14U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_impos2(_tag_fat(_Tmp1,sizeof(void*),1));});{
struct Cyc_List_List*vs=Cyc_List_imp_rev(rev_vs);
# 673
if(vs!=0 && Cyc_strcmp(*((struct _fat_ptr*)vs->hd),_tag_fat("Cyc",sizeof(char),4U))==0){
vs=vs->tl;
Cyc_Lex_token_qvar=({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Abs_n(vs,0);_Tmp0->f0=_Tmp1;}),_Tmp0->f1=v;_Tmp0;});}else{
if(vs!=0 && Cyc_strcmp(*((struct _fat_ptr*)vs->hd),_tag_fat("C",sizeof(char),2U))==0){
vs=vs->tl;
Cyc_Lex_token_qvar=({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Abs_n(vs,1);_Tmp0->f0=_Tmp1;}),_Tmp0->f1=v;_Tmp0;});}else{
# 680
Cyc_Lex_token_qvar=({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(vs);_Tmp0->f0=_Tmp1;}),_Tmp0->f1=v;_Tmp0;});}}
if(Cyc_Lex_is_typedef(vs,v))return 394;else{return 393;}}}}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned starting_line;struct _fat_ptr filename;struct _fat_ptr linenumpos;unsigned linenumpos_offset;};
# 697
static struct Cyc_Lex_PosInfo*Cyc_Lex_pos_info=0;
# 699
static int Cyc_Lex_linenumber=1;
# 702
static struct Cyc_Lex_PosInfo*Cyc_Lex_rnew_filepos(struct _RegionHandle*r,struct _fat_ptr filename,unsigned starting_line,struct Cyc_Lex_PosInfo*next){
# 706
struct _fat_ptr linenumpos=({unsigned _Tmp0=10U;_tag_fat(_region_calloc(Cyc_Core_heap_region,Cyc_Core_unique_qual,sizeof(unsigned),_Tmp0),sizeof(unsigned),_Tmp0);});
((unsigned*)linenumpos.curr)[0]=(unsigned)Cyc_yylloc.first_line;{
struct Cyc_Lex_PosInfo*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_Lex_PosInfo));_Tmp0->next=next,_Tmp0->starting_line=starting_line,_Tmp0->filename=(struct _fat_ptr)filename,_Tmp0->linenumpos=linenumpos,_Tmp0->linenumpos_offset=1U;return _Tmp0;}}
# 715
static void Cyc_Lex_inc_linenumber (void){
if(Cyc_Lex_pos_info==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("empty position info!",sizeof(char),21U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_Tmp2;})(_tag_fat(_Tmp1,sizeof(void*),1));});{
struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;
struct _fat_ptr linenums=_tag_fat(0,0,0);
({struct _fat_ptr _Tmp0=p->linenumpos;struct _fat_ptr _Tmp1=linenums;p->linenumpos=_Tmp1;linenums=_Tmp0;});{
unsigned offset=p->linenumpos_offset;
unsigned n=_get_fat_size(linenums,sizeof(unsigned));
# 724
if(offset >= n){
# 726
struct _fat_ptr newlinenums=({unsigned _Tmp0=n * 2U;_tag_fat(_region_calloc(Cyc_Core_heap_region,Cyc_Core_unique_qual,sizeof(unsigned),_Tmp0),sizeof(unsigned),_Tmp0);});
{unsigned i=0U;for(0;i < n;++ i){
*((unsigned*)_check_fat_subscript(newlinenums,sizeof(unsigned),(int)i))=((unsigned*)linenums.curr)[(int)i];}}
({struct _fat_ptr _Tmp0=linenums;struct _fat_ptr _Tmp1=newlinenums;linenums=_Tmp1;newlinenums=_Tmp0;});
# 731
({void(*_Tmp0)(unsigned*)=({void(*_Tmp1)(unsigned*)=(void(*)(unsigned*))Cyc_Core_ufree;_Tmp1;});_Tmp0((unsigned*)_untag_fat_ptr_check_bound(newlinenums,sizeof(unsigned),1U));});}
# 734
*((unsigned*)_check_fat_subscript(linenums,sizeof(unsigned),(int)offset))=(unsigned)Cyc_yylloc.first_line;
p->linenumpos_offset=offset + 1U;
++ Cyc_Lex_linenumber;
({struct _fat_ptr _Tmp0=p->linenumpos;struct _fat_ptr _Tmp1=linenums;p->linenumpos=_Tmp1;linenums=_Tmp0;});}}}
# 740
static void Cyc_Lex_process_directive(struct _fat_ptr line){
int i;
char buf[100U];
struct _fat_ptr filename=_tag_fat("",sizeof(char),1U);
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _Tmp0=({struct Cyc_IntPtr_sa_ScanfArg_struct _Tmp1;_Tmp1.tag=2,_Tmp1.f1=& i;_Tmp1;});struct Cyc_CharPtr_sa_ScanfArg_struct _Tmp1=({struct Cyc_CharPtr_sa_ScanfArg_struct _Tmp2;_Tmp2.tag=7,_Tmp2.f1=_tag_fat(buf,sizeof(char),100U);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_sscanf(line,_tag_fat("# %d \"%s",sizeof(char),9U),_tag_fat(_Tmp2,sizeof(void*),2));})==2){
if(Cyc_Flags_compile_for_boot){
# 748
int last_slash=-1;
{int i=(int)(Cyc_strlen(_tag_fat(buf,sizeof(char),100U))- 1U);for(0;i >= 0;-- i){
if((int)buf[i]==47){
last_slash=i;
break;}}}
# 754
filename=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)_fat_ptr_plus(_fat_ptr_plus(_tag_fat(buf,sizeof(char),100U),sizeof(char),last_slash),sizeof(char),1);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("\"%s",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});}else{
# 756
filename=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat(buf,sizeof(char),100U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("\"%s",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});}
if((Cyc_Lex_linenumber==i && Cyc_Lex_pos_info!=0)&&
 Cyc_strcmp(Cyc_Lex_pos_info->filename,filename)==0)
return;
Cyc_Lex_linenumber=i;}else{
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _Tmp0=({struct Cyc_IntPtr_sa_ScanfArg_struct _Tmp1;_Tmp1.tag=2,_Tmp1.f1=& i;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_sscanf(line,_tag_fat("# %d",sizeof(char),5U),_tag_fat(_Tmp1,sizeof(void*),1));})==1){
if(Cyc_Lex_linenumber==i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info!=0)filename=Cyc_Lex_pos_info->filename;}else{
# 766
++ Cyc_Lex_linenumber;
return;}}
# 769
Cyc_Lex_pos_info=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned)Cyc_Lex_linenumber,Cyc_Lex_pos_info);}struct _tuple32{struct _fat_ptr f0;unsigned f1;};
# 774
struct _tuple32 Cyc_Lex_xlate_pos(unsigned char_offset){
{struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;for(0;p!=0;p=p->next){
unsigned first_char_offset=*((unsigned*)_check_fat_subscript(p->linenumpos,sizeof(unsigned),0));
if(char_offset < first_char_offset && p->next!=0)continue;{
# 780
unsigned base=0U;
unsigned size=p->linenumpos_offset;
while(size > 1U){
{int half=(int)(size / 2U);
int mid=(int)(base + (unsigned)half);
if(char_offset > *((unsigned*)_check_fat_subscript(p->linenumpos,sizeof(unsigned),mid))){
base=base + (unsigned)half;
size=size - (unsigned)half;}else{
# 789
size=(unsigned)half;}}
# 783
1U;}{
# 792
struct _tuple32 _Tmp0;_Tmp0.f0=p->filename,_Tmp0.f1=p->starting_line + base;return _Tmp0;}}}}{
# 794
struct _tuple32 _Tmp0;_Tmp0.f0=_tag_fat(0,0,0),_Tmp0.f1=0U;return _Tmp0;}}
# 798
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
# 808
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){
# 810
int ans=Cyc_Lex_token(lbuf);
({int _Tmp0=({int _Tmp1=Cyc_Lexing_lexeme_start(lbuf);yyllocptr->first_line=_Tmp1;});Cyc_yylloc.first_line=_Tmp0;});
({int _Tmp0=({int _Tmp1=Cyc_Lexing_lexeme_end(lbuf);yyllocptr->last_line=_Tmp1;});Cyc_yylloc.last_line=_Tmp0;});
switch((int)ans){case 395:
 goto _LL4;case 391: _LL4:
 goto _LL6;case 389: _LL6:
 goto _LL8;case 390: _LL8:
 goto _LLA;case 386: _LLA:
 goto _LLC;case 387: _LLC:
 goto _LLE;case 384: _LLE:
 goto _LL10;case 392: _LL10:
({union Cyc_YYSTYPE _Tmp0=({union Cyc_YYSTYPE _Tmp1;_Tmp1.String_tok.tag=3U,_Tmp1.String_tok.val=Cyc_Lex_token_string;_Tmp1;});*yylval=_Tmp0;});goto _LL0;case 393:
 goto _LL14;case 394: _LL14:
({union Cyc_YYSTYPE _Tmp0=({union Cyc_YYSTYPE _Tmp1;_Tmp1.QualId_tok.tag=4U,_Tmp1.QualId_tok.val=Cyc_Lex_token_qvar;_Tmp1;});*yylval=_Tmp0;});goto _LL0;case 385:
({union Cyc_YYSTYPE _Tmp0=({union Cyc_YYSTYPE _Tmp1;_Tmp1.Int_tok.tag=1U,_Tmp1.Int_tok.val=Cyc_Lex_token_int;_Tmp1;});*yylval=_Tmp0;});goto _LL0;case 388:
({union Cyc_YYSTYPE _Tmp0=({union Cyc_YYSTYPE _Tmp1;_Tmp1.Char_tok.tag=2U,_Tmp1.Char_tok.val=Cyc_Lex_token_char;_Tmp1;});*yylval=_Tmp0;});goto _LL0;default:
 goto _LL0;}_LL0:;
# 828
return ans;}
# 835
const int Cyc_Lex_lex_base[200U]={0,113,119,120,125,126,127,131,- 6,4,12,2,- 3,- 1,- 2,115,- 4,121,- 1,131,- 5,209,217,240,272,132,- 4,- 3,- 2,5,2,133,- 17,138,- 1,351,- 18,6,- 12,- 11,280,- 13,- 10,- 7,- 8,- 9,424,447,295,- 14,154,- 17,7,- 1,- 15,- 16,8,- 1,502,303,575,650,367,- 16,- 58,178,- 38,9,2,- 40,137,30,107,117,32,115,101,377,150,727,770,135,138,98,141,823,898,987,1045,100,1120,1178,110,- 57,- 22,- 28,1253,1328,- 23,1403,- 41,- 42,1478,117,1536,1611,1686,119,1761,1836,1911,130,- 26,134,- 31,- 25,- 34,146,- 35,1986,2015,518,500,130,132,131,737,2025,2055,2089,2129,502,144,2199,2237,724,145,146,158,168,194,725,204,205,219,726,283,284,- 7,278,- 39,204,- 20,- 38,10,169,2169,- 33,- 32,- 15,- 19,1,2277,2,180,479,187,188,192,193,196,197,198,200,203,2350,2434,- 55,- 49,- 48,- 47,- 46,- 45,- 44,- 43,- 50,- 53,- 54,809,211,- 51,- 52,- 56,- 27,- 24,- 21,355,- 36,11,426};
const int Cyc_Lex_lex_backtrk[200U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,5,3,4,- 1,- 1,- 1,2,- 1,2,- 1,5,- 1,2,- 1,2,2,2,- 1,- 1,- 1,1,3,15,- 1,15,- 1,18,- 1,1,- 1,- 1,13,- 1,- 1,- 1,- 1,- 1,- 1,14,13,- 1,15,- 1,1,- 1,- 1,- 1,14,- 1,17,12,- 1,13,12,- 1,- 1,36,- 1,37,57,- 1,57,57,57,57,57,57,57,57,57,6,8,57,57,57,57,57,0,0,57,57,57,57,57,- 1,- 1,- 1,2,0,- 1,3,- 1,- 1,0,- 1,- 1,1,1,- 1,4,- 1,4,29,- 1,28,- 1,- 1,- 1,- 1,- 1,9,8,- 1,8,8,8,8,- 1,9,9,6,7,6,6,- 1,5,5,5,5,5,6,6,7,7,7,7,6,6,6,- 1,6,- 1,- 1,- 1,- 1,37,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,35,36};
const int Cyc_Lex_lex_default[200U]={64,54,26,31,26,15,7,7,0,- 1,- 1,- 1,0,0,0,25,0,25,0,- 1,0,- 1,- 1,- 1,- 1,25,0,0,0,- 1,- 1,50,0,50,0,- 1,0,- 1,0,0,- 1,0,0,0,0,0,- 1,- 1,- 1,0,50,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,- 1,0,- 1,- 1,0,196,- 1,- 1,161,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,151,0,0,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,196,0,- 1,- 1};
const int Cyc_Lex_lex_trans[2691U]={0,0,0,0,0,0,0,0,0,65,66,65,65,67,8,14,14,14,63,153,153,197,0,0,0,0,0,0,0,0,0,0,65,68,69,70,13,71,72,73,192,191,74,75,13,76,77,78,79,80,80,80,80,80,80,80,80,80,81,14,82,83,84,38,85,86,86,86,86,86,86,86,86,86,86,86,87,86,86,86,86,86,86,86,86,86,86,86,86,86,86,195,88,160,89,90,91,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,55,92,- 1,56,- 1,27,28,32,26,52,33,27,28,16,8,29,17,9,- 1,- 1,- 1,- 1,193,63,197,51,- 1,198,27,57,53,34,- 1,- 1,27,159,39,18,98,36,158,- 1,14,- 1,- 1,194,10,14,94,- 1,- 1,11,104,51,104,- 1,21,21,21,21,21,21,21,21,199,- 1,199,199,112,150,116,14,115,117,151,113,41,114,30,49,111,118,58,124,- 1,45,162,199,152,35,- 1,153,157,45,154,19,190,139,137,8,14,- 1,- 1,186,185,14,14,- 1,184,183,14,95,182,181,180,124,179,45,14,178,8,43,14,- 1,14,45,14,187,22,139,137,8,0,93,24,24,24,24,24,24,24,24,23,23,23,23,23,23,23,23,23,23,8,43,0,0,43,143,44,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,0,0,0,0,0,0,44,23,23,23,23,23,23,43,143,44,23,23,23,23,23,23,12,12,12,12,12,12,12,12,48,48,48,48,48,48,48,48,44,23,23,23,23,23,23,49,49,49,49,49,49,49,49,62,62,62,62,62,62,62,62,147,148,14,0,148,37,197,0,0,198,51,0,- 1,0,- 1,0,- 1,36,- 1,0,0,0,- 1,20,12,0,38,0,- 1,- 1,- 1,39,147,148,- 1,- 1,148,0,0,0,40,40,40,40,40,40,40,40,0,0,0,- 1,0,0,0,41,41,41,41,41,41,41,41,41,155,0,156,156,156,156,156,156,156,156,156,156,199,0,199,199,0,0,0,0,42,0,0,0,0,12,26,0,0,0,20,0,0,0,0,199,0,- 1,8,0,0,0,43,0,44,0,45,0,46,47,47,47,47,47,47,47,47,47,47,0,0,0,0,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,0,0,0,0,47,47,47,47,47,47,187,0,0,47,47,47,47,47,47,188,188,188,188,188,188,188,188,0,39,0,0,0,0,42,0,0,47,47,47,47,47,47,59,59,59,59,59,59,59,59,0,0,0,126,0,126,0,38,127,127,127,127,127,127,127,127,127,127,125,0,140,0,0,0,0,0,0,45,0,43,0,0,0,0,0,0,45,0,0,0,0,14,12,0,0,0,26,0,0,0,125,0,140,- 1,20,0,0,0,8,45,43,43,44,0,60,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,119,0,129,129,129,129,129,129,129,129,130,130,127,127,127,127,127,127,127,127,127,127,0,121,42,0,0,138,144,149,131,0,0,0,0,0,8,44,148,132,0,0,133,119,0,120,120,120,120,120,120,120,120,120,120,121,42,0,0,138,144,149,131,0,0,0,121,42,8,44,148,132,0,122,133,187,0,0,0,0,0,0,123,0,189,189,189,189,189,189,189,189,0,0,0,0,0,0,121,42,0,0,0,0,0,122,0,0,0,0,0,0,0,0,123,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,109,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,102,102,102,102,102,102,102,102,102,102,103,0,0,0,0,0,0,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,0,0,0,0,102,0,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,100,0,0,0,0,101,0,0,0,0,0,0,0,0,102,102,102,102,102,102,102,102,102,102,103,0,0,0,0,0,0,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,0,0,0,0,102,0,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,0,0,0,0,0,0,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,97,97,97,97,97,97,97,97,97,97,0,0,0,0,0,0,0,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,0,0,0,0,97,0,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,0,0,0,0,96,0,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,0,0,0,0,0,0,0,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,0,0,0,0,96,0,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,97,97,97,97,97,97,97,97,97,97,0,0,0,0,0,0,0,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,0,0,0,0,97,0,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,99,99,99,99,99,99,99,99,99,99,0,0,0,0,0,0,0,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,0,0,0,0,99,0,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,102,102,102,102,102,102,102,102,102,102,103,0,0,0,0,0,0,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,0,0,0,0,102,0,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,0,0,0,0,106,0,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,107,0,0,0,0,0,0,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,0,0,0,0,105,0,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,106,106,106,106,106,106,106,106,106,106,0,0,0,0,0,0,0,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,0,0,0,0,106,0,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,108,108,108,108,108,108,108,108,108,108,0,0,0,0,0,0,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,0,0,0,0,108,0,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,110,110,110,110,110,110,110,110,110,110,0,0,0,0,0,0,0,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,0,0,0,0,110,0,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,0,0,0,0,0,0,0,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,0,0,0,0,110,0,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,128,128,128,128,128,128,128,128,128,128,0,0,0,0,0,0,0,0,0,0,0,121,42,0,0,0,0,119,42,120,120,120,120,120,120,120,120,120,120,127,127,127,127,127,127,127,127,127,127,0,121,42,0,121,42,0,0,122,0,0,42,42,0,0,0,0,123,42,0,128,128,128,128,128,128,128,128,128,128,0,0,0,121,42,0,0,0,0,0,122,121,42,0,42,0,0,0,42,123,42,0,119,0,129,129,129,129,129,129,129,129,130,130,0,0,0,0,0,0,0,0,0,121,42,121,42,0,0,0,42,0,145,0,0,0,0,0,0,0,0,146,119,0,130,130,130,130,130,130,130,130,130,130,0,0,0,121,42,0,0,0,0,0,145,121,42,0,0,0,0,0,141,146,0,0,0,0,0,0,0,142,0,0,156,156,156,156,156,156,156,156,156,156,0,0,0,121,42,0,0,0,0,0,141,121,42,0,0,0,0,0,42,142,134,134,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,134,134,134,134,134,134,121,42,0,0,0,0,0,42,0,0,0,0,0,0,0,134,134,134,134,134,134,134,134,134,134,0,134,134,134,134,134,134,134,134,134,134,134,134,0,0,0,163,0,135,0,0,164,0,0,0,0,0,136,0,0,165,165,165,165,165,165,165,165,0,134,134,134,134,134,134,166,0,0,0,0,135,0,0,0,0,0,0,0,0,136,0,0,0,0,0,0,0,0,0,0,0,0,0,0,167,0,0,0,0,168,169,0,0,0,170,0,0,0,0,0,0,0,171,0,0,0,172,0,173,0,174,0,175,176,176,176,176,176,176,176,176,176,176,0,0,0,0,0,0,0,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,0,0,0,0,0,0,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,177,0,0,0,0,0,0,0,0,176,176,176,176,176,176,176,176,176,176,0,0,0,0,0,0,0,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,0,0,0,0,0,0,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[2691U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,9,29,37,52,56,67,154,198,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,30,0,0,0,161,163,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,68,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,71,0,74,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,15,1,73,2,2,3,17,2,3,4,4,5,6,4,5,6,7,25,31,7,72,76,70,33,15,70,2,1,2,3,17,73,4,75,83,5,89,76,76,50,19,25,31,72,6,19,92,33,7,6,103,75,107,7,19,19,19,19,19,19,19,19,65,50,65,65,111,78,81,19,113,81,78,82,82,82,4,84,84,117,1,123,15,124,73,65,78,3,17,151,155,125,151,5,164,132,136,137,19,25,31,166,167,19,19,33,168,169,19,92,170,171,172,123,173,124,19,174,138,139,19,50,19,125,19,189,19,132,136,137,- 1,0,21,21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,22,22,138,139,- 1,- 1,140,142,143,22,22,22,22,22,22,23,23,23,23,23,23,23,23,23,23,- 1,- 1,- 1,- 1,- 1,- 1,144,23,23,23,23,23,23,140,142,143,22,22,22,22,22,22,24,24,24,24,24,24,24,24,40,40,40,40,40,40,40,40,144,23,23,23,23,23,23,48,48,48,48,48,48,48,48,59,59,59,59,59,59,59,59,146,147,35,- 1,149,35,196,- 1,- 1,196,1,- 1,15,- 1,73,- 1,2,3,17,- 1,- 1,- 1,4,5,6,- 1,35,- 1,7,25,31,35,146,147,70,33,149,- 1,- 1,- 1,35,35,35,35,35,35,35,35,- 1,- 1,- 1,50,- 1,- 1,- 1,35,62,62,62,62,62,62,62,62,77,- 1,77,77,77,77,77,77,77,77,77,77,199,- 1,199,199,- 1,- 1,- 1,- 1,35,- 1,- 1,- 1,- 1,35,35,- 1,- 1,- 1,35,- 1,- 1,- 1,- 1,199,- 1,151,35,- 1,- 1,- 1,35,- 1,35,- 1,35,- 1,35,46,46,46,46,46,46,46,46,46,46,- 1,- 1,- 1,- 1,- 1,- 1,- 1,46,46,46,46,46,46,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,- 1,- 1,- 1,- 1,47,47,47,47,47,47,165,- 1,- 1,46,46,46,46,46,46,165,165,165,165,165,165,165,165,- 1,58,- 1,- 1,- 1,- 1,58,- 1,- 1,47,47,47,47,47,47,58,58,58,58,58,58,58,58,- 1,- 1,- 1,121,- 1,121,- 1,58,121,121,121,121,121,121,121,121,121,121,122,- 1,131,- 1,- 1,- 1,- 1,- 1,- 1,122,- 1,131,- 1,- 1,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,58,58,- 1,- 1,- 1,58,- 1,- 1,- 1,122,- 1,131,196,58,- 1,- 1,- 1,58,122,58,131,58,- 1,58,60,60,60,60,60,60,60,60,60,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,- 1,- 1,- 1,- 1,- 1,- 1,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,61,61,61,61,61,61,61,61,61,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,- 1,- 1,- 1,- 1,- 1,- 1,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,79,- 1,79,79,79,79,79,79,79,79,79,79,126,126,126,126,126,126,126,126,126,126,- 1,79,79,- 1,- 1,135,141,145,79,- 1,- 1,- 1,- 1,- 1,135,141,145,79,- 1,- 1,79,80,- 1,80,80,80,80,80,80,80,80,80,80,79,79,- 1,- 1,135,141,145,79,- 1,- 1,- 1,80,80,135,141,145,79,- 1,80,79,188,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,188,188,188,188,188,188,188,188,- 1,- 1,- 1,- 1,- 1,- 1,80,80,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,- 1,- 1,- 1,- 1,85,- 1,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,85,86,86,86,86,86,86,86,86,86,86,86,- 1,- 1,- 1,- 1,- 1,- 1,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,- 1,- 1,- 1,- 1,86,- 1,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,87,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,87,87,87,87,87,87,87,87,87,87,- 1,- 1,- 1,- 1,- 1,- 1,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,- 1,- 1,- 1,- 1,87,- 1,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,- 1,- 1,- 1,- 1,- 1,- 1,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,88,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,- 1,- 1,- 1,- 1,91,- 1,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,91,96,96,96,96,96,96,96,96,96,96,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,- 1,- 1,- 1,- 1,96,- 1,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,- 1,- 1,- 1,- 1,97,- 1,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,97,99,99,99,99,99,99,99,99,99,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,- 1,- 1,- 1,- 1,99,- 1,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,99,102,102,102,102,102,102,102,102,102,102,102,- 1,- 1,- 1,- 1,- 1,- 1,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,- 1,- 1,- 1,- 1,102,- 1,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,- 1,- 1,- 1,- 1,104,- 1,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,105,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,- 1,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,- 1,- 1,- 1,- 1,105,- 1,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,105,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,- 1,- 1,- 1,- 1,106,- 1,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,106,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,- 1,- 1,- 1,- 1,108,- 1,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,108,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,119,- 1,- 1,- 1,- 1,120,119,120,120,120,120,120,120,120,120,120,120,127,127,127,127,127,127,127,127,127,127,- 1,120,120,- 1,119,119,- 1,- 1,120,- 1,- 1,119,127,- 1,- 1,- 1,- 1,120,127,- 1,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,120,120,- 1,- 1,- 1,- 1,- 1,120,128,128,- 1,127,- 1,- 1,- 1,128,120,127,- 1,129,- 1,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,128,129,129,- 1,- 1,- 1,128,- 1,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,130,130,130,130,130,130,130,130,130,130,- 1,- 1,- 1,129,129,- 1,- 1,- 1,- 1,- 1,129,130,130,- 1,- 1,- 1,- 1,- 1,130,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,156,156,156,156,156,156,156,156,156,156,- 1,- 1,- 1,130,130,- 1,- 1,- 1,- 1,- 1,130,156,156,- 1,- 1,- 1,- 1,- 1,156,130,133,133,133,133,133,133,133,133,133,133,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,133,133,133,133,133,156,156,- 1,- 1,- 1,- 1,- 1,156,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,134,134,134,134,134,134,134,134,134,- 1,133,133,133,133,133,133,134,134,134,134,134,134,- 1,- 1,- 1,162,- 1,134,- 1,- 1,162,- 1,- 1,- 1,- 1,- 1,134,- 1,- 1,162,162,162,162,162,162,162,162,- 1,134,134,134,134,134,134,162,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,162,- 1,- 1,- 1,- 1,162,162,- 1,- 1,- 1,162,- 1,- 1,- 1,- 1,- 1,- 1,- 1,162,- 1,- 1,- 1,162,- 1,162,- 1,162,- 1,162,175,175,175,175,175,175,175,175,175,175,- 1,- 1,- 1,- 1,- 1,- 1,- 1,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,- 1,- 1,- 1,- 1,- 1,- 1,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,176,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,176,176,176,176,176,176,176,176,176,176,- 1,- 1,- 1,- 1,- 1,- 1,- 1,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,- 1,- 1,- 1,- 1,- 1,- 1,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 842
int state;int base;int backtrk;
int c;
state=start_state;
# 846
if(state >= 0){
({int _Tmp0=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_Tmp0;});
lbuf->lex_last_action=-1;}else{
# 850
state=(- state)- 1;}
# 852
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_base,200U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_backtrk,200U,sizeof(int),state));
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 860
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 864
c=256;}}else{
# 866
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c==-1)c=256;else{
if(c < 0)c=256 + c;}}
# 870
if(*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_check,2691U,sizeof(int),base + c))==state)
state=*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_trans,2691U,sizeof(int),base + c));else{
# 873
state=*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_default,200U,sizeof(int),state));}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action==-1)
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("empty token",sizeof(char),12U);_Tmp0;}));else{
# 879
return lbuf->lex_last_action;}}else{
# 882
if(c==256)lbuf->lex_eof_reached=0;}
# 853
1U;}}
# 886
int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 844 "lex.cyl"
 return(int)Cyc_Lex_process_id(lexbuf);case 1:
# 847 "lex.cyl"
 return(int)Cyc_Lex_process_qual_id(lexbuf);case 2:
# 850 "lex.cyl"
 Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));
return 391;case 3:
# 853
 Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));
return 395;case 4:
# 858 "lex.cyl"
 return Cyc_Lex_do_possible_qualifier(lexbuf);case 5:
# 861 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 385;case 6:
# 864
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 385;case 7:
# 870 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 385;case 8:
# 873
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 385;case 9:
# 878 "lex.cyl"
 Cyc_Lex_token_string=Cyc_Lexing_lexeme(lexbuf);
return 390;case 10:
# 881
 return 367;case 11:
# 882 "lex.cyl"
 return 368;case 12:
# 883 "lex.cyl"
 return 365;case 13:
# 884 "lex.cyl"
 return 366;case 14:
# 885 "lex.cyl"
 return 361;case 15:
# 886 "lex.cyl"
 return 362;case 16:
# 887 "lex.cyl"
 return 374;case 17:
# 888 "lex.cyl"
 return 375;case 18:
# 889 "lex.cyl"
 return 371;case 19:
# 890 "lex.cyl"
 return 372;case 20:
# 891 "lex.cyl"
 return 373;case 21:
# 892 "lex.cyl"
 return 380;case 22:
# 893 "lex.cyl"
 return 379;case 23:
# 894 "lex.cyl"
 return 378;case 24:
# 895 "lex.cyl"
 return 376;case 25:
# 896 "lex.cyl"
 return 377;case 26:
# 897 "lex.cyl"
 return 369;case 27:
# 898 "lex.cyl"
 return 370;case 28:
# 899 "lex.cyl"
 return 363;case 29:
# 901
 return 364;case 30:
# 902 "lex.cyl"
 return 382;case 31:
# 903 "lex.cyl"
 return 360;case 32:
# 904 "lex.cyl"
 return 381;case 33:
# 905 "lex.cyl"
 return 383;case 34:
# 906 "lex.cyl"
 return 320;case 35:
# 909 "lex.cyl"
 Cyc_Lex_process_directive(Cyc_Lexing_lexeme(lexbuf));return Cyc_Lex_token(lexbuf);case 36:
# 910 "lex.cyl"
 return Cyc_Lex_token(lexbuf);case 37:
# 911 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 38:
# 912 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
Cyc_Lex_comment(lexbuf);
return Cyc_Lex_token(lexbuf);case 39:
# 918 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_strng(lexbuf)){
1U;}
Cyc_Lex_token_string=Cyc_Lex_get_stored_string();
return 386;case 40:
# 926 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_wstrng(lexbuf)){
1U;}
Cyc_Lex_token_string=Cyc_Lex_get_stored_string();
return 387;case 41:
# 934 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_scan_charconst(lexbuf)){
1U;}
Cyc_Lex_token_string=Cyc_Lex_get_stored_string();
return 389;case 42:
# 941
 Cyc_Lex_token_char='\a';return 388;case 43:
# 942 "lex.cyl"
 Cyc_Lex_token_char='\b';return 388;case 44:
# 943 "lex.cyl"
 Cyc_Lex_token_char='\f';return 388;case 45:
# 944 "lex.cyl"
 Cyc_Lex_token_char='\n';return 388;case 46:
# 945 "lex.cyl"
 Cyc_Lex_token_char='\r';return 388;case 47:
# 946 "lex.cyl"
 Cyc_Lex_token_char='\t';return 388;case 48:
# 947 "lex.cyl"
 Cyc_Lex_token_char='\v';return 388;case 49:
# 948 "lex.cyl"
 Cyc_Lex_token_char='\\';return 388;case 50:
# 949 "lex.cyl"
 Cyc_Lex_token_char='\'';return 388;case 51:
# 950 "lex.cyl"
 Cyc_Lex_token_char='"';return 388;case 52:
# 951 "lex.cyl"
 Cyc_Lex_token_char='?';return 388;case 53:
# 954 "lex.cyl"
 Cyc_Lex_token_char=({union Cyc_Absyn_Cnst _Tmp0=Cyc_Lex_intconst(lexbuf,2,1,8);Cyc_Lex_cnst2char(_Tmp0,lexbuf);});
return 388;case 54:
# 959 "lex.cyl"
 Cyc_Lex_token_char=({union Cyc_Absyn_Cnst _Tmp0=Cyc_Lex_intconst(lexbuf,3,1,16);Cyc_Lex_cnst2char(_Tmp0,lexbuf);});
return 388;case 55:
# 963
 Cyc_Lex_token_char=Cyc_Lexing_lexeme_char(lexbuf,1);
return 388;case 56:
# 967
 return -1;case 57:
# 969
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default:
 lexbuf->refill_buff(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);};
# 973
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 975
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 972 "lex.cyl"
 return 0;case 1:
# 974
 Cyc_Lex_store_string_char('\a');return 1;case 2:
# 975 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3:
# 976 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4:
# 977 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5:
# 978 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6:
# 979 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7:
# 980 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8:
# 981 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9:
# 982 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10:
# 983 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11:
# 984 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12:
# 987 "lex.cyl"
 Cyc_Lex_store_string(Cyc_Lexing_lexeme(lexbuf));return 1;case 13:
# 990 "lex.cyl"
 Cyc_Lex_store_string(Cyc_Lexing_lexeme(lexbuf));return 1;case 14:
# 992
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15:
# 994
 Cyc_Lex_inc_linenumber();Cyc_Lex_runaway_err(_tag_fat("wide character ends in newline",sizeof(char),31U),lexbuf);return 0;case 16:
# 995 "lex.cyl"
 Cyc_Lex_runaway_err(_tag_fat("unterminated wide character",sizeof(char),28U),lexbuf);return 0;case 17:
# 996 "lex.cyl"
 Cyc_Lex_err(_tag_fat("bad character following backslash in wide character",sizeof(char),52U),lexbuf);return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);};
# 1000
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 1002
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,1);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 1001 "lex.cyl"
 return 1;case 1:
# 1002 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2:
# 1003 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3:
# 1005
 lexbuf->lex_curr_pos -=1;return 0;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);};
# 1009
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 1011
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,2);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 1009 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1:
# 1010 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2:
# 1011 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3:
# 1012 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4:
# 1013 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5:
# 1014 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('n');}else{
# 1018
Cyc_Lex_store_string_char('\n');}
return 1;case 6:
# 1021 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7:
# 1022 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('t');}else{
# 1026
Cyc_Lex_store_string_char('\t');}
return 1;case 8:
# 1028 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9:
# 1029 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10:
# 1030 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11:
# 1031 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12:
# 1032 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13:
# 1035 "lex.cyl"
 Cyc_Lex_store_string_char(({union Cyc_Absyn_Cnst _Tmp0=Cyc_Lex_intconst(lexbuf,1,0,8);Cyc_Lex_cnst2char(_Tmp0,lexbuf);}));
return 1;case 14:
# 1040 "lex.cyl"
 Cyc_Lex_store_string_char(({union Cyc_Absyn_Cnst _Tmp0=Cyc_Lex_intconst(lexbuf,2,0,16);Cyc_Lex_cnst2char(_Tmp0,lexbuf);}));
return 1;case 15:
# 1044
 Cyc_Lex_store_string(Cyc_Lexing_lexeme(lexbuf));
return 1;case 16:
# 1046 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_runaway_err(_tag_fat("string ends in newline",sizeof(char),23U),lexbuf);
return 0;case 17:
# 1050 "lex.cyl"
 Cyc_Lex_runaway_err(_tag_fat("unterminated string",sizeof(char),20U),lexbuf);
return 0;case 18:
# 1053 "lex.cyl"
 Cyc_Lex_err(_tag_fat("bad character following backslash in string",sizeof(char),44U),lexbuf);
return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);};
# 1058
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 1060
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,3);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 1063 "lex.cyl"
 Cyc_Lex_store_string(_tag_fat("\" L\"",sizeof(char),5U));return 1;case 1:
# 1064 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2:
# 1065 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3:
# 1067
 lexbuf->lex_curr_pos -=1;return 0;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);};
# 1071
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 1073
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,4);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 1070 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1:
# 1072
 Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,1));
return 1;case 2:
# 1078 "lex.cyl"
 Cyc_Lex_store_string(Cyc_Lexing_lexeme(lexbuf));
return 1;case 3:
# 1080 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_runaway_err(_tag_fat("string ends in newline",sizeof(char),23U),lexbuf);
return 0;case 4:
# 1083 "lex.cyl"
 Cyc_Lex_runaway_err(_tag_fat("unterminated string",sizeof(char),20U),lexbuf);
return 0;case 5:
# 1085 "lex.cyl"
 Cyc_Lex_err(_tag_fat("bad character following backslash in string",sizeof(char),44U),lexbuf);
return 1;default:
 lexbuf->refill_buff(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);};
# 1090
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 1092
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,5);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
switch((int)lexstate){case 0:
# 1088 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1:
# 1089 "lex.cyl"
 if(-- Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2:
# 1093 "lex.cyl"
 Cyc_Lex_runaway_err(_tag_fat("unterminated comment",sizeof(char),21U),lexbuf);
return 0;case 3:
# 1096 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4:
# 1097 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5:
# 1098 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6:
# 1099 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default:
 lexbuf->refill_buff(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);};
# 1103
_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=_tag_fat("some action didn't return!",sizeof(char),27U);_Tmp0;}));}
# 1105
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,6);}
# 1107 "lex.cyl"
void Cyc_Lex_pos_init (void){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}
# 1112
static struct Cyc_Xarray_Xarray*Cyc_Lex_empty_xarray(struct _RegionHandle*id_rgn,int dummy){
struct Cyc_Xarray_Xarray*symbols=({
struct Cyc_Xarray_Xarray*(*_Tmp0)(struct _RegionHandle*,int,struct _fat_ptr*)=({struct Cyc_Xarray_Xarray*(*_Tmp1)(struct _RegionHandle*,int,struct _fat_ptr*)=(struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _fat_ptr*))Cyc_Xarray_rcreate;_Tmp1;});struct _RegionHandle*_Tmp1=id_rgn;_Tmp0(_Tmp1,101,({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat("",sizeof(char),1U);_Tmp2;}));});
# 1116
({void(*_Tmp0)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*)=(void(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add;_Tmp0;})(symbols,& Cyc_Lex_bogus_string);
return symbols;}
# 1120
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1122
Cyc_Lex_in_extern_c=0;{
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_Tmp0=Cyc_Lex_prev_extern_c;struct Cyc_List_List*_Tmp1=x;Cyc_Lex_prev_extern_c=_Tmp1;x=_Tmp0;});
while(x!=0){
{struct Cyc_List_List*t=x->tl;
({void(*_Tmp0)(struct Cyc_List_List*)=(void(*)(struct Cyc_List_List*))Cyc_Core_ufree;_Tmp0;})(x);
x=t;}
# 1126
1U;}
# 1131
if(Cyc_Lex_ids_trie!=0){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_Tmp0=idt;struct Cyc_Lex_DynTrieSymbols*_Tmp1=Cyc_Lex_ids_trie;idt=_Tmp1;Cyc_Lex_ids_trie=_Tmp0;});{
struct Cyc_Lex_DynTrieSymbols _Tmp0=*idt;void*_Tmp1;_Tmp1=_Tmp0.dyn;{struct Cyc_Core_DynamicRegion*dyn=_Tmp1;
Cyc_Core_free_ukey(dyn);
({void(*_Tmp2)(struct Cyc_Lex_DynTrieSymbols*)=(void(*)(struct Cyc_Lex_DynTrieSymbols*))Cyc_Core_ufree;_Tmp2;})(idt);}}}
# 1138
if(Cyc_Lex_typedefs_trie!=0){
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_Tmp0=tdefs;struct Cyc_Lex_DynTrie*_Tmp1=Cyc_Lex_typedefs_trie;tdefs=_Tmp1;Cyc_Lex_typedefs_trie=_Tmp0;});{
struct Cyc_Lex_DynTrie _Tmp0=*tdefs;void*_Tmp1;_Tmp1=_Tmp0.dyn;{struct Cyc_Core_DynamicRegion*dyn=_Tmp1;
Cyc_Core_free_ukey(dyn);
({void(*_Tmp2)(struct Cyc_Lex_DynTrie*)=(void(*)(struct Cyc_Lex_DynTrie*))Cyc_Core_ufree;_Tmp2;})(tdefs);}}}{
# 1146
struct Cyc_Core_NewDynamicRegion _Tmp0=Cyc_Core__new_ukey(0U,"internal-error","internal-error",0);void*_Tmp1;_Tmp1=_Tmp0.key;{struct Cyc_Core_DynamicRegion*id_dyn=_Tmp1;
struct Cyc_Lex_Trie*ts=({struct Cyc_Lex_Trie*(*_Tmp2)(struct Cyc_Core_DynamicRegion*,int,struct Cyc_Lex_Trie*(*)(struct _RegionHandle*,int))=(struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*,int,struct Cyc_Lex_Trie*(*)(struct _RegionHandle*,int)))Cyc_Core_open_region;_Tmp2;})(id_dyn,0,Cyc_Lex_empty_trie);
struct Cyc_Xarray_Xarray*xa=({struct Cyc_Xarray_Xarray*(*_Tmp2)(struct Cyc_Core_DynamicRegion*,int,struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int))=(struct Cyc_Xarray_Xarray*(*)(struct Cyc_Core_DynamicRegion*,int,struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int)))Cyc_Core_open_region;_Tmp2;})(id_dyn,0,Cyc_Lex_empty_xarray);
Cyc_Lex_ids_trie=({struct Cyc_Lex_DynTrieSymbols*_Tmp2=_aqual_malloc(Cyc_Core_unique_qual,sizeof(struct Cyc_Lex_DynTrieSymbols));_Tmp2->dyn=id_dyn,_Tmp2->t=ts,_Tmp2->symbols=xa;_Tmp2;});{
# 1151
struct Cyc_Core_NewDynamicRegion _Tmp2=Cyc_Core__new_ukey(0U,"internal-error","internal-error",0);void*_Tmp3;_Tmp3=_Tmp2.key;{struct Cyc_Core_DynamicRegion*typedefs_dyn=_Tmp3;
struct Cyc_Lex_Trie*t=({struct Cyc_Lex_Trie*(*_Tmp4)(struct Cyc_Core_DynamicRegion*,int,struct Cyc_Lex_Trie*(*)(struct _RegionHandle*,int))=(struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*,int,struct Cyc_Lex_Trie*(*)(struct _RegionHandle*,int)))Cyc_Core_open_region;_Tmp4;})(typedefs_dyn,0,Cyc_Lex_empty_trie);
Cyc_Lex_typedefs_trie=({struct Cyc_Lex_DynTrie*_Tmp4=_aqual_malloc(Cyc_Core_unique_qual,sizeof(struct Cyc_Lex_DynTrie));_Tmp4->dyn=typedefs_dyn,_Tmp4->t=t;_Tmp4;});
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);
Cyc_Lex_kw_nums=({unsigned _Tmp4=(unsigned)Cyc_Lex_num_kws;_tag_fat(({struct Cyc_Lex_KeyWordInfo*_Tmp5=_cycalloc(_check_times(_Tmp4,sizeof(struct Cyc_Lex_KeyWordInfo)));({{unsigned _Tmp6=(unsigned)Cyc_Lex_num_kws;unsigned i;for(i=0;i < _Tmp6;++ i){_Tmp5[i].token_index=0,_Tmp5[i].is_c_keyword=0;}}0;});_Tmp5;}),sizeof(struct Cyc_Lex_KeyWordInfo),_Tmp4);});{
unsigned i=0U;
unsigned rwsze=97U;
{unsigned j=0U;for(0;j < rwsze;++ j){
if(include_cyclone_keywords ||(*((struct _tuple27*)_check_known_subscript_notnull(Cyc_Lex_rw_array,97U,sizeof(struct _tuple27),(int)j))).f2){
struct _fat_ptr str=(*((struct _tuple27*)_check_known_subscript_notnull(Cyc_Lex_rw_array,97U,sizeof(struct _tuple27),(int)j))).f0;
({struct _fat_ptr _Tmp4=str;Cyc_Lex_str_index(_Tmp4,0,(int)Cyc_strlen(str));});
({struct Cyc_Lex_KeyWordInfo _Tmp4=({struct Cyc_Lex_KeyWordInfo _Tmp5;_Tmp5.token_index=(int)Cyc_Lex_rw_array[(int)j].f1,_Tmp5.is_c_keyword=Cyc_Lex_rw_array[(int)j].f2;_Tmp5;});*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),(int)i))=_Tmp4;});
++ i;}}}
# 1165
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;}}}}}}}

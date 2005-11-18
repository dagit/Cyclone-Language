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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 78 "lexing.h"
extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 153
extern void*Cyc_List_fold_right(void*(*)(void*,void*),struct Cyc_List_List*,void*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 52
extern int Cyc_zstrcmp(struct _fat_ptr,struct _fat_ptr);
# 54
extern int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 60
extern struct _fat_ptr Cyc_strcat(struct _fat_ptr,struct _fat_ptr);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 71
extern struct _fat_ptr Cyc_strcpy(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 308
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 321
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 328
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;struct Cyc_Absyn_Exp*f9;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 542
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 706 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 874
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 900
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 908
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 914
void*Cyc_Absyn_compress(void*);
# 927
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 929
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 932
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 934
extern void*Cyc_Absyn_sint_type;
# 936
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 939
extern void*Cyc_Absyn_complex_type(void*);
# 941
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_shorthand_type;extern void*Cyc_Absyn_refcnt_rgn_shorthand_type;
# 943
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 947
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 949
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_tuple_type(struct Cyc_List_List*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 968
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 980
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 984
void*Cyc_Absyn_bounds_one (void);
# 986
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1006
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1009
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1013
void*Cyc_Absyn_aqualsof_type(void*);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tagof_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1071
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1085
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1089
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_false_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1100
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1105
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1116
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1119
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1122
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1126
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1131
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1133
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1142
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1149
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1152
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1157
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 30 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 40
extern int Cyc_Flags_no_register;
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 36
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 54 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_iko;
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
# 60
extern struct Cyc_Core_Opt Cyc_Kinds_aqko;
# 76 "kinds.h"
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 80
struct Cyc_Absyn_Kind*Cyc_Kinds_id_to_kind(struct _fat_ptr,unsigned);
# 89
void*Cyc_Kinds_compress_kb(void*);
# 101
void*Cyc_Kinds_consistent_aliashint(unsigned,void*,void*);
# 42 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 91
void*Cyc_Tcutil_copy_type(void*);
# 212
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 243
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 253
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 29 "attributes.h"
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Atts_Const_att_val;
# 46
void*Cyc_Atts_parse_nullary_att(unsigned,struct _fat_ptr);
void*Cyc_Atts_parse_unary_att(unsigned,struct _fat_ptr,unsigned,struct Cyc_Absyn_Exp*);
void*Cyc_Atts_parse_format_att(unsigned,unsigned,struct _fat_ptr,struct _fat_ptr,unsigned,unsigned);
# 68
int Cyc_Atts_fntype_att(void*);
# 29 "currgn.h"
extern struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 31
void*Cyc_CurRgn_curr_rgn_type (void);
# 67 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Complex_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Parse_Declarator f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct _tuple13{struct _tuple13*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
# 55 "parse.h"
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U,Cyc_Parse_None_sc =7U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 73 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*);
void Cyc_Lex_enter_namespace(struct _fat_ptr*);
void Cyc_Lex_leave_namespace (void);
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using (void);
void Cyc_Lex_enter_extern_c (void);
void Cyc_Lex_leave_extern_c (void);
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _fat_ptr Cyc_Lex_token_string;
# 97 "parse.y"
int Cyc_Parse_parsing_tempest=0;
# 103
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x==0)return x;{
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second!=0){
{struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 109
1U;}
# 114
return first;}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};
# 170
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 175
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 177
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=msg;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Parse_Exit_exn_struct));_Tmp0->tag=Cyc_Parse_Exit;_Tmp0;}));}
# 182
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 184
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;_Tmp1=tqt->f1;_Tmp0=tqt->f2;{struct Cyc_Absyn_Tqual tq=_Tmp1;void*t=_Tmp0;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc!=0U)loc=tq.loc;
Cyc_Warn_warn(loc,_tag_fat("qualifier on type is ignored",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}
# 189
return t;}}
# 192
static void*Cyc_Parse_make_pointer_mod(struct _RegionHandle*r,struct Cyc_Absyn_PtrLoc*loc,void*nullable,void*bound,void*eff,struct Cyc_List_List*pqs,struct Cyc_Absyn_Tqual tqs){
# 197
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*autoreleased=Cyc_Tcutil_any_bool(0);
void*aqual=0;
for(1;pqs!=0;pqs=pqs->tl){
void*_Tmp0=(void*)pqs->hd;void*_Tmp1;switch(*((int*)_Tmp0)){case 5:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 6:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 7:
 autoreleased=Cyc_Absyn_true_type;goto _LL0;case 9:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 8:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 4:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 3:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0: _Tmp1=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}case 10: _Tmp1=(void*)((struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{void*aq=_Tmp1;
aqual=aq;goto _LL0;}case 2: _Tmp1=((struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{struct Cyc_List_List*ts=_Tmp1;
eff=Cyc_Absyn_join_eff(ts);goto _LL0;}default: _Tmp1=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{void*t=_Tmp1;
({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("Found region pointer qual",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}}_LL0:;}
# 214
return(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_Tmp0->tag=2,_Tmp0->f1.eff=eff,_Tmp0->f1.nullable=nullable,_Tmp0->f1.bounds=bound,_Tmp0->f1.zero_term=zeroterm,_Tmp0->f1.ptrloc=loc,_Tmp0->f1.autoreleased=autoreleased,
# 216
aqual!=0?_Tmp0->f1.aqual=aqual:({void*_Tmp1=({void*_Tmp2=Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,0);Cyc_Absyn_aqual_var_type(_Tmp2,Cyc_Absyn_rtd_qual_type);});_Tmp0->f1.aqual=_Tmp1;}),_Tmp0->f2=tqs;_Tmp0;});}
# 221
static void Cyc_Parse_check_single_constraint(unsigned loc,struct _fat_ptr id){
if(Cyc_zstrcmp(id,_tag_fat("single",sizeof(char),7U)))
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat(" is not a valid effect constraint",sizeof(char),34U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err(loc,id,_tag_fat(_Tmp1,sizeof(void*),1));});}
# 225
static void*Cyc_Parse_effect_from_atomic(struct Cyc_List_List*effs){
if(Cyc_List_length(effs)==1)
return(void*)effs->hd;else{
# 229
return Cyc_Absyn_join_eff(effs);}}
# 232
static struct _tuple0*Cyc_Parse_gensym_enum (void){
static int enum_counter=0;
struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);_Tmp0->f0=_Tmp1;}),({
struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)enum_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("__anonymous_enum_%d__",sizeof(char),22U),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;});return _Tmp0;}struct _tuple15{unsigned f0;struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple17{struct _tuple15*f0;struct _tuple16*f1;};
# 238
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple17*field_info){
# 243
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;unsigned _Tmp7;_Tmp7=field_info->f0->f0;_Tmp6=field_info->f0->f1;_Tmp5=field_info->f0->f2;_Tmp4=field_info->f0->f3;_Tmp3=field_info->f0->f4;_Tmp2=field_info->f0->f5;_Tmp1=field_info->f1->f0;_Tmp0=field_info->f1->f1;{unsigned varloc=_Tmp7;struct _tuple0*qid=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;struct Cyc_List_List*tvs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;struct Cyc_Absyn_Exp*widthopt=_Tmp1;struct Cyc_Absyn_Exp*reqopt=_Tmp0;
if(tvs!=0)
Cyc_Warn_err(loc,_tag_fat("bad type params in struct field",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_Absyn_is_qvar_qualified(qid))
Cyc_Warn_err(loc,_tag_fat("struct or union field cannot be qualified with a namespace",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_Absyn_Aggrfield*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp8->name=(*qid).f1,_Tmp8->tq=tq,_Tmp8->type=t,_Tmp8->width=widthopt,_Tmp8->attributes=atts,_Tmp8->requires_clause=reqopt;return _Tmp8;}}}
# 253
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _Tmp0;_Tmp0.Signed_spec=0,_Tmp0.Unsigned_spec=0,_Tmp0.Short_spec=0,_Tmp0.Long_spec=0,_Tmp0.Long_Long_spec=0,_Tmp0.Complex_spec=0,_Tmp0.Valid_type_spec=0,_Tmp0.Type_spec=Cyc_Absyn_sint_type,_Tmp0.loc=loc;return _Tmp0;}
# 265
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 271
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Signed_spec=1;
return s;}
# 276
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Unsigned_spec=1;
return s;}
# 281
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Short_spec=1;
return s;}
# 286
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Long_spec=1;
return s;}
# 291
static struct Cyc_Parse_Type_specifier Cyc_Parse_complex_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Complex_spec=1;
return s;}
# 298
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 300
if(Cyc_Tcutil_is_array_type(t)){
void*_Tmp0=t;void*_Tmp1=argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_eko,0): Cyc_Absyn_heap_rgn_type;return Cyc_Tcutil_promote_array(_Tmp0,_Tmp1,Cyc_Absyn_al_qual_type,0);}else{return t;}}struct _tuple18{struct _fat_ptr*f0;void*f1;};
# 313 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
struct _tuple8*_Tmp0=(struct _tuple8*)x->hd;struct _fat_ptr _Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)((struct _tuple8*)_Tmp0)->f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->tl==0){_Tmp3=_Tmp0->f0;_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f2)->f2->hd;if((struct _fat_ptr*)_Tmp3!=0){struct _fat_ptr*v=_Tmp3;void*i=_Tmp2;
# 318
{void*_Tmp4;if(*((int*)i)==1){_Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)i)->f2;{void**z=(void**)_Tmp4;
# 322
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp5=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)*v;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));});*nm=_Tmp5;});
({void*_Tmp5=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp6->name=nm,_Tmp6->identity=-1,({void*_Tmp7=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp8->tag=0,_Tmp8->f1=& Cyc_Kinds_ik;_Tmp8;});_Tmp6->kind=_Tmp7;}),_Tmp6->aquals_bound=0;_Tmp6;}));*z=_Tmp5;});
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 327
res=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp5=({struct _tuple18*_Tmp6=_cycalloc(sizeof(struct _tuple18));_Tmp6->f0=v,_Tmp6->f1=i;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=res;_Tmp4;});goto _LL0;}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->hd)==1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->tl==0){_Tmp1=*_Tmp0->f0;_Tmp3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f2)->f2->hd)->f2;{struct _fat_ptr v=_Tmp1;void**z=(void**)_Tmp3;
# 331
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)v;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp6,sizeof(void*),1));});*nm=_Tmp4;});
({void*_Tmp4=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp5->name=nm,_Tmp5->identity=-1,({void*_Tmp6=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp7->tag=0,_Tmp7->f1=& Cyc_Kinds_ek;_Tmp7;});_Tmp5->kind=_Tmp6;}),_Tmp5->aquals_bound=0;_Tmp5;}));*z=_Tmp4;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{_LL5:
 goto _LL0;}}_LL0:;}
# 336
return res;}
# 340
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
void*_Tmp0=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->tl==0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*i=_Tmp1;
# 345
res=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp3=({struct _tuple18*_Tmp4=_cycalloc(sizeof(struct _tuple18));_Tmp4->f0=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_Tmp4->f1=i;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=res;_Tmp2;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 348
return res;}
# 352
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)==0){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f0.Rel_n.tag==1){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f0.Rel_n.val==0){_Tmp1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1->f1;{struct _fat_ptr*y=_Tmp1;
# 355
{struct Cyc_List_List*ts=tags;for(0;ts!=0;ts=ts->tl){
struct _tuple18*_Tmp2=(struct _tuple18*)ts->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct _fat_ptr*x=_Tmp4;void*i=_Tmp3;
if(Cyc_strptrcmp(x,y)==0){
void*_Tmp5=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp6->tag=38,({void*_Tmp7=Cyc_Tcutil_copy_type(i);_Tmp6->f1=_Tmp7;});_Tmp6;});return Cyc_Absyn_new_exp(_Tmp5,e->loc);}}}}
# 360
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 363
return e;}
# 368
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;switch(*((int*)t)){case 4: _Tmp9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;_Tmp6=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;struct Cyc_Absyn_Exp*nelts=_Tmp7;void*zt=_Tmp6;unsigned ztloc=_Tmp5;
# 371
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts!=0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*et2=Cyc_Parse_substitute_tags(tags,et);
if(nelts!=nelts2 || et!=et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3: _Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.eff;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.nullable;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.ptrloc;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.autoreleased;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.aqual;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;void*r=_Tmp7;void*n=_Tmp6;void*b=_Tmp4;void*zt=_Tmp3;struct Cyc_Absyn_PtrLoc*pl=_Tmp2;void*rel=_Tmp1;void*aq=_Tmp0;
# 379
void*et2=Cyc_Parse_substitute_tags(tags,et);
void*b2=Cyc_Parse_substitute_tags(tags,b);
if(et2!=et || b2!=b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _TmpA;_TmpA.elt_type=et2,_TmpA.elt_tq=tq,_TmpA.ptr_atts.eff=r,_TmpA.ptr_atts.nullable=n,_TmpA.ptr_atts.bounds=b2,_TmpA.ptr_atts.zero_term=zt,_TmpA.ptr_atts.ptrloc=pl,_TmpA.ptr_atts.autoreleased=rel,_TmpA.ptr_atts.aqual=aq;_TmpA;}));
goto _LL0;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==13){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2)->tl==0){_Tmp9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2->hd;{void*t=_Tmp9;
# 385
void*t2=Cyc_Parse_substitute_tags(tags,t);
if(t!=t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 8: _Tmp9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 389
struct Cyc_Absyn_Exp*e2=Cyc_Parse_substitute_tags_exp(tags,e);
if(e2!=e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9:
 goto _LL0;}_LL0:;}
# 394
return t;}
# 399
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_Tmp0=Cyc_Parse_substitute_tags(tags,x->type);x->type=_Tmp0;});}struct _tuple19{struct Cyc_Absyn_Tqual f0;void*f1;};struct _tuple20{void*f0;struct Cyc_Absyn_Tqual f1;void*f2;};
# 406
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple20*t){
struct _tuple19*_Tmp0=_cycalloc(sizeof(struct _tuple19));_Tmp0->f0=(*t).f1,_Tmp0->f1=(*t).f2;return _Tmp0;}
# 410
static int Cyc_Parse_is_typeparam(void*tm){
if(*((int*)tm)==4)
return 1;else{
return 0;};}
# 419
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k,void*aliashint,unsigned loc){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
return Cyc_Absyn_heap_rgn_type;
# 423
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
return Cyc_Absyn_unique_rgn_shorthand_type;
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
return Cyc_Absyn_refcnt_rgn_shorthand_type;
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
return Cyc_CurRgn_curr_rgn_type();
aliashint=Cyc_Kinds_consistent_aliashint(loc,k,aliashint);
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->name=_Tmp1;}),_Tmp0->identity=-1,_Tmp0->kind=k,_Tmp0->aquals_bound=aliashint;_Tmp0;}));}
# 433
static void*Cyc_Parse_id2aqual(unsigned loc,struct _fat_ptr s){
if(Cyc_strlen(s)==2U){
char _Tmp0=((const char*)s.curr)[1];switch((int)_Tmp0){case 65:
 return Cyc_Absyn_al_qual_type;case 85:
 return Cyc_Absyn_un_qual_type;case 84:
 return Cyc_Absyn_rtd_qual_type;default:
 goto _LL0;}_LL0:;}else{
# 442
if(Cyc_strlen(s)==3U){
if((int)((const char*)s.curr)[1]==82 &&(int)((const char*)s.curr)[2]==67)
return Cyc_Absyn_rc_qual_type;}}
# 446
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad aqual bound ",sizeof(char),17U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=s;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_al_qual_type;}
# 450
static struct Cyc_List_List*Cyc_Parse_insert_aqual(struct _RegionHandle*yy,struct Cyc_List_List*qlist,void*aq,unsigned loc){
{struct Cyc_List_List*l=qlist;for(0;l!=0;l=l->tl){
void*_Tmp0=(void*)l->hd;if(*((int*)_Tmp0)==10){
# 454
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Multiple alias qualifiers",sizeof(char),26U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
return qlist;}else{
# 457
goto _LL0;}_LL0:;}}{
# 460
struct Cyc_List_List*_Tmp0=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=(void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yy,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=10,_Tmp2->f1=aq;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=qlist;return _Tmp0;}}
# 463
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of heap region",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of unique region",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of refcounted region",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of \"current\" region",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));}
# 478
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_Tmp0;if(*((int*)t)==2){_Tmp0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*pr=_Tmp0;
return pr;}}else{
({int(*_Tmp1)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp1;})(loc,_tag_fat("expecting a list of type variables, not types",sizeof(char),46U));};}
# 486
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=(void**)&((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1->kind;{void**cptr=(void**)_Tmp1;
# 489
void*_Tmp2=Cyc_Kinds_compress_kb(*cptr);if(*((int*)_Tmp2)==1){
# 491
({void*_Tmp3=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp4->tag=2,_Tmp4->f1=0,_Tmp4->f2=k;_Tmp4;}): Cyc_Kinds_kind_to_bound(k);*cptr=_Tmp3;});return;}else{
return;};}}else{
# 494
return;};}
# 499
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 507
if(tms==0)return 0;{
# 509
void*_Tmp0=(void*)tms->hd;void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp1;
# 512
if(tms->tl==0 ||
 Cyc_Parse_is_typeparam((void*)tms->tl->hd)&& tms->tl->tl==0){
# 515
void*_Tmp2;if(*((int*)args)==1){
# 517
Cyc_Warn_warn(loc,
_tag_fat("function declaration with both new- and old-style parameter declarations; ignoring old-style",sizeof(char),93U),_tag_fat(0U,sizeof(void*),0));
return tms;}else{_Tmp2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f1;{struct Cyc_List_List*ids=_Tmp2;
# 521
if(({int _Tmp3=Cyc_List_length(ids);_Tmp3!=Cyc_List_length(tds);}))
({int(*_Tmp3)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp3;})(loc,
_tag_fat("wrong number of parameter declarations in old-style function declaration",sizeof(char),73U));{
# 525
struct Cyc_List_List*rev_new_params=0;
for(1;ids!=0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2!=0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)tds2->hd;
void*_Tmp3=x->r;void*_Tmp4;if(*((int*)_Tmp3)==0){_Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 532
if(Cyc_zstrptrcmp((*vd->name).f1,(struct _fat_ptr*)ids->hd)!=0)
continue;
if(vd->initializer!=0)
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("initializer found in parameter declaration",sizeof(char),43U));
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("namespaces forbidden in parameter declarations",sizeof(char),47U));
rev_new_params=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp6=({struct _tuple8*_Tmp7=_cycalloc(sizeof(struct _tuple8));_Tmp7->f0=(*vd->name).f1,_Tmp7->f1=vd->tq,_Tmp7->f2=vd->type;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=rev_new_params;_Tmp5;});
# 540
goto L;}}else{
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("nonvariable declaration in parameter type",sizeof(char),42U));};}
# 544
L: if(tds2==0)
({int(*_Tmp3)(unsigned,struct _fat_ptr)=({int(*_Tmp4)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp4;});unsigned _Tmp4=loc;_Tmp3(_Tmp4,Cyc_strconcat(*((struct _fat_ptr*)ids->hd),_tag_fat(" is not given a type",sizeof(char),21U)));});}{
# 547
struct Cyc_List_List*_Tmp3=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));
({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yy,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yy,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp7->tag=1,({struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(rev_new_params);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=0,_Tmp7->f3=0,_Tmp7->f4=0,_Tmp7->f5=0,_Tmp7->f6=0,_Tmp7->f7=0,_Tmp7->f8=0,_Tmp7->f9=0;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;return _Tmp3;}}}};}
# 554
goto _LL4;}}else{_LL4: {
struct Cyc_List_List*_Tmp2=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp3=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_Tmp2->tl=_Tmp3;});return _Tmp2;}};}}
# 562
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 566
if(tds!=0)
d=({struct Cyc_Parse_Declarator _Tmp0;_Tmp0.id=d.id,_Tmp0.varloc=d.varloc,({struct Cyc_List_List*_Tmp1=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_Tmp0.tms=_Tmp1;});_Tmp0;});{
# 569
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 575
if(dso!=0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 581
enum Cyc_Parse_Storage_class _Tmp0=dso->sc;switch((int)_Tmp0){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
 Cyc_Warn_err(loc,_tag_fat("bad storage class on function",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}}{
# 588
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f0;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_Absyn_Tqual fn_tqual=_Tmp4;void*fn_type=_Tmp3;struct Cyc_List_List*x=_Tmp2;struct Cyc_List_List*out_atts=_Tmp1;
# 593
if(x!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));{
# 596
struct Cyc_Absyn_FnInfo _Tmp5;if(*((int*)fn_type)==5){_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fn_type)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp5;
# 598
{struct Cyc_List_List*args2=i.args;for(0;args2!=0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f0==0){
Cyc_Warn_err(loc,_tag_fat("missing argument variable in function prototype",sizeof(char),48U),_tag_fat(0U,sizeof(void*),0));
({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=_tag_fat("?",sizeof(char),2U);_Tmp7;});(*((struct _tuple8*)args2->hd)).f0=_Tmp6;});}}}
# 605
({struct Cyc_List_List*_Tmp6=Cyc_List_append(i.attributes,out_atts);i.attributes=_Tmp6;});{
struct Cyc_Absyn_Fndecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_Tmp6->sc=sc,_Tmp6->is_inline=is_inline,_Tmp6->name=d.id,_Tmp6->body=body,_Tmp6->i=i,_Tmp6->cached_type=0,_Tmp6->param_vardecls=0,_Tmp6->fn_vardecl=0,_Tmp6->orig_scope=sc,_Tmp6->escapes=0;return _Tmp6;}}}else{
# 609
({int(*_Tmp6)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp6;})(loc,_tag_fat("declarator is not a function prototype",sizeof(char),39U));};}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 613
static struct _fat_ptr Cyc_Parse_msg1={_TmpG0,_TmpG0,_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 615
static struct _fat_ptr Cyc_Parse_msg2={_TmpG1,_TmpG1,_TmpG1 + 84U};
# 622
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 625
if(s1.Signed_spec && s2.Signed_spec)
Cyc_Warn_warn(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
Cyc_Warn_warn(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
Cyc_Warn_warn(loc,_tag_fat("too many occurrences of short in specifiers",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
s1.Short_spec |=s2.Short_spec;
if(s1.Complex_spec && s2.Complex_spec)
Cyc_Warn_warn(loc,_tag_fat("too many occurrences of _Complex or __complex__ in specifiers",sizeof(char),62U),_tag_fat(0U,sizeof(void*),0));
s1.Complex_spec |=s2.Complex_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
Cyc_Warn_warn(loc,_tag_fat("too many occurrences of long in specifiers",sizeof(char),43U),_tag_fat(0U,sizeof(void*),0));
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
Cyc_Warn_err(loc,Cyc_Parse_msg1,_tag_fat(0U,sizeof(void*),0));else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 650
return s1;}
# 656
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 664
if(seen_size || seen_sign){
if(ts.Signed_spec && ts.Unsigned_spec)
Cyc_Warn_err(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
Cyc_Warn_err(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 677
if(!seen_type){
if(!seen_sign && !seen_size){
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_double_type);
Cyc_Warn_warn(loc,_tag_fat("missing type within specifier",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));}
# 683
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_int_type(sgn,sz));else{
return Cyc_Absyn_int_type(sgn,sz);}}
# 687
{enum Cyc_Absyn_Size_of _Tmp0;enum Cyc_Absyn_Sign _Tmp1;if(*((int*)t)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 1: _Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_Tmp1;enum Cyc_Absyn_Size_of sz2=_Tmp0;
# 689
if(seen_sign &&(int)sgn2!=(int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 693
if(seen_size &&(int)sz2!=(int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;}case 2:
# 699
 if(seen_size)
t=Cyc_Absyn_long_double_type;
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;default: goto _LL5;}else{_LL5:
# 705
 if(seen_sign)
Cyc_Warn_err(loc,_tag_fat("sign specification on non-integral type",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
if(seen_size)
Cyc_Warn_err(loc,_tag_fat("size qualifier on non-integral type",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}_LL0:;}
# 711
return t;}
# 714
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 718
if(ds==0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*q=d.id;
unsigned varloc=d.varloc;
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f0;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_Absyn_Tqual tq2=_Tmp4;void*new_typ=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 725
struct Cyc_List_List*tl=ds->tl==0?0:({struct _RegionHandle*_Tmp5=r;struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=Cyc_Tcutil_copy_type(t);struct _tuple13*_Tmp8=ds->tl;Cyc_Parse_apply_tmss(_Tmp5,_Tmp6,_Tmp7,_Tmp8,shared_atts);});
struct Cyc_List_List*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_region_malloc(r,0U,sizeof(struct _tuple15));_Tmp7->f0=varloc,_Tmp7->f1=q,_Tmp7->f2=tq2,_Tmp7->f3=new_typ,_Tmp7->f4=tvs,_Tmp7->f5=atts;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=tl;return _Tmp5;}}}
# 729
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 732
if(tms==0){struct _tuple14 _Tmp0;_Tmp0.f0=tq,_Tmp0.f1=t,_Tmp0.f2=0,_Tmp0.f3=atts;return _Tmp0;}{
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;struct Cyc_Absyn_PtrAtts _Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*zeroterm=_Tmp5;unsigned ztloc=_Tmp4;
# 735
struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 735
struct Cyc_List_List*_Tmp8=atts;return Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);}case 1: _Tmp5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp5;void*zeroterm=_Tmp3;unsigned ztloc=_Tmp4;
# 738
struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 738
struct Cyc_List_List*_Tmp8=atts;return Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);}case 3: _Tmp5=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp5;
# 741
unsigned _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;int _TmpE;void*_TmpF;if(*((int*)args)==1){_TmpF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f1;_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f2;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f3;_TmpC=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f4;_TmpB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f5;_TmpA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f6;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f7;_Tmp8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f8;_Tmp7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f9;{struct Cyc_List_List*args2=_TmpF;int c_vararg=_TmpE;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpD;void*eff=_TmpC;struct Cyc_List_List*effc=_TmpB;struct Cyc_List_List*qb=_TmpA;struct Cyc_Absyn_Exp*req=_Tmp9;struct Cyc_Absyn_Exp*ens=_Tmp8;struct Cyc_Absyn_Exp*thrw=_Tmp7;
# 743
struct Cyc_List_List*typvars=0;
# 745
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as!=0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));_Tmp10->hd=(void*)as->hd,_Tmp10->tl=fn_atts;_Tmp10;});else{
# 750
new_atts=({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));_Tmp10->hd=(void*)as->hd,_Tmp10->tl=new_atts;_Tmp10;});}}}
# 752
if(tms->tl!=0){
void*_Tmp10=(void*)tms->tl->hd;void*_Tmp11;if(*((int*)_Tmp10)==4){_Tmp11=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp10)->f1;{struct Cyc_List_List*ts=_Tmp11;
# 755
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{
goto _LL12;}_LL12:;}
# 761
if(((((!c_vararg && cyc_vararg==0)&& args2!=0)&& args2->tl==0)&&(*((struct _tuple8*)args2->hd)).f0==0)&&(*((struct _tuple8*)args2->hd)).f2==Cyc_Absyn_void_type)
# 766
args2=0;{
# 771
struct Cyc_List_List*new_requires=0;
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_Tmp10=(struct _tuple8*)a->hd;void*_Tmp11;struct Cyc_Absyn_Tqual _Tmp12;void*_Tmp13;_Tmp13=_Tmp10->f0;_Tmp12=_Tmp10->f1;_Tmp11=(void**)& _Tmp10->f2;{struct _fat_ptr*vopt=_Tmp13;struct Cyc_Absyn_Tqual tq=_Tmp12;void**t=(void**)_Tmp11;
void*_Tmp14=*t;unsigned _Tmp15;void*_Tmp16;void*_Tmp17;struct Cyc_Absyn_Tqual _Tmp18;void*_Tmp19;if(*((int*)_Tmp14)==4){_Tmp19=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.elt_type;_Tmp18=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.tq;_Tmp17=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.num_elts;_Tmp16=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.zero_term;_Tmp15=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.zt_loc;{void*et=_Tmp19;struct Cyc_Absyn_Tqual tq=_Tmp18;struct Cyc_Absyn_Exp*neltsopt=_Tmp17;void*zt=_Tmp16;unsigned ztloc=_Tmp15;
# 776
if(neltsopt!=0 && vopt!=0){
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0)),v->f0.Loc_n.tag=4U,v->f0.Loc_n.val=0,v->f1=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,({struct Cyc_Absyn_Exp*_Tmp1A[1];({struct Cyc_Absyn_Exp*_Tmp1B=Cyc_Absyn_var_exp(v,0U);_Tmp1A[0]=_Tmp1B;});Cyc_List_list(_tag_fat(_Tmp1A,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);
new_requires=({struct Cyc_List_List*_Tmp1A=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1A->hd=new_req,_Tmp1A->tl=new_requires;_Tmp1A;});}}
# 783
goto _LL1A;}}else{
goto _LL1A;}_LL1A:;}}}
# 787
if(new_requires!=0){
struct Cyc_Absyn_Exp*r;
if(req!=0)
r=req;else{
# 792
r=(struct Cyc_Absyn_Exp*)new_requires->hd;
new_requires=new_requires->tl;}
# 795
for(1;new_requires!=0;new_requires=new_requires->tl){
r=Cyc_Absyn_and_exp(r,(struct Cyc_Absyn_Exp*)new_requires->hd,0U);}
req=r;}{
# 801
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 803
if(tags!=0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 808
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_Tmp10=(struct _tuple8*)a->hd;void*_Tmp11;struct Cyc_Absyn_Tqual _Tmp12;void*_Tmp13;_Tmp13=_Tmp10->f0;_Tmp12=_Tmp10->f1;_Tmp11=(void**)& _Tmp10->f2;{struct _fat_ptr*vopt=_Tmp13;struct Cyc_Absyn_Tqual tq=_Tmp12;void**t=(void**)_Tmp11;
if(tags!=0)
({void*_Tmp14=Cyc_Parse_substitute_tags(tags,*t);*t=_Tmp14;});
({void*_Tmp14=Cyc_Parse_array2ptr(*t,1);*t=_Tmp14;});}}}{
# 820
struct Cyc_Absyn_Tqual _Tmp10=Cyc_Absyn_empty_tqual(tq.loc);void*_Tmp11=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,effc,qb,fn_atts,req,ens,thrw);
# 820
struct Cyc_List_List*_Tmp12=new_atts;return Cyc_Parse_apply_tms(_Tmp10,_Tmp11,_Tmp12,tms->tl);}}}}}else{_Tmp6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f2;{unsigned loc=_Tmp6;
# 827
({int(*_Tmp10)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp10;})(loc,_tag_fat("function declaration without parameter types",sizeof(char),45U));}};}case 4: _Tmp5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;unsigned loc=_Tmp4;
# 834
if(tms->tl==0){
struct _tuple14 _Tmp6;_Tmp6.f0=tq,_Tmp6.f1=t,_Tmp6.f2=ts,_Tmp6.f3=atts;return _Tmp6;}
# 839
({int(*_Tmp6)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp6;})(loc,
_tag_fat("type parameters must appear before function arguments in declarator",sizeof(char),68U));}case 2: _Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 842
struct Cyc_Absyn_Tqual _Tmp6=tq2;void*_Tmp7=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp8;_Tmp8.elt_type=t,_Tmp8.elt_tq=tq,_Tmp8.ptr_atts=ptratts;_Tmp8;}));struct Cyc_List_List*_Tmp8=atts;return Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);}default: _Tmp4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{unsigned loc=_Tmp4;struct Cyc_List_List*atts2=_Tmp5;
# 847
struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=t;struct Cyc_List_List*_Tmp8=Cyc_List_append(atts,atts2);return Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);}};}}
# 853
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 862
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple15*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;unsigned _Tmp5;_Tmp5=t->f0;_Tmp4=t->f1;_Tmp3=t->f2;_Tmp2=t->f3;_Tmp1=t->f4;_Tmp0=t->f5;{unsigned varloc=_Tmp5;struct _tuple0*x=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*typ=_Tmp2;struct Cyc_List_List*tvs=_Tmp1;struct Cyc_List_List*atts=_Tmp0;
# 865
Cyc_Lex_register_typedef(x);{
# 867
struct Cyc_Core_Opt*kind;
void*type;
{void*_Tmp6;if(*((int*)typ)==1){_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)typ)->f1;{struct Cyc_Core_Opt*kopt=_Tmp6;
# 871
type=0;
kind=kopt==0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}{
# 876
void*_Tmp6=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_Tmp7->tag=8,({struct Cyc_Absyn_Typedefdecl*_Tmp8=({struct Cyc_Absyn_Typedefdecl*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));_Tmp9->name=x,_Tmp9->tvs=tvs,_Tmp9->kind=kind,_Tmp9->defn=type,_Tmp9->atts=atts,_Tmp9->tq=tq,_Tmp9->extern_c=0;_Tmp9;});_Tmp7->f1=_Tmp8;});_Tmp7;});return Cyc_Absyn_new_decl(_Tmp6,loc);}}}}
# 883
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});return Cyc_Absyn_new_stmt(_Tmp0,d->loc);}
# 887
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return({struct Cyc_Absyn_Stmt*(*_Tmp0)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right;_Tmp0;})(Cyc_Parse_flatten_decl,ds,s);}
# 891
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es,struct Cyc_List_List**rs){
# 895
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
struct Cyc_List_List*renames=0;
for(1;ds!=0;ds=ds->tl){
struct _tuple12 _Tmp0=ds->hd;void*_Tmp1;void*_Tmp2;struct Cyc_Parse_Declarator _Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_Parse_Declarator d=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;struct Cyc_Absyn_Exp*rename=_Tmp1;
declarators=({struct _tuple13*_Tmp4=_region_malloc(r,0U,sizeof(struct _tuple13));_Tmp4->tl=declarators,_Tmp4->hd=d;_Tmp4;});
exprs=({struct Cyc_List_List*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp4->hd=e,_Tmp4->tl=exprs;_Tmp4;});
renames=({struct Cyc_List_List*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp4->hd=rename,_Tmp4->tl=renames;_Tmp4;});}}
# 904
({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(exprs);*es=_Tmp0;});
({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(renames);*rs=_Tmp0;});
({struct _tuple13*_Tmp0=({struct _tuple13*(*_Tmp1)(struct _tuple13*)=(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;_Tmp1;})(declarators);*decls=_Tmp0;});}
# 914
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 917
struct _RegionHandle _Tmp0=_new_region(0U,"mkrgn");struct _RegionHandle*mkrgn=& _Tmp0;_push_region(mkrgn);
{void*_Tmp1;struct Cyc_Parse_Type_specifier _Tmp2;struct Cyc_Absyn_Tqual _Tmp3;_Tmp3=ds.tq;_Tmp2=ds.type_specs;_Tmp1=ds.attributes;{struct Cyc_Absyn_Tqual tq=_Tmp3;struct Cyc_Parse_Type_specifier tss=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
if(tq.loc==0U)tq.loc=tqual_loc;
if(ds.is_inline)
Cyc_Warn_warn(loc,_tag_fat("inline qualifier on non-function definition",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));{
# 923
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _Tmp4=ds.sc;switch((int)_Tmp4){case Cyc_Parse_Typedef_sc:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc:
 s=Cyc_Flags_no_register?2U: 5U;goto _LL3;case Cyc_Parse_Abstract_sc:
 s=1U;goto _LL3;default:
 goto _LL3;}_LL3:;}{
# 939
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
struct Cyc_List_List*renames=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs,& renames);{
# 944
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es!=0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd!=0){
exps_empty=0;
break;}}}{
# 952
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators==0){
# 956
int _Tmp4;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;void*_Tmp7;switch(*((int*)base_type)){case 9: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1)->r)){case 0: _Tmp7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
# 958
({struct Cyc_List_List*_Tmp8=Cyc_List_append(ad->attributes,atts);ad->attributes=_Tmp8;});
ad->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 1: _Tmp7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;
# 962
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on enum not supported",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
ed->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpB->tag=7,_TmpB->f1=ed;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: _Tmp7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;
# 966
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on datatypes not supported",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));
dd->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=dd;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)){case 24: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.tag==1){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f0;_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f1;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp6;struct _tuple0*n=_Tmp7;struct Cyc_List_List*ts=_Tmp5;
# 970
struct Cyc_List_List*ts2=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),ad->kind=k,ad->sc=s,ad->name=n,ad->tvs=ts2,ad->impl=0,ad->attributes=0,ad->expected_mem_kind=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on type declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{goto _LL25;}case 22: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.tag==2){_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl**tudp=_Tmp7;
# 975
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=*tudp;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.name;_Tmp4=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.is_extensible;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{struct _tuple0*n=_Tmp7;int isx=_Tmp4;struct Cyc_List_List*ts=_Tmp5;
# 978
struct Cyc_List_List*ts2=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),_Tmp8->hd=tud,_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 19: _Tmp7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct _tuple0*n=_Tmp7;
# 983
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,ed->name=n,ed->fields=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 20: _Tmp7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct Cyc_List_List*fs=_Tmp7;
# 989
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,({struct _tuple0*_Tmp8=Cyc_Parse_gensym_enum();ed->name=_Tmp8;}),({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=fs;_Tmp9;});ed->fields=_Tmp8;});
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: goto _LL25;}default: _LL25:
 Cyc_Warn_err(loc,_tag_fat("missing declarator",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));{struct Cyc_List_List*_Tmp8=0;_npop_handler(0);return _Tmp8;}};}{
# 996
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);
if(istypedef){
# 1001
if(!exps_empty)
Cyc_Warn_err(loc,_tag_fat("initializer in typedef declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*decls=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp4;})(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_Tmp4=decls;_npop_handler(0);return _Tmp4;}}{
# 1007
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*ds=fields;for(0;ds!=0;(
# 1010
ds=ds->tl,exprs=exprs->tl,renames=renames->tl)){
struct _tuple15*_Tmp4=(struct _tuple15*)ds->hd;void*_Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;unsigned _TmpA;_TmpA=_Tmp4->f0;_Tmp9=_Tmp4->f1;_Tmp8=_Tmp4->f2;_Tmp7=_Tmp4->f3;_Tmp6=_Tmp4->f4;_Tmp5=_Tmp4->f5;{unsigned varloc=_TmpA;struct _tuple0*x=_Tmp9;struct Cyc_Absyn_Tqual tq2=_Tmp8;void*t2=_Tmp7;struct Cyc_List_List*tvs2=_Tmp6;struct Cyc_List_List*atts2=_Tmp5;
if(tvs2!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(exprs==0)
({int(*_TmpB)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TmpB;})(loc,_tag_fat("unexpected NULL in parse!",sizeof(char),26U));
if(renames==0)
({int(*_TmpB)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TmpB;})(loc,_tag_fat("unexpected NULL in parse!",sizeof(char),26U));{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)exprs->hd,(struct Cyc_Absyn_Exp*)renames->hd);
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({struct Cyc_Absyn_Decl*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpF->tag=0,_TmpF->f1=vd;_TmpF;});_TmpD->r=_TmpE;}),_TmpD->loc=loc;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=decls;_TmpB;});}}}}{
# 1024
struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(decls);_npop_handler(0);return _Tmp4;}}}}}}}}}
# 918
;_pop_region();}
# 1028
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
long long _Tmp0;char _Tmp1;int _Tmp2;switch(x.LongLong_c.tag){case 5: _Tmp2=x.Int_c.val.f1;{int i=_Tmp2;
return(unsigned)i;}case 2: _Tmp1=x.Char_c.val.f1;{char c=_Tmp1;
return(unsigned)c;}case 6: _Tmp0=x.LongLong_c.val.f1;{long long x=_Tmp0;
# 1033
unsigned long long y=(unsigned long long)x;
if(y > 4294967295U)
Cyc_Warn_err(loc,_tag_fat("integer constant too large",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
return(unsigned)x;}default:
# 1038
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_Absynpp_cnst2string(x);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err(loc,_tag_fat("expected integer constant but found %s",sizeof(char),39U),_tag_fat(_Tmp4,sizeof(void*),1));});
return 0U;};}
# 1044
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 14: _Tmp6=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*x=_Tmp6;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 1048
struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_unknownid_exp(vd->name,p->loc);return Cyc_Absyn_deref_exp(_Tmp7,p->loc);}}else{goto _LL13;}case 5: _Tmp6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp6;
struct Cyc_Absyn_Exp*_Tmp7=Cyc_Parse_pat2exp(p2);return Cyc_Absyn_address_exp(_Tmp7,p->loc);}case 8:
 return Cyc_Absyn_null_exp(p->loc);case 9: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 10: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
return Cyc_Absyn_char_exp(c,p->loc);}case 11: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp4;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 15: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3==0){_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _tuple0*x=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 1055
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=({struct Cyc_List_List*(*_Tmp7)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;_Tmp7;})(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 16: _Tmp6=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
return e;}default: _LL13:
# 1060
 Cyc_Warn_err(p->loc,_tag_fat("cannot mix patterns and expressions in case",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_null_exp(p->loc);};}
# 1064
static struct _tuple16 Cyc_Parse_split_seq(struct Cyc_Absyn_Exp*maybe_seq){
void*_Tmp0=maybe_seq->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==9){_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1067
struct _tuple16 _Tmp3;_Tmp3.f0=e1,_Tmp3.f1=e2;return _Tmp3;}}else{
# 1069
struct _tuple16 _Tmp3;_Tmp3.f0=maybe_seq,_Tmp3.f1=0;return _Tmp3;};}
# 1072
static struct Cyc_Absyn_Exp*Cyc_Parse_join_assn(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
if(e1!=0 && e2!=0)return Cyc_Absyn_and_exp(e1,e2,0U);else{
if(e1!=0)return e1;else{
return e2;}}}struct _tuple21{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1077
static struct _tuple21 Cyc_Parse_join_assns(struct _tuple21 a1,struct _tuple21 a2){
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=a1.f0;_Tmp1=a1.f1;_Tmp0=a1.f2;{struct Cyc_Absyn_Exp*r1=_Tmp2;struct Cyc_Absyn_Exp*e1=_Tmp1;struct Cyc_Absyn_Exp*t1=_Tmp0;
void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=a2.f0;_Tmp4=a2.f1;_Tmp3=a2.f2;{struct Cyc_Absyn_Exp*r2=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;struct Cyc_Absyn_Exp*t2=_Tmp3;
struct Cyc_Absyn_Exp*r=Cyc_Parse_join_assn(r1,r2);
struct Cyc_Absyn_Exp*e=Cyc_Parse_join_assn(e1,e2);
struct Cyc_Absyn_Exp*t=Cyc_Parse_join_assn(t1,t2);
struct _tuple21 _Tmp6;_Tmp6.f0=r,_Tmp6.f1=e,_Tmp6.f2=t;return _Tmp6;}}}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f0;void*f1;void*f2;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f0;int f1;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f0;enum Cyc_Absyn_AggrKind f1;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _union_YYSTYPE_YY29{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f0;struct Cyc_Parse_Type_specifier f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY36{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f0;int f1;struct Cyc_Absyn_VarargInfo*f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _tuple28{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _union_YYSTYPE_YY51{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _tuple29{void*f0;void*f1;};struct _union_YYSTYPE_YY53{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _tuple30{struct Cyc_List_List*f0;unsigned f1;};struct _union_YYSTYPE_YY56{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _union_YYSTYPE_YY59{int tag;void*val;};struct _union_YYSTYPE_YY60{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY62{int tag;struct _tuple21 val;};struct _union_YYSTYPE_YY63{int tag;void*val;};struct _tuple31{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY64{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY65{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY67{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};struct _union_YYSTYPE_YY68{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY69{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YY67 YY67;struct _union_YYSTYPE_YY68 YY68;struct _union_YYSTYPE_YY69 YY69;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1169
static void Cyc_yythrowfail(struct _fat_ptr s){
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=s;_Tmp0;}));}static char _TmpG2[7U]="cnst_t";
# 1136 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2,_TmpG2,_TmpG2 + 7U};
union Cyc_Absyn_Cnst _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Int_tok.tag==1){_Tmp0=yy1->Int_tok.val;{union Cyc_Absyn_Cnst yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1143
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.Int_tok.tag=1U,_Tmp0.Int_tok.val=yy1;return _Tmp0;}static char _TmpG3[5U]="char";
# 1137 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3,_TmpG3,_TmpG3 + 5U};
char _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Char_tok.tag==2){_Tmp0=yy1->Char_tok.val;{char yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1144
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.Char_tok.tag=2U,_Tmp0.Char_tok.val=yy1;return _Tmp0;}static char _TmpG4[13U]="string_t<`H>";
# 1138 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4,_TmpG4,_TmpG4 + 13U};
struct _fat_ptr _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->String_tok.tag==3){_Tmp0=yy1->String_tok.val;{struct _fat_ptr yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1145
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.String_tok.tag=3U,_Tmp0.String_tok.val=yy1;return _Tmp0;}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1141 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG5,_TmpG5,_TmpG5 + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY1.tag==7){_Tmp0=yy1->YY1.val;{struct _tuple22*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1148
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY1.tag=7U,_Tmp0.YY1.val=yy1;return _Tmp0;}static char _TmpG6[11U]="ptrbound_t";
# 1142 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG6,_TmpG6,_TmpG6 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY2.tag==8){_Tmp0=yy1->YY2.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1149
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY2.tag=8U,_Tmp0.YY2.val=yy1;return _Tmp0;}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1143 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG7,_TmpG7,_TmpG7 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY3.tag==9){_Tmp0=yy1->YY3.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1150
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY3.tag=9U,_Tmp0.YY3.val=yy1;return _Tmp0;}static char _TmpG8[6U]="exp_t";
# 1144 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG8,_TmpG8,_TmpG8 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Exp_tok.tag==5){_Tmp0=yy1->Exp_tok.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1151
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.Exp_tok.tag=5U,_Tmp0.Exp_tok.val=yy1;return _Tmp0;}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG9,_TmpG9,_TmpG9 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY4.tag==10){_Tmp0=yy1->YY4.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1159
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY4.tag=10U,_Tmp0.YY4.val=yy1;return _Tmp0;}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1153 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGA,_TmpGA,_TmpGA + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY5.tag==11){_Tmp0=yy1->YY5.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1160
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY5.tag=11U,_Tmp0.YY5.val=yy1;return _Tmp0;}static char _TmpGB[9U]="primop_t";
# 1154 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGB,_TmpGB,_TmpGB + 9U};
enum Cyc_Absyn_Primop _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY6.tag==12){_Tmp0=yy1->YY6.val;{enum Cyc_Absyn_Primop yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1161
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY6.tag=12U,_Tmp0.YY6.val=yy1;return _Tmp0;}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1155 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGC,_TmpGC,_TmpGC + 19U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY7.tag==13){_Tmp0=yy1->YY7.val;{struct Cyc_Core_Opt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1162
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY7.tag=13U,_Tmp0.YY7.val=yy1;return _Tmp0;}static char _TmpGD[7U]="qvar_t";
# 1156 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGD,_TmpGD,_TmpGD + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->QualId_tok.tag==4){_Tmp0=yy1->QualId_tok.val;{struct _tuple0*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1163
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.QualId_tok.tag=4U,_Tmp0.QualId_tok.val=yy1;return _Tmp0;}static char _TmpGE[7U]="stmt_t";
# 1159 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGE,_TmpGE,_TmpGE + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Stmt_tok.tag==6){_Tmp0=yy1->Stmt_tok.val;{struct Cyc_Absyn_Stmt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1166
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.Stmt_tok.tag=6U,_Tmp0.Stmt_tok.val=yy1;return _Tmp0;}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1162 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGF,_TmpGF,_TmpGF + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY8.tag==14){_Tmp0=yy1->YY8.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1169
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY8.tag=14U,_Tmp0.YY8.val=yy1;return _Tmp0;}static char _TmpG10[6U]="pat_t";
# 1163 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG10,_TmpG10,_TmpG10 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY9.tag==15){_Tmp0=yy1->YY9.val;{struct Cyc_Absyn_Pat*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1170
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY9.tag=15U,_Tmp0.YY9.val=yy1;return _Tmp0;}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1168 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG11,_TmpG11,_TmpG11 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY10.tag==16){_Tmp0=yy1->YY10.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1175
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY10.tag=16U,_Tmp0.YY10.val=yy1;return _Tmp0;}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1169 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG12,_TmpG12,_TmpG12 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY11.tag==17){_Tmp0=yy1->YY11.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1176
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY11.tag=17U,_Tmp0.YY11.val=yy1;return _Tmp0;}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1170 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG13,_TmpG13,_TmpG13 + 36U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY12.tag==18){_Tmp0=yy1->YY12.val;{struct _tuple24*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1177
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY12.tag=18U,_Tmp0.YY12.val=yy1;return _Tmp0;}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1171 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG14,_TmpG14,_TmpG14 + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY13.tag==19){_Tmp0=yy1->YY13.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1178
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY13.tag=19U,_Tmp0.YY13.val=yy1;return _Tmp0;}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1172 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG15,_TmpG15,_TmpG15 + 58U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY14.tag==20){_Tmp0=yy1->YY14.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1179
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY14.tag=20U,_Tmp0.YY14.val=yy1;return _Tmp0;}static char _TmpG16[9U]="fndecl_t";
# 1173 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG16,_TmpG16,_TmpG16 + 9U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY15.tag==21){_Tmp0=yy1->YY15.val;{struct Cyc_Absyn_Fndecl*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1180
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY15.tag=21U,_Tmp0.YY15.val=yy1;return _Tmp0;}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1174 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG17,_TmpG17,_TmpG17 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY16.tag==22){_Tmp0=yy1->YY16.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1181
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY16.tag=22U,_Tmp0.YY16.val=yy1;return _Tmp0;}static char _TmpG18[12U]="decl_spec_t";
# 1177 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG18,_TmpG18,_TmpG18 + 12U};
struct Cyc_Parse_Declaration_spec _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY17.tag==23){_Tmp0=yy1->YY17.val;{struct Cyc_Parse_Declaration_spec yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1184
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY17.tag=23U,_Tmp0.YY17.val=yy1;return _Tmp0;}static char _TmpG19[41U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)";
# 1178 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG19,_TmpG19,_TmpG19 + 41U};
struct _tuple12 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY18.tag==24){_Tmp0=yy1->YY18.val;{struct _tuple12 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1185
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY18.tag=24U,_Tmp0.YY18.val=yy1;return _Tmp0;}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1179 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1A,_TmpG1A,_TmpG1A + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY19.tag==25){_Tmp0=yy1->YY19.val;{struct _tuple11*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1186
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY19.tag=25U,_Tmp0.YY19.val=yy1;return _Tmp0;}static char _TmpG1B[16U]="storage_class_t";
# 1180 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1B,_TmpG1B,_TmpG1B + 16U};
enum Cyc_Parse_Storage_class _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY20.tag==26){_Tmp0=yy1->YY20.val;{enum Cyc_Parse_Storage_class yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1187
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY20.tag=26U,_Tmp0.YY20.val=yy1;return _Tmp0;}static char _TmpG1C[17U]="type_specifier_t";
# 1181 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1C,_TmpG1C,_TmpG1C + 17U};
struct Cyc_Parse_Type_specifier _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY21.tag==27){_Tmp0=yy1->YY21.val;{struct Cyc_Parse_Type_specifier yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1188
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY21.tag=27U,_Tmp0.YY21.val=yy1;return _Tmp0;}static char _TmpG1D[12U]="aggr_kind_t";
# 1183 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1D,_TmpG1D,_TmpG1D + 12U};
enum Cyc_Absyn_AggrKind _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY22.tag==28){_Tmp0=yy1->YY22.val;{enum Cyc_Absyn_AggrKind yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1190
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY22.tag=28U,_Tmp0.YY22.val=yy1;return _Tmp0;}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1184 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1E,_TmpG1E,_TmpG1E + 20U};
struct _tuple25 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY23.tag==29){_Tmp0=yy1->YY23.val;{struct _tuple25 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1191
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY23.tag=29U,_Tmp0.YY23.val=yy1;return _Tmp0;}static char _TmpG1F[8U]="tqual_t";
# 1185 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1F,_TmpG1F,_TmpG1F + 8U};
struct Cyc_Absyn_Tqual _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY24.tag==30){_Tmp0=yy1->YY24.val;{struct Cyc_Absyn_Tqual yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1192
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY24.tag=30U,_Tmp0.YY24.val=yy1;return _Tmp0;}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1186 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG20,_TmpG20,_TmpG20 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY25.tag==31){_Tmp0=yy1->YY25.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1193
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY25.tag=31U,_Tmp0.YY25.val=yy1;return _Tmp0;}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1187 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG21,_TmpG21,_TmpG21 + 34U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY26.tag==32){_Tmp0=yy1->YY26.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1194
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY26.tag=32U,_Tmp0.YY26.val=yy1;return _Tmp0;}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1188 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG22,_TmpG22,_TmpG22 + 33U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY27.tag==33){_Tmp0=yy1->YY27.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1195
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY27.tag=33U,_Tmp0.YY27.val=yy1;return _Tmp0;}static char _TmpG23[18U]="declarator_t<`yy>";
# 1189 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG23,_TmpG23,_TmpG23 + 18U};
struct Cyc_Parse_Declarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY28.tag==34){_Tmp0=yy1->YY28.val;{struct Cyc_Parse_Declarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1196
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY28.tag=34U,_Tmp0.YY28.val=yy1;return _Tmp0;}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1190 "parse.y"
static struct _tuple12*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG24,_TmpG24,_TmpG24 + 45U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY29.tag==35){_Tmp0=yy1->YY29.val;{struct _tuple12*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1197
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple12*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY29.tag=35U,_Tmp0.YY29.val=yy1;return _Tmp0;}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1191 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG25,_TmpG25,_TmpG25 + 57U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY30.tag==36){_Tmp0=yy1->YY30.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1198
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY30.tag=36U,_Tmp0.YY30.val=yy1;return _Tmp0;}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1192 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG26,_TmpG26,_TmpG26 + 26U};
struct Cyc_Parse_Abstractdeclarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY31.tag==37){_Tmp0=yy1->YY31.val;{struct Cyc_Parse_Abstractdeclarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1199
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY31.tag=37U,_Tmp0.YY31.val=yy1;return _Tmp0;}static char _TmpG27[5U]="bool";
# 1193 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG27,_TmpG27,_TmpG27 + 5U};
int _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY32.tag==38){_Tmp0=yy1->YY32.val;{int yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1200
static union Cyc_YYSTYPE Cyc_YY32(int yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY32.tag=38U,_Tmp0.YY32.val=yy1;return _Tmp0;}static char _TmpG28[8U]="scope_t";
# 1194 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG28,_TmpG28,_TmpG28 + 8U};
enum Cyc_Absyn_Scope _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY33.tag==39){_Tmp0=yy1->YY33.val;{enum Cyc_Absyn_Scope yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1201
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY33.tag=39U,_Tmp0.YY33.val=yy1;return _Tmp0;}static char _TmpG29[16U]="datatypefield_t";
# 1195 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG29,_TmpG29,_TmpG29 + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY34.tag==40){_Tmp0=yy1->YY34.val;{struct Cyc_Absyn_Datatypefield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1202
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY34.tag=40U,_Tmp0.YY34.val=yy1;return _Tmp0;}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1196 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2A,_TmpG2A,_TmpG2A + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY35.tag==41){_Tmp0=yy1->YY35.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1203
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY35.tag=41U,_Tmp0.YY35.val=yy1;return _Tmp0;}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1197 "parse.y"
static struct _tuple26 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2B,_TmpG2B,_TmpG2B + 41U};
struct _tuple26 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY36.tag==42){_Tmp0=yy1->YY36.val;{struct _tuple26 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1204
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple26 yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY36.tag=42U,_Tmp0.YY36.val=yy1;return _Tmp0;}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1198 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2C,_TmpG2C,_TmpG2C + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY37.tag==43){_Tmp0=yy1->YY37.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1205
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY37.tag=43U,_Tmp0.YY37.val=yy1;return _Tmp0;}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1199 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2D,_TmpG2D,_TmpG2D + 31U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY38.tag==44){_Tmp0=yy1->YY38.val;{struct _tuple8*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1206
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY38.tag=44U,_Tmp0.YY38.val=yy1;return _Tmp0;}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1200 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2E,_TmpG2E,_TmpG2E + 42U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY39.tag==45){_Tmp0=yy1->YY39.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1207
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY39.tag=45U,_Tmp0.YY39.val=yy1;return _Tmp0;}static char _TmpG2F[139U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1201 "parse.y"
static struct _tuple27*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2F,_TmpG2F,_TmpG2F + 139U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY40.tag==46){_Tmp0=yy1->YY40.val;{struct _tuple27*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1208
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple27*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY40.tag=46U,_Tmp0.YY40.val=yy1;return _Tmp0;}static char _TmpG30[8U]="types_t";
# 1202 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG30,_TmpG30,_TmpG30 + 8U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY41.tag==47){_Tmp0=yy1->YY41.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1209
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY41.tag=47U,_Tmp0.YY41.val=yy1;return _Tmp0;}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1204 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG31,_TmpG31,_TmpG31 + 24U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY42.tag==48){_Tmp0=yy1->YY42.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1211
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY42.tag=48U,_Tmp0.YY42.val=yy1;return _Tmp0;}static char _TmpG32[13U]="designator_t";
# 1205 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG32,_TmpG32,_TmpG32 + 13U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY43.tag==49){_Tmp0=yy1->YY43.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1212
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY43.tag=49U,_Tmp0.YY43.val=yy1;return _Tmp0;}static char _TmpG33[7U]="kind_t";
# 1206 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG33,_TmpG33,_TmpG33 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY44.tag==50){_Tmp0=yy1->YY44.val;{struct Cyc_Absyn_Kind*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1213
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY44.tag=50U,_Tmp0.YY44.val=yy1;return _Tmp0;}static char _TmpG34[7U]="type_t";
# 1207 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG34,_TmpG34,_TmpG34 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY45.tag==51){_Tmp0=yy1->YY45.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1214
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY45.tag=51U,_Tmp0.YY45.val=yy1;return _Tmp0;}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1208 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG35,_TmpG35,_TmpG35 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY46.tag==52){_Tmp0=yy1->YY46.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1215
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY46.tag=52U,_Tmp0.YY46.val=yy1;return _Tmp0;}static char _TmpG36[12U]="attribute_t";
# 1209 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG36,_TmpG36,_TmpG36 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY47.tag==53){_Tmp0=yy1->YY47.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1216
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY47.tag=53U,_Tmp0.YY47.val=yy1;return _Tmp0;}static char _TmpG37[12U]="enumfield_t";
# 1210 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG37,_TmpG37,_TmpG37 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY48.tag==54){_Tmp0=yy1->YY48.val;{struct Cyc_Absyn_Enumfield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1217
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY48.tag=54U,_Tmp0.YY48.val=yy1;return _Tmp0;}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1211 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG38,_TmpG38,_TmpG38 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY49.tag==55){_Tmp0=yy1->YY49.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1218
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY49.tag=55U,_Tmp0.YY49.val=yy1;return _Tmp0;}static char _TmpG39[11U]="type_opt_t";
# 1212 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG39,_TmpG39,_TmpG39 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY50.tag==56){_Tmp0=yy1->YY50.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1219
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY50.tag=56U,_Tmp0.YY50.val=yy1;return _Tmp0;}static char _TmpG3A[61U]="$(list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)*`H";
# 1214 "parse.y"
static struct _tuple28*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3A,_TmpG3A,_TmpG3A + 61U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY51.tag==57){_Tmp0=yy1->YY51.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1221
static union Cyc_YYSTYPE Cyc_YY51(struct _tuple28*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY51.tag=57U,_Tmp0.YY51.val=yy1;return _Tmp0;}static char _TmpG3B[12U]="effconstr_t";
# 1215 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3B,_TmpG3B,_TmpG3B + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY52.tag==58){_Tmp0=yy1->YY52.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1222
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY52.tag=58U,_Tmp0.YY52.val=yy1;return _Tmp0;}static char _TmpG3C[21U]="$(type_t, type_t)@`H";
# 1216 "parse.y"
static struct _tuple29*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3C,_TmpG3C,_TmpG3C + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY53.tag==59){_Tmp0=yy1->YY53.val;{struct _tuple29*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1223
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple29*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY53.tag=59U,_Tmp0.YY53.val=yy1;return _Tmp0;}static char _TmpG3D[11U]="booltype_t";
# 1217 "parse.y"
static void*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3D,_TmpG3D,_TmpG3D + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY54.tag==60){_Tmp0=yy1->YY54.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1224
static union Cyc_YYSTYPE Cyc_YY54(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY54.tag=60U,_Tmp0.YY54.val=yy1;return _Tmp0;}static char _TmpG3E[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1218 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3E,_TmpG3E,_TmpG3E + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY55.tag==61){_Tmp0=yy1->YY55.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1225
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY55.tag=61U,_Tmp0.YY55.val=yy1;return _Tmp0;}static char _TmpG3F[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1219 "parse.y"
static struct _tuple30*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3F,_TmpG3F,_TmpG3F + 48U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY56.tag==62){_Tmp0=yy1->YY56.val;{struct _tuple30*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1226
static union Cyc_YYSTYPE Cyc_YY56(struct _tuple30*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY56.tag=62U,_Tmp0.YY56.val=yy1;return _Tmp0;}static char _TmpG40[18U]="list_t<qvar_t,`H>";
# 1220 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG40,_TmpG40,_TmpG40 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY57.tag==63){_Tmp0=yy1->YY57.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1227
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY57.tag=63U,_Tmp0.YY57.val=yy1;return _Tmp0;}static char _TmpG41[12U]="aqualtype_t";
# 1221 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG41,_TmpG41,_TmpG41 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY58.tag==64){_Tmp0=yy1->YY58.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1228
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY58.tag=64U,_Tmp0.YY58.val=yy1;return _Tmp0;}static char _TmpG42[20U]="pointer_qual_t<`yy>";
# 1222 "parse.y"
static void*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG42,_TmpG42,_TmpG42 + 20U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY59.tag==65){_Tmp0=yy1->YY59.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1229
static union Cyc_YYSTYPE Cyc_YY59(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY59.tag=65U,_Tmp0.YY59.val=yy1;return _Tmp0;}static char _TmpG43[21U]="pointer_quals_t<`yy>";
# 1223 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG43,_TmpG43,_TmpG43 + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY60.tag==66){_Tmp0=yy1->YY60.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1230
static union Cyc_YYSTYPE Cyc_YY60(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY60.tag=66U,_Tmp0.YY60.val=yy1;return _Tmp0;}static char _TmpG44[10U]="exp_opt_t";
# 1224 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG44,_TmpG44,_TmpG44 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY61.tag==67){_Tmp0=yy1->YY61.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1231
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY61.tag=67U,_Tmp0.YY61.val=yy1;return _Tmp0;}static char _TmpG45[33U]="$(exp_opt_t,exp_opt_t,exp_opt_t)";
# 1225 "parse.y"
static struct _tuple21 Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG45,_TmpG45,_TmpG45 + 33U};
struct _tuple21 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY62.tag==68){_Tmp0=yy1->YY62.val;{struct _tuple21 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1232
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple21 yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY62.tag=68U,_Tmp0.YY62.val=yy1;return _Tmp0;}static char _TmpG46[10U]="raw_exp_t";
# 1227 "parse.y"
static void*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG46,_TmpG46,_TmpG46 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY63.tag==69){_Tmp0=yy1->YY63.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1234
static union Cyc_YYSTYPE Cyc_YY63(void*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY63.tag=69U,_Tmp0.YY63.val=yy1;return _Tmp0;}static char _TmpG47[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1228 "parse.y"
static struct _tuple31*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG47,_TmpG47,_TmpG47 + 112U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY64.tag==70){_Tmp0=yy1->YY64.val;{struct _tuple31*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1235
static union Cyc_YYSTYPE Cyc_YY64(struct _tuple31*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY64.tag=70U,_Tmp0.YY64.val=yy1;return _Tmp0;}static char _TmpG48[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1229 "parse.y"
static struct _tuple28*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG48,_TmpG48,_TmpG48 + 73U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY65.tag==71){_Tmp0=yy1->YY65.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1236
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple28*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY65.tag=71U,_Tmp0.YY65.val=yy1;return _Tmp0;}static char _TmpG49[28U]="list_t<string_t<`H>@`H, `H>";
# 1230 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG49,_TmpG49,_TmpG49 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY66.tag==72){_Tmp0=yy1->YY66.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1237
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY66.tag=72U,_Tmp0.YY66.val=yy1;return _Tmp0;}static char _TmpG4A[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1231 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY67(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4A,_TmpG4A,_TmpG4A + 38U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY67.tag==73){_Tmp0=yy1->YY67.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1238
static union Cyc_YYSTYPE Cyc_YY67(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY67.tag=73U,_Tmp0.YY67.val=yy1;return _Tmp0;}static char _TmpG4B[26U]="$(string_t<`H>, exp_t)@`H";
# 1232 "parse.y"
static struct _tuple32*Cyc_yyget_YY68(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4B,_TmpG4B,_TmpG4B + 26U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY68.tag==74){_Tmp0=yy1->YY68.val;{struct _tuple32*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1239
static union Cyc_YYSTYPE Cyc_YY68(struct _tuple32*yy1){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY68.tag=74U,_Tmp0.YY68.val=yy1;return _Tmp0;}static char _TmpG4C[16U]="exp_maker_fun_t";
# 1233 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY69(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_TmpG4C,_TmpG4C,_TmpG4C + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY69.tag==75){_Tmp0=yy1->YY69.val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1240
static union Cyc_YYSTYPE Cyc_YY69(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){union Cyc_YYSTYPE _Tmp0;_Tmp0.YY69.tag=75U,_Tmp0.YY69.val=yy1;return _Tmp0;}struct Cyc_Yyltype{int timestamp;unsigned first_line;unsigned first_column;unsigned last_line;unsigned last_column;};
# 1256
struct Cyc_Yyltype Cyc_yynewloc (void){
struct Cyc_Yyltype _Tmp0;_Tmp0.timestamp=0,_Tmp0.first_line=0U,_Tmp0.first_column=0U,_Tmp0.last_line=0U,_Tmp0.last_column=0U;return _Tmp0;}
# 1259
struct Cyc_Yyltype Cyc_yylloc={0,0U,0U,0U,0U};
# 1270 "parse.y"
static short Cyc_yytranslate[398U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,169,2,2,154,167,164,2,151,152,147,161,146,165,156,166,2,2,2,2,2,2,2,2,2,2,155,143,149,148,150,160,159,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,157,2,158,163,153,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,144,162,145,168,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142};static char _TmpG4D[2U]="$";static char _TmpG4E[6U]="error";static char _TmpG4F[12U]="$undefined.";static char _TmpG50[5U]="AUTO";static char _TmpG51[9U]="REGISTER";static char _TmpG52[7U]="STATIC";static char _TmpG53[7U]="EXTERN";static char _TmpG54[8U]="TYPEDEF";static char _TmpG55[5U]="VOID";static char _TmpG56[5U]="CHAR";static char _TmpG57[6U]="SHORT";static char _TmpG58[4U]="INT";static char _TmpG59[5U]="LONG";static char _TmpG5A[6U]="FLOAT";static char _TmpG5B[7U]="DOUBLE";static char _TmpG5C[7U]="SIGNED";static char _TmpG5D[9U]="UNSIGNED";static char _TmpG5E[6U]="CONST";static char _TmpG5F[9U]="VOLATILE";static char _TmpG60[9U]="RESTRICT";static char _TmpG61[7U]="STRUCT";static char _TmpG62[6U]="UNION";static char _TmpG63[5U]="CASE";static char _TmpG64[8U]="DEFAULT";static char _TmpG65[7U]="INLINE";static char _TmpG66[7U]="SIZEOF";static char _TmpG67[9U]="OFFSETOF";static char _TmpG68[3U]="IF";static char _TmpG69[5U]="ELSE";static char _TmpG6A[7U]="SWITCH";static char _TmpG6B[6U]="WHILE";static char _TmpG6C[3U]="DO";static char _TmpG6D[4U]="FOR";static char _TmpG6E[5U]="GOTO";static char _TmpG6F[9U]="CONTINUE";static char _TmpG70[6U]="BREAK";static char _TmpG71[7U]="RETURN";static char _TmpG72[5U]="ENUM";static char _TmpG73[7U]="TYPEOF";static char _TmpG74[16U]="BUILTIN_VA_LIST";static char _TmpG75[10U]="EXTENSION";static char _TmpG76[8U]="COMPLEX";static char _TmpG77[8U]="NULL_kw";static char _TmpG78[4U]="LET";static char _TmpG79[6U]="THROW";static char _TmpG7A[4U]="TRY";static char _TmpG7B[6U]="CATCH";static char _TmpG7C[7U]="EXPORT";static char _TmpG7D[9U]="OVERRIDE";static char _TmpG7E[5U]="HIDE";static char _TmpG7F[4U]="NEW";static char _TmpG80[5U]="QNEW";static char _TmpG81[9U]="ABSTRACT";static char _TmpG82[9U]="FALLTHRU";static char _TmpG83[6U]="USING";static char _TmpG84[10U]="NAMESPACE";static char _TmpG85[9U]="DATATYPE";static char _TmpG86[7U]="MALLOC";static char _TmpG87[8U]="RMALLOC";static char _TmpG88[9U]="RVMALLOC";static char _TmpG89[15U]="RMALLOC_INLINE";static char _TmpG8A[8U]="QMALLOC";static char _TmpG8B[7U]="CALLOC";static char _TmpG8C[8U]="QCALLOC";static char _TmpG8D[8U]="RCALLOC";static char _TmpG8E[5U]="SWAP";static char _TmpG8F[9U]="REGION_T";static char _TmpG90[6U]="TAG_T";static char _TmpG91[7U]="REGION";static char _TmpG92[5U]="RNEW";static char _TmpG93[8U]="REGIONS";static char _TmpG94[7U]="PORTON";static char _TmpG95[8U]="PORTOFF";static char _TmpG96[7U]="PRAGMA";static char _TmpG97[10U]="TEMPESTON";static char _TmpG98[11U]="TEMPESTOFF";static char _TmpG99[13U]="AQ_ALIASABLE";static char _TmpG9A[10U]="AQ_REFCNT";static char _TmpG9B[14U]="AQ_RESTRICTED";static char _TmpG9C[10U]="AQ_UNIQUE";static char _TmpG9D[8U]="AQUAL_T";static char _TmpG9E[8U]="NUMELTS";static char _TmpG9F[6U]="TAGOF";static char _TmpGA0[8U]="VALUEOF";static char _TmpGA1[10U]="VALUEOF_T";static char _TmpGA2[9U]="TAGCHECK";static char _TmpGA3[13U]="NUMELTS_QUAL";static char _TmpGA4[10U]="THIN_QUAL";static char _TmpGA5[9U]="FAT_QUAL";static char _TmpGA6[13U]="NOTNULL_QUAL";static char _TmpGA7[14U]="NULLABLE_QUAL";static char _TmpGA8[14U]="REQUIRES_QUAL";static char _TmpGA9[13U]="ENSURES_QUAL";static char _TmpGAA[12U]="EFFECT_QUAL";static char _TmpGAB[12U]="THROWS_QUAL";static char _TmpGAC[12U]="REGION_QUAL";static char _TmpGAD[16U]="NOZEROTERM_QUAL";static char _TmpGAE[14U]="ZEROTERM_QUAL";static char _TmpGAF[12U]="TAGGED_QUAL";static char _TmpGB0[12U]="ASSERT_QUAL";static char _TmpGB1[18U]="ASSERT_FALSE_QUAL";static char _TmpGB2[11U]="ALIAS_QUAL";static char _TmpGB3[7U]="AQUALS";static char _TmpGB4[16U]="EXTENSIBLE_QUAL";static char _TmpGB5[18U]="AUTORELEASED_QUAL";static char _TmpGB6[7U]="PTR_OP";static char _TmpGB7[7U]="INC_OP";static char _TmpGB8[7U]="DEC_OP";static char _TmpGB9[8U]="LEFT_OP";static char _TmpGBA[9U]="RIGHT_OP";static char _TmpGBB[6U]="LE_OP";static char _TmpGBC[6U]="GE_OP";static char _TmpGBD[6U]="EQ_OP";static char _TmpGBE[6U]="NE_OP";static char _TmpGBF[7U]="AND_OP";static char _TmpGC0[6U]="OR_OP";static char _TmpGC1[11U]="MUL_ASSIGN";static char _TmpGC2[11U]="DIV_ASSIGN";static char _TmpGC3[11U]="MOD_ASSIGN";static char _TmpGC4[11U]="ADD_ASSIGN";static char _TmpGC5[11U]="SUB_ASSIGN";static char _TmpGC6[12U]="LEFT_ASSIGN";static char _TmpGC7[13U]="RIGHT_ASSIGN";static char _TmpGC8[11U]="AND_ASSIGN";static char _TmpGC9[11U]="XOR_ASSIGN";static char _TmpGCA[10U]="OR_ASSIGN";static char _TmpGCB[9U]="ELLIPSIS";static char _TmpGCC[11U]="LEFT_RIGHT";static char _TmpGCD[12U]="COLON_COLON";static char _TmpGCE[11U]="IDENTIFIER";static char _TmpGCF[17U]="INTEGER_CONSTANT";static char _TmpGD0[7U]="STRING";static char _TmpGD1[8U]="WSTRING";static char _TmpGD2[19U]="CHARACTER_CONSTANT";static char _TmpGD3[20U]="WCHARACTER_CONSTANT";static char _TmpGD4[18U]="FLOATING_CONSTANT";static char _TmpGD5[9U]="TYPE_VAR";static char _TmpGD6[13U]="TYPEDEF_NAME";static char _TmpGD7[16U]="QUAL_IDENTIFIER";static char _TmpGD8[18U]="QUAL_TYPEDEF_NAME";static char _TmpGD9[18U]="AQUAL_SHORT_CONST";static char _TmpGDA[10U]="ATTRIBUTE";static char _TmpGDB[8U]="ASM_TOK";static char _TmpGDC[4U]="';'";static char _TmpGDD[4U]="'{'";static char _TmpGDE[4U]="'}'";static char _TmpGDF[4U]="','";static char _TmpGE0[4U]="'*'";static char _TmpGE1[4U]="'='";static char _TmpGE2[4U]="'<'";static char _TmpGE3[4U]="'>'";static char _TmpGE4[4U]="'('";static char _TmpGE5[4U]="')'";static char _TmpGE6[4U]="'_'";static char _TmpGE7[4U]="'$'";static char _TmpGE8[4U]="':'";static char _TmpGE9[4U]="'.'";static char _TmpGEA[4U]="'['";static char _TmpGEB[4U]="']'";static char _TmpGEC[4U]="'@'";static char _TmpGED[4U]="'?'";static char _TmpGEE[4U]="'+'";static char _TmpGEF[4U]="'|'";static char _TmpGF0[4U]="'^'";static char _TmpGF1[4U]="'&'";static char _TmpGF2[4U]="'-'";static char _TmpGF3[4U]="'/'";static char _TmpGF4[4U]="'%'";static char _TmpGF5[4U]="'~'";static char _TmpGF6[4U]="'!'";static char _TmpGF7[5U]="prog";static char _TmpGF8[17U]="translation_unit";static char _TmpGF9[18U]="tempest_on_action";static char _TmpGFA[19U]="tempest_off_action";static char _TmpGFB[16U]="extern_c_action";static char _TmpGFC[13U]="end_extern_c";static char _TmpGFD[14U]="hide_list_opt";static char _TmpGFE[17U]="hide_list_values";static char _TmpGFF[16U]="export_list_opt";static char _TmpG100[12U]="export_list";static char _TmpG101[19U]="export_list_values";static char _TmpG102[13U]="override_opt";static char _TmpG103[21U]="external_declaration";static char _TmpG104[14U]="optional_semi";static char _TmpG105[20U]="function_definition";static char _TmpG106[21U]="function_definition2";static char _TmpG107[13U]="using_action";static char _TmpG108[15U]="unusing_action";static char _TmpG109[17U]="namespace_action";static char _TmpG10A[19U]="unnamespace_action";static char _TmpG10B[12U]="declaration";static char _TmpG10C[9U]="open_opt";static char _TmpG10D[17U]="declaration_list";static char _TmpG10E[23U]="declaration_specifiers";static char _TmpG10F[24U]="storage_class_specifier";static char _TmpG110[15U]="attributes_opt";static char _TmpG111[11U]="attributes";static char _TmpG112[15U]="attribute_list";static char _TmpG113[10U]="attribute";static char _TmpG114[15U]="type_specifier";static char _TmpG115[25U]="type_specifier_notypedef";static char _TmpG116[5U]="kind";static char _TmpG117[15U]="type_qualifier";static char _TmpG118[15U]="enum_specifier";static char _TmpG119[11U]="enum_field";static char _TmpG11A[22U]="enum_declaration_list";static char _TmpG11B[26U]="struct_or_union_specifier";static char _TmpG11C[26U]="maybe_tagged_struct_union";static char _TmpG11D[16U]="struct_or_union";static char _TmpG11E[16U]="type_params_opt";static char _TmpG11F[24U]="struct_declaration_list";static char _TmpG120[25U]="struct_declaration_list0";static char _TmpG121[25U]="init_declarator_list_rev";static char _TmpG122[16U]="init_declarator";static char _TmpG123[19U]="struct_declaration";static char _TmpG124[25U]="specifier_qualifier_list";static char _TmpG125[35U]="notypedef_specifier_qualifier_list";static char _TmpG126[27U]="struct_declarator_list_rev";static char _TmpG127[18U]="struct_declarator";static char _TmpG128[20U]="requires_clause_opt";static char _TmpG129[19U]="datatype_specifier";static char _TmpG12A[14U]="qual_datatype";static char _TmpG12B[19U]="datatypefield_list";static char _TmpG12C[20U]="datatypefield_scope";static char _TmpG12D[14U]="datatypefield";static char _TmpG12E[11U]="declarator";static char _TmpG12F[23U]="declarator_withtypedef";static char _TmpG130[18U]="direct_declarator";static char _TmpG131[30U]="direct_declarator_withtypedef";static char _TmpG132[8U]="pointer";static char _TmpG133[12U]="one_pointer";static char _TmpG134[14U]="pointer_quals";static char _TmpG135[13U]="pointer_qual";static char _TmpG136[16U]="aqual_specifier";static char _TmpG137[23U]="pointer_null_and_bound";static char _TmpG138[14U]="pointer_bound";static char _TmpG139[18U]="zeroterm_qual_opt";static char _TmpG13A[8U]="eff_set";static char _TmpG13B[8U]="eff_opt";static char _TmpG13C[11U]="tqual_list";static char _TmpG13D[20U]="parameter_type_list";static char _TmpG13E[14U]="opt_aqual_bnd";static char _TmpG13F[9U]="type_var";static char _TmpG140[16U]="optional_effect";static char _TmpG141[27U]="optional_effconstr_qualbnd";static char _TmpG142[18U]="effconstr_qualbnd";static char _TmpG143[14U]="effconstr_elt";static char _TmpG144[13U]="qual_bnd_elt";static char _TmpG145[12U]="aqual_const";static char _TmpG146[15U]="qual_bnd_const";static char _TmpG147[14U]="qual_bnd_term";static char _TmpG148[16U]="optional_inject";static char _TmpG149[11U]="effect_set";static char _TmpG14A[14U]="atomic_effect";static char _TmpG14B[11U]="region_set";static char _TmpG14C[15U]="parameter_list";static char _TmpG14D[22U]="parameter_declaration";static char _TmpG14E[16U]="identifier_list";static char _TmpG14F[17U]="identifier_list0";static char _TmpG150[12U]="initializer";static char _TmpG151[18U]="array_initializer";static char _TmpG152[17U]="initializer_list";static char _TmpG153[12U]="designation";static char _TmpG154[16U]="designator_list";static char _TmpG155[11U]="designator";static char _TmpG156[10U]="type_name";static char _TmpG157[14U]="any_type_name";static char _TmpG158[15U]="type_name_list";static char _TmpG159[20U]="abstract_declarator";static char _TmpG15A[27U]="direct_abstract_declarator";static char _TmpG15B[32U]="requires_and_ensures_and_throws";static char _TmpG15C[36U]="requires_and_ensures_and_throws_opt";static char _TmpG15D[10U]="statement";static char _TmpG15E[18U]="labeled_statement";static char _TmpG15F[21U]="expression_statement";static char _TmpG160[19U]="compound_statement";static char _TmpG161[16U]="block_item_list";static char _TmpG162[20U]="selection_statement";static char _TmpG163[15U]="switch_clauses";static char _TmpG164[20U]="iteration_statement";static char _TmpG165[12U]="for_exp_opt";static char _TmpG166[15U]="jump_statement";static char _TmpG167[12U]="exp_pattern";static char _TmpG168[20U]="conditional_pattern";static char _TmpG169[19U]="logical_or_pattern";static char _TmpG16A[20U]="logical_and_pattern";static char _TmpG16B[21U]="inclusive_or_pattern";static char _TmpG16C[21U]="exclusive_or_pattern";static char _TmpG16D[12U]="and_pattern";static char _TmpG16E[17U]="equality_pattern";static char _TmpG16F[19U]="relational_pattern";static char _TmpG170[14U]="shift_pattern";static char _TmpG171[17U]="additive_pattern";static char _TmpG172[23U]="multiplicative_pattern";static char _TmpG173[13U]="cast_pattern";static char _TmpG174[14U]="unary_pattern";static char _TmpG175[16U]="postfix_pattern";static char _TmpG176[16U]="primary_pattern";static char _TmpG177[8U]="pattern";static char _TmpG178[19U]="tuple_pattern_list";static char _TmpG179[20U]="tuple_pattern_list0";static char _TmpG17A[14U]="field_pattern";static char _TmpG17B[19U]="field_pattern_list";static char _TmpG17C[20U]="field_pattern_list0";static char _TmpG17D[11U]="expression";static char _TmpG17E[22U]="assignment_expression";static char _TmpG17F[20U]="assignment_operator";static char _TmpG180[23U]="conditional_expression";static char _TmpG181[20U]="constant_expression";static char _TmpG182[22U]="logical_or_expression";static char _TmpG183[23U]="logical_and_expression";static char _TmpG184[24U]="inclusive_or_expression";static char _TmpG185[24U]="exclusive_or_expression";static char _TmpG186[15U]="and_expression";static char _TmpG187[20U]="equality_expression";static char _TmpG188[22U]="relational_expression";static char _TmpG189[17U]="shift_expression";static char _TmpG18A[20U]="additive_expression";static char _TmpG18B[26U]="multiplicative_expression";static char _TmpG18C[12U]="equality_op";static char _TmpG18D[14U]="relational_op";static char _TmpG18E[12U]="additive_op";static char _TmpG18F[18U]="multiplicative_op";static char _TmpG190[16U]="cast_expression";static char _TmpG191[17U]="unary_expression";static char _TmpG192[15U]="unary_operator";static char _TmpG193[9U]="asm_expr";static char _TmpG194[13U]="volatile_opt";static char _TmpG195[12U]="asm_out_opt";static char _TmpG196[12U]="asm_outlist";static char _TmpG197[11U]="asm_in_opt";static char _TmpG198[11U]="asm_inlist";static char _TmpG199[11U]="asm_io_elt";static char _TmpG19A[16U]="asm_clobber_opt";static char _TmpG19B[17U]="asm_clobber_list";static char _TmpG19C[19U]="postfix_expression";static char _TmpG19D[17U]="field_expression";static char _TmpG19E[19U]="primary_expression";static char _TmpG19F[25U]="argument_expression_list";static char _TmpG1A0[26U]="argument_expression_list0";static char _TmpG1A1[9U]="constant";static char _TmpG1A2[20U]="qual_opt_identifier";static char _TmpG1A3[17U]="qual_opt_typedef";static char _TmpG1A4[18U]="struct_union_name";static char _TmpG1A5[11U]="field_name";static char _TmpG1A6[12U]="right_angle";
# 1649 "parse.y"
static struct _fat_ptr Cyc_yytname[346U]={{_TmpG4D,_TmpG4D,_TmpG4D + 2U},{_TmpG4E,_TmpG4E,_TmpG4E + 6U},{_TmpG4F,_TmpG4F,_TmpG4F + 12U},{_TmpG50,_TmpG50,_TmpG50 + 5U},{_TmpG51,_TmpG51,_TmpG51 + 9U},{_TmpG52,_TmpG52,_TmpG52 + 7U},{_TmpG53,_TmpG53,_TmpG53 + 7U},{_TmpG54,_TmpG54,_TmpG54 + 8U},{_TmpG55,_TmpG55,_TmpG55 + 5U},{_TmpG56,_TmpG56,_TmpG56 + 5U},{_TmpG57,_TmpG57,_TmpG57 + 6U},{_TmpG58,_TmpG58,_TmpG58 + 4U},{_TmpG59,_TmpG59,_TmpG59 + 5U},{_TmpG5A,_TmpG5A,_TmpG5A + 6U},{_TmpG5B,_TmpG5B,_TmpG5B + 7U},{_TmpG5C,_TmpG5C,_TmpG5C + 7U},{_TmpG5D,_TmpG5D,_TmpG5D + 9U},{_TmpG5E,_TmpG5E,_TmpG5E + 6U},{_TmpG5F,_TmpG5F,_TmpG5F + 9U},{_TmpG60,_TmpG60,_TmpG60 + 9U},{_TmpG61,_TmpG61,_TmpG61 + 7U},{_TmpG62,_TmpG62,_TmpG62 + 6U},{_TmpG63,_TmpG63,_TmpG63 + 5U},{_TmpG64,_TmpG64,_TmpG64 + 8U},{_TmpG65,_TmpG65,_TmpG65 + 7U},{_TmpG66,_TmpG66,_TmpG66 + 7U},{_TmpG67,_TmpG67,_TmpG67 + 9U},{_TmpG68,_TmpG68,_TmpG68 + 3U},{_TmpG69,_TmpG69,_TmpG69 + 5U},{_TmpG6A,_TmpG6A,_TmpG6A + 7U},{_TmpG6B,_TmpG6B,_TmpG6B + 6U},{_TmpG6C,_TmpG6C,_TmpG6C + 3U},{_TmpG6D,_TmpG6D,_TmpG6D + 4U},{_TmpG6E,_TmpG6E,_TmpG6E + 5U},{_TmpG6F,_TmpG6F,_TmpG6F + 9U},{_TmpG70,_TmpG70,_TmpG70 + 6U},{_TmpG71,_TmpG71,_TmpG71 + 7U},{_TmpG72,_TmpG72,_TmpG72 + 5U},{_TmpG73,_TmpG73,_TmpG73 + 7U},{_TmpG74,_TmpG74,_TmpG74 + 16U},{_TmpG75,_TmpG75,_TmpG75 + 10U},{_TmpG76,_TmpG76,_TmpG76 + 8U},{_TmpG77,_TmpG77,_TmpG77 + 8U},{_TmpG78,_TmpG78,_TmpG78 + 4U},{_TmpG79,_TmpG79,_TmpG79 + 6U},{_TmpG7A,_TmpG7A,_TmpG7A + 4U},{_TmpG7B,_TmpG7B,_TmpG7B + 6U},{_TmpG7C,_TmpG7C,_TmpG7C + 7U},{_TmpG7D,_TmpG7D,_TmpG7D + 9U},{_TmpG7E,_TmpG7E,_TmpG7E + 5U},{_TmpG7F,_TmpG7F,_TmpG7F + 4U},{_TmpG80,_TmpG80,_TmpG80 + 5U},{_TmpG81,_TmpG81,_TmpG81 + 9U},{_TmpG82,_TmpG82,_TmpG82 + 9U},{_TmpG83,_TmpG83,_TmpG83 + 6U},{_TmpG84,_TmpG84,_TmpG84 + 10U},{_TmpG85,_TmpG85,_TmpG85 + 9U},{_TmpG86,_TmpG86,_TmpG86 + 7U},{_TmpG87,_TmpG87,_TmpG87 + 8U},{_TmpG88,_TmpG88,_TmpG88 + 9U},{_TmpG89,_TmpG89,_TmpG89 + 15U},{_TmpG8A,_TmpG8A,_TmpG8A + 8U},{_TmpG8B,_TmpG8B,_TmpG8B + 7U},{_TmpG8C,_TmpG8C,_TmpG8C + 8U},{_TmpG8D,_TmpG8D,_TmpG8D + 8U},{_TmpG8E,_TmpG8E,_TmpG8E + 5U},{_TmpG8F,_TmpG8F,_TmpG8F + 9U},{_TmpG90,_TmpG90,_TmpG90 + 6U},{_TmpG91,_TmpG91,_TmpG91 + 7U},{_TmpG92,_TmpG92,_TmpG92 + 5U},{_TmpG93,_TmpG93,_TmpG93 + 8U},{_TmpG94,_TmpG94,_TmpG94 + 7U},{_TmpG95,_TmpG95,_TmpG95 + 8U},{_TmpG96,_TmpG96,_TmpG96 + 7U},{_TmpG97,_TmpG97,_TmpG97 + 10U},{_TmpG98,_TmpG98,_TmpG98 + 11U},{_TmpG99,_TmpG99,_TmpG99 + 13U},{_TmpG9A,_TmpG9A,_TmpG9A + 10U},{_TmpG9B,_TmpG9B,_TmpG9B + 14U},{_TmpG9C,_TmpG9C,_TmpG9C + 10U},{_TmpG9D,_TmpG9D,_TmpG9D + 8U},{_TmpG9E,_TmpG9E,_TmpG9E + 8U},{_TmpG9F,_TmpG9F,_TmpG9F + 6U},{_TmpGA0,_TmpGA0,_TmpGA0 + 8U},{_TmpGA1,_TmpGA1,_TmpGA1 + 10U},{_TmpGA2,_TmpGA2,_TmpGA2 + 9U},{_TmpGA3,_TmpGA3,_TmpGA3 + 13U},{_TmpGA4,_TmpGA4,_TmpGA4 + 10U},{_TmpGA5,_TmpGA5,_TmpGA5 + 9U},{_TmpGA6,_TmpGA6,_TmpGA6 + 13U},{_TmpGA7,_TmpGA7,_TmpGA7 + 14U},{_TmpGA8,_TmpGA8,_TmpGA8 + 14U},{_TmpGA9,_TmpGA9,_TmpGA9 + 13U},{_TmpGAA,_TmpGAA,_TmpGAA + 12U},{_TmpGAB,_TmpGAB,_TmpGAB + 12U},{_TmpGAC,_TmpGAC,_TmpGAC + 12U},{_TmpGAD,_TmpGAD,_TmpGAD + 16U},{_TmpGAE,_TmpGAE,_TmpGAE + 14U},{_TmpGAF,_TmpGAF,_TmpGAF + 12U},{_TmpGB0,_TmpGB0,_TmpGB0 + 12U},{_TmpGB1,_TmpGB1,_TmpGB1 + 18U},{_TmpGB2,_TmpGB2,_TmpGB2 + 11U},{_TmpGB3,_TmpGB3,_TmpGB3 + 7U},{_TmpGB4,_TmpGB4,_TmpGB4 + 16U},{_TmpGB5,_TmpGB5,_TmpGB5 + 18U},{_TmpGB6,_TmpGB6,_TmpGB6 + 7U},{_TmpGB7,_TmpGB7,_TmpGB7 + 7U},{_TmpGB8,_TmpGB8,_TmpGB8 + 7U},{_TmpGB9,_TmpGB9,_TmpGB9 + 8U},{_TmpGBA,_TmpGBA,_TmpGBA + 9U},{_TmpGBB,_TmpGBB,_TmpGBB + 6U},{_TmpGBC,_TmpGBC,_TmpGBC + 6U},{_TmpGBD,_TmpGBD,_TmpGBD + 6U},{_TmpGBE,_TmpGBE,_TmpGBE + 6U},{_TmpGBF,_TmpGBF,_TmpGBF + 7U},{_TmpGC0,_TmpGC0,_TmpGC0 + 6U},{_TmpGC1,_TmpGC1,_TmpGC1 + 11U},{_TmpGC2,_TmpGC2,_TmpGC2 + 11U},{_TmpGC3,_TmpGC3,_TmpGC3 + 11U},{_TmpGC4,_TmpGC4,_TmpGC4 + 11U},{_TmpGC5,_TmpGC5,_TmpGC5 + 11U},{_TmpGC6,_TmpGC6,_TmpGC6 + 12U},{_TmpGC7,_TmpGC7,_TmpGC7 + 13U},{_TmpGC8,_TmpGC8,_TmpGC8 + 11U},{_TmpGC9,_TmpGC9,_TmpGC9 + 11U},{_TmpGCA,_TmpGCA,_TmpGCA + 10U},{_TmpGCB,_TmpGCB,_TmpGCB + 9U},{_TmpGCC,_TmpGCC,_TmpGCC + 11U},{_TmpGCD,_TmpGCD,_TmpGCD + 12U},{_TmpGCE,_TmpGCE,_TmpGCE + 11U},{_TmpGCF,_TmpGCF,_TmpGCF + 17U},{_TmpGD0,_TmpGD0,_TmpGD0 + 7U},{_TmpGD1,_TmpGD1,_TmpGD1 + 8U},{_TmpGD2,_TmpGD2,_TmpGD2 + 19U},{_TmpGD3,_TmpGD3,_TmpGD3 + 20U},{_TmpGD4,_TmpGD4,_TmpGD4 + 18U},{_TmpGD5,_TmpGD5,_TmpGD5 + 9U},{_TmpGD6,_TmpGD6,_TmpGD6 + 13U},{_TmpGD7,_TmpGD7,_TmpGD7 + 16U},{_TmpGD8,_TmpGD8,_TmpGD8 + 18U},{_TmpGD9,_TmpGD9,_TmpGD9 + 18U},{_TmpGDA,_TmpGDA,_TmpGDA + 10U},{_TmpGDB,_TmpGDB,_TmpGDB + 8U},{_TmpGDC,_TmpGDC,_TmpGDC + 4U},{_TmpGDD,_TmpGDD,_TmpGDD + 4U},{_TmpGDE,_TmpGDE,_TmpGDE + 4U},{_TmpGDF,_TmpGDF,_TmpGDF + 4U},{_TmpGE0,_TmpGE0,_TmpGE0 + 4U},{_TmpGE1,_TmpGE1,_TmpGE1 + 4U},{_TmpGE2,_TmpGE2,_TmpGE2 + 4U},{_TmpGE3,_TmpGE3,_TmpGE3 + 4U},{_TmpGE4,_TmpGE4,_TmpGE4 + 4U},{_TmpGE5,_TmpGE5,_TmpGE5 + 4U},{_TmpGE6,_TmpGE6,_TmpGE6 + 4U},{_TmpGE7,_TmpGE7,_TmpGE7 + 4U},{_TmpGE8,_TmpGE8,_TmpGE8 + 4U},{_TmpGE9,_TmpGE9,_TmpGE9 + 4U},{_TmpGEA,_TmpGEA,_TmpGEA + 4U},{_TmpGEB,_TmpGEB,_TmpGEB + 4U},{_TmpGEC,_TmpGEC,_TmpGEC + 4U},{_TmpGED,_TmpGED,_TmpGED + 4U},{_TmpGEE,_TmpGEE,_TmpGEE + 4U},{_TmpGEF,_TmpGEF,_TmpGEF + 4U},{_TmpGF0,_TmpGF0,_TmpGF0 + 4U},{_TmpGF1,_TmpGF1,_TmpGF1 + 4U},{_TmpGF2,_TmpGF2,_TmpGF2 + 4U},{_TmpGF3,_TmpGF3,_TmpGF3 + 4U},{_TmpGF4,_TmpGF4,_TmpGF4 + 4U},{_TmpGF5,_TmpGF5,_TmpGF5 + 4U},{_TmpGF6,_TmpGF6,_TmpGF6 + 4U},{_TmpGF7,_TmpGF7,_TmpGF7 + 5U},{_TmpGF8,_TmpGF8,_TmpGF8 + 17U},{_TmpGF9,_TmpGF9,_TmpGF9 + 18U},{_TmpGFA,_TmpGFA,_TmpGFA + 19U},{_TmpGFB,_TmpGFB,_TmpGFB + 16U},{_TmpGFC,_TmpGFC,_TmpGFC + 13U},{_TmpGFD,_TmpGFD,_TmpGFD + 14U},{_TmpGFE,_TmpGFE,_TmpGFE + 17U},{_TmpGFF,_TmpGFF,_TmpGFF + 16U},{_TmpG100,_TmpG100,_TmpG100 + 12U},{_TmpG101,_TmpG101,_TmpG101 + 19U},{_TmpG102,_TmpG102,_TmpG102 + 13U},{_TmpG103,_TmpG103,_TmpG103 + 21U},{_TmpG104,_TmpG104,_TmpG104 + 14U},{_TmpG105,_TmpG105,_TmpG105 + 20U},{_TmpG106,_TmpG106,_TmpG106 + 21U},{_TmpG107,_TmpG107,_TmpG107 + 13U},{_TmpG108,_TmpG108,_TmpG108 + 15U},{_TmpG109,_TmpG109,_TmpG109 + 17U},{_TmpG10A,_TmpG10A,_TmpG10A + 19U},{_TmpG10B,_TmpG10B,_TmpG10B + 12U},{_TmpG10C,_TmpG10C,_TmpG10C + 9U},{_TmpG10D,_TmpG10D,_TmpG10D + 17U},{_TmpG10E,_TmpG10E,_TmpG10E + 23U},{_TmpG10F,_TmpG10F,_TmpG10F + 24U},{_TmpG110,_TmpG110,_TmpG110 + 15U},{_TmpG111,_TmpG111,_TmpG111 + 11U},{_TmpG112,_TmpG112,_TmpG112 + 15U},{_TmpG113,_TmpG113,_TmpG113 + 10U},{_TmpG114,_TmpG114,_TmpG114 + 15U},{_TmpG115,_TmpG115,_TmpG115 + 25U},{_TmpG116,_TmpG116,_TmpG116 + 5U},{_TmpG117,_TmpG117,_TmpG117 + 15U},{_TmpG118,_TmpG118,_TmpG118 + 15U},{_TmpG119,_TmpG119,_TmpG119 + 11U},{_TmpG11A,_TmpG11A,_TmpG11A + 22U},{_TmpG11B,_TmpG11B,_TmpG11B + 26U},{_TmpG11C,_TmpG11C,_TmpG11C + 26U},{_TmpG11D,_TmpG11D,_TmpG11D + 16U},{_TmpG11E,_TmpG11E,_TmpG11E + 16U},{_TmpG11F,_TmpG11F,_TmpG11F + 24U},{_TmpG120,_TmpG120,_TmpG120 + 25U},{_TmpG121,_TmpG121,_TmpG121 + 25U},{_TmpG122,_TmpG122,_TmpG122 + 16U},{_TmpG123,_TmpG123,_TmpG123 + 19U},{_TmpG124,_TmpG124,_TmpG124 + 25U},{_TmpG125,_TmpG125,_TmpG125 + 35U},{_TmpG126,_TmpG126,_TmpG126 + 27U},{_TmpG127,_TmpG127,_TmpG127 + 18U},{_TmpG128,_TmpG128,_TmpG128 + 20U},{_TmpG129,_TmpG129,_TmpG129 + 19U},{_TmpG12A,_TmpG12A,_TmpG12A + 14U},{_TmpG12B,_TmpG12B,_TmpG12B + 19U},{_TmpG12C,_TmpG12C,_TmpG12C + 20U},{_TmpG12D,_TmpG12D,_TmpG12D + 14U},{_TmpG12E,_TmpG12E,_TmpG12E + 11U},{_TmpG12F,_TmpG12F,_TmpG12F + 23U},{_TmpG130,_TmpG130,_TmpG130 + 18U},{_TmpG131,_TmpG131,_TmpG131 + 30U},{_TmpG132,_TmpG132,_TmpG132 + 8U},{_TmpG133,_TmpG133,_TmpG133 + 12U},{_TmpG134,_TmpG134,_TmpG134 + 14U},{_TmpG135,_TmpG135,_TmpG135 + 13U},{_TmpG136,_TmpG136,_TmpG136 + 16U},{_TmpG137,_TmpG137,_TmpG137 + 23U},{_TmpG138,_TmpG138,_TmpG138 + 14U},{_TmpG139,_TmpG139,_TmpG139 + 18U},{_TmpG13A,_TmpG13A,_TmpG13A + 8U},{_TmpG13B,_TmpG13B,_TmpG13B + 8U},{_TmpG13C,_TmpG13C,_TmpG13C + 11U},{_TmpG13D,_TmpG13D,_TmpG13D + 20U},{_TmpG13E,_TmpG13E,_TmpG13E + 14U},{_TmpG13F,_TmpG13F,_TmpG13F + 9U},{_TmpG140,_TmpG140,_TmpG140 + 16U},{_TmpG141,_TmpG141,_TmpG141 + 27U},{_TmpG142,_TmpG142,_TmpG142 + 18U},{_TmpG143,_TmpG143,_TmpG143 + 14U},{_TmpG144,_TmpG144,_TmpG144 + 13U},{_TmpG145,_TmpG145,_TmpG145 + 12U},{_TmpG146,_TmpG146,_TmpG146 + 15U},{_TmpG147,_TmpG147,_TmpG147 + 14U},{_TmpG148,_TmpG148,_TmpG148 + 16U},{_TmpG149,_TmpG149,_TmpG149 + 11U},{_TmpG14A,_TmpG14A,_TmpG14A + 14U},{_TmpG14B,_TmpG14B,_TmpG14B + 11U},{_TmpG14C,_TmpG14C,_TmpG14C + 15U},{_TmpG14D,_TmpG14D,_TmpG14D + 22U},{_TmpG14E,_TmpG14E,_TmpG14E + 16U},{_TmpG14F,_TmpG14F,_TmpG14F + 17U},{_TmpG150,_TmpG150,_TmpG150 + 12U},{_TmpG151,_TmpG151,_TmpG151 + 18U},{_TmpG152,_TmpG152,_TmpG152 + 17U},{_TmpG153,_TmpG153,_TmpG153 + 12U},{_TmpG154,_TmpG154,_TmpG154 + 16U},{_TmpG155,_TmpG155,_TmpG155 + 11U},{_TmpG156,_TmpG156,_TmpG156 + 10U},{_TmpG157,_TmpG157,_TmpG157 + 14U},{_TmpG158,_TmpG158,_TmpG158 + 15U},{_TmpG159,_TmpG159,_TmpG159 + 20U},{_TmpG15A,_TmpG15A,_TmpG15A + 27U},{_TmpG15B,_TmpG15B,_TmpG15B + 32U},{_TmpG15C,_TmpG15C,_TmpG15C + 36U},{_TmpG15D,_TmpG15D,_TmpG15D + 10U},{_TmpG15E,_TmpG15E,_TmpG15E + 18U},{_TmpG15F,_TmpG15F,_TmpG15F + 21U},{_TmpG160,_TmpG160,_TmpG160 + 19U},{_TmpG161,_TmpG161,_TmpG161 + 16U},{_TmpG162,_TmpG162,_TmpG162 + 20U},{_TmpG163,_TmpG163,_TmpG163 + 15U},{_TmpG164,_TmpG164,_TmpG164 + 20U},{_TmpG165,_TmpG165,_TmpG165 + 12U},{_TmpG166,_TmpG166,_TmpG166 + 15U},{_TmpG167,_TmpG167,_TmpG167 + 12U},{_TmpG168,_TmpG168,_TmpG168 + 20U},{_TmpG169,_TmpG169,_TmpG169 + 19U},{_TmpG16A,_TmpG16A,_TmpG16A + 20U},{_TmpG16B,_TmpG16B,_TmpG16B + 21U},{_TmpG16C,_TmpG16C,_TmpG16C + 21U},{_TmpG16D,_TmpG16D,_TmpG16D + 12U},{_TmpG16E,_TmpG16E,_TmpG16E + 17U},{_TmpG16F,_TmpG16F,_TmpG16F + 19U},{_TmpG170,_TmpG170,_TmpG170 + 14U},{_TmpG171,_TmpG171,_TmpG171 + 17U},{_TmpG172,_TmpG172,_TmpG172 + 23U},{_TmpG173,_TmpG173,_TmpG173 + 13U},{_TmpG174,_TmpG174,_TmpG174 + 14U},{_TmpG175,_TmpG175,_TmpG175 + 16U},{_TmpG176,_TmpG176,_TmpG176 + 16U},{_TmpG177,_TmpG177,_TmpG177 + 8U},{_TmpG178,_TmpG178,_TmpG178 + 19U},{_TmpG179,_TmpG179,_TmpG179 + 20U},{_TmpG17A,_TmpG17A,_TmpG17A + 14U},{_TmpG17B,_TmpG17B,_TmpG17B + 19U},{_TmpG17C,_TmpG17C,_TmpG17C + 20U},{_TmpG17D,_TmpG17D,_TmpG17D + 11U},{_TmpG17E,_TmpG17E,_TmpG17E + 22U},{_TmpG17F,_TmpG17F,_TmpG17F + 20U},{_TmpG180,_TmpG180,_TmpG180 + 23U},{_TmpG181,_TmpG181,_TmpG181 + 20U},{_TmpG182,_TmpG182,_TmpG182 + 22U},{_TmpG183,_TmpG183,_TmpG183 + 23U},{_TmpG184,_TmpG184,_TmpG184 + 24U},{_TmpG185,_TmpG185,_TmpG185 + 24U},{_TmpG186,_TmpG186,_TmpG186 + 15U},{_TmpG187,_TmpG187,_TmpG187 + 20U},{_TmpG188,_TmpG188,_TmpG188 + 22U},{_TmpG189,_TmpG189,_TmpG189 + 17U},{_TmpG18A,_TmpG18A,_TmpG18A + 20U},{_TmpG18B,_TmpG18B,_TmpG18B + 26U},{_TmpG18C,_TmpG18C,_TmpG18C + 12U},{_TmpG18D,_TmpG18D,_TmpG18D + 14U},{_TmpG18E,_TmpG18E,_TmpG18E + 12U},{_TmpG18F,_TmpG18F,_TmpG18F + 18U},{_TmpG190,_TmpG190,_TmpG190 + 16U},{_TmpG191,_TmpG191,_TmpG191 + 17U},{_TmpG192,_TmpG192,_TmpG192 + 15U},{_TmpG193,_TmpG193,_TmpG193 + 9U},{_TmpG194,_TmpG194,_TmpG194 + 13U},{_TmpG195,_TmpG195,_TmpG195 + 12U},{_TmpG196,_TmpG196,_TmpG196 + 12U},{_TmpG197,_TmpG197,_TmpG197 + 11U},{_TmpG198,_TmpG198,_TmpG198 + 11U},{_TmpG199,_TmpG199,_TmpG199 + 11U},{_TmpG19A,_TmpG19A,_TmpG19A + 16U},{_TmpG19B,_TmpG19B,_TmpG19B + 17U},{_TmpG19C,_TmpG19C,_TmpG19C + 19U},{_TmpG19D,_TmpG19D,_TmpG19D + 17U},{_TmpG19E,_TmpG19E,_TmpG19E + 19U},{_TmpG19F,_TmpG19F,_TmpG19F + 25U},{_TmpG1A0,_TmpG1A0,_TmpG1A0 + 26U},{_TmpG1A1,_TmpG1A1,_TmpG1A1 + 9U},{_TmpG1A2,_TmpG1A2,_TmpG1A2 + 20U},{_TmpG1A3,_TmpG1A3,_TmpG1A3 + 17U},{_TmpG1A4,_TmpG1A4,_TmpG1A4 + 18U},{_TmpG1A5,_TmpG1A5,_TmpG1A5 + 11U},{_TmpG1A6,_TmpG1A6,_TmpG1A6 + 12U}};
# 1714
static short Cyc_yyr1[591U]={0,170,171,171,171,171,171,171,171,171,171,171,171,172,173,174,175,176,176,177,177,177,178,178,179,179,179,180,180,181,181,182,182,182,183,183,184,184,184,184,185,185,186,187,188,189,190,190,190,190,190,190,191,191,192,192,193,193,193,193,193,193,193,193,193,193,193,194,194,194,194,194,194,194,195,195,196,197,197,198,198,198,198,199,199,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,200,201,202,202,202,203,203,203,204,204,205,205,205,206,206,206,207,207,208,208,209,209,210,210,211,211,212,212,213,213,213,213,214,215,215,215,215,215,215,216,216,216,216,216,216,217,217,218,218,218,218,219,219,220,220,220,221,221,222,222,222,222,223,223,223,224,224,225,225,226,226,227,227,227,227,227,227,227,227,227,228,228,228,228,228,228,228,228,228,228,229,229,230,231,231,232,232,232,232,232,232,232,232,232,232,232,232,233,233,233,- 1,- 1,234,234,234,235,235,236,236,236,237,237,238,238,238,239,239,240,240,240,240,240,241,241,242,242,243,243,244,244,245,245,245,245,246,246,246,247,248,248,248,248,248,249,249,250,250,251,251,252,252,253,253,253,253,254,254,255,255,256,256,256,257,258,258,259,259,260,260,260,260,260,261,261,261,261,262,262,263,263,264,264,265,265,266,266,266,266,266,266,267,267,268,268,268,269,269,269,269,269,269,269,269,269,270,270,270,270,270,270,271,271,272,272,272,272,272,272,273,274,274,275,275,276,276,276,276,276,276,276,276,277,277,277,277,277,277,278,278,278,278,278,278,279,279,279,279,280,280,281,281,281,281,281,281,281,281,282,283,283,284,284,285,285,286,286,287,287,288,288,289,289,290,290,291,291,291,292,292,293,293,294,294,295,295,295,295,295,296,297,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,299,299,299,300,300,301,301,302,302,302,303,303,304,304,305,305,305,306,306,306,306,306,306,306,306,306,306,306,307,307,307,307,307,307,307,307,307,308,309,309,310,310,311,311,312,312,313,313,314,314,315,315,316,316,316,317,317,318,318,319,319,320,320,320,320,321,321,322,322,322,323,323,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,324,325,325,325,325,326,327,327,328,328,328,329,329,330,330,330,331,331,332,333,333,333,334,334,335,335,335,335,335,335,335,335,335,335,335,336,336,336,336,337,337,337,337,337,337,337,337,337,337,337,338,339,339,340,340,340,340,340,341,341,342,342,343,343,344,344,345,345};
# 1776
static short Cyc_yyr2[591U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,4,1,1,1,1,1,1,1,4,1,1,1,4,0,1,2,2,1,0,3,0,1,1,1,3,0,1,1,0,2,2,3,5,5,7,0,1,2,4,0,2,0,2,1,1,3,3,3,3,4,3,1,1,1,1,1,1,4,1,4,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,1,3,1,1,2,3,3,4,4,5,4,5,4,2,4,4,4,5,5,5,0,1,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,8,6,6,6,9,13,11,4,4,6,6,4,2,2,4,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1838
static short Cyc_yydefact[1171U]={0,33,67,68,69,70,72,85,86,87,88,89,90,91,92,93,111,112,113,127,128,63,0,0,98,0,94,0,73,0,0,165,105,108,0,0,0,13,14,0,0,0,0,581,241,583,582,584,0,224,0,101,0,224,223,1,0,0,0,0,31,0,0,32,0,56,65,59,83,61,95,96,0,126,99,0,0,176,0,199,202,100,180,129,71,70,64,0,115,0,58,579,0,581,576,577,578,580,0,129,0,0,409,0,0,0,281,0,413,411,42,44,0,0,52,0,0,0,0,0,125,166,0,242,243,0,0,221,0,0,0,0,222,0,0,0,2,0,0,0,0,46,0,135,137,57,66,60,62,585,586,129,131,129,0,54,0,0,35,0,245,0,188,177,200,0,207,208,212,213,0,0,211,210,0,209,215,231,202,0,84,71,119,0,117,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,565,566,529,0,0,0,527,0,526,524,525,0,438,440,454,464,466,468,470,472,474,476,478,481,483,496,0,498,547,564,562,581,421,0,0,0,0,422,0,0,412,49,0,0,129,0,0,257,259,260,258,0,261,0,146,142,144,301,262,308,303,0,0,0,0,0,8,9,0,0,217,216,0,587,588,241,110,0,0,0,181,102,279,0,276,10,11,0,3,0,5,0,47,0,529,0,0,35,124,0,132,133,158,0,163,0,0,0,0,0,0,0,0,0,0,0,0,581,338,340,0,348,342,0,346,331,332,333,0,334,335,336,0,55,35,137,34,36,309,0,266,282,0,0,247,245,0,226,0,0,0,0,0,233,232,74,229,203,0,120,116,0,0,0,505,0,0,521,456,496,0,457,458,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,499,500,530,520,0,502,0,0,0,0,501,0,97,0,0,0,0,0,0,485,486,0,489,490,487,488,0,0,0,491,492,0,493,494,495,0,0,444,445,446,447,448,449,450,451,452,453,443,0,503,0,553,554,0,0,0,568,0,129,414,0,0,0,435,581,588,0,0,0,0,297,431,436,0,433,0,0,410,0,283,0,0,428,429,0,426,0,0,304,0,274,147,152,148,150,143,145,245,0,311,302,312,590,589,0,104,107,0,51,0,0,106,109,244,80,79,0,77,225,182,245,278,178,311,280,189,190,0,103,16,29,43,0,45,0,136,138,139,285,284,35,37,129,122,134,0,0,0,154,161,0,129,171,0,0,0,0,0,581,0,366,0,369,370,371,0,0,373,0,0,0,349,343,137,347,341,339,38,0,187,267,0,0,0,273,246,268,329,0,236,0,247,186,228,227,183,226,0,0,0,0,234,75,0,130,121,463,118,114,0,0,0,0,581,286,291,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,574,0,573,439,465,0,467,469,471,473,475,477,479,480,482,484,442,441,552,549,0,551,0,0,0,424,425,0,300,0,432,295,298,420,0,296,423,417,48,0,418,0,0,0,305,0,153,149,151,0,0,226,0,313,0,245,0,322,307,0,0,0,0,0,0,146,0,129,0,245,0,198,179,277,0,22,4,6,0,39,247,0,157,141,158,0,0,156,164,173,172,0,0,167,0,0,0,356,0,0,0,366,0,367,368,372,0,0,0,344,337,0,40,310,245,0,270,0,0,0,0,0,330,185,0,248,249,250,0,0,266,237,184,204,206,205,214,234,201,230,504,0,0,0,287,0,292,0,507,0,0,0,0,0,0,0,0,563,515,516,519,0,0,522,523,531,0,0,497,571,0,0,550,548,0,0,0,0,299,434,437,419,427,430,306,263,275,329,314,315,226,0,0,226,0,0,50,218,581,0,76,78,0,191,0,0,0,226,0,0,0,17,23,140,131,155,0,159,162,174,171,171,0,0,0,0,0,0,0,0,0,366,356,374,0,345,41,247,0,271,269,0,0,0,0,0,0,0,0,0,247,0,235,559,0,558,0,288,293,0,459,460,439,439,439,0,0,0,0,461,462,552,551,536,0,572,555,0,575,455,569,570,0,415,416,319,317,321,329,316,226,53,0,81,192,197,329,196,193,226,0,0,0,0,0,0,0,170,169,350,356,0,0,0,0,0,376,377,379,381,383,385,387,389,391,393,396,398,400,402,407,408,0,0,353,362,0,366,0,0,375,239,272,0,0,0,0,251,252,0,264,256,253,254,238,245,506,0,0,294,509,510,511,0,0,0,0,518,517,0,542,536,532,534,528,556,0,320,318,0,195,194,30,25,0,0,35,0,7,123,160,0,0,0,356,0,405,0,0,356,0,0,0,0,0,0,0,0,0,0,0,0,0,403,356,0,0,366,355,323,324,325,255,0,247,561,560,0,0,0,0,0,0,543,542,539,537,0,533,557,0,26,24,0,27,0,19,175,351,352,0,0,0,0,356,358,380,0,382,384,386,388,390,392,394,395,397,399,0,357,363,0,0,326,327,328,0,240,0,0,508,0,0,0,0,545,544,0,538,535,0,28,18,20,0,354,404,0,401,359,0,356,365,0,265,290,289,512,0,0,541,0,540,82,21,0,378,356,360,364,0,0,546,406,361,0,514,0,513,0,0,0};
# 1958
static short Cyc_yydefgoto[176U]={1168,55,56,57,58,530,942,1084,850,851,1024,714,59,347,60,331,61,532,62,534,63,274,156,64,65,601,264,516,517,265,68,285,266,70,182,183,71,72,73,180,309,310,142,143,311,267,495,547,548,726,74,75,730,731,732,76,549,77,522,78,79,177,178,279,80,127,595,364,365,777,688,124,81,354,589,764,765,766,268,269,990,581,585,768,489,355,294,105,106,615,538,616,468,469,470,270,348,349,689,501,761,762,334,335,336,337,338,339,866,340,741,341,954,955,956,957,958,959,960,961,962,963,964,965,966,967,968,969,471,484,485,472,473,474,342,222,448,223,607,224,225,226,227,228,229,230,231,232,233,421,426,431,435,234,235,236,403,404,913,1010,1011,1073,1012,1075,1125,237,893,238,640,641,239,240,83,1025,475,505};
# 1979
static short Cyc_yypact[1171U]={3108,- -32768,- -32768,- -32768,- -32768,- 19,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3944,128,- 44,- -32768,3944,- -32768,950,- -32768,169,44,- -32768,26,74,100,64,119,- -32768,- -32768,76,113,125,225,- -32768,24,- -32768,- -32768,- -32768,135,177,256,197,189,177,- -32768,- -32768,230,255,226,2947,- -32768,243,503,- -32768,348,3944,3944,3944,- -32768,3944,- -32768,- -32768,763,264,- -32768,169,3836,712,49,108,850,- -32768,- -32768,270,287,374,- -32768,169,325,6776,- -32768,- -32768,1742,173,- -32768,- -32768,- -32768,- -32768,327,270,359,6776,- -32768,363,1742,376,375,381,- -32768,302,- -32768,- -32768,4049,4049,389,410,2947,2947,722,6776,- -32768,- -32768,- 65,- -32768,- -32768,404,6776,- -32768,643,405,- 65,4808,- -32768,2947,2947,3268,- -32768,2947,3268,2947,3268,- -32768,95,- -32768,3684,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,270,4808,315,1785,- -32768,3836,348,422,4049,4157,5736,- -32768,712,- -32768,429,- -32768,- -32768,- -32768,- -32768,445,470,- -32768,- -32768,475,- -32768,- -32768,53,850,4049,- -32768,- -32768,492,511,551,169,7251,575,7366,6776,7021,579,596,598,614,616,635,637,644,656,663,674,677,679,683,686,689,698,7366,7366,- -32768,- -32768,724,7481,2615,703,- -32768,7481,- -32768,- -32768,- -32768,158,- -32768,- -32768,- 42,661,694,704,709,591,86,599,- 91,80,- -32768,1396,7481,138,- 34,- -32768,734,105,- -32768,1742,58,749,1266,762,357,1266,- -32768,- -32768,767,6776,270,893,761,- -32768,- -32768,- -32768,- -32768,765,- -32768,4451,4808,4913,4808,836,- -32768,- -32768,- -32768,- 41,- 41,788,775,773,- -32768,- -32768,771,5,- -32768,- -32768,360,- -32768,- -32768,785,- -32768,31,784,779,- -32768,- -32768,617,364,- -32768,- -32768,- -32768,787,- -32768,789,- -32768,799,- -32768,643,724,5866,3836,422,783,805,4808,- -32768,820,169,808,802,176,810,4941,813,837,825,826,5996,2453,4941,172,815,- -32768,- -32768,831,1952,1952,348,1952,- -32768,- -32768,- -32768,840,- -32768,- -32768,- -32768,183,- -32768,422,62,- -32768,- -32768,828,- 20,847,- -32768,41,834,839,212,845,619,833,6776,863,4049,722,- -32768,- -32768,859,841,- -32768,- 20,169,- -32768,6776,856,2615,- -32768,4808,2615,- -32768,- -32768,- -32768,5071,- -32768,888,6776,6776,6776,6776,6776,6776,6776,6776,6776,6776,876,6776,6776,4808,1056,6776,6776,- -32768,- -32768,- -32768,- -32768,855,- -32768,1952,858,392,6776,- -32768,6776,- -32768,7481,6776,7481,7481,7481,7481,- -32768,- -32768,7481,- -32768,- -32768,- -32768,- -32768,7481,7481,7481,- -32768,- -32768,7481,- -32768,- -32768,- -32768,7481,6776,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6776,- -32768,- 65,- -32768,- -32768,6126,- 65,6776,- -32768,864,270,- -32768,862,865,866,- -32768,13,327,- 65,6776,1742,227,- -32768,- -32768,- -32768,869,871,853,1742,- -32768,877,- -32768,465,1266,- -32768,- -32768,881,889,4049,4049,- -32768,891,895,- -32768,4913,4913,4913,- -32768,- -32768,1349,6256,390,- -32768,725,- -32768,- -32768,41,- -32768,- -32768,892,- -32768,913,4049,- -32768,- -32768,- -32768,- -32768,894,896,904,- -32768,- -32768,3373,- -32768,756,622,- -32768,- -32768,- -32768,4808,- -32768,- -32768,1003,- -32768,2947,- -32768,2947,- -32768,905,- -32768,- -32768,- -32768,422,- -32768,270,- -32768,- -32768,1124,6776,476,- -32768,- 36,706,270,747,6776,6776,901,910,6776,900,1026,2286,915,- -32768,- -32768,- -32768,523,1013,- -32768,6386,2119,2777,- -32768,- -32768,3684,- -32768,- -32768,- -32768,- -32768,4049,- -32768,- -32768,4808,909,4556,- -32768,- -32768,902,646,473,- -32768,4661,839,- -32768,- -32768,- -32768,- -32768,619,914,917,185,919,774,- -32768,863,- -32768,- -32768,- -32768,- -32768,- -32768,920,927,922,932,926,- -32768,- -32768,612,5866,416,934,943,945,946,947,953,954,959,426,944,955,957,964,627,965,966,975,974,7136,- -32768,- -32768,969,977,- -32768,661,195,694,704,709,591,86,599,- 91,- 91,80,- -32768,- -32768,- -32768,- -32768,- -32768,982,- -32768,133,4049,5603,4808,- -32768,4808,- -32768,967,- -32768,- -32768,- -32768,- -32768,3321,- -32768,- -32768,- -32768,- -32768,990,- -32768,2927,196,228,- -32768,4808,- -32768,- -32768,- -32768,984,985,619,986,725,4049,4304,6516,- -32768,- -32768,6776,999,240,6906,991,31,3526,994,270,4049,4157,6646,- -32768,756,- -32768,983,1100,- -32768,- -32768,5866,- -32768,839,973,- -32768,- -32768,820,997,6776,- -32768,- -32768,- -32768,- -32768,1004,169,528,441,452,6776,782,490,4941,1000,6776,1007,1008,- -32768,- -32768,1014,1021,1005,1952,- -32768,3836,- -32768,828,1022,4049,- -32768,1023,41,1018,1024,1025,- -32768,- -32768,1027,- -32768,1033,1034,1062,- 47,85,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,774,- -32768,- -32768,1038,321,1038,1035,- -32768,5204,- -32768,7021,- -32768,6776,6776,6776,6776,1167,6776,6776,7021,- -32768,- -32768,- -32768,- -32768,- 65,- 65,- -32768,- -32768,1040,1044,5337,- -32768,- -32768,6776,6776,- -32768,- -32768,- 20,624,1068,1069,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,646,- -32768,- -32768,619,- 20,1047,619,1042,521,- -32768,- -32768,1057,1050,- -32768,- -32768,1052,- -32768,- 20,1053,1054,619,1055,3268,1071,1163,- -32768,- -32768,4808,- -32768,6776,- -32768,- -32768,1065,73,747,4941,1073,1067,1113,1074,1079,4941,6776,1085,6776,782,- -32768,1087,- -32768,- -32768,839,272,- -32768,- -32768,6776,6776,6776,41,473,473,- 12,1095,41,839,4808,- -32768,- -32768,433,- -32768,6776,- -32768,- -32768,5866,- -32768,888,1080,1081,1083,1090,1086,1093,1094,- -32768,888,1089,1092,- 45,1097,- -32768,- -32768,664,- -32768,- -32768,- -32768,- -32768,5603,- -32768,- -32768,- -32768,- -32768,- -32768,646,- -32768,619,- -32768,1115,- -32768,- -32768,- -32768,646,- -32768,- -32768,619,1107,215,1110,2947,1111,1103,4808,- -32768,- -32768,1230,782,1125,7596,1108,2777,1117,- -32768,- 28,- -32768,1154,1118,659,591,86,738,- 91,80,- -32768,- -32768,- -32768,- -32768,1159,7481,1952,- -32768,- -32768,543,6776,1143,1144,- -32768,- -32768,- -32768,1138,1139,1141,1142,- -32768,- -32768,1145,- -32768,- -32768,- -32768,- -32768,- -32768,1022,- -32768,453,280,- -32768,- -32768,- -32768,- -32768,6776,4808,6776,1270,- -32768,- -32768,1146,27,293,- -32768,- -32768,- -32768,- -32768,5470,- -32768,- -32768,1153,- -32768,- -32768,- -32768,- -32768,1155,1156,541,763,- -32768,- -32768,- -32768,573,4941,1157,782,2615,- -32768,4808,1152,1583,7481,6776,7481,7481,7481,7481,7481,7481,7481,7481,7481,7481,6776,- -32768,782,1162,1158,6776,- -32768,646,646,646,- -32768,4049,839,- -32768,- -32768,2777,1160,1161,1165,1164,6776,1175,311,- -32768,- -32768,1176,- -32768,- -32768,1186,- -32768,- -32768,763,- -32768,1172,545,- -32768,- -32768,- -32768,1173,1168,1177,7481,782,- -32768,661,319,694,704,704,591,86,599,- 91,- 91,80,- -32768,335,- -32768,- -32768,4941,1169,- -32768,- -32768,- -32768,337,- -32768,1174,748,- -32768,1170,1300,4808,589,- -32768,1180,1176,- -32768,- -32768,1178,- -32768,- -32768,- -32768,763,- -32768,1038,321,- -32768,- -32768,6776,1583,- -32768,4941,- -32768,- -32768,- -32768,- -32768,1181,1179,- -32768,1196,- -32768,- -32768,- -32768,447,- -32768,782,- -32768,- -32768,4808,1182,- -32768,- -32768,- -32768,1183,- -32768,1184,- -32768,1328,1333,- -32768};
# 2099
static short Cyc_yypgoto[176U]={- -32768,82,- -32768,- -32768,- -32768,- -32768,- -32768,204,- -32768,- -32768,257,- -32768,- -32768,- 298,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 73,- -32768,- 114,37,- -32768,- -32768,0,638,- -32768,136,- 181,1208,32,- -32768,- -32768,- 138,- -32768,- -32768,1302,- 24,491,- -32768,- -32768,1043,1037,269,378,- -32768,626,- -32768,- -32768,- -32768,- 8,- -32768,- -32768,253,- 257,1267,- 429,16,- -32768,1194,- -32768,988,- -32768,1298,- 547,- 319,- -32768,576,- 131,1070,338,- 339,- 540,19,- -32768,- -32768,- 84,- 506,- -32768,584,597,- 312,- 531,- 129,- 485,- 134,- -32768,- 177,- 168,- 626,- 362,- -32768,887,79,- 74,- 123,- 145,- 315,- 171,- 703,- 313,- -32768,- -32768,- 56,- 321,- -32768,- 178,- -32768,- 680,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,156,- -32768,- -32768,705,- 204,- -32768,258,772,- -32768,- 158,- 49,- 166,- 399,- 392,- 383,960,- 390,- 415,- 407,- 384,- 414,415,417,418,419,- 209,493,517,1098,- -32768,- -32768,- -32768,373,- -32768,- 917,312,- -32768,996,261,- -32768,- 417,- -32768,147,345,8,- 71,- 115,- 218};
# 2124
static short Cyc_yytable[7766U]={66,151,293,155,405,559,649,286,410,541,571,572,566,574,643,286,591,653,617,650,158,66,381,645,382,66,356,449,648,353,306,378,69,646,281,521,659,814,271,272,586,598,712,651,652,478,577,372,514,771,128,770,756,69,506,724,368,69,86,66,869,511,90,887,283,66,66,66,502,66,429,155,284,413,430,246,66,162,728,729,150,332,767,343,493,636,1008,1039,307,502,988,69,1074,456,711,164,753,69,69,69,344,69,145,146,147,712,148,89,69,503,1009,582,84,157,502,888,66,66,414,725,504,711,500,924,989,457,578,308,537,314,503,579,1040,66,66,66,67,66,66,66,66,136,243,828,66,19,20,524,69,69,604,747,122,825,66,503,66,67,1008,1128,515,67,244,162,123,69,69,69,- 587,69,69,69,69,111,108,112,69,44,43,853,916,157,1072,107,692,583,69,46,69,44,977,333,697,157,460,67,422,423,276,277,50,67,67,67,304,67,363,116,692,1151,305,461,67,606,580,295,296,297,- 168,298,299,300,301,113,1016,118,654,432,352,114,481,605,1019,343,243,424,425,- 245,302,108,- 245,303,718,450,451,452,433,434,242,115,540,108,67,67,244,49,749,43,332,332,250,332,117,705,119,492,46,53,54,67,67,67,87,67,67,67,67,678,281,411,67,121,925,499,778,928,125,599,705,453,67,812,67,407,454,455,1055,494,43,937,526,617,243,129,411,43,66,46,523,606,412,493,493,493,46,567,- 282,144,863,- 282,526,126,244,568,66,130,576,554,550,411,555,66,66,332,66,657,980,774,69,660,131,411,490,157,43,82,504,221,823,993,810,667,45,46,47,352,69,504,590,248,1022,90,1023,69,69,602,69,88,333,333,135,333,109,133,110,670,1111,282,767,767,824,289,1017,466,467,43,137,138,606,504,108,1020,836,108,46,82,108,48,134,459,292,504,108,49,82,994,66,50,152,82,345,483,681,682,876,53,54,149,668,179,153,312,898,82,981,749,411,874,807,292,889,- 15,184,504,663,1066,700,109,69,1076,785,293,67,333,617,586,254,841,1009,109,283,892,609,255,610,611,280,1126,284,750,67,82,82,841,179,411,1072,67,67,185,67,313,408,82,1139,631,245,43,82,82,82,411,82,82,82,82,46,740,247,1143,1140,141,492,492,492,49,748,721,504,50,155,696,82,411,752,181,411,53,54,477,527,480,512,499,249,366,528,751,719,251,704,252,710,1116,494,494,494,727,706,253,184,526,66,491,66,496,523,273,411,813,852,497,67,582,638,720,275,498,948,257,258,259,260,526,974,287,345,290,526,898,66,550,411,831,69,346,69,606,786,66,830,985,411,66,845,261,992,844,795,312,359,565,283,1064,843,995,573,411,109,996,284,109,69,861,109,919,360,157,411,1162,109,69,763,996,862,69,157,897,677,44,157,411,926,262,715,108,716,583,899,722,900,361,723,108,669,934,362,908,108,909,1101,408,675,776,408,1105,411,525,369,1102,1095,618,867,620,621,622,139,140,82,1097,627,1053,918,898,1100,1063,370,525,551,1098,1099,556,490,1103,1104,894,744,411,67,411,67,859,644,930,860,332,856,343,82,374,877,377,379,379,346,910,911,1082,1132,411,584,1133,696,978,875,1054,67,606,366,371,280,400,401,419,420,67,379,427,428,67,379,710,706,661,184,593,594,1093,1087,527,550,998,606,606,606,1086,375,1083,526,379,383,526,800,451,452,411,550,758,759,550,760,1149,402,815,897,816,43,384,66,385,66,43,728,729,45,46,47,783,784,45,46,47,1052,490,49,386,292,387,520,920,921,1032,43,520,498,415,53,54,453,498,69,46,69,801,455,333,388,157,389,292,49,16,17,18,50,390,292,1141,257,258,259,260,53,54,864,865,944,391,776,1014,1015,733,734,109,392,737,1030,377,742,1156,108,109,1043,1044,278,393,109,108,394,1158,395,982,983,984,396,43,822,397,897,292,398,1106,584,45,46,47,1047,1048,66,399,292,946,947,48,409,1089,416,545,44,292,1094,159,262,160,379,525,48,417,525,161,685,686,687,418,693,1108,694,82,458,82,69,1065,695,1137,67,462,67,1112,1113,1114,525,476,43,1145,411,525,479,48,288,332,45,46,47,986,987,707,379,708,379,379,379,379,486,709,379,1138,487,507,508,379,379,379,510,509,379,123,584,542,379,518,939,519,529,358,531,91,165,166,167,168,169,366,66,170,533,171,172,173,150,43,543,174,552,553,175,1085,834,45,46,47,379,557,1157,292,560,332,561,49,562,563,569,545,66,491,69,546,580,292,1163,53,54,1155,570,49,67,575,587,497,1115,504,176,596,91,498,588,53,54,592,742,44,48,608,603,413,69,628,635,92,674,333,637,108,664,662,672,665,666,673,109,482,970,894,241,94,1027,109,95,96,97,676,98,46,1037,679,150,680,683,99,66,379,100,684,699,698,101,701,102,103,702,525,703,713,735,717,736,738,739,104,743,745,754,782,1085,757,92,525,772,332,525,773,69,775,779,780,781,333,858,539,67,93,94,- 587,1068,95,96,97,787,98,46,788,150,789,790,791,99,584,796,100,91,792,793,101,43,102,103,794,804,797,67,798,45,46,47,1090,104,1091,799,802,803,805,49,808,312,809,545,817,975,848,742,201,379,597,53,54,811,820,826,827,951,952,66,150,835,839,829,1117,842,849,855,857,870,868,539,997,411,91,619,873,871,292,623,624,625,626,872,352,629,630,878,880,633,634,69,886,67,881,882,333,883,884,885,639,806,642,895,43,94,210,211,95,96,97,905,82,46,912,914,922,923,927,929,1148,932,931,933,935,936,376,655,109,215,248,941,938,292,940,945,949,379,950,656,584,584,584,973,639,584,92,976,972,979,991,999,1000,742,1001,1002,1003,1164,1004,1005,1006,241,94,1007,1018,95,96,97,1013,98,46,1021,43,1026,1029,1028,99,1031,1036,100,45,46,47,953,48,102,103,1041,1033,691,49,1038,1051,216,545,67,104,218,379,1042,219,220,53,54,149,1056,82,379,1057,1058,1059,408,1060,1061,1070,1062,1071,1096,1079,1080,1081,1088,379,1092,1109,1124,1008,91,1107,1110,1121,1119,1120,742,1122,1129,1131,1134,1144,1135,1142,1146,1136,1118,1147,1150,1161,1169,1123,1152,1160,1159,1170,1165,1166,1167,1153,291,1130,639,840,639,120,943,163,535,544,379,854,600,132,891,890,879,513,671,7,8,9,10,11,12,13,14,15,16,17,18,19,20,149,367,379,379,379,1045,647,819,1046,92,971,1049,1077,1050,1127,22,23,24,539,26,0,463,632,0,464,94,1154,0,95,96,97,536,465,46,31,0,0,0,0,99,0,0,100,0,32,33,101,0,102,103,0,466,467,0,0,0,149,0,39,104,0,0,40,0,539,0,0,0,0,0,0,0,0,1035,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,436,0,0,379,0,0,833,0,0,0,0,0,838,0,350,0,0,149,0,0,847,0,0,0,44,45,0,47,539,48,0,352,0,0,0,49,0,0,0,497,0,51,52,0,0,498,639,53,54,0,0,437,438,439,440,441,442,443,444,445,446,0,0,0,0,0,0,0,0,0,0,379,0,379,379,379,379,379,379,379,379,379,379,447,0,0,0,0,0,0,0,0,0,0,0,539,0,0,0,901,902,903,904,0,906,907,0,0,0,0,0,0,0,0,0,0,0,539,0,0,917,0,0,0,379,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,864,865,21,186,187,315,0,316,317,318,319,320,321,322,323,22,23,24,324,26,91,27,189,325,0,0,0,379,190,191,28,326,0,0,31,192,193,194,195,196,197,198,199,0,32,33,34,200,0,0,0,201,0,0,0,0,0,0,39,202,203,204,40,205,0,539,0,0,0,0,0,0,0,0,0,0,41,206,207,0,0,42,0,0,208,209,0,0,539,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,327,94,210,211,95,96,97,44,45,46,47,0,48,212,328,154,0,0,213,0,0,0,214,0,51,330,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1067,0,1069,0,0,0,0,0,0,0,91,0,0,539,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,186,187,315,0,316,317,318,319,320,321,322,323,22,23,24,324,26,91,27,189,325,0,0,0,0,190,191,28,326,0,0,31,192,193,194,195,196,197,198,199,0,32,33,34,200,0,92,0,201,0,0,0,0,0,0,39,202,203,204,40,205,241,94,0,0,95,96,97,0,98,46,0,0,41,206,207,99,0,42,100,0,208,209,101,0,102,103,0,0,0,0,0,0,0,0,0,104,0,0,0,0,0,0,0,327,94,210,211,95,96,97,44,45,46,47,0,48,212,328,154,329,0,213,0,0,0,214,0,51,330,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,186,187,315,0,316,317,318,319,320,321,322,323,22,23,24,324,26,91,27,189,325,0,0,0,0,190,191,28,326,0,0,31,192,193,194,195,196,197,198,199,0,32,33,34,200,0,0,0,201,0,0,0,0,0,0,39,202,203,204,40,205,0,0,0,0,0,0,0,0,0,0,0,0,41,206,207,0,0,42,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,327,94,210,211,95,96,97,44,45,46,47,0,48,212,328,154,0,0,213,0,0,0,214,0,51,330,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,186,187,315,0,316,317,318,319,320,321,322,323,22,23,24,324,26,91,27,189,325,0,0,0,0,190,191,28,326,0,0,31,192,193,194,195,196,197,198,199,0,32,33,34,200,0,0,0,201,0,0,0,0,0,0,39,202,203,204,40,205,0,0,0,0,0,0,0,0,0,0,0,0,41,206,207,0,0,42,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,558,94,210,211,95,96,97,44,45,46,47,0,48,212,328,154,0,0,213,0,0,0,214,0,51,330,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,186,187,0,0,0,0,0,0,0,0,0,0,22,23,24,324,26,91,27,189,0,0,0,0,0,190,191,28,0,0,0,31,192,193,194,195,196,197,198,199,0,32,33,34,200,0,0,0,201,0,0,0,0,0,0,39,202,203,204,40,205,0,0,0,0,0,0,0,0,0,0,0,0,41,206,207,0,0,42,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,44,45,46,47,0,48,212,0,0,0,0,213,0,0,0,214,0,51,330,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,186,187,0,0,0,0,0,0,0,0,0,0,22,23,24,324,26,91,0,0,0,0,0,0,0,0,0,28,0,0,0,31,192,193,194,195,196,197,198,199,0,32,33,0,0,0,0,0,201,0,0,0,0,0,0,39,202,203,204,40,205,0,0,0,0,0,0,0,0,0,0,0,0,41,206,207,0,0,42,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,44,45,46,47,0,48,212,0,0,0,0,213,0,0,0,376,0,51,330,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,22,23,24,188,26,91,0,189,0,0,0,0,0,190,191,0,0,0,0,31,192,193,194,195,196,197,198,199,0,32,33,0,200,0,0,0,201,0,0,0,0,0,0,39,202,203,204,40,205,0,0,0,0,0,0,0,0,0,0,0,0,41,206,207,0,0,42,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,44,45,46,47,0,48,212,0,406,0,0,213,0,0,0,214,0,51,330,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,22,23,24,188,26,91,0,189,0,0,0,0,0,190,191,0,0,0,0,31,192,193,194,195,196,197,198,199,0,32,33,0,200,0,0,0,201,0,0,0,0,0,0,39,202,203,204,40,205,0,0,0,0,0,0,0,0,0,0,0,0,41,206,207,0,0,42,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,44,45,46,47,0,48,212,0,0,0,0,213,0,0,0,214,0,51,330,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,91,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,92,0,0,0,41,0,0,0,0,42,0,0,821,0,0,241,94,0,0,95,96,97,0,98,46,0,0,0,0,0,99,0,0,100,0,43,0,101,0,102,103,0,44,45,46,47,0,48,0,0,104,- 12,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,0,0,0,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,91,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,22,23,24,- 12,26,49,0,0,0,50,0,51,52,0,0,0,0,53,54,31,0,0,0,0,0,92,0,0,0,32,33,0,0,0,0,0,0,818,0,0,464,94,0,39,95,96,97,40,465,46,0,0,0,0,0,99,0,0,100,0,0,41,101,0,102,103,42,466,467,0,0,0,0,0,0,104,0,0,0,0,0,0,0,0,0,0,0,0,0,350,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,352,0,0,0,49,0,0,0,520,0,51,52,0,0,498,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,0,0,0,0,49,0,0,0,545,0,51,52,0,0,0,0,53,54,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,304,0,154,0,0,0,305,0,0,0,0,51,52,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,44,45,0,47,0,48,0,0,154,22,23,24,25,26,0,0,0,51,52,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,0,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,256,0,0,0,0,0,257,258,259,260,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,261,42,0,0,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,44,45,0,47,262,48,0,0,263,22,23,24,0,26,0,0,0,51,52,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,350,0,0,351,0,0,0,0,0,0,44,45,0,47,0,48,0,352,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,350,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,352,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,488,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,755,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,769,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,0,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,0,26,0,0,0,0,0,0,51,52,0,0,0,186,187,315,31,316,317,318,319,320,321,322,323,0,32,33,188,0,91,0,189,325,0,0,0,0,190,191,39,326,0,0,40,192,193,194,195,196,197,198,199,0,0,0,0,200,41,0,0,201,0,42,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,44,0,0,0,0,48,0,0,0,0,0,0,0,0,0,0,0,51,52,0,0,558,94,210,211,95,96,97,0,0,46,0,0,0,212,328,154,0,0,213,0,0,0,214,0,0,215,186,187,0,0,0,0,216,612,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,613,94,210,211,95,96,97,0,284,46,0,0,0,212,0,380,614,0,213,0,0,0,214,0,0,215,0,466,467,186,187,0,216,0,0,217,218,0,0,219,220,0,0,0,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,613,94,210,211,95,96,97,0,284,46,0,0,0,212,0,380,896,0,213,0,0,0,214,0,0,215,0,466,467,186,187,0,216,0,0,217,218,0,0,219,220,0,0,0,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,613,94,210,211,95,96,97,0,284,46,0,0,0,212,0,380,915,0,213,0,0,0,214,0,0,215,0,466,467,186,187,0,216,0,0,217,218,0,0,219,220,0,0,0,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,613,94,210,211,95,96,97,0,284,46,0,0,0,212,0,380,1078,0,213,0,0,0,214,0,0,215,0,466,467,186,187,0,216,0,0,217,218,0,0,219,220,0,0,0,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,613,94,210,211,95,96,97,0,284,46,0,0,0,212,0,380,0,0,213,0,0,0,214,0,0,215,0,466,467,186,187,0,216,0,0,217,218,0,0,219,220,0,0,0,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,0,0,0,213,0,0,0,214,0,0,215,186,187,0,357,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,380,0,0,213,0,0,0,214,0,0,215,186,187,0,0,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,564,0,0,0,213,0,0,0,214,0,0,215,186,187,0,0,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,0,0,0,213,0,0,0,214,658,0,215,186,187,0,0,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,0,0,0,213,0,0,0,214,0,0,215,186,187,0,690,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,0,0,0,213,0,0,0,214,746,0,215,186,187,0,0,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,0,0,0,213,0,0,0,214,0,0,215,186,187,0,832,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,0,0,0,213,0,0,0,214,0,0,215,186,187,0,846,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,0,0,0,213,0,0,0,214,0,0,215,186,187,0,0,0,0,216,0,0,217,218,0,0,219,220,188,0,91,0,189,0,0,0,0,0,190,191,0,0,0,0,0,192,193,194,195,196,197,198,199,0,0,0,0,200,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,837,94,210,211,95,96,97,0,0,46,0,186,187,212,0,0,0,0,213,0,0,0,214,0,0,215,188,0,91,0,0,0,216,0,0,217,218,0,0,219,220,0,0,192,193,194,195,196,197,198,199,0,0,0,0,0,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,186,187,212,0,380,0,0,213,0,0,0,214,0,0,215,188,0,91,0,0,0,216,0,0,217,218,0,0,219,220,0,0,192,193,194,195,196,197,198,199,0,0,0,0,0,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,186,187,212,0,806,0,0,213,0,0,0,214,0,0,215,188,0,91,0,0,0,216,0,0,217,218,0,0,219,220,0,0,192,193,194,195,196,197,198,199,0,0,0,0,0,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,186,187,212,0,0,0,0,213,0,0,0,373,0,0,215,188,0,91,0,0,0,216,0,0,217,218,0,0,219,220,0,0,192,193,194,195,196,197,198,199,0,0,0,0,0,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,186,187,212,0,0,0,0,213,0,0,0,376,0,0,215,188,0,91,0,0,0,216,0,0,217,218,0,0,219,220,0,0,192,193,194,195,196,197,198,199,0,0,0,0,0,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,186,187,212,0,0,0,0,213,0,0,0,214,0,0,215,188,0,91,0,0,0,216,0,0,217,218,0,0,219,220,0,0,192,193,194,195,196,197,198,199,0,0,0,0,0,0,0,0,201,0,0,0,0,0,0,0,202,203,204,0,205,0,0,0,0,0,0,0,0,0,0,0,0,0,206,207,0,0,0,0,0,208,209,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,210,211,95,96,97,0,0,46,0,0,0,212,0,0,0,0,213,0,0,0,1034,0,0,215,0,0,0,0,0,0,216,0,0,217,218,0,0,219,220};
# 2904
static short Cyc_yycheck[7766U]={0,72,131,76,213,318,421,122,217,307,331,332,325,334,413,130,355,431,380,426,76,21,190,415,190,25,160,236,418,160,144,189,0,416,118,292,453,663,112,113,352,360,527,427,428,249,344,185,17,596,50,591,583,21,272,91,179,25,21,59,740,279,25,110,129,65,66,67,109,69,161,144,137,115,165,99,76,77,5,6,72,154,588,156,265,406,131,115,144,109,102,59,1009,127,523,79,581,65,66,67,156,69,65,66,67,590,69,151,76,150,155,70,131,76,109,162,116,117,160,155,161,550,267,826,136,159,146,151,305,153,150,349,160,133,134,135,0,137,138,139,140,59,129,690,144,20,21,292,116,117,368,568,128,684,154,150,156,21,131,1076,129,25,149,163,140,133,134,135,155,137,138,139,140,129,27,149,144,136,129,719,806,144,155,27,499,144,154,138,156,136,870,154,504,156,136,59,110,111,116,117,151,65,66,67,142,69,153,143,523,1126,148,153,76,371,129,133,134,135,145,137,138,139,140,149,927,149,435,147,143,129,254,369,935,306,129,149,150,152,143,92,155,146,540,105,106,107,166,167,92,149,306,104,116,117,149,147,569,129,331,332,104,334,143,520,151,265,138,159,160,133,134,135,144,137,138,139,140,481,362,146,144,56,829,267,603,832,151,361,545,151,154,158,156,214,156,157,976,265,129,846,292,663,129,50,146,129,306,138,292,467,152,492,493,494,138,143,143,64,735,146,312,144,149,151,324,128,143,151,312,146,154,331,332,406,334,450,876,152,306,454,151,146,263,306,129,0,161,89,152,889,155,466,137,138,139,143,324,161,146,101,145,324,147,331,332,365,334,22,331,332,144,334,27,143,29,148,1056,119,884,885,152,128,929,156,157,129,143,144,546,161,243,938,152,246,138,50,249,141,143,243,131,161,255,147,59,890,406,151,144,64,157,255,486,487,753,159,160,72,467,149,75,152,784,78,152,738,146,748,637,160,769,144,87,161,458,155,510,92,406,146,617,570,306,406,806,757,144,704,155,104,129,130,373,151,375,376,118,146,137,573,324,116,117,720,149,146,155,331,332,144,334,156,214,128,155,396,149,129,133,134,135,146,137,138,139,140,138,560,129,152,155,143,492,493,494,147,569,546,161,151,573,501,157,146,578,131,146,159,160,152,146,253,152,497,151,177,152,573,542,143,520,146,522,1063,492,493,494,551,520,148,185,523,532,264,534,266,520,148,146,662,717,151,406,70,152,545,136,157,861,76,77,78,79,545,867,151,303,152,550,921,560,545,146,694,532,143,534,725,152,569,693,883,146,573,708,102,888,708,152,310,151,323,129,130,707,152,333,146,243,156,137,246,560,152,249,813,151,560,146,152,255,569,129,156,152,573,569,784,143,136,573,146,830,140,532,468,534,144,786,143,786,151,146,476,468,843,151,795,481,795,1045,373,476,601,376,1049,146,292,146,1046,1039,383,152,385,386,387,143,144,303,1041,392,972,810,1015,1044,994,145,312,313,1042,1043,316,583,1047,1048,780,143,146,532,146,534,143,414,152,146,748,725,750,333,186,754,188,189,190,143,800,801,146,143,146,352,146,692,871,750,152,560,855,360,148,362,208,209,112,113,569,213,108,109,573,217,711,704,455,369,96,97,1038,1031,146,704,898,880,881,882,152,151,1025,720,236,151,723,105,106,107,146,720,91,92,723,94,152,18,664,921,666,129,151,748,151,750,129,5,6,137,138,139,145,146,137,138,139,971,684,147,151,497,151,151,145,146,949,129,151,157,114,159,160,151,157,748,138,750,156,157,748,151,750,151,520,147,17,18,19,151,151,527,1110,76,77,78,79,159,160,22,23,855,151,776,145,146,553,554,468,151,557,945,324,560,1140,673,476,163,164,102,151,481,680,151,1142,151,880,881,882,151,129,680,151,1015,570,151,1050,504,137,138,139,108,109,848,151,581,859,860,141,151,1033,162,151,136,590,1038,149,140,151,371,520,141,163,523,157,492,493,494,164,149,1053,151,532,144,534,848,996,157,1092,748,136,750,1058,1059,1060,545,129,129,145,146,550,129,141,126,972,137,138,139,884,885,149,413,151,415,416,417,418,151,157,421,1093,151,129,143,426,427,428,151,150,431,140,588,144,435,145,848,152,145,161,145,42,86,87,88,89,90,603,942,93,145,95,96,97,940,129,145,101,144,151,104,1026,698,137,138,139,467,151,1140,694,151,1038,129,147,143,143,155,151,972,704,942,155,129,708,1156,159,160,1139,151,147,848,145,152,151,1062,161,140,158,42,157,155,159,160,152,740,136,141,145,161,115,972,129,151,114,155,972,152,864,150,149,145,150,150,146,673,126,864,1136,129,130,942,680,133,134,135,152,137,138,953,152,1026,146,145,144,1038,546,147,146,129,151,151,151,153,154,152,704,146,48,151,148,144,155,30,164,143,46,151,129,1133,161,114,720,152,1140,723,152,1038,152,152,146,152,1038,731,305,942,129,130,155,1003,133,134,135,152,137,138,146,1082,146,146,146,144,757,152,147,42,146,146,151,129,153,154,146,131,152,972,152,137,138,139,1034,164,1036,152,152,152,145,147,152,853,146,151,158,868,144,870,73,637,359,159,160,152,145,152,152,25,26,1140,1133,143,152,158,1066,152,47,151,145,143,151,380,895,146,42,384,152,144,890,388,389,390,391,143,143,394,395,145,151,398,399,1140,111,1038,151,151,1140,151,146,146,409,144,411,149,129,130,131,132,133,134,135,25,848,138,155,152,129,129,152,158,1122,152,146,152,152,152,151,436,864,154,953,49,158,945,144,151,144,725,152,448,883,884,885,145,453,888,114,143,155,143,136,152,152,976,152,146,151,1159,146,146,152,129,130,152,130,133,134,135,152,137,138,145,129,144,152,145,144,28,151,147,137,138,139,151,141,153,154,114,144,498,147,155,114,161,151,1140,164,165,786,162,168,169,159,160,940,143,942,795,145,152,152,1034,152,152,25,151,151,1040,146,145,145,145,810,152,143,131,131,42,1051,152,146,152,152,1056,151,130,145,145,145,152,152,152,146,1066,25,146,131,0,1071,152,152,151,0,152,152,152,1133,130,1082,568,703,570,41,853,78,303,310,855,723,362,53,776,769,757,285,469,8,9,10,11,12,13,14,15,16,17,18,19,20,21,1026,178,880,881,882,961,417,673,962,114,864,964,1010,965,1073,37,38,39,617,41,- 1,126,397,- 1,129,130,1136,- 1,133,134,135,304,137,138,56,- 1,- 1,- 1,- 1,144,- 1,- 1,147,- 1,66,67,151,- 1,153,154,- 1,156,157,- 1,- 1,- 1,1082,- 1,80,164,- 1,- 1,84,- 1,663,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,951,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,- 1,971,- 1,- 1,695,- 1,- 1,- 1,- 1,- 1,701,- 1,126,- 1,- 1,1133,- 1,- 1,709,- 1,- 1,- 1,136,137,- 1,139,717,141,- 1,143,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,157,735,159,160,- 1,- 1,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1039,- 1,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,784,- 1,- 1,- 1,788,789,790,791,- 1,793,794,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,806,- 1,- 1,809,- 1,- 1,- 1,1092,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,1139,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,85,- 1,898,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,921,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,136,137,138,139,- 1,141,142,143,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1002,- 1,1004,- 1,- 1,- 1,- 1,- 1,- 1,- 1,42,- 1,- 1,1015,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,114,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,85,129,130,- 1,- 1,133,134,135,- 1,137,138,- 1,- 1,98,99,100,144,- 1,103,147,- 1,106,107,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,136,137,138,139,- 1,141,142,143,144,145,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,136,137,138,139,- 1,141,142,143,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,136,137,138,139,- 1,141,142,143,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,136,137,138,139,- 1,141,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,136,137,138,139,- 1,141,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,136,137,138,139,- 1,141,142,- 1,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,99,100,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,136,137,138,139,- 1,141,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,42,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,126,- 1,- 1,129,130,- 1,- 1,133,134,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,- 1,129,- 1,151,- 1,153,154,- 1,136,137,138,139,- 1,141,- 1,- 1,164,145,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,159,160,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,- 1,136,137,138,139,- 1,141,- 1,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,159,160,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,42,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,- 1,136,137,138,139,- 1,141,37,38,39,145,41,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,159,160,56,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,130,- 1,80,133,134,135,84,137,138,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,- 1,- 1,98,151,- 1,153,154,103,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,- 1,136,137,138,139,- 1,141,- 1,143,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,157,- 1,159,160,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,- 1,136,137,138,139,- 1,141,- 1,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,153,154,- 1,- 1,- 1,- 1,159,160,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,- 1,141,142,- 1,144,- 1,- 1,- 1,148,- 1,- 1,- 1,- 1,153,154,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,136,137,- 1,139,- 1,141,- 1,- 1,144,37,38,39,40,41,- 1,- 1,- 1,153,154,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,- 1,141,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,76,77,78,79,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,140,141,- 1,- 1,144,37,38,39,- 1,41,- 1,- 1,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,- 1,141,- 1,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,153,154,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,- 1,141,- 1,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,153,154,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,- 1,141,37,38,39,145,41,- 1,- 1,- 1,- 1,- 1,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,- 1,141,37,38,39,145,41,- 1,- 1,- 1,- 1,- 1,- 1,153,154,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,- 1,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,153,154,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,137,- 1,139,- 1,141,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,153,154,- 1,- 1,- 1,25,26,27,56,29,30,31,32,33,34,35,36,- 1,66,67,40,- 1,42,- 1,44,45,- 1,- 1,- 1,- 1,50,51,80,53,- 1,- 1,84,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,98,- 1,- 1,73,- 1,103,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,136,- 1,- 1,- 1,- 1,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,153,154,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,143,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,25,26,- 1,- 1,- 1,- 1,161,32,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,137,138,- 1,- 1,- 1,142,- 1,144,145,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,- 1,156,157,25,26,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,137,138,- 1,- 1,- 1,142,- 1,144,145,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,- 1,156,157,25,26,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,137,138,- 1,- 1,- 1,142,- 1,144,145,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,- 1,156,157,25,26,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,137,138,- 1,- 1,- 1,142,- 1,144,145,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,- 1,156,157,25,26,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,137,138,- 1,- 1,- 1,142,- 1,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,- 1,156,157,25,26,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,25,26,- 1,158,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,25,26,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,143,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,25,26,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,152,- 1,154,25,26,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,25,26,- 1,158,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,152,- 1,154,25,26,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,25,26,- 1,158,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,25,26,- 1,158,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,25,26,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,25,26,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,40,- 1,42,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,25,26,142,- 1,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,40,- 1,42,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,25,26,142,- 1,144,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,40,- 1,42,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,25,26,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,40,- 1,42,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,25,26,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,40,- 1,42,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,25,26,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,40,- 1,42,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,83,- 1,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,99,100,- 1,- 1,- 1,- 1,- 1,106,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,131,132,133,134,135,- 1,- 1,138,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,151,- 1,- 1,154,- 1,- 1,- 1,- 1,- 1,- 1,161,- 1,- 1,164,165,- 1,- 1,168,169};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int,int);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*,struct Cyc_Yyltype*);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple33{unsigned f0;struct _tuple0*f1;int f2;};struct _tuple34{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _Tmp0=_new_region(0U,"yyregion");struct _RegionHandle*yyregion=& _Tmp0;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _Tmp1;_Tmp1.YYINITIALSVAL.tag=76U,_Tmp1.YYINITIALSVAL.val=0;_Tmp1;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _Tmp1=200U;_tag_fat(_region_calloc(yyregion,0U,sizeof(short),_Tmp1),sizeof(short),_Tmp1);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _Tmp1=200U;({struct Cyc_Yystacktype*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(struct Cyc_Yystacktype)));});({{unsigned _Tmp3=200U;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i].v=yylval,({struct Cyc_Yyltype _Tmp4=Cyc_yynewloc();_Tmp2[i].l=_Tmp4;});}}0;});_Tmp2;});}),sizeof(struct Cyc_Yystacktype),200U);
# 174
struct Cyc_Yystacktype*yyyvsp;
# 177
int yystacksize=200;
# 179
union Cyc_YYSTYPE yyval=yylval;
# 183
int yylen;
# 190
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 200
yyssp_offset=-1;
yyvsp_offset=0;
# 206
yynewstate:
# 208
*((short*)_check_fat_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 210
if(yyssp_offset >= (yystacksize - 1)- 14){
# 212
if(yystacksize >= 10000){
Cyc_yyerror(_tag_fat("parser stack overflow",sizeof(char),22U),yystate,yychar);
_throw(& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _Tmp1=(unsigned)yystacksize;_tag_fat(({short*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(short)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=((short*)yyss.curr)[(int)i]:(_Tmp2[i]=0);}}0;});_Tmp2;}),sizeof(short),_Tmp1);});
# 222
struct _fat_ptr yyvs1=({unsigned _Tmp1=(unsigned)yystacksize;
_tag_fat(({struct Cyc_Yystacktype*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(struct Cyc_Yystacktype)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_Tmp2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_Tmp2;}),sizeof(struct Cyc_Yystacktype),_Tmp1);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1171U,sizeof(short),yystate));
if(yyn==-32768)goto yydefault;
# 261
if(yychar==-2)
# 267
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 271
if(yychar <= 0){
# 273
yychar1=0;
yychar=0;}else{
# 282
yychar1=yychar > 0 && yychar <= 397?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,398U,sizeof(short),yychar)): 346;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 7765)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7766U,sizeof(short),yyn))!=yychar1)goto yydefault;
# 302
yyn=(int)Cyc_yytable[yyn];
# 309
if(yyn < 0){
# 311
if(yyn==-32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 315
if(yyn==0)goto yyerrlab;}
# 317
if(yyn==1170){
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 328 "cycbison.simple"
if(yychar!=0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_Tmp1;});
# 338
if(yyerrstatus!=0)-- yyerrstatus;
# 340
yystate=yyn;
goto yynewstate;
# 344
yydefault:
# 346
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefact,1171U,sizeof(short),yystate));
if(yyn==0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,591U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr_check_bound(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),14U));
if(yylen > 0)
yyval=yyyvsp[0].v;
# 370 "cycbison.simple"
switch((int)yyn){case 1:
# 1241 "parse.y"
 yyval=yyyvsp[0].v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(& yyyvsp[0].v);
# 1244
goto _LL0;case 2:
# 1247 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 3:
# 1251 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_using();
# 1254
goto _LL0;case 4:
# 1255 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 5:
# 1258
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_namespace();
# 1261
goto _LL0;case 6:
# 1262 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 7:  {
# 1264 "parse.y"
int is_c_include=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(& yyyvsp[4].v);
struct _tuple30*_Tmp1=Cyc_yyget_YY56(& yyyvsp[5].v);unsigned _Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*exs=_Tmp3;unsigned wc=_Tmp2;
struct Cyc_List_List*hides=Cyc_yyget_YY57(& yyyvsp[6].v);
if(exs!=0 && hides!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U),_tag_fat(0U,sizeof(void*),0));
if((unsigned)hides && !((int)wc))
wc=yyyvsp[6].l.first_line;
if(!is_c_include){
if(exs!=0 || cycdecls!=0){
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("expecting \"C include\"",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f0=wc,_TmpA->f1=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}else{
# 1278
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_Tmp8->tag=11,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}}else{
# 1281
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f0=wc,_TmpA->f1=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}
# 1283
goto _LL0;}}case 8:
# 1284 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Porton_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 9:
# 1286 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Portoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 10:
# 1288 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempeston_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 11:
# 1290 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 12:
# 1291 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13:
# 1295 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14:
# 1298
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15:
# 1303 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})==0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C include",sizeof(char),10U));})==0)
yyval=Cyc_YY32(1);else{
# 1309
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY32(1);}}
# 1313
goto _LL0;case 16:
# 1316 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17:
# 1320 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 18:
# 1321 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 19:
# 1325 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 20:
# 1326 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 21:
# 1327 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY57(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 22:
# 1331 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 23:
# 1332 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 24:
# 1336 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY55(& yyyvsp[2].v);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 25:
# 1337 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 26:
# 1338 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,_Tmp1->f1=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 27:
# 1343 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));_Tmp3->f0=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 28:
# 1345 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));_Tmp3->f0=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY55(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 29:
# 1349 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 30:
# 1350 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 31:
# 1354 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp5=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_new_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 32:
# 1355 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 33:
# 1356 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 36:
# 1366 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 37:  {
# 1368 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 38:
# 1380 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 39:  {
# 1382 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 40:  {
# 1390 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 41:  {
# 1393 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 42:
# 1398 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(& yyyvsp[1].v));yyval=yyyvsp[1].v;
goto _LL0;case 43:
# 1401
 Cyc_Lex_leave_using();
goto _LL0;case 44:
# 1404
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp1=_Tmp2;});_Tmp1;}));yyval=yyyvsp[1].v;
goto _LL0;case 45:
# 1407
 Cyc_Lex_leave_namespace();
goto _LL0;case 46:
# 1413 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);unsigned _Tmp2=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,0,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 47:
# 1415 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);struct _tuple11*_Tmp2=({struct _tuple11*(*_Tmp3)(struct _tuple11*)=({struct _tuple11*(*_Tmp4)(struct _tuple11*)=(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;_Tmp4;});_Tmp3(Cyc_yyget_YY19(& yyyvsp[1].v));});unsigned _Tmp3=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 48:
# 1418
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_let_decl(_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 49:  {
# 1420 "parse.y"
struct Cyc_List_List*vds=0;
{struct Cyc_List_List*ids=Cyc_yyget_YY37(& yyyvsp[1].v);for(0;ids!=0;ids=ids->tl){
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0)),({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);qv->f0=_Tmp1;}),qv->f1=(struct _fat_ptr*)ids->hd;
vds=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Vardecl*_Tmp2=({struct _tuple0*_Tmp3=qv;Cyc_Absyn_new_vardecl(0U,_Tmp3,Cyc_Absyn_wildtyp(0),0,0);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=vds;_Tmp1;});}}
# 1425
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(vds);Cyc_Absyn_letv_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1427
goto _LL0;}case 50:
# 1430 "parse.y"
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[2].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ek);tv->kind=_Tmp1;}),tv->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[4].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0,0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=Cyc_Absyn_region_decl(tv,vd,0,yyyvsp[0].l.first_line);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1436
goto _LL0;}}case 51:  {
# 1438
struct _fat_ptr tvstring=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp2,sizeof(void*),1));});
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[1].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=tvstring;_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ek);tv->kind=_Tmp1;}),tv->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[1].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0,0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Tvar*_Tmp3=tv;struct Cyc_Absyn_Vardecl*_Tmp4=vd;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_YY61(& yyyvsp[2].v);Cyc_Absyn_region_decl(_Tmp3,_Tmp4,_Tmp5,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1445
goto _LL0;}}}case 52:
# 1448 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 53:
# 1450 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("open",sizeof(char),5U));})!=0)Cyc_Warn_err(yyyvsp[3].l.first_line,_tag_fat("expecting `open'",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY61(Cyc_yyget_Exp_tok(& yyyvsp[3].v));
goto _LL0;case 54:
# 1454
 yyval=yyyvsp[0].v;
goto _LL0;case 55:
# 1455 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 56:
# 1461 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 57:  {
# 1464 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
if((int)two.sc!=7)
Cyc_Warn_warn(yyyvsp[0].l.first_line,
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1471
goto _LL0;}case 58:
# 1472 "parse.y"
 Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));
yyval=yyyvsp[1].v;
# 1475
goto _LL0;case 59:
# 1476 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 60:  {
# 1479 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,({
struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=two.type_specs;Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,Cyc_yyget_YY21(& yyyvsp[0].v));});_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1484
goto _LL0;}case 61:
# 1485 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.tq=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 62:  {
# 1487 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.tq);});_Tmp1.tq=_Tmp2;}),_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1491
goto _LL0;}case 63:
# 1492 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=1,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 64:  {
# 1495 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=1,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1498
goto _LL0;}case 65:
# 1499 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;case 66:  {
# 1502 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,({
# 1505
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.attributes);});_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;}case 67:
# 1509 "parse.y"
 yyval=Cyc_YY20(4U);
goto _LL0;case 68:
# 1510 "parse.y"
 yyval=Cyc_YY20(5U);
goto _LL0;case 69:
# 1511 "parse.y"
 yyval=Cyc_YY20(3U);
goto _LL0;case 70:
# 1512 "parse.y"
 yyval=Cyc_YY20(1U);
goto _LL0;case 71:
# 1514 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY20(2U);
# 1518
goto _LL0;case 72:
# 1518 "parse.y"
 yyval=Cyc_YY20(0U);
goto _LL0;case 73:
# 1520 "parse.y"
 yyval=Cyc_YY20(6U);
goto _LL0;case 74:
# 1525 "parse.y"
 yyval=Cyc_YY46(0);
goto _LL0;case 75:
# 1526 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 76:
# 1530 "parse.y"
 yyval=yyyvsp[3].v;
goto _LL0;case 77:
# 1533
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 78:
# 1534 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 79:
# 1537
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Atts_parse_nullary_att(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 80:
# 1538 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81:
# 1540 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);unsigned _Tmp3=yyyvsp[2].l.first_line;Cyc_Atts_parse_unary_att(_Tmp1,_Tmp2,_Tmp3,Cyc_yyget_Exp_tok(& yyyvsp[2].v));}));
goto _LL0;case 82:
# 1542 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;unsigned _Tmp2=yyyvsp[2].l.first_line;struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[0].v);struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);unsigned _Tmp5=({
unsigned _Tmp6=yyyvsp[4].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[4].v));});
# 1542
Cyc_Atts_parse_format_att(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,({
# 1544
unsigned _Tmp6=yyyvsp[6].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[6].v));}));}));
goto _LL0;case 83:
# 1554 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 84:
# 1556 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_typedef_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[1].v),0,0);});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 85:
# 1560 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,yyyvsp[0].l.first_line));
goto _LL0;case 86:
# 1561 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,yyyvsp[0].l.first_line));
goto _LL0;case 87:
# 1562 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec(yyyvsp[0].l.first_line));
goto _LL0;case 88:
# 1563 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,yyyvsp[0].l.first_line));
goto _LL0;case 89:
# 1564 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec(yyyvsp[0].l.first_line));
goto _LL0;case 90:
# 1565 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,yyyvsp[0].l.first_line));
goto _LL0;case 91:
# 1566 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,yyyvsp[0].l.first_line));
goto _LL0;case 92:
# 1567 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec(yyyvsp[0].l.first_line));
goto _LL0;case 93:
# 1568 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec(yyyvsp[0].l.first_line));
goto _LL0;case 94:
# 1569 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_complex_spec(yyyvsp[0].l.first_line));
goto _LL0;case 95:
# 1570 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 96:
# 1571 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 97:
# 1573 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 98:
# 1575 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_builtin_type(_tag_fat("__builtin_va_list",sizeof(char),18U),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 99:
# 1577 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 100:
# 1579 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_yyget_YY45(& yyyvsp[0].v);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 101:
# 1581 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 102:
# 1583 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(& yyyvsp[2].v)),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 103:
# 1585 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tuple_type(({struct Cyc_List_List*(*_Tmp2)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp3)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp3;});struct _tuple19*(*_Tmp3)(unsigned,struct _tuple8*)=({struct _tuple19*(*_Tmp4)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_Tmp4;});unsigned _Tmp4=yyyvsp[2].l.first_line;_Tmp2(_Tmp3,_Tmp4,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[2].v)));}));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1587
goto _LL0;case 104:
# 1588 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 105:
# 1590 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_eko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 106:
# 1592 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_aqual_handle_type(Cyc_yyget_YY58(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 107:
# 1594 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 108:
# 1596 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 109:
# 1598 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 110:
# 1603 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Kinds_id_to_kind(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 111:  {
# 1607 "parse.y"
unsigned loc=Cyc_Flags_porting_c_code?yyyvsp[0].l.first_line: 0U;
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=1,_Tmp1.q_volatile=0,_Tmp1.q_restrict=0,_Tmp1.real_const=1,_Tmp1.loc=loc;_Tmp1;}));
goto _LL0;}case 112:
# 1609 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=1,_Tmp1.q_restrict=0,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 113:
# 1610 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=0,_Tmp1.q_restrict=1,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 114:  {
# 1616 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl)),({void*_Tmp1=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Enumdecl*_Tmp3=({struct Cyc_Absyn_Enumdecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_Tmp4->sc=2U,({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp4->name=_Tmp5;}),({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY49(& yyyvsp[3].v);_Tmp6->v=_Tmp7;});_Tmp6;});_Tmp4->fields=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});ed->r=_Tmp1;}),ed->loc=yyyvsp[0].l.first_line;
# 1618
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=9,_Tmp2->f1=ed,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1620
goto _LL0;}case 115:
# 1621 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(& yyyvsp[1].v),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 116:
# 1623 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 117:
# 1629 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),_Tmp1->tag=0,_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 118:
# 1631 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->tag=_Tmp2;}),_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 119:
# 1635 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 120:
# 1636 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 121:
# 1637 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY49(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 122:
# 1643 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp2->tag=6,({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0,({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY25(& yyyvsp[2].v);_Tmp2->f3=_Tmp3;});_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 123:  {
# 1647 "parse.y"
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[2].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[4].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[4].v));});
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[5].v);
struct _tuple28 _Tmp1=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_TypeDecl*td=({enum Cyc_Absyn_AggrKind _Tmp4=Cyc_yyget_YY23(& yyyvsp[0].v).f1;struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp6=ts;struct Cyc_Absyn_AggrdeclImpl*_Tmp7=({
struct Cyc_List_List*_Tmp8=exist_ts;struct Cyc_List_List*_Tmp9=ec;struct Cyc_List_List*_TmpA=qb;struct Cyc_List_List*_TmpB=Cyc_yyget_YY25(& yyyvsp[6].v);Cyc_Absyn_aggrdecl_impl(_Tmp8,_Tmp9,_TmpA,_TmpB,Cyc_yyget_YY23(& yyyvsp[0].v).f0);});
# 1651
Cyc_Absyn_aggr_tdecl(_Tmp4,2U,_Tmp5,_Tmp6,_Tmp7,0,yyyvsp[0].l.first_line);});
# 1654
yyval=Cyc_YY21(({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp5->tag=9,_Tmp5->f1=td,_Tmp5->f2=0;_Tmp5;});Cyc_Parse_type_spec(_Tmp4,yyyvsp[0].l.first_line);}));
# 1656
goto _LL0;}}case 124:
# 1657 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_AggrInfo _Tmp2=({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY23(& yyyvsp[0].v).f1;struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_UnknownAggr(_Tmp3,_Tmp4,Cyc_yyget_YY23(& yyyvsp[0].v).f0?({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=(void*)1;_Tmp5;}): 0);});Cyc_Absyn_aggr_type(_Tmp2,
Cyc_yyget_YY41(& yyyvsp[2].v));});
# 1657
Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1659
goto _LL0;case 125:
# 1662 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp1;_Tmp1.f0=1,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[1].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 126:
# 1663 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp1;_Tmp1.f0=0,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 127:
# 1666
 yyval=Cyc_YY22(0U);
goto _LL0;case 128:
# 1667 "parse.y"
 yyval=Cyc_YY22(1U);
goto _LL0;case 129:
# 1670
 yyval=Cyc_YY41(0);
goto _LL0;case 130:
# 1671 "parse.y"
 yyval=Cyc_YY41(Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 131:
# 1676 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 132:  {
# 1678 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*x=Cyc_yyget_YY26(& yyyvsp[0].v);for(0;x!=0;x=x->tl){
decls=Cyc_List_imp_append((struct Cyc_List_List*)x->hd,decls);}}{
# 1682
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags!=0)
({void(*_Tmp1)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;_Tmp1;})(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1687
goto _LL0;}}case 133:
# 1691 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 134:
# 1692 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY26(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 135:
# 1698 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple11));_Tmp1->tl=0,({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 136:
# 1700 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple11));({struct _tuple11*_Tmp2=Cyc_yyget_YY19(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;}),({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 137:
# 1704 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0,_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 138:
# 1705 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0,({struct Cyc_Absyn_Exp*_Tmp2=({void*_Tmp3=Cyc_yyget_YY63(& yyyvsp[2].v);Cyc_Absyn_new_exp(_Tmp3,yyyvsp[1].l.first_line);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 139:
# 1706 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 140:
# 1707 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=({void*_Tmp3=Cyc_yyget_YY63(& yyyvsp[2].v);Cyc_Absyn_new_exp(_Tmp3,yyyvsp[1].l.first_line);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 141:  {
# 1712 "parse.y"
struct _RegionHandle _Tmp1=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp1;_push_region(temp);
{struct _tuple26 _Tmp2=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f0;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*x=Cyc_yyget_YY30(& yyyvsp[1].v);for(0;x!=0;x=x->tl){
struct _tuple12*_Tmp6=(struct _tuple12*)x->hd;void*_Tmp7;void*_Tmp8;struct Cyc_Parse_Declarator _Tmp9;_Tmp9=_Tmp6->f0;_Tmp8=_Tmp6->f1;_Tmp7=_Tmp6->f2;{struct Cyc_Parse_Declarator d=_Tmp9;struct Cyc_Absyn_Exp*wd=_Tmp8;struct Cyc_Absyn_Exp*wh=_Tmp7;
decls=({struct _tuple13*_TmpA=_region_malloc(temp,0U,sizeof(struct _tuple13));_TmpA->tl=decls,_TmpA->hd=d;_TmpA;});
widths_and_reqs=({struct Cyc_List_List*_TmpA=_region_malloc(temp,0U,sizeof(struct Cyc_List_List));({struct _tuple16*_TmpB=({struct _tuple16*_TmpC=_region_malloc(temp,0U,sizeof(struct _tuple16));_TmpC->f0=wd,_TmpC->f1=wh;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=widths_and_reqs;_TmpA;});}}}{
# 1722
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct Cyc_List_List*info=({struct _RegionHandle*_Tmp6=temp;struct _RegionHandle*_Tmp7=temp;struct Cyc_List_List*_Tmp8=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1723
Cyc_List_rzip(_Tmp6,_Tmp7,_Tmp8,widths_and_reqs);});
# 1725
yyval=Cyc_YY25(({struct Cyc_List_List*(*_Tmp6)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp6;})(Cyc_Parse_make_aggr_field,yyyvsp[0].l.first_line,info));
# 1727
_npop_handler(0);goto _LL0;}}}}
# 1713
;_pop_region();}case 142:
# 1734 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 143:  {
# 1736 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 144:
# 1738 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 145:  {
# 1740 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f0);});_Tmp1.f0=_Tmp2;}),_Tmp1.f1=two.f1,_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 146:
# 1742 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 147:  {
# 1744 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,_Tmp1.f1=two.f1,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f2);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;}case 148:
# 1750 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 149:  {
# 1752 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 150:
# 1754 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 151:  {
# 1756 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f0);});_Tmp1.f0=_Tmp2;}),_Tmp1.f1=two.f1,_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 152:
# 1758 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 153:  {
# 1760 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,_Tmp1.f1=two.f1,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f2);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;}case 154:
# 1766 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 155:
# 1768 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({struct _tuple12*_Tmp2=Cyc_yyget_YY29(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY30(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 156:
# 1773 "parse.y"
 yyval=Cyc_YY29(({struct _tuple12*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple12));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_YY61(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 157:
# 1777 "parse.y"
 yyval=Cyc_YY29(({struct _tuple12*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple12));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f0=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->f0.id=_Tmp2;}),_Tmp1->f0.varloc=0U,_Tmp1->f0.tms=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
# 1779
goto _LL0;case 158:
# 1782 "parse.y"
 yyval=Cyc_YY29(({struct _tuple12*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple12));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f0=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->f0.id=_Tmp2;}),_Tmp1->f0.varloc=0U,_Tmp1->f0.tms=0,_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
# 1784
goto _LL0;case 159:
# 1785 "parse.y"
 yyval=Cyc_YY29(({struct _tuple12*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple12));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 160:
# 1789 "parse.y"
 yyval=Cyc_YY61(Cyc_yyget_Exp_tok(& yyyvsp[2].v));
goto _LL0;case 161:
# 1790 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 162:  {
# 1796 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[2].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_Absyn_TypeDecl*dd=({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=ts;struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY35(& yyyvsp[4].v);_Tmp4->v=_Tmp5;});_Tmp4;});int _Tmp4=is_extensible;Cyc_Absyn_datatype_tdecl(2U,_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
# 1800
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=9,_Tmp2->f1=dd,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1802
goto _LL0;}case 163:  {
# 1803 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeInfo _Tmp2=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1806
goto _LL0;}case 164:  {
# 1807 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeFieldInfo _Tmp2=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.datatype_name=_Tmp4;}),({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[3].v);_Tmp3.field_name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_field_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[4].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1810
goto _LL0;}case 165:
# 1813 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 166:
# 1814 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 167:
# 1818 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 168:
# 1819 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 169:
# 1820 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 170:
# 1821 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 171:
# 1825 "parse.y"
 yyval=Cyc_YY33(2U);
goto _LL0;case 172:
# 1826 "parse.y"
 yyval=Cyc_YY33(3U);
goto _LL0;case 173:
# 1827 "parse.y"
 yyval=Cyc_YY33(0U);
goto _LL0;case 174:
# 1831 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=0,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;case 175:  {
# 1833 "parse.y"
struct Cyc_List_List*typs=({struct Cyc_List_List*(*_Tmp1)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});struct _tuple19*(*_Tmp2)(unsigned,struct _tuple8*)=({struct _tuple19*(*_Tmp3)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_Tmp3;});unsigned _Tmp3=yyyvsp[3].l.first_line;_Tmp1(_Tmp2,_Tmp3,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[3].v)));});
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=typs,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;}case 176:
# 1839 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 177:  {
# 1841 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 178:
# 1848 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 179:  {
# 1850 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 180:
# 1855 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 181:
# 1856 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 182:  {
# 1860 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1864
goto _LL0;}case 183:
# 1865 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 184:
# 1867 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 185:  {
# 1870 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY62(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _TmpC;({struct _tuple0*_TmpD=Cyc_yyget_YY28(& yyyvsp[0].v).id;_TmpC.id=_TmpD;}),({unsigned _TmpD=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_TmpC.varloc=_TmpD;}),({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp10=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp10->tag=3,({void*_Tmp11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp12=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp12->tag=1,_Tmp12->f1=lis,_Tmp12->f2=b,_Tmp12->f3=c,_Tmp12->f4=eff,_Tmp12->f5=ec,_Tmp12->f6=qb,_Tmp12->f7=req,_Tmp12->f8=ens,_Tmp12->f9=thrws;_Tmp12;});_Tmp10->f1=_Tmp11;});_Tmp10;});_TmpE->hd=_TmpF;}),({struct Cyc_List_List*_TmpF=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_TmpE->tl=_TmpF;});_TmpE;});_TmpC.tms=_TmpD;});_TmpC;}));
# 1874
goto _LL0;}}}case 186:
# 1875 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 187:  {
# 1878
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[1].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1881
goto _LL0;}case 188:
# 1882 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 1882
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1885
goto _LL0;case 189:
# 1889 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 190:
# 1890 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 191:
# 1891 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 192:  {
# 1895 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1899
goto _LL0;}case 193:  {
# 1900 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 194:  {
# 1904 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1908
goto _LL0;}case 195:  {
# 1909 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY62(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _TmpC;_TmpC.id=one.id,_TmpC.varloc=one.varloc,({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp10=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp10->tag=3,({void*_Tmp11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp12=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp12->tag=1,_Tmp12->f1=lis,_Tmp12->f2=b,_Tmp12->f3=c,_Tmp12->f4=eff,_Tmp12->f5=ec,_Tmp12->f6=qb,_Tmp12->f7=req,_Tmp12->f8=ens,_Tmp12->f9=thrws;_Tmp12;});_Tmp10->f1=_Tmp11;});_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=one.tms;_TmpE;});_TmpC.tms=_TmpD;});_TmpC;}));
# 1914
goto _LL0;}}}case 196:  {
# 1915 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 197:  {
# 1919
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[1].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 198:  {
# 1923 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 199:
# 1929 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 200:
# 1930 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY27(& yyyvsp[1].v));}));
goto _LL0;case 201:  {
# 1934 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(& yyyvsp[3].v)!=0)
ans=({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp3->tag=5,_Tmp3->f1=yyyvsp[3].l.first_line,({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY46(& yyyvsp[3].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=ans;_Tmp1;});{
# 1938
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _Tmp1=*Cyc_yyget_YY1(& yyyvsp[0].v);void*_Tmp2;void*_Tmp3;unsigned _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{unsigned ploc=_Tmp4;void*nullable=_Tmp3;void*bound=_Tmp2;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));_Tmp5->ptr_loc=ploc,_Tmp5->rgn_loc=yyyvsp[2].l.first_line,_Tmp5->zt_loc=yyyvsp[1].l.first_line;_Tmp5;});{
void*mod=({struct _RegionHandle*_Tmp5=yyr;struct Cyc_Absyn_PtrLoc*_Tmp6=ptrloc;void*_Tmp7=nullable;void*_Tmp8=bound;void*_Tmp9=Cyc_yyget_YY45(& yyyvsp[2].v);struct Cyc_List_List*_TmpA=Cyc_yyget_YY60(& yyyvsp[1].v);Cyc_Parse_make_pointer_mod(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_yyget_YY24(& yyyvsp[4].v));});
ans=({struct Cyc_List_List*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));_Tmp5->hd=mod,_Tmp5->tl=ans;_Tmp5;});
yyval=Cyc_YY27(ans);
# 1946
goto _LL0;}}}}case 202:
# 1948
 yyval=Cyc_YY60(0);
goto _LL0;case 203:
# 1949 "parse.y"
 yyval=Cyc_YY60(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY59(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY60(& yyyvsp[1].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 204:
# 1954 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 205:
# 1956 "parse.y"
 Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("@region qualifiers are deprecated; use @effect instead",sizeof(char),55U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY59((void*)({struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=2,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 206:
# 1958 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=2,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 207:
# 1959 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=3;_Tmp1;}));
goto _LL0;case 208:
# 1960 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=4;_Tmp1;}));
goto _LL0;case 209:
# 1961 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=7;_Tmp1;}));
goto _LL0;case 210:
# 1962 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=5;_Tmp1;}));
goto _LL0;case 211:
# 1963 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=6;_Tmp1;}));
goto _LL0;case 212:
# 1964 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=8;_Tmp1;}));
goto _LL0;case 213:
# 1965 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=9;_Tmp1;}));
goto _LL0;case 214:
# 1966 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=10,({void*_Tmp2=Cyc_yyget_YY58(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 215:  {
# 1967 "parse.y"
void*aq=({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));});yyval=Cyc_YY59((void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=10,_Tmp1->f1=aq;_Tmp1;}));
goto _LL0;}case 216:
# 1971 "parse.y"
 yyval=Cyc_YY58(Cyc_yyget_YY58(& yyyvsp[0].v));
goto _LL0;case 217:
# 1977 "parse.y"
 yyval=Cyc_YY58(({void*_Tmp1=Cyc_yyget_YY45(& yyyvsp[0].v);Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 1979
goto _LL0;case 218:
# 1981 "parse.y"
 yyval=Cyc_YY58(({void*_Tmp1=Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 1983
goto _LL0;case 219:
# 1987 "parse.y"
 goto _LL0;case 220:
# 1987 "parse.y"
 yyval=Cyc_YY58(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 221:
# 1993 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_Tmp1->f2=Cyc_Absyn_fat_bound_type:({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 222:
# 1994 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_false_type,({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 223:
# 1995 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_true_type,_Tmp1->f2=Cyc_Absyn_fat_bound_type;_Tmp1;}));
goto _LL0;case 224:
# 1998
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 225:
# 1999 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 226:
# 2002
 yyval=Cyc_YY54(Cyc_Tcutil_any_bool(0));
goto _LL0;case 227:
# 2003 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_true_type);
goto _LL0;case 228:
# 2004 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_false_type);
goto _LL0;case 229:
# 2008 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 230:
# 2009 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 231:
# 2013 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_eko,0));
goto _LL0;case 232:  {
# 2014 "parse.y"
struct Cyc_List_List*es=Cyc_yyget_YY41(& yyyvsp[0].v);if(Cyc_List_length(es)==1)yyval=Cyc_YY45((void*)_check_null(es)->hd);else{yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[0].v)));}
goto _LL0;}case 233:
# 2015 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_eko,0));
goto _LL0;case 234:
# 2026 "parse.y"
 yyval=Cyc_YY24(Cyc_Absyn_empty_tqual((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l.first_line));
goto _LL0;case 235:
# 2027 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp1,Cyc_yyget_YY24(& yyyvsp[1].v));}));
goto _LL0;case 236:  {
# 2032 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[1].v);
struct _tuple28 _Tmp1=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));_Tmp4->f0=0,_Tmp4->f1=0,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[0].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=ec,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 237:  {
# 2036 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[2].v);
struct _tuple28 _Tmp1=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=0,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[1].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=ec,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 238:  {
# 2040 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[4].v);
struct _tuple28 _Tmp1=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=1,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=ec,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 239:  {
# 2045
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[1].v);v->inject=_Tmp5;});{
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[4].v);
struct _tuple28 _Tmp5=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp6;_Tmp6.f0=0,_Tmp6.f1=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*ec=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY40(({struct _tuple27*_Tmp8=_cycalloc(sizeof(struct _tuple27));_Tmp8->f0=0,_Tmp8->f1=0,_Tmp8->f2=v,({void*_Tmp9=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp8->f3=_Tmp9;}),_Tmp8->f4=ec,_Tmp8->f5=qb;_Tmp8;}));
# 2051
goto _LL0;}}}}case 240:  {
# 2053
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[4].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[3].v);v->inject=_Tmp5;});{
struct _tuple28*ec_qb=Cyc_yyget_YY51(& yyyvsp[6].v);
struct _tuple28 _Tmp5=(unsigned)ec_qb?*ec_qb:({struct _tuple28 _Tmp6;_Tmp6.f0=0,_Tmp6.f1=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*ec=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY40(({struct _tuple27*_Tmp8=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp9=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp8->f0=_Tmp9;}),_Tmp8->f1=0,_Tmp8->f2=v,({void*_Tmp9=Cyc_yyget_YY50(& yyyvsp[5].v);_Tmp8->f3=_Tmp9;}),_Tmp8->f4=ec,_Tmp8->f5=qb;_Tmp8;}));
# 2059
goto _LL0;}}}}case 241:
# 2063 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 242:
# 2064 "parse.y"
 yyval=Cyc_YY50(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 243:
# 2067
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp3->tag=1,_Tmp3->f1=0;_Tmp3;});void*_Tmp3=Cyc_yyget_YY50(& yyyvsp[1].v);Cyc_Parse_id2type(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 244:
# 2068 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(& yyyvsp[2].v));void*_Tmp3=Cyc_yyget_YY50(& yyyvsp[3].v);Cyc_Parse_id2type(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 245:
# 2071
 yyval=Cyc_YY50(0);
goto _LL0;case 246:
# 2072 "parse.y"
 yyval=Cyc_YY50(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 247:
# 2076 "parse.y"
 yyval=Cyc_YY51(0);
goto _LL0;case 248:
# 2077 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 249:
# 2082 "parse.y"
 yyval=Cyc_YY51(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=({void*_Tmp3[1];({void*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(void*),1));});_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 250:
# 2084 "parse.y"
 yyval=Cyc_YY51(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,({struct Cyc_List_List*_Tmp2=({struct _tuple29*_Tmp3[1];({struct _tuple29*_Tmp4=Cyc_yyget_YY53(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(struct _tuple29*),1));});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 251:  {
# 2087 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(& yyyvsp[2].v);
void*_Tmp1;if(rest!=0){_Tmp1=rest->f0;{struct Cyc_List_List*rpo=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=rpo;_Tmp3;});(*rest).f0=_Tmp2;});
yyval=Cyc_YY51(rest);
# 2092
goto _LL0;}}else{_throw_match();}}case 252:  {
# 2094 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(& yyyvsp[2].v);
void*_Tmp1;if(rest!=0){_Tmp1=rest->f1;{struct Cyc_List_List*qb=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp4=Cyc_yyget_YY53(& yyyvsp[0].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=qb;_Tmp3;});(*rest).f1=_Tmp2;});
yyval=Cyc_YY51(rest);
# 2099
goto _LL0;}}else{_throw_match();}}case 253:  {
# 2104 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct)),kb->tag=2,kb->f1=0,kb->f2=& Cyc_Kinds_ek;{
void*t=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);void*_Tmp2=(void*)kb;Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[2].l.first_line);});
yyval=Cyc_YY52((void*)({struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct));_Tmp1->tag=2,({void*_Tmp2=Cyc_Parse_effect_from_atomic(Cyc_yyget_YY41(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=t;_Tmp1;}));
# 2108
goto _LL0;}}case 254:
# 2110 "parse.y"
 yyval=Cyc_YY52((void*)({struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_Tmp1->tag=1,({void*_Tmp2=Cyc_Parse_effect_from_atomic(Cyc_yyget_YY41(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),({
void*_Tmp2=Cyc_Parse_effect_from_atomic(Cyc_yyget_YY41(& yyyvsp[2].v));_Tmp1->f2=_Tmp2;});_Tmp1;}));
# 2113
goto _LL0;case 255:
# 2115 "parse.y"
({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_check_single_constraint(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));});
yyval=Cyc_YY52((void*)({struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_Tmp1->tag=0,({void*_Tmp2=Cyc_Parse_effect_from_atomic(Cyc_yyget_YY41(& yyyvsp[2].v));_Tmp1->f1=_Tmp2;});_Tmp1;}));
# 2118
goto _LL0;case 256:
# 2123 "parse.y"
 yyval=Cyc_YY53(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->f0=_Tmp2;}),({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
# 2125
goto _LL0;case 257:
# 2128 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_al_qual_type);
goto _LL0;case 258:
# 2129 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_un_qual_type);
goto _LL0;case 259:
# 2130 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rc_qual_type);
goto _LL0;case 260:
# 2131 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rtd_qual_type);
goto _LL0;case 261:
# 2132 "parse.y"
 yyval=Cyc_YY58(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 262:
# 2137 "parse.y"
 yyval=Cyc_YY45(Cyc_yyget_YY58(& yyyvsp[0].v));
goto _LL0;case 263:
# 2140 "parse.y"
 yyval=Cyc_YY45(({void*_Tmp1=Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 2142
goto _LL0;case 264:  {
# 2147 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct)),kb->tag=0,kb->f1=& Cyc_Kinds_aqk;
yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=(void*)kb;Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[0].l.first_line);}));
# 2150
goto _LL0;}case 265:
# 2152 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v)));
# 2154
goto _LL0;case 266:
# 2181 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 267:
# 2183 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_zstrcmp(_Tmp1,_tag_fat("inject",sizeof(char),7U));})!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(yyyvsp[0].l.first_line,_tag_fat(_Tmp2,sizeof(void*),1));});
yyval=Cyc_YY32(1);
# 2187
goto _LL0;case 268:
# 2190 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 269:
# 2191 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY41(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));}));
goto _LL0;case 270:
# 2195 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 271:
# 2196 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 272:
# 2197 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 273:
# 2199 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(& yyyvsp[0].v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 2202
goto _LL0;case 274:
# 2207 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 275:
# 2209 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 276:
# 2214 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 277:
# 2215 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY39(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 278:  {
# 2221 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
struct Cyc_Parse_Declarator _Tmp5=Cyc_yyget_YY28(& yyyvsp[1].v);void*_Tmp6;unsigned _Tmp7;void*_Tmp8;_Tmp8=_Tmp5.id;_Tmp7=_Tmp5.varloc;_Tmp6=_Tmp5.tms;{struct _tuple0*qv=_Tmp8;unsigned varloc=_Tmp7;struct Cyc_List_List*tms=_Tmp6;
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct _tuple14 _Tmp9=Cyc_Parse_apply_tms(tq,t,atts,tms);void*_TmpA;void*_TmpB;void*_TmpC;struct Cyc_Absyn_Tqual _TmpD;_TmpD=_Tmp9.f0;_TmpC=_Tmp9.f1;_TmpB=_Tmp9.f2;_TmpA=_Tmp9.f3;{struct Cyc_Absyn_Tqual tq2=_TmpD;void*t2=_TmpC;struct Cyc_List_List*tvs=_TmpB;struct Cyc_List_List*atts2=_TmpA;
if(tvs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_err2(yyyvsp[1].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_err2(yyyvsp[0].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});{
struct _fat_ptr*idopt=(*qv).f1;
if(atts2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_TmpE=_cycalloc(sizeof(struct _tuple8));_TmpE->f0=idopt,_TmpE->f1=tq2,_TmpE->f2=t2;_TmpE;}));
# 2235
goto _LL0;}}}}}}case 279:  {
# 2236 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
if(atts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_Tmp6,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=tq,_Tmp5->f2=t;_Tmp5;}));
# 2243
goto _LL0;}}}case 280:  {
# 2244 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct Cyc_List_List*tms=Cyc_yyget_YY31(& yyyvsp[1].v).tms;
struct _tuple14 _Tmp5=Cyc_Parse_apply_tms(tq,t,atts,tms);void*_Tmp6;void*_Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;_Tmp9=_Tmp5.f0;_Tmp8=_Tmp5.f1;_Tmp7=_Tmp5.f2;_Tmp6=_Tmp5.f3;{struct Cyc_Absyn_Tqual tq2=_Tmp9;void*t2=_Tmp8;struct Cyc_List_List*tvs=_Tmp7;struct Cyc_List_List*atts2=_Tmp6;
if(tvs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=
_tag_fat("bad type parameters on formal argument, ignoring",sizeof(char),49U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpB,sizeof(void*),1));});
if(atts2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpB,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_TmpA=_cycalloc(sizeof(struct _tuple8));_TmpA->f0=0,_TmpA->f1=tq2,_TmpA->f2=t2;_TmpA;}));
# 2256
goto _LL0;}}}}case 281:
# 2259 "parse.y"
 yyval=Cyc_YY37(Cyc_List_imp_rev(Cyc_yyget_YY37(& yyyvsp[0].v)));
goto _LL0;case 282:
# 2262
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 283:
# 2263 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY37(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 284:
# 2267 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 285:
# 2268 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 286:
# 2273 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 287:
# 2275 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 288:
# 2277 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 289:  {
# 2279 "parse.y"
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[2].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});void*_Tmp3=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,_Tmp3,
Cyc_Absyn_uint_exp(0U,yyyvsp[2].l.first_line),0);});
# 2282
vd->tq.real_const=1;
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_Tmp2->tag=26,_Tmp2->f1=vd,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2285
goto _LL0;}case 290:  {
# 2287 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_Tmp2->tag=27,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=t,_Tmp2->f3=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2290
goto _LL0;}case 291:
# 2295 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f0=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 292:
# 2297 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp3->f0=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 293:
# 2299 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f0=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 294:
# 2301 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[2].v);_Tmp3->f0=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 295:
# 2305 "parse.y"
 yyval=Cyc_YY42(Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[0].v)));
goto _LL0;case 296:
# 2306 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp3->tag=1,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 297:
# 2311 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 298:
# 2312 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 299:
# 2316 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 300:
# 2317 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp1->tag=1,({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 301:  {
# 2322 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
if(atts!=0)
Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("ignoring attributes in type",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=tq,_Tmp5->f2=t;_Tmp5;}));
# 2328
goto _LL0;}}case 302:  {
# 2329 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
struct Cyc_List_List*tms=Cyc_yyget_YY31(& yyyvsp[1].v).tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);
if(t_info.f2!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(t_info.f3!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,_tag_fat("bad specifiers, ignoring",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=t_info.f0,_Tmp5->f2=t_info.f1;_Tmp5;}));
# 2339
goto _LL0;}}case 303:
# 2342 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 304:
# 2343 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(0));
goto _LL0;case 305:
# 2344 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 306:
# 2345 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v)));
goto _LL0;case 307:
# 2346 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;})));
goto _LL0;case 308:
# 2347 "parse.y"
 yyval=Cyc_YY45(Cyc_yyget_YY45(& yyyvsp[0].v));
goto _LL0;case 309:
# 2353 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 310:
# 2354 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 311:
# 2358 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY27(& yyyvsp[0].v);_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 312:
# 2359 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 313:
# 2361 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,Cyc_yyget_YY31(& yyyvsp[1].v).tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 314:
# 2366 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 315:
# 2368 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[2].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 316:
# 2370 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 317:
# 2372 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 318:
# 2374 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2374
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2377
goto _LL0;case 319:  {
# 2378 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY62(& yyyvsp[3].v);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _TmpC;({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp10=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp10->tag=3,({void*_Tmp11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp12=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp12->tag=1,_Tmp12->f1=lis,_Tmp12->f2=b,_Tmp12->f3=c,_Tmp12->f4=eff,_Tmp12->f5=ec,_Tmp12->f6=qb,_Tmp12->f7=req,_Tmp12->f8=ens,_Tmp12->f9=thrws;_Tmp12;});_Tmp10->f1=_Tmp11;});_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=0;_TmpE;});_TmpC.tms=_TmpD;});_TmpC;}));
# 2382
goto _LL0;}}}case 320:  {
# 2383 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*ec=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY62(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _TmpC;({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp10=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp10->tag=3,({void*_Tmp11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp12=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp12->tag=1,_Tmp12->f1=lis,_Tmp12->f2=b,_Tmp12->f3=c,_Tmp12->f4=eff,_Tmp12->f5=ec,_Tmp12->f6=qb,_Tmp12->f7=req,_Tmp12->f8=ens,_Tmp12->f9=thrws;_Tmp12;});_Tmp10->f1=_Tmp11;});_Tmp10;});_TmpE->hd=_TmpF;}),({
struct Cyc_List_List*_TmpF=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_TmpE->tl=_TmpF;});_TmpE;});
# 2385
_TmpC.tms=_TmpD;});_TmpC;}));
# 2388
goto _LL0;}}}case 321:  {
# 2390
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[1].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[1].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2391
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2394
goto _LL0;}case 322:
# 2395 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2397
goto _LL0;case 323:
# 2401 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0,_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 324:
# 2403 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;_Tmp1.f0=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 325:
# 2405 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;_Tmp1.f0=0,_Tmp1.f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 326:
# 2407 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f0=_Tmp3;}),_Tmp2.f1=0,_Tmp2.f2=0;_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY62(& yyyvsp[4].v));}));
goto _LL0;case 327:
# 2409 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;_Tmp2.f0=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f1=_Tmp3;}),_Tmp2.f2=0;_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY62(& yyyvsp[4].v));}));
goto _LL0;case 328:
# 2411 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f2=_Tmp3;});_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY62(& yyyvsp[4].v));}));
goto _LL0;case 329:
# 2415 "parse.y"
 yyval=Cyc_YY62(({struct _tuple21 _Tmp1;_Tmp1.f0=0,_Tmp1.f1=0,_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 330:
# 2416 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 331:
# 2421 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 332:
# 2422 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 333:
# 2423 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 334:
# 2424 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 335:
# 2425 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 336:
# 2426 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 337:
# 2432 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 338:
# 2436 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 339:
# 2437 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_exp_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 340:
# 2442 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 341:
# 2443 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 342:
# 2448 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));}));
goto _LL0;case 343:
# 2449 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 344:
# 2451 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_Absyn_skip_stmt(0U));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 345:
# 2453 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_yyget_Stmt_tok(& yyyvsp[3].v));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 346:
# 2454 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 347:
# 2455 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Absyn_seq_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 348:
# 2456 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 349:
# 2459 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 350:
# 2464 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 351:
# 2466 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[6].v);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 352:
# 2472 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 353:
# 2475
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_unknownid_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 354:
# 2478
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[3].v);Cyc_Absyn_tuple_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 355:
# 2482 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);Cyc_Absyn_trycatch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 356:
# 2496 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 357:
# 2499 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2499
_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 358:
# 2502 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[2].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2502
_Tmp1->hd=_Tmp2;}),({
# 2504
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 359:
# 2506 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 360:
# 2508 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[4].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2508
_Tmp1->hd=_Tmp2;}),({
# 2510
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 361:
# 2512 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[5].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 362:
# 2519 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);Cyc_Absyn_while_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 363:
# 2521 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_do_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 364:
# 2523 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[8].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 365:  {
# 2525 "parse.y"
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[5].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[7].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp1,s);}));
goto _LL0;}case 366:
# 2529
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 367:
# 2530 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 368:
# 2533
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp2=_Tmp3;});_Tmp2;});Cyc_Absyn_goto_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 369:
# 2534 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 370:
# 2535 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 371:
# 2536 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 372:
# 2537 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_return_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 373:
# 2539 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 374:
# 2540 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 375:
# 2542 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_fallthru_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 376:
# 2551 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 377:
# 2554
 yyval=yyyvsp[0].v;
goto _LL0;case 378:
# 2556 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 379:
# 2559
 yyval=yyyvsp[0].v;
goto _LL0;case 380:
# 2561 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 381:
# 2564
 yyval=yyyvsp[0].v;
goto _LL0;case 382:
# 2566 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 383:
# 2569
 yyval=yyyvsp[0].v;
goto _LL0;case 384:
# 2571 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 385:
# 2574
 yyval=yyyvsp[0].v;
goto _LL0;case 386:
# 2576 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 387:
# 2579
 yyval=yyyvsp[0].v;
goto _LL0;case 388:
# 2581 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 389:
# 2584
 yyval=yyyvsp[0].v;
goto _LL0;case 390:
# 2586 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY69(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 391:
# 2589
 yyval=yyyvsp[0].v;
goto _LL0;case 392:
# 2591 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY69(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 393:
# 2594
 yyval=yyyvsp[0].v;
goto _LL0;case 394:
# 2596 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 395:
# 2598 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 396:
# 2601
 yyval=yyyvsp[0].v;
goto _LL0;case 397:
# 2603 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 398:
# 2606
 yyval=yyyvsp[0].v;
goto _LL0;case 399:
# 2608 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 400:
# 2611
 yyval=yyyvsp[0].v;
goto _LL0;case 401:  {
# 2613 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);})));
# 2616
goto _LL0;}case 402:
# 2619 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 403:
# 2623 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 404:  {
# 2625 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line)));
goto _LL0;}case 405:
# 2628 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);})));
goto _LL0;case 406:
# 2630 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=(*Cyc_yyget_YY38(& yyyvsp[2].v)).f2;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 407:
# 2635 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 408:
# 2643 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 409:
# 2647 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line));
goto _LL0;case 410:
# 2648 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 411:
# 2649 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_Tmp2->tag=14,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 412:
# 2650 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=5,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 413:  {
# 2652 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(& yyyvsp[0].v);
{void*_Tmp1=e->r;struct _fat_ptr _Tmp2;int _Tmp3;short _Tmp4;char _Tmp5;enum Cyc_Absyn_Sign _Tmp6;if(*((int*)_Tmp1)==0)switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.LongLong_c.tag){case 2: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f0;_Tmp5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;char i=_Tmp5;
# 2655
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_Tmp8->tag=10,_Tmp8->f1=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4F5;}case 4: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f0;_Tmp4=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;short i=_Tmp4;
# 2657
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=9,_Tmp8->f1=s,_Tmp8->f2=(int)i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4F5;}case 5: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f0;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;int i=_Tmp3;
# 2659
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=9,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4F5;}case 7: _Tmp2=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f0;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f1;{struct _fat_ptr s=_Tmp2;int i=_Tmp3;
# 2661
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_Tmp8->tag=11,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4F5;}case 1:
# 2663
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL4F5;case 8:
 goto _LL503;case 9: _LL503:
# 2666
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("strings cannot occur within patterns",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));goto _LL4F5;case 6:
# 2668
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));goto _LL4F5;default: goto _LL506;}else{_LL506:
# 2670
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("bad constant in case",sizeof(char),21U),_tag_fat(0U,sizeof(void*),0));}_LL4F5:;}
# 2673
goto _LL0;}case 414:
# 2674 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `as'",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0,0);});_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2676
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2679
goto _LL0;case 415:
# 2680 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `alias'",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_ek;_Tmp2;});tv->kind=_Tmp1;}),tv->aquals_bound=0;{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0,0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2689
goto _LL0;}}}case 416:
# 2690 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `alias'",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_ek;_Tmp2;});tv->kind=_Tmp1;}),tv->aquals_bound=0;{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0,0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2699
goto _LL0;}}}case 417:  {
# 2700 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=0,_Tmp5->f2=1,_Tmp5->f3=0,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2703
goto _LL0;}}case 418:  {
# 2704 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY10(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_Tmp5->tag=15,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=ps,_Tmp5->f3=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2707
goto _LL0;}}case 419:  {
# 2708 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[3].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp5;});unsigned _Tmp5=yyyvsp[2].l.first_line;_Tmp4(Cyc_Parse_typ2tvar,_Tmp5,Cyc_yyget_YY41(& yyyvsp[2].v));});
union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,Cyc_yyget_QualId_tok(& yyyvsp[0].v),0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct)),typ->tag=0,({void*_Tmp4=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_Tmp5->tag=24,_Tmp5->f1=ai;_Tmp5;});typ->f1=_Tmp4;}),typ->f2=0;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=(void*)typ,_Tmp5->f2=0,_Tmp5->f3=exist_ts,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2714
goto _LL0;}}case 420:  {
# 2715 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp5;});unsigned _Tmp5=yyyvsp[1].l.first_line;_Tmp4(Cyc_Parse_typ2tvar,_Tmp5,Cyc_yyget_YY41(& yyyvsp[1].v));});
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=0,_Tmp5->f2=0,_Tmp5->f3=exist_ts,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2719
goto _LL0;}}case 421:
# 2720 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=5,({struct Cyc_Absyn_Pat*_Tmp3=({void*_Tmp4=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp5->tag=5,({struct Cyc_Absyn_Pat*_Tmp6=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;});_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);});_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 422:
# 2722 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0,0);});_Tmp2->f1=_Tmp3;}),({
# 2724
struct Cyc_Absyn_Pat*_Tmp3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[1].l.first_line);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2722
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2726
goto _LL0;case 423:
# 2727 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
Cyc_Warn_err(yyyvsp[2].l.first_line,_tag_fat("expecting `as'",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0,0);});_Tmp2->f1=_Tmp3;}),({
# 2731
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[3].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2729
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2733
goto _LL0;case 424:  {
# 2734 "parse.y"
void*tag=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);void*_Tmp2=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik);Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[2].l.first_line);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,({struct Cyc_Absyn_Tvar*_Tmp3=Cyc_Parse_typ2tvar(yyyvsp[2].l.first_line,tag);_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(tag),0,0);});
# 2736
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2735
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2739
goto _LL0;}case 425:  {
# 2740 "parse.y"
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,_Tmp2->f1=tv,({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0,0);});
# 2742
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2741
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2745
goto _LL0;}case 426:
# 2748 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 427:
# 2749 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 428:
# 2750 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f0=0,_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 429:
# 2753
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 430:
# 2754 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY11(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 431:
# 2757
 yyval=Cyc_YY12(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));_Tmp1->f0=0,({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 432:
# 2758 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 433:
# 2761
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 434:
# 2762 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 435:
# 2763 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f0=0,_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 436:
# 2766
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp2=Cyc_yyget_YY12(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 437:
# 2767 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp2=Cyc_yyget_YY12(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY13(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 438:
# 2772 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 439:
# 2773 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_seq_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 440:
# 2777 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 441:
# 2779 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Core_Opt*_Tmp2=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assignop_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 442:
# 2781 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_swap_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 443:
# 2785 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 444:
# 2786 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)1U;_Tmp1;}));
goto _LL0;case 445:
# 2787 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)3U;_Tmp1;}));
goto _LL0;case 446:
# 2788 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)4U;_Tmp1;}));
goto _LL0;case 447:
# 2789 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)0U;_Tmp1;}));
goto _LL0;case 448:
# 2790 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)2U;_Tmp1;}));
goto _LL0;case 449:
# 2791 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)16U;_Tmp1;}));
goto _LL0;case 450:
# 2792 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)17U;_Tmp1;}));
goto _LL0;case 451:
# 2793 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)13U;_Tmp1;}));
goto _LL0;case 452:
# 2794 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)15U;_Tmp1;}));
goto _LL0;case 453:
# 2795 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)14U;_Tmp1;}));
goto _LL0;case 454:
# 2799 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 455:
# 2801 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 456:
# 2803 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_throw_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 457:
# 2805 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 458:
# 2806 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 459:
# 2807 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 460:
# 2808 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 461:  {
# 2809 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp4=rgn;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp6=qual;Cyc_Absyn_New_exp(_Tmp4,_Tmp5,_Tmp6,yyyvsp[0].l.first_line);}));
goto _LL0;}}case 462:  {
# 2810 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp4=rgn;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp6=qual;Cyc_Absyn_New_exp(_Tmp4,_Tmp5,_Tmp6,yyyvsp[0].l.first_line);}));
goto _LL0;}}case 463:
# 2822 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 464:
# 2825
 yyval=yyyvsp[0].v;
goto _LL0;case 465:
# 2827 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 466:
# 2830
 yyval=yyyvsp[0].v;
goto _LL0;case 467:
# 2832 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 468:
# 2835
 yyval=yyyvsp[0].v;
goto _LL0;case 469:
# 2837 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 470:
# 2840
 yyval=yyyvsp[0].v;
goto _LL0;case 471:
# 2842 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 472:
# 2845
 yyval=yyyvsp[0].v;
goto _LL0;case 473:
# 2847 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 474:
# 2850
 yyval=yyyvsp[0].v;
goto _LL0;case 475:
# 2852 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY69(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 476:
# 2855
 yyval=yyyvsp[0].v;
goto _LL0;case 477:
# 2857 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY69(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 478:
# 2860
 yyval=yyyvsp[0].v;
goto _LL0;case 479:
# 2862 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 480:
# 2864 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 481:
# 2867
 yyval=yyyvsp[0].v;
goto _LL0;case 482:
# 2869 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 483:
# 2872
 yyval=yyyvsp[0].v;
goto _LL0;case 484:
# 2874 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 485:
# 2877
 yyval=Cyc_YY69(Cyc_Absyn_eq_exp);
goto _LL0;case 486:
# 2878 "parse.y"
 yyval=Cyc_YY69(Cyc_Absyn_neq_exp);
goto _LL0;case 487:
# 2881
 yyval=Cyc_YY69(Cyc_Absyn_lt_exp);
goto _LL0;case 488:
# 2882 "parse.y"
 yyval=Cyc_YY69(Cyc_Absyn_gt_exp);
goto _LL0;case 489:
# 2883 "parse.y"
 yyval=Cyc_YY69(Cyc_Absyn_lte_exp);
goto _LL0;case 490:
# 2884 "parse.y"
 yyval=Cyc_YY69(Cyc_Absyn_gte_exp);
goto _LL0;case 491:
# 2887
 yyval=Cyc_YY6(0U);
goto _LL0;case 492:
# 2888 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 493:
# 2891
 yyval=Cyc_YY6(1U);
goto _LL0;case 494:
# 2892 "parse.y"
 yyval=Cyc_YY6(3U);
goto _LL0;case 495:
# 2893 "parse.y"
 yyval=Cyc_YY6(4U);
goto _LL0;case 496:
# 2897 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 497:  {
# 2899 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;}case 498:
# 2904 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 499:
# 2905 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;case 500:
# 2906 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,2U,yyyvsp[0].l.first_line);}));
goto _LL0;case 501:
# 2907 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_address_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 502:
# 2908 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_deref_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 503:
# 2909 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 504:  {
# 2911 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line));
# 2914
goto _LL0;}case 505:
# 2914 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 506:  {
# 2916 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
# 2919
goto _LL0;}case 507:
# 2921
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,_Tmp2->f1.rgn=0,_Tmp2->f1.aqual=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2923
goto _LL0;case 508:
# 2924 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.aqual=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2926
goto _LL0;case 509:
# 2927 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.aqual=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2929
goto _LL0;case 510:  {
# 2930 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;
yyval=Cyc_Exp_tok(({void*_Tmp4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1.mknd=2U,_Tmp5->f1.rgn=rgn,_Tmp5->f1.aqual=qual,_Tmp5->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp5->f1.num_elts=_Tmp6;}),_Tmp5->f1.fat_result=0,_Tmp5->f1.inline_call=0;_Tmp5;});Cyc_Absyn_new_exp(_Tmp4,yyyvsp[0].l.first_line);}));
# 2933
goto _LL0;}}case 511:  {
# 2934 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;
yyval=Cyc_Exp_tok(({void*_Tmp4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1.mknd=0U,_Tmp5->f1.rgn=rgn,_Tmp5->f1.aqual=qual,_Tmp5->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp5->f1.num_elts=_Tmp6;}),_Tmp5->f1.fat_result=0,_Tmp5->f1.inline_call=1;_Tmp5;});Cyc_Absyn_new_exp(_Tmp4,yyyvsp[0].l.first_line);}));
# 2937
goto _LL0;}}case 512:  {
# 2938 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,_Tmp2->f1.rgn=0,_Tmp2->f1.aqual=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2941
goto _LL0;}case 513:  {
# 2942 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[10].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[10].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.aqual=_Tmp3;}),({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2945
goto _LL0;}case 514:  {
# 2947
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[8].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[8].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.aqual=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2950
goto _LL0;}case 515:
# 2951 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2[1];({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2[0]=_Tmp3;});Cyc_List_list(_tag_fat(_Tmp2,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(18U,_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 516:
# 2953 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_tagof_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 517:
# 2955 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=37,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 518:
# 2957 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=37,({struct Cyc_Absyn_Exp*_Tmp3=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_deref_exp(_Tmp4,yyyvsp[2].l.first_line);});_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 519:  {
# 2959 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,yyyvsp[0].l.first_line));
goto _LL0;}case 520:
# 2961 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=Cyc_yyget_YY63(& yyyvsp[1].v);Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 521:
# 2962 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_extension_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 522:
# 2963 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 523:
# 2964 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_false_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 524:
# 2968 "parse.y"
 yyval=Cyc_YY6(12U);
goto _LL0;case 525:
# 2969 "parse.y"
 yyval=Cyc_YY6(11U);
goto _LL0;case 526:
# 2970 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 527:
# 2971 "parse.y"
 yyval=Cyc_YY6(0U);
goto _LL0;case 528:  {
# 2976 "parse.y"
struct _tuple31*_Tmp1=Cyc_yyget_YY64(& yyyvsp[3].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*outlist=_Tmp4;struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY63((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_Tmp5->tag=39,({int _Tmp6=Cyc_yyget_YY32(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=outlist,_Tmp5->f4=inlist,_Tmp5->f5=clobbers;_Tmp5;}));
goto _LL0;}}case 529:
# 2981 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 530:
# 2982 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 531:
# 2986 "parse.y"
 yyval=Cyc_YY64(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f0=0,_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 532:  {
# 2988 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY65(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY64(({struct _tuple31*_Tmp4=_cycalloc(sizeof(struct _tuple31));_Tmp4->f0=0,_Tmp4->f1=inlist,_Tmp4->f2=clobbers;_Tmp4;}));
goto _LL0;}}case 533:  {
# 2991 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY65(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY64(({struct _tuple31*_Tmp4=_cycalloc(sizeof(struct _tuple31));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY67(& yyyvsp[1].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=inlist,_Tmp4->f2=clobbers;_Tmp4;}));
goto _LL0;}}case 534:
# 2995
 yyval=Cyc_YY67(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY68(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 535:
# 2996 "parse.y"
 yyval=Cyc_YY67(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY68(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 536:
# 3000 "parse.y"
 yyval=Cyc_YY65(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 537:
# 3001 "parse.y"
 yyval=Cyc_YY65(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 538:
# 3002 "parse.y"
 yyval=Cyc_YY65(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY67(& yyyvsp[1].v));_Tmp1->f0=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 539:
# 3005
 yyval=Cyc_YY67(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY68(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 540:
# 3006 "parse.y"
 yyval=Cyc_YY67(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY68(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 541:
# 3010 "parse.y"
 yyval=Cyc_YY68(({struct _tuple32*_Tmp1=_cycalloc(sizeof(struct _tuple32));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 542:
# 3015 "parse.y"
 yyval=Cyc_YY66(0);
goto _LL0;case 543:
# 3016 "parse.y"
 yyval=Cyc_YY66(0);
goto _LL0;case 544:
# 3017 "parse.y"
 yyval=Cyc_YY66(Cyc_List_imp_rev(Cyc_yyget_YY66(& yyyvsp[1].v)));
goto _LL0;case 545:
# 3020
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 546:
# 3021 "parse.y"
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 547:
# 3026 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 548:
# 3028 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_subscript_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 549:
# 3030 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_unknowncall_exp(_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 550:
# 3032 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_unknowncall_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 551:
# 3034 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrmember_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 552:
# 3036 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrarrow_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 553:
# 3038 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,1U,yyyvsp[0].l.first_line);}));
goto _LL0;case 554:
# 3040 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,3U,yyyvsp[0].l.first_line);}));
goto _LL0;case 555:
# 3042 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 556:
# 3044 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 557:
# 3046 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 558:
# 3051 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 559:  {
# 3054
unsigned i=({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_cnst2uint(_Tmp1,Cyc_yyget_Int_tok(& yyyvsp[0].v));});
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=Cyc_Absyn_tuple_field_designator((int)i);void*_Tmp2;_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp1)->f1;{struct _fat_ptr*f=_Tmp2;
yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp5->tag=0,_Tmp5->f1=f;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;}));
goto _LL0;}}case 560:
# 3058 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 561:  {
# 3061
unsigned i=({unsigned _Tmp1=yyyvsp[2].l.first_line;Cyc_Parse_cnst2uint(_Tmp1,Cyc_yyget_Int_tok(& yyyvsp[2].v));});
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=Cyc_Absyn_tuple_field_designator((int)i);void*_Tmp2;_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp1)->f1;{struct _fat_ptr*f=_Tmp2;
yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp5->tag=0,_Tmp5->f1=f;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp3->tl=_Tmp4;});_Tmp3;}));
goto _LL0;}}case 562:
# 3068 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_unknownid_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 563:
# 3069 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Absyn_pragma_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 564:
# 3070 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 565:
# 3071 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_string_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 566:
# 3072 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wstring_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 567:
# 3073 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 568:
# 3077 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_noinstantiate_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 569:
# 3080
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[3].v));Cyc_Absyn_instantiate_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 570:
# 3083
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp2->tag=28,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[3].v));_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 571:
# 3085 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_tuple_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 572:
# 3087 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Absyn_stmt_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 573:
# 3091 "parse.y"
 yyval=Cyc_YY4(Cyc_List_imp_rev(Cyc_yyget_YY4(& yyyvsp[0].v)));
goto _LL0;case 574:
# 3096 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 575:
# 3098 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 576:
# 3104 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _Tmp1=Cyc_yyget_Int_tok(& yyyvsp[0].v);Cyc_Absyn_const_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 577:
# 3105 "parse.y"
 yyval=Cyc_Exp_tok(({char _Tmp1=Cyc_yyget_Char_tok(& yyyvsp[0].v);Cyc_Absyn_char_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 578:
# 3106 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wchar_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 579:
# 3108 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp(yyyvsp[0].l.first_line));
goto _LL0;case 580:  {
# 3110 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(& yyyvsp[0].v);
int l=(int)Cyc_strlen(f);
int i=1;
if(l > 0){
char c=((const char*)f.curr)[l - 1];
if((int)c==102 ||(int)c==70)i=0;else{
if((int)c==108 ||(int)c==76)i=2;}}
# 3118
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,yyyvsp[0].l.first_line));
# 3120
goto _LL0;}case 581:
# 3123 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f0=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 582:
# 3124 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 583:
# 3127
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f0=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 584:
# 3128 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 585:
# 3133 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 586:
# 3134 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 587:
# 3137
 yyval=yyyvsp[0].v;
goto _LL0;case 588:
# 3138 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 589:
# 3143 "parse.y"
 goto _LL0;case 590:
# 3143 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default:
# 3147
 goto _LL0;}_LL0:;
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 389 "cycbison.simple"
(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))).v=yyval;
# 392
if(yylen==0){
struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr_check_bound(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset - 1),sizeof(struct Cyc_Yystacktype),2U));
p[1].l.first_line=yylloc.first_line;
p[1].l.first_column=yylloc.first_column;
p[1].l.last_line=p[0].l.last_line;
p[1].l.last_column=p[0].l.last_column;}else{
# 399
((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset].l.last_line=(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l.last_line;
((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset].l.last_column=((struct Cyc_Yystacktype*)yyvs.curr)[(yyvsp_offset + yylen)- 1].l.last_column;}
# 409
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr1,591U,sizeof(short),yyn));
# 411
yystate=({int _Tmp1=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,176U,sizeof(short),yyn - 170));_Tmp1 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7765)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7766U,sizeof(short),yystate))==(int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,176U,sizeof(short),yyn - 170));}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus==0){
# 424
++ yynerrs;
# 427
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1171U,sizeof(short),yystate));
# 429
if(yyn > -32768 && yyn < 7765){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 346U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7766U,sizeof(short),x + yyn))==x)
({unsigned long _Tmp1=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,346U,sizeof(struct _fat_ptr),x)))+ 15U;sze +=_Tmp1;}),count ++;}
msg=({unsigned _Tmp1=(unsigned)(sze + 15)+ 1U;_tag_fat(({char*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(char)));});({{unsigned _Tmp3=(unsigned)(sze + 15);unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;}),sizeof(char),_Tmp1);});
Cyc_strcpy(msg,_tag_fat("parse error",sizeof(char),12U));
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 346U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7766U,sizeof(short),x + yyn))==x){
# 451
Cyc_strcat(msg,count==0?
_tag_fat(", expecting `",sizeof(char),14U):
 _tag_fat(" or `",sizeof(char),6U));
Cyc_strcat(msg,*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,346U,sizeof(struct _fat_ptr),x)));
Cyc_strcat(msg,_tag_fat("'",sizeof(char),2U));
++ count;}}}
# 459
Cyc_yyerror(msg,yystate,yychar);}else{
# 463
Cyc_yyerror(_tag_fat("parse error",sizeof(char),12U),yystate,yychar);}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus==3){
# 474
if(yychar==0){
int _Tmp1=1;_npop_handler(0);return _Tmp1;}
# 483
yychar=-2;}
# 489
yyerrstatus=3;
# 491
goto yyerrhandle;
# 493
yyerrdefault:
# 503 "cycbison.simple"
 yyerrpop:
# 505
 if(yyssp_offset==0){int _Tmp1=1;_npop_handler(0);return _Tmp1;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_fat_subscript(yyss,sizeof(short),-- yyssp_offset));
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1171U,sizeof(short),yystate));
if(yyn==-32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7765)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7766U,sizeof(short),yyn))!=1)goto yyerrdefault;
# 528
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 531
if(yyn==-32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 535
if(yyn==0)goto yyerrpop;}
# 537
if(yyn==1170){
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 546
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_Tmp1;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3146 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
void*_Tmp0;struct _fat_ptr _Tmp1;char _Tmp2;union Cyc_Absyn_Cnst _Tmp3;switch(v.Stmt_tok.tag){case 1: _Tmp3=v.Int_tok.val;{union Cyc_Absyn_Cnst c=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_cnst2string(c);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 2: _Tmp2=v.Char_tok.val;{char c=_Tmp2;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)c);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%c",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 3: _Tmp1=v.String_tok.val;{struct _fat_ptr s=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("\"%s\"",sizeof(char),5U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 4: _Tmp0=v.QualId_tok.val;{struct _tuple0*q=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_qvar2string(q);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 5: _Tmp0=v.Exp_tok.val;{struct Cyc_Absyn_Exp*e=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 6: _Tmp0=v.Stmt_tok.val;{struct Cyc_Absyn_Stmt*s=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_Absynpp_stmt2string(s);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}default:
 Cyc_fprintf(Cyc_stderr,_tag_fat("?",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 3158
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return _tag_fat("end-of-file",sizeof(char),12U);
if(token==384)
return Cyc_Lex_token_string;
if(token==393)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 397?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,398U,sizeof(short),token)): 346;
if((unsigned)z < 346U)
return*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,346U,sizeof(struct _fat_ptr),z));
return _tag_fat(0,0,0);}}
# 3172
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _Tmp0=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _Tmp0;_push_region(yyr);
({struct _RegionHandle*_Tmp1=yyr;Cyc_yyparse(_Tmp1,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_Tmp1=Cyc_Parse_parse_result;_npop_handler(0);return _Tmp1;}
# 3175
;_pop_region();}}

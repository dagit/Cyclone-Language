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
extern struct _fat_ptr Cyc_strcpy(struct _fat_ptr,struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 135 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 156
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 182
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U,Cyc_Absyn_AqualKind =8U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 294
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 307
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 314
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 481 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 488
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 506
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 520
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 683 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 847
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 872
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 880
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 886
void*Cyc_Absyn_compress(void*);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 901
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 904
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 906
extern void*Cyc_Absyn_sint_type;
# 908
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 911
extern void*Cyc_Absyn_complex_type(void*);
# 913
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_shorthand_type;extern void*Cyc_Absyn_refcnt_rgn_shorthand_type;
# 915
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 919
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 921
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_tuple_type(struct Cyc_List_List*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 940
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 952
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 956
void*Cyc_Absyn_bounds_one (void);
# 958
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 978
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 981
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 985
void*Cyc_Absyn_aqualsof_type(void*);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1042
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
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1075
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1092
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1096
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1101
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1103
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1111
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1118
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1121
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1126
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 26 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 36
extern int Cyc_Flags_no_register;
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 30
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 36
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 52 "kinds.h"
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
# 54
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_iko;
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
# 202
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 230
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 240
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
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 31
void*Cyc_CurRgn_curr_rgn_type (void);
# 67 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Complex_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Parse_Declarator f0;struct Cyc_Absyn_Exp*f1;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct _tuple13{struct _tuple13*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
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
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 114
return first;}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};
# 169
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 174
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 176
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=msg;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Parse_Exit_exn_struct));_Tmp0->tag=Cyc_Parse_Exit;_Tmp0;}));}
# 181
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 183
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;_Tmp1=tqt->f1;_Tmp0=tqt->f2;{struct Cyc_Absyn_Tqual tq=_Tmp1;void*t=_Tmp0;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc!=0U)loc=tq.loc;
Cyc_Warn_warn(loc,_tag_fat("qualifier on type is ignored",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));}
# 188
return t;}}
# 191
static void*Cyc_Parse_make_pointer_mod(struct _RegionHandle*r,struct Cyc_Absyn_PtrLoc*loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs,struct Cyc_Absyn_Tqual tqs){
# 196
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*autoreleased=Cyc_Tcutil_any_bool(0);
void*aqual=0;
for(1;pqs!=0;pqs=pqs->tl){
void*_Tmp0=(void*)pqs->hd;void*_Tmp1;switch(*((int*)_Tmp0)){case 4:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 6:
 autoreleased=Cyc_Absyn_true_type;goto _LL0;case 8:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 7:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0: _Tmp1=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}case 1: _Tmp1=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{void*t=_Tmp1;
rgn=t;goto _LL0;}default: _Tmp1=(void*)((struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{void*aq=_Tmp1;
aqual=aq;goto _LL0;}}_LL0:;}
# 212
return(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_Tmp0->tag=2,_Tmp0->f1.rgn=rgn,_Tmp0->f1.nullable=nullable,_Tmp0->f1.bounds=bound,_Tmp0->f1.zero_term=zeroterm,_Tmp0->f1.ptrloc=loc,_Tmp0->f1.autoreleased=autoreleased,
# 214
aqual!=0?_Tmp0->f1.aqual=aqual:({void*_Tmp1=({void*_Tmp2=Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,0);Cyc_Absyn_aqual_var_type(_Tmp2,Cyc_Absyn_rtd_qual_type);});_Tmp0->f1.aqual=_Tmp1;}),_Tmp0->f2=tqs;_Tmp0;});}
# 220
static struct _tuple0*Cyc_Parse_gensym_enum (void){
static int enum_counter=0;
return({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);_Tmp0->f0=_Tmp1;}),({
struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)enum_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("__anonymous_enum_%d__",sizeof(char),22U),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;});_Tmp0;});}struct _tuple15{unsigned f0;struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple17{struct _tuple15*f0;struct _tuple16*f1;};
# 226
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple17*field_info){
# 231
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;unsigned _Tmp7;_Tmp7=field_info->f0->f0;_Tmp6=field_info->f0->f1;_Tmp5=field_info->f0->f2;_Tmp4=field_info->f0->f3;_Tmp3=field_info->f0->f4;_Tmp2=field_info->f0->f5;_Tmp1=field_info->f1->f0;_Tmp0=field_info->f1->f1;{unsigned varloc=_Tmp7;struct _tuple0*qid=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;struct Cyc_List_List*tvs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;struct Cyc_Absyn_Exp*widthopt=_Tmp1;struct Cyc_Absyn_Exp*reqopt=_Tmp0;
if(tvs!=0)
Cyc_Warn_err(loc,_tag_fat("bad type params in struct field",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_Absyn_is_qvar_qualified(qid))
Cyc_Warn_err(loc,_tag_fat("struct or union field cannot be qualified with a namespace",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));
return({struct Cyc_Absyn_Aggrfield*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp8->name=(*qid).f1,_Tmp8->tq=tq,_Tmp8->type=t,_Tmp8->width=widthopt,_Tmp8->attributes=atts,_Tmp8->requires_clause=reqopt;_Tmp8;});}}
# 241
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _Tmp0;_Tmp0.Signed_spec=0,_Tmp0.Unsigned_spec=0,_Tmp0.Short_spec=0,_Tmp0.Long_spec=0,_Tmp0.Long_Long_spec=0,_Tmp0.Complex_spec=0,_Tmp0.Valid_type_spec=0,_Tmp0.Type_spec=Cyc_Absyn_sint_type,_Tmp0.loc=loc;_Tmp0;});}
# 253
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 259
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Signed_spec=1;
return s;}
# 264
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Unsigned_spec=1;
return s;}
# 269
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Short_spec=1;
return s;}
# 274
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Long_spec=1;
return s;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_complex_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Complex_spec=1;
return s;}
# 286
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 288
return Cyc_Tcutil_is_array_type(t)?({
void*_Tmp0=t;void*_Tmp1=argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type;Cyc_Tcutil_promote_array(_Tmp0,_Tmp1,Cyc_Absyn_al_qual_type,0);}): t;}struct _tuple18{struct _fat_ptr*f0;void*f1;};
# 301 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
struct _tuple8*_Tmp0=(struct _tuple8*)x->hd;struct _fat_ptr _Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)((struct _tuple8*)_Tmp0)->f2)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->tl==0){_Tmp3=_Tmp0->f0;_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f2)->f2->hd;if((struct _fat_ptr*)_Tmp3!=0){struct _fat_ptr*v=_Tmp3;void*i=_Tmp2;
# 306
{void*_Tmp4;if(*((int*)i)==1){_Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)i)->f2;{void**z=(void**)_Tmp4;
# 310
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp5=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));});*nm=_Tmp5;});
({void*_Tmp5=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp6->name=nm,_Tmp6->identity=-1,({void*_Tmp7=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp8->tag=0,_Tmp8->f1=& Cyc_Kinds_ik;_Tmp8;});_Tmp6->kind=_Tmp7;}),_Tmp6->aquals_bound=0;_Tmp6;}));*z=_Tmp5;});
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 315
res=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp5=({struct _tuple18*_Tmp6=_cycalloc(sizeof(struct _tuple18));_Tmp6->f0=v,_Tmp6->f1=i;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=res;_Tmp4;});goto _LL0;}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->hd)==1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f2)->f2)->tl==0){_Tmp1=*_Tmp0->f0;_Tmp3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f2)->f2->hd)->f2;{struct _fat_ptr v=_Tmp1;void**z=(void**)_Tmp3;
# 319
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)v);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp6,sizeof(void*),1));});*nm=_Tmp4;});
({void*_Tmp4=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp5->name=nm,_Tmp5->identity=-1,({void*_Tmp6=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp7->tag=0,_Tmp7->f1=& Cyc_Kinds_rk;_Tmp7;});_Tmp5->kind=_Tmp6;}),_Tmp5->aquals_bound=0;_Tmp5;}));*z=_Tmp4;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f0!=0)goto _LL5;else{_LL5:
 goto _LL0;}}_LL0:;}
# 324
return res;}
# 328
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
void*_Tmp0=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->tl==0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*i=_Tmp1;
# 333
res=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp3=({struct _tuple18*_Tmp4=_cycalloc(sizeof(struct _tuple18));_Tmp4->f0=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_Tmp4->f1=i;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=res;_Tmp2;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 336
return res;}
# 340
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)==0){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f0.Rel_n.tag==1){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f0.Rel_n.val==0){_Tmp1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1->f1;{struct _fat_ptr*y=_Tmp1;
# 343
{struct Cyc_List_List*ts=tags;for(0;ts!=0;ts=ts->tl){
struct _tuple18*_Tmp2=(struct _tuple18*)ts->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f0;_Tmp3=_Tmp2->f1;{struct _fat_ptr*x=_Tmp4;void*i=_Tmp3;
if(Cyc_strptrcmp(x,y)==0)
return({void*_Tmp5=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp6->tag=38,({void*_Tmp7=Cyc_Tcutil_copy_type(i);_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_exp(_Tmp5,e->loc);});}}}
# 348
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 351
return e;}
# 356
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;switch(*((int*)t)){case 4: _Tmp9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;_Tmp6=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;struct Cyc_Absyn_Exp*nelts=_Tmp7;void*zt=_Tmp6;unsigned ztloc=_Tmp5;
# 359
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts!=0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*et2=Cyc_Parse_substitute_tags(tags,et);
if(nelts!=nelts2 || et!=et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3: _Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.rgn;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.nullable;_Tmp4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.ptrloc;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.autoreleased;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.aqual;{void*et=_Tmp9;struct Cyc_Absyn_Tqual tq=_Tmp8;void*r=_Tmp7;void*n=_Tmp6;void*b=_Tmp4;void*zt=_Tmp3;struct Cyc_Absyn_PtrLoc*pl=_Tmp2;void*rel=_Tmp1;void*aq=_Tmp0;
# 367
void*et2=Cyc_Parse_substitute_tags(tags,et);
void*b2=Cyc_Parse_substitute_tags(tags,b);
if(et2!=et || b2!=b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _TmpA;_TmpA.elt_type=et2,_TmpA.elt_tq=tq,_TmpA.ptr_atts.rgn=r,_TmpA.ptr_atts.nullable=n,_TmpA.ptr_atts.bounds=b2,_TmpA.ptr_atts.zero_term=zt,_TmpA.ptr_atts.ptrloc=pl,_TmpA.ptr_atts.autoreleased=rel,_TmpA.ptr_atts.aqual=aq;_TmpA;}));
goto _LL0;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==14){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2)->tl==0){_Tmp9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2->hd;{void*t=_Tmp9;
# 373
void*t2=Cyc_Parse_substitute_tags(tags,t);
if(t!=t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 8: _Tmp9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 377
struct Cyc_Absyn_Exp*e2=Cyc_Parse_substitute_tags_exp(tags,e);
if(e2!=e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9:
 goto _LL0;}_LL0:;}
# 382
return t;}
# 387
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_Tmp0=Cyc_Parse_substitute_tags(tags,x->type);x->type=_Tmp0;});}struct _tuple19{struct Cyc_Absyn_Tqual f0;void*f1;};struct _tuple20{void*f0;struct Cyc_Absyn_Tqual f1;void*f2;};
# 394
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple20*t){
return({struct _tuple19*_Tmp0=_cycalloc(sizeof(struct _tuple19));_Tmp0->f0=(*t).f1,_Tmp0->f1=(*t).f2;_Tmp0;});}
# 398
static int Cyc_Parse_is_typeparam(void*tm){
if(*((int*)tm)==4)
return 1;else{
return 0;};}
# 407
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k,void*aliashint,unsigned loc){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
return Cyc_Absyn_heap_rgn_type;
# 411
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
return Cyc_Absyn_unique_rgn_shorthand_type;
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
return Cyc_Absyn_refcnt_rgn_shorthand_type;
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
return Cyc_CurRgn_curr_rgn_type();
aliashint=Cyc_Kinds_consistent_aliashint(loc,k,aliashint);
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->name=_Tmp1;}),_Tmp0->identity=-1,_Tmp0->kind=k,_Tmp0->aquals_bound=aliashint;_Tmp0;}));}
# 421
static void*Cyc_Parse_id2aqual(unsigned loc,struct _fat_ptr s){
if(Cyc_strlen(s)==2U){
char _Tmp0=((const char*)s.curr)[1];switch((int)_Tmp0){case 65:
 return Cyc_Absyn_al_qual_type;case 85:
 return Cyc_Absyn_un_qual_type;case 84:
 return Cyc_Absyn_rtd_qual_type;default:
 goto _LL0;}_LL0:;}else{
# 430
if(Cyc_strlen(s)==3U){
if((int)((const char*)s.curr)[1]==82 &&(int)((const char*)s.curr)[2]==67)
return Cyc_Absyn_rc_qual_type;}}
# 434
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("bad aqual bound ",sizeof(char),17U);_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=s;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return Cyc_Absyn_al_qual_type;}
# 438
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of heap region",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of unique region",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of refcounted region",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of \"current\" region",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));}
# 453
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_Tmp0;if(*((int*)t)==2){_Tmp0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*pr=_Tmp0;
return pr;}}else{
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,_tag_fat("expecting a list of type variables, not types",sizeof(char),46U));};}
# 461
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=(void**)&((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1->kind;{void**cptr=(void**)_Tmp1;
# 464
void*_Tmp2=Cyc_Kinds_compress_kb(*cptr);if(*((int*)_Tmp2)==1){
# 466
({void*_Tmp3=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp4->tag=2,_Tmp4->f1=0,_Tmp4->f2=k;_Tmp4;}): Cyc_Kinds_kind_to_bound(k);*cptr=_Tmp3;});return;}else{
return;};}}else{
# 469
return;};}
# 474
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 482
if(tms==0)return 0;{
# 484
void*_Tmp0=(void*)tms->hd;void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp1;
# 487
if(tms->tl==0 ||
 Cyc_Parse_is_typeparam((void*)tms->tl->hd)&& tms->tl->tl==0){
# 490
void*_Tmp2;if(*((int*)args)==1){
# 492
Cyc_Warn_warn(loc,
_tag_fat("function declaration with both new- and old-style parameter declarations; ignoring old-style",sizeof(char),93U),_tag_fat(0U,sizeof(void*),0));
return tms;}else{_Tmp2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f1;{struct Cyc_List_List*ids=_Tmp2;
# 496
if(({int _Tmp3=Cyc_List_length(ids);_Tmp3!=Cyc_List_length(tds);}))
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,
_tag_fat("wrong number of parameter declarations in old-style function declaration",sizeof(char),73U));{
# 500
struct Cyc_List_List*rev_new_params=0;
for(1;ids!=0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2!=0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)tds2->hd;
void*_Tmp3=x->r;void*_Tmp4;if(*((int*)_Tmp3)==0){_Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 507
if(Cyc_zstrptrcmp((*vd->name).f1,(struct _fat_ptr*)ids->hd)!=0)
continue;
if(vd->initializer!=0)
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,_tag_fat("initializer found in parameter declaration",sizeof(char),43U));
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,_tag_fat("namespaces forbidden in parameter declarations",sizeof(char),47U));
rev_new_params=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp6=({struct _tuple8*_Tmp7=_cycalloc(sizeof(struct _tuple8));_Tmp7->f0=(*vd->name).f1,_Tmp7->f1=vd->tq,_Tmp7->f2=vd->type;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=rev_new_params;_Tmp5;});
# 515
goto L;}}else{
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,_tag_fat("nonvariable declaration in parameter type",sizeof(char),42U));};}
# 519
L: if(tds2==0)
({unsigned _Tmp3=loc;({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(_Tmp3,Cyc_strconcat(*((struct _fat_ptr*)ids->hd),_tag_fat(" is not given a type",sizeof(char),21U)));});}
# 522
return({struct Cyc_List_List*_Tmp3=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));
({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yy,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yy,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp7->tag=1,({struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(rev_new_params);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=0,_Tmp7->f3=0,_Tmp7->f4=0,_Tmp7->f5=0,_Tmp7->f6=0,_Tmp7->f7=0,_Tmp7->f8=0;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});}}};}
# 529
goto _LL4;}}else{_LL4:
 return({struct Cyc_List_List*_Tmp2=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp3=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_Tmp2->tl=_Tmp3;});_Tmp2;});};}}
# 537
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 541
if(tds!=0)
d=({struct Cyc_Parse_Declarator _Tmp0;_Tmp0.id=d.id,_Tmp0.varloc=d.varloc,({struct Cyc_List_List*_Tmp1=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_Tmp0.tms=_Tmp1;});_Tmp0;});{
# 544
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 550
if(dso!=0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 556
enum Cyc_Parse_Storage_class _Tmp0=dso->sc;switch((int)_Tmp0){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
 Cyc_Warn_err(loc,_tag_fat("bad storage class on function",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}}{
# 563
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f0;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_Absyn_Tqual fn_tqual=_Tmp4;void*fn_type=_Tmp3;struct Cyc_List_List*x=_Tmp2;struct Cyc_List_List*out_atts=_Tmp1;
# 568
if(x!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));{
# 571
struct Cyc_Absyn_FnInfo _Tmp5;if(*((int*)fn_type)==5){_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fn_type)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp5;
# 573
{struct Cyc_List_List*args2=i.args;for(0;args2!=0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f0==0){
Cyc_Warn_err(loc,_tag_fat("missing argument variable in function prototype",sizeof(char),48U),_tag_fat(0U,sizeof(void*),0));
({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=_tag_fat("?",sizeof(char),2U);_Tmp7;});(*((struct _tuple8*)args2->hd)).f0=_Tmp6;});}}}
# 580
({struct Cyc_List_List*_Tmp6=Cyc_List_append(i.attributes,out_atts);i.attributes=_Tmp6;});
return({struct Cyc_Absyn_Fndecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_Tmp6->sc=sc,_Tmp6->is_inline=is_inline,_Tmp6->name=d.id,_Tmp6->body=body,_Tmp6->i=i,_Tmp6->cached_type=0,_Tmp6->param_vardecls=0,_Tmp6->fn_vardecl=0,_Tmp6->orig_scope=sc;_Tmp6;});}}else{
# 584
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,_tag_fat("declarator is not a function prototype",sizeof(char),39U));};}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 588
static struct _fat_ptr Cyc_Parse_msg1={_TmpG0,_TmpG0,_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 590
static struct _fat_ptr Cyc_Parse_msg2={_TmpG1,_TmpG1,_TmpG1 + 84U};
# 597
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 600
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
# 625
return s1;}
# 631
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 639
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
# 652
if(!seen_type){
if(!seen_sign && !seen_size){
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_double_type);
Cyc_Warn_warn(loc,_tag_fat("missing type within specifier",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));}
# 658
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_int_type(sgn,sz));else{
return Cyc_Absyn_int_type(sgn,sz);}}
# 662
{enum Cyc_Absyn_Size_of _Tmp0;enum Cyc_Absyn_Sign _Tmp1;if(*((int*)t)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 1: _Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_Tmp1;enum Cyc_Absyn_Size_of sz2=_Tmp0;
# 664
if(seen_sign &&(int)sgn2!=(int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 668
if(seen_size &&(int)sz2!=(int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;}case 2:
# 674
 if(seen_size)
t=Cyc_Absyn_long_double_type;
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;default: goto _LL5;}else{_LL5:
# 680
 if(seen_sign)
Cyc_Warn_err(loc,_tag_fat("sign specification on non-integral type",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
if(seen_size)
Cyc_Warn_err(loc,_tag_fat("size qualifier on non-integral type",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}_LL0:;}
# 686
return t;}
# 689
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 693
if(ds==0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*q=d.id;
unsigned varloc=d.varloc;
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f0;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_Absyn_Tqual tq2=_Tmp4;void*new_typ=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 700
struct Cyc_List_List*tl=ds->tl==0?0:({struct _RegionHandle*_Tmp5=r;struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=Cyc_Tcutil_copy_type(t);struct _tuple13*_Tmp8=ds->tl;Cyc_Parse_apply_tmss(_Tmp5,_Tmp6,_Tmp7,_Tmp8,shared_atts);});
return({struct Cyc_List_List*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_region_malloc(r,0U,sizeof(struct _tuple15));_Tmp7->f0=varloc,_Tmp7->f1=q,_Tmp7->f2=tq2,_Tmp7->f3=new_typ,_Tmp7->f4=tvs,_Tmp7->f5=atts;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=tl;_Tmp5;});}}}
# 704
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 707
if(tms==0)return({struct _tuple14 _Tmp0;_Tmp0.f0=tq,_Tmp0.f1=t,_Tmp0.f2=0,_Tmp0.f3=atts;_Tmp0;});{
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;struct Cyc_Absyn_PtrAtts _Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*zeroterm=_Tmp5;unsigned ztloc=_Tmp4;
# 710
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 710
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 1: _Tmp5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp5;void*zeroterm=_Tmp3;unsigned ztloc=_Tmp4;
# 713
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 713
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 3: _Tmp5=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp5;
# 716
unsigned _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;int _TmpD;void*_TmpE;if(*((int*)args)==1){_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f1;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f2;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f3;_TmpB=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f4;_TmpA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f5;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f6;_Tmp8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f7;_Tmp7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f8;{struct Cyc_List_List*args2=_TmpE;int c_vararg=_TmpD;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpC;void*eff=_TmpB;struct Cyc_List_List*rgn_po=_TmpA;struct Cyc_List_List*qb=_Tmp9;struct Cyc_Absyn_Exp*req=_Tmp8;struct Cyc_Absyn_Exp*ens=_Tmp7;
# 718
struct Cyc_List_List*typvars=0;
# 720
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as!=0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(void*)as->hd,_TmpF->tl=fn_atts;_TmpF;});else{
# 725
new_atts=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(void*)as->hd,_TmpF->tl=new_atts;_TmpF;});}}}
# 727
if(tms->tl!=0){
void*_TmpF=(void*)tms->tl->hd;void*_Tmp10;if(*((int*)_TmpF)==4){_Tmp10=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_TmpF)->f1;{struct Cyc_List_List*ts=_Tmp10;
# 730
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{
goto _LL12;}_LL12:;}
# 736
if(((((!c_vararg && cyc_vararg==0)&& args2!=0)&& args2->tl==0)&&(*((struct _tuple8*)args2->hd)).f0==0)&&(*((struct _tuple8*)args2->hd)).f2==Cyc_Absyn_void_type)
# 741
args2=0;{
# 746
struct Cyc_List_List*new_requires=0;
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_TmpF=(struct _tuple8*)a->hd;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;_Tmp12=_TmpF->f0;_Tmp11=_TmpF->f1;_Tmp10=(void**)& _TmpF->f2;{struct _fat_ptr*vopt=_Tmp12;struct Cyc_Absyn_Tqual tq=_Tmp11;void**t=(void**)_Tmp10;
void*_Tmp13=*t;unsigned _Tmp14;void*_Tmp15;void*_Tmp16;struct Cyc_Absyn_Tqual _Tmp17;void*_Tmp18;if(*((int*)_Tmp13)==4){_Tmp18=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.elt_type;_Tmp17=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.tq;_Tmp16=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.num_elts;_Tmp15=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.zero_term;_Tmp14=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.zt_loc;{void*et=_Tmp18;struct Cyc_Absyn_Tqual tq=_Tmp17;struct Cyc_Absyn_Exp*neltsopt=_Tmp16;void*zt=_Tmp15;unsigned ztloc=_Tmp14;
# 751
if(neltsopt!=0 && vopt!=0){
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0)),v->f0.Loc_n.tag=4U,v->f0.Loc_n.val=0,v->f1=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,({struct Cyc_Absyn_Exp*_Tmp19[1];({struct Cyc_Absyn_Exp*_Tmp1A=Cyc_Absyn_var_exp(v,0U);_Tmp19[0]=_Tmp1A;});Cyc_List_list(_tag_fat(_Tmp19,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);
new_requires=({struct Cyc_List_List*_Tmp19=_cycalloc(sizeof(struct Cyc_List_List));_Tmp19->hd=new_req,_Tmp19->tl=new_requires;_Tmp19;});}}
# 758
goto _LL1A;}}else{
goto _LL1A;}_LL1A:;}}}
# 762
if(new_requires!=0){
struct Cyc_Absyn_Exp*r;
if(req!=0)
r=req;else{
# 767
r=(struct Cyc_Absyn_Exp*)new_requires->hd;
new_requires=new_requires->tl;}
# 770
for(1;new_requires!=0;new_requires=new_requires->tl){
r=Cyc_Absyn_and_exp(r,(struct Cyc_Absyn_Exp*)new_requires->hd,0U);}
req=r;}{
# 776
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 778
if(tags!=0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 783
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_TmpF=(struct _tuple8*)a->hd;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;_Tmp12=_TmpF->f0;_Tmp11=_TmpF->f1;_Tmp10=(void**)& _TmpF->f2;{struct _fat_ptr*vopt=_Tmp12;struct Cyc_Absyn_Tqual tq=_Tmp11;void**t=(void**)_Tmp10;
if(tags!=0)
({void*_Tmp13=Cyc_Parse_substitute_tags(tags,*t);*t=_Tmp13;});
({void*_Tmp13=Cyc_Parse_array2ptr(*t,1);*t=_Tmp13;});}}}
# 795
return({struct Cyc_Absyn_Tqual _TmpF=Cyc_Absyn_empty_tqual(tq.loc);void*_Tmp10=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,qb,fn_atts,req,ens);
# 795
struct Cyc_List_List*_Tmp11=new_atts;Cyc_Parse_apply_tms(_TmpF,_Tmp10,_Tmp11,
# 800
_check_null(tms)->tl);});}}}}else{_Tmp6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f2;{unsigned loc=_Tmp6;
# 802
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,_tag_fat("function declaration without parameter types",sizeof(char),45U));}};}case 4: _Tmp5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;unsigned loc=_Tmp4;
# 809
if(tms->tl==0)
return({struct _tuple14 _Tmp6;_Tmp6.f0=tq,_Tmp6.f1=t,_Tmp6.f2=ts,_Tmp6.f3=atts;_Tmp6;});
# 814
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,
_tag_fat("type parameters must appear before function arguments in declarator",sizeof(char),68U));}case 2: _Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 817
return({struct Cyc_Absyn_Tqual _Tmp6=tq2;void*_Tmp7=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp8;_Tmp8.elt_type=t,_Tmp8.elt_tq=tq,_Tmp8.ptr_atts=ptratts;_Tmp8;}));struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}default: _Tmp4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{unsigned loc=_Tmp4;struct Cyc_List_List*atts2=_Tmp5;
# 822
return({struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=t;struct Cyc_List_List*_Tmp8=Cyc_List_append(atts,atts2);Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}};}}
# 828
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 837
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple15*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;unsigned _Tmp5;_Tmp5=t->f0;_Tmp4=t->f1;_Tmp3=t->f2;_Tmp2=t->f3;_Tmp1=t->f4;_Tmp0=t->f5;{unsigned varloc=_Tmp5;struct _tuple0*x=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*typ=_Tmp2;struct Cyc_List_List*tvs=_Tmp1;struct Cyc_List_List*atts=_Tmp0;
# 840
Cyc_Lex_register_typedef(x);{
# 842
struct Cyc_Core_Opt*kind;
void*type;
{void*_Tmp6;if(*((int*)typ)==1){_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)typ)->f1;{struct Cyc_Core_Opt*kopt=_Tmp6;
# 846
type=0;
kind=kopt==0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}
# 851
return({void*_Tmp6=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_Tmp7->tag=8,({struct Cyc_Absyn_Typedefdecl*_Tmp8=({struct Cyc_Absyn_Typedefdecl*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));_Tmp9->name=x,_Tmp9->tvs=tvs,_Tmp9->kind=kind,_Tmp9->defn=type,_Tmp9->atts=atts,_Tmp9->tq=tq,_Tmp9->extern_c=0;_Tmp9;});_Tmp7->f1=_Tmp8;});_Tmp7;});Cyc_Absyn_new_decl(_Tmp6,loc);});}}}
# 858
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});Cyc_Absyn_new_stmt(_Tmp0,d->loc);});}
# 862
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right;})(Cyc_Parse_flatten_decl,ds,s);}
# 866
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es){
# 869
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
for(1;ds!=0;ds=ds->tl){
struct _tuple12 _Tmp0=ds->hd;void*_Tmp1;struct Cyc_Parse_Declarator _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_Parse_Declarator d=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
declarators=({struct _tuple13*_Tmp3=_region_malloc(r,0U,sizeof(struct _tuple13));_Tmp3->tl=declarators,_Tmp3->hd=d;_Tmp3;});
exprs=({struct Cyc_List_List*_Tmp3=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp3->hd=e,_Tmp3->tl=exprs;_Tmp3;});}}
# 876
({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(exprs);*es=_Tmp0;});
({struct _tuple13*_Tmp0=({(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;})(declarators);*decls=_Tmp0;});}
# 885
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 888
struct _RegionHandle _Tmp0=_new_region(0U,"mkrgn");struct _RegionHandle*mkrgn=& _Tmp0;_push_region(mkrgn);
{void*_Tmp1;struct Cyc_Parse_Type_specifier _Tmp2;struct Cyc_Absyn_Tqual _Tmp3;_Tmp3=ds.tq;_Tmp2=ds.type_specs;_Tmp1=ds.attributes;{struct Cyc_Absyn_Tqual tq=_Tmp3;struct Cyc_Parse_Type_specifier tss=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
if(tq.loc==0U)tq.loc=tqual_loc;
if(ds.is_inline)
Cyc_Warn_warn(loc,_tag_fat("inline qualifier on non-function definition",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));{
# 894
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
# 910
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 914
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es!=0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd!=0){
exps_empty=0;
break;}}}{
# 922
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators==0){
# 926
int _Tmp4;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;void*_Tmp7;switch(*((int*)base_type)){case 9: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1)->r)){case 0: _Tmp7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
# 928
({struct Cyc_List_List*_Tmp8=Cyc_List_append(ad->attributes,atts);ad->attributes=_Tmp8;});
ad->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 1: _Tmp7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;
# 932
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on enum not supported",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
ed->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpB->tag=7,_TmpB->f1=ed;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: _Tmp7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;
# 936
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on datatypes not supported",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));
dd->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=dd;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)){case 25: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.tag==1){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f0;_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f1;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp6;struct _tuple0*n=_Tmp7;struct Cyc_List_List*ts=_Tmp5;
# 940
struct Cyc_List_List*ts2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),ad->kind=k,ad->sc=s,ad->name=n,ad->tvs=ts2,ad->impl=0,ad->attributes=0,ad->expected_mem_kind=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on type declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{goto _LL25;}case 23: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.tag==2){_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl**tudp=_Tmp7;
# 945
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=*tudp;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.name;_Tmp4=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.is_extensible;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{struct _tuple0*n=_Tmp7;int isx=_Tmp4;struct Cyc_List_List*ts=_Tmp5;
# 948
struct Cyc_List_List*ts2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),_Tmp8->hd=tud,_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 20: _Tmp7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct _tuple0*n=_Tmp7;
# 953
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,ed->name=n,ed->fields=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 21: _Tmp7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct Cyc_List_List*fs=_Tmp7;
# 959
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,({struct _tuple0*_Tmp8=Cyc_Parse_gensym_enum();ed->name=_Tmp8;}),({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=fs;_Tmp9;});ed->fields=_Tmp8;});
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: goto _LL25;}default: _LL25:
 Cyc_Warn_err(loc,_tag_fat("missing declarator",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));{struct Cyc_List_List*_Tmp8=0;_npop_handler(0);return _Tmp8;}};}{
# 966
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);
if(istypedef){
# 971
if(!exps_empty)
Cyc_Warn_err(loc,_tag_fat("initializer in typedef declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*decls=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_Tmp4=decls;_npop_handler(0);return _Tmp4;}}{
# 977
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*ds=fields;for(0;ds!=0;(ds=ds->tl,exprs=_check_null(exprs)->tl)){
struct _tuple15*_Tmp4=(struct _tuple15*)ds->hd;void*_Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;unsigned _TmpA;_TmpA=_Tmp4->f0;_Tmp9=_Tmp4->f1;_Tmp8=_Tmp4->f2;_Tmp7=_Tmp4->f3;_Tmp6=_Tmp4->f4;_Tmp5=_Tmp4->f5;{unsigned varloc=_TmpA;struct _tuple0*x=_Tmp9;struct Cyc_Absyn_Tqual tq2=_Tmp8;void*t2=_Tmp7;struct Cyc_List_List*tvs2=_Tmp6;struct Cyc_List_List*atts2=_Tmp5;
if(tvs2!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(exprs==0)
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,_tag_fat("unexpected NULL in parse!",sizeof(char),26U));{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)_check_null(exprs)->hd);
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({struct Cyc_Absyn_Decl*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpF->tag=0,_TmpF->f1=vd;_TmpF;});_TmpD->r=_TmpE;}),_TmpD->loc=loc;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=decls;_TmpB;});}}}}{
# 990
struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(decls);_npop_handler(0);return _Tmp4;}}}}}}}}}
# 889
;_pop_region();}
# 994
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
long long _Tmp0;char _Tmp1;int _Tmp2;switch(x.LongLong_c.tag){case 5: _Tmp2=x.Int_c.val.f1;{int i=_Tmp2;
return(unsigned)i;}case 2: _Tmp1=x.Char_c.val.f1;{char c=_Tmp1;
return(unsigned)c;}case 6: _Tmp0=x.LongLong_c.val.f1;{long long x=_Tmp0;
# 999
unsigned long long y=(unsigned long long)x;
if(y > 4294967295U)
Cyc_Warn_err(loc,_tag_fat("integer constant too large",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
return(unsigned)x;}default:
# 1004
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err(loc,_tag_fat("expected integer constant but found %s",sizeof(char),39U),_tag_fat(_Tmp4,sizeof(void*),1));});
return 0U;};}
# 1010
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 14: _Tmp6=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*x=_Tmp6;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 1014
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_Tmp7,p->loc);});}}else{goto _LL13;}case 5: _Tmp6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp6;
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_Tmp7,p->loc);});}case 8:
 return Cyc_Absyn_null_exp(p->loc);case 9: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 10: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
return Cyc_Absyn_char_exp(c,p->loc);}case 11: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp4;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 15: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3==0){_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _tuple0*x=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 1021
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 16: _Tmp6=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
return e;}default: _LL13:
# 1026
 Cyc_Warn_err(p->loc,_tag_fat("cannot mix patterns and expressions in case",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_null_exp(p->loc);};}
# 1030
static struct _tuple16 Cyc_Parse_split_seq(struct Cyc_Absyn_Exp*maybe_seq){
void*_Tmp0=maybe_seq->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==9){_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1033
return({struct _tuple16 _Tmp3;_Tmp3.f0=e1,_Tmp3.f1=e2;_Tmp3;});}}else{
# 1035
return({struct _tuple16 _Tmp3;_Tmp3.f0=maybe_seq,_Tmp3.f1=0;_Tmp3;});};}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned f0;void*f1;void*f2;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f0;int f1;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple24{int f0;enum Cyc_Absyn_AggrKind f1;};struct _union_YYSTYPE_YY23{int tag;struct _tuple24 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY29{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f0;struct Cyc_Parse_Type_specifier f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY36{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f0;int f1;struct Cyc_Absyn_VarargInfo*f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _tuple28{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _union_YYSTYPE_YY51{int tag;struct _tuple28*val;};struct _tuple29{void*f0;void*f1;};struct _union_YYSTYPE_YY52{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _tuple30{struct Cyc_List_List*f0;unsigned f1;};struct _union_YYSTYPE_YY55{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;void*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _union_YYSTYPE_YY59{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY60{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY61{int tag;struct _tuple16 val;};struct _union_YYSTYPE_YY62{int tag;void*val;};struct _tuple31{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY64{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY65{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};struct _union_YYSTYPE_YY67{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY68{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YY67 YY67;struct _union_YYSTYPE_YY68 YY68;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1121
static void Cyc_yythrowfail(struct _fat_ptr s){
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=s;_Tmp0;}));}static char _TmpG2[7U]="cnst_t";
# 1088 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2,_TmpG2,_TmpG2 + 7U};
union Cyc_Absyn_Cnst _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Int_tok.tag==1){_Tmp0=yy1->Int_tok.val;{union Cyc_Absyn_Cnst yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1095
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Int_tok.tag=1U,_Tmp0.Int_tok.val=yy1;_Tmp0;});}static char _TmpG3[5U]="char";
# 1089 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3,_TmpG3,_TmpG3 + 5U};
char _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Char_tok.tag==2){_Tmp0=yy1->Char_tok.val;{char yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1096
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Char_tok.tag=2U,_Tmp0.Char_tok.val=yy1;_Tmp0;});}static char _TmpG4[13U]="string_t<`H>";
# 1090 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4,_TmpG4,_TmpG4 + 13U};
struct _fat_ptr _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->String_tok.tag==3){_Tmp0=yy1->String_tok.val;{struct _fat_ptr yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1097
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.String_tok.tag=3U,_Tmp0.String_tok.val=yy1;_Tmp0;});}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1093 "parse.y"
static struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG5,_TmpG5,_TmpG5 + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY1.tag==7){_Tmp0=yy1->YY1.val;{struct _tuple21*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1100
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY1.tag=7U,_Tmp0.YY1.val=yy1;_Tmp0;});}static char _TmpG6[11U]="ptrbound_t";
# 1094 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG6,_TmpG6,_TmpG6 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY2.tag==8){_Tmp0=yy1->YY2.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1101
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY2.tag=8U,_Tmp0.YY2.val=yy1;_Tmp0;});}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1095 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG7,_TmpG7,_TmpG7 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY3.tag==9){_Tmp0=yy1->YY3.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1102
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY3.tag=9U,_Tmp0.YY3.val=yy1;_Tmp0;});}static char _TmpG8[6U]="exp_t";
# 1096 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG8,_TmpG8,_TmpG8 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Exp_tok.tag==5){_Tmp0=yy1->Exp_tok.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1103
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Exp_tok.tag=5U,_Tmp0.Exp_tok.val=yy1;_Tmp0;});}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG9,_TmpG9,_TmpG9 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY4.tag==10){_Tmp0=yy1->YY4.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1111
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY4.tag=10U,_Tmp0.YY4.val=yy1;_Tmp0;});}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1105 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGA,_TmpGA,_TmpGA + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY5.tag==11){_Tmp0=yy1->YY5.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1112
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY5.tag=11U,_Tmp0.YY5.val=yy1;_Tmp0;});}static char _TmpGB[9U]="primop_t";
# 1106 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGB,_TmpGB,_TmpGB + 9U};
enum Cyc_Absyn_Primop _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY6.tag==12){_Tmp0=yy1->YY6.val;{enum Cyc_Absyn_Primop yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1113
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY6.tag=12U,_Tmp0.YY6.val=yy1;_Tmp0;});}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1107 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGC,_TmpGC,_TmpGC + 19U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY7.tag==13){_Tmp0=yy1->YY7.val;{struct Cyc_Core_Opt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1114
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY7.tag=13U,_Tmp0.YY7.val=yy1;_Tmp0;});}static char _TmpGD[7U]="qvar_t";
# 1108 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGD,_TmpGD,_TmpGD + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->QualId_tok.tag==4){_Tmp0=yy1->QualId_tok.val;{struct _tuple0*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1115
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.QualId_tok.tag=4U,_Tmp0.QualId_tok.val=yy1;_Tmp0;});}static char _TmpGE[7U]="stmt_t";
# 1111 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGE,_TmpGE,_TmpGE + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Stmt_tok.tag==6){_Tmp0=yy1->Stmt_tok.val;{struct Cyc_Absyn_Stmt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1118
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Stmt_tok.tag=6U,_Tmp0.Stmt_tok.val=yy1;_Tmp0;});}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1114 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGF,_TmpGF,_TmpGF + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY8.tag==14){_Tmp0=yy1->YY8.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1121
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY8.tag=14U,_Tmp0.YY8.val=yy1;_Tmp0;});}static char _TmpG10[6U]="pat_t";
# 1115 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG10,_TmpG10,_TmpG10 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY9.tag==15){_Tmp0=yy1->YY9.val;{struct Cyc_Absyn_Pat*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1122
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY9.tag=15U,_Tmp0.YY9.val=yy1;_Tmp0;});}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1120 "parse.y"
static struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG11,_TmpG11,_TmpG11 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY10.tag==16){_Tmp0=yy1->YY10.val;{struct _tuple22*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1127
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY10.tag=16U,_Tmp0.YY10.val=yy1;_Tmp0;});}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1121 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG12,_TmpG12,_TmpG12 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY11.tag==17){_Tmp0=yy1->YY11.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1128
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY11.tag=17U,_Tmp0.YY11.val=yy1;_Tmp0;});}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1122 "parse.y"
static struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG13,_TmpG13,_TmpG13 + 36U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY12.tag==18){_Tmp0=yy1->YY12.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1129
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY12.tag=18U,_Tmp0.YY12.val=yy1;_Tmp0;});}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1123 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG14,_TmpG14,_TmpG14 + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY13.tag==19){_Tmp0=yy1->YY13.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1130
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY13.tag=19U,_Tmp0.YY13.val=yy1;_Tmp0;});}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1124 "parse.y"
static struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG15,_TmpG15,_TmpG15 + 58U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY14.tag==20){_Tmp0=yy1->YY14.val;{struct _tuple22*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1131
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY14.tag=20U,_Tmp0.YY14.val=yy1;_Tmp0;});}static char _TmpG16[9U]="fndecl_t";
# 1125 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG16,_TmpG16,_TmpG16 + 9U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY15.tag==21){_Tmp0=yy1->YY15.val;{struct Cyc_Absyn_Fndecl*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1132
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY15.tag=21U,_Tmp0.YY15.val=yy1;_Tmp0;});}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1126 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG17,_TmpG17,_TmpG17 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY16.tag==22){_Tmp0=yy1->YY16.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1133
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY16.tag=22U,_Tmp0.YY16.val=yy1;_Tmp0;});}static char _TmpG18[12U]="decl_spec_t";
# 1129 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG18,_TmpG18,_TmpG18 + 12U};
struct Cyc_Parse_Declaration_spec _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY17.tag==23){_Tmp0=yy1->YY17.val;{struct Cyc_Parse_Declaration_spec yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1136
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY17.tag=23U,_Tmp0.YY17.val=yy1;_Tmp0;});}static char _TmpG19[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1130 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG19,_TmpG19,_TmpG19 + 31U};
struct _tuple12 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY18.tag==24){_Tmp0=yy1->YY18.val;{struct _tuple12 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1137
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY18.tag=24U,_Tmp0.YY18.val=yy1;_Tmp0;});}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1131 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1A,_TmpG1A,_TmpG1A + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY19.tag==25){_Tmp0=yy1->YY19.val;{struct _tuple11*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1138
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY19.tag=25U,_Tmp0.YY19.val=yy1;_Tmp0;});}static char _TmpG1B[16U]="storage_class_t";
# 1132 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1B,_TmpG1B,_TmpG1B + 16U};
enum Cyc_Parse_Storage_class _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY20.tag==26){_Tmp0=yy1->YY20.val;{enum Cyc_Parse_Storage_class yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1139
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY20.tag=26U,_Tmp0.YY20.val=yy1;_Tmp0;});}static char _TmpG1C[17U]="type_specifier_t";
# 1133 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1C,_TmpG1C,_TmpG1C + 17U};
struct Cyc_Parse_Type_specifier _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY21.tag==27){_Tmp0=yy1->YY21.val;{struct Cyc_Parse_Type_specifier yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1140
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY21.tag=27U,_Tmp0.YY21.val=yy1;_Tmp0;});}static char _TmpG1D[12U]="aggr_kind_t";
# 1135 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1D,_TmpG1D,_TmpG1D + 12U};
enum Cyc_Absyn_AggrKind _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY22.tag==28){_Tmp0=yy1->YY22.val;{enum Cyc_Absyn_AggrKind yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1142
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY22.tag=28U,_Tmp0.YY22.val=yy1;_Tmp0;});}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1136 "parse.y"
static struct _tuple24 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1E,_TmpG1E,_TmpG1E + 20U};
struct _tuple24 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY23.tag==29){_Tmp0=yy1->YY23.val;{struct _tuple24 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1143
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple24 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY23.tag=29U,_Tmp0.YY23.val=yy1;_Tmp0;});}static char _TmpG1F[8U]="tqual_t";
# 1137 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1F,_TmpG1F,_TmpG1F + 8U};
struct Cyc_Absyn_Tqual _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY24.tag==30){_Tmp0=yy1->YY24.val;{struct Cyc_Absyn_Tqual yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1144
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY24.tag=30U,_Tmp0.YY24.val=yy1;_Tmp0;});}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1138 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG20,_TmpG20,_TmpG20 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY25.tag==31){_Tmp0=yy1->YY25.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1145
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY25.tag=31U,_Tmp0.YY25.val=yy1;_Tmp0;});}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1139 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG21,_TmpG21,_TmpG21 + 34U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY26.tag==32){_Tmp0=yy1->YY26.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1146
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY26.tag=32U,_Tmp0.YY26.val=yy1;_Tmp0;});}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1140 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG22,_TmpG22,_TmpG22 + 33U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY27.tag==33){_Tmp0=yy1->YY27.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1147
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY27.tag=33U,_Tmp0.YY27.val=yy1;_Tmp0;});}static char _TmpG23[18U]="declarator_t<`yy>";
# 1141 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG23,_TmpG23,_TmpG23 + 18U};
struct Cyc_Parse_Declarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY28.tag==34){_Tmp0=yy1->YY28.val;{struct Cyc_Parse_Declarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1148
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY28.tag=34U,_Tmp0.YY28.val=yy1;_Tmp0;});}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1142 "parse.y"
static struct _tuple25*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG24,_TmpG24,_TmpG24 + 45U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY29.tag==35){_Tmp0=yy1->YY29.val;{struct _tuple25*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1149
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple25*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY29.tag=35U,_Tmp0.YY29.val=yy1;_Tmp0;});}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1143 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG25,_TmpG25,_TmpG25 + 57U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY30.tag==36){_Tmp0=yy1->YY30.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1150
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY30.tag=36U,_Tmp0.YY30.val=yy1;_Tmp0;});}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1144 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG26,_TmpG26,_TmpG26 + 26U};
struct Cyc_Parse_Abstractdeclarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY31.tag==37){_Tmp0=yy1->YY31.val;{struct Cyc_Parse_Abstractdeclarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1151
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY31.tag=37U,_Tmp0.YY31.val=yy1;_Tmp0;});}static char _TmpG27[5U]="bool";
# 1145 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG27,_TmpG27,_TmpG27 + 5U};
int _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY32.tag==38){_Tmp0=yy1->YY32.val;{int yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1152
static union Cyc_YYSTYPE Cyc_YY32(int yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY32.tag=38U,_Tmp0.YY32.val=yy1;_Tmp0;});}static char _TmpG28[8U]="scope_t";
# 1146 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG28,_TmpG28,_TmpG28 + 8U};
enum Cyc_Absyn_Scope _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY33.tag==39){_Tmp0=yy1->YY33.val;{enum Cyc_Absyn_Scope yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1153
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY33.tag=39U,_Tmp0.YY33.val=yy1;_Tmp0;});}static char _TmpG29[16U]="datatypefield_t";
# 1147 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG29,_TmpG29,_TmpG29 + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY34.tag==40){_Tmp0=yy1->YY34.val;{struct Cyc_Absyn_Datatypefield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1154
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY34.tag=40U,_Tmp0.YY34.val=yy1;_Tmp0;});}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1148 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2A,_TmpG2A,_TmpG2A + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY35.tag==41){_Tmp0=yy1->YY35.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1155
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY35.tag=41U,_Tmp0.YY35.val=yy1;_Tmp0;});}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1149 "parse.y"
static struct _tuple26 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2B,_TmpG2B,_TmpG2B + 41U};
struct _tuple26 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY36.tag==42){_Tmp0=yy1->YY36.val;{struct _tuple26 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1156
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple26 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY36.tag=42U,_Tmp0.YY36.val=yy1;_Tmp0;});}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1150 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2C,_TmpG2C,_TmpG2C + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY37.tag==43){_Tmp0=yy1->YY37.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1157
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY37.tag=43U,_Tmp0.YY37.val=yy1;_Tmp0;});}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1151 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2D,_TmpG2D,_TmpG2D + 31U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY38.tag==44){_Tmp0=yy1->YY38.val;{struct _tuple8*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1158
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY38.tag=44U,_Tmp0.YY38.val=yy1;_Tmp0;});}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1152 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2E,_TmpG2E,_TmpG2E + 42U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY39.tag==45){_Tmp0=yy1->YY39.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1159
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY39.tag=45U,_Tmp0.YY39.val=yy1;_Tmp0;});}static char _TmpG2F[147U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1153 "parse.y"
static struct _tuple27*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2F,_TmpG2F,_TmpG2F + 147U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY40.tag==46){_Tmp0=yy1->YY40.val;{struct _tuple27*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1160
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple27*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY40.tag=46U,_Tmp0.YY40.val=yy1;_Tmp0;});}static char _TmpG30[8U]="types_t";
# 1154 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG30,_TmpG30,_TmpG30 + 8U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY41.tag==47){_Tmp0=yy1->YY41.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1161
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY41.tag=47U,_Tmp0.YY41.val=yy1;_Tmp0;});}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1156 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG31,_TmpG31,_TmpG31 + 24U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY42.tag==48){_Tmp0=yy1->YY42.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1163
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY42.tag=48U,_Tmp0.YY42.val=yy1;_Tmp0;});}static char _TmpG32[13U]="designator_t";
# 1157 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG32,_TmpG32,_TmpG32 + 13U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY43.tag==49){_Tmp0=yy1->YY43.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1164
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY43.tag=49U,_Tmp0.YY43.val=yy1;_Tmp0;});}static char _TmpG33[7U]="kind_t";
# 1158 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG33,_TmpG33,_TmpG33 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY44.tag==50){_Tmp0=yy1->YY44.val;{struct Cyc_Absyn_Kind*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1165
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY44.tag=50U,_Tmp0.YY44.val=yy1;_Tmp0;});}static char _TmpG34[7U]="type_t";
# 1159 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG34,_TmpG34,_TmpG34 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY45.tag==51){_Tmp0=yy1->YY45.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1166
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY45.tag=51U,_Tmp0.YY45.val=yy1;_Tmp0;});}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1160 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG35,_TmpG35,_TmpG35 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY46.tag==52){_Tmp0=yy1->YY46.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1167
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY46.tag=52U,_Tmp0.YY46.val=yy1;_Tmp0;});}static char _TmpG36[12U]="attribute_t";
# 1161 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG36,_TmpG36,_TmpG36 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY47.tag==53){_Tmp0=yy1->YY47.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1168
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY47.tag=53U,_Tmp0.YY47.val=yy1;_Tmp0;});}static char _TmpG37[12U]="enumfield_t";
# 1162 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG37,_TmpG37,_TmpG37 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY48.tag==54){_Tmp0=yy1->YY48.val;{struct Cyc_Absyn_Enumfield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1169
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY48.tag=54U,_Tmp0.YY48.val=yy1;_Tmp0;});}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1163 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG38,_TmpG38,_TmpG38 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY49.tag==55){_Tmp0=yy1->YY49.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1170
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY49.tag=55U,_Tmp0.YY49.val=yy1;_Tmp0;});}static char _TmpG39[11U]="type_opt_t";
# 1164 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG39,_TmpG39,_TmpG39 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY50.tag==56){_Tmp0=yy1->YY50.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1171
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY50.tag=56U,_Tmp0.YY50.val=yy1;_Tmp0;});}static char _TmpG3A[69U]="$(list_t<$(type_t,type_t)@`H,`H>, list_t<$(type_t,type_t)@`H,`H>)*`H";
# 1166 "parse.y"
static struct _tuple28*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3A,_TmpG3A,_TmpG3A + 69U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY51.tag==57){_Tmp0=yy1->YY51.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1173
static union Cyc_YYSTYPE Cyc_YY51(struct _tuple28*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY51.tag=57U,_Tmp0.YY51.val=yy1;_Tmp0;});}static char _TmpG3B[21U]="$(type_t, type_t)@`H";
# 1167 "parse.y"
static struct _tuple29*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3B,_TmpG3B,_TmpG3B + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY52.tag==58){_Tmp0=yy1->YY52.val;{struct _tuple29*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1174
static union Cyc_YYSTYPE Cyc_YY52(struct _tuple29*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY52.tag=58U,_Tmp0.YY52.val=yy1;_Tmp0;});}static char _TmpG3C[11U]="booltype_t";
# 1168 "parse.y"
static void*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3C,_TmpG3C,_TmpG3C + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY53.tag==59){_Tmp0=yy1->YY53.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1175
static union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY53.tag=59U,_Tmp0.YY53.val=yy1;_Tmp0;});}static char _TmpG3D[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1169 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3D,_TmpG3D,_TmpG3D + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY54.tag==60){_Tmp0=yy1->YY54.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1176
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY54.tag=60U,_Tmp0.YY54.val=yy1;_Tmp0;});}static char _TmpG3E[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1170 "parse.y"
static struct _tuple30*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3E,_TmpG3E,_TmpG3E + 48U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY55.tag==61){_Tmp0=yy1->YY55.val;{struct _tuple30*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1177
static union Cyc_YYSTYPE Cyc_YY55(struct _tuple30*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY55.tag=61U,_Tmp0.YY55.val=yy1;_Tmp0;});}static char _TmpG3F[18U]="list_t<qvar_t,`H>";
# 1171 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3F,_TmpG3F,_TmpG3F + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY56.tag==62){_Tmp0=yy1->YY56.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1178
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY56.tag=62U,_Tmp0.YY56.val=yy1;_Tmp0;});}static char _TmpG40[12U]="aqualtype_t";
# 1172 "parse.y"
static void*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG40,_TmpG40,_TmpG40 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY57.tag==63){_Tmp0=yy1->YY57.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1179
static union Cyc_YYSTYPE Cyc_YY57(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY57.tag=63U,_Tmp0.YY57.val=yy1;_Tmp0;});}static char _TmpG41[20U]="pointer_qual_t<`yy>";
# 1173 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG41,_TmpG41,_TmpG41 + 20U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY58.tag==64){_Tmp0=yy1->YY58.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1180
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY58.tag=64U,_Tmp0.YY58.val=yy1;_Tmp0;});}static char _TmpG42[21U]="pointer_quals_t<`yy>";
# 1174 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG42,_TmpG42,_TmpG42 + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY59.tag==65){_Tmp0=yy1->YY59.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1181
static union Cyc_YYSTYPE Cyc_YY59(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY59.tag=65U,_Tmp0.YY59.val=yy1;_Tmp0;});}static char _TmpG43[10U]="exp_opt_t";
# 1175 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG43,_TmpG43,_TmpG43 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY60.tag==66){_Tmp0=yy1->YY60.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1182
static union Cyc_YYSTYPE Cyc_YY60(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY60.tag=66U,_Tmp0.YY60.val=yy1;_Tmp0;});}static char _TmpG44[23U]="$(exp_opt_t,exp_opt_t)";
# 1176 "parse.y"
static struct _tuple16 Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG44,_TmpG44,_TmpG44 + 23U};
struct _tuple16 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY61.tag==67){_Tmp0=yy1->YY61.val;{struct _tuple16 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1183
static union Cyc_YYSTYPE Cyc_YY61(struct _tuple16 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY61.tag=67U,_Tmp0.YY61.val=yy1;_Tmp0;});}static char _TmpG45[10U]="raw_exp_t";
# 1177 "parse.y"
static void*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG45,_TmpG45,_TmpG45 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY62.tag==68){_Tmp0=yy1->YY62.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1184
static union Cyc_YYSTYPE Cyc_YY62(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY62.tag=68U,_Tmp0.YY62.val=yy1;_Tmp0;});}static char _TmpG46[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1178 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG46,_TmpG46,_TmpG46 + 112U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY63.tag==69){_Tmp0=yy1->YY63.val;{struct _tuple31*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1185
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY63.tag=69U,_Tmp0.YY63.val=yy1;_Tmp0;});}static char _TmpG47[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1179 "parse.y"
static struct _tuple28*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG47,_TmpG47,_TmpG47 + 73U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY64.tag==70){_Tmp0=yy1->YY64.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1186
static union Cyc_YYSTYPE Cyc_YY64(struct _tuple28*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY64.tag=70U,_Tmp0.YY64.val=yy1;_Tmp0;});}static char _TmpG48[28U]="list_t<string_t<`H>@`H, `H>";
# 1180 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG48,_TmpG48,_TmpG48 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY65.tag==71){_Tmp0=yy1->YY65.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1187
static union Cyc_YYSTYPE Cyc_YY65(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY65.tag=71U,_Tmp0.YY65.val=yy1;_Tmp0;});}static char _TmpG49[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1181 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG49,_TmpG49,_TmpG49 + 38U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY66.tag==72){_Tmp0=yy1->YY66.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1188
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY66.tag=72U,_Tmp0.YY66.val=yy1;_Tmp0;});}static char _TmpG4A[26U]="$(string_t<`H>, exp_t)@`H";
# 1182 "parse.y"
static struct _tuple32*Cyc_yyget_YY67(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4A,_TmpG4A,_TmpG4A + 26U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY67.tag==73){_Tmp0=yy1->YY67.val;{struct _tuple32*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1189
static union Cyc_YYSTYPE Cyc_YY67(struct _tuple32*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY67.tag=73U,_Tmp0.YY67.val=yy1;_Tmp0;});}static char _TmpG4B[16U]="exp_maker_fun_t";
# 1183 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY68(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_TmpG4B,_TmpG4B,_TmpG4B + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY68.tag==74){_Tmp0=yy1->YY68.val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1190
static union Cyc_YYSTYPE Cyc_YY68(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY68.tag=74U,_Tmp0.YY68.val=yy1;_Tmp0;});}struct Cyc_Yyltype{int timestamp;unsigned first_line;unsigned first_column;unsigned last_line;unsigned last_column;};
# 1206
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _Tmp0;_Tmp0.timestamp=0,_Tmp0.first_line=0U,_Tmp0.first_column=0U,_Tmp0.last_line=0U,_Tmp0.last_column=0U;_Tmp0;});}
# 1209
struct Cyc_Yyltype Cyc_yylloc={0,0U,0U,0U,0U};
# 1220 "parse.y"
static short Cyc_yytranslate[394U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,165,2,2,150,163,160,2,147,148,143,157,142,161,152,162,2,2,2,2,2,2,2,2,2,2,151,139,145,144,146,156,155,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,153,2,154,159,149,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,140,158,141,164,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138};static char _TmpG4C[2U]="$";static char _TmpG4D[6U]="error";static char _TmpG4E[12U]="$undefined.";static char _TmpG4F[5U]="AUTO";static char _TmpG50[9U]="REGISTER";static char _TmpG51[7U]="STATIC";static char _TmpG52[7U]="EXTERN";static char _TmpG53[8U]="TYPEDEF";static char _TmpG54[5U]="VOID";static char _TmpG55[5U]="CHAR";static char _TmpG56[6U]="SHORT";static char _TmpG57[4U]="INT";static char _TmpG58[5U]="LONG";static char _TmpG59[6U]="FLOAT";static char _TmpG5A[7U]="DOUBLE";static char _TmpG5B[7U]="SIGNED";static char _TmpG5C[9U]="UNSIGNED";static char _TmpG5D[6U]="CONST";static char _TmpG5E[9U]="VOLATILE";static char _TmpG5F[9U]="RESTRICT";static char _TmpG60[7U]="STRUCT";static char _TmpG61[6U]="UNION";static char _TmpG62[5U]="CASE";static char _TmpG63[8U]="DEFAULT";static char _TmpG64[7U]="INLINE";static char _TmpG65[7U]="SIZEOF";static char _TmpG66[9U]="OFFSETOF";static char _TmpG67[3U]="IF";static char _TmpG68[5U]="ELSE";static char _TmpG69[7U]="SWITCH";static char _TmpG6A[6U]="WHILE";static char _TmpG6B[3U]="DO";static char _TmpG6C[4U]="FOR";static char _TmpG6D[5U]="GOTO";static char _TmpG6E[9U]="CONTINUE";static char _TmpG6F[6U]="BREAK";static char _TmpG70[7U]="RETURN";static char _TmpG71[5U]="ENUM";static char _TmpG72[7U]="TYPEOF";static char _TmpG73[16U]="BUILTIN_VA_LIST";static char _TmpG74[10U]="EXTENSION";static char _TmpG75[8U]="COMPLEX";static char _TmpG76[8U]="NULL_kw";static char _TmpG77[4U]="LET";static char _TmpG78[6U]="THROW";static char _TmpG79[4U]="TRY";static char _TmpG7A[6U]="CATCH";static char _TmpG7B[7U]="EXPORT";static char _TmpG7C[9U]="OVERRIDE";static char _TmpG7D[5U]="HIDE";static char _TmpG7E[4U]="NEW";static char _TmpG7F[5U]="QNEW";static char _TmpG80[9U]="ABSTRACT";static char _TmpG81[9U]="FALLTHRU";static char _TmpG82[6U]="USING";static char _TmpG83[10U]="NAMESPACE";static char _TmpG84[9U]="DATATYPE";static char _TmpG85[7U]="MALLOC";static char _TmpG86[8U]="RMALLOC";static char _TmpG87[9U]="RVMALLOC";static char _TmpG88[15U]="RMALLOC_INLINE";static char _TmpG89[8U]="QMALLOC";static char _TmpG8A[7U]="CALLOC";static char _TmpG8B[8U]="QCALLOC";static char _TmpG8C[8U]="RCALLOC";static char _TmpG8D[5U]="SWAP";static char _TmpG8E[9U]="REGION_T";static char _TmpG8F[6U]="TAG_T";static char _TmpG90[7U]="REGION";static char _TmpG91[5U]="RNEW";static char _TmpG92[8U]="REGIONS";static char _TmpG93[7U]="PORTON";static char _TmpG94[8U]="PORTOFF";static char _TmpG95[7U]="PRAGMA";static char _TmpG96[10U]="TEMPESTON";static char _TmpG97[11U]="TEMPESTOFF";static char _TmpG98[13U]="AQ_ALIASABLE";static char _TmpG99[10U]="AQ_REFCNT";static char _TmpG9A[14U]="AQ_RESTRICTED";static char _TmpG9B[10U]="AQ_UNIQUE";static char _TmpG9C[8U]="AQUAL_T";static char _TmpG9D[8U]="NUMELTS";static char _TmpG9E[8U]="VALUEOF";static char _TmpG9F[10U]="VALUEOF_T";static char _TmpGA0[9U]="TAGCHECK";static char _TmpGA1[13U]="NUMELTS_QUAL";static char _TmpGA2[10U]="THIN_QUAL";static char _TmpGA3[9U]="FAT_QUAL";static char _TmpGA4[13U]="NOTNULL_QUAL";static char _TmpGA5[14U]="NULLABLE_QUAL";static char _TmpGA6[14U]="REQUIRES_QUAL";static char _TmpGA7[13U]="ENSURES_QUAL";static char _TmpGA8[12U]="REGION_QUAL";static char _TmpGA9[16U]="NOZEROTERM_QUAL";static char _TmpGAA[14U]="ZEROTERM_QUAL";static char _TmpGAB[12U]="TAGGED_QUAL";static char _TmpGAC[12U]="ASSERT_QUAL";static char _TmpGAD[11U]="ALIAS_QUAL";static char _TmpGAE[7U]="AQUALS";static char _TmpGAF[16U]="EXTENSIBLE_QUAL";static char _TmpGB0[18U]="AUTORELEASED_QUAL";static char _TmpGB1[7U]="PTR_OP";static char _TmpGB2[7U]="INC_OP";static char _TmpGB3[7U]="DEC_OP";static char _TmpGB4[8U]="LEFT_OP";static char _TmpGB5[9U]="RIGHT_OP";static char _TmpGB6[6U]="LE_OP";static char _TmpGB7[6U]="GE_OP";static char _TmpGB8[6U]="EQ_OP";static char _TmpGB9[6U]="NE_OP";static char _TmpGBA[7U]="AND_OP";static char _TmpGBB[6U]="OR_OP";static char _TmpGBC[11U]="MUL_ASSIGN";static char _TmpGBD[11U]="DIV_ASSIGN";static char _TmpGBE[11U]="MOD_ASSIGN";static char _TmpGBF[11U]="ADD_ASSIGN";static char _TmpGC0[11U]="SUB_ASSIGN";static char _TmpGC1[12U]="LEFT_ASSIGN";static char _TmpGC2[13U]="RIGHT_ASSIGN";static char _TmpGC3[11U]="AND_ASSIGN";static char _TmpGC4[11U]="XOR_ASSIGN";static char _TmpGC5[10U]="OR_ASSIGN";static char _TmpGC6[9U]="ELLIPSIS";static char _TmpGC7[11U]="LEFT_RIGHT";static char _TmpGC8[12U]="COLON_COLON";static char _TmpGC9[11U]="IDENTIFIER";static char _TmpGCA[17U]="INTEGER_CONSTANT";static char _TmpGCB[7U]="STRING";static char _TmpGCC[8U]="WSTRING";static char _TmpGCD[19U]="CHARACTER_CONSTANT";static char _TmpGCE[20U]="WCHARACTER_CONSTANT";static char _TmpGCF[18U]="FLOATING_CONSTANT";static char _TmpGD0[9U]="TYPE_VAR";static char _TmpGD1[13U]="TYPEDEF_NAME";static char _TmpGD2[16U]="QUAL_IDENTIFIER";static char _TmpGD3[18U]="QUAL_TYPEDEF_NAME";static char _TmpGD4[18U]="AQUAL_SHORT_CONST";static char _TmpGD5[10U]="ATTRIBUTE";static char _TmpGD6[8U]="ASM_TOK";static char _TmpGD7[4U]="';'";static char _TmpGD8[4U]="'{'";static char _TmpGD9[4U]="'}'";static char _TmpGDA[4U]="','";static char _TmpGDB[4U]="'*'";static char _TmpGDC[4U]="'='";static char _TmpGDD[4U]="'<'";static char _TmpGDE[4U]="'>'";static char _TmpGDF[4U]="'('";static char _TmpGE0[4U]="')'";static char _TmpGE1[4U]="'_'";static char _TmpGE2[4U]="'$'";static char _TmpGE3[4U]="':'";static char _TmpGE4[4U]="'.'";static char _TmpGE5[4U]="'['";static char _TmpGE6[4U]="']'";static char _TmpGE7[4U]="'@'";static char _TmpGE8[4U]="'?'";static char _TmpGE9[4U]="'+'";static char _TmpGEA[4U]="'|'";static char _TmpGEB[4U]="'^'";static char _TmpGEC[4U]="'&'";static char _TmpGED[4U]="'-'";static char _TmpGEE[4U]="'/'";static char _TmpGEF[4U]="'%'";static char _TmpGF0[4U]="'~'";static char _TmpGF1[4U]="'!'";static char _TmpGF2[5U]="prog";static char _TmpGF3[17U]="translation_unit";static char _TmpGF4[18U]="tempest_on_action";static char _TmpGF5[19U]="tempest_off_action";static char _TmpGF6[16U]="extern_c_action";static char _TmpGF7[13U]="end_extern_c";static char _TmpGF8[14U]="hide_list_opt";static char _TmpGF9[17U]="hide_list_values";static char _TmpGFA[16U]="export_list_opt";static char _TmpGFB[12U]="export_list";static char _TmpGFC[19U]="export_list_values";static char _TmpGFD[13U]="override_opt";static char _TmpGFE[21U]="external_declaration";static char _TmpGFF[14U]="optional_semi";static char _TmpG100[20U]="function_definition";static char _TmpG101[21U]="function_definition2";static char _TmpG102[13U]="using_action";static char _TmpG103[15U]="unusing_action";static char _TmpG104[17U]="namespace_action";static char _TmpG105[19U]="unnamespace_action";static char _TmpG106[12U]="declaration";static char _TmpG107[9U]="open_opt";static char _TmpG108[17U]="declaration_list";static char _TmpG109[23U]="declaration_specifiers";static char _TmpG10A[24U]="storage_class_specifier";static char _TmpG10B[15U]="attributes_opt";static char _TmpG10C[11U]="attributes";static char _TmpG10D[15U]="attribute_list";static char _TmpG10E[10U]="attribute";static char _TmpG10F[15U]="type_specifier";static char _TmpG110[25U]="type_specifier_notypedef";static char _TmpG111[5U]="kind";static char _TmpG112[15U]="type_qualifier";static char _TmpG113[15U]="enum_specifier";static char _TmpG114[11U]="enum_field";static char _TmpG115[22U]="enum_declaration_list";static char _TmpG116[26U]="struct_or_union_specifier";static char _TmpG117[26U]="maybe_tagged_struct_union";static char _TmpG118[16U]="struct_or_union";static char _TmpG119[16U]="type_params_opt";static char _TmpG11A[24U]="struct_declaration_list";static char _TmpG11B[25U]="struct_declaration_list0";static char _TmpG11C[25U]="init_declarator_list_rev";static char _TmpG11D[16U]="init_declarator";static char _TmpG11E[19U]="struct_declaration";static char _TmpG11F[25U]="specifier_qualifier_list";static char _TmpG120[35U]="notypedef_specifier_qualifier_list";static char _TmpG121[27U]="struct_declarator_list_rev";static char _TmpG122[18U]="struct_declarator";static char _TmpG123[20U]="requires_clause_opt";static char _TmpG124[19U]="datatype_specifier";static char _TmpG125[14U]="qual_datatype";static char _TmpG126[19U]="datatypefield_list";static char _TmpG127[20U]="datatypefield_scope";static char _TmpG128[14U]="datatypefield";static char _TmpG129[11U]="declarator";static char _TmpG12A[23U]="declarator_withtypedef";static char _TmpG12B[18U]="direct_declarator";static char _TmpG12C[30U]="direct_declarator_withtypedef";static char _TmpG12D[8U]="pointer";static char _TmpG12E[12U]="one_pointer";static char _TmpG12F[14U]="pointer_quals";static char _TmpG130[13U]="pointer_qual";static char _TmpG131[16U]="aqual_specifier";static char _TmpG132[23U]="pointer_null_and_bound";static char _TmpG133[14U]="pointer_bound";static char _TmpG134[18U]="zeroterm_qual_opt";static char _TmpG135[8U]="rgn_opt";static char _TmpG136[11U]="tqual_list";static char _TmpG137[20U]="parameter_type_list";static char _TmpG138[14U]="opt_aqual_bnd";static char _TmpG139[9U]="type_var";static char _TmpG13A[16U]="optional_effect";static char _TmpG13B[23U]="optional_rgnpo_qualbnd";static char _TmpG13C[14U]="rgnpo_qualbnd";static char _TmpG13D[14U]="rgn_order_elt";static char _TmpG13E[13U]="qual_bnd_elt";static char _TmpG13F[12U]="aqual_const";static char _TmpG140[15U]="qual_bnd_const";static char _TmpG141[14U]="qual_bnd_term";static char _TmpG142[16U]="optional_inject";static char _TmpG143[11U]="effect_set";static char _TmpG144[14U]="atomic_effect";static char _TmpG145[11U]="region_set";static char _TmpG146[15U]="parameter_list";static char _TmpG147[22U]="parameter_declaration";static char _TmpG148[16U]="identifier_list";static char _TmpG149[17U]="identifier_list0";static char _TmpG14A[12U]="initializer";static char _TmpG14B[18U]="array_initializer";static char _TmpG14C[17U]="initializer_list";static char _TmpG14D[12U]="designation";static char _TmpG14E[16U]="designator_list";static char _TmpG14F[11U]="designator";static char _TmpG150[10U]="type_name";static char _TmpG151[14U]="any_type_name";static char _TmpG152[15U]="type_name_list";static char _TmpG153[20U]="abstract_declarator";static char _TmpG154[27U]="direct_abstract_declarator";static char _TmpG155[25U]="requires_and_ensures_opt";static char _TmpG156[10U]="statement";static char _TmpG157[18U]="labeled_statement";static char _TmpG158[21U]="expression_statement";static char _TmpG159[19U]="compound_statement";static char _TmpG15A[16U]="block_item_list";static char _TmpG15B[20U]="selection_statement";static char _TmpG15C[15U]="switch_clauses";static char _TmpG15D[20U]="iteration_statement";static char _TmpG15E[12U]="for_exp_opt";static char _TmpG15F[15U]="jump_statement";static char _TmpG160[12U]="exp_pattern";static char _TmpG161[20U]="conditional_pattern";static char _TmpG162[19U]="logical_or_pattern";static char _TmpG163[20U]="logical_and_pattern";static char _TmpG164[21U]="inclusive_or_pattern";static char _TmpG165[21U]="exclusive_or_pattern";static char _TmpG166[12U]="and_pattern";static char _TmpG167[17U]="equality_pattern";static char _TmpG168[19U]="relational_pattern";static char _TmpG169[14U]="shift_pattern";static char _TmpG16A[17U]="additive_pattern";static char _TmpG16B[23U]="multiplicative_pattern";static char _TmpG16C[13U]="cast_pattern";static char _TmpG16D[14U]="unary_pattern";static char _TmpG16E[16U]="postfix_pattern";static char _TmpG16F[16U]="primary_pattern";static char _TmpG170[8U]="pattern";static char _TmpG171[19U]="tuple_pattern_list";static char _TmpG172[20U]="tuple_pattern_list0";static char _TmpG173[14U]="field_pattern";static char _TmpG174[19U]="field_pattern_list";static char _TmpG175[20U]="field_pattern_list0";static char _TmpG176[11U]="expression";static char _TmpG177[22U]="assignment_expression";static char _TmpG178[20U]="assignment_operator";static char _TmpG179[23U]="conditional_expression";static char _TmpG17A[20U]="constant_expression";static char _TmpG17B[22U]="logical_or_expression";static char _TmpG17C[23U]="logical_and_expression";static char _TmpG17D[24U]="inclusive_or_expression";static char _TmpG17E[24U]="exclusive_or_expression";static char _TmpG17F[15U]="and_expression";static char _TmpG180[20U]="equality_expression";static char _TmpG181[22U]="relational_expression";static char _TmpG182[17U]="shift_expression";static char _TmpG183[20U]="additive_expression";static char _TmpG184[26U]="multiplicative_expression";static char _TmpG185[12U]="equality_op";static char _TmpG186[14U]="relational_op";static char _TmpG187[12U]="additive_op";static char _TmpG188[18U]="multiplicative_op";static char _TmpG189[16U]="cast_expression";static char _TmpG18A[17U]="unary_expression";static char _TmpG18B[15U]="unary_operator";static char _TmpG18C[9U]="asm_expr";static char _TmpG18D[13U]="volatile_opt";static char _TmpG18E[12U]="asm_out_opt";static char _TmpG18F[12U]="asm_outlist";static char _TmpG190[11U]="asm_in_opt";static char _TmpG191[11U]="asm_inlist";static char _TmpG192[11U]="asm_io_elt";static char _TmpG193[16U]="asm_clobber_opt";static char _TmpG194[17U]="asm_clobber_list";static char _TmpG195[19U]="postfix_expression";static char _TmpG196[17U]="field_expression";static char _TmpG197[19U]="primary_expression";static char _TmpG198[25U]="argument_expression_list";static char _TmpG199[26U]="argument_expression_list0";static char _TmpG19A[9U]="constant";static char _TmpG19B[20U]="qual_opt_identifier";static char _TmpG19C[17U]="qual_opt_typedef";static char _TmpG19D[18U]="struct_union_name";static char _TmpG19E[11U]="field_name";static char _TmpG19F[12U]="right_angle";
# 1591 "parse.y"
static struct _fat_ptr Cyc_yytname[340U]={{_TmpG4C,_TmpG4C,_TmpG4C + 2U},{_TmpG4D,_TmpG4D,_TmpG4D + 6U},{_TmpG4E,_TmpG4E,_TmpG4E + 12U},{_TmpG4F,_TmpG4F,_TmpG4F + 5U},{_TmpG50,_TmpG50,_TmpG50 + 9U},{_TmpG51,_TmpG51,_TmpG51 + 7U},{_TmpG52,_TmpG52,_TmpG52 + 7U},{_TmpG53,_TmpG53,_TmpG53 + 8U},{_TmpG54,_TmpG54,_TmpG54 + 5U},{_TmpG55,_TmpG55,_TmpG55 + 5U},{_TmpG56,_TmpG56,_TmpG56 + 6U},{_TmpG57,_TmpG57,_TmpG57 + 4U},{_TmpG58,_TmpG58,_TmpG58 + 5U},{_TmpG59,_TmpG59,_TmpG59 + 6U},{_TmpG5A,_TmpG5A,_TmpG5A + 7U},{_TmpG5B,_TmpG5B,_TmpG5B + 7U},{_TmpG5C,_TmpG5C,_TmpG5C + 9U},{_TmpG5D,_TmpG5D,_TmpG5D + 6U},{_TmpG5E,_TmpG5E,_TmpG5E + 9U},{_TmpG5F,_TmpG5F,_TmpG5F + 9U},{_TmpG60,_TmpG60,_TmpG60 + 7U},{_TmpG61,_TmpG61,_TmpG61 + 6U},{_TmpG62,_TmpG62,_TmpG62 + 5U},{_TmpG63,_TmpG63,_TmpG63 + 8U},{_TmpG64,_TmpG64,_TmpG64 + 7U},{_TmpG65,_TmpG65,_TmpG65 + 7U},{_TmpG66,_TmpG66,_TmpG66 + 9U},{_TmpG67,_TmpG67,_TmpG67 + 3U},{_TmpG68,_TmpG68,_TmpG68 + 5U},{_TmpG69,_TmpG69,_TmpG69 + 7U},{_TmpG6A,_TmpG6A,_TmpG6A + 6U},{_TmpG6B,_TmpG6B,_TmpG6B + 3U},{_TmpG6C,_TmpG6C,_TmpG6C + 4U},{_TmpG6D,_TmpG6D,_TmpG6D + 5U},{_TmpG6E,_TmpG6E,_TmpG6E + 9U},{_TmpG6F,_TmpG6F,_TmpG6F + 6U},{_TmpG70,_TmpG70,_TmpG70 + 7U},{_TmpG71,_TmpG71,_TmpG71 + 5U},{_TmpG72,_TmpG72,_TmpG72 + 7U},{_TmpG73,_TmpG73,_TmpG73 + 16U},{_TmpG74,_TmpG74,_TmpG74 + 10U},{_TmpG75,_TmpG75,_TmpG75 + 8U},{_TmpG76,_TmpG76,_TmpG76 + 8U},{_TmpG77,_TmpG77,_TmpG77 + 4U},{_TmpG78,_TmpG78,_TmpG78 + 6U},{_TmpG79,_TmpG79,_TmpG79 + 4U},{_TmpG7A,_TmpG7A,_TmpG7A + 6U},{_TmpG7B,_TmpG7B,_TmpG7B + 7U},{_TmpG7C,_TmpG7C,_TmpG7C + 9U},{_TmpG7D,_TmpG7D,_TmpG7D + 5U},{_TmpG7E,_TmpG7E,_TmpG7E + 4U},{_TmpG7F,_TmpG7F,_TmpG7F + 5U},{_TmpG80,_TmpG80,_TmpG80 + 9U},{_TmpG81,_TmpG81,_TmpG81 + 9U},{_TmpG82,_TmpG82,_TmpG82 + 6U},{_TmpG83,_TmpG83,_TmpG83 + 10U},{_TmpG84,_TmpG84,_TmpG84 + 9U},{_TmpG85,_TmpG85,_TmpG85 + 7U},{_TmpG86,_TmpG86,_TmpG86 + 8U},{_TmpG87,_TmpG87,_TmpG87 + 9U},{_TmpG88,_TmpG88,_TmpG88 + 15U},{_TmpG89,_TmpG89,_TmpG89 + 8U},{_TmpG8A,_TmpG8A,_TmpG8A + 7U},{_TmpG8B,_TmpG8B,_TmpG8B + 8U},{_TmpG8C,_TmpG8C,_TmpG8C + 8U},{_TmpG8D,_TmpG8D,_TmpG8D + 5U},{_TmpG8E,_TmpG8E,_TmpG8E + 9U},{_TmpG8F,_TmpG8F,_TmpG8F + 6U},{_TmpG90,_TmpG90,_TmpG90 + 7U},{_TmpG91,_TmpG91,_TmpG91 + 5U},{_TmpG92,_TmpG92,_TmpG92 + 8U},{_TmpG93,_TmpG93,_TmpG93 + 7U},{_TmpG94,_TmpG94,_TmpG94 + 8U},{_TmpG95,_TmpG95,_TmpG95 + 7U},{_TmpG96,_TmpG96,_TmpG96 + 10U},{_TmpG97,_TmpG97,_TmpG97 + 11U},{_TmpG98,_TmpG98,_TmpG98 + 13U},{_TmpG99,_TmpG99,_TmpG99 + 10U},{_TmpG9A,_TmpG9A,_TmpG9A + 14U},{_TmpG9B,_TmpG9B,_TmpG9B + 10U},{_TmpG9C,_TmpG9C,_TmpG9C + 8U},{_TmpG9D,_TmpG9D,_TmpG9D + 8U},{_TmpG9E,_TmpG9E,_TmpG9E + 8U},{_TmpG9F,_TmpG9F,_TmpG9F + 10U},{_TmpGA0,_TmpGA0,_TmpGA0 + 9U},{_TmpGA1,_TmpGA1,_TmpGA1 + 13U},{_TmpGA2,_TmpGA2,_TmpGA2 + 10U},{_TmpGA3,_TmpGA3,_TmpGA3 + 9U},{_TmpGA4,_TmpGA4,_TmpGA4 + 13U},{_TmpGA5,_TmpGA5,_TmpGA5 + 14U},{_TmpGA6,_TmpGA6,_TmpGA6 + 14U},{_TmpGA7,_TmpGA7,_TmpGA7 + 13U},{_TmpGA8,_TmpGA8,_TmpGA8 + 12U},{_TmpGA9,_TmpGA9,_TmpGA9 + 16U},{_TmpGAA,_TmpGAA,_TmpGAA + 14U},{_TmpGAB,_TmpGAB,_TmpGAB + 12U},{_TmpGAC,_TmpGAC,_TmpGAC + 12U},{_TmpGAD,_TmpGAD,_TmpGAD + 11U},{_TmpGAE,_TmpGAE,_TmpGAE + 7U},{_TmpGAF,_TmpGAF,_TmpGAF + 16U},{_TmpGB0,_TmpGB0,_TmpGB0 + 18U},{_TmpGB1,_TmpGB1,_TmpGB1 + 7U},{_TmpGB2,_TmpGB2,_TmpGB2 + 7U},{_TmpGB3,_TmpGB3,_TmpGB3 + 7U},{_TmpGB4,_TmpGB4,_TmpGB4 + 8U},{_TmpGB5,_TmpGB5,_TmpGB5 + 9U},{_TmpGB6,_TmpGB6,_TmpGB6 + 6U},{_TmpGB7,_TmpGB7,_TmpGB7 + 6U},{_TmpGB8,_TmpGB8,_TmpGB8 + 6U},{_TmpGB9,_TmpGB9,_TmpGB9 + 6U},{_TmpGBA,_TmpGBA,_TmpGBA + 7U},{_TmpGBB,_TmpGBB,_TmpGBB + 6U},{_TmpGBC,_TmpGBC,_TmpGBC + 11U},{_TmpGBD,_TmpGBD,_TmpGBD + 11U},{_TmpGBE,_TmpGBE,_TmpGBE + 11U},{_TmpGBF,_TmpGBF,_TmpGBF + 11U},{_TmpGC0,_TmpGC0,_TmpGC0 + 11U},{_TmpGC1,_TmpGC1,_TmpGC1 + 12U},{_TmpGC2,_TmpGC2,_TmpGC2 + 13U},{_TmpGC3,_TmpGC3,_TmpGC3 + 11U},{_TmpGC4,_TmpGC4,_TmpGC4 + 11U},{_TmpGC5,_TmpGC5,_TmpGC5 + 10U},{_TmpGC6,_TmpGC6,_TmpGC6 + 9U},{_TmpGC7,_TmpGC7,_TmpGC7 + 11U},{_TmpGC8,_TmpGC8,_TmpGC8 + 12U},{_TmpGC9,_TmpGC9,_TmpGC9 + 11U},{_TmpGCA,_TmpGCA,_TmpGCA + 17U},{_TmpGCB,_TmpGCB,_TmpGCB + 7U},{_TmpGCC,_TmpGCC,_TmpGCC + 8U},{_TmpGCD,_TmpGCD,_TmpGCD + 19U},{_TmpGCE,_TmpGCE,_TmpGCE + 20U},{_TmpGCF,_TmpGCF,_TmpGCF + 18U},{_TmpGD0,_TmpGD0,_TmpGD0 + 9U},{_TmpGD1,_TmpGD1,_TmpGD1 + 13U},{_TmpGD2,_TmpGD2,_TmpGD2 + 16U},{_TmpGD3,_TmpGD3,_TmpGD3 + 18U},{_TmpGD4,_TmpGD4,_TmpGD4 + 18U},{_TmpGD5,_TmpGD5,_TmpGD5 + 10U},{_TmpGD6,_TmpGD6,_TmpGD6 + 8U},{_TmpGD7,_TmpGD7,_TmpGD7 + 4U},{_TmpGD8,_TmpGD8,_TmpGD8 + 4U},{_TmpGD9,_TmpGD9,_TmpGD9 + 4U},{_TmpGDA,_TmpGDA,_TmpGDA + 4U},{_TmpGDB,_TmpGDB,_TmpGDB + 4U},{_TmpGDC,_TmpGDC,_TmpGDC + 4U},{_TmpGDD,_TmpGDD,_TmpGDD + 4U},{_TmpGDE,_TmpGDE,_TmpGDE + 4U},{_TmpGDF,_TmpGDF,_TmpGDF + 4U},{_TmpGE0,_TmpGE0,_TmpGE0 + 4U},{_TmpGE1,_TmpGE1,_TmpGE1 + 4U},{_TmpGE2,_TmpGE2,_TmpGE2 + 4U},{_TmpGE3,_TmpGE3,_TmpGE3 + 4U},{_TmpGE4,_TmpGE4,_TmpGE4 + 4U},{_TmpGE5,_TmpGE5,_TmpGE5 + 4U},{_TmpGE6,_TmpGE6,_TmpGE6 + 4U},{_TmpGE7,_TmpGE7,_TmpGE7 + 4U},{_TmpGE8,_TmpGE8,_TmpGE8 + 4U},{_TmpGE9,_TmpGE9,_TmpGE9 + 4U},{_TmpGEA,_TmpGEA,_TmpGEA + 4U},{_TmpGEB,_TmpGEB,_TmpGEB + 4U},{_TmpGEC,_TmpGEC,_TmpGEC + 4U},{_TmpGED,_TmpGED,_TmpGED + 4U},{_TmpGEE,_TmpGEE,_TmpGEE + 4U},{_TmpGEF,_TmpGEF,_TmpGEF + 4U},{_TmpGF0,_TmpGF0,_TmpGF0 + 4U},{_TmpGF1,_TmpGF1,_TmpGF1 + 4U},{_TmpGF2,_TmpGF2,_TmpGF2 + 5U},{_TmpGF3,_TmpGF3,_TmpGF3 + 17U},{_TmpGF4,_TmpGF4,_TmpGF4 + 18U},{_TmpGF5,_TmpGF5,_TmpGF5 + 19U},{_TmpGF6,_TmpGF6,_TmpGF6 + 16U},{_TmpGF7,_TmpGF7,_TmpGF7 + 13U},{_TmpGF8,_TmpGF8,_TmpGF8 + 14U},{_TmpGF9,_TmpGF9,_TmpGF9 + 17U},{_TmpGFA,_TmpGFA,_TmpGFA + 16U},{_TmpGFB,_TmpGFB,_TmpGFB + 12U},{_TmpGFC,_TmpGFC,_TmpGFC + 19U},{_TmpGFD,_TmpGFD,_TmpGFD + 13U},{_TmpGFE,_TmpGFE,_TmpGFE + 21U},{_TmpGFF,_TmpGFF,_TmpGFF + 14U},{_TmpG100,_TmpG100,_TmpG100 + 20U},{_TmpG101,_TmpG101,_TmpG101 + 21U},{_TmpG102,_TmpG102,_TmpG102 + 13U},{_TmpG103,_TmpG103,_TmpG103 + 15U},{_TmpG104,_TmpG104,_TmpG104 + 17U},{_TmpG105,_TmpG105,_TmpG105 + 19U},{_TmpG106,_TmpG106,_TmpG106 + 12U},{_TmpG107,_TmpG107,_TmpG107 + 9U},{_TmpG108,_TmpG108,_TmpG108 + 17U},{_TmpG109,_TmpG109,_TmpG109 + 23U},{_TmpG10A,_TmpG10A,_TmpG10A + 24U},{_TmpG10B,_TmpG10B,_TmpG10B + 15U},{_TmpG10C,_TmpG10C,_TmpG10C + 11U},{_TmpG10D,_TmpG10D,_TmpG10D + 15U},{_TmpG10E,_TmpG10E,_TmpG10E + 10U},{_TmpG10F,_TmpG10F,_TmpG10F + 15U},{_TmpG110,_TmpG110,_TmpG110 + 25U},{_TmpG111,_TmpG111,_TmpG111 + 5U},{_TmpG112,_TmpG112,_TmpG112 + 15U},{_TmpG113,_TmpG113,_TmpG113 + 15U},{_TmpG114,_TmpG114,_TmpG114 + 11U},{_TmpG115,_TmpG115,_TmpG115 + 22U},{_TmpG116,_TmpG116,_TmpG116 + 26U},{_TmpG117,_TmpG117,_TmpG117 + 26U},{_TmpG118,_TmpG118,_TmpG118 + 16U},{_TmpG119,_TmpG119,_TmpG119 + 16U},{_TmpG11A,_TmpG11A,_TmpG11A + 24U},{_TmpG11B,_TmpG11B,_TmpG11B + 25U},{_TmpG11C,_TmpG11C,_TmpG11C + 25U},{_TmpG11D,_TmpG11D,_TmpG11D + 16U},{_TmpG11E,_TmpG11E,_TmpG11E + 19U},{_TmpG11F,_TmpG11F,_TmpG11F + 25U},{_TmpG120,_TmpG120,_TmpG120 + 35U},{_TmpG121,_TmpG121,_TmpG121 + 27U},{_TmpG122,_TmpG122,_TmpG122 + 18U},{_TmpG123,_TmpG123,_TmpG123 + 20U},{_TmpG124,_TmpG124,_TmpG124 + 19U},{_TmpG125,_TmpG125,_TmpG125 + 14U},{_TmpG126,_TmpG126,_TmpG126 + 19U},{_TmpG127,_TmpG127,_TmpG127 + 20U},{_TmpG128,_TmpG128,_TmpG128 + 14U},{_TmpG129,_TmpG129,_TmpG129 + 11U},{_TmpG12A,_TmpG12A,_TmpG12A + 23U},{_TmpG12B,_TmpG12B,_TmpG12B + 18U},{_TmpG12C,_TmpG12C,_TmpG12C + 30U},{_TmpG12D,_TmpG12D,_TmpG12D + 8U},{_TmpG12E,_TmpG12E,_TmpG12E + 12U},{_TmpG12F,_TmpG12F,_TmpG12F + 14U},{_TmpG130,_TmpG130,_TmpG130 + 13U},{_TmpG131,_TmpG131,_TmpG131 + 16U},{_TmpG132,_TmpG132,_TmpG132 + 23U},{_TmpG133,_TmpG133,_TmpG133 + 14U},{_TmpG134,_TmpG134,_TmpG134 + 18U},{_TmpG135,_TmpG135,_TmpG135 + 8U},{_TmpG136,_TmpG136,_TmpG136 + 11U},{_TmpG137,_TmpG137,_TmpG137 + 20U},{_TmpG138,_TmpG138,_TmpG138 + 14U},{_TmpG139,_TmpG139,_TmpG139 + 9U},{_TmpG13A,_TmpG13A,_TmpG13A + 16U},{_TmpG13B,_TmpG13B,_TmpG13B + 23U},{_TmpG13C,_TmpG13C,_TmpG13C + 14U},{_TmpG13D,_TmpG13D,_TmpG13D + 14U},{_TmpG13E,_TmpG13E,_TmpG13E + 13U},{_TmpG13F,_TmpG13F,_TmpG13F + 12U},{_TmpG140,_TmpG140,_TmpG140 + 15U},{_TmpG141,_TmpG141,_TmpG141 + 14U},{_TmpG142,_TmpG142,_TmpG142 + 16U},{_TmpG143,_TmpG143,_TmpG143 + 11U},{_TmpG144,_TmpG144,_TmpG144 + 14U},{_TmpG145,_TmpG145,_TmpG145 + 11U},{_TmpG146,_TmpG146,_TmpG146 + 15U},{_TmpG147,_TmpG147,_TmpG147 + 22U},{_TmpG148,_TmpG148,_TmpG148 + 16U},{_TmpG149,_TmpG149,_TmpG149 + 17U},{_TmpG14A,_TmpG14A,_TmpG14A + 12U},{_TmpG14B,_TmpG14B,_TmpG14B + 18U},{_TmpG14C,_TmpG14C,_TmpG14C + 17U},{_TmpG14D,_TmpG14D,_TmpG14D + 12U},{_TmpG14E,_TmpG14E,_TmpG14E + 16U},{_TmpG14F,_TmpG14F,_TmpG14F + 11U},{_TmpG150,_TmpG150,_TmpG150 + 10U},{_TmpG151,_TmpG151,_TmpG151 + 14U},{_TmpG152,_TmpG152,_TmpG152 + 15U},{_TmpG153,_TmpG153,_TmpG153 + 20U},{_TmpG154,_TmpG154,_TmpG154 + 27U},{_TmpG155,_TmpG155,_TmpG155 + 25U},{_TmpG156,_TmpG156,_TmpG156 + 10U},{_TmpG157,_TmpG157,_TmpG157 + 18U},{_TmpG158,_TmpG158,_TmpG158 + 21U},{_TmpG159,_TmpG159,_TmpG159 + 19U},{_TmpG15A,_TmpG15A,_TmpG15A + 16U},{_TmpG15B,_TmpG15B,_TmpG15B + 20U},{_TmpG15C,_TmpG15C,_TmpG15C + 15U},{_TmpG15D,_TmpG15D,_TmpG15D + 20U},{_TmpG15E,_TmpG15E,_TmpG15E + 12U},{_TmpG15F,_TmpG15F,_TmpG15F + 15U},{_TmpG160,_TmpG160,_TmpG160 + 12U},{_TmpG161,_TmpG161,_TmpG161 + 20U},{_TmpG162,_TmpG162,_TmpG162 + 19U},{_TmpG163,_TmpG163,_TmpG163 + 20U},{_TmpG164,_TmpG164,_TmpG164 + 21U},{_TmpG165,_TmpG165,_TmpG165 + 21U},{_TmpG166,_TmpG166,_TmpG166 + 12U},{_TmpG167,_TmpG167,_TmpG167 + 17U},{_TmpG168,_TmpG168,_TmpG168 + 19U},{_TmpG169,_TmpG169,_TmpG169 + 14U},{_TmpG16A,_TmpG16A,_TmpG16A + 17U},{_TmpG16B,_TmpG16B,_TmpG16B + 23U},{_TmpG16C,_TmpG16C,_TmpG16C + 13U},{_TmpG16D,_TmpG16D,_TmpG16D + 14U},{_TmpG16E,_TmpG16E,_TmpG16E + 16U},{_TmpG16F,_TmpG16F,_TmpG16F + 16U},{_TmpG170,_TmpG170,_TmpG170 + 8U},{_TmpG171,_TmpG171,_TmpG171 + 19U},{_TmpG172,_TmpG172,_TmpG172 + 20U},{_TmpG173,_TmpG173,_TmpG173 + 14U},{_TmpG174,_TmpG174,_TmpG174 + 19U},{_TmpG175,_TmpG175,_TmpG175 + 20U},{_TmpG176,_TmpG176,_TmpG176 + 11U},{_TmpG177,_TmpG177,_TmpG177 + 22U},{_TmpG178,_TmpG178,_TmpG178 + 20U},{_TmpG179,_TmpG179,_TmpG179 + 23U},{_TmpG17A,_TmpG17A,_TmpG17A + 20U},{_TmpG17B,_TmpG17B,_TmpG17B + 22U},{_TmpG17C,_TmpG17C,_TmpG17C + 23U},{_TmpG17D,_TmpG17D,_TmpG17D + 24U},{_TmpG17E,_TmpG17E,_TmpG17E + 24U},{_TmpG17F,_TmpG17F,_TmpG17F + 15U},{_TmpG180,_TmpG180,_TmpG180 + 20U},{_TmpG181,_TmpG181,_TmpG181 + 22U},{_TmpG182,_TmpG182,_TmpG182 + 17U},{_TmpG183,_TmpG183,_TmpG183 + 20U},{_TmpG184,_TmpG184,_TmpG184 + 26U},{_TmpG185,_TmpG185,_TmpG185 + 12U},{_TmpG186,_TmpG186,_TmpG186 + 14U},{_TmpG187,_TmpG187,_TmpG187 + 12U},{_TmpG188,_TmpG188,_TmpG188 + 18U},{_TmpG189,_TmpG189,_TmpG189 + 16U},{_TmpG18A,_TmpG18A,_TmpG18A + 17U},{_TmpG18B,_TmpG18B,_TmpG18B + 15U},{_TmpG18C,_TmpG18C,_TmpG18C + 9U},{_TmpG18D,_TmpG18D,_TmpG18D + 13U},{_TmpG18E,_TmpG18E,_TmpG18E + 12U},{_TmpG18F,_TmpG18F,_TmpG18F + 12U},{_TmpG190,_TmpG190,_TmpG190 + 11U},{_TmpG191,_TmpG191,_TmpG191 + 11U},{_TmpG192,_TmpG192,_TmpG192 + 11U},{_TmpG193,_TmpG193,_TmpG193 + 16U},{_TmpG194,_TmpG194,_TmpG194 + 17U},{_TmpG195,_TmpG195,_TmpG195 + 19U},{_TmpG196,_TmpG196,_TmpG196 + 17U},{_TmpG197,_TmpG197,_TmpG197 + 19U},{_TmpG198,_TmpG198,_TmpG198 + 25U},{_TmpG199,_TmpG199,_TmpG199 + 26U},{_TmpG19A,_TmpG19A,_TmpG19A + 9U},{_TmpG19B,_TmpG19B,_TmpG19B + 20U},{_TmpG19C,_TmpG19C,_TmpG19C + 17U},{_TmpG19D,_TmpG19D,_TmpG19D + 18U},{_TmpG19E,_TmpG19E,_TmpG19E + 11U},{_TmpG19F,_TmpG19F,_TmpG19F + 12U}};
# 1654
static short Cyc_yyr1[575U]={0,166,167,167,167,167,167,167,167,167,167,167,167,168,169,170,171,172,172,173,173,173,174,174,175,175,175,176,176,177,177,178,178,178,179,179,180,180,180,180,181,181,182,183,184,185,186,186,186,186,186,186,187,187,188,188,189,189,189,189,189,189,189,189,189,189,189,190,190,190,190,190,190,190,191,191,192,193,193,194,194,194,194,195,195,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,196,197,198,198,198,199,199,199,200,200,201,201,201,202,202,202,203,203,204,204,205,205,206,206,207,207,208,208,209,209,210,211,211,211,211,211,211,212,212,212,212,212,212,213,213,214,214,214,214,215,215,216,216,216,217,217,218,218,218,218,219,219,219,220,220,221,221,222,222,223,223,223,223,223,223,223,223,223,224,224,224,224,224,224,224,224,224,224,225,225,226,227,227,228,228,228,228,228,228,228,228,228,228,229,229,229,230,230,230,231,231,232,232,232,233,233,233,234,234,235,235,235,235,235,236,236,237,237,238,238,239,239,240,240,240,240,241,242,243,243,243,243,243,244,244,245,245,246,246,247,247,248,248,248,248,249,249,250,250,251,251,251,252,253,253,254,254,255,255,255,255,255,256,256,256,256,257,257,258,258,259,259,260,260,261,261,261,261,261,261,262,262,263,263,263,264,264,264,264,264,264,264,264,264,265,265,265,265,266,266,266,266,266,266,267,268,268,269,269,270,270,270,270,270,270,270,270,271,271,271,271,271,271,272,272,272,272,272,272,273,273,273,273,274,274,275,275,275,275,275,275,275,275,276,277,277,278,278,279,279,280,280,281,281,282,282,283,283,284,284,285,285,285,286,286,287,287,288,288,289,289,289,289,289,290,291,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,293,293,293,294,294,295,295,296,296,296,297,297,298,298,299,299,299,300,300,300,300,300,300,300,300,300,300,300,301,301,301,301,301,301,301,301,301,302,303,303,304,304,305,305,306,306,307,307,308,308,309,309,310,310,310,311,311,312,312,313,313,314,314,314,314,315,315,316,316,316,317,317,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,318,319,319,319,319,320,321,321,322,322,322,323,323,324,324,324,325,325,326,327,327,327,328,328,329,329,329,329,329,329,329,329,329,329,329,330,330,330,330,331,331,331,331,331,331,331,331,331,331,331,332,333,333,334,334,334,334,334,335,335,336,336,337,337,338,338,339,339};
# 1715
static short Cyc_yyr2[575U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,1,4,1,2,4,2,2,1,0,3,0,1,1,0,1,1,0,2,2,3,5,5,7,0,1,2,4,0,2,0,2,1,1,3,3,3,3,1,1,1,1,1,1,4,1,4,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,1,3,1,1,2,3,3,4,4,5,4,5,4,2,0,4,4,8,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,8,6,6,6,9,13,11,4,6,6,4,2,2,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1776
static short Cyc_yydefact[1142U]={0,33,67,68,69,70,72,85,86,87,88,89,90,91,92,93,111,112,113,127,128,63,0,0,98,0,94,0,73,0,0,163,105,108,0,0,0,13,14,0,0,0,0,565,233,567,566,568,0,218,0,101,0,218,217,1,0,0,0,0,31,0,0,32,0,56,65,59,83,61,95,96,0,126,99,0,0,174,0,197,200,100,178,129,71,70,64,0,115,0,58,563,0,565,560,561,562,564,0,129,0,0,395,0,0,0,271,0,399,397,42,44,0,0,52,0,0,0,0,0,125,164,0,234,235,0,0,215,0,0,0,0,216,0,0,0,2,0,0,0,0,46,0,135,137,57,66,60,62,569,570,129,131,129,0,54,0,0,35,0,237,0,186,175,198,0,204,205,209,210,0,208,207,0,206,223,200,0,84,71,119,0,117,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,550,513,0,0,0,511,0,510,508,509,0,424,426,440,450,452,454,456,458,460,462,464,467,469,482,0,484,531,548,546,565,407,0,0,0,0,408,0,0,398,49,0,0,129,0,0,247,249,250,248,0,251,0,144,140,142,291,252,298,293,0,0,0,0,0,8,9,0,0,571,572,233,110,0,0,0,179,102,269,0,266,10,11,0,3,0,5,0,47,0,0,0,35,124,0,132,133,156,0,161,0,0,0,0,0,0,0,0,0,0,0,0,565,324,326,0,334,328,0,332,317,318,319,0,320,321,322,0,55,35,137,34,36,299,0,256,272,0,0,239,237,0,220,0,0,0,0,225,74,224,201,0,120,116,0,0,0,491,0,0,506,442,482,0,443,444,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,485,486,514,505,0,488,0,0,0,0,487,0,97,0,0,0,0,0,0,471,472,0,475,476,473,474,0,0,0,477,478,0,479,480,481,0,0,430,431,432,433,434,435,436,437,438,439,429,0,489,0,537,538,0,0,0,552,0,129,400,0,0,0,421,565,572,0,0,0,0,287,417,422,0,419,0,0,396,0,273,0,0,414,415,0,412,0,0,294,0,264,145,150,146,148,141,143,237,0,301,292,302,574,573,0,104,107,0,51,0,106,109,236,80,79,0,77,219,180,237,268,176,301,270,187,188,0,103,16,29,43,0,45,0,136,138,275,274,35,37,129,122,134,0,0,0,152,159,0,129,169,0,0,0,0,0,565,0,352,0,355,356,357,0,0,359,0,0,0,335,329,137,333,327,325,38,0,185,257,0,0,0,263,238,258,313,0,228,0,239,184,222,221,181,220,0,0,0,233,0,212,226,75,130,121,449,118,114,0,0,0,0,565,276,281,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,551,558,0,557,425,451,0,453,455,457,459,461,463,465,466,468,470,428,427,536,533,0,535,0,0,0,410,411,0,290,0,418,285,288,406,0,286,409,403,48,0,404,0,0,0,295,0,151,147,149,0,0,220,0,303,0,237,0,312,297,0,0,0,0,0,144,0,129,0,237,0,196,177,267,0,22,4,6,39,239,0,155,139,156,0,0,154,162,171,170,0,0,165,0,0,0,342,0,0,0,352,0,353,354,358,0,0,0,330,323,0,40,300,237,0,260,0,0,0,0,183,240,241,242,0,0,256,229,182,202,203,0,213,211,226,199,490,0,0,0,277,0,282,0,493,0,0,0,0,0,0,0,0,547,501,504,0,0,507,515,0,0,483,555,0,0,534,532,0,0,0,0,289,420,423,405,413,416,296,253,265,313,304,305,220,0,0,220,0,0,50,565,0,76,78,0,189,0,0,0,220,0,0,0,17,23,131,153,0,157,160,172,169,169,0,0,0,0,0,0,0,0,0,352,342,360,0,331,41,239,0,261,259,0,0,0,0,0,0,239,0,0,227,543,0,542,0,278,283,0,445,446,425,425,425,0,0,0,0,447,448,536,535,520,0,556,539,0,559,441,553,554,0,401,402,309,307,311,313,306,220,53,0,81,190,195,313,194,191,220,0,0,0,0,0,0,0,168,167,336,342,0,0,0,0,0,362,363,365,367,369,371,373,375,377,379,382,384,386,388,393,394,0,0,339,348,0,352,0,0,361,231,262,0,0,243,244,0,254,246,245,230,237,214,492,0,0,284,495,496,497,0,0,0,0,503,502,0,526,520,516,518,512,540,0,310,308,0,193,192,30,25,0,0,35,0,7,123,158,0,0,0,342,0,391,0,0,342,0,0,0,0,0,0,0,0,0,0,0,0,0,389,342,0,0,352,341,314,315,0,239,545,544,0,0,0,0,0,0,527,526,523,521,0,517,541,0,26,24,0,27,0,19,173,337,338,0,0,0,0,342,344,366,0,368,370,372,374,376,378,380,381,383,385,0,343,349,0,0,0,0,232,0,0,494,0,0,0,0,529,528,0,522,519,0,28,18,20,0,340,390,0,387,345,0,342,351,0,0,255,280,279,498,0,0,525,0,524,82,21,0,364,342,346,350,0,0,0,530,392,347,316,0,500,0,499,0,0,0};
# 1894
static short Cyc_yydefgoto[174U]={1139,55,56,57,58,517,916,1054,827,828,996,698,59,339,60,323,61,519,62,521,63,270,156,64,65,589,260,503,504,261,68,278,262,70,180,181,71,72,73,178,301,302,142,143,303,263,483,533,534,709,74,75,713,714,715,76,535,77,509,78,79,175,176,587,80,127,581,355,758,673,124,81,346,575,744,745,746,264,265,962,567,571,748,477,347,287,105,106,602,524,603,456,457,458,266,340,341,674,489,743,326,327,328,329,330,331,842,332,724,333,928,929,930,931,932,933,934,935,936,937,938,939,940,941,942,943,459,472,473,460,461,462,334,218,436,219,594,220,221,222,223,224,225,226,227,228,229,409,414,419,423,230,231,232,391,392,887,982,983,1043,984,1045,1093,233,867,234,625,626,235,236,83,997,463,493};
# 1915
static short Cyc_yypact[1142U]={3112,- -32768,- -32768,- -32768,- -32768,- 24,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3924,157,- 15,- -32768,3924,- -32768,712,- -32768,162,72,- -32768,40,73,- 14,104,120,- -32768,- -32768,138,146,67,229,- -32768,21,- -32768,- -32768,- -32768,173,187,852,221,188,187,- -32768,- -32768,213,227,250,2955,- -32768,307,521,- -32768,684,3924,3924,3924,- -32768,3924,- -32768,- -32768,273,274,- -32768,162,3820,99,65,335,687,- -32768,- -32768,319,337,347,- -32768,162,413,6567,- -32768,- -32768,3329,222,- -32768,- -32768,- -32768,- -32768,343,319,397,6567,- -32768,383,3329,402,436,418,- -32768,61,- -32768,- -32768,4025,4025,425,462,2955,2955,4025,6567,- -32768,- -32768,70,- -32768,- -32768,466,6567,- -32768,776,508,70,4659,- -32768,2955,2955,3268,- -32768,2955,3268,2955,3268,- -32768,103,- -32768,3672,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,319,4659,158,1821,- -32768,3820,684,530,4025,4129,5559,- -32768,99,- -32768,535,- -32768,- -32768,- -32768,- -32768,556,- -32768,- -32768,558,- -32768,15,687,4025,- -32768,- -32768,548,576,568,162,7026,577,7137,6567,6804,580,588,592,613,615,618,635,653,661,688,703,707,709,716,721,7137,7137,- -32768,- -32768,858,7248,2631,723,- -32768,7248,- -32768,- -32768,- -32768,240,- -32768,- -32768,- 36,768,726,736,739,523,74,640,420,62,- -32768,1214,7248,153,- 34,- -32768,756,77,- -32768,3329,64,771,841,780,270,841,- -32768,- -32768,781,6567,319,1364,760,- -32768,- -32768,- -32768,- -32768,762,- -32768,1063,4659,4760,4659,511,- -32768,- -32768,- -32768,- 27,- 27,789,782,769,- -32768,- -32768,- 27,288,- -32768,- -32768,784,- -32768,53,785,774,- -32768,- -32768,732,354,- -32768,- -32768,- -32768,788,- -32768,792,- -32768,793,- -32768,776,5685,3820,530,795,797,4659,- -32768,757,162,796,794,183,798,4788,800,814,786,803,5811,2473,4788,131,802,- -32768,- -32768,808,1984,1984,684,1984,- -32768,- -32768,- -32768,815,- -32768,- -32768,- -32768,211,- -32768,530,816,- -32768,- -32768,801,37,840,- -32768,42,813,817,286,825,655,822,6567,4025,610,- -32768,843,- -32768,- -32768,37,162,- -32768,6567,837,2631,- -32768,4659,2631,- -32768,- -32768,- -32768,4914,- -32768,872,6567,6567,6567,6567,6567,6567,6567,6567,6567,6567,860,6567,4659,1115,6567,- -32768,- -32768,- -32768,- -32768,845,- -32768,1984,848,358,6567,- -32768,6567,- -32768,7248,6567,7248,7248,7248,7248,- -32768,- -32768,7248,- -32768,- -32768,- -32768,- -32768,7248,7248,7248,- -32768,- -32768,7248,- -32768,- -32768,- -32768,7248,6567,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6567,- -32768,70,- -32768,- -32768,5937,70,6567,- -32768,853,319,- -32768,854,856,857,- -32768,193,343,70,6567,3329,269,- -32768,- -32768,- -32768,863,869,861,3329,- -32768,865,- -32768,401,841,- -32768,- -32768,866,873,4025,4025,- -32768,875,876,- -32768,4760,4760,4760,- -32768,- -32768,3369,6063,427,- -32768,225,- -32768,- -32768,42,- -32768,- -32768,874,- -32768,894,- -32768,- -32768,- -32768,- -32768,877,879,880,- -32768,- -32768,1300,- -32768,228,207,- -32768,- -32768,- -32768,4659,- -32768,- -32768,975,- -32768,2955,- -32768,2955,- -32768,- -32768,- -32768,- -32768,530,- -32768,319,- -32768,- -32768,912,6567,454,- -32768,- 8,323,319,747,6567,6567,878,888,6567,889,999,2310,902,- -32768,- -32768,- -32768,505,996,- -32768,6189,2147,2789,- -32768,- -32768,3672,- -32768,- -32768,- -32768,- -32768,4025,- -32768,- -32768,4659,897,4415,- -32768,- -32768,895,677,956,- -32768,4516,817,- -32768,- -32768,- -32768,- -32768,655,903,156,906,784,908,- -32768,778,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,910,918,913,937,914,- -32768,- -32768,629,5685,447,915,922,927,943,944,945,947,948,456,949,957,959,298,961,966,954,6915,- -32768,- -32768,962,969,- -32768,768,226,726,736,739,523,74,640,420,420,62,- -32768,- -32768,- -32768,- -32768,- -32768,964,- -32768,52,4025,5430,4659,- -32768,4659,- -32768,960,- -32768,- -32768,- -32768,- -32768,2915,- -32768,- -32768,- -32768,- -32768,972,- -32768,3072,281,284,- -32768,4659,- -32768,- -32768,- -32768,967,970,655,963,225,4025,4272,6315,- -32768,- -32768,6567,983,6693,976,53,3518,977,319,4025,4129,6441,- -32768,228,- -32768,986,1080,- -32768,- -32768,- -32768,817,1036,- -32768,- -32768,757,981,6567,- -32768,- -32768,- -32768,- -32768,990,162,541,492,493,6567,763,528,4788,985,6567,995,993,- -32768,- -32768,997,1001,994,1984,- -32768,3820,- -32768,801,1005,4025,- -32768,1004,42,991,1002,- -32768,- -32768,1006,1008,1044,1007,151,- -32768,- -32768,- -32768,- -32768,4025,- -32768,- -32768,778,- -32768,1012,189,1012,1009,- -32768,5043,- -32768,6804,- -32768,6567,6567,6567,6567,1130,6567,6567,6804,- -32768,- -32768,- -32768,70,70,- -32768,1015,1019,5172,- -32768,- -32768,6567,6567,- -32768,- -32768,37,647,1031,1035,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,677,- -32768,- -32768,655,37,1024,655,1020,543,- -32768,1021,1027,- -32768,- -32768,1029,- -32768,37,1033,1034,655,1030,3268,1049,1141,- -32768,4659,- -32768,6567,- -32768,- -32768,1046,43,747,4788,1054,1051,1416,1056,1061,4788,6567,1058,6567,763,- -32768,1066,- -32768,- -32768,817,295,- -32768,- -32768,6567,6567,956,956,- 41,1076,817,4659,311,- -32768,- -32768,390,- -32768,6567,- -32768,- -32768,5685,- -32768,872,1070,1071,1073,1067,1064,1082,1083,- -32768,872,1074,1078,- 47,1079,- -32768,- -32768,664,- -32768,- -32768,- -32768,- -32768,5430,- -32768,- -32768,- -32768,- -32768,- -32768,677,- -32768,655,- -32768,1102,- -32768,- -32768,- -32768,677,- -32768,- -32768,655,1089,393,1091,2955,1093,1084,4659,- -32768,- -32768,1208,763,1097,7359,1092,2789,1087,- -32768,- 32,- -32768,1137,1090,651,523,74,711,420,62,- -32768,- -32768,- -32768,- -32768,1140,7248,1984,- -32768,- -32768,550,6567,1112,1111,- -32768,- -32768,- -32768,1105,1108,- -32768,- -32768,1110,- -32768,- -32768,- -32768,- -32768,1005,- -32768,- -32768,432,321,- -32768,- -32768,- -32768,- -32768,6567,4659,6567,1234,- -32768,- -32768,1116,- 44,342,- -32768,- -32768,- -32768,- -32768,5301,- -32768,- -32768,1122,- -32768,- -32768,- -32768,- -32768,1125,1126,562,273,- -32768,- -32768,- -32768,571,4788,1127,763,2631,- -32768,4659,1121,1621,7248,6567,7248,7248,7248,7248,7248,7248,7248,7248,7248,7248,6567,- -32768,763,1131,1123,6567,- -32768,1183,- -32768,4025,817,- -32768,- -32768,2789,1128,1132,1133,1134,6567,1151,361,- -32768,- -32768,1155,- -32768,- -32768,1157,- -32768,- -32768,273,- -32768,1144,583,- -32768,- -32768,- -32768,1145,1139,1147,7248,763,- -32768,768,381,726,736,736,523,74,640,420,420,62,- -32768,382,- -32768,- -32768,4788,1142,1146,391,- -32768,1150,691,- -32768,1148,1267,4659,578,- -32768,1152,1155,- -32768,- -32768,1154,- -32768,- -32768,- -32768,273,- -32768,1012,189,- -32768,- -32768,6567,1621,- -32768,4788,6567,- -32768,- -32768,- -32768,- -32768,1153,1156,- -32768,1168,- -32768,- -32768,- -32768,453,- -32768,763,- -32768,- -32768,1158,4659,1159,- -32768,- -32768,- -32768,- -32768,1176,- -32768,1177,- -32768,1298,1299,- -32768};
# 2033
static short Cyc_yypgoto[174U]={- -32768,134,- -32768,- -32768,- -32768,- -32768,- -32768,202,- -32768,- -32768,253,- -32768,- -32768,- 291,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 60,- -32768,- 115,60,- -32768,- -32768,0,649,- -32768,93,- 201,1212,33,- -32768,- -32768,- 136,- -32768,- -32768,1303,- 43,516,- -32768,- -32768,1052,1048,609,348,- -32768,645,- -32768,- -32768,- -32768,12,- -32768,- -32768,101,- 254,1274,- 388,44,- -32768,1178,- -32768,- -32768,- -32768,1302,- 436,- -32768,596,- 116,- 258,- 103,- 344,- 516,35,- -32768,- -32768,1010,- 547,- -32768,611,619,- 301,- 514,- 125,- 461,- 128,- -32768,- 285,- 166,- 596,- 360,- -32768,904,30,- 95,- 131,- 195,- 413,- 709,- 246,- -32768,- -32768,- 3,- 289,- -32768,- 227,- -32768,- 685,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,9,- -32768,- -32768,706,- 194,- -32768,334,836,- -32768,- 163,- 446,- 149,- 387,- 384,- 364,968,- 378,- 394,- 410,- 374,- 393,430,435,431,429,- 202,398,532,- -32768,- -32768,- -32768,- -32768,392,- -32768,- 918,333,- -32768,992,275,- -32768,- 379,- -32768,259,433,- 70,- 71,- 117,- 57};
# 2058
static short Cyc_yytable[7525U]={66,151,150,577,635,279,286,393,527,653,604,398,523,279,628,634,155,267,268,630,500,66,371,274,368,66,638,747,633,298,437,508,348,69,557,558,107,560,845,372,631,636,637,572,345,563,358,362,711,712,128,466,792,696,69,739,242,960,69,66,481,750,644,1044,545,66,66,66,488,66,501,552,356,158,677,401,66,162,490,1011,980,86,707,980,155,90,704,19,20,444,511,961,69,67,324,898,335,677,69,69,69,238,69,84,981,621,736,1042,300,69,306,114,568,246,67,696,66,66,67,491,402,445,695,164,1012,145,146,147,1096,148,492,115,89,66,66,66,157,66,66,66,66,299,490,708,66,122,751,44,695,69,69,129,67,336,66,803,66,123,67,67,67,951,67,162,354,144,69,69,69,67,69,69,69,69,44,730,1120,69,502,564,410,411,569,491,- 166,112,829,69,890,69,43,682,988,136,399,276,448,111,593,46,991,250,239,277,157,420,790,469,251,67,67,494,50,449,325,513,157,498,113,412,413,639,240,592,421,422,67,67,67,282,67,67,67,67,513,701,48,67,335,806,395,570,295,116,159,296,160,67,447,67,272,273,161,689,438,439,440,584,337,117,471,480,832,324,324,1027,324,288,289,290,553,291,292,293,294,663,566,689,554,481,481,481,43,118,565,121,108,43,604,478,344,46,593,119,482,526,46,87,66,- 237,441,591,- 237,177,753,442,443,487,43,732,305,747,747,492,276,866,66,46,239,765,125,642,277,66,66,645,66,126,755,510,540,69,43,541,324,131,954,652,240,839,45,46,47,1081,- 571,130,964,239,536,69,562,108,133,399,507,590,69,69,157,69,486,- 272,48,108,- 272,48,134,240,399,593,678,899,679,691,902,692,90,788,680,666,667,693,399,325,325,918,325,911,400,570,135,67,852,596,66,597,598,337,43,779,439,440,965,648,872,862,45,46,47,67,956,957,399,655,152,617,67,67,465,67,785,454,455,217,604,344,559,69,576,801,399,286,802,82,818,244,499,690,492,572,513,492,850,955,733,441,137,138,43,818,780,443,492,275,325,88,45,46,47,966,109,513,110,399,177,654,513,989,492,735,531,570,1036,660,179,732,992,- 15,49,871,480,480,480,82,1046,702,723,67,241,681,53,54,82,981,710,731,514,82,108,155,399,108,515,1094,108,149,623,688,153,694,108,82,1042,482,482,482,791,1084,43,66,182,66,243,399,399,109,45,46,47,487,245,703,1107,1108,994,872,995,109,967,1112,662,247,968,399,396,593,66,808,492,82,82,510,69,183,69,66,734,276,1034,66,820,82,249,809,822,277,82,82,82,269,82,82,82,82,485,536,821,417,248,69,486,418,364,468,367,369,369,970,69,399,82,922,69,705,271,766,706,948,399,478,873,1132,388,389,775,968,157,369,1072,882,871,369,67,280,67,157,182,874,690,157,952,1033,757,1071,1065,892,883,872,1075,1067,369,407,408,513,399,399,513,570,1070,67,837,838,853,868,727,1073,1074,399,67,551,1068,1069,67,699,49,700,283,1025,485,864,139,140,884,885,486,1128,53,54,593,338,399,324,109,335,800,109,843,681,109,793,835,794,351,836,109,399,253,254,255,256,359,904,399,593,593,694,1004,396,1026,478,396,338,871,352,1052,353,1053,605,585,607,608,609,361,514,367,108,614,360,512,1056,399,1063,1100,108,365,1101,1118,373,108,82,851,66,536,66,893,374,629,512,537,375,285,542,586,1024,415,416,258,536,579,580,536,900,711,712,91,570,570,1057,82,369,376,304,377,908,69,378,69,741,742,285,763,764,165,166,167,168,169,646,1059,170,171,172,379,1064,173,840,841,174,894,895,757,325,182,157,1002,16,17,18,1078,369,380,369,369,369,369,986,987,369,381,43,1015,1016,369,369,369,1019,1020,369,46,1125,1076,369,92,141,67,66,67,49,670,671,672,50,1114,399,1109,382,1106,93,94,53,54,95,96,97,150,98,46,920,921,944,383,1035,99,369,384,100,385,43,69,101,1105,102,103,386,1127,45,46,47,387,479,397,484,104,716,717,49,390,720,403,507,725,1126,43,91,404,486,324,53,54,109,45,46,47,958,959,405,446,109,1133,406,49,43,109,450,531,464,467,474,532,475,46,304,53,54,495,497,66,108,67,49,123,496,506,50,108,548,505,1055,150,516,369,53,54,518,520,528,538,1083,529,547,512,539,549,512,1124,543,66,546,593,69,324,92,82,555,82,556,561,1009,492,913,297,573,281,451,512,566,452,94,574,512,95,96,97,578,453,46,582,43,595,69,48,99,150,401,100,615,46,868,101,48,102,103,620,454,455,49,622,350,647,50,649,104,650,651,657,1038,325,53,54,67,66,658,659,661,664,665,668,812,669,684,369,683,687,697,685,718,568,686,719,722,1055,150,253,254,255,256,1060,43,1061,67,721,726,728,69,737,45,46,47,324,48,999,752,740,754,257,49,756,725,759,531,760,761,762,767,768,- 571,1085,53,54,769,325,7,8,9,10,11,12,13,14,15,16,17,18,19,20,770,771,772,44,773,774,109,258,782,285,783,569,776,109,108,22,23,24,67,26,777,369,778,66,781,786,787,789,798,795,804,285,807,805,31,1117,512,813,285,816,819,825,826,831,32,33,833,844,525,846,399,512,847,856,512,848,69,849,39,344,854,40,834,858,857,859,860,784,861,869,879,896,91,41,1135,897,43,42,905,369,285,886,888,325,45,46,47,901,369,903,906,285,907,949,49,725,909,910,531,912,285,369,583,199,914,915,53,54,919,923,44,45,950,47,924,48,67,947,969,476,953,525,946,963,974,606,975,51,52,610,611,612,613,971,972,616,973,978,619,976,977,979,985,990,369,993,998,1001,624,1000,627,1003,1005,1010,1008,43,94,206,207,95,96,97,1013,1014,46,1023,1028,1029,1030,369,369,1031,1032,82,1040,640,244,366,1041,1049,211,1050,1051,1058,1062,1079,1080,641,109,1082,1089,1087,624,1092,424,1088,1090,980,1097,725,1099,1102,1103,285,1104,1110,1113,1116,1111,1119,1131,1115,479,1140,1141,1129,285,1121,1122,1130,1098,1134,1136,7,8,9,10,11,12,13,14,15,16,17,18,19,20,676,1007,1137,1138,425,426,427,428,429,430,431,432,433,434,817,22,23,24,396,26,284,369,120,917,1066,149,522,82,530,830,163,865,357,132,31,1077,435,855,863,656,725,588,797,1017,32,33,1022,1021,1086,1018,945,632,1047,1091,1095,0,618,1123,39,0,0,40,0,0,0,0,0,0,624,0,624,0,0,41,0,0,0,42,0,0,0,0,0,0,91,0,0,369,0,369,369,369,369,369,369,369,369,369,369,0,342,0,0,43,0,0,0,0,0,149,44,45,46,47,0,48,304,344,525,925,926,49,0,0,0,507,0,51,52,0,0,486,0,53,54,0,91,0,369,0,0,0,0,0,0,0,0,0,0,0,285,0,92,0,0,0,0,0,0,0,0,0,525,149,470,0,0,237,94,0,0,95,96,97,0,98,46,0,0,0,0,0,99,369,0,100,0,369,0,101,0,102,103,0,811,0,0,0,0,815,0,0,104,0,92,0,285,824,0,0,0,0,149,0,0,0,0,0,0,237,94,0,0,95,96,97,0,98,46,0,0,0,624,0,99,0,0,100,0,0,0,927,0,102,103,0,0,0,0,0,0,212,0,0,104,214,0,0,215,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,525,0,0,0,875,876,877,878,0,880,881,0,0,0,0,0,0,0,0,0,525,0,0,891,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,840,841,21,184,185,307,0,308,309,310,311,312,313,314,315,22,23,24,316,26,91,27,187,317,0,0,0,0,188,189,28,318,0,0,31,190,191,192,193,194,195,196,197,0,32,33,34,198,0,0,0,199,0,0,0,0,0,0,39,200,201,40,202,0,0,525,0,0,0,0,0,0,0,41,203,0,0,42,0,0,204,205,0,0,0,0,0,0,525,0,0,0,0,0,0,0,0,0,0,0,0,0,0,319,94,206,207,95,96,97,44,45,46,47,0,48,208,320,154,0,0,209,0,0,0,210,0,51,322,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1037,0,1039,0,0,0,0,0,0,0,0,0,0,525,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,184,185,307,0,308,309,310,311,312,313,314,315,22,23,24,316,26,91,27,187,317,0,0,0,0,188,189,28,318,0,0,31,190,191,192,193,194,195,196,197,0,32,33,34,198,0,0,0,199,0,0,0,0,0,0,39,200,201,40,202,0,0,0,0,0,0,0,0,0,0,41,203,0,0,42,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,319,94,206,207,95,96,97,44,45,46,47,0,48,208,320,154,321,0,209,0,0,0,210,0,51,322,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,184,185,307,0,308,309,310,311,312,313,314,315,22,23,24,316,26,91,27,187,317,0,0,0,0,188,189,28,318,0,0,31,190,191,192,193,194,195,196,197,0,32,33,34,198,0,0,0,199,0,0,0,0,0,0,39,200,201,40,202,0,0,0,0,0,0,0,0,0,0,41,203,0,0,42,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,319,94,206,207,95,96,97,44,45,46,47,0,48,208,320,154,0,0,209,0,0,0,210,0,51,322,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,184,185,307,0,308,309,310,311,312,313,314,315,22,23,24,316,26,91,27,187,317,0,0,0,0,188,189,28,318,0,0,31,190,191,192,193,194,195,196,197,0,32,33,34,198,0,0,0,199,0,0,0,0,0,0,39,200,201,40,202,0,0,0,0,0,0,0,0,0,0,41,203,0,0,42,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,544,94,206,207,95,96,97,44,45,46,47,0,48,208,320,154,0,0,209,0,0,0,210,0,51,322,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,184,185,0,0,0,0,0,0,0,0,0,0,22,23,24,316,26,91,27,187,0,0,0,0,0,188,189,28,0,0,0,31,190,191,192,193,194,195,196,197,0,32,33,34,198,0,0,0,199,0,0,0,0,0,0,39,200,201,40,202,0,0,0,0,0,0,0,0,0,0,41,203,0,0,42,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,44,45,46,47,0,48,208,0,0,0,0,209,0,0,0,210,0,51,322,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,184,185,0,0,0,0,0,0,0,0,0,0,22,23,24,316,26,91,0,0,0,0,0,0,0,0,0,28,0,0,0,31,190,191,192,193,194,195,196,197,0,32,33,0,0,0,0,0,199,0,0,0,0,0,0,39,200,201,40,202,0,0,0,0,0,0,0,0,0,0,41,203,0,0,42,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,44,45,46,47,0,48,208,0,0,0,0,209,0,0,0,366,0,51,322,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,22,23,24,186,26,91,0,187,0,0,0,0,0,188,189,0,0,0,0,31,190,191,192,193,194,195,196,197,0,32,33,0,198,0,0,0,199,0,0,0,0,0,0,39,200,201,40,202,0,0,0,0,0,0,0,0,0,0,41,203,0,0,42,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,44,45,46,47,0,48,208,0,394,0,0,209,0,0,0,210,0,51,322,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,22,23,24,186,26,91,0,187,0,0,0,0,0,188,189,0,0,0,0,31,190,191,192,193,194,195,196,197,0,32,33,0,198,0,0,0,199,0,0,0,0,0,0,39,200,201,40,202,0,0,0,0,0,0,0,0,0,0,41,203,0,0,42,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,44,45,46,47,0,48,208,0,0,0,0,209,0,0,0,210,0,51,322,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216,- 12,1,91,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,92,35,36,0,37,38,0,0,0,0,39,0,796,40,0,452,94,0,0,95,96,97,0,453,46,41,0,0,0,42,99,0,0,100,0,0,0,101,0,102,103,0,454,455,0,0,0,0,0,0,104,0,0,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,0,0,- 12,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,- 12,1,91,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,92,35,36,0,37,38,0,0,0,0,39,0,799,40,0,237,94,0,0,95,96,97,0,98,46,41,0,0,0,42,99,0,0,100,0,0,0,101,0,102,103,0,0,0,0,0,0,0,0,0,104,0,0,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,0,0,0,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,91,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,22,23,24,- 12,26,49,0,0,0,50,0,51,52,0,0,0,0,53,54,31,0,0,0,0,0,0,0,0,0,32,33,0,0,92,0,0,0,0,0,0,0,0,0,39,0,0,40,0,237,94,0,0,95,96,97,0,98,46,41,0,0,0,42,99,0,0,100,0,0,0,101,0,102,103,0,0,0,0,0,0,0,0,0,104,0,342,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,344,0,0,0,49,0,0,0,485,0,51,52,0,0,486,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,0,0,0,0,49,0,0,0,531,0,51,52,0,0,0,0,53,54,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,0,154,0,0,0,297,0,0,0,0,51,52,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,44,45,0,47,0,48,0,0,154,22,23,24,25,26,0,0,0,51,52,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,0,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,252,0,0,0,0,0,253,254,255,256,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,257,42,0,0,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,44,45,0,47,258,48,0,0,259,22,23,24,0,26,0,0,0,51,52,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,342,0,0,343,0,0,0,0,0,0,44,45,0,47,0,48,0,344,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,342,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,344,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,738,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,749,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,0,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,0,26,0,0,0,0,0,0,51,52,0,0,0,184,185,307,31,308,309,310,311,312,313,314,315,0,32,33,186,0,91,0,187,317,0,0,0,0,188,189,39,318,0,40,0,190,191,192,193,194,195,196,197,0,0,41,0,198,0,42,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,44,0,0,0,0,48,0,0,0,0,0,0,0,0,0,0,0,51,52,0,0,544,94,206,207,95,96,97,0,0,46,0,0,0,208,320,154,0,0,209,0,0,0,210,0,0,211,184,185,0,0,0,0,212,599,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,600,94,206,207,95,96,97,0,277,46,0,0,0,208,0,370,601,0,209,0,0,0,210,0,0,211,0,454,455,184,185,0,212,0,0,213,214,0,0,215,216,0,0,0,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,600,94,206,207,95,96,97,0,277,46,0,0,0,208,0,370,870,0,209,0,0,0,210,0,0,211,0,454,455,184,185,0,212,0,0,213,214,0,0,215,216,0,0,0,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,600,94,206,207,95,96,97,0,277,46,0,0,0,208,0,370,889,0,209,0,0,0,210,0,0,211,0,454,455,184,185,0,212,0,0,213,214,0,0,215,216,0,0,0,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,600,94,206,207,95,96,97,0,277,46,0,0,0,208,0,370,1048,0,209,0,0,0,210,0,0,211,0,454,455,184,185,0,212,0,0,213,214,0,0,215,216,0,0,0,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,600,94,206,207,95,96,97,0,277,46,0,0,0,208,0,370,0,0,209,0,0,0,210,0,0,211,0,454,455,184,185,0,212,0,0,213,214,0,0,215,216,0,0,0,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,0,0,0,209,0,0,0,210,0,0,211,184,185,0,349,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,370,0,0,209,0,0,0,210,0,0,211,184,185,0,0,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,550,0,0,0,209,0,0,0,210,0,0,211,184,185,0,0,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,0,0,0,209,0,0,0,210,643,0,211,184,185,0,0,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,0,0,0,209,0,0,0,210,0,0,211,184,185,0,675,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,0,0,0,209,0,0,0,210,729,0,211,184,185,0,0,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,0,0,0,209,0,0,0,210,0,0,211,184,185,0,810,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,0,0,0,209,0,0,0,210,0,0,211,184,185,0,823,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,0,0,0,209,0,0,0,210,0,0,211,184,185,0,0,0,0,212,0,0,213,214,0,0,215,216,186,0,91,0,187,0,0,0,0,0,188,189,0,0,0,0,0,190,191,192,193,194,195,196,197,0,0,0,0,198,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,814,94,206,207,95,96,97,0,0,46,0,184,185,208,0,0,0,0,209,0,0,0,210,0,0,211,186,0,91,0,0,0,212,0,0,213,214,0,0,215,216,0,0,190,191,192,193,194,195,196,197,0,0,0,0,0,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,184,185,208,0,370,0,0,209,0,0,0,210,0,0,211,186,0,91,0,0,0,212,0,0,213,214,0,0,215,216,0,0,190,191,192,193,194,195,196,197,0,0,0,0,0,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,184,185,208,0,784,0,0,209,0,0,0,210,0,0,211,186,0,91,0,0,0,212,0,0,213,214,0,0,215,216,0,0,190,191,192,193,194,195,196,197,0,0,0,0,0,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,184,185,208,0,0,0,0,209,0,0,0,363,0,0,211,186,0,91,0,0,0,212,0,0,213,214,0,0,215,216,0,0,190,191,192,193,194,195,196,197,0,0,0,0,0,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,184,185,208,0,0,0,0,209,0,0,0,366,0,0,211,186,0,91,0,0,0,212,0,0,213,214,0,0,215,216,0,0,190,191,192,193,194,195,196,197,0,0,0,0,0,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,184,185,208,0,0,0,0,209,0,0,0,210,0,0,211,186,0,91,0,0,0,212,0,0,213,214,0,0,215,216,0,0,190,191,192,193,194,195,196,197,0,0,0,0,0,0,0,0,199,0,0,0,0,0,0,0,200,201,0,202,0,0,0,0,0,0,0,0,0,0,0,203,0,0,0,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,206,207,95,96,97,0,0,46,0,0,0,208,0,0,0,0,209,0,0,0,1006,0,0,211,0,0,0,0,0,0,212,0,0,213,214,0,0,215,216};
# 2814
static short Cyc_yycheck[7525U]={0,72,72,347,414,122,131,209,299,455,370,213,297,130,401,409,76,112,113,403,278,21,188,118,187,25,419,574,406,144,232,285,160,0,323,324,27,326,723,188,404,415,416,344,160,336,177,183,5,6,50,245,648,514,21,569,99,98,25,59,261,577,441,981,310,65,66,67,263,69,17,317,175,76,487,111,76,77,105,111,127,21,90,127,144,25,532,20,21,123,285,132,59,0,154,804,156,510,65,66,67,92,69,127,151,394,567,151,151,76,153,125,70,104,21,576,116,117,25,146,156,155,510,79,156,65,66,67,1046,69,157,145,147,133,134,135,76,137,138,139,140,144,105,151,144,124,582,132,536,116,117,50,59,156,154,669,156,136,65,66,67,846,69,163,149,64,133,134,135,76,137,138,139,140,132,554,1094,144,125,142,106,107,140,146,141,145,702,154,784,156,125,492,901,59,142,125,132,125,361,134,909,140,125,133,144,143,154,250,147,116,117,268,147,149,154,285,156,274,145,145,146,423,145,359,162,163,133,134,135,128,137,138,139,140,304,526,137,144,298,675,210,344,139,139,145,142,147,154,239,156,116,117,153,507,101,102,103,352,157,139,251,261,708,323,324,950,326,133,134,135,139,137,138,139,140,469,125,531,147,480,481,482,125,145,341,56,27,125,648,259,139,134,455,147,261,298,134,140,298,148,147,358,151,145,148,152,153,263,125,555,152,858,859,157,125,126,316,134,125,604,147,438,133,323,324,442,326,140,586,285,147,298,125,150,394,147,852,454,145,718,133,134,135,1028,151,124,862,125,304,316,139,92,139,142,147,355,323,324,298,326,153,139,137,104,142,137,139,145,142,532,145,807,147,145,810,147,316,151,153,474,475,153,142,323,324,831,326,823,148,492,140,298,736,363,394,365,366,296,125,101,102,103,863,446,764,749,133,134,135,316,856,857,142,144,140,385,323,324,148,326,622,152,153,89,784,139,325,394,142,148,142,556,148,0,688,101,148,507,157,740,510,157,731,148,559,147,139,140,125,703,152,153,157,119,394,22,133,134,135,148,27,531,29,142,145,456,536,903,157,564,147,574,151,464,127,721,912,140,143,764,480,481,482,50,142,528,546,394,145,489,155,156,59,151,537,555,142,64,239,559,142,242,148,142,245,72,148,507,75,509,251,78,151,480,481,482,647,1033,125,519,87,521,125,142,142,92,133,134,135,485,147,531,151,151,141,895,143,104,148,148,139,139,152,142,210,708,546,678,157,116,117,507,519,140,521,555,559,125,126,559,691,128,144,679,692,133,133,134,135,144,137,138,139,140,147,531,692,157,142,546,153,161,184,249,186,187,188,872,555,142,157,837,559,139,132,148,142,843,142,569,766,148,204,205,148,152,546,209,1018,775,895,213,519,147,521,555,183,766,688,559,847,965,589,1017,1011,788,775,987,1021,1013,232,108,109,703,142,142,706,740,1016,546,148,148,737,760,139,1019,1020,142,555,315,1014,1015,559,519,143,521,148,946,147,754,139,140,779,780,153,1111,155,156,831,139,142,731,239,733,665,242,148,677,245,649,139,651,147,142,251,142,76,77,78,79,142,148,142,856,857,695,923,363,148,669,366,139,987,147,142,147,997,373,98,375,376,377,144,142,316,456,382,141,285,148,142,1010,139,464,147,142,148,147,469,296,733,731,688,733,791,147,402,304,305,147,131,308,132,945,104,105,136,703,93,94,706,808,5,6,42,858,859,1003,325,361,147,152,147,820,731,147,733,90,91,160,141,142,85,86,87,88,89,443,1005,92,93,94,147,1010,97,22,23,100,141,142,757,731,359,733,919,17,18,19,1025,401,147,403,404,405,406,141,142,409,147,125,159,160,414,415,416,104,105,419,134,1108,1022,423,110,139,731,825,733,143,480,481,482,147,141,142,1080,147,1063,125,126,155,156,129,130,131,914,133,134,835,836,840,147,968,140,455,147,143,147,125,825,147,1062,149,150,147,1110,133,134,135,147,260,147,262,160,539,540,143,18,543,110,147,546,1108,125,42,158,153,946,155,156,456,133,134,135,858,859,159,140,464,1125,160,143,125,469,132,147,125,125,147,151,147,134,302,155,156,125,146,916,658,825,143,136,139,148,147,665,139,141,998,998,141,532,155,156,141,141,140,140,1032,141,125,507,147,139,510,1107,147,946,147,1111,916,1010,110,519,151,521,147,141,927,157,825,144,148,126,122,531,125,125,126,151,536,129,130,131,148,133,134,154,125,141,946,137,140,1052,111,143,125,134,1104,147,137,149,150,147,152,153,143,148,161,145,147,146,160,146,146,141,975,946,155,156,916,1010,142,151,148,148,142,141,683,142,125,622,147,142,48,147,147,70,148,140,30,1101,1101,76,77,78,79,1006,125,1008,946,151,139,46,1010,147,133,134,135,1108,137,916,148,157,147,98,143,148,723,148,147,142,148,125,148,142,151,1036,155,156,142,1010,8,9,10,11,12,13,14,15,16,17,18,19,20,21,142,142,142,132,142,142,658,136,127,485,141,140,148,665,840,37,38,39,1010,41,148,708,148,1108,148,148,142,148,141,154,148,507,154,148,56,1090,688,139,514,148,148,140,47,147,66,67,141,147,297,139,142,703,140,147,706,139,1108,148,80,139,141,83,714,142,147,142,107,140,146,145,25,125,42,95,1129,125,125,99,142,766,556,151,148,1108,133,134,135,148,775,154,148,567,148,844,143,846,148,148,147,154,576,788,351,73,140,49,155,156,147,140,132,133,139,135,148,137,1108,141,869,141,139,370,151,132,142,374,147,149,150,378,379,380,381,148,148,384,148,148,387,142,142,148,148,126,831,141,140,148,397,141,399,28,140,151,147,125,126,127,128,129,130,131,110,158,134,110,139,141,148,856,857,148,147,825,25,424,927,147,147,142,150,141,141,141,148,139,148,436,840,91,142,148,441,127,65,148,147,127,126,950,141,141,148,679,142,148,141,25,147,142,127,148,688,0,0,147,692,148,1101,148,1052,148,148,8,9,10,11,12,13,14,15,16,17,18,19,20,21,486,925,148,148,112,113,114,115,116,117,118,119,120,121,687,37,38,39,1006,41,130,945,41,829,1012,914,296,916,302,706,78,757,176,53,56,1023,144,740,749,457,1028,353,658,935,66,67,939,938,1036,936,840,405,982,1041,1043,- 1,386,1104,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,554,- 1,556,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,42,- 1,- 1,1011,- 1,1013,1014,1015,1016,1017,1018,1019,1020,1021,1022,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,- 1,998,132,133,134,135,- 1,137,829,139,604,25,26,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,153,- 1,155,156,- 1,42,- 1,1062,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,863,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,648,1052,122,- 1,- 1,125,126,- 1,- 1,129,130,131,- 1,133,134,- 1,- 1,- 1,- 1,- 1,140,1107,- 1,143,- 1,1111,- 1,147,- 1,149,150,- 1,680,- 1,- 1,- 1,- 1,685,- 1,- 1,160,- 1,110,- 1,919,693,- 1,- 1,- 1,- 1,1101,- 1,- 1,- 1,- 1,- 1,- 1,125,126,- 1,- 1,129,130,131,- 1,133,134,- 1,- 1,- 1,718,- 1,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,764,- 1,- 1,- 1,768,769,770,771,- 1,773,774,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,784,- 1,- 1,787,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,872,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,- 1,99,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,895,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,132,133,134,135,- 1,137,138,139,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,974,- 1,976,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,987,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,- 1,99,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,132,133,134,135,- 1,137,138,139,140,141,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,- 1,99,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,132,133,134,135,- 1,137,138,139,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,- 1,99,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,132,133,134,135,- 1,137,138,139,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,- 1,99,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,132,133,134,135,- 1,137,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,- 1,99,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,132,133,134,135,- 1,137,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,- 1,99,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,132,133,134,135,- 1,137,138,- 1,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,96,- 1,- 1,99,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,132,133,134,135,- 1,137,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,0,1,42,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,110,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,122,83,- 1,125,126,- 1,- 1,129,130,131,- 1,133,134,95,- 1,- 1,- 1,99,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,152,153,- 1,- 1,- 1,- 1,- 1,- 1,160,- 1,- 1,- 1,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,- 1,137,- 1,- 1,- 1,141,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,155,156,0,1,42,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,110,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,122,83,- 1,125,126,- 1,- 1,129,130,131,- 1,133,134,95,- 1,- 1,- 1,99,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,160,- 1,- 1,- 1,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,- 1,137,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,155,156,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,42,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,- 1,137,37,38,39,141,41,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,155,156,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,125,126,- 1,- 1,129,130,131,- 1,133,134,95,- 1,- 1,- 1,99,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,160,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,- 1,137,- 1,139,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,153,- 1,155,156,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,- 1,137,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,149,150,- 1,- 1,- 1,- 1,155,156,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,- 1,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,- 1,- 1,149,150,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,132,133,- 1,135,- 1,137,- 1,- 1,140,37,38,39,40,41,- 1,- 1,- 1,149,150,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,- 1,137,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,76,77,78,79,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,98,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,136,137,- 1,- 1,140,37,38,39,- 1,41,- 1,- 1,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,- 1,137,- 1,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,149,150,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,- 1,137,- 1,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,149,150,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,- 1,137,37,38,39,141,41,- 1,- 1,- 1,- 1,- 1,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,- 1,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,149,150,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,- 1,135,- 1,137,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,149,150,- 1,- 1,- 1,25,26,27,56,29,30,31,32,33,34,35,36,- 1,66,67,40,- 1,42,- 1,44,45,- 1,- 1,- 1,- 1,50,51,80,53,- 1,83,- 1,57,58,59,60,61,62,63,64,- 1,- 1,95,- 1,69,- 1,99,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,132,- 1,- 1,- 1,- 1,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,149,150,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,139,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,25,26,- 1,- 1,- 1,- 1,157,32,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,133,134,- 1,- 1,- 1,138,- 1,140,141,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,- 1,152,153,25,26,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,133,134,- 1,- 1,- 1,138,- 1,140,141,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,- 1,152,153,25,26,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,133,134,- 1,- 1,- 1,138,- 1,140,141,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,- 1,152,153,25,26,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,133,134,- 1,- 1,- 1,138,- 1,140,141,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,- 1,152,153,25,26,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,133,134,- 1,- 1,- 1,138,- 1,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,- 1,152,153,25,26,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,25,26,- 1,154,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,25,26,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,139,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,25,26,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,148,- 1,150,25,26,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,25,26,- 1,154,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,148,- 1,150,25,26,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,25,26,- 1,154,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,25,26,- 1,154,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,25,26,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,25,26,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,40,- 1,42,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,25,26,138,- 1,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,40,- 1,42,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,25,26,138,- 1,140,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,40,- 1,42,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,25,26,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,40,- 1,42,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,25,26,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,40,- 1,42,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,25,26,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,40,- 1,42,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,125,126,127,128,129,130,131,- 1,- 1,134,- 1,- 1,- 1,138,- 1,- 1,- 1,- 1,143,- 1,- 1,- 1,147,- 1,- 1,150,- 1,- 1,- 1,- 1,- 1,- 1,157,- 1,- 1,160,161,- 1,- 1,164,165};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
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
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _Tmp1;_Tmp1.YYINITIALSVAL.tag=75U,_Tmp1.YYINITIALSVAL.val=0;_Tmp1;});
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
struct _fat_ptr yyss1=({unsigned _Tmp1=(unsigned)yystacksize;_tag_fat(({short*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(short)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_Tmp2[i]=0);}}0;});_Tmp2;}),sizeof(short),_Tmp1);});
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1142U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 393?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,394U,sizeof(short),yychar)): 340;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 7524)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7525U,sizeof(short),yyn))!=yychar1)goto yydefault;
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
if(yyn==1141){
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefact,1142U,sizeof(short),yystate));
if(yyn==0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,575U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),14U));
if(yylen > 0)
yyval=yyyvsp[0].v;
# 370 "cycbison.simple"
switch((int)yyn){case 1:
# 1191 "parse.y"
 yyval=yyyvsp[0].v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(& yyyvsp[0].v);
# 1194
goto _LL0;case 2:
# 1197 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 3:
# 1201 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_using();
# 1204
goto _LL0;case 4:
# 1205 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 5:
# 1208
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_namespace();
# 1211
goto _LL0;case 6:
# 1212 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 7:  {
# 1214 "parse.y"
int is_c_include=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(& yyyvsp[4].v);
struct _tuple30*_Tmp1=Cyc_yyget_YY55(& yyyvsp[5].v);unsigned _Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*exs=_Tmp3;unsigned wc=_Tmp2;
struct Cyc_List_List*hides=Cyc_yyget_YY56(& yyyvsp[6].v);
if(exs!=0 && hides!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U),_tag_fat(0U,sizeof(void*),0));
if((unsigned)hides && !((int)wc))
wc=yyyvsp[6].l.first_line;
if(!is_c_include){
if(exs!=0 || cycdecls!=0){
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("expecting \"C include\"",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f0=wc,_TmpA->f1=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}else{
# 1228
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_Tmp8->tag=11,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}}else{
# 1231
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f0=wc,_TmpA->f1=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}
# 1233
goto _LL0;}}case 8:
# 1234 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Porton_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 9:
# 1236 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Portoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 10:
# 1238 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempeston_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 11:
# 1240 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 12:
# 1241 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13:
# 1245 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14:
# 1248
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15:
# 1253 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})==0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C include",sizeof(char),10U));})==0)
yyval=Cyc_YY32(1);else{
# 1259
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY32(1);}}
# 1263
goto _LL0;case 16:
# 1266 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17:
# 1270 "parse.y"
 yyval=Cyc_YY56(0);
goto _LL0;case 18:
# 1271 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 19:
# 1275 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 20:
# 1276 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 21:
# 1277 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY56(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 22:
# 1281 "parse.y"
 yyval=Cyc_YY55(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 23:
# 1282 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 24:
# 1286 "parse.y"
 yyval=Cyc_YY55(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY54(& yyyvsp[2].v);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 25:
# 1287 "parse.y"
 yyval=Cyc_YY55(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 26:
# 1288 "parse.y"
 yyval=Cyc_YY55(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f0=0,_Tmp1->f1=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 27:
# 1293 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));_Tmp3->f0=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 28:
# 1295 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));_Tmp3->f0=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY54(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 29:
# 1299 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 30:
# 1300 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 31:
# 1304 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp5=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_new_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 32:
# 1305 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 33:
# 1306 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 36:
# 1316 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 37:  {
# 1318 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 38:
# 1330 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 39:  {
# 1332 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 40:  {
# 1340 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 41:  {
# 1343 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 42:
# 1348 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(& yyyvsp[1].v));yyval=yyyvsp[1].v;
goto _LL0;case 43:
# 1351
 Cyc_Lex_leave_using();
goto _LL0;case 44:
# 1354
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp1=_Tmp2;});_Tmp1;}));yyval=yyyvsp[1].v;
goto _LL0;case 45:
# 1357
 Cyc_Lex_leave_namespace();
goto _LL0;case 46:
# 1363 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);unsigned _Tmp2=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,0,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 47:
# 1365 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);struct _tuple11*_Tmp2=({(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;})(Cyc_yyget_YY19(& yyyvsp[1].v));unsigned _Tmp3=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 48:
# 1368
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_let_decl(_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 49:  {
# 1370 "parse.y"
struct Cyc_List_List*vds=0;
{struct Cyc_List_List*ids=Cyc_yyget_YY37(& yyyvsp[1].v);for(0;ids!=0;ids=ids->tl){
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0)),({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);qv->f0=_Tmp1;}),qv->f1=(struct _fat_ptr*)ids->hd;
vds=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Vardecl*_Tmp2=({struct _tuple0*_Tmp3=qv;Cyc_Absyn_new_vardecl(0U,_Tmp3,Cyc_Absyn_wildtyp(0),0);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=vds;_Tmp1;});}}
# 1375
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(vds);Cyc_Absyn_letv_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1377
goto _LL0;}case 50:
# 1380 "parse.y"
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[2].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp1;}),tv->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[4].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=Cyc_Absyn_region_decl(tv,vd,0,yyyvsp[0].l.first_line);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1386
goto _LL0;}}case 51:  {
# 1388
struct _fat_ptr tvstring=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp2,sizeof(void*),1));});
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[1].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=tvstring;_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp1;}),tv->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[1].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Tvar*_Tmp3=tv;struct Cyc_Absyn_Vardecl*_Tmp4=vd;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_YY60(& yyyvsp[2].v);Cyc_Absyn_region_decl(_Tmp3,_Tmp4,_Tmp5,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1395
goto _LL0;}}}case 52:
# 1398 "parse.y"
 yyval=Cyc_YY60(0);
goto _LL0;case 53:
# 1400 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("open",sizeof(char),5U));})!=0)Cyc_Warn_err(yyyvsp[3].l.first_line,_tag_fat("expecting `open'",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY60(Cyc_yyget_Exp_tok(& yyyvsp[3].v));
goto _LL0;case 54:
# 1404
 yyval=yyyvsp[0].v;
goto _LL0;case 55:
# 1405 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 56:
# 1411 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 57:  {
# 1414 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
if((int)two.sc!=7)
Cyc_Warn_warn(yyyvsp[0].l.first_line,
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1421
goto _LL0;}case 58:
# 1422 "parse.y"
 Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));
yyval=yyyvsp[1].v;
# 1425
goto _LL0;case 59:
# 1426 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 60:  {
# 1429 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,({
struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=two.type_specs;Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,Cyc_yyget_YY21(& yyyvsp[0].v));});_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1434
goto _LL0;}case 61:
# 1435 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.tq=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 62:  {
# 1437 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.tq);});_Tmp1.tq=_Tmp2;}),_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1441
goto _LL0;}case 63:
# 1442 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=1,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 64:  {
# 1445 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=1,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1448
goto _LL0;}case 65:
# 1449 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;case 66:  {
# 1452 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,({
# 1455
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.attributes);});_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;}case 67:
# 1459 "parse.y"
 yyval=Cyc_YY20(4U);
goto _LL0;case 68:
# 1460 "parse.y"
 yyval=Cyc_YY20(5U);
goto _LL0;case 69:
# 1461 "parse.y"
 yyval=Cyc_YY20(3U);
goto _LL0;case 70:
# 1462 "parse.y"
 yyval=Cyc_YY20(1U);
goto _LL0;case 71:
# 1464 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY20(2U);
# 1468
goto _LL0;case 72:
# 1468 "parse.y"
 yyval=Cyc_YY20(0U);
goto _LL0;case 73:
# 1470 "parse.y"
 yyval=Cyc_YY20(6U);
goto _LL0;case 74:
# 1475 "parse.y"
 yyval=Cyc_YY46(0);
goto _LL0;case 75:
# 1476 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 76:
# 1480 "parse.y"
 yyval=yyyvsp[3].v;
goto _LL0;case 77:
# 1483
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 78:
# 1484 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 79:
# 1487
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Atts_parse_nullary_att(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 80:
# 1488 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81:
# 1490 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);unsigned _Tmp3=yyyvsp[2].l.first_line;Cyc_Atts_parse_unary_att(_Tmp1,_Tmp2,_Tmp3,Cyc_yyget_Exp_tok(& yyyvsp[2].v));}));
goto _LL0;case 82:
# 1492 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;unsigned _Tmp2=yyyvsp[2].l.first_line;struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[0].v);struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);unsigned _Tmp5=({
unsigned _Tmp6=yyyvsp[4].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[4].v));});
# 1492
Cyc_Atts_parse_format_att(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,({
# 1494
unsigned _Tmp6=yyyvsp[6].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[6].v));}));}));
goto _LL0;case 83:
# 1504 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 84:
# 1506 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_typedef_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[1].v),0,0);});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 85:
# 1510 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,yyyvsp[0].l.first_line));
goto _LL0;case 86:
# 1511 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,yyyvsp[0].l.first_line));
goto _LL0;case 87:
# 1512 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec(yyyvsp[0].l.first_line));
goto _LL0;case 88:
# 1513 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,yyyvsp[0].l.first_line));
goto _LL0;case 89:
# 1514 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec(yyyvsp[0].l.first_line));
goto _LL0;case 90:
# 1515 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,yyyvsp[0].l.first_line));
goto _LL0;case 91:
# 1516 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,yyyvsp[0].l.first_line));
goto _LL0;case 92:
# 1517 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec(yyyvsp[0].l.first_line));
goto _LL0;case 93:
# 1518 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec(yyyvsp[0].l.first_line));
goto _LL0;case 94:
# 1519 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_complex_spec(yyyvsp[0].l.first_line));
goto _LL0;case 95:
# 1520 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 96:
# 1521 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 97:
# 1523 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 98:
# 1525 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_builtin_type(_tag_fat("__builtin_va_list",sizeof(char),18U),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 99:
# 1527 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 100:
# 1529 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_yyget_YY45(& yyyvsp[0].v);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 101:
# 1531 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 102:
# 1533 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(& yyyvsp[2].v)),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 103:
# 1535 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tuple_type(({unsigned _Tmp2=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(({(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;}),_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[2].v)));}));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1537
goto _LL0;case 104:
# 1538 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 105:
# 1540 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 106:
# 1542 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_aqual_handle_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 107:
# 1544 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 108:
# 1546 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 109:
# 1548 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 110:
# 1553 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Kinds_id_to_kind(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 111:  {
# 1557 "parse.y"
unsigned loc=Cyc_Flags_porting_c_code?yyyvsp[0].l.first_line: 0U;
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=1,_Tmp1.q_volatile=0,_Tmp1.q_restrict=0,_Tmp1.real_const=1,_Tmp1.loc=loc;_Tmp1;}));
goto _LL0;}case 112:
# 1559 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=1,_Tmp1.q_restrict=0,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 113:
# 1560 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=0,_Tmp1.q_restrict=1,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 114:  {
# 1566 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl)),({void*_Tmp1=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Enumdecl*_Tmp3=({struct Cyc_Absyn_Enumdecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_Tmp4->sc=2U,({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp4->name=_Tmp5;}),({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY49(& yyyvsp[3].v);_Tmp6->v=_Tmp7;});_Tmp6;});_Tmp4->fields=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});ed->r=_Tmp1;}),ed->loc=yyyvsp[0].l.first_line;
# 1568
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=9,_Tmp2->f1=ed,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1570
goto _LL0;}case 115:
# 1571 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(& yyyvsp[1].v),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 116:
# 1573 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 117:
# 1579 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),_Tmp1->tag=0,_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 118:
# 1581 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->tag=_Tmp2;}),_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 119:
# 1585 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 120:
# 1586 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 121:
# 1587 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY49(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 122:
# 1593 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp2->tag=6,({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0,({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY25(& yyyvsp[2].v);_Tmp2->f3=_Tmp3;});_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 123:  {
# 1597 "parse.y"
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_List_List*exist_ts=({unsigned _Tmp1=yyyvsp[4].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY41(& yyyvsp[4].v));});
struct _tuple28*po_qb=Cyc_yyget_YY51(& yyyvsp[5].v);
struct _tuple28 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_TypeDecl*td=({enum Cyc_Absyn_AggrKind _Tmp4=Cyc_yyget_YY23(& yyyvsp[0].v).f1;struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp6=ts;struct Cyc_Absyn_AggrdeclImpl*_Tmp7=({
struct Cyc_List_List*_Tmp8=exist_ts;struct Cyc_List_List*_Tmp9=po;struct Cyc_List_List*_TmpA=qb;struct Cyc_List_List*_TmpB=Cyc_yyget_YY25(& yyyvsp[6].v);Cyc_Absyn_aggrdecl_impl(_Tmp8,_Tmp9,_TmpA,_TmpB,Cyc_yyget_YY23(& yyyvsp[0].v).f0);});
# 1601
Cyc_Absyn_aggr_tdecl(_Tmp4,2U,_Tmp5,_Tmp6,_Tmp7,0,yyyvsp[0].l.first_line);});
# 1604
yyval=Cyc_YY21(({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp5->tag=9,_Tmp5->f1=td,_Tmp5->f2=0;_Tmp5;});Cyc_Parse_type_spec(_Tmp4,yyyvsp[0].l.first_line);}));
# 1606
goto _LL0;}}case 124:
# 1607 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_AggrInfo _Tmp2=({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY23(& yyyvsp[0].v).f1;struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_UnknownAggr(_Tmp3,_Tmp4,Cyc_yyget_YY23(& yyyvsp[0].v).f0?({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=(void*)1;_Tmp5;}): 0);});Cyc_Absyn_aggr_type(_Tmp2,
Cyc_yyget_YY41(& yyyvsp[2].v));});
# 1607
Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1609
goto _LL0;case 125:
# 1612 "parse.y"
 yyval=Cyc_YY23(({struct _tuple24 _Tmp1;_Tmp1.f0=1,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[1].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 126:
# 1613 "parse.y"
 yyval=Cyc_YY23(({struct _tuple24 _Tmp1;_Tmp1.f0=0,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 127:
# 1616
 yyval=Cyc_YY22(0U);
goto _LL0;case 128:
# 1617 "parse.y"
 yyval=Cyc_YY22(1U);
goto _LL0;case 129:
# 1620
 yyval=Cyc_YY41(0);
goto _LL0;case 130:
# 1621 "parse.y"
 yyval=Cyc_YY41(Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 131:
# 1626 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 132:  {
# 1628 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*x=Cyc_yyget_YY26(& yyyvsp[0].v);for(0;x!=0;x=x->tl){
decls=Cyc_List_imp_append((struct Cyc_List_List*)x->hd,decls);}}{
# 1632
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags!=0)
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1637
goto _LL0;}}case 133:
# 1641 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 134:
# 1642 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY26(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 135:
# 1648 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple11));_Tmp1->tl=0,({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 136:
# 1650 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple11));({struct _tuple11*_Tmp2=Cyc_yyget_YY19(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;}),({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 137:
# 1654 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0;_Tmp1;}));
goto _LL0;case 138:
# 1655 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 139:  {
# 1660 "parse.y"
struct _RegionHandle _Tmp1=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp1;_push_region(temp);
{struct _tuple26 _Tmp2=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f0;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*x=Cyc_yyget_YY30(& yyyvsp[1].v);for(0;x!=0;x=x->tl){
struct _tuple25*_Tmp6=(struct _tuple25*)x->hd;void*_Tmp7;void*_Tmp8;struct Cyc_Parse_Declarator _Tmp9;_Tmp9=_Tmp6->f0;_Tmp8=_Tmp6->f1;_Tmp7=_Tmp6->f2;{struct Cyc_Parse_Declarator d=_Tmp9;struct Cyc_Absyn_Exp*wd=_Tmp8;struct Cyc_Absyn_Exp*wh=_Tmp7;
decls=({struct _tuple13*_TmpA=_region_malloc(temp,0U,sizeof(struct _tuple13));_TmpA->tl=decls,_TmpA->hd=d;_TmpA;});
widths_and_reqs=({struct Cyc_List_List*_TmpA=_region_malloc(temp,0U,sizeof(struct Cyc_List_List));({struct _tuple16*_TmpB=({struct _tuple16*_TmpC=_region_malloc(temp,0U,sizeof(struct _tuple16));_TmpC->f0=wd,_TmpC->f1=wh;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=widths_and_reqs;_TmpA;});}}}{
# 1670
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct Cyc_List_List*info=({struct _RegionHandle*_Tmp6=temp;struct _RegionHandle*_Tmp7=temp;struct Cyc_List_List*_Tmp8=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1671
Cyc_List_rzip(_Tmp6,_Tmp7,_Tmp8,widths_and_reqs);});
# 1673
yyval=Cyc_YY25(({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_make_aggr_field,yyyvsp[0].l.first_line,info));
# 1675
_npop_handler(0);goto _LL0;}}}}
# 1661
;_pop_region();}case 140:
# 1682 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 141:  {
# 1684 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 142:
# 1686 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 143:  {
# 1688 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f0);});_Tmp1.f0=_Tmp2;}),_Tmp1.f1=two.f1,_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 144:
# 1690 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 145:  {
# 1692 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,_Tmp1.f1=two.f1,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f2);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;}case 146:
# 1698 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 147:  {
# 1700 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 148:
# 1702 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 149:  {
# 1704 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f0);});_Tmp1.f0=_Tmp2;}),_Tmp1.f1=two.f1,_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 150:
# 1706 "parse.y"
 yyval=Cyc_YY36(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 151:  {
# 1708 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple26 _Tmp1;_Tmp1.f0=two.f0,_Tmp1.f1=two.f1,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f2);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;}case 152:
# 1714 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 153:
# 1716 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp2=Cyc_yyget_YY29(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY30(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 154:
# 1721 "parse.y"
 yyval=Cyc_YY29(({struct _tuple25*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple25));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_YY60(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 155:
# 1725 "parse.y"
 yyval=Cyc_YY29(({struct _tuple25*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple25));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f0=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->f0.id=_Tmp2;}),_Tmp1->f0.varloc=0U,_Tmp1->f0.tms=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
# 1727
goto _LL0;case 156:
# 1730 "parse.y"
 yyval=Cyc_YY29(({struct _tuple25*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple25));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f0=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->f0.id=_Tmp2;}),_Tmp1->f0.varloc=0U,_Tmp1->f0.tms=0,_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
# 1732
goto _LL0;case 157:
# 1733 "parse.y"
 yyval=Cyc_YY29(({struct _tuple25*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple25));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 158:
# 1737 "parse.y"
 yyval=Cyc_YY60(Cyc_yyget_Exp_tok(& yyyvsp[2].v));
goto _LL0;case 159:
# 1738 "parse.y"
 yyval=Cyc_YY60(0);
goto _LL0;case 160:  {
# 1744 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_Absyn_TypeDecl*dd=({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=ts;struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY35(& yyyvsp[4].v);_Tmp4->v=_Tmp5;});_Tmp4;});int _Tmp4=is_extensible;Cyc_Absyn_datatype_tdecl(2U,_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
# 1748
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=9,_Tmp2->f1=dd,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1750
goto _LL0;}case 161:  {
# 1751 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeInfo _Tmp2=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1754
goto _LL0;}case 162:  {
# 1755 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeFieldInfo _Tmp2=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.datatype_name=_Tmp4;}),({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[3].v);_Tmp3.field_name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_field_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[4].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1758
goto _LL0;}case 163:
# 1761 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 164:
# 1762 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 165:
# 1766 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 166:
# 1767 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 167:
# 1768 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 168:
# 1769 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 169:
# 1773 "parse.y"
 yyval=Cyc_YY33(2U);
goto _LL0;case 170:
# 1774 "parse.y"
 yyval=Cyc_YY33(3U);
goto _LL0;case 171:
# 1775 "parse.y"
 yyval=Cyc_YY33(0U);
goto _LL0;case 172:
# 1779 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=0,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;case 173:  {
# 1781 "parse.y"
struct Cyc_List_List*typs=({unsigned _Tmp1=yyyvsp[3].l.first_line;({(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(({(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;}),_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[3].v)));});
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=typs,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;}case 174:
# 1787 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 175:  {
# 1789 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 176:
# 1796 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 177:  {
# 1798 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 178:
# 1803 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 179:
# 1804 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 180:  {
# 1808 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1812
goto _LL0;}case 181:
# 1813 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 182:
# 1815 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 183:  {
# 1818 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_Exp*req=Cyc_yyget_YY61(& yyyvsp[4].v).f0;
struct Cyc_Absyn_Exp*ens=Cyc_yyget_YY61(& yyyvsp[4].v).f1;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp8;({struct _tuple0*_Tmp9=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp8.id=_Tmp9;}),({unsigned _Tmp9=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp8.varloc=_Tmp9;}),({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=qb,_TmpE->f7=req,_TmpE->f8=ens;_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),({struct Cyc_List_List*_TmpB=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_TmpA->tl=_TmpB;});_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 1823
goto _LL0;}}case 184:
# 1824 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 185:  {
# 1827
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1830
goto _LL0;}case 186:
# 1831 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 1831
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1834
goto _LL0;case 187:
# 1838 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 188:
# 1839 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 189:
# 1840 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 190:  {
# 1844 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1848
goto _LL0;}case 191:  {
# 1849 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 192:  {
# 1853 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1857
goto _LL0;}case 193:  {
# 1858 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_Exp*req=Cyc_yyget_YY61(& yyyvsp[4].v).f0;
struct Cyc_Absyn_Exp*ens=Cyc_yyget_YY61(& yyyvsp[4].v).f1;
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp8;_Tmp8.id=one.id,_Tmp8.varloc=one.varloc,({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=qb,_TmpE->f7=req,_TmpE->f8=ens;_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=one.tms;_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 1864
goto _LL0;}}case 194:  {
# 1865 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 195:  {
# 1869
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 196:  {
# 1873 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 197:
# 1879 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 198:
# 1880 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY27(& yyyvsp[1].v));}));
goto _LL0;case 199:  {
# 1884 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(& yyyvsp[3].v)!=0)
ans=({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp3->tag=5,_Tmp3->f1=yyyvsp[3].l.first_line,({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY46(& yyyvsp[3].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=ans;_Tmp1;});{
# 1888
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _Tmp1=*Cyc_yyget_YY1(& yyyvsp[0].v);void*_Tmp2;void*_Tmp3;unsigned _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{unsigned ploc=_Tmp4;void*nullable=_Tmp3;void*bound=_Tmp2;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));_Tmp5->ptr_loc=ploc,_Tmp5->rgn_loc=yyyvsp[2].l.first_line,_Tmp5->zt_loc=yyyvsp[1].l.first_line;_Tmp5;});{
void*mod=({struct _RegionHandle*_Tmp5=yyr;struct Cyc_Absyn_PtrLoc*_Tmp6=ptrloc;void*_Tmp7=nullable;void*_Tmp8=bound;void*_Tmp9=Cyc_yyget_YY45(& yyyvsp[2].v);struct Cyc_List_List*_TmpA=Cyc_yyget_YY59(& yyyvsp[1].v);Cyc_Parse_make_pointer_mod(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_yyget_YY24(& yyyvsp[4].v));});
ans=({struct Cyc_List_List*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));_Tmp5->hd=mod,_Tmp5->tl=ans;_Tmp5;});
yyval=Cyc_YY27(ans);
# 1896
goto _LL0;}}}}case 200:
# 1898
 yyval=Cyc_YY59(0);
goto _LL0;case 201:
# 1899 "parse.y"
 yyval=Cyc_YY59(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY58(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY59(& yyyvsp[1].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 202:
# 1904 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 203:
# 1905 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=1,({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 204:
# 1906 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=2;_Tmp1;}));
goto _LL0;case 205:
# 1907 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=3;_Tmp1;}));
goto _LL0;case 206:
# 1908 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=6;_Tmp1;}));
goto _LL0;case 207:
# 1909 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=4;_Tmp1;}));
goto _LL0;case 208:
# 1910 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=5;_Tmp1;}));
goto _LL0;case 209:
# 1911 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=7;_Tmp1;}));
goto _LL0;case 210:
# 1912 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=8;_Tmp1;}));
goto _LL0;case 211:
# 1913 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=9,({void*_Tmp2=Cyc_yyget_YY57(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 212:
# 1917 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_YY57(& yyyvsp[0].v));
goto _LL0;case 213:
# 1919 "parse.y"
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[0].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_aqk);tv->kind=_Tmp1;}),({void*_Tmp1=Cyc_yyget_YY50(& yyyvsp[1].v);tv->aquals_bound=_Tmp1;});{
void*t=Cyc_Absyn_var_type(tv);
yyval=Cyc_YY57(Cyc_Absyn_aqual_var_type(t,Cyc_Absyn_al_qual_type));
# 1924
goto _LL0;}}case 214:
# 1926 "parse.y"
 yyval=Cyc_YY57(({void*_Tmp1=Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 1928
goto _LL0;case 215:
# 1933 "parse.y"
 yyval=Cyc_YY1(({struct _tuple21*_Tmp1=_cycalloc(sizeof(struct _tuple21));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_Tmp1->f2=Cyc_Absyn_fat_bound_type:({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 216:
# 1934 "parse.y"
 yyval=Cyc_YY1(({struct _tuple21*_Tmp1=_cycalloc(sizeof(struct _tuple21));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_false_type,({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 217:
# 1935 "parse.y"
 yyval=Cyc_YY1(({struct _tuple21*_Tmp1=_cycalloc(sizeof(struct _tuple21));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_true_type,_Tmp1->f2=Cyc_Absyn_fat_bound_type;_Tmp1;}));
goto _LL0;case 218:
# 1938
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 219:
# 1939 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 220:
# 1942
 yyval=Cyc_YY53(Cyc_Tcutil_any_bool(0));
goto _LL0;case 221:
# 1943 "parse.y"
 yyval=Cyc_YY53(Cyc_Absyn_true_type);
goto _LL0;case 222:
# 1944 "parse.y"
 yyval=Cyc_YY53(Cyc_Absyn_false_type);
goto _LL0;case 223:
# 1949 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));
goto _LL0;case 224:
# 1950 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(& yyyvsp[0].v),& Cyc_Kinds_rk,1);yyval=yyyvsp[0].v;
goto _LL0;case 225:
# 1951 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));
goto _LL0;case 226:
# 1955 "parse.y"
 yyval=Cyc_YY24(Cyc_Absyn_empty_tqual((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l.first_line));
goto _LL0;case 227:
# 1956 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp1,Cyc_yyget_YY24(& yyyvsp[1].v));}));
goto _LL0;case 228:  {
# 1961 "parse.y"
struct _tuple28*po_qb=Cyc_yyget_YY51(& yyyvsp[1].v);
struct _tuple28 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));_Tmp4->f0=0,_Tmp4->f1=0,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[0].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=po,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 229:  {
# 1965 "parse.y"
struct _tuple28*po_qb=Cyc_yyget_YY51(& yyyvsp[2].v);
struct _tuple28 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=0,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[1].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=po,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 230:  {
# 1969 "parse.y"
struct _tuple28*po_qb=Cyc_yyget_YY51(& yyyvsp[4].v);
struct _tuple28 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=1,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=po,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 231:  {
# 1974
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[1].v);v->inject=_Tmp5;});{
struct _tuple28*po_qb=Cyc_yyget_YY51(& yyyvsp[4].v);
struct _tuple28 _Tmp5=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp6;_Tmp6.f0=0,_Tmp6.f1=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*po=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY40(({struct _tuple27*_Tmp8=_cycalloc(sizeof(struct _tuple27));_Tmp8->f0=0,_Tmp8->f1=0,_Tmp8->f2=v,({void*_Tmp9=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp8->f3=_Tmp9;}),_Tmp8->f4=po,_Tmp8->f5=qb;_Tmp8;}));
# 1980
goto _LL0;}}}}case 232:  {
# 1982
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[4].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[3].v);v->inject=_Tmp5;});{
struct _tuple28*po_qb=Cyc_yyget_YY51(& yyyvsp[6].v);
struct _tuple28 _Tmp5=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp6;_Tmp6.f0=0,_Tmp6.f1=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*po=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY40(({struct _tuple27*_Tmp8=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp9=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp8->f0=_Tmp9;}),_Tmp8->f1=0,_Tmp8->f2=v,({void*_Tmp9=Cyc_yyget_YY50(& yyyvsp[5].v);_Tmp8->f3=_Tmp9;}),_Tmp8->f4=po,_Tmp8->f5=qb;_Tmp8;}));
# 1988
goto _LL0;}}}}case 233:
# 1992 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 234:
# 1993 "parse.y"
 yyval=Cyc_YY50(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 235:
# 1996
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp3->tag=1,_Tmp3->f1=0;_Tmp3;});void*_Tmp3=Cyc_yyget_YY50(& yyyvsp[1].v);Cyc_Parse_id2type(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 236:
# 1997 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(& yyyvsp[2].v));void*_Tmp3=Cyc_yyget_YY50(& yyyvsp[3].v);Cyc_Parse_id2type(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 237:
# 2000
 yyval=Cyc_YY50(0);
goto _LL0;case 238:
# 2001 "parse.y"
 yyval=Cyc_YY50(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 239:
# 2005 "parse.y"
 yyval=Cyc_YY51(0);
goto _LL0;case 240:
# 2006 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 241:
# 2011 "parse.y"
 yyval=Cyc_YY51(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=({struct _tuple29*_Tmp3[1];({struct _tuple29*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(struct _tuple29*),1));});_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 242:
# 2013 "parse.y"
 yyval=Cyc_YY51(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,({struct Cyc_List_List*_Tmp2=({struct _tuple29*_Tmp3[1];({struct _tuple29*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(struct _tuple29*),1));});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 243:  {
# 2016 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(& yyyvsp[2].v);
void*_Tmp1;if(rest!=0){_Tmp1=rest->f0;{struct Cyc_List_List*rpo=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=rpo;_Tmp3;});(*rest).f0=_Tmp2;});
yyval=Cyc_YY51(rest);
# 2021
goto _LL0;}}else{_throw_match();}}case 244:  {
# 2023 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(& yyyvsp[2].v);
void*_Tmp1;if(rest!=0){_Tmp1=rest->f1;{struct Cyc_List_List*qb=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=qb;_Tmp3;});(*rest).f1=_Tmp2;});
yyval=Cyc_YY51(rest);
# 2028
goto _LL0;}}else{_throw_match();}}case 245:  {
# 2035 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct)),kb->tag=2,kb->f1=0,kb->f2=& Cyc_Kinds_rk;{
void*t=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);void*_Tmp2=(void*)kb;Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[2].l.first_line);});
yyval=Cyc_YY52(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));({void*_Tmp2=Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=t;_Tmp1;}));
# 2039
goto _LL0;}}case 246:
# 2044 "parse.y"
 yyval=Cyc_YY52(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->f0=_Tmp2;}),({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
# 2046
goto _LL0;case 247:
# 2049 "parse.y"
 yyval=Cyc_YY57(Cyc_Absyn_al_qual_type);
goto _LL0;case 248:
# 2050 "parse.y"
 yyval=Cyc_YY57(Cyc_Absyn_un_qual_type);
goto _LL0;case 249:
# 2051 "parse.y"
 yyval=Cyc_YY57(Cyc_Absyn_rc_qual_type);
goto _LL0;case 250:
# 2052 "parse.y"
 yyval=Cyc_YY57(Cyc_Absyn_rtd_qual_type);
goto _LL0;case 251:
# 2053 "parse.y"
 yyval=Cyc_YY57(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 252:
# 2058 "parse.y"
 yyval=Cyc_YY45(Cyc_yyget_YY57(& yyyvsp[0].v));
goto _LL0;case 253:
# 2061 "parse.y"
 yyval=Cyc_YY45(({void*_Tmp1=Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 2063
goto _LL0;case 254:  {
# 2068 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct)),kb->tag=0,kb->f1=& Cyc_Kinds_aqk;
yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=(void*)kb;Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[0].l.first_line);}));
# 2071
goto _LL0;}case 255:
# 2073 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v)));
# 2075
goto _LL0;case 256:
# 2102 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 257:
# 2104 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_zstrcmp(_Tmp1,_tag_fat("inject",sizeof(char),7U));})!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(yyyvsp[0].l.first_line,_tag_fat(_Tmp2,sizeof(void*),1));});
yyval=Cyc_YY32(1);
# 2108
goto _LL0;case 258:
# 2111 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 259:
# 2112 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY41(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));}));
goto _LL0;case 260:
# 2116 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 261:
# 2117 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 262:
# 2118 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 263:
# 2120 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(& yyyvsp[0].v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 2123
goto _LL0;case 264:
# 2128 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);}));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 265:
# 2130 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);}));_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 266:
# 2135 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 267:
# 2136 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY39(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 268:  {
# 2142 "parse.y"
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
# 2156
goto _LL0;}}}}}}case 269:  {
# 2157 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
if(atts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_Tmp6,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=tq,_Tmp5->f2=t;_Tmp5;}));
# 2164
goto _LL0;}}}case 270:  {
# 2165 "parse.y"
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
# 2177
goto _LL0;}}}}case 271:
# 2180 "parse.y"
 yyval=Cyc_YY37(Cyc_List_imp_rev(Cyc_yyget_YY37(& yyyvsp[0].v)));
goto _LL0;case 272:
# 2183
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 273:
# 2184 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY37(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 274:
# 2188 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 275:
# 2189 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 276:
# 2194 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 277:
# 2196 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 278:
# 2198 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 279:  {
# 2200 "parse.y"
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[2].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});void*_Tmp3=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,_Tmp3,
Cyc_Absyn_uint_exp(0U,yyyvsp[2].l.first_line));});
# 2203
vd->tq.real_const=1;
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_Tmp2->tag=26,_Tmp2->f1=vd,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2206
goto _LL0;}case 280:  {
# 2208 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_Tmp2->tag=27,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=t,_Tmp2->f3=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2211
goto _LL0;}case 281:
# 2216 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f0=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 282:
# 2218 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp3->f0=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 283:
# 2220 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f0=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 284:
# 2222 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[2].v);_Tmp3->f0=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 285:
# 2226 "parse.y"
 yyval=Cyc_YY42(Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[0].v)));
goto _LL0;case 286:
# 2227 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp3->tag=1,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 287:
# 2232 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 288:
# 2233 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 289:
# 2237 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 290:
# 2238 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp1->tag=1,({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 291:  {
# 2243 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
if(atts!=0)
Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("ignoring attributes in type",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=tq,_Tmp5->f2=t;_Tmp5;}));
# 2249
goto _LL0;}}case 292:  {
# 2250 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
struct Cyc_List_List*tms=Cyc_yyget_YY31(& yyyvsp[1].v).tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);
if(t_info.f2!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(t_info.f3!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,_tag_fat("bad specifiers, ignoring",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=t_info.f0,_Tmp5->f2=t_info.f1;_Tmp5;}));
# 2260
goto _LL0;}}case 293:
# 2263 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 294:
# 2264 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(0));
goto _LL0;case 295:
# 2265 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 296:
# 2266 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v)));
goto _LL0;case 297:
# 2267 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;})));
goto _LL0;case 298:
# 2268 "parse.y"
 yyval=Cyc_YY45(Cyc_yyget_YY45(& yyyvsp[0].v));
goto _LL0;case 299:
# 2274 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 300:
# 2275 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 301:
# 2279 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY27(& yyyvsp[0].v);_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 302:
# 2280 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 303:
# 2282 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,Cyc_yyget_YY31(& yyyvsp[1].v).tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 304:
# 2287 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 305:
# 2289 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[2].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 306:
# 2291 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 307:
# 2293 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[3].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 308:
# 2295 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2295
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2298
goto _LL0;case 309:  {
# 2299 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp8;({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=qb,({struct Cyc_Absyn_Exp*_TmpF=Cyc_yyget_YY61(& yyyvsp[3].v).f0;_TmpE->f7=_TmpF;}),({struct Cyc_Absyn_Exp*_TmpF=Cyc_yyget_YY61(& yyyvsp[3].v).f1;_TmpE->f8=_TmpF;});_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=0;_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 2302
goto _LL0;}}case 310:  {
# 2303 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp8;({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=qb,({
struct Cyc_Absyn_Exp*_TmpF=Cyc_yyget_YY61(& yyyvsp[4].v).f0;_TmpE->f7=_TmpF;}),({struct Cyc_Absyn_Exp*_TmpF=Cyc_yyget_YY61(& yyyvsp[4].v).f1;_TmpE->f8=_TmpF;});_TmpE;});
# 2304
_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),({
struct Cyc_List_List*_TmpB=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_TmpA->tl=_TmpB;});_TmpA;});
# 2304
_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 2307
goto _LL0;}}case 311:  {
# 2309
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[1].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2310
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2313
goto _LL0;}case 312:
# 2314 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2316
goto _LL0;case 313:
# 2319 "parse.y"
 yyval=Cyc_YY61(({struct _tuple16 _Tmp1;_Tmp1.f0=0,_Tmp1.f1=0;_Tmp1;}));
goto _LL0;case 314:
# 2320 "parse.y"
 yyval=Cyc_YY61(({struct _tuple16 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0;_Tmp1;}));
goto _LL0;case 315:
# 2321 "parse.y"
 yyval=Cyc_YY61(({struct _tuple16 _Tmp1;_Tmp1.f0=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 316:
# 2323 "parse.y"
 yyval=Cyc_YY61(({struct _tuple16 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 317:
# 2327 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 318:
# 2328 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 319:
# 2329 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 320:
# 2330 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 321:
# 2331 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 322:
# 2332 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 323:
# 2338 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 324:
# 2342 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 325:
# 2343 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_exp_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 326:
# 2348 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 327:
# 2349 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 328:
# 2354 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));}));
goto _LL0;case 329:
# 2355 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 330:
# 2357 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_Absyn_skip_stmt(0U));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 331:
# 2359 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_yyget_Stmt_tok(& yyyvsp[3].v));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 332:
# 2360 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 333:
# 2361 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Absyn_seq_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 334:
# 2362 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 335:
# 2365 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 336:
# 2370 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 337:
# 2372 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[6].v);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 338:
# 2378 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 339:
# 2381
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_unknownid_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 340:
# 2384
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[3].v);Cyc_Absyn_tuple_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 341:
# 2388 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);Cyc_Absyn_trycatch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 342:
# 2402 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 343:
# 2405 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2405
_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 344:
# 2408 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[2].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2408
_Tmp1->hd=_Tmp2;}),({
# 2410
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 345:
# 2412 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 346:
# 2414 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[4].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2414
_Tmp1->hd=_Tmp2;}),({
# 2416
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 347:
# 2418 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[5].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 348:
# 2425 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);Cyc_Absyn_while_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 349:
# 2427 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_do_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 350:
# 2429 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[8].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 351:  {
# 2431 "parse.y"
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[5].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[7].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp1,s);}));
goto _LL0;}case 352:
# 2435
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 353:
# 2436 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 354:
# 2439
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp2=_Tmp3;});_Tmp2;});Cyc_Absyn_goto_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 355:
# 2440 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 356:
# 2441 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 357:
# 2442 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 358:
# 2443 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_return_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 359:
# 2445 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 360:
# 2446 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 361:
# 2448 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_fallthru_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 362:
# 2457 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 363:
# 2460
 yyval=yyyvsp[0].v;
goto _LL0;case 364:
# 2462 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 365:
# 2465
 yyval=yyyvsp[0].v;
goto _LL0;case 366:
# 2467 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 367:
# 2470
 yyval=yyyvsp[0].v;
goto _LL0;case 368:
# 2472 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 369:
# 2475
 yyval=yyyvsp[0].v;
goto _LL0;case 370:
# 2477 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 371:
# 2480
 yyval=yyyvsp[0].v;
goto _LL0;case 372:
# 2482 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 373:
# 2485
 yyval=yyyvsp[0].v;
goto _LL0;case 374:
# 2487 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 375:
# 2490
 yyval=yyyvsp[0].v;
goto _LL0;case 376:
# 2492 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 377:
# 2495
 yyval=yyyvsp[0].v;
goto _LL0;case 378:
# 2497 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 379:
# 2500
 yyval=yyyvsp[0].v;
goto _LL0;case 380:
# 2502 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 381:
# 2504 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 382:
# 2507
 yyval=yyyvsp[0].v;
goto _LL0;case 383:
# 2509 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 384:
# 2512
 yyval=yyyvsp[0].v;
goto _LL0;case 385:
# 2514 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 386:
# 2517
 yyval=yyyvsp[0].v;
goto _LL0;case 387:  {
# 2519 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);})));
# 2522
goto _LL0;}case 388:
# 2525 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 389:
# 2529 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 390:  {
# 2531 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line)));
goto _LL0;}case 391:
# 2534 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);})));
goto _LL0;case 392:
# 2536 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=(*Cyc_yyget_YY38(& yyyvsp[2].v)).f2;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 393:
# 2541 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 394:
# 2549 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 395:
# 2553 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line));
goto _LL0;case 396:
# 2554 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 397:
# 2555 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_Tmp2->tag=14,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 398:
# 2556 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=5,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 399:  {
# 2558 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(& yyyvsp[0].v);
{void*_Tmp1=e->r;struct _fat_ptr _Tmp2;int _Tmp3;short _Tmp4;char _Tmp5;enum Cyc_Absyn_Sign _Tmp6;if(*((int*)_Tmp1)==0)switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.LongLong_c.tag){case 2: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f0;_Tmp5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;char i=_Tmp5;
# 2561
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_Tmp8->tag=10,_Tmp8->f1=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4C9;}case 4: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f0;_Tmp4=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;short i=_Tmp4;
# 2563
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=9,_Tmp8->f1=s,_Tmp8->f2=(int)i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4C9;}case 5: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f0;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;int i=_Tmp3;
# 2565
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=9,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4C9;}case 7: _Tmp2=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f0;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f1;{struct _fat_ptr s=_Tmp2;int i=_Tmp3;
# 2567
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_Tmp8->tag=11,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4C9;}case 1:
# 2569
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL4C9;case 8:
 goto _LL4D7;case 9: _LL4D7:
# 2572
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("strings cannot occur within patterns",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));goto _LL4C9;case 6:
# 2574
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));goto _LL4C9;default: goto _LL4DA;}else{_LL4DA:
# 2576
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("bad constant in case",sizeof(char),21U),_tag_fat(0U,sizeof(void*),0));}_LL4C9:;}
# 2579
goto _LL0;}case 400:
# 2580 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `as'",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2582
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2585
goto _LL0;case 401:
# 2586 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `alias'",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_rk;_Tmp2;});tv->kind=_Tmp1;}),tv->aquals_bound=0;{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2595
goto _LL0;}}}case 402:
# 2596 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `alias'",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_rk;_Tmp2;});tv->kind=_Tmp1;}),tv->aquals_bound=0;{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2605
goto _LL0;}}}case 403:  {
# 2606 "parse.y"
struct _tuple22 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=0,_Tmp5->f2=1,_Tmp5->f3=0,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2609
goto _LL0;}}case 404:  {
# 2610 "parse.y"
struct _tuple22 _Tmp1=*Cyc_yyget_YY10(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_Tmp5->tag=15,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=ps,_Tmp5->f3=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2613
goto _LL0;}}case 405:  {
# 2614 "parse.y"
struct _tuple22 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[3].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({unsigned _Tmp4=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp4,Cyc_yyget_YY41(& yyyvsp[2].v));});
union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,Cyc_yyget_QualId_tok(& yyyvsp[0].v),0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct)),typ->tag=0,({void*_Tmp4=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_Tmp5->tag=25,_Tmp5->f1=ai;_Tmp5;});typ->f1=_Tmp4;}),typ->f2=0;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=(void*)typ,_Tmp5->f2=0,_Tmp5->f3=exist_ts,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2620
goto _LL0;}}case 406:  {
# 2621 "parse.y"
struct _tuple22 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({unsigned _Tmp4=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp4,Cyc_yyget_YY41(& yyyvsp[1].v));});
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=0,_Tmp5->f2=0,_Tmp5->f3=exist_ts,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2625
goto _LL0;}}case 407:
# 2626 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=5,({struct Cyc_Absyn_Pat*_Tmp3=({void*_Tmp4=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp5->tag=5,({struct Cyc_Absyn_Pat*_Tmp6=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;});_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);});_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 408:
# 2628 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
# 2630
struct Cyc_Absyn_Pat*_Tmp3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[1].l.first_line);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2628
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2632
goto _LL0;case 409:
# 2633 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
Cyc_Warn_err(yyyvsp[2].l.first_line,_tag_fat("expecting `as'",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
# 2637
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[3].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2635
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2639
goto _LL0;case 410:  {
# 2640 "parse.y"
void*tag=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);void*_Tmp2=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik);Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[2].l.first_line);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,({struct Cyc_Absyn_Tvar*_Tmp3=Cyc_Parse_typ2tvar(yyyvsp[2].l.first_line,tag);_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(tag),0);});
# 2642
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2641
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2645
goto _LL0;}case 411:  {
# 2646 "parse.y"
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,_Tmp2->f1=tv,({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2648
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2647
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2651
goto _LL0;}case 412:
# 2654 "parse.y"
 yyval=Cyc_YY10(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 413:
# 2655 "parse.y"
 yyval=Cyc_YY10(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 414:
# 2656 "parse.y"
 yyval=Cyc_YY10(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f0=0,_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 415:
# 2659
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 416:
# 2660 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY11(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 417:
# 2663
 yyval=Cyc_YY12(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f0=0,({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 418:
# 2664 "parse.y"
 yyval=Cyc_YY12(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 419:
# 2667
 yyval=Cyc_YY14(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 420:
# 2668 "parse.y"
 yyval=Cyc_YY14(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 421:
# 2669 "parse.y"
 yyval=Cyc_YY14(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f0=0,_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 422:
# 2672
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple23*_Tmp2=Cyc_yyget_YY12(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 423:
# 2673 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple23*_Tmp2=Cyc_yyget_YY12(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY13(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 424:
# 2678 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 425:
# 2679 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_seq_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 426:
# 2683 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 427:
# 2685 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Core_Opt*_Tmp2=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assignop_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 428:
# 2687 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_swap_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 429:
# 2691 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 430:
# 2692 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)1U;_Tmp1;}));
goto _LL0;case 431:
# 2693 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)3U;_Tmp1;}));
goto _LL0;case 432:
# 2694 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)4U;_Tmp1;}));
goto _LL0;case 433:
# 2695 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)0U;_Tmp1;}));
goto _LL0;case 434:
# 2696 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)2U;_Tmp1;}));
goto _LL0;case 435:
# 2697 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)16U;_Tmp1;}));
goto _LL0;case 436:
# 2698 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)17U;_Tmp1;}));
goto _LL0;case 437:
# 2699 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)13U;_Tmp1;}));
goto _LL0;case 438:
# 2700 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)15U;_Tmp1;}));
goto _LL0;case 439:
# 2701 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)14U;_Tmp1;}));
goto _LL0;case 440:
# 2705 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 441:
# 2707 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 442:
# 2709 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_throw_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 443:
# 2711 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 444:
# 2712 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 445:
# 2713 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 446:
# 2714 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 447:  {
# 2715 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp4=rgn;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp6=qual;Cyc_Absyn_New_exp(_Tmp4,_Tmp5,_Tmp6,yyyvsp[0].l.first_line);}));
goto _LL0;}}case 448:  {
# 2716 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp4=rgn;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp6=qual;Cyc_Absyn_New_exp(_Tmp4,_Tmp5,_Tmp6,yyyvsp[0].l.first_line);}));
goto _LL0;}}case 449:
# 2728 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 450:
# 2731
 yyval=yyyvsp[0].v;
goto _LL0;case 451:
# 2733 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 452:
# 2736
 yyval=yyyvsp[0].v;
goto _LL0;case 453:
# 2738 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 454:
# 2741
 yyval=yyyvsp[0].v;
goto _LL0;case 455:
# 2743 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 456:
# 2746
 yyval=yyyvsp[0].v;
goto _LL0;case 457:
# 2748 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 458:
# 2751
 yyval=yyyvsp[0].v;
goto _LL0;case 459:
# 2753 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 460:
# 2756
 yyval=yyyvsp[0].v;
goto _LL0;case 461:
# 2758 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 462:
# 2761
 yyval=yyyvsp[0].v;
goto _LL0;case 463:
# 2763 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 464:
# 2766
 yyval=yyyvsp[0].v;
goto _LL0;case 465:
# 2768 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 466:
# 2770 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 467:
# 2773
 yyval=yyyvsp[0].v;
goto _LL0;case 468:
# 2775 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 469:
# 2778
 yyval=yyyvsp[0].v;
goto _LL0;case 470:
# 2780 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 471:
# 2783
 yyval=Cyc_YY68(Cyc_Absyn_eq_exp);
goto _LL0;case 472:
# 2784 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_neq_exp);
goto _LL0;case 473:
# 2787
 yyval=Cyc_YY68(Cyc_Absyn_lt_exp);
goto _LL0;case 474:
# 2788 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_gt_exp);
goto _LL0;case 475:
# 2789 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_lte_exp);
goto _LL0;case 476:
# 2790 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_gte_exp);
goto _LL0;case 477:
# 2793
 yyval=Cyc_YY6(0U);
goto _LL0;case 478:
# 2794 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 479:
# 2797
 yyval=Cyc_YY6(1U);
goto _LL0;case 480:
# 2798 "parse.y"
 yyval=Cyc_YY6(3U);
goto _LL0;case 481:
# 2799 "parse.y"
 yyval=Cyc_YY6(4U);
goto _LL0;case 482:
# 2803 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 483:  {
# 2805 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;}case 484:
# 2810 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 485:
# 2811 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;case 486:
# 2812 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,2U,yyyvsp[0].l.first_line);}));
goto _LL0;case 487:
# 2813 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_address_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 488:
# 2814 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_deref_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 489:
# 2815 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 490:  {
# 2817 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line));
# 2820
goto _LL0;}case 491:
# 2820 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 492:  {
# 2822 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
# 2825
goto _LL0;}case 493:
# 2827
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,_Tmp2->f1.rgn=0,_Tmp2->f1.aqual=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2829
goto _LL0;case 494:
# 2830 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.aqual=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2832
goto _LL0;case 495:
# 2833 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.aqual=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2835
goto _LL0;case 496:  {
# 2836 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;
yyval=Cyc_Exp_tok(({void*_Tmp4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1.mknd=2U,_Tmp5->f1.rgn=rgn,_Tmp5->f1.aqual=qual,_Tmp5->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp5->f1.num_elts=_Tmp6;}),_Tmp5->f1.fat_result=0,_Tmp5->f1.inline_call=0;_Tmp5;});Cyc_Absyn_new_exp(_Tmp4,yyyvsp[0].l.first_line);}));
# 2839
goto _LL0;}}case 497:  {
# 2840 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;
yyval=Cyc_Exp_tok(({void*_Tmp4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1.mknd=0U,_Tmp5->f1.rgn=rgn,_Tmp5->f1.aqual=qual,_Tmp5->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp5->f1.num_elts=_Tmp6;}),_Tmp5->f1.fat_result=0,_Tmp5->f1.inline_call=1;_Tmp5;});Cyc_Absyn_new_exp(_Tmp4,yyyvsp[0].l.first_line);}));
# 2843
goto _LL0;}}case 498:  {
# 2844 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,_Tmp2->f1.rgn=0,_Tmp2->f1.aqual=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2847
goto _LL0;}case 499:  {
# 2848 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[10].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[10].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.aqual=_Tmp3;}),({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2851
goto _LL0;}case 500:  {
# 2853
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[8].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[8].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.aqual=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2856
goto _LL0;}case 501:
# 2857 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2[1];({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2[0]=_Tmp3;});Cyc_List_list(_tag_fat(_Tmp2,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(18U,_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 502:
# 2859 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=37,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 503:
# 2861 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=37,({struct Cyc_Absyn_Exp*_Tmp3=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_deref_exp(_Tmp4,yyyvsp[2].l.first_line);});_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 504:  {
# 2863 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,yyyvsp[0].l.first_line));
goto _LL0;}case 505:
# 2865 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=Cyc_yyget_YY62(& yyyvsp[1].v);Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 506:
# 2866 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_extension_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 507:
# 2867 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 508:
# 2871 "parse.y"
 yyval=Cyc_YY6(12U);
goto _LL0;case 509:
# 2872 "parse.y"
 yyval=Cyc_YY6(11U);
goto _LL0;case 510:
# 2873 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 511:
# 2874 "parse.y"
 yyval=Cyc_YY6(0U);
goto _LL0;case 512:  {
# 2879 "parse.y"
struct _tuple31*_Tmp1=Cyc_yyget_YY63(& yyyvsp[3].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*outlist=_Tmp4;struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY62((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_Tmp5->tag=39,({int _Tmp6=Cyc_yyget_YY32(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=outlist,_Tmp5->f4=inlist,_Tmp5->f5=clobbers;_Tmp5;}));
goto _LL0;}}case 513:
# 2884 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 514:
# 2885 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 515:
# 2889 "parse.y"
 yyval=Cyc_YY63(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f0=0,_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 516:  {
# 2891 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY64(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY63(({struct _tuple31*_Tmp4=_cycalloc(sizeof(struct _tuple31));_Tmp4->f0=0,_Tmp4->f1=inlist,_Tmp4->f2=clobbers;_Tmp4;}));
goto _LL0;}}case 517:  {
# 2894 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY64(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY63(({struct _tuple31*_Tmp4=_cycalloc(sizeof(struct _tuple31));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY66(& yyyvsp[1].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=inlist,_Tmp4->f2=clobbers;_Tmp4;}));
goto _LL0;}}case 518:
# 2898
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 519:
# 2899 "parse.y"
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY67(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 520:
# 2903 "parse.y"
 yyval=Cyc_YY64(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 521:
# 2904 "parse.y"
 yyval=Cyc_YY64(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f0=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 522:
# 2905 "parse.y"
 yyval=Cyc_YY64(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY66(& yyyvsp[1].v));_Tmp1->f0=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 523:
# 2908
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 524:
# 2909 "parse.y"
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY67(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 525:
# 2913 "parse.y"
 yyval=Cyc_YY67(({struct _tuple32*_Tmp1=_cycalloc(sizeof(struct _tuple32));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 526:
# 2918 "parse.y"
 yyval=Cyc_YY65(0);
goto _LL0;case 527:
# 2919 "parse.y"
 yyval=Cyc_YY65(0);
goto _LL0;case 528:
# 2920 "parse.y"
 yyval=Cyc_YY65(Cyc_List_imp_rev(Cyc_yyget_YY65(& yyyvsp[1].v)));
goto _LL0;case 529:
# 2923
 yyval=Cyc_YY65(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 530:
# 2924 "parse.y"
 yyval=Cyc_YY65(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 531:
# 2929 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 532:
# 2931 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_subscript_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 533:
# 2933 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_unknowncall_exp(_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 534:
# 2935 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_unknowncall_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 535:
# 2937 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrmember_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 536:
# 2939 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrarrow_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 537:
# 2941 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,1U,yyyvsp[0].l.first_line);}));
goto _LL0;case 538:
# 2943 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,3U,yyyvsp[0].l.first_line);}));
goto _LL0;case 539:
# 2945 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 540:
# 2947 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 541:
# 2949 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 542:
# 2954 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 543:  {
# 2957
unsigned i=({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_cnst2uint(_Tmp1,Cyc_yyget_Int_tok(& yyyvsp[0].v));});
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=Cyc_Absyn_tuple_field_designator((int)i);void*_Tmp2;_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp1)->f1;{struct _fat_ptr*f=_Tmp2;
yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp5->tag=0,_Tmp5->f1=f;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;}));
goto _LL0;}}case 544:
# 2961 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 545:  {
# 2964
unsigned i=({unsigned _Tmp1=yyyvsp[2].l.first_line;Cyc_Parse_cnst2uint(_Tmp1,Cyc_yyget_Int_tok(& yyyvsp[2].v));});
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=Cyc_Absyn_tuple_field_designator((int)i);void*_Tmp2;_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp1)->f1;{struct _fat_ptr*f=_Tmp2;
yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp5->tag=0,_Tmp5->f1=f;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp3->tl=_Tmp4;});_Tmp3;}));
goto _LL0;}}case 546:
# 2971 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_unknownid_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 547:
# 2972 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Absyn_pragma_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 548:
# 2973 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 549:
# 2974 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_string_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 550:
# 2975 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wstring_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 551:
# 2976 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 552:
# 2980 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_noinstantiate_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 553:
# 2983
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[3].v));Cyc_Absyn_instantiate_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 554:
# 2986
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp2->tag=28,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[3].v));_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 555:
# 2988 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_tuple_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 556:
# 2990 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Absyn_stmt_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 557:
# 2994 "parse.y"
 yyval=Cyc_YY4(Cyc_List_imp_rev(Cyc_yyget_YY4(& yyyvsp[0].v)));
goto _LL0;case 558:
# 2999 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 559:
# 3001 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 560:
# 3007 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _Tmp1=Cyc_yyget_Int_tok(& yyyvsp[0].v);Cyc_Absyn_const_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 561:
# 3008 "parse.y"
 yyval=Cyc_Exp_tok(({char _Tmp1=Cyc_yyget_Char_tok(& yyyvsp[0].v);Cyc_Absyn_char_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 562:
# 3009 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wchar_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 563:
# 3011 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp(yyyvsp[0].l.first_line));
goto _LL0;case 564:  {
# 3013 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(& yyyvsp[0].v);
int l=(int)Cyc_strlen(f);
int i=1;
if(l > 0){
char c=((const char*)f.curr)[l - 1];
if((int)c==102 ||(int)c==70)i=0;else{
if((int)c==108 ||(int)c==76)i=2;}}
# 3021
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,yyyvsp[0].l.first_line));
# 3023
goto _LL0;}case 565:
# 3026 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f0=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 566:
# 3027 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 567:
# 3030
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f0=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 568:
# 3031 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 569:
# 3036 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 570:
# 3037 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 571:
# 3040
 yyval=yyyvsp[0].v;
goto _LL0;case 572:
# 3041 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 573:
# 3046 "parse.y"
 goto _LL0;case 574:
# 3046 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default:
# 3050
 goto _LL0;}_LL0:;
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 389 "cycbison.simple"
(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))).v=yyval;
# 392
if(yylen==0){
struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset - 1),sizeof(struct Cyc_Yystacktype),2U));
p[1].l.first_line=yylloc.first_line;
p[1].l.first_column=yylloc.first_column;
p[1].l.last_line=p[0].l.last_line;
p[1].l.last_column=p[0].l.last_column;}else{
# 399
({unsigned _Tmp1=(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l.last_line;(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l.last_line=_Tmp1;});
((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset].l.last_column=((struct Cyc_Yystacktype*)yyvs.curr)[(yyvsp_offset + yylen)- 1].l.last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _Tmp1=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,174U,sizeof(short),yyn - 166));_Tmp1 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7524)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7525U,sizeof(short),yystate))==(int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)Cyc_yydefgoto[yyn - 166];}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus==0){
# 424
++ yynerrs;
# 427
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1142U,sizeof(short),yystate));
# 429
if(yyn > -32768 && yyn < 7524){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 340U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7525U,sizeof(short),x + yyn))==x)
({unsigned long _Tmp1=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,340U,sizeof(struct _fat_ptr),x)))+ 15U;sze +=_Tmp1;}),count ++;}
msg=({unsigned _Tmp1=(unsigned)(sze + 15)+ 1U;_tag_fat(({char*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(char)));});({{unsigned _Tmp3=(unsigned)(sze + 15);unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;}),sizeof(char),_Tmp1);});
Cyc_strcpy(msg,_tag_fat("parse error",sizeof(char),12U));
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 340U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7525U,sizeof(short),x + yyn))==x){
# 451
Cyc_strcat(msg,count==0?
_tag_fat(", expecting `",sizeof(char),14U):
 _tag_fat(" or `",sizeof(char),6U));
Cyc_strcat(msg,*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,340U,sizeof(struct _fat_ptr),x)));
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1142U,sizeof(short),yystate));
if(yyn==-32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7524)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7525U,sizeof(short),yyn))!=1)goto yyerrdefault;
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
if(yyn==1141){
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 546
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_Tmp1;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3049 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
void*_Tmp0;struct _fat_ptr _Tmp1;char _Tmp2;union Cyc_Absyn_Cnst _Tmp3;switch(v.Stmt_tok.tag){case 1: _Tmp3=v.Int_tok.val;{union Cyc_Absyn_Cnst c=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 2: _Tmp2=v.Char_tok.val;{char c=_Tmp2;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)c);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%c",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 3: _Tmp1=v.String_tok.val;{struct _fat_ptr s=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("\"%s\"",sizeof(char),5U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 4: _Tmp0=v.QualId_tok.val;{struct _tuple0*q=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 5: _Tmp0=v.Exp_tok.val;{struct Cyc_Absyn_Exp*e=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 6: _Tmp0=v.Stmt_tok.val;{struct Cyc_Absyn_Stmt*s=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}default:
 Cyc_fprintf(Cyc_stderr,_tag_fat("?",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 3061
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return _tag_fat("end-of-file",sizeof(char),12U);
if(token==380)
return Cyc_Lex_token_string;
if(token==389)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 393?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,394U,sizeof(short),token)): 340;
if((unsigned)z < 340U)
return*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,340U,sizeof(struct _fat_ptr),z));
return _tag_fat(0,0,0);}}
# 3075
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _Tmp0=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _Tmp0;_push_region(yyr);
({struct _RegionHandle*_Tmp1=yyr;Cyc_yyparse(_Tmp1,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_Tmp1=Cyc_Parse_parse_result;_npop_handler(0);return _Tmp1;}
# 3078
;_pop_region();}}

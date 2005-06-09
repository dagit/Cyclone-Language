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
# 139 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 160
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 180 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 186
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U,Cyc_Absyn_AqualKind =8U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;void*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;void*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;void*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 305
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 318
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 325
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;struct Cyc_Absyn_Exp*f9;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 493 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 500
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 518
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 532
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 696 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 860
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 885
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 893
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 899
void*Cyc_Absyn_compress(void*);
# 912
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 914
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 917
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 919
extern void*Cyc_Absyn_sint_type;
# 921
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 924
extern void*Cyc_Absyn_complex_type(void*);
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_shorthand_type;extern void*Cyc_Absyn_refcnt_rgn_shorthand_type;
# 928
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 932
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 934
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_tuple_type(struct Cyc_List_List*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 953
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 965
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 969
void*Cyc_Absyn_bounds_one (void);
# 971
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 991
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 994
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 998
void*Cyc_Absyn_aqualsof_type(void*);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1055
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
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_false_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1100
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1103
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1106
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1110
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1115
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1117
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1125
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1132
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1135
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1140
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
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
static struct Cyc_List_List*Cyc_Parse_insert_aqual(struct _RegionHandle*yy,struct Cyc_List_List*qlist,void*aq,unsigned loc){
{struct Cyc_List_List*l=qlist;for(0;l!=0;l=l->tl){
void*_Tmp0=(void*)l->hd;if(*((int*)_Tmp0)==9){
# 442
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Multiple alias qualifiers",sizeof(char),26U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});
return qlist;}else{
# 445
goto _LL0;}_LL0:;}}
# 448
return({struct Cyc_List_List*_Tmp0=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=(void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp2=_region_malloc(yy,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp2->tag=9,_Tmp2->f1=aq;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=qlist;_Tmp0;});}
# 451
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of heap region",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of unique region",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of refcounted region",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of \"current\" region",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));}
# 466
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_Tmp0;if(*((int*)t)==2){_Tmp0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*pr=_Tmp0;
return pr;}}else{
({int(*_Tmp1)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp1;})(loc,_tag_fat("expecting a list of type variables, not types",sizeof(char),46U));};}
# 474
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=(void**)&((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1->kind;{void**cptr=(void**)_Tmp1;
# 477
void*_Tmp2=Cyc_Kinds_compress_kb(*cptr);if(*((int*)_Tmp2)==1){
# 479
({void*_Tmp3=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp4->tag=2,_Tmp4->f1=0,_Tmp4->f2=k;_Tmp4;}): Cyc_Kinds_kind_to_bound(k);*cptr=_Tmp3;});return;}else{
return;};}}else{
# 482
return;};}
# 487
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 495
if(tms==0)return 0;{
# 497
void*_Tmp0=(void*)tms->hd;void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp1;
# 500
if(tms->tl==0 ||
 Cyc_Parse_is_typeparam((void*)tms->tl->hd)&& _check_null(tms->tl)->tl==0){
# 503
void*_Tmp2;if(*((int*)args)==1){
# 505
Cyc_Warn_warn(loc,
_tag_fat("function declaration with both new- and old-style parameter declarations; ignoring old-style",sizeof(char),93U),_tag_fat(0U,sizeof(void*),0));
return tms;}else{_Tmp2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f1;{struct Cyc_List_List*ids=_Tmp2;
# 509
if(({int _Tmp3=Cyc_List_length(ids);_Tmp3!=Cyc_List_length(tds);}))
({int(*_Tmp3)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp3;})(loc,
_tag_fat("wrong number of parameter declarations in old-style function declaration",sizeof(char),73U));{
# 513
struct Cyc_List_List*rev_new_params=0;
for(1;ids!=0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2!=0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)tds2->hd;
void*_Tmp3=x->r;void*_Tmp4;if(*((int*)_Tmp3)==0){_Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 520
if(Cyc_zstrptrcmp((*vd->name).f1,(struct _fat_ptr*)ids->hd)!=0)
continue;
if(vd->initializer!=0)
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("initializer found in parameter declaration",sizeof(char),43U));
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("namespaces forbidden in parameter declarations",sizeof(char),47U));
rev_new_params=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp6=({struct _tuple8*_Tmp7=_cycalloc(sizeof(struct _tuple8));_Tmp7->f0=(*vd->name).f1,_Tmp7->f1=vd->tq,_Tmp7->f2=vd->type;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=rev_new_params;_Tmp5;});
# 528
goto L;}}else{
({int(*_Tmp5)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp5;})(x->loc,_tag_fat("nonvariable declaration in parameter type",sizeof(char),42U));};}
# 532
L: if(tds2==0)
({int(*_Tmp3)(unsigned,struct _fat_ptr)=({int(*_Tmp4)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp4;});unsigned _Tmp4=loc;_Tmp3(_Tmp4,Cyc_strconcat(*((struct _fat_ptr*)ids->hd),_tag_fat(" is not given a type",sizeof(char),21U)));});}
# 535
return({struct Cyc_List_List*_Tmp3=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));
({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yy,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yy,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp7->tag=1,({struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(rev_new_params);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=0,_Tmp7->f3=0,_Tmp7->f4=0,_Tmp7->f5=0,_Tmp7->f6=0,_Tmp7->f7=0,_Tmp7->f8=0,_Tmp7->f9=0;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});}}};}
# 542
goto _LL4;}}else{_LL4:
 return({struct Cyc_List_List*_Tmp2=_region_malloc(yy,0U,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp3=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_Tmp2->tl=_Tmp3;});_Tmp2;});};}}
# 550
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 554
if(tds!=0)
d=({struct Cyc_Parse_Declarator _Tmp0;_Tmp0.id=d.id,_Tmp0.varloc=d.varloc,({struct Cyc_List_List*_Tmp1=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_Tmp0.tms=_Tmp1;});_Tmp0;});{
# 557
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 563
if(dso!=0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 569
enum Cyc_Parse_Storage_class _Tmp0=dso->sc;switch((int)_Tmp0){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
 Cyc_Warn_err(loc,_tag_fat("bad storage class on function",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}}{
# 576
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f0;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_Absyn_Tqual fn_tqual=_Tmp4;void*fn_type=_Tmp3;struct Cyc_List_List*x=_Tmp2;struct Cyc_List_List*out_atts=_Tmp1;
# 581
if(x!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));{
# 584
struct Cyc_Absyn_FnInfo _Tmp5;if(*((int*)fn_type)==5){_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fn_type)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp5;
# 586
{struct Cyc_List_List*args2=i.args;for(0;args2!=0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f0==0){
Cyc_Warn_err(loc,_tag_fat("missing argument variable in function prototype",sizeof(char),48U),_tag_fat(0U,sizeof(void*),0));
({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=_tag_fat("?",sizeof(char),2U);_Tmp7;});(*((struct _tuple8*)args2->hd)).f0=_Tmp6;});}}}
# 593
({struct Cyc_List_List*_Tmp6=Cyc_List_append(i.attributes,out_atts);i.attributes=_Tmp6;});
return({struct Cyc_Absyn_Fndecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_Tmp6->sc=sc,_Tmp6->is_inline=is_inline,_Tmp6->name=d.id,_Tmp6->body=body,_Tmp6->i=i,_Tmp6->cached_type=0,_Tmp6->param_vardecls=0,_Tmp6->fn_vardecl=0,_Tmp6->orig_scope=sc;_Tmp6;});}}else{
# 597
({int(*_Tmp6)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp6;})(loc,_tag_fat("declarator is not a function prototype",sizeof(char),39U));};}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 601
static struct _fat_ptr Cyc_Parse_msg1={_TmpG0,_TmpG0,_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 603
static struct _fat_ptr Cyc_Parse_msg2={_TmpG1,_TmpG1,_TmpG1 + 84U};
# 610
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 613
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
# 638
return s1;}
# 644
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 652
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
# 665
if(!seen_type){
if(!seen_sign && !seen_size){
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_double_type);
Cyc_Warn_warn(loc,_tag_fat("missing type within specifier",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));}
# 671
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_int_type(sgn,sz));else{
return Cyc_Absyn_int_type(sgn,sz);}}
# 675
{enum Cyc_Absyn_Size_of _Tmp0;enum Cyc_Absyn_Sign _Tmp1;if(*((int*)t)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 1: _Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_Tmp1;enum Cyc_Absyn_Size_of sz2=_Tmp0;
# 677
if(seen_sign &&(int)sgn2!=(int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 681
if(seen_size &&(int)sz2!=(int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;}case 2:
# 687
 if(seen_size)
t=Cyc_Absyn_long_double_type;
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;default: goto _LL5;}else{_LL5:
# 693
 if(seen_sign)
Cyc_Warn_err(loc,_tag_fat("sign specification on non-integral type",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
if(seen_size)
Cyc_Warn_err(loc,_tag_fat("size qualifier on non-integral type",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}_LL0:;}
# 699
return t;}
# 702
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 706
if(ds==0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*q=d.id;
unsigned varloc=d.varloc;
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f0;_Tmp3=_Tmp0.f1;_Tmp2=_Tmp0.f2;_Tmp1=_Tmp0.f3;{struct Cyc_Absyn_Tqual tq2=_Tmp4;void*new_typ=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 713
struct Cyc_List_List*tl=ds->tl==0?0:({struct _RegionHandle*_Tmp5=r;struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=Cyc_Tcutil_copy_type(t);struct _tuple13*_Tmp8=ds->tl;Cyc_Parse_apply_tmss(_Tmp5,_Tmp6,_Tmp7,_Tmp8,shared_atts);});
return({struct Cyc_List_List*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_region_malloc(r,0U,sizeof(struct _tuple15));_Tmp7->f0=varloc,_Tmp7->f1=q,_Tmp7->f2=tq2,_Tmp7->f3=new_typ,_Tmp7->f4=tvs,_Tmp7->f5=atts;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=tl;_Tmp5;});}}}
# 717
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 720
if(tms==0)return({struct _tuple14 _Tmp0;_Tmp0.f0=tq,_Tmp0.f1=t,_Tmp0.f2=0,_Tmp0.f3=atts;_Tmp0;});{
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;struct Cyc_Absyn_PtrAtts _Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*zeroterm=_Tmp5;unsigned ztloc=_Tmp4;
# 723
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 723
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 1: _Tmp5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp5;void*zeroterm=_Tmp3;unsigned ztloc=_Tmp4;
# 726
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 726
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 3: _Tmp5=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp5;
# 729
unsigned _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;void*_TmpD;int _TmpE;void*_TmpF;if(*((int*)args)==1){_TmpF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f1;_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f2;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f3;_TmpC=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f4;_TmpB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f5;_TmpA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f6;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f7;_Tmp8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f8;_Tmp7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f9;{struct Cyc_List_List*args2=_TmpF;int c_vararg=_TmpE;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpD;void*eff=_TmpC;struct Cyc_List_List*rgn_po=_TmpB;struct Cyc_List_List*qb=_TmpA;struct Cyc_Absyn_Exp*req=_Tmp9;struct Cyc_Absyn_Exp*ens=_Tmp8;struct Cyc_Absyn_Exp*thrw=_Tmp7;
# 731
struct Cyc_List_List*typvars=0;
# 733
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as!=0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));_Tmp10->hd=(void*)as->hd,_Tmp10->tl=fn_atts;_Tmp10;});else{
# 738
new_atts=({struct Cyc_List_List*_Tmp10=_cycalloc(sizeof(struct Cyc_List_List));_Tmp10->hd=(void*)as->hd,_Tmp10->tl=new_atts;_Tmp10;});}}}
# 740
if(tms->tl!=0){
void*_Tmp10=(void*)tms->tl->hd;void*_Tmp11;if(*((int*)_Tmp10)==4){_Tmp11=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp10)->f1;{struct Cyc_List_List*ts=_Tmp11;
# 743
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{
goto _LL12;}_LL12:;}
# 749
if(((((!c_vararg && cyc_vararg==0)&& args2!=0)&& args2->tl==0)&&(*((struct _tuple8*)args2->hd)).f0==0)&&(*((struct _tuple8*)args2->hd)).f2==Cyc_Absyn_void_type)
# 754
args2=0;{
# 759
struct Cyc_List_List*new_requires=0;
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_Tmp10=(struct _tuple8*)a->hd;void*_Tmp11;struct Cyc_Absyn_Tqual _Tmp12;void*_Tmp13;_Tmp13=_Tmp10->f0;_Tmp12=_Tmp10->f1;_Tmp11=(void**)& _Tmp10->f2;{struct _fat_ptr*vopt=_Tmp13;struct Cyc_Absyn_Tqual tq=_Tmp12;void**t=(void**)_Tmp11;
void*_Tmp14=*t;unsigned _Tmp15;void*_Tmp16;void*_Tmp17;struct Cyc_Absyn_Tqual _Tmp18;void*_Tmp19;if(*((int*)_Tmp14)==4){_Tmp19=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.elt_type;_Tmp18=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.tq;_Tmp17=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.num_elts;_Tmp16=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.zero_term;_Tmp15=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp14)->f1.zt_loc;{void*et=_Tmp19;struct Cyc_Absyn_Tqual tq=_Tmp18;struct Cyc_Absyn_Exp*neltsopt=_Tmp17;void*zt=_Tmp16;unsigned ztloc=_Tmp15;
# 764
if(neltsopt!=0 && vopt!=0){
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0)),v->f0.Loc_n.tag=4U,v->f0.Loc_n.val=0,v->f1=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,({struct Cyc_Absyn_Exp*_Tmp1A[1];({struct Cyc_Absyn_Exp*_Tmp1B=Cyc_Absyn_var_exp(v,0U);_Tmp1A[0]=_Tmp1B;});Cyc_List_list(_tag_fat(_Tmp1A,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);
new_requires=({struct Cyc_List_List*_Tmp1A=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1A->hd=new_req,_Tmp1A->tl=new_requires;_Tmp1A;});}}
# 771
goto _LL1A;}}else{
goto _LL1A;}_LL1A:;}}}
# 775
if(new_requires!=0){
struct Cyc_Absyn_Exp*r;
if(req!=0)
r=req;else{
# 780
r=(struct Cyc_Absyn_Exp*)new_requires->hd;
new_requires=new_requires->tl;}
# 783
for(1;new_requires!=0;new_requires=new_requires->tl){
r=Cyc_Absyn_and_exp(r,(struct Cyc_Absyn_Exp*)new_requires->hd,0U);}
req=r;}{
# 789
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 791
if(tags!=0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 796
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_Tmp10=(struct _tuple8*)a->hd;void*_Tmp11;struct Cyc_Absyn_Tqual _Tmp12;void*_Tmp13;_Tmp13=_Tmp10->f0;_Tmp12=_Tmp10->f1;_Tmp11=(void**)& _Tmp10->f2;{struct _fat_ptr*vopt=_Tmp13;struct Cyc_Absyn_Tqual tq=_Tmp12;void**t=(void**)_Tmp11;
if(tags!=0)
({void*_Tmp14=Cyc_Parse_substitute_tags(tags,*t);*t=_Tmp14;});
({void*_Tmp14=Cyc_Parse_array2ptr(*t,1);*t=_Tmp14;});}}}
# 808
return({struct Cyc_Absyn_Tqual _Tmp10=Cyc_Absyn_empty_tqual(tq.loc);void*_Tmp11=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,qb,fn_atts,req,ens,thrw);
# 808
struct Cyc_List_List*_Tmp12=new_atts;Cyc_Parse_apply_tms(_Tmp10,_Tmp11,_Tmp12,
# 813
_check_null(tms)->tl);});}}}}else{_Tmp6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f2;{unsigned loc=_Tmp6;
# 815
({int(*_Tmp10)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp10;})(loc,_tag_fat("function declaration without parameter types",sizeof(char),45U));}};}case 4: _Tmp5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;unsigned loc=_Tmp4;
# 822
if(tms->tl==0)
return({struct _tuple14 _Tmp6;_Tmp6.f0=tq,_Tmp6.f1=t,_Tmp6.f2=ts,_Tmp6.f3=atts;_Tmp6;});
# 827
({int(*_Tmp6)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_Tmp6;})(loc,
_tag_fat("type parameters must appear before function arguments in declarator",sizeof(char),68U));}case 2: _Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 830
return({struct Cyc_Absyn_Tqual _Tmp6=tq2;void*_Tmp7=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp8;_Tmp8.elt_type=t,_Tmp8.elt_tq=tq,_Tmp8.ptr_atts=ptratts;_Tmp8;}));struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}default: _Tmp4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{unsigned loc=_Tmp4;struct Cyc_List_List*atts2=_Tmp5;
# 835
return({struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=t;struct Cyc_List_List*_Tmp8=Cyc_List_append(atts,atts2);Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}};}}
# 841
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 850
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple15*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;unsigned _Tmp5;_Tmp5=t->f0;_Tmp4=t->f1;_Tmp3=t->f2;_Tmp2=t->f3;_Tmp1=t->f4;_Tmp0=t->f5;{unsigned varloc=_Tmp5;struct _tuple0*x=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*typ=_Tmp2;struct Cyc_List_List*tvs=_Tmp1;struct Cyc_List_List*atts=_Tmp0;
# 853
Cyc_Lex_register_typedef(x);{
# 855
struct Cyc_Core_Opt*kind;
void*type;
{void*_Tmp6;if(*((int*)typ)==1){_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)typ)->f1;{struct Cyc_Core_Opt*kopt=_Tmp6;
# 859
type=0;
kind=kopt==0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}
# 864
return({void*_Tmp6=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_Tmp7->tag=8,({struct Cyc_Absyn_Typedefdecl*_Tmp8=({struct Cyc_Absyn_Typedefdecl*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));_Tmp9->name=x,_Tmp9->tvs=tvs,_Tmp9->kind=kind,_Tmp9->defn=type,_Tmp9->atts=atts,_Tmp9->tq=tq,_Tmp9->extern_c=0;_Tmp9;});_Tmp7->f1=_Tmp8;});_Tmp7;});Cyc_Absyn_new_decl(_Tmp6,loc);});}}}
# 871
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});Cyc_Absyn_new_stmt(_Tmp0,d->loc);});}
# 875
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return({struct Cyc_Absyn_Stmt*(*_Tmp0)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right;_Tmp0;})(Cyc_Parse_flatten_decl,ds,s);}
# 879
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es){
# 882
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
for(1;ds!=0;ds=ds->tl){
struct _tuple12 _Tmp0=ds->hd;void*_Tmp1;struct Cyc_Parse_Declarator _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_Parse_Declarator d=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
declarators=({struct _tuple13*_Tmp3=_region_malloc(r,0U,sizeof(struct _tuple13));_Tmp3->tl=declarators,_Tmp3->hd=d;_Tmp3;});
exprs=({struct Cyc_List_List*_Tmp3=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp3->hd=e,_Tmp3->tl=exprs;_Tmp3;});}}
# 889
({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(exprs);*es=_Tmp0;});
({struct _tuple13*_Tmp0=({struct _tuple13*(*_Tmp1)(struct _tuple13*)=(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;_Tmp1;})(declarators);*decls=_Tmp0;});}
# 898
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 901
struct _RegionHandle _Tmp0=_new_region(0U,"mkrgn");struct _RegionHandle*mkrgn=& _Tmp0;_push_region(mkrgn);
{void*_Tmp1;struct Cyc_Parse_Type_specifier _Tmp2;struct Cyc_Absyn_Tqual _Tmp3;_Tmp3=ds.tq;_Tmp2=ds.type_specs;_Tmp1=ds.attributes;{struct Cyc_Absyn_Tqual tq=_Tmp3;struct Cyc_Parse_Type_specifier tss=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
if(tq.loc==0U)tq.loc=tqual_loc;
if(ds.is_inline)
Cyc_Warn_warn(loc,_tag_fat("inline qualifier on non-function definition",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));{
# 907
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
# 923
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 927
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es!=0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd!=0){
exps_empty=0;
break;}}}{
# 935
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators==0){
# 939
int _Tmp4;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;void*_Tmp7;switch(*((int*)base_type)){case 9: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1)->r)){case 0: _Tmp7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
# 941
({struct Cyc_List_List*_Tmp8=Cyc_List_append(ad->attributes,atts);ad->attributes=_Tmp8;});
ad->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 1: _Tmp7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;
# 945
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on enum not supported",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
ed->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpB->tag=7,_TmpB->f1=ed;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: _Tmp7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;
# 949
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on datatypes not supported",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));
dd->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=dd;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)){case 25: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.tag==1){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f0;_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f1;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp6;struct _tuple0*n=_Tmp7;struct Cyc_List_List*ts=_Tmp5;
# 953
struct Cyc_List_List*ts2=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),ad->kind=k,ad->sc=s,ad->name=n,ad->tvs=ts2,ad->impl=0,ad->attributes=0,ad->expected_mem_kind=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on type declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{goto _LL25;}case 23: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.tag==2){_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl**tudp=_Tmp7;
# 958
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=*tudp;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.name;_Tmp4=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.is_extensible;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{struct _tuple0*n=_Tmp7;int isx=_Tmp4;struct Cyc_List_List*ts=_Tmp5;
# 961
struct Cyc_List_List*ts2=({struct Cyc_List_List*(*_Tmp8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),_Tmp8->hd=tud,_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 20: _Tmp7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct _tuple0*n=_Tmp7;
# 966
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,ed->name=n,ed->fields=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 21: _Tmp7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct Cyc_List_List*fs=_Tmp7;
# 972
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,({struct _tuple0*_Tmp8=Cyc_Parse_gensym_enum();ed->name=_Tmp8;}),({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=fs;_Tmp9;});ed->fields=_Tmp8;});
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: goto _LL25;}default: _LL25:
 Cyc_Warn_err(loc,_tag_fat("missing declarator",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));{struct Cyc_List_List*_Tmp8=0;_npop_handler(0);return _Tmp8;}};}{
# 979
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);
if(istypedef){
# 984
if(!exps_empty)
Cyc_Warn_err(loc,_tag_fat("initializer in typedef declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*decls=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp4;})(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_Tmp4=decls;_npop_handler(0);return _Tmp4;}}{
# 990
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*ds=fields;for(0;ds!=0;(ds=ds->tl,exprs=_check_null(exprs)->tl)){
struct _tuple15*_Tmp4=(struct _tuple15*)ds->hd;void*_Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;unsigned _TmpA;_TmpA=_Tmp4->f0;_Tmp9=_Tmp4->f1;_Tmp8=_Tmp4->f2;_Tmp7=_Tmp4->f3;_Tmp6=_Tmp4->f4;_Tmp5=_Tmp4->f5;{unsigned varloc=_TmpA;struct _tuple0*x=_Tmp9;struct Cyc_Absyn_Tqual tq2=_Tmp8;void*t2=_Tmp7;struct Cyc_List_List*tvs2=_Tmp6;struct Cyc_List_List*atts2=_Tmp5;
if(tvs2!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(exprs==0)
({int(*_TmpB)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TmpB;})(loc,_tag_fat("unexpected NULL in parse!",sizeof(char),26U));{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)exprs->hd);
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({struct Cyc_Absyn_Decl*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpF->tag=0,_TmpF->f1=vd;_TmpF;});_TmpD->r=_TmpE;}),_TmpD->loc=loc;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=decls;_TmpB;});}}}}{
# 1003
struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(decls);_npop_handler(0);return _Tmp4;}}}}}}}}}
# 902
;_pop_region();}
# 1007
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
long long _Tmp0;char _Tmp1;int _Tmp2;switch(x.LongLong_c.tag){case 5: _Tmp2=x.Int_c.val.f1;{int i=_Tmp2;
return(unsigned)i;}case 2: _Tmp1=x.Char_c.val.f1;{char c=_Tmp1;
return(unsigned)c;}case 6: _Tmp0=x.LongLong_c.val.f1;{long long x=_Tmp0;
# 1012
unsigned long long y=(unsigned long long)x;
if(y > 4294967295U)
Cyc_Warn_err(loc,_tag_fat("integer constant too large",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
return(unsigned)x;}default:
# 1017
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err(loc,_tag_fat("expected integer constant but found %s",sizeof(char),39U),_tag_fat(_Tmp4,sizeof(void*),1));});
return 0U;};}
# 1023
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 14: _Tmp6=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*x=_Tmp6;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 1027
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_Tmp7,p->loc);});}}else{goto _LL13;}case 5: _Tmp6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp6;
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_Tmp7,p->loc);});}case 8:
 return Cyc_Absyn_null_exp(p->loc);case 9: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 10: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
return Cyc_Absyn_char_exp(c,p->loc);}case 11: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp4;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 15: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3==0){_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _tuple0*x=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 1034
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=({struct Cyc_List_List*(*_Tmp7)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;_Tmp7;})(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 16: _Tmp6=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
return e;}default: _LL13:
# 1039
 Cyc_Warn_err(p->loc,_tag_fat("cannot mix patterns and expressions in case",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_null_exp(p->loc);};}
# 1043
static struct _tuple16 Cyc_Parse_split_seq(struct Cyc_Absyn_Exp*maybe_seq){
void*_Tmp0=maybe_seq->r;void*_Tmp1;void*_Tmp2;if(*((int*)_Tmp0)==9){_Tmp2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp2;struct Cyc_Absyn_Exp*e2=_Tmp1;
# 1046
return({struct _tuple16 _Tmp3;_Tmp3.f0=e1,_Tmp3.f1=e2;_Tmp3;});}}else{
# 1048
return({struct _tuple16 _Tmp3;_Tmp3.f0=maybe_seq,_Tmp3.f1=0;_Tmp3;});};}
# 1051
static struct Cyc_Absyn_Exp*Cyc_Parse_join_assn(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
if(e1!=0 && e2!=0)return Cyc_Absyn_and_exp(e1,e2,0U);else{
if(e1!=0)return e1;else{
return e2;}}}struct _tuple21{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1056
static struct _tuple21 Cyc_Parse_join_assns(struct _tuple21 a1,struct _tuple21 a2){
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=a1.f0;_Tmp1=a1.f1;_Tmp0=a1.f2;{struct Cyc_Absyn_Exp*r1=_Tmp2;struct Cyc_Absyn_Exp*e1=_Tmp1;struct Cyc_Absyn_Exp*t1=_Tmp0;
void*_Tmp3;void*_Tmp4;void*_Tmp5;_Tmp5=a2.f0;_Tmp4=a2.f1;_Tmp3=a2.f2;{struct Cyc_Absyn_Exp*r2=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;struct Cyc_Absyn_Exp*t2=_Tmp3;
struct Cyc_Absyn_Exp*r=Cyc_Parse_join_assn(r1,r2);
struct Cyc_Absyn_Exp*e=Cyc_Parse_join_assn(e1,e2);
struct Cyc_Absyn_Exp*t=Cyc_Parse_join_assn(t1,t2);
return({struct _tuple21 _Tmp6;_Tmp6.f0=r,_Tmp6.f1=e,_Tmp6.f2=t;_Tmp6;});}}}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f0;void*f1;void*f2;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f0;int f1;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f0;enum Cyc_Absyn_AggrKind f1;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _tuple26{struct Cyc_Parse_Declarator f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY29{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_Absyn_Tqual f0;struct Cyc_Parse_Type_specifier f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY36{int tag;struct _tuple27 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f0;int f1;struct Cyc_Absyn_VarargInfo*f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _tuple29{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _union_YYSTYPE_YY51{int tag;struct _tuple29*val;};struct _tuple30{void*f0;void*f1;};struct _union_YYSTYPE_YY52{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _tuple31{struct Cyc_List_List*f0;unsigned f1;};struct _union_YYSTYPE_YY55{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;void*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _union_YYSTYPE_YY59{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY60{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY61{int tag;struct _tuple21 val;};struct _union_YYSTYPE_YY62{int tag;void*val;};struct _tuple32{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY64{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY65{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_List_List*val;};struct _tuple33{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};struct _union_YYSTYPE_YY67{int tag;struct _tuple33*val;};struct _union_YYSTYPE_YY68{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YY67 YY67;struct _union_YYSTYPE_YY68 YY68;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1147
static void Cyc_yythrowfail(struct _fat_ptr s){
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=s;_Tmp0;}));}static char _TmpG2[7U]="cnst_t";
# 1115 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2,_TmpG2,_TmpG2 + 7U};
union Cyc_Absyn_Cnst _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Int_tok.tag==1){_Tmp0=yy1->Int_tok.val;{union Cyc_Absyn_Cnst yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1122
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Int_tok.tag=1U,_Tmp0.Int_tok.val=yy1;_Tmp0;});}static char _TmpG3[5U]="char";
# 1116 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3,_TmpG3,_TmpG3 + 5U};
char _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Char_tok.tag==2){_Tmp0=yy1->Char_tok.val;{char yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1123
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Char_tok.tag=2U,_Tmp0.Char_tok.val=yy1;_Tmp0;});}static char _TmpG4[13U]="string_t<`H>";
# 1117 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4,_TmpG4,_TmpG4 + 13U};
struct _fat_ptr _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->String_tok.tag==3){_Tmp0=yy1->String_tok.val;{struct _fat_ptr yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1124
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.String_tok.tag=3U,_Tmp0.String_tok.val=yy1;_Tmp0;});}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1120 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG5,_TmpG5,_TmpG5 + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY1.tag==7){_Tmp0=yy1->YY1.val;{struct _tuple22*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1127
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY1.tag=7U,_Tmp0.YY1.val=yy1;_Tmp0;});}static char _TmpG6[11U]="ptrbound_t";
# 1121 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG6,_TmpG6,_TmpG6 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY2.tag==8){_Tmp0=yy1->YY2.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1128
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY2.tag=8U,_Tmp0.YY2.val=yy1;_Tmp0;});}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1122 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG7,_TmpG7,_TmpG7 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY3.tag==9){_Tmp0=yy1->YY3.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1129
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY3.tag=9U,_Tmp0.YY3.val=yy1;_Tmp0;});}static char _TmpG8[6U]="exp_t";
# 1123 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG8,_TmpG8,_TmpG8 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Exp_tok.tag==5){_Tmp0=yy1->Exp_tok.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1130
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Exp_tok.tag=5U,_Tmp0.Exp_tok.val=yy1;_Tmp0;});}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG9,_TmpG9,_TmpG9 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY4.tag==10){_Tmp0=yy1->YY4.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1138
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY4.tag=10U,_Tmp0.YY4.val=yy1;_Tmp0;});}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1132 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGA,_TmpGA,_TmpGA + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY5.tag==11){_Tmp0=yy1->YY5.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1139
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY5.tag=11U,_Tmp0.YY5.val=yy1;_Tmp0;});}static char _TmpGB[9U]="primop_t";
# 1133 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGB,_TmpGB,_TmpGB + 9U};
enum Cyc_Absyn_Primop _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY6.tag==12){_Tmp0=yy1->YY6.val;{enum Cyc_Absyn_Primop yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1140
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY6.tag=12U,_Tmp0.YY6.val=yy1;_Tmp0;});}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1134 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGC,_TmpGC,_TmpGC + 19U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY7.tag==13){_Tmp0=yy1->YY7.val;{struct Cyc_Core_Opt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1141
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY7.tag=13U,_Tmp0.YY7.val=yy1;_Tmp0;});}static char _TmpGD[7U]="qvar_t";
# 1135 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGD,_TmpGD,_TmpGD + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->QualId_tok.tag==4){_Tmp0=yy1->QualId_tok.val;{struct _tuple0*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1142
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.QualId_tok.tag=4U,_Tmp0.QualId_tok.val=yy1;_Tmp0;});}static char _TmpGE[7U]="stmt_t";
# 1138 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGE,_TmpGE,_TmpGE + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Stmt_tok.tag==6){_Tmp0=yy1->Stmt_tok.val;{struct Cyc_Absyn_Stmt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1145
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Stmt_tok.tag=6U,_Tmp0.Stmt_tok.val=yy1;_Tmp0;});}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1141 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGF,_TmpGF,_TmpGF + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY8.tag==14){_Tmp0=yy1->YY8.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1148
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY8.tag=14U,_Tmp0.YY8.val=yy1;_Tmp0;});}static char _TmpG10[6U]="pat_t";
# 1142 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG10,_TmpG10,_TmpG10 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY9.tag==15){_Tmp0=yy1->YY9.val;{struct Cyc_Absyn_Pat*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1149
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY9.tag=15U,_Tmp0.YY9.val=yy1;_Tmp0;});}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1147 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG11,_TmpG11,_TmpG11 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY10.tag==16){_Tmp0=yy1->YY10.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1154
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY10.tag=16U,_Tmp0.YY10.val=yy1;_Tmp0;});}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1148 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG12,_TmpG12,_TmpG12 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY11.tag==17){_Tmp0=yy1->YY11.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1155
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY11.tag=17U,_Tmp0.YY11.val=yy1;_Tmp0;});}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1149 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG13,_TmpG13,_TmpG13 + 36U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY12.tag==18){_Tmp0=yy1->YY12.val;{struct _tuple24*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1156
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY12.tag=18U,_Tmp0.YY12.val=yy1;_Tmp0;});}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1150 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG14,_TmpG14,_TmpG14 + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY13.tag==19){_Tmp0=yy1->YY13.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1157
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY13.tag=19U,_Tmp0.YY13.val=yy1;_Tmp0;});}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1151 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG15,_TmpG15,_TmpG15 + 58U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY14.tag==20){_Tmp0=yy1->YY14.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1158
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY14.tag=20U,_Tmp0.YY14.val=yy1;_Tmp0;});}static char _TmpG16[9U]="fndecl_t";
# 1152 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG16,_TmpG16,_TmpG16 + 9U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY15.tag==21){_Tmp0=yy1->YY15.val;{struct Cyc_Absyn_Fndecl*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1159
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY15.tag=21U,_Tmp0.YY15.val=yy1;_Tmp0;});}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1153 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG17,_TmpG17,_TmpG17 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY16.tag==22){_Tmp0=yy1->YY16.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1160
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY16.tag=22U,_Tmp0.YY16.val=yy1;_Tmp0;});}static char _TmpG18[12U]="decl_spec_t";
# 1156 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG18,_TmpG18,_TmpG18 + 12U};
struct Cyc_Parse_Declaration_spec _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY17.tag==23){_Tmp0=yy1->YY17.val;{struct Cyc_Parse_Declaration_spec yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1163
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY17.tag=23U,_Tmp0.YY17.val=yy1;_Tmp0;});}static char _TmpG19[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1157 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG19,_TmpG19,_TmpG19 + 31U};
struct _tuple12 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY18.tag==24){_Tmp0=yy1->YY18.val;{struct _tuple12 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1164
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY18.tag=24U,_Tmp0.YY18.val=yy1;_Tmp0;});}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1158 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1A,_TmpG1A,_TmpG1A + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY19.tag==25){_Tmp0=yy1->YY19.val;{struct _tuple11*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1165
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY19.tag=25U,_Tmp0.YY19.val=yy1;_Tmp0;});}static char _TmpG1B[16U]="storage_class_t";
# 1159 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1B,_TmpG1B,_TmpG1B + 16U};
enum Cyc_Parse_Storage_class _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY20.tag==26){_Tmp0=yy1->YY20.val;{enum Cyc_Parse_Storage_class yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1166
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY20.tag=26U,_Tmp0.YY20.val=yy1;_Tmp0;});}static char _TmpG1C[17U]="type_specifier_t";
# 1160 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1C,_TmpG1C,_TmpG1C + 17U};
struct Cyc_Parse_Type_specifier _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY21.tag==27){_Tmp0=yy1->YY21.val;{struct Cyc_Parse_Type_specifier yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1167
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY21.tag=27U,_Tmp0.YY21.val=yy1;_Tmp0;});}static char _TmpG1D[12U]="aggr_kind_t";
# 1162 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1D,_TmpG1D,_TmpG1D + 12U};
enum Cyc_Absyn_AggrKind _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY22.tag==28){_Tmp0=yy1->YY22.val;{enum Cyc_Absyn_AggrKind yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1169
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY22.tag=28U,_Tmp0.YY22.val=yy1;_Tmp0;});}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1163 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1E,_TmpG1E,_TmpG1E + 20U};
struct _tuple25 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY23.tag==29){_Tmp0=yy1->YY23.val;{struct _tuple25 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1170
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY23.tag=29U,_Tmp0.YY23.val=yy1;_Tmp0;});}static char _TmpG1F[8U]="tqual_t";
# 1164 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1F,_TmpG1F,_TmpG1F + 8U};
struct Cyc_Absyn_Tqual _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY24.tag==30){_Tmp0=yy1->YY24.val;{struct Cyc_Absyn_Tqual yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1171
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY24.tag=30U,_Tmp0.YY24.val=yy1;_Tmp0;});}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1165 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG20,_TmpG20,_TmpG20 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY25.tag==31){_Tmp0=yy1->YY25.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1172
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY25.tag=31U,_Tmp0.YY25.val=yy1;_Tmp0;});}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1166 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG21,_TmpG21,_TmpG21 + 34U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY26.tag==32){_Tmp0=yy1->YY26.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1173
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY26.tag=32U,_Tmp0.YY26.val=yy1;_Tmp0;});}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1167 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG22,_TmpG22,_TmpG22 + 33U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY27.tag==33){_Tmp0=yy1->YY27.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1174
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY27.tag=33U,_Tmp0.YY27.val=yy1;_Tmp0;});}static char _TmpG23[18U]="declarator_t<`yy>";
# 1168 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG23,_TmpG23,_TmpG23 + 18U};
struct Cyc_Parse_Declarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY28.tag==34){_Tmp0=yy1->YY28.val;{struct Cyc_Parse_Declarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1175
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY28.tag=34U,_Tmp0.YY28.val=yy1;_Tmp0;});}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1169 "parse.y"
static struct _tuple26*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG24,_TmpG24,_TmpG24 + 45U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY29.tag==35){_Tmp0=yy1->YY29.val;{struct _tuple26*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1176
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple26*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY29.tag=35U,_Tmp0.YY29.val=yy1;_Tmp0;});}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1170 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG25,_TmpG25,_TmpG25 + 57U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY30.tag==36){_Tmp0=yy1->YY30.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1177
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY30.tag=36U,_Tmp0.YY30.val=yy1;_Tmp0;});}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1171 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG26,_TmpG26,_TmpG26 + 26U};
struct Cyc_Parse_Abstractdeclarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY31.tag==37){_Tmp0=yy1->YY31.val;{struct Cyc_Parse_Abstractdeclarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1178
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY31.tag=37U,_Tmp0.YY31.val=yy1;_Tmp0;});}static char _TmpG27[5U]="bool";
# 1172 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG27,_TmpG27,_TmpG27 + 5U};
int _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY32.tag==38){_Tmp0=yy1->YY32.val;{int yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1179
static union Cyc_YYSTYPE Cyc_YY32(int yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY32.tag=38U,_Tmp0.YY32.val=yy1;_Tmp0;});}static char _TmpG28[8U]="scope_t";
# 1173 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG28,_TmpG28,_TmpG28 + 8U};
enum Cyc_Absyn_Scope _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY33.tag==39){_Tmp0=yy1->YY33.val;{enum Cyc_Absyn_Scope yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1180
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY33.tag=39U,_Tmp0.YY33.val=yy1;_Tmp0;});}static char _TmpG29[16U]="datatypefield_t";
# 1174 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG29,_TmpG29,_TmpG29 + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY34.tag==40){_Tmp0=yy1->YY34.val;{struct Cyc_Absyn_Datatypefield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1181
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY34.tag=40U,_Tmp0.YY34.val=yy1;_Tmp0;});}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1175 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2A,_TmpG2A,_TmpG2A + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY35.tag==41){_Tmp0=yy1->YY35.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1182
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY35.tag=41U,_Tmp0.YY35.val=yy1;_Tmp0;});}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1176 "parse.y"
static struct _tuple27 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2B,_TmpG2B,_TmpG2B + 41U};
struct _tuple27 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY36.tag==42){_Tmp0=yy1->YY36.val;{struct _tuple27 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1183
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple27 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY36.tag=42U,_Tmp0.YY36.val=yy1;_Tmp0;});}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1177 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2C,_TmpG2C,_TmpG2C + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY37.tag==43){_Tmp0=yy1->YY37.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1184
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY37.tag=43U,_Tmp0.YY37.val=yy1;_Tmp0;});}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1178 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2D,_TmpG2D,_TmpG2D + 31U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY38.tag==44){_Tmp0=yy1->YY38.val;{struct _tuple8*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1185
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY38.tag=44U,_Tmp0.YY38.val=yy1;_Tmp0;});}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1179 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2E,_TmpG2E,_TmpG2E + 42U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY39.tag==45){_Tmp0=yy1->YY39.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1186
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY39.tag=45U,_Tmp0.YY39.val=yy1;_Tmp0;});}static char _TmpG2F[147U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1180 "parse.y"
static struct _tuple28*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2F,_TmpG2F,_TmpG2F + 147U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY40.tag==46){_Tmp0=yy1->YY40.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1187
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple28*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY40.tag=46U,_Tmp0.YY40.val=yy1;_Tmp0;});}static char _TmpG30[8U]="types_t";
# 1181 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG30,_TmpG30,_TmpG30 + 8U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY41.tag==47){_Tmp0=yy1->YY41.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1188
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY41.tag=47U,_Tmp0.YY41.val=yy1;_Tmp0;});}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1183 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG31,_TmpG31,_TmpG31 + 24U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY42.tag==48){_Tmp0=yy1->YY42.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1190
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY42.tag=48U,_Tmp0.YY42.val=yy1;_Tmp0;});}static char _TmpG32[13U]="designator_t";
# 1184 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG32,_TmpG32,_TmpG32 + 13U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY43.tag==49){_Tmp0=yy1->YY43.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1191
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY43.tag=49U,_Tmp0.YY43.val=yy1;_Tmp0;});}static char _TmpG33[7U]="kind_t";
# 1185 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG33,_TmpG33,_TmpG33 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY44.tag==50){_Tmp0=yy1->YY44.val;{struct Cyc_Absyn_Kind*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1192
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY44.tag=50U,_Tmp0.YY44.val=yy1;_Tmp0;});}static char _TmpG34[7U]="type_t";
# 1186 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG34,_TmpG34,_TmpG34 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY45.tag==51){_Tmp0=yy1->YY45.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1193
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY45.tag=51U,_Tmp0.YY45.val=yy1;_Tmp0;});}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1187 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG35,_TmpG35,_TmpG35 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY46.tag==52){_Tmp0=yy1->YY46.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1194
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY46.tag=52U,_Tmp0.YY46.val=yy1;_Tmp0;});}static char _TmpG36[12U]="attribute_t";
# 1188 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG36,_TmpG36,_TmpG36 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY47.tag==53){_Tmp0=yy1->YY47.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1195
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY47.tag=53U,_Tmp0.YY47.val=yy1;_Tmp0;});}static char _TmpG37[12U]="enumfield_t";
# 1189 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG37,_TmpG37,_TmpG37 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY48.tag==54){_Tmp0=yy1->YY48.val;{struct Cyc_Absyn_Enumfield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1196
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY48.tag=54U,_Tmp0.YY48.val=yy1;_Tmp0;});}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1190 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG38,_TmpG38,_TmpG38 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY49.tag==55){_Tmp0=yy1->YY49.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1197
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY49.tag=55U,_Tmp0.YY49.val=yy1;_Tmp0;});}static char _TmpG39[11U]="type_opt_t";
# 1191 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG39,_TmpG39,_TmpG39 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY50.tag==56){_Tmp0=yy1->YY50.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1198
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY50.tag=56U,_Tmp0.YY50.val=yy1;_Tmp0;});}static char _TmpG3A[69U]="$(list_t<$(type_t,type_t)@`H,`H>, list_t<$(type_t,type_t)@`H,`H>)*`H";
# 1193 "parse.y"
static struct _tuple29*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3A,_TmpG3A,_TmpG3A + 69U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY51.tag==57){_Tmp0=yy1->YY51.val;{struct _tuple29*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1200
static union Cyc_YYSTYPE Cyc_YY51(struct _tuple29*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY51.tag=57U,_Tmp0.YY51.val=yy1;_Tmp0;});}static char _TmpG3B[21U]="$(type_t, type_t)@`H";
# 1194 "parse.y"
static struct _tuple30*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3B,_TmpG3B,_TmpG3B + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY52.tag==58){_Tmp0=yy1->YY52.val;{struct _tuple30*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1201
static union Cyc_YYSTYPE Cyc_YY52(struct _tuple30*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY52.tag=58U,_Tmp0.YY52.val=yy1;_Tmp0;});}static char _TmpG3C[11U]="booltype_t";
# 1195 "parse.y"
static void*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3C,_TmpG3C,_TmpG3C + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY53.tag==59){_Tmp0=yy1->YY53.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1202
static union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY53.tag=59U,_Tmp0.YY53.val=yy1;_Tmp0;});}static char _TmpG3D[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1196 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3D,_TmpG3D,_TmpG3D + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY54.tag==60){_Tmp0=yy1->YY54.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1203
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY54.tag=60U,_Tmp0.YY54.val=yy1;_Tmp0;});}static char _TmpG3E[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1197 "parse.y"
static struct _tuple31*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3E,_TmpG3E,_TmpG3E + 48U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY55.tag==61){_Tmp0=yy1->YY55.val;{struct _tuple31*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1204
static union Cyc_YYSTYPE Cyc_YY55(struct _tuple31*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY55.tag=61U,_Tmp0.YY55.val=yy1;_Tmp0;});}static char _TmpG3F[18U]="list_t<qvar_t,`H>";
# 1198 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3F,_TmpG3F,_TmpG3F + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY56.tag==62){_Tmp0=yy1->YY56.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1205
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY56.tag=62U,_Tmp0.YY56.val=yy1;_Tmp0;});}static char _TmpG40[12U]="aqualtype_t";
# 1199 "parse.y"
static void*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG40,_TmpG40,_TmpG40 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY57.tag==63){_Tmp0=yy1->YY57.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1206
static union Cyc_YYSTYPE Cyc_YY57(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY57.tag=63U,_Tmp0.YY57.val=yy1;_Tmp0;});}static char _TmpG41[20U]="pointer_qual_t<`yy>";
# 1200 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG41,_TmpG41,_TmpG41 + 20U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY58.tag==64){_Tmp0=yy1->YY58.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1207
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY58.tag=64U,_Tmp0.YY58.val=yy1;_Tmp0;});}static char _TmpG42[21U]="pointer_quals_t<`yy>";
# 1201 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG42,_TmpG42,_TmpG42 + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY59.tag==65){_Tmp0=yy1->YY59.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1208
static union Cyc_YYSTYPE Cyc_YY59(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY59.tag=65U,_Tmp0.YY59.val=yy1;_Tmp0;});}static char _TmpG43[10U]="exp_opt_t";
# 1202 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG43,_TmpG43,_TmpG43 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY60.tag==66){_Tmp0=yy1->YY60.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1209
static union Cyc_YYSTYPE Cyc_YY60(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY60.tag=66U,_Tmp0.YY60.val=yy1;_Tmp0;});}static char _TmpG44[33U]="$(exp_opt_t,exp_opt_t,exp_opt_t)";
# 1203 "parse.y"
static struct _tuple21 Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG44,_TmpG44,_TmpG44 + 33U};
struct _tuple21 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY61.tag==67){_Tmp0=yy1->YY61.val;{struct _tuple21 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1210
static union Cyc_YYSTYPE Cyc_YY61(struct _tuple21 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY61.tag=67U,_Tmp0.YY61.val=yy1;_Tmp0;});}static char _TmpG45[10U]="raw_exp_t";
# 1205 "parse.y"
static void*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG45,_TmpG45,_TmpG45 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY62.tag==68){_Tmp0=yy1->YY62.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1212
static union Cyc_YYSTYPE Cyc_YY62(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY62.tag=68U,_Tmp0.YY62.val=yy1;_Tmp0;});}static char _TmpG46[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1206 "parse.y"
static struct _tuple32*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG46,_TmpG46,_TmpG46 + 112U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY63.tag==69){_Tmp0=yy1->YY63.val;{struct _tuple32*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1213
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple32*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY63.tag=69U,_Tmp0.YY63.val=yy1;_Tmp0;});}static char _TmpG47[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1207 "parse.y"
static struct _tuple29*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG47,_TmpG47,_TmpG47 + 73U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY64.tag==70){_Tmp0=yy1->YY64.val;{struct _tuple29*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1214
static union Cyc_YYSTYPE Cyc_YY64(struct _tuple29*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY64.tag=70U,_Tmp0.YY64.val=yy1;_Tmp0;});}static char _TmpG48[28U]="list_t<string_t<`H>@`H, `H>";
# 1208 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG48,_TmpG48,_TmpG48 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY65.tag==71){_Tmp0=yy1->YY65.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1215
static union Cyc_YYSTYPE Cyc_YY65(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY65.tag=71U,_Tmp0.YY65.val=yy1;_Tmp0;});}static char _TmpG49[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1209 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG49,_TmpG49,_TmpG49 + 38U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY66.tag==72){_Tmp0=yy1->YY66.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1216
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY66.tag=72U,_Tmp0.YY66.val=yy1;_Tmp0;});}static char _TmpG4A[26U]="$(string_t<`H>, exp_t)@`H";
# 1210 "parse.y"
static struct _tuple33*Cyc_yyget_YY67(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4A,_TmpG4A,_TmpG4A + 26U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY67.tag==73){_Tmp0=yy1->YY67.val;{struct _tuple33*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1217
static union Cyc_YYSTYPE Cyc_YY67(struct _tuple33*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY67.tag=73U,_Tmp0.YY67.val=yy1;_Tmp0;});}static char _TmpG4B[16U]="exp_maker_fun_t";
# 1211 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY68(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_TmpG4B,_TmpG4B,_TmpG4B + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY68.tag==74){_Tmp0=yy1->YY68.val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1218
static union Cyc_YYSTYPE Cyc_YY68(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY68.tag=74U,_Tmp0.YY68.val=yy1;_Tmp0;});}struct Cyc_Yyltype{int timestamp;unsigned first_line;unsigned first_column;unsigned last_line;unsigned last_column;};
# 1234
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _Tmp0;_Tmp0.timestamp=0,_Tmp0.first_line=0U,_Tmp0.first_column=0U,_Tmp0.last_line=0U,_Tmp0.last_column=0U;_Tmp0;});}
# 1237
struct Cyc_Yyltype Cyc_yylloc={0,0U,0U,0U,0U};
# 1248 "parse.y"
static short Cyc_yytranslate[396U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,167,2,2,152,165,162,2,149,150,145,159,144,163,154,164,2,2,2,2,2,2,2,2,2,2,153,141,147,146,148,158,157,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,155,2,156,161,151,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,142,160,143,166,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140};static char _TmpG4C[2U]="$";static char _TmpG4D[6U]="error";static char _TmpG4E[12U]="$undefined.";static char _TmpG4F[5U]="AUTO";static char _TmpG50[9U]="REGISTER";static char _TmpG51[7U]="STATIC";static char _TmpG52[7U]="EXTERN";static char _TmpG53[8U]="TYPEDEF";static char _TmpG54[5U]="VOID";static char _TmpG55[5U]="CHAR";static char _TmpG56[6U]="SHORT";static char _TmpG57[4U]="INT";static char _TmpG58[5U]="LONG";static char _TmpG59[6U]="FLOAT";static char _TmpG5A[7U]="DOUBLE";static char _TmpG5B[7U]="SIGNED";static char _TmpG5C[9U]="UNSIGNED";static char _TmpG5D[6U]="CONST";static char _TmpG5E[9U]="VOLATILE";static char _TmpG5F[9U]="RESTRICT";static char _TmpG60[7U]="STRUCT";static char _TmpG61[6U]="UNION";static char _TmpG62[5U]="CASE";static char _TmpG63[8U]="DEFAULT";static char _TmpG64[7U]="INLINE";static char _TmpG65[7U]="SIZEOF";static char _TmpG66[9U]="OFFSETOF";static char _TmpG67[3U]="IF";static char _TmpG68[5U]="ELSE";static char _TmpG69[7U]="SWITCH";static char _TmpG6A[6U]="WHILE";static char _TmpG6B[3U]="DO";static char _TmpG6C[4U]="FOR";static char _TmpG6D[5U]="GOTO";static char _TmpG6E[9U]="CONTINUE";static char _TmpG6F[6U]="BREAK";static char _TmpG70[7U]="RETURN";static char _TmpG71[5U]="ENUM";static char _TmpG72[7U]="TYPEOF";static char _TmpG73[16U]="BUILTIN_VA_LIST";static char _TmpG74[10U]="EXTENSION";static char _TmpG75[8U]="COMPLEX";static char _TmpG76[8U]="NULL_kw";static char _TmpG77[4U]="LET";static char _TmpG78[6U]="THROW";static char _TmpG79[4U]="TRY";static char _TmpG7A[6U]="CATCH";static char _TmpG7B[7U]="EXPORT";static char _TmpG7C[9U]="OVERRIDE";static char _TmpG7D[5U]="HIDE";static char _TmpG7E[4U]="NEW";static char _TmpG7F[5U]="QNEW";static char _TmpG80[9U]="ABSTRACT";static char _TmpG81[9U]="FALLTHRU";static char _TmpG82[6U]="USING";static char _TmpG83[10U]="NAMESPACE";static char _TmpG84[9U]="DATATYPE";static char _TmpG85[7U]="MALLOC";static char _TmpG86[8U]="RMALLOC";static char _TmpG87[9U]="RVMALLOC";static char _TmpG88[15U]="RMALLOC_INLINE";static char _TmpG89[8U]="QMALLOC";static char _TmpG8A[7U]="CALLOC";static char _TmpG8B[8U]="QCALLOC";static char _TmpG8C[8U]="RCALLOC";static char _TmpG8D[5U]="SWAP";static char _TmpG8E[9U]="REGION_T";static char _TmpG8F[6U]="TAG_T";static char _TmpG90[7U]="REGION";static char _TmpG91[5U]="RNEW";static char _TmpG92[8U]="REGIONS";static char _TmpG93[7U]="PORTON";static char _TmpG94[8U]="PORTOFF";static char _TmpG95[7U]="PRAGMA";static char _TmpG96[10U]="TEMPESTON";static char _TmpG97[11U]="TEMPESTOFF";static char _TmpG98[13U]="AQ_ALIASABLE";static char _TmpG99[10U]="AQ_REFCNT";static char _TmpG9A[14U]="AQ_RESTRICTED";static char _TmpG9B[10U]="AQ_UNIQUE";static char _TmpG9C[8U]="AQUAL_T";static char _TmpG9D[8U]="NUMELTS";static char _TmpG9E[8U]="VALUEOF";static char _TmpG9F[10U]="VALUEOF_T";static char _TmpGA0[9U]="TAGCHECK";static char _TmpGA1[13U]="NUMELTS_QUAL";static char _TmpGA2[10U]="THIN_QUAL";static char _TmpGA3[9U]="FAT_QUAL";static char _TmpGA4[13U]="NOTNULL_QUAL";static char _TmpGA5[14U]="NULLABLE_QUAL";static char _TmpGA6[14U]="REQUIRES_QUAL";static char _TmpGA7[13U]="ENSURES_QUAL";static char _TmpGA8[12U]="THROWS_QUAL";static char _TmpGA9[12U]="REGION_QUAL";static char _TmpGAA[16U]="NOZEROTERM_QUAL";static char _TmpGAB[14U]="ZEROTERM_QUAL";static char _TmpGAC[12U]="TAGGED_QUAL";static char _TmpGAD[12U]="ASSERT_QUAL";static char _TmpGAE[18U]="ASSERT_FALSE_QUAL";static char _TmpGAF[11U]="ALIAS_QUAL";static char _TmpGB0[7U]="AQUALS";static char _TmpGB1[16U]="EXTENSIBLE_QUAL";static char _TmpGB2[18U]="AUTORELEASED_QUAL";static char _TmpGB3[7U]="PTR_OP";static char _TmpGB4[7U]="INC_OP";static char _TmpGB5[7U]="DEC_OP";static char _TmpGB6[8U]="LEFT_OP";static char _TmpGB7[9U]="RIGHT_OP";static char _TmpGB8[6U]="LE_OP";static char _TmpGB9[6U]="GE_OP";static char _TmpGBA[6U]="EQ_OP";static char _TmpGBB[6U]="NE_OP";static char _TmpGBC[7U]="AND_OP";static char _TmpGBD[6U]="OR_OP";static char _TmpGBE[11U]="MUL_ASSIGN";static char _TmpGBF[11U]="DIV_ASSIGN";static char _TmpGC0[11U]="MOD_ASSIGN";static char _TmpGC1[11U]="ADD_ASSIGN";static char _TmpGC2[11U]="SUB_ASSIGN";static char _TmpGC3[12U]="LEFT_ASSIGN";static char _TmpGC4[13U]="RIGHT_ASSIGN";static char _TmpGC5[11U]="AND_ASSIGN";static char _TmpGC6[11U]="XOR_ASSIGN";static char _TmpGC7[10U]="OR_ASSIGN";static char _TmpGC8[9U]="ELLIPSIS";static char _TmpGC9[11U]="LEFT_RIGHT";static char _TmpGCA[12U]="COLON_COLON";static char _TmpGCB[11U]="IDENTIFIER";static char _TmpGCC[17U]="INTEGER_CONSTANT";static char _TmpGCD[7U]="STRING";static char _TmpGCE[8U]="WSTRING";static char _TmpGCF[19U]="CHARACTER_CONSTANT";static char _TmpGD0[20U]="WCHARACTER_CONSTANT";static char _TmpGD1[18U]="FLOATING_CONSTANT";static char _TmpGD2[9U]="TYPE_VAR";static char _TmpGD3[13U]="TYPEDEF_NAME";static char _TmpGD4[16U]="QUAL_IDENTIFIER";static char _TmpGD5[18U]="QUAL_TYPEDEF_NAME";static char _TmpGD6[18U]="AQUAL_SHORT_CONST";static char _TmpGD7[10U]="ATTRIBUTE";static char _TmpGD8[8U]="ASM_TOK";static char _TmpGD9[4U]="';'";static char _TmpGDA[4U]="'{'";static char _TmpGDB[4U]="'}'";static char _TmpGDC[4U]="','";static char _TmpGDD[4U]="'*'";static char _TmpGDE[4U]="'='";static char _TmpGDF[4U]="'<'";static char _TmpGE0[4U]="'>'";static char _TmpGE1[4U]="'('";static char _TmpGE2[4U]="')'";static char _TmpGE3[4U]="'_'";static char _TmpGE4[4U]="'$'";static char _TmpGE5[4U]="':'";static char _TmpGE6[4U]="'.'";static char _TmpGE7[4U]="'['";static char _TmpGE8[4U]="']'";static char _TmpGE9[4U]="'@'";static char _TmpGEA[4U]="'?'";static char _TmpGEB[4U]="'+'";static char _TmpGEC[4U]="'|'";static char _TmpGED[4U]="'^'";static char _TmpGEE[4U]="'&'";static char _TmpGEF[4U]="'-'";static char _TmpGF0[4U]="'/'";static char _TmpGF1[4U]="'%'";static char _TmpGF2[4U]="'~'";static char _TmpGF3[4U]="'!'";static char _TmpGF4[5U]="prog";static char _TmpGF5[17U]="translation_unit";static char _TmpGF6[18U]="tempest_on_action";static char _TmpGF7[19U]="tempest_off_action";static char _TmpGF8[16U]="extern_c_action";static char _TmpGF9[13U]="end_extern_c";static char _TmpGFA[14U]="hide_list_opt";static char _TmpGFB[17U]="hide_list_values";static char _TmpGFC[16U]="export_list_opt";static char _TmpGFD[12U]="export_list";static char _TmpGFE[19U]="export_list_values";static char _TmpGFF[13U]="override_opt";static char _TmpG100[21U]="external_declaration";static char _TmpG101[14U]="optional_semi";static char _TmpG102[20U]="function_definition";static char _TmpG103[21U]="function_definition2";static char _TmpG104[13U]="using_action";static char _TmpG105[15U]="unusing_action";static char _TmpG106[17U]="namespace_action";static char _TmpG107[19U]="unnamespace_action";static char _TmpG108[12U]="declaration";static char _TmpG109[9U]="open_opt";static char _TmpG10A[17U]="declaration_list";static char _TmpG10B[23U]="declaration_specifiers";static char _TmpG10C[24U]="storage_class_specifier";static char _TmpG10D[15U]="attributes_opt";static char _TmpG10E[11U]="attributes";static char _TmpG10F[15U]="attribute_list";static char _TmpG110[10U]="attribute";static char _TmpG111[15U]="type_specifier";static char _TmpG112[25U]="type_specifier_notypedef";static char _TmpG113[5U]="kind";static char _TmpG114[15U]="type_qualifier";static char _TmpG115[15U]="enum_specifier";static char _TmpG116[11U]="enum_field";static char _TmpG117[22U]="enum_declaration_list";static char _TmpG118[26U]="struct_or_union_specifier";static char _TmpG119[26U]="maybe_tagged_struct_union";static char _TmpG11A[16U]="struct_or_union";static char _TmpG11B[16U]="type_params_opt";static char _TmpG11C[24U]="struct_declaration_list";static char _TmpG11D[25U]="struct_declaration_list0";static char _TmpG11E[25U]="init_declarator_list_rev";static char _TmpG11F[16U]="init_declarator";static char _TmpG120[19U]="struct_declaration";static char _TmpG121[25U]="specifier_qualifier_list";static char _TmpG122[35U]="notypedef_specifier_qualifier_list";static char _TmpG123[27U]="struct_declarator_list_rev";static char _TmpG124[18U]="struct_declarator";static char _TmpG125[20U]="requires_clause_opt";static char _TmpG126[19U]="datatype_specifier";static char _TmpG127[14U]="qual_datatype";static char _TmpG128[19U]="datatypefield_list";static char _TmpG129[20U]="datatypefield_scope";static char _TmpG12A[14U]="datatypefield";static char _TmpG12B[11U]="declarator";static char _TmpG12C[23U]="declarator_withtypedef";static char _TmpG12D[18U]="direct_declarator";static char _TmpG12E[30U]="direct_declarator_withtypedef";static char _TmpG12F[8U]="pointer";static char _TmpG130[12U]="one_pointer";static char _TmpG131[14U]="pointer_quals";static char _TmpG132[13U]="pointer_qual";static char _TmpG133[16U]="aqual_specifier";static char _TmpG134[23U]="pointer_null_and_bound";static char _TmpG135[14U]="pointer_bound";static char _TmpG136[18U]="zeroterm_qual_opt";static char _TmpG137[8U]="rgn_opt";static char _TmpG138[11U]="tqual_list";static char _TmpG139[20U]="parameter_type_list";static char _TmpG13A[14U]="opt_aqual_bnd";static char _TmpG13B[9U]="type_var";static char _TmpG13C[16U]="optional_effect";static char _TmpG13D[23U]="optional_rgnpo_qualbnd";static char _TmpG13E[14U]="rgnpo_qualbnd";static char _TmpG13F[14U]="rgn_order_elt";static char _TmpG140[13U]="qual_bnd_elt";static char _TmpG141[12U]="aqual_const";static char _TmpG142[15U]="qual_bnd_const";static char _TmpG143[14U]="qual_bnd_term";static char _TmpG144[16U]="optional_inject";static char _TmpG145[11U]="effect_set";static char _TmpG146[14U]="atomic_effect";static char _TmpG147[11U]="region_set";static char _TmpG148[15U]="parameter_list";static char _TmpG149[22U]="parameter_declaration";static char _TmpG14A[16U]="identifier_list";static char _TmpG14B[17U]="identifier_list0";static char _TmpG14C[12U]="initializer";static char _TmpG14D[18U]="array_initializer";static char _TmpG14E[17U]="initializer_list";static char _TmpG14F[12U]="designation";static char _TmpG150[16U]="designator_list";static char _TmpG151[11U]="designator";static char _TmpG152[10U]="type_name";static char _TmpG153[14U]="any_type_name";static char _TmpG154[15U]="type_name_list";static char _TmpG155[20U]="abstract_declarator";static char _TmpG156[27U]="direct_abstract_declarator";static char _TmpG157[32U]="requires_and_ensures_and_throws";static char _TmpG158[36U]="requires_and_ensures_and_throws_opt";static char _TmpG159[10U]="statement";static char _TmpG15A[18U]="labeled_statement";static char _TmpG15B[21U]="expression_statement";static char _TmpG15C[19U]="compound_statement";static char _TmpG15D[16U]="block_item_list";static char _TmpG15E[20U]="selection_statement";static char _TmpG15F[15U]="switch_clauses";static char _TmpG160[20U]="iteration_statement";static char _TmpG161[12U]="for_exp_opt";static char _TmpG162[15U]="jump_statement";static char _TmpG163[12U]="exp_pattern";static char _TmpG164[20U]="conditional_pattern";static char _TmpG165[19U]="logical_or_pattern";static char _TmpG166[20U]="logical_and_pattern";static char _TmpG167[21U]="inclusive_or_pattern";static char _TmpG168[21U]="exclusive_or_pattern";static char _TmpG169[12U]="and_pattern";static char _TmpG16A[17U]="equality_pattern";static char _TmpG16B[19U]="relational_pattern";static char _TmpG16C[14U]="shift_pattern";static char _TmpG16D[17U]="additive_pattern";static char _TmpG16E[23U]="multiplicative_pattern";static char _TmpG16F[13U]="cast_pattern";static char _TmpG170[14U]="unary_pattern";static char _TmpG171[16U]="postfix_pattern";static char _TmpG172[16U]="primary_pattern";static char _TmpG173[8U]="pattern";static char _TmpG174[19U]="tuple_pattern_list";static char _TmpG175[20U]="tuple_pattern_list0";static char _TmpG176[14U]="field_pattern";static char _TmpG177[19U]="field_pattern_list";static char _TmpG178[20U]="field_pattern_list0";static char _TmpG179[11U]="expression";static char _TmpG17A[22U]="assignment_expression";static char _TmpG17B[20U]="assignment_operator";static char _TmpG17C[23U]="conditional_expression";static char _TmpG17D[20U]="constant_expression";static char _TmpG17E[22U]="logical_or_expression";static char _TmpG17F[23U]="logical_and_expression";static char _TmpG180[24U]="inclusive_or_expression";static char _TmpG181[24U]="exclusive_or_expression";static char _TmpG182[15U]="and_expression";static char _TmpG183[20U]="equality_expression";static char _TmpG184[22U]="relational_expression";static char _TmpG185[17U]="shift_expression";static char _TmpG186[20U]="additive_expression";static char _TmpG187[26U]="multiplicative_expression";static char _TmpG188[12U]="equality_op";static char _TmpG189[14U]="relational_op";static char _TmpG18A[12U]="additive_op";static char _TmpG18B[18U]="multiplicative_op";static char _TmpG18C[16U]="cast_expression";static char _TmpG18D[17U]="unary_expression";static char _TmpG18E[15U]="unary_operator";static char _TmpG18F[9U]="asm_expr";static char _TmpG190[13U]="volatile_opt";static char _TmpG191[12U]="asm_out_opt";static char _TmpG192[12U]="asm_outlist";static char _TmpG193[11U]="asm_in_opt";static char _TmpG194[11U]="asm_inlist";static char _TmpG195[11U]="asm_io_elt";static char _TmpG196[16U]="asm_clobber_opt";static char _TmpG197[17U]="asm_clobber_list";static char _TmpG198[19U]="postfix_expression";static char _TmpG199[17U]="field_expression";static char _TmpG19A[19U]="primary_expression";static char _TmpG19B[25U]="argument_expression_list";static char _TmpG19C[26U]="argument_expression_list0";static char _TmpG19D[9U]="constant";static char _TmpG19E[20U]="qual_opt_identifier";static char _TmpG19F[17U]="qual_opt_typedef";static char _TmpG1A0[18U]="struct_union_name";static char _TmpG1A1[11U]="field_name";static char _TmpG1A2[12U]="right_angle";
# 1624 "parse.y"
static struct _fat_ptr Cyc_yytname[343U]={{_TmpG4C,_TmpG4C,_TmpG4C + 2U},{_TmpG4D,_TmpG4D,_TmpG4D + 6U},{_TmpG4E,_TmpG4E,_TmpG4E + 12U},{_TmpG4F,_TmpG4F,_TmpG4F + 5U},{_TmpG50,_TmpG50,_TmpG50 + 9U},{_TmpG51,_TmpG51,_TmpG51 + 7U},{_TmpG52,_TmpG52,_TmpG52 + 7U},{_TmpG53,_TmpG53,_TmpG53 + 8U},{_TmpG54,_TmpG54,_TmpG54 + 5U},{_TmpG55,_TmpG55,_TmpG55 + 5U},{_TmpG56,_TmpG56,_TmpG56 + 6U},{_TmpG57,_TmpG57,_TmpG57 + 4U},{_TmpG58,_TmpG58,_TmpG58 + 5U},{_TmpG59,_TmpG59,_TmpG59 + 6U},{_TmpG5A,_TmpG5A,_TmpG5A + 7U},{_TmpG5B,_TmpG5B,_TmpG5B + 7U},{_TmpG5C,_TmpG5C,_TmpG5C + 9U},{_TmpG5D,_TmpG5D,_TmpG5D + 6U},{_TmpG5E,_TmpG5E,_TmpG5E + 9U},{_TmpG5F,_TmpG5F,_TmpG5F + 9U},{_TmpG60,_TmpG60,_TmpG60 + 7U},{_TmpG61,_TmpG61,_TmpG61 + 6U},{_TmpG62,_TmpG62,_TmpG62 + 5U},{_TmpG63,_TmpG63,_TmpG63 + 8U},{_TmpG64,_TmpG64,_TmpG64 + 7U},{_TmpG65,_TmpG65,_TmpG65 + 7U},{_TmpG66,_TmpG66,_TmpG66 + 9U},{_TmpG67,_TmpG67,_TmpG67 + 3U},{_TmpG68,_TmpG68,_TmpG68 + 5U},{_TmpG69,_TmpG69,_TmpG69 + 7U},{_TmpG6A,_TmpG6A,_TmpG6A + 6U},{_TmpG6B,_TmpG6B,_TmpG6B + 3U},{_TmpG6C,_TmpG6C,_TmpG6C + 4U},{_TmpG6D,_TmpG6D,_TmpG6D + 5U},{_TmpG6E,_TmpG6E,_TmpG6E + 9U},{_TmpG6F,_TmpG6F,_TmpG6F + 6U},{_TmpG70,_TmpG70,_TmpG70 + 7U},{_TmpG71,_TmpG71,_TmpG71 + 5U},{_TmpG72,_TmpG72,_TmpG72 + 7U},{_TmpG73,_TmpG73,_TmpG73 + 16U},{_TmpG74,_TmpG74,_TmpG74 + 10U},{_TmpG75,_TmpG75,_TmpG75 + 8U},{_TmpG76,_TmpG76,_TmpG76 + 8U},{_TmpG77,_TmpG77,_TmpG77 + 4U},{_TmpG78,_TmpG78,_TmpG78 + 6U},{_TmpG79,_TmpG79,_TmpG79 + 4U},{_TmpG7A,_TmpG7A,_TmpG7A + 6U},{_TmpG7B,_TmpG7B,_TmpG7B + 7U},{_TmpG7C,_TmpG7C,_TmpG7C + 9U},{_TmpG7D,_TmpG7D,_TmpG7D + 5U},{_TmpG7E,_TmpG7E,_TmpG7E + 4U},{_TmpG7F,_TmpG7F,_TmpG7F + 5U},{_TmpG80,_TmpG80,_TmpG80 + 9U},{_TmpG81,_TmpG81,_TmpG81 + 9U},{_TmpG82,_TmpG82,_TmpG82 + 6U},{_TmpG83,_TmpG83,_TmpG83 + 10U},{_TmpG84,_TmpG84,_TmpG84 + 9U},{_TmpG85,_TmpG85,_TmpG85 + 7U},{_TmpG86,_TmpG86,_TmpG86 + 8U},{_TmpG87,_TmpG87,_TmpG87 + 9U},{_TmpG88,_TmpG88,_TmpG88 + 15U},{_TmpG89,_TmpG89,_TmpG89 + 8U},{_TmpG8A,_TmpG8A,_TmpG8A + 7U},{_TmpG8B,_TmpG8B,_TmpG8B + 8U},{_TmpG8C,_TmpG8C,_TmpG8C + 8U},{_TmpG8D,_TmpG8D,_TmpG8D + 5U},{_TmpG8E,_TmpG8E,_TmpG8E + 9U},{_TmpG8F,_TmpG8F,_TmpG8F + 6U},{_TmpG90,_TmpG90,_TmpG90 + 7U},{_TmpG91,_TmpG91,_TmpG91 + 5U},{_TmpG92,_TmpG92,_TmpG92 + 8U},{_TmpG93,_TmpG93,_TmpG93 + 7U},{_TmpG94,_TmpG94,_TmpG94 + 8U},{_TmpG95,_TmpG95,_TmpG95 + 7U},{_TmpG96,_TmpG96,_TmpG96 + 10U},{_TmpG97,_TmpG97,_TmpG97 + 11U},{_TmpG98,_TmpG98,_TmpG98 + 13U},{_TmpG99,_TmpG99,_TmpG99 + 10U},{_TmpG9A,_TmpG9A,_TmpG9A + 14U},{_TmpG9B,_TmpG9B,_TmpG9B + 10U},{_TmpG9C,_TmpG9C,_TmpG9C + 8U},{_TmpG9D,_TmpG9D,_TmpG9D + 8U},{_TmpG9E,_TmpG9E,_TmpG9E + 8U},{_TmpG9F,_TmpG9F,_TmpG9F + 10U},{_TmpGA0,_TmpGA0,_TmpGA0 + 9U},{_TmpGA1,_TmpGA1,_TmpGA1 + 13U},{_TmpGA2,_TmpGA2,_TmpGA2 + 10U},{_TmpGA3,_TmpGA3,_TmpGA3 + 9U},{_TmpGA4,_TmpGA4,_TmpGA4 + 13U},{_TmpGA5,_TmpGA5,_TmpGA5 + 14U},{_TmpGA6,_TmpGA6,_TmpGA6 + 14U},{_TmpGA7,_TmpGA7,_TmpGA7 + 13U},{_TmpGA8,_TmpGA8,_TmpGA8 + 12U},{_TmpGA9,_TmpGA9,_TmpGA9 + 12U},{_TmpGAA,_TmpGAA,_TmpGAA + 16U},{_TmpGAB,_TmpGAB,_TmpGAB + 14U},{_TmpGAC,_TmpGAC,_TmpGAC + 12U},{_TmpGAD,_TmpGAD,_TmpGAD + 12U},{_TmpGAE,_TmpGAE,_TmpGAE + 18U},{_TmpGAF,_TmpGAF,_TmpGAF + 11U},{_TmpGB0,_TmpGB0,_TmpGB0 + 7U},{_TmpGB1,_TmpGB1,_TmpGB1 + 16U},{_TmpGB2,_TmpGB2,_TmpGB2 + 18U},{_TmpGB3,_TmpGB3,_TmpGB3 + 7U},{_TmpGB4,_TmpGB4,_TmpGB4 + 7U},{_TmpGB5,_TmpGB5,_TmpGB5 + 7U},{_TmpGB6,_TmpGB6,_TmpGB6 + 8U},{_TmpGB7,_TmpGB7,_TmpGB7 + 9U},{_TmpGB8,_TmpGB8,_TmpGB8 + 6U},{_TmpGB9,_TmpGB9,_TmpGB9 + 6U},{_TmpGBA,_TmpGBA,_TmpGBA + 6U},{_TmpGBB,_TmpGBB,_TmpGBB + 6U},{_TmpGBC,_TmpGBC,_TmpGBC + 7U},{_TmpGBD,_TmpGBD,_TmpGBD + 6U},{_TmpGBE,_TmpGBE,_TmpGBE + 11U},{_TmpGBF,_TmpGBF,_TmpGBF + 11U},{_TmpGC0,_TmpGC0,_TmpGC0 + 11U},{_TmpGC1,_TmpGC1,_TmpGC1 + 11U},{_TmpGC2,_TmpGC2,_TmpGC2 + 11U},{_TmpGC3,_TmpGC3,_TmpGC3 + 12U},{_TmpGC4,_TmpGC4,_TmpGC4 + 13U},{_TmpGC5,_TmpGC5,_TmpGC5 + 11U},{_TmpGC6,_TmpGC6,_TmpGC6 + 11U},{_TmpGC7,_TmpGC7,_TmpGC7 + 10U},{_TmpGC8,_TmpGC8,_TmpGC8 + 9U},{_TmpGC9,_TmpGC9,_TmpGC9 + 11U},{_TmpGCA,_TmpGCA,_TmpGCA + 12U},{_TmpGCB,_TmpGCB,_TmpGCB + 11U},{_TmpGCC,_TmpGCC,_TmpGCC + 17U},{_TmpGCD,_TmpGCD,_TmpGCD + 7U},{_TmpGCE,_TmpGCE,_TmpGCE + 8U},{_TmpGCF,_TmpGCF,_TmpGCF + 19U},{_TmpGD0,_TmpGD0,_TmpGD0 + 20U},{_TmpGD1,_TmpGD1,_TmpGD1 + 18U},{_TmpGD2,_TmpGD2,_TmpGD2 + 9U},{_TmpGD3,_TmpGD3,_TmpGD3 + 13U},{_TmpGD4,_TmpGD4,_TmpGD4 + 16U},{_TmpGD5,_TmpGD5,_TmpGD5 + 18U},{_TmpGD6,_TmpGD6,_TmpGD6 + 18U},{_TmpGD7,_TmpGD7,_TmpGD7 + 10U},{_TmpGD8,_TmpGD8,_TmpGD8 + 8U},{_TmpGD9,_TmpGD9,_TmpGD9 + 4U},{_TmpGDA,_TmpGDA,_TmpGDA + 4U},{_TmpGDB,_TmpGDB,_TmpGDB + 4U},{_TmpGDC,_TmpGDC,_TmpGDC + 4U},{_TmpGDD,_TmpGDD,_TmpGDD + 4U},{_TmpGDE,_TmpGDE,_TmpGDE + 4U},{_TmpGDF,_TmpGDF,_TmpGDF + 4U},{_TmpGE0,_TmpGE0,_TmpGE0 + 4U},{_TmpGE1,_TmpGE1,_TmpGE1 + 4U},{_TmpGE2,_TmpGE2,_TmpGE2 + 4U},{_TmpGE3,_TmpGE3,_TmpGE3 + 4U},{_TmpGE4,_TmpGE4,_TmpGE4 + 4U},{_TmpGE5,_TmpGE5,_TmpGE5 + 4U},{_TmpGE6,_TmpGE6,_TmpGE6 + 4U},{_TmpGE7,_TmpGE7,_TmpGE7 + 4U},{_TmpGE8,_TmpGE8,_TmpGE8 + 4U},{_TmpGE9,_TmpGE9,_TmpGE9 + 4U},{_TmpGEA,_TmpGEA,_TmpGEA + 4U},{_TmpGEB,_TmpGEB,_TmpGEB + 4U},{_TmpGEC,_TmpGEC,_TmpGEC + 4U},{_TmpGED,_TmpGED,_TmpGED + 4U},{_TmpGEE,_TmpGEE,_TmpGEE + 4U},{_TmpGEF,_TmpGEF,_TmpGEF + 4U},{_TmpGF0,_TmpGF0,_TmpGF0 + 4U},{_TmpGF1,_TmpGF1,_TmpGF1 + 4U},{_TmpGF2,_TmpGF2,_TmpGF2 + 4U},{_TmpGF3,_TmpGF3,_TmpGF3 + 4U},{_TmpGF4,_TmpGF4,_TmpGF4 + 5U},{_TmpGF5,_TmpGF5,_TmpGF5 + 17U},{_TmpGF6,_TmpGF6,_TmpGF6 + 18U},{_TmpGF7,_TmpGF7,_TmpGF7 + 19U},{_TmpGF8,_TmpGF8,_TmpGF8 + 16U},{_TmpGF9,_TmpGF9,_TmpGF9 + 13U},{_TmpGFA,_TmpGFA,_TmpGFA + 14U},{_TmpGFB,_TmpGFB,_TmpGFB + 17U},{_TmpGFC,_TmpGFC,_TmpGFC + 16U},{_TmpGFD,_TmpGFD,_TmpGFD + 12U},{_TmpGFE,_TmpGFE,_TmpGFE + 19U},{_TmpGFF,_TmpGFF,_TmpGFF + 13U},{_TmpG100,_TmpG100,_TmpG100 + 21U},{_TmpG101,_TmpG101,_TmpG101 + 14U},{_TmpG102,_TmpG102,_TmpG102 + 20U},{_TmpG103,_TmpG103,_TmpG103 + 21U},{_TmpG104,_TmpG104,_TmpG104 + 13U},{_TmpG105,_TmpG105,_TmpG105 + 15U},{_TmpG106,_TmpG106,_TmpG106 + 17U},{_TmpG107,_TmpG107,_TmpG107 + 19U},{_TmpG108,_TmpG108,_TmpG108 + 12U},{_TmpG109,_TmpG109,_TmpG109 + 9U},{_TmpG10A,_TmpG10A,_TmpG10A + 17U},{_TmpG10B,_TmpG10B,_TmpG10B + 23U},{_TmpG10C,_TmpG10C,_TmpG10C + 24U},{_TmpG10D,_TmpG10D,_TmpG10D + 15U},{_TmpG10E,_TmpG10E,_TmpG10E + 11U},{_TmpG10F,_TmpG10F,_TmpG10F + 15U},{_TmpG110,_TmpG110,_TmpG110 + 10U},{_TmpG111,_TmpG111,_TmpG111 + 15U},{_TmpG112,_TmpG112,_TmpG112 + 25U},{_TmpG113,_TmpG113,_TmpG113 + 5U},{_TmpG114,_TmpG114,_TmpG114 + 15U},{_TmpG115,_TmpG115,_TmpG115 + 15U},{_TmpG116,_TmpG116,_TmpG116 + 11U},{_TmpG117,_TmpG117,_TmpG117 + 22U},{_TmpG118,_TmpG118,_TmpG118 + 26U},{_TmpG119,_TmpG119,_TmpG119 + 26U},{_TmpG11A,_TmpG11A,_TmpG11A + 16U},{_TmpG11B,_TmpG11B,_TmpG11B + 16U},{_TmpG11C,_TmpG11C,_TmpG11C + 24U},{_TmpG11D,_TmpG11D,_TmpG11D + 25U},{_TmpG11E,_TmpG11E,_TmpG11E + 25U},{_TmpG11F,_TmpG11F,_TmpG11F + 16U},{_TmpG120,_TmpG120,_TmpG120 + 19U},{_TmpG121,_TmpG121,_TmpG121 + 25U},{_TmpG122,_TmpG122,_TmpG122 + 35U},{_TmpG123,_TmpG123,_TmpG123 + 27U},{_TmpG124,_TmpG124,_TmpG124 + 18U},{_TmpG125,_TmpG125,_TmpG125 + 20U},{_TmpG126,_TmpG126,_TmpG126 + 19U},{_TmpG127,_TmpG127,_TmpG127 + 14U},{_TmpG128,_TmpG128,_TmpG128 + 19U},{_TmpG129,_TmpG129,_TmpG129 + 20U},{_TmpG12A,_TmpG12A,_TmpG12A + 14U},{_TmpG12B,_TmpG12B,_TmpG12B + 11U},{_TmpG12C,_TmpG12C,_TmpG12C + 23U},{_TmpG12D,_TmpG12D,_TmpG12D + 18U},{_TmpG12E,_TmpG12E,_TmpG12E + 30U},{_TmpG12F,_TmpG12F,_TmpG12F + 8U},{_TmpG130,_TmpG130,_TmpG130 + 12U},{_TmpG131,_TmpG131,_TmpG131 + 14U},{_TmpG132,_TmpG132,_TmpG132 + 13U},{_TmpG133,_TmpG133,_TmpG133 + 16U},{_TmpG134,_TmpG134,_TmpG134 + 23U},{_TmpG135,_TmpG135,_TmpG135 + 14U},{_TmpG136,_TmpG136,_TmpG136 + 18U},{_TmpG137,_TmpG137,_TmpG137 + 8U},{_TmpG138,_TmpG138,_TmpG138 + 11U},{_TmpG139,_TmpG139,_TmpG139 + 20U},{_TmpG13A,_TmpG13A,_TmpG13A + 14U},{_TmpG13B,_TmpG13B,_TmpG13B + 9U},{_TmpG13C,_TmpG13C,_TmpG13C + 16U},{_TmpG13D,_TmpG13D,_TmpG13D + 23U},{_TmpG13E,_TmpG13E,_TmpG13E + 14U},{_TmpG13F,_TmpG13F,_TmpG13F + 14U},{_TmpG140,_TmpG140,_TmpG140 + 13U},{_TmpG141,_TmpG141,_TmpG141 + 12U},{_TmpG142,_TmpG142,_TmpG142 + 15U},{_TmpG143,_TmpG143,_TmpG143 + 14U},{_TmpG144,_TmpG144,_TmpG144 + 16U},{_TmpG145,_TmpG145,_TmpG145 + 11U},{_TmpG146,_TmpG146,_TmpG146 + 14U},{_TmpG147,_TmpG147,_TmpG147 + 11U},{_TmpG148,_TmpG148,_TmpG148 + 15U},{_TmpG149,_TmpG149,_TmpG149 + 22U},{_TmpG14A,_TmpG14A,_TmpG14A + 16U},{_TmpG14B,_TmpG14B,_TmpG14B + 17U},{_TmpG14C,_TmpG14C,_TmpG14C + 12U},{_TmpG14D,_TmpG14D,_TmpG14D + 18U},{_TmpG14E,_TmpG14E,_TmpG14E + 17U},{_TmpG14F,_TmpG14F,_TmpG14F + 12U},{_TmpG150,_TmpG150,_TmpG150 + 16U},{_TmpG151,_TmpG151,_TmpG151 + 11U},{_TmpG152,_TmpG152,_TmpG152 + 10U},{_TmpG153,_TmpG153,_TmpG153 + 14U},{_TmpG154,_TmpG154,_TmpG154 + 15U},{_TmpG155,_TmpG155,_TmpG155 + 20U},{_TmpG156,_TmpG156,_TmpG156 + 27U},{_TmpG157,_TmpG157,_TmpG157 + 32U},{_TmpG158,_TmpG158,_TmpG158 + 36U},{_TmpG159,_TmpG159,_TmpG159 + 10U},{_TmpG15A,_TmpG15A,_TmpG15A + 18U},{_TmpG15B,_TmpG15B,_TmpG15B + 21U},{_TmpG15C,_TmpG15C,_TmpG15C + 19U},{_TmpG15D,_TmpG15D,_TmpG15D + 16U},{_TmpG15E,_TmpG15E,_TmpG15E + 20U},{_TmpG15F,_TmpG15F,_TmpG15F + 15U},{_TmpG160,_TmpG160,_TmpG160 + 20U},{_TmpG161,_TmpG161,_TmpG161 + 12U},{_TmpG162,_TmpG162,_TmpG162 + 15U},{_TmpG163,_TmpG163,_TmpG163 + 12U},{_TmpG164,_TmpG164,_TmpG164 + 20U},{_TmpG165,_TmpG165,_TmpG165 + 19U},{_TmpG166,_TmpG166,_TmpG166 + 20U},{_TmpG167,_TmpG167,_TmpG167 + 21U},{_TmpG168,_TmpG168,_TmpG168 + 21U},{_TmpG169,_TmpG169,_TmpG169 + 12U},{_TmpG16A,_TmpG16A,_TmpG16A + 17U},{_TmpG16B,_TmpG16B,_TmpG16B + 19U},{_TmpG16C,_TmpG16C,_TmpG16C + 14U},{_TmpG16D,_TmpG16D,_TmpG16D + 17U},{_TmpG16E,_TmpG16E,_TmpG16E + 23U},{_TmpG16F,_TmpG16F,_TmpG16F + 13U},{_TmpG170,_TmpG170,_TmpG170 + 14U},{_TmpG171,_TmpG171,_TmpG171 + 16U},{_TmpG172,_TmpG172,_TmpG172 + 16U},{_TmpG173,_TmpG173,_TmpG173 + 8U},{_TmpG174,_TmpG174,_TmpG174 + 19U},{_TmpG175,_TmpG175,_TmpG175 + 20U},{_TmpG176,_TmpG176,_TmpG176 + 14U},{_TmpG177,_TmpG177,_TmpG177 + 19U},{_TmpG178,_TmpG178,_TmpG178 + 20U},{_TmpG179,_TmpG179,_TmpG179 + 11U},{_TmpG17A,_TmpG17A,_TmpG17A + 22U},{_TmpG17B,_TmpG17B,_TmpG17B + 20U},{_TmpG17C,_TmpG17C,_TmpG17C + 23U},{_TmpG17D,_TmpG17D,_TmpG17D + 20U},{_TmpG17E,_TmpG17E,_TmpG17E + 22U},{_TmpG17F,_TmpG17F,_TmpG17F + 23U},{_TmpG180,_TmpG180,_TmpG180 + 24U},{_TmpG181,_TmpG181,_TmpG181 + 24U},{_TmpG182,_TmpG182,_TmpG182 + 15U},{_TmpG183,_TmpG183,_TmpG183 + 20U},{_TmpG184,_TmpG184,_TmpG184 + 22U},{_TmpG185,_TmpG185,_TmpG185 + 17U},{_TmpG186,_TmpG186,_TmpG186 + 20U},{_TmpG187,_TmpG187,_TmpG187 + 26U},{_TmpG188,_TmpG188,_TmpG188 + 12U},{_TmpG189,_TmpG189,_TmpG189 + 14U},{_TmpG18A,_TmpG18A,_TmpG18A + 12U},{_TmpG18B,_TmpG18B,_TmpG18B + 18U},{_TmpG18C,_TmpG18C,_TmpG18C + 16U},{_TmpG18D,_TmpG18D,_TmpG18D + 17U},{_TmpG18E,_TmpG18E,_TmpG18E + 15U},{_TmpG18F,_TmpG18F,_TmpG18F + 9U},{_TmpG190,_TmpG190,_TmpG190 + 13U},{_TmpG191,_TmpG191,_TmpG191 + 12U},{_TmpG192,_TmpG192,_TmpG192 + 12U},{_TmpG193,_TmpG193,_TmpG193 + 11U},{_TmpG194,_TmpG194,_TmpG194 + 11U},{_TmpG195,_TmpG195,_TmpG195 + 11U},{_TmpG196,_TmpG196,_TmpG196 + 16U},{_TmpG197,_TmpG197,_TmpG197 + 17U},{_TmpG198,_TmpG198,_TmpG198 + 19U},{_TmpG199,_TmpG199,_TmpG199 + 17U},{_TmpG19A,_TmpG19A,_TmpG19A + 19U},{_TmpG19B,_TmpG19B,_TmpG19B + 25U},{_TmpG19C,_TmpG19C,_TmpG19C + 26U},{_TmpG19D,_TmpG19D,_TmpG19D + 9U},{_TmpG19E,_TmpG19E,_TmpG19E + 20U},{_TmpG19F,_TmpG19F,_TmpG19F + 17U},{_TmpG1A0,_TmpG1A0,_TmpG1A0 + 18U},{_TmpG1A1,_TmpG1A1,_TmpG1A1 + 11U},{_TmpG1A2,_TmpG1A2,_TmpG1A2 + 12U}};
# 1688
static short Cyc_yyr1[583U]={0,168,169,169,169,169,169,169,169,169,169,169,169,170,171,172,173,174,174,175,175,175,176,176,177,177,177,178,178,179,179,180,180,180,181,181,182,182,182,182,183,183,184,185,186,187,188,188,188,188,188,188,189,189,190,190,191,191,191,191,191,191,191,191,191,191,191,192,192,192,192,192,192,192,193,193,194,195,195,196,196,196,196,197,197,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,198,199,200,200,200,201,201,201,202,202,203,203,203,204,204,204,205,205,206,206,207,207,208,208,209,209,210,210,211,211,212,213,213,213,213,213,213,214,214,214,214,214,214,215,215,216,216,216,216,217,217,218,218,218,219,219,220,220,220,220,221,221,221,222,222,223,223,224,224,225,225,225,225,225,225,225,225,225,226,226,226,226,226,226,226,226,226,226,227,227,228,229,229,230,230,230,230,230,230,230,230,230,230,230,231,231,231,- 1,- 1,232,232,232,233,233,234,234,234,235,235,235,236,236,237,237,237,237,237,238,238,239,239,240,240,241,241,242,242,242,242,243,244,245,245,245,245,245,246,246,247,247,248,248,249,249,250,250,250,250,251,251,252,252,253,253,253,254,255,255,256,256,257,257,257,257,257,258,258,258,258,259,259,260,260,261,261,262,262,263,263,263,263,263,263,264,264,265,265,265,266,266,266,266,266,266,266,266,266,267,267,267,267,267,267,268,268,269,269,269,269,269,269,270,271,271,272,272,273,273,273,273,273,273,273,273,274,274,274,274,274,274,275,275,275,275,275,275,276,276,276,276,277,277,278,278,278,278,278,278,278,278,279,280,280,281,281,282,282,283,283,284,284,285,285,286,286,287,287,288,288,288,289,289,290,290,291,291,292,292,292,292,292,293,294,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,295,296,296,296,297,297,298,298,299,299,299,300,300,301,301,302,302,302,303,303,303,303,303,303,303,303,303,303,303,304,304,304,304,304,304,304,304,304,305,306,306,307,307,308,308,309,309,310,310,311,311,312,312,313,313,313,314,314,315,315,316,316,317,317,317,317,318,318,319,319,319,320,320,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,322,322,322,322,323,324,324,325,325,325,326,326,327,327,327,328,328,329,330,330,330,331,331,332,332,332,332,332,332,332,332,332,332,332,333,333,333,333,334,334,334,334,334,334,334,334,334,334,334,335,336,336,337,337,337,337,337,338,338,339,339,340,340,341,341,342,342};
# 1750
static short Cyc_yyr2[583U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,1,4,1,1,1,4,0,1,2,2,1,0,3,0,1,1,0,1,1,0,2,2,3,5,5,7,0,1,2,4,0,2,0,2,1,1,3,3,3,3,1,1,1,1,1,1,4,1,4,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,1,3,1,1,2,3,3,4,4,5,4,5,4,2,4,4,4,5,5,5,0,1,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,8,6,6,6,9,13,11,4,6,6,4,2,2,4,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1812
static short Cyc_yydefact[1150U]={0,33,67,68,69,70,72,85,86,87,88,89,90,91,92,93,111,112,113,127,128,63,0,0,98,0,94,0,73,0,0,163,105,108,0,0,0,13,14,0,0,0,0,573,236,575,574,576,0,221,0,101,0,221,220,1,0,0,0,0,31,0,0,32,0,56,65,59,83,61,95,96,0,126,99,0,0,174,0,197,200,100,178,129,71,70,64,0,115,0,58,571,0,573,568,569,570,572,0,129,0,0,402,0,0,0,274,0,406,404,42,44,0,0,52,0,0,0,0,0,125,164,0,237,238,0,0,218,0,0,0,0,219,0,0,0,2,0,0,0,0,46,0,135,137,57,66,60,62,577,578,129,131,129,0,54,0,0,35,0,240,0,186,175,198,0,204,205,209,210,0,208,207,0,206,212,226,200,0,84,71,119,0,117,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,557,558,521,0,0,0,519,0,518,516,517,0,431,433,447,457,459,461,463,465,467,469,471,474,476,489,0,491,539,556,554,573,414,0,0,0,0,415,0,0,405,49,0,0,129,0,0,250,252,253,251,0,254,0,144,140,142,294,255,301,296,0,0,0,0,0,8,9,0,0,214,213,0,579,580,236,110,0,0,0,179,102,272,0,269,10,11,0,3,0,5,0,47,0,0,0,35,124,0,132,133,156,0,161,0,0,0,0,0,0,0,0,0,0,0,0,573,331,333,0,341,335,0,339,324,325,326,0,327,328,329,0,55,35,137,34,36,302,0,259,275,0,0,242,240,0,223,0,0,0,0,228,74,227,201,0,120,116,0,0,0,498,0,0,513,449,489,0,450,451,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,492,493,522,512,0,495,0,0,0,0,494,0,97,0,0,0,0,0,0,478,479,0,482,483,480,481,0,0,0,484,485,0,486,487,488,0,0,437,438,439,440,441,442,443,444,445,446,436,0,496,0,545,546,0,0,0,560,0,129,407,0,0,0,428,573,580,0,0,0,0,290,424,429,0,426,0,0,403,0,276,0,0,421,422,0,419,0,0,297,0,267,145,150,146,148,141,143,240,0,304,295,305,582,581,0,104,107,0,51,0,0,106,109,239,80,79,0,77,222,180,240,271,176,304,273,187,188,0,103,16,29,43,0,45,0,136,138,278,277,35,37,129,122,134,0,0,0,152,159,0,129,169,0,0,0,0,0,573,0,359,0,362,363,364,0,0,366,0,0,0,342,336,137,340,334,332,38,0,185,260,0,0,0,266,241,261,322,0,231,0,242,184,225,224,181,223,0,0,0,229,75,130,121,456,118,114,0,0,0,0,573,279,284,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,559,566,0,565,432,458,0,460,462,464,466,468,470,472,473,475,477,435,434,544,541,0,543,0,0,0,417,418,0,293,0,425,288,291,413,0,289,416,410,48,0,411,0,0,0,298,0,151,147,149,0,0,223,0,306,0,240,0,315,300,0,0,0,0,0,0,144,0,129,0,240,0,196,177,270,0,22,4,6,39,242,0,155,139,156,0,0,154,162,171,170,0,0,165,0,0,0,349,0,0,0,359,0,360,361,365,0,0,0,337,330,0,40,303,240,0,263,0,0,0,0,0,323,183,243,244,245,0,0,259,232,182,202,203,211,229,199,497,0,0,0,280,0,285,0,500,0,0,0,0,0,0,0,0,555,508,511,0,0,514,515,523,0,0,490,563,0,0,542,540,0,0,0,0,292,427,430,412,420,423,299,256,268,322,307,308,223,0,0,223,0,0,50,215,573,0,76,78,0,189,0,0,0,223,0,0,0,17,23,131,153,0,157,160,172,169,169,0,0,0,0,0,0,0,0,0,359,349,367,0,338,41,242,0,264,262,0,0,0,0,0,0,0,242,0,230,551,0,550,0,281,286,0,452,453,432,432,432,0,0,0,0,454,455,544,543,528,0,564,547,0,567,448,561,562,0,408,409,312,310,314,322,309,223,53,0,81,190,195,322,194,191,223,0,0,0,0,0,0,0,168,167,343,349,0,0,0,0,0,369,370,372,374,376,378,380,382,384,386,389,391,393,395,400,401,0,0,346,355,0,359,0,0,368,234,265,0,0,0,246,247,0,257,249,248,233,240,499,0,0,287,502,503,504,0,0,0,0,510,509,0,534,528,524,526,520,548,0,313,311,0,193,192,30,25,0,0,35,0,7,123,158,0,0,0,349,0,398,0,0,349,0,0,0,0,0,0,0,0,0,0,0,0,0,396,349,0,0,359,348,316,317,318,0,242,553,552,0,0,0,0,0,0,535,534,531,529,0,525,549,0,26,24,0,27,0,19,173,344,345,0,0,0,0,349,351,373,0,375,377,379,381,383,385,387,388,390,392,0,350,356,0,0,319,320,321,0,235,0,0,501,0,0,0,0,537,536,0,530,527,0,28,18,20,0,347,397,0,394,352,0,349,358,0,258,283,282,505,0,0,533,0,532,82,21,0,371,349,353,357,0,0,538,399,354,0,507,0,506,0,0,0};
# 1930
static short Cyc_yydefgoto[175U]={1147,55,56,57,58,524,924,1063,835,836,1004,704,59,344,60,328,61,526,62,528,63,272,156,64,65,593,262,510,511,263,68,283,264,70,181,182,71,72,73,179,306,307,142,143,308,265,489,540,541,715,74,75,719,720,721,76,542,77,516,78,79,176,177,277,80,127,588,360,764,678,124,81,351,582,752,753,754,266,267,971,574,578,756,483,352,292,105,106,606,531,607,462,463,464,268,345,346,679,495,750,751,331,332,333,334,335,336,850,337,730,338,936,937,938,939,940,941,942,943,944,945,946,947,948,949,950,951,465,478,479,466,467,468,339,220,442,221,598,222,223,224,225,226,227,228,229,230,231,415,420,425,429,232,233,234,397,398,895,990,991,1052,992,1054,1104,235,875,236,630,631,237,238,83,1005,469,499};
# 1951
static short Cyc_yypact[1150U]={3208,- -32768,- -32768,- -32768,- -32768,- 86,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4032,309,- 76,- -32768,4032,- -32768,1694,- -32768,146,28,- -32768,- 56,- 28,- 75,100,102,- -32768,- -32768,56,74,288,205,- -32768,- 44,- -32768,- -32768,- -32768,126,145,538,164,143,145,- -32768,- -32768,198,253,296,3049,- -32768,241,405,- -32768,254,4032,4032,4032,- -32768,4032,- -32768,- -32768,365,310,- -32768,146,3926,210,189,157,799,- -32768,- -32768,312,324,369,- -32768,146,464,6877,- -32768,- -32768,1859,282,- -32768,- -32768,- -32768,- -32768,486,312,474,6877,- -32768,479,1859,530,500,527,- -32768,76,- -32768,- -32768,4135,4135,540,573,3049,3049,341,6877,- -32768,- -32768,278,- -32768,- -32768,539,6877,- -32768,162,552,278,4882,- -32768,3049,3049,3366,- -32768,3049,3366,3049,3366,- -32768,80,- -32768,3776,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,312,4882,283,1901,- -32768,3926,254,578,4135,4241,5853,- -32768,210,- -32768,589,- -32768,- -32768,- -32768,- -32768,606,- -32768,- -32768,622,- -32768,- -32768,13,799,4135,- -32768,- -32768,570,584,623,146,7171,632,7224,6877,1482,634,639,642,646,651,655,660,662,668,670,673,680,685,688,691,695,7224,7224,- -32768,- -32768,767,7337,2721,711,- -32768,7337,- -32768,- -32768,- -32768,127,- -32768,- -32768,- 62,765,719,722,743,541,108,736,412,188,- -32768,847,7337,110,- 46,- -32768,768,- 37,- -32768,1859,27,781,1523,789,330,1523,- -32768,- -32768,790,6877,312,1094,769,- -32768,- -32768,- -32768,- -32768,771,- -32768,4531,4882,4985,4882,568,- -32768,- -32768,- -32768,- 36,- 36,794,787,774,- -32768,- -32768,776,38,- -32768,- -32768,367,- -32768,- -32768,791,- -32768,53,788,782,- -32768,- -32768,313,374,- -32768,- -32768,- -32768,795,- -32768,797,- -32768,801,- -32768,162,5981,3926,578,800,802,4882,- -32768,1179,146,804,806,196,822,5070,824,821,792,808,6109,2561,5070,266,823,- -32768,- -32768,825,2066,2066,254,2066,- -32768,- -32768,- -32768,807,- -32768,- -32768,- -32768,340,- -32768,578,829,- -32768,- -32768,818,98,826,- -32768,62,828,827,419,831,752,832,6877,4135,341,- -32768,840,- -32768,- -32768,98,146,- -32768,6877,839,2721,- -32768,4882,2721,- -32768,- -32768,- -32768,5198,- -32768,870,6877,6877,6877,6877,6877,6877,6877,6877,6877,6877,857,6877,4882,239,6877,6877,- -32768,- -32768,- -32768,- -32768,836,- -32768,2066,845,379,6877,- -32768,6877,- -32768,7337,6877,7337,7337,7337,7337,- -32768,- -32768,7337,- -32768,- -32768,- -32768,- -32768,7337,7337,7337,- -32768,- -32768,7337,- -32768,- -32768,- -32768,7337,6877,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6877,- -32768,278,- -32768,- -32768,6237,278,6877,- -32768,849,312,- -32768,844,850,851,- -32768,147,486,278,6877,1859,230,- -32768,- -32768,- -32768,858,856,853,1859,- -32768,852,- -32768,615,1523,- -32768,- -32768,854,863,4135,4135,- -32768,865,866,- -32768,4985,4985,4985,- -32768,- -32768,1263,6365,336,- -32768,511,- -32768,- -32768,62,- -32768,- -32768,860,- -32768,884,4135,- -32768,- -32768,- -32768,- -32768,864,862,872,- -32768,- -32768,3469,- -32768,561,453,- -32768,- -32768,- -32768,4882,- -32768,- -32768,971,- -32768,3049,- -32768,3049,- -32768,- -32768,- -32768,- -32768,578,- -32768,312,- -32768,- -32768,1212,6877,619,- -32768,- 10,320,312,848,6877,6877,873,879,6877,871,995,2396,885,- -32768,- -32768,- -32768,620,983,- -32768,6493,2231,2881,- -32768,- -32768,3776,- -32768,- -32768,- -32768,- -32768,4135,- -32768,- -32768,4882,881,4634,- -32768,- -32768,874,707,674,- -32768,4737,827,- -32768,- -32768,- -32768,- -32768,752,882,163,886,730,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,887,891,888,912,892,- -32768,- -32768,713,5981,400,890,900,902,904,905,906,908,909,447,911,913,914,430,915,917,925,926,7058,- -32768,- -32768,918,931,- -32768,765,202,719,722,743,541,108,736,412,412,188,- -32768,- -32768,- -32768,- -32768,- -32768,927,- -32768,- 55,4135,5722,4882,- -32768,4882,- -32768,899,- -32768,- -32768,- -32768,- -32768,3426,- -32768,- -32768,- -32768,- -32768,935,- -32768,3029,228,251,- -32768,4882,- -32768,- -32768,- -32768,932,933,752,928,511,4135,4386,6621,- -32768,- -32768,6877,940,275,7005,937,53,3620,938,312,4135,4241,6749,- -32768,561,- -32768,943,1009,- -32768,- -32768,- -32768,827,657,- -32768,- -32768,1179,942,6877,- -32768,- -32768,- -32768,- -32768,946,146,621,454,459,6877,842,503,5070,944,6877,953,951,- -32768,- -32768,954,956,948,2066,- -32768,3926,- -32768,818,958,4135,- -32768,963,62,952,960,961,- -32768,- -32768,- -32768,968,972,998,973,95,- -32768,- -32768,- -32768,- -32768,- -32768,730,- -32768,978,529,978,976,- -32768,5329,- -32768,1482,- -32768,6877,6877,6877,6877,1099,6877,6877,1482,- -32768,- -32768,- -32768,278,278,- -32768,- -32768,974,975,5460,- -32768,- -32768,6877,6877,- -32768,- -32768,98,729,999,1001,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,707,- -32768,- -32768,752,98,980,752,977,565,- -32768,- -32768,987,982,- -32768,- -32768,985,- -32768,98,988,989,752,981,3366,1000,1092,- -32768,4882,- -32768,6877,- -32768,- -32768,994,36,848,5070,1003,996,1032,1002,1005,5070,6877,1008,6877,842,- -32768,1010,- -32768,- -32768,827,323,- -32768,- -32768,6877,6877,6877,674,674,- 26,1019,827,4882,- -32768,- -32768,522,- -32768,6877,- -32768,- -32768,5981,- -32768,870,1006,1007,1011,1014,1013,1022,1028,- -32768,870,1023,1025,82,1026,- -32768,- -32768,731,- -32768,- -32768,- -32768,- -32768,5722,- -32768,- -32768,- -32768,- -32768,- -32768,707,- -32768,752,- -32768,1050,- -32768,- -32768,- -32768,707,- -32768,- -32768,752,1037,554,1040,3049,1042,1038,4882,- -32768,- -32768,1159,842,1048,7390,1043,2881,1049,- -32768,- 51,- -32768,1084,1041,735,541,108,784,412,188,- -32768,- -32768,- -32768,- -32768,1091,7337,2066,- -32768,- -32768,585,6877,1063,1062,- -32768,- -32768,- -32768,1057,1058,1059,- -32768,- -32768,1064,- -32768,- -32768,- -32768,- -32768,958,- -32768,535,236,- -32768,- -32768,- -32768,- -32768,6877,4882,6877,1186,- -32768,- -32768,1065,125,386,- -32768,- -32768,- -32768,- -32768,5591,- -32768,- -32768,1071,- -32768,- -32768,- -32768,- -32768,1073,1074,636,365,- -32768,- -32768,- -32768,590,5070,1076,842,2721,- -32768,4882,1070,1736,7337,6877,7337,7337,7337,7337,7337,7337,7337,7337,7337,7337,6877,- -32768,842,1082,1078,6877,- -32768,707,707,707,4135,827,- -32768,- -32768,2881,1081,1083,1080,1085,6877,1106,387,- -32768,- -32768,1108,- -32768,- -32768,1110,- -32768,- -32768,365,- -32768,1097,638,- -32768,- -32768,- -32768,1098,1102,1100,7337,842,- -32768,765,423,719,722,722,541,108,736,412,412,188,- -32768,443,- -32768,- -32768,5070,1104,- -32768,- -32768,- -32768,424,- -32768,1112,759,- -32768,1107,1222,4882,592,- -32768,1105,1108,- -32768,- -32768,1109,- -32768,- -32768,- -32768,365,- -32768,978,529,- -32768,- -32768,6877,1736,- -32768,5070,- -32768,- -32768,- -32768,- -32768,1111,1113,- -32768,1129,- -32768,- -32768,- -32768,591,- -32768,842,- -32768,- -32768,4882,1114,- -32768,- -32768,- -32768,1115,- -32768,1116,- -32768,1261,1267,- -32768};
# 2069
static short Cyc_yypgoto[175U]={- -32768,- 13,- -32768,- -32768,- -32768,- -32768,- -32768,156,- -32768,- -32768,208,- -32768,- -32768,- 293,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 59,- -32768,- 112,132,- -32768,- -32768,0,577,- -32768,93,- 182,1155,33,- -32768,- -32768,- 144,- -32768,- -32768,1245,140,450,- -32768,- -32768,990,986,679,337,- -32768,576,- -32768,- -32768,- -32768,63,- -32768,- -32768,101,- 204,1211,- 456,34,- -32768,1117,- -32768,934,- -32768,1237,- 534,- -32768,533,- 142,1015,- 92,- 349,- 520,46,- -32768,- -32768,- 105,- 550,- -32768,542,551,- 304,- 532,- 124,- 468,- 130,- -32768,- 273,- 154,- 606,- 326,- -32768,846,- 178,- 107,- 140,- 202,- 233,- 212,- 733,- 300,- -32768,- -32768,- 8,- 200,- -32768,- 163,- -32768,- 673,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,175,- -32768,- -32768,640,- 223,- -32768,481,687,- -32768,- 160,- 356,- 132,- 399,- 382,- 391,894,- 402,- 401,- 397,- 385,- 416,364,368,362,366,- 195,693,463,- -32768,- -32768,- -32768,- -32768,327,- -32768,- 914,269,- -32768,936,203,- -32768,- 387,- -32768,103,477,- 71,- 70,- 118,- 95};
# 2094
static short Cyc_yytable[7558U]={66,150,151,584,284,269,270,291,633,643,638,534,284,279,639,552,399,155,350,636,404,66,559,640,472,66,278,635,373,530,353,755,303,69,401,376,641,642,363,443,367,717,718,84,745,579,136,799,570,608,128,407,114,702,69,759,853,377,69,66,649,701,1019,494,758,66,66,66,158,66,508,496,115,89,969,1053,66,162,906,450,713,487,122,484,361,155,515,701,518,405,241,112,69,67,123,329,408,340,69,69,69,797,69,274,275,658,742,1020,970,69,242,451,497,164,67,702,66,66,67,113,293,294,295,498,296,297,298,299,564,565,108,567,575,66,66,66,304,66,66,66,66,1107,810,714,66,496,813,44,341,69,69,129,67,86,66,111,66,90,67,67,67,454,67,162,359,144,69,69,69,67,69,69,69,69,736,500,996,69,455,- 166,509,959,505,710,999,898,497,69,837,69,600,1130,601,602,687,108,44,145,146,147,626,148,107,118,576,496,597,108,157,67,67,988,621,444,445,446,416,417,252,520,596,300,573,119,301,253,67,67,67,287,67,67,67,67,644,989,349,67,520,244,707,116,571,117,340,- 240,497,67,- 240,67,591,572,668,279,988,418,419,577,342,447,682,121,738,486,448,449,278,240,595,329,329,405,329,43,241,125,157,406,1051,248,907,91,46,910,682,1035,330,126,157,43,130,305,131,311,242,533,488,919,46,493,- 579,597,49,66,487,487,487,49,19,20,695,50,200,761,53,54,43,755,755,53,54,66,498,43,517,46,647,608,66,66,650,66,46,426,695,771,69,847,50,133,962,329,657,543,108,547,405,108,548,48,108,973,427,428,69,795,108,159,840,160,594,69,69,1090,69,161,43,94,208,209,95,96,97,671,672,46,660,997,808,597,405,43,137,138,460,461,1000,498,371,1045,46,213,475,860,134,141,67,690,484,49,66,809,342,50,974,281,577,560,871,241,498,53,54,282,67,561,453,255,256,257,258,67,67,- 275,67,821,- 275,738,477,242,178,566,792,69,498,157,43,310,135,291,43,276,579,696,880,46,520,43,45,46,47,87,152,90,739,45,46,47,49,178,330,330,514,330,741,608,- 15,520,492,538,53,54,520,963,405,44,800,82,801,260,471,569,498,926,405,491,486,486,486,577,826,492,43,67,729,686,484,879,180,88,45,46,47,737,109,826,110,155,964,965,966,405,798,705,694,706,700,506,521,488,488,488,1095,405,522,493,66,82,66,628,1055,1105,330,785,445,446,82,858,709,989,1051,82,816,815,405,930,139,140,517,149,772,956,153,66,597,82,829,828,740,69,349,69,66,583,183,108,66,405,830,109,219,423,543,108,1122,424,1118,880,108,447,43,109,246,498,786,449,69,405,45,46,47,405,653,82,82,69,1119,781,405,69,280,245,514,405,845,82,184,978,492,846,82,82,82,1084,82,82,82,82,881,67,1074,67,1079,696,1080,1042,763,890,247,1081,879,1077,1078,243,82,900,861,659,520,1076,882,520,1082,1083,250,665,67,405,876,891,48,413,414,851,577,67,281,874,683,67,684,183,281,1043,282,43,685,892,893,880,282,249,975,251,46,708,976,48,329,597,340,43,686,49,716,157,271,50,285,45,46,47,960,402,157,53,54,1002,157,1003,48,700,288,901,597,597,597,273,697,405,698,1066,1062,49,364,912,699,491,109,343,908,109,879,492,109,53,54,365,543,405,109,859,474,916,521,1034,405,66,356,66,1065,1141,1128,543,575,976,543,16,17,18,255,256,257,258,1033,357,667,1017,1032,405,711,733,843,712,405,844,108,519,1012,366,69,358,69,108,259,577,577,343,82,1111,1061,370,1112,378,43,396,519,544,379,1120,549,380,45,46,47,381,763,747,748,749,382,558,49,1010,383,1047,538,82,44,384,290,385,260,286,53,54,576,386,1072,387,921,1137,388,675,676,677,1091,1092,1093,389,67,309,67,66,390,1085,1069,391,1070,290,392,183,421,422,393,807,586,587,355,402,1068,150,402,717,718,1073,769,770,1044,609,403,611,612,613,848,849,69,1096,618,330,1087,157,902,903,994,995,1116,409,369,410,372,374,374,411,165,166,167,168,169,634,1027,1028,170,171,172,329,1023,1024,173,394,395,174,1124,405,374,412,928,929,374,1117,452,1007,430,967,968,456,470,473,480,1135,481,501,503,1127,66,504,67,374,502,123,651,512,513,555,1094,150,1064,175,523,109,525,485,535,490,527,536,545,109,554,556,568,108,109,573,66,546,1136,69,1134,329,1143,431,432,433,434,435,436,437,438,439,440,550,1142,553,563,302,562,498,580,48,581,585,599,407,619,625,309,69,589,532,150,519,654,441,519,627,652,876,655,656,663,662,666,82,669,82,664,670,673,688,674,689,692,691,372,519,693,67,66,703,519,725,724,952,727,728,732,722,723,734,743,726,760,746,731,766,762,765,767,768,773,150,1064,590,774,- 579,775,67,776,777,778,69,779,780,789,802,834,933,934,374,329,782,532,783,784,787,610,788,793,790,614,615,616,617,91,794,620,796,805,623,624,820,811,812,814,833,330,824,827,841,629,839,632,852,854,405,855,856,857,349,374,864,374,374,374,374,862,869,374,865,866,67,867,374,374,374,868,645,374,66,791,870,374,877,887,896,904,894,905,646,909,913,914,911,629,915,91,920,917,918,109,923,922,927,92,931,932,109,955,958,330,961,69,972,374,954,979,980,982,239,94,981,983,95,96,97,984,98,46,819,290,519,985,986,99,987,993,100,998,681,1001,935,1006,102,103,1008,519,1011,1009,519,1013,214,1016,290,104,216,1021,842,217,218,290,1022,1018,1031,1036,1037,92,1038,1039,1040,731,1049,67,1041,1050,1058,1059,1060,476,1067,1071,239,94,1088,1100,95,96,97,1089,98,46,1098,374,1099,1101,1103,99,988,1108,100,1110,1113,290,101,1115,102,103,1126,629,1129,629,330,1114,290,1121,1123,104,1125,1140,1131,1138,1148,290,1139,1144,1145,1146,1149,1132,1109,825,7,8,9,10,11,12,13,14,15,16,17,18,19,20,289,120,925,838,163,132,529,592,537,362,532,873,863,507,872,22,23,24,804,26,637,43,1025,1029,661,82,953,1026,1030,45,46,47,1056,1133,31,374,1106,0,0,49,109,0,622,538,32,33,0,539,957,0,731,53,54,0,43,532,0,0,39,0,0,40,45,46,47,0,48,0,0,0,0,0,49,977,41,0,538,0,290,42,0,0,0,0,53,54,0,818,485,0,0,0,290,823,0,0,0,0,0,0,0,832,347,0,0,0,0,0,0,0,0,0,44,45,149,47,82,48,0,349,0,0,374,49,0,0,629,491,0,51,52,246,0,492,0,53,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,731,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,532,0,0,0,883,884,885,886,374,888,889,0,0,0,0,0,0,374,0,0,0,532,0,0,899,0,149,0,0,0,0,374,0,0,0,0,0,0,402,0,0,0,0,0,1075,0,0,0,0,0,185,186,0,0,0,1086,0,0,0,309,731,0,0,0,0,187,0,91,0,1097,0,0,0,0,1102,374,0,0,0,0,0,149,191,192,193,194,195,196,197,198,0,0,0,0,290,0,0,0,200,0,374,374,374,0,0,0,201,202,91,203,532,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,149,532,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,290,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,375,0,1015,211,0,0,0,212,0,0,213,92,0,0,0,0,0,214,0,0,215,216,374,457,217,218,458,94,0,0,95,96,97,0,459,46,0,0,0,0,0,99,0,0,100,1046,0,1048,101,0,102,103,0,460,461,0,0,0,532,0,0,104,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,374,0,374,374,374,374,374,374,374,374,374,374,0,0,0,0,0,0,0,0,0,0,0,0,91,0,0,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,848,849,21,185,186,312,374,313,314,315,316,317,318,319,320,22,23,24,321,26,91,27,188,322,0,0,0,0,189,190,28,323,0,0,31,191,192,193,194,195,196,197,198,0,32,33,34,199,92,0,0,200,0,374,0,0,0,0,39,201,202,40,203,93,94,0,0,95,96,97,0,98,46,0,41,204,205,0,99,42,0,100,206,207,0,101,0,102,103,0,0,0,0,0,0,0,0,0,104,0,0,0,0,0,0,324,94,208,209,95,96,97,44,45,46,47,0,48,210,325,154,0,0,211,0,0,0,212,0,51,327,0,0,0,0,0,0,214,0,0,215,216,0,91,217,218,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,185,186,312,0,313,314,315,316,317,318,319,320,22,23,24,321,26,91,27,188,322,0,0,0,0,189,190,28,323,0,0,31,191,192,193,194,195,196,197,198,0,32,33,34,199,92,0,0,200,0,0,0,0,0,0,39,201,202,40,203,239,94,0,0,95,96,97,0,98,46,0,41,204,205,0,99,42,0,100,206,207,0,101,0,102,103,0,0,0,0,0,0,0,0,0,104,0,0,0,0,0,0,324,94,208,209,95,96,97,44,45,46,47,0,48,210,325,154,326,0,211,0,0,0,212,0,51,327,0,0,0,0,0,0,214,0,0,215,216,0,0,217,218,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,185,186,312,0,313,314,315,316,317,318,319,320,22,23,24,321,26,91,27,188,322,0,0,0,0,189,190,28,323,0,0,31,191,192,193,194,195,196,197,198,0,32,33,34,199,0,0,0,200,0,0,0,0,0,0,39,201,202,40,203,0,0,0,0,0,0,0,0,0,0,0,41,204,205,0,0,42,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,324,94,208,209,95,96,97,44,45,46,47,0,48,210,325,154,0,0,211,0,0,0,212,0,51,327,0,0,0,0,0,0,214,0,0,215,216,0,0,217,218,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,185,186,312,0,313,314,315,316,317,318,319,320,22,23,24,321,26,91,27,188,322,0,0,0,0,189,190,28,323,0,0,31,191,192,193,194,195,196,197,198,0,32,33,34,199,0,0,0,200,0,0,0,0,0,0,39,201,202,40,203,0,0,0,0,0,0,0,0,0,0,0,41,204,205,0,0,42,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,551,94,208,209,95,96,97,44,45,46,47,0,48,210,325,154,0,0,211,0,0,0,212,0,51,327,0,0,0,0,0,0,214,0,0,215,216,0,0,217,218,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,185,186,0,0,0,0,0,0,0,0,0,0,22,23,24,321,26,91,27,188,0,0,0,0,0,189,190,28,0,0,0,31,191,192,193,194,195,196,197,198,0,32,33,34,199,0,0,0,200,0,0,0,0,0,0,39,201,202,40,203,0,0,0,0,0,0,0,0,0,0,0,41,204,205,0,0,42,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,44,45,46,47,0,48,210,0,0,0,0,211,0,0,0,212,0,51,327,0,0,0,0,0,0,214,0,0,215,216,0,0,217,218,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,185,186,0,0,0,0,0,0,0,0,0,0,22,23,24,321,26,91,0,0,0,0,0,0,0,0,0,28,0,0,0,31,191,192,193,194,195,196,197,198,0,32,33,0,0,0,0,0,200,0,0,0,0,0,0,39,201,202,40,203,0,0,0,0,0,0,0,0,0,0,0,41,204,205,0,0,42,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,44,45,46,47,0,48,210,0,0,0,0,211,0,0,0,371,0,51,327,0,0,0,0,0,0,214,0,0,215,216,0,0,217,218,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,22,23,24,187,26,91,0,188,0,0,0,0,0,189,190,0,0,0,0,31,191,192,193,194,195,196,197,198,0,32,33,0,199,0,0,0,200,0,0,0,0,0,0,39,201,202,40,203,0,0,0,0,0,0,0,0,0,0,0,41,204,205,0,0,42,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,44,45,46,47,0,48,210,0,400,0,0,211,0,0,0,212,0,51,327,0,0,0,0,0,0,214,0,0,215,216,0,0,217,218,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,185,186,0,0,0,0,0,0,0,0,0,0,22,23,24,187,26,91,0,188,0,0,0,0,0,189,190,0,0,0,0,31,191,192,193,194,195,196,197,198,0,32,33,0,199,0,0,0,200,0,0,0,0,0,0,39,201,202,40,203,0,0,0,0,0,0,0,0,0,0,0,41,204,205,0,0,42,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,44,45,46,47,0,48,210,0,0,0,0,211,0,0,0,212,0,51,327,0,0,0,0,0,0,214,0,0,215,216,0,0,217,218,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,91,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,92,0,0,0,41,0,0,0,0,42,0,0,806,0,0,239,94,0,0,95,96,97,0,98,46,0,0,0,0,0,99,0,0,100,0,43,0,101,0,102,103,0,44,45,46,47,0,48,0,0,104,- 12,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,0,0,0,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,91,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,22,23,24,- 12,26,49,0,0,0,50,0,51,52,0,0,0,0,53,54,31,0,0,0,0,0,0,0,0,0,32,33,0,92,0,0,0,0,0,0,0,0,0,0,39,803,0,40,458,94,0,0,95,96,97,0,459,46,0,0,41,0,0,99,0,42,100,0,0,0,101,0,102,103,0,460,461,0,0,0,0,0,0,104,0,0,0,0,347,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,349,0,0,0,49,0,0,0,514,0,51,52,0,0,492,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,0,0,0,0,0,0,44,45,46,47,0,48,0,0,0,0,0,49,0,0,0,538,0,51,52,0,0,0,0,53,54,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,0,154,0,0,0,302,0,0,0,0,51,52,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,34,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,44,45,0,47,0,48,0,0,154,22,23,24,25,26,0,0,0,51,52,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,0,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,254,0,0,0,0,0,255,256,257,258,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,259,42,0,0,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,44,45,0,47,260,48,0,0,261,22,23,24,0,26,0,0,0,51,52,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,347,0,0,348,0,0,0,0,0,0,44,45,0,47,0,48,0,349,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,347,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,349,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,482,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,744,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,757,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,0,0,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,44,45,0,47,0,48,22,23,24,0,26,0,0,0,0,0,0,51,52,0,0,0,0,0,0,31,0,0,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,42,0,0,0,0,0,0,0,0,185,186,312,0,313,314,315,316,317,318,319,320,0,0,0,187,0,91,0,188,322,0,0,0,44,189,190,0,323,48,0,0,191,192,193,194,195,196,197,198,0,51,52,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,551,94,208,209,95,96,97,0,0,46,0,0,0,210,325,154,0,0,211,0,0,0,212,0,0,213,185,186,0,0,0,0,214,603,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,604,94,208,209,95,96,97,0,282,46,0,0,0,210,0,375,605,0,211,0,0,0,212,0,0,213,0,460,461,185,186,0,214,0,0,215,216,0,0,217,218,0,0,0,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,604,94,208,209,95,96,97,0,282,46,0,0,0,210,0,375,878,0,211,0,0,0,212,0,0,213,0,460,461,185,186,0,214,0,0,215,216,0,0,217,218,0,0,0,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,604,94,208,209,95,96,97,0,282,46,0,0,0,210,0,375,897,0,211,0,0,0,212,0,0,213,0,460,461,185,186,0,214,0,0,215,216,0,0,217,218,0,0,0,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,604,94,208,209,95,96,97,0,282,46,0,0,0,210,0,375,1057,0,211,0,0,0,212,0,0,213,0,460,461,185,186,0,214,0,0,215,216,0,0,217,218,0,0,0,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,604,94,208,209,95,96,97,0,282,46,0,0,0,210,0,375,0,0,211,0,0,0,212,0,0,213,0,460,461,185,186,0,214,0,0,215,216,0,0,217,218,0,0,0,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,0,0,0,211,0,0,0,212,0,0,213,185,186,0,354,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,375,0,0,211,0,0,0,212,0,0,213,185,186,0,0,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,557,0,0,0,211,0,0,0,212,0,0,213,185,186,0,0,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,0,0,0,211,0,0,0,212,648,0,213,185,186,0,0,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,0,0,0,211,0,0,0,212,0,0,213,185,186,0,680,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,0,0,0,211,0,0,0,212,735,0,213,185,186,0,0,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,0,0,0,211,0,0,0,212,0,0,213,185,186,0,817,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,0,0,0,211,0,0,0,212,0,0,213,185,186,0,831,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,0,0,0,201,202,0,203,0,0,0,0,0,0,0,0,0,0,0,0,204,205,0,0,0,0,0,206,207,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,0,0,0,211,0,0,0,212,0,0,213,185,186,0,0,0,0,214,0,0,215,216,0,0,217,218,187,0,91,0,188,0,0,0,0,0,189,190,0,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,199,0,0,0,200,0,0,0,0,185,186,0,201,202,0,203,0,0,0,0,0,0,0,0,187,0,91,0,204,205,0,0,0,0,0,206,207,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,0,0,0,0,200,822,94,208,209,95,96,97,201,202,46,203,0,0,210,0,0,0,0,211,0,0,0,212,204,205,213,0,0,0,0,206,207,214,0,0,215,216,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,185,186,210,0,791,0,0,211,0,0,0,212,0,0,213,187,0,91,0,0,0,214,0,0,215,216,0,0,217,218,0,0,191,192,193,194,195,196,197,198,0,0,0,0,0,0,0,0,200,0,0,0,0,185,186,0,201,202,0,203,0,0,0,0,0,0,0,0,187,0,91,0,204,205,0,0,0,0,0,206,207,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,0,0,0,0,200,43,94,208,209,95,96,97,201,202,46,203,0,0,210,0,0,0,0,211,0,0,0,368,204,205,213,0,0,0,0,206,207,214,0,0,215,216,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,185,186,210,0,0,0,0,211,0,0,0,371,0,0,213,187,0,91,0,0,0,214,0,0,215,216,0,0,217,218,0,0,191,192,193,194,195,196,197,198,0,0,0,0,0,0,0,0,200,0,0,0,0,185,186,0,201,202,0,203,0,0,0,0,0,0,0,0,187,0,91,0,204,205,0,0,0,0,0,206,207,0,0,0,0,191,192,193,194,195,196,197,198,0,0,0,0,0,0,0,0,200,43,94,208,209,95,96,97,201,202,46,203,0,0,210,0,0,0,0,211,0,0,0,212,204,205,213,0,0,0,0,206,207,214,0,0,215,216,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,43,94,208,209,95,96,97,0,0,46,0,0,0,210,0,0,0,0,211,0,0,0,1014,0,0,213,0,0,0,0,0,0,214,0,0,215,216,0,0,217,218};
# 2853
static short Cyc_yycheck[7558U]={0,72,72,352,122,112,113,131,407,425,412,304,130,118,415,315,211,76,160,410,215,21,322,420,247,25,118,409,188,302,160,581,144,0,212,189,421,422,178,234,184,5,6,129,576,349,59,653,341,375,50,113,127,521,21,589,729,189,25,59,447,517,113,265,584,65,66,67,76,69,17,107,147,149,100,989,76,77,811,125,90,263,126,261,176,144,290,543,290,144,127,147,59,0,138,154,158,156,65,66,67,156,69,116,117,461,574,158,134,76,147,157,148,79,21,583,116,117,25,147,133,134,135,159,137,138,139,140,328,329,27,331,70,133,134,135,144,137,138,139,140,1055,674,153,144,107,680,134,156,116,117,50,59,21,154,127,156,25,65,66,67,134,69,163,151,64,133,134,135,76,137,138,139,140,561,270,909,144,151,143,127,854,277,539,917,791,148,154,708,156,368,1105,370,371,498,92,134,65,66,67,400,69,27,147,142,107,366,104,76,116,117,129,390,103,104,105,108,109,142,290,364,141,127,149,144,149,133,134,135,128,137,138,139,140,429,153,141,144,309,99,533,141,144,141,303,150,148,154,153,156,357,346,475,358,129,147,148,349,157,149,493,56,562,263,154,155,358,92,363,328,329,144,331,127,127,149,144,150,153,104,814,42,136,817,517,958,154,142,156,127,126,151,149,153,147,303,263,831,136,265,153,461,145,303,486,487,488,145,20,21,514,149,73,150,157,158,127,867,868,157,158,321,159,127,290,136,444,653,328,329,448,331,136,145,538,608,303,724,149,141,860,400,460,309,241,149,144,244,152,139,247,871,164,165,321,153,253,147,714,149,360,328,329,1036,331,155,127,128,129,130,131,132,133,480,481,136,146,911,150,539,144,127,141,142,154,155,920,159,149,153,136,152,252,742,141,141,303,504,576,145,400,150,301,149,872,127,498,141,757,127,159,157,158,135,321,149,241,76,77,78,79,328,329,141,331,150,144,727,253,147,147,330,627,400,159,303,127,154,142,563,127,100,746,514,770,136,517,127,135,136,137,142,142,321,566,135,136,137,145,147,328,329,149,331,571,791,142,538,155,149,157,158,543,150,144,134,654,0,656,138,150,141,159,839,144,149,486,487,488,581,694,155,127,400,553,495,674,770,129,22,135,136,137,562,27,709,29,566,864,865,866,144,652,526,514,528,516,150,144,486,487,488,1042,144,150,491,526,50,528,150,144,144,400,103,104,105,59,737,538,153,153,64,684,683,144,845,141,142,514,72,150,851,75,553,714,78,698,697,566,526,141,528,562,144,87,462,566,144,698,92,89,159,538,470,150,163,153,903,475,149,127,104,101,159,154,155,553,144,135,136,137,144,452,116,117,562,153,150,144,566,119,127,149,144,150,128,142,880,155,150,133,134,135,1029,137,138,139,140,772,526,1019,528,1024,694,1025,974,593,781,149,1026,903,1022,1023,147,157,795,743,462,709,1021,772,712,1027,1028,144,470,553,144,766,781,139,110,111,150,746,562,127,128,147,566,149,184,127,128,135,127,155,785,786,995,135,141,150,146,136,535,154,139,737,839,739,127,682,145,544,553,146,149,149,135,136,137,855,212,562,157,158,143,566,145,139,701,150,798,864,865,866,134,147,144,149,1011,1005,145,144,150,155,149,241,141,815,244,995,155,247,157,158,143,694,144,253,739,251,828,144,150,144,737,149,739,150,150,150,709,70,154,712,17,18,19,76,77,78,79,954,149,141,935,953,144,141,141,141,144,144,144,663,290,931,146,737,149,739,670,100,867,868,141,301,141,144,149,144,149,127,18,309,310,149,1089,313,149,135,136,137,149,763,90,91,92,149,320,145,927,149,983,149,330,134,149,131,149,138,126,157,158,142,149,1018,149,833,1121,149,486,487,488,1038,1039,1040,149,737,152,739,833,149,1030,1014,149,1016,160,149,364,106,107,149,670,94,95,161,368,1013,922,371,5,6,1018,143,144,976,378,149,380,381,382,22,23,833,1045,387,737,1033,739,143,144,143,144,1071,112,185,160,187,188,189,161,85,86,87,88,89,408,106,107,93,94,95,954,161,162,99,206,207,102,143,144,211,162,843,844,215,1072,142,924,65,867,868,134,127,127,149,1119,149,127,148,1101,924,149,833,234,141,138,449,143,150,141,1041,1006,1006,138,143,462,143,262,142,264,143,143,142,470,127,141,143,848,475,127,954,149,1119,924,1118,1018,1138,114,115,116,117,118,119,120,121,122,123,149,1135,149,149,146,153,159,150,139,153,150,143,113,127,149,307,954,156,302,1061,514,148,146,517,150,147,1115,148,148,144,143,150,526,150,528,153,144,143,149,144,127,150,149,321,538,144,924,1018,48,543,142,149,848,153,30,141,546,547,46,149,550,150,159,553,144,150,150,150,127,150,1112,1112,356,144,153,144,954,144,144,144,1018,144,144,129,156,47,25,26,366,1119,150,375,150,150,150,379,150,150,143,383,384,385,386,42,144,389,150,143,392,393,141,150,150,156,142,954,150,150,143,403,149,405,149,141,144,142,141,150,141,407,149,409,410,411,412,143,109,415,149,149,1018,144,420,421,422,144,430,425,1119,142,148,429,147,25,150,127,153,127,442,150,144,150,156,447,150,42,156,150,150,663,49,142,149,112,142,150,670,143,141,1018,141,1119,134,461,153,150,150,144,127,128,150,149,131,132,133,144,135,136,688,491,694,144,150,142,150,150,145,128,492,143,149,142,151,152,143,709,28,150,712,142,159,149,514,162,163,112,720,166,167,521,160,153,112,141,143,112,150,150,150,729,25,1119,149,149,144,143,143,124,143,150,127,128,141,144,131,132,133,150,135,136,150,539,150,149,129,142,129,128,145,143,143,563,149,144,151,152,25,561,144,563,1119,150,574,150,143,162,150,129,150,149,0,583,150,150,150,150,0,1112,1061,693,8,9,10,11,12,13,14,15,16,17,18,19,20,21,130,41,837,712,78,53,301,358,307,177,608,763,746,283,757,37,38,39,663,41,411,127,943,946,463,833,848,944,947,135,136,137,990,1115,56,627,1052,- 1,- 1,145,848,- 1,391,149,66,67,- 1,153,852,- 1,854,157,158,- 1,127,653,- 1,- 1,80,- 1,- 1,83,135,136,137,- 1,139,- 1,- 1,- 1,- 1,- 1,145,877,96,- 1,149,- 1,684,101,- 1,- 1,- 1,- 1,157,158,- 1,685,694,- 1,- 1,- 1,698,691,- 1,- 1,- 1,- 1,- 1,- 1,- 1,699,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,922,137,924,139,- 1,141,- 1,- 1,714,145,- 1,- 1,724,149,- 1,151,152,935,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,958,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,770,- 1,- 1,- 1,774,775,776,777,772,779,780,- 1,- 1,- 1,- 1,- 1,- 1,781,- 1,- 1,- 1,791,- 1,- 1,794,- 1,1006,- 1,- 1,- 1,- 1,795,- 1,- 1,- 1,- 1,- 1,- 1,1014,- 1,- 1,- 1,- 1,- 1,1020,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,1031,- 1,- 1,- 1,837,1036,- 1,- 1,- 1,- 1,40,- 1,42,- 1,1045,- 1,- 1,- 1,- 1,1050,839,- 1,- 1,- 1,- 1,- 1,1061,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,872,- 1,- 1,- 1,73,- 1,864,865,866,- 1,- 1,- 1,81,82,42,84,880,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,1112,903,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,927,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,142,- 1,933,145,- 1,- 1,- 1,149,- 1,- 1,152,112,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,953,124,166,167,127,128,- 1,- 1,131,132,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,982,- 1,984,149,- 1,151,152,- 1,154,155,- 1,- 1,- 1,995,- 1,- 1,162,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1019,- 1,1021,1022,1023,1024,1025,1026,1027,1028,1029,1030,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,42,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1071,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,112,- 1,- 1,73,- 1,1118,- 1,- 1,- 1,- 1,80,81,82,83,84,127,128,- 1,- 1,131,132,133,- 1,135,136,- 1,96,97,98,- 1,142,101,- 1,145,104,105,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,162,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,134,135,136,137,- 1,139,140,141,142,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,42,166,167,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,112,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,127,128,- 1,- 1,131,132,133,- 1,135,136,- 1,96,97,98,- 1,142,101,- 1,145,104,105,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,162,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,134,135,136,137,- 1,139,140,141,142,143,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,98,- 1,- 1,101,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,134,135,136,137,- 1,139,140,141,142,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,98,- 1,- 1,101,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,134,135,136,137,- 1,139,140,141,142,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,68,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,98,- 1,- 1,101,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,134,135,136,137,- 1,139,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,98,- 1,- 1,101,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,134,135,136,137,- 1,139,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,98,- 1,- 1,101,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,134,135,136,137,- 1,139,140,- 1,142,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,56,57,58,59,60,61,62,63,64,- 1,66,67,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,80,81,82,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,97,98,- 1,- 1,101,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,134,135,136,137,- 1,139,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,42,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,124,- 1,- 1,127,128,- 1,- 1,131,132,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,- 1,127,- 1,149,- 1,151,152,- 1,134,135,136,137,- 1,139,- 1,- 1,162,143,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,157,158,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,134,135,136,137,- 1,139,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,157,158,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,72,- 1,74,75,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,134,135,136,137,- 1,139,37,38,39,143,41,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,157,158,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,124,- 1,83,127,128,- 1,- 1,131,132,133,- 1,135,136,- 1,- 1,96,- 1,- 1,142,- 1,101,145,- 1,- 1,- 1,149,- 1,151,152,- 1,154,155,- 1,- 1,- 1,- 1,- 1,- 1,162,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,134,135,136,137,- 1,139,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,155,- 1,157,158,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,134,135,136,137,- 1,139,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,151,152,- 1,- 1,- 1,- 1,157,158,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,- 1,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,134,135,- 1,137,- 1,139,- 1,- 1,142,37,38,39,40,41,- 1,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,- 1,139,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,76,77,78,79,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,138,139,- 1,- 1,142,37,38,39,- 1,41,- 1,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,- 1,139,- 1,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,- 1,139,- 1,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,- 1,139,37,38,39,143,41,- 1,- 1,- 1,- 1,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,- 1,139,37,38,39,143,41,- 1,- 1,- 1,- 1,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,- 1,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,137,- 1,139,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,- 1,44,45,- 1,- 1,- 1,134,50,51,- 1,53,139,- 1,- 1,57,58,59,60,61,62,63,64,- 1,151,152,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,141,142,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,25,26,- 1,- 1,- 1,- 1,159,32,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,135,136,- 1,- 1,- 1,140,- 1,142,143,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,- 1,154,155,25,26,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,135,136,- 1,- 1,- 1,140,- 1,142,143,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,- 1,154,155,25,26,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,135,136,- 1,- 1,- 1,140,- 1,142,143,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,- 1,154,155,25,26,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,135,136,- 1,- 1,- 1,140,- 1,142,143,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,- 1,154,155,25,26,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,135,136,- 1,- 1,- 1,140,- 1,142,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,- 1,154,155,25,26,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,25,26,- 1,156,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,142,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,25,26,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,141,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,25,26,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,150,- 1,152,25,26,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,25,26,- 1,156,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,150,- 1,152,25,26,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,25,26,- 1,156,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,25,26,- 1,156,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,25,26,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,69,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,25,26,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,127,128,129,130,131,132,133,81,82,136,84,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,97,98,152,- 1,- 1,- 1,- 1,104,105,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,25,26,140,- 1,142,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,40,- 1,42,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,25,26,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,127,128,129,130,131,132,133,81,82,136,84,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,97,98,152,- 1,- 1,- 1,- 1,104,105,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,25,26,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,40,- 1,42,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,25,26,- 1,81,82,- 1,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,- 1,97,98,- 1,- 1,- 1,- 1,- 1,104,105,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,127,128,129,130,131,132,133,81,82,136,84,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,97,98,152,- 1,- 1,- 1,- 1,104,105,159,- 1,- 1,162,163,- 1,- 1,166,167,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,128,129,130,131,132,133,- 1,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,145,- 1,- 1,- 1,149,- 1,- 1,152,- 1,- 1,- 1,- 1,- 1,- 1,159,- 1,- 1,162,163,- 1,- 1,166,167};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int,int);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*,struct Cyc_Yyltype*);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple34{unsigned f0;struct _tuple0*f1;int f2;};struct _tuple35{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1150U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 395?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,396U,sizeof(short),yychar)): 343;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 7557)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7558U,sizeof(short),yyn))!=yychar1)goto yydefault;
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
if(yyn==1149){
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefact,1150U,sizeof(short),yystate));
if(yyn==0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,583U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),14U));
if(yylen > 0)
yyval=yyyvsp[0].v;
# 370 "cycbison.simple"
switch((int)yyn){case 1:
# 1219 "parse.y"
 yyval=yyyvsp[0].v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(& yyyvsp[0].v);
# 1222
goto _LL0;case 2:
# 1225 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 3:
# 1229 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_using();
# 1232
goto _LL0;case 4:
# 1233 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 5:
# 1236
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_namespace();
# 1239
goto _LL0;case 6:
# 1240 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 7:  {
# 1242 "parse.y"
int is_c_include=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(& yyyvsp[4].v);
struct _tuple31*_Tmp1=Cyc_yyget_YY55(& yyyvsp[5].v);unsigned _Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*exs=_Tmp3;unsigned wc=_Tmp2;
struct Cyc_List_List*hides=Cyc_yyget_YY56(& yyyvsp[6].v);
if(exs!=0 && hides!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U),_tag_fat(0U,sizeof(void*),0));
if((unsigned)hides && !((int)wc))
wc=yyyvsp[6].l.first_line;
if(!is_c_include){
if(exs!=0 || cycdecls!=0){
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("expecting \"C include\"",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f0=wc,_TmpA->f1=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}else{
# 1256
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_Tmp8->tag=11,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}}else{
# 1259
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f0=wc,_TmpA->f1=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}
# 1261
goto _LL0;}}case 8:
# 1262 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Porton_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 9:
# 1264 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Portoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 10:
# 1266 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempeston_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 11:
# 1268 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 12:
# 1269 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13:
# 1273 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14:
# 1276
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15:
# 1281 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})==0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C include",sizeof(char),10U));})==0)
yyval=Cyc_YY32(1);else{
# 1287
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY32(1);}}
# 1291
goto _LL0;case 16:
# 1294 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17:
# 1298 "parse.y"
 yyval=Cyc_YY56(0);
goto _LL0;case 18:
# 1299 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 19:
# 1303 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 20:
# 1304 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 21:
# 1305 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY56(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 22:
# 1309 "parse.y"
 yyval=Cyc_YY55(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f0=0,_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 23:
# 1310 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 24:
# 1314 "parse.y"
 yyval=Cyc_YY55(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY54(& yyyvsp[2].v);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 25:
# 1315 "parse.y"
 yyval=Cyc_YY55(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f0=0,_Tmp1->f1=0U;_Tmp1;}));
goto _LL0;case 26:
# 1316 "parse.y"
 yyval=Cyc_YY55(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f0=0,_Tmp1->f1=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 27:
# 1321 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f0=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 28:
# 1323 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f0=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),_Tmp3->f2=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY54(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 29:
# 1327 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 30:
# 1328 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 31:
# 1332 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp5=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_new_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 32:
# 1333 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 33:
# 1334 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 36:
# 1344 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 37:  {
# 1346 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 38:
# 1358 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 39:  {
# 1360 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 40:  {
# 1368 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 41:  {
# 1371 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 42:
# 1376 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(& yyyvsp[1].v));yyval=yyyvsp[1].v;
goto _LL0;case 43:
# 1379
 Cyc_Lex_leave_using();
goto _LL0;case 44:
# 1382
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp1=_Tmp2;});_Tmp1;}));yyval=yyyvsp[1].v;
goto _LL0;case 45:
# 1385
 Cyc_Lex_leave_namespace();
goto _LL0;case 46:
# 1391 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);unsigned _Tmp2=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,0,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 47:
# 1393 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);struct _tuple11*_Tmp2=({struct _tuple11*(*_Tmp3)(struct _tuple11*)=({struct _tuple11*(*_Tmp4)(struct _tuple11*)=(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;_Tmp4;});_Tmp3(Cyc_yyget_YY19(& yyyvsp[1].v));});unsigned _Tmp3=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 48:
# 1396
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_let_decl(_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 49:  {
# 1398 "parse.y"
struct Cyc_List_List*vds=0;
{struct Cyc_List_List*ids=Cyc_yyget_YY37(& yyyvsp[1].v);for(0;ids!=0;ids=ids->tl){
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0)),({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);qv->f0=_Tmp1;}),qv->f1=(struct _fat_ptr*)ids->hd;
vds=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Vardecl*_Tmp2=({struct _tuple0*_Tmp3=qv;Cyc_Absyn_new_vardecl(0U,_Tmp3,Cyc_Absyn_wildtyp(0),0);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=vds;_Tmp1;});}}
# 1403
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(vds);Cyc_Absyn_letv_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1405
goto _LL0;}case 50:
# 1408 "parse.y"
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[2].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp1;}),tv->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[4].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=Cyc_Absyn_region_decl(tv,vd,0,yyyvsp[0].l.first_line);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1414
goto _LL0;}}case 51:  {
# 1416
struct _fat_ptr tvstring=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp2,sizeof(void*),1));});
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[1].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=tvstring;_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp1;}),tv->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[1].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Tvar*_Tmp3=tv;struct Cyc_Absyn_Vardecl*_Tmp4=vd;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_YY60(& yyyvsp[2].v);Cyc_Absyn_region_decl(_Tmp3,_Tmp4,_Tmp5,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1423
goto _LL0;}}}case 52:
# 1426 "parse.y"
 yyval=Cyc_YY60(0);
goto _LL0;case 53:
# 1428 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("open",sizeof(char),5U));})!=0)Cyc_Warn_err(yyyvsp[3].l.first_line,_tag_fat("expecting `open'",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY60(Cyc_yyget_Exp_tok(& yyyvsp[3].v));
goto _LL0;case 54:
# 1432
 yyval=yyyvsp[0].v;
goto _LL0;case 55:
# 1433 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 56:
# 1439 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 57:  {
# 1442 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
if((int)two.sc!=7)
Cyc_Warn_warn(yyyvsp[0].l.first_line,
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1449
goto _LL0;}case 58:
# 1450 "parse.y"
 Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));
yyval=yyyvsp[1].v;
# 1453
goto _LL0;case 59:
# 1454 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 60:  {
# 1457 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,({
struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=two.type_specs;Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,Cyc_yyget_YY21(& yyyvsp[0].v));});_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1462
goto _LL0;}case 61:
# 1463 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.tq=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 62:  {
# 1465 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.tq);});_Tmp1.tq=_Tmp2;}),_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1469
goto _LL0;}case 63:
# 1470 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=1,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 64:  {
# 1473 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=1,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1476
goto _LL0;}case 65:
# 1477 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;case 66:  {
# 1480 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,({
# 1483
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.attributes);});_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;}case 67:
# 1487 "parse.y"
 yyval=Cyc_YY20(4U);
goto _LL0;case 68:
# 1488 "parse.y"
 yyval=Cyc_YY20(5U);
goto _LL0;case 69:
# 1489 "parse.y"
 yyval=Cyc_YY20(3U);
goto _LL0;case 70:
# 1490 "parse.y"
 yyval=Cyc_YY20(1U);
goto _LL0;case 71:
# 1492 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY20(2U);
# 1496
goto _LL0;case 72:
# 1496 "parse.y"
 yyval=Cyc_YY20(0U);
goto _LL0;case 73:
# 1498 "parse.y"
 yyval=Cyc_YY20(6U);
goto _LL0;case 74:
# 1503 "parse.y"
 yyval=Cyc_YY46(0);
goto _LL0;case 75:
# 1504 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 76:
# 1508 "parse.y"
 yyval=yyyvsp[3].v;
goto _LL0;case 77:
# 1511
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 78:
# 1512 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 79:
# 1515
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Atts_parse_nullary_att(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 80:
# 1516 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81:
# 1518 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);unsigned _Tmp3=yyyvsp[2].l.first_line;Cyc_Atts_parse_unary_att(_Tmp1,_Tmp2,_Tmp3,Cyc_yyget_Exp_tok(& yyyvsp[2].v));}));
goto _LL0;case 82:
# 1520 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;unsigned _Tmp2=yyyvsp[2].l.first_line;struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[0].v);struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);unsigned _Tmp5=({
unsigned _Tmp6=yyyvsp[4].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[4].v));});
# 1520
Cyc_Atts_parse_format_att(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,({
# 1522
unsigned _Tmp6=yyyvsp[6].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[6].v));}));}));
goto _LL0;case 83:
# 1532 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 84:
# 1534 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_typedef_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[1].v),0,0);});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 85:
# 1538 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,yyyvsp[0].l.first_line));
goto _LL0;case 86:
# 1539 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,yyyvsp[0].l.first_line));
goto _LL0;case 87:
# 1540 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec(yyyvsp[0].l.first_line));
goto _LL0;case 88:
# 1541 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,yyyvsp[0].l.first_line));
goto _LL0;case 89:
# 1542 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec(yyyvsp[0].l.first_line));
goto _LL0;case 90:
# 1543 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,yyyvsp[0].l.first_line));
goto _LL0;case 91:
# 1544 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,yyyvsp[0].l.first_line));
goto _LL0;case 92:
# 1545 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec(yyyvsp[0].l.first_line));
goto _LL0;case 93:
# 1546 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec(yyyvsp[0].l.first_line));
goto _LL0;case 94:
# 1547 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_complex_spec(yyyvsp[0].l.first_line));
goto _LL0;case 95:
# 1548 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 96:
# 1549 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 97:
# 1551 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 98:
# 1553 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_builtin_type(_tag_fat("__builtin_va_list",sizeof(char),18U),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 99:
# 1555 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 100:
# 1557 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_yyget_YY45(& yyyvsp[0].v);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 101:
# 1559 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 102:
# 1561 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(& yyyvsp[2].v)),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 103:
# 1563 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tuple_type(({struct Cyc_List_List*(*_Tmp2)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp3)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp3;});struct _tuple19*(*_Tmp3)(unsigned,struct _tuple8*)=({struct _tuple19*(*_Tmp4)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_Tmp4;});unsigned _Tmp4=yyyvsp[2].l.first_line;_Tmp2(_Tmp3,_Tmp4,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[2].v)));}));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1565
goto _LL0;case 104:
# 1566 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 105:
# 1568 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 106:
# 1570 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_aqual_handle_type(Cyc_yyget_YY57(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 107:
# 1572 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 108:
# 1574 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 109:
# 1576 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 110:
# 1581 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Kinds_id_to_kind(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 111:  {
# 1585 "parse.y"
unsigned loc=Cyc_Flags_porting_c_code?yyyvsp[0].l.first_line: 0U;
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=1,_Tmp1.q_volatile=0,_Tmp1.q_restrict=0,_Tmp1.real_const=1,_Tmp1.loc=loc;_Tmp1;}));
goto _LL0;}case 112:
# 1587 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=1,_Tmp1.q_restrict=0,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 113:
# 1588 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=0,_Tmp1.q_restrict=1,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 114:  {
# 1594 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl)),({void*_Tmp1=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Enumdecl*_Tmp3=({struct Cyc_Absyn_Enumdecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_Tmp4->sc=2U,({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp4->name=_Tmp5;}),({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY49(& yyyvsp[3].v);_Tmp6->v=_Tmp7;});_Tmp6;});_Tmp4->fields=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});ed->r=_Tmp1;}),ed->loc=yyyvsp[0].l.first_line;
# 1596
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=9,_Tmp2->f1=ed,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1598
goto _LL0;}case 115:
# 1599 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(& yyyvsp[1].v),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 116:
# 1601 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 117:
# 1607 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),_Tmp1->tag=0,_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 118:
# 1609 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->tag=_Tmp2;}),_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 119:
# 1613 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 120:
# 1614 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 121:
# 1615 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY49(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 122:
# 1621 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp2->tag=6,({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0,({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY25(& yyyvsp[2].v);_Tmp2->f3=_Tmp3;});_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 123:  {
# 1625 "parse.y"
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[2].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[4].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[4].v));});
struct _tuple29*po_qb=Cyc_yyget_YY51(& yyyvsp[5].v);
struct _tuple29 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple29 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_TypeDecl*td=({enum Cyc_Absyn_AggrKind _Tmp4=Cyc_yyget_YY23(& yyyvsp[0].v).f1;struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp6=ts;struct Cyc_Absyn_AggrdeclImpl*_Tmp7=({
struct Cyc_List_List*_Tmp8=exist_ts;struct Cyc_List_List*_Tmp9=po;struct Cyc_List_List*_TmpA=qb;struct Cyc_List_List*_TmpB=Cyc_yyget_YY25(& yyyvsp[6].v);Cyc_Absyn_aggrdecl_impl(_Tmp8,_Tmp9,_TmpA,_TmpB,Cyc_yyget_YY23(& yyyvsp[0].v).f0);});
# 1629
Cyc_Absyn_aggr_tdecl(_Tmp4,2U,_Tmp5,_Tmp6,_Tmp7,0,yyyvsp[0].l.first_line);});
# 1632
yyval=Cyc_YY21(({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp5->tag=9,_Tmp5->f1=td,_Tmp5->f2=0;_Tmp5;});Cyc_Parse_type_spec(_Tmp4,yyyvsp[0].l.first_line);}));
# 1634
goto _LL0;}}case 124:
# 1635 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_AggrInfo _Tmp2=({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY23(& yyyvsp[0].v).f1;struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_UnknownAggr(_Tmp3,_Tmp4,Cyc_yyget_YY23(& yyyvsp[0].v).f0?({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=(void*)1;_Tmp5;}): 0);});Cyc_Absyn_aggr_type(_Tmp2,
Cyc_yyget_YY41(& yyyvsp[2].v));});
# 1635
Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1637
goto _LL0;case 125:
# 1640 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp1;_Tmp1.f0=1,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[1].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 126:
# 1641 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp1;_Tmp1.f0=0,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 127:
# 1644
 yyval=Cyc_YY22(0U);
goto _LL0;case 128:
# 1645 "parse.y"
 yyval=Cyc_YY22(1U);
goto _LL0;case 129:
# 1648
 yyval=Cyc_YY41(0);
goto _LL0;case 130:
# 1649 "parse.y"
 yyval=Cyc_YY41(Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 131:
# 1654 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 132:  {
# 1656 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*x=Cyc_yyget_YY26(& yyyvsp[0].v);for(0;x!=0;x=x->tl){
decls=Cyc_List_imp_append((struct Cyc_List_List*)x->hd,decls);}}{
# 1660
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags!=0)
({void(*_Tmp1)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;_Tmp1;})(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1665
goto _LL0;}}case 133:
# 1669 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 134:
# 1670 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY26(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 135:
# 1676 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple11));_Tmp1->tl=0,({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 136:
# 1678 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple11));({struct _tuple11*_Tmp2=Cyc_yyget_YY19(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;}),({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 137:
# 1682 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0;_Tmp1;}));
goto _LL0;case 138:
# 1683 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 139:  {
# 1688 "parse.y"
struct _RegionHandle _Tmp1=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp1;_push_region(temp);
{struct _tuple27 _Tmp2=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f0;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*x=Cyc_yyget_YY30(& yyyvsp[1].v);for(0;x!=0;x=x->tl){
struct _tuple26*_Tmp6=(struct _tuple26*)x->hd;void*_Tmp7;void*_Tmp8;struct Cyc_Parse_Declarator _Tmp9;_Tmp9=_Tmp6->f0;_Tmp8=_Tmp6->f1;_Tmp7=_Tmp6->f2;{struct Cyc_Parse_Declarator d=_Tmp9;struct Cyc_Absyn_Exp*wd=_Tmp8;struct Cyc_Absyn_Exp*wh=_Tmp7;
decls=({struct _tuple13*_TmpA=_region_malloc(temp,0U,sizeof(struct _tuple13));_TmpA->tl=decls,_TmpA->hd=d;_TmpA;});
widths_and_reqs=({struct Cyc_List_List*_TmpA=_region_malloc(temp,0U,sizeof(struct Cyc_List_List));({struct _tuple16*_TmpB=({struct _tuple16*_TmpC=_region_malloc(temp,0U,sizeof(struct _tuple16));_TmpC->f0=wd,_TmpC->f1=wh;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=widths_and_reqs;_TmpA;});}}}{
# 1698
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct Cyc_List_List*info=({struct _RegionHandle*_Tmp6=temp;struct _RegionHandle*_Tmp7=temp;struct Cyc_List_List*_Tmp8=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1699
Cyc_List_rzip(_Tmp6,_Tmp7,_Tmp8,widths_and_reqs);});
# 1701
yyval=Cyc_YY25(({struct Cyc_List_List*(*_Tmp6)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp6;})(Cyc_Parse_make_aggr_field,yyyvsp[0].l.first_line,info));
# 1703
_npop_handler(0);goto _LL0;}}}}
# 1689
;_pop_region();}case 140:
# 1710 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 141:  {
# 1712 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;_Tmp1.f0=two.f0,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 142:
# 1714 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 143:  {
# 1716 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f0);});_Tmp1.f0=_Tmp2;}),_Tmp1.f1=two.f1,_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 144:
# 1718 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 145:  {
# 1720 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;_Tmp1.f0=two.f0,_Tmp1.f1=two.f1,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f2);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;}case 146:
# 1726 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 147:  {
# 1728 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;_Tmp1.f0=two.f0,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 148:
# 1730 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 149:  {
# 1732 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f0);});_Tmp1.f0=_Tmp2;}),_Tmp1.f1=two.f1,_Tmp1.f2=two.f2;_Tmp1;}));
goto _LL0;}case 150:
# 1734 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f0=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 151:  {
# 1736 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;_Tmp1.f0=two.f0,_Tmp1.f1=two.f1,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f2);});_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;}case 152:
# 1742 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 153:
# 1744 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp2=Cyc_yyget_YY29(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY30(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 154:
# 1749 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple26));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_YY60(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 155:
# 1753 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple26));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f0=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->f0.id=_Tmp2;}),_Tmp1->f0.varloc=0U,_Tmp1->f0.tms=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
# 1755
goto _LL0;case 156:
# 1758 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple26));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f0=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->f0.id=_Tmp2;}),_Tmp1->f0.varloc=0U,_Tmp1->f0.tms=0,_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
# 1760
goto _LL0;case 157:
# 1761 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp1=_region_malloc(yyr,0U,sizeof(struct _tuple26));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 158:
# 1765 "parse.y"
 yyval=Cyc_YY60(Cyc_yyget_Exp_tok(& yyyvsp[2].v));
goto _LL0;case 159:
# 1766 "parse.y"
 yyval=Cyc_YY60(0);
goto _LL0;case 160:  {
# 1772 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[2].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_Absyn_TypeDecl*dd=({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=ts;struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY35(& yyyvsp[4].v);_Tmp4->v=_Tmp5;});_Tmp4;});int _Tmp4=is_extensible;Cyc_Absyn_datatype_tdecl(2U,_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
# 1776
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=9,_Tmp2->f1=dd,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1778
goto _LL0;}case 161:  {
# 1779 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeInfo _Tmp2=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1782
goto _LL0;}case 162:  {
# 1783 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeFieldInfo _Tmp2=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.datatype_name=_Tmp4;}),({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[3].v);_Tmp3.field_name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_field_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[4].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1786
goto _LL0;}case 163:
# 1789 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 164:
# 1790 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 165:
# 1794 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 166:
# 1795 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 167:
# 1796 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 168:
# 1797 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 169:
# 1801 "parse.y"
 yyval=Cyc_YY33(2U);
goto _LL0;case 170:
# 1802 "parse.y"
 yyval=Cyc_YY33(3U);
goto _LL0;case 171:
# 1803 "parse.y"
 yyval=Cyc_YY33(0U);
goto _LL0;case 172:
# 1807 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=0,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;case 173:  {
# 1809 "parse.y"
struct Cyc_List_List*typs=({struct Cyc_List_List*(*_Tmp1)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});struct _tuple19*(*_Tmp2)(unsigned,struct _tuple8*)=({struct _tuple19*(*_Tmp3)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_Tmp3;});unsigned _Tmp3=yyyvsp[3].l.first_line;_Tmp1(_Tmp2,_Tmp3,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[3].v)));});
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=typs,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;}case 174:
# 1815 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 175:  {
# 1817 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 176:
# 1824 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 177:  {
# 1826 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 178:
# 1831 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 179:
# 1832 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 180:  {
# 1836 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1840
goto _LL0;}case 181:
# 1841 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 182:
# 1843 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 183:  {
# 1846 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY61(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _TmpC;({struct _tuple0*_TmpD=Cyc_yyget_YY28(& yyyvsp[0].v).id;_TmpC.id=_TmpD;}),({unsigned _TmpD=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_TmpC.varloc=_TmpD;}),({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp10=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp10->tag=3,({void*_Tmp11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp12=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp12->tag=1,_Tmp12->f1=lis,_Tmp12->f2=b,_Tmp12->f3=c,_Tmp12->f4=eff,_Tmp12->f5=po,_Tmp12->f6=qb,_Tmp12->f7=req,_Tmp12->f8=ens,_Tmp12->f9=thrws;_Tmp12;});_Tmp10->f1=_Tmp11;});_Tmp10;});_TmpE->hd=_TmpF;}),({struct Cyc_List_List*_TmpF=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_TmpE->tl=_TmpF;});_TmpE;});_TmpC.tms=_TmpD;});_TmpC;}));
# 1850
goto _LL0;}}}case 184:
# 1851 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 185:  {
# 1854
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[1].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1857
goto _LL0;}case 186:
# 1858 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 1858
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1861
goto _LL0;case 187:
# 1865 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 188:
# 1866 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 189:
# 1867 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 190:  {
# 1871 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1875
goto _LL0;}case 191:  {
# 1876 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 192:  {
# 1880 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1884
goto _LL0;}case 193:  {
# 1885 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY61(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _TmpC;_TmpC.id=one.id,_TmpC.varloc=one.varloc,({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp10=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp10->tag=3,({void*_Tmp11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp12=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp12->tag=1,_Tmp12->f1=lis,_Tmp12->f2=b,_Tmp12->f3=c,_Tmp12->f4=eff,_Tmp12->f5=po,_Tmp12->f6=qb,_Tmp12->f7=req,_Tmp12->f8=ens,_Tmp12->f9=thrws;_Tmp12;});_Tmp10->f1=_Tmp11;});_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=one.tms;_TmpE;});_TmpC.tms=_TmpD;});_TmpC;}));
# 1890
goto _LL0;}}}case 194:  {
# 1891 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 195:  {
# 1895
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[1].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 196:  {
# 1899 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 197:
# 1905 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 198:
# 1906 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY27(& yyyvsp[1].v));}));
goto _LL0;case 199:  {
# 1910 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(& yyyvsp[3].v)!=0)
ans=({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp3->tag=5,_Tmp3->f1=yyyvsp[3].l.first_line,({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY46(& yyyvsp[3].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=ans;_Tmp1;});{
# 1914
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _Tmp1=*Cyc_yyget_YY1(& yyyvsp[0].v);void*_Tmp2;void*_Tmp3;unsigned _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{unsigned ploc=_Tmp4;void*nullable=_Tmp3;void*bound=_Tmp2;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));_Tmp5->ptr_loc=ploc,_Tmp5->rgn_loc=yyyvsp[2].l.first_line,_Tmp5->zt_loc=yyyvsp[1].l.first_line;_Tmp5;});{
void*mod=({struct _RegionHandle*_Tmp5=yyr;struct Cyc_Absyn_PtrLoc*_Tmp6=ptrloc;void*_Tmp7=nullable;void*_Tmp8=bound;void*_Tmp9=Cyc_yyget_YY45(& yyyvsp[2].v);struct Cyc_List_List*_TmpA=Cyc_yyget_YY59(& yyyvsp[1].v);Cyc_Parse_make_pointer_mod(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_yyget_YY24(& yyyvsp[4].v));});
ans=({struct Cyc_List_List*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));_Tmp5->hd=mod,_Tmp5->tl=ans;_Tmp5;});
yyval=Cyc_YY27(ans);
# 1922
goto _LL0;}}}}case 200:
# 1924
 yyval=Cyc_YY59(0);
goto _LL0;case 201:
# 1925 "parse.y"
 yyval=Cyc_YY59(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY58(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY59(& yyyvsp[1].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 202:
# 1930 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 203:
# 1931 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=1,({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 204:
# 1932 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=2;_Tmp1;}));
goto _LL0;case 205:
# 1933 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=3;_Tmp1;}));
goto _LL0;case 206:
# 1934 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=6;_Tmp1;}));
goto _LL0;case 207:
# 1935 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=4;_Tmp1;}));
goto _LL0;case 208:
# 1936 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=5;_Tmp1;}));
goto _LL0;case 209:
# 1937 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=7;_Tmp1;}));
goto _LL0;case 210:
# 1938 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=8;_Tmp1;}));
goto _LL0;case 211:
# 1939 "parse.y"
 yyval=Cyc_YY58((void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=9,({void*_Tmp2=Cyc_yyget_YY57(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 212:  {
# 1940 "parse.y"
void*aq=({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));});yyval=Cyc_YY58((void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=9,_Tmp1->f1=aq;_Tmp1;}));
goto _LL0;}case 213:
# 1944 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_YY57(& yyyvsp[0].v));
goto _LL0;case 214:
# 1950 "parse.y"
 yyval=Cyc_YY57(({void*_Tmp1=Cyc_yyget_YY45(& yyyvsp[0].v);Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 1952
goto _LL0;case 215:
# 1954 "parse.y"
 yyval=Cyc_YY57(({void*_Tmp1=Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 1956
goto _LL0;case 216:
# 1960 "parse.y"
 goto _LL0;case 217:
# 1960 "parse.y"
 yyval=Cyc_YY57(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 218:
# 1966 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_Tmp1->f2=Cyc_Absyn_fat_bound_type:({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 219:
# 1967 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_false_type,({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 220:
# 1968 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f0=yyyvsp[0].l.first_line,_Tmp1->f1=Cyc_Absyn_true_type,_Tmp1->f2=Cyc_Absyn_fat_bound_type;_Tmp1;}));
goto _LL0;case 221:
# 1971
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 222:
# 1972 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 223:
# 1975
 yyval=Cyc_YY53(Cyc_Tcutil_any_bool(0));
goto _LL0;case 224:
# 1976 "parse.y"
 yyval=Cyc_YY53(Cyc_Absyn_true_type);
goto _LL0;case 225:
# 1977 "parse.y"
 yyval=Cyc_YY53(Cyc_Absyn_false_type);
goto _LL0;case 226:
# 1982 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));
goto _LL0;case 227:
# 1983 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(& yyyvsp[0].v),& Cyc_Kinds_rk,1);yyval=yyyvsp[0].v;
goto _LL0;case 228:
# 1984 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));
goto _LL0;case 229:
# 1988 "parse.y"
 yyval=Cyc_YY24(Cyc_Absyn_empty_tqual((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l.first_line));
goto _LL0;case 230:
# 1989 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp1,Cyc_yyget_YY24(& yyyvsp[1].v));}));
goto _LL0;case 231:  {
# 1994 "parse.y"
struct _tuple29*po_qb=Cyc_yyget_YY51(& yyyvsp[1].v);
struct _tuple29 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple29 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple28*_Tmp4=_cycalloc(sizeof(struct _tuple28));_Tmp4->f0=0,_Tmp4->f1=0,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[0].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=po,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 232:  {
# 1998 "parse.y"
struct _tuple29*po_qb=Cyc_yyget_YY51(& yyyvsp[2].v);
struct _tuple29 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple29 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple28*_Tmp4=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=0,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[1].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=po,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 233:  {
# 2002 "parse.y"
struct _tuple29*po_qb=Cyc_yyget_YY51(& yyyvsp[4].v);
struct _tuple29 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple29 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY40(({struct _tuple28*_Tmp4=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=1,_Tmp4->f2=0,({void*_Tmp5=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp4->f3=_Tmp5;}),_Tmp4->f4=po,_Tmp4->f5=qb;_Tmp4;}));
goto _LL0;}}case 234:  {
# 2007
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[1].v);v->inject=_Tmp5;});{
struct _tuple29*po_qb=Cyc_yyget_YY51(& yyyvsp[4].v);
struct _tuple29 _Tmp5=(unsigned)po_qb?*po_qb:({struct _tuple29 _Tmp6;_Tmp6.f0=0,_Tmp6.f1=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*po=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY40(({struct _tuple28*_Tmp8=_cycalloc(sizeof(struct _tuple28));_Tmp8->f0=0,_Tmp8->f1=0,_Tmp8->f2=v,({void*_Tmp9=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp8->f3=_Tmp9;}),_Tmp8->f4=po,_Tmp8->f5=qb;_Tmp8;}));
# 2013
goto _LL0;}}}}case 235:  {
# 2015
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[4].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[3].v);v->inject=_Tmp5;});{
struct _tuple29*po_qb=Cyc_yyget_YY51(& yyyvsp[6].v);
struct _tuple29 _Tmp5=(unsigned)po_qb?*po_qb:({struct _tuple29 _Tmp6;_Tmp6.f0=0,_Tmp6.f1=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*po=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY40(({struct _tuple28*_Tmp8=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp9=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp8->f0=_Tmp9;}),_Tmp8->f1=0,_Tmp8->f2=v,({void*_Tmp9=Cyc_yyget_YY50(& yyyvsp[5].v);_Tmp8->f3=_Tmp9;}),_Tmp8->f4=po,_Tmp8->f5=qb;_Tmp8;}));
# 2021
goto _LL0;}}}}case 236:
# 2025 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 237:
# 2026 "parse.y"
 yyval=Cyc_YY50(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 238:
# 2029
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp3->tag=1,_Tmp3->f1=0;_Tmp3;});void*_Tmp3=Cyc_yyget_YY50(& yyyvsp[1].v);Cyc_Parse_id2type(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 239:
# 2030 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(& yyyvsp[2].v));void*_Tmp3=Cyc_yyget_YY50(& yyyvsp[3].v);Cyc_Parse_id2type(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 240:
# 2033
 yyval=Cyc_YY50(0);
goto _LL0;case 241:
# 2034 "parse.y"
 yyval=Cyc_YY50(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 242:
# 2038 "parse.y"
 yyval=Cyc_YY51(0);
goto _LL0;case 243:
# 2039 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 244:
# 2044 "parse.y"
 yyval=Cyc_YY51(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));({struct Cyc_List_List*_Tmp2=({struct _tuple30*_Tmp3[1];({struct _tuple30*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(struct _tuple30*),1));});_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 245:
# 2046 "parse.y"
 yyval=Cyc_YY51(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));_Tmp1->f0=0,({struct Cyc_List_List*_Tmp2=({struct _tuple30*_Tmp3[1];({struct _tuple30*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(struct _tuple30*),1));});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 246:  {
# 2049 "parse.y"
struct _tuple29*rest=Cyc_yyget_YY51(& yyyvsp[2].v);
void*_Tmp1;if(rest!=0){_Tmp1=rest->f0;{struct Cyc_List_List*rpo=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple30*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=rpo;_Tmp3;});(*rest).f0=_Tmp2;});
yyval=Cyc_YY51(rest);
# 2054
goto _LL0;}}else{_throw_match();}}case 247:  {
# 2056 "parse.y"
struct _tuple29*rest=Cyc_yyget_YY51(& yyyvsp[2].v);
void*_Tmp1;if(rest!=0){_Tmp1=rest->f1;{struct Cyc_List_List*qb=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple30*_Tmp4=Cyc_yyget_YY52(& yyyvsp[0].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=qb;_Tmp3;});(*rest).f1=_Tmp2;});
yyval=Cyc_YY51(rest);
# 2061
goto _LL0;}}else{_throw_match();}}case 248:  {
# 2068 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct)),kb->tag=2,kb->f1=0,kb->f2=& Cyc_Kinds_rk;{
void*t=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);void*_Tmp2=(void*)kb;Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[2].l.first_line);});
yyval=Cyc_YY52(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));({void*_Tmp2=Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=t;_Tmp1;}));
# 2072
goto _LL0;}}case 249:
# 2077 "parse.y"
 yyval=Cyc_YY52(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->f0=_Tmp2;}),({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
# 2079
goto _LL0;case 250:
# 2082 "parse.y"
 yyval=Cyc_YY57(Cyc_Absyn_al_qual_type);
goto _LL0;case 251:
# 2083 "parse.y"
 yyval=Cyc_YY57(Cyc_Absyn_un_qual_type);
goto _LL0;case 252:
# 2084 "parse.y"
 yyval=Cyc_YY57(Cyc_Absyn_rc_qual_type);
goto _LL0;case 253:
# 2085 "parse.y"
 yyval=Cyc_YY57(Cyc_Absyn_rtd_qual_type);
goto _LL0;case 254:
# 2086 "parse.y"
 yyval=Cyc_YY57(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_id2aqual(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 255:
# 2091 "parse.y"
 yyval=Cyc_YY45(Cyc_yyget_YY57(& yyyvsp[0].v));
goto _LL0;case 256:
# 2094 "parse.y"
 yyval=Cyc_YY45(({void*_Tmp1=Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Absyn_aqual_var_type(_Tmp1,Cyc_Absyn_al_qual_type);}));
# 2096
goto _LL0;case 257:  {
# 2101 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct)),kb->tag=0,kb->f1=& Cyc_Kinds_aqk;
yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);void*_Tmp2=(void*)kb;Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[0].l.first_line);}));
# 2104
goto _LL0;}case 258:
# 2106 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_aqualsof_type(Cyc_yyget_YY45(& yyyvsp[2].v)));
# 2108
goto _LL0;case 259:
# 2135 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 260:
# 2137 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_zstrcmp(_Tmp1,_tag_fat("inject",sizeof(char),7U));})!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(yyyvsp[0].l.first_line,_tag_fat(_Tmp2,sizeof(void*),1));});
yyval=Cyc_YY32(1);
# 2141
goto _LL0;case 261:
# 2144 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 262:
# 2145 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY41(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));}));
goto _LL0;case 263:
# 2149 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 264:
# 2150 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 265:
# 2151 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 266:
# 2153 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(& yyyvsp[0].v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 2156
goto _LL0;case 267:
# 2161 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);}));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 268:
# 2163 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);}));_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 269:
# 2168 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 270:
# 2169 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY39(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 271:  {
# 2175 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
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
# 2189
goto _LL0;}}}}}}case 272:  {
# 2190 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
if(atts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_Tmp6,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=tq,_Tmp5->f2=t;_Tmp5;}));
# 2197
goto _LL0;}}}case 273:  {
# 2198 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
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
# 2210
goto _LL0;}}}}case 274:
# 2213 "parse.y"
 yyval=Cyc_YY37(Cyc_List_imp_rev(Cyc_yyget_YY37(& yyyvsp[0].v)));
goto _LL0;case 275:
# 2216
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 276:
# 2217 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY37(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 277:
# 2221 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 278:
# 2222 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 279:
# 2227 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 280:
# 2229 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 281:
# 2231 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=35,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 282:  {
# 2233 "parse.y"
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[2].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});void*_Tmp3=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,_Tmp3,
Cyc_Absyn_uint_exp(0U,yyyvsp[2].l.first_line));});
# 2236
vd->tq.real_const=1;
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_Tmp2->tag=26,_Tmp2->f1=vd,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2239
goto _LL0;}case 283:  {
# 2241 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_Tmp2->tag=27,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=t,_Tmp2->f3=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2244
goto _LL0;}case 284:
# 2249 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));_Tmp3->f0=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 285:
# 2251 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp3->f0=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 286:
# 2253 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));_Tmp3->f0=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 287:
# 2255 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[2].v);_Tmp3->f0=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 288:
# 2259 "parse.y"
 yyval=Cyc_YY42(Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[0].v)));
goto _LL0;case 289:
# 2260 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp3->tag=1,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 290:
# 2265 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 291:
# 2266 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 292:
# 2270 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 293:
# 2271 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp1->tag=1,({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 294:  {
# 2276 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
if(atts!=0)
Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("ignoring attributes in type",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=tq,_Tmp5->f2=t;_Tmp5;}));
# 2282
goto _LL0;}}case 295:  {
# 2283 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f0;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
struct Cyc_List_List*tms=Cyc_yyget_YY31(& yyyvsp[1].v).tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);
if(t_info.f2!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(t_info.f3!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,_tag_fat("bad specifiers, ignoring",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f0=0,_Tmp5->f1=t_info.f0,_Tmp5->f2=t_info.f1;_Tmp5;}));
# 2293
goto _LL0;}}case 296:
# 2296 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 297:
# 2297 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(0));
goto _LL0;case 298:
# 2298 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 299:
# 2299 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v)));
goto _LL0;case 300:
# 2300 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;})));
goto _LL0;case 301:
# 2301 "parse.y"
 yyval=Cyc_YY45(Cyc_yyget_YY45(& yyyvsp[0].v));
goto _LL0;case 302:
# 2307 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 303:
# 2308 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 304:
# 2312 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY27(& yyyvsp[0].v);_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 305:
# 2313 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 306:
# 2315 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,Cyc_yyget_YY31(& yyyvsp[1].v).tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 307:
# 2320 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 308:
# 2322 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[2].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 309:
# 2324 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 310:
# 2326 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[3].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 311:
# 2328 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY53(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2328
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2331
goto _LL0;case 312:  {
# 2332 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY40(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY61(& yyyvsp[3].v);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _TmpC;({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp10=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp10->tag=3,({void*_Tmp11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp12=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp12->tag=1,_Tmp12->f1=lis,_Tmp12->f2=b,_Tmp12->f3=c,_Tmp12->f4=eff,_Tmp12->f5=po,_Tmp12->f6=qb,_Tmp12->f7=req,_Tmp12->f8=ens,_Tmp12->f9=thrws;_Tmp12;});_Tmp10->f1=_Tmp11;});_Tmp10;});_TmpE->hd=_TmpF;}),_TmpE->tl=0;_TmpE;});_TmpC.tms=_TmpD;});_TmpC;}));
# 2336
goto _LL0;}}}case 313:  {
# 2337 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f0;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct _tuple21 _Tmp8=Cyc_yyget_YY61(& yyyvsp[4].v);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_Absyn_Exp*req=_TmpB;struct Cyc_Absyn_Exp*ens=_TmpA;struct Cyc_Absyn_Exp*thrws=_Tmp9;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _TmpC;({struct Cyc_List_List*_TmpD=({struct Cyc_List_List*_TmpE=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_TmpF=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp10=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp10->tag=3,({void*_Tmp11=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp12=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp12->tag=1,_Tmp12->f1=lis,_Tmp12->f2=b,_Tmp12->f3=c,_Tmp12->f4=eff,_Tmp12->f5=po,_Tmp12->f6=qb,_Tmp12->f7=req,_Tmp12->f8=ens,_Tmp12->f9=thrws;_Tmp12;});_Tmp10->f1=_Tmp11;});_Tmp10;});_TmpE->hd=_TmpF;}),({
struct Cyc_List_List*_TmpF=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_TmpE->tl=_TmpF;});_TmpE;});
# 2339
_TmpC.tms=_TmpD;});_TmpC;}));
# 2342
goto _LL0;}}}case 314:  {
# 2344
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp2;});unsigned _Tmp2=yyyvsp[1].l.first_line;_Tmp1(Cyc_Parse_typ2tvar,_Tmp2,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[1].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2345
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2348
goto _LL0;}case 315:
# 2349 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,0U,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2351
goto _LL0;case 316:
# 2355 "parse.y"
 yyval=Cyc_YY61(({struct _tuple21 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f0=_Tmp2;}),_Tmp1.f1=0,_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 317:
# 2357 "parse.y"
 yyval=Cyc_YY61(({struct _tuple21 _Tmp1;_Tmp1.f0=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 318:
# 2359 "parse.y"
 yyval=Cyc_YY61(({struct _tuple21 _Tmp1;_Tmp1.f0=0,_Tmp1.f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 319:
# 2361 "parse.y"
 yyval=Cyc_YY61(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f0=_Tmp3;}),_Tmp2.f1=0,_Tmp2.f2=0;_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY61(& yyyvsp[4].v));}));
goto _LL0;case 320:
# 2363 "parse.y"
 yyval=Cyc_YY61(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;_Tmp2.f0=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f1=_Tmp3;}),_Tmp2.f2=0;_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY61(& yyyvsp[4].v));}));
goto _LL0;case 321:
# 2365 "parse.y"
 yyval=Cyc_YY61(({struct _tuple21 _Tmp1=({struct _tuple21 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2.f2=_Tmp3;});_Tmp2;});Cyc_Parse_join_assns(_Tmp1,Cyc_yyget_YY61(& yyyvsp[4].v));}));
goto _LL0;case 322:
# 2369 "parse.y"
 yyval=Cyc_YY61(({struct _tuple21 _Tmp1;_Tmp1.f0=0,_Tmp1.f1=0,_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 323:
# 2370 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 324:
# 2375 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 325:
# 2376 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 326:
# 2377 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 327:
# 2378 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 328:
# 2379 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 329:
# 2380 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 330:
# 2386 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 331:
# 2390 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 332:
# 2391 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_exp_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 333:
# 2396 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 334:
# 2397 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 335:
# 2402 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));}));
goto _LL0;case 336:
# 2403 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 337:
# 2405 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_Absyn_skip_stmt(0U));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 338:
# 2407 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_yyget_Stmt_tok(& yyyvsp[3].v));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 339:
# 2408 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 340:
# 2409 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Absyn_seq_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 341:
# 2410 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 342:
# 2413 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 343:
# 2418 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 344:
# 2420 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[6].v);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 345:
# 2426 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 346:
# 2429
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_unknownid_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 347:
# 2432
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[3].v);Cyc_Absyn_tuple_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 348:
# 2436 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);Cyc_Absyn_trycatch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 349:
# 2450 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 350:
# 2453 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2453
_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 351:
# 2456 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[2].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2456
_Tmp1->hd=_Tmp2;}),({
# 2458
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 352:
# 2460 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 353:
# 2462 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[4].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2462
_Tmp1->hd=_Tmp2;}),({
# 2464
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 354:
# 2466 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[5].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 355:
# 2473 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);Cyc_Absyn_while_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 356:
# 2475 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_do_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 357:
# 2477 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[8].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 358:  {
# 2479 "parse.y"
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[5].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[7].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp1,s);}));
goto _LL0;}case 359:
# 2483
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 360:
# 2484 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 361:
# 2487
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp2=_Tmp3;});_Tmp2;});Cyc_Absyn_goto_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 362:
# 2488 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 363:
# 2489 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 364:
# 2490 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 365:
# 2491 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_return_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 366:
# 2493 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 367:
# 2494 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 368:
# 2496 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_fallthru_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 369:
# 2505 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 370:
# 2508
 yyval=yyyvsp[0].v;
goto _LL0;case 371:
# 2510 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 372:
# 2513
 yyval=yyyvsp[0].v;
goto _LL0;case 373:
# 2515 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 374:
# 2518
 yyval=yyyvsp[0].v;
goto _LL0;case 375:
# 2520 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 376:
# 2523
 yyval=yyyvsp[0].v;
goto _LL0;case 377:
# 2525 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 378:
# 2528
 yyval=yyyvsp[0].v;
goto _LL0;case 379:
# 2530 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 380:
# 2533
 yyval=yyyvsp[0].v;
goto _LL0;case 381:
# 2535 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 382:
# 2538
 yyval=yyyvsp[0].v;
goto _LL0;case 383:
# 2540 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 384:
# 2543
 yyval=yyyvsp[0].v;
goto _LL0;case 385:
# 2545 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 386:
# 2548
 yyval=yyyvsp[0].v;
goto _LL0;case 387:
# 2550 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 388:
# 2552 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 389:
# 2555
 yyval=yyyvsp[0].v;
goto _LL0;case 390:
# 2557 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 391:
# 2560
 yyval=yyyvsp[0].v;
goto _LL0;case 392:
# 2562 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 393:
# 2565
 yyval=yyyvsp[0].v;
goto _LL0;case 394:  {
# 2567 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);})));
# 2570
goto _LL0;}case 395:
# 2573 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 396:
# 2577 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 397:  {
# 2579 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line)));
goto _LL0;}case 398:
# 2582 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);})));
goto _LL0;case 399:
# 2584 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=(*Cyc_yyget_YY38(& yyyvsp[2].v)).f2;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 400:
# 2589 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 401:
# 2597 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 402:
# 2601 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line));
goto _LL0;case 403:
# 2602 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 404:
# 2603 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_Tmp2->tag=14,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 405:
# 2604 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=5,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 406:  {
# 2606 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(& yyyvsp[0].v);
{void*_Tmp1=e->r;struct _fat_ptr _Tmp2;int _Tmp3;short _Tmp4;char _Tmp5;enum Cyc_Absyn_Sign _Tmp6;if(*((int*)_Tmp1)==0)switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.LongLong_c.tag){case 2: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f0;_Tmp5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;char i=_Tmp5;
# 2609
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_Tmp8->tag=10,_Tmp8->f1=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4E5;}case 4: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f0;_Tmp4=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;short i=_Tmp4;
# 2611
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=9,_Tmp8->f1=s,_Tmp8->f2=(int)i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4E5;}case 5: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f0;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f1;{enum Cyc_Absyn_Sign s=_Tmp6;int i=_Tmp3;
# 2613
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=9,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4E5;}case 7: _Tmp2=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f0;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f1;{struct _fat_ptr s=_Tmp2;int i=_Tmp3;
# 2615
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_Tmp8->tag=11,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4E5;}case 1:
# 2617
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL4E5;case 8:
 goto _LL4F3;case 9: _LL4F3:
# 2620
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("strings cannot occur within patterns",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));goto _LL4E5;case 6:
# 2622
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));goto _LL4E5;default: goto _LL4F6;}else{_LL4F6:
# 2624
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("bad constant in case",sizeof(char),21U),_tag_fat(0U,sizeof(void*),0));}_LL4E5:;}
# 2627
goto _LL0;}case 407:
# 2628 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `as'",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2630
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2633
goto _LL0;case 408:
# 2634 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `alias'",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_rk;_Tmp2;});tv->kind=_Tmp1;}),tv->aquals_bound=0;{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2643
goto _LL0;}}}case 409:
# 2644 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `alias'",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_rk;_Tmp2;});tv->kind=_Tmp1;}),tv->aquals_bound=0;{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2653
goto _LL0;}}}case 410:  {
# 2654 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=0,_Tmp5->f2=1,_Tmp5->f3=0,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2657
goto _LL0;}}case 411:  {
# 2658 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY10(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_Tmp5->tag=15,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=ps,_Tmp5->f3=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2661
goto _LL0;}}case 412:  {
# 2662 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[3].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp5;});unsigned _Tmp5=yyyvsp[2].l.first_line;_Tmp4(Cyc_Parse_typ2tvar,_Tmp5,Cyc_yyget_YY41(& yyyvsp[2].v));});
union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,Cyc_yyget_QualId_tok(& yyyvsp[0].v),0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct)),typ->tag=0,({void*_Tmp4=(void*)({struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_Tmp5->tag=25,_Tmp5->f1=ai;_Tmp5;});typ->f1=_Tmp4;}),typ->f2=0;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=(void*)typ,_Tmp5->f2=0,_Tmp5->f3=exist_ts,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2668
goto _LL0;}}case 413:  {
# 2669 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;_Tmp5;});unsigned _Tmp5=yyyvsp[1].l.first_line;_Tmp4(Cyc_Parse_typ2tvar,_Tmp5,Cyc_yyget_YY41(& yyyvsp[1].v));});
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,_Tmp5->f1=0,_Tmp5->f2=0,_Tmp5->f3=exist_ts,_Tmp5->f4=fps,_Tmp5->f5=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2673
goto _LL0;}}case 414:
# 2674 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=5,({struct Cyc_Absyn_Pat*_Tmp3=({void*_Tmp4=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp5->tag=5,({struct Cyc_Absyn_Pat*_Tmp6=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;});_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);});_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 415:
# 2676 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
# 2678
struct Cyc_Absyn_Pat*_Tmp3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[1].l.first_line);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2676
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2680
goto _LL0;case 416:
# 2681 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
Cyc_Warn_err(yyyvsp[2].l.first_line,_tag_fat("expecting `as'",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
# 2685
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[3].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2683
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2687
goto _LL0;case 417:  {
# 2688 "parse.y"
void*tag=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);void*_Tmp2=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik);Cyc_Parse_id2type(_Tmp1,_Tmp2,0,yyyvsp[2].l.first_line);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,({struct Cyc_Absyn_Tvar*_Tmp3=Cyc_Parse_typ2tvar(yyyvsp[2].l.first_line,tag);_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(tag),0);});
# 2690
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2689
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2693
goto _LL0;}case 418:  {
# 2694 "parse.y"
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,_Tmp2->f1=tv,({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2696
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2695
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2699
goto _LL0;}case 419:
# 2702 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 420:
# 2703 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 421:
# 2704 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f0=0,_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 422:
# 2707
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 423:
# 2708 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY11(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 424:
# 2711
 yyval=Cyc_YY12(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));_Tmp1->f0=0,({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 425:
# 2712 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 426:
# 2715
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 427:
# 2716 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f0=_Tmp2;}),_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 428:
# 2717 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f0=0,_Tmp1->f1=1;_Tmp1;}));
goto _LL0;case 429:
# 2720
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp2=Cyc_yyget_YY12(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 430:
# 2721 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp2=Cyc_yyget_YY12(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY13(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 431:
# 2726 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 432:
# 2727 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_seq_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 433:
# 2731 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 434:
# 2733 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Core_Opt*_Tmp2=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assignop_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 435:
# 2735 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_swap_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 436:
# 2739 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 437:
# 2740 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)1U;_Tmp1;}));
goto _LL0;case 438:
# 2741 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)3U;_Tmp1;}));
goto _LL0;case 439:
# 2742 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)4U;_Tmp1;}));
goto _LL0;case 440:
# 2743 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)0U;_Tmp1;}));
goto _LL0;case 441:
# 2744 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)2U;_Tmp1;}));
goto _LL0;case 442:
# 2745 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)16U;_Tmp1;}));
goto _LL0;case 443:
# 2746 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)17U;_Tmp1;}));
goto _LL0;case 444:
# 2747 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)13U;_Tmp1;}));
goto _LL0;case 445:
# 2748 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)15U;_Tmp1;}));
goto _LL0;case 446:
# 2749 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)14U;_Tmp1;}));
goto _LL0;case 447:
# 2753 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 448:
# 2755 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 449:
# 2757 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_throw_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 450:
# 2759 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 451:
# 2760 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 452:
# 2761 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 453:
# 2762 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 454:  {
# 2763 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp4=rgn;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp6=qual;Cyc_Absyn_New_exp(_Tmp4,_Tmp5,_Tmp6,yyyvsp[0].l.first_line);}));
goto _LL0;}}case 455:  {
# 2764 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp4=rgn;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp6=qual;Cyc_Absyn_New_exp(_Tmp4,_Tmp5,_Tmp6,yyyvsp[0].l.first_line);}));
goto _LL0;}}case 456:
# 2776 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 457:
# 2779
 yyval=yyyvsp[0].v;
goto _LL0;case 458:
# 2781 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 459:
# 2784
 yyval=yyyvsp[0].v;
goto _LL0;case 460:
# 2786 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 461:
# 2789
 yyval=yyyvsp[0].v;
goto _LL0;case 462:
# 2791 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 463:
# 2794
 yyval=yyyvsp[0].v;
goto _LL0;case 464:
# 2796 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 465:
# 2799
 yyval=yyyvsp[0].v;
goto _LL0;case 466:
# 2801 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 467:
# 2804
 yyval=yyyvsp[0].v;
goto _LL0;case 468:
# 2806 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 469:
# 2809
 yyval=yyyvsp[0].v;
goto _LL0;case 470:
# 2811 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 471:
# 2814
 yyval=yyyvsp[0].v;
goto _LL0;case 472:
# 2816 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 473:
# 2818 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 474:
# 2821
 yyval=yyyvsp[0].v;
goto _LL0;case 475:
# 2823 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 476:
# 2826
 yyval=yyyvsp[0].v;
goto _LL0;case 477:
# 2828 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 478:
# 2831
 yyval=Cyc_YY68(Cyc_Absyn_eq_exp);
goto _LL0;case 479:
# 2832 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_neq_exp);
goto _LL0;case 480:
# 2835
 yyval=Cyc_YY68(Cyc_Absyn_lt_exp);
goto _LL0;case 481:
# 2836 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_gt_exp);
goto _LL0;case 482:
# 2837 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_lte_exp);
goto _LL0;case 483:
# 2838 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_gte_exp);
goto _LL0;case 484:
# 2841
 yyval=Cyc_YY6(0U);
goto _LL0;case 485:
# 2842 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 486:
# 2845
 yyval=Cyc_YY6(1U);
goto _LL0;case 487:
# 2846 "parse.y"
 yyval=Cyc_YY6(3U);
goto _LL0;case 488:
# 2847 "parse.y"
 yyval=Cyc_YY6(4U);
goto _LL0;case 489:
# 2851 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 490:  {
# 2853 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;}case 491:
# 2858 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 492:
# 2859 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;case 493:
# 2860 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,2U,yyyvsp[0].l.first_line);}));
goto _LL0;case 494:
# 2861 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_address_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 495:
# 2862 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_deref_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 496:
# 2863 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 497:  {
# 2865 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line));
# 2868
goto _LL0;}case 498:
# 2868 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 499:  {
# 2870 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
# 2873
goto _LL0;}case 500:
# 2875
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,_Tmp2->f1.rgn=0,_Tmp2->f1.aqual=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2877
goto _LL0;case 501:
# 2878 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.aqual=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2880
goto _LL0;case 502:
# 2881 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.aqual=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2883
goto _LL0;case 503:  {
# 2884 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;
yyval=Cyc_Exp_tok(({void*_Tmp4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1.mknd=2U,_Tmp5->f1.rgn=rgn,_Tmp5->f1.aqual=qual,_Tmp5->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp5->f1.num_elts=_Tmp6;}),_Tmp5->f1.fat_result=0,_Tmp5->f1.inline_call=0;_Tmp5;});Cyc_Absyn_new_exp(_Tmp4,yyyvsp[0].l.first_line);}));
# 2887
goto _LL0;}}case 504:  {
# 2888 "parse.y"
struct _tuple16 _Tmp1=Cyc_Parse_split_seq(Cyc_yyget_Exp_tok(& yyyvsp[2].v));void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{struct Cyc_Absyn_Exp*rgn=_Tmp3;struct Cyc_Absyn_Exp*qual=_Tmp2;
yyval=Cyc_Exp_tok(({void*_Tmp4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp5->tag=33,_Tmp5->f1.mknd=0U,_Tmp5->f1.rgn=rgn,_Tmp5->f1.aqual=qual,_Tmp5->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp5->f1.num_elts=_Tmp6;}),_Tmp5->f1.fat_result=0,_Tmp5->f1.inline_call=1;_Tmp5;});Cyc_Absyn_new_exp(_Tmp4,yyyvsp[0].l.first_line);}));
# 2891
goto _LL0;}}case 505:  {
# 2892 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,_Tmp2->f1.rgn=0,_Tmp2->f1.aqual=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2895
goto _LL0;}case 506:  {
# 2896 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[10].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[10].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.aqual=_Tmp3;}),({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2899
goto _LL0;}case 507:  {
# 2901
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[8].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[8].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=33,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.aqual=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2904
goto _LL0;}case 508:
# 2905 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2[1];({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2[0]=_Tmp3;});Cyc_List_list(_tag_fat(_Tmp2,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(18U,_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 509:
# 2907 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=37,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 510:
# 2909 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=37,({struct Cyc_Absyn_Exp*_Tmp3=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_deref_exp(_Tmp4,yyyvsp[2].l.first_line);});_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 511:  {
# 2911 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,yyyvsp[0].l.first_line));
goto _LL0;}case 512:
# 2913 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=Cyc_yyget_YY62(& yyyvsp[1].v);Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 513:
# 2914 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_extension_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 514:
# 2915 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 515:
# 2916 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_false_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 516:
# 2920 "parse.y"
 yyval=Cyc_YY6(12U);
goto _LL0;case 517:
# 2921 "parse.y"
 yyval=Cyc_YY6(11U);
goto _LL0;case 518:
# 2922 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 519:
# 2923 "parse.y"
 yyval=Cyc_YY6(0U);
goto _LL0;case 520:  {
# 2928 "parse.y"
struct _tuple32*_Tmp1=Cyc_yyget_YY63(& yyyvsp[3].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f0;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*outlist=_Tmp4;struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY62((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_Tmp5->tag=39,({int _Tmp6=Cyc_yyget_YY32(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=outlist,_Tmp5->f4=inlist,_Tmp5->f5=clobbers;_Tmp5;}));
goto _LL0;}}case 521:
# 2933 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 522:
# 2934 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 523:
# 2938 "parse.y"
 yyval=Cyc_YY63(({struct _tuple32*_Tmp1=_cycalloc(sizeof(struct _tuple32));_Tmp1->f0=0,_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 524:  {
# 2940 "parse.y"
struct _tuple29*_Tmp1=Cyc_yyget_YY64(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY63(({struct _tuple32*_Tmp4=_cycalloc(sizeof(struct _tuple32));_Tmp4->f0=0,_Tmp4->f1=inlist,_Tmp4->f2=clobbers;_Tmp4;}));
goto _LL0;}}case 525:  {
# 2943 "parse.y"
struct _tuple29*_Tmp1=Cyc_yyget_YY64(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f0;_Tmp2=_Tmp1->f1;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY63(({struct _tuple32*_Tmp4=_cycalloc(sizeof(struct _tuple32));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY66(& yyyvsp[1].v));_Tmp4->f0=_Tmp5;}),_Tmp4->f1=inlist,_Tmp4->f2=clobbers;_Tmp4;}));
goto _LL0;}}case 526:
# 2947
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 527:
# 2948 "parse.y"
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=Cyc_yyget_YY67(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 528:
# 2952 "parse.y"
 yyval=Cyc_YY64(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));_Tmp1->f0=0,_Tmp1->f1=0;_Tmp1;}));
goto _LL0;case 529:
# 2953 "parse.y"
 yyval=Cyc_YY64(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));_Tmp1->f0=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 530:
# 2954 "parse.y"
 yyval=Cyc_YY64(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY66(& yyyvsp[1].v));_Tmp1->f0=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 531:
# 2957
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 532:
# 2958 "parse.y"
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=Cyc_yyget_YY67(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 533:
# 2962 "parse.y"
 yyval=Cyc_YY67(({struct _tuple33*_Tmp1=_cycalloc(sizeof(struct _tuple33));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);_Tmp1->f0=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 534:
# 2967 "parse.y"
 yyval=Cyc_YY65(0);
goto _LL0;case 535:
# 2968 "parse.y"
 yyval=Cyc_YY65(0);
goto _LL0;case 536:
# 2969 "parse.y"
 yyval=Cyc_YY65(Cyc_List_imp_rev(Cyc_yyget_YY65(& yyyvsp[1].v)));
goto _LL0;case 537:
# 2972
 yyval=Cyc_YY65(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 538:
# 2973 "parse.y"
 yyval=Cyc_YY65(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 539:
# 2978 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 540:
# 2980 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_subscript_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 541:
# 2982 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_unknowncall_exp(_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 542:
# 2984 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_unknowncall_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 543:
# 2986 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrmember_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 544:
# 2988 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrarrow_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 545:
# 2990 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,1U,yyyvsp[0].l.first_line);}));
goto _LL0;case 546:
# 2992 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,3U,yyyvsp[0].l.first_line);}));
goto _LL0;case 547:
# 2994 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 548:
# 2996 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 549:
# 2998 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=24,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 550:
# 3003 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 551:  {
# 3006
unsigned i=({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Parse_cnst2uint(_Tmp1,Cyc_yyget_Int_tok(& yyyvsp[0].v));});
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=Cyc_Absyn_tuple_field_designator((int)i);void*_Tmp2;_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp1)->f1;{struct _fat_ptr*f=_Tmp2;
yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp5->tag=0,_Tmp5->f1=f;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;}));
goto _LL0;}}case 552:
# 3010 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 553:  {
# 3013
unsigned i=({unsigned _Tmp1=yyyvsp[2].l.first_line;Cyc_Parse_cnst2uint(_Tmp1,Cyc_yyget_Int_tok(& yyyvsp[2].v));});
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=Cyc_Absyn_tuple_field_designator((int)i);void*_Tmp2;_Tmp2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp1)->f1;{struct _fat_ptr*f=_Tmp2;
yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp5->tag=0,_Tmp5->f1=f;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp3->tl=_Tmp4;});_Tmp3;}));
goto _LL0;}}case 554:
# 3020 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_unknownid_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 555:
# 3021 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Absyn_pragma_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 556:
# 3022 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 557:
# 3023 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_string_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 558:
# 3024 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wstring_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 559:
# 3025 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 560:
# 3029 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_noinstantiate_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 561:
# 3032
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[3].v));Cyc_Absyn_instantiate_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 562:
# 3035
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp2->tag=28,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[3].v));_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 563:
# 3037 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_tuple_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 564:
# 3039 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Absyn_stmt_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 565:
# 3043 "parse.y"
 yyval=Cyc_YY4(Cyc_List_imp_rev(Cyc_yyget_YY4(& yyyvsp[0].v)));
goto _LL0;case 566:
# 3048 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 567:
# 3050 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 568:
# 3056 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _Tmp1=Cyc_yyget_Int_tok(& yyyvsp[0].v);Cyc_Absyn_const_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 569:
# 3057 "parse.y"
 yyval=Cyc_Exp_tok(({char _Tmp1=Cyc_yyget_Char_tok(& yyyvsp[0].v);Cyc_Absyn_char_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 570:
# 3058 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wchar_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 571:
# 3060 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp(yyyvsp[0].l.first_line));
goto _LL0;case 572:  {
# 3062 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(& yyyvsp[0].v);
int l=(int)Cyc_strlen(f);
int i=1;
if(l > 0){
char c=((const char*)f.curr)[l - 1];
if((int)c==102 ||(int)c==70)i=0;else{
if((int)c==108 ||(int)c==76)i=2;}}
# 3070
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,yyyvsp[0].l.first_line));
# 3072
goto _LL0;}case 573:
# 3075 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f0=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 574:
# 3076 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 575:
# 3079
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f0=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 576:
# 3080 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 577:
# 3085 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 578:
# 3086 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 579:
# 3089
 yyval=yyyvsp[0].v;
goto _LL0;case 580:
# 3090 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 581:
# 3095 "parse.y"
 goto _LL0;case 582:
# 3095 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default:
# 3099
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
yystate=({int _Tmp1=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,175U,sizeof(short),yyn - 168));_Tmp1 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7557)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7558U,sizeof(short),yystate))==(int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)Cyc_yydefgoto[yyn - 168];}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus==0){
# 424
++ yynerrs;
# 427
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1150U,sizeof(short),yystate));
# 429
if(yyn > -32768 && yyn < 7557){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 343U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7558U,sizeof(short),x + yyn))==x)
({unsigned long _Tmp1=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,343U,sizeof(struct _fat_ptr),x)))+ 15U;sze +=_Tmp1;}),count ++;}
msg=({unsigned _Tmp1=(unsigned)(sze + 15)+ 1U;_tag_fat(({char*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(char)));});({{unsigned _Tmp3=(unsigned)(sze + 15);unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;}),sizeof(char),_Tmp1);});
Cyc_strcpy(msg,_tag_fat("parse error",sizeof(char),12U));
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 343U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7558U,sizeof(short),x + yyn))==x){
# 451
Cyc_strcat(msg,count==0?
_tag_fat(", expecting `",sizeof(char),14U):
 _tag_fat(" or `",sizeof(char),6U));
Cyc_strcat(msg,*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,343U,sizeof(struct _fat_ptr),x)));
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1150U,sizeof(short),yystate));
if(yyn==-32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7557)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7558U,sizeof(short),yyn))!=1)goto yyerrdefault;
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
if(yyn==1149){
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 546
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_Tmp1;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3098 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
void*_Tmp0;struct _fat_ptr _Tmp1;char _Tmp2;union Cyc_Absyn_Cnst _Tmp3;switch(v.Stmt_tok.tag){case 1: _Tmp3=v.Int_tok.val;{union Cyc_Absyn_Cnst c=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 2: _Tmp2=v.Char_tok.val;{char c=_Tmp2;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)c);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%c",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 3: _Tmp1=v.String_tok.val;{struct _fat_ptr s=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("\"%s\"",sizeof(char),5U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 4: _Tmp0=v.QualId_tok.val;{struct _tuple0*q=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 5: _Tmp0=v.Exp_tok.val;{struct Cyc_Absyn_Exp*e=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 6: _Tmp0=v.Stmt_tok.val;{struct Cyc_Absyn_Stmt*s=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}default:
 Cyc_fprintf(Cyc_stderr,_tag_fat("?",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 3110
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return _tag_fat("end-of-file",sizeof(char),12U);
if(token==382)
return Cyc_Lex_token_string;
if(token==391)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 395?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,396U,sizeof(short),token)): 343;
if((unsigned)z < 343U)
return*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,343U,sizeof(struct _fat_ptr),z));
return _tag_fat(0,0,0);}}
# 3124
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _Tmp0=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _Tmp0;_push_region(yyr);
({struct _RegionHandle*_Tmp1=yyr;Cyc_yyparse(_Tmp1,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_Tmp1=Cyc_Parse_parse_result;_npop_handler(0);return _Tmp1;}
# 3127
;_pop_region();}}

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
extern struct _fat_ptr Cyc_strcpy(struct _fat_ptr,struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 134 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 155
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 160
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 165
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 272
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 285
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 292
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 453 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 460
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 478
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 492
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 653 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 815
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 840
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 848
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 854
void*Cyc_Absyn_compress(void*);
# 867
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 869
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 872
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 874
extern void*Cyc_Absyn_sint_type;
# 876
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 879
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 883
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 885
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 910
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 914
void*Cyc_Absyn_bounds_one (void);
# 916
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 936
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 939
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 954
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 960
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 972
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 989
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 998
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
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1031
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1042
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1045
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1048
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1052
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1057
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1059
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1066
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1073
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1076
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1081
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
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
# 34
extern int Cyc_Flags_no_register;
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 30
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 32
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
# 47
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
# 49
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
# 51
extern struct Cyc_Core_Opt Cyc_Kinds_iko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_trko;
# 66
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 70
struct Cyc_Absyn_Kind*Cyc_Kinds_id_to_kind(struct _fat_ptr,unsigned);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 46 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 228
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 238
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
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct _tuple13{struct _tuple13*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
# 54 "parse.h"
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
return first;}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 167
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 172
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 174
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=msg;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
(void*)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Parse_Exit_exn_struct));_Tmp0->tag=Cyc_Parse_Exit;_Tmp0;}));}
# 179
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 181
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;_Tmp1=tqt->f2;_Tmp0=tqt->f3;{struct Cyc_Absyn_Tqual tq=_Tmp1;void*t=_Tmp0;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc!=0U)loc=tq.loc;
Cyc_Warn_warn(loc,({const char*_Tmp2="qualifier on type is ignored";_tag_fat(_Tmp2,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));}
# 186
return t;}}struct _tuple15{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 189
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 195
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*autoreleased=Cyc_Tcutil_any_bool(0);
for(1;pqs!=0;pqs=pqs->tl){
void*_Tmp0=(void*)pqs->hd;void*_Tmp1;switch(*((int*)_Tmp0)){case 4:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 6:
 autoreleased=Cyc_Absyn_true_type;goto _LL0;case 8:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 7:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0: _Tmp1=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp1;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}default: _Tmp1=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_Tmp0)->f1;{void*t=_Tmp1;
rgn=t;goto _LL0;}}_LL0:;}
# 209
return({struct _tuple15 _Tmp0;_Tmp0.f1=nullable,_Tmp0.f2=bound,_Tmp0.f3=zeroterm,_Tmp0.f4=rgn,_Tmp0.f5=autoreleased;_Tmp0;});}
# 215
static struct _tuple0*Cyc_Parse_gensym_enum (void){
# 217
static int enum_counter=0;
return({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);_Tmp0->f1=_Tmp1;}),({
struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)enum_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="__anonymous_enum_%d__";_tag_fat(_Tmp6,sizeof(char),22U);}),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 222
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 227
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;unsigned _Tmp7;_Tmp7=field_info->f1->f1;_Tmp6=field_info->f1->f2;_Tmp5=field_info->f1->f3;_Tmp4=field_info->f1->f4;_Tmp3=field_info->f1->f5;_Tmp2=field_info->f1->f6;_Tmp1=field_info->f2->f1;_Tmp0=field_info->f2->f2;{unsigned varloc=_Tmp7;struct _tuple0*qid=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;struct Cyc_List_List*tvs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;struct Cyc_Absyn_Exp*widthopt=_Tmp1;struct Cyc_Absyn_Exp*reqopt=_Tmp0;
if(tvs!=0)
Cyc_Warn_err(loc,({const char*_Tmp8="bad type params in struct field";_tag_fat(_Tmp8,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_Absyn_is_qvar_qualified(qid))
Cyc_Warn_err(loc,({const char*_Tmp8="struct or union field cannot be qualified with a namespace";_tag_fat(_Tmp8,sizeof(char),59U);}),_tag_fat(0U,sizeof(void*),0));
return({struct Cyc_Absyn_Aggrfield*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp8->name=(*qid).f2,_Tmp8->tq=tq,_Tmp8->type=t,_Tmp8->width=widthopt,_Tmp8->attributes=atts,_Tmp8->requires_clause=reqopt;_Tmp8;});}}
# 237
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _Tmp0;_Tmp0.Signed_spec=0,_Tmp0.Unsigned_spec=0,_Tmp0.Short_spec=0,_Tmp0.Long_spec=0,_Tmp0.Long_Long_spec=0,_Tmp0.Valid_type_spec=0,_Tmp0.Type_spec=Cyc_Absyn_sint_type,_Tmp0.loc=loc;_Tmp0;});}
# 248
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 254
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Signed_spec=1;
return s;}
# 259
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Unsigned_spec=1;
return s;}
# 264
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Short_spec=1;
return s;}
# 269
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Long_spec=1;
return s;}
# 276
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 278
return Cyc_Tcutil_is_array_type(t)?({
void*_Tmp0=t;Cyc_Tcutil_promote_array(_Tmp0,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 291 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
struct _tuple8*_Tmp0=(struct _tuple8*)x->hd;struct _fat_ptr _Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)((struct _tuple8*)_Tmp0)->f3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->tl==0){_Tmp3=_Tmp0->f1;_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f3)->f2->hd;if((struct _fat_ptr*)_Tmp3!=0){struct _fat_ptr*v=_Tmp3;void*i=_Tmp2;
# 296
{void*_Tmp4;if(*((int*)i)==1){_Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)i)->f2;{void**z=(void**)_Tmp4;
# 300
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=*v;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="`%s";_tag_fat(_Tmp8,sizeof(char),4U);}),_tag_fat(_Tmp7,sizeof(void*),1));});*nm=_Tmp5;});
({void*_Tmp5=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp6->name=nm,_Tmp6->identity=-1,({void*_Tmp7=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp8->tag=0,_Tmp8->f1=& Cyc_Kinds_ik;_Tmp8;});_Tmp6->kind=_Tmp7;});_Tmp6;}));*z=_Tmp5;});
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 305
res=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp5=({struct _tuple19*_Tmp6=_cycalloc(sizeof(struct _tuple19));_Tmp6->f1=v,_Tmp6->f2=i;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=res;_Tmp4;});goto _LL0;}else{if(((struct _tuple8*)_Tmp0)->f1!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1!=0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f1)==3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->hd)==1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->tl==0){_Tmp1=*_Tmp0->f1;_Tmp3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f3)->f2->hd)->f2;{struct _fat_ptr v=_Tmp1;void**z=(void**)_Tmp3;
# 309
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=v;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="`%s";_tag_fat(_Tmp7,sizeof(char),4U);}),_tag_fat(_Tmp6,sizeof(void*),1));});*nm=_Tmp4;});
({void*_Tmp4=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp5->name=nm,_Tmp5->identity=-1,({void*_Tmp6=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp7->tag=0,_Tmp7->f1=& Cyc_Kinds_rk;_Tmp7;});_Tmp5->kind=_Tmp6;});_Tmp5;}));*z=_Tmp4;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1!=0)goto _LL5;else{_LL5:
 goto _LL0;}}_LL0:;}
# 314
return res;}
# 318
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
void*_Tmp0=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->tl==0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*i=_Tmp1;
# 323
res=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp3=({struct _tuple19*_Tmp4=_cycalloc(sizeof(struct _tuple19));_Tmp4->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_Tmp4->f2=i;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=res;_Tmp2;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 326
return res;}
# 330
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)==0){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f1.Rel_n.tag==1){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f1.Rel_n.val==0){_Tmp1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1->f2;{struct _fat_ptr*y=_Tmp1;
# 333
{struct Cyc_List_List*ts=tags;for(0;ts!=0;ts=ts->tl){
struct _tuple19*_Tmp2=(struct _tuple19*)ts->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct _fat_ptr*x=_Tmp4;void*i=_Tmp3;
if(Cyc_strptrcmp(x,y)==0)
return({void*_Tmp5=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp6->tag=39,({void*_Tmp7=Cyc_Tcutil_copy_type(i);_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_exp(_Tmp5,e->loc);});}}}
# 338
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 341
return e;}
# 346
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;void*_Tmp6;struct Cyc_Absyn_Tqual _Tmp7;void*_Tmp8;switch(*((int*)t)){case 4: _Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.tq;_Tmp6=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.num_elts;_Tmp5=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zero_term;_Tmp4=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)t)->f1.zt_loc;{void*et=_Tmp8;struct Cyc_Absyn_Tqual tq=_Tmp7;struct Cyc_Absyn_Exp*nelts=_Tmp6;void*zt=_Tmp5;unsigned ztloc=_Tmp4;
# 349
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts!=0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*et2=Cyc_Parse_substitute_tags(tags,et);
if(nelts!=nelts2 || et!=et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3: _Tmp8=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_type;_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.elt_tq;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.rgn;_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.nullable;_Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.bounds;_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.zero_term;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.ptrloc;_Tmp0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)t)->f1.ptr_atts.autoreleased;{void*et=_Tmp8;struct Cyc_Absyn_Tqual tq=_Tmp7;void*r=_Tmp6;void*n=_Tmp5;void*b=_Tmp3;void*zt=_Tmp2;struct Cyc_Absyn_PtrLoc*pl=_Tmp1;void*rel=_Tmp0;
# 357
void*et2=Cyc_Parse_substitute_tags(tags,et);
void*b2=Cyc_Parse_substitute_tags(tags,b);
if(et2!=et || b2!=b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp9;_Tmp9.elt_type=et2,_Tmp9.elt_tq=tq,_Tmp9.ptr_atts.rgn=r,_Tmp9.ptr_atts.nullable=n,_Tmp9.ptr_atts.bounds=b2,_Tmp9.ptr_atts.zero_term=zt,_Tmp9.ptr_atts.ptrloc=pl,_Tmp9.ptr_atts.autoreleased=rel;_Tmp9;}));
goto _LL0;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)==13){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2)->tl==0){_Tmp8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f2->hd;{void*t=_Tmp8;
# 363
void*t2=Cyc_Parse_substitute_tags(tags,t);
if(t!=t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9: _Tmp8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 367
struct Cyc_Absyn_Exp*e2=Cyc_Parse_substitute_tags_exp(tags,e);
if(e2!=e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9:
 goto _LL0;}_LL0:;}
# 372
return t;}
# 377
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_Tmp0=Cyc_Parse_substitute_tags(tags,x->type);x->type=_Tmp0;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple21{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 384
static struct _tuple20*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple21*t){
return({struct _tuple20*_Tmp0=_cycalloc(sizeof(struct _tuple20));_Tmp0->f1=(*t).f2,_Tmp0->f2=(*t).f3;_Tmp0;});}
# 388
static int Cyc_Parse_is_typeparam(void*tm){
if(*((int*)tm)==4)
return 1;else{
return 0;};}
# 397
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(Cyc_zstrcmp(s,({const char*_Tmp0="`H";_tag_fat(_Tmp0,sizeof(char),3U);}))==0)
return Cyc_Absyn_heap_rgn_type;
if(Cyc_zstrcmp(s,({const char*_Tmp0="`U";_tag_fat(_Tmp0,sizeof(char),3U);}))==0)
return Cyc_Absyn_unique_rgn_type;
if(Cyc_zstrcmp(s,({const char*_Tmp0="`RC";_tag_fat(_Tmp0,sizeof(char),4U);}))==0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->name=_Tmp1;}),_Tmp0->identity=-1,_Tmp0->kind=k;_Tmp0;}));}
# 409
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(Cyc_zstrcmp(s,({const char*_Tmp0="`H";_tag_fat(_Tmp0,sizeof(char),3U);}))==0)
Cyc_Warn_err(loc,({const char*_Tmp0="bad occurrence of heap region";_tag_fat(_Tmp0,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,({const char*_Tmp0="`U";_tag_fat(_Tmp0,sizeof(char),3U);}))==0)
Cyc_Warn_err(loc,({const char*_Tmp0="bad occurrence of unique region";_tag_fat(_Tmp0,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,({const char*_Tmp0="`RC";_tag_fat(_Tmp0,sizeof(char),4U);}))==0)
Cyc_Warn_err(loc,({const char*_Tmp0="bad occurrence of refcounted region";_tag_fat(_Tmp0,sizeof(char),36U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
Cyc_Warn_err(loc,({const char*_Tmp0="bad occurrence of \"current\" region";_tag_fat(_Tmp0,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));}
# 424
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_Tmp0;if(*((int*)t)==2){_Tmp0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*pr=_Tmp0;
return pr;}}else{
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_Tmp1="expecting a list of type variables, not types";_tag_fat(_Tmp1,sizeof(char),46U);}));};}
# 432
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=(void**)&((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1->kind;{void**cptr=(void**)_Tmp1;
# 435
void*_Tmp2=Cyc_Kinds_compress_kb(*cptr);if(*((int*)_Tmp2)==1){
# 437
({void*_Tmp3=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp4->tag=2,_Tmp4->f1=0,_Tmp4->f2=k;_Tmp4;}): Cyc_Kinds_kind_to_bound(k);*cptr=_Tmp3;});
return;}else{
return;};}}else{
# 441
return;};}
# 446
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 454
if(tms==0)return 0;{
# 456
void*_Tmp0=(void*)tms->hd;void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp1;
# 459
if(tms->tl==0 ||
 Cyc_Parse_is_typeparam((void*)_check_null(tms->tl)->hd)&& _check_null(tms->tl)->tl==0){
# 462
void*_Tmp2;if(*((int*)args)==1){
# 464
Cyc_Warn_warn(loc,({const char*_Tmp3="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_Tmp3,sizeof(char),93U);}),_tag_fat(0U,sizeof(void*),0));
# 466
return tms;}else{_Tmp2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f1;{struct Cyc_List_List*ids=_Tmp2;
# 468
if(({int _Tmp3=Cyc_List_length(ids);_Tmp3!=Cyc_List_length(tds);}))
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_Tmp3="wrong number of parameter declarations in old-style function declaration";_tag_fat(_Tmp3,sizeof(char),73U);}));{
# 472
struct Cyc_List_List*rev_new_params=0;
for(1;ids!=0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2!=0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)tds2->hd;
void*_Tmp3=x->r;void*_Tmp4;if(*((int*)_Tmp3)==0){_Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 479
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!=0)
continue;
if(vd->initializer!=0)
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,({const char*_Tmp5="initializer found in parameter declaration";_tag_fat(_Tmp5,sizeof(char),43U);}));
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,({const char*_Tmp5="namespaces forbidden in parameter declarations";_tag_fat(_Tmp5,sizeof(char),47U);}));
rev_new_params=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp6=({struct _tuple8*_Tmp7=_cycalloc(sizeof(struct _tuple8));_Tmp7->f1=(*vd->name).f2,_Tmp7->f2=vd->tq,_Tmp7->f3=vd->type;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=rev_new_params;_Tmp5;});
# 487
goto L;}}else{
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,({const char*_Tmp5="nonvariable declaration in parameter type";_tag_fat(_Tmp5,sizeof(char),42U);}));};}
# 491
L: if(tds2==0)
({unsigned _Tmp3=loc;({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(_Tmp3,Cyc_strconcat(*((struct _fat_ptr*)ids->hd),({const char*_Tmp4=" is not given a type";_tag_fat(_Tmp4,sizeof(char),21U);})));});}
# 494
return({struct Cyc_List_List*_Tmp3=_region_malloc(yy,sizeof(struct Cyc_List_List));
({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yy,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yy,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp7->tag=1,({struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(rev_new_params);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=0,_Tmp7->f3=0,_Tmp7->f4=0,_Tmp7->f5=0,_Tmp7->f6=0,_Tmp7->f7=0;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});}}};}
# 501
goto _LL4;}}else{_LL4:
 return({struct Cyc_List_List*_Tmp2=_region_malloc(yy,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp3=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_Tmp2->tl=_Tmp3;});_Tmp2;});};}}
# 509
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 513
if(tds!=0)
d=({struct Cyc_Parse_Declarator _Tmp0;_Tmp0.id=d.id,_Tmp0.varloc=d.varloc,({struct Cyc_List_List*_Tmp1=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_Tmp0.tms=_Tmp1;});_Tmp0;});{
# 516
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 522
if(dso!=0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 528
enum Cyc_Parse_Storage_class _Tmp0=dso->sc;switch((int)_Tmp0){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
 Cyc_Warn_err(loc,({const char*_Tmp1="bad storage class on function";_tag_fat(_Tmp1,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}}{
# 535
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f1;_Tmp3=_Tmp0.f2;_Tmp2=_Tmp0.f3;_Tmp1=_Tmp0.f4;{struct Cyc_Absyn_Tqual fn_tqual=_Tmp4;void*fn_type=_Tmp3;struct Cyc_List_List*x=_Tmp2;struct Cyc_List_List*out_atts=_Tmp1;
# 540
if(x!=0)
Cyc_Warn_warn(loc,({const char*_Tmp5="bad type params, ignoring";_tag_fat(_Tmp5,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));{
# 543
struct Cyc_Absyn_FnInfo _Tmp5;if(*((int*)fn_type)==5){_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fn_type)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp5;
# 545
{struct Cyc_List_List*args2=i.args;for(0;args2!=0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1==0){
Cyc_Warn_err(loc,({const char*_Tmp6="missing argument variable in function prototype";_tag_fat(_Tmp6,sizeof(char),48U);}),_tag_fat(0U,sizeof(void*),0));
({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=({const char*_Tmp8="?";_tag_fat(_Tmp8,sizeof(char),2U);});_Tmp7;});(*((struct _tuple8*)args2->hd)).f1=_Tmp6;});}}}
# 552
({struct Cyc_List_List*_Tmp6=Cyc_List_append(i.attributes,out_atts);i.attributes=_Tmp6;});
return({struct Cyc_Absyn_Fndecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_Tmp6->sc=sc,_Tmp6->is_inline=is_inline,_Tmp6->name=d.id,_Tmp6->body=body,_Tmp6->i=i,_Tmp6->cached_type=0,_Tmp6->param_vardecls=0,_Tmp6->fn_vardecl=0,_Tmp6->orig_scope=sc;_Tmp6;});}}else{
# 556
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_Tmp6="declarator is not a function prototype";_tag_fat(_Tmp6,sizeof(char),39U);}));};}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 560
static struct _fat_ptr Cyc_Parse_msg1={_TmpG0,_TmpG0,_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 562
static struct _fat_ptr Cyc_Parse_msg2={_TmpG1,_TmpG1,_TmpG1 + 84U};
# 569
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 572
if(s1.Signed_spec && s2.Signed_spec)
Cyc_Warn_warn(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
Cyc_Warn_warn(loc,Cyc_Parse_msg2,_tag_fat(0U,sizeof(void*),0));
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
Cyc_Warn_warn(loc,({const char*_Tmp0="too many occurrences of short in specifiers";_tag_fat(_Tmp0,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
Cyc_Warn_warn(loc,({const char*_Tmp0="too many occurrences of long in specifiers";_tag_fat(_Tmp0,sizeof(char),43U);}),_tag_fat(0U,sizeof(void*),0));
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
Cyc_Warn_err(loc,Cyc_Parse_msg1,_tag_fat(0U,sizeof(void*),0));else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 594
return s1;}
# 600
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 608
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
# 621
if(!seen_type){
if(!seen_sign && !seen_size)
Cyc_Warn_warn(loc,({const char*_Tmp0="missing type within specifier";_tag_fat(_Tmp0,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_int_type(sgn,sz);}
# 626
{enum Cyc_Absyn_Size_of _Tmp0;enum Cyc_Absyn_Sign _Tmp1;if(*((int*)t)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 1: _Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_Tmp1;enum Cyc_Absyn_Size_of sz2=_Tmp0;
# 628
if(seen_sign &&(int)sgn2!=(int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 632
if(seen_size &&(int)sz2!=(int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL0;}case 2:
# 636
 if(seen_size)
t=Cyc_Absyn_long_double_type;
goto _LL0;default: goto _LL5;}else{_LL5:
# 640
 if(seen_sign)
Cyc_Warn_err(loc,({const char*_Tmp2="sign specification on non-integral type";_tag_fat(_Tmp2,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
if(seen_size)
Cyc_Warn_err(loc,({const char*_Tmp2="size qualifier on non-integral type";_tag_fat(_Tmp2,sizeof(char),36U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}_LL0:;}
# 646
return t;}
# 649
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 653
if(ds==0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*q=d.id;
unsigned varloc=d.varloc;
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f1;_Tmp3=_Tmp0.f2;_Tmp2=_Tmp0.f3;_Tmp1=_Tmp0.f4;{struct Cyc_Absyn_Tqual tq2=_Tmp4;void*new_typ=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 660
struct Cyc_List_List*tl=ds->tl==0?0:({struct _RegionHandle*_Tmp5=r;struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=Cyc_Tcutil_copy_type(t);struct _tuple13*_Tmp8=ds->tl;Cyc_Parse_apply_tmss(_Tmp5,_Tmp6,_Tmp7,_Tmp8,shared_atts);});
return({struct Cyc_List_List*_Tmp5=_region_malloc(r,sizeof(struct Cyc_List_List));({struct _tuple16*_Tmp6=({struct _tuple16*_Tmp7=_region_malloc(r,sizeof(struct _tuple16));_Tmp7->f1=varloc,_Tmp7->f2=q,_Tmp7->f3=tq2,_Tmp7->f4=new_typ,_Tmp7->f5=tvs,_Tmp7->f6=atts;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=tl;_Tmp5;});}}}
# 664
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 667
if(tms==0)return({struct _tuple14 _Tmp0;_Tmp0.f1=tq,_Tmp0.f2=t,_Tmp0.f3=0,_Tmp0.f4=atts;_Tmp0;});{
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;struct Cyc_Absyn_PtrAtts _Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*zeroterm=_Tmp5;unsigned ztloc=_Tmp4;
# 670
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 670
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 1: _Tmp5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp5;void*zeroterm=_Tmp3;unsigned ztloc=_Tmp4;
# 673
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 673
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 3: _Tmp5=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp5;
# 676
unsigned _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;int _TmpC;void*_TmpD;if(*((int*)args)==1){_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f1;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f2;_TmpB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f3;_TmpA=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f4;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f5;_Tmp8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f6;_Tmp7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f7;{struct Cyc_List_List*args2=_TmpD;int c_vararg=_TmpC;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpB;void*eff=_TmpA;struct Cyc_List_List*rgn_po=_Tmp9;struct Cyc_Absyn_Exp*req=_Tmp8;struct Cyc_Absyn_Exp*ens=_Tmp7;
# 678
struct Cyc_List_List*typvars=0;
# 680
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as!=0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));_TmpE->hd=(void*)as->hd,_TmpE->tl=fn_atts;_TmpE;});else{
# 685
new_atts=({struct Cyc_List_List*_TmpE=_cycalloc(sizeof(struct Cyc_List_List));_TmpE->hd=(void*)as->hd,_TmpE->tl=new_atts;_TmpE;});}}}
# 687
if(tms->tl!=0){
void*_TmpE=(void*)_check_null(tms->tl)->hd;void*_TmpF;if(*((int*)_TmpE)==4){_TmpF=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_TmpE)->f1;{struct Cyc_List_List*ts=_TmpF;
# 690
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{
goto _LL12;}_LL12:;}
# 696
if(((((!c_vararg && cyc_vararg==0)&& args2!=0)&& args2->tl==0)&&(*((struct _tuple8*)args2->hd)).f1==0)&&(*((struct _tuple8*)args2->hd)).f3==Cyc_Absyn_void_type)
# 701
args2=0;{
# 706
struct Cyc_List_List*new_requires=0;
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_TmpE=(struct _tuple8*)a->hd;void*_TmpF;struct Cyc_Absyn_Tqual _Tmp10;void*_Tmp11;_Tmp11=_TmpE->f1;_Tmp10=_TmpE->f2;_TmpF=(void**)& _TmpE->f3;{struct _fat_ptr*vopt=_Tmp11;struct Cyc_Absyn_Tqual tq=_Tmp10;void**t=(void**)_TmpF;
void*_Tmp12=*t;unsigned _Tmp13;void*_Tmp14;void*_Tmp15;struct Cyc_Absyn_Tqual _Tmp16;void*_Tmp17;if(*((int*)_Tmp12)==4){_Tmp17=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp12)->f1.elt_type;_Tmp16=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp12)->f1.tq;_Tmp15=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp12)->f1.num_elts;_Tmp14=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp12)->f1.zero_term;_Tmp13=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp12)->f1.zt_loc;{void*et=_Tmp17;struct Cyc_Absyn_Tqual tq=_Tmp16;struct Cyc_Absyn_Exp*neltsopt=_Tmp15;void*zt=_Tmp14;unsigned ztloc=_Tmp13;
# 711
if(neltsopt!=0 && vopt!=0){
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0)),v->f1.Loc_n.tag=4U,v->f1.Loc_n.val=0,v->f2=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,({struct Cyc_Absyn_Exp*_Tmp18[1];({struct Cyc_Absyn_Exp*_Tmp19=Cyc_Absyn_var_exp(v,0U);_Tmp18[0]=_Tmp19;});Cyc_List_list(_tag_fat(_Tmp18,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);
new_requires=({struct Cyc_List_List*_Tmp18=_cycalloc(sizeof(struct Cyc_List_List));_Tmp18->hd=new_req,_Tmp18->tl=new_requires;_Tmp18;});}}
# 718
goto _LL1A;}}else{
goto _LL1A;}_LL1A:;}}}
# 722
if(new_requires!=0){
struct Cyc_Absyn_Exp*r;
if(req!=0)
r=req;else{
# 727
r=(struct Cyc_Absyn_Exp*)new_requires->hd;
new_requires=new_requires->tl;}
# 730
for(1;new_requires!=0;new_requires=new_requires->tl){
r=Cyc_Absyn_and_exp(r,(struct Cyc_Absyn_Exp*)new_requires->hd,0U);}
req=r;}{
# 736
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 738
if(tags!=0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 743
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_TmpE=(struct _tuple8*)a->hd;void*_TmpF;struct Cyc_Absyn_Tqual _Tmp10;void*_Tmp11;_Tmp11=_TmpE->f1;_Tmp10=_TmpE->f2;_TmpF=(void**)& _TmpE->f3;{struct _fat_ptr*vopt=_Tmp11;struct Cyc_Absyn_Tqual tq=_Tmp10;void**t=(void**)_TmpF;
if(tags!=0)
({void*_Tmp12=Cyc_Parse_substitute_tags(tags,*t);*t=_Tmp12;});
({void*_Tmp12=Cyc_Parse_array2ptr(*t,1);*t=_Tmp12;});}}}
# 755
return({struct Cyc_Absyn_Tqual _TmpE=Cyc_Absyn_empty_tqual(tq.loc);void*_TmpF=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 755
struct Cyc_List_List*_Tmp10=new_atts;Cyc_Parse_apply_tms(_TmpE,_TmpF,_Tmp10,
# 760
_check_null(tms)->tl);});}}}}else{_Tmp6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f2;{unsigned loc=_Tmp6;
# 762
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_TmpE="function declaration without parameter types";_tag_fat(_TmpE,sizeof(char),45U);}));}};}case 4: _Tmp5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;unsigned loc=_Tmp4;
# 769
if(tms->tl==0)
return({struct _tuple14 _Tmp6;_Tmp6.f1=tq,_Tmp6.f2=t,_Tmp6.f3=ts,_Tmp6.f4=atts;_Tmp6;});
# 774
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_Tmp6="type parameters must appear before function arguments in declarator";_tag_fat(_Tmp6,sizeof(char),68U);}));}case 2: _Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 777
return({struct Cyc_Absyn_Tqual _Tmp6=tq2;void*_Tmp7=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp8;_Tmp8.elt_type=t,_Tmp8.elt_tq=tq,_Tmp8.ptr_atts=ptratts;_Tmp8;}));struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}default: _Tmp4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{unsigned loc=_Tmp4;struct Cyc_List_List*atts2=_Tmp5;
# 782
return({struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=t;struct Cyc_List_List*_Tmp8=Cyc_List_append(atts,atts2);Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}};}}
# 788
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 797
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;unsigned _Tmp5;_Tmp5=t->f1;_Tmp4=t->f2;_Tmp3=t->f3;_Tmp2=t->f4;_Tmp1=t->f5;_Tmp0=t->f6;{unsigned varloc=_Tmp5;struct _tuple0*x=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*typ=_Tmp2;struct Cyc_List_List*tvs=_Tmp1;struct Cyc_List_List*atts=_Tmp0;
# 800
Cyc_Lex_register_typedef(x);{
# 802
struct Cyc_Core_Opt*kind;
void*type;
{void*_Tmp6;if(*((int*)typ)==1){_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)typ)->f1;{struct Cyc_Core_Opt*kopt=_Tmp6;
# 806
type=0;
kind=kopt==0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}
# 811
return({void*_Tmp6=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_Tmp7->tag=8,({struct Cyc_Absyn_Typedefdecl*_Tmp8=({struct Cyc_Absyn_Typedefdecl*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));_Tmp9->name=x,_Tmp9->tvs=tvs,_Tmp9->kind=kind,_Tmp9->defn=type,_Tmp9->atts=atts,_Tmp9->tq=tq,_Tmp9->extern_c=0;_Tmp9;});_Tmp7->f1=_Tmp8;});_Tmp7;});Cyc_Absyn_new_decl(_Tmp6,loc);});}}}
# 818
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});Cyc_Absyn_new_stmt(_Tmp0,d->loc);});}
# 822
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right;})(Cyc_Parse_flatten_decl,ds,s);}
# 826
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es){
# 829
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
for(1;ds!=0;ds=ds->tl){
struct _tuple12 _Tmp0=ds->hd;void*_Tmp1;struct Cyc_Parse_Declarator _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_Parse_Declarator d=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
declarators=({struct _tuple13*_Tmp3=_region_malloc(r,sizeof(struct _tuple13));_Tmp3->tl=declarators,_Tmp3->hd=d;_Tmp3;});
exprs=({struct Cyc_List_List*_Tmp3=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp3->hd=e,_Tmp3->tl=exprs;_Tmp3;});}}
# 836
({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(exprs);*es=_Tmp0;});
({struct _tuple13*_Tmp0=({(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;})(declarators);*decls=_Tmp0;});}
# 845
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 848
struct _RegionHandle _Tmp0=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _Tmp0;_push_region(mkrgn);
{void*_Tmp1;struct Cyc_Parse_Type_specifier _Tmp2;struct Cyc_Absyn_Tqual _Tmp3;_Tmp3=ds.tq;_Tmp2=ds.type_specs;_Tmp1=ds.attributes;{struct Cyc_Absyn_Tqual tq=_Tmp3;struct Cyc_Parse_Type_specifier tss=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
if(tq.loc==0U)tq.loc=tqual_loc;
if(ds.is_inline)
Cyc_Warn_warn(loc,({const char*_Tmp4="inline qualifier on non-function definition";_tag_fat(_Tmp4,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));{
# 854
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
# 870
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 874
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es!=0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd!=0){
exps_empty=0;
break;}}}{
# 882
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators==0){
# 886
int _Tmp4;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;void*_Tmp7;switch(*((int*)base_type)){case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1)->r)){case 0: _Tmp7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
# 888
({struct Cyc_List_List*_Tmp8=Cyc_List_append(ad->attributes,atts);ad->attributes=_Tmp8;});
ad->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 1: _Tmp7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;
# 892
if(atts!=0)Cyc_Warn_err(loc,({const char*_Tmp8="attributes on enum not supported";_tag_fat(_Tmp8,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
ed->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpB->tag=7,_TmpB->f1=ed;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: _Tmp7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;
# 896
if(atts!=0)Cyc_Warn_err(loc,({const char*_Tmp8="attributes on datatypes not supported";_tag_fat(_Tmp8,sizeof(char),38U);}),_tag_fat(0U,sizeof(void*),0));
dd->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=dd;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)){case 20: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.tag==1){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f1;_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f2;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp6;struct _tuple0*n=_Tmp7;struct Cyc_List_List*ts=_Tmp5;
# 900
struct Cyc_List_List*ts2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),ad->kind=k,ad->sc=s,ad->name=n,ad->tvs=ts2,ad->impl=0,ad->attributes=0,ad->expected_mem_kind=0;
if(atts!=0)Cyc_Warn_err(loc,({const char*_Tmp8="bad attributes on type declaration";_tag_fat(_Tmp8,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{goto _LL25;}case 18: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.tag==2){_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl**tudp=_Tmp7;
# 905
if(atts!=0)Cyc_Warn_err(loc,({const char*_Tmp8="bad attributes on datatype";_tag_fat(_Tmp8,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=*tudp;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.name;_Tmp4=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.is_extensible;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{struct _tuple0*n=_Tmp7;int isx=_Tmp4;struct Cyc_List_List*ts=_Tmp5;
# 908
struct Cyc_List_List*ts2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts!=0)Cyc_Warn_err(loc,({const char*_Tmp8="bad attributes on datatype";_tag_fat(_Tmp8,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),_Tmp8->hd=tud,_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 15: _Tmp7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct _tuple0*n=_Tmp7;
# 913
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,ed->name=n,ed->fields=0;
if(atts!=0)Cyc_Warn_err(loc,({const char*_Tmp8="bad attributes on enum";_tag_fat(_Tmp8,sizeof(char),23U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 16: _Tmp7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct Cyc_List_List*fs=_Tmp7;
# 919
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,({struct _tuple0*_Tmp8=Cyc_Parse_gensym_enum();ed->name=_Tmp8;}),({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=fs;_Tmp9;});ed->fields=_Tmp8;});
if(atts!=0)Cyc_Warn_err(loc,({const char*_Tmp8="bad attributes on enum";_tag_fat(_Tmp8,sizeof(char),23U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: goto _LL25;}default: _LL25:
 Cyc_Warn_err(loc,({const char*_Tmp8="missing declarator";_tag_fat(_Tmp8,sizeof(char),19U);}),_tag_fat(0U,sizeof(void*),0));{struct Cyc_List_List*_Tmp8=0;_npop_handler(0);return _Tmp8;}};}{
# 926
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);
if(istypedef){
# 931
if(!exps_empty)
Cyc_Warn_err(loc,({const char*_Tmp4="initializer in typedef declaration";_tag_fat(_Tmp4,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*decls=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple16*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_Tmp4=decls;_npop_handler(0);return _Tmp4;}}{
# 937
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*ds=fields;for(0;ds!=0;(ds=ds->tl,exprs=_check_null(exprs)->tl)){
struct _tuple16*_Tmp4=(struct _tuple16*)ds->hd;void*_Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;unsigned _TmpA;_TmpA=_Tmp4->f1;_Tmp9=_Tmp4->f2;_Tmp8=_Tmp4->f3;_Tmp7=_Tmp4->f4;_Tmp6=_Tmp4->f5;_Tmp5=_Tmp4->f6;{unsigned varloc=_TmpA;struct _tuple0*x=_Tmp9;struct Cyc_Absyn_Tqual tq2=_Tmp8;void*t2=_Tmp7;struct Cyc_List_List*tvs2=_Tmp6;struct Cyc_List_List*atts2=_Tmp5;
if(tvs2!=0)
Cyc_Warn_warn(loc,({const char*_TmpB="bad type params, ignoring";_tag_fat(_TmpB,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
if(exprs==0)
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,({const char*_TmpB="unexpected NULL in parse!";_tag_fat(_TmpB,sizeof(char),26U);}));{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)_check_null(exprs)->hd);
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({struct Cyc_Absyn_Decl*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpF->tag=0,_TmpF->f1=vd;_TmpF;});_TmpD->r=_TmpE;}),_TmpD->loc=loc;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=decls;_TmpB;});}}}}{
# 950
struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(decls);_npop_handler(0);return _Tmp4;}}}}}}}}}
# 849
;_pop_region();}
# 954
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
long long _Tmp0;char _Tmp1;int _Tmp2;switch(x.LongLong_c.tag){case 5: _Tmp2=x.Int_c.val.f2;{int i=_Tmp2;
return(unsigned)i;}case 2: _Tmp1=x.Char_c.val.f2;{char c=_Tmp1;
return(unsigned)c;}case 6: _Tmp0=x.LongLong_c.val.f2;{long long x=_Tmp0;
# 959
unsigned long long y=(unsigned long long)x;
if(y > 4294967295U)
Cyc_Warn_err(loc,({const char*_Tmp3="integer constant too large";_tag_fat(_Tmp3,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));
return(unsigned)x;}default:
# 964
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_cnst2string(x);_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err(loc,({const char*_Tmp5="expected integer constant but found %s";_tag_fat(_Tmp5,sizeof(char),39U);}),_tag_fat(_Tmp4,sizeof(void*),1));});
return 0U;};}
# 970
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 15: _Tmp6=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*x=_Tmp6;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 974
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_Tmp7,p->loc);});}}else{goto _LL13;}case 6: _Tmp6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp6;
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_Tmp7,p->loc);});}case 9:
 return Cyc_Absyn_null_exp(p->loc);case 10: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
return Cyc_Absyn_char_exp(c,p->loc);}case 12: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp4;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3==0){_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _tuple0*x=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 981
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17: _Tmp6=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
return e;}default: _LL13:
# 986
 Cyc_Warn_err(p->loc,({const char*_Tmp7="cannot mix patterns and expressions in case";_tag_fat(_Tmp7,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_null_exp(p->loc);};}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f1;enum Cyc_Absyn_AggrKind f2;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _tuple26{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY29{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY36{int tag;struct _tuple27 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _tuple29{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY54{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY59{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY60{int tag;void*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY61{int tag;struct _tuple30*val;};struct _tuple31{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY62{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY63{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY64{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY65{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1072
static void Cyc_yythrowfail(struct _fat_ptr s){
(void*)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=s;_Tmp0;}));}static char _TmpG2[7U]="cnst_t";
# 1041 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2,_TmpG2,_TmpG2 + 7U};
union Cyc_Absyn_Cnst _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Int_tok.tag==1){_Tmp0=yy1->Int_tok.val;{union Cyc_Absyn_Cnst yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1048
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Int_tok.tag=1U,_Tmp0.Int_tok.val=yy1;_Tmp0;});}static char _TmpG3[5U]="char";
# 1042 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3,_TmpG3,_TmpG3 + 5U};
char _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Char_tok.tag==2){_Tmp0=yy1->Char_tok.val;{char yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1049
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Char_tok.tag=2U,_Tmp0.Char_tok.val=yy1;_Tmp0;});}static char _TmpG4[13U]="string_t<`H>";
# 1043 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4,_TmpG4,_TmpG4 + 13U};
struct _fat_ptr _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->String_tok.tag==3){_Tmp0=yy1->String_tok.val;{struct _fat_ptr yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1050
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.String_tok.tag=3U,_Tmp0.String_tok.val=yy1;_Tmp0;});}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1046 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG5,_TmpG5,_TmpG5 + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY1.tag==7){_Tmp0=yy1->YY1.val;{struct _tuple22*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1053
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY1.tag=7U,_Tmp0.YY1.val=yy1;_Tmp0;});}static char _TmpG6[11U]="ptrbound_t";
# 1047 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG6,_TmpG6,_TmpG6 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY2.tag==8){_Tmp0=yy1->YY2.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1054
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY2.tag=8U,_Tmp0.YY2.val=yy1;_Tmp0;});}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1048 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG7,_TmpG7,_TmpG7 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY3.tag==9){_Tmp0=yy1->YY3.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1055
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY3.tag=9U,_Tmp0.YY3.val=yy1;_Tmp0;});}static char _TmpG8[6U]="exp_t";
# 1049 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG8,_TmpG8,_TmpG8 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Exp_tok.tag==5){_Tmp0=yy1->Exp_tok.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1056
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Exp_tok.tag=5U,_Tmp0.Exp_tok.val=yy1;_Tmp0;});}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG9,_TmpG9,_TmpG9 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY4.tag==10){_Tmp0=yy1->YY4.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1064
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY4.tag=10U,_Tmp0.YY4.val=yy1;_Tmp0;});}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1058 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGA,_TmpGA,_TmpGA + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY5.tag==11){_Tmp0=yy1->YY5.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1065
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY5.tag=11U,_Tmp0.YY5.val=yy1;_Tmp0;});}static char _TmpGB[9U]="primop_t";
# 1059 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGB,_TmpGB,_TmpGB + 9U};
enum Cyc_Absyn_Primop _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY6.tag==12){_Tmp0=yy1->YY6.val;{enum Cyc_Absyn_Primop yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1066
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY6.tag=12U,_Tmp0.YY6.val=yy1;_Tmp0;});}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1060 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGC,_TmpGC,_TmpGC + 19U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY7.tag==13){_Tmp0=yy1->YY7.val;{struct Cyc_Core_Opt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1067
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY7.tag=13U,_Tmp0.YY7.val=yy1;_Tmp0;});}static char _TmpGD[7U]="qvar_t";
# 1061 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGD,_TmpGD,_TmpGD + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->QualId_tok.tag==4){_Tmp0=yy1->QualId_tok.val;{struct _tuple0*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1068
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.QualId_tok.tag=4U,_Tmp0.QualId_tok.val=yy1;_Tmp0;});}static char _TmpGE[7U]="stmt_t";
# 1064 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGE,_TmpGE,_TmpGE + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Stmt_tok.tag==6){_Tmp0=yy1->Stmt_tok.val;{struct Cyc_Absyn_Stmt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1071
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Stmt_tok.tag=6U,_Tmp0.Stmt_tok.val=yy1;_Tmp0;});}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1067 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGF,_TmpGF,_TmpGF + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY8.tag==14){_Tmp0=yy1->YY8.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1074
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY8.tag=14U,_Tmp0.YY8.val=yy1;_Tmp0;});}static char _TmpG10[6U]="pat_t";
# 1068 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG10,_TmpG10,_TmpG10 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY9.tag==15){_Tmp0=yy1->YY9.val;{struct Cyc_Absyn_Pat*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1075
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY9.tag=15U,_Tmp0.YY9.val=yy1;_Tmp0;});}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1073 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG11,_TmpG11,_TmpG11 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY10.tag==16){_Tmp0=yy1->YY10.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1080
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY10.tag=16U,_Tmp0.YY10.val=yy1;_Tmp0;});}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1074 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG12,_TmpG12,_TmpG12 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY11.tag==17){_Tmp0=yy1->YY11.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1081
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY11.tag=17U,_Tmp0.YY11.val=yy1;_Tmp0;});}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1075 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG13,_TmpG13,_TmpG13 + 36U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY12.tag==18){_Tmp0=yy1->YY12.val;{struct _tuple24*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1082
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY12.tag=18U,_Tmp0.YY12.val=yy1;_Tmp0;});}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1076 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG14,_TmpG14,_TmpG14 + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY13.tag==19){_Tmp0=yy1->YY13.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1083
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY13.tag=19U,_Tmp0.YY13.val=yy1;_Tmp0;});}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1077 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG15,_TmpG15,_TmpG15 + 58U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY14.tag==20){_Tmp0=yy1->YY14.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1084
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY14.tag=20U,_Tmp0.YY14.val=yy1;_Tmp0;});}static char _TmpG16[9U]="fndecl_t";
# 1078 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG16,_TmpG16,_TmpG16 + 9U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY15.tag==21){_Tmp0=yy1->YY15.val;{struct Cyc_Absyn_Fndecl*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1085
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY15.tag=21U,_Tmp0.YY15.val=yy1;_Tmp0;});}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1079 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG17,_TmpG17,_TmpG17 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY16.tag==22){_Tmp0=yy1->YY16.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1086
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY16.tag=22U,_Tmp0.YY16.val=yy1;_Tmp0;});}static char _TmpG18[12U]="decl_spec_t";
# 1082 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG18,_TmpG18,_TmpG18 + 12U};
struct Cyc_Parse_Declaration_spec _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY17.tag==23){_Tmp0=yy1->YY17.val;{struct Cyc_Parse_Declaration_spec yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1089
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY17.tag=23U,_Tmp0.YY17.val=yy1;_Tmp0;});}static char _TmpG19[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1083 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG19,_TmpG19,_TmpG19 + 31U};
struct _tuple12 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY18.tag==24){_Tmp0=yy1->YY18.val;{struct _tuple12 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1090
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY18.tag=24U,_Tmp0.YY18.val=yy1;_Tmp0;});}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1084 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1A,_TmpG1A,_TmpG1A + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY19.tag==25){_Tmp0=yy1->YY19.val;{struct _tuple11*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1091
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY19.tag=25U,_Tmp0.YY19.val=yy1;_Tmp0;});}static char _TmpG1B[16U]="storage_class_t";
# 1085 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1B,_TmpG1B,_TmpG1B + 16U};
enum Cyc_Parse_Storage_class _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY20.tag==26){_Tmp0=yy1->YY20.val;{enum Cyc_Parse_Storage_class yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1092
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY20.tag=26U,_Tmp0.YY20.val=yy1;_Tmp0;});}static char _TmpG1C[17U]="type_specifier_t";
# 1086 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1C,_TmpG1C,_TmpG1C + 17U};
struct Cyc_Parse_Type_specifier _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY21.tag==27){_Tmp0=yy1->YY21.val;{struct Cyc_Parse_Type_specifier yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1093
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY21.tag=27U,_Tmp0.YY21.val=yy1;_Tmp0;});}static char _TmpG1D[12U]="aggr_kind_t";
# 1088 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1D,_TmpG1D,_TmpG1D + 12U};
enum Cyc_Absyn_AggrKind _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY22.tag==28){_Tmp0=yy1->YY22.val;{enum Cyc_Absyn_AggrKind yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1095
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY22.tag=28U,_Tmp0.YY22.val=yy1;_Tmp0;});}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1089 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1E,_TmpG1E,_TmpG1E + 20U};
struct _tuple25 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY23.tag==29){_Tmp0=yy1->YY23.val;{struct _tuple25 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1096
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY23.tag=29U,_Tmp0.YY23.val=yy1;_Tmp0;});}static char _TmpG1F[8U]="tqual_t";
# 1090 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1F,_TmpG1F,_TmpG1F + 8U};
struct Cyc_Absyn_Tqual _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY24.tag==30){_Tmp0=yy1->YY24.val;{struct Cyc_Absyn_Tqual yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1097
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY24.tag=30U,_Tmp0.YY24.val=yy1;_Tmp0;});}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1091 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG20,_TmpG20,_TmpG20 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY25.tag==31){_Tmp0=yy1->YY25.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1098
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY25.tag=31U,_Tmp0.YY25.val=yy1;_Tmp0;});}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1092 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG21,_TmpG21,_TmpG21 + 34U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY26.tag==32){_Tmp0=yy1->YY26.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1099
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY26.tag=32U,_Tmp0.YY26.val=yy1;_Tmp0;});}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1093 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG22,_TmpG22,_TmpG22 + 33U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY27.tag==33){_Tmp0=yy1->YY27.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1100
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY27.tag=33U,_Tmp0.YY27.val=yy1;_Tmp0;});}static char _TmpG23[18U]="declarator_t<`yy>";
# 1094 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG23,_TmpG23,_TmpG23 + 18U};
struct Cyc_Parse_Declarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY28.tag==34){_Tmp0=yy1->YY28.val;{struct Cyc_Parse_Declarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1101
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY28.tag=34U,_Tmp0.YY28.val=yy1;_Tmp0;});}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1095 "parse.y"
static struct _tuple26*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG24,_TmpG24,_TmpG24 + 45U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY29.tag==35){_Tmp0=yy1->YY29.val;{struct _tuple26*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1102
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple26*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY29.tag=35U,_Tmp0.YY29.val=yy1;_Tmp0;});}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1096 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG25,_TmpG25,_TmpG25 + 57U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY30.tag==36){_Tmp0=yy1->YY30.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1103
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY30.tag=36U,_Tmp0.YY30.val=yy1;_Tmp0;});}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1097 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG26,_TmpG26,_TmpG26 + 26U};
struct Cyc_Parse_Abstractdeclarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY31.tag==37){_Tmp0=yy1->YY31.val;{struct Cyc_Parse_Abstractdeclarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1104
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY31.tag=37U,_Tmp0.YY31.val=yy1;_Tmp0;});}static char _TmpG27[5U]="bool";
# 1098 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG27,_TmpG27,_TmpG27 + 5U};
int _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY32.tag==38){_Tmp0=yy1->YY32.val;{int yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1105
static union Cyc_YYSTYPE Cyc_YY32(int yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY32.tag=38U,_Tmp0.YY32.val=yy1;_Tmp0;});}static char _TmpG28[8U]="scope_t";
# 1099 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG28,_TmpG28,_TmpG28 + 8U};
enum Cyc_Absyn_Scope _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY33.tag==39){_Tmp0=yy1->YY33.val;{enum Cyc_Absyn_Scope yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1106
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY33.tag=39U,_Tmp0.YY33.val=yy1;_Tmp0;});}static char _TmpG29[16U]="datatypefield_t";
# 1100 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG29,_TmpG29,_TmpG29 + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY34.tag==40){_Tmp0=yy1->YY34.val;{struct Cyc_Absyn_Datatypefield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1107
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY34.tag=40U,_Tmp0.YY34.val=yy1;_Tmp0;});}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1101 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2A,_TmpG2A,_TmpG2A + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY35.tag==41){_Tmp0=yy1->YY35.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1108
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY35.tag=41U,_Tmp0.YY35.val=yy1;_Tmp0;});}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1102 "parse.y"
static struct _tuple27 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2B,_TmpG2B,_TmpG2B + 41U};
struct _tuple27 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY36.tag==42){_Tmp0=yy1->YY36.val;{struct _tuple27 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1109
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple27 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY36.tag=42U,_Tmp0.YY36.val=yy1;_Tmp0;});}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1103 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2C,_TmpG2C,_TmpG2C + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY37.tag==43){_Tmp0=yy1->YY37.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1110
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY37.tag=43U,_Tmp0.YY37.val=yy1;_Tmp0;});}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1104 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2D,_TmpG2D,_TmpG2D + 31U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY38.tag==44){_Tmp0=yy1->YY38.val;{struct _tuple8*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1111
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY38.tag=44U,_Tmp0.YY38.val=yy1;_Tmp0;});}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1105 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2E,_TmpG2E,_TmpG2E + 42U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY39.tag==45){_Tmp0=yy1->YY39.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1112
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY39.tag=45U,_Tmp0.YY39.val=yy1;_Tmp0;});}static char _TmpG2F[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1106 "parse.y"
static struct _tuple28*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2F,_TmpG2F,_TmpG2F + 115U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY40.tag==46){_Tmp0=yy1->YY40.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1113
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple28*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY40.tag=46U,_Tmp0.YY40.val=yy1;_Tmp0;});}static char _TmpG30[8U]="types_t";
# 1107 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG30,_TmpG30,_TmpG30 + 8U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY41.tag==47){_Tmp0=yy1->YY41.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1114
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY41.tag=47U,_Tmp0.YY41.val=yy1;_Tmp0;});}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1109 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG31,_TmpG31,_TmpG31 + 24U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY42.tag==48){_Tmp0=yy1->YY42.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1116
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY42.tag=48U,_Tmp0.YY42.val=yy1;_Tmp0;});}static char _TmpG32[13U]="designator_t";
# 1110 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG32,_TmpG32,_TmpG32 + 13U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY43.tag==49){_Tmp0=yy1->YY43.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1117
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY43.tag=49U,_Tmp0.YY43.val=yy1;_Tmp0;});}static char _TmpG33[7U]="kind_t";
# 1111 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG33,_TmpG33,_TmpG33 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY44.tag==50){_Tmp0=yy1->YY44.val;{struct Cyc_Absyn_Kind*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1118
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY44.tag=50U,_Tmp0.YY44.val=yy1;_Tmp0;});}static char _TmpG34[7U]="type_t";
# 1112 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG34,_TmpG34,_TmpG34 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY45.tag==51){_Tmp0=yy1->YY45.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1119
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY45.tag=51U,_Tmp0.YY45.val=yy1;_Tmp0;});}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1113 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG35,_TmpG35,_TmpG35 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY46.tag==52){_Tmp0=yy1->YY46.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1120
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY46.tag=52U,_Tmp0.YY46.val=yy1;_Tmp0;});}static char _TmpG36[12U]="attribute_t";
# 1114 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG36,_TmpG36,_TmpG36 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY47.tag==53){_Tmp0=yy1->YY47.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1121
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY47.tag=53U,_Tmp0.YY47.val=yy1;_Tmp0;});}static char _TmpG37[12U]="enumfield_t";
# 1115 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG37,_TmpG37,_TmpG37 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY48.tag==54){_Tmp0=yy1->YY48.val;{struct Cyc_Absyn_Enumfield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1122
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY48.tag=54U,_Tmp0.YY48.val=yy1;_Tmp0;});}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1116 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG38,_TmpG38,_TmpG38 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY49.tag==55){_Tmp0=yy1->YY49.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1123
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY49.tag=55U,_Tmp0.YY49.val=yy1;_Tmp0;});}static char _TmpG39[11U]="type_opt_t";
# 1117 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG39,_TmpG39,_TmpG39 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY50.tag==56){_Tmp0=yy1->YY50.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1124
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY50.tag=56U,_Tmp0.YY50.val=yy1;_Tmp0;});}static char _TmpG3A[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1118 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3A,_TmpG3A,_TmpG3A + 31U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY51.tag==57){_Tmp0=yy1->YY51.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1125
static union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY51.tag=57U,_Tmp0.YY51.val=yy1;_Tmp0;});}static char _TmpG3B[11U]="booltype_t";
# 1119 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3B,_TmpG3B,_TmpG3B + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY52.tag==58){_Tmp0=yy1->YY52.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1126
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY52.tag=58U,_Tmp0.YY52.val=yy1;_Tmp0;});}static char _TmpG3C[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1120 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3C,_TmpG3C,_TmpG3C + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY53.tag==59){_Tmp0=yy1->YY53.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1127
static union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY53.tag=59U,_Tmp0.YY53.val=yy1;_Tmp0;});}static char _TmpG3D[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1121 "parse.y"
static struct _tuple29*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3D,_TmpG3D,_TmpG3D + 48U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY54.tag==60){_Tmp0=yy1->YY54.val;{struct _tuple29*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1128
static union Cyc_YYSTYPE Cyc_YY54(struct _tuple29*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY54.tag=60U,_Tmp0.YY54.val=yy1;_Tmp0;});}static char _TmpG3E[18U]="list_t<qvar_t,`H>";
# 1122 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3E,_TmpG3E,_TmpG3E + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY55.tag==61){_Tmp0=yy1->YY55.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1129
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY55.tag=61U,_Tmp0.YY55.val=yy1;_Tmp0;});}static char _TmpG3F[20U]="pointer_qual_t<`yy>";
# 1123 "parse.y"
static void*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3F,_TmpG3F,_TmpG3F + 20U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY56.tag==62){_Tmp0=yy1->YY56.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1130
static union Cyc_YYSTYPE Cyc_YY56(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY56.tag=62U,_Tmp0.YY56.val=yy1;_Tmp0;});}static char _TmpG40[21U]="pointer_quals_t<`yy>";
# 1124 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG40,_TmpG40,_TmpG40 + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY57.tag==63){_Tmp0=yy1->YY57.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1131
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY57.tag=63U,_Tmp0.YY57.val=yy1;_Tmp0;});}static char _TmpG41[10U]="exp_opt_t";
# 1125 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG41,_TmpG41,_TmpG41 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY58.tag==64){_Tmp0=yy1->YY58.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1132
static union Cyc_YYSTYPE Cyc_YY58(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY58.tag=64U,_Tmp0.YY58.val=yy1;_Tmp0;});}static char _TmpG42[23U]="$(exp_opt_t,exp_opt_t)";
# 1126 "parse.y"
static struct _tuple17 Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG42,_TmpG42,_TmpG42 + 23U};
struct _tuple17 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY59.tag==65){_Tmp0=yy1->YY59.val;{struct _tuple17 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1133
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple17 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY59.tag=65U,_Tmp0.YY59.val=yy1;_Tmp0;});}static char _TmpG43[10U]="raw_exp_t";
# 1127 "parse.y"
static void*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG43,_TmpG43,_TmpG43 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY60.tag==66){_Tmp0=yy1->YY60.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1134
static union Cyc_YYSTYPE Cyc_YY60(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY60.tag=66U,_Tmp0.YY60.val=yy1;_Tmp0;});}static char _TmpG44[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1128 "parse.y"
static struct _tuple30*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG44,_TmpG44,_TmpG44 + 112U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY61.tag==67){_Tmp0=yy1->YY61.val;{struct _tuple30*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1135
static union Cyc_YYSTYPE Cyc_YY61(struct _tuple30*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY61.tag=67U,_Tmp0.YY61.val=yy1;_Tmp0;});}static char _TmpG45[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1129 "parse.y"
static struct _tuple31*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG45,_TmpG45,_TmpG45 + 73U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY62.tag==68){_Tmp0=yy1->YY62.val;{struct _tuple31*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1136
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple31*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY62.tag=68U,_Tmp0.YY62.val=yy1;_Tmp0;});}static char _TmpG46[28U]="list_t<string_t<`H>@`H, `H>";
# 1130 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG46,_TmpG46,_TmpG46 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY63.tag==69){_Tmp0=yy1->YY63.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1137
static union Cyc_YYSTYPE Cyc_YY63(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY63.tag=69U,_Tmp0.YY63.val=yy1;_Tmp0;});}static char _TmpG47[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1131 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG47,_TmpG47,_TmpG47 + 38U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY64.tag==70){_Tmp0=yy1->YY64.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1138
static union Cyc_YYSTYPE Cyc_YY64(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY64.tag=70U,_Tmp0.YY64.val=yy1;_Tmp0;});}static char _TmpG48[26U]="$(string_t<`H>, exp_t)@`H";
# 1132 "parse.y"
static struct _tuple32*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG48,_TmpG48,_TmpG48 + 26U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY65.tag==71){_Tmp0=yy1->YY65.val;{struct _tuple32*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1139
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple32*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY65.tag=71U,_Tmp0.YY65.val=yy1;_Tmp0;});}static char _TmpG49[16U]="exp_maker_fun_t";
# 1133 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_TmpG49,_TmpG49,_TmpG49 + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY66.tag==72){_Tmp0=yy1->YY66.val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1140
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY66.tag=72U,_Tmp0.YY66.val=yy1;_Tmp0;});}struct Cyc_Yyltype{int timestamp;unsigned first_line;unsigned first_column;unsigned last_line;unsigned last_column;};
# 1156
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _Tmp0;_Tmp0.timestamp=0,_Tmp0.first_line=0U,_Tmp0.first_column=0U,_Tmp0.last_line=0U,_Tmp0.last_column=0U;_Tmp0;});}
# 1159
struct Cyc_Yyltype Cyc_yylloc={0,0U,0U,0U,0U};
# 1170 "parse.y"
static short Cyc_yytranslate[382U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,153,2,2,138,151,148,2,135,136,131,145,130,149,140,150,2,2,2,2,2,2,2,2,2,2,139,127,133,132,134,144,143,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,141,2,142,147,137,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,128,146,129,152,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126};static char _TmpG4A[2U]="$";static char _TmpG4B[6U]="error";static char _TmpG4C[12U]="$undefined.";static char _TmpG4D[5U]="AUTO";static char _TmpG4E[9U]="REGISTER";static char _TmpG4F[7U]="STATIC";static char _TmpG50[7U]="EXTERN";static char _TmpG51[8U]="TYPEDEF";static char _TmpG52[5U]="VOID";static char _TmpG53[5U]="CHAR";static char _TmpG54[6U]="SHORT";static char _TmpG55[4U]="INT";static char _TmpG56[5U]="LONG";static char _TmpG57[6U]="FLOAT";static char _TmpG58[7U]="DOUBLE";static char _TmpG59[7U]="SIGNED";static char _TmpG5A[9U]="UNSIGNED";static char _TmpG5B[6U]="CONST";static char _TmpG5C[9U]="VOLATILE";static char _TmpG5D[9U]="RESTRICT";static char _TmpG5E[7U]="STRUCT";static char _TmpG5F[6U]="UNION";static char _TmpG60[5U]="CASE";static char _TmpG61[8U]="DEFAULT";static char _TmpG62[7U]="INLINE";static char _TmpG63[7U]="SIZEOF";static char _TmpG64[9U]="OFFSETOF";static char _TmpG65[3U]="IF";static char _TmpG66[5U]="ELSE";static char _TmpG67[7U]="SWITCH";static char _TmpG68[6U]="WHILE";static char _TmpG69[3U]="DO";static char _TmpG6A[4U]="FOR";static char _TmpG6B[5U]="GOTO";static char _TmpG6C[9U]="CONTINUE";static char _TmpG6D[6U]="BREAK";static char _TmpG6E[7U]="RETURN";static char _TmpG6F[5U]="ENUM";static char _TmpG70[7U]="TYPEOF";static char _TmpG71[16U]="BUILTIN_VA_LIST";static char _TmpG72[10U]="EXTENSION";static char _TmpG73[8U]="NULL_kw";static char _TmpG74[4U]="LET";static char _TmpG75[6U]="THROW";static char _TmpG76[4U]="TRY";static char _TmpG77[6U]="CATCH";static char _TmpG78[7U]="EXPORT";static char _TmpG79[9U]="OVERRIDE";static char _TmpG7A[5U]="HIDE";static char _TmpG7B[4U]="NEW";static char _TmpG7C[9U]="ABSTRACT";static char _TmpG7D[9U]="FALLTHRU";static char _TmpG7E[6U]="USING";static char _TmpG7F[10U]="NAMESPACE";static char _TmpG80[9U]="DATATYPE";static char _TmpG81[7U]="MALLOC";static char _TmpG82[8U]="RMALLOC";static char _TmpG83[9U]="RVMALLOC";static char _TmpG84[15U]="RMALLOC_INLINE";static char _TmpG85[7U]="CALLOC";static char _TmpG86[8U]="RCALLOC";static char _TmpG87[5U]="SWAP";static char _TmpG88[9U]="REGION_T";static char _TmpG89[6U]="TAG_T";static char _TmpG8A[7U]="REGION";static char _TmpG8B[5U]="RNEW";static char _TmpG8C[8U]="REGIONS";static char _TmpG8D[7U]="PORTON";static char _TmpG8E[8U]="PORTOFF";static char _TmpG8F[7U]="PRAGMA";static char _TmpG90[10U]="TEMPESTON";static char _TmpG91[11U]="TEMPESTOFF";static char _TmpG92[8U]="NUMELTS";static char _TmpG93[8U]="VALUEOF";static char _TmpG94[10U]="VALUEOF_T";static char _TmpG95[9U]="TAGCHECK";static char _TmpG96[13U]="NUMELTS_QUAL";static char _TmpG97[10U]="THIN_QUAL";static char _TmpG98[9U]="FAT_QUAL";static char _TmpG99[13U]="NOTNULL_QUAL";static char _TmpG9A[14U]="NULLABLE_QUAL";static char _TmpG9B[14U]="REQUIRES_QUAL";static char _TmpG9C[13U]="ENSURES_QUAL";static char _TmpG9D[12U]="REGION_QUAL";static char _TmpG9E[16U]="NOZEROTERM_QUAL";static char _TmpG9F[14U]="ZEROTERM_QUAL";static char _TmpGA0[12U]="TAGGED_QUAL";static char _TmpGA1[12U]="ASSERT_QUAL";static char _TmpGA2[16U]="EXTENSIBLE_QUAL";static char _TmpGA3[18U]="AUTORELEASED_QUAL";static char _TmpGA4[7U]="PTR_OP";static char _TmpGA5[7U]="INC_OP";static char _TmpGA6[7U]="DEC_OP";static char _TmpGA7[8U]="LEFT_OP";static char _TmpGA8[9U]="RIGHT_OP";static char _TmpGA9[6U]="LE_OP";static char _TmpGAA[6U]="GE_OP";static char _TmpGAB[6U]="EQ_OP";static char _TmpGAC[6U]="NE_OP";static char _TmpGAD[7U]="AND_OP";static char _TmpGAE[6U]="OR_OP";static char _TmpGAF[11U]="MUL_ASSIGN";static char _TmpGB0[11U]="DIV_ASSIGN";static char _TmpGB1[11U]="MOD_ASSIGN";static char _TmpGB2[11U]="ADD_ASSIGN";static char _TmpGB3[11U]="SUB_ASSIGN";static char _TmpGB4[12U]="LEFT_ASSIGN";static char _TmpGB5[13U]="RIGHT_ASSIGN";static char _TmpGB6[11U]="AND_ASSIGN";static char _TmpGB7[11U]="XOR_ASSIGN";static char _TmpGB8[10U]="OR_ASSIGN";static char _TmpGB9[9U]="ELLIPSIS";static char _TmpGBA[11U]="LEFT_RIGHT";static char _TmpGBB[12U]="COLON_COLON";static char _TmpGBC[11U]="IDENTIFIER";static char _TmpGBD[17U]="INTEGER_CONSTANT";static char _TmpGBE[7U]="STRING";static char _TmpGBF[8U]="WSTRING";static char _TmpGC0[19U]="CHARACTER_CONSTANT";static char _TmpGC1[20U]="WCHARACTER_CONSTANT";static char _TmpGC2[18U]="FLOATING_CONSTANT";static char _TmpGC3[9U]="TYPE_VAR";static char _TmpGC4[13U]="TYPEDEF_NAME";static char _TmpGC5[16U]="QUAL_IDENTIFIER";static char _TmpGC6[18U]="QUAL_TYPEDEF_NAME";static char _TmpGC7[10U]="ATTRIBUTE";static char _TmpGC8[8U]="ASM_TOK";static char _TmpGC9[4U]="';'";static char _TmpGCA[4U]="'{'";static char _TmpGCB[4U]="'}'";static char _TmpGCC[4U]="','";static char _TmpGCD[4U]="'*'";static char _TmpGCE[4U]="'='";static char _TmpGCF[4U]="'<'";static char _TmpGD0[4U]="'>'";static char _TmpGD1[4U]="'('";static char _TmpGD2[4U]="')'";static char _TmpGD3[4U]="'_'";static char _TmpGD4[4U]="'$'";static char _TmpGD5[4U]="':'";static char _TmpGD6[4U]="'.'";static char _TmpGD7[4U]="'['";static char _TmpGD8[4U]="']'";static char _TmpGD9[4U]="'@'";static char _TmpGDA[4U]="'?'";static char _TmpGDB[4U]="'+'";static char _TmpGDC[4U]="'|'";static char _TmpGDD[4U]="'^'";static char _TmpGDE[4U]="'&'";static char _TmpGDF[4U]="'-'";static char _TmpGE0[4U]="'/'";static char _TmpGE1[4U]="'%'";static char _TmpGE2[4U]="'~'";static char _TmpGE3[4U]="'!'";static char _TmpGE4[5U]="prog";static char _TmpGE5[17U]="translation_unit";static char _TmpGE6[18U]="tempest_on_action";static char _TmpGE7[19U]="tempest_off_action";static char _TmpGE8[16U]="extern_c_action";static char _TmpGE9[13U]="end_extern_c";static char _TmpGEA[14U]="hide_list_opt";static char _TmpGEB[17U]="hide_list_values";static char _TmpGEC[16U]="export_list_opt";static char _TmpGED[12U]="export_list";static char _TmpGEE[19U]="export_list_values";static char _TmpGEF[13U]="override_opt";static char _TmpGF0[21U]="external_declaration";static char _TmpGF1[14U]="optional_semi";static char _TmpGF2[20U]="function_definition";static char _TmpGF3[21U]="function_definition2";static char _TmpGF4[13U]="using_action";static char _TmpGF5[15U]="unusing_action";static char _TmpGF6[17U]="namespace_action";static char _TmpGF7[19U]="unnamespace_action";static char _TmpGF8[12U]="declaration";static char _TmpGF9[9U]="open_opt";static char _TmpGFA[17U]="declaration_list";static char _TmpGFB[23U]="declaration_specifiers";static char _TmpGFC[24U]="storage_class_specifier";static char _TmpGFD[15U]="attributes_opt";static char _TmpGFE[11U]="attributes";static char _TmpGFF[15U]="attribute_list";static char _TmpG100[10U]="attribute";static char _TmpG101[15U]="type_specifier";static char _TmpG102[25U]="type_specifier_notypedef";static char _TmpG103[5U]="kind";static char _TmpG104[15U]="type_qualifier";static char _TmpG105[15U]="enum_specifier";static char _TmpG106[11U]="enum_field";static char _TmpG107[22U]="enum_declaration_list";static char _TmpG108[26U]="struct_or_union_specifier";static char _TmpG109[26U]="maybe_tagged_struct_union";static char _TmpG10A[16U]="struct_or_union";static char _TmpG10B[16U]="type_params_opt";static char _TmpG10C[24U]="struct_declaration_list";static char _TmpG10D[25U]="struct_declaration_list0";static char _TmpG10E[25U]="init_declarator_list_rev";static char _TmpG10F[16U]="init_declarator";static char _TmpG110[19U]="struct_declaration";static char _TmpG111[25U]="specifier_qualifier_list";static char _TmpG112[35U]="notypedef_specifier_qualifier_list";static char _TmpG113[27U]="struct_declarator_list_rev";static char _TmpG114[18U]="struct_declarator";static char _TmpG115[20U]="requires_clause_opt";static char _TmpG116[19U]="datatype_specifier";static char _TmpG117[14U]="qual_datatype";static char _TmpG118[19U]="datatypefield_list";static char _TmpG119[20U]="datatypefield_scope";static char _TmpG11A[14U]="datatypefield";static char _TmpG11B[11U]="declarator";static char _TmpG11C[23U]="declarator_withtypedef";static char _TmpG11D[18U]="direct_declarator";static char _TmpG11E[30U]="direct_declarator_withtypedef";static char _TmpG11F[8U]="pointer";static char _TmpG120[12U]="one_pointer";static char _TmpG121[14U]="pointer_quals";static char _TmpG122[13U]="pointer_qual";static char _TmpG123[23U]="pointer_null_and_bound";static char _TmpG124[14U]="pointer_bound";static char _TmpG125[18U]="zeroterm_qual_opt";static char _TmpG126[8U]="rgn_opt";static char _TmpG127[11U]="tqual_list";static char _TmpG128[20U]="parameter_type_list";static char _TmpG129[9U]="type_var";static char _TmpG12A[16U]="optional_effect";static char _TmpG12B[19U]="optional_rgn_order";static char _TmpG12C[10U]="rgn_order";static char _TmpG12D[16U]="optional_inject";static char _TmpG12E[11U]="effect_set";static char _TmpG12F[14U]="atomic_effect";static char _TmpG130[11U]="region_set";static char _TmpG131[15U]="parameter_list";static char _TmpG132[22U]="parameter_declaration";static char _TmpG133[16U]="identifier_list";static char _TmpG134[17U]="identifier_list0";static char _TmpG135[12U]="initializer";static char _TmpG136[18U]="array_initializer";static char _TmpG137[17U]="initializer_list";static char _TmpG138[12U]="designation";static char _TmpG139[16U]="designator_list";static char _TmpG13A[11U]="designator";static char _TmpG13B[10U]="type_name";static char _TmpG13C[14U]="any_type_name";static char _TmpG13D[15U]="type_name_list";static char _TmpG13E[20U]="abstract_declarator";static char _TmpG13F[27U]="direct_abstract_declarator";static char _TmpG140[25U]="requires_and_ensures_opt";static char _TmpG141[10U]="statement";static char _TmpG142[18U]="labeled_statement";static char _TmpG143[21U]="expression_statement";static char _TmpG144[19U]="compound_statement";static char _TmpG145[16U]="block_item_list";static char _TmpG146[20U]="selection_statement";static char _TmpG147[15U]="switch_clauses";static char _TmpG148[20U]="iteration_statement";static char _TmpG149[12U]="for_exp_opt";static char _TmpG14A[15U]="jump_statement";static char _TmpG14B[12U]="exp_pattern";static char _TmpG14C[20U]="conditional_pattern";static char _TmpG14D[19U]="logical_or_pattern";static char _TmpG14E[20U]="logical_and_pattern";static char _TmpG14F[21U]="inclusive_or_pattern";static char _TmpG150[21U]="exclusive_or_pattern";static char _TmpG151[12U]="and_pattern";static char _TmpG152[17U]="equality_pattern";static char _TmpG153[19U]="relational_pattern";static char _TmpG154[14U]="shift_pattern";static char _TmpG155[17U]="additive_pattern";static char _TmpG156[23U]="multiplicative_pattern";static char _TmpG157[13U]="cast_pattern";static char _TmpG158[14U]="unary_pattern";static char _TmpG159[16U]="postfix_pattern";static char _TmpG15A[16U]="primary_pattern";static char _TmpG15B[8U]="pattern";static char _TmpG15C[19U]="tuple_pattern_list";static char _TmpG15D[20U]="tuple_pattern_list0";static char _TmpG15E[14U]="field_pattern";static char _TmpG15F[19U]="field_pattern_list";static char _TmpG160[20U]="field_pattern_list0";static char _TmpG161[11U]="expression";static char _TmpG162[22U]="assignment_expression";static char _TmpG163[20U]="assignment_operator";static char _TmpG164[23U]="conditional_expression";static char _TmpG165[20U]="constant_expression";static char _TmpG166[22U]="logical_or_expression";static char _TmpG167[23U]="logical_and_expression";static char _TmpG168[24U]="inclusive_or_expression";static char _TmpG169[24U]="exclusive_or_expression";static char _TmpG16A[15U]="and_expression";static char _TmpG16B[20U]="equality_expression";static char _TmpG16C[22U]="relational_expression";static char _TmpG16D[17U]="shift_expression";static char _TmpG16E[20U]="additive_expression";static char _TmpG16F[26U]="multiplicative_expression";static char _TmpG170[12U]="equality_op";static char _TmpG171[14U]="relational_op";static char _TmpG172[12U]="additive_op";static char _TmpG173[18U]="multiplicative_op";static char _TmpG174[16U]="cast_expression";static char _TmpG175[17U]="unary_expression";static char _TmpG176[15U]="unary_operator";static char _TmpG177[9U]="asm_expr";static char _TmpG178[13U]="volatile_opt";static char _TmpG179[12U]="asm_out_opt";static char _TmpG17A[12U]="asm_outlist";static char _TmpG17B[11U]="asm_in_opt";static char _TmpG17C[11U]="asm_inlist";static char _TmpG17D[11U]="asm_io_elt";static char _TmpG17E[16U]="asm_clobber_opt";static char _TmpG17F[17U]="asm_clobber_list";static char _TmpG180[19U]="postfix_expression";static char _TmpG181[17U]="field_expression";static char _TmpG182[19U]="primary_expression";static char _TmpG183[25U]="argument_expression_list";static char _TmpG184[26U]="argument_expression_list0";static char _TmpG185[9U]="constant";static char _TmpG186[20U]="qual_opt_identifier";static char _TmpG187[17U]="qual_opt_typedef";static char _TmpG188[18U]="struct_union_name";static char _TmpG189[11U]="field_name";static char _TmpG18A[12U]="right_angle";
# 1524 "parse.y"
static struct _fat_ptr Cyc_yytname[321U]={{_TmpG4A,_TmpG4A,_TmpG4A + 2U},{_TmpG4B,_TmpG4B,_TmpG4B + 6U},{_TmpG4C,_TmpG4C,_TmpG4C + 12U},{_TmpG4D,_TmpG4D,_TmpG4D + 5U},{_TmpG4E,_TmpG4E,_TmpG4E + 9U},{_TmpG4F,_TmpG4F,_TmpG4F + 7U},{_TmpG50,_TmpG50,_TmpG50 + 7U},{_TmpG51,_TmpG51,_TmpG51 + 8U},{_TmpG52,_TmpG52,_TmpG52 + 5U},{_TmpG53,_TmpG53,_TmpG53 + 5U},{_TmpG54,_TmpG54,_TmpG54 + 6U},{_TmpG55,_TmpG55,_TmpG55 + 4U},{_TmpG56,_TmpG56,_TmpG56 + 5U},{_TmpG57,_TmpG57,_TmpG57 + 6U},{_TmpG58,_TmpG58,_TmpG58 + 7U},{_TmpG59,_TmpG59,_TmpG59 + 7U},{_TmpG5A,_TmpG5A,_TmpG5A + 9U},{_TmpG5B,_TmpG5B,_TmpG5B + 6U},{_TmpG5C,_TmpG5C,_TmpG5C + 9U},{_TmpG5D,_TmpG5D,_TmpG5D + 9U},{_TmpG5E,_TmpG5E,_TmpG5E + 7U},{_TmpG5F,_TmpG5F,_TmpG5F + 6U},{_TmpG60,_TmpG60,_TmpG60 + 5U},{_TmpG61,_TmpG61,_TmpG61 + 8U},{_TmpG62,_TmpG62,_TmpG62 + 7U},{_TmpG63,_TmpG63,_TmpG63 + 7U},{_TmpG64,_TmpG64,_TmpG64 + 9U},{_TmpG65,_TmpG65,_TmpG65 + 3U},{_TmpG66,_TmpG66,_TmpG66 + 5U},{_TmpG67,_TmpG67,_TmpG67 + 7U},{_TmpG68,_TmpG68,_TmpG68 + 6U},{_TmpG69,_TmpG69,_TmpG69 + 3U},{_TmpG6A,_TmpG6A,_TmpG6A + 4U},{_TmpG6B,_TmpG6B,_TmpG6B + 5U},{_TmpG6C,_TmpG6C,_TmpG6C + 9U},{_TmpG6D,_TmpG6D,_TmpG6D + 6U},{_TmpG6E,_TmpG6E,_TmpG6E + 7U},{_TmpG6F,_TmpG6F,_TmpG6F + 5U},{_TmpG70,_TmpG70,_TmpG70 + 7U},{_TmpG71,_TmpG71,_TmpG71 + 16U},{_TmpG72,_TmpG72,_TmpG72 + 10U},{_TmpG73,_TmpG73,_TmpG73 + 8U},{_TmpG74,_TmpG74,_TmpG74 + 4U},{_TmpG75,_TmpG75,_TmpG75 + 6U},{_TmpG76,_TmpG76,_TmpG76 + 4U},{_TmpG77,_TmpG77,_TmpG77 + 6U},{_TmpG78,_TmpG78,_TmpG78 + 7U},{_TmpG79,_TmpG79,_TmpG79 + 9U},{_TmpG7A,_TmpG7A,_TmpG7A + 5U},{_TmpG7B,_TmpG7B,_TmpG7B + 4U},{_TmpG7C,_TmpG7C,_TmpG7C + 9U},{_TmpG7D,_TmpG7D,_TmpG7D + 9U},{_TmpG7E,_TmpG7E,_TmpG7E + 6U},{_TmpG7F,_TmpG7F,_TmpG7F + 10U},{_TmpG80,_TmpG80,_TmpG80 + 9U},{_TmpG81,_TmpG81,_TmpG81 + 7U},{_TmpG82,_TmpG82,_TmpG82 + 8U},{_TmpG83,_TmpG83,_TmpG83 + 9U},{_TmpG84,_TmpG84,_TmpG84 + 15U},{_TmpG85,_TmpG85,_TmpG85 + 7U},{_TmpG86,_TmpG86,_TmpG86 + 8U},{_TmpG87,_TmpG87,_TmpG87 + 5U},{_TmpG88,_TmpG88,_TmpG88 + 9U},{_TmpG89,_TmpG89,_TmpG89 + 6U},{_TmpG8A,_TmpG8A,_TmpG8A + 7U},{_TmpG8B,_TmpG8B,_TmpG8B + 5U},{_TmpG8C,_TmpG8C,_TmpG8C + 8U},{_TmpG8D,_TmpG8D,_TmpG8D + 7U},{_TmpG8E,_TmpG8E,_TmpG8E + 8U},{_TmpG8F,_TmpG8F,_TmpG8F + 7U},{_TmpG90,_TmpG90,_TmpG90 + 10U},{_TmpG91,_TmpG91,_TmpG91 + 11U},{_TmpG92,_TmpG92,_TmpG92 + 8U},{_TmpG93,_TmpG93,_TmpG93 + 8U},{_TmpG94,_TmpG94,_TmpG94 + 10U},{_TmpG95,_TmpG95,_TmpG95 + 9U},{_TmpG96,_TmpG96,_TmpG96 + 13U},{_TmpG97,_TmpG97,_TmpG97 + 10U},{_TmpG98,_TmpG98,_TmpG98 + 9U},{_TmpG99,_TmpG99,_TmpG99 + 13U},{_TmpG9A,_TmpG9A,_TmpG9A + 14U},{_TmpG9B,_TmpG9B,_TmpG9B + 14U},{_TmpG9C,_TmpG9C,_TmpG9C + 13U},{_TmpG9D,_TmpG9D,_TmpG9D + 12U},{_TmpG9E,_TmpG9E,_TmpG9E + 16U},{_TmpG9F,_TmpG9F,_TmpG9F + 14U},{_TmpGA0,_TmpGA0,_TmpGA0 + 12U},{_TmpGA1,_TmpGA1,_TmpGA1 + 12U},{_TmpGA2,_TmpGA2,_TmpGA2 + 16U},{_TmpGA3,_TmpGA3,_TmpGA3 + 18U},{_TmpGA4,_TmpGA4,_TmpGA4 + 7U},{_TmpGA5,_TmpGA5,_TmpGA5 + 7U},{_TmpGA6,_TmpGA6,_TmpGA6 + 7U},{_TmpGA7,_TmpGA7,_TmpGA7 + 8U},{_TmpGA8,_TmpGA8,_TmpGA8 + 9U},{_TmpGA9,_TmpGA9,_TmpGA9 + 6U},{_TmpGAA,_TmpGAA,_TmpGAA + 6U},{_TmpGAB,_TmpGAB,_TmpGAB + 6U},{_TmpGAC,_TmpGAC,_TmpGAC + 6U},{_TmpGAD,_TmpGAD,_TmpGAD + 7U},{_TmpGAE,_TmpGAE,_TmpGAE + 6U},{_TmpGAF,_TmpGAF,_TmpGAF + 11U},{_TmpGB0,_TmpGB0,_TmpGB0 + 11U},{_TmpGB1,_TmpGB1,_TmpGB1 + 11U},{_TmpGB2,_TmpGB2,_TmpGB2 + 11U},{_TmpGB3,_TmpGB3,_TmpGB3 + 11U},{_TmpGB4,_TmpGB4,_TmpGB4 + 12U},{_TmpGB5,_TmpGB5,_TmpGB5 + 13U},{_TmpGB6,_TmpGB6,_TmpGB6 + 11U},{_TmpGB7,_TmpGB7,_TmpGB7 + 11U},{_TmpGB8,_TmpGB8,_TmpGB8 + 10U},{_TmpGB9,_TmpGB9,_TmpGB9 + 9U},{_TmpGBA,_TmpGBA,_TmpGBA + 11U},{_TmpGBB,_TmpGBB,_TmpGBB + 12U},{_TmpGBC,_TmpGBC,_TmpGBC + 11U},{_TmpGBD,_TmpGBD,_TmpGBD + 17U},{_TmpGBE,_TmpGBE,_TmpGBE + 7U},{_TmpGBF,_TmpGBF,_TmpGBF + 8U},{_TmpGC0,_TmpGC0,_TmpGC0 + 19U},{_TmpGC1,_TmpGC1,_TmpGC1 + 20U},{_TmpGC2,_TmpGC2,_TmpGC2 + 18U},{_TmpGC3,_TmpGC3,_TmpGC3 + 9U},{_TmpGC4,_TmpGC4,_TmpGC4 + 13U},{_TmpGC5,_TmpGC5,_TmpGC5 + 16U},{_TmpGC6,_TmpGC6,_TmpGC6 + 18U},{_TmpGC7,_TmpGC7,_TmpGC7 + 10U},{_TmpGC8,_TmpGC8,_TmpGC8 + 8U},{_TmpGC9,_TmpGC9,_TmpGC9 + 4U},{_TmpGCA,_TmpGCA,_TmpGCA + 4U},{_TmpGCB,_TmpGCB,_TmpGCB + 4U},{_TmpGCC,_TmpGCC,_TmpGCC + 4U},{_TmpGCD,_TmpGCD,_TmpGCD + 4U},{_TmpGCE,_TmpGCE,_TmpGCE + 4U},{_TmpGCF,_TmpGCF,_TmpGCF + 4U},{_TmpGD0,_TmpGD0,_TmpGD0 + 4U},{_TmpGD1,_TmpGD1,_TmpGD1 + 4U},{_TmpGD2,_TmpGD2,_TmpGD2 + 4U},{_TmpGD3,_TmpGD3,_TmpGD3 + 4U},{_TmpGD4,_TmpGD4,_TmpGD4 + 4U},{_TmpGD5,_TmpGD5,_TmpGD5 + 4U},{_TmpGD6,_TmpGD6,_TmpGD6 + 4U},{_TmpGD7,_TmpGD7,_TmpGD7 + 4U},{_TmpGD8,_TmpGD8,_TmpGD8 + 4U},{_TmpGD9,_TmpGD9,_TmpGD9 + 4U},{_TmpGDA,_TmpGDA,_TmpGDA + 4U},{_TmpGDB,_TmpGDB,_TmpGDB + 4U},{_TmpGDC,_TmpGDC,_TmpGDC + 4U},{_TmpGDD,_TmpGDD,_TmpGDD + 4U},{_TmpGDE,_TmpGDE,_TmpGDE + 4U},{_TmpGDF,_TmpGDF,_TmpGDF + 4U},{_TmpGE0,_TmpGE0,_TmpGE0 + 4U},{_TmpGE1,_TmpGE1,_TmpGE1 + 4U},{_TmpGE2,_TmpGE2,_TmpGE2 + 4U},{_TmpGE3,_TmpGE3,_TmpGE3 + 4U},{_TmpGE4,_TmpGE4,_TmpGE4 + 5U},{_TmpGE5,_TmpGE5,_TmpGE5 + 17U},{_TmpGE6,_TmpGE6,_TmpGE6 + 18U},{_TmpGE7,_TmpGE7,_TmpGE7 + 19U},{_TmpGE8,_TmpGE8,_TmpGE8 + 16U},{_TmpGE9,_TmpGE9,_TmpGE9 + 13U},{_TmpGEA,_TmpGEA,_TmpGEA + 14U},{_TmpGEB,_TmpGEB,_TmpGEB + 17U},{_TmpGEC,_TmpGEC,_TmpGEC + 16U},{_TmpGED,_TmpGED,_TmpGED + 12U},{_TmpGEE,_TmpGEE,_TmpGEE + 19U},{_TmpGEF,_TmpGEF,_TmpGEF + 13U},{_TmpGF0,_TmpGF0,_TmpGF0 + 21U},{_TmpGF1,_TmpGF1,_TmpGF1 + 14U},{_TmpGF2,_TmpGF2,_TmpGF2 + 20U},{_TmpGF3,_TmpGF3,_TmpGF3 + 21U},{_TmpGF4,_TmpGF4,_TmpGF4 + 13U},{_TmpGF5,_TmpGF5,_TmpGF5 + 15U},{_TmpGF6,_TmpGF6,_TmpGF6 + 17U},{_TmpGF7,_TmpGF7,_TmpGF7 + 19U},{_TmpGF8,_TmpGF8,_TmpGF8 + 12U},{_TmpGF9,_TmpGF9,_TmpGF9 + 9U},{_TmpGFA,_TmpGFA,_TmpGFA + 17U},{_TmpGFB,_TmpGFB,_TmpGFB + 23U},{_TmpGFC,_TmpGFC,_TmpGFC + 24U},{_TmpGFD,_TmpGFD,_TmpGFD + 15U},{_TmpGFE,_TmpGFE,_TmpGFE + 11U},{_TmpGFF,_TmpGFF,_TmpGFF + 15U},{_TmpG100,_TmpG100,_TmpG100 + 10U},{_TmpG101,_TmpG101,_TmpG101 + 15U},{_TmpG102,_TmpG102,_TmpG102 + 25U},{_TmpG103,_TmpG103,_TmpG103 + 5U},{_TmpG104,_TmpG104,_TmpG104 + 15U},{_TmpG105,_TmpG105,_TmpG105 + 15U},{_TmpG106,_TmpG106,_TmpG106 + 11U},{_TmpG107,_TmpG107,_TmpG107 + 22U},{_TmpG108,_TmpG108,_TmpG108 + 26U},{_TmpG109,_TmpG109,_TmpG109 + 26U},{_TmpG10A,_TmpG10A,_TmpG10A + 16U},{_TmpG10B,_TmpG10B,_TmpG10B + 16U},{_TmpG10C,_TmpG10C,_TmpG10C + 24U},{_TmpG10D,_TmpG10D,_TmpG10D + 25U},{_TmpG10E,_TmpG10E,_TmpG10E + 25U},{_TmpG10F,_TmpG10F,_TmpG10F + 16U},{_TmpG110,_TmpG110,_TmpG110 + 19U},{_TmpG111,_TmpG111,_TmpG111 + 25U},{_TmpG112,_TmpG112,_TmpG112 + 35U},{_TmpG113,_TmpG113,_TmpG113 + 27U},{_TmpG114,_TmpG114,_TmpG114 + 18U},{_TmpG115,_TmpG115,_TmpG115 + 20U},{_TmpG116,_TmpG116,_TmpG116 + 19U},{_TmpG117,_TmpG117,_TmpG117 + 14U},{_TmpG118,_TmpG118,_TmpG118 + 19U},{_TmpG119,_TmpG119,_TmpG119 + 20U},{_TmpG11A,_TmpG11A,_TmpG11A + 14U},{_TmpG11B,_TmpG11B,_TmpG11B + 11U},{_TmpG11C,_TmpG11C,_TmpG11C + 23U},{_TmpG11D,_TmpG11D,_TmpG11D + 18U},{_TmpG11E,_TmpG11E,_TmpG11E + 30U},{_TmpG11F,_TmpG11F,_TmpG11F + 8U},{_TmpG120,_TmpG120,_TmpG120 + 12U},{_TmpG121,_TmpG121,_TmpG121 + 14U},{_TmpG122,_TmpG122,_TmpG122 + 13U},{_TmpG123,_TmpG123,_TmpG123 + 23U},{_TmpG124,_TmpG124,_TmpG124 + 14U},{_TmpG125,_TmpG125,_TmpG125 + 18U},{_TmpG126,_TmpG126,_TmpG126 + 8U},{_TmpG127,_TmpG127,_TmpG127 + 11U},{_TmpG128,_TmpG128,_TmpG128 + 20U},{_TmpG129,_TmpG129,_TmpG129 + 9U},{_TmpG12A,_TmpG12A,_TmpG12A + 16U},{_TmpG12B,_TmpG12B,_TmpG12B + 19U},{_TmpG12C,_TmpG12C,_TmpG12C + 10U},{_TmpG12D,_TmpG12D,_TmpG12D + 16U},{_TmpG12E,_TmpG12E,_TmpG12E + 11U},{_TmpG12F,_TmpG12F,_TmpG12F + 14U},{_TmpG130,_TmpG130,_TmpG130 + 11U},{_TmpG131,_TmpG131,_TmpG131 + 15U},{_TmpG132,_TmpG132,_TmpG132 + 22U},{_TmpG133,_TmpG133,_TmpG133 + 16U},{_TmpG134,_TmpG134,_TmpG134 + 17U},{_TmpG135,_TmpG135,_TmpG135 + 12U},{_TmpG136,_TmpG136,_TmpG136 + 18U},{_TmpG137,_TmpG137,_TmpG137 + 17U},{_TmpG138,_TmpG138,_TmpG138 + 12U},{_TmpG139,_TmpG139,_TmpG139 + 16U},{_TmpG13A,_TmpG13A,_TmpG13A + 11U},{_TmpG13B,_TmpG13B,_TmpG13B + 10U},{_TmpG13C,_TmpG13C,_TmpG13C + 14U},{_TmpG13D,_TmpG13D,_TmpG13D + 15U},{_TmpG13E,_TmpG13E,_TmpG13E + 20U},{_TmpG13F,_TmpG13F,_TmpG13F + 27U},{_TmpG140,_TmpG140,_TmpG140 + 25U},{_TmpG141,_TmpG141,_TmpG141 + 10U},{_TmpG142,_TmpG142,_TmpG142 + 18U},{_TmpG143,_TmpG143,_TmpG143 + 21U},{_TmpG144,_TmpG144,_TmpG144 + 19U},{_TmpG145,_TmpG145,_TmpG145 + 16U},{_TmpG146,_TmpG146,_TmpG146 + 20U},{_TmpG147,_TmpG147,_TmpG147 + 15U},{_TmpG148,_TmpG148,_TmpG148 + 20U},{_TmpG149,_TmpG149,_TmpG149 + 12U},{_TmpG14A,_TmpG14A,_TmpG14A + 15U},{_TmpG14B,_TmpG14B,_TmpG14B + 12U},{_TmpG14C,_TmpG14C,_TmpG14C + 20U},{_TmpG14D,_TmpG14D,_TmpG14D + 19U},{_TmpG14E,_TmpG14E,_TmpG14E + 20U},{_TmpG14F,_TmpG14F,_TmpG14F + 21U},{_TmpG150,_TmpG150,_TmpG150 + 21U},{_TmpG151,_TmpG151,_TmpG151 + 12U},{_TmpG152,_TmpG152,_TmpG152 + 17U},{_TmpG153,_TmpG153,_TmpG153 + 19U},{_TmpG154,_TmpG154,_TmpG154 + 14U},{_TmpG155,_TmpG155,_TmpG155 + 17U},{_TmpG156,_TmpG156,_TmpG156 + 23U},{_TmpG157,_TmpG157,_TmpG157 + 13U},{_TmpG158,_TmpG158,_TmpG158 + 14U},{_TmpG159,_TmpG159,_TmpG159 + 16U},{_TmpG15A,_TmpG15A,_TmpG15A + 16U},{_TmpG15B,_TmpG15B,_TmpG15B + 8U},{_TmpG15C,_TmpG15C,_TmpG15C + 19U},{_TmpG15D,_TmpG15D,_TmpG15D + 20U},{_TmpG15E,_TmpG15E,_TmpG15E + 14U},{_TmpG15F,_TmpG15F,_TmpG15F + 19U},{_TmpG160,_TmpG160,_TmpG160 + 20U},{_TmpG161,_TmpG161,_TmpG161 + 11U},{_TmpG162,_TmpG162,_TmpG162 + 22U},{_TmpG163,_TmpG163,_TmpG163 + 20U},{_TmpG164,_TmpG164,_TmpG164 + 23U},{_TmpG165,_TmpG165,_TmpG165 + 20U},{_TmpG166,_TmpG166,_TmpG166 + 22U},{_TmpG167,_TmpG167,_TmpG167 + 23U},{_TmpG168,_TmpG168,_TmpG168 + 24U},{_TmpG169,_TmpG169,_TmpG169 + 24U},{_TmpG16A,_TmpG16A,_TmpG16A + 15U},{_TmpG16B,_TmpG16B,_TmpG16B + 20U},{_TmpG16C,_TmpG16C,_TmpG16C + 22U},{_TmpG16D,_TmpG16D,_TmpG16D + 17U},{_TmpG16E,_TmpG16E,_TmpG16E + 20U},{_TmpG16F,_TmpG16F,_TmpG16F + 26U},{_TmpG170,_TmpG170,_TmpG170 + 12U},{_TmpG171,_TmpG171,_TmpG171 + 14U},{_TmpG172,_TmpG172,_TmpG172 + 12U},{_TmpG173,_TmpG173,_TmpG173 + 18U},{_TmpG174,_TmpG174,_TmpG174 + 16U},{_TmpG175,_TmpG175,_TmpG175 + 17U},{_TmpG176,_TmpG176,_TmpG176 + 15U},{_TmpG177,_TmpG177,_TmpG177 + 9U},{_TmpG178,_TmpG178,_TmpG178 + 13U},{_TmpG179,_TmpG179,_TmpG179 + 12U},{_TmpG17A,_TmpG17A,_TmpG17A + 12U},{_TmpG17B,_TmpG17B,_TmpG17B + 11U},{_TmpG17C,_TmpG17C,_TmpG17C + 11U},{_TmpG17D,_TmpG17D,_TmpG17D + 11U},{_TmpG17E,_TmpG17E,_TmpG17E + 16U},{_TmpG17F,_TmpG17F,_TmpG17F + 17U},{_TmpG180,_TmpG180,_TmpG180 + 19U},{_TmpG181,_TmpG181,_TmpG181 + 17U},{_TmpG182,_TmpG182,_TmpG182 + 19U},{_TmpG183,_TmpG183,_TmpG183 + 25U},{_TmpG184,_TmpG184,_TmpG184 + 26U},{_TmpG185,_TmpG185,_TmpG185 + 9U},{_TmpG186,_TmpG186,_TmpG186 + 20U},{_TmpG187,_TmpG187,_TmpG187 + 17U},{_TmpG188,_TmpG188,_TmpG188 + 18U},{_TmpG189,_TmpG189,_TmpG189 + 11U},{_TmpG18A,_TmpG18A,_TmpG18A + 12U}};
# 1584
static short Cyc_yyr1[549U]={0,154,155,155,155,155,155,155,155,155,155,155,155,156,157,158,159,160,160,161,161,161,162,162,163,163,163,164,164,165,165,166,166,166,167,167,168,168,168,168,169,169,170,171,172,173,174,174,174,174,174,174,175,175,176,176,177,177,177,177,177,177,177,177,177,177,177,178,178,178,178,178,178,178,179,179,180,181,181,182,182,182,182,183,183,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,184,185,186,186,186,187,187,187,188,188,189,189,189,190,190,190,191,191,192,192,193,193,194,194,195,195,196,196,197,197,198,199,199,199,199,199,199,200,200,200,200,200,200,201,201,202,202,202,202,203,203,204,204,204,205,205,206,206,206,206,207,207,207,208,208,209,209,210,210,211,211,211,211,211,211,211,211,211,212,212,212,212,212,212,212,212,212,212,213,213,214,215,215,216,216,216,216,216,216,216,216,216,217,217,217,218,218,219,219,219,220,220,220,221,221,222,222,222,222,222,223,223,224,224,225,225,226,226,227,227,228,228,229,229,229,229,230,230,231,231,232,232,232,233,234,234,235,235,236,236,236,236,236,237,237,237,237,238,238,239,239,240,240,241,241,242,242,242,242,242,243,243,244,244,244,245,245,245,245,245,245,245,245,245,246,246,246,246,247,247,247,247,247,247,248,249,249,250,250,251,251,251,251,251,251,251,251,252,252,252,252,252,252,253,253,253,253,253,253,254,254,254,254,255,255,256,256,256,256,256,256,256,256,257,258,258,259,259,260,260,261,261,262,262,263,263,264,264,265,265,266,266,266,267,267,268,268,269,269,270,270,270,270,270,271,272,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,273,274,274,274,275,275,276,276,277,277,277,278,278,279,279,280,280,280,281,281,281,281,281,281,281,281,281,281,281,282,282,282,282,282,282,282,283,284,284,285,285,286,286,287,287,288,288,289,289,290,290,291,291,291,292,292,293,293,294,294,295,295,295,295,296,296,297,297,297,298,298,299,299,299,299,299,299,299,299,299,299,299,299,299,299,299,299,299,299,299,299,299,299,300,300,300,300,301,302,302,303,303,303,304,304,305,305,305,306,306,307,308,308,308,309,309,310,310,310,310,310,310,310,310,310,310,310,311,311,311,311,312,312,312,312,312,312,312,312,312,312,312,313,314,314,315,315,315,315,315,316,316,317,317,318,318,319,319,320,320};
# 1642
static short Cyc_yyr2[549U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,5,4,2,0,4,4,8,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,6,6,6,9,11,4,6,6,4,2,2,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1700
static short Cyc_yydefact[1073U]={0,33,67,68,69,70,72,85,86,87,88,89,90,91,92,93,109,110,111,125,126,63,0,0,97,0,0,73,0,0,161,104,106,0,0,0,13,14,0,0,0,539,227,541,540,542,0,212,0,100,0,212,211,1,0,0,0,0,31,0,0,32,0,56,65,59,83,61,94,95,0,124,98,0,0,172,0,195,198,99,176,127,71,70,64,0,113,0,58,537,0,539,534,535,536,538,0,127,0,0,373,0,0,0,250,0,377,375,42,44,0,0,52,0,0,0,0,123,162,0,0,0,209,0,0,0,0,210,0,0,0,2,0,0,0,0,46,0,133,135,57,66,60,62,543,544,127,129,127,0,54,0,0,35,0,229,0,184,173,196,0,202,203,207,208,0,206,205,204,217,198,0,84,71,117,0,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,523,524,487,0,0,0,485,0,484,482,483,0,402,404,418,426,428,430,432,434,436,438,440,443,445,458,0,460,505,522,520,539,385,0,0,0,0,386,0,0,376,49,0,0,127,0,0,0,142,138,140,270,272,0,0,0,0,0,8,9,0,545,546,228,108,0,0,0,177,101,248,0,245,10,11,0,3,0,5,0,47,0,0,0,35,122,0,130,131,154,0,159,0,0,0,0,0,0,0,0,0,0,0,0,539,302,304,0,312,306,0,310,295,296,297,0,298,299,300,0,55,35,135,34,36,277,0,235,251,0,0,231,229,0,214,0,0,0,219,74,218,199,0,118,114,0,0,0,467,0,0,480,420,458,0,421,422,0,0,0,0,0,0,0,0,0,0,0,0,461,462,488,479,0,464,0,0,0,0,463,0,96,0,0,0,0,0,0,447,448,0,451,452,449,450,0,0,0,453,454,0,455,456,457,0,0,408,409,410,411,412,413,414,415,416,417,407,0,465,0,511,512,0,0,0,526,0,127,378,0,0,0,399,539,546,0,0,0,0,266,395,400,0,397,0,0,374,392,393,0,390,252,0,0,0,0,273,0,243,143,148,144,146,139,141,229,0,279,271,280,548,547,0,103,105,0,51,0,107,80,79,0,77,213,178,229,247,174,279,249,185,186,0,102,16,29,43,0,45,0,134,136,254,253,35,37,127,120,132,0,0,0,150,157,0,127,167,0,0,0,0,0,539,0,330,0,333,334,335,0,0,337,0,0,0,313,307,135,311,305,303,38,0,183,236,0,0,0,242,230,237,291,0,222,0,231,182,216,215,179,214,0,0,220,75,128,119,425,116,112,0,0,0,0,539,255,260,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,525,532,0,531,403,427,0,429,431,433,435,437,439,441,442,444,446,406,405,510,507,0,509,0,0,0,388,389,0,269,0,396,264,267,384,0,265,387,381,0,48,0,382,0,274,0,149,145,147,0,0,214,0,281,0,229,0,290,276,0,0,0,0,0,142,0,127,0,229,0,194,175,246,0,22,4,6,39,231,0,153,137,154,0,0,152,160,169,168,0,0,163,0,0,0,320,0,0,0,330,0,331,332,336,0,0,0,308,301,0,40,278,229,0,239,0,0,0,0,181,232,0,235,223,180,200,201,220,197,466,0,0,0,256,0,261,469,0,0,0,0,0,0,521,475,478,0,0,481,489,0,0,459,529,0,0,508,506,0,0,0,0,268,398,401,391,394,383,275,244,291,282,283,214,0,0,214,0,0,50,539,0,76,78,0,187,0,0,0,214,0,0,0,17,23,129,151,0,155,158,170,167,167,0,0,0,0,0,0,0,0,0,330,320,338,0,309,41,231,0,240,238,0,0,0,231,0,221,517,0,516,0,257,262,0,0,0,0,0,0,423,424,510,509,494,0,530,513,0,533,419,527,528,0,379,380,287,285,289,291,284,214,53,0,81,188,193,291,192,189,214,0,0,0,0,0,0,0,166,165,314,320,0,0,0,0,0,340,341,343,345,347,349,351,353,355,357,360,362,364,366,371,372,0,0,317,326,0,330,0,0,339,225,241,0,0,233,224,229,468,0,0,263,470,471,472,0,0,477,476,0,500,494,490,492,486,514,0,288,286,0,191,190,30,25,0,0,35,0,7,121,156,0,0,0,320,0,369,0,0,320,0,0,0,0,0,0,0,0,0,0,0,0,0,367,320,0,0,330,319,292,293,0,231,519,518,0,0,0,0,501,500,497,495,0,491,515,0,26,24,0,27,0,19,171,315,316,0,0,0,0,320,322,344,0,346,348,350,352,354,356,358,359,361,363,0,321,327,0,0,0,234,226,0,0,0,0,0,503,502,0,496,493,0,28,18,20,0,318,368,0,365,323,0,320,329,0,0,259,258,473,0,499,0,498,82,21,0,342,320,324,328,0,0,504,370,325,294,474,0,0,0};
# 1811
static short Cyc_yydefgoto[167U]={1070,53,54,55,56,492,865,993,784,785,937,665,57,321,58,305,59,494,60,496,61,253,151,62,63,560,245,478,479,246,66,260,247,68,174,175,69,70,71,172,283,284,137,138,285,248,460,508,509,676,72,73,680,681,682,74,510,75,484,76,77,169,170,78,122,556,336,719,640,79,328,550,711,542,546,547,454,329,269,103,104,573,499,574,434,435,436,249,322,323,641,466,710,308,309,310,311,312,313,799,314,691,315,877,878,879,880,881,882,883,884,885,886,887,888,889,890,891,892,437,446,447,438,439,440,316,209,414,210,565,211,212,213,214,215,216,217,218,219,220,387,392,397,401,221,222,223,369,370,836,923,924,982,925,984,1030,224,820,225,593,594,226,227,81,938,441,470};
# 1831
static short Cyc_yypact[1073U]={2859,- -32768,- -32768,- -32768,- -32768,- 15,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3647,144,- 12,- -32768,3647,1202,- -32768,205,43,- -32768,69,128,- 28,97,150,- -32768,- -32768,177,276,260,- -32768,203,- -32768,- -32768,- -32768,243,282,652,275,306,282,- -32768,- -32768,321,343,347,2714,- -32768,227,298,- -32768,562,3647,3647,3647,- -32768,3647,- -32768,- -32768,546,361,- -32768,205,3555,168,91,147,783,- -32768,- -32768,354,382,396,- -32768,205,389,5977,- -32768,- -32768,2964,74,- -32768,- -32768,- -32768,- -32768,392,354,437,5977,- -32768,431,2964,442,446,460,- -32768,- 40,- -32768,- -32768,3809,3809,464,468,2714,2714,5977,- -32768,- -32768,32,473,5977,- -32768,585,474,32,4214,- -32768,2714,2714,3003,- -32768,2714,3003,2714,3003,- -32768,29,- -32768,3419,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,354,4214,108,1664,- -32768,3555,562,486,3809,3728,5065,- -32768,168,- -32768,485,- -32768,- -32768,- -32768,- -32768,487,- -32768,- -32768,- -32768,- 34,783,3809,- -32768,- -32768,502,516,547,205,6388,528,6487,5977,6190,539,553,567,576,580,584,591,595,597,608,615,621,6487,6487,- -32768,- -32768,729,6586,2414,630,- -32768,6586,- -32768,- -32768,- -32768,101,- -32768,- -32768,6,668,623,625,638,561,117,601,- 72,103,- -32768,930,6586,289,- 21,- -32768,643,111,- -32768,2964,119,670,1328,675,130,1000,- -32768,- -32768,679,5977,354,1000,672,3971,4214,4295,4214,427,- -32768,- 24,- 24,687,676,671,- -32768,- -32768,263,- -32768,- -32768,- -32768,- -32768,12,681,677,- -32768,- -32768,819,326,- -32768,- -32768,- -32768,683,- -32768,685,- -32768,690,- -32768,585,5179,3555,486,692,693,4214,- -32768,963,205,699,694,92,695,4366,697,719,701,708,5293,2268,4366,207,698,- -32768,- -32768,707,1815,1815,562,1815,- -32768,- -32768,- -32768,717,- -32768,- -32768,- -32768,200,- -32768,486,713,- -32768,- -32768,702,89,734,- -32768,16,720,718,229,728,633,710,5977,3809,- -32768,730,- -32768,- -32768,89,205,- -32768,5977,724,2414,- -32768,4214,2414,- -32768,- -32768,- -32768,4480,- -32768,769,5977,5977,5977,5977,5977,5977,5977,759,5977,4214,1365,5977,- -32768,- -32768,- -32768,- -32768,739,- -32768,1815,740,350,5977,- -32768,5977,- -32768,6586,5977,6586,6586,6586,6586,- -32768,- -32768,6586,- -32768,- -32768,- -32768,- -32768,6586,6586,6586,- -32768,- -32768,6586,- -32768,- -32768,- -32768,6586,5977,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5977,- -32768,32,- -32768,- -32768,5407,32,5977,- -32768,742,354,- -32768,744,745,749,- -32768,83,392,32,5977,2964,183,- -32768,- -32768,- -32768,756,761,747,2964,- -32768,- -32768,- -32768,753,766,- -32768,246,1328,763,3809,- -32768,768,773,- -32768,4295,4295,4295,- -32768,- -32768,3140,5521,366,- -32768,185,- -32768,- -32768,16,- -32768,- -32768,772,- -32768,794,- -32768,- -32768,774,775,780,- -32768,- -32768,1221,- -32768,344,532,- -32768,- -32768,- -32768,4214,- -32768,- -32768,865,- -32768,2714,- -32768,2714,- -32768,- -32768,- -32768,- -32768,486,- -32768,354,- -32768,- -32768,1142,5977,262,- -32768,- 22,273,354,716,5977,5977,779,789,5977,781,889,2117,795,- -32768,- -32768,- -32768,291,878,- -32768,5635,1966,2560,- -32768,- -32768,3419,- -32768,- -32768,- -32768,- -32768,3809,- -32768,- -32768,4214,790,4052,- -32768,- -32768,782,642,16,- -32768,4133,718,- -32768,- -32768,- -32768,- -32768,633,788,199,531,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,793,800,798,821,797,- -32768,- -32768,605,5179,801,809,810,814,815,817,372,812,822,823,293,825,839,828,6289,- -32768,- -32768,830,840,- -32768,668,210,623,625,638,561,117,601,- 72,- 72,103,- -32768,- -32768,- -32768,- -32768,- -32768,835,- -32768,90,3809,4948,4214,- -32768,4214,- -32768,831,- -32768,- -32768,- -32768,- -32768,2675,- -32768,- -32768,- -32768,2820,- -32768,845,- -32768,225,- -32768,4214,- -32768,- -32768,- -32768,842,843,633,834,185,3809,3890,5749,- -32768,- -32768,5977,853,6091,847,12,3277,849,354,3809,3728,5863,- -32768,344,- -32768,859,942,- -32768,- -32768,- -32768,718,1168,- -32768,- -32768,963,854,5977,- -32768,- -32768,- -32768,- -32768,863,205,325,373,416,5977,714,423,4366,858,5977,867,868,- -32768,- -32768,869,875,870,1815,- -32768,3555,- -32768,702,876,3809,- -32768,866,16,879,881,- -32768,- -32768,871,72,- -32768,- -32768,- -32768,- -32768,531,- -32768,885,339,885,884,- -32768,4597,- -32768,- -32768,5977,5977,5977,994,5977,6190,- -32768,- -32768,- -32768,32,32,- -32768,882,887,4714,- -32768,- -32768,5977,5977,- -32768,- -32768,89,612,910,911,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,642,- -32768,- -32768,633,89,890,633,900,479,- -32768,897,893,- -32768,- -32768,907,- -32768,89,908,912,633,904,3003,921,1003,- -32768,4214,- -32768,5977,- -32768,- -32768,917,56,716,4366,925,918,1048,919,927,4366,5977,932,5977,714,- -32768,933,- -32768,- -32768,718,226,- -32768,- -32768,5977,5977,940,718,4214,- -32768,- -32768,123,- -32768,5977,- -32768,- -32768,5179,928,929,934,931,938,- -32768,769,935,939,- 45,943,- -32768,- -32768,624,- -32768,- -32768,- -32768,- -32768,4948,- -32768,- -32768,- -32768,- -32768,- -32768,642,- -32768,633,- -32768,948,- -32768,- -32768,- -32768,642,- -32768,- -32768,633,947,391,941,2714,951,945,4214,- -32768,- -32768,1054,714,956,6685,953,2560,952,- -32768,8,- -32768,991,946,616,561,117,666,- 72,103,- -32768,- -32768,- -32768,- -32768,996,6586,1815,- -32768,- -32768,511,5977,966,967,- -32768,- -32768,- -32768,968,969,973,- -32768,876,- -32768,359,236,- -32768,- -32768,- -32768,- -32768,4214,1083,- -32768,- -32768,975,4,264,- -32768,- -32768,- -32768,- -32768,4831,- -32768,- -32768,982,- -32768,- -32768,- -32768,- -32768,988,992,408,546,- -32768,- -32768,- -32768,541,4366,995,714,2414,- -32768,4214,989,1513,6586,5977,6586,6586,6586,6586,6586,6586,6586,6586,6586,6586,5977,- -32768,714,1006,998,5977,- -32768,1045,- -32768,16,718,- -32768,- -32768,2560,1004,1007,5977,1014,270,- -32768,- -32768,1027,- -32768,- -32768,1030,- -32768,- -32768,546,- -32768,1017,409,- -32768,- -32768,- -32768,1020,1018,1023,6586,714,- -32768,668,283,623,625,625,561,117,601,- 72,- 72,103,- -32768,310,- -32768,- -32768,4366,1019,1015,- -32768,- -32768,1028,651,1022,4214,542,- -32768,1026,1027,- -32768,- -32768,1024,- -32768,- -32768,- -32768,546,- -32768,885,339,- -32768,- -32768,5977,1513,- -32768,4366,5977,- -32768,- -32768,- -32768,1025,- -32768,1043,- -32768,- -32768,- -32768,159,- -32768,714,- -32768,- -32768,1029,1036,- -32768,- -32768,- -32768,- -32768,- -32768,1164,1173,- -32768};
# 1942
static short Cyc_yypgoto[167U]={- -32768,61,- -32768,- -32768,- -32768,- -32768,- -32768,136,- -32768,- -32768,184,- -32768,- -32768,- 272,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 70,- -32768,- 111,106,- -32768,- -32768,0,523,- -32768,218,- 170,1055,33,- -32768,- -32768,- 140,- -32768,- -32768,1148,- 8,395,- -32768,- -32768,913,915,647,224,- -32768,515,- -32768,- -32768,- -32768,5,- -32768,- -32768,51,- 220,1116,- 419,115,- -32768,1032,- -32768,- -32768,1143,54,- -32768,477,- 114,- 147,- 276,- 509,231,492,499,- 433,- 494,- 124,- 440,- 117,- -32768,- 160,- 169,- 561,- 242,- -32768,776,73,- 92,- 127,- 155,- 381,- 705,- 257,- -32768,- -32768,- 71,- 300,- -32768,- 64,- -32768,- 605,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,34,965,- -32768,582,760,- -32768,- 76,653,- -32768,- 164,- 423,- 150,- 365,- 366,- 375,826,- 350,- 360,- 380,- 342,- 371,328,329,330,327,- 184,457,419,- -32768,- -32768,- -32768,- -32768,290,- -32768,- 844,237,- -32768,856,180,- -32768,- 374,- -32768,342,471,- 39,- 69,- 95,- 74};
# 1966
static short Cyc_yytable[6839U]={64,146,268,153,150,532,533,599,535,502,621,208,603,352,596,598,371,349,250,251,376,64,337,235,261,64,606,602,280,476,261,145,353,67,601,520,649,343,330,415,257,327,527,714,339,612,538,483,123,663,706,604,605,552,67,750,847,64,67,674,105,678,679,64,64,64,662,64,281,150,467,921,589,395,64,157,458,396,983,306,318,317,543,644,671,802,112,42,241,233,67,422,662,465,922,242,67,67,67,124,67,82,703,335,644,113,379,67,952,575,468,663,486,139,64,64,712,675,131,498,921,469,423,87,229,374,477,84,64,64,64,88,64,64,64,64,237,42,282,64,288,1033,760,981,544,929,258,67,67,64,380,64,953,932,259,697,277,109,157,278,786,67,67,67,449,67,67,67,67,140,141,142,67,143,264,255,256,471,564,545,152,839,67,467,67,- 164,541,1055,230,270,271,272,159,273,274,275,276,230,900,326,563,- 251,110,319,- 251,41,41,231,- 229,501,317,- 229,388,389,44,44,231,607,65,539,377,526,- 545,468,114,230,48,515,488,668,516,377,748,450,398,306,306,378,306,65,426,171,559,65,231,152,457,488,287,540,390,391,789,399,400,307,427,152,41,910,377,111,656,911,425,562,443,44,374,564,445,374,85,699,373,65,445,115,47,459,64,65,65,65,582,65,656,458,458,458,51,52,65,46,968,1066,19,20,64,911,903,154,306,155,597,64,64,908,64,156,46,796,116,67,118,623,119,455,645,41,646,610,545,432,433,613,647,537,44,319,377,67,65,65,528,717,561,620,67,67,377,67,529,564,469,614,65,65,65,746,65,65,65,65,132,133,326,65,534,551,634,759,904,464,1020,867,377,65,106,65,469,469,64,631,575,977,377,909,120,416,417,418,485,737,417,418,152,41,125,672,905,906,673,377,985,43,44,45,807,475,1031,511,545,922,88,67,743,268,506,981,121,307,307,377,307,726,616,567,694,568,569,377,1044,700,419,134,135,809,419,420,421,699,106,738,421,775,585,816,683,684,377,126,687,657,106,692,488,702,128,1045,775,690,792,258,819,793,489,457,457,457,698,259,490,701,150,1023,648,488,622,46,129,80,488,258,975,130,628,658,307,659,377,259,655,825,661,660,591,171,749,147,459,459,459,86,64,669,64,107,65,108,575,462,377,377,677,41,670,463,733,794,- 15,564,173,43,44,45,65,177,765,80,935,64,936,65,65,232,65,67,80,67,64,777,766,80,64,320,1037,871,991,1038,712,144,779,897,148,778,377,80,16,17,18,234,795,377,67,666,176,667,47,800,545,107,462,67,831,824,236,67,463,238,51,52,106,107,769,106,239,464,106,1011,1007,1008,841,832,106,80,80,1004,1006,254,65,1014,240,718,80,966,252,485,1010,80,80,80,825,80,80,80,80,1009,262,377,265,715,810,320,692,853,657,455,1012,1013,333,511,334,80,564,1063,821,152,306,808,317,488,340,974,488,345,152,348,350,350,152,377,833,834,648,341,41,967,176,564,564,1002,366,367,43,44,45,350,385,386,41,350,661,346,757,913,992,482,43,44,45,489,377,463,354,842,41,995,1053,342,350,637,638,639,824,44,825,996,355,136,751,849,752,47,393,394,763,48,64,41,64,107,356,857,107,51,52,107,44,455,965,357,65,107,65,358,47,554,555,359,48,678,679,708,709,898,360,692,51,52,361,67,362,67,724,725,797,798,487,65,901,843,844,363,943,1060,912,368,65,80,364,718,65,927,928,348,365,487,512,960,961,517,1046,956,957,375,41,381,824,382,511,424,383,267,263,44,106,46,80,1015,1050,377,64,47,106,511,384,48,511,442,1062,428,106,448,286,51,52,869,870,350,235,472,267,473,307,474,152,452,945,332,480,176,491,481,493,67,976,1042,848,495,503,851,504,692,145,306,545,513,523,514,518,893,521,522,860,524,350,530,350,350,350,350,531,862,350,279,536,469,541,350,350,350,557,566,350,46,548,549,350,160,161,162,163,164,553,64,165,166,167,379,994,374,168,583,588,615,590,1005,617,618,1059,306,998,619,564,625,627,1003,1016,629,350,626,456,692,461,64,630,635,67,633,145,1025,1017,636,1028,107,930,650,651,652,654,653,664,107,685,933,65,686,65,689,688,107,693,695,716,704,940,707,67,720,721,286,500,41,722,723,- 545,727,1043,728,729,43,44,45,730,731,821,732,734,950,47,64,145,487,482,740,487,741,735,736,463,739,51,52,350,80,744,80,106,994,745,747,106,753,758,306,764,487,761,762,770,1061,487,773,67,776,558,782,783,788,978,402,790,801,803,811,1067,804,377,145,65,307,805,326,500,815,806,576,577,578,579,580,581,742,813,584,814,822,587,829,999,835,1000,837,845,846,850,854,592,855,595,403,404,405,406,407,408,409,410,411,412,89,852,856,858,64,861,350,859,863,1024,864,868,872,873,608,896,307,895,899,902,907,413,931,914,915,917,609,918,939,916,919,592,874,875,920,934,41,67,926,941,942,944,65,946,43,44,45,949,89,954,951,955,969,47,964,970,107,506,90,1052,107,507,973,971,972,51,52,979,267,980,444,988,65,228,92,643,989,93,94,95,990,96,44,997,1001,487,1021,97,267,1029,98,350,1018,1019,99,267,100,101,106,1026,487,1027,921,487,1034,1036,90,102,1039,1048,307,791,1041,1040,1047,1054,1049,1051,1065,1056,1064,228,92,1071,1068,93,94,95,65,96,44,1069,1072,1057,1035,97,774,267,98,266,866,592,876,592,100,101,117,787,267,350,497,158,203,127,818,102,205,267,505,206,207,338,350,1022,817,812,451,755,600,632,624,958,986,959,963,894,962,0,1032,586,1058,0,0,0,0,0,0,500,7,8,9,10,11,12,13,14,15,16,17,18,19,20,89,0,350,0,0,0,0,0,0,0,80,0,0,41,0,22,23,24,0,0,65,43,44,45,46,107,500,350,350,0,47,0,30,0,506,0,0,0,0,41,31,32,51,52,0,0,0,43,44,45,267,0,38,0,0,0,47,768,90,456,506,0,772,267,39,0,40,0,51,52,781,0,0,91,92,0,0,93,94,95,0,96,44,0,0,0,0,97,948,324,98,144,41,80,99,592,100,101,0,42,43,44,45,46,0,326,0,102,350,47,0,0,0,482,0,49,50,0,0,463,0,51,52,0,0,0,89,0,0,0,0,0,0,0,0,500,0,0,826,827,828,0,830,0,0,0,0,0,0,0,0,0,500,0,0,840,0,0,0,0,0,0,0,89,0,0,350,144,350,350,350,350,350,350,350,350,350,350,0,0,0,0,0,0,90,0,0,0,0,0,286,190,0,0,0,0,429,0,0,430,92,0,0,93,94,95,0,431,44,0,0,0,0,97,0,350,98,0,0,144,99,267,100,101,0,432,433,0,0,0,0,0,0,102,0,500,41,92,197,198,93,94,95,0,0,44,0,0,0,0,0,0,0,0,500,0,0,347,350,0,202,0,350,0,0,0,144,0,0,0,0,0,267,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,797,798,21,178,179,289,0,290,291,292,293,294,295,296,297,22,23,24,298,89,26,181,299,0,0,0,0,182,27,300,0,0,30,183,184,185,186,187,188,0,31,32,33,189,0,0,500,190,0,0,191,192,38,193,0,0,0,0,0,0,0,0,0,0,39,194,40,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,301,92,197,198,93,94,95,42,43,44,45,46,199,302,149,0,0,200,0,0,0,201,0,49,304,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,289,0,290,291,292,293,294,295,296,297,22,23,24,298,89,26,181,299,0,0,0,0,182,27,300,0,0,30,183,184,185,186,187,188,0,31,32,33,189,0,0,0,190,0,0,191,192,38,193,0,0,0,0,0,0,0,0,0,0,39,194,40,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,301,92,197,198,93,94,95,42,43,44,45,46,199,302,149,303,0,200,0,0,0,201,0,49,304,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,289,0,290,291,292,293,294,295,296,297,22,23,24,298,89,26,181,299,0,0,0,0,182,27,300,0,0,30,183,184,185,186,187,188,0,31,32,33,189,0,0,0,190,0,0,191,192,38,193,0,0,0,0,0,0,0,0,0,0,39,194,40,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,301,92,197,198,93,94,95,42,43,44,45,46,199,302,149,0,0,200,0,0,0,201,0,49,304,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,289,0,290,291,292,293,294,295,296,297,22,23,24,298,89,26,181,299,0,0,0,0,182,27,300,0,0,30,183,184,185,186,187,188,0,31,32,33,189,0,0,0,190,0,0,191,192,38,193,0,0,0,0,0,0,0,0,0,0,39,194,40,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,519,92,197,198,93,94,95,42,43,44,45,46,199,302,149,0,0,200,0,0,0,201,0,49,304,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,298,89,26,181,0,0,0,0,0,182,27,0,0,0,30,183,184,185,186,187,188,0,31,32,33,189,0,0,0,190,0,0,191,192,38,193,0,0,0,0,0,0,0,0,0,0,39,194,40,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,42,43,44,45,46,199,0,0,0,0,200,0,0,0,201,0,49,304,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,298,89,0,0,0,0,0,0,0,0,27,0,0,0,30,183,184,185,186,187,188,0,31,32,0,0,0,0,0,190,0,0,191,192,38,193,0,0,0,0,0,0,0,0,0,0,39,194,40,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,42,43,44,45,46,199,0,0,0,0,200,0,0,0,347,0,49,304,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,180,89,0,181,0,0,0,0,0,182,0,0,0,0,30,183,184,185,186,187,188,0,31,32,0,189,0,0,0,190,0,0,191,192,38,193,0,0,0,0,0,0,0,0,0,0,39,194,40,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,42,43,44,45,46,199,0,372,0,0,200,0,0,0,201,0,49,304,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,180,89,0,181,0,0,0,0,0,182,0,0,0,0,30,183,184,185,186,187,188,0,31,32,0,189,0,0,0,190,0,0,191,192,38,193,0,0,0,0,0,0,0,0,0,0,39,194,40,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,42,43,44,45,46,199,0,0,0,0,200,0,0,0,201,0,49,304,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207,- 12,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,754,0,38,430,92,0,0,93,94,95,0,431,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,432,433,0,0,0,0,0,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,- 12,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,756,0,38,228,92,0,0,93,94,95,0,96,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,0,0,0,0,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,0,0,38,228,92,0,0,93,94,95,0,96,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,0,0,0,0,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,324,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,326,0,0,0,47,0,0,0,462,0,49,50,0,0,463,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,506,0,49,50,0,0,0,0,51,52,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,0,279,0,0,0,0,49,50,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,0,49,50,0,0,0,27,0,0,0,30,0,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,324,0,0,325,0,0,0,22,23,24,42,43,0,45,46,0,326,0,0,0,0,0,0,0,30,0,49,50,0,0,0,0,31,32,0,0,243,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,244,0,0,0,0,0,0,30,0,49,50,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,324,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,326,0,0,0,0,0,0,0,30,0,49,50,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,453,0,0,0,0,0,30,0,49,50,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,705,0,0,0,0,0,30,0,49,50,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,713,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,178,179,289,0,290,291,292,293,294,295,296,297,0,0,0,180,89,0,181,299,0,0,0,0,182,42,300,0,0,46,183,184,185,186,187,188,0,0,0,0,189,49,50,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,519,92,197,198,93,94,95,0,0,44,0,0,199,302,149,0,0,200,0,0,0,201,0,0,202,178,179,0,0,0,0,203,570,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,259,44,0,0,199,0,351,572,0,200,0,0,0,201,0,0,202,0,432,433,178,179,0,203,0,0,204,205,0,0,206,207,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,259,44,0,0,199,0,351,823,0,200,0,0,0,201,0,0,202,0,432,433,178,179,0,203,0,0,204,205,0,0,206,207,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,259,44,0,0,199,0,351,838,0,200,0,0,0,201,0,0,202,0,432,433,178,179,0,203,0,0,204,205,0,0,206,207,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,259,44,0,0,199,0,351,987,0,200,0,0,0,201,0,0,202,0,432,433,178,179,0,203,0,0,204,205,0,0,206,207,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,571,92,197,198,93,94,95,0,259,44,0,0,199,0,351,0,0,200,0,0,0,201,0,0,202,0,432,433,178,179,0,203,0,0,204,205,0,0,206,207,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,178,179,0,331,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,351,0,0,200,0,0,0,201,0,0,202,178,179,0,0,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,525,0,0,0,200,0,0,0,201,0,0,202,178,179,0,0,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,0,0,0,200,0,0,0,201,611,0,202,178,179,0,0,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,178,179,0,642,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,0,0,0,200,0,0,0,201,696,0,202,178,179,0,0,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,178,179,0,767,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,178,179,0,780,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,0,0,0,200,0,0,0,201,0,0,202,178,179,0,0,0,0,203,0,0,204,205,0,0,206,207,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,188,0,0,0,0,189,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,771,92,197,198,93,94,95,0,0,44,178,179,199,0,0,0,0,200,0,0,0,201,0,0,202,180,89,0,0,0,0,203,0,0,204,205,0,0,206,207,183,184,185,186,187,188,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,178,179,199,0,351,0,0,200,0,0,0,201,0,0,202,180,89,0,0,0,0,203,0,0,204,205,0,0,206,207,183,184,185,186,187,188,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,178,179,199,0,742,0,0,200,0,0,0,201,0,0,202,180,89,0,0,0,0,203,0,0,204,205,0,0,206,207,183,184,185,186,187,188,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,178,179,199,0,0,0,0,200,0,0,0,344,0,0,202,180,89,0,0,0,0,203,0,0,204,205,0,0,206,207,183,184,185,186,187,188,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,178,179,199,0,0,0,0,200,0,0,0,347,0,0,202,180,89,0,0,0,0,203,0,0,204,205,0,0,206,207,183,184,185,186,187,188,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,178,179,199,0,0,0,0,200,0,0,0,201,0,0,202,180,89,0,0,0,0,203,0,0,204,205,0,0,206,207,183,184,185,186,187,188,0,0,0,0,0,0,0,0,190,0,0,191,192,0,193,0,0,0,0,0,0,0,0,0,0,0,194,0,0,0,195,196,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,197,198,93,94,95,0,0,44,0,0,199,0,0,0,0,200,0,0,0,947,0,0,202,0,0,0,0,0,0,203,0,0,204,205,0,0,206,207};
# 2653
static short Cyc_yycheck[6839U]={0,70,126,74,74,305,306,382,308,281,433,87,392,182,379,381,200,181,110,111,204,21,169,99,119,25,397,387,139,17,125,70,182,0,384,292,469,177,155,223,116,155,299,552,171,419,318,267,48,489,544,393,394,329,21,616,761,57,25,81,26,5,6,63,64,65,485,67,139,139,94,116,372,145,74,75,246,149,922,149,151,151,66,464,507,690,114,121,128,97,57,112,511,248,139,135,63,64,65,48,67,116,542,137,485,133,100,74,100,351,134,551,267,62,114,115,549,139,57,279,116,145,143,135,90,201,114,21,128,129,130,25,132,133,134,135,102,121,146,139,148,985,636,139,128,850,114,114,115,149,144,151,144,858,122,529,127,114,158,130,669,128,129,130,240,132,133,134,135,63,64,65,139,67,123,114,115,251,342,326,74,742,149,94,151,129,114,1031,114,128,129,130,77,132,133,134,135,114,803,127,340,127,133,152,130,114,114,133,136,280,280,139,95,96,123,123,133,401,0,130,130,297,139,134,127,114,135,135,267,501,138,130,142,241,131,305,306,136,308,21,121,133,334,25,133,139,246,286,140,323,133,134,675,150,151,149,137,151,114,136,130,133,482,140,230,339,136,123,344,433,236,347,128,530,201,57,242,127,131,246,280,63,64,65,360,67,506,457,458,459,143,144,74,125,899,136,20,21,298,140,809,133,372,135,380,305,306,816,308,141,125,685,135,280,54,132,113,244,133,114,135,416,469,140,141,420,141,127,123,278,130,298,114,115,127,136,336,432,305,306,130,308,135,507,145,421,128,129,130,139,132,133,134,135,127,128,127,139,307,130,452,136,136,248,969,788,130,149,26,151,145,145,372,127,616,139,130,817,135,90,91,92,267,90,91,92,280,114,113,127,813,814,130,130,130,122,123,124,698,136,130,286,549,139,298,372,590,531,135,139,128,305,306,130,308,575,424,344,127,346,347,130,139,534,135,127,128,703,135,140,141,688,90,140,141,655,363,713,514,515,130,135,518,482,102,521,485,539,127,139,670,521,127,114,115,130,130,457,458,459,530,122,136,534,534,974,466,506,434,125,127,0,511,114,115,128,442,133,372,135,130,122,482,725,484,141,136,133,615,128,457,458,459,22,494,503,496,26,280,28,742,135,130,130,512,114,506,141,136,136,128,675,116,122,123,124,298,128,645,48,129,521,131,305,306,133,308,494,57,496,530,658,646,62,534,127,127,794,130,130,973,70,659,800,73,659,130,76,17,18,19,114,136,130,521,494,85,496,131,136,707,90,135,530,733,725,135,534,141,127,143,144,230,102,650,233,130,462,236,959,955,956,746,733,242,114,115,952,954,121,372,962,132,560,123,895,132,482,958,128,129,130,844,132,133,134,135,957,135,130,136,557,704,127,690,136,655,544,960,961,135,506,135,152,788,1048,721,521,698,700,700,670,130,909,673,178,530,180,181,182,534,130,737,738,644,129,114,136,177,813,814,951,195,196,122,123,124,200,97,98,114,204,662,135,630,825,938,135,122,123,124,130,130,141,135,749,114,136,136,132,223,457,458,459,844,123,928,944,135,127,617,765,619,131,93,94,642,135,698,114,700,230,135,777,233,143,144,236,123,636,894,135,494,242,496,135,131,84,85,135,135,5,6,81,82,801,135,803,143,144,135,698,135,700,129,130,22,23,267,521,804,129,130,135,868,1045,822,18,530,278,135,718,534,129,130,298,135,286,287,93,94,290,1019,147,148,135,114,99,928,146,655,128,147,126,121,123,434,125,307,963,129,130,782,131,442,670,148,135,673,114,1047,121,450,114,147,143,144,792,793,342,876,114,155,127,698,134,700,135,872,156,129,340,129,136,129,782,911,1001,764,129,128,767,129,899,863,895,973,128,127,135,135,797,135,114,780,127,379,139,381,382,383,384,135,782,387,132,129,145,114,392,393,394,142,129,397,125,136,139,401,76,77,78,79,80,136,865,83,84,85,100,939,947,89,114,135,133,136,953,134,134,1044,951,946,134,1048,129,139,951,964,136,433,130,245,969,247,895,130,129,865,136,939,977,966,130,980,434,852,135,114,135,130,136,47,442,135,861,698,128,700,30,139,450,127,45,136,135,865,145,895,136,130,284,279,114,136,114,139,136,1002,130,130,122,123,124,130,130,1041,130,136,876,131,951,991,482,135,116,485,129,136,136,141,136,143,144,507,494,136,496,626,1038,130,136,630,142,129,1045,142,506,136,136,127,1045,511,136,951,136,333,128,46,135,917,61,129,135,127,129,1060,128,130,1038,782,895,127,127,351,134,136,354,355,356,357,358,359,128,135,362,135,133,365,25,947,139,949,136,114,114,136,130,375,136,377,101,102,103,104,105,106,107,108,109,110,41,142,136,136,1045,142,590,136,128,977,48,135,128,136,402,129,951,139,127,127,121,132,115,136,136,135,414,130,128,136,136,419,25,26,136,129,114,1045,136,129,136,28,865,128,122,123,124,135,41,99,139,146,127,131,99,129,626,135,99,1027,630,139,130,136,136,143,144,25,462,135,111,130,895,114,115,463,129,118,119,120,129,122,123,129,136,655,82,128,482,116,131,675,127,136,135,489,137,138,797,136,670,135,116,673,115,129,99,148,129,135,1045,681,130,136,136,130,129,136,116,136,136,114,115,0,136,118,119,120,951,122,123,136,0,1038,991,128,654,531,131,125,786,529,135,531,137,138,39,673,542,733,278,76,145,51,718,148,149,551,284,152,153,170,746,973,713,707,242,626,383,450,435,884,923,885,888,797,887,- 1,982,364,1041,- 1,- 1,- 1,- 1,- 1,- 1,575,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,788,- 1,- 1,- 1,- 1,- 1,- 1,- 1,782,- 1,- 1,114,- 1,37,38,39,- 1,- 1,1045,122,123,124,125,797,616,813,814,- 1,131,- 1,54,- 1,135,- 1,- 1,- 1,- 1,114,62,63,143,144,- 1,- 1,- 1,122,123,124,646,- 1,74,- 1,- 1,- 1,131,647,99,655,135,- 1,652,659,86,- 1,88,- 1,143,144,660,- 1,- 1,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,- 1,- 1,- 1,128,874,111,131,863,114,865,135,685,137,138,- 1,121,122,123,124,125,- 1,127,- 1,148,894,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,141,- 1,143,144,- 1,- 1,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,725,- 1,- 1,728,729,730,- 1,732,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,742,- 1,- 1,745,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,- 1,952,939,954,955,956,957,958,959,960,961,962,963,- 1,- 1,- 1,- 1,- 1,- 1,99,- 1,- 1,- 1,- 1,- 1,786,69,- 1,- 1,- 1,- 1,111,- 1,- 1,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,1001,131,- 1,- 1,991,135,817,137,138,- 1,140,141,- 1,- 1,- 1,- 1,- 1,- 1,148,- 1,825,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,844,- 1,- 1,135,1044,- 1,138,- 1,1048,- 1,- 1,- 1,1038,- 1,- 1,- 1,- 1,- 1,868,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,60,- 1,62,63,64,65,- 1,- 1,928,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,60,- 1,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,60,- 1,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,60,- 1,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,60,- 1,62,63,64,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,60,- 1,62,63,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,60,- 1,62,63,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,60,- 1,62,63,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,74,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,88,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,99,- 1,62,63,64,- 1,- 1,67,68,- 1,70,71,111,- 1,74,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,86,- 1,88,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,140,141,- 1,- 1,- 1,- 1,- 1,- 1,148,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,121,122,123,124,125,- 1,- 1,- 1,129,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,143,144,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,99,- 1,62,63,64,- 1,- 1,67,68,- 1,70,71,111,- 1,74,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,86,- 1,88,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,148,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,121,122,123,124,125,- 1,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,143,144,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,99,- 1,62,63,64,- 1,- 1,67,68,- 1,70,71,- 1,- 1,74,114,115,- 1,- 1,118,119,120,- 1,122,123,- 1,86,- 1,88,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,148,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,121,122,123,124,125,- 1,- 1,- 1,129,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,121,122,- 1,124,125,- 1,127,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,141,- 1,143,144,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,- 1,- 1,- 1,- 1,- 1,- 1,121,122,123,124,125,- 1,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,137,138,- 1,- 1,- 1,- 1,143,144,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,121,122,- 1,124,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,137,138,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,121,122,- 1,124,125,- 1,- 1,128,37,38,39,40,- 1,- 1,- 1,- 1,137,138,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,114,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,66,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,128,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,121,122,- 1,124,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,137,138,- 1,- 1,- 1,- 1,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,88,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,121,51,- 1,- 1,125,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,137,138,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,127,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,32,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,129,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,122,123,- 1,- 1,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,140,141,25,26,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,142,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,127,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,142,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,136,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,142,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,142,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,25,26,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,60,- 1,- 1,- 1,- 1,65,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,25,26,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,40,41,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153,55,56,57,58,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,- 1,- 1,72,73,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,91,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,119,120,- 1,- 1,123,- 1,- 1,126,- 1,- 1,- 1,- 1,131,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,- 1,- 1,- 1,- 1,145,- 1,- 1,148,149,- 1,- 1,152,153};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int,int);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*,struct Cyc_Yyltype*);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple33{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _Tmp0=_new_region("yyregion");struct _RegionHandle*yyregion=& _Tmp0;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _Tmp1;_Tmp1.YYINITIALSVAL.tag=73U,_Tmp1.YYINITIALSVAL.val=0;_Tmp1;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _Tmp1=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_Tmp1),sizeof(short),_Tmp1);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _Tmp1=200U;({struct Cyc_Yystacktype*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(struct Cyc_Yystacktype)));});({{unsigned _Tmp3=200U;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i].v=yylval,({struct Cyc_Yyltype _Tmp4=Cyc_yynewloc();_Tmp2[i].l=_Tmp4;});}}0;});_Tmp2;});}),sizeof(struct Cyc_Yystacktype),200U);
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
if(yyssp_offset >= (yystacksize - 1)- 12){
# 212
if(yystacksize >= 10000){
Cyc_yyerror(({const char*_Tmp1="parser stack overflow";_tag_fat(_Tmp1,sizeof(char),22U);}),yystate,yychar);
(void*)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _Tmp1=(unsigned)yystacksize;_tag_fat(({short*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(short)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=((short*)yyss.curr)[(int)i]:(_Tmp2[i]=0);}}0;});_Tmp2;}),sizeof(short),_Tmp1);});
# 222
struct _fat_ptr yyvs1=({unsigned _Tmp1=(unsigned)yystacksize;
_tag_fat(({struct Cyc_Yystacktype*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(struct Cyc_Yystacktype)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_Tmp2[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_Tmp2;}),sizeof(struct Cyc_Yystacktype),_Tmp1);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1073U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 381?(int)Cyc_yytranslate[yychar]: 321;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 6838)||(int)Cyc_yycheck[yyn]!=yychar1)goto yydefault;
# 302
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,6839U,sizeof(short),yyn));
# 309
if(yyn < 0){
# 311
if(yyn==-32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 315
if(yyn==0)goto yyerrlab;}
# 317
if(yyn==1072){
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefact,1073U,sizeof(short),yystate));
if(yyn==0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,549U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=yyyvsp[0].v;
# 370 "cycbison.simple"
switch((int)yyn){case 1:
# 1141 "parse.y"
 yyval=yyyvsp[0].v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(& yyyvsp[0].v);
# 1144
goto _LL0;case 2:
# 1147 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 3:
# 1151 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_using();
# 1154
goto _LL0;case 4:
# 1155 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 5:
# 1158
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_namespace();
# 1161
goto _LL0;case 6:
# 1162 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 7:  {
# 1164 "parse.y"
int is_c_include=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(& yyyvsp[4].v);
struct _tuple29*_Tmp1=Cyc_yyget_YY54(& yyyvsp[5].v);unsigned _Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*exs=_Tmp3;unsigned wc=_Tmp2;
struct Cyc_List_List*hides=Cyc_yyget_YY55(& yyyvsp[6].v);
if(exs!=0 && hides!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,({const char*_Tmp4="hide list can only be used with export { * }, or no export block";_tag_fat(_Tmp4,sizeof(char),65U);}),_tag_fat(0U,sizeof(void*),0));
if((unsigned)hides && !((int)wc))
wc=yyyvsp[6].l.first_line;
if(!is_c_include){
if(exs!=0 || cycdecls!=0){
Cyc_Warn_err(yyyvsp[0].l.first_line,({const char*_Tmp4="expecting \"C include\"";_tag_fat(_Tmp4,sizeof(char),22U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f1=wc,_TmpA->f2=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}else{
# 1178
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_Tmp8->tag=11,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}}else{
# 1181
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f1=wc,_TmpA->f2=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY16(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}
# 1183
goto _LL0;}}case 8:
# 1184 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Porton_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 9:
# 1186 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Portoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 10:
# 1188 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempeston_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 11:
# 1190 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY16(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 12:
# 1191 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13:
# 1195 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14:
# 1198
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15:
# 1203 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,({const char*_Tmp2="C";_tag_fat(_Tmp2,sizeof(char),2U);}));})==0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,({const char*_Tmp2="C include";_tag_fat(_Tmp2,sizeof(char),10U);}));})==0)
yyval=Cyc_YY32(1);else{
# 1209
Cyc_Warn_err(yyyvsp[0].l.first_line,({const char*_Tmp1="expecting \"C\" or \"C include\"";_tag_fat(_Tmp1,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY32(1);}}
# 1213
goto _LL0;case 16:
# 1216 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17:
# 1220 "parse.y"
 yyval=Cyc_YY55(0);
goto _LL0;case 18:
# 1221 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 19:
# 1225 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 20:
# 1226 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 21:
# 1227 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY55(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 22:
# 1231 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));_Tmp1->f1=0,_Tmp1->f2=0U;_Tmp1;}));
goto _LL0;case 23:
# 1232 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 24:
# 1236 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY53(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0U;_Tmp1;}));
goto _LL0;case 25:
# 1237 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));_Tmp1->f1=0,_Tmp1->f2=0U;_Tmp1;}));
goto _LL0;case 26:
# 1238 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));_Tmp1->f1=0,_Tmp1->f2=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 27:
# 1243 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));_Tmp3->f1=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f2=_Tmp4;}),_Tmp3->f3=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 28:
# 1245 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));_Tmp3->f1=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f2=_Tmp4;}),_Tmp3->f3=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY53(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 29:
# 1249 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 30:
# 1250 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 31:
# 1254 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp5=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_new_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 32:
# 1255 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 33:
# 1256 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 36:
# 1266 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 37:  {
# 1268 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 38:
# 1280 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 39:  {
# 1282 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 40:  {
# 1290 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 41:  {
# 1293 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(& yyyvsp[0].v);
yyval=Cyc_YY15(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY16(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 42:
# 1298 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(& yyyvsp[1].v));yyval=yyyvsp[1].v;
goto _LL0;case 43:
# 1301
 Cyc_Lex_leave_using();
goto _LL0;case 44:
# 1304
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp1=_Tmp2;});_Tmp1;}));yyval=yyyvsp[1].v;
goto _LL0;case 45:
# 1307
 Cyc_Lex_leave_namespace();
goto _LL0;case 46:
# 1313 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);unsigned _Tmp2=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,0,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 47:
# 1315 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);struct _tuple11*_Tmp2=({(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;})(Cyc_yyget_YY19(& yyyvsp[1].v));unsigned _Tmp3=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 48:
# 1318
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_let_decl(_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 49:  {
# 1320 "parse.y"
struct Cyc_List_List*vds=0;
{struct Cyc_List_List*ids=Cyc_yyget_YY37(& yyyvsp[1].v);for(0;ids!=0;ids=ids->tl){
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0)),({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);qv->f1=_Tmp1;}),qv->f2=(struct _fat_ptr*)ids->hd;
vds=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Vardecl*_Tmp2=({struct _tuple0*_Tmp3=qv;Cyc_Absyn_new_vardecl(0U,_Tmp3,Cyc_Absyn_wildtyp(0),0);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=vds;_Tmp1;});}}
# 1325
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(vds);Cyc_Absyn_letv_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1327
goto _LL0;}case 50:
# 1330 "parse.y"
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[2].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp1;});{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[4].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=Cyc_Absyn_region_decl(tv,vd,0,yyyvsp[0].l.first_line);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1336
goto _LL0;}}case 51:  {
# 1338
struct _fat_ptr tvstring=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[1].v);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(({const char*_Tmp3="`%s";_tag_fat(_Tmp3,sizeof(char),4U);}),_tag_fat(_Tmp2,sizeof(void*),1));});
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[1].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=tvstring;_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp1;});{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[1].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Tvar*_Tmp3=tv;struct Cyc_Absyn_Vardecl*_Tmp4=vd;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_YY58(& yyyvsp[2].v);Cyc_Absyn_region_decl(_Tmp3,_Tmp4,_Tmp5,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1345
goto _LL0;}}}case 52:
# 1348 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 53:
# 1350 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,({const char*_Tmp2="open";_tag_fat(_Tmp2,sizeof(char),5U);}));})!=0)Cyc_Warn_err(yyyvsp[3].l.first_line,({const char*_Tmp1="expecting `open'";_tag_fat(_Tmp1,sizeof(char),17U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY58(Cyc_yyget_Exp_tok(& yyyvsp[3].v));
goto _LL0;case 54:
# 1354
 yyval=yyyvsp[0].v;
goto _LL0;case 55:
# 1355 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY16(& yyyvsp[1].v));}));
goto _LL0;case 56:
# 1361 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 57:  {
# 1364 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
if((int)two.sc!=7)
Cyc_Warn_warn(yyyvsp[0].l.first_line,({const char*_Tmp1="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_Tmp1,sizeof(char),73U);}),_tag_fat(0U,sizeof(void*),0));
# 1368
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1371
goto _LL0;}case 58:
# 1372 "parse.y"
 Cyc_Warn_warn(yyyvsp[0].l.first_line,({const char*_Tmp1="__extension__ keyword ignored in declaration";_tag_fat(_Tmp1,sizeof(char),45U);}),_tag_fat(0U,sizeof(void*),0));
yyval=yyyvsp[1].v;
# 1375
goto _LL0;case 59:
# 1376 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 60:  {
# 1379 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,({
struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=two.type_specs;Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,Cyc_yyget_YY21(& yyyvsp[0].v));});_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1384
goto _LL0;}case 61:
# 1385 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.tq=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 62:  {
# 1387 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.tq);});_Tmp1.tq=_Tmp2;}),_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1391
goto _LL0;}case 63:
# 1392 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=1,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 64:  {
# 1395 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=1,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1398
goto _LL0;}case 65:
# 1399 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;case 66:  {
# 1402 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(& yyyvsp[1].v);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,({
# 1405
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.attributes);});_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;}case 67:
# 1409 "parse.y"
 yyval=Cyc_YY20(4U);
goto _LL0;case 68:
# 1410 "parse.y"
 yyval=Cyc_YY20(5U);
goto _LL0;case 69:
# 1411 "parse.y"
 yyval=Cyc_YY20(3U);
goto _LL0;case 70:
# 1412 "parse.y"
 yyval=Cyc_YY20(1U);
goto _LL0;case 71:
# 1414 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,({const char*_Tmp2="C";_tag_fat(_Tmp2,sizeof(char),2U);}));})!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,({const char*_Tmp1="only extern or extern \"C\" is allowed";_tag_fat(_Tmp1,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY20(2U);
# 1418
goto _LL0;case 72:
# 1418 "parse.y"
 yyval=Cyc_YY20(0U);
goto _LL0;case 73:
# 1420 "parse.y"
 yyval=Cyc_YY20(6U);
goto _LL0;case 74:
# 1425 "parse.y"
 yyval=Cyc_YY46(0);
goto _LL0;case 75:
# 1426 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 76:
# 1430 "parse.y"
 yyval=yyyvsp[3].v;
goto _LL0;case 77:
# 1433
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 78:
# 1434 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 79:
# 1437
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Atts_parse_nullary_att(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 80:
# 1438 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81:
# 1440 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);unsigned _Tmp3=yyyvsp[2].l.first_line;Cyc_Atts_parse_unary_att(_Tmp1,_Tmp2,_Tmp3,Cyc_yyget_Exp_tok(& yyyvsp[2].v));}));
goto _LL0;case 82:
# 1442 "parse.y"
 yyval=Cyc_YY47(({unsigned _Tmp1=yyyvsp[0].l.first_line;unsigned _Tmp2=yyyvsp[2].l.first_line;struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[0].v);struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);unsigned _Tmp5=({
unsigned _Tmp6=yyyvsp[4].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[4].v));});
# 1442
Cyc_Atts_parse_format_att(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,({
# 1444
unsigned _Tmp6=yyyvsp[6].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[6].v));}));}));
goto _LL0;case 83:
# 1454 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 84:
# 1456 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_typedef_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[1].v),0,0);});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 85:
# 1460 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,yyyvsp[0].l.first_line));
goto _LL0;case 86:
# 1461 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,yyyvsp[0].l.first_line));
goto _LL0;case 87:
# 1462 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec(yyyvsp[0].l.first_line));
goto _LL0;case 88:
# 1463 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,yyyvsp[0].l.first_line));
goto _LL0;case 89:
# 1464 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec(yyyvsp[0].l.first_line));
goto _LL0;case 90:
# 1465 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,yyyvsp[0].l.first_line));
goto _LL0;case 91:
# 1466 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,yyyvsp[0].l.first_line));
goto _LL0;case 92:
# 1467 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec(yyyvsp[0].l.first_line));
goto _LL0;case 93:
# 1468 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec(yyyvsp[0].l.first_line));
goto _LL0;case 94:
# 1469 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 95:
# 1470 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 96:
# 1472 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 97:
# 1474 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_builtin_type(({const char*_Tmp2="__builtin_va_list";_tag_fat(_Tmp2,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 98:
# 1476 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 99:
# 1478 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_yyget_YY45(& yyyvsp[0].v);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 100:
# 1480 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 101:
# 1482 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(& yyyvsp[2].v)),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 102:
# 1484 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_Absyn_Type_struct));_Tmp2->tag=6,({struct Cyc_List_List*_Tmp3=({unsigned _Tmp4=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(({(struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;}),_Tmp4,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[2].v)));});_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1486
goto _LL0;case 103:
# 1487 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 104:
# 1489 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 105:
# 1491 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_yyget_YY45(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 106:
# 1493 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 107:
# 1495 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 108:
# 1500 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Kinds_id_to_kind(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 109:  {
# 1504 "parse.y"
unsigned loc=Cyc_Flags_porting_c_code?yyyvsp[0].l.first_line: 0U;
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=1,_Tmp1.q_volatile=0,_Tmp1.q_restrict=0,_Tmp1.real_const=1,_Tmp1.loc=loc;_Tmp1;}));
goto _LL0;}case 110:
# 1506 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=1,_Tmp1.q_restrict=0,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 111:
# 1507 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=0,_Tmp1.q_restrict=1,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 112:  {
# 1513 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl)),({void*_Tmp1=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Enumdecl*_Tmp3=({struct Cyc_Absyn_Enumdecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_Tmp4->sc=2U,({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp4->name=_Tmp5;}),({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY49(& yyyvsp[3].v);_Tmp6->v=_Tmp7;});_Tmp6;});_Tmp4->fields=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});ed->r=_Tmp1;}),ed->loc=yyyvsp[0].l.first_line;
# 1515
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=10,_Tmp2->f1=ed,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1517
goto _LL0;}case 113:
# 1518 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(& yyyvsp[1].v),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 114:
# 1520 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 115:
# 1526 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),_Tmp1->tag=0,_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 116:
# 1528 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->tag=_Tmp2;}),_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 117:
# 1532 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 118:
# 1533 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 119:
# 1534 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY49(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 120:
# 1540 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp2->tag=7,({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY25(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 121:  {
# 1544 "parse.y"
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_List_List*exist_ts=({unsigned _Tmp1=yyyvsp[4].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY41(& yyyvsp[4].v));});
struct Cyc_Absyn_TypeDecl*td=({enum Cyc_Absyn_AggrKind _Tmp1=Cyc_yyget_YY23(& yyyvsp[0].v).f2;struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=ts;struct Cyc_Absyn_AggrdeclImpl*_Tmp4=({
struct Cyc_List_List*_Tmp5=exist_ts;struct Cyc_List_List*_Tmp6=Cyc_yyget_YY51(& yyyvsp[5].v);struct Cyc_List_List*_Tmp7=Cyc_yyget_YY25(& yyyvsp[6].v);Cyc_Absyn_aggrdecl_impl(_Tmp5,_Tmp6,_Tmp7,Cyc_yyget_YY23(& yyyvsp[0].v).f1);});
# 1546
Cyc_Absyn_aggr_tdecl(_Tmp1,2U,_Tmp2,_Tmp3,_Tmp4,0,yyyvsp[0].l.first_line);});
# 1549
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=10,_Tmp2->f1=td,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1551
goto _LL0;}case 122:
# 1552 "parse.y"
 yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_AggrInfo _Tmp2=({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY23(& yyyvsp[0].v).f2;struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_UnknownAggr(_Tmp3,_Tmp4,Cyc_yyget_YY23(& yyyvsp[0].v).f1?({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=(void*)1;_Tmp5;}): 0);});Cyc_Absyn_aggr_type(_Tmp2,
Cyc_yyget_YY41(& yyyvsp[2].v));});
# 1552
Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1554
goto _LL0;case 123:
# 1557 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp1;_Tmp1.f1=1,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[1].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 124:
# 1558 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _Tmp1;_Tmp1.f1=0,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 125:
# 1561
 yyval=Cyc_YY22(0U);
goto _LL0;case 126:
# 1562 "parse.y"
 yyval=Cyc_YY22(1U);
goto _LL0;case 127:
# 1565
 yyval=Cyc_YY41(0);
goto _LL0;case 128:
# 1566 "parse.y"
 yyval=Cyc_YY41(Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 129:
# 1571 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 130:  {
# 1573 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*x=Cyc_yyget_YY26(& yyyvsp[0].v);for(0;x!=0;x=x->tl){
decls=Cyc_List_imp_append((struct Cyc_List_List*)x->hd,decls);}}{
# 1577
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags!=0)
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1582
goto _LL0;}}case 131:
# 1586 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 132:
# 1587 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY25(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY26(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 133:
# 1593 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple11));_Tmp1->tl=0,({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 134:
# 1595 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple11));({struct _tuple11*_Tmp2=Cyc_yyget_YY19(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;}),({struct _tuple12 _Tmp2=Cyc_yyget_YY18(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 135:
# 1599 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 136:
# 1600 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 137:  {
# 1605 "parse.y"
struct _RegionHandle _Tmp1=_new_region("temp");struct _RegionHandle*temp=& _Tmp1;_push_region(temp);
{struct _tuple27 _Tmp2=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*x=Cyc_yyget_YY30(& yyyvsp[1].v);for(0;x!=0;x=x->tl){
struct _tuple26*_Tmp6=(struct _tuple26*)x->hd;void*_Tmp7;void*_Tmp8;struct Cyc_Parse_Declarator _Tmp9;_Tmp9=_Tmp6->f1;_Tmp8=_Tmp6->f2;_Tmp7=_Tmp6->f3;{struct Cyc_Parse_Declarator d=_Tmp9;struct Cyc_Absyn_Exp*wd=_Tmp8;struct Cyc_Absyn_Exp*wh=_Tmp7;
decls=({struct _tuple13*_TmpA=_region_malloc(temp,sizeof(struct _tuple13));_TmpA->tl=decls,_TmpA->hd=d;_TmpA;});
widths_and_reqs=({struct Cyc_List_List*_TmpA=_region_malloc(temp,sizeof(struct Cyc_List_List));({struct _tuple17*_TmpB=({struct _tuple17*_TmpC=_region_malloc(temp,sizeof(struct _tuple17));_TmpC->f1=wd,_TmpC->f2=wh;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=widths_and_reqs;_TmpA;});}}}{
# 1615
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct Cyc_List_List*info=({struct _RegionHandle*_Tmp6=temp;struct _RegionHandle*_Tmp7=temp;struct Cyc_List_List*_Tmp8=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1616
Cyc_List_rzip(_Tmp6,_Tmp7,_Tmp8,widths_and_reqs);});
# 1618
yyval=Cyc_YY25(({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple18*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_make_aggr_field,yyyvsp[0].l.first_line,info));
# 1620
_npop_handler(0);goto _LL0;}}}}
# 1606
;_pop_region();}case 138:
# 1627 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 139:  {
# 1629 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;_Tmp1.f1=two.f1,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f2);});_Tmp1.f2=_Tmp2;}),_Tmp1.f3=two.f3;_Tmp1;}));
goto _LL0;}case 140:
# 1631 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 141:  {
# 1633 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2,_Tmp1.f3=two.f3;_Tmp1;}));
goto _LL0;}case 142:
# 1635 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f2=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 143:  {
# 1637 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;_Tmp1.f1=two.f1,_Tmp1.f2=two.f2,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f3);});_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;}case 144:
# 1643 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 145:  {
# 1645 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;_Tmp1.f1=two.f1,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY21(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f2);});_Tmp1.f2=_Tmp2;}),_Tmp1.f3=two.f3;_Tmp1;}));
goto _LL0;}case 146:
# 1647 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY24(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 147:  {
# 1649 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2,_Tmp1.f3=two.f3;_Tmp1;}));
goto _LL0;}case 148:
# 1651 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f2=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 149:  {
# 1653 "parse.y"
struct _tuple27 two=Cyc_yyget_YY36(& yyyvsp[1].v);yyval=Cyc_YY36(({struct _tuple27 _Tmp1;_Tmp1.f1=two.f1,_Tmp1.f2=two.f2,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f3);});_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;}case 150:
# 1659 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 151:
# 1661 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp2=Cyc_yyget_YY29(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY30(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 152:
# 1666 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple26));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_YY58(& yyyvsp[1].v);_Tmp1->f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 153:
# 1670 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple26));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f1=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=({const char*_Tmp6="";_tag_fat(_Tmp6,sizeof(char),1U);});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->f1.id=_Tmp2;}),_Tmp1->f1.varloc=0U,_Tmp1->f1.tms=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;}),_Tmp1->f3=0;_Tmp1;}));
# 1672
goto _LL0;case 154:
# 1675 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple26));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f1=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=({const char*_Tmp6="";_tag_fat(_Tmp6,sizeof(char),1U);});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->f1.id=_Tmp2;}),_Tmp1->f1.varloc=0U,_Tmp1->f1.tms=0,_Tmp1->f2=0,_Tmp1->f3=0;_Tmp1;}));
# 1677
goto _LL0;case 155:
# 1678 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple26));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f2=_Tmp2;}),_Tmp1->f3=0;_Tmp1;}));
goto _LL0;case 156:
# 1682 "parse.y"
 yyval=Cyc_YY58(Cyc_yyget_Exp_tok(& yyyvsp[2].v));
goto _LL0;case 157:
# 1683 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 158:  {
# 1689 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));});
struct Cyc_Absyn_TypeDecl*dd=({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=ts;struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY35(& yyyvsp[4].v);_Tmp4->v=_Tmp5;});_Tmp4;});int _Tmp4=is_extensible;Cyc_Absyn_datatype_tdecl(2U,_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
# 1693
yyval=Cyc_YY21(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=10,_Tmp2->f1=dd,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1695
goto _LL0;}case 159:  {
# 1696 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeInfo _Tmp2=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[2].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1699
goto _LL0;}case 160:  {
# 1700 "parse.y"
int is_extensible=Cyc_yyget_YY32(& yyyvsp[0].v);
yyval=Cyc_YY21(({void*_Tmp1=({union Cyc_Absyn_DatatypeFieldInfo _Tmp2=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.datatype_name=_Tmp4;}),({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[3].v);_Tmp3.field_name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_field_type(_Tmp2,Cyc_yyget_YY41(& yyyvsp[4].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1703
goto _LL0;}case 161:
# 1706 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 162:
# 1707 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 163:
# 1711 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 164:
# 1712 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 165:
# 1713 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 166:
# 1714 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY35(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 167:
# 1718 "parse.y"
 yyval=Cyc_YY33(2U);
goto _LL0;case 168:
# 1719 "parse.y"
 yyval=Cyc_YY33(3U);
goto _LL0;case 169:
# 1720 "parse.y"
 yyval=Cyc_YY33(0U);
goto _LL0;case 170:
# 1724 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=0,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;case 171:  {
# 1726 "parse.y"
struct Cyc_List_List*typs=({unsigned _Tmp1=yyyvsp[3].l.first_line;({(struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(({(struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;}),_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[3].v)));});
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=typs,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;}case 172:
# 1732 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 173:  {
# 1734 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 174:
# 1741 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 175:  {
# 1743 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(& yyyvsp[1].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 176:
# 1748 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 177:
# 1749 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 178:  {
# 1753 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1757
goto _LL0;}case 179:
# 1758 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY52(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 180:
# 1760 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY52(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 181:  {
# 1763 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;void*_Tmp6;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp6;int b=_Tmp5;struct Cyc_Absyn_VarargInfo*c=_Tmp4;void*eff=_Tmp3;struct Cyc_List_List*po=_Tmp2;
struct Cyc_Absyn_Exp*req=Cyc_yyget_YY59(& yyyvsp[4].v).f1;
struct Cyc_Absyn_Exp*ens=Cyc_yyget_YY59(& yyyvsp[4].v).f2;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp7;({struct _tuple0*_Tmp8=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp7.id=_Tmp8;}),({unsigned _Tmp8=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp7.varloc=_Tmp8;}),({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpA=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpB=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpB->tag=3,({void*_TmpC=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpD=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpD->tag=1,_TmpD->f1=lis,_TmpD->f2=b,_TmpD->f3=c,_TmpD->f4=eff,_TmpD->f5=po,_TmpD->f6=req,_TmpD->f7=ens;_TmpD;});_TmpB->f1=_TmpC;});_TmpB;});_Tmp9->hd=_TmpA;}),({struct Cyc_List_List*_TmpA=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp9->tl=_TmpA;});_Tmp9;});_Tmp7.tms=_Tmp8;});_Tmp7;}));
# 1768
goto _LL0;}}case 182:
# 1769 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 183:  {
# 1772
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1775
goto _LL0;}case 184:
# 1776 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY28(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 1776
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1779
goto _LL0;case 185:
# 1783 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 186:
# 1784 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 187:
# 1785 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 188:  {
# 1789 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1793
goto _LL0;}case 189:  {
# 1794 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY52(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 190:  {
# 1798 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY52(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1802
goto _LL0;}case 191:  {
# 1803 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;void*_Tmp6;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp6;int b=_Tmp5;struct Cyc_Absyn_VarargInfo*c=_Tmp4;void*eff=_Tmp3;struct Cyc_List_List*po=_Tmp2;
struct Cyc_Absyn_Exp*req=Cyc_yyget_YY59(& yyyvsp[4].v).f1;
struct Cyc_Absyn_Exp*ens=Cyc_yyget_YY59(& yyyvsp[4].v).f2;
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp7;_Tmp7.id=one.id,_Tmp7.varloc=one.varloc,({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpA=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpB=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpB->tag=3,({void*_TmpC=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpD=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpD->tag=1,_TmpD->f1=lis,_TmpD->f2=b,_TmpD->f3=c,_TmpD->f4=eff,_TmpD->f5=po,_TmpD->f6=req,_TmpD->f7=ens;_TmpD;});_TmpB->f1=_TmpC;});_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=one.tms;_Tmp9;});_Tmp7.tms=_Tmp8;});_Tmp7;}));
# 1809
goto _LL0;}}case 192:  {
# 1810 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY37(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 193:  {
# 1814
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 194:  {
# 1818 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(& yyyvsp[0].v);
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 195:
# 1824 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 196:
# 1825 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY27(& yyyvsp[1].v));}));
goto _LL0;case 197:  {
# 1829 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(& yyyvsp[3].v)!=0)
ans=({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp3->tag=5,_Tmp3->f1=yyyvsp[3].l.first_line,({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY46(& yyyvsp[3].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=ans;_Tmp1;});{
# 1833
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _Tmp1=*Cyc_yyget_YY1(& yyyvsp[0].v);void*_Tmp2;void*_Tmp3;unsigned _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{unsigned ploc=_Tmp4;void*nullable=_Tmp3;void*bound=_Tmp2;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));_Tmp5->ptr_loc=ploc,_Tmp5->rgn_loc=yyyvsp[2].l.first_line,_Tmp5->zt_loc=yyyvsp[1].l.first_line;_Tmp5;});{
struct _tuple15 _Tmp5=({unsigned _Tmp6=ploc;void*_Tmp7=nullable;void*_Tmp8=bound;void*_Tmp9=Cyc_yyget_YY45(& yyyvsp[2].v);Cyc_Parse_collapse_pointer_quals(_Tmp6,_Tmp7,_Tmp8,_Tmp9,Cyc_yyget_YY57(& yyyvsp[1].v));});void*_Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;_TmpA=_Tmp5.f1;_Tmp9=_Tmp5.f2;_Tmp8=_Tmp5.f3;_Tmp7=_Tmp5.f4;_Tmp6=_Tmp5.f5;{void*nullable=_TmpA;void*bound=_Tmp9;void*zeroterm=_Tmp8;void*rgn_opt=_Tmp7;void*autoreleased=_Tmp6;
ans=({struct Cyc_List_List*_TmpB=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpC=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_TmpD=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_TmpD->tag=2,_TmpD->f1.rgn=rgn_opt,_TmpD->f1.nullable=nullable,_TmpD->f1.bounds=bound,_TmpD->f1.zero_term=zeroterm,_TmpD->f1.ptrloc=ptrloc,_TmpD->f1.autoreleased=autoreleased,({struct Cyc_Absyn_Tqual _TmpE=Cyc_yyget_YY24(& yyyvsp[4].v);_TmpD->f2=_TmpE;});_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=ans;_TmpB;});
yyval=Cyc_YY27(ans);
# 1841
goto _LL0;}}}}}case 198:
# 1843
 yyval=Cyc_YY57(0);
goto _LL0;case 199:
# 1844 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY56(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY57(& yyyvsp[1].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 200:
# 1849 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 201:
# 1850 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=1,({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 202:
# 1851 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=2;_Tmp1;}));
goto _LL0;case 203:
# 1852 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=3;_Tmp1;}));
goto _LL0;case 204:
# 1853 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=6;_Tmp1;}));
goto _LL0;case 205:
# 1854 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=4;_Tmp1;}));
goto _LL0;case 206:
# 1855 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=5;_Tmp1;}));
goto _LL0;case 207:
# 1856 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=7;_Tmp1;}));
goto _LL0;case 208:
# 1857 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=8;_Tmp1;}));
goto _LL0;case 209:
# 1863 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f1=yyyvsp[0].l.first_line,_Tmp1->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_Tmp1->f3=Cyc_Absyn_fat_bound_type:({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 210:
# 1864 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f1=yyyvsp[0].l.first_line,_Tmp1->f2=Cyc_Absyn_false_type,({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 211:
# 1865 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f1=yyyvsp[0].l.first_line,_Tmp1->f2=Cyc_Absyn_true_type,_Tmp1->f3=Cyc_Absyn_fat_bound_type;_Tmp1;}));
goto _LL0;case 212:
# 1868
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 213:
# 1869 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 214:
# 1872
 yyval=Cyc_YY52(Cyc_Tcutil_any_bool(0));
goto _LL0;case 215:
# 1873 "parse.y"
 yyval=Cyc_YY52(Cyc_Absyn_true_type);
goto _LL0;case 216:
# 1874 "parse.y"
 yyval=Cyc_YY52(Cyc_Absyn_false_type);
goto _LL0;case 217:
# 1879 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 218:
# 1880 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(& yyyvsp[0].v),& Cyc_Kinds_trk,1);yyval=yyyvsp[0].v;
goto _LL0;case 219:
# 1881 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 220:
# 1885 "parse.y"
 yyval=Cyc_YY24(Cyc_Absyn_empty_tqual((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l.first_line));
goto _LL0;case 221:
# 1886 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _Tmp1=Cyc_yyget_YY24(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp1,Cyc_yyget_YY24(& yyyvsp[1].v));}));
goto _LL0;case 222:
# 1890 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f1=0,_Tmp1->f2=0,_Tmp1->f3=0,({void*_Tmp2=Cyc_yyget_YY50(& yyyvsp[0].v);_Tmp1->f4=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY51(& yyyvsp[1].v);_Tmp1->f5=_Tmp2;});_Tmp1;}));
goto _LL0;case 223:
# 1892 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0,_Tmp1->f3=0,({void*_Tmp2=Cyc_yyget_YY50(& yyyvsp[1].v);_Tmp1->f4=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY51(& yyyvsp[2].v);_Tmp1->f5=_Tmp2;});_Tmp1;}));
goto _LL0;case 224:
# 1894 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=1,_Tmp1->f3=0,({void*_Tmp2=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp1->f4=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY51(& yyyvsp[4].v);_Tmp1->f5=_Tmp2;});_Tmp1;}));
goto _LL0;case 225:  {
# 1897
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f1;_Tmp3=_Tmp1->f2;_Tmp2=_Tmp1->f3;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[1].v);v->inject=_Tmp5;});
yyval=Cyc_YY40(({struct _tuple28*_Tmp5=_cycalloc(sizeof(struct _tuple28));_Tmp5->f1=0,_Tmp5->f2=0,_Tmp5->f3=v,({void*_Tmp6=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp5->f4=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY51(& yyyvsp[4].v);_Tmp5->f5=_Tmp6;});_Tmp5;}));
# 1901
goto _LL0;}}case 226:  {
# 1903
struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[4].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f1;_Tmp3=_Tmp1->f2;_Tmp2=_Tmp1->f3;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY32(& yyyvsp[3].v);v->inject=_Tmp5;});
yyval=Cyc_YY40(({struct _tuple28*_Tmp5=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp6=Cyc_List_imp_rev(Cyc_yyget_YY39(& yyyvsp[0].v));_Tmp5->f1=_Tmp6;}),_Tmp5->f2=0,_Tmp5->f3=v,({void*_Tmp6=Cyc_yyget_YY50(& yyyvsp[5].v);_Tmp5->f4=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY51(& yyyvsp[6].v);_Tmp5->f5=_Tmp6;});_Tmp5;}));
# 1907
goto _LL0;}}case 227:
# 1911 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Parse_id2type(_Tmp1,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp2->tag=1,_Tmp2->f1=0;_Tmp2;}));}));
goto _LL0;case 228:
# 1912 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Parse_id2type(_Tmp1,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(& yyyvsp[2].v)));}));
goto _LL0;case 229:
# 1915
 yyval=Cyc_YY50(0);
goto _LL0;case 230:
# 1916 "parse.y"
 yyval=Cyc_YY50(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 231:
# 1920 "parse.y"
 yyval=Cyc_YY51(0);
goto _LL0;case 232:
# 1921 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 233:  {
# 1929 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct)),kb->tag=2,kb->f1=0,kb->f2=& Cyc_Kinds_trk;{
void*t=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_id2type(_Tmp1,(void*)kb);});
yyval=Cyc_YY51(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({void*_Tmp4=Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[0].v));_Tmp3->f1=_Tmp4;}),_Tmp3->f2=t;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1933
goto _LL0;}}case 234:  {
# 1935 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct)),kb->tag=2,kb->f1=0,kb->f2=& Cyc_Kinds_trk;{
void*t=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_id2type(_Tmp1,(void*)kb);});
yyval=Cyc_YY51(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({void*_Tmp4=Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[0].v));_Tmp3->f1=_Tmp4;}),_Tmp3->f2=t;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY51(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
# 1939
goto _LL0;}}case 235:
# 1943 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 236:
# 1945 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_zstrcmp(_Tmp1,({const char*_Tmp2="inject";_tag_fat(_Tmp2,sizeof(char),7U);}));})!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="missing type in function declaration";_tag_fat(_Tmp3,sizeof(char),37U);});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(yyyvsp[0].l.first_line,_tag_fat(_Tmp2,sizeof(void*),1));});
yyval=Cyc_YY32(1);
# 1949
goto _LL0;case 237:
# 1952 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 238:
# 1953 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY41(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY41(& yyyvsp[2].v));}));
goto _LL0;case 239:
# 1957 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 240:
# 1958 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 241:
# 1959 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 242:
# 1961 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(& yyyvsp[0].v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1964
goto _LL0;case 243:
# 1969 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);}));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 244:
# 1971 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);}));_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 245:
# 1976 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 246:
# 1977 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY38(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY39(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 247:  {
# 1983 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
struct Cyc_Parse_Declarator _Tmp5=Cyc_yyget_YY28(& yyyvsp[1].v);void*_Tmp6;unsigned _Tmp7;void*_Tmp8;_Tmp8=_Tmp5.id;_Tmp7=_Tmp5.varloc;_Tmp6=_Tmp5.tms;{struct _tuple0*qv=_Tmp8;unsigned varloc=_Tmp7;struct Cyc_List_List*tms=_Tmp6;
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct _tuple14 _Tmp9=Cyc_Parse_apply_tms(tq,t,atts,tms);void*_TmpA;void*_TmpB;void*_TmpC;struct Cyc_Absyn_Tqual _TmpD;_TmpD=_Tmp9.f1;_TmpC=_Tmp9.f2;_TmpB=_Tmp9.f3;_TmpA=_Tmp9.f4;{struct Cyc_Absyn_Tqual tq2=_TmpD;void*t2=_TmpC;struct Cyc_List_List*tvs=_TmpB;struct Cyc_List_List*atts2=_TmpA;
if(tvs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=({const char*_Tmp10="parameter with bad type params";_tag_fat(_Tmp10,sizeof(char),31U);});_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_err2(yyyvsp[1].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=({const char*_Tmp10="parameter cannot be qualified with a namespace";_tag_fat(_Tmp10,sizeof(char),47U);});_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_err2(yyyvsp[0].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=({const char*_Tmp10="extra attributes on parameter, ignoring";_tag_fat(_Tmp10,sizeof(char),40U);});_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_TmpE=_cycalloc(sizeof(struct _tuple8));_TmpE->f1=idopt,_TmpE->f2=tq2,_TmpE->f3=t2;_TmpE;}));
# 1997
goto _LL0;}}}}}}case 248:  {
# 1998 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
if(atts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="bad attributes on parameter, ignoring";_tag_fat(_Tmp7,sizeof(char),38U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_Tmp6,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f1=0,_Tmp5->f2=tq,_Tmp5->f3=t;_Tmp5;}));
# 2005
goto _LL0;}}}case 249:  {
# 2006 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct Cyc_List_List*tms=Cyc_yyget_YY31(& yyyvsp[1].v).tms;
struct _tuple14 _Tmp5=Cyc_Parse_apply_tms(tq,t,atts,tms);void*_Tmp6;void*_Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;_Tmp9=_Tmp5.f1;_Tmp8=_Tmp5.f2;_Tmp7=_Tmp5.f3;_Tmp6=_Tmp5.f4;{struct Cyc_Absyn_Tqual tq2=_Tmp9;void*t2=_Tmp8;struct Cyc_List_List*tvs=_Tmp7;struct Cyc_List_List*atts2=_Tmp6;
if(tvs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="bad type parameters on formal argument, ignoring";_tag_fat(_TmpC,sizeof(char),49U);});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpB,sizeof(void*),1));});
# 2014
if(atts2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="bad attributes on parameter, ignoring";_tag_fat(_TmpC,sizeof(char),38U);});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpB,sizeof(void*),1));});
yyval=Cyc_YY38(({struct _tuple8*_TmpA=_cycalloc(sizeof(struct _tuple8));_TmpA->f1=0,_TmpA->f2=tq2,_TmpA->f3=t2;_TmpA;}));
# 2018
goto _LL0;}}}}case 250:
# 2021 "parse.y"
 yyval=Cyc_YY37(Cyc_List_imp_rev(Cyc_yyget_YY37(& yyyvsp[0].v)));
goto _LL0;case 251:
# 2024
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 252:
# 2025 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY37(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 253:
# 2029 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 254:
# 2030 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 255:
# 2035 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=36,_Tmp2->f1=0,_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 256:
# 2037 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=36,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 257:
# 2039 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=36,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 258:  {
# 2041 "parse.y"
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[2].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});void*_Tmp3=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,_Tmp3,
Cyc_Absyn_uint_exp(0U,yyyvsp[2].l.first_line));});
# 2044
vd->tq.real_const=1;
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_Tmp2->tag=27,_Tmp2->f1=vd,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2047
goto _LL0;}case 259:  {
# 2049 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_Tmp2->tag=28,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=t,_Tmp2->f3=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2052
goto _LL0;}case 260:
# 2057 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));_Tmp3->f1=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 261:
# 2059 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 262:
# 2061 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));_Tmp3->f1=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 263:
# 2063 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple35*_Tmp2=({struct _tuple35*_Tmp3=_cycalloc(sizeof(struct _tuple35));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY42(& yyyvsp[2].v);_Tmp3->f1=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 264:
# 2067 "parse.y"
 yyval=Cyc_YY42(Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[0].v)));
goto _LL0;case 265:
# 2068 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp3->tag=1,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 266:
# 2073 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 267:
# 2074 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY43(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 268:
# 2078 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 269:
# 2079 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp1->tag=1,({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 270:  {
# 2084 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
if(atts!=0)
Cyc_Warn_warn(yyyvsp[0].l.first_line,({const char*_Tmp5="ignoring attributes in type";_tag_fat(_Tmp5,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f1=0,_Tmp5->f2=tq,_Tmp5->f3=t;_Tmp5;}));
# 2090
goto _LL0;}}case 271:  {
# 2091 "parse.y"
struct _tuple27 _Tmp1=Cyc_yyget_YY36(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
struct Cyc_List_List*tms=Cyc_yyget_YY31(& yyyvsp[1].v).tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);
if(t_info.f3!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,({const char*_Tmp5="bad type params, ignoring";_tag_fat(_Tmp5,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
if(t_info.f4!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,({const char*_Tmp5="bad specifiers, ignoring";_tag_fat(_Tmp5,sizeof(char),25U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY38(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f1=0,_Tmp5->f2=t_info.f1,_Tmp5->f3=t_info.f2;_Tmp5;}));
# 2101
goto _LL0;}}case 272:
# 2104 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 273:
# 2105 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(0));
goto _LL0;case 274:
# 2106 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(& yyyvsp[1].v)));
goto _LL0;case 275:
# 2107 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(& yyyvsp[2].v)));
goto _LL0;case 276:
# 2108 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;})));
goto _LL0;case 277:
# 2114 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 278:
# 2115 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY45(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY41(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 279:
# 2119 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY27(& yyyvsp[0].v);_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 280:
# 2120 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 281:
# 2122 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY27(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,Cyc_yyget_YY31(& yyyvsp[1].v).tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 282:
# 2127 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 283:
# 2129 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY52(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[2].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 284:
# 2131 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY52(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 285:
# 2133 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY52(& yyyvsp[3].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 286:
# 2135 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY52(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2135
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2138
goto _LL0;case 287:  {
# 2139 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY40(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;void*_Tmp6;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp6;int b=_Tmp5;struct Cyc_Absyn_VarargInfo*c=_Tmp4;void*eff=_Tmp3;struct Cyc_List_List*po=_Tmp2;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp7;({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpA=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpB=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpB->tag=3,({void*_TmpC=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpD=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpD->tag=1,_TmpD->f1=lis,_TmpD->f2=b,_TmpD->f3=c,_TmpD->f4=eff,_TmpD->f5=po,({struct Cyc_Absyn_Exp*_TmpE=Cyc_yyget_YY59(& yyyvsp[3].v).f1;_TmpD->f6=_TmpE;}),({struct Cyc_Absyn_Exp*_TmpE=Cyc_yyget_YY59(& yyyvsp[3].v).f2;_TmpD->f7=_TmpE;});_TmpD;});_TmpB->f1=_TmpC;});_TmpB;});_Tmp9->hd=_TmpA;}),_Tmp9->tl=0;_Tmp9;});_Tmp7.tms=_Tmp8;});_Tmp7;}));
# 2142
goto _LL0;}}case 288:  {
# 2143 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY40(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;void*_Tmp6;_Tmp6=_Tmp1->f1;_Tmp5=_Tmp1->f2;_Tmp4=_Tmp1->f3;_Tmp3=_Tmp1->f4;_Tmp2=_Tmp1->f5;{struct Cyc_List_List*lis=_Tmp6;int b=_Tmp5;struct Cyc_Absyn_VarargInfo*c=_Tmp4;void*eff=_Tmp3;struct Cyc_List_List*po=_Tmp2;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp7;({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpA=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpB=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpB->tag=3,({void*_TmpC=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpD=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpD->tag=1,_TmpD->f1=lis,_TmpD->f2=b,_TmpD->f3=c,_TmpD->f4=eff,_TmpD->f5=po,({
struct Cyc_Absyn_Exp*_TmpE=Cyc_yyget_YY59(& yyyvsp[4].v).f1;_TmpD->f6=_TmpE;}),({struct Cyc_Absyn_Exp*_TmpE=Cyc_yyget_YY59(& yyyvsp[4].v).f2;_TmpD->f7=_TmpE;});_TmpD;});
# 2144
_TmpB->f1=_TmpC;});_TmpB;});_Tmp9->hd=_TmpA;}),({
struct Cyc_List_List*_TmpA=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp9->tl=_TmpA;});_Tmp9;});
# 2144
_Tmp7.tms=_Tmp8;});_Tmp7;}));
# 2147
goto _LL0;}}case 289:  {
# 2149
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[2].v)));});
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[1].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2150
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2153
goto _LL0;}case 290:
# 2154 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY46(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY31(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2156
goto _LL0;case 291:
# 2159 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _Tmp1;_Tmp1.f1=0,_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 292:
# 2160 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 293:
# 2161 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _Tmp1;_Tmp1.f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 294:
# 2163 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 295:
# 2167 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 296:
# 2168 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 297:
# 2169 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 298:
# 2170 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 299:
# 2171 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 300:
# 2172 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 301:
# 2178 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 302:
# 2182 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 303:
# 2183 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_exp_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 304:
# 2188 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 305:
# 2189 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 306:
# 2194 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));}));
goto _LL0;case 307:
# 2195 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 308:
# 2197 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_Absyn_skip_stmt(0U));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 309:
# 2199 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_yyget_Stmt_tok(& yyyvsp[3].v));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 310:
# 2200 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 311:
# 2201 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Absyn_seq_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 312:
# 2202 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 313:
# 2205 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY15(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 314:
# 2210 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 315:
# 2212 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[6].v);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 316:
# 2218 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 317:
# 2221
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_unknownid_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 318:
# 2224
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[3].v);Cyc_Absyn_tuple_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 319:
# 2228 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);Cyc_Absyn_trycatch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 320:
# 2242 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 321:
# 2245 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2245
_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 322:
# 2248 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[2].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2248
_Tmp1->hd=_Tmp2;}),({
# 2250
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 323:
# 2252 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 324:
# 2254 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[4].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2254
_Tmp1->hd=_Tmp2;}),({
# 2256
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[5].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 325:
# 2258 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[5].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY8(& yyyvsp[6].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 326:
# 2265 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);Cyc_Absyn_while_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 327:
# 2267 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_do_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 328:
# 2269 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[8].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 329:  {
# 2271 "parse.y"
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[5].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[7].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY16(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp1,s);}));
goto _LL0;}case 330:
# 2275
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 331:
# 2276 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 332:
# 2279
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp2=_Tmp3;});_Tmp2;});Cyc_Absyn_goto_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 333:
# 2280 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 334:
# 2281 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 335:
# 2282 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 336:
# 2283 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_return_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 337:
# 2285 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 338:
# 2286 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 339:
# 2288 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_fallthru_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 340:
# 2297 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 341:
# 2300
 yyval=yyyvsp[0].v;
goto _LL0;case 342:
# 2302 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 343:
# 2305
 yyval=yyyvsp[0].v;
goto _LL0;case 344:
# 2307 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 345:
# 2310
 yyval=yyyvsp[0].v;
goto _LL0;case 346:
# 2312 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 347:
# 2315
 yyval=yyyvsp[0].v;
goto _LL0;case 348:
# 2317 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 349:
# 2320
 yyval=yyyvsp[0].v;
goto _LL0;case 350:
# 2322 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 351:
# 2325
 yyval=yyyvsp[0].v;
goto _LL0;case 352:
# 2327 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 353:
# 2330
 yyval=yyyvsp[0].v;
goto _LL0;case 354:
# 2332 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 355:
# 2335
 yyval=yyyvsp[0].v;
goto _LL0;case 356:
# 2337 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 357:
# 2340
 yyval=yyyvsp[0].v;
goto _LL0;case 358:
# 2342 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 359:
# 2344 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 360:
# 2347
 yyval=yyyvsp[0].v;
goto _LL0;case 361:
# 2349 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 362:
# 2352
 yyval=yyyvsp[0].v;
goto _LL0;case 363:
# 2354 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 364:
# 2357
 yyval=yyyvsp[0].v;
goto _LL0;case 365:  {
# 2359 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);})));
# 2362
goto _LL0;}case 366:
# 2365 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 367:
# 2369 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 368:  {
# 2371 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line)));
goto _LL0;}case 369:
# 2374 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);})));
goto _LL0;case 370:
# 2376 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_Tmp1=(*Cyc_yyget_YY38(& yyyvsp[2].v)).f3;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 371:
# 2381 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 372:
# 2389 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 373:
# 2393 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line));
goto _LL0;case 374:
# 2394 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 375:
# 2395 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_Tmp2->tag=15,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 376:
# 2396 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=6,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 377:  {
# 2398 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(& yyyvsp[0].v);
{void*_Tmp1=e->r;struct _fat_ptr _Tmp2;int _Tmp3;short _Tmp4;char _Tmp5;enum Cyc_Absyn_Sign _Tmp6;if(*((int*)_Tmp1)==0)switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.LongLong_c.tag){case 2: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f1;_Tmp5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp6;char i=_Tmp5;
# 2402
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_Tmp8->tag=11,_Tmp8->f1=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL480;}case 4: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f1;_Tmp4=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp6;short i=_Tmp4;
# 2404
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=10,_Tmp8->f1=s,_Tmp8->f2=(int)i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL480;}case 5: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f1;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp6;int i=_Tmp3;
# 2406
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=10,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL480;}case 7: _Tmp2=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f1;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp3;
# 2408
yyval=Cyc_YY9(({void*_Tmp7=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_Tmp8->tag=12,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL480;}case 1:
# 2410
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL480;case 8:
 goto _LL48E;case 9: _LL48E:
# 2413
 Cyc_Warn_err(yyyvsp[0].l.first_line,({const char*_Tmp7="strings cannot occur within patterns";_tag_fat(_Tmp7,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));goto _LL480;case 6:
# 2415
 Cyc_Warn_err(yyyvsp[0].l.first_line,({const char*_Tmp7="long long's in patterns not yet implemented";_tag_fat(_Tmp7,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));goto _LL480;default: goto _LL491;}else{_LL491:
# 2417
 Cyc_Warn_err(yyyvsp[0].l.first_line,({const char*_Tmp7="bad constant in case";_tag_fat(_Tmp7,sizeof(char),21U);}),_tag_fat(0U,sizeof(void*),0));}_LL480:;}
# 2420
goto _LL0;}case 378:
# 2421 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,({const char*_Tmp2="as";_tag_fat(_Tmp2,sizeof(char),3U);}));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,({const char*_Tmp1="expecting `as'";_tag_fat(_Tmp1,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2423
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2426
goto _LL0;case 379:
# 2427 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,({const char*_Tmp2="alias";_tag_fat(_Tmp2,sizeof(char),6U);}));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,({const char*_Tmp1="expecting `alias'";_tag_fat(_Tmp1,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_rk;_Tmp2;});tv->kind=_Tmp1;});{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2436
goto _LL0;}}}case 380:
# 2437 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,({const char*_Tmp2="alias";_tag_fat(_Tmp2,sizeof(char),6U);}));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,({const char*_Tmp1="expecting `alias'";_tag_fat(_Tmp1,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_rk;_Tmp2;});tv->kind=_Tmp1;});{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0);});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2446
goto _LL0;}}}case 381:  {
# 2447 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY10(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct));_Tmp5->tag=5,_Tmp5->f1=ps,_Tmp5->f2=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2450
goto _LL0;}}case 382:  {
# 2451 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY10(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_Tmp5->tag=16,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=ps,_Tmp5->f3=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2454
goto _LL0;}}case 383:  {
# 2455 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[3].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({unsigned _Tmp4=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp4,Cyc_yyget_YY41(& yyyvsp[2].v));});
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=7,({union Cyc_Absyn_AggrInfo*_Tmp6=({union Cyc_Absyn_AggrInfo*_Tmp7=_cycalloc(sizeof(union Cyc_Absyn_AggrInfo));({union Cyc_Absyn_AggrInfo _Tmp8=Cyc_Absyn_UnknownAggr(0U,Cyc_yyget_QualId_tok(& yyyvsp[0].v),0);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),_Tmp5->f2=exist_ts,_Tmp5->f3=fps,_Tmp5->f4=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2460
goto _LL0;}}case 384:  {
# 2461 "parse.y"
struct _tuple23 _Tmp1=*Cyc_yyget_YY14(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({unsigned _Tmp4=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp4,Cyc_yyget_YY41(& yyyvsp[1].v));});
yyval=Cyc_YY9(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=7,_Tmp5->f1=0,_Tmp5->f2=exist_ts,_Tmp5->f3=fps,_Tmp5->f4=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2465
goto _LL0;}}case 385:
# 2466 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=6,({struct Cyc_Absyn_Pat*_Tmp3=({void*_Tmp4=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,({struct Cyc_Absyn_Pat*_Tmp6=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;});_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);});_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 386:
# 2468 "parse.y"
 yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
# 2470
struct Cyc_Absyn_Pat*_Tmp3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[1].l.first_line);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2468
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2472
goto _LL0;case 387:
# 2473 "parse.y"
 if(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_strcmp(_Tmp1,({const char*_Tmp2="as";_tag_fat(_Tmp2,sizeof(char),3U);}));})!=0)
Cyc_Warn_err(yyyvsp[2].l.first_line,({const char*_Tmp1="expecting `as'";_tag_fat(_Tmp1,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
# 2477
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY9(& yyyvsp[3].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2475
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2479
goto _LL0;case 388:  {
# 2480 "parse.y"
void*tag=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_id2type(_Tmp1,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,({struct Cyc_Absyn_Tvar*_Tmp3=Cyc_Parse_typ2tvar(yyyvsp[2].l.first_line,tag);_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(tag),0);});
# 2482
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2481
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2485
goto _LL0;}case 389:  {
# 2486 "parse.y"
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));
yyval=Cyc_YY9(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,_Tmp2->f1=tv,({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2488
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2487
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2491
goto _LL0;}case 390:
# 2494 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 391:
# 2495 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY11(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=1;_Tmp1;}));
goto _LL0;case 392:
# 2496 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f1=0,_Tmp1->f2=1;_Tmp1;}));
goto _LL0;case 393:
# 2499
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 394:
# 2500 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY11(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 395:
# 2503
 yyval=Cyc_YY12(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));_Tmp1->f1=0,({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[0].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 396:
# 2504 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;}),({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY9(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 397:
# 2507
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 398:
# 2508 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY13(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=1;_Tmp1;}));
goto _LL0;case 399:
# 2509 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f1=0,_Tmp1->f2=1;_Tmp1;}));
goto _LL0;case 400:
# 2512
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp2=Cyc_yyget_YY12(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 401:
# 2513 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple24*_Tmp2=Cyc_yyget_YY12(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY13(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 402:
# 2518 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 403:
# 2519 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_seq_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 404:
# 2523 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 405:
# 2525 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Core_Opt*_Tmp2=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assignop_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 406:
# 2527 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_swap_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 407:
# 2531 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 408:
# 2532 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)1U;_Tmp1;}));
goto _LL0;case 409:
# 2533 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)3U;_Tmp1;}));
goto _LL0;case 410:
# 2534 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)4U;_Tmp1;}));
goto _LL0;case 411:
# 2535 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)0U;_Tmp1;}));
goto _LL0;case 412:
# 2536 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)2U;_Tmp1;}));
goto _LL0;case 413:
# 2537 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)16U;_Tmp1;}));
goto _LL0;case 414:
# 2538 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)17U;_Tmp1;}));
goto _LL0;case 415:
# 2539 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)13U;_Tmp1;}));
goto _LL0;case 416:
# 2540 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)15U;_Tmp1;}));
goto _LL0;case 417:
# 2541 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)14U;_Tmp1;}));
goto _LL0;case 418:
# 2545 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 419:
# 2547 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 420:
# 2549 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_throw_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 421:
# 2551 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 422:
# 2552 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 423:
# 2553 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_New_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 424:
# 2555 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_New_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 425:
# 2559 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 426:
# 2562
 yyval=yyyvsp[0].v;
goto _LL0;case 427:
# 2564 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 428:
# 2567
 yyval=yyyvsp[0].v;
goto _LL0;case 429:
# 2569 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 430:
# 2572
 yyval=yyyvsp[0].v;
goto _LL0;case 431:
# 2574 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 432:
# 2577
 yyval=yyyvsp[0].v;
goto _LL0;case 433:
# 2579 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 434:
# 2582
 yyval=yyyvsp[0].v;
goto _LL0;case 435:
# 2584 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 436:
# 2587
 yyval=yyyvsp[0].v;
goto _LL0;case 437:
# 2589 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 438:
# 2592
 yyval=yyyvsp[0].v;
goto _LL0;case 439:
# 2594 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 440:
# 2597
 yyval=yyyvsp[0].v;
goto _LL0;case 441:
# 2599 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 442:
# 2601 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 443:
# 2604
 yyval=yyyvsp[0].v;
goto _LL0;case 444:
# 2606 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 445:
# 2609
 yyval=yyyvsp[0].v;
goto _LL0;case 446:
# 2611 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 447:
# 2614
 yyval=Cyc_YY66(Cyc_Absyn_eq_exp);
goto _LL0;case 448:
# 2615 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_neq_exp);
goto _LL0;case 449:
# 2618
 yyval=Cyc_YY66(Cyc_Absyn_lt_exp);
goto _LL0;case 450:
# 2619 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_gt_exp);
goto _LL0;case 451:
# 2620 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_lte_exp);
goto _LL0;case 452:
# 2621 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_gte_exp);
goto _LL0;case 453:
# 2624
 yyval=Cyc_YY6(0U);
goto _LL0;case 454:
# 2625 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 455:
# 2628
 yyval=Cyc_YY6(1U);
goto _LL0;case 456:
# 2629 "parse.y"
 yyval=Cyc_YY6(3U);
goto _LL0;case 457:
# 2630 "parse.y"
 yyval=Cyc_YY6(4U);
goto _LL0;case 458:
# 2634 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 459:  {
# 2636 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;}case 460:
# 2641 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 461:
# 2642 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;case 462:
# 2643 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,2U,yyyvsp[0].l.first_line);}));
goto _LL0;case 463:
# 2644 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_address_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 464:
# 2645 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_deref_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 465:
# 2646 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY6(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 466:  {
# 2648 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line));
# 2651
goto _LL0;}case 467:
# 2651 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 468:  {
# 2653 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
# 2656
goto _LL0;}case 469:
# 2658
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=0U,_Tmp2->f1.rgn=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2660
goto _LL0;case 470:
# 2661 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2663
goto _LL0;case 471:
# 2664 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=2U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2666
goto _LL0;case 472:
# 2667 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=1;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2669
goto _LL0;case 473:  {
# 2670 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=1U,_Tmp2->f1.rgn=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2673
goto _LL0;}case 474:  {
# 2675
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[8].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[8].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2678
goto _LL0;}case 475:
# 2679 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2[1];({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2[0]=_Tmp3;});Cyc_List_list(_tag_fat(_Tmp2,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(18U,_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 476:
# 2681 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=38,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 477:
# 2683 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=38,({struct Cyc_Absyn_Exp*_Tmp3=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_deref_exp(_Tmp4,yyyvsp[2].l.first_line);});_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 478:  {
# 2685 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY38(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,yyyvsp[0].l.first_line));
goto _LL0;}case 479:
# 2687 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=Cyc_yyget_YY60(& yyyvsp[1].v);Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 480:
# 2688 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_extension_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 481:
# 2689 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 482:
# 2693 "parse.y"
 yyval=Cyc_YY6(12U);
goto _LL0;case 483:
# 2694 "parse.y"
 yyval=Cyc_YY6(11U);
goto _LL0;case 484:
# 2695 "parse.y"
 yyval=Cyc_YY6(2U);
goto _LL0;case 485:
# 2696 "parse.y"
 yyval=Cyc_YY6(0U);
goto _LL0;case 486:  {
# 2701 "parse.y"
struct _tuple30*_Tmp1=Cyc_yyget_YY61(& yyyvsp[3].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f1;_Tmp3=_Tmp1->f2;_Tmp2=_Tmp1->f3;{struct Cyc_List_List*outlist=_Tmp4;struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY60((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_Tmp5->tag=40,({int _Tmp6=Cyc_yyget_YY32(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=outlist,_Tmp5->f4=inlist,_Tmp5->f5=clobbers;_Tmp5;}));
goto _LL0;}}case 487:
# 2706 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 488:
# 2707 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 489:
# 2711 "parse.y"
 yyval=Cyc_YY61(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f1=0,_Tmp1->f2=0,_Tmp1->f3=0;_Tmp1;}));
goto _LL0;case 490:  {
# 2713 "parse.y"
struct _tuple31*_Tmp1=Cyc_yyget_YY62(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY61(({struct _tuple30*_Tmp4=_cycalloc(sizeof(struct _tuple30));_Tmp4->f1=0,_Tmp4->f2=inlist,_Tmp4->f3=clobbers;_Tmp4;}));
goto _LL0;}}case 491:  {
# 2716 "parse.y"
struct _tuple31*_Tmp1=Cyc_yyget_YY62(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY61(({struct _tuple30*_Tmp4=_cycalloc(sizeof(struct _tuple30));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY64(& yyyvsp[1].v));_Tmp4->f1=_Tmp5;}),_Tmp4->f2=inlist,_Tmp4->f3=clobbers;_Tmp4;}));
goto _LL0;}}case 492:
# 2720
 yyval=Cyc_YY64(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY65(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 493:
# 2721 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY65(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY64(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 494:
# 2725 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 495:
# 2726 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f1=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY63(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 496:
# 2727 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY64(& yyyvsp[1].v));_Tmp1->f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY63(& yyyvsp[2].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 497:
# 2730
 yyval=Cyc_YY64(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY65(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 498:
# 2731 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY65(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY64(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 499:
# 2735 "parse.y"
 yyval=Cyc_YY65(({struct _tuple32*_Tmp1=_cycalloc(sizeof(struct _tuple32));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 500:
# 2740 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 501:
# 2741 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 502:
# 2742 "parse.y"
 yyval=Cyc_YY63(Cyc_List_imp_rev(Cyc_yyget_YY63(& yyyvsp[1].v)));
goto _LL0;case 503:
# 2745
 yyval=Cyc_YY63(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 504:
# 2746 "parse.y"
 yyval=Cyc_YY63(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY63(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 505:
# 2751 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 506:
# 2753 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_subscript_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 507:
# 2755 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_unknowncall_exp(_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 508:
# 2757 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_unknowncall_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 509:
# 2759 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrmember_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 510:
# 2761 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrarrow_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 511:
# 2763 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,1U,yyyvsp[0].l.first_line);}));
goto _LL0;case 512:
# 2765 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,3U,yyyvsp[0].l.first_line);}));
goto _LL0;case 513:
# 2767 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=25,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 514:
# 2769 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=25,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 515:
# 2771 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=25,({struct _tuple8*_Tmp3=Cyc_yyget_YY38(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 516:
# 2776 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 517:
# 2779
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct));_Tmp3->tag=1,({unsigned _Tmp4=({unsigned _Tmp5=yyyvsp[0].l.first_line;Cyc_Parse_cnst2uint(_Tmp5,Cyc_yyget_Int_tok(& yyyvsp[0].v));});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 518:
# 2781 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 519:
# 2784
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct));_Tmp3->tag=1,({unsigned _Tmp4=({unsigned _Tmp5=yyyvsp[2].l.first_line;Cyc_Parse_cnst2uint(_Tmp5,Cyc_yyget_Int_tok(& yyyvsp[2].v));});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 520:
# 2789 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_unknownid_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 521:
# 2790 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Absyn_pragma_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 522:
# 2791 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 523:
# 2792 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_string_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 524:
# 2793 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wstring_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 525:
# 2794 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 526:
# 2798 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_noinstantiate_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 527:
# 2801
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY41(& yyyvsp[3].v));Cyc_Absyn_instantiate_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 528:
# 2804
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp2->tag=29,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY41(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[3].v));_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 529:
# 2806 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_tuple_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 530:
# 2808 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Absyn_stmt_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 531:
# 2812 "parse.y"
 yyval=Cyc_YY4(Cyc_List_imp_rev(Cyc_yyget_YY4(& yyyvsp[0].v)));
goto _LL0;case 532:
# 2817 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 533:
# 2819 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY4(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 534:
# 2825 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _Tmp1=Cyc_yyget_Int_tok(& yyyvsp[0].v);Cyc_Absyn_const_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 535:
# 2826 "parse.y"
 yyval=Cyc_Exp_tok(({char _Tmp1=Cyc_yyget_Char_tok(& yyyvsp[0].v);Cyc_Absyn_char_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 536:
# 2827 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wchar_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 537:
# 2829 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp(yyyvsp[0].l.first_line));
goto _LL0;case 538:  {
# 2831 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(& yyyvsp[0].v);
int l=(int)Cyc_strlen(f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c==102 ||(int)c==70)i=0;else{
if((int)c==108 ||(int)c==76)i=2;}}
# 2839
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,yyyvsp[0].l.first_line));
# 2841
goto _LL0;}case 539:
# 2844 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f1=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 540:
# 2845 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 541:
# 2848
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f1=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 542:
# 2849 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 543:
# 2854 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 544:
# 2855 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 545:
# 2858
 yyval=yyyvsp[0].v;
goto _LL0;case 546:
# 2859 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 547:
# 2864 "parse.y"
 goto _LL0;case 548:
# 2864 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default:
# 2868
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
((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset].l.last_line=(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l.last_line;
((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset].l.last_column=(*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l.last_column;}
# 409
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr1,549U,sizeof(short),yyn));
# 411
yystate=({int _Tmp1=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,167U,sizeof(short),yyn - 154));_Tmp1 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 6838)&&(int)Cyc_yycheck[yystate]==(int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,167U,sizeof(short),yyn - 154));}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus==0){
# 424
++ yynerrs;
# 427
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1073U,sizeof(short),yystate));
# 429
if(yyn > -32768 && yyn < 6838){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 321U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6839U,sizeof(short),x + yyn))==x)
({unsigned long _Tmp1=Cyc_strlen(*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,321U,sizeof(struct _fat_ptr),x)))+ 15U;sze +=_Tmp1;}),count ++;}
msg=({unsigned _Tmp1=(unsigned)(sze + 15)+ 1U;_tag_fat(({char*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(char)));});({{unsigned _Tmp3=(unsigned)(sze + 15);unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;}),sizeof(char),_Tmp1);});
Cyc_strcpy(msg,({const char*_Tmp1="parse error";_tag_fat(_Tmp1,sizeof(char),12U);}));
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 321U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6839U,sizeof(short),x + yyn))==x){
# 451
Cyc_strcat(msg,count==0?({const char*_Tmp1=", expecting `";_tag_fat(_Tmp1,sizeof(char),14U);}):({const char*_Tmp1=" or `";_tag_fat(_Tmp1,sizeof(char),6U);}));
# 454
Cyc_strcat(msg,Cyc_yytname[x]);
Cyc_strcat(msg,({const char*_Tmp1="'";_tag_fat(_Tmp1,sizeof(char),2U);}));
++ count;}}}
# 459
Cyc_yyerror(msg,yystate,yychar);}else{
# 463
Cyc_yyerror(({const char*_Tmp1="parse error";_tag_fat(_Tmp1,sizeof(char),12U);}),yystate,yychar);}}
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1073U,sizeof(short),yystate));
if(yyn==-32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 6838)||(int)Cyc_yycheck[yyn]!=1)goto yyerrdefault;
# 528
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,6839U,sizeof(short),yyn));
if(yyn < 0){
# 531
if(yyn==-32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 535
if(yyn==0)goto yyerrpop;}
# 537
if(yyn==1072){
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 546
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_Tmp1;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 2867 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
void*_Tmp0;struct _fat_ptr _Tmp1;char _Tmp2;union Cyc_Absyn_Cnst _Tmp3;switch(v.Stmt_tok.tag){case 1: _Tmp3=v.Int_tok.val;{union Cyc_Absyn_Cnst c=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_cnst2string(c);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%s";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 2: _Tmp2=v.Char_tok.val;{char c=_Tmp2;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)c);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%c";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 3: _Tmp1=v.String_tok.val;{struct _fat_ptr s=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="\"%s\"";_tag_fat(_Tmp6,sizeof(char),5U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 4: _Tmp0=v.QualId_tok.val;{struct _tuple0*q=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_qvar2string(q);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%s";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 5: _Tmp0=v.Exp_tok.val;{struct Cyc_Absyn_Exp*e=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(e);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%s";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 6: _Tmp0=v.Stmt_tok.val;{struct Cyc_Absyn_Stmt*s=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_stmt2string(s);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%s";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}default:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="?";_tag_fat(_Tmp4,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 2879
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_Tmp0="end-of-file";_tag_fat(_Tmp0,sizeof(char),12U);});
if(token==369)
return Cyc_Lex_token_string;
if(token==378)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 381?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,382U,sizeof(short),token)): 321;
if((unsigned)z < 321U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 2893
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _Tmp0=_new_region("yyr");struct _RegionHandle*yyr=& _Tmp0;_push_region(yyr);
({struct _RegionHandle*_Tmp1=yyr;Cyc_yyparse(_Tmp1,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_Tmp1=Cyc_Parse_parse_result;_npop_handler(0);return _Tmp1;}
# 2896
;_pop_region();}}

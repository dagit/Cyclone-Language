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

/* Alias qualifier stuff */

typedef unsigned char _AliasQualHandle_t;

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
# 169
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 174
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U,Cyc_Absyn_AqualKind =8U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 285
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 298
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 305
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 472 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 479
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 497
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 511
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 672 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 836
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 861
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 869
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 875
void*Cyc_Absyn_compress(void*);
# 888
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 890
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 893
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 895
extern void*Cyc_Absyn_sint_type;
# 897
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 900
extern void*Cyc_Absyn_complex_type(void*);
# 902
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 904
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_dt_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 908
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 910
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 938
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 942
void*Cyc_Absyn_bounds_one (void);
# 944
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 964
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 967
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 971
void*Cyc_Absyn_aqualsof_type(void*);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1028
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
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1055
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1060
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1074
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1077
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1081
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1086
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1088
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1096
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1103
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1106
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1111
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
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 49
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
# 51
extern struct Cyc_Core_Opt Cyc_Kinds_bko;
# 53
extern struct Cyc_Core_Opt Cyc_Kinds_iko;
# 58
extern struct Cyc_Core_Opt Cyc_Kinds_trko;
# 68
extern struct Cyc_Core_Opt Cyc_Kinds_aqko;
# 70
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 74
struct Cyc_Absyn_Kind*Cyc_Kinds_id_to_kind(struct _fat_ptr,unsigned);
# 82
void*Cyc_Kinds_compress_kb(void*);
# 42 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 89
void*Cyc_Tcutil_copy_type(void*);
# 198
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 226
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 236
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
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Complex_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct _tuple13{struct _tuple13*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
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
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
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
void*_Tmp0;struct Cyc_Absyn_Tqual _Tmp1;_Tmp1=tqt->f2;_Tmp0=tqt->f3;{struct Cyc_Absyn_Tqual tq=_Tmp1;void*t=_Tmp0;
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
return(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_Tmp0=_region_malloc(r,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_Tmp0->tag=2,_Tmp0->f1.rgn=rgn,_Tmp0->f1.nullable=nullable,_Tmp0->f1.bounds=bound,_Tmp0->f1.zero_term=zeroterm,_Tmp0->f1.ptrloc=loc,_Tmp0->f1.autoreleased=autoreleased,
# 214
aqual!=0?_Tmp0->f1.aqual=aqual:({void*_Tmp1=({void*_Tmp2=Cyc_Absyn_new_evar(& Cyc_Kinds_aqko,0);Cyc_Absyn_aqual_var_type(_Tmp2,Cyc_Absyn_rtd_qual_type);});_Tmp0->f1.aqual=_Tmp1;}),_Tmp0->f2=tqs;_Tmp0;});}
# 220
static struct _tuple0*Cyc_Parse_gensym_enum (void){
static int enum_counter=0;
return({struct _tuple0*_Tmp0=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);_Tmp0->f1=_Tmp1;}),({
struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)enum_counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("__anonymous_enum_%d__",sizeof(char),22U),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}struct _tuple15{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 226
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple17*field_info){
# 231
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;struct Cyc_Absyn_Tqual _Tmp5;void*_Tmp6;unsigned _Tmp7;_Tmp7=field_info->f1->f1;_Tmp6=field_info->f1->f2;_Tmp5=field_info->f1->f3;_Tmp4=field_info->f1->f4;_Tmp3=field_info->f1->f5;_Tmp2=field_info->f1->f6;_Tmp1=field_info->f2->f1;_Tmp0=field_info->f2->f2;{unsigned varloc=_Tmp7;struct _tuple0*qid=_Tmp6;struct Cyc_Absyn_Tqual tq=_Tmp5;void*t=_Tmp4;struct Cyc_List_List*tvs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;struct Cyc_Absyn_Exp*widthopt=_Tmp1;struct Cyc_Absyn_Exp*reqopt=_Tmp0;
if(tvs!=0)
Cyc_Warn_err(loc,_tag_fat("bad type params in struct field",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_Absyn_is_qvar_qualified(qid))
Cyc_Warn_err(loc,_tag_fat("struct or union field cannot be qualified with a namespace",sizeof(char),59U),_tag_fat(0U,sizeof(void*),0));
return({struct Cyc_Absyn_Aggrfield*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_Tmp8->name=(*qid).f2,_Tmp8->tq=tq,_Tmp8->type=t,_Tmp8->width=widthopt,_Tmp8->attributes=atts,_Tmp8->requires_clause=reqopt;_Tmp8;});}}
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
void*_Tmp0=t;void*_Tmp1=argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type;Cyc_Tcutil_promote_array(_Tmp0,_Tmp1,Cyc_Absyn_al_qual_type,0);}): t;}struct _tuple18{struct _fat_ptr*f1;void*f2;};
# 301 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
struct _tuple8*_Tmp0=(struct _tuple8*)x->hd;struct _fat_ptr _Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)((struct _tuple8*)_Tmp0)->f3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->tl==0){_Tmp3=_Tmp0->f1;_Tmp2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f3)->f2->hd;if((struct _fat_ptr*)_Tmp3!=0){struct _fat_ptr*v=_Tmp3;void*i=_Tmp2;
# 306
{void*_Tmp4;if(*((int*)i)==1){_Tmp4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)i)->f2;{void**z=(void**)_Tmp4;
# 310
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp5=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));});*nm=_Tmp5;});
({void*_Tmp5=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp6->name=nm,_Tmp6->identity=-1,({void*_Tmp7=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp8->tag=0,_Tmp8->f1=& Cyc_Kinds_ik;_Tmp8;});_Tmp6->kind=_Tmp7;});_Tmp6;}));*z=_Tmp5;});
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 315
res=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp5=({struct _tuple18*_Tmp6=_cycalloc(sizeof(struct _tuple18));_Tmp6->f1=v,_Tmp6->f2=i;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=res;_Tmp4;});goto _LL0;}else{if(((struct _tuple8*)_Tmp0)->f1!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1!=0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1!=0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f1)==4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2!=0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->hd)==1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_Tmp0)->f3)->f2)->tl==0){_Tmp1=*_Tmp0->f1;_Tmp3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0->f3)->f2->hd)->f2;{struct _fat_ptr v=_Tmp1;void**z=(void**)_Tmp3;
# 319
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)v);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp6,sizeof(void*),1));});*nm=_Tmp4;});
({void*_Tmp4=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_Tmp5->name=nm,_Tmp5->identity=-1,({void*_Tmp6=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp7->tag=0,_Tmp7->f1=& Cyc_Kinds_rk;_Tmp7;});_Tmp5->kind=_Tmp6;});_Tmp5;}));*z=_Tmp4;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_Tmp0)->f1!=0)goto _LL5;else{_LL5:
 goto _LL0;}}_LL0:;}
# 324
return res;}
# 328
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*res=0;
for(1;x!=0;x=x->tl){
void*_Tmp0=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_Tmp1;if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==5){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2)->tl==0){_Tmp1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2->hd;{void*i=_Tmp1;
# 333
res=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp3=({struct _tuple18*_Tmp4=_cycalloc(sizeof(struct _tuple18));_Tmp4->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_Tmp4->f2=i;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=res;_Tmp2;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 336
return res;}
# 340
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)==0){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f1.Rel_n.tag==1){if(((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1)->f1.Rel_n.val==0){_Tmp1=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1)->f1->f2;{struct _fat_ptr*y=_Tmp1;
# 343
{struct Cyc_List_List*ts=tags;for(0;ts!=0;ts=ts->tl){
struct _tuple18*_Tmp2=(struct _tuple18*)ts->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct _fat_ptr*x=_Tmp4;void*i=_Tmp3;
if(Cyc_strptrcmp(x,y)==0)
return({void*_Tmp5=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_Tmp6->tag=39,({void*_Tmp7=Cyc_Tcutil_copy_type(i);_Tmp6->f1=_Tmp7;});_Tmp6;});Cyc_Absyn_new_exp(_Tmp5,e->loc);});}}}
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
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9: _Tmp9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp9;
# 377
struct Cyc_Absyn_Exp*e2=Cyc_Parse_substitute_tags_exp(tags,e);
if(e2!=e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9:
 goto _LL0;}_LL0:;}
# 382
return t;}
# 387
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_Tmp0=Cyc_Parse_substitute_tags(tags,x->type);x->type=_Tmp0;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple20{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 394
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple20*t){
return({struct _tuple19*_Tmp0=_cycalloc(sizeof(struct _tuple19));_Tmp0->f1=(*t).f2,_Tmp0->f2=(*t).f3;_Tmp0;});}
# 398
static int Cyc_Parse_is_typeparam(void*tm){
if(*((int*)tm)==4)
return 1;else{
return 0;};}
# 407
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
return Cyc_Absyn_heap_rgn_type;
# 411
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
return Cyc_Absyn_unique_rgn_type;
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->name=_Tmp1;}),_Tmp0->identity=-1,_Tmp0->kind=k;_Tmp0;}));}
# 420
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(Cyc_zstrcmp(s,_tag_fat("`H",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of heap region",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`U",sizeof(char),3U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of unique region",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,_tag_fat("`RC",sizeof(char),4U))==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of refcounted region",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
if(Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name)==0)
Cyc_Warn_err(loc,_tag_fat("bad occurrence of \"current\" region",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));}
# 435
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_Tmp0;if(*((int*)t)==2){_Tmp0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)t)->f1;{struct Cyc_Absyn_Tvar*pr=_Tmp0;
return pr;}}else{
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,_tag_fat("expecting a list of type variables, not types",sizeof(char),46U));};}
# 443
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_Tmp0=Cyc_Absyn_compress(t);void*_Tmp1;if(*((int*)_Tmp0)==2){_Tmp1=(void**)&((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_Tmp0)->f1->kind;{void**cptr=(void**)_Tmp1;
# 446
void*_Tmp2=Cyc_Kinds_compress_kb(*cptr);if(*((int*)_Tmp2)==1){
# 448
({void*_Tmp3=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_Tmp4->tag=2,_Tmp4->f1=0,_Tmp4->f2=k;_Tmp4;}): Cyc_Kinds_kind_to_bound(k);*cptr=_Tmp3;});return;}else{
return;};}}else{
# 451
return;};}
# 456
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 464
if(tms==0)return 0;{
# 466
void*_Tmp0=(void*)tms->hd;void*_Tmp1;if(*((int*)_Tmp0)==3){_Tmp1=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp1;
# 469
if(tms->tl==0 ||
 Cyc_Parse_is_typeparam((void*)_check_null(tms->tl)->hd)&& _check_null(tms->tl)->tl==0){
# 472
void*_Tmp2;if(*((int*)args)==1){
# 474
Cyc_Warn_warn(loc,
_tag_fat("function declaration with both new- and old-style parameter declarations; ignoring old-style",sizeof(char),93U),_tag_fat(0U,sizeof(void*),0));
return tms;}else{_Tmp2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f1;{struct Cyc_List_List*ids=_Tmp2;
# 478
if(({int _Tmp3=Cyc_List_length(ids);_Tmp3!=Cyc_List_length(tds);}))
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,
_tag_fat("wrong number of parameter declarations in old-style function declaration",sizeof(char),73U));{
# 482
struct Cyc_List_List*rev_new_params=0;
for(1;ids!=0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2!=0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)tds2->hd;
void*_Tmp3=x->r;void*_Tmp4;if(*((int*)_Tmp3)==0){_Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp3)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 489
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!=0)
continue;
if(vd->initializer!=0)
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,_tag_fat("initializer found in parameter declaration",sizeof(char),43U));
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,_tag_fat("namespaces forbidden in parameter declarations",sizeof(char),47U));
rev_new_params=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp6=({struct _tuple8*_Tmp7=_cycalloc(sizeof(struct _tuple8));_Tmp7->f1=(*vd->name).f2,_Tmp7->f2=vd->tq,_Tmp7->f3=vd->type;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=rev_new_params;_Tmp5;});
# 497
goto L;}}else{
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(x->loc,_tag_fat("nonvariable declaration in parameter type",sizeof(char),42U));};}
# 501
L: if(tds2==0)
({unsigned _Tmp3=loc;({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(_Tmp3,Cyc_strconcat(*((struct _fat_ptr*)ids->hd),_tag_fat(" is not given a type",sizeof(char),21U)));});}
# 504
return({struct Cyc_List_List*_Tmp3=_region_malloc(yy,sizeof(struct Cyc_List_List));
({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yy,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yy,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_Tmp7->tag=1,({struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(rev_new_params);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=0,_Tmp7->f3=0,_Tmp7->f4=0,_Tmp7->f5=0,_Tmp7->f6=0,_Tmp7->f7=0,_Tmp7->f8=0;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});}}};}
# 511
goto _LL4;}}else{_LL4:
 return({struct Cyc_List_List*_Tmp2=_region_malloc(yy,sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)tms->hd,({struct Cyc_List_List*_Tmp3=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_Tmp2->tl=_Tmp3;});_Tmp2;});};}}
# 519
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 523
if(tds!=0)
d=({struct Cyc_Parse_Declarator _Tmp0;_Tmp0.id=d.id,_Tmp0.varloc=d.varloc,({struct Cyc_List_List*_Tmp1=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_Tmp0.tms=_Tmp1;});_Tmp0;});{
# 526
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 532
if(dso!=0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 538
enum Cyc_Parse_Storage_class _Tmp0=dso->sc;switch((int)_Tmp0){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
 Cyc_Warn_err(loc,_tag_fat("bad storage class on function",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}}{
# 545
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f1;_Tmp3=_Tmp0.f2;_Tmp2=_Tmp0.f3;_Tmp1=_Tmp0.f4;{struct Cyc_Absyn_Tqual fn_tqual=_Tmp4;void*fn_type=_Tmp3;struct Cyc_List_List*x=_Tmp2;struct Cyc_List_List*out_atts=_Tmp1;
# 550
if(x!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));{
# 553
struct Cyc_Absyn_FnInfo _Tmp5;if(*((int*)fn_type)==5){_Tmp5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)fn_type)->f1;{struct Cyc_Absyn_FnInfo i=_Tmp5;
# 555
{struct Cyc_List_List*args2=i.args;for(0;args2!=0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1==0){
Cyc_Warn_err(loc,_tag_fat("missing argument variable in function prototype",sizeof(char),48U),_tag_fat(0U,sizeof(void*),0));
({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));*_Tmp7=_tag_fat("?",sizeof(char),2U);_Tmp7;});(*((struct _tuple8*)args2->hd)).f1=_Tmp6;});}}}
# 562
({struct Cyc_List_List*_Tmp6=Cyc_List_append(i.attributes,out_atts);i.attributes=_Tmp6;});
return({struct Cyc_Absyn_Fndecl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));_Tmp6->sc=sc,_Tmp6->is_inline=is_inline,_Tmp6->name=d.id,_Tmp6->body=body,_Tmp6->i=i,_Tmp6->cached_type=0,_Tmp6->param_vardecls=0,_Tmp6->fn_vardecl=0,_Tmp6->orig_scope=sc;_Tmp6;});}}else{
# 566
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,_tag_fat("declarator is not a function prototype",sizeof(char),39U));};}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 570
static struct _fat_ptr Cyc_Parse_msg1={_TmpG0,_TmpG0,_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 572
static struct _fat_ptr Cyc_Parse_msg2={_TmpG1,_TmpG1,_TmpG1 + 84U};
# 579
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 582
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
# 607
return s1;}
# 613
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 621
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
# 634
if(!seen_type){
if(!seen_sign && !seen_size){
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_double_type);
Cyc_Warn_warn(loc,_tag_fat("missing type within specifier",sizeof(char),30U),_tag_fat(0U,sizeof(void*),0));}
# 640
if(ts.Complex_spec)
return Cyc_Absyn_complex_type(Cyc_Absyn_int_type(sgn,sz));else{
return Cyc_Absyn_int_type(sgn,sz);}}
# 644
{enum Cyc_Absyn_Size_of _Tmp0;enum Cyc_Absyn_Sign _Tmp1;if(*((int*)t)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)){case 1: _Tmp1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f1;_Tmp0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)t)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_Tmp1;enum Cyc_Absyn_Size_of sz2=_Tmp0;
# 646
if(seen_sign &&(int)sgn2!=(int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 650
if(seen_size &&(int)sz2!=(int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;}case 2:
# 656
 if(seen_size)
t=Cyc_Absyn_long_double_type;
if(ts.Complex_spec)
t=Cyc_Absyn_complex_type(t);
goto _LL0;default: goto _LL5;}else{_LL5:
# 662
 if(seen_sign)
Cyc_Warn_err(loc,_tag_fat("sign specification on non-integral type",sizeof(char),40U),_tag_fat(0U,sizeof(void*),0));
if(seen_size)
Cyc_Warn_err(loc,_tag_fat("size qualifier on non-integral type",sizeof(char),36U),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}_LL0:;}
# 668
return t;}
# 671
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 675
if(ds==0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*q=d.id;
unsigned varloc=d.varloc;
struct _tuple14 _Tmp0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);void*_Tmp1;void*_Tmp2;void*_Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp0.f1;_Tmp3=_Tmp0.f2;_Tmp2=_Tmp0.f3;_Tmp1=_Tmp0.f4;{struct Cyc_Absyn_Tqual tq2=_Tmp4;void*new_typ=_Tmp3;struct Cyc_List_List*tvs=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
# 682
struct Cyc_List_List*tl=ds->tl==0?0:({struct _RegionHandle*_Tmp5=r;struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=Cyc_Tcutil_copy_type(t);struct _tuple13*_Tmp8=ds->tl;Cyc_Parse_apply_tmss(_Tmp5,_Tmp6,_Tmp7,_Tmp8,shared_atts);});
return({struct Cyc_List_List*_Tmp5=_region_malloc(r,sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_region_malloc(r,sizeof(struct _tuple15));_Tmp7->f1=varloc,_Tmp7->f2=q,_Tmp7->f3=tq2,_Tmp7->f4=new_typ,_Tmp7->f5=tvs,_Tmp7->f6=atts;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=tl;_Tmp5;});}}}
# 686
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 689
if(tms==0)return({struct _tuple14 _Tmp0;_Tmp0.f1=tq,_Tmp0.f2=t,_Tmp0.f3=0,_Tmp0.f4=atts;_Tmp0;});{
void*_Tmp0=(void*)tms->hd;struct Cyc_Absyn_Tqual _Tmp1;struct Cyc_Absyn_PtrAtts _Tmp2;void*_Tmp3;unsigned _Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{void*zeroterm=_Tmp5;unsigned ztloc=_Tmp4;
# 692
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 692
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 1: _Tmp5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp5;void*zeroterm=_Tmp3;unsigned ztloc=_Tmp4;
# 695
return({struct Cyc_Absyn_Tqual _Tmp6=Cyc_Absyn_empty_tqual(0U);void*_Tmp7=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 695
struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}case 3: _Tmp5=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;{void*args=_Tmp5;
# 698
unsigned _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;int _TmpD;void*_TmpE;if(*((int*)args)==1){_TmpE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f1;_TmpD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f2;_TmpC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f3;_TmpB=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f4;_TmpA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f5;_Tmp9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f6;_Tmp8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f7;_Tmp7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)args)->f8;{struct Cyc_List_List*args2=_TmpE;int c_vararg=_TmpD;struct Cyc_Absyn_VarargInfo*cyc_vararg=_TmpC;void*eff=_TmpB;struct Cyc_List_List*rgn_po=_TmpA;struct Cyc_List_List*qb=_Tmp9;struct Cyc_Absyn_Exp*req=_Tmp8;struct Cyc_Absyn_Exp*ens=_Tmp7;
# 700
struct Cyc_List_List*typvars=0;
# 702
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as!=0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(void*)as->hd,_TmpF->tl=fn_atts;_TmpF;});else{
# 707
new_atts=({struct Cyc_List_List*_TmpF=_cycalloc(sizeof(struct Cyc_List_List));_TmpF->hd=(void*)as->hd,_TmpF->tl=new_atts;_TmpF;});}}}
# 709
if(tms->tl!=0){
void*_TmpF=(void*)_check_null(tms->tl)->hd;void*_Tmp10;if(*((int*)_TmpF)==4){_Tmp10=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_TmpF)->f1;{struct Cyc_List_List*ts=_Tmp10;
# 712
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{
goto _LL12;}_LL12:;}
# 718
if(((((!c_vararg && cyc_vararg==0)&& args2!=0)&& args2->tl==0)&&(*((struct _tuple8*)args2->hd)).f1==0)&&(*((struct _tuple8*)args2->hd)).f3==Cyc_Absyn_void_type)
# 723
args2=0;{
# 728
struct Cyc_List_List*new_requires=0;
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_TmpF=(struct _tuple8*)a->hd;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;_Tmp12=_TmpF->f1;_Tmp11=_TmpF->f2;_Tmp10=(void**)& _TmpF->f3;{struct _fat_ptr*vopt=_Tmp12;struct Cyc_Absyn_Tqual tq=_Tmp11;void**t=(void**)_Tmp10;
void*_Tmp13=*t;unsigned _Tmp14;void*_Tmp15;void*_Tmp16;struct Cyc_Absyn_Tqual _Tmp17;void*_Tmp18;if(*((int*)_Tmp13)==4){_Tmp18=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.elt_type;_Tmp17=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.tq;_Tmp16=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.num_elts;_Tmp15=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.zero_term;_Tmp14=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp13)->f1.zt_loc;{void*et=_Tmp18;struct Cyc_Absyn_Tqual tq=_Tmp17;struct Cyc_Absyn_Exp*neltsopt=_Tmp16;void*zt=_Tmp15;unsigned ztloc=_Tmp14;
# 733
if(neltsopt!=0 && vopt!=0){
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0)),v->f1.Loc_n.tag=4U,v->f1.Loc_n.val=0,v->f2=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,({struct Cyc_Absyn_Exp*_Tmp19[1];({struct Cyc_Absyn_Exp*_Tmp1A=Cyc_Absyn_var_exp(v,0U);_Tmp19[0]=_Tmp1A;});Cyc_List_list(_tag_fat(_Tmp19,sizeof(struct Cyc_Absyn_Exp*),1));}),0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);
new_requires=({struct Cyc_List_List*_Tmp19=_cycalloc(sizeof(struct Cyc_List_List));_Tmp19->hd=new_req,_Tmp19->tl=new_requires;_Tmp19;});}}
# 740
goto _LL1A;}}else{
goto _LL1A;}_LL1A:;}}}
# 744
if(new_requires!=0){
struct Cyc_Absyn_Exp*r;
if(req!=0)
r=req;else{
# 749
r=(struct Cyc_Absyn_Exp*)new_requires->hd;
new_requires=new_requires->tl;}
# 752
for(1;new_requires!=0;new_requires=new_requires->tl){
r=Cyc_Absyn_and_exp(r,(struct Cyc_Absyn_Exp*)new_requires->hd,0U);}
req=r;}{
# 758
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 760
if(tags!=0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 765
{struct Cyc_List_List*a=args2;for(0;a!=0;a=a->tl){
struct _tuple8*_TmpF=(struct _tuple8*)a->hd;void*_Tmp10;struct Cyc_Absyn_Tqual _Tmp11;void*_Tmp12;_Tmp12=_TmpF->f1;_Tmp11=_TmpF->f2;_Tmp10=(void**)& _TmpF->f3;{struct _fat_ptr*vopt=_Tmp12;struct Cyc_Absyn_Tqual tq=_Tmp11;void**t=(void**)_Tmp10;
if(tags!=0)
({void*_Tmp13=Cyc_Parse_substitute_tags(tags,*t);*t=_Tmp13;});
({void*_Tmp13=Cyc_Parse_array2ptr(*t,1);*t=_Tmp13;});}}}
# 777
return({struct Cyc_Absyn_Tqual _TmpF=Cyc_Absyn_empty_tqual(tq.loc);void*_Tmp10=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,qb,fn_atts,req,ens);
# 777
struct Cyc_List_List*_Tmp11=new_atts;Cyc_Parse_apply_tms(_TmpF,_Tmp10,_Tmp11,
# 782
_check_null(tms)->tl);});}}}}else{_Tmp6=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)args)->f2;{unsigned loc=_Tmp6;
# 784
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,_tag_fat("function declaration without parameter types",sizeof(char),45U));}};}case 4: _Tmp5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_List_List*ts=_Tmp5;unsigned loc=_Tmp4;
# 791
if(tms->tl==0)
return({struct _tuple14 _Tmp6;_Tmp6.f1=tq,_Tmp6.f2=t,_Tmp6.f3=ts,_Tmp6.f4=atts;_Tmp6;});
# 796
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,
_tag_fat("type parameters must appear before function arguments in declarator",sizeof(char),68U));}case 2: _Tmp2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_Tmp2;struct Cyc_Absyn_Tqual tq2=_Tmp1;
# 799
return({struct Cyc_Absyn_Tqual _Tmp6=tq2;void*_Tmp7=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _Tmp8;_Tmp8.elt_type=t,_Tmp8.elt_tq=tq,_Tmp8.ptr_atts=ptratts;_Tmp8;}));struct Cyc_List_List*_Tmp8=atts;Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}default: _Tmp4=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_Tmp0)->f2;{unsigned loc=_Tmp4;struct Cyc_List_List*atts2=_Tmp5;
# 804
return({struct Cyc_Absyn_Tqual _Tmp6=tq;void*_Tmp7=t;struct Cyc_List_List*_Tmp8=Cyc_List_append(atts,atts2);Cyc_Parse_apply_tms(_Tmp6,_Tmp7,_Tmp8,tms->tl);});}};}}
# 810
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 819
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple15*t){
void*_Tmp0;void*_Tmp1;void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;unsigned _Tmp5;_Tmp5=t->f1;_Tmp4=t->f2;_Tmp3=t->f3;_Tmp2=t->f4;_Tmp1=t->f5;_Tmp0=t->f6;{unsigned varloc=_Tmp5;struct _tuple0*x=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*typ=_Tmp2;struct Cyc_List_List*tvs=_Tmp1;struct Cyc_List_List*atts=_Tmp0;
# 822
Cyc_Lex_register_typedef(x);{
# 824
struct Cyc_Core_Opt*kind;
void*type;
{void*_Tmp6;if(*((int*)typ)==1){_Tmp6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)typ)->f1;{struct Cyc_Core_Opt*kopt=_Tmp6;
# 828
type=0;
kind=kopt==0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}
# 833
return({void*_Tmp6=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_Tmp7->tag=8,({struct Cyc_Absyn_Typedefdecl*_Tmp8=({struct Cyc_Absyn_Typedefdecl*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));_Tmp9->name=x,_Tmp9->tvs=tvs,_Tmp9->kind=kind,_Tmp9->defn=type,_Tmp9->atts=atts,_Tmp9->tq=tq,_Tmp9->extern_c=0;_Tmp9;});_Tmp7->f1=_Tmp8;});_Tmp7;});Cyc_Absyn_new_decl(_Tmp6,loc);});}}}
# 840
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_Tmp0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_Tmp1->tag=12,_Tmp1->f1=d,_Tmp1->f2=s;_Tmp1;});Cyc_Absyn_new_stmt(_Tmp0,d->loc);});}
# 844
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return({(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right;})(Cyc_Parse_flatten_decl,ds,s);}
# 848
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es){
# 851
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
for(1;ds!=0;ds=ds->tl){
struct _tuple12 _Tmp0=ds->hd;void*_Tmp1;struct Cyc_Parse_Declarator _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_Parse_Declarator d=_Tmp2;struct Cyc_Absyn_Exp*e=_Tmp1;
declarators=({struct _tuple13*_Tmp3=_region_malloc(r,sizeof(struct _tuple13));_Tmp3->tl=declarators,_Tmp3->hd=d;_Tmp3;});
exprs=({struct Cyc_List_List*_Tmp3=_region_malloc(r,sizeof(struct Cyc_List_List));_Tmp3->hd=e,_Tmp3->tl=exprs;_Tmp3;});}}
# 858
({struct Cyc_List_List*_Tmp0=Cyc_List_imp_rev(exprs);*es=_Tmp0;});
({struct _tuple13*_Tmp0=({(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;})(declarators);*decls=_Tmp0;});}
# 867
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 870
struct _RegionHandle _Tmp0=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _Tmp0;_push_region(mkrgn);
{void*_Tmp1;struct Cyc_Parse_Type_specifier _Tmp2;struct Cyc_Absyn_Tqual _Tmp3;_Tmp3=ds.tq;_Tmp2=ds.type_specs;_Tmp1=ds.attributes;{struct Cyc_Absyn_Tqual tq=_Tmp3;struct Cyc_Parse_Type_specifier tss=_Tmp2;struct Cyc_List_List*atts=_Tmp1;
if(tq.loc==0U)tq.loc=tqual_loc;
if(ds.is_inline)
Cyc_Warn_warn(loc,_tag_fat("inline qualifier on non-function definition",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));{
# 876
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
# 892
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 896
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es!=0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd!=0){
exps_empty=0;
break;}}}{
# 904
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators==0){
# 908
int _Tmp4;void*_Tmp5;enum Cyc_Absyn_AggrKind _Tmp6;void*_Tmp7;switch(*((int*)base_type)){case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1)->r)){case 0: _Tmp7=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
# 910
({struct Cyc_List_List*_Tmp8=Cyc_List_append(ad->attributes,atts);ad->attributes=_Tmp8;});
ad->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 1: _Tmp7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp7;
# 914
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on enum not supported",sizeof(char),33U),_tag_fat(0U,sizeof(void*),0));
ed->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpB->tag=7,_TmpB->f1=ed;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: _Tmp7=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)base_type)->f1->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp7;
# 918
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("attributes on datatypes not supported",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));
dd->sc=s;{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=dd;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)){case 25: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.tag==1){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f1;_Tmp7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownAggr.val.f2;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{enum Cyc_Absyn_AggrKind k=_Tmp6;struct _tuple0*n=_Tmp7;struct Cyc_List_List*ts=_Tmp5;
# 922
struct Cyc_List_List*ts2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl)),ad->kind=k,ad->sc=s,ad->name=n,ad->tvs=ts2,ad->impl=0,ad->attributes=0,ad->expected_mem_kind=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on type declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TmpB->tag=5,_TmpB->f1=ad;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{goto _LL25;}case 23: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.tag==2){_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl**tudp=_Tmp7;
# 927
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({void*_TmpA=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TmpB->tag=6,_TmpB->f1=*tudp;_TmpB;});Cyc_Absyn_new_decl(_TmpA,loc);});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}else{_Tmp7=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.name;_Tmp4=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1.UnknownDatatype.val.is_extensible;_Tmp5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f2;{struct _tuple0*n=_Tmp7;int isx=_Tmp4;struct Cyc_List_List*ts=_Tmp5;
# 930
struct Cyc_List_List*ts2=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,loc,ts);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on datatype",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),_Tmp8->hd=tud,_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}}case 20: _Tmp7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct _tuple0*n=_Tmp7;
# 935
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,ed->name=n,ed->fields=0;
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}case 21: _Tmp7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)base_type)->f1)->f1;{struct Cyc_List_List*fs=_Tmp7;
# 941
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl)),ed->sc=s,({struct _tuple0*_Tmp8=Cyc_Parse_gensym_enum();ed->name=_Tmp8;}),({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=fs;_Tmp9;});ed->fields=_Tmp8;});
if(atts!=0)Cyc_Warn_err(loc,_tag_fat("bad attributes on enum",sizeof(char),23U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*_Tmp8;_Tmp8=_cycalloc(sizeof(struct Cyc_List_List)),({struct Cyc_Absyn_Decl*_Tmp9=({struct Cyc_Absyn_Decl*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TmpC->tag=7,_TmpC->f1=ed;_TmpC;});_TmpA->r=_TmpB;}),_TmpA->loc=loc;_TmpA;});_Tmp8->hd=_Tmp9;}),_Tmp8->tl=0;_npop_handler(0);return _Tmp8;}}default: goto _LL25;}default: _LL25:
 Cyc_Warn_err(loc,_tag_fat("missing declarator",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));{struct Cyc_List_List*_Tmp8=0;_npop_handler(0);return _Tmp8;}};}{
# 948
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);
if(istypedef){
# 953
if(!exps_empty)
Cyc_Warn_err(loc,_tag_fat("initializer in typedef declaration",sizeof(char),35U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*decls=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_Tmp4=decls;_npop_handler(0);return _Tmp4;}}{
# 959
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*ds=fields;for(0;ds!=0;(ds=ds->tl,exprs=_check_null(exprs)->tl)){
struct _tuple15*_Tmp4=(struct _tuple15*)ds->hd;void*_Tmp5;void*_Tmp6;void*_Tmp7;struct Cyc_Absyn_Tqual _Tmp8;void*_Tmp9;unsigned _TmpA;_TmpA=_Tmp4->f1;_Tmp9=_Tmp4->f2;_Tmp8=_Tmp4->f3;_Tmp7=_Tmp4->f4;_Tmp6=_Tmp4->f5;_Tmp5=_Tmp4->f6;{unsigned varloc=_TmpA;struct _tuple0*x=_Tmp9;struct Cyc_Absyn_Tqual tq2=_Tmp8;void*t2=_Tmp7;struct Cyc_List_List*tvs2=_Tmp6;struct Cyc_List_List*atts2=_Tmp5;
if(tvs2!=0)
Cyc_Warn_warn(loc,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(exprs==0)
({(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;})(loc,_tag_fat("unexpected NULL in parse!",sizeof(char),26U));{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)_check_null(exprs)->hd);
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_TmpB=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_TmpC=({struct Cyc_Absyn_Decl*_TmpD=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_TmpE=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TmpF->tag=0,_TmpF->f1=vd;_TmpF;});_TmpD->r=_TmpE;}),_TmpD->loc=loc;_TmpD;});_TmpB->hd=_TmpC;}),_TmpB->tl=decls;_TmpB;});}}}}{
# 972
struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(decls);_npop_handler(0);return _Tmp4;}}}}}}}}}
# 871
;_pop_region();}
# 976
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
long long _Tmp0;char _Tmp1;int _Tmp2;switch(x.LongLong_c.tag){case 5: _Tmp2=x.Int_c.val.f2;{int i=_Tmp2;
return(unsigned)i;}case 2: _Tmp1=x.Char_c.val.f2;{char c=_Tmp1;
return(unsigned)c;}case 6: _Tmp0=x.LongLong_c.val.f2;{long long x=_Tmp0;
# 981
unsigned long long y=(unsigned long long)x;
if(y > 4294967295U)
Cyc_Warn_err(loc,_tag_fat("integer constant too large",sizeof(char),27U),_tag_fat(0U,sizeof(void*),0));
return(unsigned)x;}default:
# 986
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err(loc,_tag_fat("expected integer constant but found %s",sizeof(char),39U),_tag_fat(_Tmp4,sizeof(void*),1));});
return 0U;};}
# 992
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;void*_Tmp1;struct _fat_ptr _Tmp2;char _Tmp3;int _Tmp4;enum Cyc_Absyn_Sign _Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 15: _Tmp6=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _tuple0*x=_Tmp6;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2)->r)==0){_Tmp6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;
# 996
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_Tmp7,p->loc);});}}else{goto _LL13;}case 6: _Tmp6=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp6;
return({struct Cyc_Absyn_Exp*_Tmp7=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_Tmp7,p->loc);});}case 9:
 return Cyc_Absyn_null_exp(p->loc);case 10: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign s=_Tmp5;int i=_Tmp4;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11: _Tmp3=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp3;
return Cyc_Absyn_char_exp(c,p->loc);}case 12: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp4;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f3==0){_Tmp6=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct _tuple0*x=_Tmp6;struct Cyc_List_List*ps=_Tmp1;
# 1003
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17: _Tmp6=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
return e;}default: _LL13:
# 1008
 Cyc_Warn_err(p->loc,_tag_fat("cannot mix patterns and expressions in case",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
return Cyc_Absyn_null_exp(p->loc);};}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY11{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY15{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY20{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY21{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY22{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple24{int f1;enum Cyc_Absyn_AggrKind f2;};struct _union_YYSTYPE_YY24{int tag;struct _tuple24 val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY30{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY33{int tag;int val;};struct _union_YYSTYPE_YY34{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY37{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY39{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _union_YYSTYPE_YY41{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY48{int tag;void*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _tuple28{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY52{int tag;struct _tuple28*val;};struct _tuple29{void*f1;void*f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _tuple30{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY56{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _union_YYSTYPE_YY59{int tag;void*val;};struct _union_YYSTYPE_YY60{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY61{int tag;struct _tuple16 val;};struct _union_YYSTYPE_YY62{int tag;void*val;};struct _tuple31{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY64{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY65{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY67{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY68{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YY67 YY67;struct _union_YYSTYPE_YY68 YY68;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1096
static void Cyc_yythrowfail(struct _fat_ptr s){
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=s;_Tmp0;}));}static char _TmpG2[7U]="cnst_t";
# 1063 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2,_TmpG2,_TmpG2 + 7U};
union Cyc_Absyn_Cnst _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Int_tok.tag==1){_Tmp0=yy1->Int_tok.val;{union Cyc_Absyn_Cnst yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1070
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Int_tok.tag=1U,_Tmp0.Int_tok.val=yy1;_Tmp0;});}static char _TmpG3[5U]="char";
# 1064 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3,_TmpG3,_TmpG3 + 5U};
char _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Char_tok.tag==2){_Tmp0=yy1->Char_tok.val;{char yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1071
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Char_tok.tag=2U,_Tmp0.Char_tok.val=yy1;_Tmp0;});}static char _TmpG4[13U]="string_t<`H>";
# 1065 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4,_TmpG4,_TmpG4 + 13U};
struct _fat_ptr _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->String_tok.tag==3){_Tmp0=yy1->String_tok.val;{struct _fat_ptr yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1072
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.String_tok.tag=3U,_Tmp0.String_tok.val=yy1;_Tmp0;});}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1068 "parse.y"
static struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG5,_TmpG5,_TmpG5 + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY1.tag==7){_Tmp0=yy1->YY1.val;{struct _tuple21*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1075
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY1.tag=7U,_Tmp0.YY1.val=yy1;_Tmp0;});}static char _TmpG6[11U]="ptrbound_t";
# 1069 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG6,_TmpG6,_TmpG6 + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY2.tag==8){_Tmp0=yy1->YY2.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1076
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY2.tag=8U,_Tmp0.YY2.val=yy1;_Tmp0;});}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1070 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG7,_TmpG7,_TmpG7 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY3.tag==9){_Tmp0=yy1->YY3.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1077
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY3.tag=9U,_Tmp0.YY3.val=yy1;_Tmp0;});}static char _TmpG8[6U]="exp_t";
# 1071 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG8,_TmpG8,_TmpG8 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Exp_tok.tag==5){_Tmp0=yy1->Exp_tok.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1078
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Exp_tok.tag=5U,_Tmp0.Exp_tok.val=yy1;_Tmp0;});}static char _TmpG9[10U]="exp_opt_t";
static struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG9,_TmpG9,_TmpG9 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY4.tag==10){_Tmp0=yy1->YY4.val;{struct Cyc_Absyn_Exp*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1086
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY4.tag=10U,_Tmp0.YY4.val=yy1;_Tmp0;});}static char _TmpGA[17U]="list_t<exp_t,`H>";
# 1080 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGA,_TmpGA,_TmpGA + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY5.tag==11){_Tmp0=yy1->YY5.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1087
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY5.tag=11U,_Tmp0.YY5.val=yy1;_Tmp0;});}static char _TmpGB[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1081 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGB,_TmpGB,_TmpGB + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY6.tag==12){_Tmp0=yy1->YY6.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1088
static union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY6.tag=12U,_Tmp0.YY6.val=yy1;_Tmp0;});}static char _TmpGC[9U]="primop_t";
# 1082 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGC,_TmpGC,_TmpGC + 9U};
enum Cyc_Absyn_Primop _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY7.tag==13){_Tmp0=yy1->YY7.val;{enum Cyc_Absyn_Primop yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1089
static union Cyc_YYSTYPE Cyc_YY7(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY7.tag=13U,_Tmp0.YY7.val=yy1;_Tmp0;});}static char _TmpGD[19U]="opt_t<primop_t,`H>";
# 1083 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGD,_TmpGD,_TmpGD + 19U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY8.tag==14){_Tmp0=yy1->YY8.val;{struct Cyc_Core_Opt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1090
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY8.tag=14U,_Tmp0.YY8.val=yy1;_Tmp0;});}static char _TmpGE[7U]="qvar_t";
# 1084 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGE,_TmpGE,_TmpGE + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->QualId_tok.tag==4){_Tmp0=yy1->QualId_tok.val;{struct _tuple0*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1091
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.QualId_tok.tag=4U,_Tmp0.QualId_tok.val=yy1;_Tmp0;});}static char _TmpGF[7U]="stmt_t";
# 1087 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpGF,_TmpGF,_TmpGF + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->Stmt_tok.tag==6){_Tmp0=yy1->Stmt_tok.val;{struct Cyc_Absyn_Stmt*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1094
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.Stmt_tok.tag=6U,_Tmp0.Stmt_tok.val=yy1;_Tmp0;});}static char _TmpG10[27U]="list_t<switch_clause_t,`H>";
# 1090 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG10,_TmpG10,_TmpG10 + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY9.tag==15){_Tmp0=yy1->YY9.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1097
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY9.tag=15U,_Tmp0.YY9.val=yy1;_Tmp0;});}static char _TmpG11[6U]="pat_t";
# 1091 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG11,_TmpG11,_TmpG11 + 6U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY10.tag==16){_Tmp0=yy1->YY10.val;{struct Cyc_Absyn_Pat*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1098
static union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY10.tag=16U,_Tmp0.YY10.val=yy1;_Tmp0;});}static char _TmpG12[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1096 "parse.y"
static struct _tuple22*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG12,_TmpG12,_TmpG12 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY11.tag==17){_Tmp0=yy1->YY11.val;{struct _tuple22*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1103
static union Cyc_YYSTYPE Cyc_YY11(struct _tuple22*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY11.tag=17U,_Tmp0.YY11.val=yy1;_Tmp0;});}static char _TmpG13[17U]="list_t<pat_t,`H>";
# 1097 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG13,_TmpG13,_TmpG13 + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY12.tag==18){_Tmp0=yy1->YY12.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1104
static union Cyc_YYSTYPE Cyc_YY12(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY12.tag=18U,_Tmp0.YY12.val=yy1;_Tmp0;});}static char _TmpG14[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1098 "parse.y"
static struct _tuple23*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG14,_TmpG14,_TmpG14 + 36U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY13.tag==19){_Tmp0=yy1->YY13.val;{struct _tuple23*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1105
static union Cyc_YYSTYPE Cyc_YY13(struct _tuple23*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY13.tag=19U,_Tmp0.YY13.val=yy1;_Tmp0;});}static char _TmpG15[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1099 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG15,_TmpG15,_TmpG15 + 47U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY14.tag==20){_Tmp0=yy1->YY14.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1106
static union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY14.tag=20U,_Tmp0.YY14.val=yy1;_Tmp0;});}static char _TmpG16[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1100 "parse.y"
static struct _tuple22*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG16,_TmpG16,_TmpG16 + 58U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY15.tag==21){_Tmp0=yy1->YY15.val;{struct _tuple22*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1107
static union Cyc_YYSTYPE Cyc_YY15(struct _tuple22*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY15.tag=21U,_Tmp0.YY15.val=yy1;_Tmp0;});}static char _TmpG17[9U]="fndecl_t";
# 1101 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG17,_TmpG17,_TmpG17 + 9U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY16.tag==22){_Tmp0=yy1->YY16.val;{struct Cyc_Absyn_Fndecl*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1108
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY16.tag=22U,_Tmp0.YY16.val=yy1;_Tmp0;});}static char _TmpG18[18U]="list_t<decl_t,`H>";
# 1102 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG18,_TmpG18,_TmpG18 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY17.tag==23){_Tmp0=yy1->YY17.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1109
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY17.tag=23U,_Tmp0.YY17.val=yy1;_Tmp0;});}static char _TmpG19[12U]="decl_spec_t";
# 1105 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG19,_TmpG19,_TmpG19 + 12U};
struct Cyc_Parse_Declaration_spec _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY18.tag==24){_Tmp0=yy1->YY18.val;{struct Cyc_Parse_Declaration_spec yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1112
static union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY18.tag=24U,_Tmp0.YY18.val=yy1;_Tmp0;});}static char _TmpG1A[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1106 "parse.y"
static struct _tuple12 Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1A,_TmpG1A,_TmpG1A + 31U};
struct _tuple12 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY19.tag==25){_Tmp0=yy1->YY19.val;{struct _tuple12 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1113
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple12 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY19.tag=25U,_Tmp0.YY19.val=yy1;_Tmp0;});}static char _TmpG1B[23U]="declarator_list_t<`yy>";
# 1107 "parse.y"
static struct _tuple11*Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1B,_TmpG1B,_TmpG1B + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY20.tag==26){_Tmp0=yy1->YY20.val;{struct _tuple11*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1114
static union Cyc_YYSTYPE Cyc_YY20(struct _tuple11*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY20.tag=26U,_Tmp0.YY20.val=yy1;_Tmp0;});}static char _TmpG1C[16U]="storage_class_t";
# 1108 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1C,_TmpG1C,_TmpG1C + 16U};
enum Cyc_Parse_Storage_class _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY21.tag==27){_Tmp0=yy1->YY21.val;{enum Cyc_Parse_Storage_class yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1115
static union Cyc_YYSTYPE Cyc_YY21(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY21.tag=27U,_Tmp0.YY21.val=yy1;_Tmp0;});}static char _TmpG1D[17U]="type_specifier_t";
# 1109 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1D,_TmpG1D,_TmpG1D + 17U};
struct Cyc_Parse_Type_specifier _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY22.tag==28){_Tmp0=yy1->YY22.val;{struct Cyc_Parse_Type_specifier yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1116
static union Cyc_YYSTYPE Cyc_YY22(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY22.tag=28U,_Tmp0.YY22.val=yy1;_Tmp0;});}static char _TmpG1E[12U]="aggr_kind_t";
# 1111 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1E,_TmpG1E,_TmpG1E + 12U};
enum Cyc_Absyn_AggrKind _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY23.tag==29){_Tmp0=yy1->YY23.val;{enum Cyc_Absyn_AggrKind yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1118
static union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY23.tag=29U,_Tmp0.YY23.val=yy1;_Tmp0;});}static char _TmpG1F[20U]="$(bool,aggr_kind_t)";
# 1112 "parse.y"
static struct _tuple24 Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG1F,_TmpG1F,_TmpG1F + 20U};
struct _tuple24 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY24.tag==30){_Tmp0=yy1->YY24.val;{struct _tuple24 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1119
static union Cyc_YYSTYPE Cyc_YY24(struct _tuple24 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY24.tag=30U,_Tmp0.YY24.val=yy1;_Tmp0;});}static char _TmpG20[8U]="tqual_t";
# 1113 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG20,_TmpG20,_TmpG20 + 8U};
struct Cyc_Absyn_Tqual _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY25.tag==31){_Tmp0=yy1->YY25.val;{struct Cyc_Absyn_Tqual yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1120
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY25.tag=31U,_Tmp0.YY25.val=yy1;_Tmp0;});}static char _TmpG21[23U]="list_t<aggrfield_t,`H>";
# 1114 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG21,_TmpG21,_TmpG21 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY26.tag==32){_Tmp0=yy1->YY26.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1121
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY26.tag=32U,_Tmp0.YY26.val=yy1;_Tmp0;});}static char _TmpG22[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1115 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG22,_TmpG22,_TmpG22 + 34U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY27.tag==33){_Tmp0=yy1->YY27.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1122
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY27.tag=33U,_Tmp0.YY27.val=yy1;_Tmp0;});}static char _TmpG23[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1116 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG23,_TmpG23,_TmpG23 + 33U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY28.tag==34){_Tmp0=yy1->YY28.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1123
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY28.tag=34U,_Tmp0.YY28.val=yy1;_Tmp0;});}static char _TmpG24[18U]="declarator_t<`yy>";
# 1117 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG24,_TmpG24,_TmpG24 + 18U};
struct Cyc_Parse_Declarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY29.tag==35){_Tmp0=yy1->YY29.val;{struct Cyc_Parse_Declarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1124
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY29.tag=35U,_Tmp0.YY29.val=yy1;_Tmp0;});}static char _TmpG25[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1118 "parse.y"
static struct _tuple25*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG25,_TmpG25,_TmpG25 + 45U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY30.tag==36){_Tmp0=yy1->YY30.val;{struct _tuple25*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1125
static union Cyc_YYSTYPE Cyc_YY30(struct _tuple25*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY30.tag=36U,_Tmp0.YY30.val=yy1;_Tmp0;});}static char _TmpG26[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1119 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG26,_TmpG26,_TmpG26 + 57U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY31.tag==37){_Tmp0=yy1->YY31.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1126
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY31.tag=37U,_Tmp0.YY31.val=yy1;_Tmp0;});}static char _TmpG27[26U]="abstractdeclarator_t<`yy>";
# 1120 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG27,_TmpG27,_TmpG27 + 26U};
struct Cyc_Parse_Abstractdeclarator _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY32.tag==38){_Tmp0=yy1->YY32.val;{struct Cyc_Parse_Abstractdeclarator yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1127
static union Cyc_YYSTYPE Cyc_YY32(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY32.tag=38U,_Tmp0.YY32.val=yy1;_Tmp0;});}static char _TmpG28[5U]="bool";
# 1121 "parse.y"
static int Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG28,_TmpG28,_TmpG28 + 5U};
int _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY33.tag==39){_Tmp0=yy1->YY33.val;{int yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1128
static union Cyc_YYSTYPE Cyc_YY33(int yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY33.tag=39U,_Tmp0.YY33.val=yy1;_Tmp0;});}static char _TmpG29[8U]="scope_t";
# 1122 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG29,_TmpG29,_TmpG29 + 8U};
enum Cyc_Absyn_Scope _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY34.tag==40){_Tmp0=yy1->YY34.val;{enum Cyc_Absyn_Scope yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1129
static union Cyc_YYSTYPE Cyc_YY34(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY34.tag=40U,_Tmp0.YY34.val=yy1;_Tmp0;});}static char _TmpG2A[16U]="datatypefield_t";
# 1123 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2A,_TmpG2A,_TmpG2A + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY35.tag==41){_Tmp0=yy1->YY35.val;{struct Cyc_Absyn_Datatypefield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1130
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY35.tag=41U,_Tmp0.YY35.val=yy1;_Tmp0;});}static char _TmpG2B[27U]="list_t<datatypefield_t,`H>";
# 1124 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2B,_TmpG2B,_TmpG2B + 27U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY36.tag==42){_Tmp0=yy1->YY36.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1131
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY36.tag=42U,_Tmp0.YY36.val=yy1;_Tmp0;});}static char _TmpG2C[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1125 "parse.y"
static struct _tuple26 Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2C,_TmpG2C,_TmpG2C + 41U};
struct _tuple26 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY37.tag==43){_Tmp0=yy1->YY37.val;{struct _tuple26 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1132
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple26 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY37.tag=43U,_Tmp0.YY37.val=yy1;_Tmp0;});}static char _TmpG2D[17U]="list_t<var_t,`H>";
# 1126 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2D,_TmpG2D,_TmpG2D + 17U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY38.tag==44){_Tmp0=yy1->YY38.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1133
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY38.tag=44U,_Tmp0.YY38.val=yy1;_Tmp0;});}static char _TmpG2E[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1127 "parse.y"
static struct _tuple8*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2E,_TmpG2E,_TmpG2E + 31U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY39.tag==45){_Tmp0=yy1->YY39.val;{struct _tuple8*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1134
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple8*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY39.tag=45U,_Tmp0.YY39.val=yy1;_Tmp0;});}static char _TmpG2F[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1128 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG2F,_TmpG2F,_TmpG2F + 42U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY40.tag==46){_Tmp0=yy1->YY40.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1135
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY40.tag=46U,_Tmp0.YY40.val=yy1;_Tmp0;});}static char _TmpG30[147U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1129 "parse.y"
static struct _tuple27*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG30,_TmpG30,_TmpG30 + 147U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY41.tag==47){_Tmp0=yy1->YY41.val;{struct _tuple27*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1136
static union Cyc_YYSTYPE Cyc_YY41(struct _tuple27*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY41.tag=47U,_Tmp0.YY41.val=yy1;_Tmp0;});}static char _TmpG31[8U]="types_t";
# 1130 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG31,_TmpG31,_TmpG31 + 8U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY42.tag==48){_Tmp0=yy1->YY42.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1137
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY42.tag=48U,_Tmp0.YY42.val=yy1;_Tmp0;});}static char _TmpG32[24U]="list_t<designator_t,`H>";
# 1132 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG32,_TmpG32,_TmpG32 + 24U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY43.tag==49){_Tmp0=yy1->YY43.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1139
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY43.tag=49U,_Tmp0.YY43.val=yy1;_Tmp0;});}static char _TmpG33[13U]="designator_t";
# 1133 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG33,_TmpG33,_TmpG33 + 13U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY44.tag==50){_Tmp0=yy1->YY44.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1140
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY44.tag=50U,_Tmp0.YY44.val=yy1;_Tmp0;});}static char _TmpG34[7U]="kind_t";
# 1134 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG34,_TmpG34,_TmpG34 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY45.tag==51){_Tmp0=yy1->YY45.val;{struct Cyc_Absyn_Kind*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1141
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY45.tag=51U,_Tmp0.YY45.val=yy1;_Tmp0;});}static char _TmpG35[7U]="type_t";
# 1135 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG35,_TmpG35,_TmpG35 + 7U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY46.tag==52){_Tmp0=yy1->YY46.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1142
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY46.tag=52U,_Tmp0.YY46.val=yy1;_Tmp0;});}static char _TmpG36[23U]="list_t<attribute_t,`H>";
# 1136 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG36,_TmpG36,_TmpG36 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY47.tag==53){_Tmp0=yy1->YY47.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1143
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY47.tag=53U,_Tmp0.YY47.val=yy1;_Tmp0;});}static char _TmpG37[12U]="attribute_t";
# 1137 "parse.y"
static void*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG37,_TmpG37,_TmpG37 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY48.tag==54){_Tmp0=yy1->YY48.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1144
static union Cyc_YYSTYPE Cyc_YY48(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY48.tag=54U,_Tmp0.YY48.val=yy1;_Tmp0;});}static char _TmpG38[12U]="enumfield_t";
# 1138 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG38,_TmpG38,_TmpG38 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY49.tag==55){_Tmp0=yy1->YY49.val;{struct Cyc_Absyn_Enumfield*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1145
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY49.tag=55U,_Tmp0.YY49.val=yy1;_Tmp0;});}static char _TmpG39[23U]="list_t<enumfield_t,`H>";
# 1139 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG39,_TmpG39,_TmpG39 + 23U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY50.tag==56){_Tmp0=yy1->YY50.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1146
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY50.tag=56U,_Tmp0.YY50.val=yy1;_Tmp0;});}static char _TmpG3A[11U]="type_opt_t";
# 1140 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3A,_TmpG3A,_TmpG3A + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY51.tag==57){_Tmp0=yy1->YY51.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1147
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY51.tag=57U,_Tmp0.YY51.val=yy1;_Tmp0;});}static char _TmpG3B[69U]="$(list_t<$(type_t,type_t)@`H,`H>, list_t<$(type_t,type_t)@`H,`H>)*`H";
# 1142 "parse.y"
static struct _tuple28*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3B,_TmpG3B,_TmpG3B + 69U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY52.tag==58){_Tmp0=yy1->YY52.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1149
static union Cyc_YYSTYPE Cyc_YY52(struct _tuple28*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY52.tag=58U,_Tmp0.YY52.val=yy1;_Tmp0;});}static char _TmpG3C[21U]="$(type_t, type_t)@`H";
# 1143 "parse.y"
static struct _tuple29*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3C,_TmpG3C,_TmpG3C + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY53.tag==59){_Tmp0=yy1->YY53.val;{struct _tuple29*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1150
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple29*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY53.tag=59U,_Tmp0.YY53.val=yy1;_Tmp0;});}static char _TmpG3D[11U]="booltype_t";
# 1144 "parse.y"
static void*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3D,_TmpG3D,_TmpG3D + 11U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY54.tag==60){_Tmp0=yy1->YY54.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1151
static union Cyc_YYSTYPE Cyc_YY54(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY54.tag=60U,_Tmp0.YY54.val=yy1;_Tmp0;});}static char _TmpG3E[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1145 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3E,_TmpG3E,_TmpG3E + 35U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY55.tag==61){_Tmp0=yy1->YY55.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1152
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY55.tag=61U,_Tmp0.YY55.val=yy1;_Tmp0;});}static char _TmpG3F[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1146 "parse.y"
static struct _tuple30*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG3F,_TmpG3F,_TmpG3F + 48U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY56.tag==62){_Tmp0=yy1->YY56.val;{struct _tuple30*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1153
static union Cyc_YYSTYPE Cyc_YY56(struct _tuple30*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY56.tag=62U,_Tmp0.YY56.val=yy1;_Tmp0;});}static char _TmpG40[18U]="list_t<qvar_t,`H>";
# 1147 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG40,_TmpG40,_TmpG40 + 18U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY57.tag==63){_Tmp0=yy1->YY57.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1154
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY57.tag=63U,_Tmp0.YY57.val=yy1;_Tmp0;});}static char _TmpG41[12U]="aqualtype_t";
# 1148 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG41,_TmpG41,_TmpG41 + 12U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY58.tag==64){_Tmp0=yy1->YY58.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1155
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY58.tag=64U,_Tmp0.YY58.val=yy1;_Tmp0;});}static char _TmpG42[20U]="pointer_qual_t<`yy>";
# 1149 "parse.y"
static void*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG42,_TmpG42,_TmpG42 + 20U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY59.tag==65){_Tmp0=yy1->YY59.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1156
static union Cyc_YYSTYPE Cyc_YY59(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY59.tag=65U,_Tmp0.YY59.val=yy1;_Tmp0;});}static char _TmpG43[21U]="pointer_quals_t<`yy>";
# 1150 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG43,_TmpG43,_TmpG43 + 21U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY60.tag==66){_Tmp0=yy1->YY60.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1157
static union Cyc_YYSTYPE Cyc_YY60(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY60.tag=66U,_Tmp0.YY60.val=yy1;_Tmp0;});}static char _TmpG44[23U]="$(exp_opt_t,exp_opt_t)";
# 1152 "parse.y"
static struct _tuple16 Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG44,_TmpG44,_TmpG44 + 23U};
struct _tuple16 _Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY61.tag==67){_Tmp0=yy1->YY61.val;{struct _tuple16 yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1159
static union Cyc_YYSTYPE Cyc_YY61(struct _tuple16 yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY61.tag=67U,_Tmp0.YY61.val=yy1;_Tmp0;});}static char _TmpG45[10U]="raw_exp_t";
# 1153 "parse.y"
static void*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG45,_TmpG45,_TmpG45 + 10U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY62.tag==68){_Tmp0=yy1->YY62.val;{void*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1160
static union Cyc_YYSTYPE Cyc_YY62(void*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY62.tag=68U,_Tmp0.YY62.val=yy1;_Tmp0;});}static char _TmpG46[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1154 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG46,_TmpG46,_TmpG46 + 112U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY63.tag==69){_Tmp0=yy1->YY63.val;{struct _tuple31*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1161
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY63.tag=69U,_Tmp0.YY63.val=yy1;_Tmp0;});}static char _TmpG47[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1155 "parse.y"
static struct _tuple28*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG47,_TmpG47,_TmpG47 + 73U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY64.tag==70){_Tmp0=yy1->YY64.val;{struct _tuple28*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1162
static union Cyc_YYSTYPE Cyc_YY64(struct _tuple28*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY64.tag=70U,_Tmp0.YY64.val=yy1;_Tmp0;});}static char _TmpG48[28U]="list_t<string_t<`H>@`H, `H>";
# 1156 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG48,_TmpG48,_TmpG48 + 28U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY65.tag==71){_Tmp0=yy1->YY65.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1163
static union Cyc_YYSTYPE Cyc_YY65(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY65.tag=71U,_Tmp0.YY65.val=yy1;_Tmp0;});}static char _TmpG49[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1157 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG49,_TmpG49,_TmpG49 + 38U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY66.tag==72){_Tmp0=yy1->YY66.val;{struct Cyc_List_List*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1164
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY66.tag=72U,_Tmp0.YY66.val=yy1;_Tmp0;});}static char _TmpG4A[26U]="$(string_t<`H>, exp_t)@`H";
# 1158 "parse.y"
static struct _tuple32*Cyc_yyget_YY67(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_TmpG4A,_TmpG4A,_TmpG4A + 26U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY67.tag==73){_Tmp0=yy1->YY67.val;{struct _tuple32*yy=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1165
static union Cyc_YYSTYPE Cyc_YY67(struct _tuple32*yy1){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY67.tag=73U,_Tmp0.YY67.val=yy1;_Tmp0;});}static char _TmpG4B[16U]="exp_maker_fun_t";
# 1159 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY68(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_TmpG4B,_TmpG4B,_TmpG4B + 16U};
void*_Tmp0;if(((union Cyc_YYSTYPE*)yy1)->YY68.tag==74){_Tmp0=yy1->YY68.val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_Tmp0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1166
static union Cyc_YYSTYPE Cyc_YY68(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){return({union Cyc_YYSTYPE _Tmp0;_Tmp0.YY68.tag=74U,_Tmp0.YY68.val=yy1;_Tmp0;});}struct Cyc_Yyltype{int timestamp;unsigned first_line;unsigned first_column;unsigned last_line;unsigned last_column;};
# 1182
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _Tmp0;_Tmp0.timestamp=0,_Tmp0.first_line=0U,_Tmp0.first_column=0U,_Tmp0.last_line=0U,_Tmp0.last_column=0U;_Tmp0;});}
# 1185
struct Cyc_Yyltype Cyc_yylloc={0,0U,0U,0U,0U};
# 1196 "parse.y"
static short Cyc_yytranslate[391U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,162,2,2,147,160,157,2,144,145,140,154,139,158,149,159,2,2,2,2,2,2,2,2,2,2,148,136,142,141,143,153,152,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,150,2,151,156,146,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,137,155,138,161,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135};static char _TmpG4C[2U]="$";static char _TmpG4D[6U]="error";static char _TmpG4E[12U]="$undefined.";static char _TmpG4F[5U]="AUTO";static char _TmpG50[9U]="REGISTER";static char _TmpG51[7U]="STATIC";static char _TmpG52[7U]="EXTERN";static char _TmpG53[8U]="TYPEDEF";static char _TmpG54[5U]="VOID";static char _TmpG55[5U]="CHAR";static char _TmpG56[6U]="SHORT";static char _TmpG57[4U]="INT";static char _TmpG58[5U]="LONG";static char _TmpG59[6U]="FLOAT";static char _TmpG5A[7U]="DOUBLE";static char _TmpG5B[7U]="SIGNED";static char _TmpG5C[9U]="UNSIGNED";static char _TmpG5D[6U]="CONST";static char _TmpG5E[9U]="VOLATILE";static char _TmpG5F[9U]="RESTRICT";static char _TmpG60[7U]="STRUCT";static char _TmpG61[6U]="UNION";static char _TmpG62[5U]="CASE";static char _TmpG63[8U]="DEFAULT";static char _TmpG64[7U]="INLINE";static char _TmpG65[7U]="SIZEOF";static char _TmpG66[9U]="OFFSETOF";static char _TmpG67[3U]="IF";static char _TmpG68[5U]="ELSE";static char _TmpG69[7U]="SWITCH";static char _TmpG6A[6U]="WHILE";static char _TmpG6B[3U]="DO";static char _TmpG6C[4U]="FOR";static char _TmpG6D[5U]="GOTO";static char _TmpG6E[9U]="CONTINUE";static char _TmpG6F[6U]="BREAK";static char _TmpG70[7U]="RETURN";static char _TmpG71[5U]="ENUM";static char _TmpG72[7U]="TYPEOF";static char _TmpG73[16U]="BUILTIN_VA_LIST";static char _TmpG74[10U]="EXTENSION";static char _TmpG75[8U]="COMPLEX";static char _TmpG76[8U]="NULL_kw";static char _TmpG77[4U]="LET";static char _TmpG78[6U]="THROW";static char _TmpG79[4U]="TRY";static char _TmpG7A[6U]="CATCH";static char _TmpG7B[7U]="EXPORT";static char _TmpG7C[9U]="OVERRIDE";static char _TmpG7D[5U]="HIDE";static char _TmpG7E[4U]="NEW";static char _TmpG7F[9U]="ABSTRACT";static char _TmpG80[9U]="FALLTHRU";static char _TmpG81[6U]="USING";static char _TmpG82[10U]="NAMESPACE";static char _TmpG83[9U]="DATATYPE";static char _TmpG84[7U]="MALLOC";static char _TmpG85[8U]="RMALLOC";static char _TmpG86[9U]="RVMALLOC";static char _TmpG87[15U]="RMALLOC_INLINE";static char _TmpG88[7U]="CALLOC";static char _TmpG89[8U]="RCALLOC";static char _TmpG8A[5U]="SWAP";static char _TmpG8B[9U]="REGION_T";static char _TmpG8C[6U]="TAG_T";static char _TmpG8D[7U]="REGION";static char _TmpG8E[5U]="RNEW";static char _TmpG8F[8U]="REGIONS";static char _TmpG90[7U]="PORTON";static char _TmpG91[8U]="PORTOFF";static char _TmpG92[7U]="PRAGMA";static char _TmpG93[10U]="TEMPESTON";static char _TmpG94[11U]="TEMPESTOFF";static char _TmpG95[13U]="AQ_ALIASABLE";static char _TmpG96[10U]="AQ_DYNTRK";static char _TmpG97[10U]="AQ_REFCNT";static char _TmpG98[14U]="AQ_RESTRICTED";static char _TmpG99[10U]="AQ_UNIQUE";static char _TmpG9A[8U]="AQUAL_T";static char _TmpG9B[8U]="NUMELTS";static char _TmpG9C[8U]="VALUEOF";static char _TmpG9D[10U]="VALUEOF_T";static char _TmpG9E[9U]="TAGCHECK";static char _TmpG9F[13U]="NUMELTS_QUAL";static char _TmpGA0[10U]="THIN_QUAL";static char _TmpGA1[9U]="FAT_QUAL";static char _TmpGA2[13U]="NOTNULL_QUAL";static char _TmpGA3[14U]="NULLABLE_QUAL";static char _TmpGA4[14U]="REQUIRES_QUAL";static char _TmpGA5[13U]="ENSURES_QUAL";static char _TmpGA6[12U]="REGION_QUAL";static char _TmpGA7[16U]="NOZEROTERM_QUAL";static char _TmpGA8[14U]="ZEROTERM_QUAL";static char _TmpGA9[12U]="TAGGED_QUAL";static char _TmpGAA[12U]="ASSERT_QUAL";static char _TmpGAB[11U]="ALIAS_QUAL";static char _TmpGAC[7U]="AQUALS";static char _TmpGAD[16U]="EXTENSIBLE_QUAL";static char _TmpGAE[18U]="AUTORELEASED_QUAL";static char _TmpGAF[7U]="PTR_OP";static char _TmpGB0[7U]="INC_OP";static char _TmpGB1[7U]="DEC_OP";static char _TmpGB2[8U]="LEFT_OP";static char _TmpGB3[9U]="RIGHT_OP";static char _TmpGB4[6U]="LE_OP";static char _TmpGB5[6U]="GE_OP";static char _TmpGB6[6U]="EQ_OP";static char _TmpGB7[6U]="NE_OP";static char _TmpGB8[7U]="AND_OP";static char _TmpGB9[6U]="OR_OP";static char _TmpGBA[11U]="MUL_ASSIGN";static char _TmpGBB[11U]="DIV_ASSIGN";static char _TmpGBC[11U]="MOD_ASSIGN";static char _TmpGBD[11U]="ADD_ASSIGN";static char _TmpGBE[11U]="SUB_ASSIGN";static char _TmpGBF[12U]="LEFT_ASSIGN";static char _TmpGC0[13U]="RIGHT_ASSIGN";static char _TmpGC1[11U]="AND_ASSIGN";static char _TmpGC2[11U]="XOR_ASSIGN";static char _TmpGC3[10U]="OR_ASSIGN";static char _TmpGC4[9U]="ELLIPSIS";static char _TmpGC5[11U]="LEFT_RIGHT";static char _TmpGC6[12U]="COLON_COLON";static char _TmpGC7[11U]="IDENTIFIER";static char _TmpGC8[17U]="INTEGER_CONSTANT";static char _TmpGC9[7U]="STRING";static char _TmpGCA[8U]="WSTRING";static char _TmpGCB[19U]="CHARACTER_CONSTANT";static char _TmpGCC[20U]="WCHARACTER_CONSTANT";static char _TmpGCD[18U]="FLOATING_CONSTANT";static char _TmpGCE[9U]="TYPE_VAR";static char _TmpGCF[13U]="TYPEDEF_NAME";static char _TmpGD0[16U]="QUAL_IDENTIFIER";static char _TmpGD1[18U]="QUAL_TYPEDEF_NAME";static char _TmpGD2[10U]="ATTRIBUTE";static char _TmpGD3[8U]="ASM_TOK";static char _TmpGD4[4U]="';'";static char _TmpGD5[4U]="'{'";static char _TmpGD6[4U]="'}'";static char _TmpGD7[4U]="','";static char _TmpGD8[4U]="'*'";static char _TmpGD9[4U]="'='";static char _TmpGDA[4U]="'<'";static char _TmpGDB[4U]="'>'";static char _TmpGDC[4U]="'('";static char _TmpGDD[4U]="')'";static char _TmpGDE[4U]="'_'";static char _TmpGDF[4U]="'$'";static char _TmpGE0[4U]="':'";static char _TmpGE1[4U]="'.'";static char _TmpGE2[4U]="'['";static char _TmpGE3[4U]="']'";static char _TmpGE4[4U]="'@'";static char _TmpGE5[4U]="'?'";static char _TmpGE6[4U]="'+'";static char _TmpGE7[4U]="'|'";static char _TmpGE8[4U]="'^'";static char _TmpGE9[4U]="'&'";static char _TmpGEA[4U]="'-'";static char _TmpGEB[4U]="'/'";static char _TmpGEC[4U]="'%'";static char _TmpGED[4U]="'~'";static char _TmpGEE[4U]="'!'";static char _TmpGEF[5U]="prog";static char _TmpGF0[17U]="translation_unit";static char _TmpGF1[18U]="tempest_on_action";static char _TmpGF2[19U]="tempest_off_action";static char _TmpGF3[16U]="extern_c_action";static char _TmpGF4[13U]="end_extern_c";static char _TmpGF5[14U]="hide_list_opt";static char _TmpGF6[17U]="hide_list_values";static char _TmpGF7[16U]="export_list_opt";static char _TmpGF8[12U]="export_list";static char _TmpGF9[19U]="export_list_values";static char _TmpGFA[13U]="override_opt";static char _TmpGFB[21U]="external_declaration";static char _TmpGFC[14U]="optional_semi";static char _TmpGFD[20U]="function_definition";static char _TmpGFE[21U]="function_definition2";static char _TmpGFF[13U]="using_action";static char _TmpG100[15U]="unusing_action";static char _TmpG101[17U]="namespace_action";static char _TmpG102[19U]="unnamespace_action";static char _TmpG103[12U]="declaration";static char _TmpG104[9U]="open_opt";static char _TmpG105[17U]="declaration_list";static char _TmpG106[23U]="declaration_specifiers";static char _TmpG107[24U]="storage_class_specifier";static char _TmpG108[15U]="attributes_opt";static char _TmpG109[11U]="attributes";static char _TmpG10A[15U]="attribute_list";static char _TmpG10B[10U]="attribute";static char _TmpG10C[15U]="type_specifier";static char _TmpG10D[25U]="type_specifier_notypedef";static char _TmpG10E[5U]="kind";static char _TmpG10F[15U]="type_qualifier";static char _TmpG110[15U]="enum_specifier";static char _TmpG111[11U]="enum_field";static char _TmpG112[22U]="enum_declaration_list";static char _TmpG113[26U]="struct_or_union_specifier";static char _TmpG114[26U]="maybe_tagged_struct_union";static char _TmpG115[16U]="struct_or_union";static char _TmpG116[16U]="type_params_opt";static char _TmpG117[24U]="struct_declaration_list";static char _TmpG118[25U]="struct_declaration_list0";static char _TmpG119[25U]="init_declarator_list_rev";static char _TmpG11A[16U]="init_declarator";static char _TmpG11B[19U]="struct_declaration";static char _TmpG11C[25U]="specifier_qualifier_list";static char _TmpG11D[35U]="notypedef_specifier_qualifier_list";static char _TmpG11E[27U]="struct_declarator_list_rev";static char _TmpG11F[18U]="struct_declarator";static char _TmpG120[20U]="requires_clause_opt";static char _TmpG121[19U]="datatype_specifier";static char _TmpG122[14U]="qual_datatype";static char _TmpG123[19U]="datatypefield_list";static char _TmpG124[20U]="datatypefield_scope";static char _TmpG125[14U]="datatypefield";static char _TmpG126[11U]="declarator";static char _TmpG127[23U]="declarator_withtypedef";static char _TmpG128[18U]="direct_declarator";static char _TmpG129[30U]="direct_declarator_withtypedef";static char _TmpG12A[8U]="pointer";static char _TmpG12B[12U]="one_pointer";static char _TmpG12C[14U]="pointer_quals";static char _TmpG12D[13U]="pointer_qual";static char _TmpG12E[16U]="aqual_specifier";static char _TmpG12F[23U]="pointer_null_and_bound";static char _TmpG130[14U]="pointer_bound";static char _TmpG131[18U]="zeroterm_qual_opt";static char _TmpG132[8U]="rgn_opt";static char _TmpG133[11U]="tqual_list";static char _TmpG134[20U]="parameter_type_list";static char _TmpG135[9U]="type_var";static char _TmpG136[16U]="optional_effect";static char _TmpG137[23U]="optional_rgnpo_qualbnd";static char _TmpG138[14U]="rgnpo_qualbnd";static char _TmpG139[14U]="rgn_order_elt";static char _TmpG13A[13U]="qual_bnd_elt";static char _TmpG13B[14U]="qual_bnd_term";static char _TmpG13C[16U]="optional_inject";static char _TmpG13D[11U]="effect_set";static char _TmpG13E[14U]="atomic_effect";static char _TmpG13F[11U]="region_set";static char _TmpG140[15U]="parameter_list";static char _TmpG141[22U]="parameter_declaration";static char _TmpG142[16U]="identifier_list";static char _TmpG143[17U]="identifier_list0";static char _TmpG144[12U]="initializer";static char _TmpG145[18U]="array_initializer";static char _TmpG146[17U]="initializer_list";static char _TmpG147[12U]="designation";static char _TmpG148[16U]="designator_list";static char _TmpG149[11U]="designator";static char _TmpG14A[10U]="type_name";static char _TmpG14B[14U]="any_type_name";static char _TmpG14C[15U]="type_name_list";static char _TmpG14D[20U]="abstract_declarator";static char _TmpG14E[27U]="direct_abstract_declarator";static char _TmpG14F[25U]="requires_and_ensures_opt";static char _TmpG150[10U]="statement";static char _TmpG151[18U]="labeled_statement";static char _TmpG152[21U]="expression_statement";static char _TmpG153[19U]="compound_statement";static char _TmpG154[16U]="block_item_list";static char _TmpG155[20U]="selection_statement";static char _TmpG156[15U]="switch_clauses";static char _TmpG157[20U]="iteration_statement";static char _TmpG158[12U]="for_exp_opt";static char _TmpG159[15U]="jump_statement";static char _TmpG15A[12U]="exp_pattern";static char _TmpG15B[20U]="conditional_pattern";static char _TmpG15C[19U]="logical_or_pattern";static char _TmpG15D[20U]="logical_and_pattern";static char _TmpG15E[21U]="inclusive_or_pattern";static char _TmpG15F[21U]="exclusive_or_pattern";static char _TmpG160[12U]="and_pattern";static char _TmpG161[17U]="equality_pattern";static char _TmpG162[19U]="relational_pattern";static char _TmpG163[14U]="shift_pattern";static char _TmpG164[17U]="additive_pattern";static char _TmpG165[23U]="multiplicative_pattern";static char _TmpG166[13U]="cast_pattern";static char _TmpG167[14U]="unary_pattern";static char _TmpG168[16U]="postfix_pattern";static char _TmpG169[16U]="primary_pattern";static char _TmpG16A[8U]="pattern";static char _TmpG16B[19U]="tuple_pattern_list";static char _TmpG16C[20U]="tuple_pattern_list0";static char _TmpG16D[14U]="field_pattern";static char _TmpG16E[19U]="field_pattern_list";static char _TmpG16F[20U]="field_pattern_list0";static char _TmpG170[11U]="expression";static char _TmpG171[22U]="assignment_expression";static char _TmpG172[20U]="assignment_operator";static char _TmpG173[23U]="conditional_expression";static char _TmpG174[13U]="opt_qual_exp";static char _TmpG175[20U]="constant_expression";static char _TmpG176[22U]="logical_or_expression";static char _TmpG177[23U]="logical_and_expression";static char _TmpG178[24U]="inclusive_or_expression";static char _TmpG179[24U]="exclusive_or_expression";static char _TmpG17A[15U]="and_expression";static char _TmpG17B[20U]="equality_expression";static char _TmpG17C[22U]="relational_expression";static char _TmpG17D[17U]="shift_expression";static char _TmpG17E[20U]="additive_expression";static char _TmpG17F[26U]="multiplicative_expression";static char _TmpG180[12U]="equality_op";static char _TmpG181[14U]="relational_op";static char _TmpG182[12U]="additive_op";static char _TmpG183[18U]="multiplicative_op";static char _TmpG184[16U]="cast_expression";static char _TmpG185[17U]="unary_expression";static char _TmpG186[15U]="unary_operator";static char _TmpG187[9U]="asm_expr";static char _TmpG188[13U]="volatile_opt";static char _TmpG189[12U]="asm_out_opt";static char _TmpG18A[12U]="asm_outlist";static char _TmpG18B[11U]="asm_in_opt";static char _TmpG18C[11U]="asm_inlist";static char _TmpG18D[11U]="asm_io_elt";static char _TmpG18E[16U]="asm_clobber_opt";static char _TmpG18F[17U]="asm_clobber_list";static char _TmpG190[19U]="postfix_expression";static char _TmpG191[17U]="field_expression";static char _TmpG192[19U]="primary_expression";static char _TmpG193[25U]="argument_expression_list";static char _TmpG194[26U]="argument_expression_list0";static char _TmpG195[9U]="constant";static char _TmpG196[20U]="qual_opt_identifier";static char _TmpG197[17U]="qual_opt_typedef";static char _TmpG198[18U]="struct_union_name";static char _TmpG199[11U]="field_name";static char _TmpG19A[12U]="right_angle";
# 1563 "parse.y"
static struct _fat_ptr Cyc_yytname[335U]={{_TmpG4C,_TmpG4C,_TmpG4C + 2U},{_TmpG4D,_TmpG4D,_TmpG4D + 6U},{_TmpG4E,_TmpG4E,_TmpG4E + 12U},{_TmpG4F,_TmpG4F,_TmpG4F + 5U},{_TmpG50,_TmpG50,_TmpG50 + 9U},{_TmpG51,_TmpG51,_TmpG51 + 7U},{_TmpG52,_TmpG52,_TmpG52 + 7U},{_TmpG53,_TmpG53,_TmpG53 + 8U},{_TmpG54,_TmpG54,_TmpG54 + 5U},{_TmpG55,_TmpG55,_TmpG55 + 5U},{_TmpG56,_TmpG56,_TmpG56 + 6U},{_TmpG57,_TmpG57,_TmpG57 + 4U},{_TmpG58,_TmpG58,_TmpG58 + 5U},{_TmpG59,_TmpG59,_TmpG59 + 6U},{_TmpG5A,_TmpG5A,_TmpG5A + 7U},{_TmpG5B,_TmpG5B,_TmpG5B + 7U},{_TmpG5C,_TmpG5C,_TmpG5C + 9U},{_TmpG5D,_TmpG5D,_TmpG5D + 6U},{_TmpG5E,_TmpG5E,_TmpG5E + 9U},{_TmpG5F,_TmpG5F,_TmpG5F + 9U},{_TmpG60,_TmpG60,_TmpG60 + 7U},{_TmpG61,_TmpG61,_TmpG61 + 6U},{_TmpG62,_TmpG62,_TmpG62 + 5U},{_TmpG63,_TmpG63,_TmpG63 + 8U},{_TmpG64,_TmpG64,_TmpG64 + 7U},{_TmpG65,_TmpG65,_TmpG65 + 7U},{_TmpG66,_TmpG66,_TmpG66 + 9U},{_TmpG67,_TmpG67,_TmpG67 + 3U},{_TmpG68,_TmpG68,_TmpG68 + 5U},{_TmpG69,_TmpG69,_TmpG69 + 7U},{_TmpG6A,_TmpG6A,_TmpG6A + 6U},{_TmpG6B,_TmpG6B,_TmpG6B + 3U},{_TmpG6C,_TmpG6C,_TmpG6C + 4U},{_TmpG6D,_TmpG6D,_TmpG6D + 5U},{_TmpG6E,_TmpG6E,_TmpG6E + 9U},{_TmpG6F,_TmpG6F,_TmpG6F + 6U},{_TmpG70,_TmpG70,_TmpG70 + 7U},{_TmpG71,_TmpG71,_TmpG71 + 5U},{_TmpG72,_TmpG72,_TmpG72 + 7U},{_TmpG73,_TmpG73,_TmpG73 + 16U},{_TmpG74,_TmpG74,_TmpG74 + 10U},{_TmpG75,_TmpG75,_TmpG75 + 8U},{_TmpG76,_TmpG76,_TmpG76 + 8U},{_TmpG77,_TmpG77,_TmpG77 + 4U},{_TmpG78,_TmpG78,_TmpG78 + 6U},{_TmpG79,_TmpG79,_TmpG79 + 4U},{_TmpG7A,_TmpG7A,_TmpG7A + 6U},{_TmpG7B,_TmpG7B,_TmpG7B + 7U},{_TmpG7C,_TmpG7C,_TmpG7C + 9U},{_TmpG7D,_TmpG7D,_TmpG7D + 5U},{_TmpG7E,_TmpG7E,_TmpG7E + 4U},{_TmpG7F,_TmpG7F,_TmpG7F + 9U},{_TmpG80,_TmpG80,_TmpG80 + 9U},{_TmpG81,_TmpG81,_TmpG81 + 6U},{_TmpG82,_TmpG82,_TmpG82 + 10U},{_TmpG83,_TmpG83,_TmpG83 + 9U},{_TmpG84,_TmpG84,_TmpG84 + 7U},{_TmpG85,_TmpG85,_TmpG85 + 8U},{_TmpG86,_TmpG86,_TmpG86 + 9U},{_TmpG87,_TmpG87,_TmpG87 + 15U},{_TmpG88,_TmpG88,_TmpG88 + 7U},{_TmpG89,_TmpG89,_TmpG89 + 8U},{_TmpG8A,_TmpG8A,_TmpG8A + 5U},{_TmpG8B,_TmpG8B,_TmpG8B + 9U},{_TmpG8C,_TmpG8C,_TmpG8C + 6U},{_TmpG8D,_TmpG8D,_TmpG8D + 7U},{_TmpG8E,_TmpG8E,_TmpG8E + 5U},{_TmpG8F,_TmpG8F,_TmpG8F + 8U},{_TmpG90,_TmpG90,_TmpG90 + 7U},{_TmpG91,_TmpG91,_TmpG91 + 8U},{_TmpG92,_TmpG92,_TmpG92 + 7U},{_TmpG93,_TmpG93,_TmpG93 + 10U},{_TmpG94,_TmpG94,_TmpG94 + 11U},{_TmpG95,_TmpG95,_TmpG95 + 13U},{_TmpG96,_TmpG96,_TmpG96 + 10U},{_TmpG97,_TmpG97,_TmpG97 + 10U},{_TmpG98,_TmpG98,_TmpG98 + 14U},{_TmpG99,_TmpG99,_TmpG99 + 10U},{_TmpG9A,_TmpG9A,_TmpG9A + 8U},{_TmpG9B,_TmpG9B,_TmpG9B + 8U},{_TmpG9C,_TmpG9C,_TmpG9C + 8U},{_TmpG9D,_TmpG9D,_TmpG9D + 10U},{_TmpG9E,_TmpG9E,_TmpG9E + 9U},{_TmpG9F,_TmpG9F,_TmpG9F + 13U},{_TmpGA0,_TmpGA0,_TmpGA0 + 10U},{_TmpGA1,_TmpGA1,_TmpGA1 + 9U},{_TmpGA2,_TmpGA2,_TmpGA2 + 13U},{_TmpGA3,_TmpGA3,_TmpGA3 + 14U},{_TmpGA4,_TmpGA4,_TmpGA4 + 14U},{_TmpGA5,_TmpGA5,_TmpGA5 + 13U},{_TmpGA6,_TmpGA6,_TmpGA6 + 12U},{_TmpGA7,_TmpGA7,_TmpGA7 + 16U},{_TmpGA8,_TmpGA8,_TmpGA8 + 14U},{_TmpGA9,_TmpGA9,_TmpGA9 + 12U},{_TmpGAA,_TmpGAA,_TmpGAA + 12U},{_TmpGAB,_TmpGAB,_TmpGAB + 11U},{_TmpGAC,_TmpGAC,_TmpGAC + 7U},{_TmpGAD,_TmpGAD,_TmpGAD + 16U},{_TmpGAE,_TmpGAE,_TmpGAE + 18U},{_TmpGAF,_TmpGAF,_TmpGAF + 7U},{_TmpGB0,_TmpGB0,_TmpGB0 + 7U},{_TmpGB1,_TmpGB1,_TmpGB1 + 7U},{_TmpGB2,_TmpGB2,_TmpGB2 + 8U},{_TmpGB3,_TmpGB3,_TmpGB3 + 9U},{_TmpGB4,_TmpGB4,_TmpGB4 + 6U},{_TmpGB5,_TmpGB5,_TmpGB5 + 6U},{_TmpGB6,_TmpGB6,_TmpGB6 + 6U},{_TmpGB7,_TmpGB7,_TmpGB7 + 6U},{_TmpGB8,_TmpGB8,_TmpGB8 + 7U},{_TmpGB9,_TmpGB9,_TmpGB9 + 6U},{_TmpGBA,_TmpGBA,_TmpGBA + 11U},{_TmpGBB,_TmpGBB,_TmpGBB + 11U},{_TmpGBC,_TmpGBC,_TmpGBC + 11U},{_TmpGBD,_TmpGBD,_TmpGBD + 11U},{_TmpGBE,_TmpGBE,_TmpGBE + 11U},{_TmpGBF,_TmpGBF,_TmpGBF + 12U},{_TmpGC0,_TmpGC0,_TmpGC0 + 13U},{_TmpGC1,_TmpGC1,_TmpGC1 + 11U},{_TmpGC2,_TmpGC2,_TmpGC2 + 11U},{_TmpGC3,_TmpGC3,_TmpGC3 + 10U},{_TmpGC4,_TmpGC4,_TmpGC4 + 9U},{_TmpGC5,_TmpGC5,_TmpGC5 + 11U},{_TmpGC6,_TmpGC6,_TmpGC6 + 12U},{_TmpGC7,_TmpGC7,_TmpGC7 + 11U},{_TmpGC8,_TmpGC8,_TmpGC8 + 17U},{_TmpGC9,_TmpGC9,_TmpGC9 + 7U},{_TmpGCA,_TmpGCA,_TmpGCA + 8U},{_TmpGCB,_TmpGCB,_TmpGCB + 19U},{_TmpGCC,_TmpGCC,_TmpGCC + 20U},{_TmpGCD,_TmpGCD,_TmpGCD + 18U},{_TmpGCE,_TmpGCE,_TmpGCE + 9U},{_TmpGCF,_TmpGCF,_TmpGCF + 13U},{_TmpGD0,_TmpGD0,_TmpGD0 + 16U},{_TmpGD1,_TmpGD1,_TmpGD1 + 18U},{_TmpGD2,_TmpGD2,_TmpGD2 + 10U},{_TmpGD3,_TmpGD3,_TmpGD3 + 8U},{_TmpGD4,_TmpGD4,_TmpGD4 + 4U},{_TmpGD5,_TmpGD5,_TmpGD5 + 4U},{_TmpGD6,_TmpGD6,_TmpGD6 + 4U},{_TmpGD7,_TmpGD7,_TmpGD7 + 4U},{_TmpGD8,_TmpGD8,_TmpGD8 + 4U},{_TmpGD9,_TmpGD9,_TmpGD9 + 4U},{_TmpGDA,_TmpGDA,_TmpGDA + 4U},{_TmpGDB,_TmpGDB,_TmpGDB + 4U},{_TmpGDC,_TmpGDC,_TmpGDC + 4U},{_TmpGDD,_TmpGDD,_TmpGDD + 4U},{_TmpGDE,_TmpGDE,_TmpGDE + 4U},{_TmpGDF,_TmpGDF,_TmpGDF + 4U},{_TmpGE0,_TmpGE0,_TmpGE0 + 4U},{_TmpGE1,_TmpGE1,_TmpGE1 + 4U},{_TmpGE2,_TmpGE2,_TmpGE2 + 4U},{_TmpGE3,_TmpGE3,_TmpGE3 + 4U},{_TmpGE4,_TmpGE4,_TmpGE4 + 4U},{_TmpGE5,_TmpGE5,_TmpGE5 + 4U},{_TmpGE6,_TmpGE6,_TmpGE6 + 4U},{_TmpGE7,_TmpGE7,_TmpGE7 + 4U},{_TmpGE8,_TmpGE8,_TmpGE8 + 4U},{_TmpGE9,_TmpGE9,_TmpGE9 + 4U},{_TmpGEA,_TmpGEA,_TmpGEA + 4U},{_TmpGEB,_TmpGEB,_TmpGEB + 4U},{_TmpGEC,_TmpGEC,_TmpGEC + 4U},{_TmpGED,_TmpGED,_TmpGED + 4U},{_TmpGEE,_TmpGEE,_TmpGEE + 4U},{_TmpGEF,_TmpGEF,_TmpGEF + 5U},{_TmpGF0,_TmpGF0,_TmpGF0 + 17U},{_TmpGF1,_TmpGF1,_TmpGF1 + 18U},{_TmpGF2,_TmpGF2,_TmpGF2 + 19U},{_TmpGF3,_TmpGF3,_TmpGF3 + 16U},{_TmpGF4,_TmpGF4,_TmpGF4 + 13U},{_TmpGF5,_TmpGF5,_TmpGF5 + 14U},{_TmpGF6,_TmpGF6,_TmpGF6 + 17U},{_TmpGF7,_TmpGF7,_TmpGF7 + 16U},{_TmpGF8,_TmpGF8,_TmpGF8 + 12U},{_TmpGF9,_TmpGF9,_TmpGF9 + 19U},{_TmpGFA,_TmpGFA,_TmpGFA + 13U},{_TmpGFB,_TmpGFB,_TmpGFB + 21U},{_TmpGFC,_TmpGFC,_TmpGFC + 14U},{_TmpGFD,_TmpGFD,_TmpGFD + 20U},{_TmpGFE,_TmpGFE,_TmpGFE + 21U},{_TmpGFF,_TmpGFF,_TmpGFF + 13U},{_TmpG100,_TmpG100,_TmpG100 + 15U},{_TmpG101,_TmpG101,_TmpG101 + 17U},{_TmpG102,_TmpG102,_TmpG102 + 19U},{_TmpG103,_TmpG103,_TmpG103 + 12U},{_TmpG104,_TmpG104,_TmpG104 + 9U},{_TmpG105,_TmpG105,_TmpG105 + 17U},{_TmpG106,_TmpG106,_TmpG106 + 23U},{_TmpG107,_TmpG107,_TmpG107 + 24U},{_TmpG108,_TmpG108,_TmpG108 + 15U},{_TmpG109,_TmpG109,_TmpG109 + 11U},{_TmpG10A,_TmpG10A,_TmpG10A + 15U},{_TmpG10B,_TmpG10B,_TmpG10B + 10U},{_TmpG10C,_TmpG10C,_TmpG10C + 15U},{_TmpG10D,_TmpG10D,_TmpG10D + 25U},{_TmpG10E,_TmpG10E,_TmpG10E + 5U},{_TmpG10F,_TmpG10F,_TmpG10F + 15U},{_TmpG110,_TmpG110,_TmpG110 + 15U},{_TmpG111,_TmpG111,_TmpG111 + 11U},{_TmpG112,_TmpG112,_TmpG112 + 22U},{_TmpG113,_TmpG113,_TmpG113 + 26U},{_TmpG114,_TmpG114,_TmpG114 + 26U},{_TmpG115,_TmpG115,_TmpG115 + 16U},{_TmpG116,_TmpG116,_TmpG116 + 16U},{_TmpG117,_TmpG117,_TmpG117 + 24U},{_TmpG118,_TmpG118,_TmpG118 + 25U},{_TmpG119,_TmpG119,_TmpG119 + 25U},{_TmpG11A,_TmpG11A,_TmpG11A + 16U},{_TmpG11B,_TmpG11B,_TmpG11B + 19U},{_TmpG11C,_TmpG11C,_TmpG11C + 25U},{_TmpG11D,_TmpG11D,_TmpG11D + 35U},{_TmpG11E,_TmpG11E,_TmpG11E + 27U},{_TmpG11F,_TmpG11F,_TmpG11F + 18U},{_TmpG120,_TmpG120,_TmpG120 + 20U},{_TmpG121,_TmpG121,_TmpG121 + 19U},{_TmpG122,_TmpG122,_TmpG122 + 14U},{_TmpG123,_TmpG123,_TmpG123 + 19U},{_TmpG124,_TmpG124,_TmpG124 + 20U},{_TmpG125,_TmpG125,_TmpG125 + 14U},{_TmpG126,_TmpG126,_TmpG126 + 11U},{_TmpG127,_TmpG127,_TmpG127 + 23U},{_TmpG128,_TmpG128,_TmpG128 + 18U},{_TmpG129,_TmpG129,_TmpG129 + 30U},{_TmpG12A,_TmpG12A,_TmpG12A + 8U},{_TmpG12B,_TmpG12B,_TmpG12B + 12U},{_TmpG12C,_TmpG12C,_TmpG12C + 14U},{_TmpG12D,_TmpG12D,_TmpG12D + 13U},{_TmpG12E,_TmpG12E,_TmpG12E + 16U},{_TmpG12F,_TmpG12F,_TmpG12F + 23U},{_TmpG130,_TmpG130,_TmpG130 + 14U},{_TmpG131,_TmpG131,_TmpG131 + 18U},{_TmpG132,_TmpG132,_TmpG132 + 8U},{_TmpG133,_TmpG133,_TmpG133 + 11U},{_TmpG134,_TmpG134,_TmpG134 + 20U},{_TmpG135,_TmpG135,_TmpG135 + 9U},{_TmpG136,_TmpG136,_TmpG136 + 16U},{_TmpG137,_TmpG137,_TmpG137 + 23U},{_TmpG138,_TmpG138,_TmpG138 + 14U},{_TmpG139,_TmpG139,_TmpG139 + 14U},{_TmpG13A,_TmpG13A,_TmpG13A + 13U},{_TmpG13B,_TmpG13B,_TmpG13B + 14U},{_TmpG13C,_TmpG13C,_TmpG13C + 16U},{_TmpG13D,_TmpG13D,_TmpG13D + 11U},{_TmpG13E,_TmpG13E,_TmpG13E + 14U},{_TmpG13F,_TmpG13F,_TmpG13F + 11U},{_TmpG140,_TmpG140,_TmpG140 + 15U},{_TmpG141,_TmpG141,_TmpG141 + 22U},{_TmpG142,_TmpG142,_TmpG142 + 16U},{_TmpG143,_TmpG143,_TmpG143 + 17U},{_TmpG144,_TmpG144,_TmpG144 + 12U},{_TmpG145,_TmpG145,_TmpG145 + 18U},{_TmpG146,_TmpG146,_TmpG146 + 17U},{_TmpG147,_TmpG147,_TmpG147 + 12U},{_TmpG148,_TmpG148,_TmpG148 + 16U},{_TmpG149,_TmpG149,_TmpG149 + 11U},{_TmpG14A,_TmpG14A,_TmpG14A + 10U},{_TmpG14B,_TmpG14B,_TmpG14B + 14U},{_TmpG14C,_TmpG14C,_TmpG14C + 15U},{_TmpG14D,_TmpG14D,_TmpG14D + 20U},{_TmpG14E,_TmpG14E,_TmpG14E + 27U},{_TmpG14F,_TmpG14F,_TmpG14F + 25U},{_TmpG150,_TmpG150,_TmpG150 + 10U},{_TmpG151,_TmpG151,_TmpG151 + 18U},{_TmpG152,_TmpG152,_TmpG152 + 21U},{_TmpG153,_TmpG153,_TmpG153 + 19U},{_TmpG154,_TmpG154,_TmpG154 + 16U},{_TmpG155,_TmpG155,_TmpG155 + 20U},{_TmpG156,_TmpG156,_TmpG156 + 15U},{_TmpG157,_TmpG157,_TmpG157 + 20U},{_TmpG158,_TmpG158,_TmpG158 + 12U},{_TmpG159,_TmpG159,_TmpG159 + 15U},{_TmpG15A,_TmpG15A,_TmpG15A + 12U},{_TmpG15B,_TmpG15B,_TmpG15B + 20U},{_TmpG15C,_TmpG15C,_TmpG15C + 19U},{_TmpG15D,_TmpG15D,_TmpG15D + 20U},{_TmpG15E,_TmpG15E,_TmpG15E + 21U},{_TmpG15F,_TmpG15F,_TmpG15F + 21U},{_TmpG160,_TmpG160,_TmpG160 + 12U},{_TmpG161,_TmpG161,_TmpG161 + 17U},{_TmpG162,_TmpG162,_TmpG162 + 19U},{_TmpG163,_TmpG163,_TmpG163 + 14U},{_TmpG164,_TmpG164,_TmpG164 + 17U},{_TmpG165,_TmpG165,_TmpG165 + 23U},{_TmpG166,_TmpG166,_TmpG166 + 13U},{_TmpG167,_TmpG167,_TmpG167 + 14U},{_TmpG168,_TmpG168,_TmpG168 + 16U},{_TmpG169,_TmpG169,_TmpG169 + 16U},{_TmpG16A,_TmpG16A,_TmpG16A + 8U},{_TmpG16B,_TmpG16B,_TmpG16B + 19U},{_TmpG16C,_TmpG16C,_TmpG16C + 20U},{_TmpG16D,_TmpG16D,_TmpG16D + 14U},{_TmpG16E,_TmpG16E,_TmpG16E + 19U},{_TmpG16F,_TmpG16F,_TmpG16F + 20U},{_TmpG170,_TmpG170,_TmpG170 + 11U},{_TmpG171,_TmpG171,_TmpG171 + 22U},{_TmpG172,_TmpG172,_TmpG172 + 20U},{_TmpG173,_TmpG173,_TmpG173 + 23U},{_TmpG174,_TmpG174,_TmpG174 + 13U},{_TmpG175,_TmpG175,_TmpG175 + 20U},{_TmpG176,_TmpG176,_TmpG176 + 22U},{_TmpG177,_TmpG177,_TmpG177 + 23U},{_TmpG178,_TmpG178,_TmpG178 + 24U},{_TmpG179,_TmpG179,_TmpG179 + 24U},{_TmpG17A,_TmpG17A,_TmpG17A + 15U},{_TmpG17B,_TmpG17B,_TmpG17B + 20U},{_TmpG17C,_TmpG17C,_TmpG17C + 22U},{_TmpG17D,_TmpG17D,_TmpG17D + 17U},{_TmpG17E,_TmpG17E,_TmpG17E + 20U},{_TmpG17F,_TmpG17F,_TmpG17F + 26U},{_TmpG180,_TmpG180,_TmpG180 + 12U},{_TmpG181,_TmpG181,_TmpG181 + 14U},{_TmpG182,_TmpG182,_TmpG182 + 12U},{_TmpG183,_TmpG183,_TmpG183 + 18U},{_TmpG184,_TmpG184,_TmpG184 + 16U},{_TmpG185,_TmpG185,_TmpG185 + 17U},{_TmpG186,_TmpG186,_TmpG186 + 15U},{_TmpG187,_TmpG187,_TmpG187 + 9U},{_TmpG188,_TmpG188,_TmpG188 + 13U},{_TmpG189,_TmpG189,_TmpG189 + 12U},{_TmpG18A,_TmpG18A,_TmpG18A + 12U},{_TmpG18B,_TmpG18B,_TmpG18B + 11U},{_TmpG18C,_TmpG18C,_TmpG18C + 11U},{_TmpG18D,_TmpG18D,_TmpG18D + 11U},{_TmpG18E,_TmpG18E,_TmpG18E + 16U},{_TmpG18F,_TmpG18F,_TmpG18F + 17U},{_TmpG190,_TmpG190,_TmpG190 + 19U},{_TmpG191,_TmpG191,_TmpG191 + 17U},{_TmpG192,_TmpG192,_TmpG192 + 19U},{_TmpG193,_TmpG193,_TmpG193 + 25U},{_TmpG194,_TmpG194,_TmpG194 + 26U},{_TmpG195,_TmpG195,_TmpG195 + 9U},{_TmpG196,_TmpG196,_TmpG196 + 20U},{_TmpG197,_TmpG197,_TmpG197 + 17U},{_TmpG198,_TmpG198,_TmpG198 + 18U},{_TmpG199,_TmpG199,_TmpG199 + 11U},{_TmpG19A,_TmpG19A,_TmpG19A + 12U}};
# 1625
static short Cyc_yyr1[572U]={0,163,164,164,164,164,164,164,164,164,164,164,164,165,166,167,168,169,169,170,170,170,171,171,172,172,172,173,173,174,174,175,175,175,176,176,177,177,177,177,178,178,179,180,181,182,183,183,183,183,183,183,184,184,185,185,186,186,186,186,186,186,186,186,186,186,186,187,187,187,187,187,187,187,188,188,189,190,190,191,191,191,191,192,192,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,193,194,195,195,195,196,196,196,197,197,198,198,198,199,199,199,200,200,201,201,202,202,203,203,204,204,205,205,206,206,207,208,208,208,208,208,208,209,209,209,209,209,209,210,210,211,211,211,211,212,212,213,213,213,214,214,215,215,215,215,216,216,216,217,217,218,218,219,219,220,220,220,220,220,220,220,220,220,221,221,221,221,221,221,221,221,221,221,222,222,223,224,224,225,225,225,225,225,225,225,225,225,225,226,226,226,226,226,226,226,227,227,227,228,228,229,229,229,230,230,230,231,231,232,232,232,232,232,233,233,234,234,235,235,236,236,236,236,237,238,239,239,239,239,239,239,240,240,241,241,242,242,242,242,243,243,244,244,245,245,245,246,247,247,248,248,249,249,249,249,249,250,250,250,250,251,251,252,252,253,253,254,254,255,255,255,255,255,255,256,256,257,257,257,258,258,258,258,258,258,258,258,258,259,259,259,259,260,260,260,260,260,260,261,262,262,263,263,264,264,264,264,264,264,264,264,265,265,265,265,265,265,266,266,266,266,266,266,267,267,267,267,268,268,269,269,269,269,269,269,269,269,270,271,271,272,272,273,273,274,274,275,275,276,276,277,277,278,278,279,279,279,280,280,281,281,282,282,283,283,283,283,283,284,285,286,286,286,286,286,286,286,286,286,286,286,286,286,286,286,286,286,287,287,287,288,288,289,289,290,290,290,291,291,292,292,293,293,293,294,294,294,294,294,294,294,294,294,294,294,295,295,295,295,295,295,295,296,296,297,298,298,299,299,300,300,301,301,302,302,303,303,304,304,305,305,305,306,306,307,307,308,308,309,309,309,309,310,310,311,311,311,312,312,313,313,313,313,313,313,313,313,313,313,313,313,313,313,313,313,313,313,313,313,313,313,314,314,314,314,315,316,316,317,317,317,318,318,319,319,319,320,320,321,322,322,322,323,323,324,324,324,324,324,324,324,324,324,324,324,325,325,325,325,326,326,326,326,326,326,326,326,326,326,326,327,328,328,329,329,329,329,329,330,330,331,331,332,332,333,333,334,334};
# 1686
static short Cyc_yyr2[572U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,1,4,1,1,1,1,1,1,4,2,2,1,0,3,0,1,1,0,1,1,0,2,2,3,5,5,7,1,3,0,2,0,2,1,1,3,3,3,3,1,1,1,1,1,4,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,1,3,1,1,2,3,3,4,4,5,4,5,4,2,0,4,4,8,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,3,3,6,6,0,3,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,6,6,6,9,11,4,6,6,4,2,2,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1747
static short Cyc_yydefact[1115U]={0,33,67,68,69,70,72,85,86,87,88,89,90,91,92,93,111,112,113,127,128,63,0,0,98,0,94,0,73,0,0,163,105,108,0,0,0,13,14,0,0,0,0,562,237,564,563,565,0,222,0,101,0,222,221,1,0,0,0,0,31,0,0,32,0,56,65,59,83,61,95,96,0,126,99,0,0,174,0,197,200,100,178,129,71,70,64,0,115,0,58,560,0,562,557,558,559,561,0,129,0,0,394,0,0,0,270,0,398,396,42,44,0,0,52,0,0,0,0,0,125,164,0,0,0,219,0,0,0,0,220,0,0,0,2,0,0,0,0,46,0,135,137,57,66,60,62,566,567,129,131,129,0,54,0,0,35,0,239,0,186,175,198,0,204,205,209,210,0,208,207,0,206,227,200,0,84,71,119,0,117,0,0,0,0,0,446,0,0,0,0,0,0,446,0,0,0,0,0,0,0,546,547,510,0,0,0,508,0,507,505,506,0,423,425,439,449,451,453,455,457,459,461,463,466,468,481,0,483,528,545,543,562,406,0,0,0,0,407,0,0,397,49,0,0,129,0,0,249,252,251,253,250,0,0,144,140,142,290,297,292,0,0,0,0,0,8,9,0,0,568,569,238,110,0,0,0,179,102,268,0,265,10,11,0,3,0,5,0,47,0,0,0,35,124,0,132,133,156,0,161,0,0,0,0,0,0,0,0,0,0,0,0,562,323,325,0,333,327,0,331,316,317,318,0,319,320,321,0,55,35,137,34,36,298,0,255,271,0,0,241,239,0,224,0,0,0,0,229,74,228,201,0,120,116,0,0,0,490,0,0,503,441,481,0,0,0,0,0,0,0,0,0,0,0,0,0,0,484,485,511,502,0,487,0,0,0,0,486,0,97,0,0,0,0,0,0,470,471,0,474,475,472,473,0,0,0,476,477,0,478,479,480,0,0,429,430,431,432,433,434,435,436,437,438,428,0,488,0,534,535,0,0,0,549,0,129,399,0,0,0,420,562,569,0,0,0,0,286,416,421,0,418,0,0,395,413,414,0,411,272,0,0,0,0,0,293,0,263,145,150,146,148,141,143,239,0,300,291,301,571,570,0,104,107,0,51,0,106,109,80,79,0,77,223,180,239,267,176,300,269,187,188,0,103,16,29,43,0,45,0,136,0,138,274,273,35,37,129,122,134,0,0,0,152,159,0,129,169,0,0,0,0,0,562,0,351,0,354,355,356,0,0,358,0,0,0,334,328,137,332,326,324,38,0,185,256,0,0,0,262,240,257,312,0,232,0,241,184,226,225,181,224,0,0,212,215,214,216,213,0,217,0,230,75,130,121,448,118,114,0,0,0,0,0,442,443,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,548,555,0,554,424,450,0,452,454,456,458,460,462,464,465,467,469,427,426,533,530,0,532,0,0,0,409,410,0,289,0,417,284,287,405,0,285,408,402,0,48,0,403,0,0,294,0,151,147,149,0,0,224,0,302,0,239,0,311,296,0,0,0,0,0,144,0,129,0,239,0,196,177,266,0,22,4,6,0,562,275,280,0,0,39,241,0,155,139,156,0,0,154,162,171,170,0,0,165,0,0,0,341,0,0,0,351,0,352,353,357,0,0,0,329,322,0,40,299,239,0,259,0,0,0,0,183,242,243,244,0,0,255,233,182,202,203,0,211,230,199,489,0,0,447,492,0,0,0,0,0,0,544,498,501,0,0,504,512,0,0,482,552,0,0,531,529,0,0,0,0,288,419,422,412,415,404,295,254,264,312,303,304,224,0,0,224,0,0,50,562,0,76,78,0,189,0,0,0,224,0,0,0,17,23,0,276,0,281,131,153,0,157,160,172,169,169,0,0,0,0,0,0,0,0,0,351,341,359,0,330,41,241,0,260,258,0,0,0,0,0,0,241,0,0,231,540,0,539,0,0,0,0,0,0,533,532,517,0,553,536,0,556,440,550,551,0,400,401,308,306,310,312,305,224,53,0,81,190,195,312,194,191,224,0,0,0,0,0,277,282,0,0,0,0,168,167,335,341,0,0,0,0,0,361,362,364,366,368,370,372,374,376,378,381,383,385,387,392,393,0,0,338,347,0,351,0,0,360,235,261,0,0,245,246,248,247,234,239,218,491,0,493,494,495,0,0,444,445,500,499,0,523,517,513,515,509,537,0,309,307,0,193,192,30,25,0,0,35,0,7,0,283,123,158,0,0,0,341,0,390,0,0,341,0,0,0,0,0,0,0,0,0,0,0,0,0,388,341,0,0,351,340,313,314,241,542,541,0,0,0,524,523,520,518,0,514,538,0,26,24,0,27,0,19,0,173,336,337,0,0,0,0,341,343,365,0,367,369,371,373,375,377,379,380,382,384,0,342,348,0,0,0,236,0,0,0,526,525,0,519,516,0,28,18,20,0,0,0,339,389,0,386,344,0,341,350,0,0,496,0,522,0,521,82,21,279,278,0,363,341,345,349,0,0,527,391,346,315,497,0,0,0};
# 1862
static short Cyc_yydefgoto[172U]={1112,55,56,57,58,506,899,1035,815,816,979,687,59,333,60,317,61,508,62,510,63,264,154,64,65,583,255,492,493,256,68,272,257,70,178,179,71,72,73,176,295,296,140,141,297,258,473,523,524,704,74,75,708,709,710,76,525,77,498,78,79,173,174,582,80,125,571,349,752,662,81,340,565,739,740,741,259,557,561,743,467,341,281,105,106,693,514,694,446,447,448,260,334,335,663,479,738,320,321,322,323,324,325,834,326,719,327,915,916,917,918,919,920,921,922,923,924,925,926,927,928,929,930,449,458,459,450,451,452,328,213,426,214,365,588,215,216,217,218,219,220,221,222,223,224,399,404,409,413,225,226,227,381,382,870,965,966,1024,967,1026,1070,228,859,229,614,615,230,231,83,980,453,483};
# 1883
static short Cyc_yypact[1115U]={3063,- -32768,- -32768,- -32768,- -32768,18,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4051,224,- 3,- -32768,4051,- -32768,3196,- -32768,- 45,157,- -32768,74,149,118,165,179,- -32768,- -32768,161,190,171,303,- -32768,226,- -32768,- -32768,- -32768,216,236,665,270,302,236,- -32768,- -32768,306,322,338,2909,- -32768,293,424,- -32768,751,4051,4051,4051,- -32768,4051,- -32768,- -32768,174,353,- -32768,- 45,3950,290,167,156,1154,- -32768,- -32768,352,374,428,- -32768,- 45,398,6684,- -32768,- -32768,3343,260,- -32768,- -32768,- -32768,- -32768,426,352,452,6684,- -32768,435,3343,449,448,455,- -32768,141,- -32768,- -32768,4149,4149,457,470,2909,2909,626,6684,- -32768,- -32768,232,478,6684,- -32768,189,479,232,4824,- -32768,2909,2909,3216,- -32768,2909,3216,2909,3216,- -32768,196,- -32768,3805,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,352,4824,327,1796,- -32768,3950,751,495,4149,4250,5700,- -32768,290,- -32768,497,- -32768,- -32768,- -32768,- -32768,533,- -32768,- -32768,543,- -32768,- 17,1154,4149,- -32768,- -32768,476,530,498,- 45,7017,545,7125,6684,568,585,589,594,597,606,616,568,618,625,630,642,645,7125,7125,- -32768,- -32768,761,7176,2591,654,- -32768,7176,- -32768,- -32768,- -32768,57,- -32768,- -32768,- 56,700,651,655,673,552,121,602,217,93,- -32768,840,7176,73,- 69,- -32768,697,126,- -32768,3343,59,705,1459,714,318,2889,- -32768,- -32768,715,6684,352,2889,695,- -32768,- -32768,- -32768,- -32768,- -32768,696,4530,4824,1374,4824,596,- -32768,- -32768,92,92,719,709,708,- -32768,- -32768,43,343,- -32768,- -32768,- -32768,- -32768,53,710,707,- -32768,- -32768,871,350,- -32768,- -32768,- -32768,716,- -32768,718,- -32768,720,- -32768,189,1130,3950,495,729,730,4824,- -32768,732,- 45,734,713,111,725,4947,731,750,717,741,5823,2436,4947,201,733,- -32768,- -32768,745,1956,1956,751,1956,- -32768,- -32768,- -32768,755,- -32768,- -32768,- -32768,255,- -32768,495,738,- -32768,- -32768,743,1,775,- -32768,26,756,752,341,760,633,757,6684,4149,410,- -32768,772,- -32768,- -32768,1,- 45,- -32768,6684,771,2591,- -32768,4824,2591,- -32768,- -32768,- -32768,1234,6858,6684,6684,6684,6684,6684,6684,768,790,6684,4824,1356,6684,- -32768,- -32768,- -32768,- -32768,770,- -32768,1956,773,368,6684,- -32768,6684,- -32768,7176,6684,7176,7176,7176,7176,- -32768,- -32768,7176,- -32768,- -32768,- -32768,- -32768,7176,7176,7176,- -32768,- -32768,7176,- -32768,- -32768,- -32768,7176,6684,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6684,- -32768,232,- -32768,- -32768,5946,232,6684,- -32768,774,352,- -32768,776,777,780,- -32768,67,426,232,6684,3343,298,- -32768,- -32768,- -32768,791,789,767,3343,- -32768,- -32768,- -32768,793,795,- -32768,392,1459,796,4149,4149,- -32768,804,807,- -32768,1374,1374,1374,- -32768,- -32768,3508,6069,380,- -32768,544,- -32768,- -32768,26,- -32768,- -32768,799,- -32768,825,- -32768,- -32768,- -32768,816,820,822,- -32768,- -32768,3362,- -32768,577,440,- -32768,- -32768,- -32768,4824,- -32768,- -32768,919,- -32768,2909,- -32768,2909,- -32768,5070,- -32768,- -32768,- -32768,495,- -32768,352,- -32768,- -32768,908,6684,471,- -32768,- 59,256,352,739,6684,6684,824,832,6684,826,940,2276,837,- -32768,- -32768,- -32768,472,929,- -32768,6192,2116,2746,- -32768,- -32768,3805,- -32768,- -32768,- -32768,- -32768,4149,- -32768,- -32768,4824,833,4628,- -32768,- -32768,828,664,163,- -32768,4726,752,- -32768,- -32768,- -32768,- -32768,633,835,168,- -32768,- -32768,- -32768,- -32768,- -32768,834,- -32768,838,749,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,839,848,843,6315,77,- -32768,880,845,853,854,857,858,860,6684,861,864,865,415,867,889,879,6966,- -32768,- -32768,873,866,- -32768,700,191,651,655,673,552,121,602,217,217,93,- -32768,- -32768,- -32768,- -32768,- -32768,874,- -32768,106,4149,5574,4824,- -32768,4824,- -32768,869,- -32768,- -32768,- -32768,- -32768,3284,- -32768,- -32768,- -32768,3043,- -32768,884,- -32768,214,231,- -32768,4824,- -32768,- -32768,- -32768,885,890,633,878,544,4149,4390,6438,- -32768,- -32768,6684,898,6807,891,53,3654,892,352,4149,4250,6561,- -32768,577,- -32768,909,998,- -32768,- -32768,926,902,- -32768,- -32768,617,1130,- -32768,752,931,- -32768,- -32768,732,907,6684,- -32768,- -32768,- -32768,- -32768,917,- 45,493,409,411,6684,754,438,4947,912,6684,921,927,- -32768,- -32768,928,922,923,1956,- -32768,3950,- -32768,743,933,4149,- -32768,932,26,934,935,- -32768,- -32768,937,938,967,930,- 33,- -32768,- -32768,- -32768,- -32768,4149,- -32768,749,- -32768,944,369,944,- -32768,- -32768,6684,6684,6684,1057,6684,460,- -32768,- -32768,- -32768,232,232,- -32768,941,943,5196,- -32768,- -32768,6684,6684,- -32768,- -32768,1,644,962,963,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,664,- -32768,- -32768,633,1,945,633,946,501,- -32768,952,947,- -32768,- -32768,949,- -32768,1,950,951,633,948,3216,961,1051,- -32768,965,- -32768,5322,- -32768,4824,- -32768,6684,- -32768,- -32768,964,75,739,4947,974,968,1416,966,977,4947,6684,980,6684,754,- -32768,981,- -32768,- -32768,752,250,- -32768,- -32768,6684,6684,163,163,988,989,752,4824,253,- -32768,- -32768,130,- -32768,976,978,982,984,986,6858,985,987,- 40,990,- -32768,- -32768,656,- -32768,- -32768,- -32768,- -32768,5574,- -32768,- -32768,- -32768,- -32768,- -32768,664,- -32768,633,- -32768,1002,- -32768,- -32768,- -32768,664,- -32768,- -32768,633,995,691,997,2909,6684,- -32768,- -32768,1130,999,991,4824,- -32768,- -32768,1112,754,1004,7284,1000,2746,1003,- -32768,- 30,- -32768,1035,992,647,552,121,698,217,93,- -32768,- -32768,- -32768,- -32768,1037,7176,1956,- -32768,- -32768,512,6684,1010,1015,- -32768,- -32768,- -32768,1009,1012,- -32768,- -32768,- -32768,- -32768,- -32768,933,- -32768,- -32768,402,- -32768,- -32768,- -32768,4824,1123,- -32768,880,- -32768,- -32768,1014,- 34,266,- -32768,- -32768,- -32768,- -32768,5448,- -32768,- -32768,1020,- -32768,- -32768,- -32768,- -32768,1022,1024,573,174,- -32768,272,- -32768,- -32768,- -32768,522,4947,1025,754,2591,- -32768,4824,1026,1636,7176,6684,7176,7176,7176,7176,7176,7176,7176,7176,7176,7176,6684,- -32768,754,1029,1028,6684,- -32768,1078,- -32768,752,- -32768,- -32768,1030,1032,6684,1044,279,- -32768,- -32768,1052,- -32768,- -32768,1054,- -32768,- -32768,174,- -32768,1041,584,2746,- -32768,- -32768,- -32768,1045,1039,1043,7176,754,- -32768,700,320,651,655,655,552,121,602,217,217,93,- -32768,326,- -32768,- -32768,4947,1040,1049,- -32768,1050,4824,524,- -32768,1058,1052,- -32768,- -32768,1053,- -32768,- -32768,- -32768,174,1061,682,- -32768,944,369,- -32768,- -32768,6684,1636,- -32768,4947,6684,- -32768,1059,- -32768,1077,- -32768,- -32768,- -32768,- -32768,- -32768,296,- -32768,754,- -32768,- -32768,1060,1062,- -32768,- -32768,- -32768,- -32768,- -32768,1203,1206,- -32768};
# 1998
static short Cyc_yypgoto[172U]={- -32768,- 11,- -32768,- -32768,- -32768,- -32768,- -32768,135,- -32768,- -32768,175,- -32768,- -32768,- 288,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,52,- -32768,- 114,6,- -32768,- -32768,0,540,- -32768,134,- 188,1089,33,- -32768,- -32768,- 146,- -32768,- -32768,1178,2,399,- -32768,- -32768,939,936,635,301,- -32768,521,- -32768,- -32768,- -32768,5,- -32768,- -32768,147,- 234,1145,- 415,16,- -32768,1068,- -32768,- -32768,- -32768,1172,- 433,- -32768,482,- 111,- 155,- 338,- 516,- 23,- -32768,- -32768,- 108,483,500,- 304,- 513,- 125,- 447,- 134,- -32768,- 285,- 343,- 588,- 418,- -32768,801,7,- 52,- 158,- 37,- 320,- 704,- 297,- -32768,- -32768,- 68,- 277,- -32768,- 220,- -32768,- 656,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,227,1017,- -32768,603,781,- -32768,314,735,- -32768,- 141,1067,- 426,- 339,- 379,- 378,- 374,841,- 373,- 388,- 365,- 368,- 393,339,345,344,340,- 195,531,432,- -32768,- -32768,- -32768,- -32768,307,- -32768,- 909,247,- -32768,897,192,914,- 367,- -32768,140,517,- 71,- 70,- 92,- 205};
# 2023
static short Cyc_yytable[7447U]={66,148,149,567,280,517,513,535,156,383,268,623,617,388,542,619,627,352,350,642,620,66,595,622,342,66,596,86,292,702,273,90,427,69,562,356,273,625,626,624,547,548,553,550,362,497,734,339,134,780,126,745,434,391,69,1025,685,484,69,66,261,262,837,488,633,66,66,66,471,66,490,143,144,145,293,146,66,160,43,996,706,707,155,435,684,963,330,46,881,703,556,963,69,558,695,162,699,392,69,69,69,237,69,338,480,266,267,610,964,69,731,684,- 239,44,1023,- 239,66,66,1073,685,282,283,284,997,285,286,287,288,153,348,555,66,66,66,67,66,66,66,66,746,554,89,66,84,481,791,480,585,155,69,69,294,66,300,66,67,44,666,319,67,155,160,1095,559,69,69,69,108,69,69,69,69,428,429,430,69,491,725,671,666,971,821,938,560,873,69,481,69,974,438,234,19,20,67,153,480,389,127,434,67,67,67,390,67,318,439,329,586,502,235,67,142,385,- 166,587,- 568,112,431,628,695,756,478,432,433,516,400,401,502,696,435,558,794,108,410,43,481,248,249,250,251,252,114,500,46,108,389,482,462,727,234,67,67,411,412,107,530,470,778,531,253,115,468,678,402,403,67,67,67,235,67,67,67,67,276,477,952,67,824,245,953,111,1012,471,471,471,246,67,678,67,472,43,113,66,44,574,499,49,43,155,46,559,116,331,118,587,45,46,47,53,54,66,50,43,748,526,117,90,66,66,233,66,46,482,319,319,69,319,560,941,49,389,241,289,50,119,290,631,543,949,776,634,53,54,69,329,544,831,43,122,584,69,69,641,69,695,270,46,1063,121,789,123,87,882,271,590,885,591,592,482,318,318,407,318,124,108,408,790,108,894,43,108,587,606,234,66,482,108,45,46,47,319,552,128,844,389,942,- 271,905,951,- 271,521,903,235,212,482,1027,854,482,950,560,820,389,655,656,964,239,773,69,1071,727,1037,280,943,944,48,679,67,1023,502,135,136,562,157,269,158,728,318,331,637,644,159,1108,131,806,67,953,129,444,445,842,502,67,67,972,67,502,742,389,132,389,903,437,975,455,806,389,549,457,1086,175,470,470,470,457,1087,133,299,338,779,670,566,729,389,575,576,577,578,579,489,503,150,477,270,858,175,504,677,688,683,689,271,1065,730,472,472,472,580,389,66,796,66,- 15,499,612,767,429,430,82,67,386,697,698,808,959,475,270,1018,960,652,705,476,389,909,271,902,181,66,526,935,88,581,69,155,69,109,66,110,810,389,66,389,155,903,177,829,155,830,797,461,431,137,138,587,43,768,433,468,82,236,69,809,45,46,47,876,238,82,389,69,240,560,82,69,835,496,242,108,243,718,147,476,883,151,902,108,82,244,726,263,389,265,153,108,891,180,866,679,700,722,109,701,389,1017,1057,1053,353,751,1047,984,939,1049,109,274,541,277,1050,1051,502,1052,827,502,332,828,82,82,875,1055,1056,1054,355,389,345,67,82,67,781,887,782,82,82,82,389,82,82,82,82,1010,1011,397,398,843,503,860,389,1105,468,670,1038,354,1093,67,386,82,643,386,867,868,346,48,67,845,649,587,67,683,902,667,347,668,359,989,1039,1034,526,669,560,560,856,180,248,249,250,251,252,405,406,618,587,587,332,364,48,1033,358,526,361,363,526,1045,680,1077,681,253,1078,569,570,66,682,66,366,378,379,319,367,155,363,49,1009,368,363,475,369,742,742,706,707,476,635,53,54,370,109,736,737,109,818,819,109,363,69,371,69,373,109,279,1088,16,17,18,374,1041,659,660,661,375,1046,832,833,318,380,329,987,877,878,751,298,376,108,43,377,1060,108,1104,279,969,970,501,46,387,48,1004,1005,896,1000,1001,49,394,82,393,50,1102,395,1058,66,43,501,527,53,54,532,1099,389,45,46,47,1085,148,945,946,977,396,978,907,908,436,440,82,454,460,464,465,361,485,711,712,486,69,715,494,1084,720,487,495,538,505,43,507,529,509,275,67,1019,67,45,46,47,518,1103,519,533,180,528,49,537,43,536,521,539,787,291,522,545,1109,46,53,54,363,139,982,546,469,49,474,551,344,50,363,482,556,66,564,563,414,53,54,568,48,386,572,589,148,1036,603,604,609,648,636,763,611,638,639,994,363,640,363,363,363,363,647,646,363,298,69,66,651,363,363,363,650,319,363,654,657,672,363,1101,658,67,673,587,415,416,417,418,419,420,421,422,423,424,674,676,148,109,1020,675,69,686,713,714,717,109,108,721,716,723,363,732,749,109,747,425,735,750,753,318,800,754,755,391,757,860,758,759,43,66,760,761,1042,762,1043,319,45,46,47,775,764,148,1036,765,766,49,769,501,770,496,501,771,774,777,783,476,788,53,54,82,515,82,69,795,792,43,720,67,801,793,804,807,501,45,46,47,48,501,1079,814,813,318,49,817,- 568,823,521,363,43,825,836,838,840,931,53,54,45,46,47,839,389,67,841,338,846,49,852,853,1092,521,850,851,848,849,573,772,864,53,54,879,880,66,871,869,884,888,889,319,890,892,893,886,897,895,898,597,598,599,600,601,602,900,906,605,279,910,608,911,933,934,937,940,947,948,69,954,613,955,616,958,973,956,957,67,961,279,962,976,981,968,986,985,279,318,988,990,363,998,993,1008,1013,999,1021,629,936,995,720,1014,1015,182,183,1016,1022,1030,1031,630,1032,1040,109,1061,613,1064,109,1069,184,1044,91,1062,185,1066,1067,963,1074,1076,186,279,1083,1081,1082,1089,187,188,189,190,191,192,279,1090,501,1091,193,1094,1096,1098,194,279,1107,1113,1106,1110,1114,1111,1075,195,196,665,197,1097,983,501,805,278,501,120,904,67,822,161,198,130,826,855,239,511,199,200,520,857,363,847,621,163,164,165,166,167,351,653,168,169,170,515,645,171,785,720,172,43,94,201,202,95,96,97,372,1002,46,463,932,203,1007,512,1003,1006,204,1072,1028,607,205,1100,91,206,594,613,0,613,0,0,207,0,0,208,209,0,0,210,211,0,0,0,0,0,0,0,0,0,0,279,194,386,0,363,0,0,0,1048,469,0,0,0,279,0,0,0,0,0,1059,0,0,0,0,720,0,0,82,0,0,0,0,0,1068,0,0,0,0,0,0,0,0,0,0,0,0,109,0,1080,0,0,363,0,0,43,94,201,202,95,96,97,0,0,46,0,0,0,0,0,515,0,0,0,0,0,593,363,363,206,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,363,91,0,0,0,0,0,799,0,0,0,0,803,0,22,23,24,147,26,82,812,0,0,0,0,0,0,0,0,194,0,0,31,515,0,0,0,0,0,0,32,33,0,0,912,913,992,0,0,0,0,613,0,0,0,39,0,0,40,298,0,91,0,0,0,0,363,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,279,0,0,861,862,863,0,865,147,0,360,91,0,206,44,0,0,515,48,0,874,0,0,0,0,0,0,0,0,0,51,52,0,0,92,0,0,363,0,363,363,363,363,363,363,363,363,363,363,232,94,279,0,95,96,97,0,98,46,0,147,0,0,99,515,0,100,0,0,0,914,0,102,103,0,0,0,92,0,0,207,0,0,104,209,363,0,210,211,441,0,0,442,94,0,0,95,96,97,0,443,46,0,0,0,147,99,0,0,100,0,0,0,101,0,102,103,0,444,445,0,0,0,515,0,0,104,363,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,515,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,832,833,21,182,183,301,0,302,303,304,305,306,307,308,309,22,23,24,310,26,91,27,185,311,0,0,0,0,186,28,312,0,0,31,187,188,189,190,191,192,0,32,33,34,193,0,0,515,194,0,0,0,0,0,0,0,39,195,196,40,197,0,0,0,0,0,0,0,0,0,0,41,198,0,0,42,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,313,94,201,202,95,96,97,44,45,46,47,48,203,314,152,0,0,204,0,0,0,205,0,51,316,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,182,183,301,0,302,303,304,305,306,307,308,309,22,23,24,310,26,91,27,185,311,0,0,0,0,186,28,312,0,0,31,187,188,189,190,191,192,0,32,33,34,193,0,0,0,194,0,0,0,0,0,0,0,39,195,196,40,197,0,0,0,0,0,0,0,0,0,0,41,198,0,0,42,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,313,94,201,202,95,96,97,44,45,46,47,48,203,314,152,315,0,204,0,0,0,205,0,51,316,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,182,183,301,0,302,303,304,305,306,307,308,309,22,23,24,310,26,91,27,185,311,0,0,0,0,186,28,312,0,0,31,187,188,189,190,191,192,0,32,33,34,193,0,0,0,194,0,0,0,0,0,0,0,39,195,196,40,197,0,0,0,0,0,0,0,0,0,0,41,198,0,0,42,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,313,94,201,202,95,96,97,44,45,46,47,48,203,314,152,0,0,204,0,0,0,205,0,51,316,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,182,183,301,0,302,303,304,305,306,307,308,309,22,23,24,310,26,91,27,185,311,0,0,0,0,186,28,312,0,0,31,187,188,189,190,191,192,0,32,33,34,193,0,0,0,194,0,0,0,0,0,0,0,39,195,196,40,197,0,0,0,0,0,0,0,0,0,0,41,198,0,0,42,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,534,94,201,202,95,96,97,44,45,46,47,48,203,314,152,0,0,204,0,0,0,205,0,51,316,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,182,183,0,0,0,0,0,0,0,0,0,0,22,23,24,310,26,91,27,185,0,0,0,0,0,186,28,0,0,0,31,187,188,189,190,191,192,0,32,33,34,193,0,0,0,194,0,0,0,0,0,0,0,39,195,196,40,197,0,0,0,0,0,0,0,0,0,0,41,198,0,0,42,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,44,45,46,47,48,203,0,0,0,0,204,0,0,0,205,0,51,316,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,182,183,0,0,0,0,0,0,0,0,0,0,22,23,24,310,26,91,0,0,0,0,0,0,0,0,28,0,0,0,31,187,188,189,190,191,192,0,32,33,0,0,0,0,0,194,0,0,0,0,0,0,0,39,195,196,40,197,0,0,0,0,0,0,0,0,0,0,41,198,0,0,42,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,44,45,46,47,48,203,0,0,0,0,204,0,0,0,360,0,51,316,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,182,183,0,0,0,0,0,0,0,0,0,0,22,23,24,184,26,91,0,185,0,0,0,0,0,186,0,0,0,0,31,187,188,189,190,191,192,0,32,33,0,193,0,0,0,194,0,0,0,0,0,0,0,39,195,196,40,197,0,0,0,0,0,0,0,0,0,0,41,198,0,0,42,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,44,45,46,47,48,203,0,384,0,0,204,0,0,0,205,0,51,316,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,182,183,0,0,0,0,0,0,0,0,0,0,22,23,24,184,26,91,0,185,0,0,0,0,0,186,0,0,0,0,31,187,188,189,190,191,192,0,32,33,0,193,0,0,0,194,0,0,0,0,0,0,0,39,195,196,40,197,0,0,0,0,0,0,0,0,0,0,41,198,0,0,42,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,44,45,46,47,48,203,0,0,0,0,204,0,0,0,205,0,51,316,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,91,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,92,0,0,0,0,41,0,0,0,42,0,0,456,0,0,232,94,0,0,95,96,97,0,98,46,0,0,0,0,99,0,0,100,0,0,43,101,0,102,103,0,0,44,45,46,47,48,0,0,104,- 12,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,91,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,92,0,0,0,0,41,0,0,0,42,0,0,786,0,0,232,94,0,0,95,96,97,0,98,46,0,0,0,0,99,0,0,100,0,0,43,101,0,102,103,0,0,44,45,46,47,48,0,0,104,0,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,91,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,28,0,29,30,31,0,0,0,0,0,0,0,32,33,34,0,0,35,36,0,37,38,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,92,0,0,0,0,41,0,0,0,42,0,0,0,0,0,93,94,0,0,95,96,97,91,98,46,0,0,0,0,99,0,0,100,0,0,43,101,0,102,103,0,0,44,45,46,47,48,0,0,104,- 12,0,49,0,0,0,50,0,51,52,0,0,0,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,91,0,0,0,0,0,0,92,0,0,0,0,0,0,22,23,24,0,26,784,0,0,442,94,0,0,95,96,97,0,443,46,31,0,0,0,99,0,0,100,32,33,0,101,0,102,103,0,444,445,0,0,0,0,0,39,104,0,40,0,0,0,0,0,0,0,92,0,0,0,41,0,0,0,42,0,0,0,0,0,0,232,94,0,0,95,96,97,0,98,46,0,0,0,0,99,0,336,100,0,43,0,101,0,102,103,0,44,45,46,47,48,0,338,0,104,0,49,0,0,0,496,0,51,52,0,0,476,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,336,0,0,0,0,0,0,0,0,0,44,45,0,47,48,0,338,0,0,0,49,0,0,0,475,0,51,52,0,0,476,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,0,0,0,0,0,0,44,45,46,47,48,0,0,0,0,0,49,0,0,0,521,0,51,52,0,0,0,0,53,54,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,32,33,34,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,0,47,48,0,0,152,0,0,0,291,0,0,0,0,51,52,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,32,33,34,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,2,3,4,85,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,44,45,0,47,48,0,0,152,22,23,24,25,26,0,0,0,51,52,0,0,0,0,28,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,44,45,0,47,48,22,23,24,0,26,0,0,0,0,0,0,51,52,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,247,0,0,0,0,0,248,249,250,251,252,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,253,42,0,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,44,45,0,47,48,0,0,254,22,23,24,0,26,0,0,0,51,52,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,336,0,0,337,0,0,0,0,0,0,44,45,0,47,48,0,338,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,336,0,0,0,0,0,0,0,0,0,44,45,0,47,48,0,338,0,0,0,0,0,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,44,45,0,47,48,22,23,24,466,26,0,0,0,0,0,0,51,52,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,44,45,0,47,48,22,23,24,733,26,0,0,0,0,0,0,51,52,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,744,0,0,0,0,0,0,0,0,0,44,45,0,47,48,22,23,24,0,26,0,0,0,0,0,0,51,52,0,0,0,0,0,31,0,0,0,0,0,0,0,32,33,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,40,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,42,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,45,0,47,48,0,0,0,0,0,0,0,0,0,0,0,51,52,182,183,301,0,302,303,304,305,306,307,308,309,0,0,0,184,0,91,0,185,311,0,0,0,0,186,0,312,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,534,94,201,202,95,96,97,0,0,46,0,0,203,314,152,0,0,204,0,0,0,205,0,0,206,182,183,0,0,0,0,207,690,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,691,94,201,202,95,96,97,0,271,46,0,0,203,0,512,692,0,204,0,0,0,205,0,0,206,0,444,445,182,183,0,207,0,0,208,209,0,0,210,211,0,0,0,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,691,94,201,202,95,96,97,0,271,46,0,0,203,0,512,872,0,204,0,0,0,205,0,0,206,0,444,445,182,183,0,207,0,0,208,209,0,0,210,211,0,0,0,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,691,94,201,202,95,96,97,0,271,46,0,0,203,0,512,901,0,204,0,0,0,205,0,0,206,0,444,445,182,183,0,207,0,0,208,209,0,0,210,211,0,0,0,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,691,94,201,202,95,96,97,0,271,46,0,0,203,0,512,1029,0,204,0,0,0,205,0,0,206,0,444,445,182,183,0,207,0,0,208,209,0,0,210,211,0,0,0,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,691,94,201,202,95,96,97,0,271,46,0,0,203,0,512,0,0,204,0,0,0,205,0,0,206,0,444,445,182,183,0,207,0,0,208,209,0,0,210,211,0,0,0,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,0,0,0,204,0,0,0,205,0,0,206,182,183,0,343,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,540,0,0,0,204,0,0,0,205,0,0,206,182,183,0,0,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,0,0,0,204,0,0,0,205,632,0,206,182,183,0,0,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,0,0,0,204,0,0,0,205,0,0,206,182,183,0,664,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,0,0,0,204,0,0,0,205,724,0,206,182,183,0,0,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,384,0,0,204,0,0,0,205,0,0,206,182,183,0,0,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,0,0,0,204,0,0,0,205,0,0,206,182,183,0,798,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,0,0,0,204,0,0,0,205,0,0,206,182,183,0,811,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,0,0,0,204,0,0,0,205,0,0,206,182,183,0,0,0,0,207,0,0,208,209,0,0,210,211,184,0,91,0,185,0,0,0,0,0,186,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,193,0,0,0,194,0,0,0,0,0,182,183,0,195,196,0,197,0,0,0,0,0,0,0,0,184,0,91,198,0,0,0,0,0,199,200,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,0,0,0,0,194,0,802,94,201,202,95,96,97,195,196,46,197,0,203,0,0,0,0,204,0,0,0,205,198,0,206,0,0,0,199,200,0,207,0,0,208,209,0,0,210,211,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,182,183,203,0,512,0,0,204,0,0,0,205,0,0,206,184,0,91,0,0,0,207,0,0,208,209,0,0,210,211,0,187,188,189,190,191,192,0,0,0,0,0,0,0,0,194,0,0,0,0,0,182,183,0,195,196,0,197,0,0,0,0,0,0,0,0,184,0,91,198,0,0,0,0,0,199,200,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,0,0,0,0,194,0,43,94,201,202,95,96,97,195,196,46,197,0,203,0,772,0,0,204,0,0,0,205,198,0,206,0,0,0,199,200,0,207,0,0,208,209,0,0,210,211,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,182,183,203,0,0,0,0,204,0,0,0,357,0,0,206,184,0,91,0,0,0,207,0,0,208,209,0,0,210,211,0,187,188,189,190,191,192,0,0,0,0,0,0,0,0,194,0,0,0,0,0,182,183,0,195,196,0,197,0,0,0,0,0,0,0,0,184,0,91,198,0,0,0,0,0,199,200,0,0,0,0,0,187,188,189,190,191,192,0,0,0,0,0,0,0,0,194,0,43,94,201,202,95,96,97,195,196,46,197,0,203,0,0,0,0,204,0,0,0,360,198,0,206,0,0,0,199,200,0,207,0,0,208,209,0,0,210,211,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,182,183,203,0,0,0,0,204,0,0,0,205,0,0,206,184,0,91,0,0,0,207,0,0,208,209,0,0,210,211,0,187,188,189,190,191,192,0,0,0,0,0,0,0,0,194,0,0,0,0,0,0,0,0,195,196,0,197,0,0,0,0,0,0,0,0,0,0,0,198,0,0,0,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,43,94,201,202,95,96,97,0,0,46,0,0,203,0,0,0,0,204,0,0,0,991,0,0,206,0,0,0,0,0,0,207,0,0,208,209,0,0,210,211};
# 2771
static short Cyc_yycheck[7447U]={0,72,72,341,129,293,291,304,76,204,118,399,391,208,311,393,409,175,173,445,394,21,365,396,158,25,365,21,142,88,122,25,227,0,338,181,128,405,406,404,317,318,330,320,185,279,559,158,59,637,50,567,121,109,21,964,503,262,25,59,112,113,718,268,431,65,66,67,256,69,17,65,66,67,142,69,76,77,123,109,5,6,76,152,499,125,154,132,792,148,123,125,59,67,512,79,522,153,65,66,67,99,69,136,103,116,117,384,148,76,557,526,145,130,148,148,116,117,1027,566,131,132,133,153,135,136,137,138,76,146,335,131,132,133,0,135,136,137,138,572,139,144,142,125,143,658,103,352,142,116,117,149,152,151,154,21,130,477,152,25,154,161,1071,137,131,132,133,27,135,136,137,138,99,100,101,142,123,544,482,499,884,697,838,338,772,152,143,154,892,130,123,20,21,59,142,103,139,50,121,65,66,67,145,69,152,146,154,353,279,142,76,64,205,138,355,148,142,144,413,637,143,258,149,150,292,104,105,298,516,152,67,664,92,140,123,143,73,74,75,76,77,123,279,132,104,139,154,245,545,123,116,117,159,160,27,144,256,151,147,96,142,254,496,142,143,131,132,133,142,135,136,137,138,126,258,145,142,703,137,149,123,937,470,471,472,144,152,521,154,256,123,142,292,130,346,279,140,123,292,132,137,136,155,142,445,131,132,133,152,153,310,144,123,145,298,136,310,317,318,92,320,132,154,317,318,292,320,482,844,140,139,104,136,144,144,139,428,136,854,148,432,152,153,310,292,144,713,123,122,349,317,318,444,320,772,123,132,1013,55,145,144,137,795,131,357,798,359,360,154,317,318,154,320,137,234,158,145,237,811,123,240,522,375,123,384,154,246,131,132,133,384,136,122,731,139,145,136,823,145,139,144,819,142,89,154,139,744,154,855,564,695,139,464,465,148,101,611,384,139,716,148,546,848,849,134,496,292,148,499,136,137,735,142,119,144,549,384,290,436,141,150,145,136,677,310,149,144,149,150,726,521,317,318,886,320,526,564,139,136,139,878,234,895,145,698,139,319,240,148,142,470,471,472,246,148,137,149,136,636,479,139,549,139,73,74,75,76,77,145,139,137,475,123,124,142,145,496,508,498,510,131,1017,554,470,471,472,96,139,508,667,510,137,496,145,99,100,101,0,384,205,518,521,680,866,144,123,124,866,136,527,150,139,829,131,819,137,536,521,835,22,130,508,536,510,27,545,29,681,139,549,139,545,970,125,145,549,145,668,244,144,136,137,703,123,149,150,559,50,142,536,681,131,132,133,779,123,59,139,545,144,735,64,549,145,144,136,446,139,536,72,150,796,75,878,454,78,141,545,141,139,130,549,462,808,87,145,677,136,136,92,139,139,950,1006,1002,139,583,996,903,839,998,104,144,309,145,999,1000,698,1001,136,701,136,139,116,117,776,1004,1005,1003,141,139,144,508,126,510,638,145,640,131,132,133,139,135,136,137,138,933,145,106,107,728,139,754,139,1090,658,666,145,138,145,536,357,155,446,360,767,768,144,134,545,732,454,823,549,684,970,142,144,144,144,910,988,980,677,150,850,851,749,181,73,74,75,76,77,102,103,392,848,849,136,142,134,139,182,698,184,185,701,995,142,136,144,96,139,91,92,726,150,728,144,199,200,726,144,728,204,140,932,144,208,144,144,850,851,5,6,150,433,152,153,144,234,88,89,237,138,139,240,227,726,144,728,144,246,129,1062,17,18,19,144,990,470,471,472,144,995,22,23,726,18,728,906,138,139,751,150,144,647,123,144,1010,651,1089,158,138,139,279,132,144,134,102,103,813,156,157,140,155,290,108,144,1087,156,1007,813,123,298,299,152,153,302,138,139,131,132,133,1045,897,850,851,138,157,140,827,828,137,130,319,123,123,144,144,310,123,529,530,136,813,533,138,1044,536,143,145,136,138,123,138,144,138,124,726,953,728,131,132,133,137,1087,138,144,353,137,140,123,123,144,144,136,651,141,148,148,1102,132,152,153,355,136,899,144,255,140,257,138,159,144,365,154,123,899,148,145,62,152,153,145,134,593,151,138,981,981,144,123,144,148,142,603,145,143,143,914,391,143,393,394,395,396,139,138,399,296,899,933,139,404,405,406,145,933,409,145,138,144,413,1086,139,813,123,1090,110,111,112,113,114,115,116,117,118,119,144,139,1033,446,957,145,933,48,144,137,30,454,832,136,148,46,445,144,144,462,145,141,154,145,145,933,672,139,145,109,145,1083,139,139,123,995,139,139,991,139,993,995,131,132,133,139,145,1078,1078,145,145,140,145,496,125,144,499,138,145,145,151,150,138,152,153,508,291,510,995,151,145,123,718,899,136,145,145,145,521,131,132,133,134,526,1037,47,137,995,140,123,148,144,144,522,123,138,144,136,136,832,152,153,131,132,133,137,139,933,145,136,138,140,105,143,1067,144,139,139,144,144,345,137,25,152,153,123,123,1087,145,148,145,139,145,1087,145,145,145,151,137,151,49,366,367,368,369,370,371,142,144,374,475,137,377,145,148,138,136,136,130,130,1087,145,387,145,389,139,124,145,144,995,145,496,145,138,137,145,145,138,503,1087,28,137,611,108,144,108,136,155,25,414,836,148,838,138,145,25,26,145,144,139,138,426,138,138,647,136,431,89,651,125,40,145,42,145,44,145,144,125,124,138,50,546,139,138,145,145,56,57,58,59,60,61,557,144,677,145,66,139,145,138,70,566,125,0,145,145,0,145,1033,79,80,476,82,1078,900,698,676,128,701,41,821,1087,701,78,94,53,709,744,914,290,100,101,296,751,703,735,395,83,84,85,86,87,174,462,90,91,92,512,447,95,647,937,98,123,124,125,126,127,128,129,193,922,132,246,832,135,926,137,923,925,140,1024,965,376,144,1083,42,147,364,544,- 1,546,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,668,70,991,- 1,776,- 1,- 1,- 1,997,677,- 1,- 1,- 1,681,- 1,- 1,- 1,- 1,- 1,1008,- 1,- 1,- 1,- 1,1013,- 1,- 1,813,- 1,- 1,- 1,- 1,- 1,1022,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,832,- 1,1037,- 1,- 1,823,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,637,- 1,- 1,- 1,- 1,- 1,144,848,849,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,866,42,- 1,- 1,- 1,- 1,- 1,669,- 1,- 1,- 1,- 1,674,- 1,37,38,39,897,41,899,682,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,55,695,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,25,26,912,- 1,- 1,- 1,- 1,713,- 1,- 1,- 1,78,- 1,- 1,81,821,- 1,42,- 1,- 1,- 1,- 1,932,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,855,- 1,- 1,758,759,760,- 1,762,981,- 1,144,42,- 1,147,130,- 1,- 1,772,134,- 1,775,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,147,- 1,- 1,108,- 1,- 1,996,- 1,998,999,1000,1001,1002,1003,1004,1005,1006,1007,123,124,906,- 1,127,128,129,- 1,131,132,- 1,1033,- 1,- 1,137,819,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,108,- 1,- 1,154,- 1,- 1,157,158,1044,- 1,161,162,120,- 1,- 1,123,124,- 1,- 1,127,128,129,- 1,131,132,- 1,- 1,- 1,1078,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,149,150,- 1,- 1,- 1,878,- 1,- 1,157,1086,- 1,- 1,- 1,1090,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,903,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,65,66,- 1,- 1,970,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,65,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,65,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,65,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,65,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,130,131,132,133,134,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,130,131,132,133,134,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,130,131,132,133,134,135,- 1,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,55,56,57,58,59,60,61,- 1,63,64,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,94,- 1,- 1,97,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,130,131,132,133,134,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,42,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,65,- 1,- 1,68,69,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,123,144,- 1,146,147,- 1,- 1,130,131,132,133,134,- 1,- 1,157,138,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,152,153,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,42,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,65,- 1,- 1,68,69,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,127,128,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,123,144,- 1,146,147,- 1,- 1,130,131,132,133,134,- 1,- 1,157,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,152,153,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,42,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,53,54,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,65,- 1,- 1,68,69,- 1,71,72,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,123,124,- 1,- 1,127,128,129,42,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,123,144,- 1,146,147,- 1,- 1,130,131,132,133,134,- 1,- 1,157,138,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,152,153,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,120,- 1,- 1,123,124,- 1,- 1,127,128,129,- 1,131,132,55,- 1,- 1,- 1,137,- 1,- 1,140,63,64,- 1,144,- 1,146,147,- 1,149,150,- 1,- 1,- 1,- 1,- 1,78,157,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,123,124,- 1,- 1,127,128,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,120,140,- 1,123,- 1,144,- 1,146,147,- 1,130,131,132,133,134,- 1,136,- 1,157,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,150,- 1,152,153,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,150,- 1,152,153,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,130,131,132,133,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,146,147,- 1,- 1,- 1,- 1,152,153,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,- 1,- 1,137,- 1,- 1,- 1,141,- 1,- 1,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,130,131,- 1,133,134,- 1,- 1,137,37,38,39,40,41,- 1,- 1,- 1,146,147,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,67,- 1,- 1,- 1,- 1,- 1,73,74,75,76,77,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,96,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,- 1,- 1,137,37,38,39,- 1,41,- 1,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,37,38,39,138,41,- 1,- 1,- 1,- 1,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,37,38,39,138,41,- 1,- 1,- 1,- 1,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,55,- 1,- 1,- 1,- 1,- 1,- 1,- 1,63,64,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,78,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,133,134,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,147,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,- 1,44,45,- 1,- 1,- 1,- 1,50,- 1,52,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,136,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,25,26,- 1,- 1,- 1,- 1,154,32,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,131,132,- 1,- 1,135,- 1,137,138,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,- 1,149,150,25,26,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,131,132,- 1,- 1,135,- 1,137,138,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,- 1,149,150,25,26,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,131,132,- 1,- 1,135,- 1,137,138,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,- 1,149,150,25,26,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,131,132,- 1,- 1,135,- 1,137,138,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,- 1,149,150,25,26,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,131,132,- 1,- 1,135,- 1,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,- 1,149,150,25,26,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,25,26,- 1,151,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,136,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,25,26,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,145,- 1,147,25,26,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,25,26,- 1,151,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,145,- 1,147,25,26,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,25,26,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,25,26,- 1,151,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,25,26,- 1,151,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,25,26,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,66,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,25,26,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,123,124,125,126,127,128,129,79,80,132,82,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,94,- 1,147,- 1,- 1,- 1,100,101,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,25,26,135,- 1,137,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,40,- 1,42,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,25,26,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,123,124,125,126,127,128,129,79,80,132,82,- 1,135,- 1,137,- 1,- 1,140,- 1,- 1,- 1,144,94,- 1,147,- 1,- 1,- 1,100,101,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,25,26,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,40,- 1,42,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,25,26,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,123,124,125,126,127,128,129,79,80,132,82,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,94,- 1,147,- 1,- 1,- 1,100,101,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,25,26,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,40,- 1,42,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162,- 1,56,57,58,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,100,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,124,125,126,127,128,129,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,144,- 1,- 1,147,- 1,- 1,- 1,- 1,- 1,- 1,154,- 1,- 1,157,158,- 1,- 1,161,162};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int,int);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*,struct Cyc_Yyltype*);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple33{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple34{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _Tmp1;_Tmp1.YYINITIALSVAL.tag=75U,_Tmp1.YYINITIALSVAL.val=0;_Tmp1;});
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
Cyc_yyerror(_tag_fat("parser stack overflow",sizeof(char),22U),yystate,yychar);
_throw(& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _Tmp1=(unsigned)yystacksize;_tag_fat(({short*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(short)));});({{unsigned _Tmp3=(unsigned)yystacksize;unsigned i;for(i=0;i < _Tmp3;++ i){i <= (unsigned)yyssp_offset?_Tmp2[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_Tmp2[i]=0);}}0;});_Tmp2;}),sizeof(short),_Tmp1);});
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1115U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 390?(int)Cyc_yytranslate[yychar]: 335;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 7446)||(int)Cyc_yycheck[yyn]!=yychar1)goto yydefault;
# 302
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,7447U,sizeof(short),yyn));
# 309
if(yyn < 0){
# 311
if(yyn==-32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 315
if(yyn==0)goto yyerrlab;}
# 317
if(yyn==1114){
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefact,1115U,sizeof(short),yystate));
if(yyn==0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,572U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=yyyvsp[0].v;
# 370 "cycbison.simple"
switch((int)yyn){case 1:
# 1167 "parse.y"
 yyval=yyyvsp[0].v;
Cyc_Parse_parse_result=Cyc_yyget_YY17(& yyyvsp[0].v);
# 1170
goto _LL0;case 2:
# 1173 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY17(& yyyvsp[1].v));}));
goto _LL0;case 3:
# 1177 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_using();
# 1180
goto _LL0;case 4:
# 1181 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_Tmp5->tag=10,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY17(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 5:
# 1184
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
Cyc_Lex_leave_namespace();
# 1187
goto _LL0;case 6:
# 1188 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp4=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_Tmp5->tag=9,({struct _fat_ptr*_Tmp6=({struct _fat_ptr*_Tmp7=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp8=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->r=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY17(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 7:  {
# 1190 "parse.y"
int is_c_include=Cyc_yyget_YY33(& yyyvsp[0].v);
struct Cyc_List_List*cycdecls=Cyc_yyget_YY17(& yyyvsp[4].v);
struct _tuple30*_Tmp1=Cyc_yyget_YY56(& yyyvsp[5].v);unsigned _Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*exs=_Tmp3;unsigned wc=_Tmp2;
struct Cyc_List_List*hides=Cyc_yyget_YY57(& yyyvsp[6].v);
if(exs!=0 && hides!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U),_tag_fat(0U,sizeof(void*),0));
if((unsigned)hides && !((int)wc))
wc=yyyvsp[6].l.first_line;
if(!is_c_include){
if(exs!=0 || cycdecls!=0){
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("expecting \"C include\"",sizeof(char),22U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f1=wc,_TmpA->f2=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY17(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}else{
# 1204
yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_Tmp8->tag=11,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY17(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}}else{
# 1207
yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Decl));({void*_Tmp7=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_Tmp8->tag=12,({struct Cyc_List_List*_Tmp9=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp8->f1=_Tmp9;}),_Tmp8->f2=cycdecls,_Tmp8->f3=exs,({struct _tuple10*_Tmp9=({struct _tuple10*_TmpA=_cycalloc(sizeof(struct _tuple10));_TmpA->f1=wc,_TmpA->f2=hides;_TmpA;});_Tmp8->f4=_Tmp9;});_Tmp8;});_Tmp6->r=_Tmp7;}),_Tmp6->loc=yyyvsp[0].l.first_line;_Tmp6;});_Tmp4->hd=_Tmp5;}),({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY17(& yyyvsp[7].v);_Tmp4->tl=_Tmp5;});_Tmp4;}));}
# 1209
goto _LL0;}}case 8:
# 1210 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Porton_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 9:
# 1212 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Portoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 10:
# 1214 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempeston_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 11:
# 1216 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Decl*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_Tmp3->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY17(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 12:
# 1217 "parse.y"
 yyval=Cyc_YY17(0);
goto _LL0;case 13:
# 1221 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14:
# 1224
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15:
# 1229 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})==0)
yyval=Cyc_YY33(0);else{
if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C include",sizeof(char),10U));})==0)
yyval=Cyc_YY33(1);else{
# 1235
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY33(1);}}
# 1239
goto _LL0;case 16:
# 1242 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17:
# 1246 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 18:
# 1247 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 19:
# 1251 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 20:
# 1252 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 21:
# 1253 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY57(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 22:
# 1257 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f1=0,_Tmp1->f2=0U;_Tmp1;}));
goto _LL0;case 23:
# 1258 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 24:
# 1262 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY55(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0U;_Tmp1;}));
goto _LL0;case 25:
# 1263 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f1=0,_Tmp1->f2=0U;_Tmp1;}));
goto _LL0;case 26:
# 1264 "parse.y"
 yyval=Cyc_YY56(({struct _tuple30*_Tmp1=_cycalloc(sizeof(struct _tuple30));_Tmp1->f1=0,_Tmp1->f2=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 27:
# 1269 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));_Tmp3->f1=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f2=_Tmp4;}),_Tmp3->f3=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 28:
# 1271 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple33*_Tmp2=({struct _tuple33*_Tmp3=_cycalloc(sizeof(struct _tuple33));_Tmp3->f1=yyyvsp[0].l.first_line,({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp3->f2=_Tmp4;}),_Tmp3->f3=0;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY55(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 29:
# 1275 "parse.y"
 yyval=Cyc_YY17(0);
goto _LL0;case 30:
# 1276 "parse.y"
 yyval=yyyvsp[2].v;
goto _LL0;case 31:
# 1280 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({void*_Tmp3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp4->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp5=Cyc_yyget_YY16(& yyyvsp[0].v);_Tmp4->f1=_Tmp5;});_Tmp4;});Cyc_Absyn_new_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 32:
# 1281 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 33:
# 1282 "parse.y"
 yyval=Cyc_YY17(0);
goto _LL0;case 36:
# 1292 "parse.y"
 yyval=Cyc_YY16(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 37:  {
# 1294 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY18(& yyyvsp[0].v);
yyval=Cyc_YY16(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 38:
# 1306 "parse.y"
 yyval=Cyc_YY16(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY17(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,0,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 39:  {
# 1308 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY18(& yyyvsp[0].v);
yyval=Cyc_YY16(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY17(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 40:  {
# 1316 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY18(& yyyvsp[0].v);
yyval=Cyc_YY16(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[1].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,0,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;}case 41:  {
# 1319 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY18(& yyyvsp[0].v);
yyval=Cyc_YY16(({struct _RegionHandle*_Tmp1=yyr;struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[1].v);struct Cyc_List_List*_Tmp3=Cyc_yyget_YY17(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);Cyc_Parse_make_function(_Tmp1,& d,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;}case 42:
# 1324 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(& yyyvsp[1].v));yyval=yyyvsp[1].v;
goto _LL0;case 43:
# 1327
 Cyc_Lex_leave_using();
goto _LL0;case 44:
# 1330
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp1=_Tmp2;});_Tmp1;}));yyval=yyyvsp[1].v;
goto _LL0;case 45:
# 1333
 Cyc_Lex_leave_namespace();
goto _LL0;case 46:
# 1339 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY18(& yyyvsp[0].v);unsigned _Tmp2=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,0,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 47:
# 1341 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _Tmp1=Cyc_yyget_YY18(& yyyvsp[0].v);struct _tuple11*_Tmp2=({(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;})(Cyc_yyget_YY20(& yyyvsp[1].v));unsigned _Tmp3=yyyvsp[0].l.first_line;Cyc_Parse_make_declarations(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 48:
# 1344
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY10(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_let_decl(_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 49:  {
# 1346 "parse.y"
struct Cyc_List_List*vds=0;
{struct Cyc_List_List*ids=Cyc_yyget_YY38(& yyyvsp[1].v);for(0;ids!=0;ids=ids->tl){
struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0)),({union Cyc_Absyn_Nmspace _Tmp1=Cyc_Absyn_Rel_n(0);qv->f1=_Tmp1;}),qv->f2=(struct _fat_ptr*)ids->hd;
vds=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Vardecl*_Tmp2=({struct _tuple0*_Tmp3=qv;Cyc_Absyn_new_vardecl(0U,_Tmp3,Cyc_Absyn_wildtyp(0),0);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=vds;_Tmp1;});}}
# 1351
yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(vds);Cyc_Absyn_letv_decl(_Tmp3,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1353
goto _LL0;}case 50:
# 1356 "parse.y"
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[2].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp1;});{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[4].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=Cyc_Absyn_region_decl(tv,vd,0,yyyvsp[0].l.first_line);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1362
goto _LL0;}}case 51:  {
# 1364
struct _fat_ptr tvstring=({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)((struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_Tmp2,sizeof(void*),1));});
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[1].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=tvstring;_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp1;});{
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[1].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Decl*_Tmp2=({struct Cyc_Absyn_Tvar*_Tmp3=tv;struct Cyc_Absyn_Vardecl*_Tmp4=vd;struct Cyc_Absyn_Exp*_Tmp5=Cyc_yyget_YY4(& yyyvsp[2].v);Cyc_Absyn_region_decl(_Tmp3,_Tmp4,_Tmp5,yyyvsp[0].l.first_line);});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 1371
goto _LL0;}}}case 52:
# 1374 "parse.y"
 yyval=Cyc_YY4(0);
goto _LL0;case 53:
# 1376 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("open",sizeof(char),5U));})!=0)Cyc_Warn_err(yyyvsp[3].l.first_line,_tag_fat("expecting `open'",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY4(Cyc_yyget_Exp_tok(& yyyvsp[3].v));
goto _LL0;case 54:
# 1380
 yyval=yyyvsp[0].v;
goto _LL0;case 55:
# 1381 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY17(& yyyvsp[1].v));}));
goto _LL0;case 56:
# 1387 "parse.y"
 yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 57:  {
# 1390 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY18(& yyyvsp[1].v);
if((int)two.sc!=7)
Cyc_Warn_warn(yyyvsp[0].l.first_line,
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;({enum Cyc_Parse_Storage_class _Tmp2=Cyc_yyget_YY21(& yyyvsp[0].v);_Tmp1.sc=_Tmp2;}),_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1397
goto _LL0;}case 58:
# 1398 "parse.y"
 Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));
yyval=yyyvsp[1].v;
# 1401
goto _LL0;case 59:
# 1402 "parse.y"
 yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 60:  {
# 1405 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY18(& yyyvsp[1].v);
yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,({
struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=two.type_specs;Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,Cyc_yyget_YY22(& yyyvsp[0].v));});_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1410
goto _LL0;}case 61:
# 1411 "parse.y"
 yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY25(& yyyvsp[0].v);_Tmp1.tq=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 62:  {
# 1413 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY18(& yyyvsp[1].v);
yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY25(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.tq);});_Tmp1.tq=_Tmp2;}),_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1417
goto _LL0;}case 63:
# 1418 "parse.y"
 yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=1,_Tmp1.attributes=0;_Tmp1;}));
goto _LL0;case 64:  {
# 1421 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY18(& yyyvsp[1].v);
yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=1,_Tmp1.attributes=two.attributes;_Tmp1;}));
# 1424
goto _LL0;}case 65:
# 1425 "parse.y"
 yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=7U,({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.tq=_Tmp2;}),({
struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.type_specs=_Tmp2;}),_Tmp1.is_inline=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;case 66:  {
# 1428 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY18(& yyyvsp[1].v);
yyval=Cyc_YY18(({struct Cyc_Parse_Declaration_spec _Tmp1;_Tmp1.sc=two.sc,_Tmp1.tq=two.tq,_Tmp1.type_specs=two.type_specs,_Tmp1.is_inline=two.is_inline,({
# 1431
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY47(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.attributes);});_Tmp1.attributes=_Tmp2;});_Tmp1;}));
goto _LL0;}case 67:
# 1435 "parse.y"
 yyval=Cyc_YY21(4U);
goto _LL0;case 68:
# 1436 "parse.y"
 yyval=Cyc_YY21(5U);
goto _LL0;case 69:
# 1437 "parse.y"
 yyval=Cyc_YY21(3U);
goto _LL0;case 70:
# 1438 "parse.y"
 yyval=Cyc_YY21(1U);
goto _LL0;case 71:
# 1440 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("C",sizeof(char),2U));})!=0)
Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY21(2U);
# 1444
goto _LL0;case 72:
# 1444 "parse.y"
 yyval=Cyc_YY21(0U);
goto _LL0;case 73:
# 1446 "parse.y"
 yyval=Cyc_YY21(6U);
goto _LL0;case 74:
# 1451 "parse.y"
 yyval=Cyc_YY47(0);
goto _LL0;case 75:
# 1452 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 76:
# 1456 "parse.y"
 yyval=yyyvsp[3].v;
goto _LL0;case 77:
# 1459
 yyval=Cyc_YY47(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 78:
# 1460 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY48(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY47(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 79:
# 1463
 yyval=Cyc_YY48(({unsigned _Tmp1=yyyvsp[0].l.first_line;Cyc_Atts_parse_nullary_att(_Tmp1,Cyc_yyget_String_tok(& yyyvsp[0].v));}));
goto _LL0;case 80:
# 1464 "parse.y"
 yyval=Cyc_YY48((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81:
# 1466 "parse.y"
 yyval=Cyc_YY48(({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);unsigned _Tmp3=yyyvsp[2].l.first_line;Cyc_Atts_parse_unary_att(_Tmp1,_Tmp2,_Tmp3,Cyc_yyget_Exp_tok(& yyyvsp[2].v));}));
goto _LL0;case 82:
# 1468 "parse.y"
 yyval=Cyc_YY48(({unsigned _Tmp1=yyyvsp[0].l.first_line;unsigned _Tmp2=yyyvsp[2].l.first_line;struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[0].v);struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);unsigned _Tmp5=({
unsigned _Tmp6=yyyvsp[4].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[4].v));});
# 1468
Cyc_Atts_parse_format_att(_Tmp1,_Tmp2,_Tmp3,_Tmp4,_Tmp5,({
# 1470
unsigned _Tmp6=yyyvsp[6].l.first_line;Cyc_Parse_cnst2uint(_Tmp6,Cyc_yyget_Int_tok(& yyyvsp[6].v));}));}));
goto _LL0;case 83:
# 1480 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 84:
# 1482 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_typedef_type(_Tmp2,Cyc_yyget_YY42(& yyyvsp[1].v),0,0);});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 85:
# 1486 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_type_spec(Cyc_Absyn_void_type,yyyvsp[0].l.first_line));
goto _LL0;case 86:
# 1487 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_type_spec(Cyc_Absyn_char_type,yyyvsp[0].l.first_line));
goto _LL0;case 87:
# 1488 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_short_spec(yyyvsp[0].l.first_line));
goto _LL0;case 88:
# 1489 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,yyyvsp[0].l.first_line));
goto _LL0;case 89:
# 1490 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_long_spec(yyyvsp[0].l.first_line));
goto _LL0;case 90:
# 1491 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_type_spec(Cyc_Absyn_float_type,yyyvsp[0].l.first_line));
goto _LL0;case 91:
# 1492 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_type_spec(Cyc_Absyn_double_type,yyyvsp[0].l.first_line));
goto _LL0;case 92:
# 1493 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_signed_spec(yyyvsp[0].l.first_line));
goto _LL0;case 93:
# 1494 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_unsigned_spec(yyyvsp[0].l.first_line));
goto _LL0;case 94:
# 1495 "parse.y"
 yyval=Cyc_YY22(Cyc_Parse_complex_spec(yyyvsp[0].l.first_line));
goto _LL0;case 95:
# 1496 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 96:
# 1497 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 97:
# 1499 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 98:
# 1501 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_builtin_type(_tag_fat("__builtin_va_list",sizeof(char),18U),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 99:
# 1503 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 100:
# 1505 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_yyget_YY46(& yyyvsp[0].v);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 101:
# 1507 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 102:
# 1509 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY45(& yyyvsp[2].v)),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 103:
# 1511 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_Absyn_Type_struct));_Tmp2->tag=6,({struct Cyc_List_List*_Tmp3=({unsigned _Tmp4=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(({(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;}),_Tmp4,Cyc_List_imp_rev(Cyc_yyget_YY40(& yyyvsp[2].v)));});_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1513
goto _LL0;case 104:
# 1514 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY46(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 105:
# 1516 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 106:
# 1518 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_aqual_handle_type(Cyc_yyget_YY46(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 107:
# 1520 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_yyget_YY46(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 108:
# 1522 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 109:
# 1524 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 110:
# 1529 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Kinds_id_to_kind(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 111:  {
# 1533 "parse.y"
unsigned loc=Cyc_Flags_porting_c_code?yyyvsp[0].l.first_line: 0U;
yyval=Cyc_YY25(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=1,_Tmp1.q_volatile=0,_Tmp1.q_restrict=0,_Tmp1.real_const=1,_Tmp1.loc=loc;_Tmp1;}));
goto _LL0;}case 112:
# 1535 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=1,_Tmp1.q_restrict=0,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 113:
# 1536 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_Absyn_Tqual _Tmp1;_Tmp1.print_const=0,_Tmp1.q_volatile=0,_Tmp1.q_restrict=1,_Tmp1.real_const=0,_Tmp1.loc=0U;_Tmp1;}));
goto _LL0;case 114:  {
# 1542 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl)),({void*_Tmp1=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Enumdecl*_Tmp3=({struct Cyc_Absyn_Enumdecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_Tmp4->sc=2U,({struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp4->name=_Tmp5;}),({struct Cyc_Core_Opt*_Tmp5=({struct Cyc_Core_Opt*_Tmp6=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp7=Cyc_yyget_YY50(& yyyvsp[3].v);_Tmp6->v=_Tmp7;});_Tmp6;});_Tmp4->fields=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});ed->r=_Tmp1;}),ed->loc=yyyvsp[0].l.first_line;
# 1544
yyval=Cyc_YY22(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=10,_Tmp2->f1=ed,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1546
goto _LL0;}case 115:
# 1547 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(& yyyvsp[1].v),0);Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 116:
# 1549 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY50(& yyyvsp[2].v));Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 117:
# 1555 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),_Tmp1->tag=0,_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 118:
# 1557 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_Absyn_Enumfield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1->name=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->tag=_Tmp2;}),_Tmp1->loc=yyyvsp[0].l.first_line;_Tmp1;}));
goto _LL0;case 119:
# 1561 "parse.y"
 yyval=Cyc_YY50(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY49(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 120:
# 1562 "parse.y"
 yyval=Cyc_YY50(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY49(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 121:
# 1563 "parse.y"
 yyval=Cyc_YY50(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Enumfield*_Tmp2=Cyc_yyget_YY49(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY50(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 122:
# 1569 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_Tmp2->tag=7,({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY23(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY26(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 123:  {
# 1573 "parse.y"
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY42(& yyyvsp[2].v));});
struct Cyc_List_List*exist_ts=({unsigned _Tmp1=yyyvsp[4].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY42(& yyyvsp[4].v));});
struct _tuple28*po_qb=Cyc_yyget_YY52(& yyyvsp[5].v);
struct _tuple28 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_TypeDecl*td=({enum Cyc_Absyn_AggrKind _Tmp4=Cyc_yyget_YY24(& yyyvsp[0].v).f2;struct _tuple0*_Tmp5=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp6=ts;struct Cyc_Absyn_AggrdeclImpl*_Tmp7=({
struct Cyc_List_List*_Tmp8=exist_ts;struct Cyc_List_List*_Tmp9=po;struct Cyc_List_List*_TmpA=qb;struct Cyc_List_List*_TmpB=Cyc_yyget_YY26(& yyyvsp[6].v);Cyc_Absyn_aggrdecl_impl(_Tmp8,_Tmp9,_TmpA,_TmpB,Cyc_yyget_YY24(& yyyvsp[0].v).f1);});
# 1577
Cyc_Absyn_aggr_tdecl(_Tmp4,2U,_Tmp5,_Tmp6,_Tmp7,0,yyyvsp[0].l.first_line);});
# 1580
yyval=Cyc_YY22(({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp5->tag=10,_Tmp5->f1=td,_Tmp5->f2=0;_Tmp5;});Cyc_Parse_type_spec(_Tmp4,yyyvsp[0].l.first_line);}));
# 1582
goto _LL0;}}case 124:
# 1583 "parse.y"
 yyval=Cyc_YY22(({void*_Tmp1=({union Cyc_Absyn_AggrInfo _Tmp2=({enum Cyc_Absyn_AggrKind _Tmp3=Cyc_yyget_YY24(& yyyvsp[0].v).f2;struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_UnknownAggr(_Tmp3,_Tmp4,Cyc_yyget_YY24(& yyyvsp[0].v).f1?({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp5->v=(void*)1;_Tmp5;}): 0);});Cyc_Absyn_aggr_type(_Tmp2,
Cyc_yyget_YY42(& yyyvsp[2].v));});
# 1583
Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1585
goto _LL0;case 125:
# 1588 "parse.y"
 yyval=Cyc_YY24(({struct _tuple24 _Tmp1;_Tmp1.f1=1,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY23(& yyyvsp[1].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 126:
# 1589 "parse.y"
 yyval=Cyc_YY24(({struct _tuple24 _Tmp1;_Tmp1.f1=0,({enum Cyc_Absyn_AggrKind _Tmp2=Cyc_yyget_YY23(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 127:
# 1592
 yyval=Cyc_YY23(0U);
goto _LL0;case 128:
# 1593 "parse.y"
 yyval=Cyc_YY23(1U);
goto _LL0;case 129:
# 1596
 yyval=Cyc_YY42(0);
goto _LL0;case 130:
# 1597 "parse.y"
 yyval=Cyc_YY42(Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[1].v)));
goto _LL0;case 131:
# 1602 "parse.y"
 yyval=Cyc_YY26(0);
goto _LL0;case 132:  {
# 1604 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*x=Cyc_yyget_YY27(& yyyvsp[0].v);for(0;x!=0;x=x->tl){
decls=Cyc_List_imp_append((struct Cyc_List_List*)x->hd,decls);}}{
# 1608
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags!=0)
({(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c;})(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY26(decls);
# 1613
goto _LL0;}}case 133:
# 1617 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY26(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 134:
# 1618 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY26(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY27(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 135:
# 1624 "parse.y"
 yyval=Cyc_YY20(({struct _tuple11*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple11));_Tmp1->tl=0,({struct _tuple12 _Tmp2=Cyc_yyget_YY19(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 136:
# 1626 "parse.y"
 yyval=Cyc_YY20(({struct _tuple11*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple11));({struct _tuple11*_Tmp2=Cyc_yyget_YY20(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;}),({struct _tuple12 _Tmp2=Cyc_yyget_YY19(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;});_Tmp1;}));
goto _LL0;case 137:
# 1630 "parse.y"
 yyval=Cyc_YY19(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 138:
# 1631 "parse.y"
 yyval=Cyc_YY19(({struct _tuple12 _Tmp1;({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 139:  {
# 1636 "parse.y"
struct _RegionHandle _Tmp1=_new_region("temp");struct _RegionHandle*temp=& _Tmp1;_push_region(temp);
{struct _tuple26 _Tmp2=Cyc_yyget_YY37(& yyyvsp[0].v);void*_Tmp3;struct Cyc_Parse_Type_specifier _Tmp4;struct Cyc_Absyn_Tqual _Tmp5;_Tmp5=_Tmp2.f1;_Tmp4=_Tmp2.f2;_Tmp3=_Tmp2.f3;{struct Cyc_Absyn_Tqual tq=_Tmp5;struct Cyc_Parse_Type_specifier tspecs=_Tmp4;struct Cyc_List_List*atts=_Tmp3;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*x=Cyc_yyget_YY31(& yyyvsp[1].v);for(0;x!=0;x=x->tl){
struct _tuple25*_Tmp6=(struct _tuple25*)x->hd;void*_Tmp7;void*_Tmp8;struct Cyc_Parse_Declarator _Tmp9;_Tmp9=_Tmp6->f1;_Tmp8=_Tmp6->f2;_Tmp7=_Tmp6->f3;{struct Cyc_Parse_Declarator d=_Tmp9;struct Cyc_Absyn_Exp*wd=_Tmp8;struct Cyc_Absyn_Exp*wh=_Tmp7;
decls=({struct _tuple13*_TmpA=_region_malloc(temp,sizeof(struct _tuple13));_TmpA->tl=decls,_TmpA->hd=d;_TmpA;});
widths_and_reqs=({struct Cyc_List_List*_TmpA=_region_malloc(temp,sizeof(struct Cyc_List_List));({struct _tuple16*_TmpB=({struct _tuple16*_TmpC=_region_malloc(temp,sizeof(struct _tuple16));_TmpC->f1=wd,_TmpC->f2=wh;_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=widths_and_reqs;_TmpA;});}}}{
# 1646
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct Cyc_List_List*info=({struct _RegionHandle*_Tmp6=temp;struct _RegionHandle*_Tmp7=temp;struct Cyc_List_List*_Tmp8=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1647
Cyc_List_rzip(_Tmp6,_Tmp7,_Tmp8,widths_and_reqs);});
# 1649
yyval=Cyc_YY26(({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_make_aggr_field,yyyvsp[0].l.first_line,info));
# 1651
_npop_handler(0);goto _LL0;}}}}
# 1637
;_pop_region();}case 140:
# 1658 "parse.y"
 yyval=Cyc_YY37(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 141:  {
# 1660 "parse.y"
struct _tuple26 two=Cyc_yyget_YY37(& yyyvsp[1].v);yyval=Cyc_YY37(({struct _tuple26 _Tmp1;_Tmp1.f1=two.f1,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY22(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f2);});_Tmp1.f2=_Tmp2;}),_Tmp1.f3=two.f3;_Tmp1;}));
goto _LL0;}case 142:
# 1662 "parse.y"
 yyval=Cyc_YY37(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY25(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 143:  {
# 1664 "parse.y"
struct _tuple26 two=Cyc_yyget_YY37(& yyyvsp[1].v);yyval=Cyc_YY37(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY25(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2,_Tmp1.f3=two.f3;_Tmp1;}));
goto _LL0;}case 144:
# 1666 "parse.y"
 yyval=Cyc_YY37(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f2=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 145:  {
# 1668 "parse.y"
struct _tuple26 two=Cyc_yyget_YY37(& yyyvsp[1].v);yyval=Cyc_YY37(({struct _tuple26 _Tmp1;_Tmp1.f1=two.f1,_Tmp1.f2=two.f2,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY47(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f3);});_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;}case 146:
# 1674 "parse.y"
 yyval=Cyc_YY37(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_yyget_YY22(& yyyvsp[0].v);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 147:  {
# 1676 "parse.y"
struct _tuple26 two=Cyc_yyget_YY37(& yyyvsp[1].v);yyval=Cyc_YY37(({struct _tuple26 _Tmp1;_Tmp1.f1=two.f1,({struct Cyc_Parse_Type_specifier _Tmp2=({unsigned _Tmp3=yyyvsp[0].l.first_line;struct Cyc_Parse_Type_specifier _Tmp4=Cyc_yyget_YY22(& yyyvsp[0].v);Cyc_Parse_combine_specifiers(_Tmp3,_Tmp4,two.f2);});_Tmp1.f2=_Tmp2;}),_Tmp1.f3=two.f3;_Tmp1;}));
goto _LL0;}case 148:
# 1678 "parse.y"
 yyval=Cyc_YY37(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_yyget_YY25(& yyyvsp[0].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f2=_Tmp2;}),_Tmp1.f3=0;_Tmp1;}));
goto _LL0;case 149:  {
# 1680 "parse.y"
struct _tuple26 two=Cyc_yyget_YY37(& yyyvsp[1].v);yyval=Cyc_YY37(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=({struct Cyc_Absyn_Tqual _Tmp3=Cyc_yyget_YY25(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp3,two.f1);});_Tmp1.f1=_Tmp2;}),_Tmp1.f2=two.f2,_Tmp1.f3=two.f3;_Tmp1;}));
goto _LL0;}case 150:
# 1682 "parse.y"
 yyval=Cyc_YY37(({struct _tuple26 _Tmp1;({struct Cyc_Absyn_Tqual _Tmp2=Cyc_Absyn_empty_tqual(yyyvsp[0].l.first_line);_Tmp1.f1=_Tmp2;}),({struct Cyc_Parse_Type_specifier _Tmp2=Cyc_Parse_empty_spec(0U);_Tmp1.f2=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY47(& yyyvsp[0].v);_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 151:  {
# 1684 "parse.y"
struct _tuple26 two=Cyc_yyget_YY37(& yyyvsp[1].v);yyval=Cyc_YY37(({struct _tuple26 _Tmp1;_Tmp1.f1=two.f1,_Tmp1.f2=two.f2,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY47(& yyyvsp[0].v);Cyc_List_append(_Tmp3,two.f3);});_Tmp1.f3=_Tmp2;});_Tmp1;}));
goto _LL0;}case 152:
# 1690 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp2=Cyc_yyget_YY30(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 153:
# 1692 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp2=Cyc_yyget_YY30(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY31(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 154:
# 1697 "parse.y"
 yyval=Cyc_YY30(({struct _tuple25*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple25));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_YY4(& yyyvsp[1].v);_Tmp1->f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 155:
# 1701 "parse.y"
 yyval=Cyc_YY30(({struct _tuple25*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple25));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f1=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->f1.id=_Tmp2;}),_Tmp1->f1.varloc=0U,_Tmp1->f1.tms=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;}),_Tmp1->f3=0;_Tmp1;}));
# 1703
goto _LL0;case 156:
# 1706 "parse.y"
 yyval=Cyc_YY30(({struct _tuple25*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple25));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp4=Cyc_Absyn_Rel_n(0);_Tmp3->f1=_Tmp4;}),({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=_tag_fat("",sizeof(char),1U);_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->f1.id=_Tmp2;}),_Tmp1->f1.varloc=0U,_Tmp1->f1.tms=0,_Tmp1->f2=0,_Tmp1->f3=0;_Tmp1;}));
# 1708
goto _LL0;case 157:
# 1709 "parse.y"
 yyval=Cyc_YY30(({struct _tuple25*_Tmp1=_region_malloc(yyr,sizeof(struct _tuple25));({struct Cyc_Parse_Declarator _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f2=_Tmp2;}),_Tmp1->f3=0;_Tmp1;}));
goto _LL0;case 158:
# 1713 "parse.y"
 yyval=Cyc_YY4(Cyc_yyget_Exp_tok(& yyyvsp[2].v));
goto _LL0;case 159:
# 1714 "parse.y"
 yyval=Cyc_YY4(0);
goto _LL0;case 160:  {
# 1720 "parse.y"
int is_extensible=Cyc_yyget_YY33(& yyyvsp[0].v);
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_yyget_YY42(& yyyvsp[2].v));});
struct Cyc_Absyn_TypeDecl*dd=({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=ts;struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY36(& yyyvsp[4].v);_Tmp4->v=_Tmp5;});_Tmp4;});int _Tmp4=is_extensible;Cyc_Absyn_datatype_tdecl(2U,_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
# 1724
yyval=Cyc_YY22(({void*_Tmp1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_Tmp2->tag=10,_Tmp2->f1=dd,_Tmp2->f2=0;_Tmp2;});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1726
goto _LL0;}case 161:  {
# 1727 "parse.y"
int is_extensible=Cyc_yyget_YY33(& yyyvsp[0].v);
yyval=Cyc_YY22(({void*_Tmp1=({union Cyc_Absyn_DatatypeInfo _Tmp2=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_type(_Tmp2,Cyc_yyget_YY42(& yyyvsp[2].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1730
goto _LL0;}case 162:  {
# 1731 "parse.y"
int is_extensible=Cyc_yyget_YY33(& yyyvsp[0].v);
yyval=Cyc_YY22(({void*_Tmp1=({union Cyc_Absyn_DatatypeFieldInfo _Tmp2=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _Tmp3;({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp3.datatype_name=_Tmp4;}),({struct _tuple0*_Tmp4=Cyc_yyget_QualId_tok(& yyyvsp[3].v);_Tmp3.field_name=_Tmp4;}),_Tmp3.is_extensible=is_extensible;_Tmp3;}));Cyc_Absyn_datatype_field_type(_Tmp2,Cyc_yyget_YY42(& yyyvsp[4].v));});Cyc_Parse_type_spec(_Tmp1,yyyvsp[0].l.first_line);}));
# 1734
goto _LL0;}case 163:
# 1737 "parse.y"
 yyval=Cyc_YY33(0);
goto _LL0;case 164:
# 1738 "parse.y"
 yyval=Cyc_YY33(1);
goto _LL0;case 165:
# 1742 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY35(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 166:
# 1743 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY35(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 167:
# 1744 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY35(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY36(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 168:
# 1745 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Datatypefield*_Tmp2=Cyc_yyget_YY35(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY36(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 169:
# 1749 "parse.y"
 yyval=Cyc_YY34(2U);
goto _LL0;case 170:
# 1750 "parse.y"
 yyval=Cyc_YY34(3U);
goto _LL0;case 171:
# 1751 "parse.y"
 yyval=Cyc_YY34(0U);
goto _LL0;case 172:
# 1755 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=0,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;case 173:  {
# 1757 "parse.y"
struct Cyc_List_List*typs=({unsigned _Tmp1=yyyvsp[3].l.first_line;({(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(({(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;}),_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY40(& yyyvsp[3].v)));});
yyval=Cyc_YY35(({struct Cyc_Absyn_Datatypefield*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);_Tmp1->name=_Tmp2;}),_Tmp1->typs=typs,_Tmp1->loc=yyyvsp[0].l.first_line,({enum Cyc_Absyn_Scope _Tmp2=Cyc_yyget_YY34(& yyyvsp[0].v);_Tmp1->sc=_Tmp2;});_Tmp1;}));
goto _LL0;}case 174:
# 1763 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 175:  {
# 1765 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY29(& yyyvsp[1].v);
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY28(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 176:
# 1772 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 177:  {
# 1774 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY29(& yyyvsp[1].v);
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=two.id,_Tmp1.varloc=two.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY28(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,two.tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 178:
# 1779 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 179:
# 1780 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 180:  {
# 1784 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY29(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY47(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1788
goto _LL0;}case 181:
# 1789 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY29(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 182:
# 1791 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY29(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 183:  {
# 1794 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY41(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f1;_Tmp6=_Tmp1->f2;_Tmp5=_Tmp1->f3;_Tmp4=_Tmp1->f4;_Tmp3=_Tmp1->f5;_Tmp2=_Tmp1->f6;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_Exp*req=Cyc_yyget_YY61(& yyyvsp[4].v).f1;
struct Cyc_Absyn_Exp*ens=Cyc_yyget_YY61(& yyyvsp[4].v).f2;
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp8;({struct _tuple0*_Tmp9=Cyc_yyget_YY29(& yyyvsp[0].v).id;_Tmp8.id=_Tmp9;}),({unsigned _Tmp9=Cyc_yyget_YY29(& yyyvsp[0].v).varloc;_Tmp8.varloc=_Tmp9;}),({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=qb,_TmpE->f7=req,_TmpE->f8=ens;_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),({struct Cyc_List_List*_TmpB=Cyc_yyget_YY29(& yyyvsp[0].v).tms;_TmpA->tl=_TmpB;});_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 1799
goto _LL0;}}case 184:
# 1800 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY38(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY29(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 185:  {
# 1803
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[2].v)));});
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY29(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1806
goto _LL0;}case 186:
# 1807 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).id;_Tmp1.id=_Tmp2;}),({unsigned _Tmp2=Cyc_yyget_YY29(& yyyvsp[0].v).varloc;_Tmp1.varloc=_Tmp2;}),({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY47(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY29(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 1807
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1810
goto _LL0;case 187:
# 1814 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 188:
# 1815 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp1.id=_Tmp2;}),_Tmp1.varloc=yyyvsp[0].l.first_line,_Tmp1.tms=0;_Tmp1;}));
goto _LL0;case 189:
# 1816 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 190:  {
# 1820 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY29(& yyyvsp[2].v);
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp4=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp4->tag=5,_Tmp4->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp5=Cyc_yyget_YY47(& yyyvsp[1].v);_Tmp4->f2=_Tmp5;});_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=d.tms;_Tmp2;});d.tms=_Tmp1;});
yyval=yyyvsp[2].v;
# 1824
goto _LL0;}case 191:  {
# 1825 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY29(& yyyvsp[0].v);
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 192:  {
# 1829 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY29(& yyyvsp[0].v);
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 1833
goto _LL0;}case 193:  {
# 1834 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY41(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f1;_Tmp6=_Tmp1->f2;_Tmp5=_Tmp1->f3;_Tmp4=_Tmp1->f4;_Tmp3=_Tmp1->f5;_Tmp2=_Tmp1->f6;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
struct Cyc_Absyn_Exp*req=Cyc_yyget_YY61(& yyyvsp[4].v).f1;
struct Cyc_Absyn_Exp*ens=Cyc_yyget_YY61(& yyyvsp[4].v).f2;
struct Cyc_Parse_Declarator one=Cyc_yyget_YY29(& yyyvsp[0].v);
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp8;_Tmp8.id=one.id,_Tmp8.varloc=one.varloc,({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=qb,_TmpE->f7=req,_TmpE->f8=ens;_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=one.tms;_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 1840
goto _LL0;}}case 194:  {
# 1841 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY29(& yyyvsp[0].v);
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_Tmp5->tag=3,({void*_Tmp6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_Tmp7=_region_malloc(yyr,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_Tmp7->tag=0,({struct Cyc_List_List*_Tmp8=Cyc_yyget_YY38(& yyyvsp[2].v);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=yyyvsp[0].l.first_line;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 195:  {
# 1845
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[2].v)));});
struct Cyc_Parse_Declarator one=Cyc_yyget_YY29(& yyyvsp[0].v);
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[0].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 196:  {
# 1849 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY29(& yyyvsp[0].v);
yyval=Cyc_YY29(({struct Cyc_Parse_Declarator _Tmp1;_Tmp1.id=one.id,_Tmp1.varloc=one.varloc,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY47(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=one.tms;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;}case 197:
# 1855 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 198:
# 1856 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY28(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY28(& yyyvsp[1].v));}));
goto _LL0;case 199:  {
# 1860 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY47(& yyyvsp[3].v)!=0)
ans=({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp3->tag=5,_Tmp3->f1=yyyvsp[3].l.first_line,({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY47(& yyyvsp[3].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=ans;_Tmp1;});{
# 1864
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _Tmp1=*Cyc_yyget_YY1(& yyyvsp[0].v);void*_Tmp2;void*_Tmp3;unsigned _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{unsigned ploc=_Tmp4;void*nullable=_Tmp3;void*bound=_Tmp2;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));_Tmp5->ptr_loc=ploc,_Tmp5->rgn_loc=yyyvsp[2].l.first_line,_Tmp5->zt_loc=yyyvsp[1].l.first_line;_Tmp5;});{
void*mod=({struct _RegionHandle*_Tmp5=yyr;struct Cyc_Absyn_PtrLoc*_Tmp6=ptrloc;void*_Tmp7=nullable;void*_Tmp8=bound;void*_Tmp9=Cyc_yyget_YY46(& yyyvsp[2].v);struct Cyc_List_List*_TmpA=Cyc_yyget_YY60(& yyyvsp[1].v);Cyc_Parse_make_pointer_mod(_Tmp5,_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_yyget_YY25(& yyyvsp[4].v));});
ans=({struct Cyc_List_List*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_List_List));_Tmp5->hd=mod,_Tmp5->tl=ans;_Tmp5;});
yyval=Cyc_YY28(ans);
# 1872
goto _LL0;}}}}case 200:
# 1874
 yyval=Cyc_YY60(0);
goto _LL0;case 201:
# 1875 "parse.y"
 yyval=Cyc_YY60(({struct Cyc_List_List*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY59(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY60(& yyyvsp[1].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 202:
# 1880 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 203:
# 1881 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=1,({void*_Tmp2=Cyc_yyget_YY46(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 204:
# 1882 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=2;_Tmp1;}));
goto _LL0;case 205:
# 1883 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=3;_Tmp1;}));
goto _LL0;case 206:
# 1884 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=6;_Tmp1;}));
goto _LL0;case 207:
# 1885 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=4;_Tmp1;}));
goto _LL0;case 208:
# 1886 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=5;_Tmp1;}));
goto _LL0;case 209:
# 1887 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=7;_Tmp1;}));
goto _LL0;case 210:
# 1888 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=8;_Tmp1;}));
goto _LL0;case 211:
# 1889 "parse.y"
 yyval=Cyc_YY59((void*)({struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_Tmp1=_region_malloc(yyr,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_Tmp1->tag=9,({void*_Tmp2=Cyc_yyget_YY58(& yyyvsp[2].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 212:
# 1893 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_al_qual_type);
goto _LL0;case 213:
# 1894 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_un_qual_type);
goto _LL0;case 214:
# 1895 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rc_qual_type);
goto _LL0;case 215:
# 1896 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_dt_qual_type);
goto _LL0;case 216:
# 1897 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rtd_qual_type);
goto _LL0;case 217:
# 1899 "parse.y"
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Parse_tvar_ok(_Tmp1,yyyvsp[0].l.first_line);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_aqk);tv->kind=_Tmp1;});{
void*t=Cyc_Absyn_var_type(tv);
yyval=Cyc_YY58(Cyc_Absyn_aqual_var_type(t,Cyc_Absyn_al_qual_type));
# 1904
goto _LL0;}}case 218:
# 1906 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_aqualsof_type(Cyc_yyget_YY46(& yyyvsp[2].v)));
# 1908
goto _LL0;case 219:
# 1913 "parse.y"
 yyval=Cyc_YY1(({struct _tuple21*_Tmp1=_cycalloc(sizeof(struct _tuple21));_Tmp1->f1=yyyvsp[0].l.first_line,_Tmp1->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_Tmp1->f3=Cyc_Absyn_fat_bound_type:({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 220:
# 1914 "parse.y"
 yyval=Cyc_YY1(({struct _tuple21*_Tmp1=_cycalloc(sizeof(struct _tuple21));_Tmp1->f1=yyyvsp[0].l.first_line,_Tmp1->f2=Cyc_Absyn_false_type,({void*_Tmp2=Cyc_yyget_YY2(& yyyvsp[1].v);_Tmp1->f3=_Tmp2;});_Tmp1;}));
goto _LL0;case 221:
# 1915 "parse.y"
 yyval=Cyc_YY1(({struct _tuple21*_Tmp1=_cycalloc(sizeof(struct _tuple21));_Tmp1->f1=yyyvsp[0].l.first_line,_Tmp1->f2=Cyc_Absyn_true_type,_Tmp1->f3=Cyc_Absyn_fat_bound_type;_Tmp1;}));
goto _LL0;case 222:
# 1918
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 223:
# 1919 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 224:
# 1922
 yyval=Cyc_YY54(Cyc_Tcutil_any_bool(0));
goto _LL0;case 225:
# 1923 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_true_type);
goto _LL0;case 226:
# 1924 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_false_type);
goto _LL0;case 227:
# 1929 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 228:
# 1930 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY46(& yyyvsp[0].v),& Cyc_Kinds_trk,1);yyval=yyyvsp[0].v;
goto _LL0;case 229:
# 1931 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 230:
# 1935 "parse.y"
 yyval=Cyc_YY25(Cyc_Absyn_empty_tqual((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l.first_line));
goto _LL0;case 231:
# 1936 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_Absyn_Tqual _Tmp1=Cyc_yyget_YY25(& yyyvsp[0].v);Cyc_Absyn_combine_tqual(_Tmp1,Cyc_yyget_YY25(& yyyvsp[1].v));}));
goto _LL0;case 232:  {
# 1941 "parse.y"
struct _tuple28*po_qb=Cyc_yyget_YY52(& yyyvsp[1].v);
struct _tuple28 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY41(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));_Tmp4->f1=0,_Tmp4->f2=0,_Tmp4->f3=0,({void*_Tmp5=Cyc_yyget_YY51(& yyyvsp[0].v);_Tmp4->f4=_Tmp5;}),_Tmp4->f5=po,_Tmp4->f6=qb;_Tmp4;}));
goto _LL0;}}case 233:  {
# 1945 "parse.y"
struct _tuple28*po_qb=Cyc_yyget_YY52(& yyyvsp[2].v);
struct _tuple28 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY41(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY40(& yyyvsp[0].v));_Tmp4->f1=_Tmp5;}),_Tmp4->f2=0,_Tmp4->f3=0,({void*_Tmp5=Cyc_yyget_YY51(& yyyvsp[1].v);_Tmp4->f4=_Tmp5;}),_Tmp4->f5=po,_Tmp4->f6=qb;_Tmp4;}));
goto _LL0;}}case 234:  {
# 1949 "parse.y"
struct _tuple28*po_qb=Cyc_yyget_YY52(& yyyvsp[4].v);
struct _tuple28 _Tmp1=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=0;_Tmp2;});void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY41(({struct _tuple27*_Tmp4=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY40(& yyyvsp[0].v));_Tmp4->f1=_Tmp5;}),_Tmp4->f2=1,_Tmp4->f3=0,({void*_Tmp5=Cyc_yyget_YY51(& yyyvsp[3].v);_Tmp4->f4=_Tmp5;}),_Tmp4->f5=po,_Tmp4->f6=qb;_Tmp4;}));
goto _LL0;}}case 235:  {
# 1954
struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[2].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f1;_Tmp3=_Tmp1->f2;_Tmp2=_Tmp1->f3;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY33(& yyyvsp[1].v);v->inject=_Tmp5;});{
struct _tuple28*po_qb=Cyc_yyget_YY52(& yyyvsp[4].v);
struct _tuple28 _Tmp5=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp6;_Tmp6.f1=0,_Tmp6.f2=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{struct Cyc_List_List*po=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY41(({struct _tuple27*_Tmp8=_cycalloc(sizeof(struct _tuple27));_Tmp8->f1=0,_Tmp8->f2=0,_Tmp8->f3=v,({void*_Tmp9=Cyc_yyget_YY51(& yyyvsp[3].v);_Tmp8->f4=_Tmp9;}),_Tmp8->f5=po,_Tmp8->f6=qb;_Tmp8;}));
# 1960
goto _LL0;}}}}case 236:  {
# 1962
struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[4].v);void*_Tmp2;struct Cyc_Absyn_Tqual _Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f1;_Tmp3=_Tmp1->f2;_Tmp2=_Tmp1->f3;{struct _fat_ptr*n=_Tmp4;struct Cyc_Absyn_Tqual tq=_Tmp3;void*t=_Tmp2;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo)),v->name=n,v->tq=tq,v->type=t,({int _Tmp5=Cyc_yyget_YY33(& yyyvsp[3].v);v->inject=_Tmp5;});{
struct _tuple28*po_qb=Cyc_yyget_YY52(& yyyvsp[6].v);
struct _tuple28 _Tmp5=(unsigned)po_qb?*po_qb:({struct _tuple28 _Tmp6;_Tmp6.f1=0,_Tmp6.f2=0;_Tmp6;});void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{struct Cyc_List_List*po=_Tmp7;struct Cyc_List_List*qb=_Tmp6;
yyval=Cyc_YY41(({struct _tuple27*_Tmp8=_cycalloc(sizeof(struct _tuple27));({struct Cyc_List_List*_Tmp9=Cyc_List_imp_rev(Cyc_yyget_YY40(& yyyvsp[0].v));_Tmp8->f1=_Tmp9;}),_Tmp8->f2=0,_Tmp8->f3=v,({void*_Tmp9=Cyc_yyget_YY51(& yyyvsp[5].v);_Tmp8->f4=_Tmp9;}),_Tmp8->f5=po,_Tmp8->f6=qb;_Tmp8;}));
# 1968
goto _LL0;}}}}case 237:
# 1972 "parse.y"
 yyval=Cyc_YY46(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Parse_id2type(_Tmp1,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp2->tag=1,_Tmp2->f1=0;_Tmp2;}));}));
goto _LL0;case 238:
# 1973 "parse.y"
 yyval=Cyc_YY46(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Parse_id2type(_Tmp1,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY45(& yyyvsp[2].v)));}));
goto _LL0;case 239:
# 1976
 yyval=Cyc_YY51(0);
goto _LL0;case 240:
# 1977 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_join_eff(Cyc_yyget_YY42(& yyyvsp[1].v)));
goto _LL0;case 241:
# 1981 "parse.y"
 yyval=Cyc_YY52(0);
goto _LL0;case 242:
# 1982 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 243:
# 1987 "parse.y"
 yyval=Cyc_YY52(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=({struct _tuple29*_Tmp3[1];({struct _tuple29*_Tmp4=Cyc_yyget_YY53(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(struct _tuple29*),1));});_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 244:
# 1989 "parse.y"
 yyval=Cyc_YY52(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f1=0,({struct Cyc_List_List*_Tmp2=({struct _tuple29*_Tmp3[1];({struct _tuple29*_Tmp4=Cyc_yyget_YY53(& yyyvsp[0].v);_Tmp3[0]=_Tmp4;});Cyc_List_list(_tag_fat(_Tmp3,sizeof(struct _tuple29*),1));});_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 245:  {
# 1992 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY52(& yyyvsp[2].v);
void*_Tmp1;if(rest!=0){_Tmp1=rest->f1;{struct Cyc_List_List*rpo=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp4=Cyc_yyget_YY53(& yyyvsp[0].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=rpo;_Tmp3;});(*rest).f1=_Tmp2;});
yyval=Cyc_YY52(rest);
# 1997
goto _LL0;}}else{_throw_match();}}case 246:  {
# 1999 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY52(& yyyvsp[2].v);
void*_Tmp1;if(rest!=0){_Tmp1=rest->f2;{struct Cyc_List_List*qb=_Tmp1;
({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp4=Cyc_yyget_YY53(& yyyvsp[0].v);_Tmp3->hd=_Tmp4;}),_Tmp3->tl=qb;_Tmp3;});(*rest).f2=_Tmp2;});
yyval=Cyc_YY52(rest);
# 2004
goto _LL0;}}else{_throw_match();}}case 247:  {
# 2011 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct)),kb->tag=2,kb->f1=0,kb->f2=& Cyc_Kinds_trk;{
void*t=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_id2type(_Tmp1,(void*)kb);});
yyval=Cyc_YY53(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));({void*_Tmp2=Cyc_Absyn_join_eff(Cyc_yyget_YY42(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=t;_Tmp1;}));
# 2015
goto _LL0;}}case 248:  {
# 2020 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct)),kb->tag=0,kb->f1=& Cyc_Kinds_aqk;{
void*t=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_id2type(_Tmp1,(void*)kb);});
yyval=Cyc_YY53(({struct _tuple29*_Tmp1=_cycalloc(sizeof(struct _tuple29));_Tmp1->f1=t,({void*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
# 2024
goto _LL0;}}case 249:
# 2027 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_al_qual_type);
goto _LL0;case 250:
# 2028 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_un_qual_type);
goto _LL0;case 251:
# 2029 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_rc_qual_type);
goto _LL0;case 252:
# 2030 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_dt_qual_type);
goto _LL0;case 253:
# 2031 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_rtd_qual_type);
goto _LL0;case 254:
# 2036 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_aqualsof_type(Cyc_yyget_YY46(& yyyvsp[2].v)));
# 2038
goto _LL0;case 255:
# 2065 "parse.y"
 yyval=Cyc_YY33(0);
goto _LL0;case 256:
# 2067 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_zstrcmp(_Tmp1,_tag_fat("inject",sizeof(char),7U));})!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(yyyvsp[0].l.first_line,_tag_fat(_Tmp2,sizeof(void*),1));});
yyval=Cyc_YY33(1);
# 2071
goto _LL0;case 257:
# 2074 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 258:
# 2075 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY42(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp1,Cyc_yyget_YY42(& yyyvsp[2].v));}));
goto _LL0;case 259:
# 2079 "parse.y"
 yyval=Cyc_YY42(0);
goto _LL0;case 260:
# 2080 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 261:
# 2081 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY46(& yyyvsp[2].v));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 262:
# 2083 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY46(& yyyvsp[0].v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
# 2086
goto _LL0;case 263:
# 2091 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp3=Cyc_yyget_YY39(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);}));_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 264:
# 2093 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_Absyn_access_eff(({struct _tuple8*_Tmp3=Cyc_yyget_YY39(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[0].l.first_line);}));_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 265:
# 2098 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY39(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 266:
# 2099 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple8*_Tmp2=Cyc_yyget_YY39(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY40(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 267:  {
# 2105 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY37(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
struct Cyc_Parse_Declarator _Tmp5=Cyc_yyget_YY29(& yyyvsp[1].v);void*_Tmp6;unsigned _Tmp7;void*_Tmp8;_Tmp8=_Tmp5.id;_Tmp7=_Tmp5.varloc;_Tmp6=_Tmp5.tms;{struct _tuple0*qv=_Tmp8;unsigned varloc=_Tmp7;struct Cyc_List_List*tms=_Tmp6;
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct _tuple14 _Tmp9=Cyc_Parse_apply_tms(tq,t,atts,tms);void*_TmpA;void*_TmpB;void*_TmpC;struct Cyc_Absyn_Tqual _TmpD;_TmpD=_Tmp9.f1;_TmpC=_Tmp9.f2;_TmpB=_Tmp9.f3;_TmpA=_Tmp9.f4;{struct Cyc_Absyn_Tqual tq2=_TmpD;void*t2=_TmpC;struct Cyc_List_List*tvs=_TmpB;struct Cyc_List_List*atts2=_TmpA;
if(tvs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_err2(yyyvsp[1].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_err2(yyyvsp[0].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpE=({struct Cyc_Warn_String_Warn_Warg_struct _TmpF;_TmpF.tag=0,_TmpF.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_TmpF;});void*_TmpF[1];_TmpF[0]=& _TmpE;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpF,sizeof(void*),1));});
yyval=Cyc_YY39(({struct _tuple8*_TmpE=_cycalloc(sizeof(struct _tuple8));_TmpE->f1=idopt,_TmpE->f2=tq2,_TmpE->f3=t2;_TmpE;}));
# 2119
goto _LL0;}}}}}}case 268:  {
# 2120 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY37(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
if(atts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_Tmp6,sizeof(void*),1));});
yyval=Cyc_YY39(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f1=0,_Tmp5->f2=tq,_Tmp5->f3=t;_Tmp5;}));
# 2127
goto _LL0;}}}case 269:  {
# 2128 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY37(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tspecs=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
if(tq.loc==0U)tq.loc=yyyvsp[0].l.first_line;{
void*t=Cyc_Parse_speclist2typ(tspecs,yyyvsp[0].l.first_line);
struct Cyc_List_List*tms=Cyc_yyget_YY32(& yyyvsp[1].v).tms;
struct _tuple14 _Tmp5=Cyc_Parse_apply_tms(tq,t,atts,tms);void*_Tmp6;void*_Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;_Tmp9=_Tmp5.f1;_Tmp8=_Tmp5.f2;_Tmp7=_Tmp5.f3;_Tmp6=_Tmp5.f4;{struct Cyc_Absyn_Tqual tq2=_Tmp9;void*t2=_Tmp8;struct Cyc_List_List*tvs=_Tmp7;struct Cyc_List_List*atts2=_Tmp6;
if(tvs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=
_tag_fat("bad type parameters on formal argument, ignoring",sizeof(char),49U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpB,sizeof(void*),1));});
if(atts2!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_warn2(yyyvsp[0].l.first_line,_tag_fat(_TmpB,sizeof(void*),1));});
yyval=Cyc_YY39(({struct _tuple8*_TmpA=_cycalloc(sizeof(struct _tuple8));_TmpA->f1=0,_TmpA->f2=tq2,_TmpA->f3=t2;_TmpA;}));
# 2140
goto _LL0;}}}}case 270:
# 2143 "parse.y"
 yyval=Cyc_YY38(Cyc_List_imp_rev(Cyc_yyget_YY38(& yyyvsp[0].v)));
goto _LL0;case 271:
# 2146
 yyval=Cyc_YY38(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 272:
# 2147 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY38(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 273:
# 2151 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 274:
# 2152 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 275:
# 2157 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=36,_Tmp2->f1=0,_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 276:
# 2159 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=36,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY6(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 277:
# 2161 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_Tmp2->tag=36,_Tmp2->f1=0,({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY6(& yyyvsp[1].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 278:  {
# 2163 "parse.y"
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[2].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});void*_Tmp3=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,_Tmp3,
Cyc_Absyn_uint_exp(0U,yyyvsp[2].l.first_line));});
# 2166
vd->tq.real_const=1;
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_Tmp2->tag=27,_Tmp2->f1=vd,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2169
goto _LL0;}case 279:  {
# 2171 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_Tmp2->tag=28,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=t,_Tmp2->f3=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2174
goto _LL0;}case 280:
# 2179 "parse.y"
 yyval=Cyc_YY6(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f1=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 281:
# 2181 "parse.y"
 yyval=Cyc_YY6(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY43(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 282:
# 2183 "parse.y"
 yyval=Cyc_YY6(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));_Tmp3->f1=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY6(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 283:
# 2185 "parse.y"
 yyval=Cyc_YY6(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple34*_Tmp2=({struct _tuple34*_Tmp3=_cycalloc(sizeof(struct _tuple34));({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY43(& yyyvsp[2].v);_Tmp3->f1=_Tmp4;}),({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->f2=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY6(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 284:
# 2189 "parse.y"
 yyval=Cyc_YY43(Cyc_List_imp_rev(Cyc_yyget_YY43(& yyyvsp[0].v)));
goto _LL0;case 285:
# 2190 "parse.y"
 yyval=Cyc_YY43(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp3->tag=1,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 286:
# 2195 "parse.y"
 yyval=Cyc_YY43(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY44(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 287:
# 2196 "parse.y"
 yyval=Cyc_YY43(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY44(& yyyvsp[1].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY43(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 288:
# 2200 "parse.y"
 yyval=Cyc_YY44((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_Tmp1->tag=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 289:
# 2201 "parse.y"
 yyval=Cyc_YY44((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_Tmp1->tag=1,({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f1=_Tmp2;});_Tmp1;}));
goto _LL0;case 290:  {
# 2206 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY37(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
if(atts!=0)
Cyc_Warn_warn(yyyvsp[0].l.first_line,_tag_fat("ignoring attributes in type",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY39(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f1=0,_Tmp5->f2=tq,_Tmp5->f3=t;_Tmp5;}));
# 2212
goto _LL0;}}case 291:  {
# 2213 "parse.y"
struct _tuple26 _Tmp1=Cyc_yyget_YY37(& yyyvsp[0].v);void*_Tmp2;struct Cyc_Parse_Type_specifier _Tmp3;struct Cyc_Absyn_Tqual _Tmp4;_Tmp4=_Tmp1.f1;_Tmp3=_Tmp1.f2;_Tmp2=_Tmp1.f3;{struct Cyc_Absyn_Tqual tq=_Tmp4;struct Cyc_Parse_Type_specifier tss=_Tmp3;struct Cyc_List_List*atts=_Tmp2;
void*t=Cyc_Parse_speclist2typ(tss,yyyvsp[0].l.first_line);
struct Cyc_List_List*tms=Cyc_yyget_YY32(& yyyvsp[1].v).tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);
if(t_info.f3!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,_tag_fat("bad type params, ignoring",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));
if(t_info.f4!=0)
Cyc_Warn_warn(yyyvsp[1].l.first_line,_tag_fat("bad specifiers, ignoring",sizeof(char),25U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY39(({struct _tuple8*_Tmp5=_cycalloc(sizeof(struct _tuple8));_Tmp5->f1=0,_Tmp5->f2=t_info.f1,_Tmp5->f3=t_info.f2;_Tmp5;}));
# 2223
goto _LL0;}}case 292:
# 2226 "parse.y"
 yyval=Cyc_YY46(({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[0].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 293:
# 2227 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_join_eff(0));
goto _LL0;case 294:
# 2228 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_join_eff(Cyc_yyget_YY42(& yyyvsp[1].v)));
goto _LL0;case 295:
# 2229 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY46(& yyyvsp[2].v)));
goto _LL0;case 296:
# 2230 "parse.y"
 yyval=Cyc_YY46(Cyc_Absyn_join_eff(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[2].v);_Tmp1->tl=_Tmp2;});_Tmp1;})));
goto _LL0;case 297:
# 2231 "parse.y"
 yyval=Cyc_YY46(Cyc_yyget_YY46(& yyyvsp[0].v));
goto _LL0;case 298:
# 2237 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY46(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 299:
# 2238 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=Cyc_yyget_YY46(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY42(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 300:
# 2242 "parse.y"
 yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY28(& yyyvsp[0].v);_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 301:
# 2243 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 302:
# 2245 "parse.y"
 yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY28(& yyyvsp[0].v);Cyc_List_imp_append(_Tmp3,Cyc_yyget_YY32(& yyyvsp[1].v).tms);});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 303:
# 2250 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 304:
# 2252 "parse.y"
 yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[2].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 305:
# 2254 "parse.y"
 yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=0,({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY32(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 306:
# 2256 "parse.y"
 yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[3].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[3].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=0;_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
goto _LL0;case 307:
# 2258 "parse.y"
 yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_Tmp5->tag=1,({struct Cyc_Absyn_Exp*_Tmp6=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp5->f1=_Tmp6;}),({void*_Tmp6=Cyc_yyget_YY54(& yyyvsp[4].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=yyyvsp[4].l.first_line;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY32(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2258
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2261
goto _LL0;case 308:  {
# 2262 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY41(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f1;_Tmp6=_Tmp1->f2;_Tmp5=_Tmp1->f3;_Tmp4=_Tmp1->f4;_Tmp3=_Tmp1->f5;_Tmp2=_Tmp1->f6;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp8;({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=qb,({struct Cyc_Absyn_Exp*_TmpF=Cyc_yyget_YY61(& yyyvsp[3].v).f1;_TmpE->f7=_TmpF;}),({struct Cyc_Absyn_Exp*_TmpF=Cyc_yyget_YY61(& yyyvsp[3].v).f2;_TmpE->f8=_TmpF;});_TmpE;});_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),_TmpA->tl=0;_TmpA;});_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 2265
goto _LL0;}}case 309:  {
# 2266 "parse.y"
struct _tuple27*_Tmp1=Cyc_yyget_YY41(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;int _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->f1;_Tmp6=_Tmp1->f2;_Tmp5=_Tmp1->f3;_Tmp4=_Tmp1->f4;_Tmp3=_Tmp1->f5;_Tmp2=_Tmp1->f6;{struct Cyc_List_List*lis=_Tmp7;int b=_Tmp6;struct Cyc_Absyn_VarargInfo*c=_Tmp5;void*eff=_Tmp4;struct Cyc_List_List*po=_Tmp3;struct Cyc_List_List*qb=_Tmp2;
yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp8;({struct Cyc_List_List*_Tmp9=({struct Cyc_List_List*_TmpA=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_TmpB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TmpC=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_TmpC->tag=3,({void*_TmpD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TmpE=_region_malloc(yyr,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_TmpE->tag=1,_TmpE->f1=lis,_TmpE->f2=b,_TmpE->f3=c,_TmpE->f4=eff,_TmpE->f5=po,_TmpE->f6=qb,({
struct Cyc_Absyn_Exp*_TmpF=Cyc_yyget_YY61(& yyyvsp[4].v).f1;_TmpE->f7=_TmpF;}),({struct Cyc_Absyn_Exp*_TmpF=Cyc_yyget_YY61(& yyyvsp[4].v).f2;_TmpE->f8=_TmpF;});_TmpE;});
# 2267
_TmpC->f1=_TmpD;});_TmpC;});_TmpA->hd=_TmpB;}),({
struct Cyc_List_List*_TmpB=Cyc_yyget_YY32(& yyyvsp[0].v).tms;_TmpA->tl=_TmpB;});_TmpA;});
# 2267
_Tmp8.tms=_Tmp9;});_Tmp8;}));
# 2270
goto _LL0;}}case 310:  {
# 2272
struct Cyc_List_List*ts=({unsigned _Tmp1=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp1,Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[2].v)));});
yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_Tmp5->tag=4,_Tmp5->f1=ts,_Tmp5->f2=yyyvsp[1].l.first_line,_Tmp5->f3=0;_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=Cyc_yyget_YY32(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});
# 2273
_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2276
goto _LL0;}case 311:
# 2277 "parse.y"
 yyval=Cyc_YY32(({struct Cyc_Parse_Abstractdeclarator _Tmp1;({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_region_malloc(yyr,sizeof(struct Cyc_List_List));({void*_Tmp4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_Tmp5=_region_malloc(yyr,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_Tmp5->tag=5,_Tmp5->f1=yyyvsp[1].l.first_line,({struct Cyc_List_List*_Tmp6=Cyc_yyget_YY47(& yyyvsp[1].v);_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY32(& yyyvsp[0].v).tms;_Tmp3->tl=_Tmp4;});_Tmp3;});_Tmp1.tms=_Tmp2;});_Tmp1;}));
# 2279
goto _LL0;case 312:
# 2282 "parse.y"
 yyval=Cyc_YY61(({struct _tuple16 _Tmp1;_Tmp1.f1=0,_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 313:
# 2283 "parse.y"
 yyval=Cyc_YY61(({struct _tuple16 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),_Tmp1.f2=0;_Tmp1;}));
goto _LL0;case 314:
# 2284 "parse.y"
 yyval=Cyc_YY61(({struct _tuple16 _Tmp1;_Tmp1.f1=0,({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 315:
# 2286 "parse.y"
 yyval=Cyc_YY61(({struct _tuple16 _Tmp1;({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1.f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[6].v);_Tmp1.f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 316:
# 2290 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 317:
# 2291 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 318:
# 2292 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 319:
# 2293 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 320:
# 2294 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 321:
# 2295 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 322:
# 2301 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 323:
# 2305 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 324:
# 2306 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_exp_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 325:
# 2311 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 326:
# 2312 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 327:
# 2317 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_Absyn_skip_stmt(yyyvsp[0].l.first_line));}));
goto _LL0;case 328:
# 2318 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY17(& yyyvsp[0].v);Cyc_Parse_flatten_declarations(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 329:
# 2320 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY17(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_Absyn_skip_stmt(0U));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 330:
# 2322 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_Tmp2->tag=13,({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;}),({struct Cyc_Absyn_Stmt*_Tmp3=({struct Cyc_List_List*_Tmp4=Cyc_yyget_YY17(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp4,Cyc_yyget_Stmt_tok(& yyyvsp[3].v));});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 331:
# 2323 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 332:
# 2324 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[0].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);Cyc_Absyn_seq_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 333:
# 2325 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY16(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 334:
# 2328 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_Tmp1=({void*_Tmp2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_Tmp3->tag=1,({struct Cyc_Absyn_Fndecl*_Tmp4=Cyc_yyget_YY16(& yyyvsp[0].v);_Tmp3->f1=_Tmp4;});_Tmp3;});Cyc_Absyn_new_decl(_Tmp2,yyyvsp[0].l.first_line);});Cyc_Parse_flatten_decl(_Tmp1,Cyc_yyget_Stmt_tok(& yyyvsp[1].v));}));
goto _LL0;case 335:
# 2333 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 336:
# 2335 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);struct Cyc_Absyn_Stmt*_Tmp3=Cyc_yyget_Stmt_tok(& yyyvsp[6].v);Cyc_Absyn_ifthenelse_stmt(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 337:
# 2341 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[5].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 338:
# 2344
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct _tuple0*_Tmp2=Cyc_yyget_QualId_tok(& yyyvsp[1].v);Cyc_Absyn_unknownid_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[3].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 339:
# 2347
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[3].v);Cyc_Absyn_tuple_exp(_Tmp2,yyyvsp[1].l.first_line);});struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[6].v);Cyc_Absyn_switch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 340:
# 2351 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[4].v);Cyc_Absyn_trycatch_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 341:
# 2365 "parse.y"
 yyval=Cyc_YY9(0);
goto _LL0;case 342:
# 2368 "parse.y"
 yyval=Cyc_YY9(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2368
_Tmp1->hd=_Tmp2;}),({
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 343:
# 2371 "parse.y"
 yyval=Cyc_YY9(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY10(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[2].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2371
_Tmp1->hd=_Tmp2;}),({
# 2373
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[3].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 344:
# 2375 "parse.y"
 yyval=Cyc_YY9(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY10(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,_Tmp3->where_clause=0,({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[3].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[4].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 345:
# 2377 "parse.y"
 yyval=Cyc_YY9(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY10(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({
struct Cyc_Absyn_Stmt*_Tmp4=Cyc_Absyn_fallthru_stmt(0,yyyvsp[4].l.first_line);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});
# 2377
_Tmp1->hd=_Tmp2;}),({
# 2379
struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[5].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 346:
# 2381 "parse.y"
 yyval=Cyc_YY9(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Switch_clause*_Tmp2=({struct Cyc_Absyn_Switch_clause*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));({struct Cyc_Absyn_Pat*_Tmp4=Cyc_yyget_YY10(& yyyvsp[1].v);_Tmp3->pattern=_Tmp4;}),_Tmp3->pat_vars=0,({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[3].v);_Tmp3->where_clause=_Tmp4;}),({struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[5].v);_Tmp3->body=_Tmp4;}),_Tmp3->loc=yyyvsp[0].l.first_line;_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY9(& yyyvsp[6].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 347:
# 2388 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Stmt*_Tmp2=Cyc_yyget_Stmt_tok(& yyyvsp[4].v);Cyc_Absyn_while_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 348:
# 2390 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_do_stmt(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 349:
# 2392 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[4].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[6].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[8].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);}));
goto _LL0;case 350:  {
# 2394 "parse.y"
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[5].v);struct Cyc_Absyn_Stmt*_Tmp4=Cyc_yyget_Stmt_tok(& yyyvsp[7].v);Cyc_Absyn_for_stmt(_Tmp1,_Tmp2,_Tmp3,_Tmp4,yyyvsp[0].l.first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY17(& yyyvsp[2].v);Cyc_Parse_flatten_declarations(_Tmp1,s);}));
goto _LL0;}case 351:
# 2398
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 352:
# 2399 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 353:
# 2402
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp2=_Tmp3;});_Tmp2;});Cyc_Absyn_goto_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 354:
# 2403 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 355:
# 2404 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt(yyyvsp[0].l.first_line));
goto _LL0;case 356:
# 2405 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 357:
# 2406 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_return_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 358:
# 2408 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 359:
# 2409 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,yyyvsp[0].l.first_line));
goto _LL0;case 360:
# 2411 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY5(& yyyvsp[2].v);Cyc_Absyn_fallthru_stmt(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 361:
# 2420 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 362:
# 2423
 yyval=yyyvsp[0].v;
goto _LL0;case 363:
# 2425 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 364:
# 2428
 yyval=yyyvsp[0].v;
goto _LL0;case 365:
# 2430 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 366:
# 2433
 yyval=yyyvsp[0].v;
goto _LL0;case 367:
# 2435 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 368:
# 2438
 yyval=yyyvsp[0].v;
goto _LL0;case 369:
# 2440 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 370:
# 2443
 yyval=yyyvsp[0].v;
goto _LL0;case 371:
# 2445 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 372:
# 2448
 yyval=yyyvsp[0].v;
goto _LL0;case 373:
# 2450 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 374:
# 2453
 yyval=yyyvsp[0].v;
goto _LL0;case 375:
# 2455 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 376:
# 2458
 yyval=yyyvsp[0].v;
goto _LL0;case 377:
# 2460 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 378:
# 2463
 yyval=yyyvsp[0].v;
goto _LL0;case 379:
# 2465 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 380:
# 2467 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 381:
# 2470
 yyval=yyyvsp[0].v;
goto _LL0;case 382:
# 2472 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 383:
# 2475
 yyval=yyyvsp[0].v;
goto _LL0;case 384:
# 2477 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_Parse_pat2exp(Cyc_yyget_YY10(& yyyvsp[0].v));struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);})));
goto _LL0;case 385:
# 2480
 yyval=yyyvsp[0].v;
goto _LL0;case 386:  {
# 2482 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);})));
# 2485
goto _LL0;}case 387:
# 2488 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 388:
# 2492 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY7(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 389:  {
# 2494 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_YY10(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line)));
goto _LL0;}case 390:
# 2497 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);})));
goto _LL0;case 391:
# 2499 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(({void*_Tmp1=(*Cyc_yyget_YY39(& yyyvsp[2].v)).f3;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);})));
goto _LL0;case 392:
# 2504 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 393:
# 2512 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 394:
# 2516 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[0].l.first_line));
goto _LL0;case 395:
# 2517 "parse.y"
 yyval=Cyc_YY10(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(& yyyvsp[1].v)));
goto _LL0;case 396:
# 2518 "parse.y"
 yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_Tmp2->tag=15,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 397:
# 2519 "parse.y"
 yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=6,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY10(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 398:  {
# 2521 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(& yyyvsp[0].v);
{void*_Tmp1=e->r;struct _fat_ptr _Tmp2;int _Tmp3;short _Tmp4;char _Tmp5;enum Cyc_Absyn_Sign _Tmp6;if(*((int*)_Tmp1)==0)switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.LongLong_c.tag){case 2: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f1;_Tmp5=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Char_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp6;char i=_Tmp5;
# 2524
yyval=Cyc_YY10(({void*_Tmp7=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_Tmp8->tag=11,_Tmp8->f1=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4C3;}case 4: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f1;_Tmp4=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Short_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp6;short i=_Tmp4;
# 2526
yyval=Cyc_YY10(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=10,_Tmp8->f1=s,_Tmp8->f2=(int)i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4C3;}case 5: _Tmp6=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f1;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Int_c.val.f2;{enum Cyc_Absyn_Sign s=_Tmp6;int i=_Tmp3;
# 2528
yyval=Cyc_YY10(({void*_Tmp7=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp8->tag=10,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4C3;}case 7: _Tmp2=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f1;_Tmp3=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp1)->f1.Float_c.val.f2;{struct _fat_ptr s=_Tmp2;int i=_Tmp3;
# 2530
yyval=Cyc_YY10(({void*_Tmp7=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_Tmp8->tag=12,_Tmp8->f1=s,_Tmp8->f2=i;_Tmp8;});Cyc_Absyn_new_pat(_Tmp7,e->loc);}));goto _LL4C3;}case 1:
# 2532
 yyval=Cyc_YY10(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL4C3;case 8:
 goto _LL4D1;case 9: _LL4D1:
# 2535
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("strings cannot occur within patterns",sizeof(char),37U),_tag_fat(0U,sizeof(void*),0));goto _LL4C3;case 6:
# 2537
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));goto _LL4C3;default: goto _LL4D4;}else{_LL4D4:
# 2539
 Cyc_Warn_err(yyyvsp[0].l.first_line,_tag_fat("bad constant in case",sizeof(char),21U),_tag_fat(0U,sizeof(void*),0));}_LL4C3:;}
# 2542
goto _LL0;}case 399:
# 2543 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[1].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `as'",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp2->tag=1,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY10(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2545
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2548
goto _LL0;case 400:
# 2549 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `alias'",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_rk;_Tmp2;});tv->kind=_Tmp1;});{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY39(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0);});
yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2558
goto _LL0;}}}case 401:
# 2559 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_strcmp(_Tmp1,_tag_fat("alias",sizeof(char),6U));})!=0)
Cyc_Warn_err(yyyvsp[1].l.first_line,_tag_fat("expecting `alias'",sizeof(char),18U),_tag_fat(0U,sizeof(void*),0));{
unsigned location=yyyvsp[0].l.first_line;
({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_tvar_ok(_Tmp1,location);});{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp2=_Tmp3;});_Tmp2;});tv->name=_Tmp1;}),tv->identity=-1,({void*_Tmp1=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_Tmp2->tag=0,_Tmp2->f1=& Cyc_Kinds_rk;_Tmp2;});tv->kind=_Tmp1;});{
struct Cyc_Absyn_Vardecl*vd=({unsigned _Tmp1=yyyvsp[0].l.first_line;struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_cycalloc(sizeof(struct _tuple0));_Tmp3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[5].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f2=_Tmp4;});_Tmp3;});Cyc_Absyn_new_vardecl(_Tmp1,_Tmp2,({
struct _tuple8*_Tmp3=Cyc_yyget_YY39(& yyyvsp[4].v);Cyc_Parse_type_name_to_type(_Tmp3,yyyvsp[4].l.first_line);}),0);});
yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_Tmp2->tag=2,_Tmp2->f1=tv,_Tmp2->f2=vd;_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,location);}));
# 2568
goto _LL0;}}}case 402:  {
# 2569 "parse.y"
struct _tuple22 _Tmp1=*Cyc_yyget_YY11(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY10(({void*_Tmp4=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct));_Tmp5->tag=5,_Tmp5->f1=ps,_Tmp5->f2=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2572
goto _LL0;}}case 403:  {
# 2573 "parse.y"
struct _tuple22 _Tmp1=*Cyc_yyget_YY11(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*ps=_Tmp3;int dots=_Tmp2;
yyval=Cyc_YY10(({void*_Tmp4=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_Tmp5->tag=16,({struct _tuple0*_Tmp6=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),_Tmp5->f2=ps,_Tmp5->f3=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2576
goto _LL0;}}case 404:  {
# 2577 "parse.y"
struct _tuple22 _Tmp1=*Cyc_yyget_YY15(& yyyvsp[3].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({unsigned _Tmp4=yyyvsp[2].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp4,Cyc_yyget_YY42(& yyyvsp[2].v));});
yyval=Cyc_YY10(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=7,({union Cyc_Absyn_AggrInfo*_Tmp6=({union Cyc_Absyn_AggrInfo*_Tmp7=_cycalloc(sizeof(union Cyc_Absyn_AggrInfo));({union Cyc_Absyn_AggrInfo _Tmp8=Cyc_Absyn_UnknownAggr(0U,Cyc_yyget_QualId_tok(& yyyvsp[0].v),0);*_Tmp7=_Tmp8;});_Tmp7;});_Tmp5->f1=_Tmp6;}),_Tmp5->f2=exist_ts,_Tmp5->f3=fps,_Tmp5->f4=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2582
goto _LL0;}}case 405:  {
# 2583 "parse.y"
struct _tuple22 _Tmp1=*Cyc_yyget_YY15(& yyyvsp[2].v);int _Tmp2;void*_Tmp3;_Tmp3=_Tmp1.f1;_Tmp2=_Tmp1.f2;{struct Cyc_List_List*fps=_Tmp3;int dots=_Tmp2;
struct Cyc_List_List*exist_ts=({unsigned _Tmp4=yyyvsp[1].l.first_line;({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Parse_typ2tvar,_Tmp4,Cyc_yyget_YY42(& yyyvsp[1].v));});
yyval=Cyc_YY10(({void*_Tmp4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp5->tag=7,_Tmp5->f1=0,_Tmp5->f2=exist_ts,_Tmp5->f3=fps,_Tmp5->f4=dots;_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);}));
# 2587
goto _LL0;}}case 406:
# 2588 "parse.y"
 yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp2->tag=6,({struct Cyc_Absyn_Pat*_Tmp3=({void*_Tmp4=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_Tmp5->tag=6,({struct Cyc_Absyn_Pat*_Tmp6=Cyc_yyget_YY10(& yyyvsp[1].v);_Tmp5->f1=_Tmp6;});_Tmp5;});Cyc_Absyn_new_pat(_Tmp4,yyyvsp[0].l.first_line);});_Tmp2->f1=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 407:
# 2590 "parse.y"
 yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
# 2592
struct Cyc_Absyn_Pat*_Tmp3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,yyyvsp[1].l.first_line);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2590
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2594
goto _LL0;case 408:
# 2595 "parse.y"
 if(({struct _fat_ptr _Tmp1=(struct _fat_ptr)Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_strcmp(_Tmp1,_tag_fat("as",sizeof(char),3U));})!=0)
Cyc_Warn_err(yyyvsp[2].l.first_line,_tag_fat("expecting `as'",sizeof(char),15U),_tag_fat(0U,sizeof(void*),0));
yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_Tmp2->tag=3,({struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[1].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,Cyc_Absyn_void_type,0);});_Tmp2->f1=_Tmp3;}),({
# 2599
struct Cyc_Absyn_Pat*_Tmp3=Cyc_yyget_YY10(& yyyvsp[3].v);_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2597
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2601
goto _LL0;case 409:  {
# 2602 "parse.y"
void*tag=({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Parse_id2type(_Tmp1,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});
yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,({struct Cyc_Absyn_Tvar*_Tmp3=Cyc_Parse_typ2tvar(yyyvsp[2].l.first_line,tag);_Tmp2->f1=_Tmp3;}),({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(tag),0);});
# 2604
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2603
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2607
goto _LL0;}case 410:  {
# 2608 "parse.y"
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));
yyval=Cyc_YY10(({void*_Tmp1=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_Tmp2->tag=4,_Tmp2->f1=tv,({
struct Cyc_Absyn_Vardecl*_Tmp3=({unsigned _Tmp4=yyyvsp[0].l.first_line;struct _tuple0*_Tmp5=({struct _tuple0*_Tmp6=_cycalloc(sizeof(struct _tuple0));_Tmp6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp8=_Tmp9;});_Tmp8;});_Tmp6->f2=_Tmp7;});_Tmp6;});Cyc_Absyn_new_vardecl(_Tmp4,_Tmp5,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2610
_Tmp2->f2=_Tmp3;});_Tmp2;});
# 2609
Cyc_Absyn_new_pat(_Tmp1,yyyvsp[0].l.first_line);}));
# 2613
goto _LL0;}case 411:
# 2616 "parse.y"
 yyval=Cyc_YY11(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY12(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 412:
# 2617 "parse.y"
 yyval=Cyc_YY11(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY12(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=1;_Tmp1;}));
goto _LL0;case 413:
# 2618 "parse.y"
 yyval=Cyc_YY11(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f1=0,_Tmp1->f2=1;_Tmp1;}));
goto _LL0;case 414:
# 2621
 yyval=Cyc_YY12(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY10(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 415:
# 2622 "parse.y"
 yyval=Cyc_YY12(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY10(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY12(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 416:
# 2625
 yyval=Cyc_YY13(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));_Tmp1->f1=0,({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY10(& yyyvsp[0].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 417:
# 2626 "parse.y"
 yyval=Cyc_YY13(({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY43(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;}),({struct Cyc_Absyn_Pat*_Tmp2=Cyc_yyget_YY10(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 418:
# 2629
 yyval=Cyc_YY15(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY14(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 419:
# 2630 "parse.y"
 yyval=Cyc_YY15(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));({struct Cyc_List_List*_Tmp2=Cyc_List_rev(Cyc_yyget_YY14(& yyyvsp[0].v));_Tmp1->f1=_Tmp2;}),_Tmp1->f2=1;_Tmp1;}));
goto _LL0;case 420:
# 2631 "parse.y"
 yyval=Cyc_YY15(({struct _tuple22*_Tmp1=_cycalloc(sizeof(struct _tuple22));_Tmp1->f1=0,_Tmp1->f2=1;_Tmp1;}));
goto _LL0;case 421:
# 2634
 yyval=Cyc_YY14(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple23*_Tmp2=Cyc_yyget_YY13(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 422:
# 2635 "parse.y"
 yyval=Cyc_YY14(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple23*_Tmp2=Cyc_yyget_YY13(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY14(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 423:
# 2640 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 424:
# 2641 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_seq_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 425:
# 2645 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 426:
# 2647 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Core_Opt*_Tmp2=Cyc_yyget_YY8(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assignop_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 427:
# 2649 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_swap_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 428:
# 2653 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 429:
# 2654 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)1U;_Tmp1;}));
goto _LL0;case 430:
# 2655 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)3U;_Tmp1;}));
goto _LL0;case 431:
# 2656 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)4U;_Tmp1;}));
goto _LL0;case 432:
# 2657 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)0U;_Tmp1;}));
goto _LL0;case 433:
# 2658 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)2U;_Tmp1;}));
goto _LL0;case 434:
# 2659 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)16U;_Tmp1;}));
goto _LL0;case 435:
# 2660 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)17U;_Tmp1;}));
goto _LL0;case 436:
# 2661 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)13U;_Tmp1;}));
goto _LL0;case 437:
# 2662 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)15U;_Tmp1;}));
goto _LL0;case 438:
# 2663 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp1->v=(void*)14U;_Tmp1;}));
goto _LL0;case 439:
# 2667 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 440:
# 2669 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);Cyc_Absyn_conditional_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 441:
# 2671 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_throw_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 442:
# 2673 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_YY4(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 443:
# 2674 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_YY4(& yyyvsp[1].v);Cyc_Absyn_New_exp(0,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 444:
# 2675 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[3].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[5].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_YY4(& yyyvsp[1].v);Cyc_Absyn_New_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 445:
# 2677 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[3].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[5].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_YY4(& yyyvsp[1].v);Cyc_Absyn_New_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 446:
# 2681 "parse.y"
 yyval=Cyc_YY4(0);
goto _LL0;case 447:
# 2684 "parse.y"
 yyval=Cyc_YY4(Cyc_yyget_Exp_tok(& yyyvsp[1].v));
# 2686
goto _LL0;case 448:
# 2689 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 449:
# 2692
 yyval=yyyvsp[0].v;
goto _LL0;case 450:
# 2694 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_or_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 451:
# 2697
 yyval=yyyvsp[0].v;
goto _LL0;case 452:
# 2699 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_and_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 453:
# 2702
 yyval=yyyvsp[0].v;
goto _LL0;case 454:
# 2704 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(14U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 455:
# 2707
 yyval=yyyvsp[0].v;
goto _LL0;case 456:
# 2709 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(15U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 457:
# 2712
 yyval=yyyvsp[0].v;
goto _LL0;case 458:
# 2714 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(13U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 459:
# 2717
 yyval=yyyvsp[0].v;
goto _LL0;case 460:
# 2719 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 461:
# 2722
 yyval=yyyvsp[0].v;
goto _LL0;case 462:
# 2724 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_Tmp1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY68(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1(_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 463:
# 2727
 yyval=yyyvsp[0].v;
goto _LL0;case 464:
# 2729 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(16U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 465:
# 2731 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(17U,_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 466:
# 2734
 yyval=yyyvsp[0].v;
goto _LL0;case 467:
# 2736 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 468:
# 2739
 yyval=yyyvsp[0].v;
goto _LL0;case 469:
# 2741 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY7(& yyyvsp[1].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_prim2_exp(_Tmp1,_Tmp2,_Tmp3,yyyvsp[0].l.first_line);}));
goto _LL0;case 470:
# 2744
 yyval=Cyc_YY68(Cyc_Absyn_eq_exp);
goto _LL0;case 471:
# 2745 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_neq_exp);
goto _LL0;case 472:
# 2748
 yyval=Cyc_YY68(Cyc_Absyn_lt_exp);
goto _LL0;case 473:
# 2749 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_gt_exp);
goto _LL0;case 474:
# 2750 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_lte_exp);
goto _LL0;case 475:
# 2751 "parse.y"
 yyval=Cyc_YY68(Cyc_Absyn_gte_exp);
goto _LL0;case 476:
# 2754
 yyval=Cyc_YY7(0U);
goto _LL0;case 477:
# 2755 "parse.y"
 yyval=Cyc_YY7(2U);
goto _LL0;case 478:
# 2758
 yyval=Cyc_YY7(1U);
goto _LL0;case 479:
# 2759 "parse.y"
 yyval=Cyc_YY7(3U);
goto _LL0;case 480:
# 2760 "parse.y"
 yyval=Cyc_YY7(4U);
goto _LL0;case 481:
# 2764 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 482:  {
# 2766 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[1].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[1].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[3].v);Cyc_Absyn_cast_exp(_Tmp1,_Tmp2,1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;}case 483:
# 2771 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 484:
# 2772 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,0U,yyyvsp[0].l.first_line);}));
goto _LL0;case 485:
# 2773 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_increment_exp(_Tmp1,2U,yyyvsp[0].l.first_line);}));
goto _LL0;case 486:
# 2774 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_address_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 487:
# 2775 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_deref_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 488:
# 2776 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _Tmp1=Cyc_yyget_YY7(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_prim1_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 489:  {
# 2778 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,yyyvsp[0].l.first_line));
# 2781
goto _LL0;}case 490:
# 2781 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_sizeofexp_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 491:  {
# 2783 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=t;struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY3(& yyyvsp[4].v));Cyc_Absyn_offsetof_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
# 2786
goto _LL0;}case 492:
# 2788
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=0U,_Tmp2->f1.rgn=0,_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2790
goto _LL0;case 493:
# 2791 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2793
goto _LL0;case 494:
# 2794 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=2U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2796
goto _LL0;case 495:
# 2797 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=0U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),_Tmp2->f1.elt_type=0,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=1;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2799
goto _LL0;case 496:  {
# 2800 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[6].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[6].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=1U,_Tmp2->f1.rgn=0,({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2803
goto _LL0;}case 497:  {
# 2805
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[8].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[8].l.first_line);});
yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_Tmp2->tag=34,_Tmp2->f1.mknd=1U,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1.rgn=_Tmp3;}),({void**_Tmp3=({void**_Tmp4=_cycalloc(sizeof(void*));*_Tmp4=t;_Tmp4;});_Tmp2->f1.elt_type=_Tmp3;}),({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[4].v);_Tmp2->f1.num_elts=_Tmp3;}),_Tmp2->f1.fat_result=0,_Tmp2->f1.inline_call=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
# 2808
goto _LL0;}case 498:
# 2809 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=({struct Cyc_Absyn_Exp*_Tmp2[1];({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2[0]=_Tmp3;});Cyc_List_list(_tag_fat(_Tmp2,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(18U,_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 499:
# 2811 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=38,({struct Cyc_Absyn_Exp*_Tmp3=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 500:
# 2813 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_Tmp2->tag=38,({struct Cyc_Absyn_Exp*_Tmp3=({struct Cyc_Absyn_Exp*_Tmp4=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_deref_exp(_Tmp4,yyyvsp[2].l.first_line);});_Tmp2->f1=_Tmp3;}),({struct _fat_ptr*_Tmp3=({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp5=Cyc_yyget_String_tok(& yyyvsp[4].v);*_Tmp4=_Tmp5;});_Tmp4;});_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 501:  {
# 2815 "parse.y"
void*t=({struct _tuple8*_Tmp1=Cyc_yyget_YY39(& yyyvsp[2].v);Cyc_Parse_type_name_to_type(_Tmp1,yyyvsp[2].l.first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,yyyvsp[0].l.first_line));
goto _LL0;}case 502:
# 2817 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=Cyc_yyget_YY62(& yyyvsp[1].v);Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 503:
# 2818 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[1].v);Cyc_Absyn_extension_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 504:
# 2819 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_assert_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 505:
# 2823 "parse.y"
 yyval=Cyc_YY7(12U);
goto _LL0;case 506:
# 2824 "parse.y"
 yyval=Cyc_YY7(11U);
goto _LL0;case 507:
# 2825 "parse.y"
 yyval=Cyc_YY7(2U);
goto _LL0;case 508:
# 2826 "parse.y"
 yyval=Cyc_YY7(0U);
goto _LL0;case 509:  {
# 2831 "parse.y"
struct _tuple31*_Tmp1=Cyc_yyget_YY63(& yyyvsp[3].v);void*_Tmp2;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp1->f1;_Tmp3=_Tmp1->f2;_Tmp2=_Tmp1->f3;{struct Cyc_List_List*outlist=_Tmp4;struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY62((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_Tmp5->tag=40,({int _Tmp6=Cyc_yyget_YY33(& yyyvsp[0].v);_Tmp5->f1=_Tmp6;}),({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);_Tmp5->f2=_Tmp6;}),_Tmp5->f3=outlist,_Tmp5->f4=inlist,_Tmp5->f5=clobbers;_Tmp5;}));
goto _LL0;}}case 510:
# 2836 "parse.y"
 yyval=Cyc_YY33(0);
goto _LL0;case 511:
# 2837 "parse.y"
 yyval=Cyc_YY33(1);
goto _LL0;case 512:
# 2841 "parse.y"
 yyval=Cyc_YY63(({struct _tuple31*_Tmp1=_cycalloc(sizeof(struct _tuple31));_Tmp1->f1=0,_Tmp1->f2=0,_Tmp1->f3=0;_Tmp1;}));
goto _LL0;case 513:  {
# 2843 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY64(& yyyvsp[1].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY63(({struct _tuple31*_Tmp4=_cycalloc(sizeof(struct _tuple31));_Tmp4->f1=0,_Tmp4->f2=inlist,_Tmp4->f3=clobbers;_Tmp4;}));
goto _LL0;}}case 514:  {
# 2846 "parse.y"
struct _tuple28*_Tmp1=Cyc_yyget_YY64(& yyyvsp[2].v);void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp1->f1;_Tmp2=_Tmp1->f2;{struct Cyc_List_List*inlist=_Tmp3;struct Cyc_List_List*clobbers=_Tmp2;
yyval=Cyc_YY63(({struct _tuple31*_Tmp4=_cycalloc(sizeof(struct _tuple31));({struct Cyc_List_List*_Tmp5=Cyc_List_imp_rev(Cyc_yyget_YY66(& yyyvsp[1].v));_Tmp4->f1=_Tmp5;}),_Tmp4->f2=inlist,_Tmp4->f3=clobbers;_Tmp4;}));
goto _LL0;}}case 515:
# 2850
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 516:
# 2851 "parse.y"
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY67(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 517:
# 2855 "parse.y"
 yyval=Cyc_YY64(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f1=0,_Tmp1->f2=0;_Tmp1;}));
goto _LL0;case 518:
# 2856 "parse.y"
 yyval=Cyc_YY64(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));_Tmp1->f1=0,({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[1].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 519:
# 2857 "parse.y"
 yyval=Cyc_YY64(({struct _tuple28*_Tmp1=_cycalloc(sizeof(struct _tuple28));({struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY66(& yyyvsp[1].v));_Tmp1->f1=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[2].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 520:
# 2860
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY67(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 521:
# 2861 "parse.y"
 yyval=Cyc_YY66(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple32*_Tmp2=Cyc_yyget_YY67(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY66(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 522:
# 2865 "parse.y"
 yyval=Cyc_YY67(({struct _tuple32*_Tmp1=_cycalloc(sizeof(struct _tuple32));({struct _fat_ptr _Tmp2=Cyc_yyget_String_tok(& yyyvsp[0].v);_Tmp1->f1=_Tmp2;}),({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 523:
# 2870 "parse.y"
 yyval=Cyc_YY65(0);
goto _LL0;case 524:
# 2871 "parse.y"
 yyval=Cyc_YY65(0);
goto _LL0;case 525:
# 2872 "parse.y"
 yyval=Cyc_YY65(Cyc_List_imp_rev(Cyc_yyget_YY65(& yyyvsp[1].v)));
goto _LL0;case 526:
# 2875
 yyval=Cyc_YY65(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 527:
# 2876 "parse.y"
 yyval=Cyc_YY65(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY65(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 528:
# 2881 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 529:
# 2883 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);Cyc_Absyn_subscript_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 530:
# 2885 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_unknowncall_exp(_Tmp1,0,yyyvsp[0].l.first_line);}));
goto _LL0;case 531:
# 2887 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[2].v);Cyc_Absyn_unknowncall_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 532:
# 2889 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrmember_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 533:
# 2891 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp3=_Tmp4;});_Tmp3;});Cyc_Absyn_aggrarrow_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 534:
# 2893 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,1U,yyyvsp[0].l.first_line);}));
goto _LL0;case 535:
# 2895 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_increment_exp(_Tmp1,3U,yyyvsp[0].l.first_line);}));
goto _LL0;case 536:
# 2897 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=25,({struct _tuple8*_Tmp3=Cyc_yyget_YY39(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),_Tmp2->f2=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 537:
# 2899 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=25,({struct _tuple8*_Tmp3=Cyc_yyget_YY39(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY6(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 538:
# 2901 "parse.y"
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_Tmp2->tag=25,({struct _tuple8*_Tmp3=Cyc_yyget_YY39(& yyyvsp[1].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY6(& yyyvsp[4].v));_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 539:
# 2906 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 540:
# 2909
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct));_Tmp3->tag=1,({unsigned _Tmp4=({unsigned _Tmp5=yyyvsp[0].l.first_line;Cyc_Parse_cnst2uint(_Tmp5,Cyc_yyget_Int_tok(& yyyvsp[0].v));});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 541:
# 2911 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_Tmp3->tag=0,({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=Cyc_yyget_String_tok(& yyyvsp[2].v);*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 542:
# 2914
 yyval=Cyc_YY3(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp2=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct));_Tmp3->tag=1,({unsigned _Tmp4=({unsigned _Tmp5=yyyvsp[2].l.first_line;Cyc_Parse_cnst2uint(_Tmp5,Cyc_yyget_Int_tok(& yyyvsp[2].v));});_Tmp3->f1=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY3(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 543:
# 2919 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_Tmp1=Cyc_yyget_QualId_tok(& yyyvsp[0].v);Cyc_Absyn_unknownid_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 544:
# 2920 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[2].v);Cyc_Absyn_pragma_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 545:
# 2921 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 546:
# 2922 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_string_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 547:
# 2923 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wstring_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 548:
# 2924 "parse.y"
 yyval=yyyvsp[1].v;
goto _LL0;case 549:
# 2928 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);Cyc_Absyn_noinstantiate_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 550:
# 2931
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_Tmp1=Cyc_yyget_Exp_tok(& yyyvsp[0].v);struct Cyc_List_List*_Tmp2=Cyc_List_imp_rev(Cyc_yyget_YY42(& yyyvsp[3].v));Cyc_Absyn_instantiate_exp(_Tmp1,_Tmp2,yyyvsp[0].l.first_line);}));
goto _LL0;case 551:
# 2934
 yyval=Cyc_Exp_tok(({void*_Tmp1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_Tmp2->tag=29,({struct _tuple0*_Tmp3=Cyc_yyget_QualId_tok(& yyyvsp[0].v);_Tmp2->f1=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_yyget_YY42(& yyyvsp[2].v);_Tmp2->f2=_Tmp3;}),({struct Cyc_List_List*_Tmp3=Cyc_List_imp_rev(Cyc_yyget_YY6(& yyyvsp[3].v));_Tmp2->f3=_Tmp3;}),_Tmp2->f4=0;_Tmp2;});Cyc_Absyn_new_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 552:
# 2936 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_Tmp1=Cyc_yyget_YY5(& yyyvsp[2].v);Cyc_Absyn_tuple_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 553:
# 2938 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_Tmp1=Cyc_yyget_Stmt_tok(& yyyvsp[2].v);Cyc_Absyn_stmt_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 554:
# 2942 "parse.y"
 yyval=Cyc_YY5(Cyc_List_imp_rev(Cyc_yyget_YY5(& yyyvsp[0].v)));
goto _LL0;case 555:
# 2947 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[0].v);_Tmp1->hd=_Tmp2;}),_Tmp1->tl=0;_Tmp1;}));
goto _LL0;case 556:
# 2949 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Exp*_Tmp2=Cyc_yyget_Exp_tok(& yyyvsp[2].v);_Tmp1->hd=_Tmp2;}),({struct Cyc_List_List*_Tmp2=Cyc_yyget_YY5(& yyyvsp[0].v);_Tmp1->tl=_Tmp2;});_Tmp1;}));
goto _LL0;case 557:
# 2955 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _Tmp1=Cyc_yyget_Int_tok(& yyyvsp[0].v);Cyc_Absyn_const_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 558:
# 2956 "parse.y"
 yyval=Cyc_Exp_tok(({char _Tmp1=Cyc_yyget_Char_tok(& yyyvsp[0].v);Cyc_Absyn_char_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 559:
# 2957 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _Tmp1=Cyc_yyget_String_tok(& yyyvsp[0].v);Cyc_Absyn_wchar_exp(_Tmp1,yyyvsp[0].l.first_line);}));
goto _LL0;case 560:
# 2959 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp(yyyvsp[0].l.first_line));
goto _LL0;case 561:  {
# 2961 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(& yyyvsp[0].v);
int l=(int)Cyc_strlen(f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c==102 ||(int)c==70)i=0;else{
if((int)c==108 ||(int)c==76)i=2;}}
# 2969
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,yyyvsp[0].l.first_line));
# 2971
goto _LL0;}case 562:
# 2974 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f1=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 563:
# 2975 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 564:
# 2978
 yyval=Cyc_QualId_tok(({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Rel_n(0);_Tmp1->f1=_Tmp2;}),({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_yyget_String_tok(& yyyvsp[0].v);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->f2=_Tmp2;});_Tmp1;}));
goto _LL0;case 565:
# 2979 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 566:
# 2984 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 567:
# 2985 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 568:
# 2988
 yyval=yyyvsp[0].v;
goto _LL0;case 569:
# 2989 "parse.y"
 yyval=yyyvsp[0].v;
goto _LL0;case 570:
# 2994 "parse.y"
 goto _LL0;case 571:
# 2994 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default:
# 2998
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
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr1,572U,sizeof(short),yyn));
# 411
yystate=({int _Tmp1=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,172U,sizeof(short),yyn - 163));_Tmp1 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7446)&&(int)Cyc_yycheck[yystate]==(int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,172U,sizeof(short),yyn - 163));}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus==0){
# 424
++ yynerrs;
# 427
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1115U,sizeof(short),yystate));
# 429
if(yyn > -32768 && yyn < 7446){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 335U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7447U,sizeof(short),x + yyn))==x)
({unsigned long _Tmp1=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,335U,sizeof(struct _fat_ptr),x)))+ 15U;sze +=_Tmp1;}),count ++;}
msg=({unsigned _Tmp1=(unsigned)(sze + 15)+ 1U;_tag_fat(({char*_Tmp2=({struct _RegionHandle*_Tmp3=yyregion;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(char)));});({{unsigned _Tmp3=(unsigned)(sze + 15);unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;}),sizeof(char),_Tmp1);});
Cyc_strcpy(msg,_tag_fat("parse error",sizeof(char),12U));
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 335U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7447U,sizeof(short),x + yyn))==x){
# 451
Cyc_strcat(msg,count==0?
_tag_fat(", expecting `",sizeof(char),14U):
 _tag_fat(" or `",sizeof(char),6U));
Cyc_strcat(msg,*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,335U,sizeof(struct _fat_ptr),x)));
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1115U,sizeof(short),yystate));
if(yyn==-32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7446)||(int)Cyc_yycheck[yyn]!=1)goto yyerrdefault;
# 528
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,7447U,sizeof(short),yyn));
if(yyn < 0){
# 531
if(yyn==-32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 535
if(yyn==0)goto yyerrpop;}
# 537
if(yyn==1114){
int _Tmp1=0;_npop_handler(0);return _Tmp1;}
# 546
({struct Cyc_Yystacktype _Tmp1=({struct Cyc_Yystacktype _Tmp2;_Tmp2.v=yylval,_Tmp2.l=yylloc;_Tmp2;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_Tmp1;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 2997 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
void*_Tmp0;struct _fat_ptr _Tmp1;char _Tmp2;union Cyc_Absyn_Cnst _Tmp3;switch(v.Stmt_tok.tag){case 1: _Tmp3=v.Int_tok.val;{union Cyc_Absyn_Cnst c=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 2: _Tmp2=v.Char_tok.val;{char c=_Tmp2;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)c);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%c",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 3: _Tmp1=v.String_tok.val;{struct _fat_ptr s=_Tmp1;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("\"%s\"",sizeof(char),5U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 4: _Tmp0=v.QualId_tok.val;{struct _tuple0*q=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 5: _Tmp0=v.Exp_tok.val;{struct Cyc_Absyn_Exp*e=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 6: _Tmp0=v.Stmt_tok.val;{struct Cyc_Absyn_Stmt*s=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}default:
 Cyc_fprintf(Cyc_stderr,_tag_fat("?",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));goto _LL0;}_LL0:;}
# 3009
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return _tag_fat("end-of-file",sizeof(char),12U);
if(token==378)
return Cyc_Lex_token_string;
if(token==387)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 390?(int)Cyc_yytranslate[token]: 335;
if((unsigned)z < 335U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 3023
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _Tmp0=_new_region("yyr");struct _RegionHandle*yyr=& _Tmp0;_push_region(yyr);
({struct _RegionHandle*_Tmp1=yyr;Cyc_yyparse(_Tmp1,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_Tmp1=Cyc_Parse_parse_result;_npop_handler(0);return _Tmp1;}
# 3026
;_pop_region();}}

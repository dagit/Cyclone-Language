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
  _zero_arr_inplace_plus_other_fn(t,(void***)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_otherr(t,x,i) \
  _zero_arr_inplace_plus_post_other_fn(t,(void***)(x),(i),__FILE__,__LINE__)

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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 379
extern struct Cyc_List_List*Cyc_List_tabulate_c(int,void*(*)(void*,int),void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_Set_Set;
# 54 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 94
extern int Cyc_Set_cardinality(struct Cyc_Set_Set*);
# 97
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 137
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 176
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 181
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 289
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 302
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 673 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};
# 854
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 867
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 874
void*Cyc_Absyn_compress(void*);
# 887
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 889
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 892
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 894
extern void*Cyc_Absyn_sint_type;
# 897
void*Cyc_Absyn_gen_float_type(unsigned);
# 899
extern void*Cyc_Absyn_unique_rgn_type;
# 901
extern void*Cyc_Absyn_empty_effect;
# 903
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 905
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_enum_type(struct _tuple2*,struct Cyc_Absyn_Enumdecl*);
# 934
void*Cyc_Absyn_bounds_one (void);
# 936
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 959
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 977
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1047
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
# 1131
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 69 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 74
struct _fat_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple2*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc_RgnOrder_RgnPO;
# 41 "tcutil.h"
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
int Cyc_Tcutil_is_array_type(void*);
# 52
int Cyc_Tcutil_is_nullable_pointer_type(void*,int);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
# 107
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 114
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple13{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple14{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 151
struct _tuple14*Cyc_Tcutil_r_make_inst_var(struct _tuple13*,struct Cyc_Absyn_Tvar*);
# 158
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*,unsigned,struct _fat_ptr);
# 166
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*,unsigned,struct Cyc_List_List*,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*);
# 177
int Cyc_Tcutil_is_noalias_region(void*,int);
# 185
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 228
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 235
int Cyc_Tcutil_force_type2bool(int,void*);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
# 40
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 47
extern struct Cyc_Core_Opt Cyc_Kinds_rko;
# 50
extern struct Cyc_Core_Opt Cyc_Kinds_mko;
# 56
extern struct Cyc_Core_Opt Cyc_Kinds_trko;
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 99 "tcenv.h"
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 142
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int,int);
# 144
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned,void*);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 32
struct Cyc_Absyn_PtrInfo Cyc_Tcexp_fresh_pointer_type(struct Cyc_Tcenv_Tenv*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _fat_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _fat_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};struct _tuple15{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_Tmp0=p->r;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1==0){_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_List_List*exist_ts=_Tmp4;struct Cyc_List_List*dps=_Tmp3;int dots=_Tmp2;
# 52
if(topt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="cannot determine pattern type";_tag_fat(_Tmp7,sizeof(char),30U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),1));});else{
# 55
void*_Tmp5=Cyc_Absyn_compress(*topt);union Cyc_Absyn_AggrInfo _Tmp6;if(*((int*)_Tmp5)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)==20){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1;{union Cyc_Absyn_AggrInfo ainfo=_Tmp6;
# 57
({void*_Tmp7=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_Tmp8->tag=7,({union Cyc_Absyn_AggrInfo*_Tmp9=({union Cyc_Absyn_AggrInfo*_TmpA=_cycalloc(sizeof(union Cyc_Absyn_AggrInfo));*_TmpA=ainfo;_TmpA;});_Tmp8->f1=_Tmp9;}),_Tmp8->f2=exist_ts,_Tmp8->f3=dps,_Tmp8->f4=dots;_Tmp8;});p->r=_Tmp7;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}}else{goto _LL12;}}else{_LL12:
# 61
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="pattern expects aggregate type instead of ";_tag_fat(_Tmp9,sizeof(char),43U);});_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)*topt;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp9,sizeof(void*),2));});
goto _LLF;}_LLF:;}
# 64
return;}}else{if(((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1)->KnownAggr.tag==2){_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1->KnownAggr.val;_Tmp3=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Aggrdecl**adp=_Tmp4;struct Cyc_List_List**exist_ts=(struct Cyc_List_List**)_Tmp3;struct Cyc_List_List*dps=_Tmp1;int dots=_Tmp2;
# 67
struct Cyc_Absyn_Aggrdecl*ad=*adp;
if(ad->impl==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="can't destructure an abstract";_tag_fat(_Tmp7,sizeof(char),30U);});_Tmp6;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp7;_Tmp7.tag=6,_Tmp7.f1=ad;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),2));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 73
int more_exists=({int _Tmp5=Cyc_List_length(_check_null(ad->impl)->exist_vars);_Tmp5 - Cyc_List_length(*exist_ts);});
if(more_exists < 0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="too many existential type variables in pattern";_tag_fat(_Tmp7,sizeof(char),47U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),1));});{
struct Cyc_List_List**ts=exist_ts;
{int n=Cyc_List_length(_check_null(ad->impl)->exist_vars);for(0;n!=0;-- n){
ts=& _check_null(*ts)->tl;}}
*ts=0;}}else{
if(more_exists > 0){
# 82
struct Cyc_List_List*new_ts=0;
for(1;more_exists!=0;-- more_exists){
new_ts=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Absyn_Tvar*_Tmp6=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_Tmp7->tag=1,_Tmp7->f1=0;_Tmp7;}));_Tmp5->hd=_Tmp6;}),_Tmp5->tl=new_ts;_Tmp5;});}
({struct Cyc_List_List*_Tmp5=Cyc_List_imp_append(*exist_ts,new_ts);*exist_ts=_Tmp5;});}}
# 87
return;}}}else{
# 106
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="resolve_pat unknownAggr";_tag_fat(_Tmp7,sizeof(char),24U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp6,sizeof(void*),1));});}}case 17: _Tmp4=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
# 89
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcutil_is_const_exp(e)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="non-constant expression in case pattern";_tag_fat(_Tmp7,sizeof(char),40U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),1));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 95
struct _tuple15 _Tmp5=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp6;unsigned _Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{unsigned i=_Tmp7;int known=_Tmp6;
if(!known){
({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_exp2string(e);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err(p->loc,({const char*_TmpA="cannot evaluate expression at compile time\nWe suggest using x && x == %s instead";_tag_fat(_TmpA,sizeof(char),81U);}),_tag_fat(_Tmp9,sizeof(void*),1));});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}
# 101
({void*_Tmp8=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_Tmp9->tag=10,_Tmp9->f1=2U,_Tmp9->f2=(int)i;_Tmp9;});p->r=_Tmp8;});
return;}}}case 15:
# 104
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="resolve_pat UnknownId_p";_tag_fat(_Tmp7,sizeof(char),24U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp6,sizeof(void*),1));});case 16:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="resolve_pat UnknownCall_p";_tag_fat(_Tmp7,sizeof(char),26U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp6,sizeof(void*),1));});default:
# 108
 return;};}
# 112
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned loc){
return({struct Cyc_Absyn_Pat*_Tmp0=_cycalloc(sizeof(struct Cyc_Absyn_Pat));_Tmp0->loc=loc,_Tmp0->topt=0,_Tmp0->r=(void*)& Cyc_Absyn_Wild_p_val;_Tmp0;});}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 115
static struct _tuple16*Cyc_Tcpat_wild_dlp(unsigned loc){
return({struct _tuple16*_Tmp0=_cycalloc(sizeof(struct _tuple16));_Tmp0->f1=0,({struct Cyc_Absyn_Pat*_Tmp1=Cyc_Tcpat_wild_pat(loc);_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 119
static struct Cyc_List_List*Cyc_Tcpat_remove_dots(struct Cyc_List_List*earlier,int num_pats,int num_fields,void*(*f)(unsigned),unsigned loc,struct _fat_ptr msg_str){
# 121
if(num_pats >= num_fields)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="unnecessary ... in ";_tag_fat(_Tmp2,sizeof(char),20U);});_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=msg_str;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=" pattern";_tag_fat(_Tmp4,sizeof(char),9U);});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),3));});{
struct Cyc_List_List*new_ps=0;
{int i=0;for(0;i < num_fields - num_pats;++ i){
new_ps=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp1=f(loc);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=new_ps;_Tmp0;});}}
return Cyc_List_imp_append(earlier,new_ps);}}
# 128
static struct _fat_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 131
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
return topt==0?Cyc_Absyn_new_evar(& Cyc_Kinds_mko,({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=s;_Tmp0;})):*topt;}
# 134
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 138
if(topt==0)
return numt;
if(Cyc_Tcutil_is_arithmetic_type(*topt))
return*topt;{
void*_Tmp0=Cyc_Absyn_compress(numt);if(*((int*)_Tmp0)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)){case 15:
 goto _LL4;case 16: _LL4:
 return*topt;default: goto _LL5;}else{_LL5:
 return numt;};}}struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 149
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 151
if(vd!=0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _Tmp0=Cyc_Absyn_empty_tqual(0U);(*vd)->tq=_Tmp0;});}
# 155
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple17*_Tmp2=({struct _tuple17*_Tmp3=_cycalloc(sizeof(struct _tuple17));_Tmp3->f1=vd,_Tmp3->f2=e;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*v_result_ptr;_Tmp1;});*v_result_ptr=_Tmp0;});}
# 157
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 159
void*_Tmp0;void*_Tmp1;_Tmp1=res1.tvars_and_bounds_opt;_Tmp0=res1.patvars;{struct _tuple1*p1=_Tmp1;struct Cyc_List_List*vs1=_Tmp0;
void*_Tmp2;void*_Tmp3;_Tmp3=res2.tvars_and_bounds_opt;_Tmp2=res2.patvars;{struct _tuple1*p2=_Tmp3;struct Cyc_List_List*vs2=_Tmp2;
if(p1==0 && p2==0)
return({struct Cyc_Tcpat_TcPatResult _Tmp4;_Tmp4.tvars_and_bounds_opt=0,({struct Cyc_List_List*_Tmp5=Cyc_List_append(vs1,vs2);_Tmp4.patvars=_Tmp5;});_Tmp4;});
if(p1==0)
p1=({struct _tuple1*_Tmp4=_cycalloc(sizeof(struct _tuple1));_Tmp4->f1=0,_Tmp4->f2=0;_Tmp4;});
if(p2==0)
p2=({struct _tuple1*_Tmp4=_cycalloc(sizeof(struct _tuple1));_Tmp4->f1=0,_Tmp4->f2=0;_Tmp4;});
return({struct Cyc_Tcpat_TcPatResult _Tmp4;({struct _tuple1*_Tmp5=({struct _tuple1*_Tmp6=_cycalloc(sizeof(struct _tuple1));({struct Cyc_List_List*_Tmp7=Cyc_List_append((*p1).f1,(*p2).f1);_Tmp6->f1=_Tmp7;}),({
struct Cyc_List_List*_Tmp7=Cyc_List_append((*p1).f2,(*p2).f2);_Tmp6->f2=_Tmp7;});_Tmp6;});
# 167
_Tmp4.tvars_and_bounds_opt=_Tmp5;}),({
# 169
struct Cyc_List_List*_Tmp5=Cyc_List_append(vs1,vs2);_Tmp4.patvars=_Tmp5;});_Tmp4;});}}}
# 173
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array_type(t)?({
void*_Tmp0=t;Cyc_Tcutil_promote_array(_Tmp0,rgn_opt==0?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,({struct Cyc_Core_Opt*_Tmp1=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_List_List*_Tmp2=Cyc_Tcenv_lookup_type_vars(te);_Tmp1->v=_Tmp2;});_Tmp1;})): rgn_opt,0);}): t;}struct _tuple18{struct Cyc_Absyn_Tvar*f1;int f2;};
# 180
static struct _tuple18*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple18*_Tmp0=_cycalloc(sizeof(struct _tuple18));_Tmp0->f1=tv,_Tmp0->f2=0;_Tmp0;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple20{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 184
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 187
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _Tmp0;_Tmp0.tvars_and_bounds_opt=0,_Tmp0.patvars=0;_Tmp0;});
# 192
{void*_Tmp0=p->r;void*_Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 0:
# 194
 t=({struct Cyc_List_List*_Tmp5=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_Tmp5,topt);});goto _LL0;case 1: _Tmp4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp3;
# 198
struct _tuple2*_Tmp5=vd->name;struct _fat_ptr _Tmp6;_Tmp6=*_Tmp5->f2;{struct _fat_ptr v=_Tmp6;
if(Cyc_strcmp(v,({const char*_Tmp7="true";_tag_fat(_Tmp7,sizeof(char),5U);}))==0 || Cyc_strcmp(v,({const char*_Tmp7="false";_tag_fat(_Tmp7,sizeof(char),6U);}))==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="you probably do not want to use ";_tag_fat(_Tmp9,sizeof(char),33U);});_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=v;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB=" as a local variable";_tag_fat(_TmpB,sizeof(char),21U);});_TmpA;});void*_TmpA[3];_TmpA[0]=& _Tmp7,_TmpA[1]=& _Tmp8,_TmpA[2]=& _Tmp9;Cyc_Warn_warn2(p->loc,_tag_fat(_TmpA,sizeof(void*),3));});
# 202
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=_check_null(p2->topt);
# 206
{void*_Tmp7=Cyc_Absyn_compress(t);if(*((int*)_Tmp7)==4){
# 208
if(rgn_pat==0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="array reference would point into unknown/unallowed region";_tag_fat(_TmpA,sizeof(char),58U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp9,sizeof(void*),1));});
goto _LL2E;}else{
# 212
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="pattern would point to an abstract member";_tag_fat(_TmpA,sizeof(char),42U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp9,sizeof(void*),1));});
goto _LL2E;}_LL2E:;}
# 216
({struct Cyc_Absyn_Vardecl**_Tmp7=({struct Cyc_Absyn_Vardecl**_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));*_Tmp8=vd;_Tmp8;});struct Cyc_Absyn_Exp*_Tmp8=access_exp;struct Cyc_List_List**_Tmp9=& res.patvars;Cyc_Tcpat_set_vd(_Tmp7,_Tmp8,_Tmp9,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL0;}}case 2: _Tmp4=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 219
struct Cyc_Tcenv_Tenv*te2=({unsigned _Tmp5=p->loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Tcenv_add_type_vars(_Tmp5,_Tmp6,({struct Cyc_Absyn_Tvar*_Tmp7[1];_Tmp7[0]=tv;Cyc_List_list(_tag_fat(_Tmp7,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_bounds_opt==0)
({struct _tuple1*_Tmp5=({struct _tuple1*_Tmp6=_cycalloc(sizeof(struct _tuple1));_Tmp6->f1=0,_Tmp6->f2=0;_Tmp6;});res.tvars_and_bounds_opt=_Tmp5;});
({struct Cyc_List_List*_Tmp5=({
struct Cyc_List_List*_Tmp6=(*res.tvars_and_bounds_opt).f1;Cyc_List_append(_Tmp6,({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple18*_Tmp8=({struct _tuple18*_Tmp9=_cycalloc(sizeof(struct _tuple18));_Tmp9->f1=tv,_Tmp9->f2=1;_Tmp9;});_Tmp7->hd=_Tmp8;}),_Tmp7->tl=0;_Tmp7;}));});
# 222
(*res.tvars_and_bounds_opt).f1=_Tmp5;});
# 224
({unsigned _Tmp5=p->loc;struct Cyc_Tcenv_Tenv*_Tmp6=te2;struct Cyc_List_List*_Tmp7=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_Tmp5,_Tmp6,_Tmp7,& Cyc_Kinds_tmk,1,0,vd->type);});
# 227
t=({struct Cyc_List_List*_Tmp5=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_Tmp5,topt);});
{void*_Tmp5=Cyc_Absyn_compress(t);if(*((int*)_Tmp5)==4){
# 230
if(rgn_pat==0 || !allow_ref_pat)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="array reference would point into unknown/unallowed region";_tag_fat(_Tmp8,sizeof(char),58U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL33;}else{
# 234
if(!Cyc_Kinds_kind_leq(Cyc_Tcutil_type_kind(t),& Cyc_Kinds_tmk))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="pattern would point to an abstract member";_tag_fat(_Tmp8,sizeof(char),42U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL33;}_LL33:;}
# 238
({struct Cyc_Absyn_Vardecl**_Tmp5=({struct Cyc_Absyn_Vardecl**_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));*_Tmp6=vd;_Tmp6;});struct Cyc_Absyn_Exp*_Tmp6=access_exp;struct Cyc_List_List**_Tmp7=& res.patvars;Cyc_Tcpat_set_vd(_Tmp5,_Tmp6,_Tmp7,vd->type);});
goto _LL0;}case 3: _Tmp4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;struct Cyc_Absyn_Pat*p2=_Tmp3;
# 242
res=Cyc_Tcpat_tcPatRec(te,p2,topt,rgn_pat,allow_ref_pat,access_exp);
t=_check_null(p2->topt);
if(!allow_ref_pat || rgn_pat==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="* pattern would point into an unknown/unallowed region";_tag_fat(_Tmp7,sizeof(char),55U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),1));});
goto _LL0;}else{
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="* pattern cannot take the address of an alias-free path";_tag_fat(_Tmp7,sizeof(char),56U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),1));});}{
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.elt_type=t;
pi.ptr_atts.rgn=rgn_pat;
pi.ptr_atts.nullable=Cyc_Absyn_false_type;{
void*t2=Cyc_Absyn_pointer_type(pi);
if((unsigned)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0U);
new_access_exp->topt=t2;}
# 259
({struct Cyc_Absyn_Vardecl**_Tmp5=({struct Cyc_Absyn_Vardecl**_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));*_Tmp6=vd;_Tmp6;});struct Cyc_Absyn_Exp*_Tmp6=new_access_exp;struct Cyc_List_List**_Tmp7=& res.patvars;Cyc_Tcpat_set_vd(_Tmp5,_Tmp6,_Tmp7,t2);});
goto _LL0;}}}case 4: _Tmp4=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp4;struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 265
({struct Cyc_Absyn_Vardecl**_Tmp5=({struct Cyc_Absyn_Vardecl**_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl*));*_Tmp6=vd;_Tmp6;});struct Cyc_Absyn_Exp*_Tmp6=access_exp;struct Cyc_List_List**_Tmp7=& res.patvars;Cyc_Tcpat_set_vd(_Tmp5,_Tmp6,_Tmp7,vd->type);});
# 269
({unsigned _Tmp5=p->loc;struct Cyc_Tcenv_Tenv*_Tmp6=te;Cyc_Tcenv_add_type_vars(_Tmp5,_Tmp6,({struct Cyc_Absyn_Tvar*_Tmp7[1];_Tmp7[0]=tv;Cyc_List_list(_tag_fat(_Tmp7,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_bounds_opt==0)
({struct _tuple1*_Tmp5=({struct _tuple1*_Tmp6=_cycalloc(sizeof(struct _tuple1));_Tmp6->f1=0,_Tmp6->f2=0;_Tmp6;});res.tvars_and_bounds_opt=_Tmp5;});
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple18*_Tmp7=({struct _tuple18*_Tmp8=_cycalloc(sizeof(struct _tuple18));_Tmp8->f1=tv,_Tmp8->f2=0;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=(*res.tvars_and_bounds_opt).f1;_Tmp6;});
# 272
(*res.tvars_and_bounds_opt).f1=_Tmp5;});
# 274
t=Cyc_Absyn_uint_type;
goto _LL0;}case 10: if(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1==Cyc_Absyn_Unsigned){
# 277
t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_type);goto _LL0;}else{
t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_type);goto _LL0;}case 11:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_type);goto _LL0;case 12: _Tmp2=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{int i=_Tmp2;
t=({void**_Tmp5=topt;Cyc_Tcpat_num_type(_Tmp5,Cyc_Absyn_gen_float_type((unsigned)i));});goto _LL0;}case 13: _Tmp4=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
t=({void**_Tmp5=topt;Cyc_Tcpat_num_type(_Tmp5,Cyc_Absyn_enum_type(ed->name,ed));});goto _LL0;}case 14: _Tmp4=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{void*tenum=_Tmp4;
t=Cyc_Tcpat_num_type(topt,tenum);goto _LL0;}case 9:
# 284
 if(topt!=0){
void*_Tmp5=Cyc_Absyn_compress(*topt);if(*((int*)_Tmp5)==3){
t=*topt;goto tcpat_end;}else{
goto _LL38;}_LL38:;}{
# 289
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.ptr_atts.nullable=Cyc_Absyn_true_type;
t=Cyc_Absyn_pointer_type(pi);
goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp4;
# 297
void*inner_typ=Cyc_Absyn_void_type;
void**inner_topt=0;
int elt_const=0;
if(topt!=0){
void*_Tmp5=Cyc_Absyn_compress(*topt);struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;if(*((int*)_Tmp5)==3){_Tmp7=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp5)->f1.elt_type;_Tmp6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp5)->f1.elt_tq;{void*elt_typ=_Tmp7;struct Cyc_Absyn_Tqual elt_tq=_Tmp6;
# 303
inner_typ=elt_typ;
inner_topt=& inner_typ;
elt_const=elt_tq.real_const;
goto _LL3D;}}else{
goto _LL3D;}_LL3D:;}{
# 312
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Kinds_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0U);
res=({struct Cyc_Tcpat_TcPatResult _Tmp5=res;Cyc_Tcpat_combine_results(_Tmp5,Cyc_Tcpat_tcPatRec(te,p2,inner_topt,ptr_rgn,1,new_access_exp));});
# 321
{void*_Tmp5=Cyc_Absyn_compress(_check_null(p2->topt));void*_Tmp6;void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp5)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)==19){if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1.KnownDatatypefield.tag==2){_Tmp8=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1.KnownDatatypefield.val.f1;_Tmp7=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1.KnownDatatypefield.val.f2;_Tmp6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp8;struct Cyc_Absyn_Datatypefield*tuf=_Tmp7;struct Cyc_List_List*targs=_Tmp6;
# 325
{void*_Tmp9=Cyc_Absyn_compress(inner_typ);if(*((int*)_Tmp9)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp9)->f1)==19)
# 327
goto DONT_PROMOTE;else{goto _LL4A;}}else{_LL4A:
 goto _LL47;}_LL47:;}{
# 331
void*new_type=({union Cyc_Absyn_DatatypeInfo _Tmp9=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));*_TmpA=tud;_TmpA;}));Cyc_Absyn_datatype_type(_Tmp9,targs);});
# 333
p2->topt=new_type;
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct));_Tmp9->tag=3,_Tmp9->f1.elt_type=new_type,
elt_const?({struct Cyc_Absyn_Tqual _TmpA=Cyc_Absyn_const_tqual(0U);_Tmp9->f1.elt_tq=_TmpA;}):({
struct Cyc_Absyn_Tqual _TmpA=Cyc_Absyn_empty_tqual(0U);_Tmp9->f1.elt_tq=_TmpA;}),
_Tmp9->f1.ptr_atts.rgn=ptr_rgn,({void*_TmpA=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));_Tmp9->f1.ptr_atts.nullable=_TmpA;}),({
void*_TmpA=Cyc_Absyn_bounds_one();_Tmp9->f1.ptr_atts.bounds=_TmpA;}),_Tmp9->f1.ptr_atts.zero_term=Cyc_Absyn_false_type,_Tmp9->f1.ptr_atts.ptrloc=0,({
void*_TmpA=Cyc_Tcutil_any_bool(Cyc_Tcenv_lookup_type_vars(te));_Tmp9->f1.ptr_atts.autoreleased=_TmpA;});_Tmp9;});
goto _LL42;}}}else{goto _LL45;}}else{goto _LL45;}}else{_LL45:
# 342
 DONT_PROMOTE: {
struct Cyc_Absyn_PtrInfo pi=Cyc_Tcexp_fresh_pointer_type(te);
pi.elt_type=_check_null(p2->topt);
({struct Cyc_Absyn_Tqual _Tmp9=elt_const?Cyc_Absyn_const_tqual(0U): Cyc_Absyn_empty_tqual(0U);pi.elt_tq=_Tmp9;});
pi.ptr_atts.rgn=ptr_rgn;
t=Cyc_Absyn_pointer_type(pi);}}_LL42:;}
# 349
if((unsigned)new_access_exp)new_access_exp->topt=p2->topt;
goto _LL0;}}case 5: _Tmp4=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_List_List**ps_p=_Tmp4;int dots=_Tmp2;
# 353
struct Cyc_List_List*ps=*ps_p;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt!=0){
void*_Tmp5=Cyc_Absyn_compress(*topt);void*_Tmp6;if(*((int*)_Tmp5)==6){_Tmp6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_Tmp5)->f1;{struct Cyc_List_List*tqts=_Tmp6;
# 359
topt_ts=tqts;
if(dots){
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=ps;int _Tmp9=Cyc_List_length(ps);int _TmpA=Cyc_List_length(tqts);unsigned _TmpB=p->loc;({(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;})(_Tmp8,_Tmp9,_TmpA,Cyc_Tcpat_wild_pat,_TmpB,({const char*_TmpC="tuple";_tag_fat(_TmpC,sizeof(char),6U);}));});*ps_p=_Tmp7;});
# 363
ps=*ps_p;}
# 365
goto _LL4C;}}else{
goto _LL4C;}_LL4C:;}else{
# 368
if(dots)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="cannot determine missing fields for ... in tuple pattern";_tag_fat(_Tmp7,sizeof(char),57U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),1));});}
{int i=0;for(0;ps!=0;(ps=ps->tl,i ++)){
void**inner_topt=0;
if(topt_ts!=0){
inner_topt=&(*((struct _tuple19*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 376
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=({struct Cyc_Absyn_Exp*_Tmp5=access_exp;Cyc_Absyn_subscript_exp(_Tmp5,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _Tmp6;_Tmp6.Int_c.tag=5U,_Tmp6.Int_c.val.f1=1U,_Tmp6.Int_c.val.f2=i;_Tmp6;}),0U),0U);});
# 381
res=({struct Cyc_Tcpat_TcPatResult _Tmp5=res;Cyc_Tcpat_combine_results(_Tmp5,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)ps->hd,inner_topt,rgn_pat,allow_ref_pat,new_access_exp));});
# 384
if((unsigned)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)ps->hd)->topt;
pat_ts=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple19*_Tmp6=({struct _tuple19*_Tmp7=_cycalloc(sizeof(struct _tuple19));({struct Cyc_Absyn_Tqual _Tmp8=Cyc_Absyn_empty_tqual(0U);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=_check_null(((struct Cyc_Absyn_Pat*)ps->hd)->topt);_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=pat_ts;_Tmp5;});}}}
# 387
t=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Absyn_TupleType_Absyn_Type_struct));_Tmp5->tag=6,({struct Cyc_List_List*_Tmp6=Cyc_List_imp_rev(pat_ts);_Tmp5->f1=_Tmp6;});_Tmp5;});
goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1!=0){if(((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1)->KnownAggr.tag==2){_Tmp4=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1->KnownAggr.val;_Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;struct Cyc_List_List*exist_ts=_Tmp3;struct Cyc_List_List**dps_p=_Tmp1;int dots=_Tmp2;
# 391
struct Cyc_List_List*dps=*dps_p;
if(ad->impl==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="can't destructure abstract ";_tag_fat(_Tmp7,sizeof(char),28U);});_Tmp6;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _Tmp7;_Tmp7.tag=6,_Tmp7.f1=ad;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),2));});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL0;}
# 399
if((int)ad->kind==1 && _check_null(ad->impl)->tagged)
allow_ref_pat=0;
if(exist_ts!=0){
# 405
if(topt==0 || Cyc_Tcutil_type_kind(*topt)!=& Cyc_Kinds_ak)
allow_ref_pat=0;}
# 408
{struct _RegionHandle _Tmp5=_new_region("rgn");struct _RegionHandle*rgn=& _Tmp5;_push_region(rgn);
# 410
{struct Cyc_List_List*var_tvs=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*u=_check_null(ad->impl)->exist_vars;
{struct Cyc_List_List*t=exist_ts;for(0;t!=0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)_check_null(u)->hd;
u=u->tl;{
void*kb1=Cyc_Kinds_compress_kb(tv->kind);
void*kb2=Cyc_Kinds_compress_kb(uv->kind);
struct Cyc_Absyn_Kind*k2;
{void*_Tmp6;switch(*((int*)kb2)){case 2: _Tmp6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb2)->f2;{struct Cyc_Absyn_Kind*k=_Tmp6;
_Tmp6=k;goto _LL55;}case 0: _Tmp6=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)kb2)->f1;_LL55: {struct Cyc_Absyn_Kind*k=_Tmp6;
k2=k;goto _LL51;}default:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="unconstrained existential tyvar in aggregate";_tag_fat(_Tmp9,sizeof(char),45U);});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp8,sizeof(void*),1));});}_LL51:;}
# 425
{void*_Tmp6;void*_Tmp7;switch(*((int*)kb1)){case 0: _Tmp7=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)kb1)->f1;{struct Cyc_Absyn_Kind*k1=_Tmp7;
# 428
if(!Cyc_Kinds_kind_leq(k2,k1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="type variable ";_tag_fat(_TmpA,sizeof(char),15U);});_Tmp9;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Tvar_Warn_Warg_struct _TmpA;_TmpA.tag=7,_TmpA.f1=tv;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC=" has kind ";_tag_fat(_TmpC,sizeof(char),11U);});_TmpB;});struct Cyc_Warn_KindBound_Warn_Warg_struct _TmpB=({struct Cyc_Warn_KindBound_Warn_Warg_struct _TmpC;_TmpC.tag=8,_TmpC.f1=(void*)kb1;_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE=" but must have at least kind ";_tag_fat(_TmpE,sizeof(char),30U);});_TmpD;});struct Cyc_Warn_Kind_Warn_Warg_struct _TmpD=({struct Cyc_Warn_Kind_Warn_Warg_struct _TmpE;_TmpE.tag=9,_TmpE.f1=k2;_TmpE;});void*_TmpE[6];_TmpE[0]=& _Tmp8,_TmpE[1]=& _Tmp9,_TmpE[2]=& _TmpA,_TmpE[3]=& _TmpB,_TmpE[4]=& _TmpC,_TmpE[5]=& _TmpD;Cyc_Warn_err2(p->loc,_tag_fat(_TmpE,sizeof(void*),6));});
# 431
goto _LL58;}case 2: _Tmp7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb1)->f1;_Tmp6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)kb1)->f2;{struct Cyc_Core_Opt**f=_Tmp7;struct Cyc_Absyn_Kind*k=_Tmp6;
_Tmp7=f;goto _LL5E;}default: _Tmp7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)kb1)->f1;_LL5E: {struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp7;
({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=kb2;_Tmp9;});*f=_Tmp8;});goto _LL58;}}_LL58:;}{
# 435
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp6->tag=2,_Tmp6->f1=tv;_Tmp6;});
var_tvs=({struct Cyc_List_List*_Tmp6=_region_malloc(rgn,sizeof(struct Cyc_List_List));_Tmp6->hd=vartype,_Tmp6->tl=var_tvs;_Tmp6;});
# 439
if((int)k2->kind==3){
if((int)k2->aliasqual!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="opened existential had unique or top region kind";_tag_fat(_Tmp8,sizeof(char),49U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp7,sizeof(void*),1));});
outlives_constraints=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));
({struct _tuple0*_Tmp7=({struct _tuple0*_Tmp8=_cycalloc(sizeof(struct _tuple0));_Tmp8->f1=Cyc_Absyn_empty_effect,_Tmp8->f2=vartype;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=outlives_constraints;_Tmp6;});}}}}}
# 447
var_tvs=Cyc_List_imp_rev(var_tvs);{
# 449
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,exist_ts);
# 451
struct _tuple13 env=({struct _tuple13 _Tmp6;({struct Cyc_List_List*_Tmp7=Cyc_Tcenv_lookup_type_vars(te2);_Tmp6.f1=_Tmp7;}),_Tmp6.f2=rgn;_Tmp6;});
struct Cyc_List_List*all_inst=({(struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_rmap_c;})(rgn,Cyc_Tcutil_r_make_inst_var,& env,ad->tvs);
struct Cyc_List_List*exist_inst=Cyc_List_rzip(rgn,rgn,_check_null(ad->impl)->exist_vars,var_tvs);
struct Cyc_List_List*all_typs=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),all_inst);
struct Cyc_List_List*exist_typs=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),exist_inst);
struct Cyc_List_List*inst=Cyc_List_rappend(rgn,all_inst,exist_inst);
# 458
if(exist_ts!=0 || _check_null(ad->impl)->rgn_po!=0){
if(res.tvars_and_bounds_opt==0)
({struct _tuple1*_Tmp6=({struct _tuple1*_Tmp7=_cycalloc(sizeof(struct _tuple1));_Tmp7->f1=0,_Tmp7->f2=0;_Tmp7;});res.tvars_and_bounds_opt=_Tmp6;});
({struct Cyc_List_List*_Tmp6=({
struct Cyc_List_List*_Tmp7=(*res.tvars_and_bounds_opt).f1;Cyc_List_append(_Tmp7,({(struct Cyc_List_List*(*)(struct _tuple18*(*)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_add_false,exist_ts));});
# 461
(*res.tvars_and_bounds_opt).f1=_Tmp6;});
# 463
({struct Cyc_List_List*_Tmp6=
Cyc_List_append((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 463
(*res.tvars_and_bounds_opt).f2=_Tmp6;});{
# 465
struct Cyc_List_List*rpo_inst=0;
{struct Cyc_List_List*rpo=_check_null(ad->impl)->rgn_po;for(0;rpo!=0;rpo=rpo->tl){
rpo_inst=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp7=({struct _tuple0*_Tmp8=_cycalloc(sizeof(struct _tuple0));({void*_Tmp9=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f1);_Tmp8->f1=_Tmp9;}),({
void*_Tmp9=Cyc_Tcutil_rsubstitute(rgn,inst,(*((struct _tuple0*)rpo->hd)).f2);_Tmp8->f2=_Tmp9;});_Tmp8;});
# 467
_Tmp6->hd=_Tmp7;}),_Tmp6->tl=rpo_inst;_Tmp6;});}}
# 470
rpo_inst=Cyc_List_imp_rev(rpo_inst);
({struct Cyc_List_List*_Tmp6=
Cyc_List_append((*res.tvars_and_bounds_opt).f2,rpo_inst);
# 471
(*res.tvars_and_bounds_opt).f2=_Tmp6;});}}
# 476
t=({union Cyc_Absyn_AggrInfo _Tmp6=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_Tmp7=ad;_Tmp7;}));Cyc_Absyn_aggr_type(_Tmp6,all_typs);});
if(dots &&(int)ad->kind==1)
Cyc_Warn_warn(p->loc,({const char*_Tmp6="`...' pattern not allowed in union pattern";_tag_fat(_Tmp6,sizeof(char),43U);}),_tag_fat(0U,sizeof(void*),0));else{
if(dots){
({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=dps;int _Tmp8=Cyc_List_length(dps);int _Tmp9=Cyc_List_length(_check_null(ad->impl)->fields);unsigned _TmpA=p->loc;({(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct _tuple16*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;})(_Tmp7,_Tmp8,_Tmp9,Cyc_Tcpat_wild_dlp,_TmpA,({const char*_TmpB="struct";_tag_fat(_TmpB,sizeof(char),7U);}));});*dps_p=_Tmp6;});
# 482
dps=*dps_p;}}{
# 484
struct Cyc_List_List*fields=
Cyc_Tcutil_resolve_aggregate_designators(rgn,p->loc,dps,ad->kind,_check_null(ad->impl)->fields);
for(1;fields!=0;fields=fields->tl){
struct _tuple20*_Tmp6=(struct _tuple20*)fields->hd;void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6->f1;_Tmp7=_Tmp6->f2;{struct Cyc_Absyn_Aggrfield*field=_Tmp8;struct Cyc_Absyn_Pat*pat=_Tmp7;
void*inst_fieldtyp=Cyc_Tcutil_rsubstitute(rgn,inst,field->type);
# 490
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,field->name,0U);
res=({struct Cyc_Tcpat_TcPatResult _Tmp9=res;Cyc_Tcpat_combine_results(_Tmp9,Cyc_Tcpat_tcPatRec(te2,pat,& inst_fieldtyp,rgn_pat,allow_ref_pat,new_access_exp));});
# 498
if(!Cyc_Unify_unify(_check_null(pat->topt),inst_fieldtyp))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="field ";_tag_fat(_TmpB,sizeof(char),7U);});_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=*field->name;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD=" of ";_tag_fat(_TmpD,sizeof(char),5U);});_TmpC;});struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Aggrdecl_Warn_Warg_struct _TmpD;_TmpD.tag=6,_TmpD.f1=ad;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF=" pattern expects type ";_tag_fat(_TmpF,sizeof(char),23U);});_TmpE;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpF;_TmpF.tag=2,_TmpF.f1=(void*)inst_fieldtyp;_TmpF;});struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=({const char*_Tmp11=" != ";_tag_fat(_Tmp11,sizeof(char),5U);});_Tmp10;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp11;_Tmp11.tag=2,_Tmp11.f1=(void*)
# 501
_check_null(pat->topt);_Tmp11;});void*_Tmp11[8];_Tmp11[0]=& _Tmp9,_Tmp11[1]=& _TmpA,_Tmp11[2]=& _TmpB,_Tmp11[3]=& _TmpC,_Tmp11[4]=& _TmpD,_Tmp11[5]=& _TmpE,_Tmp11[6]=& _TmpF,_Tmp11[7]=& _Tmp10;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp11,sizeof(void*),8));});
if((unsigned)new_access_exp)new_access_exp->topt=pat->topt;}}}}}
# 410
;_pop_region();}
# 505
goto _LL0;}}else{_LL24:
# 545
 goto _LL26;}}else{
# 544
goto _LL24;}case 8: _Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;struct Cyc_List_List**ps_p=(struct Cyc_List_List**)_Tmp1;int dots=_Tmp2;
# 508
struct Cyc_List_List*ps=*ps_p;
struct Cyc_List_List*tqts=tuf->typs;
# 511
struct _tuple13 env=({struct _tuple13 _Tmp5;({struct Cyc_List_List*_Tmp6=Cyc_Tcenv_lookup_type_vars(te);_Tmp5.f1=_Tmp6;}),_Tmp5.f2=Cyc_Core_heap_region;_Tmp5;});
struct Cyc_List_List*inst=({(struct Cyc_List_List*(*)(struct _tuple14*(*)(struct _tuple13*,struct Cyc_Absyn_Tvar*),struct _tuple13*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_r_make_inst_var,& env,tud->tvs);
struct Cyc_List_List*all_typs=({(struct Cyc_List_List*(*)(void*(*)(struct _tuple14*),struct Cyc_List_List*))Cyc_List_map;})(({(void*(*)(struct _tuple14*))Cyc_Core_snd;}),inst);
t=({union Cyc_Absyn_DatatypeFieldInfo _Tmp5=Cyc_Absyn_KnownDatatypefield(tud,tuf);Cyc_Absyn_datatype_field_type(_Tmp5,all_typs);});
if(dots){
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=ps;int _Tmp7=Cyc_List_length(ps);int _Tmp8=Cyc_List_length(tqts);unsigned _Tmp9=p->loc;({(struct Cyc_List_List*(*)(struct Cyc_List_List*,int,int,struct Cyc_Absyn_Pat*(*)(unsigned),unsigned,struct _fat_ptr))Cyc_Tcpat_remove_dots;})(_Tmp6,_Tmp7,_Tmp8,Cyc_Tcpat_wild_pat,_Tmp9,({const char*_TmpA="datatype field";_tag_fat(_TmpA,sizeof(char),15U);}));});*ps_p=_Tmp5;});
# 518
ps=*ps_p;}
# 520
for(1;ps!=0 && tqts!=0;(ps=ps->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*p2=(struct Cyc_Absyn_Pat*)ps->hd;
# 524
void*field_typ=Cyc_Tcutil_substitute(inst,(*((struct _tuple19*)tqts->hd)).f2);
# 527
if((unsigned)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_type);
res=({struct Cyc_Tcpat_TcPatResult _Tmp5=res;Cyc_Tcpat_combine_results(_Tmp5,Cyc_Tcpat_tcPatRec(te,p2,& field_typ,rgn_pat,allow_ref_pat,0));});
# 534
if(!Cyc_Unify_unify(_check_null(p2->topt),field_typ))
({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=tuf->name;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=" expects argument type ";_tag_fat(_Tmp8,sizeof(char),24U);});_Tmp7;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8;_Tmp8.tag=2,_Tmp8.f1=(void*)field_typ;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA=" not ";_tag_fat(_TmpA,sizeof(char),6U);});_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)
_check_null(p2->topt);_TmpA;});void*_TmpA[5];_TmpA[0]=& _Tmp5,_TmpA[1]=& _Tmp6,_TmpA[2]=& _Tmp7,_TmpA[3]=& _Tmp8,_TmpA[4]=& _Tmp9;Cyc_Warn_err2(p2->loc,_tag_fat(_TmpA,sizeof(void*),5));});}
# 538
if(ps!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="too many arguments for datatype constructor ";_tag_fat(_Tmp7,sizeof(char),45U);});_Tmp6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=tuf->name;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),2));});
if(tqts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="too few arguments for datatype constructor ";_tag_fat(_Tmp7,sizeof(char),44U);});_Tmp6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=tuf->name;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp7,sizeof(void*),2));});
goto _LL0;}case 15: _LL26:
# 546
 goto _LL28;case 17: _LL28:
 goto _LL2A;default: _LL2A:
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL0;}_LL0:;}
# 550
tcpat_end:
 p->topt=t;
return res;}}
# 555
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 557
struct Cyc_Tcpat_TcPatResult ans=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 559
struct _tuple1 _Tmp0=Cyc_List_split(ans.patvars);void*_Tmp1;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*vs1=_Tmp1;
struct Cyc_List_List*vs=0;
{struct Cyc_List_List*x=vs1;for(0;x!=0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd!=0)vs=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=*_check_null((struct Cyc_Absyn_Vardecl**)x->hd),_Tmp2->tl=vs;_Tmp2;});}}
({struct Cyc_List_List*_Tmp2=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_get_name,vs);unsigned _Tmp3=p->loc;Cyc_Tcutil_check_unique_vars(_Tmp2,_Tmp3,({const char*_Tmp4="pattern contains a repeated variable";_tag_fat(_Tmp4,sizeof(char),37U);}));});
# 568
{struct Cyc_List_List*x=ans.patvars;for(0;x!=0;x=x->tl){
struct _tuple17*_Tmp2=(struct _tuple17*)x->hd;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=(struct Cyc_Absyn_Exp**)& _Tmp2->f2;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp4;struct Cyc_Absyn_Exp**expopt=(struct Cyc_Absyn_Exp**)_Tmp3;
if(*expopt!=0 &&*expopt!=pat_var_exp)
({struct Cyc_Absyn_Exp*_Tmp5=Cyc_Tcutil_deep_copy_exp(1,_check_null(*expopt));*expopt=_Tmp5;});}}}
# 573
return ans;}}
# 579
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 582
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(old_type);_Tmp1.f1=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(new_type);_Tmp1.f2=_Tmp2;});_Tmp1;});struct Cyc_Absyn_PtrInfo _Tmp1;struct Cyc_Absyn_PtrInfo _Tmp2;if(*((int*)_Tmp0.f1)==3){if(*((int*)_Tmp0.f2)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_PtrInfo pold=_Tmp2;struct Cyc_Absyn_PtrInfo pnew=_Tmp1;
# 584
struct Cyc_Absyn_PointerType_Absyn_Type_struct*ptry;ptry=_cycalloc(sizeof(struct Cyc_Absyn_PointerType_Absyn_Type_struct)),ptry->tag=3,ptry->f1.elt_type=pold.elt_type,ptry->f1.elt_tq=pnew.elt_tq,
ptry->f1.ptr_atts.rgn=pold.ptr_atts.rgn,ptry->f1.ptr_atts.nullable=pnew.ptr_atts.nullable,ptry->f1.ptr_atts.bounds=pnew.ptr_atts.bounds,ptry->f1.ptr_atts.zero_term=pnew.ptr_atts.zero_term,ptry->f1.ptr_atts.ptrloc=pold.ptr_atts.ptrloc,ptry->f1.ptr_atts.autoreleased=pnew.ptr_atts.autoreleased;
# 591
return({struct Cyc_RgnOrder_RgnPO*_Tmp3=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_Tmp4=assump;void*_Tmp5=(void*)ptry;Cyc_Tcutil_subtype(_Tmp3,_Tmp4,_Tmp5,new_type);})&&({
struct Cyc_RgnOrder_RgnPO*_Tmp3=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_Absyn_Exp*_Tmp4=initializer;Cyc_Tcutil_coerce_assign(_Tmp3,_Tmp4,(void*)ptry);});}}else{goto _LL3;}}else{_LL3:
 return 0;};}
# 600
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 603
struct Cyc_List_List*assump;assump=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple0*_Tmp0=({struct _tuple0*_Tmp1=_cycalloc(sizeof(struct _tuple0));_Tmp1->f1=Cyc_Absyn_unique_rgn_type,({void*_Tmp2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp3->tag=2,_Tmp3->f1=tv;_Tmp3;});_Tmp1->f2=_Tmp2;});_Tmp1;});assump->hd=_Tmp0;}),assump->tl=0;
if(({struct Cyc_RgnOrder_RgnPO*_Tmp0=Cyc_Tcenv_curr_rgnpo(tenv);struct Cyc_List_List*_Tmp1=assump;void*_Tmp2=old_type;Cyc_Tcutil_subtype(_Tmp0,_Tmp1,_Tmp2,new_type);})){
# 621 "tcpat.cyc"
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;({void*_Tmp2=Cyc_Absyn_compress(old_type);_Tmp1.f1=_Tmp2;}),({void*_Tmp2=Cyc_Absyn_compress(new_type);_Tmp1.f2=_Tmp2;});_Tmp1;});struct Cyc_Absyn_PtrInfo _Tmp1;struct Cyc_Absyn_PtrInfo _Tmp2;if(*((int*)_Tmp0.f1)==3){if(*((int*)_Tmp0.f2)==3){_Tmp2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_PtrInfo pold=_Tmp2;struct Cyc_Absyn_PtrInfo pnew=_Tmp1;
goto _LL0;}}else{goto _LL3;}}else{_LL3:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="alias requires pointer type";_tag_fat(_Tmp5,sizeof(char),28U);});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),1));});goto _LL0;}_LL0:;}else{
# 626
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="cannot alias value of type ";_tag_fat(_Tmp2,sizeof(char),28U);});_Tmp1;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp2;_Tmp2.tag=2,_Tmp2.f1=(void*)old_type;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=" to type ";_tag_fat(_Tmp4,sizeof(char),10U);});_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)new_type;_Tmp4;});void*_Tmp4[4];_Tmp4[0]=& _Tmp0,_Tmp4[1]=& _Tmp1,_Tmp4[2]=& _Tmp2,_Tmp4[3]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),4));});}}
# 632
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 634
void*_Tmp0=p->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 6: _Tmp3=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*p2=_Tmp3;
# 636
void*_Tmp4=_check_null(p->topt);void*_Tmp5;if(*((int*)_Tmp4)==3){_Tmp5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp4)->f1.ptr_atts.rgn;{void*rt=_Tmp5;
# 638
Cyc_Tcenv_check_rgn_accessible(te,p->loc,rt);
({struct Cyc_Tcenv_Tenv*_Tmp6=te;struct Cyc_Absyn_Pat*_Tmp7=p2;int _Tmp8=Cyc_Tcutil_is_noalias_region(rt,0);Cyc_Tcpat_check_pat_regions_rec(_Tmp6,_Tmp7,_Tmp8,patvars);});
return;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="check_pat_regions: bad pointer type";_tag_fat(_Tmp8,sizeof(char),36U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp7,sizeof(void*),1));});};}case 7: _Tmp3=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{union Cyc_Absyn_AggrInfo*ai=_Tmp3;struct Cyc_List_List*exist_ts=_Tmp2;struct Cyc_List_List*dps=_Tmp1;
# 644
for(1;dps!=0;dps=dps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple16*)dps->hd)).f2,did_noalias_deref,patvars);}
return;}case 8: _Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_List_List*ps=_Tmp3;
# 648
did_noalias_deref=0;_Tmp3=ps;goto _LL8;}case 5: _Tmp3=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_LL8: {struct Cyc_List_List*ps=_Tmp3;
# 650
for(1;ps!=0;ps=ps->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)ps->hd,did_noalias_deref,patvars);}
return;}case 3: _Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 654
{struct Cyc_List_List*x=patvars;for(0;x!=0;x=x->tl){
struct _tuple17*_Tmp4=(struct _tuple17*)x->hd;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4->f1;_Tmp5=_Tmp4->f2;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp6;struct Cyc_Absyn_Exp*eopt=_Tmp5;
# 660
if((vdopt!=0 &&*vdopt==vd)&& eopt!=0){
{void*_Tmp7=eopt->r;void*_Tmp8;if(*((int*)_Tmp7)==15){_Tmp8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp7)->f1;{struct Cyc_Absyn_Exp*e=_Tmp8;
# 663
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_check_null(e->topt)))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="reference pattern not allowed on alias-free pointers";_tag_fat(_TmpB,sizeof(char),53U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_Warn_err2(p->loc,_tag_fat(_TmpA,sizeof(void*),1));});
goto _LL19;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="check_pat_regions: bad reference access exp";_tag_fat(_TmpB,sizeof(char),44U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpA,sizeof(void*),1));});}_LL19:;}
# 668
break;}}}}
# 671
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 1: _Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;struct Cyc_Absyn_Pat*p2=_Tmp2;
# 674
{void*_Tmp4=p->topt;if(_Tmp4!=0){if(*((int*)_Tmp4)==4){
# 676
if(!did_noalias_deref)
goto _LL1E;
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7="pattern to array would create alias of no-alias pointer";_tag_fat(_Tmp7,sizeof(char),56U);});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),1));});
return;}else{goto _LL21;}}else{_LL21:
 goto _LL1E;}_LL1E:;}
# 682
Cyc_Tcpat_check_pat_regions_rec(te,p2,did_noalias_deref,patvars);
return;}case 2: _Tmp3=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp3;struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 685
{struct Cyc_List_List*x=patvars;for(0;x!=0;x=x->tl){
struct _tuple17*_Tmp4=(struct _tuple17*)x->hd;void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4->f1;_Tmp5=_Tmp4->f2;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp6;struct Cyc_Absyn_Exp*eopt=_Tmp5;
# 689
if(vdopt!=0 &&*vdopt==vd){
if(eopt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="cannot alias pattern expression in datatype";_tag_fat(_Tmp9,sizeof(char),44U);});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp8,sizeof(void*),1));});else{
# 693
struct Cyc_Tcenv_Tenv*te2=({unsigned _Tmp7=p->loc;struct Cyc_Tcenv_Tenv*_Tmp8=te;Cyc_Tcenv_add_type_vars(_Tmp7,_Tmp8,({struct Cyc_Absyn_Tvar*_Tmp9[1];_Tmp9[0]=tv;Cyc_List_list(_tag_fat(_Tmp9,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
te2=({struct Cyc_Tcenv_Tenv*_Tmp7=te2;Cyc_Tcenv_add_region(_Tmp7,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp8->tag=2,_Tmp8->f1=tv;_Tmp8;}),0,0);});
# 696
Cyc_Tcpat_check_alias_coercion(te2,p->loc,_check_null(eopt->topt),tv,vd->type,eopt);}
# 698
break;}}}}
# 701
goto _LL0;}default:
 return;}_LL0:;}
# 717 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 719
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x!=0;x=x->tl){
struct _tuple17*_Tmp0=(struct _tuple17*)x->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{struct Cyc_Absyn_Vardecl**vdopt=_Tmp2;struct Cyc_Absyn_Exp*eopt=_Tmp1;
if(eopt!=0){
struct Cyc_Absyn_Exp*e=eopt;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_check_null(e->topt))&& !Cyc_Tcutil_is_noalias_path(e)){
# 726
if(vdopt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="pattern dereferences an alias-free-pointer ";_tag_fat(_Tmp5,sizeof(char),44U);});_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="from a non-unique path";_tag_fat(_Tmp6,sizeof(char),23U);});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp5,sizeof(void*),2));});else{
# 730
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="pattern for variable ";_tag_fat(_Tmp5,sizeof(char),22U);});_Tmp4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(*vdopt)->name;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7=" dereferences an alias-free-pointer from a non-unique path";_tag_fat(_Tmp7,sizeof(char),59U);});_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp6,sizeof(void*),3));});}}}}}}}
# 777 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 787
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};struct _union_Name_value_Name_v{int tag;struct _fat_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 801
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _fat_ptr s){return({union Cyc_Tcpat_Name_value _Tmp0;_Tmp0.Name_v.tag=1U,_Tmp0.Name_v.val=s;_Tmp0;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _Tmp0;_Tmp0.Int_v.tag=2U,_Tmp0.Int_v.val=i;_Tmp0;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 818
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _Tmp0=c1->name;int _Tmp1;struct _fat_ptr _Tmp2;if(_Tmp0.Name_v.tag==1){_Tmp2=_Tmp0.Name_v.val;{struct _fat_ptr n1=_Tmp2;
# 821
union Cyc_Tcpat_Name_value _Tmp3=c2->name;struct _fat_ptr _Tmp4;if(_Tmp3.Name_v.tag==1){_Tmp4=_Tmp3.Name_v.val;{struct _fat_ptr n2=_Tmp4;
return Cyc_strcmp(n1,n2);}}else{
return -1;};}}else{_Tmp1=_Tmp0.Int_v.val;{int i1=_Tmp1;
# 826
union Cyc_Tcpat_Name_value _Tmp3=c2->name;int _Tmp4;if(_Tmp3.Name_v.tag==1)
return 1;else{_Tmp4=_Tmp3.Int_v.val;{int i2=_Tmp4;
return i1 - i2;}};}};}
# 834
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set (void){
return({(struct Cyc_Set_Set*(*)(struct _RegionHandle*,int(*)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty;})(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 838
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 842
static unsigned Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned ans=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)_check_null(td->fields)->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)_check_null(fs)->hd)->name)!=0){
++ ans;
fs=fs->tl;}
# 849
return ans;}
# 852
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _fat_ptr*f){
int i=1;
{struct Cyc_List_List*fields=_check_null(ad->impl)->fields;for(0;fields!=0;(fields=fields->tl,++ i)){
if(Cyc_strcmp(*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,*f)==0)return i;}}
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="get_member_offset ";_tag_fat(_Tmp2,sizeof(char),19U);});_Tmp1;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=*f;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=" failed";_tag_fat(_Tmp4,sizeof(char),8U);});_Tmp3;});void*_Tmp3[3];_Tmp3[0]=& _Tmp0,_Tmp3[1]=& _Tmp1,_Tmp3[2]=& _Tmp2;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp3,sizeof(void*),3));});}
# 859
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
void*_Tmp0;if(pw.where_clause.tag==2){_Tmp0=pw.where_clause.val;{struct Cyc_Absyn_Exp*e=_Tmp0;
return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct));_Tmp1->tag=0,_Tmp1->f1=e;_Tmp1;});}}else{_Tmp0=pw.pattern.val;{struct Cyc_Absyn_Pat*p=_Tmp0;
# 863
void*_Tmp1=p->r;union Cyc_Absyn_AggrInfo _Tmp2;void*_Tmp3;struct _fat_ptr _Tmp4;char _Tmp5;int _Tmp6;enum Cyc_Absyn_Sign _Tmp7;void*_Tmp8;switch(*((int*)_Tmp1)){case 1: _Tmp8=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct Cyc_Absyn_Pat*p1=_Tmp8;
_Tmp8=p1;goto _LL9;}case 3: _Tmp8=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;_LL9: {struct Cyc_Absyn_Pat*p1=_Tmp8;
return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _Tmp9;_Tmp9.pattern.tag=1U,_Tmp9.pattern.val=p1;_Tmp9;}));}case 9:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{enum Cyc_Absyn_Sign s=_Tmp7;int i=_Tmp6;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct));_Tmp9->tag=6,_Tmp9->f1=(unsigned)i;_Tmp9;});}case 11: _Tmp5=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;{char c=_Tmp5;
return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct));_Tmp9->tag=6,_Tmp9->f1=(unsigned)c;_Tmp9;});}case 12: _Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp6=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct _fat_ptr f=_Tmp4;int i=_Tmp6;
return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct));_Tmp9->tag=5,_Tmp9->f1=f,_Tmp9->f2=i;_Tmp9;});}case 13: _Tmp8=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp3=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct Cyc_Absyn_Enumdecl*ed=_Tmp8;struct Cyc_Absyn_Enumfield*ef=_Tmp3;
return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct));_Tmp9->tag=3,_Tmp9->f1=ed,_Tmp9->f2=ef;_Tmp9;});}case 14: _Tmp8=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp3=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{void*t=_Tmp8;struct Cyc_Absyn_Enumfield*ef=_Tmp3;
return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct));_Tmp9->tag=4,_Tmp9->f1=t,_Tmp9->f2=ef;_Tmp9;});}case 6:
# 873
 if(!Cyc_Tcutil_is_pointer_type(_check_null(p->topt))|| !
Cyc_Tcutil_is_nullable_pointer_type(_check_null(p->topt),0))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="non-null pointer type or non-pointer type in pointer pattern";_tag_fat(_TmpB,sizeof(char),61U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpA,sizeof(void*),1));});
return(void*)& Cyc_Tcpat_NeqNull_val;case 8: _Tmp8=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct Cyc_Absyn_Datatypedecl*ddecl=_Tmp8;struct Cyc_Absyn_Datatypefield*df=_Tmp3;
# 878
if(ddecl->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct));_Tmp9->tag=9,_Tmp9->f1=ddecl,_Tmp9->f2=df;_Tmp9;});
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct));_Tmp9->tag=7,({int _TmpA=(int)Cyc_Tcpat_datatype_tag_number(ddecl,df->name);_Tmp9->f1=_TmpA;}),_Tmp9->f2=ddecl,_Tmp9->f3=df;_Tmp9;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f1!=0){_Tmp2=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f3;{union Cyc_Absyn_AggrInfo info=_Tmp2;struct Cyc_List_List*dlps=_Tmp8;
# 882
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(!_check_null(ad->impl)->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="non-tagged aggregate in pattern test";_tag_fat(_TmpB,sizeof(char),37U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpA,sizeof(void*),1));});{
void*_Tmp9=(void*)_check_null((*((struct _tuple16*)_check_null(dlps)->hd)).f1)->hd;void*_TmpA;if(*((int*)_Tmp9)==1){_TmpA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp9)->f1;{struct _fat_ptr*f=_TmpA;
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_TmpB=_cycalloc(sizeof(struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct));_TmpB->tag=8,_TmpB->f1=f,({int _TmpC=Cyc_Tcpat_get_member_offset(ad,f);_TmpB->f2=_TmpC;});_TmpB;});}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=({const char*_TmpD="no field name in tagged union pattern";_tag_fat(_TmpD,sizeof(char),38U);});_TmpC;});void*_TmpC[1];_TmpC[0]=& _TmpB;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpC,sizeof(void*),1));});};}}}else{goto _LL1C;}default: _LL1C:
# 889
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="non-test pattern in pattern test";_tag_fat(_TmpB,sizeof(char),33U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpA,sizeof(void*),1));});};}};}
# 894
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _Tmp0;_Tmp0.pattern.tag=1U,_Tmp0.pattern.val=p;_Tmp0;});}
# 898
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(({const char*_Tmp2="NULL";_tag_fat(_Tmp2,sizeof(char),5U);}));_Tmp0->name=_Tmp1;}),_Tmp0->arity=0,_Tmp0->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});_Tmp0;});}
# 901
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(({const char*_Tmp2="&";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0->name=_Tmp1;}),_Tmp0->arity=1,_Tmp0->span=& Cyc_Tcpat_two_opt,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});_Tmp0;});}
# 904
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(({const char*_Tmp2="&";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0->name=_Tmp1;}),_Tmp0->arity=1,_Tmp0->span=& Cyc_Tcpat_one_opt,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});_Tmp0;});}
# 907
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Int_v(i);_Tmp0->name=_Tmp1;}),_Tmp0->arity=0,_Tmp0->span=0,_Tmp0->orig_pat=p;_Tmp0;});}
# 910
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(f);_Tmp0->name=_Tmp1;}),_Tmp0->arity=0,_Tmp0->span=0,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});_Tmp0;});}
# 913
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Int_v((int)c);_Tmp0->name=_Tmp1;}),_Tmp0->arity=0,_Tmp0->span=& Cyc_Tcpat_twofiftysix_opt,({union Cyc_Tcpat_PatOrWhere _Tmp1=Cyc_Tcpat_pw(p);_Tmp0->orig_pat=_Tmp1;});_Tmp0;});}
# 916
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp1=Cyc_Tcpat_Name_v(({const char*_Tmp2="$";_tag_fat(_Tmp2,sizeof(char),2U);}));_Tmp0->name=_Tmp1;}),_Tmp0->arity=i,_Tmp0->span=& Cyc_Tcpat_one_opt,_Tmp0->orig_pat=p;_Tmp0;});}
# 921
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_null_con(p);_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 924
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_int_con(i,p);_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 927
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_char_con(c,p);_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 930
static void*Cyc_Tcpat_float_pat(struct _fat_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_float_con(f,p);_Tmp0->f1=_Tmp1;}),_Tmp0->f2=0;_Tmp0;});}
# 933
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_null_ptr_con(p0);_Tmp0->f1=_Tmp1;}),({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=p,_Tmp2->tl=0;_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 936
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=Cyc_Tcpat_ptr_con(p0);_Tmp0->f1=_Tmp1;}),({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=p,_Tmp2->tl=0;_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 939
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,({struct Cyc_Tcpat_Con_s*_Tmp1=({int _Tmp2=Cyc_List_length(ss);Cyc_Tcpat_tuple_con(_Tmp2,p);});_Tmp0->f1=_Tmp1;}),_Tmp0->f2=ss;_Tmp0;});}
# 942
static void*Cyc_Tcpat_con_pat(struct _fat_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 944
struct Cyc_Tcpat_Con_s*c;c=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s)),({union Cyc_Tcpat_Name_value _Tmp0=Cyc_Tcpat_Name_v(con_name);c->name=_Tmp0;}),({int _Tmp0=Cyc_List_length(ps);c->arity=_Tmp0;}),c->span=span,({union Cyc_Tcpat_PatOrWhere _Tmp0=Cyc_Tcpat_pw(p);c->orig_pat=_Tmp0;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct));_Tmp0->tag=1,_Tmp0->f1=c,_Tmp0->f2=ps;_Tmp0;});}
# 949
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_Tmp0=p->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;struct _fat_ptr _Tmp4;char _Tmp5;int _Tmp6;enum Cyc_Absyn_Sign _Tmp7;switch(*((int*)_Tmp0)){case 0:
 goto _LL4;case 2: _LL4:
 goto _LL6;case 4: _LL6:
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));_Tmp8->tag=0;_Tmp8;});goto _LL0;case 9:
 s=Cyc_Tcpat_null_pat(p);goto _LL0;case 10: _Tmp7=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{enum Cyc_Absyn_Sign sn=_Tmp7;int i=_Tmp6;
s=({int _Tmp8=i;Cyc_Tcpat_int_pat(_Tmp8,Cyc_Tcpat_pw(p));});goto _LL0;}case 11: _Tmp5=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{char c=_Tmp5;
s=Cyc_Tcpat_char_pat(c,p);goto _LL0;}case 12: _Tmp4=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct _fat_ptr f=_Tmp4;
s=Cyc_Tcpat_float_pat(f,p);goto _LL0;}case 1: _Tmp3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Pat*p2=_Tmp3;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 3: _Tmp3=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Pat*p2=_Tmp3;
s=Cyc_Tcpat_compile_pat(p2);goto _LL0;}case 6: _Tmp3=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Pat*pp=_Tmp3;
# 962
{void*_Tmp8=Cyc_Absyn_compress(_check_null(p->topt));void*_Tmp9;if(*((int*)_Tmp8)==3){_Tmp9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp8)->f1.ptr_atts.nullable;{void*n=_Tmp9;
# 964
int is_nullable=Cyc_Tcutil_force_type2bool(0,n);
void*ss=Cyc_Tcpat_compile_pat(pp);
s=is_nullable?Cyc_Tcpat_null_ptr_pat(ss,p): Cyc_Tcpat_ptr_pat(ss,p);
goto _LL27;}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="expecting pointertype for pattern!";_tag_fat(_TmpC,sizeof(char),35U);});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpB,sizeof(void*),1));});}_LL27:;}
# 970
goto _LL0;}case 8: _Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp3;struct Cyc_Absyn_Datatypefield*tuf=_Tmp2;struct Cyc_List_List*ps=_Tmp1;
# 972
int*span;
{void*_Tmp8=Cyc_Absyn_compress(_check_null(p->topt));if(*((int*)_Tmp8)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp8)->f1)){case 18:
# 975
 span=tud->is_extensible?0:({int*_Tmp9=_cycalloc_atomic(sizeof(int));({int _TmpA=Cyc_List_length((struct Cyc_List_List*)_check_null(tud->fields)->v);*_Tmp9=_TmpA;});_Tmp9;});
goto _LL2C;case 19:
 span=& Cyc_Tcpat_one_opt;goto _LL2C;default: goto _LL31;}else{_LL31:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB="void datatype pattern has bad type";_tag_fat(_TmpB,sizeof(char),35U);});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpA,sizeof(void*),1));});}_LL2C:;}
# 980
s=({struct _fat_ptr _Tmp8=*(*tuf->name).f2;int*_Tmp9=span;struct Cyc_List_List*_TmpA=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_con_pat(_Tmp8,_Tmp9,_TmpA,p);});
goto _LL0;}case 5: _Tmp3=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_Tmp0)->f1;{struct Cyc_List_List*ps=_Tmp3;
# 983
s=({struct Cyc_List_List*_Tmp8=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_compile_pat,ps);Cyc_Tcpat_tuple_pat(_Tmp8,Cyc_Tcpat_pw(p));});goto _LL0;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1!=0){if(((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1)->KnownAggr.tag==2){_Tmp3=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f1->KnownAggr.val;_Tmp2=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp3;struct Cyc_List_List*dlps=_Tmp2;
# 988
if((int)ad->kind==0){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=_check_null(ad->impl)->fields;for(0;fields!=0;fields=fields->tl){
# 992
int found=Cyc_strcmp(*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,({const char*_Tmp8="";_tag_fat(_Tmp8,sizeof(char),1U);}))==0;
{struct Cyc_List_List*dlps0=dlps;for(0;!found && dlps0!=0;dlps0=dlps0->tl){
struct _tuple16*_Tmp8=(struct _tuple16*)dlps0->hd;void*_Tmp9;void*_TmpA;_TmpA=_Tmp8->f1;_Tmp9=_Tmp8->f2;{struct Cyc_List_List*dl=_TmpA;struct Cyc_Absyn_Pat*p=_Tmp9;
void*_TmpB;if(dl!=0){if(*((int*)((struct Cyc_List_List*)dl)->hd)==1){if(((struct Cyc_List_List*)dl)->tl==0){_TmpB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)dl->hd)->f1;{struct _fat_ptr*f=_TmpB;
# 997
if(Cyc_strptrcmp(f,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)==0){
ps=({struct Cyc_List_List*_TmpC=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpD=Cyc_Tcpat_compile_pat(p);_TmpC->hd=_TmpD;}),_TmpC->tl=ps;_TmpC;});
found=1;}
# 1001
goto _LL36;}}else{goto _LL39;}}else{goto _LL39;}}else{_LL39:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE="bad designator(s)";_tag_fat(_TmpE,sizeof(char),18U);});_TmpD;});void*_TmpD[1];_TmpD[0]=& _TmpC;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpD,sizeof(void*),1));});}_LL36:;}}}
# 1005
if(!found)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="bad designator";_tag_fat(_TmpA,sizeof(char),15U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp9,sizeof(void*),1));});}}
# 1008
s=({struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(ps);Cyc_Tcpat_tuple_pat(_Tmp8,Cyc_Tcpat_pw(p));});}else{
# 1011
if(!_check_null(ad->impl)->tagged)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="patterns on untagged unions not yet supported.";_tag_fat(_TmpA,sizeof(char),47U);});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(p->loc,_tag_fat(_Tmp9,sizeof(void*),1));});{
int*span;span=_cycalloc_atomic(sizeof(int)),({int _Tmp8=Cyc_List_length(_check_null(ad->impl)->fields);*span=_Tmp8;});{
void*_Tmp8;void*_Tmp9;if(dlps!=0){if(((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f1!=0){if(*((int*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f1)->hd)==1){if(((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f1)->tl==0){if(((struct Cyc_List_List*)dlps)->tl==0){_Tmp9=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct _tuple16*)dlps->hd)->f1->hd)->f1;_Tmp8=((struct _tuple16*)dlps->hd)->f2;{struct _fat_ptr*f=_Tmp9;struct Cyc_Absyn_Pat*p2=_Tmp8;
# 1016
s=({struct _fat_ptr _TmpA=*f;int*_TmpB=span;struct Cyc_List_List*_TmpC=({struct Cyc_List_List*_TmpD=_cycalloc(sizeof(struct Cyc_List_List));({void*_TmpE=Cyc_Tcpat_compile_pat(p2);_TmpD->hd=_TmpE;}),_TmpD->tl=0;_TmpD;});Cyc_Tcpat_con_pat(_TmpA,_TmpB,_TmpC,p);});
goto _LL3B;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{goto _LL3E;}}else{_LL3E:
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="bad union pattern";_tag_fat(_TmpC,sizeof(char),18U);});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpB,sizeof(void*),1));});}_LL3B:;}}}
# 1021
goto _LL0;}}else{goto _LL23;}}else{_LL23: _LL24:
# 1033
 goto _LL26;}case 13: _Tmp3=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp3;
# 1028
_Tmp3=ef;goto _LL1E;}case 14: _Tmp3=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp0)->f2;_LL1E: {struct Cyc_Absyn_Enumfield*ef=_Tmp3;
s=Cyc_Tcpat_con_pat(*(*ef->name).f2,0,0,p);goto _LL0;}case 15:
# 1031
 goto _LL22;case 16: _LL22:
 goto _LL24;default: _LL26:
# 1034
 s=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));_Tmp8->tag=0;_Tmp8;});}_LL0:;}
# 1036
return s;}struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1061
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});int _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;unsigned _Tmp6;switch(*((int*)_Tmp0.f1)){case 0: if(*((int*)_Tmp0.f2)==0)
return 1;else{goto _LLB;}case 1: if(*((int*)_Tmp0.f2)==1)
return 1;else{goto _LLB;}case 2: if(*((int*)_Tmp0.f2)==2){_Tmp6=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_Tmp0.f1)->f1;_Tmp5=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_Tmp0.f2)->f1;{unsigned i1=_Tmp6;unsigned i2=_Tmp5;
return i1==i2;}}else{goto _LLB;}case 3: if(*((int*)_Tmp0.f2)==3){_Tmp4=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0.f1)->f2;_Tmp6=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0.f1)->f3;_Tmp3=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0.f2)->f2;_Tmp5=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_Tmp0.f2)->f3;{struct Cyc_Absyn_Datatypefield*df1=_Tmp4;unsigned i1=_Tmp6;struct Cyc_Absyn_Datatypefield*df2=_Tmp3;unsigned i2=_Tmp5;
# 1067
return df1==df2 && i1==i2;}}else{goto _LLB;}default: if(*((int*)_Tmp0.f2)==4){_Tmp2=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0.f1)->f1;_Tmp4=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0.f1)->f2;_Tmp1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0.f2)->f1;_Tmp3=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_Tmp0.f2)->f2;{int b1=_Tmp2;struct _fat_ptr*f1=_Tmp4;int b2=_Tmp1;struct _fat_ptr*f2=_Tmp3;
# 1069
return b1==b2 && Cyc_strptrcmp(f1,f2)==0;}}else{_LLB:
 return 0;}};}
# 1074
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
for(1;p1!=0 && p2!=0;(p1=p1->tl,p2=p2->tl)){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;}
if(p1!=p2)return 0;
return 1;}
# 1081
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*test=Cyc_Tcpat_get_pat_test(con->orig_pat);
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(*((int*)d2)==2){_Tmp2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d2)->f1;_Tmp1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d2)->f2;_Tmp0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d2)->f3;{struct Cyc_List_List*access2=_Tmp2;struct Cyc_List_List*switch_clauses=_Tmp1;void*default_decision=_Tmp0;
# 1085
if(Cyc_Tcpat_same_path(access,access2)&&(int)(con->orig_pat.pattern.tag==1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct));_Tmp3->tag=2,_Tmp3->f1=access2,({
struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f1=test,_Tmp7->f2=d1;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=switch_clauses;_Tmp5;});_Tmp3->f2=_Tmp4;}),_Tmp3->f3=default_decision;_Tmp3;});
# 1089
goto _LL4;}}else{_LL4:
 return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct));_Tmp3->tag=2,_Tmp3->f1=access,({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp6=({struct _tuple0*_Tmp7=_cycalloc(sizeof(struct _tuple0));_Tmp7->f1=test,_Tmp7->f2=d1;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=0;_Tmp5;});_Tmp3->f2=_Tmp4;}),_Tmp3->f3=d2;_Tmp3;});};}struct _tuple21{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1102
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes =0U,Cyc_Tcpat_No =1U,Cyc_Tcpat_Maybe =2U};
# 1105
static void Cyc_Tcpat_print_tab(int i){
for(1;i!=0;-- i){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0=" ";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}}
# 1109
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _Tmp0=c->name;int _Tmp1;struct _fat_ptr _Tmp2;if(_Tmp0.Name_v.tag==1){_Tmp2=_Tmp0.Name_v.val;{struct _fat_ptr s=_Tmp2;
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="%s";_tag_fat(_Tmp5,sizeof(char),3U);}),_tag_fat(_Tmp4,sizeof(void*),1));});goto _LL0;}}else{_Tmp1=_Tmp0.Int_v.val;{int i=_Tmp1;
({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)i;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="%d";_tag_fat(_Tmp5,sizeof(char),3U);}),_tag_fat(_Tmp4,sizeof(void*),1));});goto _LL0;}}_LL0:;}
# 1116
static void Cyc_Tcpat_print_access(void*a){
int _Tmp0;void*_Tmp1;unsigned _Tmp2;switch(*((int*)a)){case 0:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="DUMMY";_tag_fat(_Tmp3,sizeof(char),6U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 1:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="*";_tag_fat(_Tmp3,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 2: _Tmp2=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)a)->f1;{unsigned i=_Tmp2;
({struct Cyc_Int_pa_PrintArg_struct _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=(unsigned long)((int)i);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="[%d]";_tag_fat(_Tmp5,sizeof(char),5U);}),_tag_fat(_Tmp4,sizeof(void*),1));});goto _LL0;}case 3: _Tmp1=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)a)->f2;_Tmp2=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)a)->f3;{struct Cyc_Absyn_Datatypefield*f=_Tmp1;unsigned i=_Tmp2;
# 1122
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_Absynpp_qvar2string(f->name);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)i);_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%s[%d]";_tag_fat(_Tmp6,sizeof(char),7U);}),_tag_fat(_Tmp5,sizeof(void*),2));});goto _LL0;}default: _Tmp0=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)a)->f1;_Tmp1=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)a)->f2;{int tagged=_Tmp0;struct _fat_ptr*f=_Tmp1;
# 1124
if(tagged)
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3=".tagunion";_tag_fat(_Tmp3,sizeof(char),10U);}),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=*f;_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(Cyc_stderr,({const char*_Tmp5=".%s";_tag_fat(_Tmp5,sizeof(char),4U);}),_tag_fat(_Tmp4,sizeof(void*),1));});
goto _LL0;}}_LL0:;}
# 1131
static void Cyc_Tcpat_print_pat_test(void*p){
int _Tmp0;unsigned _Tmp1;struct _fat_ptr _Tmp2;void*_Tmp3;switch(*((int*)p)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)p)->f1==0){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="where(NULL)";_tag_fat(_Tmp4,sizeof(char),12U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;}else{_Tmp3=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)p)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_exp2string(_check_null(e));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="where(%s)";_tag_fat(_Tmp6,sizeof(char),10U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}}case 1:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="NULL";_tag_fat(_Tmp4,sizeof(char),5U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 2:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="NOT-NULL:";_tag_fat(_Tmp4,sizeof(char),10U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;case 4: _Tmp3=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)p)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp3;
_Tmp3=ef;goto _LLC;}case 3: _Tmp3=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)p)->f2;_LLC: {struct Cyc_Absyn_Enumfield*ef=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_qvar2string(ef->name);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%s";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 5: _Tmp2=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)p)->f1;{struct _fat_ptr s=_Tmp2;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%s";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 6: _Tmp1=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)p)->f1;{unsigned i=_Tmp1;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)i);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="%d";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 7: _Tmp0=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)p)->f1;{int i=_Tmp0;
({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="datatypetag(%d)";_tag_fat(_Tmp6,sizeof(char),16U);}),_tag_fat(_Tmp5,sizeof(void*),1));});goto _LL0;}case 8: _Tmp3=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)p)->f1;_Tmp0=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)p)->f2;{struct _fat_ptr*f=_Tmp3;int i=_Tmp0;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*f;_Tmp5;});struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)i;_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_fprintf(Cyc_stderr,({const char*_Tmp7="uniontag[%s](%d)";_tag_fat(_Tmp7,sizeof(char),17U);}),_tag_fat(_Tmp6,sizeof(void*),2));});goto _LL0;}default: _Tmp3=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)p)->f2;{struct Cyc_Absyn_Datatypefield*f=_Tmp3;
# 1144
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_qvar2string(f->name);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="datatypefield(%s)";_tag_fat(_Tmp6,sizeof(char),18U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}}_LL0:;}
# 1148
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=Cyc_Absynpp_stmt2string(r->rhs);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="%s";_tag_fat(_Tmp2,sizeof(char),3U);}),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 1152
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)d)){case 1: _Tmp2=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)d)->f1;{struct Cyc_Tcpat_Rhs*rhs=_Tmp2;
# 1155
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Success(";_tag_fat(_Tmp3,sizeof(char),9U);}),_tag_fat(0U,sizeof(void*),0));Cyc_Tcpat_print_rhs(rhs);Cyc_fprintf(Cyc_stderr,({const char*_Tmp3=")\n";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}case 0:
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Failure\n";_tag_fat(_Tmp3,sizeof(char),9U);}),_tag_fat(0U,sizeof(void*),0));goto _LL0;default: _Tmp2=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f1;_Tmp1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f2;_Tmp0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f3;{struct Cyc_List_List*a=_Tmp2;struct Cyc_List_List*cases=_Tmp1;void*def=_Tmp0;
# 1160
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Switch[";_tag_fat(_Tmp3,sizeof(char),8U);}),_tag_fat(0U,sizeof(void*),0));
for(1;a!=0;a=a->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)a->hd)->access);
if(a->tl!=0)Cyc_fprintf(Cyc_stderr,({const char*_Tmp3=",";_tag_fat(_Tmp3,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}
# 1166
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="] {\n";_tag_fat(_Tmp3,sizeof(char),5U);}),_tag_fat(0U,sizeof(void*),0));
for(1;cases!=0;cases=cases->tl){
struct _tuple0 _Tmp3=*((struct _tuple0*)cases->hd);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{void*pt=_Tmp5;void*d=_Tmp4;
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="case ";_tag_fat(_Tmp6,sizeof(char),6U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_Tcpat_print_pat_test(pt);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6=":\n";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_Tcpat_print_dec_tree(d,tab + 7);}}
# 1175
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="default:\n";_tag_fat(_Tmp3,sizeof(char),10U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_Tcpat_print_dec_tree(def,tab + 7);
Cyc_Tcpat_print_tab(tab);
Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="}\n";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));}}_LL0:;}
# 1183
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1191
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_Tmp0;if(*((int*)td)==1){_Tmp0=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)td)->f1;{struct Cyc_Set_Set*cs=_Tmp0;
# 1201
return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_Tmp1->tag=1,({struct Cyc_Set_Set*_Tmp2=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_insert;})(cs,c);_Tmp1->f1=_Tmp2;});_Tmp1;});}}else{
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=({const char*_Tmp3="add_neg called when td is Positive";_tag_fat(_Tmp3,sizeof(char),35U);});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp2,sizeof(void*),1));});};}
# 1208
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_Tmp0;if(*((int*)td)==0){_Tmp0=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)td)->f1;{struct Cyc_Tcpat_Con_s*c2=_Tmp0;
# 1212
return Cyc_Tcpat_compare_con(c,c2)==0?0U: 1U;}}else{_Tmp0=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)td)->f1;{struct Cyc_Set_Set*cs=_Tmp0;
# 1215
if(({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;})(cs,c))return 1U;
# 1218
if(c->span!=0 &&({int _Tmp1=*_check_null(c->span);_Tmp1==Cyc_Set_cardinality(cs)+ 1;}))
return 0U;
# 1221
return 2U;}};}struct _tuple22{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1229
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(ctxt==0)
return 0;else{_Tmp2=((struct _tuple22*)ctxt->hd)->f1;_Tmp1=((struct _tuple22*)ctxt->hd)->f2;_Tmp0=ctxt->tl;{struct Cyc_Tcpat_Con_s*c=_Tmp2;struct Cyc_List_List*args=_Tmp1;struct Cyc_List_List*tl=_Tmp0;
return({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple22*_Tmp4=({struct _tuple22*_Tmp5=_cycalloc(sizeof(struct _tuple22));_Tmp5->f1=c,({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=dsc,_Tmp7->tl=args;_Tmp7;});_Tmp5->f2=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=tl;_Tmp3;});}};}
# 1239
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(ctxt==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="norm_context: empty context";_tag_fat(_Tmp5,sizeof(char),28U);});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp4,sizeof(void*),1));});else{_Tmp2=((struct _tuple22*)ctxt->hd)->f1;_Tmp1=((struct _tuple22*)ctxt->hd)->f2;_Tmp0=ctxt->tl;{struct Cyc_Tcpat_Con_s*c=_Tmp2;struct Cyc_List_List*args=_Tmp1;struct Cyc_List_List*tl=_Tmp0;
return({struct Cyc_List_List*_Tmp3=tl;Cyc_Tcpat_augment(_Tmp3,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct));_Tmp4->tag=0,_Tmp4->f1=c,({struct Cyc_List_List*_Tmp5=Cyc_List_rev(args);_Tmp4->f2=_Tmp5;});_Tmp4;}));});}};}
# 1251
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1253
struct _tuple1 _Tmp0=({struct _tuple1 _Tmp1;_Tmp1.f1=ctxt,_Tmp1.f2=work;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;if(_Tmp0.f1==0){if(_Tmp0.f2==0)
return dsc;else{
goto _LL6;}}else{if(_Tmp0.f2==0){_LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8="build_desc: ctxt and work don't match";_tag_fat(_Tmp8,sizeof(char),38U);});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp7,sizeof(void*),1));});}else{_Tmp5=((struct _tuple22*)_Tmp0.f1->hd)->f1;_Tmp4=((struct _tuple22*)_Tmp0.f1->hd)->f2;_Tmp3=_Tmp0.f1->tl;_Tmp2=((struct _tuple21*)_Tmp0.f2->hd)->f3;_Tmp1=_Tmp0.f2->tl;{struct Cyc_Tcpat_Con_s*c=_Tmp5;struct Cyc_List_List*args=_Tmp4;struct Cyc_List_List*rest=_Tmp3;struct Cyc_List_List*dargs=_Tmp2;struct Cyc_List_List*work2=_Tmp1;
# 1258
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*td;td=_cycalloc(sizeof(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct)),td->tag=0,td->f1=c,({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=Cyc_List_rev(args);Cyc_List_append(_Tmp7,({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=dsc,_Tmp8->tl=dargs;_Tmp8;}));});td->f2=_Tmp6;});
return Cyc_Tcpat_build_desc(rest,(void*)td,work2);}}};}
# 1263
static void*Cyc_Tcpat_match(void*,struct Cyc_List_List*,void*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Tcpat_Rhs*,struct Cyc_List_List*);struct _tuple23{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1268
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
void*_Tmp0;void*_Tmp1;void*_Tmp2;if(allmrules==0)
return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct));_Tmp3->tag=0,_Tmp3->f1=dsc;_Tmp3;});else{_Tmp2=((struct _tuple23*)allmrules->hd)->f1;_Tmp1=((struct _tuple23*)allmrules->hd)->f2;_Tmp0=allmrules->tl;{void*pat1=_Tmp2;struct Cyc_Tcpat_Rhs*rhs1=_Tmp1;struct Cyc_List_List*rulerest=_Tmp0;
# 1272
return Cyc_Tcpat_match(pat1,0,dsc,0,0,rhs1,rulerest);}};}
# 1277
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_Tmp0=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_Tmp1->tag=1,({struct Cyc_Set_Set*_Tmp2=Cyc_Tcpat_empty_con_set();_Tmp1->f1=_Tmp2;});_Tmp1;});Cyc_Tcpat_or_match(_Tmp0,allmrules);});}
# 1284
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1287
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;if(work==0)
return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcpat_Success_Tcpat_Decision_struct));_Tmp4->tag=1,_Tmp4->f1=right_hand_side;_Tmp4;});else{if(((struct _tuple21*)((struct Cyc_List_List*)work)->hd)->f1==0){if(((struct _tuple21*)((struct Cyc_List_List*)work)->hd)->f2==0){if(((struct _tuple21*)((struct Cyc_List_List*)work)->hd)->f3==0){_Tmp3=work->tl;{struct Cyc_List_List*workr=_Tmp3;
# 1290
return({struct Cyc_List_List*_Tmp4=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_Tmp5=workr;struct Cyc_Tcpat_Rhs*_Tmp6=right_hand_side;Cyc_Tcpat_and_match(_Tmp4,_Tmp5,_Tmp6,rules);});}}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _Tmp3=((struct _tuple21*)work->hd)->f1;_Tmp2=((struct _tuple21*)work->hd)->f2;_Tmp1=((struct _tuple21*)work->hd)->f3;_Tmp0=work->tl;{struct Cyc_List_List*pats=_Tmp3;struct Cyc_List_List*objs=_Tmp2;struct Cyc_List_List*dscs=_Tmp1;struct Cyc_List_List*workr=_Tmp0;
# 1292
if((pats==0 || objs==0)|| dscs==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="tcpat:and_match: malformed work frame";_tag_fat(_Tmp6,sizeof(char),38U);});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_Tmp5,sizeof(void*),1));});{
struct Cyc_List_List*_Tmp4=pats;void*_Tmp5;void*_Tmp6;_Tmp6=(void*)_Tmp4->hd;_Tmp5=_Tmp4->tl;{void*pat1=_Tmp6;struct Cyc_List_List*patr=_Tmp5;
struct Cyc_List_List*_Tmp7=objs;void*_Tmp8;void*_Tmp9;_Tmp9=(struct Cyc_List_List*)_Tmp7->hd;_Tmp8=_Tmp7->tl;{struct Cyc_List_List*obj1=_Tmp9;struct Cyc_List_List*objr=_Tmp8;
struct Cyc_List_List*_TmpA=dscs;void*_TmpB;void*_TmpC;_TmpC=(void*)_TmpA->hd;_TmpB=_TmpA->tl;{void*dsc1=_TmpC;struct Cyc_List_List*dscr=_TmpB;
struct _tuple21*wf;wf=_cycalloc(sizeof(struct _tuple21)),wf->f1=patr,wf->f2=objr,wf->f3=dscr;
return({void*_TmpD=pat1;struct Cyc_List_List*_TmpE=obj1;void*_TmpF=dsc1;struct Cyc_List_List*_Tmp10=ctx;struct Cyc_List_List*_Tmp11=({struct Cyc_List_List*_Tmp12=_cycalloc(sizeof(struct Cyc_List_List));_Tmp12->hd=wf,_Tmp12->tl=workr;_Tmp12;});struct Cyc_Tcpat_Rhs*_Tmp12=right_hand_side;Cyc_Tcpat_match(_TmpD,_TmpE,_TmpF,_Tmp10,_Tmp11,_Tmp12,rules);});}}}}}}};}
# 1303
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_Tmp0;if(*((int*)dsc)==1){_Tmp0=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)dsc)->f1;{struct Cyc_Set_Set*ncs=_Tmp0;
# 1309
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct));_Tmp1->tag=1,({struct Cyc_Set_Set*_Tmp2=Cyc_Tcpat_empty_con_set();_Tmp1->f1=_Tmp2;});_Tmp1;});
struct Cyc_List_List*res=0;
{int i=0;for(0;i < pcon->arity;++ i){
res=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=any,_Tmp1->tl=res;_Tmp1;});}}
return res;}}else{_Tmp0=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)dsc)->f2;{struct Cyc_List_List*dargs=_Tmp0;
return dargs;}};}
# 1318
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
void*_Tmp0;if(pw.where_clause.tag==2)
return(void*)& Cyc_Tcpat_Dummy_val;else{_Tmp0=pw.pattern.val;{struct Cyc_Absyn_Pat*p=_Tmp0;
# 1322
void*_Tmp1=p->r;union Cyc_Absyn_AggrInfo _Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp1)){case 6:
# 1324
 if(i!=0)
({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)i;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp7="get_access on pointer pattern with offset %d\n";_tag_fat(_Tmp7,sizeof(char),46U);}),_tag_fat(_Tmp6,sizeof(void*),1));});
return(void*)& Cyc_Tcpat_Deref_val;case 5:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Tcpat_TupleField_Tcpat_Access_struct));_Tmp5->tag=2,_Tmp5->f1=(unsigned)i;_Tmp5;});case 8: _Tmp4=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp1)->f2;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;struct Cyc_Absyn_Datatypefield*tuf=_Tmp3;
return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct));_Tmp5->tag=3,_Tmp5->f1=tud,_Tmp5->f2=tuf,_Tmp5->f3=(unsigned)i;_Tmp5;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f1!=0){_Tmp2=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f1;_Tmp4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp1)->f3;{union Cyc_Absyn_AggrInfo info=_Tmp2;struct Cyc_List_List*dlps=_Tmp4;
# 1330
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(info);
if(_check_null(ad->impl)->tagged){
struct Cyc_List_List*_Tmp5=(*((struct _tuple16*)_check_null(dlps)->hd)).f1;void*_Tmp6;if(_Tmp5!=0){if(*((int*)((struct Cyc_List_List*)_Tmp5)->hd)==1){if(((struct Cyc_List_List*)_Tmp5)->tl==0){_Tmp6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_Tmp5->hd)->f1;{struct _fat_ptr*f=_Tmp6;
# 1334
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct));_Tmp7->tag=4,_Tmp7->f1=_check_null(ad->impl)->tagged,_Tmp7->f2=f;_Tmp7;});}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13:
# 1336
({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=Cyc_Absynpp_pat2string(p);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp9="get_access on bad aggr pattern: %s";_tag_fat(_Tmp9,sizeof(char),35U);}),_tag_fat(_Tmp8,sizeof(void*),1));});};}{
# 1338
struct Cyc_List_List*fields=_check_null(ad->impl)->fields;
int orig_i=i;
for(1;i!=0;-- i){fields=_check_null(fields)->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_Tcpat_AggrField_Tcpat_Access_struct));_Tmp5->tag=4,_Tmp5->f1=_check_null(ad->impl)->tagged,_Tmp5->f2=((struct Cyc_Absyn_Aggrfield*)_check_null(fields)->hd)->name;_Tmp5;});}}}else{goto _LLE;}default: _LLE:
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_pat2string(p);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp7="get_access on bad pattern: %s";_tag_fat(_Tmp7,sizeof(char),30U);}),_tag_fat(_Tmp6,sizeof(void*),1));});};}};}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1348
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple24*env,int i){
void*_Tmp0;void*_Tmp1;_Tmp1=env->f1;_Tmp0=env->f2;{struct Cyc_List_List*obj=_Tmp1;struct Cyc_Tcpat_Con_s*pcon=_Tmp0;
void*acc=Cyc_Tcpat_get_access(pcon->orig_pat,i);
return({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct Cyc_Tcpat_PathNode*_Tmp3=({struct Cyc_Tcpat_PathNode*_Tmp4=_cycalloc(sizeof(struct Cyc_Tcpat_PathNode));_Tmp4->orig_pat=pcon->orig_pat,_Tmp4->access=acc;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=obj;_Tmp2;});}}
# 1353
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple24 env=({struct _tuple24 _Tmp0;_Tmp0.f1=obj,_Tmp0.f2=pcon;_Tmp0;});
return({(struct Cyc_List_List*(*)(int,struct Cyc_List_List*(*)(struct _tuple24*,int),struct _tuple24*))Cyc_List_tabulate_c;})(pcon->arity,Cyc_Tcpat_getoarg,& env);}
# 1361
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1364
void*_Tmp0;void*_Tmp1;if(*((int*)p)==0)
return({struct Cyc_List_List*_Tmp2=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_Tmp3=work;struct Cyc_Tcpat_Rhs*_Tmp4=right_hand_side;Cyc_Tcpat_and_match(_Tmp2,_Tmp3,_Tmp4,rules);});else{_Tmp1=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)p)->f1;_Tmp0=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)p)->f2;{struct Cyc_Tcpat_Con_s*pcon=_Tmp1;struct Cyc_List_List*pargs=_Tmp0;
# 1367
enum Cyc_Tcpat_Answer _Tmp2=Cyc_Tcpat_static_match(pcon,dsc);switch((int)_Tmp2){case Cyc_Tcpat_Yes:  {
# 1369
struct Cyc_List_List*ctx2;ctx2=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple22*_Tmp3=({struct _tuple22*_Tmp4=_cycalloc(sizeof(struct _tuple22));_Tmp4->f1=pcon,_Tmp4->f2=0;_Tmp4;});ctx2->hd=_Tmp3;}),ctx2->tl=ctx;{
struct _tuple21*work_frame;work_frame=_cycalloc(sizeof(struct _tuple21)),work_frame->f1=pargs,({struct Cyc_List_List*_Tmp3=Cyc_Tcpat_getoargs(pcon,obj);work_frame->f2=_Tmp3;}),({
struct Cyc_List_List*_Tmp3=Cyc_Tcpat_getdargs(pcon,dsc);work_frame->f3=_Tmp3;});{
struct Cyc_List_List*work2;work2=_cycalloc(sizeof(struct Cyc_List_List)),work2->hd=work_frame,work2->tl=work;
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}}}case Cyc_Tcpat_No:
# 1375
 return({void*_Tmp3=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_Tmp3,rules);});default:  {
# 1377
struct Cyc_List_List*ctx2;ctx2=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple22*_Tmp3=({struct _tuple22*_Tmp4=_cycalloc(sizeof(struct _tuple22));_Tmp4->f1=pcon,_Tmp4->f2=0;_Tmp4;});ctx2->hd=_Tmp3;}),ctx2->tl=ctx;{
struct _tuple21*work_frame;work_frame=_cycalloc(sizeof(struct _tuple21)),work_frame->f1=pargs,({struct Cyc_List_List*_Tmp3=Cyc_Tcpat_getoargs(pcon,obj);work_frame->f2=_Tmp3;}),({
struct Cyc_List_List*_Tmp3=Cyc_Tcpat_getdargs(pcon,dsc);work_frame->f3=_Tmp3;});{
struct Cyc_List_List*work2;work2=_cycalloc(sizeof(struct Cyc_List_List)),work2->hd=work_frame,work2->tl=work;{
void*s=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*f=({void*_Tmp3=({struct Cyc_List_List*_Tmp4=ctx;void*_Tmp5=Cyc_Tcpat_add_neg(dsc,pcon);Cyc_Tcpat_build_desc(_Tmp4,_Tmp5,work);});Cyc_Tcpat_or_match(_Tmp3,rules);});
return Cyc_Tcpat_ifeq(obj,pcon,s,f);}}}}};}};}
# 1393
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1398
void*_Tmp0;void*_Tmp1;switch(*((int*)d)){case 0: _Tmp1=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)d)->f1;{void*td=_Tmp1;
# 1400
if(!(*exhaust_done)){not_exhaust(env1,td);*exhaust_done=1;}
goto _LL0;}case 1: _Tmp1=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)d)->f1;{struct Cyc_Tcpat_Rhs*r=_Tmp1;
rhs_appears(env2,r);goto _LL0;}default: _Tmp1=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f2;_Tmp0=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)d)->f3;{struct Cyc_List_List*cases=_Tmp1;void*def=_Tmp0;
# 1404
for(1;cases!=0;cases=cases->tl){
struct _tuple0 _Tmp2=*((struct _tuple0*)cases->hd);void*_Tmp3;_Tmp3=_Tmp2.f2;{void*d=_Tmp3;
Cyc_Tcpat_check_exhaust_overlap(d,not_exhaust,env1,rhs_appears,env2,exhaust_done);}}
# 1409
Cyc_Tcpat_check_exhaust_overlap(def,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1411
goto _LL0;}}_LL0:;}
# 1419
static struct _tuple23*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs)),rhs->used=0,rhs->pat_loc=swc->pattern->loc,rhs->rhs=swc->body;{
void*sp;
if(swc->where_clause!=0){
union Cyc_Tcpat_PatOrWhere w=({union Cyc_Tcpat_PatOrWhere _Tmp0;_Tmp0.where_clause.tag=2U,_Tmp0.where_clause.val=swc->where_clause;_Tmp0;});
sp=({struct Cyc_List_List*_Tmp0=({void*_Tmp1[2];_Tmp1[0]=sp0,({void*_Tmp2=Cyc_Tcpat_int_pat(*ctr,w);_Tmp1[1]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_Tmp0,w);});
*ctr=*ctr + 1;}else{
# 1428
sp=({struct Cyc_List_List*_Tmp0=({void*_Tmp1[2];_Tmp1[0]=sp0,({void*_Tmp2=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct));_Tmp3->tag=0;_Tmp3;});_Tmp1[1]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_Tmp0,({union Cyc_Tcpat_PatOrWhere _Tmp1;_Tmp1.where_clause.tag=2U,_Tmp1.where_clause.val=0;_Tmp1;}));});}
return({struct _tuple23*_Tmp0=_cycalloc(sizeof(struct _tuple23));_Tmp0->f1=sp,_Tmp0->f2=rhs;_Tmp0;});}}char Cyc_Tcpat_Desc2string[12U]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1434
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1436
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _fat_ptr Cyc_Tcpat_desc2string(void*d){
void*_Tmp0;void*_Tmp1;if(*((int*)d)==0){_Tmp1=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)d)->f1;_Tmp0=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)d)->f2;{struct Cyc_Tcpat_Con_s*c=_Tmp1;struct Cyc_List_List*ds=_Tmp0;
# 1440
union Cyc_Tcpat_Name_value n=c->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _Tmp2=c->orig_pat;void*_Tmp3;if(_Tmp2.where_clause.tag==2)
return Cyc_Tcpat_desc2string((void*)_check_null(ds)->hd);else{_Tmp3=_Tmp2.pattern.val;{struct Cyc_Absyn_Pat*p2=_Tmp3;
p=p2;goto _LL5;}}_LL5:;}{
# 1446
void*_Tmp2=p->r;struct _fat_ptr _Tmp3;char _Tmp4;int _Tmp5;void*_Tmp6;void*_Tmp7;switch(*((int*)_Tmp2)){case 0:
 return({const char*_Tmp8="_";_tag_fat(_Tmp8,sizeof(char),2U);});case 1: _Tmp7=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;
return Cyc_Absynpp_qvar2string(vd->name);}case 3: _Tmp7=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp7;
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_qvar2string(vd->name);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="*%s";_tag_fat(_TmpA,sizeof(char),4U);}),_tag_fat(_Tmp9,sizeof(void*),1));});}case 4: _Tmp7=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;_Tmp6=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Tvar*tv=_Tmp7;struct Cyc_Absyn_Vardecl*vd=_Tmp6;
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_qvar2string(vd->name);_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=*tv->name;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_aprintf(({const char*_TmpB="%s<`%s>";_tag_fat(_TmpB,sizeof(char),8U);}),_tag_fat(_TmpA,sizeof(void*),2));});}case 5:
# 1452
 return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Tcpat_descs2string(ds);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="$(%s)";_tag_fat(_TmpA,sizeof(char),6U);}),_tag_fat(_Tmp9,sizeof(void*),1));});case 6:
 return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Tcpat_descs2string(ds);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="&%s";_tag_fat(_TmpA,sizeof(char),4U);}),_tag_fat(_Tmp9,sizeof(void*),1));});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp2)->f1!=0){if(((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp2)->f1)->KnownAggr.tag==2){_Tmp7=*((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp2)->f1->KnownAggr.val;_Tmp6=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_Tmp2)->f3;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;struct Cyc_List_List*dlps=_Tmp6;
# 1455
if((int)ad->kind==1){
void*_Tmp8;if(dlps!=0){if(((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f1!=0){if(*((int*)((struct Cyc_List_List*)((struct _tuple16*)((struct Cyc_List_List*)dlps)->hd)->f1)->hd)==1){_Tmp8=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct _tuple16*)dlps->hd)->f1->hd)->f1;{struct _fat_ptr*f=_Tmp8;
# 1458
return({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_Absynpp_qvar2string(ad->name);_TmpA.f1=_TmpB;});_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=*f;_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,({
struct _fat_ptr _TmpD=Cyc_Tcpat_descs2string(ds);_TmpC.f1=_TmpD;});_TmpC;});void*_TmpC[3];_TmpC[0]=& _Tmp9,_TmpC[1]=& _TmpA,_TmpC[2]=& _TmpB;Cyc_aprintf(({const char*_TmpD="%s{.%s = %s}";_tag_fat(_TmpD,sizeof(char),13U);}),_tag_fat(_TmpC,sizeof(void*),3));});}}else{goto _LL2E;}}else{goto _LL2E;}}else{_LL2E:
 goto _LL2B;}_LL2B:;}
# 1462
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_qvar2string(ad->name);_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_Tcpat_descs2string(ds);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_aprintf(({const char*_TmpB="%s{%s}";_tag_fat(_TmpB,sizeof(char),7U);}),_tag_fat(_TmpA,sizeof(void*),2));});}}else{goto _LL29;}}else{goto _LL29;}case 8: _Tmp7=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Datatypefield*tuf=_Tmp7;
# 1464
if(ds==0)
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_qvar2string(tuf->name);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="%s";_tag_fat(_TmpA,sizeof(char),3U);}),_tag_fat(_Tmp9,sizeof(void*),1));});else{
# 1467
return({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=Cyc_Absynpp_qvar2string(tuf->name);_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_Tcpat_descs2string(ds);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_aprintf(({const char*_TmpB="%s(%s)";_tag_fat(_TmpB,sizeof(char),7U);}),_tag_fat(_TmpA,sizeof(void*),2));});}}case 9:
 return({const char*_Tmp8="NULL";_tag_fat(_Tmp8,sizeof(char),5U);});case 10: _Tmp5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{int i=_Tmp5;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)i;_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="%d";_tag_fat(_TmpA,sizeof(char),3U);}),_tag_fat(_Tmp9,sizeof(void*),1));});}case 11: _Tmp4=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;{char c=_Tmp4;
return({struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)((int)c);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_aprintf(({const char*_TmpA="%d";_tag_fat(_TmpA,sizeof(char),3U);}),_tag_fat(_Tmp9,sizeof(void*),1));});}case 12: _Tmp3=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;_Tmp5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{struct _fat_ptr f=_Tmp3;int i=_Tmp5;
return f;}case 13: _Tmp7=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;{struct Cyc_Absyn_Enumfield*ef=_Tmp7;
_Tmp7=ef;goto _LL26;}case 14: _Tmp7=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_Tmp2)->f2;_LL26: {struct Cyc_Absyn_Enumfield*ef=_Tmp7;
return Cyc_Absynpp_qvar2string(ef->name);}case 17: _Tmp7=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Exp*e=_Tmp7;
return Cyc_Absynpp_exp2string(e);}default: _LL29:
(void*)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}}}else{_Tmp1=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)d)->f1;{struct Cyc_Set_Set*s=_Tmp1;
# 1478
if(Cyc_Set_is_empty(s))return({const char*_Tmp2="_";_tag_fat(_Tmp2,sizeof(char),2U);});{
struct Cyc_Tcpat_Con_s*c=({(struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*))Cyc_Set_choose;})(s);
# 1482
if((int)(c->orig_pat.where_clause.tag==2))(void*)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*orig_pat=({union Cyc_Tcpat_PatOrWhere _Tmp2=c->orig_pat;if(_Tmp2.pattern.tag!=1)_throw_match();_Tmp2.pattern.val;});
void*_Tmp2=Cyc_Absyn_compress(_check_null(orig_pat->topt));void*_Tmp3;switch(*((int*)_Tmp2)){case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)){case 1: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f2==Cyc_Absyn_Char_sz){
# 1487
{int i=0;for(0;i < 256;++ i){
if(!({struct Cyc_Set_Set*_Tmp4=s;({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;})(_Tmp4,Cyc_Tcpat_char_con((char)i,orig_pat));}))
return({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)i;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="%d";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}}
(void*)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{
# 1493
{unsigned i=0U;for(0;i < 4294967295U;++ i){
if(!({struct Cyc_Set_Set*_Tmp4=s;({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;})(_Tmp4,Cyc_Tcpat_int_con((int)i,c->orig_pat));}))
return({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned long)((int)i);_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="%d";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}}
(void*)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 18: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1.KnownDatatype.tag==2){_Tmp3=*((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1.KnownDatatype.val;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp3;
# 1502
if(tud->is_extensible)(void*)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*fields=(struct Cyc_List_List*)_check_null(tud->fields)->v;
int span=Cyc_List_length(fields);
for(1;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)fields->hd)->name).f2;
struct Cyc_List_List*args=((struct Cyc_Absyn_Datatypefield*)fields->hd)->typs;
if(!({struct Cyc_Set_Set*_Tmp4=s;({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;})(_Tmp4,({struct Cyc_Tcpat_Con_s*_Tmp5=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp6=Cyc_Tcpat_Name_v(n);_Tmp5->name=_Tmp6;}),_Tmp5->arity=0,_Tmp5->span=0,_Tmp5->orig_pat=c->orig_pat;_Tmp5;}));}))
return Cyc_List_length(args)==0?n:({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=n;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="%s(...)";_tag_fat(_Tmp6,sizeof(char),8U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}
# 1511
(void*)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}}else{goto _LL3B;}case 20: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1.KnownAggr.tag==2){_Tmp3=*((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1.KnownAggr.val;if((int)((struct Cyc_Absyn_Aggrdecl*)_Tmp3)->kind==1){struct Cyc_Absyn_Aggrdecl*ad=_Tmp3;
# 1513
struct Cyc_List_List*fields=_check_null(ad->impl)->fields;
int span=Cyc_List_length(fields);
struct _tuple2*_Tmp4=ad->name;struct _fat_ptr _Tmp5;_Tmp5=*_Tmp4->f2;{struct _fat_ptr union_name=_Tmp5;
for(1;(unsigned)fields;fields=fields->tl){
struct _fat_ptr n=*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;
if(!({struct Cyc_Set_Set*_Tmp6=s;({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;})(_Tmp6,({struct Cyc_Tcpat_Con_s*_Tmp7=_cycalloc(sizeof(struct Cyc_Tcpat_Con_s));({union Cyc_Tcpat_Name_value _Tmp8=Cyc_Tcpat_Name_v(n);_Tmp7->name=_Tmp8;}),_Tmp7->arity=0,_Tmp7->span=0,_Tmp7->orig_pat=c->orig_pat;_Tmp7;}));}))
return({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=union_name;_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=n;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="%s{.%s = _}";_tag_fat(_Tmp9,sizeof(char),12U);}),_tag_fat(_Tmp8,sizeof(void*),2));});}
# 1521
(void*)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL3B;}}else{goto _LL3B;}default: goto _LL3B;}case 3: _Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp2)->f1.ptr_atts.nullable;{void*n=_Tmp3;
# 1498
if(Cyc_Tcutil_force_type2bool(0,n)&& !({struct Cyc_Set_Set*_Tmp4=s;({(int(*)(struct Cyc_Set_Set*,struct Cyc_Tcpat_Con_s*))Cyc_Set_member;})(_Tmp4,Cyc_Tcpat_null_con(orig_pat));}))
return({const char*_Tmp4="NULL";_tag_fat(_Tmp4,sizeof(char),5U);});
return({const char*_Tmp4="&_";_tag_fat(_Tmp4,sizeof(char),3U);});}default: _LL3B:
# 1522
(void*)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}}}};}
# 1526
static struct _fat_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=Cyc_Tcpat_desc2string(d);*_Tmp0=_Tmp1;});_Tmp0;});}
# 1529
static struct _fat_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*ss=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(void*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_desc2stringptr,ds);
struct _fat_ptr*comma;comma=_cycalloc(sizeof(struct _fat_ptr)),*comma=({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);});
{struct Cyc_List_List*x=ss;for(0;x!=0;x=_check_null(x)->tl){
if(x->tl!=0){
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=comma,_Tmp1->tl=x->tl;_Tmp1;});x->tl=_Tmp0;});
x=x->tl;}}}
# 1537
return Cyc_strconcat_l(ss);}
# 1540
static void Cyc_Tcpat_not_exhaust_err(unsigned loc,void*desc){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _fat_ptr s=Cyc_Tcpat_desc2string(desc);
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="patterns may not be exhaustive.\n\tmissing case for ";_tag_fat(_Tmp4,sizeof(char),51U);});_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),2));});}
# 1542
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_Tmp2)->tag==Cyc_Tcpat_Desc2string){
# 1545
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="patterns may not be exhaustive.";_tag_fat(_Tmp6,sizeof(char),32U);});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),1));});
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}}_LL0:;}}}
# 1549
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1553
void Cyc_Tcpat_check_switch_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1559
int where_ctr=0;
int*env=& where_ctr;
struct Cyc_List_List*match_rules=({(struct Cyc_List_List*(*)(struct _tuple23*(*)(int*,struct Cyc_Absyn_Switch_clause*),int*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcpat_get_match,env,swcs);
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);
*dopt=dec_tree;{
# 1565
int ex_done=0;
({(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;})(dec_tree,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1568
for(1;match_rules!=0;match_rules=match_rules->tl){
struct _tuple23*_Tmp0=(struct _tuple23*)match_rules->hd;unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0->f2->used;_Tmp1=_Tmp0->f2->pat_loc;{int b=_Tmp2;unsigned loc2=_Tmp1;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_Tmp5,sizeof(char),74U);});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(loc2,_tag_fat(_Tmp4,sizeof(void*),1));});
# 1573
break;}}}}}
# 1582
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple15*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _fat_ptr s=Cyc_Tcpat_desc2string(desc);
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="pattern not exhaustive.\n\tmissing case for ";_tag_fat(_Tmp4,sizeof(char),43U);});_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=s;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_Warn_warn2((*pr).f1,_tag_fat(_Tmp4,sizeof(void*),2));});}
# 1585
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_Tmp2)->tag==Cyc_Tcpat_Desc2string){
# 1588
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6="pattern not exhaustive.";_tag_fat(_Tmp6,sizeof(char),24U);});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_Warn_warn2((*pr).f1,_tag_fat(_Tmp5,sizeof(void*),1));});
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;(void*)_rethrow(exn);}}_LL0:;}}}}
# 1592
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1596
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs)),rhs->used=0,rhs->pat_loc=p->loc,({struct Cyc_Absyn_Stmt*_Tmp0=Cyc_Absyn_skip_stmt(0U);rhs->rhs=_Tmp0;});{
struct Cyc_List_List*match_rules;match_rules=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple23*_Tmp0=({struct _tuple23*_Tmp1=_cycalloc(sizeof(struct _tuple23));({void*_Tmp2=Cyc_Tcpat_compile_pat(p);_Tmp1->f1=_Tmp2;}),_Tmp1->f2=rhs;_Tmp1;});match_rules->hd=_Tmp0;}),match_rules->tl=0;{
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);
struct _tuple15 exhaust_env=({struct _tuple15 _Tmp0;_Tmp0.f1=loc,_Tmp0.f2=1;_Tmp0;});
int ex_done=0;
({(void(*)(void*,void(*)(struct _tuple15*,void*),struct _tuple15*,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;})(dec_tree,Cyc_Tcpat_not_exhaust_warn,& exhaust_env,Cyc_Tcpat_dummy_fn,0,& ex_done);
# 1604
*dopt=dec_tree;
return exhaust_env.f2;}}}
# 1612
static struct _tuple23*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
# 1616
if(swc->where_clause!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2="&&-clauses not supported in exception handlers.";_tag_fat(_Tmp2,sizeof(char),48U);});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(_check_null(swc->where_clause)->loc,_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1619
struct Cyc_Tcpat_Rhs*rhs;rhs=_cycalloc(sizeof(struct Cyc_Tcpat_Rhs)),rhs->used=0,rhs->pat_loc=swc->pattern->loc,rhs->rhs=swc->body;
return({struct _tuple23*_Tmp0=_cycalloc(sizeof(struct _tuple23));_Tmp0->f1=sp0,_Tmp0->f2=rhs;_Tmp0;});}}
# 1622
static void Cyc_Tcpat_not_exhaust_err2(unsigned loc,void*d){;}
# 1625
void Cyc_Tcpat_check_catch_overlap(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1629
struct Cyc_List_List*match_rules=({(struct Cyc_List_List*(*)(struct _tuple23*(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_Tcpat_get_match2,swcs);
void*dec_tree=Cyc_Tcpat_match_compile(match_rules);
*dopt=dec_tree;{
int ex_done=0;
({(void(*)(void*,void(*)(unsigned,void*),unsigned,void(*)(int,struct Cyc_Tcpat_Rhs*),int,int*))Cyc_Tcpat_check_exhaust_overlap;})(dec_tree,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& ex_done);
# 1635
for(1;match_rules!=0;match_rules=match_rules->tl){
# 1637
if(match_rules->tl==0)break;{
struct _tuple23*_Tmp0=(struct _tuple23*)match_rules->hd;unsigned _Tmp1;int _Tmp2;_Tmp2=_Tmp0->f2->used;_Tmp1=_Tmp0->f2->pat_loc;{int b=_Tmp2;unsigned loc2=_Tmp1;
if(!b){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_fat(_Tmp5,sizeof(char),74U);});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(loc2,_tag_fat(_Tmp4,sizeof(void*),1));});
break;}}}}}}
# 1646
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*l=(struct Cyc_List_List*)_check_null(pat_vars)->v;for(0;l!=0;l=l->tl){
if((*((struct _tuple17*)l->hd)).f1!=0)
return 1;}}
return 0;}

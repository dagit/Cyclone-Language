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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 154 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 47
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*,int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*);
# 100
extern void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(void*));struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 210
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);
# 261
extern int Cyc_List_exists_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 142 "cycboot.h"
extern int Cyc_getc(struct Cyc___cycFILE*);
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 222 "cycboot.h"
extern int Cyc_ungetc(int,struct Cyc___cycFILE*);
# 243
extern int Cyc_vsprintf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 105 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 131
extern struct _fat_ptr Cyc_strtok(struct _fat_ptr,struct _fat_ptr);
# 43 "filename.h"
extern struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 163
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 313 "absyn.h"
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 333
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 549 "absyn.h"
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_hash_qvar(struct _tuple0*);
# 939
void*Cyc_Absyn_compress(void*);
# 949
void*Cyc_Absyn_app_type(void*,struct _fat_ptr);
# 974
extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 1005
extern void*Cyc_Absyn_fat_bound_type;
# 1009
void*Cyc_Absyn_bounds_one (void);
# 1013
void*Cyc_Absyn_thinconst (void);
# 1241
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);
struct Cyc_List_List*Cyc_Binding_resolve_all_interproc(struct Cyc_List_List*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cycinf_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 72
struct _fat_ptr Cyc_Absynpp_fullqvar2string(struct _tuple0*);
# 25 "cyclone.h"
struct Cyc_List_List*Cyc_Cyclone_parse_file(struct _fat_ptr);
# 27 "warn.h"
extern void Cyc_Warn_reset(struct _fat_ptr);
# 37
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 43
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 277 "tcutil.h"
int Cyc_Tcutil_is_cvar_type(void*);
# 280
void Cyc_Tcutil_introduce_cvars(void*);
void Cyc_Tcutil_replace_cvars(void*,void*);
void Cyc_Tcutil_revert_cvars(void*);
int Cyc_Tcutil_is_main(struct _tuple0*);
# 33 "bansheeif.h"
void Cyc_BansheeIf_persist_cvarmap(struct Cyc___cycFILE*);
void Cyc_BansheeIf_reset_local_cvar(void*);
void Cyc_BansheeIf_register_toplevel_cvar(void*);
void Cyc_BansheeIf_init_toplevel_cvars(struct Cyc_List_List*);
# 38
void Cyc_BansheeIf_resolve_or_default(void*);
# 40
int Cyc_BansheeIf_add_all_constraints(struct Cyc_List_List*);
# 26 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_constraint_file(struct Cyc___cycFILE*);
# 41 "flags.h"
extern int Cyc_Flags_resolve;
extern int Cyc_Flags_no_merge;
# 22 "interproc.cyc"
void Cyc_Tcutil_deconstruct_type(void*);char Cyc_Interproc_Rebinding[10U]="Rebinding";struct Cyc_Interproc_Rebinding_exn_struct{char*tag;};char Cyc_Interproc_Unresolved_sym[15U]="Unresolved_sym";struct Cyc_Interproc_Unresolved_sym_exn_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 31
static struct Cyc_Interproc_Rebinding_exn_struct Cyc_Interproc_Rebinding_val={Cyc_Interproc_Rebinding};
# 33
struct _fat_ptr Cyc_Interproc_make_parg(struct _fat_ptr ap){struct _fat_ptr _T0;struct _fat_ptr _T1;void**_T2;unsigned _T3;struct _fat_ptr _T4;void**_T5;unsigned _T6;struct _fat_ptr _T7;unsigned char*_T8;void**_T9;unsigned _TA;int _TB;struct _fat_ptr _TC;_T1=ap;{unsigned _TD=
_get_fat_size(_T1,sizeof(void*));_T3=_check_times(_TD,sizeof(void*));{void**_TE=_cycalloc(_T3);_T4=ap;{unsigned _TF=_get_fat_size(_T4,sizeof(void*));unsigned i;i=0;_TL3: if(i < _TF)goto _TL1;else{goto _TL2;}_TL1: _T6=i;_T5=_TE + _T6;_T7=ap;_T8=_T7.curr;_T9=(void**)_T8;_TA=i;_TB=(int)_TA;*_T5=_T9[_TB];i=i + 1;goto _TL3;_TL2:;}_T2=(void**)_TE;}_T0=_tag_fat(_T2,sizeof(void*),_TD);}{struct _fat_ptr ret=_T0;_TC=ret;
return _TC;}}
# 38
static struct _fat_ptr*Cyc_Interproc_cych_pfx=0;
void Cyc_Interproc_set_inference_prefix(struct _fat_ptr pfx){struct _fat_ptr*_T0;{struct _fat_ptr*_T1=_cycalloc(sizeof(struct _fat_ptr));
*_T1=pfx;_T0=(struct _fat_ptr*)_T1;}Cyc_Interproc_cych_pfx=_T0;}
# 42
static struct _fat_ptr Cyc_Interproc_get_inference_prefix (void){struct _fat_ptr*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;
if(Cyc_Interproc_cych_pfx!=0)goto _TL4;{struct _fat_ptr*_T3=_cycalloc(sizeof(struct _fat_ptr));
*_T3=_tag_fat(".",sizeof(char),2U);_T0=(struct _fat_ptr*)_T3;}Cyc_Interproc_cych_pfx=_T0;goto _TL5;_TL4: _TL5: _T1=Cyc_Interproc_cych_pfx;_T2=*_T1;
return _T2;}
# 57 "interproc.cyc"
static struct Cyc___cycFILE*Cyc_Interproc_tagsFile=0;
# 59
static struct Cyc___cycFILE*Cyc_Interproc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){struct _handler_cons*_T0;int*_T1;int _T2;struct _fat_ptr _T3;char*_T4;char*_T5;const char*_T6;struct _fat_ptr _T7;char*_T8;char*_T9;const char*_TA;void*_TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc_String_pa_PrintArg_struct _TD;void**_TE;void**_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct _handler_cons _T12;_T0=& _T12;_push_handler(_T0);{int _T13=0;_T1=_T12.handler;_T2=setjmp(_T1);if(!_T2)goto _TL6;_T13=1;goto _TL7;_TL6: _TL7: if(_T13)goto _TL8;else{goto _TLA;}_TLA: _T3=filename;_T4=_untag_fat_ptr_check_bound(_T3,sizeof(char),1U);_T5=_check_null(_T4);_T6=(const char*)_T5;_T7=mode;_T8=_untag_fat_ptr_check_bound(_T7,sizeof(char),1U);_T9=_check_null(_T8);_TA=(const char*)_T9;{struct Cyc___cycFILE*_T14=
Cyc_fopen(_T6,_TA);_npop_handler(0);return _T14;}_pop_handler();goto _TL9;_TL8: _TB=Cyc_Core_get_exn_thrown();{void*_T14=(void*)_TB;{struct Cyc_String_pa_PrintArg_struct _T15;_T15.tag=0;
# 63
_T15.f1=msg_part;_TC=_T15;}{struct Cyc_String_pa_PrintArg_struct _T15=_TC;{struct Cyc_String_pa_PrintArg_struct _T16;_T16.tag=0;_T16.f1=filename;_TD=_T16;}{struct Cyc_String_pa_PrintArg_struct _T16=_TD;void*_T17[2];_TE=_T17 + 0;*_TE=& _T15;_TF=_T17 + 1;*_TF=& _T16;_T10=_tag_fat("Couldn't open %s %s",sizeof(char),20U);_T11=_tag_fat(_T17,sizeof(void*),2);Cyc_Warn_err(0U,_T10,_T11);}}
return 0;;}_TL9:;}}
# 68
static struct _fat_ptr Cyc_Interproc_namespace2string(struct Cyc_List_List*ns){struct Cyc_List_List*_T0;struct _fat_ptr _T1;unsigned long _T2;int _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;_T0=ns;_T1=
_tag_fat("::",sizeof(char),3U);{struct _fat_ptr a=Cyc_str_sepstr(_T0,_T1);_T2=
Cyc_strlen(a);_T3=(int)_T2;if(!_T3)goto _TLB;_T4=a;_T5=
_tag_fat("::",sizeof(char),3U);_T6=Cyc_strconcat(_T4,_T5);return _T6;_TLB: _T7=a;
# 73
return _T7;}}
# 76
static void Cyc_Interproc_dump_decl_tags(struct Cyc_List_List*ns,struct _fat_ptr sourcefile,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct Cyc_String_pa_PrintArg_struct _T7;struct Cyc_String_pa_PrintArg_struct _T8;struct Cyc_Absyn_Fndecl*_T9;struct _tuple0*_TA;struct Cyc_String_pa_PrintArg_struct _TB;void**_TC;void**_TD;void**_TE;struct Cyc___cycFILE*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct _fat_ptr _T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;
_TL10: _T0=tds;_T1=(unsigned)_T0;if(_T1)goto _TLE;else{goto _TLF;}
_TLE: _T2=tds;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T18=_T4->r;struct _tuple0*_T19;struct Cyc_List_List*_T1A;struct _fat_ptr*_T1B;struct Cyc_Absyn_Fndecl*_T1C;_T5=(int*)_T18;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1D=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T18;_T1C=_T1D->f1;}{struct Cyc_Absyn_Fndecl*fd=_T1C;{struct Cyc_String_pa_PrintArg_struct _T1D;_T1D.tag=0;
# 80
_T1D.f1=Cyc_Interproc_namespace2string(ns);_T7=_T1D;}{struct Cyc_String_pa_PrintArg_struct _T1D=_T7;{struct Cyc_String_pa_PrintArg_struct _T1E;_T1E.tag=0;_T9=fd;_TA=_T9->name;
_T1E.f1=Cyc_Absynpp_fullqvar2string(_TA);_T8=_T1E;}{struct Cyc_String_pa_PrintArg_struct _T1E=_T8;{struct Cyc_String_pa_PrintArg_struct _T1F;_T1F.tag=0;
_T1F.f1=sourcefile;_TB=_T1F;}{struct Cyc_String_pa_PrintArg_struct _T1F=_TB;void*_T20[3];_TC=_T20 + 0;*_TC=& _T1D;_TD=_T20 + 1;*_TD=& _T1E;_TE=_T20 + 2;*_TE=& _T1F;_TF=
# 80
_check_null(Cyc_Interproc_tagsFile);_T10=_tag_fat("^%s%s^%s^function^0\n",sizeof(char),21U);_T11=_tag_fat(_T20,sizeof(void*),3);Cyc_fprintf(_TF,_T10,_T11);}}}goto _LL0;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1D=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T18;_T1B=_T1D->f1;_T1A=_T1D->f2;}{struct _fat_ptr*n=_T1B;struct Cyc_List_List*l=_T1A;_T12=ns;{struct Cyc_List_List*_T1D=_cycalloc(sizeof(struct Cyc_List_List));
# 85
_T1D->hd=n;_T1D->tl=0;_T13=(struct Cyc_List_List*)_T1D;}_T14=Cyc_List_append(_T12,_T13);_T15=sourcefile;_T16=l;Cyc_Interproc_dump_decl_tags(_T14,_T15,_T16);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1D=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T18;_T19=_T1D->f1;_T1A=_T1D->f2;}{struct _tuple0*n=_T19;struct Cyc_List_List*l=_T1A;
# 88
Cyc_Interproc_dump_decl_tags(ns,sourcefile,l);goto _LL0;}default: goto _LL0;}_LL0:;}_T17=tds;
# 77
tds=_T17->tl;goto _TL10;_TLF:;}
# 97
void Cyc_Interproc_dump_tags(struct _fat_ptr sourcefile,struct Cyc_List_List*tds){struct Cyc___cycFILE*_T0;unsigned _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc___cycFILE*_T5;unsigned _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;_T0=Cyc_Interproc_tagsFile;_T1=(unsigned)_T0;
if(_T1)goto _TL12;else{goto _TL14;}
_TL14: _T2=_tag_fat("CYCTAGS",sizeof(char),8U);_T3=_tag_fat("w",sizeof(char),2U);_T4=_tag_fat("CYCTAGS",sizeof(char),8U);Cyc_Interproc_tagsFile=Cyc_Interproc_try_file_open(_T2,_T3,_T4);_T5=Cyc_Interproc_tagsFile;_T6=(unsigned)_T5;
if(_T6)goto _TL15;else{goto _TL17;}
_TL17: _T7=_tag_fat("Could not open TAGS file",sizeof(char),25U);_T8=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(0U,_T7,_T8);
return;_TL15: goto _TL13;_TL12: _TL13:
# 105
 Cyc_Interproc_dump_decl_tags(0,sourcefile,tds);}
# 107
void Cyc_Interproc_close_tags_file (void){struct Cyc___cycFILE*_T0;unsigned _T1;_T0=Cyc_Interproc_tagsFile;_T1=(unsigned)_T0;
if(!_T1)goto _TL18;
Cyc_fclose(Cyc_Interproc_tagsFile);goto _TL19;_TL18: _TL19:;}
# 117
static struct Cyc_Hashtable_Table*Cyc_Interproc_tagmap=0;struct _tuple11{struct _fat_ptr f0;struct Cyc_List_List*f1;};
# 119
static void Cyc_Interproc_print_filedecls(struct Cyc_List_List*fd){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_List_List*_T3;void*_T4;struct _tuple11*_T5;void**_T6;struct Cyc___cycFILE*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_List_List*_TA;
_TL1D: _T0=fd;_T1=(unsigned)_T0;if(_T1)goto _TL1B;else{goto _TL1C;}
_TL1B:{struct Cyc_String_pa_PrintArg_struct _TB;_TB.tag=0;_T3=fd;_T4=_T3->hd;_T5=(struct _tuple11*)_T4;_TB.f1=_T5->f0;_T2=_TB;}{struct Cyc_String_pa_PrintArg_struct _TB=_T2;void*_TC[1];_T6=_TC + 0;*_T6=& _TB;_T7=Cyc_stderr;_T8=_tag_fat("Decls for file %s::\n",sizeof(char),21U);_T9=_tag_fat(_TC,sizeof(void*),1);Cyc_fprintf(_T7,_T8,_T9);}_TA=fd;
# 120
fd=_TA->tl;goto _TL1D;_TL1C:;}
# 125
static void Cyc_Interproc_skip_whitespace(struct Cyc___cycFILE*f){
int c=Cyc_getc(f);
_TL1E: if(c==32)goto _TL1F;else{goto _TL22;}_TL22: if(c==9)goto _TL1F;else{goto _TL21;}_TL21: if(c==10)goto _TL1F;else{goto _TL20;}
_TL1F: c=Cyc_getc(f);goto _TL1E;_TL20:
# 130
 Cyc_ungetc(c,f);}
# 132
static struct _fat_ptr Cyc_Interproc_grabline(struct Cyc___cycFILE*f){char*_T0;struct _handler_cons*_T1;int*_T2;int _T3;struct _fat_ptr _T4;int _T5;int _T6;char*_T7;char*_T8;int _T9;struct _fat_ptr _TA;int _TB;char*_TC;char*_TD;void*_TE;struct Cyc___cycFILE*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct _fat_ptr _T12;
static char buf[1000U];
int c=0;
int i=0;_T0=buf;{
struct _fat_ptr bufp=_tag_fat(_T0,sizeof(char),1000U);
Cyc_Interproc_skip_whitespace(f);{struct _handler_cons _T13;_T1=& _T13;_push_handler(_T1);{int _T14=0;_T2=_T13.handler;_T3=setjmp(_T2);if(!_T3)goto _TL23;_T14=1;goto _TL24;_TL23: _TL24: if(_T14)goto _TL25;else{goto _TL27;}_TL27:
# 139
 c=Cyc_getc(f);
_TL28: if(c!=10)goto _TL2B;else{goto _TL2A;}_TL2B: if(c!=-1)goto _TL29;else{goto _TL2A;}
_TL29: _T4=bufp;_T5=i;i=_T5 + 1;_T6=_T5;_T7=_check_fat_subscript(_T4,sizeof(char),_T6);_T8=(char*)_T7;_T9=c;*_T8=(char)_T9;
c=Cyc_getc(f);goto _TL28;_TL2A:
# 144
 if(c!=-1)goto _TL2C;{struct _fat_ptr _T15=_tag_fat(0,0,0);_npop_handler(0);return _T15;}_TL2C:
 Cyc_ungetc(c,f);_TA=bufp;_TB=i;_TC=_check_fat_subscript(_TA,sizeof(char),_TB);_TD=(char*)_TC;
*_TD='\000';{struct _fat_ptr _T15=
Cyc_strdup(bufp);_npop_handler(0);return _T15;}_pop_handler();goto _TL26;_TL25: _TE=Cyc_Core_get_exn_thrown();{void*_T15=(void*)_TE;_TF=Cyc_stderr;_T10=
# 149
_tag_fat("grabline failed\n",sizeof(char),17U);_T11=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_TF,_T10,_T11);goto _LL0;_LL0:;}_TL26:;}}_T12=
# 151
_tag_fat(0,0,0);return _T12;}}
# 154
static int Cyc_Interproc_mstrptrcmp(struct _fat_ptr*m1,struct _fat_ptr*m2){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;int _T4;_T0=m1;_T1=*_T0;_T2=m2;_T3=*_T2;_T4=
Cyc_strcmp(_T1,_T3);return _T4;}
# 157
static int Cyc_Interproc_hash_mstringptr(struct _fat_ptr*m1){struct _fat_ptr*_T0;struct _fat_ptr _T1;int _T2;_T0=m1;_T1=*_T0;_T2=
Cyc_Hashtable_hash_string(_T1);return _T2;}
# 161
static void Cyc_Interproc_print_string(struct _fat_ptr*s){struct Cyc_String_pa_PrintArg_struct _T0;struct _fat_ptr*_T1;struct Cyc_Int_pa_PrintArg_struct _T2;int _T3;void**_T4;void**_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;_T1=s;
_T8.f1=*_T1;_T0=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T0;{struct Cyc_Int_pa_PrintArg_struct _T9;_T9.tag=1;_T3=Cyc_Interproc_hash_mstringptr(s);_T9.f1=(unsigned long)_T3;_T2=_T9;}{struct Cyc_Int_pa_PrintArg_struct _T9=_T2;void*_TA[2];_T4=_TA + 0;*_T4=& _T8;_T5=_TA + 1;*_T5=& _T9;_T6=_tag_fat("<%s>:%d",sizeof(char),8U);_T7=_tag_fat(_TA,sizeof(void*),2);Cyc_printf(_T6,_T7);}}}
# 165
static int Cyc_Interproc_init_tagmap_from_decls(struct _fat_ptr*source,struct Cyc_List_List*ds){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;void(*_T7)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*);void(*_T8)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T9;struct _fat_ptr*_TA;struct _fat_ptr _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc_Absyn_Fndecl*_TD;struct _tuple0*_TE;void**_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct _fat_ptr*_T12;struct Cyc_List_List*_T13;
_TL31: _T0=ds;_T1=(unsigned)_T0;if(_T1)goto _TL2F;else{goto _TL30;}
_TL2F: _T2=ds;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T14=_T4->r;struct Cyc_List_List*_T15;struct Cyc_Absyn_Fndecl*_T16;_T5=(int*)_T14;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T17=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T14;_T16=_T17->f1;}{struct Cyc_Absyn_Fndecl*fd=_T16;_T8=Cyc_Hashtable_insert;{
# 169
void(*_T17)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*))_T8;_T7=_T17;}_T9=_check_null(Cyc_Interproc_tagmap);{struct _fat_ptr*_T17=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T18;_T18.tag=0;_TD=fd;_TE=_TD->name;
_T18.f1=Cyc_Absynpp_fullqvar2string(_TE);_TC=_T18;}{struct Cyc_String_pa_PrintArg_struct _T18=_TC;void*_T19[1];_TF=_T19 + 0;*_TF=& _T18;_T10=_tag_fat("%s",sizeof(char),3U);_T11=_tag_fat(_T19,sizeof(void*),1);_TB=Cyc_aprintf(_T10,_T11);}*_T17=_TB;_TA=(struct _fat_ptr*)_T17;}_T12=source;
# 169
_T7(_T9,_TA,_T12);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T17=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T14;_T15=_T17->f2;}{struct Cyc_List_List*l=_T15;_T15=l;goto _LL6;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T17=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T14;_T15=_T17->f2;}_LL6: {struct Cyc_List_List*l=_T15;
# 175
Cyc_Interproc_init_tagmap_from_decls(source,l);goto _LL0;}default: goto _LL0;}_LL0:;}_T13=ds;
# 166
ds=_T13->tl;goto _TL31;_TL30:
# 181
 return 1;}
# 185
static int Cyc_Interproc_init_tagmap(struct Cyc_List_List*ftds){struct Cyc_Hashtable_Table*_T0;unsigned _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc___cycFILE*_T5;unsigned _T6;struct Cyc_List_List*_T7;unsigned _T8;struct Cyc_Hashtable_Table*(*_T9)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_TA)(int,int(*)(void*,void*),int(*)(void*));struct _fat_ptr*_TB;struct _fat_ptr _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_List_List*_TE;void*_TF;struct _tuple11*_T10;void**_T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_List_List*_T14;void*_T15;struct _tuple11*_T16;struct Cyc_List_List*_T17;int _T18;struct Cyc_Hashtable_Table*(*_T19)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T1A)(int,int(*)(void*,void*),int(*)(void*));struct _fat_ptr _T1B;char*_T1C;unsigned _T1D;struct _fat_ptr _T1E;char*_T1F;char*_T20;char _T21;int _T22;struct _fat_ptr*_T23;struct _fat_ptr _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct _fat_ptr _T26;struct _fat_ptr _T27;void**_T28;struct _fat_ptr _T29;struct _fat_ptr _T2A;struct _fat_ptr*_T2B;struct _fat_ptr _T2C;struct Cyc_String_pa_PrintArg_struct _T2D;struct _fat_ptr _T2E;struct _fat_ptr _T2F;void**_T30;struct _fat_ptr _T31;struct _fat_ptr _T32;struct _fat_ptr _T33;struct _fat_ptr _T34;struct _fat_ptr _T35;struct _fat_ptr _T36;int _T37;void(*_T38)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*);void(*_T39)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T3A;struct _fat_ptr*_T3B;struct _fat_ptr*_T3C;_T0=Cyc_Interproc_tagmap;_T1=(unsigned)_T0;
if(!_T1)goto _TL33;
return 1;_TL33: _T2=
_tag_fat("CYCTAGS",sizeof(char),8U);_T3=_tag_fat("r",sizeof(char),2U);_T4=_tag_fat("CYCTAGS",sizeof(char),8U);{struct Cyc___cycFILE*tags=Cyc_Interproc_try_file_open(_T2,_T3,_T4);_T5=tags;_T6=(unsigned)_T5;
if(_T6)goto _TL35;else{goto _TL37;}
_TL37: _T7=ftds;_T8=(unsigned)_T7;if(_T8)goto _TL38;else{goto _TL3A;}
_TL3A: return 0;_TL38: _TA=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T3D)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_TA;_T9=_T3D;}Cyc_Interproc_tagmap=_T9(11,Cyc_Interproc_mstrptrcmp,Cyc_Interproc_hash_mstringptr);{struct _fat_ptr*_T3D=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T3E;_T3E.tag=0;_TE=ftds;_TF=_TE->hd;_T10=(struct _tuple11*)_TF;
_T3E.f1=_T10->f0;_TD=_T3E;}{struct Cyc_String_pa_PrintArg_struct _T3E=_TD;void*_T3F[1];_T11=_T3F + 0;*_T11=& _T3E;_T12=_tag_fat("%s",sizeof(char),3U);_T13=_tag_fat(_T3F,sizeof(void*),1);_TC=Cyc_aprintf(_T12,_T13);}*_T3D=_TC;_TB=(struct _fat_ptr*)_T3D;}_T14=ftds;_T15=_T14->hd;_T16=(struct _tuple11*)_T15;_T17=_T16->f1;_T18=Cyc_Interproc_init_tagmap_from_decls(_TB,_T17);return _T18;_TL35: _T1A=Cyc_Hashtable_create;{
# 195
struct Cyc_Hashtable_Table*(*_T3D)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T1A;_T19=_T3D;}Cyc_Interproc_tagmap=_T19(11,Cyc_Interproc_mstrptrcmp,Cyc_Interproc_hash_mstringptr);{
struct _fat_ptr line;
_TL3B: line=Cyc_Interproc_grabline(tags);_T1B=line;_T1C=_T1B.curr;_T1D=(unsigned)_T1C;if(_T1D)goto _TL3C;else{goto _TL3D;}
_TL3C: _T1E=line;_T1F=_check_fat_subscript(_T1E,sizeof(char),0);_T20=(char*)_T1F;_T21=*_T20;_T22=(int)_T21;if(_T22==94)goto _TL3E;goto _TL3B;_TL3E:{
# 200
struct _fat_ptr*symname;symname=_cycalloc(sizeof(struct _fat_ptr));_T23=symname;{struct Cyc_String_pa_PrintArg_struct _T3D;_T3D.tag=0;_T26=line;_T27=_tag_fat("^@",sizeof(char),3U);_T3D.f1=Cyc_strtok(_T26,_T27);_T25=_T3D;}{struct Cyc_String_pa_PrintArg_struct _T3D=_T25;void*_T3E[1];_T28=_T3E + 0;*_T28=& _T3D;_T29=_tag_fat("%s",sizeof(char),3U);_T2A=_tag_fat(_T3E,sizeof(void*),1);_T24=Cyc_aprintf(_T29,_T2A);}*_T23=_T24;{
struct _fat_ptr*filename;filename=_cycalloc(sizeof(struct _fat_ptr));_T2B=filename;{struct Cyc_String_pa_PrintArg_struct _T3D;_T3D.tag=0;_T2E=_tag_fat(0,0,0);_T2F=_tag_fat("^@",sizeof(char),3U);_T3D.f1=Cyc_strtok(_T2E,_T2F);_T2D=_T3D;}{struct Cyc_String_pa_PrintArg_struct _T3D=_T2D;void*_T3E[1];_T30=_T3E + 0;*_T30=& _T3D;_T31=_tag_fat("%s",sizeof(char),3U);_T32=_tag_fat(_T3E,sizeof(void*),1);_T2C=Cyc_aprintf(_T31,_T32);}*_T2B=_T2C;_T33=
_tag_fat(0,0,0);_T34=_tag_fat("^@",sizeof(char),3U);{struct _fat_ptr symtype=Cyc_strtok(_T33,_T34);_T35=symtype;_T36=
_tag_fat("function",sizeof(char),9U);_T37=Cyc_strcmp(_T35,_T36);if(_T37)goto _TL40;else{goto _TL42;}
# 206
_TL42: _T39=Cyc_Hashtable_insert;{void(*_T3D)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*))_T39;_T38=_T3D;}_T3A=_check_null(Cyc_Interproc_tagmap);_T3B=symname;_T3C=filename;_T38(_T3A,_T3B,_T3C);goto _TL41;_TL40: _TL41:;}}}goto _TL3B;_TL3D:
# 209
 Cyc_fclose(tags);
return 1;}}}
# 214
static struct _fat_ptr Cyc_Interproc_locate_qvar(struct _tuple0*qv,struct Cyc_List_List*ftds){struct Cyc_Hashtable_Table*_T0;unsigned _T1;int _T2;struct Cyc_Core_Not_found_exn_struct*_T3;struct Cyc_Core_Not_found_exn_struct*_T4;struct _fat_ptr*_T5;struct _fat_ptr _T6;struct Cyc_String_pa_PrintArg_struct _T7;void**_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct _fat_ptr*(*_TB)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_TC)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TD;struct _fat_ptr*_TE;struct _fat_ptr*_TF;struct _fat_ptr _T10;_T0=Cyc_Interproc_tagmap;_T1=(unsigned)_T0;
if(_T1)goto _TL43;else{goto _TL45;}_TL45: _T2=Cyc_Interproc_init_tagmap(ftds);if(_T2)goto _TL43;else{goto _TL46;}
_TL46: _T3=& Cyc_Core_Not_found_val;_T4=(struct Cyc_Core_Not_found_exn_struct*)_T3;_throw(_T4);goto _TL44;_TL43: _TL44: {
# 219
struct _fat_ptr*qvname;qvname=_cycalloc(sizeof(struct _fat_ptr));_T5=qvname;{struct Cyc_String_pa_PrintArg_struct _T11;_T11.tag=0;_T11.f1=Cyc_Absynpp_fullqvar2string(qv);_T7=_T11;}{struct Cyc_String_pa_PrintArg_struct _T11=_T7;void*_T12[1];_T8=_T12 + 0;*_T8=& _T11;_T9=_tag_fat("%s",sizeof(char),3U);_TA=_tag_fat(_T12,sizeof(void*),1);_T6=Cyc_aprintf(_T9,_TA);}*_T5=_T6;_TC=Cyc_Hashtable_lookup;{
struct _fat_ptr*(*_T11)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct _fat_ptr*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_TC;_TB=_T11;}_TD=_check_null(Cyc_Interproc_tagmap);_TE=qvname;_TF=_TB(_TD,_TE);_T10=*_TF;return _T10;}}struct _tuple12{struct _fat_ptr f0;void*f1;};
# 226
static int Cyc_Interproc_cmp_filenames(struct _fat_ptr*fname,struct _tuple12*ftd){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct _tuple12*_T2;struct _fat_ptr _T3;int _T4;int _T5;_T0=fname;_T1=*_T0;_T2=ftd;_T3=_T2->f0;_T4=
Cyc_strcmp(_T1,_T3);_T5=!_T4;return _T5;}struct _tuple13{int f0;struct Cyc_List_List*f1;};
# 233
static struct _tuple13 Cyc_Interproc_add_filedecls(struct _fat_ptr filename,struct Cyc_List_List*ftds){int(*_T0)(int(*)(struct _fat_ptr*,struct _tuple11*),struct _fat_ptr*,struct Cyc_List_List*);int(*_T1)(int(*)(void*,void*),void*,struct Cyc_List_List*);int(*_T2)(struct _fat_ptr*,struct _tuple11*);struct _fat_ptr*_T3;struct _fat_ptr*_T4;struct Cyc_List_List*_T5;int _T6;struct _tuple13 _T7;struct Cyc_List_List*_T8;unsigned _T9;struct _tuple13 _TA;struct Cyc_List_List*_TB;struct _tuple11*_TC;struct Cyc_List_List*_TD;struct _tuple13 _TE;_T1=Cyc_List_exists_c;{
int(*_TF)(int(*)(struct _fat_ptr*,struct _tuple11*),struct _fat_ptr*,struct Cyc_List_List*)=(int(*)(int(*)(struct _fat_ptr*,struct _tuple11*),struct _fat_ptr*,struct Cyc_List_List*))_T1;_T0=_TF;}{int(*_TF)(struct _fat_ptr*,struct _tuple11*)=(int(*)(struct _fat_ptr*,struct _tuple11*))Cyc_Interproc_cmp_filenames;_T2=_TF;}_T3=& filename;_T4=(struct _fat_ptr*)_T3;_T5=ftds;_T6=_T0(_T2,_T4,_T5);if(!_T6)goto _TL47;{struct _tuple13 _TF;
_TF.f0=1;_TF.f1=0;_T7=_TF;}return _T7;_TL47: {
struct Cyc_List_List*decls=Cyc_Cyclone_parse_file(filename);_T8=decls;_T9=(unsigned)_T8;
if(_T9)goto _TL49;else{goto _TL4B;}
_TL4B:{struct _tuple13 _TF;_TF.f0=0;_TF.f1=0;_TA=_TF;}return _TA;_TL49: {
# 240
struct Cyc_List_List*newl;newl=_cycalloc(sizeof(struct Cyc_List_List));_TB=newl;{struct _tuple11*_TF=_cycalloc(sizeof(struct _tuple11));_TF->f0=filename;_TF->f1=decls;_TC=(struct _tuple11*)_TF;}_TB->hd=_TC;_TD=newl;_TD->tl=0;
Cyc_List_imp_append(ftds,newl);{struct _tuple13 _TF;
# 245
_TF.f0=1;_TF.f1=newl;_TE=_TF;}return _TE;}}}struct _tuple14{int f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 256 "interproc.cyc"
static struct _tuple14 Cyc_Interproc_locate_externs(struct Cyc_List_List*es,struct Cyc_List_List*ftds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;int*_T3;int _T4;void*_T5;void*_T6;int*_T7;unsigned _T8;void*_T9;int _TA;struct Cyc_Interproc_Unresolved_sym_exn_struct*_TB;struct Cyc_List_List*_TC;void*_TD;void*_TE;struct _handler_cons*_TF;int*_T10;int _T11;struct Cyc_List_List*_T12;struct Cyc_List_List*_T13;void*_T14;void*_T15;struct Cyc_Core_Not_found_exn_struct*_T16;char*_T17;char*_T18;struct Cyc_Interproc_Unresolved_sym_exn_struct*_T19;struct Cyc_List_List*_T1A;void*_T1B;void*_T1C;void*_T1D;struct _handler_cons*_T1E;int*_T1F;int _T20;struct Cyc_Absyn_Vardecl*_T21;struct _tuple0*_T22;struct Cyc_List_List*_T23;int _T24;struct _fat_ptr _T25;struct Cyc_List_List*_T26;void*_T27;struct _tuple11*_T28;struct _fat_ptr _T29;int _T2A;struct Cyc_List_List*_T2B;struct Cyc_List_List*_T2C;void*_T2D;struct Cyc_Absyn_Vardecl*_T2E;void*_T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;void*_T32;void*_T33;struct Cyc_Core_Not_found_exn_struct*_T34;char*_T35;char*_T36;struct Cyc_Warn_String_Warn_Warg_struct _T37;struct Cyc_Warn_Exp_Warn_Warg_struct _T38;struct Cyc_List_List*_T39;void*_T3A;void**_T3B;void**_T3C;int(*_T3D)(struct _fat_ptr);void*(*_T3E)(struct _fat_ptr);struct _fat_ptr _T3F;struct Cyc_Warn_String_Warn_Warg_struct _T40;struct Cyc_Warn_Exp_Warn_Warg_struct _T41;struct Cyc_List_List*_T42;void*_T43;void**_T44;void**_T45;int(*_T46)(struct _fat_ptr);void*(*_T47)(struct _fat_ptr);struct _fat_ptr _T48;struct _fat_ptr _T49;char*_T4A;unsigned _T4B;int _T4C;int(*_T4D)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T4E)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T4F;struct _fat_ptr _T50;int _T51;struct _tuple14 _T52;struct Cyc_List_List*_T53;unsigned _T54;struct Cyc_List_List*_T55;struct _tuple14 _T56;
struct Cyc_List_List*newdecls=0;
struct Cyc_List_List*located=0;
_TL4F: if(es!=0)goto _TL4D;else{goto _TL4E;}
_TL4D:{struct _fat_ptr filename=_tag_fat(0,0,0);_T0=es;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Exp*)_T1;{
void*_T57=_T2->r;void*_T58;_T3=(int*)_T57;_T4=*_T3;if(_T4!=1)goto _TL50;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T59=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T57;_T5=_T59->f1;_T58=(void*)_T5;}{void*b=_T58;{struct Cyc_Absyn_Vardecl*_T59;struct _tuple0*_T5A;_T6=b;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 0: _T9=b;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T5B=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T9;_T5A=_T5B->f1;}{struct _tuple0*q=_T5A;_TA=Cyc_Flags_no_merge;
# 265
if(!_TA)goto _TL53;{struct Cyc_Interproc_Unresolved_sym_exn_struct*_T5B=_cycalloc(sizeof(struct Cyc_Interproc_Unresolved_sym_exn_struct));_T5B->tag=Cyc_Interproc_Unresolved_sym;_TC=es;_TD=_TC->hd;
_T5B->f1=(struct Cyc_Absyn_Exp*)_TD;_TB=(struct Cyc_Interproc_Unresolved_sym_exn_struct*)_T5B;}_TE=(void*)_TB;_throw(_TE);goto _TL54;_TL53: _TL54:{struct _handler_cons _T5B;_TF=& _T5B;_push_handler(_TF);{int _T5C=0;_T10=_T5B.handler;_T11=setjmp(_T10);if(!_T11)goto _TL55;_T5C=1;goto _TL56;_TL55: _TL56: if(_T5C)goto _TL57;else{goto _TL59;}_TL59:
# 268
 filename=Cyc_Interproc_locate_qvar(q,ftds);{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));_T13=es;_T14=_T13->hd;
_T5D->hd=(struct Cyc_Absyn_Exp*)_T14;_T5D->tl=located;_T12=(struct Cyc_List_List*)_T5D;}located=_T12;_pop_handler();goto _TL58;_TL57: _T15=Cyc_Core_get_exn_thrown();{void*_T5D=(void*)_T15;void*_T5E;_T16=(struct Cyc_Core_Not_found_exn_struct*)_T5D;_T17=_T16->tag;_T18=Cyc_Core_Not_found;if(_T17!=_T18)goto _TL5A;{struct Cyc_Interproc_Unresolved_sym_exn_struct*_T5F=_cycalloc(sizeof(struct Cyc_Interproc_Unresolved_sym_exn_struct));_T5F->tag=Cyc_Interproc_Unresolved_sym;_T1A=es;_T1B=_T1A->hd;
# 271
_T5F->f1=(struct Cyc_Absyn_Exp*)_T1B;_T19=(struct Cyc_Interproc_Unresolved_sym_exn_struct*)_T5F;}_T1C=(void*)_T19;_throw(_T1C);goto _TL5B;_TL5A: _T5E=_T5D;{void*exn=_T5E;_rethrow(exn);}_TL5B:;}_TL58:;}}goto _LL5;}case 1: _T1D=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T5B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T1D;_T59=_T5B->f1;}{struct Cyc_Absyn_Vardecl*vd=_T59;{struct _handler_cons _T5B;_T1E=& _T5B;_push_handler(_T1E);{int _T5C=0;_T1F=_T5B.handler;_T20=setjmp(_T1F);if(!_T20)goto _TL5C;_T5C=1;goto _TL5D;_TL5C: _TL5D: if(_T5C)goto _TL5E;else{goto _TL60;}_TL60: _T21=vd;_T22=_T21->name;_T23=ftds;
# 276
filename=Cyc_Interproc_locate_qvar(_T22,_T23);_T24=Cyc_Flags_no_merge;
if(!_T24)goto _TL61;_T25=filename;_T26=
_check_null(ftds);_T27=_T26->hd;_T28=(struct _tuple11*)_T27;_T29=_T28->f0;_T2A=Cyc_strcmp(_T25,_T29);if(_T2A)goto _TL63;else{goto _TL65;}
# 280
_TL65:{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));_T2C=es;_T2D=_T2C->hd;_T5D->hd=(struct Cyc_Absyn_Exp*)_T2D;_T5D->tl=located;_T2B=(struct Cyc_List_List*)_T5D;}located=_T2B;_npop_handler(0);goto _TL4C;
# 287
_TL63: _T2E=vd;_T2F=_T2E->type;Cyc_Tcutil_introduce_cvars(_T2F);
filename=_tag_fat(0,0,0);_npop_handler(0);goto _TL4C;
# 293
_TL61:{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));_T31=es;_T32=_T31->hd;_T5D->hd=(struct Cyc_Absyn_Exp*)_T32;_T5D->tl=located;_T30=(struct Cyc_List_List*)_T5D;}located=_T30;_pop_handler();goto _TL5F;_TL5E: _T33=Cyc_Core_get_exn_thrown();{void*_T5D=(void*)_T33;void*_T5E;_T34=(struct Cyc_Core_Not_found_exn_struct*)_T5D;_T35=_T34->tag;_T36=Cyc_Core_Not_found;if(_T35!=_T36)goto _TL66;goto _TL4C;_TL66: _T5E=_T5D;{void*exn=_T5E;_rethrow(exn);};}_TL5F:;}}goto _LL5;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T5B;_T5B.tag=0;
# 303
_T5B.f1=_tag_fat("locate_externs: Unexpected expression",sizeof(char),38U);_T37=_T5B;}{struct Cyc_Warn_String_Warn_Warg_struct _T5B=_T37;{struct Cyc_Warn_Exp_Warn_Warg_struct _T5C;_T5C.tag=4;_T39=es;_T3A=_T39->hd;_T5C.f1=(struct Cyc_Absyn_Exp*)_T3A;_T38=_T5C;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T5C=_T38;void*_T5D[2];_T3B=_T5D + 0;*_T3B=& _T5B;_T3C=_T5D + 1;*_T3C=& _T5C;_T3E=Cyc_Warn_impos2;{int(*_T5E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T3E;_T3D=_T5E;}_T3F=_tag_fat(_T5D,sizeof(void*),2);_T3D(_T3F);}}goto _LL5;}_LL5:;}goto _LL0;}_TL50:{struct Cyc_Warn_String_Warn_Warg_struct _T59;_T59.tag=0;
# 308
_T59.f1=_tag_fat("locate_externs: Unexpected expression",sizeof(char),38U);_T40=_T59;}{struct Cyc_Warn_String_Warn_Warg_struct _T59=_T40;{struct Cyc_Warn_Exp_Warn_Warg_struct _T5A;_T5A.tag=4;_T42=es;_T43=_T42->hd;_T5A.f1=(struct Cyc_Absyn_Exp*)_T43;_T41=_T5A;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T5A=_T41;void*_T5B[2];_T44=_T5B + 0;*_T44=& _T59;_T45=_T5B + 1;*_T45=& _T5A;_T47=Cyc_Warn_impos2;{int(*_T5C)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T47;_T46=_T5C;}_T48=_tag_fat(_T5B,sizeof(void*),2);_T46(_T48);}}goto _LL0;_LL0:;}_T49=filename;_T4A=_T49.curr;_T4B=(unsigned)_T4A;
# 312
if(!_T4B)goto _TL68;_T4C=Cyc_Flags_no_merge;if(_T4C)goto _TL68;else{goto _TL6A;}_TL6A: goto _TL69;_TL68: _T4E=Cyc___assert_fail;{int(*_T57)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T4E;_T4D=_T57;}_T4F=_tag_fat("filename && !Flags::no_merge",sizeof(char),29U);_T50=_tag_fat("interproc.cyc",sizeof(char),14U);_T4D(_T4F,_T50,312U);_TL69: {
struct _tuple13 _T57=Cyc_Interproc_add_filedecls(filename,ftds);struct Cyc_List_List*_T58;int _T59;_T59=_T57.f0;_T58=_T57.f1;{int b=_T59;struct Cyc_List_List*newl=_T58;_T51=b;
if(_T51)goto _TL6B;else{goto _TL6D;}
_TL6D:{struct _tuple14 _T5A;_T5A.f0=0;_T5A.f1=0;_T5A.f2=0;_T52=_T5A;}return _T52;_TL6B: _T53=newdecls;_T54=(unsigned)_T53;
if(_T54)goto _TL6E;else{goto _TL70;}
_TL70: newdecls=newl;goto _TL6F;_TL6E: _TL6F:;}}}_TL4C: _T55=es;
# 259
es=_T55->tl;goto _TL4F;_TL4E:{struct _tuple14 _T57;
# 319
_T57.f0=1;_T57.f1=newdecls;_T57.f2=located;_T56=_T57;}return _T56;}
# 330 "interproc.cyc"
static void Cyc_Interproc_build_fndecl_tab(struct Cyc_Hashtable_Table*fdt,struct Cyc_List_List*decls){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;void(*_T7)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Fndecl*);void(*_T8)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T9;struct Cyc_Absyn_Fndecl*_TA;struct _tuple0*_TB;struct Cyc_Absyn_Fndecl*_TC;struct Cyc_List_List*_TD;
struct Cyc_List_List*ds=decls;_TL74: _T0=ds;_T1=(unsigned)_T0;if(_T1)goto _TL72;else{goto _TL73;}
_TL72: _T2=ds;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_TE=_T4->r;struct Cyc_List_List*_TF;struct Cyc_Absyn_Fndecl*_T10;_T5=(int*)_TE;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_TE;_T10=_T11->f1;}{struct Cyc_Absyn_Fndecl*fd=_T10;_T8=Cyc_Hashtable_insert;{
# 334
void(*_T11)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Fndecl*)=(void(*)(struct Cyc_Hashtable_Table*,struct _tuple0*,struct Cyc_Absyn_Fndecl*))_T8;_T7=_T11;}_T9=fdt;_TA=fd;_TB=_TA->name;_TC=fd;_T7(_T9,_TB,_TC);goto _LL0;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_TE;_TF=_T11->f2;}{struct Cyc_List_List*l=_TF;_TF=l;goto _LL6;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_TE;_TF=_T11->f2;}_LL6: {struct Cyc_List_List*l=_TF;
# 338
Cyc_Interproc_build_fndecl_tab(fdt,l);goto _LL0;}default: goto _LL0;}_LL0:;}_TD=ds;
# 331
ds=_TD->tl;goto _TL74;_TL73:;}
# 345
static void Cyc_Interproc_remove_cvars(void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_PtrInfo _T4;void*_T5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T6;void**_T7;void**_T8;void*_T9;int _TA;void**_TB;void*_TC;void*_TD;struct Cyc_Absyn_FnType_Absyn_Type_struct*_TE;struct Cyc_Absyn_FnInfo*_TF;struct Cyc_Absyn_FnInfo*_T10;void*_T11;struct Cyc_Absyn_FnInfo*_T12;struct Cyc_List_List*_T13;unsigned _T14;struct Cyc_List_List*_T15;void*_T16;struct _tuple8*_T17;void*_T18;struct Cyc_List_List*_T19;void*_T1A;void*_T1B;struct Cyc_List_List*_T1C;unsigned _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T22;int _T23;void*_T24;struct Cyc_List_List*_T25;unsigned _T26;struct Cyc_List_List*_T27;void*_T28;struct Cyc_Absyn_Aggrfield*_T29;void*_T2A;struct Cyc_List_List*_T2B;struct Cyc_List_List*_T2C;void*_T2D;void*_T2E;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 4: _T3=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T3;_T4=_T2F->f1;_T2E=_T4.elt_type;_T5=t;_T6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T5;_T7=& _T6->f1.ptr_atts.bounds;_T2D=(void**)_T7;}{void*ta=_T2E;void**b=(void**)_T2D;_T8=b;_T9=*_T8;_TA=
# 348
Cyc_Tcutil_is_cvar_type(_T9);if(!_TA)goto _TL77;_TB=b;
*_TB=Cyc_Absyn_bounds_one();goto _TL78;_TL77: _TL78:
# 351
 Cyc_Interproc_remove_cvars(ta);
return;}case 6: _TC=t;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TC;_TD=t;_TE=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_TD;_TF=& _TE->f1;_T2E=(struct Cyc_Absyn_FnInfo*)_TF;}{struct Cyc_Absyn_FnInfo*i=(struct Cyc_Absyn_FnInfo*)_T2E;_T10=i;_T11=_T10->ret_type;
# 354
Cyc_Interproc_remove_cvars(_T11);_T12=i;{
struct Cyc_List_List*args=_T12->args;_TL7C: _T13=args;_T14=(unsigned)_T13;if(_T14)goto _TL7A;else{goto _TL7B;}
_TL7A: _T15=args;_T16=_T15->hd;_T17=(struct _tuple8*)_T16;_T18=_T17->f2;Cyc_Interproc_remove_cvars(_T18);_T19=args;
# 355
args=_T19->tl;goto _TL7C;_TL7B:;}
# 358
return;}case 0: _T1A=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1A;_T2C=_T2F->f2;}{struct Cyc_List_List*ts=_T2C;_T2C=ts;goto _LL8;}case 8: _T1B=t;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T1B;_T2C=_T2F->f2;}_LL8: {struct Cyc_List_List*ts=_T2C;
# 361
_TL80: _T1C=ts;_T1D=(unsigned)_T1C;if(_T1D)goto _TL7E;else{goto _TL7F;}_TL7E: _T1E=ts;_T1F=_T1E->hd;Cyc_Interproc_remove_cvars(_T1F);_T20=ts;ts=_T20->tl;goto _TL80;_TL7F:
 return;}case 7: _T21=t;_T22=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T21;_T23=_T22->f2;if(_T23!=1)goto _TL81;_T24=t;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T2F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T24;_T2C=_T2F->f3;}{struct Cyc_List_List*fields=_T2C;
# 364
_TL86: _T25=fields;_T26=(unsigned)_T25;if(_T26)goto _TL84;else{goto _TL85;}
_TL84: _T27=fields;_T28=_T27->hd;_T29=(struct Cyc_Absyn_Aggrfield*)_T28;_T2A=_T29->type;Cyc_Interproc_remove_cvars(_T2A);_T2B=fields;
# 364
fields=_T2B->tl;goto _TL86;_TL85:
# 367
 return;}_TL81: goto _LLB;default: _LLB:
# 369
 return;};}struct _tuple15{void*f0;void*f1;};
# 374
static void Cyc_Interproc_ensure_cvar_identity(unsigned loc,void*dest,void*src){struct _tuple15 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;int _T6;void*_T7;struct Cyc_Absyn_PtrInfo _T8;void*_T9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TA;void**_TB;void*_TC;struct Cyc_Absyn_PtrInfo _TD;void*_TE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TF;void**_T10;void**_T11;void*_T12;int _T13;void**_T14;void**_T15;void*_T16;int*_T17;int _T18;void*_T19;void*_T1A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1B;struct Cyc_Absyn_FnInfo*_T1C;void*_T1D;void*_T1E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1F;struct Cyc_Absyn_FnInfo*_T20;unsigned _T21;struct Cyc_Absyn_FnInfo*_T22;void*_T23;struct Cyc_Absyn_FnInfo*_T24;void*_T25;struct Cyc_Absyn_FnInfo*_T26;struct Cyc_List_List*_T27;int _T28;struct Cyc_Absyn_FnInfo*_T29;struct Cyc_List_List*_T2A;int _T2B;struct Cyc_Interproc_Rebinding_exn_struct*_T2C;struct Cyc_Interproc_Rebinding_exn_struct*_T2D;struct Cyc_Absyn_FnInfo*_T2E;struct Cyc_Absyn_FnInfo*_T2F;struct Cyc_List_List*_T30;unsigned _T31;struct Cyc_List_List*_T32;unsigned _T33;unsigned _T34;struct Cyc_List_List*_T35;void*_T36;struct _tuple8*_T37;void*_T38;struct Cyc_List_List*_T39;void*_T3A;struct _tuple8*_T3B;void*_T3C;struct Cyc_List_List*_T3D;struct Cyc_List_List*_T3E;void*_T3F;int*_T40;int _T41;void*_T42;void*_T43;int _T44;int _T45;struct Cyc_Warn_String_Warn_Warg_struct _T46;struct Cyc_Warn_Typ_Warn_Warg_struct _T47;void**_T48;void**_T49;unsigned _T4A;struct _fat_ptr _T4B;struct Cyc_List_List*_T4C;unsigned _T4D;struct Cyc_List_List*_T4E;void*_T4F;struct Cyc_List_List*_T50;void*_T51;int*_T52;int _T53;void*_T54;void*_T55;int _T56;int _T57;struct Cyc_Interproc_Rebinding_exn_struct*_T58;struct Cyc_Interproc_Rebinding_exn_struct*_T59;struct Cyc_List_List*_T5A;unsigned _T5B;struct Cyc_List_List*_T5C;unsigned _T5D;unsigned _T5E;struct Cyc_List_List*_T5F;void*_T60;struct Cyc_List_List*_T61;void*_T62;struct Cyc_List_List*_T63;struct Cyc_List_List*_T64;void*_T65;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T66;int _T67;void*_T68;int*_T69;int _T6A;void*_T6B;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T6C;int _T6D;void*_T6E;void*_T6F;int _T70;int _T71;struct Cyc_Interproc_Rebinding_exn_struct*_T72;struct Cyc_Interproc_Rebinding_exn_struct*_T73;struct Cyc_List_List*_T74;unsigned _T75;struct Cyc_List_List*_T76;unsigned _T77;unsigned _T78;struct Cyc_List_List*_T79;void*_T7A;struct Cyc_Absyn_Aggrfield*_T7B;void*_T7C;struct Cyc_List_List*_T7D;void*_T7E;struct Cyc_Absyn_Aggrfield*_T7F;void*_T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;{struct _tuple15 _T83;
_T83.f0=dest;_T83.f1=src;_T0=_T83;}{struct _tuple15 _T83=_T0;struct Cyc_List_List*_T84;struct _tuple0*_T85;struct Cyc_List_List*_T86;struct _tuple0*_T87;void*_T88;void*_T89;void*_T8A;void*_T8B;_T1=_T83.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 4: _T4=_T83.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=4)goto _TL88;_T7=_T83.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T7;_T8=_T8C->f1;_T8B=_T8.elt_type;_T9=_T83.f0;_TA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T9;_TB=& _TA->f1.ptr_atts.bounds;_T8A=(void**)_TB;}_TC=_T83.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TC;_TD=_T8C->f1;_T89=_TD.elt_type;_TE=_T83.f1;_TF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TE;_T10=& _TF->f1.ptr_atts.bounds;_T88=(void**)_T10;}{void*tD=_T8B;void**bD=_T8A;void*tS=_T89;void**bS=(void**)_T88;_T11=bS;_T12=*_T11;_T13=
# 377
Cyc_Tcutil_is_cvar_type(_T12);if(!_T13)goto _TL8A;_T14=bD;_T15=bS;
*_T14=*_T15;goto _TL8B;_TL8A: _TL8B:
 Cyc_Interproc_ensure_cvar_identity(loc,tD,tS);
return;}_TL88: goto _LLB;case 6: _T16=_T83.f1;_T17=(int*)_T16;_T18=*_T17;if(_T18!=6)goto _TL8C;_T19=_T83.f0;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T19;_T1A=_T83.f0;_T1B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1A;_T1C=& _T1B->f1;_T8B=(struct Cyc_Absyn_FnInfo*)_T1C;}_T1D=_T83.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1D;_T1E=_T83.f1;_T1F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1E;_T20=& _T1F->f1;_T8A=(struct Cyc_Absyn_FnInfo*)_T20;}{struct Cyc_Absyn_FnInfo*iD=(struct Cyc_Absyn_FnInfo*)_T8B;struct Cyc_Absyn_FnInfo*iS=(struct Cyc_Absyn_FnInfo*)_T8A;_T21=loc;_T22=iD;_T23=_T22->ret_type;_T24=iS;_T25=_T24->ret_type;
# 382
Cyc_Interproc_ensure_cvar_identity(_T21,_T23,_T25);_T26=iD;_T27=_T26->args;_T28=
Cyc_List_length(_T27);_T29=iS;_T2A=_T29->args;_T2B=Cyc_List_length(_T2A);if(_T28==_T2B)goto _TL8E;_T2C=& Cyc_Interproc_Rebinding_val;_T2D=(struct Cyc_Interproc_Rebinding_exn_struct*)_T2C;_throw(_T2D);goto _TL8F;_TL8E: _TL8F: _T2E=iD;{
# 385
struct Cyc_List_List*argD=_T2E->args;_T2F=iS;{struct Cyc_List_List*argS=_T2F->args;
_TL93: _T30=argD;_T31=(unsigned)_T30;
# 385
if(_T31)goto _TL94;else{goto _TL92;}_TL94: _T32=argS;_T33=(unsigned)_T32;if(_T33)goto _TL91;else{goto _TL92;}
# 387
_TL91: _T34=loc;_T35=argD;_T36=_T35->hd;_T37=(struct _tuple8*)_T36;_T38=_T37->f2;_T39=argS;_T3A=_T39->hd;_T3B=(struct _tuple8*)_T3A;_T3C=_T3B->f2;Cyc_Interproc_ensure_cvar_identity(_T34,_T38,_T3C);_T3D=argD;
# 386
argD=_T3D->tl;_T3E=argS;argS=_T3E->tl;goto _TL93;_TL92:;}}
# 389
return;}_TL8C: goto _LLB;case 8: _T3F=_T83.f1;_T40=(int*)_T3F;_T41=*_T40;if(_T41!=8)goto _TL95;_T42=_T83.f0;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T42;_T87=_T8C->f1;_T86=_T8C->f2;}_T43=_T83.f1;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T43;_T85=_T8C->f1;_T84=_T8C->f2;}{struct _tuple0*nameD=_T87;struct Cyc_List_List*tD=_T86;struct _tuple0*nameS=_T85;struct Cyc_List_List*tS=_T84;_T44=
# 391
Cyc_List_length(tD);_T45=Cyc_List_length(tS);if(_T44==_T45)goto _TL97;{struct Cyc_Warn_String_Warn_Warg_struct _T8C;_T8C.tag=0;
# 396
_T8C.f1=_tag_fat("Typedef names could not be resolved yet; removing any constraint variables from ",sizeof(char),81U);_T46=_T8C;}{struct Cyc_Warn_String_Warn_Warg_struct _T8C=_T46;{struct Cyc_Warn_Typ_Warn_Warg_struct _T8D;_T8D.tag=2;_T8D.f1=src;_T47=_T8D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T8D=_T47;void*_T8E[2];_T48=_T8E + 0;*_T48=& _T8C;_T49=_T8E + 1;*_T49=& _T8D;_T4A=loc;_T4B=_tag_fat(_T8E,sizeof(void*),2);Cyc_Warn_warn2(_T4A,_T4B);}}
_TL9C: _T4C=tS;_T4D=(unsigned)_T4C;if(_T4D)goto _TL9A;else{goto _TL9B;}
_TL9A: _T4E=tS;_T4F=_T4E->hd;Cyc_Interproc_remove_cvars(_T4F);_T50=tS;
# 397
tS=_T50->tl;goto _TL9C;_TL9B:
# 399
 return;_TL97: _T86=tD;_T84=tS;goto _LL8;}_TL95: goto _LLB;case 0: _T51=_T83.f1;_T52=(int*)_T51;_T53=*_T52;if(_T53!=0)goto _TL9D;_T54=_T83.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T54;_T86=_T8C->f2;}_T55=_T83.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T55;_T84=_T8C->f2;}_LL8:{struct Cyc_List_List*tD=_T86;struct Cyc_List_List*tS=_T84;_T56=
# 403
Cyc_List_length(tD);_T57=Cyc_List_length(tS);if(_T56==_T57)goto _TL9F;_T58=& Cyc_Interproc_Rebinding_val;_T59=(struct Cyc_Interproc_Rebinding_exn_struct*)_T58;_throw(_T59);goto _TLA0;_TL9F: _TLA0:
# 405
 _TLA4: _T5A=tD;_T5B=(unsigned)_T5A;if(_T5B)goto _TLA5;else{goto _TLA3;}_TLA5: _T5C=tS;_T5D=(unsigned)_T5C;if(_T5D)goto _TLA2;else{goto _TLA3;}
_TLA2: _T5E=loc;_T5F=tD;_T60=_T5F->hd;_T61=tS;_T62=_T61->hd;Cyc_Interproc_ensure_cvar_identity(_T5E,_T60,_T62);_T63=tD;
# 405
tD=_T63->tl;_T64=tS;tS=_T64->tl;goto _TLA4;_TLA3:
# 407
 return;}_TL9D: goto _LLB;case 7: _T65=_T83.f0;_T66=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T65;_T67=_T66->f2;if(_T67!=1)goto _TLA6;_T68=_T83.f1;_T69=(int*)_T68;_T6A=*_T69;if(_T6A!=7)goto _TLA8;_T6B=_T83.f1;_T6C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T6B;_T6D=_T6C->f2;if(_T6D!=1)goto _TLAA;_T6E=_T83.f0;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T6E;_T86=_T8C->f3;}_T6F=_T83.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T8C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T6F;_T84=_T8C->f3;}{struct Cyc_List_List*fieldD=_T86;struct Cyc_List_List*fieldS=_T84;_T70=
# 409
Cyc_List_length(fieldD);_T71=Cyc_List_length(fieldS);if(_T70==_T71)goto _TLAC;_T72=& Cyc_Interproc_Rebinding_val;_T73=(struct Cyc_Interproc_Rebinding_exn_struct*)_T72;_throw(_T73);goto _TLAD;_TLAC: _TLAD:
# 411
 _TLB1: _T74=fieldD;_T75=(unsigned)_T74;if(_T75)goto _TLB2;else{goto _TLB0;}_TLB2: _T76=fieldS;_T77=(unsigned)_T76;if(_T77)goto _TLAF;else{goto _TLB0;}
_TLAF: _T78=loc;_T79=fieldD;_T7A=_T79->hd;_T7B=(struct Cyc_Absyn_Aggrfield*)_T7A;_T7C=_T7B->type;_T7D=fieldS;_T7E=_T7D->hd;_T7F=(struct Cyc_Absyn_Aggrfield*)_T7E;_T80=_T7F->type;Cyc_Interproc_ensure_cvar_identity(_T78,_T7C,_T80);_T81=fieldD;
# 411
fieldD=_T81->tl;_T82=fieldS;fieldS=_T82->tl;goto _TLB1;_TLB0:
# 413
 return;}_TLAA: goto _LLB;_TLA8: goto _LLB;_TLA6: goto _LLB;default: _LLB:
# 415
 return;};}}
# 421
static void Cyc_Interproc_imp_update_prototypes(struct Cyc_List_List*decls,struct Cyc_Hashtable_Table*fndecl_tab){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct _handler_cons*_T7;int*_T8;int _T9;struct Cyc_Absyn_Fndecl*(*_TA)(struct Cyc_Hashtable_Table*,struct _tuple0*);void*(*_TB)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TC;struct Cyc_Absyn_Vardecl*_TD;struct _tuple0*_TE;void*_TF;struct Cyc_Core_Not_found_exn_struct*_T10;char*_T11;char*_T12;struct _handler_cons*_T13;int*_T14;int _T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_Absyn_Decl*_T18;unsigned _T19;struct Cyc_Absyn_Vardecl*_T1A;void*_T1B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1C;struct Cyc_Absyn_Fndecl*_T1D;void*_T1E;void*_T1F;struct Cyc_Interproc_Rebinding_exn_struct*_T20;char*_T21;char*_T22;struct Cyc_Warn_String_Warn_Warg_struct _T23;struct Cyc_Warn_Typ_Warn_Warg_struct _T24;struct Cyc_Absyn_Vardecl*_T25;struct Cyc_Warn_Typ_Warn_Warg_struct _T26;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T27;struct Cyc_Absyn_Fndecl*_T28;void**_T29;void**_T2A;void**_T2B;struct Cyc_List_List*_T2C;void*_T2D;struct Cyc_Absyn_Decl*_T2E;unsigned _T2F;struct _fat_ptr _T30;struct Cyc_List_List*_T31;
# 423
_TLB6: _T0=decls;_T1=(unsigned)_T0;if(_T1)goto _TLB4;else{goto _TLB5;}
_TLB4: _T2=decls;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T32=_T4->r;struct Cyc_List_List*_T33;struct Cyc_Absyn_Vardecl*_T34;_T5=(int*)_T32;_T6=*_T5;switch(_T6){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T35=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T32;_T34=_T35->f1;}{struct Cyc_Absyn_Vardecl*vd=_T34;
# 426
struct Cyc_Absyn_Fndecl*fd;{struct _handler_cons _T35;_T7=& _T35;_push_handler(_T7);{int _T36=0;_T8=_T35.handler;_T9=setjmp(_T8);if(!_T9)goto _TLB8;_T36=1;goto _TLB9;_TLB8: _TLB9: if(_T36)goto _TLBA;else{goto _TLBC;}_TLBC: _TB=Cyc_Hashtable_lookup;{
# 428
struct Cyc_Absyn_Fndecl*(*_T37)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(struct Cyc_Absyn_Fndecl*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))_TB;_TA=_T37;}_TC=fndecl_tab;_TD=vd;_TE=_TD->name;fd=_TA(_TC,_TE);_pop_handler();goto _TLBB;_TLBA: _TF=Cyc_Core_get_exn_thrown();{void*_T37=(void*)_TF;void*_T38;_T10=(struct Cyc_Core_Not_found_exn_struct*)_T37;_T11=_T10->tag;_T12=Cyc_Core_Not_found;if(_T11!=_T12)goto _TLBD;goto _TLB3;_TLBD: _T38=_T37;{void*exn=_T38;_rethrow(exn);};}_TLBB:;}}{struct _handler_cons _T35;_T13=& _T35;_push_handler(_T13);{int _T36=0;_T14=_T35.handler;_T15=setjmp(_T14);if(!_T15)goto _TLBF;_T36=1;goto _TLC0;_TLBF: _TLC0: if(_T36)goto _TLC1;else{goto _TLC3;}_TLC3: _T16=decls;_T17=_T16->hd;_T18=(struct Cyc_Absyn_Decl*)_T17;_T19=_T18->loc;_T1A=vd;_T1B=_T1A->type;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T37=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T37->tag=6;_T1D=fd;
# 434
_T37->f1=_T1D->i;_T1C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T37;}_T1E=(void*)_T1C;Cyc_Interproc_ensure_cvar_identity(_T19,_T1B,_T1E);_pop_handler();goto _TLC2;_TLC1: _T1F=Cyc_Core_get_exn_thrown();{void*_T37=(void*)_T1F;void*_T38;_T20=(struct Cyc_Interproc_Rebinding_exn_struct*)_T37;_T21=_T20->tag;_T22=Cyc_Interproc_Rebinding;if(_T21!=_T22)goto _TLC4;{struct Cyc_Warn_String_Warn_Warg_struct _T39;_T39.tag=0;
# 437
_T39.f1=_tag_fat("Incompatible types in declarations: ",sizeof(char),37U);_T23=_T39;}{struct Cyc_Warn_String_Warn_Warg_struct _T39=_T23;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3A;_T3A.tag=2;_T25=vd;_T3A.f1=_T25->type;_T24=_T3A;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3A=_T24;{struct Cyc_Warn_Typ_Warn_Warg_struct _T3B;_T3B.tag=2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T3C=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T3C->tag=6;_T28=fd;_T3C->f1=_T28->i;_T27=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T3C;}_T3B.f1=(void*)_T27;_T26=_T3B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T3B=_T26;void*_T3C[3];_T29=_T3C + 0;*_T29=& _T39;_T2A=_T3C + 1;*_T2A=& _T3A;_T2B=_T3C + 2;*_T2B=& _T3B;_T2C=decls;_T2D=_T2C->hd;_T2E=(struct Cyc_Absyn_Decl*)_T2D;_T2F=_T2E->loc;_T30=_tag_fat(_T3C,sizeof(void*),3);Cyc_Warn_err2(_T2F,_T30);}}}goto _LLE;_TLC4: _T38=_T37;{void*exn=_T38;_rethrow(exn);}_LLE:;}_TLC2:;}}goto _LL0;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T35=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T32;_T33=_T35->f2;}{struct Cyc_List_List*l=_T33;_T33=l;goto _LL6;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T35=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T32;_T33=_T35->f2;}_LL6: {struct Cyc_List_List*l=_T33;
# 443
Cyc_Interproc_imp_update_prototypes(l,fndecl_tab);goto _LL0;}default: goto _LL0;}_LL0:;}_TLB3: _T31=decls;
# 423
decls=_T31->tl;goto _TLB6;_TLB5:;}
# 450
static struct Cyc_List_List*Cyc_Interproc_update_prototype_cvars(struct Cyc_List_List*exts,struct Cyc_List_List*decls){struct Cyc_Hashtable_Table*(*_T0)(struct _RegionHandle*,int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*));struct Cyc_Hashtable_Table*(*_T1)(struct _RegionHandle*,int,int(*)(void*,void*),int(*)(void*));struct _RegionHandle*_T2;int(*_T3)(struct _tuple0*,struct _tuple0*);int(*_T4)(struct _tuple0*);struct _RegionHandle _T5=_new_region(0U,"r");struct _RegionHandle*r=& _T5;_push_region(r);_T1=Cyc_Hashtable_rcreate;{
# 452
struct Cyc_Hashtable_Table*(*_T6)(struct _RegionHandle*,int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*))=(struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*,int,int(*)(struct _tuple0*,struct _tuple0*),int(*)(struct _tuple0*)))_T1;_T0=_T6;}_T2=r;_T3=Cyc_Absyn_qvar_cmp;_T4=Cyc_Absyn_hash_qvar;{struct Cyc_Hashtable_Table*fndecl_tab=_T0(_T2,101,_T3,_T4);
Cyc_Interproc_build_fndecl_tab(fndecl_tab,decls);
# 455
Cyc_Interproc_imp_update_prototypes(decls,fndecl_tab);{struct Cyc_List_List*_T6=decls;_npop_handler(0);return _T6;}}_pop_region();}
# 470 "interproc.cyc"
static char Cyc_Interproc__prefix[1024U];
static int Cyc_Interproc_myhash_stringptr(struct _fat_ptr*a){struct _fat_ptr*_T0;struct _fat_ptr _T1;int _T2;_T0=a;_T1=*_T0;_T2=
Cyc_Hashtable_hash_string(_T1);return _T2;}
# 474
static int Cyc_Interproc_mystrptrcmp(struct _fat_ptr*a,struct _fat_ptr*b){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;int _T4;_T0=a;_T1=*_T0;_T2=b;_T3=*_T2;_T4=
Cyc_strcmp(_T1,_T3);return _T4;}
# 477
static void Cyc_Interproc_print_strptr(struct _fat_ptr*a){struct Cyc_String_pa_PrintArg_struct _T0;struct _fat_ptr*_T1;struct Cyc_Int_pa_PrintArg_struct _T2;int _T3;void**_T4;void**_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;_T1=a;
_T8.f1=*_T1;_T0=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T0;{struct Cyc_Int_pa_PrintArg_struct _T9;_T9.tag=1;_T3=Cyc_Hashtable_hash_stringptr(a);_T9.f1=(unsigned long)_T3;_T2=_T9;}{struct Cyc_Int_pa_PrintArg_struct _T9=_T2;void*_TA[2];_T4=_TA + 0;*_T4=& _T8;_T5=_TA + 1;*_T5=& _T9;_T6=_tag_fat("%s(%d)",sizeof(char),7U);_T7=_tag_fat(_TA,sizeof(void*),2);Cyc_printf(_T6,_T7);}}}
# 480
static void Cyc_Interproc_print_type(void*a){struct Cyc_String_pa_PrintArg_struct _T0;void**_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;{struct Cyc_String_pa_PrintArg_struct _T4;_T4.tag=0;
_T4.f1=Cyc_Absynpp_typ2string(a);_T0=_T4;}{struct Cyc_String_pa_PrintArg_struct _T4=_T0;void*_T5[1];_T1=_T5 + 0;*_T1=& _T4;_T2=_tag_fat("%s",sizeof(char),3U);_T3=_tag_fat(_T5,sizeof(void*),1);Cyc_printf(_T2,_T3);}}
# 483
static void Cyc_Interproc_dump_table(struct Cyc_Hashtable_Table*cvmap){void(*_T0)(struct Cyc_Hashtable_Table*,void(*)(struct _fat_ptr*),void(*)(void*));void(*_T1)(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(void*));struct Cyc_Hashtable_Table*_T2;_T1=Cyc_Hashtable_print_table_map;{
void(*_T3)(struct Cyc_Hashtable_Table*,void(*)(struct _fat_ptr*),void(*)(void*))=(void(*)(struct Cyc_Hashtable_Table*,void(*)(struct _fat_ptr*),void(*)(void*)))_T1;_T0=_T3;}_T2=cvmap;_T0(_T2,Cyc_Interproc_print_strptr,Cyc_Interproc_print_type);}struct _tuple16{struct Cyc_Absyn_Tqual f0;void*f1;};
# 489
static struct Cyc_List_List*Cyc_Interproc_cvars_in_type(void*t,int index,int is_local,int pfxpos){int(*_T0)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T1)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Hashtable_Table*_T4;unsigned _T5;struct Cyc_Hashtable_Table*(*_T6)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T7)(int,int(*)(void*,void*),int(*)(void*));int(*_T8)(struct _fat_ptr*,struct _fat_ptr*);int(*_T9)(struct _fat_ptr*);int*_TA;unsigned _TB;struct Cyc_Absyn_PtrInfo _TC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TD;void**_TE;void*_TF;int _T10;int _T11;int _T12;void**_T13;int*_T14;int _T15;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T16;int*_T17;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T18;const char**_T19;int*_T1A;int _T1B;char*_T1C;struct _fat_ptr _T1D;int _T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_Int_pa_PrintArg_struct _T22;int _T23;void**_T24;struct _fat_ptr _T25;struct _fat_ptr _T26;struct Cyc_String_pa_PrintArg_struct _T27;char*_T28;void**_T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;void**(*_T2C)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));void**(*_T2D)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_Hashtable_Table*_T2E;struct _fat_ptr*_T2F;struct _fat_ptr*_T30;int(*_T31)(struct _fat_ptr*,struct _fat_ptr*);int(*_T32)(struct _fat_ptr*);void**_T33;unsigned _T34;void**_T35;void**_T36;const char**_T37;struct _fat_ptr _T38;char*_T39;struct Cyc_List_List*_T3A;void**_T3B;int _T3C;int*_T3D;int*_T3E;int _T3F;int*_T40;void(*_T41)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*);void(*_T42)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T43;struct _fat_ptr*_T44;void**_T45;void*_T46;char*_T47;struct _fat_ptr _T48;int _T49;char*_T4A;char*_T4B;unsigned _T4C;unsigned char*_T4D;char*_T4E;struct Cyc_Absyn_ArrayInfo _T4F;void*_T50;int _T51;int _T52;int _T53;struct Cyc_List_List*_T54;unsigned _T55;struct Cyc_List_List*_T56;void*_T57;char*_T58;struct _fat_ptr _T59;int _T5A;struct _fat_ptr _T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;struct Cyc_Int_pa_PrintArg_struct _T5E;int _T5F;struct Cyc_Int_pa_PrintArg_struct _T60;int _T61;int _T62;void**_T63;void**_T64;struct _fat_ptr _T65;void*_T66;int _T67;char*_T68;struct _fat_ptr _T69;unsigned long _T6A;int _T6B;struct Cyc_List_List*_T6C;struct Cyc_List_List*_T6D;struct Cyc_List_List*_T6E;char*_T6F;struct _fat_ptr _T70;int _T71;char*_T72;char*_T73;unsigned _T74;unsigned char*_T75;char*_T76;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T77;void*_T78;int*_T79;unsigned _T7A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7B;void*_T7C;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T7D;union Cyc_Absyn_DatatypeInfo _T7E;struct _union_DatatypeInfo_KnownDatatype _T7F;unsigned _T80;void*_T81;union Cyc_Absyn_DatatypeInfo _T82;struct _union_DatatypeInfo_KnownDatatype _T83;struct Cyc_Absyn_Datatypedecl**_T84;struct Cyc_Absyn_Datatypedecl*_T85;struct Cyc_Core_Opt*_T86;unsigned _T87;struct Cyc_Absyn_Datatypedecl*_T88;struct Cyc_Core_Opt*_T89;void*_T8A;struct Cyc_Absyn_Datatypedecl*_T8B;struct _tuple0*_T8C;struct Cyc_List_List*_T8D;unsigned _T8E;struct Cyc_List_List*_T8F;void*_T90;struct Cyc_Absyn_Datatypefield*_T91;struct Cyc_List_List*_T92;void*_T93;struct Cyc_Absyn_Datatypefield*_T94;struct _tuple0*_T95;struct Cyc_List_List*_T96;unsigned _T97;struct Cyc_List_List*_T98;void*_T99;char*_T9A;struct _fat_ptr _T9B;int _T9C;struct _fat_ptr _T9D;struct _fat_ptr _T9E;struct _fat_ptr _T9F;struct Cyc_Int_pa_PrintArg_struct _TA0;struct Cyc_String_pa_PrintArg_struct _TA1;struct Cyc_Int_pa_PrintArg_struct _TA2;struct Cyc_String_pa_PrintArg_struct _TA3;struct Cyc_Int_pa_PrintArg_struct _TA4;int _TA5;int _TA6;void**_TA7;void**_TA8;void**_TA9;void**_TAA;void**_TAB;struct _fat_ptr _TAC;void*_TAD;int _TAE;char*_TAF;struct _fat_ptr _TB0;unsigned long _TB1;int _TB2;struct Cyc_List_List*_TB3;struct Cyc_List_List*_TB4;struct Cyc_List_List*_TB5;struct Cyc_List_List*_TB6;char*_TB7;struct _fat_ptr _TB8;int _TB9;char*_TBA;char*_TBB;unsigned _TBC;unsigned char*_TBD;char*_TBE;struct Cyc_Absyn_Datatypedecl*_TBF;void*_TC0;union Cyc_Absyn_DatatypeInfo _TC1;struct _union_DatatypeInfo_UnknownDatatype _TC2;struct Cyc_Absyn_UnknownDatatypeInfo _TC3;struct Cyc_List_List*_TC4;unsigned _TC5;struct Cyc_List_List*_TC6;unsigned _TC7;char*_TC8;struct _fat_ptr _TC9;int _TCA;struct _fat_ptr _TCB;struct _fat_ptr _TCC;struct _fat_ptr _TCD;struct Cyc_Int_pa_PrintArg_struct _TCE;int _TCF;struct Cyc_Int_pa_PrintArg_struct _TD0;struct Cyc_String_pa_PrintArg_struct _TD1;struct Cyc_Int_pa_PrintArg_struct _TD2;int _TD3;void**_TD4;void**_TD5;void**_TD6;void**_TD7;struct _fat_ptr _TD8;struct Cyc_List_List*_TD9;void*_TDA;int _TDB;char*_TDC;struct _fat_ptr _TDD;unsigned long _TDE;int _TDF;struct Cyc_List_List*_TE0;struct Cyc_List_List*_TE1;struct Cyc_List_List*_TE2;char*_TE3;struct _fat_ptr _TE4;int _TE5;char*_TE6;char*_TE7;unsigned _TE8;unsigned char*_TE9;char*_TEA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TEB;void*_TEC;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TED;union Cyc_Absyn_DatatypeFieldInfo _TEE;struct _union_DatatypeFieldInfo_KnownDatatypefield _TEF;unsigned _TF0;void*_TF1;union Cyc_Absyn_DatatypeFieldInfo _TF2;struct _union_DatatypeFieldInfo_KnownDatatypefield _TF3;struct _tuple1 _TF4;union Cyc_Absyn_DatatypeFieldInfo _TF5;struct _union_DatatypeFieldInfo_KnownDatatypefield _TF6;struct _tuple1 _TF7;int(*_TF8)(struct _fat_ptr,struct _fat_ptr);void*(*_TF9)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TFA;struct _fat_ptr _TFB;void*_TFC;union Cyc_Absyn_DatatypeFieldInfo _TFD;struct _union_DatatypeFieldInfo_UnknownDatatypefield _TFE;struct Cyc_Absyn_UnknownDatatypeFieldInfo _TFF;union Cyc_Absyn_DatatypeFieldInfo _T100;struct _union_DatatypeFieldInfo_UnknownDatatypefield _T101;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T102;struct Cyc_List_List*_T103;unsigned _T104;struct Cyc_List_List*_T105;unsigned _T106;char*_T107;struct _fat_ptr _T108;int _T109;struct _fat_ptr _T10A;struct _fat_ptr _T10B;struct _fat_ptr _T10C;struct Cyc_Int_pa_PrintArg_struct _T10D;int _T10E;struct Cyc_Int_pa_PrintArg_struct _T10F;struct Cyc_String_pa_PrintArg_struct _T110;struct Cyc_Int_pa_PrintArg_struct _T111;struct Cyc_String_pa_PrintArg_struct _T112;struct Cyc_Int_pa_PrintArg_struct _T113;int _T114;void**_T115;void**_T116;void**_T117;void**_T118;void**_T119;void**_T11A;struct _fat_ptr _T11B;struct Cyc_List_List*_T11C;void*_T11D;int _T11E;char*_T11F;struct _fat_ptr _T120;unsigned long _T121;int _T122;struct Cyc_List_List*_T123;struct Cyc_List_List*_T124;struct Cyc_List_List*_T125;char*_T126;struct _fat_ptr _T127;int _T128;char*_T129;char*_T12A;unsigned _T12B;unsigned char*_T12C;char*_T12D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T12E;void*_T12F;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T130;union Cyc_Absyn_AggrInfo _T131;struct _union_AggrInfo_KnownAggr _T132;unsigned _T133;void*_T134;union Cyc_Absyn_AggrInfo _T135;struct _union_AggrInfo_KnownAggr _T136;struct Cyc_Absyn_Aggrdecl**_T137;struct Cyc_Absyn_Aggrdecl*_T138;struct _tuple0*_T139;struct Cyc_Absyn_Aggrdecl*_T13A;struct Cyc_Absyn_AggrdeclImpl*_T13B;unsigned _T13C;struct Cyc_Absyn_Aggrdecl*_T13D;struct Cyc_Absyn_AggrdeclImpl*_T13E;struct Cyc_List_List*_T13F;unsigned _T140;char*_T141;struct _fat_ptr _T142;int _T143;struct _fat_ptr _T144;struct _fat_ptr _T145;struct _fat_ptr _T146;struct Cyc_Int_pa_PrintArg_struct _T147;struct Cyc_String_pa_PrintArg_struct _T148;struct Cyc_Int_pa_PrintArg_struct _T149;int _T14A;int _T14B;void**_T14C;void**_T14D;void**_T14E;struct _fat_ptr _T14F;struct Cyc_List_List*_T150;void*_T151;struct Cyc_Absyn_Aggrfield*_T152;void*_T153;int _T154;char*_T155;struct _fat_ptr _T156;unsigned long _T157;int _T158;struct Cyc_List_List*_T159;struct Cyc_List_List*_T15A;struct Cyc_List_List*_T15B;char*_T15C;struct _fat_ptr _T15D;int _T15E;char*_T15F;char*_T160;unsigned _T161;unsigned char*_T162;char*_T163;struct Cyc_Absyn_Aggrdecl*_T164;void*_T165;union Cyc_Absyn_AggrInfo _T166;struct _union_AggrInfo_UnknownAggr _T167;struct _tuple2 _T168;struct Cyc_List_List*_T169;unsigned _T16A;struct Cyc_List_List*_T16B;unsigned _T16C;char*_T16D;struct _fat_ptr _T16E;int _T16F;struct _fat_ptr _T170;struct _fat_ptr _T171;struct _fat_ptr _T172;struct Cyc_Int_pa_PrintArg_struct _T173;int _T174;struct Cyc_Int_pa_PrintArg_struct _T175;struct Cyc_String_pa_PrintArg_struct _T176;struct Cyc_Int_pa_PrintArg_struct _T177;int _T178;void**_T179;void**_T17A;void**_T17B;void**_T17C;struct _fat_ptr _T17D;struct Cyc_List_List*_T17E;void*_T17F;int _T180;char*_T181;struct _fat_ptr _T182;unsigned long _T183;int _T184;struct Cyc_List_List*_T185;struct Cyc_List_List*_T186;struct Cyc_List_List*_T187;char*_T188;struct _fat_ptr _T189;int _T18A;char*_T18B;char*_T18C;unsigned _T18D;unsigned char*_T18E;char*_T18F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T190;struct Cyc_Absyn_FnInfo*_T191;char*_T192;struct _fat_ptr _T193;int _T194;struct _fat_ptr _T195;struct _fat_ptr _T196;struct _fat_ptr _T197;struct Cyc_Int_pa_PrintArg_struct _T198;int _T199;void**_T19A;struct _fat_ptr _T19B;struct Cyc_Absyn_FnInfo*_T19C;void*_T19D;int _T19E;char*_T19F;struct _fat_ptr _T1A0;unsigned long _T1A1;int _T1A2;struct Cyc_List_List*_T1A3;struct Cyc_List_List*_T1A4;struct Cyc_Absyn_FnInfo*_T1A5;struct Cyc_List_List*_T1A6;unsigned _T1A7;struct Cyc_List_List*_T1A8;void*_T1A9;char*_T1AA;struct _fat_ptr _T1AB;int _T1AC;struct _fat_ptr _T1AD;struct _fat_ptr _T1AE;struct _fat_ptr _T1AF;struct Cyc_Int_pa_PrintArg_struct _T1B0;int _T1B1;struct Cyc_Int_pa_PrintArg_struct _T1B2;int _T1B3;int _T1B4;void**_T1B5;void**_T1B6;struct _fat_ptr _T1B7;void*_T1B8;int _T1B9;char*_T1BA;struct _fat_ptr _T1BB;unsigned long _T1BC;int _T1BD;struct Cyc_List_List*_T1BE;struct Cyc_List_List*_T1BF;struct Cyc_List_List*_T1C0;char*_T1C1;struct _fat_ptr _T1C2;int _T1C3;char*_T1C4;char*_T1C5;unsigned _T1C6;unsigned char*_T1C7;char*_T1C8;void*_T1C9;struct Cyc_Absyn_Typedefdecl*_T1CA;unsigned _T1CB;struct Cyc_Absyn_Typedefdecl*_T1CC;void*_T1CD;unsigned _T1CE;char*_T1CF;struct _fat_ptr _T1D0;int _T1D1;struct _fat_ptr _T1D2;struct _fat_ptr _T1D3;struct _fat_ptr _T1D4;struct Cyc_Int_pa_PrintArg_struct _T1D5;struct Cyc_String_pa_PrintArg_struct _T1D6;void**_T1D7;void**_T1D8;struct _fat_ptr _T1D9;struct Cyc_Absyn_Typedefdecl*_T1DA;void*_T1DB;void*_T1DC;int _T1DD;char*_T1DE;struct _fat_ptr _T1DF;unsigned long _T1E0;int _T1E1;struct Cyc_List_List*_T1E2;struct Cyc_List_List*_T1E3;char*_T1E4;struct _fat_ptr _T1E5;int _T1E6;char*_T1E7;char*_T1E8;unsigned _T1E9;unsigned char*_T1EA;char*_T1EB;struct Cyc_List_List*_T1EC;unsigned _T1ED;char*_T1EE;struct _fat_ptr _T1EF;int _T1F0;struct _fat_ptr _T1F1;struct _fat_ptr _T1F2;struct _fat_ptr _T1F3;struct Cyc_Int_pa_PrintArg_struct _T1F4;int _T1F5;struct Cyc_Int_pa_PrintArg_struct _T1F6;struct Cyc_String_pa_PrintArg_struct _T1F7;struct Cyc_Int_pa_PrintArg_struct _T1F8;int _T1F9;void**_T1FA;void**_T1FB;void**_T1FC;void**_T1FD;struct _fat_ptr _T1FE;struct Cyc_List_List*_T1FF;void*_T200;int _T201;char*_T202;struct _fat_ptr _T203;unsigned long _T204;int _T205;struct Cyc_List_List*_T206;struct Cyc_List_List*_T207;struct Cyc_List_List*_T208;char*_T209;struct _fat_ptr _T20A;int _T20B;char*_T20C;char*_T20D;unsigned _T20E;unsigned char*_T20F;char*_T210;struct Cyc_Absyn_TypeDecl*_T211;int*_T212;unsigned _T213;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T214;struct Cyc_Absyn_Aggrdecl**_T215;void*_T216;struct _fat_ptr _T217;void*_T218;int _T219;int _T21A;struct Cyc_List_List*_T21B;struct Cyc_List_List*_T21C;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T21D;struct Cyc_Absyn_Datatypedecl**_T21E;void*_T21F;struct _fat_ptr _T220;void*_T221;int _T222;int _T223;struct Cyc_List_List*_T224;struct Cyc_List_List*_T225;struct Cyc_List_List*_T226;
# 491
static struct Cyc_Hashtable_Table*scvmap=0;
if(pfxpos >= 900)goto _TLC6;goto _TLC7;_TLC6: _T1=Cyc___assert_fail;{int(*_T227)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T1;_T0=_T227;}_T2=_tag_fat("pfxpos < 900",sizeof(char),13U);_T3=_tag_fat("interproc.cyc",sizeof(char),14U);_T0(_T2,_T3,492U);_TLC7: _T4=scvmap;_T5=(unsigned)_T4;
if(_T5)goto _TLC8;else{goto _TLCA;}
_TLCA: _T7=Cyc_Hashtable_create;{struct Cyc_Hashtable_Table*(*_T227)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T7;_T6=_T227;}_T8=Cyc_strptrcmp;_T9=Cyc_Hashtable_hash_stringptr;scvmap=_T6(101,_T8,_T9);goto _TLC9;_TLC8: _TLC9: {
# 496
struct Cyc_Hashtable_Table*cvmap=scvmap;
struct Cyc_List_List*ret=0;{
void*_T227=Cyc_Absyn_compress(t);struct Cyc_Absyn_TypeDecl*_T228;struct Cyc_Absyn_Typedefdecl*_T229;struct _tuple0*_T22A;struct Cyc_Absyn_Datatypefield*_T22B;struct Cyc_List_List*_T22C;void**_T22D;void*_T22E;_TA=(int*)_T227;_TB=*_TA;switch(_TB){case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T227;_TC=_T22F->f1;_T22E=_TC.elt_type;_TD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T227;_TE=& _TD->f1.ptr_atts.bounds;_T22D=(void**)_TE;}{void*elt=_T22E;void**bd=_T22D;_TF=elt;_T10=index + 1;_T11=is_local;_T12=pfxpos;
# 500
ret=Cyc_Interproc_cvars_in_type(_TF,_T10,_T11,_T12);_T13=bd;{
void*_T22F=*_T13;void*_T230;void*_T231;_T14=(int*)_T22F;_T15=*_T14;if(_T15!=3)goto _TLCC;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T232=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T22F;_T16=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T22F;_T17=& _T16->f3;_T231=(int*)_T17;_T18=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T22F;_T19=& _T18->f5;_T230=(const char**)_T19;}{int*id=(int*)_T231;const char**name=(const char**)_T230;_T1A=id;_T1B=*_T1A;
# 503
if(_T1B < 0)goto _TLCE;goto _LL1B;_TLCE: _T1C=Cyc_Interproc__prefix;_T1D=
# 505
_tag_fat(_T1C,sizeof(char),1024U);_T1E=pfxpos;_T1F=_fat_ptr_plus(_T1D,sizeof(char),_T1E);_T20=_tag_fat("_%d",sizeof(char),4U);{struct Cyc_Int_pa_PrintArg_struct _T232;_T232.tag=1;_T23=index;_T232.f1=(unsigned long)_T23;_T22=_T232;}{struct Cyc_Int_pa_PrintArg_struct _T232=_T22;void*_T233[1];_T24=_T233 + 0;*_T24=& _T232;_T25=_tag_fat(_T233,sizeof(void*),1);_T21=Cyc_Interproc_make_parg(_T25);}Cyc_vsprintf(_T1F,_T20,_T21);{struct Cyc_String_pa_PrintArg_struct _T232;_T232.tag=0;_T28=Cyc_Interproc__prefix;
_T232.f1=_tag_fat(_T28,sizeof(char),1024U);_T27=_T232;}{struct Cyc_String_pa_PrintArg_struct _T232=_T27;void*_T233[1];_T29=_T233 + 0;*_T29=& _T232;_T2A=_tag_fat("%s",sizeof(char),3U);_T2B=_tag_fat(_T233,sizeof(void*),1);_T26=Cyc_aprintf(_T2A,_T2B);}{struct _fat_ptr nn=_T26;_T2D=Cyc_Hashtable_lookup_other_opt;{
# 508
void**(*_T232)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(void**(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T2D;_T2C=_T232;}_T2E=cvmap;_T2F=& nn;_T30=(struct _fat_ptr*)_T2F;_T31=Cyc_strptrcmp;_T32=Cyc_Hashtable_hash_stringptr;{void**val=_T2C(_T2E,_T30,_T31,_T32);_T33=val;_T34=(unsigned)_T33;
if(!_T34)goto _TLD0;_T35=bd;_T36=val;
*_T35=*_T36;goto _TLD1;
# 513
_TLD0: _T37=name;_T38=nn;_T39=_untag_fat_ptr_check_bound(_T38,sizeof(char),1U);*_T37=(const char*)_T39;{struct Cyc_List_List*_T232=_cycalloc(sizeof(struct Cyc_List_List));_T3B=bd;
_T232->hd=*_T3B;_T232->tl=ret;_T3A=(struct Cyc_List_List*)_T232;}ret=_T3A;_T3C=is_local;
if(_T3C)goto _TLD2;else{goto _TLD4;}
_TLD4: _T3D=id;_T3E=id;_T3F=*_T3E;*_T3D=- _T3F;goto _TLD3;
# 518
_TLD2: _T40=id;*_T40=0;_TLD3: _T42=Cyc_Hashtable_insert;{
void(*_T232)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*))_T42;_T41=_T232;}_T43=cvmap;{struct _fat_ptr*_T232=_cycalloc(sizeof(struct _fat_ptr));*_T232=nn;_T44=(struct _fat_ptr*)_T232;}_T45=bd;_T46=*_T45;_T41(_T43,_T44,_T46);_TLD1: _T47=Cyc_Interproc__prefix;_T48=_tag_fat(_T47,sizeof(char),1024U);_T49=pfxpos;{struct _fat_ptr _T232=_fat_ptr_plus(_T48,sizeof(char),_T49);_T4A=_check_fat_subscript(_T232,sizeof(char),0U);_T4B=(char*)_T4A;{char _T233=*_T4B;char _T234='\000';_T4C=_get_fat_size(_T232,sizeof(char));if(_T4C!=1U)goto _TLD5;if(_T233!=0)goto _TLD5;if(_T234==0)goto _TLD5;_throw_arraybounds();goto _TLD6;_TLD5: _TLD6: _T4D=_T232.curr;_T4E=(char*)_T4D;*_T4E=_T234;}}goto _LL1B;}}}_TLCC: goto _LL1B;_LL1B:;}goto _LL0;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T227;_T4F=_T22F->f1;_T22E=_T4F.elt_type;}{void*elt=_T22E;_T50=elt;_T51=index + 1;_T52=is_local;_T53=pfxpos;
# 528
ret=Cyc_Interproc_cvars_in_type(_T50,_T51,_T52,_T53);goto _LL0;}case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T227;_T22E=_T22F->f3;}{struct Cyc_List_List*af=_T22E;
# 531
int findex=0;
_TLDA: _T54=af;_T55=(unsigned)_T54;if(_T55)goto _TLD8;else{goto _TLD9;}
_TLD8: _T56=af;_T57=_T56->hd;{struct Cyc_Absyn_Aggrfield*_T22F=(struct Cyc_Absyn_Aggrfield*)_T57;void*_T230;{struct Cyc_Absyn_Aggrfield _T231=*_T22F;_T230=_T231.type;}{void*t=_T230;_T58=Cyc_Interproc__prefix;_T59=
# 535
_tag_fat(_T58,sizeof(char),1024U);_T5A=pfxpos;_T5B=_fat_ptr_plus(_T59,sizeof(char),_T5A);_T5C=_tag_fat("_%d_AA%d",sizeof(char),9U);{struct Cyc_Int_pa_PrintArg_struct _T231;_T231.tag=1;_T5F=index;_T231.f1=(unsigned long)_T5F;_T5E=_T231;}{struct Cyc_Int_pa_PrintArg_struct _T231=_T5E;{struct Cyc_Int_pa_PrintArg_struct _T232;_T232.tag=1;_T61=findex;findex=_T61 + 1;_T62=_T61;_T232.f1=(unsigned long)_T62;_T60=_T232;}{struct Cyc_Int_pa_PrintArg_struct _T232=_T60;void*_T233[2];_T63=_T233 + 0;*_T63=& _T231;_T64=_T233 + 1;*_T64=& _T232;_T65=_tag_fat(_T233,sizeof(void*),2);_T5D=Cyc_Interproc_make_parg(_T65);}}Cyc_vsprintf(_T5B,_T5C,_T5D);_T66=t;_T67=is_local;_T68=Cyc_Interproc__prefix;_T69=
_tag_fat(_T68,sizeof(char),1024U);_T6A=Cyc_strlen(_T69);_T6B=(int)_T6A;_T6C=Cyc_Interproc_cvars_in_type(_T66,0,_T67,_T6B);_T6D=ret;ret=Cyc_List_append(_T6C,_T6D);goto _LL20;}_LL20:;}_T6E=af;
# 532
af=_T6E->tl;goto _TLDA;_TLD9: _T6F=Cyc_Interproc__prefix;_T70=_tag_fat(_T6F,sizeof(char),1024U);_T71=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_T70,sizeof(char),_T71);_T72=_check_fat_subscript(_T22F,sizeof(char),0U);_T73=(char*)_T72;{char _T230=*_T73;char _T231='\000';_T74=_get_fat_size(_T22F,sizeof(char));if(_T74!=1U)goto _TLDB;if(_T230!=0)goto _TLDB;if(_T231==0)goto _TLDB;_throw_arraybounds();goto _TLDC;_TLDB: _TLDC: _T75=_T22F.curr;_T76=(char*)_T75;*_T76=_T231;}}goto _LL0;}case 0: _T77=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_T78=_T77->f1;_T79=(int*)_T78;_T7A=*_T79;switch(_T7A){case 22: _T7B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_T7C=_T7B->f1;_T7D=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T7C;_T7E=_T7D->f1;_T7F=_T7E.KnownDatatype;_T80=_T7F.tag;if(_T80!=2)goto _TLDE;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_T81=_T22F->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T230=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T81;_T82=_T230->f1;_T83=_T82.KnownDatatype;_T84=_T83.val;{struct Cyc_Absyn_Datatypedecl*_T231=*_T84;_T22E=_T231;}}_T22C=_T22F->f2;}{struct Cyc_Absyn_Datatypedecl*dtd=_T22E;struct Cyc_List_List*ts=_T22C;_T85=dtd;_T86=_T85->fields;_T87=(unsigned)_T86;
# 543
if(!_T87)goto _TLE0;_T88=dtd;_T89=_T88->fields;_T8A=_T89->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T8A;
int findex=0;_T8B=dtd;_T8C=_T8B->name;{
struct _fat_ptr dtn=Cyc_Absynpp_fullqvar2string(_T8C);
_TLE5: _T8D=fs;_T8E=(unsigned)_T8D;if(_T8E)goto _TLE3;else{goto _TLE4;}
_TLE3: _T8F=fs;_T90=_T8F->hd;_T91=(struct Cyc_Absyn_Datatypefield*)_T90;{struct Cyc_List_List*ft=_T91->typs;
int finner=0;_T92=fs;_T93=_T92->hd;_T94=(struct Cyc_Absyn_Datatypefield*)_T93;_T95=_T94->name;{
struct _fat_ptr fn=Cyc_Absynpp_fullqvar2string(_T95);
_TLE9: _T96=ft;_T97=(unsigned)_T96;if(_T97)goto _TLE7;else{goto _TLE8;}
_TLE7: _T98=ft;_T99=_T98->hd;{struct _tuple16*_T22F=(struct _tuple16*)_T99;void*_T230;{struct _tuple16 _T231=*_T22F;_T230=_T231.f1;}{void*t=_T230;_T9A=Cyc_Interproc__prefix;_T9B=
_tag_fat(_T9A,sizeof(char),1024U);_T9C=pfxpos;_T9D=_fat_ptr_plus(_T9B,sizeof(char),_T9C);_T9E=_tag_fat("DD%dI%s_DF%dI%s_%d",sizeof(char),19U);{struct Cyc_Int_pa_PrintArg_struct _T231;_T231.tag=1;_T231.f1=Cyc_strlen(dtn);_TA0=_T231;}{struct Cyc_Int_pa_PrintArg_struct _T231=_TA0;{struct Cyc_String_pa_PrintArg_struct _T232;_T232.tag=0;_T232.f1=dtn;_TA1=_T232;}{struct Cyc_String_pa_PrintArg_struct _T232=_TA1;{struct Cyc_Int_pa_PrintArg_struct _T233;_T233.tag=1;_T233.f1=Cyc_strlen(fn);_TA2=_T233;}{struct Cyc_Int_pa_PrintArg_struct _T233=_TA2;{struct Cyc_String_pa_PrintArg_struct _T234;_T234.tag=0;_T234.f1=fn;_TA3=_T234;}{struct Cyc_String_pa_PrintArg_struct _T234=_TA3;{struct Cyc_Int_pa_PrintArg_struct _T235;_T235.tag=1;_TA5=finner;finner=_TA5 + 1;_TA6=_TA5;_T235.f1=(unsigned long)_TA6;_TA4=_T235;}{struct Cyc_Int_pa_PrintArg_struct _T235=_TA4;void*_T236[5];_TA7=_T236 + 0;*_TA7=& _T231;_TA8=_T236 + 1;*_TA8=& _T232;_TA9=_T236 + 2;*_TA9=& _T233;_TAA=_T236 + 3;*_TAA=& _T234;_TAB=_T236 + 4;*_TAB=& _T235;_TAC=_tag_fat(_T236,sizeof(void*),5);_T9F=Cyc_Interproc_make_parg(_TAC);}}}}}Cyc_vsprintf(_T9D,_T9E,_T9F);_TAD=t;_TAE=is_local;_TAF=Cyc_Interproc__prefix;_TB0=
_tag_fat(_TAF,sizeof(char),1024U);_TB1=Cyc_strlen(_TB0);_TB2=(int)_TB1;_TB3=Cyc_Interproc_cvars_in_type(_TAD,0,_TAE,_TB2);_TB4=ret;ret=Cyc_List_append(_TB3,_TB4);}}_TB5=ft;
# 551
ft=_TB5->tl;goto _TLE9;_TLE8:
# 556
 findex=findex + 1;}}_TB6=fs;
# 547
fs=_TB6->tl;goto _TLE5;_TLE4: _TB7=Cyc_Interproc__prefix;_TB8=_tag_fat(_TB7,sizeof(char),1024U);_TB9=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_TB8,sizeof(char),_TB9);_TBA=_check_fat_subscript(_T22F,sizeof(char),0U);_TBB=(char*)_TBA;{char _T230=*_TBB;char _T231='\000';_TBC=_get_fat_size(_T22F,sizeof(char));if(_TBC!=1U)goto _TLEA;if(_T230!=0)goto _TLEA;if(_T231==0)goto _TLEA;_throw_arraybounds();goto _TLEB;_TLEA: _TLEB: _TBD=_T22F.curr;_TBE=(char*)_TBD;*_TBE=_T231;}}}}goto _TLE1;_TLE0: _TLE1: _TBF=dtd;_T22E=_TBF->name;_T22C=ts;goto _LLA;}_TLDE:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_TC0=_T22F->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T230=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TC0;_TC1=_T230->f1;_TC2=_TC1.UnknownDatatype;_TC3=_TC2.val;_T22E=_TC3.name;}_T22C=_T22F->f2;}_LLA: {struct _tuple0*n=_T22E;struct Cyc_List_List*ts=_T22C;_TC4=ts;_TC5=(unsigned)_TC4;
# 562
if(!_TC5)goto _TLEC;{
struct _fat_ptr adn=Cyc_Absynpp_fullqvar2string(n);{
int ix=0;_TLF1: _TC6=ts;_TC7=(unsigned)_TC6;if(_TC7)goto _TLEF;else{goto _TLF0;}
_TLEF: _TC8=Cyc_Interproc__prefix;_TC9=_tag_fat(_TC8,sizeof(char),1024U);_TCA=pfxpos;_TCB=_fat_ptr_plus(_TC9,sizeof(char),_TCA);_TCC=_tag_fat("_%dDD%dI%s_DDARG%d",sizeof(char),19U);{struct Cyc_Int_pa_PrintArg_struct _T22F;_T22F.tag=1;_TCF=index;_T22F.f1=(unsigned long)_TCF;_TCE=_T22F;}{struct Cyc_Int_pa_PrintArg_struct _T22F=_TCE;{struct Cyc_Int_pa_PrintArg_struct _T230;_T230.tag=1;_T230.f1=Cyc_strlen(adn);_TD0=_T230;}{struct Cyc_Int_pa_PrintArg_struct _T230=_TD0;{struct Cyc_String_pa_PrintArg_struct _T231;_T231.tag=0;_T231.f1=adn;_TD1=_T231;}{struct Cyc_String_pa_PrintArg_struct _T231=_TD1;{struct Cyc_Int_pa_PrintArg_struct _T232;_T232.tag=1;_TD3=ix;_T232.f1=(unsigned long)_TD3;_TD2=_T232;}{struct Cyc_Int_pa_PrintArg_struct _T232=_TD2;void*_T233[4];_TD4=_T233 + 0;*_TD4=& _T22F;_TD5=_T233 + 1;*_TD5=& _T230;_TD6=_T233 + 2;*_TD6=& _T231;_TD7=_T233 + 3;*_TD7=& _T232;_TD8=_tag_fat(_T233,sizeof(void*),4);_TCD=Cyc_Interproc_make_parg(_TD8);}}}}Cyc_vsprintf(_TCB,_TCC,_TCD);_TD9=ts;_TDA=_TD9->hd;_TDB=is_local;_TDC=Cyc_Interproc__prefix;_TDD=
_tag_fat(_TDC,sizeof(char),1024U);_TDE=Cyc_strlen(_TDD);_TDF=(int)_TDE;_TE0=Cyc_Interproc_cvars_in_type(_TDA,0,_TDB,_TDF);_TE1=ret;ret=Cyc_List_append(_TE0,_TE1);_TE2=ts;
# 564
ts=_TE2->tl;ix=ix + 1;goto _TLF1;_TLF0:;}_TE3=Cyc_Interproc__prefix;_TE4=_tag_fat(_TE3,sizeof(char),1024U);_TE5=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_TE4,sizeof(char),_TE5);_TE6=_check_fat_subscript(_T22F,sizeof(char),0U);_TE7=(char*)_TE6;{char _T230=*_TE7;char _T231='\000';_TE8=_get_fat_size(_T22F,sizeof(char));if(_TE8!=1U)goto _TLF2;if(_T230!=0)goto _TLF2;if(_T231==0)goto _TLF2;_throw_arraybounds();goto _TLF3;_TLF2: _TLF3: _TE9=_T22F.curr;_TEA=(char*)_TE9;*_TEA=_T231;}}}goto _TLED;_TLEC: _TLED: goto _LL0;}case 23: _TEB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_TEC=_TEB->f1;_TED=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TEC;_TEE=_TED->f1;_TEF=_TEE.KnownDatatypefield;_TF0=_TEF.tag;if(_TF0!=2)goto _TLF4;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_TF1=_T22F->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T230=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TF1;_TF2=_T230->f1;_TF3=_TF2.KnownDatatypefield;_TF4=_TF3.val;_T22E=_TF4.f0;_TF5=_T230->f1;_TF6=_TF5.KnownDatatypefield;_TF7=_TF6.val;_T22B=_TF7.f1;}_T22C=_T22F->f2;}{struct Cyc_Absyn_Datatypedecl*dd=_T22E;struct Cyc_Absyn_Datatypefield*dfd=_T22B;struct Cyc_List_List*ts=_T22C;_TF9=Cyc_Warn_impos;{
# 572
int(*_T22F)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TF9;_TF8=_T22F;}_TFA=_tag_fat("Unexpected DatatypeFieldCon in interproc cvar naming",sizeof(char),53U);_TFB=_tag_fat(0U,sizeof(void*),0);_TF8(_TFA,_TFB);}goto _TLF5;_TLF4:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_TFC=_T22F->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T230=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TFC;_TFD=_T230->f1;_TFE=_TFD.UnknownDatatypefield;_TFF=_TFE.val;_T22E=_TFF.datatype_name;_T100=_T230->f1;_T101=_T100.UnknownDatatypefield;_T102=_T101.val;_T22A=_T102.field_name;}_T22C=_T22F->f2;}{struct _tuple0*dn=_T22E;struct _tuple0*dfn=_T22A;struct Cyc_List_List*ts=_T22C;_T103=ts;_T104=(unsigned)_T103;
# 574
if(!_T104)goto _TLF6;{
struct _fat_ptr sdn=Cyc_Absynpp_fullqvar2string(dn);
struct _fat_ptr sdfn=Cyc_Absynpp_fullqvar2string(dfn);{
int ix=0;_TLFB: _T105=ts;_T106=(unsigned)_T105;if(_T106)goto _TLF9;else{goto _TLFA;}
_TLF9: _T107=Cyc_Interproc__prefix;_T108=_tag_fat(_T107,sizeof(char),1024U);_T109=pfxpos;_T10A=_fat_ptr_plus(_T108,sizeof(char),_T109);_T10B=_tag_fat("_%dDD%dI%s_DF%dI%s_DFARG%d",sizeof(char),27U);{struct Cyc_Int_pa_PrintArg_struct _T22F;_T22F.tag=1;_T10E=index;_T22F.f1=(unsigned long)_T10E;_T10D=_T22F;}{struct Cyc_Int_pa_PrintArg_struct _T22F=_T10D;{struct Cyc_Int_pa_PrintArg_struct _T230;_T230.tag=1;_T230.f1=Cyc_strlen(sdn);_T10F=_T230;}{struct Cyc_Int_pa_PrintArg_struct _T230=_T10F;{struct Cyc_String_pa_PrintArg_struct _T231;_T231.tag=0;_T231.f1=sdn;_T110=_T231;}{struct Cyc_String_pa_PrintArg_struct _T231=_T110;{struct Cyc_Int_pa_PrintArg_struct _T232;_T232.tag=1;_T232.f1=Cyc_strlen(sdfn);_T111=_T232;}{struct Cyc_Int_pa_PrintArg_struct _T232=_T111;{struct Cyc_String_pa_PrintArg_struct _T233;_T233.tag=0;_T233.f1=sdfn;_T112=_T233;}{struct Cyc_String_pa_PrintArg_struct _T233=_T112;{struct Cyc_Int_pa_PrintArg_struct _T234;_T234.tag=1;_T114=ix;_T234.f1=(unsigned long)_T114;_T113=_T234;}{struct Cyc_Int_pa_PrintArg_struct _T234=_T113;void*_T235[6];_T115=_T235 + 0;*_T115=& _T22F;_T116=_T235 + 1;*_T116=& _T230;_T117=_T235 + 2;*_T117=& _T231;_T118=_T235 + 3;*_T118=& _T232;_T119=_T235 + 4;*_T119=& _T233;_T11A=_T235 + 5;*_T11A=& _T234;_T11B=_tag_fat(_T235,sizeof(void*),6);_T10C=Cyc_Interproc_make_parg(_T11B);}}}}}}Cyc_vsprintf(_T10A,_T10B,_T10C);_T11C=ts;_T11D=_T11C->hd;_T11E=is_local;_T11F=Cyc_Interproc__prefix;_T120=
_tag_fat(_T11F,sizeof(char),1024U);_T121=Cyc_strlen(_T120);_T122=(int)_T121;_T123=Cyc_Interproc_cvars_in_type(_T11D,0,_T11E,_T122);_T124=ret;ret=Cyc_List_append(_T123,_T124);_T125=ts;
# 577
ts=_T125->tl;ix=ix + 1;goto _TLFB;_TLFA:;}_T126=Cyc_Interproc__prefix;_T127=_tag_fat(_T126,sizeof(char),1024U);_T128=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_T127,sizeof(char),_T128);_T129=_check_fat_subscript(_T22F,sizeof(char),0U);_T12A=(char*)_T129;{char _T230=*_T12A;char _T231='\000';_T12B=_get_fat_size(_T22F,sizeof(char));if(_T12B!=1U)goto _TLFC;if(_T230!=0)goto _TLFC;if(_T231==0)goto _TLFC;_throw_arraybounds();goto _TLFD;_TLFC: _TLFD: _T12C=_T22F.curr;_T12D=(char*)_T12C;*_T12D=_T231;}}}goto _TLF7;_TLF6: _TLF7: goto _LL0;}_TLF5:;case 24: _T12E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_T12F=_T12E->f1;_T130=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T12F;_T131=_T130->f1;_T132=_T131.KnownAggr;_T133=_T132.tag;if(_T133!=2)goto _TLFE;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_T134=_T22F->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T230=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T134;_T135=_T230->f1;_T136=_T135.KnownAggr;_T137=_T136.val;{struct Cyc_Absyn_Aggrdecl*_T231=*_T137;_T22E=_T231;}}_T22C=_T22F->f2;}{struct Cyc_Absyn_Aggrdecl*ad=_T22E;struct Cyc_List_List*ts=_T22C;_T138=ad;_T139=_T138->name;{
# 585
struct _fat_ptr adn=Cyc_Absynpp_fullqvar2string(_T139);
int findex=0;_T13A=ad;_T13B=_T13A->impl;_T13C=(unsigned)_T13B;
if(!_T13C)goto _TL100;_T13D=ad;_T13E=_T13D->impl;{
struct Cyc_List_List*fs=_T13E->fields;_TL105: _T13F=fs;_T140=(unsigned)_T13F;if(_T140)goto _TL103;else{goto _TL104;}
_TL103: _T141=Cyc_Interproc__prefix;_T142=_tag_fat(_T141,sizeof(char),1024U);_T143=pfxpos;_T144=_fat_ptr_plus(_T142,sizeof(char),_T143);_T145=_tag_fat("AD%dI%s_AF%d",sizeof(char),13U);{struct Cyc_Int_pa_PrintArg_struct _T22F;_T22F.tag=1;_T22F.f1=Cyc_strlen(adn);_T147=_T22F;}{struct Cyc_Int_pa_PrintArg_struct _T22F=_T147;{struct Cyc_String_pa_PrintArg_struct _T230;_T230.tag=0;_T230.f1=adn;_T148=_T230;}{struct Cyc_String_pa_PrintArg_struct _T230=_T148;{struct Cyc_Int_pa_PrintArg_struct _T231;_T231.tag=1;_T14A=findex;findex=_T14A + 1;_T14B=_T14A;_T231.f1=(unsigned long)_T14B;_T149=_T231;}{struct Cyc_Int_pa_PrintArg_struct _T231=_T149;void*_T232[3];_T14C=_T232 + 0;*_T14C=& _T22F;_T14D=_T232 + 1;*_T14D=& _T230;_T14E=_T232 + 2;*_T14E=& _T231;_T14F=_tag_fat(_T232,sizeof(void*),3);_T146=Cyc_Interproc_make_parg(_T14F);}}}Cyc_vsprintf(_T144,_T145,_T146);_T150=fs;_T151=_T150->hd;_T152=(struct Cyc_Absyn_Aggrfield*)_T151;_T153=_T152->type;_T154=is_local;_T155=Cyc_Interproc__prefix;_T156=
_tag_fat(_T155,sizeof(char),1024U);_T157=Cyc_strlen(_T156);_T158=(int)_T157;_T159=Cyc_Interproc_cvars_in_type(_T153,0,_T154,_T158);_T15A=ret;ret=Cyc_List_append(_T159,_T15A);_T15B=fs;
# 588
fs=_T15B->tl;goto _TL105;_TL104:;}_T15C=Cyc_Interproc__prefix;_T15D=_tag_fat(_T15C,sizeof(char),1024U);_T15E=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_T15D,sizeof(char),_T15E);_T15F=_check_fat_subscript(_T22F,sizeof(char),0U);_T160=(char*)_T15F;{char _T230=*_T160;char _T231='\000';_T161=_get_fat_size(_T22F,sizeof(char));if(_T161!=1U)goto _TL106;if(_T230!=0)goto _TL106;if(_T231==0)goto _TL106;_throw_arraybounds();goto _TL107;_TL106: _TL107: _T162=_T22F.curr;_T163=(char*)_T162;*_T163=_T231;}}goto _TL101;_TL100: _TL101: _T164=ad;_T22E=_T164->name;_T22C=ts;goto _LL12;}}_TLFE:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T227;_T165=_T22F->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T230=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T165;_T166=_T230->f1;_T167=_T166.UnknownAggr;_T168=_T167.val;_T22E=_T168.f1;}_T22C=_T22F->f2;}_LL12: {struct _tuple0*n=_T22E;struct Cyc_List_List*ts=_T22C;
# 596
struct _fat_ptr adn=Cyc_Absynpp_fullqvar2string(n);
int findex=0;_T169=ts;_T16A=(unsigned)_T169;
if(!_T16A)goto _TL108;{
int ix=0;_TL10D: _T16B=ts;_T16C=(unsigned)_T16B;if(_T16C)goto _TL10B;else{goto _TL10C;}
_TL10B: _T16D=Cyc_Interproc__prefix;_T16E=_tag_fat(_T16D,sizeof(char),1024U);_T16F=pfxpos;_T170=_fat_ptr_plus(_T16E,sizeof(char),_T16F);_T171=_tag_fat("_%dAD%dI%s_ADARG%d",sizeof(char),19U);{struct Cyc_Int_pa_PrintArg_struct _T22F;_T22F.tag=1;_T174=index;_T22F.f1=(unsigned long)_T174;_T173=_T22F;}{struct Cyc_Int_pa_PrintArg_struct _T22F=_T173;{struct Cyc_Int_pa_PrintArg_struct _T230;_T230.tag=1;_T230.f1=Cyc_strlen(adn);_T175=_T230;}{struct Cyc_Int_pa_PrintArg_struct _T230=_T175;{struct Cyc_String_pa_PrintArg_struct _T231;_T231.tag=0;_T231.f1=adn;_T176=_T231;}{struct Cyc_String_pa_PrintArg_struct _T231=_T176;{struct Cyc_Int_pa_PrintArg_struct _T232;_T232.tag=1;_T178=ix;_T232.f1=(unsigned long)_T178;_T177=_T232;}{struct Cyc_Int_pa_PrintArg_struct _T232=_T177;void*_T233[4];_T179=_T233 + 0;*_T179=& _T22F;_T17A=_T233 + 1;*_T17A=& _T230;_T17B=_T233 + 2;*_T17B=& _T231;_T17C=_T233 + 3;*_T17C=& _T232;_T17D=_tag_fat(_T233,sizeof(void*),4);_T172=Cyc_Interproc_make_parg(_T17D);}}}}Cyc_vsprintf(_T170,_T171,_T172);_T17E=ts;_T17F=_T17E->hd;_T180=is_local;_T181=Cyc_Interproc__prefix;_T182=
_tag_fat(_T181,sizeof(char),1024U);_T183=Cyc_strlen(_T182);_T184=(int)_T183;_T185=Cyc_Interproc_cvars_in_type(_T17F,0,_T180,_T184);_T186=ret;ret=Cyc_List_append(_T185,_T186);_T187=ts;
# 599
ts=_T187->tl;ix=ix + 1;goto _TL10D;_TL10C:;}_T188=Cyc_Interproc__prefix;_T189=_tag_fat(_T188,sizeof(char),1024U);_T18A=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_T189,sizeof(char),_T18A);_T18B=_check_fat_subscript(_T22F,sizeof(char),0U);_T18C=(char*)_T18B;{char _T230=*_T18C;char _T231='\000';_T18D=_get_fat_size(_T22F,sizeof(char));if(_T18D!=1U)goto _TL10E;if(_T230!=0)goto _TL10E;if(_T231==0)goto _TL10E;_throw_arraybounds();goto _TL10F;_TL10E: _TL10F: _T18E=_T22F.curr;_T18F=(char*)_T18E;*_T18F=_T231;}}goto _TL109;_TL108: _TL109: goto _LL0;}default: goto _LL19;};case 6:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T227;_T190=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T227;_T191=& _T190->f1;_T22E=(struct Cyc_Absyn_FnInfo*)_T191;}{struct Cyc_Absyn_FnInfo*fi=_T22E;_T192=Cyc_Interproc__prefix;_T193=
# 607
_tag_fat(_T192,sizeof(char),1024U);_T194=pfxpos;_T195=_fat_ptr_plus(_T193,sizeof(char),_T194);_T196=_tag_fat("_%d_FR",sizeof(char),7U);{struct Cyc_Int_pa_PrintArg_struct _T22F;_T22F.tag=1;_T199=index;_T22F.f1=(unsigned long)_T199;_T198=_T22F;}{struct Cyc_Int_pa_PrintArg_struct _T22F=_T198;void*_T230[1];_T19A=_T230 + 0;*_T19A=& _T22F;_T19B=_tag_fat(_T230,sizeof(void*),1);_T197=Cyc_Interproc_make_parg(_T19B);}Cyc_vsprintf(_T195,_T196,_T197);_T19C=fi;_T19D=_T19C->ret_type;_T19E=is_local;_T19F=Cyc_Interproc__prefix;_T1A0=
_tag_fat(_T19F,sizeof(char),1024U);_T1A1=Cyc_strlen(_T1A0);_T1A2=(int)_T1A1;_T1A3=Cyc_Interproc_cvars_in_type(_T19D,0,_T19E,_T1A2);_T1A4=ret;ret=Cyc_List_append(_T1A3,_T1A4);{
int argix=0;_T1A5=fi;{
struct Cyc_List_List*args=_T1A5->args;_TL113: _T1A6=args;_T1A7=(unsigned)_T1A6;if(_T1A7)goto _TL111;else{goto _TL112;}
_TL111: _T1A8=args;_T1A9=_T1A8->hd;{struct _tuple8*_T22F=(struct _tuple8*)_T1A9;void*_T230;{struct _tuple8 _T231=*_T22F;_T230=_T231.f2;}{void*t=_T230;_T1AA=Cyc_Interproc__prefix;_T1AB=
_tag_fat(_T1AA,sizeof(char),1024U);_T1AC=pfxpos;_T1AD=_fat_ptr_plus(_T1AB,sizeof(char),_T1AC);_T1AE=_tag_fat("_%d_ARG%d",sizeof(char),10U);{struct Cyc_Int_pa_PrintArg_struct _T231;_T231.tag=1;_T1B1=index;_T231.f1=(unsigned long)_T1B1;_T1B0=_T231;}{struct Cyc_Int_pa_PrintArg_struct _T231=_T1B0;{struct Cyc_Int_pa_PrintArg_struct _T232;_T232.tag=1;_T1B3=argix;argix=_T1B3 + 1;_T1B4=_T1B3;_T232.f1=(unsigned long)_T1B4;_T1B2=_T232;}{struct Cyc_Int_pa_PrintArg_struct _T232=_T1B2;void*_T233[2];_T1B5=_T233 + 0;*_T1B5=& _T231;_T1B6=_T233 + 1;*_T1B6=& _T232;_T1B7=_tag_fat(_T233,sizeof(void*),2);_T1AF=Cyc_Interproc_make_parg(_T1B7);}}Cyc_vsprintf(_T1AD,_T1AE,_T1AF);_T1B8=t;_T1B9=is_local;_T1BA=Cyc_Interproc__prefix;_T1BB=
# 614
_tag_fat(_T1BA,sizeof(char),1024U);_T1BC=Cyc_strlen(_T1BB);_T1BD=(int)_T1BC;_T1BE=Cyc_Interproc_cvars_in_type(_T1B8,0,_T1B9,_T1BD);_T1BF=ret;ret=Cyc_List_append(_T1BE,_T1BF);}}_T1C0=args;
# 610
args=_T1C0->tl;goto _TL113;_TL112:;}_T1C1=Cyc_Interproc__prefix;_T1C2=_tag_fat(_T1C1,sizeof(char),1024U);_T1C3=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_T1C2,sizeof(char),_T1C3);_T1C4=_check_fat_subscript(_T22F,sizeof(char),0U);_T1C5=(char*)_T1C4;{char _T230=*_T1C5;char _T231='\000';_T1C6=_get_fat_size(_T22F,sizeof(char));if(_T1C6!=1U)goto _TL114;if(_T230!=0)goto _TL114;if(_T231==0)goto _TL114;_throw_arraybounds();goto _TL115;_TL114: _TL115: _T1C7=_T22F.curr;_T1C8=(char*)_T1C7;*_T1C8=_T231;}}goto _LL0;}}case 8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T227;_T22A=_T22F->f1;_T22C=_T22F->f2;_T229=_T22F->f3;_T1C9=_T22F->f4;_T22E=(void*)_T1C9;}{struct _tuple0*tdn=_T22A;struct Cyc_List_List*ts=_T22C;struct Cyc_Absyn_Typedefdecl*tdef=_T229;void*topt=_T22E;
# 619
struct _fat_ptr tn=Cyc_Absynpp_fullqvar2string(tdn);_T1CA=tdef;_T1CB=(unsigned)_T1CA;
if(!_T1CB)goto _TL116;_T1CC=tdef;_T1CD=_T1CC->defn;_T1CE=(unsigned)_T1CD;if(!_T1CE)goto _TL116;_T1CF=Cyc_Interproc__prefix;_T1D0=
_tag_fat(_T1CF,sizeof(char),1024U);_T1D1=pfxpos;_T1D2=_fat_ptr_plus(_T1D0,sizeof(char),_T1D1);_T1D3=_tag_fat("TD%dI%s",sizeof(char),8U);{struct Cyc_Int_pa_PrintArg_struct _T22F;_T22F.tag=1;_T22F.f1=Cyc_strlen(tn);_T1D5=_T22F;}{struct Cyc_Int_pa_PrintArg_struct _T22F=_T1D5;{struct Cyc_String_pa_PrintArg_struct _T230;_T230.tag=0;_T230.f1=tn;_T1D6=_T230;}{struct Cyc_String_pa_PrintArg_struct _T230=_T1D6;void*_T231[2];_T1D7=_T231 + 0;*_T1D7=& _T22F;_T1D8=_T231 + 1;*_T1D8=& _T230;_T1D9=_tag_fat(_T231,sizeof(void*),2);_T1D4=Cyc_Interproc_make_parg(_T1D9);}}Cyc_vsprintf(_T1D2,_T1D3,_T1D4);_T1DA=tdef;_T1DB=_T1DA->defn;_T1DC=
_check_null(_T1DB);_T1DD=is_local;_T1DE=Cyc_Interproc__prefix;_T1DF=_tag_fat(_T1DE,sizeof(char),1024U);_T1E0=Cyc_strlen(_T1DF);_T1E1=(int)_T1E0;_T1E2=Cyc_Interproc_cvars_in_type(_T1DC,0,_T1DD,_T1E1);_T1E3=ret;ret=Cyc_List_append(_T1E2,_T1E3);_T1E4=Cyc_Interproc__prefix;_T1E5=_tag_fat(_T1E4,sizeof(char),1024U);_T1E6=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_T1E5,sizeof(char),_T1E6);_T1E7=_check_fat_subscript(_T22F,sizeof(char),0U);_T1E8=(char*)_T1E7;{char _T230=*_T1E8;char _T231='\000';_T1E9=_get_fat_size(_T22F,sizeof(char));if(_T1E9!=1U)goto _TL118;if(_T230!=0)goto _TL118;if(_T231==0)goto _TL118;_throw_arraybounds();goto _TL119;_TL118: _TL119: _T1EA=_T22F.curr;_T1EB=(char*)_T1EA;*_T1EB=_T231;}}goto _TL117;_TL116: _TL117:{
# 626
int ix=0;_TL11D: _T1EC=ts;_T1ED=(unsigned)_T1EC;if(_T1ED)goto _TL11B;else{goto _TL11C;}
_TL11B: _T1EE=Cyc_Interproc__prefix;_T1EF=_tag_fat(_T1EE,sizeof(char),1024U);_T1F0=pfxpos;_T1F1=_fat_ptr_plus(_T1EF,sizeof(char),_T1F0);_T1F2=_tag_fat("_%dTD%dI%s_TDARG%d",sizeof(char),19U);{struct Cyc_Int_pa_PrintArg_struct _T22F;_T22F.tag=1;_T1F5=index;_T22F.f1=(unsigned long)_T1F5;_T1F4=_T22F;}{struct Cyc_Int_pa_PrintArg_struct _T22F=_T1F4;{struct Cyc_Int_pa_PrintArg_struct _T230;_T230.tag=1;_T230.f1=Cyc_strlen(tn);_T1F6=_T230;}{struct Cyc_Int_pa_PrintArg_struct _T230=_T1F6;{struct Cyc_String_pa_PrintArg_struct _T231;_T231.tag=0;_T231.f1=tn;_T1F7=_T231;}{struct Cyc_String_pa_PrintArg_struct _T231=_T1F7;{struct Cyc_Int_pa_PrintArg_struct _T232;_T232.tag=1;_T1F9=ix;_T232.f1=(unsigned long)_T1F9;_T1F8=_T232;}{struct Cyc_Int_pa_PrintArg_struct _T232=_T1F8;void*_T233[4];_T1FA=_T233 + 0;*_T1FA=& _T22F;_T1FB=_T233 + 1;*_T1FB=& _T230;_T1FC=_T233 + 2;*_T1FC=& _T231;_T1FD=_T233 + 3;*_T1FD=& _T232;_T1FE=_tag_fat(_T233,sizeof(void*),4);_T1F3=Cyc_Interproc_make_parg(_T1FE);}}}}Cyc_vsprintf(_T1F1,_T1F2,_T1F3);_T1FF=ts;_T200=_T1FF->hd;_T201=is_local;_T202=Cyc_Interproc__prefix;_T203=
_tag_fat(_T202,sizeof(char),1024U);_T204=Cyc_strlen(_T203);_T205=(int)_T204;_T206=Cyc_Interproc_cvars_in_type(_T200,0,_T201,_T205);_T207=ret;ret=Cyc_List_append(_T206,_T207);_T208=ts;
# 626
ts=_T208->tl;ix=ix + 1;goto _TL11D;_TL11C:;}_T209=Cyc_Interproc__prefix;_T20A=_tag_fat(_T209,sizeof(char),1024U);_T20B=pfxpos;{struct _fat_ptr _T22F=_fat_ptr_plus(_T20A,sizeof(char),_T20B);_T20C=_check_fat_subscript(_T22F,sizeof(char),0U);_T20D=(char*)_T20C;{char _T230=*_T20D;char _T231='\000';_T20E=_get_fat_size(_T22F,sizeof(char));if(_T20E!=1U)goto _TL11E;if(_T230!=0)goto _TL11E;if(_T231==0)goto _TL11E;_throw_arraybounds();goto _TL11F;_TL11E: _TL11F: _T20F=_T22F.curr;_T210=(char*)_T20F;*_T210=_T231;}}goto _LL0;}case 10:{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T22F=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T227;_T228=_T22F->f1;_T22D=_T22F->f2;}{struct Cyc_Absyn_TypeDecl*td=_T228;void**to=_T22D;_T211=td;{
# 633
void*_T22F=_T211->r;struct Cyc_Absyn_Datatypedecl*_T230;struct Cyc_Absyn_Aggrdecl*_T231;_T212=(int*)_T22F;_T213=*_T212;switch(_T213){case 0:{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T232=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T22F;_T231=_T232->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T231;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T232=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T232->tag=24;{struct Cyc_Absyn_Aggrdecl**_T233=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 635
*_T233=ad;_T215=(struct Cyc_Absyn_Aggrdecl**)_T233;}_T232->f1=Cyc_Absyn_KnownAggr(_T215);_T214=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T232;}_T216=(void*)_T214;_T217=_tag_fat(0U,sizeof(void*),0);_T218=Cyc_Absyn_app_type(_T216,_T217);_T219=is_local;_T21A=pfxpos;_T21B=Cyc_Interproc_cvars_in_type(_T218,0,_T219,_T21A);_T21C=ret;ret=Cyc_List_append(_T21B,_T21C);goto _LL29;}case 2:{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T232=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T22F;_T230=_T232->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T230;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T232=_cycalloc(sizeof(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct));_T232->tag=22;{struct Cyc_Absyn_Datatypedecl**_T233=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));
# 639
*_T233=tud;_T21E=(struct Cyc_Absyn_Datatypedecl**)_T233;}_T232->f1=Cyc_Absyn_KnownDatatype(_T21E);_T21D=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T232;}_T21F=(void*)_T21D;_T220=_tag_fat(0U,sizeof(void*),0);_T221=Cyc_Absyn_app_type(_T21F,_T220);_T222=is_local;_T223=pfxpos;_T224=Cyc_Interproc_cvars_in_type(_T221,0,_T222,_T223);_T225=ret;ret=Cyc_List_append(_T224,_T225);goto _LL29;}default: goto _LL29;}_LL29:;}goto _LL0;}default: _LL19: goto _LL0;}_LL0:;}_T226=ret;
# 649
return _T226;}}
# 653
static int Cyc_Interproc_exp_noop(void*a,struct Cyc_Absyn_Exp*b){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;void*_T3;_T0=b;{
void*_T4=_T0->r;void*_T5;_T1=(int*)_T4;_T2=*_T1;if(_T2!=14)goto _TL121;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T4;_T3=_T6->f1;_T5=(void*)_T3;}{void*t=_T5;
# 656
Cyc_Tcutil_revert_cvars(t);goto _LL0;}_TL121: goto _LL0;_LL0:;}
# 661
return 1;}struct _tuple17{struct _fat_ptr f0;int f1;void(*f2)(struct Cyc_List_List*,void*);void*f3;};
# 666
static int Cyc_Interproc_cvars_in_local_decl(struct _tuple17*env,struct Cyc_Absyn_Stmt*s){struct _tuple17*_T0;struct _tuple17*_T1;int*_T2;void*_T3;struct Cyc_Absyn_Stmt*_T4;int*_T5;int _T6;struct Cyc_Absyn_Decl*_T7;int*_T8;unsigned _T9;struct Cyc_Absyn_Vardecl*_TA;struct _tuple0*_TB;char*_TC;unsigned _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_Int_pa_PrintArg_struct _T11;struct Cyc_String_pa_PrintArg_struct _T12;struct Cyc_Int_pa_PrintArg_struct _T13;struct Cyc_String_pa_PrintArg_struct _T14;struct Cyc_Int_pa_PrintArg_struct _T15;int*_T16;int _T17;int _T18;void**_T19;void**_T1A;void**_T1B;void**_T1C;void**_T1D;struct _fat_ptr _T1E;struct Cyc_Absyn_Vardecl*_T1F;void*_T20;char*_T21;struct _fat_ptr _T22;unsigned long _T23;int _T24;struct Cyc_Absyn_Pat*_T25;int*_T26;int _T27;struct Cyc_Absyn_Vardecl*_T28;struct _tuple0*_T29;char*_T2A;unsigned _T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;struct Cyc_Int_pa_PrintArg_struct _T2F;struct Cyc_String_pa_PrintArg_struct _T30;struct Cyc_Int_pa_PrintArg_struct _T31;struct Cyc_String_pa_PrintArg_struct _T32;struct Cyc_Int_pa_PrintArg_struct _T33;int*_T34;int _T35;int _T36;void**_T37;void**_T38;void**_T39;void**_T3A;void**_T3B;struct _fat_ptr _T3C;struct Cyc_Absyn_Vardecl*_T3D;void*_T3E;char*_T3F;struct _fat_ptr _T40;unsigned long _T41;int _T42;void*_T43;void(*_T44)(struct Cyc_List_List*,void*);void*_T45;struct _fat_ptr _T46;_T0=env;{struct _tuple17 _T47=*_T0;_T46=_T47.f0;_T1=env;_T2=& _T1->f1;_T45=(int*)_T2;_T44=_T47.f2;_T3=_T47.f3;_T43=(void*)_T3;}{struct _fat_ptr pfx=_T46;int*count=(int*)_T45;void(*fn)(struct Cyc_List_List*,void*)=_T44;void*inner_env=_T43;_T4=s;{
# 668
void*_T47=_T4->r;struct Cyc_Absyn_Stmt*_T48;struct Cyc_Absyn_Decl*_T49;_T5=(int*)_T47;_T6=*_T5;if(_T6!=12)goto _TL123;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T4A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T47;_T49=_T4A->f1;_T48=_T4A->f2;}{struct Cyc_Absyn_Decl*d=_T49;struct Cyc_Absyn_Stmt*s1=_T48;_T7=d;{
# 670
void*_T4A=_T7->r;struct Cyc_Absyn_Pat*_T4B;struct Cyc_Absyn_Vardecl*_T4C;_T8=(int*)_T4A;_T9=*_T8;switch(_T9){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T4D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T4A;_T4C=_T4D->f1;}{struct Cyc_Absyn_Vardecl*vd=_T4C;_TA=vd;_TB=_TA->name;{
# 672
struct _fat_ptr ln=Cyc_Absynpp_fullqvar2string(_TB);_TC=Cyc_Interproc__prefix;_TD=1024U - 1U;_TE=
_tag_fat(_TC,sizeof(char),_TD);_TF=_tag_fat("%dI%s_%dI%s_%d",sizeof(char),15U);{struct Cyc_Int_pa_PrintArg_struct _T4D;_T4D.tag=1;
_T4D.f1=Cyc_strlen(pfx);_T11=_T4D;}{struct Cyc_Int_pa_PrintArg_struct _T4D=_T11;{struct Cyc_String_pa_PrintArg_struct _T4E;_T4E.tag=0;_T4E.f1=pfx;_T12=_T4E;}{struct Cyc_String_pa_PrintArg_struct _T4E=_T12;{struct Cyc_Int_pa_PrintArg_struct _T4F;_T4F.tag=1;_T4F.f1=Cyc_strlen(ln);_T13=_T4F;}{struct Cyc_Int_pa_PrintArg_struct _T4F=_T13;{struct Cyc_String_pa_PrintArg_struct _T50;_T50.tag=0;_T50.f1=ln;_T14=_T50;}{struct Cyc_String_pa_PrintArg_struct _T50=_T14;{struct Cyc_Int_pa_PrintArg_struct _T51;_T51.tag=1;_T16=count;_T17=*_T16;*_T16=_T17 + 1;_T18=_T17;_T51.f1=(unsigned long)_T18;_T15=_T51;}{struct Cyc_Int_pa_PrintArg_struct _T51=_T15;void*_T52[5];_T19=_T52 + 0;*_T19=& _T4D;_T1A=_T52 + 1;*_T1A=& _T4E;_T1B=_T52 + 2;*_T1B=& _T4F;_T1C=_T52 + 3;*_T1C=& _T50;_T1D=_T52 + 4;*_T1D=& _T51;_T1E=_tag_fat(_T52,sizeof(void*),5);_T10=Cyc_Interproc_make_parg(_T1E);}}}}}
# 673
Cyc_vsprintf(_TE,_TF,_T10);_T1F=vd;_T20=_T1F->type;_T21=Cyc_Interproc__prefix;_T22=
# 675
_tag_fat(_T21,sizeof(char),1024U);_T23=Cyc_strlen(_T22);_T24=(int)_T23;{struct Cyc_List_List*cvars=Cyc_Interproc_cvars_in_type(_T20,0,1,_T24);
# 679
fn(cvars,inner_env);goto _LL8;}}}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T4D=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T4A;_T4B=_T4D->f1;}{struct Cyc_Absyn_Pat*p=_T4B;_T25=p;{
# 682
void*_T4D=_T25->r;struct Cyc_Absyn_Vardecl*_T4E;_T26=(int*)_T4D;_T27=*_T26;if(_T27!=2)goto _TL126;{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T4F=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T4D;_T4E=_T4F->f2;}{struct Cyc_Absyn_Vardecl*vd=_T4E;_T28=vd;_T29=_T28->name;{
# 684
struct _fat_ptr an=Cyc_Absynpp_fullqvar2string(_T29);_T2A=Cyc_Interproc__prefix;_T2B=1024U - 1U;_T2C=
_tag_fat(_T2A,sizeof(char),_T2B);_T2D=_tag_fat("%dI%s_%dI%s_%d",sizeof(char),15U);{struct Cyc_Int_pa_PrintArg_struct _T4F;_T4F.tag=1;_T4F.f1=Cyc_strlen(pfx);_T2F=_T4F;}{struct Cyc_Int_pa_PrintArg_struct _T4F=_T2F;{struct Cyc_String_pa_PrintArg_struct _T50;_T50.tag=0;_T50.f1=pfx;_T30=_T50;}{struct Cyc_String_pa_PrintArg_struct _T50=_T30;{struct Cyc_Int_pa_PrintArg_struct _T51;_T51.tag=1;_T51.f1=Cyc_strlen(an);_T31=_T51;}{struct Cyc_Int_pa_PrintArg_struct _T51=_T31;{struct Cyc_String_pa_PrintArg_struct _T52;_T52.tag=0;_T52.f1=an;_T32=_T52;}{struct Cyc_String_pa_PrintArg_struct _T52=_T32;{struct Cyc_Int_pa_PrintArg_struct _T53;_T53.tag=1;_T34=count;_T35=*_T34;*_T34=_T35 + 1;_T36=_T35;_T53.f1=(unsigned long)_T36;_T33=_T53;}{struct Cyc_Int_pa_PrintArg_struct _T53=_T33;void*_T54[5];_T37=_T54 + 0;*_T37=& _T4F;_T38=_T54 + 1;*_T38=& _T50;_T39=_T54 + 2;*_T39=& _T51;_T3A=_T54 + 3;*_T3A=& _T52;_T3B=_T54 + 4;*_T3B=& _T53;_T3C=_tag_fat(_T54,sizeof(void*),5);_T2E=Cyc_Interproc_make_parg(_T3C);}}}}}Cyc_vsprintf(_T2C,_T2D,_T2E);_T3D=vd;_T3E=_T3D->type;_T3F=Cyc_Interproc__prefix;_T40=
_tag_fat(_T3F,sizeof(char),1024U);_T41=Cyc_strlen(_T40);_T42=(int)_T41;{struct Cyc_List_List*cvars=Cyc_Interproc_cvars_in_type(_T3E,0,1,_T42);
# 690
fn(cvars,inner_env);goto _LLF;}}}_TL126: goto _LLF;_LLF:;}goto _LL8;}default: goto _LL8;}_LL8:;}goto _LL3;}_TL123: goto _LL3;_LL3:;}
# 703
return 1;}}
# 709
static void Cyc_Interproc_remove_local_constraints(struct Cyc_List_List*cvars,int ignore){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;
_TL12B: _T0=cvars;_T1=(unsigned)_T0;if(_T1)goto _TL129;else{goto _TL12A;}
_TL129: _T2=cvars;_T3=_T2->hd;Cyc_BansheeIf_reset_local_cvar(_T3);_T4=cvars;
# 710
cvars=_T4->tl;goto _TL12B;_TL12A:;}struct _tuple18{struct _fat_ptr f0;int f1;void(*f2)(struct Cyc_List_List*,int);int f3;};
# 721
static struct Cyc_List_List*Cyc_Interproc_assign_cvar_names(struct Cyc_List_List*ds){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_Absyn_Vardecl*_T9;struct _tuple0*_TA;char*_TB;unsigned _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Int_pa_PrintArg_struct _T10;struct Cyc_String_pa_PrintArg_struct _T11;void**_T12;void**_T13;struct _fat_ptr _T14;struct Cyc_Absyn_Vardecl*_T15;void*_T16;char*_T17;struct _fat_ptr _T18;unsigned long _T19;int _T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_Absyn_Fndecl*_T1D;struct _tuple0*_T1E;char*_T1F;unsigned _T20;struct _fat_ptr _T21;struct _fat_ptr _T22;struct _fat_ptr _T23;struct Cyc_Int_pa_PrintArg_struct _T24;struct Cyc_String_pa_PrintArg_struct _T25;void**_T26;void**_T27;struct _fat_ptr _T28;struct Cyc_Absyn_Fndecl*_T29;struct _tuple0*_T2A;int _T2B;struct Cyc_Absyn_Fndecl*_T2C;struct Cyc_Absyn_FnInfo _T2D;struct Cyc_List_List*_T2E;unsigned _T2F;struct Cyc_List_List*_T30;struct Cyc_List_List*_T31;unsigned _T32;struct Cyc_List_List*_T33;struct Cyc_List_List*_T34;void*_T35;struct _tuple8*_T36;void*_T37;void*_T38;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T39;struct Cyc_Absyn_Fndecl*_T3A;void*_T3B;char*_T3C;struct _fat_ptr _T3D;unsigned long _T3E;int _T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*_T41;struct _tuple18*_T42;struct _tuple18*_T43;struct _tuple18*_T44;struct _tuple18*_T45;void(*_T46)(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*),struct _tuple18*,struct Cyc_Absyn_Stmt*);void(*_T47)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);int(*_T48)(struct _tuple18*,struct Cyc_Absyn_Exp*);int(*_T49)(struct _tuple18*,struct Cyc_Absyn_Stmt*);struct _tuple18*_T4A;struct Cyc_Absyn_Fndecl*_T4B;struct Cyc_Absyn_Stmt*_T4C;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T4D;struct Cyc_Absyn_Aggrdecl**_T4E;void*_T4F;struct _fat_ptr _T50;void*_T51;struct Cyc_List_List*_T52;struct Cyc_List_List*_T53;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T54;struct Cyc_Absyn_Datatypedecl**_T55;void*_T56;struct _fat_ptr _T57;void*_T58;struct Cyc_List_List*_T59;struct Cyc_List_List*_T5A;struct Cyc_Absyn_Typedefdecl*_T5B;struct _tuple0*_T5C;struct Cyc_Absyn_Typedefdecl*_T5D;void*_T5E;struct Cyc_List_List*_T5F;struct Cyc_List_List*_T60;struct Cyc_List_List*_T61;struct Cyc_List_List*_T62;
struct Cyc_List_List*ret=0;
_TL12F: _T0=ds;_T1=(unsigned)_T0;if(_T1)goto _TL12D;else{goto _TL12E;}
_TL12D: _T2=ds;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T63=_T4->r;struct Cyc_Absyn_Typedefdecl*_T64;struct Cyc_Absyn_Datatypedecl*_T65;struct Cyc_Absyn_Aggrdecl*_T66;struct Cyc_Absyn_Fndecl*_T67;struct Cyc_Absyn_Vardecl*_T68;struct Cyc_List_List*_T69;_T5=(int*)_T63;_T6=*_T5;switch(_T6){case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T6A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T63;_T69=_T6A->f2;}{struct Cyc_List_List*l=_T69;_T69=l;goto _LL4;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T6A=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T63;_T69=_T6A->f2;}_LL4: {struct Cyc_List_List*l=_T69;_T7=ret;_T8=
# 726
Cyc_Interproc_assign_cvar_names(l);ret=Cyc_List_append(_T7,_T8);goto _LL0;}case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T6A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T63;_T68=_T6A->f1;}{struct Cyc_Absyn_Vardecl*vd=_T68;_T9=vd;_TA=_T9->name;{
# 728
struct _fat_ptr tn=Cyc_Absynpp_fullqvar2string(_TA);_TB=Cyc_Interproc__prefix;_TC=1024U - 1U;_TD=
_tag_fat(_TB,sizeof(char),_TC);_TE=_tag_fat("%dI%s",sizeof(char),6U);{struct Cyc_Int_pa_PrintArg_struct _T6A;_T6A.tag=1;_T6A.f1=Cyc_strlen(tn);_T10=_T6A;}{struct Cyc_Int_pa_PrintArg_struct _T6A=_T10;{struct Cyc_String_pa_PrintArg_struct _T6B;_T6B.tag=0;_T6B.f1=tn;_T11=_T6B;}{struct Cyc_String_pa_PrintArg_struct _T6B=_T11;void*_T6C[2];_T12=_T6C + 0;*_T12=& _T6A;_T13=_T6C + 1;*_T13=& _T6B;_T14=_tag_fat(_T6C,sizeof(void*),2);_TF=Cyc_Interproc_make_parg(_T14);}}Cyc_vsprintf(_TD,_TE,_TF);_T15=vd;_T16=_T15->type;_T17=Cyc_Interproc__prefix;_T18=
_tag_fat(_T17,sizeof(char),1024U);_T19=Cyc_strlen(_T18);_T1A=(int)_T19;_T1B=Cyc_Interproc_cvars_in_type(_T16,0,0,_T1A);_T1C=ret;ret=Cyc_List_append(_T1B,_T1C);goto _LL0;}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T6A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T63;_T67=_T6A->f1;}{struct Cyc_Absyn_Fndecl*fd=_T67;_T1D=fd;_T1E=_T1D->name;{
# 734
struct _fat_ptr fdname=Cyc_Absynpp_fullqvar2string(_T1E);_T1F=Cyc_Interproc__prefix;_T20=1024U - 1U;_T21=
_tag_fat(_T1F,sizeof(char),_T20);_T22=_tag_fat("%dI%s",sizeof(char),6U);{struct Cyc_Int_pa_PrintArg_struct _T6A;_T6A.tag=1;_T6A.f1=Cyc_strlen(fdname);_T24=_T6A;}{struct Cyc_Int_pa_PrintArg_struct _T6A=_T24;{struct Cyc_String_pa_PrintArg_struct _T6B;_T6B.tag=0;_T6B.f1=fdname;_T25=_T6B;}{struct Cyc_String_pa_PrintArg_struct _T6B=_T25;void*_T6C[2];_T26=_T6C + 0;*_T26=& _T6A;_T27=_T6C + 1;*_T27=& _T6B;_T28=_tag_fat(_T6C,sizeof(void*),2);_T23=Cyc_Interproc_make_parg(_T28);}}Cyc_vsprintf(_T21,_T22,_T23);_T29=fd;_T2A=_T29->name;_T2B=
Cyc_Tcutil_is_main(_T2A);if(!_T2B)goto _TL131;_T2C=fd;_T2D=_T2C->i;{
struct Cyc_List_List*args=_T2D.args;_T2E=args;_T2F=(unsigned)_T2E;
if(!_T2F)goto _TL133;_T30=args;_T31=_T30->tl;_T32=(unsigned)_T31;if(!_T32)goto _TL133;_T33=args;_T34=_T33->tl;_T35=_T34->hd;_T36=(struct _tuple8*)_T35;_T37=_T36->f2;_T38=Cyc_Absyn_fat_bound_type;
Cyc_Tcutil_replace_cvars(_T37,_T38);goto _TL134;_TL133: _TL134:;}goto _TL132;
# 743
_TL131:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T6A=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T6A->tag=6;_T3A=fd;_T6A->f1=_T3A->i;_T39=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T6A;}_T3B=(void*)_T39;_T3C=Cyc_Interproc__prefix;_T3D=_tag_fat(_T3C,sizeof(char),1024U);_T3E=Cyc_strlen(_T3D);_T3F=(int)_T3E;_T40=Cyc_Interproc_cvars_in_type(_T3B,0,0,_T3F);_T41=ret;ret=Cyc_List_append(_T40,_T41);_TL132: {
# 747
struct _tuple18*env;env=_cycalloc(sizeof(struct _tuple18));_T42=env;_T42->f0=fdname;_T43=env;_T43->f1=0;_T44=env;_T44->f2=Cyc_Interproc_remove_local_constraints;_T45=env;_T45->f3=0;_T47=Cyc_Absyn_visit_stmt;{
void(*_T6A)(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*),struct _tuple18*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*),struct _tuple18*,struct Cyc_Absyn_Stmt*))_T47;_T46=_T6A;}{int(*_T6A)(struct _tuple18*,struct Cyc_Absyn_Exp*)=(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*))Cyc_Interproc_exp_noop;_T48=_T6A;}{int(*_T6A)(struct _tuple18*,struct Cyc_Absyn_Stmt*)=(int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*))Cyc_Interproc_cvars_in_local_decl;_T49=_T6A;}_T4A=env;_T4B=fd;_T4C=_T4B->body;_T46(_T48,_T49,_T4A,_T4C);goto _LL0;}}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T6A=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T63;_T66=_T6A->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T66;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T6A=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T6A->tag=24;{struct Cyc_Absyn_Aggrdecl**_T6B=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 751
*_T6B=ad;_T4E=(struct Cyc_Absyn_Aggrdecl**)_T6B;}_T6A->f1=Cyc_Absyn_KnownAggr(_T4E);_T4D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6A;}_T4F=(void*)_T4D;_T50=_tag_fat(0U,sizeof(void*),0);_T51=Cyc_Absyn_app_type(_T4F,_T50);_T52=Cyc_Interproc_cvars_in_type(_T51,0,0,0);_T53=ret;ret=Cyc_List_append(_T52,_T53);goto _LL0;}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T6A=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T63;_T65=_T6A->f1;}{struct Cyc_Absyn_Datatypedecl*dd=_T65;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T6A=_cycalloc(sizeof(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct));_T6A->tag=22;{struct Cyc_Absyn_Datatypedecl**_T6B=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));
# 755
*_T6B=dd;_T55=(struct Cyc_Absyn_Datatypedecl**)_T6B;}_T6A->f1=Cyc_Absyn_KnownDatatype(_T55);_T54=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T6A;}_T56=(void*)_T54;_T57=_tag_fat(0U,sizeof(void*),0);_T58=Cyc_Absyn_app_type(_T56,_T57);_T59=Cyc_Interproc_cvars_in_type(_T58,0,0,0);_T5A=ret;ret=Cyc_List_append(_T59,_T5A);goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T6A=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T63;_T64=_T6A->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T64;_T5B=td;_T5C=_T5B->name;_T5D=td;_T5E=
# 759
Cyc_Absyn_typedef_type(_T5C,0,_T5D,0);_T5F=Cyc_Interproc_cvars_in_type(_T5E,0,0,0);_T60=ret;ret=Cyc_List_append(_T5F,_T60);goto _LL0;}default: goto _LL0;}_LL0:;}_T61=ds;
# 723
ds=_T61->tl;goto _TL12F;_TL12E: _T62=ret;
# 766
return _T62;}
# 769
static struct Cyc_List_List*Cyc_Interproc_parse_constraint_graph (void){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc___cycFILE*_T8;unsigned _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_List_List*_TC;{struct Cyc_String_pa_PrintArg_struct _TD;_TD.tag=0;
_TD.f1=Cyc_Interproc_get_inference_prefix();_T1=_TD;}{struct Cyc_String_pa_PrintArg_struct _TD=_T1;void*_TE[1];_T2=_TE + 0;*_T2=& _TD;_T3=_tag_fat("%s/.cych/cvars",sizeof(char),15U);_T4=_tag_fat(_TE,sizeof(void*),1);_T0=Cyc_aprintf(_T3,_T4);}{struct _fat_ptr cfilename=_T0;_T5=cfilename;_T6=
_tag_fat("r",sizeof(char),2U);_T7=_tag_fat("constraint file",sizeof(char),16U);{struct Cyc___cycFILE*file=Cyc_Interproc_try_file_open(_T5,_T6,_T7);_T8=file;_T9=(unsigned)_T8;
if(_T9)goto _TL135;else{goto _TL137;}
_TL137: _TA=_tag_fat("Unable to initialize constraint graph",sizeof(char),38U);_TB=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_TA,_TB);
return 0;_TL135:
# 776
 Cyc_Warn_reset(cfilename);{
struct Cyc_List_List*ret=Cyc_Parse_parse_constraint_file(file);
Cyc_fclose(file);_TC=ret;
return _TC;}}}}
# 781
static int Cyc_Interproc_read_constraint_graph (void){
struct Cyc_List_List*cg=Cyc_Interproc_parse_constraint_graph();
Cyc_BansheeIf_init_toplevel_cvars(cg);
return 0;}
# 787
static void Cyc_Interproc_print_cvarlist(struct Cyc_List_List*l){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_List_List*_T4;
_TL13B: _T0=l;_T1=(unsigned)_T0;if(_T1)goto _TL139;else{goto _TL13A;}
_TL139: _T2=l;_T3=_T2->hd;Cyc_Tcutil_deconstruct_type(_T3);_T4=l;
# 788
l=_T4->tl;goto _TL13B;_TL13A:;}
# 795
static void Cyc_Interproc_bind_cvars(struct Cyc_List_List*ds){int _T0;int _T1;struct Cyc_List_List*_T2;unsigned _T3;struct Cyc_List_List*_T4;int*_T5;int _T6;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T7;void**_T8;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T9;void**_TA;void**_TB;void*_TC;int(*_TD)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_TE)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _TF;struct _fat_ptr _T10;int(*_T11)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T12)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T13;struct _fat_ptr _T14;void**_T15;void*_T16;int(*_T17)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T18)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T19;struct _fat_ptr _T1A;struct Cyc_List_List*_T1B;void*_T1C;int(*_T1D)(struct _fat_ptr,struct _fat_ptr);void*(*_T1E)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T1F;struct _fat_ptr _T20;struct Cyc_List_List*_T21;_T0=Cyc_Flags_no_merge;
if(_T0)goto _TL13F;else{goto _TL13E;}_TL13F: _T1=Cyc_Interproc_read_constraint_graph();if(_T1)goto _TL13E;else{goto _TL13C;}
_TL13E: Cyc_BansheeIf_init_toplevel_cvars(0);goto _TL13D;_TL13C: _TL13D: {
# 799
struct Cyc_List_List*cvars=Cyc_Interproc_assign_cvar_names(ds);
_TL143: _T2=cvars;_T3=(unsigned)_T2;if(_T3)goto _TL141;else{goto _TL142;}
_TL141: _T4=cvars;{void*_T22=_T4->hd;const char*_T23;void*_T24;int _T25;void*_T26;struct Cyc_Core_Opt*_T27;_T5=(int*)_T22;_T6=*_T5;if(_T6!=3)goto _TL144;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T28=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T22;_T27=_T28->f1;_T7=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T22;_T8=& _T7->f2;_T26=(void**)_T8;_T25=_T28->f3;_T9=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T22;_TA=& _T9->f4;_T24=(void**)_TA;_T23=_T28->f5;}{struct Cyc_Core_Opt*ok=_T27;void**topt=(void**)_T26;int id=_T25;void**bv=(void**)_T24;const char*name=_T23;_TB=topt;_TC=*_TB;
# 803
if(_TC!=0)goto _TL146;goto _TL147;_TL146: _TE=Cyc___assert_fail;{int(*_T28)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_TE;_TD=_T28;}_TF=_tag_fat("*topt==0",sizeof(char),9U);_T10=_tag_fat("interproc.cyc",sizeof(char),14U);_TD(_TF,_T10,803U);_TL147:
 if(id <= 0)goto _TL148;goto _TL149;_TL148: _T12=Cyc___assert_fail;{int(*_T28)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T12;_T11=_T28;}_T13=_tag_fat("id>0",sizeof(char),5U);_T14=_tag_fat("interproc.cyc",sizeof(char),14U);_T11(_T13,_T14,804U);_TL149: _T15=bv;_T16=*_T15;
if(_T16!=0)goto _TL14A;goto _TL14B;_TL14A: _T18=Cyc___assert_fail;{int(*_T28)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T18;_T17=_T28;}_T19=_tag_fat("*bv==0",sizeof(char),7U);_T1A=_tag_fat("interproc.cyc",sizeof(char),14U);_T17(_T19,_T1A,805U);_TL14B: _T1B=cvars;_T1C=_T1B->hd;
# 809
Cyc_BansheeIf_register_toplevel_cvar(_T1C);goto _LL0;}_TL144: _T1E=Cyc_Warn_impos;{
# 812
int(*_T28)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T1E;_T1D=_T28;}_T1F=_tag_fat("Unexpected type in cvar list",sizeof(char),29U);_T20=_tag_fat(0U,sizeof(void*),0);_T1D(_T1F,_T20);_LL0:;}_T21=cvars;
# 800
cvars=_T21->tl;goto _TL143;_TL142:;}}
# 820
void Cyc_Interproc_emit_decls(struct _fat_ptr filename,struct Cyc_List_List*ds){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;void**_T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc___cycFILE*_T8;unsigned _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_String_pa_PrintArg_struct _TE;void**_TF;void**_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct _fat_ptr _T14;struct _fat_ptr _T15;struct Cyc___cycFILE*_T16;unsigned _T17;struct Cyc_Absynpp_Params*_T18;struct Cyc_Absynpp_Params*_T19;{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;
# 822
_T1A.f1=Cyc_Interproc_get_inference_prefix();_T1=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_T1;void*_T1B[1];_T2=_T1B + 0;*_T2=& _T1A;_T3=_tag_fat("%s/.cych/cvars",sizeof(char),15U);_T4=_tag_fat(_T1B,sizeof(void*),1);_T0=Cyc_aprintf(_T3,_T4);}{struct _fat_ptr cvar_filename=_T0;_T5=cvar_filename;_T6=
_tag_fat("w",sizeof(char),2U);_T7=_tag_fat("constraint file",sizeof(char),16U);{struct Cyc___cycFILE*file=Cyc_Interproc_try_file_open(_T5,_T6,_T7);_T8=file;_T9=(unsigned)_T8;
if(_T9)goto _TL14C;else{goto _TL14E;}
_TL14E: _TA=_tag_fat("Unable to open constrant file",sizeof(char),30U);_TB=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_TA,_TB);
return;_TL14C:
# 828
 Cyc_BansheeIf_persist_cvarmap(file);
Cyc_fclose(file);{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;
# 831
_T1A.f1=Cyc_Interproc_get_inference_prefix();_TD=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_TD;{struct Cyc_String_pa_PrintArg_struct _T1B;_T1B.tag=0;_T1B.f1=Cyc_Filename_basename(filename);_TE=_T1B;}{struct Cyc_String_pa_PrintArg_struct _T1B=_TE;void*_T1C[2];_TF=_T1C + 0;*_TF=& _T1A;_T10=_T1C + 1;*_T10=& _T1B;_T11=_tag_fat("%s/.cych/%s",sizeof(char),12U);_T12=_tag_fat(_T1C,sizeof(void*),2);_TC=Cyc_aprintf(_T11,_T12);}}{struct _fat_ptr outname=_TC;_T13=outname;_T14=
_tag_fat("w",sizeof(char),2U);_T15=_tag_fat("output file",sizeof(char),12U);{struct Cyc___cycFILE*outfile=Cyc_Interproc_try_file_open(_T13,_T14,_T15);_T16=outfile;_T17=(unsigned)_T16;
if(_T17)goto _TL14F;else{goto _TL151;}
_TL151: return;_TL14F: _T18=& Cyc_Absynpp_cycinf_params_r;_T19=(struct Cyc_Absynpp_Params*)_T18;
Cyc_Absynpp_set_params(_T19);
Cyc_Absynpp_decllist2file(ds,outfile);
return;}}}}}
# 841
struct Cyc_List_List*Cyc_Interproc_binding_cvar_resolution(struct Cyc_List_List*tds){int _T0;int(*_T1)(struct _fat_ptr,struct _fat_ptr);void*(*_T2)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T3;struct _fat_ptr _T4;int _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;void*_T8;struct Cyc_List_List*_T9;_T0=Cyc_Flags_resolve;
if(_T0)goto _TL152;else{goto _TL154;}
_TL154: _T2=Cyc_Warn_impos;{int(*_TA)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2;_T1=_TA;}_T3=_tag_fat("Cvar resolution is only supported with --resolve",sizeof(char),49U);_T4=_tag_fat(0U,sizeof(void*),0);_T1(_T3,_T4);goto _TL153;_TL152: _TL153: _T5=
# 845
Cyc_Interproc_read_constraint_graph();if(!_T5)goto _TL155;_T6=
_tag_fat("Failed to read constraint graph; cannot resolve Cvars",sizeof(char),54U);_T7=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(0U,_T6,_T7);goto _TL156;_TL155: _TL156:
# 848
 Cyc_Binding_resolve_all(tds);_T8=
Cyc_Absyn_thinconst();Cyc_BansheeIf_resolve_or_default(_T8);_T9=tds;
return _T9;}struct _tuple19{const char*f0;const char*f1;};
# 853
int Cyc_Interproc_cmp_stats(struct _tuple19*a,struct _tuple19*b){struct _fat_ptr _T0;struct _tuple19*_T1;void*_T2;void*_T3;unsigned _T4;struct _fat_ptr _T5;struct _tuple19*_T6;void*_T7;void*_T8;unsigned _T9;int _TA;_T1=a;{const char*_TB=_T1->f1;_T2=(void*)_TB;_T3=(void*)_TB;_T4=_get_zero_arr_size_char(_T3,1U);_T0=_tag_fat(_T2,sizeof(char),_T4);}_T6=b;{const char*_TB=_T6->f1;_T7=(void*)_TB;_T8=(void*)_TB;_T9=_get_zero_arr_size_char(_T8,1U);_T5=_tag_fat(_T7,sizeof(char),_T9);}_TA=
Cyc_strcmp(_T0,_T5);return _TA;}
# 858
struct Cyc_List_List*Cyc_Interproc_resolve_all(struct _fat_ptr filename,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;struct _tuple11*_T1;struct Cyc_List_List*_T2;unsigned _T3;struct Cyc_List_List*_T4;void*_T5;struct _tuple11*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;unsigned _T9;struct _handler_cons*_TA;int*_TB;int _TC;int _TD;struct Cyc_List_List*_TE;unsigned _TF;void*_T10;struct Cyc_Interproc_Unresolved_sym_exn_struct*_T11;char*_T12;char*_T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_Exp_Warn_Warg_struct _T15;void**_T16;void**_T17;struct Cyc_Absyn_Exp*_T18;unsigned _T19;struct _fat_ptr _T1A;struct Cyc_List_List*_T1B;unsigned _T1C;struct Cyc_List_List*_T1D;void*_T1E;struct _tuple11*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;unsigned _T22;struct Cyc_List_List*_T23;struct Cyc_List_List*_T24;void*_T25;struct _tuple11*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;
struct Cyc_List_List*orig=tds;
struct Cyc_List_List*first=0;struct Cyc_List_List*worklist=0;{struct Cyc_List_List*_T2A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_T2B=_cycalloc(sizeof(struct _tuple11));
_T2B->f0=filename;_T2B->f1=tds;_T1=(struct _tuple11*)_T2B;}_T2A->hd=_T1;_T2A->tl=0;_T0=(struct Cyc_List_List*)_T2A;}first=_T0;{
struct Cyc_List_List*all_located_fnsyms=0;
# 864
_TL157: _T2=worklist;_T3=(unsigned)_T2;if(!_T3)goto _TL159;_T4=worklist;_T5=_T4->hd;_T6=(struct _tuple11*)_T5;
tds=_T6->f1;_T7=worklist;
worklist=_T7->tl;goto _TL15A;_TL159: _TL15A:{
# 868
struct Cyc_List_List*externs=Cyc_Binding_resolve_all_interproc(tds);_T8=externs;_T9=(unsigned)_T8;
if(!_T9)goto _TL15B;{struct _handler_cons _T2A;_TA=& _T2A;_push_handler(_TA);{int _T2B=0;_TB=_T2A.handler;_TC=setjmp(_TB);if(!_TC)goto _TL15D;_T2B=1;goto _TL15E;_TL15D: _TL15E: if(_T2B)goto _TL15F;else{goto _TL161;}_TL161:{
# 871
struct _tuple14 _T2C=Cyc_Interproc_locate_externs(externs,first);struct Cyc_List_List*_T2D;struct Cyc_List_List*_T2E;int _T2F;_T2F=_T2C.f0;_T2E=_T2C.f1;_T2D=_T2C.f2;{int b=_T2F;struct Cyc_List_List*nd=_T2E;struct Cyc_List_List*rext=_T2D;_TD=b;
if(_TD)goto _TL162;else{goto _TL164;}
_TL164:{struct Cyc_List_List*_T30=tds;_npop_handler(0);return _T30;}_TL162: _TE=worklist;_TF=(unsigned)_TE;
if(_TF)goto _TL165;else{goto _TL167;}
_TL167: worklist=nd;goto _TL166;_TL165: _TL166:
# 877
 all_located_fnsyms=Cyc_List_imp_append(rext,all_located_fnsyms);}}_pop_handler();goto _TL160;_TL15F: _T10=Cyc_Core_get_exn_thrown();{void*_T2C=(void*)_T10;void*_T2D;_T11=(struct Cyc_Interproc_Unresolved_sym_exn_struct*)_T2C;_T12=_T11->tag;_T13=Cyc_Interproc_Unresolved_sym;if(_T12!=_T13)goto _TL168;{struct Cyc_Interproc_Unresolved_sym_exn_struct*_T2E=(struct Cyc_Interproc_Unresolved_sym_exn_struct*)_T2C;_T2D=_T2E->f1;}{struct Cyc_Absyn_Exp*e=_T2D;{struct Cyc_Warn_String_Warn_Warg_struct _T2E;_T2E.tag=0;
# 880
_T2E.f1=_tag_fat("Undefined symbol",sizeof(char),17U);_T14=_T2E;}{struct Cyc_Warn_String_Warn_Warg_struct _T2E=_T14;{struct Cyc_Warn_Exp_Warn_Warg_struct _T2F;_T2F.tag=4;_T2F.f1=e;_T15=_T2F;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T2F=_T15;void*_T30[2];_T16=_T30 + 0;*_T16=& _T2E;_T17=_T30 + 1;*_T17=& _T2F;_T18=e;_T19=_T18->loc;_T1A=_tag_fat(_T30,sizeof(void*),2);Cyc_Warn_err2(_T19,_T1A);}}
return 0;}_TL168: _T2D=_T2C;{void*exn=_T2D;_rethrow(exn);};}_TL160:;}}goto _TL15C;_TL15B: _TL15C:;}_T1B=worklist;_T1C=(unsigned)_T1B;
# 863
if(_T1C)goto _TL157;else{goto _TL158;}_TL158: _T1D=first;_T1E=_T1D->hd;_T1F=(struct _tuple11*)_T1E;{
# 885
struct Cyc_List_List*prev=_T1F->f1;
struct Cyc_List_List*all_decls=orig;_T20=first;{
struct Cyc_List_List*it=_T20->tl;_TL16D: _T21=it;_T22=(unsigned)_T21;if(_T22)goto _TL16B;else{goto _TL16C;}
_TL16B: _T23=all_decls;_T24=it;_T25=_T24->hd;_T26=(struct _tuple11*)_T25;_T27=_T26->f1;all_decls=Cyc_List_imp_append(_T23,_T27);_T28=it;
# 887
it=_T28->tl;goto _TL16D;_TL16C:;}
# 893
Cyc_Interproc_bind_cvars(all_decls);_T29=all_decls;
return _T29;}}}struct _tuple20{void*f0;struct Cyc_List_List*f1;};struct _tuple21{const char*f0;const char*f1;int f2;int f3;};
# 897
void Cyc_Interproc_inference_statistics (void){struct Cyc___cycFILE*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;int _T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_List_List*_T7;unsigned _T8;struct Cyc_List_List*_T9;void*_TA;void*_TB;int*_TC;int _TD;void*_TE;int*_TF;unsigned _T10;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11;void*_T12;int*_T13;unsigned _T14;struct Cyc_List_List*_T15;struct _tuple21*_T16;struct Cyc_List_List*_T17;struct _tuple21*_T18;struct Cyc_List_List*_T19;struct Cyc_Int_pa_PrintArg_struct _T1A;int _T1B;void**_T1C;struct Cyc___cycFILE*_T1D;struct _fat_ptr _T1E;struct _fat_ptr _T1F;struct Cyc_List_List*(*_T20)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*);struct Cyc_List_List*(*_T21)(int(*)(void*,void*),struct Cyc_List_List*);int(*_T22)(struct _tuple21*,struct _tuple21*);struct Cyc_List_List*_T23;struct Cyc_String_pa_PrintArg_struct _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct Cyc_String_pa_PrintArg_struct _T26;struct Cyc_String_pa_PrintArg_struct _T27;void**_T28;void**_T29;void**_T2A;void**_T2B;struct Cyc___cycFILE*_T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;struct Cyc___cycFILE*_T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_List_List*_T32;unsigned _T33;const char*_T34;struct Cyc_List_List*_T35;void*_T36;struct _tuple21*_T37;int _T38;const char*_T39;struct Cyc_List_List*_T3A;void*_T3B;struct _tuple21*_T3C;int _T3D;struct Cyc_String_pa_PrintArg_struct _T3E;struct _fat_ptr _T3F;struct Cyc_List_List*_T40;void*_T41;struct _tuple21*_T42;void*_T43;void*_T44;unsigned _T45;struct Cyc_String_pa_PrintArg_struct _T46;struct _fat_ptr _T47;struct Cyc_List_List*_T48;void*_T49;struct _tuple21*_T4A;void*_T4B;void*_T4C;unsigned _T4D;struct Cyc_String_pa_PrintArg_struct _T4E;struct _fat_ptr _T4F;void*_T50;void*_T51;unsigned _T52;struct Cyc_String_pa_PrintArg_struct _T53;struct _fat_ptr _T54;void*_T55;void*_T56;unsigned _T57;void**_T58;void**_T59;void**_T5A;void**_T5B;struct Cyc___cycFILE*_T5C;struct _fat_ptr _T5D;struct _fat_ptr _T5E;struct Cyc_List_List*_T5F;struct Cyc___cycFILE*_T60;struct _fat_ptr _T61;struct _fat_ptr _T62;struct Cyc___cycFILE*_T63;struct _fat_ptr _T64;struct _fat_ptr _T65;_T0=Cyc_stderr;_T1=
_tag_fat("Looking for constraint variables...\n",sizeof(char),37U);_T2=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T0,_T1,_T2);{
struct Cyc_List_List*graph=Cyc_Interproc_parse_constraint_graph();_T3=
Cyc_BansheeIf_add_all_constraints(graph);if(!_T3)goto _TL16E;_T4=Cyc_stderr;_T5=
_tag_fat("Inconsistent constraint graph\n",sizeof(char),31U);_T6=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4,_T5,_T6);
return;_TL16E: {
# 906
struct Cyc_List_List*ips=0;
int nips=0;
_TL173: _T7=graph;_T8=(unsigned)_T7;if(_T8)goto _TL171;else{goto _TL172;}
_TL171: _T9=graph;_TA=_T9->hd;{struct _tuple20*_T66=(struct _tuple20*)_TA;void*_T67;{struct _tuple20 _T68=*_T66;_T67=_T68.f0;}{void*t=_T67;int _T68;const char*_T69;const char*_T6A;_TB=t;_TC=(int*)_TB;_TD=*_TC;if(_TD!=3)goto _TL174;_TE=t;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TE;_T6A=_T6B->f5;_T69=_T6B->f6;_T68=_T6B->f7;}{const char*name=_T6A;const char*loc=_T69;int origfat=_T68;{
# 912
void*_T6B=Cyc_Absyn_compress(t);_TF=(int*)_T6B;_T10=*_TF;switch(_T10){case 3: goto _LLC;case 0: _T11=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6B;_T12=_T11->f1;_T13=(int*)_T12;_T14=*_T13;switch(_T14){case 13: _LLC:{struct Cyc_List_List*_T6C=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple21*_T6D=_cycalloc(sizeof(struct _tuple21));
# 915
_T6D->f0=name;_T6D->f1=loc;_T6D->f2=0;_T6D->f3=origfat;_T16=(struct _tuple21*)_T6D;}_T6C->hd=_T16;_T6C->tl=ips;_T15=(struct Cyc_List_List*)_T6C;}ips=_T15;
nips=nips + 1;goto _LL8;case 14:{struct Cyc_List_List*_T6C=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple21*_T6D=_cycalloc(sizeof(struct _tuple21));
# 919
_T6D->f0=name;_T6D->f1=loc;_T6D->f2=1;_T6D->f3=origfat;_T18=(struct _tuple21*)_T6D;}_T6C->hd=_T18;_T6C->tl=ips;_T17=(struct Cyc_List_List*)_T6C;}ips=_T17;
nips=nips + 1;goto _LL8;default: goto _LLF;};default: _LLF: goto _LL8;}_LL8:;}goto _LL3;}_TL174: goto _LL3;_LL3:;}}_T19=graph;
# 908
graph=_T19->tl;goto _TL173;_TL172:{struct Cyc_Int_pa_PrintArg_struct _T66;_T66.tag=1;_T1B=nips;
# 930
_T66.f1=(unsigned long)_T1B;_T1A=_T66;}{struct Cyc_Int_pa_PrintArg_struct _T66=_T1A;void*_T67[1];_T1C=_T67 + 0;*_T1C=& _T66;_T1D=Cyc_stderr;_T1E=_tag_fat("Found %d of them.\n",sizeof(char),19U);_T1F=_tag_fat(_T67,sizeof(void*),1);Cyc_fprintf(_T1D,_T1E,_T1F);}_T21=Cyc_List_merge_sort;{
# 933
struct Cyc_List_List*(*_T66)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*))_T21;_T20=_T66;}_T22=(int(*)(struct _tuple21*,struct _tuple21*))Cyc_Interproc_cmp_stats;_T23=ips;ips=_T20(_T22,_T23);{struct Cyc_String_pa_PrintArg_struct _T66;_T66.tag=0;
_T66.f1=_tag_fat("Constraint variable",sizeof(char),20U);_T24=_T66;}{struct Cyc_String_pa_PrintArg_struct _T66=_T24;{struct Cyc_String_pa_PrintArg_struct _T67;_T67.tag=0;_T67.f1=_tag_fat("Location, file:line",sizeof(char),20U);_T25=_T67;}{struct Cyc_String_pa_PrintArg_struct _T67=_T25;{struct Cyc_String_pa_PrintArg_struct _T68;_T68.tag=0;_T68.f1=_tag_fat("Orig",sizeof(char),5U);_T26=_T68;}{struct Cyc_String_pa_PrintArg_struct _T68=_T26;{struct Cyc_String_pa_PrintArg_struct _T69;_T69.tag=0;_T69.f1=_tag_fat("Inf",sizeof(char),4U);_T27=_T69;}{struct Cyc_String_pa_PrintArg_struct _T69=_T27;void*_T6A[4];_T28=_T6A + 0;*_T28=& _T66;_T29=_T6A + 1;*_T29=& _T67;_T2A=_T6A + 2;*_T2A=& _T68;_T2B=_T6A + 3;*_T2B=& _T69;_T2C=Cyc_stderr;_T2D=_tag_fat("%-49s %-30s %-4s %-4s\n",sizeof(char),23U);_T2E=_tag_fat(_T6A,sizeof(void*),4);Cyc_fprintf(_T2C,_T2D,_T2E);}}}}_T2F=Cyc_stderr;_T30=
_tag_fat("------------------------------------------------------------------------------------------\n",sizeof(char),92U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2F,_T30,_T31);
_TL17B: _T32=ips;_T33=(unsigned)_T32;if(_T33)goto _TL179;else{goto _TL17A;}
_TL179: _T35=ips;_T36=_T35->hd;_T37=(struct _tuple21*)_T36;_T38=_T37->f3;if(!_T38)goto _TL17C;_T34="fat";goto _TL17D;_TL17C: _T34="thin";_TL17D:{const char*origtype=_T34;_T3A=ips;_T3B=_T3A->hd;_T3C=(struct _tuple21*)_T3B;_T3D=_T3C->f2;
if(!_T3D)goto _TL17E;_T39="fat";goto _TL17F;_TL17E: _T39="thin";_TL17F: {const char*inftype=_T39;{struct Cyc_String_pa_PrintArg_struct _T66;_T66.tag=0;_T40=ips;_T41=_T40->hd;_T42=(struct _tuple21*)_T41;{const char*_T67=_T42->f0;_T43=(void*)_T67;_T44=(void*)_T67;_T45=_get_zero_arr_size_char(_T44,1U);_T3F=_tag_fat(_T43,sizeof(char),_T45);}
_T66.f1=_T3F;_T3E=_T66;}{struct Cyc_String_pa_PrintArg_struct _T66=_T3E;{struct Cyc_String_pa_PrintArg_struct _T67;_T67.tag=0;_T48=ips;_T49=_T48->hd;_T4A=(struct _tuple21*)_T49;{const char*_T68=_T4A->f1;_T4B=(void*)_T68;_T4C=(void*)_T68;_T4D=_get_zero_arr_size_char(_T4C,1U);_T47=_tag_fat(_T4B,sizeof(char),_T4D);}_T67.f1=_T47;_T46=_T67;}{struct Cyc_String_pa_PrintArg_struct _T67=_T46;{struct Cyc_String_pa_PrintArg_struct _T68;_T68.tag=0;{const char*_T69=origtype;_T50=(void*)_T69;_T51=(void*)_T69;_T52=_get_zero_arr_size_char(_T51,1U);_T4F=_tag_fat(_T50,sizeof(char),_T52);}_T68.f1=_T4F;_T4E=_T68;}{struct Cyc_String_pa_PrintArg_struct _T68=_T4E;{struct Cyc_String_pa_PrintArg_struct _T69;_T69.tag=0;{const char*_T6A=inftype;_T55=(void*)_T6A;_T56=(void*)_T6A;_T57=_get_zero_arr_size_char(_T56,1U);_T54=_tag_fat(_T55,sizeof(char),_T57);}_T69.f1=_T54;_T53=_T69;}{struct Cyc_String_pa_PrintArg_struct _T69=_T53;void*_T6A[4];_T58=_T6A + 0;*_T58=& _T66;_T59=_T6A + 1;*_T59=& _T67;_T5A=_T6A + 2;*_T5A=& _T68;_T5B=_T6A + 3;*_T5B=& _T69;_T5C=Cyc_stderr;_T5D=_tag_fat("C_PTRBND_%-40s %-30s %-4s %-4s\n",sizeof(char),32U);_T5E=_tag_fat(_T6A,sizeof(void*),4);Cyc_fprintf(_T5C,_T5D,_T5E);}}}}}}_T5F=ips;
# 936
ips=_T5F->tl;goto _TL17B;_TL17A: _T60=Cyc_stderr;_T61=
# 956 "interproc.cyc"
_tag_fat("------------------------------------------------------------------------------------------\n",sizeof(char),92U);_T62=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T60,_T61,_T62);_T63=Cyc_stderr;_T64=
_tag_fat("That's all folks.\n",sizeof(char),19U);_T65=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T63,_T64,_T65);}}}

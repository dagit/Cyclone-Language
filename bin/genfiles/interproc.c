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
struct _fat_ptr Cyc_Interproc_make_parg(struct _fat_ptr ap){struct _fat_ptr _T0;struct _fat_ptr _T1;void**_T2;unsigned _T3;unsigned _T4;struct _fat_ptr _T5;unsigned char*_T6;void**_T7;unsigned _T8;int _T9;struct _fat_ptr _TA;_T1=ap;{unsigned _TB=
_get_fat_size(_T1,sizeof(void*));_T3=_check_times(_TB,sizeof(void*));{void**_TC=_cycalloc(_T3);{unsigned _TD=_TB;unsigned i;i=0;_TL3: if(i < _TD)goto _TL1;else{goto _TL2;}_TL1: _T4=i;_T5=ap;_T6=_T5.curr;_T7=(void**)_T6;_T8=i;_T9=(int)_T8;_TC[_T4]=_T7[_T9];i=i + 1;goto _TL3;_TL2:;}_T2=(void**)_TC;}_T0=_tag_fat(_T2,sizeof(void*),_TB);}{struct _fat_ptr ret=_T0;_TA=ret;
return _TA;}}
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
static struct Cyc___cycFILE*Cyc_Interproc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){struct _handler_cons*_T0;int _T1;struct _fat_ptr _T2;unsigned char*_T3;unsigned char*_T4;const char*_T5;struct _fat_ptr _T6;unsigned char*_T7;unsigned char*_T8;const char*_T9;void*_TA;struct Cyc_String_pa_PrintArg_struct _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct _handler_cons _TF;_T0=& _TF;_push_handler(_T0);{int _T10=0;_T1=setjmp(_TF.handler);if(!_T1)goto _TL6;_T10=1;goto _TL7;_TL6: _TL7: if(_T10)goto _TL8;else{goto _TLA;}_TLA: _T2=filename;_T3=_untag_fat_ptr_check_bound(_T2,sizeof(char),1U);_T4=_check_null(_T3);_T5=(const char*)_T4;_T6=mode;_T7=_untag_fat_ptr_check_bound(_T6,sizeof(char),1U);_T8=_check_null(_T7);_T9=(const char*)_T8;{struct Cyc___cycFILE*_T11=
Cyc_fopen(_T5,_T9);_npop_handler(0);return _T11;}_pop_handler();goto _TL9;_TL8: _TA=Cyc_Core_get_exn_thrown();{void*_T11=(void*)_TA;{struct Cyc_String_pa_PrintArg_struct _T12;_T12.tag=0;
# 63
_T12.f1=msg_part;_TB=_T12;}{struct Cyc_String_pa_PrintArg_struct _T12=_TB;{struct Cyc_String_pa_PrintArg_struct _T13;_T13.tag=0;_T13.f1=filename;_TC=_T13;}{struct Cyc_String_pa_PrintArg_struct _T13=_TC;void*_T14[2];_T14[0]=& _T12;_T14[1]=& _T13;_TD=_tag_fat("Couldn't open %s %s",sizeof(char),20U);_TE=_tag_fat(_T14,sizeof(void*),2);Cyc_Warn_err(0U,_TD,_TE);}}
return 0;;}_TL9:;}}
# 68
static struct _fat_ptr Cyc_Interproc_namespace2string(struct Cyc_List_List*ns){struct Cyc_List_List*_T0;struct _fat_ptr _T1;unsigned long _T2;int _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;_T0=ns;_T1=
_tag_fat("::",sizeof(char),3U);{struct _fat_ptr a=Cyc_str_sepstr(_T0,_T1);_T2=
Cyc_strlen(a);_T3=(int)_T2;if(!_T3)goto _TLB;_T4=a;_T5=
_tag_fat("::",sizeof(char),3U);_T6=Cyc_strconcat(_T4,_T5);return _T6;_TLB: _T7=a;
# 73
return _T7;}}
# 76
static void Cyc_Interproc_dump_decl_tags(struct Cyc_List_List*ns,struct _fat_ptr sourcefile,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct Cyc_String_pa_PrintArg_struct _T7;struct Cyc_String_pa_PrintArg_struct _T8;struct Cyc_Absyn_Fndecl*_T9;struct _tuple0*_TA;struct Cyc_String_pa_PrintArg_struct _TB;struct Cyc___cycFILE*_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct _fat_ptr _T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;
_TL10: _T0=tds;_T1=(unsigned)_T0;if(_T1)goto _TLE;else{goto _TLF;}
_TLE: _T2=tds;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T15=_T4->r;struct _tuple0*_T16;struct Cyc_List_List*_T17;struct _fat_ptr*_T18;struct Cyc_Absyn_Fndecl*_T19;_T5=(int*)_T15;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T15;_T19=_T1A->f1;}{struct Cyc_Absyn_Fndecl*fd=_T19;{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;
# 80
_T1A.f1=Cyc_Interproc_namespace2string(ns);_T7=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_T7;{struct Cyc_String_pa_PrintArg_struct _T1B;_T1B.tag=0;_T9=fd;_TA=_T9->name;
_T1B.f1=Cyc_Absynpp_fullqvar2string(_TA);_T8=_T1B;}{struct Cyc_String_pa_PrintArg_struct _T1B=_T8;{struct Cyc_String_pa_PrintArg_struct _T1C;_T1C.tag=0;
_T1C.f1=sourcefile;_TB=_T1C;}{struct Cyc_String_pa_PrintArg_struct _T1C=_TB;void*_T1D[3];_T1D[0]=& _T1A;_T1D[1]=& _T1B;_T1D[2]=& _T1C;_TC=
# 80
_check_null(Cyc_Interproc_tagsFile);_TD=_tag_fat("^%s%s^%s^function^0\n",sizeof(char),21U);_TE=_tag_fat(_T1D,sizeof(void*),3);Cyc_fprintf(_TC,_TD,_TE);}}}goto _LL0;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1A=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T15;_T18=_T1A->f1;_T17=_T1A->f2;}{struct _fat_ptr*n=_T18;struct Cyc_List_List*l=_T17;_TF=ns;{struct Cyc_List_List*_T1A=_cycalloc(sizeof(struct Cyc_List_List));
# 85
_T1A->hd=n;_T1A->tl=0;_T10=(struct Cyc_List_List*)_T1A;}_T11=Cyc_List_append(_TF,_T10);_T12=sourcefile;_T13=l;Cyc_Interproc_dump_decl_tags(_T11,_T12,_T13);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1A=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T15;_T16=_T1A->f1;_T17=_T1A->f2;}{struct _tuple0*n=_T16;struct Cyc_List_List*l=_T17;
# 88
Cyc_Interproc_dump_decl_tags(ns,sourcefile,l);goto _LL0;}default: goto _LL0;}_LL0:;}_T14=tds;
# 77
tds=_T14->tl;goto _TL10;_TLF:;}
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
static void Cyc_Interproc_print_filedecls(struct Cyc_List_List*fd){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_List_List*_T3;void*_T4;struct _tuple11*_T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_List_List*_T9;
_TL1D: _T0=fd;_T1=(unsigned)_T0;if(_T1)goto _TL1B;else{goto _TL1C;}
_TL1B:{struct Cyc_String_pa_PrintArg_struct _TA;_TA.tag=0;_T3=fd;_T4=_T3->hd;_T5=(struct _tuple11*)_T4;_TA.f1=_T5->f0;_T2=_TA;}{struct Cyc_String_pa_PrintArg_struct _TA=_T2;void*_TB[1];_TB[0]=& _TA;_T6=Cyc_stderr;_T7=_tag_fat("Decls for file %s::\n",sizeof(char),21U);_T8=_tag_fat(_TB,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}_T9=fd;
# 120
fd=_T9->tl;goto _TL1D;_TL1C:;}
# 125
static void Cyc_Interproc_skip_whitespace(struct Cyc___cycFILE*f){
int c=Cyc_getc(f);
_TL1E: if(c==32)goto _TL1F;else{goto _TL22;}_TL22: if(c==9)goto _TL1F;else{goto _TL21;}_TL21: if(c==10)goto _TL1F;else{goto _TL20;}
_TL1F: c=Cyc_getc(f);goto _TL1E;_TL20:
# 130
 Cyc_ungetc(c,f);}
# 132
static struct _fat_ptr Cyc_Interproc_grabline(struct Cyc___cycFILE*f){char*_T0;struct _handler_cons*_T1;int _T2;int _T3;int _T4;struct _fat_ptr _T5;int _T6;int _T7;unsigned char*_T8;char*_T9;int _TA;int _TB;int _TC;struct _fat_ptr _TD;int _TE;unsigned char*_TF;char*_T10;void*_T11;struct Cyc___cycFILE*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;struct _fat_ptr _T15;
static char buf[1000U];
int c=0;
int i=0;_T0=buf;{
struct _fat_ptr bufp=_tag_fat(_T0,sizeof(char),1000U);
Cyc_Interproc_skip_whitespace(f);{struct _handler_cons _T16;_T1=& _T16;_push_handler(_T1);{int _T17=0;_T2=setjmp(_T16.handler);if(!_T2)goto _TL23;_T17=1;goto _TL24;_TL23: _TL24: if(_T17)goto _TL25;else{goto _TL27;}_TL27:
# 139
 c=Cyc_getc(f);
_TL28: if(c!=10)goto _TL2B;else{goto _TL2A;}_TL2B: _T3=c;_T4=- 1;if(_T3!=_T4)goto _TL29;else{goto _TL2A;}
_TL29: _T5=bufp;_T6=i;i=_T6 + 1;_T7=_T6;_T8=_check_fat_subscript(_T5,sizeof(char),_T7);_T9=(char*)_T8;_TA=c;*_T9=(char)_TA;
c=Cyc_getc(f);goto _TL28;_TL2A: _TB=c;_TC=- 1;
# 144
if(_TB!=_TC)goto _TL2C;{struct _fat_ptr _T18=_tag_fat(0,0,0);_npop_handler(0);return _T18;}_TL2C:
 Cyc_ungetc(c,f);_TD=bufp;_TE=i;_TF=_check_fat_subscript(_TD,sizeof(char),_TE);_T10=(char*)_TF;
*_T10='\000';{struct _fat_ptr _T18=
Cyc_strdup(bufp);_npop_handler(0);return _T18;}_pop_handler();goto _TL26;_TL25: _T11=Cyc_Core_get_exn_thrown();{void*_T18=(void*)_T11;_T12=Cyc_stderr;_T13=
# 149
_tag_fat("grabline failed\n",sizeof(char),17U);_T14=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T12,_T13,_T14);goto _LL0;_LL0:;}_TL26:;}}_T15=
# 151
_tag_fat(0,0,0);return _T15;}}
# 154
static int Cyc_Interproc_mstrptrcmp(struct _fat_ptr*m1,struct _fat_ptr*m2){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct _fat_ptr*_T2;struct _fat_ptr _T3;int _T4;_T0=m1;_T1=*_T0;_T2=m2;_T3=*_T2;_T4=
Cyc_strcmp(_T1,_T3);return _T4;}
# 157
static int Cyc_Interproc_hash_mstringptr(struct _fat_ptr*m1){struct _fat_ptr*_T0;struct _fat_ptr _T1;int _T2;_T0=m1;_T1=*_T0;_T2=
Cyc_Hashtable_hash_string(_T1);return _T2;}
# 161
static void Cyc_Interproc_print_string(struct _fat_ptr*s){struct Cyc_String_pa_PrintArg_struct _T0;struct _fat_ptr*_T1;struct Cyc_Int_pa_PrintArg_struct _T2;int _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;{struct Cyc_String_pa_PrintArg_struct _T6;_T6.tag=0;_T1=s;
_T6.f1=*_T1;_T0=_T6;}{struct Cyc_String_pa_PrintArg_struct _T6=_T0;{struct Cyc_Int_pa_PrintArg_struct _T7;_T7.tag=1;_T3=Cyc_Interproc_hash_mstringptr(s);_T7.f1=(unsigned long)_T3;_T2=_T7;}{struct Cyc_Int_pa_PrintArg_struct _T7=_T2;void*_T8[2];_T8[0]=& _T6;_T8[1]=& _T7;_T4=_tag_fat("<%s>:%d",sizeof(char),8U);_T5=_tag_fat(_T8,sizeof(void*),2);Cyc_printf(_T4,_T5);}}}
# 165
static int Cyc_Interproc_init_tagmap_from_decls(struct _fat_ptr*source,struct Cyc_List_List*ds){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;void(*_T7)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*);void(*_T8)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T9;struct _fat_ptr*_TA;struct _fat_ptr _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc_Absyn_Fndecl*_TD;struct _tuple0*_TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct _fat_ptr*_T11;struct Cyc_List_List*_T12;
_TL31: _T0=ds;_T1=(unsigned)_T0;if(_T1)goto _TL2F;else{goto _TL30;}
_TL2F: _T2=ds;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T13=_T4->r;struct Cyc_List_List*_T14;struct Cyc_Absyn_Fndecl*_T15;_T5=(int*)_T13;_T6=*_T5;switch(_T6){case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T16=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T13;_T15=_T16->f1;}{struct Cyc_Absyn_Fndecl*fd=_T15;_T8=Cyc_Hashtable_insert;{
# 169
void(*_T16)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*))_T8;_T7=_T16;}_T9=_check_null(Cyc_Interproc_tagmap);{struct _fat_ptr*_T16=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T17;_T17.tag=0;_TD=fd;_TE=_TD->name;
_T17.f1=Cyc_Absynpp_fullqvar2string(_TE);_TC=_T17;}{struct Cyc_String_pa_PrintArg_struct _T17=_TC;void*_T18[1];_T18[0]=& _T17;_TF=_tag_fat("%s",sizeof(char),3U);_T10=_tag_fat(_T18,sizeof(void*),1);_TB=Cyc_aprintf(_TF,_T10);}*_T16=_TB;_TA=(struct _fat_ptr*)_T16;}_T11=source;
# 169
_T7(_T9,_TA,_T11);goto _LL0;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T16=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T13;_T14=_T16->f2;}{struct Cyc_List_List*l=_T14;_T14=l;goto _LL6;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T16=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T13;_T14=_T16->f2;}_LL6: {struct Cyc_List_List*l=_T14;
# 175
Cyc_Interproc_init_tagmap_from_decls(source,l);goto _LL0;}default: goto _LL0;}_LL0:;}_T12=ds;
# 166
ds=_T12->tl;goto _TL31;_TL30:
# 181
 return 1;}
# 185
static int Cyc_Interproc_init_tagmap(struct Cyc_List_List*ftds){struct Cyc_Hashtable_Table*_T0;unsigned _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct Cyc___cycFILE*_T5;unsigned _T6;struct Cyc_List_List*_T7;unsigned _T8;struct Cyc_Hashtable_Table*(*_T9)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_TA)(int,int(*)(void*,void*),int(*)(void*));struct _fat_ptr*_TB;struct _fat_ptr _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_List_List*_TE;void*_TF;struct _tuple11*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct Cyc_List_List*_T13;void*_T14;struct _tuple11*_T15;struct Cyc_List_List*_T16;int _T17;struct Cyc_Hashtable_Table*(*_T18)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T19)(int,int(*)(void*,void*),int(*)(void*));struct _fat_ptr _T1A;unsigned char*_T1B;unsigned _T1C;struct _fat_ptr _T1D;unsigned char*_T1E;char*_T1F;char _T20;int _T21;struct _fat_ptr*_T22;struct _fat_ptr _T23;struct Cyc_String_pa_PrintArg_struct _T24;struct _fat_ptr _T25;struct _fat_ptr _T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct _fat_ptr*_T29;struct _fat_ptr _T2A;struct Cyc_String_pa_PrintArg_struct _T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;struct _fat_ptr _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct _fat_ptr _T32;struct _fat_ptr _T33;int _T34;void(*_T35)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*);void(*_T36)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T37;struct _fat_ptr*_T38;struct _fat_ptr*_T39;_T0=Cyc_Interproc_tagmap;_T1=(unsigned)_T0;
if(!_T1)goto _TL33;
return 1;_TL33: _T2=
_tag_fat("CYCTAGS",sizeof(char),8U);_T3=_tag_fat("r",sizeof(char),2U);_T4=_tag_fat("CYCTAGS",sizeof(char),8U);{struct Cyc___cycFILE*tags=Cyc_Interproc_try_file_open(_T2,_T3,_T4);_T5=tags;_T6=(unsigned)_T5;
if(_T6)goto _TL35;else{goto _TL37;}
_TL37: _T7=ftds;_T8=(unsigned)_T7;if(_T8)goto _TL38;else{goto _TL3A;}
_TL3A: return 0;_TL38: _TA=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T3A)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_TA;_T9=_T3A;}Cyc_Interproc_tagmap=_T9(11,Cyc_Interproc_mstrptrcmp,Cyc_Interproc_hash_mstringptr);{struct _fat_ptr*_T3A=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T3B;_T3B.tag=0;_TE=ftds;_TF=_TE->hd;_T10=(struct _tuple11*)_TF;
_T3B.f1=_T10->f0;_TD=_T3B;}{struct Cyc_String_pa_PrintArg_struct _T3B=_TD;void*_T3C[1];_T3C[0]=& _T3B;_T11=_tag_fat("%s",sizeof(char),3U);_T12=_tag_fat(_T3C,sizeof(void*),1);_TC=Cyc_aprintf(_T11,_T12);}*_T3A=_TC;_TB=(struct _fat_ptr*)_T3A;}_T13=ftds;_T14=_T13->hd;_T15=(struct _tuple11*)_T14;_T16=_T15->f1;_T17=Cyc_Interproc_init_tagmap_from_decls(_TB,_T16);return _T17;_TL35: _T19=Cyc_Hashtable_create;{
# 195
struct Cyc_Hashtable_Table*(*_T3A)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T19;_T18=_T3A;}Cyc_Interproc_tagmap=_T18(11,Cyc_Interproc_mstrptrcmp,Cyc_Interproc_hash_mstringptr);{
struct _fat_ptr line;
_TL3B: line=Cyc_Interproc_grabline(tags);_T1A=line;_T1B=_T1A.curr;_T1C=(unsigned)_T1B;if(_T1C)goto _TL3C;else{goto _TL3D;}
_TL3C: _T1D=line;_T1E=_check_fat_subscript(_T1D,sizeof(char),0);_T1F=(char*)_T1E;_T20=*_T1F;_T21=(int)_T20;if(_T21==94)goto _TL3E;goto _TL3B;_TL3E:{
# 200
struct _fat_ptr*symname;symname=_cycalloc(sizeof(struct _fat_ptr));_T22=symname;{struct Cyc_String_pa_PrintArg_struct _T3A;_T3A.tag=0;_T25=line;_T26=_tag_fat("^@",sizeof(char),3U);_T3A.f1=Cyc_strtok(_T25,_T26);_T24=_T3A;}{struct Cyc_String_pa_PrintArg_struct _T3A=_T24;void*_T3B[1];_T3B[0]=& _T3A;_T27=_tag_fat("%s",sizeof(char),3U);_T28=_tag_fat(_T3B,sizeof(void*),1);_T23=Cyc_aprintf(_T27,_T28);}*_T22=_T23;{
struct _fat_ptr*filename;filename=_cycalloc(sizeof(struct _fat_ptr));_T29=filename;{struct Cyc_String_pa_PrintArg_struct _T3A;_T3A.tag=0;_T2C=_tag_fat(0,0,0);_T2D=_tag_fat("^@",sizeof(char),3U);_T3A.f1=Cyc_strtok(_T2C,_T2D);_T2B=_T3A;}{struct Cyc_String_pa_PrintArg_struct _T3A=_T2B;void*_T3B[1];_T3B[0]=& _T3A;_T2E=_tag_fat("%s",sizeof(char),3U);_T2F=_tag_fat(_T3B,sizeof(void*),1);_T2A=Cyc_aprintf(_T2E,_T2F);}*_T29=_T2A;_T30=
_tag_fat(0,0,0);_T31=_tag_fat("^@",sizeof(char),3U);{struct _fat_ptr symtype=Cyc_strtok(_T30,_T31);_T32=symtype;_T33=
_tag_fat("function",sizeof(char),9U);_T34=Cyc_strcmp(_T32,_T33);if(_T34)goto _TL40;else{goto _TL42;}
# 206
_TL42: _T36=Cyc_Hashtable_insert;{void(*_T3A)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct _fat_ptr*))_T36;_T35=_T3A;}_T37=_check_null(Cyc_Interproc_tagmap);_T38=symname;_T39=filename;_T35(_T37,_T38,_T39);goto _TL41;_TL40: _TL41:;}}}goto _TL3B;_TL3D:
# 209
 Cyc_fclose(tags);
return 1;}}}
# 214
static struct _fat_ptr Cyc_Interproc_locate_qvar(struct _tuple0*qv,struct Cyc_List_List*ftds){struct Cyc_Hashtable_Table*_T0;unsigned _T1;int _T2;struct Cyc_Core_Not_found_exn_struct*_T3;struct Cyc_Core_Not_found_exn_struct*_T4;struct _fat_ptr*_T5;struct _fat_ptr _T6;struct Cyc_String_pa_PrintArg_struct _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct _fat_ptr*(*_TA)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_TB)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TC;struct _fat_ptr*_TD;struct _fat_ptr*_TE;struct _fat_ptr _TF;_T0=Cyc_Interproc_tagmap;_T1=(unsigned)_T0;
if(_T1)goto _TL43;else{goto _TL45;}_TL45: _T2=Cyc_Interproc_init_tagmap(ftds);if(_T2)goto _TL43;else{goto _TL46;}
_TL46: _T3=& Cyc_Core_Not_found_val;_T4=(struct Cyc_Core_Not_found_exn_struct*)_T3;_throw(_T4);goto _TL44;_TL43: _TL44: {
# 219
struct _fat_ptr*qvname;qvname=_cycalloc(sizeof(struct _fat_ptr));_T5=qvname;{struct Cyc_String_pa_PrintArg_struct _T10;_T10.tag=0;_T10.f1=Cyc_Absynpp_fullqvar2string(qv);_T7=_T10;}{struct Cyc_String_pa_PrintArg_struct _T10=_T7;void*_T11[1];_T11[0]=& _T10;_T8=_tag_fat("%s",sizeof(char),3U);_T9=_tag_fat(_T11,sizeof(void*),1);_T6=Cyc_aprintf(_T8,_T9);}*_T5=_T6;_TB=Cyc_Hashtable_lookup;{
struct _fat_ptr*(*_T10)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct _fat_ptr*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_TB;_TA=_T10;}_TC=_check_null(Cyc_Interproc_tagmap);_TD=qvname;_TE=_TA(_TC,_TD);_TF=*_TE;return _TF;}}struct _tuple12{struct _fat_ptr f0;void*f1;};
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
static struct _tuple14 Cyc_Interproc_locate_externs(struct Cyc_List_List*es,struct Cyc_List_List*ftds){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Exp*_T2;int*_T3;int _T4;void*_T5;void*_T6;int*_T7;unsigned _T8;void*_T9;int _TA;struct Cyc_Interproc_Unresolved_sym_exn_struct*_TB;struct Cyc_List_List*_TC;void*_TD;void*_TE;struct _handler_cons*_TF;int _T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;void*_T13;void*_T14;struct Cyc_Core_Not_found_exn_struct*_T15;char*_T16;char*_T17;struct Cyc_Interproc_Unresolved_sym_exn_struct*_T18;struct Cyc_List_List*_T19;void*_T1A;void*_T1B;void*_T1C;struct _handler_cons*_T1D;int _T1E;struct Cyc_Absyn_Vardecl*_T1F;struct _tuple0*_T20;struct Cyc_List_List*_T21;int _T22;struct _fat_ptr _T23;struct Cyc_List_List*_T24;void*_T25;struct _tuple11*_T26;struct _fat_ptr _T27;int _T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;void*_T2B;struct Cyc_Absyn_Vardecl*_T2C;void*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;void*_T30;void*_T31;struct Cyc_Core_Not_found_exn_struct*_T32;char*_T33;char*_T34;struct Cyc_Warn_String_Warn_Warg_struct _T35;struct Cyc_Warn_Exp_Warn_Warg_struct _T36;struct Cyc_List_List*_T37;void*_T38;int(*_T39)(struct _fat_ptr);void*(*_T3A)(struct _fat_ptr);struct _fat_ptr _T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;struct Cyc_Warn_Exp_Warn_Warg_struct _T3D;struct Cyc_List_List*_T3E;void*_T3F;int(*_T40)(struct _fat_ptr);void*(*_T41)(struct _fat_ptr);struct _fat_ptr _T42;struct _fat_ptr _T43;unsigned char*_T44;unsigned _T45;int _T46;int(*_T47)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T48)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T49;struct _fat_ptr _T4A;int _T4B;struct _tuple14 _T4C;struct Cyc_List_List*_T4D;unsigned _T4E;struct Cyc_List_List*_T4F;struct _tuple14 _T50;
struct Cyc_List_List*newdecls=0;
struct Cyc_List_List*located=0;
_TL4F: if(es!=0)goto _TL4D;else{goto _TL4E;}
_TL4D:{struct _fat_ptr filename=_tag_fat(0,0,0);_T0=es;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Exp*)_T1;{
void*_T51=_T2->r;void*_T52;_T3=(int*)_T51;_T4=*_T3;if(_T4!=1)goto _TL50;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T53=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T51;_T5=_T53->f1;_T52=(void*)_T5;}{void*b=_T52;{struct Cyc_Absyn_Vardecl*_T53;struct _tuple0*_T54;_T6=b;_T7=(int*)_T6;_T8=*_T7;switch(_T8){case 0: _T9=b;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T9;_T54=_T55->f1;}{struct _tuple0*q=_T54;_TA=Cyc_Flags_no_merge;
# 265
if(!_TA)goto _TL53;{struct Cyc_Interproc_Unresolved_sym_exn_struct*_T55=_cycalloc(sizeof(struct Cyc_Interproc_Unresolved_sym_exn_struct));_T55->tag=Cyc_Interproc_Unresolved_sym;_TC=es;_TD=_TC->hd;
_T55->f1=(struct Cyc_Absyn_Exp*)_TD;_TB=(struct Cyc_Interproc_Unresolved_sym_exn_struct*)_T55;}_TE=(void*)_TB;_throw(_TE);goto _TL54;_TL53: _TL54:{struct _handler_cons _T55;_TF=& _T55;_push_handler(_TF);{int _T56=0;_T10=setjmp(_T55.handler);if(!_T10)goto _TL55;_T56=1;goto _TL56;_TL55: _TL56: if(_T56)goto _TL57;else{goto _TL59;}_TL59:
# 268
 filename=Cyc_Interproc_locate_qvar(q,ftds);{struct Cyc_List_List*_T57=_cycalloc(sizeof(struct Cyc_List_List));_T12=es;_T13=_T12->hd;
_T57->hd=(struct Cyc_Absyn_Exp*)_T13;_T57->tl=located;_T11=(struct Cyc_List_List*)_T57;}located=_T11;_pop_handler();goto _TL58;_TL57: _T14=Cyc_Core_get_exn_thrown();{void*_T57=(void*)_T14;void*_T58;_T15=(struct Cyc_Core_Not_found_exn_struct*)_T57;_T16=_T15->tag;_T17=Cyc_Core_Not_found;if(_T16!=_T17)goto _TL5A;{struct Cyc_Interproc_Unresolved_sym_exn_struct*_T59=_cycalloc(sizeof(struct Cyc_Interproc_Unresolved_sym_exn_struct));_T59->tag=Cyc_Interproc_Unresolved_sym;_T19=es;_T1A=_T19->hd;
# 271
_T59->f1=(struct Cyc_Absyn_Exp*)_T1A;_T18=(struct Cyc_Interproc_Unresolved_sym_exn_struct*)_T59;}_T1B=(void*)_T18;_throw(_T1B);goto _TL5B;_TL5A: _T58=_T57;{void*exn=_T58;_rethrow(exn);}_TL5B:;}_TL58:;}}goto _LL5;}case 1: _T1C=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T55=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T1C;_T53=_T55->f1;}{struct Cyc_Absyn_Vardecl*vd=_T53;{struct _handler_cons _T55;_T1D=& _T55;_push_handler(_T1D);{int _T56=0;_T1E=setjmp(_T55.handler);if(!_T1E)goto _TL5C;_T56=1;goto _TL5D;_TL5C: _TL5D: if(_T56)goto _TL5E;else{goto _TL60;}_TL60: _T1F=vd;_T20=_T1F->name;_T21=ftds;
# 276
filename=Cyc_Interproc_locate_qvar(_T20,_T21);_T22=Cyc_Flags_no_merge;
if(!_T22)goto _TL61;_T23=filename;_T24=
_check_null(ftds);_T25=_T24->hd;_T26=(struct _tuple11*)_T25;_T27=_T26->f0;_T28=Cyc_strcmp(_T23,_T27);if(_T28)goto _TL63;else{goto _TL65;}
# 280
_TL65:{struct Cyc_List_List*_T57=_cycalloc(sizeof(struct Cyc_List_List));_T2A=es;_T2B=_T2A->hd;_T57->hd=(struct Cyc_Absyn_Exp*)_T2B;_T57->tl=located;_T29=(struct Cyc_List_List*)_T57;}located=_T29;_npop_handler(0);goto _TL4C;
# 287
_TL63: _T2C=vd;_T2D=_T2C->type;Cyc_Tcutil_introduce_cvars(_T2D);
filename=_tag_fat(0,0,0);_npop_handler(0);goto _TL4C;
# 293
_TL61:{struct Cyc_List_List*_T57=_cycalloc(sizeof(struct Cyc_List_List));_T2F=es;_T30=_T2F->hd;_T57->hd=(struct Cyc_Absyn_Exp*)_T30;_T57->tl=located;_T2E=(struct Cyc_List_List*)_T57;}located=_T2E;_pop_handler();goto _TL5F;_TL5E: _T31=Cyc_Core_get_exn_thrown();{void*_T57=(void*)_T31;void*_T58;_T32=(struct Cyc_Core_Not_found_exn_struct*)_T57;_T33=_T32->tag;_T34=Cyc_Core_Not_found;if(_T33!=_T34)goto _TL66;goto _TL4C;_TL66: _T58=_T57;{void*exn=_T58;_rethrow(exn);};}_TL5F:;}}goto _LL5;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T55;_T55.tag=0;
# 303
_T55.f1=_tag_fat("locate_externs: Unexpected expression",sizeof(char),38U);_T35=_T55;}{struct Cyc_Warn_String_Warn_Warg_struct _T55=_T35;{struct Cyc_Warn_Exp_Warn_Warg_struct _T56;_T56.tag=4;_T37=es;_T38=_T37->hd;_T56.f1=(struct Cyc_Absyn_Exp*)_T38;_T36=_T56;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T56=_T36;void*_T57[2];_T57[0]=& _T55;_T57[1]=& _T56;_T3A=Cyc_Warn_impos2;{int(*_T58)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T3A;_T39=_T58;}_T3B=_tag_fat(_T57,sizeof(void*),2);_T39(_T3B);}}goto _LL5;}_LL5:;}goto _LL0;}_TL50:{struct Cyc_Warn_String_Warn_Warg_struct _T53;_T53.tag=0;
# 308
_T53.f1=_tag_fat("locate_externs: Unexpected expression",sizeof(char),38U);_T3C=_T53;}{struct Cyc_Warn_String_Warn_Warg_struct _T53=_T3C;{struct Cyc_Warn_Exp_Warn_Warg_struct _T54;_T54.tag=4;_T3E=es;_T3F=_T3E->hd;_T54.f1=(struct Cyc_Absyn_Exp*)_T3F;_T3D=_T54;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T54=_T3D;void*_T55[2];_T55[0]=& _T53;_T55[1]=& _T54;_T41=Cyc_Warn_impos2;{int(*_T56)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T41;_T40=_T56;}_T42=_tag_fat(_T55,sizeof(void*),2);_T40(_T42);}}goto _LL0;_LL0:;}_T43=filename;_T44=_T43.curr;_T45=(unsigned)_T44;
# 312
if(!_T45)goto _TL68;_T46=Cyc_Flags_no_merge;if(_T46)goto _TL68;else{goto _TL6A;}_TL6A: goto _TL69;_TL68: _T48=Cyc___assert_fail;{int(*_T51)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T48;_T47=_T51;}_T49=_tag_fat("filename && !Flags::no_merge",sizeof(char),29U);_T4A=_tag_fat("interproc.cyc",sizeof(char),14U);_T47(_T49,_T4A,312U);_TL69: {
struct _tuple13 _T51=Cyc_Interproc_add_filedecls(filename,ftds);struct Cyc_List_List*_T52;int _T53;_T53=_T51.f0;_T52=_T51.f1;{int b=_T53;struct Cyc_List_List*newl=_T52;_T4B=b;
if(_T4B)goto _TL6B;else{goto _TL6D;}
_TL6D:{struct _tuple14 _T54;_T54.f0=0;_T54.f1=0;_T54.f2=0;_T4C=_T54;}return _T4C;_TL6B: _T4D=newdecls;_T4E=(unsigned)_T4D;
if(_T4E)goto _TL6E;else{goto _TL70;}
_TL70: newdecls=newl;goto _TL6F;_TL6E: _TL6F:;}}}_TL4C: _T4F=es;
# 259
es=_T4F->tl;goto _TL4F;_TL4E:{struct _tuple14 _T51;
# 319
_T51.f0=1;_T51.f1=newdecls;_T51.f2=located;_T50=_T51;}return _T50;}
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
static void Cyc_Interproc_ensure_cvar_identity(unsigned loc,void*dest,void*src){struct _tuple15 _T0;void*_T1;int*_T2;unsigned _T3;void*_T4;int*_T5;int _T6;void*_T7;struct Cyc_Absyn_PtrInfo _T8;void*_T9;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TA;void**_TB;void*_TC;struct Cyc_Absyn_PtrInfo _TD;void*_TE;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TF;void**_T10;void**_T11;void*_T12;int _T13;void**_T14;void**_T15;void*_T16;int*_T17;int _T18;void*_T19;void*_T1A;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1B;struct Cyc_Absyn_FnInfo*_T1C;void*_T1D;void*_T1E;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1F;struct Cyc_Absyn_FnInfo*_T20;unsigned _T21;struct Cyc_Absyn_FnInfo*_T22;void*_T23;struct Cyc_Absyn_FnInfo*_T24;void*_T25;struct Cyc_Absyn_FnInfo*_T26;struct Cyc_List_List*_T27;int _T28;struct Cyc_Absyn_FnInfo*_T29;struct Cyc_List_List*_T2A;int _T2B;struct Cyc_Interproc_Rebinding_exn_struct*_T2C;struct Cyc_Interproc_Rebinding_exn_struct*_T2D;struct Cyc_Absyn_FnInfo*_T2E;struct Cyc_Absyn_FnInfo*_T2F;struct Cyc_List_List*_T30;unsigned _T31;struct Cyc_List_List*_T32;unsigned _T33;unsigned _T34;struct Cyc_List_List*_T35;void*_T36;struct _tuple8*_T37;void*_T38;struct Cyc_List_List*_T39;void*_T3A;struct _tuple8*_T3B;void*_T3C;struct Cyc_List_List*_T3D;struct Cyc_List_List*_T3E;void*_T3F;int*_T40;int _T41;void*_T42;void*_T43;int _T44;int _T45;struct Cyc_Warn_String_Warn_Warg_struct _T46;struct Cyc_Warn_Typ_Warn_Warg_struct _T47;unsigned _T48;struct _fat_ptr _T49;struct Cyc_List_List*_T4A;unsigned _T4B;struct Cyc_List_List*_T4C;void*_T4D;struct Cyc_List_List*_T4E;void*_T4F;int*_T50;int _T51;void*_T52;void*_T53;int _T54;int _T55;struct Cyc_Interproc_Rebinding_exn_struct*_T56;struct Cyc_Interproc_Rebinding_exn_struct*_T57;struct Cyc_List_List*_T58;unsigned _T59;struct Cyc_List_List*_T5A;unsigned _T5B;unsigned _T5C;struct Cyc_List_List*_T5D;void*_T5E;struct Cyc_List_List*_T5F;void*_T60;struct Cyc_List_List*_T61;struct Cyc_List_List*_T62;void*_T63;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T64;int _T65;void*_T66;int*_T67;int _T68;void*_T69;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T6A;int _T6B;void*_T6C;void*_T6D;int _T6E;int _T6F;struct Cyc_Interproc_Rebinding_exn_struct*_T70;struct Cyc_Interproc_Rebinding_exn_struct*_T71;struct Cyc_List_List*_T72;unsigned _T73;struct Cyc_List_List*_T74;unsigned _T75;unsigned _T76;struct Cyc_List_List*_T77;void*_T78;struct Cyc_Absyn_Aggrfield*_T79;void*_T7A;struct Cyc_List_List*_T7B;void*_T7C;struct Cyc_Absyn_Aggrfield*_T7D;void*_T7E;struct Cyc_List_List*_T7F;struct Cyc_List_List*_T80;{struct _tuple15 _T81;
_T81.f0=dest;_T81.f1=src;_T0=_T81;}{struct _tuple15 _T81=_T0;struct Cyc_List_List*_T82;struct _tuple0*_T83;struct Cyc_List_List*_T84;struct _tuple0*_T85;void*_T86;void*_T87;void*_T88;void*_T89;_T1=_T81.f0;_T2=(int*)_T1;_T3=*_T2;switch(_T3){case 4: _T4=_T81.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=4)goto _TL88;_T7=_T81.f0;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T7;_T8=_T8A->f1;_T89=_T8.elt_type;_T9=_T81.f0;_TA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T9;_TB=& _TA->f1.ptr_atts.bounds;_T88=(void**)_TB;}_TC=_T81.f1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TC;_TD=_T8A->f1;_T87=_TD.elt_type;_TE=_T81.f1;_TF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TE;_T10=& _TF->f1.ptr_atts.bounds;_T86=(void**)_T10;}{void*tD=_T89;void**bD=_T88;void*tS=_T87;void**bS=(void**)_T86;_T11=bS;_T12=*_T11;_T13=
# 377
Cyc_Tcutil_is_cvar_type(_T12);if(!_T13)goto _TL8A;_T14=bD;_T15=bS;
*_T14=*_T15;goto _TL8B;_TL8A: _TL8B:
 Cyc_Interproc_ensure_cvar_identity(loc,tD,tS);
return;}_TL88: goto _LLB;case 6: _T16=_T81.f1;_T17=(int*)_T16;_T18=*_T17;if(_T18!=6)goto _TL8C;_T19=_T81.f0;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T19;_T1A=_T81.f0;_T1B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1A;_T1C=& _T1B->f1;_T89=(struct Cyc_Absyn_FnInfo*)_T1C;}_T1D=_T81.f1;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1D;_T1E=_T81.f1;_T1F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1E;_T20=& _T1F->f1;_T88=(struct Cyc_Absyn_FnInfo*)_T20;}{struct Cyc_Absyn_FnInfo*iD=(struct Cyc_Absyn_FnInfo*)_T89;struct Cyc_Absyn_FnInfo*iS=(struct Cyc_Absyn_FnInfo*)_T88;_T21=loc;_T22=iD;_T23=_T22->ret_type;_T24=iS;_T25=_T24->ret_type;
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
return;}_TL8C: goto _LLB;case 8: _T3F=_T81.f1;_T40=(int*)_T3F;_T41=*_T40;if(_T41!=8)goto _TL95;_T42=_T81.f0;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T42;_T85=_T8A->f1;_T84=_T8A->f2;}_T43=_T81.f1;{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T43;_T83=_T8A->f1;_T82=_T8A->f2;}{struct _tuple0*nameD=_T85;struct Cyc_List_List*tD=_T84;struct _tuple0*nameS=_T83;struct Cyc_List_List*tS=_T82;_T44=
# 391
Cyc_List_length(tD);_T45=Cyc_List_length(tS);if(_T44==_T45)goto _TL97;{struct Cyc_Warn_String_Warn_Warg_struct _T8A;_T8A.tag=0;
# 396
_T8A.f1=_tag_fat("Typedef names could not be resolved yet; removing any constraint variables from ",sizeof(char),81U);_T46=_T8A;}{struct Cyc_Warn_String_Warn_Warg_struct _T8A=_T46;{struct Cyc_Warn_Typ_Warn_Warg_struct _T8B;_T8B.tag=2;_T8B.f1=src;_T47=_T8B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T8B=_T47;void*_T8C[2];_T8C[0]=& _T8A;_T8C[1]=& _T8B;_T48=loc;_T49=_tag_fat(_T8C,sizeof(void*),2);Cyc_Warn_warn2(_T48,_T49);}}
_TL9C: _T4A=tS;_T4B=(unsigned)_T4A;if(_T4B)goto _TL9A;else{goto _TL9B;}
_TL9A: _T4C=tS;_T4D=_T4C->hd;Cyc_Interproc_remove_cvars(_T4D);_T4E=tS;
# 397
tS=_T4E->tl;goto _TL9C;_TL9B:
# 399
 return;_TL97: _T84=tD;_T82=tS;goto _LL8;}_TL95: goto _LLB;case 0: _T4F=_T81.f1;_T50=(int*)_T4F;_T51=*_T50;if(_T51!=0)goto _TL9D;_T52=_T81.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T52;_T84=_T8A->f2;}_T53=_T81.f1;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T53;_T82=_T8A->f2;}_LL8:{struct Cyc_List_List*tD=_T84;struct Cyc_List_List*tS=_T82;_T54=
# 403
Cyc_List_length(tD);_T55=Cyc_List_length(tS);if(_T54==_T55)goto _TL9F;_T56=& Cyc_Interproc_Rebinding_val;_T57=(struct Cyc_Interproc_Rebinding_exn_struct*)_T56;_throw(_T57);goto _TLA0;_TL9F: _TLA0:
# 405
 _TLA4: _T58=tD;_T59=(unsigned)_T58;if(_T59)goto _TLA5;else{goto _TLA3;}_TLA5: _T5A=tS;_T5B=(unsigned)_T5A;if(_T5B)goto _TLA2;else{goto _TLA3;}
_TLA2: _T5C=loc;_T5D=tD;_T5E=_T5D->hd;_T5F=tS;_T60=_T5F->hd;Cyc_Interproc_ensure_cvar_identity(_T5C,_T5E,_T60);_T61=tD;
# 405
tD=_T61->tl;_T62=tS;tS=_T62->tl;goto _TLA4;_TLA3:
# 407
 return;}_TL9D: goto _LLB;case 7: _T63=_T81.f0;_T64=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T63;_T65=_T64->f2;if(_T65!=1)goto _TLA6;_T66=_T81.f1;_T67=(int*)_T66;_T68=*_T67;if(_T68!=7)goto _TLA8;_T69=_T81.f1;_T6A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T69;_T6B=_T6A->f2;if(_T6B!=1)goto _TLAA;_T6C=_T81.f0;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T6C;_T84=_T8A->f3;}_T6D=_T81.f1;{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T8A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T6D;_T82=_T8A->f3;}{struct Cyc_List_List*fieldD=_T84;struct Cyc_List_List*fieldS=_T82;_T6E=
# 409
Cyc_List_length(fieldD);_T6F=Cyc_List_length(fieldS);if(_T6E==_T6F)goto _TLAC;_T70=& Cyc_Interproc_Rebinding_val;_T71=(struct Cyc_Interproc_Rebinding_exn_struct*)_T70;_throw(_T71);goto _TLAD;_TLAC: _TLAD:
# 411
 _TLB1: _T72=fieldD;_T73=(unsigned)_T72;if(_T73)goto _TLB2;else{goto _TLB0;}_TLB2: _T74=fieldS;_T75=(unsigned)_T74;if(_T75)goto _TLAF;else{goto _TLB0;}
_TLAF: _T76=loc;_T77=fieldD;_T78=_T77->hd;_T79=(struct Cyc_Absyn_Aggrfield*)_T78;_T7A=_T79->type;_T7B=fieldS;_T7C=_T7B->hd;_T7D=(struct Cyc_Absyn_Aggrfield*)_T7C;_T7E=_T7D->type;Cyc_Interproc_ensure_cvar_identity(_T76,_T7A,_T7E);_T7F=fieldD;
# 411
fieldD=_T7F->tl;_T80=fieldS;fieldS=_T80->tl;goto _TLB1;_TLB0:
# 413
 return;}_TLAA: goto _LLB;_TLA8: goto _LLB;_TLA6: goto _LLB;default: _LLB:
# 415
 return;};}}
# 421
static void Cyc_Interproc_imp_update_prototypes(struct Cyc_List_List*decls,struct Cyc_Hashtable_Table*fndecl_tab){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct _handler_cons*_T7;int _T8;struct Cyc_Absyn_Fndecl*(*_T9)(struct Cyc_Hashtable_Table*,struct _tuple0*);void*(*_TA)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TB;struct Cyc_Absyn_Vardecl*_TC;struct _tuple0*_TD;void*_TE;struct Cyc_Core_Not_found_exn_struct*_TF;char*_T10;char*_T11;struct _handler_cons*_T12;int _T13;struct Cyc_List_List*_T14;void*_T15;struct Cyc_Absyn_Decl*_T16;unsigned _T17;struct Cyc_Absyn_Vardecl*_T18;void*_T19;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T1A;struct Cyc_Absyn_Fndecl*_T1B;void*_T1C;void*_T1D;struct Cyc_Interproc_Rebinding_exn_struct*_T1E;char*_T1F;char*_T20;struct Cyc_Warn_String_Warn_Warg_struct _T21;struct Cyc_Warn_Typ_Warn_Warg_struct _T22;struct Cyc_Absyn_Vardecl*_T23;struct Cyc_Warn_Typ_Warn_Warg_struct _T24;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T25;struct Cyc_Absyn_Fndecl*_T26;struct Cyc_List_List*_T27;void*_T28;struct Cyc_Absyn_Decl*_T29;unsigned _T2A;struct _fat_ptr _T2B;struct Cyc_List_List*_T2C;
# 423
_TLB6: _T0=decls;_T1=(unsigned)_T0;if(_T1)goto _TLB4;else{goto _TLB5;}
_TLB4: _T2=decls;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T2D=_T4->r;struct Cyc_List_List*_T2E;struct Cyc_Absyn_Vardecl*_T2F;_T5=(int*)_T2D;_T6=*_T5;switch(_T6){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T30=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2D;_T2F=_T30->f1;}{struct Cyc_Absyn_Vardecl*vd=_T2F;
# 426
struct Cyc_Absyn_Fndecl*fd;{struct _handler_cons _T30;_T7=& _T30;_push_handler(_T7);{int _T31=0;_T8=setjmp(_T30.handler);if(!_T8)goto _TLB8;_T31=1;goto _TLB9;_TLB8: _TLB9: if(_T31)goto _TLBA;else{goto _TLBC;}_TLBC: _TA=Cyc_Hashtable_lookup;{
# 428
struct Cyc_Absyn_Fndecl*(*_T32)(struct Cyc_Hashtable_Table*,struct _tuple0*)=(struct Cyc_Absyn_Fndecl*(*)(struct Cyc_Hashtable_Table*,struct _tuple0*))_TA;_T9=_T32;}_TB=fndecl_tab;_TC=vd;_TD=_TC->name;fd=_T9(_TB,_TD);_pop_handler();goto _TLBB;_TLBA: _TE=Cyc_Core_get_exn_thrown();{void*_T32=(void*)_TE;void*_T33;_TF=(struct Cyc_Core_Not_found_exn_struct*)_T32;_T10=_TF->tag;_T11=Cyc_Core_Not_found;if(_T10!=_T11)goto _TLBD;goto _TLB3;_TLBD: _T33=_T32;{void*exn=_T33;_rethrow(exn);};}_TLBB:;}}{struct _handler_cons _T30;_T12=& _T30;_push_handler(_T12);{int _T31=0;_T13=setjmp(_T30.handler);if(!_T13)goto _TLBF;_T31=1;goto _TLC0;_TLBF: _TLC0: if(_T31)goto _TLC1;else{goto _TLC3;}_TLC3: _T14=decls;_T15=_T14->hd;_T16=(struct Cyc_Absyn_Decl*)_T15;_T17=_T16->loc;_T18=vd;_T19=_T18->type;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T32=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T32->tag=6;_T1B=fd;
# 434
_T32->f1=_T1B->i;_T1A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T32;}_T1C=(void*)_T1A;Cyc_Interproc_ensure_cvar_identity(_T17,_T19,_T1C);_pop_handler();goto _TLC2;_TLC1: _T1D=Cyc_Core_get_exn_thrown();{void*_T32=(void*)_T1D;void*_T33;_T1E=(struct Cyc_Interproc_Rebinding_exn_struct*)_T32;_T1F=_T1E->tag;_T20=Cyc_Interproc_Rebinding;if(_T1F!=_T20)goto _TLC4;{struct Cyc_Warn_String_Warn_Warg_struct _T34;_T34.tag=0;
# 437
_T34.f1=_tag_fat("Incompatible types in declarations: ",sizeof(char),37U);_T21=_T34;}{struct Cyc_Warn_String_Warn_Warg_struct _T34=_T21;{struct Cyc_Warn_Typ_Warn_Warg_struct _T35;_T35.tag=2;_T23=vd;_T35.f1=_T23->type;_T22=_T35;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T35=_T22;{struct Cyc_Warn_Typ_Warn_Warg_struct _T36;_T36.tag=2;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T37=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T37->tag=6;_T26=fd;_T37->f1=_T26->i;_T25=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T37;}_T36.f1=(void*)_T25;_T24=_T36;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T36=_T24;void*_T37[3];_T37[0]=& _T34;_T37[1]=& _T35;_T37[2]=& _T36;_T27=decls;_T28=_T27->hd;_T29=(struct Cyc_Absyn_Decl*)_T28;_T2A=_T29->loc;_T2B=_tag_fat(_T37,sizeof(void*),3);Cyc_Warn_err2(_T2A,_T2B);}}}goto _LLE;_TLC4: _T33=_T32;{void*exn=_T33;_rethrow(exn);}_LLE:;}_TLC2:;}}goto _LL0;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T30=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T2D;_T2E=_T30->f2;}{struct Cyc_List_List*l=_T2E;_T2E=l;goto _LL6;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T30=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T2D;_T2E=_T30->f2;}_LL6: {struct Cyc_List_List*l=_T2E;
# 443
Cyc_Interproc_imp_update_prototypes(l,fndecl_tab);goto _LL0;}default: goto _LL0;}_LL0:;}_TLB3: _T2C=decls;
# 423
decls=_T2C->tl;goto _TLB6;_TLB5:;}
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
static void Cyc_Interproc_print_strptr(struct _fat_ptr*a){struct Cyc_String_pa_PrintArg_struct _T0;struct _fat_ptr*_T1;struct Cyc_Int_pa_PrintArg_struct _T2;int _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;{struct Cyc_String_pa_PrintArg_struct _T6;_T6.tag=0;_T1=a;
_T6.f1=*_T1;_T0=_T6;}{struct Cyc_String_pa_PrintArg_struct _T6=_T0;{struct Cyc_Int_pa_PrintArg_struct _T7;_T7.tag=1;_T3=Cyc_Hashtable_hash_stringptr(a);_T7.f1=(unsigned long)_T3;_T2=_T7;}{struct Cyc_Int_pa_PrintArg_struct _T7=_T2;void*_T8[2];_T8[0]=& _T6;_T8[1]=& _T7;_T4=_tag_fat("%s(%d)",sizeof(char),7U);_T5=_tag_fat(_T8,sizeof(void*),2);Cyc_printf(_T4,_T5);}}}
# 480
static void Cyc_Interproc_print_type(void*a){struct Cyc_String_pa_PrintArg_struct _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;{struct Cyc_String_pa_PrintArg_struct _T3;_T3.tag=0;
_T3.f1=Cyc_Absynpp_typ2string(a);_T0=_T3;}{struct Cyc_String_pa_PrintArg_struct _T3=_T0;void*_T4[1];_T4[0]=& _T3;_T1=_tag_fat("%s",sizeof(char),3U);_T2=_tag_fat(_T4,sizeof(void*),1);Cyc_printf(_T1,_T2);}}
# 483
static void Cyc_Interproc_dump_table(struct Cyc_Hashtable_Table*cvmap){void(*_T0)(struct Cyc_Hashtable_Table*,void(*)(struct _fat_ptr*),void(*)(void*));void(*_T1)(struct Cyc_Hashtable_Table*,void(*)(void*),void(*)(void*));struct Cyc_Hashtable_Table*_T2;_T1=Cyc_Hashtable_print_table_map;{
void(*_T3)(struct Cyc_Hashtable_Table*,void(*)(struct _fat_ptr*),void(*)(void*))=(void(*)(struct Cyc_Hashtable_Table*,void(*)(struct _fat_ptr*),void(*)(void*)))_T1;_T0=_T3;}_T2=cvmap;_T0(_T2,Cyc_Interproc_print_strptr,Cyc_Interproc_print_type);}struct _tuple16{struct Cyc_Absyn_Tqual f0;void*f1;};
# 489
static struct Cyc_List_List*Cyc_Interproc_cvars_in_type(void*t,int index,int is_local,int pfxpos){int(*_T0)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T1)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Hashtable_Table*_T4;unsigned _T5;struct Cyc_Hashtable_Table*(*_T6)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T7)(int,int(*)(void*,void*),int(*)(void*));int(*_T8)(struct _fat_ptr*,struct _fat_ptr*);int(*_T9)(struct _fat_ptr*);int*_TA;unsigned _TB;struct Cyc_Absyn_PtrInfo _TC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_TD;void**_TE;void*_TF;int _T10;int _T11;int _T12;void**_T13;int*_T14;int _T15;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T16;int*_T17;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T18;const char**_T19;int*_T1A;int _T1B;char*_T1C;struct _fat_ptr _T1D;int _T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_Int_pa_PrintArg_struct _T22;int _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_String_pa_PrintArg_struct _T26;char*_T27;struct _fat_ptr _T28;struct _fat_ptr _T29;void**(*_T2A)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));void**(*_T2B)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_Hashtable_Table*_T2C;struct _fat_ptr*_T2D;struct _fat_ptr*_T2E;int(*_T2F)(struct _fat_ptr*,struct _fat_ptr*);int(*_T30)(struct _fat_ptr*);void**_T31;unsigned _T32;void**_T33;void**_T34;const char**_T35;struct _fat_ptr _T36;unsigned char*_T37;struct Cyc_List_List*_T38;void**_T39;int _T3A;int*_T3B;int*_T3C;int _T3D;int*_T3E;void(*_T3F)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*);void(*_T40)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T41;struct _fat_ptr*_T42;void**_T43;void*_T44;char*_T45;struct _fat_ptr _T46;int _T47;unsigned char*_T48;char*_T49;unsigned _T4A;unsigned char*_T4B;char*_T4C;struct Cyc_Absyn_ArrayInfo _T4D;void*_T4E;int _T4F;int _T50;int _T51;struct Cyc_List_List*_T52;unsigned _T53;struct Cyc_List_List*_T54;void*_T55;char*_T56;struct _fat_ptr _T57;int _T58;struct _fat_ptr _T59;struct _fat_ptr _T5A;struct _fat_ptr _T5B;struct Cyc_Int_pa_PrintArg_struct _T5C;int _T5D;struct Cyc_Int_pa_PrintArg_struct _T5E;int _T5F;int _T60;struct _fat_ptr _T61;void*_T62;int _T63;char*_T64;struct _fat_ptr _T65;unsigned long _T66;int _T67;struct Cyc_List_List*_T68;struct Cyc_List_List*_T69;struct Cyc_List_List*_T6A;char*_T6B;struct _fat_ptr _T6C;int _T6D;unsigned char*_T6E;char*_T6F;unsigned _T70;unsigned char*_T71;char*_T72;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T73;void*_T74;int*_T75;unsigned _T76;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T77;void*_T78;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T79;union Cyc_Absyn_DatatypeInfo _T7A;struct _union_DatatypeInfo_KnownDatatype _T7B;unsigned _T7C;void*_T7D;union Cyc_Absyn_DatatypeInfo _T7E;struct _union_DatatypeInfo_KnownDatatype _T7F;struct Cyc_Absyn_Datatypedecl**_T80;struct Cyc_Absyn_Datatypedecl*_T81;struct Cyc_Core_Opt*_T82;unsigned _T83;struct Cyc_Absyn_Datatypedecl*_T84;struct Cyc_Core_Opt*_T85;void*_T86;struct Cyc_Absyn_Datatypedecl*_T87;struct _tuple0*_T88;struct Cyc_List_List*_T89;unsigned _T8A;struct Cyc_List_List*_T8B;void*_T8C;struct Cyc_Absyn_Datatypefield*_T8D;struct Cyc_List_List*_T8E;void*_T8F;struct Cyc_Absyn_Datatypefield*_T90;struct _tuple0*_T91;struct Cyc_List_List*_T92;unsigned _T93;struct Cyc_List_List*_T94;void*_T95;char*_T96;struct _fat_ptr _T97;int _T98;struct _fat_ptr _T99;struct _fat_ptr _T9A;struct _fat_ptr _T9B;struct Cyc_Int_pa_PrintArg_struct _T9C;struct Cyc_String_pa_PrintArg_struct _T9D;struct Cyc_Int_pa_PrintArg_struct _T9E;struct Cyc_String_pa_PrintArg_struct _T9F;struct Cyc_Int_pa_PrintArg_struct _TA0;int _TA1;int _TA2;struct _fat_ptr _TA3;void*_TA4;int _TA5;char*_TA6;struct _fat_ptr _TA7;unsigned long _TA8;int _TA9;struct Cyc_List_List*_TAA;struct Cyc_List_List*_TAB;struct Cyc_List_List*_TAC;struct Cyc_List_List*_TAD;char*_TAE;struct _fat_ptr _TAF;int _TB0;unsigned char*_TB1;char*_TB2;unsigned _TB3;unsigned char*_TB4;char*_TB5;struct Cyc_Absyn_Datatypedecl*_TB6;void*_TB7;union Cyc_Absyn_DatatypeInfo _TB8;struct _union_DatatypeInfo_UnknownDatatype _TB9;struct Cyc_Absyn_UnknownDatatypeInfo _TBA;struct Cyc_List_List*_TBB;unsigned _TBC;struct Cyc_List_List*_TBD;unsigned _TBE;char*_TBF;struct _fat_ptr _TC0;int _TC1;struct _fat_ptr _TC2;struct _fat_ptr _TC3;struct _fat_ptr _TC4;struct Cyc_Int_pa_PrintArg_struct _TC5;int _TC6;struct Cyc_Int_pa_PrintArg_struct _TC7;struct Cyc_String_pa_PrintArg_struct _TC8;struct Cyc_Int_pa_PrintArg_struct _TC9;int _TCA;struct _fat_ptr _TCB;struct Cyc_List_List*_TCC;void*_TCD;int _TCE;char*_TCF;struct _fat_ptr _TD0;unsigned long _TD1;int _TD2;struct Cyc_List_List*_TD3;struct Cyc_List_List*_TD4;struct Cyc_List_List*_TD5;char*_TD6;struct _fat_ptr _TD7;int _TD8;unsigned char*_TD9;char*_TDA;unsigned _TDB;unsigned char*_TDC;char*_TDD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TDE;void*_TDF;struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_TE0;union Cyc_Absyn_DatatypeFieldInfo _TE1;struct _union_DatatypeFieldInfo_KnownDatatypefield _TE2;unsigned _TE3;void*_TE4;union Cyc_Absyn_DatatypeFieldInfo _TE5;struct _union_DatatypeFieldInfo_KnownDatatypefield _TE6;struct _tuple1 _TE7;union Cyc_Absyn_DatatypeFieldInfo _TE8;struct _union_DatatypeFieldInfo_KnownDatatypefield _TE9;struct _tuple1 _TEA;int(*_TEB)(struct _fat_ptr,struct _fat_ptr);void*(*_TEC)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _TED;struct _fat_ptr _TEE;void*_TEF;union Cyc_Absyn_DatatypeFieldInfo _TF0;struct _union_DatatypeFieldInfo_UnknownDatatypefield _TF1;struct Cyc_Absyn_UnknownDatatypeFieldInfo _TF2;union Cyc_Absyn_DatatypeFieldInfo _TF3;struct _union_DatatypeFieldInfo_UnknownDatatypefield _TF4;struct Cyc_Absyn_UnknownDatatypeFieldInfo _TF5;struct Cyc_List_List*_TF6;unsigned _TF7;struct Cyc_List_List*_TF8;unsigned _TF9;char*_TFA;struct _fat_ptr _TFB;int _TFC;struct _fat_ptr _TFD;struct _fat_ptr _TFE;struct _fat_ptr _TFF;struct Cyc_Int_pa_PrintArg_struct _T100;int _T101;struct Cyc_Int_pa_PrintArg_struct _T102;struct Cyc_String_pa_PrintArg_struct _T103;struct Cyc_Int_pa_PrintArg_struct _T104;struct Cyc_String_pa_PrintArg_struct _T105;struct Cyc_Int_pa_PrintArg_struct _T106;int _T107;struct _fat_ptr _T108;struct Cyc_List_List*_T109;void*_T10A;int _T10B;char*_T10C;struct _fat_ptr _T10D;unsigned long _T10E;int _T10F;struct Cyc_List_List*_T110;struct Cyc_List_List*_T111;struct Cyc_List_List*_T112;char*_T113;struct _fat_ptr _T114;int _T115;unsigned char*_T116;char*_T117;unsigned _T118;unsigned char*_T119;char*_T11A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11B;void*_T11C;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T11D;union Cyc_Absyn_AggrInfo _T11E;struct _union_AggrInfo_KnownAggr _T11F;unsigned _T120;void*_T121;union Cyc_Absyn_AggrInfo _T122;struct _union_AggrInfo_KnownAggr _T123;struct Cyc_Absyn_Aggrdecl**_T124;struct Cyc_Absyn_Aggrdecl*_T125;struct _tuple0*_T126;struct Cyc_Absyn_Aggrdecl*_T127;struct Cyc_Absyn_AggrdeclImpl*_T128;unsigned _T129;struct Cyc_Absyn_Aggrdecl*_T12A;struct Cyc_Absyn_AggrdeclImpl*_T12B;struct Cyc_List_List*_T12C;unsigned _T12D;char*_T12E;struct _fat_ptr _T12F;int _T130;struct _fat_ptr _T131;struct _fat_ptr _T132;struct _fat_ptr _T133;struct Cyc_Int_pa_PrintArg_struct _T134;struct Cyc_String_pa_PrintArg_struct _T135;struct Cyc_Int_pa_PrintArg_struct _T136;int _T137;int _T138;struct _fat_ptr _T139;struct Cyc_List_List*_T13A;void*_T13B;struct Cyc_Absyn_Aggrfield*_T13C;void*_T13D;int _T13E;char*_T13F;struct _fat_ptr _T140;unsigned long _T141;int _T142;struct Cyc_List_List*_T143;struct Cyc_List_List*_T144;struct Cyc_List_List*_T145;char*_T146;struct _fat_ptr _T147;int _T148;unsigned char*_T149;char*_T14A;unsigned _T14B;unsigned char*_T14C;char*_T14D;struct Cyc_Absyn_Aggrdecl*_T14E;void*_T14F;union Cyc_Absyn_AggrInfo _T150;struct _union_AggrInfo_UnknownAggr _T151;struct _tuple2 _T152;struct Cyc_List_List*_T153;unsigned _T154;struct Cyc_List_List*_T155;unsigned _T156;char*_T157;struct _fat_ptr _T158;int _T159;struct _fat_ptr _T15A;struct _fat_ptr _T15B;struct _fat_ptr _T15C;struct Cyc_Int_pa_PrintArg_struct _T15D;int _T15E;struct Cyc_Int_pa_PrintArg_struct _T15F;struct Cyc_String_pa_PrintArg_struct _T160;struct Cyc_Int_pa_PrintArg_struct _T161;int _T162;struct _fat_ptr _T163;struct Cyc_List_List*_T164;void*_T165;int _T166;char*_T167;struct _fat_ptr _T168;unsigned long _T169;int _T16A;struct Cyc_List_List*_T16B;struct Cyc_List_List*_T16C;struct Cyc_List_List*_T16D;char*_T16E;struct _fat_ptr _T16F;int _T170;unsigned char*_T171;char*_T172;unsigned _T173;unsigned char*_T174;char*_T175;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T176;struct Cyc_Absyn_FnInfo*_T177;char*_T178;struct _fat_ptr _T179;int _T17A;struct _fat_ptr _T17B;struct _fat_ptr _T17C;struct _fat_ptr _T17D;struct Cyc_Int_pa_PrintArg_struct _T17E;int _T17F;struct _fat_ptr _T180;struct Cyc_Absyn_FnInfo*_T181;void*_T182;int _T183;char*_T184;struct _fat_ptr _T185;unsigned long _T186;int _T187;struct Cyc_List_List*_T188;struct Cyc_List_List*_T189;struct Cyc_Absyn_FnInfo*_T18A;struct Cyc_List_List*_T18B;unsigned _T18C;struct Cyc_List_List*_T18D;void*_T18E;char*_T18F;struct _fat_ptr _T190;int _T191;struct _fat_ptr _T192;struct _fat_ptr _T193;struct _fat_ptr _T194;struct Cyc_Int_pa_PrintArg_struct _T195;int _T196;struct Cyc_Int_pa_PrintArg_struct _T197;int _T198;int _T199;struct _fat_ptr _T19A;void*_T19B;int _T19C;char*_T19D;struct _fat_ptr _T19E;unsigned long _T19F;int _T1A0;struct Cyc_List_List*_T1A1;struct Cyc_List_List*_T1A2;struct Cyc_List_List*_T1A3;char*_T1A4;struct _fat_ptr _T1A5;int _T1A6;unsigned char*_T1A7;char*_T1A8;unsigned _T1A9;unsigned char*_T1AA;char*_T1AB;void*_T1AC;struct Cyc_Absyn_Typedefdecl*_T1AD;unsigned _T1AE;struct Cyc_Absyn_Typedefdecl*_T1AF;void*_T1B0;unsigned _T1B1;char*_T1B2;struct _fat_ptr _T1B3;int _T1B4;struct _fat_ptr _T1B5;struct _fat_ptr _T1B6;struct _fat_ptr _T1B7;struct Cyc_Int_pa_PrintArg_struct _T1B8;struct Cyc_String_pa_PrintArg_struct _T1B9;struct _fat_ptr _T1BA;struct Cyc_Absyn_Typedefdecl*_T1BB;void*_T1BC;void*_T1BD;int _T1BE;char*_T1BF;struct _fat_ptr _T1C0;unsigned long _T1C1;int _T1C2;struct Cyc_List_List*_T1C3;struct Cyc_List_List*_T1C4;char*_T1C5;struct _fat_ptr _T1C6;int _T1C7;unsigned char*_T1C8;char*_T1C9;unsigned _T1CA;unsigned char*_T1CB;char*_T1CC;struct Cyc_List_List*_T1CD;unsigned _T1CE;char*_T1CF;struct _fat_ptr _T1D0;int _T1D1;struct _fat_ptr _T1D2;struct _fat_ptr _T1D3;struct _fat_ptr _T1D4;struct Cyc_Int_pa_PrintArg_struct _T1D5;int _T1D6;struct Cyc_Int_pa_PrintArg_struct _T1D7;struct Cyc_String_pa_PrintArg_struct _T1D8;struct Cyc_Int_pa_PrintArg_struct _T1D9;int _T1DA;struct _fat_ptr _T1DB;struct Cyc_List_List*_T1DC;void*_T1DD;int _T1DE;char*_T1DF;struct _fat_ptr _T1E0;unsigned long _T1E1;int _T1E2;struct Cyc_List_List*_T1E3;struct Cyc_List_List*_T1E4;struct Cyc_List_List*_T1E5;char*_T1E6;struct _fat_ptr _T1E7;int _T1E8;unsigned char*_T1E9;char*_T1EA;unsigned _T1EB;unsigned char*_T1EC;char*_T1ED;struct Cyc_Absyn_TypeDecl*_T1EE;int*_T1EF;unsigned _T1F0;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T1F1;struct Cyc_Absyn_Aggrdecl**_T1F2;void*_T1F3;struct _fat_ptr _T1F4;void*_T1F5;int _T1F6;int _T1F7;struct Cyc_List_List*_T1F8;struct Cyc_List_List*_T1F9;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T1FA;struct Cyc_Absyn_Datatypedecl**_T1FB;void*_T1FC;struct _fat_ptr _T1FD;void*_T1FE;int _T1FF;int _T200;struct Cyc_List_List*_T201;struct Cyc_List_List*_T202;struct Cyc_List_List*_T203;
# 491
static struct Cyc_Hashtable_Table*scvmap=0;
if(pfxpos >= 900)goto _TLC6;goto _TLC7;_TLC6: _T1=Cyc___assert_fail;{int(*_T204)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T1;_T0=_T204;}_T2=_tag_fat("pfxpos < 900",sizeof(char),13U);_T3=_tag_fat("interproc.cyc",sizeof(char),14U);_T0(_T2,_T3,492U);_TLC7: _T4=scvmap;_T5=(unsigned)_T4;
if(_T5)goto _TLC8;else{goto _TLCA;}
_TLCA: _T7=Cyc_Hashtable_create;{struct Cyc_Hashtable_Table*(*_T204)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T7;_T6=_T204;}_T8=Cyc_strptrcmp;_T9=Cyc_Hashtable_hash_stringptr;scvmap=_T6(101,_T8,_T9);goto _TLC9;_TLC8: _TLC9: {
# 496
struct Cyc_Hashtable_Table*cvmap=scvmap;
struct Cyc_List_List*ret=0;{
void*_T204=Cyc_Absyn_compress(t);struct Cyc_Absyn_TypeDecl*_T205;struct Cyc_Absyn_Typedefdecl*_T206;struct _tuple0*_T207;struct Cyc_Absyn_Datatypefield*_T208;struct Cyc_List_List*_T209;void**_T20A;void*_T20B;_TA=(int*)_T204;_TB=*_TA;switch(_TB){case 4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T204;_TC=_T20C->f1;_T20B=_TC.elt_type;_TD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T204;_TE=& _TD->f1.ptr_atts.bounds;_T20A=(void**)_TE;}{void*elt=_T20B;void**bd=_T20A;_TF=elt;_T10=index + 1;_T11=is_local;_T12=pfxpos;
# 500
ret=Cyc_Interproc_cvars_in_type(_TF,_T10,_T11,_T12);_T13=bd;{
void*_T20C=*_T13;void*_T20D;void*_T20E;_T14=(int*)_T20C;_T15=*_T14;if(_T15!=3)goto _TLCC;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T20F=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T20C;_T16=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T20C;_T17=& _T16->f3;_T20E=(int*)_T17;_T18=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T20C;_T19=& _T18->f5;_T20D=(const char**)_T19;}{int*id=(int*)_T20E;const char**name=(const char**)_T20D;_T1A=id;_T1B=*_T1A;
# 503
if(_T1B < 0)goto _TLCE;goto _LL1B;_TLCE: _T1C=Cyc_Interproc__prefix;_T1D=
# 505
_tag_fat(_T1C,sizeof(char),1024U);_T1E=pfxpos;_T1F=_fat_ptr_plus(_T1D,sizeof(char),_T1E);_T20=_tag_fat("_%d",sizeof(char),4U);{struct Cyc_Int_pa_PrintArg_struct _T20F;_T20F.tag=1;_T23=index;_T20F.f1=(unsigned long)_T23;_T22=_T20F;}{struct Cyc_Int_pa_PrintArg_struct _T20F=_T22;void*_T210[1];_T210[0]=& _T20F;_T24=_tag_fat(_T210,sizeof(void*),1);_T21=Cyc_Interproc_make_parg(_T24);}Cyc_vsprintf(_T1F,_T20,_T21);{struct Cyc_String_pa_PrintArg_struct _T20F;_T20F.tag=0;_T27=Cyc_Interproc__prefix;
_T20F.f1=_tag_fat(_T27,sizeof(char),1024U);_T26=_T20F;}{struct Cyc_String_pa_PrintArg_struct _T20F=_T26;void*_T210[1];_T210[0]=& _T20F;_T28=_tag_fat("%s",sizeof(char),3U);_T29=_tag_fat(_T210,sizeof(void*),1);_T25=Cyc_aprintf(_T28,_T29);}{struct _fat_ptr nn=_T25;_T2B=Cyc_Hashtable_lookup_other_opt;{
# 508
void**(*_T20F)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(void**(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T2B;_T2A=_T20F;}_T2C=cvmap;_T2D=& nn;_T2E=(struct _fat_ptr*)_T2D;_T2F=Cyc_strptrcmp;_T30=Cyc_Hashtable_hash_stringptr;{void**val=_T2A(_T2C,_T2E,_T2F,_T30);_T31=val;_T32=(unsigned)_T31;
if(!_T32)goto _TLD0;_T33=bd;_T34=val;
*_T33=*_T34;goto _TLD1;
# 513
_TLD0: _T35=name;_T36=nn;_T37=_untag_fat_ptr_check_bound(_T36,sizeof(char),1U);*_T35=(const char*)_T37;{struct Cyc_List_List*_T20F=_cycalloc(sizeof(struct Cyc_List_List));_T39=bd;
_T20F->hd=*_T39;_T20F->tl=ret;_T38=(struct Cyc_List_List*)_T20F;}ret=_T38;_T3A=is_local;
if(_T3A)goto _TLD2;else{goto _TLD4;}
_TLD4: _T3B=id;_T3C=id;_T3D=*_T3C;*_T3B=- _T3D;goto _TLD3;
# 518
_TLD2: _T3E=id;*_T3E=0;_TLD3: _T40=Cyc_Hashtable_insert;{
void(*_T20F)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*))_T40;_T3F=_T20F;}_T41=cvmap;{struct _fat_ptr*_T20F=_cycalloc(sizeof(struct _fat_ptr));*_T20F=nn;_T42=(struct _fat_ptr*)_T20F;}_T43=bd;_T44=*_T43;_T3F(_T41,_T42,_T44);_TLD1: _T45=Cyc_Interproc__prefix;_T46=_tag_fat(_T45,sizeof(char),1024U);_T47=pfxpos;{struct _fat_ptr _T20F=_fat_ptr_plus(_T46,sizeof(char),_T47);_T48=_check_fat_subscript(_T20F,sizeof(char),0U);_T49=(char*)_T48;{char _T210=*_T49;char _T211='\000';_T4A=_get_fat_size(_T20F,sizeof(char));if(_T4A!=1U)goto _TLD5;if(_T210!=0)goto _TLD5;if(_T211==0)goto _TLD5;_throw_arraybounds();goto _TLD6;_TLD5: _TLD6: _T4B=_T20F.curr;_T4C=(char*)_T4B;*_T4C=_T211;}}goto _LL1B;}}}_TLCC: goto _LL1B;_LL1B:;}goto _LL0;}case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T204;_T4D=_T20C->f1;_T20B=_T4D.elt_type;}{void*elt=_T20B;_T4E=elt;_T4F=index + 1;_T50=is_local;_T51=pfxpos;
# 528
ret=Cyc_Interproc_cvars_in_type(_T4E,_T4F,_T50,_T51);goto _LL0;}case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T204;_T20B=_T20C->f3;}{struct Cyc_List_List*af=_T20B;
# 531
int findex=0;
_TLDA: _T52=af;_T53=(unsigned)_T52;if(_T53)goto _TLD8;else{goto _TLD9;}
_TLD8: _T54=af;_T55=_T54->hd;{struct Cyc_Absyn_Aggrfield*_T20C=(struct Cyc_Absyn_Aggrfield*)_T55;void*_T20D;{struct Cyc_Absyn_Aggrfield _T20E=*_T20C;_T20D=_T20E.type;}{void*t=_T20D;_T56=Cyc_Interproc__prefix;_T57=
# 535
_tag_fat(_T56,sizeof(char),1024U);_T58=pfxpos;_T59=_fat_ptr_plus(_T57,sizeof(char),_T58);_T5A=_tag_fat("_%d_AA%d",sizeof(char),9U);{struct Cyc_Int_pa_PrintArg_struct _T20E;_T20E.tag=1;_T5D=index;_T20E.f1=(unsigned long)_T5D;_T5C=_T20E;}{struct Cyc_Int_pa_PrintArg_struct _T20E=_T5C;{struct Cyc_Int_pa_PrintArg_struct _T20F;_T20F.tag=1;_T5F=findex;findex=_T5F + 1;_T60=_T5F;_T20F.f1=(unsigned long)_T60;_T5E=_T20F;}{struct Cyc_Int_pa_PrintArg_struct _T20F=_T5E;void*_T210[2];_T210[0]=& _T20E;_T210[1]=& _T20F;_T61=_tag_fat(_T210,sizeof(void*),2);_T5B=Cyc_Interproc_make_parg(_T61);}}Cyc_vsprintf(_T59,_T5A,_T5B);_T62=t;_T63=is_local;_T64=Cyc_Interproc__prefix;_T65=
_tag_fat(_T64,sizeof(char),1024U);_T66=Cyc_strlen(_T65);_T67=(int)_T66;_T68=Cyc_Interproc_cvars_in_type(_T62,0,_T63,_T67);_T69=ret;ret=Cyc_List_append(_T68,_T69);goto _LL20;}_LL20:;}_T6A=af;
# 532
af=_T6A->tl;goto _TLDA;_TLD9: _T6B=Cyc_Interproc__prefix;_T6C=_tag_fat(_T6B,sizeof(char),1024U);_T6D=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_T6C,sizeof(char),_T6D);_T6E=_check_fat_subscript(_T20C,sizeof(char),0U);_T6F=(char*)_T6E;{char _T20D=*_T6F;char _T20E='\000';_T70=_get_fat_size(_T20C,sizeof(char));if(_T70!=1U)goto _TLDB;if(_T20D!=0)goto _TLDB;if(_T20E==0)goto _TLDB;_throw_arraybounds();goto _TLDC;_TLDB: _TLDC: _T71=_T20C.curr;_T72=(char*)_T71;*_T72=_T20E;}}goto _LL0;}case 0: _T73=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_T74=_T73->f1;_T75=(int*)_T74;_T76=*_T75;switch(_T76){case 22: _T77=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_T78=_T77->f1;_T79=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T78;_T7A=_T79->f1;_T7B=_T7A.KnownDatatype;_T7C=_T7B.tag;if(_T7C!=2)goto _TLDE;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_T7D=_T20C->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T20D=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T7D;_T7E=_T20D->f1;_T7F=_T7E.KnownDatatype;_T80=_T7F.val;{struct Cyc_Absyn_Datatypedecl*_T20E=*_T80;_T20B=_T20E;}}_T209=_T20C->f2;}{struct Cyc_Absyn_Datatypedecl*dtd=_T20B;struct Cyc_List_List*ts=_T209;_T81=dtd;_T82=_T81->fields;_T83=(unsigned)_T82;
# 543
if(!_T83)goto _TLE0;_T84=dtd;_T85=_T84->fields;_T86=_T85->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T86;
int findex=0;_T87=dtd;_T88=_T87->name;{
struct _fat_ptr dtn=Cyc_Absynpp_fullqvar2string(_T88);
_TLE5: _T89=fs;_T8A=(unsigned)_T89;if(_T8A)goto _TLE3;else{goto _TLE4;}
_TLE3: _T8B=fs;_T8C=_T8B->hd;_T8D=(struct Cyc_Absyn_Datatypefield*)_T8C;{struct Cyc_List_List*ft=_T8D->typs;
int finner=0;_T8E=fs;_T8F=_T8E->hd;_T90=(struct Cyc_Absyn_Datatypefield*)_T8F;_T91=_T90->name;{
struct _fat_ptr fn=Cyc_Absynpp_fullqvar2string(_T91);
_TLE9: _T92=ft;_T93=(unsigned)_T92;if(_T93)goto _TLE7;else{goto _TLE8;}
_TLE7: _T94=ft;_T95=_T94->hd;{struct _tuple16*_T20C=(struct _tuple16*)_T95;void*_T20D;{struct _tuple16 _T20E=*_T20C;_T20D=_T20E.f1;}{void*t=_T20D;_T96=Cyc_Interproc__prefix;_T97=
_tag_fat(_T96,sizeof(char),1024U);_T98=pfxpos;_T99=_fat_ptr_plus(_T97,sizeof(char),_T98);_T9A=_tag_fat("DD%dI%s_DF%dI%s_%d",sizeof(char),19U);{struct Cyc_Int_pa_PrintArg_struct _T20E;_T20E.tag=1;_T20E.f1=Cyc_strlen(dtn);_T9C=_T20E;}{struct Cyc_Int_pa_PrintArg_struct _T20E=_T9C;{struct Cyc_String_pa_PrintArg_struct _T20F;_T20F.tag=0;_T20F.f1=dtn;_T9D=_T20F;}{struct Cyc_String_pa_PrintArg_struct _T20F=_T9D;{struct Cyc_Int_pa_PrintArg_struct _T210;_T210.tag=1;_T210.f1=Cyc_strlen(fn);_T9E=_T210;}{struct Cyc_Int_pa_PrintArg_struct _T210=_T9E;{struct Cyc_String_pa_PrintArg_struct _T211;_T211.tag=0;_T211.f1=fn;_T9F=_T211;}{struct Cyc_String_pa_PrintArg_struct _T211=_T9F;{struct Cyc_Int_pa_PrintArg_struct _T212;_T212.tag=1;_TA1=finner;finner=_TA1 + 1;_TA2=_TA1;_T212.f1=(unsigned long)_TA2;_TA0=_T212;}{struct Cyc_Int_pa_PrintArg_struct _T212=_TA0;void*_T213[5];_T213[0]=& _T20E;_T213[1]=& _T20F;_T213[2]=& _T210;_T213[3]=& _T211;_T213[4]=& _T212;_TA3=_tag_fat(_T213,sizeof(void*),5);_T9B=Cyc_Interproc_make_parg(_TA3);}}}}}Cyc_vsprintf(_T99,_T9A,_T9B);_TA4=t;_TA5=is_local;_TA6=Cyc_Interproc__prefix;_TA7=
_tag_fat(_TA6,sizeof(char),1024U);_TA8=Cyc_strlen(_TA7);_TA9=(int)_TA8;_TAA=Cyc_Interproc_cvars_in_type(_TA4,0,_TA5,_TA9);_TAB=ret;ret=Cyc_List_append(_TAA,_TAB);}}_TAC=ft;
# 551
ft=_TAC->tl;goto _TLE9;_TLE8:
# 556
 findex=findex + 1;}}_TAD=fs;
# 547
fs=_TAD->tl;goto _TLE5;_TLE4: _TAE=Cyc_Interproc__prefix;_TAF=_tag_fat(_TAE,sizeof(char),1024U);_TB0=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_TAF,sizeof(char),_TB0);_TB1=_check_fat_subscript(_T20C,sizeof(char),0U);_TB2=(char*)_TB1;{char _T20D=*_TB2;char _T20E='\000';_TB3=_get_fat_size(_T20C,sizeof(char));if(_TB3!=1U)goto _TLEA;if(_T20D!=0)goto _TLEA;if(_T20E==0)goto _TLEA;_throw_arraybounds();goto _TLEB;_TLEA: _TLEB: _TB4=_T20C.curr;_TB5=(char*)_TB4;*_TB5=_T20E;}}}}goto _TLE1;_TLE0: _TLE1: _TB6=dtd;_T20B=_TB6->name;_T209=ts;goto _LLA;}_TLDE:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_TB7=_T20C->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T20D=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_TB7;_TB8=_T20D->f1;_TB9=_TB8.UnknownDatatype;_TBA=_TB9.val;_T20B=_TBA.name;}_T209=_T20C->f2;}_LLA: {struct _tuple0*n=_T20B;struct Cyc_List_List*ts=_T209;_TBB=ts;_TBC=(unsigned)_TBB;
# 562
if(!_TBC)goto _TLEC;{
struct _fat_ptr adn=Cyc_Absynpp_fullqvar2string(n);{
int ix=0;_TLF1: _TBD=ts;_TBE=(unsigned)_TBD;if(_TBE)goto _TLEF;else{goto _TLF0;}
_TLEF: _TBF=Cyc_Interproc__prefix;_TC0=_tag_fat(_TBF,sizeof(char),1024U);_TC1=pfxpos;_TC2=_fat_ptr_plus(_TC0,sizeof(char),_TC1);_TC3=_tag_fat("_%dDD%dI%s_DDARG%d",sizeof(char),19U);{struct Cyc_Int_pa_PrintArg_struct _T20C;_T20C.tag=1;_TC6=index;_T20C.f1=(unsigned long)_TC6;_TC5=_T20C;}{struct Cyc_Int_pa_PrintArg_struct _T20C=_TC5;{struct Cyc_Int_pa_PrintArg_struct _T20D;_T20D.tag=1;_T20D.f1=Cyc_strlen(adn);_TC7=_T20D;}{struct Cyc_Int_pa_PrintArg_struct _T20D=_TC7;{struct Cyc_String_pa_PrintArg_struct _T20E;_T20E.tag=0;_T20E.f1=adn;_TC8=_T20E;}{struct Cyc_String_pa_PrintArg_struct _T20E=_TC8;{struct Cyc_Int_pa_PrintArg_struct _T20F;_T20F.tag=1;_TCA=ix;_T20F.f1=(unsigned long)_TCA;_TC9=_T20F;}{struct Cyc_Int_pa_PrintArg_struct _T20F=_TC9;void*_T210[4];_T210[0]=& _T20C;_T210[1]=& _T20D;_T210[2]=& _T20E;_T210[3]=& _T20F;_TCB=_tag_fat(_T210,sizeof(void*),4);_TC4=Cyc_Interproc_make_parg(_TCB);}}}}Cyc_vsprintf(_TC2,_TC3,_TC4);_TCC=ts;_TCD=_TCC->hd;_TCE=is_local;_TCF=Cyc_Interproc__prefix;_TD0=
_tag_fat(_TCF,sizeof(char),1024U);_TD1=Cyc_strlen(_TD0);_TD2=(int)_TD1;_TD3=Cyc_Interproc_cvars_in_type(_TCD,0,_TCE,_TD2);_TD4=ret;ret=Cyc_List_append(_TD3,_TD4);_TD5=ts;
# 564
ts=_TD5->tl;ix=ix + 1;goto _TLF1;_TLF0:;}_TD6=Cyc_Interproc__prefix;_TD7=_tag_fat(_TD6,sizeof(char),1024U);_TD8=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_TD7,sizeof(char),_TD8);_TD9=_check_fat_subscript(_T20C,sizeof(char),0U);_TDA=(char*)_TD9;{char _T20D=*_TDA;char _T20E='\000';_TDB=_get_fat_size(_T20C,sizeof(char));if(_TDB!=1U)goto _TLF2;if(_T20D!=0)goto _TLF2;if(_T20E==0)goto _TLF2;_throw_arraybounds();goto _TLF3;_TLF2: _TLF3: _TDC=_T20C.curr;_TDD=(char*)_TDC;*_TDD=_T20E;}}}goto _TLED;_TLEC: _TLED: goto _LL0;}case 23: _TDE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_TDF=_TDE->f1;_TE0=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TDF;_TE1=_TE0->f1;_TE2=_TE1.KnownDatatypefield;_TE3=_TE2.tag;if(_TE3!=2)goto _TLF4;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_TE4=_T20C->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T20D=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TE4;_TE5=_T20D->f1;_TE6=_TE5.KnownDatatypefield;_TE7=_TE6.val;_T20B=_TE7.f0;_TE8=_T20D->f1;_TE9=_TE8.KnownDatatypefield;_TEA=_TE9.val;_T208=_TEA.f1;}_T209=_T20C->f2;}{struct Cyc_Absyn_Datatypedecl*dd=_T20B;struct Cyc_Absyn_Datatypefield*dfd=_T208;struct Cyc_List_List*ts=_T209;_TEC=Cyc_Warn_impos;{
# 572
int(*_T20C)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_TEC;_TEB=_T20C;}_TED=_tag_fat("Unexpected DatatypeFieldCon in interproc cvar naming",sizeof(char),53U);_TEE=_tag_fat(0U,sizeof(void*),0);_TEB(_TED,_TEE);}goto _TLF5;_TLF4:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_TEF=_T20C->f1;{struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*_T20D=(struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_TEF;_TF0=_T20D->f1;_TF1=_TF0.UnknownDatatypefield;_TF2=_TF1.val;_T20B=_TF2.datatype_name;_TF3=_T20D->f1;_TF4=_TF3.UnknownDatatypefield;_TF5=_TF4.val;_T207=_TF5.field_name;}_T209=_T20C->f2;}{struct _tuple0*dn=_T20B;struct _tuple0*dfn=_T207;struct Cyc_List_List*ts=_T209;_TF6=ts;_TF7=(unsigned)_TF6;
# 574
if(!_TF7)goto _TLF6;{
struct _fat_ptr sdn=Cyc_Absynpp_fullqvar2string(dn);
struct _fat_ptr sdfn=Cyc_Absynpp_fullqvar2string(dfn);{
int ix=0;_TLFB: _TF8=ts;_TF9=(unsigned)_TF8;if(_TF9)goto _TLF9;else{goto _TLFA;}
_TLF9: _TFA=Cyc_Interproc__prefix;_TFB=_tag_fat(_TFA,sizeof(char),1024U);_TFC=pfxpos;_TFD=_fat_ptr_plus(_TFB,sizeof(char),_TFC);_TFE=_tag_fat("_%dDD%dI%s_DF%dI%s_DFARG%d",sizeof(char),27U);{struct Cyc_Int_pa_PrintArg_struct _T20C;_T20C.tag=1;_T101=index;_T20C.f1=(unsigned long)_T101;_T100=_T20C;}{struct Cyc_Int_pa_PrintArg_struct _T20C=_T100;{struct Cyc_Int_pa_PrintArg_struct _T20D;_T20D.tag=1;_T20D.f1=Cyc_strlen(sdn);_T102=_T20D;}{struct Cyc_Int_pa_PrintArg_struct _T20D=_T102;{struct Cyc_String_pa_PrintArg_struct _T20E;_T20E.tag=0;_T20E.f1=sdn;_T103=_T20E;}{struct Cyc_String_pa_PrintArg_struct _T20E=_T103;{struct Cyc_Int_pa_PrintArg_struct _T20F;_T20F.tag=1;_T20F.f1=Cyc_strlen(sdfn);_T104=_T20F;}{struct Cyc_Int_pa_PrintArg_struct _T20F=_T104;{struct Cyc_String_pa_PrintArg_struct _T210;_T210.tag=0;_T210.f1=sdfn;_T105=_T210;}{struct Cyc_String_pa_PrintArg_struct _T210=_T105;{struct Cyc_Int_pa_PrintArg_struct _T211;_T211.tag=1;_T107=ix;_T211.f1=(unsigned long)_T107;_T106=_T211;}{struct Cyc_Int_pa_PrintArg_struct _T211=_T106;void*_T212[6];_T212[0]=& _T20C;_T212[1]=& _T20D;_T212[2]=& _T20E;_T212[3]=& _T20F;_T212[4]=& _T210;_T212[5]=& _T211;_T108=_tag_fat(_T212,sizeof(void*),6);_TFF=Cyc_Interproc_make_parg(_T108);}}}}}}Cyc_vsprintf(_TFD,_TFE,_TFF);_T109=ts;_T10A=_T109->hd;_T10B=is_local;_T10C=Cyc_Interproc__prefix;_T10D=
_tag_fat(_T10C,sizeof(char),1024U);_T10E=Cyc_strlen(_T10D);_T10F=(int)_T10E;_T110=Cyc_Interproc_cvars_in_type(_T10A,0,_T10B,_T10F);_T111=ret;ret=Cyc_List_append(_T110,_T111);_T112=ts;
# 577
ts=_T112->tl;ix=ix + 1;goto _TLFB;_TLFA:;}_T113=Cyc_Interproc__prefix;_T114=_tag_fat(_T113,sizeof(char),1024U);_T115=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_T114,sizeof(char),_T115);_T116=_check_fat_subscript(_T20C,sizeof(char),0U);_T117=(char*)_T116;{char _T20D=*_T117;char _T20E='\000';_T118=_get_fat_size(_T20C,sizeof(char));if(_T118!=1U)goto _TLFC;if(_T20D!=0)goto _TLFC;if(_T20E==0)goto _TLFC;_throw_arraybounds();goto _TLFD;_TLFC: _TLFD: _T119=_T20C.curr;_T11A=(char*)_T119;*_T11A=_T20E;}}}goto _TLF7;_TLF6: _TLF7: goto _LL0;}_TLF5:;case 24: _T11B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_T11C=_T11B->f1;_T11D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T11C;_T11E=_T11D->f1;_T11F=_T11E.KnownAggr;_T120=_T11F.tag;if(_T120!=2)goto _TLFE;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_T121=_T20C->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T20D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T121;_T122=_T20D->f1;_T123=_T122.KnownAggr;_T124=_T123.val;{struct Cyc_Absyn_Aggrdecl*_T20E=*_T124;_T20B=_T20E;}}_T209=_T20C->f2;}{struct Cyc_Absyn_Aggrdecl*ad=_T20B;struct Cyc_List_List*ts=_T209;_T125=ad;_T126=_T125->name;{
# 585
struct _fat_ptr adn=Cyc_Absynpp_fullqvar2string(_T126);
int findex=0;_T127=ad;_T128=_T127->impl;_T129=(unsigned)_T128;
if(!_T129)goto _TL100;_T12A=ad;_T12B=_T12A->impl;{
struct Cyc_List_List*fs=_T12B->fields;_TL105: _T12C=fs;_T12D=(unsigned)_T12C;if(_T12D)goto _TL103;else{goto _TL104;}
_TL103: _T12E=Cyc_Interproc__prefix;_T12F=_tag_fat(_T12E,sizeof(char),1024U);_T130=pfxpos;_T131=_fat_ptr_plus(_T12F,sizeof(char),_T130);_T132=_tag_fat("AD%dI%s_AF%d",sizeof(char),13U);{struct Cyc_Int_pa_PrintArg_struct _T20C;_T20C.tag=1;_T20C.f1=Cyc_strlen(adn);_T134=_T20C;}{struct Cyc_Int_pa_PrintArg_struct _T20C=_T134;{struct Cyc_String_pa_PrintArg_struct _T20D;_T20D.tag=0;_T20D.f1=adn;_T135=_T20D;}{struct Cyc_String_pa_PrintArg_struct _T20D=_T135;{struct Cyc_Int_pa_PrintArg_struct _T20E;_T20E.tag=1;_T137=findex;findex=_T137 + 1;_T138=_T137;_T20E.f1=(unsigned long)_T138;_T136=_T20E;}{struct Cyc_Int_pa_PrintArg_struct _T20E=_T136;void*_T20F[3];_T20F[0]=& _T20C;_T20F[1]=& _T20D;_T20F[2]=& _T20E;_T139=_tag_fat(_T20F,sizeof(void*),3);_T133=Cyc_Interproc_make_parg(_T139);}}}Cyc_vsprintf(_T131,_T132,_T133);_T13A=fs;_T13B=_T13A->hd;_T13C=(struct Cyc_Absyn_Aggrfield*)_T13B;_T13D=_T13C->type;_T13E=is_local;_T13F=Cyc_Interproc__prefix;_T140=
_tag_fat(_T13F,sizeof(char),1024U);_T141=Cyc_strlen(_T140);_T142=(int)_T141;_T143=Cyc_Interproc_cvars_in_type(_T13D,0,_T13E,_T142);_T144=ret;ret=Cyc_List_append(_T143,_T144);_T145=fs;
# 588
fs=_T145->tl;goto _TL105;_TL104:;}_T146=Cyc_Interproc__prefix;_T147=_tag_fat(_T146,sizeof(char),1024U);_T148=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_T147,sizeof(char),_T148);_T149=_check_fat_subscript(_T20C,sizeof(char),0U);_T14A=(char*)_T149;{char _T20D=*_T14A;char _T20E='\000';_T14B=_get_fat_size(_T20C,sizeof(char));if(_T14B!=1U)goto _TL106;if(_T20D!=0)goto _TL106;if(_T20E==0)goto _TL106;_throw_arraybounds();goto _TL107;_TL106: _TL107: _T14C=_T20C.curr;_T14D=(char*)_T14C;*_T14D=_T20E;}}goto _TL101;_TL100: _TL101: _T14E=ad;_T20B=_T14E->name;_T209=ts;goto _LL12;}}_TLFE:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T204;_T14F=_T20C->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T20D=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T14F;_T150=_T20D->f1;_T151=_T150.UnknownAggr;_T152=_T151.val;_T20B=_T152.f1;}_T209=_T20C->f2;}_LL12: {struct _tuple0*n=_T20B;struct Cyc_List_List*ts=_T209;
# 596
struct _fat_ptr adn=Cyc_Absynpp_fullqvar2string(n);
int findex=0;_T153=ts;_T154=(unsigned)_T153;
if(!_T154)goto _TL108;{
int ix=0;_TL10D: _T155=ts;_T156=(unsigned)_T155;if(_T156)goto _TL10B;else{goto _TL10C;}
_TL10B: _T157=Cyc_Interproc__prefix;_T158=_tag_fat(_T157,sizeof(char),1024U);_T159=pfxpos;_T15A=_fat_ptr_plus(_T158,sizeof(char),_T159);_T15B=_tag_fat("_%dAD%dI%s_ADARG%d",sizeof(char),19U);{struct Cyc_Int_pa_PrintArg_struct _T20C;_T20C.tag=1;_T15E=index;_T20C.f1=(unsigned long)_T15E;_T15D=_T20C;}{struct Cyc_Int_pa_PrintArg_struct _T20C=_T15D;{struct Cyc_Int_pa_PrintArg_struct _T20D;_T20D.tag=1;_T20D.f1=Cyc_strlen(adn);_T15F=_T20D;}{struct Cyc_Int_pa_PrintArg_struct _T20D=_T15F;{struct Cyc_String_pa_PrintArg_struct _T20E;_T20E.tag=0;_T20E.f1=adn;_T160=_T20E;}{struct Cyc_String_pa_PrintArg_struct _T20E=_T160;{struct Cyc_Int_pa_PrintArg_struct _T20F;_T20F.tag=1;_T162=ix;_T20F.f1=(unsigned long)_T162;_T161=_T20F;}{struct Cyc_Int_pa_PrintArg_struct _T20F=_T161;void*_T210[4];_T210[0]=& _T20C;_T210[1]=& _T20D;_T210[2]=& _T20E;_T210[3]=& _T20F;_T163=_tag_fat(_T210,sizeof(void*),4);_T15C=Cyc_Interproc_make_parg(_T163);}}}}Cyc_vsprintf(_T15A,_T15B,_T15C);_T164=ts;_T165=_T164->hd;_T166=is_local;_T167=Cyc_Interproc__prefix;_T168=
_tag_fat(_T167,sizeof(char),1024U);_T169=Cyc_strlen(_T168);_T16A=(int)_T169;_T16B=Cyc_Interproc_cvars_in_type(_T165,0,_T166,_T16A);_T16C=ret;ret=Cyc_List_append(_T16B,_T16C);_T16D=ts;
# 599
ts=_T16D->tl;ix=ix + 1;goto _TL10D;_TL10C:;}_T16E=Cyc_Interproc__prefix;_T16F=_tag_fat(_T16E,sizeof(char),1024U);_T170=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_T16F,sizeof(char),_T170);_T171=_check_fat_subscript(_T20C,sizeof(char),0U);_T172=(char*)_T171;{char _T20D=*_T172;char _T20E='\000';_T173=_get_fat_size(_T20C,sizeof(char));if(_T173!=1U)goto _TL10E;if(_T20D!=0)goto _TL10E;if(_T20E==0)goto _TL10E;_throw_arraybounds();goto _TL10F;_TL10E: _TL10F: _T174=_T20C.curr;_T175=(char*)_T174;*_T175=_T20E;}}goto _TL109;_TL108: _TL109: goto _LL0;}default: goto _LL19;};case 6:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T204;_T176=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T204;_T177=& _T176->f1;_T20B=(struct Cyc_Absyn_FnInfo*)_T177;}{struct Cyc_Absyn_FnInfo*fi=_T20B;_T178=Cyc_Interproc__prefix;_T179=
# 607
_tag_fat(_T178,sizeof(char),1024U);_T17A=pfxpos;_T17B=_fat_ptr_plus(_T179,sizeof(char),_T17A);_T17C=_tag_fat("_%d_FR",sizeof(char),7U);{struct Cyc_Int_pa_PrintArg_struct _T20C;_T20C.tag=1;_T17F=index;_T20C.f1=(unsigned long)_T17F;_T17E=_T20C;}{struct Cyc_Int_pa_PrintArg_struct _T20C=_T17E;void*_T20D[1];_T20D[0]=& _T20C;_T180=_tag_fat(_T20D,sizeof(void*),1);_T17D=Cyc_Interproc_make_parg(_T180);}Cyc_vsprintf(_T17B,_T17C,_T17D);_T181=fi;_T182=_T181->ret_type;_T183=is_local;_T184=Cyc_Interproc__prefix;_T185=
_tag_fat(_T184,sizeof(char),1024U);_T186=Cyc_strlen(_T185);_T187=(int)_T186;_T188=Cyc_Interproc_cvars_in_type(_T182,0,_T183,_T187);_T189=ret;ret=Cyc_List_append(_T188,_T189);{
int argix=0;_T18A=fi;{
struct Cyc_List_List*args=_T18A->args;_TL113: _T18B=args;_T18C=(unsigned)_T18B;if(_T18C)goto _TL111;else{goto _TL112;}
_TL111: _T18D=args;_T18E=_T18D->hd;{struct _tuple8*_T20C=(struct _tuple8*)_T18E;void*_T20D;{struct _tuple8 _T20E=*_T20C;_T20D=_T20E.f2;}{void*t=_T20D;_T18F=Cyc_Interproc__prefix;_T190=
_tag_fat(_T18F,sizeof(char),1024U);_T191=pfxpos;_T192=_fat_ptr_plus(_T190,sizeof(char),_T191);_T193=_tag_fat("_%d_ARG%d",sizeof(char),10U);{struct Cyc_Int_pa_PrintArg_struct _T20E;_T20E.tag=1;_T196=index;_T20E.f1=(unsigned long)_T196;_T195=_T20E;}{struct Cyc_Int_pa_PrintArg_struct _T20E=_T195;{struct Cyc_Int_pa_PrintArg_struct _T20F;_T20F.tag=1;_T198=argix;argix=_T198 + 1;_T199=_T198;_T20F.f1=(unsigned long)_T199;_T197=_T20F;}{struct Cyc_Int_pa_PrintArg_struct _T20F=_T197;void*_T210[2];_T210[0]=& _T20E;_T210[1]=& _T20F;_T19A=_tag_fat(_T210,sizeof(void*),2);_T194=Cyc_Interproc_make_parg(_T19A);}}Cyc_vsprintf(_T192,_T193,_T194);_T19B=t;_T19C=is_local;_T19D=Cyc_Interproc__prefix;_T19E=
# 614
_tag_fat(_T19D,sizeof(char),1024U);_T19F=Cyc_strlen(_T19E);_T1A0=(int)_T19F;_T1A1=Cyc_Interproc_cvars_in_type(_T19B,0,_T19C,_T1A0);_T1A2=ret;ret=Cyc_List_append(_T1A1,_T1A2);}}_T1A3=args;
# 610
args=_T1A3->tl;goto _TL113;_TL112:;}_T1A4=Cyc_Interproc__prefix;_T1A5=_tag_fat(_T1A4,sizeof(char),1024U);_T1A6=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_T1A5,sizeof(char),_T1A6);_T1A7=_check_fat_subscript(_T20C,sizeof(char),0U);_T1A8=(char*)_T1A7;{char _T20D=*_T1A8;char _T20E='\000';_T1A9=_get_fat_size(_T20C,sizeof(char));if(_T1A9!=1U)goto _TL114;if(_T20D!=0)goto _TL114;if(_T20E==0)goto _TL114;_throw_arraybounds();goto _TL115;_TL114: _TL115: _T1AA=_T20C.curr;_T1AB=(char*)_T1AA;*_T1AB=_T20E;}}goto _LL0;}}case 8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T204;_T207=_T20C->f1;_T209=_T20C->f2;_T206=_T20C->f3;_T1AC=_T20C->f4;_T20B=(void*)_T1AC;}{struct _tuple0*tdn=_T207;struct Cyc_List_List*ts=_T209;struct Cyc_Absyn_Typedefdecl*tdef=_T206;void*topt=_T20B;
# 619
struct _fat_ptr tn=Cyc_Absynpp_fullqvar2string(tdn);_T1AD=tdef;_T1AE=(unsigned)_T1AD;
if(!_T1AE)goto _TL116;_T1AF=tdef;_T1B0=_T1AF->defn;_T1B1=(unsigned)_T1B0;if(!_T1B1)goto _TL116;_T1B2=Cyc_Interproc__prefix;_T1B3=
_tag_fat(_T1B2,sizeof(char),1024U);_T1B4=pfxpos;_T1B5=_fat_ptr_plus(_T1B3,sizeof(char),_T1B4);_T1B6=_tag_fat("TD%dI%s",sizeof(char),8U);{struct Cyc_Int_pa_PrintArg_struct _T20C;_T20C.tag=1;_T20C.f1=Cyc_strlen(tn);_T1B8=_T20C;}{struct Cyc_Int_pa_PrintArg_struct _T20C=_T1B8;{struct Cyc_String_pa_PrintArg_struct _T20D;_T20D.tag=0;_T20D.f1=tn;_T1B9=_T20D;}{struct Cyc_String_pa_PrintArg_struct _T20D=_T1B9;void*_T20E[2];_T20E[0]=& _T20C;_T20E[1]=& _T20D;_T1BA=_tag_fat(_T20E,sizeof(void*),2);_T1B7=Cyc_Interproc_make_parg(_T1BA);}}Cyc_vsprintf(_T1B5,_T1B6,_T1B7);_T1BB=tdef;_T1BC=_T1BB->defn;_T1BD=
_check_null(_T1BC);_T1BE=is_local;_T1BF=Cyc_Interproc__prefix;_T1C0=_tag_fat(_T1BF,sizeof(char),1024U);_T1C1=Cyc_strlen(_T1C0);_T1C2=(int)_T1C1;_T1C3=Cyc_Interproc_cvars_in_type(_T1BD,0,_T1BE,_T1C2);_T1C4=ret;ret=Cyc_List_append(_T1C3,_T1C4);_T1C5=Cyc_Interproc__prefix;_T1C6=_tag_fat(_T1C5,sizeof(char),1024U);_T1C7=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_T1C6,sizeof(char),_T1C7);_T1C8=_check_fat_subscript(_T20C,sizeof(char),0U);_T1C9=(char*)_T1C8;{char _T20D=*_T1C9;char _T20E='\000';_T1CA=_get_fat_size(_T20C,sizeof(char));if(_T1CA!=1U)goto _TL118;if(_T20D!=0)goto _TL118;if(_T20E==0)goto _TL118;_throw_arraybounds();goto _TL119;_TL118: _TL119: _T1CB=_T20C.curr;_T1CC=(char*)_T1CB;*_T1CC=_T20E;}}goto _TL117;_TL116: _TL117:{
# 626
int ix=0;_TL11D: _T1CD=ts;_T1CE=(unsigned)_T1CD;if(_T1CE)goto _TL11B;else{goto _TL11C;}
_TL11B: _T1CF=Cyc_Interproc__prefix;_T1D0=_tag_fat(_T1CF,sizeof(char),1024U);_T1D1=pfxpos;_T1D2=_fat_ptr_plus(_T1D0,sizeof(char),_T1D1);_T1D3=_tag_fat("_%dTD%dI%s_TDARG%d",sizeof(char),19U);{struct Cyc_Int_pa_PrintArg_struct _T20C;_T20C.tag=1;_T1D6=index;_T20C.f1=(unsigned long)_T1D6;_T1D5=_T20C;}{struct Cyc_Int_pa_PrintArg_struct _T20C=_T1D5;{struct Cyc_Int_pa_PrintArg_struct _T20D;_T20D.tag=1;_T20D.f1=Cyc_strlen(tn);_T1D7=_T20D;}{struct Cyc_Int_pa_PrintArg_struct _T20D=_T1D7;{struct Cyc_String_pa_PrintArg_struct _T20E;_T20E.tag=0;_T20E.f1=tn;_T1D8=_T20E;}{struct Cyc_String_pa_PrintArg_struct _T20E=_T1D8;{struct Cyc_Int_pa_PrintArg_struct _T20F;_T20F.tag=1;_T1DA=ix;_T20F.f1=(unsigned long)_T1DA;_T1D9=_T20F;}{struct Cyc_Int_pa_PrintArg_struct _T20F=_T1D9;void*_T210[4];_T210[0]=& _T20C;_T210[1]=& _T20D;_T210[2]=& _T20E;_T210[3]=& _T20F;_T1DB=_tag_fat(_T210,sizeof(void*),4);_T1D4=Cyc_Interproc_make_parg(_T1DB);}}}}Cyc_vsprintf(_T1D2,_T1D3,_T1D4);_T1DC=ts;_T1DD=_T1DC->hd;_T1DE=is_local;_T1DF=Cyc_Interproc__prefix;_T1E0=
_tag_fat(_T1DF,sizeof(char),1024U);_T1E1=Cyc_strlen(_T1E0);_T1E2=(int)_T1E1;_T1E3=Cyc_Interproc_cvars_in_type(_T1DD,0,_T1DE,_T1E2);_T1E4=ret;ret=Cyc_List_append(_T1E3,_T1E4);_T1E5=ts;
# 626
ts=_T1E5->tl;ix=ix + 1;goto _TL11D;_TL11C:;}_T1E6=Cyc_Interproc__prefix;_T1E7=_tag_fat(_T1E6,sizeof(char),1024U);_T1E8=pfxpos;{struct _fat_ptr _T20C=_fat_ptr_plus(_T1E7,sizeof(char),_T1E8);_T1E9=_check_fat_subscript(_T20C,sizeof(char),0U);_T1EA=(char*)_T1E9;{char _T20D=*_T1EA;char _T20E='\000';_T1EB=_get_fat_size(_T20C,sizeof(char));if(_T1EB!=1U)goto _TL11E;if(_T20D!=0)goto _TL11E;if(_T20E==0)goto _TL11E;_throw_arraybounds();goto _TL11F;_TL11E: _TL11F: _T1EC=_T20C.curr;_T1ED=(char*)_T1EC;*_T1ED=_T20E;}}goto _LL0;}case 10:{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T20C=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T204;_T205=_T20C->f1;_T20A=_T20C->f2;}{struct Cyc_Absyn_TypeDecl*td=_T205;void**to=_T20A;_T1EE=td;{
# 633
void*_T20C=_T1EE->r;struct Cyc_Absyn_Datatypedecl*_T20D;struct Cyc_Absyn_Aggrdecl*_T20E;_T1EF=(int*)_T20C;_T1F0=*_T1EF;switch(_T1F0){case 0:{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_T20F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T20C;_T20E=_T20F->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T20E;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T20F=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T20F->tag=24;{struct Cyc_Absyn_Aggrdecl**_T210=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 635
*_T210=ad;_T1F2=(struct Cyc_Absyn_Aggrdecl**)_T210;}_T20F->f1=Cyc_Absyn_KnownAggr(_T1F2);_T1F1=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T20F;}_T1F3=(void*)_T1F1;_T1F4=_tag_fat(0U,sizeof(void*),0);_T1F5=Cyc_Absyn_app_type(_T1F3,_T1F4);_T1F6=is_local;_T1F7=pfxpos;_T1F8=Cyc_Interproc_cvars_in_type(_T1F5,0,_T1F6,_T1F7);_T1F9=ret;ret=Cyc_List_append(_T1F8,_T1F9);goto _LL29;}case 2:{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_T20F=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T20C;_T20D=_T20F->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T20D;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T20F=_cycalloc(sizeof(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct));_T20F->tag=22;{struct Cyc_Absyn_Datatypedecl**_T210=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));
# 639
*_T210=tud;_T1FB=(struct Cyc_Absyn_Datatypedecl**)_T210;}_T20F->f1=Cyc_Absyn_KnownDatatype(_T1FB);_T1FA=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T20F;}_T1FC=(void*)_T1FA;_T1FD=_tag_fat(0U,sizeof(void*),0);_T1FE=Cyc_Absyn_app_type(_T1FC,_T1FD);_T1FF=is_local;_T200=pfxpos;_T201=Cyc_Interproc_cvars_in_type(_T1FE,0,_T1FF,_T200);_T202=ret;ret=Cyc_List_append(_T201,_T202);goto _LL29;}default: goto _LL29;}_LL29:;}goto _LL0;}default: _LL19: goto _LL0;}_LL0:;}_T203=ret;
# 649
return _T203;}}
# 653
static int Cyc_Interproc_exp_noop(void*a,struct Cyc_Absyn_Exp*b){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;void*_T3;_T0=b;{
void*_T4=_T0->r;void*_T5;_T1=(int*)_T4;_T2=*_T1;if(_T2!=14)goto _TL121;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T6=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T4;_T3=_T6->f1;_T5=(void*)_T3;}{void*t=_T5;
# 656
Cyc_Tcutil_revert_cvars(t);goto _LL0;}_TL121: goto _LL0;_LL0:;}
# 661
return 1;}struct _tuple17{struct _fat_ptr f0;int f1;void(*f2)(struct Cyc_List_List*,void*);void*f3;};
# 666
static int Cyc_Interproc_cvars_in_local_decl(struct _tuple17*env,struct Cyc_Absyn_Stmt*s){struct _tuple17*_T0;struct _tuple17*_T1;int*_T2;void*_T3;struct Cyc_Absyn_Stmt*_T4;int*_T5;int _T6;struct Cyc_Absyn_Decl*_T7;int*_T8;unsigned _T9;struct Cyc_Absyn_Vardecl*_TA;struct _tuple0*_TB;char*_TC;unsigned _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_Int_pa_PrintArg_struct _T11;struct Cyc_String_pa_PrintArg_struct _T12;struct Cyc_Int_pa_PrintArg_struct _T13;struct Cyc_String_pa_PrintArg_struct _T14;struct Cyc_Int_pa_PrintArg_struct _T15;int*_T16;int _T17;int _T18;struct _fat_ptr _T19;struct Cyc_Absyn_Vardecl*_T1A;void*_T1B;char*_T1C;struct _fat_ptr _T1D;unsigned long _T1E;int _T1F;struct Cyc_Absyn_Pat*_T20;int*_T21;int _T22;struct Cyc_Absyn_Vardecl*_T23;struct _tuple0*_T24;char*_T25;unsigned _T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct _fat_ptr _T29;struct Cyc_Int_pa_PrintArg_struct _T2A;struct Cyc_String_pa_PrintArg_struct _T2B;struct Cyc_Int_pa_PrintArg_struct _T2C;struct Cyc_String_pa_PrintArg_struct _T2D;struct Cyc_Int_pa_PrintArg_struct _T2E;int*_T2F;int _T30;int _T31;struct _fat_ptr _T32;struct Cyc_Absyn_Vardecl*_T33;void*_T34;char*_T35;struct _fat_ptr _T36;unsigned long _T37;int _T38;void*_T39;void(*_T3A)(struct Cyc_List_List*,void*);void*_T3B;struct _fat_ptr _T3C;_T0=env;{struct _tuple17 _T3D=*_T0;_T3C=_T3D.f0;_T1=env;_T2=& _T1->f1;_T3B=(int*)_T2;_T3A=_T3D.f2;_T3=_T3D.f3;_T39=(void*)_T3;}{struct _fat_ptr pfx=_T3C;int*count=(int*)_T3B;void(*fn)(struct Cyc_List_List*,void*)=_T3A;void*inner_env=_T39;_T4=s;{
# 668
void*_T3D=_T4->r;struct Cyc_Absyn_Stmt*_T3E;struct Cyc_Absyn_Decl*_T3F;_T5=(int*)_T3D;_T6=*_T5;if(_T6!=12)goto _TL123;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T40=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T3D;_T3F=_T40->f1;_T3E=_T40->f2;}{struct Cyc_Absyn_Decl*d=_T3F;struct Cyc_Absyn_Stmt*s1=_T3E;_T7=d;{
# 670
void*_T40=_T7->r;struct Cyc_Absyn_Pat*_T41;struct Cyc_Absyn_Vardecl*_T42;_T8=(int*)_T40;_T9=*_T8;switch(_T9){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T43=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T40;_T42=_T43->f1;}{struct Cyc_Absyn_Vardecl*vd=_T42;_TA=vd;_TB=_TA->name;{
# 672
struct _fat_ptr ln=Cyc_Absynpp_fullqvar2string(_TB);_TC=Cyc_Interproc__prefix;_TD=1024U - 1U;_TE=
_tag_fat(_TC,sizeof(char),_TD);_TF=_tag_fat("%dI%s_%dI%s_%d",sizeof(char),15U);{struct Cyc_Int_pa_PrintArg_struct _T43;_T43.tag=1;
_T43.f1=Cyc_strlen(pfx);_T11=_T43;}{struct Cyc_Int_pa_PrintArg_struct _T43=_T11;{struct Cyc_String_pa_PrintArg_struct _T44;_T44.tag=0;_T44.f1=pfx;_T12=_T44;}{struct Cyc_String_pa_PrintArg_struct _T44=_T12;{struct Cyc_Int_pa_PrintArg_struct _T45;_T45.tag=1;_T45.f1=Cyc_strlen(ln);_T13=_T45;}{struct Cyc_Int_pa_PrintArg_struct _T45=_T13;{struct Cyc_String_pa_PrintArg_struct _T46;_T46.tag=0;_T46.f1=ln;_T14=_T46;}{struct Cyc_String_pa_PrintArg_struct _T46=_T14;{struct Cyc_Int_pa_PrintArg_struct _T47;_T47.tag=1;_T16=count;_T17=*_T16;*_T16=_T17 + 1;_T18=_T17;_T47.f1=(unsigned long)_T18;_T15=_T47;}{struct Cyc_Int_pa_PrintArg_struct _T47=_T15;void*_T48[5];_T48[0]=& _T43;_T48[1]=& _T44;_T48[2]=& _T45;_T48[3]=& _T46;_T48[4]=& _T47;_T19=_tag_fat(_T48,sizeof(void*),5);_T10=Cyc_Interproc_make_parg(_T19);}}}}}
# 673
Cyc_vsprintf(_TE,_TF,_T10);_T1A=vd;_T1B=_T1A->type;_T1C=Cyc_Interproc__prefix;_T1D=
# 675
_tag_fat(_T1C,sizeof(char),1024U);_T1E=Cyc_strlen(_T1D);_T1F=(int)_T1E;{struct Cyc_List_List*cvars=Cyc_Interproc_cvars_in_type(_T1B,0,1,_T1F);
# 679
fn(cvars,inner_env);goto _LL8;}}}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T43=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T40;_T41=_T43->f1;}{struct Cyc_Absyn_Pat*p=_T41;_T20=p;{
# 682
void*_T43=_T20->r;struct Cyc_Absyn_Vardecl*_T44;_T21=(int*)_T43;_T22=*_T21;if(_T22!=2)goto _TL126;{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T45=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T43;_T44=_T45->f2;}{struct Cyc_Absyn_Vardecl*vd=_T44;_T23=vd;_T24=_T23->name;{
# 684
struct _fat_ptr an=Cyc_Absynpp_fullqvar2string(_T24);_T25=Cyc_Interproc__prefix;_T26=1024U - 1U;_T27=
_tag_fat(_T25,sizeof(char),_T26);_T28=_tag_fat("%dI%s_%dI%s_%d",sizeof(char),15U);{struct Cyc_Int_pa_PrintArg_struct _T45;_T45.tag=1;_T45.f1=Cyc_strlen(pfx);_T2A=_T45;}{struct Cyc_Int_pa_PrintArg_struct _T45=_T2A;{struct Cyc_String_pa_PrintArg_struct _T46;_T46.tag=0;_T46.f1=pfx;_T2B=_T46;}{struct Cyc_String_pa_PrintArg_struct _T46=_T2B;{struct Cyc_Int_pa_PrintArg_struct _T47;_T47.tag=1;_T47.f1=Cyc_strlen(an);_T2C=_T47;}{struct Cyc_Int_pa_PrintArg_struct _T47=_T2C;{struct Cyc_String_pa_PrintArg_struct _T48;_T48.tag=0;_T48.f1=an;_T2D=_T48;}{struct Cyc_String_pa_PrintArg_struct _T48=_T2D;{struct Cyc_Int_pa_PrintArg_struct _T49;_T49.tag=1;_T2F=count;_T30=*_T2F;*_T2F=_T30 + 1;_T31=_T30;_T49.f1=(unsigned long)_T31;_T2E=_T49;}{struct Cyc_Int_pa_PrintArg_struct _T49=_T2E;void*_T4A[5];_T4A[0]=& _T45;_T4A[1]=& _T46;_T4A[2]=& _T47;_T4A[3]=& _T48;_T4A[4]=& _T49;_T32=_tag_fat(_T4A,sizeof(void*),5);_T29=Cyc_Interproc_make_parg(_T32);}}}}}Cyc_vsprintf(_T27,_T28,_T29);_T33=vd;_T34=_T33->type;_T35=Cyc_Interproc__prefix;_T36=
_tag_fat(_T35,sizeof(char),1024U);_T37=Cyc_strlen(_T36);_T38=(int)_T37;{struct Cyc_List_List*cvars=Cyc_Interproc_cvars_in_type(_T34,0,1,_T38);
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
static struct Cyc_List_List*Cyc_Interproc_assign_cvar_names(struct Cyc_List_List*ds){struct Cyc_List_List*_T0;unsigned _T1;struct Cyc_List_List*_T2;void*_T3;struct Cyc_Absyn_Decl*_T4;int*_T5;unsigned _T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_Absyn_Vardecl*_T9;struct _tuple0*_TA;char*_TB;unsigned _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Int_pa_PrintArg_struct _T10;struct Cyc_String_pa_PrintArg_struct _T11;struct _fat_ptr _T12;struct Cyc_Absyn_Vardecl*_T13;void*_T14;char*_T15;struct _fat_ptr _T16;unsigned long _T17;int _T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;struct Cyc_Absyn_Fndecl*_T1B;struct _tuple0*_T1C;char*_T1D;unsigned _T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_Int_pa_PrintArg_struct _T22;struct Cyc_String_pa_PrintArg_struct _T23;struct _fat_ptr _T24;struct Cyc_Absyn_Fndecl*_T25;struct _tuple0*_T26;int _T27;struct Cyc_Absyn_Fndecl*_T28;struct Cyc_Absyn_FnInfo _T29;struct Cyc_List_List*_T2A;unsigned _T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;unsigned _T2E;struct Cyc_List_List*_T2F;struct Cyc_List_List*_T30;void*_T31;struct _tuple8*_T32;void*_T33;void*_T34;struct Cyc_Absyn_FnType_Absyn_Type_struct*_T35;struct Cyc_Absyn_Fndecl*_T36;void*_T37;char*_T38;struct _fat_ptr _T39;unsigned long _T3A;int _T3B;struct Cyc_List_List*_T3C;struct Cyc_List_List*_T3D;struct _tuple18*_T3E;struct _tuple18*_T3F;struct _tuple18*_T40;struct _tuple18*_T41;void(*_T42)(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*),struct _tuple18*,struct Cyc_Absyn_Stmt*);void(*_T43)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);int(*_T44)(struct _tuple18*,struct Cyc_Absyn_Exp*);int(*_T45)(struct _tuple18*,struct Cyc_Absyn_Stmt*);struct _tuple18*_T46;struct Cyc_Absyn_Fndecl*_T47;struct Cyc_Absyn_Stmt*_T48;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T49;struct Cyc_Absyn_Aggrdecl**_T4A;void*_T4B;struct _fat_ptr _T4C;void*_T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T50;struct Cyc_Absyn_Datatypedecl**_T51;void*_T52;struct _fat_ptr _T53;void*_T54;struct Cyc_List_List*_T55;struct Cyc_List_List*_T56;struct Cyc_Absyn_Typedefdecl*_T57;struct _tuple0*_T58;struct Cyc_Absyn_Typedefdecl*_T59;void*_T5A;struct Cyc_List_List*_T5B;struct Cyc_List_List*_T5C;struct Cyc_List_List*_T5D;struct Cyc_List_List*_T5E;
struct Cyc_List_List*ret=0;
_TL12F: _T0=ds;_T1=(unsigned)_T0;if(_T1)goto _TL12D;else{goto _TL12E;}
_TL12D: _T2=ds;_T3=_T2->hd;_T4=(struct Cyc_Absyn_Decl*)_T3;{void*_T5F=_T4->r;struct Cyc_Absyn_Typedefdecl*_T60;struct Cyc_Absyn_Datatypedecl*_T61;struct Cyc_Absyn_Aggrdecl*_T62;struct Cyc_Absyn_Fndecl*_T63;struct Cyc_Absyn_Vardecl*_T64;struct Cyc_List_List*_T65;_T5=(int*)_T5F;_T6=*_T5;switch(_T6){case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T66=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T5F;_T65=_T66->f2;}{struct Cyc_List_List*l=_T65;_T65=l;goto _LL4;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T66=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T5F;_T65=_T66->f2;}_LL4: {struct Cyc_List_List*l=_T65;_T7=ret;_T8=
# 726
Cyc_Interproc_assign_cvar_names(l);ret=Cyc_List_append(_T7,_T8);goto _LL0;}case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T66=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T5F;_T64=_T66->f1;}{struct Cyc_Absyn_Vardecl*vd=_T64;_T9=vd;_TA=_T9->name;{
# 728
struct _fat_ptr tn=Cyc_Absynpp_fullqvar2string(_TA);_TB=Cyc_Interproc__prefix;_TC=1024U - 1U;_TD=
_tag_fat(_TB,sizeof(char),_TC);_TE=_tag_fat("%dI%s",sizeof(char),6U);{struct Cyc_Int_pa_PrintArg_struct _T66;_T66.tag=1;_T66.f1=Cyc_strlen(tn);_T10=_T66;}{struct Cyc_Int_pa_PrintArg_struct _T66=_T10;{struct Cyc_String_pa_PrintArg_struct _T67;_T67.tag=0;_T67.f1=tn;_T11=_T67;}{struct Cyc_String_pa_PrintArg_struct _T67=_T11;void*_T68[2];_T68[0]=& _T66;_T68[1]=& _T67;_T12=_tag_fat(_T68,sizeof(void*),2);_TF=Cyc_Interproc_make_parg(_T12);}}Cyc_vsprintf(_TD,_TE,_TF);_T13=vd;_T14=_T13->type;_T15=Cyc_Interproc__prefix;_T16=
_tag_fat(_T15,sizeof(char),1024U);_T17=Cyc_strlen(_T16);_T18=(int)_T17;_T19=Cyc_Interproc_cvars_in_type(_T14,0,0,_T18);_T1A=ret;ret=Cyc_List_append(_T19,_T1A);goto _LL0;}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T66=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T5F;_T63=_T66->f1;}{struct Cyc_Absyn_Fndecl*fd=_T63;_T1B=fd;_T1C=_T1B->name;{
# 734
struct _fat_ptr fdname=Cyc_Absynpp_fullqvar2string(_T1C);_T1D=Cyc_Interproc__prefix;_T1E=1024U - 1U;_T1F=
_tag_fat(_T1D,sizeof(char),_T1E);_T20=_tag_fat("%dI%s",sizeof(char),6U);{struct Cyc_Int_pa_PrintArg_struct _T66;_T66.tag=1;_T66.f1=Cyc_strlen(fdname);_T22=_T66;}{struct Cyc_Int_pa_PrintArg_struct _T66=_T22;{struct Cyc_String_pa_PrintArg_struct _T67;_T67.tag=0;_T67.f1=fdname;_T23=_T67;}{struct Cyc_String_pa_PrintArg_struct _T67=_T23;void*_T68[2];_T68[0]=& _T66;_T68[1]=& _T67;_T24=_tag_fat(_T68,sizeof(void*),2);_T21=Cyc_Interproc_make_parg(_T24);}}Cyc_vsprintf(_T1F,_T20,_T21);_T25=fd;_T26=_T25->name;_T27=
Cyc_Tcutil_is_main(_T26);if(!_T27)goto _TL131;_T28=fd;_T29=_T28->i;{
struct Cyc_List_List*args=_T29.args;_T2A=args;_T2B=(unsigned)_T2A;
if(!_T2B)goto _TL133;_T2C=args;_T2D=_T2C->tl;_T2E=(unsigned)_T2D;if(!_T2E)goto _TL133;_T2F=args;_T30=_T2F->tl;_T31=_T30->hd;_T32=(struct _tuple8*)_T31;_T33=_T32->f2;_T34=Cyc_Absyn_fat_bound_type;
Cyc_Tcutil_replace_cvars(_T33,_T34);goto _TL134;_TL133: _TL134:;}goto _TL132;
# 743
_TL131:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T66=_cycalloc(sizeof(struct Cyc_Absyn_FnType_Absyn_Type_struct));_T66->tag=6;_T36=fd;_T66->f1=_T36->i;_T35=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T66;}_T37=(void*)_T35;_T38=Cyc_Interproc__prefix;_T39=_tag_fat(_T38,sizeof(char),1024U);_T3A=Cyc_strlen(_T39);_T3B=(int)_T3A;_T3C=Cyc_Interproc_cvars_in_type(_T37,0,0,_T3B);_T3D=ret;ret=Cyc_List_append(_T3C,_T3D);_TL132: {
# 747
struct _tuple18*env;env=_cycalloc(sizeof(struct _tuple18));_T3E=env;_T3E->f0=fdname;_T3F=env;_T3F->f1=0;_T40=env;_T40->f2=Cyc_Interproc_remove_local_constraints;_T41=env;_T41->f3=0;_T43=Cyc_Absyn_visit_stmt;{
void(*_T66)(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*),struct _tuple18*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*),int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*),struct _tuple18*,struct Cyc_Absyn_Stmt*))_T43;_T42=_T66;}{int(*_T66)(struct _tuple18*,struct Cyc_Absyn_Exp*)=(int(*)(struct _tuple18*,struct Cyc_Absyn_Exp*))Cyc_Interproc_exp_noop;_T44=_T66;}{int(*_T66)(struct _tuple18*,struct Cyc_Absyn_Stmt*)=(int(*)(struct _tuple18*,struct Cyc_Absyn_Stmt*))Cyc_Interproc_cvars_in_local_decl;_T45=_T66;}_T46=env;_T47=fd;_T48=_T47->body;_T42(_T44,_T45,_T46,_T48);goto _LL0;}}}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T66=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T5F;_T62=_T66->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T62;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T66=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T66->tag=24;{struct Cyc_Absyn_Aggrdecl**_T67=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));
# 751
*_T67=ad;_T4A=(struct Cyc_Absyn_Aggrdecl**)_T67;}_T66->f1=Cyc_Absyn_KnownAggr(_T4A);_T49=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T66;}_T4B=(void*)_T49;_T4C=_tag_fat(0U,sizeof(void*),0);_T4D=Cyc_Absyn_app_type(_T4B,_T4C);_T4E=Cyc_Interproc_cvars_in_type(_T4D,0,0,0);_T4F=ret;ret=Cyc_List_append(_T4E,_T4F);goto _LL0;}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T66=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T5F;_T61=_T66->f1;}{struct Cyc_Absyn_Datatypedecl*dd=_T61;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T66=_cycalloc(sizeof(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct));_T66->tag=22;{struct Cyc_Absyn_Datatypedecl**_T67=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));
# 755
*_T67=dd;_T51=(struct Cyc_Absyn_Datatypedecl**)_T67;}_T66->f1=Cyc_Absyn_KnownDatatype(_T51);_T50=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T66;}_T52=(void*)_T50;_T53=_tag_fat(0U,sizeof(void*),0);_T54=Cyc_Absyn_app_type(_T52,_T53);_T55=Cyc_Interproc_cvars_in_type(_T54,0,0,0);_T56=ret;ret=Cyc_List_append(_T55,_T56);goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T66=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T5F;_T60=_T66->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T60;_T57=td;_T58=_T57->name;_T59=td;_T5A=
# 759
Cyc_Absyn_typedef_type(_T58,0,_T59,0);_T5B=Cyc_Interproc_cvars_in_type(_T5A,0,0,0);_T5C=ret;ret=Cyc_List_append(_T5B,_T5C);goto _LL0;}default: goto _LL0;}_LL0:;}_T5D=ds;
# 723
ds=_T5D->tl;goto _TL12F;_TL12E: _T5E=ret;
# 766
return _T5E;}
# 769
static struct Cyc_List_List*Cyc_Interproc_parse_constraint_graph (void){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc___cycFILE*_T7;unsigned _T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct Cyc_List_List*_TB;{struct Cyc_String_pa_PrintArg_struct _TC;_TC.tag=0;
_TC.f1=Cyc_Interproc_get_inference_prefix();_T1=_TC;}{struct Cyc_String_pa_PrintArg_struct _TC=_T1;void*_TD[1];_TD[0]=& _TC;_T2=_tag_fat("%s/.cych/cvars",sizeof(char),15U);_T3=_tag_fat(_TD,sizeof(void*),1);_T0=Cyc_aprintf(_T2,_T3);}{struct _fat_ptr cfilename=_T0;_T4=cfilename;_T5=
_tag_fat("r",sizeof(char),2U);_T6=_tag_fat("constraint file",sizeof(char),16U);{struct Cyc___cycFILE*file=Cyc_Interproc_try_file_open(_T4,_T5,_T6);_T7=file;_T8=(unsigned)_T7;
if(_T8)goto _TL135;else{goto _TL137;}
_TL137: _T9=_tag_fat("Unable to initialize constraint graph",sizeof(char),38U);_TA=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_T9,_TA);
return 0;_TL135:
# 776
 Cyc_Warn_reset(cfilename);{
struct Cyc_List_List*ret=Cyc_Parse_parse_constraint_file(file);
Cyc_fclose(file);_TB=ret;
return _TB;}}}}
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
void Cyc_Interproc_emit_decls(struct _fat_ptr filename,struct Cyc_List_List*ds){struct _fat_ptr _T0;struct Cyc_String_pa_PrintArg_struct _T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc___cycFILE*_T7;unsigned _T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct Cyc_String_pa_PrintArg_struct _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct Cyc___cycFILE*_T13;unsigned _T14;struct Cyc_Absynpp_Params*_T15;struct Cyc_Absynpp_Params*_T16;{struct Cyc_String_pa_PrintArg_struct _T17;_T17.tag=0;
# 822
_T17.f1=Cyc_Interproc_get_inference_prefix();_T1=_T17;}{struct Cyc_String_pa_PrintArg_struct _T17=_T1;void*_T18[1];_T18[0]=& _T17;_T2=_tag_fat("%s/.cych/cvars",sizeof(char),15U);_T3=_tag_fat(_T18,sizeof(void*),1);_T0=Cyc_aprintf(_T2,_T3);}{struct _fat_ptr cvar_filename=_T0;_T4=cvar_filename;_T5=
_tag_fat("w",sizeof(char),2U);_T6=_tag_fat("constraint file",sizeof(char),16U);{struct Cyc___cycFILE*file=Cyc_Interproc_try_file_open(_T4,_T5,_T6);_T7=file;_T8=(unsigned)_T7;
if(_T8)goto _TL14C;else{goto _TL14E;}
_TL14E: _T9=_tag_fat("Unable to open constrant file",sizeof(char),30U);_TA=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(0U,_T9,_TA);
return;_TL14C:
# 828
 Cyc_BansheeIf_persist_cvarmap(file);
Cyc_fclose(file);{struct Cyc_String_pa_PrintArg_struct _T17;_T17.tag=0;
# 831
_T17.f1=Cyc_Interproc_get_inference_prefix();_TC=_T17;}{struct Cyc_String_pa_PrintArg_struct _T17=_TC;{struct Cyc_String_pa_PrintArg_struct _T18;_T18.tag=0;_T18.f1=Cyc_Filename_basename(filename);_TD=_T18;}{struct Cyc_String_pa_PrintArg_struct _T18=_TD;void*_T19[2];_T19[0]=& _T17;_T19[1]=& _T18;_TE=_tag_fat("%s/.cych/%s",sizeof(char),12U);_TF=_tag_fat(_T19,sizeof(void*),2);_TB=Cyc_aprintf(_TE,_TF);}}{struct _fat_ptr outname=_TB;_T10=outname;_T11=
_tag_fat("w",sizeof(char),2U);_T12=_tag_fat("output file",sizeof(char),12U);{struct Cyc___cycFILE*outfile=Cyc_Interproc_try_file_open(_T10,_T11,_T12);_T13=outfile;_T14=(unsigned)_T13;
if(_T14)goto _TL14F;else{goto _TL151;}
_TL151: return;_TL14F: _T15=& Cyc_Absynpp_cycinf_params_r;_T16=(struct Cyc_Absynpp_Params*)_T15;
Cyc_Absynpp_set_params(_T16);
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
struct Cyc_List_List*Cyc_Interproc_resolve_all(struct _fat_ptr filename,struct Cyc_List_List*tds){struct Cyc_List_List*_T0;struct _tuple11*_T1;struct Cyc_List_List*_T2;unsigned _T3;struct Cyc_List_List*_T4;void*_T5;struct _tuple11*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;unsigned _T9;struct _handler_cons*_TA;int _TB;int _TC;struct Cyc_List_List*_TD;unsigned _TE;void*_TF;struct Cyc_Interproc_Unresolved_sym_exn_struct*_T10;char*_T11;char*_T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;struct Cyc_Warn_Exp_Warn_Warg_struct _T14;struct Cyc_Absyn_Exp*_T15;unsigned _T16;struct _fat_ptr _T17;struct Cyc_List_List*_T18;unsigned _T19;struct Cyc_List_List*_T1A;void*_T1B;struct _tuple11*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;unsigned _T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;void*_T22;struct _tuple11*_T23;struct Cyc_List_List*_T24;struct Cyc_List_List*_T25;struct Cyc_List_List*_T26;
struct Cyc_List_List*orig=tds;
struct Cyc_List_List*first=0;struct Cyc_List_List*worklist=0;{struct Cyc_List_List*_T27=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple11*_T28=_cycalloc(sizeof(struct _tuple11));
_T28->f0=filename;_T28->f1=tds;_T1=(struct _tuple11*)_T28;}_T27->hd=_T1;_T27->tl=0;_T0=(struct Cyc_List_List*)_T27;}first=_T0;{
struct Cyc_List_List*all_located_fnsyms=0;
# 864
_TL157: _T2=worklist;_T3=(unsigned)_T2;if(!_T3)goto _TL159;_T4=worklist;_T5=_T4->hd;_T6=(struct _tuple11*)_T5;
tds=_T6->f1;_T7=worklist;
worklist=_T7->tl;goto _TL15A;_TL159: _TL15A:{
# 868
struct Cyc_List_List*externs=Cyc_Binding_resolve_all_interproc(tds);_T8=externs;_T9=(unsigned)_T8;
if(!_T9)goto _TL15B;{struct _handler_cons _T27;_TA=& _T27;_push_handler(_TA);{int _T28=0;_TB=setjmp(_T27.handler);if(!_TB)goto _TL15D;_T28=1;goto _TL15E;_TL15D: _TL15E: if(_T28)goto _TL15F;else{goto _TL161;}_TL161:{
# 871
struct _tuple14 _T29=Cyc_Interproc_locate_externs(externs,first);struct Cyc_List_List*_T2A;struct Cyc_List_List*_T2B;int _T2C;_T2C=_T29.f0;_T2B=_T29.f1;_T2A=_T29.f2;{int b=_T2C;struct Cyc_List_List*nd=_T2B;struct Cyc_List_List*rext=_T2A;_TC=b;
if(_TC)goto _TL162;else{goto _TL164;}
_TL164:{struct Cyc_List_List*_T2D=tds;_npop_handler(0);return _T2D;}_TL162: _TD=worklist;_TE=(unsigned)_TD;
if(_TE)goto _TL165;else{goto _TL167;}
_TL167: worklist=nd;goto _TL166;_TL165: _TL166:
# 877
 all_located_fnsyms=Cyc_List_imp_append(rext,all_located_fnsyms);}}_pop_handler();goto _TL160;_TL15F: _TF=Cyc_Core_get_exn_thrown();{void*_T29=(void*)_TF;void*_T2A;_T10=(struct Cyc_Interproc_Unresolved_sym_exn_struct*)_T29;_T11=_T10->tag;_T12=Cyc_Interproc_Unresolved_sym;if(_T11!=_T12)goto _TL168;{struct Cyc_Interproc_Unresolved_sym_exn_struct*_T2B=(struct Cyc_Interproc_Unresolved_sym_exn_struct*)_T29;_T2A=_T2B->f1;}{struct Cyc_Absyn_Exp*e=_T2A;{struct Cyc_Warn_String_Warn_Warg_struct _T2B;_T2B.tag=0;
# 880
_T2B.f1=_tag_fat("Undefined symbol",sizeof(char),17U);_T13=_T2B;}{struct Cyc_Warn_String_Warn_Warg_struct _T2B=_T13;{struct Cyc_Warn_Exp_Warn_Warg_struct _T2C;_T2C.tag=4;_T2C.f1=e;_T14=_T2C;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T2C=_T14;void*_T2D[2];_T2D[0]=& _T2B;_T2D[1]=& _T2C;_T15=e;_T16=_T15->loc;_T17=_tag_fat(_T2D,sizeof(void*),2);Cyc_Warn_err2(_T16,_T17);}}
return 0;}_TL168: _T2A=_T29;{void*exn=_T2A;_rethrow(exn);};}_TL160:;}}goto _TL15C;_TL15B: _TL15C:;}_T18=worklist;_T19=(unsigned)_T18;
# 863
if(_T19)goto _TL157;else{goto _TL158;}_TL158: _T1A=first;_T1B=_T1A->hd;_T1C=(struct _tuple11*)_T1B;{
# 885
struct Cyc_List_List*prev=_T1C->f1;
struct Cyc_List_List*all_decls=orig;_T1D=first;{
struct Cyc_List_List*it=_T1D->tl;_TL16D: _T1E=it;_T1F=(unsigned)_T1E;if(_T1F)goto _TL16B;else{goto _TL16C;}
_TL16B: _T20=all_decls;_T21=it;_T22=_T21->hd;_T23=(struct _tuple11*)_T22;_T24=_T23->f1;all_decls=Cyc_List_imp_append(_T20,_T24);_T25=it;
# 887
it=_T25->tl;goto _TL16D;_TL16C:;}
# 893
Cyc_Interproc_bind_cvars(all_decls);_T26=all_decls;
return _T26;}}}struct _tuple20{void*f0;struct Cyc_List_List*f1;};struct _tuple21{const char*f0;const char*f1;int f2;int f3;};
# 897
void Cyc_Interproc_inference_statistics (void){struct Cyc___cycFILE*_T0;struct _fat_ptr _T1;struct _fat_ptr _T2;int _T3;struct Cyc___cycFILE*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_List_List*_T7;unsigned _T8;struct Cyc_List_List*_T9;void*_TA;void*_TB;int*_TC;int _TD;void*_TE;int*_TF;unsigned _T10;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11;void*_T12;int*_T13;unsigned _T14;struct Cyc_List_List*_T15;struct _tuple21*_T16;struct Cyc_List_List*_T17;struct _tuple21*_T18;struct Cyc_List_List*_T19;struct Cyc_Int_pa_PrintArg_struct _T1A;int _T1B;struct Cyc___cycFILE*_T1C;struct _fat_ptr _T1D;struct _fat_ptr _T1E;struct Cyc_List_List*(*_T1F)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*);struct Cyc_List_List*(*_T20)(int(*)(void*,void*),struct Cyc_List_List*);int(*_T21)(struct _tuple21*,struct _tuple21*);struct Cyc_List_List*_T22;struct Cyc_String_pa_PrintArg_struct _T23;struct Cyc_String_pa_PrintArg_struct _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct Cyc_String_pa_PrintArg_struct _T26;struct Cyc___cycFILE*_T27;struct _fat_ptr _T28;struct _fat_ptr _T29;struct Cyc___cycFILE*_T2A;struct _fat_ptr _T2B;struct _fat_ptr _T2C;struct Cyc_List_List*_T2D;unsigned _T2E;const char*_T2F;struct Cyc_List_List*_T30;void*_T31;struct _tuple21*_T32;int _T33;const char*_T34;struct Cyc_List_List*_T35;void*_T36;struct _tuple21*_T37;int _T38;struct Cyc_String_pa_PrintArg_struct _T39;struct _fat_ptr _T3A;struct Cyc_List_List*_T3B;void*_T3C;struct _tuple21*_T3D;void*_T3E;void*_T3F;unsigned _T40;struct Cyc_String_pa_PrintArg_struct _T41;struct _fat_ptr _T42;struct Cyc_List_List*_T43;void*_T44;struct _tuple21*_T45;void*_T46;void*_T47;unsigned _T48;struct Cyc_String_pa_PrintArg_struct _T49;struct _fat_ptr _T4A;void*_T4B;void*_T4C;unsigned _T4D;struct Cyc_String_pa_PrintArg_struct _T4E;struct _fat_ptr _T4F;void*_T50;void*_T51;unsigned _T52;struct Cyc___cycFILE*_T53;struct _fat_ptr _T54;struct _fat_ptr _T55;struct Cyc_List_List*_T56;struct Cyc___cycFILE*_T57;struct _fat_ptr _T58;struct _fat_ptr _T59;struct Cyc___cycFILE*_T5A;struct _fat_ptr _T5B;struct _fat_ptr _T5C;_T0=Cyc_stderr;_T1=
_tag_fat("Looking for constraint variables...\n",sizeof(char),37U);_T2=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T0,_T1,_T2);{
struct Cyc_List_List*graph=Cyc_Interproc_parse_constraint_graph();_T3=
Cyc_BansheeIf_add_all_constraints(graph);if(!_T3)goto _TL16E;_T4=Cyc_stderr;_T5=
_tag_fat("Inconsistent constraint graph\n",sizeof(char),31U);_T6=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T4,_T5,_T6);
return;_TL16E: {
# 906
struct Cyc_List_List*ips=0;
int nips=0;
_TL173: _T7=graph;_T8=(unsigned)_T7;if(_T8)goto _TL171;else{goto _TL172;}
_TL171: _T9=graph;_TA=_T9->hd;{struct _tuple20*_T5D=(struct _tuple20*)_TA;void*_T5E;{struct _tuple20 _T5F=*_T5D;_T5E=_T5F.f0;}{void*t=_T5E;int _T5F;const char*_T60;const char*_T61;_TB=t;_TC=(int*)_TB;_TD=*_TC;if(_TD!=3)goto _TL174;_TE=t;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T62=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_TE;_T61=_T62->f5;_T60=_T62->f6;_T5F=_T62->f7;}{const char*name=_T61;const char*loc=_T60;int origfat=_T5F;{
# 912
void*_T62=Cyc_Absyn_compress(t);_TF=(int*)_T62;_T10=*_TF;switch(_T10){case 3: goto _LLC;case 0: _T11=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T62;_T12=_T11->f1;_T13=(int*)_T12;_T14=*_T13;switch(_T14){case 13: _LLC:{struct Cyc_List_List*_T63=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple21*_T64=_cycalloc(sizeof(struct _tuple21));
# 915
_T64->f0=name;_T64->f1=loc;_T64->f2=0;_T64->f3=origfat;_T16=(struct _tuple21*)_T64;}_T63->hd=_T16;_T63->tl=ips;_T15=(struct Cyc_List_List*)_T63;}ips=_T15;
nips=nips + 1;goto _LL8;case 14:{struct Cyc_List_List*_T63=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple21*_T64=_cycalloc(sizeof(struct _tuple21));
# 919
_T64->f0=name;_T64->f1=loc;_T64->f2=1;_T64->f3=origfat;_T18=(struct _tuple21*)_T64;}_T63->hd=_T18;_T63->tl=ips;_T17=(struct Cyc_List_List*)_T63;}ips=_T17;
nips=nips + 1;goto _LL8;default: goto _LLF;};default: _LLF: goto _LL8;}_LL8:;}goto _LL3;}_TL174: goto _LL3;_LL3:;}}_T19=graph;
# 908
graph=_T19->tl;goto _TL173;_TL172:{struct Cyc_Int_pa_PrintArg_struct _T5D;_T5D.tag=1;_T1B=nips;
# 930
_T5D.f1=(unsigned long)_T1B;_T1A=_T5D;}{struct Cyc_Int_pa_PrintArg_struct _T5D=_T1A;void*_T5E[1];_T5E[0]=& _T5D;_T1C=Cyc_stderr;_T1D=_tag_fat("Found %d of them.\n",sizeof(char),19U);_T1E=_tag_fat(_T5E,sizeof(void*),1);Cyc_fprintf(_T1C,_T1D,_T1E);}_T20=Cyc_List_merge_sort;{
# 933
struct Cyc_List_List*(*_T5D)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple21*,struct _tuple21*),struct Cyc_List_List*))_T20;_T1F=_T5D;}_T21=(int(*)(struct _tuple21*,struct _tuple21*))Cyc_Interproc_cmp_stats;_T22=ips;ips=_T1F(_T21,_T22);{struct Cyc_String_pa_PrintArg_struct _T5D;_T5D.tag=0;
_T5D.f1=_tag_fat("Constraint variable",sizeof(char),20U);_T23=_T5D;}{struct Cyc_String_pa_PrintArg_struct _T5D=_T23;{struct Cyc_String_pa_PrintArg_struct _T5E;_T5E.tag=0;_T5E.f1=_tag_fat("Location, file:line",sizeof(char),20U);_T24=_T5E;}{struct Cyc_String_pa_PrintArg_struct _T5E=_T24;{struct Cyc_String_pa_PrintArg_struct _T5F;_T5F.tag=0;_T5F.f1=_tag_fat("Orig",sizeof(char),5U);_T25=_T5F;}{struct Cyc_String_pa_PrintArg_struct _T5F=_T25;{struct Cyc_String_pa_PrintArg_struct _T60;_T60.tag=0;_T60.f1=_tag_fat("Inf",sizeof(char),4U);_T26=_T60;}{struct Cyc_String_pa_PrintArg_struct _T60=_T26;void*_T61[4];_T61[0]=& _T5D;_T61[1]=& _T5E;_T61[2]=& _T5F;_T61[3]=& _T60;_T27=Cyc_stderr;_T28=_tag_fat("%-49s %-30s %-4s %-4s\n",sizeof(char),23U);_T29=_tag_fat(_T61,sizeof(void*),4);Cyc_fprintf(_T27,_T28,_T29);}}}}_T2A=Cyc_stderr;_T2B=
_tag_fat("------------------------------------------------------------------------------------------\n",sizeof(char),92U);_T2C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2A,_T2B,_T2C);
_TL17B: _T2D=ips;_T2E=(unsigned)_T2D;if(_T2E)goto _TL179;else{goto _TL17A;}
_TL179: _T30=ips;_T31=_T30->hd;_T32=(struct _tuple21*)_T31;_T33=_T32->f3;if(!_T33)goto _TL17C;_T2F="fat";goto _TL17D;_TL17C: _T2F="thin";_TL17D:{const char*origtype=_T2F;_T35=ips;_T36=_T35->hd;_T37=(struct _tuple21*)_T36;_T38=_T37->f2;
if(!_T38)goto _TL17E;_T34="fat";goto _TL17F;_TL17E: _T34="thin";_TL17F: {const char*inftype=_T34;{struct Cyc_String_pa_PrintArg_struct _T5D;_T5D.tag=0;_T3B=ips;_T3C=_T3B->hd;_T3D=(struct _tuple21*)_T3C;{const char*_T5E=_T3D->f0;_T3E=(void*)_T5E;_T3F=(void*)_T5E;_T40=_get_zero_arr_size_char(_T3F,1U);_T3A=_tag_fat(_T3E,sizeof(char),_T40);}
_T5D.f1=_T3A;_T39=_T5D;}{struct Cyc_String_pa_PrintArg_struct _T5D=_T39;{struct Cyc_String_pa_PrintArg_struct _T5E;_T5E.tag=0;_T43=ips;_T44=_T43->hd;_T45=(struct _tuple21*)_T44;{const char*_T5F=_T45->f1;_T46=(void*)_T5F;_T47=(void*)_T5F;_T48=_get_zero_arr_size_char(_T47,1U);_T42=_tag_fat(_T46,sizeof(char),_T48);}_T5E.f1=_T42;_T41=_T5E;}{struct Cyc_String_pa_PrintArg_struct _T5E=_T41;{struct Cyc_String_pa_PrintArg_struct _T5F;_T5F.tag=0;{const char*_T60=origtype;_T4B=(void*)_T60;_T4C=(void*)_T60;_T4D=_get_zero_arr_size_char(_T4C,1U);_T4A=_tag_fat(_T4B,sizeof(char),_T4D);}_T5F.f1=_T4A;_T49=_T5F;}{struct Cyc_String_pa_PrintArg_struct _T5F=_T49;{struct Cyc_String_pa_PrintArg_struct _T60;_T60.tag=0;{const char*_T61=inftype;_T50=(void*)_T61;_T51=(void*)_T61;_T52=_get_zero_arr_size_char(_T51,1U);_T4F=_tag_fat(_T50,sizeof(char),_T52);}_T60.f1=_T4F;_T4E=_T60;}{struct Cyc_String_pa_PrintArg_struct _T60=_T4E;void*_T61[4];_T61[0]=& _T5D;_T61[1]=& _T5E;_T61[2]=& _T5F;_T61[3]=& _T60;_T53=Cyc_stderr;_T54=_tag_fat("C_PTRBND_%-40s %-30s %-4s %-4s\n",sizeof(char),32U);_T55=_tag_fat(_T61,sizeof(void*),4);Cyc_fprintf(_T53,_T54,_T55);}}}}}}_T56=ips;
# 936
ips=_T56->tl;goto _TL17B;_TL17A: _T57=Cyc_stderr;_T58=
# 956 "interproc.cyc"
_tag_fat("------------------------------------------------------------------------------------------\n",sizeof(char),92U);_T59=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T57,_T58,_T59);_T5A=Cyc_stderr;_T5B=
_tag_fat("That's all folks.\n",sizeof(char),19U);_T5C=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T5A,_T5B,_T5C);}}}

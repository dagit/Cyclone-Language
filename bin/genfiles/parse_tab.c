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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc___cycFILE;
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
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
extern int Cyc_zstrcmp(struct _fat_ptr,struct _fat_ptr);
# 54
extern int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 60
extern struct _fat_ptr Cyc_strcat(struct _fat_ptr,struct _fat_ptr);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 71
extern struct _fat_ptr Cyc_strcpy(struct _fat_ptr,struct _fat_ptr);
# 121 "string.h"
extern struct _fat_ptr Cyc_strchr(struct _fat_ptr,char);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*);
# 33 "position.h"
extern unsigned Cyc_Position_loc_to_seg(unsigned);
# 37
extern struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 312
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f0;struct Cyc_Absyn_Datatypefield*f1;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 325
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 332
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_SubsetType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_AssnDef_ExistAssnFn*f3;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;struct Cyc_Absyn_Exp*f9;struct Cyc_Absyn_Exp*f10;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 533
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 551
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Subset_coercion =3U,Cyc_Absyn_Other_coercion =4U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 566
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 734 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 902
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 928
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 936
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 942
void*Cyc_Absyn_compress(void*);
# 955
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 957
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 960
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 962
extern void*Cyc_Absyn_sint_type;
# 964
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 967
extern void*Cyc_Absyn_complex_type(void*);
# 969
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_shorthand_type;extern void*Cyc_Absyn_refcnt_rgn_shorthand_type;
# 971
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 975
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 977
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_tuple_type(struct Cyc_List_List*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 996
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 1008
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 1012
void*Cyc_Absyn_bounds_one (void);
void*Cyc_Absyn_cvar_type(struct Cyc_Core_Opt*);
void*Cyc_Absyn_cvar_type_name(struct Cyc_Core_Opt*,struct _fat_ptr);
# 1018
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1038
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1041
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1045
void*Cyc_Absyn_aqualsof_type(void*);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1076
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tagof_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1087
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1097
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1099
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1106
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1108
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
# 1122
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1126
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_false_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1140
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1145
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1156
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1159
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1162
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1166
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1171
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1173
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1182
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1189
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1192
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1197
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 37 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 43
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 30 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 39
extern int Cyc_Flags_override_fat;
extern int Cyc_Flags_interproc;
extern int Cyc_Flags_resolve;
# 46
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
# 59
extern struct Cyc_Core_Opt Cyc_Kinds_ptrbko;
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
# 92
void*Cyc_Tcutil_copy_type(void*);
# 215 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 246
void*Cyc_Tcutil_promote_array(void*,void*,void*,int);
# 256
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 278
void*Cyc_Tcutil_ptrbnd_cvar_equivalent(void*);
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
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 17 "bansheeif.h"
void*Cyc_BansheeIf_equality_constraint(void*,void*);
void*Cyc_BansheeIf_cond_equality_constraint(void*,void*);
void*Cyc_BansheeIf_inclusion_constraint(void*,void*);
void*Cyc_BansheeIf_implication_constraint(void*,void*);
void*Cyc_BansheeIf_and_constraint(void*,void*);
void*Cyc_BansheeIf_or_constraint(void*,void*);
void*Cyc_BansheeIf_not_constraint(void*);
void*Cyc_BansheeIf_cmpeq_constraint(void*,void*);
void*Cyc_BansheeIf_check_constraint(void*);
# 27
void*Cyc_BansheeIf_add_location(struct _fat_ptr,void*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Complex_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Parse_Declarator f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct _tuple13{struct _tuple13*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
# 57 "parse.h"
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U,Cyc_Parse_None_sc =7U};
# 62
enum Cyc_Parse_ConstraintOps{Cyc_Parse_C_AND_OP =0U,Cyc_Parse_C_OR_OP =1U,Cyc_Parse_C_NOT_OP =2U,Cyc_Parse_C_EQ_OP =3U,Cyc_Parse_C_INCL_OP =4U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 80 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*);
void Cyc_Lex_enter_namespace(struct _fat_ptr*);
void Cyc_Lex_leave_namespace (void);
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using (void);
void Cyc_Lex_enter_extern_c (void);
void Cyc_Lex_leave_extern_c (void);
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _fat_ptr Cyc_Lex_token_string;
# 104 "parse.y"
int Cyc_Parse_parsing_tempest=0;
# 110
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){struct Cyc_Parse_FlatList*_T0;struct Cyc_Parse_FlatList*_T1;struct Cyc_Parse_FlatList*_T2;struct Cyc_Parse_FlatList*_T3;struct Cyc_Parse_FlatList*_T4;struct Cyc_Parse_FlatList*_T5;
if(x!=0)goto _TL0;_T0=x;return _T0;_TL0: {
struct Cyc_Parse_FlatList*first=x;_T1=x;{
struct Cyc_Parse_FlatList*second=_T1->tl;_T2=x;
_T2->tl=0;
_TL2: if(second!=0)goto _TL3;else{goto _TL4;}
_TL3: _T3=second;{struct Cyc_Parse_FlatList*temp=_T3->tl;_T4=second;
_T4->tl=first;
first=second;
second=temp;}goto _TL2;_TL4: _T5=first;
# 121
return _T5;}}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};
# 177
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 182
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
static struct Cyc_List_List*Cyc_Parse_constraint_graph=0;
static int Cyc_Parse_inside_function_definition=0;
static int Cyc_Parse_inside_noinference_block=0;
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){struct Cyc_Warn_String_Warn_Warg_struct _T0;unsigned _T1;struct _fat_ptr _T2;struct Cyc_Parse_Exit_exn_struct*_T3;void*_T4;{struct Cyc_Warn_String_Warn_Warg_struct _T5;_T5.tag=0;
_T5.f1=msg;_T0=_T5;}{struct Cyc_Warn_String_Warn_Warg_struct _T5=_T0;void*_T6[1];_T6[0]=& _T5;_T1=loc;_T2=_tag_fat(_T6,sizeof(void*),1);Cyc_Warn_err2(_T1,_T2);}{struct Cyc_Parse_Exit_exn_struct*_T5=_cycalloc(sizeof(struct Cyc_Parse_Exit_exn_struct));
_T5->tag=Cyc_Parse_Exit;_T3=(struct Cyc_Parse_Exit_exn_struct*)_T5;}_T4=(void*)_T3;_throw(_T4);}
# 191
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){struct _tuple8*_T0;struct Cyc_Absyn_Tqual _T1;int _T2;struct Cyc_Absyn_Tqual _T3;int _T4;struct Cyc_Absyn_Tqual _T5;int _T6;struct Cyc_Absyn_Tqual _T7;unsigned _T8;struct Cyc_Absyn_Tqual _T9;unsigned _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;void*_TD;void*_TE;struct Cyc_Absyn_Tqual _TF;_T0=tqt;{struct _tuple8 _T10=*_T0;_TF=_T10.f1;_TE=_T10.f2;}{struct Cyc_Absyn_Tqual tq=_TF;void*t=_TE;_T1=tq;_T2=_T1.print_const;
# 194
if(_T2)goto _TL7;else{goto _TL9;}_TL9: _T3=tq;_T4=_T3.q_volatile;if(_T4)goto _TL7;else{goto _TL8;}_TL8: _T5=tq;_T6=_T5.q_restrict;if(_T6)goto _TL7;else{goto _TL5;}
_TL7: _T7=tq;_T8=_T7.loc;if(_T8==0U)goto _TLA;_T9=tq;loc=_T9.loc;goto _TLB;_TLA: _TLB: _TA=loc;_TB=
_tag_fat("qualifier on type is ignored",sizeof(char),29U);_TC=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TA,_TB,_TC);goto _TL6;_TL5: _TL6: _TD=t;
# 198
return _TD;}}
# 201
static void*Cyc_Parse_make_pointer_mod(struct _RegionHandle*r,struct Cyc_Absyn_PtrLoc*loc,void*nullable,void*bound,void*eff,struct Cyc_List_List*pqs,struct Cyc_Absyn_Tqual tqs){struct Cyc_List_List*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;int(*_T5)(struct _fat_ptr,struct _fat_ptr);void*(*_T6)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_List_List*_T9;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_TA;struct _RegionHandle*_TB;struct Cyc_Core_Opt*_TC;struct Cyc_Core_Opt*_TD;void*_TE;void*_TF;void*_T10;
# 206
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*autoreleased=Cyc_Tcutil_any_bool(0);
void*aqual=0;
_TLF: if(pqs!=0)goto _TLD;else{goto _TLE;}
_TLD: _T0=pqs;{void*_T11=_T0->hd;void*_T12;_T1=(int*)_T11;_T2=*_T1;switch(_T2){case 5:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 6:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 7:
 autoreleased=Cyc_Absyn_true_type;goto _LL0;case 9:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 8:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 4:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 3:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0:{struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T13=(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_T11;_T12=_T13->f1;}{struct Cyc_Absyn_Exp*e=_T12;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}case 10:{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T13=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T11;_T3=_T13->f1;_T12=(void*)_T3;}{void*aq=_T12;
aqual=aq;goto _LL0;}case 2:{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T13=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T11;_T12=_T13->f1;}{struct Cyc_List_List*ts=_T12;
eff=Cyc_Absyn_join_eff(ts);goto _LL0;}default:{struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_T13=(struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_T11;_T4=_T13->f1;_T12=(void*)_T4;}{void*t=_T12;_T6=Cyc_Warn_impos;{
int(*_T13)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T6;_T5=_T13;}_T7=_tag_fat("Found region pointer qual",sizeof(char),26U);_T8=_tag_fat(0U,sizeof(void*),0);_T5(_T7,_T8);goto _LL0;}}_LL0:;}_T9=pqs;
# 209
pqs=_T9->tl;goto _TLF;_TLE: _TB=r;{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_T11=_region_malloc(_TB,0U,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_T11->tag=2;
# 223
_T11->f1.eff=eff;_T11->f1.nullable=nullable;_T11->f1.bounds=bound;_T11->f1.zero_term=zeroterm;
_T11->f1.ptrloc=loc;_T11->f1.autoreleased=autoreleased;
if(aqual==0)goto _TL11;_T11->f1.aqual=aqual;goto _TL12;_TL11: _TC=& Cyc_Kinds_aqko;_TD=(struct Cyc_Core_Opt*)_TC;_TE=Cyc_Absyn_new_evar(_TD,0);_TF=Cyc_Absyn_rtd_qual_type;_T11->f1.aqual=Cyc_Absyn_aqual_var_type(_TE,_TF);_TL12:
 _T11->f2=tqs;_TA=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_T11;}_T10=(void*)_TA;
# 223
return _T10;}
# 230
static void Cyc_Parse_check_single_constraint(unsigned loc,struct _fat_ptr id){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;struct Cyc_String_pa_PrintArg_struct _T3;unsigned _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;_T0=id;_T1=
_tag_fat("single",sizeof(char),7U);_T2=Cyc_zstrcmp(_T0,_T1);if(!_T2)goto _TL13;{struct Cyc_String_pa_PrintArg_struct _T7;_T7.tag=0;
_T7.f1=_tag_fat(" is not a valid effect constraint",sizeof(char),34U);_T3=_T7;}{struct Cyc_String_pa_PrintArg_struct _T7=_T3;void*_T8[1];_T8[0]=& _T7;_T4=loc;_T5=id;_T6=_tag_fat(_T8,sizeof(void*),1);Cyc_Warn_err(_T4,_T5,_T6);}goto _TL14;_TL13: _TL14:;}
# 234
static void*Cyc_Parse_effect_from_atomic(struct Cyc_List_List*effs){int _T0;struct Cyc_List_List*_T1;void*_T2;void*_T3;_T0=
Cyc_List_length(effs);if(_T0!=1)goto _TL15;_T1=
_check_null(effs);_T2=_T1->hd;return _T2;
# 238
_TL15: _T3=Cyc_Absyn_join_eff(effs);return _T3;}
# 241
static struct _tuple0*Cyc_Parse_gensym_enum (void){struct _tuple0*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct Cyc_Int_pa_PrintArg_struct _T3;int _T4;int _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;
static int enum_counter=0;{struct _tuple0*_T8=_cycalloc(sizeof(struct _tuple0));
_T8->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T9=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _TA;_TA.tag=1;_T4=enum_counter;
enum_counter=_T4 + 1;_T5=_T4;_TA.f1=(unsigned long)_T5;_T3=_TA;}{struct Cyc_Int_pa_PrintArg_struct _TA=_T3;void*_TB[1];_TB[0]=& _TA;_T6=_tag_fat("__anonymous_enum_%d__",sizeof(char),22U);_T7=_tag_fat(_TB,sizeof(void*),1);_T2=Cyc_aprintf(_T6,_T7);}*_T9=_T2;_T1=(struct _fat_ptr*)_T9;}_T8->f1=_T1;_T0=(struct _tuple0*)_T8;}
# 243
return _T0;}struct _tuple15{unsigned f0;struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple17{struct _tuple15*f0;struct _tuple16*f1;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple17*field_info){struct _tuple17*_T0;struct _tuple15*_T1;struct _tuple16*_T2;unsigned _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;int _T6;unsigned _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_Absyn_Aggrfield*_TA;struct _tuple0*_TB;struct _tuple0 _TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;void*_T11;struct Cyc_Absyn_Tqual _T12;struct _tuple0*_T13;unsigned _T14;_T0=field_info;{struct _tuple17 _T15=*_T0;_T1=_T15.f0;{struct _tuple15 _T16=*_T1;_T14=_T16.f0;_T13=_T16.f1;_T12=_T16.f2;_T11=_T16.f3;_T10=_T16.f4;_TF=_T16.f5;}_T2=_T15.f1;{struct _tuple16 _T16=*_T2;_TE=_T16.f0;_TD=_T16.f1;}}{unsigned varloc=_T14;struct _tuple0*qid=_T13;struct Cyc_Absyn_Tqual tq=_T12;void*t=_T11;struct Cyc_List_List*tvs=_T10;struct Cyc_List_List*atts=_TF;struct Cyc_Absyn_Exp*widthopt=_TE;struct Cyc_Absyn_Exp*reqopt=_TD;
# 253
if(tvs==0)goto _TL17;_T3=loc;_T4=
_tag_fat("bad type params in struct field",sizeof(char),32U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T3,_T4,_T5);goto _TL18;_TL17: _TL18: _T6=
Cyc_Absyn_is_qvar_qualified(qid);if(!_T6)goto _TL19;_T7=loc;_T8=
_tag_fat("struct or union field cannot be qualified with a namespace",sizeof(char),59U);_T9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T7,_T8,_T9);goto _TL1A;_TL19: _TL1A:{struct Cyc_Absyn_Aggrfield*_T15=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_TB=qid;_TC=*_TB;
_T15->name=_TC.f1;_T15->tq=tq;_T15->type=t;
_T15->width=widthopt;_T15->attributes=atts;
_T15->requires_clause=reqopt;_TA=(struct Cyc_Absyn_Aggrfield*)_T15;}
# 257
return _TA;}}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;{struct Cyc_Parse_Type_specifier _T1;
_T1.Signed_spec=0;
_T1.Unsigned_spec=0;
_T1.Short_spec=0;
_T1.Long_spec=0;
_T1.Long_Long_spec=0;
_T1.Complex_spec=0;
_T1.Valid_type_spec=0;
_T1.Type_spec=Cyc_Absyn_sint_type;
_T1.loc=loc;_T0=_T1;}
# 263
return _T0;}
# 274
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Type_spec=t;
s.Valid_type_spec=1;_T0=s;
return _T0;}
# 280
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Signed_spec=1;_T0=s;
return _T0;}
# 285
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Unsigned_spec=1;_T0=s;
return _T0;}
# 290
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Short_spec=1;_T0=s;
return _T0;}
# 295
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Long_spec=1;_T0=s;
return _T0;}
# 300
static struct Cyc_Parse_Type_specifier Cyc_Parse_complex_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Complex_spec=1;_T0=s;
return _T0;}
# 307
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_T0;int _T1;void*_T2;void*_T3;int _T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;void*_T7;_T1=
# 309
Cyc_Tcutil_is_array_type(t);if(!_T1)goto _TL1B;_T2=t;_T4=argposn;
if(!_T4)goto _TL1D;_T5=& Cyc_Kinds_eko;_T6=(struct Cyc_Core_Opt*)_T5;_T3=Cyc_Absyn_new_evar(_T6,0);goto _TL1E;_TL1D: _T3=Cyc_Absyn_heap_rgn_type;_TL1E: _T7=Cyc_Absyn_al_qual_type;_T0=Cyc_Tcutil_promote_array(_T2,_T3,_T7,0);goto _TL1C;_TL1B: _T0=t;_TL1C:
# 309
 return _T0;}struct _tuple18{struct _fat_ptr*f0;void*f1;};
# 322 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;struct _tuple8*_T2;void*_T3;int*_T4;int _T5;struct _tuple8*_T6;void*_T7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8;void*_T9;int*_TA;int _TB;struct _tuple8*_TC;void*_TD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE;struct Cyc_List_List*_TF;struct _tuple8*_T10;void*_T11;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_List_List*_T17;void*_T18;struct _fat_ptr*_T19;void*_T1A;int*_T1B;int _T1C;void*_T1D;void*_T1E;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T1F;void**_T20;struct _fat_ptr*_T21;struct _fat_ptr _T22;struct Cyc_String_pa_PrintArg_struct _T23;struct _fat_ptr*_T24;struct _fat_ptr _T25;struct _fat_ptr _T26;void**_T27;struct Cyc_Absyn_Tvar*_T28;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T29;struct Cyc_Absyn_Kind*_T2A;struct Cyc_List_List*_T2B;struct _tuple18*_T2C;struct _tuple8*_T2D;struct _fat_ptr*_T2E;struct _tuple8*_T2F;struct _fat_ptr*_T30;struct _tuple8*_T31;struct _fat_ptr*_T32;struct _tuple8*_T33;struct _fat_ptr*_T34;struct _tuple8*_T35;void*_T36;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T37;void*_T38;int*_T39;int _T3A;struct _tuple8*_T3B;void*_T3C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3D;struct Cyc_List_List*_T3E;struct _tuple8*_T3F;void*_T40;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T41;struct Cyc_List_List*_T42;struct Cyc_List_List*_T43;void*_T44;int*_T45;int _T46;struct _tuple8*_T47;void*_T48;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T49;struct Cyc_List_List*_T4A;struct Cyc_List_List*_T4B;struct Cyc_List_List*_T4C;struct _fat_ptr*_T4D;void*_T4E;struct Cyc_List_List*_T4F;void*_T50;void*_T51;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T52;struct Cyc_List_List*_T53;void*_T54;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T55;void**_T56;struct _fat_ptr*_T57;struct _fat_ptr _T58;struct Cyc_String_pa_PrintArg_struct _T59;struct _fat_ptr _T5A;struct _fat_ptr _T5B;void**_T5C;struct Cyc_Absyn_Tvar*_T5D;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T5E;struct Cyc_Absyn_Kind*_T5F;struct _tuple8*_T60;struct _fat_ptr*_T61;struct Cyc_List_List*_T62;struct Cyc_List_List*_T63;
struct Cyc_List_List*res=0;
_TL22: if(x!=0)goto _TL20;else{goto _TL21;}
_TL20: _T0=x;_T1=_T0->hd;{struct _tuple8*_T64=(struct _tuple8*)_T1;struct _fat_ptr _T65;void*_T66;void*_T67;_T2=(struct _tuple8*)_T64;_T3=_T2->f2;_T4=(int*)_T3;_T5=*_T4;if(_T5!=0)goto _TL23;_T6=(struct _tuple8*)_T64;_T7=_T6->f2;_T8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7;_T9=_T8->f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=5)goto _TL25;_TC=(struct _tuple8*)_T64;_TD=_TC->f2;_TE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD;_TF=_TE->f2;if(_TF==0)goto _TL27;_T10=(struct _tuple8*)_T64;_T11=_T10->f2;_T12=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T13=_T12->f2;_T14=(struct Cyc_List_List*)_T13;_T15=_T14->tl;if(_T15!=0)goto _TL29;{struct _tuple8 _T68=*_T64;_T67=_T68.f0;_T16=_T68.f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T69=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T16;_T17=_T69->f2;{struct Cyc_List_List _T6A=*_T17;_T18=_T6A.hd;_T66=(void*)_T18;}}}_T19=(struct _fat_ptr*)_T67;if(_T19==0)goto _TL2B;{struct _fat_ptr*v=_T67;void*i=_T66;{void*_T68;_T1A=i;_T1B=(int*)_T1A;_T1C=*_T1B;if(_T1C!=1)goto _TL2D;_T1D=i;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T69=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T1D;_T1E=i;_T1F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T1E;_T20=& _T1F->f2;_T68=(void**)_T20;}{void**z=(void**)_T68;
# 331
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr));_T21=nm;{struct Cyc_String_pa_PrintArg_struct _T69;_T69.tag=0;_T24=v;_T69.f1=*_T24;_T23=_T69;}{struct Cyc_String_pa_PrintArg_struct _T69=_T23;void*_T6A[1];_T6A[0]=& _T69;_T25=_tag_fat("`%s",sizeof(char),4U);_T26=_tag_fat(_T6A,sizeof(void*),1);_T22=Cyc_aprintf(_T25,_T26);}*_T21=_T22;_T27=z;{struct Cyc_Absyn_Tvar*_T69=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_T69->name=nm;_T69->identity=- 1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T6A=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T6A->tag=0;_T2A=& Cyc_Kinds_ik;_T6A->f1=(struct Cyc_Absyn_Kind*)_T2A;_T29=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T6A;}_T69->kind=(void*)_T29;_T69->aquals_bound=0;_T28=(struct Cyc_Absyn_Tvar*)_T69;}*_T27=Cyc_Absyn_var_type(_T28);goto _LL7;}_TL2D: goto _LL7;_LL7:;}{struct Cyc_List_List*_T68=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T69=_cycalloc(sizeof(struct _tuple18));
# 336
_T69->f0=v;_T69->f1=i;_T2C=(struct _tuple18*)_T69;}_T68->hd=_T2C;_T68->tl=res;_T2B=(struct Cyc_List_List*)_T68;}res=_T2B;goto _LL0;}_TL2B: _T2D=(struct _tuple8*)_T64;_T2E=_T2D->f0;if(_T2E==0)goto _TL2F;goto _LL5;_TL2F: goto _LL5;_TL29: _T2F=(struct _tuple8*)_T64;_T30=_T2F->f0;if(_T30==0)goto _TL31;goto _LL5;_TL31: goto _LL5;_TL27: _T31=(struct _tuple8*)_T64;_T32=_T31->f0;if(_T32==0)goto _TL33;goto _LL5;_TL33: goto _LL5;_TL25: _T33=(struct _tuple8*)_T64;_T34=_T33->f0;if(_T34==0)goto _TL35;_T35=(struct _tuple8*)_T64;_T36=_T35->f2;_T37=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T36;_T38=_T37->f1;_T39=(int*)_T38;_T3A=*_T39;if(_T3A!=4)goto _TL37;_T3B=(struct _tuple8*)_T64;_T3C=_T3B->f2;_T3D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C;_T3E=_T3D->f2;if(_T3E==0)goto _TL39;_T3F=(struct _tuple8*)_T64;_T40=_T3F->f2;_T41=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T40;_T42=_T41->f2;_T43=(struct Cyc_List_List*)_T42;_T44=_T43->hd;_T45=(int*)_T44;_T46=*_T45;if(_T46!=1)goto _TL3B;_T47=(struct _tuple8*)_T64;_T48=_T47->f2;_T49=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T48;_T4A=_T49->f2;_T4B=(struct Cyc_List_List*)_T4A;_T4C=_T4B->tl;if(_T4C!=0)goto _TL3D;{struct _tuple8 _T68=*_T64;_T4D=_T68.f0;{struct _fat_ptr _T69=*_T4D;_T65=_T69;}_T4E=_T68.f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T69=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4E;_T4F=_T69->f2;{struct Cyc_List_List _T6A=*_T4F;_T50=_T6A.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T50;_T51=_T64->f2;_T52=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T51;_T53=_T52->f2;_T54=_T53->hd;_T55=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T54;_T56=& _T55->f2;_T67=(void**)_T56;}}}}{struct _fat_ptr v=_T65;void**z=(void**)_T67;
# 340
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr));_T57=nm;{struct Cyc_String_pa_PrintArg_struct _T68;_T68.tag=0;_T68.f1=v;_T59=_T68;}{struct Cyc_String_pa_PrintArg_struct _T68=_T59;void*_T69[1];_T69[0]=& _T68;_T5A=_tag_fat("`%s",sizeof(char),4U);_T5B=_tag_fat(_T69,sizeof(void*),1);_T58=Cyc_aprintf(_T5A,_T5B);}*_T57=_T58;_T5C=z;{struct Cyc_Absyn_Tvar*_T68=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_T68->name=nm;_T68->identity=- 1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T69=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T69->tag=0;_T5F=& Cyc_Kinds_ek;_T69->f1=(struct Cyc_Absyn_Kind*)_T5F;_T5E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T69;}_T68->kind=(void*)_T5E;_T68->aquals_bound=0;_T5D=(struct Cyc_Absyn_Tvar*)_T68;}*_T5C=Cyc_Absyn_var_type(_T5D);goto _LL0;}_TL3D: goto _LL5;_TL3B: goto _LL5;_TL39: goto _LL5;_TL37: goto _LL5;_TL35: goto _LL5;_TL23: _T60=(struct _tuple8*)_T64;_T61=_T60->f0;if(_T61==0)goto _TL3F;goto _LL5;_TL3F: _LL5: goto _LL0;_LL0:;}_T62=x;
# 324
x=_T62->tl;goto _TL22;_TL21: _T63=res;
# 345
return _T63;}
# 349
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Aggrfield*_T2;int*_T3;int _T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;int _T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;struct Cyc_List_List*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_List_List*_T11;struct _tuple18*_T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_Absyn_Aggrfield*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;
struct Cyc_List_List*res=0;
_TL44: if(x!=0)goto _TL42;else{goto _TL43;}
_TL42: _T0=x;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Aggrfield*)_T1;{void*_T18=_T2->type;void*_T19;_T3=(int*)_T18;_T4=*_T3;if(_T4!=0)goto _TL45;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=5)goto _TL47;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_TA=_T9->f2;if(_TA==0)goto _TL49;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_TC=_TB->f2;_TD=(struct Cyc_List_List*)_TC;_TE=_TD->tl;if(_TE!=0)goto _TL4B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_TF=_T1A->f2;{struct Cyc_List_List _T1B=*_TF;_T10=_T1B.hd;_T19=(void*)_T10;}}{void*i=_T19;{struct Cyc_List_List*_T1A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T1B=_cycalloc(sizeof(struct _tuple18));_T13=x;_T14=_T13->hd;_T15=(struct Cyc_Absyn_Aggrfield*)_T14;
# 354
_T1B->f0=_T15->name;_T1B->f1=i;_T12=(struct _tuple18*)_T1B;}_T1A->hd=_T12;_T1A->tl=res;_T11=(struct Cyc_List_List*)_T1A;}res=_T11;goto _LL0;}_TL4B: goto _LL3;_TL49: goto _LL3;_TL47: goto _LL3;_TL45: _LL3: goto _LL0;_LL0:;}_T16=x;
# 351
x=_T16->tl;goto _TL44;_TL43: _T17=res;
# 357
return _T17;}
# 361
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;int _T6;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T7;void*_T8;struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T9;struct _tuple0*_TA;struct _tuple0*_TB;union Cyc_Absyn_Nmspace _TC;struct _union_Nmspace_Rel_n _TD;unsigned _TE;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TF;void*_T10;struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T11;struct _tuple0*_T12;struct _tuple0*_T13;union Cyc_Absyn_Nmspace _T14;struct _union_Nmspace_Rel_n _T15;struct Cyc_List_List*_T16;void*_T17;struct _tuple0*_T18;struct Cyc_List_List*_T19;void*_T1A;int _T1B;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T1C;void*_T1D;struct Cyc_Absyn_Exp*_T1E;unsigned _T1F;struct Cyc_Absyn_Exp*_T20;struct Cyc_List_List*_T21;struct Cyc_Absyn_Exp*_T22;_T0=e;{
void*_T23=_T0->r;struct _fat_ptr*_T24;_T1=(int*)_T23;_T2=*_T1;if(_T2!=1)goto _TL4D;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T23;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL4F;_T7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T23;_T8=_T7->f1;_T9=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T8;_TA=_T9->f1;_TB=(struct _tuple0*)_TA;_TC=_TB->f0;_TD=_TC.Rel_n;_TE=_TD.tag;if(_TE!=2)goto _TL51;_TF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T23;_T10=_TF->f1;_T11=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T10;_T12=_T11->f1;_T13=(struct _tuple0*)_T12;_T14=_T13->f0;_T15=_T14.Rel_n;_T16=_T15.val;if(_T16!=0)goto _TL53;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T25=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T23;_T17=_T25->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T26=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T17;_T18=_T26->f1;{struct _tuple0 _T27=*_T18;_T24=_T27.f1;}}}{struct _fat_ptr*y=_T24;{
# 364
struct Cyc_List_List*ts=tags;_TL58: if(ts!=0)goto _TL56;else{goto _TL57;}
_TL56: _T19=ts;_T1A=_T19->hd;{struct _tuple18*_T25=(struct _tuple18*)_T1A;void*_T26;struct _fat_ptr*_T27;{struct _tuple18 _T28=*_T25;_T27=_T28.f0;_T26=_T28.f1;}{struct _fat_ptr*x=_T27;void*i=_T26;_T1B=
Cyc_strptrcmp(x,y);if(_T1B!=0)goto _TL59;{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T28=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_T28->tag=38;
_T28->f1=Cyc_Tcutil_copy_type(i);_T1C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T28;}_T1D=(void*)_T1C;_T1E=e;_T1F=_T1E->loc;_T20=Cyc_Absyn_new_exp(_T1D,_T1F);return _T20;_TL59:;}}_T21=ts;
# 364
ts=_T21->tl;goto _TL58;_TL57:;}goto _LL0;}_TL53: goto _LL3;_TL51: goto _LL3;_TL4F: goto _LL3;_TL4D: _LL3: goto _LL0;_LL0:;}_T22=e;
# 372
return _T22;}
# 377
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_ArrayInfo _T4;struct Cyc_Absyn_ArrayInfo _T5;struct Cyc_Absyn_ArrayInfo _T6;struct Cyc_Absyn_ArrayInfo _T7;struct Cyc_Absyn_ArrayInfo _T8;void*_T9;void*_TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrInfo _TC;struct Cyc_Absyn_PtrInfo _TD;struct Cyc_Absyn_PtrAtts _TE;struct Cyc_Absyn_PtrInfo _TF;struct Cyc_Absyn_PtrAtts _T10;struct Cyc_Absyn_PtrInfo _T11;struct Cyc_Absyn_PtrAtts _T12;struct Cyc_Absyn_PtrInfo _T13;struct Cyc_Absyn_PtrAtts _T14;struct Cyc_Absyn_PtrInfo _T15;struct Cyc_Absyn_PtrAtts _T16;struct Cyc_Absyn_PtrInfo _T17;struct Cyc_Absyn_PtrAtts _T18;struct Cyc_Absyn_PtrInfo _T19;struct Cyc_Absyn_PtrAtts _T1A;struct Cyc_Absyn_PtrInfo _T1B;void*_T1C;void*_T1D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1E;void*_T1F;int*_T20;int _T21;void*_T22;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_List_List*_T2B;void*_T2C;void*_T2D;void*_T2E;void*_T2F;void*_T30;{struct Cyc_Absyn_Exp*_T31;void*_T32;void*_T33;struct Cyc_Absyn_PtrLoc*_T34;void*_T35;void*_T36;unsigned _T37;void*_T38;void*_T39;struct Cyc_Absyn_Tqual _T3A;void*_T3B;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 5: _T3=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T3C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3;_T4=_T3C->f1;_T3B=_T4.elt_type;_T5=_T3C->f1;_T3A=_T5.tq;_T6=_T3C->f1;_T39=_T6.num_elts;_T7=_T3C->f1;_T38=_T7.zero_term;_T8=_T3C->f1;_T37=_T8.zt_loc;}{void*et=_T3B;struct Cyc_Absyn_Tqual tq=_T3A;struct Cyc_Absyn_Exp*nelts=_T39;void*zt=_T38;unsigned ztloc=_T37;
# 380
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts==0)goto _TL5C;
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);goto _TL5D;_TL5C: _TL5D: {
void*et2=Cyc_Parse_substitute_tags(tags,et);
if(nelts!=nelts2)goto _TL60;else{goto _TL61;}_TL61: if(et!=et2)goto _TL60;else{goto _TL5E;}
_TL60: _T9=Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);return _T9;_TL5E: goto _LL0;}}case 4: _TA=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TA;_TB=_T3C->f1;_T3B=_TB.elt_type;_TC=_T3C->f1;_T3A=_TC.elt_tq;_TD=_T3C->f1;_TE=_TD.ptr_atts;_T39=_TE.eff;_TF=_T3C->f1;_T10=_TF.ptr_atts;_T38=_T10.nullable;_T11=_T3C->f1;_T12=_T11.ptr_atts;_T36=_T12.bounds;_T13=_T3C->f1;_T14=_T13.ptr_atts;_T35=_T14.zero_term;_T15=_T3C->f1;_T16=_T15.ptr_atts;_T34=_T16.ptrloc;_T17=_T3C->f1;_T18=_T17.ptr_atts;_T33=_T18.autoreleased;_T19=_T3C->f1;_T1A=_T19.ptr_atts;_T32=_T1A.aqual;}{void*et=_T3B;struct Cyc_Absyn_Tqual tq=_T3A;void*r=_T39;void*n=_T38;void*b=_T36;void*zt=_T35;struct Cyc_Absyn_PtrLoc*pl=_T34;void*rel=_T33;void*aq=_T32;
# 388
void*et2=Cyc_Parse_substitute_tags(tags,et);
void*b2=Cyc_Parse_substitute_tags(tags,b);
if(et2!=et)goto _TL64;else{goto _TL65;}_TL65: if(b2!=b)goto _TL64;else{goto _TL62;}
_TL64:{struct Cyc_Absyn_PtrInfo _T3C;_T3C.elt_type=et2;_T3C.elt_tq=tq;_T3C.ptr_atts.eff=r;_T3C.ptr_atts.nullable=n;_T3C.ptr_atts.bounds=b2;_T3C.ptr_atts.zero_term=zt;_T3C.ptr_atts.ptrloc=pl;_T3C.ptr_atts.autoreleased=rel;_T3C.ptr_atts.aqual=aq;_T1B=_T3C;}_T1C=Cyc_Absyn_pointer_type(_T1B);return _T1C;_TL62: goto _LL0;}case 0: _T1D=t;_T1E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1D;_T1F=_T1E->f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=13)goto _TL66;_T22=t;_T23=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T22;_T24=_T23->f2;if(_T24==0)goto _TL68;_T25=t;_T26=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T25;_T27=_T26->f2;_T28=(struct Cyc_List_List*)_T27;_T29=_T28->tl;if(_T29!=0)goto _TL6A;_T2A=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2A;_T2B=_T3C->f2;{struct Cyc_List_List _T3D=*_T2B;_T2C=_T3D.hd;_T3B=(void*)_T2C;}}{void*t=_T3B;
# 394
void*t2=Cyc_Parse_substitute_tags(tags,t);
if(t==t2)goto _TL6C;_T2D=Cyc_Absyn_thin_bounds_type(t2);return _T2D;_TL6C: goto _LL0;}_TL6A: goto _LL9;_TL68: goto _LL9;_TL66: goto _LL9;case 9: _T2E=t;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T3C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T2E;_T31=_T3C->f1;}{struct Cyc_Absyn_Exp*e=_T31;
# 398
struct Cyc_Absyn_Exp*e2=Cyc_Parse_substitute_tags_exp(tags,e);
if(e2==e)goto _TL6E;_T2F=Cyc_Absyn_valueof_type(e2);return _T2F;_TL6E: goto _LL0;}default: _LL9: goto _LL0;}_LL0:;}_T30=t;
# 403
return _T30;}
# 408
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){struct Cyc_Absyn_Aggrfield*_T0;struct Cyc_List_List*_T1;struct Cyc_Absyn_Aggrfield*_T2;void*_T3;_T0=x;_T1=tags;_T2=x;_T3=_T2->type;
_T0->type=Cyc_Parse_substitute_tags(_T1,_T3);}struct _tuple19{struct Cyc_Absyn_Tqual f0;void*f1;};struct _tuple20{void*f0;struct Cyc_Absyn_Tqual f1;void*f2;};
# 415
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple20*t){struct _tuple19*_T0;struct _tuple20*_T1;struct _tuple20 _T2;struct _tuple20*_T3;struct _tuple20 _T4;{struct _tuple19*_T5=_cycalloc(sizeof(struct _tuple19));_T1=t;_T2=*_T1;
_T5->f0=_T2.f1;_T3=t;_T4=*_T3;_T5->f1=_T4.f2;_T0=(struct _tuple19*)_T5;}return _T0;}
# 419
static int Cyc_Parse_is_typeparam(void*tm){void*_T0;int*_T1;int _T2;_T0=tm;_T1=(int*)_T0;_T2=*_T1;if(_T2!=4)goto _TL70;
# 421
return 1;_TL70:
 return 0;;}
# 428
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k,void*aliashint,unsigned loc){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;void*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;int _T6;void*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;void*_TB;int _TC;void*_TD;struct Cyc_Absyn_Tvar*_TE;struct _fat_ptr*_TF;void*_T10;_T0=s;_T1=
_tag_fat("`H",sizeof(char),3U);_T2=Cyc_zstrcmp(_T0,_T1);if(_T2!=0)goto _TL72;_T3=Cyc_Absyn_heap_rgn_type;
return _T3;_TL72: _T4=s;_T5=
# 432
_tag_fat("`U",sizeof(char),3U);_T6=Cyc_zstrcmp(_T4,_T5);if(_T6!=0)goto _TL74;_T7=Cyc_Absyn_unique_rgn_shorthand_type;
return _T7;_TL74: _T8=s;_T9=
_tag_fat("`RC",sizeof(char),4U);_TA=Cyc_zstrcmp(_T8,_T9);if(_TA!=0)goto _TL76;_TB=Cyc_Absyn_refcnt_rgn_shorthand_type;
return _TB;_TL76: _TC=
Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name);if(_TC!=0)goto _TL78;_TD=
Cyc_CurRgn_curr_rgn_type();return _TD;_TL78:
 aliashint=Cyc_Kinds_consistent_aliashint(loc,k,aliashint);{struct Cyc_Absyn_Tvar*_T11=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));{struct _fat_ptr*_T12=_cycalloc(sizeof(struct _fat_ptr));
*_T12=s;_TF=(struct _fat_ptr*)_T12;}_T11->name=_TF;_T11->identity=- 1;_T11->kind=k;_T11->aquals_bound=aliashint;_TE=(struct Cyc_Absyn_Tvar*)_T11;}_T10=Cyc_Absyn_var_type(_TE);return _T10;}
# 442
static void*Cyc_Parse_id2aqual(unsigned loc,struct _fat_ptr s){unsigned long _T0;struct _fat_ptr _T1;unsigned char*_T2;const char*_T3;const char*_T4;int _T5;void*_T6;void*_T7;void*_T8;unsigned long _T9;struct _fat_ptr _TA;unsigned char*_TB;const char*_TC;const char*_TD;char _TE;int _TF;struct _fat_ptr _T10;unsigned char*_T11;const char*_T12;char _T13;int _T14;void*_T15;struct Cyc_Warn_String_Warn_Warg_struct _T16;struct Cyc_Warn_String_Warn_Warg_struct _T17;unsigned _T18;struct _fat_ptr _T19;void*_T1A;_T0=
Cyc_strlen(s);if(_T0!=2U)goto _TL7A;_T1=s;_T2=_T1.curr;_T3=(const char*)_T2;_T4=
_check_null(_T3);{char _T1B=_T4[1];_T5=(int)_T1B;switch(_T5){case 65: _T6=Cyc_Absyn_al_qual_type;
return _T6;case 85: _T7=Cyc_Absyn_un_qual_type;
return _T7;case 84: _T8=Cyc_Absyn_rtd_qual_type;
return _T8;default: goto _LL0;}_LL0:;}goto _TL7B;
# 451
_TL7A: _T9=Cyc_strlen(s);if(_T9!=3U)goto _TL7D;_TA=s;_TB=_TA.curr;_TC=(const char*)_TB;_TD=
_check_null(_TC);_TE=_TD[1];_TF=(int)_TE;if(_TF!=82)goto _TL7F;_T10=s;_T11=_T10.curr;_T12=(const char*)_T11;_T13=_T12[2];_T14=(int)_T13;if(_T14!=67)goto _TL7F;_T15=Cyc_Absyn_rc_qual_type;
return _T15;_TL7F: goto _TL7E;_TL7D: _TL7E: _TL7B:{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;
# 455
_T1B.f1=_tag_fat("bad aqual bound ",sizeof(char),17U);_T16=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_T16;{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;_T1C.f1=s;_T17=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_T17;void*_T1D[2];_T1D[0]=& _T1B;_T1D[1]=& _T1C;_T18=loc;_T19=_tag_fat(_T1D,sizeof(void*),2);Cyc_Warn_err2(_T18,_T19);}}_T1A=Cyc_Absyn_al_qual_type;
return _T1A;}
# 459
static struct Cyc_List_List*Cyc_Parse_insert_aqual(struct _RegionHandle*yy,struct Cyc_List_List*qlist,void*aq,unsigned loc){struct Cyc_List_List*_T0;int*_T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;unsigned _T4;struct _fat_ptr _T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct _RegionHandle*_T9;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_TA;struct _RegionHandle*_TB;{
struct Cyc_List_List*l=qlist;_TL84: if(l!=0)goto _TL82;else{goto _TL83;}
_TL82: _T0=l;{void*_TC=_T0->hd;_T1=(int*)_TC;_T2=*_T1;if(_T2!=10)goto _TL85;{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;
# 463
_TD.f1=_tag_fat("Multiple alias qualifiers",sizeof(char),26U);_T3=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T3;void*_TE[1];_TE[0]=& _TD;_T4=loc;_T5=_tag_fat(_TE,sizeof(void*),1);Cyc_Warn_err2(_T4,_T5);}_T6=qlist;
return _T6;_TL85: goto _LL0;_LL0:;}_T7=l;
# 460
l=_T7->tl;goto _TL84;_TL83:;}_T9=yy;{struct Cyc_List_List*_TC=_region_malloc(_T9,0U,sizeof(struct Cyc_List_List));_TB=yy;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_TD=_region_malloc(_TB,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_TD->tag=10;
# 469
_TD->f1=aq;_TA=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_TD;}_TC->hd=(void*)_TA;_TC->tl=qlist;_T8=(struct Cyc_List_List*)_TC;}return _T8;}
# 472
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;unsigned _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;int _T8;unsigned _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct _fat_ptr _TD;int _TE;unsigned _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;int _T12;unsigned _T13;struct _fat_ptr _T14;struct _fat_ptr _T15;_T0=s;_T1=
_tag_fat("`H",sizeof(char),3U);_T2=Cyc_zstrcmp(_T0,_T1);if(_T2!=0)goto _TL87;_T3=loc;_T4=
_tag_fat("bad occurrence of heap region",sizeof(char),30U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T3,_T4,_T5);goto _TL88;_TL87: _TL88: _T6=s;_T7=
_tag_fat("`U",sizeof(char),3U);_T8=Cyc_zstrcmp(_T6,_T7);if(_T8!=0)goto _TL89;_T9=loc;_TA=
_tag_fat("bad occurrence of unique region",sizeof(char),32U);_TB=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T9,_TA,_TB);goto _TL8A;_TL89: _TL8A: _TC=s;_TD=
_tag_fat("`RC",sizeof(char),4U);_TE=Cyc_zstrcmp(_TC,_TD);if(_TE!=0)goto _TL8B;_TF=loc;_T10=
_tag_fat("bad occurrence of refcounted region",sizeof(char),36U);_T11=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TF,_T10,_T11);goto _TL8C;_TL8B: _TL8C: _T12=
Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name);if(_T12!=0)goto _TL8D;_T13=loc;_T14=
_tag_fat("bad occurrence of \"current\" region",sizeof(char),35U);_T15=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T13,_T14,_T15);goto _TL8E;_TL8D: _TL8E:;}
# 487
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_Tvar*_T4;int(*_T5)(unsigned,struct _fat_ptr);unsigned _T6;struct _fat_ptr _T7;struct Cyc_Absyn_Tvar*_T8;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=2)goto _TL8F;_T3=t;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T3;_T8=_T9->f1;}{struct Cyc_Absyn_Tvar*pr=_T8;_T4=pr;
# 489
return _T4;}_TL8F:{
int(*_T9)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T5=_T9;}_T6=loc;_T7=_tag_fat("expecting a list of type variables, not types",sizeof(char),46U);_T5(_T6,_T7);;}
# 495
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){int*_T0;int _T1;struct Cyc_Absyn_Tvar*_T2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T3;struct Cyc_Absyn_Tvar*_T4;void**_T5;void**_T6;void*_T7;int*_T8;int _T9;void**_TA;void*_TB;int _TC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TD;
void*_TE=Cyc_Absyn_compress(t);void*_TF;_T0=(int*)_TE;_T1=*_T0;if(_T1!=2)goto _TL91;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T10=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TE;_T2=_T10->f1;{struct Cyc_Absyn_Tvar _T11=*_T2;_T3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TE;_T4=_T3->f1;_T5=& _T4->kind;_TF=(void**)_T5;}}{void**cptr=(void**)_TF;_T6=cptr;_T7=*_T6;{
# 498
void*_T10=Cyc_Kinds_compress_kb(_T7);_T8=(int*)_T10;_T9=*_T8;if(_T9!=1)goto _TL93;_TA=cptr;_TC=leq;
# 500
if(!_TC)goto _TL95;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T11=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T11->tag=2;_T11->f1=0;_T11->f2=k;_TD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T11;}_TB=(void*)_TD;goto _TL96;_TL95: _TB=Cyc_Kinds_kind_to_bound(k);_TL96:*_TA=_TB;return;_TL93:
 return;;}}goto _TL92;_TL91:
# 503
 return;_TL92:;}
# 508
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){struct Cyc_List_List*_T0;int*_T1;int _T2;void*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;void*_T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;void*_TE;int*_TF;int _T10;unsigned _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_List_List*_T14;void*_T15;int _T16;int _T17;int(*_T18)(unsigned,struct _fat_ptr);unsigned _T19;struct _fat_ptr _T1A;struct Cyc_List_List*_T1B;void*_T1C;struct Cyc_Absyn_Decl*_T1D;int*_T1E;int _T1F;struct Cyc_Absyn_Vardecl*_T20;struct _tuple0*_T21;struct _tuple0 _T22;struct _fat_ptr*_T23;struct Cyc_List_List*_T24;void*_T25;struct _fat_ptr*_T26;int _T27;struct Cyc_Absyn_Vardecl*_T28;struct Cyc_Absyn_Exp*_T29;int(*_T2A)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Decl*_T2B;unsigned _T2C;struct _fat_ptr _T2D;struct Cyc_Absyn_Vardecl*_T2E;struct _tuple0*_T2F;int _T30;int(*_T31)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Decl*_T32;unsigned _T33;struct _fat_ptr _T34;struct Cyc_List_List*_T35;struct _tuple8*_T36;struct Cyc_Absyn_Vardecl*_T37;struct _tuple0*_T38;struct _tuple0 _T39;struct Cyc_Absyn_Vardecl*_T3A;struct Cyc_Absyn_Vardecl*_T3B;int(*_T3C)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Decl*_T3D;unsigned _T3E;struct _fat_ptr _T3F;struct Cyc_List_List*_T40;int(*_T41)(unsigned,struct _fat_ptr);unsigned _T42;struct Cyc_List_List*_T43;void*_T44;struct _fat_ptr*_T45;struct _fat_ptr _T46;struct _fat_ptr _T47;struct _fat_ptr _T48;struct Cyc_List_List*_T49;struct Cyc_List_List*_T4A;struct _RegionHandle*_T4B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T4C;struct _RegionHandle*_T4D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T4E;struct _RegionHandle*_T4F;struct Cyc_List_List*_T50;struct _RegionHandle*_T51;struct Cyc_List_List*_T52;struct _RegionHandle*_T53;struct Cyc_List_List*_T54;struct Cyc_List_List*_T55;struct Cyc_List_List*_T56;unsigned _T57;
# 516
if(tms!=0)goto _TL97;return 0;_TL97: _T0=tms;{
# 518
void*_T58=_T0->hd;void*_T59;_T1=(int*)_T58;_T2=*_T1;if(_T2!=3)goto _TL99;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T5A=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T58;_T3=_T5A->f1;_T59=(void*)_T3;}{void*args=_T59;_T4=tms;_T5=_T4->tl;
# 521
if(_T5==0)goto _TL9D;else{goto _TL9E;}_TL9E: _T6=tms;_T7=_T6->tl;_T8=_T7->hd;_T9=
Cyc_Parse_is_typeparam(_T8);
# 521
if(_T9)goto _TL9F;else{goto _TL9B;}_TL9F: _TA=tms;_TB=_TA->tl;_TC=
_check_null(_TB);_TD=_TC->tl;
# 521
if(_TD==0)goto _TL9D;else{goto _TL9B;}
# 524
_TL9D:{struct Cyc_List_List*_T5A;_TE=args;_TF=(int*)_TE;_T10=*_TF;if(_T10!=1)goto _TLA0;_T11=loc;_T12=
# 527
_tag_fat("function declaration with both new- and old-style parameter declarations; ignoring old-style",sizeof(char),93U);_T13=_tag_fat(0U,sizeof(void*),0);
# 526
Cyc_Warn_warn(_T11,_T12,_T13);_T14=tms;
# 528
return _T14;_TLA0: _T15=args;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T5B=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T15;_T5A=_T5B->f1;}{struct Cyc_List_List*ids=_T5A;_T16=
# 530
Cyc_List_length(ids);_T17=Cyc_List_length(tds);if(_T16==_T17)goto _TLA2;{
int(*_T5B)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T18=_T5B;}_T19=loc;_T1A=
_tag_fat("wrong number of parameter declarations in old-style function declaration",sizeof(char),73U);
# 531
_T18(_T19,_T1A);goto _TLA3;_TLA2: _TLA3: {
# 534
struct Cyc_List_List*rev_new_params=0;
_TLA7: if(ids!=0)goto _TLA5;else{goto _TLA6;}
_TLA5:{struct Cyc_List_List*tds2=tds;
_TLAB: if(tds2!=0)goto _TLA9;else{goto _TLAA;}
_TLA9: _T1B=tds2;_T1C=_T1B->hd;{struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)_T1C;_T1D=x;{
void*_T5B=_T1D->r;struct Cyc_Absyn_Vardecl*_T5C;_T1E=(int*)_T5B;_T1F=*_T1E;if(_T1F!=0)goto _TLAC;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T5D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T5B;_T5C=_T5D->f1;}{struct Cyc_Absyn_Vardecl*vd=_T5C;_T20=vd;_T21=_T20->name;_T22=*_T21;_T23=_T22.f1;_T24=ids;_T25=_T24->hd;_T26=(struct _fat_ptr*)_T25;_T27=
# 541
Cyc_zstrptrcmp(_T23,_T26);if(_T27==0)goto _TLAE;goto _TLA8;_TLAE: _T28=vd;_T29=_T28->initializer;
# 543
if(_T29==0)goto _TLB0;{
int(*_T5D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T2A=_T5D;}_T2B=x;_T2C=_T2B->loc;_T2D=_tag_fat("initializer found in parameter declaration",sizeof(char),43U);_T2A(_T2C,_T2D);goto _TLB1;_TLB0: _TLB1: _T2E=vd;_T2F=_T2E->name;_T30=
Cyc_Absyn_is_qvar_qualified(_T2F);if(!_T30)goto _TLB2;{
int(*_T5D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T31=_T5D;}_T32=x;_T33=_T32->loc;_T34=_tag_fat("namespaces forbidden in parameter declarations",sizeof(char),47U);_T31(_T33,_T34);goto _TLB3;_TLB2: _TLB3:{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple8*_T5E=_cycalloc(sizeof(struct _tuple8));_T37=vd;_T38=_T37->name;_T39=*_T38;
_T5E->f0=_T39.f1;_T3A=vd;_T5E->f1=_T3A->tq;_T3B=vd;_T5E->f2=_T3B->type;_T36=(struct _tuple8*)_T5E;}_T5D->hd=_T36;
_T5D->tl=rev_new_params;_T35=(struct Cyc_List_List*)_T5D;}
# 547
rev_new_params=_T35;goto L;}_TLAC:{
# 550
int(*_T5D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T3C=_T5D;}_T3D=x;_T3E=_T3D->loc;_T3F=_tag_fat("nonvariable declaration in parameter type",sizeof(char),42U);_T3C(_T3E,_T3F);;}}_TLA8: _T40=tds2;
# 537
tds2=_T40->tl;goto _TLAB;_TLAA:
# 553
 L: if(tds2!=0)goto _TLB4;{
int(*_T5B)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T41=_T5B;}_T42=loc;_T43=ids;_T44=_T43->hd;_T45=(struct _fat_ptr*)_T44;_T46=*_T45;_T47=_tag_fat(" is not given a type",sizeof(char),21U);_T48=Cyc_strconcat(_T46,_T47);_T41(_T42,_T48);goto _TLB5;_TLB4: _TLB5:;}_T49=ids;
# 535
ids=_T49->tl;goto _TLA7;_TLA6: _T4B=yy;{struct Cyc_List_List*_T5B=_region_malloc(_T4B,0U,sizeof(struct Cyc_List_List));_T4D=yy;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T5C=_region_malloc(_T4D,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T5C->tag=3;_T4F=yy;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T5D=_region_malloc(_T4F,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T5D->tag=1;
# 557
_T5D->f1=Cyc_List_imp_rev(rev_new_params);
_T5D->f2=0;_T5D->f3=0;_T5D->f4=0;_T5D->f5=0;_T5D->f6=0;_T5D->f7=0;_T5D->f8=0;_T5D->f9=0;_T5D->f10=0;_T4E=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T5D;}
# 557
_T5C->f1=(void*)_T4E;_T4C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T5C;}_T5B->hd=(void*)_T4C;
# 559
_T5B->tl=0;_T4A=(struct Cyc_List_List*)_T5B;}
# 556
return _T4A;}};}goto _TL9C;_TL9B: _TL9C: goto _LL4;}_TL99: _LL4: _T51=yy;{struct Cyc_List_List*_T5A=_region_malloc(_T51,0U,sizeof(struct Cyc_List_List));_T52=tms;
# 564
_T5A->hd=_T52->hd;_T53=yy;_T54=tms;_T55=_T54->tl;_T56=tds;_T57=loc;_T5A->tl=Cyc_Parse_oldstyle2newstyle(_T53,_T55,_T56,_T57);_T50=(struct Cyc_List_List*)_T5A;}return _T50;;}}
# 570
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){struct Cyc_Parse_Declarator _T0;struct Cyc_Parse_Declarator _T1;struct Cyc_Parse_Declarator _T2;struct _RegionHandle*_T3;struct Cyc_Parse_Declarator _T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;unsigned _T7;struct Cyc_Parse_Declaration_spec*_T8;struct Cyc_Parse_Declaration_spec*_T9;struct Cyc_Parse_Declaration_spec*_TA;struct Cyc_Parse_Declaration_spec*_TB;struct Cyc_Parse_Declaration_spec*_TC;int _TD;unsigned _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_Absyn_Tqual _T11;void*_T12;struct Cyc_List_List*_T13;struct Cyc_Parse_Declarator _T14;struct Cyc_List_List*_T15;unsigned _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;void*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Absyn_FnInfo _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct _tuple8*_T20;struct _tuple8 _T21;struct _fat_ptr*_T22;unsigned _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_List_List*_T26;void*_T27;struct _tuple8*_T28;struct _fat_ptr*_T29;struct Cyc_List_List*_T2A;struct Cyc_Absyn_FnInfo _T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_Absyn_Fndecl*_T2E;struct Cyc_Parse_Declarator _T2F;int(*_T30)(unsigned,struct _fat_ptr);unsigned _T31;struct _fat_ptr _T32;
# 574
if(tds==0)goto _TLB6;{struct Cyc_Parse_Declarator _T33;_T1=d;
_T33.id=_T1.id;_T2=d;_T33.varloc=_T2.varloc;_T3=yy;_T4=d;_T5=_T4.tms;_T6=tds;_T7=loc;_T33.tms=Cyc_Parse_oldstyle2newstyle(_T3,_T5,_T6,_T7);_T0=_T33;}d=_T0;goto _TLB7;_TLB6: _TLB7: {
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 582
if(dso==0)goto _TLB8;_T8=dso;
tss=_T8->type_specs;_T9=dso;
tq=_T9->tq;_TA=dso;
is_inline=_TA->is_inline;_TB=dso;
atts=_TB->attributes;_TC=dso;{
# 588
enum Cyc_Parse_Storage_class _T33=_TC->sc;_TD=(int)_T33;switch(_TD){case Cyc_Parse_None_sc: goto _LL0;case Cyc_Parse_Extern_sc:
# 590
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default: _TE=loc;_TF=
_tag_fat("bad storage class on function",sizeof(char),30U);_T10=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TE,_TF,_T10);goto _LL0;}_LL0:;}goto _TLB9;_TLB8: _TLB9: {
# 595
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);_T11=tq;_T12=t;_T13=atts;_T14=d;_T15=_T14.tms;{
struct _tuple14 _T33=Cyc_Parse_apply_tms(_T11,_T12,_T13,_T15);struct Cyc_List_List*_T34;struct Cyc_List_List*_T35;void*_T36;struct Cyc_Absyn_Tqual _T37;_T37=_T33.f0;_T36=_T33.f1;_T35=_T33.f2;_T34=_T33.f3;{struct Cyc_Absyn_Tqual fn_tqual=_T37;void*fn_type=_T36;struct Cyc_List_List*x=_T35;struct Cyc_List_List*out_atts=_T34;
# 600
if(x==0)goto _TLBB;_T16=loc;_T17=
_tag_fat("bad type params, ignoring",sizeof(char),26U);_T18=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T16,_T17,_T18);goto _TLBC;_TLBB: _TLBC: {struct Cyc_Absyn_FnInfo _T38;_T19=fn_type;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=6)goto _TLBD;_T1C=fn_type;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T39=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1C;_T38=_T39->f1;}{struct Cyc_Absyn_FnInfo i=_T38;_T1D=i;{
# 605
struct Cyc_List_List*args2=_T1D.args;_TLC2: if(args2!=0)goto _TLC0;else{goto _TLC1;}
_TLC0: _T1E=args2;_T1F=_T1E->hd;_T20=(struct _tuple8*)_T1F;_T21=*_T20;_T22=_T21.f0;if(_T22!=0)goto _TLC3;_T23=loc;_T24=
_tag_fat("missing argument variable in function prototype",sizeof(char),48U);_T25=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T23,_T24,_T25);_T26=args2;_T27=_T26->hd;_T28=(struct _tuple8*)_T27;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));
*_T39=_tag_fat("?",sizeof(char),2U);_T29=(struct _fat_ptr*)_T39;}(*_T28).f0=_T29;goto _TLC4;_TLC3: _TLC4: _T2A=args2;
# 605
args2=_T2A->tl;goto _TLC2;_TLC1:;}_T2B=i;_T2C=_T2B.attributes;_T2D=out_atts;
# 616
i.attributes=Cyc_List_append(_T2C,_T2D);{struct Cyc_Absyn_Fndecl*_T39=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));
_T39->sc=sc;_T39->is_inline=is_inline;_T2F=d;_T39->name=_T2F.id;_T39->body=body;_T39->i=i;
_T39->cached_type=0;_T39->param_vardecls=0;_T39->fn_vardecl=0;
_T39->orig_scope=sc;_T39->escapes=0;_T2E=(struct Cyc_Absyn_Fndecl*)_T39;}
# 617
return _T2E;}_TLBD:{
# 620
int(*_T39)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T30=_T39;}_T31=loc;_T32=_tag_fat("declarator is not a function prototype",sizeof(char),39U);_T30(_T31,_T32);;}}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 624
static struct _fat_ptr Cyc_Parse_msg1={(unsigned char*)_TmpG0,(unsigned char*)_TmpG0,(unsigned char*)_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 626
static struct _fat_ptr Cyc_Parse_msg2={(unsigned char*)_TmpG1,(unsigned char*)_TmpG1,(unsigned char*)_TmpG1 + 84U};
# 633
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){struct Cyc_Parse_Type_specifier _T0;int _T1;struct Cyc_Parse_Type_specifier _T2;int _T3;unsigned _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_Parse_Type_specifier _T7;int _T8;struct Cyc_Parse_Type_specifier _T9;int _TA;struct Cyc_Parse_Type_specifier _TB;int _TC;unsigned _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Parse_Type_specifier _T10;int _T11;struct Cyc_Parse_Type_specifier _T12;int _T13;struct Cyc_Parse_Type_specifier _T14;int _T15;unsigned _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct Cyc_Parse_Type_specifier _T19;int _T1A;struct Cyc_Parse_Type_specifier _T1B;int _T1C;struct Cyc_Parse_Type_specifier _T1D;int _T1E;unsigned _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_Parse_Type_specifier _T22;int _T23;struct Cyc_Parse_Type_specifier _T24;int _T25;struct Cyc_Parse_Type_specifier _T26;int _T27;struct Cyc_Parse_Type_specifier _T28;int _T29;struct Cyc_Parse_Type_specifier _T2A;int _T2B;struct Cyc_Parse_Type_specifier _T2C;int _T2D;struct Cyc_Parse_Type_specifier _T2E;int _T2F;unsigned _T30;struct _fat_ptr _T31;struct _fat_ptr _T32;int _T33;struct Cyc_Parse_Type_specifier _T34;int _T35;struct Cyc_Parse_Type_specifier _T36;int _T37;int _T38;struct Cyc_Parse_Type_specifier _T39;int _T3A;struct Cyc_Parse_Type_specifier _T3B;int _T3C;struct Cyc_Parse_Type_specifier _T3D;int _T3E;int _T3F;struct Cyc_Parse_Type_specifier _T40;int _T41;struct Cyc_Parse_Type_specifier _T42;struct Cyc_Parse_Type_specifier _T43;int _T44;struct Cyc_Parse_Type_specifier _T45;int _T46;unsigned _T47;struct _fat_ptr _T48;struct _fat_ptr _T49;struct Cyc_Parse_Type_specifier _T4A;int _T4B;struct Cyc_Parse_Type_specifier _T4C;struct Cyc_Parse_Type_specifier _T4D;_T0=s1;_T1=_T0.Signed_spec;
# 636
if(!_T1)goto _TLC5;_T2=s2;_T3=_T2.Signed_spec;if(!_T3)goto _TLC5;_T4=loc;_T5=Cyc_Parse_msg2;_T6=_tag_fat(0U,sizeof(void*),0);
Cyc_Warn_warn(_T4,_T5,_T6);goto _TLC6;_TLC5: _TLC6: _T7=s2;_T8=_T7.Signed_spec;
s1.Signed_spec=s1.Signed_spec | _T8;_T9=s1;_TA=_T9.Unsigned_spec;
if(!_TA)goto _TLC7;_TB=s2;_TC=_TB.Unsigned_spec;if(!_TC)goto _TLC7;_TD=loc;_TE=Cyc_Parse_msg2;_TF=_tag_fat(0U,sizeof(void*),0);
Cyc_Warn_warn(_TD,_TE,_TF);goto _TLC8;_TLC7: _TLC8: _T10=s2;_T11=_T10.Unsigned_spec;
s1.Unsigned_spec=s1.Unsigned_spec | _T11;_T12=s1;_T13=_T12.Short_spec;
if(!_T13)goto _TLC9;_T14=s2;_T15=_T14.Short_spec;if(!_T15)goto _TLC9;_T16=loc;_T17=
_tag_fat("too many occurrences of short in specifiers",sizeof(char),44U);_T18=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T16,_T17,_T18);goto _TLCA;_TLC9: _TLCA: _T19=s2;_T1A=_T19.Short_spec;
s1.Short_spec=s1.Short_spec | _T1A;_T1B=s1;_T1C=_T1B.Complex_spec;
if(!_T1C)goto _TLCB;_T1D=s2;_T1E=_T1D.Complex_spec;if(!_T1E)goto _TLCB;_T1F=loc;_T20=
_tag_fat("too many occurrences of _Complex or __complex__ in specifiers",sizeof(char),62U);_T21=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T1F,_T20,_T21);goto _TLCC;_TLCB: _TLCC: _T22=s2;_T23=_T22.Complex_spec;
s1.Complex_spec=s1.Complex_spec | _T23;_T24=s1;_T25=_T24.Long_Long_spec;
if(_T25)goto _TLD2;else{goto _TLD1;}_TLD2: _T26=s2;_T27=_T26.Long_Long_spec;if(_T27)goto _TLCF;else{goto _TLD1;}_TLD1: _T28=s1;_T29=_T28.Long_Long_spec;if(_T29)goto _TLD3;else{goto _TLD0;}_TLD3: _T2A=s2;_T2B=_T2A.Long_spec;if(_T2B)goto _TLCF;else{goto _TLD0;}_TLD0: _T2C=s2;_T2D=_T2C.Long_Long_spec;if(_T2D)goto _TLD4;else{goto _TLCD;}_TLD4: _T2E=s1;_T2F=_T2E.Long_spec;if(_T2F)goto _TLCF;else{goto _TLCD;}
# 651
_TLCF: _T30=loc;_T31=_tag_fat("too many occurrences of long in specifiers",sizeof(char),43U);_T32=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T30,_T31,_T32);goto _TLCE;_TLCD: _TLCE: _T34=s1;_T35=_T34.Long_Long_spec;
# 653
if(_T35)goto _TLD7;else{goto _TLD8;}_TLD8: _T36=s2;_T37=_T36.Long_Long_spec;if(_T37)goto _TLD7;else{goto _TLD5;}_TLD7: _T33=1;goto _TLD6;_TLD5: _T39=s1;_T3A=_T39.Long_spec;if(!_T3A)goto _TLD9;_T3B=s2;_T38=_T3B.Long_spec;goto _TLDA;_TLD9: _T38=0;_TLDA: _T33=_T38;_TLD6:
# 652
 s1.Long_Long_spec=_T33;_T3D=s1;_T3E=_T3D.Long_Long_spec;
# 654
if(_T3E)goto _TLDB;else{goto _TLDD;}_TLDD: _T40=s1;_T41=_T40.Long_spec;if(!_T41)goto _TLDE;_T3F=1;goto _TLDF;_TLDE: _T42=s2;_T3F=_T42.Long_spec;_TLDF: _T3C=_T3F;goto _TLDC;_TLDB: _T3C=0;_TLDC: s1.Long_spec=_T3C;_T43=s1;_T44=_T43.Valid_type_spec;
if(!_T44)goto _TLE0;_T45=s2;_T46=_T45.Valid_type_spec;if(!_T46)goto _TLE0;_T47=loc;_T48=Cyc_Parse_msg1;_T49=_tag_fat(0U,sizeof(void*),0);
Cyc_Warn_err(_T47,_T48,_T49);goto _TLE1;
_TLE0: _T4A=s2;_T4B=_T4A.Valid_type_spec;if(!_T4B)goto _TLE2;_T4C=s2;
s1.Type_spec=_T4C.Type_spec;
s1.Valid_type_spec=1;goto _TLE3;_TLE2: _TLE3: _TLE1: _T4D=s1;
# 661
return _T4D;}
# 667
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){struct Cyc_Parse_Type_specifier _T0;int _T1;struct Cyc_Parse_Type_specifier _T2;int _T3;struct Cyc_Parse_Type_specifier _T4;int _T5;struct Cyc_Parse_Type_specifier _T6;int _T7;struct Cyc_Parse_Type_specifier _T8;int _T9;struct Cyc_Parse_Type_specifier _TA;void*_TB;int _TC;struct Cyc_Parse_Type_specifier _TD;int _TE;int _TF;struct Cyc_Parse_Type_specifier _T10;int _T11;struct Cyc_Parse_Type_specifier _T12;int _T13;unsigned _T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc_Parse_Type_specifier _T17;int _T18;struct Cyc_Parse_Type_specifier _T19;int _T1A;struct Cyc_Parse_Type_specifier _T1B;int _T1C;struct Cyc_Parse_Type_specifier _T1D;int _T1E;struct Cyc_Parse_Type_specifier _T1F;int _T20;struct Cyc_Parse_Type_specifier _T21;int _T22;unsigned _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_Parse_Type_specifier _T26;int _T27;struct Cyc_Parse_Type_specifier _T28;int _T29;struct Cyc_Parse_Type_specifier _T2A;int _T2B;int _T2C;int _T2D;int _T2E;struct Cyc_Parse_Type_specifier _T2F;int _T30;void*_T31;unsigned _T32;struct _fat_ptr _T33;struct _fat_ptr _T34;struct Cyc_Parse_Type_specifier _T35;int _T36;void*_T37;void*_T38;void*_T39;void*_T3A;int*_T3B;int _T3C;void*_T3D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3E;void*_T3F;int*_T40;unsigned _T41;void*_T42;void*_T43;int _T44;enum Cyc_Absyn_Sign _T45;int _T46;enum Cyc_Absyn_Sign _T47;int _T48;int _T49;enum Cyc_Absyn_Size_of _T4A;int _T4B;enum Cyc_Absyn_Size_of _T4C;int _T4D;struct Cyc_Parse_Type_specifier _T4E;int _T4F;int _T50;struct Cyc_Parse_Type_specifier _T51;int _T52;int _T53;unsigned _T54;struct _fat_ptr _T55;struct _fat_ptr _T56;int _T57;unsigned _T58;struct _fat_ptr _T59;struct _fat_ptr _T5A;void*_T5B;_T0=ts;{
int seen_type=_T0.Valid_type_spec;_T2=ts;_T3=_T2.Signed_spec;
if(!_T3)goto _TLE4;_T1=1;goto _TLE5;_TLE4: _T4=ts;_T1=_T4.Unsigned_spec;_TLE5: {int seen_sign=_T1;_T6=ts;_T7=_T6.Short_spec;
if(_T7)goto _TLE8;else{goto _TLE9;}_TLE9: _T8=ts;_T9=_T8.Long_spec;if(_T9)goto _TLE8;else{goto _TLE6;}_TLE8: _T5=1;goto _TLE7;_TLE6: _TA=ts;_T5=_TA.Long_Long_spec;_TLE7: {int seen_size=_T5;_TC=seen_type;
if(!_TC)goto _TLEA;_TD=ts;_TB=_TD.Type_spec;goto _TLEB;_TLEA: _TB=Cyc_Absyn_void_type;_TLEB: {void*t=_TB;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;_TE=seen_size;
# 675
if(_TE)goto _TLEE;else{goto _TLEF;}_TLEF: _TF=seen_sign;if(_TF)goto _TLEE;else{goto _TLEC;}
_TLEE: _T10=ts;_T11=_T10.Signed_spec;if(!_T11)goto _TLF0;_T12=ts;_T13=_T12.Unsigned_spec;if(!_T13)goto _TLF0;_T14=loc;_T15=Cyc_Parse_msg2;_T16=_tag_fat(0U,sizeof(void*),0);
Cyc_Warn_err(_T14,_T15,_T16);goto _TLF1;_TLF0: _TLF1: _T17=ts;_T18=_T17.Unsigned_spec;
if(!_T18)goto _TLF2;sgn=1U;goto _TLF3;_TLF2: _TLF3: _T19=ts;_T1A=_T19.Short_spec;
if(_T1A)goto _TLF8;else{goto _TLF7;}_TLF8: _T1B=ts;_T1C=_T1B.Long_spec;if(_T1C)goto _TLF6;else{goto _TLF9;}_TLF9: _T1D=ts;_T1E=_T1D.Long_Long_spec;if(_T1E)goto _TLF6;else{goto _TLF7;}_TLF7: _T1F=ts;_T20=_T1F.Long_spec;if(_T20)goto _TLFA;else{goto _TLF4;}_TLFA: _T21=ts;_T22=_T21.Long_Long_spec;if(_T22)goto _TLF6;else{goto _TLF4;}
# 681
_TLF6: _T23=loc;_T24=Cyc_Parse_msg2;_T25=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T23,_T24,_T25);goto _TLF5;_TLF4: _TLF5: _T26=ts;_T27=_T26.Short_spec;
if(!_T27)goto _TLFB;sz=1U;goto _TLFC;_TLFB: _TLFC: _T28=ts;_T29=_T28.Long_spec;
if(!_T29)goto _TLFD;sz=3U;goto _TLFE;_TLFD: _TLFE: _T2A=ts;_T2B=_T2A.Long_Long_spec;
if(!_T2B)goto _TLFF;sz=4U;goto _TL100;_TLFF: _TL100: goto _TLED;_TLEC: _TLED: _T2C=seen_type;
# 688
if(_T2C)goto _TL101;else{goto _TL103;}
_TL103: _T2D=seen_sign;if(_T2D)goto _TL104;else{goto _TL106;}_TL106: _T2E=seen_size;if(_T2E)goto _TL104;else{goto _TL107;}
_TL107: _T2F=ts;_T30=_T2F.Complex_spec;if(!_T30)goto _TL108;_T31=
Cyc_Absyn_complex_type(Cyc_Absyn_double_type);return _T31;_TL108: _T32=loc;_T33=
_tag_fat("missing type within specifier",sizeof(char),30U);_T34=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T32,_T33,_T34);goto _TL105;_TL104: _TL105: _T35=ts;_T36=_T35.Complex_spec;
# 694
if(!_T36)goto _TL10A;_T37=
Cyc_Absyn_int_type(sgn,sz);_T38=Cyc_Absyn_complex_type(_T37);return _T38;
_TL10A: _T39=Cyc_Absyn_int_type(sgn,sz);return _T39;_TL101:{enum Cyc_Absyn_Size_of _T5C;enum Cyc_Absyn_Sign _T5D;_T3A=t;_T3B=(int*)_T3A;_T3C=*_T3B;if(_T3C!=0)goto _TL10C;_T3D=t;_T3E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3D;_T3F=_T3E->f1;_T40=(int*)_T3F;_T41=*_T40;switch(_T41){case 1: _T42=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T42;_T43=_T5E->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T5F=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T43;_T5D=_T5F->f1;_T5C=_T5F->f2;}}{enum Cyc_Absyn_Sign sgn2=_T5D;enum Cyc_Absyn_Size_of sz2=_T5C;_T44=seen_sign;
# 700
if(!_T44)goto _TL10F;_T45=sgn2;_T46=(int)_T45;_T47=sgn;_T48=(int)_T47;if(_T46==_T48)goto _TL10F;
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);goto _TL110;_TL10F: _TL110: _T49=seen_size;
# 704
if(!_T49)goto _TL111;_T4A=sz2;_T4B=(int)_T4A;_T4C=sz;_T4D=(int)_T4C;if(_T4B==_T4D)goto _TL111;
t=Cyc_Absyn_int_type(sgn2,sz);goto _TL112;_TL111: _TL112: _T4E=ts;_T4F=_T4E.Complex_spec;
if(!_T4F)goto _TL113;
t=Cyc_Absyn_complex_type(t);goto _TL114;_TL113: _TL114: goto _LL0;}case 2: _T50=seen_size;
# 710
if(!_T50)goto _TL115;
t=Cyc_Absyn_long_double_type;goto _TL116;_TL115: _TL116: _T51=ts;_T52=_T51.Complex_spec;
if(!_T52)goto _TL117;
t=Cyc_Absyn_complex_type(t);goto _TL118;_TL117: _TL118: goto _LL0;default: goto _LL5;}goto _TL10D;_TL10C: _LL5: _T53=seen_sign;
# 716
if(!_T53)goto _TL119;_T54=loc;_T55=
_tag_fat("sign specification on non-integral type",sizeof(char),40U);_T56=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T54,_T55,_T56);goto _TL11A;_TL119: _TL11A: _T57=seen_size;
if(!_T57)goto _TL11B;_T58=loc;_T59=
_tag_fat("size qualifier on non-integral type",sizeof(char),36U);_T5A=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T58,_T59,_T5A);goto _TL11C;_TL11B: _TL11C: goto _LL0;_TL10D: _LL0:;}_T5B=t;
# 722
return _T5B;}}}}}
# 725
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){struct _tuple13*_T0;struct Cyc_Parse_Declarator _T1;struct Cyc_Parse_Declarator _T2;struct Cyc_Absyn_Tqual _T3;void*_T4;struct Cyc_List_List*_T5;struct Cyc_Parse_Declarator _T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct _tuple13*_T9;struct _tuple13*_TA;struct _RegionHandle*_TB;struct Cyc_Absyn_Tqual _TC;void*_TD;struct _tuple13*_TE;struct _tuple13*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct _RegionHandle*_T12;struct _tuple15*_T13;struct _RegionHandle*_T14;
# 729
if(ds!=0)goto _TL11D;return 0;_TL11D: _T0=ds;{
struct Cyc_Parse_Declarator d=_T0->hd;_T1=d;{
struct _tuple0*q=_T1.id;_T2=d;{
unsigned varloc=_T2.varloc;_T3=tq;_T4=t;_T5=shared_atts;_T6=d;_T7=_T6.tms;{
struct _tuple14 _T15=Cyc_Parse_apply_tms(_T3,_T4,_T5,_T7);struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;void*_T18;struct Cyc_Absyn_Tqual _T19;_T19=_T15.f0;_T18=_T15.f1;_T17=_T15.f2;_T16=_T15.f3;{struct Cyc_Absyn_Tqual tq2=_T19;void*new_typ=_T18;struct Cyc_List_List*tvs=_T17;struct Cyc_List_List*atts=_T16;_T9=ds;_TA=_T9->tl;
# 736
if(_TA!=0)goto _TL11F;_T8=0;goto _TL120;_TL11F: _TB=r;_TC=tq;_TD=Cyc_Tcutil_copy_type(t);_TE=ds;_TF=_TE->tl;_T10=shared_atts;_T8=Cyc_Parse_apply_tmss(_TB,_TC,_TD,_TF,_T10);_TL120: {struct Cyc_List_List*tl=_T8;_T12=r;{struct Cyc_List_List*_T1A=_region_malloc(_T12,0U,sizeof(struct Cyc_List_List));_T14=r;{struct _tuple15*_T1B=_region_malloc(_T14,0U,sizeof(struct _tuple15));
_T1B->f0=varloc;_T1B->f1=q;_T1B->f2=tq2;_T1B->f3=new_typ;_T1B->f4=tvs;_T1B->f5=atts;_T13=(struct _tuple15*)_T1B;}_T1A->hd=_T13;_T1A->tl=tl;_T11=(struct Cyc_List_List*)_T1A;}return _T11;}}}}}}}
# 740
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){struct _tuple14 _T0;struct Cyc_List_List*_T1;int*_T2;unsigned _T3;void*_T4;struct Cyc_Absyn_Tqual _T5;void*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct _tuple14 _TA;void*_TB;struct Cyc_Absyn_Tqual _TC;void*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct _tuple14 _T11;void*_T12;void*_T13;int*_T14;int _T15;void*_T16;void*_T17;struct Cyc_List_List*_T18;void*_T19;int _T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;int*_T24;int _T25;struct Cyc_List_List*_T26;int _T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;void*_T2B;struct _tuple8*_T2C;struct _tuple8 _T2D;struct _fat_ptr*_T2E;struct Cyc_List_List*_T2F;void*_T30;struct _tuple8*_T31;struct _tuple8 _T32;void*_T33;void*_T34;struct Cyc_List_List*_T35;void*_T36;void**_T37;void**_T38;int*_T39;int _T3A;struct Cyc_Absyn_ArrayInfo _T3B;struct Cyc_Absyn_ArrayInfo _T3C;struct Cyc_Absyn_ArrayInfo _T3D;struct Cyc_Absyn_ArrayInfo _T3E;struct Cyc_Absyn_ArrayInfo _T3F;struct _tuple0*_T40;struct _tuple0*_T41;struct _tuple0*_T42;struct Cyc_List_List*_T43;struct _fat_ptr _T44;struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;void*_T48;struct Cyc_List_List*_T49;struct Cyc_Absyn_Exp*_T4A;struct Cyc_List_List*_T4B;void*_T4C;struct Cyc_Absyn_Exp*_T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;void*_T50;void**_T51;void**_T52;struct Cyc_List_List*_T53;void**_T54;void*_T55;void**_T56;void**_T57;void*_T58;struct Cyc_List_List*_T59;struct Cyc_Absyn_Tqual _T5A;unsigned _T5B;struct Cyc_Absyn_Tqual _T5C;void*_T5D;struct Cyc_List_List*_T5E;struct Cyc_List_List*_T5F;struct Cyc_List_List*_T60;struct _tuple14 _T61;void*_T62;int(*_T63)(unsigned,struct _fat_ptr);unsigned _T64;struct _fat_ptr _T65;struct Cyc_List_List*_T66;struct Cyc_List_List*_T67;struct _tuple14 _T68;int(*_T69)(unsigned,struct _fat_ptr);unsigned _T6A;struct _fat_ptr _T6B;struct Cyc_Absyn_Tqual _T6C;struct Cyc_Absyn_PtrInfo _T6D;void*_T6E;struct Cyc_List_List*_T6F;struct Cyc_List_List*_T70;struct Cyc_List_List*_T71;struct _tuple14 _T72;struct Cyc_Absyn_Tqual _T73;void*_T74;struct Cyc_List_List*_T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;struct _tuple14 _T78;
# 743
if(tms!=0)goto _TL121;{struct _tuple14 _T79;_T79.f0=tq;_T79.f1=t;_T79.f2=0;_T79.f3=atts;_T0=_T79;}return _T0;_TL121: _T1=tms;{
void*_T79=_T1->hd;struct Cyc_Absyn_Tqual _T7A;struct Cyc_Absyn_PtrAtts _T7B;struct Cyc_List_List*_T7C;void*_T7D;unsigned _T7E;void*_T7F;_T2=(int*)_T79;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T79;_T4=_T80->f1;_T7F=(void*)_T4;_T7E=_T80->f2;}{void*zeroterm=_T7F;unsigned ztloc=_T7E;_T5=
# 746
Cyc_Absyn_empty_tqual(0U);_T6=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);_T7=atts;_T8=tms;_T9=_T8->tl;_TA=
# 746
Cyc_Parse_apply_tms(_T5,_T6,_T7,_T9);return _TA;}case 1:{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T79;_T7F=_T80->f1;_TB=_T80->f2;_T7D=(void*)_TB;_T7E=_T80->f3;}{struct Cyc_Absyn_Exp*e=_T7F;void*zeroterm=_T7D;unsigned ztloc=_T7E;_TC=
# 749
Cyc_Absyn_empty_tqual(0U);_TD=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);_TE=atts;_TF=tms;_T10=_TF->tl;_T11=
# 749
Cyc_Parse_apply_tms(_TC,_TD,_TE,_T10);return _T11;}case 3:{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T79;_T12=_T80->f1;_T7F=(void*)_T12;}{void*args=_T7F;unsigned _T80;struct Cyc_Absyn_Exp*_T81;struct Cyc_Absyn_Exp*_T82;struct Cyc_Absyn_Exp*_T83;struct Cyc_Absyn_Exp*_T84;struct Cyc_List_List*_T85;struct Cyc_List_List*_T86;void*_T87;struct Cyc_Absyn_VarargInfo*_T88;int _T89;struct Cyc_List_List*_T8A;_T13=args;_T14=(int*)_T13;_T15=*_T14;if(_T15!=1)goto _TL124;_T16=args;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T8B=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T16;_T8A=_T8B->f1;_T89=_T8B->f2;_T88=_T8B->f3;_T17=_T8B->f4;_T87=(void*)_T17;_T86=_T8B->f5;_T85=_T8B->f6;_T84=_T8B->f7;_T83=_T8B->f8;_T82=_T8B->f9;_T81=_T8B->f10;}{struct Cyc_List_List*args2=_T8A;int c_vararg=_T89;struct Cyc_Absyn_VarargInfo*cyc_vararg=_T88;void*eff=_T87;struct Cyc_List_List*effc=_T86;struct Cyc_List_List*qb=_T85;struct Cyc_Absyn_Exp*chks=_T84;struct Cyc_Absyn_Exp*req=_T83;struct Cyc_Absyn_Exp*ens=_T82;struct Cyc_Absyn_Exp*thrw=_T81;
# 754
struct Cyc_List_List*typvars=0;
# 756
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{
struct Cyc_List_List*as=atts;_TL129: if(as!=0)goto _TL127;else{goto _TL128;}
_TL127: _T18=as;_T19=_T18->hd;_T1A=Cyc_Atts_fntype_att(_T19);if(!_T1A)goto _TL12A;{struct Cyc_List_List*_T8B=_cycalloc(sizeof(struct Cyc_List_List));_T1C=as;
_T8B->hd=_T1C->hd;_T8B->tl=fn_atts;_T1B=(struct Cyc_List_List*)_T8B;}fn_atts=_T1B;goto _TL12B;
# 761
_TL12A:{struct Cyc_List_List*_T8B=_cycalloc(sizeof(struct Cyc_List_List));_T1E=as;_T8B->hd=_T1E->hd;_T8B->tl=new_atts;_T1D=(struct Cyc_List_List*)_T8B;}new_atts=_T1D;_TL12B: _T1F=as;
# 757
as=_T1F->tl;goto _TL129;_TL128:;}_T20=tms;_T21=_T20->tl;
# 763
if(_T21==0)goto _TL12C;_T22=tms;_T23=_T22->tl;{
void*_T8B=_T23->hd;struct Cyc_List_List*_T8C;_T24=(int*)_T8B;_T25=*_T24;if(_T25!=4)goto _TL12E;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T8D=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T8B;_T8C=_T8D->f1;}{struct Cyc_List_List*ts=_T8C;
# 766
typvars=ts;_T26=tms;
tms=_T26->tl;goto _LL12;}_TL12E: goto _LL12;_LL12:;}goto _TL12D;_TL12C: _TL12D: _T27=c_vararg;
# 772
if(_T27)goto _TL130;else{goto _TL132;}_TL132: if(cyc_vararg!=0)goto _TL130;if(args2==0)goto _TL130;_T28=args2;_T29=_T28->tl;if(_T29!=0)goto _TL130;_T2A=args2;_T2B=_T2A->hd;_T2C=(struct _tuple8*)_T2B;_T2D=*_T2C;_T2E=_T2D.f0;if(_T2E!=0)goto _TL130;_T2F=args2;_T30=_T2F->hd;_T31=(struct _tuple8*)_T30;_T32=*_T31;_T33=_T32.f2;_T34=Cyc_Absyn_void_type;if(_T33!=_T34)goto _TL130;
# 777
args2=0;goto _TL131;_TL130: _TL131: {
# 782
struct Cyc_List_List*new_requires=0;{
struct Cyc_List_List*a=args2;_TL136: if(a!=0)goto _TL134;else{goto _TL135;}
_TL134: _T35=a;_T36=_T35->hd;{struct _tuple8*_T8B=(struct _tuple8*)_T36;void*_T8C;struct Cyc_Absyn_Tqual _T8D;struct _fat_ptr*_T8E;{struct _tuple8 _T8F=*_T8B;_T8E=_T8F.f0;_T8D=_T8F.f1;_T37=& _T8B->f2;_T8C=(void**)_T37;}{struct _fat_ptr*vopt=_T8E;struct Cyc_Absyn_Tqual tq=_T8D;void**t=(void**)_T8C;_T38=t;{
void*_T8F=*_T38;unsigned _T90;void*_T91;struct Cyc_Absyn_Exp*_T92;struct Cyc_Absyn_Tqual _T93;void*_T94;_T39=(int*)_T8F;_T3A=*_T39;if(_T3A!=5)goto _TL137;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T95=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T8F;_T3B=_T95->f1;_T94=_T3B.elt_type;_T3C=_T95->f1;_T93=_T3C.tq;_T3D=_T95->f1;_T92=_T3D.num_elts;_T3E=_T95->f1;_T91=_T3E.zero_term;_T3F=_T95->f1;_T90=_T3F.zt_loc;}{void*et=_T94;struct Cyc_Absyn_Tqual tq=_T93;struct Cyc_Absyn_Exp*neltsopt=_T92;void*zt=_T91;unsigned ztloc=_T90;
# 787
if(neltsopt==0)goto _TL139;if(vopt==0)goto _TL139;{
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0));_T40=v;_T40->f0.Loc_n.tag=4U;_T41=v;_T41->f0.Loc_n.val=0;_T42=v;_T42->f1=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);{struct Cyc_Absyn_Exp*_T95[1];_T95[0]=
Cyc_Absyn_var_exp(v,0U);_T44=_tag_fat(_T95,sizeof(struct Cyc_Absyn_Exp*),1);_T43=Cyc_List_list(_T44);}{struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,_T43,0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);{struct Cyc_List_List*_T95=_cycalloc(sizeof(struct Cyc_List_List));
_T95->hd=new_req;_T95->tl=new_requires;_T45=(struct Cyc_List_List*)_T95;}new_requires=_T45;}}}goto _TL13A;_TL139: _TL13A: goto _LL1A;}_TL137: goto _LL1A;_LL1A:;}}}_T46=a;
# 783
a=_T46->tl;goto _TL136;_TL135:;}
# 798
if(new_requires==0)goto _TL13B;{
struct Cyc_Absyn_Exp*r;
if(req==0)goto _TL13D;
r=req;goto _TL13E;
# 803
_TL13D: _T47=new_requires;_T48=_T47->hd;r=(struct Cyc_Absyn_Exp*)_T48;_T49=new_requires;
new_requires=_T49->tl;_TL13E:
# 806
 _TL142: if(new_requires!=0)goto _TL140;else{goto _TL141;}
_TL140: _T4A=r;_T4B=new_requires;_T4C=_T4B->hd;_T4D=(struct Cyc_Absyn_Exp*)_T4C;r=Cyc_Absyn_and_exp(_T4A,_T4D,0U);_T4E=new_requires;
# 806
new_requires=_T4E->tl;goto _TL142;_TL141:
# 808
 req=r;}goto _TL13C;_TL13B: _TL13C: {
# 812
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 814
if(tags==0)goto _TL143;
t=Cyc_Parse_substitute_tags(tags,t);goto _TL144;_TL143: _TL144:
 t=Cyc_Parse_array2ptr(t,0);{
# 819
struct Cyc_List_List*a=args2;_TL148: if(a!=0)goto _TL146;else{goto _TL147;}
_TL146: _T4F=a;_T50=_T4F->hd;{struct _tuple8*_T8B=(struct _tuple8*)_T50;void*_T8C;struct Cyc_Absyn_Tqual _T8D;struct _fat_ptr*_T8E;{struct _tuple8 _T8F=*_T8B;_T8E=_T8F.f0;_T8D=_T8F.f1;_T51=& _T8B->f2;_T8C=(void**)_T51;}{struct _fat_ptr*vopt=_T8E;struct Cyc_Absyn_Tqual tq=_T8D;void**t=(void**)_T8C;
if(tags==0)goto _TL149;_T52=t;_T53=tags;_T54=t;_T55=*_T54;
*_T52=Cyc_Parse_substitute_tags(_T53,_T55);goto _TL14A;_TL149: _TL14A: _T56=t;_T57=t;_T58=*_T57;
*_T56=Cyc_Parse_array2ptr(_T58,1);}}_T59=a;
# 819
a=_T59->tl;goto _TL148;_TL147:;}_T5A=tq;_T5B=_T5A.loc;_T5C=
# 831
Cyc_Absyn_empty_tqual(_T5B);_T5D=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,effc,qb,fn_atts,chks,req,ens,thrw);_T5E=new_atts;_T5F=
# 836
_check_null(tms);_T60=_T5F->tl;_T61=
# 831
Cyc_Parse_apply_tms(_T5C,_T5D,_T5E,_T60);return _T61;}}}_TL124: _T62=args;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T8B=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T62;_T80=_T8B->f2;}{unsigned loc=_T80;{
# 838
int(*_T8B)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T63=_T8B;}_T64=loc;_T65=_tag_fat("function declaration without parameter types",sizeof(char),45U);_T63(_T64,_T65);};}case 4:{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T79;_T7C=_T80->f1;_T7E=_T80->f2;}{struct Cyc_List_List*ts=_T7C;unsigned loc=_T7E;_T66=tms;_T67=_T66->tl;
# 845
if(_T67!=0)goto _TL14B;{struct _tuple14 _T80;
_T80.f0=tq;_T80.f1=t;_T80.f2=ts;_T80.f3=atts;_T68=_T80;}return _T68;_TL14B:{
# 850
int(*_T80)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T69=_T80;}_T6A=loc;_T6B=
_tag_fat("type parameters must appear before function arguments in declarator",sizeof(char),68U);
# 850
_T69(_T6A,_T6B);}case 2:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_T79;_T7B=_T80->f1;_T7A=_T80->f2;}{struct Cyc_Absyn_PtrAtts ptratts=_T7B;struct Cyc_Absyn_Tqual tq2=_T7A;_T6C=tq2;{struct Cyc_Absyn_PtrInfo _T80;
# 853
_T80.elt_type=t;_T80.elt_tq=tq;_T80.ptr_atts=ptratts;_T6D=_T80;}_T6E=Cyc_Absyn_pointer_type(_T6D);_T6F=atts;_T70=tms;_T71=_T70->tl;_T72=Cyc_Parse_apply_tms(_T6C,_T6E,_T6F,_T71);return _T72;}default:{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T79;_T7E=_T80->f1;_T7C=_T80->f2;}{unsigned loc=_T7E;struct Cyc_List_List*atts2=_T7C;_T73=tq;_T74=t;_T75=
# 858
Cyc_List_append(atts,atts2);_T76=tms;_T77=_T76->tl;_T78=Cyc_Parse_apply_tms(_T73,_T74,_T75,_T77);return _T78;}};}}
# 864
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){void*_T0;_T0=
Cyc_Parse_collapse_type_specifiers(tss,loc);return _T0;}
# 873
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple15*t){struct _tuple15*_T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T7;struct Cyc_Absyn_Typedefdecl*_T8;void*_T9;unsigned _TA;struct Cyc_Absyn_Decl*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;void*_TE;struct Cyc_Absyn_Tqual _TF;struct _tuple0*_T10;unsigned _T11;_T0=t;{struct _tuple15 _T12=*_T0;_T11=_T12.f0;_T10=_T12.f1;_TF=_T12.f2;_TE=_T12.f3;_TD=_T12.f4;_TC=_T12.f5;}{unsigned varloc=_T11;struct _tuple0*x=_T10;struct Cyc_Absyn_Tqual tq=_TF;void*typ=_TE;struct Cyc_List_List*tvs=_TD;struct Cyc_List_List*atts=_TC;
# 876
Cyc_Lex_register_typedef(x);{
# 878
struct Cyc_Core_Opt*kind;
void*type;{struct Cyc_Core_Opt*_T12;_T1=typ;_T2=(int*)_T1;_T3=*_T2;if(_T3!=1)goto _TL14D;_T4=typ;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T13=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T4;_T12=_T13->f1;}{struct Cyc_Core_Opt*kopt=_T12;
# 882
type=0;
if(kopt!=0)goto _TL14F;_T6=& Cyc_Kinds_bko;_T5=(struct Cyc_Core_Opt*)_T6;goto _TL150;_TL14F: _T5=kopt;_TL150: kind=_T5;goto _LL3;}_TL14D:
# 885
 kind=0;type=typ;goto _LL3;_LL3:;}{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T12=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_T12->tag=8;{struct Cyc_Absyn_Typedefdecl*_T13=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));
# 887
_T13->name=x;_T13->tvs=tvs;_T13->kind=kind;
_T13->defn=type;_T13->atts=atts;
_T13->tq=tq;_T13->extern_c=0;_T8=(struct Cyc_Absyn_Typedefdecl*)_T13;}
# 887
_T12->f1=_T8;_T7=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T12;}_T9=(void*)_T7;_TA=loc;_TB=Cyc_Absyn_new_decl(_T9,_TA);return _TB;}}}
# 894
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;unsigned _T3;struct Cyc_Absyn_Stmt*_T4;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T5=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_T5->tag=12;
_T5->f1=d;_T5->f2=s;_T0=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T5;}_T1=(void*)_T0;_T2=d;_T3=_T2->loc;_T4=Cyc_Absyn_new_stmt(_T1,_T3);return _T4;}
# 898
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*(*_T0)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*);void*(*_T1)(void*(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_List_List*_T2;struct Cyc_Absyn_Stmt*_T3;struct Cyc_Absyn_Stmt*_T4;_T1=Cyc_List_fold_right;{
struct Cyc_Absyn_Stmt*(*_T5)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T5;}_T2=ds;_T3=s;_T4=_T0(Cyc_Parse_flatten_decl,_T2,_T3);return _T4;}
# 902
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es,struct Cyc_List_List**rs){struct _tuple11*_T0;struct _tuple13*_T1;struct _RegionHandle*_T2;struct Cyc_List_List*_T3;struct _RegionHandle*_T4;struct Cyc_List_List*_T5;struct _RegionHandle*_T6;struct _tuple11*_T7;struct Cyc_List_List**_T8;struct Cyc_List_List**_T9;struct _tuple13**_TA;struct _tuple13*(*_TB)(struct _tuple13*);
# 906
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
struct Cyc_List_List*renames=0;
_TL154: if(ds!=0)goto _TL152;else{goto _TL153;}
_TL152: _T0=ds;{struct _tuple12 _TC=_T0->hd;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_Parse_Declarator _TF;_TF=_TC.f0;_TE=_TC.f1;_TD=_TC.f2;{struct Cyc_Parse_Declarator d=_TF;struct Cyc_Absyn_Exp*e=_TE;struct Cyc_Absyn_Exp*rename=_TD;_T2=r;{struct _tuple13*_T10=_region_malloc(_T2,0U,sizeof(struct _tuple13));
_T10->tl=declarators;_T10->hd=d;_T1=(struct _tuple13*)_T10;}declarators=_T1;_T4=r;{struct Cyc_List_List*_T10=_region_malloc(_T4,0U,sizeof(struct Cyc_List_List));
_T10->hd=e;_T10->tl=exprs;_T3=(struct Cyc_List_List*)_T10;}exprs=_T3;_T6=r;{struct Cyc_List_List*_T10=_region_malloc(_T6,0U,sizeof(struct Cyc_List_List));
_T10->hd=rename;_T10->tl=renames;_T5=(struct Cyc_List_List*)_T10;}renames=_T5;}}_T7=ds;
# 909
ds=_T7->tl;goto _TL154;_TL153: _T8=es;
# 915
*_T8=Cyc_List_imp_rev(exprs);_T9=rs;
*_T9=Cyc_List_imp_rev(renames);_TA=decls;{
struct _tuple13*(*_TC)(struct _tuple13*)=(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;_TB=_TC;}*_TA=_TB(declarators);}
# 925
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){struct Cyc_Parse_Declaration_spec _T0;struct Cyc_Parse_Declaration_spec _T1;struct Cyc_Parse_Declaration_spec _T2;struct Cyc_Absyn_Tqual _T3;unsigned _T4;struct Cyc_Parse_Declaration_spec _T5;int _T6;unsigned _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_Parse_Declaration_spec _TA;int _TB;enum Cyc_Absyn_Scope _TC;int _TD;struct _RegionHandle*_TE;struct _tuple11*_TF;struct _tuple13**_T10;struct Cyc_List_List**_T11;struct Cyc_List_List**_T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_Absyn_Exp*_T15;struct Cyc_List_List*_T16;void*_T17;int*_T18;unsigned _T19;void*_T1A;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1B;struct Cyc_Absyn_TypeDecl*_T1C;struct Cyc_Absyn_TypeDecl*_T1D;void*_T1E;int*_T1F;unsigned _T20;void*_T21;struct Cyc_Absyn_TypeDecl*_T22;void*_T23;struct Cyc_Absyn_Aggrdecl*_T24;struct Cyc_Absyn_Aggrdecl*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;struct Cyc_Absyn_Aggrdecl*_T28;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T29;void*_T2A;unsigned _T2B;void*_T2C;struct Cyc_Absyn_TypeDecl*_T2D;void*_T2E;unsigned _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_Absyn_Enumdecl*_T32;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T33;void*_T34;unsigned _T35;void*_T36;struct Cyc_Absyn_TypeDecl*_T37;void*_T38;unsigned _T39;struct _fat_ptr _T3A;struct _fat_ptr _T3B;struct Cyc_Absyn_Datatypedecl*_T3C;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T3D;void*_T3E;unsigned _T3F;void*_T40;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T41;void*_T42;int*_T43;unsigned _T44;void*_T45;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T46;void*_T47;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T48;union Cyc_Absyn_AggrInfo _T49;struct _union_AggrInfo_UnknownAggr _T4A;unsigned _T4B;void*_T4C;void*_T4D;union Cyc_Absyn_AggrInfo _T4E;struct _union_AggrInfo_UnknownAggr _T4F;struct _tuple2 _T50;union Cyc_Absyn_AggrInfo _T51;struct _union_AggrInfo_UnknownAggr _T52;struct _tuple2 _T53;struct Cyc_List_List*(*_T54)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T55)(void*(*)(void*,void*),void*,struct Cyc_List_List*);unsigned _T56;struct Cyc_List_List*_T57;struct Cyc_Absyn_Aggrdecl*_T58;struct Cyc_Absyn_Aggrdecl*_T59;struct Cyc_Absyn_Aggrdecl*_T5A;struct Cyc_Absyn_Aggrdecl*_T5B;struct Cyc_Absyn_Aggrdecl*_T5C;struct Cyc_Absyn_Aggrdecl*_T5D;struct Cyc_Absyn_Aggrdecl*_T5E;unsigned _T5F;struct _fat_ptr _T60;struct _fat_ptr _T61;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T62;void*_T63;unsigned _T64;void*_T65;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T66;void*_T67;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T68;union Cyc_Absyn_DatatypeInfo _T69;struct _union_DatatypeInfo_KnownDatatype _T6A;unsigned _T6B;void*_T6C;void*_T6D;union Cyc_Absyn_DatatypeInfo _T6E;struct _union_DatatypeInfo_KnownDatatype _T6F;unsigned _T70;struct _fat_ptr _T71;struct _fat_ptr _T72;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T73;struct Cyc_Absyn_Datatypedecl**_T74;void*_T75;unsigned _T76;void*_T77;void*_T78;union Cyc_Absyn_DatatypeInfo _T79;struct _union_DatatypeInfo_UnknownDatatype _T7A;struct Cyc_Absyn_UnknownDatatypeInfo _T7B;union Cyc_Absyn_DatatypeInfo _T7C;struct _union_DatatypeInfo_UnknownDatatype _T7D;struct Cyc_Absyn_UnknownDatatypeInfo _T7E;struct Cyc_List_List*(*_T7F)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T80)(void*(*)(void*,void*),void*,struct Cyc_List_List*);unsigned _T81;struct Cyc_List_List*_T82;unsigned _T83;struct _fat_ptr _T84;struct _fat_ptr _T85;void*_T86;void*_T87;struct Cyc_Absyn_Enumdecl*_T88;struct Cyc_Absyn_Enumdecl*_T89;struct Cyc_Absyn_Enumdecl*_T8A;unsigned _T8B;struct _fat_ptr _T8C;struct _fat_ptr _T8D;struct Cyc_Absyn_Decl*_T8E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T8F;void*_T90;void*_T91;struct Cyc_Absyn_Enumdecl*_T92;struct Cyc_Absyn_Enumdecl*_T93;struct Cyc_Absyn_Enumdecl*_T94;struct Cyc_Core_Opt*_T95;unsigned _T96;struct _fat_ptr _T97;struct _fat_ptr _T98;struct Cyc_Absyn_Decl*_T99;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T9A;unsigned _T9B;struct _fat_ptr _T9C;struct _fat_ptr _T9D;int _T9E;int _T9F;unsigned _TA0;struct _fat_ptr _TA1;struct _fat_ptr _TA2;struct Cyc_List_List*(*_TA3)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_TA4)(void*(*)(void*,void*),void*,struct Cyc_List_List*);unsigned _TA5;struct Cyc_List_List*_TA6;struct Cyc_List_List*_TA7;void*_TA8;unsigned _TA9;struct _fat_ptr _TAA;struct _fat_ptr _TAB;int(*_TAC)(unsigned,struct _fat_ptr);unsigned _TAD;struct _fat_ptr _TAE;int(*_TAF)(unsigned,struct _fat_ptr);unsigned _TB0;struct _fat_ptr _TB1;unsigned _TB2;struct _tuple0*_TB3;void*_TB4;struct Cyc_List_List*_TB5;void*_TB6;struct Cyc_Absyn_Exp*_TB7;struct Cyc_List_List*_TB8;void*_TB9;struct Cyc_Absyn_Exp*_TBA;struct Cyc_Absyn_Vardecl*_TBB;struct Cyc_Absyn_Vardecl*_TBC;struct Cyc_Absyn_Vardecl*_TBD;struct Cyc_List_List*_TBE;struct Cyc_Absyn_Decl*_TBF;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TC0;struct Cyc_List_List*_TC1;struct Cyc_List_List*_TC2;struct Cyc_List_List*_TC3;struct _RegionHandle _TC4=_new_region(0U,"mkrgn");struct _RegionHandle*mkrgn=& _TC4;_push_region(mkrgn);{struct Cyc_List_List*_TC5;struct Cyc_Parse_Type_specifier _TC6;struct Cyc_Absyn_Tqual _TC7;_T0=ds;_TC7=_T0.tq;_T1=ds;_TC6=_T1.type_specs;_T2=ds;_TC5=_T2.attributes;{struct Cyc_Absyn_Tqual tq=_TC7;struct Cyc_Parse_Type_specifier tss=_TC6;struct Cyc_List_List*atts=_TC5;_T3=tq;_T4=_T3.loc;
# 930
if(_T4!=0U)goto _TL155;tq.loc=tqual_loc;goto _TL156;_TL155: _TL156: _T5=ds;_T6=_T5.is_inline;
if(!_T6)goto _TL157;_T7=loc;_T8=
_tag_fat("inline qualifier on non-function definition",sizeof(char),44U);_T9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T7,_T8,_T9);goto _TL158;_TL157: _TL158: {
# 934
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;_TA=ds;{
enum Cyc_Parse_Storage_class _TC8=_TA.sc;_TB=(int)_TC8;switch(_TB){case Cyc_Parse_Typedef_sc:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _TD=Cyc_Flags_no_register;
if(!_TD)goto _TL15A;_TC=2U;goto _TL15B;_TL15A: _TC=5U;_TL15B: s=_TC;goto _LL3;case Cyc_Parse_Abstract_sc:
 s=1U;goto _LL3;default: goto _LL3;}_LL3:;}{
# 950
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
struct Cyc_List_List*renames=0;_TE=mkrgn;_TF=ids;_T10=& declarators;_T11=& exprs;_T12=& renames;
Cyc_Parse_decl_split(_TE,_TF,_T10,_T11,_T12);{
# 955
int exps_empty=1;{
struct Cyc_List_List*es=exprs;_TL15F: if(es!=0)goto _TL15D;else{goto _TL15E;}
_TL15D: _T13=es;_T14=_T13->hd;_T15=(struct Cyc_Absyn_Exp*)_T14;if(_T15==0)goto _TL160;
exps_empty=0;goto _TL15E;_TL160: _T16=es;
# 956
es=_T16->tl;goto _TL15F;_TL15E:;}{
# 963
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators!=0)goto _TL162;{int _TC8;struct Cyc_Absyn_Datatypedecl**_TC9;struct Cyc_List_List*_TCA;struct _tuple0*_TCB;enum Cyc_Absyn_AggrKind _TCC;struct Cyc_Absyn_Datatypedecl*_TCD;struct Cyc_Absyn_Enumdecl*_TCE;struct Cyc_Absyn_Aggrdecl*_TCF;_T17=base_type;_T18=(int*)_T17;_T19=*_T18;switch(_T19){case 10: _T1A=base_type;_T1B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1A;_T1C=_T1B->f1;_T1D=(struct Cyc_Absyn_TypeDecl*)_T1C;_T1E=_T1D->r;_T1F=(int*)_T1E;_T20=*_T1F;switch(_T20){case 0: _T21=base_type;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T21;_T22=_TD0->f1;{struct Cyc_Absyn_TypeDecl _TD1=*_T22;_T23=_TD1.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_TD2=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T23;_TCF=_TD2->f1;}}}{struct Cyc_Absyn_Aggrdecl*ad=_TCF;_T24=ad;_T25=ad;_T26=_T25->attributes;_T27=atts;
# 969
_T24->attributes=Cyc_List_append(_T26,_T27);_T28=ad;
_T28->sc=s;{struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TD1->tag=5;
_TD1->f1=ad;_T29=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_TD1;}_T2A=(void*)_T29;_T2B=loc;_TD0->hd=Cyc_Absyn_new_decl(_T2A,_T2B);_TD0->tl=0;_npop_handler(0);return _TD0;}}case 1: _T2C=base_type;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T2C;_T2D=_TD0->f1;{struct Cyc_Absyn_TypeDecl _TD1=*_T2D;_T2E=_TD1.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_TD2=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T2E;_TCE=_TD2->f1;}}}{struct Cyc_Absyn_Enumdecl*ed=_TCE;
# 973
if(atts==0)goto _TL166;_T2F=loc;_T30=_tag_fat("attributes on enum not supported",sizeof(char),33U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2F,_T30,_T31);goto _TL167;_TL166: _TL167: _T32=ed;
_T32->sc=s;{struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TD1->tag=7;
_TD1->f1=ed;_T33=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TD1;}_T34=(void*)_T33;_T35=loc;_TD0->hd=Cyc_Absyn_new_decl(_T34,_T35);_TD0->tl=0;_npop_handler(0);return _TD0;}}default: _T36=base_type;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T36;_T37=_TD0->f1;{struct Cyc_Absyn_TypeDecl _TD1=*_T37;_T38=_TD1.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_TD2=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T38;_TCD=_TD2->f1;}}}{struct Cyc_Absyn_Datatypedecl*dd=_TCD;
# 977
if(atts==0)goto _TL168;_T39=loc;_T3A=_tag_fat("attributes on datatypes not supported",sizeof(char),38U);_T3B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T39,_T3A,_T3B);goto _TL169;_TL168: _TL169: _T3C=dd;
_T3C->sc=s;{struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TD1->tag=6;
_TD1->f1=dd;_T3D=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TD1;}_T3E=(void*)_T3D;_T3F=loc;_TD0->hd=Cyc_Absyn_new_decl(_T3E,_T3F);_TD0->tl=0;_npop_handler(0);return _TD0;}}};case 0: _T40=base_type;_T41=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T40;_T42=_T41->f1;_T43=(int*)_T42;_T44=*_T43;switch(_T44){case 24: _T45=base_type;_T46=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T45;_T47=_T46->f1;_T48=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T47;_T49=_T48->f1;_T4A=_T49.UnknownAggr;_T4B=_T4A.tag;if(_T4B!=1)goto _TL16B;_T4C=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4C;_T4D=_TD0->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4D;_T4E=_TD1->f1;_T4F=_T4E.UnknownAggr;_T50=_T4F.val;_TCC=_T50.f0;_T51=_TD1->f1;_T52=_T51.UnknownAggr;_T53=_T52.val;_TCB=_T53.f1;}_TCA=_TD0->f2;}{enum Cyc_Absyn_AggrKind k=_TCC;struct _tuple0*n=_TCB;struct Cyc_List_List*ts=_TCA;_T55=Cyc_List_map_c;{
# 981
struct Cyc_List_List*(*_TD0)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T55;_T54=_TD0;}_T56=loc;_T57=ts;{struct Cyc_List_List*ts2=_T54(Cyc_Parse_typ2tvar,_T56,_T57);
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T58=ad;_T58->kind=k;_T59=ad;_T59->sc=s;_T5A=ad;_T5A->name=n;_T5B=ad;_T5B->tvs=ts2;_T5C=ad;_T5C->impl=0;_T5D=ad;_T5D->attributes=0;_T5E=ad;_T5E->expected_mem_kind=0;
if(atts==0)goto _TL16D;_T5F=loc;_T60=_tag_fat("bad attributes on type declaration",sizeof(char),35U);_T61=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T5F,_T60,_T61);goto _TL16E;_TL16D: _TL16E: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TD1->tag=5;
_TD1->f1=ad;_T62=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_TD1;}_T63=(void*)_T62;_T64=loc;_TD0->hd=Cyc_Absyn_new_decl(_T63,_T64);_TD0->tl=0;_npop_handler(0);return _TD0;}}}_TL16B: goto _LL25;case 22: _T65=base_type;_T66=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T65;_T67=_T66->f1;_T68=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T67;_T69=_T68->f1;_T6A=_T69.KnownDatatype;_T6B=_T6A.tag;if(_T6B!=2)goto _TL16F;_T6C=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6C;_T6D=_TD0->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T6D;_T6E=_TD1->f1;_T6F=_T6E.KnownDatatype;_TC9=_T6F.val;}}{struct Cyc_Absyn_Datatypedecl**tudp=_TC9;
# 986
if(atts==0)goto _TL171;_T70=loc;_T71=_tag_fat("bad attributes on datatype",sizeof(char),27U);_T72=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T70,_T71,_T72);goto _TL172;_TL171: _TL172: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TD1->tag=6;_T74=tudp;
_TD1->f1=*_T74;_T73=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TD1;}_T75=(void*)_T73;_T76=loc;_TD0->hd=Cyc_Absyn_new_decl(_T75,_T76);_TD0->tl=0;_npop_handler(0);return _TD0;}}_TL16F: _T77=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T77;_T78=_TD0->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T78;_T79=_TD1->f1;_T7A=_T79.UnknownDatatype;_T7B=_T7A.val;_TCB=_T7B.name;_T7C=_TD1->f1;_T7D=_T7C.UnknownDatatype;_T7E=_T7D.val;_TC8=_T7E.is_extensible;}_TCA=_TD0->f2;}{struct _tuple0*n=_TCB;int isx=_TC8;struct Cyc_List_List*ts=_TCA;_T80=Cyc_List_map_c;{
# 989
struct Cyc_List_List*(*_TD0)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T80;_T7F=_TD0;}_T81=loc;_T82=ts;{struct Cyc_List_List*ts2=_T7F(Cyc_Parse_typ2tvar,_T81,_T82);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts==0)goto _TL173;_T83=loc;_T84=_tag_fat("bad attributes on datatype",sizeof(char),27U);_T85=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T83,_T84,_T85);goto _TL174;_TL173: _TL174: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));
_TD0->hd=tud;_TD0->tl=0;_npop_handler(0);return _TD0;}}}case 19: _T86=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T86;_T87=_TD0->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T87;_TCB=_TD1->f1;}}{struct _tuple0*n=_TCB;
# 994
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T88=ed;_T88->sc=s;_T89=ed;_T89->name=n;_T8A=ed;_T8A->fields=0;
if(atts==0)goto _TL175;_T8B=loc;_T8C=_tag_fat("bad attributes on enum",sizeof(char),23U);_T8D=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T8B,_T8C,_T8D);goto _TL176;_TL175: _TL176: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TD2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TD2->tag=7;
_TD2->f1=ed;_T8F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TD2;}_TD1->r=(void*)_T8F;_TD1->loc=loc;_T8E=(struct Cyc_Absyn_Decl*)_TD1;}_TD0->hd=_T8E;_TD0->tl=0;_npop_handler(0);return _TD0;}}case 20: _T90=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T90;_T91=_TD0->f1;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_T91;_TCA=_TD1->f1;}}{struct Cyc_List_List*fs=_TCA;
# 1000
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T92=ed;_T92->sc=s;_T93=ed;_T93->name=Cyc_Parse_gensym_enum();_T94=ed;{struct Cyc_Core_Opt*_TD0=_cycalloc(sizeof(struct Cyc_Core_Opt));_TD0->v=fs;_T95=(struct Cyc_Core_Opt*)_TD0;}_T94->fields=_T95;
if(atts==0)goto _TL177;_T96=loc;_T97=_tag_fat("bad attributes on enum",sizeof(char),23U);_T98=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T96,_T97,_T98);goto _TL178;_TL177: _TL178: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TD2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TD2->tag=7;
_TD2->f1=ed;_T9A=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TD2;}_TD1->r=(void*)_T9A;_TD1->loc=loc;_T99=(struct Cyc_Absyn_Decl*)_TD1;}_TD0->hd=_T99;_TD0->tl=0;_npop_handler(0);return _TD0;}}default: goto _LL25;};default: _LL25: _T9B=loc;_T9C=
_tag_fat("missing declarator",sizeof(char),19U);_T9D=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T9B,_T9C,_T9D);{struct Cyc_List_List*_TD0=0;_npop_handler(0);return _TD0;}};}goto _TL163;_TL162: _TL163: {
# 1007
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);_T9E=istypedef;
if(!_T9E)goto _TL179;_T9F=exps_empty;
# 1012
if(_T9F)goto _TL17B;else{goto _TL17D;}
_TL17D: _TA0=loc;_TA1=_tag_fat("initializer in typedef declaration",sizeof(char),35U);_TA2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TA0,_TA1,_TA2);goto _TL17C;_TL17B: _TL17C: _TA4=Cyc_List_map_c;{
struct Cyc_List_List*(*_TC8)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*))_TA4;_TA3=_TC8;}_TA5=loc;_TA6=fields;{struct Cyc_List_List*decls=_TA3(Cyc_Parse_v_typ_to_typedef,_TA5,_TA6);struct Cyc_List_List*_TC8=decls;_npop_handler(0);return _TC8;}_TL179: {
# 1018
struct Cyc_List_List*decls=0;{
struct Cyc_List_List*ds=fields;
_TL181:
# 1019
 if(ds!=0)goto _TL17F;else{goto _TL180;}
# 1022
_TL17F: _TA7=ds;_TA8=_TA7->hd;{struct _tuple15*_TC8=(struct _tuple15*)_TA8;struct Cyc_List_List*_TC9;struct Cyc_List_List*_TCA;void*_TCB;struct Cyc_Absyn_Tqual _TCC;struct _tuple0*_TCD;unsigned _TCE;{struct _tuple15 _TCF=*_TC8;_TCE=_TCF.f0;_TCD=_TCF.f1;_TCC=_TCF.f2;_TCB=_TCF.f3;_TCA=_TCF.f4;_TC9=_TCF.f5;}{unsigned varloc=_TCE;struct _tuple0*x=_TCD;struct Cyc_Absyn_Tqual tq2=_TCC;void*t2=_TCB;struct Cyc_List_List*tvs2=_TCA;struct Cyc_List_List*atts2=_TC9;
if(tvs2==0)goto _TL182;_TA9=loc;_TAA=
_tag_fat("bad type params, ignoring",sizeof(char),26U);_TAB=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TA9,_TAA,_TAB);goto _TL183;_TL182: _TL183:
 if(exprs!=0)goto _TL184;{
int(*_TCF)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TAC=_TCF;}_TAD=loc;_TAE=_tag_fat("unexpected NULL in parse!",sizeof(char),26U);_TAC(_TAD,_TAE);goto _TL185;_TL184: _TL185:
 if(renames!=0)goto _TL186;{
int(*_TCF)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TAF=_TCF;}_TB0=loc;_TB1=_tag_fat("unexpected NULL in parse!",sizeof(char),26U);_TAF(_TB0,_TB1);goto _TL187;_TL186: _TL187: _TB2=varloc;_TB3=x;_TB4=t2;_TB5=exprs;_TB6=_TB5->hd;_TB7=(struct Cyc_Absyn_Exp*)_TB6;_TB8=renames;_TB9=_TB8->hd;_TBA=(struct Cyc_Absyn_Exp*)_TB9;{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_TB2,_TB3,_TB4,_TB7,_TBA);_TBB=vd;
_TBB->tq=tq2;_TBC=vd;
_TBC->sc=s;_TBD=vd;
_TBD->attributes=atts2;{struct Cyc_List_List*_TCF=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_TD0=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct));_TD1->tag=0;
_TD1->f1=vd;_TC0=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_TD1;}_TD0->r=(void*)_TC0;_TD0->loc=loc;_TBF=(struct Cyc_Absyn_Decl*)_TD0;}_TCF->hd=_TBF;_TCF->tl=decls;_TBE=(struct Cyc_List_List*)_TCF;}decls=_TBE;}}}_TC1=ds;
# 1021
ds=_TC1->tl;_TC2=_check_null(exprs);exprs=_TC2->tl;_TC3=_check_null(renames);renames=_TC3->tl;goto _TL181;_TL180:;}{struct Cyc_List_List*_TC8=
# 1035
Cyc_List_imp_rev(decls);_npop_handler(0);return _TC8;}}}}}}}}}_pop_region();}
# 1039
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){union Cyc_Absyn_Cnst _T0;struct _union_Cnst_LongLong_c _T1;unsigned _T2;union Cyc_Absyn_Cnst _T3;struct _union_Cnst_Int_c _T4;struct _tuple5 _T5;int _T6;unsigned _T7;union Cyc_Absyn_Cnst _T8;struct _union_Cnst_Char_c _T9;struct _tuple3 _TA;char _TB;unsigned _TC;union Cyc_Absyn_Cnst _TD;struct _union_Cnst_LongLong_c _TE;struct _tuple6 _TF;long long _T10;unsigned _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;long long _T14;unsigned _T15;struct Cyc_String_pa_PrintArg_struct _T16;unsigned _T17;struct _fat_ptr _T18;struct _fat_ptr _T19;long long _T1A;char _T1B;int _T1C;_T0=x;_T1=_T0.LongLong_c;_T2=_T1.tag;switch(_T2){case 5: _T3=x;_T4=_T3.Int_c;_T5=_T4.val;_T1C=_T5.f1;{int i=_T1C;_T6=i;_T7=(unsigned)_T6;
# 1041
return _T7;}case 2: _T8=x;_T9=_T8.Char_c;_TA=_T9.val;_T1B=_TA.f1;{char c=_T1B;_TB=c;_TC=(unsigned)_TB;
return _TC;}case 6: _TD=x;_TE=_TD.LongLong_c;_TF=_TE.val;_T1A=_TF.f1;{long long x=_T1A;_T10=x;{
# 1044
unsigned long long y=(unsigned long long)_T10;
if(y <= 4294967295U)goto _TL189;_T11=loc;_T12=
_tag_fat("integer constant too large",sizeof(char),27U);_T13=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T11,_T12,_T13);goto _TL18A;_TL189: _TL18A: _T14=x;_T15=(unsigned)_T14;
return _T15;}}default:{struct Cyc_String_pa_PrintArg_struct _T1D;_T1D.tag=0;
# 1049
_T1D.f1=Cyc_Absynpp_cnst2string(x);_T16=_T1D;}{struct Cyc_String_pa_PrintArg_struct _T1D=_T16;void*_T1E[1];_T1E[0]=& _T1D;_T17=loc;_T18=_tag_fat("expected integer constant but found %s",sizeof(char),39U);_T19=_tag_fat(_T1E,sizeof(void*),1);Cyc_Warn_err(_T17,_T18,_T19);}
return 0U;};}
# 1055
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct _tuple0*_T3;struct Cyc_Absyn_Pat*_T4;unsigned _T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T7;struct Cyc_Absyn_Pat*_T8;struct Cyc_Absyn_Pat*_T9;void*_TA;int*_TB;int _TC;struct Cyc_Absyn_Vardecl*_TD;struct _tuple0*_TE;struct Cyc_Absyn_Pat*_TF;unsigned _T10;struct Cyc_Absyn_Exp*_T11;struct Cyc_Absyn_Pat*_T12;unsigned _T13;struct Cyc_Absyn_Exp*_T14;struct Cyc_Absyn_Exp*_T15;struct Cyc_Absyn_Pat*_T16;unsigned _T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_Absyn_Pat*_T19;unsigned _T1A;struct Cyc_Absyn_Exp*_T1B;enum Cyc_Absyn_Sign _T1C;int _T1D;struct Cyc_Absyn_Pat*_T1E;unsigned _T1F;struct Cyc_Absyn_Exp*_T20;char _T21;struct Cyc_Absyn_Pat*_T22;unsigned _T23;struct Cyc_Absyn_Exp*_T24;struct _fat_ptr _T25;int _T26;struct Cyc_Absyn_Pat*_T27;unsigned _T28;struct Cyc_Absyn_Exp*_T29;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T2A;int _T2B;struct _tuple0*_T2C;struct Cyc_Absyn_Pat*_T2D;unsigned _T2E;struct Cyc_List_List*(*_T2F)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*);struct Cyc_List_List*(*_T30)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_List_List*_T33;struct Cyc_Absyn_Pat*_T34;unsigned _T35;struct Cyc_Absyn_Exp*_T36;struct Cyc_Absyn_Exp*_T37;struct Cyc_Absyn_Pat*_T38;unsigned _T39;struct _fat_ptr _T3A;struct _fat_ptr _T3B;struct Cyc_Absyn_Pat*_T3C;unsigned _T3D;struct Cyc_Absyn_Exp*_T3E;_T0=p;{
void*_T3F=_T0->r;struct Cyc_Absyn_Exp*_T40;struct Cyc_List_List*_T41;struct _fat_ptr _T42;char _T43;int _T44;enum Cyc_Absyn_Sign _T45;struct Cyc_Absyn_Pat*_T46;struct Cyc_Absyn_Vardecl*_T47;struct _tuple0*_T48;_T1=(int*)_T3F;_T2=*_T1;switch(_T2){case 14:{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T3F;_T48=_T49->f1;}{struct _tuple0*x=_T48;_T3=x;_T4=p;_T5=_T4->loc;_T6=
Cyc_Absyn_unknownid_exp(_T3,_T5);return _T6;}case 3: _T7=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T3F;_T8=_T7->f2;_T9=(struct Cyc_Absyn_Pat*)_T8;_TA=_T9->r;_TB=(int*)_TA;_TC=*_TB;if(_TC!=0)goto _TL18C;{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T3F;_T47=_T49->f1;}{struct Cyc_Absyn_Vardecl*vd=_T47;_TD=vd;_TE=_TD->name;_TF=p;_T10=_TF->loc;_T11=
# 1059
Cyc_Absyn_unknownid_exp(_TE,_T10);_T12=p;_T13=_T12->loc;_T14=Cyc_Absyn_deref_exp(_T11,_T13);return _T14;}_TL18C: goto _LL13;case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T3F;_T46=_T49->f1;}{struct Cyc_Absyn_Pat*p2=_T46;_T15=
Cyc_Parse_pat2exp(p2);_T16=p;_T17=_T16->loc;_T18=Cyc_Absyn_address_exp(_T15,_T17);return _T18;}case 8: _T19=p;_T1A=_T19->loc;_T1B=
Cyc_Absyn_null_exp(_T1A);return _T1B;case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T3F;_T45=_T49->f1;_T44=_T49->f2;}{enum Cyc_Absyn_Sign s=_T45;int i=_T44;_T1C=s;_T1D=i;_T1E=p;_T1F=_T1E->loc;_T20=
Cyc_Absyn_int_exp(_T1C,_T1D,_T1F);return _T20;}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T3F;_T43=_T49->f1;}{char c=_T43;_T21=c;_T22=p;_T23=_T22->loc;_T24=
Cyc_Absyn_char_exp(_T21,_T23);return _T24;}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T3F;_T42=_T49->f1;_T44=_T49->f2;}{struct _fat_ptr s=_T42;int i=_T44;_T25=s;_T26=i;_T27=p;_T28=_T27->loc;_T29=
Cyc_Absyn_float_exp(_T25,_T26,_T28);return _T29;}case 15: _T2A=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T3F;_T2B=_T2A->f3;if(_T2B!=0)goto _TL18E;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T3F;_T48=_T49->f1;_T41=_T49->f2;}{struct _tuple0*x=_T48;struct Cyc_List_List*ps=_T41;_T2C=x;_T2D=p;_T2E=_T2D->loc;{
# 1066
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_T2C,_T2E);_T30=Cyc_List_map;{
struct Cyc_List_List*(*_T49)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))_T30;_T2F=_T49;}_T31=ps;{struct Cyc_List_List*es=_T2F(Cyc_Parse_pat2exp,_T31);_T32=e1;_T33=es;_T34=p;_T35=_T34->loc;_T36=
Cyc_Absyn_unknowncall_exp(_T32,_T33,_T35);return _T36;}}}_TL18E: goto _LL13;case 16:{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T3F;_T40=_T49->f1;}{struct Cyc_Absyn_Exp*e=_T40;_T37=e;
return _T37;}default: _LL13: _T38=p;_T39=_T38->loc;_T3A=
# 1071
_tag_fat("cannot mix patterns and expressions in case",sizeof(char),44U);_T3B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T39,_T3A,_T3B);_T3C=p;_T3D=_T3C->loc;_T3E=
Cyc_Absyn_null_exp(_T3D);return _T3E;};}}
# 1075
static struct _tuple16 Cyc_Parse_split_seq(struct Cyc_Absyn_Exp*maybe_seq){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct _tuple16 _T3;struct _tuple16 _T4;_T0=maybe_seq;{
void*_T5=_T0->r;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Exp*_T7;_T1=(int*)_T5;_T2=*_T1;if(_T2!=9)goto _TL190;{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T5;_T7=_T8->f1;_T6=_T8->f2;}{struct Cyc_Absyn_Exp*e1=_T7;struct Cyc_Absyn_Exp*e2=_T6;{struct _tuple16 _T8;
# 1078
_T8.f0=e1;_T8.f1=e2;_T3=_T8;}return _T3;}_TL190:{struct _tuple16 _T8;
# 1080
_T8.f0=maybe_seq;_T8.f1=0;_T4=_T8;}return _T4;;}}
# 1083
static struct Cyc_Absyn_Exp*Cyc_Parse_join_assn(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp*_T1;struct Cyc_Absyn_Exp*_T2;
if(e1==0)goto _TL192;if(e2==0)goto _TL192;_T0=Cyc_Absyn_and_exp(e1,e2,0U);return _T0;
_TL192: if(e1==0)goto _TL194;_T1=e1;return _T1;
_TL194: _T2=e2;return _T2;}struct _tuple21{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
# 1088
static struct _tuple21 Cyc_Parse_join_assns(struct _tuple21 a1,struct _tuple21 a2){struct _tuple21 _T0;struct _tuple21 _T1;struct _tuple21 _T2;struct _tuple21 _T3;struct _tuple21 _T4;struct _tuple21 _T5;struct _tuple21 _T6;struct _tuple21 _T7;struct _tuple21 _T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Exp*_TC;_T0=a1;_TC=_T0.f0;_T1=a1;_TB=_T1.f1;_T2=a1;_TA=_T2.f2;_T3=a1;_T9=_T3.f3;{struct Cyc_Absyn_Exp*c1=_TC;struct Cyc_Absyn_Exp*r1=_TB;struct Cyc_Absyn_Exp*e1=_TA;struct Cyc_Absyn_Exp*t1=_T9;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_Absyn_Exp*_TF;struct Cyc_Absyn_Exp*_T10;_T4=a2;_T10=_T4.f0;_T5=a2;_TF=_T5.f1;_T6=a2;_TE=_T6.f2;_T7=a2;_TD=_T7.f3;{struct Cyc_Absyn_Exp*c2=_T10;struct Cyc_Absyn_Exp*r2=_TF;struct Cyc_Absyn_Exp*e2=_TE;struct Cyc_Absyn_Exp*t2=_TD;
# 1091
struct Cyc_Absyn_Exp*c=Cyc_Parse_join_assn(c1,c2);
struct Cyc_Absyn_Exp*r=Cyc_Parse_join_assn(r1,r2);
struct Cyc_Absyn_Exp*e=Cyc_Parse_join_assn(e1,e2);
struct Cyc_Absyn_Exp*t=Cyc_Parse_join_assn(t1,t2);{struct _tuple21 _T11;
_T11.f0=c;_T11.f1=r;_T11.f2=e;_T11.f3=t;_T8=_T11;}return _T8;}}}
# 1098
static void*Cyc_Parse_assign_cvar_pos(struct _fat_ptr posstr,int ovfat,void*cv){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T5;const char**_T6;void*_T7;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T8;int*_T9;const char**_TA;struct _fat_ptr _TB;unsigned char*_TC;int*_TD;void*_TE;void*_TF;void*_T10;void*_T11;_T0=cv;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TL196;_T3=cv;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T12=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T4=cv;_T5=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T4;_T6=& _T5->f6;_T11=(const char**)_T6;_T7=cv;_T8=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T9=& _T8->f7;_T10=(int*)_T9;}{const char**pos=(const char**)_T11;int*ov=(int*)_T10;_TA=pos;_TB=posstr;_TC=_untag_fat_ptr_check_bound(_TB,sizeof(char),1U);
# 1101
*_TA=(const char*)_TC;_TD=ov;
*_TD=ovfat;_TE=cv;
return _TE;}_TL196: _TF=cv;
# 1105
return _TF;;}
# 1109
static void*Cyc_Parse_typevar2cvar(struct _fat_ptr s){struct Cyc_Hashtable_Table*_T0;unsigned _T1;struct Cyc_Hashtable_Table*(*_T2)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T3)(int,int(*)(void*,void*),int(*)(void*));int(*_T4)(struct _fat_ptr*,struct _fat_ptr*);int(*_T5)(struct _fat_ptr*);struct _handler_cons*_T6;int _T7;void*(*_T8)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_T9)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TA;struct _fat_ptr*_TB;void*_TC;struct Cyc_Core_Not_found_exn_struct*_TD;char*_TE;char*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct _fat_ptr*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;int _T15;struct Cyc_Core_Opt*_T16;struct Cyc_Core_Opt*_T17;struct _fat_ptr _T18;void(*_T19)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*);void(*_T1A)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T1B;struct _fat_ptr*_T1C;void*_T1D;void*_T1E;int(*_T1F)(unsigned,struct _fat_ptr);struct _fat_ptr _T20;
# 1112
static struct Cyc_Hashtable_Table*cvmap=0;_T0=cvmap;_T1=(unsigned)_T0;
if(_T1)goto _TL198;else{goto _TL19A;}
_TL19A: _T3=Cyc_Hashtable_create;{struct Cyc_Hashtable_Table*(*_T21)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T3;_T2=_T21;}_T4=Cyc_strptrcmp;_T5=Cyc_Hashtable_hash_stringptr;cvmap=_T2(101,_T4,_T5);goto _TL199;_TL198: _TL199: {struct _handler_cons _T21;_T6=& _T21;_push_handler(_T6);{int _T22=0;_T7=setjmp(_T21.handler);if(!_T7)goto _TL19B;_T22=1;goto _TL19C;_TL19B: _TL19C: if(_T22)goto _TL19D;else{goto _TL19F;}_TL19F: _T9=Cyc_Hashtable_lookup;{
# 1116
void*(*_T23)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(void*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_T9;_T8=_T23;}_TA=cvmap;{struct _fat_ptr*_T23=_cycalloc(sizeof(struct _fat_ptr));*_T23=s;_TB=(struct _fat_ptr*)_T23;}{void*_T23=_T8(_TA,_TB);_npop_handler(0);return _T23;}_pop_handler();goto _TL19E;_TL19D: _TC=Cyc_Core_get_exn_thrown();{void*_T23=(void*)_TC;void*_T24;_TD=(struct Cyc_Core_Not_found_exn_struct*)_T23;_TE=_TD->tag;_TF=Cyc_Core_Not_found;if(_TE!=_TF)goto _TL1A0;{
# 1119
struct _fat_ptr kind=Cyc_strchr(s,'_');_T10=kind;_T11=
_fat_ptr_plus(_T10,sizeof(char),1);{struct _fat_ptr name=Cyc_strchr(_T11,'_');_T12=& name;
_fat_ptr_inplace_plus(_T12,sizeof(char),1);_T13=kind;_T14=
_tag_fat("_PTRBND",sizeof(char),8U);_T15=Cyc_strncmp(_T13,_T14,7U);if(_T15)goto _TL1A2;else{goto _TL1A4;}
_TL1A4: _T16=& Cyc_Kinds_ptrbko;_T17=(struct Cyc_Core_Opt*)_T16;_T18=name;{void*t=Cyc_Absyn_cvar_type_name(_T17,_T18);_T1A=Cyc_Hashtable_insert;{
void(*_T25)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*))_T1A;_T19=_T25;}_T1B=_check_null(cvmap);{struct _fat_ptr*_T25=_cycalloc(sizeof(struct _fat_ptr));*_T25=s;_T1C=(struct _fat_ptr*)_T25;}_T1D=t;_T19(_T1B,_T1C,_T1D);_T1E=t;
return _T1E;}
# 1128
_TL1A2:{int(*_T25)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T1F=_T25;}_T20=_tag_fat("Constraint variable unknown kind",sizeof(char),33U);_T1F(0U,_T20);;}}goto _TL1A1;_TL1A0: _T24=_T23;{void*exn=_T24;_rethrow(exn);}_TL1A1:;}_TL19E:;}}}
# 1134
static void*Cyc_Parse_str2type(unsigned loc,struct _fat_ptr s){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;void*_T3;void*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;int _T7;void*_T8;void*_T9;void*_TA;int(*_TB)(unsigned,struct _fat_ptr);unsigned _TC;struct _fat_ptr _TD;struct Cyc_String_pa_PrintArg_struct _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;_T0=s;_T1=
_tag_fat("@fat",sizeof(char),5U);_T2=Cyc_strcmp(_T0,_T1);if(_T2)goto _TL1A5;else{goto _TL1A7;}
_TL1A7: _T3=Cyc_Tcutil_ptrbnd_cvar_equivalent(Cyc_Absyn_fat_bound_type);_T4=_check_null(_T3);return _T4;
# 1138
_TL1A5: _T5=s;_T6=_tag_fat("@thin @numelts{valueof_t(1U)}",sizeof(char),30U);_T7=Cyc_strcmp(_T5,_T6);if(_T7)goto _TL1A8;else{goto _TL1AA;}
_TL1AA: _T8=Cyc_Absyn_bounds_one();_T9=Cyc_Tcutil_ptrbnd_cvar_equivalent(_T8);_TA=_check_null(_T9);return _TA;_TL1A8:{
# 1141
int(*_T11)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TB=_T11;}_TC=loc;{struct Cyc_String_pa_PrintArg_struct _T11;_T11.tag=0;_T11.f1=s;_TE=_T11;}{struct Cyc_String_pa_PrintArg_struct _T11=_TE;void*_T12[1];_T12[0]=& _T11;_TF=_tag_fat("Unknown type constant:: %s",sizeof(char),27U);_T10=_tag_fat(_T12,sizeof(void*),1);_TD=Cyc_aprintf(_TF,_T10);}_TB(_TC,_TD);}
# 1144
static void*Cyc_Parse_composite_constraint(enum Cyc_Parse_ConstraintOps op,void*t1,void*t2){enum Cyc_Parse_ConstraintOps _T0;int _T1;void*_T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;void*_T8;int(*_T9)(unsigned,struct _fat_ptr);struct _fat_ptr _TA;_T0=op;_T1=(int)_T0;switch(_T1){case Cyc_Parse_C_AND_OP: _T2=t1;_T3=
# 1146
_check_null(t2);_T4=Cyc_BansheeIf_and_constraint(_T2,_T3);return _T4;case Cyc_Parse_C_OR_OP: _T5=t1;_T6=
_check_null(t2);_T7=Cyc_BansheeIf_or_constraint(_T5,_T6);return _T7;case Cyc_Parse_C_NOT_OP: _T8=
Cyc_BansheeIf_not_constraint(t1);return _T8;default:{
# 1150
int(*_TB)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T9=_TB;}_TA=_tag_fat("Unexpected operator for composite constraint",sizeof(char),45U);_T9(0U,_TA);};}
# 1154
static void*Cyc_Parse_comparison_constraint(enum Cyc_Parse_ConstraintOps op,void*t1,void*t2){enum Cyc_Parse_ConstraintOps _T0;int _T1;void*_T2;void*_T3;int(*_T4)(unsigned,struct _fat_ptr);struct _fat_ptr _T5;_T0=op;_T1=(int)_T0;switch(_T1){case Cyc_Parse_C_EQ_OP: _T2=
# 1156
Cyc_BansheeIf_cmpeq_constraint(t1,t2);return _T2;case Cyc_Parse_C_INCL_OP: _T3=
Cyc_BansheeIf_inclusion_constraint(t1,t2);return _T3;default:{
# 1159
int(*_T6)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T4=_T6;}_T5=_tag_fat("Unexpected operator for composite constraint",sizeof(char),45U);_T4(0U,_T5);};}struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f0;void*f1;void*f2;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f0;int f1;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f0;enum Cyc_Absyn_AggrKind f1;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _union_YYSTYPE_YY29{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f0;struct Cyc_Parse_Type_specifier f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY36{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f0;int f1;struct Cyc_Absyn_VarargInfo*f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _tuple28{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _union_YYSTYPE_YY51{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _tuple29{void*f0;void*f1;};struct _union_YYSTYPE_YY53{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _tuple30{struct Cyc_List_List*f0;unsigned f1;};struct _union_YYSTYPE_YY56{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _union_YYSTYPE_YY59{int tag;void*val;};struct _union_YYSTYPE_YY60{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY62{int tag;struct _tuple21 val;};struct _union_YYSTYPE_YY63{int tag;void*val;};struct _tuple31{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY64{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY65{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY67{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};struct _union_YYSTYPE_YY68{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY69{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YY70{int tag;enum Cyc_Parse_ConstraintOps val;};struct _union_YYSTYPE_YY71{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY72{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY73{int tag;void*val;};struct _union_YYSTYPE_YY74{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YY67 YY67;struct _union_YYSTYPE_YY68 YY68;struct _union_YYSTYPE_YY69 YY69;struct _union_YYSTYPE_YY70 YY70;struct _union_YYSTYPE_YY71 YY71;struct _union_YYSTYPE_YY72 YY72;struct _union_YYSTYPE_YY73 YY73;struct _union_YYSTYPE_YY74 YY74;};
# 1252
static void Cyc_yythrowfail(struct _fat_ptr s){struct Cyc_Core_Failure_exn_struct*_T0;void*_T1;{struct Cyc_Core_Failure_exn_struct*_T2=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_T2->tag=Cyc_Core_Failure;
_T2->f1=s;_T0=(struct Cyc_Core_Failure_exn_struct*)_T2;}_T1=(void*)_T0;_throw(_T1);}static char _TmpG2[7U]="cnst_t";
# 1215 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Int_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Int_tok _T5;union Cyc_Absyn_Cnst _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2,(unsigned char*)_TmpG2,(unsigned char*)_TmpG2 + 7U};union Cyc_Absyn_Cnst _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Int_tok;_T3=_T2.tag;if(_T3!=2)goto _TL1AD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Int_tok;_T7=_T5.val;}{union Cyc_Absyn_Cnst yy=_T7;_T6=yy;
# 1218
return _T6;}_TL1AD:
 Cyc_yythrowfail(s);;}
# 1222
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Int_tok.tag=2U;_T1.Int_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3[5U]="char";
# 1216 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Char_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Char_tok _T5;char _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3,(unsigned char*)_TmpG3,(unsigned char*)_TmpG3 + 5U};char _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Char_tok;_T3=_T2.tag;if(_T3!=3)goto _TL1AF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Char_tok;_T7=_T5.val;}{char yy=_T7;_T6=yy;
# 1219
return _T6;}_TL1AF:
 Cyc_yythrowfail(s);;}
# 1223
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Char_tok.tag=3U;_T1.Char_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4[13U]="string_t<`H>";
# 1217 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_String_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_String_tok _T5;struct _fat_ptr _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4,(unsigned char*)_TmpG4,(unsigned char*)_TmpG4 + 13U};struct _fat_ptr _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->String_tok;_T3=_T2.tag;if(_T3!=4)goto _TL1B1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.String_tok;_T7=_T5.val;}{struct _fat_ptr yy=_T7;_T6=yy;
# 1220
return _T6;}_TL1B1:
 Cyc_yythrowfail(s);;}
# 1224
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.String_tok.tag=4U;_T1.String_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1220 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY1 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY1 _T5;struct _tuple22*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG5,(unsigned char*)_TmpG5,(unsigned char*)_TmpG5 + 35U};struct _tuple22*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY1;_T3=_T2.tag;if(_T3!=8)goto _TL1B3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY1;_T7=_T5.val;}{struct _tuple22*yy=_T7;_T6=yy;
# 1223
return _T6;}_TL1B3:
 Cyc_yythrowfail(s);;}
# 1227
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY1.tag=8U;_T1.YY1.val=yy1;_T0=_T1;}return _T0;}static char _TmpG6[11U]="ptrbound_t";
# 1221 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY2 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY2 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG6,(unsigned char*)_TmpG6,(unsigned char*)_TmpG6 + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY2;_T3=_T2.tag;if(_T3!=9)goto _TL1B5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY2;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1224
return _T6;}_TL1B5:
 Cyc_yythrowfail(s);;}
# 1228
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY2.tag=9U;_T1.YY2.val=yy1;_T0=_T1;}return _T0;}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1222 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY3 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY3 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG7,(unsigned char*)_TmpG7,(unsigned char*)_TmpG7 + 28U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY3;_T3=_T2.tag;if(_T3!=10)goto _TL1B7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY3;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1225
return _T6;}_TL1B7:
 Cyc_yythrowfail(s);;}
# 1229
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY3.tag=10U;_T1.YY3.val=yy1;_T0=_T1;}return _T0;}static char _TmpG8[6U]="exp_t";
# 1223 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Exp_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Exp_tok _T5;struct Cyc_Absyn_Exp*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG8,(unsigned char*)_TmpG8,(unsigned char*)_TmpG8 + 6U};struct Cyc_Absyn_Exp*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Exp_tok;_T3=_T2.tag;if(_T3!=6)goto _TL1B9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Exp_tok;_T7=_T5.val;}{struct Cyc_Absyn_Exp*yy=_T7;_T6=yy;
# 1226
return _T6;}_TL1B9:
 Cyc_yythrowfail(s);;}
# 1230
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Exp_tok.tag=6U;_T1.Exp_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY4 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY4 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG9,(unsigned char*)_TmpG9,(unsigned char*)_TmpG9 + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY4;_T3=_T2.tag;if(_T3!=11)goto _TL1BB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY4;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1234
return _T6;}_TL1BB:
 Cyc_yythrowfail(s);;}
# 1238
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY4.tag=11U;_T1.YY4.val=yy1;_T0=_T1;}return _T0;}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1232 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY5 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY5 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGA,(unsigned char*)_TmpGA,(unsigned char*)_TmpGA + 47U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY5;_T3=_T2.tag;if(_T3!=12)goto _TL1BD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY5;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1235
return _T6;}_TL1BD:
 Cyc_yythrowfail(s);;}
# 1239
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY5.tag=12U;_T1.YY5.val=yy1;_T0=_T1;}return _T0;}static char _TmpGB[9U]="primop_t";
# 1233 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY6 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY6 _T5;enum Cyc_Absyn_Primop _T6;
static struct _fat_ptr s={(unsigned char*)_TmpGB,(unsigned char*)_TmpGB,(unsigned char*)_TmpGB + 9U};enum Cyc_Absyn_Primop _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY6;_T3=_T2.tag;if(_T3!=13)goto _TL1BF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY6;_T7=_T5.val;}{enum Cyc_Absyn_Primop yy=_T7;_T6=yy;
# 1236
return _T6;}_TL1BF:
 Cyc_yythrowfail(s);;}
# 1240
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY6.tag=13U;_T1.YY6.val=yy1;_T0=_T1;}return _T0;}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1234 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY7 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY7 _T5;struct Cyc_Core_Opt*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGC,(unsigned char*)_TmpGC,(unsigned char*)_TmpGC + 19U};struct Cyc_Core_Opt*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY7;_T3=_T2.tag;if(_T3!=14)goto _TL1C1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY7;_T7=_T5.val;}{struct Cyc_Core_Opt*yy=_T7;_T6=yy;
# 1237
return _T6;}_TL1C1:
 Cyc_yythrowfail(s);;}
# 1241
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY7.tag=14U;_T1.YY7.val=yy1;_T0=_T1;}return _T0;}static char _TmpGD[7U]="qvar_t";
# 1235 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_QualId_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_QualId_tok _T5;struct _tuple0*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGD,(unsigned char*)_TmpGD,(unsigned char*)_TmpGD + 7U};struct _tuple0*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->QualId_tok;_T3=_T2.tag;if(_T3!=5)goto _TL1C3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.QualId_tok;_T7=_T5.val;}{struct _tuple0*yy=_T7;_T6=yy;
# 1238
return _T6;}_TL1C3:
 Cyc_yythrowfail(s);;}
# 1242
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.QualId_tok.tag=5U;_T1.QualId_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpGE[7U]="stmt_t";
# 1238 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Stmt_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Stmt_tok _T5;struct Cyc_Absyn_Stmt*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGE,(unsigned char*)_TmpGE,(unsigned char*)_TmpGE + 7U};struct Cyc_Absyn_Stmt*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Stmt_tok;_T3=_T2.tag;if(_T3!=7)goto _TL1C5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Stmt_tok;_T7=_T5.val;}{struct Cyc_Absyn_Stmt*yy=_T7;_T6=yy;
# 1241
return _T6;}_TL1C5:
 Cyc_yythrowfail(s);;}
# 1245
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Stmt_tok.tag=7U;_T1.Stmt_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1241 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY8 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY8 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGF,(unsigned char*)_TmpGF,(unsigned char*)_TmpGF + 27U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY8;_T3=_T2.tag;if(_T3!=15)goto _TL1C7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY8;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1244
return _T6;}_TL1C7:
 Cyc_yythrowfail(s);;}
# 1248
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY8.tag=15U;_T1.YY8.val=yy1;_T0=_T1;}return _T0;}static char _TmpG10[6U]="pat_t";
# 1242 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY9 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY9 _T5;struct Cyc_Absyn_Pat*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG10,(unsigned char*)_TmpG10,(unsigned char*)_TmpG10 + 6U};struct Cyc_Absyn_Pat*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY9;_T3=_T2.tag;if(_T3!=16)goto _TL1C9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY9;_T7=_T5.val;}{struct Cyc_Absyn_Pat*yy=_T7;_T6=yy;
# 1245
return _T6;}_TL1C9:
 Cyc_yythrowfail(s);;}
# 1249
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY9.tag=16U;_T1.YY9.val=yy1;_T0=_T1;}return _T0;}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1247 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY10 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY10 _T5;struct _tuple23*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG11,(unsigned char*)_TmpG11,(unsigned char*)_TmpG11 + 28U};struct _tuple23*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY10;_T3=_T2.tag;if(_T3!=17)goto _TL1CB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY10;_T7=_T5.val;}{struct _tuple23*yy=_T7;_T6=yy;
# 1250
return _T6;}_TL1CB:
 Cyc_yythrowfail(s);;}
# 1254
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY10.tag=17U;_T1.YY10.val=yy1;_T0=_T1;}return _T0;}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1248 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY11 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY11 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG12,(unsigned char*)_TmpG12,(unsigned char*)_TmpG12 + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY11;_T3=_T2.tag;if(_T3!=18)goto _TL1CD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY11;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1251
return _T6;}_TL1CD:
 Cyc_yythrowfail(s);;}
# 1255
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY11.tag=18U;_T1.YY11.val=yy1;_T0=_T1;}return _T0;}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1249 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY12 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY12 _T5;struct _tuple24*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG13,(unsigned char*)_TmpG13,(unsigned char*)_TmpG13 + 36U};struct _tuple24*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY12;_T3=_T2.tag;if(_T3!=19)goto _TL1CF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY12;_T7=_T5.val;}{struct _tuple24*yy=_T7;_T6=yy;
# 1252
return _T6;}_TL1CF:
 Cyc_yythrowfail(s);;}
# 1256
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY12.tag=19U;_T1.YY12.val=yy1;_T0=_T1;}return _T0;}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1250 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY13 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY13 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG14,(unsigned char*)_TmpG14,(unsigned char*)_TmpG14 + 47U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY13;_T3=_T2.tag;if(_T3!=20)goto _TL1D1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY13;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1253
return _T6;}_TL1D1:
 Cyc_yythrowfail(s);;}
# 1257
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY13.tag=20U;_T1.YY13.val=yy1;_T0=_T1;}return _T0;}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1251 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY14 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY14 _T5;struct _tuple23*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG15,(unsigned char*)_TmpG15,(unsigned char*)_TmpG15 + 58U};struct _tuple23*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY14;_T3=_T2.tag;if(_T3!=21)goto _TL1D3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY14;_T7=_T5.val;}{struct _tuple23*yy=_T7;_T6=yy;
# 1254
return _T6;}_TL1D3:
 Cyc_yythrowfail(s);;}
# 1258
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY14.tag=21U;_T1.YY14.val=yy1;_T0=_T1;}return _T0;}static char _TmpG16[9U]="fndecl_t";
# 1252 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY15 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY15 _T5;struct Cyc_Absyn_Fndecl*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG16,(unsigned char*)_TmpG16,(unsigned char*)_TmpG16 + 9U};struct Cyc_Absyn_Fndecl*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY15;_T3=_T2.tag;if(_T3!=22)goto _TL1D5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY15;_T7=_T5.val;}{struct Cyc_Absyn_Fndecl*yy=_T7;_T6=yy;
# 1255
return _T6;}_TL1D5:
 Cyc_yythrowfail(s);;}
# 1259
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY15.tag=22U;_T1.YY15.val=yy1;_T0=_T1;}return _T0;}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1253 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY16 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY16 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG17,(unsigned char*)_TmpG17,(unsigned char*)_TmpG17 + 18U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY16;_T3=_T2.tag;if(_T3!=23)goto _TL1D7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY16;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1256
return _T6;}_TL1D7:
 Cyc_yythrowfail(s);;}
# 1260
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY16.tag=23U;_T1.YY16.val=yy1;_T0=_T1;}return _T0;}static char _TmpG18[12U]="decl_spec_t";
# 1256 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY17 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY17 _T5;struct Cyc_Parse_Declaration_spec _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG18,(unsigned char*)_TmpG18,(unsigned char*)_TmpG18 + 12U};struct Cyc_Parse_Declaration_spec _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY17;_T3=_T2.tag;if(_T3!=24)goto _TL1D9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY17;_T7=_T5.val;}{struct Cyc_Parse_Declaration_spec yy=_T7;_T6=yy;
# 1259
return _T6;}_TL1D9:
 Cyc_yythrowfail(s);;}
# 1263
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY17.tag=24U;_T1.YY17.val=yy1;_T0=_T1;}return _T0;}static char _TmpG19[41U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)";
# 1257 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY18 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY18 _T5;struct _tuple12 _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG19,(unsigned char*)_TmpG19,(unsigned char*)_TmpG19 + 41U};struct _tuple12 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY18;_T3=_T2.tag;if(_T3!=25)goto _TL1DB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY18;_T7=_T5.val;}{struct _tuple12 yy=_T7;_T6=yy;
# 1260
return _T6;}_TL1DB:
 Cyc_yythrowfail(s);;}
# 1264
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY18.tag=25U;_T1.YY18.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1258 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY19 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY19 _T5;struct _tuple11*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1A,(unsigned char*)_TmpG1A,(unsigned char*)_TmpG1A + 23U};struct _tuple11*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY19;_T3=_T2.tag;if(_T3!=26)goto _TL1DD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY19;_T7=_T5.val;}{struct _tuple11*yy=_T7;_T6=yy;
# 1261
return _T6;}_TL1DD:
 Cyc_yythrowfail(s);;}
# 1265
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY19.tag=26U;_T1.YY19.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1B[16U]="storage_class_t";
# 1259 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY20 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY20 _T5;enum Cyc_Parse_Storage_class _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1B,(unsigned char*)_TmpG1B,(unsigned char*)_TmpG1B + 16U};enum Cyc_Parse_Storage_class _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY20;_T3=_T2.tag;if(_T3!=27)goto _TL1DF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY20;_T7=_T5.val;}{enum Cyc_Parse_Storage_class yy=_T7;_T6=yy;
# 1262
return _T6;}_TL1DF:
 Cyc_yythrowfail(s);;}
# 1266
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY20.tag=27U;_T1.YY20.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1C[17U]="type_specifier_t";
# 1260 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY21 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY21 _T5;struct Cyc_Parse_Type_specifier _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1C,(unsigned char*)_TmpG1C,(unsigned char*)_TmpG1C + 17U};struct Cyc_Parse_Type_specifier _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY21;_T3=_T2.tag;if(_T3!=28)goto _TL1E1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY21;_T7=_T5.val;}{struct Cyc_Parse_Type_specifier yy=_T7;_T6=yy;
# 1263
return _T6;}_TL1E1:
 Cyc_yythrowfail(s);;}
# 1267
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY21.tag=28U;_T1.YY21.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1D[12U]="aggr_kind_t";
# 1262 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY22 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY22 _T5;enum Cyc_Absyn_AggrKind _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1D,(unsigned char*)_TmpG1D,(unsigned char*)_TmpG1D + 12U};enum Cyc_Absyn_AggrKind _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY22;_T3=_T2.tag;if(_T3!=29)goto _TL1E3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY22;_T7=_T5.val;}{enum Cyc_Absyn_AggrKind yy=_T7;_T6=yy;
# 1265
return _T6;}_TL1E3:
 Cyc_yythrowfail(s);;}
# 1269
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY22.tag=29U;_T1.YY22.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1263 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY23 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY23 _T5;struct _tuple25 _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1E,(unsigned char*)_TmpG1E,(unsigned char*)_TmpG1E + 20U};struct _tuple25 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY23;_T3=_T2.tag;if(_T3!=30)goto _TL1E5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY23;_T7=_T5.val;}{struct _tuple25 yy=_T7;_T6=yy;
# 1266
return _T6;}_TL1E5:
 Cyc_yythrowfail(s);;}
# 1270
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY23.tag=30U;_T1.YY23.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1F[8U]="tqual_t";
# 1264 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY24 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY24 _T5;struct Cyc_Absyn_Tqual _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1F,(unsigned char*)_TmpG1F,(unsigned char*)_TmpG1F + 8U};struct Cyc_Absyn_Tqual _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY24;_T3=_T2.tag;if(_T3!=31)goto _TL1E7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY24;_T7=_T5.val;}{struct Cyc_Absyn_Tqual yy=_T7;_T6=yy;
# 1267
return _T6;}_TL1E7:
 Cyc_yythrowfail(s);;}
# 1271
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY24.tag=31U;_T1.YY24.val=yy1;_T0=_T1;}return _T0;}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1265 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY25 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY25 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG20,(unsigned char*)_TmpG20,(unsigned char*)_TmpG20 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY25;_T3=_T2.tag;if(_T3!=32)goto _TL1E9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY25;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1268
return _T6;}_TL1E9:
 Cyc_yythrowfail(s);;}
# 1272
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY25.tag=32U;_T1.YY25.val=yy1;_T0=_T1;}return _T0;}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1266 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY26 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY26 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG21,(unsigned char*)_TmpG21,(unsigned char*)_TmpG21 + 34U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY26;_T3=_T2.tag;if(_T3!=33)goto _TL1EB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY26;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1269
return _T6;}_TL1EB:
 Cyc_yythrowfail(s);;}
# 1273
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY26.tag=33U;_T1.YY26.val=yy1;_T0=_T1;}return _T0;}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1267 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY27 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY27 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG22,(unsigned char*)_TmpG22,(unsigned char*)_TmpG22 + 33U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY27;_T3=_T2.tag;if(_T3!=34)goto _TL1ED;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY27;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1270
return _T6;}_TL1ED:
 Cyc_yythrowfail(s);;}
# 1274
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY27.tag=34U;_T1.YY27.val=yy1;_T0=_T1;}return _T0;}static char _TmpG23[18U]="declarator_t<`yy>";
# 1268 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY28 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY28 _T5;struct Cyc_Parse_Declarator _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG23,(unsigned char*)_TmpG23,(unsigned char*)_TmpG23 + 18U};struct Cyc_Parse_Declarator _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY28;_T3=_T2.tag;if(_T3!=35)goto _TL1EF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY28;_T7=_T5.val;}{struct Cyc_Parse_Declarator yy=_T7;_T6=yy;
# 1271
return _T6;}_TL1EF:
 Cyc_yythrowfail(s);;}
# 1275
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY28.tag=35U;_T1.YY28.val=yy1;_T0=_T1;}return _T0;}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1269 "parse.y"
static struct _tuple12*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY29 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY29 _T5;struct _tuple12*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG24,(unsigned char*)_TmpG24,(unsigned char*)_TmpG24 + 45U};struct _tuple12*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY29;_T3=_T2.tag;if(_T3!=36)goto _TL1F1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY29;_T7=_T5.val;}{struct _tuple12*yy=_T7;_T6=yy;
# 1272
return _T6;}_TL1F1:
 Cyc_yythrowfail(s);;}
# 1276
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple12*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY29.tag=36U;_T1.YY29.val=yy1;_T0=_T1;}return _T0;}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1270 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY30 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY30 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG25,(unsigned char*)_TmpG25,(unsigned char*)_TmpG25 + 57U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY30;_T3=_T2.tag;if(_T3!=37)goto _TL1F3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY30;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1273
return _T6;}_TL1F3:
 Cyc_yythrowfail(s);;}
# 1277
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY30.tag=37U;_T1.YY30.val=yy1;_T0=_T1;}return _T0;}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1271 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY31 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY31 _T5;struct Cyc_Parse_Abstractdeclarator _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG26,(unsigned char*)_TmpG26,(unsigned char*)_TmpG26 + 26U};struct Cyc_Parse_Abstractdeclarator _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY31;_T3=_T2.tag;if(_T3!=38)goto _TL1F5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY31;_T7=_T5.val;}{struct Cyc_Parse_Abstractdeclarator yy=_T7;_T6=yy;
# 1274
return _T6;}_TL1F5:
 Cyc_yythrowfail(s);;}
# 1278
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY31.tag=38U;_T1.YY31.val=yy1;_T0=_T1;}return _T0;}static char _TmpG27[5U]="bool";
# 1272 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY32 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY32 _T5;int _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG27,(unsigned char*)_TmpG27,(unsigned char*)_TmpG27 + 5U};int _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY32;_T3=_T2.tag;if(_T3!=39)goto _TL1F7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY32;_T7=_T5.val;}{int yy=_T7;_T6=yy;
# 1275
return _T6;}_TL1F7:
 Cyc_yythrowfail(s);;}
# 1279
static union Cyc_YYSTYPE Cyc_YY32(int yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY32.tag=39U;_T1.YY32.val=yy1;_T0=_T1;}return _T0;}static char _TmpG28[8U]="scope_t";
# 1273 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY33 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY33 _T5;enum Cyc_Absyn_Scope _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG28,(unsigned char*)_TmpG28,(unsigned char*)_TmpG28 + 8U};enum Cyc_Absyn_Scope _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY33;_T3=_T2.tag;if(_T3!=40)goto _TL1F9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY33;_T7=_T5.val;}{enum Cyc_Absyn_Scope yy=_T7;_T6=yy;
# 1276
return _T6;}_TL1F9:
 Cyc_yythrowfail(s);;}
# 1280
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY33.tag=40U;_T1.YY33.val=yy1;_T0=_T1;}return _T0;}static char _TmpG29[16U]="datatypefield_t";
# 1274 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY34 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY34 _T5;struct Cyc_Absyn_Datatypefield*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG29,(unsigned char*)_TmpG29,(unsigned char*)_TmpG29 + 16U};struct Cyc_Absyn_Datatypefield*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY34;_T3=_T2.tag;if(_T3!=41)goto _TL1FB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY34;_T7=_T5.val;}{struct Cyc_Absyn_Datatypefield*yy=_T7;_T6=yy;
# 1277
return _T6;}_TL1FB:
 Cyc_yythrowfail(s);;}
# 1281
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY34.tag=41U;_T1.YY34.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1275 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY35 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY35 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2A,(unsigned char*)_TmpG2A,(unsigned char*)_TmpG2A + 27U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY35;_T3=_T2.tag;if(_T3!=42)goto _TL1FD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY35;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1278
return _T6;}_TL1FD:
 Cyc_yythrowfail(s);;}
# 1282
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY35.tag=42U;_T1.YY35.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1276 "parse.y"
static struct _tuple26 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY36 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY36 _T5;struct _tuple26 _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2B,(unsigned char*)_TmpG2B,(unsigned char*)_TmpG2B + 41U};struct _tuple26 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY36;_T3=_T2.tag;if(_T3!=43)goto _TL1FF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY36;_T7=_T5.val;}{struct _tuple26 yy=_T7;_T6=yy;
# 1279
return _T6;}_TL1FF:
 Cyc_yythrowfail(s);;}
# 1283
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple26 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY36.tag=43U;_T1.YY36.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1277 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY37 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY37 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2C,(unsigned char*)_TmpG2C,(unsigned char*)_TmpG2C + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY37;_T3=_T2.tag;if(_T3!=44)goto _TL201;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY37;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1280
return _T6;}_TL201:
 Cyc_yythrowfail(s);;}
# 1284
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY37.tag=44U;_T1.YY37.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1278 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY38 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY38 _T5;struct _tuple8*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2D,(unsigned char*)_TmpG2D,(unsigned char*)_TmpG2D + 31U};struct _tuple8*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY38;_T3=_T2.tag;if(_T3!=45)goto _TL203;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY38;_T7=_T5.val;}{struct _tuple8*yy=_T7;_T6=yy;
# 1281
return _T6;}_TL203:
 Cyc_yythrowfail(s);;}
# 1285
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY38.tag=45U;_T1.YY38.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1279 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY39 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY39 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2E,(unsigned char*)_TmpG2E,(unsigned char*)_TmpG2E + 42U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY39;_T3=_T2.tag;if(_T3!=46)goto _TL205;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY39;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1282
return _T6;}_TL205:
 Cyc_yythrowfail(s);;}
# 1286
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY39.tag=46U;_T1.YY39.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2F[139U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1280 "parse.y"
static struct _tuple27*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY40 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY40 _T5;struct _tuple27*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2F,(unsigned char*)_TmpG2F,(unsigned char*)_TmpG2F + 139U};struct _tuple27*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY40;_T3=_T2.tag;if(_T3!=47)goto _TL207;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY40;_T7=_T5.val;}{struct _tuple27*yy=_T7;_T6=yy;
# 1283
return _T6;}_TL207:
 Cyc_yythrowfail(s);;}
# 1287
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple27*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY40.tag=47U;_T1.YY40.val=yy1;_T0=_T1;}return _T0;}static char _TmpG30[8U]="types_t";
# 1281 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY41 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY41 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG30,(unsigned char*)_TmpG30,(unsigned char*)_TmpG30 + 8U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY41;_T3=_T2.tag;if(_T3!=48)goto _TL209;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY41;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1284
return _T6;}_TL209:
 Cyc_yythrowfail(s);;}
# 1288
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY41.tag=48U;_T1.YY41.val=yy1;_T0=_T1;}return _T0;}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1283 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY42 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY42 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG31,(unsigned char*)_TmpG31,(unsigned char*)_TmpG31 + 24U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY42;_T3=_T2.tag;if(_T3!=49)goto _TL20B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY42;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1286
return _T6;}_TL20B:
 Cyc_yythrowfail(s);;}
# 1290
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY42.tag=49U;_T1.YY42.val=yy1;_T0=_T1;}return _T0;}static char _TmpG32[13U]="designator_t";
# 1284 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY43 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY43 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG32,(unsigned char*)_TmpG32,(unsigned char*)_TmpG32 + 13U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY43;_T3=_T2.tag;if(_T3!=50)goto _TL20D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY43;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1287
return _T6;}_TL20D:
 Cyc_yythrowfail(s);;}
# 1291
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY43.tag=50U;_T1.YY43.val=yy1;_T0=_T1;}return _T0;}static char _TmpG33[7U]="kind_t";
# 1285 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY44 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY44 _T5;struct Cyc_Absyn_Kind*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG33,(unsigned char*)_TmpG33,(unsigned char*)_TmpG33 + 7U};struct Cyc_Absyn_Kind*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY44;_T3=_T2.tag;if(_T3!=51)goto _TL20F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY44;_T7=_T5.val;}{struct Cyc_Absyn_Kind*yy=_T7;_T6=yy;
# 1288
return _T6;}_TL20F:
 Cyc_yythrowfail(s);;}
# 1292
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY44.tag=51U;_T1.YY44.val=yy1;_T0=_T1;}return _T0;}static char _TmpG34[7U]="type_t";
# 1286 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY45 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY45 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG34,(unsigned char*)_TmpG34,(unsigned char*)_TmpG34 + 7U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY45;_T3=_T2.tag;if(_T3!=52)goto _TL211;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY45;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1289
return _T6;}_TL211:
 Cyc_yythrowfail(s);;}
# 1293
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY45.tag=52U;_T1.YY45.val=yy1;_T0=_T1;}return _T0;}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1287 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY46 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY46 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG35,(unsigned char*)_TmpG35,(unsigned char*)_TmpG35 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY46;_T3=_T2.tag;if(_T3!=53)goto _TL213;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY46;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1290
return _T6;}_TL213:
 Cyc_yythrowfail(s);;}
# 1294
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY46.tag=53U;_T1.YY46.val=yy1;_T0=_T1;}return _T0;}static char _TmpG36[12U]="attribute_t";
# 1288 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY47 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY47 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG36,(unsigned char*)_TmpG36,(unsigned char*)_TmpG36 + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY47;_T3=_T2.tag;if(_T3!=54)goto _TL215;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY47;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1291
return _T6;}_TL215:
 Cyc_yythrowfail(s);;}
# 1295
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY47.tag=54U;_T1.YY47.val=yy1;_T0=_T1;}return _T0;}static char _TmpG37[12U]="enumfield_t";
# 1289 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY48 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY48 _T5;struct Cyc_Absyn_Enumfield*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG37,(unsigned char*)_TmpG37,(unsigned char*)_TmpG37 + 12U};struct Cyc_Absyn_Enumfield*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY48;_T3=_T2.tag;if(_T3!=55)goto _TL217;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY48;_T7=_T5.val;}{struct Cyc_Absyn_Enumfield*yy=_T7;_T6=yy;
# 1292
return _T6;}_TL217:
 Cyc_yythrowfail(s);;}
# 1296
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY48.tag=55U;_T1.YY48.val=yy1;_T0=_T1;}return _T0;}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1290 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY49 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY49 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG38,(unsigned char*)_TmpG38,(unsigned char*)_TmpG38 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY49;_T3=_T2.tag;if(_T3!=56)goto _TL219;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY49;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1293
return _T6;}_TL219:
 Cyc_yythrowfail(s);;}
# 1297
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY49.tag=56U;_T1.YY49.val=yy1;_T0=_T1;}return _T0;}static char _TmpG39[11U]="type_opt_t";
# 1291 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY50 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY50 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG39,(unsigned char*)_TmpG39,(unsigned char*)_TmpG39 + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY50;_T3=_T2.tag;if(_T3!=57)goto _TL21B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY50;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1294
return _T6;}_TL21B:
 Cyc_yythrowfail(s);;}
# 1298
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY50.tag=57U;_T1.YY50.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3A[61U]="$(list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)*`H";
# 1293 "parse.y"
static struct _tuple28*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY51 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY51 _T5;struct _tuple28*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3A,(unsigned char*)_TmpG3A,(unsigned char*)_TmpG3A + 61U};struct _tuple28*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY51;_T3=_T2.tag;if(_T3!=58)goto _TL21D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY51;_T7=_T5.val;}{struct _tuple28*yy=_T7;_T6=yy;
# 1296
return _T6;}_TL21D:
 Cyc_yythrowfail(s);;}
# 1300
static union Cyc_YYSTYPE Cyc_YY51(struct _tuple28*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY51.tag=58U;_T1.YY51.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3B[12U]="effconstr_t";
# 1294 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY52 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY52 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3B,(unsigned char*)_TmpG3B,(unsigned char*)_TmpG3B + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY52;_T3=_T2.tag;if(_T3!=59)goto _TL21F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY52;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1297
return _T6;}_TL21F:
 Cyc_yythrowfail(s);;}
# 1301
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY52.tag=59U;_T1.YY52.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3C[21U]="$(type_t, type_t)@`H";
# 1295 "parse.y"
static struct _tuple29*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY53 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY53 _T5;struct _tuple29*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3C,(unsigned char*)_TmpG3C,(unsigned char*)_TmpG3C + 21U};struct _tuple29*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY53;_T3=_T2.tag;if(_T3!=60)goto _TL221;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY53;_T7=_T5.val;}{struct _tuple29*yy=_T7;_T6=yy;
# 1298
return _T6;}_TL221:
 Cyc_yythrowfail(s);;}
# 1302
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple29*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY53.tag=60U;_T1.YY53.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3D[11U]="booltype_t";
# 1296 "parse.y"
static void*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY54 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY54 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3D,(unsigned char*)_TmpG3D,(unsigned char*)_TmpG3D + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY54;_T3=_T2.tag;if(_T3!=61)goto _TL223;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY54;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1299
return _T6;}_TL223:
 Cyc_yythrowfail(s);;}
# 1303
static union Cyc_YYSTYPE Cyc_YY54(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY54.tag=61U;_T1.YY54.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3E[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1297 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY55 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY55 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3E,(unsigned char*)_TmpG3E,(unsigned char*)_TmpG3E + 35U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY55;_T3=_T2.tag;if(_T3!=62)goto _TL225;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY55;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1300
return _T6;}_TL225:
 Cyc_yythrowfail(s);;}
# 1304
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY55.tag=62U;_T1.YY55.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3F[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1298 "parse.y"
static struct _tuple30*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY56 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY56 _T5;struct _tuple30*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3F,(unsigned char*)_TmpG3F,(unsigned char*)_TmpG3F + 48U};struct _tuple30*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY56;_T3=_T2.tag;if(_T3!=63)goto _TL227;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY56;_T7=_T5.val;}{struct _tuple30*yy=_T7;_T6=yy;
# 1301
return _T6;}_TL227:
 Cyc_yythrowfail(s);;}
# 1305
static union Cyc_YYSTYPE Cyc_YY56(struct _tuple30*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY56.tag=63U;_T1.YY56.val=yy1;_T0=_T1;}return _T0;}static char _TmpG40[18U]="list_t<qvar_t,`H>";
# 1299 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY57 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY57 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG40,(unsigned char*)_TmpG40,(unsigned char*)_TmpG40 + 18U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY57;_T3=_T2.tag;if(_T3!=64)goto _TL229;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY57;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1302
return _T6;}_TL229:
 Cyc_yythrowfail(s);;}
# 1306
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY57.tag=64U;_T1.YY57.val=yy1;_T0=_T1;}return _T0;}static char _TmpG41[12U]="aqualtype_t";
# 1300 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY58 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY58 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG41,(unsigned char*)_TmpG41,(unsigned char*)_TmpG41 + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY58;_T3=_T2.tag;if(_T3!=65)goto _TL22B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY58;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1303
return _T6;}_TL22B:
 Cyc_yythrowfail(s);;}
# 1307
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY58.tag=65U;_T1.YY58.val=yy1;_T0=_T1;}return _T0;}static char _TmpG42[20U]="pointer_qual_t<`yy>";
# 1301 "parse.y"
static void*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY59 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY59 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG42,(unsigned char*)_TmpG42,(unsigned char*)_TmpG42 + 20U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY59;_T3=_T2.tag;if(_T3!=66)goto _TL22D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY59;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1304
return _T6;}_TL22D:
 Cyc_yythrowfail(s);;}
# 1308
static union Cyc_YYSTYPE Cyc_YY59(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY59.tag=66U;_T1.YY59.val=yy1;_T0=_T1;}return _T0;}static char _TmpG43[21U]="pointer_quals_t<`yy>";
# 1302 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY60 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY60 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG43,(unsigned char*)_TmpG43,(unsigned char*)_TmpG43 + 21U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY60;_T3=_T2.tag;if(_T3!=67)goto _TL22F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY60;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1305
return _T6;}_TL22F:
 Cyc_yythrowfail(s);;}
# 1309
static union Cyc_YYSTYPE Cyc_YY60(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY60.tag=67U;_T1.YY60.val=yy1;_T0=_T1;}return _T0;}static char _TmpG44[10U]="exp_opt_t";
# 1303 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY61 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY61 _T5;struct Cyc_Absyn_Exp*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG44,(unsigned char*)_TmpG44,(unsigned char*)_TmpG44 + 10U};struct Cyc_Absyn_Exp*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY61;_T3=_T2.tag;if(_T3!=68)goto _TL231;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY61;_T7=_T5.val;}{struct Cyc_Absyn_Exp*yy=_T7;_T6=yy;
# 1306
return _T6;}_TL231:
 Cyc_yythrowfail(s);;}
# 1310
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY61.tag=68U;_T1.YY61.val=yy1;_T0=_T1;}return _T0;}static char _TmpG45[43U]="$(exp_opt_t,exp_opt_t,exp_opt_t,exp_opt_t)";
# 1304 "parse.y"
static struct _tuple21 Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY62 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY62 _T5;struct _tuple21 _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG45,(unsigned char*)_TmpG45,(unsigned char*)_TmpG45 + 43U};struct _tuple21 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY62;_T3=_T2.tag;if(_T3!=69)goto _TL233;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY62;_T7=_T5.val;}{struct _tuple21 yy=_T7;_T6=yy;
# 1307
return _T6;}_TL233:
 Cyc_yythrowfail(s);;}
# 1311
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple21 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY62.tag=69U;_T1.YY62.val=yy1;_T0=_T1;}return _T0;}static char _TmpG46[10U]="raw_exp_t";
# 1306 "parse.y"
static void*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY63 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY63 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG46,(unsigned char*)_TmpG46,(unsigned char*)_TmpG46 + 10U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY63;_T3=_T2.tag;if(_T3!=70)goto _TL235;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY63;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1309
return _T6;}_TL235:
 Cyc_yythrowfail(s);;}
# 1313
static union Cyc_YYSTYPE Cyc_YY63(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY63.tag=70U;_T1.YY63.val=yy1;_T0=_T1;}return _T0;}static char _TmpG47[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1307 "parse.y"
static struct _tuple31*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY64 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY64 _T5;struct _tuple31*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG47,(unsigned char*)_TmpG47,(unsigned char*)_TmpG47 + 112U};struct _tuple31*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY64;_T3=_T2.tag;if(_T3!=71)goto _TL237;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY64;_T7=_T5.val;}{struct _tuple31*yy=_T7;_T6=yy;
# 1310
return _T6;}_TL237:
 Cyc_yythrowfail(s);;}
# 1314
static union Cyc_YYSTYPE Cyc_YY64(struct _tuple31*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY64.tag=71U;_T1.YY64.val=yy1;_T0=_T1;}return _T0;}static char _TmpG48[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1308 "parse.y"
static struct _tuple28*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY65 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY65 _T5;struct _tuple28*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG48,(unsigned char*)_TmpG48,(unsigned char*)_TmpG48 + 73U};struct _tuple28*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY65;_T3=_T2.tag;if(_T3!=72)goto _TL239;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY65;_T7=_T5.val;}{struct _tuple28*yy=_T7;_T6=yy;
# 1311
return _T6;}_TL239:
 Cyc_yythrowfail(s);;}
# 1315
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple28*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY65.tag=72U;_T1.YY65.val=yy1;_T0=_T1;}return _T0;}static char _TmpG49[28U]="list_t<string_t<`H>@`H, `H>";
# 1309 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY66 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY66 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG49,(unsigned char*)_TmpG49,(unsigned char*)_TmpG49 + 28U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY66;_T3=_T2.tag;if(_T3!=73)goto _TL23B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY66;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1312
return _T6;}_TL23B:
 Cyc_yythrowfail(s);;}
# 1316
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY66.tag=73U;_T1.YY66.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4A[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1310 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY67(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY67 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY67 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4A,(unsigned char*)_TmpG4A,(unsigned char*)_TmpG4A + 38U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY67;_T3=_T2.tag;if(_T3!=74)goto _TL23D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY67;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1313
return _T6;}_TL23D:
 Cyc_yythrowfail(s);;}
# 1317
static union Cyc_YYSTYPE Cyc_YY67(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY67.tag=74U;_T1.YY67.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4B[26U]="$(string_t<`H>, exp_t)@`H";
# 1311 "parse.y"
static struct _tuple32*Cyc_yyget_YY68(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY68 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY68 _T5;struct _tuple32*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4B,(unsigned char*)_TmpG4B,(unsigned char*)_TmpG4B + 26U};struct _tuple32*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY68;_T3=_T2.tag;if(_T3!=75)goto _TL23F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY68;_T7=_T5.val;}{struct _tuple32*yy=_T7;_T6=yy;
# 1314
return _T6;}_TL23F:
 Cyc_yythrowfail(s);;}
# 1318
static union Cyc_YYSTYPE Cyc_YY68(struct _tuple32*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY68.tag=75U;_T1.YY68.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4C[16U]="exp_maker_fun_t";
# 1312 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY69(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY69 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY69 _T5;struct Cyc_Absyn_Exp*(*_T6)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
static struct _fat_ptr s={(unsigned char*)_TmpG4C,(unsigned char*)_TmpG4C,(unsigned char*)_TmpG4C + 16U};struct Cyc_Absyn_Exp*(*_T7)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY69;_T3=_T2.tag;if(_T3!=76)goto _TL241;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY69;_T7=_T5.val;}{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_T7;_T6=yy;
# 1315
return _T6;}_TL241:
 Cyc_yythrowfail(s);;}
# 1319
static union Cyc_YYSTYPE Cyc_YY69(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY69.tag=76U;_T1.YY69.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4D[19U]="enum ConstraintOps";
# 1314 "parse.y"
static enum Cyc_Parse_ConstraintOps Cyc_yyget_YY70(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY70 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY70 _T5;enum Cyc_Parse_ConstraintOps _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4D,(unsigned char*)_TmpG4D,(unsigned char*)_TmpG4D + 19U};enum Cyc_Parse_ConstraintOps _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY70;_T3=_T2.tag;if(_T3!=77)goto _TL243;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY70;_T7=_T5.val;}{enum Cyc_Parse_ConstraintOps yy=_T7;_T6=yy;
# 1317
return _T6;}_TL243:
 Cyc_yythrowfail(s);;}
# 1321
static union Cyc_YYSTYPE Cyc_YY70(enum Cyc_Parse_ConstraintOps yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY70.tag=77U;_T1.YY70.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4E[60U]="list_t<$(type_t, list_t<BansheeIf::constraint_t,`H>)@`H,`H>";
# 1315 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY71(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY71 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY71 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4E,(unsigned char*)_TmpG4E,(unsigned char*)_TmpG4E + 60U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY71;_T3=_T2.tag;if(_T3!=78)goto _TL245;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY71;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1318
return _T6;}_TL245:
 Cyc_yythrowfail(s);;}
# 1322
static union Cyc_YYSTYPE Cyc_YY71(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY71.tag=78U;_T1.YY71.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4F[35U]="list_t<BansheeIf::constraint_t,`H>";
# 1316 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY72(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY72 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY72 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4F,(unsigned char*)_TmpG4F,(unsigned char*)_TmpG4F + 35U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY72;_T3=_T2.tag;if(_T3!=79)goto _TL247;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY72;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1319
return _T6;}_TL247:
 Cyc_yythrowfail(s);;}
# 1323
static union Cyc_YYSTYPE Cyc_YY72(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY72.tag=79U;_T1.YY72.val=yy1;_T0=_T1;}return _T0;}static char _TmpG50[24U]="BansheeIf::constraint_t";
# 1317 "parse.y"
static void*Cyc_yyget_YY73(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY73 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY73 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG50,(unsigned char*)_TmpG50,(unsigned char*)_TmpG50 + 24U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY73;_T3=_T2.tag;if(_T3!=80)goto _TL249;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY73;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1320
return _T6;}_TL249:
 Cyc_yythrowfail(s);;}
# 1324
static union Cyc_YYSTYPE Cyc_YY73(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY73.tag=80U;_T1.YY73.val=yy1;_T0=_T1;}return _T0;}static char _TmpG51[4U]="int";
# 1319 "parse.y"
static int Cyc_yyget_YY74(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY74 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY74 _T5;int _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG51,(unsigned char*)_TmpG51,(unsigned char*)_TmpG51 + 4U};int _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY74;_T3=_T2.tag;if(_T3!=81)goto _TL24B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY74;_T7=_T5.val;}{int yy=_T7;_T6=yy;
# 1322
return _T6;}_TL24B:
 Cyc_yythrowfail(s);;}
# 1326
static union Cyc_YYSTYPE Cyc_YY74(int yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY74.tag=81U;_T1.YY74.val=yy1;_T0=_T1;}return _T0;}struct Cyc_Yyltype{int timestamp;unsigned first_line;unsigned first_column;unsigned last_line;unsigned last_column;};
# 1342
struct Cyc_Yyltype Cyc_yynewloc (void){struct Cyc_Yyltype _T0;{struct Cyc_Yyltype _T1;
_T1.timestamp=0;_T1.first_line=0U;_T1.first_column=0U;_T1.last_line=0U;_T1.last_column=0U;_T0=_T1;}return _T0;}
# 1345
struct Cyc_Yyltype Cyc_yylloc={0,0U,0U,0U,0U};
# 1356 "parse.y"
static short Cyc_yytranslate[402U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,173,2,2,158,171,168,2,155,156,151,166,150,169,161,170,2,2,2,2,2,2,2,2,2,2,160,147,153,152,154,165,164,174,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,175,2,2,2,2,162,2,163,167,157,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,148,159,149,172,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145,146};static char _TmpG52[2U]="$";static char _TmpG53[6U]="error";static char _TmpG54[12U]="$undefined.";static char _TmpG55[5U]="AUTO";static char _TmpG56[9U]="REGISTER";static char _TmpG57[7U]="STATIC";static char _TmpG58[7U]="EXTERN";static char _TmpG59[8U]="TYPEDEF";static char _TmpG5A[5U]="VOID";static char _TmpG5B[5U]="CHAR";static char _TmpG5C[6U]="SHORT";static char _TmpG5D[4U]="INT";static char _TmpG5E[5U]="LONG";static char _TmpG5F[6U]="FLOAT";static char _TmpG60[7U]="DOUBLE";static char _TmpG61[7U]="SIGNED";static char _TmpG62[9U]="UNSIGNED";static char _TmpG63[6U]="CONST";static char _TmpG64[9U]="VOLATILE";static char _TmpG65[9U]="RESTRICT";static char _TmpG66[7U]="STRUCT";static char _TmpG67[6U]="UNION";static char _TmpG68[5U]="CASE";static char _TmpG69[8U]="DEFAULT";static char _TmpG6A[7U]="INLINE";static char _TmpG6B[7U]="SIZEOF";static char _TmpG6C[9U]="OFFSETOF";static char _TmpG6D[3U]="IF";static char _TmpG6E[5U]="ELSE";static char _TmpG6F[7U]="SWITCH";static char _TmpG70[6U]="WHILE";static char _TmpG71[3U]="DO";static char _TmpG72[4U]="FOR";static char _TmpG73[5U]="GOTO";static char _TmpG74[9U]="CONTINUE";static char _TmpG75[6U]="BREAK";static char _TmpG76[7U]="RETURN";static char _TmpG77[5U]="ENUM";static char _TmpG78[7U]="TYPEOF";static char _TmpG79[16U]="BUILTIN_VA_LIST";static char _TmpG7A[10U]="EXTENSION";static char _TmpG7B[8U]="COMPLEX";static char _TmpG7C[8U]="NULL_kw";static char _TmpG7D[4U]="LET";static char _TmpG7E[6U]="THROW";static char _TmpG7F[4U]="TRY";static char _TmpG80[6U]="CATCH";static char _TmpG81[7U]="EXPORT";static char _TmpG82[9U]="OVERRIDE";static char _TmpG83[5U]="HIDE";static char _TmpG84[4U]="NEW";static char _TmpG85[5U]="QNEW";static char _TmpG86[9U]="ABSTRACT";static char _TmpG87[9U]="FALLTHRU";static char _TmpG88[6U]="USING";static char _TmpG89[10U]="NAMESPACE";static char _TmpG8A[12U]="NOINFERENCE";static char _TmpG8B[9U]="DATATYPE";static char _TmpG8C[7U]="MALLOC";static char _TmpG8D[8U]="RMALLOC";static char _TmpG8E[9U]="RVMALLOC";static char _TmpG8F[15U]="RMALLOC_INLINE";static char _TmpG90[8U]="QMALLOC";static char _TmpG91[7U]="CALLOC";static char _TmpG92[8U]="QCALLOC";static char _TmpG93[8U]="RCALLOC";static char _TmpG94[5U]="SWAP";static char _TmpG95[7U]="ASSERT";static char _TmpG96[9U]="REGION_T";static char _TmpG97[6U]="TAG_T";static char _TmpG98[7U]="REGION";static char _TmpG99[5U]="RNEW";static char _TmpG9A[8U]="REGIONS";static char _TmpG9B[7U]="PORTON";static char _TmpG9C[8U]="PORTOFF";static char _TmpG9D[7U]="PRAGMA";static char _TmpG9E[10U]="TEMPESTON";static char _TmpG9F[11U]="TEMPESTOFF";static char _TmpGA0[13U]="AQ_ALIASABLE";static char _TmpGA1[10U]="AQ_REFCNT";static char _TmpGA2[14U]="AQ_RESTRICTED";static char _TmpGA3[10U]="AQ_UNIQUE";static char _TmpGA4[8U]="AQUAL_T";static char _TmpGA5[8U]="NUMELTS";static char _TmpGA6[6U]="TAGOF";static char _TmpGA7[8U]="VALUEOF";static char _TmpGA8[10U]="VALUEOF_T";static char _TmpGA9[9U]="TAGCHECK";static char _TmpGAA[13U]="NUMELTS_QUAL";static char _TmpGAB[10U]="THIN_QUAL";static char _TmpGAC[9U]="FAT_QUAL";static char _TmpGAD[13U]="NOTNULL_QUAL";static char _TmpGAE[14U]="NULLABLE_QUAL";static char _TmpGAF[14U]="REQUIRES_QUAL";static char _TmpGB0[13U]="ENSURES_QUAL";static char _TmpGB1[12U]="EFFECT_QUAL";static char _TmpGB2[12U]="THROWS_QUAL";static char _TmpGB3[12U]="SUBSET_QUAL";static char _TmpGB4[12U]="REGION_QUAL";static char _TmpGB5[16U]="NOZEROTERM_QUAL";static char _TmpGB6[14U]="ZEROTERM_QUAL";static char _TmpGB7[12U]="TAGGED_QUAL";static char _TmpGB8[12U]="ASSERT_QUAL";static char _TmpGB9[18U]="ASSERT_FALSE_QUAL";static char _TmpGBA[11U]="ALIAS_QUAL";static char _TmpGBB[7U]="AQUALS";static char _TmpGBC[12U]="CHECKS_QUAL";static char _TmpGBD[16U]="EXTENSIBLE_QUAL";static char _TmpGBE[18U]="AUTORELEASED_QUAL";static char _TmpGBF[7U]="PTR_OP";static char _TmpGC0[7U]="INC_OP";static char _TmpGC1[7U]="DEC_OP";static char _TmpGC2[8U]="LEFT_OP";static char _TmpGC3[9U]="RIGHT_OP";static char _TmpGC4[6U]="LE_OP";static char _TmpGC5[6U]="GE_OP";static char _TmpGC6[6U]="EQ_OP";static char _TmpGC7[6U]="NE_OP";static char _TmpGC8[7U]="AND_OP";static char _TmpGC9[6U]="OR_OP";static char _TmpGCA[11U]="MUL_ASSIGN";static char _TmpGCB[11U]="DIV_ASSIGN";static char _TmpGCC[11U]="MOD_ASSIGN";static char _TmpGCD[11U]="ADD_ASSIGN";static char _TmpGCE[11U]="SUB_ASSIGN";static char _TmpGCF[12U]="LEFT_ASSIGN";static char _TmpGD0[13U]="RIGHT_ASSIGN";static char _TmpGD1[11U]="AND_ASSIGN";static char _TmpGD2[11U]="XOR_ASSIGN";static char _TmpGD3[10U]="OR_ASSIGN";static char _TmpGD4[9U]="ELLIPSIS";static char _TmpGD5[11U]="LEFT_RIGHT";static char _TmpGD6[12U]="COLON_COLON";static char _TmpGD7[11U]="IDENTIFIER";static char _TmpGD8[17U]="INTEGER_CONSTANT";static char _TmpGD9[7U]="STRING";static char _TmpGDA[8U]="WSTRING";static char _TmpGDB[19U]="CHARACTER_CONSTANT";static char _TmpGDC[20U]="WCHARACTER_CONSTANT";static char _TmpGDD[18U]="FLOATING_CONSTANT";static char _TmpGDE[9U]="TYPE_VAR";static char _TmpGDF[13U]="TYPEDEF_NAME";static char _TmpGE0[16U]="QUAL_IDENTIFIER";static char _TmpGE1[18U]="QUAL_TYPEDEF_NAME";static char _TmpGE2[18U]="AQUAL_SHORT_CONST";static char _TmpGE3[10U]="ATTRIBUTE";static char _TmpGE4[8U]="ASM_TOK";static char _TmpGE5[4U]="';'";static char _TmpGE6[4U]="'{'";static char _TmpGE7[4U]="'}'";static char _TmpGE8[4U]="','";static char _TmpGE9[4U]="'*'";static char _TmpGEA[4U]="'='";static char _TmpGEB[4U]="'<'";static char _TmpGEC[4U]="'>'";static char _TmpGED[4U]="'('";static char _TmpGEE[4U]="')'";static char _TmpGEF[4U]="'_'";static char _TmpGF0[4U]="'$'";static char _TmpGF1[4U]="'|'";static char _TmpGF2[4U]="':'";static char _TmpGF3[4U]="'.'";static char _TmpGF4[4U]="'['";static char _TmpGF5[4U]="']'";static char _TmpGF6[4U]="'@'";static char _TmpGF7[4U]="'?'";static char _TmpGF8[4U]="'+'";static char _TmpGF9[4U]="'^'";static char _TmpGFA[4U]="'&'";static char _TmpGFB[4U]="'-'";static char _TmpGFC[4U]="'/'";static char _TmpGFD[4U]="'%'";static char _TmpGFE[4U]="'~'";static char _TmpGFF[4U]="'!'";static char _TmpG100[4U]="'A'";static char _TmpG101[4U]="'V'";static char _TmpG102[20U]="prog_or_constraints";static char _TmpG103[5U]="prog";static char _TmpG104[17U]="translation_unit";static char _TmpG105[18U]="tempest_on_action";static char _TmpG106[19U]="tempest_off_action";static char _TmpG107[16U]="extern_c_action";static char _TmpG108[13U]="end_extern_c";static char _TmpG109[14U]="hide_list_opt";static char _TmpG10A[17U]="hide_list_values";static char _TmpG10B[16U]="export_list_opt";static char _TmpG10C[12U]="export_list";static char _TmpG10D[19U]="export_list_values";static char _TmpG10E[13U]="override_opt";static char _TmpG10F[21U]="external_declaration";static char _TmpG110[14U]="optional_semi";static char _TmpG111[20U]="function_definition";static char _TmpG112[21U]="function_definition2";static char _TmpG113[13U]="using_action";static char _TmpG114[15U]="unusing_action";static char _TmpG115[17U]="namespace_action";static char _TmpG116[19U]="unnamespace_action";static char _TmpG117[19U]="noinference_action";static char _TmpG118[21U]="unnoinference_action";static char _TmpG119[12U]="declaration";static char _TmpG11A[9U]="open_opt";static char _TmpG11B[17U]="declaration_list";static char _TmpG11C[23U]="declaration_specifiers";static char _TmpG11D[24U]="storage_class_specifier";static char _TmpG11E[15U]="attributes_opt";static char _TmpG11F[11U]="attributes";static char _TmpG120[15U]="attribute_list";static char _TmpG121[10U]="attribute";static char _TmpG122[15U]="type_specifier";static char _TmpG123[25U]="type_specifier_notypedef";static char _TmpG124[5U]="kind";static char _TmpG125[15U]="type_qualifier";static char _TmpG126[15U]="enum_specifier";static char _TmpG127[11U]="enum_field";static char _TmpG128[22U]="enum_declaration_list";static char _TmpG129[26U]="struct_or_union_specifier";static char _TmpG12A[26U]="maybe_tagged_struct_union";static char _TmpG12B[16U]="struct_or_union";static char _TmpG12C[16U]="type_params_opt";static char _TmpG12D[24U]="struct_declaration_list";static char _TmpG12E[25U]="struct_declaration_list0";static char _TmpG12F[25U]="init_declarator_list_rev";static char _TmpG130[16U]="init_declarator";static char _TmpG131[19U]="struct_declaration";static char _TmpG132[25U]="specifier_qualifier_list";static char _TmpG133[35U]="notypedef_specifier_qualifier_list";static char _TmpG134[27U]="struct_declarator_list_rev";static char _TmpG135[18U]="struct_declarator";static char _TmpG136[20U]="requires_clause_opt";static char _TmpG137[19U]="datatype_specifier";static char _TmpG138[14U]="qual_datatype";static char _TmpG139[19U]="datatypefield_list";static char _TmpG13A[20U]="datatypefield_scope";static char _TmpG13B[14U]="datatypefield";static char _TmpG13C[11U]="declarator";static char _TmpG13D[23U]="declarator_withtypedef";static char _TmpG13E[18U]="direct_declarator";static char _TmpG13F[30U]="direct_declarator_withtypedef";static char _TmpG140[8U]="pointer";static char _TmpG141[12U]="one_pointer";static char _TmpG142[14U]="pointer_quals";static char _TmpG143[13U]="pointer_qual";static char _TmpG144[16U]="aqual_specifier";static char _TmpG145[23U]="pointer_null_and_bound";static char _TmpG146[14U]="pointer_bound";static char _TmpG147[18U]="zeroterm_qual_opt";static char _TmpG148[8U]="eff_set";static char _TmpG149[8U]="eff_opt";static char _TmpG14A[11U]="tqual_list";static char _TmpG14B[20U]="parameter_type_list";static char _TmpG14C[14U]="opt_aqual_bnd";static char _TmpG14D[9U]="type_var";static char _TmpG14E[16U]="optional_effect";static char _TmpG14F[27U]="optional_effconstr_qualbnd";static char _TmpG150[18U]="effconstr_qualbnd";static char _TmpG151[14U]="effconstr_elt";static char _TmpG152[13U]="qual_bnd_elt";static char _TmpG153[12U]="aqual_const";static char _TmpG154[15U]="qual_bnd_const";static char _TmpG155[14U]="qual_bnd_term";static char _TmpG156[16U]="optional_inject";static char _TmpG157[11U]="effect_set";static char _TmpG158[14U]="atomic_effect";static char _TmpG159[11U]="region_set";static char _TmpG15A[15U]="parameter_list";static char _TmpG15B[22U]="parameter_declaration";static char _TmpG15C[16U]="identifier_list";static char _TmpG15D[17U]="identifier_list0";static char _TmpG15E[12U]="initializer";static char _TmpG15F[18U]="array_initializer";static char _TmpG160[17U]="initializer_list";static char _TmpG161[12U]="designation";static char _TmpG162[16U]="designator_list";static char _TmpG163[11U]="designator";static char _TmpG164[10U]="type_name";static char _TmpG165[14U]="any_type_name";static char _TmpG166[15U]="type_name_list";static char _TmpG167[20U]="abstract_declarator";static char _TmpG168[27U]="direct_abstract_declarator";static char _TmpG169[16U]="chk_req_ens_thr";static char _TmpG16A[20U]="chk_req_ens_thr_opt";static char _TmpG16B[10U]="statement";static char _TmpG16C[18U]="labeled_statement";static char _TmpG16D[21U]="expression_statement";static char _TmpG16E[18U]="start_fndef_block";static char _TmpG16F[16U]="end_fndef_block";static char _TmpG170[25U]="fndef_compound_statement";static char _TmpG171[19U]="compound_statement";static char _TmpG172[16U]="block_item_list";static char _TmpG173[20U]="selection_statement";static char _TmpG174[15U]="switch_clauses";static char _TmpG175[20U]="iteration_statement";static char _TmpG176[12U]="for_exp_opt";static char _TmpG177[15U]="jump_statement";static char _TmpG178[12U]="exp_pattern";static char _TmpG179[20U]="conditional_pattern";static char _TmpG17A[19U]="logical_or_pattern";static char _TmpG17B[20U]="logical_and_pattern";static char _TmpG17C[21U]="inclusive_or_pattern";static char _TmpG17D[21U]="exclusive_or_pattern";static char _TmpG17E[12U]="and_pattern";static char _TmpG17F[17U]="equality_pattern";static char _TmpG180[19U]="relational_pattern";static char _TmpG181[14U]="shift_pattern";static char _TmpG182[17U]="additive_pattern";static char _TmpG183[23U]="multiplicative_pattern";static char _TmpG184[13U]="cast_pattern";static char _TmpG185[14U]="unary_pattern";static char _TmpG186[16U]="postfix_pattern";static char _TmpG187[16U]="primary_pattern";static char _TmpG188[8U]="pattern";static char _TmpG189[19U]="tuple_pattern_list";static char _TmpG18A[20U]="tuple_pattern_list0";static char _TmpG18B[14U]="field_pattern";static char _TmpG18C[19U]="field_pattern_list";static char _TmpG18D[20U]="field_pattern_list0";static char _TmpG18E[11U]="expression";static char _TmpG18F[22U]="assignment_expression";static char _TmpG190[20U]="assignment_operator";static char _TmpG191[23U]="conditional_expression";static char _TmpG192[20U]="constant_expression";static char _TmpG193[22U]="logical_or_expression";static char _TmpG194[23U]="logical_and_expression";static char _TmpG195[24U]="inclusive_or_expression";static char _TmpG196[24U]="exclusive_or_expression";static char _TmpG197[15U]="and_expression";static char _TmpG198[20U]="equality_expression";static char _TmpG199[22U]="relational_expression";static char _TmpG19A[17U]="shift_expression";static char _TmpG19B[20U]="additive_expression";static char _TmpG19C[26U]="multiplicative_expression";static char _TmpG19D[12U]="equality_op";static char _TmpG19E[14U]="relational_op";static char _TmpG19F[12U]="additive_op";static char _TmpG1A0[18U]="multiplicative_op";static char _TmpG1A1[16U]="cast_expression";static char _TmpG1A2[17U]="unary_expression";static char _TmpG1A3[15U]="unary_operator";static char _TmpG1A4[9U]="asm_expr";static char _TmpG1A5[13U]="volatile_opt";static char _TmpG1A6[12U]="asm_out_opt";static char _TmpG1A7[12U]="asm_outlist";static char _TmpG1A8[11U]="asm_in_opt";static char _TmpG1A9[11U]="asm_inlist";static char _TmpG1AA[11U]="asm_io_elt";static char _TmpG1AB[16U]="asm_clobber_opt";static char _TmpG1AC[17U]="asm_clobber_list";static char _TmpG1AD[19U]="postfix_expression";static char _TmpG1AE[17U]="field_expression";static char _TmpG1AF[19U]="primary_expression";static char _TmpG1B0[25U]="argument_expression_list";static char _TmpG1B1[26U]="argument_expression_list0";static char _TmpG1B2[9U]="constant";static char _TmpG1B3[20U]="qual_opt_identifier";static char _TmpG1B4[17U]="qual_opt_typedef";static char _TmpG1B5[18U]="struct_union_name";static char _TmpG1B6[11U]="field_name";static char _TmpG1B7[12U]="right_angle";static char _TmpG1B8[16U]="all_constraints";static char _TmpG1B9[20U]="constraint_list_opt";static char _TmpG1BA[16U]="constraint_list";static char _TmpG1BB[15U]="tvar_or_string";static char _TmpG1BC[11U]="constraint";static char _TmpG1BD[5U]="c_op";
# 1759 "parse.y"
static struct _fat_ptr Cyc_yytname[364U]={{(unsigned char*)_TmpG52,(unsigned char*)_TmpG52,(unsigned char*)_TmpG52 + 2U},{(unsigned char*)_TmpG53,(unsigned char*)_TmpG53,(unsigned char*)_TmpG53 + 6U},{(unsigned char*)_TmpG54,(unsigned char*)_TmpG54,(unsigned char*)_TmpG54 + 12U},{(unsigned char*)_TmpG55,(unsigned char*)_TmpG55,(unsigned char*)_TmpG55 + 5U},{(unsigned char*)_TmpG56,(unsigned char*)_TmpG56,(unsigned char*)_TmpG56 + 9U},{(unsigned char*)_TmpG57,(unsigned char*)_TmpG57,(unsigned char*)_TmpG57 + 7U},{(unsigned char*)_TmpG58,(unsigned char*)_TmpG58,(unsigned char*)_TmpG58 + 7U},{(unsigned char*)_TmpG59,(unsigned char*)_TmpG59,(unsigned char*)_TmpG59 + 8U},{(unsigned char*)_TmpG5A,(unsigned char*)_TmpG5A,(unsigned char*)_TmpG5A + 5U},{(unsigned char*)_TmpG5B,(unsigned char*)_TmpG5B,(unsigned char*)_TmpG5B + 5U},{(unsigned char*)_TmpG5C,(unsigned char*)_TmpG5C,(unsigned char*)_TmpG5C + 6U},{(unsigned char*)_TmpG5D,(unsigned char*)_TmpG5D,(unsigned char*)_TmpG5D + 4U},{(unsigned char*)_TmpG5E,(unsigned char*)_TmpG5E,(unsigned char*)_TmpG5E + 5U},{(unsigned char*)_TmpG5F,(unsigned char*)_TmpG5F,(unsigned char*)_TmpG5F + 6U},{(unsigned char*)_TmpG60,(unsigned char*)_TmpG60,(unsigned char*)_TmpG60 + 7U},{(unsigned char*)_TmpG61,(unsigned char*)_TmpG61,(unsigned char*)_TmpG61 + 7U},{(unsigned char*)_TmpG62,(unsigned char*)_TmpG62,(unsigned char*)_TmpG62 + 9U},{(unsigned char*)_TmpG63,(unsigned char*)_TmpG63,(unsigned char*)_TmpG63 + 6U},{(unsigned char*)_TmpG64,(unsigned char*)_TmpG64,(unsigned char*)_TmpG64 + 9U},{(unsigned char*)_TmpG65,(unsigned char*)_TmpG65,(unsigned char*)_TmpG65 + 9U},{(unsigned char*)_TmpG66,(unsigned char*)_TmpG66,(unsigned char*)_TmpG66 + 7U},{(unsigned char*)_TmpG67,(unsigned char*)_TmpG67,(unsigned char*)_TmpG67 + 6U},{(unsigned char*)_TmpG68,(unsigned char*)_TmpG68,(unsigned char*)_TmpG68 + 5U},{(unsigned char*)_TmpG69,(unsigned char*)_TmpG69,(unsigned char*)_TmpG69 + 8U},{(unsigned char*)_TmpG6A,(unsigned char*)_TmpG6A,(unsigned char*)_TmpG6A + 7U},{(unsigned char*)_TmpG6B,(unsigned char*)_TmpG6B,(unsigned char*)_TmpG6B + 7U},{(unsigned char*)_TmpG6C,(unsigned char*)_TmpG6C,(unsigned char*)_TmpG6C + 9U},{(unsigned char*)_TmpG6D,(unsigned char*)_TmpG6D,(unsigned char*)_TmpG6D + 3U},{(unsigned char*)_TmpG6E,(unsigned char*)_TmpG6E,(unsigned char*)_TmpG6E + 5U},{(unsigned char*)_TmpG6F,(unsigned char*)_TmpG6F,(unsigned char*)_TmpG6F + 7U},{(unsigned char*)_TmpG70,(unsigned char*)_TmpG70,(unsigned char*)_TmpG70 + 6U},{(unsigned char*)_TmpG71,(unsigned char*)_TmpG71,(unsigned char*)_TmpG71 + 3U},{(unsigned char*)_TmpG72,(unsigned char*)_TmpG72,(unsigned char*)_TmpG72 + 4U},{(unsigned char*)_TmpG73,(unsigned char*)_TmpG73,(unsigned char*)_TmpG73 + 5U},{(unsigned char*)_TmpG74,(unsigned char*)_TmpG74,(unsigned char*)_TmpG74 + 9U},{(unsigned char*)_TmpG75,(unsigned char*)_TmpG75,(unsigned char*)_TmpG75 + 6U},{(unsigned char*)_TmpG76,(unsigned char*)_TmpG76,(unsigned char*)_TmpG76 + 7U},{(unsigned char*)_TmpG77,(unsigned char*)_TmpG77,(unsigned char*)_TmpG77 + 5U},{(unsigned char*)_TmpG78,(unsigned char*)_TmpG78,(unsigned char*)_TmpG78 + 7U},{(unsigned char*)_TmpG79,(unsigned char*)_TmpG79,(unsigned char*)_TmpG79 + 16U},{(unsigned char*)_TmpG7A,(unsigned char*)_TmpG7A,(unsigned char*)_TmpG7A + 10U},{(unsigned char*)_TmpG7B,(unsigned char*)_TmpG7B,(unsigned char*)_TmpG7B + 8U},{(unsigned char*)_TmpG7C,(unsigned char*)_TmpG7C,(unsigned char*)_TmpG7C + 8U},{(unsigned char*)_TmpG7D,(unsigned char*)_TmpG7D,(unsigned char*)_TmpG7D + 4U},{(unsigned char*)_TmpG7E,(unsigned char*)_TmpG7E,(unsigned char*)_TmpG7E + 6U},{(unsigned char*)_TmpG7F,(unsigned char*)_TmpG7F,(unsigned char*)_TmpG7F + 4U},{(unsigned char*)_TmpG80,(unsigned char*)_TmpG80,(unsigned char*)_TmpG80 + 6U},{(unsigned char*)_TmpG81,(unsigned char*)_TmpG81,(unsigned char*)_TmpG81 + 7U},{(unsigned char*)_TmpG82,(unsigned char*)_TmpG82,(unsigned char*)_TmpG82 + 9U},{(unsigned char*)_TmpG83,(unsigned char*)_TmpG83,(unsigned char*)_TmpG83 + 5U},{(unsigned char*)_TmpG84,(unsigned char*)_TmpG84,(unsigned char*)_TmpG84 + 4U},{(unsigned char*)_TmpG85,(unsigned char*)_TmpG85,(unsigned char*)_TmpG85 + 5U},{(unsigned char*)_TmpG86,(unsigned char*)_TmpG86,(unsigned char*)_TmpG86 + 9U},{(unsigned char*)_TmpG87,(unsigned char*)_TmpG87,(unsigned char*)_TmpG87 + 9U},{(unsigned char*)_TmpG88,(unsigned char*)_TmpG88,(unsigned char*)_TmpG88 + 6U},{(unsigned char*)_TmpG89,(unsigned char*)_TmpG89,(unsigned char*)_TmpG89 + 10U},{(unsigned char*)_TmpG8A,(unsigned char*)_TmpG8A,(unsigned char*)_TmpG8A + 12U},{(unsigned char*)_TmpG8B,(unsigned char*)_TmpG8B,(unsigned char*)_TmpG8B + 9U},{(unsigned char*)_TmpG8C,(unsigned char*)_TmpG8C,(unsigned char*)_TmpG8C + 7U},{(unsigned char*)_TmpG8D,(unsigned char*)_TmpG8D,(unsigned char*)_TmpG8D + 8U},{(unsigned char*)_TmpG8E,(unsigned char*)_TmpG8E,(unsigned char*)_TmpG8E + 9U},{(unsigned char*)_TmpG8F,(unsigned char*)_TmpG8F,(unsigned char*)_TmpG8F + 15U},{(unsigned char*)_TmpG90,(unsigned char*)_TmpG90,(unsigned char*)_TmpG90 + 8U},{(unsigned char*)_TmpG91,(unsigned char*)_TmpG91,(unsigned char*)_TmpG91 + 7U},{(unsigned char*)_TmpG92,(unsigned char*)_TmpG92,(unsigned char*)_TmpG92 + 8U},{(unsigned char*)_TmpG93,(unsigned char*)_TmpG93,(unsigned char*)_TmpG93 + 8U},{(unsigned char*)_TmpG94,(unsigned char*)_TmpG94,(unsigned char*)_TmpG94 + 5U},{(unsigned char*)_TmpG95,(unsigned char*)_TmpG95,(unsigned char*)_TmpG95 + 7U},{(unsigned char*)_TmpG96,(unsigned char*)_TmpG96,(unsigned char*)_TmpG96 + 9U},{(unsigned char*)_TmpG97,(unsigned char*)_TmpG97,(unsigned char*)_TmpG97 + 6U},{(unsigned char*)_TmpG98,(unsigned char*)_TmpG98,(unsigned char*)_TmpG98 + 7U},{(unsigned char*)_TmpG99,(unsigned char*)_TmpG99,(unsigned char*)_TmpG99 + 5U},{(unsigned char*)_TmpG9A,(unsigned char*)_TmpG9A,(unsigned char*)_TmpG9A + 8U},{(unsigned char*)_TmpG9B,(unsigned char*)_TmpG9B,(unsigned char*)_TmpG9B + 7U},{(unsigned char*)_TmpG9C,(unsigned char*)_TmpG9C,(unsigned char*)_TmpG9C + 8U},{(unsigned char*)_TmpG9D,(unsigned char*)_TmpG9D,(unsigned char*)_TmpG9D + 7U},{(unsigned char*)_TmpG9E,(unsigned char*)_TmpG9E,(unsigned char*)_TmpG9E + 10U},{(unsigned char*)_TmpG9F,(unsigned char*)_TmpG9F,(unsigned char*)_TmpG9F + 11U},{(unsigned char*)_TmpGA0,(unsigned char*)_TmpGA0,(unsigned char*)_TmpGA0 + 13U},{(unsigned char*)_TmpGA1,(unsigned char*)_TmpGA1,(unsigned char*)_TmpGA1 + 10U},{(unsigned char*)_TmpGA2,(unsigned char*)_TmpGA2,(unsigned char*)_TmpGA2 + 14U},{(unsigned char*)_TmpGA3,(unsigned char*)_TmpGA3,(unsigned char*)_TmpGA3 + 10U},{(unsigned char*)_TmpGA4,(unsigned char*)_TmpGA4,(unsigned char*)_TmpGA4 + 8U},{(unsigned char*)_TmpGA5,(unsigned char*)_TmpGA5,(unsigned char*)_TmpGA5 + 8U},{(unsigned char*)_TmpGA6,(unsigned char*)_TmpGA6,(unsigned char*)_TmpGA6 + 6U},{(unsigned char*)_TmpGA7,(unsigned char*)_TmpGA7,(unsigned char*)_TmpGA7 + 8U},{(unsigned char*)_TmpGA8,(unsigned char*)_TmpGA8,(unsigned char*)_TmpGA8 + 10U},{(unsigned char*)_TmpGA9,(unsigned char*)_TmpGA9,(unsigned char*)_TmpGA9 + 9U},{(unsigned char*)_TmpGAA,(unsigned char*)_TmpGAA,(unsigned char*)_TmpGAA + 13U},{(unsigned char*)_TmpGAB,(unsigned char*)_TmpGAB,(unsigned char*)_TmpGAB + 10U},{(unsigned char*)_TmpGAC,(unsigned char*)_TmpGAC,(unsigned char*)_TmpGAC + 9U},{(unsigned char*)_TmpGAD,(unsigned char*)_TmpGAD,(unsigned char*)_TmpGAD + 13U},{(unsigned char*)_TmpGAE,(unsigned char*)_TmpGAE,(unsigned char*)_TmpGAE + 14U},{(unsigned char*)_TmpGAF,(unsigned char*)_TmpGAF,(unsigned char*)_TmpGAF + 14U},{(unsigned char*)_TmpGB0,(unsigned char*)_TmpGB0,(unsigned char*)_TmpGB0 + 13U},{(unsigned char*)_TmpGB1,(unsigned char*)_TmpGB1,(unsigned char*)_TmpGB1 + 12U},{(unsigned char*)_TmpGB2,(unsigned char*)_TmpGB2,(unsigned char*)_TmpGB2 + 12U},{(unsigned char*)_TmpGB3,(unsigned char*)_TmpGB3,(unsigned char*)_TmpGB3 + 12U},{(unsigned char*)_TmpGB4,(unsigned char*)_TmpGB4,(unsigned char*)_TmpGB4 + 12U},{(unsigned char*)_TmpGB5,(unsigned char*)_TmpGB5,(unsigned char*)_TmpGB5 + 16U},{(unsigned char*)_TmpGB6,(unsigned char*)_TmpGB6,(unsigned char*)_TmpGB6 + 14U},{(unsigned char*)_TmpGB7,(unsigned char*)_TmpGB7,(unsigned char*)_TmpGB7 + 12U},{(unsigned char*)_TmpGB8,(unsigned char*)_TmpGB8,(unsigned char*)_TmpGB8 + 12U},{(unsigned char*)_TmpGB9,(unsigned char*)_TmpGB9,(unsigned char*)_TmpGB9 + 18U},{(unsigned char*)_TmpGBA,(unsigned char*)_TmpGBA,(unsigned char*)_TmpGBA + 11U},{(unsigned char*)_TmpGBB,(unsigned char*)_TmpGBB,(unsigned char*)_TmpGBB + 7U},{(unsigned char*)_TmpGBC,(unsigned char*)_TmpGBC,(unsigned char*)_TmpGBC + 12U},{(unsigned char*)_TmpGBD,(unsigned char*)_TmpGBD,(unsigned char*)_TmpGBD + 16U},{(unsigned char*)_TmpGBE,(unsigned char*)_TmpGBE,(unsigned char*)_TmpGBE + 18U},{(unsigned char*)_TmpGBF,(unsigned char*)_TmpGBF,(unsigned char*)_TmpGBF + 7U},{(unsigned char*)_TmpGC0,(unsigned char*)_TmpGC0,(unsigned char*)_TmpGC0 + 7U},{(unsigned char*)_TmpGC1,(unsigned char*)_TmpGC1,(unsigned char*)_TmpGC1 + 7U},{(unsigned char*)_TmpGC2,(unsigned char*)_TmpGC2,(unsigned char*)_TmpGC2 + 8U},{(unsigned char*)_TmpGC3,(unsigned char*)_TmpGC3,(unsigned char*)_TmpGC3 + 9U},{(unsigned char*)_TmpGC4,(unsigned char*)_TmpGC4,(unsigned char*)_TmpGC4 + 6U},{(unsigned char*)_TmpGC5,(unsigned char*)_TmpGC5,(unsigned char*)_TmpGC5 + 6U},{(unsigned char*)_TmpGC6,(unsigned char*)_TmpGC6,(unsigned char*)_TmpGC6 + 6U},{(unsigned char*)_TmpGC7,(unsigned char*)_TmpGC7,(unsigned char*)_TmpGC7 + 6U},{(unsigned char*)_TmpGC8,(unsigned char*)_TmpGC8,(unsigned char*)_TmpGC8 + 7U},{(unsigned char*)_TmpGC9,(unsigned char*)_TmpGC9,(unsigned char*)_TmpGC9 + 6U},{(unsigned char*)_TmpGCA,(unsigned char*)_TmpGCA,(unsigned char*)_TmpGCA + 11U},{(unsigned char*)_TmpGCB,(unsigned char*)_TmpGCB,(unsigned char*)_TmpGCB + 11U},{(unsigned char*)_TmpGCC,(unsigned char*)_TmpGCC,(unsigned char*)_TmpGCC + 11U},{(unsigned char*)_TmpGCD,(unsigned char*)_TmpGCD,(unsigned char*)_TmpGCD + 11U},{(unsigned char*)_TmpGCE,(unsigned char*)_TmpGCE,(unsigned char*)_TmpGCE + 11U},{(unsigned char*)_TmpGCF,(unsigned char*)_TmpGCF,(unsigned char*)_TmpGCF + 12U},{(unsigned char*)_TmpGD0,(unsigned char*)_TmpGD0,(unsigned char*)_TmpGD0 + 13U},{(unsigned char*)_TmpGD1,(unsigned char*)_TmpGD1,(unsigned char*)_TmpGD1 + 11U},{(unsigned char*)_TmpGD2,(unsigned char*)_TmpGD2,(unsigned char*)_TmpGD2 + 11U},{(unsigned char*)_TmpGD3,(unsigned char*)_TmpGD3,(unsigned char*)_TmpGD3 + 10U},{(unsigned char*)_TmpGD4,(unsigned char*)_TmpGD4,(unsigned char*)_TmpGD4 + 9U},{(unsigned char*)_TmpGD5,(unsigned char*)_TmpGD5,(unsigned char*)_TmpGD5 + 11U},{(unsigned char*)_TmpGD6,(unsigned char*)_TmpGD6,(unsigned char*)_TmpGD6 + 12U},{(unsigned char*)_TmpGD7,(unsigned char*)_TmpGD7,(unsigned char*)_TmpGD7 + 11U},{(unsigned char*)_TmpGD8,(unsigned char*)_TmpGD8,(unsigned char*)_TmpGD8 + 17U},{(unsigned char*)_TmpGD9,(unsigned char*)_TmpGD9,(unsigned char*)_TmpGD9 + 7U},{(unsigned char*)_TmpGDA,(unsigned char*)_TmpGDA,(unsigned char*)_TmpGDA + 8U},{(unsigned char*)_TmpGDB,(unsigned char*)_TmpGDB,(unsigned char*)_TmpGDB + 19U},{(unsigned char*)_TmpGDC,(unsigned char*)_TmpGDC,(unsigned char*)_TmpGDC + 20U},{(unsigned char*)_TmpGDD,(unsigned char*)_TmpGDD,(unsigned char*)_TmpGDD + 18U},{(unsigned char*)_TmpGDE,(unsigned char*)_TmpGDE,(unsigned char*)_TmpGDE + 9U},{(unsigned char*)_TmpGDF,(unsigned char*)_TmpGDF,(unsigned char*)_TmpGDF + 13U},{(unsigned char*)_TmpGE0,(unsigned char*)_TmpGE0,(unsigned char*)_TmpGE0 + 16U},{(unsigned char*)_TmpGE1,(unsigned char*)_TmpGE1,(unsigned char*)_TmpGE1 + 18U},{(unsigned char*)_TmpGE2,(unsigned char*)_TmpGE2,(unsigned char*)_TmpGE2 + 18U},{(unsigned char*)_TmpGE3,(unsigned char*)_TmpGE3,(unsigned char*)_TmpGE3 + 10U},{(unsigned char*)_TmpGE4,(unsigned char*)_TmpGE4,(unsigned char*)_TmpGE4 + 8U},{(unsigned char*)_TmpGE5,(unsigned char*)_TmpGE5,(unsigned char*)_TmpGE5 + 4U},{(unsigned char*)_TmpGE6,(unsigned char*)_TmpGE6,(unsigned char*)_TmpGE6 + 4U},{(unsigned char*)_TmpGE7,(unsigned char*)_TmpGE7,(unsigned char*)_TmpGE7 + 4U},{(unsigned char*)_TmpGE8,(unsigned char*)_TmpGE8,(unsigned char*)_TmpGE8 + 4U},{(unsigned char*)_TmpGE9,(unsigned char*)_TmpGE9,(unsigned char*)_TmpGE9 + 4U},{(unsigned char*)_TmpGEA,(unsigned char*)_TmpGEA,(unsigned char*)_TmpGEA + 4U},{(unsigned char*)_TmpGEB,(unsigned char*)_TmpGEB,(unsigned char*)_TmpGEB + 4U},{(unsigned char*)_TmpGEC,(unsigned char*)_TmpGEC,(unsigned char*)_TmpGEC + 4U},{(unsigned char*)_TmpGED,(unsigned char*)_TmpGED,(unsigned char*)_TmpGED + 4U},{(unsigned char*)_TmpGEE,(unsigned char*)_TmpGEE,(unsigned char*)_TmpGEE + 4U},{(unsigned char*)_TmpGEF,(unsigned char*)_TmpGEF,(unsigned char*)_TmpGEF + 4U},{(unsigned char*)_TmpGF0,(unsigned char*)_TmpGF0,(unsigned char*)_TmpGF0 + 4U},{(unsigned char*)_TmpGF1,(unsigned char*)_TmpGF1,(unsigned char*)_TmpGF1 + 4U},{(unsigned char*)_TmpGF2,(unsigned char*)_TmpGF2,(unsigned char*)_TmpGF2 + 4U},{(unsigned char*)_TmpGF3,(unsigned char*)_TmpGF3,(unsigned char*)_TmpGF3 + 4U},{(unsigned char*)_TmpGF4,(unsigned char*)_TmpGF4,(unsigned char*)_TmpGF4 + 4U},{(unsigned char*)_TmpGF5,(unsigned char*)_TmpGF5,(unsigned char*)_TmpGF5 + 4U},{(unsigned char*)_TmpGF6,(unsigned char*)_TmpGF6,(unsigned char*)_TmpGF6 + 4U},{(unsigned char*)_TmpGF7,(unsigned char*)_TmpGF7,(unsigned char*)_TmpGF7 + 4U},{(unsigned char*)_TmpGF8,(unsigned char*)_TmpGF8,(unsigned char*)_TmpGF8 + 4U},{(unsigned char*)_TmpGF9,(unsigned char*)_TmpGF9,(unsigned char*)_TmpGF9 + 4U},{(unsigned char*)_TmpGFA,(unsigned char*)_TmpGFA,(unsigned char*)_TmpGFA + 4U},{(unsigned char*)_TmpGFB,(unsigned char*)_TmpGFB,(unsigned char*)_TmpGFB + 4U},{(unsigned char*)_TmpGFC,(unsigned char*)_TmpGFC,(unsigned char*)_TmpGFC + 4U},{(unsigned char*)_TmpGFD,(unsigned char*)_TmpGFD,(unsigned char*)_TmpGFD + 4U},{(unsigned char*)_TmpGFE,(unsigned char*)_TmpGFE,(unsigned char*)_TmpGFE + 4U},{(unsigned char*)_TmpGFF,(unsigned char*)_TmpGFF,(unsigned char*)_TmpGFF + 4U},{(unsigned char*)_TmpG100,(unsigned char*)_TmpG100,(unsigned char*)_TmpG100 + 4U},{(unsigned char*)_TmpG101,(unsigned char*)_TmpG101,(unsigned char*)_TmpG101 + 4U},{(unsigned char*)_TmpG102,(unsigned char*)_TmpG102,(unsigned char*)_TmpG102 + 20U},{(unsigned char*)_TmpG103,(unsigned char*)_TmpG103,(unsigned char*)_TmpG103 + 5U},{(unsigned char*)_TmpG104,(unsigned char*)_TmpG104,(unsigned char*)_TmpG104 + 17U},{(unsigned char*)_TmpG105,(unsigned char*)_TmpG105,(unsigned char*)_TmpG105 + 18U},{(unsigned char*)_TmpG106,(unsigned char*)_TmpG106,(unsigned char*)_TmpG106 + 19U},{(unsigned char*)_TmpG107,(unsigned char*)_TmpG107,(unsigned char*)_TmpG107 + 16U},{(unsigned char*)_TmpG108,(unsigned char*)_TmpG108,(unsigned char*)_TmpG108 + 13U},{(unsigned char*)_TmpG109,(unsigned char*)_TmpG109,(unsigned char*)_TmpG109 + 14U},{(unsigned char*)_TmpG10A,(unsigned char*)_TmpG10A,(unsigned char*)_TmpG10A + 17U},{(unsigned char*)_TmpG10B,(unsigned char*)_TmpG10B,(unsigned char*)_TmpG10B + 16U},{(unsigned char*)_TmpG10C,(unsigned char*)_TmpG10C,(unsigned char*)_TmpG10C + 12U},{(unsigned char*)_TmpG10D,(unsigned char*)_TmpG10D,(unsigned char*)_TmpG10D + 19U},{(unsigned char*)_TmpG10E,(unsigned char*)_TmpG10E,(unsigned char*)_TmpG10E + 13U},{(unsigned char*)_TmpG10F,(unsigned char*)_TmpG10F,(unsigned char*)_TmpG10F + 21U},{(unsigned char*)_TmpG110,(unsigned char*)_TmpG110,(unsigned char*)_TmpG110 + 14U},{(unsigned char*)_TmpG111,(unsigned char*)_TmpG111,(unsigned char*)_TmpG111 + 20U},{(unsigned char*)_TmpG112,(unsigned char*)_TmpG112,(unsigned char*)_TmpG112 + 21U},{(unsigned char*)_TmpG113,(unsigned char*)_TmpG113,(unsigned char*)_TmpG113 + 13U},{(unsigned char*)_TmpG114,(unsigned char*)_TmpG114,(unsigned char*)_TmpG114 + 15U},{(unsigned char*)_TmpG115,(unsigned char*)_TmpG115,(unsigned char*)_TmpG115 + 17U},{(unsigned char*)_TmpG116,(unsigned char*)_TmpG116,(unsigned char*)_TmpG116 + 19U},{(unsigned char*)_TmpG117,(unsigned char*)_TmpG117,(unsigned char*)_TmpG117 + 19U},{(unsigned char*)_TmpG118,(unsigned char*)_TmpG118,(unsigned char*)_TmpG118 + 21U},{(unsigned char*)_TmpG119,(unsigned char*)_TmpG119,(unsigned char*)_TmpG119 + 12U},{(unsigned char*)_TmpG11A,(unsigned char*)_TmpG11A,(unsigned char*)_TmpG11A + 9U},{(unsigned char*)_TmpG11B,(unsigned char*)_TmpG11B,(unsigned char*)_TmpG11B + 17U},{(unsigned char*)_TmpG11C,(unsigned char*)_TmpG11C,(unsigned char*)_TmpG11C + 23U},{(unsigned char*)_TmpG11D,(unsigned char*)_TmpG11D,(unsigned char*)_TmpG11D + 24U},{(unsigned char*)_TmpG11E,(unsigned char*)_TmpG11E,(unsigned char*)_TmpG11E + 15U},{(unsigned char*)_TmpG11F,(unsigned char*)_TmpG11F,(unsigned char*)_TmpG11F + 11U},{(unsigned char*)_TmpG120,(unsigned char*)_TmpG120,(unsigned char*)_TmpG120 + 15U},{(unsigned char*)_TmpG121,(unsigned char*)_TmpG121,(unsigned char*)_TmpG121 + 10U},{(unsigned char*)_TmpG122,(unsigned char*)_TmpG122,(unsigned char*)_TmpG122 + 15U},{(unsigned char*)_TmpG123,(unsigned char*)_TmpG123,(unsigned char*)_TmpG123 + 25U},{(unsigned char*)_TmpG124,(unsigned char*)_TmpG124,(unsigned char*)_TmpG124 + 5U},{(unsigned char*)_TmpG125,(unsigned char*)_TmpG125,(unsigned char*)_TmpG125 + 15U},{(unsigned char*)_TmpG126,(unsigned char*)_TmpG126,(unsigned char*)_TmpG126 + 15U},{(unsigned char*)_TmpG127,(unsigned char*)_TmpG127,(unsigned char*)_TmpG127 + 11U},{(unsigned char*)_TmpG128,(unsigned char*)_TmpG128,(unsigned char*)_TmpG128 + 22U},{(unsigned char*)_TmpG129,(unsigned char*)_TmpG129,(unsigned char*)_TmpG129 + 26U},{(unsigned char*)_TmpG12A,(unsigned char*)_TmpG12A,(unsigned char*)_TmpG12A + 26U},{(unsigned char*)_TmpG12B,(unsigned char*)_TmpG12B,(unsigned char*)_TmpG12B + 16U},{(unsigned char*)_TmpG12C,(unsigned char*)_TmpG12C,(unsigned char*)_TmpG12C + 16U},{(unsigned char*)_TmpG12D,(unsigned char*)_TmpG12D,(unsigned char*)_TmpG12D + 24U},{(unsigned char*)_TmpG12E,(unsigned char*)_TmpG12E,(unsigned char*)_TmpG12E + 25U},{(unsigned char*)_TmpG12F,(unsigned char*)_TmpG12F,(unsigned char*)_TmpG12F + 25U},{(unsigned char*)_TmpG130,(unsigned char*)_TmpG130,(unsigned char*)_TmpG130 + 16U},{(unsigned char*)_TmpG131,(unsigned char*)_TmpG131,(unsigned char*)_TmpG131 + 19U},{(unsigned char*)_TmpG132,(unsigned char*)_TmpG132,(unsigned char*)_TmpG132 + 25U},{(unsigned char*)_TmpG133,(unsigned char*)_TmpG133,(unsigned char*)_TmpG133 + 35U},{(unsigned char*)_TmpG134,(unsigned char*)_TmpG134,(unsigned char*)_TmpG134 + 27U},{(unsigned char*)_TmpG135,(unsigned char*)_TmpG135,(unsigned char*)_TmpG135 + 18U},{(unsigned char*)_TmpG136,(unsigned char*)_TmpG136,(unsigned char*)_TmpG136 + 20U},{(unsigned char*)_TmpG137,(unsigned char*)_TmpG137,(unsigned char*)_TmpG137 + 19U},{(unsigned char*)_TmpG138,(unsigned char*)_TmpG138,(unsigned char*)_TmpG138 + 14U},{(unsigned char*)_TmpG139,(unsigned char*)_TmpG139,(unsigned char*)_TmpG139 + 19U},{(unsigned char*)_TmpG13A,(unsigned char*)_TmpG13A,(unsigned char*)_TmpG13A + 20U},{(unsigned char*)_TmpG13B,(unsigned char*)_TmpG13B,(unsigned char*)_TmpG13B + 14U},{(unsigned char*)_TmpG13C,(unsigned char*)_TmpG13C,(unsigned char*)_TmpG13C + 11U},{(unsigned char*)_TmpG13D,(unsigned char*)_TmpG13D,(unsigned char*)_TmpG13D + 23U},{(unsigned char*)_TmpG13E,(unsigned char*)_TmpG13E,(unsigned char*)_TmpG13E + 18U},{(unsigned char*)_TmpG13F,(unsigned char*)_TmpG13F,(unsigned char*)_TmpG13F + 30U},{(unsigned char*)_TmpG140,(unsigned char*)_TmpG140,(unsigned char*)_TmpG140 + 8U},{(unsigned char*)_TmpG141,(unsigned char*)_TmpG141,(unsigned char*)_TmpG141 + 12U},{(unsigned char*)_TmpG142,(unsigned char*)_TmpG142,(unsigned char*)_TmpG142 + 14U},{(unsigned char*)_TmpG143,(unsigned char*)_TmpG143,(unsigned char*)_TmpG143 + 13U},{(unsigned char*)_TmpG144,(unsigned char*)_TmpG144,(unsigned char*)_TmpG144 + 16U},{(unsigned char*)_TmpG145,(unsigned char*)_TmpG145,(unsigned char*)_TmpG145 + 23U},{(unsigned char*)_TmpG146,(unsigned char*)_TmpG146,(unsigned char*)_TmpG146 + 14U},{(unsigned char*)_TmpG147,(unsigned char*)_TmpG147,(unsigned char*)_TmpG147 + 18U},{(unsigned char*)_TmpG148,(unsigned char*)_TmpG148,(unsigned char*)_TmpG148 + 8U},{(unsigned char*)_TmpG149,(unsigned char*)_TmpG149,(unsigned char*)_TmpG149 + 8U},{(unsigned char*)_TmpG14A,(unsigned char*)_TmpG14A,(unsigned char*)_TmpG14A + 11U},{(unsigned char*)_TmpG14B,(unsigned char*)_TmpG14B,(unsigned char*)_TmpG14B + 20U},{(unsigned char*)_TmpG14C,(unsigned char*)_TmpG14C,(unsigned char*)_TmpG14C + 14U},{(unsigned char*)_TmpG14D,(unsigned char*)_TmpG14D,(unsigned char*)_TmpG14D + 9U},{(unsigned char*)_TmpG14E,(unsigned char*)_TmpG14E,(unsigned char*)_TmpG14E + 16U},{(unsigned char*)_TmpG14F,(unsigned char*)_TmpG14F,(unsigned char*)_TmpG14F + 27U},{(unsigned char*)_TmpG150,(unsigned char*)_TmpG150,(unsigned char*)_TmpG150 + 18U},{(unsigned char*)_TmpG151,(unsigned char*)_TmpG151,(unsigned char*)_TmpG151 + 14U},{(unsigned char*)_TmpG152,(unsigned char*)_TmpG152,(unsigned char*)_TmpG152 + 13U},{(unsigned char*)_TmpG153,(unsigned char*)_TmpG153,(unsigned char*)_TmpG153 + 12U},{(unsigned char*)_TmpG154,(unsigned char*)_TmpG154,(unsigned char*)_TmpG154 + 15U},{(unsigned char*)_TmpG155,(unsigned char*)_TmpG155,(unsigned char*)_TmpG155 + 14U},{(unsigned char*)_TmpG156,(unsigned char*)_TmpG156,(unsigned char*)_TmpG156 + 16U},{(unsigned char*)_TmpG157,(unsigned char*)_TmpG157,(unsigned char*)_TmpG157 + 11U},{(unsigned char*)_TmpG158,(unsigned char*)_TmpG158,(unsigned char*)_TmpG158 + 14U},{(unsigned char*)_TmpG159,(unsigned char*)_TmpG159,(unsigned char*)_TmpG159 + 11U},{(unsigned char*)_TmpG15A,(unsigned char*)_TmpG15A,(unsigned char*)_TmpG15A + 15U},{(unsigned char*)_TmpG15B,(unsigned char*)_TmpG15B,(unsigned char*)_TmpG15B + 22U},{(unsigned char*)_TmpG15C,(unsigned char*)_TmpG15C,(unsigned char*)_TmpG15C + 16U},{(unsigned char*)_TmpG15D,(unsigned char*)_TmpG15D,(unsigned char*)_TmpG15D + 17U},{(unsigned char*)_TmpG15E,(unsigned char*)_TmpG15E,(unsigned char*)_TmpG15E + 12U},{(unsigned char*)_TmpG15F,(unsigned char*)_TmpG15F,(unsigned char*)_TmpG15F + 18U},{(unsigned char*)_TmpG160,(unsigned char*)_TmpG160,(unsigned char*)_TmpG160 + 17U},{(unsigned char*)_TmpG161,(unsigned char*)_TmpG161,(unsigned char*)_TmpG161 + 12U},{(unsigned char*)_TmpG162,(unsigned char*)_TmpG162,(unsigned char*)_TmpG162 + 16U},{(unsigned char*)_TmpG163,(unsigned char*)_TmpG163,(unsigned char*)_TmpG163 + 11U},{(unsigned char*)_TmpG164,(unsigned char*)_TmpG164,(unsigned char*)_TmpG164 + 10U},{(unsigned char*)_TmpG165,(unsigned char*)_TmpG165,(unsigned char*)_TmpG165 + 14U},{(unsigned char*)_TmpG166,(unsigned char*)_TmpG166,(unsigned char*)_TmpG166 + 15U},{(unsigned char*)_TmpG167,(unsigned char*)_TmpG167,(unsigned char*)_TmpG167 + 20U},{(unsigned char*)_TmpG168,(unsigned char*)_TmpG168,(unsigned char*)_TmpG168 + 27U},{(unsigned char*)_TmpG169,(unsigned char*)_TmpG169,(unsigned char*)_TmpG169 + 16U},{(unsigned char*)_TmpG16A,(unsigned char*)_TmpG16A,(unsigned char*)_TmpG16A + 20U},{(unsigned char*)_TmpG16B,(unsigned char*)_TmpG16B,(unsigned char*)_TmpG16B + 10U},{(unsigned char*)_TmpG16C,(unsigned char*)_TmpG16C,(unsigned char*)_TmpG16C + 18U},{(unsigned char*)_TmpG16D,(unsigned char*)_TmpG16D,(unsigned char*)_TmpG16D + 21U},{(unsigned char*)_TmpG16E,(unsigned char*)_TmpG16E,(unsigned char*)_TmpG16E + 18U},{(unsigned char*)_TmpG16F,(unsigned char*)_TmpG16F,(unsigned char*)_TmpG16F + 16U},{(unsigned char*)_TmpG170,(unsigned char*)_TmpG170,(unsigned char*)_TmpG170 + 25U},{(unsigned char*)_TmpG171,(unsigned char*)_TmpG171,(unsigned char*)_TmpG171 + 19U},{(unsigned char*)_TmpG172,(unsigned char*)_TmpG172,(unsigned char*)_TmpG172 + 16U},{(unsigned char*)_TmpG173,(unsigned char*)_TmpG173,(unsigned char*)_TmpG173 + 20U},{(unsigned char*)_TmpG174,(unsigned char*)_TmpG174,(unsigned char*)_TmpG174 + 15U},{(unsigned char*)_TmpG175,(unsigned char*)_TmpG175,(unsigned char*)_TmpG175 + 20U},{(unsigned char*)_TmpG176,(unsigned char*)_TmpG176,(unsigned char*)_TmpG176 + 12U},{(unsigned char*)_TmpG177,(unsigned char*)_TmpG177,(unsigned char*)_TmpG177 + 15U},{(unsigned char*)_TmpG178,(unsigned char*)_TmpG178,(unsigned char*)_TmpG178 + 12U},{(unsigned char*)_TmpG179,(unsigned char*)_TmpG179,(unsigned char*)_TmpG179 + 20U},{(unsigned char*)_TmpG17A,(unsigned char*)_TmpG17A,(unsigned char*)_TmpG17A + 19U},{(unsigned char*)_TmpG17B,(unsigned char*)_TmpG17B,(unsigned char*)_TmpG17B + 20U},{(unsigned char*)_TmpG17C,(unsigned char*)_TmpG17C,(unsigned char*)_TmpG17C + 21U},{(unsigned char*)_TmpG17D,(unsigned char*)_TmpG17D,(unsigned char*)_TmpG17D + 21U},{(unsigned char*)_TmpG17E,(unsigned char*)_TmpG17E,(unsigned char*)_TmpG17E + 12U},{(unsigned char*)_TmpG17F,(unsigned char*)_TmpG17F,(unsigned char*)_TmpG17F + 17U},{(unsigned char*)_TmpG180,(unsigned char*)_TmpG180,(unsigned char*)_TmpG180 + 19U},{(unsigned char*)_TmpG181,(unsigned char*)_TmpG181,(unsigned char*)_TmpG181 + 14U},{(unsigned char*)_TmpG182,(unsigned char*)_TmpG182,(unsigned char*)_TmpG182 + 17U},{(unsigned char*)_TmpG183,(unsigned char*)_TmpG183,(unsigned char*)_TmpG183 + 23U},{(unsigned char*)_TmpG184,(unsigned char*)_TmpG184,(unsigned char*)_TmpG184 + 13U},{(unsigned char*)_TmpG185,(unsigned char*)_TmpG185,(unsigned char*)_TmpG185 + 14U},{(unsigned char*)_TmpG186,(unsigned char*)_TmpG186,(unsigned char*)_TmpG186 + 16U},{(unsigned char*)_TmpG187,(unsigned char*)_TmpG187,(unsigned char*)_TmpG187 + 16U},{(unsigned char*)_TmpG188,(unsigned char*)_TmpG188,(unsigned char*)_TmpG188 + 8U},{(unsigned char*)_TmpG189,(unsigned char*)_TmpG189,(unsigned char*)_TmpG189 + 19U},{(unsigned char*)_TmpG18A,(unsigned char*)_TmpG18A,(unsigned char*)_TmpG18A + 20U},{(unsigned char*)_TmpG18B,(unsigned char*)_TmpG18B,(unsigned char*)_TmpG18B + 14U},{(unsigned char*)_TmpG18C,(unsigned char*)_TmpG18C,(unsigned char*)_TmpG18C + 19U},{(unsigned char*)_TmpG18D,(unsigned char*)_TmpG18D,(unsigned char*)_TmpG18D + 20U},{(unsigned char*)_TmpG18E,(unsigned char*)_TmpG18E,(unsigned char*)_TmpG18E + 11U},{(unsigned char*)_TmpG18F,(unsigned char*)_TmpG18F,(unsigned char*)_TmpG18F + 22U},{(unsigned char*)_TmpG190,(unsigned char*)_TmpG190,(unsigned char*)_TmpG190 + 20U},{(unsigned char*)_TmpG191,(unsigned char*)_TmpG191,(unsigned char*)_TmpG191 + 23U},{(unsigned char*)_TmpG192,(unsigned char*)_TmpG192,(unsigned char*)_TmpG192 + 20U},{(unsigned char*)_TmpG193,(unsigned char*)_TmpG193,(unsigned char*)_TmpG193 + 22U},{(unsigned char*)_TmpG194,(unsigned char*)_TmpG194,(unsigned char*)_TmpG194 + 23U},{(unsigned char*)_TmpG195,(unsigned char*)_TmpG195,(unsigned char*)_TmpG195 + 24U},{(unsigned char*)_TmpG196,(unsigned char*)_TmpG196,(unsigned char*)_TmpG196 + 24U},{(unsigned char*)_TmpG197,(unsigned char*)_TmpG197,(unsigned char*)_TmpG197 + 15U},{(unsigned char*)_TmpG198,(unsigned char*)_TmpG198,(unsigned char*)_TmpG198 + 20U},{(unsigned char*)_TmpG199,(unsigned char*)_TmpG199,(unsigned char*)_TmpG199 + 22U},{(unsigned char*)_TmpG19A,(unsigned char*)_TmpG19A,(unsigned char*)_TmpG19A + 17U},{(unsigned char*)_TmpG19B,(unsigned char*)_TmpG19B,(unsigned char*)_TmpG19B + 20U},{(unsigned char*)_TmpG19C,(unsigned char*)_TmpG19C,(unsigned char*)_TmpG19C + 26U},{(unsigned char*)_TmpG19D,(unsigned char*)_TmpG19D,(unsigned char*)_TmpG19D + 12U},{(unsigned char*)_TmpG19E,(unsigned char*)_TmpG19E,(unsigned char*)_TmpG19E + 14U},{(unsigned char*)_TmpG19F,(unsigned char*)_TmpG19F,(unsigned char*)_TmpG19F + 12U},{(unsigned char*)_TmpG1A0,(unsigned char*)_TmpG1A0,(unsigned char*)_TmpG1A0 + 18U},{(unsigned char*)_TmpG1A1,(unsigned char*)_TmpG1A1,(unsigned char*)_TmpG1A1 + 16U},{(unsigned char*)_TmpG1A2,(unsigned char*)_TmpG1A2,(unsigned char*)_TmpG1A2 + 17U},{(unsigned char*)_TmpG1A3,(unsigned char*)_TmpG1A3,(unsigned char*)_TmpG1A3 + 15U},{(unsigned char*)_TmpG1A4,(unsigned char*)_TmpG1A4,(unsigned char*)_TmpG1A4 + 9U},{(unsigned char*)_TmpG1A5,(unsigned char*)_TmpG1A5,(unsigned char*)_TmpG1A5 + 13U},{(unsigned char*)_TmpG1A6,(unsigned char*)_TmpG1A6,(unsigned char*)_TmpG1A6 + 12U},{(unsigned char*)_TmpG1A7,(unsigned char*)_TmpG1A7,(unsigned char*)_TmpG1A7 + 12U},{(unsigned char*)_TmpG1A8,(unsigned char*)_TmpG1A8,(unsigned char*)_TmpG1A8 + 11U},{(unsigned char*)_TmpG1A9,(unsigned char*)_TmpG1A9,(unsigned char*)_TmpG1A9 + 11U},{(unsigned char*)_TmpG1AA,(unsigned char*)_TmpG1AA,(unsigned char*)_TmpG1AA + 11U},{(unsigned char*)_TmpG1AB,(unsigned char*)_TmpG1AB,(unsigned char*)_TmpG1AB + 16U},{(unsigned char*)_TmpG1AC,(unsigned char*)_TmpG1AC,(unsigned char*)_TmpG1AC + 17U},{(unsigned char*)_TmpG1AD,(unsigned char*)_TmpG1AD,(unsigned char*)_TmpG1AD + 19U},{(unsigned char*)_TmpG1AE,(unsigned char*)_TmpG1AE,(unsigned char*)_TmpG1AE + 17U},{(unsigned char*)_TmpG1AF,(unsigned char*)_TmpG1AF,(unsigned char*)_TmpG1AF + 19U},{(unsigned char*)_TmpG1B0,(unsigned char*)_TmpG1B0,(unsigned char*)_TmpG1B0 + 25U},{(unsigned char*)_TmpG1B1,(unsigned char*)_TmpG1B1,(unsigned char*)_TmpG1B1 + 26U},{(unsigned char*)_TmpG1B2,(unsigned char*)_TmpG1B2,(unsigned char*)_TmpG1B2 + 9U},{(unsigned char*)_TmpG1B3,(unsigned char*)_TmpG1B3,(unsigned char*)_TmpG1B3 + 20U},{(unsigned char*)_TmpG1B4,(unsigned char*)_TmpG1B4,(unsigned char*)_TmpG1B4 + 17U},{(unsigned char*)_TmpG1B5,(unsigned char*)_TmpG1B5,(unsigned char*)_TmpG1B5 + 18U},{(unsigned char*)_TmpG1B6,(unsigned char*)_TmpG1B6,(unsigned char*)_TmpG1B6 + 11U},{(unsigned char*)_TmpG1B7,(unsigned char*)_TmpG1B7,(unsigned char*)_TmpG1B7 + 12U},{(unsigned char*)_TmpG1B8,(unsigned char*)_TmpG1B8,(unsigned char*)_TmpG1B8 + 16U},{(unsigned char*)_TmpG1B9,(unsigned char*)_TmpG1B9,(unsigned char*)_TmpG1B9 + 20U},{(unsigned char*)_TmpG1BA,(unsigned char*)_TmpG1BA,(unsigned char*)_TmpG1BA + 16U},{(unsigned char*)_TmpG1BB,(unsigned char*)_TmpG1BB,(unsigned char*)_TmpG1BB + 15U},{(unsigned char*)_TmpG1BC,(unsigned char*)_TmpG1BC,(unsigned char*)_TmpG1BC + 11U},{(unsigned char*)_TmpG1BD,(unsigned char*)_TmpG1BD,(unsigned char*)_TmpG1BD + 5U}};
# 1826
static short Cyc_yyr1[625U]={0,176,176,177,178,178,178,178,178,178,178,178,178,178,178,178,179,180,181,182,183,183,184,184,184,185,185,186,186,186,187,187,188,188,189,189,189,190,190,191,191,191,191,192,192,193,194,195,196,197,198,199,199,199,199,199,199,200,200,201,201,202,202,202,202,202,202,202,202,202,202,202,203,203,203,203,203,203,203,204,204,205,206,206,207,207,207,207,208,208,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,209,210,211,211,211,212,212,212,213,213,214,214,214,215,215,215,216,216,217,217,218,218,219,219,220,220,221,221,222,222,222,222,223,224,224,224,224,224,224,225,225,225,225,225,225,226,226,227,227,227,227,228,228,229,229,229,230,230,231,231,231,231,232,232,232,233,233,234,234,235,235,236,236,236,236,236,236,236,236,236,237,237,237,237,237,237,237,237,237,237,238,238,239,240,240,241,241,241,241,241,241,241,241,241,241,241,241,242,242,242,- 1,- 1,243,243,243,244,244,244,245,245,245,246,246,247,247,247,248,248,249,249,249,249,249,250,250,251,251,252,252,253,253,254,254,254,254,255,255,255,256,257,257,257,257,257,258,258,259,259,260,260,261,261,262,262,262,262,263,263,264,264,265,265,265,266,267,267,268,268,269,269,269,269,269,270,270,270,270,271,271,272,272,273,273,274,274,275,275,275,275,275,275,276,276,277,277,277,278,278,278,278,278,278,278,278,278,279,279,279,279,279,279,279,279,280,280,281,281,281,281,281,281,282,283,283,284,285,286,286,287,287,288,288,288,288,288,288,288,288,289,289,289,289,289,289,290,290,290,290,290,290,291,291,291,291,292,292,293,293,293,293,293,293,293,293,294,295,295,296,296,297,297,298,298,299,299,300,300,301,301,302,302,303,303,303,304,304,305,305,306,306,307,307,307,307,307,308,309,310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,310,311,311,311,312,312,313,313,314,314,314,315,315,316,316,317,317,317,318,318,318,318,318,318,318,318,318,318,318,319,319,319,319,319,319,319,319,319,320,321,321,322,322,323,323,324,324,325,325,326,326,327,327,328,328,328,329,329,330,330,331,331,332,332,332,332,333,333,334,334,334,335,335,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,336,337,337,337,337,338,339,339,340,340,340,341,341,342,342,342,343,343,344,345,345,345,346,346,347,347,347,347,347,347,347,347,347,347,347,348,348,348,348,349,349,349,349,349,349,349,349,349,349,349,350,351,351,352,352,352,352,352,353,353,354,354,355,355,356,356,357,357,358,358,359,359,360,360,361,361,362,362,362,362,362,362,362,363,363,363,363,363};
# 1892
static short Cyc_yyr2[625U]={0,1,1,1,2,3,5,3,5,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,1,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,4,1,4,7,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,4,1,1,1,1,1,1,1,4,1,1,1,4,0,1,2,2,1,0,3,3,0,1,1,1,3,0,1,1,0,2,2,3,5,5,7,0,1,2,4,0,2,0,2,1,1,3,3,3,3,4,3,1,1,1,1,1,1,4,1,4,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,1,3,1,1,2,3,3,4,4,5,4,5,4,2,4,4,4,4,5,5,5,5,0,1,1,1,1,1,1,1,3,1,2,1,1,2,3,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,8,6,6,6,9,13,11,4,4,6,6,4,2,2,4,4,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,8,0,1,1,3,1,1,5,9,7,7,7,7,9,1,1,1,1,1};
# 1958
static short Cyc_yydefact[1269U]={0,36,72,73,74,75,77,90,91,92,93,94,95,96,97,98,117,118,119,133,134,68,0,0,103,0,99,0,78,0,0,49,171,110,113,0,0,0,16,17,0,0,0,0,0,595,248,597,596,598,0,230,0,106,0,230,229,1,3,0,0,0,0,34,0,0,0,35,0,61,70,64,88,66,100,101,0,132,104,0,0,182,0,205,208,105,186,135,2,76,75,248,69,0,121,0,63,593,0,595,590,591,592,594,0,135,0,0,422,0,0,0,288,0,426,424,45,47,0,0,57,0,0,0,0,0,0,131,172,0,0,249,250,0,0,227,0,0,0,0,228,0,0,0,4,0,0,0,0,0,51,0,141,143,62,71,65,67,599,600,135,137,135,349,59,0,0,0,38,0,252,0,194,183,206,0,213,214,218,219,0,0,217,216,0,215,221,238,208,0,89,76,125,0,123,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,579,580,543,0,0,0,541,0,540,538,539,0,451,453,467,477,479,481,483,485,487,489,491,494,496,509,0,511,561,578,576,595,434,0,0,0,0,435,0,0,425,54,0,0,135,0,0,264,266,267,265,0,268,0,152,148,150,308,269,315,310,0,0,0,0,0,11,12,0,0,223,222,0,0,601,602,248,116,0,0,0,0,0,187,107,286,0,283,13,14,0,5,0,7,0,0,52,0,543,0,0,38,130,0,138,139,164,0,169,60,38,143,0,0,0,0,0,0,0,0,0,0,0,0,595,347,0,350,0,361,355,0,359,340,341,351,342,0,343,344,345,0,37,39,316,0,273,289,0,0,254,252,0,233,0,0,0,0,0,240,239,79,236,209,0,126,122,0,0,0,518,0,0,534,469,509,0,470,471,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,512,513,544,533,0,515,0,0,0,0,514,0,102,0,0,0,0,0,0,498,499,0,502,503,500,501,0,0,0,504,505,0,506,507,508,0,0,457,458,459,460,461,462,463,464,465,466,456,0,516,0,567,568,0,0,0,582,0,135,427,0,0,0,448,595,602,0,0,0,0,304,444,449,0,446,0,0,423,0,290,0,0,441,442,0,439,0,0,311,0,281,153,158,154,156,149,151,252,0,318,309,319,604,603,0,109,112,0,56,0,0,111,114,0,0,184,0,195,196,251,0,85,84,0,82,232,231,188,252,285,318,287,0,108,19,32,46,0,48,0,50,0,142,144,145,292,291,38,40,135,128,140,0,0,160,167,135,177,41,0,0,0,0,0,595,0,379,0,382,383,384,0,0,386,0,0,353,0,0,362,356,143,360,352,348,0,193,274,0,0,0,280,253,275,338,0,243,0,254,192,235,234,189,233,0,0,0,0,241,80,0,136,127,476,124,120,0,0,0,0,595,293,298,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,581,588,0,587,452,478,0,480,482,484,486,488,490,492,493,495,497,455,454,566,563,0,565,0,0,0,437,438,0,307,0,445,302,305,433,0,303,436,430,53,0,431,0,0,0,312,0,159,155,157,0,0,233,0,320,0,252,0,329,314,0,0,0,0,0,0,0,252,0,204,185,607,0,0,0,152,135,284,0,25,6,8,9,0,42,254,163,147,164,0,0,162,170,179,178,0,0,173,0,0,0,369,0,0,0,379,0,380,381,385,0,0,0,357,346,354,0,43,317,252,0,277,0,0,0,0,0,0,339,191,0,255,256,257,0,0,273,244,190,210,212,211,220,241,207,237,517,0,0,0,294,0,299,0,520,0,0,0,0,0,0,0,535,0,577,528,529,532,0,0,536,537,545,0,0,510,585,0,0,564,562,0,0,0,0,306,447,450,432,440,443,313,270,282,338,321,322,233,0,0,233,0,0,55,224,0,197,0,0,0,0,233,0,0,0,608,609,595,0,81,83,0,0,20,26,146,137,161,0,165,168,180,177,177,0,0,0,0,0,0,0,0,0,379,369,387,0,358,44,254,0,278,276,0,0,0,0,0,0,0,0,0,0,254,0,242,573,0,572,0,295,300,0,472,473,452,452,452,0,0,0,0,474,475,566,565,550,0,586,569,0,589,468,583,584,0,428,429,326,324,328,338,323,233,58,198,115,203,338,202,199,233,0,0,0,0,0,0,0,605,0,0,86,0,0,0,0,0,0,0,176,175,363,369,0,0,0,0,0,389,390,392,394,396,398,400,402,404,406,409,411,413,415,420,421,0,0,366,375,0,379,0,0,388,246,279,0,0,0,0,0,258,259,0,271,263,260,261,245,252,519,0,0,301,522,523,524,0,0,0,0,531,530,0,556,550,546,548,542,570,0,327,325,201,200,0,0,0,0,0,0,0,0,606,610,0,33,28,0,0,38,0,10,129,166,0,0,0,369,0,418,0,0,369,0,0,0,0,0,0,0,0,0,0,0,0,0,416,369,0,0,379,368,331,332,333,330,262,0,254,575,574,0,0,0,0,0,0,557,556,553,551,0,547,571,612,611,0,0,0,0,623,624,622,620,621,0,0,0,0,29,27,0,30,0,22,181,364,365,0,0,0,0,369,371,393,0,395,397,399,401,403,405,407,408,410,412,0,370,376,0,0,335,336,337,334,0,247,0,0,521,0,0,0,0,559,558,0,552,549,0,0,0,0,0,0,613,0,31,21,23,0,367,417,0,414,372,0,369,378,0,272,297,296,525,0,0,555,0,554,0,0,0,0,0,0,87,24,0,391,369,373,377,0,0,560,616,617,618,615,0,0,419,374,0,527,0,0,0,619,614,526,0,0,0};
# 2088
static short Cyc_yydefgoto[188U]={1266,57,58,59,60,61,553,990,1158,887,888,1083,743,62,364,63,350,64,555,65,557,66,559,67,285,165,68,69,626,275,541,542,276,72,297,277,74,192,193,75,76,77,190,324,325,151,152,326,278,513,571,572,755,78,79,759,760,761,80,573,81,533,82,83,187,188,290,84,135,620,381,382,808,714,132,85,371,614,795,796,797,279,280,1039,606,610,799,507,372,308,111,112,640,563,641,486,487,488,281,365,366,715,519,792,793,353,354,355,167,356,168,357,358,359,903,360,770,361,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1012,1013,1014,1015,1016,1017,489,502,503,490,491,492,362,233,466,234,632,235,236,237,238,239,240,241,242,243,244,439,444,449,453,245,246,247,421,422,951,1059,1060,1133,1061,1135,1200,248,931,249,666,667,250,251,87,1084,493,523,88,878,879,1141,880,1150};
# 2110
static short Cyc_yypact[1269U]={3575,- -32768,- -32768,- -32768,- -32768,- 63,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4585,79,30,- -32768,4585,- -32768,691,- -32768,216,- 14,- -32768,- -32768,65,78,159,209,288,- -32768,- -32768,182,289,291,606,398,- -32768,152,- -32768,- -32768,- -32768,321,343,617,373,363,343,- -32768,- -32768,- -32768,391,393,406,3409,- -32768,675,709,434,- -32768,624,4585,4585,4585,- -32768,4585,- -32768,- -32768,243,443,- -32768,216,4361,335,124,468,225,- -32768,- -32768,445,- -32768,490,507,- 4,- -32768,216,505,7442,- -32768,- -32768,1423,319,- -32768,- -32768,- -32768,- -32768,519,445,534,7442,- -32768,525,1423,548,563,568,- -32768,95,- -32768,- -32768,4694,4694,579,571,3409,3409,362,7442,5329,- -32768,- -32768,170,592,- -32768,- -32768,612,6236,- -32768,590,595,170,5329,- -32768,3409,3409,3740,- -32768,3409,3740,3409,3740,3740,- -32768,370,- -32768,1683,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,445,5329,220,- -32768,- -32768,4361,624,2048,622,4694,4806,6370,- -32768,335,- -32768,642,- -32768,- -32768,- -32768,- -32768,661,688,- -32768,- -32768,705,- -32768,- -32768,- 36,225,4694,- -32768,- -32768,685,713,716,216,7933,722,8052,7442,7695,727,729,741,750,754,780,786,796,805,807,809,812,816,820,826,831,832,833,8052,8052,- -32768,- -32768,940,8171,3069,834,- -32768,8171,- -32768,- -32768,- -32768,125,- -32768,- -32768,- 40,874,835,828,825,530,296,758,397,123,- -32768,277,8171,389,8,- -32768,848,193,- -32768,1423,- 23,857,1248,865,262,1248,- -32768,- -32768,871,7442,445,1342,851,- -32768,- -32768,- -32768,- -32768,852,- -32768,4992,5329,1234,5329,466,- -32768,- -32768,- -32768,- 16,- 16,879,866,861,- -32768,- -32768,862,3,- -32768,- -32768,421,197,- -32768,- -32768,872,- -32768,883,73,870,880,875,- -32768,- -32768,929,444,- -32768,- -32768,- -32768,885,- -32768,886,- -32768,887,890,- -32768,590,940,6504,4361,622,884,891,5329,- -32768,801,216,894,- -32768,622,450,895,329,898,5431,901,916,912,914,6638,2903,5431,327,903,- -32768,2219,- -32768,910,2390,2390,624,2390,- -32768,- -32768,- -32768,- -32768,915,- -32768,- -32768,- -32768,428,- -32768,- -32768,911,70,935,- -32768,4,920,919,464,930,803,924,7442,949,4694,362,- -32768,- -32768,945,932,- -32768,70,216,- -32768,7442,950,3069,- -32768,5329,3069,- -32768,- -32768,- -32768,5566,- -32768,982,7442,7442,7442,7442,7442,7442,7442,7442,7442,7442,7442,969,7442,7442,5329,888,7442,7442,- -32768,- -32768,- -32768,- -32768,948,- -32768,2390,952,459,7442,- -32768,7442,- -32768,8171,7442,8171,8171,8171,8171,- -32768,- -32768,8171,- -32768,- -32768,- -32768,- -32768,8171,8171,8171,- -32768,- -32768,8171,- -32768,- -32768,- -32768,8171,7442,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,7442,- -32768,170,- -32768,- -32768,6772,170,7442,- -32768,951,445,- -32768,957,958,965,- -32768,298,519,170,7442,1423,374,- -32768,- -32768,- -32768,973,956,964,1423,- -32768,974,- -32768,597,1248,- -32768,- -32768,975,978,4694,4694,- -32768,976,979,- -32768,1234,1234,1234,- -32768,- -32768,4007,6906,354,- -32768,403,- -32768,- -32768,4,- -32768,- -32768,977,- -32768,1000,4694,- -32768,- -32768,972,980,407,237,- -32768,- -32768,- -32768,983,- -32768,986,993,984,- -32768,- -32768,- -32768,3849,- -32768,353,- -32768,5329,- -32768,- -32768,1087,- -32768,3409,- -32768,3409,- -32768,3409,- -32768,990,- -32768,- -32768,- -32768,622,- -32768,445,- -32768,- -32768,7442,636,- -32768,- 49,445,909,- -32768,7442,7442,995,1004,7442,994,1123,2732,1008,- -32768,- -32768,- -32768,667,1114,- -32768,7040,2561,- -32768,1017,3235,- -32768,- -32768,4205,- -32768,- -32768,- -32768,4694,- -32768,- -32768,5329,1013,5143,- -32768,- -32768,1003,623,904,- -32768,5178,919,- -32768,- -32768,- -32768,- -32768,803,1015,1016,102,1020,856,- -32768,949,- -32768,- -32768,- -32768,- -32768,- -32768,1021,1023,1025,1051,1026,- -32768,- -32768,773,6504,499,1029,1037,1038,1040,1043,1044,1050,1052,1035,565,1045,1048,1049,1055,418,1059,1060,1071,1068,7814,- -32768,- -32768,1062,1058,- -32768,874,168,835,828,825,530,296,758,397,397,123,- -32768,- -32768,- -32768,- -32768,- -32768,1063,- -32768,52,4694,6102,5329,- -32768,5329,- -32768,1057,- -32768,- -32768,- -32768,- -32768,1306,- -32768,- -32768,- -32768,- -32768,1074,- -32768,1385,188,259,- -32768,5329,- -32768,- -32768,- -32768,1069,1072,803,1061,403,4694,4957,7174,- -32768,- -32768,7442,1080,267,197,1073,7442,4694,4806,7308,- -32768,407,1096,7576,1076,73,4042,445,- -32768,1085,1187,- -32768,- -32768,- -32768,6504,- -32768,919,- -32768,- -32768,801,1081,7442,- -32768,- -32768,- -32768,- -32768,1089,216,684,574,596,7442,902,608,5431,1084,7442,1093,1091,- -32768,- -32768,1108,1110,1102,2390,- -32768,- -32768,4473,- -32768,911,1112,4694,- -32768,1113,4,1106,1109,1115,1119,- -32768,- -32768,1122,- -32768,1118,1128,1148,- 50,274,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,856,- -32768,- -32768,1117,255,1117,1116,- -32768,5700,- -32768,7695,- -32768,7442,7442,7442,7442,1254,7442,7442,- -32768,7695,- -32768,- -32768,- -32768,- -32768,170,170,- -32768,- -32768,1120,1125,5834,- -32768,- -32768,7442,7442,- -32768,- -32768,70,777,1150,1151,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,623,- -32768,- -32768,803,70,1129,803,1124,628,- -32768,- -32768,1136,- -32768,1138,70,1140,1141,803,1135,699,1143,- -32768,1154,1155,1152,- -32768,- -32768,3740,1156,1257,- -32768,- -32768,5329,- -32768,7442,- -32768,- -32768,1157,62,909,5431,1159,1153,1041,1167,1161,5431,7442,1164,7442,902,- -32768,1168,- -32768,- -32768,919,323,- -32768,- -32768,7442,7442,7442,7442,4,904,904,- 11,1177,4,919,5329,- -32768,- -32768,176,- -32768,7442,- -32768,- -32768,6504,- -32768,982,1163,1176,1180,1188,1182,1189,1190,- -32768,982,1186,1191,- 9,1193,- -32768,- -32768,789,- -32768,- -32768,- -32768,- -32768,6102,- -32768,- -32768,- -32768,- -32768,- -32768,623,- -32768,803,- -32768,- -32768,- -32768,- -32768,623,- -32768,- -32768,803,1195,1196,1197,1198,1199,1200,1201,1203,1096,1211,- -32768,1208,659,1210,3409,1212,1204,5329,- -32768,- -32768,1331,902,1214,8290,1209,3235,1205,- -32768,- 28,- -32768,1245,1213,778,530,296,837,397,123,- -32768,- -32768,- -32768,- -32768,1249,8171,2390,- -32768,- -32768,648,7442,1221,1220,- -32768,- -32768,- -32768,1217,1224,1237,1238,1239,- -32768,- -32768,1215,- -32768,- -32768,- -32768,- -32768,- -32768,1112,- -32768,559,491,- -32768,- -32768,- -32768,- -32768,7442,5329,7442,1351,- -32768,- -32768,1222,49,500,- -32768,- -32768,- -32768,- -32768,5968,- -32768,- -32768,- -32768,- -32768,34,34,1096,34,621,621,1096,1262,- -32768,- -32768,1251,- -32768,- -32768,1258,1259,690,243,- -32768,- -32768,- -32768,649,5431,1263,902,3069,- -32768,5329,1242,1877,8171,7442,8171,8171,8171,8171,8171,8171,8171,8171,8171,8171,7442,- -32768,902,1266,1261,7442,- -32768,623,623,623,623,- -32768,4694,919,- -32768,- -32768,3235,1264,1265,1268,1260,7442,1287,514,- -32768,- -32768,1288,- -32768,- -32768,- -32768,- -32768,1275,1276,1278,1279,- -32768,- -32768,- -32768,- -32768,- -32768,1281,1282,1286,1300,- -32768,- -32768,243,- -32768,1289,697,- -32768,- -32768,- -32768,1301,1293,1285,8171,902,- -32768,874,536,835,828,828,530,296,758,397,397,123,- -32768,538,- -32768,- -32768,5431,1295,- -32768,- -32768,- -32768,- -32768,535,- -32768,1303,804,- -32768,1297,1412,5329,657,- -32768,1305,1288,- -32768,- -32768,34,34,1096,34,1096,34,- -32768,1302,- -32768,- -32768,- -32768,243,- -32768,1117,255,- -32768,- -32768,7442,1877,- -32768,5431,- -32768,- -32768,- -32768,- -32768,1304,1313,- -32768,1321,- -32768,1314,1315,1317,1322,1327,1332,- -32768,- -32768,253,- -32768,902,- -32768,- -32768,5329,1329,- -32768,- -32768,- -32768,- -32768,- -32768,1096,34,- -32768,- -32768,1330,- -32768,1333,1335,1336,- -32768,- -32768,- -32768,1487,1488,- -32768};
# 2240
static short Cyc_yypgoto[188U]={- -32768,- -32768,48,- -32768,- -32768,- -32768,- -32768,- -32768,279,- -32768,- -32768,339,- -32768,- -32768,- 319,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 52,- -32768,- 114,192,- -32768,- -32768,0,763,- -32768,106,- 202,1358,13,- -32768,- -32768,- 145,- -32768,- -32768,1459,- 60,614,- -32768,- -32768,1194,1181,680,408,- -32768,753,- -32768,- -32768,- -32768,81,- -32768,- -32768,56,- 246,1425,- 334,330,- -32768,1320,- -32768,1132,- -32768,1461,- 584,- 331,- -32768,707,- 144,1223,241,- 361,- 586,57,- -32768,- -32768,- 73,- 555,- -32768,717,738,- 340,- 494,- 135,- 508,- 146,- -32768,- 311,- 183,- 628,- 370,- -32768,1047,- 189,- 111,- 166,- 186,- 452,- 226,- 746,- 303,- -32768,- -32768,- -32768,1170,- 87,- 536,- 294,- -32768,- 790,- -32768,- 720,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,172,- -32768,- -32768,836,- 213,- -32768,497,917,- -32768,- 180,82,- 182,- 421,- 413,- 432,1095,- 414,- 425,- 429,- 358,- 433,522,527,520,531,- 193,712,637,1226,- -32768,- -32768,- -32768,487,- -32768,- 1003,414,- -32768,1133,332,- -32768,- 428,- -32768,- 21,561,- 71,- 75,- 97,- 102,569,- -32768,582,- 967,- 976,481};
# 2266
static short Cyc_yytable[8464U]={70,160,672,566,307,159,114,282,283,562,669,616,576,73,675,676,679,398,399,395,671,70,674,385,373,70,370,642,164,611,801,423,298,583,73,428,425,802,73,321,590,298,741,685,753,257,623,496,532,906,389,292,136,595,467,1134,597,598,798,600,547,846,70,781,925,718,322,757,758,70,70,70,89,70,511,73,607,114,331,431,70,172,73,73,73,508,73,677,678,114,539,1098,518,73,1037,1143,718,520,783,1152,323,164,329,1142,91,1144,71,741,137,926,144,754,962,330,786,351,520,478,1026,117,549,380,70,70,153,432,1057,71,129,1038,662,71,860,1203,479,73,73,1099,521,474,131,70,70,70,91,70,70,70,70,70,522,1058,608,70,73,73,73,521,73,73,73,73,73,890,776,70,73,70,71,1139,287,288,475,172,1140,71,71,71,73,71,73,524,723,520,1057,95,71,1025,529,309,310,311,303,312,313,314,315,316,1233,113,734,634,429,635,636,499,540,1091,631,1132,954,- 174,45,92,734,844,857,96,118,1065,603,48,332,536,521,657,1067,93,71,71,1236,119,1238,114,565,536,114,1234,1235,114,1237,630,1239,265,912,114,748,71,71,71,266,71,71,71,71,71,536,45,805,71,680,154,155,156,604,157,48,624,522,330,253,71,166,71,450,429,510,963,1260,52,966,430,261,629,129,728,704,130,1261,512,778,974,120,451,452,351,131,809,351,351,728,351,1163,295,1114,631,292,1168,511,511,511,296,121,175,176,177,178,179,429,642,180,70,1182,181,182,183,254,1028,842,184,45,816,1044,185,73,124,900,1045,47,48,49,1042,70,454,855,166,255,70,51,45,70,70,531,70,522,73,122,166,48,352,73,55,56,73,73,291,73,798,798,186,45,683,351,189,332,686,45,1220,47,48,49,328,627,1066,47,48,49,693,295,930,631,1068,531,707,708,1185,296,455,456,457,458,459,460,461,462,463,464,605,599,1256,440,441,429,174,1045,856,689,726,495,508,1043,369,913,868,70,522,477,71,383,465,- 252,254,1245,522,- 252,123,889,73,501,927,268,269,270,271,125,936,126,611,71,442,443,255,254,71,1257,128,71,71,- 601,71,536,307,45,536,778,114,- 289,289,642,- 289,839,48,255,114,591,740,133,536,114,1029,50,869,592,911,578,780,45,579,169,522,170,134,782,869,47,48,49,171,468,469,470,847,91,848,935,138,273,749,546,515,510,510,510,166,756,516,516,317,139,722,318,508,845,512,512,512,696,832,469,470,71,727,769,733,96,484,485,1191,141,352,142,777,352,352,471,352,739,164,50,631,472,473,50,862,143,70,719,70,720,70,730,86,731,447,872,721,448,694,73,732,73,429,73,471,631,602,863,530,429,833,473,1034,149,94,70,874,1041,873,115,936,116,161,232,70,550,996,319,73,189,70,551,1022,320,744,259,745,73,746,517,429,609,369,73,86,615,664,352,51,383,51,291,515,293,86,534,1047,19,20,516,86,55,56,55,56,937,938,548,158,- 18,807,162,429,191,86,946,947,437,438,935,429,1136,1126,750,195,194,817,536,534,695,115,1058,71,956,71,1201,71,701,258,740,1172,1173,115,256,914,1132,1179,1176,1169,114,1175,260,536,1123,86,86,114,429,1171,429,1174,71,1225,295,1124,936,262,1221,86,1222,71,296,522,86,86,86,71,86,86,86,86,86,286,631,263,932,429,788,789,722,790,264,827,426,45,429,351,1112,86,330,791,898,284,48,97,733,948,949,631,631,631,631,51,299,957,703,52,429,429,1177,1178,45,304,899,935,55,56,194,45,429,48,964,498,50,609,904,1157,48,300,51,363,971,150,52,1145,1146,51,166,70,429,52,70,55,56,751,968,166,752,1161,55,56,73,166,45,73,1147,1148,1149,376,429,550,47,48,49,1167,1113,1160,294,429,1081,98,1082,871,1096,1231,773,115,377,429,115,306,807,115,145,146,99,100,1111,115,101,102,103,896,104,48,897,386,893,363,589,105,1156,327,106,378,1214,517,107,1215,108,109,306,976,977,978,609,535,147,148,1089,110,379,534,387,1128,979,980,981,535,388,383,445,446,982,16,17,18,548,392,854,86,114,1223,400,71,401,70,71,426,535,574,426,1186,1187,1188,1189,580,402,643,73,645,646,647,618,619,1164,403,1165,653,391,404,394,396,396,86,757,758,1180,159,711,712,713,1246,814,815,901,902,958,959,1244,670,97,418,419,987,45,405,396,1192,1063,1064,396,406,47,48,49,1102,1103,194,1125,1106,1107,407,51,1227,429,509,531,514,420,396,408,570,409,212,410,55,56,411,351,352,687,412,166,1219,992,413,607,994,995,1035,1036,414,268,269,270,271,415,416,417,427,70,71,433,436,434,435,476,480,494,1030,1031,1032,1033,73,497,327,504,505,1230,272,1159,1190,525,526,159,527,131,528,538,543,70,45,100,221,222,101,102,103,609,544,48,545,567,73,552,554,556,794,1086,558,568,1243,575,393,91,351,226,115,273,585,577,302,608,581,394,115,584,534,1258,586,115,587,45,593,348,596,999,1000,605,534,47,48,49,1018,762,763,612,522,766,613,51,771,534,97,546,159,617,621,375,91,50,516,535,55,56,535,71,70,628,633,396,431,654,661,688,45,699,535,663,535,73,690,691,47,48,49,86,50,86,692,86,932,698,51,700,709,71,531,706,710,702,705,724,725,738,742,55,56,735,729,1159,736,747,396,159,396,396,396,396,737,764,396,765,768,767,772,396,396,396,98,774,396,609,609,609,396,779,609,784,787,351,803,804,811,252,100,806,810,101,102,103,812,104,48,813,818,- 601,819,820,105,821,826,106,822,823,306,1001,396,108,109,824,828,825,71,829,830,836,227,841,110,229,831,352,230,231,834,835,837,840,843,849,866,70,852,861,858,306,867,859,870,306,877,883,885,886,73,892,564,894,905,907,429,7,8,9,10,11,12,13,14,15,16,17,18,19,20,908,909,910,369,115,917,915,924,918,838,771,115,922,933,919,22,23,24,920,26,306,921,923,943,950,952,396,960,961,965,306,967,535,352,97,32,969,622,970,306,972,973,975,983,535,984,33,34,988,985,989,997,986,998,1021,1024,993,535,564,1027,40,1040,644,1048,41,895,648,649,650,651,652,1020,71,655,656,42,1049,659,660,43,1050,1052,1051,1053,1054,44,1055,1076,665,1079,668,1056,97,1062,1069,1070,1071,1072,1073,1074,1075,1080,1085,1090,1088,1087,1092,1100,1095,1097,98,1110,1115,1116,1122,681,1101,1117,91,396,1130,1131,481,50,1118,482,100,682,97,101,102,103,665,483,48,53,54,1119,1120,1121,105,130,1166,106,306,1153,1023,107,771,108,109,1154,1155,484,485,306,1162,1183,352,1197,110,1184,1196,509,1194,1195,1199,1057,98,1204,1205,97,1206,1207,1046,1208,1209,717,1211,1218,850,1229,1213,482,100,396,1210,101,102,103,86,483,48,1217,1216,1224,1226,1228,105,1232,1249,106,1240,1247,98,107,115,108,109,97,396,484,485,1248,1250,1251,500,1252,110,252,100,1254,1253,101,102,103,1255,104,48,1259,1262,1267,1268,1263,105,1264,1265,106,1241,1212,305,107,259,108,109,884,127,98,991,891,569,173,384,665,110,625,560,665,929,853,140,928,252,100,537,771,101,102,103,916,104,48,601,396,673,1104,1108,105,697,851,106,1105,1019,396,107,98,108,109,1109,561,1137,1202,658,158,1242,86,1077,110,396,1151,252,100,0,564,101,102,103,0,104,48,1078,0,0,0,327,105,0,0,106,0,0,0,107,0,108,109,0,0,0,0,0,0,0,0,426,110,0,0,0,0,1170,0,0,0,0,0,0,0,396,0,564,1181,306,0,0,0,771,0,0,0,0,0,0,0,0,0,0,1193,0,0,0,0,1198,396,396,396,396,0,0,0,0,0,865,0,0,0,0,0,0,0,158,0,0,876,0,0,0,882,0,0,0,0,0,0,0,0,0,0,564,0,0,0,0,0,0,0,0,306,0,0,0,0,0,0,0,665,0,0,0,0,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,1094,0,0,0,0,0,158,0,0,22,23,24,25,26,0,27,0,0,0,0,396,564,0,0,28,939,940,941,942,32,944,945,0,0,0,0,0,0,0,0,33,34,35,0,564,0,0,955,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,158,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,396,0,396,396,396,396,396,396,396,396,396,396,0,91,47,0,49,0,50,319,0,163,0,0,0,320,0,0,0,0,53,54,0,0,0,0,0,0,0,0,0,0,0,564,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,564,0,396,0,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,901,902,21,196,197,333,0,334,335,336,337,338,339,340,341,22,23,24,342,26,97,27,199,343,0,0,0,0,200,201,28,344,0,0,396,32,202,203,204,205,206,207,208,209,0,210,33,34,35,211,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,1127,0,1129,0,0,0,42,0,0,0,43,217,218,564,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,345,100,221,222,101,102,103,91,47,48,49,0,50,223,346,347,0,0,224,0,0,0,225,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,196,197,333,0,334,335,336,337,338,339,340,341,22,23,24,342,26,97,27,199,343,0,0,0,0,200,201,28,344,0,0,0,32,202,203,204,205,206,207,208,209,0,210,33,34,35,211,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,0,0,0,0,0,0,42,0,0,0,43,217,218,0,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,345,100,221,222,101,102,103,91,47,48,49,0,50,223,346,347,348,0,224,0,0,0,225,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,196,197,333,0,334,335,336,337,338,339,340,341,22,23,24,342,26,97,27,199,343,0,0,0,0,200,201,28,344,0,0,0,32,202,203,204,205,206,207,208,209,0,210,33,34,35,211,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,0,0,0,0,0,0,42,0,0,0,43,217,218,0,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,345,100,221,222,101,102,103,91,47,48,49,0,50,223,346,347,594,0,224,0,0,0,225,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,196,197,333,0,334,335,336,337,338,339,340,341,22,23,24,342,26,97,27,199,343,0,0,0,0,200,201,28,344,0,0,0,32,202,203,204,205,206,207,208,209,0,210,33,34,35,211,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,0,0,0,0,0,0,42,0,0,0,43,217,218,0,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,345,100,221,222,101,102,103,91,47,48,49,0,50,223,346,347,0,0,224,0,0,0,225,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,196,197,333,0,334,335,336,337,338,339,340,341,22,23,24,342,26,97,27,199,343,0,0,0,0,200,201,28,344,0,0,0,32,202,203,204,205,206,207,208,209,0,210,33,34,35,211,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,0,0,0,0,0,0,42,0,0,0,43,217,218,0,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,582,100,221,222,101,102,103,91,47,48,49,0,50,223,346,347,0,0,224,0,0,0,225,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,196,197,0,0,0,0,0,0,0,0,0,0,22,23,24,342,26,97,27,199,0,0,0,0,0,200,201,28,0,0,0,0,32,202,203,204,205,206,207,208,209,0,210,33,34,35,211,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,0,0,0,0,0,0,42,0,0,0,43,217,218,0,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,91,47,48,49,0,50,223,0,0,0,0,224,0,0,0,225,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,196,197,0,0,0,0,0,0,0,0,0,0,22,23,24,342,26,97,0,0,0,0,0,0,0,0,0,28,0,0,0,0,32,202,203,204,205,206,207,208,209,0,210,33,34,0,0,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,0,0,0,0,0,0,42,0,0,0,43,217,218,0,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,91,47,48,49,0,50,223,0,0,0,0,224,0,0,0,393,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,22,23,24,198,26,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,32,202,203,204,205,206,207,208,209,0,210,33,34,0,211,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,0,0,0,0,0,0,42,0,0,0,43,217,218,0,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,91,47,48,49,0,50,223,0,424,0,0,224,0,0,0,225,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,196,197,0,0,0,0,0,0,0,0,0,0,22,23,24,198,26,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,32,202,203,204,205,206,207,208,209,0,210,33,34,0,211,0,0,0,212,0,0,0,0,0,0,40,213,214,215,41,216,0,0,0,0,0,0,0,0,0,42,0,0,0,43,217,218,0,0,0,44,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,91,47,48,49,0,50,223,0,0,0,0,224,0,0,0,225,0,53,349,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231,- 15,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,36,37,0,38,39,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,0,0,0,0,0,0,91,47,48,49,0,50,0,0,0,- 15,0,51,0,0,0,52,0,53,54,0,0,0,0,0,55,56,- 15,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,36,37,0,38,39,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,0,0,0,0,0,0,46,47,48,49,0,50,0,0,0,0,0,51,0,0,0,52,0,53,54,0,0,0,0,0,55,56,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,36,37,0,38,39,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,45,0,0,0,0,0,0,91,47,48,49,0,50,22,23,24,- 15,26,51,0,0,0,52,0,53,54,0,0,0,0,0,55,56,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,367,0,0,45,0,0,0,0,0,0,91,47,48,49,0,50,0,369,0,0,0,51,0,0,0,546,0,53,54,0,0,0,516,0,55,56,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,22,23,24,0,26,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,32,0,0,0,0,42,0,0,0,43,0,33,34,0,0,44,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,367,0,42,0,0,0,43,0,0,0,91,47,44,49,0,50,0,369,0,0,0,51,0,0,0,515,0,53,54,0,0,0,516,0,55,56,0,0,45,0,0,0,0,0,0,91,47,48,49,0,50,0,0,0,0,0,51,0,0,0,531,0,53,54,0,0,0,0,0,55,56,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,91,47,0,49,0,50,319,0,347,0,0,0,320,0,0,0,0,53,54,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,91,47,0,49,0,50,0,0,163,22,23,24,25,26,0,27,0,53,54,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,2,3,4,90,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,91,47,0,49,0,50,0,0,347,22,23,24,25,26,0,0,0,53,54,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,91,47,0,49,0,50,22,23,24,0,26,0,0,0,0,0,0,53,54,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,267,0,0,0,0,0,268,269,270,271,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,272,0,44,0,0,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,91,47,0,49,273,50,0,0,274,22,23,24,0,26,0,0,0,53,54,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,367,0,0,368,0,0,0,0,0,0,91,47,0,49,0,50,0,369,0,0,0,0,0,0,0,0,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,22,23,24,0,26,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,32,0,0,0,0,42,0,0,0,43,0,33,34,0,0,44,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,367,0,42,0,0,0,43,0,0,0,91,47,44,49,0,50,0,369,0,0,0,0,0,0,0,0,0,53,54,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,91,47,0,49,0,50,0,0,0,506,0,0,0,0,0,0,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,22,23,24,0,26,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,32,0,0,0,0,42,0,0,0,43,0,33,34,0,0,44,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,91,47,44,49,0,50,0,0,0,785,0,0,0,0,0,0,0,53,54,0,0,0,0,0,0,800,0,0,0,0,0,0,0,0,0,91,47,0,49,0,50,0,0,0,0,0,0,0,0,0,0,0,53,54,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,42,0,0,0,43,0,0,0,0,0,44,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,196,197,333,0,334,335,336,337,338,339,340,341,0,91,47,198,49,97,50,199,343,0,0,0,0,200,201,0,344,0,53,54,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,582,100,221,222,101,102,103,0,0,48,0,0,0,223,346,347,0,0,224,0,0,0,225,0,0,226,0,196,197,0,0,0,0,227,637,228,229,0,0,230,231,0,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,638,100,221,222,101,102,103,0,296,48,0,0,0,223,0,397,639,0,224,0,0,0,225,0,0,226,196,197,484,485,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,638,100,221,222,101,102,103,0,296,48,0,0,0,223,0,397,934,0,224,0,0,0,225,0,0,226,196,197,484,485,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,638,100,221,222,101,102,103,0,296,48,0,0,0,223,0,397,953,0,224,0,0,0,225,0,0,226,196,197,484,485,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,638,100,221,222,101,102,103,0,296,48,0,0,0,223,0,397,1138,0,224,0,0,0,225,0,0,226,196,197,484,485,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,638,100,221,222,101,102,103,0,296,48,0,0,0,223,0,397,0,0,224,0,0,0,225,0,0,226,196,197,484,485,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,301,0,48,0,0,0,223,0,0,0,0,224,0,0,0,225,0,0,226,196,197,0,0,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,0,0,0,224,0,0,0,225,0,0,226,196,197,0,0,374,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,397,0,0,224,0,0,0,225,0,0,226,196,197,0,0,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,588,0,0,0,224,0,0,0,225,0,0,226,196,197,0,0,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,0,0,0,224,0,0,0,225,684,0,226,196,197,0,0,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,0,0,0,224,0,0,0,225,0,0,226,196,197,0,0,716,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,0,0,0,224,0,0,0,225,775,0,226,196,197,0,0,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,0,0,0,224,0,0,0,225,0,0,226,196,197,0,0,864,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,0,0,0,224,0,0,0,225,0,0,226,196,197,0,0,875,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,0,0,0,224,0,0,0,225,0,0,226,196,197,0,0,0,0,0,227,0,228,229,0,0,230,231,198,0,97,0,199,0,0,0,0,0,200,201,0,0,0,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,211,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,881,100,221,222,101,102,103,0,0,48,0,196,197,223,0,0,0,0,224,0,0,0,225,0,0,226,198,0,97,0,0,0,0,227,0,228,229,0,0,230,231,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,0,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,196,197,223,0,397,0,0,224,0,0,0,225,0,0,226,198,0,97,0,0,0,0,227,0,228,229,0,0,230,231,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,0,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,196,197,223,0,838,0,0,224,0,0,0,225,0,0,226,198,0,97,0,0,0,0,227,0,228,229,0,0,230,231,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,0,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,196,197,223,0,0,0,0,224,0,0,0,390,0,0,226,198,0,97,0,0,0,0,227,0,228,229,0,0,230,231,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,0,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,196,197,223,0,0,0,0,224,0,0,0,393,0,0,226,198,0,97,0,0,0,0,227,0,228,229,0,0,230,231,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,0,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,196,197,223,0,0,0,0,224,0,0,0,225,0,0,226,198,0,97,0,0,0,0,227,0,228,229,0,0,230,231,0,0,0,202,203,204,205,206,207,208,209,0,210,0,0,0,0,0,0,0,212,0,0,0,0,0,0,0,213,214,215,0,216,0,0,0,0,0,0,0,0,0,0,0,0,0,0,217,218,0,0,0,0,0,0,219,220,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,45,100,221,222,101,102,103,0,0,48,0,0,0,223,0,0,0,0,224,0,0,0,1093,0,0,226,0,0,0,0,0,0,0,227,0,228,229,0,0,230,231};
# 3116
static short Cyc_yycheck[8464U]={0,76,434,322,139,76,27,118,119,320,431,372,331,0,439,444,449,200,200,199,433,21,436,189,170,25,170,397,80,369,616,224,129,336,21,228,225,621,25,153,343,138,550,471,93,105,377,260,294,769,195,124,52,347,247,1058,350,351,613,353,306,689,62,599,114,517,153,5,6,69,70,71,135,73,276,62,72,98,165,119,80,81,69,70,71,274,73,445,446,110,17,119,278,80,105,1071,548,113,606,1075,160,153,162,1070,140,1072,0,615,52,159,62,160,858,165,608,167,113,140,908,133,306,157,122,123,68,165,135,21,132,140,424,25,716,1136,157,122,123,165,154,131,144,141,142,143,140,145,146,147,148,149,166,160,148,153,141,142,143,154,145,146,147,148,149,749,592,165,153,167,62,135,122,123,164,173,140,69,70,71,165,73,167,283,522,113,135,155,80,907,290,141,142,143,136,145,146,147,148,149,1201,27,534,390,150,392,393,265,133,997,388,160,838,149,133,21,548,163,710,25,153,965,150,142,166,294,154,414,972,148,122,123,1206,153,1208,254,321,306,257,1204,1205,260,1207,386,1209,148,780,266,565,141,142,143,155,145,146,147,148,149,327,133,156,153,453,69,70,71,366,73,142,378,166,321,98,165,80,167,151,150,276,861,1254,155,864,156,110,385,132,531,499,135,1255,276,593,875,133,170,171,347,144,628,350,351,546,353,1092,133,1024,485,379,1097,510,511,512,141,153,88,89,90,91,92,150,689,95,321,1112,98,99,100,133,913,160,104,133,642,156,108,321,153,764,161,141,142,143,927,342,66,156,153,153,347,151,133,350,351,155,353,166,342,147,165,142,167,347,164,165,350,351,124,353,922,923,144,133,468,424,153,318,472,133,1167,141,142,143,161,382,967,141,142,143,484,133,134,570,975,155,504,505,1115,141,120,121,122,123,124,125,126,127,128,129,133,352,156,114,115,150,83,161,156,476,528,156,608,928,147,783,156,424,166,254,321,187,152,156,133,1222,166,160,147,747,424,266,800,78,79,80,81,155,815,155,787,342,153,154,153,133,347,1244,57,350,351,160,353,531,596,133,534,767,486,147,105,838,150,663,142,153,494,147,546,155,548,499,156,145,727,155,777,155,599,133,158,153,166,155,148,603,739,141,142,143,162,109,110,111,690,140,692,815,132,144,567,155,155,510,511,512,321,574,162,162,147,155,519,150,710,688,510,511,512,152,109,110,111,424,531,584,533,342,161,162,1123,147,347,147,593,350,351,155,353,546,599,145,729,161,162,145,719,148,555,153,557,155,559,153,0,155,166,730,162,169,485,555,162,557,150,559,155,754,147,720,156,150,161,162,921,148,22,584,731,926,731,27,959,29,148,95,593,150,898,146,584,153,599,156,904,152,555,107,557,593,559,278,150,369,147,599,52,150,156,424,151,377,151,379,155,125,62,294,936,20,21,162,68,164,165,164,165,817,817,306,76,148,626,79,150,135,82,827,827,116,117,959,150,150,160,570,148,93,156,727,327,486,98,160,555,842,557,150,559,494,133,739,1101,1102,110,153,784,160,1108,1105,1098,699,1104,155,752,1043,122,123,706,150,1100,150,1103,584,156,133,134,1064,147,160,136,160,593,141,166,141,142,143,599,145,146,147,148,149,140,892,150,811,150,93,94,718,96,152,156,225,133,150,777,1020,166,780,106,156,152,142,42,734,832,833,917,918,919,920,151,150,845,147,155,150,150,1106,1107,133,156,156,1064,164,165,195,133,150,142,862,264,145,522,156,1084,142,155,151,147,872,147,155,152,153,151,584,777,150,155,780,164,165,147,156,593,150,1090,164,165,777,599,133,780,173,174,175,155,150,150,141,142,143,1097,156,156,126,150,149,118,151,729,1001,156,147,254,155,150,257,139,807,260,147,148,133,134,1019,266,137,138,139,147,141,142,150,150,754,147,341,148,150,161,151,155,147,515,155,150,157,158,170,152,153,154,613,294,147,148,993,168,155,531,149,1052,165,166,167,306,152,628,112,113,173,17,18,19,546,155,706,318,901,1184,155,777,155,885,780,390,327,328,393,1117,1118,1119,1120,334,155,400,885,402,403,404,99,100,1093,155,1095,410,196,155,198,199,200,352,5,6,1109,988,510,511,512,1224,149,150,22,23,149,150,1222,432,42,219,220,885,133,155,224,1126,149,150,228,155,141,142,143,167,168,386,1045,112,113,155,151,149,150,275,155,277,18,247,155,160,155,75,155,164,165,155,1020,777,473,155,780,1166,892,155,72,896,897,922,923,155,78,79,80,81,155,155,155,155,990,885,118,168,159,167,148,140,133,917,918,919,920,990,133,325,155,155,1197,105,1085,1122,133,147,1085,154,144,155,135,149,1020,133,134,135,136,137,138,139,787,149,142,156,148,1020,149,149,149,133,990,149,149,1221,148,155,140,1097,158,486,144,133,155,134,148,155,342,494,155,727,1247,147,499,147,133,160,149,155,25,26,133,739,141,142,143,901,577,578,156,166,581,160,151,584,752,42,155,1156,156,163,171,140,145,162,531,164,165,534,990,1097,166,149,388,119,133,155,153,133,150,546,156,548,1097,154,154,141,142,143,555,145,557,154,559,1218,149,151,160,149,1020,155,150,150,156,156,155,133,150,48,164,165,155,159,1215,155,152,431,1215,433,434,435,436,156,155,439,148,30,160,147,444,445,446,118,46,449,921,922,923,453,149,926,155,166,1222,156,156,150,133,134,156,156,137,138,139,156,141,142,133,156,160,150,150,148,150,156,151,150,150,515,155,485,157,158,150,156,150,1097,156,156,135,166,150,168,169,156,1020,172,173,156,156,149,156,156,163,724,1222,149,163,156,546,147,156,156,550,135,156,148,47,1222,155,320,149,155,147,150,8,9,10,11,12,13,14,15,16,17,18,19,20,21,148,147,156,147,699,155,149,115,155,148,769,706,150,153,155,37,38,39,155,41,596,155,150,25,160,156,570,133,133,156,606,163,727,1097,42,57,156,376,156,615,156,156,163,156,739,147,68,69,148,150,49,148,156,156,149,147,155,752,397,147,82,140,401,156,86,760,405,406,407,408,409,160,1222,412,413,97,156,416,417,101,156,155,150,150,150,107,156,140,427,134,429,156,42,156,155,155,155,155,155,155,155,149,148,28,156,149,148,118,155,160,118,118,147,149,155,454,159,156,140,663,25,155,130,145,156,133,134,466,42,137,138,139,471,141,142,157,158,156,156,156,148,135,156,151,720,150,905,155,907,157,158,149,149,161,162,731,149,147,1222,155,168,156,150,739,156,156,135,135,118,150,150,42,150,150,933,150,150,516,134,150,130,25,149,133,134,729,156,137,138,139,885,141,142,156,149,156,149,156,148,150,135,151,156,155,118,155,901,157,158,42,754,161,162,156,156,156,130,156,168,133,134,150,156,137,138,139,150,141,142,156,156,0,0,156,148,156,156,151,1215,1156,138,155,1001,157,158,738,43,118,890,752,325,82,188,592,168,379,318,596,807,130,55,800,133,134,297,1024,137,138,139,787,141,142,358,817,435,1009,1012,148,487,699,151,1010,901,827,155,118,157,158,1013,319,1059,1133,415,988,1218,990,983,168,842,1074,133,134,- 1,642,137,138,139,- 1,141,142,984,- 1,- 1,- 1,890,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1093,168,- 1,- 1,- 1,- 1,1099,- 1,- 1,- 1,- 1,- 1,- 1,- 1,892,- 1,689,1110,928,- 1,- 1,- 1,1115,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1126,- 1,- 1,- 1,- 1,1131,917,918,919,920,- 1,- 1,- 1,- 1,- 1,721,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1085,- 1,- 1,732,- 1,- 1,- 1,736,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,747,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,993,- 1,- 1,- 1,- 1,- 1,- 1,- 1,764,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,999,- 1,- 1,- 1,- 1,- 1,1156,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,1019,815,- 1,- 1,52,819,820,821,822,57,824,825,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,838,- 1,- 1,841,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,1215,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1098,- 1,1100,1101,1102,1103,1104,1105,1106,1107,1108,1109,- 1,140,141,- 1,143,- 1,145,146,- 1,148,- 1,- 1,- 1,152,- 1,- 1,- 1,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,936,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,959,- 1,1166,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,1221,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,1051,- 1,1053,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,1064,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,147,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,147,148,149,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,147,148,149,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,147,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,147,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,- 1,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,102,103,- 1,- 1,- 1,107,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,141,142,143,- 1,145,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,- 1,76,77,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,- 1,140,141,142,143,- 1,145,- 1,- 1,- 1,149,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,164,165,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,- 1,76,77,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,- 1,140,141,142,143,- 1,145,- 1,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,164,165,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,- 1,76,77,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,- 1,140,141,142,143,- 1,145,37,38,39,149,41,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,164,165,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,- 1,140,141,142,143,- 1,145,- 1,147,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,162,- 1,164,165,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,68,69,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,140,141,107,143,- 1,145,- 1,147,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,162,- 1,164,165,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,- 1,140,141,142,143,- 1,145,- 1,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,157,158,- 1,- 1,- 1,- 1,- 1,164,165,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,140,141,- 1,143,- 1,145,146,- 1,148,- 1,- 1,- 1,152,- 1,- 1,- 1,- 1,157,158,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,140,141,- 1,143,- 1,145,- 1,- 1,148,37,38,39,40,41,- 1,43,- 1,157,158,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,140,141,- 1,143,- 1,145,- 1,- 1,148,37,38,39,40,41,- 1,- 1,- 1,157,158,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,140,141,- 1,143,- 1,145,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,105,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,140,141,- 1,143,144,145,- 1,- 1,148,37,38,39,- 1,41,- 1,- 1,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,133,- 1,- 1,- 1,- 1,- 1,- 1,140,141,- 1,143,- 1,145,- 1,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,157,158,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,68,69,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,140,141,107,143,- 1,145,- 1,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,140,141,- 1,143,- 1,145,- 1,- 1,- 1,149,- 1,- 1,- 1,- 1,- 1,- 1,- 1,157,158,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,68,69,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,140,141,107,143,- 1,145,- 1,- 1,- 1,149,- 1,- 1,- 1,- 1,- 1,- 1,- 1,157,158,- 1,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,140,141,- 1,143,- 1,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,157,158,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,140,141,40,143,42,145,44,45,- 1,- 1,- 1,- 1,50,51,- 1,53,- 1,157,158,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,147,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,- 1,25,26,- 1,- 1,- 1,- 1,166,32,168,169,- 1,- 1,172,173,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,141,142,- 1,- 1,- 1,146,- 1,148,149,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,161,162,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,141,142,- 1,- 1,- 1,146,- 1,148,149,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,161,162,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,141,142,- 1,- 1,- 1,146,- 1,148,149,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,161,162,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,141,142,- 1,- 1,- 1,146,- 1,148,149,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,161,162,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,141,142,- 1,- 1,- 1,146,- 1,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,161,162,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,140,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,- 1,- 1,163,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,147,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,156,- 1,158,25,26,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,- 1,- 1,163,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,156,- 1,158,25,26,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,- 1,- 1,163,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,- 1,- 1,163,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,25,26,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,25,26,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,40,- 1,42,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,25,26,146,- 1,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,40,- 1,42,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,25,26,146,- 1,148,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,40,- 1,42,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,25,26,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,40,- 1,42,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,25,26,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,40,- 1,42,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,25,26,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,40,- 1,42,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,102,103,- 1,- 1,- 1,- 1,- 1,- 1,110,111,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,133,134,135,136,137,138,139,- 1,- 1,142,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,151,- 1,- 1,- 1,155,- 1,- 1,158,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,- 1,168,169,- 1,- 1,172,173};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int,int);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*,struct Cyc_Yyltype*);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple33{unsigned f0;struct _tuple0*f1;int f2;};struct _tuple34{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple35{void*f0;struct Cyc_List_List*f1;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){union Cyc_YYSTYPE _T0;struct _fat_ptr _T1;struct _RegionHandle*_T2;void*_T3;struct Cyc_Yystacktype*_T4;struct Cyc_Yystacktype*_T5;struct _RegionHandle*_T6;unsigned _T7;unsigned _T8;unsigned _T9;struct _fat_ptr _TA;int _TB;unsigned char*_TC;short*_TD;int _TE;int _TF;int _T10;int _T11;struct _fat_ptr _T12;int _T13;int _T14;struct Cyc_Yystack_overflow_exn_struct*_T15;struct Cyc_Yystack_overflow_exn_struct*_T16;struct _fat_ptr _T17;int _T18;short*_T19;struct _RegionHandle*_T1A;unsigned _T1B;unsigned _T1C;int _T1D;unsigned _T1E;unsigned _T1F;struct _fat_ptr _T20;unsigned char*_T21;short*_T22;unsigned _T23;int _T24;unsigned _T25;struct _fat_ptr _T26;int _T27;struct Cyc_Yystacktype*_T28;struct _RegionHandle*_T29;unsigned _T2A;unsigned _T2B;int _T2C;unsigned _T2D;unsigned _T2E;struct _fat_ptr _T2F;unsigned _T30;int _T31;unsigned char*_T32;struct Cyc_Yystacktype*_T33;unsigned _T34;struct _fat_ptr _T35;unsigned char*_T36;struct Cyc_Yystacktype*_T37;short*_T38;int _T39;char*_T3A;short*_T3B;short _T3C;int _T3D;int _T3E;int _T3F;int _T40;struct Cyc_Lexing_lexbuf*_T41;union Cyc_YYSTYPE*_T42;union Cyc_YYSTYPE*_T43;struct Cyc_Yyltype*_T44;struct Cyc_Yyltype*_T45;int _T46;short*_T47;int _T48;short _T49;int _T4A;short*_T4B;int _T4C;short _T4D;int _T4E;int _T4F;short*_T50;int _T51;short _T52;int _T53;int _T54;struct _fat_ptr _T55;int _T56;unsigned char*_T57;struct Cyc_Yystacktype*_T58;struct Cyc_Yystacktype _T59;short*_T5A;int _T5B;char*_T5C;short*_T5D;short _T5E;short*_T5F;int _T60;char*_T61;short*_T62;short _T63;struct _fat_ptr _T64;int _T65;int _T66;int _T67;struct _fat_ptr _T68;unsigned char*_T69;unsigned char*_T6A;struct Cyc_Yystacktype*_T6B;struct Cyc_Yystacktype _T6C;int _T6D;int _T6E;struct Cyc_Yystacktype*_T6F;union Cyc_YYSTYPE*_T70;union Cyc_YYSTYPE*_T71;struct Cyc_Yystacktype*_T72;struct Cyc_Yystacktype _T73;struct Cyc_Yystacktype*_T74;union Cyc_YYSTYPE*_T75;union Cyc_YYSTYPE*_T76;struct Cyc_Yystacktype*_T77;union Cyc_YYSTYPE*_T78;union Cyc_YYSTYPE*_T79;struct Cyc_List_List*_T7A;struct Cyc_Yystacktype*_T7B;union Cyc_YYSTYPE*_T7C;union Cyc_YYSTYPE*_T7D;struct Cyc_List_List*_T7E;struct Cyc_List_List*_T7F;struct Cyc_List_List*_T80;struct Cyc_Absyn_Decl*_T81;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T82;struct Cyc_Yystacktype*_T83;union Cyc_YYSTYPE*_T84;union Cyc_YYSTYPE*_T85;struct Cyc_Yystacktype*_T86;union Cyc_YYSTYPE*_T87;union Cyc_YYSTYPE*_T88;struct Cyc_Yystacktype*_T89;struct Cyc_Yystacktype _T8A;struct Cyc_Yyltype _T8B;unsigned _T8C;struct Cyc_List_List*_T8D;struct Cyc_Absyn_Decl*_T8E;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T8F;struct Cyc_Yystacktype*_T90;union Cyc_YYSTYPE*_T91;union Cyc_YYSTYPE*_T92;struct Cyc_Yystacktype*_T93;union Cyc_YYSTYPE*_T94;union Cyc_YYSTYPE*_T95;struct Cyc_Yystacktype*_T96;struct Cyc_Yystacktype _T97;struct Cyc_Yyltype _T98;unsigned _T99;struct Cyc_Yystacktype*_T9A;union Cyc_YYSTYPE*_T9B;union Cyc_YYSTYPE*_T9C;struct Cyc_List_List*_T9D;struct Cyc_Absyn_Decl*_T9E;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T9F;struct _fat_ptr*_TA0;struct Cyc_Yystacktype*_TA1;union Cyc_YYSTYPE*_TA2;union Cyc_YYSTYPE*_TA3;struct Cyc_Yystacktype*_TA4;union Cyc_YYSTYPE*_TA5;union Cyc_YYSTYPE*_TA6;struct Cyc_Yystacktype*_TA7;struct Cyc_Yystacktype _TA8;struct Cyc_Yyltype _TA9;unsigned _TAA;struct Cyc_List_List*_TAB;struct Cyc_Absyn_Decl*_TAC;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_TAD;struct _fat_ptr*_TAE;struct Cyc_Yystacktype*_TAF;union Cyc_YYSTYPE*_TB0;union Cyc_YYSTYPE*_TB1;struct Cyc_Yystacktype*_TB2;union Cyc_YYSTYPE*_TB3;union Cyc_YYSTYPE*_TB4;struct Cyc_Yystacktype*_TB5;struct Cyc_Yystacktype _TB6;struct Cyc_Yyltype _TB7;unsigned _TB8;struct Cyc_Yystacktype*_TB9;union Cyc_YYSTYPE*_TBA;union Cyc_YYSTYPE*_TBB;struct Cyc_Yystacktype*_TBC;union Cyc_YYSTYPE*_TBD;union Cyc_YYSTYPE*_TBE;struct Cyc_List_List*_TBF;struct Cyc_Yystacktype*_TC0;union Cyc_YYSTYPE*_TC1;union Cyc_YYSTYPE*_TC2;struct Cyc_List_List*_TC3;struct Cyc_List_List*_TC4;struct Cyc_Yystacktype*_TC5;union Cyc_YYSTYPE*_TC6;union Cyc_YYSTYPE*_TC7;struct Cyc_Yystacktype*_TC8;union Cyc_YYSTYPE*_TC9;union Cyc_YYSTYPE*_TCA;struct Cyc_Yystacktype*_TCB;union Cyc_YYSTYPE*_TCC;union Cyc_YYSTYPE*_TCD;struct Cyc_Yystacktype*_TCE;union Cyc_YYSTYPE*_TCF;union Cyc_YYSTYPE*_TD0;struct Cyc_Yystacktype*_TD1;struct Cyc_Yystacktype _TD2;struct Cyc_Yyltype _TD3;unsigned _TD4;unsigned _TD5;struct _fat_ptr _TD6;struct _fat_ptr _TD7;struct Cyc_List_List*_TD8;unsigned _TD9;unsigned _TDA;int _TDB;struct Cyc_Yystacktype*_TDC;struct Cyc_Yystacktype _TDD;struct Cyc_Yyltype _TDE;unsigned _TDF;int _TE0;struct Cyc_Yystacktype*_TE1;struct Cyc_Yystacktype _TE2;struct Cyc_Yyltype _TE3;unsigned _TE4;unsigned _TE5;struct _fat_ptr _TE6;struct _fat_ptr _TE7;struct Cyc_List_List*_TE8;struct Cyc_Absyn_Decl*_TE9;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_TEA;struct Cyc_Yystacktype*_TEB;union Cyc_YYSTYPE*_TEC;union Cyc_YYSTYPE*_TED;struct _tuple10*_TEE;struct Cyc_Yystacktype*_TEF;struct Cyc_Yystacktype _TF0;struct Cyc_Yyltype _TF1;unsigned _TF2;struct Cyc_Yystacktype*_TF3;union Cyc_YYSTYPE*_TF4;union Cyc_YYSTYPE*_TF5;struct Cyc_List_List*_TF6;struct Cyc_Absyn_Decl*_TF7;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_TF8;struct Cyc_Yystacktype*_TF9;union Cyc_YYSTYPE*_TFA;union Cyc_YYSTYPE*_TFB;struct Cyc_Yystacktype*_TFC;struct Cyc_Yystacktype _TFD;struct Cyc_Yyltype _TFE;unsigned _TFF;struct Cyc_Yystacktype*_T100;union Cyc_YYSTYPE*_T101;union Cyc_YYSTYPE*_T102;struct Cyc_List_List*_T103;struct Cyc_Absyn_Decl*_T104;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T105;struct Cyc_Yystacktype*_T106;union Cyc_YYSTYPE*_T107;union Cyc_YYSTYPE*_T108;struct _tuple10*_T109;struct Cyc_Yystacktype*_T10A;struct Cyc_Yystacktype _T10B;struct Cyc_Yyltype _T10C;unsigned _T10D;struct Cyc_Yystacktype*_T10E;union Cyc_YYSTYPE*_T10F;union Cyc_YYSTYPE*_T110;struct Cyc_List_List*_T111;struct Cyc_Absyn_Decl*_T112;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T113;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T114;struct Cyc_Yystacktype*_T115;struct Cyc_Yystacktype _T116;struct Cyc_Yyltype _T117;unsigned _T118;struct Cyc_Yystacktype*_T119;union Cyc_YYSTYPE*_T11A;union Cyc_YYSTYPE*_T11B;struct Cyc_List_List*_T11C;struct Cyc_Absyn_Decl*_T11D;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T11E;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T11F;struct Cyc_Yystacktype*_T120;struct Cyc_Yystacktype _T121;struct Cyc_Yyltype _T122;unsigned _T123;struct Cyc_Yystacktype*_T124;union Cyc_YYSTYPE*_T125;union Cyc_YYSTYPE*_T126;struct Cyc_List_List*_T127;struct Cyc_Absyn_Decl*_T128;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T129;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T12A;struct Cyc_Yystacktype*_T12B;struct Cyc_Yystacktype _T12C;struct Cyc_Yyltype _T12D;unsigned _T12E;struct Cyc_Yystacktype*_T12F;union Cyc_YYSTYPE*_T130;union Cyc_YYSTYPE*_T131;struct Cyc_List_List*_T132;struct Cyc_Absyn_Decl*_T133;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T134;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T135;struct Cyc_Yystacktype*_T136;struct Cyc_Yystacktype _T137;struct Cyc_Yyltype _T138;unsigned _T139;struct Cyc_Yystacktype*_T13A;union Cyc_YYSTYPE*_T13B;union Cyc_YYSTYPE*_T13C;struct Cyc_Yystacktype*_T13D;union Cyc_YYSTYPE*_T13E;union Cyc_YYSTYPE*_T13F;struct _fat_ptr _T140;struct _fat_ptr _T141;int _T142;struct Cyc_Yystacktype*_T143;union Cyc_YYSTYPE*_T144;union Cyc_YYSTYPE*_T145;struct _fat_ptr _T146;struct _fat_ptr _T147;int _T148;struct Cyc_Yystacktype*_T149;struct Cyc_Yystacktype _T14A;struct Cyc_Yyltype _T14B;unsigned _T14C;unsigned _T14D;struct _fat_ptr _T14E;struct _fat_ptr _T14F;struct Cyc_Yystacktype*_T150;struct Cyc_Yystacktype _T151;struct Cyc_List_List*_T152;struct Cyc_Yystacktype*_T153;union Cyc_YYSTYPE*_T154;union Cyc_YYSTYPE*_T155;struct Cyc_List_List*_T156;struct Cyc_Yystacktype*_T157;union Cyc_YYSTYPE*_T158;union Cyc_YYSTYPE*_T159;struct Cyc_List_List*_T15A;struct Cyc_Yystacktype*_T15B;union Cyc_YYSTYPE*_T15C;union Cyc_YYSTYPE*_T15D;struct Cyc_Yystacktype*_T15E;union Cyc_YYSTYPE*_T15F;union Cyc_YYSTYPE*_T160;struct _tuple30*_T161;struct Cyc_Yystacktype*_T162;struct Cyc_Yystacktype _T163;struct _tuple30*_T164;struct Cyc_Yystacktype*_T165;union Cyc_YYSTYPE*_T166;union Cyc_YYSTYPE*_T167;struct _tuple30*_T168;struct _tuple30*_T169;struct Cyc_Yystacktype*_T16A;struct Cyc_Yystacktype _T16B;struct Cyc_Yyltype _T16C;unsigned _T16D;struct Cyc_List_List*_T16E;struct _tuple33*_T16F;struct Cyc_Yystacktype*_T170;struct Cyc_Yystacktype _T171;struct Cyc_Yyltype _T172;unsigned _T173;struct Cyc_Yystacktype*_T174;union Cyc_YYSTYPE*_T175;union Cyc_YYSTYPE*_T176;struct Cyc_List_List*_T177;struct _tuple33*_T178;struct Cyc_Yystacktype*_T179;struct Cyc_Yystacktype _T17A;struct Cyc_Yyltype _T17B;unsigned _T17C;struct Cyc_Yystacktype*_T17D;union Cyc_YYSTYPE*_T17E;union Cyc_YYSTYPE*_T17F;struct Cyc_Yystacktype*_T180;union Cyc_YYSTYPE*_T181;union Cyc_YYSTYPE*_T182;struct Cyc_Yystacktype*_T183;struct Cyc_Yystacktype _T184;struct Cyc_List_List*_T185;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T186;struct Cyc_Yystacktype*_T187;union Cyc_YYSTYPE*_T188;union Cyc_YYSTYPE*_T189;void*_T18A;struct Cyc_Yystacktype*_T18B;struct Cyc_Yystacktype _T18C;struct Cyc_Yyltype _T18D;unsigned _T18E;unsigned _T18F;struct Cyc_Yystacktype*_T190;struct Cyc_Yystacktype _T191;struct _RegionHandle*_T192;struct Cyc_Yystacktype*_T193;union Cyc_YYSTYPE*_T194;union Cyc_YYSTYPE*_T195;struct Cyc_Parse_Declarator _T196;struct Cyc_Yystacktype*_T197;union Cyc_YYSTYPE*_T198;union Cyc_YYSTYPE*_T199;struct Cyc_Absyn_Stmt*_T19A;struct Cyc_Yystacktype*_T19B;struct Cyc_Yystacktype _T19C;struct Cyc_Yyltype _T19D;unsigned _T19E;unsigned _T19F;struct Cyc_Absyn_Fndecl*_T1A0;struct Cyc_Yystacktype*_T1A1;union Cyc_YYSTYPE*_T1A2;union Cyc_YYSTYPE*_T1A3;struct _RegionHandle*_T1A4;struct Cyc_Parse_Declaration_spec*_T1A5;struct Cyc_Parse_Declaration_spec*_T1A6;struct Cyc_Yystacktype*_T1A7;union Cyc_YYSTYPE*_T1A8;union Cyc_YYSTYPE*_T1A9;struct Cyc_Parse_Declarator _T1AA;struct Cyc_Yystacktype*_T1AB;union Cyc_YYSTYPE*_T1AC;union Cyc_YYSTYPE*_T1AD;struct Cyc_Absyn_Stmt*_T1AE;struct Cyc_Yystacktype*_T1AF;struct Cyc_Yystacktype _T1B0;struct Cyc_Yyltype _T1B1;unsigned _T1B2;unsigned _T1B3;struct Cyc_Absyn_Fndecl*_T1B4;struct _RegionHandle*_T1B5;struct Cyc_Yystacktype*_T1B6;union Cyc_YYSTYPE*_T1B7;union Cyc_YYSTYPE*_T1B8;struct Cyc_Parse_Declarator _T1B9;struct Cyc_Yystacktype*_T1BA;union Cyc_YYSTYPE*_T1BB;union Cyc_YYSTYPE*_T1BC;struct Cyc_List_List*_T1BD;struct Cyc_Yystacktype*_T1BE;union Cyc_YYSTYPE*_T1BF;union Cyc_YYSTYPE*_T1C0;struct Cyc_Absyn_Stmt*_T1C1;struct Cyc_Yystacktype*_T1C2;struct Cyc_Yystacktype _T1C3;struct Cyc_Yyltype _T1C4;unsigned _T1C5;unsigned _T1C6;struct Cyc_Absyn_Fndecl*_T1C7;struct Cyc_Yystacktype*_T1C8;union Cyc_YYSTYPE*_T1C9;union Cyc_YYSTYPE*_T1CA;struct _RegionHandle*_T1CB;struct Cyc_Parse_Declaration_spec*_T1CC;struct Cyc_Parse_Declaration_spec*_T1CD;struct Cyc_Yystacktype*_T1CE;union Cyc_YYSTYPE*_T1CF;union Cyc_YYSTYPE*_T1D0;struct Cyc_Parse_Declarator _T1D1;struct Cyc_Yystacktype*_T1D2;union Cyc_YYSTYPE*_T1D3;union Cyc_YYSTYPE*_T1D4;struct Cyc_List_List*_T1D5;struct Cyc_Yystacktype*_T1D6;union Cyc_YYSTYPE*_T1D7;union Cyc_YYSTYPE*_T1D8;struct Cyc_Absyn_Stmt*_T1D9;struct Cyc_Yystacktype*_T1DA;struct Cyc_Yystacktype _T1DB;struct Cyc_Yyltype _T1DC;unsigned _T1DD;unsigned _T1DE;struct Cyc_Absyn_Fndecl*_T1DF;struct Cyc_Yystacktype*_T1E0;union Cyc_YYSTYPE*_T1E1;union Cyc_YYSTYPE*_T1E2;struct _RegionHandle*_T1E3;struct Cyc_Parse_Declaration_spec*_T1E4;struct Cyc_Parse_Declaration_spec*_T1E5;struct Cyc_Yystacktype*_T1E6;union Cyc_YYSTYPE*_T1E7;union Cyc_YYSTYPE*_T1E8;struct Cyc_Parse_Declarator _T1E9;struct Cyc_Yystacktype*_T1EA;union Cyc_YYSTYPE*_T1EB;union Cyc_YYSTYPE*_T1EC;struct Cyc_Absyn_Stmt*_T1ED;struct Cyc_Yystacktype*_T1EE;struct Cyc_Yystacktype _T1EF;struct Cyc_Yyltype _T1F0;unsigned _T1F1;unsigned _T1F2;struct Cyc_Absyn_Fndecl*_T1F3;struct Cyc_Yystacktype*_T1F4;union Cyc_YYSTYPE*_T1F5;union Cyc_YYSTYPE*_T1F6;struct _RegionHandle*_T1F7;struct Cyc_Parse_Declaration_spec*_T1F8;struct Cyc_Parse_Declaration_spec*_T1F9;struct Cyc_Yystacktype*_T1FA;union Cyc_YYSTYPE*_T1FB;union Cyc_YYSTYPE*_T1FC;struct Cyc_Parse_Declarator _T1FD;struct Cyc_Yystacktype*_T1FE;union Cyc_YYSTYPE*_T1FF;union Cyc_YYSTYPE*_T200;struct Cyc_List_List*_T201;struct Cyc_Yystacktype*_T202;union Cyc_YYSTYPE*_T203;union Cyc_YYSTYPE*_T204;struct Cyc_Absyn_Stmt*_T205;struct Cyc_Yystacktype*_T206;struct Cyc_Yystacktype _T207;struct Cyc_Yyltype _T208;unsigned _T209;unsigned _T20A;struct Cyc_Absyn_Fndecl*_T20B;struct Cyc_Yystacktype*_T20C;union Cyc_YYSTYPE*_T20D;union Cyc_YYSTYPE*_T20E;struct _tuple0*_T20F;struct Cyc_Yystacktype*_T210;struct Cyc_Yystacktype _T211;struct _fat_ptr*_T212;struct Cyc_Yystacktype*_T213;union Cyc_YYSTYPE*_T214;union Cyc_YYSTYPE*_T215;struct Cyc_Yystacktype*_T216;struct Cyc_Yystacktype _T217;struct Cyc_Yystacktype*_T218;union Cyc_YYSTYPE*_T219;union Cyc_YYSTYPE*_T21A;struct Cyc_Parse_Declaration_spec _T21B;struct Cyc_Yystacktype*_T21C;struct Cyc_Yystacktype _T21D;struct Cyc_Yyltype _T21E;unsigned _T21F;unsigned _T220;struct Cyc_Yystacktype*_T221;struct Cyc_Yystacktype _T222;struct Cyc_Yyltype _T223;unsigned _T224;unsigned _T225;struct Cyc_List_List*_T226;struct Cyc_Yystacktype*_T227;union Cyc_YYSTYPE*_T228;union Cyc_YYSTYPE*_T229;struct Cyc_Parse_Declaration_spec _T22A;struct _tuple11*(*_T22B)(struct _tuple11*);struct Cyc_Yystacktype*_T22C;union Cyc_YYSTYPE*_T22D;union Cyc_YYSTYPE*_T22E;struct _tuple11*_T22F;struct _tuple11*_T230;struct Cyc_Yystacktype*_T231;struct Cyc_Yystacktype _T232;struct Cyc_Yyltype _T233;unsigned _T234;unsigned _T235;struct Cyc_Yystacktype*_T236;struct Cyc_Yystacktype _T237;struct Cyc_Yyltype _T238;unsigned _T239;unsigned _T23A;struct Cyc_List_List*_T23B;struct Cyc_List_List*_T23C;struct Cyc_Yystacktype*_T23D;union Cyc_YYSTYPE*_T23E;union Cyc_YYSTYPE*_T23F;struct Cyc_Absyn_Pat*_T240;struct Cyc_Yystacktype*_T241;union Cyc_YYSTYPE*_T242;union Cyc_YYSTYPE*_T243;struct Cyc_Absyn_Exp*_T244;struct Cyc_Yystacktype*_T245;struct Cyc_Yystacktype _T246;struct Cyc_Yyltype _T247;unsigned _T248;unsigned _T249;struct Cyc_Yystacktype*_T24A;union Cyc_YYSTYPE*_T24B;union Cyc_YYSTYPE*_T24C;struct _tuple0*_T24D;struct _tuple0*_T24E;struct Cyc_List_List*_T24F;void*_T250;struct Cyc_List_List*_T251;struct _tuple0*_T252;void*_T253;struct Cyc_List_List*_T254;struct Cyc_List_List*_T255;struct Cyc_List_List*_T256;struct Cyc_Yystacktype*_T257;struct Cyc_Yystacktype _T258;struct Cyc_Yyltype _T259;unsigned _T25A;unsigned _T25B;struct Cyc_Yystacktype*_T25C;union Cyc_YYSTYPE*_T25D;union Cyc_YYSTYPE*_T25E;struct _fat_ptr _T25F;struct Cyc_Yystacktype*_T260;struct Cyc_Yystacktype _T261;struct Cyc_Yyltype _T262;unsigned _T263;unsigned _T264;struct Cyc_Absyn_Tvar*_T265;struct _fat_ptr*_T266;struct Cyc_Yystacktype*_T267;union Cyc_YYSTYPE*_T268;union Cyc_YYSTYPE*_T269;struct Cyc_Absyn_Tvar*_T26A;struct Cyc_Absyn_Tvar*_T26B;struct Cyc_Absyn_Kind*_T26C;struct Cyc_Absyn_Kind*_T26D;struct Cyc_Absyn_Tvar*_T26E;struct Cyc_Yystacktype*_T26F;struct Cyc_Yystacktype _T270;struct Cyc_Yyltype _T271;unsigned _T272;unsigned _T273;struct _tuple0*_T274;struct _fat_ptr*_T275;struct Cyc_Yystacktype*_T276;union Cyc_YYSTYPE*_T277;union Cyc_YYSTYPE*_T278;void*_T279;struct Cyc_List_List*_T27A;struct Cyc_Absyn_Tvar*_T27B;struct Cyc_Absyn_Vardecl*_T27C;struct Cyc_Yystacktype*_T27D;struct Cyc_Yystacktype _T27E;struct Cyc_Yyltype _T27F;unsigned _T280;unsigned _T281;struct _fat_ptr _T282;struct Cyc_String_pa_PrintArg_struct _T283;struct Cyc_Yystacktype*_T284;union Cyc_YYSTYPE*_T285;union Cyc_YYSTYPE*_T286;struct _fat_ptr _T287;struct _fat_ptr _T288;struct Cyc_Yystacktype*_T289;union Cyc_YYSTYPE*_T28A;union Cyc_YYSTYPE*_T28B;struct _fat_ptr _T28C;struct Cyc_Yystacktype*_T28D;struct Cyc_Yystacktype _T28E;struct Cyc_Yyltype _T28F;unsigned _T290;unsigned _T291;struct Cyc_Absyn_Tvar*_T292;struct _fat_ptr*_T293;struct Cyc_Absyn_Tvar*_T294;struct Cyc_Absyn_Tvar*_T295;struct Cyc_Absyn_Kind*_T296;struct Cyc_Absyn_Kind*_T297;struct Cyc_Absyn_Tvar*_T298;struct Cyc_Yystacktype*_T299;struct Cyc_Yystacktype _T29A;struct Cyc_Yyltype _T29B;unsigned _T29C;unsigned _T29D;struct _tuple0*_T29E;struct _fat_ptr*_T29F;struct Cyc_Yystacktype*_T2A0;union Cyc_YYSTYPE*_T2A1;union Cyc_YYSTYPE*_T2A2;void*_T2A3;struct Cyc_List_List*_T2A4;struct Cyc_Absyn_Tvar*_T2A5;struct Cyc_Absyn_Vardecl*_T2A6;struct Cyc_Yystacktype*_T2A7;union Cyc_YYSTYPE*_T2A8;union Cyc_YYSTYPE*_T2A9;struct Cyc_Absyn_Exp*_T2AA;struct Cyc_Yystacktype*_T2AB;struct Cyc_Yystacktype _T2AC;struct Cyc_Yyltype _T2AD;unsigned _T2AE;unsigned _T2AF;struct Cyc_Yystacktype*_T2B0;union Cyc_YYSTYPE*_T2B1;union Cyc_YYSTYPE*_T2B2;struct _fat_ptr _T2B3;struct _fat_ptr _T2B4;int _T2B5;struct Cyc_Yystacktype*_T2B6;struct Cyc_Yystacktype _T2B7;struct Cyc_Yyltype _T2B8;unsigned _T2B9;unsigned _T2BA;struct _fat_ptr _T2BB;struct _fat_ptr _T2BC;struct Cyc_Yystacktype*_T2BD;union Cyc_YYSTYPE*_T2BE;union Cyc_YYSTYPE*_T2BF;struct Cyc_Absyn_Exp*_T2C0;struct Cyc_Yystacktype*_T2C1;struct Cyc_Yystacktype _T2C2;struct Cyc_Yystacktype*_T2C3;union Cyc_YYSTYPE*_T2C4;union Cyc_YYSTYPE*_T2C5;struct Cyc_List_List*_T2C6;struct Cyc_Yystacktype*_T2C7;union Cyc_YYSTYPE*_T2C8;union Cyc_YYSTYPE*_T2C9;struct Cyc_List_List*_T2CA;struct Cyc_List_List*_T2CB;struct Cyc_Parse_Declaration_spec _T2CC;struct Cyc_Yystacktype*_T2CD;union Cyc_YYSTYPE*_T2CE;union Cyc_YYSTYPE*_T2CF;struct Cyc_Yystacktype*_T2D0;struct Cyc_Yystacktype _T2D1;struct Cyc_Yyltype _T2D2;unsigned _T2D3;unsigned _T2D4;struct Cyc_Yystacktype*_T2D5;union Cyc_YYSTYPE*_T2D6;union Cyc_YYSTYPE*_T2D7;struct Cyc_Parse_Declaration_spec _T2D8;enum Cyc_Parse_Storage_class _T2D9;int _T2DA;struct Cyc_Yystacktype*_T2DB;struct Cyc_Yystacktype _T2DC;struct Cyc_Yyltype _T2DD;unsigned _T2DE;unsigned _T2DF;struct _fat_ptr _T2E0;struct _fat_ptr _T2E1;struct Cyc_Parse_Declaration_spec _T2E2;struct Cyc_Yystacktype*_T2E3;union Cyc_YYSTYPE*_T2E4;union Cyc_YYSTYPE*_T2E5;struct Cyc_Parse_Declaration_spec _T2E6;struct Cyc_Parse_Declaration_spec _T2E7;struct Cyc_Parse_Declaration_spec _T2E8;struct Cyc_Parse_Declaration_spec _T2E9;struct Cyc_Yystacktype*_T2EA;struct Cyc_Yystacktype _T2EB;struct Cyc_Yyltype _T2EC;unsigned _T2ED;unsigned _T2EE;struct _fat_ptr _T2EF;struct _fat_ptr _T2F0;struct Cyc_Yystacktype*_T2F1;struct Cyc_Yystacktype _T2F2;struct Cyc_Parse_Declaration_spec _T2F3;struct Cyc_Yystacktype*_T2F4;struct Cyc_Yystacktype _T2F5;struct Cyc_Yyltype _T2F6;unsigned _T2F7;unsigned _T2F8;struct Cyc_Yystacktype*_T2F9;union Cyc_YYSTYPE*_T2FA;union Cyc_YYSTYPE*_T2FB;struct Cyc_Yystacktype*_T2FC;union Cyc_YYSTYPE*_T2FD;union Cyc_YYSTYPE*_T2FE;struct Cyc_Parse_Declaration_spec _T2FF;struct Cyc_Parse_Declaration_spec _T300;struct Cyc_Parse_Declaration_spec _T301;struct Cyc_Yystacktype*_T302;struct Cyc_Yystacktype _T303;struct Cyc_Yyltype _T304;unsigned _T305;unsigned _T306;struct Cyc_Parse_Declaration_spec _T307;struct Cyc_Parse_Type_specifier _T308;struct Cyc_Yystacktype*_T309;union Cyc_YYSTYPE*_T30A;union Cyc_YYSTYPE*_T30B;struct Cyc_Parse_Type_specifier _T30C;struct Cyc_Parse_Declaration_spec _T30D;struct Cyc_Parse_Declaration_spec _T30E;struct Cyc_Parse_Declaration_spec _T30F;struct Cyc_Yystacktype*_T310;union Cyc_YYSTYPE*_T311;union Cyc_YYSTYPE*_T312;struct Cyc_Yystacktype*_T313;union Cyc_YYSTYPE*_T314;union Cyc_YYSTYPE*_T315;struct Cyc_Parse_Declaration_spec _T316;struct Cyc_Parse_Declaration_spec _T317;struct Cyc_Yystacktype*_T318;union Cyc_YYSTYPE*_T319;union Cyc_YYSTYPE*_T31A;struct Cyc_Absyn_Tqual _T31B;struct Cyc_Parse_Declaration_spec _T31C;struct Cyc_Absyn_Tqual _T31D;struct Cyc_Parse_Declaration_spec _T31E;struct Cyc_Parse_Declaration_spec _T31F;struct Cyc_Parse_Declaration_spec _T320;struct Cyc_Parse_Declaration_spec _T321;struct Cyc_Yystacktype*_T322;struct Cyc_Yystacktype _T323;struct Cyc_Yyltype _T324;unsigned _T325;unsigned _T326;struct Cyc_Yystacktype*_T327;union Cyc_YYSTYPE*_T328;union Cyc_YYSTYPE*_T329;struct Cyc_Parse_Declaration_spec _T32A;struct Cyc_Parse_Declaration_spec _T32B;struct Cyc_Parse_Declaration_spec _T32C;struct Cyc_Parse_Declaration_spec _T32D;struct Cyc_Parse_Declaration_spec _T32E;struct Cyc_Parse_Declaration_spec _T32F;struct Cyc_Yystacktype*_T330;struct Cyc_Yystacktype _T331;struct Cyc_Yyltype _T332;unsigned _T333;unsigned _T334;struct Cyc_Yystacktype*_T335;union Cyc_YYSTYPE*_T336;union Cyc_YYSTYPE*_T337;struct Cyc_Yystacktype*_T338;union Cyc_YYSTYPE*_T339;union Cyc_YYSTYPE*_T33A;struct Cyc_Parse_Declaration_spec _T33B;struct Cyc_Parse_Declaration_spec _T33C;struct Cyc_Parse_Declaration_spec _T33D;struct Cyc_Parse_Declaration_spec _T33E;struct Cyc_Parse_Declaration_spec _T33F;struct Cyc_Yystacktype*_T340;union Cyc_YYSTYPE*_T341;union Cyc_YYSTYPE*_T342;struct Cyc_List_List*_T343;struct Cyc_Parse_Declaration_spec _T344;struct Cyc_List_List*_T345;struct Cyc_Yystacktype*_T346;union Cyc_YYSTYPE*_T347;union Cyc_YYSTYPE*_T348;struct _fat_ptr _T349;struct _fat_ptr _T34A;int _T34B;struct Cyc_Yystacktype*_T34C;struct Cyc_Yystacktype _T34D;struct Cyc_Yyltype _T34E;unsigned _T34F;unsigned _T350;struct _fat_ptr _T351;struct _fat_ptr _T352;struct Cyc_Yystacktype*_T353;struct Cyc_Yystacktype _T354;struct Cyc_Yystacktype*_T355;struct Cyc_Yystacktype _T356;struct Cyc_List_List*_T357;struct Cyc_Yystacktype*_T358;union Cyc_YYSTYPE*_T359;union Cyc_YYSTYPE*_T35A;struct Cyc_List_List*_T35B;struct Cyc_Yystacktype*_T35C;union Cyc_YYSTYPE*_T35D;union Cyc_YYSTYPE*_T35E;struct Cyc_Yystacktype*_T35F;union Cyc_YYSTYPE*_T360;union Cyc_YYSTYPE*_T361;struct Cyc_Yystacktype*_T362;struct Cyc_Yystacktype _T363;struct Cyc_Yyltype _T364;unsigned _T365;unsigned _T366;struct Cyc_Yystacktype*_T367;union Cyc_YYSTYPE*_T368;union Cyc_YYSTYPE*_T369;struct _fat_ptr _T36A;void*_T36B;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T36C;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T36D;void*_T36E;struct Cyc_Yystacktype*_T36F;struct Cyc_Yystacktype _T370;struct Cyc_Yyltype _T371;unsigned _T372;unsigned _T373;struct Cyc_Yystacktype*_T374;union Cyc_YYSTYPE*_T375;union Cyc_YYSTYPE*_T376;struct _fat_ptr _T377;struct Cyc_Yystacktype*_T378;struct Cyc_Yystacktype _T379;struct Cyc_Yyltype _T37A;unsigned _T37B;unsigned _T37C;struct Cyc_Yystacktype*_T37D;union Cyc_YYSTYPE*_T37E;union Cyc_YYSTYPE*_T37F;struct Cyc_Absyn_Exp*_T380;void*_T381;struct Cyc_Yystacktype*_T382;struct Cyc_Yystacktype _T383;struct Cyc_Yyltype _T384;unsigned _T385;unsigned _T386;struct Cyc_Yystacktype*_T387;struct Cyc_Yystacktype _T388;struct Cyc_Yyltype _T389;unsigned _T38A;unsigned _T38B;struct Cyc_Yystacktype*_T38C;union Cyc_YYSTYPE*_T38D;union Cyc_YYSTYPE*_T38E;struct _fat_ptr _T38F;struct Cyc_Yystacktype*_T390;union Cyc_YYSTYPE*_T391;union Cyc_YYSTYPE*_T392;struct _fat_ptr _T393;struct Cyc_Yystacktype*_T394;struct Cyc_Yystacktype _T395;struct Cyc_Yyltype _T396;unsigned _T397;unsigned _T398;struct Cyc_Yystacktype*_T399;union Cyc_YYSTYPE*_T39A;union Cyc_YYSTYPE*_T39B;union Cyc_Absyn_Cnst _T39C;unsigned _T39D;struct Cyc_Yystacktype*_T39E;struct Cyc_Yystacktype _T39F;struct Cyc_Yyltype _T3A0;unsigned _T3A1;unsigned _T3A2;struct Cyc_Yystacktype*_T3A3;union Cyc_YYSTYPE*_T3A4;union Cyc_YYSTYPE*_T3A5;union Cyc_Absyn_Cnst _T3A6;unsigned _T3A7;void*_T3A8;struct Cyc_Yystacktype*_T3A9;struct Cyc_Yystacktype _T3AA;struct Cyc_Yystacktype*_T3AB;union Cyc_YYSTYPE*_T3AC;union Cyc_YYSTYPE*_T3AD;struct _tuple0*_T3AE;struct Cyc_Yystacktype*_T3AF;union Cyc_YYSTYPE*_T3B0;union Cyc_YYSTYPE*_T3B1;struct Cyc_List_List*_T3B2;void*_T3B3;struct Cyc_Yystacktype*_T3B4;struct Cyc_Yystacktype _T3B5;struct Cyc_Yyltype _T3B6;unsigned _T3B7;unsigned _T3B8;struct Cyc_Parse_Type_specifier _T3B9;void*_T3BA;struct Cyc_Yystacktype*_T3BB;struct Cyc_Yystacktype _T3BC;struct Cyc_Yyltype _T3BD;unsigned _T3BE;unsigned _T3BF;struct Cyc_Parse_Type_specifier _T3C0;void*_T3C1;struct Cyc_Yystacktype*_T3C2;struct Cyc_Yystacktype _T3C3;struct Cyc_Yyltype _T3C4;unsigned _T3C5;unsigned _T3C6;struct Cyc_Parse_Type_specifier _T3C7;struct Cyc_Yystacktype*_T3C8;struct Cyc_Yystacktype _T3C9;struct Cyc_Yyltype _T3CA;unsigned _T3CB;unsigned _T3CC;struct Cyc_Parse_Type_specifier _T3CD;void*_T3CE;struct Cyc_Yystacktype*_T3CF;struct Cyc_Yystacktype _T3D0;struct Cyc_Yyltype _T3D1;unsigned _T3D2;unsigned _T3D3;struct Cyc_Parse_Type_specifier _T3D4;struct Cyc_Yystacktype*_T3D5;struct Cyc_Yystacktype _T3D6;struct Cyc_Yyltype _T3D7;unsigned _T3D8;unsigned _T3D9;struct Cyc_Parse_Type_specifier _T3DA;void*_T3DB;struct Cyc_Yystacktype*_T3DC;struct Cyc_Yystacktype _T3DD;struct Cyc_Yyltype _T3DE;unsigned _T3DF;unsigned _T3E0;struct Cyc_Parse_Type_specifier _T3E1;void*_T3E2;struct Cyc_Yystacktype*_T3E3;struct Cyc_Yystacktype _T3E4;struct Cyc_Yyltype _T3E5;unsigned _T3E6;unsigned _T3E7;struct Cyc_Parse_Type_specifier _T3E8;struct Cyc_Yystacktype*_T3E9;struct Cyc_Yystacktype _T3EA;struct Cyc_Yyltype _T3EB;unsigned _T3EC;unsigned _T3ED;struct Cyc_Parse_Type_specifier _T3EE;struct Cyc_Yystacktype*_T3EF;struct Cyc_Yystacktype _T3F0;struct Cyc_Yyltype _T3F1;unsigned _T3F2;unsigned _T3F3;struct Cyc_Parse_Type_specifier _T3F4;struct Cyc_Yystacktype*_T3F5;struct Cyc_Yystacktype _T3F6;struct Cyc_Yyltype _T3F7;unsigned _T3F8;unsigned _T3F9;struct Cyc_Parse_Type_specifier _T3FA;struct Cyc_Yystacktype*_T3FB;struct Cyc_Yystacktype _T3FC;struct Cyc_Yystacktype*_T3FD;struct Cyc_Yystacktype _T3FE;struct Cyc_Yystacktype*_T3FF;union Cyc_YYSTYPE*_T400;union Cyc_YYSTYPE*_T401;struct Cyc_Absyn_Exp*_T402;void*_T403;struct Cyc_Yystacktype*_T404;struct Cyc_Yystacktype _T405;struct Cyc_Yyltype _T406;unsigned _T407;unsigned _T408;struct Cyc_Parse_Type_specifier _T409;struct _fat_ptr _T40A;struct Cyc_Absyn_Kind*_T40B;struct Cyc_Absyn_Kind*_T40C;void*_T40D;struct Cyc_Yystacktype*_T40E;struct Cyc_Yystacktype _T40F;struct Cyc_Yyltype _T410;unsigned _T411;unsigned _T412;struct Cyc_Parse_Type_specifier _T413;struct Cyc_Yystacktype*_T414;struct Cyc_Yystacktype _T415;struct Cyc_Yystacktype*_T416;union Cyc_YYSTYPE*_T417;union Cyc_YYSTYPE*_T418;void*_T419;struct Cyc_Yystacktype*_T41A;struct Cyc_Yystacktype _T41B;struct Cyc_Yyltype _T41C;unsigned _T41D;unsigned _T41E;struct Cyc_Parse_Type_specifier _T41F;void*_T420;struct Cyc_Yystacktype*_T421;struct Cyc_Yystacktype _T422;struct Cyc_Yyltype _T423;unsigned _T424;unsigned _T425;struct Cyc_Parse_Type_specifier _T426;struct Cyc_Yystacktype*_T427;union Cyc_YYSTYPE*_T428;union Cyc_YYSTYPE*_T429;struct Cyc_Absyn_Kind*_T42A;struct Cyc_Core_Opt*_T42B;void*_T42C;struct Cyc_Yystacktype*_T42D;struct Cyc_Yystacktype _T42E;struct Cyc_Yyltype _T42F;unsigned _T430;unsigned _T431;struct Cyc_Parse_Type_specifier _T432;struct Cyc_List_List*(*_T433)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T434)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T435)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T436;struct Cyc_Yystacktype _T437;struct Cyc_Yyltype _T438;unsigned _T439;unsigned _T43A;struct Cyc_Yystacktype*_T43B;union Cyc_YYSTYPE*_T43C;union Cyc_YYSTYPE*_T43D;struct Cyc_List_List*_T43E;struct Cyc_List_List*_T43F;struct Cyc_List_List*_T440;void*_T441;struct Cyc_Yystacktype*_T442;struct Cyc_Yystacktype _T443;struct Cyc_Yyltype _T444;unsigned _T445;unsigned _T446;struct Cyc_Parse_Type_specifier _T447;struct Cyc_Yystacktype*_T448;union Cyc_YYSTYPE*_T449;union Cyc_YYSTYPE*_T44A;void*_T44B;void*_T44C;struct Cyc_Yystacktype*_T44D;struct Cyc_Yystacktype _T44E;struct Cyc_Yyltype _T44F;unsigned _T450;unsigned _T451;struct Cyc_Parse_Type_specifier _T452;struct Cyc_Core_Opt*_T453;struct Cyc_Core_Opt*_T454;void*_T455;void*_T456;struct Cyc_Yystacktype*_T457;struct Cyc_Yystacktype _T458;struct Cyc_Yyltype _T459;unsigned _T45A;unsigned _T45B;struct Cyc_Parse_Type_specifier _T45C;struct Cyc_Yystacktype*_T45D;union Cyc_YYSTYPE*_T45E;union Cyc_YYSTYPE*_T45F;void*_T460;void*_T461;struct Cyc_Yystacktype*_T462;struct Cyc_Yystacktype _T463;struct Cyc_Yyltype _T464;unsigned _T465;unsigned _T466;struct Cyc_Parse_Type_specifier _T467;struct Cyc_Yystacktype*_T468;union Cyc_YYSTYPE*_T469;union Cyc_YYSTYPE*_T46A;void*_T46B;void*_T46C;struct Cyc_Yystacktype*_T46D;struct Cyc_Yystacktype _T46E;struct Cyc_Yyltype _T46F;unsigned _T470;unsigned _T471;struct Cyc_Parse_Type_specifier _T472;struct Cyc_Core_Opt*_T473;struct Cyc_Core_Opt*_T474;void*_T475;void*_T476;struct Cyc_Yystacktype*_T477;struct Cyc_Yystacktype _T478;struct Cyc_Yyltype _T479;unsigned _T47A;unsigned _T47B;struct Cyc_Parse_Type_specifier _T47C;struct Cyc_Yystacktype*_T47D;union Cyc_YYSTYPE*_T47E;union Cyc_YYSTYPE*_T47F;struct Cyc_Absyn_Exp*_T480;void*_T481;struct Cyc_Yystacktype*_T482;struct Cyc_Yystacktype _T483;struct Cyc_Yyltype _T484;unsigned _T485;unsigned _T486;struct Cyc_Parse_Type_specifier _T487;struct Cyc_Yystacktype*_T488;union Cyc_YYSTYPE*_T489;union Cyc_YYSTYPE*_T48A;struct Cyc_Absyn_Tqual _T48B;unsigned _T48C;struct Cyc_Yystacktype*_T48D;struct Cyc_Yystacktype _T48E;struct Cyc_Yyltype _T48F;unsigned _T490;struct Cyc_Yystacktype*_T491;union Cyc_YYSTYPE*_T492;union Cyc_YYSTYPE*_T493;struct Cyc_Parse_Type_specifier _T494;struct Cyc_Yystacktype*_T495;struct Cyc_Yystacktype _T496;struct Cyc_Yyltype _T497;unsigned _T498;unsigned _T499;struct Cyc_Warn_String_Warn_Warg_struct _T49A;struct Cyc_Yystacktype*_T49B;struct Cyc_Yystacktype _T49C;struct Cyc_Yyltype _T49D;unsigned _T49E;unsigned _T49F;struct _fat_ptr _T4A0;int _T4A1;struct Cyc_Warn_String_Warn_Warg_struct _T4A2;struct Cyc_Yystacktype*_T4A3;struct Cyc_Yystacktype _T4A4;struct Cyc_Yyltype _T4A5;unsigned _T4A6;unsigned _T4A7;struct _fat_ptr _T4A8;struct Cyc_Warn_String_Warn_Warg_struct _T4A9;struct Cyc_Yystacktype*_T4AA;struct Cyc_Yystacktype _T4AB;struct Cyc_Yyltype _T4AC;unsigned _T4AD;unsigned _T4AE;struct _fat_ptr _T4AF;struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T4B0;struct Cyc_Yystacktype*_T4B1;union Cyc_YYSTYPE*_T4B2;union Cyc_YYSTYPE*_T4B3;void*_T4B4;struct Cyc_Yystacktype*_T4B5;struct Cyc_Yystacktype _T4B6;struct Cyc_Yyltype _T4B7;unsigned _T4B8;unsigned _T4B9;struct Cyc_Parse_Type_specifier _T4BA;struct Cyc_Yystacktype*_T4BB;union Cyc_YYSTYPE*_T4BC;union Cyc_YYSTYPE*_T4BD;struct _fat_ptr _T4BE;struct Cyc_Yystacktype*_T4BF;struct Cyc_Yystacktype _T4C0;struct Cyc_Yyltype _T4C1;unsigned _T4C2;unsigned _T4C3;struct Cyc_Absyn_Kind*_T4C4;unsigned _T4C5;int _T4C6;struct Cyc_Yystacktype*_T4C7;struct Cyc_Yystacktype _T4C8;struct Cyc_Yyltype _T4C9;unsigned _T4CA;struct Cyc_Absyn_Tqual _T4CB;struct Cyc_Absyn_Tqual _T4CC;struct Cyc_Absyn_Tqual _T4CD;struct Cyc_Absyn_TypeDecl*_T4CE;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T4CF;struct Cyc_Absyn_Enumdecl*_T4D0;struct Cyc_Yystacktype*_T4D1;union Cyc_YYSTYPE*_T4D2;union Cyc_YYSTYPE*_T4D3;struct Cyc_Core_Opt*_T4D4;struct Cyc_Yystacktype*_T4D5;union Cyc_YYSTYPE*_T4D6;union Cyc_YYSTYPE*_T4D7;struct Cyc_Absyn_TypeDecl*_T4D8;struct Cyc_Yystacktype*_T4D9;struct Cyc_Yystacktype _T4DA;struct Cyc_Yyltype _T4DB;unsigned _T4DC;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T4DD;void*_T4DE;struct Cyc_Yystacktype*_T4DF;struct Cyc_Yystacktype _T4E0;struct Cyc_Yyltype _T4E1;unsigned _T4E2;unsigned _T4E3;struct Cyc_Parse_Type_specifier _T4E4;struct Cyc_Yystacktype*_T4E5;union Cyc_YYSTYPE*_T4E6;union Cyc_YYSTYPE*_T4E7;struct _tuple0*_T4E8;void*_T4E9;struct Cyc_Yystacktype*_T4EA;struct Cyc_Yystacktype _T4EB;struct Cyc_Yyltype _T4EC;unsigned _T4ED;unsigned _T4EE;struct Cyc_Parse_Type_specifier _T4EF;struct Cyc_Yystacktype*_T4F0;union Cyc_YYSTYPE*_T4F1;union Cyc_YYSTYPE*_T4F2;struct Cyc_List_List*_T4F3;void*_T4F4;struct Cyc_Yystacktype*_T4F5;struct Cyc_Yystacktype _T4F6;struct Cyc_Yyltype _T4F7;unsigned _T4F8;unsigned _T4F9;struct Cyc_Parse_Type_specifier _T4FA;struct Cyc_Absyn_Enumfield*_T4FB;struct Cyc_Yystacktype*_T4FC;union Cyc_YYSTYPE*_T4FD;union Cyc_YYSTYPE*_T4FE;struct Cyc_Yystacktype*_T4FF;struct Cyc_Yystacktype _T500;struct Cyc_Yyltype _T501;unsigned _T502;struct Cyc_Absyn_Enumfield*_T503;struct Cyc_Yystacktype*_T504;union Cyc_YYSTYPE*_T505;union Cyc_YYSTYPE*_T506;struct Cyc_Yystacktype*_T507;union Cyc_YYSTYPE*_T508;union Cyc_YYSTYPE*_T509;struct Cyc_Yystacktype*_T50A;struct Cyc_Yystacktype _T50B;struct Cyc_Yyltype _T50C;unsigned _T50D;struct Cyc_List_List*_T50E;struct Cyc_Yystacktype*_T50F;union Cyc_YYSTYPE*_T510;union Cyc_YYSTYPE*_T511;struct Cyc_List_List*_T512;struct Cyc_Yystacktype*_T513;union Cyc_YYSTYPE*_T514;union Cyc_YYSTYPE*_T515;struct Cyc_List_List*_T516;struct Cyc_Yystacktype*_T517;union Cyc_YYSTYPE*_T518;union Cyc_YYSTYPE*_T519;struct Cyc_Yystacktype*_T51A;union Cyc_YYSTYPE*_T51B;union Cyc_YYSTYPE*_T51C;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T51D;struct Cyc_Yystacktype*_T51E;union Cyc_YYSTYPE*_T51F;union Cyc_YYSTYPE*_T520;struct Cyc_Yystacktype*_T521;union Cyc_YYSTYPE*_T522;union Cyc_YYSTYPE*_T523;void*_T524;struct Cyc_Yystacktype*_T525;struct Cyc_Yystacktype _T526;struct Cyc_Yyltype _T527;unsigned _T528;unsigned _T529;struct Cyc_Parse_Type_specifier _T52A;struct Cyc_List_List*(*_T52B)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T52C)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T52D;struct Cyc_Yystacktype _T52E;struct Cyc_Yyltype _T52F;unsigned _T530;unsigned _T531;struct Cyc_Yystacktype*_T532;union Cyc_YYSTYPE*_T533;union Cyc_YYSTYPE*_T534;struct Cyc_List_List*_T535;struct Cyc_List_List*(*_T536)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T537)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T538;struct Cyc_Yystacktype _T539;struct Cyc_Yyltype _T53A;unsigned _T53B;unsigned _T53C;struct Cyc_Yystacktype*_T53D;union Cyc_YYSTYPE*_T53E;union Cyc_YYSTYPE*_T53F;struct Cyc_List_List*_T540;struct Cyc_Yystacktype*_T541;union Cyc_YYSTYPE*_T542;union Cyc_YYSTYPE*_T543;struct _tuple28 _T544;struct _tuple28*_T545;unsigned _T546;struct _tuple28*_T547;struct _tuple28 _T548;struct Cyc_Yystacktype*_T549;union Cyc_YYSTYPE*_T54A;union Cyc_YYSTYPE*_T54B;struct _tuple25 _T54C;enum Cyc_Absyn_AggrKind _T54D;struct Cyc_Yystacktype*_T54E;union Cyc_YYSTYPE*_T54F;union Cyc_YYSTYPE*_T550;struct _tuple0*_T551;struct Cyc_List_List*_T552;struct Cyc_List_List*_T553;struct Cyc_List_List*_T554;struct Cyc_List_List*_T555;struct Cyc_Yystacktype*_T556;union Cyc_YYSTYPE*_T557;union Cyc_YYSTYPE*_T558;struct Cyc_List_List*_T559;struct Cyc_Yystacktype*_T55A;union Cyc_YYSTYPE*_T55B;union Cyc_YYSTYPE*_T55C;struct _tuple25 _T55D;int _T55E;struct Cyc_Absyn_AggrdeclImpl*_T55F;struct Cyc_Yystacktype*_T560;struct Cyc_Yystacktype _T561;struct Cyc_Yyltype _T562;unsigned _T563;unsigned _T564;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T565;void*_T566;struct Cyc_Yystacktype*_T567;struct Cyc_Yystacktype _T568;struct Cyc_Yyltype _T569;unsigned _T56A;unsigned _T56B;struct Cyc_Parse_Type_specifier _T56C;struct Cyc_Yystacktype*_T56D;union Cyc_YYSTYPE*_T56E;union Cyc_YYSTYPE*_T56F;struct _tuple25 _T570;enum Cyc_Absyn_AggrKind _T571;struct Cyc_Yystacktype*_T572;union Cyc_YYSTYPE*_T573;union Cyc_YYSTYPE*_T574;struct _tuple0*_T575;struct Cyc_Core_Opt*_T576;struct Cyc_Yystacktype*_T577;union Cyc_YYSTYPE*_T578;union Cyc_YYSTYPE*_T579;struct _tuple25 _T57A;int _T57B;struct Cyc_Core_Opt*_T57C;union Cyc_Absyn_AggrInfo _T57D;struct Cyc_Yystacktype*_T57E;union Cyc_YYSTYPE*_T57F;union Cyc_YYSTYPE*_T580;struct Cyc_List_List*_T581;void*_T582;struct Cyc_Yystacktype*_T583;struct Cyc_Yystacktype _T584;struct Cyc_Yyltype _T585;unsigned _T586;unsigned _T587;struct Cyc_Parse_Type_specifier _T588;struct _tuple25 _T589;struct Cyc_Yystacktype*_T58A;union Cyc_YYSTYPE*_T58B;union Cyc_YYSTYPE*_T58C;struct _tuple25 _T58D;struct Cyc_Yystacktype*_T58E;union Cyc_YYSTYPE*_T58F;union Cyc_YYSTYPE*_T590;struct Cyc_Yystacktype*_T591;union Cyc_YYSTYPE*_T592;union Cyc_YYSTYPE*_T593;struct Cyc_List_List*_T594;struct Cyc_List_List*_T595;struct Cyc_Yystacktype*_T596;union Cyc_YYSTYPE*_T597;union Cyc_YYSTYPE*_T598;struct Cyc_List_List*_T599;void*_T59A;struct Cyc_List_List*_T59B;struct Cyc_List_List*_T59C;struct Cyc_List_List*_T59D;void(*_T59E)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*);void(*_T59F)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T5A0;struct Cyc_List_List*_T5A1;struct Cyc_List_List*_T5A2;struct Cyc_Yystacktype*_T5A3;union Cyc_YYSTYPE*_T5A4;union Cyc_YYSTYPE*_T5A5;struct Cyc_List_List*_T5A6;struct Cyc_Yystacktype*_T5A7;union Cyc_YYSTYPE*_T5A8;union Cyc_YYSTYPE*_T5A9;struct Cyc_Yystacktype*_T5AA;union Cyc_YYSTYPE*_T5AB;union Cyc_YYSTYPE*_T5AC;struct _tuple11*_T5AD;struct _RegionHandle*_T5AE;struct Cyc_Yystacktype*_T5AF;union Cyc_YYSTYPE*_T5B0;union Cyc_YYSTYPE*_T5B1;struct _tuple11*_T5B2;struct _RegionHandle*_T5B3;struct Cyc_Yystacktype*_T5B4;union Cyc_YYSTYPE*_T5B5;union Cyc_YYSTYPE*_T5B6;struct Cyc_Yystacktype*_T5B7;union Cyc_YYSTYPE*_T5B8;union Cyc_YYSTYPE*_T5B9;struct _tuple12 _T5BA;struct Cyc_Yystacktype*_T5BB;union Cyc_YYSTYPE*_T5BC;union Cyc_YYSTYPE*_T5BD;struct _tuple12 _T5BE;struct Cyc_Yystacktype*_T5BF;union Cyc_YYSTYPE*_T5C0;union Cyc_YYSTYPE*_T5C1;struct Cyc_Yystacktype*_T5C2;union Cyc_YYSTYPE*_T5C3;union Cyc_YYSTYPE*_T5C4;void*_T5C5;struct Cyc_Yystacktype*_T5C6;struct Cyc_Yystacktype _T5C7;struct Cyc_Yyltype _T5C8;unsigned _T5C9;unsigned _T5CA;struct _tuple12 _T5CB;struct Cyc_Yystacktype*_T5CC;union Cyc_YYSTYPE*_T5CD;union Cyc_YYSTYPE*_T5CE;struct Cyc_Yystacktype*_T5CF;union Cyc_YYSTYPE*_T5D0;union Cyc_YYSTYPE*_T5D1;struct _tuple12 _T5D2;struct Cyc_Yystacktype*_T5D3;union Cyc_YYSTYPE*_T5D4;union Cyc_YYSTYPE*_T5D5;struct Cyc_Yystacktype*_T5D6;union Cyc_YYSTYPE*_T5D7;union Cyc_YYSTYPE*_T5D8;struct Cyc_Yystacktype*_T5D9;union Cyc_YYSTYPE*_T5DA;union Cyc_YYSTYPE*_T5DB;void*_T5DC;struct Cyc_Yystacktype*_T5DD;struct Cyc_Yystacktype _T5DE;struct Cyc_Yyltype _T5DF;unsigned _T5E0;unsigned _T5E1;struct Cyc_Yystacktype*_T5E2;union Cyc_YYSTYPE*_T5E3;union Cyc_YYSTYPE*_T5E4;struct Cyc_Absyn_Tqual _T5E5;unsigned _T5E6;struct Cyc_Yystacktype*_T5E7;struct Cyc_Yystacktype _T5E8;struct Cyc_Yyltype _T5E9;unsigned _T5EA;struct Cyc_Yystacktype*_T5EB;union Cyc_YYSTYPE*_T5EC;union Cyc_YYSTYPE*_T5ED;struct Cyc_List_List*_T5EE;void*_T5EF;struct _tuple13*_T5F0;struct _RegionHandle*_T5F1;struct Cyc_List_List*_T5F2;struct _RegionHandle*_T5F3;struct _tuple16*_T5F4;struct _RegionHandle*_T5F5;struct Cyc_List_List*_T5F6;struct Cyc_Parse_Type_specifier _T5F7;struct Cyc_Yystacktype*_T5F8;struct Cyc_Yystacktype _T5F9;struct Cyc_Yyltype _T5FA;unsigned _T5FB;unsigned _T5FC;struct _RegionHandle*_T5FD;struct _RegionHandle*_T5FE;struct Cyc_List_List*_T5FF;struct Cyc_List_List*_T600;struct Cyc_List_List*(*_T601)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T602)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T603;struct Cyc_Yystacktype _T604;struct Cyc_Yyltype _T605;unsigned _T606;unsigned _T607;struct Cyc_List_List*_T608;struct Cyc_List_List*_T609;struct _tuple26 _T60A;struct Cyc_Yystacktype*_T60B;struct Cyc_Yystacktype _T60C;struct Cyc_Yyltype _T60D;unsigned _T60E;unsigned _T60F;struct Cyc_Yystacktype*_T610;union Cyc_YYSTYPE*_T611;union Cyc_YYSTYPE*_T612;struct Cyc_Yystacktype*_T613;union Cyc_YYSTYPE*_T614;union Cyc_YYSTYPE*_T615;struct _tuple26 _T616;struct _tuple26 _T617;struct Cyc_Yystacktype*_T618;struct Cyc_Yystacktype _T619;struct Cyc_Yyltype _T61A;unsigned _T61B;unsigned _T61C;struct Cyc_Yystacktype*_T61D;union Cyc_YYSTYPE*_T61E;union Cyc_YYSTYPE*_T61F;struct Cyc_Parse_Type_specifier _T620;struct _tuple26 _T621;struct Cyc_Parse_Type_specifier _T622;struct _tuple26 _T623;struct _tuple26 _T624;struct Cyc_Yystacktype*_T625;union Cyc_YYSTYPE*_T626;union Cyc_YYSTYPE*_T627;struct Cyc_Yystacktype*_T628;union Cyc_YYSTYPE*_T629;union Cyc_YYSTYPE*_T62A;struct _tuple26 _T62B;struct Cyc_Yystacktype*_T62C;union Cyc_YYSTYPE*_T62D;union Cyc_YYSTYPE*_T62E;struct Cyc_Absyn_Tqual _T62F;struct _tuple26 _T630;struct Cyc_Absyn_Tqual _T631;struct _tuple26 _T632;struct _tuple26 _T633;struct _tuple26 _T634;struct Cyc_Yystacktype*_T635;struct Cyc_Yystacktype _T636;struct Cyc_Yyltype _T637;unsigned _T638;unsigned _T639;struct Cyc_Yystacktype*_T63A;union Cyc_YYSTYPE*_T63B;union Cyc_YYSTYPE*_T63C;struct Cyc_Yystacktype*_T63D;union Cyc_YYSTYPE*_T63E;union Cyc_YYSTYPE*_T63F;struct _tuple26 _T640;struct _tuple26 _T641;struct _tuple26 _T642;struct Cyc_Yystacktype*_T643;union Cyc_YYSTYPE*_T644;union Cyc_YYSTYPE*_T645;struct Cyc_List_List*_T646;struct _tuple26 _T647;struct Cyc_List_List*_T648;struct _tuple26 _T649;struct Cyc_Yystacktype*_T64A;struct Cyc_Yystacktype _T64B;struct Cyc_Yyltype _T64C;unsigned _T64D;unsigned _T64E;struct Cyc_Yystacktype*_T64F;union Cyc_YYSTYPE*_T650;union Cyc_YYSTYPE*_T651;struct Cyc_Yystacktype*_T652;union Cyc_YYSTYPE*_T653;union Cyc_YYSTYPE*_T654;struct _tuple26 _T655;struct _tuple26 _T656;struct Cyc_Yystacktype*_T657;struct Cyc_Yystacktype _T658;struct Cyc_Yyltype _T659;unsigned _T65A;unsigned _T65B;struct Cyc_Yystacktype*_T65C;union Cyc_YYSTYPE*_T65D;union Cyc_YYSTYPE*_T65E;struct Cyc_Parse_Type_specifier _T65F;struct _tuple26 _T660;struct Cyc_Parse_Type_specifier _T661;struct _tuple26 _T662;struct _tuple26 _T663;struct Cyc_Yystacktype*_T664;union Cyc_YYSTYPE*_T665;union Cyc_YYSTYPE*_T666;struct Cyc_Yystacktype*_T667;union Cyc_YYSTYPE*_T668;union Cyc_YYSTYPE*_T669;struct _tuple26 _T66A;struct Cyc_Yystacktype*_T66B;union Cyc_YYSTYPE*_T66C;union Cyc_YYSTYPE*_T66D;struct Cyc_Absyn_Tqual _T66E;struct _tuple26 _T66F;struct Cyc_Absyn_Tqual _T670;struct _tuple26 _T671;struct _tuple26 _T672;struct _tuple26 _T673;struct Cyc_Yystacktype*_T674;struct Cyc_Yystacktype _T675;struct Cyc_Yyltype _T676;unsigned _T677;unsigned _T678;struct Cyc_Yystacktype*_T679;union Cyc_YYSTYPE*_T67A;union Cyc_YYSTYPE*_T67B;struct Cyc_Yystacktype*_T67C;union Cyc_YYSTYPE*_T67D;union Cyc_YYSTYPE*_T67E;struct _tuple26 _T67F;struct _tuple26 _T680;struct _tuple26 _T681;struct Cyc_Yystacktype*_T682;union Cyc_YYSTYPE*_T683;union Cyc_YYSTYPE*_T684;struct Cyc_List_List*_T685;struct _tuple26 _T686;struct Cyc_List_List*_T687;struct Cyc_List_List*_T688;struct _RegionHandle*_T689;struct Cyc_Yystacktype*_T68A;union Cyc_YYSTYPE*_T68B;union Cyc_YYSTYPE*_T68C;struct Cyc_List_List*_T68D;struct _RegionHandle*_T68E;struct Cyc_Yystacktype*_T68F;union Cyc_YYSTYPE*_T690;union Cyc_YYSTYPE*_T691;struct Cyc_Yystacktype*_T692;union Cyc_YYSTYPE*_T693;union Cyc_YYSTYPE*_T694;struct _tuple12*_T695;struct _RegionHandle*_T696;struct Cyc_Yystacktype*_T697;union Cyc_YYSTYPE*_T698;union Cyc_YYSTYPE*_T699;struct Cyc_Yystacktype*_T69A;union Cyc_YYSTYPE*_T69B;union Cyc_YYSTYPE*_T69C;struct _tuple12*_T69D;struct _RegionHandle*_T69E;struct _tuple0*_T69F;struct _fat_ptr*_T6A0;struct Cyc_Yystacktype*_T6A1;union Cyc_YYSTYPE*_T6A2;union Cyc_YYSTYPE*_T6A3;struct _tuple12*_T6A4;struct _RegionHandle*_T6A5;struct _tuple0*_T6A6;struct _fat_ptr*_T6A7;struct _tuple12*_T6A8;struct _RegionHandle*_T6A9;struct Cyc_Yystacktype*_T6AA;union Cyc_YYSTYPE*_T6AB;union Cyc_YYSTYPE*_T6AC;struct Cyc_Yystacktype*_T6AD;union Cyc_YYSTYPE*_T6AE;union Cyc_YYSTYPE*_T6AF;struct Cyc_Yystacktype*_T6B0;union Cyc_YYSTYPE*_T6B1;union Cyc_YYSTYPE*_T6B2;struct Cyc_Absyn_Exp*_T6B3;struct Cyc_Yystacktype*_T6B4;union Cyc_YYSTYPE*_T6B5;union Cyc_YYSTYPE*_T6B6;struct Cyc_List_List*(*_T6B7)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T6B8)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T6B9;struct Cyc_Yystacktype _T6BA;struct Cyc_Yyltype _T6BB;unsigned _T6BC;unsigned _T6BD;struct Cyc_Yystacktype*_T6BE;union Cyc_YYSTYPE*_T6BF;union Cyc_YYSTYPE*_T6C0;struct Cyc_List_List*_T6C1;struct Cyc_Yystacktype*_T6C2;union Cyc_YYSTYPE*_T6C3;union Cyc_YYSTYPE*_T6C4;struct _tuple0*_T6C5;struct Cyc_List_List*_T6C6;struct Cyc_Core_Opt*_T6C7;struct Cyc_Yystacktype*_T6C8;union Cyc_YYSTYPE*_T6C9;union Cyc_YYSTYPE*_T6CA;int _T6CB;struct Cyc_Yystacktype*_T6CC;struct Cyc_Yystacktype _T6CD;struct Cyc_Yyltype _T6CE;unsigned _T6CF;unsigned _T6D0;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T6D1;void*_T6D2;struct Cyc_Yystacktype*_T6D3;struct Cyc_Yystacktype _T6D4;struct Cyc_Yyltype _T6D5;unsigned _T6D6;unsigned _T6D7;struct Cyc_Parse_Type_specifier _T6D8;struct Cyc_Yystacktype*_T6D9;union Cyc_YYSTYPE*_T6DA;union Cyc_YYSTYPE*_T6DB;struct Cyc_Absyn_UnknownDatatypeInfo _T6DC;struct Cyc_Yystacktype*_T6DD;union Cyc_YYSTYPE*_T6DE;union Cyc_YYSTYPE*_T6DF;union Cyc_Absyn_DatatypeInfo _T6E0;struct Cyc_Yystacktype*_T6E1;union Cyc_YYSTYPE*_T6E2;union Cyc_YYSTYPE*_T6E3;struct Cyc_List_List*_T6E4;void*_T6E5;struct Cyc_Yystacktype*_T6E6;struct Cyc_Yystacktype _T6E7;struct Cyc_Yyltype _T6E8;unsigned _T6E9;unsigned _T6EA;struct Cyc_Parse_Type_specifier _T6EB;struct Cyc_Yystacktype*_T6EC;union Cyc_YYSTYPE*_T6ED;union Cyc_YYSTYPE*_T6EE;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T6EF;struct Cyc_Yystacktype*_T6F0;union Cyc_YYSTYPE*_T6F1;union Cyc_YYSTYPE*_T6F2;struct Cyc_Yystacktype*_T6F3;union Cyc_YYSTYPE*_T6F4;union Cyc_YYSTYPE*_T6F5;union Cyc_Absyn_DatatypeFieldInfo _T6F6;struct Cyc_Yystacktype*_T6F7;union Cyc_YYSTYPE*_T6F8;union Cyc_YYSTYPE*_T6F9;struct Cyc_List_List*_T6FA;void*_T6FB;struct Cyc_Yystacktype*_T6FC;struct Cyc_Yystacktype _T6FD;struct Cyc_Yyltype _T6FE;unsigned _T6FF;unsigned _T700;struct Cyc_Parse_Type_specifier _T701;struct Cyc_List_List*_T702;struct Cyc_Yystacktype*_T703;union Cyc_YYSTYPE*_T704;union Cyc_YYSTYPE*_T705;struct Cyc_List_List*_T706;struct Cyc_Yystacktype*_T707;union Cyc_YYSTYPE*_T708;union Cyc_YYSTYPE*_T709;struct Cyc_List_List*_T70A;struct Cyc_Yystacktype*_T70B;union Cyc_YYSTYPE*_T70C;union Cyc_YYSTYPE*_T70D;struct Cyc_Yystacktype*_T70E;union Cyc_YYSTYPE*_T70F;union Cyc_YYSTYPE*_T710;struct Cyc_List_List*_T711;struct Cyc_Yystacktype*_T712;union Cyc_YYSTYPE*_T713;union Cyc_YYSTYPE*_T714;struct Cyc_Yystacktype*_T715;union Cyc_YYSTYPE*_T716;union Cyc_YYSTYPE*_T717;struct Cyc_Absyn_Datatypefield*_T718;struct Cyc_Yystacktype*_T719;union Cyc_YYSTYPE*_T71A;union Cyc_YYSTYPE*_T71B;struct Cyc_Yystacktype*_T71C;struct Cyc_Yystacktype _T71D;struct Cyc_Yyltype _T71E;unsigned _T71F;struct Cyc_Yystacktype*_T720;union Cyc_YYSTYPE*_T721;union Cyc_YYSTYPE*_T722;struct Cyc_List_List*(*_T723)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T724)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T725)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T726;struct Cyc_Yystacktype _T727;struct Cyc_Yyltype _T728;unsigned _T729;unsigned _T72A;struct Cyc_Yystacktype*_T72B;union Cyc_YYSTYPE*_T72C;union Cyc_YYSTYPE*_T72D;struct Cyc_List_List*_T72E;struct Cyc_List_List*_T72F;struct Cyc_Absyn_Datatypefield*_T730;struct Cyc_Yystacktype*_T731;union Cyc_YYSTYPE*_T732;union Cyc_YYSTYPE*_T733;struct Cyc_Yystacktype*_T734;struct Cyc_Yystacktype _T735;struct Cyc_Yyltype _T736;unsigned _T737;struct Cyc_Yystacktype*_T738;union Cyc_YYSTYPE*_T739;union Cyc_YYSTYPE*_T73A;struct Cyc_Yystacktype*_T73B;struct Cyc_Yystacktype _T73C;struct Cyc_Yystacktype*_T73D;union Cyc_YYSTYPE*_T73E;union Cyc_YYSTYPE*_T73F;struct Cyc_Parse_Declarator _T740;struct Cyc_Parse_Declarator _T741;struct Cyc_Parse_Declarator _T742;struct Cyc_Yystacktype*_T743;union Cyc_YYSTYPE*_T744;union Cyc_YYSTYPE*_T745;struct Cyc_List_List*_T746;struct Cyc_Parse_Declarator _T747;struct Cyc_List_List*_T748;struct Cyc_Yystacktype*_T749;struct Cyc_Yystacktype _T74A;struct Cyc_Yystacktype*_T74B;union Cyc_YYSTYPE*_T74C;union Cyc_YYSTYPE*_T74D;struct Cyc_Parse_Declarator _T74E;struct Cyc_Parse_Declarator _T74F;struct Cyc_Parse_Declarator _T750;struct Cyc_Yystacktype*_T751;union Cyc_YYSTYPE*_T752;union Cyc_YYSTYPE*_T753;struct Cyc_List_List*_T754;struct Cyc_Parse_Declarator _T755;struct Cyc_List_List*_T756;struct Cyc_Parse_Declarator _T757;struct Cyc_Yystacktype*_T758;union Cyc_YYSTYPE*_T759;union Cyc_YYSTYPE*_T75A;struct Cyc_Yystacktype*_T75B;struct Cyc_Yystacktype _T75C;struct Cyc_Yyltype _T75D;unsigned _T75E;struct Cyc_Yystacktype*_T75F;struct Cyc_Yystacktype _T760;struct Cyc_Yystacktype*_T761;union Cyc_YYSTYPE*_T762;union Cyc_YYSTYPE*_T763;struct Cyc_List_List*_T764;struct _RegionHandle*_T765;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T766;struct _RegionHandle*_T767;struct Cyc_Yystacktype*_T768;struct Cyc_Yystacktype _T769;struct Cyc_Yyltype _T76A;unsigned _T76B;struct Cyc_Yystacktype*_T76C;union Cyc_YYSTYPE*_T76D;union Cyc_YYSTYPE*_T76E;struct Cyc_Parse_Declarator _T76F;struct Cyc_Yystacktype*_T770;struct Cyc_Yystacktype _T771;struct Cyc_Parse_Declarator _T772;struct Cyc_Yystacktype*_T773;union Cyc_YYSTYPE*_T774;union Cyc_YYSTYPE*_T775;struct Cyc_Parse_Declarator _T776;struct Cyc_Yystacktype*_T777;union Cyc_YYSTYPE*_T778;union Cyc_YYSTYPE*_T779;struct Cyc_Parse_Declarator _T77A;struct Cyc_List_List*_T77B;struct _RegionHandle*_T77C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T77D;struct _RegionHandle*_T77E;struct Cyc_Yystacktype*_T77F;union Cyc_YYSTYPE*_T780;union Cyc_YYSTYPE*_T781;struct Cyc_Yystacktype*_T782;struct Cyc_Yystacktype _T783;struct Cyc_Yyltype _T784;unsigned _T785;struct Cyc_Yystacktype*_T786;union Cyc_YYSTYPE*_T787;union Cyc_YYSTYPE*_T788;struct Cyc_Parse_Declarator _T789;struct Cyc_Parse_Declarator _T78A;struct Cyc_Yystacktype*_T78B;union Cyc_YYSTYPE*_T78C;union Cyc_YYSTYPE*_T78D;struct Cyc_Parse_Declarator _T78E;struct Cyc_Yystacktype*_T78F;union Cyc_YYSTYPE*_T790;union Cyc_YYSTYPE*_T791;struct Cyc_Parse_Declarator _T792;struct Cyc_List_List*_T793;struct _RegionHandle*_T794;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T795;struct _RegionHandle*_T796;struct Cyc_Yystacktype*_T797;union Cyc_YYSTYPE*_T798;union Cyc_YYSTYPE*_T799;struct Cyc_Yystacktype*_T79A;union Cyc_YYSTYPE*_T79B;union Cyc_YYSTYPE*_T79C;struct Cyc_Yystacktype*_T79D;struct Cyc_Yystacktype _T79E;struct Cyc_Yyltype _T79F;unsigned _T7A0;struct Cyc_Yystacktype*_T7A1;union Cyc_YYSTYPE*_T7A2;union Cyc_YYSTYPE*_T7A3;struct Cyc_Parse_Declarator _T7A4;struct Cyc_Yystacktype*_T7A5;union Cyc_YYSTYPE*_T7A6;union Cyc_YYSTYPE*_T7A7;struct Cyc_Yystacktype*_T7A8;union Cyc_YYSTYPE*_T7A9;union Cyc_YYSTYPE*_T7AA;struct Cyc_Parse_Declarator _T7AB;struct Cyc_Yystacktype*_T7AC;union Cyc_YYSTYPE*_T7AD;union Cyc_YYSTYPE*_T7AE;struct Cyc_Parse_Declarator _T7AF;struct Cyc_Yystacktype*_T7B0;union Cyc_YYSTYPE*_T7B1;union Cyc_YYSTYPE*_T7B2;struct Cyc_Parse_Declarator _T7B3;struct Cyc_List_List*_T7B4;struct _RegionHandle*_T7B5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T7B6;struct _RegionHandle*_T7B7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T7B8;struct _RegionHandle*_T7B9;struct Cyc_Yystacktype*_T7BA;union Cyc_YYSTYPE*_T7BB;union Cyc_YYSTYPE*_T7BC;struct Cyc_Parse_Declarator _T7BD;struct Cyc_Parse_Declarator _T7BE;struct Cyc_Yystacktype*_T7BF;union Cyc_YYSTYPE*_T7C0;union Cyc_YYSTYPE*_T7C1;struct Cyc_Parse_Declarator _T7C2;struct Cyc_Yystacktype*_T7C3;union Cyc_YYSTYPE*_T7C4;union Cyc_YYSTYPE*_T7C5;struct Cyc_Parse_Declarator _T7C6;struct Cyc_List_List*_T7C7;struct _RegionHandle*_T7C8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T7C9;struct _RegionHandle*_T7CA;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T7CB;struct _RegionHandle*_T7CC;struct Cyc_Yystacktype*_T7CD;union Cyc_YYSTYPE*_T7CE;union Cyc_YYSTYPE*_T7CF;struct Cyc_Yystacktype*_T7D0;struct Cyc_Yystacktype _T7D1;struct Cyc_Yyltype _T7D2;unsigned _T7D3;struct Cyc_Yystacktype*_T7D4;union Cyc_YYSTYPE*_T7D5;union Cyc_YYSTYPE*_T7D6;struct Cyc_Parse_Declarator _T7D7;struct Cyc_List_List*(*_T7D8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T7D9)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T7DA;struct Cyc_Yystacktype _T7DB;struct Cyc_Yyltype _T7DC;unsigned _T7DD;unsigned _T7DE;struct Cyc_Yystacktype*_T7DF;union Cyc_YYSTYPE*_T7E0;union Cyc_YYSTYPE*_T7E1;struct Cyc_List_List*_T7E2;struct Cyc_List_List*_T7E3;struct Cyc_Parse_Declarator _T7E4;struct Cyc_Yystacktype*_T7E5;union Cyc_YYSTYPE*_T7E6;union Cyc_YYSTYPE*_T7E7;struct Cyc_Parse_Declarator _T7E8;struct Cyc_Yystacktype*_T7E9;union Cyc_YYSTYPE*_T7EA;union Cyc_YYSTYPE*_T7EB;struct Cyc_Parse_Declarator _T7EC;struct Cyc_List_List*_T7ED;struct _RegionHandle*_T7EE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T7EF;struct _RegionHandle*_T7F0;struct Cyc_Yystacktype*_T7F1;struct Cyc_Yystacktype _T7F2;struct Cyc_Yyltype _T7F3;unsigned _T7F4;struct Cyc_Yystacktype*_T7F5;union Cyc_YYSTYPE*_T7F6;union Cyc_YYSTYPE*_T7F7;struct Cyc_Parse_Declarator _T7F8;struct Cyc_Parse_Declarator _T7F9;struct Cyc_Yystacktype*_T7FA;union Cyc_YYSTYPE*_T7FB;union Cyc_YYSTYPE*_T7FC;struct Cyc_Parse_Declarator _T7FD;struct Cyc_Yystacktype*_T7FE;union Cyc_YYSTYPE*_T7FF;union Cyc_YYSTYPE*_T800;struct Cyc_Parse_Declarator _T801;struct Cyc_List_List*_T802;struct _RegionHandle*_T803;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T804;struct _RegionHandle*_T805;struct Cyc_Yystacktype*_T806;struct Cyc_Yystacktype _T807;struct Cyc_Yyltype _T808;unsigned _T809;struct Cyc_Yystacktype*_T80A;union Cyc_YYSTYPE*_T80B;union Cyc_YYSTYPE*_T80C;struct Cyc_Yystacktype*_T80D;union Cyc_YYSTYPE*_T80E;union Cyc_YYSTYPE*_T80F;struct Cyc_Parse_Declarator _T810;struct Cyc_Parse_Declarator _T811;struct Cyc_Yystacktype*_T812;union Cyc_YYSTYPE*_T813;union Cyc_YYSTYPE*_T814;struct Cyc_Yystacktype*_T815;struct Cyc_Yystacktype _T816;struct Cyc_Yyltype _T817;unsigned _T818;struct Cyc_Parse_Declarator _T819;struct Cyc_Yystacktype*_T81A;union Cyc_YYSTYPE*_T81B;union Cyc_YYSTYPE*_T81C;struct Cyc_Yystacktype*_T81D;struct Cyc_Yystacktype _T81E;struct Cyc_Yyltype _T81F;unsigned _T820;struct Cyc_Yystacktype*_T821;struct Cyc_Yystacktype _T822;struct Cyc_Yystacktype*_T823;union Cyc_YYSTYPE*_T824;union Cyc_YYSTYPE*_T825;struct Cyc_List_List*_T826;struct _RegionHandle*_T827;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T828;struct _RegionHandle*_T829;struct Cyc_Yystacktype*_T82A;struct Cyc_Yystacktype _T82B;struct Cyc_Yyltype _T82C;unsigned _T82D;struct Cyc_Yystacktype*_T82E;union Cyc_YYSTYPE*_T82F;union Cyc_YYSTYPE*_T830;struct Cyc_Parse_Declarator _T831;struct Cyc_Yystacktype*_T832;struct Cyc_Yystacktype _T833;struct Cyc_Yystacktype*_T834;union Cyc_YYSTYPE*_T835;union Cyc_YYSTYPE*_T836;struct Cyc_Parse_Declarator _T837;struct Cyc_Parse_Declarator _T838;struct Cyc_Parse_Declarator _T839;struct Cyc_List_List*_T83A;struct _RegionHandle*_T83B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T83C;struct _RegionHandle*_T83D;struct Cyc_Yystacktype*_T83E;union Cyc_YYSTYPE*_T83F;union Cyc_YYSTYPE*_T840;struct Cyc_Yystacktype*_T841;struct Cyc_Yystacktype _T842;struct Cyc_Yyltype _T843;unsigned _T844;struct Cyc_Parse_Declarator _T845;struct Cyc_Yystacktype*_T846;union Cyc_YYSTYPE*_T847;union Cyc_YYSTYPE*_T848;struct Cyc_Parse_Declarator _T849;struct Cyc_Parse_Declarator _T84A;struct Cyc_Parse_Declarator _T84B;struct Cyc_List_List*_T84C;struct _RegionHandle*_T84D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T84E;struct _RegionHandle*_T84F;struct Cyc_Yystacktype*_T850;union Cyc_YYSTYPE*_T851;union Cyc_YYSTYPE*_T852;struct Cyc_Yystacktype*_T853;union Cyc_YYSTYPE*_T854;union Cyc_YYSTYPE*_T855;struct Cyc_Yystacktype*_T856;struct Cyc_Yystacktype _T857;struct Cyc_Yyltype _T858;unsigned _T859;struct Cyc_Parse_Declarator _T85A;struct Cyc_Yystacktype*_T85B;union Cyc_YYSTYPE*_T85C;union Cyc_YYSTYPE*_T85D;struct Cyc_Yystacktype*_T85E;union Cyc_YYSTYPE*_T85F;union Cyc_YYSTYPE*_T860;struct Cyc_Yystacktype*_T861;union Cyc_YYSTYPE*_T862;union Cyc_YYSTYPE*_T863;struct Cyc_Parse_Declarator _T864;struct Cyc_Parse_Declarator _T865;struct Cyc_Parse_Declarator _T866;struct Cyc_List_List*_T867;struct _RegionHandle*_T868;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T869;struct _RegionHandle*_T86A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T86B;struct _RegionHandle*_T86C;struct Cyc_Parse_Declarator _T86D;struct Cyc_Yystacktype*_T86E;union Cyc_YYSTYPE*_T86F;union Cyc_YYSTYPE*_T870;struct Cyc_Parse_Declarator _T871;struct Cyc_Parse_Declarator _T872;struct Cyc_Parse_Declarator _T873;struct Cyc_List_List*_T874;struct _RegionHandle*_T875;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T876;struct _RegionHandle*_T877;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T878;struct _RegionHandle*_T879;struct Cyc_Yystacktype*_T87A;union Cyc_YYSTYPE*_T87B;union Cyc_YYSTYPE*_T87C;struct Cyc_Yystacktype*_T87D;struct Cyc_Yystacktype _T87E;struct Cyc_Yyltype _T87F;unsigned _T880;struct Cyc_Parse_Declarator _T881;struct Cyc_List_List*(*_T882)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T883)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T884;struct Cyc_Yystacktype _T885;struct Cyc_Yyltype _T886;unsigned _T887;unsigned _T888;struct Cyc_Yystacktype*_T889;union Cyc_YYSTYPE*_T88A;union Cyc_YYSTYPE*_T88B;struct Cyc_List_List*_T88C;struct Cyc_List_List*_T88D;struct Cyc_Yystacktype*_T88E;union Cyc_YYSTYPE*_T88F;union Cyc_YYSTYPE*_T890;struct Cyc_Parse_Declarator _T891;struct Cyc_Parse_Declarator _T892;struct Cyc_Parse_Declarator _T893;struct Cyc_List_List*_T894;struct _RegionHandle*_T895;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T896;struct _RegionHandle*_T897;struct Cyc_Yystacktype*_T898;struct Cyc_Yystacktype _T899;struct Cyc_Yyltype _T89A;unsigned _T89B;struct Cyc_Parse_Declarator _T89C;struct Cyc_Yystacktype*_T89D;union Cyc_YYSTYPE*_T89E;union Cyc_YYSTYPE*_T89F;struct Cyc_Parse_Declarator _T8A0;struct Cyc_Parse_Declarator _T8A1;struct Cyc_Parse_Declarator _T8A2;struct Cyc_List_List*_T8A3;struct _RegionHandle*_T8A4;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T8A5;struct _RegionHandle*_T8A6;struct Cyc_Yystacktype*_T8A7;struct Cyc_Yystacktype _T8A8;struct Cyc_Yyltype _T8A9;unsigned _T8AA;struct Cyc_Yystacktype*_T8AB;union Cyc_YYSTYPE*_T8AC;union Cyc_YYSTYPE*_T8AD;struct Cyc_Parse_Declarator _T8AE;struct Cyc_Yystacktype*_T8AF;struct Cyc_Yystacktype _T8B0;struct Cyc_Yystacktype*_T8B1;union Cyc_YYSTYPE*_T8B2;union Cyc_YYSTYPE*_T8B3;struct Cyc_List_List*_T8B4;struct Cyc_Yystacktype*_T8B5;union Cyc_YYSTYPE*_T8B6;union Cyc_YYSTYPE*_T8B7;struct Cyc_List_List*_T8B8;struct Cyc_List_List*_T8B9;struct Cyc_Yystacktype*_T8BA;union Cyc_YYSTYPE*_T8BB;union Cyc_YYSTYPE*_T8BC;struct Cyc_List_List*_T8BD;struct Cyc_List_List*_T8BE;struct _RegionHandle*_T8BF;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T8C0;struct _RegionHandle*_T8C1;struct Cyc_Yystacktype*_T8C2;struct Cyc_Yystacktype _T8C3;struct Cyc_Yyltype _T8C4;unsigned _T8C5;struct Cyc_Yystacktype*_T8C6;union Cyc_YYSTYPE*_T8C7;union Cyc_YYSTYPE*_T8C8;struct Cyc_Yystacktype*_T8C9;union Cyc_YYSTYPE*_T8CA;union Cyc_YYSTYPE*_T8CB;struct _tuple22*_T8CC;int _T8CD;struct Cyc_Absyn_PtrLoc*_T8CE;struct Cyc_Yystacktype*_T8CF;struct Cyc_Yystacktype _T8D0;struct Cyc_Yyltype _T8D1;unsigned _T8D2;struct Cyc_Yystacktype*_T8D3;struct Cyc_Yystacktype _T8D4;struct Cyc_Yyltype _T8D5;unsigned _T8D6;struct _RegionHandle*_T8D7;struct Cyc_Absyn_PtrLoc*_T8D8;void*_T8D9;void*_T8DA;struct Cyc_Yystacktype*_T8DB;union Cyc_YYSTYPE*_T8DC;union Cyc_YYSTYPE*_T8DD;void*_T8DE;struct Cyc_Yystacktype*_T8DF;union Cyc_YYSTYPE*_T8E0;union Cyc_YYSTYPE*_T8E1;struct Cyc_List_List*_T8E2;struct Cyc_Yystacktype*_T8E3;union Cyc_YYSTYPE*_T8E4;union Cyc_YYSTYPE*_T8E5;struct Cyc_Absyn_Tqual _T8E6;struct Cyc_List_List*_T8E7;struct _RegionHandle*_T8E8;struct Cyc_List_List*_T8E9;struct _RegionHandle*_T8EA;struct Cyc_Yystacktype*_T8EB;union Cyc_YYSTYPE*_T8EC;union Cyc_YYSTYPE*_T8ED;struct Cyc_Yystacktype*_T8EE;union Cyc_YYSTYPE*_T8EF;union Cyc_YYSTYPE*_T8F0;struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T8F1;struct _RegionHandle*_T8F2;struct Cyc_Yystacktype*_T8F3;union Cyc_YYSTYPE*_T8F4;union Cyc_YYSTYPE*_T8F5;void*_T8F6;struct Cyc_Yystacktype*_T8F7;struct Cyc_Yystacktype _T8F8;struct Cyc_Yyltype _T8F9;unsigned _T8FA;unsigned _T8FB;struct _fat_ptr _T8FC;struct _fat_ptr _T8FD;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8FE;struct _RegionHandle*_T8FF;struct Cyc_List_List*_T900;struct Cyc_Yystacktype*_T901;union Cyc_YYSTYPE*_T902;union Cyc_YYSTYPE*_T903;void*_T904;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T905;struct _RegionHandle*_T906;struct Cyc_Yystacktype*_T907;union Cyc_YYSTYPE*_T908;union Cyc_YYSTYPE*_T909;void*_T90A;struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T90B;struct _RegionHandle*_T90C;void*_T90D;struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T90E;struct _RegionHandle*_T90F;void*_T910;struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T911;struct _RegionHandle*_T912;void*_T913;struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T914;struct _RegionHandle*_T915;void*_T916;struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T917;struct _RegionHandle*_T918;void*_T919;struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T91A;struct _RegionHandle*_T91B;void*_T91C;struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T91D;struct _RegionHandle*_T91E;void*_T91F;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T920;struct _RegionHandle*_T921;struct Cyc_Yystacktype*_T922;union Cyc_YYSTYPE*_T923;union Cyc_YYSTYPE*_T924;void*_T925;struct Cyc_Yystacktype*_T926;struct Cyc_Yystacktype _T927;struct Cyc_Yyltype _T928;unsigned _T929;unsigned _T92A;struct Cyc_Yystacktype*_T92B;union Cyc_YYSTYPE*_T92C;union Cyc_YYSTYPE*_T92D;struct _fat_ptr _T92E;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T92F;struct _RegionHandle*_T930;void*_T931;struct Cyc_Yystacktype*_T932;union Cyc_YYSTYPE*_T933;union Cyc_YYSTYPE*_T934;void*_T935;struct Cyc_Yystacktype*_T936;union Cyc_YYSTYPE*_T937;union Cyc_YYSTYPE*_T938;void*_T939;void*_T93A;void*_T93B;struct Cyc_Yystacktype*_T93C;union Cyc_YYSTYPE*_T93D;union Cyc_YYSTYPE*_T93E;void*_T93F;void*_T940;void*_T941;void*_T942;struct Cyc_Yystacktype*_T943;struct Cyc_Yystacktype _T944;struct Cyc_Yyltype _T945;unsigned _T946;unsigned _T947;struct Cyc_Yystacktype*_T948;union Cyc_YYSTYPE*_T949;union Cyc_YYSTYPE*_T94A;struct _fat_ptr _T94B;void*_T94C;struct _tuple22*_T94D;struct Cyc_Yystacktype*_T94E;struct Cyc_Yystacktype _T94F;struct Cyc_Yyltype _T950;unsigned _T951;int _T952;struct Cyc_Yystacktype*_T953;struct Cyc_Yystacktype _T954;struct Cyc_Yyltype _T955;unsigned _T956;unsigned _T957;struct _fat_ptr _T958;struct Cyc_Yystacktype*_T959;union Cyc_YYSTYPE*_T95A;union Cyc_YYSTYPE*_T95B;void*_T95C;struct _tuple22*_T95D;struct Cyc_Yystacktype*_T95E;struct Cyc_Yystacktype _T95F;struct Cyc_Yyltype _T960;unsigned _T961;struct Cyc_Yystacktype*_T962;struct Cyc_Yystacktype _T963;struct Cyc_Yyltype _T964;unsigned _T965;unsigned _T966;struct _fat_ptr _T967;struct Cyc_Yystacktype*_T968;union Cyc_YYSTYPE*_T969;union Cyc_YYSTYPE*_T96A;void*_T96B;struct _tuple22*_T96C;int _T96D;int _T96E;struct _tuple22*_T96F;struct Cyc_Yystacktype*_T970;struct Cyc_Yystacktype _T971;struct Cyc_Yyltype _T972;unsigned _T973;struct Cyc_Yystacktype*_T974;struct Cyc_Yystacktype _T975;struct Cyc_Yyltype _T976;unsigned _T977;unsigned _T978;struct _fat_ptr _T979;struct Cyc_Core_Opt*_T97A;struct Cyc_Core_Opt*_T97B;void*_T97C;struct _tuple22*_T97D;struct Cyc_Yystacktype*_T97E;struct Cyc_Yystacktype _T97F;struct Cyc_Yyltype _T980;unsigned _T981;void*_T982;int _T983;struct Cyc_Core_Opt*_T984;struct Cyc_Core_Opt*_T985;struct Cyc_Yystacktype*_T986;union Cyc_YYSTYPE*_T987;union Cyc_YYSTYPE*_T988;struct Cyc_Absyn_Exp*_T989;void*_T98A;int _T98B;int(*_T98C)(unsigned,struct _fat_ptr);struct _fat_ptr _T98D;struct Cyc_Yystacktype*_T98E;union Cyc_YYSTYPE*_T98F;union Cyc_YYSTYPE*_T990;struct _fat_ptr _T991;void*_T992;void*_T993;struct Cyc_List_List*_T994;struct Cyc_Yystacktype*_T995;union Cyc_YYSTYPE*_T996;union Cyc_YYSTYPE*_T997;struct Cyc_List_List*_T998;struct Cyc_Yystacktype*_T999;union Cyc_YYSTYPE*_T99A;union Cyc_YYSTYPE*_T99B;struct Cyc_Yystacktype*_T99C;union Cyc_YYSTYPE*_T99D;union Cyc_YYSTYPE*_T99E;struct Cyc_Core_Opt*_T99F;struct Cyc_Core_Opt*_T9A0;void*_T9A1;struct Cyc_Yystacktype*_T9A2;union Cyc_YYSTYPE*_T9A3;union Cyc_YYSTYPE*_T9A4;int _T9A5;struct Cyc_List_List*_T9A6;void*_T9A7;struct Cyc_Yystacktype*_T9A8;union Cyc_YYSTYPE*_T9A9;union Cyc_YYSTYPE*_T9AA;struct Cyc_List_List*_T9AB;void*_T9AC;struct Cyc_Core_Opt*_T9AD;struct Cyc_Core_Opt*_T9AE;void*_T9AF;struct _fat_ptr _T9B0;int _T9B1;unsigned char*_T9B2;struct Cyc_Yystacktype*_T9B3;struct Cyc_Yystacktype _T9B4;struct Cyc_Yyltype _T9B5;unsigned _T9B6;unsigned _T9B7;struct Cyc_Absyn_Tqual _T9B8;struct Cyc_Yystacktype*_T9B9;union Cyc_YYSTYPE*_T9BA;union Cyc_YYSTYPE*_T9BB;struct Cyc_Absyn_Tqual _T9BC;struct Cyc_Yystacktype*_T9BD;union Cyc_YYSTYPE*_T9BE;union Cyc_YYSTYPE*_T9BF;struct Cyc_Absyn_Tqual _T9C0;struct Cyc_Absyn_Tqual _T9C1;struct Cyc_Yystacktype*_T9C2;union Cyc_YYSTYPE*_T9C3;union Cyc_YYSTYPE*_T9C4;struct _tuple28 _T9C5;struct _tuple28*_T9C6;unsigned _T9C7;struct _tuple28*_T9C8;struct _tuple28 _T9C9;struct _tuple27*_T9CA;struct Cyc_Yystacktype*_T9CB;union Cyc_YYSTYPE*_T9CC;union Cyc_YYSTYPE*_T9CD;struct Cyc_Yystacktype*_T9CE;union Cyc_YYSTYPE*_T9CF;union Cyc_YYSTYPE*_T9D0;struct _tuple28 _T9D1;struct _tuple28*_T9D2;unsigned _T9D3;struct _tuple28*_T9D4;struct _tuple28 _T9D5;struct _tuple27*_T9D6;struct Cyc_Yystacktype*_T9D7;union Cyc_YYSTYPE*_T9D8;union Cyc_YYSTYPE*_T9D9;struct Cyc_List_List*_T9DA;struct Cyc_Yystacktype*_T9DB;union Cyc_YYSTYPE*_T9DC;union Cyc_YYSTYPE*_T9DD;struct Cyc_Yystacktype*_T9DE;union Cyc_YYSTYPE*_T9DF;union Cyc_YYSTYPE*_T9E0;struct _tuple28 _T9E1;struct _tuple28*_T9E2;unsigned _T9E3;struct _tuple28*_T9E4;struct _tuple28 _T9E5;struct _tuple27*_T9E6;struct Cyc_Yystacktype*_T9E7;union Cyc_YYSTYPE*_T9E8;union Cyc_YYSTYPE*_T9E9;struct Cyc_List_List*_T9EA;struct Cyc_Yystacktype*_T9EB;union Cyc_YYSTYPE*_T9EC;union Cyc_YYSTYPE*_T9ED;struct Cyc_Yystacktype*_T9EE;union Cyc_YYSTYPE*_T9EF;union Cyc_YYSTYPE*_T9F0;struct Cyc_Absyn_VarargInfo*_T9F1;struct Cyc_Absyn_VarargInfo*_T9F2;struct Cyc_Absyn_VarargInfo*_T9F3;struct Cyc_Absyn_VarargInfo*_T9F4;struct Cyc_Yystacktype*_T9F5;union Cyc_YYSTYPE*_T9F6;union Cyc_YYSTYPE*_T9F7;struct Cyc_Yystacktype*_T9F8;union Cyc_YYSTYPE*_T9F9;union Cyc_YYSTYPE*_T9FA;struct _tuple28 _T9FB;struct _tuple28*_T9FC;unsigned _T9FD;struct _tuple28*_T9FE;struct _tuple28 _T9FF;struct _tuple27*_TA00;struct Cyc_Yystacktype*_TA01;union Cyc_YYSTYPE*_TA02;union Cyc_YYSTYPE*_TA03;struct Cyc_Yystacktype*_TA04;union Cyc_YYSTYPE*_TA05;union Cyc_YYSTYPE*_TA06;struct Cyc_Absyn_VarargInfo*_TA07;struct Cyc_Absyn_VarargInfo*_TA08;struct Cyc_Absyn_VarargInfo*_TA09;struct Cyc_Absyn_VarargInfo*_TA0A;struct Cyc_Yystacktype*_TA0B;union Cyc_YYSTYPE*_TA0C;union Cyc_YYSTYPE*_TA0D;struct Cyc_Yystacktype*_TA0E;union Cyc_YYSTYPE*_TA0F;union Cyc_YYSTYPE*_TA10;struct _tuple28 _TA11;struct _tuple28*_TA12;unsigned _TA13;struct _tuple28*_TA14;struct _tuple28 _TA15;struct _tuple27*_TA16;struct Cyc_Yystacktype*_TA17;union Cyc_YYSTYPE*_TA18;union Cyc_YYSTYPE*_TA19;struct Cyc_List_List*_TA1A;struct Cyc_Yystacktype*_TA1B;union Cyc_YYSTYPE*_TA1C;union Cyc_YYSTYPE*_TA1D;struct Cyc_Yystacktype*_TA1E;struct Cyc_Yystacktype _TA1F;struct Cyc_Yyltype _TA20;unsigned _TA21;unsigned _TA22;struct Cyc_Yystacktype*_TA23;union Cyc_YYSTYPE*_TA24;union Cyc_YYSTYPE*_TA25;struct _fat_ptr _TA26;void*_TA27;struct Cyc_Yystacktype*_TA28;union Cyc_YYSTYPE*_TA29;union Cyc_YYSTYPE*_TA2A;struct _fat_ptr _TA2B;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_TA2C;void*_TA2D;struct Cyc_Yystacktype*_TA2E;union Cyc_YYSTYPE*_TA2F;union Cyc_YYSTYPE*_TA30;void*_TA31;struct Cyc_Yystacktype*_TA32;struct Cyc_Yystacktype _TA33;struct Cyc_Yyltype _TA34;unsigned _TA35;unsigned _TA36;void*_TA37;struct Cyc_Yystacktype*_TA38;union Cyc_YYSTYPE*_TA39;union Cyc_YYSTYPE*_TA3A;struct _fat_ptr _TA3B;struct Cyc_Yystacktype*_TA3C;union Cyc_YYSTYPE*_TA3D;union Cyc_YYSTYPE*_TA3E;struct Cyc_Absyn_Kind*_TA3F;void*_TA40;struct Cyc_Yystacktype*_TA41;union Cyc_YYSTYPE*_TA42;union Cyc_YYSTYPE*_TA43;void*_TA44;struct Cyc_Yystacktype*_TA45;struct Cyc_Yystacktype _TA46;struct Cyc_Yyltype _TA47;unsigned _TA48;unsigned _TA49;void*_TA4A;struct Cyc_Yystacktype*_TA4B;union Cyc_YYSTYPE*_TA4C;union Cyc_YYSTYPE*_TA4D;struct Cyc_List_List*_TA4E;void*_TA4F;struct Cyc_Yystacktype*_TA50;struct Cyc_Yystacktype _TA51;struct _tuple28*_TA52;struct Cyc_List_List*_TA53;struct Cyc_Yystacktype*_TA54;union Cyc_YYSTYPE*_TA55;union Cyc_YYSTYPE*_TA56;struct _fat_ptr _TA57;struct _tuple28*_TA58;struct Cyc_List_List*_TA59;struct Cyc_Yystacktype*_TA5A;union Cyc_YYSTYPE*_TA5B;union Cyc_YYSTYPE*_TA5C;struct _fat_ptr _TA5D;struct Cyc_Yystacktype*_TA5E;union Cyc_YYSTYPE*_TA5F;union Cyc_YYSTYPE*_TA60;struct _tuple28*_TA61;struct _tuple28*_TA62;struct Cyc_List_List*_TA63;struct Cyc_Yystacktype*_TA64;union Cyc_YYSTYPE*_TA65;union Cyc_YYSTYPE*_TA66;struct Cyc_Yystacktype*_TA67;union Cyc_YYSTYPE*_TA68;union Cyc_YYSTYPE*_TA69;struct _tuple28*_TA6A;struct _tuple28*_TA6B;struct Cyc_List_List*_TA6C;struct Cyc_Yystacktype*_TA6D;union Cyc_YYSTYPE*_TA6E;union Cyc_YYSTYPE*_TA6F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA70;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA71;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA72;struct Cyc_Absyn_Kind*_TA73;struct Cyc_Yystacktype*_TA74;union Cyc_YYSTYPE*_TA75;union Cyc_YYSTYPE*_TA76;struct _fat_ptr _TA77;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA78;void*_TA79;struct Cyc_Yystacktype*_TA7A;struct Cyc_Yystacktype _TA7B;struct Cyc_Yyltype _TA7C;unsigned _TA7D;unsigned _TA7E;struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_TA7F;struct Cyc_Yystacktype*_TA80;union Cyc_YYSTYPE*_TA81;union Cyc_YYSTYPE*_TA82;struct Cyc_List_List*_TA83;void*_TA84;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_TA85;struct Cyc_Yystacktype*_TA86;union Cyc_YYSTYPE*_TA87;union Cyc_YYSTYPE*_TA88;struct Cyc_List_List*_TA89;struct Cyc_Yystacktype*_TA8A;union Cyc_YYSTYPE*_TA8B;union Cyc_YYSTYPE*_TA8C;struct Cyc_List_List*_TA8D;void*_TA8E;struct Cyc_Yystacktype*_TA8F;struct Cyc_Yystacktype _TA90;struct Cyc_Yyltype _TA91;unsigned _TA92;unsigned _TA93;struct Cyc_Yystacktype*_TA94;union Cyc_YYSTYPE*_TA95;union Cyc_YYSTYPE*_TA96;struct _fat_ptr _TA97;struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_TA98;struct Cyc_Yystacktype*_TA99;union Cyc_YYSTYPE*_TA9A;union Cyc_YYSTYPE*_TA9B;struct Cyc_List_List*_TA9C;void*_TA9D;struct _tuple29*_TA9E;struct Cyc_Yystacktype*_TA9F;union Cyc_YYSTYPE*_TAA0;union Cyc_YYSTYPE*_TAA1;struct Cyc_Yystacktype*_TAA2;union Cyc_YYSTYPE*_TAA3;union Cyc_YYSTYPE*_TAA4;struct Cyc_Yystacktype*_TAA5;struct Cyc_Yystacktype _TAA6;struct Cyc_Yyltype _TAA7;unsigned _TAA8;unsigned _TAA9;struct Cyc_Yystacktype*_TAAA;union Cyc_YYSTYPE*_TAAB;union Cyc_YYSTYPE*_TAAC;struct _fat_ptr _TAAD;void*_TAAE;struct Cyc_Yystacktype*_TAAF;union Cyc_YYSTYPE*_TAB0;union Cyc_YYSTYPE*_TAB1;void*_TAB2;struct Cyc_Yystacktype*_TAB3;union Cyc_YYSTYPE*_TAB4;union Cyc_YYSTYPE*_TAB5;void*_TAB6;void*_TAB7;void*_TAB8;void*_TAB9;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TABA;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TABB;struct Cyc_Absyn_Kind*_TABC;struct Cyc_Yystacktype*_TABD;union Cyc_YYSTYPE*_TABE;union Cyc_YYSTYPE*_TABF;struct _fat_ptr _TAC0;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TAC1;void*_TAC2;struct Cyc_Yystacktype*_TAC3;struct Cyc_Yystacktype _TAC4;struct Cyc_Yyltype _TAC5;unsigned _TAC6;unsigned _TAC7;void*_TAC8;struct Cyc_Yystacktype*_TAC9;union Cyc_YYSTYPE*_TACA;union Cyc_YYSTYPE*_TACB;void*_TACC;void*_TACD;struct Cyc_Yystacktype*_TACE;union Cyc_YYSTYPE*_TACF;union Cyc_YYSTYPE*_TAD0;struct _fat_ptr _TAD1;struct _fat_ptr _TAD2;int _TAD3;struct Cyc_Warn_String_Warn_Warg_struct _TAD4;struct Cyc_Yystacktype*_TAD5;struct Cyc_Yystacktype _TAD6;struct Cyc_Yyltype _TAD7;unsigned _TAD8;unsigned _TAD9;struct _fat_ptr _TADA;struct Cyc_Yystacktype*_TADB;struct Cyc_Yystacktype _TADC;struct Cyc_Yystacktype*_TADD;union Cyc_YYSTYPE*_TADE;union Cyc_YYSTYPE*_TADF;struct Cyc_List_List*_TAE0;struct Cyc_Yystacktype*_TAE1;union Cyc_YYSTYPE*_TAE2;union Cyc_YYSTYPE*_TAE3;struct Cyc_List_List*_TAE4;struct Cyc_List_List*_TAE5;struct Cyc_Yystacktype*_TAE6;struct Cyc_Yystacktype _TAE7;struct Cyc_List_List*_TAE8;struct Cyc_Yystacktype*_TAE9;union Cyc_YYSTYPE*_TAEA;union Cyc_YYSTYPE*_TAEB;void*_TAEC;struct Cyc_Yystacktype*_TAED;union Cyc_YYSTYPE*_TAEE;union Cyc_YYSTYPE*_TAEF;void*_TAF0;struct Cyc_Absyn_Kind*_TAF1;struct Cyc_Absyn_Kind*_TAF2;struct Cyc_List_List*_TAF3;struct Cyc_Yystacktype*_TAF4;union Cyc_YYSTYPE*_TAF5;union Cyc_YYSTYPE*_TAF6;struct Cyc_List_List*_TAF7;struct Cyc_Yystacktype*_TAF8;union Cyc_YYSTYPE*_TAF9;union Cyc_YYSTYPE*_TAFA;struct _tuple8*_TAFB;struct Cyc_Yystacktype*_TAFC;struct Cyc_Yystacktype _TAFD;struct Cyc_Yyltype _TAFE;unsigned _TAFF;unsigned _TB00;struct Cyc_List_List*_TB01;struct Cyc_Yystacktype*_TB02;union Cyc_YYSTYPE*_TB03;union Cyc_YYSTYPE*_TB04;struct _tuple8*_TB05;struct Cyc_Yystacktype*_TB06;struct Cyc_Yystacktype _TB07;struct Cyc_Yyltype _TB08;unsigned _TB09;unsigned _TB0A;struct Cyc_Yystacktype*_TB0B;union Cyc_YYSTYPE*_TB0C;union Cyc_YYSTYPE*_TB0D;struct Cyc_List_List*_TB0E;struct Cyc_Yystacktype*_TB0F;union Cyc_YYSTYPE*_TB10;union Cyc_YYSTYPE*_TB11;struct Cyc_List_List*_TB12;struct Cyc_Yystacktype*_TB13;union Cyc_YYSTYPE*_TB14;union Cyc_YYSTYPE*_TB15;struct Cyc_Yystacktype*_TB16;union Cyc_YYSTYPE*_TB17;union Cyc_YYSTYPE*_TB18;struct Cyc_Yystacktype*_TB19;union Cyc_YYSTYPE*_TB1A;union Cyc_YYSTYPE*_TB1B;struct Cyc_Absyn_Tqual _TB1C;unsigned _TB1D;struct Cyc_Yystacktype*_TB1E;struct Cyc_Yystacktype _TB1F;struct Cyc_Yyltype _TB20;unsigned _TB21;struct Cyc_Yystacktype*_TB22;union Cyc_YYSTYPE*_TB23;union Cyc_YYSTYPE*_TB24;struct Cyc_Parse_Type_specifier _TB25;struct Cyc_Yystacktype*_TB26;struct Cyc_Yystacktype _TB27;struct Cyc_Yyltype _TB28;unsigned _TB29;unsigned _TB2A;struct Cyc_Warn_String_Warn_Warg_struct _TB2B;struct Cyc_Yystacktype*_TB2C;struct Cyc_Yystacktype _TB2D;struct Cyc_Yyltype _TB2E;unsigned _TB2F;unsigned _TB30;struct _fat_ptr _TB31;int _TB32;struct Cyc_Warn_String_Warn_Warg_struct _TB33;struct Cyc_Yystacktype*_TB34;struct Cyc_Yystacktype _TB35;struct Cyc_Yyltype _TB36;unsigned _TB37;unsigned _TB38;struct _fat_ptr _TB39;struct _tuple0*_TB3A;struct _tuple0 _TB3B;struct Cyc_Warn_String_Warn_Warg_struct _TB3C;struct Cyc_Yystacktype*_TB3D;struct Cyc_Yystacktype _TB3E;struct Cyc_Yyltype _TB3F;unsigned _TB40;unsigned _TB41;struct _fat_ptr _TB42;struct _tuple8*_TB43;struct Cyc_Yystacktype*_TB44;union Cyc_YYSTYPE*_TB45;union Cyc_YYSTYPE*_TB46;struct Cyc_Absyn_Tqual _TB47;unsigned _TB48;struct Cyc_Yystacktype*_TB49;struct Cyc_Yystacktype _TB4A;struct Cyc_Yyltype _TB4B;unsigned _TB4C;struct Cyc_Parse_Type_specifier _TB4D;struct Cyc_Yystacktype*_TB4E;struct Cyc_Yystacktype _TB4F;struct Cyc_Yyltype _TB50;unsigned _TB51;unsigned _TB52;struct Cyc_Warn_String_Warn_Warg_struct _TB53;struct Cyc_Yystacktype*_TB54;struct Cyc_Yystacktype _TB55;struct Cyc_Yyltype _TB56;unsigned _TB57;unsigned _TB58;struct _fat_ptr _TB59;struct _tuple8*_TB5A;struct Cyc_Yystacktype*_TB5B;union Cyc_YYSTYPE*_TB5C;union Cyc_YYSTYPE*_TB5D;struct Cyc_Absyn_Tqual _TB5E;unsigned _TB5F;struct Cyc_Yystacktype*_TB60;struct Cyc_Yystacktype _TB61;struct Cyc_Yyltype _TB62;unsigned _TB63;struct Cyc_Parse_Type_specifier _TB64;struct Cyc_Yystacktype*_TB65;struct Cyc_Yystacktype _TB66;struct Cyc_Yyltype _TB67;unsigned _TB68;unsigned _TB69;struct Cyc_Yystacktype*_TB6A;union Cyc_YYSTYPE*_TB6B;union Cyc_YYSTYPE*_TB6C;struct Cyc_Parse_Abstractdeclarator _TB6D;struct Cyc_Warn_String_Warn_Warg_struct _TB6E;struct Cyc_Yystacktype*_TB6F;struct Cyc_Yystacktype _TB70;struct Cyc_Yyltype _TB71;unsigned _TB72;unsigned _TB73;struct _fat_ptr _TB74;struct Cyc_Warn_String_Warn_Warg_struct _TB75;struct Cyc_Yystacktype*_TB76;struct Cyc_Yystacktype _TB77;struct Cyc_Yyltype _TB78;unsigned _TB79;unsigned _TB7A;struct _fat_ptr _TB7B;struct _tuple8*_TB7C;struct Cyc_Yystacktype*_TB7D;union Cyc_YYSTYPE*_TB7E;union Cyc_YYSTYPE*_TB7F;struct Cyc_List_List*_TB80;struct Cyc_List_List*_TB81;struct Cyc_List_List*_TB82;struct _fat_ptr*_TB83;struct Cyc_Yystacktype*_TB84;union Cyc_YYSTYPE*_TB85;union Cyc_YYSTYPE*_TB86;struct Cyc_List_List*_TB87;struct _fat_ptr*_TB88;struct Cyc_Yystacktype*_TB89;union Cyc_YYSTYPE*_TB8A;union Cyc_YYSTYPE*_TB8B;struct Cyc_Yystacktype*_TB8C;union Cyc_YYSTYPE*_TB8D;union Cyc_YYSTYPE*_TB8E;struct Cyc_Yystacktype*_TB8F;struct Cyc_Yystacktype _TB90;struct Cyc_Yystacktype*_TB91;struct Cyc_Yystacktype _TB92;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB93;void*_TB94;struct Cyc_Yystacktype*_TB95;struct Cyc_Yystacktype _TB96;struct Cyc_Yyltype _TB97;unsigned _TB98;unsigned _TB99;struct Cyc_Absyn_Exp*_TB9A;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB9B;struct Cyc_Yystacktype*_TB9C;union Cyc_YYSTYPE*_TB9D;union Cyc_YYSTYPE*_TB9E;struct Cyc_List_List*_TB9F;void*_TBA0;struct Cyc_Yystacktype*_TBA1;struct Cyc_Yystacktype _TBA2;struct Cyc_Yyltype _TBA3;unsigned _TBA4;unsigned _TBA5;struct Cyc_Absyn_Exp*_TBA6;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TBA7;struct Cyc_Yystacktype*_TBA8;union Cyc_YYSTYPE*_TBA9;union Cyc_YYSTYPE*_TBAA;struct Cyc_List_List*_TBAB;void*_TBAC;struct Cyc_Yystacktype*_TBAD;struct Cyc_Yystacktype _TBAE;struct Cyc_Yyltype _TBAF;unsigned _TBB0;unsigned _TBB1;struct Cyc_Absyn_Exp*_TBB2;struct Cyc_Yystacktype*_TBB3;struct Cyc_Yystacktype _TBB4;struct Cyc_Yyltype _TBB5;unsigned _TBB6;unsigned _TBB7;struct _tuple0*_TBB8;struct _fat_ptr*_TBB9;struct Cyc_Yystacktype*_TBBA;union Cyc_YYSTYPE*_TBBB;union Cyc_YYSTYPE*_TBBC;void*_TBBD;struct Cyc_Yystacktype*_TBBE;struct Cyc_Yystacktype _TBBF;struct Cyc_Yyltype _TBC0;unsigned _TBC1;unsigned _TBC2;struct Cyc_Absyn_Exp*_TBC3;struct Cyc_Absyn_Vardecl*_TBC4;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TBC5;struct Cyc_Yystacktype*_TBC6;union Cyc_YYSTYPE*_TBC7;union Cyc_YYSTYPE*_TBC8;struct Cyc_Yystacktype*_TBC9;union Cyc_YYSTYPE*_TBCA;union Cyc_YYSTYPE*_TBCB;void*_TBCC;struct Cyc_Yystacktype*_TBCD;struct Cyc_Yystacktype _TBCE;struct Cyc_Yyltype _TBCF;unsigned _TBD0;unsigned _TBD1;struct Cyc_Absyn_Exp*_TBD2;struct Cyc_Yystacktype*_TBD3;union Cyc_YYSTYPE*_TBD4;union Cyc_YYSTYPE*_TBD5;struct _tuple8*_TBD6;struct Cyc_Yystacktype*_TBD7;struct Cyc_Yystacktype _TBD8;struct Cyc_Yyltype _TBD9;unsigned _TBDA;unsigned _TBDB;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TBDC;struct Cyc_Yystacktype*_TBDD;union Cyc_YYSTYPE*_TBDE;union Cyc_YYSTYPE*_TBDF;void*_TBE0;struct Cyc_Yystacktype*_TBE1;struct Cyc_Yystacktype _TBE2;struct Cyc_Yyltype _TBE3;unsigned _TBE4;unsigned _TBE5;struct Cyc_Absyn_Exp*_TBE6;struct Cyc_List_List*_TBE7;struct _tuple34*_TBE8;struct Cyc_Yystacktype*_TBE9;union Cyc_YYSTYPE*_TBEA;union Cyc_YYSTYPE*_TBEB;struct Cyc_List_List*_TBEC;struct _tuple34*_TBED;struct Cyc_Yystacktype*_TBEE;union Cyc_YYSTYPE*_TBEF;union Cyc_YYSTYPE*_TBF0;struct Cyc_Yystacktype*_TBF1;union Cyc_YYSTYPE*_TBF2;union Cyc_YYSTYPE*_TBF3;struct Cyc_List_List*_TBF4;struct _tuple34*_TBF5;struct Cyc_Yystacktype*_TBF6;union Cyc_YYSTYPE*_TBF7;union Cyc_YYSTYPE*_TBF8;struct Cyc_Yystacktype*_TBF9;union Cyc_YYSTYPE*_TBFA;union Cyc_YYSTYPE*_TBFB;struct Cyc_List_List*_TBFC;struct _tuple34*_TBFD;struct Cyc_Yystacktype*_TBFE;union Cyc_YYSTYPE*_TBFF;union Cyc_YYSTYPE*_TC00;struct Cyc_Yystacktype*_TC01;union Cyc_YYSTYPE*_TC02;union Cyc_YYSTYPE*_TC03;struct Cyc_Yystacktype*_TC04;union Cyc_YYSTYPE*_TC05;union Cyc_YYSTYPE*_TC06;struct Cyc_Yystacktype*_TC07;union Cyc_YYSTYPE*_TC08;union Cyc_YYSTYPE*_TC09;struct Cyc_List_List*_TC0A;struct Cyc_List_List*_TC0B;struct Cyc_List_List*_TC0C;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TC0D;struct _fat_ptr*_TC0E;struct Cyc_Yystacktype*_TC0F;union Cyc_YYSTYPE*_TC10;union Cyc_YYSTYPE*_TC11;struct Cyc_List_List*_TC12;struct Cyc_Yystacktype*_TC13;union Cyc_YYSTYPE*_TC14;union Cyc_YYSTYPE*_TC15;struct Cyc_List_List*_TC16;struct Cyc_Yystacktype*_TC17;union Cyc_YYSTYPE*_TC18;union Cyc_YYSTYPE*_TC19;struct Cyc_Yystacktype*_TC1A;union Cyc_YYSTYPE*_TC1B;union Cyc_YYSTYPE*_TC1C;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_TC1D;struct Cyc_Yystacktype*_TC1E;union Cyc_YYSTYPE*_TC1F;union Cyc_YYSTYPE*_TC20;void*_TC21;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TC22;struct _fat_ptr*_TC23;struct Cyc_Yystacktype*_TC24;union Cyc_YYSTYPE*_TC25;union Cyc_YYSTYPE*_TC26;void*_TC27;struct Cyc_Yystacktype*_TC28;union Cyc_YYSTYPE*_TC29;union Cyc_YYSTYPE*_TC2A;struct Cyc_Parse_Type_specifier _TC2B;struct Cyc_Yystacktype*_TC2C;struct Cyc_Yystacktype _TC2D;struct Cyc_Yyltype _TC2E;unsigned _TC2F;unsigned _TC30;struct Cyc_Yystacktype*_TC31;struct Cyc_Yystacktype _TC32;struct Cyc_Yyltype _TC33;unsigned _TC34;unsigned _TC35;struct _fat_ptr _TC36;struct _fat_ptr _TC37;struct _tuple8*_TC38;struct Cyc_Yystacktype*_TC39;union Cyc_YYSTYPE*_TC3A;union Cyc_YYSTYPE*_TC3B;struct Cyc_Parse_Type_specifier _TC3C;struct Cyc_Yystacktype*_TC3D;struct Cyc_Yystacktype _TC3E;struct Cyc_Yyltype _TC3F;unsigned _TC40;unsigned _TC41;struct Cyc_Yystacktype*_TC42;union Cyc_YYSTYPE*_TC43;union Cyc_YYSTYPE*_TC44;struct Cyc_Parse_Abstractdeclarator _TC45;struct _tuple14 _TC46;struct Cyc_List_List*_TC47;struct Cyc_Yystacktype*_TC48;struct Cyc_Yystacktype _TC49;struct Cyc_Yyltype _TC4A;unsigned _TC4B;unsigned _TC4C;struct _fat_ptr _TC4D;struct _fat_ptr _TC4E;struct _tuple14 _TC4F;struct Cyc_List_List*_TC50;struct Cyc_Yystacktype*_TC51;struct Cyc_Yystacktype _TC52;struct Cyc_Yyltype _TC53;unsigned _TC54;unsigned _TC55;struct _fat_ptr _TC56;struct _fat_ptr _TC57;struct _tuple8*_TC58;struct _tuple14 _TC59;struct _tuple14 _TC5A;struct Cyc_Yystacktype*_TC5B;union Cyc_YYSTYPE*_TC5C;union Cyc_YYSTYPE*_TC5D;struct _tuple8*_TC5E;struct Cyc_Yystacktype*_TC5F;struct Cyc_Yystacktype _TC60;struct Cyc_Yyltype _TC61;unsigned _TC62;unsigned _TC63;void*_TC64;void*_TC65;struct Cyc_Yystacktype*_TC66;union Cyc_YYSTYPE*_TC67;union Cyc_YYSTYPE*_TC68;struct Cyc_List_List*_TC69;void*_TC6A;struct Cyc_Yystacktype*_TC6B;union Cyc_YYSTYPE*_TC6C;union Cyc_YYSTYPE*_TC6D;void*_TC6E;void*_TC6F;struct Cyc_List_List*_TC70;struct Cyc_Yystacktype*_TC71;union Cyc_YYSTYPE*_TC72;union Cyc_YYSTYPE*_TC73;struct Cyc_Yystacktype*_TC74;union Cyc_YYSTYPE*_TC75;union Cyc_YYSTYPE*_TC76;void*_TC77;struct Cyc_Yystacktype*_TC78;union Cyc_YYSTYPE*_TC79;union Cyc_YYSTYPE*_TC7A;void*_TC7B;struct Cyc_List_List*_TC7C;struct Cyc_Yystacktype*_TC7D;union Cyc_YYSTYPE*_TC7E;union Cyc_YYSTYPE*_TC7F;struct Cyc_List_List*_TC80;struct Cyc_Yystacktype*_TC81;union Cyc_YYSTYPE*_TC82;union Cyc_YYSTYPE*_TC83;struct Cyc_Yystacktype*_TC84;union Cyc_YYSTYPE*_TC85;union Cyc_YYSTYPE*_TC86;struct Cyc_Parse_Abstractdeclarator _TC87;struct Cyc_Yystacktype*_TC88;union Cyc_YYSTYPE*_TC89;union Cyc_YYSTYPE*_TC8A;struct Cyc_Yystacktype*_TC8B;struct Cyc_Yystacktype _TC8C;struct Cyc_Parse_Abstractdeclarator _TC8D;struct Cyc_Yystacktype*_TC8E;union Cyc_YYSTYPE*_TC8F;union Cyc_YYSTYPE*_TC90;struct Cyc_List_List*_TC91;struct Cyc_Yystacktype*_TC92;union Cyc_YYSTYPE*_TC93;union Cyc_YYSTYPE*_TC94;struct Cyc_Parse_Abstractdeclarator _TC95;struct Cyc_List_List*_TC96;struct Cyc_Yystacktype*_TC97;struct Cyc_Yystacktype _TC98;struct Cyc_Parse_Abstractdeclarator _TC99;struct Cyc_List_List*_TC9A;struct _RegionHandle*_TC9B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC9C;struct _RegionHandle*_TC9D;struct Cyc_Yystacktype*_TC9E;union Cyc_YYSTYPE*_TC9F;union Cyc_YYSTYPE*_TCA0;struct Cyc_Yystacktype*_TCA1;struct Cyc_Yystacktype _TCA2;struct Cyc_Yyltype _TCA3;unsigned _TCA4;struct Cyc_Parse_Abstractdeclarator _TCA5;struct Cyc_List_List*_TCA6;struct _RegionHandle*_TCA7;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TCA8;struct _RegionHandle*_TCA9;struct Cyc_Yystacktype*_TCAA;union Cyc_YYSTYPE*_TCAB;union Cyc_YYSTYPE*_TCAC;struct Cyc_Yystacktype*_TCAD;struct Cyc_Yystacktype _TCAE;struct Cyc_Yyltype _TCAF;unsigned _TCB0;struct Cyc_Yystacktype*_TCB1;union Cyc_YYSTYPE*_TCB2;union Cyc_YYSTYPE*_TCB3;struct Cyc_Parse_Abstractdeclarator _TCB4;struct Cyc_Parse_Abstractdeclarator _TCB5;struct Cyc_List_List*_TCB6;struct _RegionHandle*_TCB7;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TCB8;struct _RegionHandle*_TCB9;struct Cyc_Yystacktype*_TCBA;union Cyc_YYSTYPE*_TCBB;union Cyc_YYSTYPE*_TCBC;struct Cyc_Yystacktype*_TCBD;union Cyc_YYSTYPE*_TCBE;union Cyc_YYSTYPE*_TCBF;struct Cyc_Yystacktype*_TCC0;struct Cyc_Yystacktype _TCC1;struct Cyc_Yyltype _TCC2;unsigned _TCC3;struct Cyc_Parse_Abstractdeclarator _TCC4;struct Cyc_List_List*_TCC5;struct _RegionHandle*_TCC6;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TCC7;struct _RegionHandle*_TCC8;struct Cyc_Yystacktype*_TCC9;union Cyc_YYSTYPE*_TCCA;union Cyc_YYSTYPE*_TCCB;struct Cyc_Yystacktype*_TCCC;union Cyc_YYSTYPE*_TCCD;union Cyc_YYSTYPE*_TCCE;struct Cyc_Yystacktype*_TCCF;struct Cyc_Yystacktype _TCD0;struct Cyc_Yyltype _TCD1;unsigned _TCD2;struct Cyc_Yystacktype*_TCD3;union Cyc_YYSTYPE*_TCD4;union Cyc_YYSTYPE*_TCD5;struct Cyc_Parse_Abstractdeclarator _TCD6;struct Cyc_Yystacktype*_TCD7;union Cyc_YYSTYPE*_TCD8;union Cyc_YYSTYPE*_TCD9;struct Cyc_Yystacktype*_TCDA;union Cyc_YYSTYPE*_TCDB;union Cyc_YYSTYPE*_TCDC;struct Cyc_Parse_Abstractdeclarator _TCDD;struct Cyc_List_List*_TCDE;struct _RegionHandle*_TCDF;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCE0;struct _RegionHandle*_TCE1;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCE2;struct _RegionHandle*_TCE3;struct Cyc_Yystacktype*_TCE4;union Cyc_YYSTYPE*_TCE5;union Cyc_YYSTYPE*_TCE6;struct Cyc_Yystacktype*_TCE7;union Cyc_YYSTYPE*_TCE8;union Cyc_YYSTYPE*_TCE9;struct Cyc_Parse_Abstractdeclarator _TCEA;struct Cyc_List_List*_TCEB;struct _RegionHandle*_TCEC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCED;struct _RegionHandle*_TCEE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCEF;struct _RegionHandle*_TCF0;struct Cyc_Yystacktype*_TCF1;union Cyc_YYSTYPE*_TCF2;union Cyc_YYSTYPE*_TCF3;struct Cyc_Parse_Abstractdeclarator _TCF4;struct Cyc_List_List*(*_TCF5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_TCF6)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_TCF7;struct Cyc_Yystacktype _TCF8;struct Cyc_Yyltype _TCF9;unsigned _TCFA;unsigned _TCFB;struct Cyc_Yystacktype*_TCFC;union Cyc_YYSTYPE*_TCFD;union Cyc_YYSTYPE*_TCFE;struct Cyc_List_List*_TCFF;struct Cyc_List_List*_TD00;struct Cyc_Parse_Abstractdeclarator _TD01;struct Cyc_List_List*_TD02;struct _RegionHandle*_TD03;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_TD04;struct _RegionHandle*_TD05;struct Cyc_Yystacktype*_TD06;struct Cyc_Yystacktype _TD07;struct Cyc_Yyltype _TD08;unsigned _TD09;struct Cyc_Yystacktype*_TD0A;union Cyc_YYSTYPE*_TD0B;union Cyc_YYSTYPE*_TD0C;struct Cyc_Parse_Abstractdeclarator _TD0D;struct Cyc_Parse_Abstractdeclarator _TD0E;struct Cyc_List_List*_TD0F;struct _RegionHandle*_TD10;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_TD11;struct _RegionHandle*_TD12;struct Cyc_Yystacktype*_TD13;struct Cyc_Yystacktype _TD14;struct Cyc_Yyltype _TD15;unsigned _TD16;struct Cyc_Yystacktype*_TD17;union Cyc_YYSTYPE*_TD18;union Cyc_YYSTYPE*_TD19;struct Cyc_Yystacktype*_TD1A;union Cyc_YYSTYPE*_TD1B;union Cyc_YYSTYPE*_TD1C;struct Cyc_Parse_Abstractdeclarator _TD1D;struct _tuple21 _TD1E;struct Cyc_Yystacktype*_TD1F;union Cyc_YYSTYPE*_TD20;union Cyc_YYSTYPE*_TD21;struct _tuple21 _TD22;struct Cyc_Yystacktype*_TD23;union Cyc_YYSTYPE*_TD24;union Cyc_YYSTYPE*_TD25;struct _tuple21 _TD26;struct Cyc_Yystacktype*_TD27;union Cyc_YYSTYPE*_TD28;union Cyc_YYSTYPE*_TD29;struct _tuple21 _TD2A;struct Cyc_Yystacktype*_TD2B;union Cyc_YYSTYPE*_TD2C;union Cyc_YYSTYPE*_TD2D;struct _tuple21 _TD2E;struct Cyc_Yystacktype*_TD2F;union Cyc_YYSTYPE*_TD30;union Cyc_YYSTYPE*_TD31;struct Cyc_Yystacktype*_TD32;union Cyc_YYSTYPE*_TD33;union Cyc_YYSTYPE*_TD34;struct _tuple21 _TD35;struct _tuple21 _TD36;struct _tuple21 _TD37;struct Cyc_Yystacktype*_TD38;union Cyc_YYSTYPE*_TD39;union Cyc_YYSTYPE*_TD3A;struct Cyc_Yystacktype*_TD3B;union Cyc_YYSTYPE*_TD3C;union Cyc_YYSTYPE*_TD3D;struct _tuple21 _TD3E;struct _tuple21 _TD3F;struct _tuple21 _TD40;struct Cyc_Yystacktype*_TD41;union Cyc_YYSTYPE*_TD42;union Cyc_YYSTYPE*_TD43;struct Cyc_Yystacktype*_TD44;union Cyc_YYSTYPE*_TD45;union Cyc_YYSTYPE*_TD46;struct _tuple21 _TD47;struct _tuple21 _TD48;struct _tuple21 _TD49;struct Cyc_Yystacktype*_TD4A;union Cyc_YYSTYPE*_TD4B;union Cyc_YYSTYPE*_TD4C;struct Cyc_Yystacktype*_TD4D;union Cyc_YYSTYPE*_TD4E;union Cyc_YYSTYPE*_TD4F;struct _tuple21 _TD50;struct _tuple21 _TD51;struct _tuple21 _TD52;struct Cyc_Yystacktype*_TD53;struct Cyc_Yystacktype _TD54;struct Cyc_Yystacktype*_TD55;struct Cyc_Yystacktype _TD56;struct Cyc_Yystacktype*_TD57;struct Cyc_Yystacktype _TD58;struct Cyc_Yystacktype*_TD59;struct Cyc_Yystacktype _TD5A;struct Cyc_Yystacktype*_TD5B;struct Cyc_Yystacktype _TD5C;struct Cyc_Yystacktype*_TD5D;struct Cyc_Yystacktype _TD5E;struct Cyc_Yystacktype*_TD5F;struct Cyc_Yystacktype _TD60;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD61;struct _fat_ptr*_TD62;struct Cyc_Yystacktype*_TD63;union Cyc_YYSTYPE*_TD64;union Cyc_YYSTYPE*_TD65;struct Cyc_Yystacktype*_TD66;union Cyc_YYSTYPE*_TD67;union Cyc_YYSTYPE*_TD68;void*_TD69;struct Cyc_Yystacktype*_TD6A;struct Cyc_Yystacktype _TD6B;struct Cyc_Yyltype _TD6C;unsigned _TD6D;unsigned _TD6E;struct Cyc_Absyn_Stmt*_TD6F;struct Cyc_Yystacktype*_TD70;struct Cyc_Yystacktype _TD71;struct Cyc_Yyltype _TD72;unsigned _TD73;unsigned _TD74;struct Cyc_Absyn_Stmt*_TD75;struct Cyc_Yystacktype*_TD76;union Cyc_YYSTYPE*_TD77;union Cyc_YYSTYPE*_TD78;struct Cyc_Absyn_Exp*_TD79;struct Cyc_Yystacktype*_TD7A;struct Cyc_Yystacktype _TD7B;struct Cyc_Yyltype _TD7C;unsigned _TD7D;unsigned _TD7E;struct Cyc_Absyn_Stmt*_TD7F;struct Cyc_Yystacktype*_TD80;struct Cyc_Yystacktype _TD81;struct Cyc_Yyltype _TD82;unsigned _TD83;unsigned _TD84;struct Cyc_Absyn_Stmt*_TD85;struct Cyc_Yystacktype*_TD86;struct Cyc_Yystacktype _TD87;struct Cyc_Yystacktype*_TD88;struct Cyc_Yystacktype _TD89;struct Cyc_Yyltype _TD8A;unsigned _TD8B;unsigned _TD8C;struct Cyc_Absyn_Stmt*_TD8D;struct Cyc_Yystacktype*_TD8E;struct Cyc_Yystacktype _TD8F;struct Cyc_Yystacktype*_TD90;union Cyc_YYSTYPE*_TD91;union Cyc_YYSTYPE*_TD92;struct Cyc_List_List*_TD93;struct Cyc_Yystacktype*_TD94;struct Cyc_Yystacktype _TD95;struct Cyc_Yyltype _TD96;unsigned _TD97;unsigned _TD98;struct Cyc_Absyn_Stmt*_TD99;struct Cyc_Absyn_Stmt*_TD9A;struct Cyc_Yystacktype*_TD9B;union Cyc_YYSTYPE*_TD9C;union Cyc_YYSTYPE*_TD9D;struct Cyc_List_List*_TD9E;struct Cyc_Yystacktype*_TD9F;union Cyc_YYSTYPE*_TDA0;union Cyc_YYSTYPE*_TDA1;struct Cyc_Absyn_Stmt*_TDA2;struct Cyc_Absyn_Stmt*_TDA3;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TDA4;struct _fat_ptr*_TDA5;struct Cyc_Yystacktype*_TDA6;union Cyc_YYSTYPE*_TDA7;union Cyc_YYSTYPE*_TDA8;struct Cyc_Yystacktype*_TDA9;union Cyc_YYSTYPE*_TDAA;union Cyc_YYSTYPE*_TDAB;struct Cyc_List_List*_TDAC;struct Cyc_Absyn_Stmt*_TDAD;void*_TDAE;struct Cyc_Yystacktype*_TDAF;struct Cyc_Yystacktype _TDB0;struct Cyc_Yyltype _TDB1;unsigned _TDB2;unsigned _TDB3;struct Cyc_Absyn_Stmt*_TDB4;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TDB5;struct _fat_ptr*_TDB6;struct Cyc_Yystacktype*_TDB7;union Cyc_YYSTYPE*_TDB8;union Cyc_YYSTYPE*_TDB9;struct Cyc_Yystacktype*_TDBA;union Cyc_YYSTYPE*_TDBB;union Cyc_YYSTYPE*_TDBC;struct Cyc_List_List*_TDBD;struct Cyc_Yystacktype*_TDBE;union Cyc_YYSTYPE*_TDBF;union Cyc_YYSTYPE*_TDC0;struct Cyc_Absyn_Stmt*_TDC1;void*_TDC2;struct Cyc_Yystacktype*_TDC3;struct Cyc_Yystacktype _TDC4;struct Cyc_Yyltype _TDC5;unsigned _TDC6;unsigned _TDC7;struct Cyc_Absyn_Stmt*_TDC8;struct Cyc_Yystacktype*_TDC9;struct Cyc_Yystacktype _TDCA;struct Cyc_Yystacktype*_TDCB;union Cyc_YYSTYPE*_TDCC;union Cyc_YYSTYPE*_TDCD;struct Cyc_Absyn_Stmt*_TDCE;struct Cyc_Yystacktype*_TDCF;union Cyc_YYSTYPE*_TDD0;union Cyc_YYSTYPE*_TDD1;struct Cyc_Absyn_Stmt*_TDD2;struct Cyc_Yystacktype*_TDD3;struct Cyc_Yystacktype _TDD4;struct Cyc_Yyltype _TDD5;unsigned _TDD6;unsigned _TDD7;struct Cyc_Absyn_Stmt*_TDD8;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDD9;struct Cyc_Yystacktype*_TDDA;union Cyc_YYSTYPE*_TDDB;union Cyc_YYSTYPE*_TDDC;void*_TDDD;struct Cyc_Yystacktype*_TDDE;struct Cyc_Yystacktype _TDDF;struct Cyc_Yyltype _TDE0;unsigned _TDE1;unsigned _TDE2;struct Cyc_Absyn_Decl*_TDE3;struct Cyc_Absyn_Stmt*_TDE4;struct Cyc_Absyn_Stmt*_TDE5;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDE6;struct Cyc_Yystacktype*_TDE7;union Cyc_YYSTYPE*_TDE8;union Cyc_YYSTYPE*_TDE9;void*_TDEA;struct Cyc_Yystacktype*_TDEB;struct Cyc_Yystacktype _TDEC;struct Cyc_Yyltype _TDED;unsigned _TDEE;unsigned _TDEF;struct Cyc_Absyn_Decl*_TDF0;struct Cyc_Yystacktype*_TDF1;union Cyc_YYSTYPE*_TDF2;union Cyc_YYSTYPE*_TDF3;struct Cyc_Absyn_Stmt*_TDF4;struct Cyc_Absyn_Stmt*_TDF5;struct Cyc_Yystacktype*_TDF6;union Cyc_YYSTYPE*_TDF7;union Cyc_YYSTYPE*_TDF8;struct Cyc_Absyn_Exp*_TDF9;struct Cyc_Yystacktype*_TDFA;union Cyc_YYSTYPE*_TDFB;union Cyc_YYSTYPE*_TDFC;struct Cyc_Absyn_Stmt*_TDFD;struct Cyc_Absyn_Stmt*_TDFE;struct Cyc_Yystacktype*_TDFF;struct Cyc_Yystacktype _TE00;struct Cyc_Yyltype _TE01;unsigned _TE02;unsigned _TE03;struct Cyc_Absyn_Stmt*_TE04;struct Cyc_Yystacktype*_TE05;union Cyc_YYSTYPE*_TE06;union Cyc_YYSTYPE*_TE07;struct Cyc_Absyn_Exp*_TE08;struct Cyc_Yystacktype*_TE09;union Cyc_YYSTYPE*_TE0A;union Cyc_YYSTYPE*_TE0B;struct Cyc_Absyn_Stmt*_TE0C;struct Cyc_Yystacktype*_TE0D;union Cyc_YYSTYPE*_TE0E;union Cyc_YYSTYPE*_TE0F;struct Cyc_Absyn_Stmt*_TE10;struct Cyc_Yystacktype*_TE11;struct Cyc_Yystacktype _TE12;struct Cyc_Yyltype _TE13;unsigned _TE14;unsigned _TE15;struct Cyc_Absyn_Stmt*_TE16;struct Cyc_Yystacktype*_TE17;union Cyc_YYSTYPE*_TE18;union Cyc_YYSTYPE*_TE19;struct Cyc_Absyn_Exp*_TE1A;struct Cyc_Yystacktype*_TE1B;union Cyc_YYSTYPE*_TE1C;union Cyc_YYSTYPE*_TE1D;struct Cyc_List_List*_TE1E;struct Cyc_Yystacktype*_TE1F;struct Cyc_Yystacktype _TE20;struct Cyc_Yyltype _TE21;unsigned _TE22;unsigned _TE23;struct Cyc_Absyn_Stmt*_TE24;struct Cyc_Yystacktype*_TE25;union Cyc_YYSTYPE*_TE26;union Cyc_YYSTYPE*_TE27;struct _tuple0*_TE28;struct Cyc_Yystacktype*_TE29;struct Cyc_Yystacktype _TE2A;struct Cyc_Yyltype _TE2B;unsigned _TE2C;unsigned _TE2D;struct Cyc_Absyn_Exp*_TE2E;struct Cyc_Yystacktype*_TE2F;union Cyc_YYSTYPE*_TE30;union Cyc_YYSTYPE*_TE31;struct Cyc_List_List*_TE32;struct Cyc_Yystacktype*_TE33;struct Cyc_Yystacktype _TE34;struct Cyc_Yyltype _TE35;unsigned _TE36;unsigned _TE37;struct Cyc_Absyn_Stmt*_TE38;struct Cyc_Yystacktype*_TE39;union Cyc_YYSTYPE*_TE3A;union Cyc_YYSTYPE*_TE3B;struct Cyc_List_List*_TE3C;struct Cyc_Yystacktype*_TE3D;struct Cyc_Yystacktype _TE3E;struct Cyc_Yyltype _TE3F;unsigned _TE40;unsigned _TE41;struct Cyc_Absyn_Exp*_TE42;struct Cyc_Yystacktype*_TE43;union Cyc_YYSTYPE*_TE44;union Cyc_YYSTYPE*_TE45;struct Cyc_List_List*_TE46;struct Cyc_Yystacktype*_TE47;struct Cyc_Yystacktype _TE48;struct Cyc_Yyltype _TE49;unsigned _TE4A;unsigned _TE4B;struct Cyc_Absyn_Stmt*_TE4C;struct Cyc_Yystacktype*_TE4D;union Cyc_YYSTYPE*_TE4E;union Cyc_YYSTYPE*_TE4F;struct Cyc_Absyn_Stmt*_TE50;struct Cyc_Yystacktype*_TE51;union Cyc_YYSTYPE*_TE52;union Cyc_YYSTYPE*_TE53;struct Cyc_List_List*_TE54;struct Cyc_Yystacktype*_TE55;struct Cyc_Yystacktype _TE56;struct Cyc_Yyltype _TE57;unsigned _TE58;unsigned _TE59;struct Cyc_Absyn_Stmt*_TE5A;struct Cyc_List_List*_TE5B;struct Cyc_Absyn_Switch_clause*_TE5C;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE5D;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE5E;void*_TE5F;struct Cyc_Yystacktype*_TE60;struct Cyc_Yystacktype _TE61;struct Cyc_Yyltype _TE62;unsigned _TE63;unsigned _TE64;struct Cyc_Yystacktype*_TE65;union Cyc_YYSTYPE*_TE66;union Cyc_YYSTYPE*_TE67;struct Cyc_Yystacktype*_TE68;struct Cyc_Yystacktype _TE69;struct Cyc_Yyltype _TE6A;unsigned _TE6B;struct Cyc_Yystacktype*_TE6C;union Cyc_YYSTYPE*_TE6D;union Cyc_YYSTYPE*_TE6E;struct Cyc_List_List*_TE6F;struct Cyc_Absyn_Switch_clause*_TE70;struct Cyc_Yystacktype*_TE71;union Cyc_YYSTYPE*_TE72;union Cyc_YYSTYPE*_TE73;struct Cyc_Yystacktype*_TE74;struct Cyc_Yystacktype _TE75;struct Cyc_Yyltype _TE76;unsigned _TE77;unsigned _TE78;struct Cyc_Yystacktype*_TE79;struct Cyc_Yystacktype _TE7A;struct Cyc_Yyltype _TE7B;unsigned _TE7C;struct Cyc_Yystacktype*_TE7D;union Cyc_YYSTYPE*_TE7E;union Cyc_YYSTYPE*_TE7F;struct Cyc_List_List*_TE80;struct Cyc_Absyn_Switch_clause*_TE81;struct Cyc_Yystacktype*_TE82;union Cyc_YYSTYPE*_TE83;union Cyc_YYSTYPE*_TE84;struct Cyc_Yystacktype*_TE85;union Cyc_YYSTYPE*_TE86;union Cyc_YYSTYPE*_TE87;struct Cyc_Yystacktype*_TE88;struct Cyc_Yystacktype _TE89;struct Cyc_Yyltype _TE8A;unsigned _TE8B;struct Cyc_Yystacktype*_TE8C;union Cyc_YYSTYPE*_TE8D;union Cyc_YYSTYPE*_TE8E;struct Cyc_List_List*_TE8F;struct Cyc_Absyn_Switch_clause*_TE90;struct Cyc_Yystacktype*_TE91;union Cyc_YYSTYPE*_TE92;union Cyc_YYSTYPE*_TE93;struct Cyc_Yystacktype*_TE94;union Cyc_YYSTYPE*_TE95;union Cyc_YYSTYPE*_TE96;struct Cyc_Yystacktype*_TE97;struct Cyc_Yystacktype _TE98;struct Cyc_Yyltype _TE99;unsigned _TE9A;unsigned _TE9B;struct Cyc_Yystacktype*_TE9C;struct Cyc_Yystacktype _TE9D;struct Cyc_Yyltype _TE9E;unsigned _TE9F;struct Cyc_Yystacktype*_TEA0;union Cyc_YYSTYPE*_TEA1;union Cyc_YYSTYPE*_TEA2;struct Cyc_List_List*_TEA3;struct Cyc_Absyn_Switch_clause*_TEA4;struct Cyc_Yystacktype*_TEA5;union Cyc_YYSTYPE*_TEA6;union Cyc_YYSTYPE*_TEA7;struct Cyc_Yystacktype*_TEA8;union Cyc_YYSTYPE*_TEA9;union Cyc_YYSTYPE*_TEAA;struct Cyc_Yystacktype*_TEAB;union Cyc_YYSTYPE*_TEAC;union Cyc_YYSTYPE*_TEAD;struct Cyc_Yystacktype*_TEAE;struct Cyc_Yystacktype _TEAF;struct Cyc_Yyltype _TEB0;unsigned _TEB1;struct Cyc_Yystacktype*_TEB2;union Cyc_YYSTYPE*_TEB3;union Cyc_YYSTYPE*_TEB4;struct Cyc_Yystacktype*_TEB5;union Cyc_YYSTYPE*_TEB6;union Cyc_YYSTYPE*_TEB7;struct Cyc_Absyn_Exp*_TEB8;struct Cyc_Yystacktype*_TEB9;union Cyc_YYSTYPE*_TEBA;union Cyc_YYSTYPE*_TEBB;struct Cyc_Absyn_Stmt*_TEBC;struct Cyc_Yystacktype*_TEBD;struct Cyc_Yystacktype _TEBE;struct Cyc_Yyltype _TEBF;unsigned _TEC0;unsigned _TEC1;struct Cyc_Absyn_Stmt*_TEC2;struct Cyc_Yystacktype*_TEC3;union Cyc_YYSTYPE*_TEC4;union Cyc_YYSTYPE*_TEC5;struct Cyc_Absyn_Stmt*_TEC6;struct Cyc_Yystacktype*_TEC7;union Cyc_YYSTYPE*_TEC8;union Cyc_YYSTYPE*_TEC9;struct Cyc_Absyn_Exp*_TECA;struct Cyc_Yystacktype*_TECB;struct Cyc_Yystacktype _TECC;struct Cyc_Yyltype _TECD;unsigned _TECE;unsigned _TECF;struct Cyc_Absyn_Stmt*_TED0;struct Cyc_Yystacktype*_TED1;union Cyc_YYSTYPE*_TED2;union Cyc_YYSTYPE*_TED3;struct Cyc_Absyn_Exp*_TED4;struct Cyc_Yystacktype*_TED5;union Cyc_YYSTYPE*_TED6;union Cyc_YYSTYPE*_TED7;struct Cyc_Absyn_Exp*_TED8;struct Cyc_Yystacktype*_TED9;union Cyc_YYSTYPE*_TEDA;union Cyc_YYSTYPE*_TEDB;struct Cyc_Absyn_Exp*_TEDC;struct Cyc_Yystacktype*_TEDD;union Cyc_YYSTYPE*_TEDE;union Cyc_YYSTYPE*_TEDF;struct Cyc_Absyn_Stmt*_TEE0;struct Cyc_Yystacktype*_TEE1;struct Cyc_Yystacktype _TEE2;struct Cyc_Yyltype _TEE3;unsigned _TEE4;unsigned _TEE5;struct Cyc_Absyn_Stmt*_TEE6;struct Cyc_Absyn_Exp*_TEE7;struct Cyc_Yystacktype*_TEE8;union Cyc_YYSTYPE*_TEE9;union Cyc_YYSTYPE*_TEEA;struct Cyc_Absyn_Exp*_TEEB;struct Cyc_Yystacktype*_TEEC;union Cyc_YYSTYPE*_TEED;union Cyc_YYSTYPE*_TEEE;struct Cyc_Absyn_Exp*_TEEF;struct Cyc_Yystacktype*_TEF0;union Cyc_YYSTYPE*_TEF1;union Cyc_YYSTYPE*_TEF2;struct Cyc_Absyn_Stmt*_TEF3;struct Cyc_Yystacktype*_TEF4;struct Cyc_Yystacktype _TEF5;struct Cyc_Yyltype _TEF6;unsigned _TEF7;unsigned _TEF8;struct Cyc_Yystacktype*_TEF9;union Cyc_YYSTYPE*_TEFA;union Cyc_YYSTYPE*_TEFB;struct Cyc_List_List*_TEFC;struct Cyc_Absyn_Stmt*_TEFD;struct Cyc_Absyn_Stmt*_TEFE;struct Cyc_Absyn_Exp*_TEFF;struct Cyc_Yystacktype*_TF00;struct Cyc_Yystacktype _TF01;struct _fat_ptr*_TF02;struct Cyc_Yystacktype*_TF03;union Cyc_YYSTYPE*_TF04;union Cyc_YYSTYPE*_TF05;struct Cyc_Yystacktype*_TF06;struct Cyc_Yystacktype _TF07;struct Cyc_Yyltype _TF08;unsigned _TF09;unsigned _TF0A;struct Cyc_Absyn_Stmt*_TF0B;struct Cyc_Yystacktype*_TF0C;struct Cyc_Yystacktype _TF0D;struct Cyc_Yyltype _TF0E;unsigned _TF0F;unsigned _TF10;struct Cyc_Absyn_Stmt*_TF11;struct Cyc_Yystacktype*_TF12;struct Cyc_Yystacktype _TF13;struct Cyc_Yyltype _TF14;unsigned _TF15;unsigned _TF16;struct Cyc_Absyn_Stmt*_TF17;struct Cyc_Yystacktype*_TF18;struct Cyc_Yystacktype _TF19;struct Cyc_Yyltype _TF1A;unsigned _TF1B;unsigned _TF1C;struct Cyc_Absyn_Stmt*_TF1D;struct Cyc_Yystacktype*_TF1E;union Cyc_YYSTYPE*_TF1F;union Cyc_YYSTYPE*_TF20;struct Cyc_Absyn_Exp*_TF21;struct Cyc_Yystacktype*_TF22;struct Cyc_Yystacktype _TF23;struct Cyc_Yyltype _TF24;unsigned _TF25;unsigned _TF26;struct Cyc_Absyn_Stmt*_TF27;struct Cyc_Yystacktype*_TF28;struct Cyc_Yystacktype _TF29;struct Cyc_Yyltype _TF2A;unsigned _TF2B;unsigned _TF2C;struct Cyc_Absyn_Stmt*_TF2D;struct Cyc_Yystacktype*_TF2E;struct Cyc_Yystacktype _TF2F;struct Cyc_Yyltype _TF30;unsigned _TF31;unsigned _TF32;struct Cyc_Absyn_Stmt*_TF33;struct Cyc_Yystacktype*_TF34;union Cyc_YYSTYPE*_TF35;union Cyc_YYSTYPE*_TF36;struct Cyc_List_List*_TF37;struct Cyc_Yystacktype*_TF38;struct Cyc_Yystacktype _TF39;struct Cyc_Yyltype _TF3A;unsigned _TF3B;unsigned _TF3C;struct Cyc_Absyn_Stmt*_TF3D;struct Cyc_Yystacktype*_TF3E;struct Cyc_Yystacktype _TF3F;struct Cyc_Yystacktype*_TF40;struct Cyc_Yystacktype _TF41;struct Cyc_Yystacktype*_TF42;union Cyc_YYSTYPE*_TF43;union Cyc_YYSTYPE*_TF44;struct Cyc_Absyn_Pat*_TF45;struct Cyc_Absyn_Exp*_TF46;struct Cyc_Yystacktype*_TF47;union Cyc_YYSTYPE*_TF48;union Cyc_YYSTYPE*_TF49;struct Cyc_Absyn_Exp*_TF4A;struct Cyc_Yystacktype*_TF4B;union Cyc_YYSTYPE*_TF4C;union Cyc_YYSTYPE*_TF4D;struct Cyc_Absyn_Exp*_TF4E;struct Cyc_Yystacktype*_TF4F;struct Cyc_Yystacktype _TF50;struct Cyc_Yyltype _TF51;unsigned _TF52;unsigned _TF53;struct Cyc_Absyn_Exp*_TF54;struct Cyc_Absyn_Pat*_TF55;struct Cyc_Yystacktype*_TF56;struct Cyc_Yystacktype _TF57;struct Cyc_Yystacktype*_TF58;union Cyc_YYSTYPE*_TF59;union Cyc_YYSTYPE*_TF5A;struct Cyc_Absyn_Pat*_TF5B;struct Cyc_Absyn_Exp*_TF5C;struct Cyc_Yystacktype*_TF5D;union Cyc_YYSTYPE*_TF5E;union Cyc_YYSTYPE*_TF5F;struct Cyc_Absyn_Exp*_TF60;struct Cyc_Yystacktype*_TF61;struct Cyc_Yystacktype _TF62;struct Cyc_Yyltype _TF63;unsigned _TF64;unsigned _TF65;struct Cyc_Absyn_Exp*_TF66;struct Cyc_Absyn_Pat*_TF67;struct Cyc_Yystacktype*_TF68;struct Cyc_Yystacktype _TF69;struct Cyc_Yystacktype*_TF6A;union Cyc_YYSTYPE*_TF6B;union Cyc_YYSTYPE*_TF6C;struct Cyc_Absyn_Pat*_TF6D;struct Cyc_Absyn_Exp*_TF6E;struct Cyc_Yystacktype*_TF6F;union Cyc_YYSTYPE*_TF70;union Cyc_YYSTYPE*_TF71;struct Cyc_Absyn_Exp*_TF72;struct Cyc_Yystacktype*_TF73;struct Cyc_Yystacktype _TF74;struct Cyc_Yyltype _TF75;unsigned _TF76;unsigned _TF77;struct Cyc_Absyn_Exp*_TF78;struct Cyc_Absyn_Pat*_TF79;struct Cyc_Yystacktype*_TF7A;struct Cyc_Yystacktype _TF7B;struct Cyc_Yystacktype*_TF7C;union Cyc_YYSTYPE*_TF7D;union Cyc_YYSTYPE*_TF7E;struct Cyc_Absyn_Pat*_TF7F;struct Cyc_Absyn_Exp*_TF80;struct Cyc_Yystacktype*_TF81;union Cyc_YYSTYPE*_TF82;union Cyc_YYSTYPE*_TF83;struct Cyc_Absyn_Exp*_TF84;struct Cyc_Yystacktype*_TF85;struct Cyc_Yystacktype _TF86;struct Cyc_Yyltype _TF87;unsigned _TF88;unsigned _TF89;struct Cyc_Absyn_Exp*_TF8A;struct Cyc_Absyn_Pat*_TF8B;struct Cyc_Yystacktype*_TF8C;struct Cyc_Yystacktype _TF8D;struct Cyc_Yystacktype*_TF8E;union Cyc_YYSTYPE*_TF8F;union Cyc_YYSTYPE*_TF90;struct Cyc_Absyn_Pat*_TF91;struct Cyc_Absyn_Exp*_TF92;struct Cyc_Yystacktype*_TF93;union Cyc_YYSTYPE*_TF94;union Cyc_YYSTYPE*_TF95;struct Cyc_Absyn_Exp*_TF96;struct Cyc_Yystacktype*_TF97;struct Cyc_Yystacktype _TF98;struct Cyc_Yyltype _TF99;unsigned _TF9A;unsigned _TF9B;struct Cyc_Absyn_Exp*_TF9C;struct Cyc_Absyn_Pat*_TF9D;struct Cyc_Yystacktype*_TF9E;struct Cyc_Yystacktype _TF9F;struct Cyc_Yystacktype*_TFA0;union Cyc_YYSTYPE*_TFA1;union Cyc_YYSTYPE*_TFA2;struct Cyc_Absyn_Pat*_TFA3;struct Cyc_Absyn_Exp*_TFA4;struct Cyc_Yystacktype*_TFA5;union Cyc_YYSTYPE*_TFA6;union Cyc_YYSTYPE*_TFA7;struct Cyc_Absyn_Exp*_TFA8;struct Cyc_Yystacktype*_TFA9;struct Cyc_Yystacktype _TFAA;struct Cyc_Yyltype _TFAB;unsigned _TFAC;unsigned _TFAD;struct Cyc_Absyn_Exp*_TFAE;struct Cyc_Absyn_Pat*_TFAF;struct Cyc_Yystacktype*_TFB0;struct Cyc_Yystacktype _TFB1;struct Cyc_Yystacktype*_TFB2;union Cyc_YYSTYPE*_TFB3;union Cyc_YYSTYPE*_TFB4;struct Cyc_Absyn_Exp*(*_TFB5)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TFB6;union Cyc_YYSTYPE*_TFB7;union Cyc_YYSTYPE*_TFB8;struct Cyc_Absyn_Pat*_TFB9;struct Cyc_Absyn_Exp*_TFBA;struct Cyc_Yystacktype*_TFBB;union Cyc_YYSTYPE*_TFBC;union Cyc_YYSTYPE*_TFBD;struct Cyc_Absyn_Exp*_TFBE;struct Cyc_Yystacktype*_TFBF;struct Cyc_Yystacktype _TFC0;struct Cyc_Yyltype _TFC1;unsigned _TFC2;unsigned _TFC3;struct Cyc_Absyn_Exp*_TFC4;struct Cyc_Absyn_Pat*_TFC5;struct Cyc_Yystacktype*_TFC6;struct Cyc_Yystacktype _TFC7;struct Cyc_Yystacktype*_TFC8;union Cyc_YYSTYPE*_TFC9;union Cyc_YYSTYPE*_TFCA;struct Cyc_Absyn_Exp*(*_TFCB)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TFCC;union Cyc_YYSTYPE*_TFCD;union Cyc_YYSTYPE*_TFCE;struct Cyc_Absyn_Pat*_TFCF;struct Cyc_Absyn_Exp*_TFD0;struct Cyc_Yystacktype*_TFD1;union Cyc_YYSTYPE*_TFD2;union Cyc_YYSTYPE*_TFD3;struct Cyc_Absyn_Exp*_TFD4;struct Cyc_Yystacktype*_TFD5;struct Cyc_Yystacktype _TFD6;struct Cyc_Yyltype _TFD7;unsigned _TFD8;unsigned _TFD9;struct Cyc_Absyn_Exp*_TFDA;struct Cyc_Absyn_Pat*_TFDB;struct Cyc_Yystacktype*_TFDC;struct Cyc_Yystacktype _TFDD;struct Cyc_Yystacktype*_TFDE;union Cyc_YYSTYPE*_TFDF;union Cyc_YYSTYPE*_TFE0;struct Cyc_Absyn_Pat*_TFE1;struct Cyc_Absyn_Exp*_TFE2;struct Cyc_Yystacktype*_TFE3;union Cyc_YYSTYPE*_TFE4;union Cyc_YYSTYPE*_TFE5;struct Cyc_Absyn_Exp*_TFE6;struct Cyc_Yystacktype*_TFE7;struct Cyc_Yystacktype _TFE8;struct Cyc_Yyltype _TFE9;unsigned _TFEA;unsigned _TFEB;struct Cyc_Absyn_Exp*_TFEC;struct Cyc_Absyn_Pat*_TFED;struct Cyc_Yystacktype*_TFEE;union Cyc_YYSTYPE*_TFEF;union Cyc_YYSTYPE*_TFF0;struct Cyc_Absyn_Pat*_TFF1;struct Cyc_Absyn_Exp*_TFF2;struct Cyc_Yystacktype*_TFF3;union Cyc_YYSTYPE*_TFF4;union Cyc_YYSTYPE*_TFF5;struct Cyc_Absyn_Exp*_TFF6;struct Cyc_Yystacktype*_TFF7;struct Cyc_Yystacktype _TFF8;struct Cyc_Yyltype _TFF9;unsigned _TFFA;unsigned _TFFB;struct Cyc_Absyn_Exp*_TFFC;struct Cyc_Absyn_Pat*_TFFD;struct Cyc_Yystacktype*_TFFE;struct Cyc_Yystacktype _TFFF;struct Cyc_Yystacktype*_T1000;union Cyc_YYSTYPE*_T1001;union Cyc_YYSTYPE*_T1002;enum Cyc_Absyn_Primop _T1003;struct Cyc_Yystacktype*_T1004;union Cyc_YYSTYPE*_T1005;union Cyc_YYSTYPE*_T1006;struct Cyc_Absyn_Pat*_T1007;struct Cyc_Absyn_Exp*_T1008;struct Cyc_Yystacktype*_T1009;union Cyc_YYSTYPE*_T100A;union Cyc_YYSTYPE*_T100B;struct Cyc_Absyn_Exp*_T100C;struct Cyc_Yystacktype*_T100D;struct Cyc_Yystacktype _T100E;struct Cyc_Yyltype _T100F;unsigned _T1010;unsigned _T1011;struct Cyc_Absyn_Exp*_T1012;struct Cyc_Absyn_Pat*_T1013;struct Cyc_Yystacktype*_T1014;struct Cyc_Yystacktype _T1015;struct Cyc_Yystacktype*_T1016;union Cyc_YYSTYPE*_T1017;union Cyc_YYSTYPE*_T1018;enum Cyc_Absyn_Primop _T1019;struct Cyc_Yystacktype*_T101A;union Cyc_YYSTYPE*_T101B;union Cyc_YYSTYPE*_T101C;struct Cyc_Absyn_Pat*_T101D;struct Cyc_Absyn_Exp*_T101E;struct Cyc_Yystacktype*_T101F;union Cyc_YYSTYPE*_T1020;union Cyc_YYSTYPE*_T1021;struct Cyc_Absyn_Exp*_T1022;struct Cyc_Yystacktype*_T1023;struct Cyc_Yystacktype _T1024;struct Cyc_Yyltype _T1025;unsigned _T1026;unsigned _T1027;struct Cyc_Absyn_Exp*_T1028;struct Cyc_Absyn_Pat*_T1029;struct Cyc_Yystacktype*_T102A;struct Cyc_Yystacktype _T102B;struct Cyc_Yystacktype*_T102C;union Cyc_YYSTYPE*_T102D;union Cyc_YYSTYPE*_T102E;struct _tuple8*_T102F;struct Cyc_Yystacktype*_T1030;struct Cyc_Yystacktype _T1031;struct Cyc_Yyltype _T1032;unsigned _T1033;unsigned _T1034;void*_T1035;struct Cyc_Yystacktype*_T1036;union Cyc_YYSTYPE*_T1037;union Cyc_YYSTYPE*_T1038;struct Cyc_Absyn_Exp*_T1039;struct Cyc_Yystacktype*_T103A;struct Cyc_Yystacktype _T103B;struct Cyc_Yyltype _T103C;unsigned _T103D;unsigned _T103E;struct Cyc_Absyn_Exp*_T103F;struct Cyc_Absyn_Pat*_T1040;struct Cyc_Yystacktype*_T1041;struct Cyc_Yystacktype _T1042;struct Cyc_Yystacktype*_T1043;union Cyc_YYSTYPE*_T1044;union Cyc_YYSTYPE*_T1045;enum Cyc_Absyn_Primop _T1046;struct Cyc_Yystacktype*_T1047;union Cyc_YYSTYPE*_T1048;union Cyc_YYSTYPE*_T1049;struct Cyc_Absyn_Exp*_T104A;struct Cyc_Yystacktype*_T104B;struct Cyc_Yystacktype _T104C;struct Cyc_Yyltype _T104D;unsigned _T104E;unsigned _T104F;struct Cyc_Absyn_Exp*_T1050;struct Cyc_Absyn_Pat*_T1051;struct Cyc_Yystacktype*_T1052;union Cyc_YYSTYPE*_T1053;union Cyc_YYSTYPE*_T1054;struct _tuple8*_T1055;struct Cyc_Yystacktype*_T1056;struct Cyc_Yystacktype _T1057;struct Cyc_Yyltype _T1058;unsigned _T1059;unsigned _T105A;void*_T105B;struct Cyc_Yystacktype*_T105C;struct Cyc_Yystacktype _T105D;struct Cyc_Yyltype _T105E;unsigned _T105F;unsigned _T1060;struct Cyc_Absyn_Exp*_T1061;struct Cyc_Absyn_Pat*_T1062;struct Cyc_Yystacktype*_T1063;union Cyc_YYSTYPE*_T1064;union Cyc_YYSTYPE*_T1065;struct Cyc_Absyn_Exp*_T1066;struct Cyc_Yystacktype*_T1067;struct Cyc_Yystacktype _T1068;struct Cyc_Yyltype _T1069;unsigned _T106A;unsigned _T106B;struct Cyc_Absyn_Exp*_T106C;struct Cyc_Absyn_Pat*_T106D;struct Cyc_Yystacktype*_T106E;union Cyc_YYSTYPE*_T106F;union Cyc_YYSTYPE*_T1070;struct _tuple8*_T1071;struct _tuple8 _T1072;void*_T1073;struct Cyc_Yystacktype*_T1074;union Cyc_YYSTYPE*_T1075;union Cyc_YYSTYPE*_T1076;struct Cyc_List_List*_T1077;struct Cyc_List_List*_T1078;struct Cyc_Yystacktype*_T1079;struct Cyc_Yystacktype _T107A;struct Cyc_Yyltype _T107B;unsigned _T107C;unsigned _T107D;struct Cyc_Absyn_Exp*_T107E;struct Cyc_Absyn_Pat*_T107F;struct Cyc_Yystacktype*_T1080;struct Cyc_Yystacktype _T1081;struct Cyc_Yystacktype*_T1082;struct Cyc_Yystacktype _T1083;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1084;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1085;void*_T1086;struct Cyc_Yystacktype*_T1087;struct Cyc_Yystacktype _T1088;struct Cyc_Yyltype _T1089;unsigned _T108A;unsigned _T108B;struct Cyc_Absyn_Pat*_T108C;struct Cyc_Yystacktype*_T108D;union Cyc_YYSTYPE*_T108E;union Cyc_YYSTYPE*_T108F;struct Cyc_Absyn_Exp*_T1090;struct Cyc_Absyn_Pat*_T1091;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T1092;struct Cyc_Yystacktype*_T1093;union Cyc_YYSTYPE*_T1094;union Cyc_YYSTYPE*_T1095;void*_T1096;struct Cyc_Yystacktype*_T1097;struct Cyc_Yystacktype _T1098;struct Cyc_Yyltype _T1099;unsigned _T109A;unsigned _T109B;struct Cyc_Absyn_Pat*_T109C;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T109D;struct Cyc_Yystacktype*_T109E;union Cyc_YYSTYPE*_T109F;union Cyc_YYSTYPE*_T10A0;void*_T10A1;struct Cyc_Yystacktype*_T10A2;struct Cyc_Yystacktype _T10A3;struct Cyc_Yyltype _T10A4;unsigned _T10A5;unsigned _T10A6;struct Cyc_Absyn_Pat*_T10A7;struct Cyc_Yystacktype*_T10A8;union Cyc_YYSTYPE*_T10A9;union Cyc_YYSTYPE*_T10AA;struct Cyc_Absyn_Exp*_T10AB;int*_T10AC;int _T10AD;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T10AE;union Cyc_Absyn_Cnst _T10AF;struct _union_Cnst_LongLong_c _T10B0;unsigned _T10B1;union Cyc_Absyn_Cnst _T10B2;struct _union_Cnst_Char_c _T10B3;struct _tuple3 _T10B4;union Cyc_Absyn_Cnst _T10B5;struct _union_Cnst_Char_c _T10B6;struct _tuple3 _T10B7;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T10B8;void*_T10B9;struct Cyc_Absyn_Exp*_T10BA;unsigned _T10BB;struct Cyc_Absyn_Pat*_T10BC;union Cyc_Absyn_Cnst _T10BD;struct _union_Cnst_Short_c _T10BE;struct _tuple4 _T10BF;union Cyc_Absyn_Cnst _T10C0;struct _union_Cnst_Short_c _T10C1;struct _tuple4 _T10C2;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T10C3;short _T10C4;void*_T10C5;struct Cyc_Absyn_Exp*_T10C6;unsigned _T10C7;struct Cyc_Absyn_Pat*_T10C8;union Cyc_Absyn_Cnst _T10C9;struct _union_Cnst_Int_c _T10CA;struct _tuple5 _T10CB;union Cyc_Absyn_Cnst _T10CC;struct _union_Cnst_Int_c _T10CD;struct _tuple5 _T10CE;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T10CF;void*_T10D0;struct Cyc_Absyn_Exp*_T10D1;unsigned _T10D2;struct Cyc_Absyn_Pat*_T10D3;union Cyc_Absyn_Cnst _T10D4;struct _union_Cnst_Float_c _T10D5;struct _tuple7 _T10D6;union Cyc_Absyn_Cnst _T10D7;struct _union_Cnst_Float_c _T10D8;struct _tuple7 _T10D9;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T10DA;void*_T10DB;struct Cyc_Absyn_Exp*_T10DC;unsigned _T10DD;struct Cyc_Absyn_Pat*_T10DE;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10DF;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10E0;void*_T10E1;struct Cyc_Absyn_Exp*_T10E2;unsigned _T10E3;struct Cyc_Absyn_Pat*_T10E4;struct Cyc_Yystacktype*_T10E5;struct Cyc_Yystacktype _T10E6;struct Cyc_Yyltype _T10E7;unsigned _T10E8;unsigned _T10E9;struct _fat_ptr _T10EA;struct _fat_ptr _T10EB;struct Cyc_Yystacktype*_T10EC;struct Cyc_Yystacktype _T10ED;struct Cyc_Yyltype _T10EE;unsigned _T10EF;unsigned _T10F0;struct _fat_ptr _T10F1;struct _fat_ptr _T10F2;struct Cyc_Yystacktype*_T10F3;struct Cyc_Yystacktype _T10F4;struct Cyc_Yyltype _T10F5;unsigned _T10F6;unsigned _T10F7;struct _fat_ptr _T10F8;struct _fat_ptr _T10F9;struct Cyc_Yystacktype*_T10FA;union Cyc_YYSTYPE*_T10FB;union Cyc_YYSTYPE*_T10FC;struct _fat_ptr _T10FD;struct _fat_ptr _T10FE;int _T10FF;struct Cyc_Yystacktype*_T1100;struct Cyc_Yystacktype _T1101;struct Cyc_Yyltype _T1102;unsigned _T1103;unsigned _T1104;struct _fat_ptr _T1105;struct _fat_ptr _T1106;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T1107;struct Cyc_Yystacktype*_T1108;struct Cyc_Yystacktype _T1109;struct Cyc_Yyltype _T110A;unsigned _T110B;unsigned _T110C;struct _tuple0*_T110D;struct _fat_ptr*_T110E;struct Cyc_Yystacktype*_T110F;union Cyc_YYSTYPE*_T1110;union Cyc_YYSTYPE*_T1111;void*_T1112;struct Cyc_Yystacktype*_T1113;union Cyc_YYSTYPE*_T1114;union Cyc_YYSTYPE*_T1115;void*_T1116;struct Cyc_Yystacktype*_T1117;struct Cyc_Yystacktype _T1118;struct Cyc_Yyltype _T1119;unsigned _T111A;unsigned _T111B;struct Cyc_Absyn_Pat*_T111C;struct Cyc_Yystacktype*_T111D;union Cyc_YYSTYPE*_T111E;union Cyc_YYSTYPE*_T111F;struct _fat_ptr _T1120;struct _fat_ptr _T1121;int _T1122;struct Cyc_Yystacktype*_T1123;struct Cyc_Yystacktype _T1124;struct Cyc_Yyltype _T1125;unsigned _T1126;unsigned _T1127;struct _fat_ptr _T1128;struct _fat_ptr _T1129;struct Cyc_Yystacktype*_T112A;struct Cyc_Yystacktype _T112B;struct Cyc_Yyltype _T112C;unsigned _T112D;struct Cyc_Yystacktype*_T112E;union Cyc_YYSTYPE*_T112F;union Cyc_YYSTYPE*_T1130;struct _fat_ptr _T1131;unsigned _T1132;struct Cyc_Absyn_Tvar*_T1133;struct _fat_ptr*_T1134;struct Cyc_Yystacktype*_T1135;union Cyc_YYSTYPE*_T1136;union Cyc_YYSTYPE*_T1137;struct Cyc_Absyn_Tvar*_T1138;struct Cyc_Absyn_Tvar*_T1139;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T113A;struct Cyc_Absyn_Kind*_T113B;struct Cyc_Absyn_Tvar*_T113C;struct Cyc_Yystacktype*_T113D;struct Cyc_Yystacktype _T113E;struct Cyc_Yyltype _T113F;unsigned _T1140;unsigned _T1141;struct _tuple0*_T1142;struct _fat_ptr*_T1143;struct Cyc_Yystacktype*_T1144;union Cyc_YYSTYPE*_T1145;union Cyc_YYSTYPE*_T1146;struct Cyc_Yystacktype*_T1147;union Cyc_YYSTYPE*_T1148;union Cyc_YYSTYPE*_T1149;struct _tuple8*_T114A;struct Cyc_Yystacktype*_T114B;struct Cyc_Yystacktype _T114C;struct Cyc_Yyltype _T114D;unsigned _T114E;unsigned _T114F;void*_T1150;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1151;void*_T1152;unsigned _T1153;struct Cyc_Absyn_Pat*_T1154;struct Cyc_Yystacktype*_T1155;union Cyc_YYSTYPE*_T1156;union Cyc_YYSTYPE*_T1157;struct _fat_ptr _T1158;struct _fat_ptr _T1159;int _T115A;struct Cyc_Yystacktype*_T115B;struct Cyc_Yystacktype _T115C;struct Cyc_Yyltype _T115D;unsigned _T115E;unsigned _T115F;struct _fat_ptr _T1160;struct _fat_ptr _T1161;struct Cyc_Yystacktype*_T1162;struct Cyc_Yystacktype _T1163;struct Cyc_Yyltype _T1164;unsigned _T1165;struct Cyc_Yystacktype*_T1166;union Cyc_YYSTYPE*_T1167;union Cyc_YYSTYPE*_T1168;struct _fat_ptr _T1169;unsigned _T116A;struct Cyc_Absyn_Tvar*_T116B;struct _fat_ptr*_T116C;struct Cyc_Yystacktype*_T116D;union Cyc_YYSTYPE*_T116E;union Cyc_YYSTYPE*_T116F;struct Cyc_Absyn_Tvar*_T1170;struct Cyc_Absyn_Tvar*_T1171;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1172;struct Cyc_Absyn_Kind*_T1173;struct Cyc_Absyn_Tvar*_T1174;struct Cyc_Yystacktype*_T1175;struct Cyc_Yystacktype _T1176;struct Cyc_Yyltype _T1177;unsigned _T1178;unsigned _T1179;struct _tuple0*_T117A;struct _fat_ptr*_T117B;struct Cyc_Yystacktype*_T117C;union Cyc_YYSTYPE*_T117D;union Cyc_YYSTYPE*_T117E;struct Cyc_Yystacktype*_T117F;union Cyc_YYSTYPE*_T1180;union Cyc_YYSTYPE*_T1181;struct _tuple8*_T1182;struct Cyc_Yystacktype*_T1183;struct Cyc_Yystacktype _T1184;struct Cyc_Yyltype _T1185;unsigned _T1186;unsigned _T1187;void*_T1188;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1189;void*_T118A;unsigned _T118B;struct Cyc_Absyn_Pat*_T118C;struct Cyc_Yystacktype*_T118D;union Cyc_YYSTYPE*_T118E;union Cyc_YYSTYPE*_T118F;struct _tuple23*_T1190;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1191;void*_T1192;struct Cyc_Yystacktype*_T1193;struct Cyc_Yystacktype _T1194;struct Cyc_Yyltype _T1195;unsigned _T1196;unsigned _T1197;struct Cyc_Absyn_Pat*_T1198;struct Cyc_Yystacktype*_T1199;union Cyc_YYSTYPE*_T119A;union Cyc_YYSTYPE*_T119B;struct _tuple23*_T119C;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T119D;struct Cyc_Yystacktype*_T119E;union Cyc_YYSTYPE*_T119F;union Cyc_YYSTYPE*_T11A0;void*_T11A1;struct Cyc_Yystacktype*_T11A2;struct Cyc_Yystacktype _T11A3;struct Cyc_Yyltype _T11A4;unsigned _T11A5;unsigned _T11A6;struct Cyc_Absyn_Pat*_T11A7;struct Cyc_Yystacktype*_T11A8;union Cyc_YYSTYPE*_T11A9;union Cyc_YYSTYPE*_T11AA;struct _tuple23*_T11AB;struct Cyc_List_List*(*_T11AC)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T11AD)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T11AE;struct Cyc_Yystacktype _T11AF;struct Cyc_Yyltype _T11B0;unsigned _T11B1;unsigned _T11B2;struct Cyc_Yystacktype*_T11B3;union Cyc_YYSTYPE*_T11B4;union Cyc_YYSTYPE*_T11B5;struct Cyc_List_List*_T11B6;struct Cyc_Yystacktype*_T11B7;union Cyc_YYSTYPE*_T11B8;union Cyc_YYSTYPE*_T11B9;struct _tuple0*_T11BA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11BB;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11BC;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T11BD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11BE;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T11BF;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T11C0;void*_T11C1;struct Cyc_Yystacktype*_T11C2;struct Cyc_Yystacktype _T11C3;struct Cyc_Yyltype _T11C4;unsigned _T11C5;unsigned _T11C6;struct Cyc_Absyn_Pat*_T11C7;struct Cyc_Yystacktype*_T11C8;union Cyc_YYSTYPE*_T11C9;union Cyc_YYSTYPE*_T11CA;struct _tuple23*_T11CB;struct Cyc_List_List*(*_T11CC)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T11CD)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T11CE;struct Cyc_Yystacktype _T11CF;struct Cyc_Yyltype _T11D0;unsigned _T11D1;unsigned _T11D2;struct Cyc_Yystacktype*_T11D3;union Cyc_YYSTYPE*_T11D4;union Cyc_YYSTYPE*_T11D5;struct Cyc_List_List*_T11D6;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T11D7;void*_T11D8;struct Cyc_Yystacktype*_T11D9;struct Cyc_Yystacktype _T11DA;struct Cyc_Yyltype _T11DB;unsigned _T11DC;unsigned _T11DD;struct Cyc_Absyn_Pat*_T11DE;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11DF;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11E0;struct Cyc_Yystacktype*_T11E1;union Cyc_YYSTYPE*_T11E2;union Cyc_YYSTYPE*_T11E3;void*_T11E4;struct Cyc_Yystacktype*_T11E5;struct Cyc_Yystacktype _T11E6;struct Cyc_Yyltype _T11E7;unsigned _T11E8;unsigned _T11E9;void*_T11EA;struct Cyc_Yystacktype*_T11EB;struct Cyc_Yystacktype _T11EC;struct Cyc_Yyltype _T11ED;unsigned _T11EE;unsigned _T11EF;struct Cyc_Absyn_Pat*_T11F0;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11F1;struct Cyc_Yystacktype*_T11F2;struct Cyc_Yystacktype _T11F3;struct Cyc_Yyltype _T11F4;unsigned _T11F5;unsigned _T11F6;struct _tuple0*_T11F7;struct _fat_ptr*_T11F8;struct Cyc_Yystacktype*_T11F9;union Cyc_YYSTYPE*_T11FA;union Cyc_YYSTYPE*_T11FB;void*_T11FC;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11FD;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11FE;void*_T11FF;struct Cyc_Yystacktype*_T1200;struct Cyc_Yystacktype _T1201;struct Cyc_Yyltype _T1202;unsigned _T1203;unsigned _T1204;void*_T1205;struct Cyc_Yystacktype*_T1206;struct Cyc_Yystacktype _T1207;struct Cyc_Yyltype _T1208;unsigned _T1209;unsigned _T120A;struct Cyc_Absyn_Pat*_T120B;struct Cyc_Yystacktype*_T120C;union Cyc_YYSTYPE*_T120D;union Cyc_YYSTYPE*_T120E;struct _fat_ptr _T120F;struct _fat_ptr _T1210;int _T1211;struct Cyc_Yystacktype*_T1212;struct Cyc_Yystacktype _T1213;struct Cyc_Yyltype _T1214;unsigned _T1215;unsigned _T1216;struct _fat_ptr _T1217;struct _fat_ptr _T1218;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T1219;struct Cyc_Yystacktype*_T121A;struct Cyc_Yystacktype _T121B;struct Cyc_Yyltype _T121C;unsigned _T121D;unsigned _T121E;struct _tuple0*_T121F;struct _fat_ptr*_T1220;struct Cyc_Yystacktype*_T1221;union Cyc_YYSTYPE*_T1222;union Cyc_YYSTYPE*_T1223;void*_T1224;struct Cyc_Yystacktype*_T1225;union Cyc_YYSTYPE*_T1226;union Cyc_YYSTYPE*_T1227;void*_T1228;struct Cyc_Yystacktype*_T1229;struct Cyc_Yystacktype _T122A;struct Cyc_Yyltype _T122B;unsigned _T122C;unsigned _T122D;struct Cyc_Absyn_Pat*_T122E;struct Cyc_Yystacktype*_T122F;union Cyc_YYSTYPE*_T1230;union Cyc_YYSTYPE*_T1231;struct _fat_ptr _T1232;struct Cyc_Absyn_Kind*_T1233;struct Cyc_Absyn_Kind*_T1234;void*_T1235;struct Cyc_Yystacktype*_T1236;struct Cyc_Yystacktype _T1237;struct Cyc_Yyltype _T1238;unsigned _T1239;unsigned _T123A;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T123B;struct Cyc_Yystacktype*_T123C;struct Cyc_Yystacktype _T123D;struct Cyc_Yyltype _T123E;unsigned _T123F;unsigned _T1240;void*_T1241;struct Cyc_Yystacktype*_T1242;struct Cyc_Yystacktype _T1243;struct Cyc_Yyltype _T1244;unsigned _T1245;unsigned _T1246;struct _tuple0*_T1247;struct _fat_ptr*_T1248;struct Cyc_Yystacktype*_T1249;union Cyc_YYSTYPE*_T124A;union Cyc_YYSTYPE*_T124B;void*_T124C;void*_T124D;struct Cyc_Yystacktype*_T124E;struct Cyc_Yystacktype _T124F;struct Cyc_Yyltype _T1250;unsigned _T1251;unsigned _T1252;struct Cyc_Absyn_Pat*_T1253;struct Cyc_Absyn_Kind*_T1254;struct Cyc_Absyn_Kind*_T1255;void*_T1256;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1257;struct Cyc_Yystacktype*_T1258;struct Cyc_Yystacktype _T1259;struct Cyc_Yyltype _T125A;unsigned _T125B;unsigned _T125C;struct _tuple0*_T125D;struct _fat_ptr*_T125E;struct Cyc_Yystacktype*_T125F;union Cyc_YYSTYPE*_T1260;union Cyc_YYSTYPE*_T1261;void*_T1262;void*_T1263;void*_T1264;struct Cyc_Yystacktype*_T1265;struct Cyc_Yystacktype _T1266;struct Cyc_Yyltype _T1267;unsigned _T1268;unsigned _T1269;struct Cyc_Absyn_Pat*_T126A;struct _tuple23*_T126B;struct Cyc_Yystacktype*_T126C;union Cyc_YYSTYPE*_T126D;union Cyc_YYSTYPE*_T126E;struct Cyc_List_List*_T126F;struct _tuple23*_T1270;struct Cyc_Yystacktype*_T1271;union Cyc_YYSTYPE*_T1272;union Cyc_YYSTYPE*_T1273;struct Cyc_List_List*_T1274;struct _tuple23*_T1275;struct Cyc_List_List*_T1276;struct Cyc_Yystacktype*_T1277;union Cyc_YYSTYPE*_T1278;union Cyc_YYSTYPE*_T1279;struct Cyc_List_List*_T127A;struct Cyc_Yystacktype*_T127B;union Cyc_YYSTYPE*_T127C;union Cyc_YYSTYPE*_T127D;struct Cyc_Yystacktype*_T127E;union Cyc_YYSTYPE*_T127F;union Cyc_YYSTYPE*_T1280;struct _tuple24*_T1281;struct Cyc_Yystacktype*_T1282;union Cyc_YYSTYPE*_T1283;union Cyc_YYSTYPE*_T1284;struct _tuple24*_T1285;struct Cyc_Yystacktype*_T1286;union Cyc_YYSTYPE*_T1287;union Cyc_YYSTYPE*_T1288;struct Cyc_Yystacktype*_T1289;union Cyc_YYSTYPE*_T128A;union Cyc_YYSTYPE*_T128B;struct _tuple23*_T128C;struct Cyc_Yystacktype*_T128D;union Cyc_YYSTYPE*_T128E;union Cyc_YYSTYPE*_T128F;struct Cyc_List_List*_T1290;struct _tuple23*_T1291;struct Cyc_Yystacktype*_T1292;union Cyc_YYSTYPE*_T1293;union Cyc_YYSTYPE*_T1294;struct Cyc_List_List*_T1295;struct _tuple23*_T1296;struct Cyc_List_List*_T1297;struct Cyc_Yystacktype*_T1298;union Cyc_YYSTYPE*_T1299;union Cyc_YYSTYPE*_T129A;struct Cyc_List_List*_T129B;struct Cyc_Yystacktype*_T129C;union Cyc_YYSTYPE*_T129D;union Cyc_YYSTYPE*_T129E;struct Cyc_Yystacktype*_T129F;union Cyc_YYSTYPE*_T12A0;union Cyc_YYSTYPE*_T12A1;struct Cyc_Yystacktype*_T12A2;struct Cyc_Yystacktype _T12A3;struct Cyc_Yystacktype*_T12A4;union Cyc_YYSTYPE*_T12A5;union Cyc_YYSTYPE*_T12A6;struct Cyc_Absyn_Exp*_T12A7;struct Cyc_Yystacktype*_T12A8;union Cyc_YYSTYPE*_T12A9;union Cyc_YYSTYPE*_T12AA;struct Cyc_Absyn_Exp*_T12AB;struct Cyc_Yystacktype*_T12AC;struct Cyc_Yystacktype _T12AD;struct Cyc_Yyltype _T12AE;unsigned _T12AF;unsigned _T12B0;struct Cyc_Absyn_Exp*_T12B1;struct Cyc_Yystacktype*_T12B2;struct Cyc_Yystacktype _T12B3;struct Cyc_Yystacktype*_T12B4;union Cyc_YYSTYPE*_T12B5;union Cyc_YYSTYPE*_T12B6;struct Cyc_Absyn_Exp*_T12B7;struct Cyc_Yystacktype*_T12B8;union Cyc_YYSTYPE*_T12B9;union Cyc_YYSTYPE*_T12BA;struct Cyc_Core_Opt*_T12BB;struct Cyc_Yystacktype*_T12BC;union Cyc_YYSTYPE*_T12BD;union Cyc_YYSTYPE*_T12BE;struct Cyc_Absyn_Exp*_T12BF;struct Cyc_Yystacktype*_T12C0;struct Cyc_Yystacktype _T12C1;struct Cyc_Yyltype _T12C2;unsigned _T12C3;unsigned _T12C4;struct Cyc_Absyn_Exp*_T12C5;struct Cyc_Yystacktype*_T12C6;union Cyc_YYSTYPE*_T12C7;union Cyc_YYSTYPE*_T12C8;struct Cyc_Absyn_Exp*_T12C9;struct Cyc_Yystacktype*_T12CA;union Cyc_YYSTYPE*_T12CB;union Cyc_YYSTYPE*_T12CC;struct Cyc_Absyn_Exp*_T12CD;struct Cyc_Yystacktype*_T12CE;struct Cyc_Yystacktype _T12CF;struct Cyc_Yyltype _T12D0;unsigned _T12D1;unsigned _T12D2;struct Cyc_Absyn_Exp*_T12D3;struct Cyc_Core_Opt*_T12D4;struct Cyc_Core_Opt*_T12D5;struct Cyc_Core_Opt*_T12D6;struct Cyc_Core_Opt*_T12D7;struct Cyc_Core_Opt*_T12D8;struct Cyc_Core_Opt*_T12D9;struct Cyc_Core_Opt*_T12DA;struct Cyc_Core_Opt*_T12DB;struct Cyc_Core_Opt*_T12DC;struct Cyc_Core_Opt*_T12DD;struct Cyc_Yystacktype*_T12DE;struct Cyc_Yystacktype _T12DF;struct Cyc_Yystacktype*_T12E0;union Cyc_YYSTYPE*_T12E1;union Cyc_YYSTYPE*_T12E2;struct Cyc_Absyn_Exp*_T12E3;struct Cyc_Yystacktype*_T12E4;union Cyc_YYSTYPE*_T12E5;union Cyc_YYSTYPE*_T12E6;struct Cyc_Absyn_Exp*_T12E7;struct Cyc_Yystacktype*_T12E8;union Cyc_YYSTYPE*_T12E9;union Cyc_YYSTYPE*_T12EA;struct Cyc_Absyn_Exp*_T12EB;struct Cyc_Yystacktype*_T12EC;struct Cyc_Yystacktype _T12ED;struct Cyc_Yyltype _T12EE;unsigned _T12EF;unsigned _T12F0;struct Cyc_Absyn_Exp*_T12F1;struct Cyc_Yystacktype*_T12F2;union Cyc_YYSTYPE*_T12F3;union Cyc_YYSTYPE*_T12F4;struct Cyc_Absyn_Exp*_T12F5;struct Cyc_Yystacktype*_T12F6;struct Cyc_Yystacktype _T12F7;struct Cyc_Yyltype _T12F8;unsigned _T12F9;unsigned _T12FA;struct Cyc_Absyn_Exp*_T12FB;struct Cyc_Yystacktype*_T12FC;union Cyc_YYSTYPE*_T12FD;union Cyc_YYSTYPE*_T12FE;struct Cyc_Absyn_Exp*_T12FF;struct Cyc_Yystacktype*_T1300;struct Cyc_Yystacktype _T1301;struct Cyc_Yyltype _T1302;unsigned _T1303;unsigned _T1304;struct Cyc_Absyn_Exp*_T1305;struct Cyc_Yystacktype*_T1306;union Cyc_YYSTYPE*_T1307;union Cyc_YYSTYPE*_T1308;struct Cyc_Absyn_Exp*_T1309;struct Cyc_Yystacktype*_T130A;struct Cyc_Yystacktype _T130B;struct Cyc_Yyltype _T130C;unsigned _T130D;unsigned _T130E;struct Cyc_Absyn_Exp*_T130F;struct Cyc_Yystacktype*_T1310;union Cyc_YYSTYPE*_T1311;union Cyc_YYSTYPE*_T1312;struct Cyc_Absyn_Exp*_T1313;struct Cyc_Yystacktype*_T1314;union Cyc_YYSTYPE*_T1315;union Cyc_YYSTYPE*_T1316;struct Cyc_Absyn_Exp*_T1317;struct Cyc_Yystacktype*_T1318;struct Cyc_Yystacktype _T1319;struct Cyc_Yyltype _T131A;unsigned _T131B;unsigned _T131C;struct Cyc_Absyn_Exp*_T131D;struct Cyc_Yystacktype*_T131E;union Cyc_YYSTYPE*_T131F;union Cyc_YYSTYPE*_T1320;struct Cyc_Absyn_Exp*_T1321;struct Cyc_Yystacktype*_T1322;union Cyc_YYSTYPE*_T1323;union Cyc_YYSTYPE*_T1324;struct Cyc_Absyn_Exp*_T1325;struct Cyc_Yystacktype*_T1326;struct Cyc_Yystacktype _T1327;struct Cyc_Yyltype _T1328;unsigned _T1329;unsigned _T132A;struct Cyc_Absyn_Exp*_T132B;struct Cyc_Yystacktype*_T132C;union Cyc_YYSTYPE*_T132D;union Cyc_YYSTYPE*_T132E;struct Cyc_Absyn_Exp*_T132F;struct Cyc_Absyn_Exp*_T1330;struct Cyc_Yystacktype*_T1331;union Cyc_YYSTYPE*_T1332;union Cyc_YYSTYPE*_T1333;struct Cyc_Absyn_Exp*_T1334;struct Cyc_Absyn_Exp*_T1335;struct Cyc_Yystacktype*_T1336;struct Cyc_Yystacktype _T1337;struct Cyc_Yyltype _T1338;unsigned _T1339;unsigned _T133A;struct Cyc_Absyn_Exp*_T133B;struct Cyc_Yystacktype*_T133C;union Cyc_YYSTYPE*_T133D;union Cyc_YYSTYPE*_T133E;struct Cyc_Absyn_Exp*_T133F;struct Cyc_Absyn_Exp*_T1340;struct Cyc_Yystacktype*_T1341;union Cyc_YYSTYPE*_T1342;union Cyc_YYSTYPE*_T1343;struct Cyc_Absyn_Exp*_T1344;struct Cyc_Absyn_Exp*_T1345;struct Cyc_Yystacktype*_T1346;struct Cyc_Yystacktype _T1347;struct Cyc_Yyltype _T1348;unsigned _T1349;unsigned _T134A;struct Cyc_Absyn_Exp*_T134B;struct Cyc_Yystacktype*_T134C;struct Cyc_Yystacktype _T134D;struct Cyc_Yystacktype*_T134E;struct Cyc_Yystacktype _T134F;struct Cyc_Yystacktype*_T1350;union Cyc_YYSTYPE*_T1351;union Cyc_YYSTYPE*_T1352;struct Cyc_Absyn_Exp*_T1353;struct Cyc_Yystacktype*_T1354;union Cyc_YYSTYPE*_T1355;union Cyc_YYSTYPE*_T1356;struct Cyc_Absyn_Exp*_T1357;struct Cyc_Yystacktype*_T1358;struct Cyc_Yystacktype _T1359;struct Cyc_Yyltype _T135A;unsigned _T135B;unsigned _T135C;struct Cyc_Absyn_Exp*_T135D;struct Cyc_Yystacktype*_T135E;struct Cyc_Yystacktype _T135F;struct Cyc_Yystacktype*_T1360;union Cyc_YYSTYPE*_T1361;union Cyc_YYSTYPE*_T1362;struct Cyc_Absyn_Exp*_T1363;struct Cyc_Yystacktype*_T1364;union Cyc_YYSTYPE*_T1365;union Cyc_YYSTYPE*_T1366;struct Cyc_Absyn_Exp*_T1367;struct Cyc_Yystacktype*_T1368;struct Cyc_Yystacktype _T1369;struct Cyc_Yyltype _T136A;unsigned _T136B;unsigned _T136C;struct Cyc_Absyn_Exp*_T136D;struct Cyc_Yystacktype*_T136E;struct Cyc_Yystacktype _T136F;struct Cyc_Yystacktype*_T1370;union Cyc_YYSTYPE*_T1371;union Cyc_YYSTYPE*_T1372;struct Cyc_Absyn_Exp*_T1373;struct Cyc_Yystacktype*_T1374;union Cyc_YYSTYPE*_T1375;union Cyc_YYSTYPE*_T1376;struct Cyc_Absyn_Exp*_T1377;struct Cyc_Yystacktype*_T1378;struct Cyc_Yystacktype _T1379;struct Cyc_Yyltype _T137A;unsigned _T137B;unsigned _T137C;struct Cyc_Absyn_Exp*_T137D;struct Cyc_Yystacktype*_T137E;struct Cyc_Yystacktype _T137F;struct Cyc_Yystacktype*_T1380;union Cyc_YYSTYPE*_T1381;union Cyc_YYSTYPE*_T1382;struct Cyc_Absyn_Exp*_T1383;struct Cyc_Yystacktype*_T1384;union Cyc_YYSTYPE*_T1385;union Cyc_YYSTYPE*_T1386;struct Cyc_Absyn_Exp*_T1387;struct Cyc_Yystacktype*_T1388;struct Cyc_Yystacktype _T1389;struct Cyc_Yyltype _T138A;unsigned _T138B;unsigned _T138C;struct Cyc_Absyn_Exp*_T138D;struct Cyc_Yystacktype*_T138E;struct Cyc_Yystacktype _T138F;struct Cyc_Yystacktype*_T1390;union Cyc_YYSTYPE*_T1391;union Cyc_YYSTYPE*_T1392;struct Cyc_Absyn_Exp*_T1393;struct Cyc_Yystacktype*_T1394;union Cyc_YYSTYPE*_T1395;union Cyc_YYSTYPE*_T1396;struct Cyc_Absyn_Exp*_T1397;struct Cyc_Yystacktype*_T1398;struct Cyc_Yystacktype _T1399;struct Cyc_Yyltype _T139A;unsigned _T139B;unsigned _T139C;struct Cyc_Absyn_Exp*_T139D;struct Cyc_Yystacktype*_T139E;struct Cyc_Yystacktype _T139F;struct Cyc_Yystacktype*_T13A0;union Cyc_YYSTYPE*_T13A1;union Cyc_YYSTYPE*_T13A2;struct Cyc_Absyn_Exp*(*_T13A3)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T13A4;union Cyc_YYSTYPE*_T13A5;union Cyc_YYSTYPE*_T13A6;struct Cyc_Absyn_Exp*_T13A7;struct Cyc_Yystacktype*_T13A8;union Cyc_YYSTYPE*_T13A9;union Cyc_YYSTYPE*_T13AA;struct Cyc_Absyn_Exp*_T13AB;struct Cyc_Yystacktype*_T13AC;struct Cyc_Yystacktype _T13AD;struct Cyc_Yyltype _T13AE;unsigned _T13AF;unsigned _T13B0;struct Cyc_Absyn_Exp*_T13B1;struct Cyc_Yystacktype*_T13B2;struct Cyc_Yystacktype _T13B3;struct Cyc_Yystacktype*_T13B4;union Cyc_YYSTYPE*_T13B5;union Cyc_YYSTYPE*_T13B6;struct Cyc_Absyn_Exp*(*_T13B7)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T13B8;union Cyc_YYSTYPE*_T13B9;union Cyc_YYSTYPE*_T13BA;struct Cyc_Absyn_Exp*_T13BB;struct Cyc_Yystacktype*_T13BC;union Cyc_YYSTYPE*_T13BD;union Cyc_YYSTYPE*_T13BE;struct Cyc_Absyn_Exp*_T13BF;struct Cyc_Yystacktype*_T13C0;struct Cyc_Yystacktype _T13C1;struct Cyc_Yyltype _T13C2;unsigned _T13C3;unsigned _T13C4;struct Cyc_Absyn_Exp*_T13C5;struct Cyc_Yystacktype*_T13C6;struct Cyc_Yystacktype _T13C7;struct Cyc_Yystacktype*_T13C8;union Cyc_YYSTYPE*_T13C9;union Cyc_YYSTYPE*_T13CA;struct Cyc_Absyn_Exp*_T13CB;struct Cyc_Yystacktype*_T13CC;union Cyc_YYSTYPE*_T13CD;union Cyc_YYSTYPE*_T13CE;struct Cyc_Absyn_Exp*_T13CF;struct Cyc_Yystacktype*_T13D0;struct Cyc_Yystacktype _T13D1;struct Cyc_Yyltype _T13D2;unsigned _T13D3;unsigned _T13D4;struct Cyc_Absyn_Exp*_T13D5;struct Cyc_Yystacktype*_T13D6;union Cyc_YYSTYPE*_T13D7;union Cyc_YYSTYPE*_T13D8;struct Cyc_Absyn_Exp*_T13D9;struct Cyc_Yystacktype*_T13DA;union Cyc_YYSTYPE*_T13DB;union Cyc_YYSTYPE*_T13DC;struct Cyc_Absyn_Exp*_T13DD;struct Cyc_Yystacktype*_T13DE;struct Cyc_Yystacktype _T13DF;struct Cyc_Yyltype _T13E0;unsigned _T13E1;unsigned _T13E2;struct Cyc_Absyn_Exp*_T13E3;struct Cyc_Yystacktype*_T13E4;struct Cyc_Yystacktype _T13E5;struct Cyc_Yystacktype*_T13E6;union Cyc_YYSTYPE*_T13E7;union Cyc_YYSTYPE*_T13E8;enum Cyc_Absyn_Primop _T13E9;struct Cyc_Yystacktype*_T13EA;union Cyc_YYSTYPE*_T13EB;union Cyc_YYSTYPE*_T13EC;struct Cyc_Absyn_Exp*_T13ED;struct Cyc_Yystacktype*_T13EE;union Cyc_YYSTYPE*_T13EF;union Cyc_YYSTYPE*_T13F0;struct Cyc_Absyn_Exp*_T13F1;struct Cyc_Yystacktype*_T13F2;struct Cyc_Yystacktype _T13F3;struct Cyc_Yyltype _T13F4;unsigned _T13F5;unsigned _T13F6;struct Cyc_Absyn_Exp*_T13F7;struct Cyc_Yystacktype*_T13F8;struct Cyc_Yystacktype _T13F9;struct Cyc_Yystacktype*_T13FA;union Cyc_YYSTYPE*_T13FB;union Cyc_YYSTYPE*_T13FC;enum Cyc_Absyn_Primop _T13FD;struct Cyc_Yystacktype*_T13FE;union Cyc_YYSTYPE*_T13FF;union Cyc_YYSTYPE*_T1400;struct Cyc_Absyn_Exp*_T1401;struct Cyc_Yystacktype*_T1402;union Cyc_YYSTYPE*_T1403;union Cyc_YYSTYPE*_T1404;struct Cyc_Absyn_Exp*_T1405;struct Cyc_Yystacktype*_T1406;struct Cyc_Yystacktype _T1407;struct Cyc_Yyltype _T1408;unsigned _T1409;unsigned _T140A;struct Cyc_Absyn_Exp*_T140B;struct Cyc_Absyn_Exp*(*_T140C)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T140D)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T140E)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T140F)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T1410)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T1411)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T1412;struct Cyc_Yystacktype _T1413;struct Cyc_Yystacktype*_T1414;union Cyc_YYSTYPE*_T1415;union Cyc_YYSTYPE*_T1416;struct _tuple8*_T1417;struct Cyc_Yystacktype*_T1418;struct Cyc_Yystacktype _T1419;struct Cyc_Yyltype _T141A;unsigned _T141B;unsigned _T141C;void*_T141D;struct Cyc_Yystacktype*_T141E;union Cyc_YYSTYPE*_T141F;union Cyc_YYSTYPE*_T1420;struct Cyc_Absyn_Exp*_T1421;struct Cyc_Yystacktype*_T1422;struct Cyc_Yystacktype _T1423;struct Cyc_Yyltype _T1424;unsigned _T1425;unsigned _T1426;struct Cyc_Absyn_Exp*_T1427;struct Cyc_Yystacktype*_T1428;struct Cyc_Yystacktype _T1429;struct Cyc_Yystacktype*_T142A;union Cyc_YYSTYPE*_T142B;union Cyc_YYSTYPE*_T142C;struct Cyc_Absyn_Exp*_T142D;struct Cyc_Yystacktype*_T142E;struct Cyc_Yystacktype _T142F;struct Cyc_Yyltype _T1430;unsigned _T1431;unsigned _T1432;struct Cyc_Absyn_Exp*_T1433;struct Cyc_Yystacktype*_T1434;union Cyc_YYSTYPE*_T1435;union Cyc_YYSTYPE*_T1436;struct Cyc_Absyn_Exp*_T1437;struct Cyc_Yystacktype*_T1438;struct Cyc_Yystacktype _T1439;struct Cyc_Yyltype _T143A;unsigned _T143B;unsigned _T143C;struct Cyc_Absyn_Exp*_T143D;struct Cyc_Yystacktype*_T143E;union Cyc_YYSTYPE*_T143F;union Cyc_YYSTYPE*_T1440;struct Cyc_Absyn_Exp*_T1441;struct Cyc_Yystacktype*_T1442;struct Cyc_Yystacktype _T1443;struct Cyc_Yyltype _T1444;unsigned _T1445;unsigned _T1446;struct Cyc_Absyn_Exp*_T1447;struct Cyc_Yystacktype*_T1448;union Cyc_YYSTYPE*_T1449;union Cyc_YYSTYPE*_T144A;struct Cyc_Absyn_Exp*_T144B;struct Cyc_Yystacktype*_T144C;struct Cyc_Yystacktype _T144D;struct Cyc_Yyltype _T144E;unsigned _T144F;unsigned _T1450;struct Cyc_Absyn_Exp*_T1451;struct Cyc_Yystacktype*_T1452;union Cyc_YYSTYPE*_T1453;union Cyc_YYSTYPE*_T1454;enum Cyc_Absyn_Primop _T1455;struct Cyc_Yystacktype*_T1456;union Cyc_YYSTYPE*_T1457;union Cyc_YYSTYPE*_T1458;struct Cyc_Absyn_Exp*_T1459;struct Cyc_Yystacktype*_T145A;struct Cyc_Yystacktype _T145B;struct Cyc_Yyltype _T145C;unsigned _T145D;unsigned _T145E;struct Cyc_Absyn_Exp*_T145F;struct Cyc_Yystacktype*_T1460;union Cyc_YYSTYPE*_T1461;union Cyc_YYSTYPE*_T1462;struct _tuple8*_T1463;struct Cyc_Yystacktype*_T1464;struct Cyc_Yystacktype _T1465;struct Cyc_Yyltype _T1466;unsigned _T1467;unsigned _T1468;void*_T1469;struct Cyc_Yystacktype*_T146A;struct Cyc_Yystacktype _T146B;struct Cyc_Yyltype _T146C;unsigned _T146D;unsigned _T146E;struct Cyc_Absyn_Exp*_T146F;struct Cyc_Yystacktype*_T1470;union Cyc_YYSTYPE*_T1471;union Cyc_YYSTYPE*_T1472;struct Cyc_Absyn_Exp*_T1473;struct Cyc_Yystacktype*_T1474;struct Cyc_Yystacktype _T1475;struct Cyc_Yyltype _T1476;unsigned _T1477;unsigned _T1478;struct Cyc_Absyn_Exp*_T1479;struct Cyc_Yystacktype*_T147A;union Cyc_YYSTYPE*_T147B;union Cyc_YYSTYPE*_T147C;struct _tuple8*_T147D;struct Cyc_Yystacktype*_T147E;struct Cyc_Yystacktype _T147F;struct Cyc_Yyltype _T1480;unsigned _T1481;unsigned _T1482;void*_T1483;struct Cyc_Yystacktype*_T1484;union Cyc_YYSTYPE*_T1485;union Cyc_YYSTYPE*_T1486;struct Cyc_List_List*_T1487;struct Cyc_List_List*_T1488;struct Cyc_Yystacktype*_T1489;struct Cyc_Yystacktype _T148A;struct Cyc_Yyltype _T148B;unsigned _T148C;unsigned _T148D;struct Cyc_Absyn_Exp*_T148E;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T148F;struct Cyc_Yystacktype*_T1490;union Cyc_YYSTYPE*_T1491;union Cyc_YYSTYPE*_T1492;void*_T1493;struct Cyc_Yystacktype*_T1494;struct Cyc_Yystacktype _T1495;struct Cyc_Yyltype _T1496;unsigned _T1497;unsigned _T1498;struct Cyc_Absyn_Exp*_T1499;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T149A;struct Cyc_Yystacktype*_T149B;union Cyc_YYSTYPE*_T149C;union Cyc_YYSTYPE*_T149D;struct Cyc_Yystacktype*_T149E;union Cyc_YYSTYPE*_T149F;union Cyc_YYSTYPE*_T14A0;struct Cyc_Yystacktype*_T14A1;union Cyc_YYSTYPE*_T14A2;union Cyc_YYSTYPE*_T14A3;void*_T14A4;struct Cyc_Yystacktype*_T14A5;struct Cyc_Yystacktype _T14A6;struct Cyc_Yyltype _T14A7;unsigned _T14A8;unsigned _T14A9;struct Cyc_Absyn_Exp*_T14AA;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14AB;struct Cyc_Yystacktype*_T14AC;union Cyc_YYSTYPE*_T14AD;union Cyc_YYSTYPE*_T14AE;struct Cyc_Yystacktype*_T14AF;union Cyc_YYSTYPE*_T14B0;union Cyc_YYSTYPE*_T14B1;void*_T14B2;struct Cyc_Yystacktype*_T14B3;struct Cyc_Yystacktype _T14B4;struct Cyc_Yyltype _T14B5;unsigned _T14B6;unsigned _T14B7;struct Cyc_Absyn_Exp*_T14B8;struct Cyc_Yystacktype*_T14B9;union Cyc_YYSTYPE*_T14BA;union Cyc_YYSTYPE*_T14BB;struct Cyc_Absyn_Exp*_T14BC;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14BD;struct Cyc_Yystacktype*_T14BE;union Cyc_YYSTYPE*_T14BF;union Cyc_YYSTYPE*_T14C0;void*_T14C1;struct Cyc_Yystacktype*_T14C2;struct Cyc_Yystacktype _T14C3;struct Cyc_Yyltype _T14C4;unsigned _T14C5;unsigned _T14C6;struct Cyc_Absyn_Exp*_T14C7;struct Cyc_Yystacktype*_T14C8;union Cyc_YYSTYPE*_T14C9;union Cyc_YYSTYPE*_T14CA;struct Cyc_Absyn_Exp*_T14CB;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14CC;struct Cyc_Yystacktype*_T14CD;union Cyc_YYSTYPE*_T14CE;union Cyc_YYSTYPE*_T14CF;void*_T14D0;struct Cyc_Yystacktype*_T14D1;struct Cyc_Yystacktype _T14D2;struct Cyc_Yyltype _T14D3;unsigned _T14D4;unsigned _T14D5;struct Cyc_Absyn_Exp*_T14D6;struct Cyc_Yystacktype*_T14D7;union Cyc_YYSTYPE*_T14D8;union Cyc_YYSTYPE*_T14D9;struct _tuple8*_T14DA;struct Cyc_Yystacktype*_T14DB;struct Cyc_Yystacktype _T14DC;struct Cyc_Yyltype _T14DD;unsigned _T14DE;unsigned _T14DF;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14E0;void**_T14E1;struct Cyc_Yystacktype*_T14E2;union Cyc_YYSTYPE*_T14E3;union Cyc_YYSTYPE*_T14E4;void*_T14E5;struct Cyc_Yystacktype*_T14E6;struct Cyc_Yystacktype _T14E7;struct Cyc_Yyltype _T14E8;unsigned _T14E9;unsigned _T14EA;struct Cyc_Absyn_Exp*_T14EB;struct Cyc_Yystacktype*_T14EC;union Cyc_YYSTYPE*_T14ED;union Cyc_YYSTYPE*_T14EE;struct _tuple8*_T14EF;struct Cyc_Yystacktype*_T14F0;struct Cyc_Yystacktype _T14F1;struct Cyc_Yyltype _T14F2;unsigned _T14F3;unsigned _T14F4;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14F5;struct Cyc_Yystacktype*_T14F6;union Cyc_YYSTYPE*_T14F7;union Cyc_YYSTYPE*_T14F8;struct Cyc_Yystacktype*_T14F9;union Cyc_YYSTYPE*_T14FA;union Cyc_YYSTYPE*_T14FB;void**_T14FC;struct Cyc_Yystacktype*_T14FD;union Cyc_YYSTYPE*_T14FE;union Cyc_YYSTYPE*_T14FF;void*_T1500;struct Cyc_Yystacktype*_T1501;struct Cyc_Yystacktype _T1502;struct Cyc_Yyltype _T1503;unsigned _T1504;unsigned _T1505;struct Cyc_Absyn_Exp*_T1506;struct Cyc_Yystacktype*_T1507;union Cyc_YYSTYPE*_T1508;union Cyc_YYSTYPE*_T1509;struct _tuple8*_T150A;struct Cyc_Yystacktype*_T150B;struct Cyc_Yystacktype _T150C;struct Cyc_Yyltype _T150D;unsigned _T150E;unsigned _T150F;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1510;struct Cyc_Yystacktype*_T1511;union Cyc_YYSTYPE*_T1512;union Cyc_YYSTYPE*_T1513;void**_T1514;struct Cyc_Yystacktype*_T1515;union Cyc_YYSTYPE*_T1516;union Cyc_YYSTYPE*_T1517;void*_T1518;struct Cyc_Yystacktype*_T1519;struct Cyc_Yystacktype _T151A;struct Cyc_Yyltype _T151B;unsigned _T151C;unsigned _T151D;struct Cyc_Absyn_Exp*_T151E;struct Cyc_List_List*_T151F;struct Cyc_Yystacktype*_T1520;union Cyc_YYSTYPE*_T1521;union Cyc_YYSTYPE*_T1522;struct _fat_ptr _T1523;struct Cyc_Yystacktype*_T1524;struct Cyc_Yystacktype _T1525;struct Cyc_Yyltype _T1526;unsigned _T1527;unsigned _T1528;struct Cyc_Absyn_Exp*_T1529;struct Cyc_Yystacktype*_T152A;union Cyc_YYSTYPE*_T152B;union Cyc_YYSTYPE*_T152C;struct Cyc_Absyn_Exp*_T152D;struct Cyc_Yystacktype*_T152E;struct Cyc_Yystacktype _T152F;struct Cyc_Yyltype _T1530;unsigned _T1531;unsigned _T1532;struct Cyc_Absyn_Exp*_T1533;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1534;struct Cyc_Yystacktype*_T1535;union Cyc_YYSTYPE*_T1536;union Cyc_YYSTYPE*_T1537;struct _fat_ptr*_T1538;struct Cyc_Yystacktype*_T1539;union Cyc_YYSTYPE*_T153A;union Cyc_YYSTYPE*_T153B;void*_T153C;struct Cyc_Yystacktype*_T153D;struct Cyc_Yystacktype _T153E;struct Cyc_Yyltype _T153F;unsigned _T1540;unsigned _T1541;struct Cyc_Absyn_Exp*_T1542;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1543;struct Cyc_Yystacktype*_T1544;union Cyc_YYSTYPE*_T1545;union Cyc_YYSTYPE*_T1546;struct Cyc_Absyn_Exp*_T1547;struct Cyc_Yystacktype*_T1548;struct Cyc_Yystacktype _T1549;struct Cyc_Yyltype _T154A;unsigned _T154B;unsigned _T154C;struct _fat_ptr*_T154D;struct Cyc_Yystacktype*_T154E;union Cyc_YYSTYPE*_T154F;union Cyc_YYSTYPE*_T1550;void*_T1551;struct Cyc_Yystacktype*_T1552;struct Cyc_Yystacktype _T1553;struct Cyc_Yyltype _T1554;unsigned _T1555;unsigned _T1556;struct Cyc_Absyn_Exp*_T1557;struct Cyc_Yystacktype*_T1558;union Cyc_YYSTYPE*_T1559;union Cyc_YYSTYPE*_T155A;struct _tuple8*_T155B;struct Cyc_Yystacktype*_T155C;struct Cyc_Yystacktype _T155D;struct Cyc_Yyltype _T155E;unsigned _T155F;unsigned _T1560;void*_T1561;struct Cyc_Yystacktype*_T1562;struct Cyc_Yystacktype _T1563;struct Cyc_Yyltype _T1564;unsigned _T1565;unsigned _T1566;struct Cyc_Absyn_Exp*_T1567;struct Cyc_Yystacktype*_T1568;union Cyc_YYSTYPE*_T1569;union Cyc_YYSTYPE*_T156A;void*_T156B;struct Cyc_Yystacktype*_T156C;struct Cyc_Yystacktype _T156D;struct Cyc_Yyltype _T156E;unsigned _T156F;unsigned _T1570;struct Cyc_Absyn_Exp*_T1571;struct Cyc_Yystacktype*_T1572;union Cyc_YYSTYPE*_T1573;union Cyc_YYSTYPE*_T1574;struct Cyc_Absyn_Exp*_T1575;struct Cyc_Yystacktype*_T1576;struct Cyc_Yystacktype _T1577;struct Cyc_Yyltype _T1578;unsigned _T1579;unsigned _T157A;struct Cyc_Absyn_Exp*_T157B;struct Cyc_Yystacktype*_T157C;union Cyc_YYSTYPE*_T157D;union Cyc_YYSTYPE*_T157E;struct Cyc_Absyn_Exp*_T157F;struct Cyc_Yystacktype*_T1580;struct Cyc_Yystacktype _T1581;struct Cyc_Yyltype _T1582;unsigned _T1583;unsigned _T1584;struct Cyc_Absyn_Exp*_T1585;struct Cyc_Yystacktype*_T1586;union Cyc_YYSTYPE*_T1587;union Cyc_YYSTYPE*_T1588;struct Cyc_Absyn_Exp*_T1589;struct Cyc_Yystacktype*_T158A;struct Cyc_Yystacktype _T158B;struct Cyc_Yyltype _T158C;unsigned _T158D;unsigned _T158E;struct Cyc_Absyn_Exp*_T158F;struct Cyc_Yystacktype*_T1590;union Cyc_YYSTYPE*_T1591;union Cyc_YYSTYPE*_T1592;struct Cyc_Absyn_Exp*_T1593;struct Cyc_Yystacktype*_T1594;struct Cyc_Yystacktype _T1595;struct Cyc_Yyltype _T1596;unsigned _T1597;unsigned _T1598;struct Cyc_Absyn_Exp*_T1599;struct Cyc_Yystacktype*_T159A;union Cyc_YYSTYPE*_T159B;union Cyc_YYSTYPE*_T159C;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T159D;struct Cyc_Yystacktype*_T159E;union Cyc_YYSTYPE*_T159F;union Cyc_YYSTYPE*_T15A0;struct Cyc_Yystacktype*_T15A1;union Cyc_YYSTYPE*_T15A2;union Cyc_YYSTYPE*_T15A3;void*_T15A4;struct _tuple31*_T15A5;struct Cyc_Yystacktype*_T15A6;union Cyc_YYSTYPE*_T15A7;union Cyc_YYSTYPE*_T15A8;struct _tuple31*_T15A9;struct Cyc_Yystacktype*_T15AA;union Cyc_YYSTYPE*_T15AB;union Cyc_YYSTYPE*_T15AC;struct _tuple31*_T15AD;struct Cyc_Yystacktype*_T15AE;union Cyc_YYSTYPE*_T15AF;union Cyc_YYSTYPE*_T15B0;struct Cyc_List_List*_T15B1;struct Cyc_List_List*_T15B2;struct Cyc_Yystacktype*_T15B3;union Cyc_YYSTYPE*_T15B4;union Cyc_YYSTYPE*_T15B5;struct Cyc_List_List*_T15B6;struct Cyc_Yystacktype*_T15B7;union Cyc_YYSTYPE*_T15B8;union Cyc_YYSTYPE*_T15B9;struct Cyc_Yystacktype*_T15BA;union Cyc_YYSTYPE*_T15BB;union Cyc_YYSTYPE*_T15BC;struct _tuple28*_T15BD;struct _tuple28*_T15BE;struct Cyc_Yystacktype*_T15BF;union Cyc_YYSTYPE*_T15C0;union Cyc_YYSTYPE*_T15C1;struct _tuple28*_T15C2;struct Cyc_Yystacktype*_T15C3;union Cyc_YYSTYPE*_T15C4;union Cyc_YYSTYPE*_T15C5;struct Cyc_List_List*_T15C6;struct Cyc_Yystacktype*_T15C7;union Cyc_YYSTYPE*_T15C8;union Cyc_YYSTYPE*_T15C9;struct Cyc_List_List*_T15CA;struct Cyc_Yystacktype*_T15CB;union Cyc_YYSTYPE*_T15CC;union Cyc_YYSTYPE*_T15CD;struct Cyc_List_List*_T15CE;struct Cyc_Yystacktype*_T15CF;union Cyc_YYSTYPE*_T15D0;union Cyc_YYSTYPE*_T15D1;struct Cyc_Yystacktype*_T15D2;union Cyc_YYSTYPE*_T15D3;union Cyc_YYSTYPE*_T15D4;struct _tuple32*_T15D5;struct Cyc_Yystacktype*_T15D6;union Cyc_YYSTYPE*_T15D7;union Cyc_YYSTYPE*_T15D8;struct Cyc_Yystacktype*_T15D9;union Cyc_YYSTYPE*_T15DA;union Cyc_YYSTYPE*_T15DB;struct Cyc_Yystacktype*_T15DC;union Cyc_YYSTYPE*_T15DD;union Cyc_YYSTYPE*_T15DE;struct Cyc_List_List*_T15DF;struct Cyc_List_List*_T15E0;struct Cyc_List_List*_T15E1;struct _fat_ptr*_T15E2;struct Cyc_Yystacktype*_T15E3;union Cyc_YYSTYPE*_T15E4;union Cyc_YYSTYPE*_T15E5;struct Cyc_List_List*_T15E6;struct _fat_ptr*_T15E7;struct Cyc_Yystacktype*_T15E8;union Cyc_YYSTYPE*_T15E9;union Cyc_YYSTYPE*_T15EA;struct Cyc_Yystacktype*_T15EB;union Cyc_YYSTYPE*_T15EC;union Cyc_YYSTYPE*_T15ED;struct Cyc_Yystacktype*_T15EE;struct Cyc_Yystacktype _T15EF;struct Cyc_Yystacktype*_T15F0;union Cyc_YYSTYPE*_T15F1;union Cyc_YYSTYPE*_T15F2;struct Cyc_Absyn_Exp*_T15F3;struct Cyc_Yystacktype*_T15F4;union Cyc_YYSTYPE*_T15F5;union Cyc_YYSTYPE*_T15F6;struct Cyc_Absyn_Exp*_T15F7;struct Cyc_Yystacktype*_T15F8;struct Cyc_Yystacktype _T15F9;struct Cyc_Yyltype _T15FA;unsigned _T15FB;unsigned _T15FC;struct Cyc_Absyn_Exp*_T15FD;struct Cyc_Yystacktype*_T15FE;union Cyc_YYSTYPE*_T15FF;union Cyc_YYSTYPE*_T1600;struct Cyc_Absyn_Exp*_T1601;struct Cyc_Yystacktype*_T1602;struct Cyc_Yystacktype _T1603;struct Cyc_Yyltype _T1604;unsigned _T1605;unsigned _T1606;struct Cyc_Absyn_Exp*_T1607;struct Cyc_Yystacktype*_T1608;union Cyc_YYSTYPE*_T1609;union Cyc_YYSTYPE*_T160A;struct Cyc_Absyn_Exp*_T160B;struct Cyc_Yystacktype*_T160C;union Cyc_YYSTYPE*_T160D;union Cyc_YYSTYPE*_T160E;struct Cyc_List_List*_T160F;struct Cyc_Yystacktype*_T1610;struct Cyc_Yystacktype _T1611;struct Cyc_Yyltype _T1612;unsigned _T1613;unsigned _T1614;struct Cyc_Absyn_Exp*_T1615;struct Cyc_Yystacktype*_T1616;union Cyc_YYSTYPE*_T1617;union Cyc_YYSTYPE*_T1618;struct Cyc_Absyn_Exp*_T1619;struct _fat_ptr*_T161A;struct Cyc_Yystacktype*_T161B;union Cyc_YYSTYPE*_T161C;union Cyc_YYSTYPE*_T161D;struct Cyc_Yystacktype*_T161E;struct Cyc_Yystacktype _T161F;struct Cyc_Yyltype _T1620;unsigned _T1621;unsigned _T1622;struct Cyc_Absyn_Exp*_T1623;struct Cyc_Yystacktype*_T1624;union Cyc_YYSTYPE*_T1625;union Cyc_YYSTYPE*_T1626;struct Cyc_Absyn_Exp*_T1627;struct _fat_ptr*_T1628;struct Cyc_Yystacktype*_T1629;union Cyc_YYSTYPE*_T162A;union Cyc_YYSTYPE*_T162B;struct Cyc_Yystacktype*_T162C;struct Cyc_Yystacktype _T162D;struct Cyc_Yyltype _T162E;unsigned _T162F;unsigned _T1630;struct Cyc_Absyn_Exp*_T1631;struct Cyc_Yystacktype*_T1632;union Cyc_YYSTYPE*_T1633;union Cyc_YYSTYPE*_T1634;struct Cyc_Absyn_Exp*_T1635;struct Cyc_Yystacktype*_T1636;struct Cyc_Yystacktype _T1637;struct Cyc_Yyltype _T1638;unsigned _T1639;unsigned _T163A;struct Cyc_Absyn_Exp*_T163B;struct Cyc_Yystacktype*_T163C;union Cyc_YYSTYPE*_T163D;union Cyc_YYSTYPE*_T163E;struct Cyc_Absyn_Exp*_T163F;struct Cyc_Yystacktype*_T1640;struct Cyc_Yystacktype _T1641;struct Cyc_Yyltype _T1642;unsigned _T1643;unsigned _T1644;struct Cyc_Absyn_Exp*_T1645;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1646;struct Cyc_Yystacktype*_T1647;union Cyc_YYSTYPE*_T1648;union Cyc_YYSTYPE*_T1649;void*_T164A;struct Cyc_Yystacktype*_T164B;struct Cyc_Yystacktype _T164C;struct Cyc_Yyltype _T164D;unsigned _T164E;unsigned _T164F;struct Cyc_Absyn_Exp*_T1650;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1651;struct Cyc_Yystacktype*_T1652;union Cyc_YYSTYPE*_T1653;union Cyc_YYSTYPE*_T1654;struct Cyc_Yystacktype*_T1655;union Cyc_YYSTYPE*_T1656;union Cyc_YYSTYPE*_T1657;struct Cyc_List_List*_T1658;void*_T1659;struct Cyc_Yystacktype*_T165A;struct Cyc_Yystacktype _T165B;struct Cyc_Yyltype _T165C;unsigned _T165D;unsigned _T165E;struct Cyc_Absyn_Exp*_T165F;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1660;struct Cyc_Yystacktype*_T1661;union Cyc_YYSTYPE*_T1662;union Cyc_YYSTYPE*_T1663;struct Cyc_Yystacktype*_T1664;union Cyc_YYSTYPE*_T1665;union Cyc_YYSTYPE*_T1666;struct Cyc_List_List*_T1667;void*_T1668;struct Cyc_Yystacktype*_T1669;struct Cyc_Yystacktype _T166A;struct Cyc_Yyltype _T166B;unsigned _T166C;unsigned _T166D;struct Cyc_Absyn_Exp*_T166E;struct Cyc_List_List*_T166F;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1670;struct _fat_ptr*_T1671;struct Cyc_Yystacktype*_T1672;union Cyc_YYSTYPE*_T1673;union Cyc_YYSTYPE*_T1674;struct Cyc_Yystacktype*_T1675;struct Cyc_Yystacktype _T1676;struct Cyc_Yyltype _T1677;unsigned _T1678;unsigned _T1679;struct Cyc_Yystacktype*_T167A;union Cyc_YYSTYPE*_T167B;union Cyc_YYSTYPE*_T167C;union Cyc_Absyn_Cnst _T167D;unsigned _T167E;int _T167F;struct Cyc_List_List*_T1680;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1681;struct Cyc_List_List*_T1682;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1683;struct _fat_ptr*_T1684;struct Cyc_Yystacktype*_T1685;union Cyc_YYSTYPE*_T1686;union Cyc_YYSTYPE*_T1687;struct Cyc_Yystacktype*_T1688;union Cyc_YYSTYPE*_T1689;union Cyc_YYSTYPE*_T168A;struct Cyc_Yystacktype*_T168B;struct Cyc_Yystacktype _T168C;struct Cyc_Yyltype _T168D;unsigned _T168E;unsigned _T168F;struct Cyc_Yystacktype*_T1690;union Cyc_YYSTYPE*_T1691;union Cyc_YYSTYPE*_T1692;union Cyc_Absyn_Cnst _T1693;unsigned _T1694;int _T1695;struct Cyc_List_List*_T1696;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1697;struct Cyc_Yystacktype*_T1698;union Cyc_YYSTYPE*_T1699;union Cyc_YYSTYPE*_T169A;struct Cyc_Yystacktype*_T169B;union Cyc_YYSTYPE*_T169C;union Cyc_YYSTYPE*_T169D;struct _tuple0*_T169E;struct Cyc_Yystacktype*_T169F;struct Cyc_Yystacktype _T16A0;struct Cyc_Yyltype _T16A1;unsigned _T16A2;unsigned _T16A3;struct Cyc_Absyn_Exp*_T16A4;struct Cyc_Yystacktype*_T16A5;union Cyc_YYSTYPE*_T16A6;union Cyc_YYSTYPE*_T16A7;struct _fat_ptr _T16A8;struct Cyc_Yystacktype*_T16A9;struct Cyc_Yystacktype _T16AA;struct Cyc_Yyltype _T16AB;unsigned _T16AC;unsigned _T16AD;struct Cyc_Absyn_Exp*_T16AE;struct Cyc_Yystacktype*_T16AF;struct Cyc_Yystacktype _T16B0;struct Cyc_Yystacktype*_T16B1;union Cyc_YYSTYPE*_T16B2;union Cyc_YYSTYPE*_T16B3;struct _fat_ptr _T16B4;struct Cyc_Yystacktype*_T16B5;struct Cyc_Yystacktype _T16B6;struct Cyc_Yyltype _T16B7;unsigned _T16B8;unsigned _T16B9;struct Cyc_Absyn_Exp*_T16BA;struct Cyc_Yystacktype*_T16BB;union Cyc_YYSTYPE*_T16BC;union Cyc_YYSTYPE*_T16BD;struct _fat_ptr _T16BE;struct Cyc_Yystacktype*_T16BF;struct Cyc_Yystacktype _T16C0;struct Cyc_Yyltype _T16C1;unsigned _T16C2;unsigned _T16C3;struct Cyc_Absyn_Exp*_T16C4;struct Cyc_Yystacktype*_T16C5;struct Cyc_Yystacktype _T16C6;struct Cyc_Yystacktype*_T16C7;union Cyc_YYSTYPE*_T16C8;union Cyc_YYSTYPE*_T16C9;struct Cyc_Absyn_Exp*_T16CA;struct Cyc_Yystacktype*_T16CB;struct Cyc_Yystacktype _T16CC;struct Cyc_Yyltype _T16CD;unsigned _T16CE;unsigned _T16CF;struct Cyc_Absyn_Exp*_T16D0;struct Cyc_Yystacktype*_T16D1;union Cyc_YYSTYPE*_T16D2;union Cyc_YYSTYPE*_T16D3;struct Cyc_Absyn_Exp*_T16D4;struct Cyc_Yystacktype*_T16D5;union Cyc_YYSTYPE*_T16D6;union Cyc_YYSTYPE*_T16D7;struct Cyc_List_List*_T16D8;struct Cyc_List_List*_T16D9;struct Cyc_Yystacktype*_T16DA;struct Cyc_Yystacktype _T16DB;struct Cyc_Yyltype _T16DC;unsigned _T16DD;unsigned _T16DE;struct Cyc_Absyn_Exp*_T16DF;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T16E0;struct Cyc_Yystacktype*_T16E1;union Cyc_YYSTYPE*_T16E2;union Cyc_YYSTYPE*_T16E3;struct Cyc_Yystacktype*_T16E4;union Cyc_YYSTYPE*_T16E5;union Cyc_YYSTYPE*_T16E6;struct Cyc_Yystacktype*_T16E7;union Cyc_YYSTYPE*_T16E8;union Cyc_YYSTYPE*_T16E9;struct Cyc_List_List*_T16EA;void*_T16EB;struct Cyc_Yystacktype*_T16EC;struct Cyc_Yystacktype _T16ED;struct Cyc_Yyltype _T16EE;unsigned _T16EF;unsigned _T16F0;struct Cyc_Absyn_Exp*_T16F1;struct Cyc_Yystacktype*_T16F2;union Cyc_YYSTYPE*_T16F3;union Cyc_YYSTYPE*_T16F4;struct Cyc_List_List*_T16F5;struct Cyc_Yystacktype*_T16F6;struct Cyc_Yystacktype _T16F7;struct Cyc_Yyltype _T16F8;unsigned _T16F9;unsigned _T16FA;struct Cyc_Absyn_Exp*_T16FB;struct Cyc_Yystacktype*_T16FC;union Cyc_YYSTYPE*_T16FD;union Cyc_YYSTYPE*_T16FE;struct Cyc_Absyn_Stmt*_T16FF;struct Cyc_Yystacktype*_T1700;struct Cyc_Yystacktype _T1701;struct Cyc_Yyltype _T1702;unsigned _T1703;unsigned _T1704;struct Cyc_Absyn_Exp*_T1705;struct Cyc_Yystacktype*_T1706;union Cyc_YYSTYPE*_T1707;union Cyc_YYSTYPE*_T1708;struct Cyc_List_List*_T1709;struct Cyc_List_List*_T170A;struct Cyc_List_List*_T170B;struct Cyc_Yystacktype*_T170C;union Cyc_YYSTYPE*_T170D;union Cyc_YYSTYPE*_T170E;struct Cyc_List_List*_T170F;struct Cyc_Yystacktype*_T1710;union Cyc_YYSTYPE*_T1711;union Cyc_YYSTYPE*_T1712;struct Cyc_Yystacktype*_T1713;union Cyc_YYSTYPE*_T1714;union Cyc_YYSTYPE*_T1715;struct Cyc_Yystacktype*_T1716;union Cyc_YYSTYPE*_T1717;union Cyc_YYSTYPE*_T1718;union Cyc_Absyn_Cnst _T1719;struct Cyc_Yystacktype*_T171A;struct Cyc_Yystacktype _T171B;struct Cyc_Yyltype _T171C;unsigned _T171D;unsigned _T171E;struct Cyc_Absyn_Exp*_T171F;struct Cyc_Yystacktype*_T1720;union Cyc_YYSTYPE*_T1721;union Cyc_YYSTYPE*_T1722;char _T1723;struct Cyc_Yystacktype*_T1724;struct Cyc_Yystacktype _T1725;struct Cyc_Yyltype _T1726;unsigned _T1727;unsigned _T1728;struct Cyc_Absyn_Exp*_T1729;struct Cyc_Yystacktype*_T172A;union Cyc_YYSTYPE*_T172B;union Cyc_YYSTYPE*_T172C;struct _fat_ptr _T172D;struct Cyc_Yystacktype*_T172E;struct Cyc_Yystacktype _T172F;struct Cyc_Yyltype _T1730;unsigned _T1731;unsigned _T1732;struct Cyc_Absyn_Exp*_T1733;struct Cyc_Yystacktype*_T1734;struct Cyc_Yystacktype _T1735;struct Cyc_Yyltype _T1736;unsigned _T1737;unsigned _T1738;struct Cyc_Absyn_Exp*_T1739;struct Cyc_Yystacktype*_T173A;union Cyc_YYSTYPE*_T173B;union Cyc_YYSTYPE*_T173C;unsigned long _T173D;struct _fat_ptr _T173E;unsigned char*_T173F;const char*_T1740;const char*_T1741;int _T1742;char _T1743;int _T1744;char _T1745;int _T1746;char _T1747;int _T1748;char _T1749;int _T174A;struct _fat_ptr _T174B;int _T174C;struct Cyc_Yystacktype*_T174D;struct Cyc_Yystacktype _T174E;struct Cyc_Yyltype _T174F;unsigned _T1750;unsigned _T1751;struct Cyc_Absyn_Exp*_T1752;struct _tuple0*_T1753;struct _fat_ptr*_T1754;struct Cyc_Yystacktype*_T1755;union Cyc_YYSTYPE*_T1756;union Cyc_YYSTYPE*_T1757;struct Cyc_Yystacktype*_T1758;struct Cyc_Yystacktype _T1759;struct _tuple0*_T175A;struct _fat_ptr*_T175B;struct Cyc_Yystacktype*_T175C;union Cyc_YYSTYPE*_T175D;union Cyc_YYSTYPE*_T175E;struct Cyc_Yystacktype*_T175F;struct Cyc_Yystacktype _T1760;struct Cyc_Yystacktype*_T1761;struct Cyc_Yystacktype _T1762;struct Cyc_Yystacktype*_T1763;struct Cyc_Yystacktype _T1764;struct Cyc_Yystacktype*_T1765;struct Cyc_Yystacktype _T1766;struct Cyc_Yystacktype*_T1767;struct Cyc_Yystacktype _T1768;struct Cyc_Lexing_lexbuf*_T1769;struct Cyc_List_List*_T176A;struct _tuple35*_T176B;struct Cyc_Yystacktype*_T176C;union Cyc_YYSTYPE*_T176D;union Cyc_YYSTYPE*_T176E;struct _fat_ptr _T176F;struct Cyc_Yystacktype*_T1770;union Cyc_YYSTYPE*_T1771;union Cyc_YYSTYPE*_T1772;struct _fat_ptr _T1773;struct _fat_ptr _T1774;int _T1775;int _T1776;struct Cyc_Yystacktype*_T1777;union Cyc_YYSTYPE*_T1778;union Cyc_YYSTYPE*_T1779;struct _fat_ptr _T177A;void*_T177B;struct Cyc_Yystacktype*_T177C;union Cyc_YYSTYPE*_T177D;union Cyc_YYSTYPE*_T177E;struct Cyc_Yystacktype*_T177F;union Cyc_YYSTYPE*_T1780;union Cyc_YYSTYPE*_T1781;struct Cyc_Yystacktype*_T1782;union Cyc_YYSTYPE*_T1783;union Cyc_YYSTYPE*_T1784;struct Cyc_List_List*_T1785;struct Cyc_List_List*_T1786;struct Cyc_Yystacktype*_T1787;union Cyc_YYSTYPE*_T1788;union Cyc_YYSTYPE*_T1789;struct Cyc_List_List*_T178A;struct Cyc_Yystacktype*_T178B;union Cyc_YYSTYPE*_T178C;union Cyc_YYSTYPE*_T178D;struct Cyc_Yystacktype*_T178E;union Cyc_YYSTYPE*_T178F;union Cyc_YYSTYPE*_T1790;struct Cyc_Yystacktype*_T1791;union Cyc_YYSTYPE*_T1792;union Cyc_YYSTYPE*_T1793;struct _fat_ptr _T1794;void*_T1795;struct Cyc_Yystacktype*_T1796;struct Cyc_Yystacktype _T1797;struct Cyc_Yyltype _T1798;unsigned _T1799;unsigned _T179A;struct Cyc_Yystacktype*_T179B;union Cyc_YYSTYPE*_T179C;union Cyc_YYSTYPE*_T179D;struct _fat_ptr _T179E;void*_T179F;struct Cyc_Yystacktype*_T17A0;union Cyc_YYSTYPE*_T17A1;union Cyc_YYSTYPE*_T17A2;struct _fat_ptr _T17A3;struct Cyc_Yystacktype*_T17A4;union Cyc_YYSTYPE*_T17A5;union Cyc_YYSTYPE*_T17A6;void*_T17A7;void*_T17A8;void*_T17A9;struct Cyc_Yystacktype*_T17AA;union Cyc_YYSTYPE*_T17AB;union Cyc_YYSTYPE*_T17AC;struct _fat_ptr _T17AD;struct Cyc_Yystacktype*_T17AE;union Cyc_YYSTYPE*_T17AF;union Cyc_YYSTYPE*_T17B0;enum Cyc_Parse_ConstraintOps _T17B1;struct Cyc_Yystacktype*_T17B2;union Cyc_YYSTYPE*_T17B3;union Cyc_YYSTYPE*_T17B4;void*_T17B5;struct Cyc_Yystacktype*_T17B6;union Cyc_YYSTYPE*_T17B7;union Cyc_YYSTYPE*_T17B8;void*_T17B9;void*_T17BA;void*_T17BB;struct Cyc_Yystacktype*_T17BC;union Cyc_YYSTYPE*_T17BD;union Cyc_YYSTYPE*_T17BE;struct _fat_ptr _T17BF;struct Cyc_Yystacktype*_T17C0;union Cyc_YYSTYPE*_T17C1;union Cyc_YYSTYPE*_T17C2;void*_T17C3;struct Cyc_Yystacktype*_T17C4;union Cyc_YYSTYPE*_T17C5;union Cyc_YYSTYPE*_T17C6;void*_T17C7;void*_T17C8;void*_T17C9;struct Cyc_Yystacktype*_T17CA;union Cyc_YYSTYPE*_T17CB;union Cyc_YYSTYPE*_T17CC;struct _fat_ptr _T17CD;struct Cyc_Yystacktype*_T17CE;union Cyc_YYSTYPE*_T17CF;union Cyc_YYSTYPE*_T17D0;void*_T17D1;struct Cyc_Yystacktype*_T17D2;union Cyc_YYSTYPE*_T17D3;union Cyc_YYSTYPE*_T17D4;void*_T17D5;void*_T17D6;void*_T17D7;struct Cyc_Yystacktype*_T17D8;union Cyc_YYSTYPE*_T17D9;union Cyc_YYSTYPE*_T17DA;struct _fat_ptr _T17DB;struct Cyc_Yystacktype*_T17DC;union Cyc_YYSTYPE*_T17DD;union Cyc_YYSTYPE*_T17DE;void*_T17DF;struct Cyc_Yystacktype*_T17E0;union Cyc_YYSTYPE*_T17E1;union Cyc_YYSTYPE*_T17E2;void*_T17E3;void*_T17E4;void*_T17E5;struct Cyc_Yystacktype*_T17E6;union Cyc_YYSTYPE*_T17E7;union Cyc_YYSTYPE*_T17E8;struct _fat_ptr _T17E9;struct Cyc_Yystacktype*_T17EA;union Cyc_YYSTYPE*_T17EB;union Cyc_YYSTYPE*_T17EC;void*_T17ED;struct Cyc_Yystacktype*_T17EE;union Cyc_YYSTYPE*_T17EF;union Cyc_YYSTYPE*_T17F0;void*_T17F1;void*_T17F2;void*_T17F3;struct Cyc_Yystacktype*_T17F4;union Cyc_YYSTYPE*_T17F5;union Cyc_YYSTYPE*_T17F6;struct _fat_ptr _T17F7;struct Cyc_Yystacktype*_T17F8;union Cyc_YYSTYPE*_T17F9;union Cyc_YYSTYPE*_T17FA;enum Cyc_Parse_ConstraintOps _T17FB;struct Cyc_Yystacktype*_T17FC;union Cyc_YYSTYPE*_T17FD;union Cyc_YYSTYPE*_T17FE;void*_T17FF;struct Cyc_Yystacktype*_T1800;union Cyc_YYSTYPE*_T1801;union Cyc_YYSTYPE*_T1802;void*_T1803;void*_T1804;void*_T1805;int _T1806;int _T1807;struct _fat_ptr _T1808;int _T1809;unsigned char*_T180A;struct Cyc_Yystacktype*_T180B;struct _fat_ptr _T180C;int _T180D;struct _fat_ptr _T180E;unsigned char*_T180F;unsigned char*_T1810;struct Cyc_Yystacktype*_T1811;struct Cyc_Yyltype _T1812;struct Cyc_Yystacktype*_T1813;struct Cyc_Yyltype _T1814;struct Cyc_Yystacktype*_T1815;struct Cyc_Yystacktype*_T1816;struct Cyc_Yystacktype _T1817;struct Cyc_Yyltype _T1818;struct Cyc_Yystacktype*_T1819;struct Cyc_Yystacktype*_T181A;struct Cyc_Yystacktype _T181B;struct Cyc_Yyltype _T181C;struct _fat_ptr _T181D;unsigned char*_T181E;struct Cyc_Yystacktype*_T181F;int _T1820;struct _fat_ptr _T1821;int _T1822;int _T1823;unsigned char*_T1824;struct Cyc_Yystacktype*_T1825;struct Cyc_Yystacktype _T1826;struct Cyc_Yyltype _T1827;struct _fat_ptr _T1828;unsigned char*_T1829;struct Cyc_Yystacktype*_T182A;int _T182B;struct _fat_ptr _T182C;unsigned char*_T182D;struct Cyc_Yystacktype*_T182E;int _T182F;int _T1830;struct Cyc_Yystacktype _T1831;struct Cyc_Yyltype _T1832;short*_T1833;int _T1834;char*_T1835;short*_T1836;short _T1837;short*_T1838;int _T1839;char*_T183A;short*_T183B;short _T183C;int _T183D;struct _fat_ptr _T183E;int _T183F;unsigned char*_T1840;short*_T1841;short _T1842;int _T1843;short*_T1844;int _T1845;short _T1846;int _T1847;struct _fat_ptr _T1848;unsigned char*_T1849;short*_T184A;int _T184B;short _T184C;int _T184D;short*_T184E;int _T184F;short _T1850;short*_T1851;int _T1852;short _T1853;short*_T1854;int _T1855;char*_T1856;short*_T1857;short _T1858;int _T1859;int _T185A;int _T185B;int _T185C;unsigned _T185D;unsigned _T185E;short*_T185F;int _T1860;char*_T1861;short*_T1862;short _T1863;int _T1864;int _T1865;struct _fat_ptr*_T1866;int _T1867;char*_T1868;struct _fat_ptr*_T1869;struct _fat_ptr _T186A;unsigned long _T186B;unsigned long _T186C;struct _fat_ptr _T186D;int _T186E;unsigned _T186F;char*_T1870;struct _RegionHandle*_T1871;unsigned _T1872;unsigned _T1873;struct _fat_ptr _T1874;struct _fat_ptr _T1875;int _T1876;int _T1877;unsigned _T1878;unsigned _T1879;short*_T187A;int _T187B;char*_T187C;short*_T187D;short _T187E;int _T187F;int _T1880;struct _fat_ptr _T1881;struct _fat_ptr _T1882;struct _fat_ptr _T1883;struct _fat_ptr _T1884;struct _fat_ptr _T1885;struct _fat_ptr*_T1886;int _T1887;char*_T1888;struct _fat_ptr*_T1889;struct _fat_ptr _T188A;struct _fat_ptr _T188B;struct _fat_ptr _T188C;struct _fat_ptr _T188D;int _T188E;int _T188F;struct _fat_ptr _T1890;int _T1891;unsigned char*_T1892;short*_T1893;short _T1894;short*_T1895;int _T1896;char*_T1897;short*_T1898;short _T1899;int _T189A;int _T189B;short*_T189C;int _T189D;short _T189E;int _T189F;short*_T18A0;int _T18A1;short _T18A2;int _T18A3;int _T18A4;struct _fat_ptr _T18A5;int _T18A6;unsigned char*_T18A7;struct Cyc_Yystacktype*_T18A8;struct Cyc_Yystacktype _T18A9;struct _RegionHandle _T18AA=_new_region(0U,"yyregion");struct _RegionHandle*yyregion=& _T18AA;_push_region(yyregion);{
# 149
int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;{union Cyc_YYSTYPE _T18AB;_T18AB.YYINITIALSVAL.tag=1U;
_T18AB.YYINITIALSVAL.val=0;_T0=_T18AB;}{union Cyc_YYSTYPE yylval=_T0;
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;{unsigned _T18AB=200U;_T2=yyregion;_T3=_region_calloc(_T2,0U,sizeof(short),_T18AB);_T1=_tag_fat(_T3,sizeof(short),_T18AB);}{
# 164
struct _fat_ptr yyss=_T1;
# 166
int yyvsp_offset;{unsigned _T18AB=200U;_T6=yyregion;_T7=_check_times(_T18AB,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T18AC=_region_malloc(_T6,0U,_T7);{unsigned _T18AD=_T18AB;unsigned i;i=0;_TL250: if(i < _T18AD)goto _TL24E;else{goto _TL24F;}_TL24E: _T8=i;
# 169
_T18AC[_T8].v=yylval;_T9=i;_T18AC[_T9].l=Cyc_yynewloc();i=i + 1;goto _TL250;_TL24F:;}_T5=(struct Cyc_Yystacktype*)_T18AC;}_T4=_T5;}{
# 168
struct _fat_ptr yyvs=
_tag_fat(_T4,sizeof(struct Cyc_Yystacktype),200U);
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
yychar=- 2;
# 200
yyssp_offset=- 1;
yyvsp_offset=0;
# 206
yynewstate: _TA=yyss;
# 208
yyssp_offset=yyssp_offset + 1;_TB=yyssp_offset;_TC=_check_fat_subscript(_TA,sizeof(short),_TB);_TD=(short*)_TC;_TE=yystate;*_TD=(short)_TE;_TF=yyssp_offset;_T10=yystacksize - 1;_T11=_T10 - 14;
# 210
if(_TF < _T11)goto _TL251;
# 212
if(yystacksize < 30000)goto _TL253;_T12=
_tag_fat("parser stack overflow",sizeof(char),22U);_T13=yystate;_T14=yychar;Cyc_yyerror(_T12,_T13,_T14);_T15=& Cyc_Yystack_overflow_val;_T16=(struct Cyc_Yystack_overflow_exn_struct*)_T15;_throw(_T16);goto _TL254;_TL253: _TL254:
# 216
 yystacksize=yystacksize * 2;
if(yystacksize <= 30000)goto _TL255;
yystacksize=30000;goto _TL256;_TL255: _TL256: _T18=yystacksize;{unsigned _T18AB=(unsigned)_T18;_T1A=yyregion;_T1B=_check_times(_T18AB,sizeof(short));{short*_T18AC=_region_malloc(_T1A,0U,_T1B);{unsigned _T18AD=_T18AB;unsigned i;i=0;_TL25A: if(i < _T18AD)goto _TL258;else{goto _TL259;}_TL258: _T1C=i;_T1D=yyssp_offset;_T1E=(unsigned)_T1D;
# 220
if(_T1C > _T1E)goto _TL25B;_T1F=i;_T20=yyss;_T21=_T20.curr;_T22=(short*)_T21;_T23=i;_T24=(int)_T23;_T18AC[_T1F]=_T22[_T24];goto _TL25C;_TL25B: _T25=i;_T18AC[_T25]=0;_TL25C: i=i + 1;goto _TL25A;_TL259:;}_T19=(short*)_T18AC;}_T17=
# 219
_tag_fat(_T19,sizeof(short),_T18AB);}{struct _fat_ptr yyss1=_T17;_T27=yystacksize;{unsigned _T18AB=(unsigned)_T27;_T29=yyregion;_T2A=_check_times(_T18AB,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T18AC=_region_malloc(_T29,0U,_T2A);{unsigned _T18AD=_T18AB;unsigned i;i=0;_TL260: if(i < _T18AD)goto _TL25E;else{goto _TL25F;}_TL25E: _T2B=i;_T2C=yyssp_offset;_T2D=(unsigned)_T2C;
# 224
if(_T2B > _T2D)goto _TL261;_T2E=i;_T2F=yyvs;_T30=i;_T31=(int)_T30;_T32=_check_fat_subscript(_T2F,sizeof(struct Cyc_Yystacktype),_T31);_T33=(struct Cyc_Yystacktype*)_T32;_T18AC[_T2E]=*_T33;goto _TL262;_TL261: _T34=i;_T35=yyvs;_T36=_check_fat_subscript(_T35,sizeof(struct Cyc_Yystacktype),0);_T37=(struct Cyc_Yystacktype*)_T36;_T18AC[_T34]=*_T37;_TL262: i=i + 1;goto _TL260;_TL25F:;}_T28=(struct Cyc_Yystacktype*)_T18AC;}_T26=
# 223
_tag_fat(_T28,sizeof(struct Cyc_Yystacktype),_T18AB);}{
# 222
struct _fat_ptr yyvs1=_T26;
# 230
yyss=yyss1;
yyvs=yyvs1;}}goto _TL252;_TL251: _TL252: goto yybackup;
# 242
yybackup: _T38=Cyc_yypact;_T39=yystate;_T3A=_check_known_subscript_notnull(_T38,1269U,sizeof(short),_T39);_T3B=(short*)_T3A;_T3C=*_T3B;
# 254 "cycbison.simple"
yyn=(int)_T3C;_T3D=yyn;_T3E=- 32768;
if(_T3D!=_T3E)goto _TL263;goto yydefault;_TL263: _T3F=yychar;_T40=- 2;
# 261
if(_T3F!=_T40)goto _TL265;_T41=yylex_buf;_T42=& yylval;_T43=(union Cyc_YYSTYPE*)_T42;_T44=& yylloc;_T45=(struct Cyc_Yyltype*)_T44;
# 267
yychar=Cyc_yylex(_T41,_T43,_T45);goto _TL266;_TL265: _TL266:
# 271
 if(yychar > 0)goto _TL267;
# 273
yychar1=0;
yychar=0;goto _TL268;
# 282
_TL267: if(yychar <= 0)goto _TL269;if(yychar > 401)goto _TL269;_T47=Cyc_yytranslate;_T48=yychar;_T49=_T47[_T48];_T46=(int)_T49;goto _TL26A;_TL269: _T46=364;_TL26A: yychar1=_T46;_TL268: _T4A=yychar1;
# 299 "cycbison.simple"
yyn=yyn + _T4A;
if(yyn < 0)goto _TL26D;else{goto _TL26F;}_TL26F: if(yyn > 8463)goto _TL26D;else{goto _TL26E;}_TL26E: _T4B=Cyc_yycheck;_T4C=yyn;_T4D=_T4B[_T4C];_T4E=(int)_T4D;_T4F=yychar1;if(_T4E!=_T4F)goto _TL26D;else{goto _TL26B;}_TL26D: goto yydefault;_TL26B: _T50=Cyc_yytable;_T51=yyn;_T52=_T50[_T51];
# 302
yyn=(int)_T52;
# 309
if(yyn >= 0)goto _TL270;_T53=yyn;_T54=- 32768;
# 311
if(_T53!=_T54)goto _TL272;goto yyerrlab;_TL272:
 yyn=- yyn;goto yyreduce;
# 315
_TL270: if(yyn!=0)goto _TL274;goto yyerrlab;_TL274:
# 317
 if(yyn!=1268)goto _TL276;{int _T18AB=0;_npop_handler(0);return _T18AB;}_TL276:
# 328 "cycbison.simple"
 if(yychar==0)goto _TL278;
yychar=- 2;goto _TL279;_TL278: _TL279: _T55=yyvs;
# 332
yyvsp_offset=yyvsp_offset + 1;_T56=yyvsp_offset;_T57=_check_fat_subscript(_T55,sizeof(struct Cyc_Yystacktype),_T56);_T58=(struct Cyc_Yystacktype*)_T57;{struct Cyc_Yystacktype _T18AB;_T18AB.v=yylval;_T18AB.l=yylloc;_T59=_T18AB;}*_T58=_T59;
# 338
if(yyerrstatus==0)goto _TL27A;yyerrstatus=yyerrstatus + -1;goto _TL27B;_TL27A: _TL27B:
# 340
 yystate=yyn;goto yynewstate;
# 344
yydefault: _T5A=Cyc_yydefact;_T5B=yystate;_T5C=_check_known_subscript_notnull(_T5A,1269U,sizeof(short),_T5B);_T5D=(short*)_T5C;_T5E=*_T5D;
# 346
yyn=(int)_T5E;
if(yyn!=0)goto _TL27C;goto yyerrlab;_TL27C:
# 351
 yyreduce: _T5F=Cyc_yyr2;_T60=yyn;_T61=_check_known_subscript_notnull(_T5F,625U,sizeof(short),_T60);_T62=(short*)_T61;_T63=*_T62;
# 353
yylen=(int)_T63;_T64=yyvs;_T65=yyvsp_offset + 1;_T66=yylen;_T67=_T65 - _T66;_T68=
_fat_ptr_plus(_T64,sizeof(struct Cyc_Yystacktype),_T67);_T69=_untag_fat_ptr_check_bound(_T68,sizeof(struct Cyc_Yystacktype),14U);_T6A=_check_null(_T69);yyyvsp=(struct Cyc_Yystacktype*)_T6A;
if(yylen <= 0)goto _TL27E;_T6B=yyyvsp;_T6C=_T6B[0];
yyval=_T6C.v;goto _TL27F;_TL27E: _TL27F: _T6D=yyn;_T6E=(int)_T6D;switch(_T6E){case 1:
# 1324 "parse.y"
 yyval=Cyc_YY74(0);goto _LL0;case 2:
# 1327 "parse.y"
 yyval=Cyc_YY74(1);_T6F=yyyvsp;_T70=& _T6F[0].v;_T71=(union Cyc_YYSTYPE*)_T70;
Cyc_Parse_constraint_graph=Cyc_yyget_YY71(_T71);goto _LL0;case 3: _T72=yyyvsp;_T73=_T72[0];
# 1334 "parse.y"
yyval=_T73.v;_T74=yyyvsp;_T75=& _T74[0].v;_T76=(union Cyc_YYSTYPE*)_T75;
Cyc_Parse_parse_result=Cyc_yyget_YY16(_T76);goto _LL0;case 4: _T77=yyyvsp;_T78=& _T77[0].v;_T79=(union Cyc_YYSTYPE*)_T78;_T7A=
# 1340 "parse.y"
Cyc_yyget_YY16(_T79);_T7B=yyyvsp;_T7C=& _T7B[1].v;_T7D=(union Cyc_YYSTYPE*)_T7C;_T7E=Cyc_yyget_YY16(_T7D);_T7F=Cyc_List_imp_append(_T7A,_T7E);yyval=Cyc_YY16(_T7F);goto _LL0;case 5:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T18AD=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T18AD->tag=10;_T83=yyyvsp;_T84=& _T83[0].v;_T85=(union Cyc_YYSTYPE*)_T84;
# 1344 "parse.y"
_T18AD->f1=Cyc_yyget_QualId_tok(_T85);_T86=yyyvsp;_T87=& _T86[2].v;_T88=(union Cyc_YYSTYPE*)_T87;_T18AD->f2=Cyc_yyget_YY16(_T88);_T82=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T18AD;}_T18AC->r=(void*)_T82;_T89=yyyvsp;_T8A=_T89[0];_T8B=_T8A.l;_T8C=_T8B.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_T8C);_T81=(struct Cyc_Absyn_Decl*)_T18AC;}_T18AB->hd=_T81;_T18AB->tl=0;_T80=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T80);
Cyc_Lex_leave_using();goto _LL0;case 6:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T18AD=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T18AD->tag=10;_T90=yyyvsp;_T91=& _T90[0].v;_T92=(union Cyc_YYSTYPE*)_T91;
# 1348 "parse.y"
_T18AD->f1=Cyc_yyget_QualId_tok(_T92);_T93=yyyvsp;_T94=& _T93[2].v;_T95=(union Cyc_YYSTYPE*)_T94;_T18AD->f2=Cyc_yyget_YY16(_T95);_T8F=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T18AD;}_T18AC->r=(void*)_T8F;_T96=yyyvsp;_T97=_T96[0];_T98=_T97.l;_T99=_T98.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_T99);_T8E=(struct Cyc_Absyn_Decl*)_T18AC;}_T18AB->hd=_T8E;_T9A=yyyvsp;_T9B=& _T9A[4].v;_T9C=(union Cyc_YYSTYPE*)_T9B;_T18AB->tl=Cyc_yyget_YY16(_T9C);_T8D=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T8D);goto _LL0;case 7:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T18AD=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T18AD->tag=9;{struct _fat_ptr*_T18AE=_cycalloc(sizeof(struct _fat_ptr));_TA1=yyyvsp;_TA2=& _TA1[0].v;_TA3=(union Cyc_YYSTYPE*)_TA2;
# 1351
*_T18AE=Cyc_yyget_String_tok(_TA3);_TA0=(struct _fat_ptr*)_T18AE;}_T18AD->f1=_TA0;_TA4=yyyvsp;_TA5=& _TA4[2].v;_TA6=(union Cyc_YYSTYPE*)_TA5;_T18AD->f2=Cyc_yyget_YY16(_TA6);_T9F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T18AD;}_T18AC->r=(void*)_T9F;_TA7=yyyvsp;_TA8=_TA7[0];_TA9=_TA8.l;_TAA=_TA9.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_TAA);_T9E=(struct Cyc_Absyn_Decl*)_T18AC;}_T18AB->hd=_T9E;_T18AB->tl=0;_T9D=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T9D);
Cyc_Lex_leave_namespace();goto _LL0;case 8:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T18AD=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T18AD->tag=9;{struct _fat_ptr*_T18AE=_cycalloc(sizeof(struct _fat_ptr));_TAF=yyyvsp;_TB0=& _TAF[0].v;_TB1=(union Cyc_YYSTYPE*)_TB0;
# 1355 "parse.y"
*_T18AE=Cyc_yyget_String_tok(_TB1);_TAE=(struct _fat_ptr*)_T18AE;}_T18AD->f1=_TAE;_TB2=yyyvsp;_TB3=& _TB2[2].v;_TB4=(union Cyc_YYSTYPE*)_TB3;_T18AD->f2=Cyc_yyget_YY16(_TB4);_TAD=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T18AD;}_T18AC->r=(void*)_TAD;_TB5=yyyvsp;_TB6=_TB5[0];_TB7=_TB6.l;_TB8=_TB7.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_TB8);_TAC=(struct Cyc_Absyn_Decl*)_T18AC;}_T18AB->hd=_TAC;_TB9=yyyvsp;_TBA=& _TB9[4].v;_TBB=(union Cyc_YYSTYPE*)_TBA;_T18AB->tl=Cyc_yyget_YY16(_TBB);_TAB=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_TAB);goto _LL0;case 9: _TBC=yyyvsp;_TBD=& _TBC[2].v;_TBE=(union Cyc_YYSTYPE*)_TBD;_TBF=
# 1357 "parse.y"
Cyc_yyget_YY16(_TBE);_TC0=yyyvsp;_TC1=& _TC0[4].v;_TC2=(union Cyc_YYSTYPE*)_TC1;_TC3=Cyc_yyget_YY16(_TC2);_TC4=Cyc_List_append(_TBF,_TC3);yyval=Cyc_YY16(_TC4);goto _LL0;case 10: _TC5=yyyvsp;_TC6=& _TC5[0].v;_TC7=(union Cyc_YYSTYPE*)_TC6;{
# 1359 "parse.y"
int is_c_include=Cyc_yyget_YY32(_TC7);_TC8=yyyvsp;_TC9=& _TC8[4].v;_TCA=(union Cyc_YYSTYPE*)_TC9;{
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(_TCA);_TCB=yyyvsp;_TCC=& _TCB[5].v;_TCD=(union Cyc_YYSTYPE*)_TCC;{
struct _tuple30*_T18AB=Cyc_yyget_YY56(_TCD);unsigned _T18AC;struct Cyc_List_List*_T18AD;{struct _tuple30 _T18AE=*_T18AB;_T18AD=_T18AE.f0;_T18AC=_T18AE.f1;}{struct Cyc_List_List*exs=_T18AD;unsigned wc=_T18AC;_TCE=yyyvsp;_TCF=& _TCE[6].v;_TD0=(union Cyc_YYSTYPE*)_TCF;{
struct Cyc_List_List*hides=Cyc_yyget_YY57(_TD0);
if(exs==0)goto _TL281;if(hides==0)goto _TL281;_TD1=yyyvsp;_TD2=_TD1[0];_TD3=_TD2.l;_TD4=_TD3.first_line;_TD5=
Cyc_Position_loc_to_seg(_TD4);_TD6=_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U);_TD7=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TD5,_TD6,_TD7);goto _TL282;_TL281: _TL282: _TD8=hides;_TD9=(unsigned)_TD8;
if(!_TD9)goto _TL283;_TDA=wc;_TDB=(int)_TDA;if(_TDB)goto _TL283;else{goto _TL285;}
_TL285: _TDC=yyyvsp;_TDD=_TDC[6];_TDE=_TDD.l;_TDF=_TDE.first_line;wc=Cyc_Position_loc_to_seg(_TDF);goto _TL284;_TL283: _TL284: _TE0=is_c_include;
if(_TE0)goto _TL286;else{goto _TL288;}
_TL288: if(exs!=0)goto _TL28B;else{goto _TL28C;}_TL28C: if(cycdecls!=0)goto _TL28B;else{goto _TL289;}
_TL28B: _TE1=yyyvsp;_TE2=_TE1[0];_TE3=_TE2.l;_TE4=_TE3.first_line;_TE5=Cyc_Position_loc_to_seg(_TE4);_TE6=_tag_fat("expecting \"C include\"",sizeof(char),22U);_TE7=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TE5,_TE6,_TE7);{struct Cyc_List_List*_T18AE=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AF=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T18B0=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T18B0->tag=12;_TEB=yyyvsp;_TEC=& _TEB[2].v;_TED=(union Cyc_YYSTYPE*)_TEC;
_T18B0->f1=Cyc_yyget_YY16(_TED);_T18B0->f2=cycdecls;_T18B0->f3=exs;{struct _tuple10*_T18B1=_cycalloc(sizeof(struct _tuple10));_T18B1->f0=wc;_T18B1->f1=hides;_TEE=(struct _tuple10*)_T18B1;}_T18B0->f4=_TEE;_TEA=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T18B0;}_T18AF->r=(void*)_TEA;_TEF=yyyvsp;_TF0=_TEF[0];_TF1=_TF0.l;_TF2=_TF1.first_line;_T18AF->loc=Cyc_Position_loc_to_seg(_TF2);_TE9=(struct Cyc_Absyn_Decl*)_T18AF;}_T18AE->hd=_TE9;_TF3=yyyvsp;_TF4=& _TF3[7].v;_TF5=(union Cyc_YYSTYPE*)_TF4;_T18AE->tl=Cyc_yyget_YY16(_TF5);_TE8=(struct Cyc_List_List*)_T18AE;}yyval=Cyc_YY16(_TE8);goto _TL28A;
# 1373
_TL289:{struct Cyc_List_List*_T18AE=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AF=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T18B0=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_T18B0->tag=11;_TF9=yyyvsp;_TFA=& _TF9[2].v;_TFB=(union Cyc_YYSTYPE*)_TFA;_T18B0->f1=Cyc_yyget_YY16(_TFB);_TF8=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T18B0;}_T18AF->r=(void*)_TF8;_TFC=yyyvsp;_TFD=_TFC[0];_TFE=_TFD.l;_TFF=_TFE.first_line;_T18AF->loc=Cyc_Position_loc_to_seg(_TFF);_TF7=(struct Cyc_Absyn_Decl*)_T18AF;}_T18AE->hd=_TF7;_T100=yyyvsp;_T101=& _T100[7].v;_T102=(union Cyc_YYSTYPE*)_T101;_T18AE->tl=Cyc_yyget_YY16(_T102);_TF6=(struct Cyc_List_List*)_T18AE;}yyval=Cyc_YY16(_TF6);_TL28A: goto _TL287;
# 1376
_TL286:{struct Cyc_List_List*_T18AE=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AF=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T18B0=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T18B0->tag=12;_T106=yyyvsp;_T107=& _T106[2].v;_T108=(union Cyc_YYSTYPE*)_T107;_T18B0->f1=Cyc_yyget_YY16(_T108);_T18B0->f2=cycdecls;_T18B0->f3=exs;{struct _tuple10*_T18B1=_cycalloc(sizeof(struct _tuple10));_T18B1->f0=wc;_T18B1->f1=hides;_T109=(struct _tuple10*)_T18B1;}_T18B0->f4=_T109;_T105=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T18B0;}_T18AF->r=(void*)_T105;_T10A=yyyvsp;_T10B=_T10A[0];_T10C=_T10B.l;_T10D=_T10C.first_line;_T18AF->loc=Cyc_Position_loc_to_seg(_T10D);_T104=(struct Cyc_Absyn_Decl*)_T18AF;}_T18AE->hd=_T104;_T10E=yyyvsp;_T10F=& _T10E[7].v;_T110=(union Cyc_YYSTYPE*)_T10F;_T18AE->tl=Cyc_yyget_YY16(_T110);_T103=(struct Cyc_List_List*)_T18AE;}yyval=Cyc_YY16(_T103);_TL287: goto _LL0;}}}}}case 11:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T113=& Cyc_Absyn_Porton_d_val;_T114=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_T113;
# 1379 "parse.y"
_T18AC->r=(void*)_T114;_T115=yyyvsp;_T116=_T115[0];_T117=_T116.l;_T118=_T117.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_T118);_T112=(struct Cyc_Absyn_Decl*)_T18AC;}_T18AB->hd=_T112;_T119=yyyvsp;_T11A=& _T119[2].v;_T11B=(union Cyc_YYSTYPE*)_T11A;_T18AB->tl=Cyc_yyget_YY16(_T11B);_T111=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T111);goto _LL0;case 12:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T11E=& Cyc_Absyn_Portoff_d_val;_T11F=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_T11E;
# 1381 "parse.y"
_T18AC->r=(void*)_T11F;_T120=yyyvsp;_T121=_T120[0];_T122=_T121.l;_T123=_T122.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_T123);_T11D=(struct Cyc_Absyn_Decl*)_T18AC;}_T18AB->hd=_T11D;_T124=yyyvsp;_T125=& _T124[2].v;_T126=(union Cyc_YYSTYPE*)_T125;_T18AB->tl=Cyc_yyget_YY16(_T126);_T11C=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T11C);goto _LL0;case 13:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T129=& Cyc_Absyn_Tempeston_d_val;_T12A=(struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*)_T129;
# 1383 "parse.y"
_T18AC->r=(void*)_T12A;_T12B=yyyvsp;_T12C=_T12B[0];_T12D=_T12C.l;_T12E=_T12D.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_T12E);_T128=(struct Cyc_Absyn_Decl*)_T18AC;}_T18AB->hd=_T128;_T12F=yyyvsp;_T130=& _T12F[2].v;_T131=(union Cyc_YYSTYPE*)_T130;_T18AB->tl=Cyc_yyget_YY16(_T131);_T127=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T127);goto _LL0;case 14:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T134=& Cyc_Absyn_Tempestoff_d_val;_T135=(struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*)_T134;
# 1385 "parse.y"
_T18AC->r=(void*)_T135;_T136=yyyvsp;_T137=_T136[0];_T138=_T137.l;_T139=_T138.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_T139);_T133=(struct Cyc_Absyn_Decl*)_T18AC;}_T18AB->hd=_T133;_T13A=yyyvsp;_T13B=& _T13A[2].v;_T13C=(union Cyc_YYSTYPE*)_T13B;_T18AB->tl=Cyc_yyget_YY16(_T13C);_T132=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T132);goto _LL0;case 15:
# 1386 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 16:
# 1390 "parse.y"
 Cyc_Parse_parsing_tempest=1;goto _LL0;case 17:
# 1393
 Cyc_Parse_parsing_tempest=0;goto _LL0;case 18:
# 1398 "parse.y"
 Cyc_Lex_enter_extern_c();_T13D=yyyvsp;_T13E=& _T13D[1].v;_T13F=(union Cyc_YYSTYPE*)_T13E;_T140=
Cyc_yyget_String_tok(_T13F);_T141=_tag_fat("C",sizeof(char),2U);_T142=Cyc_strcmp(_T140,_T141);if(_T142!=0)goto _TL28D;
yyval=Cyc_YY32(0);goto _TL28E;
_TL28D: _T143=yyyvsp;_T144=& _T143[1].v;_T145=(union Cyc_YYSTYPE*)_T144;_T146=Cyc_yyget_String_tok(_T145);_T147=_tag_fat("C include",sizeof(char),10U);_T148=Cyc_strcmp(_T146,_T147);if(_T148!=0)goto _TL28F;
yyval=Cyc_YY32(1);goto _TL290;
# 1404
_TL28F: _T149=yyyvsp;_T14A=_T149[0];_T14B=_T14A.l;_T14C=_T14B.first_line;_T14D=Cyc_Position_loc_to_seg(_T14C);_T14E=_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U);_T14F=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T14D,_T14E,_T14F);
yyval=Cyc_YY32(1);_TL290: _TL28E: goto _LL0;case 19:
# 1411 "parse.y"
 Cyc_Lex_leave_extern_c();goto _LL0;case 20:
# 1415 "parse.y"
 yyval=Cyc_YY57(0);goto _LL0;case 21: _T150=yyyvsp;_T151=_T150[2];
# 1416 "parse.y"
yyval=_T151.v;goto _LL0;case 22:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T153=yyyvsp;_T154=& _T153[0].v;_T155=(union Cyc_YYSTYPE*)_T154;
# 1420 "parse.y"
_T18AB->hd=Cyc_yyget_QualId_tok(_T155);_T18AB->tl=0;_T152=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY57(_T152);goto _LL0;case 23:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T157=yyyvsp;_T158=& _T157[0].v;_T159=(union Cyc_YYSTYPE*)_T158;
# 1421 "parse.y"
_T18AB->hd=Cyc_yyget_QualId_tok(_T159);_T18AB->tl=0;_T156=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY57(_T156);goto _LL0;case 24:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T15B=yyyvsp;_T15C=& _T15B[0].v;_T15D=(union Cyc_YYSTYPE*)_T15C;
# 1422 "parse.y"
_T18AB->hd=Cyc_yyget_QualId_tok(_T15D);_T15E=yyyvsp;_T15F=& _T15E[2].v;_T160=(union Cyc_YYSTYPE*)_T15F;_T18AB->tl=Cyc_yyget_YY57(_T160);_T15A=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY57(_T15A);goto _LL0;case 25:{struct _tuple30*_T18AB=_cycalloc(sizeof(struct _tuple30));
# 1426 "parse.y"
_T18AB->f0=0;_T18AB->f1=0U;_T161=(struct _tuple30*)_T18AB;}yyval=Cyc_YY56(_T161);goto _LL0;case 26: _T162=yyyvsp;_T163=_T162[0];
# 1427 "parse.y"
yyval=_T163.v;goto _LL0;case 27:{struct _tuple30*_T18AB=_cycalloc(sizeof(struct _tuple30));_T165=yyyvsp;_T166=& _T165[2].v;_T167=(union Cyc_YYSTYPE*)_T166;
# 1431 "parse.y"
_T18AB->f0=Cyc_yyget_YY55(_T167);_T18AB->f1=0U;_T164=(struct _tuple30*)_T18AB;}yyval=Cyc_YY56(_T164);goto _LL0;case 28:{struct _tuple30*_T18AB=_cycalloc(sizeof(struct _tuple30));
# 1432 "parse.y"
_T18AB->f0=0;_T18AB->f1=0U;_T168=(struct _tuple30*)_T18AB;}yyval=Cyc_YY56(_T168);goto _LL0;case 29:{struct _tuple30*_T18AB=_cycalloc(sizeof(struct _tuple30));
# 1433 "parse.y"
_T18AB->f0=0;_T16A=yyyvsp;_T16B=_T16A[0];_T16C=_T16B.l;_T16D=_T16C.first_line;_T18AB->f1=Cyc_Position_loc_to_seg(_T16D);_T169=(struct _tuple30*)_T18AB;}yyval=Cyc_YY56(_T169);goto _LL0;case 30:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T18AC=_cycalloc(sizeof(struct _tuple33));_T170=yyyvsp;_T171=_T170[0];_T172=_T171.l;_T173=_T172.first_line;
# 1438 "parse.y"
_T18AC->f0=Cyc_Position_loc_to_seg(_T173);_T174=yyyvsp;_T175=& _T174[0].v;_T176=(union Cyc_YYSTYPE*)_T175;_T18AC->f1=Cyc_yyget_QualId_tok(_T176);_T18AC->f2=0;_T16F=(struct _tuple33*)_T18AC;}_T18AB->hd=_T16F;_T18AB->tl=0;_T16E=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY55(_T16E);goto _LL0;case 31:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T18AC=_cycalloc(sizeof(struct _tuple33));_T179=yyyvsp;_T17A=_T179[0];_T17B=_T17A.l;_T17C=_T17B.first_line;
# 1440 "parse.y"
_T18AC->f0=Cyc_Position_loc_to_seg(_T17C);_T17D=yyyvsp;_T17E=& _T17D[0].v;_T17F=(union Cyc_YYSTYPE*)_T17E;_T18AC->f1=Cyc_yyget_QualId_tok(_T17F);_T18AC->f2=0;_T178=(struct _tuple33*)_T18AC;}_T18AB->hd=_T178;_T180=yyyvsp;_T181=& _T180[2].v;_T182=(union Cyc_YYSTYPE*)_T181;_T18AB->tl=Cyc_yyget_YY55(_T182);_T177=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY55(_T177);goto _LL0;case 32:
# 1444 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 33: _T183=yyyvsp;_T184=_T183[2];
# 1445 "parse.y"
yyval=_T184.v;goto _LL0;case 34:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T18AC->tag=1;_T187=yyyvsp;_T188=& _T187[0].v;_T189=(union Cyc_YYSTYPE*)_T188;
# 1449 "parse.y"
_T18AC->f1=Cyc_yyget_YY15(_T189);_T186=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T18AC;}_T18A=(void*)_T186;_T18B=yyyvsp;_T18C=_T18B[0];_T18D=_T18C.l;_T18E=_T18D.first_line;_T18F=Cyc_Position_loc_to_seg(_T18E);_T18AB->hd=Cyc_Absyn_new_decl(_T18A,_T18F);_T18AB->tl=0;_T185=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T185);goto _LL0;case 35: _T190=yyyvsp;_T191=_T190[0];
# 1450 "parse.y"
yyval=_T191.v;goto _LL0;case 36:
# 1451 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 39: _T192=yyr;_T193=yyyvsp;_T194=& _T193[0].v;_T195=(union Cyc_YYSTYPE*)_T194;_T196=
# 1485 "parse.y"
Cyc_yyget_YY28(_T195);_T197=yyyvsp;_T198=& _T197[1].v;_T199=(union Cyc_YYSTYPE*)_T198;_T19A=Cyc_yyget_Stmt_tok(_T199);_T19B=yyyvsp;_T19C=_T19B[0];_T19D=_T19C.l;_T19E=_T19D.first_line;_T19F=Cyc_Position_loc_to_seg(_T19E);_T1A0=Cyc_Parse_make_function(_T192,0,_T196,0,_T19A,_T19F);yyval=Cyc_YY15(_T1A0);goto _LL0;case 40: _T1A1=yyyvsp;_T1A2=& _T1A1[0].v;_T1A3=(union Cyc_YYSTYPE*)_T1A2;{
# 1487 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1A3);_T1A4=yyr;_T1A5=& d;_T1A6=(struct Cyc_Parse_Declaration_spec*)_T1A5;_T1A7=yyyvsp;_T1A8=& _T1A7[1].v;_T1A9=(union Cyc_YYSTYPE*)_T1A8;_T1AA=
Cyc_yyget_YY28(_T1A9);_T1AB=yyyvsp;_T1AC=& _T1AB[2].v;_T1AD=(union Cyc_YYSTYPE*)_T1AC;_T1AE=Cyc_yyget_Stmt_tok(_T1AD);_T1AF=yyyvsp;_T1B0=_T1AF[0];_T1B1=_T1B0.l;_T1B2=_T1B1.first_line;_T1B3=Cyc_Position_loc_to_seg(_T1B2);_T1B4=Cyc_Parse_make_function(_T1A4,_T1A6,_T1AA,0,_T1AE,_T1B3);yyval=Cyc_YY15(_T1B4);goto _LL0;}case 41: _T1B5=yyr;_T1B6=yyyvsp;_T1B7=& _T1B6[0].v;_T1B8=(union Cyc_YYSTYPE*)_T1B7;_T1B9=
# 1499 "parse.y"
Cyc_yyget_YY28(_T1B8);_T1BA=yyyvsp;_T1BB=& _T1BA[1].v;_T1BC=(union Cyc_YYSTYPE*)_T1BB;_T1BD=Cyc_yyget_YY16(_T1BC);_T1BE=yyyvsp;_T1BF=& _T1BE[2].v;_T1C0=(union Cyc_YYSTYPE*)_T1BF;_T1C1=Cyc_yyget_Stmt_tok(_T1C0);_T1C2=yyyvsp;_T1C3=_T1C2[0];_T1C4=_T1C3.l;_T1C5=_T1C4.first_line;_T1C6=Cyc_Position_loc_to_seg(_T1C5);_T1C7=Cyc_Parse_make_function(_T1B5,0,_T1B9,_T1BD,_T1C1,_T1C6);yyval=Cyc_YY15(_T1C7);goto _LL0;case 42: _T1C8=yyyvsp;_T1C9=& _T1C8[0].v;_T1CA=(union Cyc_YYSTYPE*)_T1C9;{
# 1501 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1CA);_T1CB=yyr;_T1CC=& d;_T1CD=(struct Cyc_Parse_Declaration_spec*)_T1CC;_T1CE=yyyvsp;_T1CF=& _T1CE[1].v;_T1D0=(union Cyc_YYSTYPE*)_T1CF;_T1D1=
Cyc_yyget_YY28(_T1D0);_T1D2=yyyvsp;_T1D3=& _T1D2[2].v;_T1D4=(union Cyc_YYSTYPE*)_T1D3;_T1D5=Cyc_yyget_YY16(_T1D4);_T1D6=yyyvsp;_T1D7=& _T1D6[3].v;_T1D8=(union Cyc_YYSTYPE*)_T1D7;_T1D9=Cyc_yyget_Stmt_tok(_T1D8);_T1DA=yyyvsp;_T1DB=_T1DA[0];_T1DC=_T1DB.l;_T1DD=_T1DC.first_line;_T1DE=Cyc_Position_loc_to_seg(_T1DD);_T1DF=Cyc_Parse_make_function(_T1CB,_T1CD,_T1D1,_T1D5,_T1D9,_T1DE);yyval=Cyc_YY15(_T1DF);goto _LL0;}case 43: _T1E0=yyyvsp;_T1E1=& _T1E0[0].v;_T1E2=(union Cyc_YYSTYPE*)_T1E1;{
# 1509 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1E2);_T1E3=yyr;_T1E4=& d;_T1E5=(struct Cyc_Parse_Declaration_spec*)_T1E4;_T1E6=yyyvsp;_T1E7=& _T1E6[1].v;_T1E8=(union Cyc_YYSTYPE*)_T1E7;_T1E9=
Cyc_yyget_YY28(_T1E8);_T1EA=yyyvsp;_T1EB=& _T1EA[2].v;_T1EC=(union Cyc_YYSTYPE*)_T1EB;_T1ED=Cyc_yyget_Stmt_tok(_T1EC);_T1EE=yyyvsp;_T1EF=_T1EE[0];_T1F0=_T1EF.l;_T1F1=_T1F0.first_line;_T1F2=Cyc_Position_loc_to_seg(_T1F1);_T1F3=Cyc_Parse_make_function(_T1E3,_T1E5,_T1E9,0,_T1ED,_T1F2);yyval=Cyc_YY15(_T1F3);goto _LL0;}case 44: _T1F4=yyyvsp;_T1F5=& _T1F4[0].v;_T1F6=(union Cyc_YYSTYPE*)_T1F5;{
# 1512 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1F6);_T1F7=yyr;_T1F8=& d;_T1F9=(struct Cyc_Parse_Declaration_spec*)_T1F8;_T1FA=yyyvsp;_T1FB=& _T1FA[1].v;_T1FC=(union Cyc_YYSTYPE*)_T1FB;_T1FD=
Cyc_yyget_YY28(_T1FC);_T1FE=yyyvsp;_T1FF=& _T1FE[2].v;_T200=(union Cyc_YYSTYPE*)_T1FF;_T201=Cyc_yyget_YY16(_T200);_T202=yyyvsp;_T203=& _T202[3].v;_T204=(union Cyc_YYSTYPE*)_T203;_T205=Cyc_yyget_Stmt_tok(_T204);_T206=yyyvsp;_T207=_T206[0];_T208=_T207.l;_T209=_T208.first_line;_T20A=Cyc_Position_loc_to_seg(_T209);_T20B=Cyc_Parse_make_function(_T1F7,_T1F9,_T1FD,_T201,_T205,_T20A);yyval=Cyc_YY15(_T20B);goto _LL0;}case 45: _T20C=yyyvsp;_T20D=& _T20C[1].v;_T20E=(union Cyc_YYSTYPE*)_T20D;_T20F=
# 1517 "parse.y"
Cyc_yyget_QualId_tok(_T20E);Cyc_Lex_enter_using(_T20F);_T210=yyyvsp;_T211=_T210[1];yyval=_T211.v;goto _LL0;case 46:
# 1520
 Cyc_Lex_leave_using();goto _LL0;case 47:{struct _fat_ptr*_T18AB=_cycalloc(sizeof(struct _fat_ptr));_T213=yyyvsp;_T214=& _T213[1].v;_T215=(union Cyc_YYSTYPE*)_T214;
# 1523
*_T18AB=Cyc_yyget_String_tok(_T215);_T212=(struct _fat_ptr*)_T18AB;}Cyc_Lex_enter_namespace(_T212);_T216=yyyvsp;_T217=_T216[1];yyval=_T217.v;goto _LL0;case 48:
# 1526
 Cyc_Lex_leave_namespace();goto _LL0;case 49:
# 1529
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + 1;goto _LL0;case 50:
# 1532
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + -1;goto _LL0;case 51: _T218=yyyvsp;_T219=& _T218[0].v;_T21A=(union Cyc_YYSTYPE*)_T219;_T21B=
# 1537 "parse.y"
Cyc_yyget_YY17(_T21A);_T21C=yyyvsp;_T21D=_T21C[0];_T21E=_T21D.l;_T21F=_T21E.first_line;_T220=Cyc_Position_loc_to_seg(_T21F);_T221=yyyvsp;_T222=_T221[0];_T223=_T222.l;_T224=_T223.first_line;_T225=Cyc_Position_loc_to_seg(_T224);_T226=Cyc_Parse_make_declarations(_T21B,0,_T220,_T225);yyval=Cyc_YY16(_T226);goto _LL0;case 52: _T227=yyyvsp;_T228=& _T227[0].v;_T229=(union Cyc_YYSTYPE*)_T228;_T22A=
# 1539 "parse.y"
Cyc_yyget_YY17(_T229);{struct _tuple11*(*_T18AB)(struct _tuple11*)=(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;_T22B=_T18AB;}_T22C=yyyvsp;_T22D=& _T22C[1].v;_T22E=(union Cyc_YYSTYPE*)_T22D;_T22F=Cyc_yyget_YY19(_T22E);_T230=_T22B(_T22F);_T231=yyyvsp;_T232=_T231[0];_T233=_T232.l;_T234=_T233.first_line;_T235=Cyc_Position_loc_to_seg(_T234);_T236=yyyvsp;_T237=_T236[0];_T238=_T237.l;_T239=_T238.first_line;_T23A=Cyc_Position_loc_to_seg(_T239);_T23B=Cyc_Parse_make_declarations(_T22A,_T230,_T235,_T23A);yyval=Cyc_YY16(_T23B);goto _LL0;case 53:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T23D=yyyvsp;_T23E=& _T23D[1].v;_T23F=(union Cyc_YYSTYPE*)_T23E;_T240=
# 1542
Cyc_yyget_YY9(_T23F);_T241=yyyvsp;_T242=& _T241[3].v;_T243=(union Cyc_YYSTYPE*)_T242;_T244=Cyc_yyget_Exp_tok(_T243);_T245=yyyvsp;_T246=_T245[0];_T247=_T246.l;_T248=_T247.first_line;_T249=Cyc_Position_loc_to_seg(_T248);_T18AB->hd=Cyc_Absyn_let_decl(_T240,_T244,_T249);_T18AB->tl=0;_T23C=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T23C);goto _LL0;case 54:  {
# 1544 "parse.y"
struct Cyc_List_List*vds=0;_T24A=yyyvsp;_T24B=& _T24A[1].v;_T24C=(union Cyc_YYSTYPE*)_T24B;{
struct Cyc_List_List*ids=Cyc_yyget_YY37(_T24C);_TL294: if(ids!=0)goto _TL292;else{goto _TL293;}
_TL292:{struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0));_T24D=qv;_T24D->f0=Cyc_Absyn_Rel_n(0);_T24E=qv;_T24F=ids;_T250=_T24F->hd;_T24E->f1=(struct _fat_ptr*)_T250;{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T252=qv;_T253=
Cyc_Absyn_wildtyp(0);_T18AB->hd=Cyc_Absyn_new_vardecl(0U,_T252,_T253,0,0);_T18AB->tl=vds;_T251=(struct Cyc_List_List*)_T18AB;}vds=_T251;}_T254=ids;
# 1545
ids=_T254->tl;goto _TL294;_TL293:;}{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T256=
# 1549
Cyc_List_imp_rev(vds);_T257=yyyvsp;_T258=_T257[0];_T259=_T258.l;_T25A=_T259.first_line;_T25B=Cyc_Position_loc_to_seg(_T25A);_T18AB->hd=Cyc_Absyn_letv_decl(_T256,_T25B);_T18AB->tl=0;_T255=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T255);goto _LL0;}case 55: _T25C=yyyvsp;_T25D=& _T25C[2].v;_T25E=(union Cyc_YYSTYPE*)_T25D;_T25F=
# 1554 "parse.y"
Cyc_yyget_String_tok(_T25E);_T260=yyyvsp;_T261=_T260[2];_T262=_T261.l;_T263=_T262.first_line;_T264=Cyc_Position_loc_to_seg(_T263);Cyc_Parse_tvar_ok(_T25F,_T264);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T265=tv;{struct _fat_ptr*_T18AB=_cycalloc(sizeof(struct _fat_ptr));_T267=yyyvsp;_T268=& _T267[2].v;_T269=(union Cyc_YYSTYPE*)_T268;*_T18AB=Cyc_yyget_String_tok(_T269);_T266=(struct _fat_ptr*)_T18AB;}_T265->name=_T266;_T26A=tv;_T26A->identity=- 1;_T26B=tv;_T26C=& Cyc_Kinds_ek;_T26D=(struct Cyc_Absyn_Kind*)_T26C;_T26B->kind=Cyc_Kinds_kind_to_bound(_T26D);_T26E=tv;_T26E->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T26F=yyyvsp;_T270=_T26F[4];_T271=_T270.l;_T272=_T271.first_line;_T273=
Cyc_Position_loc_to_seg(_T272);{struct _tuple0*_T18AB=_cycalloc(sizeof(struct _tuple0));_T18AB->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T276=yyyvsp;_T277=& _T276[4].v;_T278=(union Cyc_YYSTYPE*)_T277;*_T18AC=Cyc_yyget_String_tok(_T278);_T275=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_T275;_T274=(struct _tuple0*)_T18AB;}_T279=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T273,_T274,_T279,0,0);{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T27B=tv;_T27C=vd;_T27D=yyyvsp;_T27E=_T27D[0];_T27F=_T27E.l;_T280=_T27F.first_line;_T281=
Cyc_Position_loc_to_seg(_T280);_T18AB->hd=Cyc_Absyn_region_decl(_T27B,_T27C,0,_T281);_T18AB->tl=0;_T27A=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T27A);goto _LL0;}}}case 56:{struct Cyc_String_pa_PrintArg_struct _T18AB;_T18AB.tag=0;_T284=yyyvsp;_T285=& _T284[1].v;_T286=(union Cyc_YYSTYPE*)_T285;
# 1562
_T18AB.f1=Cyc_yyget_String_tok(_T286);_T283=_T18AB;}{struct Cyc_String_pa_PrintArg_struct _T18AB=_T283;void*_T18AC[1];_T18AC[0]=& _T18AB;_T287=_tag_fat("`%s",sizeof(char),4U);_T288=_tag_fat(_T18AC,sizeof(void*),1);_T282=Cyc_aprintf(_T287,_T288);}{struct _fat_ptr tvstring=_T282;_T289=yyyvsp;_T28A=& _T289[1].v;_T28B=(union Cyc_YYSTYPE*)_T28A;_T28C=
Cyc_yyget_String_tok(_T28B);_T28D=yyyvsp;_T28E=_T28D[1];_T28F=_T28E.l;_T290=_T28F.first_line;_T291=Cyc_Position_loc_to_seg(_T290);Cyc_Parse_tvar_ok(_T28C,_T291);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T292=tv;{struct _fat_ptr*_T18AB=_cycalloc(sizeof(struct _fat_ptr));*_T18AB=tvstring;_T293=(struct _fat_ptr*)_T18AB;}_T292->name=_T293;_T294=tv;_T294->identity=- 1;_T295=tv;_T296=& Cyc_Kinds_ek;_T297=(struct Cyc_Absyn_Kind*)_T296;_T295->kind=Cyc_Kinds_kind_to_bound(_T297);_T298=tv;_T298->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T299=yyyvsp;_T29A=_T299[1];_T29B=_T29A.l;_T29C=_T29B.first_line;_T29D=
Cyc_Position_loc_to_seg(_T29C);{struct _tuple0*_T18AB=_cycalloc(sizeof(struct _tuple0));_T18AB->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T2A0=yyyvsp;_T2A1=& _T2A0[1].v;_T2A2=(union Cyc_YYSTYPE*)_T2A1;*_T18AC=Cyc_yyget_String_tok(_T2A2);_T29F=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_T29F;_T29E=(struct _tuple0*)_T18AB;}_T2A3=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T29D,_T29E,_T2A3,0,0);{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T2A5=tv;_T2A6=vd;_T2A7=yyyvsp;_T2A8=& _T2A7[2].v;_T2A9=(union Cyc_YYSTYPE*)_T2A8;_T2AA=
Cyc_yyget_YY61(_T2A9);_T2AB=yyyvsp;_T2AC=_T2AB[0];_T2AD=_T2AC.l;_T2AE=_T2AD.first_line;_T2AF=Cyc_Position_loc_to_seg(_T2AE);_T18AB->hd=Cyc_Absyn_region_decl(_T2A5,_T2A6,_T2AA,_T2AF);_T18AB->tl=0;_T2A4=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY16(_T2A4);goto _LL0;}}}}case 57:
# 1572 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 58: _T2B0=yyyvsp;_T2B1=& _T2B0[1].v;_T2B2=(union Cyc_YYSTYPE*)_T2B1;_T2B3=
# 1574 "parse.y"
Cyc_yyget_String_tok(_T2B2);_T2B4=_tag_fat("open",sizeof(char),5U);_T2B5=Cyc_strcmp(_T2B3,_T2B4);if(_T2B5==0)goto _TL295;_T2B6=yyyvsp;_T2B7=_T2B6[3];_T2B8=_T2B7.l;_T2B9=_T2B8.first_line;_T2BA=Cyc_Position_loc_to_seg(_T2B9);_T2BB=_tag_fat("expecting `open'",sizeof(char),17U);_T2BC=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2BA,_T2BB,_T2BC);goto _TL296;_TL295: _TL296: _T2BD=yyyvsp;_T2BE=& _T2BD[3].v;_T2BF=(union Cyc_YYSTYPE*)_T2BE;_T2C0=
Cyc_yyget_Exp_tok(_T2BF);yyval=Cyc_YY61(_T2C0);goto _LL0;case 59: _T2C1=yyyvsp;_T2C2=_T2C1[0];
# 1578
yyval=_T2C2.v;goto _LL0;case 60: _T2C3=yyyvsp;_T2C4=& _T2C3[0].v;_T2C5=(union Cyc_YYSTYPE*)_T2C4;_T2C6=
# 1579 "parse.y"
Cyc_yyget_YY16(_T2C5);_T2C7=yyyvsp;_T2C8=& _T2C7[1].v;_T2C9=(union Cyc_YYSTYPE*)_T2C8;_T2CA=Cyc_yyget_YY16(_T2C9);_T2CB=Cyc_List_imp_append(_T2C6,_T2CA);yyval=Cyc_YY16(_T2CB);goto _LL0;case 61:{struct Cyc_Parse_Declaration_spec _T18AB;_T2CD=yyyvsp;_T2CE=& _T2CD[0].v;_T2CF=(union Cyc_YYSTYPE*)_T2CE;
# 1585 "parse.y"
_T18AB.sc=Cyc_yyget_YY20(_T2CF);_T2D0=yyyvsp;_T2D1=_T2D0[0];_T2D2=_T2D1.l;_T2D3=_T2D2.first_line;_T2D4=Cyc_Position_loc_to_seg(_T2D3);_T18AB.tq=Cyc_Absyn_empty_tqual(_T2D4);
_T18AB.type_specs=Cyc_Parse_empty_spec(0U);_T18AB.is_inline=0;_T18AB.attributes=0;_T2CC=_T18AB;}
# 1585
yyval=Cyc_YY17(_T2CC);goto _LL0;case 62: _T2D5=yyyvsp;_T2D6=& _T2D5[1].v;_T2D7=(union Cyc_YYSTYPE*)_T2D6;{
# 1588 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T2D7);_T2D8=two;_T2D9=_T2D8.sc;_T2DA=(int)_T2D9;
if(_T2DA==7)goto _TL297;_T2DB=yyyvsp;_T2DC=_T2DB[0];_T2DD=_T2DC.l;_T2DE=_T2DD.first_line;_T2DF=
Cyc_Position_loc_to_seg(_T2DE);_T2E0=
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U);_T2E1=_tag_fat(0U,sizeof(void*),0);
# 1590
Cyc_Warn_warn(_T2DF,_T2E0,_T2E1);goto _TL298;_TL297: _TL298:{struct Cyc_Parse_Declaration_spec _T18AB;_T2E3=yyyvsp;_T2E4=& _T2E3[0].v;_T2E5=(union Cyc_YYSTYPE*)_T2E4;
# 1592
_T18AB.sc=Cyc_yyget_YY20(_T2E5);_T2E6=two;_T18AB.tq=_T2E6.tq;_T2E7=two;_T18AB.type_specs=_T2E7.type_specs;_T2E8=two;
_T18AB.is_inline=_T2E8.is_inline;_T2E9=two;_T18AB.attributes=_T2E9.attributes;_T2E2=_T18AB;}
# 1592
yyval=Cyc_YY17(_T2E2);goto _LL0;}case 63: _T2EA=yyyvsp;_T2EB=_T2EA[0];_T2EC=_T2EB.l;_T2ED=_T2EC.first_line;_T2EE=
# 1596 "parse.y"
Cyc_Position_loc_to_seg(_T2ED);_T2EF=_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U);_T2F0=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T2EE,_T2EF,_T2F0);_T2F1=yyyvsp;_T2F2=_T2F1[1];
yyval=_T2F2.v;goto _LL0;case 64:{struct Cyc_Parse_Declaration_spec _T18AB;
# 1600 "parse.y"
_T18AB.sc=7U;_T2F4=yyyvsp;_T2F5=_T2F4[0];_T2F6=_T2F5.l;_T2F7=_T2F6.first_line;_T2F8=Cyc_Position_loc_to_seg(_T2F7);_T18AB.tq=Cyc_Absyn_empty_tqual(_T2F8);_T2F9=yyyvsp;_T2FA=& _T2F9[0].v;_T2FB=(union Cyc_YYSTYPE*)_T2FA;
_T18AB.type_specs=Cyc_yyget_YY21(_T2FB);_T18AB.is_inline=0;_T18AB.attributes=0;_T2F3=_T18AB;}
# 1600
yyval=Cyc_YY17(_T2F3);goto _LL0;case 65: _T2FC=yyyvsp;_T2FD=& _T2FC[1].v;_T2FE=(union Cyc_YYSTYPE*)_T2FD;{
# 1603 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T2FE);{struct Cyc_Parse_Declaration_spec _T18AB;_T300=two;
_T18AB.sc=_T300.sc;_T301=two;_T18AB.tq=_T301.tq;_T302=yyyvsp;_T303=_T302[0];_T304=_T303.l;_T305=_T304.first_line;_T306=
Cyc_Position_loc_to_seg(_T305);_T307=two;_T308=_T307.type_specs;_T309=yyyvsp;_T30A=& _T309[0].v;_T30B=(union Cyc_YYSTYPE*)_T30A;_T30C=Cyc_yyget_YY21(_T30B);_T18AB.type_specs=Cyc_Parse_combine_specifiers(_T306,_T308,_T30C);_T30D=two;
_T18AB.is_inline=_T30D.is_inline;_T30E=two;_T18AB.attributes=_T30E.attributes;_T2FF=_T18AB;}
# 1604
yyval=Cyc_YY17(_T2FF);goto _LL0;}case 66:{struct Cyc_Parse_Declaration_spec _T18AB;
# 1609 "parse.y"
_T18AB.sc=7U;_T310=yyyvsp;_T311=& _T310[0].v;_T312=(union Cyc_YYSTYPE*)_T311;_T18AB.tq=Cyc_yyget_YY24(_T312);_T18AB.type_specs=Cyc_Parse_empty_spec(0U);_T18AB.is_inline=0;_T18AB.attributes=0;_T30F=_T18AB;}yyval=Cyc_YY17(_T30F);goto _LL0;case 67: _T313=yyyvsp;_T314=& _T313[1].v;_T315=(union Cyc_YYSTYPE*)_T314;{
# 1611 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T315);{struct Cyc_Parse_Declaration_spec _T18AB;_T317=two;
_T18AB.sc=_T317.sc;_T318=yyyvsp;_T319=& _T318[0].v;_T31A=(union Cyc_YYSTYPE*)_T319;_T31B=Cyc_yyget_YY24(_T31A);_T31C=two;_T31D=_T31C.tq;_T18AB.tq=Cyc_Absyn_combine_tqual(_T31B,_T31D);_T31E=two;
_T18AB.type_specs=_T31E.type_specs;_T31F=two;_T18AB.is_inline=_T31F.is_inline;_T320=two;_T18AB.attributes=_T320.attributes;_T316=_T18AB;}
# 1612
yyval=Cyc_YY17(_T316);goto _LL0;}case 68:{struct Cyc_Parse_Declaration_spec _T18AB;
# 1616 "parse.y"
_T18AB.sc=7U;_T322=yyyvsp;_T323=_T322[0];_T324=_T323.l;_T325=_T324.first_line;_T326=Cyc_Position_loc_to_seg(_T325);_T18AB.tq=Cyc_Absyn_empty_tqual(_T326);
_T18AB.type_specs=Cyc_Parse_empty_spec(0U);_T18AB.is_inline=1;_T18AB.attributes=0;_T321=_T18AB;}
# 1616
yyval=Cyc_YY17(_T321);goto _LL0;case 69: _T327=yyyvsp;_T328=& _T327[1].v;_T329=(union Cyc_YYSTYPE*)_T328;{
# 1619 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T329);{struct Cyc_Parse_Declaration_spec _T18AB;_T32B=two;
_T18AB.sc=_T32B.sc;_T32C=two;_T18AB.tq=_T32C.tq;_T32D=two;_T18AB.type_specs=_T32D.type_specs;_T18AB.is_inline=1;_T32E=two;_T18AB.attributes=_T32E.attributes;_T32A=_T18AB;}yyval=Cyc_YY17(_T32A);goto _LL0;}case 70:{struct Cyc_Parse_Declaration_spec _T18AB;
# 1623 "parse.y"
_T18AB.sc=7U;_T330=yyyvsp;_T331=_T330[0];_T332=_T331.l;_T333=_T332.first_line;_T334=Cyc_Position_loc_to_seg(_T333);_T18AB.tq=Cyc_Absyn_empty_tqual(_T334);
_T18AB.type_specs=Cyc_Parse_empty_spec(0U);_T18AB.is_inline=0;_T335=yyyvsp;_T336=& _T335[0].v;_T337=(union Cyc_YYSTYPE*)_T336;_T18AB.attributes=Cyc_yyget_YY46(_T337);_T32F=_T18AB;}
# 1623
yyval=Cyc_YY17(_T32F);goto _LL0;case 71: _T338=yyyvsp;_T339=& _T338[1].v;_T33A=(union Cyc_YYSTYPE*)_T339;{
# 1626 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T33A);{struct Cyc_Parse_Declaration_spec _T18AB;_T33C=two;
_T18AB.sc=_T33C.sc;_T33D=two;_T18AB.tq=_T33D.tq;_T33E=two;
_T18AB.type_specs=_T33E.type_specs;_T33F=two;_T18AB.is_inline=_T33F.is_inline;_T340=yyyvsp;_T341=& _T340[0].v;_T342=(union Cyc_YYSTYPE*)_T341;_T343=
Cyc_yyget_YY46(_T342);_T344=two;_T345=_T344.attributes;_T18AB.attributes=Cyc_List_imp_append(_T343,_T345);_T33B=_T18AB;}
# 1627
yyval=Cyc_YY17(_T33B);goto _LL0;}case 72:
# 1633 "parse.y"
 yyval=Cyc_YY20(4U);goto _LL0;case 73:
# 1634 "parse.y"
 yyval=Cyc_YY20(5U);goto _LL0;case 74:
# 1635 "parse.y"
 yyval=Cyc_YY20(3U);goto _LL0;case 75:
# 1636 "parse.y"
 yyval=Cyc_YY20(1U);goto _LL0;case 76: _T346=yyyvsp;_T347=& _T346[1].v;_T348=(union Cyc_YYSTYPE*)_T347;_T349=
# 1638 "parse.y"
Cyc_yyget_String_tok(_T348);_T34A=_tag_fat("C",sizeof(char),2U);_T34B=Cyc_strcmp(_T349,_T34A);if(_T34B==0)goto _TL299;_T34C=yyyvsp;_T34D=_T34C[0];_T34E=_T34D.l;_T34F=_T34E.first_line;_T350=
Cyc_Position_loc_to_seg(_T34F);_T351=_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U);_T352=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T350,_T351,_T352);goto _TL29A;_TL299: _TL29A:
 yyval=Cyc_YY20(2U);goto _LL0;case 77:
# 1642 "parse.y"
 yyval=Cyc_YY20(0U);goto _LL0;case 78:
# 1644 "parse.y"
 yyval=Cyc_YY20(6U);goto _LL0;case 79:
# 1649 "parse.y"
 yyval=Cyc_YY46(0);goto _LL0;case 80: _T353=yyyvsp;_T354=_T353[0];
# 1650 "parse.y"
yyval=_T354.v;goto _LL0;case 81: _T355=yyyvsp;_T356=_T355[3];
# 1654 "parse.y"
yyval=_T356.v;goto _LL0;case 82:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T358=yyyvsp;_T359=& _T358[0].v;_T35A=(union Cyc_YYSTYPE*)_T359;
# 1657
_T18AB->hd=Cyc_yyget_YY47(_T35A);_T18AB->tl=0;_T357=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY46(_T357);goto _LL0;case 83:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T35C=yyyvsp;_T35D=& _T35C[0].v;_T35E=(union Cyc_YYSTYPE*)_T35D;
# 1658 "parse.y"
_T18AB->hd=Cyc_yyget_YY47(_T35E);_T35F=yyyvsp;_T360=& _T35F[2].v;_T361=(union Cyc_YYSTYPE*)_T360;_T18AB->tl=Cyc_yyget_YY46(_T361);_T35B=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY46(_T35B);goto _LL0;case 84: _T362=yyyvsp;_T363=_T362[0];_T364=_T363.l;_T365=_T364.first_line;_T366=
# 1661
Cyc_Position_loc_to_seg(_T365);_T367=yyyvsp;_T368=& _T367[0].v;_T369=(union Cyc_YYSTYPE*)_T368;_T36A=Cyc_yyget_String_tok(_T369);_T36B=Cyc_Atts_parse_nullary_att(_T366,_T36A);yyval=Cyc_YY47(_T36B);goto _LL0;case 85: _T36C=& Cyc_Atts_Const_att_val;_T36D=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_T36C;_T36E=(void*)_T36D;
# 1662 "parse.y"
yyval=Cyc_YY47(_T36E);goto _LL0;case 86: _T36F=yyyvsp;_T370=_T36F[0];_T371=_T370.l;_T372=_T371.first_line;_T373=
# 1664 "parse.y"
Cyc_Position_loc_to_seg(_T372);_T374=yyyvsp;_T375=& _T374[0].v;_T376=(union Cyc_YYSTYPE*)_T375;_T377=Cyc_yyget_String_tok(_T376);_T378=yyyvsp;_T379=_T378[2];_T37A=_T379.l;_T37B=_T37A.first_line;_T37C=Cyc_Position_loc_to_seg(_T37B);_T37D=yyyvsp;_T37E=& _T37D[2].v;_T37F=(union Cyc_YYSTYPE*)_T37E;_T380=Cyc_yyget_Exp_tok(_T37F);_T381=Cyc_Atts_parse_unary_att(_T373,_T377,_T37C,_T380);yyval=Cyc_YY47(_T381);goto _LL0;case 87: _T382=yyyvsp;_T383=_T382[0];_T384=_T383.l;_T385=_T384.first_line;_T386=
# 1666 "parse.y"
Cyc_Position_loc_to_seg(_T385);_T387=yyyvsp;_T388=_T387[2];_T389=_T388.l;_T38A=_T389.first_line;_T38B=Cyc_Position_loc_to_seg(_T38A);_T38C=yyyvsp;_T38D=& _T38C[0].v;_T38E=(union Cyc_YYSTYPE*)_T38D;_T38F=Cyc_yyget_String_tok(_T38E);_T390=yyyvsp;_T391=& _T390[2].v;_T392=(union Cyc_YYSTYPE*)_T391;_T393=Cyc_yyget_String_tok(_T392);_T394=yyyvsp;_T395=_T394[4];_T396=_T395.l;_T397=_T396.first_line;_T398=
Cyc_Position_loc_to_seg(_T397);_T399=yyyvsp;_T39A=& _T399[4].v;_T39B=(union Cyc_YYSTYPE*)_T39A;_T39C=Cyc_yyget_Int_tok(_T39B);_T39D=Cyc_Parse_cnst2uint(_T398,_T39C);_T39E=yyyvsp;_T39F=_T39E[6];_T3A0=_T39F.l;_T3A1=_T3A0.first_line;_T3A2=
Cyc_Position_loc_to_seg(_T3A1);_T3A3=yyyvsp;_T3A4=& _T3A3[6].v;_T3A5=(union Cyc_YYSTYPE*)_T3A4;_T3A6=Cyc_yyget_Int_tok(_T3A5);_T3A7=Cyc_Parse_cnst2uint(_T3A2,_T3A6);_T3A8=
# 1666
Cyc_Atts_parse_format_att(_T386,_T38B,_T38F,_T393,_T39D,_T3A7);yyval=Cyc_YY47(_T3A8);goto _LL0;case 88: _T3A9=yyyvsp;_T3AA=_T3A9[0];
# 1678 "parse.y"
yyval=_T3AA.v;goto _LL0;case 89: _T3AB=yyyvsp;_T3AC=& _T3AB[0].v;_T3AD=(union Cyc_YYSTYPE*)_T3AC;_T3AE=
# 1680 "parse.y"
Cyc_yyget_QualId_tok(_T3AD);_T3AF=yyyvsp;_T3B0=& _T3AF[1].v;_T3B1=(union Cyc_YYSTYPE*)_T3B0;_T3B2=Cyc_yyget_YY41(_T3B1);_T3B3=Cyc_Absyn_typedef_type(_T3AE,_T3B2,0,0);_T3B4=yyyvsp;_T3B5=_T3B4[0];_T3B6=_T3B5.l;_T3B7=_T3B6.first_line;_T3B8=Cyc_Position_loc_to_seg(_T3B7);_T3B9=Cyc_Parse_type_spec(_T3B3,_T3B8);yyval=Cyc_YY21(_T3B9);goto _LL0;case 90: _T3BA=Cyc_Absyn_void_type;_T3BB=yyyvsp;_T3BC=_T3BB[0];_T3BD=_T3BC.l;_T3BE=_T3BD.first_line;_T3BF=
# 1684 "parse.y"
Cyc_Position_loc_to_seg(_T3BE);_T3C0=Cyc_Parse_type_spec(_T3BA,_T3BF);yyval=Cyc_YY21(_T3C0);goto _LL0;case 91: _T3C1=Cyc_Absyn_char_type;_T3C2=yyyvsp;_T3C3=_T3C2[0];_T3C4=_T3C3.l;_T3C5=_T3C4.first_line;_T3C6=
# 1685 "parse.y"
Cyc_Position_loc_to_seg(_T3C5);_T3C7=Cyc_Parse_type_spec(_T3C1,_T3C6);yyval=Cyc_YY21(_T3C7);goto _LL0;case 92: _T3C8=yyyvsp;_T3C9=_T3C8[0];_T3CA=_T3C9.l;_T3CB=_T3CA.first_line;_T3CC=
# 1686 "parse.y"
Cyc_Position_loc_to_seg(_T3CB);_T3CD=Cyc_Parse_short_spec(_T3CC);yyval=Cyc_YY21(_T3CD);goto _LL0;case 93: _T3CE=Cyc_Absyn_sint_type;_T3CF=yyyvsp;_T3D0=_T3CF[0];_T3D1=_T3D0.l;_T3D2=_T3D1.first_line;_T3D3=
# 1687 "parse.y"
Cyc_Position_loc_to_seg(_T3D2);_T3D4=Cyc_Parse_type_spec(_T3CE,_T3D3);yyval=Cyc_YY21(_T3D4);goto _LL0;case 94: _T3D5=yyyvsp;_T3D6=_T3D5[0];_T3D7=_T3D6.l;_T3D8=_T3D7.first_line;_T3D9=
# 1688 "parse.y"
Cyc_Position_loc_to_seg(_T3D8);_T3DA=Cyc_Parse_long_spec(_T3D9);yyval=Cyc_YY21(_T3DA);goto _LL0;case 95: _T3DB=Cyc_Absyn_float_type;_T3DC=yyyvsp;_T3DD=_T3DC[0];_T3DE=_T3DD.l;_T3DF=_T3DE.first_line;_T3E0=
# 1689 "parse.y"
Cyc_Position_loc_to_seg(_T3DF);_T3E1=Cyc_Parse_type_spec(_T3DB,_T3E0);yyval=Cyc_YY21(_T3E1);goto _LL0;case 96: _T3E2=Cyc_Absyn_double_type;_T3E3=yyyvsp;_T3E4=_T3E3[0];_T3E5=_T3E4.l;_T3E6=_T3E5.first_line;_T3E7=
# 1690 "parse.y"
Cyc_Position_loc_to_seg(_T3E6);_T3E8=Cyc_Parse_type_spec(_T3E2,_T3E7);yyval=Cyc_YY21(_T3E8);goto _LL0;case 97: _T3E9=yyyvsp;_T3EA=_T3E9[0];_T3EB=_T3EA.l;_T3EC=_T3EB.first_line;_T3ED=
# 1691 "parse.y"
Cyc_Position_loc_to_seg(_T3EC);_T3EE=Cyc_Parse_signed_spec(_T3ED);yyval=Cyc_YY21(_T3EE);goto _LL0;case 98: _T3EF=yyyvsp;_T3F0=_T3EF[0];_T3F1=_T3F0.l;_T3F2=_T3F1.first_line;_T3F3=
# 1692 "parse.y"
Cyc_Position_loc_to_seg(_T3F2);_T3F4=Cyc_Parse_unsigned_spec(_T3F3);yyval=Cyc_YY21(_T3F4);goto _LL0;case 99: _T3F5=yyyvsp;_T3F6=_T3F5[0];_T3F7=_T3F6.l;_T3F8=_T3F7.first_line;_T3F9=
# 1693 "parse.y"
Cyc_Position_loc_to_seg(_T3F8);_T3FA=Cyc_Parse_complex_spec(_T3F9);yyval=Cyc_YY21(_T3FA);goto _LL0;case 100: _T3FB=yyyvsp;_T3FC=_T3FB[0];
# 1694 "parse.y"
yyval=_T3FC.v;goto _LL0;case 101: _T3FD=yyyvsp;_T3FE=_T3FD[0];
# 1695 "parse.y"
yyval=_T3FE.v;goto _LL0;case 102: _T3FF=yyyvsp;_T400=& _T3FF[2].v;_T401=(union Cyc_YYSTYPE*)_T400;_T402=
# 1697 "parse.y"
Cyc_yyget_Exp_tok(_T401);_T403=Cyc_Absyn_typeof_type(_T402);_T404=yyyvsp;_T405=_T404[0];_T406=_T405.l;_T407=_T406.first_line;_T408=Cyc_Position_loc_to_seg(_T407);_T409=Cyc_Parse_type_spec(_T403,_T408);yyval=Cyc_YY21(_T409);goto _LL0;case 103: _T40A=
# 1699 "parse.y"
_tag_fat("__builtin_va_list",sizeof(char),18U);_T40B=& Cyc_Kinds_bk;_T40C=(struct Cyc_Absyn_Kind*)_T40B;_T40D=Cyc_Absyn_builtin_type(_T40A,_T40C);_T40E=yyyvsp;_T40F=_T40E[0];_T410=_T40F.l;_T411=_T410.first_line;_T412=Cyc_Position_loc_to_seg(_T411);_T413=Cyc_Parse_type_spec(_T40D,_T412);yyval=Cyc_YY21(_T413);goto _LL0;case 104: _T414=yyyvsp;_T415=_T414[0];
# 1701 "parse.y"
yyval=_T415.v;goto _LL0;case 105: _T416=yyyvsp;_T417=& _T416[0].v;_T418=(union Cyc_YYSTYPE*)_T417;_T419=
# 1703 "parse.y"
Cyc_yyget_YY45(_T418);_T41A=yyyvsp;_T41B=_T41A[0];_T41C=_T41B.l;_T41D=_T41C.first_line;_T41E=Cyc_Position_loc_to_seg(_T41D);_T41F=Cyc_Parse_type_spec(_T419,_T41E);yyval=Cyc_YY21(_T41F);goto _LL0;case 106: _T420=
# 1705 "parse.y"
Cyc_Absyn_new_evar(0,0);_T421=yyyvsp;_T422=_T421[0];_T423=_T422.l;_T424=_T423.first_line;_T425=Cyc_Position_loc_to_seg(_T424);_T426=Cyc_Parse_type_spec(_T420,_T425);yyval=Cyc_YY21(_T426);goto _LL0;case 107: _T427=yyyvsp;_T428=& _T427[2].v;_T429=(union Cyc_YYSTYPE*)_T428;_T42A=
# 1707 "parse.y"
Cyc_yyget_YY44(_T429);_T42B=Cyc_Kinds_kind_to_opt(_T42A);_T42C=Cyc_Absyn_new_evar(_T42B,0);_T42D=yyyvsp;_T42E=_T42D[0];_T42F=_T42E.l;_T430=_T42F.first_line;_T431=Cyc_Position_loc_to_seg(_T430);_T432=Cyc_Parse_type_spec(_T42C,_T431);yyval=Cyc_YY21(_T432);goto _LL0;case 108: _T434=Cyc_List_map_c;{
# 1709 "parse.y"
struct Cyc_List_List*(*_T18AB)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T434;_T433=_T18AB;}{struct _tuple19*(*_T18AB)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T435=_T18AB;}_T436=yyyvsp;_T437=_T436[2];_T438=_T437.l;_T439=_T438.first_line;_T43A=Cyc_Position_loc_to_seg(_T439);_T43B=yyyvsp;_T43C=& _T43B[2].v;_T43D=(union Cyc_YYSTYPE*)_T43C;_T43E=Cyc_yyget_YY39(_T43D);_T43F=Cyc_List_imp_rev(_T43E);_T440=_T433(_T435,_T43A,_T43F);_T441=Cyc_Absyn_tuple_type(_T440);_T442=yyyvsp;_T443=_T442[0];_T444=_T443.l;_T445=_T444.first_line;_T446=
Cyc_Position_loc_to_seg(_T445);_T447=
# 1709
Cyc_Parse_type_spec(_T441,_T446);yyval=Cyc_YY21(_T447);goto _LL0;case 109: _T448=yyyvsp;_T449=& _T448[2].v;_T44A=(union Cyc_YYSTYPE*)_T449;_T44B=
# 1712 "parse.y"
Cyc_yyget_YY45(_T44A);_T44C=Cyc_Absyn_rgn_handle_type(_T44B);_T44D=yyyvsp;_T44E=_T44D[0];_T44F=_T44E.l;_T450=_T44F.first_line;_T451=Cyc_Position_loc_to_seg(_T450);_T452=Cyc_Parse_type_spec(_T44C,_T451);yyval=Cyc_YY21(_T452);goto _LL0;case 110: _T453=& Cyc_Kinds_eko;_T454=(struct Cyc_Core_Opt*)_T453;_T455=
# 1714 "parse.y"
Cyc_Absyn_new_evar(_T454,0);_T456=Cyc_Absyn_rgn_handle_type(_T455);_T457=yyyvsp;_T458=_T457[0];_T459=_T458.l;_T45A=_T459.first_line;_T45B=Cyc_Position_loc_to_seg(_T45A);_T45C=Cyc_Parse_type_spec(_T456,_T45B);yyval=Cyc_YY21(_T45C);goto _LL0;case 111: _T45D=yyyvsp;_T45E=& _T45D[2].v;_T45F=(union Cyc_YYSTYPE*)_T45E;_T460=
# 1716 "parse.y"
Cyc_yyget_YY58(_T45F);_T461=Cyc_Absyn_aqual_handle_type(_T460);_T462=yyyvsp;_T463=_T462[0];_T464=_T463.l;_T465=_T464.first_line;_T466=Cyc_Position_loc_to_seg(_T465);_T467=Cyc_Parse_type_spec(_T461,_T466);yyval=Cyc_YY21(_T467);goto _LL0;case 112: _T468=yyyvsp;_T469=& _T468[2].v;_T46A=(union Cyc_YYSTYPE*)_T469;_T46B=
# 1718 "parse.y"
Cyc_yyget_YY45(_T46A);_T46C=Cyc_Absyn_tag_type(_T46B);_T46D=yyyvsp;_T46E=_T46D[0];_T46F=_T46E.l;_T470=_T46F.first_line;_T471=Cyc_Position_loc_to_seg(_T470);_T472=Cyc_Parse_type_spec(_T46C,_T471);yyval=Cyc_YY21(_T472);goto _LL0;case 113: _T473=& Cyc_Kinds_iko;_T474=(struct Cyc_Core_Opt*)_T473;_T475=
# 1720 "parse.y"
Cyc_Absyn_new_evar(_T474,0);_T476=Cyc_Absyn_tag_type(_T475);_T477=yyyvsp;_T478=_T477[0];_T479=_T478.l;_T47A=_T479.first_line;_T47B=Cyc_Position_loc_to_seg(_T47A);_T47C=Cyc_Parse_type_spec(_T476,_T47B);yyval=Cyc_YY21(_T47C);goto _LL0;case 114: _T47D=yyyvsp;_T47E=& _T47D[2].v;_T47F=(union Cyc_YYSTYPE*)_T47E;_T480=
# 1722 "parse.y"
Cyc_yyget_Exp_tok(_T47F);_T481=Cyc_Absyn_valueof_type(_T480);_T482=yyyvsp;_T483=_T482[0];_T484=_T483.l;_T485=_T484.first_line;_T486=Cyc_Position_loc_to_seg(_T485);_T487=Cyc_Parse_type_spec(_T481,_T486);yyval=Cyc_YY21(_T487);goto _LL0;case 115: _T488=yyyvsp;_T489=& _T488[2].v;_T48A=(union Cyc_YYSTYPE*)_T489;{
# 1725
struct _tuple26 _T18AB=Cyc_yyget_YY36(_T48A);struct Cyc_List_List*_T18AC;struct Cyc_Parse_Type_specifier _T18AD;struct Cyc_Absyn_Tqual _T18AE;_T18AE=_T18AB.f0;_T18AD=_T18AB.f1;_T18AC=_T18AB.f2;{struct Cyc_Absyn_Tqual tq=_T18AE;struct Cyc_Parse_Type_specifier tspecs=_T18AD;struct Cyc_List_List*atts=_T18AC;_T48B=tq;_T48C=_T48B.loc;
if(_T48C!=0U)goto _TL29B;_T48D=yyyvsp;_T48E=_T48D[2];_T48F=_T48E.l;_T490=_T48F.first_line;tq.loc=Cyc_Position_loc_to_seg(_T490);goto _TL29C;_TL29B: _TL29C: _T491=yyyvsp;_T492=& _T491[3].v;_T493=(union Cyc_YYSTYPE*)_T492;{
struct Cyc_Parse_Declarator _T18AF=Cyc_yyget_YY28(_T493);struct Cyc_List_List*_T18B0;unsigned _T18B1;struct _tuple0*_T18B2;_T18B2=_T18AF.id;_T18B1=_T18AF.varloc;_T18B0=_T18AF.tms;{struct _tuple0*qv=_T18B2;unsigned varloc=_T18B1;struct Cyc_List_List*tms=_T18B0;_T494=tspecs;_T495=yyyvsp;_T496=_T495[2];_T497=_T496.l;_T498=_T497.first_line;_T499=
Cyc_Position_loc_to_seg(_T498);{void*t=Cyc_Parse_speclist2typ(_T494,_T499);
struct _tuple14 _T18B3=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T18B4;struct Cyc_List_List*_T18B5;void*_T18B6;struct Cyc_Absyn_Tqual _T18B7;_T18B7=_T18B3.f0;_T18B6=_T18B3.f1;_T18B5=_T18B3.f2;_T18B4=_T18B3.f3;{struct Cyc_Absyn_Tqual tq2=_T18B7;void*t2=_T18B6;struct Cyc_List_List*tvs=_T18B5;struct Cyc_List_List*atts2=_T18B4;
if(tvs==0)goto _TL29D;{struct Cyc_Warn_String_Warn_Warg_struct _T18B8;_T18B8.tag=0;_T18B8.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_T49A=_T18B8;}{struct Cyc_Warn_String_Warn_Warg_struct _T18B8=_T49A;void*_T18B9[1];_T18B9[0]=& _T18B8;_T49B=yyyvsp;_T49C=_T49B[3];_T49D=_T49C.l;_T49E=_T49D.first_line;_T49F=Cyc_Position_loc_to_seg(_T49E);_T4A0=_tag_fat(_T18B9,sizeof(void*),1);Cyc_Warn_err2(_T49F,_T4A0);}goto _TL29E;_TL29D: _TL29E: _T4A1=
Cyc_Absyn_is_qvar_qualified(qv);if(!_T4A1)goto _TL29F;{struct Cyc_Warn_String_Warn_Warg_struct _T18B8;_T18B8.tag=0;
_T18B8.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_T4A2=_T18B8;}{struct Cyc_Warn_String_Warn_Warg_struct _T18B8=_T4A2;void*_T18B9[1];_T18B9[0]=& _T18B8;_T4A3=yyyvsp;_T4A4=_T4A3[0];_T4A5=_T4A4.l;_T4A6=_T4A5.first_line;_T4A7=Cyc_Position_loc_to_seg(_T4A6);_T4A8=_tag_fat(_T18B9,sizeof(void*),1);Cyc_Warn_err2(_T4A7,_T4A8);}goto _TL2A0;_TL29F: _TL2A0:
 if(atts2==0)goto _TL2A1;{struct Cyc_Warn_String_Warn_Warg_struct _T18B8;_T18B8.tag=0;_T18B8.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_T4A9=_T18B8;}{struct Cyc_Warn_String_Warn_Warg_struct _T18B8=_T4A9;void*_T18B9[1];_T18B9[0]=& _T18B8;_T4AA=yyyvsp;_T4AB=_T4AA[0];_T4AC=_T4AB.l;_T4AD=_T4AC.first_line;_T4AE=Cyc_Position_loc_to_seg(_T4AD);_T4AF=_tag_fat(_T18B9,sizeof(void*),1);Cyc_Warn_warn2(_T4AE,_T4AF);}goto _TL2A2;_TL2A1: _TL2A2: {
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(varloc,qv,t2,0,0);{struct Cyc_Absyn_SubsetType_Absyn_Type_struct*_T18B8=_cycalloc(sizeof(struct Cyc_Absyn_SubsetType_Absyn_Type_struct));_T18B8->tag=12;
_T18B8->f1=vd;_T4B1=yyyvsp;_T4B2=& _T4B1[5].v;_T4B3=(union Cyc_YYSTYPE*)_T4B2;_T18B8->f2=Cyc_yyget_Exp_tok(_T4B3);_T18B8->f3=0;_T4B0=(struct Cyc_Absyn_SubsetType_Absyn_Type_struct*)_T18B8;}_T4B4=(void*)_T4B0;_T4B5=yyyvsp;_T4B6=_T4B5[0];_T4B7=_T4B6.l;_T4B8=_T4B7.first_line;_T4B9=Cyc_Position_loc_to_seg(_T4B8);_T4BA=Cyc_Parse_type_spec(_T4B4,_T4B9);yyval=Cyc_YY21(_T4BA);goto _LL0;}}}}}}}case 116: _T4BB=yyyvsp;_T4BC=& _T4BB[0].v;_T4BD=(union Cyc_YYSTYPE*)_T4BC;_T4BE=
# 1741 "parse.y"
Cyc_yyget_String_tok(_T4BD);_T4BF=yyyvsp;_T4C0=_T4BF[0];_T4C1=_T4C0.l;_T4C2=_T4C1.first_line;_T4C3=Cyc_Position_loc_to_seg(_T4C2);_T4C4=Cyc_Kinds_id_to_kind(_T4BE,_T4C3);yyval=Cyc_YY44(_T4C4);goto _LL0;case 117: _T4C6=Cyc_Flags_porting_c_code;
# 1745 "parse.y"
if(!_T4C6)goto _TL2A3;_T4C7=yyyvsp;_T4C8=_T4C7[0];_T4C9=_T4C8.l;_T4CA=_T4C9.first_line;_T4C5=Cyc_Position_loc_to_seg(_T4CA);goto _TL2A4;_TL2A3: _T4C5=0U;_TL2A4: {unsigned loc=_T4C5;{struct Cyc_Absyn_Tqual _T18AB;
_T18AB.print_const=1;_T18AB.q_volatile=0;_T18AB.q_restrict=0;_T18AB.real_const=1;_T18AB.loc=loc;_T4CB=_T18AB;}yyval=Cyc_YY24(_T4CB);goto _LL0;}case 118:{struct Cyc_Absyn_Tqual _T18AB;
# 1747 "parse.y"
_T18AB.print_const=0;_T18AB.q_volatile=1;_T18AB.q_restrict=0;_T18AB.real_const=0;_T18AB.loc=0U;_T4CC=_T18AB;}yyval=Cyc_YY24(_T4CC);goto _LL0;case 119:{struct Cyc_Absyn_Tqual _T18AB;
# 1748 "parse.y"
_T18AB.print_const=0;_T18AB.q_volatile=0;_T18AB.q_restrict=1;_T18AB.real_const=0;_T18AB.loc=0U;_T4CD=_T18AB;}yyval=Cyc_YY24(_T4CD);goto _LL0;case 120:  {
# 1754 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl));_T4CE=ed;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_T18AB->tag=1;{struct Cyc_Absyn_Enumdecl*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T18AC->sc=2U;_T4D1=yyyvsp;_T4D2=& _T4D1[1].v;_T4D3=(union Cyc_YYSTYPE*)_T4D2;_T18AC->name=Cyc_yyget_QualId_tok(_T4D3);{struct Cyc_Core_Opt*_T18AD=_cycalloc(sizeof(struct Cyc_Core_Opt));_T4D5=yyyvsp;_T4D6=& _T4D5[3].v;_T4D7=(union Cyc_YYSTYPE*)_T4D6;_T18AD->v=Cyc_yyget_YY49(_T4D7);_T4D4=(struct Cyc_Core_Opt*)_T18AD;}_T18AC->fields=_T4D4;_T4D0=(struct Cyc_Absyn_Enumdecl*)_T18AC;}_T18AB->f1=_T4D0;_T4CF=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T18AB;}_T4CE->r=(void*)_T4CF;_T4D8=ed;_T4D9=yyyvsp;_T4DA=_T4D9[0];_T4DB=_T4DA.l;_T4DC=_T4DB.first_line;
_T4D8->loc=Cyc_Position_loc_to_seg(_T4DC);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T18AB->tag=10;
_T18AB->f1=ed;_T18AB->f2=0;_T4DD=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T18AB;}_T4DE=(void*)_T4DD;_T4DF=yyyvsp;_T4E0=_T4DF[0];_T4E1=_T4E0.l;_T4E2=_T4E1.first_line;_T4E3=Cyc_Position_loc_to_seg(_T4E2);_T4E4=Cyc_Parse_type_spec(_T4DE,_T4E3);yyval=Cyc_YY21(_T4E4);goto _LL0;}case 121: _T4E5=yyyvsp;_T4E6=& _T4E5[1].v;_T4E7=(union Cyc_YYSTYPE*)_T4E6;_T4E8=
# 1759 "parse.y"
Cyc_yyget_QualId_tok(_T4E7);_T4E9=Cyc_Absyn_enum_type(_T4E8,0);_T4EA=yyyvsp;_T4EB=_T4EA[0];_T4EC=_T4EB.l;_T4ED=_T4EC.first_line;_T4EE=Cyc_Position_loc_to_seg(_T4ED);_T4EF=Cyc_Parse_type_spec(_T4E9,_T4EE);yyval=Cyc_YY21(_T4EF);goto _LL0;case 122: _T4F0=yyyvsp;_T4F1=& _T4F0[2].v;_T4F2=(union Cyc_YYSTYPE*)_T4F1;_T4F3=
# 1761 "parse.y"
Cyc_yyget_YY49(_T4F2);_T4F4=Cyc_Absyn_anon_enum_type(_T4F3);_T4F5=yyyvsp;_T4F6=_T4F5[0];_T4F7=_T4F6.l;_T4F8=_T4F7.first_line;_T4F9=Cyc_Position_loc_to_seg(_T4F8);_T4FA=Cyc_Parse_type_spec(_T4F4,_T4F9);yyval=Cyc_YY21(_T4FA);goto _LL0;case 123:{struct Cyc_Absyn_Enumfield*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_T4FC=yyyvsp;_T4FD=& _T4FC[0].v;_T4FE=(union Cyc_YYSTYPE*)_T4FD;
# 1767 "parse.y"
_T18AB->name=Cyc_yyget_QualId_tok(_T4FE);_T18AB->tag=0;_T4FF=yyyvsp;_T500=_T4FF[0];_T501=_T500.l;_T502=_T501.first_line;_T18AB->loc=Cyc_Position_loc_to_seg(_T502);_T4FB=(struct Cyc_Absyn_Enumfield*)_T18AB;}yyval=Cyc_YY48(_T4FB);goto _LL0;case 124:{struct Cyc_Absyn_Enumfield*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_T504=yyyvsp;_T505=& _T504[0].v;_T506=(union Cyc_YYSTYPE*)_T505;
# 1769 "parse.y"
_T18AB->name=Cyc_yyget_QualId_tok(_T506);_T507=yyyvsp;_T508=& _T507[2].v;_T509=(union Cyc_YYSTYPE*)_T508;_T18AB->tag=Cyc_yyget_Exp_tok(_T509);_T50A=yyyvsp;_T50B=_T50A[0];_T50C=_T50B.l;_T50D=_T50C.first_line;_T18AB->loc=Cyc_Position_loc_to_seg(_T50D);_T503=(struct Cyc_Absyn_Enumfield*)_T18AB;}yyval=Cyc_YY48(_T503);goto _LL0;case 125:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T50F=yyyvsp;_T510=& _T50F[0].v;_T511=(union Cyc_YYSTYPE*)_T510;
# 1773 "parse.y"
_T18AB->hd=Cyc_yyget_YY48(_T511);_T18AB->tl=0;_T50E=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY49(_T50E);goto _LL0;case 126:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T513=yyyvsp;_T514=& _T513[0].v;_T515=(union Cyc_YYSTYPE*)_T514;
# 1774 "parse.y"
_T18AB->hd=Cyc_yyget_YY48(_T515);_T18AB->tl=0;_T512=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY49(_T512);goto _LL0;case 127:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T517=yyyvsp;_T518=& _T517[0].v;_T519=(union Cyc_YYSTYPE*)_T518;
# 1775 "parse.y"
_T18AB->hd=Cyc_yyget_YY48(_T519);_T51A=yyyvsp;_T51B=& _T51A[2].v;_T51C=(union Cyc_YYSTYPE*)_T51B;_T18AB->tl=Cyc_yyget_YY49(_T51C);_T516=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY49(_T516);goto _LL0;case 128:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_T18AB->tag=7;_T51E=yyyvsp;_T51F=& _T51E[0].v;_T520=(union Cyc_YYSTYPE*)_T51F;
# 1781 "parse.y"
_T18AB->f1=Cyc_yyget_YY22(_T520);_T18AB->f2=0;_T521=yyyvsp;_T522=& _T521[2].v;_T523=(union Cyc_YYSTYPE*)_T522;_T18AB->f3=Cyc_yyget_YY25(_T523);_T51D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T18AB;}_T524=(void*)_T51D;_T525=yyyvsp;_T526=_T525[0];_T527=_T526.l;_T528=_T527.first_line;_T529=Cyc_Position_loc_to_seg(_T528);_T52A=Cyc_Parse_type_spec(_T524,_T529);yyval=Cyc_YY21(_T52A);goto _LL0;case 129: _T52C=Cyc_List_map_c;{
# 1785 "parse.y"
struct Cyc_List_List*(*_T18AB)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T52C;_T52B=_T18AB;}_T52D=yyyvsp;_T52E=_T52D[2];_T52F=_T52E.l;_T530=_T52F.first_line;_T531=Cyc_Position_loc_to_seg(_T530);_T532=yyyvsp;_T533=& _T532[2].v;_T534=(union Cyc_YYSTYPE*)_T533;_T535=Cyc_yyget_YY41(_T534);{struct Cyc_List_List*ts=_T52B(Cyc_Parse_typ2tvar,_T531,_T535);_T537=Cyc_List_map_c;{
struct Cyc_List_List*(*_T18AB)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T537;_T536=_T18AB;}_T538=yyyvsp;_T539=_T538[4];_T53A=_T539.l;_T53B=_T53A.first_line;_T53C=Cyc_Position_loc_to_seg(_T53B);_T53D=yyyvsp;_T53E=& _T53D[4].v;_T53F=(union Cyc_YYSTYPE*)_T53E;_T540=Cyc_yyget_YY41(_T53F);{struct Cyc_List_List*exist_ts=_T536(Cyc_Parse_typ2tvar,_T53C,_T540);_T541=yyyvsp;_T542=& _T541[5].v;_T543=(union Cyc_YYSTYPE*)_T542;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T543);_T545=ec_qb;_T546=(unsigned)_T545;
if(!_T546)goto _TL2A5;_T547=ec_qb;_T544=*_T547;goto _TL2A6;_TL2A5:{struct _tuple28 _T18AB;_T18AB.f0=0;_T18AB.f1=0;_T548=_T18AB;}_T544=_T548;_TL2A6: {struct _tuple28 _T18AB=_T544;struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_List_List*ec=_T18AD;struct Cyc_List_List*qb=_T18AC;_T549=yyyvsp;_T54A=& _T549[0].v;_T54B=(union Cyc_YYSTYPE*)_T54A;_T54C=
Cyc_yyget_YY23(_T54B);_T54D=_T54C.f1;_T54E=yyyvsp;_T54F=& _T54E[1].v;_T550=(union Cyc_YYSTYPE*)_T54F;_T551=Cyc_yyget_QualId_tok(_T550);_T552=ts;_T553=exist_ts;_T554=ec;_T555=qb;_T556=yyyvsp;_T557=& _T556[6].v;_T558=(union Cyc_YYSTYPE*)_T557;_T559=
Cyc_yyget_YY25(_T558);_T55A=yyyvsp;_T55B=& _T55A[0].v;_T55C=(union Cyc_YYSTYPE*)_T55B;_T55D=Cyc_yyget_YY23(_T55C);_T55E=_T55D.f0;_T55F=Cyc_Absyn_aggrdecl_impl(_T553,_T554,_T555,_T559,_T55E);_T560=yyyvsp;_T561=_T560[0];_T562=_T561.l;_T563=_T562.first_line;_T564=
Cyc_Position_loc_to_seg(_T563);{
# 1789
struct Cyc_Absyn_TypeDecl*td=Cyc_Absyn_aggr_tdecl(_T54D,2U,_T551,_T552,_T55F,0,_T564);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T18AE->tag=10;
# 1792
_T18AE->f1=td;_T18AE->f2=0;_T565=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T18AE;}_T566=(void*)_T565;_T567=yyyvsp;_T568=_T567[0];_T569=_T568.l;_T56A=_T569.first_line;_T56B=Cyc_Position_loc_to_seg(_T56A);_T56C=Cyc_Parse_type_spec(_T566,_T56B);yyval=Cyc_YY21(_T56C);goto _LL0;}}}}}}case 130: _T56D=yyyvsp;_T56E=& _T56D[0].v;_T56F=(union Cyc_YYSTYPE*)_T56E;_T570=
# 1795 "parse.y"
Cyc_yyget_YY23(_T56F);_T571=_T570.f1;_T572=yyyvsp;_T573=& _T572[1].v;_T574=(union Cyc_YYSTYPE*)_T573;_T575=Cyc_yyget_QualId_tok(_T574);_T577=yyyvsp;_T578=& _T577[0].v;_T579=(union Cyc_YYSTYPE*)_T578;_T57A=Cyc_yyget_YY23(_T579);_T57B=_T57A.f0;if(!_T57B)goto _TL2A7;{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));_T18AB->v=(void*)1;_T57C=(struct Cyc_Core_Opt*)_T18AB;}_T576=_T57C;goto _TL2A8;_TL2A7: _T576=0;_TL2A8: _T57D=Cyc_Absyn_UnknownAggr(_T571,_T575,_T576);_T57E=yyyvsp;_T57F=& _T57E[2].v;_T580=(union Cyc_YYSTYPE*)_T57F;_T581=
Cyc_yyget_YY41(_T580);_T582=
# 1795
Cyc_Absyn_aggr_type(_T57D,_T581);_T583=yyyvsp;_T584=_T583[0];_T585=_T584.l;_T586=_T585.first_line;_T587=
Cyc_Position_loc_to_seg(_T586);_T588=
# 1795
Cyc_Parse_type_spec(_T582,_T587);yyval=Cyc_YY21(_T588);goto _LL0;case 131:{struct _tuple25 _T18AB;
# 1800 "parse.y"
_T18AB.f0=1;_T58A=yyyvsp;_T58B=& _T58A[1].v;_T58C=(union Cyc_YYSTYPE*)_T58B;_T18AB.f1=Cyc_yyget_YY22(_T58C);_T589=_T18AB;}yyval=Cyc_YY23(_T589);goto _LL0;case 132:{struct _tuple25 _T18AB;
# 1801 "parse.y"
_T18AB.f0=0;_T58E=yyyvsp;_T58F=& _T58E[0].v;_T590=(union Cyc_YYSTYPE*)_T58F;_T18AB.f1=Cyc_yyget_YY22(_T590);_T58D=_T18AB;}yyval=Cyc_YY23(_T58D);goto _LL0;case 133:
# 1804
 yyval=Cyc_YY22(0U);goto _LL0;case 134:
# 1805 "parse.y"
 yyval=Cyc_YY22(1U);goto _LL0;case 135:
# 1808
 yyval=Cyc_YY41(0);goto _LL0;case 136: _T591=yyyvsp;_T592=& _T591[1].v;_T593=(union Cyc_YYSTYPE*)_T592;_T594=
# 1809 "parse.y"
Cyc_yyget_YY41(_T593);_T595=Cyc_List_imp_rev(_T594);yyval=Cyc_YY41(_T595);goto _LL0;case 137:
# 1814 "parse.y"
 yyval=Cyc_YY25(0);goto _LL0;case 138:  {
# 1816 "parse.y"
struct Cyc_List_List*decls=0;_T596=yyyvsp;_T597=& _T596[0].v;_T598=(union Cyc_YYSTYPE*)_T597;{
struct Cyc_List_List*x=Cyc_yyget_YY26(_T598);_TL2AC: if(x!=0)goto _TL2AA;else{goto _TL2AB;}
_TL2AA: _T599=x;_T59A=_T599->hd;_T59B=(struct Cyc_List_List*)_T59A;_T59C=decls;decls=Cyc_List_imp_append(_T59B,_T59C);_T59D=x;
# 1817
x=_T59D->tl;goto _TL2AC;_TL2AB:;}{
# 1820
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags==0)goto _TL2AD;_T59F=Cyc_List_iter_c;{
void(*_T18AB)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))_T59F;_T59E=_T18AB;}_T5A0=tags;_T5A1=decls;_T59E(Cyc_Parse_substitute_aggrfield_tags,_T5A0,_T5A1);goto _TL2AE;_TL2AD: _TL2AE:
 yyval=Cyc_YY25(decls);goto _LL0;}}case 139:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T5A3=yyyvsp;_T5A4=& _T5A3[0].v;_T5A5=(union Cyc_YYSTYPE*)_T5A4;
# 1829 "parse.y"
_T18AB->hd=Cyc_yyget_YY25(_T5A5);_T18AB->tl=0;_T5A2=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY26(_T5A2);goto _LL0;case 140:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T5A7=yyyvsp;_T5A8=& _T5A7[1].v;_T5A9=(union Cyc_YYSTYPE*)_T5A8;
# 1830 "parse.y"
_T18AB->hd=Cyc_yyget_YY25(_T5A9);_T5AA=yyyvsp;_T5AB=& _T5AA[0].v;_T5AC=(union Cyc_YYSTYPE*)_T5AB;_T18AB->tl=Cyc_yyget_YY26(_T5AC);_T5A6=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY26(_T5A6);goto _LL0;case 141: _T5AE=yyr;{struct _tuple11*_T18AB=_region_malloc(_T5AE,0U,sizeof(struct _tuple11));
# 1836 "parse.y"
_T18AB->tl=0;_T5AF=yyyvsp;_T5B0=& _T5AF[0].v;_T5B1=(union Cyc_YYSTYPE*)_T5B0;_T18AB->hd=Cyc_yyget_YY18(_T5B1);_T5AD=(struct _tuple11*)_T18AB;}yyval=Cyc_YY19(_T5AD);goto _LL0;case 142: _T5B3=yyr;{struct _tuple11*_T18AB=_region_malloc(_T5B3,0U,sizeof(struct _tuple11));_T5B4=yyyvsp;_T5B5=& _T5B4[0].v;_T5B6=(union Cyc_YYSTYPE*)_T5B5;
# 1838 "parse.y"
_T18AB->tl=Cyc_yyget_YY19(_T5B6);_T5B7=yyyvsp;_T5B8=& _T5B7[2].v;_T5B9=(union Cyc_YYSTYPE*)_T5B8;_T18AB->hd=Cyc_yyget_YY18(_T5B9);_T5B2=(struct _tuple11*)_T18AB;}yyval=Cyc_YY19(_T5B2);goto _LL0;case 143:{struct _tuple12 _T18AB;_T5BB=yyyvsp;_T5BC=& _T5BB[0].v;_T5BD=(union Cyc_YYSTYPE*)_T5BC;
# 1842 "parse.y"
_T18AB.f0=Cyc_yyget_YY28(_T5BD);_T18AB.f1=0;_T18AB.f2=0;_T5BA=_T18AB;}yyval=Cyc_YY18(_T5BA);goto _LL0;case 144:{struct _tuple12 _T18AB;_T5BF=yyyvsp;_T5C0=& _T5BF[0].v;_T5C1=(union Cyc_YYSTYPE*)_T5C0;
# 1843 "parse.y"
_T18AB.f0=Cyc_yyget_YY28(_T5C1);_T18AB.f1=0;_T5C2=yyyvsp;_T5C3=& _T5C2[2].v;_T5C4=(union Cyc_YYSTYPE*)_T5C3;_T5C5=Cyc_yyget_YY63(_T5C4);_T5C6=yyyvsp;_T5C7=_T5C6[1];_T5C8=_T5C7.l;_T5C9=_T5C8.first_line;_T5CA=Cyc_Position_loc_to_seg(_T5C9);_T18AB.f2=Cyc_Absyn_new_exp(_T5C5,_T5CA);_T5BE=_T18AB;}yyval=Cyc_YY18(_T5BE);goto _LL0;case 145:{struct _tuple12 _T18AB;_T5CC=yyyvsp;_T5CD=& _T5CC[0].v;_T5CE=(union Cyc_YYSTYPE*)_T5CD;
# 1844 "parse.y"
_T18AB.f0=Cyc_yyget_YY28(_T5CE);_T5CF=yyyvsp;_T5D0=& _T5CF[2].v;_T5D1=(union Cyc_YYSTYPE*)_T5D0;_T18AB.f1=Cyc_yyget_Exp_tok(_T5D1);_T18AB.f2=0;_T5CB=_T18AB;}yyval=Cyc_YY18(_T5CB);goto _LL0;case 146:{struct _tuple12 _T18AB;_T5D3=yyyvsp;_T5D4=& _T5D3[0].v;_T5D5=(union Cyc_YYSTYPE*)_T5D4;
# 1845 "parse.y"
_T18AB.f0=Cyc_yyget_YY28(_T5D5);_T5D6=yyyvsp;_T5D7=& _T5D6[4].v;_T5D8=(union Cyc_YYSTYPE*)_T5D7;_T18AB.f1=Cyc_yyget_Exp_tok(_T5D8);_T5D9=yyyvsp;_T5DA=& _T5D9[2].v;_T5DB=(union Cyc_YYSTYPE*)_T5DA;_T5DC=Cyc_yyget_YY63(_T5DB);_T5DD=yyyvsp;_T5DE=_T5DD[1];_T5DF=_T5DE.l;_T5E0=_T5DF.first_line;_T5E1=Cyc_Position_loc_to_seg(_T5E0);_T18AB.f2=Cyc_Absyn_new_exp(_T5DC,_T5E1);_T5D2=_T18AB;}yyval=Cyc_YY18(_T5D2);goto _LL0;case 147:  {struct _RegionHandle _T18AB=_new_region(0U,"temp");struct _RegionHandle*temp=& _T18AB;_push_region(temp);_T5E2=yyyvsp;_T5E3=& _T5E2[0].v;_T5E4=(union Cyc_YYSTYPE*)_T5E3;{
# 1851 "parse.y"
struct _tuple26 _T18AC=Cyc_yyget_YY36(_T5E4);struct Cyc_List_List*_T18AD;struct Cyc_Parse_Type_specifier _T18AE;struct Cyc_Absyn_Tqual _T18AF;_T18AF=_T18AC.f0;_T18AE=_T18AC.f1;_T18AD=_T18AC.f2;{struct Cyc_Absyn_Tqual tq=_T18AF;struct Cyc_Parse_Type_specifier tspecs=_T18AE;struct Cyc_List_List*atts=_T18AD;_T5E5=tq;_T5E6=_T5E5.loc;
if(_T5E6!=0U)goto _TL2AF;_T5E7=yyyvsp;_T5E8=_T5E7[0];_T5E9=_T5E8.l;_T5EA=_T5E9.first_line;tq.loc=Cyc_Position_loc_to_seg(_T5EA);goto _TL2B0;_TL2AF: _TL2B0: {
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;_T5EB=yyyvsp;_T5EC=& _T5EB[1].v;_T5ED=(union Cyc_YYSTYPE*)_T5EC;{
struct Cyc_List_List*x=Cyc_yyget_YY30(_T5ED);_TL2B4: if(x!=0)goto _TL2B2;else{goto _TL2B3;}
_TL2B2: _T5EE=x;_T5EF=_T5EE->hd;{struct _tuple12*_T18B0=(struct _tuple12*)_T5EF;struct Cyc_Absyn_Exp*_T18B1;struct Cyc_Absyn_Exp*_T18B2;struct Cyc_Parse_Declarator _T18B3;{struct _tuple12 _T18B4=*_T18B0;_T18B3=_T18B4.f0;_T18B2=_T18B4.f1;_T18B1=_T18B4.f2;}{struct Cyc_Parse_Declarator d=_T18B3;struct Cyc_Absyn_Exp*wd=_T18B2;struct Cyc_Absyn_Exp*wh=_T18B1;_T5F1=temp;{struct _tuple13*_T18B4=_region_malloc(_T5F1,0U,sizeof(struct _tuple13));
_T18B4->tl=decls;_T18B4->hd=d;_T5F0=(struct _tuple13*)_T18B4;}decls=_T5F0;_T5F3=temp;{struct Cyc_List_List*_T18B4=_region_malloc(_T5F3,0U,sizeof(struct Cyc_List_List));_T5F5=temp;{struct _tuple16*_T18B5=_region_malloc(_T5F5,0U,sizeof(struct _tuple16));
_T18B5->f0=wd;_T18B5->f1=wh;_T5F4=(struct _tuple16*)_T18B5;}_T18B4->hd=_T5F4;_T18B4->tl=widths_and_reqs;_T5F2=(struct Cyc_List_List*)_T18B4;}widths_and_reqs=_T5F2;}}_T5F6=x;
# 1855
x=_T5F6->tl;goto _TL2B4;_TL2B3:;}_T5F7=tspecs;_T5F8=yyyvsp;_T5F9=_T5F8[0];_T5FA=_T5F9.l;_T5FB=_T5FA.first_line;_T5FC=
# 1860
Cyc_Position_loc_to_seg(_T5FB);{void*t=Cyc_Parse_speclist2typ(_T5F7,_T5FC);_T5FD=temp;_T5FE=temp;_T5FF=
# 1862
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);_T600=widths_and_reqs;{
# 1861
struct Cyc_List_List*info=Cyc_List_rzip(_T5FD,_T5FE,_T5FF,_T600);_T602=Cyc_List_map_c;{
# 1863
struct Cyc_List_List*(*_T18B0)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))_T602;_T601=_T18B0;}_T603=yyyvsp;_T604=_T603[0];_T605=_T604.l;_T606=_T605.first_line;_T607=Cyc_Position_loc_to_seg(_T606);_T608=info;_T609=_T601(Cyc_Parse_make_aggr_field,_T607,_T608);yyval=Cyc_YY25(_T609);_npop_handler(0);goto _LL0;}}}}}_pop_region();}case 148:{struct _tuple26 _T18AB;_T60B=yyyvsp;_T60C=_T60B[0];_T60D=_T60C.l;_T60E=_T60D.first_line;_T60F=
# 1872 "parse.y"
Cyc_Position_loc_to_seg(_T60E);_T18AB.f0=Cyc_Absyn_empty_tqual(_T60F);_T610=yyyvsp;_T611=& _T610[0].v;_T612=(union Cyc_YYSTYPE*)_T611;_T18AB.f1=Cyc_yyget_YY21(_T612);_T18AB.f2=0;_T60A=_T18AB;}yyval=Cyc_YY36(_T60A);goto _LL0;case 149: _T613=yyyvsp;_T614=& _T613[1].v;_T615=(union Cyc_YYSTYPE*)_T614;{
# 1874 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T615);{struct _tuple26 _T18AB;_T617=two;_T18AB.f0=_T617.f0;_T618=yyyvsp;_T619=_T618[0];_T61A=_T619.l;_T61B=_T61A.first_line;_T61C=Cyc_Position_loc_to_seg(_T61B);_T61D=yyyvsp;_T61E=& _T61D[0].v;_T61F=(union Cyc_YYSTYPE*)_T61E;_T620=Cyc_yyget_YY21(_T61F);_T621=two;_T622=_T621.f1;_T18AB.f1=Cyc_Parse_combine_specifiers(_T61C,_T620,_T622);_T623=two;_T18AB.f2=_T623.f2;_T616=_T18AB;}yyval=Cyc_YY36(_T616);goto _LL0;}case 150:{struct _tuple26 _T18AB;_T625=yyyvsp;_T626=& _T625[0].v;_T627=(union Cyc_YYSTYPE*)_T626;
# 1876 "parse.y"
_T18AB.f0=Cyc_yyget_YY24(_T627);_T18AB.f1=Cyc_Parse_empty_spec(0U);_T18AB.f2=0;_T624=_T18AB;}yyval=Cyc_YY36(_T624);goto _LL0;case 151: _T628=yyyvsp;_T629=& _T628[1].v;_T62A=(union Cyc_YYSTYPE*)_T629;{
# 1878 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T62A);{struct _tuple26 _T18AB;_T62C=yyyvsp;_T62D=& _T62C[0].v;_T62E=(union Cyc_YYSTYPE*)_T62D;_T62F=Cyc_yyget_YY24(_T62E);_T630=two;_T631=_T630.f0;_T18AB.f0=Cyc_Absyn_combine_tqual(_T62F,_T631);_T632=two;_T18AB.f1=_T632.f1;_T633=two;_T18AB.f2=_T633.f2;_T62B=_T18AB;}yyval=Cyc_YY36(_T62B);goto _LL0;}case 152:{struct _tuple26 _T18AB;_T635=yyyvsp;_T636=_T635[0];_T637=_T636.l;_T638=_T637.first_line;_T639=
# 1880 "parse.y"
Cyc_Position_loc_to_seg(_T638);_T18AB.f0=Cyc_Absyn_empty_tqual(_T639);_T18AB.f1=Cyc_Parse_empty_spec(0U);_T63A=yyyvsp;_T63B=& _T63A[0].v;_T63C=(union Cyc_YYSTYPE*)_T63B;_T18AB.f2=Cyc_yyget_YY46(_T63C);_T634=_T18AB;}yyval=Cyc_YY36(_T634);goto _LL0;case 153: _T63D=yyyvsp;_T63E=& _T63D[1].v;_T63F=(union Cyc_YYSTYPE*)_T63E;{
# 1882 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T63F);{struct _tuple26 _T18AB;_T641=two;_T18AB.f0=_T641.f0;_T642=two;_T18AB.f1=_T642.f1;_T643=yyyvsp;_T644=& _T643[0].v;_T645=(union Cyc_YYSTYPE*)_T644;_T646=Cyc_yyget_YY46(_T645);_T647=two;_T648=_T647.f2;_T18AB.f2=Cyc_List_append(_T646,_T648);_T640=_T18AB;}yyval=Cyc_YY36(_T640);goto _LL0;}case 154:{struct _tuple26 _T18AB;_T64A=yyyvsp;_T64B=_T64A[0];_T64C=_T64B.l;_T64D=_T64C.first_line;_T64E=
# 1888 "parse.y"
Cyc_Position_loc_to_seg(_T64D);_T18AB.f0=Cyc_Absyn_empty_tqual(_T64E);_T64F=yyyvsp;_T650=& _T64F[0].v;_T651=(union Cyc_YYSTYPE*)_T650;_T18AB.f1=Cyc_yyget_YY21(_T651);_T18AB.f2=0;_T649=_T18AB;}yyval=Cyc_YY36(_T649);goto _LL0;case 155: _T652=yyyvsp;_T653=& _T652[1].v;_T654=(union Cyc_YYSTYPE*)_T653;{
# 1890 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T654);{struct _tuple26 _T18AB;_T656=two;_T18AB.f0=_T656.f0;_T657=yyyvsp;_T658=_T657[0];_T659=_T658.l;_T65A=_T659.first_line;_T65B=Cyc_Position_loc_to_seg(_T65A);_T65C=yyyvsp;_T65D=& _T65C[0].v;_T65E=(union Cyc_YYSTYPE*)_T65D;_T65F=Cyc_yyget_YY21(_T65E);_T660=two;_T661=_T660.f1;_T18AB.f1=Cyc_Parse_combine_specifiers(_T65B,_T65F,_T661);_T662=two;_T18AB.f2=_T662.f2;_T655=_T18AB;}yyval=Cyc_YY36(_T655);goto _LL0;}case 156:{struct _tuple26 _T18AB;_T664=yyyvsp;_T665=& _T664[0].v;_T666=(union Cyc_YYSTYPE*)_T665;
# 1892 "parse.y"
_T18AB.f0=Cyc_yyget_YY24(_T666);_T18AB.f1=Cyc_Parse_empty_spec(0U);_T18AB.f2=0;_T663=_T18AB;}yyval=Cyc_YY36(_T663);goto _LL0;case 157: _T667=yyyvsp;_T668=& _T667[1].v;_T669=(union Cyc_YYSTYPE*)_T668;{
# 1894 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T669);{struct _tuple26 _T18AB;_T66B=yyyvsp;_T66C=& _T66B[0].v;_T66D=(union Cyc_YYSTYPE*)_T66C;_T66E=Cyc_yyget_YY24(_T66D);_T66F=two;_T670=_T66F.f0;_T18AB.f0=Cyc_Absyn_combine_tqual(_T66E,_T670);_T671=two;_T18AB.f1=_T671.f1;_T672=two;_T18AB.f2=_T672.f2;_T66A=_T18AB;}yyval=Cyc_YY36(_T66A);goto _LL0;}case 158:{struct _tuple26 _T18AB;_T674=yyyvsp;_T675=_T674[0];_T676=_T675.l;_T677=_T676.first_line;_T678=
# 1896 "parse.y"
Cyc_Position_loc_to_seg(_T677);_T18AB.f0=Cyc_Absyn_empty_tqual(_T678);_T18AB.f1=Cyc_Parse_empty_spec(0U);_T679=yyyvsp;_T67A=& _T679[0].v;_T67B=(union Cyc_YYSTYPE*)_T67A;_T18AB.f2=Cyc_yyget_YY46(_T67B);_T673=_T18AB;}yyval=Cyc_YY36(_T673);goto _LL0;case 159: _T67C=yyyvsp;_T67D=& _T67C[1].v;_T67E=(union Cyc_YYSTYPE*)_T67D;{
# 1898 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T67E);{struct _tuple26 _T18AB;_T680=two;_T18AB.f0=_T680.f0;_T681=two;_T18AB.f1=_T681.f1;_T682=yyyvsp;_T683=& _T682[0].v;_T684=(union Cyc_YYSTYPE*)_T683;_T685=Cyc_yyget_YY46(_T684);_T686=two;_T687=_T686.f2;_T18AB.f2=Cyc_List_append(_T685,_T687);_T67F=_T18AB;}yyval=Cyc_YY36(_T67F);goto _LL0;}case 160: _T689=yyr;{struct Cyc_List_List*_T18AB=_region_malloc(_T689,0U,sizeof(struct Cyc_List_List));_T68A=yyyvsp;_T68B=& _T68A[0].v;_T68C=(union Cyc_YYSTYPE*)_T68B;
# 1904 "parse.y"
_T18AB->hd=Cyc_yyget_YY29(_T68C);_T18AB->tl=0;_T688=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY30(_T688);goto _LL0;case 161: _T68E=yyr;{struct Cyc_List_List*_T18AB=_region_malloc(_T68E,0U,sizeof(struct Cyc_List_List));_T68F=yyyvsp;_T690=& _T68F[2].v;_T691=(union Cyc_YYSTYPE*)_T690;
# 1906 "parse.y"
_T18AB->hd=Cyc_yyget_YY29(_T691);_T692=yyyvsp;_T693=& _T692[0].v;_T694=(union Cyc_YYSTYPE*)_T693;_T18AB->tl=Cyc_yyget_YY30(_T694);_T68D=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY30(_T68D);goto _LL0;case 162: _T696=yyr;{struct _tuple12*_T18AB=_region_malloc(_T696,0U,sizeof(struct _tuple12));_T697=yyyvsp;_T698=& _T697[0].v;_T699=(union Cyc_YYSTYPE*)_T698;
# 1911 "parse.y"
_T18AB->f0=Cyc_yyget_YY28(_T699);_T18AB->f1=0;_T69A=yyyvsp;_T69B=& _T69A[1].v;_T69C=(union Cyc_YYSTYPE*)_T69B;_T18AB->f2=Cyc_yyget_YY61(_T69C);_T695=(struct _tuple12*)_T18AB;}yyval=Cyc_YY29(_T695);goto _LL0;case 163: _T69E=yyr;{struct _tuple12*_T18AB=_region_malloc(_T69E,0U,sizeof(struct _tuple12));{struct _tuple0*_T18AC=_cycalloc(sizeof(struct _tuple0));
# 1915 "parse.y"
_T18AC->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));*_T18AD=_tag_fat("",sizeof(char),1U);_T6A0=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T6A0;_T69F=(struct _tuple0*)_T18AC;}_T18AB->f0.id=_T69F;_T18AB->f0.varloc=0U;_T18AB->f0.tms=0;_T6A1=yyyvsp;_T6A2=& _T6A1[1].v;_T6A3=(union Cyc_YYSTYPE*)_T6A2;_T18AB->f1=Cyc_yyget_Exp_tok(_T6A3);_T18AB->f2=0;_T69D=(struct _tuple12*)_T18AB;}yyval=Cyc_YY29(_T69D);goto _LL0;case 164: _T6A5=yyr;{struct _tuple12*_T18AB=_region_malloc(_T6A5,0U,sizeof(struct _tuple12));{struct _tuple0*_T18AC=_cycalloc(sizeof(struct _tuple0));
# 1920 "parse.y"
_T18AC->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));*_T18AD=_tag_fat("",sizeof(char),1U);_T6A7=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T6A7;_T6A6=(struct _tuple0*)_T18AC;}_T18AB->f0.id=_T6A6;_T18AB->f0.varloc=0U;_T18AB->f0.tms=0;_T18AB->f1=0;_T18AB->f2=0;_T6A4=(struct _tuple12*)_T18AB;}yyval=Cyc_YY29(_T6A4);goto _LL0;case 165: _T6A9=yyr;{struct _tuple12*_T18AB=_region_malloc(_T6A9,0U,sizeof(struct _tuple12));_T6AA=yyyvsp;_T6AB=& _T6AA[0].v;_T6AC=(union Cyc_YYSTYPE*)_T6AB;
# 1923 "parse.y"
_T18AB->f0=Cyc_yyget_YY28(_T6AC);_T6AD=yyyvsp;_T6AE=& _T6AD[2].v;_T6AF=(union Cyc_YYSTYPE*)_T6AE;_T18AB->f1=Cyc_yyget_Exp_tok(_T6AF);_T18AB->f2=0;_T6A8=(struct _tuple12*)_T18AB;}yyval=Cyc_YY29(_T6A8);goto _LL0;case 166: _T6B0=yyyvsp;_T6B1=& _T6B0[2].v;_T6B2=(union Cyc_YYSTYPE*)_T6B1;_T6B3=
# 1927 "parse.y"
Cyc_yyget_Exp_tok(_T6B2);yyval=Cyc_YY61(_T6B3);goto _LL0;case 167:
# 1928 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 168: _T6B4=yyyvsp;_T6B5=& _T6B4[0].v;_T6B6=(union Cyc_YYSTYPE*)_T6B5;{
# 1934 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6B6);_T6B8=Cyc_List_map_c;{
struct Cyc_List_List*(*_T18AB)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T6B8;_T6B7=_T18AB;}_T6B9=yyyvsp;_T6BA=_T6B9[2];_T6BB=_T6BA.l;_T6BC=_T6BB.first_line;_T6BD=Cyc_Position_loc_to_seg(_T6BC);_T6BE=yyyvsp;_T6BF=& _T6BE[2].v;_T6C0=(union Cyc_YYSTYPE*)_T6BF;_T6C1=Cyc_yyget_YY41(_T6C0);{struct Cyc_List_List*ts=_T6B7(Cyc_Parse_typ2tvar,_T6BD,_T6C1);_T6C2=yyyvsp;_T6C3=& _T6C2[1].v;_T6C4=(union Cyc_YYSTYPE*)_T6C3;_T6C5=
Cyc_yyget_QualId_tok(_T6C4);_T6C6=ts;{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));_T6C8=yyyvsp;_T6C9=& _T6C8[4].v;_T6CA=(union Cyc_YYSTYPE*)_T6C9;_T18AB->v=Cyc_yyget_YY35(_T6CA);_T6C7=(struct Cyc_Core_Opt*)_T18AB;}_T6CB=is_extensible;_T6CC=yyyvsp;_T6CD=_T6CC[0];_T6CE=_T6CD.l;_T6CF=_T6CE.first_line;_T6D0=
Cyc_Position_loc_to_seg(_T6CF);{
# 1936
struct Cyc_Absyn_TypeDecl*dd=Cyc_Absyn_datatype_tdecl(2U,_T6C5,_T6C6,_T6C7,_T6CB,_T6D0);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T18AB->tag=10;
# 1938
_T18AB->f1=dd;_T18AB->f2=0;_T6D1=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T18AB;}_T6D2=(void*)_T6D1;_T6D3=yyyvsp;_T6D4=_T6D3[0];_T6D5=_T6D4.l;_T6D6=_T6D5.first_line;_T6D7=Cyc_Position_loc_to_seg(_T6D6);_T6D8=Cyc_Parse_type_spec(_T6D2,_T6D7);yyval=Cyc_YY21(_T6D8);goto _LL0;}}}case 169: _T6D9=yyyvsp;_T6DA=& _T6D9[0].v;_T6DB=(union Cyc_YYSTYPE*)_T6DA;{
# 1941 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6DB);{struct Cyc_Absyn_UnknownDatatypeInfo _T18AB;_T6DD=yyyvsp;_T6DE=& _T6DD[1].v;_T6DF=(union Cyc_YYSTYPE*)_T6DE;
_T18AB.name=Cyc_yyget_QualId_tok(_T6DF);_T18AB.is_extensible=is_extensible;_T6DC=_T18AB;}_T6E0=Cyc_Absyn_UnknownDatatype(_T6DC);_T6E1=yyyvsp;_T6E2=& _T6E1[2].v;_T6E3=(union Cyc_YYSTYPE*)_T6E2;_T6E4=Cyc_yyget_YY41(_T6E3);_T6E5=Cyc_Absyn_datatype_type(_T6E0,_T6E4);_T6E6=yyyvsp;_T6E7=_T6E6[0];_T6E8=_T6E7.l;_T6E9=_T6E8.first_line;_T6EA=Cyc_Position_loc_to_seg(_T6E9);_T6EB=Cyc_Parse_type_spec(_T6E5,_T6EA);yyval=Cyc_YY21(_T6EB);goto _LL0;}case 170: _T6EC=yyyvsp;_T6ED=& _T6EC[0].v;_T6EE=(union Cyc_YYSTYPE*)_T6ED;{
# 1945 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6EE);{struct Cyc_Absyn_UnknownDatatypeFieldInfo _T18AB;_T6F0=yyyvsp;_T6F1=& _T6F0[1].v;_T6F2=(union Cyc_YYSTYPE*)_T6F1;
_T18AB.datatype_name=Cyc_yyget_QualId_tok(_T6F2);_T6F3=yyyvsp;_T6F4=& _T6F3[3].v;_T6F5=(union Cyc_YYSTYPE*)_T6F4;_T18AB.field_name=Cyc_yyget_QualId_tok(_T6F5);_T18AB.is_extensible=is_extensible;_T6EF=_T18AB;}_T6F6=Cyc_Absyn_UnknownDatatypefield(_T6EF);_T6F7=yyyvsp;_T6F8=& _T6F7[4].v;_T6F9=(union Cyc_YYSTYPE*)_T6F8;_T6FA=Cyc_yyget_YY41(_T6F9);_T6FB=Cyc_Absyn_datatype_field_type(_T6F6,_T6FA);_T6FC=yyyvsp;_T6FD=_T6FC[0];_T6FE=_T6FD.l;_T6FF=_T6FE.first_line;_T700=Cyc_Position_loc_to_seg(_T6FF);_T701=Cyc_Parse_type_spec(_T6FB,_T700);yyval=Cyc_YY21(_T701);goto _LL0;}case 171:
# 1951 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 172:
# 1952 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 173:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T703=yyyvsp;_T704=& _T703[0].v;_T705=(union Cyc_YYSTYPE*)_T704;
# 1956 "parse.y"
_T18AB->hd=Cyc_yyget_YY34(_T705);_T18AB->tl=0;_T702=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY35(_T702);goto _LL0;case 174:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T707=yyyvsp;_T708=& _T707[0].v;_T709=(union Cyc_YYSTYPE*)_T708;
# 1957 "parse.y"
_T18AB->hd=Cyc_yyget_YY34(_T709);_T18AB->tl=0;_T706=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY35(_T706);goto _LL0;case 175:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T70B=yyyvsp;_T70C=& _T70B[0].v;_T70D=(union Cyc_YYSTYPE*)_T70C;
# 1958 "parse.y"
_T18AB->hd=Cyc_yyget_YY34(_T70D);_T70E=yyyvsp;_T70F=& _T70E[2].v;_T710=(union Cyc_YYSTYPE*)_T70F;_T18AB->tl=Cyc_yyget_YY35(_T710);_T70A=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY35(_T70A);goto _LL0;case 176:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T712=yyyvsp;_T713=& _T712[0].v;_T714=(union Cyc_YYSTYPE*)_T713;
# 1959 "parse.y"
_T18AB->hd=Cyc_yyget_YY34(_T714);_T715=yyyvsp;_T716=& _T715[2].v;_T717=(union Cyc_YYSTYPE*)_T716;_T18AB->tl=Cyc_yyget_YY35(_T717);_T711=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY35(_T711);goto _LL0;case 177:
# 1963 "parse.y"
 yyval=Cyc_YY33(2U);goto _LL0;case 178:
# 1964 "parse.y"
 yyval=Cyc_YY33(3U);goto _LL0;case 179:
# 1965 "parse.y"
 yyval=Cyc_YY33(0U);goto _LL0;case 180:{struct Cyc_Absyn_Datatypefield*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T719=yyyvsp;_T71A=& _T719[1].v;_T71B=(union Cyc_YYSTYPE*)_T71A;
# 1969 "parse.y"
_T18AB->name=Cyc_yyget_QualId_tok(_T71B);_T18AB->typs=0;_T71C=yyyvsp;_T71D=_T71C[0];_T71E=_T71D.l;_T71F=_T71E.first_line;_T18AB->loc=Cyc_Position_loc_to_seg(_T71F);_T720=yyyvsp;_T721=& _T720[0].v;_T722=(union Cyc_YYSTYPE*)_T721;_T18AB->sc=Cyc_yyget_YY33(_T722);_T718=(struct Cyc_Absyn_Datatypefield*)_T18AB;}yyval=Cyc_YY34(_T718);goto _LL0;case 181: _T724=Cyc_List_map_c;{
# 1971 "parse.y"
struct Cyc_List_List*(*_T18AB)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T724;_T723=_T18AB;}{struct _tuple19*(*_T18AB)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T725=_T18AB;}_T726=yyyvsp;_T727=_T726[3];_T728=_T727.l;_T729=_T728.first_line;_T72A=Cyc_Position_loc_to_seg(_T729);_T72B=yyyvsp;_T72C=& _T72B[3].v;_T72D=(union Cyc_YYSTYPE*)_T72C;_T72E=Cyc_yyget_YY39(_T72D);_T72F=Cyc_List_imp_rev(_T72E);{struct Cyc_List_List*typs=_T723(_T725,_T72A,_T72F);{struct Cyc_Absyn_Datatypefield*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T731=yyyvsp;_T732=& _T731[1].v;_T733=(union Cyc_YYSTYPE*)_T732;
_T18AB->name=Cyc_yyget_QualId_tok(_T733);_T18AB->typs=typs;_T734=yyyvsp;_T735=_T734[0];_T736=_T735.l;_T737=_T736.first_line;_T18AB->loc=Cyc_Position_loc_to_seg(_T737);_T738=yyyvsp;_T739=& _T738[0].v;_T73A=(union Cyc_YYSTYPE*)_T739;_T18AB->sc=Cyc_yyget_YY33(_T73A);_T730=(struct Cyc_Absyn_Datatypefield*)_T18AB;}yyval=Cyc_YY34(_T730);goto _LL0;}case 182: _T73B=yyyvsp;_T73C=_T73B[0];
# 1977 "parse.y"
yyval=_T73C.v;goto _LL0;case 183: _T73D=yyyvsp;_T73E=& _T73D[1].v;_T73F=(union Cyc_YYSTYPE*)_T73E;{
# 1979 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T73F);{struct Cyc_Parse_Declarator _T18AB;_T741=two;
_T18AB.id=_T741.id;_T742=two;_T18AB.varloc=_T742.varloc;_T743=yyyvsp;_T744=& _T743[0].v;_T745=(union Cyc_YYSTYPE*)_T744;_T746=Cyc_yyget_YY27(_T745);_T747=two;_T748=_T747.tms;_T18AB.tms=Cyc_List_imp_append(_T746,_T748);_T740=_T18AB;}yyval=Cyc_YY28(_T740);goto _LL0;}case 184: _T749=yyyvsp;_T74A=_T749[0];
# 1986 "parse.y"
yyval=_T74A.v;goto _LL0;case 185: _T74B=yyyvsp;_T74C=& _T74B[1].v;_T74D=(union Cyc_YYSTYPE*)_T74C;{
# 1988 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T74D);{struct Cyc_Parse_Declarator _T18AB;_T74F=two;
_T18AB.id=_T74F.id;_T750=two;_T18AB.varloc=_T750.varloc;_T751=yyyvsp;_T752=& _T751[0].v;_T753=(union Cyc_YYSTYPE*)_T752;_T754=Cyc_yyget_YY27(_T753);_T755=two;_T756=_T755.tms;_T18AB.tms=Cyc_List_imp_append(_T754,_T756);_T74E=_T18AB;}yyval=Cyc_YY28(_T74E);goto _LL0;}case 186:{struct Cyc_Parse_Declarator _T18AB;_T758=yyyvsp;_T759=& _T758[0].v;_T75A=(union Cyc_YYSTYPE*)_T759;
# 1993 "parse.y"
_T18AB.id=Cyc_yyget_QualId_tok(_T75A);_T75B=yyyvsp;_T75C=_T75B[0];_T75D=_T75C.l;_T75E=_T75D.first_line;_T18AB.varloc=Cyc_Position_loc_to_seg(_T75E);_T18AB.tms=0;_T757=_T18AB;}yyval=Cyc_YY28(_T757);goto _LL0;case 187: _T75F=yyyvsp;_T760=_T75F[1];
# 1994 "parse.y"
yyval=_T760.v;goto _LL0;case 188: _T761=yyyvsp;_T762=& _T761[2].v;_T763=(union Cyc_YYSTYPE*)_T762;{
# 1998 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T763);_T765=yyr;{struct Cyc_List_List*_T18AB=_region_malloc(_T765,0U,sizeof(struct Cyc_List_List));_T767=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T18AC=_region_malloc(_T767,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T18AC->tag=5;_T768=yyyvsp;_T769=_T768[1];_T76A=_T769.l;_T76B=_T76A.first_line;
_T18AC->f1=Cyc_Position_loc_to_seg(_T76B);_T76C=yyyvsp;_T76D=& _T76C[1].v;_T76E=(union Cyc_YYSTYPE*)_T76D;_T18AC->f2=Cyc_yyget_YY46(_T76E);_T766=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T18AC;}_T18AB->hd=(void*)_T766;_T76F=d;_T18AB->tl=_T76F.tms;_T764=(struct Cyc_List_List*)_T18AB;}d.tms=_T764;_T770=yyyvsp;_T771=_T770[2];
yyval=_T771.v;goto _LL0;}case 189:{struct Cyc_Parse_Declarator _T18AB;_T773=yyyvsp;_T774=& _T773[0].v;_T775=(union Cyc_YYSTYPE*)_T774;_T776=
# 2003 "parse.y"
Cyc_yyget_YY28(_T775);_T18AB.id=_T776.id;_T777=yyyvsp;_T778=& _T777[0].v;_T779=(union Cyc_YYSTYPE*)_T778;_T77A=Cyc_yyget_YY28(_T779);_T18AB.varloc=_T77A.varloc;_T77C=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T77C,0U,sizeof(struct Cyc_List_List));_T77E=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T77E,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T18AD->tag=0;_T77F=yyyvsp;_T780=& _T77F[3].v;_T781=(union Cyc_YYSTYPE*)_T780;_T18AD->f1=Cyc_yyget_YY54(_T781);_T782=yyyvsp;_T783=_T782[3];_T784=_T783.l;_T785=_T784.first_line;_T18AD->f2=Cyc_Position_loc_to_seg(_T785);_T77D=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T77D;_T786=yyyvsp;_T787=& _T786[0].v;_T788=(union Cyc_YYSTYPE*)_T787;_T789=Cyc_yyget_YY28(_T788);_T18AC->tl=_T789.tms;_T77B=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_T77B;_T772=_T18AB;}yyval=Cyc_YY28(_T772);goto _LL0;case 190:{struct Cyc_Parse_Declarator _T18AB;_T78B=yyyvsp;_T78C=& _T78B[0].v;_T78D=(union Cyc_YYSTYPE*)_T78C;_T78E=
# 2005 "parse.y"
Cyc_yyget_YY28(_T78D);_T18AB.id=_T78E.id;_T78F=yyyvsp;_T790=& _T78F[0].v;_T791=(union Cyc_YYSTYPE*)_T790;_T792=Cyc_yyget_YY28(_T791);_T18AB.varloc=_T792.varloc;_T794=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T794,0U,sizeof(struct Cyc_List_List));_T796=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T796,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T18AD->tag=1;_T797=yyyvsp;_T798=& _T797[2].v;_T799=(union Cyc_YYSTYPE*)_T798;
_T18AD->f1=Cyc_yyget_Exp_tok(_T799);_T79A=yyyvsp;_T79B=& _T79A[4].v;_T79C=(union Cyc_YYSTYPE*)_T79B;_T18AD->f2=Cyc_yyget_YY54(_T79C);_T79D=yyyvsp;_T79E=_T79D[4];_T79F=_T79E.l;_T7A0=_T79F.first_line;_T18AD->f3=Cyc_Position_loc_to_seg(_T7A0);_T795=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T795;_T7A1=yyyvsp;_T7A2=& _T7A1[0].v;_T7A3=(union Cyc_YYSTYPE*)_T7A2;_T7A4=Cyc_yyget_YY28(_T7A3);_T18AC->tl=_T7A4.tms;_T793=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_T793;_T78A=_T18AB;}
# 2005
yyval=Cyc_YY28(_T78A);goto _LL0;case 191: _T7A5=yyyvsp;_T7A6=& _T7A5[2].v;_T7A7=(union Cyc_YYSTYPE*)_T7A6;{
# 2008 "parse.y"
struct _tuple27*_T18AB=Cyc_yyget_YY40(_T7A7);struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;void*_T18AE;struct Cyc_Absyn_VarargInfo*_T18AF;int _T18B0;struct Cyc_List_List*_T18B1;{struct _tuple27 _T18B2=*_T18AB;_T18B1=_T18B2.f0;_T18B0=_T18B2.f1;_T18AF=_T18B2.f2;_T18AE=_T18B2.f3;_T18AD=_T18B2.f4;_T18AC=_T18B2.f5;}{struct Cyc_List_List*lis=_T18B1;int b=_T18B0;struct Cyc_Absyn_VarargInfo*c=_T18AF;void*eff=_T18AE;struct Cyc_List_List*ec=_T18AD;struct Cyc_List_List*qb=_T18AC;_T7A8=yyyvsp;_T7A9=& _T7A8[4].v;_T7AA=(union Cyc_YYSTYPE*)_T7A9;{
struct _tuple21 _T18B2=Cyc_yyget_YY62(_T7AA);struct Cyc_Absyn_Exp*_T18B3;struct Cyc_Absyn_Exp*_T18B4;struct Cyc_Absyn_Exp*_T18B5;struct Cyc_Absyn_Exp*_T18B6;_T18B6=_T18B2.f0;_T18B5=_T18B2.f1;_T18B4=_T18B2.f2;_T18B3=_T18B2.f3;{struct Cyc_Absyn_Exp*chk=_T18B6;struct Cyc_Absyn_Exp*req=_T18B5;struct Cyc_Absyn_Exp*ens=_T18B4;struct Cyc_Absyn_Exp*thrws=_T18B3;{struct Cyc_Parse_Declarator _T18B7;_T7AC=yyyvsp;_T7AD=& _T7AC[0].v;_T7AE=(union Cyc_YYSTYPE*)_T7AD;_T7AF=
Cyc_yyget_YY28(_T7AE);_T18B7.id=_T7AF.id;_T7B0=yyyvsp;_T7B1=& _T7B0[0].v;_T7B2=(union Cyc_YYSTYPE*)_T7B1;_T7B3=Cyc_yyget_YY28(_T7B2);_T18B7.varloc=_T7B3.varloc;_T7B5=yyr;{struct Cyc_List_List*_T18B8=_region_malloc(_T7B5,0U,sizeof(struct Cyc_List_List));_T7B7=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T18B9=_region_malloc(_T7B7,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T18B9->tag=3;_T7B9=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T18BA=_region_malloc(_T7B9,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T18BA->tag=1;_T18BA->f1=lis;_T18BA->f2=b;_T18BA->f3=c;_T18BA->f4=eff;_T18BA->f5=ec;_T18BA->f6=qb;_T18BA->f7=chk;_T18BA->f8=req;_T18BA->f9=ens;_T18BA->f10=thrws;_T7B8=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T18BA;}_T18B9->f1=(void*)_T7B8;_T7B6=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T18B9;}_T18B8->hd=(void*)_T7B6;_T7BA=yyyvsp;_T7BB=& _T7BA[0].v;_T7BC=(union Cyc_YYSTYPE*)_T7BB;_T7BD=Cyc_yyget_YY28(_T7BC);_T18B8->tl=_T7BD.tms;_T7B4=(struct Cyc_List_List*)_T18B8;}_T18B7.tms=_T7B4;_T7AB=_T18B7;}yyval=Cyc_YY28(_T7AB);goto _LL0;}}}}case 192:{struct Cyc_Parse_Declarator _T18AB;_T7BF=yyyvsp;_T7C0=& _T7BF[0].v;_T7C1=(union Cyc_YYSTYPE*)_T7C0;_T7C2=
# 2013 "parse.y"
Cyc_yyget_YY28(_T7C1);_T18AB.id=_T7C2.id;_T7C3=yyyvsp;_T7C4=& _T7C3[0].v;_T7C5=(union Cyc_YYSTYPE*)_T7C4;_T7C6=Cyc_yyget_YY28(_T7C5);_T18AB.varloc=_T7C6.varloc;_T7C8=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T7C8,0U,sizeof(struct Cyc_List_List));_T7CA=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T7CA,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T18AD->tag=3;_T7CC=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T18AE=_region_malloc(_T7CC,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T18AE->tag=0;_T7CD=yyyvsp;_T7CE=& _T7CD[2].v;_T7CF=(union Cyc_YYSTYPE*)_T7CE;_T18AE->f1=Cyc_yyget_YY37(_T7CF);_T7D0=yyyvsp;_T7D1=_T7D0[0];_T7D2=_T7D1.l;_T7D3=_T7D2.first_line;_T18AE->f2=Cyc_Position_loc_to_seg(_T7D3);_T7CB=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T18AE;}_T18AD->f1=(void*)_T7CB;_T7C9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T7C9;_T7D4=yyyvsp;_T7D5=& _T7D4[0].v;_T7D6=(union Cyc_YYSTYPE*)_T7D5;_T7D7=Cyc_yyget_YY28(_T7D6);_T18AC->tl=_T7D7.tms;_T7C7=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_T7C7;_T7BE=_T18AB;}yyval=Cyc_YY28(_T7BE);goto _LL0;case 193: _T7D9=Cyc_List_map_c;{
# 2016
struct Cyc_List_List*(*_T18AB)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T7D9;_T7D8=_T18AB;}_T7DA=yyyvsp;_T7DB=_T7DA[1];_T7DC=_T7DB.l;_T7DD=_T7DC.first_line;_T7DE=Cyc_Position_loc_to_seg(_T7DD);_T7DF=yyyvsp;_T7E0=& _T7DF[2].v;_T7E1=(union Cyc_YYSTYPE*)_T7E0;_T7E2=Cyc_yyget_YY41(_T7E1);_T7E3=Cyc_List_imp_rev(_T7E2);{struct Cyc_List_List*ts=_T7D8(Cyc_Parse_typ2tvar,_T7DE,_T7E3);{struct Cyc_Parse_Declarator _T18AB;_T7E5=yyyvsp;_T7E6=& _T7E5[0].v;_T7E7=(union Cyc_YYSTYPE*)_T7E6;_T7E8=
Cyc_yyget_YY28(_T7E7);_T18AB.id=_T7E8.id;_T7E9=yyyvsp;_T7EA=& _T7E9[0].v;_T7EB=(union Cyc_YYSTYPE*)_T7EA;_T7EC=Cyc_yyget_YY28(_T7EB);_T18AB.varloc=_T7EC.varloc;_T7EE=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T7EE,0U,sizeof(struct Cyc_List_List));_T7F0=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T7F0,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T18AD->tag=4;_T18AD->f1=ts;_T7F1=yyyvsp;_T7F2=_T7F1[0];_T7F3=_T7F2.l;_T7F4=_T7F3.first_line;_T18AD->f2=Cyc_Position_loc_to_seg(_T7F4);_T18AD->f3=0;_T7EF=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T7EF;_T7F5=yyyvsp;_T7F6=& _T7F5[0].v;_T7F7=(union Cyc_YYSTYPE*)_T7F6;_T7F8=Cyc_yyget_YY28(_T7F7);_T18AC->tl=_T7F8.tms;_T7ED=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_T7ED;_T7E4=_T18AB;}yyval=Cyc_YY28(_T7E4);goto _LL0;}case 194:{struct Cyc_Parse_Declarator _T18AB;_T7FA=yyyvsp;_T7FB=& _T7FA[0].v;_T7FC=(union Cyc_YYSTYPE*)_T7FB;_T7FD=
# 2020 "parse.y"
Cyc_yyget_YY28(_T7FC);_T18AB.id=_T7FD.id;_T7FE=yyyvsp;_T7FF=& _T7FE[0].v;_T800=(union Cyc_YYSTYPE*)_T7FF;_T801=Cyc_yyget_YY28(_T800);_T18AB.varloc=_T801.varloc;_T803=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T803,0U,sizeof(struct Cyc_List_List));_T805=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T805,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T18AD->tag=5;_T806=yyyvsp;_T807=_T806[1];_T808=_T807.l;_T809=_T808.first_line;_T18AD->f1=Cyc_Position_loc_to_seg(_T809);_T80A=yyyvsp;_T80B=& _T80A[1].v;_T80C=(union Cyc_YYSTYPE*)_T80B;_T18AD->f2=Cyc_yyget_YY46(_T80C);_T804=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T804;_T80D=yyyvsp;_T80E=& _T80D[0].v;_T80F=(union Cyc_YYSTYPE*)_T80E;_T810=
Cyc_yyget_YY28(_T80F);_T18AC->tl=_T810.tms;_T802=(struct Cyc_List_List*)_T18AC;}
# 2020
_T18AB.tms=_T802;_T7F9=_T18AB;}yyval=Cyc_YY28(_T7F9);goto _LL0;case 195:{struct Cyc_Parse_Declarator _T18AB;_T812=yyyvsp;_T813=& _T812[0].v;_T814=(union Cyc_YYSTYPE*)_T813;
# 2027 "parse.y"
_T18AB.id=Cyc_yyget_QualId_tok(_T814);_T815=yyyvsp;_T816=_T815[0];_T817=_T816.l;_T818=_T817.first_line;_T18AB.varloc=Cyc_Position_loc_to_seg(_T818);_T18AB.tms=0;_T811=_T18AB;}yyval=Cyc_YY28(_T811);goto _LL0;case 196:{struct Cyc_Parse_Declarator _T18AB;_T81A=yyyvsp;_T81B=& _T81A[0].v;_T81C=(union Cyc_YYSTYPE*)_T81B;
# 2028 "parse.y"
_T18AB.id=Cyc_yyget_QualId_tok(_T81C);_T81D=yyyvsp;_T81E=_T81D[0];_T81F=_T81E.l;_T820=_T81F.first_line;_T18AB.varloc=Cyc_Position_loc_to_seg(_T820);_T18AB.tms=0;_T819=_T18AB;}yyval=Cyc_YY28(_T819);goto _LL0;case 197: _T821=yyyvsp;_T822=_T821[1];
# 2029 "parse.y"
yyval=_T822.v;goto _LL0;case 198: _T823=yyyvsp;_T824=& _T823[2].v;_T825=(union Cyc_YYSTYPE*)_T824;{
# 2033 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T825);_T827=yyr;{struct Cyc_List_List*_T18AB=_region_malloc(_T827,0U,sizeof(struct Cyc_List_List));_T829=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T18AC=_region_malloc(_T829,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T18AC->tag=5;_T82A=yyyvsp;_T82B=_T82A[1];_T82C=_T82B.l;_T82D=_T82C.first_line;
_T18AC->f1=Cyc_Position_loc_to_seg(_T82D);_T82E=yyyvsp;_T82F=& _T82E[1].v;_T830=(union Cyc_YYSTYPE*)_T82F;_T18AC->f2=Cyc_yyget_YY46(_T830);_T828=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T18AC;}_T18AB->hd=(void*)_T828;_T831=d;_T18AB->tl=_T831.tms;_T826=(struct Cyc_List_List*)_T18AB;}d.tms=_T826;_T832=yyyvsp;_T833=_T832[2];
yyval=_T833.v;goto _LL0;}case 199: _T834=yyyvsp;_T835=& _T834[0].v;_T836=(union Cyc_YYSTYPE*)_T835;{
# 2038 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T836);{struct Cyc_Parse_Declarator _T18AB;_T838=one;
_T18AB.id=_T838.id;_T839=one;_T18AB.varloc=_T839.varloc;_T83B=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T83B,0U,sizeof(struct Cyc_List_List));_T83D=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T83D,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T18AD->tag=0;_T83E=yyyvsp;_T83F=& _T83E[3].v;_T840=(union Cyc_YYSTYPE*)_T83F;
_T18AD->f1=Cyc_yyget_YY54(_T840);_T841=yyyvsp;_T842=_T841[3];_T843=_T842.l;_T844=_T843.first_line;_T18AD->f2=Cyc_Position_loc_to_seg(_T844);_T83C=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T83C;_T845=one;_T18AC->tl=_T845.tms;_T83A=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_T83A;_T837=_T18AB;}
# 2039
yyval=Cyc_YY28(_T837);goto _LL0;}case 200: _T846=yyyvsp;_T847=& _T846[0].v;_T848=(union Cyc_YYSTYPE*)_T847;{
# 2042 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T848);{struct Cyc_Parse_Declarator _T18AB;_T84A=one;
_T18AB.id=_T84A.id;_T84B=one;_T18AB.varloc=_T84B.varloc;_T84D=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T84D,0U,sizeof(struct Cyc_List_List));_T84F=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T84F,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T18AD->tag=1;_T850=yyyvsp;_T851=& _T850[2].v;_T852=(union Cyc_YYSTYPE*)_T851;
_T18AD->f1=Cyc_yyget_Exp_tok(_T852);_T853=yyyvsp;_T854=& _T853[4].v;_T855=(union Cyc_YYSTYPE*)_T854;_T18AD->f2=Cyc_yyget_YY54(_T855);_T856=yyyvsp;_T857=_T856[4];_T858=_T857.l;_T859=_T858.first_line;_T18AD->f3=Cyc_Position_loc_to_seg(_T859);_T84E=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T84E;_T85A=one;
_T18AC->tl=_T85A.tms;_T84C=(struct Cyc_List_List*)_T18AC;}
# 2044
_T18AB.tms=_T84C;_T849=_T18AB;}
# 2043
yyval=Cyc_YY28(_T849);goto _LL0;}case 201: _T85B=yyyvsp;_T85C=& _T85B[2].v;_T85D=(union Cyc_YYSTYPE*)_T85C;{
# 2047 "parse.y"
struct _tuple27*_T18AB=Cyc_yyget_YY40(_T85D);struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;void*_T18AE;struct Cyc_Absyn_VarargInfo*_T18AF;int _T18B0;struct Cyc_List_List*_T18B1;{struct _tuple27 _T18B2=*_T18AB;_T18B1=_T18B2.f0;_T18B0=_T18B2.f1;_T18AF=_T18B2.f2;_T18AE=_T18B2.f3;_T18AD=_T18B2.f4;_T18AC=_T18B2.f5;}{struct Cyc_List_List*lis=_T18B1;int b=_T18B0;struct Cyc_Absyn_VarargInfo*c=_T18AF;void*eff=_T18AE;struct Cyc_List_List*ec=_T18AD;struct Cyc_List_List*qb=_T18AC;_T85E=yyyvsp;_T85F=& _T85E[4].v;_T860=(union Cyc_YYSTYPE*)_T85F;{
struct _tuple21 _T18B2=Cyc_yyget_YY62(_T860);struct Cyc_Absyn_Exp*_T18B3;struct Cyc_Absyn_Exp*_T18B4;struct Cyc_Absyn_Exp*_T18B5;struct Cyc_Absyn_Exp*_T18B6;_T18B6=_T18B2.f0;_T18B5=_T18B2.f1;_T18B4=_T18B2.f2;_T18B3=_T18B2.f3;{struct Cyc_Absyn_Exp*chk=_T18B6;struct Cyc_Absyn_Exp*req=_T18B5;struct Cyc_Absyn_Exp*ens=_T18B4;struct Cyc_Absyn_Exp*thrws=_T18B3;_T861=yyyvsp;_T862=& _T861[0].v;_T863=(union Cyc_YYSTYPE*)_T862;{
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T863);{struct Cyc_Parse_Declarator _T18B7;_T865=one;
_T18B7.id=_T865.id;_T866=one;_T18B7.varloc=_T866.varloc;_T868=yyr;{struct Cyc_List_List*_T18B8=_region_malloc(_T868,0U,sizeof(struct Cyc_List_List));_T86A=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T18B9=_region_malloc(_T86A,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T18B9->tag=3;_T86C=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T18BA=_region_malloc(_T86C,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T18BA->tag=1;_T18BA->f1=lis;_T18BA->f2=b;_T18BA->f3=c;_T18BA->f4=eff;_T18BA->f5=ec;_T18BA->f6=qb;_T18BA->f7=chk;_T18BA->f8=req;_T18BA->f9=ens;_T18BA->f10=thrws;_T86B=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T18BA;}_T18B9->f1=(void*)_T86B;_T869=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T18B9;}_T18B8->hd=(void*)_T869;_T86D=one;_T18B8->tl=_T86D.tms;_T867=(struct Cyc_List_List*)_T18B8;}_T18B7.tms=_T867;_T864=_T18B7;}yyval=Cyc_YY28(_T864);goto _LL0;}}}}}case 202: _T86E=yyyvsp;_T86F=& _T86E[0].v;_T870=(union Cyc_YYSTYPE*)_T86F;{
# 2053 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T870);{struct Cyc_Parse_Declarator _T18AB;_T872=one;
_T18AB.id=_T872.id;_T873=one;_T18AB.varloc=_T873.varloc;_T875=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T875,0U,sizeof(struct Cyc_List_List));_T877=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T877,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T18AD->tag=3;_T879=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T18AE=_region_malloc(_T879,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T18AE->tag=0;_T87A=yyyvsp;_T87B=& _T87A[2].v;_T87C=(union Cyc_YYSTYPE*)_T87B;_T18AE->f1=Cyc_yyget_YY37(_T87C);_T87D=yyyvsp;_T87E=_T87D[0];_T87F=_T87E.l;_T880=_T87F.first_line;_T18AE->f2=Cyc_Position_loc_to_seg(_T880);_T878=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T18AE;}_T18AD->f1=(void*)_T878;_T876=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T876;_T881=one;_T18AC->tl=_T881.tms;_T874=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_T874;_T871=_T18AB;}yyval=Cyc_YY28(_T871);goto _LL0;}case 203: _T883=Cyc_List_map_c;{
# 2057
struct Cyc_List_List*(*_T18AB)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T883;_T882=_T18AB;}_T884=yyyvsp;_T885=_T884[1];_T886=_T885.l;_T887=_T886.first_line;_T888=Cyc_Position_loc_to_seg(_T887);_T889=yyyvsp;_T88A=& _T889[2].v;_T88B=(union Cyc_YYSTYPE*)_T88A;_T88C=Cyc_yyget_YY41(_T88B);_T88D=Cyc_List_imp_rev(_T88C);{struct Cyc_List_List*ts=_T882(Cyc_Parse_typ2tvar,_T888,_T88D);_T88E=yyyvsp;_T88F=& _T88E[0].v;_T890=(union Cyc_YYSTYPE*)_T88F;{
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T890);{struct Cyc_Parse_Declarator _T18AB;_T892=one;
_T18AB.id=_T892.id;_T893=one;_T18AB.varloc=_T893.varloc;_T895=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T895,0U,sizeof(struct Cyc_List_List));_T897=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T897,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T18AD->tag=4;_T18AD->f1=ts;_T898=yyyvsp;_T899=_T898[0];_T89A=_T899.l;_T89B=_T89A.first_line;_T18AD->f2=Cyc_Position_loc_to_seg(_T89B);_T18AD->f3=0;_T896=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T896;_T89C=one;_T18AC->tl=_T89C.tms;_T894=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_T894;_T891=_T18AB;}yyval=Cyc_YY28(_T891);goto _LL0;}}case 204: _T89D=yyyvsp;_T89E=& _T89D[0].v;_T89F=(union Cyc_YYSTYPE*)_T89E;{
# 2061 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T89F);{struct Cyc_Parse_Declarator _T18AB;_T8A1=one;
_T18AB.id=_T8A1.id;_T8A2=one;_T18AB.varloc=_T8A2.varloc;_T8A4=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_T8A4,0U,sizeof(struct Cyc_List_List));_T8A6=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_T8A6,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T18AD->tag=5;_T8A7=yyyvsp;_T8A8=_T8A7[1];_T8A9=_T8A8.l;_T8AA=_T8A9.first_line;_T18AD->f1=Cyc_Position_loc_to_seg(_T8AA);_T8AB=yyyvsp;_T8AC=& _T8AB[1].v;_T8AD=(union Cyc_YYSTYPE*)_T8AC;_T18AD->f2=Cyc_yyget_YY46(_T8AD);_T8A5=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_T8A5;_T8AE=one;_T18AC->tl=_T8AE.tms;_T8A3=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_T8A3;_T8A0=_T18AB;}yyval=Cyc_YY28(_T8A0);goto _LL0;}case 205: _T8AF=yyyvsp;_T8B0=_T8AF[0];
# 2067 "parse.y"
yyval=_T8B0.v;goto _LL0;case 206: _T8B1=yyyvsp;_T8B2=& _T8B1[0].v;_T8B3=(union Cyc_YYSTYPE*)_T8B2;_T8B4=
# 2068 "parse.y"
Cyc_yyget_YY27(_T8B3);_T8B5=yyyvsp;_T8B6=& _T8B5[1].v;_T8B7=(union Cyc_YYSTYPE*)_T8B6;_T8B8=Cyc_yyget_YY27(_T8B7);_T8B9=Cyc_List_imp_append(_T8B4,_T8B8);yyval=Cyc_YY27(_T8B9);goto _LL0;case 207:  {
# 2072 "parse.y"
struct Cyc_List_List*ans=0;_T8BA=yyyvsp;_T8BB=& _T8BA[3].v;_T8BC=(union Cyc_YYSTYPE*)_T8BB;_T8BD=
Cyc_yyget_YY46(_T8BC);if(_T8BD==0)goto _TL2B5;_T8BF=yyr;{struct Cyc_List_List*_T18AB=_region_malloc(_T8BF,0U,sizeof(struct Cyc_List_List));_T8C1=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T18AC=_region_malloc(_T8C1,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T18AC->tag=5;_T8C2=yyyvsp;_T8C3=_T8C2[3];_T8C4=_T8C3.l;_T8C5=_T8C4.first_line;
_T18AC->f1=Cyc_Position_loc_to_seg(_T8C5);_T8C6=yyyvsp;_T8C7=& _T8C6[3].v;_T8C8=(union Cyc_YYSTYPE*)_T8C7;_T18AC->f2=Cyc_yyget_YY46(_T8C8);_T8C0=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T18AC;}_T18AB->hd=(void*)_T8C0;_T18AB->tl=ans;_T8BE=(struct Cyc_List_List*)_T18AB;}ans=_T8BE;goto _TL2B6;_TL2B5: _TL2B6: {
# 2076
struct Cyc_Absyn_PtrLoc*ptrloc=0;_T8C9=yyyvsp;_T8CA=& _T8C9[0].v;_T8CB=(union Cyc_YYSTYPE*)_T8CA;_T8CC=
Cyc_yyget_YY1(_T8CB);{struct _tuple22 _T18AB=*_T8CC;void*_T18AC;void*_T18AD;unsigned _T18AE;_T18AE=_T18AB.f0;_T18AD=_T18AB.f1;_T18AC=_T18AB.f2;{unsigned ploc=_T18AE;void*nullable=_T18AD;void*bound=_T18AC;_T8CD=Cyc_Flags_porting_c_code;
if(!_T8CD)goto _TL2B7;{struct Cyc_Absyn_PtrLoc*_T18AF=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));
_T18AF->ptr_loc=ploc;_T8CF=yyyvsp;_T8D0=_T8CF[2];_T8D1=_T8D0.l;_T8D2=_T8D1.first_line;_T18AF->rgn_loc=Cyc_Position_loc_to_seg(_T8D2);_T8D3=yyyvsp;_T8D4=_T8D3[1];_T8D5=_T8D4.l;_T8D6=_T8D5.first_line;_T18AF->zt_loc=Cyc_Position_loc_to_seg(_T8D6);_T8CE=(struct Cyc_Absyn_PtrLoc*)_T18AF;}ptrloc=_T8CE;goto _TL2B8;_TL2B7: _TL2B8: _T8D7=yyr;_T8D8=ptrloc;_T8D9=nullable;_T8DA=bound;_T8DB=yyyvsp;_T8DC=& _T8DB[2].v;_T8DD=(union Cyc_YYSTYPE*)_T8DC;_T8DE=
Cyc_yyget_YY45(_T8DD);_T8DF=yyyvsp;_T8E0=& _T8DF[1].v;_T8E1=(union Cyc_YYSTYPE*)_T8E0;_T8E2=Cyc_yyget_YY60(_T8E1);_T8E3=yyyvsp;_T8E4=& _T8E3[4].v;_T8E5=(union Cyc_YYSTYPE*)_T8E4;_T8E6=Cyc_yyget_YY24(_T8E5);{void*mod=Cyc_Parse_make_pointer_mod(_T8D7,_T8D8,_T8D9,_T8DA,_T8DE,_T8E2,_T8E6);_T8E8=yyr;{struct Cyc_List_List*_T18AF=_region_malloc(_T8E8,0U,sizeof(struct Cyc_List_List));
_T18AF->hd=mod;_T18AF->tl=ans;_T8E7=(struct Cyc_List_List*)_T18AF;}ans=_T8E7;
yyval=Cyc_YY27(ans);goto _LL0;}}}}}case 208:
# 2086
 yyval=Cyc_YY60(0);goto _LL0;case 209: _T8EA=yyr;{struct Cyc_List_List*_T18AB=_region_malloc(_T8EA,0U,sizeof(struct Cyc_List_List));_T8EB=yyyvsp;_T8EC=& _T8EB[0].v;_T8ED=(union Cyc_YYSTYPE*)_T8EC;
# 2087 "parse.y"
_T18AB->hd=Cyc_yyget_YY59(_T8ED);_T8EE=yyyvsp;_T8EF=& _T8EE[1].v;_T8F0=(union Cyc_YYSTYPE*)_T8EF;_T18AB->tl=Cyc_yyget_YY60(_T8F0);_T8E9=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY60(_T8E9);goto _LL0;case 210: _T8F2=yyr;{struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T8F2,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_T18AB->tag=0;_T8F3=yyyvsp;_T8F4=& _T8F3[2].v;_T8F5=(union Cyc_YYSTYPE*)_T8F4;
# 2092 "parse.y"
_T18AB->f1=Cyc_yyget_Exp_tok(_T8F5);_T8F1=(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T8F6=(void*)_T8F1;yyval=Cyc_YY59(_T8F6);goto _LL0;case 211: _T8F7=yyyvsp;_T8F8=_T8F7[0];_T8F9=_T8F8.l;_T8FA=_T8F9.first_line;_T8FB=
# 2094 "parse.y"
Cyc_Position_loc_to_seg(_T8FA);_T8FC=_tag_fat("@region qualifiers are deprecated; use @effect instead",sizeof(char),55U);_T8FD=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T8FB,_T8FC,_T8FD);_T8FF=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T8FF,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T18AB->tag=2;{struct Cyc_List_List*_T18AC=_cycalloc(sizeof(struct Cyc_List_List));_T901=yyyvsp;_T902=& _T901[2].v;_T903=(union Cyc_YYSTYPE*)_T902;
_T18AC->hd=Cyc_yyget_YY45(_T903);_T18AC->tl=0;_T900=(struct Cyc_List_List*)_T18AC;}_T18AB->f1=_T900;_T8FE=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T904=(void*)_T8FE;yyval=Cyc_YY59(_T904);goto _LL0;case 212: _T906=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T906,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T18AB->tag=2;_T907=yyyvsp;_T908=& _T907[2].v;_T909=(union Cyc_YYSTYPE*)_T908;
# 2096 "parse.y"
_T18AB->f1=Cyc_yyget_YY41(_T909);_T905=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T90A=(void*)_T905;yyval=Cyc_YY59(_T90A);goto _LL0;case 213: _T90C=yyr;{struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T90C,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));
# 2097 "parse.y"
_T18AB->tag=3;_T90B=(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T90D=(void*)_T90B;yyval=Cyc_YY59(_T90D);goto _LL0;case 214: _T90F=yyr;{struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T90F,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));
# 2098 "parse.y"
_T18AB->tag=4;_T90E=(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T910=(void*)_T90E;yyval=Cyc_YY59(_T910);goto _LL0;case 215: _T912=yyr;{struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T912,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));
# 2099 "parse.y"
_T18AB->tag=7;_T911=(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T913=(void*)_T911;yyval=Cyc_YY59(_T913);goto _LL0;case 216: _T915=yyr;{struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T915,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2100 "parse.y"
_T18AB->tag=5;_T914=(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T916=(void*)_T914;yyval=Cyc_YY59(_T916);goto _LL0;case 217: _T918=yyr;{struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T918,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2101 "parse.y"
_T18AB->tag=6;_T917=(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T919=(void*)_T917;yyval=Cyc_YY59(_T919);goto _LL0;case 218: _T91B=yyr;{struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T91B,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));
# 2102 "parse.y"
_T18AB->tag=8;_T91A=(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T91C=(void*)_T91A;yyval=Cyc_YY59(_T91C);goto _LL0;case 219: _T91E=yyr;{struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T91E,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));
# 2103 "parse.y"
_T18AB->tag=9;_T91D=(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T91F=(void*)_T91D;yyval=Cyc_YY59(_T91F);goto _LL0;case 220: _T921=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T921,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T18AB->tag=10;_T922=yyyvsp;_T923=& _T922[2].v;_T924=(union Cyc_YYSTYPE*)_T923;
# 2104 "parse.y"
_T18AB->f1=Cyc_yyget_YY58(_T924);_T920=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T925=(void*)_T920;yyval=Cyc_YY59(_T925);goto _LL0;case 221: _T926=yyyvsp;_T927=_T926[0];_T928=_T927.l;_T929=_T928.first_line;_T92A=
# 2105 "parse.y"
Cyc_Position_loc_to_seg(_T929);_T92B=yyyvsp;_T92C=& _T92B[0].v;_T92D=(union Cyc_YYSTYPE*)_T92C;_T92E=Cyc_yyget_String_tok(_T92D);{void*aq=Cyc_Parse_id2aqual(_T92A,_T92E);_T930=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T18AB=_region_malloc(_T930,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T18AB->tag=10;_T18AB->f1=aq;_T92F=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T18AB;}_T931=(void*)_T92F;yyval=Cyc_YY59(_T931);goto _LL0;}case 222: _T932=yyyvsp;_T933=& _T932[0].v;_T934=(union Cyc_YYSTYPE*)_T933;_T935=
# 2109 "parse.y"
Cyc_yyget_YY58(_T934);yyval=Cyc_YY58(_T935);goto _LL0;case 223: _T936=yyyvsp;_T937=& _T936[0].v;_T938=(union Cyc_YYSTYPE*)_T937;_T939=
# 2115 "parse.y"
Cyc_yyget_YY45(_T938);_T93A=Cyc_Absyn_al_qual_type;_T93B=Cyc_Absyn_aqual_var_type(_T939,_T93A);yyval=Cyc_YY58(_T93B);goto _LL0;case 224: _T93C=yyyvsp;_T93D=& _T93C[2].v;_T93E=(union Cyc_YYSTYPE*)_T93D;_T93F=
# 2119 "parse.y"
Cyc_yyget_YY45(_T93E);_T940=Cyc_Absyn_aqualsof_type(_T93F);_T941=Cyc_Absyn_al_qual_type;_T942=Cyc_Absyn_aqual_var_type(_T940,_T941);yyval=Cyc_YY58(_T942);goto _LL0;case 225: goto _LL0;case 226: _T943=yyyvsp;_T944=_T943[0];_T945=_T944.l;_T946=_T945.first_line;_T947=
# 2125 "parse.y"
Cyc_Position_loc_to_seg(_T946);_T948=yyyvsp;_T949=& _T948[0].v;_T94A=(union Cyc_YYSTYPE*)_T949;_T94B=Cyc_yyget_String_tok(_T94A);_T94C=Cyc_Parse_id2aqual(_T947,_T94B);yyval=Cyc_YY58(_T94C);goto _LL0;case 227:{struct _tuple22*_T18AB=_cycalloc(sizeof(struct _tuple22));_T94E=yyyvsp;_T94F=_T94E[0];_T950=_T94F.l;_T951=_T950.first_line;
# 2131 "parse.y"
_T18AB->f0=Cyc_Position_loc_to_seg(_T951);_T18AB->f1=Cyc_Absyn_true_type;_T952=Cyc_Parse_parsing_tempest;if(!_T952)goto _TL2B9;_T18AB->f2=Cyc_Absyn_fat_bound_type;goto _TL2BA;_TL2B9: _T953=yyyvsp;_T954=_T953[0];_T955=_T954.l;_T956=_T955.first_line;_T957=Cyc_Position_loc_to_seg(_T956);_T958=Cyc_Position_string_of_segment(_T957);_T959=yyyvsp;_T95A=& _T959[1].v;_T95B=(union Cyc_YYSTYPE*)_T95A;_T95C=Cyc_yyget_YY2(_T95B);_T18AB->f2=Cyc_Parse_assign_cvar_pos(_T958,0,_T95C);_TL2BA: _T94D=(struct _tuple22*)_T18AB;}yyval=Cyc_YY1(_T94D);goto _LL0;case 228:{struct _tuple22*_T18AB=_cycalloc(sizeof(struct _tuple22));_T95E=yyyvsp;_T95F=_T95E[0];_T960=_T95F.l;_T961=_T960.first_line;
# 2132 "parse.y"
_T18AB->f0=Cyc_Position_loc_to_seg(_T961);_T18AB->f1=Cyc_Absyn_false_type;_T962=yyyvsp;_T963=_T962[0];_T964=_T963.l;_T965=_T964.first_line;_T966=Cyc_Position_loc_to_seg(_T965);_T967=Cyc_Position_string_of_segment(_T966);_T968=yyyvsp;_T969=& _T968[1].v;_T96A=(union Cyc_YYSTYPE*)_T969;_T96B=Cyc_yyget_YY2(_T96A);_T18AB->f2=Cyc_Parse_assign_cvar_pos(_T967,0,_T96B);_T95D=(struct _tuple22*)_T18AB;}yyval=Cyc_YY1(_T95D);goto _LL0;case 229: _T96D=Cyc_Flags_override_fat;
# 2133 "parse.y"
if(!_T96D)goto _TL2BB;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2BB;_T96E=Cyc_Flags_interproc;if(!_T96E)goto _TL2BB;{struct _tuple22*_T18AB=_cycalloc(sizeof(struct _tuple22));_T970=yyyvsp;_T971=_T970[0];_T972=_T971.l;_T973=_T972.first_line;
_T18AB->f0=Cyc_Position_loc_to_seg(_T973);_T18AB->f1=Cyc_Absyn_true_type;_T974=yyyvsp;_T975=_T974[0];_T976=_T975.l;_T977=_T976.first_line;_T978=Cyc_Position_loc_to_seg(_T977);_T979=Cyc_Position_string_of_segment(_T978);_T97A=& Cyc_Kinds_ptrbko;_T97B=(struct Cyc_Core_Opt*)_T97A;_T97C=Cyc_Absyn_cvar_type(_T97B);_T18AB->f2=Cyc_Parse_assign_cvar_pos(_T979,1,_T97C);_T96F=(struct _tuple22*)_T18AB;}_T96C=_T96F;goto _TL2BC;_TL2BB:{struct _tuple22*_T18AB=_cycalloc(sizeof(struct _tuple22));_T97E=yyyvsp;_T97F=_T97E[0];_T980=_T97F.l;_T981=_T980.first_line;
_T18AB->f0=Cyc_Position_loc_to_seg(_T981);_T18AB->f1=Cyc_Absyn_true_type;_T18AB->f2=Cyc_Absyn_fat_bound_type;_T97D=(struct _tuple22*)_T18AB;}_T96C=_T97D;_TL2BC:
# 2133
 yyval=Cyc_YY1(_T96C);goto _LL0;case 230: _T983=Cyc_Flags_interproc;
# 2138
if(!_T983)goto _TL2BD;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2BD;_T984=& Cyc_Kinds_ptrbko;_T985=(struct Cyc_Core_Opt*)_T984;_T982=Cyc_Absyn_cvar_type(_T985);goto _TL2BE;_TL2BD: _T982=Cyc_Absyn_bounds_one();_TL2BE: yyval=Cyc_YY2(_T982);goto _LL0;case 231: _T986=yyyvsp;_T987=& _T986[1].v;_T988=(union Cyc_YYSTYPE*)_T987;_T989=
# 2139 "parse.y"
Cyc_yyget_Exp_tok(_T988);_T98A=Cyc_Absyn_thin_bounds_exp(_T989);yyval=Cyc_YY2(_T98A);goto _LL0;case 232: _T98B=Cyc_Flags_resolve;
# 2142 "parse.y"
if(_T98B)goto _TL2BF;else{goto _TL2C1;}
_TL2C1:{int(*_T18AB)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T98C=_T18AB;}_T98D=_tag_fat("Type variable not permitted in pointer bound",sizeof(char),45U);_T98C(0U,_T98D);goto _TL2C0;_TL2BF: _TL2C0: _T98E=yyyvsp;_T98F=& _T98E[1].v;_T990=(union Cyc_YYSTYPE*)_T98F;_T991=
Cyc_yyget_String_tok(_T990);_T992=Cyc_Parse_typevar2cvar(_T991);yyval=Cyc_YY2(_T992);goto _LL0;case 233: _T993=
# 2149 "parse.y"
Cyc_Tcutil_any_bool(0);yyval=Cyc_YY54(_T993);goto _LL0;case 234:
# 2150 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_true_type);goto _LL0;case 235:
# 2151 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_false_type);goto _LL0;case 236:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T995=yyyvsp;_T996=& _T995[0].v;_T997=(union Cyc_YYSTYPE*)_T996;
# 2155 "parse.y"
_T18AB->hd=Cyc_yyget_YY45(_T997);_T18AB->tl=0;_T994=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY41(_T994);goto _LL0;case 237:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T999=yyyvsp;_T99A=& _T999[0].v;_T99B=(union Cyc_YYSTYPE*)_T99A;
# 2156 "parse.y"
_T18AB->hd=Cyc_yyget_YY45(_T99B);_T99C=yyyvsp;_T99D=& _T99C[2].v;_T99E=(union Cyc_YYSTYPE*)_T99D;_T18AB->tl=Cyc_yyget_YY41(_T99E);_T998=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY41(_T998);goto _LL0;case 238: _T99F=& Cyc_Kinds_eko;_T9A0=(struct Cyc_Core_Opt*)_T99F;_T9A1=
# 2160 "parse.y"
Cyc_Absyn_new_evar(_T9A0,0);yyval=Cyc_YY45(_T9A1);goto _LL0;case 239: _T9A2=yyyvsp;_T9A3=& _T9A2[0].v;_T9A4=(union Cyc_YYSTYPE*)_T9A3;{
# 2161 "parse.y"
struct Cyc_List_List*es=Cyc_yyget_YY41(_T9A4);_T9A5=Cyc_List_length(es);if(_T9A5!=1)goto _TL2C2;_T9A6=_check_null(es);_T9A7=_T9A6->hd;yyval=Cyc_YY45(_T9A7);goto _TL2C3;_TL2C2: _T9A8=yyyvsp;_T9A9=& _T9A8[0].v;_T9AA=(union Cyc_YYSTYPE*)_T9A9;_T9AB=Cyc_yyget_YY41(_T9AA);_T9AC=Cyc_Absyn_join_eff(_T9AB);yyval=Cyc_YY45(_T9AC);_TL2C3: goto _LL0;}case 240: _T9AD=& Cyc_Kinds_eko;_T9AE=(struct Cyc_Core_Opt*)_T9AD;_T9AF=
# 2162 "parse.y"
Cyc_Absyn_new_evar(_T9AE,0);yyval=Cyc_YY45(_T9AF);goto _LL0;case 241: _T9B0=yyvs;_T9B1=yyvsp_offset + 1;_T9B2=_check_fat_subscript(_T9B0,sizeof(struct Cyc_Yystacktype),_T9B1);_T9B3=(struct Cyc_Yystacktype*)_T9B2;_T9B4=*_T9B3;_T9B5=_T9B4.l;_T9B6=_T9B5.first_line;_T9B7=
# 2173 "parse.y"
Cyc_Position_loc_to_seg(_T9B6);_T9B8=Cyc_Absyn_empty_tqual(_T9B7);yyval=Cyc_YY24(_T9B8);goto _LL0;case 242: _T9B9=yyyvsp;_T9BA=& _T9B9[0].v;_T9BB=(union Cyc_YYSTYPE*)_T9BA;_T9BC=
# 2174 "parse.y"
Cyc_yyget_YY24(_T9BB);_T9BD=yyyvsp;_T9BE=& _T9BD[1].v;_T9BF=(union Cyc_YYSTYPE*)_T9BE;_T9C0=Cyc_yyget_YY24(_T9BF);_T9C1=Cyc_Absyn_combine_tqual(_T9BC,_T9C0);yyval=Cyc_YY24(_T9C1);goto _LL0;case 243: _T9C2=yyyvsp;_T9C3=& _T9C2[1].v;_T9C4=(union Cyc_YYSTYPE*)_T9C3;{
# 2179 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9C4);_T9C6=ec_qb;_T9C7=(unsigned)_T9C6;
if(!_T9C7)goto _TL2C4;_T9C8=ec_qb;_T9C5=*_T9C8;goto _TL2C5;_TL2C4:{struct _tuple28 _T18AB;_T18AB.f0=0;_T18AB.f1=0;_T9C9=_T18AB;}_T9C5=_T9C9;_TL2C5: {struct _tuple28 _T18AB=_T9C5;struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_List_List*ec=_T18AD;struct Cyc_List_List*qb=_T18AC;{struct _tuple27*_T18AE=_cycalloc(sizeof(struct _tuple27));
_T18AE->f0=0;_T18AE->f1=0;_T18AE->f2=0;_T9CB=yyyvsp;_T9CC=& _T9CB[0].v;_T9CD=(union Cyc_YYSTYPE*)_T9CC;_T18AE->f3=Cyc_yyget_YY50(_T9CD);_T18AE->f4=ec;_T18AE->f5=qb;_T9CA=(struct _tuple27*)_T18AE;}yyval=Cyc_YY40(_T9CA);goto _LL0;}}}case 244: _T9CE=yyyvsp;_T9CF=& _T9CE[2].v;_T9D0=(union Cyc_YYSTYPE*)_T9CF;{
# 2183 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9D0);_T9D2=ec_qb;_T9D3=(unsigned)_T9D2;
if(!_T9D3)goto _TL2C6;_T9D4=ec_qb;_T9D1=*_T9D4;goto _TL2C7;_TL2C6:{struct _tuple28 _T18AB;_T18AB.f0=0;_T18AB.f1=0;_T9D5=_T18AB;}_T9D1=_T9D5;_TL2C7: {struct _tuple28 _T18AB=_T9D1;struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_List_List*ec=_T18AD;struct Cyc_List_List*qb=_T18AC;{struct _tuple27*_T18AE=_cycalloc(sizeof(struct _tuple27));_T9D7=yyyvsp;_T9D8=& _T9D7[0].v;_T9D9=(union Cyc_YYSTYPE*)_T9D8;_T9DA=
Cyc_yyget_YY39(_T9D9);_T18AE->f0=Cyc_List_imp_rev(_T9DA);_T18AE->f1=0;_T18AE->f2=0;_T9DB=yyyvsp;_T9DC=& _T9DB[1].v;_T9DD=(union Cyc_YYSTYPE*)_T9DC;_T18AE->f3=Cyc_yyget_YY50(_T9DD);_T18AE->f4=ec;_T18AE->f5=qb;_T9D6=(struct _tuple27*)_T18AE;}yyval=Cyc_YY40(_T9D6);goto _LL0;}}}case 245: _T9DE=yyyvsp;_T9DF=& _T9DE[4].v;_T9E0=(union Cyc_YYSTYPE*)_T9DF;{
# 2187 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9E0);_T9E2=ec_qb;_T9E3=(unsigned)_T9E2;
if(!_T9E3)goto _TL2C8;_T9E4=ec_qb;_T9E1=*_T9E4;goto _TL2C9;_TL2C8:{struct _tuple28 _T18AB;_T18AB.f0=0;_T18AB.f1=0;_T9E5=_T18AB;}_T9E1=_T9E5;_TL2C9: {struct _tuple28 _T18AB=_T9E1;struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_List_List*ec=_T18AD;struct Cyc_List_List*qb=_T18AC;{struct _tuple27*_T18AE=_cycalloc(sizeof(struct _tuple27));_T9E7=yyyvsp;_T9E8=& _T9E7[0].v;_T9E9=(union Cyc_YYSTYPE*)_T9E8;_T9EA=
Cyc_yyget_YY39(_T9E9);_T18AE->f0=Cyc_List_imp_rev(_T9EA);_T18AE->f1=1;_T18AE->f2=0;_T9EB=yyyvsp;_T9EC=& _T9EB[3].v;_T9ED=(union Cyc_YYSTYPE*)_T9EC;_T18AE->f3=Cyc_yyget_YY50(_T9ED);_T18AE->f4=ec;_T18AE->f5=qb;_T9E6=(struct _tuple27*)_T18AE;}yyval=Cyc_YY40(_T9E6);goto _LL0;}}}case 246: _T9EE=yyyvsp;_T9EF=& _T9EE[2].v;_T9F0=(union Cyc_YYSTYPE*)_T9EF;{
# 2192
struct _tuple8*_T18AB=Cyc_yyget_YY38(_T9F0);void*_T18AC;struct Cyc_Absyn_Tqual _T18AD;struct _fat_ptr*_T18AE;{struct _tuple8 _T18AF=*_T18AB;_T18AE=_T18AF.f0;_T18AD=_T18AF.f1;_T18AC=_T18AF.f2;}{struct _fat_ptr*n=_T18AE;struct Cyc_Absyn_Tqual tq=_T18AD;void*t=_T18AC;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9F1=v;_T9F1->name=n;_T9F2=v;_T9F2->tq=tq;_T9F3=v;_T9F3->type=t;_T9F4=v;_T9F5=yyyvsp;_T9F6=& _T9F5[1].v;_T9F7=(union Cyc_YYSTYPE*)_T9F6;_T9F4->inject=Cyc_yyget_YY32(_T9F7);_T9F8=yyyvsp;_T9F9=& _T9F8[4].v;_T9FA=(union Cyc_YYSTYPE*)_T9F9;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9FA);_T9FC=ec_qb;_T9FD=(unsigned)_T9FC;
if(!_T9FD)goto _TL2CA;_T9FE=ec_qb;_T9FB=*_T9FE;goto _TL2CB;_TL2CA:{struct _tuple28 _T18AF;_T18AF.f0=0;_T18AF.f1=0;_T9FF=_T18AF;}_T9FB=_T9FF;_TL2CB: {struct _tuple28 _T18AF=_T9FB;struct Cyc_List_List*_T18B0;struct Cyc_List_List*_T18B1;_T18B1=_T18AF.f0;_T18B0=_T18AF.f1;{struct Cyc_List_List*ec=_T18B1;struct Cyc_List_List*qb=_T18B0;{struct _tuple27*_T18B2=_cycalloc(sizeof(struct _tuple27));
_T18B2->f0=0;_T18B2->f1=0;_T18B2->f2=v;_TA01=yyyvsp;_TA02=& _TA01[3].v;_TA03=(union Cyc_YYSTYPE*)_TA02;_T18B2->f3=Cyc_yyget_YY50(_TA03);_T18B2->f4=ec;_T18B2->f5=qb;_TA00=(struct _tuple27*)_T18B2;}yyval=Cyc_YY40(_TA00);goto _LL0;}}}}}case 247: _TA04=yyyvsp;_TA05=& _TA04[4].v;_TA06=(union Cyc_YYSTYPE*)_TA05;{
# 2200
struct _tuple8*_T18AB=Cyc_yyget_YY38(_TA06);void*_T18AC;struct Cyc_Absyn_Tqual _T18AD;struct _fat_ptr*_T18AE;{struct _tuple8 _T18AF=*_T18AB;_T18AE=_T18AF.f0;_T18AD=_T18AF.f1;_T18AC=_T18AF.f2;}{struct _fat_ptr*n=_T18AE;struct Cyc_Absyn_Tqual tq=_T18AD;void*t=_T18AC;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_TA07=v;_TA07->name=n;_TA08=v;_TA08->tq=tq;_TA09=v;_TA09->type=t;_TA0A=v;_TA0B=yyyvsp;_TA0C=& _TA0B[3].v;_TA0D=(union Cyc_YYSTYPE*)_TA0C;_TA0A->inject=Cyc_yyget_YY32(_TA0D);_TA0E=yyyvsp;_TA0F=& _TA0E[6].v;_TA10=(union Cyc_YYSTYPE*)_TA0F;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_TA10);_TA12=ec_qb;_TA13=(unsigned)_TA12;
if(!_TA13)goto _TL2CC;_TA14=ec_qb;_TA11=*_TA14;goto _TL2CD;_TL2CC:{struct _tuple28 _T18AF;_T18AF.f0=0;_T18AF.f1=0;_TA15=_T18AF;}_TA11=_TA15;_TL2CD: {struct _tuple28 _T18AF=_TA11;struct Cyc_List_List*_T18B0;struct Cyc_List_List*_T18B1;_T18B1=_T18AF.f0;_T18B0=_T18AF.f1;{struct Cyc_List_List*ec=_T18B1;struct Cyc_List_List*qb=_T18B0;{struct _tuple27*_T18B2=_cycalloc(sizeof(struct _tuple27));_TA17=yyyvsp;_TA18=& _TA17[0].v;_TA19=(union Cyc_YYSTYPE*)_TA18;_TA1A=
Cyc_yyget_YY39(_TA19);_T18B2->f0=Cyc_List_imp_rev(_TA1A);_T18B2->f1=0;_T18B2->f2=v;_TA1B=yyyvsp;_TA1C=& _TA1B[5].v;_TA1D=(union Cyc_YYSTYPE*)_TA1C;_T18B2->f3=Cyc_yyget_YY50(_TA1D);_T18B2->f4=ec;_T18B2->f5=qb;_TA16=(struct _tuple27*)_T18B2;}yyval=Cyc_YY40(_TA16);goto _LL0;}}}}}case 248:
# 2210 "parse.y"
 yyval=Cyc_YY50(0);goto _LL0;case 249: _TA1E=yyyvsp;_TA1F=_TA1E[0];_TA20=_TA1F.l;_TA21=_TA20.first_line;_TA22=
# 2211 "parse.y"
Cyc_Position_loc_to_seg(_TA21);_TA23=yyyvsp;_TA24=& _TA23[0].v;_TA25=(union Cyc_YYSTYPE*)_TA24;_TA26=Cyc_yyget_String_tok(_TA25);_TA27=Cyc_Parse_id2aqual(_TA22,_TA26);yyval=Cyc_YY50(_TA27);goto _LL0;case 250: _TA28=yyyvsp;_TA29=& _TA28[0].v;_TA2A=(union Cyc_YYSTYPE*)_TA29;_TA2B=
# 2214
Cyc_yyget_String_tok(_TA2A);{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_T18AB->tag=1;_T18AB->f1=0;_TA2C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T18AB;}_TA2D=(void*)_TA2C;_TA2E=yyyvsp;_TA2F=& _TA2E[1].v;_TA30=(union Cyc_YYSTYPE*)_TA2F;_TA31=Cyc_yyget_YY50(_TA30);_TA32=yyyvsp;_TA33=_TA32[0];_TA34=_TA33.l;_TA35=_TA34.first_line;_TA36=Cyc_Position_loc_to_seg(_TA35);_TA37=Cyc_Parse_id2type(_TA2B,_TA2D,_TA31,_TA36);yyval=Cyc_YY45(_TA37);goto _LL0;case 251: _TA38=yyyvsp;_TA39=& _TA38[0].v;_TA3A=(union Cyc_YYSTYPE*)_TA39;_TA3B=
# 2215 "parse.y"
Cyc_yyget_String_tok(_TA3A);_TA3C=yyyvsp;_TA3D=& _TA3C[2].v;_TA3E=(union Cyc_YYSTYPE*)_TA3D;_TA3F=Cyc_yyget_YY44(_TA3E);_TA40=Cyc_Kinds_kind_to_bound(_TA3F);_TA41=yyyvsp;_TA42=& _TA41[3].v;_TA43=(union Cyc_YYSTYPE*)_TA42;_TA44=Cyc_yyget_YY50(_TA43);_TA45=yyyvsp;_TA46=_TA45[0];_TA47=_TA46.l;_TA48=_TA47.first_line;_TA49=Cyc_Position_loc_to_seg(_TA48);_TA4A=Cyc_Parse_id2type(_TA3B,_TA40,_TA44,_TA49);yyval=Cyc_YY45(_TA4A);goto _LL0;case 252:
# 2218
 yyval=Cyc_YY50(0);goto _LL0;case 253: _TA4B=yyyvsp;_TA4C=& _TA4B[1].v;_TA4D=(union Cyc_YYSTYPE*)_TA4C;_TA4E=
# 2219 "parse.y"
Cyc_yyget_YY41(_TA4D);_TA4F=Cyc_Absyn_join_eff(_TA4E);yyval=Cyc_YY50(_TA4F);goto _LL0;case 254:
# 2223 "parse.y"
 yyval=Cyc_YY51(0);goto _LL0;case 255: _TA50=yyyvsp;_TA51=_TA50[1];
# 2224 "parse.y"
yyval=_TA51.v;goto _LL0;case 256:{struct _tuple28*_T18AB=_cycalloc(sizeof(struct _tuple28));{void*_T18AC[1];_TA54=yyyvsp;_TA55=& _TA54[0].v;_TA56=(union Cyc_YYSTYPE*)_TA55;_T18AC[0]=
# 2229 "parse.y"
Cyc_yyget_YY52(_TA56);_TA57=_tag_fat(_T18AC,sizeof(void*),1);_TA53=Cyc_List_list(_TA57);}_T18AB->f0=_TA53;_T18AB->f1=0;_TA52=(struct _tuple28*)_T18AB;}yyval=Cyc_YY51(_TA52);goto _LL0;case 257:{struct _tuple28*_T18AB=_cycalloc(sizeof(struct _tuple28));
# 2231 "parse.y"
_T18AB->f0=0;{struct _tuple29*_T18AC[1];_TA5A=yyyvsp;_TA5B=& _TA5A[0].v;_TA5C=(union Cyc_YYSTYPE*)_TA5B;_T18AC[0]=Cyc_yyget_YY53(_TA5C);_TA5D=_tag_fat(_T18AC,sizeof(struct _tuple29*),1);_TA59=Cyc_List_list(_TA5D);}_T18AB->f1=_TA59;_TA58=(struct _tuple28*)_T18AB;}yyval=Cyc_YY51(_TA58);goto _LL0;case 258: _TA5E=yyyvsp;_TA5F=& _TA5E[2].v;_TA60=(union Cyc_YYSTYPE*)_TA5F;{
# 2234 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(_TA60);_TA61=
_check_null(rest);{struct _tuple28 _T18AB=*_TA61;struct Cyc_List_List*_T18AC;_T18AC=_T18AB.f0;{struct Cyc_List_List*rpo=_T18AC;_TA62=rest;{struct Cyc_List_List*_T18AD=_cycalloc(sizeof(struct Cyc_List_List));_TA64=yyyvsp;_TA65=& _TA64[0].v;_TA66=(union Cyc_YYSTYPE*)_TA65;
_T18AD->hd=Cyc_yyget_YY52(_TA66);_T18AD->tl=rpo;_TA63=(struct Cyc_List_List*)_T18AD;}(*_TA62).f0=_TA63;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 259: _TA67=yyyvsp;_TA68=& _TA67[2].v;_TA69=(union Cyc_YYSTYPE*)_TA68;{
# 2241 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(_TA69);_TA6A=
_check_null(rest);{struct _tuple28 _T18AB=*_TA6A;struct Cyc_List_List*_T18AC;_T18AC=_T18AB.f1;{struct Cyc_List_List*qb=_T18AC;_TA6B=rest;{struct Cyc_List_List*_T18AD=_cycalloc(sizeof(struct Cyc_List_List));_TA6D=yyyvsp;_TA6E=& _TA6D[0].v;_TA6F=(union Cyc_YYSTYPE*)_TA6E;
_T18AD->hd=Cyc_yyget_YY53(_TA6F);_T18AD->tl=qb;_TA6C=(struct Cyc_List_List*)_T18AD;}(*_TA6B).f1=_TA6C;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 260:  {
# 2251 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_TA70=kb;_TA70->tag=2;_TA71=kb;_TA71->f1=0;_TA72=kb;_TA73=& Cyc_Kinds_ek;_TA72->f2=(struct Cyc_Absyn_Kind*)_TA73;_TA74=yyyvsp;_TA75=& _TA74[2].v;_TA76=(union Cyc_YYSTYPE*)_TA75;_TA77=
Cyc_yyget_String_tok(_TA76);_TA78=kb;_TA79=(void*)_TA78;_TA7A=yyyvsp;_TA7B=_TA7A[2];_TA7C=_TA7B.l;_TA7D=_TA7C.first_line;_TA7E=Cyc_Position_loc_to_seg(_TA7D);{void*t=Cyc_Parse_id2type(_TA77,_TA79,0,_TA7E);{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct));_T18AB->tag=2;_TA80=yyyvsp;_TA81=& _TA80[0].v;_TA82=(union Cyc_YYSTYPE*)_TA81;_TA83=
Cyc_yyget_YY41(_TA82);_T18AB->f1=Cyc_Parse_effect_from_atomic(_TA83);_T18AB->f2=t;_TA7F=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T18AB;}_TA84=(void*)_TA7F;yyval=Cyc_YY52(_TA84);goto _LL0;}}case 261:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_T18AB->tag=1;_TA86=yyyvsp;_TA87=& _TA86[0].v;_TA88=(union Cyc_YYSTYPE*)_TA87;_TA89=
# 2257 "parse.y"
Cyc_yyget_YY41(_TA88);_T18AB->f1=Cyc_Parse_effect_from_atomic(_TA89);_TA8A=yyyvsp;_TA8B=& _TA8A[2].v;_TA8C=(union Cyc_YYSTYPE*)_TA8B;_TA8D=
Cyc_yyget_YY41(_TA8C);_T18AB->f2=Cyc_Parse_effect_from_atomic(_TA8D);_TA85=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T18AB;}_TA8E=(void*)_TA85;
# 2257
yyval=Cyc_YY52(_TA8E);goto _LL0;case 262: _TA8F=yyyvsp;_TA90=_TA8F[0];_TA91=_TA90.l;_TA92=_TA91.first_line;_TA93=
# 2262 "parse.y"
Cyc_Position_loc_to_seg(_TA92);_TA94=yyyvsp;_TA95=& _TA94[0].v;_TA96=(union Cyc_YYSTYPE*)_TA95;_TA97=Cyc_yyget_String_tok(_TA96);Cyc_Parse_check_single_constraint(_TA93,_TA97);{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_T18AB->tag=0;_TA99=yyyvsp;_TA9A=& _TA99[2].v;_TA9B=(union Cyc_YYSTYPE*)_TA9A;_TA9C=
Cyc_yyget_YY41(_TA9B);_T18AB->f1=Cyc_Parse_effect_from_atomic(_TA9C);_TA98=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T18AB;}_TA9D=(void*)_TA98;yyval=Cyc_YY52(_TA9D);goto _LL0;case 263:{struct _tuple29*_T18AB=_cycalloc(sizeof(struct _tuple29));_TA9F=yyyvsp;_TAA0=& _TA9F[2].v;_TAA1=(union Cyc_YYSTYPE*)_TAA0;
# 2270 "parse.y"
_T18AB->f0=Cyc_yyget_YY45(_TAA1);_TAA2=yyyvsp;_TAA3=& _TAA2[0].v;_TAA4=(union Cyc_YYSTYPE*)_TAA3;_T18AB->f1=Cyc_yyget_YY45(_TAA4);_TA9E=(struct _tuple29*)_T18AB;}yyval=Cyc_YY53(_TA9E);goto _LL0;case 264:
# 2275 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_al_qual_type);goto _LL0;case 265:
# 2276 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_un_qual_type);goto _LL0;case 266:
# 2277 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rc_qual_type);goto _LL0;case 267:
# 2278 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rtd_qual_type);goto _LL0;case 268: _TAA5=yyyvsp;_TAA6=_TAA5[0];_TAA7=_TAA6.l;_TAA8=_TAA7.first_line;_TAA9=
# 2279 "parse.y"
Cyc_Position_loc_to_seg(_TAA8);_TAAA=yyyvsp;_TAAB=& _TAAA[0].v;_TAAC=(union Cyc_YYSTYPE*)_TAAB;_TAAD=Cyc_yyget_String_tok(_TAAC);_TAAE=Cyc_Parse_id2aqual(_TAA9,_TAAD);yyval=Cyc_YY58(_TAAE);goto _LL0;case 269: _TAAF=yyyvsp;_TAB0=& _TAAF[0].v;_TAB1=(union Cyc_YYSTYPE*)_TAB0;_TAB2=
# 2284 "parse.y"
Cyc_yyget_YY58(_TAB1);yyval=Cyc_YY45(_TAB2);goto _LL0;case 270: _TAB3=yyyvsp;_TAB4=& _TAB3[2].v;_TAB5=(union Cyc_YYSTYPE*)_TAB4;_TAB6=
# 2287 "parse.y"
Cyc_yyget_YY45(_TAB5);_TAB7=Cyc_Absyn_aqualsof_type(_TAB6);_TAB8=Cyc_Absyn_al_qual_type;_TAB9=Cyc_Absyn_aqual_var_type(_TAB7,_TAB8);yyval=Cyc_YY45(_TAB9);goto _LL0;case 271:  {
# 2294 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_TABA=kb;_TABA->tag=0;_TABB=kb;_TABC=& Cyc_Kinds_aqk;_TABB->f1=(struct Cyc_Absyn_Kind*)_TABC;_TABD=yyyvsp;_TABE=& _TABD[0].v;_TABF=(union Cyc_YYSTYPE*)_TABE;_TAC0=
Cyc_yyget_String_tok(_TABF);_TAC1=kb;_TAC2=(void*)_TAC1;_TAC3=yyyvsp;_TAC4=_TAC3[0];_TAC5=_TAC4.l;_TAC6=_TAC5.first_line;_TAC7=Cyc_Position_loc_to_seg(_TAC6);_TAC8=Cyc_Parse_id2type(_TAC0,_TAC2,0,_TAC7);yyval=Cyc_YY45(_TAC8);goto _LL0;}case 272: _TAC9=yyyvsp;_TACA=& _TAC9[2].v;_TACB=(union Cyc_YYSTYPE*)_TACA;_TACC=
# 2299 "parse.y"
Cyc_yyget_YY45(_TACB);_TACD=Cyc_Absyn_aqualsof_type(_TACC);yyval=Cyc_YY45(_TACD);goto _LL0;case 273:
# 2328 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 274: _TACE=yyyvsp;_TACF=& _TACE[0].v;_TAD0=(union Cyc_YYSTYPE*)_TACF;_TAD1=
# 2330 "parse.y"
Cyc_yyget_String_tok(_TAD0);_TAD2=_tag_fat("inject",sizeof(char),7U);_TAD3=Cyc_zstrcmp(_TAD1,_TAD2);if(_TAD3==0)goto _TL2CE;{struct Cyc_Warn_String_Warn_Warg_struct _T18AB;_T18AB.tag=0;
_T18AB.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_TAD4=_T18AB;}{struct Cyc_Warn_String_Warn_Warg_struct _T18AB=_TAD4;void*_T18AC[1];_T18AC[0]=& _T18AB;_TAD5=yyyvsp;_TAD6=_TAD5[0];_TAD7=_TAD6.l;_TAD8=_TAD7.first_line;_TAD9=Cyc_Position_loc_to_seg(_TAD8);_TADA=_tag_fat(_T18AC,sizeof(void*),1);Cyc_Warn_err2(_TAD9,_TADA);}goto _TL2CF;_TL2CE: _TL2CF:
 yyval=Cyc_YY32(1);goto _LL0;case 275: _TADB=yyyvsp;_TADC=_TADB[0];
# 2337 "parse.y"
yyval=_TADC.v;goto _LL0;case 276: _TADD=yyyvsp;_TADE=& _TADD[0].v;_TADF=(union Cyc_YYSTYPE*)_TADE;_TAE0=
# 2338 "parse.y"
Cyc_yyget_YY41(_TADF);_TAE1=yyyvsp;_TAE2=& _TAE1[2].v;_TAE3=(union Cyc_YYSTYPE*)_TAE2;_TAE4=Cyc_yyget_YY41(_TAE3);_TAE5=Cyc_List_imp_append(_TAE0,_TAE4);yyval=Cyc_YY41(_TAE5);goto _LL0;case 277:
# 2342 "parse.y"
 yyval=Cyc_YY41(0);goto _LL0;case 278: _TAE6=yyyvsp;_TAE7=_TAE6[1];
# 2343 "parse.y"
yyval=_TAE7.v;goto _LL0;case 279:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TAE9=yyyvsp;_TAEA=& _TAE9[2].v;_TAEB=(union Cyc_YYSTYPE*)_TAEA;_TAEC=
# 2344 "parse.y"
Cyc_yyget_YY45(_TAEB);_T18AB->hd=Cyc_Absyn_regionsof_eff(_TAEC);_T18AB->tl=0;_TAE8=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY41(_TAE8);goto _LL0;case 280: _TAED=yyyvsp;_TAEE=& _TAED[0].v;_TAEF=(union Cyc_YYSTYPE*)_TAEE;_TAF0=
# 2346 "parse.y"
Cyc_yyget_YY45(_TAEF);_TAF1=& Cyc_Kinds_ek;_TAF2=(struct Cyc_Absyn_Kind*)_TAF1;Cyc_Parse_set_vartyp_kind(_TAF0,_TAF2,0);{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TAF4=yyyvsp;_TAF5=& _TAF4[0].v;_TAF6=(union Cyc_YYSTYPE*)_TAF5;
_T18AB->hd=Cyc_yyget_YY45(_TAF6);_T18AB->tl=0;_TAF3=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY41(_TAF3);goto _LL0;case 281:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TAF8=yyyvsp;_TAF9=& _TAF8[0].v;_TAFA=(union Cyc_YYSTYPE*)_TAF9;_TAFB=
# 2354 "parse.y"
Cyc_yyget_YY38(_TAFA);_TAFC=yyyvsp;_TAFD=_TAFC[0];_TAFE=_TAFD.l;_TAFF=_TAFE.first_line;_TB00=Cyc_Position_loc_to_seg(_TAFF);_T18AB->hd=Cyc_Parse_type_name_to_type(_TAFB,_TB00);_T18AB->tl=0;_TAF7=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY41(_TAF7);goto _LL0;case 282:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TB02=yyyvsp;_TB03=& _TB02[0].v;_TB04=(union Cyc_YYSTYPE*)_TB03;_TB05=
# 2356 "parse.y"
Cyc_yyget_YY38(_TB04);_TB06=yyyvsp;_TB07=_TB06[0];_TB08=_TB07.l;_TB09=_TB08.first_line;_TB0A=Cyc_Position_loc_to_seg(_TB09);_T18AB->hd=Cyc_Parse_type_name_to_type(_TB05,_TB0A);_TB0B=yyyvsp;_TB0C=& _TB0B[2].v;_TB0D=(union Cyc_YYSTYPE*)_TB0C;_T18AB->tl=Cyc_yyget_YY41(_TB0D);_TB01=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY41(_TB01);goto _LL0;case 283:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TB0F=yyyvsp;_TB10=& _TB0F[0].v;_TB11=(union Cyc_YYSTYPE*)_TB10;
# 2361 "parse.y"
_T18AB->hd=Cyc_yyget_YY38(_TB11);_T18AB->tl=0;_TB0E=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY39(_TB0E);goto _LL0;case 284:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TB13=yyyvsp;_TB14=& _TB13[2].v;_TB15=(union Cyc_YYSTYPE*)_TB14;
# 2362 "parse.y"
_T18AB->hd=Cyc_yyget_YY38(_TB15);_TB16=yyyvsp;_TB17=& _TB16[0].v;_TB18=(union Cyc_YYSTYPE*)_TB17;_T18AB->tl=Cyc_yyget_YY39(_TB18);_TB12=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY39(_TB12);goto _LL0;case 285: _TB19=yyyvsp;_TB1A=& _TB19[0].v;_TB1B=(union Cyc_YYSTYPE*)_TB1A;{
# 2368 "parse.y"
struct _tuple26 _T18AB=Cyc_yyget_YY36(_TB1B);struct Cyc_List_List*_T18AC;struct Cyc_Parse_Type_specifier _T18AD;struct Cyc_Absyn_Tqual _T18AE;_T18AE=_T18AB.f0;_T18AD=_T18AB.f1;_T18AC=_T18AB.f2;{struct Cyc_Absyn_Tqual tq=_T18AE;struct Cyc_Parse_Type_specifier tspecs=_T18AD;struct Cyc_List_List*atts=_T18AC;_TB1C=tq;_TB1D=_TB1C.loc;
if(_TB1D!=0U)goto _TL2D0;_TB1E=yyyvsp;_TB1F=_TB1E[0];_TB20=_TB1F.l;_TB21=_TB20.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB21);goto _TL2D1;_TL2D0: _TL2D1: _TB22=yyyvsp;_TB23=& _TB22[1].v;_TB24=(union Cyc_YYSTYPE*)_TB23;{
struct Cyc_Parse_Declarator _T18AF=Cyc_yyget_YY28(_TB24);struct Cyc_List_List*_T18B0;unsigned _T18B1;struct _tuple0*_T18B2;_T18B2=_T18AF.id;_T18B1=_T18AF.varloc;_T18B0=_T18AF.tms;{struct _tuple0*qv=_T18B2;unsigned varloc=_T18B1;struct Cyc_List_List*tms=_T18B0;_TB25=tspecs;_TB26=yyyvsp;_TB27=_TB26[0];_TB28=_TB27.l;_TB29=_TB28.first_line;_TB2A=
Cyc_Position_loc_to_seg(_TB29);{void*t=Cyc_Parse_speclist2typ(_TB25,_TB2A);
struct _tuple14 _T18B3=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T18B4;struct Cyc_List_List*_T18B5;void*_T18B6;struct Cyc_Absyn_Tqual _T18B7;_T18B7=_T18B3.f0;_T18B6=_T18B3.f1;_T18B5=_T18B3.f2;_T18B4=_T18B3.f3;{struct Cyc_Absyn_Tqual tq2=_T18B7;void*t2=_T18B6;struct Cyc_List_List*tvs=_T18B5;struct Cyc_List_List*atts2=_T18B4;
if(tvs==0)goto _TL2D2;{struct Cyc_Warn_String_Warn_Warg_struct _T18B8;_T18B8.tag=0;
_T18B8.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_TB2B=_T18B8;}{struct Cyc_Warn_String_Warn_Warg_struct _T18B8=_TB2B;void*_T18B9[1];_T18B9[0]=& _T18B8;_TB2C=yyyvsp;_TB2D=_TB2C[1];_TB2E=_TB2D.l;_TB2F=_TB2E.first_line;_TB30=Cyc_Position_loc_to_seg(_TB2F);_TB31=_tag_fat(_T18B9,sizeof(void*),1);Cyc_Warn_err2(_TB30,_TB31);}goto _TL2D3;_TL2D2: _TL2D3: _TB32=
Cyc_Absyn_is_qvar_qualified(qv);if(!_TB32)goto _TL2D4;{struct Cyc_Warn_String_Warn_Warg_struct _T18B8;_T18B8.tag=0;
_T18B8.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_TB33=_T18B8;}{struct Cyc_Warn_String_Warn_Warg_struct _T18B8=_TB33;void*_T18B9[1];_T18B9[0]=& _T18B8;_TB34=yyyvsp;_TB35=_TB34[0];_TB36=_TB35.l;_TB37=_TB36.first_line;_TB38=Cyc_Position_loc_to_seg(_TB37);_TB39=_tag_fat(_T18B9,sizeof(void*),1);Cyc_Warn_err2(_TB38,_TB39);}goto _TL2D5;_TL2D4: _TL2D5: _TB3A=qv;_TB3B=*_TB3A;{
struct _fat_ptr*idopt=_TB3B.f1;
if(atts2==0)goto _TL2D6;{struct Cyc_Warn_String_Warn_Warg_struct _T18B8;_T18B8.tag=0;
_T18B8.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_TB3C=_T18B8;}{struct Cyc_Warn_String_Warn_Warg_struct _T18B8=_TB3C;void*_T18B9[1];_T18B9[0]=& _T18B8;_TB3D=yyyvsp;_TB3E=_TB3D[0];_TB3F=_TB3E.l;_TB40=_TB3F.first_line;_TB41=Cyc_Position_loc_to_seg(_TB40);_TB42=_tag_fat(_T18B9,sizeof(void*),1);Cyc_Warn_warn2(_TB41,_TB42);}goto _TL2D7;_TL2D6: _TL2D7:{struct _tuple8*_T18B8=_cycalloc(sizeof(struct _tuple8));
_T18B8->f0=idopt;_T18B8->f1=tq2;_T18B8->f2=t2;_TB43=(struct _tuple8*)_T18B8;}yyval=Cyc_YY38(_TB43);goto _LL0;}}}}}}}case 286: _TB44=yyyvsp;_TB45=& _TB44[0].v;_TB46=(union Cyc_YYSTYPE*)_TB45;{
# 2383 "parse.y"
struct _tuple26 _T18AB=Cyc_yyget_YY36(_TB46);struct Cyc_List_List*_T18AC;struct Cyc_Parse_Type_specifier _T18AD;struct Cyc_Absyn_Tqual _T18AE;_T18AE=_T18AB.f0;_T18AD=_T18AB.f1;_T18AC=_T18AB.f2;{struct Cyc_Absyn_Tqual tq=_T18AE;struct Cyc_Parse_Type_specifier tspecs=_T18AD;struct Cyc_List_List*atts=_T18AC;_TB47=tq;_TB48=_TB47.loc;
if(_TB48!=0U)goto _TL2D8;_TB49=yyyvsp;_TB4A=_TB49[0];_TB4B=_TB4A.l;_TB4C=_TB4B.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB4C);goto _TL2D9;_TL2D8: _TL2D9: _TB4D=tspecs;_TB4E=yyyvsp;_TB4F=_TB4E[0];_TB50=_TB4F.l;_TB51=_TB50.first_line;_TB52=
Cyc_Position_loc_to_seg(_TB51);{void*t=Cyc_Parse_speclist2typ(_TB4D,_TB52);
if(atts==0)goto _TL2DA;{struct Cyc_Warn_String_Warn_Warg_struct _T18AF;_T18AF.tag=0;
_T18AF.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB53=_T18AF;}{struct Cyc_Warn_String_Warn_Warg_struct _T18AF=_TB53;void*_T18B0[1];_T18B0[0]=& _T18AF;_TB54=yyyvsp;_TB55=_TB54[0];_TB56=_TB55.l;_TB57=_TB56.first_line;_TB58=Cyc_Position_loc_to_seg(_TB57);_TB59=_tag_fat(_T18B0,sizeof(void*),1);Cyc_Warn_warn2(_TB58,_TB59);}goto _TL2DB;_TL2DA: _TL2DB:{struct _tuple8*_T18AF=_cycalloc(sizeof(struct _tuple8));
_T18AF->f0=0;_T18AF->f1=tq;_T18AF->f2=t;_TB5A=(struct _tuple8*)_T18AF;}yyval=Cyc_YY38(_TB5A);goto _LL0;}}}case 287: _TB5B=yyyvsp;_TB5C=& _TB5B[0].v;_TB5D=(union Cyc_YYSTYPE*)_TB5C;{
# 2391 "parse.y"
struct _tuple26 _T18AB=Cyc_yyget_YY36(_TB5D);struct Cyc_List_List*_T18AC;struct Cyc_Parse_Type_specifier _T18AD;struct Cyc_Absyn_Tqual _T18AE;_T18AE=_T18AB.f0;_T18AD=_T18AB.f1;_T18AC=_T18AB.f2;{struct Cyc_Absyn_Tqual tq=_T18AE;struct Cyc_Parse_Type_specifier tspecs=_T18AD;struct Cyc_List_List*atts=_T18AC;_TB5E=tq;_TB5F=_TB5E.loc;
if(_TB5F!=0U)goto _TL2DC;_TB60=yyyvsp;_TB61=_TB60[0];_TB62=_TB61.l;_TB63=_TB62.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB63);goto _TL2DD;_TL2DC: _TL2DD: _TB64=tspecs;_TB65=yyyvsp;_TB66=_TB65[0];_TB67=_TB66.l;_TB68=_TB67.first_line;_TB69=
Cyc_Position_loc_to_seg(_TB68);{void*t=Cyc_Parse_speclist2typ(_TB64,_TB69);_TB6A=yyyvsp;_TB6B=& _TB6A[1].v;_TB6C=(union Cyc_YYSTYPE*)_TB6B;_TB6D=
Cyc_yyget_YY31(_TB6C);{struct Cyc_List_List*tms=_TB6D.tms;
struct _tuple14 _T18AF=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T18B0;struct Cyc_List_List*_T18B1;void*_T18B2;struct Cyc_Absyn_Tqual _T18B3;_T18B3=_T18AF.f0;_T18B2=_T18AF.f1;_T18B1=_T18AF.f2;_T18B0=_T18AF.f3;{struct Cyc_Absyn_Tqual tq2=_T18B3;void*t2=_T18B2;struct Cyc_List_List*tvs=_T18B1;struct Cyc_List_List*atts2=_T18B0;
if(tvs==0)goto _TL2DE;{struct Cyc_Warn_String_Warn_Warg_struct _T18B4;_T18B4.tag=0;
# 2398
_T18B4.f1=_tag_fat("bad type parameters on formal argument, ignoring",sizeof(char),49U);_TB6E=_T18B4;}{struct Cyc_Warn_String_Warn_Warg_struct _T18B4=_TB6E;void*_T18B5[1];_T18B5[0]=& _T18B4;_TB6F=yyyvsp;_TB70=_TB6F[0];_TB71=_TB70.l;_TB72=_TB71.first_line;_TB73=
# 2397
Cyc_Position_loc_to_seg(_TB72);_TB74=_tag_fat(_T18B5,sizeof(void*),1);Cyc_Warn_warn2(_TB73,_TB74);}goto _TL2DF;_TL2DE: _TL2DF:
# 2399
 if(atts2==0)goto _TL2E0;{struct Cyc_Warn_String_Warn_Warg_struct _T18B4;_T18B4.tag=0;
_T18B4.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB75=_T18B4;}{struct Cyc_Warn_String_Warn_Warg_struct _T18B4=_TB75;void*_T18B5[1];_T18B5[0]=& _T18B4;_TB76=yyyvsp;_TB77=_TB76[0];_TB78=_TB77.l;_TB79=_TB78.first_line;_TB7A=Cyc_Position_loc_to_seg(_TB79);_TB7B=_tag_fat(_T18B5,sizeof(void*),1);Cyc_Warn_warn2(_TB7A,_TB7B);}goto _TL2E1;_TL2E0: _TL2E1:{struct _tuple8*_T18B4=_cycalloc(sizeof(struct _tuple8));
_T18B4->f0=0;_T18B4->f1=tq2;_T18B4->f2=t2;_TB7C=(struct _tuple8*)_T18B4;}yyval=Cyc_YY38(_TB7C);goto _LL0;}}}}}case 288: _TB7D=yyyvsp;_TB7E=& _TB7D[0].v;_TB7F=(union Cyc_YYSTYPE*)_TB7E;_TB80=
# 2406 "parse.y"
Cyc_yyget_YY37(_TB7F);_TB81=Cyc_List_imp_rev(_TB80);yyval=Cyc_YY37(_TB81);goto _LL0;case 289:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_TB84=yyyvsp;_TB85=& _TB84[0].v;_TB86=(union Cyc_YYSTYPE*)_TB85;
# 2409
*_T18AC=Cyc_yyget_String_tok(_TB86);_TB83=(struct _fat_ptr*)_T18AC;}_T18AB->hd=_TB83;_T18AB->tl=0;_TB82=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY37(_TB82);goto _LL0;case 290:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_TB89=yyyvsp;_TB8A=& _TB89[2].v;_TB8B=(union Cyc_YYSTYPE*)_TB8A;
# 2410 "parse.y"
*_T18AC=Cyc_yyget_String_tok(_TB8B);_TB88=(struct _fat_ptr*)_T18AC;}_T18AB->hd=_TB88;_TB8C=yyyvsp;_TB8D=& _TB8C[0].v;_TB8E=(union Cyc_YYSTYPE*)_TB8D;_T18AB->tl=Cyc_yyget_YY37(_TB8E);_TB87=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY37(_TB87);goto _LL0;case 291: _TB8F=yyyvsp;_TB90=_TB8F[0];
# 2414 "parse.y"
yyval=_TB90.v;goto _LL0;case 292: _TB91=yyyvsp;_TB92=_TB91[0];
# 2415 "parse.y"
yyval=_TB92.v;goto _LL0;case 293:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T18AB->tag=35;
# 2420 "parse.y"
_T18AB->f1=0;_T18AB->f2=0;_TB93=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T18AB;}_TB94=(void*)_TB93;_TB95=yyyvsp;_TB96=_TB95[0];_TB97=_TB96.l;_TB98=_TB97.first_line;_TB99=Cyc_Position_loc_to_seg(_TB98);_TB9A=Cyc_Absyn_new_exp(_TB94,_TB99);yyval=Cyc_Exp_tok(_TB9A);goto _LL0;case 294:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T18AB->tag=35;
# 2422 "parse.y"
_T18AB->f1=0;_TB9C=yyyvsp;_TB9D=& _TB9C[1].v;_TB9E=(union Cyc_YYSTYPE*)_TB9D;_TB9F=Cyc_yyget_YY5(_TB9E);_T18AB->f2=Cyc_List_imp_rev(_TB9F);_TB9B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T18AB;}_TBA0=(void*)_TB9B;_TBA1=yyyvsp;_TBA2=_TBA1[0];_TBA3=_TBA2.l;_TBA4=_TBA3.first_line;_TBA5=Cyc_Position_loc_to_seg(_TBA4);_TBA6=Cyc_Absyn_new_exp(_TBA0,_TBA5);yyval=Cyc_Exp_tok(_TBA6);goto _LL0;case 295:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T18AB->tag=35;
# 2424 "parse.y"
_T18AB->f1=0;_TBA8=yyyvsp;_TBA9=& _TBA8[1].v;_TBAA=(union Cyc_YYSTYPE*)_TBA9;_TBAB=Cyc_yyget_YY5(_TBAA);_T18AB->f2=Cyc_List_imp_rev(_TBAB);_TBA7=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T18AB;}_TBAC=(void*)_TBA7;_TBAD=yyyvsp;_TBAE=_TBAD[0];_TBAF=_TBAE.l;_TBB0=_TBAF.first_line;_TBB1=Cyc_Position_loc_to_seg(_TBB0);_TBB2=Cyc_Absyn_new_exp(_TBAC,_TBB1);yyval=Cyc_Exp_tok(_TBB2);goto _LL0;case 296: _TBB3=yyyvsp;_TBB4=_TBB3[2];_TBB5=_TBB4.l;_TBB6=_TBB5.first_line;_TBB7=
# 2426 "parse.y"
Cyc_Position_loc_to_seg(_TBB6);{struct _tuple0*_T18AB=_cycalloc(sizeof(struct _tuple0));_T18AB->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_TBBA=yyyvsp;_TBBB=& _TBBA[2].v;_TBBC=(union Cyc_YYSTYPE*)_TBBB;*_T18AC=Cyc_yyget_String_tok(_TBBC);_TBB9=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_TBB9;_TBB8=(struct _tuple0*)_T18AB;}_TBBD=Cyc_Absyn_uint_type;_TBBE=yyyvsp;_TBBF=_TBBE[2];_TBC0=_TBBF.l;_TBC1=_TBC0.first_line;_TBC2=
Cyc_Position_loc_to_seg(_TBC1);_TBC3=Cyc_Absyn_uint_exp(0U,_TBC2);{
# 2426
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_TBB7,_TBB8,_TBBD,_TBC3,0);_TBC4=vd;
# 2429
_TBC4->tq.real_const=1;{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_T18AB->tag=26;
_T18AB->f1=vd;_TBC6=yyyvsp;_TBC7=& _TBC6[4].v;_TBC8=(union Cyc_YYSTYPE*)_TBC7;_T18AB->f2=Cyc_yyget_Exp_tok(_TBC8);_TBC9=yyyvsp;_TBCA=& _TBC9[6].v;_TBCB=(union Cyc_YYSTYPE*)_TBCA;_T18AB->f3=Cyc_yyget_Exp_tok(_TBCB);_T18AB->f4=0;_TBC5=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T18AB;}_TBCC=(void*)_TBC5;_TBCD=yyyvsp;_TBCE=_TBCD[0];_TBCF=_TBCE.l;_TBD0=_TBCF.first_line;_TBD1=Cyc_Position_loc_to_seg(_TBD0);_TBD2=Cyc_Absyn_new_exp(_TBCC,_TBD1);yyval=Cyc_Exp_tok(_TBD2);goto _LL0;}case 297: _TBD3=yyyvsp;_TBD4=& _TBD3[6].v;_TBD5=(union Cyc_YYSTYPE*)_TBD4;_TBD6=
# 2434 "parse.y"
Cyc_yyget_YY38(_TBD5);_TBD7=yyyvsp;_TBD8=_TBD7[6];_TBD9=_TBD8.l;_TBDA=_TBD9.first_line;_TBDB=Cyc_Position_loc_to_seg(_TBDA);{void*t=Cyc_Parse_type_name_to_type(_TBD6,_TBDB);{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_T18AB->tag=27;_TBDD=yyyvsp;_TBDE=& _TBDD[4].v;_TBDF=(union Cyc_YYSTYPE*)_TBDE;
_T18AB->f1=Cyc_yyget_Exp_tok(_TBDF);_T18AB->f2=t;_T18AB->f3=0;_TBDC=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T18AB;}_TBE0=(void*)_TBDC;_TBE1=yyyvsp;_TBE2=_TBE1[0];_TBE3=_TBE2.l;_TBE4=_TBE3.first_line;_TBE5=Cyc_Position_loc_to_seg(_TBE4);_TBE6=Cyc_Absyn_new_exp(_TBE0,_TBE5);yyval=Cyc_Exp_tok(_TBE6);goto _LL0;}case 298:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T18AC=_cycalloc(sizeof(struct _tuple34));
# 2442 "parse.y"
_T18AC->f0=0;_TBE9=yyyvsp;_TBEA=& _TBE9[0].v;_TBEB=(union Cyc_YYSTYPE*)_TBEA;_T18AC->f1=Cyc_yyget_Exp_tok(_TBEB);_TBE8=(struct _tuple34*)_T18AC;}_T18AB->hd=_TBE8;_T18AB->tl=0;_TBE7=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY5(_TBE7);goto _LL0;case 299:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T18AC=_cycalloc(sizeof(struct _tuple34));_TBEE=yyyvsp;_TBEF=& _TBEE[0].v;_TBF0=(union Cyc_YYSTYPE*)_TBEF;
# 2444 "parse.y"
_T18AC->f0=Cyc_yyget_YY42(_TBF0);_TBF1=yyyvsp;_TBF2=& _TBF1[1].v;_TBF3=(union Cyc_YYSTYPE*)_TBF2;_T18AC->f1=Cyc_yyget_Exp_tok(_TBF3);_TBED=(struct _tuple34*)_T18AC;}_T18AB->hd=_TBED;_T18AB->tl=0;_TBEC=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY5(_TBEC);goto _LL0;case 300:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T18AC=_cycalloc(sizeof(struct _tuple34));
# 2446 "parse.y"
_T18AC->f0=0;_TBF6=yyyvsp;_TBF7=& _TBF6[2].v;_TBF8=(union Cyc_YYSTYPE*)_TBF7;_T18AC->f1=Cyc_yyget_Exp_tok(_TBF8);_TBF5=(struct _tuple34*)_T18AC;}_T18AB->hd=_TBF5;_TBF9=yyyvsp;_TBFA=& _TBF9[0].v;_TBFB=(union Cyc_YYSTYPE*)_TBFA;_T18AB->tl=Cyc_yyget_YY5(_TBFB);_TBF4=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY5(_TBF4);goto _LL0;case 301:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T18AC=_cycalloc(sizeof(struct _tuple34));_TBFE=yyyvsp;_TBFF=& _TBFE[2].v;_TC00=(union Cyc_YYSTYPE*)_TBFF;
# 2448 "parse.y"
_T18AC->f0=Cyc_yyget_YY42(_TC00);_TC01=yyyvsp;_TC02=& _TC01[3].v;_TC03=(union Cyc_YYSTYPE*)_TC02;_T18AC->f1=Cyc_yyget_Exp_tok(_TC03);_TBFD=(struct _tuple34*)_T18AC;}_T18AB->hd=_TBFD;_TC04=yyyvsp;_TC05=& _TC04[0].v;_TC06=(union Cyc_YYSTYPE*)_TC05;_T18AB->tl=Cyc_yyget_YY5(_TC06);_TBFC=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY5(_TBFC);goto _LL0;case 302: _TC07=yyyvsp;_TC08=& _TC07[0].v;_TC09=(union Cyc_YYSTYPE*)_TC08;_TC0A=
# 2452 "parse.y"
Cyc_yyget_YY42(_TC09);_TC0B=Cyc_List_imp_rev(_TC0A);yyval=Cyc_YY42(_TC0B);goto _LL0;case 303:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T18AC->tag=1;{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));_TC0F=yyyvsp;_TC10=& _TC0F[0].v;_TC11=(union Cyc_YYSTYPE*)_TC10;
# 2453 "parse.y"
*_T18AD=Cyc_yyget_String_tok(_TC11);_TC0E=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_TC0E;_TC0D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T18AC;}_T18AB->hd=(void*)_TC0D;_T18AB->tl=0;_TC0C=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY42(_TC0C);goto _LL0;case 304:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TC13=yyyvsp;_TC14=& _TC13[0].v;_TC15=(union Cyc_YYSTYPE*)_TC14;
# 2458 "parse.y"
_T18AB->hd=Cyc_yyget_YY43(_TC15);_T18AB->tl=0;_TC12=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY42(_TC12);goto _LL0;case 305:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TC17=yyyvsp;_TC18=& _TC17[1].v;_TC19=(union Cyc_YYSTYPE*)_TC18;
# 2459 "parse.y"
_T18AB->hd=Cyc_yyget_YY43(_TC19);_TC1A=yyyvsp;_TC1B=& _TC1A[0].v;_TC1C=(union Cyc_YYSTYPE*)_TC1B;_T18AB->tl=Cyc_yyget_YY42(_TC1C);_TC16=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY42(_TC16);goto _LL0;case 306:{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_T18AB->tag=0;_TC1E=yyyvsp;_TC1F=& _TC1E[1].v;_TC20=(union Cyc_YYSTYPE*)_TC1F;
# 2463 "parse.y"
_T18AB->f1=Cyc_yyget_Exp_tok(_TC20);_TC1D=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_T18AB;}_TC21=(void*)_TC1D;yyval=Cyc_YY43(_TC21);goto _LL0;case 307:{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T18AB->tag=1;{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_TC24=yyyvsp;_TC25=& _TC24[1].v;_TC26=(union Cyc_YYSTYPE*)_TC25;
# 2464 "parse.y"
*_T18AC=Cyc_yyget_String_tok(_TC26);_TC23=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_TC23;_TC22=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T18AB;}_TC27=(void*)_TC22;yyval=Cyc_YY43(_TC27);goto _LL0;case 308: _TC28=yyyvsp;_TC29=& _TC28[0].v;_TC2A=(union Cyc_YYSTYPE*)_TC29;{
# 2469 "parse.y"
struct _tuple26 _T18AB=Cyc_yyget_YY36(_TC2A);struct Cyc_List_List*_T18AC;struct Cyc_Parse_Type_specifier _T18AD;struct Cyc_Absyn_Tqual _T18AE;_T18AE=_T18AB.f0;_T18AD=_T18AB.f1;_T18AC=_T18AB.f2;{struct Cyc_Absyn_Tqual tq=_T18AE;struct Cyc_Parse_Type_specifier tss=_T18AD;struct Cyc_List_List*atts=_T18AC;_TC2B=tss;_TC2C=yyyvsp;_TC2D=_TC2C[0];_TC2E=_TC2D.l;_TC2F=_TC2E.first_line;_TC30=
Cyc_Position_loc_to_seg(_TC2F);{void*t=Cyc_Parse_speclist2typ(_TC2B,_TC30);
if(atts==0)goto _TL2E2;_TC31=yyyvsp;_TC32=_TC31[0];_TC33=_TC32.l;_TC34=_TC33.first_line;_TC35=
Cyc_Position_loc_to_seg(_TC34);_TC36=_tag_fat("ignoring attributes in type",sizeof(char),28U);_TC37=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC35,_TC36,_TC37);goto _TL2E3;_TL2E2: _TL2E3:{struct _tuple8*_T18AF=_cycalloc(sizeof(struct _tuple8));
_T18AF->f0=0;_T18AF->f1=tq;_T18AF->f2=t;_TC38=(struct _tuple8*)_T18AF;}yyval=Cyc_YY38(_TC38);goto _LL0;}}}case 309: _TC39=yyyvsp;_TC3A=& _TC39[0].v;_TC3B=(union Cyc_YYSTYPE*)_TC3A;{
# 2476 "parse.y"
struct _tuple26 _T18AB=Cyc_yyget_YY36(_TC3B);struct Cyc_List_List*_T18AC;struct Cyc_Parse_Type_specifier _T18AD;struct Cyc_Absyn_Tqual _T18AE;_T18AE=_T18AB.f0;_T18AD=_T18AB.f1;_T18AC=_T18AB.f2;{struct Cyc_Absyn_Tqual tq=_T18AE;struct Cyc_Parse_Type_specifier tss=_T18AD;struct Cyc_List_List*atts=_T18AC;_TC3C=tss;_TC3D=yyyvsp;_TC3E=_TC3D[0];_TC3F=_TC3E.l;_TC40=_TC3F.first_line;_TC41=
Cyc_Position_loc_to_seg(_TC40);{void*t=Cyc_Parse_speclist2typ(_TC3C,_TC41);_TC42=yyyvsp;_TC43=& _TC42[1].v;_TC44=(union Cyc_YYSTYPE*)_TC43;_TC45=
Cyc_yyget_YY31(_TC44);{struct Cyc_List_List*tms=_TC45.tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);_TC46=t_info;_TC47=_TC46.f2;
if(_TC47==0)goto _TL2E4;_TC48=yyyvsp;_TC49=_TC48[1];_TC4A=_TC49.l;_TC4B=_TC4A.first_line;_TC4C=
Cyc_Position_loc_to_seg(_TC4B);_TC4D=_tag_fat("bad type params, ignoring",sizeof(char),26U);_TC4E=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC4C,_TC4D,_TC4E);goto _TL2E5;_TL2E4: _TL2E5: _TC4F=t_info;_TC50=_TC4F.f3;
if(_TC50==0)goto _TL2E6;_TC51=yyyvsp;_TC52=_TC51[1];_TC53=_TC52.l;_TC54=_TC53.first_line;_TC55=
Cyc_Position_loc_to_seg(_TC54);_TC56=_tag_fat("bad specifiers, ignoring",sizeof(char),25U);_TC57=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC55,_TC56,_TC57);goto _TL2E7;_TL2E6: _TL2E7:{struct _tuple8*_T18AF=_cycalloc(sizeof(struct _tuple8));
_T18AF->f0=0;_TC59=t_info;_T18AF->f1=_TC59.f0;_TC5A=t_info;_T18AF->f2=_TC5A.f1;_TC58=(struct _tuple8*)_T18AF;}yyval=Cyc_YY38(_TC58);goto _LL0;}}}}case 310: _TC5B=yyyvsp;_TC5C=& _TC5B[0].v;_TC5D=(union Cyc_YYSTYPE*)_TC5C;_TC5E=
# 2489 "parse.y"
Cyc_yyget_YY38(_TC5D);_TC5F=yyyvsp;_TC60=_TC5F[0];_TC61=_TC60.l;_TC62=_TC61.first_line;_TC63=Cyc_Position_loc_to_seg(_TC62);_TC64=Cyc_Parse_type_name_to_type(_TC5E,_TC63);yyval=Cyc_YY45(_TC64);goto _LL0;case 311: _TC65=
# 2490 "parse.y"
Cyc_Absyn_join_eff(0);yyval=Cyc_YY45(_TC65);goto _LL0;case 312: _TC66=yyyvsp;_TC67=& _TC66[1].v;_TC68=(union Cyc_YYSTYPE*)_TC67;_TC69=
# 2491 "parse.y"
Cyc_yyget_YY41(_TC68);_TC6A=Cyc_Absyn_join_eff(_TC69);yyval=Cyc_YY45(_TC6A);goto _LL0;case 313: _TC6B=yyyvsp;_TC6C=& _TC6B[2].v;_TC6D=(union Cyc_YYSTYPE*)_TC6C;_TC6E=
# 2492 "parse.y"
Cyc_yyget_YY45(_TC6D);_TC6F=Cyc_Absyn_regionsof_eff(_TC6E);yyval=Cyc_YY45(_TC6F);goto _LL0;case 314:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TC71=yyyvsp;_TC72=& _TC71[0].v;_TC73=(union Cyc_YYSTYPE*)_TC72;
# 2493 "parse.y"
_T18AB->hd=Cyc_yyget_YY45(_TC73);_TC74=yyyvsp;_TC75=& _TC74[2].v;_TC76=(union Cyc_YYSTYPE*)_TC75;_T18AB->tl=Cyc_yyget_YY41(_TC76);_TC70=(struct Cyc_List_List*)_T18AB;}_TC77=Cyc_Absyn_join_eff(_TC70);yyval=Cyc_YY45(_TC77);goto _LL0;case 315: _TC78=yyyvsp;_TC79=& _TC78[0].v;_TC7A=(union Cyc_YYSTYPE*)_TC79;_TC7B=
# 2494 "parse.y"
Cyc_yyget_YY45(_TC7A);yyval=Cyc_YY45(_TC7B);goto _LL0;case 316:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TC7D=yyyvsp;_TC7E=& _TC7D[0].v;_TC7F=(union Cyc_YYSTYPE*)_TC7E;
# 2500 "parse.y"
_T18AB->hd=Cyc_yyget_YY45(_TC7F);_T18AB->tl=0;_TC7C=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY41(_TC7C);goto _LL0;case 317:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_TC81=yyyvsp;_TC82=& _TC81[2].v;_TC83=(union Cyc_YYSTYPE*)_TC82;
# 2501 "parse.y"
_T18AB->hd=Cyc_yyget_YY45(_TC83);_TC84=yyyvsp;_TC85=& _TC84[0].v;_TC86=(union Cyc_YYSTYPE*)_TC85;_T18AB->tl=Cyc_yyget_YY41(_TC86);_TC80=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY41(_TC80);goto _LL0;case 318:{struct Cyc_Parse_Abstractdeclarator _T18AB;_TC88=yyyvsp;_TC89=& _TC88[0].v;_TC8A=(union Cyc_YYSTYPE*)_TC89;
# 2505 "parse.y"
_T18AB.tms=Cyc_yyget_YY27(_TC8A);_TC87=_T18AB;}yyval=Cyc_YY31(_TC87);goto _LL0;case 319: _TC8B=yyyvsp;_TC8C=_TC8B[0];
# 2506 "parse.y"
yyval=_TC8C.v;goto _LL0;case 320:{struct Cyc_Parse_Abstractdeclarator _T18AB;_TC8E=yyyvsp;_TC8F=& _TC8E[0].v;_TC90=(union Cyc_YYSTYPE*)_TC8F;_TC91=
# 2508 "parse.y"
Cyc_yyget_YY27(_TC90);_TC92=yyyvsp;_TC93=& _TC92[1].v;_TC94=(union Cyc_YYSTYPE*)_TC93;_TC95=Cyc_yyget_YY31(_TC94);_TC96=_TC95.tms;_T18AB.tms=Cyc_List_imp_append(_TC91,_TC96);_TC8D=_T18AB;}yyval=Cyc_YY31(_TC8D);goto _LL0;case 321: _TC97=yyyvsp;_TC98=_TC97[1];
# 2513 "parse.y"
yyval=_TC98.v;goto _LL0;case 322:{struct Cyc_Parse_Abstractdeclarator _T18AB;_TC9B=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_TC9B,0U,sizeof(struct Cyc_List_List));_TC9D=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_TC9D,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T18AD->tag=0;_TC9E=yyyvsp;_TC9F=& _TC9E[2].v;_TCA0=(union Cyc_YYSTYPE*)_TC9F;
# 2515 "parse.y"
_T18AD->f1=Cyc_yyget_YY54(_TCA0);_TCA1=yyyvsp;_TCA2=_TCA1[2];_TCA3=_TCA2.l;_TCA4=_TCA3.first_line;_T18AD->f2=Cyc_Position_loc_to_seg(_TCA4);_TC9C=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_TC9C;_T18AC->tl=0;_TC9A=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_TC9A;_TC99=_T18AB;}yyval=Cyc_YY31(_TC99);goto _LL0;case 323:{struct Cyc_Parse_Abstractdeclarator _T18AB;_TCA7=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_TCA7,0U,sizeof(struct Cyc_List_List));_TCA9=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_TCA9,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T18AD->tag=0;_TCAA=yyyvsp;_TCAB=& _TCAA[3].v;_TCAC=(union Cyc_YYSTYPE*)_TCAB;
# 2517 "parse.y"
_T18AD->f1=Cyc_yyget_YY54(_TCAC);_TCAD=yyyvsp;_TCAE=_TCAD[3];_TCAF=_TCAE.l;_TCB0=_TCAF.first_line;_T18AD->f2=Cyc_Position_loc_to_seg(_TCB0);_TCA8=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_TCA8;_TCB1=yyyvsp;_TCB2=& _TCB1[0].v;_TCB3=(union Cyc_YYSTYPE*)_TCB2;_TCB4=Cyc_yyget_YY31(_TCB3);_T18AC->tl=_TCB4.tms;_TCA6=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_TCA6;_TCA5=_T18AB;}yyval=Cyc_YY31(_TCA5);goto _LL0;case 324:{struct Cyc_Parse_Abstractdeclarator _T18AB;_TCB7=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_TCB7,0U,sizeof(struct Cyc_List_List));_TCB9=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_TCB9,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T18AD->tag=1;_TCBA=yyyvsp;_TCBB=& _TCBA[1].v;_TCBC=(union Cyc_YYSTYPE*)_TCBB;
# 2519 "parse.y"
_T18AD->f1=Cyc_yyget_Exp_tok(_TCBC);_TCBD=yyyvsp;_TCBE=& _TCBD[3].v;_TCBF=(union Cyc_YYSTYPE*)_TCBE;_T18AD->f2=Cyc_yyget_YY54(_TCBF);_TCC0=yyyvsp;_TCC1=_TCC0[3];_TCC2=_TCC1.l;_TCC3=_TCC2.first_line;_T18AD->f3=Cyc_Position_loc_to_seg(_TCC3);_TCB8=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_TCB8;_T18AC->tl=0;_TCB6=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_TCB6;_TCB5=_T18AB;}yyval=Cyc_YY31(_TCB5);goto _LL0;case 325:{struct Cyc_Parse_Abstractdeclarator _T18AB;_TCC6=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_TCC6,0U,sizeof(struct Cyc_List_List));_TCC8=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_TCC8,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T18AD->tag=1;_TCC9=yyyvsp;_TCCA=& _TCC9[2].v;_TCCB=(union Cyc_YYSTYPE*)_TCCA;
# 2521 "parse.y"
_T18AD->f1=Cyc_yyget_Exp_tok(_TCCB);_TCCC=yyyvsp;_TCCD=& _TCCC[4].v;_TCCE=(union Cyc_YYSTYPE*)_TCCD;_T18AD->f2=Cyc_yyget_YY54(_TCCE);_TCCF=yyyvsp;_TCD0=_TCCF[4];_TCD1=_TCD0.l;_TCD2=_TCD1.first_line;_T18AD->f3=Cyc_Position_loc_to_seg(_TCD2);_TCC7=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_TCC7;_TCD3=yyyvsp;_TCD4=& _TCD3[0].v;_TCD5=(union Cyc_YYSTYPE*)_TCD4;_TCD6=
Cyc_yyget_YY31(_TCD5);_T18AC->tl=_TCD6.tms;_TCC5=(struct Cyc_List_List*)_T18AC;}
# 2521
_T18AB.tms=_TCC5;_TCC4=_T18AB;}yyval=Cyc_YY31(_TCC4);goto _LL0;case 326: _TCD7=yyyvsp;_TCD8=& _TCD7[1].v;_TCD9=(union Cyc_YYSTYPE*)_TCD8;{
# 2525 "parse.y"
struct _tuple27*_T18AB=Cyc_yyget_YY40(_TCD9);struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;void*_T18AE;struct Cyc_Absyn_VarargInfo*_T18AF;int _T18B0;struct Cyc_List_List*_T18B1;{struct _tuple27 _T18B2=*_T18AB;_T18B1=_T18B2.f0;_T18B0=_T18B2.f1;_T18AF=_T18B2.f2;_T18AE=_T18B2.f3;_T18AD=_T18B2.f4;_T18AC=_T18B2.f5;}{struct Cyc_List_List*lis=_T18B1;int b=_T18B0;struct Cyc_Absyn_VarargInfo*c=_T18AF;void*eff=_T18AE;struct Cyc_List_List*ec=_T18AD;struct Cyc_List_List*qb=_T18AC;_TCDA=yyyvsp;_TCDB=& _TCDA[3].v;_TCDC=(union Cyc_YYSTYPE*)_TCDB;{
struct _tuple21 _T18B2=Cyc_yyget_YY62(_TCDC);struct Cyc_Absyn_Exp*_T18B3;struct Cyc_Absyn_Exp*_T18B4;struct Cyc_Absyn_Exp*_T18B5;struct Cyc_Absyn_Exp*_T18B6;_T18B6=_T18B2.f0;_T18B5=_T18B2.f1;_T18B4=_T18B2.f2;_T18B3=_T18B2.f3;{struct Cyc_Absyn_Exp*chk=_T18B6;struct Cyc_Absyn_Exp*req=_T18B5;struct Cyc_Absyn_Exp*ens=_T18B4;struct Cyc_Absyn_Exp*thrws=_T18B3;{struct Cyc_Parse_Abstractdeclarator _T18B7;_TCDF=yyr;{struct Cyc_List_List*_T18B8=_region_malloc(_TCDF,0U,sizeof(struct Cyc_List_List));_TCE1=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T18B9=_region_malloc(_TCE1,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T18B9->tag=3;_TCE3=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T18BA=_region_malloc(_TCE3,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T18BA->tag=1;
_T18BA->f1=lis;_T18BA->f2=b;_T18BA->f3=c;_T18BA->f4=eff;_T18BA->f5=ec;_T18BA->f6=qb;_T18BA->f7=chk;_T18BA->f8=req;_T18BA->f9=ens;_T18BA->f10=thrws;_TCE2=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T18BA;}_T18B9->f1=(void*)_TCE2;_TCE0=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T18B9;}_T18B8->hd=(void*)_TCE0;_T18B8->tl=0;_TCDE=(struct Cyc_List_List*)_T18B8;}_T18B7.tms=_TCDE;_TCDD=_T18B7;}yyval=Cyc_YY31(_TCDD);goto _LL0;}}}}case 327: _TCE4=yyyvsp;_TCE5=& _TCE4[2].v;_TCE6=(union Cyc_YYSTYPE*)_TCE5;{
# 2530 "parse.y"
struct _tuple27*_T18AB=Cyc_yyget_YY40(_TCE6);struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;void*_T18AE;struct Cyc_Absyn_VarargInfo*_T18AF;int _T18B0;struct Cyc_List_List*_T18B1;{struct _tuple27 _T18B2=*_T18AB;_T18B1=_T18B2.f0;_T18B0=_T18B2.f1;_T18AF=_T18B2.f2;_T18AE=_T18B2.f3;_T18AD=_T18B2.f4;_T18AC=_T18B2.f5;}{struct Cyc_List_List*lis=_T18B1;int b=_T18B0;struct Cyc_Absyn_VarargInfo*c=_T18AF;void*eff=_T18AE;struct Cyc_List_List*ec=_T18AD;struct Cyc_List_List*qb=_T18AC;_TCE7=yyyvsp;_TCE8=& _TCE7[4].v;_TCE9=(union Cyc_YYSTYPE*)_TCE8;{
struct _tuple21 _T18B2=Cyc_yyget_YY62(_TCE9);struct Cyc_Absyn_Exp*_T18B3;struct Cyc_Absyn_Exp*_T18B4;struct Cyc_Absyn_Exp*_T18B5;struct Cyc_Absyn_Exp*_T18B6;_T18B6=_T18B2.f0;_T18B5=_T18B2.f1;_T18B4=_T18B2.f2;_T18B3=_T18B2.f3;{struct Cyc_Absyn_Exp*chk=_T18B6;struct Cyc_Absyn_Exp*req=_T18B5;struct Cyc_Absyn_Exp*ens=_T18B4;struct Cyc_Absyn_Exp*thrws=_T18B3;{struct Cyc_Parse_Abstractdeclarator _T18B7;_TCEC=yyr;{struct Cyc_List_List*_T18B8=_region_malloc(_TCEC,0U,sizeof(struct Cyc_List_List));_TCEE=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T18B9=_region_malloc(_TCEE,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T18B9->tag=3;_TCF0=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T18BA=_region_malloc(_TCF0,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T18BA->tag=1;
_T18BA->f1=lis;
_T18BA->f2=b;_T18BA->f3=c;_T18BA->f4=eff;_T18BA->f5=ec;_T18BA->f6=qb;_T18BA->f7=chk;_T18BA->f8=req;_T18BA->f9=ens;_T18BA->f10=thrws;_TCEF=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T18BA;}
# 2532
_T18B9->f1=(void*)_TCEF;_TCED=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T18B9;}_T18B8->hd=(void*)_TCED;_TCF1=yyyvsp;_TCF2=& _TCF1[0].v;_TCF3=(union Cyc_YYSTYPE*)_TCF2;_TCF4=
Cyc_yyget_YY31(_TCF3);_T18B8->tl=_TCF4.tms;_TCEB=(struct Cyc_List_List*)_T18B8;}
# 2532
_T18B7.tms=_TCEB;_TCEA=_T18B7;}yyval=Cyc_YY31(_TCEA);goto _LL0;}}}}case 328: _TCF6=Cyc_List_map_c;{
# 2537
struct Cyc_List_List*(*_T18AB)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_TCF6;_TCF5=_T18AB;}_TCF7=yyyvsp;_TCF8=_TCF7[1];_TCF9=_TCF8.l;_TCFA=_TCF9.first_line;_TCFB=Cyc_Position_loc_to_seg(_TCFA);_TCFC=yyyvsp;_TCFD=& _TCFC[2].v;_TCFE=(union Cyc_YYSTYPE*)_TCFD;_TCFF=Cyc_yyget_YY41(_TCFE);_TD00=Cyc_List_imp_rev(_TCFF);{struct Cyc_List_List*ts=_TCF5(Cyc_Parse_typ2tvar,_TCFB,_TD00);{struct Cyc_Parse_Abstractdeclarator _T18AB;_TD03=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_TD03,0U,sizeof(struct Cyc_List_List));_TD05=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_TD05,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T18AD->tag=4;
_T18AD->f1=ts;_TD06=yyyvsp;_TD07=_TD06[1];_TD08=_TD07.l;_TD09=_TD08.first_line;_T18AD->f2=Cyc_Position_loc_to_seg(_TD09);_T18AD->f3=0;_TD04=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_TD04;_TD0A=yyyvsp;_TD0B=& _TD0A[0].v;_TD0C=(union Cyc_YYSTYPE*)_TD0B;_TD0D=
Cyc_yyget_YY31(_TD0C);_T18AC->tl=_TD0D.tms;_TD02=(struct Cyc_List_List*)_T18AC;}
# 2538
_T18AB.tms=_TD02;_TD01=_T18AB;}yyval=Cyc_YY31(_TD01);goto _LL0;}case 329:{struct Cyc_Parse_Abstractdeclarator _T18AB;_TD10=yyr;{struct Cyc_List_List*_T18AC=_region_malloc(_TD10,0U,sizeof(struct Cyc_List_List));_TD12=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T18AD=_region_malloc(_TD12,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T18AD->tag=5;_TD13=yyyvsp;_TD14=_TD13[1];_TD15=_TD14.l;_TD16=_TD15.first_line;
# 2542 "parse.y"
_T18AD->f1=Cyc_Position_loc_to_seg(_TD16);_TD17=yyyvsp;_TD18=& _TD17[1].v;_TD19=(union Cyc_YYSTYPE*)_TD18;_T18AD->f2=Cyc_yyget_YY46(_TD19);_TD11=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T18AD;}_T18AC->hd=(void*)_TD11;_TD1A=yyyvsp;_TD1B=& _TD1A[0].v;_TD1C=(union Cyc_YYSTYPE*)_TD1B;_TD1D=Cyc_yyget_YY31(_TD1C);_T18AC->tl=_TD1D.tms;_TD0F=(struct Cyc_List_List*)_T18AC;}_T18AB.tms=_TD0F;_TD0E=_T18AB;}yyval=Cyc_YY31(_TD0E);goto _LL0;case 330:{struct _tuple21 _T18AB;_TD1F=yyyvsp;_TD20=& _TD1F[2].v;_TD21=(union Cyc_YYSTYPE*)_TD20;
# 2548 "parse.y"
_T18AB.f0=Cyc_yyget_Exp_tok(_TD21);_T18AB.f1=0;_T18AB.f2=0;_T18AB.f3=0;_TD1E=_T18AB;}yyval=Cyc_YY62(_TD1E);goto _LL0;case 331:{struct _tuple21 _T18AB;
# 2550 "parse.y"
_T18AB.f0=0;_TD23=yyyvsp;_TD24=& _TD23[2].v;_TD25=(union Cyc_YYSTYPE*)_TD24;_T18AB.f1=Cyc_yyget_Exp_tok(_TD25);_T18AB.f2=0;_T18AB.f3=0;_TD22=_T18AB;}yyval=Cyc_YY62(_TD22);goto _LL0;case 332:{struct _tuple21 _T18AB;
# 2552 "parse.y"
_T18AB.f0=0;_T18AB.f1=0;_TD27=yyyvsp;_TD28=& _TD27[2].v;_TD29=(union Cyc_YYSTYPE*)_TD28;_T18AB.f2=Cyc_yyget_Exp_tok(_TD29);_T18AB.f3=0;_TD26=_T18AB;}yyval=Cyc_YY62(_TD26);goto _LL0;case 333:{struct _tuple21 _T18AB;
# 2554 "parse.y"
_T18AB.f0=0;_T18AB.f1=0;_T18AB.f2=0;_TD2B=yyyvsp;_TD2C=& _TD2B[2].v;_TD2D=(union Cyc_YYSTYPE*)_TD2C;_T18AB.f3=Cyc_yyget_Exp_tok(_TD2D);_TD2A=_T18AB;}yyval=Cyc_YY62(_TD2A);goto _LL0;case 334:{struct _tuple21 _T18AB;_TD2F=yyyvsp;_TD30=& _TD2F[2].v;_TD31=(union Cyc_YYSTYPE*)_TD30;
# 2556 "parse.y"
_T18AB.f0=Cyc_yyget_Exp_tok(_TD31);_T18AB.f1=0;_T18AB.f2=0;_T18AB.f3=0;_TD2E=_T18AB;}_TD32=yyyvsp;_TD33=& _TD32[4].v;_TD34=(union Cyc_YYSTYPE*)_TD33;_TD35=Cyc_yyget_YY62(_TD34);_TD36=Cyc_Parse_join_assns(_TD2E,_TD35);yyval=Cyc_YY62(_TD36);goto _LL0;case 335:{struct _tuple21 _T18AB;
# 2558 "parse.y"
_T18AB.f0=0;_TD38=yyyvsp;_TD39=& _TD38[2].v;_TD3A=(union Cyc_YYSTYPE*)_TD39;_T18AB.f1=Cyc_yyget_Exp_tok(_TD3A);_T18AB.f2=0;_T18AB.f3=0;_TD37=_T18AB;}_TD3B=yyyvsp;_TD3C=& _TD3B[4].v;_TD3D=(union Cyc_YYSTYPE*)_TD3C;_TD3E=Cyc_yyget_YY62(_TD3D);_TD3F=Cyc_Parse_join_assns(_TD37,_TD3E);yyval=Cyc_YY62(_TD3F);goto _LL0;case 336:{struct _tuple21 _T18AB;
# 2560 "parse.y"
_T18AB.f0=0;_T18AB.f1=0;_TD41=yyyvsp;_TD42=& _TD41[2].v;_TD43=(union Cyc_YYSTYPE*)_TD42;_T18AB.f2=Cyc_yyget_Exp_tok(_TD43);_T18AB.f3=0;_TD40=_T18AB;}_TD44=yyyvsp;_TD45=& _TD44[4].v;_TD46=(union Cyc_YYSTYPE*)_TD45;_TD47=Cyc_yyget_YY62(_TD46);_TD48=Cyc_Parse_join_assns(_TD40,_TD47);yyval=Cyc_YY62(_TD48);goto _LL0;case 337:{struct _tuple21 _T18AB;
# 2562 "parse.y"
_T18AB.f0=0;_T18AB.f1=0;_T18AB.f2=0;_TD4A=yyyvsp;_TD4B=& _TD4A[2].v;_TD4C=(union Cyc_YYSTYPE*)_TD4B;_T18AB.f3=Cyc_yyget_Exp_tok(_TD4C);_TD49=_T18AB;}_TD4D=yyyvsp;_TD4E=& _TD4D[4].v;_TD4F=(union Cyc_YYSTYPE*)_TD4E;_TD50=Cyc_yyget_YY62(_TD4F);_TD51=Cyc_Parse_join_assns(_TD49,_TD50);yyval=Cyc_YY62(_TD51);goto _LL0;case 338:{struct _tuple21 _T18AB;
# 2566 "parse.y"
_T18AB.f0=0;_T18AB.f1=0;_T18AB.f2=0;_T18AB.f3=0;_TD52=_T18AB;}yyval=Cyc_YY62(_TD52);goto _LL0;case 339: _TD53=yyyvsp;_TD54=_TD53[0];
# 2567 "parse.y"
yyval=_TD54.v;goto _LL0;case 340: _TD55=yyyvsp;_TD56=_TD55[0];
# 2572 "parse.y"
yyval=_TD56.v;goto _LL0;case 341: _TD57=yyyvsp;_TD58=_TD57[0];
# 2573 "parse.y"
yyval=_TD58.v;goto _LL0;case 342: _TD59=yyyvsp;_TD5A=_TD59[0];
# 2574 "parse.y"
yyval=_TD5A.v;goto _LL0;case 343: _TD5B=yyyvsp;_TD5C=_TD5B[0];
# 2575 "parse.y"
yyval=_TD5C.v;goto _LL0;case 344: _TD5D=yyyvsp;_TD5E=_TD5D[0];
# 2576 "parse.y"
yyval=_TD5E.v;goto _LL0;case 345: _TD5F=yyyvsp;_TD60=_TD5F[0];
# 2577 "parse.y"
yyval=_TD60.v;goto _LL0;case 346:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T18AB->tag=13;{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_TD63=yyyvsp;_TD64=& _TD63[0].v;_TD65=(union Cyc_YYSTYPE*)_TD64;
# 2583 "parse.y"
*_T18AC=Cyc_yyget_String_tok(_TD65);_TD62=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_TD62;_TD66=yyyvsp;_TD67=& _TD66[2].v;_TD68=(union Cyc_YYSTYPE*)_TD67;_T18AB->f2=Cyc_yyget_Stmt_tok(_TD68);_TD61=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T18AB;}_TD69=(void*)_TD61;_TD6A=yyyvsp;_TD6B=_TD6A[0];_TD6C=_TD6B.l;_TD6D=_TD6C.first_line;_TD6E=Cyc_Position_loc_to_seg(_TD6D);_TD6F=Cyc_Absyn_new_stmt(_TD69,_TD6E);yyval=Cyc_Stmt_tok(_TD6F);goto _LL0;case 347: _TD70=yyyvsp;_TD71=_TD70[0];_TD72=_TD71.l;_TD73=_TD72.first_line;_TD74=
# 2587 "parse.y"
Cyc_Position_loc_to_seg(_TD73);_TD75=Cyc_Absyn_skip_stmt(_TD74);yyval=Cyc_Stmt_tok(_TD75);goto _LL0;case 348: _TD76=yyyvsp;_TD77=& _TD76[0].v;_TD78=(union Cyc_YYSTYPE*)_TD77;_TD79=
# 2588 "parse.y"
Cyc_yyget_Exp_tok(_TD78);_TD7A=yyyvsp;_TD7B=_TD7A[0];_TD7C=_TD7B.l;_TD7D=_TD7C.first_line;_TD7E=Cyc_Position_loc_to_seg(_TD7D);_TD7F=Cyc_Absyn_exp_stmt(_TD79,_TD7E);yyval=Cyc_Stmt_tok(_TD7F);goto _LL0;case 349:
# 2593 "parse.y"
 Cyc_Parse_inside_function_definition=1;goto _LL0;case 350:
# 2597 "parse.y"
 Cyc_Parse_inside_function_definition=0;goto _LL0;case 351: _TD80=yyyvsp;_TD81=_TD80[0];_TD82=_TD81.l;_TD83=_TD82.first_line;_TD84=
# 2601 "parse.y"
Cyc_Position_loc_to_seg(_TD83);_TD85=Cyc_Absyn_skip_stmt(_TD84);yyval=Cyc_Stmt_tok(_TD85);goto _LL0;case 352: _TD86=yyyvsp;_TD87=_TD86[1];
# 2602 "parse.y"
yyval=_TD87.v;goto _LL0;case 353: _TD88=yyyvsp;_TD89=_TD88[0];_TD8A=_TD89.l;_TD8B=_TD8A.first_line;_TD8C=
# 2607 "parse.y"
Cyc_Position_loc_to_seg(_TD8B);_TD8D=Cyc_Absyn_skip_stmt(_TD8C);yyval=Cyc_Stmt_tok(_TD8D);goto _LL0;case 354: _TD8E=yyyvsp;_TD8F=_TD8E[1];
# 2608 "parse.y"
yyval=_TD8F.v;goto _LL0;case 355: _TD90=yyyvsp;_TD91=& _TD90[0].v;_TD92=(union Cyc_YYSTYPE*)_TD91;_TD93=
# 2613 "parse.y"
Cyc_yyget_YY16(_TD92);_TD94=yyyvsp;_TD95=_TD94[0];_TD96=_TD95.l;_TD97=_TD96.first_line;_TD98=Cyc_Position_loc_to_seg(_TD97);_TD99=Cyc_Absyn_skip_stmt(_TD98);_TD9A=Cyc_Parse_flatten_declarations(_TD93,_TD99);yyval=Cyc_Stmt_tok(_TD9A);goto _LL0;case 356: _TD9B=yyyvsp;_TD9C=& _TD9B[0].v;_TD9D=(union Cyc_YYSTYPE*)_TD9C;_TD9E=
# 2614 "parse.y"
Cyc_yyget_YY16(_TD9D);_TD9F=yyyvsp;_TDA0=& _TD9F[1].v;_TDA1=(union Cyc_YYSTYPE*)_TDA0;_TDA2=Cyc_yyget_Stmt_tok(_TDA1);_TDA3=Cyc_Parse_flatten_declarations(_TD9E,_TDA2);yyval=Cyc_Stmt_tok(_TDA3);goto _LL0;case 357:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T18AB->tag=13;{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_TDA6=yyyvsp;_TDA7=& _TDA6[0].v;_TDA8=(union Cyc_YYSTYPE*)_TDA7;
# 2616 "parse.y"
*_T18AC=Cyc_yyget_String_tok(_TDA8);_TDA5=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_TDA5;_TDA9=yyyvsp;_TDAA=& _TDA9[2].v;_TDAB=(union Cyc_YYSTYPE*)_TDAA;_TDAC=Cyc_yyget_YY16(_TDAB);_TDAD=Cyc_Absyn_skip_stmt(0U);_T18AB->f2=Cyc_Parse_flatten_declarations(_TDAC,_TDAD);_TDA4=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T18AB;}_TDAE=(void*)_TDA4;_TDAF=yyyvsp;_TDB0=_TDAF[0];_TDB1=_TDB0.l;_TDB2=_TDB1.first_line;_TDB3=Cyc_Position_loc_to_seg(_TDB2);_TDB4=Cyc_Absyn_new_stmt(_TDAE,_TDB3);yyval=Cyc_Stmt_tok(_TDB4);goto _LL0;case 358:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T18AB->tag=13;{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_TDB7=yyyvsp;_TDB8=& _TDB7[0].v;_TDB9=(union Cyc_YYSTYPE*)_TDB8;
# 2618 "parse.y"
*_T18AC=Cyc_yyget_String_tok(_TDB9);_TDB6=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_TDB6;_TDBA=yyyvsp;_TDBB=& _TDBA[2].v;_TDBC=(union Cyc_YYSTYPE*)_TDBB;_TDBD=Cyc_yyget_YY16(_TDBC);_TDBE=yyyvsp;_TDBF=& _TDBE[3].v;_TDC0=(union Cyc_YYSTYPE*)_TDBF;_TDC1=Cyc_yyget_Stmt_tok(_TDC0);_T18AB->f2=Cyc_Parse_flatten_declarations(_TDBD,_TDC1);_TDB5=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T18AB;}_TDC2=(void*)_TDB5;_TDC3=yyyvsp;_TDC4=_TDC3[0];_TDC5=_TDC4.l;_TDC6=_TDC5.first_line;_TDC7=Cyc_Position_loc_to_seg(_TDC6);_TDC8=Cyc_Absyn_new_stmt(_TDC2,_TDC7);yyval=Cyc_Stmt_tok(_TDC8);goto _LL0;case 359: _TDC9=yyyvsp;_TDCA=_TDC9[0];
# 2619 "parse.y"
yyval=_TDCA.v;goto _LL0;case 360: _TDCB=yyyvsp;_TDCC=& _TDCB[0].v;_TDCD=(union Cyc_YYSTYPE*)_TDCC;_TDCE=
# 2620 "parse.y"
Cyc_yyget_Stmt_tok(_TDCD);_TDCF=yyyvsp;_TDD0=& _TDCF[1].v;_TDD1=(union Cyc_YYSTYPE*)_TDD0;_TDD2=Cyc_yyget_Stmt_tok(_TDD1);_TDD3=yyyvsp;_TDD4=_TDD3[0];_TDD5=_TDD4.l;_TDD6=_TDD5.first_line;_TDD7=Cyc_Position_loc_to_seg(_TDD6);_TDD8=Cyc_Absyn_seq_stmt(_TDCE,_TDD2,_TDD7);yyval=Cyc_Stmt_tok(_TDD8);goto _LL0;case 361:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T18AB->tag=1;_TDDA=yyyvsp;_TDDB=& _TDDA[0].v;_TDDC=(union Cyc_YYSTYPE*)_TDDB;
# 2621 "parse.y"
_T18AB->f1=Cyc_yyget_YY15(_TDDC);_TDD9=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T18AB;}_TDDD=(void*)_TDD9;_TDDE=yyyvsp;_TDDF=_TDDE[0];_TDE0=_TDDF.l;_TDE1=_TDE0.first_line;_TDE2=Cyc_Position_loc_to_seg(_TDE1);_TDE3=Cyc_Absyn_new_decl(_TDDD,_TDE2);_TDE4=
Cyc_Absyn_skip_stmt(0U);_TDE5=
# 2621
Cyc_Parse_flatten_decl(_TDE3,_TDE4);yyval=Cyc_Stmt_tok(_TDE5);goto _LL0;case 362:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T18AB->tag=1;_TDE7=yyyvsp;_TDE8=& _TDE7[0].v;_TDE9=(union Cyc_YYSTYPE*)_TDE8;
# 2624 "parse.y"
_T18AB->f1=Cyc_yyget_YY15(_TDE9);_TDE6=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T18AB;}_TDEA=(void*)_TDE6;_TDEB=yyyvsp;_TDEC=_TDEB[0];_TDED=_TDEC.l;_TDEE=_TDED.first_line;_TDEF=Cyc_Position_loc_to_seg(_TDEE);_TDF0=Cyc_Absyn_new_decl(_TDEA,_TDEF);_TDF1=yyyvsp;_TDF2=& _TDF1[1].v;_TDF3=(union Cyc_YYSTYPE*)_TDF2;_TDF4=Cyc_yyget_Stmt_tok(_TDF3);_TDF5=Cyc_Parse_flatten_decl(_TDF0,_TDF4);yyval=Cyc_Stmt_tok(_TDF5);goto _LL0;case 363: _TDF6=yyyvsp;_TDF7=& _TDF6[2].v;_TDF8=(union Cyc_YYSTYPE*)_TDF7;_TDF9=
# 2629 "parse.y"
Cyc_yyget_Exp_tok(_TDF8);_TDFA=yyyvsp;_TDFB=& _TDFA[4].v;_TDFC=(union Cyc_YYSTYPE*)_TDFB;_TDFD=Cyc_yyget_Stmt_tok(_TDFC);_TDFE=Cyc_Absyn_skip_stmt(0U);_TDFF=yyyvsp;_TE00=_TDFF[0];_TE01=_TE00.l;_TE02=_TE01.first_line;_TE03=Cyc_Position_loc_to_seg(_TE02);_TE04=Cyc_Absyn_ifthenelse_stmt(_TDF9,_TDFD,_TDFE,_TE03);yyval=Cyc_Stmt_tok(_TE04);goto _LL0;case 364: _TE05=yyyvsp;_TE06=& _TE05[2].v;_TE07=(union Cyc_YYSTYPE*)_TE06;_TE08=
# 2631 "parse.y"
Cyc_yyget_Exp_tok(_TE07);_TE09=yyyvsp;_TE0A=& _TE09[4].v;_TE0B=(union Cyc_YYSTYPE*)_TE0A;_TE0C=Cyc_yyget_Stmt_tok(_TE0B);_TE0D=yyyvsp;_TE0E=& _TE0D[6].v;_TE0F=(union Cyc_YYSTYPE*)_TE0E;_TE10=Cyc_yyget_Stmt_tok(_TE0F);_TE11=yyyvsp;_TE12=_TE11[0];_TE13=_TE12.l;_TE14=_TE13.first_line;_TE15=Cyc_Position_loc_to_seg(_TE14);_TE16=Cyc_Absyn_ifthenelse_stmt(_TE08,_TE0C,_TE10,_TE15);yyval=Cyc_Stmt_tok(_TE16);goto _LL0;case 365: _TE17=yyyvsp;_TE18=& _TE17[2].v;_TE19=(union Cyc_YYSTYPE*)_TE18;_TE1A=
# 2637 "parse.y"
Cyc_yyget_Exp_tok(_TE19);_TE1B=yyyvsp;_TE1C=& _TE1B[5].v;_TE1D=(union Cyc_YYSTYPE*)_TE1C;_TE1E=Cyc_yyget_YY8(_TE1D);_TE1F=yyyvsp;_TE20=_TE1F[0];_TE21=_TE20.l;_TE22=_TE21.first_line;_TE23=Cyc_Position_loc_to_seg(_TE22);_TE24=Cyc_Absyn_switch_stmt(_TE1A,_TE1E,_TE23);yyval=Cyc_Stmt_tok(_TE24);goto _LL0;case 366: _TE25=yyyvsp;_TE26=& _TE25[1].v;_TE27=(union Cyc_YYSTYPE*)_TE26;_TE28=
# 2640
Cyc_yyget_QualId_tok(_TE27);_TE29=yyyvsp;_TE2A=_TE29[1];_TE2B=_TE2A.l;_TE2C=_TE2B.first_line;_TE2D=Cyc_Position_loc_to_seg(_TE2C);_TE2E=Cyc_Absyn_unknownid_exp(_TE28,_TE2D);_TE2F=yyyvsp;_TE30=& _TE2F[3].v;_TE31=(union Cyc_YYSTYPE*)_TE30;_TE32=Cyc_yyget_YY8(_TE31);_TE33=yyyvsp;_TE34=_TE33[0];_TE35=_TE34.l;_TE36=_TE35.first_line;_TE37=Cyc_Position_loc_to_seg(_TE36);_TE38=Cyc_Absyn_switch_stmt(_TE2E,_TE32,_TE37);yyval=Cyc_Stmt_tok(_TE38);goto _LL0;case 367: _TE39=yyyvsp;_TE3A=& _TE39[3].v;_TE3B=(union Cyc_YYSTYPE*)_TE3A;_TE3C=
# 2643
Cyc_yyget_YY4(_TE3B);_TE3D=yyyvsp;_TE3E=_TE3D[1];_TE3F=_TE3E.l;_TE40=_TE3F.first_line;_TE41=Cyc_Position_loc_to_seg(_TE40);_TE42=Cyc_Absyn_tuple_exp(_TE3C,_TE41);_TE43=yyyvsp;_TE44=& _TE43[6].v;_TE45=(union Cyc_YYSTYPE*)_TE44;_TE46=Cyc_yyget_YY8(_TE45);_TE47=yyyvsp;_TE48=_TE47[0];_TE49=_TE48.l;_TE4A=_TE49.first_line;_TE4B=Cyc_Position_loc_to_seg(_TE4A);_TE4C=Cyc_Absyn_switch_stmt(_TE42,_TE46,_TE4B);yyval=Cyc_Stmt_tok(_TE4C);goto _LL0;case 368: _TE4D=yyyvsp;_TE4E=& _TE4D[1].v;_TE4F=(union Cyc_YYSTYPE*)_TE4E;_TE50=
# 2647 "parse.y"
Cyc_yyget_Stmt_tok(_TE4F);_TE51=yyyvsp;_TE52=& _TE51[4].v;_TE53=(union Cyc_YYSTYPE*)_TE52;_TE54=Cyc_yyget_YY8(_TE53);_TE55=yyyvsp;_TE56=_TE55[0];_TE57=_TE56.l;_TE58=_TE57.first_line;_TE59=Cyc_Position_loc_to_seg(_TE58);_TE5A=Cyc_Absyn_trycatch_stmt(_TE50,_TE54,_TE59);yyval=Cyc_Stmt_tok(_TE5A);goto _LL0;case 369:
# 2661 "parse.y"
 yyval=Cyc_YY8(0);goto _LL0;case 370:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE5D=& Cyc_Absyn_Wild_p_val;_TE5E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_TE5D;_TE5F=(void*)_TE5E;_TE60=yyyvsp;_TE61=_TE60[0];_TE62=_TE61.l;_TE63=_TE62.first_line;_TE64=
# 2664 "parse.y"
Cyc_Position_loc_to_seg(_TE63);_T18AC->pattern=Cyc_Absyn_new_pat(_TE5F,_TE64);_T18AC->pat_vars=0;
_T18AC->where_clause=0;_TE65=yyyvsp;_TE66=& _TE65[2].v;_TE67=(union Cyc_YYSTYPE*)_TE66;_T18AC->body=Cyc_yyget_Stmt_tok(_TE67);_TE68=yyyvsp;_TE69=_TE68[0];_TE6A=_TE69.l;_TE6B=_TE6A.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_TE6B);_TE5C=(struct Cyc_Absyn_Switch_clause*)_T18AC;}
# 2664
_T18AB->hd=_TE5C;_TE6C=yyyvsp;_TE6D=& _TE6C[3].v;_TE6E=(union Cyc_YYSTYPE*)_TE6D;
_T18AB->tl=Cyc_yyget_YY8(_TE6E);_TE5B=(struct Cyc_List_List*)_T18AB;}
# 2664
yyval=Cyc_YY8(_TE5B);goto _LL0;case 371:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE71=yyyvsp;_TE72=& _TE71[1].v;_TE73=(union Cyc_YYSTYPE*)_TE72;
# 2667 "parse.y"
_T18AC->pattern=Cyc_yyget_YY9(_TE73);_T18AC->pat_vars=0;_T18AC->where_clause=0;_TE74=yyyvsp;_TE75=_TE74[2];_TE76=_TE75.l;_TE77=_TE76.first_line;_TE78=
Cyc_Position_loc_to_seg(_TE77);_T18AC->body=Cyc_Absyn_fallthru_stmt(0,_TE78);_TE79=yyyvsp;_TE7A=_TE79[0];_TE7B=_TE7A.l;_TE7C=_TE7B.first_line;
_T18AC->loc=Cyc_Position_loc_to_seg(_TE7C);_TE70=(struct Cyc_Absyn_Switch_clause*)_T18AC;}
# 2667
_T18AB->hd=_TE70;_TE7D=yyyvsp;_TE7E=& _TE7D[3].v;_TE7F=(union Cyc_YYSTYPE*)_TE7E;
# 2669
_T18AB->tl=Cyc_yyget_YY8(_TE7F);_TE6F=(struct Cyc_List_List*)_T18AB;}
# 2667
yyval=Cyc_YY8(_TE6F);goto _LL0;case 372:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE82=yyyvsp;_TE83=& _TE82[1].v;_TE84=(union Cyc_YYSTYPE*)_TE83;
# 2671 "parse.y"
_T18AC->pattern=Cyc_yyget_YY9(_TE84);_T18AC->pat_vars=0;_T18AC->where_clause=0;_TE85=yyyvsp;_TE86=& _TE85[3].v;_TE87=(union Cyc_YYSTYPE*)_TE86;_T18AC->body=Cyc_yyget_Stmt_tok(_TE87);_TE88=yyyvsp;_TE89=_TE88[0];_TE8A=_TE89.l;_TE8B=_TE8A.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_TE8B);_TE81=(struct Cyc_Absyn_Switch_clause*)_T18AC;}_T18AB->hd=_TE81;_TE8C=yyyvsp;_TE8D=& _TE8C[4].v;_TE8E=(union Cyc_YYSTYPE*)_TE8D;_T18AB->tl=Cyc_yyget_YY8(_TE8E);_TE80=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY8(_TE80);goto _LL0;case 373:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE91=yyyvsp;_TE92=& _TE91[1].v;_TE93=(union Cyc_YYSTYPE*)_TE92;
# 2673 "parse.y"
_T18AC->pattern=Cyc_yyget_YY9(_TE93);_T18AC->pat_vars=0;_TE94=yyyvsp;_TE95=& _TE94[3].v;_TE96=(union Cyc_YYSTYPE*)_TE95;_T18AC->where_clause=Cyc_yyget_Exp_tok(_TE96);_TE97=yyyvsp;_TE98=_TE97[4];_TE99=_TE98.l;_TE9A=_TE99.first_line;_TE9B=
Cyc_Position_loc_to_seg(_TE9A);_T18AC->body=Cyc_Absyn_fallthru_stmt(0,_TE9B);_TE9C=yyyvsp;_TE9D=_TE9C[0];_TE9E=_TE9D.l;_TE9F=_TE9E.first_line;
_T18AC->loc=Cyc_Position_loc_to_seg(_TE9F);_TE90=(struct Cyc_Absyn_Switch_clause*)_T18AC;}
# 2673
_T18AB->hd=_TE90;_TEA0=yyyvsp;_TEA1=& _TEA0[5].v;_TEA2=(union Cyc_YYSTYPE*)_TEA1;
# 2675
_T18AB->tl=Cyc_yyget_YY8(_TEA2);_TE8F=(struct Cyc_List_List*)_T18AB;}
# 2673
yyval=Cyc_YY8(_TE8F);goto _LL0;case 374:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TEA5=yyyvsp;_TEA6=& _TEA5[1].v;_TEA7=(union Cyc_YYSTYPE*)_TEA6;
# 2677 "parse.y"
_T18AC->pattern=Cyc_yyget_YY9(_TEA7);_T18AC->pat_vars=0;_TEA8=yyyvsp;_TEA9=& _TEA8[3].v;_TEAA=(union Cyc_YYSTYPE*)_TEA9;_T18AC->where_clause=Cyc_yyget_Exp_tok(_TEAA);_TEAB=yyyvsp;_TEAC=& _TEAB[5].v;_TEAD=(union Cyc_YYSTYPE*)_TEAC;_T18AC->body=Cyc_yyget_Stmt_tok(_TEAD);_TEAE=yyyvsp;_TEAF=_TEAE[0];_TEB0=_TEAF.l;_TEB1=_TEB0.first_line;_T18AC->loc=Cyc_Position_loc_to_seg(_TEB1);_TEA4=(struct Cyc_Absyn_Switch_clause*)_T18AC;}_T18AB->hd=_TEA4;_TEB2=yyyvsp;_TEB3=& _TEB2[6].v;_TEB4=(union Cyc_YYSTYPE*)_TEB3;_T18AB->tl=Cyc_yyget_YY8(_TEB4);_TEA3=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY8(_TEA3);goto _LL0;case 375: _TEB5=yyyvsp;_TEB6=& _TEB5[2].v;_TEB7=(union Cyc_YYSTYPE*)_TEB6;_TEB8=
# 2684 "parse.y"
Cyc_yyget_Exp_tok(_TEB7);_TEB9=yyyvsp;_TEBA=& _TEB9[4].v;_TEBB=(union Cyc_YYSTYPE*)_TEBA;_TEBC=Cyc_yyget_Stmt_tok(_TEBB);_TEBD=yyyvsp;_TEBE=_TEBD[0];_TEBF=_TEBE.l;_TEC0=_TEBF.first_line;_TEC1=Cyc_Position_loc_to_seg(_TEC0);_TEC2=Cyc_Absyn_while_stmt(_TEB8,_TEBC,_TEC1);yyval=Cyc_Stmt_tok(_TEC2);goto _LL0;case 376: _TEC3=yyyvsp;_TEC4=& _TEC3[1].v;_TEC5=(union Cyc_YYSTYPE*)_TEC4;_TEC6=
# 2686 "parse.y"
Cyc_yyget_Stmt_tok(_TEC5);_TEC7=yyyvsp;_TEC8=& _TEC7[4].v;_TEC9=(union Cyc_YYSTYPE*)_TEC8;_TECA=Cyc_yyget_Exp_tok(_TEC9);_TECB=yyyvsp;_TECC=_TECB[0];_TECD=_TECC.l;_TECE=_TECD.first_line;_TECF=Cyc_Position_loc_to_seg(_TECE);_TED0=Cyc_Absyn_do_stmt(_TEC6,_TECA,_TECF);yyval=Cyc_Stmt_tok(_TED0);goto _LL0;case 377: _TED1=yyyvsp;_TED2=& _TED1[2].v;_TED3=(union Cyc_YYSTYPE*)_TED2;_TED4=
# 2688 "parse.y"
Cyc_yyget_Exp_tok(_TED3);_TED5=yyyvsp;_TED6=& _TED5[4].v;_TED7=(union Cyc_YYSTYPE*)_TED6;_TED8=Cyc_yyget_Exp_tok(_TED7);_TED9=yyyvsp;_TEDA=& _TED9[6].v;_TEDB=(union Cyc_YYSTYPE*)_TEDA;_TEDC=Cyc_yyget_Exp_tok(_TEDB);_TEDD=yyyvsp;_TEDE=& _TEDD[8].v;_TEDF=(union Cyc_YYSTYPE*)_TEDE;_TEE0=Cyc_yyget_Stmt_tok(_TEDF);_TEE1=yyyvsp;_TEE2=_TEE1[0];_TEE3=_TEE2.l;_TEE4=_TEE3.first_line;_TEE5=Cyc_Position_loc_to_seg(_TEE4);_TEE6=Cyc_Absyn_for_stmt(_TED4,_TED8,_TEDC,_TEE0,_TEE5);yyval=Cyc_Stmt_tok(_TEE6);goto _LL0;case 378: _TEE7=
# 2690 "parse.y"
Cyc_Absyn_false_exp(0U);_TEE8=yyyvsp;_TEE9=& _TEE8[3].v;_TEEA=(union Cyc_YYSTYPE*)_TEE9;_TEEB=Cyc_yyget_Exp_tok(_TEEA);_TEEC=yyyvsp;_TEED=& _TEEC[5].v;_TEEE=(union Cyc_YYSTYPE*)_TEED;_TEEF=Cyc_yyget_Exp_tok(_TEEE);_TEF0=yyyvsp;_TEF1=& _TEF0[7].v;_TEF2=(union Cyc_YYSTYPE*)_TEF1;_TEF3=Cyc_yyget_Stmt_tok(_TEF2);_TEF4=yyyvsp;_TEF5=_TEF4[0];_TEF6=_TEF5.l;_TEF7=_TEF6.first_line;_TEF8=Cyc_Position_loc_to_seg(_TEF7);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_for_stmt(_TEE7,_TEEB,_TEEF,_TEF3,_TEF8);_TEF9=yyyvsp;_TEFA=& _TEF9[2].v;_TEFB=(union Cyc_YYSTYPE*)_TEFA;_TEFC=
Cyc_yyget_YY16(_TEFB);_TEFD=s;_TEFE=Cyc_Parse_flatten_declarations(_TEFC,_TEFD);yyval=Cyc_Stmt_tok(_TEFE);goto _LL0;}case 379: _TEFF=
# 2694
Cyc_Absyn_true_exp(0U);yyval=Cyc_Exp_tok(_TEFF);goto _LL0;case 380: _TF00=yyyvsp;_TF01=_TF00[0];
# 2695 "parse.y"
yyval=_TF01.v;goto _LL0;case 381:{struct _fat_ptr*_T18AB=_cycalloc(sizeof(struct _fat_ptr));_TF03=yyyvsp;_TF04=& _TF03[1].v;_TF05=(union Cyc_YYSTYPE*)_TF04;
# 2698
*_T18AB=Cyc_yyget_String_tok(_TF05);_TF02=(struct _fat_ptr*)_T18AB;}_TF06=yyyvsp;_TF07=_TF06[0];_TF08=_TF07.l;_TF09=_TF08.first_line;_TF0A=Cyc_Position_loc_to_seg(_TF09);_TF0B=Cyc_Absyn_goto_stmt(_TF02,_TF0A);yyval=Cyc_Stmt_tok(_TF0B);goto _LL0;case 382: _TF0C=yyyvsp;_TF0D=_TF0C[0];_TF0E=_TF0D.l;_TF0F=_TF0E.first_line;_TF10=
# 2699 "parse.y"
Cyc_Position_loc_to_seg(_TF0F);_TF11=Cyc_Absyn_continue_stmt(_TF10);yyval=Cyc_Stmt_tok(_TF11);goto _LL0;case 383: _TF12=yyyvsp;_TF13=_TF12[0];_TF14=_TF13.l;_TF15=_TF14.first_line;_TF16=
# 2700 "parse.y"
Cyc_Position_loc_to_seg(_TF15);_TF17=Cyc_Absyn_break_stmt(_TF16);yyval=Cyc_Stmt_tok(_TF17);goto _LL0;case 384: _TF18=yyyvsp;_TF19=_TF18[0];_TF1A=_TF19.l;_TF1B=_TF1A.first_line;_TF1C=
# 2701 "parse.y"
Cyc_Position_loc_to_seg(_TF1B);_TF1D=Cyc_Absyn_return_stmt(0,_TF1C);yyval=Cyc_Stmt_tok(_TF1D);goto _LL0;case 385: _TF1E=yyyvsp;_TF1F=& _TF1E[1].v;_TF20=(union Cyc_YYSTYPE*)_TF1F;_TF21=
# 2702 "parse.y"
Cyc_yyget_Exp_tok(_TF20);_TF22=yyyvsp;_TF23=_TF22[0];_TF24=_TF23.l;_TF25=_TF24.first_line;_TF26=Cyc_Position_loc_to_seg(_TF25);_TF27=Cyc_Absyn_return_stmt(_TF21,_TF26);yyval=Cyc_Stmt_tok(_TF27);goto _LL0;case 386: _TF28=yyyvsp;_TF29=_TF28[0];_TF2A=_TF29.l;_TF2B=_TF2A.first_line;_TF2C=
# 2704 "parse.y"
Cyc_Position_loc_to_seg(_TF2B);_TF2D=Cyc_Absyn_fallthru_stmt(0,_TF2C);yyval=Cyc_Stmt_tok(_TF2D);goto _LL0;case 387: _TF2E=yyyvsp;_TF2F=_TF2E[0];_TF30=_TF2F.l;_TF31=_TF30.first_line;_TF32=
# 2705 "parse.y"
Cyc_Position_loc_to_seg(_TF31);_TF33=Cyc_Absyn_fallthru_stmt(0,_TF32);yyval=Cyc_Stmt_tok(_TF33);goto _LL0;case 388: _TF34=yyyvsp;_TF35=& _TF34[2].v;_TF36=(union Cyc_YYSTYPE*)_TF35;_TF37=
# 2707 "parse.y"
Cyc_yyget_YY4(_TF36);_TF38=yyyvsp;_TF39=_TF38[0];_TF3A=_TF39.l;_TF3B=_TF3A.first_line;_TF3C=Cyc_Position_loc_to_seg(_TF3B);_TF3D=Cyc_Absyn_fallthru_stmt(_TF37,_TF3C);yyval=Cyc_Stmt_tok(_TF3D);goto _LL0;case 389: _TF3E=yyyvsp;_TF3F=_TF3E[0];
# 2716 "parse.y"
yyval=_TF3F.v;goto _LL0;case 390: _TF40=yyyvsp;_TF41=_TF40[0];
# 2719
yyval=_TF41.v;goto _LL0;case 391: _TF42=yyyvsp;_TF43=& _TF42[0].v;_TF44=(union Cyc_YYSTYPE*)_TF43;_TF45=
# 2721 "parse.y"
Cyc_yyget_YY9(_TF44);_TF46=Cyc_Parse_pat2exp(_TF45);_TF47=yyyvsp;_TF48=& _TF47[2].v;_TF49=(union Cyc_YYSTYPE*)_TF48;_TF4A=Cyc_yyget_Exp_tok(_TF49);_TF4B=yyyvsp;_TF4C=& _TF4B[4].v;_TF4D=(union Cyc_YYSTYPE*)_TF4C;_TF4E=Cyc_yyget_Exp_tok(_TF4D);_TF4F=yyyvsp;_TF50=_TF4F[0];_TF51=_TF50.l;_TF52=_TF51.first_line;_TF53=Cyc_Position_loc_to_seg(_TF52);_TF54=Cyc_Absyn_conditional_exp(_TF46,_TF4A,_TF4E,_TF53);_TF55=Cyc_Absyn_exp_pat(_TF54);yyval=Cyc_YY9(_TF55);goto _LL0;case 392: _TF56=yyyvsp;_TF57=_TF56[0];
# 2724
yyval=_TF57.v;goto _LL0;case 393: _TF58=yyyvsp;_TF59=& _TF58[0].v;_TF5A=(union Cyc_YYSTYPE*)_TF59;_TF5B=
# 2726 "parse.y"
Cyc_yyget_YY9(_TF5A);_TF5C=Cyc_Parse_pat2exp(_TF5B);_TF5D=yyyvsp;_TF5E=& _TF5D[2].v;_TF5F=(union Cyc_YYSTYPE*)_TF5E;_TF60=Cyc_yyget_Exp_tok(_TF5F);_TF61=yyyvsp;_TF62=_TF61[0];_TF63=_TF62.l;_TF64=_TF63.first_line;_TF65=Cyc_Position_loc_to_seg(_TF64);_TF66=Cyc_Absyn_or_exp(_TF5C,_TF60,_TF65);_TF67=Cyc_Absyn_exp_pat(_TF66);yyval=Cyc_YY9(_TF67);goto _LL0;case 394: _TF68=yyyvsp;_TF69=_TF68[0];
# 2729
yyval=_TF69.v;goto _LL0;case 395: _TF6A=yyyvsp;_TF6B=& _TF6A[0].v;_TF6C=(union Cyc_YYSTYPE*)_TF6B;_TF6D=
# 2731 "parse.y"
Cyc_yyget_YY9(_TF6C);_TF6E=Cyc_Parse_pat2exp(_TF6D);_TF6F=yyyvsp;_TF70=& _TF6F[2].v;_TF71=(union Cyc_YYSTYPE*)_TF70;_TF72=Cyc_yyget_Exp_tok(_TF71);_TF73=yyyvsp;_TF74=_TF73[0];_TF75=_TF74.l;_TF76=_TF75.first_line;_TF77=Cyc_Position_loc_to_seg(_TF76);_TF78=Cyc_Absyn_and_exp(_TF6E,_TF72,_TF77);_TF79=Cyc_Absyn_exp_pat(_TF78);yyval=Cyc_YY9(_TF79);goto _LL0;case 396: _TF7A=yyyvsp;_TF7B=_TF7A[0];
# 2734
yyval=_TF7B.v;goto _LL0;case 397: _TF7C=yyyvsp;_TF7D=& _TF7C[0].v;_TF7E=(union Cyc_YYSTYPE*)_TF7D;_TF7F=
# 2736 "parse.y"
Cyc_yyget_YY9(_TF7E);_TF80=Cyc_Parse_pat2exp(_TF7F);_TF81=yyyvsp;_TF82=& _TF81[2].v;_TF83=(union Cyc_YYSTYPE*)_TF82;_TF84=Cyc_yyget_Exp_tok(_TF83);_TF85=yyyvsp;_TF86=_TF85[0];_TF87=_TF86.l;_TF88=_TF87.first_line;_TF89=Cyc_Position_loc_to_seg(_TF88);_TF8A=Cyc_Absyn_prim2_exp(14U,_TF80,_TF84,_TF89);_TF8B=Cyc_Absyn_exp_pat(_TF8A);yyval=Cyc_YY9(_TF8B);goto _LL0;case 398: _TF8C=yyyvsp;_TF8D=_TF8C[0];
# 2739
yyval=_TF8D.v;goto _LL0;case 399: _TF8E=yyyvsp;_TF8F=& _TF8E[0].v;_TF90=(union Cyc_YYSTYPE*)_TF8F;_TF91=
# 2741 "parse.y"
Cyc_yyget_YY9(_TF90);_TF92=Cyc_Parse_pat2exp(_TF91);_TF93=yyyvsp;_TF94=& _TF93[2].v;_TF95=(union Cyc_YYSTYPE*)_TF94;_TF96=Cyc_yyget_Exp_tok(_TF95);_TF97=yyyvsp;_TF98=_TF97[0];_TF99=_TF98.l;_TF9A=_TF99.first_line;_TF9B=Cyc_Position_loc_to_seg(_TF9A);_TF9C=Cyc_Absyn_prim2_exp(15U,_TF92,_TF96,_TF9B);_TF9D=Cyc_Absyn_exp_pat(_TF9C);yyval=Cyc_YY9(_TF9D);goto _LL0;case 400: _TF9E=yyyvsp;_TF9F=_TF9E[0];
# 2744
yyval=_TF9F.v;goto _LL0;case 401: _TFA0=yyyvsp;_TFA1=& _TFA0[0].v;_TFA2=(union Cyc_YYSTYPE*)_TFA1;_TFA3=
# 2746 "parse.y"
Cyc_yyget_YY9(_TFA2);_TFA4=Cyc_Parse_pat2exp(_TFA3);_TFA5=yyyvsp;_TFA6=& _TFA5[2].v;_TFA7=(union Cyc_YYSTYPE*)_TFA6;_TFA8=Cyc_yyget_Exp_tok(_TFA7);_TFA9=yyyvsp;_TFAA=_TFA9[0];_TFAB=_TFAA.l;_TFAC=_TFAB.first_line;_TFAD=Cyc_Position_loc_to_seg(_TFAC);_TFAE=Cyc_Absyn_prim2_exp(13U,_TFA4,_TFA8,_TFAD);_TFAF=Cyc_Absyn_exp_pat(_TFAE);yyval=Cyc_YY9(_TFAF);goto _LL0;case 402: _TFB0=yyyvsp;_TFB1=_TFB0[0];
# 2749
yyval=_TFB1.v;goto _LL0;case 403: _TFB2=yyyvsp;_TFB3=& _TFB2[1].v;_TFB4=(union Cyc_YYSTYPE*)_TFB3;_TFB5=
# 2751 "parse.y"
Cyc_yyget_YY69(_TFB4);_TFB6=yyyvsp;_TFB7=& _TFB6[0].v;_TFB8=(union Cyc_YYSTYPE*)_TFB7;_TFB9=Cyc_yyget_YY9(_TFB8);_TFBA=Cyc_Parse_pat2exp(_TFB9);_TFBB=yyyvsp;_TFBC=& _TFBB[2].v;_TFBD=(union Cyc_YYSTYPE*)_TFBC;_TFBE=Cyc_yyget_Exp_tok(_TFBD);_TFBF=yyyvsp;_TFC0=_TFBF[0];_TFC1=_TFC0.l;_TFC2=_TFC1.first_line;_TFC3=Cyc_Position_loc_to_seg(_TFC2);_TFC4=_TFB5(_TFBA,_TFBE,_TFC3);_TFC5=Cyc_Absyn_exp_pat(_TFC4);yyval=Cyc_YY9(_TFC5);goto _LL0;case 404: _TFC6=yyyvsp;_TFC7=_TFC6[0];
# 2754
yyval=_TFC7.v;goto _LL0;case 405: _TFC8=yyyvsp;_TFC9=& _TFC8[1].v;_TFCA=(union Cyc_YYSTYPE*)_TFC9;_TFCB=
# 2756 "parse.y"
Cyc_yyget_YY69(_TFCA);_TFCC=yyyvsp;_TFCD=& _TFCC[0].v;_TFCE=(union Cyc_YYSTYPE*)_TFCD;_TFCF=Cyc_yyget_YY9(_TFCE);_TFD0=Cyc_Parse_pat2exp(_TFCF);_TFD1=yyyvsp;_TFD2=& _TFD1[2].v;_TFD3=(union Cyc_YYSTYPE*)_TFD2;_TFD4=Cyc_yyget_Exp_tok(_TFD3);_TFD5=yyyvsp;_TFD6=_TFD5[0];_TFD7=_TFD6.l;_TFD8=_TFD7.first_line;_TFD9=Cyc_Position_loc_to_seg(_TFD8);_TFDA=_TFCB(_TFD0,_TFD4,_TFD9);_TFDB=Cyc_Absyn_exp_pat(_TFDA);yyval=Cyc_YY9(_TFDB);goto _LL0;case 406: _TFDC=yyyvsp;_TFDD=_TFDC[0];
# 2759
yyval=_TFDD.v;goto _LL0;case 407: _TFDE=yyyvsp;_TFDF=& _TFDE[0].v;_TFE0=(union Cyc_YYSTYPE*)_TFDF;_TFE1=
# 2761 "parse.y"
Cyc_yyget_YY9(_TFE0);_TFE2=Cyc_Parse_pat2exp(_TFE1);_TFE3=yyyvsp;_TFE4=& _TFE3[2].v;_TFE5=(union Cyc_YYSTYPE*)_TFE4;_TFE6=Cyc_yyget_Exp_tok(_TFE5);_TFE7=yyyvsp;_TFE8=_TFE7[0];_TFE9=_TFE8.l;_TFEA=_TFE9.first_line;_TFEB=Cyc_Position_loc_to_seg(_TFEA);_TFEC=Cyc_Absyn_prim2_exp(16U,_TFE2,_TFE6,_TFEB);_TFED=Cyc_Absyn_exp_pat(_TFEC);yyval=Cyc_YY9(_TFED);goto _LL0;case 408: _TFEE=yyyvsp;_TFEF=& _TFEE[0].v;_TFF0=(union Cyc_YYSTYPE*)_TFEF;_TFF1=
# 2763 "parse.y"
Cyc_yyget_YY9(_TFF0);_TFF2=Cyc_Parse_pat2exp(_TFF1);_TFF3=yyyvsp;_TFF4=& _TFF3[2].v;_TFF5=(union Cyc_YYSTYPE*)_TFF4;_TFF6=Cyc_yyget_Exp_tok(_TFF5);_TFF7=yyyvsp;_TFF8=_TFF7[0];_TFF9=_TFF8.l;_TFFA=_TFF9.first_line;_TFFB=Cyc_Position_loc_to_seg(_TFFA);_TFFC=Cyc_Absyn_prim2_exp(17U,_TFF2,_TFF6,_TFFB);_TFFD=Cyc_Absyn_exp_pat(_TFFC);yyval=Cyc_YY9(_TFFD);goto _LL0;case 409: _TFFE=yyyvsp;_TFFF=_TFFE[0];
# 2766
yyval=_TFFF.v;goto _LL0;case 410: _T1000=yyyvsp;_T1001=& _T1000[1].v;_T1002=(union Cyc_YYSTYPE*)_T1001;_T1003=
# 2768 "parse.y"
Cyc_yyget_YY6(_T1002);_T1004=yyyvsp;_T1005=& _T1004[0].v;_T1006=(union Cyc_YYSTYPE*)_T1005;_T1007=Cyc_yyget_YY9(_T1006);_T1008=Cyc_Parse_pat2exp(_T1007);_T1009=yyyvsp;_T100A=& _T1009[2].v;_T100B=(union Cyc_YYSTYPE*)_T100A;_T100C=Cyc_yyget_Exp_tok(_T100B);_T100D=yyyvsp;_T100E=_T100D[0];_T100F=_T100E.l;_T1010=_T100F.first_line;_T1011=Cyc_Position_loc_to_seg(_T1010);_T1012=Cyc_Absyn_prim2_exp(_T1003,_T1008,_T100C,_T1011);_T1013=Cyc_Absyn_exp_pat(_T1012);yyval=Cyc_YY9(_T1013);goto _LL0;case 411: _T1014=yyyvsp;_T1015=_T1014[0];
# 2771
yyval=_T1015.v;goto _LL0;case 412: _T1016=yyyvsp;_T1017=& _T1016[1].v;_T1018=(union Cyc_YYSTYPE*)_T1017;_T1019=
# 2773 "parse.y"
Cyc_yyget_YY6(_T1018);_T101A=yyyvsp;_T101B=& _T101A[0].v;_T101C=(union Cyc_YYSTYPE*)_T101B;_T101D=Cyc_yyget_YY9(_T101C);_T101E=Cyc_Parse_pat2exp(_T101D);_T101F=yyyvsp;_T1020=& _T101F[2].v;_T1021=(union Cyc_YYSTYPE*)_T1020;_T1022=Cyc_yyget_Exp_tok(_T1021);_T1023=yyyvsp;_T1024=_T1023[0];_T1025=_T1024.l;_T1026=_T1025.first_line;_T1027=Cyc_Position_loc_to_seg(_T1026);_T1028=Cyc_Absyn_prim2_exp(_T1019,_T101E,_T1022,_T1027);_T1029=Cyc_Absyn_exp_pat(_T1028);yyval=Cyc_YY9(_T1029);goto _LL0;case 413: _T102A=yyyvsp;_T102B=_T102A[0];
# 2776
yyval=_T102B.v;goto _LL0;case 414: _T102C=yyyvsp;_T102D=& _T102C[1].v;_T102E=(union Cyc_YYSTYPE*)_T102D;_T102F=
# 2778 "parse.y"
Cyc_yyget_YY38(_T102E);_T1030=yyyvsp;_T1031=_T1030[1];_T1032=_T1031.l;_T1033=_T1032.first_line;_T1034=Cyc_Position_loc_to_seg(_T1033);{void*t=Cyc_Parse_type_name_to_type(_T102F,_T1034);_T1035=t;_T1036=yyyvsp;_T1037=& _T1036[3].v;_T1038=(union Cyc_YYSTYPE*)_T1037;_T1039=
Cyc_yyget_Exp_tok(_T1038);_T103A=yyyvsp;_T103B=_T103A[0];_T103C=_T103B.l;_T103D=_T103C.first_line;_T103E=Cyc_Position_loc_to_seg(_T103D);_T103F=Cyc_Absyn_cast_exp(_T1035,_T1039,1,0U,_T103E);_T1040=Cyc_Absyn_exp_pat(_T103F);yyval=Cyc_YY9(_T1040);goto _LL0;}case 415: _T1041=yyyvsp;_T1042=_T1041[0];
# 2784 "parse.y"
yyval=_T1042.v;goto _LL0;case 416: _T1043=yyyvsp;_T1044=& _T1043[0].v;_T1045=(union Cyc_YYSTYPE*)_T1044;_T1046=
# 2788 "parse.y"
Cyc_yyget_YY6(_T1045);_T1047=yyyvsp;_T1048=& _T1047[1].v;_T1049=(union Cyc_YYSTYPE*)_T1048;_T104A=Cyc_yyget_Exp_tok(_T1049);_T104B=yyyvsp;_T104C=_T104B[0];_T104D=_T104C.l;_T104E=_T104D.first_line;_T104F=Cyc_Position_loc_to_seg(_T104E);_T1050=Cyc_Absyn_prim1_exp(_T1046,_T104A,_T104F);_T1051=Cyc_Absyn_exp_pat(_T1050);yyval=Cyc_YY9(_T1051);goto _LL0;case 417: _T1052=yyyvsp;_T1053=& _T1052[2].v;_T1054=(union Cyc_YYSTYPE*)_T1053;_T1055=
# 2790 "parse.y"
Cyc_yyget_YY38(_T1054);_T1056=yyyvsp;_T1057=_T1056[2];_T1058=_T1057.l;_T1059=_T1058.first_line;_T105A=Cyc_Position_loc_to_seg(_T1059);{void*t=Cyc_Parse_type_name_to_type(_T1055,_T105A);_T105B=t;_T105C=yyyvsp;_T105D=_T105C[0];_T105E=_T105D.l;_T105F=_T105E.first_line;_T1060=
Cyc_Position_loc_to_seg(_T105F);_T1061=Cyc_Absyn_sizeoftype_exp(_T105B,_T1060);_T1062=Cyc_Absyn_exp_pat(_T1061);yyval=Cyc_YY9(_T1062);goto _LL0;}case 418: _T1063=yyyvsp;_T1064=& _T1063[1].v;_T1065=(union Cyc_YYSTYPE*)_T1064;_T1066=
# 2793 "parse.y"
Cyc_yyget_Exp_tok(_T1065);_T1067=yyyvsp;_T1068=_T1067[0];_T1069=_T1068.l;_T106A=_T1069.first_line;_T106B=Cyc_Position_loc_to_seg(_T106A);_T106C=Cyc_Absyn_sizeofexp_exp(_T1066,_T106B);_T106D=Cyc_Absyn_exp_pat(_T106C);yyval=Cyc_YY9(_T106D);goto _LL0;case 419: _T106E=yyyvsp;_T106F=& _T106E[2].v;_T1070=(union Cyc_YYSTYPE*)_T106F;_T1071=
# 2795 "parse.y"
Cyc_yyget_YY38(_T1070);_T1072=*_T1071;_T1073=_T1072.f2;_T1074=yyyvsp;_T1075=& _T1074[4].v;_T1076=(union Cyc_YYSTYPE*)_T1075;_T1077=Cyc_yyget_YY3(_T1076);_T1078=Cyc_List_imp_rev(_T1077);_T1079=yyyvsp;_T107A=_T1079[0];_T107B=_T107A.l;_T107C=_T107B.first_line;_T107D=Cyc_Position_loc_to_seg(_T107C);_T107E=Cyc_Absyn_offsetof_exp(_T1073,_T1078,_T107D);_T107F=Cyc_Absyn_exp_pat(_T107E);yyval=Cyc_YY9(_T107F);goto _LL0;case 420: _T1080=yyyvsp;_T1081=_T1080[0];
# 2800 "parse.y"
yyval=_T1081.v;goto _LL0;case 421: _T1082=yyyvsp;_T1083=_T1082[0];
# 2808 "parse.y"
yyval=_T1083.v;goto _LL0;case 422: _T1084=& Cyc_Absyn_Wild_p_val;_T1085=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T1084;_T1086=(void*)_T1085;_T1087=yyyvsp;_T1088=_T1087[0];_T1089=_T1088.l;_T108A=_T1089.first_line;_T108B=
# 2812 "parse.y"
Cyc_Position_loc_to_seg(_T108A);_T108C=Cyc_Absyn_new_pat(_T1086,_T108B);yyval=Cyc_YY9(_T108C);goto _LL0;case 423: _T108D=yyyvsp;_T108E=& _T108D[1].v;_T108F=(union Cyc_YYSTYPE*)_T108E;_T1090=
# 2813 "parse.y"
Cyc_yyget_Exp_tok(_T108F);_T1091=Cyc_Absyn_exp_pat(_T1090);yyval=Cyc_YY9(_T1091);goto _LL0;case 424:{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_T18AB->tag=14;_T1093=yyyvsp;_T1094=& _T1093[0].v;_T1095=(union Cyc_YYSTYPE*)_T1094;
# 2814 "parse.y"
_T18AB->f1=Cyc_yyget_QualId_tok(_T1095);_T1092=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T18AB;}_T1096=(void*)_T1092;_T1097=yyyvsp;_T1098=_T1097[0];_T1099=_T1098.l;_T109A=_T1099.first_line;_T109B=Cyc_Position_loc_to_seg(_T109A);_T109C=Cyc_Absyn_new_pat(_T1096,_T109B);yyval=Cyc_YY9(_T109C);goto _LL0;case 425:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T18AB->tag=5;_T109E=yyyvsp;_T109F=& _T109E[1].v;_T10A0=(union Cyc_YYSTYPE*)_T109F;
# 2815 "parse.y"
_T18AB->f1=Cyc_yyget_YY9(_T10A0);_T109D=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T18AB;}_T10A1=(void*)_T109D;_T10A2=yyyvsp;_T10A3=_T10A2[0];_T10A4=_T10A3.l;_T10A5=_T10A4.first_line;_T10A6=Cyc_Position_loc_to_seg(_T10A5);_T10A7=Cyc_Absyn_new_pat(_T10A1,_T10A6);yyval=Cyc_YY9(_T10A7);goto _LL0;case 426: _T10A8=yyyvsp;_T10A9=& _T10A8[0].v;_T10AA=(union Cyc_YYSTYPE*)_T10A9;{
# 2817 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(_T10AA);_T10AB=e;{
void*_T18AB=_T10AB->r;struct _fat_ptr _T18AC;int _T18AD;short _T18AE;char _T18AF;enum Cyc_Absyn_Sign _T18B0;_T10AC=(int*)_T18AB;_T10AD=*_T10AC;if(_T10AD!=0)goto _TL2E8;_T10AE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T18AB;_T10AF=_T10AE->f1;_T10B0=_T10AF.LongLong_c;_T10B1=_T10B0.tag;switch(_T10B1){case 2:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T18B1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T18AB;_T10B2=_T18B1->f1;_T10B3=_T10B2.Char_c;_T10B4=_T10B3.val;_T18B0=_T10B4.f0;_T10B5=_T18B1->f1;_T10B6=_T10B5.Char_c;_T10B7=_T10B6.val;_T18AF=_T10B7.f1;}{enum Cyc_Absyn_Sign s=_T18B0;char i=_T18AF;{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T18B1=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_T18B1->tag=10;
# 2820
_T18B1->f1=i;_T10B8=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T18B1;}_T10B9=(void*)_T10B8;_T10BA=e;_T10BB=_T10BA->loc;_T10BC=Cyc_Absyn_new_pat(_T10B9,_T10BB);yyval=Cyc_YY9(_T10BC);goto _LL542;}case 4:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T18B1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T18AB;_T10BD=_T18B1->f1;_T10BE=_T10BD.Short_c;_T10BF=_T10BE.val;_T18B0=_T10BF.f0;_T10C0=_T18B1->f1;_T10C1=_T10C0.Short_c;_T10C2=_T10C1.val;_T18AE=_T10C2.f1;}{enum Cyc_Absyn_Sign s=_T18B0;short i=_T18AE;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T18B1=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T18B1->tag=9;
# 2822
_T18B1->f1=s;_T10C4=i;_T18B1->f2=(int)_T10C4;_T10C3=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T18B1;}_T10C5=(void*)_T10C3;_T10C6=e;_T10C7=_T10C6->loc;_T10C8=Cyc_Absyn_new_pat(_T10C5,_T10C7);yyval=Cyc_YY9(_T10C8);goto _LL542;}case 5:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T18B1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T18AB;_T10C9=_T18B1->f1;_T10CA=_T10C9.Int_c;_T10CB=_T10CA.val;_T18B0=_T10CB.f0;_T10CC=_T18B1->f1;_T10CD=_T10CC.Int_c;_T10CE=_T10CD.val;_T18AD=_T10CE.f1;}{enum Cyc_Absyn_Sign s=_T18B0;int i=_T18AD;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T18B1=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T18B1->tag=9;
# 2824
_T18B1->f1=s;_T18B1->f2=i;_T10CF=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T18B1;}_T10D0=(void*)_T10CF;_T10D1=e;_T10D2=_T10D1->loc;_T10D3=Cyc_Absyn_new_pat(_T10D0,_T10D2);yyval=Cyc_YY9(_T10D3);goto _LL542;}case 7:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T18B1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T18AB;_T10D4=_T18B1->f1;_T10D5=_T10D4.Float_c;_T10D6=_T10D5.val;_T18AC=_T10D6.f0;_T10D7=_T18B1->f1;_T10D8=_T10D7.Float_c;_T10D9=_T10D8.val;_T18AD=_T10D9.f1;}{struct _fat_ptr s=_T18AC;int i=_T18AD;{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T18B1=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_T18B1->tag=11;
# 2826
_T18B1->f1=s;_T18B1->f2=i;_T10DA=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T18B1;}_T10DB=(void*)_T10DA;_T10DC=e;_T10DD=_T10DC->loc;_T10DE=Cyc_Absyn_new_pat(_T10DB,_T10DD);yyval=Cyc_YY9(_T10DE);goto _LL542;}case 1: _T10DF=& Cyc_Absyn_Null_p_val;_T10E0=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_T10DF;_T10E1=(void*)_T10E0;_T10E2=e;_T10E3=_T10E2->loc;_T10E4=
# 2828
Cyc_Absyn_new_pat(_T10E1,_T10E3);yyval=Cyc_YY9(_T10E4);goto _LL542;case 8: goto _LL550;case 9: _LL550: _T10E5=yyyvsp;_T10E6=_T10E5[0];_T10E7=_T10E6.l;_T10E8=_T10E7.first_line;_T10E9=
# 2831
Cyc_Position_loc_to_seg(_T10E8);_T10EA=_tag_fat("strings cannot occur within patterns",sizeof(char),37U);_T10EB=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10E9,_T10EA,_T10EB);goto _LL542;case 6: _T10EC=yyyvsp;_T10ED=_T10EC[0];_T10EE=_T10ED.l;_T10EF=_T10EE.first_line;_T10F0=
# 2833
Cyc_Position_loc_to_seg(_T10EF);_T10F1=_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U);_T10F2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10F0,_T10F1,_T10F2);goto _LL542;default: goto _LL553;}goto _TL2E9;_TL2E8: _LL553: _T10F3=yyyvsp;_T10F4=_T10F3[0];_T10F5=_T10F4.l;_T10F6=_T10F5.first_line;_T10F7=
# 2835
Cyc_Position_loc_to_seg(_T10F6);_T10F8=_tag_fat("bad constant in case",sizeof(char),21U);_T10F9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10F7,_T10F8,_T10F9);_TL2E9: _LL542:;}goto _LL0;}case 427: _T10FA=yyyvsp;_T10FB=& _T10FA[1].v;_T10FC=(union Cyc_YYSTYPE*)_T10FB;_T10FD=
# 2839 "parse.y"
Cyc_yyget_String_tok(_T10FC);_T10FE=_tag_fat("as",sizeof(char),3U);_T10FF=Cyc_strcmp(_T10FD,_T10FE);if(_T10FF==0)goto _TL2EB;_T1100=yyyvsp;_T1101=_T1100[1];_T1102=_T1101.l;_T1103=_T1102.first_line;_T1104=
Cyc_Position_loc_to_seg(_T1103);_T1105=_tag_fat("expecting `as'",sizeof(char),15U);_T1106=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1104,_T1105,_T1106);goto _TL2EC;_TL2EB: _TL2EC:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T18AB->tag=1;_T1108=yyyvsp;_T1109=_T1108[0];_T110A=_T1109.l;_T110B=_T110A.first_line;_T110C=
Cyc_Position_loc_to_seg(_T110B);{struct _tuple0*_T18AC=_cycalloc(sizeof(struct _tuple0));_T18AC->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));_T110F=yyyvsp;_T1110=& _T110F[0].v;_T1111=(union Cyc_YYSTYPE*)_T1110;*_T18AD=Cyc_yyget_String_tok(_T1111);_T110E=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T110E;_T110D=(struct _tuple0*)_T18AC;}_T1112=Cyc_Absyn_void_type;_T18AB->f1=Cyc_Absyn_new_vardecl(_T110C,_T110D,_T1112,0,0);_T1113=yyyvsp;_T1114=& _T1113[2].v;_T1115=(union Cyc_YYSTYPE*)_T1114;
_T18AB->f2=Cyc_yyget_YY9(_T1115);_T1107=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T18AB;}_T1116=(void*)_T1107;_T1117=yyyvsp;_T1118=_T1117[0];_T1119=_T1118.l;_T111A=_T1119.first_line;_T111B=Cyc_Position_loc_to_seg(_T111A);_T111C=
# 2841
Cyc_Absyn_new_pat(_T1116,_T111B);yyval=Cyc_YY9(_T111C);goto _LL0;case 428: _T111D=yyyvsp;_T111E=& _T111D[0].v;_T111F=(union Cyc_YYSTYPE*)_T111E;_T1120=
# 2845 "parse.y"
Cyc_yyget_String_tok(_T111F);_T1121=_tag_fat("alias",sizeof(char),6U);_T1122=Cyc_strcmp(_T1120,_T1121);if(_T1122==0)goto _TL2ED;_T1123=yyyvsp;_T1124=_T1123[1];_T1125=_T1124.l;_T1126=_T1125.first_line;_T1127=
Cyc_Position_loc_to_seg(_T1126);_T1128=_tag_fat("expecting `alias'",sizeof(char),18U);_T1129=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1127,_T1128,_T1129);goto _TL2EE;_TL2ED: _TL2EE: _T112A=yyyvsp;_T112B=_T112A[0];_T112C=_T112B.l;_T112D=_T112C.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T112D);_T112E=yyyvsp;_T112F=& _T112E[2].v;_T1130=(union Cyc_YYSTYPE*)_T112F;_T1131=
Cyc_yyget_String_tok(_T1130);_T1132=location;Cyc_Parse_tvar_ok(_T1131,_T1132);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T1133=tv;{struct _fat_ptr*_T18AB=_cycalloc(sizeof(struct _fat_ptr));_T1135=yyyvsp;_T1136=& _T1135[2].v;_T1137=(union Cyc_YYSTYPE*)_T1136;*_T18AB=Cyc_yyget_String_tok(_T1137);_T1134=(struct _fat_ptr*)_T18AB;}_T1133->name=_T1134;_T1138=tv;_T1138->identity=- 1;_T1139=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T18AB->tag=0;_T113B=& Cyc_Kinds_ek;_T18AB->f1=(struct Cyc_Absyn_Kind*)_T113B;_T113A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T18AB;}_T1139->kind=(void*)_T113A;_T113C=tv;_T113C->aquals_bound=0;_T113D=yyyvsp;_T113E=_T113D[0];_T113F=_T113E.l;_T1140=_T113F.first_line;_T1141=
Cyc_Position_loc_to_seg(_T1140);{struct _tuple0*_T18AB=_cycalloc(sizeof(struct _tuple0));_T18AB->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T1144=yyyvsp;_T1145=& _T1144[5].v;_T1146=(union Cyc_YYSTYPE*)_T1145;*_T18AC=Cyc_yyget_String_tok(_T1146);_T1143=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_T1143;_T1142=(struct _tuple0*)_T18AB;}_T1147=yyyvsp;_T1148=& _T1147[4].v;_T1149=(union Cyc_YYSTYPE*)_T1148;_T114A=
Cyc_yyget_YY38(_T1149);_T114B=yyyvsp;_T114C=_T114B[4];_T114D=_T114C.l;_T114E=_T114D.first_line;_T114F=Cyc_Position_loc_to_seg(_T114E);_T1150=Cyc_Parse_type_name_to_type(_T114A,_T114F);{
# 2850
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T1141,_T1142,_T1150,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T18AB->tag=2;
# 2852
_T18AB->f1=tv;_T18AB->f2=vd;_T1151=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T18AB;}_T1152=(void*)_T1151;_T1153=location;_T1154=Cyc_Absyn_new_pat(_T1152,_T1153);yyval=Cyc_YY9(_T1154);goto _LL0;}}}case 429: _T1155=yyyvsp;_T1156=& _T1155[0].v;_T1157=(union Cyc_YYSTYPE*)_T1156;_T1158=
# 2855 "parse.y"
Cyc_yyget_String_tok(_T1157);_T1159=_tag_fat("alias",sizeof(char),6U);_T115A=Cyc_strcmp(_T1158,_T1159);if(_T115A==0)goto _TL2EF;_T115B=yyyvsp;_T115C=_T115B[1];_T115D=_T115C.l;_T115E=_T115D.first_line;_T115F=
Cyc_Position_loc_to_seg(_T115E);_T1160=_tag_fat("expecting `alias'",sizeof(char),18U);_T1161=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T115F,_T1160,_T1161);goto _TL2F0;_TL2EF: _TL2F0: _T1162=yyyvsp;_T1163=_T1162[0];_T1164=_T1163.l;_T1165=_T1164.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T1165);_T1166=yyyvsp;_T1167=& _T1166[2].v;_T1168=(union Cyc_YYSTYPE*)_T1167;_T1169=
Cyc_yyget_String_tok(_T1168);_T116A=location;Cyc_Parse_tvar_ok(_T1169,_T116A);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T116B=tv;{struct _fat_ptr*_T18AB=_cycalloc(sizeof(struct _fat_ptr));_T116D=yyyvsp;_T116E=& _T116D[2].v;_T116F=(union Cyc_YYSTYPE*)_T116E;*_T18AB=Cyc_yyget_String_tok(_T116F);_T116C=(struct _fat_ptr*)_T18AB;}_T116B->name=_T116C;_T1170=tv;_T1170->identity=- 1;_T1171=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T18AB->tag=0;_T1173=& Cyc_Kinds_ek;_T18AB->f1=(struct Cyc_Absyn_Kind*)_T1173;_T1172=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T18AB;}_T1171->kind=(void*)_T1172;_T1174=tv;_T1174->aquals_bound=0;_T1175=yyyvsp;_T1176=_T1175[0];_T1177=_T1176.l;_T1178=_T1177.first_line;_T1179=
Cyc_Position_loc_to_seg(_T1178);{struct _tuple0*_T18AB=_cycalloc(sizeof(struct _tuple0));_T18AB->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T117C=yyyvsp;_T117D=& _T117C[5].v;_T117E=(union Cyc_YYSTYPE*)_T117D;*_T18AC=Cyc_yyget_String_tok(_T117E);_T117B=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_T117B;_T117A=(struct _tuple0*)_T18AB;}_T117F=yyyvsp;_T1180=& _T117F[4].v;_T1181=(union Cyc_YYSTYPE*)_T1180;_T1182=
Cyc_yyget_YY38(_T1181);_T1183=yyyvsp;_T1184=_T1183[4];_T1185=_T1184.l;_T1186=_T1185.first_line;_T1187=Cyc_Position_loc_to_seg(_T1186);_T1188=Cyc_Parse_type_name_to_type(_T1182,_T1187);{
# 2860
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T1179,_T117A,_T1188,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T18AB->tag=2;
# 2862
_T18AB->f1=tv;_T18AB->f2=vd;_T1189=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T18AB;}_T118A=(void*)_T1189;_T118B=location;_T118C=Cyc_Absyn_new_pat(_T118A,_T118B);yyval=Cyc_YY9(_T118C);goto _LL0;}}}case 430: _T118D=yyyvsp;_T118E=& _T118D[2].v;_T118F=(union Cyc_YYSTYPE*)_T118E;_T1190=
# 2865 "parse.y"
Cyc_yyget_YY14(_T118F);{struct _tuple23 _T18AB=*_T1190;int _T18AC;struct Cyc_List_List*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_List_List*fps=_T18AD;int dots=_T18AC;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T18AE->tag=6;
_T18AE->f1=0;_T18AE->f2=1;_T18AE->f3=0;_T18AE->f4=fps;_T18AE->f5=dots;_T1191=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T18AE;}_T1192=(void*)_T1191;_T1193=yyyvsp;_T1194=_T1193[0];_T1195=_T1194.l;_T1196=_T1195.first_line;_T1197=Cyc_Position_loc_to_seg(_T1196);_T1198=Cyc_Absyn_new_pat(_T1192,_T1197);yyval=Cyc_YY9(_T1198);goto _LL0;}}case 431: _T1199=yyyvsp;_T119A=& _T1199[2].v;_T119B=(union Cyc_YYSTYPE*)_T119A;_T119C=
# 2869 "parse.y"
Cyc_yyget_YY10(_T119B);{struct _tuple23 _T18AB=*_T119C;int _T18AC;struct Cyc_List_List*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_List_List*ps=_T18AD;int dots=_T18AC;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_T18AE->tag=15;_T119E=yyyvsp;_T119F=& _T119E[0].v;_T11A0=(union Cyc_YYSTYPE*)_T119F;
_T18AE->f1=Cyc_yyget_QualId_tok(_T11A0);_T18AE->f2=ps;_T18AE->f3=dots;_T119D=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T18AE;}_T11A1=(void*)_T119D;_T11A2=yyyvsp;_T11A3=_T11A2[0];_T11A4=_T11A3.l;_T11A5=_T11A4.first_line;_T11A6=Cyc_Position_loc_to_seg(_T11A5);_T11A7=Cyc_Absyn_new_pat(_T11A1,_T11A6);yyval=Cyc_YY9(_T11A7);goto _LL0;}}case 432: _T11A8=yyyvsp;_T11A9=& _T11A8[3].v;_T11AA=(union Cyc_YYSTYPE*)_T11A9;_T11AB=
# 2873 "parse.y"
Cyc_yyget_YY14(_T11AA);{struct _tuple23 _T18AB=*_T11AB;int _T18AC;struct Cyc_List_List*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_List_List*fps=_T18AD;int dots=_T18AC;_T11AD=Cyc_List_map_c;{
struct Cyc_List_List*(*_T18AE)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T11AD;_T11AC=_T18AE;}_T11AE=yyyvsp;_T11AF=_T11AE[2];_T11B0=_T11AF.l;_T11B1=_T11B0.first_line;_T11B2=Cyc_Position_loc_to_seg(_T11B1);_T11B3=yyyvsp;_T11B4=& _T11B3[2].v;_T11B5=(union Cyc_YYSTYPE*)_T11B4;_T11B6=Cyc_yyget_YY41(_T11B5);{struct Cyc_List_List*exist_ts=_T11AC(Cyc_Parse_typ2tvar,_T11B2,_T11B6);_T11B7=yyyvsp;_T11B8=& _T11B7[0].v;_T11B9=(union Cyc_YYSTYPE*)_T11B8;_T11BA=
Cyc_yyget_QualId_tok(_T11B9);{union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,_T11BA,0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_T11BB=typ;_T11BB->tag=0;_T11BC=typ;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T18AE->tag=24;_T18AE->f1=ai;_T11BD=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T18AE;}_T11BC->f1=(void*)_T11BD;_T11BE=typ;_T11BE->f2=0;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T18AE->tag=6;_T11C0=typ;
_T18AE->f1=(void*)_T11C0;_T18AE->f2=0;_T18AE->f3=exist_ts;_T18AE->f4=fps;_T18AE->f5=dots;_T11BF=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T18AE;}_T11C1=(void*)_T11BF;_T11C2=yyyvsp;_T11C3=_T11C2[0];_T11C4=_T11C3.l;_T11C5=_T11C4.first_line;_T11C6=Cyc_Position_loc_to_seg(_T11C5);_T11C7=Cyc_Absyn_new_pat(_T11C1,_T11C6);yyval=Cyc_YY9(_T11C7);goto _LL0;}}}}case 433: _T11C8=yyyvsp;_T11C9=& _T11C8[2].v;_T11CA=(union Cyc_YYSTYPE*)_T11C9;_T11CB=
# 2880 "parse.y"
Cyc_yyget_YY14(_T11CA);{struct _tuple23 _T18AB=*_T11CB;int _T18AC;struct Cyc_List_List*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_List_List*fps=_T18AD;int dots=_T18AC;_T11CD=Cyc_List_map_c;{
struct Cyc_List_List*(*_T18AE)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T11CD;_T11CC=_T18AE;}_T11CE=yyyvsp;_T11CF=_T11CE[1];_T11D0=_T11CF.l;_T11D1=_T11D0.first_line;_T11D2=Cyc_Position_loc_to_seg(_T11D1);_T11D3=yyyvsp;_T11D4=& _T11D3[1].v;_T11D5=(union Cyc_YYSTYPE*)_T11D4;_T11D6=Cyc_yyget_YY41(_T11D5);{struct Cyc_List_List*exist_ts=_T11CC(Cyc_Parse_typ2tvar,_T11D2,_T11D6);{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T18AE->tag=6;
_T18AE->f1=0;_T18AE->f2=0;_T18AE->f3=exist_ts;_T18AE->f4=fps;_T18AE->f5=dots;_T11D7=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T18AE;}_T11D8=(void*)_T11D7;_T11D9=yyyvsp;_T11DA=_T11D9[0];_T11DB=_T11DA.l;_T11DC=_T11DB.first_line;_T11DD=Cyc_Position_loc_to_seg(_T11DC);_T11DE=Cyc_Absyn_new_pat(_T11D8,_T11DD);yyval=Cyc_YY9(_T11DE);goto _LL0;}}}case 434:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T18AB->tag=5;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T18AC->tag=5;_T11E1=yyyvsp;_T11E2=& _T11E1[1].v;_T11E3=(union Cyc_YYSTYPE*)_T11E2;
# 2885 "parse.y"
_T18AC->f1=Cyc_yyget_YY9(_T11E3);_T11E0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T18AC;}_T11E4=(void*)_T11E0;_T11E5=yyyvsp;_T11E6=_T11E5[0];_T11E7=_T11E6.l;_T11E8=_T11E7.first_line;_T11E9=Cyc_Position_loc_to_seg(_T11E8);_T18AB->f1=Cyc_Absyn_new_pat(_T11E4,_T11E9);_T11DF=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T18AB;}_T11EA=(void*)_T11DF;_T11EB=yyyvsp;_T11EC=_T11EB[0];_T11ED=_T11EC.l;_T11EE=_T11ED.first_line;_T11EF=Cyc_Position_loc_to_seg(_T11EE);_T11F0=Cyc_Absyn_new_pat(_T11EA,_T11EF);yyval=Cyc_YY9(_T11F0);goto _LL0;case 435:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T18AB->tag=3;_T11F2=yyyvsp;_T11F3=_T11F2[0];_T11F4=_T11F3.l;_T11F5=_T11F4.first_line;_T11F6=
# 2887 "parse.y"
Cyc_Position_loc_to_seg(_T11F5);{struct _tuple0*_T18AC=_cycalloc(sizeof(struct _tuple0));_T18AC->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));_T11F9=yyyvsp;_T11FA=& _T11F9[1].v;_T11FB=(union Cyc_YYSTYPE*)_T11FA;*_T18AD=Cyc_yyget_String_tok(_T11FB);_T11F8=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T11F8;_T11F7=(struct _tuple0*)_T18AC;}_T11FC=Cyc_Absyn_void_type;_T18AB->f1=Cyc_Absyn_new_vardecl(_T11F6,_T11F7,_T11FC,0,0);_T11FD=& Cyc_Absyn_Wild_p_val;_T11FE=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T11FD;_T11FF=(void*)_T11FE;_T1200=yyyvsp;_T1201=_T1200[1];_T1202=_T1201.l;_T1203=_T1202.first_line;_T1204=
# 2889
Cyc_Position_loc_to_seg(_T1203);_T18AB->f2=Cyc_Absyn_new_pat(_T11FF,_T1204);_T11F1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T18AB;}_T1205=(void*)_T11F1;_T1206=yyyvsp;_T1207=_T1206[0];_T1208=_T1207.l;_T1209=_T1208.first_line;_T120A=
Cyc_Position_loc_to_seg(_T1209);_T120B=
# 2887
Cyc_Absyn_new_pat(_T1205,_T120A);yyval=Cyc_YY9(_T120B);goto _LL0;case 436: _T120C=yyyvsp;_T120D=& _T120C[2].v;_T120E=(union Cyc_YYSTYPE*)_T120D;_T120F=
# 2892 "parse.y"
Cyc_yyget_String_tok(_T120E);_T1210=_tag_fat("as",sizeof(char),3U);_T1211=Cyc_strcmp(_T120F,_T1210);if(_T1211==0)goto _TL2F1;_T1212=yyyvsp;_T1213=_T1212[2];_T1214=_T1213.l;_T1215=_T1214.first_line;_T1216=
Cyc_Position_loc_to_seg(_T1215);_T1217=_tag_fat("expecting `as'",sizeof(char),15U);_T1218=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1216,_T1217,_T1218);goto _TL2F2;_TL2F1: _TL2F2:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T18AB->tag=3;_T121A=yyyvsp;_T121B=_T121A[0];_T121C=_T121B.l;_T121D=_T121C.first_line;_T121E=
Cyc_Position_loc_to_seg(_T121D);{struct _tuple0*_T18AC=_cycalloc(sizeof(struct _tuple0));_T18AC->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));_T1221=yyyvsp;_T1222=& _T1221[1].v;_T1223=(union Cyc_YYSTYPE*)_T1222;*_T18AD=Cyc_yyget_String_tok(_T1223);_T1220=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T1220;_T121F=(struct _tuple0*)_T18AC;}_T1224=Cyc_Absyn_void_type;_T18AB->f1=Cyc_Absyn_new_vardecl(_T121E,_T121F,_T1224,0,0);_T1225=yyyvsp;_T1226=& _T1225[3].v;_T1227=(union Cyc_YYSTYPE*)_T1226;
# 2896
_T18AB->f2=Cyc_yyget_YY9(_T1227);_T1219=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T18AB;}_T1228=(void*)_T1219;_T1229=yyyvsp;_T122A=_T1229[0];_T122B=_T122A.l;_T122C=_T122B.first_line;_T122D=Cyc_Position_loc_to_seg(_T122C);_T122E=
# 2894
Cyc_Absyn_new_pat(_T1228,_T122D);yyval=Cyc_YY9(_T122E);goto _LL0;case 437: _T122F=yyyvsp;_T1230=& _T122F[2].v;_T1231=(union Cyc_YYSTYPE*)_T1230;_T1232=
# 2899 "parse.y"
Cyc_yyget_String_tok(_T1231);_T1233=& Cyc_Kinds_ik;_T1234=(struct Cyc_Absyn_Kind*)_T1233;_T1235=Cyc_Kinds_kind_to_bound(_T1234);_T1236=yyyvsp;_T1237=_T1236[2];_T1238=_T1237.l;_T1239=_T1238.first_line;_T123A=Cyc_Position_loc_to_seg(_T1239);{void*tag=Cyc_Parse_id2type(_T1232,_T1235,0,_T123A);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T18AB->tag=4;_T123C=yyyvsp;_T123D=_T123C[2];_T123E=_T123D.l;_T123F=_T123E.first_line;_T1240=
Cyc_Position_loc_to_seg(_T123F);_T1241=tag;_T18AB->f1=Cyc_Parse_typ2tvar(_T1240,_T1241);_T1242=yyyvsp;_T1243=_T1242[0];_T1244=_T1243.l;_T1245=_T1244.first_line;_T1246=
Cyc_Position_loc_to_seg(_T1245);{struct _tuple0*_T18AC=_cycalloc(sizeof(struct _tuple0));_T18AC->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));_T1249=yyyvsp;_T124A=& _T1249[0].v;_T124B=(union Cyc_YYSTYPE*)_T124A;*_T18AD=Cyc_yyget_String_tok(_T124B);_T1248=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T1248;_T1247=(struct _tuple0*)_T18AC;}_T124C=
Cyc_Absyn_tag_type(tag);
# 2901
_T18AB->f2=Cyc_Absyn_new_vardecl(_T1246,_T1247,_T124C,0,0);_T123B=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T18AB;}_T124D=(void*)_T123B;_T124E=yyyvsp;_T124F=_T124E[0];_T1250=_T124F.l;_T1251=_T1250.first_line;_T1252=
# 2903
Cyc_Position_loc_to_seg(_T1251);_T1253=
# 2900
Cyc_Absyn_new_pat(_T124D,_T1252);yyval=Cyc_YY9(_T1253);goto _LL0;}case 438: _T1254=& Cyc_Kinds_ik;_T1255=(struct Cyc_Absyn_Kind*)_T1254;_T1256=
# 2905 "parse.y"
Cyc_Kinds_kind_to_bound(_T1255);{struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(_T1256);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T18AB->tag=4;
_T18AB->f1=tv;_T1258=yyyvsp;_T1259=_T1258[0];_T125A=_T1259.l;_T125B=_T125A.first_line;_T125C=
Cyc_Position_loc_to_seg(_T125B);{struct _tuple0*_T18AC=_cycalloc(sizeof(struct _tuple0));_T18AC->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));_T125F=yyyvsp;_T1260=& _T125F[0].v;_T1261=(union Cyc_YYSTYPE*)_T1260;*_T18AD=Cyc_yyget_String_tok(_T1261);_T125E=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T125E;_T125D=(struct _tuple0*)_T18AC;}_T1262=
Cyc_Absyn_var_type(tv);_T1263=Cyc_Absyn_tag_type(_T1262);
# 2907
_T18AB->f2=Cyc_Absyn_new_vardecl(_T125C,_T125D,_T1263,0,0);_T1257=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T18AB;}_T1264=(void*)_T1257;_T1265=yyyvsp;_T1266=_T1265[0];_T1267=_T1266.l;_T1268=_T1267.first_line;_T1269=
# 2909
Cyc_Position_loc_to_seg(_T1268);_T126A=
# 2906
Cyc_Absyn_new_pat(_T1264,_T1269);yyval=Cyc_YY9(_T126A);goto _LL0;}case 439:{struct _tuple23*_T18AB=_cycalloc(sizeof(struct _tuple23));_T126C=yyyvsp;_T126D=& _T126C[0].v;_T126E=(union Cyc_YYSTYPE*)_T126D;_T126F=
# 2913 "parse.y"
Cyc_yyget_YY11(_T126E);_T18AB->f0=Cyc_List_rev(_T126F);_T18AB->f1=0;_T126B=(struct _tuple23*)_T18AB;}yyval=Cyc_YY10(_T126B);goto _LL0;case 440:{struct _tuple23*_T18AB=_cycalloc(sizeof(struct _tuple23));_T1271=yyyvsp;_T1272=& _T1271[0].v;_T1273=(union Cyc_YYSTYPE*)_T1272;_T1274=
# 2914 "parse.y"
Cyc_yyget_YY11(_T1273);_T18AB->f0=Cyc_List_rev(_T1274);_T18AB->f1=1;_T1270=(struct _tuple23*)_T18AB;}yyval=Cyc_YY10(_T1270);goto _LL0;case 441:{struct _tuple23*_T18AB=_cycalloc(sizeof(struct _tuple23));
# 2915 "parse.y"
_T18AB->f0=0;_T18AB->f1=1;_T1275=(struct _tuple23*)_T18AB;}yyval=Cyc_YY10(_T1275);goto _LL0;case 442:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T1277=yyyvsp;_T1278=& _T1277[0].v;_T1279=(union Cyc_YYSTYPE*)_T1278;
# 2918
_T18AB->hd=Cyc_yyget_YY9(_T1279);_T18AB->tl=0;_T1276=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY11(_T1276);goto _LL0;case 443:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T127B=yyyvsp;_T127C=& _T127B[2].v;_T127D=(union Cyc_YYSTYPE*)_T127C;
# 2919 "parse.y"
_T18AB->hd=Cyc_yyget_YY9(_T127D);_T127E=yyyvsp;_T127F=& _T127E[0].v;_T1280=(union Cyc_YYSTYPE*)_T127F;_T18AB->tl=Cyc_yyget_YY11(_T1280);_T127A=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY11(_T127A);goto _LL0;case 444:{struct _tuple24*_T18AB=_cycalloc(sizeof(struct _tuple24));
# 2922
_T18AB->f0=0;_T1282=yyyvsp;_T1283=& _T1282[0].v;_T1284=(union Cyc_YYSTYPE*)_T1283;_T18AB->f1=Cyc_yyget_YY9(_T1284);_T1281=(struct _tuple24*)_T18AB;}yyval=Cyc_YY12(_T1281);goto _LL0;case 445:{struct _tuple24*_T18AB=_cycalloc(sizeof(struct _tuple24));_T1286=yyyvsp;_T1287=& _T1286[0].v;_T1288=(union Cyc_YYSTYPE*)_T1287;
# 2923 "parse.y"
_T18AB->f0=Cyc_yyget_YY42(_T1288);_T1289=yyyvsp;_T128A=& _T1289[1].v;_T128B=(union Cyc_YYSTYPE*)_T128A;_T18AB->f1=Cyc_yyget_YY9(_T128B);_T1285=(struct _tuple24*)_T18AB;}yyval=Cyc_YY12(_T1285);goto _LL0;case 446:{struct _tuple23*_T18AB=_cycalloc(sizeof(struct _tuple23));_T128D=yyyvsp;_T128E=& _T128D[0].v;_T128F=(union Cyc_YYSTYPE*)_T128E;_T1290=
# 2926
Cyc_yyget_YY13(_T128F);_T18AB->f0=Cyc_List_rev(_T1290);_T18AB->f1=0;_T128C=(struct _tuple23*)_T18AB;}yyval=Cyc_YY14(_T128C);goto _LL0;case 447:{struct _tuple23*_T18AB=_cycalloc(sizeof(struct _tuple23));_T1292=yyyvsp;_T1293=& _T1292[0].v;_T1294=(union Cyc_YYSTYPE*)_T1293;_T1295=
# 2927 "parse.y"
Cyc_yyget_YY13(_T1294);_T18AB->f0=Cyc_List_rev(_T1295);_T18AB->f1=1;_T1291=(struct _tuple23*)_T18AB;}yyval=Cyc_YY14(_T1291);goto _LL0;case 448:{struct _tuple23*_T18AB=_cycalloc(sizeof(struct _tuple23));
# 2928 "parse.y"
_T18AB->f0=0;_T18AB->f1=1;_T1296=(struct _tuple23*)_T18AB;}yyval=Cyc_YY14(_T1296);goto _LL0;case 449:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T1298=yyyvsp;_T1299=& _T1298[0].v;_T129A=(union Cyc_YYSTYPE*)_T1299;
# 2931
_T18AB->hd=Cyc_yyget_YY12(_T129A);_T18AB->tl=0;_T1297=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY13(_T1297);goto _LL0;case 450:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T129C=yyyvsp;_T129D=& _T129C[2].v;_T129E=(union Cyc_YYSTYPE*)_T129D;
# 2932 "parse.y"
_T18AB->hd=Cyc_yyget_YY12(_T129E);_T129F=yyyvsp;_T12A0=& _T129F[0].v;_T12A1=(union Cyc_YYSTYPE*)_T12A0;_T18AB->tl=Cyc_yyget_YY13(_T12A1);_T129B=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY13(_T129B);goto _LL0;case 451: _T12A2=yyyvsp;_T12A3=_T12A2[0];
# 2937 "parse.y"
yyval=_T12A3.v;goto _LL0;case 452: _T12A4=yyyvsp;_T12A5=& _T12A4[0].v;_T12A6=(union Cyc_YYSTYPE*)_T12A5;_T12A7=
# 2938 "parse.y"
Cyc_yyget_Exp_tok(_T12A6);_T12A8=yyyvsp;_T12A9=& _T12A8[2].v;_T12AA=(union Cyc_YYSTYPE*)_T12A9;_T12AB=Cyc_yyget_Exp_tok(_T12AA);_T12AC=yyyvsp;_T12AD=_T12AC[0];_T12AE=_T12AD.l;_T12AF=_T12AE.first_line;_T12B0=Cyc_Position_loc_to_seg(_T12AF);_T12B1=Cyc_Absyn_seq_exp(_T12A7,_T12AB,_T12B0);yyval=Cyc_Exp_tok(_T12B1);goto _LL0;case 453: _T12B2=yyyvsp;_T12B3=_T12B2[0];
# 2942 "parse.y"
yyval=_T12B3.v;goto _LL0;case 454: _T12B4=yyyvsp;_T12B5=& _T12B4[0].v;_T12B6=(union Cyc_YYSTYPE*)_T12B5;_T12B7=
# 2944 "parse.y"
Cyc_yyget_Exp_tok(_T12B6);_T12B8=yyyvsp;_T12B9=& _T12B8[1].v;_T12BA=(union Cyc_YYSTYPE*)_T12B9;_T12BB=Cyc_yyget_YY7(_T12BA);_T12BC=yyyvsp;_T12BD=& _T12BC[2].v;_T12BE=(union Cyc_YYSTYPE*)_T12BD;_T12BF=Cyc_yyget_Exp_tok(_T12BE);_T12C0=yyyvsp;_T12C1=_T12C0[0];_T12C2=_T12C1.l;_T12C3=_T12C2.first_line;_T12C4=Cyc_Position_loc_to_seg(_T12C3);_T12C5=Cyc_Absyn_assignop_exp(_T12B7,_T12BB,_T12BF,_T12C4);yyval=Cyc_Exp_tok(_T12C5);goto _LL0;case 455: _T12C6=yyyvsp;_T12C7=& _T12C6[0].v;_T12C8=(union Cyc_YYSTYPE*)_T12C7;_T12C9=
# 2946 "parse.y"
Cyc_yyget_Exp_tok(_T12C8);_T12CA=yyyvsp;_T12CB=& _T12CA[2].v;_T12CC=(union Cyc_YYSTYPE*)_T12CB;_T12CD=Cyc_yyget_Exp_tok(_T12CC);_T12CE=yyyvsp;_T12CF=_T12CE[0];_T12D0=_T12CF.l;_T12D1=_T12D0.first_line;_T12D2=Cyc_Position_loc_to_seg(_T12D1);_T12D3=Cyc_Absyn_swap_exp(_T12C9,_T12CD,_T12D2);yyval=Cyc_Exp_tok(_T12D3);goto _LL0;case 456:
# 2950 "parse.y"
 yyval=Cyc_YY7(0);goto _LL0;case 457:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2951 "parse.y"
_T18AB->v=(void*)1U;_T12D4=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12D4);goto _LL0;case 458:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2952 "parse.y"
_T18AB->v=(void*)3U;_T12D5=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12D5);goto _LL0;case 459:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2953 "parse.y"
_T18AB->v=(void*)4U;_T12D6=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12D6);goto _LL0;case 460:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2954 "parse.y"
_T18AB->v=(void*)0U;_T12D7=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12D7);goto _LL0;case 461:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2955 "parse.y"
_T18AB->v=(void*)2U;_T12D8=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12D8);goto _LL0;case 462:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2956 "parse.y"
_T18AB->v=(void*)16U;_T12D9=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12D9);goto _LL0;case 463:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2957 "parse.y"
_T18AB->v=(void*)17U;_T12DA=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12DA);goto _LL0;case 464:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2958 "parse.y"
_T18AB->v=(void*)13U;_T12DB=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12DB);goto _LL0;case 465:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2959 "parse.y"
_T18AB->v=(void*)15U;_T12DC=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12DC);goto _LL0;case 466:{struct Cyc_Core_Opt*_T18AB=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2960 "parse.y"
_T18AB->v=(void*)14U;_T12DD=(struct Cyc_Core_Opt*)_T18AB;}yyval=Cyc_YY7(_T12DD);goto _LL0;case 467: _T12DE=yyyvsp;_T12DF=_T12DE[0];
# 2964 "parse.y"
yyval=_T12DF.v;goto _LL0;case 468: _T12E0=yyyvsp;_T12E1=& _T12E0[0].v;_T12E2=(union Cyc_YYSTYPE*)_T12E1;_T12E3=
# 2966 "parse.y"
Cyc_yyget_Exp_tok(_T12E2);_T12E4=yyyvsp;_T12E5=& _T12E4[2].v;_T12E6=(union Cyc_YYSTYPE*)_T12E5;_T12E7=Cyc_yyget_Exp_tok(_T12E6);_T12E8=yyyvsp;_T12E9=& _T12E8[4].v;_T12EA=(union Cyc_YYSTYPE*)_T12E9;_T12EB=Cyc_yyget_Exp_tok(_T12EA);_T12EC=yyyvsp;_T12ED=_T12EC[0];_T12EE=_T12ED.l;_T12EF=_T12EE.first_line;_T12F0=Cyc_Position_loc_to_seg(_T12EF);_T12F1=Cyc_Absyn_conditional_exp(_T12E3,_T12E7,_T12EB,_T12F0);yyval=Cyc_Exp_tok(_T12F1);goto _LL0;case 469: _T12F2=yyyvsp;_T12F3=& _T12F2[1].v;_T12F4=(union Cyc_YYSTYPE*)_T12F3;_T12F5=
# 2968 "parse.y"
Cyc_yyget_Exp_tok(_T12F4);_T12F6=yyyvsp;_T12F7=_T12F6[0];_T12F8=_T12F7.l;_T12F9=_T12F8.first_line;_T12FA=Cyc_Position_loc_to_seg(_T12F9);_T12FB=Cyc_Absyn_throw_exp(_T12F5,_T12FA);yyval=Cyc_Exp_tok(_T12FB);goto _LL0;case 470: _T12FC=yyyvsp;_T12FD=& _T12FC[1].v;_T12FE=(union Cyc_YYSTYPE*)_T12FD;_T12FF=
# 2970 "parse.y"
Cyc_yyget_Exp_tok(_T12FE);_T1300=yyyvsp;_T1301=_T1300[0];_T1302=_T1301.l;_T1303=_T1302.first_line;_T1304=Cyc_Position_loc_to_seg(_T1303);_T1305=Cyc_Absyn_New_exp(0,_T12FF,0,_T1304);yyval=Cyc_Exp_tok(_T1305);goto _LL0;case 471: _T1306=yyyvsp;_T1307=& _T1306[1].v;_T1308=(union Cyc_YYSTYPE*)_T1307;_T1309=
# 2971 "parse.y"
Cyc_yyget_Exp_tok(_T1308);_T130A=yyyvsp;_T130B=_T130A[0];_T130C=_T130B.l;_T130D=_T130C.first_line;_T130E=Cyc_Position_loc_to_seg(_T130D);_T130F=Cyc_Absyn_New_exp(0,_T1309,0,_T130E);yyval=Cyc_Exp_tok(_T130F);goto _LL0;case 472: _T1310=yyyvsp;_T1311=& _T1310[4].v;_T1312=(union Cyc_YYSTYPE*)_T1311;_T1313=
# 2972 "parse.y"
Cyc_yyget_Exp_tok(_T1312);_T1314=yyyvsp;_T1315=& _T1314[2].v;_T1316=(union Cyc_YYSTYPE*)_T1315;_T1317=Cyc_yyget_Exp_tok(_T1316);_T1318=yyyvsp;_T1319=_T1318[0];_T131A=_T1319.l;_T131B=_T131A.first_line;_T131C=Cyc_Position_loc_to_seg(_T131B);_T131D=Cyc_Absyn_New_exp(0,_T1313,_T1317,_T131C);yyval=Cyc_Exp_tok(_T131D);goto _LL0;case 473: _T131E=yyyvsp;_T131F=& _T131E[4].v;_T1320=(union Cyc_YYSTYPE*)_T131F;_T1321=
# 2973 "parse.y"
Cyc_yyget_Exp_tok(_T1320);_T1322=yyyvsp;_T1323=& _T1322[2].v;_T1324=(union Cyc_YYSTYPE*)_T1323;_T1325=Cyc_yyget_Exp_tok(_T1324);_T1326=yyyvsp;_T1327=_T1326[0];_T1328=_T1327.l;_T1329=_T1328.first_line;_T132A=Cyc_Position_loc_to_seg(_T1329);_T132B=Cyc_Absyn_New_exp(0,_T1321,_T1325,_T132A);yyval=Cyc_Exp_tok(_T132B);goto _LL0;case 474: _T132C=yyyvsp;_T132D=& _T132C[2].v;_T132E=(union Cyc_YYSTYPE*)_T132D;_T132F=
# 2974 "parse.y"
Cyc_yyget_Exp_tok(_T132E);{struct _tuple16 _T18AB=Cyc_Parse_split_seq(_T132F);struct Cyc_Absyn_Exp*_T18AC;struct Cyc_Absyn_Exp*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_Absyn_Exp*rgn=_T18AD;struct Cyc_Absyn_Exp*qual=_T18AC;_T1330=rgn;_T1331=yyyvsp;_T1332=& _T1331[4].v;_T1333=(union Cyc_YYSTYPE*)_T1332;_T1334=Cyc_yyget_Exp_tok(_T1333);_T1335=qual;_T1336=yyyvsp;_T1337=_T1336[0];_T1338=_T1337.l;_T1339=_T1338.first_line;_T133A=Cyc_Position_loc_to_seg(_T1339);_T133B=Cyc_Absyn_New_exp(_T1330,_T1334,_T1335,_T133A);yyval=Cyc_Exp_tok(_T133B);goto _LL0;}}case 475: _T133C=yyyvsp;_T133D=& _T133C[2].v;_T133E=(union Cyc_YYSTYPE*)_T133D;_T133F=
# 2975 "parse.y"
Cyc_yyget_Exp_tok(_T133E);{struct _tuple16 _T18AB=Cyc_Parse_split_seq(_T133F);struct Cyc_Absyn_Exp*_T18AC;struct Cyc_Absyn_Exp*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_Absyn_Exp*rgn=_T18AD;struct Cyc_Absyn_Exp*qual=_T18AC;_T1340=rgn;_T1341=yyyvsp;_T1342=& _T1341[4].v;_T1343=(union Cyc_YYSTYPE*)_T1342;_T1344=Cyc_yyget_Exp_tok(_T1343);_T1345=qual;_T1346=yyyvsp;_T1347=_T1346[0];_T1348=_T1347.l;_T1349=_T1348.first_line;_T134A=Cyc_Position_loc_to_seg(_T1349);_T134B=Cyc_Absyn_New_exp(_T1340,_T1344,_T1345,_T134A);yyval=Cyc_Exp_tok(_T134B);goto _LL0;}}case 476: _T134C=yyyvsp;_T134D=_T134C[0];
# 2987 "parse.y"
yyval=_T134D.v;goto _LL0;case 477: _T134E=yyyvsp;_T134F=_T134E[0];
# 2990
yyval=_T134F.v;goto _LL0;case 478: _T1350=yyyvsp;_T1351=& _T1350[0].v;_T1352=(union Cyc_YYSTYPE*)_T1351;_T1353=
# 2992 "parse.y"
Cyc_yyget_Exp_tok(_T1352);_T1354=yyyvsp;_T1355=& _T1354[2].v;_T1356=(union Cyc_YYSTYPE*)_T1355;_T1357=Cyc_yyget_Exp_tok(_T1356);_T1358=yyyvsp;_T1359=_T1358[0];_T135A=_T1359.l;_T135B=_T135A.first_line;_T135C=Cyc_Position_loc_to_seg(_T135B);_T135D=Cyc_Absyn_or_exp(_T1353,_T1357,_T135C);yyval=Cyc_Exp_tok(_T135D);goto _LL0;case 479: _T135E=yyyvsp;_T135F=_T135E[0];
# 2995
yyval=_T135F.v;goto _LL0;case 480: _T1360=yyyvsp;_T1361=& _T1360[0].v;_T1362=(union Cyc_YYSTYPE*)_T1361;_T1363=
# 2997 "parse.y"
Cyc_yyget_Exp_tok(_T1362);_T1364=yyyvsp;_T1365=& _T1364[2].v;_T1366=(union Cyc_YYSTYPE*)_T1365;_T1367=Cyc_yyget_Exp_tok(_T1366);_T1368=yyyvsp;_T1369=_T1368[0];_T136A=_T1369.l;_T136B=_T136A.first_line;_T136C=Cyc_Position_loc_to_seg(_T136B);_T136D=Cyc_Absyn_and_exp(_T1363,_T1367,_T136C);yyval=Cyc_Exp_tok(_T136D);goto _LL0;case 481: _T136E=yyyvsp;_T136F=_T136E[0];
# 3000
yyval=_T136F.v;goto _LL0;case 482: _T1370=yyyvsp;_T1371=& _T1370[0].v;_T1372=(union Cyc_YYSTYPE*)_T1371;_T1373=
# 3002 "parse.y"
Cyc_yyget_Exp_tok(_T1372);_T1374=yyyvsp;_T1375=& _T1374[2].v;_T1376=(union Cyc_YYSTYPE*)_T1375;_T1377=Cyc_yyget_Exp_tok(_T1376);_T1378=yyyvsp;_T1379=_T1378[0];_T137A=_T1379.l;_T137B=_T137A.first_line;_T137C=Cyc_Position_loc_to_seg(_T137B);_T137D=Cyc_Absyn_prim2_exp(14U,_T1373,_T1377,_T137C);yyval=Cyc_Exp_tok(_T137D);goto _LL0;case 483: _T137E=yyyvsp;_T137F=_T137E[0];
# 3005
yyval=_T137F.v;goto _LL0;case 484: _T1380=yyyvsp;_T1381=& _T1380[0].v;_T1382=(union Cyc_YYSTYPE*)_T1381;_T1383=
# 3007 "parse.y"
Cyc_yyget_Exp_tok(_T1382);_T1384=yyyvsp;_T1385=& _T1384[2].v;_T1386=(union Cyc_YYSTYPE*)_T1385;_T1387=Cyc_yyget_Exp_tok(_T1386);_T1388=yyyvsp;_T1389=_T1388[0];_T138A=_T1389.l;_T138B=_T138A.first_line;_T138C=Cyc_Position_loc_to_seg(_T138B);_T138D=Cyc_Absyn_prim2_exp(15U,_T1383,_T1387,_T138C);yyval=Cyc_Exp_tok(_T138D);goto _LL0;case 485: _T138E=yyyvsp;_T138F=_T138E[0];
# 3010
yyval=_T138F.v;goto _LL0;case 486: _T1390=yyyvsp;_T1391=& _T1390[0].v;_T1392=(union Cyc_YYSTYPE*)_T1391;_T1393=
# 3012 "parse.y"
Cyc_yyget_Exp_tok(_T1392);_T1394=yyyvsp;_T1395=& _T1394[2].v;_T1396=(union Cyc_YYSTYPE*)_T1395;_T1397=Cyc_yyget_Exp_tok(_T1396);_T1398=yyyvsp;_T1399=_T1398[0];_T139A=_T1399.l;_T139B=_T139A.first_line;_T139C=Cyc_Position_loc_to_seg(_T139B);_T139D=Cyc_Absyn_prim2_exp(13U,_T1393,_T1397,_T139C);yyval=Cyc_Exp_tok(_T139D);goto _LL0;case 487: _T139E=yyyvsp;_T139F=_T139E[0];
# 3015
yyval=_T139F.v;goto _LL0;case 488: _T13A0=yyyvsp;_T13A1=& _T13A0[1].v;_T13A2=(union Cyc_YYSTYPE*)_T13A1;_T13A3=
# 3017 "parse.y"
Cyc_yyget_YY69(_T13A2);_T13A4=yyyvsp;_T13A5=& _T13A4[0].v;_T13A6=(union Cyc_YYSTYPE*)_T13A5;_T13A7=Cyc_yyget_Exp_tok(_T13A6);_T13A8=yyyvsp;_T13A9=& _T13A8[2].v;_T13AA=(union Cyc_YYSTYPE*)_T13A9;_T13AB=Cyc_yyget_Exp_tok(_T13AA);_T13AC=yyyvsp;_T13AD=_T13AC[0];_T13AE=_T13AD.l;_T13AF=_T13AE.first_line;_T13B0=Cyc_Position_loc_to_seg(_T13AF);_T13B1=_T13A3(_T13A7,_T13AB,_T13B0);yyval=Cyc_Exp_tok(_T13B1);goto _LL0;case 489: _T13B2=yyyvsp;_T13B3=_T13B2[0];
# 3020
yyval=_T13B3.v;goto _LL0;case 490: _T13B4=yyyvsp;_T13B5=& _T13B4[1].v;_T13B6=(union Cyc_YYSTYPE*)_T13B5;_T13B7=
# 3022 "parse.y"
Cyc_yyget_YY69(_T13B6);_T13B8=yyyvsp;_T13B9=& _T13B8[0].v;_T13BA=(union Cyc_YYSTYPE*)_T13B9;_T13BB=Cyc_yyget_Exp_tok(_T13BA);_T13BC=yyyvsp;_T13BD=& _T13BC[2].v;_T13BE=(union Cyc_YYSTYPE*)_T13BD;_T13BF=Cyc_yyget_Exp_tok(_T13BE);_T13C0=yyyvsp;_T13C1=_T13C0[0];_T13C2=_T13C1.l;_T13C3=_T13C2.first_line;_T13C4=Cyc_Position_loc_to_seg(_T13C3);_T13C5=_T13B7(_T13BB,_T13BF,_T13C4);yyval=Cyc_Exp_tok(_T13C5);goto _LL0;case 491: _T13C6=yyyvsp;_T13C7=_T13C6[0];
# 3025
yyval=_T13C7.v;goto _LL0;case 492: _T13C8=yyyvsp;_T13C9=& _T13C8[0].v;_T13CA=(union Cyc_YYSTYPE*)_T13C9;_T13CB=
# 3027 "parse.y"
Cyc_yyget_Exp_tok(_T13CA);_T13CC=yyyvsp;_T13CD=& _T13CC[2].v;_T13CE=(union Cyc_YYSTYPE*)_T13CD;_T13CF=Cyc_yyget_Exp_tok(_T13CE);_T13D0=yyyvsp;_T13D1=_T13D0[0];_T13D2=_T13D1.l;_T13D3=_T13D2.first_line;_T13D4=Cyc_Position_loc_to_seg(_T13D3);_T13D5=Cyc_Absyn_prim2_exp(16U,_T13CB,_T13CF,_T13D4);yyval=Cyc_Exp_tok(_T13D5);goto _LL0;case 493: _T13D6=yyyvsp;_T13D7=& _T13D6[0].v;_T13D8=(union Cyc_YYSTYPE*)_T13D7;_T13D9=
# 3029 "parse.y"
Cyc_yyget_Exp_tok(_T13D8);_T13DA=yyyvsp;_T13DB=& _T13DA[2].v;_T13DC=(union Cyc_YYSTYPE*)_T13DB;_T13DD=Cyc_yyget_Exp_tok(_T13DC);_T13DE=yyyvsp;_T13DF=_T13DE[0];_T13E0=_T13DF.l;_T13E1=_T13E0.first_line;_T13E2=Cyc_Position_loc_to_seg(_T13E1);_T13E3=Cyc_Absyn_prim2_exp(17U,_T13D9,_T13DD,_T13E2);yyval=Cyc_Exp_tok(_T13E3);goto _LL0;case 494: _T13E4=yyyvsp;_T13E5=_T13E4[0];
# 3032
yyval=_T13E5.v;goto _LL0;case 495: _T13E6=yyyvsp;_T13E7=& _T13E6[1].v;_T13E8=(union Cyc_YYSTYPE*)_T13E7;_T13E9=
# 3034 "parse.y"
Cyc_yyget_YY6(_T13E8);_T13EA=yyyvsp;_T13EB=& _T13EA[0].v;_T13EC=(union Cyc_YYSTYPE*)_T13EB;_T13ED=Cyc_yyget_Exp_tok(_T13EC);_T13EE=yyyvsp;_T13EF=& _T13EE[2].v;_T13F0=(union Cyc_YYSTYPE*)_T13EF;_T13F1=Cyc_yyget_Exp_tok(_T13F0);_T13F2=yyyvsp;_T13F3=_T13F2[0];_T13F4=_T13F3.l;_T13F5=_T13F4.first_line;_T13F6=Cyc_Position_loc_to_seg(_T13F5);_T13F7=Cyc_Absyn_prim2_exp(_T13E9,_T13ED,_T13F1,_T13F6);yyval=Cyc_Exp_tok(_T13F7);goto _LL0;case 496: _T13F8=yyyvsp;_T13F9=_T13F8[0];
# 3037
yyval=_T13F9.v;goto _LL0;case 497: _T13FA=yyyvsp;_T13FB=& _T13FA[1].v;_T13FC=(union Cyc_YYSTYPE*)_T13FB;_T13FD=
# 3039 "parse.y"
Cyc_yyget_YY6(_T13FC);_T13FE=yyyvsp;_T13FF=& _T13FE[0].v;_T1400=(union Cyc_YYSTYPE*)_T13FF;_T1401=Cyc_yyget_Exp_tok(_T1400);_T1402=yyyvsp;_T1403=& _T1402[2].v;_T1404=(union Cyc_YYSTYPE*)_T1403;_T1405=Cyc_yyget_Exp_tok(_T1404);_T1406=yyyvsp;_T1407=_T1406[0];_T1408=_T1407.l;_T1409=_T1408.first_line;_T140A=Cyc_Position_loc_to_seg(_T1409);_T140B=Cyc_Absyn_prim2_exp(_T13FD,_T1401,_T1405,_T140A);yyval=Cyc_Exp_tok(_T140B);goto _LL0;case 498: _T140C=Cyc_Absyn_eq_exp;
# 3042
yyval=Cyc_YY69(_T140C);goto _LL0;case 499: _T140D=Cyc_Absyn_neq_exp;
# 3043 "parse.y"
yyval=Cyc_YY69(_T140D);goto _LL0;case 500: _T140E=Cyc_Absyn_lt_exp;
# 3046
yyval=Cyc_YY69(_T140E);goto _LL0;case 501: _T140F=Cyc_Absyn_gt_exp;
# 3047 "parse.y"
yyval=Cyc_YY69(_T140F);goto _LL0;case 502: _T1410=Cyc_Absyn_lte_exp;
# 3048 "parse.y"
yyval=Cyc_YY69(_T1410);goto _LL0;case 503: _T1411=Cyc_Absyn_gte_exp;
# 3049 "parse.y"
yyval=Cyc_YY69(_T1411);goto _LL0;case 504:
# 3052
 yyval=Cyc_YY6(0U);goto _LL0;case 505:
# 3053 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 506:
# 3056
 yyval=Cyc_YY6(1U);goto _LL0;case 507:
# 3057 "parse.y"
 yyval=Cyc_YY6(3U);goto _LL0;case 508:
# 3058 "parse.y"
 yyval=Cyc_YY6(4U);goto _LL0;case 509: _T1412=yyyvsp;_T1413=_T1412[0];
# 3062 "parse.y"
yyval=_T1413.v;goto _LL0;case 510: _T1414=yyyvsp;_T1415=& _T1414[1].v;_T1416=(union Cyc_YYSTYPE*)_T1415;_T1417=
# 3064 "parse.y"
Cyc_yyget_YY38(_T1416);_T1418=yyyvsp;_T1419=_T1418[1];_T141A=_T1419.l;_T141B=_T141A.first_line;_T141C=Cyc_Position_loc_to_seg(_T141B);{void*t=Cyc_Parse_type_name_to_type(_T1417,_T141C);_T141D=t;_T141E=yyyvsp;_T141F=& _T141E[3].v;_T1420=(union Cyc_YYSTYPE*)_T141F;_T1421=
Cyc_yyget_Exp_tok(_T1420);_T1422=yyyvsp;_T1423=_T1422[0];_T1424=_T1423.l;_T1425=_T1424.first_line;_T1426=Cyc_Position_loc_to_seg(_T1425);_T1427=Cyc_Absyn_cast_exp(_T141D,_T1421,1,0U,_T1426);yyval=Cyc_Exp_tok(_T1427);goto _LL0;}case 511: _T1428=yyyvsp;_T1429=_T1428[0];
# 3069 "parse.y"
yyval=_T1429.v;goto _LL0;case 512: _T142A=yyyvsp;_T142B=& _T142A[1].v;_T142C=(union Cyc_YYSTYPE*)_T142B;_T142D=
# 3070 "parse.y"
Cyc_yyget_Exp_tok(_T142C);_T142E=yyyvsp;_T142F=_T142E[0];_T1430=_T142F.l;_T1431=_T1430.first_line;_T1432=Cyc_Position_loc_to_seg(_T1431);_T1433=Cyc_Absyn_increment_exp(_T142D,0U,_T1432);yyval=Cyc_Exp_tok(_T1433);goto _LL0;case 513: _T1434=yyyvsp;_T1435=& _T1434[1].v;_T1436=(union Cyc_YYSTYPE*)_T1435;_T1437=
# 3071 "parse.y"
Cyc_yyget_Exp_tok(_T1436);_T1438=yyyvsp;_T1439=_T1438[0];_T143A=_T1439.l;_T143B=_T143A.first_line;_T143C=Cyc_Position_loc_to_seg(_T143B);_T143D=Cyc_Absyn_increment_exp(_T1437,2U,_T143C);yyval=Cyc_Exp_tok(_T143D);goto _LL0;case 514: _T143E=yyyvsp;_T143F=& _T143E[1].v;_T1440=(union Cyc_YYSTYPE*)_T143F;_T1441=
# 3072 "parse.y"
Cyc_yyget_Exp_tok(_T1440);_T1442=yyyvsp;_T1443=_T1442[0];_T1444=_T1443.l;_T1445=_T1444.first_line;_T1446=Cyc_Position_loc_to_seg(_T1445);_T1447=Cyc_Absyn_address_exp(_T1441,_T1446);yyval=Cyc_Exp_tok(_T1447);goto _LL0;case 515: _T1448=yyyvsp;_T1449=& _T1448[1].v;_T144A=(union Cyc_YYSTYPE*)_T1449;_T144B=
# 3073 "parse.y"
Cyc_yyget_Exp_tok(_T144A);_T144C=yyyvsp;_T144D=_T144C[0];_T144E=_T144D.l;_T144F=_T144E.first_line;_T1450=Cyc_Position_loc_to_seg(_T144F);_T1451=Cyc_Absyn_deref_exp(_T144B,_T1450);yyval=Cyc_Exp_tok(_T1451);goto _LL0;case 516: _T1452=yyyvsp;_T1453=& _T1452[0].v;_T1454=(union Cyc_YYSTYPE*)_T1453;_T1455=
# 3074 "parse.y"
Cyc_yyget_YY6(_T1454);_T1456=yyyvsp;_T1457=& _T1456[1].v;_T1458=(union Cyc_YYSTYPE*)_T1457;_T1459=Cyc_yyget_Exp_tok(_T1458);_T145A=yyyvsp;_T145B=_T145A[0];_T145C=_T145B.l;_T145D=_T145C.first_line;_T145E=Cyc_Position_loc_to_seg(_T145D);_T145F=Cyc_Absyn_prim1_exp(_T1455,_T1459,_T145E);yyval=Cyc_Exp_tok(_T145F);goto _LL0;case 517: _T1460=yyyvsp;_T1461=& _T1460[2].v;_T1462=(union Cyc_YYSTYPE*)_T1461;_T1463=
# 3076 "parse.y"
Cyc_yyget_YY38(_T1462);_T1464=yyyvsp;_T1465=_T1464[2];_T1466=_T1465.l;_T1467=_T1466.first_line;_T1468=Cyc_Position_loc_to_seg(_T1467);{void*t=Cyc_Parse_type_name_to_type(_T1463,_T1468);_T1469=t;_T146A=yyyvsp;_T146B=_T146A[0];_T146C=_T146B.l;_T146D=_T146C.first_line;_T146E=
Cyc_Position_loc_to_seg(_T146D);_T146F=Cyc_Absyn_sizeoftype_exp(_T1469,_T146E);yyval=Cyc_Exp_tok(_T146F);goto _LL0;}case 518: _T1470=yyyvsp;_T1471=& _T1470[1].v;_T1472=(union Cyc_YYSTYPE*)_T1471;_T1473=
# 3079 "parse.y"
Cyc_yyget_Exp_tok(_T1472);_T1474=yyyvsp;_T1475=_T1474[0];_T1476=_T1475.l;_T1477=_T1476.first_line;_T1478=Cyc_Position_loc_to_seg(_T1477);_T1479=Cyc_Absyn_sizeofexp_exp(_T1473,_T1478);yyval=Cyc_Exp_tok(_T1479);goto _LL0;case 519: _T147A=yyyvsp;_T147B=& _T147A[2].v;_T147C=(union Cyc_YYSTYPE*)_T147B;_T147D=
# 3081 "parse.y"
Cyc_yyget_YY38(_T147C);_T147E=yyyvsp;_T147F=_T147E[2];_T1480=_T147F.l;_T1481=_T1480.first_line;_T1482=Cyc_Position_loc_to_seg(_T1481);{void*t=Cyc_Parse_type_name_to_type(_T147D,_T1482);_T1483=t;_T1484=yyyvsp;_T1485=& _T1484[4].v;_T1486=(union Cyc_YYSTYPE*)_T1485;_T1487=
Cyc_yyget_YY3(_T1486);_T1488=Cyc_List_imp_rev(_T1487);_T1489=yyyvsp;_T148A=_T1489[0];_T148B=_T148A.l;_T148C=_T148B.first_line;_T148D=Cyc_Position_loc_to_seg(_T148C);_T148E=Cyc_Absyn_offsetof_exp(_T1483,_T1488,_T148D);yyval=Cyc_Exp_tok(_T148E);goto _LL0;}case 520:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T18AB->tag=33;
# 3086
_T18AB->f1.mknd=0U;_T18AB->f1.rgn=0;_T18AB->f1.aqual=0;_T18AB->f1.elt_type=0;_T1490=yyyvsp;_T1491=& _T1490[2].v;_T1492=(union Cyc_YYSTYPE*)_T1491;_T18AB->f1.num_elts=Cyc_yyget_Exp_tok(_T1492);_T18AB->f1.fat_result=0;_T18AB->f1.inline_call=0;_T148F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T18AB;}_T1493=(void*)_T148F;_T1494=yyyvsp;_T1495=_T1494[0];_T1496=_T1495.l;_T1497=_T1496.first_line;_T1498=
Cyc_Position_loc_to_seg(_T1497);_T1499=
# 3086
Cyc_Absyn_new_exp(_T1493,_T1498);yyval=Cyc_Exp_tok(_T1499);goto _LL0;case 521:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T18AB->tag=33;
# 3089 "parse.y"
_T18AB->f1.mknd=0U;_T149B=yyyvsp;_T149C=& _T149B[2].v;_T149D=(union Cyc_YYSTYPE*)_T149C;_T18AB->f1.rgn=Cyc_yyget_Exp_tok(_T149D);_T149E=yyyvsp;_T149F=& _T149E[4].v;_T14A0=(union Cyc_YYSTYPE*)_T149F;_T18AB->f1.aqual=Cyc_yyget_Exp_tok(_T14A0);_T18AB->f1.elt_type=0;_T14A1=yyyvsp;_T14A2=& _T14A1[6].v;_T14A3=(union Cyc_YYSTYPE*)_T14A2;_T18AB->f1.num_elts=Cyc_yyget_Exp_tok(_T14A3);_T18AB->f1.fat_result=0;_T18AB->f1.inline_call=0;_T149A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T18AB;}_T14A4=(void*)_T149A;_T14A5=yyyvsp;_T14A6=_T14A5[0];_T14A7=_T14A6.l;_T14A8=_T14A7.first_line;_T14A9=
Cyc_Position_loc_to_seg(_T14A8);_T14AA=
# 3089
Cyc_Absyn_new_exp(_T14A4,_T14A9);yyval=Cyc_Exp_tok(_T14AA);goto _LL0;case 522:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T18AB->tag=33;
# 3092 "parse.y"
_T18AB->f1.mknd=0U;_T14AC=yyyvsp;_T14AD=& _T14AC[2].v;_T14AE=(union Cyc_YYSTYPE*)_T14AD;_T18AB->f1.rgn=Cyc_yyget_Exp_tok(_T14AE);_T18AB->f1.aqual=0;_T18AB->f1.elt_type=0;_T14AF=yyyvsp;_T14B0=& _T14AF[4].v;_T14B1=(union Cyc_YYSTYPE*)_T14B0;_T18AB->f1.num_elts=Cyc_yyget_Exp_tok(_T14B1);_T18AB->f1.fat_result=0;_T18AB->f1.inline_call=0;_T14AB=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T18AB;}_T14B2=(void*)_T14AB;_T14B3=yyyvsp;_T14B4=_T14B3[0];_T14B5=_T14B4.l;_T14B6=_T14B5.first_line;_T14B7=
Cyc_Position_loc_to_seg(_T14B6);_T14B8=
# 3092
Cyc_Absyn_new_exp(_T14B2,_T14B7);yyval=Cyc_Exp_tok(_T14B8);goto _LL0;case 523: _T14B9=yyyvsp;_T14BA=& _T14B9[2].v;_T14BB=(union Cyc_YYSTYPE*)_T14BA;_T14BC=
# 3095 "parse.y"
Cyc_yyget_Exp_tok(_T14BB);{struct _tuple16 _T18AB=Cyc_Parse_split_seq(_T14BC);struct Cyc_Absyn_Exp*_T18AC;struct Cyc_Absyn_Exp*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_Absyn_Exp*rgn=_T18AD;struct Cyc_Absyn_Exp*qual=_T18AC;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T18AE->tag=33;
_T18AE->f1.mknd=2U;_T18AE->f1.rgn=rgn;_T18AE->f1.aqual=qual;_T18AE->f1.elt_type=0;_T14BE=yyyvsp;_T14BF=& _T14BE[4].v;_T14C0=(union Cyc_YYSTYPE*)_T14BF;_T18AE->f1.num_elts=Cyc_yyget_Exp_tok(_T14C0);_T18AE->f1.fat_result=0;_T18AE->f1.inline_call=0;_T14BD=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T18AE;}_T14C1=(void*)_T14BD;_T14C2=yyyvsp;_T14C3=_T14C2[0];_T14C4=_T14C3.l;_T14C5=_T14C4.first_line;_T14C6=
Cyc_Position_loc_to_seg(_T14C5);_T14C7=
# 3096
Cyc_Absyn_new_exp(_T14C1,_T14C6);yyval=Cyc_Exp_tok(_T14C7);goto _LL0;}}case 524: _T14C8=yyyvsp;_T14C9=& _T14C8[2].v;_T14CA=(union Cyc_YYSTYPE*)_T14C9;_T14CB=
# 3099 "parse.y"
Cyc_yyget_Exp_tok(_T14CA);{struct _tuple16 _T18AB=Cyc_Parse_split_seq(_T14CB);struct Cyc_Absyn_Exp*_T18AC;struct Cyc_Absyn_Exp*_T18AD;_T18AD=_T18AB.f0;_T18AC=_T18AB.f1;{struct Cyc_Absyn_Exp*rgn=_T18AD;struct Cyc_Absyn_Exp*qual=_T18AC;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T18AE->tag=33;
_T18AE->f1.mknd=0U;_T18AE->f1.rgn=rgn;_T18AE->f1.aqual=qual;_T18AE->f1.elt_type=0;_T14CD=yyyvsp;_T14CE=& _T14CD[4].v;_T14CF=(union Cyc_YYSTYPE*)_T14CE;_T18AE->f1.num_elts=Cyc_yyget_Exp_tok(_T14CF);_T18AE->f1.fat_result=0;_T18AE->f1.inline_call=1;_T14CC=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T18AE;}_T14D0=(void*)_T14CC;_T14D1=yyyvsp;_T14D2=_T14D1[0];_T14D3=_T14D2.l;_T14D4=_T14D3.first_line;_T14D5=
Cyc_Position_loc_to_seg(_T14D4);_T14D6=
# 3100
Cyc_Absyn_new_exp(_T14D0,_T14D5);yyval=Cyc_Exp_tok(_T14D6);goto _LL0;}}case 525: _T14D7=yyyvsp;_T14D8=& _T14D7[6].v;_T14D9=(union Cyc_YYSTYPE*)_T14D8;_T14DA=
# 3103 "parse.y"
Cyc_yyget_YY38(_T14D9);_T14DB=yyyvsp;_T14DC=_T14DB[6];_T14DD=_T14DC.l;_T14DE=_T14DD.first_line;_T14DF=Cyc_Position_loc_to_seg(_T14DE);{void*t=Cyc_Parse_type_name_to_type(_T14DA,_T14DF);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T18AB->tag=33;
_T18AB->f1.mknd=1U;_T18AB->f1.rgn=0;_T18AB->f1.aqual=0;{void**_T18AC=_cycalloc(sizeof(void*));*_T18AC=t;_T14E1=(void**)_T18AC;}_T18AB->f1.elt_type=_T14E1;_T14E2=yyyvsp;_T14E3=& _T14E2[2].v;_T14E4=(union Cyc_YYSTYPE*)_T14E3;_T18AB->f1.num_elts=Cyc_yyget_Exp_tok(_T14E4);_T18AB->f1.fat_result=0;_T18AB->f1.inline_call=0;_T14E0=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T18AB;}_T14E5=(void*)_T14E0;_T14E6=yyyvsp;_T14E7=_T14E6[0];_T14E8=_T14E7.l;_T14E9=_T14E8.first_line;_T14EA=
Cyc_Position_loc_to_seg(_T14E9);_T14EB=
# 3104
Cyc_Absyn_new_exp(_T14E5,_T14EA);yyval=Cyc_Exp_tok(_T14EB);goto _LL0;}case 526: _T14EC=yyyvsp;_T14ED=& _T14EC[10].v;_T14EE=(union Cyc_YYSTYPE*)_T14ED;_T14EF=
# 3107 "parse.y"
Cyc_yyget_YY38(_T14EE);_T14F0=yyyvsp;_T14F1=_T14F0[10];_T14F2=_T14F1.l;_T14F3=_T14F2.first_line;_T14F4=Cyc_Position_loc_to_seg(_T14F3);{void*t=Cyc_Parse_type_name_to_type(_T14EF,_T14F4);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T18AB->tag=33;
_T18AB->f1.mknd=1U;_T14F6=yyyvsp;_T14F7=& _T14F6[2].v;_T14F8=(union Cyc_YYSTYPE*)_T14F7;_T18AB->f1.rgn=Cyc_yyget_Exp_tok(_T14F8);_T14F9=yyyvsp;_T14FA=& _T14F9[4].v;_T14FB=(union Cyc_YYSTYPE*)_T14FA;_T18AB->f1.aqual=Cyc_yyget_Exp_tok(_T14FB);{void**_T18AC=_cycalloc(sizeof(void*));*_T18AC=t;_T14FC=(void**)_T18AC;}_T18AB->f1.elt_type=_T14FC;_T14FD=yyyvsp;_T14FE=& _T14FD[6].v;_T14FF=(union Cyc_YYSTYPE*)_T14FE;_T18AB->f1.num_elts=Cyc_yyget_Exp_tok(_T14FF);_T18AB->f1.fat_result=0;_T18AB->f1.inline_call=0;_T14F5=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T18AB;}_T1500=(void*)_T14F5;_T1501=yyyvsp;_T1502=_T1501[0];_T1503=_T1502.l;_T1504=_T1503.first_line;_T1505=
Cyc_Position_loc_to_seg(_T1504);_T1506=
# 3108
Cyc_Absyn_new_exp(_T1500,_T1505);yyval=Cyc_Exp_tok(_T1506);goto _LL0;}case 527: _T1507=yyyvsp;_T1508=& _T1507[8].v;_T1509=(union Cyc_YYSTYPE*)_T1508;_T150A=
# 3112
Cyc_yyget_YY38(_T1509);_T150B=yyyvsp;_T150C=_T150B[8];_T150D=_T150C.l;_T150E=_T150D.first_line;_T150F=Cyc_Position_loc_to_seg(_T150E);{void*t=Cyc_Parse_type_name_to_type(_T150A,_T150F);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T18AB->tag=33;
_T18AB->f1.mknd=1U;_T1511=yyyvsp;_T1512=& _T1511[2].v;_T1513=(union Cyc_YYSTYPE*)_T1512;_T18AB->f1.rgn=Cyc_yyget_Exp_tok(_T1513);_T18AB->f1.aqual=0;{void**_T18AC=_cycalloc(sizeof(void*));*_T18AC=t;_T1514=(void**)_T18AC;}_T18AB->f1.elt_type=_T1514;_T1515=yyyvsp;_T1516=& _T1515[4].v;_T1517=(union Cyc_YYSTYPE*)_T1516;_T18AB->f1.num_elts=Cyc_yyget_Exp_tok(_T1517);_T18AB->f1.fat_result=0;_T18AB->f1.inline_call=0;_T1510=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T18AB;}_T1518=(void*)_T1510;_T1519=yyyvsp;_T151A=_T1519[0];_T151B=_T151A.l;_T151C=_T151B.first_line;_T151D=
Cyc_Position_loc_to_seg(_T151C);_T151E=
# 3113
Cyc_Absyn_new_exp(_T1518,_T151D);yyval=Cyc_Exp_tok(_T151E);goto _LL0;}case 528:{struct Cyc_Absyn_Exp*_T18AB[1];_T1520=yyyvsp;_T1521=& _T1520[2].v;_T1522=(union Cyc_YYSTYPE*)_T1521;_T18AB[0]=
# 3116 "parse.y"
Cyc_yyget_Exp_tok(_T1522);_T1523=_tag_fat(_T18AB,sizeof(struct Cyc_Absyn_Exp*),1);_T151F=Cyc_List_list(_T1523);}_T1524=yyyvsp;_T1525=_T1524[0];_T1526=_T1525.l;_T1527=_T1526.first_line;_T1528=Cyc_Position_loc_to_seg(_T1527);_T1529=Cyc_Absyn_primop_exp(18U,_T151F,_T1528);yyval=Cyc_Exp_tok(_T1529);goto _LL0;case 529: _T152A=yyyvsp;_T152B=& _T152A[2].v;_T152C=(union Cyc_YYSTYPE*)_T152B;_T152D=
# 3118 "parse.y"
Cyc_yyget_Exp_tok(_T152C);_T152E=yyyvsp;_T152F=_T152E[0];_T1530=_T152F.l;_T1531=_T1530.first_line;_T1532=Cyc_Position_loc_to_seg(_T1531);_T1533=Cyc_Absyn_tagof_exp(_T152D,_T1532);yyval=Cyc_Exp_tok(_T1533);goto _LL0;case 530:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T18AB->tag=37;_T1535=yyyvsp;_T1536=& _T1535[2].v;_T1537=(union Cyc_YYSTYPE*)_T1536;
# 3120 "parse.y"
_T18AB->f1=Cyc_yyget_Exp_tok(_T1537);{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T1539=yyyvsp;_T153A=& _T1539[4].v;_T153B=(union Cyc_YYSTYPE*)_T153A;*_T18AC=Cyc_yyget_String_tok(_T153B);_T1538=(struct _fat_ptr*)_T18AC;}_T18AB->f2=_T1538;_T1534=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T18AB;}_T153C=(void*)_T1534;_T153D=yyyvsp;_T153E=_T153D[0];_T153F=_T153E.l;_T1540=_T153F.first_line;_T1541=Cyc_Position_loc_to_seg(_T1540);_T1542=Cyc_Absyn_new_exp(_T153C,_T1541);yyval=Cyc_Exp_tok(_T1542);goto _LL0;case 531:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T18AB->tag=37;_T1544=yyyvsp;_T1545=& _T1544[2].v;_T1546=(union Cyc_YYSTYPE*)_T1545;_T1547=
# 3122 "parse.y"
Cyc_yyget_Exp_tok(_T1546);_T1548=yyyvsp;_T1549=_T1548[2];_T154A=_T1549.l;_T154B=_T154A.first_line;_T154C=Cyc_Position_loc_to_seg(_T154B);_T18AB->f1=Cyc_Absyn_deref_exp(_T1547,_T154C);{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T154E=yyyvsp;_T154F=& _T154E[4].v;_T1550=(union Cyc_YYSTYPE*)_T154F;*_T18AC=Cyc_yyget_String_tok(_T1550);_T154D=(struct _fat_ptr*)_T18AC;}_T18AB->f2=_T154D;_T1543=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T18AB;}_T1551=(void*)_T1543;_T1552=yyyvsp;_T1553=_T1552[0];_T1554=_T1553.l;_T1555=_T1554.first_line;_T1556=Cyc_Position_loc_to_seg(_T1555);_T1557=Cyc_Absyn_new_exp(_T1551,_T1556);yyval=Cyc_Exp_tok(_T1557);goto _LL0;case 532: _T1558=yyyvsp;_T1559=& _T1558[2].v;_T155A=(union Cyc_YYSTYPE*)_T1559;_T155B=
# 3124 "parse.y"
Cyc_yyget_YY38(_T155A);_T155C=yyyvsp;_T155D=_T155C[2];_T155E=_T155D.l;_T155F=_T155E.first_line;_T1560=Cyc_Position_loc_to_seg(_T155F);{void*t=Cyc_Parse_type_name_to_type(_T155B,_T1560);_T1561=t;_T1562=yyyvsp;_T1563=_T1562[0];_T1564=_T1563.l;_T1565=_T1564.first_line;_T1566=
Cyc_Position_loc_to_seg(_T1565);_T1567=Cyc_Absyn_valueof_exp(_T1561,_T1566);yyval=Cyc_Exp_tok(_T1567);goto _LL0;}case 533: _T1568=yyyvsp;_T1569=& _T1568[1].v;_T156A=(union Cyc_YYSTYPE*)_T1569;_T156B=
# 3126 "parse.y"
Cyc_yyget_YY63(_T156A);_T156C=yyyvsp;_T156D=_T156C[0];_T156E=_T156D.l;_T156F=_T156E.first_line;_T1570=Cyc_Position_loc_to_seg(_T156F);_T1571=Cyc_Absyn_new_exp(_T156B,_T1570);yyval=Cyc_Exp_tok(_T1571);goto _LL0;case 534: _T1572=yyyvsp;_T1573=& _T1572[1].v;_T1574=(union Cyc_YYSTYPE*)_T1573;_T1575=
# 3127 "parse.y"
Cyc_yyget_Exp_tok(_T1574);_T1576=yyyvsp;_T1577=_T1576[0];_T1578=_T1577.l;_T1579=_T1578.first_line;_T157A=Cyc_Position_loc_to_seg(_T1579);_T157B=Cyc_Absyn_extension_exp(_T1575,_T157A);yyval=Cyc_Exp_tok(_T157B);goto _LL0;case 535: _T157C=yyyvsp;_T157D=& _T157C[2].v;_T157E=(union Cyc_YYSTYPE*)_T157D;_T157F=
# 3128 "parse.y"
Cyc_yyget_Exp_tok(_T157E);_T1580=yyyvsp;_T1581=_T1580[0];_T1582=_T1581.l;_T1583=_T1582.first_line;_T1584=Cyc_Position_loc_to_seg(_T1583);_T1585=Cyc_Absyn_assert_exp(_T157F,0,_T1584);yyval=Cyc_Exp_tok(_T1585);goto _LL0;case 536: _T1586=yyyvsp;_T1587=& _T1586[2].v;_T1588=(union Cyc_YYSTYPE*)_T1587;_T1589=
# 3129 "parse.y"
Cyc_yyget_Exp_tok(_T1588);_T158A=yyyvsp;_T158B=_T158A[0];_T158C=_T158B.l;_T158D=_T158C.first_line;_T158E=Cyc_Position_loc_to_seg(_T158D);_T158F=Cyc_Absyn_assert_exp(_T1589,1,_T158E);yyval=Cyc_Exp_tok(_T158F);goto _LL0;case 537: _T1590=yyyvsp;_T1591=& _T1590[2].v;_T1592=(union Cyc_YYSTYPE*)_T1591;_T1593=
# 3130 "parse.y"
Cyc_yyget_Exp_tok(_T1592);_T1594=yyyvsp;_T1595=_T1594[0];_T1596=_T1595.l;_T1597=_T1596.first_line;_T1598=Cyc_Position_loc_to_seg(_T1597);_T1599=Cyc_Absyn_assert_false_exp(_T1593,_T1598);yyval=Cyc_Exp_tok(_T1599);goto _LL0;case 538:
# 3134 "parse.y"
 yyval=Cyc_YY6(12U);goto _LL0;case 539:
# 3135 "parse.y"
 yyval=Cyc_YY6(11U);goto _LL0;case 540:
# 3136 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 541:
# 3137 "parse.y"
 yyval=Cyc_YY6(0U);goto _LL0;case 542: _T159A=yyyvsp;_T159B=& _T159A[3].v;_T159C=(union Cyc_YYSTYPE*)_T159B;{
# 3142 "parse.y"
struct _tuple31*_T18AB=Cyc_yyget_YY64(_T159C);struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;struct Cyc_List_List*_T18AE;{struct _tuple31 _T18AF=*_T18AB;_T18AE=_T18AF.f0;_T18AD=_T18AF.f1;_T18AC=_T18AF.f2;}{struct Cyc_List_List*outlist=_T18AE;struct Cyc_List_List*inlist=_T18AD;struct Cyc_List_List*clobbers=_T18AC;{struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T18AF=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_T18AF->tag=39;_T159E=yyyvsp;_T159F=& _T159E[0].v;_T15A0=(union Cyc_YYSTYPE*)_T159F;
_T18AF->f1=Cyc_yyget_YY32(_T15A0);_T15A1=yyyvsp;_T15A2=& _T15A1[2].v;_T15A3=(union Cyc_YYSTYPE*)_T15A2;_T18AF->f2=Cyc_yyget_String_tok(_T15A3);_T18AF->f3=outlist;_T18AF->f4=inlist;_T18AF->f5=clobbers;_T159D=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_T18AF;}_T15A4=(void*)_T159D;yyval=Cyc_YY63(_T15A4);goto _LL0;}}case 543:
# 3147 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 544:
# 3148 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 545:{struct _tuple31*_T18AB=_cycalloc(sizeof(struct _tuple31));
# 3152 "parse.y"
_T18AB->f0=0;_T18AB->f1=0;_T18AB->f2=0;_T15A5=(struct _tuple31*)_T18AB;}yyval=Cyc_YY64(_T15A5);goto _LL0;case 546: _T15A6=yyyvsp;_T15A7=& _T15A6[1].v;_T15A8=(union Cyc_YYSTYPE*)_T15A7;{
# 3154 "parse.y"
struct _tuple28*_T18AB=Cyc_yyget_YY65(_T15A8);struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;{struct _tuple28 _T18AE=*_T18AB;_T18AD=_T18AE.f0;_T18AC=_T18AE.f1;}{struct Cyc_List_List*inlist=_T18AD;struct Cyc_List_List*clobbers=_T18AC;{struct _tuple31*_T18AE=_cycalloc(sizeof(struct _tuple31));
_T18AE->f0=0;_T18AE->f1=inlist;_T18AE->f2=clobbers;_T15A9=(struct _tuple31*)_T18AE;}yyval=Cyc_YY64(_T15A9);goto _LL0;}}case 547: _T15AA=yyyvsp;_T15AB=& _T15AA[2].v;_T15AC=(union Cyc_YYSTYPE*)_T15AB;{
# 3157 "parse.y"
struct _tuple28*_T18AB=Cyc_yyget_YY65(_T15AC);struct Cyc_List_List*_T18AC;struct Cyc_List_List*_T18AD;{struct _tuple28 _T18AE=*_T18AB;_T18AD=_T18AE.f0;_T18AC=_T18AE.f1;}{struct Cyc_List_List*inlist=_T18AD;struct Cyc_List_List*clobbers=_T18AC;{struct _tuple31*_T18AE=_cycalloc(sizeof(struct _tuple31));_T15AE=yyyvsp;_T15AF=& _T15AE[1].v;_T15B0=(union Cyc_YYSTYPE*)_T15AF;_T15B1=
Cyc_yyget_YY67(_T15B0);_T18AE->f0=Cyc_List_imp_rev(_T15B1);_T18AE->f1=inlist;_T18AE->f2=clobbers;_T15AD=(struct _tuple31*)_T18AE;}yyval=Cyc_YY64(_T15AD);goto _LL0;}}case 548:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T15B3=yyyvsp;_T15B4=& _T15B3[0].v;_T15B5=(union Cyc_YYSTYPE*)_T15B4;
# 3161
_T18AB->hd=Cyc_yyget_YY68(_T15B5);_T18AB->tl=0;_T15B2=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY67(_T15B2);goto _LL0;case 549:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T15B7=yyyvsp;_T15B8=& _T15B7[2].v;_T15B9=(union Cyc_YYSTYPE*)_T15B8;
# 3162 "parse.y"
_T18AB->hd=Cyc_yyget_YY68(_T15B9);_T15BA=yyyvsp;_T15BB=& _T15BA[0].v;_T15BC=(union Cyc_YYSTYPE*)_T15BB;_T18AB->tl=Cyc_yyget_YY67(_T15BC);_T15B6=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY67(_T15B6);goto _LL0;case 550:{struct _tuple28*_T18AB=_cycalloc(sizeof(struct _tuple28));
# 3166 "parse.y"
_T18AB->f0=0;_T18AB->f1=0;_T15BD=(struct _tuple28*)_T18AB;}yyval=Cyc_YY65(_T15BD);goto _LL0;case 551:{struct _tuple28*_T18AB=_cycalloc(sizeof(struct _tuple28));
# 3167 "parse.y"
_T18AB->f0=0;_T15BF=yyyvsp;_T15C0=& _T15BF[1].v;_T15C1=(union Cyc_YYSTYPE*)_T15C0;_T18AB->f1=Cyc_yyget_YY66(_T15C1);_T15BE=(struct _tuple28*)_T18AB;}yyval=Cyc_YY65(_T15BE);goto _LL0;case 552:{struct _tuple28*_T18AB=_cycalloc(sizeof(struct _tuple28));_T15C3=yyyvsp;_T15C4=& _T15C3[1].v;_T15C5=(union Cyc_YYSTYPE*)_T15C4;_T15C6=
# 3168 "parse.y"
Cyc_yyget_YY67(_T15C5);_T18AB->f0=Cyc_List_imp_rev(_T15C6);_T15C7=yyyvsp;_T15C8=& _T15C7[2].v;_T15C9=(union Cyc_YYSTYPE*)_T15C8;_T18AB->f1=Cyc_yyget_YY66(_T15C9);_T15C2=(struct _tuple28*)_T18AB;}yyval=Cyc_YY65(_T15C2);goto _LL0;case 553:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T15CB=yyyvsp;_T15CC=& _T15CB[0].v;_T15CD=(union Cyc_YYSTYPE*)_T15CC;
# 3171
_T18AB->hd=Cyc_yyget_YY68(_T15CD);_T18AB->tl=0;_T15CA=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY67(_T15CA);goto _LL0;case 554:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T15CF=yyyvsp;_T15D0=& _T15CF[2].v;_T15D1=(union Cyc_YYSTYPE*)_T15D0;
# 3172 "parse.y"
_T18AB->hd=Cyc_yyget_YY68(_T15D1);_T15D2=yyyvsp;_T15D3=& _T15D2[0].v;_T15D4=(union Cyc_YYSTYPE*)_T15D3;_T18AB->tl=Cyc_yyget_YY67(_T15D4);_T15CE=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY67(_T15CE);goto _LL0;case 555:{struct _tuple32*_T18AB=_cycalloc(sizeof(struct _tuple32));_T15D6=yyyvsp;_T15D7=& _T15D6[0].v;_T15D8=(union Cyc_YYSTYPE*)_T15D7;
# 3176 "parse.y"
_T18AB->f0=Cyc_yyget_String_tok(_T15D8);_T15D9=yyyvsp;_T15DA=& _T15D9[2].v;_T15DB=(union Cyc_YYSTYPE*)_T15DA;_T18AB->f1=Cyc_yyget_Exp_tok(_T15DB);_T15D5=(struct _tuple32*)_T18AB;}yyval=Cyc_YY68(_T15D5);goto _LL0;case 556:
# 3181 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 557:
# 3182 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 558: _T15DC=yyyvsp;_T15DD=& _T15DC[1].v;_T15DE=(union Cyc_YYSTYPE*)_T15DD;_T15DF=
# 3183 "parse.y"
Cyc_yyget_YY66(_T15DE);_T15E0=Cyc_List_imp_rev(_T15DF);yyval=Cyc_YY66(_T15E0);goto _LL0;case 559:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T15E3=yyyvsp;_T15E4=& _T15E3[0].v;_T15E5=(union Cyc_YYSTYPE*)_T15E4;
# 3186
*_T18AC=Cyc_yyget_String_tok(_T15E5);_T15E2=(struct _fat_ptr*)_T18AC;}_T18AB->hd=_T15E2;_T18AB->tl=0;_T15E1=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY66(_T15E1);goto _LL0;case 560:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T15E8=yyyvsp;_T15E9=& _T15E8[2].v;_T15EA=(union Cyc_YYSTYPE*)_T15E9;
# 3187 "parse.y"
*_T18AC=Cyc_yyget_String_tok(_T15EA);_T15E7=(struct _fat_ptr*)_T18AC;}_T18AB->hd=_T15E7;_T15EB=yyyvsp;_T15EC=& _T15EB[0].v;_T15ED=(union Cyc_YYSTYPE*)_T15EC;_T18AB->tl=Cyc_yyget_YY66(_T15ED);_T15E6=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY66(_T15E6);goto _LL0;case 561: _T15EE=yyyvsp;_T15EF=_T15EE[0];
# 3192 "parse.y"
yyval=_T15EF.v;goto _LL0;case 562: _T15F0=yyyvsp;_T15F1=& _T15F0[0].v;_T15F2=(union Cyc_YYSTYPE*)_T15F1;_T15F3=
# 3194 "parse.y"
Cyc_yyget_Exp_tok(_T15F2);_T15F4=yyyvsp;_T15F5=& _T15F4[2].v;_T15F6=(union Cyc_YYSTYPE*)_T15F5;_T15F7=Cyc_yyget_Exp_tok(_T15F6);_T15F8=yyyvsp;_T15F9=_T15F8[0];_T15FA=_T15F9.l;_T15FB=_T15FA.first_line;_T15FC=Cyc_Position_loc_to_seg(_T15FB);_T15FD=Cyc_Absyn_subscript_exp(_T15F3,_T15F7,_T15FC);yyval=Cyc_Exp_tok(_T15FD);goto _LL0;case 563: _T15FE=yyyvsp;_T15FF=& _T15FE[0].v;_T1600=(union Cyc_YYSTYPE*)_T15FF;_T1601=
# 3196 "parse.y"
Cyc_yyget_Exp_tok(_T1600);_T1602=yyyvsp;_T1603=_T1602[0];_T1604=_T1603.l;_T1605=_T1604.first_line;_T1606=Cyc_Position_loc_to_seg(_T1605);_T1607=Cyc_Absyn_unknowncall_exp(_T1601,0,_T1606);yyval=Cyc_Exp_tok(_T1607);goto _LL0;case 564: _T1608=yyyvsp;_T1609=& _T1608[0].v;_T160A=(union Cyc_YYSTYPE*)_T1609;_T160B=
# 3198 "parse.y"
Cyc_yyget_Exp_tok(_T160A);_T160C=yyyvsp;_T160D=& _T160C[2].v;_T160E=(union Cyc_YYSTYPE*)_T160D;_T160F=Cyc_yyget_YY4(_T160E);_T1610=yyyvsp;_T1611=_T1610[0];_T1612=_T1611.l;_T1613=_T1612.first_line;_T1614=Cyc_Position_loc_to_seg(_T1613);_T1615=Cyc_Absyn_unknowncall_exp(_T160B,_T160F,_T1614);yyval=Cyc_Exp_tok(_T1615);goto _LL0;case 565: _T1616=yyyvsp;_T1617=& _T1616[0].v;_T1618=(union Cyc_YYSTYPE*)_T1617;_T1619=
# 3200 "parse.y"
Cyc_yyget_Exp_tok(_T1618);{struct _fat_ptr*_T18AB=_cycalloc(sizeof(struct _fat_ptr));_T161B=yyyvsp;_T161C=& _T161B[2].v;_T161D=(union Cyc_YYSTYPE*)_T161C;*_T18AB=Cyc_yyget_String_tok(_T161D);_T161A=(struct _fat_ptr*)_T18AB;}_T161E=yyyvsp;_T161F=_T161E[0];_T1620=_T161F.l;_T1621=_T1620.first_line;_T1622=Cyc_Position_loc_to_seg(_T1621);_T1623=Cyc_Absyn_aggrmember_exp(_T1619,_T161A,_T1622);yyval=Cyc_Exp_tok(_T1623);goto _LL0;case 566: _T1624=yyyvsp;_T1625=& _T1624[0].v;_T1626=(union Cyc_YYSTYPE*)_T1625;_T1627=
# 3202 "parse.y"
Cyc_yyget_Exp_tok(_T1626);{struct _fat_ptr*_T18AB=_cycalloc(sizeof(struct _fat_ptr));_T1629=yyyvsp;_T162A=& _T1629[2].v;_T162B=(union Cyc_YYSTYPE*)_T162A;*_T18AB=Cyc_yyget_String_tok(_T162B);_T1628=(struct _fat_ptr*)_T18AB;}_T162C=yyyvsp;_T162D=_T162C[0];_T162E=_T162D.l;_T162F=_T162E.first_line;_T1630=Cyc_Position_loc_to_seg(_T162F);_T1631=Cyc_Absyn_aggrarrow_exp(_T1627,_T1628,_T1630);yyval=Cyc_Exp_tok(_T1631);goto _LL0;case 567: _T1632=yyyvsp;_T1633=& _T1632[0].v;_T1634=(union Cyc_YYSTYPE*)_T1633;_T1635=
# 3204 "parse.y"
Cyc_yyget_Exp_tok(_T1634);_T1636=yyyvsp;_T1637=_T1636[0];_T1638=_T1637.l;_T1639=_T1638.first_line;_T163A=Cyc_Position_loc_to_seg(_T1639);_T163B=Cyc_Absyn_increment_exp(_T1635,1U,_T163A);yyval=Cyc_Exp_tok(_T163B);goto _LL0;case 568: _T163C=yyyvsp;_T163D=& _T163C[0].v;_T163E=(union Cyc_YYSTYPE*)_T163D;_T163F=
# 3206 "parse.y"
Cyc_yyget_Exp_tok(_T163E);_T1640=yyyvsp;_T1641=_T1640[0];_T1642=_T1641.l;_T1643=_T1642.first_line;_T1644=Cyc_Position_loc_to_seg(_T1643);_T1645=Cyc_Absyn_increment_exp(_T163F,3U,_T1644);yyval=Cyc_Exp_tok(_T1645);goto _LL0;case 569:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T18AB->tag=24;_T1647=yyyvsp;_T1648=& _T1647[1].v;_T1649=(union Cyc_YYSTYPE*)_T1648;
# 3208 "parse.y"
_T18AB->f1=Cyc_yyget_YY38(_T1649);_T18AB->f2=0;_T1646=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T18AB;}_T164A=(void*)_T1646;_T164B=yyyvsp;_T164C=_T164B[0];_T164D=_T164C.l;_T164E=_T164D.first_line;_T164F=Cyc_Position_loc_to_seg(_T164E);_T1650=Cyc_Absyn_new_exp(_T164A,_T164F);yyval=Cyc_Exp_tok(_T1650);goto _LL0;case 570:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T18AB->tag=24;_T1652=yyyvsp;_T1653=& _T1652[1].v;_T1654=(union Cyc_YYSTYPE*)_T1653;
# 3210 "parse.y"
_T18AB->f1=Cyc_yyget_YY38(_T1654);_T1655=yyyvsp;_T1656=& _T1655[4].v;_T1657=(union Cyc_YYSTYPE*)_T1656;_T1658=Cyc_yyget_YY5(_T1657);_T18AB->f2=Cyc_List_imp_rev(_T1658);_T1651=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T18AB;}_T1659=(void*)_T1651;_T165A=yyyvsp;_T165B=_T165A[0];_T165C=_T165B.l;_T165D=_T165C.first_line;_T165E=Cyc_Position_loc_to_seg(_T165D);_T165F=Cyc_Absyn_new_exp(_T1659,_T165E);yyval=Cyc_Exp_tok(_T165F);goto _LL0;case 571:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T18AB->tag=24;_T1661=yyyvsp;_T1662=& _T1661[1].v;_T1663=(union Cyc_YYSTYPE*)_T1662;
# 3212 "parse.y"
_T18AB->f1=Cyc_yyget_YY38(_T1663);_T1664=yyyvsp;_T1665=& _T1664[4].v;_T1666=(union Cyc_YYSTYPE*)_T1665;_T1667=Cyc_yyget_YY5(_T1666);_T18AB->f2=Cyc_List_imp_rev(_T1667);_T1660=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T18AB;}_T1668=(void*)_T1660;_T1669=yyyvsp;_T166A=_T1669[0];_T166B=_T166A.l;_T166C=_T166B.first_line;_T166D=Cyc_Position_loc_to_seg(_T166C);_T166E=Cyc_Absyn_new_exp(_T1668,_T166D);yyval=Cyc_Exp_tok(_T166E);goto _LL0;case 572:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T18AC->tag=0;{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));_T1672=yyyvsp;_T1673=& _T1672[0].v;_T1674=(union Cyc_YYSTYPE*)_T1673;
# 3217 "parse.y"
*_T18AD=Cyc_yyget_String_tok(_T1674);_T1671=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T1671;_T1670=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T18AC;}_T18AB->hd=(void*)_T1670;_T18AB->tl=0;_T166F=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY3(_T166F);goto _LL0;case 573: _T1675=yyyvsp;_T1676=_T1675[0];_T1677=_T1676.l;_T1678=_T1677.first_line;_T1679=
# 3220
Cyc_Position_loc_to_seg(_T1678);_T167A=yyyvsp;_T167B=& _T167A[0].v;_T167C=(union Cyc_YYSTYPE*)_T167B;_T167D=Cyc_yyget_Int_tok(_T167C);{unsigned i=Cyc_Parse_cnst2uint(_T1679,_T167D);_T167E=i;_T167F=(int)_T167E;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T18AB=Cyc_Absyn_tuple_field_designator(_T167F);struct _fat_ptr*_T18AC;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T18AD=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T18AB;_T18AC=_T18AD->f1;}{struct _fat_ptr*f=_T18AC;{struct Cyc_List_List*_T18AD=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T18AE->tag=0;
_T18AE->f1=f;_T1681=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T18AE;}_T18AD->hd=(void*)_T1681;_T18AD->tl=0;_T1680=(struct Cyc_List_List*)_T18AD;}yyval=Cyc_YY3(_T1680);goto _LL0;}}}case 574:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T18AC=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T18AC->tag=0;{struct _fat_ptr*_T18AD=_cycalloc(sizeof(struct _fat_ptr));_T1685=yyyvsp;_T1686=& _T1685[2].v;_T1687=(union Cyc_YYSTYPE*)_T1686;
# 3224 "parse.y"
*_T18AD=Cyc_yyget_String_tok(_T1687);_T1684=(struct _fat_ptr*)_T18AD;}_T18AC->f1=_T1684;_T1683=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T18AC;}_T18AB->hd=(void*)_T1683;_T1688=yyyvsp;_T1689=& _T1688[0].v;_T168A=(union Cyc_YYSTYPE*)_T1689;_T18AB->tl=Cyc_yyget_YY3(_T168A);_T1682=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY3(_T1682);goto _LL0;case 575: _T168B=yyyvsp;_T168C=_T168B[2];_T168D=_T168C.l;_T168E=_T168D.first_line;_T168F=
# 3227
Cyc_Position_loc_to_seg(_T168E);_T1690=yyyvsp;_T1691=& _T1690[2].v;_T1692=(union Cyc_YYSTYPE*)_T1691;_T1693=Cyc_yyget_Int_tok(_T1692);{unsigned i=Cyc_Parse_cnst2uint(_T168F,_T1693);_T1694=i;_T1695=(int)_T1694;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T18AB=Cyc_Absyn_tuple_field_designator(_T1695);struct _fat_ptr*_T18AC;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T18AD=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T18AB;_T18AC=_T18AD->f1;}{struct _fat_ptr*f=_T18AC;{struct Cyc_List_List*_T18AD=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T18AE=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T18AE->tag=0;
_T18AE->f1=f;_T1697=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T18AE;}_T18AD->hd=(void*)_T1697;_T1698=yyyvsp;_T1699=& _T1698[0].v;_T169A=(union Cyc_YYSTYPE*)_T1699;_T18AD->tl=Cyc_yyget_YY3(_T169A);_T1696=(struct Cyc_List_List*)_T18AD;}yyval=Cyc_YY3(_T1696);goto _LL0;}}}case 576: _T169B=yyyvsp;_T169C=& _T169B[0].v;_T169D=(union Cyc_YYSTYPE*)_T169C;_T169E=
# 3234 "parse.y"
Cyc_yyget_QualId_tok(_T169D);_T169F=yyyvsp;_T16A0=_T169F[0];_T16A1=_T16A0.l;_T16A2=_T16A1.first_line;_T16A3=Cyc_Position_loc_to_seg(_T16A2);_T16A4=Cyc_Absyn_unknownid_exp(_T169E,_T16A3);yyval=Cyc_Exp_tok(_T16A4);goto _LL0;case 577: _T16A5=yyyvsp;_T16A6=& _T16A5[2].v;_T16A7=(union Cyc_YYSTYPE*)_T16A6;_T16A8=
# 3235 "parse.y"
Cyc_yyget_String_tok(_T16A7);_T16A9=yyyvsp;_T16AA=_T16A9[0];_T16AB=_T16AA.l;_T16AC=_T16AB.first_line;_T16AD=Cyc_Position_loc_to_seg(_T16AC);_T16AE=Cyc_Absyn_pragma_exp(_T16A8,_T16AD);yyval=Cyc_Exp_tok(_T16AE);goto _LL0;case 578: _T16AF=yyyvsp;_T16B0=_T16AF[0];
# 3236 "parse.y"
yyval=_T16B0.v;goto _LL0;case 579: _T16B1=yyyvsp;_T16B2=& _T16B1[0].v;_T16B3=(union Cyc_YYSTYPE*)_T16B2;_T16B4=
# 3237 "parse.y"
Cyc_yyget_String_tok(_T16B3);_T16B5=yyyvsp;_T16B6=_T16B5[0];_T16B7=_T16B6.l;_T16B8=_T16B7.first_line;_T16B9=Cyc_Position_loc_to_seg(_T16B8);_T16BA=Cyc_Absyn_string_exp(_T16B4,_T16B9);yyval=Cyc_Exp_tok(_T16BA);goto _LL0;case 580: _T16BB=yyyvsp;_T16BC=& _T16BB[0].v;_T16BD=(union Cyc_YYSTYPE*)_T16BC;_T16BE=
# 3238 "parse.y"
Cyc_yyget_String_tok(_T16BD);_T16BF=yyyvsp;_T16C0=_T16BF[0];_T16C1=_T16C0.l;_T16C2=_T16C1.first_line;_T16C3=Cyc_Position_loc_to_seg(_T16C2);_T16C4=Cyc_Absyn_wstring_exp(_T16BE,_T16C3);yyval=Cyc_Exp_tok(_T16C4);goto _LL0;case 581: _T16C5=yyyvsp;_T16C6=_T16C5[1];
# 3239 "parse.y"
yyval=_T16C6.v;goto _LL0;case 582: _T16C7=yyyvsp;_T16C8=& _T16C7[0].v;_T16C9=(union Cyc_YYSTYPE*)_T16C8;_T16CA=
# 3243 "parse.y"
Cyc_yyget_Exp_tok(_T16C9);_T16CB=yyyvsp;_T16CC=_T16CB[0];_T16CD=_T16CC.l;_T16CE=_T16CD.first_line;_T16CF=Cyc_Position_loc_to_seg(_T16CE);_T16D0=Cyc_Absyn_noinstantiate_exp(_T16CA,_T16CF);yyval=Cyc_Exp_tok(_T16D0);goto _LL0;case 583: _T16D1=yyyvsp;_T16D2=& _T16D1[0].v;_T16D3=(union Cyc_YYSTYPE*)_T16D2;_T16D4=
# 3246
Cyc_yyget_Exp_tok(_T16D3);_T16D5=yyyvsp;_T16D6=& _T16D5[3].v;_T16D7=(union Cyc_YYSTYPE*)_T16D6;_T16D8=Cyc_yyget_YY41(_T16D7);_T16D9=Cyc_List_imp_rev(_T16D8);_T16DA=yyyvsp;_T16DB=_T16DA[0];_T16DC=_T16DB.l;_T16DD=_T16DC.first_line;_T16DE=Cyc_Position_loc_to_seg(_T16DD);_T16DF=Cyc_Absyn_instantiate_exp(_T16D4,_T16D9,_T16DE);yyval=Cyc_Exp_tok(_T16DF);goto _LL0;case 584:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T18AB=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T18AB->tag=28;_T16E1=yyyvsp;_T16E2=& _T16E1[0].v;_T16E3=(union Cyc_YYSTYPE*)_T16E2;
# 3249
_T18AB->f1=Cyc_yyget_QualId_tok(_T16E3);_T16E4=yyyvsp;_T16E5=& _T16E4[2].v;_T16E6=(union Cyc_YYSTYPE*)_T16E5;_T18AB->f2=Cyc_yyget_YY41(_T16E6);_T16E7=yyyvsp;_T16E8=& _T16E7[3].v;_T16E9=(union Cyc_YYSTYPE*)_T16E8;_T16EA=Cyc_yyget_YY5(_T16E9);_T18AB->f3=Cyc_List_imp_rev(_T16EA);_T18AB->f4=0;_T16E0=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T18AB;}_T16EB=(void*)_T16E0;_T16EC=yyyvsp;_T16ED=_T16EC[0];_T16EE=_T16ED.l;_T16EF=_T16EE.first_line;_T16F0=Cyc_Position_loc_to_seg(_T16EF);_T16F1=Cyc_Absyn_new_exp(_T16EB,_T16F0);yyval=Cyc_Exp_tok(_T16F1);goto _LL0;case 585: _T16F2=yyyvsp;_T16F3=& _T16F2[2].v;_T16F4=(union Cyc_YYSTYPE*)_T16F3;_T16F5=
# 3251 "parse.y"
Cyc_yyget_YY4(_T16F4);_T16F6=yyyvsp;_T16F7=_T16F6[0];_T16F8=_T16F7.l;_T16F9=_T16F8.first_line;_T16FA=Cyc_Position_loc_to_seg(_T16F9);_T16FB=Cyc_Absyn_tuple_exp(_T16F5,_T16FA);yyval=Cyc_Exp_tok(_T16FB);goto _LL0;case 586: _T16FC=yyyvsp;_T16FD=& _T16FC[2].v;_T16FE=(union Cyc_YYSTYPE*)_T16FD;_T16FF=
# 3253 "parse.y"
Cyc_yyget_Stmt_tok(_T16FE);_T1700=yyyvsp;_T1701=_T1700[0];_T1702=_T1701.l;_T1703=_T1702.first_line;_T1704=Cyc_Position_loc_to_seg(_T1703);_T1705=Cyc_Absyn_stmt_exp(_T16FF,_T1704);yyval=Cyc_Exp_tok(_T1705);goto _LL0;case 587: _T1706=yyyvsp;_T1707=& _T1706[0].v;_T1708=(union Cyc_YYSTYPE*)_T1707;_T1709=
# 3257 "parse.y"
Cyc_yyget_YY4(_T1708);_T170A=Cyc_List_imp_rev(_T1709);yyval=Cyc_YY4(_T170A);goto _LL0;case 588:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T170C=yyyvsp;_T170D=& _T170C[0].v;_T170E=(union Cyc_YYSTYPE*)_T170D;
# 3262 "parse.y"
_T18AB->hd=Cyc_yyget_Exp_tok(_T170E);_T18AB->tl=0;_T170B=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY4(_T170B);goto _LL0;case 589:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T1710=yyyvsp;_T1711=& _T1710[2].v;_T1712=(union Cyc_YYSTYPE*)_T1711;
# 3264 "parse.y"
_T18AB->hd=Cyc_yyget_Exp_tok(_T1712);_T1713=yyyvsp;_T1714=& _T1713[0].v;_T1715=(union Cyc_YYSTYPE*)_T1714;_T18AB->tl=Cyc_yyget_YY4(_T1715);_T170F=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY4(_T170F);goto _LL0;case 590: _T1716=yyyvsp;_T1717=& _T1716[0].v;_T1718=(union Cyc_YYSTYPE*)_T1717;_T1719=
# 3270 "parse.y"
Cyc_yyget_Int_tok(_T1718);_T171A=yyyvsp;_T171B=_T171A[0];_T171C=_T171B.l;_T171D=_T171C.first_line;_T171E=Cyc_Position_loc_to_seg(_T171D);_T171F=Cyc_Absyn_const_exp(_T1719,_T171E);yyval=Cyc_Exp_tok(_T171F);goto _LL0;case 591: _T1720=yyyvsp;_T1721=& _T1720[0].v;_T1722=(union Cyc_YYSTYPE*)_T1721;_T1723=
# 3271 "parse.y"
Cyc_yyget_Char_tok(_T1722);_T1724=yyyvsp;_T1725=_T1724[0];_T1726=_T1725.l;_T1727=_T1726.first_line;_T1728=Cyc_Position_loc_to_seg(_T1727);_T1729=Cyc_Absyn_char_exp(_T1723,_T1728);yyval=Cyc_Exp_tok(_T1729);goto _LL0;case 592: _T172A=yyyvsp;_T172B=& _T172A[0].v;_T172C=(union Cyc_YYSTYPE*)_T172B;_T172D=
# 3272 "parse.y"
Cyc_yyget_String_tok(_T172C);_T172E=yyyvsp;_T172F=_T172E[0];_T1730=_T172F.l;_T1731=_T1730.first_line;_T1732=Cyc_Position_loc_to_seg(_T1731);_T1733=Cyc_Absyn_wchar_exp(_T172D,_T1732);yyval=Cyc_Exp_tok(_T1733);goto _LL0;case 593: _T1734=yyyvsp;_T1735=_T1734[0];_T1736=_T1735.l;_T1737=_T1736.first_line;_T1738=
# 3274 "parse.y"
Cyc_Position_loc_to_seg(_T1737);_T1739=Cyc_Absyn_null_exp(_T1738);yyval=Cyc_Exp_tok(_T1739);goto _LL0;case 594: _T173A=yyyvsp;_T173B=& _T173A[0].v;_T173C=(union Cyc_YYSTYPE*)_T173B;{
# 3276 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(_T173C);_T173D=
Cyc_strlen(f);{int l=(int)_T173D;
int i=1;
if(l <= 0)goto _TL2F3;_T173E=f;_T173F=_T173E.curr;_T1740=(const char*)_T173F;_T1741=
_check_null(_T1740);_T1742=l - 1;{char c=_T1741[_T1742];_T1743=c;_T1744=(int)_T1743;
if(_T1744==102)goto _TL2F7;else{goto _TL2F8;}_TL2F8: _T1745=c;_T1746=(int)_T1745;if(_T1746==70)goto _TL2F7;else{goto _TL2F5;}_TL2F7: i=0;goto _TL2F6;
_TL2F5: _T1747=c;_T1748=(int)_T1747;if(_T1748==108)goto _TL2FB;else{goto _TL2FC;}_TL2FC: _T1749=c;_T174A=(int)_T1749;if(_T174A==76)goto _TL2FB;else{goto _TL2F9;}_TL2FB: i=2;goto _TL2FA;_TL2F9: _TL2FA: _TL2F6:;}goto _TL2F4;_TL2F3: _TL2F4: _T174B=f;_T174C=i;_T174D=yyyvsp;_T174E=_T174D[0];_T174F=_T174E.l;_T1750=_T174F.first_line;_T1751=
# 3284
Cyc_Position_loc_to_seg(_T1750);_T1752=Cyc_Absyn_float_exp(_T174B,_T174C,_T1751);yyval=Cyc_Exp_tok(_T1752);goto _LL0;}}case 595:{struct _tuple0*_T18AB=_cycalloc(sizeof(struct _tuple0));
# 3289 "parse.y"
_T18AB->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T1755=yyyvsp;_T1756=& _T1755[0].v;_T1757=(union Cyc_YYSTYPE*)_T1756;*_T18AC=Cyc_yyget_String_tok(_T1757);_T1754=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_T1754;_T1753=(struct _tuple0*)_T18AB;}yyval=Cyc_QualId_tok(_T1753);goto _LL0;case 596: _T1758=yyyvsp;_T1759=_T1758[0];
# 3290 "parse.y"
yyval=_T1759.v;goto _LL0;case 597:{struct _tuple0*_T18AB=_cycalloc(sizeof(struct _tuple0));
# 3293
_T18AB->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T18AC=_cycalloc(sizeof(struct _fat_ptr));_T175C=yyyvsp;_T175D=& _T175C[0].v;_T175E=(union Cyc_YYSTYPE*)_T175D;*_T18AC=Cyc_yyget_String_tok(_T175E);_T175B=(struct _fat_ptr*)_T18AC;}_T18AB->f1=_T175B;_T175A=(struct _tuple0*)_T18AB;}yyval=Cyc_QualId_tok(_T175A);goto _LL0;case 598: _T175F=yyyvsp;_T1760=_T175F[0];
# 3294 "parse.y"
yyval=_T1760.v;goto _LL0;case 599: _T1761=yyyvsp;_T1762=_T1761[0];
# 3299 "parse.y"
yyval=_T1762.v;goto _LL0;case 600: _T1763=yyyvsp;_T1764=_T1763[0];
# 3300 "parse.y"
yyval=_T1764.v;goto _LL0;case 601: _T1765=yyyvsp;_T1766=_T1765[0];
# 3303
yyval=_T1766.v;goto _LL0;case 602: _T1767=yyyvsp;_T1768=_T1767[0];
# 3304 "parse.y"
yyval=_T1768.v;goto _LL0;case 603: goto _LL0;case 604: _T1769=yylex_buf;
# 3309 "parse.y"
_T1769->lex_curr_pos=_T1769->lex_curr_pos - 1;goto _LL0;case 605:
# 3313 "parse.y"
 yyval=Cyc_YY71(0);goto _LL0;case 606:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple35*_T18AC=_cycalloc(sizeof(struct _tuple35));_T176C=yyyvsp;_T176D=& _T176C[1].v;_T176E=(union Cyc_YYSTYPE*)_T176D;_T176F=
# 3315 "parse.y"
Cyc_yyget_String_tok(_T176E);_T1770=yyyvsp;_T1771=& _T1770[3].v;_T1772=(union Cyc_YYSTYPE*)_T1771;_T1773=Cyc_yyget_String_tok(_T1772);_T1774=_tag_fat("true",sizeof(char),5U);_T1775=Cyc_strcmp(_T1773,_T1774);_T1776=_T1775==0;_T1777=yyyvsp;_T1778=& _T1777[0].v;_T1779=(union Cyc_YYSTYPE*)_T1778;_T177A=Cyc_yyget_String_tok(_T1779);_T177B=Cyc_Parse_typevar2cvar(_T177A);_T18AC->f0=Cyc_Parse_assign_cvar_pos(_T176F,_T1776,_T177B);_T177C=yyyvsp;_T177D=& _T177C[5].v;_T177E=(union Cyc_YYSTYPE*)_T177D;_T18AC->f1=Cyc_yyget_YY72(_T177E);_T176B=(struct _tuple35*)_T18AC;}_T18AB->hd=_T176B;_T177F=yyyvsp;_T1780=& _T177F[7].v;_T1781=(union Cyc_YYSTYPE*)_T1780;_T18AB->tl=Cyc_yyget_YY71(_T1781);_T176A=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY71(_T176A);goto _LL0;case 607:
# 3319 "parse.y"
 yyval=Cyc_YY72(0);goto _LL0;case 608: _T1782=yyyvsp;_T1783=& _T1782[0].v;_T1784=(union Cyc_YYSTYPE*)_T1783;_T1785=
# 3320 "parse.y"
Cyc_yyget_YY72(_T1784);yyval=Cyc_YY72(_T1785);goto _LL0;case 609:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T1787=yyyvsp;_T1788=& _T1787[0].v;_T1789=(union Cyc_YYSTYPE*)_T1788;
# 3324 "parse.y"
_T18AB->hd=Cyc_yyget_YY73(_T1789);_T18AB->tl=0;_T1786=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY72(_T1786);goto _LL0;case 610:{struct Cyc_List_List*_T18AB=_cycalloc(sizeof(struct Cyc_List_List));_T178B=yyyvsp;_T178C=& _T178B[0].v;_T178D=(union Cyc_YYSTYPE*)_T178C;
# 3325 "parse.y"
_T18AB->hd=Cyc_yyget_YY73(_T178D);_T178E=yyyvsp;_T178F=& _T178E[2].v;_T1790=(union Cyc_YYSTYPE*)_T178F;_T18AB->tl=Cyc_yyget_YY72(_T1790);_T178A=(struct Cyc_List_List*)_T18AB;}yyval=Cyc_YY72(_T178A);goto _LL0;case 611: _T1791=yyyvsp;_T1792=& _T1791[0].v;_T1793=(union Cyc_YYSTYPE*)_T1792;_T1794=
# 3329 "parse.y"
Cyc_yyget_String_tok(_T1793);_T1795=Cyc_Parse_typevar2cvar(_T1794);yyval=Cyc_YY45(_T1795);goto _LL0;case 612: _T1796=yyyvsp;_T1797=_T1796[0];_T1798=_T1797.l;_T1799=_T1798.first_line;_T179A=
# 3330 "parse.y"
Cyc_Position_loc_to_seg(_T1799);_T179B=yyyvsp;_T179C=& _T179B[0].v;_T179D=(union Cyc_YYSTYPE*)_T179C;_T179E=Cyc_yyget_String_tok(_T179D);_T179F=Cyc_Parse_str2type(_T179A,_T179E);yyval=Cyc_YY45(_T179F);goto _LL0;case 613: _T17A0=yyyvsp;_T17A1=& _T17A0[0].v;_T17A2=(union Cyc_YYSTYPE*)_T17A1;_T17A3=
# 3334 "parse.y"
Cyc_yyget_String_tok(_T17A2);_T17A4=yyyvsp;_T17A5=& _T17A4[3].v;_T17A6=(union Cyc_YYSTYPE*)_T17A5;_T17A7=Cyc_yyget_YY73(_T17A6);_T17A8=Cyc_BansheeIf_check_constraint(_T17A7);_T17A9=Cyc_BansheeIf_add_location(_T17A3,_T17A8);yyval=Cyc_YY73(_T17A9);goto _LL0;case 614: _T17AA=yyyvsp;_T17AB=& _T17AA[0].v;_T17AC=(union Cyc_YYSTYPE*)_T17AB;_T17AD=
# 3335 "parse.y"
Cyc_yyget_String_tok(_T17AC);_T17AE=yyyvsp;_T17AF=& _T17AE[3].v;_T17B0=(union Cyc_YYSTYPE*)_T17AF;_T17B1=Cyc_yyget_YY70(_T17B0);_T17B2=yyyvsp;_T17B3=& _T17B2[5].v;_T17B4=(union Cyc_YYSTYPE*)_T17B3;_T17B5=Cyc_yyget_YY45(_T17B4);_T17B6=yyyvsp;_T17B7=& _T17B6[7].v;_T17B8=(union Cyc_YYSTYPE*)_T17B7;_T17B9=Cyc_yyget_YY45(_T17B8);_T17BA=Cyc_Parse_comparison_constraint(_T17B1,_T17B5,_T17B9);_T17BB=Cyc_BansheeIf_add_location(_T17AD,_T17BA);yyval=Cyc_YY73(_T17BB);goto _LL0;case 615: _T17BC=yyyvsp;_T17BD=& _T17BC[0].v;_T17BE=(union Cyc_YYSTYPE*)_T17BD;_T17BF=
# 3336 "parse.y"
Cyc_yyget_String_tok(_T17BE);_T17C0=yyyvsp;_T17C1=& _T17C0[3].v;_T17C2=(union Cyc_YYSTYPE*)_T17C1;_T17C3=Cyc_yyget_YY45(_T17C2);_T17C4=yyyvsp;_T17C5=& _T17C4[5].v;_T17C6=(union Cyc_YYSTYPE*)_T17C5;_T17C7=Cyc_yyget_YY45(_T17C6);_T17C8=Cyc_BansheeIf_cond_equality_constraint(_T17C3,_T17C7);_T17C9=Cyc_BansheeIf_add_location(_T17BF,_T17C8);yyval=Cyc_YY73(_T17C9);goto _LL0;case 616: _T17CA=yyyvsp;_T17CB=& _T17CA[0].v;_T17CC=(union Cyc_YYSTYPE*)_T17CB;_T17CD=
# 3337 "parse.y"
Cyc_yyget_String_tok(_T17CC);_T17CE=yyyvsp;_T17CF=& _T17CE[3].v;_T17D0=(union Cyc_YYSTYPE*)_T17CF;_T17D1=Cyc_yyget_YY45(_T17D0);_T17D2=yyyvsp;_T17D3=& _T17D2[5].v;_T17D4=(union Cyc_YYSTYPE*)_T17D3;_T17D5=Cyc_yyget_YY45(_T17D4);_T17D6=Cyc_BansheeIf_equality_constraint(_T17D1,_T17D5);_T17D7=Cyc_BansheeIf_add_location(_T17CD,_T17D6);yyval=Cyc_YY73(_T17D7);goto _LL0;case 617: _T17D8=yyyvsp;_T17D9=& _T17D8[0].v;_T17DA=(union Cyc_YYSTYPE*)_T17D9;_T17DB=
# 3338 "parse.y"
Cyc_yyget_String_tok(_T17DA);_T17DC=yyyvsp;_T17DD=& _T17DC[3].v;_T17DE=(union Cyc_YYSTYPE*)_T17DD;_T17DF=Cyc_yyget_YY45(_T17DE);_T17E0=yyyvsp;_T17E1=& _T17E0[5].v;_T17E2=(union Cyc_YYSTYPE*)_T17E1;_T17E3=Cyc_yyget_YY45(_T17E2);_T17E4=Cyc_BansheeIf_inclusion_constraint(_T17DF,_T17E3);_T17E5=Cyc_BansheeIf_add_location(_T17DB,_T17E4);yyval=Cyc_YY73(_T17E5);goto _LL0;case 618: _T17E6=yyyvsp;_T17E7=& _T17E6[0].v;_T17E8=(union Cyc_YYSTYPE*)_T17E7;_T17E9=
# 3339 "parse.y"
Cyc_yyget_String_tok(_T17E8);_T17EA=yyyvsp;_T17EB=& _T17EA[3].v;_T17EC=(union Cyc_YYSTYPE*)_T17EB;_T17ED=Cyc_yyget_YY73(_T17EC);_T17EE=yyyvsp;_T17EF=& _T17EE[5].v;_T17F0=(union Cyc_YYSTYPE*)_T17EF;_T17F1=Cyc_yyget_YY73(_T17F0);_T17F2=Cyc_BansheeIf_implication_constraint(_T17ED,_T17F1);_T17F3=Cyc_BansheeIf_add_location(_T17E9,_T17F2);yyval=Cyc_YY73(_T17F3);goto _LL0;case 619: _T17F4=yyyvsp;_T17F5=& _T17F4[0].v;_T17F6=(union Cyc_YYSTYPE*)_T17F5;_T17F7=
# 3340 "parse.y"
Cyc_yyget_String_tok(_T17F6);_T17F8=yyyvsp;_T17F9=& _T17F8[3].v;_T17FA=(union Cyc_YYSTYPE*)_T17F9;_T17FB=Cyc_yyget_YY70(_T17FA);_T17FC=yyyvsp;_T17FD=& _T17FC[5].v;_T17FE=(union Cyc_YYSTYPE*)_T17FD;_T17FF=Cyc_yyget_YY73(_T17FE);_T1800=yyyvsp;_T1801=& _T1800[7].v;_T1802=(union Cyc_YYSTYPE*)_T1801;_T1803=Cyc_yyget_YY73(_T1802);_T1804=Cyc_Parse_composite_constraint(_T17FB,_T17FF,_T1803);_T1805=Cyc_BansheeIf_add_location(_T17F7,_T1804);yyval=Cyc_YY73(_T1805);goto _LL0;case 620:
# 3343
 yyval=Cyc_YY70(0U);goto _LL0;case 621:
# 3344 "parse.y"
 yyval=Cyc_YY70(1U);goto _LL0;case 622:
# 3345 "parse.y"
 yyval=Cyc_YY70(2U);goto _LL0;case 623:
# 3346 "parse.y"
 yyval=Cyc_YY70(3U);goto _LL0;case 624:
# 3347 "parse.y"
 yyval=Cyc_YY70(4U);goto _LL0;default: goto _LL0;}_LL0: _T1806=yylen;
# 375 "cycbison.simple"
yyvsp_offset=yyvsp_offset - _T1806;_T1807=yylen;
yyssp_offset=yyssp_offset - _T1807;_T1808=yyvs;
# 389 "cycbison.simple"
yyvsp_offset=yyvsp_offset + 1;_T1809=yyvsp_offset;_T180A=_check_fat_subscript(_T1808,sizeof(struct Cyc_Yystacktype),_T1809);_T180B=(struct Cyc_Yystacktype*)_T180A;(*_T180B).v=yyval;
# 392
if(yylen!=0)goto _TL2FD;_T180C=yyvs;_T180D=yyvsp_offset - 1;_T180E=
_fat_ptr_plus(_T180C,sizeof(struct Cyc_Yystacktype),_T180D);_T180F=_untag_fat_ptr_check_bound(_T180E,sizeof(struct Cyc_Yystacktype),2U);_T1810=_check_null(_T180F);{struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_T1810;_T1811=p;_T1812=yylloc;
_T1811[1].l.first_line=_T1812.first_line;_T1813=p;_T1814=yylloc;
_T1813[1].l.first_column=_T1814.first_column;_T1815=p;_T1816=p;_T1817=_T1816[0];_T1818=_T1817.l;
_T1815[1].l.last_line=_T1818.last_line;_T1819=p;_T181A=p;_T181B=_T181A[0];_T181C=_T181B.l;
_T1819[1].l.last_column=_T181C.last_column;}goto _TL2FE;
# 399
_TL2FD: _T181D=yyvs;_T181E=_T181D.curr;_T181F=(struct Cyc_Yystacktype*)_T181E;_T1820=yyvsp_offset;_T1821=yyvs;_T1822=yyvsp_offset + yylen;_T1823=_T1822 - 1;_T1824=_check_fat_subscript(_T1821,sizeof(struct Cyc_Yystacktype),_T1823);_T1825=(struct Cyc_Yystacktype*)_T1824;_T1826=*_T1825;_T1827=_T1826.l;_T181F[_T1820].l.last_line=_T1827.last_line;_T1828=yyvs;_T1829=_T1828.curr;_T182A=(struct Cyc_Yystacktype*)_T1829;_T182B=yyvsp_offset;_T182C=yyvs;_T182D=_T182C.curr;_T182E=(struct Cyc_Yystacktype*)_T182D;_T182F=yyvsp_offset + yylen;_T1830=_T182F - 1;_T1831=_T182E[_T1830];_T1832=_T1831.l;
_T182A[_T182B].l.last_column=_T1832.last_column;_TL2FE: _T1833=Cyc_yyr1;_T1834=yyn;_T1835=_check_known_subscript_notnull(_T1833,625U,sizeof(short),_T1834);_T1836=(short*)_T1835;_T1837=*_T1836;
# 409
yyn=(int)_T1837;_T1838=Cyc_yypgoto;_T1839=yyn - 176;_T183A=_check_known_subscript_notnull(_T1838,188U,sizeof(short),_T1839);_T183B=(short*)_T183A;_T183C=*_T183B;_T183D=(int)_T183C;_T183E=yyss;_T183F=yyssp_offset;_T1840=_check_fat_subscript(_T183E,sizeof(short),_T183F);_T1841=(short*)_T1840;_T1842=*_T1841;_T1843=(int)_T1842;
# 411
yystate=_T183D + _T1843;
if(yystate < 0)goto _TL2FF;if(yystate > 8463)goto _TL2FF;_T1844=Cyc_yycheck;_T1845=yystate;_T1846=_T1844[_T1845];_T1847=(int)_T1846;_T1848=yyss;_T1849=_T1848.curr;_T184A=(short*)_T1849;_T184B=yyssp_offset;_T184C=_T184A[_T184B];_T184D=(int)_T184C;if(_T1847!=_T184D)goto _TL2FF;_T184E=Cyc_yytable;_T184F=yystate;_T1850=_T184E[_T184F];
yystate=(int)_T1850;goto _TL300;
# 415
_TL2FF: _T1851=Cyc_yydefgoto;_T1852=yyn - 176;_T1853=_T1851[_T1852];yystate=(int)_T1853;_TL300: goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus!=0)goto _TL301;
# 424
yynerrs=yynerrs + 1;_T1854=Cyc_yypact;_T1855=yystate;_T1856=_check_known_subscript_notnull(_T1854,1269U,sizeof(short),_T1855);_T1857=(short*)_T1856;_T1858=*_T1857;
# 427
yyn=(int)_T1858;_T1859=yyn;_T185A=- 32768;
# 429
if(_T1859 <= _T185A)goto _TL303;if(yyn >= 8463)goto _TL303;{
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
if(yyn >= 0)goto _TL305;_T185B=- yyn;goto _TL306;_TL305: _T185B=0;_TL306: x=_T185B;
_TL30A: _T185C=x;_T185D=(unsigned)_T185C;_T185E=364U / sizeof(char*);
# 437
if(_T185D < _T185E)goto _TL308;else{goto _TL309;}
# 439
_TL308: _T185F=Cyc_yycheck;_T1860=x + yyn;_T1861=_check_known_subscript_notnull(_T185F,8464U,sizeof(short),_T1860);_T1862=(short*)_T1861;_T1863=*_T1862;_T1864=(int)_T1863;_T1865=x;if(_T1864!=_T1865)goto _TL30B;_T1866=Cyc_yytname;_T1867=x;_T1868=_check_known_subscript_notnull(_T1866,364U,sizeof(struct _fat_ptr),_T1867);_T1869=(struct _fat_ptr*)_T1868;_T186A=*_T1869;_T186B=
Cyc_strlen(_T186A);_T186C=_T186B + 15U;sze=sze + _T186C;count=count + 1;goto _TL30C;_TL30B: _TL30C:
# 438
 x=x + 1;goto _TL30A;_TL309: _T186E=sze + 15;_T186F=(unsigned)_T186E;{unsigned _T18AB=_T186F + 1U;_T1871=yyregion;_T1872=_check_times(_T18AB,sizeof(char));{char*_T18AC=_region_malloc(_T1871,0U,_T1872);{unsigned _T18AD=_T18AB;unsigned i;i=0;_TL310: if(i < _T18AD)goto _TL30E;else{goto _TL30F;}_TL30E: _T1873=i;
# 441
_T18AC[_T1873]='\000';i=i + 1;goto _TL310;_TL30F: _T18AC[_T18AD]=0;}_T1870=(char*)_T18AC;}_T186D=_tag_fat(_T1870,sizeof(char),_T18AB);}msg=_T186D;_T1874=msg;_T1875=
_tag_fat("parse error",sizeof(char),12U);Cyc_strcpy(_T1874,_T1875);
# 444
if(count >= 5)goto _TL311;
# 446
count=0;
if(yyn >= 0)goto _TL313;_T1876=- yyn;goto _TL314;_TL313: _T1876=0;_TL314: x=_T1876;
_TL318: _T1877=x;_T1878=(unsigned)_T1877;_T1879=364U / sizeof(char*);
# 447
if(_T1878 < _T1879)goto _TL316;else{goto _TL317;}
# 449
_TL316: _T187A=Cyc_yycheck;_T187B=x + yyn;_T187C=_check_known_subscript_notnull(_T187A,8464U,sizeof(short),_T187B);_T187D=(short*)_T187C;_T187E=*_T187D;_T187F=(int)_T187E;_T1880=x;if(_T187F!=_T1880)goto _TL319;_T1881=msg;
# 451
if(count!=0)goto _TL31B;_T1883=
_tag_fat(", expecting `",sizeof(char),14U);_T1882=_T1883;goto _TL31C;_TL31B: _T1884=
_tag_fat(" or `",sizeof(char),6U);_T1882=_T1884;_TL31C:
# 451
 Cyc_strcat(_T1881,_T1882);_T1885=msg;_T1886=Cyc_yytname;_T1887=x;_T1888=_check_known_subscript_notnull(_T1886,364U,sizeof(struct _fat_ptr),_T1887);_T1889=(struct _fat_ptr*)_T1888;_T188A=*_T1889;
# 454
Cyc_strcat(_T1885,_T188A);_T188B=msg;_T188C=
_tag_fat("'",sizeof(char),2U);Cyc_strcat(_T188B,_T188C);
count=count + 1;goto _TL31A;_TL319: _TL31A:
# 448
 x=x + 1;goto _TL318;_TL317: goto _TL312;_TL311: _TL312:
# 459
 Cyc_yyerror(msg,yystate,yychar);}goto _TL304;
# 463
_TL303: _T188D=_tag_fat("parse error",sizeof(char),12U);_T188E=yystate;_T188F=yychar;Cyc_yyerror(_T188D,_T188E,_T188F);_TL304: goto _TL302;_TL301: _TL302: goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus!=3)goto _TL31D;
# 474
if(yychar!=0)goto _TL31F;{int _T18AB=1;_npop_handler(0);return _T18AB;}_TL31F:
# 483
 yychar=- 2;goto _TL31E;_TL31D: _TL31E:
# 489
 yyerrstatus=3;goto yyerrhandle;
# 493
yyerrdefault:
# 503 "cycbison.simple"
 yyerrpop:
# 505
 if(yyssp_offset!=0)goto _TL321;{int _T18AB=1;_npop_handler(0);return _T18AB;}_TL321:
 yyvsp_offset=yyvsp_offset + -1;_T1890=yyss;
yyssp_offset=yyssp_offset + -1;_T1891=yyssp_offset;_T1892=_check_fat_subscript(_T1890,sizeof(short),_T1891);_T1893=(short*)_T1892;_T1894=*_T1893;yystate=(int)_T1894;
# 521 "cycbison.simple"
yyerrhandle: _T1895=Cyc_yypact;_T1896=yystate;_T1897=_check_known_subscript_notnull(_T1895,1269U,sizeof(short),_T1896);_T1898=(short*)_T1897;_T1899=*_T1898;
yyn=(int)_T1899;_T189A=yyn;_T189B=- 32768;
if(_T189A!=_T189B)goto _TL323;goto yyerrdefault;_TL323:
# 525
 yyn=yyn + 1;
if(yyn < 0)goto _TL327;else{goto _TL329;}_TL329: if(yyn > 8463)goto _TL327;else{goto _TL328;}_TL328: _T189C=Cyc_yycheck;_T189D=yyn;_T189E=_T189C[_T189D];_T189F=(int)_T189E;if(_T189F!=1)goto _TL327;else{goto _TL325;}_TL327: goto yyerrdefault;_TL325: _T18A0=Cyc_yytable;_T18A1=yyn;_T18A2=_T18A0[_T18A1];
# 528
yyn=(int)_T18A2;
if(yyn >= 0)goto _TL32A;_T18A3=yyn;_T18A4=- 32768;
# 531
if(_T18A3!=_T18A4)goto _TL32C;goto yyerrpop;_TL32C:
 yyn=- yyn;goto yyreduce;
# 535
_TL32A: if(yyn!=0)goto _TL32E;goto yyerrpop;_TL32E:
# 537
 if(yyn!=1268)goto _TL330;{int _T18AB=0;_npop_handler(0);return _T18AB;}_TL330: _T18A5=yyvs;
# 546
yyvsp_offset=yyvsp_offset + 1;_T18A6=yyvsp_offset;_T18A7=_check_fat_subscript(_T18A5,sizeof(struct Cyc_Yystacktype),_T18A6);_T18A8=(struct Cyc_Yystacktype*)_T18A7;{struct Cyc_Yystacktype _T18AB;_T18AB.v=yylval;_T18AB.l=yylloc;_T18A9=_T18AB;}*_T18A8=_T18A9;goto yynewstate;}}}}_pop_region();}
# 3351 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _T0;struct _union_YYSTYPE_Stmt_tok _T1;unsigned _T2;union Cyc_YYSTYPE _T3;struct _union_YYSTYPE_Int_tok _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;union Cyc_YYSTYPE _T9;struct _union_YYSTYPE_Char_tok _TA;struct Cyc_Int_pa_PrintArg_struct _TB;char _TC;int _TD;struct Cyc___cycFILE*_TE;struct _fat_ptr _TF;struct _fat_ptr _T10;union Cyc_YYSTYPE _T11;struct _union_YYSTYPE_String_tok _T12;struct Cyc_String_pa_PrintArg_struct _T13;struct Cyc___cycFILE*_T14;struct _fat_ptr _T15;struct _fat_ptr _T16;union Cyc_YYSTYPE _T17;struct _union_YYSTYPE_QualId_tok _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct Cyc___cycFILE*_T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;union Cyc_YYSTYPE _T1D;struct _union_YYSTYPE_Exp_tok _T1E;struct Cyc_String_pa_PrintArg_struct _T1F;struct Cyc___cycFILE*_T20;struct _fat_ptr _T21;struct _fat_ptr _T22;union Cyc_YYSTYPE _T23;struct _union_YYSTYPE_Stmt_tok _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct Cyc___cycFILE*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc___cycFILE*_T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;struct Cyc_Absyn_Stmt*_T2C;struct Cyc_Absyn_Exp*_T2D;struct _tuple0*_T2E;struct _fat_ptr _T2F;char _T30;union Cyc_Absyn_Cnst _T31;_T0=v;_T1=_T0.Stmt_tok;_T2=_T1.tag;switch(_T2){case 2: _T3=v;_T4=_T3.Int_tok;_T31=_T4.val;{union Cyc_Absyn_Cnst c=_T31;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
# 3353
_T32.f1=Cyc_Absynpp_cnst2string(c);_T5=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T5;void*_T33[1];_T33[0]=& _T32;_T6=Cyc_stderr;_T7=_tag_fat("%s",sizeof(char),3U);_T8=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}goto _LL0;}case 3: _T9=v;_TA=_T9.Char_tok;_T30=_TA.val;{char c=_T30;{struct Cyc_Int_pa_PrintArg_struct _T32;_T32.tag=1;_TC=c;_TD=(int)_TC;
_T32.f1=(unsigned long)_TD;_TB=_T32;}{struct Cyc_Int_pa_PrintArg_struct _T32=_TB;void*_T33[1];_T33[0]=& _T32;_TE=Cyc_stderr;_TF=_tag_fat("%c",sizeof(char),3U);_T10=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_TE,_TF,_T10);}goto _LL0;}case 4: _T11=v;_T12=_T11.String_tok;_T2F=_T12.val;{struct _fat_ptr s=_T2F;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
_T32.f1=s;_T13=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T13;void*_T33[1];_T33[0]=& _T32;_T14=Cyc_stderr;_T15=_tag_fat("\"%s\"",sizeof(char),5U);_T16=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T14,_T15,_T16);}goto _LL0;}case 5: _T17=v;_T18=_T17.QualId_tok;_T2E=_T18.val;{struct _tuple0*q=_T2E;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
_T32.f1=Cyc_Absynpp_qvar2string(q);_T19=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T19;void*_T33[1];_T33[0]=& _T32;_T1A=Cyc_stderr;_T1B=_tag_fat("%s",sizeof(char),3U);_T1C=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T1A,_T1B,_T1C);}goto _LL0;}case 6: _T1D=v;_T1E=_T1D.Exp_tok;_T2D=_T1E.val;{struct Cyc_Absyn_Exp*e=_T2D;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
_T32.f1=Cyc_Absynpp_exp2string(e);_T1F=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T1F;void*_T33[1];_T33[0]=& _T32;_T20=Cyc_stderr;_T21=_tag_fat("%s",sizeof(char),3U);_T22=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T20,_T21,_T22);}goto _LL0;}case 7: _T23=v;_T24=_T23.Stmt_tok;_T2C=_T24.val;{struct Cyc_Absyn_Stmt*s=_T2C;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
_T32.f1=Cyc_Absynpp_stmt2string(s);_T25=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T25;void*_T33[1];_T33[0]=& _T32;_T26=Cyc_stderr;_T27=_tag_fat("%s",sizeof(char),3U);_T28=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T26,_T27,_T28);}goto _LL0;}default: _T29=Cyc_stderr;_T2A=
_tag_fat("?",sizeof(char),2U);_T2B=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T29,_T2A,_T2B);goto _LL0;}_LL0:;}
# 3363
struct _fat_ptr Cyc_token2string(int token){struct _fat_ptr _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;int _T3;short*_T4;int _T5;short _T6;int _T7;unsigned _T8;struct _fat_ptr*_T9;int _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;
if(token > 0)goto _TL333;_T0=
_tag_fat("end-of-file",sizeof(char),12U);return _T0;_TL333:
 if(token!=388)goto _TL335;_T1=Cyc_Lex_token_string;
return _T1;_TL335:
 if(token!=397)goto _TL337;_T2=
Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);return _T2;_TL337:
 if(token <= 0)goto _TL339;if(token > 401)goto _TL339;_T4=Cyc_yytranslate;_T5=token;_T6=_T4[_T5];_T3=(int)_T6;goto _TL33A;_TL339: _T3=364;_TL33A: {int z=_T3;_T7=z;_T8=(unsigned)_T7;
if(_T8 >= 364U)goto _TL33B;_T9=Cyc_yytname;_TA=z;_TB=_T9[_TA];
return _TB;_TL33B: _TC=
_tag_fat(0,0,0);return _TC;}}
# 3377
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){struct _RegionHandle*_T0;struct Cyc_Lexing_lexbuf*_T1;
Cyc_Parse_parse_result=0;{struct _RegionHandle _T2=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _T2;_push_region(yyr);_T0=yyr;_T1=
# 3380
Cyc_Lexing_from_file(f);Cyc_yyparse(_T0,_T1);{struct Cyc_List_List*_T3=Cyc_Parse_parse_result;_npop_handler(0);return _T3;}_pop_region();}}
# 3384
struct Cyc_List_List*Cyc_Parse_parse_constraint_file(struct Cyc___cycFILE*f){struct _RegionHandle*_T0;struct Cyc_Lexing_lexbuf*_T1;
Cyc_Parse_constraint_graph=0;{struct _RegionHandle _T2=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _T2;_push_region(yyr);_T0=yyr;_T1=
# 3387
Cyc_Lexing_from_file(f);Cyc_yyparse(_T0,_T1);{struct Cyc_List_List*_T3=Cyc_Parse_constraint_graph;_npop_handler(0);return _T3;}_pop_region();}}

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
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Cvar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;void*f4;const char*f5;const char*f6;int f7;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_Absyn_Exp*f7;struct Cyc_Absyn_Exp*f8;struct Cyc_Absyn_Exp*f9;struct Cyc_Absyn_Exp*f10;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 531
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};
# 563
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;struct Cyc_Absyn_Exp*aqual;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple8{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;int f5;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 731 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 899
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 925
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 933
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 939
void*Cyc_Absyn_compress(void*);
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 954
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 957
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 959
extern void*Cyc_Absyn_sint_type;
# 961
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 964
extern void*Cyc_Absyn_complex_type(void*);
# 966
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_shorthand_type;extern void*Cyc_Absyn_refcnt_rgn_shorthand_type;
# 968
extern void*Cyc_Absyn_al_qual_type;extern void*Cyc_Absyn_un_qual_type;extern void*Cyc_Absyn_rc_qual_type;extern void*Cyc_Absyn_rtd_qual_type;
# 972
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 974
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_aqual_handle_type(void*);extern void*Cyc_Absyn_aqual_var_type(void*,void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_tuple_type(struct Cyc_List_List*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 993
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*Cyc_Absyn_tuple_field_designator(int);
# 1005
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 1009
void*Cyc_Absyn_bounds_one (void);
void*Cyc_Absyn_cvar_type(struct Cyc_Core_Opt*);
void*Cyc_Absyn_cvar_type_name(struct Cyc_Core_Opt*,struct _fat_ptr);
# 1015
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1035
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1038
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1042
void*Cyc_Absyn_aqualsof_type(void*);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tagof_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1084
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1094
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1096
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1103
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1105
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
# 1119
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1123
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_false_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1137
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1142
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1153
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1156
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1159
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1163
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1168
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1170
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1179
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1186
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1189
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1194
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
# 79 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*);
void Cyc_Lex_enter_namespace(struct _fat_ptr*);
void Cyc_Lex_leave_namespace (void);
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using (void);
void Cyc_Lex_enter_extern_c (void);
void Cyc_Lex_leave_extern_c (void);
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _fat_ptr Cyc_Lex_token_string;
# 103 "parse.y"
int Cyc_Parse_parsing_tempest=0;
# 109
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
# 120
return _T5;}}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};
# 176
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f0;void*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 181
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
static struct Cyc_List_List*Cyc_Parse_constraint_graph=0;
static int Cyc_Parse_inside_function_definition=0;
static int Cyc_Parse_inside_noinference_block=0;
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){struct Cyc_Warn_String_Warn_Warg_struct _T0;unsigned _T1;struct _fat_ptr _T2;struct Cyc_Parse_Exit_exn_struct*_T3;void*_T4;{struct Cyc_Warn_String_Warn_Warg_struct _T5;_T5.tag=0;
_T5.f1=msg;_T0=_T5;}{struct Cyc_Warn_String_Warn_Warg_struct _T5=_T0;void*_T6[1];_T6[0]=& _T5;_T1=loc;_T2=_tag_fat(_T6,sizeof(void*),1);Cyc_Warn_err2(_T1,_T2);}{struct Cyc_Parse_Exit_exn_struct*_T5=_cycalloc(sizeof(struct Cyc_Parse_Exit_exn_struct));
_T5->tag=Cyc_Parse_Exit;_T3=(struct Cyc_Parse_Exit_exn_struct*)_T5;}_T4=(void*)_T3;_throw(_T4);}
# 190
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){struct _tuple8*_T0;struct Cyc_Absyn_Tqual _T1;int _T2;struct Cyc_Absyn_Tqual _T3;int _T4;struct Cyc_Absyn_Tqual _T5;int _T6;struct Cyc_Absyn_Tqual _T7;unsigned _T8;struct Cyc_Absyn_Tqual _T9;unsigned _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;void*_TD;void*_TE;struct Cyc_Absyn_Tqual _TF;_T0=tqt;{struct _tuple8 _T10=*_T0;_TF=_T10.f1;_TE=_T10.f2;}{struct Cyc_Absyn_Tqual tq=_TF;void*t=_TE;_T1=tq;_T2=_T1.print_const;
# 193
if(_T2)goto _TL7;else{goto _TL9;}_TL9: _T3=tq;_T4=_T3.q_volatile;if(_T4)goto _TL7;else{goto _TL8;}_TL8: _T5=tq;_T6=_T5.q_restrict;if(_T6)goto _TL7;else{goto _TL5;}
_TL7: _T7=tq;_T8=_T7.loc;if(_T8==0U)goto _TLA;_T9=tq;loc=_T9.loc;goto _TLB;_TLA: _TLB: _TA=loc;_TB=
_tag_fat("qualifier on type is ignored",sizeof(char),29U);_TC=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TA,_TB,_TC);goto _TL6;_TL5: _TL6: _TD=t;
# 197
return _TD;}}
# 200
static void*Cyc_Parse_make_pointer_mod(struct _RegionHandle*r,struct Cyc_Absyn_PtrLoc*loc,void*nullable,void*bound,void*eff,struct Cyc_List_List*pqs,struct Cyc_Absyn_Tqual tqs){struct Cyc_List_List*_T0;int*_T1;unsigned _T2;void*_T3;void*_T4;int(*_T5)(struct _fat_ptr,struct _fat_ptr);void*(*_T6)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_List_List*_T9;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_TA;struct _RegionHandle*_TB;struct Cyc_Core_Opt*_TC;struct Cyc_Core_Opt*_TD;void*_TE;void*_TF;void*_T10;
# 205
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
# 208
pqs=_T9->tl;goto _TLF;_TLE: _TB=r;{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_T11=_region_malloc(_TB,0U,sizeof(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct));_T11->tag=2;
# 222
_T11->f1.eff=eff;_T11->f1.nullable=nullable;_T11->f1.bounds=bound;_T11->f1.zero_term=zeroterm;
_T11->f1.ptrloc=loc;_T11->f1.autoreleased=autoreleased;
if(aqual==0)goto _TL11;_T11->f1.aqual=aqual;goto _TL12;_TL11: _TC=& Cyc_Kinds_aqko;_TD=(struct Cyc_Core_Opt*)_TC;_TE=Cyc_Absyn_new_evar(_TD,0);_TF=Cyc_Absyn_rtd_qual_type;_T11->f1.aqual=Cyc_Absyn_aqual_var_type(_TE,_TF);_TL12:
 _T11->f2=tqs;_TA=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_T11;}_T10=(void*)_TA;
# 222
return _T10;}
# 229
static void Cyc_Parse_check_single_constraint(unsigned loc,struct _fat_ptr id){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;struct Cyc_String_pa_PrintArg_struct _T3;unsigned _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;_T0=id;_T1=
_tag_fat("single",sizeof(char),7U);_T2=Cyc_zstrcmp(_T0,_T1);if(!_T2)goto _TL13;{struct Cyc_String_pa_PrintArg_struct _T7;_T7.tag=0;
_T7.f1=_tag_fat(" is not a valid effect constraint",sizeof(char),34U);_T3=_T7;}{struct Cyc_String_pa_PrintArg_struct _T7=_T3;void*_T8[1];_T8[0]=& _T7;_T4=loc;_T5=id;_T6=_tag_fat(_T8,sizeof(void*),1);Cyc_Warn_err(_T4,_T5,_T6);}goto _TL14;_TL13: _TL14:;}
# 233
static void*Cyc_Parse_effect_from_atomic(struct Cyc_List_List*effs){int _T0;struct Cyc_List_List*_T1;void*_T2;void*_T3;_T0=
Cyc_List_length(effs);if(_T0!=1)goto _TL15;_T1=
_check_null(effs);_T2=_T1->hd;return _T2;
# 237
_TL15: _T3=Cyc_Absyn_join_eff(effs);return _T3;}
# 240
static struct _tuple0*Cyc_Parse_gensym_enum (void){struct _tuple0*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct Cyc_Int_pa_PrintArg_struct _T3;int _T4;int _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;
static int enum_counter=0;{struct _tuple0*_T8=_cycalloc(sizeof(struct _tuple0));
_T8->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T9=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _TA;_TA.tag=1;_T4=enum_counter;
enum_counter=_T4 + 1;_T5=_T4;_TA.f1=(unsigned long)_T5;_T3=_TA;}{struct Cyc_Int_pa_PrintArg_struct _TA=_T3;void*_TB[1];_TB[0]=& _TA;_T6=_tag_fat("__anonymous_enum_%d__",sizeof(char),22U);_T7=_tag_fat(_TB,sizeof(void*),1);_T2=Cyc_aprintf(_T6,_T7);}*_T9=_T2;_T1=(struct _fat_ptr*)_T9;}_T8->f1=_T1;_T0=(struct _tuple0*)_T8;}
# 242
return _T0;}struct _tuple15{unsigned f0;struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple17{struct _tuple15*f0;struct _tuple16*f1;};
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple17*field_info){struct _tuple17*_T0;struct _tuple15*_T1;struct _tuple16*_T2;unsigned _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;int _T6;unsigned _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_Absyn_Aggrfield*_TA;struct _tuple0*_TB;struct _tuple0 _TC;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;void*_T11;struct Cyc_Absyn_Tqual _T12;struct _tuple0*_T13;unsigned _T14;_T0=field_info;{struct _tuple17 _T15=*_T0;_T1=_T15.f0;{struct _tuple15 _T16=*_T1;_T14=_T16.f0;_T13=_T16.f1;_T12=_T16.f2;_T11=_T16.f3;_T10=_T16.f4;_TF=_T16.f5;}_T2=_T15.f1;{struct _tuple16 _T16=*_T2;_TE=_T16.f0;_TD=_T16.f1;}}{unsigned varloc=_T14;struct _tuple0*qid=_T13;struct Cyc_Absyn_Tqual tq=_T12;void*t=_T11;struct Cyc_List_List*tvs=_T10;struct Cyc_List_List*atts=_TF;struct Cyc_Absyn_Exp*widthopt=_TE;struct Cyc_Absyn_Exp*reqopt=_TD;
# 252
if(tvs==0)goto _TL17;_T3=loc;_T4=
_tag_fat("bad type params in struct field",sizeof(char),32U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T3,_T4,_T5);goto _TL18;_TL17: _TL18: _T6=
Cyc_Absyn_is_qvar_qualified(qid);if(!_T6)goto _TL19;_T7=loc;_T8=
_tag_fat("struct or union field cannot be qualified with a namespace",sizeof(char),59U);_T9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T7,_T8,_T9);goto _TL1A;_TL19: _TL1A:{struct Cyc_Absyn_Aggrfield*_T15=_cycalloc(sizeof(struct Cyc_Absyn_Aggrfield));_TB=qid;_TC=*_TB;
_T15->name=_TC.f1;_T15->tq=tq;_T15->type=t;
_T15->width=widthopt;_T15->attributes=atts;
_T15->requires_clause=reqopt;_TA=(struct Cyc_Absyn_Aggrfield*)_T15;}
# 256
return _TA;}}
# 261
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
# 262
return _T0;}
# 273
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Type_spec=t;
s.Valid_type_spec=1;_T0=s;
return _T0;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Signed_spec=1;_T0=s;
return _T0;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Unsigned_spec=1;_T0=s;
return _T0;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Short_spec=1;_T0=s;
return _T0;}
# 294
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Long_spec=1;_T0=s;
return _T0;}
# 299
static struct Cyc_Parse_Type_specifier Cyc_Parse_complex_spec(unsigned loc){struct Cyc_Parse_Type_specifier _T0;
struct Cyc_Parse_Type_specifier s=Cyc_Parse_empty_spec(loc);
s.Complex_spec=1;_T0=s;
return _T0;}
# 306
static void*Cyc_Parse_array2ptr(void*t,int argposn){void*_T0;int _T1;void*_T2;void*_T3;int _T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;void*_T7;_T1=
# 308
Cyc_Tcutil_is_array_type(t);if(!_T1)goto _TL1B;_T2=t;_T4=argposn;
if(!_T4)goto _TL1D;_T5=& Cyc_Kinds_eko;_T6=(struct Cyc_Core_Opt*)_T5;_T3=Cyc_Absyn_new_evar(_T6,0);goto _TL1E;_TL1D: _T3=Cyc_Absyn_heap_rgn_type;_TL1E: _T7=Cyc_Absyn_al_qual_type;_T0=Cyc_Tcutil_promote_array(_T2,_T3,_T7,0);goto _TL1C;_TL1B: _T0=t;_TL1C:
# 308
 return _T0;}struct _tuple18{struct _fat_ptr*f0;void*f1;};
# 321 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;struct _tuple8*_T2;void*_T3;int*_T4;int _T5;struct _tuple8*_T6;void*_T7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8;void*_T9;int*_TA;int _TB;struct _tuple8*_TC;void*_TD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE;struct Cyc_List_List*_TF;struct _tuple8*_T10;void*_T11;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_List_List*_T17;void*_T18;struct _fat_ptr*_T19;void*_T1A;int*_T1B;int _T1C;void*_T1D;void*_T1E;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T1F;void**_T20;struct _fat_ptr*_T21;struct _fat_ptr _T22;struct Cyc_String_pa_PrintArg_struct _T23;struct _fat_ptr*_T24;struct _fat_ptr _T25;struct _fat_ptr _T26;void**_T27;struct Cyc_Absyn_Tvar*_T28;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T29;struct Cyc_Absyn_Kind*_T2A;struct Cyc_List_List*_T2B;struct _tuple18*_T2C;struct _tuple8*_T2D;struct _fat_ptr*_T2E;struct _tuple8*_T2F;struct _fat_ptr*_T30;struct _tuple8*_T31;struct _fat_ptr*_T32;struct _tuple8*_T33;struct _fat_ptr*_T34;struct _tuple8*_T35;void*_T36;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T37;void*_T38;int*_T39;int _T3A;struct _tuple8*_T3B;void*_T3C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3D;struct Cyc_List_List*_T3E;struct _tuple8*_T3F;void*_T40;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T41;struct Cyc_List_List*_T42;struct Cyc_List_List*_T43;void*_T44;int*_T45;int _T46;struct _tuple8*_T47;void*_T48;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T49;struct Cyc_List_List*_T4A;struct Cyc_List_List*_T4B;struct Cyc_List_List*_T4C;struct _fat_ptr*_T4D;void*_T4E;struct Cyc_List_List*_T4F;void*_T50;void*_T51;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T52;struct Cyc_List_List*_T53;void*_T54;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T55;void**_T56;struct _fat_ptr*_T57;struct _fat_ptr _T58;struct Cyc_String_pa_PrintArg_struct _T59;struct _fat_ptr _T5A;struct _fat_ptr _T5B;void**_T5C;struct Cyc_Absyn_Tvar*_T5D;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T5E;struct Cyc_Absyn_Kind*_T5F;struct _tuple8*_T60;struct _fat_ptr*_T61;struct Cyc_List_List*_T62;struct Cyc_List_List*_T63;
struct Cyc_List_List*res=0;
_TL22: if(x!=0)goto _TL20;else{goto _TL21;}
_TL20: _T0=x;_T1=_T0->hd;{struct _tuple8*_T64=(struct _tuple8*)_T1;struct _fat_ptr _T65;void*_T66;void*_T67;_T2=(struct _tuple8*)_T64;_T3=_T2->f2;_T4=(int*)_T3;_T5=*_T4;if(_T5!=0)goto _TL23;_T6=(struct _tuple8*)_T64;_T7=_T6->f2;_T8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7;_T9=_T8->f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=5)goto _TL25;_TC=(struct _tuple8*)_T64;_TD=_TC->f2;_TE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD;_TF=_TE->f2;if(_TF==0)goto _TL27;_T10=(struct _tuple8*)_T64;_T11=_T10->f2;_T12=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T13=_T12->f2;_T14=(struct Cyc_List_List*)_T13;_T15=_T14->tl;if(_T15!=0)goto _TL29;{struct _tuple8 _T68=*_T64;_T67=_T68.f0;_T16=_T68.f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T69=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T16;_T17=_T69->f2;{struct Cyc_List_List _T6A=*_T17;_T18=_T6A.hd;_T66=(void*)_T18;}}}_T19=(struct _fat_ptr*)_T67;if(_T19==0)goto _TL2B;{struct _fat_ptr*v=_T67;void*i=_T66;{void*_T68;_T1A=i;_T1B=(int*)_T1A;_T1C=*_T1B;if(_T1C!=1)goto _TL2D;_T1D=i;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T69=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T1D;_T1E=i;_T1F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T1E;_T20=& _T1F->f2;_T68=(void**)_T20;}{void**z=(void**)_T68;
# 330
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr));_T21=nm;{struct Cyc_String_pa_PrintArg_struct _T69;_T69.tag=0;_T24=v;_T69.f1=*_T24;_T23=_T69;}{struct Cyc_String_pa_PrintArg_struct _T69=_T23;void*_T6A[1];_T6A[0]=& _T69;_T25=_tag_fat("`%s",sizeof(char),4U);_T26=_tag_fat(_T6A,sizeof(void*),1);_T22=Cyc_aprintf(_T25,_T26);}*_T21=_T22;_T27=z;{struct Cyc_Absyn_Tvar*_T69=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_T69->name=nm;_T69->identity=-1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T6A=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T6A->tag=0;_T2A=& Cyc_Kinds_ik;_T6A->f1=(struct Cyc_Absyn_Kind*)_T2A;_T29=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T6A;}_T69->kind=(void*)_T29;_T69->aquals_bound=0;_T28=(struct Cyc_Absyn_Tvar*)_T69;}*_T27=Cyc_Absyn_var_type(_T28);goto _LL7;}_TL2D: goto _LL7;_LL7:;}{struct Cyc_List_List*_T68=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T69=_cycalloc(sizeof(struct _tuple18));
# 335
_T69->f0=v;_T69->f1=i;_T2C=(struct _tuple18*)_T69;}_T68->hd=_T2C;_T68->tl=res;_T2B=(struct Cyc_List_List*)_T68;}res=_T2B;goto _LL0;}_TL2B: _T2D=(struct _tuple8*)_T64;_T2E=_T2D->f0;if(_T2E==0)goto _TL2F;goto _LL5;_TL2F: goto _LL5;_TL29: _T2F=(struct _tuple8*)_T64;_T30=_T2F->f0;if(_T30==0)goto _TL31;goto _LL5;_TL31: goto _LL5;_TL27: _T31=(struct _tuple8*)_T64;_T32=_T31->f0;if(_T32==0)goto _TL33;goto _LL5;_TL33: goto _LL5;_TL25: _T33=(struct _tuple8*)_T64;_T34=_T33->f0;if(_T34==0)goto _TL35;_T35=(struct _tuple8*)_T64;_T36=_T35->f2;_T37=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T36;_T38=_T37->f1;_T39=(int*)_T38;_T3A=*_T39;if(_T3A!=4)goto _TL37;_T3B=(struct _tuple8*)_T64;_T3C=_T3B->f2;_T3D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C;_T3E=_T3D->f2;if(_T3E==0)goto _TL39;_T3F=(struct _tuple8*)_T64;_T40=_T3F->f2;_T41=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T40;_T42=_T41->f2;_T43=(struct Cyc_List_List*)_T42;_T44=_T43->hd;_T45=(int*)_T44;_T46=*_T45;if(_T46!=1)goto _TL3B;_T47=(struct _tuple8*)_T64;_T48=_T47->f2;_T49=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T48;_T4A=_T49->f2;_T4B=(struct Cyc_List_List*)_T4A;_T4C=_T4B->tl;if(_T4C!=0)goto _TL3D;{struct _tuple8 _T68=*_T64;_T4D=_T68.f0;{struct _fat_ptr _T69=*_T4D;_T65=_T69;}_T4E=_T68.f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T69=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4E;_T4F=_T69->f2;{struct Cyc_List_List _T6A=*_T4F;_T50=_T6A.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T50;_T51=_T64->f2;_T52=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T51;_T53=_T52->f2;_T54=_T53->hd;_T55=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T54;_T56=& _T55->f2;_T67=(void**)_T56;}}}}{struct _fat_ptr v=_T65;void**z=(void**)_T67;
# 339
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr));_T57=nm;{struct Cyc_String_pa_PrintArg_struct _T68;_T68.tag=0;_T68.f1=v;_T59=_T68;}{struct Cyc_String_pa_PrintArg_struct _T68=_T59;void*_T69[1];_T69[0]=& _T68;_T5A=_tag_fat("`%s",sizeof(char),4U);_T5B=_tag_fat(_T69,sizeof(void*),1);_T58=Cyc_aprintf(_T5A,_T5B);}*_T57=_T58;_T5C=z;{struct Cyc_Absyn_Tvar*_T68=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_T68->name=nm;_T68->identity=-1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T69=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T69->tag=0;_T5F=& Cyc_Kinds_ek;_T69->f1=(struct Cyc_Absyn_Kind*)_T5F;_T5E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T69;}_T68->kind=(void*)_T5E;_T68->aquals_bound=0;_T5D=(struct Cyc_Absyn_Tvar*)_T68;}*_T5C=Cyc_Absyn_var_type(_T5D);goto _LL0;}_TL3D: goto _LL5;_TL3B: goto _LL5;_TL39: goto _LL5;_TL37: goto _LL5;_TL35: goto _LL5;_TL23: _T60=(struct _tuple8*)_T64;_T61=_T60->f0;if(_T61==0)goto _TL3F;goto _LL5;_TL3F: _LL5: goto _LL0;_LL0:;}_T62=x;
# 323
x=_T62->tl;goto _TL22;_TL21: _T63=res;
# 344
return _T63;}
# 348
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Aggrfield*_T2;int*_T3;int _T4;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5;void*_T6;int*_T7;int _T8;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9;struct Cyc_List_List*_TA;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;void*_T10;struct Cyc_List_List*_T11;struct _tuple18*_T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_Absyn_Aggrfield*_T15;struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;
struct Cyc_List_List*res=0;
_TL44: if(x!=0)goto _TL42;else{goto _TL43;}
_TL42: _T0=x;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Aggrfield*)_T1;{void*_T18=_T2->type;void*_T19;_T3=(int*)_T18;_T4=*_T3;if(_T4!=0)goto _TL45;_T5=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_T6=_T5->f1;_T7=(int*)_T6;_T8=*_T7;if(_T8!=5)goto _TL47;_T9=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_TA=_T9->f2;if(_TA==0)goto _TL49;_TB=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_TC=_TB->f2;_TD=(struct Cyc_List_List*)_TC;_TE=_TD->tl;if(_TE!=0)goto _TL4B;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T18;_TF=_T1A->f2;{struct Cyc_List_List _T1B=*_TF;_T10=_T1B.hd;_T19=(void*)_T10;}}{void*i=_T19;{struct Cyc_List_List*_T1A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T1B=_cycalloc(sizeof(struct _tuple18));_T13=x;_T14=_T13->hd;_T15=(struct Cyc_Absyn_Aggrfield*)_T14;
# 353
_T1B->f0=_T15->name;_T1B->f1=i;_T12=(struct _tuple18*)_T1B;}_T1A->hd=_T12;_T1A->tl=res;_T11=(struct Cyc_List_List*)_T1A;}res=_T11;goto _LL0;}_TL4B: goto _LL3;_TL49: goto _LL3;_TL47: goto _LL3;_TL45: _LL3: goto _LL0;_LL0:;}_T16=x;
# 350
x=_T16->tl;goto _TL44;_TL43: _T17=res;
# 356
return _T17;}
# 360
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T3;void*_T4;int*_T5;int _T6;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T7;void*_T8;struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T9;struct _tuple0*_TA;struct _tuple0*_TB;union Cyc_Absyn_Nmspace _TC;struct _union_Nmspace_Rel_n _TD;unsigned _TE;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_TF;void*_T10;struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T11;struct _tuple0*_T12;struct _tuple0*_T13;union Cyc_Absyn_Nmspace _T14;struct _union_Nmspace_Rel_n _T15;struct Cyc_List_List*_T16;void*_T17;struct _tuple0*_T18;struct Cyc_List_List*_T19;void*_T1A;int _T1B;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T1C;void*_T1D;struct Cyc_Absyn_Exp*_T1E;unsigned _T1F;struct Cyc_Absyn_Exp*_T20;struct Cyc_List_List*_T21;struct Cyc_Absyn_Exp*_T22;_T0=e;{
void*_T23=_T0->r;struct _fat_ptr*_T24;_T1=(int*)_T23;_T2=*_T1;if(_T2!=1)goto _TL4D;_T3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T23;_T4=_T3->f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TL4F;_T7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T23;_T8=_T7->f1;_T9=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T8;_TA=_T9->f1;_TB=(struct _tuple0*)_TA;_TC=_TB->f0;_TD=_TC.Rel_n;_TE=_TD.tag;if(_TE!=2)goto _TL51;_TF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T23;_T10=_TF->f1;_T11=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T10;_T12=_T11->f1;_T13=(struct _tuple0*)_T12;_T14=_T13->f0;_T15=_T14.Rel_n;_T16=_T15.val;if(_T16!=0)goto _TL53;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T25=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T23;_T17=_T25->f1;{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_T26=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_T17;_T18=_T26->f1;{struct _tuple0 _T27=*_T18;_T24=_T27.f1;}}}{struct _fat_ptr*y=_T24;{
# 363
struct Cyc_List_List*ts=tags;_TL58: if(ts!=0)goto _TL56;else{goto _TL57;}
_TL56: _T19=ts;_T1A=_T19->hd;{struct _tuple18*_T25=(struct _tuple18*)_T1A;void*_T26;struct _fat_ptr*_T27;{struct _tuple18 _T28=*_T25;_T27=_T28.f0;_T26=_T28.f1;}{struct _fat_ptr*x=_T27;void*i=_T26;_T1B=
Cyc_strptrcmp(x,y);if(_T1B!=0)goto _TL59;{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T28=_cycalloc(sizeof(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct));_T28->tag=38;
_T28->f1=Cyc_Tcutil_copy_type(i);_T1C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T28;}_T1D=(void*)_T1C;_T1E=e;_T1F=_T1E->loc;_T20=Cyc_Absyn_new_exp(_T1D,_T1F);return _T20;_TL59:;}}_T21=ts;
# 363
ts=_T21->tl;goto _TL58;_TL57:;}goto _LL0;}_TL53: goto _LL3;_TL51: goto _LL3;_TL4F: goto _LL3;_TL4D: _LL3: goto _LL0;_LL0:;}_T22=e;
# 371
return _T22;}
# 376
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_ArrayInfo _T4;struct Cyc_Absyn_ArrayInfo _T5;struct Cyc_Absyn_ArrayInfo _T6;struct Cyc_Absyn_ArrayInfo _T7;struct Cyc_Absyn_ArrayInfo _T8;void*_T9;void*_TA;struct Cyc_Absyn_PtrInfo _TB;struct Cyc_Absyn_PtrInfo _TC;struct Cyc_Absyn_PtrInfo _TD;struct Cyc_Absyn_PtrAtts _TE;struct Cyc_Absyn_PtrInfo _TF;struct Cyc_Absyn_PtrAtts _T10;struct Cyc_Absyn_PtrInfo _T11;struct Cyc_Absyn_PtrAtts _T12;struct Cyc_Absyn_PtrInfo _T13;struct Cyc_Absyn_PtrAtts _T14;struct Cyc_Absyn_PtrInfo _T15;struct Cyc_Absyn_PtrAtts _T16;struct Cyc_Absyn_PtrInfo _T17;struct Cyc_Absyn_PtrAtts _T18;struct Cyc_Absyn_PtrInfo _T19;struct Cyc_Absyn_PtrAtts _T1A;struct Cyc_Absyn_PtrInfo _T1B;void*_T1C;void*_T1D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1E;void*_T1F;int*_T20;int _T21;void*_T22;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T26;struct Cyc_List_List*_T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;void*_T2A;struct Cyc_List_List*_T2B;void*_T2C;void*_T2D;void*_T2E;void*_T2F;void*_T30;{struct Cyc_Absyn_Exp*_T31;void*_T32;void*_T33;struct Cyc_Absyn_PtrLoc*_T34;void*_T35;void*_T36;unsigned _T37;void*_T38;void*_T39;struct Cyc_Absyn_Tqual _T3A;void*_T3B;_T0=t;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 5: _T3=t;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T3C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T3;_T4=_T3C->f1;_T3B=_T4.elt_type;_T5=_T3C->f1;_T3A=_T5.tq;_T6=_T3C->f1;_T39=_T6.num_elts;_T7=_T3C->f1;_T38=_T7.zero_term;_T8=_T3C->f1;_T37=_T8.zt_loc;}{void*et=_T3B;struct Cyc_Absyn_Tqual tq=_T3A;struct Cyc_Absyn_Exp*nelts=_T39;void*zt=_T38;unsigned ztloc=_T37;
# 379
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts==0)goto _TL5C;
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);goto _TL5D;_TL5C: _TL5D: {
void*et2=Cyc_Parse_substitute_tags(tags,et);
if(nelts!=nelts2)goto _TL60;else{goto _TL61;}_TL61: if(et!=et2)goto _TL60;else{goto _TL5E;}
_TL60: _T9=Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);return _T9;_TL5E: goto _LL0;}}case 4: _TA=t;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_TA;_TB=_T3C->f1;_T3B=_TB.elt_type;_TC=_T3C->f1;_T3A=_TC.elt_tq;_TD=_T3C->f1;_TE=_TD.ptr_atts;_T39=_TE.eff;_TF=_T3C->f1;_T10=_TF.ptr_atts;_T38=_T10.nullable;_T11=_T3C->f1;_T12=_T11.ptr_atts;_T36=_T12.bounds;_T13=_T3C->f1;_T14=_T13.ptr_atts;_T35=_T14.zero_term;_T15=_T3C->f1;_T16=_T15.ptr_atts;_T34=_T16.ptrloc;_T17=_T3C->f1;_T18=_T17.ptr_atts;_T33=_T18.autoreleased;_T19=_T3C->f1;_T1A=_T19.ptr_atts;_T32=_T1A.aqual;}{void*et=_T3B;struct Cyc_Absyn_Tqual tq=_T3A;void*r=_T39;void*n=_T38;void*b=_T36;void*zt=_T35;struct Cyc_Absyn_PtrLoc*pl=_T34;void*rel=_T33;void*aq=_T32;
# 387
void*et2=Cyc_Parse_substitute_tags(tags,et);
void*b2=Cyc_Parse_substitute_tags(tags,b);
if(et2!=et)goto _TL64;else{goto _TL65;}_TL65: if(b2!=b)goto _TL64;else{goto _TL62;}
_TL64:{struct Cyc_Absyn_PtrInfo _T3C;_T3C.elt_type=et2;_T3C.elt_tq=tq;_T3C.ptr_atts.eff=r;_T3C.ptr_atts.nullable=n;_T3C.ptr_atts.bounds=b2;_T3C.ptr_atts.zero_term=zt;_T3C.ptr_atts.ptrloc=pl;_T3C.ptr_atts.autoreleased=rel;_T3C.ptr_atts.aqual=aq;_T1B=_T3C;}_T1C=Cyc_Absyn_pointer_type(_T1B);return _T1C;_TL62: goto _LL0;}case 0: _T1D=t;_T1E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T1D;_T1F=_T1E->f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=13)goto _TL66;_T22=t;_T23=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T22;_T24=_T23->f2;if(_T24==0)goto _TL68;_T25=t;_T26=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T25;_T27=_T26->f2;_T28=(struct Cyc_List_List*)_T27;_T29=_T28->tl;if(_T29!=0)goto _TL6A;_T2A=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T2A;_T2B=_T3C->f2;{struct Cyc_List_List _T3D=*_T2B;_T2C=_T3D.hd;_T3B=(void*)_T2C;}}{void*t=_T3B;
# 393
void*t2=Cyc_Parse_substitute_tags(tags,t);
if(t==t2)goto _TL6C;_T2D=Cyc_Absyn_thin_bounds_type(t2);return _T2D;_TL6C: goto _LL0;}_TL6A: goto _LL9;_TL68: goto _LL9;_TL66: goto _LL9;case 9: _T2E=t;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T3C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T2E;_T31=_T3C->f1;}{struct Cyc_Absyn_Exp*e=_T31;
# 397
struct Cyc_Absyn_Exp*e2=Cyc_Parse_substitute_tags_exp(tags,e);
if(e2==e)goto _TL6E;_T2F=Cyc_Absyn_valueof_type(e2);return _T2F;_TL6E: goto _LL0;}default: _LL9: goto _LL0;}_LL0:;}_T30=t;
# 402
return _T30;}
# 407
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){struct Cyc_Absyn_Aggrfield*_T0;struct Cyc_List_List*_T1;struct Cyc_Absyn_Aggrfield*_T2;void*_T3;_T0=x;_T1=tags;_T2=x;_T3=_T2->type;
_T0->type=Cyc_Parse_substitute_tags(_T1,_T3);}struct _tuple19{struct Cyc_Absyn_Tqual f0;void*f1;};struct _tuple20{void*f0;struct Cyc_Absyn_Tqual f1;void*f2;};
# 414
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple20*t){struct _tuple19*_T0;struct _tuple20*_T1;struct _tuple20 _T2;struct _tuple20*_T3;struct _tuple20 _T4;{struct _tuple19*_T5=_cycalloc(sizeof(struct _tuple19));_T1=t;_T2=*_T1;
_T5->f0=_T2.f1;_T3=t;_T4=*_T3;_T5->f1=_T4.f2;_T0=(struct _tuple19*)_T5;}return _T0;}
# 418
static int Cyc_Parse_is_typeparam(void*tm){void*_T0;int*_T1;int _T2;_T0=tm;_T1=(int*)_T0;_T2=*_T1;if(_T2!=4)goto _TL70;
# 420
return 1;_TL70:
 return 0;;}
# 427
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k,void*aliashint,unsigned loc){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;void*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;int _T6;void*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;int _TA;void*_TB;int _TC;void*_TD;struct Cyc_Absyn_Tvar*_TE;struct _fat_ptr*_TF;void*_T10;_T0=s;_T1=
_tag_fat("`H",sizeof(char),3U);_T2=Cyc_zstrcmp(_T0,_T1);if(_T2!=0)goto _TL72;_T3=Cyc_Absyn_heap_rgn_type;
return _T3;_TL72: _T4=s;_T5=
# 431
_tag_fat("`U",sizeof(char),3U);_T6=Cyc_zstrcmp(_T4,_T5);if(_T6!=0)goto _TL74;_T7=Cyc_Absyn_unique_rgn_shorthand_type;
return _T7;_TL74: _T8=s;_T9=
_tag_fat("`RC",sizeof(char),4U);_TA=Cyc_zstrcmp(_T8,_T9);if(_TA!=0)goto _TL76;_TB=Cyc_Absyn_refcnt_rgn_shorthand_type;
return _TB;_TL76: _TC=
Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name);if(_TC!=0)goto _TL78;_TD=
Cyc_CurRgn_curr_rgn_type();return _TD;_TL78:
 aliashint=Cyc_Kinds_consistent_aliashint(loc,k,aliashint);{struct Cyc_Absyn_Tvar*_T11=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));{struct _fat_ptr*_T12=_cycalloc(sizeof(struct _fat_ptr));
*_T12=s;_TF=(struct _fat_ptr*)_T12;}_T11->name=_TF;_T11->identity=-1;_T11->kind=k;_T11->aquals_bound=aliashint;_TE=(struct Cyc_Absyn_Tvar*)_T11;}_T10=Cyc_Absyn_var_type(_TE);return _T10;}
# 441
static void*Cyc_Parse_id2aqual(unsigned loc,struct _fat_ptr s){unsigned long _T0;struct _fat_ptr _T1;unsigned char*_T2;const char*_T3;int _T4;void*_T5;void*_T6;void*_T7;unsigned long _T8;struct _fat_ptr _T9;unsigned char*_TA;const char*_TB;char _TC;int _TD;struct _fat_ptr _TE;unsigned char*_TF;const char*_T10;char _T11;int _T12;void*_T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;unsigned _T16;struct _fat_ptr _T17;void*_T18;_T0=
Cyc_strlen(s);if(_T0!=2U)goto _TL7A;_T1=s;_T2=_T1.curr;_T3=(const char*)_T2;{
char _T19=_T3[1];_T4=(int)_T19;switch(_T4){case 65: _T5=Cyc_Absyn_al_qual_type;
return _T5;case 85: _T6=Cyc_Absyn_un_qual_type;
return _T6;case 84: _T7=Cyc_Absyn_rtd_qual_type;
return _T7;default: goto _LL0;}_LL0:;}goto _TL7B;
# 450
_TL7A: _T8=Cyc_strlen(s);if(_T8!=3U)goto _TL7D;_T9=s;_TA=_T9.curr;_TB=(const char*)_TA;_TC=_TB[1];_TD=(int)_TC;
if(_TD!=82)goto _TL7F;_TE=s;_TF=_TE.curr;_T10=(const char*)_TF;_T11=_T10[2];_T12=(int)_T11;if(_T12!=67)goto _TL7F;_T13=Cyc_Absyn_rc_qual_type;
return _T13;_TL7F: goto _TL7E;_TL7D: _TL7E: _TL7B:{struct Cyc_Warn_String_Warn_Warg_struct _T19;_T19.tag=0;
# 454
_T19.f1=_tag_fat("bad aqual bound ",sizeof(char),17U);_T14=_T19;}{struct Cyc_Warn_String_Warn_Warg_struct _T19=_T14;{struct Cyc_Warn_String_Warn_Warg_struct _T1A;_T1A.tag=0;_T1A.f1=s;_T15=_T1A;}{struct Cyc_Warn_String_Warn_Warg_struct _T1A=_T15;void*_T1B[2];_T1B[0]=& _T19;_T1B[1]=& _T1A;_T16=loc;_T17=_tag_fat(_T1B,sizeof(void*),2);Cyc_Warn_err2(_T16,_T17);}}_T18=Cyc_Absyn_al_qual_type;
return _T18;}
# 458
static struct Cyc_List_List*Cyc_Parse_insert_aqual(struct _RegionHandle*yy,struct Cyc_List_List*qlist,void*aq,unsigned loc){struct Cyc_List_List*_T0;int*_T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;unsigned _T4;struct _fat_ptr _T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct _RegionHandle*_T9;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_TA;struct _RegionHandle*_TB;{
struct Cyc_List_List*l=qlist;_TL84: if(l!=0)goto _TL82;else{goto _TL83;}
_TL82: _T0=l;{void*_TC=_T0->hd;_T1=(int*)_TC;_T2=*_T1;if(_T2!=10)goto _TL85;{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;
# 462
_TD.f1=_tag_fat("Multiple alias qualifiers",sizeof(char),26U);_T3=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T3;void*_TE[1];_TE[0]=& _TD;_T4=loc;_T5=_tag_fat(_TE,sizeof(void*),1);Cyc_Warn_err2(_T4,_T5);}_T6=qlist;
return _T6;_TL85: goto _LL0;_LL0:;}_T7=l;
# 459
l=_T7->tl;goto _TL84;_TL83:;}_T9=yy;{struct Cyc_List_List*_TC=_region_malloc(_T9,0U,sizeof(struct Cyc_List_List));_TB=yy;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_TD=_region_malloc(_TB,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_TD->tag=10;
# 468
_TD->f1=aq;_TA=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_TD;}_TC->hd=(void*)_TA;_TC->tl=qlist;_T8=(struct Cyc_List_List*)_TC;}return _T8;}
# 471
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;unsigned _T3;struct _fat_ptr _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;int _T8;unsigned _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct _fat_ptr _TD;int _TE;unsigned _TF;struct _fat_ptr _T10;struct _fat_ptr _T11;int _T12;unsigned _T13;struct _fat_ptr _T14;struct _fat_ptr _T15;_T0=s;_T1=
_tag_fat("`H",sizeof(char),3U);_T2=Cyc_zstrcmp(_T0,_T1);if(_T2!=0)goto _TL87;_T3=loc;_T4=
_tag_fat("bad occurrence of heap region",sizeof(char),30U);_T5=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T3,_T4,_T5);goto _TL88;_TL87: _TL88: _T6=s;_T7=
_tag_fat("`U",sizeof(char),3U);_T8=Cyc_zstrcmp(_T6,_T7);if(_T8!=0)goto _TL89;_T9=loc;_TA=
_tag_fat("bad occurrence of unique region",sizeof(char),32U);_TB=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T9,_TA,_TB);goto _TL8A;_TL89: _TL8A: _TC=s;_TD=
_tag_fat("`RC",sizeof(char),4U);_TE=Cyc_zstrcmp(_TC,_TD);if(_TE!=0)goto _TL8B;_TF=loc;_T10=
_tag_fat("bad occurrence of refcounted region",sizeof(char),36U);_T11=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TF,_T10,_T11);goto _TL8C;_TL8B: _TL8C: _T12=
Cyc_zstrcmp(s,Cyc_CurRgn_curr_rgn_name);if(_T12!=0)goto _TL8D;_T13=loc;_T14=
_tag_fat("bad occurrence of \"current\" region",sizeof(char),35U);_T15=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T13,_T14,_T15);goto _TL8E;_TL8D: _TL8E:;}
# 486
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Absyn_Tvar*_T4;int(*_T5)(unsigned,struct _fat_ptr);unsigned _T6;struct _fat_ptr _T7;struct Cyc_Absyn_Tvar*_T8;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=2)goto _TL8F;_T3=t;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T3;_T8=_T9->f1;}{struct Cyc_Absyn_Tvar*pr=_T8;_T4=pr;
# 488
return _T4;}_TL8F:{
int(*_T9)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T5=_T9;}_T6=loc;_T7=_tag_fat("expecting a list of type variables, not types",sizeof(char),46U);_T5(_T6,_T7);;}
# 494
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){int*_T0;int _T1;struct Cyc_Absyn_Tvar*_T2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T3;struct Cyc_Absyn_Tvar*_T4;void**_T5;void**_T6;void*_T7;int*_T8;int _T9;void**_TA;void*_TB;int _TC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TD;
void*_TE=Cyc_Absyn_compress(t);void*_TF;_T0=(int*)_TE;_T1=*_T0;if(_T1!=2)goto _TL91;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T10=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TE;_T2=_T10->f1;{struct Cyc_Absyn_Tvar _T11=*_T2;_T3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_TE;_T4=_T3->f1;_T5=& _T4->kind;_TF=(void**)_T5;}}{void**cptr=(void**)_TF;_T6=cptr;_T7=*_T6;{
# 497
void*_T10=Cyc_Kinds_compress_kb(_T7);_T8=(int*)_T10;_T9=*_T8;if(_T9!=1)goto _TL93;_TA=cptr;_TC=leq;
# 499
if(!_TC)goto _TL95;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T11=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_T11->tag=2;_T11->f1=0;_T11->f2=k;_TD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T11;}_TB=(void*)_TD;goto _TL96;_TL95: _TB=Cyc_Kinds_kind_to_bound(k);_TL96:*_TA=_TB;return;_TL93:
 return;;}}goto _TL92;_TL91:
# 502
 return;_TL92:;}
# 507
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){struct Cyc_List_List*_T0;int*_T1;int _T2;void*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;void*_T8;int _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;void*_TE;int*_TF;int _T10;unsigned _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;struct Cyc_List_List*_T14;void*_T15;int _T16;int _T17;int(*_T18)(unsigned,struct _fat_ptr);unsigned _T19;struct _fat_ptr _T1A;struct Cyc_List_List*_T1B;void*_T1C;struct Cyc_Absyn_Decl*_T1D;int*_T1E;int _T1F;struct Cyc_Absyn_Vardecl*_T20;struct _tuple0*_T21;struct _tuple0 _T22;struct _fat_ptr*_T23;struct Cyc_List_List*_T24;void*_T25;struct _fat_ptr*_T26;int _T27;struct Cyc_Absyn_Vardecl*_T28;struct Cyc_Absyn_Exp*_T29;int(*_T2A)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Decl*_T2B;unsigned _T2C;struct _fat_ptr _T2D;struct Cyc_Absyn_Vardecl*_T2E;struct _tuple0*_T2F;int _T30;int(*_T31)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Decl*_T32;unsigned _T33;struct _fat_ptr _T34;struct Cyc_List_List*_T35;struct _tuple8*_T36;struct Cyc_Absyn_Vardecl*_T37;struct _tuple0*_T38;struct _tuple0 _T39;struct Cyc_Absyn_Vardecl*_T3A;struct Cyc_Absyn_Vardecl*_T3B;int(*_T3C)(unsigned,struct _fat_ptr);struct Cyc_Absyn_Decl*_T3D;unsigned _T3E;struct _fat_ptr _T3F;struct Cyc_List_List*_T40;int(*_T41)(unsigned,struct _fat_ptr);unsigned _T42;struct Cyc_List_List*_T43;void*_T44;struct _fat_ptr*_T45;struct _fat_ptr _T46;struct _fat_ptr _T47;struct _fat_ptr _T48;struct Cyc_List_List*_T49;struct Cyc_List_List*_T4A;struct _RegionHandle*_T4B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T4C;struct _RegionHandle*_T4D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T4E;struct _RegionHandle*_T4F;struct Cyc_List_List*_T50;struct _RegionHandle*_T51;struct Cyc_List_List*_T52;struct _RegionHandle*_T53;struct Cyc_List_List*_T54;struct Cyc_List_List*_T55;struct Cyc_List_List*_T56;unsigned _T57;
# 515
if(tms!=0)goto _TL97;return 0;_TL97: _T0=tms;{
# 517
void*_T58=_T0->hd;void*_T59;_T1=(int*)_T58;_T2=*_T1;if(_T2!=3)goto _TL99;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T5A=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T58;_T3=_T5A->f1;_T59=(void*)_T3;}{void*args=_T59;_T4=tms;_T5=_T4->tl;
# 520
if(_T5==0)goto _TL9D;else{goto _TL9E;}_TL9E: _T6=tms;_T7=_T6->tl;_T8=_T7->hd;_T9=
Cyc_Parse_is_typeparam(_T8);
# 520
if(_T9)goto _TL9F;else{goto _TL9B;}_TL9F: _TA=tms;_TB=_TA->tl;_TC=
_check_null(_TB);_TD=_TC->tl;
# 520
if(_TD==0)goto _TL9D;else{goto _TL9B;}
# 523
_TL9D:{struct Cyc_List_List*_T5A;_TE=args;_TF=(int*)_TE;_T10=*_TF;if(_T10!=1)goto _TLA0;_T11=loc;_T12=
# 526
_tag_fat("function declaration with both new- and old-style parameter declarations; ignoring old-style",sizeof(char),93U);_T13=_tag_fat(0U,sizeof(void*),0);
# 525
Cyc_Warn_warn(_T11,_T12,_T13);_T14=tms;
# 527
return _T14;_TLA0: _T15=args;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T5B=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T15;_T5A=_T5B->f1;}{struct Cyc_List_List*ids=_T5A;_T16=
# 529
Cyc_List_length(ids);_T17=Cyc_List_length(tds);if(_T16==_T17)goto _TLA2;{
int(*_T5B)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T18=_T5B;}_T19=loc;_T1A=
_tag_fat("wrong number of parameter declarations in old-style function declaration",sizeof(char),73U);
# 530
_T18(_T19,_T1A);goto _TLA3;_TLA2: _TLA3: {
# 533
struct Cyc_List_List*rev_new_params=0;
_TLA7: if(ids!=0)goto _TLA5;else{goto _TLA6;}
_TLA5:{struct Cyc_List_List*tds2=tds;
_TLAB: if(tds2!=0)goto _TLA9;else{goto _TLAA;}
_TLA9: _T1B=tds2;_T1C=_T1B->hd;{struct Cyc_Absyn_Decl*x=(struct Cyc_Absyn_Decl*)_T1C;_T1D=x;{
void*_T5B=_T1D->r;struct Cyc_Absyn_Vardecl*_T5C;_T1E=(int*)_T5B;_T1F=*_T1E;if(_T1F!=0)goto _TLAC;{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T5D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T5B;_T5C=_T5D->f1;}{struct Cyc_Absyn_Vardecl*vd=_T5C;_T20=vd;_T21=_T20->name;_T22=*_T21;_T23=_T22.f1;_T24=ids;_T25=_T24->hd;_T26=(struct _fat_ptr*)_T25;_T27=
# 540
Cyc_zstrptrcmp(_T23,_T26);if(_T27==0)goto _TLAE;goto _TLA8;_TLAE: _T28=vd;_T29=_T28->initializer;
# 542
if(_T29==0)goto _TLB0;{
int(*_T5D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T2A=_T5D;}_T2B=x;_T2C=_T2B->loc;_T2D=_tag_fat("initializer found in parameter declaration",sizeof(char),43U);_T2A(_T2C,_T2D);goto _TLB1;_TLB0: _TLB1: _T2E=vd;_T2F=_T2E->name;_T30=
Cyc_Absyn_is_qvar_qualified(_T2F);if(!_T30)goto _TLB2;{
int(*_T5D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T31=_T5D;}_T32=x;_T33=_T32->loc;_T34=_tag_fat("namespaces forbidden in parameter declarations",sizeof(char),47U);_T31(_T33,_T34);goto _TLB3;_TLB2: _TLB3:{struct Cyc_List_List*_T5D=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple8*_T5E=_cycalloc(sizeof(struct _tuple8));_T37=vd;_T38=_T37->name;_T39=*_T38;
_T5E->f0=_T39.f1;_T3A=vd;_T5E->f1=_T3A->tq;_T3B=vd;_T5E->f2=_T3B->type;_T36=(struct _tuple8*)_T5E;}_T5D->hd=_T36;
_T5D->tl=rev_new_params;_T35=(struct Cyc_List_List*)_T5D;}
# 546
rev_new_params=_T35;goto L;}_TLAC:{
# 549
int(*_T5D)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T3C=_T5D;}_T3D=x;_T3E=_T3D->loc;_T3F=_tag_fat("nonvariable declaration in parameter type",sizeof(char),42U);_T3C(_T3E,_T3F);;}}_TLA8: _T40=tds2;
# 536
tds2=_T40->tl;goto _TLAB;_TLAA:
# 552
 L: if(tds2!=0)goto _TLB4;{
int(*_T5B)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T41=_T5B;}_T42=loc;_T43=ids;_T44=_T43->hd;_T45=(struct _fat_ptr*)_T44;_T46=*_T45;_T47=_tag_fat(" is not given a type",sizeof(char),21U);_T48=Cyc_strconcat(_T46,_T47);_T41(_T42,_T48);goto _TLB5;_TLB4: _TLB5:;}_T49=ids;
# 534
ids=_T49->tl;goto _TLA7;_TLA6: _T4B=yy;{struct Cyc_List_List*_T5B=_region_malloc(_T4B,0U,sizeof(struct Cyc_List_List));_T4D=yy;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T5C=_region_malloc(_T4D,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T5C->tag=3;_T4F=yy;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T5D=_region_malloc(_T4F,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T5D->tag=1;
# 556
_T5D->f1=Cyc_List_imp_rev(rev_new_params);
_T5D->f2=0;_T5D->f3=0;_T5D->f4=0;_T5D->f5=0;_T5D->f6=0;_T5D->f7=0;_T5D->f8=0;_T5D->f9=0;_T5D->f10=0;_T4E=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T5D;}
# 556
_T5C->f1=(void*)_T4E;_T4C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T5C;}_T5B->hd=(void*)_T4C;
# 558
_T5B->tl=0;_T4A=(struct Cyc_List_List*)_T5B;}
# 555
return _T4A;}};}goto _TL9C;_TL9B: _TL9C: goto _LL4;}_TL99: _LL4: _T51=yy;{struct Cyc_List_List*_T5A=_region_malloc(_T51,0U,sizeof(struct Cyc_List_List));_T52=tms;
# 563
_T5A->hd=_T52->hd;_T53=yy;_T54=tms;_T55=_T54->tl;_T56=tds;_T57=loc;_T5A->tl=Cyc_Parse_oldstyle2newstyle(_T53,_T55,_T56,_T57);_T50=(struct Cyc_List_List*)_T5A;}return _T50;;}}
# 569
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){struct Cyc_Parse_Declarator _T0;struct Cyc_Parse_Declarator _T1;struct Cyc_Parse_Declarator _T2;struct _RegionHandle*_T3;struct Cyc_Parse_Declarator _T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;unsigned _T7;struct Cyc_Parse_Declaration_spec*_T8;struct Cyc_Parse_Declaration_spec*_T9;struct Cyc_Parse_Declaration_spec*_TA;struct Cyc_Parse_Declaration_spec*_TB;struct Cyc_Parse_Declaration_spec*_TC;int _TD;unsigned _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;struct Cyc_Absyn_Tqual _T11;void*_T12;struct Cyc_List_List*_T13;struct Cyc_Parse_Declarator _T14;struct Cyc_List_List*_T15;unsigned _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;void*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Absyn_FnInfo _T1D;struct Cyc_List_List*_T1E;void*_T1F;struct _tuple8*_T20;struct _tuple8 _T21;struct _fat_ptr*_T22;unsigned _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_List_List*_T26;void*_T27;struct _tuple8*_T28;struct _fat_ptr*_T29;struct Cyc_List_List*_T2A;struct Cyc_Absyn_FnInfo _T2B;struct Cyc_List_List*_T2C;struct Cyc_List_List*_T2D;struct Cyc_Absyn_Fndecl*_T2E;struct Cyc_Parse_Declarator _T2F;int(*_T30)(unsigned,struct _fat_ptr);unsigned _T31;struct _fat_ptr _T32;
# 573
if(tds==0)goto _TLB6;{struct Cyc_Parse_Declarator _T33;_T1=d;
_T33.id=_T1.id;_T2=d;_T33.varloc=_T2.varloc;_T3=yy;_T4=d;_T5=_T4.tms;_T6=tds;_T7=loc;_T33.tms=Cyc_Parse_oldstyle2newstyle(_T3,_T5,_T6,_T7);_T0=_T33;}d=_T0;goto _TLB7;_TLB6: _TLB7: {
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 581
if(dso==0)goto _TLB8;_T8=dso;
tss=_T8->type_specs;_T9=dso;
tq=_T9->tq;_TA=dso;
is_inline=_TA->is_inline;_TB=dso;
atts=_TB->attributes;_TC=dso;{
# 587
enum Cyc_Parse_Storage_class _T33=_TC->sc;_TD=(int)_T33;switch(_TD){case Cyc_Parse_None_sc: goto _LL0;case Cyc_Parse_Extern_sc:
# 589
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default: _TE=loc;_TF=
_tag_fat("bad storage class on function",sizeof(char),30U);_T10=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TE,_TF,_T10);goto _LL0;}_LL0:;}goto _TLB9;_TLB8: _TLB9: {
# 594
void*t=Cyc_Parse_collapse_type_specifiers(tss,loc);_T11=tq;_T12=t;_T13=atts;_T14=d;_T15=_T14.tms;{
struct _tuple14 _T33=Cyc_Parse_apply_tms(_T11,_T12,_T13,_T15);struct Cyc_List_List*_T34;struct Cyc_List_List*_T35;void*_T36;struct Cyc_Absyn_Tqual _T37;_T37=_T33.f0;_T36=_T33.f1;_T35=_T33.f2;_T34=_T33.f3;{struct Cyc_Absyn_Tqual fn_tqual=_T37;void*fn_type=_T36;struct Cyc_List_List*x=_T35;struct Cyc_List_List*out_atts=_T34;
# 599
if(x==0)goto _TLBB;_T16=loc;_T17=
_tag_fat("bad type params, ignoring",sizeof(char),26U);_T18=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T16,_T17,_T18);goto _TLBC;_TLBB: _TLBC: {struct Cyc_Absyn_FnInfo _T38;_T19=fn_type;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=6)goto _TLBD;_T1C=fn_type;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T39=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T1C;_T38=_T39->f1;}{struct Cyc_Absyn_FnInfo i=_T38;_T1D=i;{
# 604
struct Cyc_List_List*args2=_T1D.args;_TLC2: if(args2!=0)goto _TLC0;else{goto _TLC1;}
_TLC0: _T1E=args2;_T1F=_T1E->hd;_T20=(struct _tuple8*)_T1F;_T21=*_T20;_T22=_T21.f0;if(_T22!=0)goto _TLC3;_T23=loc;_T24=
_tag_fat("missing argument variable in function prototype",sizeof(char),48U);_T25=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T23,_T24,_T25);_T26=args2;_T27=_T26->hd;_T28=(struct _tuple8*)_T27;{struct _fat_ptr*_T39=_cycalloc(sizeof(struct _fat_ptr));
*_T39=_tag_fat("?",sizeof(char),2U);_T29=(struct _fat_ptr*)_T39;}(*_T28).f0=_T29;goto _TLC4;_TLC3: _TLC4: _T2A=args2;
# 604
args2=_T2A->tl;goto _TLC2;_TLC1:;}_T2B=i;_T2C=_T2B.attributes;_T2D=out_atts;
# 615
i.attributes=Cyc_List_append(_T2C,_T2D);{struct Cyc_Absyn_Fndecl*_T39=_cycalloc(sizeof(struct Cyc_Absyn_Fndecl));
_T39->sc=sc;_T39->is_inline=is_inline;_T2F=d;_T39->name=_T2F.id;_T39->body=body;_T39->i=i;
_T39->cached_type=0;_T39->param_vardecls=0;_T39->fn_vardecl=0;
_T39->orig_scope=sc;_T39->escapes=0;_T2E=(struct Cyc_Absyn_Fndecl*)_T39;}
# 616
return _T2E;}_TLBD:{
# 619
int(*_T39)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T30=_T39;}_T31=loc;_T32=_tag_fat("declarator is not a function prototype",sizeof(char),39U);_T30(_T31,_T32);;}}}}}}static char _TmpG0[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 623
static struct _fat_ptr Cyc_Parse_msg1={_TmpG0,_TmpG0,_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 625
static struct _fat_ptr Cyc_Parse_msg2={_TmpG1,_TmpG1,_TmpG1 + 84U};
# 632
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){struct Cyc_Parse_Type_specifier _T0;int _T1;struct Cyc_Parse_Type_specifier _T2;int _T3;unsigned _T4;struct _fat_ptr _T5;struct _fat_ptr _T6;struct Cyc_Parse_Type_specifier _T7;int _T8;struct Cyc_Parse_Type_specifier _T9;int _TA;struct Cyc_Parse_Type_specifier _TB;int _TC;unsigned _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_Parse_Type_specifier _T10;int _T11;struct Cyc_Parse_Type_specifier _T12;int _T13;struct Cyc_Parse_Type_specifier _T14;int _T15;unsigned _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;struct Cyc_Parse_Type_specifier _T19;int _T1A;struct Cyc_Parse_Type_specifier _T1B;int _T1C;struct Cyc_Parse_Type_specifier _T1D;int _T1E;unsigned _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;struct Cyc_Parse_Type_specifier _T22;int _T23;struct Cyc_Parse_Type_specifier _T24;int _T25;struct Cyc_Parse_Type_specifier _T26;int _T27;struct Cyc_Parse_Type_specifier _T28;int _T29;struct Cyc_Parse_Type_specifier _T2A;int _T2B;struct Cyc_Parse_Type_specifier _T2C;int _T2D;struct Cyc_Parse_Type_specifier _T2E;int _T2F;unsigned _T30;struct _fat_ptr _T31;struct _fat_ptr _T32;int _T33;struct Cyc_Parse_Type_specifier _T34;int _T35;struct Cyc_Parse_Type_specifier _T36;int _T37;int _T38;struct Cyc_Parse_Type_specifier _T39;int _T3A;struct Cyc_Parse_Type_specifier _T3B;int _T3C;struct Cyc_Parse_Type_specifier _T3D;int _T3E;int _T3F;struct Cyc_Parse_Type_specifier _T40;int _T41;struct Cyc_Parse_Type_specifier _T42;struct Cyc_Parse_Type_specifier _T43;int _T44;struct Cyc_Parse_Type_specifier _T45;int _T46;unsigned _T47;struct _fat_ptr _T48;struct _fat_ptr _T49;struct Cyc_Parse_Type_specifier _T4A;int _T4B;struct Cyc_Parse_Type_specifier _T4C;struct Cyc_Parse_Type_specifier _T4D;_T0=s1;_T1=_T0.Signed_spec;
# 635
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
# 650
_TLCF: _T30=loc;_T31=_tag_fat("too many occurrences of long in specifiers",sizeof(char),43U);_T32=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T30,_T31,_T32);goto _TLCE;_TLCD: _TLCE: _T34=s1;_T35=_T34.Long_Long_spec;
# 652
if(_T35)goto _TLD7;else{goto _TLD8;}_TLD8: _T36=s2;_T37=_T36.Long_Long_spec;if(_T37)goto _TLD7;else{goto _TLD5;}_TLD7: _T33=1;goto _TLD6;_TLD5: _T39=s1;_T3A=_T39.Long_spec;if(!_T3A)goto _TLD9;_T3B=s2;_T38=_T3B.Long_spec;goto _TLDA;_TLD9: _T38=0;_TLDA: _T33=_T38;_TLD6:
# 651
 s1.Long_Long_spec=_T33;_T3D=s1;_T3E=_T3D.Long_Long_spec;
# 653
if(_T3E)goto _TLDB;else{goto _TLDD;}_TLDD: _T40=s1;_T41=_T40.Long_spec;if(!_T41)goto _TLDE;_T3F=1;goto _TLDF;_TLDE: _T42=s2;_T3F=_T42.Long_spec;_TLDF: _T3C=_T3F;goto _TLDC;_TLDB: _T3C=0;_TLDC: s1.Long_spec=_T3C;_T43=s1;_T44=_T43.Valid_type_spec;
if(!_T44)goto _TLE0;_T45=s2;_T46=_T45.Valid_type_spec;if(!_T46)goto _TLE0;_T47=loc;_T48=Cyc_Parse_msg1;_T49=_tag_fat(0U,sizeof(void*),0);
Cyc_Warn_err(_T47,_T48,_T49);goto _TLE1;
_TLE0: _T4A=s2;_T4B=_T4A.Valid_type_spec;if(!_T4B)goto _TLE2;_T4C=s2;
s1.Type_spec=_T4C.Type_spec;
s1.Valid_type_spec=1;goto _TLE3;_TLE2: _TLE3: _TLE1: _T4D=s1;
# 660
return _T4D;}
# 666
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){struct Cyc_Parse_Type_specifier _T0;int _T1;struct Cyc_Parse_Type_specifier _T2;int _T3;struct Cyc_Parse_Type_specifier _T4;int _T5;struct Cyc_Parse_Type_specifier _T6;int _T7;struct Cyc_Parse_Type_specifier _T8;int _T9;struct Cyc_Parse_Type_specifier _TA;void*_TB;int _TC;struct Cyc_Parse_Type_specifier _TD;int _TE;int _TF;struct Cyc_Parse_Type_specifier _T10;int _T11;struct Cyc_Parse_Type_specifier _T12;int _T13;unsigned _T14;struct _fat_ptr _T15;struct _fat_ptr _T16;struct Cyc_Parse_Type_specifier _T17;int _T18;struct Cyc_Parse_Type_specifier _T19;int _T1A;struct Cyc_Parse_Type_specifier _T1B;int _T1C;struct Cyc_Parse_Type_specifier _T1D;int _T1E;struct Cyc_Parse_Type_specifier _T1F;int _T20;struct Cyc_Parse_Type_specifier _T21;int _T22;unsigned _T23;struct _fat_ptr _T24;struct _fat_ptr _T25;struct Cyc_Parse_Type_specifier _T26;int _T27;struct Cyc_Parse_Type_specifier _T28;int _T29;struct Cyc_Parse_Type_specifier _T2A;int _T2B;int _T2C;int _T2D;int _T2E;struct Cyc_Parse_Type_specifier _T2F;int _T30;void*_T31;unsigned _T32;struct _fat_ptr _T33;struct _fat_ptr _T34;struct Cyc_Parse_Type_specifier _T35;int _T36;void*_T37;void*_T38;void*_T39;void*_T3A;int*_T3B;int _T3C;void*_T3D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3E;void*_T3F;int*_T40;unsigned _T41;void*_T42;void*_T43;int _T44;enum Cyc_Absyn_Sign _T45;int _T46;enum Cyc_Absyn_Sign _T47;int _T48;int _T49;enum Cyc_Absyn_Size_of _T4A;int _T4B;enum Cyc_Absyn_Size_of _T4C;int _T4D;struct Cyc_Parse_Type_specifier _T4E;int _T4F;int _T50;struct Cyc_Parse_Type_specifier _T51;int _T52;int _T53;unsigned _T54;struct _fat_ptr _T55;struct _fat_ptr _T56;int _T57;unsigned _T58;struct _fat_ptr _T59;struct _fat_ptr _T5A;void*_T5B;_T0=ts;{
int seen_type=_T0.Valid_type_spec;_T2=ts;_T3=_T2.Signed_spec;
if(!_T3)goto _TLE4;_T1=1;goto _TLE5;_TLE4: _T4=ts;_T1=_T4.Unsigned_spec;_TLE5: {int seen_sign=_T1;_T6=ts;_T7=_T6.Short_spec;
if(_T7)goto _TLE8;else{goto _TLE9;}_TLE9: _T8=ts;_T9=_T8.Long_spec;if(_T9)goto _TLE8;else{goto _TLE6;}_TLE8: _T5=1;goto _TLE7;_TLE6: _TA=ts;_T5=_TA.Long_Long_spec;_TLE7: {int seen_size=_T5;_TC=seen_type;
if(!_TC)goto _TLEA;_TD=ts;_TB=_TD.Type_spec;goto _TLEB;_TLEA: _TB=Cyc_Absyn_void_type;_TLEB: {void*t=_TB;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;_TE=seen_size;
# 674
if(_TE)goto _TLEE;else{goto _TLEF;}_TLEF: _TF=seen_sign;if(_TF)goto _TLEE;else{goto _TLEC;}
_TLEE: _T10=ts;_T11=_T10.Signed_spec;if(!_T11)goto _TLF0;_T12=ts;_T13=_T12.Unsigned_spec;if(!_T13)goto _TLF0;_T14=loc;_T15=Cyc_Parse_msg2;_T16=_tag_fat(0U,sizeof(void*),0);
Cyc_Warn_err(_T14,_T15,_T16);goto _TLF1;_TLF0: _TLF1: _T17=ts;_T18=_T17.Unsigned_spec;
if(!_T18)goto _TLF2;sgn=1U;goto _TLF3;_TLF2: _TLF3: _T19=ts;_T1A=_T19.Short_spec;
if(_T1A)goto _TLF8;else{goto _TLF7;}_TLF8: _T1B=ts;_T1C=_T1B.Long_spec;if(_T1C)goto _TLF6;else{goto _TLF9;}_TLF9: _T1D=ts;_T1E=_T1D.Long_Long_spec;if(_T1E)goto _TLF6;else{goto _TLF7;}_TLF7: _T1F=ts;_T20=_T1F.Long_spec;if(_T20)goto _TLFA;else{goto _TLF4;}_TLFA: _T21=ts;_T22=_T21.Long_Long_spec;if(_T22)goto _TLF6;else{goto _TLF4;}
# 680
_TLF6: _T23=loc;_T24=Cyc_Parse_msg2;_T25=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T23,_T24,_T25);goto _TLF5;_TLF4: _TLF5: _T26=ts;_T27=_T26.Short_spec;
if(!_T27)goto _TLFB;sz=1U;goto _TLFC;_TLFB: _TLFC: _T28=ts;_T29=_T28.Long_spec;
if(!_T29)goto _TLFD;sz=3U;goto _TLFE;_TLFD: _TLFE: _T2A=ts;_T2B=_T2A.Long_Long_spec;
if(!_T2B)goto _TLFF;sz=4U;goto _TL100;_TLFF: _TL100: goto _TLED;_TLEC: _TLED: _T2C=seen_type;
# 687
if(_T2C)goto _TL101;else{goto _TL103;}
_TL103: _T2D=seen_sign;if(_T2D)goto _TL104;else{goto _TL106;}_TL106: _T2E=seen_size;if(_T2E)goto _TL104;else{goto _TL107;}
_TL107: _T2F=ts;_T30=_T2F.Complex_spec;if(!_T30)goto _TL108;_T31=
Cyc_Absyn_complex_type(Cyc_Absyn_double_type);return _T31;_TL108: _T32=loc;_T33=
_tag_fat("missing type within specifier",sizeof(char),30U);_T34=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T32,_T33,_T34);goto _TL105;_TL104: _TL105: _T35=ts;_T36=_T35.Complex_spec;
# 693
if(!_T36)goto _TL10A;_T37=
Cyc_Absyn_int_type(sgn,sz);_T38=Cyc_Absyn_complex_type(_T37);return _T38;
_TL10A: _T39=Cyc_Absyn_int_type(sgn,sz);return _T39;_TL101:{enum Cyc_Absyn_Size_of _T5C;enum Cyc_Absyn_Sign _T5D;_T3A=t;_T3B=(int*)_T3A;_T3C=*_T3B;if(_T3C!=0)goto _TL10C;_T3D=t;_T3E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3D;_T3F=_T3E->f1;_T40=(int*)_T3F;_T41=*_T40;switch(_T41){case 1: _T42=t;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T5E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T42;_T43=_T5E->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T5F=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T43;_T5D=_T5F->f1;_T5C=_T5F->f2;}}{enum Cyc_Absyn_Sign sgn2=_T5D;enum Cyc_Absyn_Size_of sz2=_T5C;_T44=seen_sign;
# 699
if(!_T44)goto _TL10F;_T45=sgn2;_T46=(int)_T45;_T47=sgn;_T48=(int)_T47;if(_T46==_T48)goto _TL10F;
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);goto _TL110;_TL10F: _TL110: _T49=seen_size;
# 703
if(!_T49)goto _TL111;_T4A=sz2;_T4B=(int)_T4A;_T4C=sz;_T4D=(int)_T4C;if(_T4B==_T4D)goto _TL111;
t=Cyc_Absyn_int_type(sgn2,sz);goto _TL112;_TL111: _TL112: _T4E=ts;_T4F=_T4E.Complex_spec;
if(!_T4F)goto _TL113;
t=Cyc_Absyn_complex_type(t);goto _TL114;_TL113: _TL114: goto _LL0;}case 2: _T50=seen_size;
# 709
if(!_T50)goto _TL115;
t=Cyc_Absyn_long_double_type;goto _TL116;_TL115: _TL116: _T51=ts;_T52=_T51.Complex_spec;
if(!_T52)goto _TL117;
t=Cyc_Absyn_complex_type(t);goto _TL118;_TL117: _TL118: goto _LL0;default: goto _LL5;}goto _TL10D;_TL10C: _LL5: _T53=seen_sign;
# 715
if(!_T53)goto _TL119;_T54=loc;_T55=
_tag_fat("sign specification on non-integral type",sizeof(char),40U);_T56=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T54,_T55,_T56);goto _TL11A;_TL119: _TL11A: _T57=seen_size;
if(!_T57)goto _TL11B;_T58=loc;_T59=
_tag_fat("size qualifier on non-integral type",sizeof(char),36U);_T5A=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T58,_T59,_T5A);goto _TL11C;_TL11B: _TL11C: goto _LL0;_TL10D: _LL0:;}_T5B=t;
# 721
return _T5B;}}}}}
# 724
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){struct _tuple13*_T0;struct Cyc_Parse_Declarator _T1;struct Cyc_Parse_Declarator _T2;struct Cyc_Absyn_Tqual _T3;void*_T4;struct Cyc_List_List*_T5;struct Cyc_Parse_Declarator _T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct _tuple13*_T9;struct _tuple13*_TA;struct _RegionHandle*_TB;struct Cyc_Absyn_Tqual _TC;void*_TD;struct _tuple13*_TE;struct _tuple13*_TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct _RegionHandle*_T12;struct _tuple15*_T13;struct _RegionHandle*_T14;
# 728
if(ds!=0)goto _TL11D;return 0;_TL11D: _T0=ds;{
struct Cyc_Parse_Declarator d=_T0->hd;_T1=d;{
struct _tuple0*q=_T1.id;_T2=d;{
unsigned varloc=_T2.varloc;_T3=tq;_T4=t;_T5=shared_atts;_T6=d;_T7=_T6.tms;{
struct _tuple14 _T15=Cyc_Parse_apply_tms(_T3,_T4,_T5,_T7);struct Cyc_List_List*_T16;struct Cyc_List_List*_T17;void*_T18;struct Cyc_Absyn_Tqual _T19;_T19=_T15.f0;_T18=_T15.f1;_T17=_T15.f2;_T16=_T15.f3;{struct Cyc_Absyn_Tqual tq2=_T19;void*new_typ=_T18;struct Cyc_List_List*tvs=_T17;struct Cyc_List_List*atts=_T16;_T9=ds;_TA=_T9->tl;
# 735
if(_TA!=0)goto _TL11F;_T8=0;goto _TL120;_TL11F: _TB=r;_TC=tq;_TD=Cyc_Tcutil_copy_type(t);_TE=ds;_TF=_TE->tl;_T10=shared_atts;_T8=Cyc_Parse_apply_tmss(_TB,_TC,_TD,_TF,_T10);_TL120: {struct Cyc_List_List*tl=_T8;_T12=r;{struct Cyc_List_List*_T1A=_region_malloc(_T12,0U,sizeof(struct Cyc_List_List));_T14=r;{struct _tuple15*_T1B=_region_malloc(_T14,0U,sizeof(struct _tuple15));
_T1B->f0=varloc;_T1B->f1=q;_T1B->f2=tq2;_T1B->f3=new_typ;_T1B->f4=tvs;_T1B->f5=atts;_T13=(struct _tuple15*)_T1B;}_T1A->hd=_T13;_T1A->tl=tl;_T11=(struct Cyc_List_List*)_T1A;}return _T11;}}}}}}}
# 739
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){struct _tuple14 _T0;struct Cyc_List_List*_T1;int*_T2;unsigned _T3;void*_T4;struct Cyc_Absyn_Tqual _T5;void*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct _tuple14 _TA;void*_TB;struct Cyc_Absyn_Tqual _TC;void*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct _tuple14 _T11;void*_T12;void*_T13;int*_T14;int _T15;void*_T16;void*_T17;struct Cyc_List_List*_T18;void*_T19;int _T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;int*_T24;int _T25;struct Cyc_List_List*_T26;int _T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;void*_T2B;struct _tuple8*_T2C;struct _tuple8 _T2D;struct _fat_ptr*_T2E;struct Cyc_List_List*_T2F;void*_T30;struct _tuple8*_T31;struct _tuple8 _T32;void*_T33;void*_T34;struct Cyc_List_List*_T35;void*_T36;void**_T37;void**_T38;int*_T39;int _T3A;struct Cyc_Absyn_ArrayInfo _T3B;struct Cyc_Absyn_ArrayInfo _T3C;struct Cyc_Absyn_ArrayInfo _T3D;struct Cyc_Absyn_ArrayInfo _T3E;struct Cyc_Absyn_ArrayInfo _T3F;struct _tuple0*_T40;struct _tuple0*_T41;struct _tuple0*_T42;struct Cyc_List_List*_T43;struct _fat_ptr _T44;struct Cyc_List_List*_T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;void*_T48;struct Cyc_List_List*_T49;struct Cyc_Absyn_Exp*_T4A;struct Cyc_List_List*_T4B;void*_T4C;struct Cyc_Absyn_Exp*_T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;void*_T50;void**_T51;void**_T52;struct Cyc_List_List*_T53;void**_T54;void*_T55;void**_T56;void**_T57;void*_T58;struct Cyc_List_List*_T59;struct Cyc_Absyn_Tqual _T5A;unsigned _T5B;struct Cyc_Absyn_Tqual _T5C;void*_T5D;struct Cyc_List_List*_T5E;struct Cyc_List_List*_T5F;struct Cyc_List_List*_T60;struct _tuple14 _T61;void*_T62;int(*_T63)(unsigned,struct _fat_ptr);unsigned _T64;struct _fat_ptr _T65;struct Cyc_List_List*_T66;struct Cyc_List_List*_T67;struct _tuple14 _T68;int(*_T69)(unsigned,struct _fat_ptr);unsigned _T6A;struct _fat_ptr _T6B;struct Cyc_Absyn_Tqual _T6C;struct Cyc_Absyn_PtrInfo _T6D;void*_T6E;struct Cyc_List_List*_T6F;struct Cyc_List_List*_T70;struct Cyc_List_List*_T71;struct _tuple14 _T72;struct Cyc_Absyn_Tqual _T73;void*_T74;struct Cyc_List_List*_T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;struct _tuple14 _T78;
# 742
if(tms!=0)goto _TL121;{struct _tuple14 _T79;_T79.f0=tq;_T79.f1=t;_T79.f2=0;_T79.f3=atts;_T0=_T79;}return _T0;_TL121: _T1=tms;{
void*_T79=_T1->hd;struct Cyc_Absyn_Tqual _T7A;struct Cyc_Absyn_PtrAtts _T7B;struct Cyc_List_List*_T7C;void*_T7D;unsigned _T7E;void*_T7F;_T2=(int*)_T79;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T79;_T4=_T80->f1;_T7F=(void*)_T4;_T7E=_T80->f2;}{void*zeroterm=_T7F;unsigned ztloc=_T7E;_T5=
# 745
Cyc_Absyn_empty_tqual(0U);_T6=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);_T7=atts;_T8=tms;_T9=_T8->tl;_TA=
# 745
Cyc_Parse_apply_tms(_T5,_T6,_T7,_T9);return _TA;}case 1:{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T79;_T7F=_T80->f1;_TB=_T80->f2;_T7D=(void*)_TB;_T7E=_T80->f3;}{struct Cyc_Absyn_Exp*e=_T7F;void*zeroterm=_T7D;unsigned ztloc=_T7E;_TC=
# 748
Cyc_Absyn_empty_tqual(0U);_TD=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);_TE=atts;_TF=tms;_T10=_TF->tl;_T11=
# 748
Cyc_Parse_apply_tms(_TC,_TD,_TE,_T10);return _T11;}case 3:{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T79;_T12=_T80->f1;_T7F=(void*)_T12;}{void*args=_T7F;unsigned _T80;struct Cyc_Absyn_Exp*_T81;struct Cyc_Absyn_Exp*_T82;struct Cyc_Absyn_Exp*_T83;struct Cyc_Absyn_Exp*_T84;struct Cyc_List_List*_T85;struct Cyc_List_List*_T86;void*_T87;struct Cyc_Absyn_VarargInfo*_T88;int _T89;struct Cyc_List_List*_T8A;_T13=args;_T14=(int*)_T13;_T15=*_T14;if(_T15!=1)goto _TL124;_T16=args;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T8B=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T16;_T8A=_T8B->f1;_T89=_T8B->f2;_T88=_T8B->f3;_T17=_T8B->f4;_T87=(void*)_T17;_T86=_T8B->f5;_T85=_T8B->f6;_T84=_T8B->f7;_T83=_T8B->f8;_T82=_T8B->f9;_T81=_T8B->f10;}{struct Cyc_List_List*args2=_T8A;int c_vararg=_T89;struct Cyc_Absyn_VarargInfo*cyc_vararg=_T88;void*eff=_T87;struct Cyc_List_List*effc=_T86;struct Cyc_List_List*qb=_T85;struct Cyc_Absyn_Exp*chks=_T84;struct Cyc_Absyn_Exp*req=_T83;struct Cyc_Absyn_Exp*ens=_T82;struct Cyc_Absyn_Exp*thrw=_T81;
# 753
struct Cyc_List_List*typvars=0;
# 755
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{
struct Cyc_List_List*as=atts;_TL129: if(as!=0)goto _TL127;else{goto _TL128;}
_TL127: _T18=as;_T19=_T18->hd;_T1A=Cyc_Atts_fntype_att(_T19);if(!_T1A)goto _TL12A;{struct Cyc_List_List*_T8B=_cycalloc(sizeof(struct Cyc_List_List));_T1C=as;
_T8B->hd=_T1C->hd;_T8B->tl=fn_atts;_T1B=(struct Cyc_List_List*)_T8B;}fn_atts=_T1B;goto _TL12B;
# 760
_TL12A:{struct Cyc_List_List*_T8B=_cycalloc(sizeof(struct Cyc_List_List));_T1E=as;_T8B->hd=_T1E->hd;_T8B->tl=new_atts;_T1D=(struct Cyc_List_List*)_T8B;}new_atts=_T1D;_TL12B: _T1F=as;
# 756
as=_T1F->tl;goto _TL129;_TL128:;}_T20=tms;_T21=_T20->tl;
# 762
if(_T21==0)goto _TL12C;_T22=tms;_T23=_T22->tl;{
void*_T8B=_T23->hd;struct Cyc_List_List*_T8C;_T24=(int*)_T8B;_T25=*_T24;if(_T25!=4)goto _TL12E;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T8D=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T8B;_T8C=_T8D->f1;}{struct Cyc_List_List*ts=_T8C;
# 765
typvars=ts;_T26=tms;
tms=_T26->tl;goto _LL12;}_TL12E: goto _LL12;_LL12:;}goto _TL12D;_TL12C: _TL12D: _T27=c_vararg;
# 771
if(_T27)goto _TL130;else{goto _TL132;}_TL132: if(cyc_vararg!=0)goto _TL130;if(args2==0)goto _TL130;_T28=args2;_T29=_T28->tl;if(_T29!=0)goto _TL130;_T2A=args2;_T2B=_T2A->hd;_T2C=(struct _tuple8*)_T2B;_T2D=*_T2C;_T2E=_T2D.f0;if(_T2E!=0)goto _TL130;_T2F=args2;_T30=_T2F->hd;_T31=(struct _tuple8*)_T30;_T32=*_T31;_T33=_T32.f2;_T34=Cyc_Absyn_void_type;if(_T33!=_T34)goto _TL130;
# 776
args2=0;goto _TL131;_TL130: _TL131: {
# 781
struct Cyc_List_List*new_requires=0;{
struct Cyc_List_List*a=args2;_TL136: if(a!=0)goto _TL134;else{goto _TL135;}
_TL134: _T35=a;_T36=_T35->hd;{struct _tuple8*_T8B=(struct _tuple8*)_T36;void*_T8C;struct Cyc_Absyn_Tqual _T8D;struct _fat_ptr*_T8E;{struct _tuple8 _T8F=*_T8B;_T8E=_T8F.f0;_T8D=_T8F.f1;_T37=& _T8B->f2;_T8C=(void**)_T37;}{struct _fat_ptr*vopt=_T8E;struct Cyc_Absyn_Tqual tq=_T8D;void**t=(void**)_T8C;_T38=t;{
void*_T8F=*_T38;unsigned _T90;void*_T91;struct Cyc_Absyn_Exp*_T92;struct Cyc_Absyn_Tqual _T93;void*_T94;_T39=(int*)_T8F;_T3A=*_T39;if(_T3A!=5)goto _TL137;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T95=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T8F;_T3B=_T95->f1;_T94=_T3B.elt_type;_T3C=_T95->f1;_T93=_T3C.tq;_T3D=_T95->f1;_T92=_T3D.num_elts;_T3E=_T95->f1;_T91=_T3E.zero_term;_T3F=_T95->f1;_T90=_T3F.zt_loc;}{void*et=_T94;struct Cyc_Absyn_Tqual tq=_T93;struct Cyc_Absyn_Exp*neltsopt=_T92;void*zt=_T91;unsigned ztloc=_T90;
# 786
if(neltsopt==0)goto _TL139;if(vopt==0)goto _TL139;{
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0));_T40=v;_T40->f0.Loc_n.tag=4U;_T41=v;_T41->f0.Loc_n.val=0;_T42=v;_T42->f1=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);{struct Cyc_Absyn_Exp*_T95[1];_T95[0]=
Cyc_Absyn_var_exp(v,0U);_T44=_tag_fat(_T95,sizeof(struct Cyc_Absyn_Exp*),1);_T43=Cyc_List_list(_T44);}{struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,_T43,0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);{struct Cyc_List_List*_T95=_cycalloc(sizeof(struct Cyc_List_List));
_T95->hd=new_req;_T95->tl=new_requires;_T45=(struct Cyc_List_List*)_T95;}new_requires=_T45;}}}goto _TL13A;_TL139: _TL13A: goto _LL1A;}_TL137: goto _LL1A;_LL1A:;}}}_T46=a;
# 782
a=_T46->tl;goto _TL136;_TL135:;}
# 797
if(new_requires==0)goto _TL13B;{
struct Cyc_Absyn_Exp*r;
if(req==0)goto _TL13D;
r=req;goto _TL13E;
# 802
_TL13D: _T47=new_requires;_T48=_T47->hd;r=(struct Cyc_Absyn_Exp*)_T48;_T49=new_requires;
new_requires=_T49->tl;_TL13E:
# 805
 _TL142: if(new_requires!=0)goto _TL140;else{goto _TL141;}
_TL140: _T4A=r;_T4B=new_requires;_T4C=_T4B->hd;_T4D=(struct Cyc_Absyn_Exp*)_T4C;r=Cyc_Absyn_and_exp(_T4A,_T4D,0U);_T4E=new_requires;
# 805
new_requires=_T4E->tl;goto _TL142;_TL141:
# 807
 req=r;}goto _TL13C;_TL13B: _TL13C: {
# 811
struct Cyc_List_List*tags=Cyc_Parse_get_arg_tags(args2);
# 813
if(tags==0)goto _TL143;
t=Cyc_Parse_substitute_tags(tags,t);goto _TL144;_TL143: _TL144:
 t=Cyc_Parse_array2ptr(t,0);{
# 818
struct Cyc_List_List*a=args2;_TL148: if(a!=0)goto _TL146;else{goto _TL147;}
_TL146: _T4F=a;_T50=_T4F->hd;{struct _tuple8*_T8B=(struct _tuple8*)_T50;void*_T8C;struct Cyc_Absyn_Tqual _T8D;struct _fat_ptr*_T8E;{struct _tuple8 _T8F=*_T8B;_T8E=_T8F.f0;_T8D=_T8F.f1;_T51=& _T8B->f2;_T8C=(void**)_T51;}{struct _fat_ptr*vopt=_T8E;struct Cyc_Absyn_Tqual tq=_T8D;void**t=(void**)_T8C;
if(tags==0)goto _TL149;_T52=t;_T53=tags;_T54=t;_T55=*_T54;
*_T52=Cyc_Parse_substitute_tags(_T53,_T55);goto _TL14A;_TL149: _TL14A: _T56=t;_T57=t;_T58=*_T57;
*_T56=Cyc_Parse_array2ptr(_T58,1);}}_T59=a;
# 818
a=_T59->tl;goto _TL148;_TL147:;}_T5A=tq;_T5B=_T5A.loc;_T5C=
# 830
Cyc_Absyn_empty_tqual(_T5B);_T5D=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,effc,qb,fn_atts,chks,req,ens,thrw);_T5E=new_atts;_T5F=
# 835
_check_null(tms);_T60=_T5F->tl;_T61=
# 830
Cyc_Parse_apply_tms(_T5C,_T5D,_T5E,_T60);return _T61;}}}_TL124: _T62=args;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T8B=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T62;_T80=_T8B->f2;}{unsigned loc=_T80;{
# 837
int(*_T8B)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T63=_T8B;}_T64=loc;_T65=_tag_fat("function declaration without parameter types",sizeof(char),45U);_T63(_T64,_T65);};}case 4:{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T79;_T7C=_T80->f1;_T7E=_T80->f2;}{struct Cyc_List_List*ts=_T7C;unsigned loc=_T7E;_T66=tms;_T67=_T66->tl;
# 844
if(_T67!=0)goto _TL14B;{struct _tuple14 _T80;
_T80.f0=tq;_T80.f1=t;_T80.f2=ts;_T80.f3=atts;_T68=_T80;}return _T68;_TL14B:{
# 849
int(*_T80)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T69=_T80;}_T6A=loc;_T6B=
_tag_fat("type parameters must appear before function arguments in declarator",sizeof(char),68U);
# 849
_T69(_T6A,_T6B);}case 2:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_T79;_T7B=_T80->f1;_T7A=_T80->f2;}{struct Cyc_Absyn_PtrAtts ptratts=_T7B;struct Cyc_Absyn_Tqual tq2=_T7A;_T6C=tq2;{struct Cyc_Absyn_PtrInfo _T80;
# 852
_T80.elt_type=t;_T80.elt_tq=tq;_T80.ptr_atts=ptratts;_T6D=_T80;}_T6E=Cyc_Absyn_pointer_type(_T6D);_T6F=atts;_T70=tms;_T71=_T70->tl;_T72=Cyc_Parse_apply_tms(_T6C,_T6E,_T6F,_T71);return _T72;}default:{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T80=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T79;_T7E=_T80->f1;_T7C=_T80->f2;}{unsigned loc=_T7E;struct Cyc_List_List*atts2=_T7C;_T73=tq;_T74=t;_T75=
# 857
Cyc_List_append(atts,atts2);_T76=tms;_T77=_T76->tl;_T78=Cyc_Parse_apply_tms(_T73,_T74,_T75,_T77);return _T78;}};}}
# 863
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){void*_T0;_T0=
Cyc_Parse_collapse_type_specifiers(tss,loc);return _T0;}
# 872
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple15*t){struct _tuple15*_T0;void*_T1;int*_T2;int _T3;void*_T4;struct Cyc_Core_Opt*_T5;struct Cyc_Core_Opt*_T6;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T7;struct Cyc_Absyn_Typedefdecl*_T8;void*_T9;unsigned _TA;struct Cyc_Absyn_Decl*_TB;struct Cyc_List_List*_TC;struct Cyc_List_List*_TD;void*_TE;struct Cyc_Absyn_Tqual _TF;struct _tuple0*_T10;unsigned _T11;_T0=t;{struct _tuple15 _T12=*_T0;_T11=_T12.f0;_T10=_T12.f1;_TF=_T12.f2;_TE=_T12.f3;_TD=_T12.f4;_TC=_T12.f5;}{unsigned varloc=_T11;struct _tuple0*x=_T10;struct Cyc_Absyn_Tqual tq=_TF;void*typ=_TE;struct Cyc_List_List*tvs=_TD;struct Cyc_List_List*atts=_TC;
# 875
Cyc_Lex_register_typedef(x);{
# 877
struct Cyc_Core_Opt*kind;
void*type;{struct Cyc_Core_Opt*_T12;_T1=typ;_T2=(int*)_T1;_T3=*_T2;if(_T3!=1)goto _TL14D;_T4=typ;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T13=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T4;_T12=_T13->f1;}{struct Cyc_Core_Opt*kopt=_T12;
# 881
type=0;
if(kopt!=0)goto _TL14F;_T6=& Cyc_Kinds_bko;_T5=(struct Cyc_Core_Opt*)_T6;goto _TL150;_TL14F: _T5=kopt;_TL150: kind=_T5;goto _LL3;}_TL14D:
# 884
 kind=0;type=typ;goto _LL3;_LL3:;}{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T12=_cycalloc(sizeof(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct));_T12->tag=8;{struct Cyc_Absyn_Typedefdecl*_T13=_cycalloc(sizeof(struct Cyc_Absyn_Typedefdecl));
# 886
_T13->name=x;_T13->tvs=tvs;_T13->kind=kind;
_T13->defn=type;_T13->atts=atts;
_T13->tq=tq;_T13->extern_c=0;_T8=(struct Cyc_Absyn_Typedefdecl*)_T13;}
# 886
_T12->f1=_T8;_T7=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T12;}_T9=(void*)_T7;_TA=loc;_TB=Cyc_Absyn_new_decl(_T9,_TA);return _TB;}}}
# 893
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;unsigned _T3;struct Cyc_Absyn_Stmt*_T4;{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T5=_cycalloc(sizeof(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct));_T5->tag=12;
_T5->f1=d;_T5->f2=s;_T0=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T5;}_T1=(void*)_T0;_T2=d;_T3=_T2->loc;_T4=Cyc_Absyn_new_stmt(_T1,_T3);return _T4;}
# 897
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*(*_T0)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*);void*(*_T1)(void*(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_List_List*_T2;struct Cyc_Absyn_Stmt*_T3;struct Cyc_Absyn_Stmt*_T4;_T1=Cyc_List_fold_right;{
struct Cyc_Absyn_Stmt*(*_T5)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*)=(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T5;}_T2=ds;_T3=s;_T4=_T0(Cyc_Parse_flatten_decl,_T2,_T3);return _T4;}
# 901
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es,struct Cyc_List_List**rs){struct _tuple11*_T0;struct _tuple13*_T1;struct _RegionHandle*_T2;struct Cyc_List_List*_T3;struct _RegionHandle*_T4;struct Cyc_List_List*_T5;struct _RegionHandle*_T6;struct _tuple11*_T7;struct Cyc_List_List**_T8;struct Cyc_List_List**_T9;struct _tuple13**_TA;struct _tuple13*(*_TB)(struct _tuple13*);
# 905
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
struct Cyc_List_List*renames=0;
_TL154: if(ds!=0)goto _TL152;else{goto _TL153;}
_TL152: _T0=ds;{struct _tuple12 _TC=_T0->hd;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_Parse_Declarator _TF;_TF=_TC.f0;_TE=_TC.f1;_TD=_TC.f2;{struct Cyc_Parse_Declarator d=_TF;struct Cyc_Absyn_Exp*e=_TE;struct Cyc_Absyn_Exp*rename=_TD;_T2=r;{struct _tuple13*_T10=_region_malloc(_T2,0U,sizeof(struct _tuple13));
_T10->tl=declarators;_T10->hd=d;_T1=(struct _tuple13*)_T10;}declarators=_T1;_T4=r;{struct Cyc_List_List*_T10=_region_malloc(_T4,0U,sizeof(struct Cyc_List_List));
_T10->hd=e;_T10->tl=exprs;_T3=(struct Cyc_List_List*)_T10;}exprs=_T3;_T6=r;{struct Cyc_List_List*_T10=_region_malloc(_T6,0U,sizeof(struct Cyc_List_List));
_T10->hd=rename;_T10->tl=renames;_T5=(struct Cyc_List_List*)_T10;}renames=_T5;}}_T7=ds;
# 908
ds=_T7->tl;goto _TL154;_TL153: _T8=es;
# 914
*_T8=Cyc_List_imp_rev(exprs);_T9=rs;
*_T9=Cyc_List_imp_rev(renames);_TA=decls;{
struct _tuple13*(*_TC)(struct _tuple13*)=(struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev;_TB=_TC;}*_TA=_TB(declarators);}
# 924
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){struct Cyc_Parse_Declaration_spec _T0;struct Cyc_Parse_Declaration_spec _T1;struct Cyc_Parse_Declaration_spec _T2;struct Cyc_Absyn_Tqual _T3;unsigned _T4;struct Cyc_Parse_Declaration_spec _T5;int _T6;unsigned _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_Parse_Declaration_spec _TA;int _TB;enum Cyc_Absyn_Scope _TC;int _TD;struct _RegionHandle*_TE;struct _tuple11*_TF;struct _tuple13**_T10;struct Cyc_List_List**_T11;struct Cyc_List_List**_T12;struct Cyc_List_List*_T13;void*_T14;struct Cyc_Absyn_Exp*_T15;struct Cyc_List_List*_T16;void*_T17;int*_T18;unsigned _T19;void*_T1A;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1B;struct Cyc_Absyn_TypeDecl*_T1C;struct Cyc_Absyn_TypeDecl*_T1D;void*_T1E;int*_T1F;unsigned _T20;void*_T21;struct Cyc_Absyn_TypeDecl*_T22;void*_T23;struct Cyc_Absyn_Aggrdecl*_T24;struct Cyc_Absyn_Aggrdecl*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;struct Cyc_Absyn_Aggrdecl*_T28;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T29;void*_T2A;unsigned _T2B;void*_T2C;struct Cyc_Absyn_TypeDecl*_T2D;void*_T2E;unsigned _T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_Absyn_Enumdecl*_T32;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T33;void*_T34;unsigned _T35;void*_T36;struct Cyc_Absyn_TypeDecl*_T37;void*_T38;unsigned _T39;struct _fat_ptr _T3A;struct _fat_ptr _T3B;struct Cyc_Absyn_Datatypedecl*_T3C;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T3D;void*_T3E;unsigned _T3F;void*_T40;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T41;void*_T42;int*_T43;unsigned _T44;void*_T45;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T46;void*_T47;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T48;union Cyc_Absyn_AggrInfo _T49;struct _union_AggrInfo_UnknownAggr _T4A;unsigned _T4B;void*_T4C;void*_T4D;union Cyc_Absyn_AggrInfo _T4E;struct _union_AggrInfo_UnknownAggr _T4F;struct _tuple2 _T50;union Cyc_Absyn_AggrInfo _T51;struct _union_AggrInfo_UnknownAggr _T52;struct _tuple2 _T53;struct Cyc_List_List*(*_T54)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T55)(void*(*)(void*,void*),void*,struct Cyc_List_List*);unsigned _T56;struct Cyc_List_List*_T57;struct Cyc_Absyn_Aggrdecl*_T58;struct Cyc_Absyn_Aggrdecl*_T59;struct Cyc_Absyn_Aggrdecl*_T5A;struct Cyc_Absyn_Aggrdecl*_T5B;struct Cyc_Absyn_Aggrdecl*_T5C;struct Cyc_Absyn_Aggrdecl*_T5D;struct Cyc_Absyn_Aggrdecl*_T5E;unsigned _T5F;struct _fat_ptr _T60;struct _fat_ptr _T61;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T62;void*_T63;unsigned _T64;void*_T65;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T66;void*_T67;struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_T68;union Cyc_Absyn_DatatypeInfo _T69;struct _union_DatatypeInfo_KnownDatatype _T6A;unsigned _T6B;void*_T6C;void*_T6D;union Cyc_Absyn_DatatypeInfo _T6E;struct _union_DatatypeInfo_KnownDatatype _T6F;unsigned _T70;struct _fat_ptr _T71;struct _fat_ptr _T72;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T73;struct Cyc_Absyn_Datatypedecl**_T74;void*_T75;unsigned _T76;void*_T77;void*_T78;union Cyc_Absyn_DatatypeInfo _T79;struct _union_DatatypeInfo_UnknownDatatype _T7A;struct Cyc_Absyn_UnknownDatatypeInfo _T7B;union Cyc_Absyn_DatatypeInfo _T7C;struct _union_DatatypeInfo_UnknownDatatype _T7D;struct Cyc_Absyn_UnknownDatatypeInfo _T7E;struct Cyc_List_List*(*_T7F)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T80)(void*(*)(void*,void*),void*,struct Cyc_List_List*);unsigned _T81;struct Cyc_List_List*_T82;unsigned _T83;struct _fat_ptr _T84;struct _fat_ptr _T85;void*_T86;void*_T87;struct Cyc_Absyn_Enumdecl*_T88;struct Cyc_Absyn_Enumdecl*_T89;struct Cyc_Absyn_Enumdecl*_T8A;unsigned _T8B;struct _fat_ptr _T8C;struct _fat_ptr _T8D;struct Cyc_Absyn_Decl*_T8E;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T8F;void*_T90;void*_T91;struct Cyc_Absyn_Enumdecl*_T92;struct Cyc_Absyn_Enumdecl*_T93;struct Cyc_Absyn_Enumdecl*_T94;struct Cyc_Core_Opt*_T95;unsigned _T96;struct _fat_ptr _T97;struct _fat_ptr _T98;struct Cyc_Absyn_Decl*_T99;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T9A;unsigned _T9B;struct _fat_ptr _T9C;struct _fat_ptr _T9D;int _T9E;int _T9F;unsigned _TA0;struct _fat_ptr _TA1;struct _fat_ptr _TA2;struct Cyc_List_List*(*_TA3)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_TA4)(void*(*)(void*,void*),void*,struct Cyc_List_List*);unsigned _TA5;struct Cyc_List_List*_TA6;struct Cyc_List_List*_TA7;void*_TA8;unsigned _TA9;struct _fat_ptr _TAA;struct _fat_ptr _TAB;int(*_TAC)(unsigned,struct _fat_ptr);unsigned _TAD;struct _fat_ptr _TAE;int(*_TAF)(unsigned,struct _fat_ptr);unsigned _TB0;struct _fat_ptr _TB1;unsigned _TB2;struct _tuple0*_TB3;void*_TB4;struct Cyc_List_List*_TB5;void*_TB6;struct Cyc_Absyn_Exp*_TB7;struct Cyc_List_List*_TB8;void*_TB9;struct Cyc_Absyn_Exp*_TBA;struct Cyc_Absyn_Vardecl*_TBB;struct Cyc_Absyn_Vardecl*_TBC;struct Cyc_Absyn_Vardecl*_TBD;struct Cyc_List_List*_TBE;struct Cyc_Absyn_Decl*_TBF;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_TC0;struct Cyc_List_List*_TC1;struct Cyc_List_List*_TC2;struct Cyc_List_List*_TC3;struct _RegionHandle _TC4=_new_region(0U,"mkrgn");struct _RegionHandle*mkrgn=& _TC4;_push_region(mkrgn);{struct Cyc_List_List*_TC5;struct Cyc_Parse_Type_specifier _TC6;struct Cyc_Absyn_Tqual _TC7;_T0=ds;_TC7=_T0.tq;_T1=ds;_TC6=_T1.type_specs;_T2=ds;_TC5=_T2.attributes;{struct Cyc_Absyn_Tqual tq=_TC7;struct Cyc_Parse_Type_specifier tss=_TC6;struct Cyc_List_List*atts=_TC5;_T3=tq;_T4=_T3.loc;
# 929
if(_T4!=0U)goto _TL155;tq.loc=tqual_loc;goto _TL156;_TL155: _TL156: _T5=ds;_T6=_T5.is_inline;
if(!_T6)goto _TL157;_T7=loc;_T8=
_tag_fat("inline qualifier on non-function definition",sizeof(char),44U);_T9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T7,_T8,_T9);goto _TL158;_TL157: _TL158: {
# 933
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
# 949
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
struct Cyc_List_List*renames=0;_TE=mkrgn;_TF=ids;_T10=& declarators;_T11=& exprs;_T12=& renames;
Cyc_Parse_decl_split(_TE,_TF,_T10,_T11,_T12);{
# 954
int exps_empty=1;{
struct Cyc_List_List*es=exprs;_TL15F: if(es!=0)goto _TL15D;else{goto _TL15E;}
_TL15D: _T13=es;_T14=_T13->hd;_T15=(struct Cyc_Absyn_Exp*)_T14;if(_T15==0)goto _TL160;
exps_empty=0;goto _TL15E;_TL160: _T16=es;
# 955
es=_T16->tl;goto _TL15F;_TL15E:;}{
# 962
void*base_type=Cyc_Parse_collapse_type_specifiers(tss,loc);
if(declarators!=0)goto _TL162;{int _TC8;struct Cyc_Absyn_Datatypedecl**_TC9;struct Cyc_List_List*_TCA;struct _tuple0*_TCB;enum Cyc_Absyn_AggrKind _TCC;struct Cyc_Absyn_Datatypedecl*_TCD;struct Cyc_Absyn_Enumdecl*_TCE;struct Cyc_Absyn_Aggrdecl*_TCF;_T17=base_type;_T18=(int*)_T17;_T19=*_T18;switch(_T19){case 10: _T1A=base_type;_T1B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1A;_T1C=_T1B->f1;_T1D=(struct Cyc_Absyn_TypeDecl*)_T1C;_T1E=_T1D->r;_T1F=(int*)_T1E;_T20=*_T1F;switch(_T20){case 0: _T21=base_type;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T21;_T22=_TD0->f1;{struct Cyc_Absyn_TypeDecl _TD1=*_T22;_T23=_TD1.r;{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_TD2=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_T23;_TCF=_TD2->f1;}}}{struct Cyc_Absyn_Aggrdecl*ad=_TCF;_T24=ad;_T25=ad;_T26=_T25->attributes;_T27=atts;
# 968
_T24->attributes=Cyc_List_append(_T26,_T27);_T28=ad;
_T28->sc=s;{struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TD1->tag=5;
_TD1->f1=ad;_T29=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_TD1;}_T2A=(void*)_T29;_T2B=loc;_TD0->hd=Cyc_Absyn_new_decl(_T2A,_T2B);_TD0->tl=0;_npop_handler(0);return _TD0;}}case 1: _T2C=base_type;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T2C;_T2D=_TD0->f1;{struct Cyc_Absyn_TypeDecl _TD1=*_T2D;_T2E=_TD1.r;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_TD2=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T2E;_TCE=_TD2->f1;}}}{struct Cyc_Absyn_Enumdecl*ed=_TCE;
# 972
if(atts==0)goto _TL166;_T2F=loc;_T30=_tag_fat("attributes on enum not supported",sizeof(char),33U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2F,_T30,_T31);goto _TL167;_TL166: _TL167: _T32=ed;
_T32->sc=s;{struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TD1->tag=7;
_TD1->f1=ed;_T33=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TD1;}_T34=(void*)_T33;_T35=loc;_TD0->hd=Cyc_Absyn_new_decl(_T34,_T35);_TD0->tl=0;_npop_handler(0);return _TD0;}}default: _T36=base_type;{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T36;_T37=_TD0->f1;{struct Cyc_Absyn_TypeDecl _TD1=*_T37;_T38=_TD1.r;{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_TD2=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_T38;_TCD=_TD2->f1;}}}{struct Cyc_Absyn_Datatypedecl*dd=_TCD;
# 976
if(atts==0)goto _TL168;_T39=loc;_T3A=_tag_fat("attributes on datatypes not supported",sizeof(char),38U);_T3B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T39,_T3A,_T3B);goto _TL169;_TL168: _TL169: _T3C=dd;
_T3C->sc=s;{struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TD1->tag=6;
_TD1->f1=dd;_T3D=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TD1;}_T3E=(void*)_T3D;_T3F=loc;_TD0->hd=Cyc_Absyn_new_decl(_T3E,_T3F);_TD0->tl=0;_npop_handler(0);return _TD0;}}};case 0: _T40=base_type;_T41=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T40;_T42=_T41->f1;_T43=(int*)_T42;_T44=*_T43;switch(_T44){case 24: _T45=base_type;_T46=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T45;_T47=_T46->f1;_T48=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T47;_T49=_T48->f1;_T4A=_T49.UnknownAggr;_T4B=_T4A.tag;if(_T4B!=1)goto _TL16B;_T4C=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4C;_T4D=_TD0->f1;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T4D;_T4E=_TD1->f1;_T4F=_T4E.UnknownAggr;_T50=_T4F.val;_TCC=_T50.f0;_T51=_TD1->f1;_T52=_T51.UnknownAggr;_T53=_T52.val;_TCB=_T53.f1;}_TCA=_TD0->f2;}{enum Cyc_Absyn_AggrKind k=_TCC;struct _tuple0*n=_TCB;struct Cyc_List_List*ts=_TCA;_T55=Cyc_List_map_c;{
# 980
struct Cyc_List_List*(*_TD0)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T55;_T54=_TD0;}_T56=loc;_T57=ts;{struct Cyc_List_List*ts2=_T54(Cyc_Parse_typ2tvar,_T56,_T57);
struct Cyc_Absyn_Aggrdecl*ad;ad=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T58=ad;_T58->kind=k;_T59=ad;_T59->sc=s;_T5A=ad;_T5A->name=n;_T5B=ad;_T5B->tvs=ts2;_T5C=ad;_T5C->impl=0;_T5D=ad;_T5D->attributes=0;_T5E=ad;_T5E->expected_mem_kind=0;
if(atts==0)goto _TL16D;_T5F=loc;_T60=_tag_fat("bad attributes on type declaration",sizeof(char),35U);_T61=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T5F,_T60,_T61);goto _TL16E;_TL16D: _TL16E: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_TD1->tag=5;
_TD1->f1=ad;_T62=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_TD1;}_T63=(void*)_T62;_T64=loc;_TD0->hd=Cyc_Absyn_new_decl(_T63,_T64);_TD0->tl=0;_npop_handler(0);return _TD0;}}}_TL16B: goto _LL25;case 22: _T65=base_type;_T66=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T65;_T67=_T66->f1;_T68=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T67;_T69=_T68->f1;_T6A=_T69.KnownDatatype;_T6B=_T6A.tag;if(_T6B!=2)goto _TL16F;_T6C=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6C;_T6D=_TD0->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T6D;_T6E=_TD1->f1;_T6F=_T6E.KnownDatatype;_TC9=_T6F.val;}}{struct Cyc_Absyn_Datatypedecl**tudp=_TC9;
# 985
if(atts==0)goto _TL171;_T70=loc;_T71=_tag_fat("bad attributes on datatype",sizeof(char),27U);_T72=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T70,_T71,_T72);goto _TL172;_TL171: _TL172: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct));_TD1->tag=6;_T74=tudp;
_TD1->f1=*_T74;_T73=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TD1;}_T75=(void*)_T73;_T76=loc;_TD0->hd=Cyc_Absyn_new_decl(_T75,_T76);_TD0->tl=0;_npop_handler(0);return _TD0;}}_TL16F: _T77=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T77;_T78=_TD0->f1;{struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_T78;_T79=_TD1->f1;_T7A=_T79.UnknownDatatype;_T7B=_T7A.val;_TCB=_T7B.name;_T7C=_TD1->f1;_T7D=_T7C.UnknownDatatype;_T7E=_T7D.val;_TC8=_T7E.is_extensible;}_TCA=_TD0->f2;}{struct _tuple0*n=_TCB;int isx=_TC8;struct Cyc_List_List*ts=_TCA;_T80=Cyc_List_map_c;{
# 988
struct Cyc_List_List*(*_TD0)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T80;_T7F=_TD0;}_T81=loc;_T82=ts;{struct Cyc_List_List*ts2=_T7F(Cyc_Parse_typ2tvar,_T81,_T82);
struct Cyc_Absyn_Decl*tud=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);
if(atts==0)goto _TL173;_T83=loc;_T84=_tag_fat("bad attributes on datatype",sizeof(char),27U);_T85=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T83,_T84,_T85);goto _TL174;_TL173: _TL174: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));
_TD0->hd=tud;_TD0->tl=0;_npop_handler(0);return _TD0;}}}case 19: _T86=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T86;_T87=_TD0->f1;{struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_T87;_TCB=_TD1->f1;}}{struct _tuple0*n=_TCB;
# 993
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T88=ed;_T88->sc=s;_T89=ed;_T89->name=n;_T8A=ed;_T8A->fields=0;
if(atts==0)goto _TL175;_T8B=loc;_T8C=_tag_fat("bad attributes on enum",sizeof(char),23U);_T8D=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T8B,_T8C,_T8D);goto _TL176;_TL175: _TL176: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TD2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TD2->tag=7;
_TD2->f1=ed;_T8F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TD2;}_TD1->r=(void*)_T8F;_TD1->loc=loc;_T8E=(struct Cyc_Absyn_Decl*)_TD1;}_TD0->hd=_T8E;_TD0->tl=0;_npop_handler(0);return _TD0;}}case 20: _T90=base_type;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TD0=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T90;_T91=_TD0->f1;{struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*_TD1=(struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_T91;_TCA=_TD1->f1;}}{struct Cyc_List_List*fs=_TCA;
# 999
struct Cyc_Absyn_Enumdecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T92=ed;_T92->sc=s;_T93=ed;_T93->name=Cyc_Parse_gensym_enum();_T94=ed;{struct Cyc_Core_Opt*_TD0=_cycalloc(sizeof(struct Cyc_Core_Opt));_TD0->v=fs;_T95=(struct Cyc_Core_Opt*)_TD0;}_T94->fields=_T95;
if(atts==0)goto _TL177;_T96=loc;_T97=_tag_fat("bad attributes on enum",sizeof(char),23U);_T98=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T96,_T97,_T98);goto _TL178;_TL177: _TL178: {struct Cyc_List_List*_TD0;_TD0=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_TD1=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_TD2=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_TD2->tag=7;
_TD2->f1=ed;_T9A=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TD2;}_TD1->r=(void*)_T9A;_TD1->loc=loc;_T99=(struct Cyc_Absyn_Decl*)_TD1;}_TD0->hd=_T99;_TD0->tl=0;_npop_handler(0);return _TD0;}}default: goto _LL25;};default: _LL25: _T9B=loc;_T9C=
_tag_fat("missing declarator",sizeof(char),19U);_T9D=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T9B,_T9C,_T9D);{struct Cyc_List_List*_TD0=0;_npop_handler(0);return _TD0;}};}goto _TL163;_TL162: _TL163: {
# 1006
struct Cyc_List_List*fields=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);_T9E=istypedef;
if(!_T9E)goto _TL179;_T9F=exps_empty;
# 1011
if(_T9F)goto _TL17B;else{goto _TL17D;}
_TL17D: _TA0=loc;_TA1=_tag_fat("initializer in typedef declaration",sizeof(char),35U);_TA2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TA0,_TA1,_TA2);goto _TL17C;_TL17B: _TL17C: _TA4=Cyc_List_map_c;{
struct Cyc_List_List*(*_TC8)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple15*),unsigned,struct Cyc_List_List*))_TA4;_TA3=_TC8;}_TA5=loc;_TA6=fields;{struct Cyc_List_List*decls=_TA3(Cyc_Parse_v_typ_to_typedef,_TA5,_TA6);struct Cyc_List_List*_TC8=decls;_npop_handler(0);return _TC8;}_TL179: {
# 1017
struct Cyc_List_List*decls=0;{
struct Cyc_List_List*ds=fields;
_TL181:
# 1018
 if(ds!=0)goto _TL17F;else{goto _TL180;}
# 1021
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
# 1020
ds=_TC1->tl;_TC2=_check_null(exprs);exprs=_TC2->tl;_TC3=_check_null(renames);renames=_TC3->tl;goto _TL181;_TL180:;}{struct Cyc_List_List*_TC8=
# 1034
Cyc_List_imp_rev(decls);_npop_handler(0);return _TC8;}}}}}}}}}_pop_region();}
# 1038
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){union Cyc_Absyn_Cnst _T0;struct _union_Cnst_LongLong_c _T1;unsigned _T2;union Cyc_Absyn_Cnst _T3;struct _union_Cnst_Int_c _T4;struct _tuple5 _T5;int _T6;unsigned _T7;union Cyc_Absyn_Cnst _T8;struct _union_Cnst_Char_c _T9;struct _tuple3 _TA;char _TB;unsigned _TC;union Cyc_Absyn_Cnst _TD;struct _union_Cnst_LongLong_c _TE;struct _tuple6 _TF;long long _T10;unsigned _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;long long _T14;unsigned _T15;struct Cyc_String_pa_PrintArg_struct _T16;unsigned _T17;struct _fat_ptr _T18;struct _fat_ptr _T19;long long _T1A;char _T1B;int _T1C;_T0=x;_T1=_T0.LongLong_c;_T2=_T1.tag;switch(_T2){case 5: _T3=x;_T4=_T3.Int_c;_T5=_T4.val;_T1C=_T5.f1;{int i=_T1C;_T6=i;_T7=(unsigned)_T6;
# 1040
return _T7;}case 2: _T8=x;_T9=_T8.Char_c;_TA=_T9.val;_T1B=_TA.f1;{char c=_T1B;_TB=c;_TC=(unsigned)_TB;
return _TC;}case 6: _TD=x;_TE=_TD.LongLong_c;_TF=_TE.val;_T1A=_TF.f1;{long long x=_T1A;_T10=x;{
# 1043
unsigned long long y=(unsigned long long)_T10;
if(y <= 4294967295U)goto _TL189;_T11=loc;_T12=
_tag_fat("integer constant too large",sizeof(char),27U);_T13=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T11,_T12,_T13);goto _TL18A;_TL189: _TL18A: _T14=x;_T15=(unsigned)_T14;
return _T15;}}default:{struct Cyc_String_pa_PrintArg_struct _T1D;_T1D.tag=0;
# 1048
_T1D.f1=Cyc_Absynpp_cnst2string(x);_T16=_T1D;}{struct Cyc_String_pa_PrintArg_struct _T1D=_T16;void*_T1E[1];_T1E[0]=& _T1D;_T17=loc;_T18=_tag_fat("expected integer constant but found %s",sizeof(char),39U);_T19=_tag_fat(_T1E,sizeof(void*),1);Cyc_Warn_err(_T17,_T18,_T19);}
return 0U;};}
# 1054
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){struct Cyc_Absyn_Pat*_T0;int*_T1;unsigned _T2;struct _tuple0*_T3;struct Cyc_Absyn_Pat*_T4;unsigned _T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T7;struct Cyc_Absyn_Pat*_T8;struct Cyc_Absyn_Pat*_T9;void*_TA;int*_TB;int _TC;struct Cyc_Absyn_Vardecl*_TD;struct _tuple0*_TE;struct Cyc_Absyn_Pat*_TF;unsigned _T10;struct Cyc_Absyn_Exp*_T11;struct Cyc_Absyn_Pat*_T12;unsigned _T13;struct Cyc_Absyn_Exp*_T14;struct Cyc_Absyn_Exp*_T15;struct Cyc_Absyn_Pat*_T16;unsigned _T17;struct Cyc_Absyn_Exp*_T18;struct Cyc_Absyn_Pat*_T19;unsigned _T1A;struct Cyc_Absyn_Exp*_T1B;enum Cyc_Absyn_Sign _T1C;int _T1D;struct Cyc_Absyn_Pat*_T1E;unsigned _T1F;struct Cyc_Absyn_Exp*_T20;char _T21;struct Cyc_Absyn_Pat*_T22;unsigned _T23;struct Cyc_Absyn_Exp*_T24;struct _fat_ptr _T25;int _T26;struct Cyc_Absyn_Pat*_T27;unsigned _T28;struct Cyc_Absyn_Exp*_T29;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T2A;int _T2B;struct _tuple0*_T2C;struct Cyc_Absyn_Pat*_T2D;unsigned _T2E;struct Cyc_List_List*(*_T2F)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*);struct Cyc_List_List*(*_T30)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_List_List*_T33;struct Cyc_Absyn_Pat*_T34;unsigned _T35;struct Cyc_Absyn_Exp*_T36;struct Cyc_Absyn_Exp*_T37;struct Cyc_Absyn_Pat*_T38;unsigned _T39;struct _fat_ptr _T3A;struct _fat_ptr _T3B;struct Cyc_Absyn_Pat*_T3C;unsigned _T3D;struct Cyc_Absyn_Exp*_T3E;_T0=p;{
void*_T3F=_T0->r;struct Cyc_Absyn_Exp*_T40;struct Cyc_List_List*_T41;struct _fat_ptr _T42;char _T43;int _T44;enum Cyc_Absyn_Sign _T45;struct Cyc_Absyn_Pat*_T46;struct Cyc_Absyn_Vardecl*_T47;struct _tuple0*_T48;_T1=(int*)_T3F;_T2=*_T1;switch(_T2){case 14:{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T3F;_T48=_T49->f1;}{struct _tuple0*x=_T48;_T3=x;_T4=p;_T5=_T4->loc;_T6=
Cyc_Absyn_unknownid_exp(_T3,_T5);return _T6;}case 3: _T7=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T3F;_T8=_T7->f2;_T9=(struct Cyc_Absyn_Pat*)_T8;_TA=_T9->r;_TB=(int*)_TA;_TC=*_TB;if(_TC!=0)goto _TL18C;{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T3F;_T47=_T49->f1;}{struct Cyc_Absyn_Vardecl*vd=_T47;_TD=vd;_TE=_TD->name;_TF=p;_T10=_TF->loc;_T11=
# 1058
Cyc_Absyn_unknownid_exp(_TE,_T10);_T12=p;_T13=_T12->loc;_T14=Cyc_Absyn_deref_exp(_T11,_T13);return _T14;}_TL18C: goto _LL13;case 5:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T3F;_T46=_T49->f1;}{struct Cyc_Absyn_Pat*p2=_T46;_T15=
Cyc_Parse_pat2exp(p2);_T16=p;_T17=_T16->loc;_T18=Cyc_Absyn_address_exp(_T15,_T17);return _T18;}case 8: _T19=p;_T1A=_T19->loc;_T1B=
Cyc_Absyn_null_exp(_T1A);return _T1B;case 9:{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T3F;_T45=_T49->f1;_T44=_T49->f2;}{enum Cyc_Absyn_Sign s=_T45;int i=_T44;_T1C=s;_T1D=i;_T1E=p;_T1F=_T1E->loc;_T20=
Cyc_Absyn_int_exp(_T1C,_T1D,_T1F);return _T20;}case 10:{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T3F;_T43=_T49->f1;}{char c=_T43;_T21=c;_T22=p;_T23=_T22->loc;_T24=
Cyc_Absyn_char_exp(_T21,_T23);return _T24;}case 11:{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T3F;_T42=_T49->f1;_T44=_T49->f2;}{struct _fat_ptr s=_T42;int i=_T44;_T25=s;_T26=i;_T27=p;_T28=_T27->loc;_T29=
Cyc_Absyn_float_exp(_T25,_T26,_T28);return _T29;}case 15: _T2A=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T3F;_T2B=_T2A->f3;if(_T2B!=0)goto _TL18E;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T3F;_T48=_T49->f1;_T41=_T49->f2;}{struct _tuple0*x=_T48;struct Cyc_List_List*ps=_T41;_T2C=x;_T2D=p;_T2E=_T2D->loc;{
# 1065
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_T2C,_T2E);_T30=Cyc_List_map;{
struct Cyc_List_List*(*_T49)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))_T30;_T2F=_T49;}_T31=ps;{struct Cyc_List_List*es=_T2F(Cyc_Parse_pat2exp,_T31);_T32=e1;_T33=es;_T34=p;_T35=_T34->loc;_T36=
Cyc_Absyn_unknowncall_exp(_T32,_T33,_T35);return _T36;}}}_TL18E: goto _LL13;case 16:{struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_T49=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_T3F;_T40=_T49->f1;}{struct Cyc_Absyn_Exp*e=_T40;_T37=e;
return _T37;}default: _LL13: _T38=p;_T39=_T38->loc;_T3A=
# 1070
_tag_fat("cannot mix patterns and expressions in case",sizeof(char),44U);_T3B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T39,_T3A,_T3B);_T3C=p;_T3D=_T3C->loc;_T3E=
Cyc_Absyn_null_exp(_T3D);return _T3E;};}}
# 1074
static struct _tuple16 Cyc_Parse_split_seq(struct Cyc_Absyn_Exp*maybe_seq){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct _tuple16 _T3;struct _tuple16 _T4;_T0=maybe_seq;{
void*_T5=_T0->r;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Exp*_T7;_T1=(int*)_T5;_T2=*_T1;if(_T2!=9)goto _TL190;{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T5;_T7=_T8->f1;_T6=_T8->f2;}{struct Cyc_Absyn_Exp*e1=_T7;struct Cyc_Absyn_Exp*e2=_T6;{struct _tuple16 _T8;
# 1077
_T8.f0=e1;_T8.f1=e2;_T3=_T8;}return _T3;}_TL190:{struct _tuple16 _T8;
# 1079
_T8.f0=maybe_seq;_T8.f1=0;_T4=_T8;}return _T4;;}}
# 1082
static struct Cyc_Absyn_Exp*Cyc_Parse_join_assn(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){struct Cyc_Absyn_Exp*_T0;struct Cyc_Absyn_Exp*_T1;struct Cyc_Absyn_Exp*_T2;
if(e1==0)goto _TL192;if(e2==0)goto _TL192;_T0=Cyc_Absyn_and_exp(e1,e2,0U);return _T0;
_TL192: if(e1==0)goto _TL194;_T1=e1;return _T1;
_TL194: _T2=e2;return _T2;}struct _tuple21{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};
# 1087
static struct _tuple21 Cyc_Parse_join_assns(struct _tuple21 a1,struct _tuple21 a2){struct _tuple21 _T0;struct _tuple21 _T1;struct _tuple21 _T2;struct _tuple21 _T3;struct _tuple21 _T4;struct _tuple21 _T5;struct _tuple21 _T6;struct _tuple21 _T7;struct _tuple21 _T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Absyn_Exp*_TA;struct Cyc_Absyn_Exp*_TB;struct Cyc_Absyn_Exp*_TC;_T0=a1;_TC=_T0.f0;_T1=a1;_TB=_T1.f1;_T2=a1;_TA=_T2.f2;_T3=a1;_T9=_T3.f3;{struct Cyc_Absyn_Exp*c1=_TC;struct Cyc_Absyn_Exp*r1=_TB;struct Cyc_Absyn_Exp*e1=_TA;struct Cyc_Absyn_Exp*t1=_T9;struct Cyc_Absyn_Exp*_TD;struct Cyc_Absyn_Exp*_TE;struct Cyc_Absyn_Exp*_TF;struct Cyc_Absyn_Exp*_T10;_T4=a2;_T10=_T4.f0;_T5=a2;_TF=_T5.f1;_T6=a2;_TE=_T6.f2;_T7=a2;_TD=_T7.f3;{struct Cyc_Absyn_Exp*c2=_T10;struct Cyc_Absyn_Exp*r2=_TF;struct Cyc_Absyn_Exp*e2=_TE;struct Cyc_Absyn_Exp*t2=_TD;
# 1090
struct Cyc_Absyn_Exp*c=Cyc_Parse_join_assn(c1,c2);
struct Cyc_Absyn_Exp*r=Cyc_Parse_join_assn(r1,r2);
struct Cyc_Absyn_Exp*e=Cyc_Parse_join_assn(e1,e2);
struct Cyc_Absyn_Exp*t=Cyc_Parse_join_assn(t1,t2);{struct _tuple21 _T11;
_T11.f0=c;_T11.f1=r;_T11.f2=e;_T11.f3=t;_T8=_T11;}return _T8;}}}
# 1097
static void*Cyc_Parse_assign_cvar_pos(struct _fat_ptr posstr,int ovfat,void*cv){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T5;const char**_T6;void*_T7;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T8;int*_T9;const char**_TA;struct _fat_ptr _TB;char*_TC;int*_TD;void*_TE;void*_TF;void*_T10;void*_T11;_T0=cv;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TL196;_T3=cv;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T12=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T4=cv;_T5=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T4;_T6=& _T5->f6;_T11=(const char**)_T6;_T7=cv;_T8=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T9=& _T8->f7;_T10=(int*)_T9;}{const char**pos=(const char**)_T11;int*ov=(int*)_T10;_TA=pos;_TB=posstr;_TC=_untag_fat_ptr_check_bound(_TB,sizeof(char),1U);
# 1100
*_TA=(const char*)_TC;_TD=ov;
*_TD=ovfat;_TE=cv;
return _TE;}_TL196: _TF=cv;
# 1104
return _TF;;}
# 1108
static void*Cyc_Parse_typevar2cvar(struct _fat_ptr s){struct Cyc_Hashtable_Table*_T0;unsigned _T1;struct Cyc_Hashtable_Table*(*_T2)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T3)(int,int(*)(void*,void*),int(*)(void*));int(*_T4)(struct _fat_ptr*,struct _fat_ptr*);int(*_T5)(struct _fat_ptr*);struct _handler_cons*_T6;int*_T7;int _T8;void*(*_T9)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_TA)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TB;struct _fat_ptr*_TC;void*_TD;struct Cyc_Core_Not_found_exn_struct*_TE;char*_TF;char*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;struct _fat_ptr*_T13;struct _fat_ptr _T14;struct _fat_ptr _T15;int _T16;struct Cyc_Core_Opt*_T17;struct Cyc_Core_Opt*_T18;struct _fat_ptr _T19;void(*_T1A)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*);void(*_T1B)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T1C;struct _fat_ptr*_T1D;void*_T1E;void*_T1F;int(*_T20)(unsigned,struct _fat_ptr);struct _fat_ptr _T21;
# 1111
static struct Cyc_Hashtable_Table*cvmap=0;_T0=cvmap;_T1=(unsigned)_T0;
if(_T1)goto _TL198;else{goto _TL19A;}
_TL19A: _T3=Cyc_Hashtable_create;{struct Cyc_Hashtable_Table*(*_T22)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T3;_T2=_T22;}_T4=Cyc_strptrcmp;_T5=Cyc_Hashtable_hash_stringptr;cvmap=_T2(101,_T4,_T5);goto _TL199;_TL198: _TL199: {struct _handler_cons _T22;_T6=& _T22;_push_handler(_T6);{int _T23=0;_T7=_T22.handler;_T8=setjmp(_T7);if(!_T8)goto _TL19B;_T23=1;goto _TL19C;_TL19B: _TL19C: if(_T23)goto _TL19D;else{goto _TL19F;}_TL19F: _TA=Cyc_Hashtable_lookup;{
# 1115
void*(*_T24)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(void*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_TA;_T9=_T24;}_TB=cvmap;{struct _fat_ptr*_T24=_cycalloc(sizeof(struct _fat_ptr));*_T24=s;_TC=(struct _fat_ptr*)_T24;}{void*_T24=_T9(_TB,_TC);_npop_handler(0);return _T24;}_pop_handler();goto _TL19E;_TL19D: _TD=Cyc_Core_get_exn_thrown();{void*_T24=(void*)_TD;void*_T25;_TE=(struct Cyc_Core_Not_found_exn_struct*)_T24;_TF=_TE->tag;_T10=Cyc_Core_Not_found;if(_TF!=_T10)goto _TL1A0;{
# 1118
struct _fat_ptr kind=Cyc_strchr(s,'_');_T11=kind;_T12=
_fat_ptr_plus(_T11,sizeof(char),1);{struct _fat_ptr name=Cyc_strchr(_T12,'_');_T13=& name;
_fat_ptr_inplace_plus(_T13,sizeof(char),1);_T14=kind;_T15=
_tag_fat("_PTRBND",sizeof(char),8U);_T16=Cyc_strncmp(_T14,_T15,7U);if(_T16)goto _TL1A2;else{goto _TL1A4;}
_TL1A4: _T17=& Cyc_Kinds_ptrbko;_T18=(struct Cyc_Core_Opt*)_T17;_T19=name;{void*t=Cyc_Absyn_cvar_type_name(_T18,_T19);_T1B=Cyc_Hashtable_insert;{
void(*_T26)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*))_T1B;_T1A=_T26;}_T1C=_check_null(cvmap);{struct _fat_ptr*_T26=_cycalloc(sizeof(struct _fat_ptr));*_T26=s;_T1D=(struct _fat_ptr*)_T26;}_T1E=t;_T1A(_T1C,_T1D,_T1E);_T1F=t;
return _T1F;}
# 1127
_TL1A2:{int(*_T26)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T20=_T26;}_T21=_tag_fat("Constraint variable unknown kind",sizeof(char),33U);_T20(0U,_T21);;}}goto _TL1A1;_TL1A0: _T25=_T24;{void*exn=_T25;_rethrow(exn);}_TL1A1:;}_TL19E:;}}}
# 1133
static void*Cyc_Parse_str2type(unsigned loc,struct _fat_ptr s){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;void*_T3;void*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;int _T7;void*_T8;void*_T9;void*_TA;int(*_TB)(unsigned,struct _fat_ptr);unsigned _TC;struct _fat_ptr _TD;struct Cyc_String_pa_PrintArg_struct _TE;struct _fat_ptr _TF;struct _fat_ptr _T10;_T0=s;_T1=
_tag_fat("@fat",sizeof(char),5U);_T2=Cyc_strcmp(_T0,_T1);if(_T2)goto _TL1A5;else{goto _TL1A7;}
_TL1A7: _T3=Cyc_Tcutil_ptrbnd_cvar_equivalent(Cyc_Absyn_fat_bound_type);_T4=_check_null(_T3);return _T4;
# 1137
_TL1A5: _T5=s;_T6=_tag_fat("@thin @numelts{valueof_t(1U)}",sizeof(char),30U);_T7=Cyc_strcmp(_T5,_T6);if(_T7)goto _TL1A8;else{goto _TL1AA;}
_TL1AA: _T8=Cyc_Absyn_bounds_one();_T9=Cyc_Tcutil_ptrbnd_cvar_equivalent(_T8);_TA=_check_null(_T9);return _TA;_TL1A8:{
# 1140
int(*_T11)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TB=_T11;}_TC=loc;{struct Cyc_String_pa_PrintArg_struct _T11;_T11.tag=0;_T11.f1=s;_TE=_T11;}{struct Cyc_String_pa_PrintArg_struct _T11=_TE;void*_T12[1];_T12[0]=& _T11;_TF=_tag_fat("Unknown type constant:: %s",sizeof(char),27U);_T10=_tag_fat(_T12,sizeof(void*),1);_TD=Cyc_aprintf(_TF,_T10);}_TB(_TC,_TD);}
# 1143
static void*Cyc_Parse_composite_constraint(enum Cyc_Parse_ConstraintOps op,void*t1,void*t2){enum Cyc_Parse_ConstraintOps _T0;int _T1;void*_T2;void*_T3;void*_T4;void*_T5;void*_T6;void*_T7;void*_T8;int(*_T9)(unsigned,struct _fat_ptr);struct _fat_ptr _TA;_T0=op;_T1=(int)_T0;switch(_T1){case Cyc_Parse_C_AND_OP: _T2=t1;_T3=
# 1145
_check_null(t2);_T4=Cyc_BansheeIf_and_constraint(_T2,_T3);return _T4;case Cyc_Parse_C_OR_OP: _T5=t1;_T6=
_check_null(t2);_T7=Cyc_BansheeIf_or_constraint(_T5,_T6);return _T7;case Cyc_Parse_C_NOT_OP: _T8=
Cyc_BansheeIf_not_constraint(t1);return _T8;default:{
# 1149
int(*_TB)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T9=_TB;}_TA=_tag_fat("Unexpected operator for composite constraint",sizeof(char),45U);_T9(0U,_TA);};}
# 1153
static void*Cyc_Parse_comparison_constraint(enum Cyc_Parse_ConstraintOps op,void*t1,void*t2){enum Cyc_Parse_ConstraintOps _T0;int _T1;void*_T2;void*_T3;int(*_T4)(unsigned,struct _fat_ptr);struct _fat_ptr _T5;_T0=op;_T1=(int)_T0;switch(_T1){case Cyc_Parse_C_EQ_OP: _T2=
# 1155
Cyc_BansheeIf_cmpeq_constraint(t1,t2);return _T2;case Cyc_Parse_C_INCL_OP: _T3=
Cyc_BansheeIf_inclusion_constraint(t1,t2);return _T3;default:{
# 1158
int(*_T6)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T4=_T6;}_T5=_tag_fat("Unexpected operator for composite constraint",sizeof(char),45U);_T4(0U,_T5);};}struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f0;void*f1;void*f2;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f0;int f1;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f0;struct Cyc_Absyn_Pat*f1;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f0;enum Cyc_Absyn_AggrKind f1;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _union_YYSTYPE_YY29{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f0;struct Cyc_Parse_Type_specifier f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY36{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f0;int f1;struct Cyc_Absyn_VarargInfo*f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _tuple28{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _union_YYSTYPE_YY51{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _tuple29{void*f0;void*f1;};struct _union_YYSTYPE_YY53{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY54{int tag;void*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _tuple30{struct Cyc_List_List*f0;unsigned f1;};struct _union_YYSTYPE_YY56{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _union_YYSTYPE_YY59{int tag;void*val;};struct _union_YYSTYPE_YY60{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY62{int tag;struct _tuple21 val;};struct _union_YYSTYPE_YY63{int tag;void*val;};struct _tuple31{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY64{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY65{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY67{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f0;struct Cyc_Absyn_Exp*f1;};struct _union_YYSTYPE_YY68{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY69{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YY70{int tag;enum Cyc_Parse_ConstraintOps val;};struct _union_YYSTYPE_YY71{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY72{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY73{int tag;void*val;};struct _union_YYSTYPE_YY74{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YY67 YY67;struct _union_YYSTYPE_YY68 YY68;struct _union_YYSTYPE_YY69 YY69;struct _union_YYSTYPE_YY70 YY70;struct _union_YYSTYPE_YY71 YY71;struct _union_YYSTYPE_YY72 YY72;struct _union_YYSTYPE_YY73 YY73;struct _union_YYSTYPE_YY74 YY74;};
# 1251
static void Cyc_yythrowfail(struct _fat_ptr s){struct Cyc_Core_Failure_exn_struct*_T0;void*_T1;{struct Cyc_Core_Failure_exn_struct*_T2=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_T2->tag=Cyc_Core_Failure;
_T2->f1=s;_T0=(struct Cyc_Core_Failure_exn_struct*)_T2;}_T1=(void*)_T0;_throw(_T1);}static char _TmpG2[7U]="cnst_t";
# 1214 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Int_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Int_tok _T5;union Cyc_Absyn_Cnst _T6;
static struct _fat_ptr s={_TmpG2,_TmpG2,_TmpG2 + 7U};union Cyc_Absyn_Cnst _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Int_tok;_T3=_T2.tag;if(_T3!=2)goto _TL1AD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Int_tok;_T7=_T5.val;}{union Cyc_Absyn_Cnst yy=_T7;_T6=yy;
# 1217
return _T6;}_TL1AD:
 Cyc_yythrowfail(s);;}
# 1221
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Int_tok.tag=2U;_T1.Int_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3[5U]="char";
# 1215 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Char_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Char_tok _T5;char _T6;
static struct _fat_ptr s={_TmpG3,_TmpG3,_TmpG3 + 5U};char _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Char_tok;_T3=_T2.tag;if(_T3!=3)goto _TL1AF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Char_tok;_T7=_T5.val;}{char yy=_T7;_T6=yy;
# 1218
return _T6;}_TL1AF:
 Cyc_yythrowfail(s);;}
# 1222
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Char_tok.tag=3U;_T1.Char_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4[13U]="string_t<`H>";
# 1216 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_String_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_String_tok _T5;struct _fat_ptr _T6;
static struct _fat_ptr s={_TmpG4,_TmpG4,_TmpG4 + 13U};struct _fat_ptr _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->String_tok;_T3=_T2.tag;if(_T3!=4)goto _TL1B1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.String_tok;_T7=_T5.val;}{struct _fat_ptr yy=_T7;_T6=yy;
# 1219
return _T6;}_TL1B1:
 Cyc_yythrowfail(s);;}
# 1223
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.String_tok.tag=4U;_T1.String_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1219 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY1 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY1 _T5;struct _tuple22*_T6;
static struct _fat_ptr s={_TmpG5,_TmpG5,_TmpG5 + 35U};struct _tuple22*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY1;_T3=_T2.tag;if(_T3!=8)goto _TL1B3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY1;_T7=_T5.val;}{struct _tuple22*yy=_T7;_T6=yy;
# 1222
return _T6;}_TL1B3:
 Cyc_yythrowfail(s);;}
# 1226
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY1.tag=8U;_T1.YY1.val=yy1;_T0=_T1;}return _T0;}static char _TmpG6[11U]="ptrbound_t";
# 1220 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY2 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY2 _T5;void*_T6;
static struct _fat_ptr s={_TmpG6,_TmpG6,_TmpG6 + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY2;_T3=_T2.tag;if(_T3!=9)goto _TL1B5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY2;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1223
return _T6;}_TL1B5:
 Cyc_yythrowfail(s);;}
# 1227
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY2.tag=9U;_T1.YY2.val=yy1;_T0=_T1;}return _T0;}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1221 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY3 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY3 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG7,_TmpG7,_TmpG7 + 28U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY3;_T3=_T2.tag;if(_T3!=10)goto _TL1B7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY3;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1224
return _T6;}_TL1B7:
 Cyc_yythrowfail(s);;}
# 1228
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY3.tag=10U;_T1.YY3.val=yy1;_T0=_T1;}return _T0;}static char _TmpG8[6U]="exp_t";
# 1222 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Exp_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Exp_tok _T5;struct Cyc_Absyn_Exp*_T6;
static struct _fat_ptr s={_TmpG8,_TmpG8,_TmpG8 + 6U};struct Cyc_Absyn_Exp*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Exp_tok;_T3=_T2.tag;if(_T3!=6)goto _TL1B9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Exp_tok;_T7=_T5.val;}{struct Cyc_Absyn_Exp*yy=_T7;_T6=yy;
# 1225
return _T6;}_TL1B9:
 Cyc_yythrowfail(s);;}
# 1229
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Exp_tok.tag=6U;_T1.Exp_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY4 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY4 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG9,_TmpG9,_TmpG9 + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY4;_T3=_T2.tag;if(_T3!=11)goto _TL1BB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY4;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1233
return _T6;}_TL1BB:
 Cyc_yythrowfail(s);;}
# 1237
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY4.tag=11U;_T1.YY4.val=yy1;_T0=_T1;}return _T0;}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1231 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY5 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY5 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpGA,_TmpGA,_TmpGA + 47U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY5;_T3=_T2.tag;if(_T3!=12)goto _TL1BD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY5;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1234
return _T6;}_TL1BD:
 Cyc_yythrowfail(s);;}
# 1238
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY5.tag=12U;_T1.YY5.val=yy1;_T0=_T1;}return _T0;}static char _TmpGB[9U]="primop_t";
# 1232 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY6 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY6 _T5;enum Cyc_Absyn_Primop _T6;
static struct _fat_ptr s={_TmpGB,_TmpGB,_TmpGB + 9U};enum Cyc_Absyn_Primop _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY6;_T3=_T2.tag;if(_T3!=13)goto _TL1BF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY6;_T7=_T5.val;}{enum Cyc_Absyn_Primop yy=_T7;_T6=yy;
# 1235
return _T6;}_TL1BF:
 Cyc_yythrowfail(s);;}
# 1239
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY6.tag=13U;_T1.YY6.val=yy1;_T0=_T1;}return _T0;}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1233 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY7 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY7 _T5;struct Cyc_Core_Opt*_T6;
static struct _fat_ptr s={_TmpGC,_TmpGC,_TmpGC + 19U};struct Cyc_Core_Opt*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY7;_T3=_T2.tag;if(_T3!=14)goto _TL1C1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY7;_T7=_T5.val;}{struct Cyc_Core_Opt*yy=_T7;_T6=yy;
# 1236
return _T6;}_TL1C1:
 Cyc_yythrowfail(s);;}
# 1240
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY7.tag=14U;_T1.YY7.val=yy1;_T0=_T1;}return _T0;}static char _TmpGD[7U]="qvar_t";
# 1234 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_QualId_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_QualId_tok _T5;struct _tuple0*_T6;
static struct _fat_ptr s={_TmpGD,_TmpGD,_TmpGD + 7U};struct _tuple0*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->QualId_tok;_T3=_T2.tag;if(_T3!=5)goto _TL1C3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.QualId_tok;_T7=_T5.val;}{struct _tuple0*yy=_T7;_T6=yy;
# 1237
return _T6;}_TL1C3:
 Cyc_yythrowfail(s);;}
# 1241
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.QualId_tok.tag=5U;_T1.QualId_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpGE[7U]="stmt_t";
# 1237 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Stmt_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Stmt_tok _T5;struct Cyc_Absyn_Stmt*_T6;
static struct _fat_ptr s={_TmpGE,_TmpGE,_TmpGE + 7U};struct Cyc_Absyn_Stmt*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Stmt_tok;_T3=_T2.tag;if(_T3!=7)goto _TL1C5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Stmt_tok;_T7=_T5.val;}{struct Cyc_Absyn_Stmt*yy=_T7;_T6=yy;
# 1240
return _T6;}_TL1C5:
 Cyc_yythrowfail(s);;}
# 1244
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Stmt_tok.tag=7U;_T1.Stmt_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1240 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY8 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY8 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpGF,_TmpGF,_TmpGF + 27U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY8;_T3=_T2.tag;if(_T3!=15)goto _TL1C7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY8;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1243
return _T6;}_TL1C7:
 Cyc_yythrowfail(s);;}
# 1247
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY8.tag=15U;_T1.YY8.val=yy1;_T0=_T1;}return _T0;}static char _TmpG10[6U]="pat_t";
# 1241 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY9 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY9 _T5;struct Cyc_Absyn_Pat*_T6;
static struct _fat_ptr s={_TmpG10,_TmpG10,_TmpG10 + 6U};struct Cyc_Absyn_Pat*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY9;_T3=_T2.tag;if(_T3!=16)goto _TL1C9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY9;_T7=_T5.val;}{struct Cyc_Absyn_Pat*yy=_T7;_T6=yy;
# 1244
return _T6;}_TL1C9:
 Cyc_yythrowfail(s);;}
# 1248
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY9.tag=16U;_T1.YY9.val=yy1;_T0=_T1;}return _T0;}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1246 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY10 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY10 _T5;struct _tuple23*_T6;
static struct _fat_ptr s={_TmpG11,_TmpG11,_TmpG11 + 28U};struct _tuple23*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY10;_T3=_T2.tag;if(_T3!=17)goto _TL1CB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY10;_T7=_T5.val;}{struct _tuple23*yy=_T7;_T6=yy;
# 1249
return _T6;}_TL1CB:
 Cyc_yythrowfail(s);;}
# 1253
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY10.tag=17U;_T1.YY10.val=yy1;_T0=_T1;}return _T0;}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1247 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY11 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY11 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG12,_TmpG12,_TmpG12 + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY11;_T3=_T2.tag;if(_T3!=18)goto _TL1CD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY11;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1250
return _T6;}_TL1CD:
 Cyc_yythrowfail(s);;}
# 1254
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY11.tag=18U;_T1.YY11.val=yy1;_T0=_T1;}return _T0;}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1248 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY12 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY12 _T5;struct _tuple24*_T6;
static struct _fat_ptr s={_TmpG13,_TmpG13,_TmpG13 + 36U};struct _tuple24*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY12;_T3=_T2.tag;if(_T3!=19)goto _TL1CF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY12;_T7=_T5.val;}{struct _tuple24*yy=_T7;_T6=yy;
# 1251
return _T6;}_TL1CF:
 Cyc_yythrowfail(s);;}
# 1255
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY12.tag=19U;_T1.YY12.val=yy1;_T0=_T1;}return _T0;}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1249 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY13 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY13 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG14,_TmpG14,_TmpG14 + 47U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY13;_T3=_T2.tag;if(_T3!=20)goto _TL1D1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY13;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1252
return _T6;}_TL1D1:
 Cyc_yythrowfail(s);;}
# 1256
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY13.tag=20U;_T1.YY13.val=yy1;_T0=_T1;}return _T0;}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1250 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY14 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY14 _T5;struct _tuple23*_T6;
static struct _fat_ptr s={_TmpG15,_TmpG15,_TmpG15 + 58U};struct _tuple23*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY14;_T3=_T2.tag;if(_T3!=21)goto _TL1D3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY14;_T7=_T5.val;}{struct _tuple23*yy=_T7;_T6=yy;
# 1253
return _T6;}_TL1D3:
 Cyc_yythrowfail(s);;}
# 1257
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY14.tag=21U;_T1.YY14.val=yy1;_T0=_T1;}return _T0;}static char _TmpG16[9U]="fndecl_t";
# 1251 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY15 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY15 _T5;struct Cyc_Absyn_Fndecl*_T6;
static struct _fat_ptr s={_TmpG16,_TmpG16,_TmpG16 + 9U};struct Cyc_Absyn_Fndecl*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY15;_T3=_T2.tag;if(_T3!=22)goto _TL1D5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY15;_T7=_T5.val;}{struct Cyc_Absyn_Fndecl*yy=_T7;_T6=yy;
# 1254
return _T6;}_TL1D5:
 Cyc_yythrowfail(s);;}
# 1258
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY15.tag=22U;_T1.YY15.val=yy1;_T0=_T1;}return _T0;}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1252 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY16 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY16 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG17,_TmpG17,_TmpG17 + 18U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY16;_T3=_T2.tag;if(_T3!=23)goto _TL1D7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY16;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1255
return _T6;}_TL1D7:
 Cyc_yythrowfail(s);;}
# 1259
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY16.tag=23U;_T1.YY16.val=yy1;_T0=_T1;}return _T0;}static char _TmpG18[12U]="decl_spec_t";
# 1255 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY17 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY17 _T5;struct Cyc_Parse_Declaration_spec _T6;
static struct _fat_ptr s={_TmpG18,_TmpG18,_TmpG18 + 12U};struct Cyc_Parse_Declaration_spec _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY17;_T3=_T2.tag;if(_T3!=24)goto _TL1D9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY17;_T7=_T5.val;}{struct Cyc_Parse_Declaration_spec yy=_T7;_T6=yy;
# 1258
return _T6;}_TL1D9:
 Cyc_yythrowfail(s);;}
# 1262
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY17.tag=24U;_T1.YY17.val=yy1;_T0=_T1;}return _T0;}static char _TmpG19[41U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)";
# 1256 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY18 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY18 _T5;struct _tuple12 _T6;
static struct _fat_ptr s={_TmpG19,_TmpG19,_TmpG19 + 41U};struct _tuple12 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY18;_T3=_T2.tag;if(_T3!=25)goto _TL1DB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY18;_T7=_T5.val;}{struct _tuple12 yy=_T7;_T6=yy;
# 1259
return _T6;}_TL1DB:
 Cyc_yythrowfail(s);;}
# 1263
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY18.tag=25U;_T1.YY18.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1257 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY19 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY19 _T5;struct _tuple11*_T6;
static struct _fat_ptr s={_TmpG1A,_TmpG1A,_TmpG1A + 23U};struct _tuple11*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY19;_T3=_T2.tag;if(_T3!=26)goto _TL1DD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY19;_T7=_T5.val;}{struct _tuple11*yy=_T7;_T6=yy;
# 1260
return _T6;}_TL1DD:
 Cyc_yythrowfail(s);;}
# 1264
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY19.tag=26U;_T1.YY19.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1B[16U]="storage_class_t";
# 1258 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY20 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY20 _T5;enum Cyc_Parse_Storage_class _T6;
static struct _fat_ptr s={_TmpG1B,_TmpG1B,_TmpG1B + 16U};enum Cyc_Parse_Storage_class _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY20;_T3=_T2.tag;if(_T3!=27)goto _TL1DF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY20;_T7=_T5.val;}{enum Cyc_Parse_Storage_class yy=_T7;_T6=yy;
# 1261
return _T6;}_TL1DF:
 Cyc_yythrowfail(s);;}
# 1265
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY20.tag=27U;_T1.YY20.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1C[17U]="type_specifier_t";
# 1259 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY21 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY21 _T5;struct Cyc_Parse_Type_specifier _T6;
static struct _fat_ptr s={_TmpG1C,_TmpG1C,_TmpG1C + 17U};struct Cyc_Parse_Type_specifier _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY21;_T3=_T2.tag;if(_T3!=28)goto _TL1E1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY21;_T7=_T5.val;}{struct Cyc_Parse_Type_specifier yy=_T7;_T6=yy;
# 1262
return _T6;}_TL1E1:
 Cyc_yythrowfail(s);;}
# 1266
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY21.tag=28U;_T1.YY21.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1D[12U]="aggr_kind_t";
# 1261 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY22 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY22 _T5;enum Cyc_Absyn_AggrKind _T6;
static struct _fat_ptr s={_TmpG1D,_TmpG1D,_TmpG1D + 12U};enum Cyc_Absyn_AggrKind _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY22;_T3=_T2.tag;if(_T3!=29)goto _TL1E3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY22;_T7=_T5.val;}{enum Cyc_Absyn_AggrKind yy=_T7;_T6=yy;
# 1264
return _T6;}_TL1E3:
 Cyc_yythrowfail(s);;}
# 1268
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY22.tag=29U;_T1.YY22.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1262 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY23 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY23 _T5;struct _tuple25 _T6;
static struct _fat_ptr s={_TmpG1E,_TmpG1E,_TmpG1E + 20U};struct _tuple25 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY23;_T3=_T2.tag;if(_T3!=30)goto _TL1E5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY23;_T7=_T5.val;}{struct _tuple25 yy=_T7;_T6=yy;
# 1265
return _T6;}_TL1E5:
 Cyc_yythrowfail(s);;}
# 1269
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY23.tag=30U;_T1.YY23.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1F[8U]="tqual_t";
# 1263 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY24 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY24 _T5;struct Cyc_Absyn_Tqual _T6;
static struct _fat_ptr s={_TmpG1F,_TmpG1F,_TmpG1F + 8U};struct Cyc_Absyn_Tqual _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY24;_T3=_T2.tag;if(_T3!=31)goto _TL1E7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY24;_T7=_T5.val;}{struct Cyc_Absyn_Tqual yy=_T7;_T6=yy;
# 1266
return _T6;}_TL1E7:
 Cyc_yythrowfail(s);;}
# 1270
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY24.tag=31U;_T1.YY24.val=yy1;_T0=_T1;}return _T0;}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1264 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY25 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY25 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG20,_TmpG20,_TmpG20 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY25;_T3=_T2.tag;if(_T3!=32)goto _TL1E9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY25;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1267
return _T6;}_TL1E9:
 Cyc_yythrowfail(s);;}
# 1271
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY25.tag=32U;_T1.YY25.val=yy1;_T0=_T1;}return _T0;}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1265 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY26 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY26 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG21,_TmpG21,_TmpG21 + 34U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY26;_T3=_T2.tag;if(_T3!=33)goto _TL1EB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY26;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1268
return _T6;}_TL1EB:
 Cyc_yythrowfail(s);;}
# 1272
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY26.tag=33U;_T1.YY26.val=yy1;_T0=_T1;}return _T0;}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1266 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY27 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY27 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG22,_TmpG22,_TmpG22 + 33U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY27;_T3=_T2.tag;if(_T3!=34)goto _TL1ED;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY27;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1269
return _T6;}_TL1ED:
 Cyc_yythrowfail(s);;}
# 1273
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY27.tag=34U;_T1.YY27.val=yy1;_T0=_T1;}return _T0;}static char _TmpG23[18U]="declarator_t<`yy>";
# 1267 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY28 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY28 _T5;struct Cyc_Parse_Declarator _T6;
static struct _fat_ptr s={_TmpG23,_TmpG23,_TmpG23 + 18U};struct Cyc_Parse_Declarator _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY28;_T3=_T2.tag;if(_T3!=35)goto _TL1EF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY28;_T7=_T5.val;}{struct Cyc_Parse_Declarator yy=_T7;_T6=yy;
# 1270
return _T6;}_TL1EF:
 Cyc_yythrowfail(s);;}
# 1274
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY28.tag=35U;_T1.YY28.val=yy1;_T0=_T1;}return _T0;}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1268 "parse.y"
static struct _tuple12*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY29 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY29 _T5;struct _tuple12*_T6;
static struct _fat_ptr s={_TmpG24,_TmpG24,_TmpG24 + 45U};struct _tuple12*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY29;_T3=_T2.tag;if(_T3!=36)goto _TL1F1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY29;_T7=_T5.val;}{struct _tuple12*yy=_T7;_T6=yy;
# 1271
return _T6;}_TL1F1:
 Cyc_yythrowfail(s);;}
# 1275
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple12*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY29.tag=36U;_T1.YY29.val=yy1;_T0=_T1;}return _T0;}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1269 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY30 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY30 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG25,_TmpG25,_TmpG25 + 57U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY30;_T3=_T2.tag;if(_T3!=37)goto _TL1F3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY30;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1272
return _T6;}_TL1F3:
 Cyc_yythrowfail(s);;}
# 1276
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY30.tag=37U;_T1.YY30.val=yy1;_T0=_T1;}return _T0;}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1270 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY31 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY31 _T5;struct Cyc_Parse_Abstractdeclarator _T6;
static struct _fat_ptr s={_TmpG26,_TmpG26,_TmpG26 + 26U};struct Cyc_Parse_Abstractdeclarator _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY31;_T3=_T2.tag;if(_T3!=38)goto _TL1F5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY31;_T7=_T5.val;}{struct Cyc_Parse_Abstractdeclarator yy=_T7;_T6=yy;
# 1273
return _T6;}_TL1F5:
 Cyc_yythrowfail(s);;}
# 1277
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY31.tag=38U;_T1.YY31.val=yy1;_T0=_T1;}return _T0;}static char _TmpG27[5U]="bool";
# 1271 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY32 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY32 _T5;int _T6;
static struct _fat_ptr s={_TmpG27,_TmpG27,_TmpG27 + 5U};int _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY32;_T3=_T2.tag;if(_T3!=39)goto _TL1F7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY32;_T7=_T5.val;}{int yy=_T7;_T6=yy;
# 1274
return _T6;}_TL1F7:
 Cyc_yythrowfail(s);;}
# 1278
static union Cyc_YYSTYPE Cyc_YY32(int yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY32.tag=39U;_T1.YY32.val=yy1;_T0=_T1;}return _T0;}static char _TmpG28[8U]="scope_t";
# 1272 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY33 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY33 _T5;enum Cyc_Absyn_Scope _T6;
static struct _fat_ptr s={_TmpG28,_TmpG28,_TmpG28 + 8U};enum Cyc_Absyn_Scope _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY33;_T3=_T2.tag;if(_T3!=40)goto _TL1F9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY33;_T7=_T5.val;}{enum Cyc_Absyn_Scope yy=_T7;_T6=yy;
# 1275
return _T6;}_TL1F9:
 Cyc_yythrowfail(s);;}
# 1279
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY33.tag=40U;_T1.YY33.val=yy1;_T0=_T1;}return _T0;}static char _TmpG29[16U]="datatypefield_t";
# 1273 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY34 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY34 _T5;struct Cyc_Absyn_Datatypefield*_T6;
static struct _fat_ptr s={_TmpG29,_TmpG29,_TmpG29 + 16U};struct Cyc_Absyn_Datatypefield*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY34;_T3=_T2.tag;if(_T3!=41)goto _TL1FB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY34;_T7=_T5.val;}{struct Cyc_Absyn_Datatypefield*yy=_T7;_T6=yy;
# 1276
return _T6;}_TL1FB:
 Cyc_yythrowfail(s);;}
# 1280
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY34.tag=41U;_T1.YY34.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1274 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY35 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY35 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG2A,_TmpG2A,_TmpG2A + 27U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY35;_T3=_T2.tag;if(_T3!=42)goto _TL1FD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY35;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1277
return _T6;}_TL1FD:
 Cyc_yythrowfail(s);;}
# 1281
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY35.tag=42U;_T1.YY35.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1275 "parse.y"
static struct _tuple26 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY36 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY36 _T5;struct _tuple26 _T6;
static struct _fat_ptr s={_TmpG2B,_TmpG2B,_TmpG2B + 41U};struct _tuple26 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY36;_T3=_T2.tag;if(_T3!=43)goto _TL1FF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY36;_T7=_T5.val;}{struct _tuple26 yy=_T7;_T6=yy;
# 1278
return _T6;}_TL1FF:
 Cyc_yythrowfail(s);;}
# 1282
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple26 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY36.tag=43U;_T1.YY36.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1276 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY37 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY37 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG2C,_TmpG2C,_TmpG2C + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY37;_T3=_T2.tag;if(_T3!=44)goto _TL201;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY37;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1279
return _T6;}_TL201:
 Cyc_yythrowfail(s);;}
# 1283
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY37.tag=44U;_T1.YY37.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1277 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY38 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY38 _T5;struct _tuple8*_T6;
static struct _fat_ptr s={_TmpG2D,_TmpG2D,_TmpG2D + 31U};struct _tuple8*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY38;_T3=_T2.tag;if(_T3!=45)goto _TL203;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY38;_T7=_T5.val;}{struct _tuple8*yy=_T7;_T6=yy;
# 1280
return _T6;}_TL203:
 Cyc_yythrowfail(s);;}
# 1284
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY38.tag=45U;_T1.YY38.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1278 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY39 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY39 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG2E,_TmpG2E,_TmpG2E + 42U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY39;_T3=_T2.tag;if(_T3!=46)goto _TL205;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY39;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1281
return _T6;}_TL205:
 Cyc_yythrowfail(s);;}
# 1285
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY39.tag=46U;_T1.YY39.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2F[139U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1279 "parse.y"
static struct _tuple27*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY40 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY40 _T5;struct _tuple27*_T6;
static struct _fat_ptr s={_TmpG2F,_TmpG2F,_TmpG2F + 139U};struct _tuple27*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY40;_T3=_T2.tag;if(_T3!=47)goto _TL207;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY40;_T7=_T5.val;}{struct _tuple27*yy=_T7;_T6=yy;
# 1282
return _T6;}_TL207:
 Cyc_yythrowfail(s);;}
# 1286
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple27*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY40.tag=47U;_T1.YY40.val=yy1;_T0=_T1;}return _T0;}static char _TmpG30[8U]="types_t";
# 1280 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY41 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY41 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG30,_TmpG30,_TmpG30 + 8U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY41;_T3=_T2.tag;if(_T3!=48)goto _TL209;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY41;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1283
return _T6;}_TL209:
 Cyc_yythrowfail(s);;}
# 1287
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY41.tag=48U;_T1.YY41.val=yy1;_T0=_T1;}return _T0;}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1282 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY42 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY42 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG31,_TmpG31,_TmpG31 + 24U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY42;_T3=_T2.tag;if(_T3!=49)goto _TL20B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY42;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1285
return _T6;}_TL20B:
 Cyc_yythrowfail(s);;}
# 1289
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY42.tag=49U;_T1.YY42.val=yy1;_T0=_T1;}return _T0;}static char _TmpG32[13U]="designator_t";
# 1283 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY43 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY43 _T5;void*_T6;
static struct _fat_ptr s={_TmpG32,_TmpG32,_TmpG32 + 13U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY43;_T3=_T2.tag;if(_T3!=50)goto _TL20D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY43;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1286
return _T6;}_TL20D:
 Cyc_yythrowfail(s);;}
# 1290
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY43.tag=50U;_T1.YY43.val=yy1;_T0=_T1;}return _T0;}static char _TmpG33[7U]="kind_t";
# 1284 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY44 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY44 _T5;struct Cyc_Absyn_Kind*_T6;
static struct _fat_ptr s={_TmpG33,_TmpG33,_TmpG33 + 7U};struct Cyc_Absyn_Kind*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY44;_T3=_T2.tag;if(_T3!=51)goto _TL20F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY44;_T7=_T5.val;}{struct Cyc_Absyn_Kind*yy=_T7;_T6=yy;
# 1287
return _T6;}_TL20F:
 Cyc_yythrowfail(s);;}
# 1291
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY44.tag=51U;_T1.YY44.val=yy1;_T0=_T1;}return _T0;}static char _TmpG34[7U]="type_t";
# 1285 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY45 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY45 _T5;void*_T6;
static struct _fat_ptr s={_TmpG34,_TmpG34,_TmpG34 + 7U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY45;_T3=_T2.tag;if(_T3!=52)goto _TL211;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY45;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1288
return _T6;}_TL211:
 Cyc_yythrowfail(s);;}
# 1292
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY45.tag=52U;_T1.YY45.val=yy1;_T0=_T1;}return _T0;}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1286 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY46 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY46 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG35,_TmpG35,_TmpG35 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY46;_T3=_T2.tag;if(_T3!=53)goto _TL213;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY46;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1289
return _T6;}_TL213:
 Cyc_yythrowfail(s);;}
# 1293
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY46.tag=53U;_T1.YY46.val=yy1;_T0=_T1;}return _T0;}static char _TmpG36[12U]="attribute_t";
# 1287 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY47 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY47 _T5;void*_T6;
static struct _fat_ptr s={_TmpG36,_TmpG36,_TmpG36 + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY47;_T3=_T2.tag;if(_T3!=54)goto _TL215;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY47;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1290
return _T6;}_TL215:
 Cyc_yythrowfail(s);;}
# 1294
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY47.tag=54U;_T1.YY47.val=yy1;_T0=_T1;}return _T0;}static char _TmpG37[12U]="enumfield_t";
# 1288 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY48 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY48 _T5;struct Cyc_Absyn_Enumfield*_T6;
static struct _fat_ptr s={_TmpG37,_TmpG37,_TmpG37 + 12U};struct Cyc_Absyn_Enumfield*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY48;_T3=_T2.tag;if(_T3!=55)goto _TL217;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY48;_T7=_T5.val;}{struct Cyc_Absyn_Enumfield*yy=_T7;_T6=yy;
# 1291
return _T6;}_TL217:
 Cyc_yythrowfail(s);;}
# 1295
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY48.tag=55U;_T1.YY48.val=yy1;_T0=_T1;}return _T0;}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1289 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY49 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY49 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG38,_TmpG38,_TmpG38 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY49;_T3=_T2.tag;if(_T3!=56)goto _TL219;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY49;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1292
return _T6;}_TL219:
 Cyc_yythrowfail(s);;}
# 1296
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY49.tag=56U;_T1.YY49.val=yy1;_T0=_T1;}return _T0;}static char _TmpG39[11U]="type_opt_t";
# 1290 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY50 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY50 _T5;void*_T6;
static struct _fat_ptr s={_TmpG39,_TmpG39,_TmpG39 + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY50;_T3=_T2.tag;if(_T3!=57)goto _TL21B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY50;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1293
return _T6;}_TL21B:
 Cyc_yythrowfail(s);;}
# 1297
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY50.tag=57U;_T1.YY50.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3A[61U]="$(list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)*`H";
# 1292 "parse.y"
static struct _tuple28*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY51 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY51 _T5;struct _tuple28*_T6;
static struct _fat_ptr s={_TmpG3A,_TmpG3A,_TmpG3A + 61U};struct _tuple28*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY51;_T3=_T2.tag;if(_T3!=58)goto _TL21D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY51;_T7=_T5.val;}{struct _tuple28*yy=_T7;_T6=yy;
# 1295
return _T6;}_TL21D:
 Cyc_yythrowfail(s);;}
# 1299
static union Cyc_YYSTYPE Cyc_YY51(struct _tuple28*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY51.tag=58U;_T1.YY51.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3B[12U]="effconstr_t";
# 1293 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY52 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY52 _T5;void*_T6;
static struct _fat_ptr s={_TmpG3B,_TmpG3B,_TmpG3B + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY52;_T3=_T2.tag;if(_T3!=59)goto _TL21F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY52;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1296
return _T6;}_TL21F:
 Cyc_yythrowfail(s);;}
# 1300
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY52.tag=59U;_T1.YY52.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3C[21U]="$(type_t, type_t)@`H";
# 1294 "parse.y"
static struct _tuple29*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY53 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY53 _T5;struct _tuple29*_T6;
static struct _fat_ptr s={_TmpG3C,_TmpG3C,_TmpG3C + 21U};struct _tuple29*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY53;_T3=_T2.tag;if(_T3!=60)goto _TL221;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY53;_T7=_T5.val;}{struct _tuple29*yy=_T7;_T6=yy;
# 1297
return _T6;}_TL221:
 Cyc_yythrowfail(s);;}
# 1301
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple29*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY53.tag=60U;_T1.YY53.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3D[11U]="booltype_t";
# 1295 "parse.y"
static void*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY54 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY54 _T5;void*_T6;
static struct _fat_ptr s={_TmpG3D,_TmpG3D,_TmpG3D + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY54;_T3=_T2.tag;if(_T3!=61)goto _TL223;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY54;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1298
return _T6;}_TL223:
 Cyc_yythrowfail(s);;}
# 1302
static union Cyc_YYSTYPE Cyc_YY54(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY54.tag=61U;_T1.YY54.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3E[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1296 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY55 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY55 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG3E,_TmpG3E,_TmpG3E + 35U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY55;_T3=_T2.tag;if(_T3!=62)goto _TL225;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY55;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1299
return _T6;}_TL225:
 Cyc_yythrowfail(s);;}
# 1303
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY55.tag=62U;_T1.YY55.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3F[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1297 "parse.y"
static struct _tuple30*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY56 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY56 _T5;struct _tuple30*_T6;
static struct _fat_ptr s={_TmpG3F,_TmpG3F,_TmpG3F + 48U};struct _tuple30*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY56;_T3=_T2.tag;if(_T3!=63)goto _TL227;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY56;_T7=_T5.val;}{struct _tuple30*yy=_T7;_T6=yy;
# 1300
return _T6;}_TL227:
 Cyc_yythrowfail(s);;}
# 1304
static union Cyc_YYSTYPE Cyc_YY56(struct _tuple30*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY56.tag=63U;_T1.YY56.val=yy1;_T0=_T1;}return _T0;}static char _TmpG40[18U]="list_t<qvar_t,`H>";
# 1298 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY57 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY57 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG40,_TmpG40,_TmpG40 + 18U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY57;_T3=_T2.tag;if(_T3!=64)goto _TL229;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY57;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1301
return _T6;}_TL229:
 Cyc_yythrowfail(s);;}
# 1305
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY57.tag=64U;_T1.YY57.val=yy1;_T0=_T1;}return _T0;}static char _TmpG41[12U]="aqualtype_t";
# 1299 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY58 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY58 _T5;void*_T6;
static struct _fat_ptr s={_TmpG41,_TmpG41,_TmpG41 + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY58;_T3=_T2.tag;if(_T3!=65)goto _TL22B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY58;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1302
return _T6;}_TL22B:
 Cyc_yythrowfail(s);;}
# 1306
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY58.tag=65U;_T1.YY58.val=yy1;_T0=_T1;}return _T0;}static char _TmpG42[20U]="pointer_qual_t<`yy>";
# 1300 "parse.y"
static void*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY59 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY59 _T5;void*_T6;
static struct _fat_ptr s={_TmpG42,_TmpG42,_TmpG42 + 20U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY59;_T3=_T2.tag;if(_T3!=66)goto _TL22D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY59;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1303
return _T6;}_TL22D:
 Cyc_yythrowfail(s);;}
# 1307
static union Cyc_YYSTYPE Cyc_YY59(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY59.tag=66U;_T1.YY59.val=yy1;_T0=_T1;}return _T0;}static char _TmpG43[21U]="pointer_quals_t<`yy>";
# 1301 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY60 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY60 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG43,_TmpG43,_TmpG43 + 21U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY60;_T3=_T2.tag;if(_T3!=67)goto _TL22F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY60;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1304
return _T6;}_TL22F:
 Cyc_yythrowfail(s);;}
# 1308
static union Cyc_YYSTYPE Cyc_YY60(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY60.tag=67U;_T1.YY60.val=yy1;_T0=_T1;}return _T0;}static char _TmpG44[10U]="exp_opt_t";
# 1302 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY61 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY61 _T5;struct Cyc_Absyn_Exp*_T6;
static struct _fat_ptr s={_TmpG44,_TmpG44,_TmpG44 + 10U};struct Cyc_Absyn_Exp*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY61;_T3=_T2.tag;if(_T3!=68)goto _TL231;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY61;_T7=_T5.val;}{struct Cyc_Absyn_Exp*yy=_T7;_T6=yy;
# 1305
return _T6;}_TL231:
 Cyc_yythrowfail(s);;}
# 1309
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY61.tag=68U;_T1.YY61.val=yy1;_T0=_T1;}return _T0;}static char _TmpG45[43U]="$(exp_opt_t,exp_opt_t,exp_opt_t,exp_opt_t)";
# 1303 "parse.y"
static struct _tuple21 Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY62 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY62 _T5;struct _tuple21 _T6;
static struct _fat_ptr s={_TmpG45,_TmpG45,_TmpG45 + 43U};struct _tuple21 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY62;_T3=_T2.tag;if(_T3!=69)goto _TL233;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY62;_T7=_T5.val;}{struct _tuple21 yy=_T7;_T6=yy;
# 1306
return _T6;}_TL233:
 Cyc_yythrowfail(s);;}
# 1310
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple21 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY62.tag=69U;_T1.YY62.val=yy1;_T0=_T1;}return _T0;}static char _TmpG46[10U]="raw_exp_t";
# 1305 "parse.y"
static void*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY63 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY63 _T5;void*_T6;
static struct _fat_ptr s={_TmpG46,_TmpG46,_TmpG46 + 10U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY63;_T3=_T2.tag;if(_T3!=70)goto _TL235;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY63;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1308
return _T6;}_TL235:
 Cyc_yythrowfail(s);;}
# 1312
static union Cyc_YYSTYPE Cyc_YY63(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY63.tag=70U;_T1.YY63.val=yy1;_T0=_T1;}return _T0;}static char _TmpG47[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1306 "parse.y"
static struct _tuple31*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY64 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY64 _T5;struct _tuple31*_T6;
static struct _fat_ptr s={_TmpG47,_TmpG47,_TmpG47 + 112U};struct _tuple31*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY64;_T3=_T2.tag;if(_T3!=71)goto _TL237;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY64;_T7=_T5.val;}{struct _tuple31*yy=_T7;_T6=yy;
# 1309
return _T6;}_TL237:
 Cyc_yythrowfail(s);;}
# 1313
static union Cyc_YYSTYPE Cyc_YY64(struct _tuple31*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY64.tag=71U;_T1.YY64.val=yy1;_T0=_T1;}return _T0;}static char _TmpG48[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1307 "parse.y"
static struct _tuple28*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY65 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY65 _T5;struct _tuple28*_T6;
static struct _fat_ptr s={_TmpG48,_TmpG48,_TmpG48 + 73U};struct _tuple28*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY65;_T3=_T2.tag;if(_T3!=72)goto _TL239;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY65;_T7=_T5.val;}{struct _tuple28*yy=_T7;_T6=yy;
# 1310
return _T6;}_TL239:
 Cyc_yythrowfail(s);;}
# 1314
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple28*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY65.tag=72U;_T1.YY65.val=yy1;_T0=_T1;}return _T0;}static char _TmpG49[28U]="list_t<string_t<`H>@`H, `H>";
# 1308 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY66 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY66 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG49,_TmpG49,_TmpG49 + 28U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY66;_T3=_T2.tag;if(_T3!=73)goto _TL23B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY66;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1311
return _T6;}_TL23B:
 Cyc_yythrowfail(s);;}
# 1315
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY66.tag=73U;_T1.YY66.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4A[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1309 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY67(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY67 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY67 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG4A,_TmpG4A,_TmpG4A + 38U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY67;_T3=_T2.tag;if(_T3!=74)goto _TL23D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY67;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1312
return _T6;}_TL23D:
 Cyc_yythrowfail(s);;}
# 1316
static union Cyc_YYSTYPE Cyc_YY67(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY67.tag=74U;_T1.YY67.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4B[26U]="$(string_t<`H>, exp_t)@`H";
# 1310 "parse.y"
static struct _tuple32*Cyc_yyget_YY68(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY68 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY68 _T5;struct _tuple32*_T6;
static struct _fat_ptr s={_TmpG4B,_TmpG4B,_TmpG4B + 26U};struct _tuple32*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY68;_T3=_T2.tag;if(_T3!=75)goto _TL23F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY68;_T7=_T5.val;}{struct _tuple32*yy=_T7;_T6=yy;
# 1313
return _T6;}_TL23F:
 Cyc_yythrowfail(s);;}
# 1317
static union Cyc_YYSTYPE Cyc_YY68(struct _tuple32*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY68.tag=75U;_T1.YY68.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4C[16U]="exp_maker_fun_t";
# 1311 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY69(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY69 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY69 _T5;struct Cyc_Absyn_Exp*(*_T6)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
static struct _fat_ptr s={_TmpG4C,_TmpG4C,_TmpG4C + 16U};struct Cyc_Absyn_Exp*(*_T7)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY69;_T3=_T2.tag;if(_T3!=76)goto _TL241;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY69;_T7=_T5.val;}{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_T7;_T6=yy;
# 1314
return _T6;}_TL241:
 Cyc_yythrowfail(s);;}
# 1318
static union Cyc_YYSTYPE Cyc_YY69(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY69.tag=76U;_T1.YY69.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4D[19U]="enum ConstraintOps";
# 1313 "parse.y"
static enum Cyc_Parse_ConstraintOps Cyc_yyget_YY70(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY70 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY70 _T5;enum Cyc_Parse_ConstraintOps _T6;
static struct _fat_ptr s={_TmpG4D,_TmpG4D,_TmpG4D + 19U};enum Cyc_Parse_ConstraintOps _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY70;_T3=_T2.tag;if(_T3!=77)goto _TL243;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY70;_T7=_T5.val;}{enum Cyc_Parse_ConstraintOps yy=_T7;_T6=yy;
# 1316
return _T6;}_TL243:
 Cyc_yythrowfail(s);;}
# 1320
static union Cyc_YYSTYPE Cyc_YY70(enum Cyc_Parse_ConstraintOps yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY70.tag=77U;_T1.YY70.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4E[60U]="list_t<$(type_t, list_t<BansheeIf::constraint_t,`H>)@`H,`H>";
# 1314 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY71(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY71 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY71 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG4E,_TmpG4E,_TmpG4E + 60U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY71;_T3=_T2.tag;if(_T3!=78)goto _TL245;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY71;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1317
return _T6;}_TL245:
 Cyc_yythrowfail(s);;}
# 1321
static union Cyc_YYSTYPE Cyc_YY71(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY71.tag=78U;_T1.YY71.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4F[35U]="list_t<BansheeIf::constraint_t,`H>";
# 1315 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY72(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY72 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY72 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={_TmpG4F,_TmpG4F,_TmpG4F + 35U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY72;_T3=_T2.tag;if(_T3!=79)goto _TL247;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY72;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1318
return _T6;}_TL247:
 Cyc_yythrowfail(s);;}
# 1322
static union Cyc_YYSTYPE Cyc_YY72(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY72.tag=79U;_T1.YY72.val=yy1;_T0=_T1;}return _T0;}static char _TmpG50[24U]="BansheeIf::constraint_t";
# 1316 "parse.y"
static void*Cyc_yyget_YY73(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY73 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY73 _T5;void*_T6;
static struct _fat_ptr s={_TmpG50,_TmpG50,_TmpG50 + 24U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY73;_T3=_T2.tag;if(_T3!=80)goto _TL249;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY73;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1319
return _T6;}_TL249:
 Cyc_yythrowfail(s);;}
# 1323
static union Cyc_YYSTYPE Cyc_YY73(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY73.tag=80U;_T1.YY73.val=yy1;_T0=_T1;}return _T0;}static char _TmpG51[4U]="int";
# 1318 "parse.y"
static int Cyc_yyget_YY74(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY74 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY74 _T5;int _T6;
static struct _fat_ptr s={_TmpG51,_TmpG51,_TmpG51 + 4U};int _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY74;_T3=_T2.tag;if(_T3!=81)goto _TL24B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY74;_T7=_T5.val;}{int yy=_T7;_T6=yy;
# 1321
return _T6;}_TL24B:
 Cyc_yythrowfail(s);;}
# 1325
static union Cyc_YYSTYPE Cyc_YY74(int yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY74.tag=81U;_T1.YY74.val=yy1;_T0=_T1;}return _T0;}struct Cyc_Yyltype{int timestamp;unsigned first_line;unsigned first_column;unsigned last_line;unsigned last_column;};
# 1341
struct Cyc_Yyltype Cyc_yynewloc (void){struct Cyc_Yyltype _T0;{struct Cyc_Yyltype _T1;
_T1.timestamp=0;_T1.first_line=0U;_T1.first_column=0U;_T1.last_line=0U;_T1.last_column=0U;_T0=_T1;}return _T0;}
# 1344
struct Cyc_Yyltype Cyc_yylloc={0,0U,0U,0U,0U};
# 1355 "parse.y"
static short Cyc_yytranslate[401U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,172,2,2,157,170,167,2,154,155,150,164,149,168,159,169,2,2,2,2,2,2,2,2,2,2,158,146,152,151,153,163,162,173,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,174,2,2,2,2,160,2,161,166,156,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,147,165,148,171,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,129,130,131,132,133,134,135,136,137,138,139,140,141,142,143,144,145};static char _TmpG52[2U]="$";static char _TmpG53[6U]="error";static char _TmpG54[12U]="$undefined.";static char _TmpG55[5U]="AUTO";static char _TmpG56[9U]="REGISTER";static char _TmpG57[7U]="STATIC";static char _TmpG58[7U]="EXTERN";static char _TmpG59[8U]="TYPEDEF";static char _TmpG5A[5U]="VOID";static char _TmpG5B[5U]="CHAR";static char _TmpG5C[6U]="SHORT";static char _TmpG5D[4U]="INT";static char _TmpG5E[5U]="LONG";static char _TmpG5F[6U]="FLOAT";static char _TmpG60[7U]="DOUBLE";static char _TmpG61[7U]="SIGNED";static char _TmpG62[9U]="UNSIGNED";static char _TmpG63[6U]="CONST";static char _TmpG64[9U]="VOLATILE";static char _TmpG65[9U]="RESTRICT";static char _TmpG66[7U]="STRUCT";static char _TmpG67[6U]="UNION";static char _TmpG68[5U]="CASE";static char _TmpG69[8U]="DEFAULT";static char _TmpG6A[7U]="INLINE";static char _TmpG6B[7U]="SIZEOF";static char _TmpG6C[9U]="OFFSETOF";static char _TmpG6D[3U]="IF";static char _TmpG6E[5U]="ELSE";static char _TmpG6F[7U]="SWITCH";static char _TmpG70[6U]="WHILE";static char _TmpG71[3U]="DO";static char _TmpG72[4U]="FOR";static char _TmpG73[5U]="GOTO";static char _TmpG74[9U]="CONTINUE";static char _TmpG75[6U]="BREAK";static char _TmpG76[7U]="RETURN";static char _TmpG77[5U]="ENUM";static char _TmpG78[7U]="TYPEOF";static char _TmpG79[16U]="BUILTIN_VA_LIST";static char _TmpG7A[10U]="EXTENSION";static char _TmpG7B[8U]="COMPLEX";static char _TmpG7C[8U]="NULL_kw";static char _TmpG7D[4U]="LET";static char _TmpG7E[6U]="THROW";static char _TmpG7F[4U]="TRY";static char _TmpG80[6U]="CATCH";static char _TmpG81[7U]="EXPORT";static char _TmpG82[9U]="OVERRIDE";static char _TmpG83[5U]="HIDE";static char _TmpG84[4U]="NEW";static char _TmpG85[5U]="QNEW";static char _TmpG86[9U]="ABSTRACT";static char _TmpG87[9U]="FALLTHRU";static char _TmpG88[6U]="USING";static char _TmpG89[10U]="NAMESPACE";static char _TmpG8A[12U]="NOINFERENCE";static char _TmpG8B[9U]="DATATYPE";static char _TmpG8C[7U]="MALLOC";static char _TmpG8D[8U]="RMALLOC";static char _TmpG8E[9U]="RVMALLOC";static char _TmpG8F[15U]="RMALLOC_INLINE";static char _TmpG90[8U]="QMALLOC";static char _TmpG91[7U]="CALLOC";static char _TmpG92[8U]="QCALLOC";static char _TmpG93[8U]="RCALLOC";static char _TmpG94[5U]="SWAP";static char _TmpG95[7U]="ASSERT";static char _TmpG96[9U]="REGION_T";static char _TmpG97[6U]="TAG_T";static char _TmpG98[7U]="REGION";static char _TmpG99[5U]="RNEW";static char _TmpG9A[8U]="REGIONS";static char _TmpG9B[7U]="PORTON";static char _TmpG9C[8U]="PORTOFF";static char _TmpG9D[7U]="PRAGMA";static char _TmpG9E[10U]="TEMPESTON";static char _TmpG9F[11U]="TEMPESTOFF";static char _TmpGA0[13U]="AQ_ALIASABLE";static char _TmpGA1[10U]="AQ_REFCNT";static char _TmpGA2[14U]="AQ_RESTRICTED";static char _TmpGA3[10U]="AQ_UNIQUE";static char _TmpGA4[8U]="AQUAL_T";static char _TmpGA5[8U]="NUMELTS";static char _TmpGA6[6U]="TAGOF";static char _TmpGA7[8U]="VALUEOF";static char _TmpGA8[10U]="VALUEOF_T";static char _TmpGA9[9U]="TAGCHECK";static char _TmpGAA[13U]="NUMELTS_QUAL";static char _TmpGAB[10U]="THIN_QUAL";static char _TmpGAC[9U]="FAT_QUAL";static char _TmpGAD[13U]="NOTNULL_QUAL";static char _TmpGAE[14U]="NULLABLE_QUAL";static char _TmpGAF[14U]="REQUIRES_QUAL";static char _TmpGB0[13U]="ENSURES_QUAL";static char _TmpGB1[12U]="EFFECT_QUAL";static char _TmpGB2[12U]="THROWS_QUAL";static char _TmpGB3[12U]="REGION_QUAL";static char _TmpGB4[16U]="NOZEROTERM_QUAL";static char _TmpGB5[14U]="ZEROTERM_QUAL";static char _TmpGB6[12U]="TAGGED_QUAL";static char _TmpGB7[12U]="ASSERT_QUAL";static char _TmpGB8[18U]="ASSERT_FALSE_QUAL";static char _TmpGB9[11U]="ALIAS_QUAL";static char _TmpGBA[7U]="AQUALS";static char _TmpGBB[12U]="CHECKS_QUAL";static char _TmpGBC[16U]="EXTENSIBLE_QUAL";static char _TmpGBD[18U]="AUTORELEASED_QUAL";static char _TmpGBE[7U]="PTR_OP";static char _TmpGBF[7U]="INC_OP";static char _TmpGC0[7U]="DEC_OP";static char _TmpGC1[8U]="LEFT_OP";static char _TmpGC2[9U]="RIGHT_OP";static char _TmpGC3[6U]="LE_OP";static char _TmpGC4[6U]="GE_OP";static char _TmpGC5[6U]="EQ_OP";static char _TmpGC6[6U]="NE_OP";static char _TmpGC7[7U]="AND_OP";static char _TmpGC8[6U]="OR_OP";static char _TmpGC9[11U]="MUL_ASSIGN";static char _TmpGCA[11U]="DIV_ASSIGN";static char _TmpGCB[11U]="MOD_ASSIGN";static char _TmpGCC[11U]="ADD_ASSIGN";static char _TmpGCD[11U]="SUB_ASSIGN";static char _TmpGCE[12U]="LEFT_ASSIGN";static char _TmpGCF[13U]="RIGHT_ASSIGN";static char _TmpGD0[11U]="AND_ASSIGN";static char _TmpGD1[11U]="XOR_ASSIGN";static char _TmpGD2[10U]="OR_ASSIGN";static char _TmpGD3[9U]="ELLIPSIS";static char _TmpGD4[11U]="LEFT_RIGHT";static char _TmpGD5[12U]="COLON_COLON";static char _TmpGD6[11U]="IDENTIFIER";static char _TmpGD7[17U]="INTEGER_CONSTANT";static char _TmpGD8[7U]="STRING";static char _TmpGD9[8U]="WSTRING";static char _TmpGDA[19U]="CHARACTER_CONSTANT";static char _TmpGDB[20U]="WCHARACTER_CONSTANT";static char _TmpGDC[18U]="FLOATING_CONSTANT";static char _TmpGDD[9U]="TYPE_VAR";static char _TmpGDE[13U]="TYPEDEF_NAME";static char _TmpGDF[16U]="QUAL_IDENTIFIER";static char _TmpGE0[18U]="QUAL_TYPEDEF_NAME";static char _TmpGE1[18U]="AQUAL_SHORT_CONST";static char _TmpGE2[10U]="ATTRIBUTE";static char _TmpGE3[8U]="ASM_TOK";static char _TmpGE4[4U]="';'";static char _TmpGE5[4U]="'{'";static char _TmpGE6[4U]="'}'";static char _TmpGE7[4U]="','";static char _TmpGE8[4U]="'*'";static char _TmpGE9[4U]="'='";static char _TmpGEA[4U]="'<'";static char _TmpGEB[4U]="'>'";static char _TmpGEC[4U]="'('";static char _TmpGED[4U]="')'";static char _TmpGEE[4U]="'_'";static char _TmpGEF[4U]="'$'";static char _TmpGF0[4U]="':'";static char _TmpGF1[4U]="'.'";static char _TmpGF2[4U]="'['";static char _TmpGF3[4U]="']'";static char _TmpGF4[4U]="'@'";static char _TmpGF5[4U]="'?'";static char _TmpGF6[4U]="'+'";static char _TmpGF7[4U]="'|'";static char _TmpGF8[4U]="'^'";static char _TmpGF9[4U]="'&'";static char _TmpGFA[4U]="'-'";static char _TmpGFB[4U]="'/'";static char _TmpGFC[4U]="'%'";static char _TmpGFD[4U]="'~'";static char _TmpGFE[4U]="'!'";static char _TmpGFF[4U]="'A'";static char _TmpG100[4U]="'V'";static char _TmpG101[20U]="prog_or_constraints";static char _TmpG102[5U]="prog";static char _TmpG103[17U]="translation_unit";static char _TmpG104[18U]="tempest_on_action";static char _TmpG105[19U]="tempest_off_action";static char _TmpG106[16U]="extern_c_action";static char _TmpG107[13U]="end_extern_c";static char _TmpG108[14U]="hide_list_opt";static char _TmpG109[17U]="hide_list_values";static char _TmpG10A[16U]="export_list_opt";static char _TmpG10B[12U]="export_list";static char _TmpG10C[19U]="export_list_values";static char _TmpG10D[13U]="override_opt";static char _TmpG10E[21U]="external_declaration";static char _TmpG10F[14U]="optional_semi";static char _TmpG110[20U]="function_definition";static char _TmpG111[21U]="function_definition2";static char _TmpG112[13U]="using_action";static char _TmpG113[15U]="unusing_action";static char _TmpG114[17U]="namespace_action";static char _TmpG115[19U]="unnamespace_action";static char _TmpG116[19U]="noinference_action";static char _TmpG117[21U]="unnoinference_action";static char _TmpG118[12U]="declaration";static char _TmpG119[9U]="open_opt";static char _TmpG11A[17U]="declaration_list";static char _TmpG11B[23U]="declaration_specifiers";static char _TmpG11C[24U]="storage_class_specifier";static char _TmpG11D[15U]="attributes_opt";static char _TmpG11E[11U]="attributes";static char _TmpG11F[15U]="attribute_list";static char _TmpG120[10U]="attribute";static char _TmpG121[15U]="type_specifier";static char _TmpG122[25U]="type_specifier_notypedef";static char _TmpG123[5U]="kind";static char _TmpG124[15U]="type_qualifier";static char _TmpG125[15U]="enum_specifier";static char _TmpG126[11U]="enum_field";static char _TmpG127[22U]="enum_declaration_list";static char _TmpG128[26U]="struct_or_union_specifier";static char _TmpG129[26U]="maybe_tagged_struct_union";static char _TmpG12A[16U]="struct_or_union";static char _TmpG12B[16U]="type_params_opt";static char _TmpG12C[24U]="struct_declaration_list";static char _TmpG12D[25U]="struct_declaration_list0";static char _TmpG12E[25U]="init_declarator_list_rev";static char _TmpG12F[16U]="init_declarator";static char _TmpG130[19U]="struct_declaration";static char _TmpG131[25U]="specifier_qualifier_list";static char _TmpG132[35U]="notypedef_specifier_qualifier_list";static char _TmpG133[27U]="struct_declarator_list_rev";static char _TmpG134[18U]="struct_declarator";static char _TmpG135[20U]="requires_clause_opt";static char _TmpG136[19U]="datatype_specifier";static char _TmpG137[14U]="qual_datatype";static char _TmpG138[19U]="datatypefield_list";static char _TmpG139[20U]="datatypefield_scope";static char _TmpG13A[14U]="datatypefield";static char _TmpG13B[11U]="declarator";static char _TmpG13C[23U]="declarator_withtypedef";static char _TmpG13D[18U]="direct_declarator";static char _TmpG13E[30U]="direct_declarator_withtypedef";static char _TmpG13F[8U]="pointer";static char _TmpG140[12U]="one_pointer";static char _TmpG141[14U]="pointer_quals";static char _TmpG142[13U]="pointer_qual";static char _TmpG143[16U]="aqual_specifier";static char _TmpG144[23U]="pointer_null_and_bound";static char _TmpG145[14U]="pointer_bound";static char _TmpG146[18U]="zeroterm_qual_opt";static char _TmpG147[8U]="eff_set";static char _TmpG148[8U]="eff_opt";static char _TmpG149[11U]="tqual_list";static char _TmpG14A[20U]="parameter_type_list";static char _TmpG14B[14U]="opt_aqual_bnd";static char _TmpG14C[9U]="type_var";static char _TmpG14D[16U]="optional_effect";static char _TmpG14E[27U]="optional_effconstr_qualbnd";static char _TmpG14F[18U]="effconstr_qualbnd";static char _TmpG150[14U]="effconstr_elt";static char _TmpG151[13U]="qual_bnd_elt";static char _TmpG152[12U]="aqual_const";static char _TmpG153[15U]="qual_bnd_const";static char _TmpG154[14U]="qual_bnd_term";static char _TmpG155[16U]="optional_inject";static char _TmpG156[11U]="effect_set";static char _TmpG157[14U]="atomic_effect";static char _TmpG158[11U]="region_set";static char _TmpG159[15U]="parameter_list";static char _TmpG15A[22U]="parameter_declaration";static char _TmpG15B[16U]="identifier_list";static char _TmpG15C[17U]="identifier_list0";static char _TmpG15D[12U]="initializer";static char _TmpG15E[18U]="array_initializer";static char _TmpG15F[17U]="initializer_list";static char _TmpG160[12U]="designation";static char _TmpG161[16U]="designator_list";static char _TmpG162[11U]="designator";static char _TmpG163[10U]="type_name";static char _TmpG164[14U]="any_type_name";static char _TmpG165[15U]="type_name_list";static char _TmpG166[20U]="abstract_declarator";static char _TmpG167[27U]="direct_abstract_declarator";static char _TmpG168[16U]="chk_req_ens_thr";static char _TmpG169[20U]="chk_req_ens_thr_opt";static char _TmpG16A[10U]="statement";static char _TmpG16B[18U]="labeled_statement";static char _TmpG16C[21U]="expression_statement";static char _TmpG16D[18U]="start_fndef_block";static char _TmpG16E[16U]="end_fndef_block";static char _TmpG16F[25U]="fndef_compound_statement";static char _TmpG170[19U]="compound_statement";static char _TmpG171[16U]="block_item_list";static char _TmpG172[20U]="selection_statement";static char _TmpG173[15U]="switch_clauses";static char _TmpG174[20U]="iteration_statement";static char _TmpG175[12U]="for_exp_opt";static char _TmpG176[15U]="jump_statement";static char _TmpG177[12U]="exp_pattern";static char _TmpG178[20U]="conditional_pattern";static char _TmpG179[19U]="logical_or_pattern";static char _TmpG17A[20U]="logical_and_pattern";static char _TmpG17B[21U]="inclusive_or_pattern";static char _TmpG17C[21U]="exclusive_or_pattern";static char _TmpG17D[12U]="and_pattern";static char _TmpG17E[17U]="equality_pattern";static char _TmpG17F[19U]="relational_pattern";static char _TmpG180[14U]="shift_pattern";static char _TmpG181[17U]="additive_pattern";static char _TmpG182[23U]="multiplicative_pattern";static char _TmpG183[13U]="cast_pattern";static char _TmpG184[14U]="unary_pattern";static char _TmpG185[16U]="postfix_pattern";static char _TmpG186[16U]="primary_pattern";static char _TmpG187[8U]="pattern";static char _TmpG188[19U]="tuple_pattern_list";static char _TmpG189[20U]="tuple_pattern_list0";static char _TmpG18A[14U]="field_pattern";static char _TmpG18B[19U]="field_pattern_list";static char _TmpG18C[20U]="field_pattern_list0";static char _TmpG18D[11U]="expression";static char _TmpG18E[22U]="assignment_expression";static char _TmpG18F[20U]="assignment_operator";static char _TmpG190[23U]="conditional_expression";static char _TmpG191[20U]="constant_expression";static char _TmpG192[22U]="logical_or_expression";static char _TmpG193[23U]="logical_and_expression";static char _TmpG194[24U]="inclusive_or_expression";static char _TmpG195[24U]="exclusive_or_expression";static char _TmpG196[15U]="and_expression";static char _TmpG197[20U]="equality_expression";static char _TmpG198[22U]="relational_expression";static char _TmpG199[17U]="shift_expression";static char _TmpG19A[20U]="additive_expression";static char _TmpG19B[26U]="multiplicative_expression";static char _TmpG19C[12U]="equality_op";static char _TmpG19D[14U]="relational_op";static char _TmpG19E[12U]="additive_op";static char _TmpG19F[18U]="multiplicative_op";static char _TmpG1A0[16U]="cast_expression";static char _TmpG1A1[17U]="unary_expression";static char _TmpG1A2[15U]="unary_operator";static char _TmpG1A3[9U]="asm_expr";static char _TmpG1A4[13U]="volatile_opt";static char _TmpG1A5[12U]="asm_out_opt";static char _TmpG1A6[12U]="asm_outlist";static char _TmpG1A7[11U]="asm_in_opt";static char _TmpG1A8[11U]="asm_inlist";static char _TmpG1A9[11U]="asm_io_elt";static char _TmpG1AA[16U]="asm_clobber_opt";static char _TmpG1AB[17U]="asm_clobber_list";static char _TmpG1AC[19U]="postfix_expression";static char _TmpG1AD[17U]="field_expression";static char _TmpG1AE[19U]="primary_expression";static char _TmpG1AF[25U]="argument_expression_list";static char _TmpG1B0[26U]="argument_expression_list0";static char _TmpG1B1[9U]="constant";static char _TmpG1B2[20U]="qual_opt_identifier";static char _TmpG1B3[17U]="qual_opt_typedef";static char _TmpG1B4[18U]="struct_union_name";static char _TmpG1B5[11U]="field_name";static char _TmpG1B6[12U]="right_angle";static char _TmpG1B7[16U]="all_constraints";static char _TmpG1B8[20U]="constraint_list_opt";static char _TmpG1B9[16U]="constraint_list";static char _TmpG1BA[15U]="tvar_or_string";static char _TmpG1BB[11U]="constraint";static char _TmpG1BC[5U]="c_op";
# 1756 "parse.y"
static struct _fat_ptr Cyc_yytname[363U]={{_TmpG52,_TmpG52,_TmpG52 + 2U},{_TmpG53,_TmpG53,_TmpG53 + 6U},{_TmpG54,_TmpG54,_TmpG54 + 12U},{_TmpG55,_TmpG55,_TmpG55 + 5U},{_TmpG56,_TmpG56,_TmpG56 + 9U},{_TmpG57,_TmpG57,_TmpG57 + 7U},{_TmpG58,_TmpG58,_TmpG58 + 7U},{_TmpG59,_TmpG59,_TmpG59 + 8U},{_TmpG5A,_TmpG5A,_TmpG5A + 5U},{_TmpG5B,_TmpG5B,_TmpG5B + 5U},{_TmpG5C,_TmpG5C,_TmpG5C + 6U},{_TmpG5D,_TmpG5D,_TmpG5D + 4U},{_TmpG5E,_TmpG5E,_TmpG5E + 5U},{_TmpG5F,_TmpG5F,_TmpG5F + 6U},{_TmpG60,_TmpG60,_TmpG60 + 7U},{_TmpG61,_TmpG61,_TmpG61 + 7U},{_TmpG62,_TmpG62,_TmpG62 + 9U},{_TmpG63,_TmpG63,_TmpG63 + 6U},{_TmpG64,_TmpG64,_TmpG64 + 9U},{_TmpG65,_TmpG65,_TmpG65 + 9U},{_TmpG66,_TmpG66,_TmpG66 + 7U},{_TmpG67,_TmpG67,_TmpG67 + 6U},{_TmpG68,_TmpG68,_TmpG68 + 5U},{_TmpG69,_TmpG69,_TmpG69 + 8U},{_TmpG6A,_TmpG6A,_TmpG6A + 7U},{_TmpG6B,_TmpG6B,_TmpG6B + 7U},{_TmpG6C,_TmpG6C,_TmpG6C + 9U},{_TmpG6D,_TmpG6D,_TmpG6D + 3U},{_TmpG6E,_TmpG6E,_TmpG6E + 5U},{_TmpG6F,_TmpG6F,_TmpG6F + 7U},{_TmpG70,_TmpG70,_TmpG70 + 6U},{_TmpG71,_TmpG71,_TmpG71 + 3U},{_TmpG72,_TmpG72,_TmpG72 + 4U},{_TmpG73,_TmpG73,_TmpG73 + 5U},{_TmpG74,_TmpG74,_TmpG74 + 9U},{_TmpG75,_TmpG75,_TmpG75 + 6U},{_TmpG76,_TmpG76,_TmpG76 + 7U},{_TmpG77,_TmpG77,_TmpG77 + 5U},{_TmpG78,_TmpG78,_TmpG78 + 7U},{_TmpG79,_TmpG79,_TmpG79 + 16U},{_TmpG7A,_TmpG7A,_TmpG7A + 10U},{_TmpG7B,_TmpG7B,_TmpG7B + 8U},{_TmpG7C,_TmpG7C,_TmpG7C + 8U},{_TmpG7D,_TmpG7D,_TmpG7D + 4U},{_TmpG7E,_TmpG7E,_TmpG7E + 6U},{_TmpG7F,_TmpG7F,_TmpG7F + 4U},{_TmpG80,_TmpG80,_TmpG80 + 6U},{_TmpG81,_TmpG81,_TmpG81 + 7U},{_TmpG82,_TmpG82,_TmpG82 + 9U},{_TmpG83,_TmpG83,_TmpG83 + 5U},{_TmpG84,_TmpG84,_TmpG84 + 4U},{_TmpG85,_TmpG85,_TmpG85 + 5U},{_TmpG86,_TmpG86,_TmpG86 + 9U},{_TmpG87,_TmpG87,_TmpG87 + 9U},{_TmpG88,_TmpG88,_TmpG88 + 6U},{_TmpG89,_TmpG89,_TmpG89 + 10U},{_TmpG8A,_TmpG8A,_TmpG8A + 12U},{_TmpG8B,_TmpG8B,_TmpG8B + 9U},{_TmpG8C,_TmpG8C,_TmpG8C + 7U},{_TmpG8D,_TmpG8D,_TmpG8D + 8U},{_TmpG8E,_TmpG8E,_TmpG8E + 9U},{_TmpG8F,_TmpG8F,_TmpG8F + 15U},{_TmpG90,_TmpG90,_TmpG90 + 8U},{_TmpG91,_TmpG91,_TmpG91 + 7U},{_TmpG92,_TmpG92,_TmpG92 + 8U},{_TmpG93,_TmpG93,_TmpG93 + 8U},{_TmpG94,_TmpG94,_TmpG94 + 5U},{_TmpG95,_TmpG95,_TmpG95 + 7U},{_TmpG96,_TmpG96,_TmpG96 + 9U},{_TmpG97,_TmpG97,_TmpG97 + 6U},{_TmpG98,_TmpG98,_TmpG98 + 7U},{_TmpG99,_TmpG99,_TmpG99 + 5U},{_TmpG9A,_TmpG9A,_TmpG9A + 8U},{_TmpG9B,_TmpG9B,_TmpG9B + 7U},{_TmpG9C,_TmpG9C,_TmpG9C + 8U},{_TmpG9D,_TmpG9D,_TmpG9D + 7U},{_TmpG9E,_TmpG9E,_TmpG9E + 10U},{_TmpG9F,_TmpG9F,_TmpG9F + 11U},{_TmpGA0,_TmpGA0,_TmpGA0 + 13U},{_TmpGA1,_TmpGA1,_TmpGA1 + 10U},{_TmpGA2,_TmpGA2,_TmpGA2 + 14U},{_TmpGA3,_TmpGA3,_TmpGA3 + 10U},{_TmpGA4,_TmpGA4,_TmpGA4 + 8U},{_TmpGA5,_TmpGA5,_TmpGA5 + 8U},{_TmpGA6,_TmpGA6,_TmpGA6 + 6U},{_TmpGA7,_TmpGA7,_TmpGA7 + 8U},{_TmpGA8,_TmpGA8,_TmpGA8 + 10U},{_TmpGA9,_TmpGA9,_TmpGA9 + 9U},{_TmpGAA,_TmpGAA,_TmpGAA + 13U},{_TmpGAB,_TmpGAB,_TmpGAB + 10U},{_TmpGAC,_TmpGAC,_TmpGAC + 9U},{_TmpGAD,_TmpGAD,_TmpGAD + 13U},{_TmpGAE,_TmpGAE,_TmpGAE + 14U},{_TmpGAF,_TmpGAF,_TmpGAF + 14U},{_TmpGB0,_TmpGB0,_TmpGB0 + 13U},{_TmpGB1,_TmpGB1,_TmpGB1 + 12U},{_TmpGB2,_TmpGB2,_TmpGB2 + 12U},{_TmpGB3,_TmpGB3,_TmpGB3 + 12U},{_TmpGB4,_TmpGB4,_TmpGB4 + 16U},{_TmpGB5,_TmpGB5,_TmpGB5 + 14U},{_TmpGB6,_TmpGB6,_TmpGB6 + 12U},{_TmpGB7,_TmpGB7,_TmpGB7 + 12U},{_TmpGB8,_TmpGB8,_TmpGB8 + 18U},{_TmpGB9,_TmpGB9,_TmpGB9 + 11U},{_TmpGBA,_TmpGBA,_TmpGBA + 7U},{_TmpGBB,_TmpGBB,_TmpGBB + 12U},{_TmpGBC,_TmpGBC,_TmpGBC + 16U},{_TmpGBD,_TmpGBD,_TmpGBD + 18U},{_TmpGBE,_TmpGBE,_TmpGBE + 7U},{_TmpGBF,_TmpGBF,_TmpGBF + 7U},{_TmpGC0,_TmpGC0,_TmpGC0 + 7U},{_TmpGC1,_TmpGC1,_TmpGC1 + 8U},{_TmpGC2,_TmpGC2,_TmpGC2 + 9U},{_TmpGC3,_TmpGC3,_TmpGC3 + 6U},{_TmpGC4,_TmpGC4,_TmpGC4 + 6U},{_TmpGC5,_TmpGC5,_TmpGC5 + 6U},{_TmpGC6,_TmpGC6,_TmpGC6 + 6U},{_TmpGC7,_TmpGC7,_TmpGC7 + 7U},{_TmpGC8,_TmpGC8,_TmpGC8 + 6U},{_TmpGC9,_TmpGC9,_TmpGC9 + 11U},{_TmpGCA,_TmpGCA,_TmpGCA + 11U},{_TmpGCB,_TmpGCB,_TmpGCB + 11U},{_TmpGCC,_TmpGCC,_TmpGCC + 11U},{_TmpGCD,_TmpGCD,_TmpGCD + 11U},{_TmpGCE,_TmpGCE,_TmpGCE + 12U},{_TmpGCF,_TmpGCF,_TmpGCF + 13U},{_TmpGD0,_TmpGD0,_TmpGD0 + 11U},{_TmpGD1,_TmpGD1,_TmpGD1 + 11U},{_TmpGD2,_TmpGD2,_TmpGD2 + 10U},{_TmpGD3,_TmpGD3,_TmpGD3 + 9U},{_TmpGD4,_TmpGD4,_TmpGD4 + 11U},{_TmpGD5,_TmpGD5,_TmpGD5 + 12U},{_TmpGD6,_TmpGD6,_TmpGD6 + 11U},{_TmpGD7,_TmpGD7,_TmpGD7 + 17U},{_TmpGD8,_TmpGD8,_TmpGD8 + 7U},{_TmpGD9,_TmpGD9,_TmpGD9 + 8U},{_TmpGDA,_TmpGDA,_TmpGDA + 19U},{_TmpGDB,_TmpGDB,_TmpGDB + 20U},{_TmpGDC,_TmpGDC,_TmpGDC + 18U},{_TmpGDD,_TmpGDD,_TmpGDD + 9U},{_TmpGDE,_TmpGDE,_TmpGDE + 13U},{_TmpGDF,_TmpGDF,_TmpGDF + 16U},{_TmpGE0,_TmpGE0,_TmpGE0 + 18U},{_TmpGE1,_TmpGE1,_TmpGE1 + 18U},{_TmpGE2,_TmpGE2,_TmpGE2 + 10U},{_TmpGE3,_TmpGE3,_TmpGE3 + 8U},{_TmpGE4,_TmpGE4,_TmpGE4 + 4U},{_TmpGE5,_TmpGE5,_TmpGE5 + 4U},{_TmpGE6,_TmpGE6,_TmpGE6 + 4U},{_TmpGE7,_TmpGE7,_TmpGE7 + 4U},{_TmpGE8,_TmpGE8,_TmpGE8 + 4U},{_TmpGE9,_TmpGE9,_TmpGE9 + 4U},{_TmpGEA,_TmpGEA,_TmpGEA + 4U},{_TmpGEB,_TmpGEB,_TmpGEB + 4U},{_TmpGEC,_TmpGEC,_TmpGEC + 4U},{_TmpGED,_TmpGED,_TmpGED + 4U},{_TmpGEE,_TmpGEE,_TmpGEE + 4U},{_TmpGEF,_TmpGEF,_TmpGEF + 4U},{_TmpGF0,_TmpGF0,_TmpGF0 + 4U},{_TmpGF1,_TmpGF1,_TmpGF1 + 4U},{_TmpGF2,_TmpGF2,_TmpGF2 + 4U},{_TmpGF3,_TmpGF3,_TmpGF3 + 4U},{_TmpGF4,_TmpGF4,_TmpGF4 + 4U},{_TmpGF5,_TmpGF5,_TmpGF5 + 4U},{_TmpGF6,_TmpGF6,_TmpGF6 + 4U},{_TmpGF7,_TmpGF7,_TmpGF7 + 4U},{_TmpGF8,_TmpGF8,_TmpGF8 + 4U},{_TmpGF9,_TmpGF9,_TmpGF9 + 4U},{_TmpGFA,_TmpGFA,_TmpGFA + 4U},{_TmpGFB,_TmpGFB,_TmpGFB + 4U},{_TmpGFC,_TmpGFC,_TmpGFC + 4U},{_TmpGFD,_TmpGFD,_TmpGFD + 4U},{_TmpGFE,_TmpGFE,_TmpGFE + 4U},{_TmpGFF,_TmpGFF,_TmpGFF + 4U},{_TmpG100,_TmpG100,_TmpG100 + 4U},{_TmpG101,_TmpG101,_TmpG101 + 20U},{_TmpG102,_TmpG102,_TmpG102 + 5U},{_TmpG103,_TmpG103,_TmpG103 + 17U},{_TmpG104,_TmpG104,_TmpG104 + 18U},{_TmpG105,_TmpG105,_TmpG105 + 19U},{_TmpG106,_TmpG106,_TmpG106 + 16U},{_TmpG107,_TmpG107,_TmpG107 + 13U},{_TmpG108,_TmpG108,_TmpG108 + 14U},{_TmpG109,_TmpG109,_TmpG109 + 17U},{_TmpG10A,_TmpG10A,_TmpG10A + 16U},{_TmpG10B,_TmpG10B,_TmpG10B + 12U},{_TmpG10C,_TmpG10C,_TmpG10C + 19U},{_TmpG10D,_TmpG10D,_TmpG10D + 13U},{_TmpG10E,_TmpG10E,_TmpG10E + 21U},{_TmpG10F,_TmpG10F,_TmpG10F + 14U},{_TmpG110,_TmpG110,_TmpG110 + 20U},{_TmpG111,_TmpG111,_TmpG111 + 21U},{_TmpG112,_TmpG112,_TmpG112 + 13U},{_TmpG113,_TmpG113,_TmpG113 + 15U},{_TmpG114,_TmpG114,_TmpG114 + 17U},{_TmpG115,_TmpG115,_TmpG115 + 19U},{_TmpG116,_TmpG116,_TmpG116 + 19U},{_TmpG117,_TmpG117,_TmpG117 + 21U},{_TmpG118,_TmpG118,_TmpG118 + 12U},{_TmpG119,_TmpG119,_TmpG119 + 9U},{_TmpG11A,_TmpG11A,_TmpG11A + 17U},{_TmpG11B,_TmpG11B,_TmpG11B + 23U},{_TmpG11C,_TmpG11C,_TmpG11C + 24U},{_TmpG11D,_TmpG11D,_TmpG11D + 15U},{_TmpG11E,_TmpG11E,_TmpG11E + 11U},{_TmpG11F,_TmpG11F,_TmpG11F + 15U},{_TmpG120,_TmpG120,_TmpG120 + 10U},{_TmpG121,_TmpG121,_TmpG121 + 15U},{_TmpG122,_TmpG122,_TmpG122 + 25U},{_TmpG123,_TmpG123,_TmpG123 + 5U},{_TmpG124,_TmpG124,_TmpG124 + 15U},{_TmpG125,_TmpG125,_TmpG125 + 15U},{_TmpG126,_TmpG126,_TmpG126 + 11U},{_TmpG127,_TmpG127,_TmpG127 + 22U},{_TmpG128,_TmpG128,_TmpG128 + 26U},{_TmpG129,_TmpG129,_TmpG129 + 26U},{_TmpG12A,_TmpG12A,_TmpG12A + 16U},{_TmpG12B,_TmpG12B,_TmpG12B + 16U},{_TmpG12C,_TmpG12C,_TmpG12C + 24U},{_TmpG12D,_TmpG12D,_TmpG12D + 25U},{_TmpG12E,_TmpG12E,_TmpG12E + 25U},{_TmpG12F,_TmpG12F,_TmpG12F + 16U},{_TmpG130,_TmpG130,_TmpG130 + 19U},{_TmpG131,_TmpG131,_TmpG131 + 25U},{_TmpG132,_TmpG132,_TmpG132 + 35U},{_TmpG133,_TmpG133,_TmpG133 + 27U},{_TmpG134,_TmpG134,_TmpG134 + 18U},{_TmpG135,_TmpG135,_TmpG135 + 20U},{_TmpG136,_TmpG136,_TmpG136 + 19U},{_TmpG137,_TmpG137,_TmpG137 + 14U},{_TmpG138,_TmpG138,_TmpG138 + 19U},{_TmpG139,_TmpG139,_TmpG139 + 20U},{_TmpG13A,_TmpG13A,_TmpG13A + 14U},{_TmpG13B,_TmpG13B,_TmpG13B + 11U},{_TmpG13C,_TmpG13C,_TmpG13C + 23U},{_TmpG13D,_TmpG13D,_TmpG13D + 18U},{_TmpG13E,_TmpG13E,_TmpG13E + 30U},{_TmpG13F,_TmpG13F,_TmpG13F + 8U},{_TmpG140,_TmpG140,_TmpG140 + 12U},{_TmpG141,_TmpG141,_TmpG141 + 14U},{_TmpG142,_TmpG142,_TmpG142 + 13U},{_TmpG143,_TmpG143,_TmpG143 + 16U},{_TmpG144,_TmpG144,_TmpG144 + 23U},{_TmpG145,_TmpG145,_TmpG145 + 14U},{_TmpG146,_TmpG146,_TmpG146 + 18U},{_TmpG147,_TmpG147,_TmpG147 + 8U},{_TmpG148,_TmpG148,_TmpG148 + 8U},{_TmpG149,_TmpG149,_TmpG149 + 11U},{_TmpG14A,_TmpG14A,_TmpG14A + 20U},{_TmpG14B,_TmpG14B,_TmpG14B + 14U},{_TmpG14C,_TmpG14C,_TmpG14C + 9U},{_TmpG14D,_TmpG14D,_TmpG14D + 16U},{_TmpG14E,_TmpG14E,_TmpG14E + 27U},{_TmpG14F,_TmpG14F,_TmpG14F + 18U},{_TmpG150,_TmpG150,_TmpG150 + 14U},{_TmpG151,_TmpG151,_TmpG151 + 13U},{_TmpG152,_TmpG152,_TmpG152 + 12U},{_TmpG153,_TmpG153,_TmpG153 + 15U},{_TmpG154,_TmpG154,_TmpG154 + 14U},{_TmpG155,_TmpG155,_TmpG155 + 16U},{_TmpG156,_TmpG156,_TmpG156 + 11U},{_TmpG157,_TmpG157,_TmpG157 + 14U},{_TmpG158,_TmpG158,_TmpG158 + 11U},{_TmpG159,_TmpG159,_TmpG159 + 15U},{_TmpG15A,_TmpG15A,_TmpG15A + 22U},{_TmpG15B,_TmpG15B,_TmpG15B + 16U},{_TmpG15C,_TmpG15C,_TmpG15C + 17U},{_TmpG15D,_TmpG15D,_TmpG15D + 12U},{_TmpG15E,_TmpG15E,_TmpG15E + 18U},{_TmpG15F,_TmpG15F,_TmpG15F + 17U},{_TmpG160,_TmpG160,_TmpG160 + 12U},{_TmpG161,_TmpG161,_TmpG161 + 16U},{_TmpG162,_TmpG162,_TmpG162 + 11U},{_TmpG163,_TmpG163,_TmpG163 + 10U},{_TmpG164,_TmpG164,_TmpG164 + 14U},{_TmpG165,_TmpG165,_TmpG165 + 15U},{_TmpG166,_TmpG166,_TmpG166 + 20U},{_TmpG167,_TmpG167,_TmpG167 + 27U},{_TmpG168,_TmpG168,_TmpG168 + 16U},{_TmpG169,_TmpG169,_TmpG169 + 20U},{_TmpG16A,_TmpG16A,_TmpG16A + 10U},{_TmpG16B,_TmpG16B,_TmpG16B + 18U},{_TmpG16C,_TmpG16C,_TmpG16C + 21U},{_TmpG16D,_TmpG16D,_TmpG16D + 18U},{_TmpG16E,_TmpG16E,_TmpG16E + 16U},{_TmpG16F,_TmpG16F,_TmpG16F + 25U},{_TmpG170,_TmpG170,_TmpG170 + 19U},{_TmpG171,_TmpG171,_TmpG171 + 16U},{_TmpG172,_TmpG172,_TmpG172 + 20U},{_TmpG173,_TmpG173,_TmpG173 + 15U},{_TmpG174,_TmpG174,_TmpG174 + 20U},{_TmpG175,_TmpG175,_TmpG175 + 12U},{_TmpG176,_TmpG176,_TmpG176 + 15U},{_TmpG177,_TmpG177,_TmpG177 + 12U},{_TmpG178,_TmpG178,_TmpG178 + 20U},{_TmpG179,_TmpG179,_TmpG179 + 19U},{_TmpG17A,_TmpG17A,_TmpG17A + 20U},{_TmpG17B,_TmpG17B,_TmpG17B + 21U},{_TmpG17C,_TmpG17C,_TmpG17C + 21U},{_TmpG17D,_TmpG17D,_TmpG17D + 12U},{_TmpG17E,_TmpG17E,_TmpG17E + 17U},{_TmpG17F,_TmpG17F,_TmpG17F + 19U},{_TmpG180,_TmpG180,_TmpG180 + 14U},{_TmpG181,_TmpG181,_TmpG181 + 17U},{_TmpG182,_TmpG182,_TmpG182 + 23U},{_TmpG183,_TmpG183,_TmpG183 + 13U},{_TmpG184,_TmpG184,_TmpG184 + 14U},{_TmpG185,_TmpG185,_TmpG185 + 16U},{_TmpG186,_TmpG186,_TmpG186 + 16U},{_TmpG187,_TmpG187,_TmpG187 + 8U},{_TmpG188,_TmpG188,_TmpG188 + 19U},{_TmpG189,_TmpG189,_TmpG189 + 20U},{_TmpG18A,_TmpG18A,_TmpG18A + 14U},{_TmpG18B,_TmpG18B,_TmpG18B + 19U},{_TmpG18C,_TmpG18C,_TmpG18C + 20U},{_TmpG18D,_TmpG18D,_TmpG18D + 11U},{_TmpG18E,_TmpG18E,_TmpG18E + 22U},{_TmpG18F,_TmpG18F,_TmpG18F + 20U},{_TmpG190,_TmpG190,_TmpG190 + 23U},{_TmpG191,_TmpG191,_TmpG191 + 20U},{_TmpG192,_TmpG192,_TmpG192 + 22U},{_TmpG193,_TmpG193,_TmpG193 + 23U},{_TmpG194,_TmpG194,_TmpG194 + 24U},{_TmpG195,_TmpG195,_TmpG195 + 24U},{_TmpG196,_TmpG196,_TmpG196 + 15U},{_TmpG197,_TmpG197,_TmpG197 + 20U},{_TmpG198,_TmpG198,_TmpG198 + 22U},{_TmpG199,_TmpG199,_TmpG199 + 17U},{_TmpG19A,_TmpG19A,_TmpG19A + 20U},{_TmpG19B,_TmpG19B,_TmpG19B + 26U},{_TmpG19C,_TmpG19C,_TmpG19C + 12U},{_TmpG19D,_TmpG19D,_TmpG19D + 14U},{_TmpG19E,_TmpG19E,_TmpG19E + 12U},{_TmpG19F,_TmpG19F,_TmpG19F + 18U},{_TmpG1A0,_TmpG1A0,_TmpG1A0 + 16U},{_TmpG1A1,_TmpG1A1,_TmpG1A1 + 17U},{_TmpG1A2,_TmpG1A2,_TmpG1A2 + 15U},{_TmpG1A3,_TmpG1A3,_TmpG1A3 + 9U},{_TmpG1A4,_TmpG1A4,_TmpG1A4 + 13U},{_TmpG1A5,_TmpG1A5,_TmpG1A5 + 12U},{_TmpG1A6,_TmpG1A6,_TmpG1A6 + 12U},{_TmpG1A7,_TmpG1A7,_TmpG1A7 + 11U},{_TmpG1A8,_TmpG1A8,_TmpG1A8 + 11U},{_TmpG1A9,_TmpG1A9,_TmpG1A9 + 11U},{_TmpG1AA,_TmpG1AA,_TmpG1AA + 16U},{_TmpG1AB,_TmpG1AB,_TmpG1AB + 17U},{_TmpG1AC,_TmpG1AC,_TmpG1AC + 19U},{_TmpG1AD,_TmpG1AD,_TmpG1AD + 17U},{_TmpG1AE,_TmpG1AE,_TmpG1AE + 19U},{_TmpG1AF,_TmpG1AF,_TmpG1AF + 25U},{_TmpG1B0,_TmpG1B0,_TmpG1B0 + 26U},{_TmpG1B1,_TmpG1B1,_TmpG1B1 + 9U},{_TmpG1B2,_TmpG1B2,_TmpG1B2 + 20U},{_TmpG1B3,_TmpG1B3,_TmpG1B3 + 17U},{_TmpG1B4,_TmpG1B4,_TmpG1B4 + 18U},{_TmpG1B5,_TmpG1B5,_TmpG1B5 + 11U},{_TmpG1B6,_TmpG1B6,_TmpG1B6 + 12U},{_TmpG1B7,_TmpG1B7,_TmpG1B7 + 16U},{_TmpG1B8,_TmpG1B8,_TmpG1B8 + 20U},{_TmpG1B9,_TmpG1B9,_TmpG1B9 + 16U},{_TmpG1BA,_TmpG1BA,_TmpG1BA + 15U},{_TmpG1BB,_TmpG1BB,_TmpG1BB + 11U},{_TmpG1BC,_TmpG1BC,_TmpG1BC + 5U}};
# 1823
static short Cyc_yyr1[624U]={0,175,175,176,177,177,177,177,177,177,177,177,177,177,177,177,178,179,180,181,182,182,183,183,183,184,184,185,185,185,186,186,187,187,188,188,188,189,189,190,190,190,190,191,191,192,193,194,195,196,197,198,198,198,198,198,198,199,199,200,200,201,201,201,201,201,201,201,201,201,201,201,202,202,202,202,202,202,202,203,203,204,205,205,206,206,206,206,207,207,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,208,209,210,210,210,211,211,211,212,212,213,213,213,214,214,214,215,215,216,216,217,217,218,218,219,219,220,220,221,221,221,221,222,223,223,223,223,223,223,224,224,224,224,224,224,225,225,226,226,226,226,227,227,228,228,228,229,229,230,230,230,230,231,231,231,232,232,233,233,234,234,235,235,235,235,235,235,235,235,235,236,236,236,236,236,236,236,236,236,236,237,237,238,239,239,240,240,240,240,240,240,240,240,240,240,240,240,241,241,241,- 1,- 1,242,242,242,243,243,243,244,244,244,245,245,246,246,246,247,247,248,248,248,248,248,249,249,250,250,251,251,252,252,253,253,253,253,254,254,254,255,256,256,256,256,256,257,257,258,258,259,259,260,260,261,261,261,261,262,262,263,263,264,264,264,265,266,266,267,267,268,268,268,268,268,269,269,269,269,270,270,271,271,272,272,273,273,274,274,274,274,274,274,275,275,276,276,276,277,277,277,277,277,277,277,277,277,278,278,278,278,278,278,278,278,279,279,280,280,280,280,280,280,281,282,282,283,284,285,285,286,286,287,287,287,287,287,287,287,287,288,288,288,288,288,288,289,289,289,289,289,289,290,290,290,290,291,291,292,292,292,292,292,292,292,292,293,294,294,295,295,296,296,297,297,298,298,299,299,300,300,301,301,302,302,302,303,303,304,304,305,305,306,306,306,306,306,307,308,309,309,309,309,309,309,309,309,309,309,309,309,309,309,309,309,309,310,310,310,311,311,312,312,313,313,313,314,314,315,315,316,316,316,317,317,317,317,317,317,317,317,317,317,317,318,318,318,318,318,318,318,318,318,319,320,320,321,321,322,322,323,323,324,324,325,325,326,326,327,327,327,328,328,329,329,330,330,331,331,331,331,332,332,333,333,333,334,334,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,336,336,336,336,337,338,338,339,339,339,340,340,341,341,341,342,342,343,344,344,344,345,345,346,346,346,346,346,346,346,346,346,346,346,347,347,347,347,348,348,348,348,348,348,348,348,348,348,348,349,350,350,351,351,351,351,351,352,352,353,353,354,354,355,355,356,356,357,357,358,358,359,359,360,360,361,361,361,361,361,361,361,362,362,362,362,362};
# 1889
static short Cyc_yyr2[624U]={0,1,1,1,2,3,5,3,5,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,1,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,5,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,4,1,1,1,1,1,1,1,4,1,1,1,4,0,1,2,2,1,0,3,3,0,1,1,1,3,0,1,1,0,2,2,3,5,5,7,0,1,2,4,0,2,0,2,1,1,3,3,3,3,4,3,1,1,1,1,1,1,4,1,4,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,1,3,1,1,2,3,3,4,4,5,4,5,4,2,4,4,4,4,5,5,5,5,0,1,1,1,1,1,1,1,3,1,2,1,1,2,3,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,8,6,6,6,9,13,11,4,4,6,6,4,2,2,4,4,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,8,0,1,1,3,1,1,5,9,7,7,7,7,9,1,1,1,1,1};
# 1955
static short Cyc_yydefact[1262U]={0,36,72,73,74,75,77,90,91,92,93,94,95,96,97,98,116,117,118,132,133,68,0,0,103,0,99,0,78,0,0,49,170,110,113,0,0,0,16,17,0,0,0,0,594,247,596,595,597,0,229,0,106,0,229,228,1,3,0,0,0,0,34,0,0,0,35,0,61,70,64,88,66,100,101,0,131,104,0,0,181,0,204,207,105,185,134,2,76,75,247,69,0,120,0,63,592,0,594,589,590,591,593,0,134,0,0,421,0,0,0,287,0,425,423,45,47,0,0,57,0,0,0,0,0,130,171,0,0,248,249,0,0,226,0,0,0,0,227,0,0,0,4,0,0,0,0,0,51,0,140,142,62,71,65,67,598,599,134,136,134,348,59,0,0,0,38,0,251,0,193,182,205,0,212,213,217,218,0,0,216,215,0,214,220,237,207,0,89,76,124,0,122,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,578,579,542,0,0,0,540,0,539,537,538,0,450,452,466,476,478,480,482,484,486,488,490,493,495,508,0,510,560,577,575,594,433,0,0,0,0,434,0,0,424,54,0,0,134,0,0,263,265,266,264,0,267,0,151,147,149,307,268,314,309,0,0,0,0,0,11,12,0,0,222,221,0,600,601,247,115,0,0,0,0,0,186,107,285,0,282,13,14,0,5,0,7,0,0,52,0,542,0,0,38,129,0,137,138,163,0,168,60,38,142,0,0,0,0,0,0,0,0,0,0,0,0,594,346,0,349,0,360,354,0,358,339,340,350,341,0,342,343,344,0,37,39,315,0,272,288,0,0,253,251,0,232,0,0,0,0,0,239,238,79,235,208,0,125,121,0,0,0,517,0,0,533,468,508,0,469,470,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,511,512,543,532,0,514,0,0,0,0,513,0,102,0,0,0,0,0,0,497,498,0,501,502,499,500,0,0,0,503,504,0,505,506,507,0,0,456,457,458,459,460,461,462,463,464,465,455,0,515,0,566,567,0,0,0,581,0,134,426,0,0,0,447,594,601,0,0,0,0,303,443,448,0,445,0,0,422,0,289,0,0,440,441,0,438,0,0,310,0,280,152,157,153,155,148,150,251,0,317,308,318,603,602,0,109,112,0,56,0,0,111,114,250,0,85,84,0,82,231,230,187,251,284,183,317,286,194,195,0,108,19,32,46,0,48,0,50,0,141,143,144,291,290,38,40,134,127,139,0,0,0,159,166,0,134,176,41,0,0,0,0,0,594,0,378,0,381,382,383,0,0,385,0,0,352,0,0,361,355,142,359,351,347,0,192,273,0,0,0,279,252,274,337,0,242,0,253,191,234,233,188,232,0,0,0,0,240,80,0,135,126,475,123,119,0,0,0,0,594,292,297,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,580,587,0,586,451,477,0,479,481,483,485,487,489,491,492,494,496,454,453,565,562,0,564,0,0,0,436,437,0,306,0,444,301,304,432,0,302,435,429,53,0,430,0,0,0,311,0,158,154,156,0,0,232,0,319,0,251,0,328,313,0,0,0,606,0,0,0,151,0,134,0,251,0,203,184,283,0,25,6,8,9,0,42,253,0,162,146,163,0,0,161,169,178,177,0,0,172,0,0,0,368,0,0,0,378,0,379,380,384,0,0,0,356,345,353,0,43,316,251,0,276,0,0,0,0,0,0,338,190,0,254,255,256,0,0,272,243,189,209,211,210,219,240,206,236,516,0,0,0,293,0,298,0,519,0,0,0,0,0,0,0,534,0,576,527,528,531,0,0,535,536,544,0,0,509,584,0,0,563,561,0,0,0,0,305,446,449,431,439,442,312,269,281,337,320,321,232,0,0,232,0,0,55,223,0,0,607,608,594,0,81,83,0,196,0,0,0,232,0,0,0,20,26,145,136,160,0,164,167,179,176,176,0,0,0,0,0,0,0,0,0,378,368,386,0,357,44,253,0,277,275,0,0,0,0,0,0,0,0,0,0,253,0,241,572,0,571,0,294,299,0,471,472,451,451,451,0,0,0,0,473,474,565,564,549,0,585,568,0,588,467,582,583,0,427,428,325,323,327,337,322,232,58,0,0,0,0,0,0,0,604,0,0,86,197,202,337,201,198,232,0,0,0,0,0,0,0,175,174,362,368,0,0,0,0,0,388,389,391,393,395,397,399,401,403,405,408,410,412,414,419,420,0,0,365,374,0,378,0,0,387,245,278,0,0,0,0,0,257,258,0,270,262,259,260,244,251,518,0,0,300,521,522,523,0,0,0,0,530,529,0,555,549,545,547,541,569,0,326,324,0,0,0,0,0,0,0,0,605,609,0,200,199,33,28,0,0,38,0,10,128,165,0,0,0,368,0,417,0,0,368,0,0,0,0,0,0,0,0,0,0,0,0,0,415,368,0,0,378,367,330,331,332,329,261,0,253,574,573,0,0,0,0,0,0,556,555,552,550,0,546,570,611,610,0,0,0,0,622,623,621,619,620,0,0,0,0,29,27,0,30,0,22,180,363,364,0,0,0,0,368,370,392,0,394,396,398,400,402,404,406,407,409,411,0,369,375,0,0,334,335,336,333,0,246,0,0,520,0,0,0,0,558,557,0,551,548,0,0,0,0,0,0,612,0,31,21,23,0,366,416,0,413,371,0,368,377,0,271,296,295,524,0,0,554,0,553,0,0,0,0,0,0,87,24,0,390,368,372,376,0,0,559,615,616,617,614,0,0,418,373,0,526,0,0,0,618,613,525,0,0,0};
# 2085
static short Cyc_yydefgoto[188U]={1259,56,57,58,59,60,547,983,1151,881,882,1076,737,61,361,62,347,63,549,64,551,65,553,66,283,163,67,68,622,273,532,533,274,71,294,275,73,190,191,74,75,76,188,321,322,149,150,323,276,510,566,567,750,77,78,754,755,756,79,568,80,539,81,82,185,186,288,83,133,616,378,379,803,710,130,84,368,610,790,791,792,277,278,1032,602,606,794,504,369,305,110,111,636,557,637,483,484,485,279,362,363,711,516,787,788,350,351,352,165,353,166,354,355,356,897,357,765,358,995,996,997,998,999,1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,486,499,500,487,488,489,359,231,463,232,628,233,234,235,236,237,238,239,240,241,242,436,441,446,450,243,244,245,418,419,945,1052,1053,1126,1054,1128,1193,246,925,247,662,663,248,249,86,1077,490,520,87,865,866,1134,867,1143};
# 2107
static short Cyc_yypact[1262U]={3373,- -32768,- -32768,- -32768,- -32768,- 81,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,4493,323,- 16,- -32768,4493,- -32768,1088,- -32768,72,- 21,- -32768,- -32768,15,36,93,62,81,- -32768,- -32768,97,165,260,296,- -32768,212,- -32768,- -32768,- -32768,209,218,208,253,238,218,- -32768,- -32768,- -32768,263,284,288,3209,- -32768,672,683,309,- -32768,756,4493,4493,4493,- -32768,4493,- -32768,- -32768,326,351,- -32768,72,4271,242,132,225,1392,- -32768,- -32768,327,- -32768,356,388,- 24,- -32768,72,389,7390,- -32768,- -32768,1516,152,- -32768,- -32768,- -32768,- -32768,413,327,415,7390,- -32768,437,1516,466,471,476,- -32768,182,- -32768,- -32768,4601,4601,481,511,3209,3209,640,7390,- -32768,- -32768,83,503,- -32768,- -32768,506,6193,- -32768,394,522,83,5232,- -32768,3209,3209,3536,- -32768,3209,3536,3209,3536,3536,- -32768,102,- -32768,3961,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,327,5232,293,- -32768,- -32768,4271,756,1856,556,4601,4712,6326,- -32768,242,- -32768,574,- -32768,- -32768,- -32768,- -32768,604,627,- -32768,- -32768,632,- -32768,- -32768,- 43,1392,4601,- -32768,- -32768,599,612,645,72,7823,671,7887,7390,7587,673,680,685,692,702,714,719,726,733,755,757,766,768,770,771,772,780,784,7887,7887,- -32768,- -32768,919,8005,2871,785,- -32768,8005,- -32768,- -32768,- -32768,270,- -32768,- -32768,- 36,824,777,779,787,717,143,629,200,- 53,- -32768,1273,8005,162,- 39,- -32768,800,123,- -32768,1516,13,811,1177,819,342,1177,- -32768,- -32768,820,7390,327,942,801,- -32768,- -32768,- -32768,- -32768,803,- -32768,4897,5232,5247,5232,499,- -32768,- -32768,- -32768,41,41,830,817,812,- -32768,- -32768,813,- 22,- -32768,- -32768,362,- -32768,- -32768,823,- -32768,838,57,825,827,821,- -32768,- -32768,1021,411,- -32768,- -32768,- -32768,829,- -32768,834,- -32768,843,846,- -32768,394,919,6459,4271,556,832,847,5232,- -32768,1214,72,839,- -32768,556,474,854,244,855,5380,867,868,877,881,6592,2706,5380,177,871,- -32768,2026,- -32768,882,2196,2196,756,2196,- -32768,- -32768,- -32768,- -32768,889,- -32768,- -32768,- -32768,302,- -32768,- -32768,875,17,911,- -32768,70,890,891,325,892,739,887,7390,912,4601,640,- -32768,- -32768,908,897,- -32768,17,72,- -32768,7390,917,2871,- -32768,5232,2871,- -32768,- -32768,- -32768,5513,- -32768,948,7390,7390,7390,7390,7390,7390,7390,7390,7390,7390,7390,936,7390,7390,5232,1274,7390,7390,- -32768,- -32768,- -32768,- -32768,916,- -32768,2196,918,544,7390,- -32768,7390,- -32768,8005,7390,8005,8005,8005,8005,- -32768,- -32768,8005,- -32768,- -32768,- -32768,- -32768,8005,8005,8005,- -32768,- -32768,8005,- -32768,- -32768,- -32768,8005,7390,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,7390,- -32768,83,- -32768,- -32768,6725,83,7390,- -32768,924,327,- -32768,928,931,933,- -32768,60,413,83,7390,1516,395,- -32768,- -32768,- -32768,929,939,937,1516,- -32768,945,- -32768,539,1177,- -32768,- -32768,949,954,4601,4601,- -32768,943,957,- -32768,5247,5247,5247,- -32768,- -32768,3644,6858,546,- -32768,245,- -32768,- -32768,70,- -32768,- -32768,940,- -32768,965,4601,- -32768,- -32768,- -32768,953,- -32768,958,956,959,- -32768,- -32768,- -32768,1455,- -32768,280,469,- -32768,- -32768,- -32768,5232,- -32768,- -32768,1066,- -32768,3209,- -32768,3209,- -32768,3209,- -32768,964,- -32768,- -32768,- -32768,556,- -32768,327,- -32768,- -32768,900,7390,566,- -32768,6,429,327,837,- -32768,7390,7390,972,971,7390,961,1097,2536,982,- -32768,- -32768,- -32768,567,1083,- -32768,6991,2366,- -32768,985,3036,- -32768,- -32768,4116,- -32768,- -32768,- -32768,4601,- -32768,- -32768,5232,978,5047,- -32768,- -32768,973,483,849,- -32768,5082,891,- -32768,- -32768,- -32768,- -32768,739,981,987,145,988,752,- -32768,912,- -32768,- -32768,- -32768,- -32768,- -32768,990,998,994,1007,996,- -32768,- -32768,700,6459,580,997,1009,1010,1011,1015,1016,1018,1020,1001,583,1017,1019,1022,1023,303,1027,1030,1039,1028,7705,- -32768,- -32768,1031,1041,- -32768,824,129,777,779,787,717,143,629,200,200,- 53,- -32768,- -32768,- -32768,- -32768,- -32768,1033,- -32768,159,4601,6057,5232,- -32768,5232,- -32768,1032,- -32768,- -32768,- -32768,- -32768,1416,- -32768,- -32768,- -32768,- -32768,1043,- -32768,984,295,317,- -32768,5232,- -32768,- -32768,- -32768,1037,1044,739,1040,245,4601,4862,7124,- -32768,- -32768,7390,1052,332,1069,7523,1051,57,3800,1054,327,4601,4712,7257,- -32768,280,- -32768,1060,1163,- -32768,- -32768,- -32768,6459,- -32768,891,857,- -32768,- -32768,1214,1057,7390,- -32768,- -32768,- -32768,- -32768,1064,72,587,594,606,7390,831,608,5380,1062,7390,1068,1073,- -32768,- -32768,1070,1072,1075,2196,- -32768,- -32768,4382,- -32768,875,1077,4601,- -32768,1079,70,1078,1080,1082,1085,- -32768,- -32768,1086,- -32768,1084,1092,1123,- 29,179,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,752,- -32768,- -32768,1099,173,1099,1095,- -32768,5649,- -32768,7587,- -32768,7390,7390,7390,7390,1224,7390,7390,- -32768,7587,- -32768,- -32768,- -32768,- -32768,83,83,- -32768,- -32768,1094,1098,5785,- -32768,- -32768,7390,7390,- -32768,- -32768,17,711,1122,1125,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,483,- -32768,- -32768,739,17,1103,739,1101,625,- -32768,- -32768,699,1104,- -32768,1117,1115,1110,- -32768,- -32768,1111,- -32768,17,1112,1113,739,1109,3536,1126,1223,- -32768,- -32768,5232,- -32768,7390,- -32768,- -32768,1120,58,837,5380,1128,1121,1643,1127,1130,5380,7390,1133,7390,831,- -32768,1134,- -32768,- -32768,891,338,- -32768,- -32768,7390,7390,7390,7390,70,849,849,- 9,1142,70,891,5232,- -32768,- -32768,272,- -32768,7390,- -32768,- -32768,6459,- -32768,948,1135,1138,1140,1147,1129,1148,1149,- -32768,948,1144,1145,- 26,1146,- -32768,- -32768,735,- -32768,- -32768,- -32768,- -32768,6057,- -32768,- -32768,- -32768,- -32768,- -32768,483,- -32768,739,- -32768,1153,1157,1158,1165,1166,1167,1168,1164,1069,1169,- -32768,- -32768,- -32768,483,- -32768,- -32768,739,1175,650,1178,3209,1180,1171,5232,- -32768,- -32768,1301,831,1183,8069,1181,3036,1184,- -32768,- 30,- -32768,1221,1176,725,717,143,783,200,- 53,- -32768,- -32768,- -32768,- -32768,1226,8005,2196,- -32768,- -32768,639,7390,1199,1203,- -32768,- -32768,- -32768,1192,1197,1202,1205,1206,- -32768,- -32768,1204,- -32768,- -32768,- -32768,- -32768,- -32768,1077,- -32768,475,369,- -32768,- -32768,- -32768,- -32768,7390,5232,7390,1337,- -32768,- -32768,1209,88,374,- -32768,- -32768,- -32768,- -32768,5921,- -32768,- -32768,51,51,1069,51,579,579,1069,1232,- -32768,- -32768,1218,- -32768,- -32768,- -32768,- -32768,1225,1230,613,326,- -32768,- -32768,- -32768,648,5380,1231,831,2871,- -32768,5232,1216,1686,8005,7390,8005,8005,8005,8005,8005,8005,8005,8005,8005,8005,7390,- -32768,831,1234,1227,7390,- -32768,483,483,483,483,- -32768,4601,891,- -32768,- -32768,3036,1228,1229,1220,1233,7390,1247,410,- -32768,- -32768,1254,- -32768,- -32768,- -32768,- -32768,1241,1255,1256,1264,- -32768,- -32768,- -32768,- -32768,- -32768,1267,1268,1248,1285,- -32768,- -32768,326,- -32768,1271,655,- -32768,- -32768,- -32768,1272,1266,1277,8005,831,- -32768,824,414,777,779,779,717,143,629,200,200,- 53,- -32768,432,- -32768,- -32768,5380,1275,- -32768,- -32768,- -32768,- -32768,492,- -32768,1279,759,- -32768,1278,1397,5232,657,- -32768,1280,1254,- -32768,- -32768,51,51,1069,51,1069,51,- -32768,1282,- -32768,- -32768,- -32768,326,- -32768,1099,173,- -32768,- -32768,7390,1686,- -32768,5380,- -32768,- -32768,- -32768,- -32768,1269,1283,- -32768,1298,- -32768,1288,1289,1290,1291,1286,1299,- -32768,- -32768,480,- -32768,831,- -32768,- -32768,5232,1292,- -32768,- -32768,- -32768,- -32768,- -32768,1069,51,- -32768,- -32768,1294,- -32768,1295,1296,1297,- -32768,- -32768,- -32768,1453,1454,- -32768};
# 2237
static short Cyc_yypgoto[188U]={- -32768,- -32768,122,- -32768,- -32768,- -32768,- -32768,- -32768,248,- -32768,- -32768,308,- -32768,- -32768,- 301,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 65,- -32768,- 110,56,- -32768,- -32768,0,734,- -32768,89,- 218,1323,33,- -32768,- -32768,- 133,- -32768,- -32768,1419,250,593,- -32768,- -32768,1170,1156,658,307,- -32768,732,- -32768,- -32768,- -32768,14,- -32768,- -32768,67,- 238,1405,- 353,66,- -32768,1302,- -32768,1124,- -32768,1443,- 436,- 330,- -32768,696,- 158,1207,118,- 335,- 583,- 1,- -32768,- -32768,- 83,- 557,- -32768,707,721,- 299,- 531,- 131,- 496,- 125,- -32768,- 272,- 179,- 638,- 368,- -32768,1024,- 187,- 87,- 163,- 183,- 457,- 289,- 777,- 274,- -32768,- -32768,- -32768,1150,- 76,- 549,- 343,- -32768,- 793,- -32768,- 715,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 5,- -32768,- -32768,809,- 219,- -32768,267,472,- -32768,- 147,144,- 178,- 412,- 402,- 408,1074,- 398,- 421,- 433,- 405,- 429,505,507,504,508,- 213,528,616,1201,- -32768,- -32768,- -32768,461,- -32768,- 980,393,- -32768,1114,304,- -32768,- 426,- -32768,760,453,- 64,- 73,- 124,- 226,551,- -32768,554,- 870,- 752,457};
# 2263
static short Cyc_yytable[8242U]={69,591,158,295,593,594,304,596,672,420,367,157,295,425,162,671,665,675,560,395,396,69,112,668,382,69,638,572,667,796,280,281,464,72,612,670,422,673,674,493,290,318,681,370,619,556,776,841,735,900,392,134,793,88,72,521,508,714,72,579,386,69,526,752,753,538,586,607,69,69,69,1127,69,781,530,319,956,91,658,69,170,95,428,714,919,505,162,328,1091,70,517,471,251,515,72,1030,90,447,327,748,348,72,72,72,259,72,778,127,1050,1019,70,116,72,377,70,735,448,449,135,129,541,69,69,472,152,153,154,429,155,517,1031,518,1051,1092,151,164,920,600,94,69,69,69,603,69,69,69,69,69,172,1196,70,69,475,517,72,72,625,70,70,70,884,70,771,69,749,69,599,117,70,476,518,170,72,72,72,852,72,72,72,72,72,797,1058,142,72,1132,1018,734,118,531,1133,1135,252,1137,518,948,72,1084,72,1071,630,300,631,632,44,519,- 173,164,121,90,70,70,253,47,1226,292,734,604,- 600,164,719,349,1050,293,653,119,906,122,70,70,70,329,70,70,70,70,70,676,627,543,70,289,559,285,286,120,1125,474,314,123,626,315,70,327,70,252,437,438,742,498,543,306,307,308,44,309,310,311,312,313,465,466,467,47,507,253,855,700,426,348,19,20,348,348,252,348,51,837,620,508,508,508,1156,290,804,439,440,1161,- 288,728,800,- 288,1107,380,253,292,924,509,426,519,1136,601,1175,293,1145,773,468,638,69,124,839,469,470,587,1021,366,728,1227,1228,263,1230,588,1232,894,- 251,627,264,- 251,1035,69,44,679,514,127,69,682,128,69,69,47,69,72,49,126,255,129,348,689,50,793,793,230,51,131,444,132,811,1213,445,540,54,55,72,257,164,50,44,72,1254,623,72,72,329,72,136,47,49,54,55,49,569,291,137,1178,167,95,168,715,574,716,349,575,169,349,349,717,349,70,320,139,326,827,466,467,703,704,595,505,627,426,957,69,1238,960,49,427,1036,1037,70,905,140,1038,730,70,731,141,70,70,722,70,732,978,930,907,1250,187,834,1229,598,1231,850,426,325,85,72,44,147,468,44,519,921,304,828,470,47,638,46,47,48,883,92,366,851,729,611,93,543,349,691,187,114,519,115,607,605,775,697,863,773,872,423,426,380,1022,289,1253,519,492,159,842,543,843,519,- 18,85,543,872,507,507,507,70,426,777,496,85,764,718,527,426,505,85,840,189,1129,772,1059,44,1119,156,495,162,160,1051,1184,85,47,193,727,929,733,509,509,509,1072,50,192,692,256,51,69,114,69,857,69,481,482,54,55,858,1194,544,44,114,426,744,254,545,874,1125,46,47,48,1214,875,85,85,783,784,514,785,69,426,72,564,72,930,72,85,786,69,1215,258,85,85,85,69,85,85,85,85,85,44,627,540,299,585,876,292,1117,46,47,48,260,72,951,293,1027,85,989,316,261,1034,72,537,1015,317,690,262,72,513,569,958,282,931,932,1249,164,605,70,1038,70,372,70,940,941,164,192,1218,975,50,284,164,296,512,423,802,519,423,1040,513,297,54,55,729,639,1169,641,642,643,70,1105,738,1172,739,649,740,1168,301,70,1162,543,929,926,543,70,699,1165,1166,426,930,950,1164,908,426,1170,1171,666,849,1167,660,512,1116,360,942,943,114,513,348,114,745,327,114,746,768,718,747,426,114,266,267,268,269,388,685,391,393,393,605,373,426,1138,1139,426,890,733,812,891,683,822,627,442,443,380,426,287,415,416,1160,383,892,393,1140,1141,1142,393,426,542,426,374,360,384,893,1149,898,627,627,627,627,85,16,17,18,69,393,426,69,1150,542,570,90,962,375,44,271,576,929,376,113,426,558,46,47,48,569,1106,303,385,544,1074,1104,1075,1207,85,1153,1208,72,426,1089,72,1154,569,743,1224,569,707,708,709,324,143,144,751,1179,1180,1181,1182,389,303,397,349,145,146,164,434,435,398,802,192,614,615,399,757,758,752,753,761,618,400,766,809,810,963,964,965,895,896,1082,401,113,1121,952,953,70,966,967,70,968,558,391,402,113,640,969,1237,403,644,645,646,647,648,69,404,651,652,1056,1057,655,656,405,44,1173,1011,1095,1096,887,1099,1100,661,47,664,1157,605,1158,148,1216,987,988,50,1220,426,406,51,407,72,393,1118,1028,1029,157,54,55,408,603,409,677,410,411,412,266,267,268,269,506,1185,511,413,678,114,417,414,424,661,430,431,1239,114,432,1212,473,348,114,477,491,494,270,433,501,393,502,393,393,393,393,522,523,393,524,129,525,70,393,393,393,529,534,393,535,536,546,393,561,324,789,548,69,96,713,571,861,90,44,542,550,271,542,552,562,604,46,47,48,581,980,85,1223,85,1152,85,50,573,577,393,564,113,69,157,113,72,542,113,54,55,580,542,582,113,348,96,583,1183,589,985,766,44,605,605,605,592,345,605,519,46,47,48,601,49,608,72,613,617,609,50,90,49,1251,564,1023,1024,1025,1026,97,661,624,54,55,661,629,428,1236,650,349,657,497,70,659,250,99,684,694,100,101,102,686,103,47,687,157,688,926,695,104,69,705,105,393,720,696,106,721,107,108,698,97,70,702,701,1079,706,723,726,109,558,725,724,848,736,741,250,99,760,762,100,101,102,72,103,47,759,763,767,769,96,104,779,774,105,1152,798,782,106,808,107,108,799,801,157,805,349,806,114,807,348,109,813,44,- 600,114,821,558,814,815,816,46,47,48,817,818,1016,819,766,820,303,50,823,831,824,537,832,825,826,70,542,513,829,54,55,830,835,393,838,860,836,847,853,844,1039,303,869,542,862,854,542,856,303,864,878,97,870,879,889,873,880,886,888,558,901,69,899,902,903,96,98,99,426,366,100,101,102,909,103,47,904,661,911,916,912,104,913,918,105,914,915,917,106,113,107,108,833,927,72,937,303,113,944,946,954,109,113,955,959,970,303,257,961,971,972,973,974,976,977,303,979,349,982,981,986,990,991,393,1014,1017,1020,1033,558,1045,766,1013,933,934,935,936,1041,938,939,1042,97,1043,1044,1046,1047,1048,1049,1055,1070,1067,70,558,478,1060,949,479,99,1061,1062,100,101,102,96,480,47,1063,1064,1065,1066,1073,104,1078,1081,105,1080,1083,1085,106,85,107,108,1088,481,482,1093,451,393,1094,1090,1103,109,1108,44,1110,114,210,393,1109,1111,423,46,47,48,1112,1115,1163,1113,1114,1123,1124,50,393,128,1146,564,1189,1174,1159,565,1147,303,766,54,55,1148,1155,1176,1192,1177,1187,1188,506,1186,1190,1050,303,1197,1191,452,453,454,455,456,457,458,459,460,461,558,1203,1198,1199,44,99,219,220,100,101,102,1200,393,47,1201,1202,1204,1206,1209,1210,1222,1240,462,558,1211,1219,390,1225,1217,224,1242,1221,156,1247,85,1233,1241,393,393,393,393,1243,1244,1245,1246,1252,1248,1255,1256,1257,1258,1260,1261,113,1234,1205,96,302,871,125,113,7,8,9,10,11,12,13,14,15,16,17,18,19,20,984,563,885,173,174,175,176,177,554,171,178,381,179,180,181,22,23,24,182,26,138,923,183,621,528,922,910,846,597,669,1097,693,1101,1098,1012,32,1130,1102,1235,1120,555,1122,1195,1087,1068,1144,33,34,1069,654,0,0,558,0,156,0,97,0,184,0,40,0,0,393,41,324,0,0,845,0,0,479,99,0,0,100,101,102,42,480,47,96,0,0,43,0,104,0,0,105,0,0,0,106,0,107,108,0,481,482,0,0,0,303,0,0,109,364,0,0,44,0,0,0,0,0,0,90,46,47,48,0,49,0,366,156,0,0,50,0,0,0,537,0,52,53,0,0,513,0,54,55,393,0,393,393,393,393,393,393,393,393,393,393,0,0,97,0,0,0,0,0,0,0,0,0,0,303,0,0,0,250,99,0,0,100,101,102,113,103,47,0,0,0,156,0,104,0,0,105,0,992,993,106,0,107,108,0,0,0,0,0,0,0,0,0,109,0,96,0,393,0,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,895,896,21,194,195,330,0,331,332,333,334,335,336,337,338,22,23,24,339,26,96,27,197,340,0,0,0,0,198,199,28,341,0,0,393,32,200,201,202,203,204,205,206,207,0,208,33,34,35,209,0,0,97,210,0,0,0,0,0,0,40,211,212,213,41,214,0,250,99,0,0,100,101,102,0,103,47,0,42,215,216,0,104,0,43,105,0,217,218,994,0,107,108,0,0,0,0,0,0,225,0,0,109,227,0,0,228,229,0,0,342,99,219,220,100,101,102,90,46,47,48,0,49,221,343,344,0,0,222,0,0,0,223,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,194,195,330,0,331,332,333,334,335,336,337,338,22,23,24,339,26,96,27,197,340,0,0,0,0,198,199,28,341,0,0,0,32,200,201,202,203,204,205,206,207,0,208,33,34,35,209,0,0,0,210,0,0,0,0,0,0,40,211,212,213,41,214,0,0,0,0,0,0,0,0,0,0,0,0,42,215,216,0,0,0,43,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,342,99,219,220,100,101,102,90,46,47,48,0,49,221,343,344,345,0,222,0,0,0,223,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,194,195,330,0,331,332,333,334,335,336,337,338,22,23,24,339,26,96,27,197,340,0,0,0,0,198,199,28,341,0,0,0,32,200,201,202,203,204,205,206,207,0,208,33,34,35,209,0,0,0,210,0,0,0,0,0,0,40,211,212,213,41,214,0,0,0,0,0,0,0,0,0,0,0,0,42,215,216,0,0,0,43,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,342,99,219,220,100,101,102,90,46,47,48,0,49,221,343,344,590,0,222,0,0,0,223,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,194,195,330,0,331,332,333,334,335,336,337,338,22,23,24,339,26,96,27,197,340,0,0,0,0,198,199,28,341,0,0,0,32,200,201,202,203,204,205,206,207,0,208,33,34,35,209,0,0,0,210,0,0,0,0,0,0,40,211,212,213,41,214,0,0,0,0,0,0,0,0,0,0,0,0,42,215,216,0,0,0,43,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,342,99,219,220,100,101,102,90,46,47,48,0,49,221,343,344,0,0,222,0,0,0,223,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,194,195,330,0,331,332,333,334,335,336,337,338,22,23,24,339,26,96,27,197,340,0,0,0,0,198,199,28,341,0,0,0,32,200,201,202,203,204,205,206,207,0,208,33,34,35,209,0,0,0,210,0,0,0,0,0,0,40,211,212,213,41,214,0,0,0,0,0,0,0,0,0,0,0,0,42,215,216,0,0,0,43,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,578,99,219,220,100,101,102,90,46,47,48,0,49,221,343,344,0,0,222,0,0,0,223,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,194,195,0,0,0,0,0,0,0,0,0,0,22,23,24,339,26,96,27,197,0,0,0,0,0,198,199,28,0,0,0,0,32,200,201,202,203,204,205,206,207,0,208,33,34,35,209,0,0,0,210,0,0,0,0,0,0,40,211,212,213,41,214,0,0,0,0,0,0,0,0,0,0,0,0,42,215,216,0,0,0,43,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,90,46,47,48,0,49,221,0,0,0,0,222,0,0,0,223,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,194,195,0,0,0,0,0,0,0,0,0,0,22,23,24,339,26,96,0,0,0,0,0,0,0,0,0,28,0,0,0,0,32,200,201,202,203,204,205,206,207,0,208,33,34,0,0,0,0,0,210,0,0,0,0,0,0,40,211,212,213,41,214,0,0,0,0,0,0,0,0,0,0,0,0,42,215,216,0,0,0,43,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,90,46,47,48,0,49,221,0,0,0,0,222,0,0,0,390,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,22,23,24,196,26,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,32,200,201,202,203,204,205,206,207,0,208,33,34,0,209,0,0,0,210,0,0,0,0,0,0,40,211,212,213,41,214,0,0,0,0,0,0,0,0,0,0,0,0,42,215,216,0,0,0,43,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,90,46,47,48,0,49,221,0,421,0,0,222,0,0,0,223,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,22,23,24,196,26,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,32,200,201,202,203,204,205,206,207,0,208,33,34,0,209,0,0,0,210,0,0,0,0,0,0,40,211,212,213,41,214,0,0,0,0,0,0,0,0,0,0,0,0,42,215,216,0,0,0,43,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,90,46,47,48,0,49,221,0,0,0,0,222,0,0,0,223,0,52,346,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229,- 15,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,36,37,0,38,39,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,0,0,0,0,0,0,90,46,47,48,0,49,0,0,0,- 15,0,50,0,0,0,51,0,52,53,0,0,0,0,54,55,- 15,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,36,37,0,38,39,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,0,0,0,0,0,0,45,46,47,48,0,49,0,0,0,0,0,50,0,0,0,51,0,52,53,0,0,0,0,54,55,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,29,30,31,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,36,37,0,38,39,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,44,0,0,0,0,0,0,90,46,47,48,0,49,22,23,24,- 15,26,50,0,0,0,51,0,52,53,0,0,0,0,54,55,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,364,0,0,0,0,0,0,0,0,0,90,46,0,48,0,49,0,366,0,0,0,50,0,0,0,512,0,52,53,0,0,513,0,54,55,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,0,0,0,0,0,0,90,46,47,48,0,49,0,0,0,0,0,50,0,0,0,564,0,52,53,0,0,0,0,54,55,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,90,46,0,48,0,49,316,0,161,0,0,0,317,0,0,0,0,52,53,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,90,46,0,48,0,49,316,0,344,0,0,0,317,0,0,0,0,52,53,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,26,0,27,0,0,0,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,90,46,0,48,0,49,0,0,161,22,23,24,25,26,0,27,0,52,53,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,35,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,2,3,4,89,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,90,46,0,48,0,49,0,0,344,22,23,24,25,26,0,0,0,52,53,0,0,0,0,0,28,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,90,46,0,48,0,49,22,23,24,0,26,0,0,0,0,0,0,52,53,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,265,0,0,0,0,0,266,267,268,269,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,270,0,43,0,0,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,90,46,0,48,271,49,0,0,272,22,23,24,0,26,0,0,0,52,53,0,0,0,0,0,0,0,0,0,0,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,364,0,0,365,0,0,0,0,0,0,90,46,0,48,0,49,0,366,0,0,0,0,0,0,0,0,0,52,53,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,22,23,24,0,26,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,32,0,0,0,0,0,0,0,42,0,0,33,34,0,43,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,364,0,0,0,0,0,42,0,0,0,90,46,43,48,0,49,0,366,0,0,0,0,0,0,0,0,0,52,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,90,46,0,48,0,49,0,0,0,503,0,0,0,0,0,0,0,52,53,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,22,23,24,0,26,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,32,0,0,0,0,0,0,0,42,0,0,33,34,0,43,0,0,0,0,0,0,0,0,0,0,40,0,0,0,41,0,0,0,0,0,0,0,0,0,0,0,0,0,42,0,0,0,90,46,43,48,0,49,0,0,0,780,0,0,0,0,0,0,0,52,53,0,0,0,0,0,0,795,0,0,0,0,0,0,0,0,0,90,46,0,48,0,49,0,0,0,0,0,0,0,0,0,0,0,52,53,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,22,23,24,0,26,0,0,0,0,0,0,0,0,0,0,22,23,24,0,26,32,0,0,0,0,0,0,0,0,0,0,33,34,0,0,32,0,0,0,0,0,0,0,0,0,40,33,34,0,41,0,0,0,0,0,0,0,0,0,0,40,0,0,42,41,0,0,0,0,43,0,0,0,0,0,0,0,0,42,0,0,0,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,90,46,0,48,0,49,0,0,0,0,0,0,0,0,0,90,0,52,53,0,49,0,0,0,0,0,0,0,0,0,0,0,52,53,194,195,330,0,331,332,333,334,335,336,337,338,0,0,0,196,0,96,0,197,340,0,0,0,0,198,199,0,341,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,578,99,219,220,100,101,102,0,0,47,0,0,0,221,343,344,0,0,222,0,0,0,223,0,0,224,194,195,0,0,0,0,225,633,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,634,99,219,220,100,101,102,0,293,47,0,0,0,221,0,394,635,0,222,0,0,0,223,0,0,224,0,481,482,194,195,0,225,0,0,226,227,0,0,228,229,0,0,0,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,634,99,219,220,100,101,102,0,293,47,0,0,0,221,0,394,928,0,222,0,0,0,223,0,0,224,0,481,482,194,195,0,225,0,0,226,227,0,0,228,229,0,0,0,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,634,99,219,220,100,101,102,0,293,47,0,0,0,221,0,394,947,0,222,0,0,0,223,0,0,224,0,481,482,194,195,0,225,0,0,226,227,0,0,228,229,0,0,0,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,634,99,219,220,100,101,102,0,293,47,0,0,0,221,0,394,1131,0,222,0,0,0,223,0,0,224,0,481,482,194,195,0,225,0,0,226,227,0,0,228,229,0,0,0,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,634,99,219,220,100,101,102,0,293,47,0,0,0,221,0,394,0,0,222,0,0,0,223,0,0,224,0,481,482,194,195,0,225,0,0,226,227,0,0,228,229,0,0,0,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,298,0,47,0,0,0,221,0,0,0,0,222,0,0,0,223,0,0,224,194,195,0,0,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,0,0,0,222,0,0,0,223,0,0,224,194,195,0,371,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,394,0,0,222,0,0,0,223,0,0,224,194,195,0,0,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,584,0,0,0,222,0,0,0,223,0,0,224,194,195,0,0,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,0,0,0,222,0,0,0,223,680,0,224,194,195,0,0,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,0,0,0,222,0,0,0,223,0,0,224,194,195,0,712,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,0,0,0,222,0,0,0,223,770,0,224,194,195,0,0,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,0,0,0,222,0,0,0,223,0,0,224,194,195,0,859,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,0,0,0,222,0,0,0,223,0,0,224,194,195,0,877,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,0,0,0,222,0,0,0,223,0,0,224,194,195,0,0,0,0,225,0,0,226,227,0,0,228,229,196,0,96,0,197,0,0,0,0,0,198,199,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,209,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,194,195,0,0,0,0,0,0,0,0,0,0,215,216,0,196,0,96,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,868,99,219,220,100,101,102,210,0,47,0,0,0,221,0,211,212,213,222,214,0,0,223,0,0,224,0,0,0,0,0,0,225,215,216,226,227,0,0,228,229,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,194,195,221,0,394,0,0,222,0,0,0,223,0,0,224,196,0,96,0,0,0,225,0,0,226,227,0,0,228,229,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,0,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,0,0,0,0,0,0,0,0,0,0,0,0,215,216,0,0,0,0,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,194,195,221,0,833,0,0,222,0,0,0,223,0,0,224,196,0,96,0,0,0,225,0,0,226,227,0,0,228,229,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,0,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,194,195,0,0,0,0,0,0,0,0,0,0,215,216,0,196,0,96,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,44,99,219,220,100,101,102,210,0,47,0,0,0,221,0,211,212,213,222,214,0,0,387,0,0,224,0,0,0,0,0,0,225,215,216,226,227,0,0,228,229,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,194,195,221,0,0,0,0,222,0,0,0,390,0,0,224,196,0,96,0,0,0,225,0,0,226,227,0,0,228,229,0,0,0,200,201,202,203,204,205,206,207,0,208,0,0,0,0,0,0,0,210,0,0,0,0,0,0,0,211,212,213,0,214,0,194,195,0,0,0,0,0,0,0,0,0,0,215,216,0,196,0,96,0,0,217,218,0,0,0,0,0,0,0,0,0,0,0,200,201,202,203,204,205,206,207,0,208,44,99,219,220,100,101,102,210,0,47,0,0,0,221,0,211,212,213,222,214,0,0,223,0,0,224,0,0,0,0,0,0,225,215,216,226,227,0,0,228,229,217,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,44,99,219,220,100,101,102,0,0,47,0,0,0,221,0,0,0,0,222,0,0,0,1086,0,0,224,0,0,0,0,0,0,225,0,0,226,227,0,0,228,229};
# 3091
static short Cyc_yycheck[8242U]={0,344,75,127,347,348,137,350,441,222,168,75,136,226,79,436,428,446,319,198,198,21,27,431,187,25,394,328,430,612,117,118,245,0,369,433,223,442,443,258,123,151,468,168,374,317,595,685,544,764,197,51,609,134,21,281,274,514,25,333,193,61,288,5,6,303,340,366,68,69,70,1051,72,604,17,151,853,21,421,79,80,25,118,540,113,272,151,163,118,0,112,130,97,276,61,104,139,150,163,93,165,68,69,70,109,72,602,131,134,902,21,132,79,156,25,611,169,170,51,143,303,121,122,162,68,69,70,163,72,112,139,153,158,163,67,79,165,363,154,139,140,141,72,143,144,145,146,147,82,1129,61,151,139,112,121,122,382,68,69,70,743,72,588,163,158,165,149,152,79,156,153,171,139,140,141,706,143,144,145,146,147,617,959,61,151,134,901,540,152,132,139,1061,132,1063,153,833,163,990,165,976,387,134,389,390,132,164,148,151,146,139,121,122,152,141,1194,132,569,147,158,163,519,165,134,140,411,132,775,146,139,140,141,164,143,144,145,146,147,450,385,303,151,123,318,121,122,152,158,252,146,152,383,149,163,318,165,132,113,114,559,264,324,139,140,141,132,143,144,145,146,147,108,109,110,141,274,152,712,496,149,344,20,21,347,348,132,350,154,158,375,507,508,509,1085,376,624,152,153,1090,146,537,155,149,1017,185,152,132,133,274,149,164,1062,132,1105,140,1066,589,154,685,318,154,161,159,160,146,907,146,564,1197,1198,147,1200,154,1202,759,155,482,154,158,921,339,132,465,276,131,344,469,134,347,348,141,350,318,144,57,104,143,421,481,150,916,917,94,154,154,164,147,638,1160,168,303,162,163,339,106,318,150,132,344,1248,379,347,348,315,350,131,141,144,162,163,144,324,124,154,1108,152,339,154,152,154,154,344,157,160,347,348,160,350,318,158,146,160,108,109,110,501,502,349,604,565,149,856,421,1215,859,144,155,922,155,339,772,146,159,152,344,154,147,347,348,525,350,160,877,810,778,1237,152,659,1199,146,1201,155,149,159,0,421,132,147,154,132,164,795,592,159,160,141,833,140,141,142,741,147,146,155,537,149,22,540,421,483,152,27,164,29,782,366,595,491,155,762,727,223,149,374,155,376,1247,164,155,147,686,564,688,164,147,51,569,744,507,508,509,421,149,599,263,61,580,516,155,149,706,67,684,134,149,589,961,132,158,75,262,595,78,158,1116,81,141,147,537,810,539,507,508,509,979,150,92,151,132,154,549,97,551,715,553,159,160,162,163,716,149,149,132,109,149,564,152,155,730,158,140,141,142,158,731,121,122,93,94,512,96,580,149,549,154,551,953,553,134,105,589,158,154,139,140,141,595,143,144,145,146,147,132,749,537,132,338,731,132,133,140,141,142,146,580,840,140,915,164,892,145,149,920,589,154,898,151,482,151,595,160,564,857,151,812,812,155,580,519,549,159,551,169,553,822,822,589,193,155,874,150,139,595,149,154,387,622,164,390,930,160,154,162,163,727,397,1098,399,400,401,580,1013,549,1101,551,407,553,1097,155,589,1091,744,953,806,747,595,146,1094,1095,149,1057,837,1093,779,149,1099,1100,429,702,1096,155,154,1036,146,827,828,252,160,772,255,565,775,258,146,146,714,149,149,264,78,79,80,81,194,473,196,197,198,609,154,149,151,152,149,146,734,155,149,470,155,886,111,112,624,149,104,217,218,1090,149,155,222,172,173,174,226,149,303,149,154,146,148,155,149,155,911,912,913,914,315,17,18,19,772,245,149,775,1077,324,325,139,155,154,132,143,331,1057,154,27,149,317,140,141,142,727,155,137,151,149,148,1012,150,146,349,155,149,772,149,994,775,1083,744,561,155,747,507,508,509,159,146,147,570,1110,1111,1112,1113,154,168,154,772,146,147,775,115,116,154,802,383,98,99,154,573,574,5,6,577,373,154,580,148,149,151,152,153,22,23,986,154,97,1045,148,149,772,163,164,775,166,394,339,154,109,398,172,1215,154,402,403,404,405,406,879,154,409,410,148,149,413,414,154,132,1102,895,166,167,749,111,112,424,141,426,1086,782,1088,146,1177,890,891,150,148,149,154,154,154,879,385,1038,916,917,981,162,163,154,72,154,451,154,154,154,78,79,80,81,273,1119,275,154,463,483,18,154,154,468,117,165,1217,491,166,1159,147,1013,496,139,132,132,104,167,154,428,154,430,431,432,433,132,146,436,153,143,154,879,441,442,443,134,148,446,148,155,148,450,147,322,132,148,983,42,513,147,720,139,132,537,148,143,540,148,148,147,140,141,142,132,879,549,1190,551,1078,553,150,154,154,482,154,252,1013,1078,255,983,564,258,162,163,154,569,146,264,1090,42,146,1115,158,886,764,132,915,916,917,154,148,920,164,140,141,142,132,144,155,1013,155,161,158,150,139,144,1240,154,911,912,913,914,117,588,164,162,163,592,148,118,1214,132,1013,154,129,983,155,132,133,152,148,136,137,138,153,140,141,153,1149,153,1211,149,147,1090,148,150,565,154,158,154,132,156,157,155,117,1013,149,155,983,149,154,149,167,638,155,154,129,48,151,132,133,147,158,136,137,138,1090,140,141,154,30,146,46,42,147,154,148,150,1208,155,164,154,132,156,157,155,155,1208,155,1090,149,695,155,1215,167,155,132,158,702,155,685,149,149,149,140,141,142,149,149,899,149,901,149,512,150,155,134,155,154,148,155,155,1090,727,160,155,162,163,155,155,659,155,717,149,148,155,161,927,537,724,744,146,155,747,161,544,134,732,117,155,147,755,155,47,154,148,741,146,1215,154,147,146,42,132,133,149,146,136,137,138,148,140,141,155,759,154,149,154,147,154,114,150,154,154,149,154,483,156,157,147,152,1215,25,592,491,158,155,132,167,496,132,155,155,602,994,161,146,149,155,155,155,155,611,161,1215,49,147,154,147,155,749,148,146,146,139,810,154,1017,158,814,815,816,817,155,819,820,155,117,155,149,149,149,155,155,155,133,139,1215,833,129,154,836,132,133,154,154,136,137,138,42,140,141,154,154,154,154,148,147,147,155,150,148,28,147,154,879,156,157,154,159,160,117,66,812,165,158,117,167,146,132,155,895,75,822,148,155,1086,140,141,142,155,154,1092,155,155,25,154,150,837,134,149,154,149,1103,155,158,148,716,1108,162,163,148,148,146,134,155,155,155,727,1119,154,134,731,149,1124,119,120,121,122,123,124,125,126,127,128,930,155,149,149,132,133,134,135,136,137,138,149,886,141,149,149,133,148,148,155,25,154,151,953,149,148,154,149,155,157,134,155,981,149,983,155,155,911,912,913,914,155,155,155,155,155,149,155,155,155,155,0,0,695,1208,1149,42,136,726,42,702,8,9,10,11,12,13,14,15,16,17,18,19,20,21,884,322,747,88,89,90,91,92,315,81,95,186,97,98,99,37,38,39,103,41,54,802,107,376,294,795,782,695,355,432,1002,484,1005,1003,895,57,1052,1006,1211,1044,316,1046,1126,992,970,1065,68,69,971,412,- 1,- 1,1057,- 1,1078,- 1,117,- 1,143,- 1,82,- 1,- 1,1012,86,884,- 1,- 1,129,- 1,- 1,132,133,- 1,- 1,136,137,138,100,140,141,42,- 1,- 1,106,- 1,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,159,160,- 1,- 1,- 1,922,- 1,- 1,167,129,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,- 1,139,140,141,142,- 1,144,- 1,146,1149,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,160,- 1,162,163,1091,- 1,1093,1094,1095,1096,1097,1098,1099,1100,1101,1102,- 1,- 1,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,986,- 1,- 1,- 1,132,133,- 1,- 1,136,137,138,895,140,141,- 1,- 1,- 1,1208,- 1,147,- 1,- 1,150,- 1,25,26,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,- 1,42,- 1,1159,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,1214,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,117,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,132,133,- 1,- 1,136,137,138,- 1,140,141,- 1,100,101,102,- 1,147,- 1,106,150,- 1,109,110,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,146,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,146,147,148,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,146,147,148,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,146,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,- 1,- 1,- 1,- 1,50,51,52,53,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,146,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,- 1,50,51,52,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,70,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,- 1,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,57,58,59,60,61,62,63,64,65,- 1,67,68,69,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,82,83,84,85,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,101,102,- 1,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,140,141,142,- 1,144,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,- 1,76,77,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,- 1,139,140,141,142,- 1,144,- 1,- 1,- 1,148,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,162,163,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,- 1,76,77,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,- 1,139,140,141,142,- 1,144,- 1,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,162,163,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,73,74,- 1,76,77,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,- 1,139,140,141,142,- 1,144,37,38,39,148,41,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,162,163,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,- 1,144,- 1,146,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,160,- 1,162,163,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,- 1,139,140,141,142,- 1,144,- 1,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,156,157,- 1,- 1,- 1,- 1,162,163,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,- 1,144,145,- 1,147,- 1,- 1,- 1,151,- 1,- 1,- 1,- 1,156,157,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,- 1,144,145,- 1,147,- 1,- 1,- 1,151,- 1,- 1,- 1,- 1,156,157,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,139,140,- 1,142,- 1,144,- 1,- 1,147,37,38,39,40,41,- 1,43,- 1,156,157,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,139,140,- 1,142,- 1,144,- 1,- 1,147,37,38,39,40,41,- 1,- 1,- 1,156,157,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,- 1,144,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,72,- 1,- 1,- 1,- 1,- 1,78,79,80,81,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,104,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,143,144,- 1,- 1,147,37,38,39,- 1,41,- 1,- 1,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,- 1,144,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,156,157,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,68,69,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,139,140,106,142,- 1,144,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,- 1,144,- 1,- 1,- 1,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,156,157,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,68,69,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,139,140,106,142,- 1,144,- 1,- 1,- 1,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,156,157,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,- 1,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,156,157,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,- 1,- 1,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,68,69,- 1,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,- 1,- 1,100,86,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,140,- 1,142,- 1,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,- 1,156,157,- 1,144,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,156,157,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,- 1,44,45,- 1,- 1,- 1,- 1,50,51,- 1,53,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,146,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,- 1,- 1,- 1,164,32,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,140,141,- 1,- 1,- 1,145,- 1,147,148,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,- 1,159,160,25,26,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,140,141,- 1,- 1,- 1,145,- 1,147,148,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,- 1,159,160,25,26,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,140,141,- 1,- 1,- 1,145,- 1,147,148,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,- 1,159,160,25,26,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,140,141,- 1,- 1,- 1,145,- 1,147,148,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,- 1,159,160,25,26,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,140,141,- 1,- 1,- 1,145,- 1,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,- 1,159,160,25,26,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,- 1,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,139,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,161,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,146,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,155,- 1,157,25,26,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,161,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,155,- 1,157,25,26,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,161,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,161,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,25,26,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,40,- 1,42,- 1,44,- 1,- 1,- 1,- 1,- 1,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,71,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,40,- 1,42,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,132,133,134,135,136,137,138,75,- 1,141,- 1,- 1,- 1,145,- 1,83,84,85,150,87,- 1,- 1,154,- 1,- 1,157,- 1,- 1,- 1,- 1,- 1,- 1,164,101,102,167,168,- 1,- 1,171,172,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,25,26,145,- 1,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,40,- 1,42,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,25,26,145,- 1,147,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,40,- 1,42,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,40,- 1,42,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,132,133,134,135,136,137,138,75,- 1,141,- 1,- 1,- 1,145,- 1,83,84,85,150,87,- 1,- 1,154,- 1,- 1,157,- 1,- 1,- 1,- 1,- 1,- 1,164,101,102,167,168,- 1,- 1,171,172,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,25,26,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,40,- 1,42,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,- 1,- 1,- 1,- 1,- 1,- 1,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,85,- 1,87,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,102,- 1,40,- 1,42,- 1,- 1,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,61,62,63,64,65,- 1,67,132,133,134,135,136,137,138,75,- 1,141,- 1,- 1,- 1,145,- 1,83,84,85,150,87,- 1,- 1,154,- 1,- 1,157,- 1,- 1,- 1,- 1,- 1,- 1,164,101,102,167,168,- 1,- 1,171,172,109,110,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,132,133,134,135,136,137,138,- 1,- 1,141,- 1,- 1,- 1,145,- 1,- 1,- 1,- 1,150,- 1,- 1,- 1,154,- 1,- 1,157,- 1,- 1,- 1,- 1,- 1,- 1,164,- 1,- 1,167,168,- 1,- 1,171,172};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int,int);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*,struct Cyc_Yyltype*);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple33{unsigned f0;struct _tuple0*f1;int f2;};struct _tuple34{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};struct _tuple35{void*f0;struct Cyc_List_List*f1;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){union Cyc_YYSTYPE _T0;struct _fat_ptr _T1;struct _RegionHandle*_T2;void*_T3;struct Cyc_Yystacktype*_T4;struct Cyc_Yystacktype*_T5;struct _RegionHandle*_T6;unsigned _T7;unsigned _T8;unsigned _T9;struct _fat_ptr _TA;int _TB;char*_TC;short*_TD;int _TE;int _TF;int _T10;int _T11;struct _fat_ptr _T12;int _T13;int _T14;struct Cyc_Yystack_overflow_exn_struct*_T15;struct Cyc_Yystack_overflow_exn_struct*_T16;struct _fat_ptr _T17;int _T18;short*_T19;struct _RegionHandle*_T1A;unsigned _T1B;int _T1C;unsigned _T1D;int _T1E;unsigned _T1F;unsigned _T20;struct _fat_ptr _T21;unsigned char*_T22;short*_T23;unsigned _T24;int _T25;unsigned _T26;struct _fat_ptr _T27;int _T28;struct Cyc_Yystacktype*_T29;struct _RegionHandle*_T2A;unsigned _T2B;int _T2C;unsigned _T2D;int _T2E;unsigned _T2F;unsigned _T30;struct _fat_ptr _T31;unsigned _T32;int _T33;char*_T34;struct Cyc_Yystacktype*_T35;unsigned _T36;struct _fat_ptr _T37;char*_T38;struct Cyc_Yystacktype*_T39;short*_T3A;int _T3B;char*_T3C;short*_T3D;short _T3E;struct Cyc_Lexing_lexbuf*_T3F;union Cyc_YYSTYPE*_T40;union Cyc_YYSTYPE*_T41;struct Cyc_Yyltype*_T42;struct Cyc_Yyltype*_T43;int _T44;short*_T45;int _T46;short _T47;int _T48;short*_T49;int _T4A;short _T4B;int _T4C;int _T4D;short*_T4E;int _T4F;short _T50;struct _fat_ptr _T51;int _T52;char*_T53;struct Cyc_Yystacktype*_T54;struct Cyc_Yystacktype _T55;short*_T56;int _T57;char*_T58;short*_T59;short _T5A;short*_T5B;int _T5C;char*_T5D;short*_T5E;short _T5F;struct _fat_ptr _T60;int _T61;int _T62;int _T63;struct _fat_ptr _T64;char*_T65;char*_T66;struct Cyc_Yystacktype*_T67;struct Cyc_Yystacktype _T68;int _T69;int _T6A;struct Cyc_Yystacktype*_T6B;union Cyc_YYSTYPE*_T6C;union Cyc_YYSTYPE*_T6D;struct Cyc_Yystacktype*_T6E;struct Cyc_Yystacktype _T6F;struct Cyc_Yystacktype*_T70;union Cyc_YYSTYPE*_T71;union Cyc_YYSTYPE*_T72;struct Cyc_Yystacktype*_T73;union Cyc_YYSTYPE*_T74;union Cyc_YYSTYPE*_T75;struct Cyc_List_List*_T76;struct Cyc_Yystacktype*_T77;union Cyc_YYSTYPE*_T78;union Cyc_YYSTYPE*_T79;struct Cyc_List_List*_T7A;struct Cyc_List_List*_T7B;struct Cyc_List_List*_T7C;struct Cyc_Absyn_Decl*_T7D;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T7E;struct Cyc_Yystacktype*_T7F;union Cyc_YYSTYPE*_T80;union Cyc_YYSTYPE*_T81;struct Cyc_Yystacktype*_T82;union Cyc_YYSTYPE*_T83;union Cyc_YYSTYPE*_T84;struct Cyc_Yystacktype*_T85;struct Cyc_Yystacktype _T86;struct Cyc_Yyltype _T87;unsigned _T88;struct Cyc_List_List*_T89;struct Cyc_Absyn_Decl*_T8A;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T8B;struct Cyc_Yystacktype*_T8C;union Cyc_YYSTYPE*_T8D;union Cyc_YYSTYPE*_T8E;struct Cyc_Yystacktype*_T8F;union Cyc_YYSTYPE*_T90;union Cyc_YYSTYPE*_T91;struct Cyc_Yystacktype*_T92;struct Cyc_Yystacktype _T93;struct Cyc_Yyltype _T94;unsigned _T95;struct Cyc_Yystacktype*_T96;union Cyc_YYSTYPE*_T97;union Cyc_YYSTYPE*_T98;struct Cyc_List_List*_T99;struct Cyc_Absyn_Decl*_T9A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T9B;struct _fat_ptr*_T9C;struct Cyc_Yystacktype*_T9D;union Cyc_YYSTYPE*_T9E;union Cyc_YYSTYPE*_T9F;struct Cyc_Yystacktype*_TA0;union Cyc_YYSTYPE*_TA1;union Cyc_YYSTYPE*_TA2;struct Cyc_Yystacktype*_TA3;struct Cyc_Yystacktype _TA4;struct Cyc_Yyltype _TA5;unsigned _TA6;struct Cyc_List_List*_TA7;struct Cyc_Absyn_Decl*_TA8;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_TA9;struct _fat_ptr*_TAA;struct Cyc_Yystacktype*_TAB;union Cyc_YYSTYPE*_TAC;union Cyc_YYSTYPE*_TAD;struct Cyc_Yystacktype*_TAE;union Cyc_YYSTYPE*_TAF;union Cyc_YYSTYPE*_TB0;struct Cyc_Yystacktype*_TB1;struct Cyc_Yystacktype _TB2;struct Cyc_Yyltype _TB3;unsigned _TB4;struct Cyc_Yystacktype*_TB5;union Cyc_YYSTYPE*_TB6;union Cyc_YYSTYPE*_TB7;struct Cyc_Yystacktype*_TB8;union Cyc_YYSTYPE*_TB9;union Cyc_YYSTYPE*_TBA;struct Cyc_List_List*_TBB;struct Cyc_Yystacktype*_TBC;union Cyc_YYSTYPE*_TBD;union Cyc_YYSTYPE*_TBE;struct Cyc_List_List*_TBF;struct Cyc_List_List*_TC0;struct Cyc_Yystacktype*_TC1;union Cyc_YYSTYPE*_TC2;union Cyc_YYSTYPE*_TC3;struct Cyc_Yystacktype*_TC4;union Cyc_YYSTYPE*_TC5;union Cyc_YYSTYPE*_TC6;struct Cyc_Yystacktype*_TC7;union Cyc_YYSTYPE*_TC8;union Cyc_YYSTYPE*_TC9;struct Cyc_Yystacktype*_TCA;union Cyc_YYSTYPE*_TCB;union Cyc_YYSTYPE*_TCC;struct Cyc_Yystacktype*_TCD;struct Cyc_Yystacktype _TCE;struct Cyc_Yyltype _TCF;unsigned _TD0;unsigned _TD1;struct _fat_ptr _TD2;struct _fat_ptr _TD3;struct Cyc_List_List*_TD4;unsigned _TD5;unsigned _TD6;int _TD7;struct Cyc_Yystacktype*_TD8;struct Cyc_Yystacktype _TD9;struct Cyc_Yyltype _TDA;unsigned _TDB;int _TDC;struct Cyc_Yystacktype*_TDD;struct Cyc_Yystacktype _TDE;struct Cyc_Yyltype _TDF;unsigned _TE0;unsigned _TE1;struct _fat_ptr _TE2;struct _fat_ptr _TE3;struct Cyc_List_List*_TE4;struct Cyc_Absyn_Decl*_TE5;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_TE6;struct Cyc_Yystacktype*_TE7;union Cyc_YYSTYPE*_TE8;union Cyc_YYSTYPE*_TE9;struct _tuple10*_TEA;struct Cyc_Yystacktype*_TEB;struct Cyc_Yystacktype _TEC;struct Cyc_Yyltype _TED;unsigned _TEE;struct Cyc_Yystacktype*_TEF;union Cyc_YYSTYPE*_TF0;union Cyc_YYSTYPE*_TF1;struct Cyc_List_List*_TF2;struct Cyc_Absyn_Decl*_TF3;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_TF4;struct Cyc_Yystacktype*_TF5;union Cyc_YYSTYPE*_TF6;union Cyc_YYSTYPE*_TF7;struct Cyc_Yystacktype*_TF8;struct Cyc_Yystacktype _TF9;struct Cyc_Yyltype _TFA;unsigned _TFB;struct Cyc_Yystacktype*_TFC;union Cyc_YYSTYPE*_TFD;union Cyc_YYSTYPE*_TFE;struct Cyc_List_List*_TFF;struct Cyc_Absyn_Decl*_T100;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T101;struct Cyc_Yystacktype*_T102;union Cyc_YYSTYPE*_T103;union Cyc_YYSTYPE*_T104;struct _tuple10*_T105;struct Cyc_Yystacktype*_T106;struct Cyc_Yystacktype _T107;struct Cyc_Yyltype _T108;unsigned _T109;struct Cyc_Yystacktype*_T10A;union Cyc_YYSTYPE*_T10B;union Cyc_YYSTYPE*_T10C;struct Cyc_List_List*_T10D;struct Cyc_Absyn_Decl*_T10E;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T10F;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T110;struct Cyc_Yystacktype*_T111;struct Cyc_Yystacktype _T112;struct Cyc_Yyltype _T113;unsigned _T114;struct Cyc_Yystacktype*_T115;union Cyc_YYSTYPE*_T116;union Cyc_YYSTYPE*_T117;struct Cyc_List_List*_T118;struct Cyc_Absyn_Decl*_T119;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T11A;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T11B;struct Cyc_Yystacktype*_T11C;struct Cyc_Yystacktype _T11D;struct Cyc_Yyltype _T11E;unsigned _T11F;struct Cyc_Yystacktype*_T120;union Cyc_YYSTYPE*_T121;union Cyc_YYSTYPE*_T122;struct Cyc_List_List*_T123;struct Cyc_Absyn_Decl*_T124;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T125;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T126;struct Cyc_Yystacktype*_T127;struct Cyc_Yystacktype _T128;struct Cyc_Yyltype _T129;unsigned _T12A;struct Cyc_Yystacktype*_T12B;union Cyc_YYSTYPE*_T12C;union Cyc_YYSTYPE*_T12D;struct Cyc_List_List*_T12E;struct Cyc_Absyn_Decl*_T12F;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T130;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T131;struct Cyc_Yystacktype*_T132;struct Cyc_Yystacktype _T133;struct Cyc_Yyltype _T134;unsigned _T135;struct Cyc_Yystacktype*_T136;union Cyc_YYSTYPE*_T137;union Cyc_YYSTYPE*_T138;struct Cyc_Yystacktype*_T139;union Cyc_YYSTYPE*_T13A;union Cyc_YYSTYPE*_T13B;struct _fat_ptr _T13C;struct _fat_ptr _T13D;int _T13E;struct Cyc_Yystacktype*_T13F;union Cyc_YYSTYPE*_T140;union Cyc_YYSTYPE*_T141;struct _fat_ptr _T142;struct _fat_ptr _T143;int _T144;struct Cyc_Yystacktype*_T145;struct Cyc_Yystacktype _T146;struct Cyc_Yyltype _T147;unsigned _T148;unsigned _T149;struct _fat_ptr _T14A;struct _fat_ptr _T14B;struct Cyc_Yystacktype*_T14C;struct Cyc_Yystacktype _T14D;struct Cyc_List_List*_T14E;struct Cyc_Yystacktype*_T14F;union Cyc_YYSTYPE*_T150;union Cyc_YYSTYPE*_T151;struct Cyc_List_List*_T152;struct Cyc_Yystacktype*_T153;union Cyc_YYSTYPE*_T154;union Cyc_YYSTYPE*_T155;struct Cyc_List_List*_T156;struct Cyc_Yystacktype*_T157;union Cyc_YYSTYPE*_T158;union Cyc_YYSTYPE*_T159;struct Cyc_Yystacktype*_T15A;union Cyc_YYSTYPE*_T15B;union Cyc_YYSTYPE*_T15C;struct _tuple30*_T15D;struct Cyc_Yystacktype*_T15E;struct Cyc_Yystacktype _T15F;struct _tuple30*_T160;struct Cyc_Yystacktype*_T161;union Cyc_YYSTYPE*_T162;union Cyc_YYSTYPE*_T163;struct _tuple30*_T164;struct _tuple30*_T165;struct Cyc_Yystacktype*_T166;struct Cyc_Yystacktype _T167;struct Cyc_Yyltype _T168;unsigned _T169;struct Cyc_List_List*_T16A;struct _tuple33*_T16B;struct Cyc_Yystacktype*_T16C;struct Cyc_Yystacktype _T16D;struct Cyc_Yyltype _T16E;unsigned _T16F;struct Cyc_Yystacktype*_T170;union Cyc_YYSTYPE*_T171;union Cyc_YYSTYPE*_T172;struct Cyc_List_List*_T173;struct _tuple33*_T174;struct Cyc_Yystacktype*_T175;struct Cyc_Yystacktype _T176;struct Cyc_Yyltype _T177;unsigned _T178;struct Cyc_Yystacktype*_T179;union Cyc_YYSTYPE*_T17A;union Cyc_YYSTYPE*_T17B;struct Cyc_Yystacktype*_T17C;union Cyc_YYSTYPE*_T17D;union Cyc_YYSTYPE*_T17E;struct Cyc_Yystacktype*_T17F;struct Cyc_Yystacktype _T180;struct Cyc_List_List*_T181;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T182;struct Cyc_Yystacktype*_T183;union Cyc_YYSTYPE*_T184;union Cyc_YYSTYPE*_T185;void*_T186;struct Cyc_Yystacktype*_T187;struct Cyc_Yystacktype _T188;struct Cyc_Yyltype _T189;unsigned _T18A;unsigned _T18B;struct Cyc_Yystacktype*_T18C;struct Cyc_Yystacktype _T18D;struct _RegionHandle*_T18E;struct Cyc_Yystacktype*_T18F;union Cyc_YYSTYPE*_T190;union Cyc_YYSTYPE*_T191;struct Cyc_Parse_Declarator _T192;struct Cyc_Yystacktype*_T193;union Cyc_YYSTYPE*_T194;union Cyc_YYSTYPE*_T195;struct Cyc_Absyn_Stmt*_T196;struct Cyc_Yystacktype*_T197;struct Cyc_Yystacktype _T198;struct Cyc_Yyltype _T199;unsigned _T19A;unsigned _T19B;struct Cyc_Absyn_Fndecl*_T19C;struct Cyc_Yystacktype*_T19D;union Cyc_YYSTYPE*_T19E;union Cyc_YYSTYPE*_T19F;struct _RegionHandle*_T1A0;struct Cyc_Parse_Declaration_spec*_T1A1;struct Cyc_Parse_Declaration_spec*_T1A2;struct Cyc_Yystacktype*_T1A3;union Cyc_YYSTYPE*_T1A4;union Cyc_YYSTYPE*_T1A5;struct Cyc_Parse_Declarator _T1A6;struct Cyc_Yystacktype*_T1A7;union Cyc_YYSTYPE*_T1A8;union Cyc_YYSTYPE*_T1A9;struct Cyc_Absyn_Stmt*_T1AA;struct Cyc_Yystacktype*_T1AB;struct Cyc_Yystacktype _T1AC;struct Cyc_Yyltype _T1AD;unsigned _T1AE;unsigned _T1AF;struct Cyc_Absyn_Fndecl*_T1B0;struct _RegionHandle*_T1B1;struct Cyc_Yystacktype*_T1B2;union Cyc_YYSTYPE*_T1B3;union Cyc_YYSTYPE*_T1B4;struct Cyc_Parse_Declarator _T1B5;struct Cyc_Yystacktype*_T1B6;union Cyc_YYSTYPE*_T1B7;union Cyc_YYSTYPE*_T1B8;struct Cyc_List_List*_T1B9;struct Cyc_Yystacktype*_T1BA;union Cyc_YYSTYPE*_T1BB;union Cyc_YYSTYPE*_T1BC;struct Cyc_Absyn_Stmt*_T1BD;struct Cyc_Yystacktype*_T1BE;struct Cyc_Yystacktype _T1BF;struct Cyc_Yyltype _T1C0;unsigned _T1C1;unsigned _T1C2;struct Cyc_Absyn_Fndecl*_T1C3;struct Cyc_Yystacktype*_T1C4;union Cyc_YYSTYPE*_T1C5;union Cyc_YYSTYPE*_T1C6;struct _RegionHandle*_T1C7;struct Cyc_Parse_Declaration_spec*_T1C8;struct Cyc_Parse_Declaration_spec*_T1C9;struct Cyc_Yystacktype*_T1CA;union Cyc_YYSTYPE*_T1CB;union Cyc_YYSTYPE*_T1CC;struct Cyc_Parse_Declarator _T1CD;struct Cyc_Yystacktype*_T1CE;union Cyc_YYSTYPE*_T1CF;union Cyc_YYSTYPE*_T1D0;struct Cyc_List_List*_T1D1;struct Cyc_Yystacktype*_T1D2;union Cyc_YYSTYPE*_T1D3;union Cyc_YYSTYPE*_T1D4;struct Cyc_Absyn_Stmt*_T1D5;struct Cyc_Yystacktype*_T1D6;struct Cyc_Yystacktype _T1D7;struct Cyc_Yyltype _T1D8;unsigned _T1D9;unsigned _T1DA;struct Cyc_Absyn_Fndecl*_T1DB;struct Cyc_Yystacktype*_T1DC;union Cyc_YYSTYPE*_T1DD;union Cyc_YYSTYPE*_T1DE;struct _RegionHandle*_T1DF;struct Cyc_Parse_Declaration_spec*_T1E0;struct Cyc_Parse_Declaration_spec*_T1E1;struct Cyc_Yystacktype*_T1E2;union Cyc_YYSTYPE*_T1E3;union Cyc_YYSTYPE*_T1E4;struct Cyc_Parse_Declarator _T1E5;struct Cyc_Yystacktype*_T1E6;union Cyc_YYSTYPE*_T1E7;union Cyc_YYSTYPE*_T1E8;struct Cyc_Absyn_Stmt*_T1E9;struct Cyc_Yystacktype*_T1EA;struct Cyc_Yystacktype _T1EB;struct Cyc_Yyltype _T1EC;unsigned _T1ED;unsigned _T1EE;struct Cyc_Absyn_Fndecl*_T1EF;struct Cyc_Yystacktype*_T1F0;union Cyc_YYSTYPE*_T1F1;union Cyc_YYSTYPE*_T1F2;struct _RegionHandle*_T1F3;struct Cyc_Parse_Declaration_spec*_T1F4;struct Cyc_Parse_Declaration_spec*_T1F5;struct Cyc_Yystacktype*_T1F6;union Cyc_YYSTYPE*_T1F7;union Cyc_YYSTYPE*_T1F8;struct Cyc_Parse_Declarator _T1F9;struct Cyc_Yystacktype*_T1FA;union Cyc_YYSTYPE*_T1FB;union Cyc_YYSTYPE*_T1FC;struct Cyc_List_List*_T1FD;struct Cyc_Yystacktype*_T1FE;union Cyc_YYSTYPE*_T1FF;union Cyc_YYSTYPE*_T200;struct Cyc_Absyn_Stmt*_T201;struct Cyc_Yystacktype*_T202;struct Cyc_Yystacktype _T203;struct Cyc_Yyltype _T204;unsigned _T205;unsigned _T206;struct Cyc_Absyn_Fndecl*_T207;struct Cyc_Yystacktype*_T208;union Cyc_YYSTYPE*_T209;union Cyc_YYSTYPE*_T20A;struct _tuple0*_T20B;struct Cyc_Yystacktype*_T20C;struct Cyc_Yystacktype _T20D;struct _fat_ptr*_T20E;struct Cyc_Yystacktype*_T20F;union Cyc_YYSTYPE*_T210;union Cyc_YYSTYPE*_T211;struct Cyc_Yystacktype*_T212;struct Cyc_Yystacktype _T213;struct Cyc_Yystacktype*_T214;union Cyc_YYSTYPE*_T215;union Cyc_YYSTYPE*_T216;struct Cyc_Parse_Declaration_spec _T217;struct Cyc_Yystacktype*_T218;struct Cyc_Yystacktype _T219;struct Cyc_Yyltype _T21A;unsigned _T21B;unsigned _T21C;struct Cyc_Yystacktype*_T21D;struct Cyc_Yystacktype _T21E;struct Cyc_Yyltype _T21F;unsigned _T220;unsigned _T221;struct Cyc_List_List*_T222;struct Cyc_Yystacktype*_T223;union Cyc_YYSTYPE*_T224;union Cyc_YYSTYPE*_T225;struct Cyc_Parse_Declaration_spec _T226;struct _tuple11*(*_T227)(struct _tuple11*);struct Cyc_Yystacktype*_T228;union Cyc_YYSTYPE*_T229;union Cyc_YYSTYPE*_T22A;struct _tuple11*_T22B;struct _tuple11*_T22C;struct Cyc_Yystacktype*_T22D;struct Cyc_Yystacktype _T22E;struct Cyc_Yyltype _T22F;unsigned _T230;unsigned _T231;struct Cyc_Yystacktype*_T232;struct Cyc_Yystacktype _T233;struct Cyc_Yyltype _T234;unsigned _T235;unsigned _T236;struct Cyc_List_List*_T237;struct Cyc_List_List*_T238;struct Cyc_Yystacktype*_T239;union Cyc_YYSTYPE*_T23A;union Cyc_YYSTYPE*_T23B;struct Cyc_Absyn_Pat*_T23C;struct Cyc_Yystacktype*_T23D;union Cyc_YYSTYPE*_T23E;union Cyc_YYSTYPE*_T23F;struct Cyc_Absyn_Exp*_T240;struct Cyc_Yystacktype*_T241;struct Cyc_Yystacktype _T242;struct Cyc_Yyltype _T243;unsigned _T244;unsigned _T245;struct Cyc_Yystacktype*_T246;union Cyc_YYSTYPE*_T247;union Cyc_YYSTYPE*_T248;struct _tuple0*_T249;struct _tuple0*_T24A;struct Cyc_List_List*_T24B;void*_T24C;struct Cyc_List_List*_T24D;struct _tuple0*_T24E;void*_T24F;struct Cyc_List_List*_T250;struct Cyc_List_List*_T251;struct Cyc_List_List*_T252;struct Cyc_Yystacktype*_T253;struct Cyc_Yystacktype _T254;struct Cyc_Yyltype _T255;unsigned _T256;unsigned _T257;struct Cyc_Yystacktype*_T258;union Cyc_YYSTYPE*_T259;union Cyc_YYSTYPE*_T25A;struct _fat_ptr _T25B;struct Cyc_Yystacktype*_T25C;struct Cyc_Yystacktype _T25D;struct Cyc_Yyltype _T25E;unsigned _T25F;unsigned _T260;struct Cyc_Absyn_Tvar*_T261;struct _fat_ptr*_T262;struct Cyc_Yystacktype*_T263;union Cyc_YYSTYPE*_T264;union Cyc_YYSTYPE*_T265;struct Cyc_Absyn_Tvar*_T266;struct Cyc_Absyn_Tvar*_T267;struct Cyc_Absyn_Kind*_T268;struct Cyc_Absyn_Kind*_T269;struct Cyc_Absyn_Tvar*_T26A;struct Cyc_Yystacktype*_T26B;struct Cyc_Yystacktype _T26C;struct Cyc_Yyltype _T26D;unsigned _T26E;unsigned _T26F;struct _tuple0*_T270;struct _fat_ptr*_T271;struct Cyc_Yystacktype*_T272;union Cyc_YYSTYPE*_T273;union Cyc_YYSTYPE*_T274;void*_T275;struct Cyc_List_List*_T276;struct Cyc_Absyn_Tvar*_T277;struct Cyc_Absyn_Vardecl*_T278;struct Cyc_Yystacktype*_T279;struct Cyc_Yystacktype _T27A;struct Cyc_Yyltype _T27B;unsigned _T27C;unsigned _T27D;struct _fat_ptr _T27E;struct Cyc_String_pa_PrintArg_struct _T27F;struct Cyc_Yystacktype*_T280;union Cyc_YYSTYPE*_T281;union Cyc_YYSTYPE*_T282;struct _fat_ptr _T283;struct _fat_ptr _T284;struct Cyc_Yystacktype*_T285;union Cyc_YYSTYPE*_T286;union Cyc_YYSTYPE*_T287;struct _fat_ptr _T288;struct Cyc_Yystacktype*_T289;struct Cyc_Yystacktype _T28A;struct Cyc_Yyltype _T28B;unsigned _T28C;unsigned _T28D;struct Cyc_Absyn_Tvar*_T28E;struct _fat_ptr*_T28F;struct Cyc_Absyn_Tvar*_T290;struct Cyc_Absyn_Tvar*_T291;struct Cyc_Absyn_Kind*_T292;struct Cyc_Absyn_Kind*_T293;struct Cyc_Absyn_Tvar*_T294;struct Cyc_Yystacktype*_T295;struct Cyc_Yystacktype _T296;struct Cyc_Yyltype _T297;unsigned _T298;unsigned _T299;struct _tuple0*_T29A;struct _fat_ptr*_T29B;struct Cyc_Yystacktype*_T29C;union Cyc_YYSTYPE*_T29D;union Cyc_YYSTYPE*_T29E;void*_T29F;struct Cyc_List_List*_T2A0;struct Cyc_Absyn_Tvar*_T2A1;struct Cyc_Absyn_Vardecl*_T2A2;struct Cyc_Yystacktype*_T2A3;union Cyc_YYSTYPE*_T2A4;union Cyc_YYSTYPE*_T2A5;struct Cyc_Absyn_Exp*_T2A6;struct Cyc_Yystacktype*_T2A7;struct Cyc_Yystacktype _T2A8;struct Cyc_Yyltype _T2A9;unsigned _T2AA;unsigned _T2AB;struct Cyc_Yystacktype*_T2AC;union Cyc_YYSTYPE*_T2AD;union Cyc_YYSTYPE*_T2AE;struct _fat_ptr _T2AF;struct _fat_ptr _T2B0;int _T2B1;struct Cyc_Yystacktype*_T2B2;struct Cyc_Yystacktype _T2B3;struct Cyc_Yyltype _T2B4;unsigned _T2B5;unsigned _T2B6;struct _fat_ptr _T2B7;struct _fat_ptr _T2B8;struct Cyc_Yystacktype*_T2B9;union Cyc_YYSTYPE*_T2BA;union Cyc_YYSTYPE*_T2BB;struct Cyc_Absyn_Exp*_T2BC;struct Cyc_Yystacktype*_T2BD;struct Cyc_Yystacktype _T2BE;struct Cyc_Yystacktype*_T2BF;union Cyc_YYSTYPE*_T2C0;union Cyc_YYSTYPE*_T2C1;struct Cyc_List_List*_T2C2;struct Cyc_Yystacktype*_T2C3;union Cyc_YYSTYPE*_T2C4;union Cyc_YYSTYPE*_T2C5;struct Cyc_List_List*_T2C6;struct Cyc_List_List*_T2C7;struct Cyc_Parse_Declaration_spec _T2C8;struct Cyc_Yystacktype*_T2C9;union Cyc_YYSTYPE*_T2CA;union Cyc_YYSTYPE*_T2CB;struct Cyc_Yystacktype*_T2CC;struct Cyc_Yystacktype _T2CD;struct Cyc_Yyltype _T2CE;unsigned _T2CF;unsigned _T2D0;struct Cyc_Yystacktype*_T2D1;union Cyc_YYSTYPE*_T2D2;union Cyc_YYSTYPE*_T2D3;struct Cyc_Parse_Declaration_spec _T2D4;enum Cyc_Parse_Storage_class _T2D5;int _T2D6;struct Cyc_Yystacktype*_T2D7;struct Cyc_Yystacktype _T2D8;struct Cyc_Yyltype _T2D9;unsigned _T2DA;unsigned _T2DB;struct _fat_ptr _T2DC;struct _fat_ptr _T2DD;struct Cyc_Parse_Declaration_spec _T2DE;struct Cyc_Yystacktype*_T2DF;union Cyc_YYSTYPE*_T2E0;union Cyc_YYSTYPE*_T2E1;struct Cyc_Parse_Declaration_spec _T2E2;struct Cyc_Parse_Declaration_spec _T2E3;struct Cyc_Parse_Declaration_spec _T2E4;struct Cyc_Parse_Declaration_spec _T2E5;struct Cyc_Yystacktype*_T2E6;struct Cyc_Yystacktype _T2E7;struct Cyc_Yyltype _T2E8;unsigned _T2E9;unsigned _T2EA;struct _fat_ptr _T2EB;struct _fat_ptr _T2EC;struct Cyc_Yystacktype*_T2ED;struct Cyc_Yystacktype _T2EE;struct Cyc_Parse_Declaration_spec _T2EF;struct Cyc_Yystacktype*_T2F0;struct Cyc_Yystacktype _T2F1;struct Cyc_Yyltype _T2F2;unsigned _T2F3;unsigned _T2F4;struct Cyc_Yystacktype*_T2F5;union Cyc_YYSTYPE*_T2F6;union Cyc_YYSTYPE*_T2F7;struct Cyc_Yystacktype*_T2F8;union Cyc_YYSTYPE*_T2F9;union Cyc_YYSTYPE*_T2FA;struct Cyc_Parse_Declaration_spec _T2FB;struct Cyc_Parse_Declaration_spec _T2FC;struct Cyc_Parse_Declaration_spec _T2FD;struct Cyc_Yystacktype*_T2FE;struct Cyc_Yystacktype _T2FF;struct Cyc_Yyltype _T300;unsigned _T301;unsigned _T302;struct Cyc_Parse_Declaration_spec _T303;struct Cyc_Parse_Type_specifier _T304;struct Cyc_Yystacktype*_T305;union Cyc_YYSTYPE*_T306;union Cyc_YYSTYPE*_T307;struct Cyc_Parse_Type_specifier _T308;struct Cyc_Parse_Declaration_spec _T309;struct Cyc_Parse_Declaration_spec _T30A;struct Cyc_Parse_Declaration_spec _T30B;struct Cyc_Yystacktype*_T30C;union Cyc_YYSTYPE*_T30D;union Cyc_YYSTYPE*_T30E;struct Cyc_Yystacktype*_T30F;union Cyc_YYSTYPE*_T310;union Cyc_YYSTYPE*_T311;struct Cyc_Parse_Declaration_spec _T312;struct Cyc_Parse_Declaration_spec _T313;struct Cyc_Yystacktype*_T314;union Cyc_YYSTYPE*_T315;union Cyc_YYSTYPE*_T316;struct Cyc_Absyn_Tqual _T317;struct Cyc_Parse_Declaration_spec _T318;struct Cyc_Absyn_Tqual _T319;struct Cyc_Parse_Declaration_spec _T31A;struct Cyc_Parse_Declaration_spec _T31B;struct Cyc_Parse_Declaration_spec _T31C;struct Cyc_Parse_Declaration_spec _T31D;struct Cyc_Yystacktype*_T31E;struct Cyc_Yystacktype _T31F;struct Cyc_Yyltype _T320;unsigned _T321;unsigned _T322;struct Cyc_Yystacktype*_T323;union Cyc_YYSTYPE*_T324;union Cyc_YYSTYPE*_T325;struct Cyc_Parse_Declaration_spec _T326;struct Cyc_Parse_Declaration_spec _T327;struct Cyc_Parse_Declaration_spec _T328;struct Cyc_Parse_Declaration_spec _T329;struct Cyc_Parse_Declaration_spec _T32A;struct Cyc_Parse_Declaration_spec _T32B;struct Cyc_Yystacktype*_T32C;struct Cyc_Yystacktype _T32D;struct Cyc_Yyltype _T32E;unsigned _T32F;unsigned _T330;struct Cyc_Yystacktype*_T331;union Cyc_YYSTYPE*_T332;union Cyc_YYSTYPE*_T333;struct Cyc_Yystacktype*_T334;union Cyc_YYSTYPE*_T335;union Cyc_YYSTYPE*_T336;struct Cyc_Parse_Declaration_spec _T337;struct Cyc_Parse_Declaration_spec _T338;struct Cyc_Parse_Declaration_spec _T339;struct Cyc_Parse_Declaration_spec _T33A;struct Cyc_Parse_Declaration_spec _T33B;struct Cyc_Yystacktype*_T33C;union Cyc_YYSTYPE*_T33D;union Cyc_YYSTYPE*_T33E;struct Cyc_List_List*_T33F;struct Cyc_Parse_Declaration_spec _T340;struct Cyc_List_List*_T341;struct Cyc_Yystacktype*_T342;union Cyc_YYSTYPE*_T343;union Cyc_YYSTYPE*_T344;struct _fat_ptr _T345;struct _fat_ptr _T346;int _T347;struct Cyc_Yystacktype*_T348;struct Cyc_Yystacktype _T349;struct Cyc_Yyltype _T34A;unsigned _T34B;unsigned _T34C;struct _fat_ptr _T34D;struct _fat_ptr _T34E;struct Cyc_Yystacktype*_T34F;struct Cyc_Yystacktype _T350;struct Cyc_Yystacktype*_T351;struct Cyc_Yystacktype _T352;struct Cyc_List_List*_T353;struct Cyc_Yystacktype*_T354;union Cyc_YYSTYPE*_T355;union Cyc_YYSTYPE*_T356;struct Cyc_List_List*_T357;struct Cyc_Yystacktype*_T358;union Cyc_YYSTYPE*_T359;union Cyc_YYSTYPE*_T35A;struct Cyc_Yystacktype*_T35B;union Cyc_YYSTYPE*_T35C;union Cyc_YYSTYPE*_T35D;struct Cyc_Yystacktype*_T35E;struct Cyc_Yystacktype _T35F;struct Cyc_Yyltype _T360;unsigned _T361;unsigned _T362;struct Cyc_Yystacktype*_T363;union Cyc_YYSTYPE*_T364;union Cyc_YYSTYPE*_T365;struct _fat_ptr _T366;void*_T367;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T368;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T369;void*_T36A;struct Cyc_Yystacktype*_T36B;struct Cyc_Yystacktype _T36C;struct Cyc_Yyltype _T36D;unsigned _T36E;unsigned _T36F;struct Cyc_Yystacktype*_T370;union Cyc_YYSTYPE*_T371;union Cyc_YYSTYPE*_T372;struct _fat_ptr _T373;struct Cyc_Yystacktype*_T374;struct Cyc_Yystacktype _T375;struct Cyc_Yyltype _T376;unsigned _T377;unsigned _T378;struct Cyc_Yystacktype*_T379;union Cyc_YYSTYPE*_T37A;union Cyc_YYSTYPE*_T37B;struct Cyc_Absyn_Exp*_T37C;void*_T37D;struct Cyc_Yystacktype*_T37E;struct Cyc_Yystacktype _T37F;struct Cyc_Yyltype _T380;unsigned _T381;unsigned _T382;struct Cyc_Yystacktype*_T383;struct Cyc_Yystacktype _T384;struct Cyc_Yyltype _T385;unsigned _T386;unsigned _T387;struct Cyc_Yystacktype*_T388;union Cyc_YYSTYPE*_T389;union Cyc_YYSTYPE*_T38A;struct _fat_ptr _T38B;struct Cyc_Yystacktype*_T38C;union Cyc_YYSTYPE*_T38D;union Cyc_YYSTYPE*_T38E;struct _fat_ptr _T38F;struct Cyc_Yystacktype*_T390;struct Cyc_Yystacktype _T391;struct Cyc_Yyltype _T392;unsigned _T393;unsigned _T394;struct Cyc_Yystacktype*_T395;union Cyc_YYSTYPE*_T396;union Cyc_YYSTYPE*_T397;union Cyc_Absyn_Cnst _T398;unsigned _T399;struct Cyc_Yystacktype*_T39A;struct Cyc_Yystacktype _T39B;struct Cyc_Yyltype _T39C;unsigned _T39D;unsigned _T39E;struct Cyc_Yystacktype*_T39F;union Cyc_YYSTYPE*_T3A0;union Cyc_YYSTYPE*_T3A1;union Cyc_Absyn_Cnst _T3A2;unsigned _T3A3;void*_T3A4;struct Cyc_Yystacktype*_T3A5;struct Cyc_Yystacktype _T3A6;struct Cyc_Yystacktype*_T3A7;union Cyc_YYSTYPE*_T3A8;union Cyc_YYSTYPE*_T3A9;struct _tuple0*_T3AA;struct Cyc_Yystacktype*_T3AB;union Cyc_YYSTYPE*_T3AC;union Cyc_YYSTYPE*_T3AD;struct Cyc_List_List*_T3AE;void*_T3AF;struct Cyc_Yystacktype*_T3B0;struct Cyc_Yystacktype _T3B1;struct Cyc_Yyltype _T3B2;unsigned _T3B3;unsigned _T3B4;struct Cyc_Parse_Type_specifier _T3B5;void*_T3B6;struct Cyc_Yystacktype*_T3B7;struct Cyc_Yystacktype _T3B8;struct Cyc_Yyltype _T3B9;unsigned _T3BA;unsigned _T3BB;struct Cyc_Parse_Type_specifier _T3BC;void*_T3BD;struct Cyc_Yystacktype*_T3BE;struct Cyc_Yystacktype _T3BF;struct Cyc_Yyltype _T3C0;unsigned _T3C1;unsigned _T3C2;struct Cyc_Parse_Type_specifier _T3C3;struct Cyc_Yystacktype*_T3C4;struct Cyc_Yystacktype _T3C5;struct Cyc_Yyltype _T3C6;unsigned _T3C7;unsigned _T3C8;struct Cyc_Parse_Type_specifier _T3C9;void*_T3CA;struct Cyc_Yystacktype*_T3CB;struct Cyc_Yystacktype _T3CC;struct Cyc_Yyltype _T3CD;unsigned _T3CE;unsigned _T3CF;struct Cyc_Parse_Type_specifier _T3D0;struct Cyc_Yystacktype*_T3D1;struct Cyc_Yystacktype _T3D2;struct Cyc_Yyltype _T3D3;unsigned _T3D4;unsigned _T3D5;struct Cyc_Parse_Type_specifier _T3D6;void*_T3D7;struct Cyc_Yystacktype*_T3D8;struct Cyc_Yystacktype _T3D9;struct Cyc_Yyltype _T3DA;unsigned _T3DB;unsigned _T3DC;struct Cyc_Parse_Type_specifier _T3DD;void*_T3DE;struct Cyc_Yystacktype*_T3DF;struct Cyc_Yystacktype _T3E0;struct Cyc_Yyltype _T3E1;unsigned _T3E2;unsigned _T3E3;struct Cyc_Parse_Type_specifier _T3E4;struct Cyc_Yystacktype*_T3E5;struct Cyc_Yystacktype _T3E6;struct Cyc_Yyltype _T3E7;unsigned _T3E8;unsigned _T3E9;struct Cyc_Parse_Type_specifier _T3EA;struct Cyc_Yystacktype*_T3EB;struct Cyc_Yystacktype _T3EC;struct Cyc_Yyltype _T3ED;unsigned _T3EE;unsigned _T3EF;struct Cyc_Parse_Type_specifier _T3F0;struct Cyc_Yystacktype*_T3F1;struct Cyc_Yystacktype _T3F2;struct Cyc_Yyltype _T3F3;unsigned _T3F4;unsigned _T3F5;struct Cyc_Parse_Type_specifier _T3F6;struct Cyc_Yystacktype*_T3F7;struct Cyc_Yystacktype _T3F8;struct Cyc_Yystacktype*_T3F9;struct Cyc_Yystacktype _T3FA;struct Cyc_Yystacktype*_T3FB;union Cyc_YYSTYPE*_T3FC;union Cyc_YYSTYPE*_T3FD;struct Cyc_Absyn_Exp*_T3FE;void*_T3FF;struct Cyc_Yystacktype*_T400;struct Cyc_Yystacktype _T401;struct Cyc_Yyltype _T402;unsigned _T403;unsigned _T404;struct Cyc_Parse_Type_specifier _T405;struct _fat_ptr _T406;struct Cyc_Absyn_Kind*_T407;struct Cyc_Absyn_Kind*_T408;void*_T409;struct Cyc_Yystacktype*_T40A;struct Cyc_Yystacktype _T40B;struct Cyc_Yyltype _T40C;unsigned _T40D;unsigned _T40E;struct Cyc_Parse_Type_specifier _T40F;struct Cyc_Yystacktype*_T410;struct Cyc_Yystacktype _T411;struct Cyc_Yystacktype*_T412;union Cyc_YYSTYPE*_T413;union Cyc_YYSTYPE*_T414;void*_T415;struct Cyc_Yystacktype*_T416;struct Cyc_Yystacktype _T417;struct Cyc_Yyltype _T418;unsigned _T419;unsigned _T41A;struct Cyc_Parse_Type_specifier _T41B;void*_T41C;struct Cyc_Yystacktype*_T41D;struct Cyc_Yystacktype _T41E;struct Cyc_Yyltype _T41F;unsigned _T420;unsigned _T421;struct Cyc_Parse_Type_specifier _T422;struct Cyc_Yystacktype*_T423;union Cyc_YYSTYPE*_T424;union Cyc_YYSTYPE*_T425;struct Cyc_Absyn_Kind*_T426;struct Cyc_Core_Opt*_T427;void*_T428;struct Cyc_Yystacktype*_T429;struct Cyc_Yystacktype _T42A;struct Cyc_Yyltype _T42B;unsigned _T42C;unsigned _T42D;struct Cyc_Parse_Type_specifier _T42E;struct Cyc_List_List*(*_T42F)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T430)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T431)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T432;struct Cyc_Yystacktype _T433;struct Cyc_Yyltype _T434;unsigned _T435;unsigned _T436;struct Cyc_Yystacktype*_T437;union Cyc_YYSTYPE*_T438;union Cyc_YYSTYPE*_T439;struct Cyc_List_List*_T43A;struct Cyc_List_List*_T43B;struct Cyc_List_List*_T43C;void*_T43D;struct Cyc_Yystacktype*_T43E;struct Cyc_Yystacktype _T43F;struct Cyc_Yyltype _T440;unsigned _T441;unsigned _T442;struct Cyc_Parse_Type_specifier _T443;struct Cyc_Yystacktype*_T444;union Cyc_YYSTYPE*_T445;union Cyc_YYSTYPE*_T446;void*_T447;void*_T448;struct Cyc_Yystacktype*_T449;struct Cyc_Yystacktype _T44A;struct Cyc_Yyltype _T44B;unsigned _T44C;unsigned _T44D;struct Cyc_Parse_Type_specifier _T44E;struct Cyc_Core_Opt*_T44F;struct Cyc_Core_Opt*_T450;void*_T451;void*_T452;struct Cyc_Yystacktype*_T453;struct Cyc_Yystacktype _T454;struct Cyc_Yyltype _T455;unsigned _T456;unsigned _T457;struct Cyc_Parse_Type_specifier _T458;struct Cyc_Yystacktype*_T459;union Cyc_YYSTYPE*_T45A;union Cyc_YYSTYPE*_T45B;void*_T45C;void*_T45D;struct Cyc_Yystacktype*_T45E;struct Cyc_Yystacktype _T45F;struct Cyc_Yyltype _T460;unsigned _T461;unsigned _T462;struct Cyc_Parse_Type_specifier _T463;struct Cyc_Yystacktype*_T464;union Cyc_YYSTYPE*_T465;union Cyc_YYSTYPE*_T466;void*_T467;void*_T468;struct Cyc_Yystacktype*_T469;struct Cyc_Yystacktype _T46A;struct Cyc_Yyltype _T46B;unsigned _T46C;unsigned _T46D;struct Cyc_Parse_Type_specifier _T46E;struct Cyc_Core_Opt*_T46F;struct Cyc_Core_Opt*_T470;void*_T471;void*_T472;struct Cyc_Yystacktype*_T473;struct Cyc_Yystacktype _T474;struct Cyc_Yyltype _T475;unsigned _T476;unsigned _T477;struct Cyc_Parse_Type_specifier _T478;struct Cyc_Yystacktype*_T479;union Cyc_YYSTYPE*_T47A;union Cyc_YYSTYPE*_T47B;struct Cyc_Absyn_Exp*_T47C;void*_T47D;struct Cyc_Yystacktype*_T47E;struct Cyc_Yystacktype _T47F;struct Cyc_Yyltype _T480;unsigned _T481;unsigned _T482;struct Cyc_Parse_Type_specifier _T483;struct Cyc_Yystacktype*_T484;union Cyc_YYSTYPE*_T485;union Cyc_YYSTYPE*_T486;struct _fat_ptr _T487;struct Cyc_Yystacktype*_T488;struct Cyc_Yystacktype _T489;struct Cyc_Yyltype _T48A;unsigned _T48B;unsigned _T48C;struct Cyc_Absyn_Kind*_T48D;unsigned _T48E;int _T48F;struct Cyc_Yystacktype*_T490;struct Cyc_Yystacktype _T491;struct Cyc_Yyltype _T492;unsigned _T493;struct Cyc_Absyn_Tqual _T494;struct Cyc_Absyn_Tqual _T495;struct Cyc_Absyn_Tqual _T496;struct Cyc_Absyn_TypeDecl*_T497;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T498;struct Cyc_Absyn_Enumdecl*_T499;struct Cyc_Yystacktype*_T49A;union Cyc_YYSTYPE*_T49B;union Cyc_YYSTYPE*_T49C;struct Cyc_Core_Opt*_T49D;struct Cyc_Yystacktype*_T49E;union Cyc_YYSTYPE*_T49F;union Cyc_YYSTYPE*_T4A0;struct Cyc_Absyn_TypeDecl*_T4A1;struct Cyc_Yystacktype*_T4A2;struct Cyc_Yystacktype _T4A3;struct Cyc_Yyltype _T4A4;unsigned _T4A5;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T4A6;void*_T4A7;struct Cyc_Yystacktype*_T4A8;struct Cyc_Yystacktype _T4A9;struct Cyc_Yyltype _T4AA;unsigned _T4AB;unsigned _T4AC;struct Cyc_Parse_Type_specifier _T4AD;struct Cyc_Yystacktype*_T4AE;union Cyc_YYSTYPE*_T4AF;union Cyc_YYSTYPE*_T4B0;struct _tuple0*_T4B1;void*_T4B2;struct Cyc_Yystacktype*_T4B3;struct Cyc_Yystacktype _T4B4;struct Cyc_Yyltype _T4B5;unsigned _T4B6;unsigned _T4B7;struct Cyc_Parse_Type_specifier _T4B8;struct Cyc_Yystacktype*_T4B9;union Cyc_YYSTYPE*_T4BA;union Cyc_YYSTYPE*_T4BB;struct Cyc_List_List*_T4BC;void*_T4BD;struct Cyc_Yystacktype*_T4BE;struct Cyc_Yystacktype _T4BF;struct Cyc_Yyltype _T4C0;unsigned _T4C1;unsigned _T4C2;struct Cyc_Parse_Type_specifier _T4C3;struct Cyc_Absyn_Enumfield*_T4C4;struct Cyc_Yystacktype*_T4C5;union Cyc_YYSTYPE*_T4C6;union Cyc_YYSTYPE*_T4C7;struct Cyc_Yystacktype*_T4C8;struct Cyc_Yystacktype _T4C9;struct Cyc_Yyltype _T4CA;unsigned _T4CB;struct Cyc_Absyn_Enumfield*_T4CC;struct Cyc_Yystacktype*_T4CD;union Cyc_YYSTYPE*_T4CE;union Cyc_YYSTYPE*_T4CF;struct Cyc_Yystacktype*_T4D0;union Cyc_YYSTYPE*_T4D1;union Cyc_YYSTYPE*_T4D2;struct Cyc_Yystacktype*_T4D3;struct Cyc_Yystacktype _T4D4;struct Cyc_Yyltype _T4D5;unsigned _T4D6;struct Cyc_List_List*_T4D7;struct Cyc_Yystacktype*_T4D8;union Cyc_YYSTYPE*_T4D9;union Cyc_YYSTYPE*_T4DA;struct Cyc_List_List*_T4DB;struct Cyc_Yystacktype*_T4DC;union Cyc_YYSTYPE*_T4DD;union Cyc_YYSTYPE*_T4DE;struct Cyc_List_List*_T4DF;struct Cyc_Yystacktype*_T4E0;union Cyc_YYSTYPE*_T4E1;union Cyc_YYSTYPE*_T4E2;struct Cyc_Yystacktype*_T4E3;union Cyc_YYSTYPE*_T4E4;union Cyc_YYSTYPE*_T4E5;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T4E6;struct Cyc_Yystacktype*_T4E7;union Cyc_YYSTYPE*_T4E8;union Cyc_YYSTYPE*_T4E9;struct Cyc_Yystacktype*_T4EA;union Cyc_YYSTYPE*_T4EB;union Cyc_YYSTYPE*_T4EC;void*_T4ED;struct Cyc_Yystacktype*_T4EE;struct Cyc_Yystacktype _T4EF;struct Cyc_Yyltype _T4F0;unsigned _T4F1;unsigned _T4F2;struct Cyc_Parse_Type_specifier _T4F3;struct Cyc_List_List*(*_T4F4)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T4F5)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T4F6;struct Cyc_Yystacktype _T4F7;struct Cyc_Yyltype _T4F8;unsigned _T4F9;unsigned _T4FA;struct Cyc_Yystacktype*_T4FB;union Cyc_YYSTYPE*_T4FC;union Cyc_YYSTYPE*_T4FD;struct Cyc_List_List*_T4FE;struct Cyc_List_List*(*_T4FF)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T500)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T501;struct Cyc_Yystacktype _T502;struct Cyc_Yyltype _T503;unsigned _T504;unsigned _T505;struct Cyc_Yystacktype*_T506;union Cyc_YYSTYPE*_T507;union Cyc_YYSTYPE*_T508;struct Cyc_List_List*_T509;struct Cyc_Yystacktype*_T50A;union Cyc_YYSTYPE*_T50B;union Cyc_YYSTYPE*_T50C;struct _tuple28 _T50D;struct _tuple28*_T50E;unsigned _T50F;struct _tuple28*_T510;struct _tuple28 _T511;struct Cyc_Yystacktype*_T512;union Cyc_YYSTYPE*_T513;union Cyc_YYSTYPE*_T514;struct _tuple25 _T515;enum Cyc_Absyn_AggrKind _T516;struct Cyc_Yystacktype*_T517;union Cyc_YYSTYPE*_T518;union Cyc_YYSTYPE*_T519;struct _tuple0*_T51A;struct Cyc_List_List*_T51B;struct Cyc_List_List*_T51C;struct Cyc_List_List*_T51D;struct Cyc_List_List*_T51E;struct Cyc_Yystacktype*_T51F;union Cyc_YYSTYPE*_T520;union Cyc_YYSTYPE*_T521;struct Cyc_List_List*_T522;struct Cyc_Yystacktype*_T523;union Cyc_YYSTYPE*_T524;union Cyc_YYSTYPE*_T525;struct _tuple25 _T526;int _T527;struct Cyc_Absyn_AggrdeclImpl*_T528;struct Cyc_Yystacktype*_T529;struct Cyc_Yystacktype _T52A;struct Cyc_Yyltype _T52B;unsigned _T52C;unsigned _T52D;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T52E;void*_T52F;struct Cyc_Yystacktype*_T530;struct Cyc_Yystacktype _T531;struct Cyc_Yyltype _T532;unsigned _T533;unsigned _T534;struct Cyc_Parse_Type_specifier _T535;struct Cyc_Yystacktype*_T536;union Cyc_YYSTYPE*_T537;union Cyc_YYSTYPE*_T538;struct _tuple25 _T539;enum Cyc_Absyn_AggrKind _T53A;struct Cyc_Yystacktype*_T53B;union Cyc_YYSTYPE*_T53C;union Cyc_YYSTYPE*_T53D;struct _tuple0*_T53E;struct Cyc_Core_Opt*_T53F;struct Cyc_Yystacktype*_T540;union Cyc_YYSTYPE*_T541;union Cyc_YYSTYPE*_T542;struct _tuple25 _T543;int _T544;struct Cyc_Core_Opt*_T545;union Cyc_Absyn_AggrInfo _T546;struct Cyc_Yystacktype*_T547;union Cyc_YYSTYPE*_T548;union Cyc_YYSTYPE*_T549;struct Cyc_List_List*_T54A;void*_T54B;struct Cyc_Yystacktype*_T54C;struct Cyc_Yystacktype _T54D;struct Cyc_Yyltype _T54E;unsigned _T54F;unsigned _T550;struct Cyc_Parse_Type_specifier _T551;struct _tuple25 _T552;struct Cyc_Yystacktype*_T553;union Cyc_YYSTYPE*_T554;union Cyc_YYSTYPE*_T555;struct _tuple25 _T556;struct Cyc_Yystacktype*_T557;union Cyc_YYSTYPE*_T558;union Cyc_YYSTYPE*_T559;struct Cyc_Yystacktype*_T55A;union Cyc_YYSTYPE*_T55B;union Cyc_YYSTYPE*_T55C;struct Cyc_List_List*_T55D;struct Cyc_List_List*_T55E;struct Cyc_Yystacktype*_T55F;union Cyc_YYSTYPE*_T560;union Cyc_YYSTYPE*_T561;struct Cyc_List_List*_T562;void*_T563;struct Cyc_List_List*_T564;struct Cyc_List_List*_T565;struct Cyc_List_List*_T566;void(*_T567)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*);void(*_T568)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T569;struct Cyc_List_List*_T56A;struct Cyc_List_List*_T56B;struct Cyc_Yystacktype*_T56C;union Cyc_YYSTYPE*_T56D;union Cyc_YYSTYPE*_T56E;struct Cyc_List_List*_T56F;struct Cyc_Yystacktype*_T570;union Cyc_YYSTYPE*_T571;union Cyc_YYSTYPE*_T572;struct Cyc_Yystacktype*_T573;union Cyc_YYSTYPE*_T574;union Cyc_YYSTYPE*_T575;struct _tuple11*_T576;struct _RegionHandle*_T577;struct Cyc_Yystacktype*_T578;union Cyc_YYSTYPE*_T579;union Cyc_YYSTYPE*_T57A;struct _tuple11*_T57B;struct _RegionHandle*_T57C;struct Cyc_Yystacktype*_T57D;union Cyc_YYSTYPE*_T57E;union Cyc_YYSTYPE*_T57F;struct Cyc_Yystacktype*_T580;union Cyc_YYSTYPE*_T581;union Cyc_YYSTYPE*_T582;struct _tuple12 _T583;struct Cyc_Yystacktype*_T584;union Cyc_YYSTYPE*_T585;union Cyc_YYSTYPE*_T586;struct _tuple12 _T587;struct Cyc_Yystacktype*_T588;union Cyc_YYSTYPE*_T589;union Cyc_YYSTYPE*_T58A;struct Cyc_Yystacktype*_T58B;union Cyc_YYSTYPE*_T58C;union Cyc_YYSTYPE*_T58D;void*_T58E;struct Cyc_Yystacktype*_T58F;struct Cyc_Yystacktype _T590;struct Cyc_Yyltype _T591;unsigned _T592;unsigned _T593;struct _tuple12 _T594;struct Cyc_Yystacktype*_T595;union Cyc_YYSTYPE*_T596;union Cyc_YYSTYPE*_T597;struct Cyc_Yystacktype*_T598;union Cyc_YYSTYPE*_T599;union Cyc_YYSTYPE*_T59A;struct _tuple12 _T59B;struct Cyc_Yystacktype*_T59C;union Cyc_YYSTYPE*_T59D;union Cyc_YYSTYPE*_T59E;struct Cyc_Yystacktype*_T59F;union Cyc_YYSTYPE*_T5A0;union Cyc_YYSTYPE*_T5A1;struct Cyc_Yystacktype*_T5A2;union Cyc_YYSTYPE*_T5A3;union Cyc_YYSTYPE*_T5A4;void*_T5A5;struct Cyc_Yystacktype*_T5A6;struct Cyc_Yystacktype _T5A7;struct Cyc_Yyltype _T5A8;unsigned _T5A9;unsigned _T5AA;struct Cyc_Yystacktype*_T5AB;union Cyc_YYSTYPE*_T5AC;union Cyc_YYSTYPE*_T5AD;struct Cyc_Absyn_Tqual _T5AE;unsigned _T5AF;struct Cyc_Yystacktype*_T5B0;struct Cyc_Yystacktype _T5B1;struct Cyc_Yyltype _T5B2;unsigned _T5B3;struct Cyc_Yystacktype*_T5B4;union Cyc_YYSTYPE*_T5B5;union Cyc_YYSTYPE*_T5B6;struct Cyc_List_List*_T5B7;void*_T5B8;struct _tuple13*_T5B9;struct _RegionHandle*_T5BA;struct Cyc_List_List*_T5BB;struct _RegionHandle*_T5BC;struct _tuple16*_T5BD;struct _RegionHandle*_T5BE;struct Cyc_List_List*_T5BF;struct Cyc_Parse_Type_specifier _T5C0;struct Cyc_Yystacktype*_T5C1;struct Cyc_Yystacktype _T5C2;struct Cyc_Yyltype _T5C3;unsigned _T5C4;unsigned _T5C5;struct _RegionHandle*_T5C6;struct _RegionHandle*_T5C7;struct Cyc_List_List*_T5C8;struct Cyc_List_List*_T5C9;struct Cyc_List_List*(*_T5CA)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T5CB)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T5CC;struct Cyc_Yystacktype _T5CD;struct Cyc_Yyltype _T5CE;unsigned _T5CF;unsigned _T5D0;struct Cyc_List_List*_T5D1;struct Cyc_List_List*_T5D2;struct _tuple26 _T5D3;struct Cyc_Yystacktype*_T5D4;struct Cyc_Yystacktype _T5D5;struct Cyc_Yyltype _T5D6;unsigned _T5D7;unsigned _T5D8;struct Cyc_Yystacktype*_T5D9;union Cyc_YYSTYPE*_T5DA;union Cyc_YYSTYPE*_T5DB;struct Cyc_Yystacktype*_T5DC;union Cyc_YYSTYPE*_T5DD;union Cyc_YYSTYPE*_T5DE;struct _tuple26 _T5DF;struct _tuple26 _T5E0;struct Cyc_Yystacktype*_T5E1;struct Cyc_Yystacktype _T5E2;struct Cyc_Yyltype _T5E3;unsigned _T5E4;unsigned _T5E5;struct Cyc_Yystacktype*_T5E6;union Cyc_YYSTYPE*_T5E7;union Cyc_YYSTYPE*_T5E8;struct Cyc_Parse_Type_specifier _T5E9;struct _tuple26 _T5EA;struct Cyc_Parse_Type_specifier _T5EB;struct _tuple26 _T5EC;struct _tuple26 _T5ED;struct Cyc_Yystacktype*_T5EE;union Cyc_YYSTYPE*_T5EF;union Cyc_YYSTYPE*_T5F0;struct Cyc_Yystacktype*_T5F1;union Cyc_YYSTYPE*_T5F2;union Cyc_YYSTYPE*_T5F3;struct _tuple26 _T5F4;struct Cyc_Yystacktype*_T5F5;union Cyc_YYSTYPE*_T5F6;union Cyc_YYSTYPE*_T5F7;struct Cyc_Absyn_Tqual _T5F8;struct _tuple26 _T5F9;struct Cyc_Absyn_Tqual _T5FA;struct _tuple26 _T5FB;struct _tuple26 _T5FC;struct _tuple26 _T5FD;struct Cyc_Yystacktype*_T5FE;struct Cyc_Yystacktype _T5FF;struct Cyc_Yyltype _T600;unsigned _T601;unsigned _T602;struct Cyc_Yystacktype*_T603;union Cyc_YYSTYPE*_T604;union Cyc_YYSTYPE*_T605;struct Cyc_Yystacktype*_T606;union Cyc_YYSTYPE*_T607;union Cyc_YYSTYPE*_T608;struct _tuple26 _T609;struct _tuple26 _T60A;struct _tuple26 _T60B;struct Cyc_Yystacktype*_T60C;union Cyc_YYSTYPE*_T60D;union Cyc_YYSTYPE*_T60E;struct Cyc_List_List*_T60F;struct _tuple26 _T610;struct Cyc_List_List*_T611;struct _tuple26 _T612;struct Cyc_Yystacktype*_T613;struct Cyc_Yystacktype _T614;struct Cyc_Yyltype _T615;unsigned _T616;unsigned _T617;struct Cyc_Yystacktype*_T618;union Cyc_YYSTYPE*_T619;union Cyc_YYSTYPE*_T61A;struct Cyc_Yystacktype*_T61B;union Cyc_YYSTYPE*_T61C;union Cyc_YYSTYPE*_T61D;struct _tuple26 _T61E;struct _tuple26 _T61F;struct Cyc_Yystacktype*_T620;struct Cyc_Yystacktype _T621;struct Cyc_Yyltype _T622;unsigned _T623;unsigned _T624;struct Cyc_Yystacktype*_T625;union Cyc_YYSTYPE*_T626;union Cyc_YYSTYPE*_T627;struct Cyc_Parse_Type_specifier _T628;struct _tuple26 _T629;struct Cyc_Parse_Type_specifier _T62A;struct _tuple26 _T62B;struct _tuple26 _T62C;struct Cyc_Yystacktype*_T62D;union Cyc_YYSTYPE*_T62E;union Cyc_YYSTYPE*_T62F;struct Cyc_Yystacktype*_T630;union Cyc_YYSTYPE*_T631;union Cyc_YYSTYPE*_T632;struct _tuple26 _T633;struct Cyc_Yystacktype*_T634;union Cyc_YYSTYPE*_T635;union Cyc_YYSTYPE*_T636;struct Cyc_Absyn_Tqual _T637;struct _tuple26 _T638;struct Cyc_Absyn_Tqual _T639;struct _tuple26 _T63A;struct _tuple26 _T63B;struct _tuple26 _T63C;struct Cyc_Yystacktype*_T63D;struct Cyc_Yystacktype _T63E;struct Cyc_Yyltype _T63F;unsigned _T640;unsigned _T641;struct Cyc_Yystacktype*_T642;union Cyc_YYSTYPE*_T643;union Cyc_YYSTYPE*_T644;struct Cyc_Yystacktype*_T645;union Cyc_YYSTYPE*_T646;union Cyc_YYSTYPE*_T647;struct _tuple26 _T648;struct _tuple26 _T649;struct _tuple26 _T64A;struct Cyc_Yystacktype*_T64B;union Cyc_YYSTYPE*_T64C;union Cyc_YYSTYPE*_T64D;struct Cyc_List_List*_T64E;struct _tuple26 _T64F;struct Cyc_List_List*_T650;struct Cyc_List_List*_T651;struct _RegionHandle*_T652;struct Cyc_Yystacktype*_T653;union Cyc_YYSTYPE*_T654;union Cyc_YYSTYPE*_T655;struct Cyc_List_List*_T656;struct _RegionHandle*_T657;struct Cyc_Yystacktype*_T658;union Cyc_YYSTYPE*_T659;union Cyc_YYSTYPE*_T65A;struct Cyc_Yystacktype*_T65B;union Cyc_YYSTYPE*_T65C;union Cyc_YYSTYPE*_T65D;struct _tuple12*_T65E;struct _RegionHandle*_T65F;struct Cyc_Yystacktype*_T660;union Cyc_YYSTYPE*_T661;union Cyc_YYSTYPE*_T662;struct Cyc_Yystacktype*_T663;union Cyc_YYSTYPE*_T664;union Cyc_YYSTYPE*_T665;struct _tuple12*_T666;struct _RegionHandle*_T667;struct _tuple0*_T668;struct _fat_ptr*_T669;struct Cyc_Yystacktype*_T66A;union Cyc_YYSTYPE*_T66B;union Cyc_YYSTYPE*_T66C;struct _tuple12*_T66D;struct _RegionHandle*_T66E;struct _tuple0*_T66F;struct _fat_ptr*_T670;struct _tuple12*_T671;struct _RegionHandle*_T672;struct Cyc_Yystacktype*_T673;union Cyc_YYSTYPE*_T674;union Cyc_YYSTYPE*_T675;struct Cyc_Yystacktype*_T676;union Cyc_YYSTYPE*_T677;union Cyc_YYSTYPE*_T678;struct Cyc_Yystacktype*_T679;union Cyc_YYSTYPE*_T67A;union Cyc_YYSTYPE*_T67B;struct Cyc_Absyn_Exp*_T67C;struct Cyc_Yystacktype*_T67D;union Cyc_YYSTYPE*_T67E;union Cyc_YYSTYPE*_T67F;struct Cyc_List_List*(*_T680)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T681)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T682;struct Cyc_Yystacktype _T683;struct Cyc_Yyltype _T684;unsigned _T685;unsigned _T686;struct Cyc_Yystacktype*_T687;union Cyc_YYSTYPE*_T688;union Cyc_YYSTYPE*_T689;struct Cyc_List_List*_T68A;struct Cyc_Yystacktype*_T68B;union Cyc_YYSTYPE*_T68C;union Cyc_YYSTYPE*_T68D;struct _tuple0*_T68E;struct Cyc_List_List*_T68F;struct Cyc_Core_Opt*_T690;struct Cyc_Yystacktype*_T691;union Cyc_YYSTYPE*_T692;union Cyc_YYSTYPE*_T693;int _T694;struct Cyc_Yystacktype*_T695;struct Cyc_Yystacktype _T696;struct Cyc_Yyltype _T697;unsigned _T698;unsigned _T699;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T69A;void*_T69B;struct Cyc_Yystacktype*_T69C;struct Cyc_Yystacktype _T69D;struct Cyc_Yyltype _T69E;unsigned _T69F;unsigned _T6A0;struct Cyc_Parse_Type_specifier _T6A1;struct Cyc_Yystacktype*_T6A2;union Cyc_YYSTYPE*_T6A3;union Cyc_YYSTYPE*_T6A4;struct Cyc_Absyn_UnknownDatatypeInfo _T6A5;struct Cyc_Yystacktype*_T6A6;union Cyc_YYSTYPE*_T6A7;union Cyc_YYSTYPE*_T6A8;union Cyc_Absyn_DatatypeInfo _T6A9;struct Cyc_Yystacktype*_T6AA;union Cyc_YYSTYPE*_T6AB;union Cyc_YYSTYPE*_T6AC;struct Cyc_List_List*_T6AD;void*_T6AE;struct Cyc_Yystacktype*_T6AF;struct Cyc_Yystacktype _T6B0;struct Cyc_Yyltype _T6B1;unsigned _T6B2;unsigned _T6B3;struct Cyc_Parse_Type_specifier _T6B4;struct Cyc_Yystacktype*_T6B5;union Cyc_YYSTYPE*_T6B6;union Cyc_YYSTYPE*_T6B7;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T6B8;struct Cyc_Yystacktype*_T6B9;union Cyc_YYSTYPE*_T6BA;union Cyc_YYSTYPE*_T6BB;struct Cyc_Yystacktype*_T6BC;union Cyc_YYSTYPE*_T6BD;union Cyc_YYSTYPE*_T6BE;union Cyc_Absyn_DatatypeFieldInfo _T6BF;struct Cyc_Yystacktype*_T6C0;union Cyc_YYSTYPE*_T6C1;union Cyc_YYSTYPE*_T6C2;struct Cyc_List_List*_T6C3;void*_T6C4;struct Cyc_Yystacktype*_T6C5;struct Cyc_Yystacktype _T6C6;struct Cyc_Yyltype _T6C7;unsigned _T6C8;unsigned _T6C9;struct Cyc_Parse_Type_specifier _T6CA;struct Cyc_List_List*_T6CB;struct Cyc_Yystacktype*_T6CC;union Cyc_YYSTYPE*_T6CD;union Cyc_YYSTYPE*_T6CE;struct Cyc_List_List*_T6CF;struct Cyc_Yystacktype*_T6D0;union Cyc_YYSTYPE*_T6D1;union Cyc_YYSTYPE*_T6D2;struct Cyc_List_List*_T6D3;struct Cyc_Yystacktype*_T6D4;union Cyc_YYSTYPE*_T6D5;union Cyc_YYSTYPE*_T6D6;struct Cyc_Yystacktype*_T6D7;union Cyc_YYSTYPE*_T6D8;union Cyc_YYSTYPE*_T6D9;struct Cyc_List_List*_T6DA;struct Cyc_Yystacktype*_T6DB;union Cyc_YYSTYPE*_T6DC;union Cyc_YYSTYPE*_T6DD;struct Cyc_Yystacktype*_T6DE;union Cyc_YYSTYPE*_T6DF;union Cyc_YYSTYPE*_T6E0;struct Cyc_Absyn_Datatypefield*_T6E1;struct Cyc_Yystacktype*_T6E2;union Cyc_YYSTYPE*_T6E3;union Cyc_YYSTYPE*_T6E4;struct Cyc_Yystacktype*_T6E5;struct Cyc_Yystacktype _T6E6;struct Cyc_Yyltype _T6E7;unsigned _T6E8;struct Cyc_Yystacktype*_T6E9;union Cyc_YYSTYPE*_T6EA;union Cyc_YYSTYPE*_T6EB;struct Cyc_List_List*(*_T6EC)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T6ED)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T6EE)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T6EF;struct Cyc_Yystacktype _T6F0;struct Cyc_Yyltype _T6F1;unsigned _T6F2;unsigned _T6F3;struct Cyc_Yystacktype*_T6F4;union Cyc_YYSTYPE*_T6F5;union Cyc_YYSTYPE*_T6F6;struct Cyc_List_List*_T6F7;struct Cyc_List_List*_T6F8;struct Cyc_Absyn_Datatypefield*_T6F9;struct Cyc_Yystacktype*_T6FA;union Cyc_YYSTYPE*_T6FB;union Cyc_YYSTYPE*_T6FC;struct Cyc_Yystacktype*_T6FD;struct Cyc_Yystacktype _T6FE;struct Cyc_Yyltype _T6FF;unsigned _T700;struct Cyc_Yystacktype*_T701;union Cyc_YYSTYPE*_T702;union Cyc_YYSTYPE*_T703;struct Cyc_Yystacktype*_T704;struct Cyc_Yystacktype _T705;struct Cyc_Yystacktype*_T706;union Cyc_YYSTYPE*_T707;union Cyc_YYSTYPE*_T708;struct Cyc_Parse_Declarator _T709;struct Cyc_Parse_Declarator _T70A;struct Cyc_Parse_Declarator _T70B;struct Cyc_Yystacktype*_T70C;union Cyc_YYSTYPE*_T70D;union Cyc_YYSTYPE*_T70E;struct Cyc_List_List*_T70F;struct Cyc_Parse_Declarator _T710;struct Cyc_List_List*_T711;struct Cyc_Yystacktype*_T712;struct Cyc_Yystacktype _T713;struct Cyc_Yystacktype*_T714;union Cyc_YYSTYPE*_T715;union Cyc_YYSTYPE*_T716;struct Cyc_Parse_Declarator _T717;struct Cyc_Parse_Declarator _T718;struct Cyc_Parse_Declarator _T719;struct Cyc_Yystacktype*_T71A;union Cyc_YYSTYPE*_T71B;union Cyc_YYSTYPE*_T71C;struct Cyc_List_List*_T71D;struct Cyc_Parse_Declarator _T71E;struct Cyc_List_List*_T71F;struct Cyc_Parse_Declarator _T720;struct Cyc_Yystacktype*_T721;union Cyc_YYSTYPE*_T722;union Cyc_YYSTYPE*_T723;struct Cyc_Yystacktype*_T724;struct Cyc_Yystacktype _T725;struct Cyc_Yyltype _T726;unsigned _T727;struct Cyc_Yystacktype*_T728;struct Cyc_Yystacktype _T729;struct Cyc_Yystacktype*_T72A;union Cyc_YYSTYPE*_T72B;union Cyc_YYSTYPE*_T72C;struct Cyc_List_List*_T72D;struct _RegionHandle*_T72E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T72F;struct _RegionHandle*_T730;struct Cyc_Yystacktype*_T731;struct Cyc_Yystacktype _T732;struct Cyc_Yyltype _T733;unsigned _T734;struct Cyc_Yystacktype*_T735;union Cyc_YYSTYPE*_T736;union Cyc_YYSTYPE*_T737;struct Cyc_Parse_Declarator _T738;struct Cyc_Yystacktype*_T739;struct Cyc_Yystacktype _T73A;struct Cyc_Parse_Declarator _T73B;struct Cyc_Yystacktype*_T73C;union Cyc_YYSTYPE*_T73D;union Cyc_YYSTYPE*_T73E;struct Cyc_Parse_Declarator _T73F;struct Cyc_Yystacktype*_T740;union Cyc_YYSTYPE*_T741;union Cyc_YYSTYPE*_T742;struct Cyc_Parse_Declarator _T743;struct Cyc_List_List*_T744;struct _RegionHandle*_T745;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T746;struct _RegionHandle*_T747;struct Cyc_Yystacktype*_T748;union Cyc_YYSTYPE*_T749;union Cyc_YYSTYPE*_T74A;struct Cyc_Yystacktype*_T74B;struct Cyc_Yystacktype _T74C;struct Cyc_Yyltype _T74D;unsigned _T74E;struct Cyc_Yystacktype*_T74F;union Cyc_YYSTYPE*_T750;union Cyc_YYSTYPE*_T751;struct Cyc_Parse_Declarator _T752;struct Cyc_Parse_Declarator _T753;struct Cyc_Yystacktype*_T754;union Cyc_YYSTYPE*_T755;union Cyc_YYSTYPE*_T756;struct Cyc_Parse_Declarator _T757;struct Cyc_Yystacktype*_T758;union Cyc_YYSTYPE*_T759;union Cyc_YYSTYPE*_T75A;struct Cyc_Parse_Declarator _T75B;struct Cyc_List_List*_T75C;struct _RegionHandle*_T75D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T75E;struct _RegionHandle*_T75F;struct Cyc_Yystacktype*_T760;union Cyc_YYSTYPE*_T761;union Cyc_YYSTYPE*_T762;struct Cyc_Yystacktype*_T763;union Cyc_YYSTYPE*_T764;union Cyc_YYSTYPE*_T765;struct Cyc_Yystacktype*_T766;struct Cyc_Yystacktype _T767;struct Cyc_Yyltype _T768;unsigned _T769;struct Cyc_Yystacktype*_T76A;union Cyc_YYSTYPE*_T76B;union Cyc_YYSTYPE*_T76C;struct Cyc_Parse_Declarator _T76D;struct Cyc_Yystacktype*_T76E;union Cyc_YYSTYPE*_T76F;union Cyc_YYSTYPE*_T770;struct Cyc_Yystacktype*_T771;union Cyc_YYSTYPE*_T772;union Cyc_YYSTYPE*_T773;struct Cyc_Parse_Declarator _T774;struct Cyc_Yystacktype*_T775;union Cyc_YYSTYPE*_T776;union Cyc_YYSTYPE*_T777;struct Cyc_Parse_Declarator _T778;struct Cyc_Yystacktype*_T779;union Cyc_YYSTYPE*_T77A;union Cyc_YYSTYPE*_T77B;struct Cyc_Parse_Declarator _T77C;struct Cyc_List_List*_T77D;struct _RegionHandle*_T77E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T77F;struct _RegionHandle*_T780;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T781;struct _RegionHandle*_T782;struct Cyc_Yystacktype*_T783;union Cyc_YYSTYPE*_T784;union Cyc_YYSTYPE*_T785;struct Cyc_Parse_Declarator _T786;struct Cyc_Parse_Declarator _T787;struct Cyc_Yystacktype*_T788;union Cyc_YYSTYPE*_T789;union Cyc_YYSTYPE*_T78A;struct Cyc_Parse_Declarator _T78B;struct Cyc_Yystacktype*_T78C;union Cyc_YYSTYPE*_T78D;union Cyc_YYSTYPE*_T78E;struct Cyc_Parse_Declarator _T78F;struct Cyc_List_List*_T790;struct _RegionHandle*_T791;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T792;struct _RegionHandle*_T793;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T794;struct _RegionHandle*_T795;struct Cyc_Yystacktype*_T796;union Cyc_YYSTYPE*_T797;union Cyc_YYSTYPE*_T798;struct Cyc_Yystacktype*_T799;struct Cyc_Yystacktype _T79A;struct Cyc_Yyltype _T79B;unsigned _T79C;struct Cyc_Yystacktype*_T79D;union Cyc_YYSTYPE*_T79E;union Cyc_YYSTYPE*_T79F;struct Cyc_Parse_Declarator _T7A0;struct Cyc_List_List*(*_T7A1)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T7A2)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T7A3;struct Cyc_Yystacktype _T7A4;struct Cyc_Yyltype _T7A5;unsigned _T7A6;unsigned _T7A7;struct Cyc_Yystacktype*_T7A8;union Cyc_YYSTYPE*_T7A9;union Cyc_YYSTYPE*_T7AA;struct Cyc_List_List*_T7AB;struct Cyc_List_List*_T7AC;struct Cyc_Parse_Declarator _T7AD;struct Cyc_Yystacktype*_T7AE;union Cyc_YYSTYPE*_T7AF;union Cyc_YYSTYPE*_T7B0;struct Cyc_Parse_Declarator _T7B1;struct Cyc_Yystacktype*_T7B2;union Cyc_YYSTYPE*_T7B3;union Cyc_YYSTYPE*_T7B4;struct Cyc_Parse_Declarator _T7B5;struct Cyc_List_List*_T7B6;struct _RegionHandle*_T7B7;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T7B8;struct _RegionHandle*_T7B9;struct Cyc_Yystacktype*_T7BA;struct Cyc_Yystacktype _T7BB;struct Cyc_Yyltype _T7BC;unsigned _T7BD;struct Cyc_Yystacktype*_T7BE;union Cyc_YYSTYPE*_T7BF;union Cyc_YYSTYPE*_T7C0;struct Cyc_Parse_Declarator _T7C1;struct Cyc_Parse_Declarator _T7C2;struct Cyc_Yystacktype*_T7C3;union Cyc_YYSTYPE*_T7C4;union Cyc_YYSTYPE*_T7C5;struct Cyc_Parse_Declarator _T7C6;struct Cyc_Yystacktype*_T7C7;union Cyc_YYSTYPE*_T7C8;union Cyc_YYSTYPE*_T7C9;struct Cyc_Parse_Declarator _T7CA;struct Cyc_List_List*_T7CB;struct _RegionHandle*_T7CC;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T7CD;struct _RegionHandle*_T7CE;struct Cyc_Yystacktype*_T7CF;struct Cyc_Yystacktype _T7D0;struct Cyc_Yyltype _T7D1;unsigned _T7D2;struct Cyc_Yystacktype*_T7D3;union Cyc_YYSTYPE*_T7D4;union Cyc_YYSTYPE*_T7D5;struct Cyc_Yystacktype*_T7D6;union Cyc_YYSTYPE*_T7D7;union Cyc_YYSTYPE*_T7D8;struct Cyc_Parse_Declarator _T7D9;struct Cyc_Parse_Declarator _T7DA;struct Cyc_Yystacktype*_T7DB;union Cyc_YYSTYPE*_T7DC;union Cyc_YYSTYPE*_T7DD;struct Cyc_Yystacktype*_T7DE;struct Cyc_Yystacktype _T7DF;struct Cyc_Yyltype _T7E0;unsigned _T7E1;struct Cyc_Parse_Declarator _T7E2;struct Cyc_Yystacktype*_T7E3;union Cyc_YYSTYPE*_T7E4;union Cyc_YYSTYPE*_T7E5;struct Cyc_Yystacktype*_T7E6;struct Cyc_Yystacktype _T7E7;struct Cyc_Yyltype _T7E8;unsigned _T7E9;struct Cyc_Yystacktype*_T7EA;struct Cyc_Yystacktype _T7EB;struct Cyc_Yystacktype*_T7EC;union Cyc_YYSTYPE*_T7ED;union Cyc_YYSTYPE*_T7EE;struct Cyc_List_List*_T7EF;struct _RegionHandle*_T7F0;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T7F1;struct _RegionHandle*_T7F2;struct Cyc_Yystacktype*_T7F3;struct Cyc_Yystacktype _T7F4;struct Cyc_Yyltype _T7F5;unsigned _T7F6;struct Cyc_Yystacktype*_T7F7;union Cyc_YYSTYPE*_T7F8;union Cyc_YYSTYPE*_T7F9;struct Cyc_Parse_Declarator _T7FA;struct Cyc_Yystacktype*_T7FB;struct Cyc_Yystacktype _T7FC;struct Cyc_Yystacktype*_T7FD;union Cyc_YYSTYPE*_T7FE;union Cyc_YYSTYPE*_T7FF;struct Cyc_Parse_Declarator _T800;struct Cyc_Parse_Declarator _T801;struct Cyc_Parse_Declarator _T802;struct Cyc_List_List*_T803;struct _RegionHandle*_T804;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T805;struct _RegionHandle*_T806;struct Cyc_Yystacktype*_T807;union Cyc_YYSTYPE*_T808;union Cyc_YYSTYPE*_T809;struct Cyc_Yystacktype*_T80A;struct Cyc_Yystacktype _T80B;struct Cyc_Yyltype _T80C;unsigned _T80D;struct Cyc_Parse_Declarator _T80E;struct Cyc_Yystacktype*_T80F;union Cyc_YYSTYPE*_T810;union Cyc_YYSTYPE*_T811;struct Cyc_Parse_Declarator _T812;struct Cyc_Parse_Declarator _T813;struct Cyc_Parse_Declarator _T814;struct Cyc_List_List*_T815;struct _RegionHandle*_T816;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T817;struct _RegionHandle*_T818;struct Cyc_Yystacktype*_T819;union Cyc_YYSTYPE*_T81A;union Cyc_YYSTYPE*_T81B;struct Cyc_Yystacktype*_T81C;union Cyc_YYSTYPE*_T81D;union Cyc_YYSTYPE*_T81E;struct Cyc_Yystacktype*_T81F;struct Cyc_Yystacktype _T820;struct Cyc_Yyltype _T821;unsigned _T822;struct Cyc_Parse_Declarator _T823;struct Cyc_Yystacktype*_T824;union Cyc_YYSTYPE*_T825;union Cyc_YYSTYPE*_T826;struct Cyc_Yystacktype*_T827;union Cyc_YYSTYPE*_T828;union Cyc_YYSTYPE*_T829;struct Cyc_Yystacktype*_T82A;union Cyc_YYSTYPE*_T82B;union Cyc_YYSTYPE*_T82C;struct Cyc_Parse_Declarator _T82D;struct Cyc_Parse_Declarator _T82E;struct Cyc_Parse_Declarator _T82F;struct Cyc_List_List*_T830;struct _RegionHandle*_T831;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T832;struct _RegionHandle*_T833;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T834;struct _RegionHandle*_T835;struct Cyc_Parse_Declarator _T836;struct Cyc_Yystacktype*_T837;union Cyc_YYSTYPE*_T838;union Cyc_YYSTYPE*_T839;struct Cyc_Parse_Declarator _T83A;struct Cyc_Parse_Declarator _T83B;struct Cyc_Parse_Declarator _T83C;struct Cyc_List_List*_T83D;struct _RegionHandle*_T83E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T83F;struct _RegionHandle*_T840;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T841;struct _RegionHandle*_T842;struct Cyc_Yystacktype*_T843;union Cyc_YYSTYPE*_T844;union Cyc_YYSTYPE*_T845;struct Cyc_Yystacktype*_T846;struct Cyc_Yystacktype _T847;struct Cyc_Yyltype _T848;unsigned _T849;struct Cyc_Parse_Declarator _T84A;struct Cyc_List_List*(*_T84B)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T84C)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T84D;struct Cyc_Yystacktype _T84E;struct Cyc_Yyltype _T84F;unsigned _T850;unsigned _T851;struct Cyc_Yystacktype*_T852;union Cyc_YYSTYPE*_T853;union Cyc_YYSTYPE*_T854;struct Cyc_List_List*_T855;struct Cyc_List_List*_T856;struct Cyc_Yystacktype*_T857;union Cyc_YYSTYPE*_T858;union Cyc_YYSTYPE*_T859;struct Cyc_Parse_Declarator _T85A;struct Cyc_Parse_Declarator _T85B;struct Cyc_Parse_Declarator _T85C;struct Cyc_List_List*_T85D;struct _RegionHandle*_T85E;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T85F;struct _RegionHandle*_T860;struct Cyc_Yystacktype*_T861;struct Cyc_Yystacktype _T862;struct Cyc_Yyltype _T863;unsigned _T864;struct Cyc_Parse_Declarator _T865;struct Cyc_Yystacktype*_T866;union Cyc_YYSTYPE*_T867;union Cyc_YYSTYPE*_T868;struct Cyc_Parse_Declarator _T869;struct Cyc_Parse_Declarator _T86A;struct Cyc_Parse_Declarator _T86B;struct Cyc_List_List*_T86C;struct _RegionHandle*_T86D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T86E;struct _RegionHandle*_T86F;struct Cyc_Yystacktype*_T870;struct Cyc_Yystacktype _T871;struct Cyc_Yyltype _T872;unsigned _T873;struct Cyc_Yystacktype*_T874;union Cyc_YYSTYPE*_T875;union Cyc_YYSTYPE*_T876;struct Cyc_Parse_Declarator _T877;struct Cyc_Yystacktype*_T878;struct Cyc_Yystacktype _T879;struct Cyc_Yystacktype*_T87A;union Cyc_YYSTYPE*_T87B;union Cyc_YYSTYPE*_T87C;struct Cyc_List_List*_T87D;struct Cyc_Yystacktype*_T87E;union Cyc_YYSTYPE*_T87F;union Cyc_YYSTYPE*_T880;struct Cyc_List_List*_T881;struct Cyc_List_List*_T882;struct Cyc_Yystacktype*_T883;union Cyc_YYSTYPE*_T884;union Cyc_YYSTYPE*_T885;struct Cyc_List_List*_T886;struct Cyc_List_List*_T887;struct _RegionHandle*_T888;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T889;struct _RegionHandle*_T88A;struct Cyc_Yystacktype*_T88B;struct Cyc_Yystacktype _T88C;struct Cyc_Yyltype _T88D;unsigned _T88E;struct Cyc_Yystacktype*_T88F;union Cyc_YYSTYPE*_T890;union Cyc_YYSTYPE*_T891;struct Cyc_Yystacktype*_T892;union Cyc_YYSTYPE*_T893;union Cyc_YYSTYPE*_T894;struct _tuple22*_T895;int _T896;struct Cyc_Absyn_PtrLoc*_T897;struct Cyc_Yystacktype*_T898;struct Cyc_Yystacktype _T899;struct Cyc_Yyltype _T89A;unsigned _T89B;struct Cyc_Yystacktype*_T89C;struct Cyc_Yystacktype _T89D;struct Cyc_Yyltype _T89E;unsigned _T89F;struct _RegionHandle*_T8A0;struct Cyc_Absyn_PtrLoc*_T8A1;void*_T8A2;void*_T8A3;struct Cyc_Yystacktype*_T8A4;union Cyc_YYSTYPE*_T8A5;union Cyc_YYSTYPE*_T8A6;void*_T8A7;struct Cyc_Yystacktype*_T8A8;union Cyc_YYSTYPE*_T8A9;union Cyc_YYSTYPE*_T8AA;struct Cyc_List_List*_T8AB;struct Cyc_Yystacktype*_T8AC;union Cyc_YYSTYPE*_T8AD;union Cyc_YYSTYPE*_T8AE;struct Cyc_Absyn_Tqual _T8AF;struct Cyc_List_List*_T8B0;struct _RegionHandle*_T8B1;struct Cyc_List_List*_T8B2;struct _RegionHandle*_T8B3;struct Cyc_Yystacktype*_T8B4;union Cyc_YYSTYPE*_T8B5;union Cyc_YYSTYPE*_T8B6;struct Cyc_Yystacktype*_T8B7;union Cyc_YYSTYPE*_T8B8;union Cyc_YYSTYPE*_T8B9;struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T8BA;struct _RegionHandle*_T8BB;struct Cyc_Yystacktype*_T8BC;union Cyc_YYSTYPE*_T8BD;union Cyc_YYSTYPE*_T8BE;void*_T8BF;struct Cyc_Yystacktype*_T8C0;struct Cyc_Yystacktype _T8C1;struct Cyc_Yyltype _T8C2;unsigned _T8C3;unsigned _T8C4;struct _fat_ptr _T8C5;struct _fat_ptr _T8C6;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8C7;struct _RegionHandle*_T8C8;struct Cyc_List_List*_T8C9;struct Cyc_Yystacktype*_T8CA;union Cyc_YYSTYPE*_T8CB;union Cyc_YYSTYPE*_T8CC;void*_T8CD;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8CE;struct _RegionHandle*_T8CF;struct Cyc_Yystacktype*_T8D0;union Cyc_YYSTYPE*_T8D1;union Cyc_YYSTYPE*_T8D2;void*_T8D3;struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T8D4;struct _RegionHandle*_T8D5;void*_T8D6;struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T8D7;struct _RegionHandle*_T8D8;void*_T8D9;struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T8DA;struct _RegionHandle*_T8DB;void*_T8DC;struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T8DD;struct _RegionHandle*_T8DE;void*_T8DF;struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T8E0;struct _RegionHandle*_T8E1;void*_T8E2;struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T8E3;struct _RegionHandle*_T8E4;void*_T8E5;struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T8E6;struct _RegionHandle*_T8E7;void*_T8E8;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T8E9;struct _RegionHandle*_T8EA;struct Cyc_Yystacktype*_T8EB;union Cyc_YYSTYPE*_T8EC;union Cyc_YYSTYPE*_T8ED;void*_T8EE;struct Cyc_Yystacktype*_T8EF;struct Cyc_Yystacktype _T8F0;struct Cyc_Yyltype _T8F1;unsigned _T8F2;unsigned _T8F3;struct Cyc_Yystacktype*_T8F4;union Cyc_YYSTYPE*_T8F5;union Cyc_YYSTYPE*_T8F6;struct _fat_ptr _T8F7;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T8F8;struct _RegionHandle*_T8F9;void*_T8FA;struct Cyc_Yystacktype*_T8FB;union Cyc_YYSTYPE*_T8FC;union Cyc_YYSTYPE*_T8FD;void*_T8FE;struct Cyc_Yystacktype*_T8FF;union Cyc_YYSTYPE*_T900;union Cyc_YYSTYPE*_T901;void*_T902;void*_T903;void*_T904;struct Cyc_Yystacktype*_T905;union Cyc_YYSTYPE*_T906;union Cyc_YYSTYPE*_T907;void*_T908;void*_T909;void*_T90A;void*_T90B;struct Cyc_Yystacktype*_T90C;struct Cyc_Yystacktype _T90D;struct Cyc_Yyltype _T90E;unsigned _T90F;unsigned _T910;struct Cyc_Yystacktype*_T911;union Cyc_YYSTYPE*_T912;union Cyc_YYSTYPE*_T913;struct _fat_ptr _T914;void*_T915;struct _tuple22*_T916;struct Cyc_Yystacktype*_T917;struct Cyc_Yystacktype _T918;struct Cyc_Yyltype _T919;unsigned _T91A;int _T91B;struct Cyc_Yystacktype*_T91C;struct Cyc_Yystacktype _T91D;struct Cyc_Yyltype _T91E;unsigned _T91F;unsigned _T920;struct _fat_ptr _T921;struct Cyc_Yystacktype*_T922;union Cyc_YYSTYPE*_T923;union Cyc_YYSTYPE*_T924;void*_T925;struct _tuple22*_T926;struct Cyc_Yystacktype*_T927;struct Cyc_Yystacktype _T928;struct Cyc_Yyltype _T929;unsigned _T92A;struct Cyc_Yystacktype*_T92B;struct Cyc_Yystacktype _T92C;struct Cyc_Yyltype _T92D;unsigned _T92E;unsigned _T92F;struct _fat_ptr _T930;struct Cyc_Yystacktype*_T931;union Cyc_YYSTYPE*_T932;union Cyc_YYSTYPE*_T933;void*_T934;struct _tuple22*_T935;int _T936;int _T937;struct _tuple22*_T938;struct Cyc_Yystacktype*_T939;struct Cyc_Yystacktype _T93A;struct Cyc_Yyltype _T93B;unsigned _T93C;struct Cyc_Yystacktype*_T93D;struct Cyc_Yystacktype _T93E;struct Cyc_Yyltype _T93F;unsigned _T940;unsigned _T941;struct _fat_ptr _T942;struct Cyc_Core_Opt*_T943;struct Cyc_Core_Opt*_T944;void*_T945;struct _tuple22*_T946;struct Cyc_Yystacktype*_T947;struct Cyc_Yystacktype _T948;struct Cyc_Yyltype _T949;unsigned _T94A;void*_T94B;int _T94C;struct Cyc_Core_Opt*_T94D;struct Cyc_Core_Opt*_T94E;struct Cyc_Yystacktype*_T94F;union Cyc_YYSTYPE*_T950;union Cyc_YYSTYPE*_T951;struct Cyc_Absyn_Exp*_T952;void*_T953;int _T954;int(*_T955)(unsigned,struct _fat_ptr);struct _fat_ptr _T956;struct Cyc_Yystacktype*_T957;union Cyc_YYSTYPE*_T958;union Cyc_YYSTYPE*_T959;struct _fat_ptr _T95A;void*_T95B;void*_T95C;struct Cyc_List_List*_T95D;struct Cyc_Yystacktype*_T95E;union Cyc_YYSTYPE*_T95F;union Cyc_YYSTYPE*_T960;struct Cyc_List_List*_T961;struct Cyc_Yystacktype*_T962;union Cyc_YYSTYPE*_T963;union Cyc_YYSTYPE*_T964;struct Cyc_Yystacktype*_T965;union Cyc_YYSTYPE*_T966;union Cyc_YYSTYPE*_T967;struct Cyc_Core_Opt*_T968;struct Cyc_Core_Opt*_T969;void*_T96A;struct Cyc_Yystacktype*_T96B;union Cyc_YYSTYPE*_T96C;union Cyc_YYSTYPE*_T96D;int _T96E;struct Cyc_List_List*_T96F;void*_T970;struct Cyc_Yystacktype*_T971;union Cyc_YYSTYPE*_T972;union Cyc_YYSTYPE*_T973;struct Cyc_List_List*_T974;void*_T975;struct Cyc_Core_Opt*_T976;struct Cyc_Core_Opt*_T977;void*_T978;struct _fat_ptr _T979;int _T97A;char*_T97B;struct Cyc_Yystacktype*_T97C;struct Cyc_Yystacktype _T97D;struct Cyc_Yyltype _T97E;unsigned _T97F;unsigned _T980;struct Cyc_Absyn_Tqual _T981;struct Cyc_Yystacktype*_T982;union Cyc_YYSTYPE*_T983;union Cyc_YYSTYPE*_T984;struct Cyc_Absyn_Tqual _T985;struct Cyc_Yystacktype*_T986;union Cyc_YYSTYPE*_T987;union Cyc_YYSTYPE*_T988;struct Cyc_Absyn_Tqual _T989;struct Cyc_Absyn_Tqual _T98A;struct Cyc_Yystacktype*_T98B;union Cyc_YYSTYPE*_T98C;union Cyc_YYSTYPE*_T98D;struct _tuple28 _T98E;struct _tuple28*_T98F;unsigned _T990;struct _tuple28*_T991;struct _tuple28 _T992;struct _tuple27*_T993;struct Cyc_Yystacktype*_T994;union Cyc_YYSTYPE*_T995;union Cyc_YYSTYPE*_T996;struct Cyc_Yystacktype*_T997;union Cyc_YYSTYPE*_T998;union Cyc_YYSTYPE*_T999;struct _tuple28 _T99A;struct _tuple28*_T99B;unsigned _T99C;struct _tuple28*_T99D;struct _tuple28 _T99E;struct _tuple27*_T99F;struct Cyc_Yystacktype*_T9A0;union Cyc_YYSTYPE*_T9A1;union Cyc_YYSTYPE*_T9A2;struct Cyc_List_List*_T9A3;struct Cyc_Yystacktype*_T9A4;union Cyc_YYSTYPE*_T9A5;union Cyc_YYSTYPE*_T9A6;struct Cyc_Yystacktype*_T9A7;union Cyc_YYSTYPE*_T9A8;union Cyc_YYSTYPE*_T9A9;struct _tuple28 _T9AA;struct _tuple28*_T9AB;unsigned _T9AC;struct _tuple28*_T9AD;struct _tuple28 _T9AE;struct _tuple27*_T9AF;struct Cyc_Yystacktype*_T9B0;union Cyc_YYSTYPE*_T9B1;union Cyc_YYSTYPE*_T9B2;struct Cyc_List_List*_T9B3;struct Cyc_Yystacktype*_T9B4;union Cyc_YYSTYPE*_T9B5;union Cyc_YYSTYPE*_T9B6;struct Cyc_Yystacktype*_T9B7;union Cyc_YYSTYPE*_T9B8;union Cyc_YYSTYPE*_T9B9;struct Cyc_Absyn_VarargInfo*_T9BA;struct Cyc_Absyn_VarargInfo*_T9BB;struct Cyc_Absyn_VarargInfo*_T9BC;struct Cyc_Absyn_VarargInfo*_T9BD;struct Cyc_Yystacktype*_T9BE;union Cyc_YYSTYPE*_T9BF;union Cyc_YYSTYPE*_T9C0;struct Cyc_Yystacktype*_T9C1;union Cyc_YYSTYPE*_T9C2;union Cyc_YYSTYPE*_T9C3;struct _tuple28 _T9C4;struct _tuple28*_T9C5;unsigned _T9C6;struct _tuple28*_T9C7;struct _tuple28 _T9C8;struct _tuple27*_T9C9;struct Cyc_Yystacktype*_T9CA;union Cyc_YYSTYPE*_T9CB;union Cyc_YYSTYPE*_T9CC;struct Cyc_Yystacktype*_T9CD;union Cyc_YYSTYPE*_T9CE;union Cyc_YYSTYPE*_T9CF;struct Cyc_Absyn_VarargInfo*_T9D0;struct Cyc_Absyn_VarargInfo*_T9D1;struct Cyc_Absyn_VarargInfo*_T9D2;struct Cyc_Absyn_VarargInfo*_T9D3;struct Cyc_Yystacktype*_T9D4;union Cyc_YYSTYPE*_T9D5;union Cyc_YYSTYPE*_T9D6;struct Cyc_Yystacktype*_T9D7;union Cyc_YYSTYPE*_T9D8;union Cyc_YYSTYPE*_T9D9;struct _tuple28 _T9DA;struct _tuple28*_T9DB;unsigned _T9DC;struct _tuple28*_T9DD;struct _tuple28 _T9DE;struct _tuple27*_T9DF;struct Cyc_Yystacktype*_T9E0;union Cyc_YYSTYPE*_T9E1;union Cyc_YYSTYPE*_T9E2;struct Cyc_List_List*_T9E3;struct Cyc_Yystacktype*_T9E4;union Cyc_YYSTYPE*_T9E5;union Cyc_YYSTYPE*_T9E6;struct Cyc_Yystacktype*_T9E7;struct Cyc_Yystacktype _T9E8;struct Cyc_Yyltype _T9E9;unsigned _T9EA;unsigned _T9EB;struct Cyc_Yystacktype*_T9EC;union Cyc_YYSTYPE*_T9ED;union Cyc_YYSTYPE*_T9EE;struct _fat_ptr _T9EF;void*_T9F0;struct Cyc_Yystacktype*_T9F1;union Cyc_YYSTYPE*_T9F2;union Cyc_YYSTYPE*_T9F3;struct _fat_ptr _T9F4;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T9F5;void*_T9F6;struct Cyc_Yystacktype*_T9F7;union Cyc_YYSTYPE*_T9F8;union Cyc_YYSTYPE*_T9F9;void*_T9FA;struct Cyc_Yystacktype*_T9FB;struct Cyc_Yystacktype _T9FC;struct Cyc_Yyltype _T9FD;unsigned _T9FE;unsigned _T9FF;void*_TA00;struct Cyc_Yystacktype*_TA01;union Cyc_YYSTYPE*_TA02;union Cyc_YYSTYPE*_TA03;struct _fat_ptr _TA04;struct Cyc_Yystacktype*_TA05;union Cyc_YYSTYPE*_TA06;union Cyc_YYSTYPE*_TA07;struct Cyc_Absyn_Kind*_TA08;void*_TA09;struct Cyc_Yystacktype*_TA0A;union Cyc_YYSTYPE*_TA0B;union Cyc_YYSTYPE*_TA0C;void*_TA0D;struct Cyc_Yystacktype*_TA0E;struct Cyc_Yystacktype _TA0F;struct Cyc_Yyltype _TA10;unsigned _TA11;unsigned _TA12;void*_TA13;struct Cyc_Yystacktype*_TA14;union Cyc_YYSTYPE*_TA15;union Cyc_YYSTYPE*_TA16;struct Cyc_List_List*_TA17;void*_TA18;struct Cyc_Yystacktype*_TA19;struct Cyc_Yystacktype _TA1A;struct _tuple28*_TA1B;struct Cyc_List_List*_TA1C;struct Cyc_Yystacktype*_TA1D;union Cyc_YYSTYPE*_TA1E;union Cyc_YYSTYPE*_TA1F;struct _fat_ptr _TA20;struct _tuple28*_TA21;struct Cyc_List_List*_TA22;struct Cyc_Yystacktype*_TA23;union Cyc_YYSTYPE*_TA24;union Cyc_YYSTYPE*_TA25;struct _fat_ptr _TA26;struct Cyc_Yystacktype*_TA27;union Cyc_YYSTYPE*_TA28;union Cyc_YYSTYPE*_TA29;struct _tuple28*_TA2A;struct _tuple28*_TA2B;struct Cyc_List_List*_TA2C;struct Cyc_Yystacktype*_TA2D;union Cyc_YYSTYPE*_TA2E;union Cyc_YYSTYPE*_TA2F;struct Cyc_Yystacktype*_TA30;union Cyc_YYSTYPE*_TA31;union Cyc_YYSTYPE*_TA32;struct _tuple28*_TA33;struct _tuple28*_TA34;struct Cyc_List_List*_TA35;struct Cyc_Yystacktype*_TA36;union Cyc_YYSTYPE*_TA37;union Cyc_YYSTYPE*_TA38;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA39;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA3A;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA3B;struct Cyc_Absyn_Kind*_TA3C;struct Cyc_Yystacktype*_TA3D;union Cyc_YYSTYPE*_TA3E;union Cyc_YYSTYPE*_TA3F;struct _fat_ptr _TA40;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA41;void*_TA42;struct Cyc_Yystacktype*_TA43;struct Cyc_Yystacktype _TA44;struct Cyc_Yyltype _TA45;unsigned _TA46;unsigned _TA47;struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_TA48;struct Cyc_Yystacktype*_TA49;union Cyc_YYSTYPE*_TA4A;union Cyc_YYSTYPE*_TA4B;struct Cyc_List_List*_TA4C;void*_TA4D;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_TA4E;struct Cyc_Yystacktype*_TA4F;union Cyc_YYSTYPE*_TA50;union Cyc_YYSTYPE*_TA51;struct Cyc_List_List*_TA52;struct Cyc_Yystacktype*_TA53;union Cyc_YYSTYPE*_TA54;union Cyc_YYSTYPE*_TA55;struct Cyc_List_List*_TA56;void*_TA57;struct Cyc_Yystacktype*_TA58;struct Cyc_Yystacktype _TA59;struct Cyc_Yyltype _TA5A;unsigned _TA5B;unsigned _TA5C;struct Cyc_Yystacktype*_TA5D;union Cyc_YYSTYPE*_TA5E;union Cyc_YYSTYPE*_TA5F;struct _fat_ptr _TA60;struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_TA61;struct Cyc_Yystacktype*_TA62;union Cyc_YYSTYPE*_TA63;union Cyc_YYSTYPE*_TA64;struct Cyc_List_List*_TA65;void*_TA66;struct _tuple29*_TA67;struct Cyc_Yystacktype*_TA68;union Cyc_YYSTYPE*_TA69;union Cyc_YYSTYPE*_TA6A;struct Cyc_Yystacktype*_TA6B;union Cyc_YYSTYPE*_TA6C;union Cyc_YYSTYPE*_TA6D;struct Cyc_Yystacktype*_TA6E;struct Cyc_Yystacktype _TA6F;struct Cyc_Yyltype _TA70;unsigned _TA71;unsigned _TA72;struct Cyc_Yystacktype*_TA73;union Cyc_YYSTYPE*_TA74;union Cyc_YYSTYPE*_TA75;struct _fat_ptr _TA76;void*_TA77;struct Cyc_Yystacktype*_TA78;union Cyc_YYSTYPE*_TA79;union Cyc_YYSTYPE*_TA7A;void*_TA7B;struct Cyc_Yystacktype*_TA7C;union Cyc_YYSTYPE*_TA7D;union Cyc_YYSTYPE*_TA7E;void*_TA7F;void*_TA80;void*_TA81;void*_TA82;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA83;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA84;struct Cyc_Absyn_Kind*_TA85;struct Cyc_Yystacktype*_TA86;union Cyc_YYSTYPE*_TA87;union Cyc_YYSTYPE*_TA88;struct _fat_ptr _TA89;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA8A;void*_TA8B;struct Cyc_Yystacktype*_TA8C;struct Cyc_Yystacktype _TA8D;struct Cyc_Yyltype _TA8E;unsigned _TA8F;unsigned _TA90;void*_TA91;struct Cyc_Yystacktype*_TA92;union Cyc_YYSTYPE*_TA93;union Cyc_YYSTYPE*_TA94;void*_TA95;void*_TA96;struct Cyc_Yystacktype*_TA97;union Cyc_YYSTYPE*_TA98;union Cyc_YYSTYPE*_TA99;struct _fat_ptr _TA9A;struct _fat_ptr _TA9B;int _TA9C;struct Cyc_Warn_String_Warn_Warg_struct _TA9D;struct Cyc_Yystacktype*_TA9E;struct Cyc_Yystacktype _TA9F;struct Cyc_Yyltype _TAA0;unsigned _TAA1;unsigned _TAA2;struct _fat_ptr _TAA3;struct Cyc_Yystacktype*_TAA4;struct Cyc_Yystacktype _TAA5;struct Cyc_Yystacktype*_TAA6;union Cyc_YYSTYPE*_TAA7;union Cyc_YYSTYPE*_TAA8;struct Cyc_List_List*_TAA9;struct Cyc_Yystacktype*_TAAA;union Cyc_YYSTYPE*_TAAB;union Cyc_YYSTYPE*_TAAC;struct Cyc_List_List*_TAAD;struct Cyc_List_List*_TAAE;struct Cyc_Yystacktype*_TAAF;struct Cyc_Yystacktype _TAB0;struct Cyc_List_List*_TAB1;struct Cyc_Yystacktype*_TAB2;union Cyc_YYSTYPE*_TAB3;union Cyc_YYSTYPE*_TAB4;void*_TAB5;struct Cyc_Yystacktype*_TAB6;union Cyc_YYSTYPE*_TAB7;union Cyc_YYSTYPE*_TAB8;void*_TAB9;struct Cyc_Absyn_Kind*_TABA;struct Cyc_Absyn_Kind*_TABB;struct Cyc_List_List*_TABC;struct Cyc_Yystacktype*_TABD;union Cyc_YYSTYPE*_TABE;union Cyc_YYSTYPE*_TABF;struct Cyc_List_List*_TAC0;struct Cyc_Yystacktype*_TAC1;union Cyc_YYSTYPE*_TAC2;union Cyc_YYSTYPE*_TAC3;struct _tuple8*_TAC4;struct Cyc_Yystacktype*_TAC5;struct Cyc_Yystacktype _TAC6;struct Cyc_Yyltype _TAC7;unsigned _TAC8;unsigned _TAC9;struct Cyc_List_List*_TACA;struct Cyc_Yystacktype*_TACB;union Cyc_YYSTYPE*_TACC;union Cyc_YYSTYPE*_TACD;struct _tuple8*_TACE;struct Cyc_Yystacktype*_TACF;struct Cyc_Yystacktype _TAD0;struct Cyc_Yyltype _TAD1;unsigned _TAD2;unsigned _TAD3;struct Cyc_Yystacktype*_TAD4;union Cyc_YYSTYPE*_TAD5;union Cyc_YYSTYPE*_TAD6;struct Cyc_List_List*_TAD7;struct Cyc_Yystacktype*_TAD8;union Cyc_YYSTYPE*_TAD9;union Cyc_YYSTYPE*_TADA;struct Cyc_List_List*_TADB;struct Cyc_Yystacktype*_TADC;union Cyc_YYSTYPE*_TADD;union Cyc_YYSTYPE*_TADE;struct Cyc_Yystacktype*_TADF;union Cyc_YYSTYPE*_TAE0;union Cyc_YYSTYPE*_TAE1;struct Cyc_Yystacktype*_TAE2;union Cyc_YYSTYPE*_TAE3;union Cyc_YYSTYPE*_TAE4;struct Cyc_Absyn_Tqual _TAE5;unsigned _TAE6;struct Cyc_Yystacktype*_TAE7;struct Cyc_Yystacktype _TAE8;struct Cyc_Yyltype _TAE9;unsigned _TAEA;struct Cyc_Yystacktype*_TAEB;union Cyc_YYSTYPE*_TAEC;union Cyc_YYSTYPE*_TAED;struct Cyc_Parse_Type_specifier _TAEE;struct Cyc_Yystacktype*_TAEF;struct Cyc_Yystacktype _TAF0;struct Cyc_Yyltype _TAF1;unsigned _TAF2;unsigned _TAF3;struct Cyc_Warn_String_Warn_Warg_struct _TAF4;struct Cyc_Yystacktype*_TAF5;struct Cyc_Yystacktype _TAF6;struct Cyc_Yyltype _TAF7;unsigned _TAF8;unsigned _TAF9;struct _fat_ptr _TAFA;int _TAFB;struct Cyc_Warn_String_Warn_Warg_struct _TAFC;struct Cyc_Yystacktype*_TAFD;struct Cyc_Yystacktype _TAFE;struct Cyc_Yyltype _TAFF;unsigned _TB00;unsigned _TB01;struct _fat_ptr _TB02;struct _tuple0*_TB03;struct _tuple0 _TB04;struct Cyc_Warn_String_Warn_Warg_struct _TB05;struct Cyc_Yystacktype*_TB06;struct Cyc_Yystacktype _TB07;struct Cyc_Yyltype _TB08;unsigned _TB09;unsigned _TB0A;struct _fat_ptr _TB0B;struct _tuple8*_TB0C;struct Cyc_Yystacktype*_TB0D;union Cyc_YYSTYPE*_TB0E;union Cyc_YYSTYPE*_TB0F;struct Cyc_Absyn_Tqual _TB10;unsigned _TB11;struct Cyc_Yystacktype*_TB12;struct Cyc_Yystacktype _TB13;struct Cyc_Yyltype _TB14;unsigned _TB15;struct Cyc_Parse_Type_specifier _TB16;struct Cyc_Yystacktype*_TB17;struct Cyc_Yystacktype _TB18;struct Cyc_Yyltype _TB19;unsigned _TB1A;unsigned _TB1B;struct Cyc_Warn_String_Warn_Warg_struct _TB1C;struct Cyc_Yystacktype*_TB1D;struct Cyc_Yystacktype _TB1E;struct Cyc_Yyltype _TB1F;unsigned _TB20;unsigned _TB21;struct _fat_ptr _TB22;struct _tuple8*_TB23;struct Cyc_Yystacktype*_TB24;union Cyc_YYSTYPE*_TB25;union Cyc_YYSTYPE*_TB26;struct Cyc_Absyn_Tqual _TB27;unsigned _TB28;struct Cyc_Yystacktype*_TB29;struct Cyc_Yystacktype _TB2A;struct Cyc_Yyltype _TB2B;unsigned _TB2C;struct Cyc_Parse_Type_specifier _TB2D;struct Cyc_Yystacktype*_TB2E;struct Cyc_Yystacktype _TB2F;struct Cyc_Yyltype _TB30;unsigned _TB31;unsigned _TB32;struct Cyc_Yystacktype*_TB33;union Cyc_YYSTYPE*_TB34;union Cyc_YYSTYPE*_TB35;struct Cyc_Parse_Abstractdeclarator _TB36;struct Cyc_Warn_String_Warn_Warg_struct _TB37;struct Cyc_Yystacktype*_TB38;struct Cyc_Yystacktype _TB39;struct Cyc_Yyltype _TB3A;unsigned _TB3B;unsigned _TB3C;struct _fat_ptr _TB3D;struct Cyc_Warn_String_Warn_Warg_struct _TB3E;struct Cyc_Yystacktype*_TB3F;struct Cyc_Yystacktype _TB40;struct Cyc_Yyltype _TB41;unsigned _TB42;unsigned _TB43;struct _fat_ptr _TB44;struct _tuple8*_TB45;struct Cyc_Yystacktype*_TB46;union Cyc_YYSTYPE*_TB47;union Cyc_YYSTYPE*_TB48;struct Cyc_List_List*_TB49;struct Cyc_List_List*_TB4A;struct Cyc_List_List*_TB4B;struct _fat_ptr*_TB4C;struct Cyc_Yystacktype*_TB4D;union Cyc_YYSTYPE*_TB4E;union Cyc_YYSTYPE*_TB4F;struct Cyc_List_List*_TB50;struct _fat_ptr*_TB51;struct Cyc_Yystacktype*_TB52;union Cyc_YYSTYPE*_TB53;union Cyc_YYSTYPE*_TB54;struct Cyc_Yystacktype*_TB55;union Cyc_YYSTYPE*_TB56;union Cyc_YYSTYPE*_TB57;struct Cyc_Yystacktype*_TB58;struct Cyc_Yystacktype _TB59;struct Cyc_Yystacktype*_TB5A;struct Cyc_Yystacktype _TB5B;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB5C;void*_TB5D;struct Cyc_Yystacktype*_TB5E;struct Cyc_Yystacktype _TB5F;struct Cyc_Yyltype _TB60;unsigned _TB61;unsigned _TB62;struct Cyc_Absyn_Exp*_TB63;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB64;struct Cyc_Yystacktype*_TB65;union Cyc_YYSTYPE*_TB66;union Cyc_YYSTYPE*_TB67;struct Cyc_List_List*_TB68;void*_TB69;struct Cyc_Yystacktype*_TB6A;struct Cyc_Yystacktype _TB6B;struct Cyc_Yyltype _TB6C;unsigned _TB6D;unsigned _TB6E;struct Cyc_Absyn_Exp*_TB6F;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB70;struct Cyc_Yystacktype*_TB71;union Cyc_YYSTYPE*_TB72;union Cyc_YYSTYPE*_TB73;struct Cyc_List_List*_TB74;void*_TB75;struct Cyc_Yystacktype*_TB76;struct Cyc_Yystacktype _TB77;struct Cyc_Yyltype _TB78;unsigned _TB79;unsigned _TB7A;struct Cyc_Absyn_Exp*_TB7B;struct Cyc_Yystacktype*_TB7C;struct Cyc_Yystacktype _TB7D;struct Cyc_Yyltype _TB7E;unsigned _TB7F;unsigned _TB80;struct _tuple0*_TB81;struct _fat_ptr*_TB82;struct Cyc_Yystacktype*_TB83;union Cyc_YYSTYPE*_TB84;union Cyc_YYSTYPE*_TB85;void*_TB86;struct Cyc_Yystacktype*_TB87;struct Cyc_Yystacktype _TB88;struct Cyc_Yyltype _TB89;unsigned _TB8A;unsigned _TB8B;struct Cyc_Absyn_Exp*_TB8C;struct Cyc_Absyn_Vardecl*_TB8D;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TB8E;struct Cyc_Yystacktype*_TB8F;union Cyc_YYSTYPE*_TB90;union Cyc_YYSTYPE*_TB91;struct Cyc_Yystacktype*_TB92;union Cyc_YYSTYPE*_TB93;union Cyc_YYSTYPE*_TB94;void*_TB95;struct Cyc_Yystacktype*_TB96;struct Cyc_Yystacktype _TB97;struct Cyc_Yyltype _TB98;unsigned _TB99;unsigned _TB9A;struct Cyc_Absyn_Exp*_TB9B;struct Cyc_Yystacktype*_TB9C;union Cyc_YYSTYPE*_TB9D;union Cyc_YYSTYPE*_TB9E;struct _tuple8*_TB9F;struct Cyc_Yystacktype*_TBA0;struct Cyc_Yystacktype _TBA1;struct Cyc_Yyltype _TBA2;unsigned _TBA3;unsigned _TBA4;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TBA5;struct Cyc_Yystacktype*_TBA6;union Cyc_YYSTYPE*_TBA7;union Cyc_YYSTYPE*_TBA8;void*_TBA9;struct Cyc_Yystacktype*_TBAA;struct Cyc_Yystacktype _TBAB;struct Cyc_Yyltype _TBAC;unsigned _TBAD;unsigned _TBAE;struct Cyc_Absyn_Exp*_TBAF;struct Cyc_List_List*_TBB0;struct _tuple34*_TBB1;struct Cyc_Yystacktype*_TBB2;union Cyc_YYSTYPE*_TBB3;union Cyc_YYSTYPE*_TBB4;struct Cyc_List_List*_TBB5;struct _tuple34*_TBB6;struct Cyc_Yystacktype*_TBB7;union Cyc_YYSTYPE*_TBB8;union Cyc_YYSTYPE*_TBB9;struct Cyc_Yystacktype*_TBBA;union Cyc_YYSTYPE*_TBBB;union Cyc_YYSTYPE*_TBBC;struct Cyc_List_List*_TBBD;struct _tuple34*_TBBE;struct Cyc_Yystacktype*_TBBF;union Cyc_YYSTYPE*_TBC0;union Cyc_YYSTYPE*_TBC1;struct Cyc_Yystacktype*_TBC2;union Cyc_YYSTYPE*_TBC3;union Cyc_YYSTYPE*_TBC4;struct Cyc_List_List*_TBC5;struct _tuple34*_TBC6;struct Cyc_Yystacktype*_TBC7;union Cyc_YYSTYPE*_TBC8;union Cyc_YYSTYPE*_TBC9;struct Cyc_Yystacktype*_TBCA;union Cyc_YYSTYPE*_TBCB;union Cyc_YYSTYPE*_TBCC;struct Cyc_Yystacktype*_TBCD;union Cyc_YYSTYPE*_TBCE;union Cyc_YYSTYPE*_TBCF;struct Cyc_Yystacktype*_TBD0;union Cyc_YYSTYPE*_TBD1;union Cyc_YYSTYPE*_TBD2;struct Cyc_List_List*_TBD3;struct Cyc_List_List*_TBD4;struct Cyc_List_List*_TBD5;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBD6;struct _fat_ptr*_TBD7;struct Cyc_Yystacktype*_TBD8;union Cyc_YYSTYPE*_TBD9;union Cyc_YYSTYPE*_TBDA;struct Cyc_List_List*_TBDB;struct Cyc_Yystacktype*_TBDC;union Cyc_YYSTYPE*_TBDD;union Cyc_YYSTYPE*_TBDE;struct Cyc_List_List*_TBDF;struct Cyc_Yystacktype*_TBE0;union Cyc_YYSTYPE*_TBE1;union Cyc_YYSTYPE*_TBE2;struct Cyc_Yystacktype*_TBE3;union Cyc_YYSTYPE*_TBE4;union Cyc_YYSTYPE*_TBE5;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_TBE6;struct Cyc_Yystacktype*_TBE7;union Cyc_YYSTYPE*_TBE8;union Cyc_YYSTYPE*_TBE9;void*_TBEA;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBEB;struct _fat_ptr*_TBEC;struct Cyc_Yystacktype*_TBED;union Cyc_YYSTYPE*_TBEE;union Cyc_YYSTYPE*_TBEF;void*_TBF0;struct Cyc_Yystacktype*_TBF1;union Cyc_YYSTYPE*_TBF2;union Cyc_YYSTYPE*_TBF3;struct Cyc_Parse_Type_specifier _TBF4;struct Cyc_Yystacktype*_TBF5;struct Cyc_Yystacktype _TBF6;struct Cyc_Yyltype _TBF7;unsigned _TBF8;unsigned _TBF9;struct Cyc_Yystacktype*_TBFA;struct Cyc_Yystacktype _TBFB;struct Cyc_Yyltype _TBFC;unsigned _TBFD;unsigned _TBFE;struct _fat_ptr _TBFF;struct _fat_ptr _TC00;struct _tuple8*_TC01;struct Cyc_Yystacktype*_TC02;union Cyc_YYSTYPE*_TC03;union Cyc_YYSTYPE*_TC04;struct Cyc_Parse_Type_specifier _TC05;struct Cyc_Yystacktype*_TC06;struct Cyc_Yystacktype _TC07;struct Cyc_Yyltype _TC08;unsigned _TC09;unsigned _TC0A;struct Cyc_Yystacktype*_TC0B;union Cyc_YYSTYPE*_TC0C;union Cyc_YYSTYPE*_TC0D;struct Cyc_Parse_Abstractdeclarator _TC0E;struct _tuple14 _TC0F;struct Cyc_List_List*_TC10;struct Cyc_Yystacktype*_TC11;struct Cyc_Yystacktype _TC12;struct Cyc_Yyltype _TC13;unsigned _TC14;unsigned _TC15;struct _fat_ptr _TC16;struct _fat_ptr _TC17;struct _tuple14 _TC18;struct Cyc_List_List*_TC19;struct Cyc_Yystacktype*_TC1A;struct Cyc_Yystacktype _TC1B;struct Cyc_Yyltype _TC1C;unsigned _TC1D;unsigned _TC1E;struct _fat_ptr _TC1F;struct _fat_ptr _TC20;struct _tuple8*_TC21;struct _tuple14 _TC22;struct _tuple14 _TC23;struct Cyc_Yystacktype*_TC24;union Cyc_YYSTYPE*_TC25;union Cyc_YYSTYPE*_TC26;struct _tuple8*_TC27;struct Cyc_Yystacktype*_TC28;struct Cyc_Yystacktype _TC29;struct Cyc_Yyltype _TC2A;unsigned _TC2B;unsigned _TC2C;void*_TC2D;void*_TC2E;struct Cyc_Yystacktype*_TC2F;union Cyc_YYSTYPE*_TC30;union Cyc_YYSTYPE*_TC31;struct Cyc_List_List*_TC32;void*_TC33;struct Cyc_Yystacktype*_TC34;union Cyc_YYSTYPE*_TC35;union Cyc_YYSTYPE*_TC36;void*_TC37;void*_TC38;struct Cyc_List_List*_TC39;struct Cyc_Yystacktype*_TC3A;union Cyc_YYSTYPE*_TC3B;union Cyc_YYSTYPE*_TC3C;struct Cyc_Yystacktype*_TC3D;union Cyc_YYSTYPE*_TC3E;union Cyc_YYSTYPE*_TC3F;void*_TC40;struct Cyc_Yystacktype*_TC41;union Cyc_YYSTYPE*_TC42;union Cyc_YYSTYPE*_TC43;void*_TC44;struct Cyc_List_List*_TC45;struct Cyc_Yystacktype*_TC46;union Cyc_YYSTYPE*_TC47;union Cyc_YYSTYPE*_TC48;struct Cyc_List_List*_TC49;struct Cyc_Yystacktype*_TC4A;union Cyc_YYSTYPE*_TC4B;union Cyc_YYSTYPE*_TC4C;struct Cyc_Yystacktype*_TC4D;union Cyc_YYSTYPE*_TC4E;union Cyc_YYSTYPE*_TC4F;struct Cyc_Parse_Abstractdeclarator _TC50;struct Cyc_Yystacktype*_TC51;union Cyc_YYSTYPE*_TC52;union Cyc_YYSTYPE*_TC53;struct Cyc_Yystacktype*_TC54;struct Cyc_Yystacktype _TC55;struct Cyc_Parse_Abstractdeclarator _TC56;struct Cyc_Yystacktype*_TC57;union Cyc_YYSTYPE*_TC58;union Cyc_YYSTYPE*_TC59;struct Cyc_List_List*_TC5A;struct Cyc_Yystacktype*_TC5B;union Cyc_YYSTYPE*_TC5C;union Cyc_YYSTYPE*_TC5D;struct Cyc_Parse_Abstractdeclarator _TC5E;struct Cyc_List_List*_TC5F;struct Cyc_Yystacktype*_TC60;struct Cyc_Yystacktype _TC61;struct Cyc_Parse_Abstractdeclarator _TC62;struct Cyc_List_List*_TC63;struct _RegionHandle*_TC64;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC65;struct _RegionHandle*_TC66;struct Cyc_Yystacktype*_TC67;union Cyc_YYSTYPE*_TC68;union Cyc_YYSTYPE*_TC69;struct Cyc_Yystacktype*_TC6A;struct Cyc_Yystacktype _TC6B;struct Cyc_Yyltype _TC6C;unsigned _TC6D;struct Cyc_Parse_Abstractdeclarator _TC6E;struct Cyc_List_List*_TC6F;struct _RegionHandle*_TC70;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC71;struct _RegionHandle*_TC72;struct Cyc_Yystacktype*_TC73;union Cyc_YYSTYPE*_TC74;union Cyc_YYSTYPE*_TC75;struct Cyc_Yystacktype*_TC76;struct Cyc_Yystacktype _TC77;struct Cyc_Yyltype _TC78;unsigned _TC79;struct Cyc_Yystacktype*_TC7A;union Cyc_YYSTYPE*_TC7B;union Cyc_YYSTYPE*_TC7C;struct Cyc_Parse_Abstractdeclarator _TC7D;struct Cyc_Parse_Abstractdeclarator _TC7E;struct Cyc_List_List*_TC7F;struct _RegionHandle*_TC80;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TC81;struct _RegionHandle*_TC82;struct Cyc_Yystacktype*_TC83;union Cyc_YYSTYPE*_TC84;union Cyc_YYSTYPE*_TC85;struct Cyc_Yystacktype*_TC86;union Cyc_YYSTYPE*_TC87;union Cyc_YYSTYPE*_TC88;struct Cyc_Yystacktype*_TC89;struct Cyc_Yystacktype _TC8A;struct Cyc_Yyltype _TC8B;unsigned _TC8C;struct Cyc_Parse_Abstractdeclarator _TC8D;struct Cyc_List_List*_TC8E;struct _RegionHandle*_TC8F;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TC90;struct _RegionHandle*_TC91;struct Cyc_Yystacktype*_TC92;union Cyc_YYSTYPE*_TC93;union Cyc_YYSTYPE*_TC94;struct Cyc_Yystacktype*_TC95;union Cyc_YYSTYPE*_TC96;union Cyc_YYSTYPE*_TC97;struct Cyc_Yystacktype*_TC98;struct Cyc_Yystacktype _TC99;struct Cyc_Yyltype _TC9A;unsigned _TC9B;struct Cyc_Yystacktype*_TC9C;union Cyc_YYSTYPE*_TC9D;union Cyc_YYSTYPE*_TC9E;struct Cyc_Parse_Abstractdeclarator _TC9F;struct Cyc_Yystacktype*_TCA0;union Cyc_YYSTYPE*_TCA1;union Cyc_YYSTYPE*_TCA2;struct Cyc_Yystacktype*_TCA3;union Cyc_YYSTYPE*_TCA4;union Cyc_YYSTYPE*_TCA5;struct Cyc_Parse_Abstractdeclarator _TCA6;struct Cyc_List_List*_TCA7;struct _RegionHandle*_TCA8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCA9;struct _RegionHandle*_TCAA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCAB;struct _RegionHandle*_TCAC;struct Cyc_Yystacktype*_TCAD;union Cyc_YYSTYPE*_TCAE;union Cyc_YYSTYPE*_TCAF;struct Cyc_Yystacktype*_TCB0;union Cyc_YYSTYPE*_TCB1;union Cyc_YYSTYPE*_TCB2;struct Cyc_Parse_Abstractdeclarator _TCB3;struct Cyc_List_List*_TCB4;struct _RegionHandle*_TCB5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCB6;struct _RegionHandle*_TCB7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCB8;struct _RegionHandle*_TCB9;struct Cyc_Yystacktype*_TCBA;union Cyc_YYSTYPE*_TCBB;union Cyc_YYSTYPE*_TCBC;struct Cyc_Parse_Abstractdeclarator _TCBD;struct Cyc_List_List*(*_TCBE)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_TCBF)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_TCC0;struct Cyc_Yystacktype _TCC1;struct Cyc_Yyltype _TCC2;unsigned _TCC3;unsigned _TCC4;struct Cyc_Yystacktype*_TCC5;union Cyc_YYSTYPE*_TCC6;union Cyc_YYSTYPE*_TCC7;struct Cyc_List_List*_TCC8;struct Cyc_List_List*_TCC9;struct Cyc_Parse_Abstractdeclarator _TCCA;struct Cyc_List_List*_TCCB;struct _RegionHandle*_TCCC;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_TCCD;struct _RegionHandle*_TCCE;struct Cyc_Yystacktype*_TCCF;struct Cyc_Yystacktype _TCD0;struct Cyc_Yyltype _TCD1;unsigned _TCD2;struct Cyc_Yystacktype*_TCD3;union Cyc_YYSTYPE*_TCD4;union Cyc_YYSTYPE*_TCD5;struct Cyc_Parse_Abstractdeclarator _TCD6;struct Cyc_Parse_Abstractdeclarator _TCD7;struct Cyc_List_List*_TCD8;struct _RegionHandle*_TCD9;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_TCDA;struct _RegionHandle*_TCDB;struct Cyc_Yystacktype*_TCDC;struct Cyc_Yystacktype _TCDD;struct Cyc_Yyltype _TCDE;unsigned _TCDF;struct Cyc_Yystacktype*_TCE0;union Cyc_YYSTYPE*_TCE1;union Cyc_YYSTYPE*_TCE2;struct Cyc_Yystacktype*_TCE3;union Cyc_YYSTYPE*_TCE4;union Cyc_YYSTYPE*_TCE5;struct Cyc_Parse_Abstractdeclarator _TCE6;struct _tuple21 _TCE7;struct Cyc_Yystacktype*_TCE8;union Cyc_YYSTYPE*_TCE9;union Cyc_YYSTYPE*_TCEA;struct _tuple21 _TCEB;struct Cyc_Yystacktype*_TCEC;union Cyc_YYSTYPE*_TCED;union Cyc_YYSTYPE*_TCEE;struct _tuple21 _TCEF;struct Cyc_Yystacktype*_TCF0;union Cyc_YYSTYPE*_TCF1;union Cyc_YYSTYPE*_TCF2;struct _tuple21 _TCF3;struct Cyc_Yystacktype*_TCF4;union Cyc_YYSTYPE*_TCF5;union Cyc_YYSTYPE*_TCF6;struct _tuple21 _TCF7;struct Cyc_Yystacktype*_TCF8;union Cyc_YYSTYPE*_TCF9;union Cyc_YYSTYPE*_TCFA;struct Cyc_Yystacktype*_TCFB;union Cyc_YYSTYPE*_TCFC;union Cyc_YYSTYPE*_TCFD;struct _tuple21 _TCFE;struct _tuple21 _TCFF;struct _tuple21 _TD00;struct Cyc_Yystacktype*_TD01;union Cyc_YYSTYPE*_TD02;union Cyc_YYSTYPE*_TD03;struct Cyc_Yystacktype*_TD04;union Cyc_YYSTYPE*_TD05;union Cyc_YYSTYPE*_TD06;struct _tuple21 _TD07;struct _tuple21 _TD08;struct _tuple21 _TD09;struct Cyc_Yystacktype*_TD0A;union Cyc_YYSTYPE*_TD0B;union Cyc_YYSTYPE*_TD0C;struct Cyc_Yystacktype*_TD0D;union Cyc_YYSTYPE*_TD0E;union Cyc_YYSTYPE*_TD0F;struct _tuple21 _TD10;struct _tuple21 _TD11;struct _tuple21 _TD12;struct Cyc_Yystacktype*_TD13;union Cyc_YYSTYPE*_TD14;union Cyc_YYSTYPE*_TD15;struct Cyc_Yystacktype*_TD16;union Cyc_YYSTYPE*_TD17;union Cyc_YYSTYPE*_TD18;struct _tuple21 _TD19;struct _tuple21 _TD1A;struct _tuple21 _TD1B;struct Cyc_Yystacktype*_TD1C;struct Cyc_Yystacktype _TD1D;struct Cyc_Yystacktype*_TD1E;struct Cyc_Yystacktype _TD1F;struct Cyc_Yystacktype*_TD20;struct Cyc_Yystacktype _TD21;struct Cyc_Yystacktype*_TD22;struct Cyc_Yystacktype _TD23;struct Cyc_Yystacktype*_TD24;struct Cyc_Yystacktype _TD25;struct Cyc_Yystacktype*_TD26;struct Cyc_Yystacktype _TD27;struct Cyc_Yystacktype*_TD28;struct Cyc_Yystacktype _TD29;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD2A;struct _fat_ptr*_TD2B;struct Cyc_Yystacktype*_TD2C;union Cyc_YYSTYPE*_TD2D;union Cyc_YYSTYPE*_TD2E;struct Cyc_Yystacktype*_TD2F;union Cyc_YYSTYPE*_TD30;union Cyc_YYSTYPE*_TD31;void*_TD32;struct Cyc_Yystacktype*_TD33;struct Cyc_Yystacktype _TD34;struct Cyc_Yyltype _TD35;unsigned _TD36;unsigned _TD37;struct Cyc_Absyn_Stmt*_TD38;struct Cyc_Yystacktype*_TD39;struct Cyc_Yystacktype _TD3A;struct Cyc_Yyltype _TD3B;unsigned _TD3C;unsigned _TD3D;struct Cyc_Absyn_Stmt*_TD3E;struct Cyc_Yystacktype*_TD3F;union Cyc_YYSTYPE*_TD40;union Cyc_YYSTYPE*_TD41;struct Cyc_Absyn_Exp*_TD42;struct Cyc_Yystacktype*_TD43;struct Cyc_Yystacktype _TD44;struct Cyc_Yyltype _TD45;unsigned _TD46;unsigned _TD47;struct Cyc_Absyn_Stmt*_TD48;struct Cyc_Yystacktype*_TD49;struct Cyc_Yystacktype _TD4A;struct Cyc_Yyltype _TD4B;unsigned _TD4C;unsigned _TD4D;struct Cyc_Absyn_Stmt*_TD4E;struct Cyc_Yystacktype*_TD4F;struct Cyc_Yystacktype _TD50;struct Cyc_Yystacktype*_TD51;struct Cyc_Yystacktype _TD52;struct Cyc_Yyltype _TD53;unsigned _TD54;unsigned _TD55;struct Cyc_Absyn_Stmt*_TD56;struct Cyc_Yystacktype*_TD57;struct Cyc_Yystacktype _TD58;struct Cyc_Yystacktype*_TD59;union Cyc_YYSTYPE*_TD5A;union Cyc_YYSTYPE*_TD5B;struct Cyc_List_List*_TD5C;struct Cyc_Yystacktype*_TD5D;struct Cyc_Yystacktype _TD5E;struct Cyc_Yyltype _TD5F;unsigned _TD60;unsigned _TD61;struct Cyc_Absyn_Stmt*_TD62;struct Cyc_Absyn_Stmt*_TD63;struct Cyc_Yystacktype*_TD64;union Cyc_YYSTYPE*_TD65;union Cyc_YYSTYPE*_TD66;struct Cyc_List_List*_TD67;struct Cyc_Yystacktype*_TD68;union Cyc_YYSTYPE*_TD69;union Cyc_YYSTYPE*_TD6A;struct Cyc_Absyn_Stmt*_TD6B;struct Cyc_Absyn_Stmt*_TD6C;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD6D;struct _fat_ptr*_TD6E;struct Cyc_Yystacktype*_TD6F;union Cyc_YYSTYPE*_TD70;union Cyc_YYSTYPE*_TD71;struct Cyc_Yystacktype*_TD72;union Cyc_YYSTYPE*_TD73;union Cyc_YYSTYPE*_TD74;struct Cyc_List_List*_TD75;struct Cyc_Absyn_Stmt*_TD76;void*_TD77;struct Cyc_Yystacktype*_TD78;struct Cyc_Yystacktype _TD79;struct Cyc_Yyltype _TD7A;unsigned _TD7B;unsigned _TD7C;struct Cyc_Absyn_Stmt*_TD7D;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD7E;struct _fat_ptr*_TD7F;struct Cyc_Yystacktype*_TD80;union Cyc_YYSTYPE*_TD81;union Cyc_YYSTYPE*_TD82;struct Cyc_Yystacktype*_TD83;union Cyc_YYSTYPE*_TD84;union Cyc_YYSTYPE*_TD85;struct Cyc_List_List*_TD86;struct Cyc_Yystacktype*_TD87;union Cyc_YYSTYPE*_TD88;union Cyc_YYSTYPE*_TD89;struct Cyc_Absyn_Stmt*_TD8A;void*_TD8B;struct Cyc_Yystacktype*_TD8C;struct Cyc_Yystacktype _TD8D;struct Cyc_Yyltype _TD8E;unsigned _TD8F;unsigned _TD90;struct Cyc_Absyn_Stmt*_TD91;struct Cyc_Yystacktype*_TD92;struct Cyc_Yystacktype _TD93;struct Cyc_Yystacktype*_TD94;union Cyc_YYSTYPE*_TD95;union Cyc_YYSTYPE*_TD96;struct Cyc_Absyn_Stmt*_TD97;struct Cyc_Yystacktype*_TD98;union Cyc_YYSTYPE*_TD99;union Cyc_YYSTYPE*_TD9A;struct Cyc_Absyn_Stmt*_TD9B;struct Cyc_Yystacktype*_TD9C;struct Cyc_Yystacktype _TD9D;struct Cyc_Yyltype _TD9E;unsigned _TD9F;unsigned _TDA0;struct Cyc_Absyn_Stmt*_TDA1;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDA2;struct Cyc_Yystacktype*_TDA3;union Cyc_YYSTYPE*_TDA4;union Cyc_YYSTYPE*_TDA5;void*_TDA6;struct Cyc_Yystacktype*_TDA7;struct Cyc_Yystacktype _TDA8;struct Cyc_Yyltype _TDA9;unsigned _TDAA;unsigned _TDAB;struct Cyc_Absyn_Decl*_TDAC;struct Cyc_Absyn_Stmt*_TDAD;struct Cyc_Absyn_Stmt*_TDAE;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDAF;struct Cyc_Yystacktype*_TDB0;union Cyc_YYSTYPE*_TDB1;union Cyc_YYSTYPE*_TDB2;void*_TDB3;struct Cyc_Yystacktype*_TDB4;struct Cyc_Yystacktype _TDB5;struct Cyc_Yyltype _TDB6;unsigned _TDB7;unsigned _TDB8;struct Cyc_Absyn_Decl*_TDB9;struct Cyc_Yystacktype*_TDBA;union Cyc_YYSTYPE*_TDBB;union Cyc_YYSTYPE*_TDBC;struct Cyc_Absyn_Stmt*_TDBD;struct Cyc_Absyn_Stmt*_TDBE;struct Cyc_Yystacktype*_TDBF;union Cyc_YYSTYPE*_TDC0;union Cyc_YYSTYPE*_TDC1;struct Cyc_Absyn_Exp*_TDC2;struct Cyc_Yystacktype*_TDC3;union Cyc_YYSTYPE*_TDC4;union Cyc_YYSTYPE*_TDC5;struct Cyc_Absyn_Stmt*_TDC6;struct Cyc_Absyn_Stmt*_TDC7;struct Cyc_Yystacktype*_TDC8;struct Cyc_Yystacktype _TDC9;struct Cyc_Yyltype _TDCA;unsigned _TDCB;unsigned _TDCC;struct Cyc_Absyn_Stmt*_TDCD;struct Cyc_Yystacktype*_TDCE;union Cyc_YYSTYPE*_TDCF;union Cyc_YYSTYPE*_TDD0;struct Cyc_Absyn_Exp*_TDD1;struct Cyc_Yystacktype*_TDD2;union Cyc_YYSTYPE*_TDD3;union Cyc_YYSTYPE*_TDD4;struct Cyc_Absyn_Stmt*_TDD5;struct Cyc_Yystacktype*_TDD6;union Cyc_YYSTYPE*_TDD7;union Cyc_YYSTYPE*_TDD8;struct Cyc_Absyn_Stmt*_TDD9;struct Cyc_Yystacktype*_TDDA;struct Cyc_Yystacktype _TDDB;struct Cyc_Yyltype _TDDC;unsigned _TDDD;unsigned _TDDE;struct Cyc_Absyn_Stmt*_TDDF;struct Cyc_Yystacktype*_TDE0;union Cyc_YYSTYPE*_TDE1;union Cyc_YYSTYPE*_TDE2;struct Cyc_Absyn_Exp*_TDE3;struct Cyc_Yystacktype*_TDE4;union Cyc_YYSTYPE*_TDE5;union Cyc_YYSTYPE*_TDE6;struct Cyc_List_List*_TDE7;struct Cyc_Yystacktype*_TDE8;struct Cyc_Yystacktype _TDE9;struct Cyc_Yyltype _TDEA;unsigned _TDEB;unsigned _TDEC;struct Cyc_Absyn_Stmt*_TDED;struct Cyc_Yystacktype*_TDEE;union Cyc_YYSTYPE*_TDEF;union Cyc_YYSTYPE*_TDF0;struct _tuple0*_TDF1;struct Cyc_Yystacktype*_TDF2;struct Cyc_Yystacktype _TDF3;struct Cyc_Yyltype _TDF4;unsigned _TDF5;unsigned _TDF6;struct Cyc_Absyn_Exp*_TDF7;struct Cyc_Yystacktype*_TDF8;union Cyc_YYSTYPE*_TDF9;union Cyc_YYSTYPE*_TDFA;struct Cyc_List_List*_TDFB;struct Cyc_Yystacktype*_TDFC;struct Cyc_Yystacktype _TDFD;struct Cyc_Yyltype _TDFE;unsigned _TDFF;unsigned _TE00;struct Cyc_Absyn_Stmt*_TE01;struct Cyc_Yystacktype*_TE02;union Cyc_YYSTYPE*_TE03;union Cyc_YYSTYPE*_TE04;struct Cyc_List_List*_TE05;struct Cyc_Yystacktype*_TE06;struct Cyc_Yystacktype _TE07;struct Cyc_Yyltype _TE08;unsigned _TE09;unsigned _TE0A;struct Cyc_Absyn_Exp*_TE0B;struct Cyc_Yystacktype*_TE0C;union Cyc_YYSTYPE*_TE0D;union Cyc_YYSTYPE*_TE0E;struct Cyc_List_List*_TE0F;struct Cyc_Yystacktype*_TE10;struct Cyc_Yystacktype _TE11;struct Cyc_Yyltype _TE12;unsigned _TE13;unsigned _TE14;struct Cyc_Absyn_Stmt*_TE15;struct Cyc_Yystacktype*_TE16;union Cyc_YYSTYPE*_TE17;union Cyc_YYSTYPE*_TE18;struct Cyc_Absyn_Stmt*_TE19;struct Cyc_Yystacktype*_TE1A;union Cyc_YYSTYPE*_TE1B;union Cyc_YYSTYPE*_TE1C;struct Cyc_List_List*_TE1D;struct Cyc_Yystacktype*_TE1E;struct Cyc_Yystacktype _TE1F;struct Cyc_Yyltype _TE20;unsigned _TE21;unsigned _TE22;struct Cyc_Absyn_Stmt*_TE23;struct Cyc_List_List*_TE24;struct Cyc_Absyn_Switch_clause*_TE25;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE26;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE27;void*_TE28;struct Cyc_Yystacktype*_TE29;struct Cyc_Yystacktype _TE2A;struct Cyc_Yyltype _TE2B;unsigned _TE2C;unsigned _TE2D;struct Cyc_Yystacktype*_TE2E;union Cyc_YYSTYPE*_TE2F;union Cyc_YYSTYPE*_TE30;struct Cyc_Yystacktype*_TE31;struct Cyc_Yystacktype _TE32;struct Cyc_Yyltype _TE33;unsigned _TE34;struct Cyc_Yystacktype*_TE35;union Cyc_YYSTYPE*_TE36;union Cyc_YYSTYPE*_TE37;struct Cyc_List_List*_TE38;struct Cyc_Absyn_Switch_clause*_TE39;struct Cyc_Yystacktype*_TE3A;union Cyc_YYSTYPE*_TE3B;union Cyc_YYSTYPE*_TE3C;struct Cyc_Yystacktype*_TE3D;struct Cyc_Yystacktype _TE3E;struct Cyc_Yyltype _TE3F;unsigned _TE40;unsigned _TE41;struct Cyc_Yystacktype*_TE42;struct Cyc_Yystacktype _TE43;struct Cyc_Yyltype _TE44;unsigned _TE45;struct Cyc_Yystacktype*_TE46;union Cyc_YYSTYPE*_TE47;union Cyc_YYSTYPE*_TE48;struct Cyc_List_List*_TE49;struct Cyc_Absyn_Switch_clause*_TE4A;struct Cyc_Yystacktype*_TE4B;union Cyc_YYSTYPE*_TE4C;union Cyc_YYSTYPE*_TE4D;struct Cyc_Yystacktype*_TE4E;union Cyc_YYSTYPE*_TE4F;union Cyc_YYSTYPE*_TE50;struct Cyc_Yystacktype*_TE51;struct Cyc_Yystacktype _TE52;struct Cyc_Yyltype _TE53;unsigned _TE54;struct Cyc_Yystacktype*_TE55;union Cyc_YYSTYPE*_TE56;union Cyc_YYSTYPE*_TE57;struct Cyc_List_List*_TE58;struct Cyc_Absyn_Switch_clause*_TE59;struct Cyc_Yystacktype*_TE5A;union Cyc_YYSTYPE*_TE5B;union Cyc_YYSTYPE*_TE5C;struct Cyc_Yystacktype*_TE5D;union Cyc_YYSTYPE*_TE5E;union Cyc_YYSTYPE*_TE5F;struct Cyc_Yystacktype*_TE60;struct Cyc_Yystacktype _TE61;struct Cyc_Yyltype _TE62;unsigned _TE63;unsigned _TE64;struct Cyc_Yystacktype*_TE65;struct Cyc_Yystacktype _TE66;struct Cyc_Yyltype _TE67;unsigned _TE68;struct Cyc_Yystacktype*_TE69;union Cyc_YYSTYPE*_TE6A;union Cyc_YYSTYPE*_TE6B;struct Cyc_List_List*_TE6C;struct Cyc_Absyn_Switch_clause*_TE6D;struct Cyc_Yystacktype*_TE6E;union Cyc_YYSTYPE*_TE6F;union Cyc_YYSTYPE*_TE70;struct Cyc_Yystacktype*_TE71;union Cyc_YYSTYPE*_TE72;union Cyc_YYSTYPE*_TE73;struct Cyc_Yystacktype*_TE74;union Cyc_YYSTYPE*_TE75;union Cyc_YYSTYPE*_TE76;struct Cyc_Yystacktype*_TE77;struct Cyc_Yystacktype _TE78;struct Cyc_Yyltype _TE79;unsigned _TE7A;struct Cyc_Yystacktype*_TE7B;union Cyc_YYSTYPE*_TE7C;union Cyc_YYSTYPE*_TE7D;struct Cyc_Yystacktype*_TE7E;union Cyc_YYSTYPE*_TE7F;union Cyc_YYSTYPE*_TE80;struct Cyc_Absyn_Exp*_TE81;struct Cyc_Yystacktype*_TE82;union Cyc_YYSTYPE*_TE83;union Cyc_YYSTYPE*_TE84;struct Cyc_Absyn_Stmt*_TE85;struct Cyc_Yystacktype*_TE86;struct Cyc_Yystacktype _TE87;struct Cyc_Yyltype _TE88;unsigned _TE89;unsigned _TE8A;struct Cyc_Absyn_Stmt*_TE8B;struct Cyc_Yystacktype*_TE8C;union Cyc_YYSTYPE*_TE8D;union Cyc_YYSTYPE*_TE8E;struct Cyc_Absyn_Stmt*_TE8F;struct Cyc_Yystacktype*_TE90;union Cyc_YYSTYPE*_TE91;union Cyc_YYSTYPE*_TE92;struct Cyc_Absyn_Exp*_TE93;struct Cyc_Yystacktype*_TE94;struct Cyc_Yystacktype _TE95;struct Cyc_Yyltype _TE96;unsigned _TE97;unsigned _TE98;struct Cyc_Absyn_Stmt*_TE99;struct Cyc_Yystacktype*_TE9A;union Cyc_YYSTYPE*_TE9B;union Cyc_YYSTYPE*_TE9C;struct Cyc_Absyn_Exp*_TE9D;struct Cyc_Yystacktype*_TE9E;union Cyc_YYSTYPE*_TE9F;union Cyc_YYSTYPE*_TEA0;struct Cyc_Absyn_Exp*_TEA1;struct Cyc_Yystacktype*_TEA2;union Cyc_YYSTYPE*_TEA3;union Cyc_YYSTYPE*_TEA4;struct Cyc_Absyn_Exp*_TEA5;struct Cyc_Yystacktype*_TEA6;union Cyc_YYSTYPE*_TEA7;union Cyc_YYSTYPE*_TEA8;struct Cyc_Absyn_Stmt*_TEA9;struct Cyc_Yystacktype*_TEAA;struct Cyc_Yystacktype _TEAB;struct Cyc_Yyltype _TEAC;unsigned _TEAD;unsigned _TEAE;struct Cyc_Absyn_Stmt*_TEAF;struct Cyc_Absyn_Exp*_TEB0;struct Cyc_Yystacktype*_TEB1;union Cyc_YYSTYPE*_TEB2;union Cyc_YYSTYPE*_TEB3;struct Cyc_Absyn_Exp*_TEB4;struct Cyc_Yystacktype*_TEB5;union Cyc_YYSTYPE*_TEB6;union Cyc_YYSTYPE*_TEB7;struct Cyc_Absyn_Exp*_TEB8;struct Cyc_Yystacktype*_TEB9;union Cyc_YYSTYPE*_TEBA;union Cyc_YYSTYPE*_TEBB;struct Cyc_Absyn_Stmt*_TEBC;struct Cyc_Yystacktype*_TEBD;struct Cyc_Yystacktype _TEBE;struct Cyc_Yyltype _TEBF;unsigned _TEC0;unsigned _TEC1;struct Cyc_Yystacktype*_TEC2;union Cyc_YYSTYPE*_TEC3;union Cyc_YYSTYPE*_TEC4;struct Cyc_List_List*_TEC5;struct Cyc_Absyn_Stmt*_TEC6;struct Cyc_Absyn_Stmt*_TEC7;struct Cyc_Absyn_Exp*_TEC8;struct Cyc_Yystacktype*_TEC9;struct Cyc_Yystacktype _TECA;struct _fat_ptr*_TECB;struct Cyc_Yystacktype*_TECC;union Cyc_YYSTYPE*_TECD;union Cyc_YYSTYPE*_TECE;struct Cyc_Yystacktype*_TECF;struct Cyc_Yystacktype _TED0;struct Cyc_Yyltype _TED1;unsigned _TED2;unsigned _TED3;struct Cyc_Absyn_Stmt*_TED4;struct Cyc_Yystacktype*_TED5;struct Cyc_Yystacktype _TED6;struct Cyc_Yyltype _TED7;unsigned _TED8;unsigned _TED9;struct Cyc_Absyn_Stmt*_TEDA;struct Cyc_Yystacktype*_TEDB;struct Cyc_Yystacktype _TEDC;struct Cyc_Yyltype _TEDD;unsigned _TEDE;unsigned _TEDF;struct Cyc_Absyn_Stmt*_TEE0;struct Cyc_Yystacktype*_TEE1;struct Cyc_Yystacktype _TEE2;struct Cyc_Yyltype _TEE3;unsigned _TEE4;unsigned _TEE5;struct Cyc_Absyn_Stmt*_TEE6;struct Cyc_Yystacktype*_TEE7;union Cyc_YYSTYPE*_TEE8;union Cyc_YYSTYPE*_TEE9;struct Cyc_Absyn_Exp*_TEEA;struct Cyc_Yystacktype*_TEEB;struct Cyc_Yystacktype _TEEC;struct Cyc_Yyltype _TEED;unsigned _TEEE;unsigned _TEEF;struct Cyc_Absyn_Stmt*_TEF0;struct Cyc_Yystacktype*_TEF1;struct Cyc_Yystacktype _TEF2;struct Cyc_Yyltype _TEF3;unsigned _TEF4;unsigned _TEF5;struct Cyc_Absyn_Stmt*_TEF6;struct Cyc_Yystacktype*_TEF7;struct Cyc_Yystacktype _TEF8;struct Cyc_Yyltype _TEF9;unsigned _TEFA;unsigned _TEFB;struct Cyc_Absyn_Stmt*_TEFC;struct Cyc_Yystacktype*_TEFD;union Cyc_YYSTYPE*_TEFE;union Cyc_YYSTYPE*_TEFF;struct Cyc_List_List*_TF00;struct Cyc_Yystacktype*_TF01;struct Cyc_Yystacktype _TF02;struct Cyc_Yyltype _TF03;unsigned _TF04;unsigned _TF05;struct Cyc_Absyn_Stmt*_TF06;struct Cyc_Yystacktype*_TF07;struct Cyc_Yystacktype _TF08;struct Cyc_Yystacktype*_TF09;struct Cyc_Yystacktype _TF0A;struct Cyc_Yystacktype*_TF0B;union Cyc_YYSTYPE*_TF0C;union Cyc_YYSTYPE*_TF0D;struct Cyc_Absyn_Pat*_TF0E;struct Cyc_Absyn_Exp*_TF0F;struct Cyc_Yystacktype*_TF10;union Cyc_YYSTYPE*_TF11;union Cyc_YYSTYPE*_TF12;struct Cyc_Absyn_Exp*_TF13;struct Cyc_Yystacktype*_TF14;union Cyc_YYSTYPE*_TF15;union Cyc_YYSTYPE*_TF16;struct Cyc_Absyn_Exp*_TF17;struct Cyc_Yystacktype*_TF18;struct Cyc_Yystacktype _TF19;struct Cyc_Yyltype _TF1A;unsigned _TF1B;unsigned _TF1C;struct Cyc_Absyn_Exp*_TF1D;struct Cyc_Absyn_Pat*_TF1E;struct Cyc_Yystacktype*_TF1F;struct Cyc_Yystacktype _TF20;struct Cyc_Yystacktype*_TF21;union Cyc_YYSTYPE*_TF22;union Cyc_YYSTYPE*_TF23;struct Cyc_Absyn_Pat*_TF24;struct Cyc_Absyn_Exp*_TF25;struct Cyc_Yystacktype*_TF26;union Cyc_YYSTYPE*_TF27;union Cyc_YYSTYPE*_TF28;struct Cyc_Absyn_Exp*_TF29;struct Cyc_Yystacktype*_TF2A;struct Cyc_Yystacktype _TF2B;struct Cyc_Yyltype _TF2C;unsigned _TF2D;unsigned _TF2E;struct Cyc_Absyn_Exp*_TF2F;struct Cyc_Absyn_Pat*_TF30;struct Cyc_Yystacktype*_TF31;struct Cyc_Yystacktype _TF32;struct Cyc_Yystacktype*_TF33;union Cyc_YYSTYPE*_TF34;union Cyc_YYSTYPE*_TF35;struct Cyc_Absyn_Pat*_TF36;struct Cyc_Absyn_Exp*_TF37;struct Cyc_Yystacktype*_TF38;union Cyc_YYSTYPE*_TF39;union Cyc_YYSTYPE*_TF3A;struct Cyc_Absyn_Exp*_TF3B;struct Cyc_Yystacktype*_TF3C;struct Cyc_Yystacktype _TF3D;struct Cyc_Yyltype _TF3E;unsigned _TF3F;unsigned _TF40;struct Cyc_Absyn_Exp*_TF41;struct Cyc_Absyn_Pat*_TF42;struct Cyc_Yystacktype*_TF43;struct Cyc_Yystacktype _TF44;struct Cyc_Yystacktype*_TF45;union Cyc_YYSTYPE*_TF46;union Cyc_YYSTYPE*_TF47;struct Cyc_Absyn_Pat*_TF48;struct Cyc_Absyn_Exp*_TF49;struct Cyc_Yystacktype*_TF4A;union Cyc_YYSTYPE*_TF4B;union Cyc_YYSTYPE*_TF4C;struct Cyc_Absyn_Exp*_TF4D;struct Cyc_Yystacktype*_TF4E;struct Cyc_Yystacktype _TF4F;struct Cyc_Yyltype _TF50;unsigned _TF51;unsigned _TF52;struct Cyc_Absyn_Exp*_TF53;struct Cyc_Absyn_Pat*_TF54;struct Cyc_Yystacktype*_TF55;struct Cyc_Yystacktype _TF56;struct Cyc_Yystacktype*_TF57;union Cyc_YYSTYPE*_TF58;union Cyc_YYSTYPE*_TF59;struct Cyc_Absyn_Pat*_TF5A;struct Cyc_Absyn_Exp*_TF5B;struct Cyc_Yystacktype*_TF5C;union Cyc_YYSTYPE*_TF5D;union Cyc_YYSTYPE*_TF5E;struct Cyc_Absyn_Exp*_TF5F;struct Cyc_Yystacktype*_TF60;struct Cyc_Yystacktype _TF61;struct Cyc_Yyltype _TF62;unsigned _TF63;unsigned _TF64;struct Cyc_Absyn_Exp*_TF65;struct Cyc_Absyn_Pat*_TF66;struct Cyc_Yystacktype*_TF67;struct Cyc_Yystacktype _TF68;struct Cyc_Yystacktype*_TF69;union Cyc_YYSTYPE*_TF6A;union Cyc_YYSTYPE*_TF6B;struct Cyc_Absyn_Pat*_TF6C;struct Cyc_Absyn_Exp*_TF6D;struct Cyc_Yystacktype*_TF6E;union Cyc_YYSTYPE*_TF6F;union Cyc_YYSTYPE*_TF70;struct Cyc_Absyn_Exp*_TF71;struct Cyc_Yystacktype*_TF72;struct Cyc_Yystacktype _TF73;struct Cyc_Yyltype _TF74;unsigned _TF75;unsigned _TF76;struct Cyc_Absyn_Exp*_TF77;struct Cyc_Absyn_Pat*_TF78;struct Cyc_Yystacktype*_TF79;struct Cyc_Yystacktype _TF7A;struct Cyc_Yystacktype*_TF7B;union Cyc_YYSTYPE*_TF7C;union Cyc_YYSTYPE*_TF7D;struct Cyc_Absyn_Exp*(*_TF7E)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TF7F;union Cyc_YYSTYPE*_TF80;union Cyc_YYSTYPE*_TF81;struct Cyc_Absyn_Pat*_TF82;struct Cyc_Absyn_Exp*_TF83;struct Cyc_Yystacktype*_TF84;union Cyc_YYSTYPE*_TF85;union Cyc_YYSTYPE*_TF86;struct Cyc_Absyn_Exp*_TF87;struct Cyc_Yystacktype*_TF88;struct Cyc_Yystacktype _TF89;struct Cyc_Yyltype _TF8A;unsigned _TF8B;unsigned _TF8C;struct Cyc_Absyn_Exp*_TF8D;struct Cyc_Absyn_Pat*_TF8E;struct Cyc_Yystacktype*_TF8F;struct Cyc_Yystacktype _TF90;struct Cyc_Yystacktype*_TF91;union Cyc_YYSTYPE*_TF92;union Cyc_YYSTYPE*_TF93;struct Cyc_Absyn_Exp*(*_TF94)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TF95;union Cyc_YYSTYPE*_TF96;union Cyc_YYSTYPE*_TF97;struct Cyc_Absyn_Pat*_TF98;struct Cyc_Absyn_Exp*_TF99;struct Cyc_Yystacktype*_TF9A;union Cyc_YYSTYPE*_TF9B;union Cyc_YYSTYPE*_TF9C;struct Cyc_Absyn_Exp*_TF9D;struct Cyc_Yystacktype*_TF9E;struct Cyc_Yystacktype _TF9F;struct Cyc_Yyltype _TFA0;unsigned _TFA1;unsigned _TFA2;struct Cyc_Absyn_Exp*_TFA3;struct Cyc_Absyn_Pat*_TFA4;struct Cyc_Yystacktype*_TFA5;struct Cyc_Yystacktype _TFA6;struct Cyc_Yystacktype*_TFA7;union Cyc_YYSTYPE*_TFA8;union Cyc_YYSTYPE*_TFA9;struct Cyc_Absyn_Pat*_TFAA;struct Cyc_Absyn_Exp*_TFAB;struct Cyc_Yystacktype*_TFAC;union Cyc_YYSTYPE*_TFAD;union Cyc_YYSTYPE*_TFAE;struct Cyc_Absyn_Exp*_TFAF;struct Cyc_Yystacktype*_TFB0;struct Cyc_Yystacktype _TFB1;struct Cyc_Yyltype _TFB2;unsigned _TFB3;unsigned _TFB4;struct Cyc_Absyn_Exp*_TFB5;struct Cyc_Absyn_Pat*_TFB6;struct Cyc_Yystacktype*_TFB7;union Cyc_YYSTYPE*_TFB8;union Cyc_YYSTYPE*_TFB9;struct Cyc_Absyn_Pat*_TFBA;struct Cyc_Absyn_Exp*_TFBB;struct Cyc_Yystacktype*_TFBC;union Cyc_YYSTYPE*_TFBD;union Cyc_YYSTYPE*_TFBE;struct Cyc_Absyn_Exp*_TFBF;struct Cyc_Yystacktype*_TFC0;struct Cyc_Yystacktype _TFC1;struct Cyc_Yyltype _TFC2;unsigned _TFC3;unsigned _TFC4;struct Cyc_Absyn_Exp*_TFC5;struct Cyc_Absyn_Pat*_TFC6;struct Cyc_Yystacktype*_TFC7;struct Cyc_Yystacktype _TFC8;struct Cyc_Yystacktype*_TFC9;union Cyc_YYSTYPE*_TFCA;union Cyc_YYSTYPE*_TFCB;enum Cyc_Absyn_Primop _TFCC;struct Cyc_Yystacktype*_TFCD;union Cyc_YYSTYPE*_TFCE;union Cyc_YYSTYPE*_TFCF;struct Cyc_Absyn_Pat*_TFD0;struct Cyc_Absyn_Exp*_TFD1;struct Cyc_Yystacktype*_TFD2;union Cyc_YYSTYPE*_TFD3;union Cyc_YYSTYPE*_TFD4;struct Cyc_Absyn_Exp*_TFD5;struct Cyc_Yystacktype*_TFD6;struct Cyc_Yystacktype _TFD7;struct Cyc_Yyltype _TFD8;unsigned _TFD9;unsigned _TFDA;struct Cyc_Absyn_Exp*_TFDB;struct Cyc_Absyn_Pat*_TFDC;struct Cyc_Yystacktype*_TFDD;struct Cyc_Yystacktype _TFDE;struct Cyc_Yystacktype*_TFDF;union Cyc_YYSTYPE*_TFE0;union Cyc_YYSTYPE*_TFE1;enum Cyc_Absyn_Primop _TFE2;struct Cyc_Yystacktype*_TFE3;union Cyc_YYSTYPE*_TFE4;union Cyc_YYSTYPE*_TFE5;struct Cyc_Absyn_Pat*_TFE6;struct Cyc_Absyn_Exp*_TFE7;struct Cyc_Yystacktype*_TFE8;union Cyc_YYSTYPE*_TFE9;union Cyc_YYSTYPE*_TFEA;struct Cyc_Absyn_Exp*_TFEB;struct Cyc_Yystacktype*_TFEC;struct Cyc_Yystacktype _TFED;struct Cyc_Yyltype _TFEE;unsigned _TFEF;unsigned _TFF0;struct Cyc_Absyn_Exp*_TFF1;struct Cyc_Absyn_Pat*_TFF2;struct Cyc_Yystacktype*_TFF3;struct Cyc_Yystacktype _TFF4;struct Cyc_Yystacktype*_TFF5;union Cyc_YYSTYPE*_TFF6;union Cyc_YYSTYPE*_TFF7;struct _tuple8*_TFF8;struct Cyc_Yystacktype*_TFF9;struct Cyc_Yystacktype _TFFA;struct Cyc_Yyltype _TFFB;unsigned _TFFC;unsigned _TFFD;void*_TFFE;struct Cyc_Yystacktype*_TFFF;union Cyc_YYSTYPE*_T1000;union Cyc_YYSTYPE*_T1001;struct Cyc_Absyn_Exp*_T1002;struct Cyc_Yystacktype*_T1003;struct Cyc_Yystacktype _T1004;struct Cyc_Yyltype _T1005;unsigned _T1006;unsigned _T1007;struct Cyc_Absyn_Exp*_T1008;struct Cyc_Absyn_Pat*_T1009;struct Cyc_Yystacktype*_T100A;struct Cyc_Yystacktype _T100B;struct Cyc_Yystacktype*_T100C;union Cyc_YYSTYPE*_T100D;union Cyc_YYSTYPE*_T100E;enum Cyc_Absyn_Primop _T100F;struct Cyc_Yystacktype*_T1010;union Cyc_YYSTYPE*_T1011;union Cyc_YYSTYPE*_T1012;struct Cyc_Absyn_Exp*_T1013;struct Cyc_Yystacktype*_T1014;struct Cyc_Yystacktype _T1015;struct Cyc_Yyltype _T1016;unsigned _T1017;unsigned _T1018;struct Cyc_Absyn_Exp*_T1019;struct Cyc_Absyn_Pat*_T101A;struct Cyc_Yystacktype*_T101B;union Cyc_YYSTYPE*_T101C;union Cyc_YYSTYPE*_T101D;struct _tuple8*_T101E;struct Cyc_Yystacktype*_T101F;struct Cyc_Yystacktype _T1020;struct Cyc_Yyltype _T1021;unsigned _T1022;unsigned _T1023;void*_T1024;struct Cyc_Yystacktype*_T1025;struct Cyc_Yystacktype _T1026;struct Cyc_Yyltype _T1027;unsigned _T1028;unsigned _T1029;struct Cyc_Absyn_Exp*_T102A;struct Cyc_Absyn_Pat*_T102B;struct Cyc_Yystacktype*_T102C;union Cyc_YYSTYPE*_T102D;union Cyc_YYSTYPE*_T102E;struct Cyc_Absyn_Exp*_T102F;struct Cyc_Yystacktype*_T1030;struct Cyc_Yystacktype _T1031;struct Cyc_Yyltype _T1032;unsigned _T1033;unsigned _T1034;struct Cyc_Absyn_Exp*_T1035;struct Cyc_Absyn_Pat*_T1036;struct Cyc_Yystacktype*_T1037;union Cyc_YYSTYPE*_T1038;union Cyc_YYSTYPE*_T1039;struct _tuple8*_T103A;struct _tuple8 _T103B;void*_T103C;struct Cyc_Yystacktype*_T103D;union Cyc_YYSTYPE*_T103E;union Cyc_YYSTYPE*_T103F;struct Cyc_List_List*_T1040;struct Cyc_List_List*_T1041;struct Cyc_Yystacktype*_T1042;struct Cyc_Yystacktype _T1043;struct Cyc_Yyltype _T1044;unsigned _T1045;unsigned _T1046;struct Cyc_Absyn_Exp*_T1047;struct Cyc_Absyn_Pat*_T1048;struct Cyc_Yystacktype*_T1049;struct Cyc_Yystacktype _T104A;struct Cyc_Yystacktype*_T104B;struct Cyc_Yystacktype _T104C;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T104D;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T104E;void*_T104F;struct Cyc_Yystacktype*_T1050;struct Cyc_Yystacktype _T1051;struct Cyc_Yyltype _T1052;unsigned _T1053;unsigned _T1054;struct Cyc_Absyn_Pat*_T1055;struct Cyc_Yystacktype*_T1056;union Cyc_YYSTYPE*_T1057;union Cyc_YYSTYPE*_T1058;struct Cyc_Absyn_Exp*_T1059;struct Cyc_Absyn_Pat*_T105A;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T105B;struct Cyc_Yystacktype*_T105C;union Cyc_YYSTYPE*_T105D;union Cyc_YYSTYPE*_T105E;void*_T105F;struct Cyc_Yystacktype*_T1060;struct Cyc_Yystacktype _T1061;struct Cyc_Yyltype _T1062;unsigned _T1063;unsigned _T1064;struct Cyc_Absyn_Pat*_T1065;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1066;struct Cyc_Yystacktype*_T1067;union Cyc_YYSTYPE*_T1068;union Cyc_YYSTYPE*_T1069;void*_T106A;struct Cyc_Yystacktype*_T106B;struct Cyc_Yystacktype _T106C;struct Cyc_Yyltype _T106D;unsigned _T106E;unsigned _T106F;struct Cyc_Absyn_Pat*_T1070;struct Cyc_Yystacktype*_T1071;union Cyc_YYSTYPE*_T1072;union Cyc_YYSTYPE*_T1073;struct Cyc_Absyn_Exp*_T1074;int*_T1075;int _T1076;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1077;union Cyc_Absyn_Cnst _T1078;struct _union_Cnst_LongLong_c _T1079;unsigned _T107A;union Cyc_Absyn_Cnst _T107B;struct _union_Cnst_Char_c _T107C;struct _tuple3 _T107D;union Cyc_Absyn_Cnst _T107E;struct _union_Cnst_Char_c _T107F;struct _tuple3 _T1080;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T1081;void*_T1082;struct Cyc_Absyn_Exp*_T1083;unsigned _T1084;struct Cyc_Absyn_Pat*_T1085;union Cyc_Absyn_Cnst _T1086;struct _union_Cnst_Short_c _T1087;struct _tuple4 _T1088;union Cyc_Absyn_Cnst _T1089;struct _union_Cnst_Short_c _T108A;struct _tuple4 _T108B;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T108C;short _T108D;void*_T108E;struct Cyc_Absyn_Exp*_T108F;unsigned _T1090;struct Cyc_Absyn_Pat*_T1091;union Cyc_Absyn_Cnst _T1092;struct _union_Cnst_Int_c _T1093;struct _tuple5 _T1094;union Cyc_Absyn_Cnst _T1095;struct _union_Cnst_Int_c _T1096;struct _tuple5 _T1097;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1098;void*_T1099;struct Cyc_Absyn_Exp*_T109A;unsigned _T109B;struct Cyc_Absyn_Pat*_T109C;union Cyc_Absyn_Cnst _T109D;struct _union_Cnst_Float_c _T109E;struct _tuple7 _T109F;union Cyc_Absyn_Cnst _T10A0;struct _union_Cnst_Float_c _T10A1;struct _tuple7 _T10A2;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T10A3;void*_T10A4;struct Cyc_Absyn_Exp*_T10A5;unsigned _T10A6;struct Cyc_Absyn_Pat*_T10A7;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10A8;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10A9;void*_T10AA;struct Cyc_Absyn_Exp*_T10AB;unsigned _T10AC;struct Cyc_Absyn_Pat*_T10AD;struct Cyc_Yystacktype*_T10AE;struct Cyc_Yystacktype _T10AF;struct Cyc_Yyltype _T10B0;unsigned _T10B1;unsigned _T10B2;struct _fat_ptr _T10B3;struct _fat_ptr _T10B4;struct Cyc_Yystacktype*_T10B5;struct Cyc_Yystacktype _T10B6;struct Cyc_Yyltype _T10B7;unsigned _T10B8;unsigned _T10B9;struct _fat_ptr _T10BA;struct _fat_ptr _T10BB;struct Cyc_Yystacktype*_T10BC;struct Cyc_Yystacktype _T10BD;struct Cyc_Yyltype _T10BE;unsigned _T10BF;unsigned _T10C0;struct _fat_ptr _T10C1;struct _fat_ptr _T10C2;struct Cyc_Yystacktype*_T10C3;union Cyc_YYSTYPE*_T10C4;union Cyc_YYSTYPE*_T10C5;struct _fat_ptr _T10C6;struct _fat_ptr _T10C7;int _T10C8;struct Cyc_Yystacktype*_T10C9;struct Cyc_Yystacktype _T10CA;struct Cyc_Yyltype _T10CB;unsigned _T10CC;unsigned _T10CD;struct _fat_ptr _T10CE;struct _fat_ptr _T10CF;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T10D0;struct Cyc_Yystacktype*_T10D1;struct Cyc_Yystacktype _T10D2;struct Cyc_Yyltype _T10D3;unsigned _T10D4;unsigned _T10D5;struct _tuple0*_T10D6;struct _fat_ptr*_T10D7;struct Cyc_Yystacktype*_T10D8;union Cyc_YYSTYPE*_T10D9;union Cyc_YYSTYPE*_T10DA;void*_T10DB;struct Cyc_Yystacktype*_T10DC;union Cyc_YYSTYPE*_T10DD;union Cyc_YYSTYPE*_T10DE;void*_T10DF;struct Cyc_Yystacktype*_T10E0;struct Cyc_Yystacktype _T10E1;struct Cyc_Yyltype _T10E2;unsigned _T10E3;unsigned _T10E4;struct Cyc_Absyn_Pat*_T10E5;struct Cyc_Yystacktype*_T10E6;union Cyc_YYSTYPE*_T10E7;union Cyc_YYSTYPE*_T10E8;struct _fat_ptr _T10E9;struct _fat_ptr _T10EA;int _T10EB;struct Cyc_Yystacktype*_T10EC;struct Cyc_Yystacktype _T10ED;struct Cyc_Yyltype _T10EE;unsigned _T10EF;unsigned _T10F0;struct _fat_ptr _T10F1;struct _fat_ptr _T10F2;struct Cyc_Yystacktype*_T10F3;struct Cyc_Yystacktype _T10F4;struct Cyc_Yyltype _T10F5;unsigned _T10F6;struct Cyc_Yystacktype*_T10F7;union Cyc_YYSTYPE*_T10F8;union Cyc_YYSTYPE*_T10F9;struct _fat_ptr _T10FA;unsigned _T10FB;struct Cyc_Absyn_Tvar*_T10FC;struct _fat_ptr*_T10FD;struct Cyc_Yystacktype*_T10FE;union Cyc_YYSTYPE*_T10FF;union Cyc_YYSTYPE*_T1100;struct Cyc_Absyn_Tvar*_T1101;struct Cyc_Absyn_Tvar*_T1102;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1103;struct Cyc_Absyn_Kind*_T1104;struct Cyc_Absyn_Tvar*_T1105;struct Cyc_Yystacktype*_T1106;struct Cyc_Yystacktype _T1107;struct Cyc_Yyltype _T1108;unsigned _T1109;unsigned _T110A;struct _tuple0*_T110B;struct _fat_ptr*_T110C;struct Cyc_Yystacktype*_T110D;union Cyc_YYSTYPE*_T110E;union Cyc_YYSTYPE*_T110F;struct Cyc_Yystacktype*_T1110;union Cyc_YYSTYPE*_T1111;union Cyc_YYSTYPE*_T1112;struct _tuple8*_T1113;struct Cyc_Yystacktype*_T1114;struct Cyc_Yystacktype _T1115;struct Cyc_Yyltype _T1116;unsigned _T1117;unsigned _T1118;void*_T1119;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T111A;void*_T111B;unsigned _T111C;struct Cyc_Absyn_Pat*_T111D;struct Cyc_Yystacktype*_T111E;union Cyc_YYSTYPE*_T111F;union Cyc_YYSTYPE*_T1120;struct _fat_ptr _T1121;struct _fat_ptr _T1122;int _T1123;struct Cyc_Yystacktype*_T1124;struct Cyc_Yystacktype _T1125;struct Cyc_Yyltype _T1126;unsigned _T1127;unsigned _T1128;struct _fat_ptr _T1129;struct _fat_ptr _T112A;struct Cyc_Yystacktype*_T112B;struct Cyc_Yystacktype _T112C;struct Cyc_Yyltype _T112D;unsigned _T112E;struct Cyc_Yystacktype*_T112F;union Cyc_YYSTYPE*_T1130;union Cyc_YYSTYPE*_T1131;struct _fat_ptr _T1132;unsigned _T1133;struct Cyc_Absyn_Tvar*_T1134;struct _fat_ptr*_T1135;struct Cyc_Yystacktype*_T1136;union Cyc_YYSTYPE*_T1137;union Cyc_YYSTYPE*_T1138;struct Cyc_Absyn_Tvar*_T1139;struct Cyc_Absyn_Tvar*_T113A;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T113B;struct Cyc_Absyn_Kind*_T113C;struct Cyc_Absyn_Tvar*_T113D;struct Cyc_Yystacktype*_T113E;struct Cyc_Yystacktype _T113F;struct Cyc_Yyltype _T1140;unsigned _T1141;unsigned _T1142;struct _tuple0*_T1143;struct _fat_ptr*_T1144;struct Cyc_Yystacktype*_T1145;union Cyc_YYSTYPE*_T1146;union Cyc_YYSTYPE*_T1147;struct Cyc_Yystacktype*_T1148;union Cyc_YYSTYPE*_T1149;union Cyc_YYSTYPE*_T114A;struct _tuple8*_T114B;struct Cyc_Yystacktype*_T114C;struct Cyc_Yystacktype _T114D;struct Cyc_Yyltype _T114E;unsigned _T114F;unsigned _T1150;void*_T1151;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1152;void*_T1153;unsigned _T1154;struct Cyc_Absyn_Pat*_T1155;struct Cyc_Yystacktype*_T1156;union Cyc_YYSTYPE*_T1157;union Cyc_YYSTYPE*_T1158;struct _tuple23*_T1159;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T115A;void*_T115B;struct Cyc_Yystacktype*_T115C;struct Cyc_Yystacktype _T115D;struct Cyc_Yyltype _T115E;unsigned _T115F;unsigned _T1160;struct Cyc_Absyn_Pat*_T1161;struct Cyc_Yystacktype*_T1162;union Cyc_YYSTYPE*_T1163;union Cyc_YYSTYPE*_T1164;struct _tuple23*_T1165;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T1166;struct Cyc_Yystacktype*_T1167;union Cyc_YYSTYPE*_T1168;union Cyc_YYSTYPE*_T1169;void*_T116A;struct Cyc_Yystacktype*_T116B;struct Cyc_Yystacktype _T116C;struct Cyc_Yyltype _T116D;unsigned _T116E;unsigned _T116F;struct Cyc_Absyn_Pat*_T1170;struct Cyc_Yystacktype*_T1171;union Cyc_YYSTYPE*_T1172;union Cyc_YYSTYPE*_T1173;struct _tuple23*_T1174;struct Cyc_List_List*(*_T1175)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T1176)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T1177;struct Cyc_Yystacktype _T1178;struct Cyc_Yyltype _T1179;unsigned _T117A;unsigned _T117B;struct Cyc_Yystacktype*_T117C;union Cyc_YYSTYPE*_T117D;union Cyc_YYSTYPE*_T117E;struct Cyc_List_List*_T117F;struct Cyc_Yystacktype*_T1180;union Cyc_YYSTYPE*_T1181;union Cyc_YYSTYPE*_T1182;struct _tuple0*_T1183;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1184;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1185;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T1186;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1187;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1188;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1189;void*_T118A;struct Cyc_Yystacktype*_T118B;struct Cyc_Yystacktype _T118C;struct Cyc_Yyltype _T118D;unsigned _T118E;unsigned _T118F;struct Cyc_Absyn_Pat*_T1190;struct Cyc_Yystacktype*_T1191;union Cyc_YYSTYPE*_T1192;union Cyc_YYSTYPE*_T1193;struct _tuple23*_T1194;struct Cyc_List_List*(*_T1195)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T1196)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T1197;struct Cyc_Yystacktype _T1198;struct Cyc_Yyltype _T1199;unsigned _T119A;unsigned _T119B;struct Cyc_Yystacktype*_T119C;union Cyc_YYSTYPE*_T119D;union Cyc_YYSTYPE*_T119E;struct Cyc_List_List*_T119F;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T11A0;void*_T11A1;struct Cyc_Yystacktype*_T11A2;struct Cyc_Yystacktype _T11A3;struct Cyc_Yyltype _T11A4;unsigned _T11A5;unsigned _T11A6;struct Cyc_Absyn_Pat*_T11A7;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11A8;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11A9;struct Cyc_Yystacktype*_T11AA;union Cyc_YYSTYPE*_T11AB;union Cyc_YYSTYPE*_T11AC;void*_T11AD;struct Cyc_Yystacktype*_T11AE;struct Cyc_Yystacktype _T11AF;struct Cyc_Yyltype _T11B0;unsigned _T11B1;unsigned _T11B2;void*_T11B3;struct Cyc_Yystacktype*_T11B4;struct Cyc_Yystacktype _T11B5;struct Cyc_Yyltype _T11B6;unsigned _T11B7;unsigned _T11B8;struct Cyc_Absyn_Pat*_T11B9;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11BA;struct Cyc_Yystacktype*_T11BB;struct Cyc_Yystacktype _T11BC;struct Cyc_Yyltype _T11BD;unsigned _T11BE;unsigned _T11BF;struct _tuple0*_T11C0;struct _fat_ptr*_T11C1;struct Cyc_Yystacktype*_T11C2;union Cyc_YYSTYPE*_T11C3;union Cyc_YYSTYPE*_T11C4;void*_T11C5;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11C6;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11C7;void*_T11C8;struct Cyc_Yystacktype*_T11C9;struct Cyc_Yystacktype _T11CA;struct Cyc_Yyltype _T11CB;unsigned _T11CC;unsigned _T11CD;void*_T11CE;struct Cyc_Yystacktype*_T11CF;struct Cyc_Yystacktype _T11D0;struct Cyc_Yyltype _T11D1;unsigned _T11D2;unsigned _T11D3;struct Cyc_Absyn_Pat*_T11D4;struct Cyc_Yystacktype*_T11D5;union Cyc_YYSTYPE*_T11D6;union Cyc_YYSTYPE*_T11D7;struct _fat_ptr _T11D8;struct _fat_ptr _T11D9;int _T11DA;struct Cyc_Yystacktype*_T11DB;struct Cyc_Yystacktype _T11DC;struct Cyc_Yyltype _T11DD;unsigned _T11DE;unsigned _T11DF;struct _fat_ptr _T11E0;struct _fat_ptr _T11E1;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11E2;struct Cyc_Yystacktype*_T11E3;struct Cyc_Yystacktype _T11E4;struct Cyc_Yyltype _T11E5;unsigned _T11E6;unsigned _T11E7;struct _tuple0*_T11E8;struct _fat_ptr*_T11E9;struct Cyc_Yystacktype*_T11EA;union Cyc_YYSTYPE*_T11EB;union Cyc_YYSTYPE*_T11EC;void*_T11ED;struct Cyc_Yystacktype*_T11EE;union Cyc_YYSTYPE*_T11EF;union Cyc_YYSTYPE*_T11F0;void*_T11F1;struct Cyc_Yystacktype*_T11F2;struct Cyc_Yystacktype _T11F3;struct Cyc_Yyltype _T11F4;unsigned _T11F5;unsigned _T11F6;struct Cyc_Absyn_Pat*_T11F7;struct Cyc_Yystacktype*_T11F8;union Cyc_YYSTYPE*_T11F9;union Cyc_YYSTYPE*_T11FA;struct _fat_ptr _T11FB;struct Cyc_Absyn_Kind*_T11FC;struct Cyc_Absyn_Kind*_T11FD;void*_T11FE;struct Cyc_Yystacktype*_T11FF;struct Cyc_Yystacktype _T1200;struct Cyc_Yyltype _T1201;unsigned _T1202;unsigned _T1203;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1204;struct Cyc_Yystacktype*_T1205;struct Cyc_Yystacktype _T1206;struct Cyc_Yyltype _T1207;unsigned _T1208;unsigned _T1209;void*_T120A;struct Cyc_Yystacktype*_T120B;struct Cyc_Yystacktype _T120C;struct Cyc_Yyltype _T120D;unsigned _T120E;unsigned _T120F;struct _tuple0*_T1210;struct _fat_ptr*_T1211;struct Cyc_Yystacktype*_T1212;union Cyc_YYSTYPE*_T1213;union Cyc_YYSTYPE*_T1214;void*_T1215;void*_T1216;struct Cyc_Yystacktype*_T1217;struct Cyc_Yystacktype _T1218;struct Cyc_Yyltype _T1219;unsigned _T121A;unsigned _T121B;struct Cyc_Absyn_Pat*_T121C;struct Cyc_Absyn_Kind*_T121D;struct Cyc_Absyn_Kind*_T121E;void*_T121F;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1220;struct Cyc_Yystacktype*_T1221;struct Cyc_Yystacktype _T1222;struct Cyc_Yyltype _T1223;unsigned _T1224;unsigned _T1225;struct _tuple0*_T1226;struct _fat_ptr*_T1227;struct Cyc_Yystacktype*_T1228;union Cyc_YYSTYPE*_T1229;union Cyc_YYSTYPE*_T122A;void*_T122B;void*_T122C;void*_T122D;struct Cyc_Yystacktype*_T122E;struct Cyc_Yystacktype _T122F;struct Cyc_Yyltype _T1230;unsigned _T1231;unsigned _T1232;struct Cyc_Absyn_Pat*_T1233;struct _tuple23*_T1234;struct Cyc_Yystacktype*_T1235;union Cyc_YYSTYPE*_T1236;union Cyc_YYSTYPE*_T1237;struct Cyc_List_List*_T1238;struct _tuple23*_T1239;struct Cyc_Yystacktype*_T123A;union Cyc_YYSTYPE*_T123B;union Cyc_YYSTYPE*_T123C;struct Cyc_List_List*_T123D;struct _tuple23*_T123E;struct Cyc_List_List*_T123F;struct Cyc_Yystacktype*_T1240;union Cyc_YYSTYPE*_T1241;union Cyc_YYSTYPE*_T1242;struct Cyc_List_List*_T1243;struct Cyc_Yystacktype*_T1244;union Cyc_YYSTYPE*_T1245;union Cyc_YYSTYPE*_T1246;struct Cyc_Yystacktype*_T1247;union Cyc_YYSTYPE*_T1248;union Cyc_YYSTYPE*_T1249;struct _tuple24*_T124A;struct Cyc_Yystacktype*_T124B;union Cyc_YYSTYPE*_T124C;union Cyc_YYSTYPE*_T124D;struct _tuple24*_T124E;struct Cyc_Yystacktype*_T124F;union Cyc_YYSTYPE*_T1250;union Cyc_YYSTYPE*_T1251;struct Cyc_Yystacktype*_T1252;union Cyc_YYSTYPE*_T1253;union Cyc_YYSTYPE*_T1254;struct _tuple23*_T1255;struct Cyc_Yystacktype*_T1256;union Cyc_YYSTYPE*_T1257;union Cyc_YYSTYPE*_T1258;struct Cyc_List_List*_T1259;struct _tuple23*_T125A;struct Cyc_Yystacktype*_T125B;union Cyc_YYSTYPE*_T125C;union Cyc_YYSTYPE*_T125D;struct Cyc_List_List*_T125E;struct _tuple23*_T125F;struct Cyc_List_List*_T1260;struct Cyc_Yystacktype*_T1261;union Cyc_YYSTYPE*_T1262;union Cyc_YYSTYPE*_T1263;struct Cyc_List_List*_T1264;struct Cyc_Yystacktype*_T1265;union Cyc_YYSTYPE*_T1266;union Cyc_YYSTYPE*_T1267;struct Cyc_Yystacktype*_T1268;union Cyc_YYSTYPE*_T1269;union Cyc_YYSTYPE*_T126A;struct Cyc_Yystacktype*_T126B;struct Cyc_Yystacktype _T126C;struct Cyc_Yystacktype*_T126D;union Cyc_YYSTYPE*_T126E;union Cyc_YYSTYPE*_T126F;struct Cyc_Absyn_Exp*_T1270;struct Cyc_Yystacktype*_T1271;union Cyc_YYSTYPE*_T1272;union Cyc_YYSTYPE*_T1273;struct Cyc_Absyn_Exp*_T1274;struct Cyc_Yystacktype*_T1275;struct Cyc_Yystacktype _T1276;struct Cyc_Yyltype _T1277;unsigned _T1278;unsigned _T1279;struct Cyc_Absyn_Exp*_T127A;struct Cyc_Yystacktype*_T127B;struct Cyc_Yystacktype _T127C;struct Cyc_Yystacktype*_T127D;union Cyc_YYSTYPE*_T127E;union Cyc_YYSTYPE*_T127F;struct Cyc_Absyn_Exp*_T1280;struct Cyc_Yystacktype*_T1281;union Cyc_YYSTYPE*_T1282;union Cyc_YYSTYPE*_T1283;struct Cyc_Core_Opt*_T1284;struct Cyc_Yystacktype*_T1285;union Cyc_YYSTYPE*_T1286;union Cyc_YYSTYPE*_T1287;struct Cyc_Absyn_Exp*_T1288;struct Cyc_Yystacktype*_T1289;struct Cyc_Yystacktype _T128A;struct Cyc_Yyltype _T128B;unsigned _T128C;unsigned _T128D;struct Cyc_Absyn_Exp*_T128E;struct Cyc_Yystacktype*_T128F;union Cyc_YYSTYPE*_T1290;union Cyc_YYSTYPE*_T1291;struct Cyc_Absyn_Exp*_T1292;struct Cyc_Yystacktype*_T1293;union Cyc_YYSTYPE*_T1294;union Cyc_YYSTYPE*_T1295;struct Cyc_Absyn_Exp*_T1296;struct Cyc_Yystacktype*_T1297;struct Cyc_Yystacktype _T1298;struct Cyc_Yyltype _T1299;unsigned _T129A;unsigned _T129B;struct Cyc_Absyn_Exp*_T129C;struct Cyc_Core_Opt*_T129D;struct Cyc_Core_Opt*_T129E;struct Cyc_Core_Opt*_T129F;struct Cyc_Core_Opt*_T12A0;struct Cyc_Core_Opt*_T12A1;struct Cyc_Core_Opt*_T12A2;struct Cyc_Core_Opt*_T12A3;struct Cyc_Core_Opt*_T12A4;struct Cyc_Core_Opt*_T12A5;struct Cyc_Core_Opt*_T12A6;struct Cyc_Yystacktype*_T12A7;struct Cyc_Yystacktype _T12A8;struct Cyc_Yystacktype*_T12A9;union Cyc_YYSTYPE*_T12AA;union Cyc_YYSTYPE*_T12AB;struct Cyc_Absyn_Exp*_T12AC;struct Cyc_Yystacktype*_T12AD;union Cyc_YYSTYPE*_T12AE;union Cyc_YYSTYPE*_T12AF;struct Cyc_Absyn_Exp*_T12B0;struct Cyc_Yystacktype*_T12B1;union Cyc_YYSTYPE*_T12B2;union Cyc_YYSTYPE*_T12B3;struct Cyc_Absyn_Exp*_T12B4;struct Cyc_Yystacktype*_T12B5;struct Cyc_Yystacktype _T12B6;struct Cyc_Yyltype _T12B7;unsigned _T12B8;unsigned _T12B9;struct Cyc_Absyn_Exp*_T12BA;struct Cyc_Yystacktype*_T12BB;union Cyc_YYSTYPE*_T12BC;union Cyc_YYSTYPE*_T12BD;struct Cyc_Absyn_Exp*_T12BE;struct Cyc_Yystacktype*_T12BF;struct Cyc_Yystacktype _T12C0;struct Cyc_Yyltype _T12C1;unsigned _T12C2;unsigned _T12C3;struct Cyc_Absyn_Exp*_T12C4;struct Cyc_Yystacktype*_T12C5;union Cyc_YYSTYPE*_T12C6;union Cyc_YYSTYPE*_T12C7;struct Cyc_Absyn_Exp*_T12C8;struct Cyc_Yystacktype*_T12C9;struct Cyc_Yystacktype _T12CA;struct Cyc_Yyltype _T12CB;unsigned _T12CC;unsigned _T12CD;struct Cyc_Absyn_Exp*_T12CE;struct Cyc_Yystacktype*_T12CF;union Cyc_YYSTYPE*_T12D0;union Cyc_YYSTYPE*_T12D1;struct Cyc_Absyn_Exp*_T12D2;struct Cyc_Yystacktype*_T12D3;struct Cyc_Yystacktype _T12D4;struct Cyc_Yyltype _T12D5;unsigned _T12D6;unsigned _T12D7;struct Cyc_Absyn_Exp*_T12D8;struct Cyc_Yystacktype*_T12D9;union Cyc_YYSTYPE*_T12DA;union Cyc_YYSTYPE*_T12DB;struct Cyc_Absyn_Exp*_T12DC;struct Cyc_Yystacktype*_T12DD;union Cyc_YYSTYPE*_T12DE;union Cyc_YYSTYPE*_T12DF;struct Cyc_Absyn_Exp*_T12E0;struct Cyc_Yystacktype*_T12E1;struct Cyc_Yystacktype _T12E2;struct Cyc_Yyltype _T12E3;unsigned _T12E4;unsigned _T12E5;struct Cyc_Absyn_Exp*_T12E6;struct Cyc_Yystacktype*_T12E7;union Cyc_YYSTYPE*_T12E8;union Cyc_YYSTYPE*_T12E9;struct Cyc_Absyn_Exp*_T12EA;struct Cyc_Yystacktype*_T12EB;union Cyc_YYSTYPE*_T12EC;union Cyc_YYSTYPE*_T12ED;struct Cyc_Absyn_Exp*_T12EE;struct Cyc_Yystacktype*_T12EF;struct Cyc_Yystacktype _T12F0;struct Cyc_Yyltype _T12F1;unsigned _T12F2;unsigned _T12F3;struct Cyc_Absyn_Exp*_T12F4;struct Cyc_Yystacktype*_T12F5;union Cyc_YYSTYPE*_T12F6;union Cyc_YYSTYPE*_T12F7;struct Cyc_Absyn_Exp*_T12F8;struct Cyc_Absyn_Exp*_T12F9;struct Cyc_Yystacktype*_T12FA;union Cyc_YYSTYPE*_T12FB;union Cyc_YYSTYPE*_T12FC;struct Cyc_Absyn_Exp*_T12FD;struct Cyc_Absyn_Exp*_T12FE;struct Cyc_Yystacktype*_T12FF;struct Cyc_Yystacktype _T1300;struct Cyc_Yyltype _T1301;unsigned _T1302;unsigned _T1303;struct Cyc_Absyn_Exp*_T1304;struct Cyc_Yystacktype*_T1305;union Cyc_YYSTYPE*_T1306;union Cyc_YYSTYPE*_T1307;struct Cyc_Absyn_Exp*_T1308;struct Cyc_Absyn_Exp*_T1309;struct Cyc_Yystacktype*_T130A;union Cyc_YYSTYPE*_T130B;union Cyc_YYSTYPE*_T130C;struct Cyc_Absyn_Exp*_T130D;struct Cyc_Absyn_Exp*_T130E;struct Cyc_Yystacktype*_T130F;struct Cyc_Yystacktype _T1310;struct Cyc_Yyltype _T1311;unsigned _T1312;unsigned _T1313;struct Cyc_Absyn_Exp*_T1314;struct Cyc_Yystacktype*_T1315;struct Cyc_Yystacktype _T1316;struct Cyc_Yystacktype*_T1317;struct Cyc_Yystacktype _T1318;struct Cyc_Yystacktype*_T1319;union Cyc_YYSTYPE*_T131A;union Cyc_YYSTYPE*_T131B;struct Cyc_Absyn_Exp*_T131C;struct Cyc_Yystacktype*_T131D;union Cyc_YYSTYPE*_T131E;union Cyc_YYSTYPE*_T131F;struct Cyc_Absyn_Exp*_T1320;struct Cyc_Yystacktype*_T1321;struct Cyc_Yystacktype _T1322;struct Cyc_Yyltype _T1323;unsigned _T1324;unsigned _T1325;struct Cyc_Absyn_Exp*_T1326;struct Cyc_Yystacktype*_T1327;struct Cyc_Yystacktype _T1328;struct Cyc_Yystacktype*_T1329;union Cyc_YYSTYPE*_T132A;union Cyc_YYSTYPE*_T132B;struct Cyc_Absyn_Exp*_T132C;struct Cyc_Yystacktype*_T132D;union Cyc_YYSTYPE*_T132E;union Cyc_YYSTYPE*_T132F;struct Cyc_Absyn_Exp*_T1330;struct Cyc_Yystacktype*_T1331;struct Cyc_Yystacktype _T1332;struct Cyc_Yyltype _T1333;unsigned _T1334;unsigned _T1335;struct Cyc_Absyn_Exp*_T1336;struct Cyc_Yystacktype*_T1337;struct Cyc_Yystacktype _T1338;struct Cyc_Yystacktype*_T1339;union Cyc_YYSTYPE*_T133A;union Cyc_YYSTYPE*_T133B;struct Cyc_Absyn_Exp*_T133C;struct Cyc_Yystacktype*_T133D;union Cyc_YYSTYPE*_T133E;union Cyc_YYSTYPE*_T133F;struct Cyc_Absyn_Exp*_T1340;struct Cyc_Yystacktype*_T1341;struct Cyc_Yystacktype _T1342;struct Cyc_Yyltype _T1343;unsigned _T1344;unsigned _T1345;struct Cyc_Absyn_Exp*_T1346;struct Cyc_Yystacktype*_T1347;struct Cyc_Yystacktype _T1348;struct Cyc_Yystacktype*_T1349;union Cyc_YYSTYPE*_T134A;union Cyc_YYSTYPE*_T134B;struct Cyc_Absyn_Exp*_T134C;struct Cyc_Yystacktype*_T134D;union Cyc_YYSTYPE*_T134E;union Cyc_YYSTYPE*_T134F;struct Cyc_Absyn_Exp*_T1350;struct Cyc_Yystacktype*_T1351;struct Cyc_Yystacktype _T1352;struct Cyc_Yyltype _T1353;unsigned _T1354;unsigned _T1355;struct Cyc_Absyn_Exp*_T1356;struct Cyc_Yystacktype*_T1357;struct Cyc_Yystacktype _T1358;struct Cyc_Yystacktype*_T1359;union Cyc_YYSTYPE*_T135A;union Cyc_YYSTYPE*_T135B;struct Cyc_Absyn_Exp*_T135C;struct Cyc_Yystacktype*_T135D;union Cyc_YYSTYPE*_T135E;union Cyc_YYSTYPE*_T135F;struct Cyc_Absyn_Exp*_T1360;struct Cyc_Yystacktype*_T1361;struct Cyc_Yystacktype _T1362;struct Cyc_Yyltype _T1363;unsigned _T1364;unsigned _T1365;struct Cyc_Absyn_Exp*_T1366;struct Cyc_Yystacktype*_T1367;struct Cyc_Yystacktype _T1368;struct Cyc_Yystacktype*_T1369;union Cyc_YYSTYPE*_T136A;union Cyc_YYSTYPE*_T136B;struct Cyc_Absyn_Exp*(*_T136C)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T136D;union Cyc_YYSTYPE*_T136E;union Cyc_YYSTYPE*_T136F;struct Cyc_Absyn_Exp*_T1370;struct Cyc_Yystacktype*_T1371;union Cyc_YYSTYPE*_T1372;union Cyc_YYSTYPE*_T1373;struct Cyc_Absyn_Exp*_T1374;struct Cyc_Yystacktype*_T1375;struct Cyc_Yystacktype _T1376;struct Cyc_Yyltype _T1377;unsigned _T1378;unsigned _T1379;struct Cyc_Absyn_Exp*_T137A;struct Cyc_Yystacktype*_T137B;struct Cyc_Yystacktype _T137C;struct Cyc_Yystacktype*_T137D;union Cyc_YYSTYPE*_T137E;union Cyc_YYSTYPE*_T137F;struct Cyc_Absyn_Exp*(*_T1380)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T1381;union Cyc_YYSTYPE*_T1382;union Cyc_YYSTYPE*_T1383;struct Cyc_Absyn_Exp*_T1384;struct Cyc_Yystacktype*_T1385;union Cyc_YYSTYPE*_T1386;union Cyc_YYSTYPE*_T1387;struct Cyc_Absyn_Exp*_T1388;struct Cyc_Yystacktype*_T1389;struct Cyc_Yystacktype _T138A;struct Cyc_Yyltype _T138B;unsigned _T138C;unsigned _T138D;struct Cyc_Absyn_Exp*_T138E;struct Cyc_Yystacktype*_T138F;struct Cyc_Yystacktype _T1390;struct Cyc_Yystacktype*_T1391;union Cyc_YYSTYPE*_T1392;union Cyc_YYSTYPE*_T1393;struct Cyc_Absyn_Exp*_T1394;struct Cyc_Yystacktype*_T1395;union Cyc_YYSTYPE*_T1396;union Cyc_YYSTYPE*_T1397;struct Cyc_Absyn_Exp*_T1398;struct Cyc_Yystacktype*_T1399;struct Cyc_Yystacktype _T139A;struct Cyc_Yyltype _T139B;unsigned _T139C;unsigned _T139D;struct Cyc_Absyn_Exp*_T139E;struct Cyc_Yystacktype*_T139F;union Cyc_YYSTYPE*_T13A0;union Cyc_YYSTYPE*_T13A1;struct Cyc_Absyn_Exp*_T13A2;struct Cyc_Yystacktype*_T13A3;union Cyc_YYSTYPE*_T13A4;union Cyc_YYSTYPE*_T13A5;struct Cyc_Absyn_Exp*_T13A6;struct Cyc_Yystacktype*_T13A7;struct Cyc_Yystacktype _T13A8;struct Cyc_Yyltype _T13A9;unsigned _T13AA;unsigned _T13AB;struct Cyc_Absyn_Exp*_T13AC;struct Cyc_Yystacktype*_T13AD;struct Cyc_Yystacktype _T13AE;struct Cyc_Yystacktype*_T13AF;union Cyc_YYSTYPE*_T13B0;union Cyc_YYSTYPE*_T13B1;enum Cyc_Absyn_Primop _T13B2;struct Cyc_Yystacktype*_T13B3;union Cyc_YYSTYPE*_T13B4;union Cyc_YYSTYPE*_T13B5;struct Cyc_Absyn_Exp*_T13B6;struct Cyc_Yystacktype*_T13B7;union Cyc_YYSTYPE*_T13B8;union Cyc_YYSTYPE*_T13B9;struct Cyc_Absyn_Exp*_T13BA;struct Cyc_Yystacktype*_T13BB;struct Cyc_Yystacktype _T13BC;struct Cyc_Yyltype _T13BD;unsigned _T13BE;unsigned _T13BF;struct Cyc_Absyn_Exp*_T13C0;struct Cyc_Yystacktype*_T13C1;struct Cyc_Yystacktype _T13C2;struct Cyc_Yystacktype*_T13C3;union Cyc_YYSTYPE*_T13C4;union Cyc_YYSTYPE*_T13C5;enum Cyc_Absyn_Primop _T13C6;struct Cyc_Yystacktype*_T13C7;union Cyc_YYSTYPE*_T13C8;union Cyc_YYSTYPE*_T13C9;struct Cyc_Absyn_Exp*_T13CA;struct Cyc_Yystacktype*_T13CB;union Cyc_YYSTYPE*_T13CC;union Cyc_YYSTYPE*_T13CD;struct Cyc_Absyn_Exp*_T13CE;struct Cyc_Yystacktype*_T13CF;struct Cyc_Yystacktype _T13D0;struct Cyc_Yyltype _T13D1;unsigned _T13D2;unsigned _T13D3;struct Cyc_Absyn_Exp*_T13D4;struct Cyc_Absyn_Exp*(*_T13D5)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13D6)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13D7)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13D8)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13D9)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DA)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T13DB;struct Cyc_Yystacktype _T13DC;struct Cyc_Yystacktype*_T13DD;union Cyc_YYSTYPE*_T13DE;union Cyc_YYSTYPE*_T13DF;struct _tuple8*_T13E0;struct Cyc_Yystacktype*_T13E1;struct Cyc_Yystacktype _T13E2;struct Cyc_Yyltype _T13E3;unsigned _T13E4;unsigned _T13E5;void*_T13E6;struct Cyc_Yystacktype*_T13E7;union Cyc_YYSTYPE*_T13E8;union Cyc_YYSTYPE*_T13E9;struct Cyc_Absyn_Exp*_T13EA;struct Cyc_Yystacktype*_T13EB;struct Cyc_Yystacktype _T13EC;struct Cyc_Yyltype _T13ED;unsigned _T13EE;unsigned _T13EF;struct Cyc_Absyn_Exp*_T13F0;struct Cyc_Yystacktype*_T13F1;struct Cyc_Yystacktype _T13F2;struct Cyc_Yystacktype*_T13F3;union Cyc_YYSTYPE*_T13F4;union Cyc_YYSTYPE*_T13F5;struct Cyc_Absyn_Exp*_T13F6;struct Cyc_Yystacktype*_T13F7;struct Cyc_Yystacktype _T13F8;struct Cyc_Yyltype _T13F9;unsigned _T13FA;unsigned _T13FB;struct Cyc_Absyn_Exp*_T13FC;struct Cyc_Yystacktype*_T13FD;union Cyc_YYSTYPE*_T13FE;union Cyc_YYSTYPE*_T13FF;struct Cyc_Absyn_Exp*_T1400;struct Cyc_Yystacktype*_T1401;struct Cyc_Yystacktype _T1402;struct Cyc_Yyltype _T1403;unsigned _T1404;unsigned _T1405;struct Cyc_Absyn_Exp*_T1406;struct Cyc_Yystacktype*_T1407;union Cyc_YYSTYPE*_T1408;union Cyc_YYSTYPE*_T1409;struct Cyc_Absyn_Exp*_T140A;struct Cyc_Yystacktype*_T140B;struct Cyc_Yystacktype _T140C;struct Cyc_Yyltype _T140D;unsigned _T140E;unsigned _T140F;struct Cyc_Absyn_Exp*_T1410;struct Cyc_Yystacktype*_T1411;union Cyc_YYSTYPE*_T1412;union Cyc_YYSTYPE*_T1413;struct Cyc_Absyn_Exp*_T1414;struct Cyc_Yystacktype*_T1415;struct Cyc_Yystacktype _T1416;struct Cyc_Yyltype _T1417;unsigned _T1418;unsigned _T1419;struct Cyc_Absyn_Exp*_T141A;struct Cyc_Yystacktype*_T141B;union Cyc_YYSTYPE*_T141C;union Cyc_YYSTYPE*_T141D;enum Cyc_Absyn_Primop _T141E;struct Cyc_Yystacktype*_T141F;union Cyc_YYSTYPE*_T1420;union Cyc_YYSTYPE*_T1421;struct Cyc_Absyn_Exp*_T1422;struct Cyc_Yystacktype*_T1423;struct Cyc_Yystacktype _T1424;struct Cyc_Yyltype _T1425;unsigned _T1426;unsigned _T1427;struct Cyc_Absyn_Exp*_T1428;struct Cyc_Yystacktype*_T1429;union Cyc_YYSTYPE*_T142A;union Cyc_YYSTYPE*_T142B;struct _tuple8*_T142C;struct Cyc_Yystacktype*_T142D;struct Cyc_Yystacktype _T142E;struct Cyc_Yyltype _T142F;unsigned _T1430;unsigned _T1431;void*_T1432;struct Cyc_Yystacktype*_T1433;struct Cyc_Yystacktype _T1434;struct Cyc_Yyltype _T1435;unsigned _T1436;unsigned _T1437;struct Cyc_Absyn_Exp*_T1438;struct Cyc_Yystacktype*_T1439;union Cyc_YYSTYPE*_T143A;union Cyc_YYSTYPE*_T143B;struct Cyc_Absyn_Exp*_T143C;struct Cyc_Yystacktype*_T143D;struct Cyc_Yystacktype _T143E;struct Cyc_Yyltype _T143F;unsigned _T1440;unsigned _T1441;struct Cyc_Absyn_Exp*_T1442;struct Cyc_Yystacktype*_T1443;union Cyc_YYSTYPE*_T1444;union Cyc_YYSTYPE*_T1445;struct _tuple8*_T1446;struct Cyc_Yystacktype*_T1447;struct Cyc_Yystacktype _T1448;struct Cyc_Yyltype _T1449;unsigned _T144A;unsigned _T144B;void*_T144C;struct Cyc_Yystacktype*_T144D;union Cyc_YYSTYPE*_T144E;union Cyc_YYSTYPE*_T144F;struct Cyc_List_List*_T1450;struct Cyc_List_List*_T1451;struct Cyc_Yystacktype*_T1452;struct Cyc_Yystacktype _T1453;struct Cyc_Yyltype _T1454;unsigned _T1455;unsigned _T1456;struct Cyc_Absyn_Exp*_T1457;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1458;struct Cyc_Yystacktype*_T1459;union Cyc_YYSTYPE*_T145A;union Cyc_YYSTYPE*_T145B;void*_T145C;struct Cyc_Yystacktype*_T145D;struct Cyc_Yystacktype _T145E;struct Cyc_Yyltype _T145F;unsigned _T1460;unsigned _T1461;struct Cyc_Absyn_Exp*_T1462;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1463;struct Cyc_Yystacktype*_T1464;union Cyc_YYSTYPE*_T1465;union Cyc_YYSTYPE*_T1466;struct Cyc_Yystacktype*_T1467;union Cyc_YYSTYPE*_T1468;union Cyc_YYSTYPE*_T1469;struct Cyc_Yystacktype*_T146A;union Cyc_YYSTYPE*_T146B;union Cyc_YYSTYPE*_T146C;void*_T146D;struct Cyc_Yystacktype*_T146E;struct Cyc_Yystacktype _T146F;struct Cyc_Yyltype _T1470;unsigned _T1471;unsigned _T1472;struct Cyc_Absyn_Exp*_T1473;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1474;struct Cyc_Yystacktype*_T1475;union Cyc_YYSTYPE*_T1476;union Cyc_YYSTYPE*_T1477;struct Cyc_Yystacktype*_T1478;union Cyc_YYSTYPE*_T1479;union Cyc_YYSTYPE*_T147A;void*_T147B;struct Cyc_Yystacktype*_T147C;struct Cyc_Yystacktype _T147D;struct Cyc_Yyltype _T147E;unsigned _T147F;unsigned _T1480;struct Cyc_Absyn_Exp*_T1481;struct Cyc_Yystacktype*_T1482;union Cyc_YYSTYPE*_T1483;union Cyc_YYSTYPE*_T1484;struct Cyc_Absyn_Exp*_T1485;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1486;struct Cyc_Yystacktype*_T1487;union Cyc_YYSTYPE*_T1488;union Cyc_YYSTYPE*_T1489;void*_T148A;struct Cyc_Yystacktype*_T148B;struct Cyc_Yystacktype _T148C;struct Cyc_Yyltype _T148D;unsigned _T148E;unsigned _T148F;struct Cyc_Absyn_Exp*_T1490;struct Cyc_Yystacktype*_T1491;union Cyc_YYSTYPE*_T1492;union Cyc_YYSTYPE*_T1493;struct Cyc_Absyn_Exp*_T1494;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1495;struct Cyc_Yystacktype*_T1496;union Cyc_YYSTYPE*_T1497;union Cyc_YYSTYPE*_T1498;void*_T1499;struct Cyc_Yystacktype*_T149A;struct Cyc_Yystacktype _T149B;struct Cyc_Yyltype _T149C;unsigned _T149D;unsigned _T149E;struct Cyc_Absyn_Exp*_T149F;struct Cyc_Yystacktype*_T14A0;union Cyc_YYSTYPE*_T14A1;union Cyc_YYSTYPE*_T14A2;struct _tuple8*_T14A3;struct Cyc_Yystacktype*_T14A4;struct Cyc_Yystacktype _T14A5;struct Cyc_Yyltype _T14A6;unsigned _T14A7;unsigned _T14A8;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14A9;void**_T14AA;struct Cyc_Yystacktype*_T14AB;union Cyc_YYSTYPE*_T14AC;union Cyc_YYSTYPE*_T14AD;void*_T14AE;struct Cyc_Yystacktype*_T14AF;struct Cyc_Yystacktype _T14B0;struct Cyc_Yyltype _T14B1;unsigned _T14B2;unsigned _T14B3;struct Cyc_Absyn_Exp*_T14B4;struct Cyc_Yystacktype*_T14B5;union Cyc_YYSTYPE*_T14B6;union Cyc_YYSTYPE*_T14B7;struct _tuple8*_T14B8;struct Cyc_Yystacktype*_T14B9;struct Cyc_Yystacktype _T14BA;struct Cyc_Yyltype _T14BB;unsigned _T14BC;unsigned _T14BD;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14BE;struct Cyc_Yystacktype*_T14BF;union Cyc_YYSTYPE*_T14C0;union Cyc_YYSTYPE*_T14C1;struct Cyc_Yystacktype*_T14C2;union Cyc_YYSTYPE*_T14C3;union Cyc_YYSTYPE*_T14C4;void**_T14C5;struct Cyc_Yystacktype*_T14C6;union Cyc_YYSTYPE*_T14C7;union Cyc_YYSTYPE*_T14C8;void*_T14C9;struct Cyc_Yystacktype*_T14CA;struct Cyc_Yystacktype _T14CB;struct Cyc_Yyltype _T14CC;unsigned _T14CD;unsigned _T14CE;struct Cyc_Absyn_Exp*_T14CF;struct Cyc_Yystacktype*_T14D0;union Cyc_YYSTYPE*_T14D1;union Cyc_YYSTYPE*_T14D2;struct _tuple8*_T14D3;struct Cyc_Yystacktype*_T14D4;struct Cyc_Yystacktype _T14D5;struct Cyc_Yyltype _T14D6;unsigned _T14D7;unsigned _T14D8;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14D9;struct Cyc_Yystacktype*_T14DA;union Cyc_YYSTYPE*_T14DB;union Cyc_YYSTYPE*_T14DC;void**_T14DD;struct Cyc_Yystacktype*_T14DE;union Cyc_YYSTYPE*_T14DF;union Cyc_YYSTYPE*_T14E0;void*_T14E1;struct Cyc_Yystacktype*_T14E2;struct Cyc_Yystacktype _T14E3;struct Cyc_Yyltype _T14E4;unsigned _T14E5;unsigned _T14E6;struct Cyc_Absyn_Exp*_T14E7;struct Cyc_List_List*_T14E8;struct Cyc_Yystacktype*_T14E9;union Cyc_YYSTYPE*_T14EA;union Cyc_YYSTYPE*_T14EB;struct _fat_ptr _T14EC;struct Cyc_Yystacktype*_T14ED;struct Cyc_Yystacktype _T14EE;struct Cyc_Yyltype _T14EF;unsigned _T14F0;unsigned _T14F1;struct Cyc_Absyn_Exp*_T14F2;struct Cyc_Yystacktype*_T14F3;union Cyc_YYSTYPE*_T14F4;union Cyc_YYSTYPE*_T14F5;struct Cyc_Absyn_Exp*_T14F6;struct Cyc_Yystacktype*_T14F7;struct Cyc_Yystacktype _T14F8;struct Cyc_Yyltype _T14F9;unsigned _T14FA;unsigned _T14FB;struct Cyc_Absyn_Exp*_T14FC;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T14FD;struct Cyc_Yystacktype*_T14FE;union Cyc_YYSTYPE*_T14FF;union Cyc_YYSTYPE*_T1500;struct _fat_ptr*_T1501;struct Cyc_Yystacktype*_T1502;union Cyc_YYSTYPE*_T1503;union Cyc_YYSTYPE*_T1504;void*_T1505;struct Cyc_Yystacktype*_T1506;struct Cyc_Yystacktype _T1507;struct Cyc_Yyltype _T1508;unsigned _T1509;unsigned _T150A;struct Cyc_Absyn_Exp*_T150B;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T150C;struct Cyc_Yystacktype*_T150D;union Cyc_YYSTYPE*_T150E;union Cyc_YYSTYPE*_T150F;struct Cyc_Absyn_Exp*_T1510;struct Cyc_Yystacktype*_T1511;struct Cyc_Yystacktype _T1512;struct Cyc_Yyltype _T1513;unsigned _T1514;unsigned _T1515;struct _fat_ptr*_T1516;struct Cyc_Yystacktype*_T1517;union Cyc_YYSTYPE*_T1518;union Cyc_YYSTYPE*_T1519;void*_T151A;struct Cyc_Yystacktype*_T151B;struct Cyc_Yystacktype _T151C;struct Cyc_Yyltype _T151D;unsigned _T151E;unsigned _T151F;struct Cyc_Absyn_Exp*_T1520;struct Cyc_Yystacktype*_T1521;union Cyc_YYSTYPE*_T1522;union Cyc_YYSTYPE*_T1523;struct _tuple8*_T1524;struct Cyc_Yystacktype*_T1525;struct Cyc_Yystacktype _T1526;struct Cyc_Yyltype _T1527;unsigned _T1528;unsigned _T1529;void*_T152A;struct Cyc_Yystacktype*_T152B;struct Cyc_Yystacktype _T152C;struct Cyc_Yyltype _T152D;unsigned _T152E;unsigned _T152F;struct Cyc_Absyn_Exp*_T1530;struct Cyc_Yystacktype*_T1531;union Cyc_YYSTYPE*_T1532;union Cyc_YYSTYPE*_T1533;void*_T1534;struct Cyc_Yystacktype*_T1535;struct Cyc_Yystacktype _T1536;struct Cyc_Yyltype _T1537;unsigned _T1538;unsigned _T1539;struct Cyc_Absyn_Exp*_T153A;struct Cyc_Yystacktype*_T153B;union Cyc_YYSTYPE*_T153C;union Cyc_YYSTYPE*_T153D;struct Cyc_Absyn_Exp*_T153E;struct Cyc_Yystacktype*_T153F;struct Cyc_Yystacktype _T1540;struct Cyc_Yyltype _T1541;unsigned _T1542;unsigned _T1543;struct Cyc_Absyn_Exp*_T1544;struct Cyc_Yystacktype*_T1545;union Cyc_YYSTYPE*_T1546;union Cyc_YYSTYPE*_T1547;struct Cyc_Absyn_Exp*_T1548;struct Cyc_Yystacktype*_T1549;struct Cyc_Yystacktype _T154A;struct Cyc_Yyltype _T154B;unsigned _T154C;unsigned _T154D;struct Cyc_Absyn_Exp*_T154E;struct Cyc_Yystacktype*_T154F;union Cyc_YYSTYPE*_T1550;union Cyc_YYSTYPE*_T1551;struct Cyc_Absyn_Exp*_T1552;struct Cyc_Yystacktype*_T1553;struct Cyc_Yystacktype _T1554;struct Cyc_Yyltype _T1555;unsigned _T1556;unsigned _T1557;struct Cyc_Absyn_Exp*_T1558;struct Cyc_Yystacktype*_T1559;union Cyc_YYSTYPE*_T155A;union Cyc_YYSTYPE*_T155B;struct Cyc_Absyn_Exp*_T155C;struct Cyc_Yystacktype*_T155D;struct Cyc_Yystacktype _T155E;struct Cyc_Yyltype _T155F;unsigned _T1560;unsigned _T1561;struct Cyc_Absyn_Exp*_T1562;struct Cyc_Yystacktype*_T1563;union Cyc_YYSTYPE*_T1564;union Cyc_YYSTYPE*_T1565;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T1566;struct Cyc_Yystacktype*_T1567;union Cyc_YYSTYPE*_T1568;union Cyc_YYSTYPE*_T1569;struct Cyc_Yystacktype*_T156A;union Cyc_YYSTYPE*_T156B;union Cyc_YYSTYPE*_T156C;void*_T156D;struct _tuple31*_T156E;struct Cyc_Yystacktype*_T156F;union Cyc_YYSTYPE*_T1570;union Cyc_YYSTYPE*_T1571;struct _tuple31*_T1572;struct Cyc_Yystacktype*_T1573;union Cyc_YYSTYPE*_T1574;union Cyc_YYSTYPE*_T1575;struct _tuple31*_T1576;struct Cyc_Yystacktype*_T1577;union Cyc_YYSTYPE*_T1578;union Cyc_YYSTYPE*_T1579;struct Cyc_List_List*_T157A;struct Cyc_List_List*_T157B;struct Cyc_Yystacktype*_T157C;union Cyc_YYSTYPE*_T157D;union Cyc_YYSTYPE*_T157E;struct Cyc_List_List*_T157F;struct Cyc_Yystacktype*_T1580;union Cyc_YYSTYPE*_T1581;union Cyc_YYSTYPE*_T1582;struct Cyc_Yystacktype*_T1583;union Cyc_YYSTYPE*_T1584;union Cyc_YYSTYPE*_T1585;struct _tuple28*_T1586;struct _tuple28*_T1587;struct Cyc_Yystacktype*_T1588;union Cyc_YYSTYPE*_T1589;union Cyc_YYSTYPE*_T158A;struct _tuple28*_T158B;struct Cyc_Yystacktype*_T158C;union Cyc_YYSTYPE*_T158D;union Cyc_YYSTYPE*_T158E;struct Cyc_List_List*_T158F;struct Cyc_Yystacktype*_T1590;union Cyc_YYSTYPE*_T1591;union Cyc_YYSTYPE*_T1592;struct Cyc_List_List*_T1593;struct Cyc_Yystacktype*_T1594;union Cyc_YYSTYPE*_T1595;union Cyc_YYSTYPE*_T1596;struct Cyc_List_List*_T1597;struct Cyc_Yystacktype*_T1598;union Cyc_YYSTYPE*_T1599;union Cyc_YYSTYPE*_T159A;struct Cyc_Yystacktype*_T159B;union Cyc_YYSTYPE*_T159C;union Cyc_YYSTYPE*_T159D;struct _tuple32*_T159E;struct Cyc_Yystacktype*_T159F;union Cyc_YYSTYPE*_T15A0;union Cyc_YYSTYPE*_T15A1;struct Cyc_Yystacktype*_T15A2;union Cyc_YYSTYPE*_T15A3;union Cyc_YYSTYPE*_T15A4;struct Cyc_Yystacktype*_T15A5;union Cyc_YYSTYPE*_T15A6;union Cyc_YYSTYPE*_T15A7;struct Cyc_List_List*_T15A8;struct Cyc_List_List*_T15A9;struct Cyc_List_List*_T15AA;struct _fat_ptr*_T15AB;struct Cyc_Yystacktype*_T15AC;union Cyc_YYSTYPE*_T15AD;union Cyc_YYSTYPE*_T15AE;struct Cyc_List_List*_T15AF;struct _fat_ptr*_T15B0;struct Cyc_Yystacktype*_T15B1;union Cyc_YYSTYPE*_T15B2;union Cyc_YYSTYPE*_T15B3;struct Cyc_Yystacktype*_T15B4;union Cyc_YYSTYPE*_T15B5;union Cyc_YYSTYPE*_T15B6;struct Cyc_Yystacktype*_T15B7;struct Cyc_Yystacktype _T15B8;struct Cyc_Yystacktype*_T15B9;union Cyc_YYSTYPE*_T15BA;union Cyc_YYSTYPE*_T15BB;struct Cyc_Absyn_Exp*_T15BC;struct Cyc_Yystacktype*_T15BD;union Cyc_YYSTYPE*_T15BE;union Cyc_YYSTYPE*_T15BF;struct Cyc_Absyn_Exp*_T15C0;struct Cyc_Yystacktype*_T15C1;struct Cyc_Yystacktype _T15C2;struct Cyc_Yyltype _T15C3;unsigned _T15C4;unsigned _T15C5;struct Cyc_Absyn_Exp*_T15C6;struct Cyc_Yystacktype*_T15C7;union Cyc_YYSTYPE*_T15C8;union Cyc_YYSTYPE*_T15C9;struct Cyc_Absyn_Exp*_T15CA;struct Cyc_Yystacktype*_T15CB;struct Cyc_Yystacktype _T15CC;struct Cyc_Yyltype _T15CD;unsigned _T15CE;unsigned _T15CF;struct Cyc_Absyn_Exp*_T15D0;struct Cyc_Yystacktype*_T15D1;union Cyc_YYSTYPE*_T15D2;union Cyc_YYSTYPE*_T15D3;struct Cyc_Absyn_Exp*_T15D4;struct Cyc_Yystacktype*_T15D5;union Cyc_YYSTYPE*_T15D6;union Cyc_YYSTYPE*_T15D7;struct Cyc_List_List*_T15D8;struct Cyc_Yystacktype*_T15D9;struct Cyc_Yystacktype _T15DA;struct Cyc_Yyltype _T15DB;unsigned _T15DC;unsigned _T15DD;struct Cyc_Absyn_Exp*_T15DE;struct Cyc_Yystacktype*_T15DF;union Cyc_YYSTYPE*_T15E0;union Cyc_YYSTYPE*_T15E1;struct Cyc_Absyn_Exp*_T15E2;struct _fat_ptr*_T15E3;struct Cyc_Yystacktype*_T15E4;union Cyc_YYSTYPE*_T15E5;union Cyc_YYSTYPE*_T15E6;struct Cyc_Yystacktype*_T15E7;struct Cyc_Yystacktype _T15E8;struct Cyc_Yyltype _T15E9;unsigned _T15EA;unsigned _T15EB;struct Cyc_Absyn_Exp*_T15EC;struct Cyc_Yystacktype*_T15ED;union Cyc_YYSTYPE*_T15EE;union Cyc_YYSTYPE*_T15EF;struct Cyc_Absyn_Exp*_T15F0;struct _fat_ptr*_T15F1;struct Cyc_Yystacktype*_T15F2;union Cyc_YYSTYPE*_T15F3;union Cyc_YYSTYPE*_T15F4;struct Cyc_Yystacktype*_T15F5;struct Cyc_Yystacktype _T15F6;struct Cyc_Yyltype _T15F7;unsigned _T15F8;unsigned _T15F9;struct Cyc_Absyn_Exp*_T15FA;struct Cyc_Yystacktype*_T15FB;union Cyc_YYSTYPE*_T15FC;union Cyc_YYSTYPE*_T15FD;struct Cyc_Absyn_Exp*_T15FE;struct Cyc_Yystacktype*_T15FF;struct Cyc_Yystacktype _T1600;struct Cyc_Yyltype _T1601;unsigned _T1602;unsigned _T1603;struct Cyc_Absyn_Exp*_T1604;struct Cyc_Yystacktype*_T1605;union Cyc_YYSTYPE*_T1606;union Cyc_YYSTYPE*_T1607;struct Cyc_Absyn_Exp*_T1608;struct Cyc_Yystacktype*_T1609;struct Cyc_Yystacktype _T160A;struct Cyc_Yyltype _T160B;unsigned _T160C;unsigned _T160D;struct Cyc_Absyn_Exp*_T160E;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T160F;struct Cyc_Yystacktype*_T1610;union Cyc_YYSTYPE*_T1611;union Cyc_YYSTYPE*_T1612;void*_T1613;struct Cyc_Yystacktype*_T1614;struct Cyc_Yystacktype _T1615;struct Cyc_Yyltype _T1616;unsigned _T1617;unsigned _T1618;struct Cyc_Absyn_Exp*_T1619;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T161A;struct Cyc_Yystacktype*_T161B;union Cyc_YYSTYPE*_T161C;union Cyc_YYSTYPE*_T161D;struct Cyc_Yystacktype*_T161E;union Cyc_YYSTYPE*_T161F;union Cyc_YYSTYPE*_T1620;struct Cyc_List_List*_T1621;void*_T1622;struct Cyc_Yystacktype*_T1623;struct Cyc_Yystacktype _T1624;struct Cyc_Yyltype _T1625;unsigned _T1626;unsigned _T1627;struct Cyc_Absyn_Exp*_T1628;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1629;struct Cyc_Yystacktype*_T162A;union Cyc_YYSTYPE*_T162B;union Cyc_YYSTYPE*_T162C;struct Cyc_Yystacktype*_T162D;union Cyc_YYSTYPE*_T162E;union Cyc_YYSTYPE*_T162F;struct Cyc_List_List*_T1630;void*_T1631;struct Cyc_Yystacktype*_T1632;struct Cyc_Yystacktype _T1633;struct Cyc_Yyltype _T1634;unsigned _T1635;unsigned _T1636;struct Cyc_Absyn_Exp*_T1637;struct Cyc_List_List*_T1638;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1639;struct _fat_ptr*_T163A;struct Cyc_Yystacktype*_T163B;union Cyc_YYSTYPE*_T163C;union Cyc_YYSTYPE*_T163D;struct Cyc_Yystacktype*_T163E;struct Cyc_Yystacktype _T163F;struct Cyc_Yyltype _T1640;unsigned _T1641;unsigned _T1642;struct Cyc_Yystacktype*_T1643;union Cyc_YYSTYPE*_T1644;union Cyc_YYSTYPE*_T1645;union Cyc_Absyn_Cnst _T1646;unsigned _T1647;int _T1648;struct Cyc_List_List*_T1649;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T164A;struct Cyc_List_List*_T164B;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T164C;struct _fat_ptr*_T164D;struct Cyc_Yystacktype*_T164E;union Cyc_YYSTYPE*_T164F;union Cyc_YYSTYPE*_T1650;struct Cyc_Yystacktype*_T1651;union Cyc_YYSTYPE*_T1652;union Cyc_YYSTYPE*_T1653;struct Cyc_Yystacktype*_T1654;struct Cyc_Yystacktype _T1655;struct Cyc_Yyltype _T1656;unsigned _T1657;unsigned _T1658;struct Cyc_Yystacktype*_T1659;union Cyc_YYSTYPE*_T165A;union Cyc_YYSTYPE*_T165B;union Cyc_Absyn_Cnst _T165C;unsigned _T165D;int _T165E;struct Cyc_List_List*_T165F;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1660;struct Cyc_Yystacktype*_T1661;union Cyc_YYSTYPE*_T1662;union Cyc_YYSTYPE*_T1663;struct Cyc_Yystacktype*_T1664;union Cyc_YYSTYPE*_T1665;union Cyc_YYSTYPE*_T1666;struct _tuple0*_T1667;struct Cyc_Yystacktype*_T1668;struct Cyc_Yystacktype _T1669;struct Cyc_Yyltype _T166A;unsigned _T166B;unsigned _T166C;struct Cyc_Absyn_Exp*_T166D;struct Cyc_Yystacktype*_T166E;union Cyc_YYSTYPE*_T166F;union Cyc_YYSTYPE*_T1670;struct _fat_ptr _T1671;struct Cyc_Yystacktype*_T1672;struct Cyc_Yystacktype _T1673;struct Cyc_Yyltype _T1674;unsigned _T1675;unsigned _T1676;struct Cyc_Absyn_Exp*_T1677;struct Cyc_Yystacktype*_T1678;struct Cyc_Yystacktype _T1679;struct Cyc_Yystacktype*_T167A;union Cyc_YYSTYPE*_T167B;union Cyc_YYSTYPE*_T167C;struct _fat_ptr _T167D;struct Cyc_Yystacktype*_T167E;struct Cyc_Yystacktype _T167F;struct Cyc_Yyltype _T1680;unsigned _T1681;unsigned _T1682;struct Cyc_Absyn_Exp*_T1683;struct Cyc_Yystacktype*_T1684;union Cyc_YYSTYPE*_T1685;union Cyc_YYSTYPE*_T1686;struct _fat_ptr _T1687;struct Cyc_Yystacktype*_T1688;struct Cyc_Yystacktype _T1689;struct Cyc_Yyltype _T168A;unsigned _T168B;unsigned _T168C;struct Cyc_Absyn_Exp*_T168D;struct Cyc_Yystacktype*_T168E;struct Cyc_Yystacktype _T168F;struct Cyc_Yystacktype*_T1690;union Cyc_YYSTYPE*_T1691;union Cyc_YYSTYPE*_T1692;struct Cyc_Absyn_Exp*_T1693;struct Cyc_Yystacktype*_T1694;struct Cyc_Yystacktype _T1695;struct Cyc_Yyltype _T1696;unsigned _T1697;unsigned _T1698;struct Cyc_Absyn_Exp*_T1699;struct Cyc_Yystacktype*_T169A;union Cyc_YYSTYPE*_T169B;union Cyc_YYSTYPE*_T169C;struct Cyc_Absyn_Exp*_T169D;struct Cyc_Yystacktype*_T169E;union Cyc_YYSTYPE*_T169F;union Cyc_YYSTYPE*_T16A0;struct Cyc_List_List*_T16A1;struct Cyc_List_List*_T16A2;struct Cyc_Yystacktype*_T16A3;struct Cyc_Yystacktype _T16A4;struct Cyc_Yyltype _T16A5;unsigned _T16A6;unsigned _T16A7;struct Cyc_Absyn_Exp*_T16A8;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T16A9;struct Cyc_Yystacktype*_T16AA;union Cyc_YYSTYPE*_T16AB;union Cyc_YYSTYPE*_T16AC;struct Cyc_Yystacktype*_T16AD;union Cyc_YYSTYPE*_T16AE;union Cyc_YYSTYPE*_T16AF;struct Cyc_Yystacktype*_T16B0;union Cyc_YYSTYPE*_T16B1;union Cyc_YYSTYPE*_T16B2;struct Cyc_List_List*_T16B3;void*_T16B4;struct Cyc_Yystacktype*_T16B5;struct Cyc_Yystacktype _T16B6;struct Cyc_Yyltype _T16B7;unsigned _T16B8;unsigned _T16B9;struct Cyc_Absyn_Exp*_T16BA;struct Cyc_Yystacktype*_T16BB;union Cyc_YYSTYPE*_T16BC;union Cyc_YYSTYPE*_T16BD;struct Cyc_List_List*_T16BE;struct Cyc_Yystacktype*_T16BF;struct Cyc_Yystacktype _T16C0;struct Cyc_Yyltype _T16C1;unsigned _T16C2;unsigned _T16C3;struct Cyc_Absyn_Exp*_T16C4;struct Cyc_Yystacktype*_T16C5;union Cyc_YYSTYPE*_T16C6;union Cyc_YYSTYPE*_T16C7;struct Cyc_Absyn_Stmt*_T16C8;struct Cyc_Yystacktype*_T16C9;struct Cyc_Yystacktype _T16CA;struct Cyc_Yyltype _T16CB;unsigned _T16CC;unsigned _T16CD;struct Cyc_Absyn_Exp*_T16CE;struct Cyc_Yystacktype*_T16CF;union Cyc_YYSTYPE*_T16D0;union Cyc_YYSTYPE*_T16D1;struct Cyc_List_List*_T16D2;struct Cyc_List_List*_T16D3;struct Cyc_List_List*_T16D4;struct Cyc_Yystacktype*_T16D5;union Cyc_YYSTYPE*_T16D6;union Cyc_YYSTYPE*_T16D7;struct Cyc_List_List*_T16D8;struct Cyc_Yystacktype*_T16D9;union Cyc_YYSTYPE*_T16DA;union Cyc_YYSTYPE*_T16DB;struct Cyc_Yystacktype*_T16DC;union Cyc_YYSTYPE*_T16DD;union Cyc_YYSTYPE*_T16DE;struct Cyc_Yystacktype*_T16DF;union Cyc_YYSTYPE*_T16E0;union Cyc_YYSTYPE*_T16E1;union Cyc_Absyn_Cnst _T16E2;struct Cyc_Yystacktype*_T16E3;struct Cyc_Yystacktype _T16E4;struct Cyc_Yyltype _T16E5;unsigned _T16E6;unsigned _T16E7;struct Cyc_Absyn_Exp*_T16E8;struct Cyc_Yystacktype*_T16E9;union Cyc_YYSTYPE*_T16EA;union Cyc_YYSTYPE*_T16EB;char _T16EC;struct Cyc_Yystacktype*_T16ED;struct Cyc_Yystacktype _T16EE;struct Cyc_Yyltype _T16EF;unsigned _T16F0;unsigned _T16F1;struct Cyc_Absyn_Exp*_T16F2;struct Cyc_Yystacktype*_T16F3;union Cyc_YYSTYPE*_T16F4;union Cyc_YYSTYPE*_T16F5;struct _fat_ptr _T16F6;struct Cyc_Yystacktype*_T16F7;struct Cyc_Yystacktype _T16F8;struct Cyc_Yyltype _T16F9;unsigned _T16FA;unsigned _T16FB;struct Cyc_Absyn_Exp*_T16FC;struct Cyc_Yystacktype*_T16FD;struct Cyc_Yystacktype _T16FE;struct Cyc_Yyltype _T16FF;unsigned _T1700;unsigned _T1701;struct Cyc_Absyn_Exp*_T1702;struct Cyc_Yystacktype*_T1703;union Cyc_YYSTYPE*_T1704;union Cyc_YYSTYPE*_T1705;unsigned long _T1706;struct _fat_ptr _T1707;unsigned char*_T1708;const char*_T1709;int _T170A;char _T170B;int _T170C;char _T170D;int _T170E;char _T170F;int _T1710;char _T1711;int _T1712;struct _fat_ptr _T1713;int _T1714;struct Cyc_Yystacktype*_T1715;struct Cyc_Yystacktype _T1716;struct Cyc_Yyltype _T1717;unsigned _T1718;unsigned _T1719;struct Cyc_Absyn_Exp*_T171A;struct _tuple0*_T171B;struct _fat_ptr*_T171C;struct Cyc_Yystacktype*_T171D;union Cyc_YYSTYPE*_T171E;union Cyc_YYSTYPE*_T171F;struct Cyc_Yystacktype*_T1720;struct Cyc_Yystacktype _T1721;struct _tuple0*_T1722;struct _fat_ptr*_T1723;struct Cyc_Yystacktype*_T1724;union Cyc_YYSTYPE*_T1725;union Cyc_YYSTYPE*_T1726;struct Cyc_Yystacktype*_T1727;struct Cyc_Yystacktype _T1728;struct Cyc_Yystacktype*_T1729;struct Cyc_Yystacktype _T172A;struct Cyc_Yystacktype*_T172B;struct Cyc_Yystacktype _T172C;struct Cyc_Yystacktype*_T172D;struct Cyc_Yystacktype _T172E;struct Cyc_Yystacktype*_T172F;struct Cyc_Yystacktype _T1730;struct Cyc_Lexing_lexbuf*_T1731;struct Cyc_List_List*_T1732;struct _tuple35*_T1733;struct Cyc_Yystacktype*_T1734;union Cyc_YYSTYPE*_T1735;union Cyc_YYSTYPE*_T1736;struct _fat_ptr _T1737;struct Cyc_Yystacktype*_T1738;union Cyc_YYSTYPE*_T1739;union Cyc_YYSTYPE*_T173A;struct _fat_ptr _T173B;struct _fat_ptr _T173C;int _T173D;int _T173E;struct Cyc_Yystacktype*_T173F;union Cyc_YYSTYPE*_T1740;union Cyc_YYSTYPE*_T1741;struct _fat_ptr _T1742;void*_T1743;struct Cyc_Yystacktype*_T1744;union Cyc_YYSTYPE*_T1745;union Cyc_YYSTYPE*_T1746;struct Cyc_Yystacktype*_T1747;union Cyc_YYSTYPE*_T1748;union Cyc_YYSTYPE*_T1749;struct Cyc_Yystacktype*_T174A;union Cyc_YYSTYPE*_T174B;union Cyc_YYSTYPE*_T174C;struct Cyc_List_List*_T174D;struct Cyc_List_List*_T174E;struct Cyc_Yystacktype*_T174F;union Cyc_YYSTYPE*_T1750;union Cyc_YYSTYPE*_T1751;struct Cyc_List_List*_T1752;struct Cyc_Yystacktype*_T1753;union Cyc_YYSTYPE*_T1754;union Cyc_YYSTYPE*_T1755;struct Cyc_Yystacktype*_T1756;union Cyc_YYSTYPE*_T1757;union Cyc_YYSTYPE*_T1758;struct Cyc_Yystacktype*_T1759;union Cyc_YYSTYPE*_T175A;union Cyc_YYSTYPE*_T175B;struct _fat_ptr _T175C;void*_T175D;struct Cyc_Yystacktype*_T175E;struct Cyc_Yystacktype _T175F;struct Cyc_Yyltype _T1760;unsigned _T1761;unsigned _T1762;struct Cyc_Yystacktype*_T1763;union Cyc_YYSTYPE*_T1764;union Cyc_YYSTYPE*_T1765;struct _fat_ptr _T1766;void*_T1767;struct Cyc_Yystacktype*_T1768;union Cyc_YYSTYPE*_T1769;union Cyc_YYSTYPE*_T176A;struct _fat_ptr _T176B;struct Cyc_Yystacktype*_T176C;union Cyc_YYSTYPE*_T176D;union Cyc_YYSTYPE*_T176E;void*_T176F;void*_T1770;void*_T1771;struct Cyc_Yystacktype*_T1772;union Cyc_YYSTYPE*_T1773;union Cyc_YYSTYPE*_T1774;struct _fat_ptr _T1775;struct Cyc_Yystacktype*_T1776;union Cyc_YYSTYPE*_T1777;union Cyc_YYSTYPE*_T1778;enum Cyc_Parse_ConstraintOps _T1779;struct Cyc_Yystacktype*_T177A;union Cyc_YYSTYPE*_T177B;union Cyc_YYSTYPE*_T177C;void*_T177D;struct Cyc_Yystacktype*_T177E;union Cyc_YYSTYPE*_T177F;union Cyc_YYSTYPE*_T1780;void*_T1781;void*_T1782;void*_T1783;struct Cyc_Yystacktype*_T1784;union Cyc_YYSTYPE*_T1785;union Cyc_YYSTYPE*_T1786;struct _fat_ptr _T1787;struct Cyc_Yystacktype*_T1788;union Cyc_YYSTYPE*_T1789;union Cyc_YYSTYPE*_T178A;void*_T178B;struct Cyc_Yystacktype*_T178C;union Cyc_YYSTYPE*_T178D;union Cyc_YYSTYPE*_T178E;void*_T178F;void*_T1790;void*_T1791;struct Cyc_Yystacktype*_T1792;union Cyc_YYSTYPE*_T1793;union Cyc_YYSTYPE*_T1794;struct _fat_ptr _T1795;struct Cyc_Yystacktype*_T1796;union Cyc_YYSTYPE*_T1797;union Cyc_YYSTYPE*_T1798;void*_T1799;struct Cyc_Yystacktype*_T179A;union Cyc_YYSTYPE*_T179B;union Cyc_YYSTYPE*_T179C;void*_T179D;void*_T179E;void*_T179F;struct Cyc_Yystacktype*_T17A0;union Cyc_YYSTYPE*_T17A1;union Cyc_YYSTYPE*_T17A2;struct _fat_ptr _T17A3;struct Cyc_Yystacktype*_T17A4;union Cyc_YYSTYPE*_T17A5;union Cyc_YYSTYPE*_T17A6;void*_T17A7;struct Cyc_Yystacktype*_T17A8;union Cyc_YYSTYPE*_T17A9;union Cyc_YYSTYPE*_T17AA;void*_T17AB;void*_T17AC;void*_T17AD;struct Cyc_Yystacktype*_T17AE;union Cyc_YYSTYPE*_T17AF;union Cyc_YYSTYPE*_T17B0;struct _fat_ptr _T17B1;struct Cyc_Yystacktype*_T17B2;union Cyc_YYSTYPE*_T17B3;union Cyc_YYSTYPE*_T17B4;void*_T17B5;struct Cyc_Yystacktype*_T17B6;union Cyc_YYSTYPE*_T17B7;union Cyc_YYSTYPE*_T17B8;void*_T17B9;void*_T17BA;void*_T17BB;struct Cyc_Yystacktype*_T17BC;union Cyc_YYSTYPE*_T17BD;union Cyc_YYSTYPE*_T17BE;struct _fat_ptr _T17BF;struct Cyc_Yystacktype*_T17C0;union Cyc_YYSTYPE*_T17C1;union Cyc_YYSTYPE*_T17C2;enum Cyc_Parse_ConstraintOps _T17C3;struct Cyc_Yystacktype*_T17C4;union Cyc_YYSTYPE*_T17C5;union Cyc_YYSTYPE*_T17C6;void*_T17C7;struct Cyc_Yystacktype*_T17C8;union Cyc_YYSTYPE*_T17C9;union Cyc_YYSTYPE*_T17CA;void*_T17CB;void*_T17CC;void*_T17CD;int _T17CE;int _T17CF;struct _fat_ptr _T17D0;int _T17D1;char*_T17D2;struct Cyc_Yystacktype*_T17D3;struct _fat_ptr _T17D4;int _T17D5;struct _fat_ptr _T17D6;char*_T17D7;char*_T17D8;struct Cyc_Yystacktype*_T17D9;struct Cyc_Yyltype _T17DA;struct Cyc_Yystacktype*_T17DB;struct Cyc_Yyltype _T17DC;struct Cyc_Yystacktype*_T17DD;struct Cyc_Yystacktype*_T17DE;struct Cyc_Yystacktype _T17DF;struct Cyc_Yyltype _T17E0;struct Cyc_Yystacktype*_T17E1;struct Cyc_Yystacktype*_T17E2;struct Cyc_Yystacktype _T17E3;struct Cyc_Yyltype _T17E4;struct _fat_ptr _T17E5;unsigned char*_T17E6;struct Cyc_Yystacktype*_T17E7;int _T17E8;struct _fat_ptr _T17E9;int _T17EA;int _T17EB;char*_T17EC;struct Cyc_Yystacktype*_T17ED;struct Cyc_Yystacktype _T17EE;struct Cyc_Yyltype _T17EF;struct _fat_ptr _T17F0;unsigned char*_T17F1;struct Cyc_Yystacktype*_T17F2;int _T17F3;struct _fat_ptr _T17F4;unsigned char*_T17F5;struct Cyc_Yystacktype*_T17F6;int _T17F7;int _T17F8;struct Cyc_Yystacktype _T17F9;struct Cyc_Yyltype _T17FA;short*_T17FB;int _T17FC;char*_T17FD;short*_T17FE;short _T17FF;short*_T1800;int _T1801;char*_T1802;short*_T1803;short _T1804;int _T1805;struct _fat_ptr _T1806;int _T1807;char*_T1808;short*_T1809;short _T180A;int _T180B;short*_T180C;int _T180D;short _T180E;int _T180F;struct _fat_ptr _T1810;unsigned char*_T1811;short*_T1812;int _T1813;short _T1814;int _T1815;short*_T1816;int _T1817;short _T1818;short*_T1819;int _T181A;char*_T181B;short*_T181C;short _T181D;short*_T181E;int _T181F;char*_T1820;short*_T1821;short _T1822;int _T1823;int _T1824;unsigned _T1825;unsigned _T1826;short*_T1827;int _T1828;char*_T1829;short*_T182A;short _T182B;int _T182C;int _T182D;struct _fat_ptr*_T182E;int _T182F;char*_T1830;struct _fat_ptr*_T1831;struct _fat_ptr _T1832;unsigned long _T1833;unsigned long _T1834;struct _fat_ptr _T1835;int _T1836;unsigned _T1837;char*_T1838;struct _RegionHandle*_T1839;unsigned _T183A;int _T183B;unsigned _T183C;struct _fat_ptr _T183D;struct _fat_ptr _T183E;int _T183F;int _T1840;unsigned _T1841;unsigned _T1842;short*_T1843;int _T1844;char*_T1845;short*_T1846;short _T1847;int _T1848;int _T1849;struct _fat_ptr _T184A;struct _fat_ptr _T184B;struct _fat_ptr _T184C;struct _fat_ptr _T184D;struct _fat_ptr _T184E;struct _fat_ptr*_T184F;int _T1850;char*_T1851;struct _fat_ptr*_T1852;struct _fat_ptr _T1853;struct _fat_ptr _T1854;struct _fat_ptr _T1855;struct _fat_ptr _T1856;int _T1857;int _T1858;struct _fat_ptr _T1859;int _T185A;char*_T185B;short*_T185C;short _T185D;short*_T185E;int _T185F;char*_T1860;short*_T1861;short _T1862;short*_T1863;int _T1864;short _T1865;int _T1866;short*_T1867;int _T1868;short _T1869;struct _fat_ptr _T186A;int _T186B;char*_T186C;struct Cyc_Yystacktype*_T186D;struct Cyc_Yystacktype _T186E;struct _RegionHandle _T186F=_new_region(0U,"yyregion");struct _RegionHandle*yyregion=& _T186F;_push_region(yyregion);{
# 149
int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;{union Cyc_YYSTYPE _T1870;_T1870.YYINITIALSVAL.tag=1U;
_T1870.YYINITIALSVAL.val=0;_T0=_T1870;}{union Cyc_YYSTYPE yylval=_T0;
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;{unsigned _T1870=200U;_T2=yyregion;_T3=_region_calloc(_T2,0U,sizeof(short),_T1870);_T1=_tag_fat(_T3,sizeof(short),_T1870);}{
# 164
struct _fat_ptr yyss=_T1;
# 166
int yyvsp_offset;{unsigned _T1870=200U;_T6=yyregion;_T7=_check_times(_T1870,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T1871=_region_malloc(_T6,0U,_T7);{unsigned _T1872=200U;unsigned i;i=0;_TL250: if(i < _T1872)goto _TL24E;else{goto _TL24F;}_TL24E: _T8=i;
# 169
_T1871[_T8].v=yylval;_T9=i;_T1871[_T9].l=Cyc_yynewloc();i=i + 1;goto _TL250;_TL24F:;}_T5=(struct Cyc_Yystacktype*)_T1871;}_T4=_T5;}{
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
yychar=-2;
# 200
yyssp_offset=-1;
yyvsp_offset=0;
# 206
yynewstate: _TA=yyss;
# 208
yyssp_offset=yyssp_offset + 1;_TB=yyssp_offset;_TC=_check_fat_subscript(_TA,sizeof(short),_TB);_TD=(short*)_TC;_TE=yystate;*_TD=(short)_TE;_TF=yyssp_offset;_T10=yystacksize - 1;_T11=_T10 - 14;
# 210
if(_TF < _T11)goto _TL251;
# 212
if(yystacksize < 10000)goto _TL253;_T12=
_tag_fat("parser stack overflow",sizeof(char),22U);_T13=yystate;_T14=yychar;Cyc_yyerror(_T12,_T13,_T14);_T15=& Cyc_Yystack_overflow_val;_T16=(struct Cyc_Yystack_overflow_exn_struct*)_T15;_throw(_T16);goto _TL254;_TL253: _TL254:
# 216
 yystacksize=yystacksize * 2;
if(yystacksize <= 10000)goto _TL255;
yystacksize=10000;goto _TL256;_TL255: _TL256: _T18=yystacksize;{unsigned _T1870=(unsigned)_T18;_T1A=yyregion;_T1B=_check_times(_T1870,sizeof(short));{short*_T1871=_region_malloc(_T1A,0U,_T1B);_T1C=yystacksize;{unsigned _T1872=(unsigned)_T1C;unsigned i;i=0;_TL25A: if(i < _T1872)goto _TL258;else{goto _TL259;}_TL258: _T1D=i;_T1E=yyssp_offset;_T1F=(unsigned)_T1E;
# 220
if(_T1D > _T1F)goto _TL25B;_T20=i;_T21=yyss;_T22=_T21.curr;_T23=(short*)_T22;_T24=i;_T25=(int)_T24;_T1871[_T20]=_T23[_T25];goto _TL25C;_TL25B: _T26=i;_T1871[_T26]=0;_TL25C: i=i + 1;goto _TL25A;_TL259:;}_T19=(short*)_T1871;}_T17=
# 219
_tag_fat(_T19,sizeof(short),_T1870);}{struct _fat_ptr yyss1=_T17;_T28=yystacksize;{unsigned _T1870=(unsigned)_T28;_T2A=yyregion;_T2B=_check_times(_T1870,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T1871=_region_malloc(_T2A,0U,_T2B);_T2C=yystacksize;{unsigned _T1872=(unsigned)_T2C;unsigned i;i=0;_TL260: if(i < _T1872)goto _TL25E;else{goto _TL25F;}_TL25E: _T2D=i;_T2E=yyssp_offset;_T2F=(unsigned)_T2E;
# 224
if(_T2D > _T2F)goto _TL261;_T30=i;_T31=yyvs;_T32=i;_T33=(int)_T32;_T34=_check_fat_subscript(_T31,sizeof(struct Cyc_Yystacktype),_T33);_T35=(struct Cyc_Yystacktype*)_T34;_T1871[_T30]=*_T35;goto _TL262;_TL261: _T36=i;_T37=yyvs;_T38=_check_fat_subscript(_T37,sizeof(struct Cyc_Yystacktype),0);_T39=(struct Cyc_Yystacktype*)_T38;_T1871[_T36]=*_T39;_TL262: i=i + 1;goto _TL260;_TL25F:;}_T29=(struct Cyc_Yystacktype*)_T1871;}_T27=
# 223
_tag_fat(_T29,sizeof(struct Cyc_Yystacktype),_T1870);}{
# 222
struct _fat_ptr yyvs1=_T27;
# 230
yyss=yyss1;
yyvs=yyvs1;}}goto _TL252;_TL251: _TL252: goto yybackup;
# 242
yybackup: _T3A=Cyc_yypact;_T3B=yystate;_T3C=_check_known_subscript_notnull(_T3A,1262U,sizeof(short),_T3B);_T3D=(short*)_T3C;_T3E=*_T3D;
# 254 "cycbison.simple"
yyn=(int)_T3E;
if(yyn!=-32768)goto _TL263;goto yydefault;_TL263:
# 261
 if(yychar!=-2)goto _TL265;_T3F=yylex_buf;_T40=& yylval;_T41=(union Cyc_YYSTYPE*)_T40;_T42=& yylloc;_T43=(struct Cyc_Yyltype*)_T42;
# 267
yychar=Cyc_yylex(_T3F,_T41,_T43);goto _TL266;_TL265: _TL266:
# 271
 if(yychar > 0)goto _TL267;
# 273
yychar1=0;
yychar=0;goto _TL268;
# 282
_TL267: if(yychar <= 0)goto _TL269;if(yychar > 400)goto _TL269;_T45=Cyc_yytranslate;_T46=yychar;_T47=_T45[_T46];_T44=(int)_T47;goto _TL26A;_TL269: _T44=363;_TL26A: yychar1=_T44;_TL268: _T48=yychar1;
# 299 "cycbison.simple"
yyn=yyn + _T48;
if(yyn < 0)goto _TL26D;else{goto _TL26F;}_TL26F: if(yyn > 8241)goto _TL26D;else{goto _TL26E;}_TL26E: _T49=Cyc_yycheck;_T4A=yyn;_T4B=_T49[_T4A];_T4C=(int)_T4B;_T4D=yychar1;if(_T4C!=_T4D)goto _TL26D;else{goto _TL26B;}_TL26D: goto yydefault;_TL26B: _T4E=Cyc_yytable;_T4F=yyn;_T50=_T4E[_T4F];
# 302
yyn=(int)_T50;
# 309
if(yyn >= 0)goto _TL270;
# 311
if(yyn!=-32768)goto _TL272;goto yyerrlab;_TL272:
 yyn=- yyn;goto yyreduce;
# 315
_TL270: if(yyn!=0)goto _TL274;goto yyerrlab;_TL274:
# 317
 if(yyn!=1261)goto _TL276;{int _T1870=0;_npop_handler(0);return _T1870;}_TL276:
# 328 "cycbison.simple"
 if(yychar==0)goto _TL278;
yychar=-2;goto _TL279;_TL278: _TL279: _T51=yyvs;
# 332
yyvsp_offset=yyvsp_offset + 1;_T52=yyvsp_offset;_T53=_check_fat_subscript(_T51,sizeof(struct Cyc_Yystacktype),_T52);_T54=(struct Cyc_Yystacktype*)_T53;{struct Cyc_Yystacktype _T1870;_T1870.v=yylval;_T1870.l=yylloc;_T55=_T1870;}*_T54=_T55;
# 338
if(yyerrstatus==0)goto _TL27A;yyerrstatus=yyerrstatus + -1;goto _TL27B;_TL27A: _TL27B:
# 340
 yystate=yyn;goto yynewstate;
# 344
yydefault: _T56=Cyc_yydefact;_T57=yystate;_T58=_check_known_subscript_notnull(_T56,1262U,sizeof(short),_T57);_T59=(short*)_T58;_T5A=*_T59;
# 346
yyn=(int)_T5A;
if(yyn!=0)goto _TL27C;goto yyerrlab;_TL27C:
# 351
 yyreduce: _T5B=Cyc_yyr2;_T5C=yyn;_T5D=_check_known_subscript_notnull(_T5B,624U,sizeof(short),_T5C);_T5E=(short*)_T5D;_T5F=*_T5E;
# 353
yylen=(int)_T5F;_T60=yyvs;_T61=yyvsp_offset + 1;_T62=yylen;_T63=_T61 - _T62;_T64=
_fat_ptr_plus(_T60,sizeof(struct Cyc_Yystacktype),_T63);_T65=_untag_fat_ptr_check_bound(_T64,sizeof(struct Cyc_Yystacktype),14U);_T66=_check_null(_T65);yyyvsp=(struct Cyc_Yystacktype*)_T66;
if(yylen <= 0)goto _TL27E;_T67=yyyvsp;_T68=_T67[0];
yyval=_T68.v;goto _TL27F;_TL27E: _TL27F: _T69=yyn;_T6A=(int)_T69;switch(_T6A){case 1:
# 1323 "parse.y"
 yyval=Cyc_YY74(0);goto _LL0;case 2:
# 1326 "parse.y"
 yyval=Cyc_YY74(1);_T6B=yyyvsp;_T6C=& _T6B[0].v;_T6D=(union Cyc_YYSTYPE*)_T6C;
Cyc_Parse_constraint_graph=Cyc_yyget_YY71(_T6D);goto _LL0;case 3: _T6E=yyyvsp;_T6F=_T6E[0];
# 1333 "parse.y"
yyval=_T6F.v;_T70=yyyvsp;_T71=& _T70[0].v;_T72=(union Cyc_YYSTYPE*)_T71;
Cyc_Parse_parse_result=Cyc_yyget_YY16(_T72);goto _LL0;case 4: _T73=yyyvsp;_T74=& _T73[0].v;_T75=(union Cyc_YYSTYPE*)_T74;_T76=
# 1339 "parse.y"
Cyc_yyget_YY16(_T75);_T77=yyyvsp;_T78=& _T77[1].v;_T79=(union Cyc_YYSTYPE*)_T78;_T7A=Cyc_yyget_YY16(_T79);_T7B=Cyc_List_imp_append(_T76,_T7A);yyval=Cyc_YY16(_T7B);goto _LL0;case 5:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1872=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T1872->tag=10;_T7F=yyyvsp;_T80=& _T7F[0].v;_T81=(union Cyc_YYSTYPE*)_T80;
# 1343 "parse.y"
_T1872->f1=Cyc_yyget_QualId_tok(_T81);_T82=yyyvsp;_T83=& _T82[2].v;_T84=(union Cyc_YYSTYPE*)_T83;_T1872->f2=Cyc_yyget_YY16(_T84);_T7E=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1872;}_T1871->r=(void*)_T7E;_T85=yyyvsp;_T86=_T85[0];_T87=_T86.l;_T88=_T87.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_T88);_T7D=(struct Cyc_Absyn_Decl*)_T1871;}_T1870->hd=_T7D;_T1870->tl=0;_T7C=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T7C);
Cyc_Lex_leave_using();goto _LL0;case 6:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1872=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T1872->tag=10;_T8C=yyyvsp;_T8D=& _T8C[0].v;_T8E=(union Cyc_YYSTYPE*)_T8D;
# 1347 "parse.y"
_T1872->f1=Cyc_yyget_QualId_tok(_T8E);_T8F=yyyvsp;_T90=& _T8F[2].v;_T91=(union Cyc_YYSTYPE*)_T90;_T1872->f2=Cyc_yyget_YY16(_T91);_T8B=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1872;}_T1871->r=(void*)_T8B;_T92=yyyvsp;_T93=_T92[0];_T94=_T93.l;_T95=_T94.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_T95);_T8A=(struct Cyc_Absyn_Decl*)_T1871;}_T1870->hd=_T8A;_T96=yyyvsp;_T97=& _T96[4].v;_T98=(union Cyc_YYSTYPE*)_T97;_T1870->tl=Cyc_yyget_YY16(_T98);_T89=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T89);goto _LL0;case 7:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1872=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T1872->tag=9;{struct _fat_ptr*_T1873=_cycalloc(sizeof(struct _fat_ptr));_T9D=yyyvsp;_T9E=& _T9D[0].v;_T9F=(union Cyc_YYSTYPE*)_T9E;
# 1350
*_T1873=Cyc_yyget_String_tok(_T9F);_T9C=(struct _fat_ptr*)_T1873;}_T1872->f1=_T9C;_TA0=yyyvsp;_TA1=& _TA0[2].v;_TA2=(union Cyc_YYSTYPE*)_TA1;_T1872->f2=Cyc_yyget_YY16(_TA2);_T9B=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1872;}_T1871->r=(void*)_T9B;_TA3=yyyvsp;_TA4=_TA3[0];_TA5=_TA4.l;_TA6=_TA5.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_TA6);_T9A=(struct Cyc_Absyn_Decl*)_T1871;}_T1870->hd=_T9A;_T1870->tl=0;_T99=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T99);
Cyc_Lex_leave_namespace();goto _LL0;case 8:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1872=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T1872->tag=9;{struct _fat_ptr*_T1873=_cycalloc(sizeof(struct _fat_ptr));_TAB=yyyvsp;_TAC=& _TAB[0].v;_TAD=(union Cyc_YYSTYPE*)_TAC;
# 1354 "parse.y"
*_T1873=Cyc_yyget_String_tok(_TAD);_TAA=(struct _fat_ptr*)_T1873;}_T1872->f1=_TAA;_TAE=yyyvsp;_TAF=& _TAE[2].v;_TB0=(union Cyc_YYSTYPE*)_TAF;_T1872->f2=Cyc_yyget_YY16(_TB0);_TA9=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1872;}_T1871->r=(void*)_TA9;_TB1=yyyvsp;_TB2=_TB1[0];_TB3=_TB2.l;_TB4=_TB3.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_TB4);_TA8=(struct Cyc_Absyn_Decl*)_T1871;}_T1870->hd=_TA8;_TB5=yyyvsp;_TB6=& _TB5[4].v;_TB7=(union Cyc_YYSTYPE*)_TB6;_T1870->tl=Cyc_yyget_YY16(_TB7);_TA7=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_TA7);goto _LL0;case 9: _TB8=yyyvsp;_TB9=& _TB8[2].v;_TBA=(union Cyc_YYSTYPE*)_TB9;_TBB=
# 1356 "parse.y"
Cyc_yyget_YY16(_TBA);_TBC=yyyvsp;_TBD=& _TBC[4].v;_TBE=(union Cyc_YYSTYPE*)_TBD;_TBF=Cyc_yyget_YY16(_TBE);_TC0=Cyc_List_append(_TBB,_TBF);yyval=Cyc_YY16(_TC0);goto _LL0;case 10: _TC1=yyyvsp;_TC2=& _TC1[0].v;_TC3=(union Cyc_YYSTYPE*)_TC2;{
# 1358 "parse.y"
int is_c_include=Cyc_yyget_YY32(_TC3);_TC4=yyyvsp;_TC5=& _TC4[4].v;_TC6=(union Cyc_YYSTYPE*)_TC5;{
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(_TC6);_TC7=yyyvsp;_TC8=& _TC7[5].v;_TC9=(union Cyc_YYSTYPE*)_TC8;{
struct _tuple30*_T1870=Cyc_yyget_YY56(_TC9);unsigned _T1871;struct Cyc_List_List*_T1872;{struct _tuple30 _T1873=*_T1870;_T1872=_T1873.f0;_T1871=_T1873.f1;}{struct Cyc_List_List*exs=_T1872;unsigned wc=_T1871;_TCA=yyyvsp;_TCB=& _TCA[6].v;_TCC=(union Cyc_YYSTYPE*)_TCB;{
struct Cyc_List_List*hides=Cyc_yyget_YY57(_TCC);
if(exs==0)goto _TL281;if(hides==0)goto _TL281;_TCD=yyyvsp;_TCE=_TCD[0];_TCF=_TCE.l;_TD0=_TCF.first_line;_TD1=
Cyc_Position_loc_to_seg(_TD0);_TD2=_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U);_TD3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TD1,_TD2,_TD3);goto _TL282;_TL281: _TL282: _TD4=hides;_TD5=(unsigned)_TD4;
if(!_TD5)goto _TL283;_TD6=wc;_TD7=(int)_TD6;if(_TD7)goto _TL283;else{goto _TL285;}
_TL285: _TD8=yyyvsp;_TD9=_TD8[6];_TDA=_TD9.l;_TDB=_TDA.first_line;wc=Cyc_Position_loc_to_seg(_TDB);goto _TL284;_TL283: _TL284: _TDC=is_c_include;
if(_TDC)goto _TL286;else{goto _TL288;}
_TL288: if(exs!=0)goto _TL28B;else{goto _TL28C;}_TL28C: if(cycdecls!=0)goto _TL28B;else{goto _TL289;}
_TL28B: _TDD=yyyvsp;_TDE=_TDD[0];_TDF=_TDE.l;_TE0=_TDF.first_line;_TE1=Cyc_Position_loc_to_seg(_TE0);_TE2=_tag_fat("expecting \"C include\"",sizeof(char),22U);_TE3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TE1,_TE2,_TE3);{struct Cyc_List_List*_T1873=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1874=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1875=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T1875->tag=12;_TE7=yyyvsp;_TE8=& _TE7[2].v;_TE9=(union Cyc_YYSTYPE*)_TE8;
_T1875->f1=Cyc_yyget_YY16(_TE9);_T1875->f2=cycdecls;_T1875->f3=exs;{struct _tuple10*_T1876=_cycalloc(sizeof(struct _tuple10));_T1876->f0=wc;_T1876->f1=hides;_TEA=(struct _tuple10*)_T1876;}_T1875->f4=_TEA;_TE6=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1875;}_T1874->r=(void*)_TE6;_TEB=yyyvsp;_TEC=_TEB[0];_TED=_TEC.l;_TEE=_TED.first_line;_T1874->loc=Cyc_Position_loc_to_seg(_TEE);_TE5=(struct Cyc_Absyn_Decl*)_T1874;}_T1873->hd=_TE5;_TEF=yyyvsp;_TF0=& _TEF[7].v;_TF1=(union Cyc_YYSTYPE*)_TF0;_T1873->tl=Cyc_yyget_YY16(_TF1);_TE4=(struct Cyc_List_List*)_T1873;}yyval=Cyc_YY16(_TE4);goto _TL28A;
# 1372
_TL289:{struct Cyc_List_List*_T1873=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1874=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T1875=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_T1875->tag=11;_TF5=yyyvsp;_TF6=& _TF5[2].v;_TF7=(union Cyc_YYSTYPE*)_TF6;_T1875->f1=Cyc_yyget_YY16(_TF7);_TF4=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T1875;}_T1874->r=(void*)_TF4;_TF8=yyyvsp;_TF9=_TF8[0];_TFA=_TF9.l;_TFB=_TFA.first_line;_T1874->loc=Cyc_Position_loc_to_seg(_TFB);_TF3=(struct Cyc_Absyn_Decl*)_T1874;}_T1873->hd=_TF3;_TFC=yyyvsp;_TFD=& _TFC[7].v;_TFE=(union Cyc_YYSTYPE*)_TFD;_T1873->tl=Cyc_yyget_YY16(_TFE);_TF2=(struct Cyc_List_List*)_T1873;}yyval=Cyc_YY16(_TF2);_TL28A: goto _TL287;
# 1375
_TL286:{struct Cyc_List_List*_T1873=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1874=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1875=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T1875->tag=12;_T102=yyyvsp;_T103=& _T102[2].v;_T104=(union Cyc_YYSTYPE*)_T103;_T1875->f1=Cyc_yyget_YY16(_T104);_T1875->f2=cycdecls;_T1875->f3=exs;{struct _tuple10*_T1876=_cycalloc(sizeof(struct _tuple10));_T1876->f0=wc;_T1876->f1=hides;_T105=(struct _tuple10*)_T1876;}_T1875->f4=_T105;_T101=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1875;}_T1874->r=(void*)_T101;_T106=yyyvsp;_T107=_T106[0];_T108=_T107.l;_T109=_T108.first_line;_T1874->loc=Cyc_Position_loc_to_seg(_T109);_T100=(struct Cyc_Absyn_Decl*)_T1874;}_T1873->hd=_T100;_T10A=yyyvsp;_T10B=& _T10A[7].v;_T10C=(union Cyc_YYSTYPE*)_T10B;_T1873->tl=Cyc_yyget_YY16(_T10C);_TFF=(struct Cyc_List_List*)_T1873;}yyval=Cyc_YY16(_TFF);_TL287: goto _LL0;}}}}}case 11:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T10F=& Cyc_Absyn_Porton_d_val;_T110=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_T10F;
# 1378 "parse.y"
_T1871->r=(void*)_T110;_T111=yyyvsp;_T112=_T111[0];_T113=_T112.l;_T114=_T113.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_T114);_T10E=(struct Cyc_Absyn_Decl*)_T1871;}_T1870->hd=_T10E;_T115=yyyvsp;_T116=& _T115[2].v;_T117=(union Cyc_YYSTYPE*)_T116;_T1870->tl=Cyc_yyget_YY16(_T117);_T10D=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T10D);goto _LL0;case 12:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T11A=& Cyc_Absyn_Portoff_d_val;_T11B=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_T11A;
# 1380 "parse.y"
_T1871->r=(void*)_T11B;_T11C=yyyvsp;_T11D=_T11C[0];_T11E=_T11D.l;_T11F=_T11E.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_T11F);_T119=(struct Cyc_Absyn_Decl*)_T1871;}_T1870->hd=_T119;_T120=yyyvsp;_T121=& _T120[2].v;_T122=(union Cyc_YYSTYPE*)_T121;_T1870->tl=Cyc_yyget_YY16(_T122);_T118=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T118);goto _LL0;case 13:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T125=& Cyc_Absyn_Tempeston_d_val;_T126=(struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*)_T125;
# 1382 "parse.y"
_T1871->r=(void*)_T126;_T127=yyyvsp;_T128=_T127[0];_T129=_T128.l;_T12A=_T129.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_T12A);_T124=(struct Cyc_Absyn_Decl*)_T1871;}_T1870->hd=_T124;_T12B=yyyvsp;_T12C=& _T12B[2].v;_T12D=(union Cyc_YYSTYPE*)_T12C;_T1870->tl=Cyc_yyget_YY16(_T12D);_T123=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T123);goto _LL0;case 14:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T130=& Cyc_Absyn_Tempestoff_d_val;_T131=(struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*)_T130;
# 1384 "parse.y"
_T1871->r=(void*)_T131;_T132=yyyvsp;_T133=_T132[0];_T134=_T133.l;_T135=_T134.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_T135);_T12F=(struct Cyc_Absyn_Decl*)_T1871;}_T1870->hd=_T12F;_T136=yyyvsp;_T137=& _T136[2].v;_T138=(union Cyc_YYSTYPE*)_T137;_T1870->tl=Cyc_yyget_YY16(_T138);_T12E=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T12E);goto _LL0;case 15:
# 1385 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 16:
# 1389 "parse.y"
 Cyc_Parse_parsing_tempest=1;goto _LL0;case 17:
# 1392
 Cyc_Parse_parsing_tempest=0;goto _LL0;case 18:
# 1397 "parse.y"
 Cyc_Lex_enter_extern_c();_T139=yyyvsp;_T13A=& _T139[1].v;_T13B=(union Cyc_YYSTYPE*)_T13A;_T13C=
Cyc_yyget_String_tok(_T13B);_T13D=_tag_fat("C",sizeof(char),2U);_T13E=Cyc_strcmp(_T13C,_T13D);if(_T13E!=0)goto _TL28D;
yyval=Cyc_YY32(0);goto _TL28E;
_TL28D: _T13F=yyyvsp;_T140=& _T13F[1].v;_T141=(union Cyc_YYSTYPE*)_T140;_T142=Cyc_yyget_String_tok(_T141);_T143=_tag_fat("C include",sizeof(char),10U);_T144=Cyc_strcmp(_T142,_T143);if(_T144!=0)goto _TL28F;
yyval=Cyc_YY32(1);goto _TL290;
# 1403
_TL28F: _T145=yyyvsp;_T146=_T145[0];_T147=_T146.l;_T148=_T147.first_line;_T149=Cyc_Position_loc_to_seg(_T148);_T14A=_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U);_T14B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T149,_T14A,_T14B);
yyval=Cyc_YY32(1);_TL290: _TL28E: goto _LL0;case 19:
# 1410 "parse.y"
 Cyc_Lex_leave_extern_c();goto _LL0;case 20:
# 1414 "parse.y"
 yyval=Cyc_YY57(0);goto _LL0;case 21: _T14C=yyyvsp;_T14D=_T14C[2];
# 1415 "parse.y"
yyval=_T14D.v;goto _LL0;case 22:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T14F=yyyvsp;_T150=& _T14F[0].v;_T151=(union Cyc_YYSTYPE*)_T150;
# 1419 "parse.y"
_T1870->hd=Cyc_yyget_QualId_tok(_T151);_T1870->tl=0;_T14E=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY57(_T14E);goto _LL0;case 23:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T153=yyyvsp;_T154=& _T153[0].v;_T155=(union Cyc_YYSTYPE*)_T154;
# 1420 "parse.y"
_T1870->hd=Cyc_yyget_QualId_tok(_T155);_T1870->tl=0;_T152=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY57(_T152);goto _LL0;case 24:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T157=yyyvsp;_T158=& _T157[0].v;_T159=(union Cyc_YYSTYPE*)_T158;
# 1421 "parse.y"
_T1870->hd=Cyc_yyget_QualId_tok(_T159);_T15A=yyyvsp;_T15B=& _T15A[2].v;_T15C=(union Cyc_YYSTYPE*)_T15B;_T1870->tl=Cyc_yyget_YY57(_T15C);_T156=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY57(_T156);goto _LL0;case 25:{struct _tuple30*_T1870=_cycalloc(sizeof(struct _tuple30));
# 1425 "parse.y"
_T1870->f0=0;_T1870->f1=0U;_T15D=(struct _tuple30*)_T1870;}yyval=Cyc_YY56(_T15D);goto _LL0;case 26: _T15E=yyyvsp;_T15F=_T15E[0];
# 1426 "parse.y"
yyval=_T15F.v;goto _LL0;case 27:{struct _tuple30*_T1870=_cycalloc(sizeof(struct _tuple30));_T161=yyyvsp;_T162=& _T161[2].v;_T163=(union Cyc_YYSTYPE*)_T162;
# 1430 "parse.y"
_T1870->f0=Cyc_yyget_YY55(_T163);_T1870->f1=0U;_T160=(struct _tuple30*)_T1870;}yyval=Cyc_YY56(_T160);goto _LL0;case 28:{struct _tuple30*_T1870=_cycalloc(sizeof(struct _tuple30));
# 1431 "parse.y"
_T1870->f0=0;_T1870->f1=0U;_T164=(struct _tuple30*)_T1870;}yyval=Cyc_YY56(_T164);goto _LL0;case 29:{struct _tuple30*_T1870=_cycalloc(sizeof(struct _tuple30));
# 1432 "parse.y"
_T1870->f0=0;_T166=yyyvsp;_T167=_T166[0];_T168=_T167.l;_T169=_T168.first_line;_T1870->f1=Cyc_Position_loc_to_seg(_T169);_T165=(struct _tuple30*)_T1870;}yyval=Cyc_YY56(_T165);goto _LL0;case 30:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T1871=_cycalloc(sizeof(struct _tuple33));_T16C=yyyvsp;_T16D=_T16C[0];_T16E=_T16D.l;_T16F=_T16E.first_line;
# 1437 "parse.y"
_T1871->f0=Cyc_Position_loc_to_seg(_T16F);_T170=yyyvsp;_T171=& _T170[0].v;_T172=(union Cyc_YYSTYPE*)_T171;_T1871->f1=Cyc_yyget_QualId_tok(_T172);_T1871->f2=0;_T16B=(struct _tuple33*)_T1871;}_T1870->hd=_T16B;_T1870->tl=0;_T16A=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY55(_T16A);goto _LL0;case 31:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T1871=_cycalloc(sizeof(struct _tuple33));_T175=yyyvsp;_T176=_T175[0];_T177=_T176.l;_T178=_T177.first_line;
# 1439 "parse.y"
_T1871->f0=Cyc_Position_loc_to_seg(_T178);_T179=yyyvsp;_T17A=& _T179[0].v;_T17B=(union Cyc_YYSTYPE*)_T17A;_T1871->f1=Cyc_yyget_QualId_tok(_T17B);_T1871->f2=0;_T174=(struct _tuple33*)_T1871;}_T1870->hd=_T174;_T17C=yyyvsp;_T17D=& _T17C[2].v;_T17E=(union Cyc_YYSTYPE*)_T17D;_T1870->tl=Cyc_yyget_YY55(_T17E);_T173=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY55(_T173);goto _LL0;case 32:
# 1443 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 33: _T17F=yyyvsp;_T180=_T17F[2];
# 1444 "parse.y"
yyval=_T180.v;goto _LL0;case 34:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1871->tag=1;_T183=yyyvsp;_T184=& _T183[0].v;_T185=(union Cyc_YYSTYPE*)_T184;
# 1448 "parse.y"
_T1871->f1=Cyc_yyget_YY15(_T185);_T182=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1871;}_T186=(void*)_T182;_T187=yyyvsp;_T188=_T187[0];_T189=_T188.l;_T18A=_T189.first_line;_T18B=Cyc_Position_loc_to_seg(_T18A);_T1870->hd=Cyc_Absyn_new_decl(_T186,_T18B);_T1870->tl=0;_T181=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T181);goto _LL0;case 35: _T18C=yyyvsp;_T18D=_T18C[0];
# 1449 "parse.y"
yyval=_T18D.v;goto _LL0;case 36:
# 1450 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 39: _T18E=yyr;_T18F=yyyvsp;_T190=& _T18F[0].v;_T191=(union Cyc_YYSTYPE*)_T190;_T192=
# 1484 "parse.y"
Cyc_yyget_YY28(_T191);_T193=yyyvsp;_T194=& _T193[1].v;_T195=(union Cyc_YYSTYPE*)_T194;_T196=Cyc_yyget_Stmt_tok(_T195);_T197=yyyvsp;_T198=_T197[0];_T199=_T198.l;_T19A=_T199.first_line;_T19B=Cyc_Position_loc_to_seg(_T19A);_T19C=Cyc_Parse_make_function(_T18E,0,_T192,0,_T196,_T19B);yyval=Cyc_YY15(_T19C);goto _LL0;case 40: _T19D=yyyvsp;_T19E=& _T19D[0].v;_T19F=(union Cyc_YYSTYPE*)_T19E;{
# 1486 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T19F);_T1A0=yyr;_T1A1=& d;_T1A2=(struct Cyc_Parse_Declaration_spec*)_T1A1;_T1A3=yyyvsp;_T1A4=& _T1A3[1].v;_T1A5=(union Cyc_YYSTYPE*)_T1A4;_T1A6=
Cyc_yyget_YY28(_T1A5);_T1A7=yyyvsp;_T1A8=& _T1A7[2].v;_T1A9=(union Cyc_YYSTYPE*)_T1A8;_T1AA=Cyc_yyget_Stmt_tok(_T1A9);_T1AB=yyyvsp;_T1AC=_T1AB[0];_T1AD=_T1AC.l;_T1AE=_T1AD.first_line;_T1AF=Cyc_Position_loc_to_seg(_T1AE);_T1B0=Cyc_Parse_make_function(_T1A0,_T1A2,_T1A6,0,_T1AA,_T1AF);yyval=Cyc_YY15(_T1B0);goto _LL0;}case 41: _T1B1=yyr;_T1B2=yyyvsp;_T1B3=& _T1B2[0].v;_T1B4=(union Cyc_YYSTYPE*)_T1B3;_T1B5=
# 1498 "parse.y"
Cyc_yyget_YY28(_T1B4);_T1B6=yyyvsp;_T1B7=& _T1B6[1].v;_T1B8=(union Cyc_YYSTYPE*)_T1B7;_T1B9=Cyc_yyget_YY16(_T1B8);_T1BA=yyyvsp;_T1BB=& _T1BA[2].v;_T1BC=(union Cyc_YYSTYPE*)_T1BB;_T1BD=Cyc_yyget_Stmt_tok(_T1BC);_T1BE=yyyvsp;_T1BF=_T1BE[0];_T1C0=_T1BF.l;_T1C1=_T1C0.first_line;_T1C2=Cyc_Position_loc_to_seg(_T1C1);_T1C3=Cyc_Parse_make_function(_T1B1,0,_T1B5,_T1B9,_T1BD,_T1C2);yyval=Cyc_YY15(_T1C3);goto _LL0;case 42: _T1C4=yyyvsp;_T1C5=& _T1C4[0].v;_T1C6=(union Cyc_YYSTYPE*)_T1C5;{
# 1500 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1C6);_T1C7=yyr;_T1C8=& d;_T1C9=(struct Cyc_Parse_Declaration_spec*)_T1C8;_T1CA=yyyvsp;_T1CB=& _T1CA[1].v;_T1CC=(union Cyc_YYSTYPE*)_T1CB;_T1CD=
Cyc_yyget_YY28(_T1CC);_T1CE=yyyvsp;_T1CF=& _T1CE[2].v;_T1D0=(union Cyc_YYSTYPE*)_T1CF;_T1D1=Cyc_yyget_YY16(_T1D0);_T1D2=yyyvsp;_T1D3=& _T1D2[3].v;_T1D4=(union Cyc_YYSTYPE*)_T1D3;_T1D5=Cyc_yyget_Stmt_tok(_T1D4);_T1D6=yyyvsp;_T1D7=_T1D6[0];_T1D8=_T1D7.l;_T1D9=_T1D8.first_line;_T1DA=Cyc_Position_loc_to_seg(_T1D9);_T1DB=Cyc_Parse_make_function(_T1C7,_T1C9,_T1CD,_T1D1,_T1D5,_T1DA);yyval=Cyc_YY15(_T1DB);goto _LL0;}case 43: _T1DC=yyyvsp;_T1DD=& _T1DC[0].v;_T1DE=(union Cyc_YYSTYPE*)_T1DD;{
# 1508 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1DE);_T1DF=yyr;_T1E0=& d;_T1E1=(struct Cyc_Parse_Declaration_spec*)_T1E0;_T1E2=yyyvsp;_T1E3=& _T1E2[1].v;_T1E4=(union Cyc_YYSTYPE*)_T1E3;_T1E5=
Cyc_yyget_YY28(_T1E4);_T1E6=yyyvsp;_T1E7=& _T1E6[2].v;_T1E8=(union Cyc_YYSTYPE*)_T1E7;_T1E9=Cyc_yyget_Stmt_tok(_T1E8);_T1EA=yyyvsp;_T1EB=_T1EA[0];_T1EC=_T1EB.l;_T1ED=_T1EC.first_line;_T1EE=Cyc_Position_loc_to_seg(_T1ED);_T1EF=Cyc_Parse_make_function(_T1DF,_T1E1,_T1E5,0,_T1E9,_T1EE);yyval=Cyc_YY15(_T1EF);goto _LL0;}case 44: _T1F0=yyyvsp;_T1F1=& _T1F0[0].v;_T1F2=(union Cyc_YYSTYPE*)_T1F1;{
# 1511 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1F2);_T1F3=yyr;_T1F4=& d;_T1F5=(struct Cyc_Parse_Declaration_spec*)_T1F4;_T1F6=yyyvsp;_T1F7=& _T1F6[1].v;_T1F8=(union Cyc_YYSTYPE*)_T1F7;_T1F9=
Cyc_yyget_YY28(_T1F8);_T1FA=yyyvsp;_T1FB=& _T1FA[2].v;_T1FC=(union Cyc_YYSTYPE*)_T1FB;_T1FD=Cyc_yyget_YY16(_T1FC);_T1FE=yyyvsp;_T1FF=& _T1FE[3].v;_T200=(union Cyc_YYSTYPE*)_T1FF;_T201=Cyc_yyget_Stmt_tok(_T200);_T202=yyyvsp;_T203=_T202[0];_T204=_T203.l;_T205=_T204.first_line;_T206=Cyc_Position_loc_to_seg(_T205);_T207=Cyc_Parse_make_function(_T1F3,_T1F5,_T1F9,_T1FD,_T201,_T206);yyval=Cyc_YY15(_T207);goto _LL0;}case 45: _T208=yyyvsp;_T209=& _T208[1].v;_T20A=(union Cyc_YYSTYPE*)_T209;_T20B=
# 1516 "parse.y"
Cyc_yyget_QualId_tok(_T20A);Cyc_Lex_enter_using(_T20B);_T20C=yyyvsp;_T20D=_T20C[1];yyval=_T20D.v;goto _LL0;case 46:
# 1519
 Cyc_Lex_leave_using();goto _LL0;case 47:{struct _fat_ptr*_T1870=_cycalloc(sizeof(struct _fat_ptr));_T20F=yyyvsp;_T210=& _T20F[1].v;_T211=(union Cyc_YYSTYPE*)_T210;
# 1522
*_T1870=Cyc_yyget_String_tok(_T211);_T20E=(struct _fat_ptr*)_T1870;}Cyc_Lex_enter_namespace(_T20E);_T212=yyyvsp;_T213=_T212[1];yyval=_T213.v;goto _LL0;case 48:
# 1525
 Cyc_Lex_leave_namespace();goto _LL0;case 49:
# 1528
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + 1;goto _LL0;case 50:
# 1531
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + -1;goto _LL0;case 51: _T214=yyyvsp;_T215=& _T214[0].v;_T216=(union Cyc_YYSTYPE*)_T215;_T217=
# 1536 "parse.y"
Cyc_yyget_YY17(_T216);_T218=yyyvsp;_T219=_T218[0];_T21A=_T219.l;_T21B=_T21A.first_line;_T21C=Cyc_Position_loc_to_seg(_T21B);_T21D=yyyvsp;_T21E=_T21D[0];_T21F=_T21E.l;_T220=_T21F.first_line;_T221=Cyc_Position_loc_to_seg(_T220);_T222=Cyc_Parse_make_declarations(_T217,0,_T21C,_T221);yyval=Cyc_YY16(_T222);goto _LL0;case 52: _T223=yyyvsp;_T224=& _T223[0].v;_T225=(union Cyc_YYSTYPE*)_T224;_T226=
# 1538 "parse.y"
Cyc_yyget_YY17(_T225);{struct _tuple11*(*_T1870)(struct _tuple11*)=(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;_T227=_T1870;}_T228=yyyvsp;_T229=& _T228[1].v;_T22A=(union Cyc_YYSTYPE*)_T229;_T22B=Cyc_yyget_YY19(_T22A);_T22C=_T227(_T22B);_T22D=yyyvsp;_T22E=_T22D[0];_T22F=_T22E.l;_T230=_T22F.first_line;_T231=Cyc_Position_loc_to_seg(_T230);_T232=yyyvsp;_T233=_T232[0];_T234=_T233.l;_T235=_T234.first_line;_T236=Cyc_Position_loc_to_seg(_T235);_T237=Cyc_Parse_make_declarations(_T226,_T22C,_T231,_T236);yyval=Cyc_YY16(_T237);goto _LL0;case 53:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T239=yyyvsp;_T23A=& _T239[1].v;_T23B=(union Cyc_YYSTYPE*)_T23A;_T23C=
# 1541
Cyc_yyget_YY9(_T23B);_T23D=yyyvsp;_T23E=& _T23D[3].v;_T23F=(union Cyc_YYSTYPE*)_T23E;_T240=Cyc_yyget_Exp_tok(_T23F);_T241=yyyvsp;_T242=_T241[0];_T243=_T242.l;_T244=_T243.first_line;_T245=Cyc_Position_loc_to_seg(_T244);_T1870->hd=Cyc_Absyn_let_decl(_T23C,_T240,_T245);_T1870->tl=0;_T238=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T238);goto _LL0;case 54:  {
# 1543 "parse.y"
struct Cyc_List_List*vds=0;_T246=yyyvsp;_T247=& _T246[1].v;_T248=(union Cyc_YYSTYPE*)_T247;{
struct Cyc_List_List*ids=Cyc_yyget_YY37(_T248);_TL294: if(ids!=0)goto _TL292;else{goto _TL293;}
_TL292:{struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0));_T249=qv;_T249->f0=Cyc_Absyn_Rel_n(0);_T24A=qv;_T24B=ids;_T24C=_T24B->hd;_T24A->f1=(struct _fat_ptr*)_T24C;{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T24E=qv;_T24F=
Cyc_Absyn_wildtyp(0);_T1870->hd=Cyc_Absyn_new_vardecl(0U,_T24E,_T24F,0,0);_T1870->tl=vds;_T24D=(struct Cyc_List_List*)_T1870;}vds=_T24D;}_T250=ids;
# 1544
ids=_T250->tl;goto _TL294;_TL293:;}{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T252=
# 1548
Cyc_List_imp_rev(vds);_T253=yyyvsp;_T254=_T253[0];_T255=_T254.l;_T256=_T255.first_line;_T257=Cyc_Position_loc_to_seg(_T256);_T1870->hd=Cyc_Absyn_letv_decl(_T252,_T257);_T1870->tl=0;_T251=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T251);goto _LL0;}case 55: _T258=yyyvsp;_T259=& _T258[2].v;_T25A=(union Cyc_YYSTYPE*)_T259;_T25B=
# 1553 "parse.y"
Cyc_yyget_String_tok(_T25A);_T25C=yyyvsp;_T25D=_T25C[2];_T25E=_T25D.l;_T25F=_T25E.first_line;_T260=Cyc_Position_loc_to_seg(_T25F);Cyc_Parse_tvar_ok(_T25B,_T260);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T261=tv;{struct _fat_ptr*_T1870=_cycalloc(sizeof(struct _fat_ptr));_T263=yyyvsp;_T264=& _T263[2].v;_T265=(union Cyc_YYSTYPE*)_T264;*_T1870=Cyc_yyget_String_tok(_T265);_T262=(struct _fat_ptr*)_T1870;}_T261->name=_T262;_T266=tv;_T266->identity=-1;_T267=tv;_T268=& Cyc_Kinds_ek;_T269=(struct Cyc_Absyn_Kind*)_T268;_T267->kind=Cyc_Kinds_kind_to_bound(_T269);_T26A=tv;_T26A->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T26B=yyyvsp;_T26C=_T26B[4];_T26D=_T26C.l;_T26E=_T26D.first_line;_T26F=
Cyc_Position_loc_to_seg(_T26E);{struct _tuple0*_T1870=_cycalloc(sizeof(struct _tuple0));_T1870->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T272=yyyvsp;_T273=& _T272[4].v;_T274=(union Cyc_YYSTYPE*)_T273;*_T1871=Cyc_yyget_String_tok(_T274);_T271=(struct _fat_ptr*)_T1871;}_T1870->f1=_T271;_T270=(struct _tuple0*)_T1870;}_T275=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T26F,_T270,_T275,0,0);{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T277=tv;_T278=vd;_T279=yyyvsp;_T27A=_T279[0];_T27B=_T27A.l;_T27C=_T27B.first_line;_T27D=
Cyc_Position_loc_to_seg(_T27C);_T1870->hd=Cyc_Absyn_region_decl(_T277,_T278,0,_T27D);_T1870->tl=0;_T276=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T276);goto _LL0;}}}case 56:{struct Cyc_String_pa_PrintArg_struct _T1870;_T1870.tag=0;_T280=yyyvsp;_T281=& _T280[1].v;_T282=(union Cyc_YYSTYPE*)_T281;
# 1561
_T1870.f1=Cyc_yyget_String_tok(_T282);_T27F=_T1870;}{struct Cyc_String_pa_PrintArg_struct _T1870=_T27F;void*_T1871[1];_T1871[0]=& _T1870;_T283=_tag_fat("`%s",sizeof(char),4U);_T284=_tag_fat(_T1871,sizeof(void*),1);_T27E=Cyc_aprintf(_T283,_T284);}{struct _fat_ptr tvstring=_T27E;_T285=yyyvsp;_T286=& _T285[1].v;_T287=(union Cyc_YYSTYPE*)_T286;_T288=
Cyc_yyget_String_tok(_T287);_T289=yyyvsp;_T28A=_T289[1];_T28B=_T28A.l;_T28C=_T28B.first_line;_T28D=Cyc_Position_loc_to_seg(_T28C);Cyc_Parse_tvar_ok(_T288,_T28D);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T28E=tv;{struct _fat_ptr*_T1870=_cycalloc(sizeof(struct _fat_ptr));*_T1870=tvstring;_T28F=(struct _fat_ptr*)_T1870;}_T28E->name=_T28F;_T290=tv;_T290->identity=-1;_T291=tv;_T292=& Cyc_Kinds_ek;_T293=(struct Cyc_Absyn_Kind*)_T292;_T291->kind=Cyc_Kinds_kind_to_bound(_T293);_T294=tv;_T294->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T295=yyyvsp;_T296=_T295[1];_T297=_T296.l;_T298=_T297.first_line;_T299=
Cyc_Position_loc_to_seg(_T298);{struct _tuple0*_T1870=_cycalloc(sizeof(struct _tuple0));_T1870->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T29C=yyyvsp;_T29D=& _T29C[1].v;_T29E=(union Cyc_YYSTYPE*)_T29D;*_T1871=Cyc_yyget_String_tok(_T29E);_T29B=(struct _fat_ptr*)_T1871;}_T1870->f1=_T29B;_T29A=(struct _tuple0*)_T1870;}_T29F=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T299,_T29A,_T29F,0,0);{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T2A1=tv;_T2A2=vd;_T2A3=yyyvsp;_T2A4=& _T2A3[2].v;_T2A5=(union Cyc_YYSTYPE*)_T2A4;_T2A6=
Cyc_yyget_YY61(_T2A5);_T2A7=yyyvsp;_T2A8=_T2A7[0];_T2A9=_T2A8.l;_T2AA=_T2A9.first_line;_T2AB=Cyc_Position_loc_to_seg(_T2AA);_T1870->hd=Cyc_Absyn_region_decl(_T2A1,_T2A2,_T2A6,_T2AB);_T1870->tl=0;_T2A0=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY16(_T2A0);goto _LL0;}}}}case 57:
# 1571 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 58: _T2AC=yyyvsp;_T2AD=& _T2AC[1].v;_T2AE=(union Cyc_YYSTYPE*)_T2AD;_T2AF=
# 1573 "parse.y"
Cyc_yyget_String_tok(_T2AE);_T2B0=_tag_fat("open",sizeof(char),5U);_T2B1=Cyc_strcmp(_T2AF,_T2B0);if(_T2B1==0)goto _TL295;_T2B2=yyyvsp;_T2B3=_T2B2[3];_T2B4=_T2B3.l;_T2B5=_T2B4.first_line;_T2B6=Cyc_Position_loc_to_seg(_T2B5);_T2B7=_tag_fat("expecting `open'",sizeof(char),17U);_T2B8=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2B6,_T2B7,_T2B8);goto _TL296;_TL295: _TL296: _T2B9=yyyvsp;_T2BA=& _T2B9[3].v;_T2BB=(union Cyc_YYSTYPE*)_T2BA;_T2BC=
Cyc_yyget_Exp_tok(_T2BB);yyval=Cyc_YY61(_T2BC);goto _LL0;case 59: _T2BD=yyyvsp;_T2BE=_T2BD[0];
# 1577
yyval=_T2BE.v;goto _LL0;case 60: _T2BF=yyyvsp;_T2C0=& _T2BF[0].v;_T2C1=(union Cyc_YYSTYPE*)_T2C0;_T2C2=
# 1578 "parse.y"
Cyc_yyget_YY16(_T2C1);_T2C3=yyyvsp;_T2C4=& _T2C3[1].v;_T2C5=(union Cyc_YYSTYPE*)_T2C4;_T2C6=Cyc_yyget_YY16(_T2C5);_T2C7=Cyc_List_imp_append(_T2C2,_T2C6);yyval=Cyc_YY16(_T2C7);goto _LL0;case 61:{struct Cyc_Parse_Declaration_spec _T1870;_T2C9=yyyvsp;_T2CA=& _T2C9[0].v;_T2CB=(union Cyc_YYSTYPE*)_T2CA;
# 1584 "parse.y"
_T1870.sc=Cyc_yyget_YY20(_T2CB);_T2CC=yyyvsp;_T2CD=_T2CC[0];_T2CE=_T2CD.l;_T2CF=_T2CE.first_line;_T2D0=Cyc_Position_loc_to_seg(_T2CF);_T1870.tq=Cyc_Absyn_empty_tqual(_T2D0);
_T1870.type_specs=Cyc_Parse_empty_spec(0U);_T1870.is_inline=0;_T1870.attributes=0;_T2C8=_T1870;}
# 1584
yyval=Cyc_YY17(_T2C8);goto _LL0;case 62: _T2D1=yyyvsp;_T2D2=& _T2D1[1].v;_T2D3=(union Cyc_YYSTYPE*)_T2D2;{
# 1587 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T2D3);_T2D4=two;_T2D5=_T2D4.sc;_T2D6=(int)_T2D5;
if(_T2D6==7)goto _TL297;_T2D7=yyyvsp;_T2D8=_T2D7[0];_T2D9=_T2D8.l;_T2DA=_T2D9.first_line;_T2DB=
Cyc_Position_loc_to_seg(_T2DA);_T2DC=
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U);_T2DD=_tag_fat(0U,sizeof(void*),0);
# 1589
Cyc_Warn_warn(_T2DB,_T2DC,_T2DD);goto _TL298;_TL297: _TL298:{struct Cyc_Parse_Declaration_spec _T1870;_T2DF=yyyvsp;_T2E0=& _T2DF[0].v;_T2E1=(union Cyc_YYSTYPE*)_T2E0;
# 1591
_T1870.sc=Cyc_yyget_YY20(_T2E1);_T2E2=two;_T1870.tq=_T2E2.tq;_T2E3=two;_T1870.type_specs=_T2E3.type_specs;_T2E4=two;
_T1870.is_inline=_T2E4.is_inline;_T2E5=two;_T1870.attributes=_T2E5.attributes;_T2DE=_T1870;}
# 1591
yyval=Cyc_YY17(_T2DE);goto _LL0;}case 63: _T2E6=yyyvsp;_T2E7=_T2E6[0];_T2E8=_T2E7.l;_T2E9=_T2E8.first_line;_T2EA=
# 1595 "parse.y"
Cyc_Position_loc_to_seg(_T2E9);_T2EB=_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U);_T2EC=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T2EA,_T2EB,_T2EC);_T2ED=yyyvsp;_T2EE=_T2ED[1];
yyval=_T2EE.v;goto _LL0;case 64:{struct Cyc_Parse_Declaration_spec _T1870;
# 1599 "parse.y"
_T1870.sc=7U;_T2F0=yyyvsp;_T2F1=_T2F0[0];_T2F2=_T2F1.l;_T2F3=_T2F2.first_line;_T2F4=Cyc_Position_loc_to_seg(_T2F3);_T1870.tq=Cyc_Absyn_empty_tqual(_T2F4);_T2F5=yyyvsp;_T2F6=& _T2F5[0].v;_T2F7=(union Cyc_YYSTYPE*)_T2F6;
_T1870.type_specs=Cyc_yyget_YY21(_T2F7);_T1870.is_inline=0;_T1870.attributes=0;_T2EF=_T1870;}
# 1599
yyval=Cyc_YY17(_T2EF);goto _LL0;case 65: _T2F8=yyyvsp;_T2F9=& _T2F8[1].v;_T2FA=(union Cyc_YYSTYPE*)_T2F9;{
# 1602 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T2FA);{struct Cyc_Parse_Declaration_spec _T1870;_T2FC=two;
_T1870.sc=_T2FC.sc;_T2FD=two;_T1870.tq=_T2FD.tq;_T2FE=yyyvsp;_T2FF=_T2FE[0];_T300=_T2FF.l;_T301=_T300.first_line;_T302=
Cyc_Position_loc_to_seg(_T301);_T303=two;_T304=_T303.type_specs;_T305=yyyvsp;_T306=& _T305[0].v;_T307=(union Cyc_YYSTYPE*)_T306;_T308=Cyc_yyget_YY21(_T307);_T1870.type_specs=Cyc_Parse_combine_specifiers(_T302,_T304,_T308);_T309=two;
_T1870.is_inline=_T309.is_inline;_T30A=two;_T1870.attributes=_T30A.attributes;_T2FB=_T1870;}
# 1603
yyval=Cyc_YY17(_T2FB);goto _LL0;}case 66:{struct Cyc_Parse_Declaration_spec _T1870;
# 1608 "parse.y"
_T1870.sc=7U;_T30C=yyyvsp;_T30D=& _T30C[0].v;_T30E=(union Cyc_YYSTYPE*)_T30D;_T1870.tq=Cyc_yyget_YY24(_T30E);_T1870.type_specs=Cyc_Parse_empty_spec(0U);_T1870.is_inline=0;_T1870.attributes=0;_T30B=_T1870;}yyval=Cyc_YY17(_T30B);goto _LL0;case 67: _T30F=yyyvsp;_T310=& _T30F[1].v;_T311=(union Cyc_YYSTYPE*)_T310;{
# 1610 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T311);{struct Cyc_Parse_Declaration_spec _T1870;_T313=two;
_T1870.sc=_T313.sc;_T314=yyyvsp;_T315=& _T314[0].v;_T316=(union Cyc_YYSTYPE*)_T315;_T317=Cyc_yyget_YY24(_T316);_T318=two;_T319=_T318.tq;_T1870.tq=Cyc_Absyn_combine_tqual(_T317,_T319);_T31A=two;
_T1870.type_specs=_T31A.type_specs;_T31B=two;_T1870.is_inline=_T31B.is_inline;_T31C=two;_T1870.attributes=_T31C.attributes;_T312=_T1870;}
# 1611
yyval=Cyc_YY17(_T312);goto _LL0;}case 68:{struct Cyc_Parse_Declaration_spec _T1870;
# 1615 "parse.y"
_T1870.sc=7U;_T31E=yyyvsp;_T31F=_T31E[0];_T320=_T31F.l;_T321=_T320.first_line;_T322=Cyc_Position_loc_to_seg(_T321);_T1870.tq=Cyc_Absyn_empty_tqual(_T322);
_T1870.type_specs=Cyc_Parse_empty_spec(0U);_T1870.is_inline=1;_T1870.attributes=0;_T31D=_T1870;}
# 1615
yyval=Cyc_YY17(_T31D);goto _LL0;case 69: _T323=yyyvsp;_T324=& _T323[1].v;_T325=(union Cyc_YYSTYPE*)_T324;{
# 1618 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T325);{struct Cyc_Parse_Declaration_spec _T1870;_T327=two;
_T1870.sc=_T327.sc;_T328=two;_T1870.tq=_T328.tq;_T329=two;_T1870.type_specs=_T329.type_specs;_T1870.is_inline=1;_T32A=two;_T1870.attributes=_T32A.attributes;_T326=_T1870;}yyval=Cyc_YY17(_T326);goto _LL0;}case 70:{struct Cyc_Parse_Declaration_spec _T1870;
# 1622 "parse.y"
_T1870.sc=7U;_T32C=yyyvsp;_T32D=_T32C[0];_T32E=_T32D.l;_T32F=_T32E.first_line;_T330=Cyc_Position_loc_to_seg(_T32F);_T1870.tq=Cyc_Absyn_empty_tqual(_T330);
_T1870.type_specs=Cyc_Parse_empty_spec(0U);_T1870.is_inline=0;_T331=yyyvsp;_T332=& _T331[0].v;_T333=(union Cyc_YYSTYPE*)_T332;_T1870.attributes=Cyc_yyget_YY46(_T333);_T32B=_T1870;}
# 1622
yyval=Cyc_YY17(_T32B);goto _LL0;case 71: _T334=yyyvsp;_T335=& _T334[1].v;_T336=(union Cyc_YYSTYPE*)_T335;{
# 1625 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T336);{struct Cyc_Parse_Declaration_spec _T1870;_T338=two;
_T1870.sc=_T338.sc;_T339=two;_T1870.tq=_T339.tq;_T33A=two;
_T1870.type_specs=_T33A.type_specs;_T33B=two;_T1870.is_inline=_T33B.is_inline;_T33C=yyyvsp;_T33D=& _T33C[0].v;_T33E=(union Cyc_YYSTYPE*)_T33D;_T33F=
Cyc_yyget_YY46(_T33E);_T340=two;_T341=_T340.attributes;_T1870.attributes=Cyc_List_imp_append(_T33F,_T341);_T337=_T1870;}
# 1626
yyval=Cyc_YY17(_T337);goto _LL0;}case 72:
# 1632 "parse.y"
 yyval=Cyc_YY20(4U);goto _LL0;case 73:
# 1633 "parse.y"
 yyval=Cyc_YY20(5U);goto _LL0;case 74:
# 1634 "parse.y"
 yyval=Cyc_YY20(3U);goto _LL0;case 75:
# 1635 "parse.y"
 yyval=Cyc_YY20(1U);goto _LL0;case 76: _T342=yyyvsp;_T343=& _T342[1].v;_T344=(union Cyc_YYSTYPE*)_T343;_T345=
# 1637 "parse.y"
Cyc_yyget_String_tok(_T344);_T346=_tag_fat("C",sizeof(char),2U);_T347=Cyc_strcmp(_T345,_T346);if(_T347==0)goto _TL299;_T348=yyyvsp;_T349=_T348[0];_T34A=_T349.l;_T34B=_T34A.first_line;_T34C=
Cyc_Position_loc_to_seg(_T34B);_T34D=_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U);_T34E=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T34C,_T34D,_T34E);goto _TL29A;_TL299: _TL29A:
 yyval=Cyc_YY20(2U);goto _LL0;case 77:
# 1641 "parse.y"
 yyval=Cyc_YY20(0U);goto _LL0;case 78:
# 1643 "parse.y"
 yyval=Cyc_YY20(6U);goto _LL0;case 79:
# 1648 "parse.y"
 yyval=Cyc_YY46(0);goto _LL0;case 80: _T34F=yyyvsp;_T350=_T34F[0];
# 1649 "parse.y"
yyval=_T350.v;goto _LL0;case 81: _T351=yyyvsp;_T352=_T351[3];
# 1653 "parse.y"
yyval=_T352.v;goto _LL0;case 82:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T354=yyyvsp;_T355=& _T354[0].v;_T356=(union Cyc_YYSTYPE*)_T355;
# 1656
_T1870->hd=Cyc_yyget_YY47(_T356);_T1870->tl=0;_T353=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY46(_T353);goto _LL0;case 83:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T358=yyyvsp;_T359=& _T358[0].v;_T35A=(union Cyc_YYSTYPE*)_T359;
# 1657 "parse.y"
_T1870->hd=Cyc_yyget_YY47(_T35A);_T35B=yyyvsp;_T35C=& _T35B[2].v;_T35D=(union Cyc_YYSTYPE*)_T35C;_T1870->tl=Cyc_yyget_YY46(_T35D);_T357=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY46(_T357);goto _LL0;case 84: _T35E=yyyvsp;_T35F=_T35E[0];_T360=_T35F.l;_T361=_T360.first_line;_T362=
# 1660
Cyc_Position_loc_to_seg(_T361);_T363=yyyvsp;_T364=& _T363[0].v;_T365=(union Cyc_YYSTYPE*)_T364;_T366=Cyc_yyget_String_tok(_T365);_T367=Cyc_Atts_parse_nullary_att(_T362,_T366);yyval=Cyc_YY47(_T367);goto _LL0;case 85: _T368=& Cyc_Atts_Const_att_val;_T369=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_T368;_T36A=(void*)_T369;
# 1661 "parse.y"
yyval=Cyc_YY47(_T36A);goto _LL0;case 86: _T36B=yyyvsp;_T36C=_T36B[0];_T36D=_T36C.l;_T36E=_T36D.first_line;_T36F=
# 1663 "parse.y"
Cyc_Position_loc_to_seg(_T36E);_T370=yyyvsp;_T371=& _T370[0].v;_T372=(union Cyc_YYSTYPE*)_T371;_T373=Cyc_yyget_String_tok(_T372);_T374=yyyvsp;_T375=_T374[2];_T376=_T375.l;_T377=_T376.first_line;_T378=Cyc_Position_loc_to_seg(_T377);_T379=yyyvsp;_T37A=& _T379[2].v;_T37B=(union Cyc_YYSTYPE*)_T37A;_T37C=Cyc_yyget_Exp_tok(_T37B);_T37D=Cyc_Atts_parse_unary_att(_T36F,_T373,_T378,_T37C);yyval=Cyc_YY47(_T37D);goto _LL0;case 87: _T37E=yyyvsp;_T37F=_T37E[0];_T380=_T37F.l;_T381=_T380.first_line;_T382=
# 1665 "parse.y"
Cyc_Position_loc_to_seg(_T381);_T383=yyyvsp;_T384=_T383[2];_T385=_T384.l;_T386=_T385.first_line;_T387=Cyc_Position_loc_to_seg(_T386);_T388=yyyvsp;_T389=& _T388[0].v;_T38A=(union Cyc_YYSTYPE*)_T389;_T38B=Cyc_yyget_String_tok(_T38A);_T38C=yyyvsp;_T38D=& _T38C[2].v;_T38E=(union Cyc_YYSTYPE*)_T38D;_T38F=Cyc_yyget_String_tok(_T38E);_T390=yyyvsp;_T391=_T390[4];_T392=_T391.l;_T393=_T392.first_line;_T394=
Cyc_Position_loc_to_seg(_T393);_T395=yyyvsp;_T396=& _T395[4].v;_T397=(union Cyc_YYSTYPE*)_T396;_T398=Cyc_yyget_Int_tok(_T397);_T399=Cyc_Parse_cnst2uint(_T394,_T398);_T39A=yyyvsp;_T39B=_T39A[6];_T39C=_T39B.l;_T39D=_T39C.first_line;_T39E=
Cyc_Position_loc_to_seg(_T39D);_T39F=yyyvsp;_T3A0=& _T39F[6].v;_T3A1=(union Cyc_YYSTYPE*)_T3A0;_T3A2=Cyc_yyget_Int_tok(_T3A1);_T3A3=Cyc_Parse_cnst2uint(_T39E,_T3A2);_T3A4=
# 1665
Cyc_Atts_parse_format_att(_T382,_T387,_T38B,_T38F,_T399,_T3A3);yyval=Cyc_YY47(_T3A4);goto _LL0;case 88: _T3A5=yyyvsp;_T3A6=_T3A5[0];
# 1677 "parse.y"
yyval=_T3A6.v;goto _LL0;case 89: _T3A7=yyyvsp;_T3A8=& _T3A7[0].v;_T3A9=(union Cyc_YYSTYPE*)_T3A8;_T3AA=
# 1679 "parse.y"
Cyc_yyget_QualId_tok(_T3A9);_T3AB=yyyvsp;_T3AC=& _T3AB[1].v;_T3AD=(union Cyc_YYSTYPE*)_T3AC;_T3AE=Cyc_yyget_YY41(_T3AD);_T3AF=Cyc_Absyn_typedef_type(_T3AA,_T3AE,0,0);_T3B0=yyyvsp;_T3B1=_T3B0[0];_T3B2=_T3B1.l;_T3B3=_T3B2.first_line;_T3B4=Cyc_Position_loc_to_seg(_T3B3);_T3B5=Cyc_Parse_type_spec(_T3AF,_T3B4);yyval=Cyc_YY21(_T3B5);goto _LL0;case 90: _T3B6=Cyc_Absyn_void_type;_T3B7=yyyvsp;_T3B8=_T3B7[0];_T3B9=_T3B8.l;_T3BA=_T3B9.first_line;_T3BB=
# 1683 "parse.y"
Cyc_Position_loc_to_seg(_T3BA);_T3BC=Cyc_Parse_type_spec(_T3B6,_T3BB);yyval=Cyc_YY21(_T3BC);goto _LL0;case 91: _T3BD=Cyc_Absyn_char_type;_T3BE=yyyvsp;_T3BF=_T3BE[0];_T3C0=_T3BF.l;_T3C1=_T3C0.first_line;_T3C2=
# 1684 "parse.y"
Cyc_Position_loc_to_seg(_T3C1);_T3C3=Cyc_Parse_type_spec(_T3BD,_T3C2);yyval=Cyc_YY21(_T3C3);goto _LL0;case 92: _T3C4=yyyvsp;_T3C5=_T3C4[0];_T3C6=_T3C5.l;_T3C7=_T3C6.first_line;_T3C8=
# 1685 "parse.y"
Cyc_Position_loc_to_seg(_T3C7);_T3C9=Cyc_Parse_short_spec(_T3C8);yyval=Cyc_YY21(_T3C9);goto _LL0;case 93: _T3CA=Cyc_Absyn_sint_type;_T3CB=yyyvsp;_T3CC=_T3CB[0];_T3CD=_T3CC.l;_T3CE=_T3CD.first_line;_T3CF=
# 1686 "parse.y"
Cyc_Position_loc_to_seg(_T3CE);_T3D0=Cyc_Parse_type_spec(_T3CA,_T3CF);yyval=Cyc_YY21(_T3D0);goto _LL0;case 94: _T3D1=yyyvsp;_T3D2=_T3D1[0];_T3D3=_T3D2.l;_T3D4=_T3D3.first_line;_T3D5=
# 1687 "parse.y"
Cyc_Position_loc_to_seg(_T3D4);_T3D6=Cyc_Parse_long_spec(_T3D5);yyval=Cyc_YY21(_T3D6);goto _LL0;case 95: _T3D7=Cyc_Absyn_float_type;_T3D8=yyyvsp;_T3D9=_T3D8[0];_T3DA=_T3D9.l;_T3DB=_T3DA.first_line;_T3DC=
# 1688 "parse.y"
Cyc_Position_loc_to_seg(_T3DB);_T3DD=Cyc_Parse_type_spec(_T3D7,_T3DC);yyval=Cyc_YY21(_T3DD);goto _LL0;case 96: _T3DE=Cyc_Absyn_double_type;_T3DF=yyyvsp;_T3E0=_T3DF[0];_T3E1=_T3E0.l;_T3E2=_T3E1.first_line;_T3E3=
# 1689 "parse.y"
Cyc_Position_loc_to_seg(_T3E2);_T3E4=Cyc_Parse_type_spec(_T3DE,_T3E3);yyval=Cyc_YY21(_T3E4);goto _LL0;case 97: _T3E5=yyyvsp;_T3E6=_T3E5[0];_T3E7=_T3E6.l;_T3E8=_T3E7.first_line;_T3E9=
# 1690 "parse.y"
Cyc_Position_loc_to_seg(_T3E8);_T3EA=Cyc_Parse_signed_spec(_T3E9);yyval=Cyc_YY21(_T3EA);goto _LL0;case 98: _T3EB=yyyvsp;_T3EC=_T3EB[0];_T3ED=_T3EC.l;_T3EE=_T3ED.first_line;_T3EF=
# 1691 "parse.y"
Cyc_Position_loc_to_seg(_T3EE);_T3F0=Cyc_Parse_unsigned_spec(_T3EF);yyval=Cyc_YY21(_T3F0);goto _LL0;case 99: _T3F1=yyyvsp;_T3F2=_T3F1[0];_T3F3=_T3F2.l;_T3F4=_T3F3.first_line;_T3F5=
# 1692 "parse.y"
Cyc_Position_loc_to_seg(_T3F4);_T3F6=Cyc_Parse_complex_spec(_T3F5);yyval=Cyc_YY21(_T3F6);goto _LL0;case 100: _T3F7=yyyvsp;_T3F8=_T3F7[0];
# 1693 "parse.y"
yyval=_T3F8.v;goto _LL0;case 101: _T3F9=yyyvsp;_T3FA=_T3F9[0];
# 1694 "parse.y"
yyval=_T3FA.v;goto _LL0;case 102: _T3FB=yyyvsp;_T3FC=& _T3FB[2].v;_T3FD=(union Cyc_YYSTYPE*)_T3FC;_T3FE=
# 1696 "parse.y"
Cyc_yyget_Exp_tok(_T3FD);_T3FF=Cyc_Absyn_typeof_type(_T3FE);_T400=yyyvsp;_T401=_T400[0];_T402=_T401.l;_T403=_T402.first_line;_T404=Cyc_Position_loc_to_seg(_T403);_T405=Cyc_Parse_type_spec(_T3FF,_T404);yyval=Cyc_YY21(_T405);goto _LL0;case 103: _T406=
# 1698 "parse.y"
_tag_fat("__builtin_va_list",sizeof(char),18U);_T407=& Cyc_Kinds_bk;_T408=(struct Cyc_Absyn_Kind*)_T407;_T409=Cyc_Absyn_builtin_type(_T406,_T408);_T40A=yyyvsp;_T40B=_T40A[0];_T40C=_T40B.l;_T40D=_T40C.first_line;_T40E=Cyc_Position_loc_to_seg(_T40D);_T40F=Cyc_Parse_type_spec(_T409,_T40E);yyval=Cyc_YY21(_T40F);goto _LL0;case 104: _T410=yyyvsp;_T411=_T410[0];
# 1700 "parse.y"
yyval=_T411.v;goto _LL0;case 105: _T412=yyyvsp;_T413=& _T412[0].v;_T414=(union Cyc_YYSTYPE*)_T413;_T415=
# 1702 "parse.y"
Cyc_yyget_YY45(_T414);_T416=yyyvsp;_T417=_T416[0];_T418=_T417.l;_T419=_T418.first_line;_T41A=Cyc_Position_loc_to_seg(_T419);_T41B=Cyc_Parse_type_spec(_T415,_T41A);yyval=Cyc_YY21(_T41B);goto _LL0;case 106: _T41C=
# 1704 "parse.y"
Cyc_Absyn_new_evar(0,0);_T41D=yyyvsp;_T41E=_T41D[0];_T41F=_T41E.l;_T420=_T41F.first_line;_T421=Cyc_Position_loc_to_seg(_T420);_T422=Cyc_Parse_type_spec(_T41C,_T421);yyval=Cyc_YY21(_T422);goto _LL0;case 107: _T423=yyyvsp;_T424=& _T423[2].v;_T425=(union Cyc_YYSTYPE*)_T424;_T426=
# 1706 "parse.y"
Cyc_yyget_YY44(_T425);_T427=Cyc_Kinds_kind_to_opt(_T426);_T428=Cyc_Absyn_new_evar(_T427,0);_T429=yyyvsp;_T42A=_T429[0];_T42B=_T42A.l;_T42C=_T42B.first_line;_T42D=Cyc_Position_loc_to_seg(_T42C);_T42E=Cyc_Parse_type_spec(_T428,_T42D);yyval=Cyc_YY21(_T42E);goto _LL0;case 108: _T430=Cyc_List_map_c;{
# 1708 "parse.y"
struct Cyc_List_List*(*_T1870)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T430;_T42F=_T1870;}{struct _tuple19*(*_T1870)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T431=_T1870;}_T432=yyyvsp;_T433=_T432[2];_T434=_T433.l;_T435=_T434.first_line;_T436=Cyc_Position_loc_to_seg(_T435);_T437=yyyvsp;_T438=& _T437[2].v;_T439=(union Cyc_YYSTYPE*)_T438;_T43A=Cyc_yyget_YY39(_T439);_T43B=Cyc_List_imp_rev(_T43A);_T43C=_T42F(_T431,_T436,_T43B);_T43D=Cyc_Absyn_tuple_type(_T43C);_T43E=yyyvsp;_T43F=_T43E[0];_T440=_T43F.l;_T441=_T440.first_line;_T442=
Cyc_Position_loc_to_seg(_T441);_T443=
# 1708
Cyc_Parse_type_spec(_T43D,_T442);yyval=Cyc_YY21(_T443);goto _LL0;case 109: _T444=yyyvsp;_T445=& _T444[2].v;_T446=(union Cyc_YYSTYPE*)_T445;_T447=
# 1711 "parse.y"
Cyc_yyget_YY45(_T446);_T448=Cyc_Absyn_rgn_handle_type(_T447);_T449=yyyvsp;_T44A=_T449[0];_T44B=_T44A.l;_T44C=_T44B.first_line;_T44D=Cyc_Position_loc_to_seg(_T44C);_T44E=Cyc_Parse_type_spec(_T448,_T44D);yyval=Cyc_YY21(_T44E);goto _LL0;case 110: _T44F=& Cyc_Kinds_eko;_T450=(struct Cyc_Core_Opt*)_T44F;_T451=
# 1713 "parse.y"
Cyc_Absyn_new_evar(_T450,0);_T452=Cyc_Absyn_rgn_handle_type(_T451);_T453=yyyvsp;_T454=_T453[0];_T455=_T454.l;_T456=_T455.first_line;_T457=Cyc_Position_loc_to_seg(_T456);_T458=Cyc_Parse_type_spec(_T452,_T457);yyval=Cyc_YY21(_T458);goto _LL0;case 111: _T459=yyyvsp;_T45A=& _T459[2].v;_T45B=(union Cyc_YYSTYPE*)_T45A;_T45C=
# 1715 "parse.y"
Cyc_yyget_YY58(_T45B);_T45D=Cyc_Absyn_aqual_handle_type(_T45C);_T45E=yyyvsp;_T45F=_T45E[0];_T460=_T45F.l;_T461=_T460.first_line;_T462=Cyc_Position_loc_to_seg(_T461);_T463=Cyc_Parse_type_spec(_T45D,_T462);yyval=Cyc_YY21(_T463);goto _LL0;case 112: _T464=yyyvsp;_T465=& _T464[2].v;_T466=(union Cyc_YYSTYPE*)_T465;_T467=
# 1717 "parse.y"
Cyc_yyget_YY45(_T466);_T468=Cyc_Absyn_tag_type(_T467);_T469=yyyvsp;_T46A=_T469[0];_T46B=_T46A.l;_T46C=_T46B.first_line;_T46D=Cyc_Position_loc_to_seg(_T46C);_T46E=Cyc_Parse_type_spec(_T468,_T46D);yyval=Cyc_YY21(_T46E);goto _LL0;case 113: _T46F=& Cyc_Kinds_iko;_T470=(struct Cyc_Core_Opt*)_T46F;_T471=
# 1719 "parse.y"
Cyc_Absyn_new_evar(_T470,0);_T472=Cyc_Absyn_tag_type(_T471);_T473=yyyvsp;_T474=_T473[0];_T475=_T474.l;_T476=_T475.first_line;_T477=Cyc_Position_loc_to_seg(_T476);_T478=Cyc_Parse_type_spec(_T472,_T477);yyval=Cyc_YY21(_T478);goto _LL0;case 114: _T479=yyyvsp;_T47A=& _T479[2].v;_T47B=(union Cyc_YYSTYPE*)_T47A;_T47C=
# 1721 "parse.y"
Cyc_yyget_Exp_tok(_T47B);_T47D=Cyc_Absyn_valueof_type(_T47C);_T47E=yyyvsp;_T47F=_T47E[0];_T480=_T47F.l;_T481=_T480.first_line;_T482=Cyc_Position_loc_to_seg(_T481);_T483=Cyc_Parse_type_spec(_T47D,_T482);yyval=Cyc_YY21(_T483);goto _LL0;case 115: _T484=yyyvsp;_T485=& _T484[0].v;_T486=(union Cyc_YYSTYPE*)_T485;_T487=
# 1726 "parse.y"
Cyc_yyget_String_tok(_T486);_T488=yyyvsp;_T489=_T488[0];_T48A=_T489.l;_T48B=_T48A.first_line;_T48C=Cyc_Position_loc_to_seg(_T48B);_T48D=Cyc_Kinds_id_to_kind(_T487,_T48C);yyval=Cyc_YY44(_T48D);goto _LL0;case 116: _T48F=Cyc_Flags_porting_c_code;
# 1730 "parse.y"
if(!_T48F)goto _TL29B;_T490=yyyvsp;_T491=_T490[0];_T492=_T491.l;_T493=_T492.first_line;_T48E=Cyc_Position_loc_to_seg(_T493);goto _TL29C;_TL29B: _T48E=0U;_TL29C: {unsigned loc=_T48E;{struct Cyc_Absyn_Tqual _T1870;
_T1870.print_const=1;_T1870.q_volatile=0;_T1870.q_restrict=0;_T1870.real_const=1;_T1870.loc=loc;_T494=_T1870;}yyval=Cyc_YY24(_T494);goto _LL0;}case 117:{struct Cyc_Absyn_Tqual _T1870;
# 1732 "parse.y"
_T1870.print_const=0;_T1870.q_volatile=1;_T1870.q_restrict=0;_T1870.real_const=0;_T1870.loc=0U;_T495=_T1870;}yyval=Cyc_YY24(_T495);goto _LL0;case 118:{struct Cyc_Absyn_Tqual _T1870;
# 1733 "parse.y"
_T1870.print_const=0;_T1870.q_volatile=0;_T1870.q_restrict=1;_T1870.real_const=0;_T1870.loc=0U;_T496=_T1870;}yyval=Cyc_YY24(_T496);goto _LL0;case 119:  {
# 1739 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl));_T497=ed;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_T1870->tag=1;{struct Cyc_Absyn_Enumdecl*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T1871->sc=2U;_T49A=yyyvsp;_T49B=& _T49A[1].v;_T49C=(union Cyc_YYSTYPE*)_T49B;_T1871->name=Cyc_yyget_QualId_tok(_T49C);{struct Cyc_Core_Opt*_T1872=_cycalloc(sizeof(struct Cyc_Core_Opt));_T49E=yyyvsp;_T49F=& _T49E[3].v;_T4A0=(union Cyc_YYSTYPE*)_T49F;_T1872->v=Cyc_yyget_YY49(_T4A0);_T49D=(struct Cyc_Core_Opt*)_T1872;}_T1871->fields=_T49D;_T499=(struct Cyc_Absyn_Enumdecl*)_T1871;}_T1870->f1=_T499;_T498=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T1870;}_T497->r=(void*)_T498;_T4A1=ed;_T4A2=yyyvsp;_T4A3=_T4A2[0];_T4A4=_T4A3.l;_T4A5=_T4A4.first_line;
_T4A1->loc=Cyc_Position_loc_to_seg(_T4A5);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T1870->tag=10;
_T1870->f1=ed;_T1870->f2=0;_T4A6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1870;}_T4A7=(void*)_T4A6;_T4A8=yyyvsp;_T4A9=_T4A8[0];_T4AA=_T4A9.l;_T4AB=_T4AA.first_line;_T4AC=Cyc_Position_loc_to_seg(_T4AB);_T4AD=Cyc_Parse_type_spec(_T4A7,_T4AC);yyval=Cyc_YY21(_T4AD);goto _LL0;}case 120: _T4AE=yyyvsp;_T4AF=& _T4AE[1].v;_T4B0=(union Cyc_YYSTYPE*)_T4AF;_T4B1=
# 1744 "parse.y"
Cyc_yyget_QualId_tok(_T4B0);_T4B2=Cyc_Absyn_enum_type(_T4B1,0);_T4B3=yyyvsp;_T4B4=_T4B3[0];_T4B5=_T4B4.l;_T4B6=_T4B5.first_line;_T4B7=Cyc_Position_loc_to_seg(_T4B6);_T4B8=Cyc_Parse_type_spec(_T4B2,_T4B7);yyval=Cyc_YY21(_T4B8);goto _LL0;case 121: _T4B9=yyyvsp;_T4BA=& _T4B9[2].v;_T4BB=(union Cyc_YYSTYPE*)_T4BA;_T4BC=
# 1746 "parse.y"
Cyc_yyget_YY49(_T4BB);_T4BD=Cyc_Absyn_anon_enum_type(_T4BC);_T4BE=yyyvsp;_T4BF=_T4BE[0];_T4C0=_T4BF.l;_T4C1=_T4C0.first_line;_T4C2=Cyc_Position_loc_to_seg(_T4C1);_T4C3=Cyc_Parse_type_spec(_T4BD,_T4C2);yyval=Cyc_YY21(_T4C3);goto _LL0;case 122:{struct Cyc_Absyn_Enumfield*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_T4C5=yyyvsp;_T4C6=& _T4C5[0].v;_T4C7=(union Cyc_YYSTYPE*)_T4C6;
# 1752 "parse.y"
_T1870->name=Cyc_yyget_QualId_tok(_T4C7);_T1870->tag=0;_T4C8=yyyvsp;_T4C9=_T4C8[0];_T4CA=_T4C9.l;_T4CB=_T4CA.first_line;_T1870->loc=Cyc_Position_loc_to_seg(_T4CB);_T4C4=(struct Cyc_Absyn_Enumfield*)_T1870;}yyval=Cyc_YY48(_T4C4);goto _LL0;case 123:{struct Cyc_Absyn_Enumfield*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_T4CD=yyyvsp;_T4CE=& _T4CD[0].v;_T4CF=(union Cyc_YYSTYPE*)_T4CE;
# 1754 "parse.y"
_T1870->name=Cyc_yyget_QualId_tok(_T4CF);_T4D0=yyyvsp;_T4D1=& _T4D0[2].v;_T4D2=(union Cyc_YYSTYPE*)_T4D1;_T1870->tag=Cyc_yyget_Exp_tok(_T4D2);_T4D3=yyyvsp;_T4D4=_T4D3[0];_T4D5=_T4D4.l;_T4D6=_T4D5.first_line;_T1870->loc=Cyc_Position_loc_to_seg(_T4D6);_T4CC=(struct Cyc_Absyn_Enumfield*)_T1870;}yyval=Cyc_YY48(_T4CC);goto _LL0;case 124:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T4D8=yyyvsp;_T4D9=& _T4D8[0].v;_T4DA=(union Cyc_YYSTYPE*)_T4D9;
# 1758 "parse.y"
_T1870->hd=Cyc_yyget_YY48(_T4DA);_T1870->tl=0;_T4D7=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY49(_T4D7);goto _LL0;case 125:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T4DC=yyyvsp;_T4DD=& _T4DC[0].v;_T4DE=(union Cyc_YYSTYPE*)_T4DD;
# 1759 "parse.y"
_T1870->hd=Cyc_yyget_YY48(_T4DE);_T1870->tl=0;_T4DB=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY49(_T4DB);goto _LL0;case 126:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T4E0=yyyvsp;_T4E1=& _T4E0[0].v;_T4E2=(union Cyc_YYSTYPE*)_T4E1;
# 1760 "parse.y"
_T1870->hd=Cyc_yyget_YY48(_T4E2);_T4E3=yyyvsp;_T4E4=& _T4E3[2].v;_T4E5=(union Cyc_YYSTYPE*)_T4E4;_T1870->tl=Cyc_yyget_YY49(_T4E5);_T4DF=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY49(_T4DF);goto _LL0;case 127:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_T1870->tag=7;_T4E7=yyyvsp;_T4E8=& _T4E7[0].v;_T4E9=(union Cyc_YYSTYPE*)_T4E8;
# 1766 "parse.y"
_T1870->f1=Cyc_yyget_YY22(_T4E9);_T1870->f2=0;_T4EA=yyyvsp;_T4EB=& _T4EA[2].v;_T4EC=(union Cyc_YYSTYPE*)_T4EB;_T1870->f3=Cyc_yyget_YY25(_T4EC);_T4E6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T1870;}_T4ED=(void*)_T4E6;_T4EE=yyyvsp;_T4EF=_T4EE[0];_T4F0=_T4EF.l;_T4F1=_T4F0.first_line;_T4F2=Cyc_Position_loc_to_seg(_T4F1);_T4F3=Cyc_Parse_type_spec(_T4ED,_T4F2);yyval=Cyc_YY21(_T4F3);goto _LL0;case 128: _T4F5=Cyc_List_map_c;{
# 1770 "parse.y"
struct Cyc_List_List*(*_T1870)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T4F5;_T4F4=_T1870;}_T4F6=yyyvsp;_T4F7=_T4F6[2];_T4F8=_T4F7.l;_T4F9=_T4F8.first_line;_T4FA=Cyc_Position_loc_to_seg(_T4F9);_T4FB=yyyvsp;_T4FC=& _T4FB[2].v;_T4FD=(union Cyc_YYSTYPE*)_T4FC;_T4FE=Cyc_yyget_YY41(_T4FD);{struct Cyc_List_List*ts=_T4F4(Cyc_Parse_typ2tvar,_T4FA,_T4FE);_T500=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1870)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T500;_T4FF=_T1870;}_T501=yyyvsp;_T502=_T501[4];_T503=_T502.l;_T504=_T503.first_line;_T505=Cyc_Position_loc_to_seg(_T504);_T506=yyyvsp;_T507=& _T506[4].v;_T508=(union Cyc_YYSTYPE*)_T507;_T509=Cyc_yyget_YY41(_T508);{struct Cyc_List_List*exist_ts=_T4FF(Cyc_Parse_typ2tvar,_T505,_T509);_T50A=yyyvsp;_T50B=& _T50A[5].v;_T50C=(union Cyc_YYSTYPE*)_T50B;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T50C);_T50E=ec_qb;_T50F=(unsigned)_T50E;
if(!_T50F)goto _TL29D;_T510=ec_qb;_T50D=*_T510;goto _TL29E;_TL29D:{struct _tuple28 _T1870;_T1870.f0=0;_T1870.f1=0;_T511=_T1870;}_T50D=_T511;_TL29E: {struct _tuple28 _T1870=_T50D;struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_List_List*ec=_T1872;struct Cyc_List_List*qb=_T1871;_T512=yyyvsp;_T513=& _T512[0].v;_T514=(union Cyc_YYSTYPE*)_T513;_T515=
Cyc_yyget_YY23(_T514);_T516=_T515.f1;_T517=yyyvsp;_T518=& _T517[1].v;_T519=(union Cyc_YYSTYPE*)_T518;_T51A=Cyc_yyget_QualId_tok(_T519);_T51B=ts;_T51C=exist_ts;_T51D=ec;_T51E=qb;_T51F=yyyvsp;_T520=& _T51F[6].v;_T521=(union Cyc_YYSTYPE*)_T520;_T522=
Cyc_yyget_YY25(_T521);_T523=yyyvsp;_T524=& _T523[0].v;_T525=(union Cyc_YYSTYPE*)_T524;_T526=Cyc_yyget_YY23(_T525);_T527=_T526.f0;_T528=Cyc_Absyn_aggrdecl_impl(_T51C,_T51D,_T51E,_T522,_T527);_T529=yyyvsp;_T52A=_T529[0];_T52B=_T52A.l;_T52C=_T52B.first_line;_T52D=
Cyc_Position_loc_to_seg(_T52C);{
# 1774
struct Cyc_Absyn_TypeDecl*td=Cyc_Absyn_aggr_tdecl(_T516,2U,_T51A,_T51B,_T528,0,_T52D);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T1873->tag=10;
# 1777
_T1873->f1=td;_T1873->f2=0;_T52E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1873;}_T52F=(void*)_T52E;_T530=yyyvsp;_T531=_T530[0];_T532=_T531.l;_T533=_T532.first_line;_T534=Cyc_Position_loc_to_seg(_T533);_T535=Cyc_Parse_type_spec(_T52F,_T534);yyval=Cyc_YY21(_T535);goto _LL0;}}}}}}case 129: _T536=yyyvsp;_T537=& _T536[0].v;_T538=(union Cyc_YYSTYPE*)_T537;_T539=
# 1780 "parse.y"
Cyc_yyget_YY23(_T538);_T53A=_T539.f1;_T53B=yyyvsp;_T53C=& _T53B[1].v;_T53D=(union Cyc_YYSTYPE*)_T53C;_T53E=Cyc_yyget_QualId_tok(_T53D);_T540=yyyvsp;_T541=& _T540[0].v;_T542=(union Cyc_YYSTYPE*)_T541;_T543=Cyc_yyget_YY23(_T542);_T544=_T543.f0;if(!_T544)goto _TL29F;{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));_T1870->v=(void*)1;_T545=(struct Cyc_Core_Opt*)_T1870;}_T53F=_T545;goto _TL2A0;_TL29F: _T53F=0;_TL2A0: _T546=Cyc_Absyn_UnknownAggr(_T53A,_T53E,_T53F);_T547=yyyvsp;_T548=& _T547[2].v;_T549=(union Cyc_YYSTYPE*)_T548;_T54A=
Cyc_yyget_YY41(_T549);_T54B=
# 1780
Cyc_Absyn_aggr_type(_T546,_T54A);_T54C=yyyvsp;_T54D=_T54C[0];_T54E=_T54D.l;_T54F=_T54E.first_line;_T550=
Cyc_Position_loc_to_seg(_T54F);_T551=
# 1780
Cyc_Parse_type_spec(_T54B,_T550);yyval=Cyc_YY21(_T551);goto _LL0;case 130:{struct _tuple25 _T1870;
# 1785 "parse.y"
_T1870.f0=1;_T553=yyyvsp;_T554=& _T553[1].v;_T555=(union Cyc_YYSTYPE*)_T554;_T1870.f1=Cyc_yyget_YY22(_T555);_T552=_T1870;}yyval=Cyc_YY23(_T552);goto _LL0;case 131:{struct _tuple25 _T1870;
# 1786 "parse.y"
_T1870.f0=0;_T557=yyyvsp;_T558=& _T557[0].v;_T559=(union Cyc_YYSTYPE*)_T558;_T1870.f1=Cyc_yyget_YY22(_T559);_T556=_T1870;}yyval=Cyc_YY23(_T556);goto _LL0;case 132:
# 1789
 yyval=Cyc_YY22(0U);goto _LL0;case 133:
# 1790 "parse.y"
 yyval=Cyc_YY22(1U);goto _LL0;case 134:
# 1793
 yyval=Cyc_YY41(0);goto _LL0;case 135: _T55A=yyyvsp;_T55B=& _T55A[1].v;_T55C=(union Cyc_YYSTYPE*)_T55B;_T55D=
# 1794 "parse.y"
Cyc_yyget_YY41(_T55C);_T55E=Cyc_List_imp_rev(_T55D);yyval=Cyc_YY41(_T55E);goto _LL0;case 136:
# 1799 "parse.y"
 yyval=Cyc_YY25(0);goto _LL0;case 137:  {
# 1801 "parse.y"
struct Cyc_List_List*decls=0;_T55F=yyyvsp;_T560=& _T55F[0].v;_T561=(union Cyc_YYSTYPE*)_T560;{
struct Cyc_List_List*x=Cyc_yyget_YY26(_T561);_TL2A4: if(x!=0)goto _TL2A2;else{goto _TL2A3;}
_TL2A2: _T562=x;_T563=_T562->hd;_T564=(struct Cyc_List_List*)_T563;_T565=decls;decls=Cyc_List_imp_append(_T564,_T565);_T566=x;
# 1802
x=_T566->tl;goto _TL2A4;_TL2A3:;}{
# 1805
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags==0)goto _TL2A5;_T568=Cyc_List_iter_c;{
void(*_T1870)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))_T568;_T567=_T1870;}_T569=tags;_T56A=decls;_T567(Cyc_Parse_substitute_aggrfield_tags,_T569,_T56A);goto _TL2A6;_TL2A5: _TL2A6:
 yyval=Cyc_YY25(decls);goto _LL0;}}case 138:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T56C=yyyvsp;_T56D=& _T56C[0].v;_T56E=(union Cyc_YYSTYPE*)_T56D;
# 1814 "parse.y"
_T1870->hd=Cyc_yyget_YY25(_T56E);_T1870->tl=0;_T56B=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY26(_T56B);goto _LL0;case 139:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T570=yyyvsp;_T571=& _T570[1].v;_T572=(union Cyc_YYSTYPE*)_T571;
# 1815 "parse.y"
_T1870->hd=Cyc_yyget_YY25(_T572);_T573=yyyvsp;_T574=& _T573[0].v;_T575=(union Cyc_YYSTYPE*)_T574;_T1870->tl=Cyc_yyget_YY26(_T575);_T56F=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY26(_T56F);goto _LL0;case 140: _T577=yyr;{struct _tuple11*_T1870=_region_malloc(_T577,0U,sizeof(struct _tuple11));
# 1821 "parse.y"
_T1870->tl=0;_T578=yyyvsp;_T579=& _T578[0].v;_T57A=(union Cyc_YYSTYPE*)_T579;_T1870->hd=Cyc_yyget_YY18(_T57A);_T576=(struct _tuple11*)_T1870;}yyval=Cyc_YY19(_T576);goto _LL0;case 141: _T57C=yyr;{struct _tuple11*_T1870=_region_malloc(_T57C,0U,sizeof(struct _tuple11));_T57D=yyyvsp;_T57E=& _T57D[0].v;_T57F=(union Cyc_YYSTYPE*)_T57E;
# 1823 "parse.y"
_T1870->tl=Cyc_yyget_YY19(_T57F);_T580=yyyvsp;_T581=& _T580[2].v;_T582=(union Cyc_YYSTYPE*)_T581;_T1870->hd=Cyc_yyget_YY18(_T582);_T57B=(struct _tuple11*)_T1870;}yyval=Cyc_YY19(_T57B);goto _LL0;case 142:{struct _tuple12 _T1870;_T584=yyyvsp;_T585=& _T584[0].v;_T586=(union Cyc_YYSTYPE*)_T585;
# 1827 "parse.y"
_T1870.f0=Cyc_yyget_YY28(_T586);_T1870.f1=0;_T1870.f2=0;_T583=_T1870;}yyval=Cyc_YY18(_T583);goto _LL0;case 143:{struct _tuple12 _T1870;_T588=yyyvsp;_T589=& _T588[0].v;_T58A=(union Cyc_YYSTYPE*)_T589;
# 1828 "parse.y"
_T1870.f0=Cyc_yyget_YY28(_T58A);_T1870.f1=0;_T58B=yyyvsp;_T58C=& _T58B[2].v;_T58D=(union Cyc_YYSTYPE*)_T58C;_T58E=Cyc_yyget_YY63(_T58D);_T58F=yyyvsp;_T590=_T58F[1];_T591=_T590.l;_T592=_T591.first_line;_T593=Cyc_Position_loc_to_seg(_T592);_T1870.f2=Cyc_Absyn_new_exp(_T58E,_T593);_T587=_T1870;}yyval=Cyc_YY18(_T587);goto _LL0;case 144:{struct _tuple12 _T1870;_T595=yyyvsp;_T596=& _T595[0].v;_T597=(union Cyc_YYSTYPE*)_T596;
# 1829 "parse.y"
_T1870.f0=Cyc_yyget_YY28(_T597);_T598=yyyvsp;_T599=& _T598[2].v;_T59A=(union Cyc_YYSTYPE*)_T599;_T1870.f1=Cyc_yyget_Exp_tok(_T59A);_T1870.f2=0;_T594=_T1870;}yyval=Cyc_YY18(_T594);goto _LL0;case 145:{struct _tuple12 _T1870;_T59C=yyyvsp;_T59D=& _T59C[0].v;_T59E=(union Cyc_YYSTYPE*)_T59D;
# 1830 "parse.y"
_T1870.f0=Cyc_yyget_YY28(_T59E);_T59F=yyyvsp;_T5A0=& _T59F[4].v;_T5A1=(union Cyc_YYSTYPE*)_T5A0;_T1870.f1=Cyc_yyget_Exp_tok(_T5A1);_T5A2=yyyvsp;_T5A3=& _T5A2[2].v;_T5A4=(union Cyc_YYSTYPE*)_T5A3;_T5A5=Cyc_yyget_YY63(_T5A4);_T5A6=yyyvsp;_T5A7=_T5A6[1];_T5A8=_T5A7.l;_T5A9=_T5A8.first_line;_T5AA=Cyc_Position_loc_to_seg(_T5A9);_T1870.f2=Cyc_Absyn_new_exp(_T5A5,_T5AA);_T59B=_T1870;}yyval=Cyc_YY18(_T59B);goto _LL0;case 146:  {struct _RegionHandle _T1870=_new_region(0U,"temp");struct _RegionHandle*temp=& _T1870;_push_region(temp);_T5AB=yyyvsp;_T5AC=& _T5AB[0].v;_T5AD=(union Cyc_YYSTYPE*)_T5AC;{
# 1836 "parse.y"
struct _tuple26 _T1871=Cyc_yyget_YY36(_T5AD);struct Cyc_List_List*_T1872;struct Cyc_Parse_Type_specifier _T1873;struct Cyc_Absyn_Tqual _T1874;_T1874=_T1871.f0;_T1873=_T1871.f1;_T1872=_T1871.f2;{struct Cyc_Absyn_Tqual tq=_T1874;struct Cyc_Parse_Type_specifier tspecs=_T1873;struct Cyc_List_List*atts=_T1872;_T5AE=tq;_T5AF=_T5AE.loc;
if(_T5AF!=0U)goto _TL2A7;_T5B0=yyyvsp;_T5B1=_T5B0[0];_T5B2=_T5B1.l;_T5B3=_T5B2.first_line;tq.loc=Cyc_Position_loc_to_seg(_T5B3);goto _TL2A8;_TL2A7: _TL2A8: {
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;_T5B4=yyyvsp;_T5B5=& _T5B4[1].v;_T5B6=(union Cyc_YYSTYPE*)_T5B5;{
struct Cyc_List_List*x=Cyc_yyget_YY30(_T5B6);_TL2AC: if(x!=0)goto _TL2AA;else{goto _TL2AB;}
_TL2AA: _T5B7=x;_T5B8=_T5B7->hd;{struct _tuple12*_T1875=(struct _tuple12*)_T5B8;struct Cyc_Absyn_Exp*_T1876;struct Cyc_Absyn_Exp*_T1877;struct Cyc_Parse_Declarator _T1878;{struct _tuple12 _T1879=*_T1875;_T1878=_T1879.f0;_T1877=_T1879.f1;_T1876=_T1879.f2;}{struct Cyc_Parse_Declarator d=_T1878;struct Cyc_Absyn_Exp*wd=_T1877;struct Cyc_Absyn_Exp*wh=_T1876;_T5BA=temp;{struct _tuple13*_T1879=_region_malloc(_T5BA,0U,sizeof(struct _tuple13));
_T1879->tl=decls;_T1879->hd=d;_T5B9=(struct _tuple13*)_T1879;}decls=_T5B9;_T5BC=temp;{struct Cyc_List_List*_T1879=_region_malloc(_T5BC,0U,sizeof(struct Cyc_List_List));_T5BE=temp;{struct _tuple16*_T187A=_region_malloc(_T5BE,0U,sizeof(struct _tuple16));
_T187A->f0=wd;_T187A->f1=wh;_T5BD=(struct _tuple16*)_T187A;}_T1879->hd=_T5BD;_T1879->tl=widths_and_reqs;_T5BB=(struct Cyc_List_List*)_T1879;}widths_and_reqs=_T5BB;}}_T5BF=x;
# 1840
x=_T5BF->tl;goto _TL2AC;_TL2AB:;}_T5C0=tspecs;_T5C1=yyyvsp;_T5C2=_T5C1[0];_T5C3=_T5C2.l;_T5C4=_T5C3.first_line;_T5C5=
# 1845
Cyc_Position_loc_to_seg(_T5C4);{void*t=Cyc_Parse_speclist2typ(_T5C0,_T5C5);_T5C6=temp;_T5C7=temp;_T5C8=
# 1847
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);_T5C9=widths_and_reqs;{
# 1846
struct Cyc_List_List*info=Cyc_List_rzip(_T5C6,_T5C7,_T5C8,_T5C9);_T5CB=Cyc_List_map_c;{
# 1848
struct Cyc_List_List*(*_T1875)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))_T5CB;_T5CA=_T1875;}_T5CC=yyyvsp;_T5CD=_T5CC[0];_T5CE=_T5CD.l;_T5CF=_T5CE.first_line;_T5D0=Cyc_Position_loc_to_seg(_T5CF);_T5D1=info;_T5D2=_T5CA(Cyc_Parse_make_aggr_field,_T5D0,_T5D1);yyval=Cyc_YY25(_T5D2);_npop_handler(0);goto _LL0;}}}}}_pop_region();}case 147:{struct _tuple26 _T1870;_T5D4=yyyvsp;_T5D5=_T5D4[0];_T5D6=_T5D5.l;_T5D7=_T5D6.first_line;_T5D8=
# 1857 "parse.y"
Cyc_Position_loc_to_seg(_T5D7);_T1870.f0=Cyc_Absyn_empty_tqual(_T5D8);_T5D9=yyyvsp;_T5DA=& _T5D9[0].v;_T5DB=(union Cyc_YYSTYPE*)_T5DA;_T1870.f1=Cyc_yyget_YY21(_T5DB);_T1870.f2=0;_T5D3=_T1870;}yyval=Cyc_YY36(_T5D3);goto _LL0;case 148: _T5DC=yyyvsp;_T5DD=& _T5DC[1].v;_T5DE=(union Cyc_YYSTYPE*)_T5DD;{
# 1859 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T5DE);{struct _tuple26 _T1870;_T5E0=two;_T1870.f0=_T5E0.f0;_T5E1=yyyvsp;_T5E2=_T5E1[0];_T5E3=_T5E2.l;_T5E4=_T5E3.first_line;_T5E5=Cyc_Position_loc_to_seg(_T5E4);_T5E6=yyyvsp;_T5E7=& _T5E6[0].v;_T5E8=(union Cyc_YYSTYPE*)_T5E7;_T5E9=Cyc_yyget_YY21(_T5E8);_T5EA=two;_T5EB=_T5EA.f1;_T1870.f1=Cyc_Parse_combine_specifiers(_T5E5,_T5E9,_T5EB);_T5EC=two;_T1870.f2=_T5EC.f2;_T5DF=_T1870;}yyval=Cyc_YY36(_T5DF);goto _LL0;}case 149:{struct _tuple26 _T1870;_T5EE=yyyvsp;_T5EF=& _T5EE[0].v;_T5F0=(union Cyc_YYSTYPE*)_T5EF;
# 1861 "parse.y"
_T1870.f0=Cyc_yyget_YY24(_T5F0);_T1870.f1=Cyc_Parse_empty_spec(0U);_T1870.f2=0;_T5ED=_T1870;}yyval=Cyc_YY36(_T5ED);goto _LL0;case 150: _T5F1=yyyvsp;_T5F2=& _T5F1[1].v;_T5F3=(union Cyc_YYSTYPE*)_T5F2;{
# 1863 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T5F3);{struct _tuple26 _T1870;_T5F5=yyyvsp;_T5F6=& _T5F5[0].v;_T5F7=(union Cyc_YYSTYPE*)_T5F6;_T5F8=Cyc_yyget_YY24(_T5F7);_T5F9=two;_T5FA=_T5F9.f0;_T1870.f0=Cyc_Absyn_combine_tqual(_T5F8,_T5FA);_T5FB=two;_T1870.f1=_T5FB.f1;_T5FC=two;_T1870.f2=_T5FC.f2;_T5F4=_T1870;}yyval=Cyc_YY36(_T5F4);goto _LL0;}case 151:{struct _tuple26 _T1870;_T5FE=yyyvsp;_T5FF=_T5FE[0];_T600=_T5FF.l;_T601=_T600.first_line;_T602=
# 1865 "parse.y"
Cyc_Position_loc_to_seg(_T601);_T1870.f0=Cyc_Absyn_empty_tqual(_T602);_T1870.f1=Cyc_Parse_empty_spec(0U);_T603=yyyvsp;_T604=& _T603[0].v;_T605=(union Cyc_YYSTYPE*)_T604;_T1870.f2=Cyc_yyget_YY46(_T605);_T5FD=_T1870;}yyval=Cyc_YY36(_T5FD);goto _LL0;case 152: _T606=yyyvsp;_T607=& _T606[1].v;_T608=(union Cyc_YYSTYPE*)_T607;{
# 1867 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T608);{struct _tuple26 _T1870;_T60A=two;_T1870.f0=_T60A.f0;_T60B=two;_T1870.f1=_T60B.f1;_T60C=yyyvsp;_T60D=& _T60C[0].v;_T60E=(union Cyc_YYSTYPE*)_T60D;_T60F=Cyc_yyget_YY46(_T60E);_T610=two;_T611=_T610.f2;_T1870.f2=Cyc_List_append(_T60F,_T611);_T609=_T1870;}yyval=Cyc_YY36(_T609);goto _LL0;}case 153:{struct _tuple26 _T1870;_T613=yyyvsp;_T614=_T613[0];_T615=_T614.l;_T616=_T615.first_line;_T617=
# 1873 "parse.y"
Cyc_Position_loc_to_seg(_T616);_T1870.f0=Cyc_Absyn_empty_tqual(_T617);_T618=yyyvsp;_T619=& _T618[0].v;_T61A=(union Cyc_YYSTYPE*)_T619;_T1870.f1=Cyc_yyget_YY21(_T61A);_T1870.f2=0;_T612=_T1870;}yyval=Cyc_YY36(_T612);goto _LL0;case 154: _T61B=yyyvsp;_T61C=& _T61B[1].v;_T61D=(union Cyc_YYSTYPE*)_T61C;{
# 1875 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T61D);{struct _tuple26 _T1870;_T61F=two;_T1870.f0=_T61F.f0;_T620=yyyvsp;_T621=_T620[0];_T622=_T621.l;_T623=_T622.first_line;_T624=Cyc_Position_loc_to_seg(_T623);_T625=yyyvsp;_T626=& _T625[0].v;_T627=(union Cyc_YYSTYPE*)_T626;_T628=Cyc_yyget_YY21(_T627);_T629=two;_T62A=_T629.f1;_T1870.f1=Cyc_Parse_combine_specifiers(_T624,_T628,_T62A);_T62B=two;_T1870.f2=_T62B.f2;_T61E=_T1870;}yyval=Cyc_YY36(_T61E);goto _LL0;}case 155:{struct _tuple26 _T1870;_T62D=yyyvsp;_T62E=& _T62D[0].v;_T62F=(union Cyc_YYSTYPE*)_T62E;
# 1877 "parse.y"
_T1870.f0=Cyc_yyget_YY24(_T62F);_T1870.f1=Cyc_Parse_empty_spec(0U);_T1870.f2=0;_T62C=_T1870;}yyval=Cyc_YY36(_T62C);goto _LL0;case 156: _T630=yyyvsp;_T631=& _T630[1].v;_T632=(union Cyc_YYSTYPE*)_T631;{
# 1879 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T632);{struct _tuple26 _T1870;_T634=yyyvsp;_T635=& _T634[0].v;_T636=(union Cyc_YYSTYPE*)_T635;_T637=Cyc_yyget_YY24(_T636);_T638=two;_T639=_T638.f0;_T1870.f0=Cyc_Absyn_combine_tqual(_T637,_T639);_T63A=two;_T1870.f1=_T63A.f1;_T63B=two;_T1870.f2=_T63B.f2;_T633=_T1870;}yyval=Cyc_YY36(_T633);goto _LL0;}case 157:{struct _tuple26 _T1870;_T63D=yyyvsp;_T63E=_T63D[0];_T63F=_T63E.l;_T640=_T63F.first_line;_T641=
# 1881 "parse.y"
Cyc_Position_loc_to_seg(_T640);_T1870.f0=Cyc_Absyn_empty_tqual(_T641);_T1870.f1=Cyc_Parse_empty_spec(0U);_T642=yyyvsp;_T643=& _T642[0].v;_T644=(union Cyc_YYSTYPE*)_T643;_T1870.f2=Cyc_yyget_YY46(_T644);_T63C=_T1870;}yyval=Cyc_YY36(_T63C);goto _LL0;case 158: _T645=yyyvsp;_T646=& _T645[1].v;_T647=(union Cyc_YYSTYPE*)_T646;{
# 1883 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T647);{struct _tuple26 _T1870;_T649=two;_T1870.f0=_T649.f0;_T64A=two;_T1870.f1=_T64A.f1;_T64B=yyyvsp;_T64C=& _T64B[0].v;_T64D=(union Cyc_YYSTYPE*)_T64C;_T64E=Cyc_yyget_YY46(_T64D);_T64F=two;_T650=_T64F.f2;_T1870.f2=Cyc_List_append(_T64E,_T650);_T648=_T1870;}yyval=Cyc_YY36(_T648);goto _LL0;}case 159: _T652=yyr;{struct Cyc_List_List*_T1870=_region_malloc(_T652,0U,sizeof(struct Cyc_List_List));_T653=yyyvsp;_T654=& _T653[0].v;_T655=(union Cyc_YYSTYPE*)_T654;
# 1889 "parse.y"
_T1870->hd=Cyc_yyget_YY29(_T655);_T1870->tl=0;_T651=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY30(_T651);goto _LL0;case 160: _T657=yyr;{struct Cyc_List_List*_T1870=_region_malloc(_T657,0U,sizeof(struct Cyc_List_List));_T658=yyyvsp;_T659=& _T658[2].v;_T65A=(union Cyc_YYSTYPE*)_T659;
# 1891 "parse.y"
_T1870->hd=Cyc_yyget_YY29(_T65A);_T65B=yyyvsp;_T65C=& _T65B[0].v;_T65D=(union Cyc_YYSTYPE*)_T65C;_T1870->tl=Cyc_yyget_YY30(_T65D);_T656=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY30(_T656);goto _LL0;case 161: _T65F=yyr;{struct _tuple12*_T1870=_region_malloc(_T65F,0U,sizeof(struct _tuple12));_T660=yyyvsp;_T661=& _T660[0].v;_T662=(union Cyc_YYSTYPE*)_T661;
# 1896 "parse.y"
_T1870->f0=Cyc_yyget_YY28(_T662);_T1870->f1=0;_T663=yyyvsp;_T664=& _T663[1].v;_T665=(union Cyc_YYSTYPE*)_T664;_T1870->f2=Cyc_yyget_YY61(_T665);_T65E=(struct _tuple12*)_T1870;}yyval=Cyc_YY29(_T65E);goto _LL0;case 162: _T667=yyr;{struct _tuple12*_T1870=_region_malloc(_T667,0U,sizeof(struct _tuple12));{struct _tuple0*_T1871=_cycalloc(sizeof(struct _tuple0));
# 1900 "parse.y"
_T1871->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));*_T1872=_tag_fat("",sizeof(char),1U);_T669=(struct _fat_ptr*)_T1872;}_T1871->f1=_T669;_T668=(struct _tuple0*)_T1871;}_T1870->f0.id=_T668;_T1870->f0.varloc=0U;_T1870->f0.tms=0;_T66A=yyyvsp;_T66B=& _T66A[1].v;_T66C=(union Cyc_YYSTYPE*)_T66B;_T1870->f1=Cyc_yyget_Exp_tok(_T66C);_T1870->f2=0;_T666=(struct _tuple12*)_T1870;}yyval=Cyc_YY29(_T666);goto _LL0;case 163: _T66E=yyr;{struct _tuple12*_T1870=_region_malloc(_T66E,0U,sizeof(struct _tuple12));{struct _tuple0*_T1871=_cycalloc(sizeof(struct _tuple0));
# 1905 "parse.y"
_T1871->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));*_T1872=_tag_fat("",sizeof(char),1U);_T670=(struct _fat_ptr*)_T1872;}_T1871->f1=_T670;_T66F=(struct _tuple0*)_T1871;}_T1870->f0.id=_T66F;_T1870->f0.varloc=0U;_T1870->f0.tms=0;_T1870->f1=0;_T1870->f2=0;_T66D=(struct _tuple12*)_T1870;}yyval=Cyc_YY29(_T66D);goto _LL0;case 164: _T672=yyr;{struct _tuple12*_T1870=_region_malloc(_T672,0U,sizeof(struct _tuple12));_T673=yyyvsp;_T674=& _T673[0].v;_T675=(union Cyc_YYSTYPE*)_T674;
# 1908 "parse.y"
_T1870->f0=Cyc_yyget_YY28(_T675);_T676=yyyvsp;_T677=& _T676[2].v;_T678=(union Cyc_YYSTYPE*)_T677;_T1870->f1=Cyc_yyget_Exp_tok(_T678);_T1870->f2=0;_T671=(struct _tuple12*)_T1870;}yyval=Cyc_YY29(_T671);goto _LL0;case 165: _T679=yyyvsp;_T67A=& _T679[2].v;_T67B=(union Cyc_YYSTYPE*)_T67A;_T67C=
# 1912 "parse.y"
Cyc_yyget_Exp_tok(_T67B);yyval=Cyc_YY61(_T67C);goto _LL0;case 166:
# 1913 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 167: _T67D=yyyvsp;_T67E=& _T67D[0].v;_T67F=(union Cyc_YYSTYPE*)_T67E;{
# 1919 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T67F);_T681=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1870)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T681;_T680=_T1870;}_T682=yyyvsp;_T683=_T682[2];_T684=_T683.l;_T685=_T684.first_line;_T686=Cyc_Position_loc_to_seg(_T685);_T687=yyyvsp;_T688=& _T687[2].v;_T689=(union Cyc_YYSTYPE*)_T688;_T68A=Cyc_yyget_YY41(_T689);{struct Cyc_List_List*ts=_T680(Cyc_Parse_typ2tvar,_T686,_T68A);_T68B=yyyvsp;_T68C=& _T68B[1].v;_T68D=(union Cyc_YYSTYPE*)_T68C;_T68E=
Cyc_yyget_QualId_tok(_T68D);_T68F=ts;{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));_T691=yyyvsp;_T692=& _T691[4].v;_T693=(union Cyc_YYSTYPE*)_T692;_T1870->v=Cyc_yyget_YY35(_T693);_T690=(struct Cyc_Core_Opt*)_T1870;}_T694=is_extensible;_T695=yyyvsp;_T696=_T695[0];_T697=_T696.l;_T698=_T697.first_line;_T699=
Cyc_Position_loc_to_seg(_T698);{
# 1921
struct Cyc_Absyn_TypeDecl*dd=Cyc_Absyn_datatype_tdecl(2U,_T68E,_T68F,_T690,_T694,_T699);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T1870->tag=10;
# 1923
_T1870->f1=dd;_T1870->f2=0;_T69A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1870;}_T69B=(void*)_T69A;_T69C=yyyvsp;_T69D=_T69C[0];_T69E=_T69D.l;_T69F=_T69E.first_line;_T6A0=Cyc_Position_loc_to_seg(_T69F);_T6A1=Cyc_Parse_type_spec(_T69B,_T6A0);yyval=Cyc_YY21(_T6A1);goto _LL0;}}}case 168: _T6A2=yyyvsp;_T6A3=& _T6A2[0].v;_T6A4=(union Cyc_YYSTYPE*)_T6A3;{
# 1926 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6A4);{struct Cyc_Absyn_UnknownDatatypeInfo _T1870;_T6A6=yyyvsp;_T6A7=& _T6A6[1].v;_T6A8=(union Cyc_YYSTYPE*)_T6A7;
_T1870.name=Cyc_yyget_QualId_tok(_T6A8);_T1870.is_extensible=is_extensible;_T6A5=_T1870;}_T6A9=Cyc_Absyn_UnknownDatatype(_T6A5);_T6AA=yyyvsp;_T6AB=& _T6AA[2].v;_T6AC=(union Cyc_YYSTYPE*)_T6AB;_T6AD=Cyc_yyget_YY41(_T6AC);_T6AE=Cyc_Absyn_datatype_type(_T6A9,_T6AD);_T6AF=yyyvsp;_T6B0=_T6AF[0];_T6B1=_T6B0.l;_T6B2=_T6B1.first_line;_T6B3=Cyc_Position_loc_to_seg(_T6B2);_T6B4=Cyc_Parse_type_spec(_T6AE,_T6B3);yyval=Cyc_YY21(_T6B4);goto _LL0;}case 169: _T6B5=yyyvsp;_T6B6=& _T6B5[0].v;_T6B7=(union Cyc_YYSTYPE*)_T6B6;{
# 1930 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6B7);{struct Cyc_Absyn_UnknownDatatypeFieldInfo _T1870;_T6B9=yyyvsp;_T6BA=& _T6B9[1].v;_T6BB=(union Cyc_YYSTYPE*)_T6BA;
_T1870.datatype_name=Cyc_yyget_QualId_tok(_T6BB);_T6BC=yyyvsp;_T6BD=& _T6BC[3].v;_T6BE=(union Cyc_YYSTYPE*)_T6BD;_T1870.field_name=Cyc_yyget_QualId_tok(_T6BE);_T1870.is_extensible=is_extensible;_T6B8=_T1870;}_T6BF=Cyc_Absyn_UnknownDatatypefield(_T6B8);_T6C0=yyyvsp;_T6C1=& _T6C0[4].v;_T6C2=(union Cyc_YYSTYPE*)_T6C1;_T6C3=Cyc_yyget_YY41(_T6C2);_T6C4=Cyc_Absyn_datatype_field_type(_T6BF,_T6C3);_T6C5=yyyvsp;_T6C6=_T6C5[0];_T6C7=_T6C6.l;_T6C8=_T6C7.first_line;_T6C9=Cyc_Position_loc_to_seg(_T6C8);_T6CA=Cyc_Parse_type_spec(_T6C4,_T6C9);yyval=Cyc_YY21(_T6CA);goto _LL0;}case 170:
# 1936 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 171:
# 1937 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 172:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T6CC=yyyvsp;_T6CD=& _T6CC[0].v;_T6CE=(union Cyc_YYSTYPE*)_T6CD;
# 1941 "parse.y"
_T1870->hd=Cyc_yyget_YY34(_T6CE);_T1870->tl=0;_T6CB=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY35(_T6CB);goto _LL0;case 173:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T6D0=yyyvsp;_T6D1=& _T6D0[0].v;_T6D2=(union Cyc_YYSTYPE*)_T6D1;
# 1942 "parse.y"
_T1870->hd=Cyc_yyget_YY34(_T6D2);_T1870->tl=0;_T6CF=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY35(_T6CF);goto _LL0;case 174:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T6D4=yyyvsp;_T6D5=& _T6D4[0].v;_T6D6=(union Cyc_YYSTYPE*)_T6D5;
# 1943 "parse.y"
_T1870->hd=Cyc_yyget_YY34(_T6D6);_T6D7=yyyvsp;_T6D8=& _T6D7[2].v;_T6D9=(union Cyc_YYSTYPE*)_T6D8;_T1870->tl=Cyc_yyget_YY35(_T6D9);_T6D3=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY35(_T6D3);goto _LL0;case 175:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T6DB=yyyvsp;_T6DC=& _T6DB[0].v;_T6DD=(union Cyc_YYSTYPE*)_T6DC;
# 1944 "parse.y"
_T1870->hd=Cyc_yyget_YY34(_T6DD);_T6DE=yyyvsp;_T6DF=& _T6DE[2].v;_T6E0=(union Cyc_YYSTYPE*)_T6DF;_T1870->tl=Cyc_yyget_YY35(_T6E0);_T6DA=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY35(_T6DA);goto _LL0;case 176:
# 1948 "parse.y"
 yyval=Cyc_YY33(2U);goto _LL0;case 177:
# 1949 "parse.y"
 yyval=Cyc_YY33(3U);goto _LL0;case 178:
# 1950 "parse.y"
 yyval=Cyc_YY33(0U);goto _LL0;case 179:{struct Cyc_Absyn_Datatypefield*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T6E2=yyyvsp;_T6E3=& _T6E2[1].v;_T6E4=(union Cyc_YYSTYPE*)_T6E3;
# 1954 "parse.y"
_T1870->name=Cyc_yyget_QualId_tok(_T6E4);_T1870->typs=0;_T6E5=yyyvsp;_T6E6=_T6E5[0];_T6E7=_T6E6.l;_T6E8=_T6E7.first_line;_T1870->loc=Cyc_Position_loc_to_seg(_T6E8);_T6E9=yyyvsp;_T6EA=& _T6E9[0].v;_T6EB=(union Cyc_YYSTYPE*)_T6EA;_T1870->sc=Cyc_yyget_YY33(_T6EB);_T6E1=(struct Cyc_Absyn_Datatypefield*)_T1870;}yyval=Cyc_YY34(_T6E1);goto _LL0;case 180: _T6ED=Cyc_List_map_c;{
# 1956 "parse.y"
struct Cyc_List_List*(*_T1870)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T6ED;_T6EC=_T1870;}{struct _tuple19*(*_T1870)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T6EE=_T1870;}_T6EF=yyyvsp;_T6F0=_T6EF[3];_T6F1=_T6F0.l;_T6F2=_T6F1.first_line;_T6F3=Cyc_Position_loc_to_seg(_T6F2);_T6F4=yyyvsp;_T6F5=& _T6F4[3].v;_T6F6=(union Cyc_YYSTYPE*)_T6F5;_T6F7=Cyc_yyget_YY39(_T6F6);_T6F8=Cyc_List_imp_rev(_T6F7);{struct Cyc_List_List*typs=_T6EC(_T6EE,_T6F3,_T6F8);{struct Cyc_Absyn_Datatypefield*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T6FA=yyyvsp;_T6FB=& _T6FA[1].v;_T6FC=(union Cyc_YYSTYPE*)_T6FB;
_T1870->name=Cyc_yyget_QualId_tok(_T6FC);_T1870->typs=typs;_T6FD=yyyvsp;_T6FE=_T6FD[0];_T6FF=_T6FE.l;_T700=_T6FF.first_line;_T1870->loc=Cyc_Position_loc_to_seg(_T700);_T701=yyyvsp;_T702=& _T701[0].v;_T703=(union Cyc_YYSTYPE*)_T702;_T1870->sc=Cyc_yyget_YY33(_T703);_T6F9=(struct Cyc_Absyn_Datatypefield*)_T1870;}yyval=Cyc_YY34(_T6F9);goto _LL0;}case 181: _T704=yyyvsp;_T705=_T704[0];
# 1962 "parse.y"
yyval=_T705.v;goto _LL0;case 182: _T706=yyyvsp;_T707=& _T706[1].v;_T708=(union Cyc_YYSTYPE*)_T707;{
# 1964 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T708);{struct Cyc_Parse_Declarator _T1870;_T70A=two;
_T1870.id=_T70A.id;_T70B=two;_T1870.varloc=_T70B.varloc;_T70C=yyyvsp;_T70D=& _T70C[0].v;_T70E=(union Cyc_YYSTYPE*)_T70D;_T70F=Cyc_yyget_YY27(_T70E);_T710=two;_T711=_T710.tms;_T1870.tms=Cyc_List_imp_append(_T70F,_T711);_T709=_T1870;}yyval=Cyc_YY28(_T709);goto _LL0;}case 183: _T712=yyyvsp;_T713=_T712[0];
# 1971 "parse.y"
yyval=_T713.v;goto _LL0;case 184: _T714=yyyvsp;_T715=& _T714[1].v;_T716=(union Cyc_YYSTYPE*)_T715;{
# 1973 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T716);{struct Cyc_Parse_Declarator _T1870;_T718=two;
_T1870.id=_T718.id;_T719=two;_T1870.varloc=_T719.varloc;_T71A=yyyvsp;_T71B=& _T71A[0].v;_T71C=(union Cyc_YYSTYPE*)_T71B;_T71D=Cyc_yyget_YY27(_T71C);_T71E=two;_T71F=_T71E.tms;_T1870.tms=Cyc_List_imp_append(_T71D,_T71F);_T717=_T1870;}yyval=Cyc_YY28(_T717);goto _LL0;}case 185:{struct Cyc_Parse_Declarator _T1870;_T721=yyyvsp;_T722=& _T721[0].v;_T723=(union Cyc_YYSTYPE*)_T722;
# 1978 "parse.y"
_T1870.id=Cyc_yyget_QualId_tok(_T723);_T724=yyyvsp;_T725=_T724[0];_T726=_T725.l;_T727=_T726.first_line;_T1870.varloc=Cyc_Position_loc_to_seg(_T727);_T1870.tms=0;_T720=_T1870;}yyval=Cyc_YY28(_T720);goto _LL0;case 186: _T728=yyyvsp;_T729=_T728[1];
# 1979 "parse.y"
yyval=_T729.v;goto _LL0;case 187: _T72A=yyyvsp;_T72B=& _T72A[2].v;_T72C=(union Cyc_YYSTYPE*)_T72B;{
# 1983 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T72C);_T72E=yyr;{struct Cyc_List_List*_T1870=_region_malloc(_T72E,0U,sizeof(struct Cyc_List_List));_T730=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1871=_region_malloc(_T730,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1871->tag=5;_T731=yyyvsp;_T732=_T731[1];_T733=_T732.l;_T734=_T733.first_line;
_T1871->f1=Cyc_Position_loc_to_seg(_T734);_T735=yyyvsp;_T736=& _T735[1].v;_T737=(union Cyc_YYSTYPE*)_T736;_T1871->f2=Cyc_yyget_YY46(_T737);_T72F=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1871;}_T1870->hd=(void*)_T72F;_T738=d;_T1870->tl=_T738.tms;_T72D=(struct Cyc_List_List*)_T1870;}d.tms=_T72D;_T739=yyyvsp;_T73A=_T739[2];
yyval=_T73A.v;goto _LL0;}case 188:{struct Cyc_Parse_Declarator _T1870;_T73C=yyyvsp;_T73D=& _T73C[0].v;_T73E=(union Cyc_YYSTYPE*)_T73D;_T73F=
# 1988 "parse.y"
Cyc_yyget_YY28(_T73E);_T1870.id=_T73F.id;_T740=yyyvsp;_T741=& _T740[0].v;_T742=(union Cyc_YYSTYPE*)_T741;_T743=Cyc_yyget_YY28(_T742);_T1870.varloc=_T743.varloc;_T745=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T745,0U,sizeof(struct Cyc_List_List));_T747=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T747,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1872->tag=0;_T748=yyyvsp;_T749=& _T748[3].v;_T74A=(union Cyc_YYSTYPE*)_T749;_T1872->f1=Cyc_yyget_YY54(_T74A);_T74B=yyyvsp;_T74C=_T74B[3];_T74D=_T74C.l;_T74E=_T74D.first_line;_T1872->f2=Cyc_Position_loc_to_seg(_T74E);_T746=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T746;_T74F=yyyvsp;_T750=& _T74F[0].v;_T751=(union Cyc_YYSTYPE*)_T750;_T752=Cyc_yyget_YY28(_T751);_T1871->tl=_T752.tms;_T744=(struct Cyc_List_List*)_T1871;}_T1870.tms=_T744;_T73B=_T1870;}yyval=Cyc_YY28(_T73B);goto _LL0;case 189:{struct Cyc_Parse_Declarator _T1870;_T754=yyyvsp;_T755=& _T754[0].v;_T756=(union Cyc_YYSTYPE*)_T755;_T757=
# 1990 "parse.y"
Cyc_yyget_YY28(_T756);_T1870.id=_T757.id;_T758=yyyvsp;_T759=& _T758[0].v;_T75A=(union Cyc_YYSTYPE*)_T759;_T75B=Cyc_yyget_YY28(_T75A);_T1870.varloc=_T75B.varloc;_T75D=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T75D,0U,sizeof(struct Cyc_List_List));_T75F=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T75F,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1872->tag=1;_T760=yyyvsp;_T761=& _T760[2].v;_T762=(union Cyc_YYSTYPE*)_T761;
_T1872->f1=Cyc_yyget_Exp_tok(_T762);_T763=yyyvsp;_T764=& _T763[4].v;_T765=(union Cyc_YYSTYPE*)_T764;_T1872->f2=Cyc_yyget_YY54(_T765);_T766=yyyvsp;_T767=_T766[4];_T768=_T767.l;_T769=_T768.first_line;_T1872->f3=Cyc_Position_loc_to_seg(_T769);_T75E=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T75E;_T76A=yyyvsp;_T76B=& _T76A[0].v;_T76C=(union Cyc_YYSTYPE*)_T76B;_T76D=Cyc_yyget_YY28(_T76C);_T1871->tl=_T76D.tms;_T75C=(struct Cyc_List_List*)_T1871;}_T1870.tms=_T75C;_T753=_T1870;}
# 1990
yyval=Cyc_YY28(_T753);goto _LL0;case 190: _T76E=yyyvsp;_T76F=& _T76E[2].v;_T770=(union Cyc_YYSTYPE*)_T76F;{
# 1993 "parse.y"
struct _tuple27*_T1870=Cyc_yyget_YY40(_T770);struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;void*_T1873;struct Cyc_Absyn_VarargInfo*_T1874;int _T1875;struct Cyc_List_List*_T1876;{struct _tuple27 _T1877=*_T1870;_T1876=_T1877.f0;_T1875=_T1877.f1;_T1874=_T1877.f2;_T1873=_T1877.f3;_T1872=_T1877.f4;_T1871=_T1877.f5;}{struct Cyc_List_List*lis=_T1876;int b=_T1875;struct Cyc_Absyn_VarargInfo*c=_T1874;void*eff=_T1873;struct Cyc_List_List*ec=_T1872;struct Cyc_List_List*qb=_T1871;_T771=yyyvsp;_T772=& _T771[4].v;_T773=(union Cyc_YYSTYPE*)_T772;{
struct _tuple21 _T1877=Cyc_yyget_YY62(_T773);struct Cyc_Absyn_Exp*_T1878;struct Cyc_Absyn_Exp*_T1879;struct Cyc_Absyn_Exp*_T187A;struct Cyc_Absyn_Exp*_T187B;_T187B=_T1877.f0;_T187A=_T1877.f1;_T1879=_T1877.f2;_T1878=_T1877.f3;{struct Cyc_Absyn_Exp*chk=_T187B;struct Cyc_Absyn_Exp*req=_T187A;struct Cyc_Absyn_Exp*ens=_T1879;struct Cyc_Absyn_Exp*thrws=_T1878;{struct Cyc_Parse_Declarator _T187C;_T775=yyyvsp;_T776=& _T775[0].v;_T777=(union Cyc_YYSTYPE*)_T776;_T778=
Cyc_yyget_YY28(_T777);_T187C.id=_T778.id;_T779=yyyvsp;_T77A=& _T779[0].v;_T77B=(union Cyc_YYSTYPE*)_T77A;_T77C=Cyc_yyget_YY28(_T77B);_T187C.varloc=_T77C.varloc;_T77E=yyr;{struct Cyc_List_List*_T187D=_region_malloc(_T77E,0U,sizeof(struct Cyc_List_List));_T780=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T187E=_region_malloc(_T780,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T187E->tag=3;_T782=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T187F=_region_malloc(_T782,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T187F->tag=1;_T187F->f1=lis;_T187F->f2=b;_T187F->f3=c;_T187F->f4=eff;_T187F->f5=ec;_T187F->f6=qb;_T187F->f7=chk;_T187F->f8=req;_T187F->f9=ens;_T187F->f10=thrws;_T781=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T187F;}_T187E->f1=(void*)_T781;_T77F=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T187E;}_T187D->hd=(void*)_T77F;_T783=yyyvsp;_T784=& _T783[0].v;_T785=(union Cyc_YYSTYPE*)_T784;_T786=Cyc_yyget_YY28(_T785);_T187D->tl=_T786.tms;_T77D=(struct Cyc_List_List*)_T187D;}_T187C.tms=_T77D;_T774=_T187C;}yyval=Cyc_YY28(_T774);goto _LL0;}}}}case 191:{struct Cyc_Parse_Declarator _T1870;_T788=yyyvsp;_T789=& _T788[0].v;_T78A=(union Cyc_YYSTYPE*)_T789;_T78B=
# 1998 "parse.y"
Cyc_yyget_YY28(_T78A);_T1870.id=_T78B.id;_T78C=yyyvsp;_T78D=& _T78C[0].v;_T78E=(union Cyc_YYSTYPE*)_T78D;_T78F=Cyc_yyget_YY28(_T78E);_T1870.varloc=_T78F.varloc;_T791=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T791,0U,sizeof(struct Cyc_List_List));_T793=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T793,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1872->tag=3;_T795=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T1873=_region_malloc(_T795,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T1873->tag=0;_T796=yyyvsp;_T797=& _T796[2].v;_T798=(union Cyc_YYSTYPE*)_T797;_T1873->f1=Cyc_yyget_YY37(_T798);_T799=yyyvsp;_T79A=_T799[0];_T79B=_T79A.l;_T79C=_T79B.first_line;_T1873->f2=Cyc_Position_loc_to_seg(_T79C);_T794=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T1873;}_T1872->f1=(void*)_T794;_T792=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T792;_T79D=yyyvsp;_T79E=& _T79D[0].v;_T79F=(union Cyc_YYSTYPE*)_T79E;_T7A0=Cyc_yyget_YY28(_T79F);_T1871->tl=_T7A0.tms;_T790=(struct Cyc_List_List*)_T1871;}_T1870.tms=_T790;_T787=_T1870;}yyval=Cyc_YY28(_T787);goto _LL0;case 192: _T7A2=Cyc_List_map_c;{
# 2001
struct Cyc_List_List*(*_T1870)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T7A2;_T7A1=_T1870;}_T7A3=yyyvsp;_T7A4=_T7A3[1];_T7A5=_T7A4.l;_T7A6=_T7A5.first_line;_T7A7=Cyc_Position_loc_to_seg(_T7A6);_T7A8=yyyvsp;_T7A9=& _T7A8[2].v;_T7AA=(union Cyc_YYSTYPE*)_T7A9;_T7AB=Cyc_yyget_YY41(_T7AA);_T7AC=Cyc_List_imp_rev(_T7AB);{struct Cyc_List_List*ts=_T7A1(Cyc_Parse_typ2tvar,_T7A7,_T7AC);{struct Cyc_Parse_Declarator _T1870;_T7AE=yyyvsp;_T7AF=& _T7AE[0].v;_T7B0=(union Cyc_YYSTYPE*)_T7AF;_T7B1=
Cyc_yyget_YY28(_T7B0);_T1870.id=_T7B1.id;_T7B2=yyyvsp;_T7B3=& _T7B2[0].v;_T7B4=(union Cyc_YYSTYPE*)_T7B3;_T7B5=Cyc_yyget_YY28(_T7B4);_T1870.varloc=_T7B5.varloc;_T7B7=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T7B7,0U,sizeof(struct Cyc_List_List));_T7B9=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T7B9,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1872->tag=4;_T1872->f1=ts;_T7BA=yyyvsp;_T7BB=_T7BA[0];_T7BC=_T7BB.l;_T7BD=_T7BC.first_line;_T1872->f2=Cyc_Position_loc_to_seg(_T7BD);_T1872->f3=0;_T7B8=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T7B8;_T7BE=yyyvsp;_T7BF=& _T7BE[0].v;_T7C0=(union Cyc_YYSTYPE*)_T7BF;_T7C1=Cyc_yyget_YY28(_T7C0);_T1871->tl=_T7C1.tms;_T7B6=(struct Cyc_List_List*)_T1871;}_T1870.tms=_T7B6;_T7AD=_T1870;}yyval=Cyc_YY28(_T7AD);goto _LL0;}case 193:{struct Cyc_Parse_Declarator _T1870;_T7C3=yyyvsp;_T7C4=& _T7C3[0].v;_T7C5=(union Cyc_YYSTYPE*)_T7C4;_T7C6=
# 2005 "parse.y"
Cyc_yyget_YY28(_T7C5);_T1870.id=_T7C6.id;_T7C7=yyyvsp;_T7C8=& _T7C7[0].v;_T7C9=(union Cyc_YYSTYPE*)_T7C8;_T7CA=Cyc_yyget_YY28(_T7C9);_T1870.varloc=_T7CA.varloc;_T7CC=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T7CC,0U,sizeof(struct Cyc_List_List));_T7CE=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T7CE,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1872->tag=5;_T7CF=yyyvsp;_T7D0=_T7CF[1];_T7D1=_T7D0.l;_T7D2=_T7D1.first_line;_T1872->f1=Cyc_Position_loc_to_seg(_T7D2);_T7D3=yyyvsp;_T7D4=& _T7D3[1].v;_T7D5=(union Cyc_YYSTYPE*)_T7D4;_T1872->f2=Cyc_yyget_YY46(_T7D5);_T7CD=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T7CD;_T7D6=yyyvsp;_T7D7=& _T7D6[0].v;_T7D8=(union Cyc_YYSTYPE*)_T7D7;_T7D9=
Cyc_yyget_YY28(_T7D8);_T1871->tl=_T7D9.tms;_T7CB=(struct Cyc_List_List*)_T1871;}
# 2005
_T1870.tms=_T7CB;_T7C2=_T1870;}yyval=Cyc_YY28(_T7C2);goto _LL0;case 194:{struct Cyc_Parse_Declarator _T1870;_T7DB=yyyvsp;_T7DC=& _T7DB[0].v;_T7DD=(union Cyc_YYSTYPE*)_T7DC;
# 2012 "parse.y"
_T1870.id=Cyc_yyget_QualId_tok(_T7DD);_T7DE=yyyvsp;_T7DF=_T7DE[0];_T7E0=_T7DF.l;_T7E1=_T7E0.first_line;_T1870.varloc=Cyc_Position_loc_to_seg(_T7E1);_T1870.tms=0;_T7DA=_T1870;}yyval=Cyc_YY28(_T7DA);goto _LL0;case 195:{struct Cyc_Parse_Declarator _T1870;_T7E3=yyyvsp;_T7E4=& _T7E3[0].v;_T7E5=(union Cyc_YYSTYPE*)_T7E4;
# 2013 "parse.y"
_T1870.id=Cyc_yyget_QualId_tok(_T7E5);_T7E6=yyyvsp;_T7E7=_T7E6[0];_T7E8=_T7E7.l;_T7E9=_T7E8.first_line;_T1870.varloc=Cyc_Position_loc_to_seg(_T7E9);_T1870.tms=0;_T7E2=_T1870;}yyval=Cyc_YY28(_T7E2);goto _LL0;case 196: _T7EA=yyyvsp;_T7EB=_T7EA[1];
# 2014 "parse.y"
yyval=_T7EB.v;goto _LL0;case 197: _T7EC=yyyvsp;_T7ED=& _T7EC[2].v;_T7EE=(union Cyc_YYSTYPE*)_T7ED;{
# 2018 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T7EE);_T7F0=yyr;{struct Cyc_List_List*_T1870=_region_malloc(_T7F0,0U,sizeof(struct Cyc_List_List));_T7F2=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1871=_region_malloc(_T7F2,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1871->tag=5;_T7F3=yyyvsp;_T7F4=_T7F3[1];_T7F5=_T7F4.l;_T7F6=_T7F5.first_line;
_T1871->f1=Cyc_Position_loc_to_seg(_T7F6);_T7F7=yyyvsp;_T7F8=& _T7F7[1].v;_T7F9=(union Cyc_YYSTYPE*)_T7F8;_T1871->f2=Cyc_yyget_YY46(_T7F9);_T7F1=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1871;}_T1870->hd=(void*)_T7F1;_T7FA=d;_T1870->tl=_T7FA.tms;_T7EF=(struct Cyc_List_List*)_T1870;}d.tms=_T7EF;_T7FB=yyyvsp;_T7FC=_T7FB[2];
yyval=_T7FC.v;goto _LL0;}case 198: _T7FD=yyyvsp;_T7FE=& _T7FD[0].v;_T7FF=(union Cyc_YYSTYPE*)_T7FE;{
# 2023 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T7FF);{struct Cyc_Parse_Declarator _T1870;_T801=one;
_T1870.id=_T801.id;_T802=one;_T1870.varloc=_T802.varloc;_T804=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T804,0U,sizeof(struct Cyc_List_List));_T806=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T806,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1872->tag=0;_T807=yyyvsp;_T808=& _T807[3].v;_T809=(union Cyc_YYSTYPE*)_T808;
_T1872->f1=Cyc_yyget_YY54(_T809);_T80A=yyyvsp;_T80B=_T80A[3];_T80C=_T80B.l;_T80D=_T80C.first_line;_T1872->f2=Cyc_Position_loc_to_seg(_T80D);_T805=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T805;_T80E=one;_T1871->tl=_T80E.tms;_T803=(struct Cyc_List_List*)_T1871;}_T1870.tms=_T803;_T800=_T1870;}
# 2024
yyval=Cyc_YY28(_T800);goto _LL0;}case 199: _T80F=yyyvsp;_T810=& _T80F[0].v;_T811=(union Cyc_YYSTYPE*)_T810;{
# 2027 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T811);{struct Cyc_Parse_Declarator _T1870;_T813=one;
_T1870.id=_T813.id;_T814=one;_T1870.varloc=_T814.varloc;_T816=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T816,0U,sizeof(struct Cyc_List_List));_T818=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T818,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1872->tag=1;_T819=yyyvsp;_T81A=& _T819[2].v;_T81B=(union Cyc_YYSTYPE*)_T81A;
_T1872->f1=Cyc_yyget_Exp_tok(_T81B);_T81C=yyyvsp;_T81D=& _T81C[4].v;_T81E=(union Cyc_YYSTYPE*)_T81D;_T1872->f2=Cyc_yyget_YY54(_T81E);_T81F=yyyvsp;_T820=_T81F[4];_T821=_T820.l;_T822=_T821.first_line;_T1872->f3=Cyc_Position_loc_to_seg(_T822);_T817=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T817;_T823=one;
_T1871->tl=_T823.tms;_T815=(struct Cyc_List_List*)_T1871;}
# 2029
_T1870.tms=_T815;_T812=_T1870;}
# 2028
yyval=Cyc_YY28(_T812);goto _LL0;}case 200: _T824=yyyvsp;_T825=& _T824[2].v;_T826=(union Cyc_YYSTYPE*)_T825;{
# 2032 "parse.y"
struct _tuple27*_T1870=Cyc_yyget_YY40(_T826);struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;void*_T1873;struct Cyc_Absyn_VarargInfo*_T1874;int _T1875;struct Cyc_List_List*_T1876;{struct _tuple27 _T1877=*_T1870;_T1876=_T1877.f0;_T1875=_T1877.f1;_T1874=_T1877.f2;_T1873=_T1877.f3;_T1872=_T1877.f4;_T1871=_T1877.f5;}{struct Cyc_List_List*lis=_T1876;int b=_T1875;struct Cyc_Absyn_VarargInfo*c=_T1874;void*eff=_T1873;struct Cyc_List_List*ec=_T1872;struct Cyc_List_List*qb=_T1871;_T827=yyyvsp;_T828=& _T827[4].v;_T829=(union Cyc_YYSTYPE*)_T828;{
struct _tuple21 _T1877=Cyc_yyget_YY62(_T829);struct Cyc_Absyn_Exp*_T1878;struct Cyc_Absyn_Exp*_T1879;struct Cyc_Absyn_Exp*_T187A;struct Cyc_Absyn_Exp*_T187B;_T187B=_T1877.f0;_T187A=_T1877.f1;_T1879=_T1877.f2;_T1878=_T1877.f3;{struct Cyc_Absyn_Exp*chk=_T187B;struct Cyc_Absyn_Exp*req=_T187A;struct Cyc_Absyn_Exp*ens=_T1879;struct Cyc_Absyn_Exp*thrws=_T1878;_T82A=yyyvsp;_T82B=& _T82A[0].v;_T82C=(union Cyc_YYSTYPE*)_T82B;{
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T82C);{struct Cyc_Parse_Declarator _T187C;_T82E=one;
_T187C.id=_T82E.id;_T82F=one;_T187C.varloc=_T82F.varloc;_T831=yyr;{struct Cyc_List_List*_T187D=_region_malloc(_T831,0U,sizeof(struct Cyc_List_List));_T833=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T187E=_region_malloc(_T833,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T187E->tag=3;_T835=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T187F=_region_malloc(_T835,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T187F->tag=1;_T187F->f1=lis;_T187F->f2=b;_T187F->f3=c;_T187F->f4=eff;_T187F->f5=ec;_T187F->f6=qb;_T187F->f7=chk;_T187F->f8=req;_T187F->f9=ens;_T187F->f10=thrws;_T834=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T187F;}_T187E->f1=(void*)_T834;_T832=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T187E;}_T187D->hd=(void*)_T832;_T836=one;_T187D->tl=_T836.tms;_T830=(struct Cyc_List_List*)_T187D;}_T187C.tms=_T830;_T82D=_T187C;}yyval=Cyc_YY28(_T82D);goto _LL0;}}}}}case 201: _T837=yyyvsp;_T838=& _T837[0].v;_T839=(union Cyc_YYSTYPE*)_T838;{
# 2038 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T839);{struct Cyc_Parse_Declarator _T1870;_T83B=one;
_T1870.id=_T83B.id;_T83C=one;_T1870.varloc=_T83C.varloc;_T83E=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T83E,0U,sizeof(struct Cyc_List_List));_T840=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T840,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1872->tag=3;_T842=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T1873=_region_malloc(_T842,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T1873->tag=0;_T843=yyyvsp;_T844=& _T843[2].v;_T845=(union Cyc_YYSTYPE*)_T844;_T1873->f1=Cyc_yyget_YY37(_T845);_T846=yyyvsp;_T847=_T846[0];_T848=_T847.l;_T849=_T848.first_line;_T1873->f2=Cyc_Position_loc_to_seg(_T849);_T841=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T1873;}_T1872->f1=(void*)_T841;_T83F=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T83F;_T84A=one;_T1871->tl=_T84A.tms;_T83D=(struct Cyc_List_List*)_T1871;}_T1870.tms=_T83D;_T83A=_T1870;}yyval=Cyc_YY28(_T83A);goto _LL0;}case 202: _T84C=Cyc_List_map_c;{
# 2042
struct Cyc_List_List*(*_T1870)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T84C;_T84B=_T1870;}_T84D=yyyvsp;_T84E=_T84D[1];_T84F=_T84E.l;_T850=_T84F.first_line;_T851=Cyc_Position_loc_to_seg(_T850);_T852=yyyvsp;_T853=& _T852[2].v;_T854=(union Cyc_YYSTYPE*)_T853;_T855=Cyc_yyget_YY41(_T854);_T856=Cyc_List_imp_rev(_T855);{struct Cyc_List_List*ts=_T84B(Cyc_Parse_typ2tvar,_T851,_T856);_T857=yyyvsp;_T858=& _T857[0].v;_T859=(union Cyc_YYSTYPE*)_T858;{
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T859);{struct Cyc_Parse_Declarator _T1870;_T85B=one;
_T1870.id=_T85B.id;_T85C=one;_T1870.varloc=_T85C.varloc;_T85E=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T85E,0U,sizeof(struct Cyc_List_List));_T860=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T860,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1872->tag=4;_T1872->f1=ts;_T861=yyyvsp;_T862=_T861[0];_T863=_T862.l;_T864=_T863.first_line;_T1872->f2=Cyc_Position_loc_to_seg(_T864);_T1872->f3=0;_T85F=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T85F;_T865=one;_T1871->tl=_T865.tms;_T85D=(struct Cyc_List_List*)_T1871;}_T1870.tms=_T85D;_T85A=_T1870;}yyval=Cyc_YY28(_T85A);goto _LL0;}}case 203: _T866=yyyvsp;_T867=& _T866[0].v;_T868=(union Cyc_YYSTYPE*)_T867;{
# 2046 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T868);{struct Cyc_Parse_Declarator _T1870;_T86A=one;
_T1870.id=_T86A.id;_T86B=one;_T1870.varloc=_T86B.varloc;_T86D=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_T86D,0U,sizeof(struct Cyc_List_List));_T86F=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_T86F,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1872->tag=5;_T870=yyyvsp;_T871=_T870[1];_T872=_T871.l;_T873=_T872.first_line;_T1872->f1=Cyc_Position_loc_to_seg(_T873);_T874=yyyvsp;_T875=& _T874[1].v;_T876=(union Cyc_YYSTYPE*)_T875;_T1872->f2=Cyc_yyget_YY46(_T876);_T86E=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_T86E;_T877=one;_T1871->tl=_T877.tms;_T86C=(struct Cyc_List_List*)_T1871;}_T1870.tms=_T86C;_T869=_T1870;}yyval=Cyc_YY28(_T869);goto _LL0;}case 204: _T878=yyyvsp;_T879=_T878[0];
# 2052 "parse.y"
yyval=_T879.v;goto _LL0;case 205: _T87A=yyyvsp;_T87B=& _T87A[0].v;_T87C=(union Cyc_YYSTYPE*)_T87B;_T87D=
# 2053 "parse.y"
Cyc_yyget_YY27(_T87C);_T87E=yyyvsp;_T87F=& _T87E[1].v;_T880=(union Cyc_YYSTYPE*)_T87F;_T881=Cyc_yyget_YY27(_T880);_T882=Cyc_List_imp_append(_T87D,_T881);yyval=Cyc_YY27(_T882);goto _LL0;case 206:  {
# 2057 "parse.y"
struct Cyc_List_List*ans=0;_T883=yyyvsp;_T884=& _T883[3].v;_T885=(union Cyc_YYSTYPE*)_T884;_T886=
Cyc_yyget_YY46(_T885);if(_T886==0)goto _TL2AD;_T888=yyr;{struct Cyc_List_List*_T1870=_region_malloc(_T888,0U,sizeof(struct Cyc_List_List));_T88A=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1871=_region_malloc(_T88A,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1871->tag=5;_T88B=yyyvsp;_T88C=_T88B[3];_T88D=_T88C.l;_T88E=_T88D.first_line;
_T1871->f1=Cyc_Position_loc_to_seg(_T88E);_T88F=yyyvsp;_T890=& _T88F[3].v;_T891=(union Cyc_YYSTYPE*)_T890;_T1871->f2=Cyc_yyget_YY46(_T891);_T889=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1871;}_T1870->hd=(void*)_T889;_T1870->tl=ans;_T887=(struct Cyc_List_List*)_T1870;}ans=_T887;goto _TL2AE;_TL2AD: _TL2AE: {
# 2061
struct Cyc_Absyn_PtrLoc*ptrloc=0;_T892=yyyvsp;_T893=& _T892[0].v;_T894=(union Cyc_YYSTYPE*)_T893;_T895=
Cyc_yyget_YY1(_T894);{struct _tuple22 _T1870=*_T895;void*_T1871;void*_T1872;unsigned _T1873;_T1873=_T1870.f0;_T1872=_T1870.f1;_T1871=_T1870.f2;{unsigned ploc=_T1873;void*nullable=_T1872;void*bound=_T1871;_T896=Cyc_Flags_porting_c_code;
if(!_T896)goto _TL2AF;{struct Cyc_Absyn_PtrLoc*_T1874=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));
_T1874->ptr_loc=ploc;_T898=yyyvsp;_T899=_T898[2];_T89A=_T899.l;_T89B=_T89A.first_line;_T1874->rgn_loc=Cyc_Position_loc_to_seg(_T89B);_T89C=yyyvsp;_T89D=_T89C[1];_T89E=_T89D.l;_T89F=_T89E.first_line;_T1874->zt_loc=Cyc_Position_loc_to_seg(_T89F);_T897=(struct Cyc_Absyn_PtrLoc*)_T1874;}ptrloc=_T897;goto _TL2B0;_TL2AF: _TL2B0: _T8A0=yyr;_T8A1=ptrloc;_T8A2=nullable;_T8A3=bound;_T8A4=yyyvsp;_T8A5=& _T8A4[2].v;_T8A6=(union Cyc_YYSTYPE*)_T8A5;_T8A7=
Cyc_yyget_YY45(_T8A6);_T8A8=yyyvsp;_T8A9=& _T8A8[1].v;_T8AA=(union Cyc_YYSTYPE*)_T8A9;_T8AB=Cyc_yyget_YY60(_T8AA);_T8AC=yyyvsp;_T8AD=& _T8AC[4].v;_T8AE=(union Cyc_YYSTYPE*)_T8AD;_T8AF=Cyc_yyget_YY24(_T8AE);{void*mod=Cyc_Parse_make_pointer_mod(_T8A0,_T8A1,_T8A2,_T8A3,_T8A7,_T8AB,_T8AF);_T8B1=yyr;{struct Cyc_List_List*_T1874=_region_malloc(_T8B1,0U,sizeof(struct Cyc_List_List));
_T1874->hd=mod;_T1874->tl=ans;_T8B0=(struct Cyc_List_List*)_T1874;}ans=_T8B0;
yyval=Cyc_YY27(ans);goto _LL0;}}}}}case 207:
# 2071
 yyval=Cyc_YY60(0);goto _LL0;case 208: _T8B3=yyr;{struct Cyc_List_List*_T1870=_region_malloc(_T8B3,0U,sizeof(struct Cyc_List_List));_T8B4=yyyvsp;_T8B5=& _T8B4[0].v;_T8B6=(union Cyc_YYSTYPE*)_T8B5;
# 2072 "parse.y"
_T1870->hd=Cyc_yyget_YY59(_T8B6);_T8B7=yyyvsp;_T8B8=& _T8B7[1].v;_T8B9=(union Cyc_YYSTYPE*)_T8B8;_T1870->tl=Cyc_yyget_YY60(_T8B9);_T8B2=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY60(_T8B2);goto _LL0;case 209: _T8BB=yyr;{struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8BB,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_T1870->tag=0;_T8BC=yyyvsp;_T8BD=& _T8BC[2].v;_T8BE=(union Cyc_YYSTYPE*)_T8BD;
# 2077 "parse.y"
_T1870->f1=Cyc_yyget_Exp_tok(_T8BE);_T8BA=(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8BF=(void*)_T8BA;yyval=Cyc_YY59(_T8BF);goto _LL0;case 210: _T8C0=yyyvsp;_T8C1=_T8C0[0];_T8C2=_T8C1.l;_T8C3=_T8C2.first_line;_T8C4=
# 2079 "parse.y"
Cyc_Position_loc_to_seg(_T8C3);_T8C5=_tag_fat("@region qualifiers are deprecated; use @effect instead",sizeof(char),55U);_T8C6=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T8C4,_T8C5,_T8C6);_T8C8=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8C8,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T1870->tag=2;{struct Cyc_List_List*_T1871=_cycalloc(sizeof(struct Cyc_List_List));_T8CA=yyyvsp;_T8CB=& _T8CA[2].v;_T8CC=(union Cyc_YYSTYPE*)_T8CB;
_T1871->hd=Cyc_yyget_YY45(_T8CC);_T1871->tl=0;_T8C9=(struct Cyc_List_List*)_T1871;}_T1870->f1=_T8C9;_T8C7=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8CD=(void*)_T8C7;yyval=Cyc_YY59(_T8CD);goto _LL0;case 211: _T8CF=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8CF,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T1870->tag=2;_T8D0=yyyvsp;_T8D1=& _T8D0[2].v;_T8D2=(union Cyc_YYSTYPE*)_T8D1;
# 2081 "parse.y"
_T1870->f1=Cyc_yyget_YY41(_T8D2);_T8CE=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8D3=(void*)_T8CE;yyval=Cyc_YY59(_T8D3);goto _LL0;case 212: _T8D5=yyr;{struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8D5,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));
# 2082 "parse.y"
_T1870->tag=3;_T8D4=(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8D6=(void*)_T8D4;yyval=Cyc_YY59(_T8D6);goto _LL0;case 213: _T8D8=yyr;{struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8D8,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));
# 2083 "parse.y"
_T1870->tag=4;_T8D7=(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8D9=(void*)_T8D7;yyval=Cyc_YY59(_T8D9);goto _LL0;case 214: _T8DB=yyr;{struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8DB,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));
# 2084 "parse.y"
_T1870->tag=7;_T8DA=(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8DC=(void*)_T8DA;yyval=Cyc_YY59(_T8DC);goto _LL0;case 215: _T8DE=yyr;{struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8DE,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2085 "parse.y"
_T1870->tag=5;_T8DD=(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8DF=(void*)_T8DD;yyval=Cyc_YY59(_T8DF);goto _LL0;case 216: _T8E1=yyr;{struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8E1,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2086 "parse.y"
_T1870->tag=6;_T8E0=(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8E2=(void*)_T8E0;yyval=Cyc_YY59(_T8E2);goto _LL0;case 217: _T8E4=yyr;{struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8E4,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));
# 2087 "parse.y"
_T1870->tag=8;_T8E3=(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8E5=(void*)_T8E3;yyval=Cyc_YY59(_T8E5);goto _LL0;case 218: _T8E7=yyr;{struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8E7,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));
# 2088 "parse.y"
_T1870->tag=9;_T8E6=(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8E8=(void*)_T8E6;yyval=Cyc_YY59(_T8E8);goto _LL0;case 219: _T8EA=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8EA,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T1870->tag=10;_T8EB=yyyvsp;_T8EC=& _T8EB[2].v;_T8ED=(union Cyc_YYSTYPE*)_T8EC;
# 2089 "parse.y"
_T1870->f1=Cyc_yyget_YY58(_T8ED);_T8E9=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8EE=(void*)_T8E9;yyval=Cyc_YY59(_T8EE);goto _LL0;case 220: _T8EF=yyyvsp;_T8F0=_T8EF[0];_T8F1=_T8F0.l;_T8F2=_T8F1.first_line;_T8F3=
# 2090 "parse.y"
Cyc_Position_loc_to_seg(_T8F2);_T8F4=yyyvsp;_T8F5=& _T8F4[0].v;_T8F6=(union Cyc_YYSTYPE*)_T8F5;_T8F7=Cyc_yyget_String_tok(_T8F6);{void*aq=Cyc_Parse_id2aqual(_T8F3,_T8F7);_T8F9=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T1870=_region_malloc(_T8F9,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T1870->tag=10;_T1870->f1=aq;_T8F8=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T1870;}_T8FA=(void*)_T8F8;yyval=Cyc_YY59(_T8FA);goto _LL0;}case 221: _T8FB=yyyvsp;_T8FC=& _T8FB[0].v;_T8FD=(union Cyc_YYSTYPE*)_T8FC;_T8FE=
# 2094 "parse.y"
Cyc_yyget_YY58(_T8FD);yyval=Cyc_YY58(_T8FE);goto _LL0;case 222: _T8FF=yyyvsp;_T900=& _T8FF[0].v;_T901=(union Cyc_YYSTYPE*)_T900;_T902=
# 2100 "parse.y"
Cyc_yyget_YY45(_T901);_T903=Cyc_Absyn_al_qual_type;_T904=Cyc_Absyn_aqual_var_type(_T902,_T903);yyval=Cyc_YY58(_T904);goto _LL0;case 223: _T905=yyyvsp;_T906=& _T905[2].v;_T907=(union Cyc_YYSTYPE*)_T906;_T908=
# 2104 "parse.y"
Cyc_yyget_YY45(_T907);_T909=Cyc_Absyn_aqualsof_type(_T908);_T90A=Cyc_Absyn_al_qual_type;_T90B=Cyc_Absyn_aqual_var_type(_T909,_T90A);yyval=Cyc_YY58(_T90B);goto _LL0;case 224: goto _LL0;case 225: _T90C=yyyvsp;_T90D=_T90C[0];_T90E=_T90D.l;_T90F=_T90E.first_line;_T910=
# 2110 "parse.y"
Cyc_Position_loc_to_seg(_T90F);_T911=yyyvsp;_T912=& _T911[0].v;_T913=(union Cyc_YYSTYPE*)_T912;_T914=Cyc_yyget_String_tok(_T913);_T915=Cyc_Parse_id2aqual(_T910,_T914);yyval=Cyc_YY58(_T915);goto _LL0;case 226:{struct _tuple22*_T1870=_cycalloc(sizeof(struct _tuple22));_T917=yyyvsp;_T918=_T917[0];_T919=_T918.l;_T91A=_T919.first_line;
# 2116 "parse.y"
_T1870->f0=Cyc_Position_loc_to_seg(_T91A);_T1870->f1=Cyc_Absyn_true_type;_T91B=Cyc_Parse_parsing_tempest;if(!_T91B)goto _TL2B1;_T1870->f2=Cyc_Absyn_fat_bound_type;goto _TL2B2;_TL2B1: _T91C=yyyvsp;_T91D=_T91C[0];_T91E=_T91D.l;_T91F=_T91E.first_line;_T920=Cyc_Position_loc_to_seg(_T91F);_T921=Cyc_Position_string_of_segment(_T920);_T922=yyyvsp;_T923=& _T922[1].v;_T924=(union Cyc_YYSTYPE*)_T923;_T925=Cyc_yyget_YY2(_T924);_T1870->f2=Cyc_Parse_assign_cvar_pos(_T921,0,_T925);_TL2B2: _T916=(struct _tuple22*)_T1870;}yyval=Cyc_YY1(_T916);goto _LL0;case 227:{struct _tuple22*_T1870=_cycalloc(sizeof(struct _tuple22));_T927=yyyvsp;_T928=_T927[0];_T929=_T928.l;_T92A=_T929.first_line;
# 2117 "parse.y"
_T1870->f0=Cyc_Position_loc_to_seg(_T92A);_T1870->f1=Cyc_Absyn_false_type;_T92B=yyyvsp;_T92C=_T92B[0];_T92D=_T92C.l;_T92E=_T92D.first_line;_T92F=Cyc_Position_loc_to_seg(_T92E);_T930=Cyc_Position_string_of_segment(_T92F);_T931=yyyvsp;_T932=& _T931[1].v;_T933=(union Cyc_YYSTYPE*)_T932;_T934=Cyc_yyget_YY2(_T933);_T1870->f2=Cyc_Parse_assign_cvar_pos(_T930,0,_T934);_T926=(struct _tuple22*)_T1870;}yyval=Cyc_YY1(_T926);goto _LL0;case 228: _T936=Cyc_Flags_override_fat;
# 2118 "parse.y"
if(!_T936)goto _TL2B3;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2B3;_T937=Cyc_Flags_interproc;if(!_T937)goto _TL2B3;{struct _tuple22*_T1870=_cycalloc(sizeof(struct _tuple22));_T939=yyyvsp;_T93A=_T939[0];_T93B=_T93A.l;_T93C=_T93B.first_line;
_T1870->f0=Cyc_Position_loc_to_seg(_T93C);_T1870->f1=Cyc_Absyn_true_type;_T93D=yyyvsp;_T93E=_T93D[0];_T93F=_T93E.l;_T940=_T93F.first_line;_T941=Cyc_Position_loc_to_seg(_T940);_T942=Cyc_Position_string_of_segment(_T941);_T943=& Cyc_Kinds_ptrbko;_T944=(struct Cyc_Core_Opt*)_T943;_T945=Cyc_Absyn_cvar_type(_T944);_T1870->f2=Cyc_Parse_assign_cvar_pos(_T942,1,_T945);_T938=(struct _tuple22*)_T1870;}_T935=_T938;goto _TL2B4;_TL2B3:{struct _tuple22*_T1870=_cycalloc(sizeof(struct _tuple22));_T947=yyyvsp;_T948=_T947[0];_T949=_T948.l;_T94A=_T949.first_line;
_T1870->f0=Cyc_Position_loc_to_seg(_T94A);_T1870->f1=Cyc_Absyn_true_type;_T1870->f2=Cyc_Absyn_fat_bound_type;_T946=(struct _tuple22*)_T1870;}_T935=_T946;_TL2B4:
# 2118
 yyval=Cyc_YY1(_T935);goto _LL0;case 229: _T94C=Cyc_Flags_interproc;
# 2123
if(!_T94C)goto _TL2B5;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2B5;_T94D=& Cyc_Kinds_ptrbko;_T94E=(struct Cyc_Core_Opt*)_T94D;_T94B=Cyc_Absyn_cvar_type(_T94E);goto _TL2B6;_TL2B5: _T94B=Cyc_Absyn_bounds_one();_TL2B6: yyval=Cyc_YY2(_T94B);goto _LL0;case 230: _T94F=yyyvsp;_T950=& _T94F[1].v;_T951=(union Cyc_YYSTYPE*)_T950;_T952=
# 2124 "parse.y"
Cyc_yyget_Exp_tok(_T951);_T953=Cyc_Absyn_thin_bounds_exp(_T952);yyval=Cyc_YY2(_T953);goto _LL0;case 231: _T954=Cyc_Flags_resolve;
# 2127 "parse.y"
if(_T954)goto _TL2B7;else{goto _TL2B9;}
_TL2B9:{int(*_T1870)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T955=_T1870;}_T956=_tag_fat("Type variable not permitted in pointer bound",sizeof(char),45U);_T955(0U,_T956);goto _TL2B8;_TL2B7: _TL2B8: _T957=yyyvsp;_T958=& _T957[1].v;_T959=(union Cyc_YYSTYPE*)_T958;_T95A=
Cyc_yyget_String_tok(_T959);_T95B=Cyc_Parse_typevar2cvar(_T95A);yyval=Cyc_YY2(_T95B);goto _LL0;case 232: _T95C=
# 2134 "parse.y"
Cyc_Tcutil_any_bool(0);yyval=Cyc_YY54(_T95C);goto _LL0;case 233:
# 2135 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_true_type);goto _LL0;case 234:
# 2136 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_false_type);goto _LL0;case 235:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T95E=yyyvsp;_T95F=& _T95E[0].v;_T960=(union Cyc_YYSTYPE*)_T95F;
# 2140 "parse.y"
_T1870->hd=Cyc_yyget_YY45(_T960);_T1870->tl=0;_T95D=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY41(_T95D);goto _LL0;case 236:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T962=yyyvsp;_T963=& _T962[0].v;_T964=(union Cyc_YYSTYPE*)_T963;
# 2141 "parse.y"
_T1870->hd=Cyc_yyget_YY45(_T964);_T965=yyyvsp;_T966=& _T965[2].v;_T967=(union Cyc_YYSTYPE*)_T966;_T1870->tl=Cyc_yyget_YY41(_T967);_T961=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY41(_T961);goto _LL0;case 237: _T968=& Cyc_Kinds_eko;_T969=(struct Cyc_Core_Opt*)_T968;_T96A=
# 2145 "parse.y"
Cyc_Absyn_new_evar(_T969,0);yyval=Cyc_YY45(_T96A);goto _LL0;case 238: _T96B=yyyvsp;_T96C=& _T96B[0].v;_T96D=(union Cyc_YYSTYPE*)_T96C;{
# 2146 "parse.y"
struct Cyc_List_List*es=Cyc_yyget_YY41(_T96D);_T96E=Cyc_List_length(es);if(_T96E!=1)goto _TL2BA;_T96F=_check_null(es);_T970=_T96F->hd;yyval=Cyc_YY45(_T970);goto _TL2BB;_TL2BA: _T971=yyyvsp;_T972=& _T971[0].v;_T973=(union Cyc_YYSTYPE*)_T972;_T974=Cyc_yyget_YY41(_T973);_T975=Cyc_Absyn_join_eff(_T974);yyval=Cyc_YY45(_T975);_TL2BB: goto _LL0;}case 239: _T976=& Cyc_Kinds_eko;_T977=(struct Cyc_Core_Opt*)_T976;_T978=
# 2147 "parse.y"
Cyc_Absyn_new_evar(_T977,0);yyval=Cyc_YY45(_T978);goto _LL0;case 240: _T979=yyvs;_T97A=yyvsp_offset + 1;_T97B=_check_fat_subscript(_T979,sizeof(struct Cyc_Yystacktype),_T97A);_T97C=(struct Cyc_Yystacktype*)_T97B;_T97D=*_T97C;_T97E=_T97D.l;_T97F=_T97E.first_line;_T980=
# 2158 "parse.y"
Cyc_Position_loc_to_seg(_T97F);_T981=Cyc_Absyn_empty_tqual(_T980);yyval=Cyc_YY24(_T981);goto _LL0;case 241: _T982=yyyvsp;_T983=& _T982[0].v;_T984=(union Cyc_YYSTYPE*)_T983;_T985=
# 2159 "parse.y"
Cyc_yyget_YY24(_T984);_T986=yyyvsp;_T987=& _T986[1].v;_T988=(union Cyc_YYSTYPE*)_T987;_T989=Cyc_yyget_YY24(_T988);_T98A=Cyc_Absyn_combine_tqual(_T985,_T989);yyval=Cyc_YY24(_T98A);goto _LL0;case 242: _T98B=yyyvsp;_T98C=& _T98B[1].v;_T98D=(union Cyc_YYSTYPE*)_T98C;{
# 2164 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T98D);_T98F=ec_qb;_T990=(unsigned)_T98F;
if(!_T990)goto _TL2BC;_T991=ec_qb;_T98E=*_T991;goto _TL2BD;_TL2BC:{struct _tuple28 _T1870;_T1870.f0=0;_T1870.f1=0;_T992=_T1870;}_T98E=_T992;_TL2BD: {struct _tuple28 _T1870=_T98E;struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_List_List*ec=_T1872;struct Cyc_List_List*qb=_T1871;{struct _tuple27*_T1873=_cycalloc(sizeof(struct _tuple27));
_T1873->f0=0;_T1873->f1=0;_T1873->f2=0;_T994=yyyvsp;_T995=& _T994[0].v;_T996=(union Cyc_YYSTYPE*)_T995;_T1873->f3=Cyc_yyget_YY50(_T996);_T1873->f4=ec;_T1873->f5=qb;_T993=(struct _tuple27*)_T1873;}yyval=Cyc_YY40(_T993);goto _LL0;}}}case 243: _T997=yyyvsp;_T998=& _T997[2].v;_T999=(union Cyc_YYSTYPE*)_T998;{
# 2168 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T999);_T99B=ec_qb;_T99C=(unsigned)_T99B;
if(!_T99C)goto _TL2BE;_T99D=ec_qb;_T99A=*_T99D;goto _TL2BF;_TL2BE:{struct _tuple28 _T1870;_T1870.f0=0;_T1870.f1=0;_T99E=_T1870;}_T99A=_T99E;_TL2BF: {struct _tuple28 _T1870=_T99A;struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_List_List*ec=_T1872;struct Cyc_List_List*qb=_T1871;{struct _tuple27*_T1873=_cycalloc(sizeof(struct _tuple27));_T9A0=yyyvsp;_T9A1=& _T9A0[0].v;_T9A2=(union Cyc_YYSTYPE*)_T9A1;_T9A3=
Cyc_yyget_YY39(_T9A2);_T1873->f0=Cyc_List_imp_rev(_T9A3);_T1873->f1=0;_T1873->f2=0;_T9A4=yyyvsp;_T9A5=& _T9A4[1].v;_T9A6=(union Cyc_YYSTYPE*)_T9A5;_T1873->f3=Cyc_yyget_YY50(_T9A6);_T1873->f4=ec;_T1873->f5=qb;_T99F=(struct _tuple27*)_T1873;}yyval=Cyc_YY40(_T99F);goto _LL0;}}}case 244: _T9A7=yyyvsp;_T9A8=& _T9A7[4].v;_T9A9=(union Cyc_YYSTYPE*)_T9A8;{
# 2172 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9A9);_T9AB=ec_qb;_T9AC=(unsigned)_T9AB;
if(!_T9AC)goto _TL2C0;_T9AD=ec_qb;_T9AA=*_T9AD;goto _TL2C1;_TL2C0:{struct _tuple28 _T1870;_T1870.f0=0;_T1870.f1=0;_T9AE=_T1870;}_T9AA=_T9AE;_TL2C1: {struct _tuple28 _T1870=_T9AA;struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_List_List*ec=_T1872;struct Cyc_List_List*qb=_T1871;{struct _tuple27*_T1873=_cycalloc(sizeof(struct _tuple27));_T9B0=yyyvsp;_T9B1=& _T9B0[0].v;_T9B2=(union Cyc_YYSTYPE*)_T9B1;_T9B3=
Cyc_yyget_YY39(_T9B2);_T1873->f0=Cyc_List_imp_rev(_T9B3);_T1873->f1=1;_T1873->f2=0;_T9B4=yyyvsp;_T9B5=& _T9B4[3].v;_T9B6=(union Cyc_YYSTYPE*)_T9B5;_T1873->f3=Cyc_yyget_YY50(_T9B6);_T1873->f4=ec;_T1873->f5=qb;_T9AF=(struct _tuple27*)_T1873;}yyval=Cyc_YY40(_T9AF);goto _LL0;}}}case 245: _T9B7=yyyvsp;_T9B8=& _T9B7[2].v;_T9B9=(union Cyc_YYSTYPE*)_T9B8;{
# 2177
struct _tuple8*_T1870=Cyc_yyget_YY38(_T9B9);void*_T1871;struct Cyc_Absyn_Tqual _T1872;struct _fat_ptr*_T1873;{struct _tuple8 _T1874=*_T1870;_T1873=_T1874.f0;_T1872=_T1874.f1;_T1871=_T1874.f2;}{struct _fat_ptr*n=_T1873;struct Cyc_Absyn_Tqual tq=_T1872;void*t=_T1871;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9BA=v;_T9BA->name=n;_T9BB=v;_T9BB->tq=tq;_T9BC=v;_T9BC->type=t;_T9BD=v;_T9BE=yyyvsp;_T9BF=& _T9BE[1].v;_T9C0=(union Cyc_YYSTYPE*)_T9BF;_T9BD->inject=Cyc_yyget_YY32(_T9C0);_T9C1=yyyvsp;_T9C2=& _T9C1[4].v;_T9C3=(union Cyc_YYSTYPE*)_T9C2;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9C3);_T9C5=ec_qb;_T9C6=(unsigned)_T9C5;
if(!_T9C6)goto _TL2C2;_T9C7=ec_qb;_T9C4=*_T9C7;goto _TL2C3;_TL2C2:{struct _tuple28 _T1874;_T1874.f0=0;_T1874.f1=0;_T9C8=_T1874;}_T9C4=_T9C8;_TL2C3: {struct _tuple28 _T1874=_T9C4;struct Cyc_List_List*_T1875;struct Cyc_List_List*_T1876;_T1876=_T1874.f0;_T1875=_T1874.f1;{struct Cyc_List_List*ec=_T1876;struct Cyc_List_List*qb=_T1875;{struct _tuple27*_T1877=_cycalloc(sizeof(struct _tuple27));
_T1877->f0=0;_T1877->f1=0;_T1877->f2=v;_T9CA=yyyvsp;_T9CB=& _T9CA[3].v;_T9CC=(union Cyc_YYSTYPE*)_T9CB;_T1877->f3=Cyc_yyget_YY50(_T9CC);_T1877->f4=ec;_T1877->f5=qb;_T9C9=(struct _tuple27*)_T1877;}yyval=Cyc_YY40(_T9C9);goto _LL0;}}}}}case 246: _T9CD=yyyvsp;_T9CE=& _T9CD[4].v;_T9CF=(union Cyc_YYSTYPE*)_T9CE;{
# 2185
struct _tuple8*_T1870=Cyc_yyget_YY38(_T9CF);void*_T1871;struct Cyc_Absyn_Tqual _T1872;struct _fat_ptr*_T1873;{struct _tuple8 _T1874=*_T1870;_T1873=_T1874.f0;_T1872=_T1874.f1;_T1871=_T1874.f2;}{struct _fat_ptr*n=_T1873;struct Cyc_Absyn_Tqual tq=_T1872;void*t=_T1871;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9D0=v;_T9D0->name=n;_T9D1=v;_T9D1->tq=tq;_T9D2=v;_T9D2->type=t;_T9D3=v;_T9D4=yyyvsp;_T9D5=& _T9D4[3].v;_T9D6=(union Cyc_YYSTYPE*)_T9D5;_T9D3->inject=Cyc_yyget_YY32(_T9D6);_T9D7=yyyvsp;_T9D8=& _T9D7[6].v;_T9D9=(union Cyc_YYSTYPE*)_T9D8;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9D9);_T9DB=ec_qb;_T9DC=(unsigned)_T9DB;
if(!_T9DC)goto _TL2C4;_T9DD=ec_qb;_T9DA=*_T9DD;goto _TL2C5;_TL2C4:{struct _tuple28 _T1874;_T1874.f0=0;_T1874.f1=0;_T9DE=_T1874;}_T9DA=_T9DE;_TL2C5: {struct _tuple28 _T1874=_T9DA;struct Cyc_List_List*_T1875;struct Cyc_List_List*_T1876;_T1876=_T1874.f0;_T1875=_T1874.f1;{struct Cyc_List_List*ec=_T1876;struct Cyc_List_List*qb=_T1875;{struct _tuple27*_T1877=_cycalloc(sizeof(struct _tuple27));_T9E0=yyyvsp;_T9E1=& _T9E0[0].v;_T9E2=(union Cyc_YYSTYPE*)_T9E1;_T9E3=
Cyc_yyget_YY39(_T9E2);_T1877->f0=Cyc_List_imp_rev(_T9E3);_T1877->f1=0;_T1877->f2=v;_T9E4=yyyvsp;_T9E5=& _T9E4[5].v;_T9E6=(union Cyc_YYSTYPE*)_T9E5;_T1877->f3=Cyc_yyget_YY50(_T9E6);_T1877->f4=ec;_T1877->f5=qb;_T9DF=(struct _tuple27*)_T1877;}yyval=Cyc_YY40(_T9DF);goto _LL0;}}}}}case 247:
# 2195 "parse.y"
 yyval=Cyc_YY50(0);goto _LL0;case 248: _T9E7=yyyvsp;_T9E8=_T9E7[0];_T9E9=_T9E8.l;_T9EA=_T9E9.first_line;_T9EB=
# 2196 "parse.y"
Cyc_Position_loc_to_seg(_T9EA);_T9EC=yyyvsp;_T9ED=& _T9EC[0].v;_T9EE=(union Cyc_YYSTYPE*)_T9ED;_T9EF=Cyc_yyget_String_tok(_T9EE);_T9F0=Cyc_Parse_id2aqual(_T9EB,_T9EF);yyval=Cyc_YY50(_T9F0);goto _LL0;case 249: _T9F1=yyyvsp;_T9F2=& _T9F1[0].v;_T9F3=(union Cyc_YYSTYPE*)_T9F2;_T9F4=
# 2199
Cyc_yyget_String_tok(_T9F3);{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_T1870->tag=1;_T1870->f1=0;_T9F5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T1870;}_T9F6=(void*)_T9F5;_T9F7=yyyvsp;_T9F8=& _T9F7[1].v;_T9F9=(union Cyc_YYSTYPE*)_T9F8;_T9FA=Cyc_yyget_YY50(_T9F9);_T9FB=yyyvsp;_T9FC=_T9FB[0];_T9FD=_T9FC.l;_T9FE=_T9FD.first_line;_T9FF=Cyc_Position_loc_to_seg(_T9FE);_TA00=Cyc_Parse_id2type(_T9F4,_T9F6,_T9FA,_T9FF);yyval=Cyc_YY45(_TA00);goto _LL0;case 250: _TA01=yyyvsp;_TA02=& _TA01[0].v;_TA03=(union Cyc_YYSTYPE*)_TA02;_TA04=
# 2200 "parse.y"
Cyc_yyget_String_tok(_TA03);_TA05=yyyvsp;_TA06=& _TA05[2].v;_TA07=(union Cyc_YYSTYPE*)_TA06;_TA08=Cyc_yyget_YY44(_TA07);_TA09=Cyc_Kinds_kind_to_bound(_TA08);_TA0A=yyyvsp;_TA0B=& _TA0A[3].v;_TA0C=(union Cyc_YYSTYPE*)_TA0B;_TA0D=Cyc_yyget_YY50(_TA0C);_TA0E=yyyvsp;_TA0F=_TA0E[0];_TA10=_TA0F.l;_TA11=_TA10.first_line;_TA12=Cyc_Position_loc_to_seg(_TA11);_TA13=Cyc_Parse_id2type(_TA04,_TA09,_TA0D,_TA12);yyval=Cyc_YY45(_TA13);goto _LL0;case 251:
# 2203
 yyval=Cyc_YY50(0);goto _LL0;case 252: _TA14=yyyvsp;_TA15=& _TA14[1].v;_TA16=(union Cyc_YYSTYPE*)_TA15;_TA17=
# 2204 "parse.y"
Cyc_yyget_YY41(_TA16);_TA18=Cyc_Absyn_join_eff(_TA17);yyval=Cyc_YY50(_TA18);goto _LL0;case 253:
# 2208 "parse.y"
 yyval=Cyc_YY51(0);goto _LL0;case 254: _TA19=yyyvsp;_TA1A=_TA19[1];
# 2209 "parse.y"
yyval=_TA1A.v;goto _LL0;case 255:{struct _tuple28*_T1870=_cycalloc(sizeof(struct _tuple28));{void*_T1871[1];_TA1D=yyyvsp;_TA1E=& _TA1D[0].v;_TA1F=(union Cyc_YYSTYPE*)_TA1E;_T1871[0]=
# 2214 "parse.y"
Cyc_yyget_YY52(_TA1F);_TA20=_tag_fat(_T1871,sizeof(void*),1);_TA1C=Cyc_List_list(_TA20);}_T1870->f0=_TA1C;_T1870->f1=0;_TA1B=(struct _tuple28*)_T1870;}yyval=Cyc_YY51(_TA1B);goto _LL0;case 256:{struct _tuple28*_T1870=_cycalloc(sizeof(struct _tuple28));
# 2216 "parse.y"
_T1870->f0=0;{struct _tuple29*_T1871[1];_TA23=yyyvsp;_TA24=& _TA23[0].v;_TA25=(union Cyc_YYSTYPE*)_TA24;_T1871[0]=Cyc_yyget_YY53(_TA25);_TA26=_tag_fat(_T1871,sizeof(struct _tuple29*),1);_TA22=Cyc_List_list(_TA26);}_T1870->f1=_TA22;_TA21=(struct _tuple28*)_T1870;}yyval=Cyc_YY51(_TA21);goto _LL0;case 257: _TA27=yyyvsp;_TA28=& _TA27[2].v;_TA29=(union Cyc_YYSTYPE*)_TA28;{
# 2219 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(_TA29);_TA2A=
_check_null(rest);{struct _tuple28 _T1870=*_TA2A;struct Cyc_List_List*_T1871;_T1871=_T1870.f0;{struct Cyc_List_List*rpo=_T1871;_TA2B=rest;{struct Cyc_List_List*_T1872=_cycalloc(sizeof(struct Cyc_List_List));_TA2D=yyyvsp;_TA2E=& _TA2D[0].v;_TA2F=(union Cyc_YYSTYPE*)_TA2E;
_T1872->hd=Cyc_yyget_YY52(_TA2F);_T1872->tl=rpo;_TA2C=(struct Cyc_List_List*)_T1872;}(*_TA2B).f0=_TA2C;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 258: _TA30=yyyvsp;_TA31=& _TA30[2].v;_TA32=(union Cyc_YYSTYPE*)_TA31;{
# 2226 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(_TA32);_TA33=
_check_null(rest);{struct _tuple28 _T1870=*_TA33;struct Cyc_List_List*_T1871;_T1871=_T1870.f1;{struct Cyc_List_List*qb=_T1871;_TA34=rest;{struct Cyc_List_List*_T1872=_cycalloc(sizeof(struct Cyc_List_List));_TA36=yyyvsp;_TA37=& _TA36[0].v;_TA38=(union Cyc_YYSTYPE*)_TA37;
_T1872->hd=Cyc_yyget_YY53(_TA38);_T1872->tl=qb;_TA35=(struct Cyc_List_List*)_T1872;}(*_TA34).f1=_TA35;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 259:  {
# 2236 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_TA39=kb;_TA39->tag=2;_TA3A=kb;_TA3A->f1=0;_TA3B=kb;_TA3C=& Cyc_Kinds_ek;_TA3B->f2=(struct Cyc_Absyn_Kind*)_TA3C;_TA3D=yyyvsp;_TA3E=& _TA3D[2].v;_TA3F=(union Cyc_YYSTYPE*)_TA3E;_TA40=
Cyc_yyget_String_tok(_TA3F);_TA41=kb;_TA42=(void*)_TA41;_TA43=yyyvsp;_TA44=_TA43[2];_TA45=_TA44.l;_TA46=_TA45.first_line;_TA47=Cyc_Position_loc_to_seg(_TA46);{void*t=Cyc_Parse_id2type(_TA40,_TA42,0,_TA47);{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct));_T1870->tag=2;_TA49=yyyvsp;_TA4A=& _TA49[0].v;_TA4B=(union Cyc_YYSTYPE*)_TA4A;_TA4C=
Cyc_yyget_YY41(_TA4B);_T1870->f1=Cyc_Parse_effect_from_atomic(_TA4C);_T1870->f2=t;_TA48=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T1870;}_TA4D=(void*)_TA48;yyval=Cyc_YY52(_TA4D);goto _LL0;}}case 260:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_T1870->tag=1;_TA4F=yyyvsp;_TA50=& _TA4F[0].v;_TA51=(union Cyc_YYSTYPE*)_TA50;_TA52=
# 2242 "parse.y"
Cyc_yyget_YY41(_TA51);_T1870->f1=Cyc_Parse_effect_from_atomic(_TA52);_TA53=yyyvsp;_TA54=& _TA53[2].v;_TA55=(union Cyc_YYSTYPE*)_TA54;_TA56=
Cyc_yyget_YY41(_TA55);_T1870->f2=Cyc_Parse_effect_from_atomic(_TA56);_TA4E=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T1870;}_TA57=(void*)_TA4E;
# 2242
yyval=Cyc_YY52(_TA57);goto _LL0;case 261: _TA58=yyyvsp;_TA59=_TA58[0];_TA5A=_TA59.l;_TA5B=_TA5A.first_line;_TA5C=
# 2247 "parse.y"
Cyc_Position_loc_to_seg(_TA5B);_TA5D=yyyvsp;_TA5E=& _TA5D[0].v;_TA5F=(union Cyc_YYSTYPE*)_TA5E;_TA60=Cyc_yyget_String_tok(_TA5F);Cyc_Parse_check_single_constraint(_TA5C,_TA60);{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_T1870->tag=0;_TA62=yyyvsp;_TA63=& _TA62[2].v;_TA64=(union Cyc_YYSTYPE*)_TA63;_TA65=
Cyc_yyget_YY41(_TA64);_T1870->f1=Cyc_Parse_effect_from_atomic(_TA65);_TA61=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T1870;}_TA66=(void*)_TA61;yyval=Cyc_YY52(_TA66);goto _LL0;case 262:{struct _tuple29*_T1870=_cycalloc(sizeof(struct _tuple29));_TA68=yyyvsp;_TA69=& _TA68[2].v;_TA6A=(union Cyc_YYSTYPE*)_TA69;
# 2255 "parse.y"
_T1870->f0=Cyc_yyget_YY45(_TA6A);_TA6B=yyyvsp;_TA6C=& _TA6B[0].v;_TA6D=(union Cyc_YYSTYPE*)_TA6C;_T1870->f1=Cyc_yyget_YY45(_TA6D);_TA67=(struct _tuple29*)_T1870;}yyval=Cyc_YY53(_TA67);goto _LL0;case 263:
# 2260 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_al_qual_type);goto _LL0;case 264:
# 2261 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_un_qual_type);goto _LL0;case 265:
# 2262 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rc_qual_type);goto _LL0;case 266:
# 2263 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rtd_qual_type);goto _LL0;case 267: _TA6E=yyyvsp;_TA6F=_TA6E[0];_TA70=_TA6F.l;_TA71=_TA70.first_line;_TA72=
# 2264 "parse.y"
Cyc_Position_loc_to_seg(_TA71);_TA73=yyyvsp;_TA74=& _TA73[0].v;_TA75=(union Cyc_YYSTYPE*)_TA74;_TA76=Cyc_yyget_String_tok(_TA75);_TA77=Cyc_Parse_id2aqual(_TA72,_TA76);yyval=Cyc_YY58(_TA77);goto _LL0;case 268: _TA78=yyyvsp;_TA79=& _TA78[0].v;_TA7A=(union Cyc_YYSTYPE*)_TA79;_TA7B=
# 2269 "parse.y"
Cyc_yyget_YY58(_TA7A);yyval=Cyc_YY45(_TA7B);goto _LL0;case 269: _TA7C=yyyvsp;_TA7D=& _TA7C[2].v;_TA7E=(union Cyc_YYSTYPE*)_TA7D;_TA7F=
# 2272 "parse.y"
Cyc_yyget_YY45(_TA7E);_TA80=Cyc_Absyn_aqualsof_type(_TA7F);_TA81=Cyc_Absyn_al_qual_type;_TA82=Cyc_Absyn_aqual_var_type(_TA80,_TA81);yyval=Cyc_YY45(_TA82);goto _LL0;case 270:  {
# 2279 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_TA83=kb;_TA83->tag=0;_TA84=kb;_TA85=& Cyc_Kinds_aqk;_TA84->f1=(struct Cyc_Absyn_Kind*)_TA85;_TA86=yyyvsp;_TA87=& _TA86[0].v;_TA88=(union Cyc_YYSTYPE*)_TA87;_TA89=
Cyc_yyget_String_tok(_TA88);_TA8A=kb;_TA8B=(void*)_TA8A;_TA8C=yyyvsp;_TA8D=_TA8C[0];_TA8E=_TA8D.l;_TA8F=_TA8E.first_line;_TA90=Cyc_Position_loc_to_seg(_TA8F);_TA91=Cyc_Parse_id2type(_TA89,_TA8B,0,_TA90);yyval=Cyc_YY45(_TA91);goto _LL0;}case 271: _TA92=yyyvsp;_TA93=& _TA92[2].v;_TA94=(union Cyc_YYSTYPE*)_TA93;_TA95=
# 2284 "parse.y"
Cyc_yyget_YY45(_TA94);_TA96=Cyc_Absyn_aqualsof_type(_TA95);yyval=Cyc_YY45(_TA96);goto _LL0;case 272:
# 2313 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 273: _TA97=yyyvsp;_TA98=& _TA97[0].v;_TA99=(union Cyc_YYSTYPE*)_TA98;_TA9A=
# 2315 "parse.y"
Cyc_yyget_String_tok(_TA99);_TA9B=_tag_fat("inject",sizeof(char),7U);_TA9C=Cyc_zstrcmp(_TA9A,_TA9B);if(_TA9C==0)goto _TL2C6;{struct Cyc_Warn_String_Warn_Warg_struct _T1870;_T1870.tag=0;
_T1870.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_TA9D=_T1870;}{struct Cyc_Warn_String_Warn_Warg_struct _T1870=_TA9D;void*_T1871[1];_T1871[0]=& _T1870;_TA9E=yyyvsp;_TA9F=_TA9E[0];_TAA0=_TA9F.l;_TAA1=_TAA0.first_line;_TAA2=Cyc_Position_loc_to_seg(_TAA1);_TAA3=_tag_fat(_T1871,sizeof(void*),1);Cyc_Warn_err2(_TAA2,_TAA3);}goto _TL2C7;_TL2C6: _TL2C7:
 yyval=Cyc_YY32(1);goto _LL0;case 274: _TAA4=yyyvsp;_TAA5=_TAA4[0];
# 2322 "parse.y"
yyval=_TAA5.v;goto _LL0;case 275: _TAA6=yyyvsp;_TAA7=& _TAA6[0].v;_TAA8=(union Cyc_YYSTYPE*)_TAA7;_TAA9=
# 2323 "parse.y"
Cyc_yyget_YY41(_TAA8);_TAAA=yyyvsp;_TAAB=& _TAAA[2].v;_TAAC=(union Cyc_YYSTYPE*)_TAAB;_TAAD=Cyc_yyget_YY41(_TAAC);_TAAE=Cyc_List_imp_append(_TAA9,_TAAD);yyval=Cyc_YY41(_TAAE);goto _LL0;case 276:
# 2327 "parse.y"
 yyval=Cyc_YY41(0);goto _LL0;case 277: _TAAF=yyyvsp;_TAB0=_TAAF[1];
# 2328 "parse.y"
yyval=_TAB0.v;goto _LL0;case 278:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TAB2=yyyvsp;_TAB3=& _TAB2[2].v;_TAB4=(union Cyc_YYSTYPE*)_TAB3;_TAB5=
# 2329 "parse.y"
Cyc_yyget_YY45(_TAB4);_T1870->hd=Cyc_Absyn_regionsof_eff(_TAB5);_T1870->tl=0;_TAB1=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY41(_TAB1);goto _LL0;case 279: _TAB6=yyyvsp;_TAB7=& _TAB6[0].v;_TAB8=(union Cyc_YYSTYPE*)_TAB7;_TAB9=
# 2331 "parse.y"
Cyc_yyget_YY45(_TAB8);_TABA=& Cyc_Kinds_ek;_TABB=(struct Cyc_Absyn_Kind*)_TABA;Cyc_Parse_set_vartyp_kind(_TAB9,_TABB,0);{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TABD=yyyvsp;_TABE=& _TABD[0].v;_TABF=(union Cyc_YYSTYPE*)_TABE;
_T1870->hd=Cyc_yyget_YY45(_TABF);_T1870->tl=0;_TABC=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY41(_TABC);goto _LL0;case 280:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TAC1=yyyvsp;_TAC2=& _TAC1[0].v;_TAC3=(union Cyc_YYSTYPE*)_TAC2;_TAC4=
# 2339 "parse.y"
Cyc_yyget_YY38(_TAC3);_TAC5=yyyvsp;_TAC6=_TAC5[0];_TAC7=_TAC6.l;_TAC8=_TAC7.first_line;_TAC9=Cyc_Position_loc_to_seg(_TAC8);_T1870->hd=Cyc_Parse_type_name_to_type(_TAC4,_TAC9);_T1870->tl=0;_TAC0=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY41(_TAC0);goto _LL0;case 281:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TACB=yyyvsp;_TACC=& _TACB[0].v;_TACD=(union Cyc_YYSTYPE*)_TACC;_TACE=
# 2341 "parse.y"
Cyc_yyget_YY38(_TACD);_TACF=yyyvsp;_TAD0=_TACF[0];_TAD1=_TAD0.l;_TAD2=_TAD1.first_line;_TAD3=Cyc_Position_loc_to_seg(_TAD2);_T1870->hd=Cyc_Parse_type_name_to_type(_TACE,_TAD3);_TAD4=yyyvsp;_TAD5=& _TAD4[2].v;_TAD6=(union Cyc_YYSTYPE*)_TAD5;_T1870->tl=Cyc_yyget_YY41(_TAD6);_TACA=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY41(_TACA);goto _LL0;case 282:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TAD8=yyyvsp;_TAD9=& _TAD8[0].v;_TADA=(union Cyc_YYSTYPE*)_TAD9;
# 2346 "parse.y"
_T1870->hd=Cyc_yyget_YY38(_TADA);_T1870->tl=0;_TAD7=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY39(_TAD7);goto _LL0;case 283:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TADC=yyyvsp;_TADD=& _TADC[2].v;_TADE=(union Cyc_YYSTYPE*)_TADD;
# 2347 "parse.y"
_T1870->hd=Cyc_yyget_YY38(_TADE);_TADF=yyyvsp;_TAE0=& _TADF[0].v;_TAE1=(union Cyc_YYSTYPE*)_TAE0;_T1870->tl=Cyc_yyget_YY39(_TAE1);_TADB=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY39(_TADB);goto _LL0;case 284: _TAE2=yyyvsp;_TAE3=& _TAE2[0].v;_TAE4=(union Cyc_YYSTYPE*)_TAE3;{
# 2353 "parse.y"
struct _tuple26 _T1870=Cyc_yyget_YY36(_TAE4);struct Cyc_List_List*_T1871;struct Cyc_Parse_Type_specifier _T1872;struct Cyc_Absyn_Tqual _T1873;_T1873=_T1870.f0;_T1872=_T1870.f1;_T1871=_T1870.f2;{struct Cyc_Absyn_Tqual tq=_T1873;struct Cyc_Parse_Type_specifier tspecs=_T1872;struct Cyc_List_List*atts=_T1871;_TAE5=tq;_TAE6=_TAE5.loc;
if(_TAE6!=0U)goto _TL2C8;_TAE7=yyyvsp;_TAE8=_TAE7[0];_TAE9=_TAE8.l;_TAEA=_TAE9.first_line;tq.loc=Cyc_Position_loc_to_seg(_TAEA);goto _TL2C9;_TL2C8: _TL2C9: _TAEB=yyyvsp;_TAEC=& _TAEB[1].v;_TAED=(union Cyc_YYSTYPE*)_TAEC;{
struct Cyc_Parse_Declarator _T1874=Cyc_yyget_YY28(_TAED);struct Cyc_List_List*_T1875;unsigned _T1876;struct _tuple0*_T1877;_T1877=_T1874.id;_T1876=_T1874.varloc;_T1875=_T1874.tms;{struct _tuple0*qv=_T1877;unsigned varloc=_T1876;struct Cyc_List_List*tms=_T1875;_TAEE=tspecs;_TAEF=yyyvsp;_TAF0=_TAEF[0];_TAF1=_TAF0.l;_TAF2=_TAF1.first_line;_TAF3=
Cyc_Position_loc_to_seg(_TAF2);{void*t=Cyc_Parse_speclist2typ(_TAEE,_TAF3);
struct _tuple14 _T1878=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T1879;struct Cyc_List_List*_T187A;void*_T187B;struct Cyc_Absyn_Tqual _T187C;_T187C=_T1878.f0;_T187B=_T1878.f1;_T187A=_T1878.f2;_T1879=_T1878.f3;{struct Cyc_Absyn_Tqual tq2=_T187C;void*t2=_T187B;struct Cyc_List_List*tvs=_T187A;struct Cyc_List_List*atts2=_T1879;
if(tvs==0)goto _TL2CA;{struct Cyc_Warn_String_Warn_Warg_struct _T187D;_T187D.tag=0;
_T187D.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_TAF4=_T187D;}{struct Cyc_Warn_String_Warn_Warg_struct _T187D=_TAF4;void*_T187E[1];_T187E[0]=& _T187D;_TAF5=yyyvsp;_TAF6=_TAF5[1];_TAF7=_TAF6.l;_TAF8=_TAF7.first_line;_TAF9=Cyc_Position_loc_to_seg(_TAF8);_TAFA=_tag_fat(_T187E,sizeof(void*),1);Cyc_Warn_err2(_TAF9,_TAFA);}goto _TL2CB;_TL2CA: _TL2CB: _TAFB=
Cyc_Absyn_is_qvar_qualified(qv);if(!_TAFB)goto _TL2CC;{struct Cyc_Warn_String_Warn_Warg_struct _T187D;_T187D.tag=0;
_T187D.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_TAFC=_T187D;}{struct Cyc_Warn_String_Warn_Warg_struct _T187D=_TAFC;void*_T187E[1];_T187E[0]=& _T187D;_TAFD=yyyvsp;_TAFE=_TAFD[0];_TAFF=_TAFE.l;_TB00=_TAFF.first_line;_TB01=Cyc_Position_loc_to_seg(_TB00);_TB02=_tag_fat(_T187E,sizeof(void*),1);Cyc_Warn_err2(_TB01,_TB02);}goto _TL2CD;_TL2CC: _TL2CD: _TB03=qv;_TB04=*_TB03;{
struct _fat_ptr*idopt=_TB04.f1;
if(atts2==0)goto _TL2CE;{struct Cyc_Warn_String_Warn_Warg_struct _T187D;_T187D.tag=0;
_T187D.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_TB05=_T187D;}{struct Cyc_Warn_String_Warn_Warg_struct _T187D=_TB05;void*_T187E[1];_T187E[0]=& _T187D;_TB06=yyyvsp;_TB07=_TB06[0];_TB08=_TB07.l;_TB09=_TB08.first_line;_TB0A=Cyc_Position_loc_to_seg(_TB09);_TB0B=_tag_fat(_T187E,sizeof(void*),1);Cyc_Warn_warn2(_TB0A,_TB0B);}goto _TL2CF;_TL2CE: _TL2CF:{struct _tuple8*_T187D=_cycalloc(sizeof(struct _tuple8));
_T187D->f0=idopt;_T187D->f1=tq2;_T187D->f2=t2;_TB0C=(struct _tuple8*)_T187D;}yyval=Cyc_YY38(_TB0C);goto _LL0;}}}}}}}case 285: _TB0D=yyyvsp;_TB0E=& _TB0D[0].v;_TB0F=(union Cyc_YYSTYPE*)_TB0E;{
# 2368 "parse.y"
struct _tuple26 _T1870=Cyc_yyget_YY36(_TB0F);struct Cyc_List_List*_T1871;struct Cyc_Parse_Type_specifier _T1872;struct Cyc_Absyn_Tqual _T1873;_T1873=_T1870.f0;_T1872=_T1870.f1;_T1871=_T1870.f2;{struct Cyc_Absyn_Tqual tq=_T1873;struct Cyc_Parse_Type_specifier tspecs=_T1872;struct Cyc_List_List*atts=_T1871;_TB10=tq;_TB11=_TB10.loc;
if(_TB11!=0U)goto _TL2D0;_TB12=yyyvsp;_TB13=_TB12[0];_TB14=_TB13.l;_TB15=_TB14.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB15);goto _TL2D1;_TL2D0: _TL2D1: _TB16=tspecs;_TB17=yyyvsp;_TB18=_TB17[0];_TB19=_TB18.l;_TB1A=_TB19.first_line;_TB1B=
Cyc_Position_loc_to_seg(_TB1A);{void*t=Cyc_Parse_speclist2typ(_TB16,_TB1B);
if(atts==0)goto _TL2D2;{struct Cyc_Warn_String_Warn_Warg_struct _T1874;_T1874.tag=0;
_T1874.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB1C=_T1874;}{struct Cyc_Warn_String_Warn_Warg_struct _T1874=_TB1C;void*_T1875[1];_T1875[0]=& _T1874;_TB1D=yyyvsp;_TB1E=_TB1D[0];_TB1F=_TB1E.l;_TB20=_TB1F.first_line;_TB21=Cyc_Position_loc_to_seg(_TB20);_TB22=_tag_fat(_T1875,sizeof(void*),1);Cyc_Warn_warn2(_TB21,_TB22);}goto _TL2D3;_TL2D2: _TL2D3:{struct _tuple8*_T1874=_cycalloc(sizeof(struct _tuple8));
_T1874->f0=0;_T1874->f1=tq;_T1874->f2=t;_TB23=(struct _tuple8*)_T1874;}yyval=Cyc_YY38(_TB23);goto _LL0;}}}case 286: _TB24=yyyvsp;_TB25=& _TB24[0].v;_TB26=(union Cyc_YYSTYPE*)_TB25;{
# 2376 "parse.y"
struct _tuple26 _T1870=Cyc_yyget_YY36(_TB26);struct Cyc_List_List*_T1871;struct Cyc_Parse_Type_specifier _T1872;struct Cyc_Absyn_Tqual _T1873;_T1873=_T1870.f0;_T1872=_T1870.f1;_T1871=_T1870.f2;{struct Cyc_Absyn_Tqual tq=_T1873;struct Cyc_Parse_Type_specifier tspecs=_T1872;struct Cyc_List_List*atts=_T1871;_TB27=tq;_TB28=_TB27.loc;
if(_TB28!=0U)goto _TL2D4;_TB29=yyyvsp;_TB2A=_TB29[0];_TB2B=_TB2A.l;_TB2C=_TB2B.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB2C);goto _TL2D5;_TL2D4: _TL2D5: _TB2D=tspecs;_TB2E=yyyvsp;_TB2F=_TB2E[0];_TB30=_TB2F.l;_TB31=_TB30.first_line;_TB32=
Cyc_Position_loc_to_seg(_TB31);{void*t=Cyc_Parse_speclist2typ(_TB2D,_TB32);_TB33=yyyvsp;_TB34=& _TB33[1].v;_TB35=(union Cyc_YYSTYPE*)_TB34;_TB36=
Cyc_yyget_YY31(_TB35);{struct Cyc_List_List*tms=_TB36.tms;
struct _tuple14 _T1874=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T1875;struct Cyc_List_List*_T1876;void*_T1877;struct Cyc_Absyn_Tqual _T1878;_T1878=_T1874.f0;_T1877=_T1874.f1;_T1876=_T1874.f2;_T1875=_T1874.f3;{struct Cyc_Absyn_Tqual tq2=_T1878;void*t2=_T1877;struct Cyc_List_List*tvs=_T1876;struct Cyc_List_List*atts2=_T1875;
if(tvs==0)goto _TL2D6;{struct Cyc_Warn_String_Warn_Warg_struct _T1879;_T1879.tag=0;
# 2383
_T1879.f1=_tag_fat("bad type parameters on formal argument, ignoring",sizeof(char),49U);_TB37=_T1879;}{struct Cyc_Warn_String_Warn_Warg_struct _T1879=_TB37;void*_T187A[1];_T187A[0]=& _T1879;_TB38=yyyvsp;_TB39=_TB38[0];_TB3A=_TB39.l;_TB3B=_TB3A.first_line;_TB3C=
# 2382
Cyc_Position_loc_to_seg(_TB3B);_TB3D=_tag_fat(_T187A,sizeof(void*),1);Cyc_Warn_warn2(_TB3C,_TB3D);}goto _TL2D7;_TL2D6: _TL2D7:
# 2384
 if(atts2==0)goto _TL2D8;{struct Cyc_Warn_String_Warn_Warg_struct _T1879;_T1879.tag=0;
_T1879.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB3E=_T1879;}{struct Cyc_Warn_String_Warn_Warg_struct _T1879=_TB3E;void*_T187A[1];_T187A[0]=& _T1879;_TB3F=yyyvsp;_TB40=_TB3F[0];_TB41=_TB40.l;_TB42=_TB41.first_line;_TB43=Cyc_Position_loc_to_seg(_TB42);_TB44=_tag_fat(_T187A,sizeof(void*),1);Cyc_Warn_warn2(_TB43,_TB44);}goto _TL2D9;_TL2D8: _TL2D9:{struct _tuple8*_T1879=_cycalloc(sizeof(struct _tuple8));
_T1879->f0=0;_T1879->f1=tq2;_T1879->f2=t2;_TB45=(struct _tuple8*)_T1879;}yyval=Cyc_YY38(_TB45);goto _LL0;}}}}}case 287: _TB46=yyyvsp;_TB47=& _TB46[0].v;_TB48=(union Cyc_YYSTYPE*)_TB47;_TB49=
# 2391 "parse.y"
Cyc_yyget_YY37(_TB48);_TB4A=Cyc_List_imp_rev(_TB49);yyval=Cyc_YY37(_TB4A);goto _LL0;case 288:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_TB4D=yyyvsp;_TB4E=& _TB4D[0].v;_TB4F=(union Cyc_YYSTYPE*)_TB4E;
# 2394
*_T1871=Cyc_yyget_String_tok(_TB4F);_TB4C=(struct _fat_ptr*)_T1871;}_T1870->hd=_TB4C;_T1870->tl=0;_TB4B=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY37(_TB4B);goto _LL0;case 289:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_TB52=yyyvsp;_TB53=& _TB52[2].v;_TB54=(union Cyc_YYSTYPE*)_TB53;
# 2395 "parse.y"
*_T1871=Cyc_yyget_String_tok(_TB54);_TB51=(struct _fat_ptr*)_T1871;}_T1870->hd=_TB51;_TB55=yyyvsp;_TB56=& _TB55[0].v;_TB57=(union Cyc_YYSTYPE*)_TB56;_T1870->tl=Cyc_yyget_YY37(_TB57);_TB50=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY37(_TB50);goto _LL0;case 290: _TB58=yyyvsp;_TB59=_TB58[0];
# 2399 "parse.y"
yyval=_TB59.v;goto _LL0;case 291: _TB5A=yyyvsp;_TB5B=_TB5A[0];
# 2400 "parse.y"
yyval=_TB5B.v;goto _LL0;case 292:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1870->tag=35;
# 2405 "parse.y"
_T1870->f1=0;_T1870->f2=0;_TB5C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1870;}_TB5D=(void*)_TB5C;_TB5E=yyyvsp;_TB5F=_TB5E[0];_TB60=_TB5F.l;_TB61=_TB60.first_line;_TB62=Cyc_Position_loc_to_seg(_TB61);_TB63=Cyc_Absyn_new_exp(_TB5D,_TB62);yyval=Cyc_Exp_tok(_TB63);goto _LL0;case 293:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1870->tag=35;
# 2407 "parse.y"
_T1870->f1=0;_TB65=yyyvsp;_TB66=& _TB65[1].v;_TB67=(union Cyc_YYSTYPE*)_TB66;_TB68=Cyc_yyget_YY5(_TB67);_T1870->f2=Cyc_List_imp_rev(_TB68);_TB64=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1870;}_TB69=(void*)_TB64;_TB6A=yyyvsp;_TB6B=_TB6A[0];_TB6C=_TB6B.l;_TB6D=_TB6C.first_line;_TB6E=Cyc_Position_loc_to_seg(_TB6D);_TB6F=Cyc_Absyn_new_exp(_TB69,_TB6E);yyval=Cyc_Exp_tok(_TB6F);goto _LL0;case 294:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1870->tag=35;
# 2409 "parse.y"
_T1870->f1=0;_TB71=yyyvsp;_TB72=& _TB71[1].v;_TB73=(union Cyc_YYSTYPE*)_TB72;_TB74=Cyc_yyget_YY5(_TB73);_T1870->f2=Cyc_List_imp_rev(_TB74);_TB70=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1870;}_TB75=(void*)_TB70;_TB76=yyyvsp;_TB77=_TB76[0];_TB78=_TB77.l;_TB79=_TB78.first_line;_TB7A=Cyc_Position_loc_to_seg(_TB79);_TB7B=Cyc_Absyn_new_exp(_TB75,_TB7A);yyval=Cyc_Exp_tok(_TB7B);goto _LL0;case 295: _TB7C=yyyvsp;_TB7D=_TB7C[2];_TB7E=_TB7D.l;_TB7F=_TB7E.first_line;_TB80=
# 2411 "parse.y"
Cyc_Position_loc_to_seg(_TB7F);{struct _tuple0*_T1870=_cycalloc(sizeof(struct _tuple0));_T1870->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_TB83=yyyvsp;_TB84=& _TB83[2].v;_TB85=(union Cyc_YYSTYPE*)_TB84;*_T1871=Cyc_yyget_String_tok(_TB85);_TB82=(struct _fat_ptr*)_T1871;}_T1870->f1=_TB82;_TB81=(struct _tuple0*)_T1870;}_TB86=Cyc_Absyn_uint_type;_TB87=yyyvsp;_TB88=_TB87[2];_TB89=_TB88.l;_TB8A=_TB89.first_line;_TB8B=
Cyc_Position_loc_to_seg(_TB8A);_TB8C=Cyc_Absyn_uint_exp(0U,_TB8B);{
# 2411
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_TB80,_TB81,_TB86,_TB8C,0);_TB8D=vd;
# 2414
_TB8D->tq.real_const=1;{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_T1870->tag=26;
_T1870->f1=vd;_TB8F=yyyvsp;_TB90=& _TB8F[4].v;_TB91=(union Cyc_YYSTYPE*)_TB90;_T1870->f2=Cyc_yyget_Exp_tok(_TB91);_TB92=yyyvsp;_TB93=& _TB92[6].v;_TB94=(union Cyc_YYSTYPE*)_TB93;_T1870->f3=Cyc_yyget_Exp_tok(_TB94);_T1870->f4=0;_TB8E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T1870;}_TB95=(void*)_TB8E;_TB96=yyyvsp;_TB97=_TB96[0];_TB98=_TB97.l;_TB99=_TB98.first_line;_TB9A=Cyc_Position_loc_to_seg(_TB99);_TB9B=Cyc_Absyn_new_exp(_TB95,_TB9A);yyval=Cyc_Exp_tok(_TB9B);goto _LL0;}case 296: _TB9C=yyyvsp;_TB9D=& _TB9C[6].v;_TB9E=(union Cyc_YYSTYPE*)_TB9D;_TB9F=
# 2419 "parse.y"
Cyc_yyget_YY38(_TB9E);_TBA0=yyyvsp;_TBA1=_TBA0[6];_TBA2=_TBA1.l;_TBA3=_TBA2.first_line;_TBA4=Cyc_Position_loc_to_seg(_TBA3);{void*t=Cyc_Parse_type_name_to_type(_TB9F,_TBA4);{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_T1870->tag=27;_TBA6=yyyvsp;_TBA7=& _TBA6[4].v;_TBA8=(union Cyc_YYSTYPE*)_TBA7;
_T1870->f1=Cyc_yyget_Exp_tok(_TBA8);_T1870->f2=t;_T1870->f3=0;_TBA5=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T1870;}_TBA9=(void*)_TBA5;_TBAA=yyyvsp;_TBAB=_TBAA[0];_TBAC=_TBAB.l;_TBAD=_TBAC.first_line;_TBAE=Cyc_Position_loc_to_seg(_TBAD);_TBAF=Cyc_Absyn_new_exp(_TBA9,_TBAE);yyval=Cyc_Exp_tok(_TBAF);goto _LL0;}case 297:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1871=_cycalloc(sizeof(struct _tuple34));
# 2427 "parse.y"
_T1871->f0=0;_TBB2=yyyvsp;_TBB3=& _TBB2[0].v;_TBB4=(union Cyc_YYSTYPE*)_TBB3;_T1871->f1=Cyc_yyget_Exp_tok(_TBB4);_TBB1=(struct _tuple34*)_T1871;}_T1870->hd=_TBB1;_T1870->tl=0;_TBB0=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY5(_TBB0);goto _LL0;case 298:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1871=_cycalloc(sizeof(struct _tuple34));_TBB7=yyyvsp;_TBB8=& _TBB7[0].v;_TBB9=(union Cyc_YYSTYPE*)_TBB8;
# 2429 "parse.y"
_T1871->f0=Cyc_yyget_YY42(_TBB9);_TBBA=yyyvsp;_TBBB=& _TBBA[1].v;_TBBC=(union Cyc_YYSTYPE*)_TBBB;_T1871->f1=Cyc_yyget_Exp_tok(_TBBC);_TBB6=(struct _tuple34*)_T1871;}_T1870->hd=_TBB6;_T1870->tl=0;_TBB5=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY5(_TBB5);goto _LL0;case 299:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1871=_cycalloc(sizeof(struct _tuple34));
# 2431 "parse.y"
_T1871->f0=0;_TBBF=yyyvsp;_TBC0=& _TBBF[2].v;_TBC1=(union Cyc_YYSTYPE*)_TBC0;_T1871->f1=Cyc_yyget_Exp_tok(_TBC1);_TBBE=(struct _tuple34*)_T1871;}_T1870->hd=_TBBE;_TBC2=yyyvsp;_TBC3=& _TBC2[0].v;_TBC4=(union Cyc_YYSTYPE*)_TBC3;_T1870->tl=Cyc_yyget_YY5(_TBC4);_TBBD=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY5(_TBBD);goto _LL0;case 300:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1871=_cycalloc(sizeof(struct _tuple34));_TBC7=yyyvsp;_TBC8=& _TBC7[2].v;_TBC9=(union Cyc_YYSTYPE*)_TBC8;
# 2433 "parse.y"
_T1871->f0=Cyc_yyget_YY42(_TBC9);_TBCA=yyyvsp;_TBCB=& _TBCA[3].v;_TBCC=(union Cyc_YYSTYPE*)_TBCB;_T1871->f1=Cyc_yyget_Exp_tok(_TBCC);_TBC6=(struct _tuple34*)_T1871;}_T1870->hd=_TBC6;_TBCD=yyyvsp;_TBCE=& _TBCD[0].v;_TBCF=(union Cyc_YYSTYPE*)_TBCE;_T1870->tl=Cyc_yyget_YY5(_TBCF);_TBC5=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY5(_TBC5);goto _LL0;case 301: _TBD0=yyyvsp;_TBD1=& _TBD0[0].v;_TBD2=(union Cyc_YYSTYPE*)_TBD1;_TBD3=
# 2437 "parse.y"
Cyc_yyget_YY42(_TBD2);_TBD4=Cyc_List_imp_rev(_TBD3);yyval=Cyc_YY42(_TBD4);goto _LL0;case 302:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T1871->tag=1;{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));_TBD8=yyyvsp;_TBD9=& _TBD8[0].v;_TBDA=(union Cyc_YYSTYPE*)_TBD9;
# 2438 "parse.y"
*_T1872=Cyc_yyget_String_tok(_TBDA);_TBD7=(struct _fat_ptr*)_T1872;}_T1871->f1=_TBD7;_TBD6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1871;}_T1870->hd=(void*)_TBD6;_T1870->tl=0;_TBD5=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY42(_TBD5);goto _LL0;case 303:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TBDC=yyyvsp;_TBDD=& _TBDC[0].v;_TBDE=(union Cyc_YYSTYPE*)_TBDD;
# 2443 "parse.y"
_T1870->hd=Cyc_yyget_YY43(_TBDE);_T1870->tl=0;_TBDB=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY42(_TBDB);goto _LL0;case 304:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TBE0=yyyvsp;_TBE1=& _TBE0[1].v;_TBE2=(union Cyc_YYSTYPE*)_TBE1;
# 2444 "parse.y"
_T1870->hd=Cyc_yyget_YY43(_TBE2);_TBE3=yyyvsp;_TBE4=& _TBE3[0].v;_TBE5=(union Cyc_YYSTYPE*)_TBE4;_T1870->tl=Cyc_yyget_YY42(_TBE5);_TBDF=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY42(_TBDF);goto _LL0;case 305:{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_T1870->tag=0;_TBE7=yyyvsp;_TBE8=& _TBE7[1].v;_TBE9=(union Cyc_YYSTYPE*)_TBE8;
# 2448 "parse.y"
_T1870->f1=Cyc_yyget_Exp_tok(_TBE9);_TBE6=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_T1870;}_TBEA=(void*)_TBE6;yyval=Cyc_YY43(_TBEA);goto _LL0;case 306:{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T1870->tag=1;{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_TBED=yyyvsp;_TBEE=& _TBED[1].v;_TBEF=(union Cyc_YYSTYPE*)_TBEE;
# 2449 "parse.y"
*_T1871=Cyc_yyget_String_tok(_TBEF);_TBEC=(struct _fat_ptr*)_T1871;}_T1870->f1=_TBEC;_TBEB=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1870;}_TBF0=(void*)_TBEB;yyval=Cyc_YY43(_TBF0);goto _LL0;case 307: _TBF1=yyyvsp;_TBF2=& _TBF1[0].v;_TBF3=(union Cyc_YYSTYPE*)_TBF2;{
# 2454 "parse.y"
struct _tuple26 _T1870=Cyc_yyget_YY36(_TBF3);struct Cyc_List_List*_T1871;struct Cyc_Parse_Type_specifier _T1872;struct Cyc_Absyn_Tqual _T1873;_T1873=_T1870.f0;_T1872=_T1870.f1;_T1871=_T1870.f2;{struct Cyc_Absyn_Tqual tq=_T1873;struct Cyc_Parse_Type_specifier tss=_T1872;struct Cyc_List_List*atts=_T1871;_TBF4=tss;_TBF5=yyyvsp;_TBF6=_TBF5[0];_TBF7=_TBF6.l;_TBF8=_TBF7.first_line;_TBF9=
Cyc_Position_loc_to_seg(_TBF8);{void*t=Cyc_Parse_speclist2typ(_TBF4,_TBF9);
if(atts==0)goto _TL2DA;_TBFA=yyyvsp;_TBFB=_TBFA[0];_TBFC=_TBFB.l;_TBFD=_TBFC.first_line;_TBFE=
Cyc_Position_loc_to_seg(_TBFD);_TBFF=_tag_fat("ignoring attributes in type",sizeof(char),28U);_TC00=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TBFE,_TBFF,_TC00);goto _TL2DB;_TL2DA: _TL2DB:{struct _tuple8*_T1874=_cycalloc(sizeof(struct _tuple8));
_T1874->f0=0;_T1874->f1=tq;_T1874->f2=t;_TC01=(struct _tuple8*)_T1874;}yyval=Cyc_YY38(_TC01);goto _LL0;}}}case 308: _TC02=yyyvsp;_TC03=& _TC02[0].v;_TC04=(union Cyc_YYSTYPE*)_TC03;{
# 2461 "parse.y"
struct _tuple26 _T1870=Cyc_yyget_YY36(_TC04);struct Cyc_List_List*_T1871;struct Cyc_Parse_Type_specifier _T1872;struct Cyc_Absyn_Tqual _T1873;_T1873=_T1870.f0;_T1872=_T1870.f1;_T1871=_T1870.f2;{struct Cyc_Absyn_Tqual tq=_T1873;struct Cyc_Parse_Type_specifier tss=_T1872;struct Cyc_List_List*atts=_T1871;_TC05=tss;_TC06=yyyvsp;_TC07=_TC06[0];_TC08=_TC07.l;_TC09=_TC08.first_line;_TC0A=
Cyc_Position_loc_to_seg(_TC09);{void*t=Cyc_Parse_speclist2typ(_TC05,_TC0A);_TC0B=yyyvsp;_TC0C=& _TC0B[1].v;_TC0D=(union Cyc_YYSTYPE*)_TC0C;_TC0E=
Cyc_yyget_YY31(_TC0D);{struct Cyc_List_List*tms=_TC0E.tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);_TC0F=t_info;_TC10=_TC0F.f2;
if(_TC10==0)goto _TL2DC;_TC11=yyyvsp;_TC12=_TC11[1];_TC13=_TC12.l;_TC14=_TC13.first_line;_TC15=
Cyc_Position_loc_to_seg(_TC14);_TC16=_tag_fat("bad type params, ignoring",sizeof(char),26U);_TC17=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC15,_TC16,_TC17);goto _TL2DD;_TL2DC: _TL2DD: _TC18=t_info;_TC19=_TC18.f3;
if(_TC19==0)goto _TL2DE;_TC1A=yyyvsp;_TC1B=_TC1A[1];_TC1C=_TC1B.l;_TC1D=_TC1C.first_line;_TC1E=
Cyc_Position_loc_to_seg(_TC1D);_TC1F=_tag_fat("bad specifiers, ignoring",sizeof(char),25U);_TC20=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC1E,_TC1F,_TC20);goto _TL2DF;_TL2DE: _TL2DF:{struct _tuple8*_T1874=_cycalloc(sizeof(struct _tuple8));
_T1874->f0=0;_TC22=t_info;_T1874->f1=_TC22.f0;_TC23=t_info;_T1874->f2=_TC23.f1;_TC21=(struct _tuple8*)_T1874;}yyval=Cyc_YY38(_TC21);goto _LL0;}}}}case 309: _TC24=yyyvsp;_TC25=& _TC24[0].v;_TC26=(union Cyc_YYSTYPE*)_TC25;_TC27=
# 2474 "parse.y"
Cyc_yyget_YY38(_TC26);_TC28=yyyvsp;_TC29=_TC28[0];_TC2A=_TC29.l;_TC2B=_TC2A.first_line;_TC2C=Cyc_Position_loc_to_seg(_TC2B);_TC2D=Cyc_Parse_type_name_to_type(_TC27,_TC2C);yyval=Cyc_YY45(_TC2D);goto _LL0;case 310: _TC2E=
# 2475 "parse.y"
Cyc_Absyn_join_eff(0);yyval=Cyc_YY45(_TC2E);goto _LL0;case 311: _TC2F=yyyvsp;_TC30=& _TC2F[1].v;_TC31=(union Cyc_YYSTYPE*)_TC30;_TC32=
# 2476 "parse.y"
Cyc_yyget_YY41(_TC31);_TC33=Cyc_Absyn_join_eff(_TC32);yyval=Cyc_YY45(_TC33);goto _LL0;case 312: _TC34=yyyvsp;_TC35=& _TC34[2].v;_TC36=(union Cyc_YYSTYPE*)_TC35;_TC37=
# 2477 "parse.y"
Cyc_yyget_YY45(_TC36);_TC38=Cyc_Absyn_regionsof_eff(_TC37);yyval=Cyc_YY45(_TC38);goto _LL0;case 313:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TC3A=yyyvsp;_TC3B=& _TC3A[0].v;_TC3C=(union Cyc_YYSTYPE*)_TC3B;
# 2478 "parse.y"
_T1870->hd=Cyc_yyget_YY45(_TC3C);_TC3D=yyyvsp;_TC3E=& _TC3D[2].v;_TC3F=(union Cyc_YYSTYPE*)_TC3E;_T1870->tl=Cyc_yyget_YY41(_TC3F);_TC39=(struct Cyc_List_List*)_T1870;}_TC40=Cyc_Absyn_join_eff(_TC39);yyval=Cyc_YY45(_TC40);goto _LL0;case 314: _TC41=yyyvsp;_TC42=& _TC41[0].v;_TC43=(union Cyc_YYSTYPE*)_TC42;_TC44=
# 2479 "parse.y"
Cyc_yyget_YY45(_TC43);yyval=Cyc_YY45(_TC44);goto _LL0;case 315:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TC46=yyyvsp;_TC47=& _TC46[0].v;_TC48=(union Cyc_YYSTYPE*)_TC47;
# 2485 "parse.y"
_T1870->hd=Cyc_yyget_YY45(_TC48);_T1870->tl=0;_TC45=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY41(_TC45);goto _LL0;case 316:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_TC4A=yyyvsp;_TC4B=& _TC4A[2].v;_TC4C=(union Cyc_YYSTYPE*)_TC4B;
# 2486 "parse.y"
_T1870->hd=Cyc_yyget_YY45(_TC4C);_TC4D=yyyvsp;_TC4E=& _TC4D[0].v;_TC4F=(union Cyc_YYSTYPE*)_TC4E;_T1870->tl=Cyc_yyget_YY41(_TC4F);_TC49=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY41(_TC49);goto _LL0;case 317:{struct Cyc_Parse_Abstractdeclarator _T1870;_TC51=yyyvsp;_TC52=& _TC51[0].v;_TC53=(union Cyc_YYSTYPE*)_TC52;
# 2490 "parse.y"
_T1870.tms=Cyc_yyget_YY27(_TC53);_TC50=_T1870;}yyval=Cyc_YY31(_TC50);goto _LL0;case 318: _TC54=yyyvsp;_TC55=_TC54[0];
# 2491 "parse.y"
yyval=_TC55.v;goto _LL0;case 319:{struct Cyc_Parse_Abstractdeclarator _T1870;_TC57=yyyvsp;_TC58=& _TC57[0].v;_TC59=(union Cyc_YYSTYPE*)_TC58;_TC5A=
# 2493 "parse.y"
Cyc_yyget_YY27(_TC59);_TC5B=yyyvsp;_TC5C=& _TC5B[1].v;_TC5D=(union Cyc_YYSTYPE*)_TC5C;_TC5E=Cyc_yyget_YY31(_TC5D);_TC5F=_TC5E.tms;_T1870.tms=Cyc_List_imp_append(_TC5A,_TC5F);_TC56=_T1870;}yyval=Cyc_YY31(_TC56);goto _LL0;case 320: _TC60=yyyvsp;_TC61=_TC60[1];
# 2498 "parse.y"
yyval=_TC61.v;goto _LL0;case 321:{struct Cyc_Parse_Abstractdeclarator _T1870;_TC64=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_TC64,0U,sizeof(struct Cyc_List_List));_TC66=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_TC66,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1872->tag=0;_TC67=yyyvsp;_TC68=& _TC67[2].v;_TC69=(union Cyc_YYSTYPE*)_TC68;
# 2500 "parse.y"
_T1872->f1=Cyc_yyget_YY54(_TC69);_TC6A=yyyvsp;_TC6B=_TC6A[2];_TC6C=_TC6B.l;_TC6D=_TC6C.first_line;_T1872->f2=Cyc_Position_loc_to_seg(_TC6D);_TC65=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_TC65;_T1871->tl=0;_TC63=(struct Cyc_List_List*)_T1871;}_T1870.tms=_TC63;_TC62=_T1870;}yyval=Cyc_YY31(_TC62);goto _LL0;case 322:{struct Cyc_Parse_Abstractdeclarator _T1870;_TC70=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_TC70,0U,sizeof(struct Cyc_List_List));_TC72=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_TC72,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1872->tag=0;_TC73=yyyvsp;_TC74=& _TC73[3].v;_TC75=(union Cyc_YYSTYPE*)_TC74;
# 2502 "parse.y"
_T1872->f1=Cyc_yyget_YY54(_TC75);_TC76=yyyvsp;_TC77=_TC76[3];_TC78=_TC77.l;_TC79=_TC78.first_line;_T1872->f2=Cyc_Position_loc_to_seg(_TC79);_TC71=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_TC71;_TC7A=yyyvsp;_TC7B=& _TC7A[0].v;_TC7C=(union Cyc_YYSTYPE*)_TC7B;_TC7D=Cyc_yyget_YY31(_TC7C);_T1871->tl=_TC7D.tms;_TC6F=(struct Cyc_List_List*)_T1871;}_T1870.tms=_TC6F;_TC6E=_T1870;}yyval=Cyc_YY31(_TC6E);goto _LL0;case 323:{struct Cyc_Parse_Abstractdeclarator _T1870;_TC80=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_TC80,0U,sizeof(struct Cyc_List_List));_TC82=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_TC82,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1872->tag=1;_TC83=yyyvsp;_TC84=& _TC83[1].v;_TC85=(union Cyc_YYSTYPE*)_TC84;
# 2504 "parse.y"
_T1872->f1=Cyc_yyget_Exp_tok(_TC85);_TC86=yyyvsp;_TC87=& _TC86[3].v;_TC88=(union Cyc_YYSTYPE*)_TC87;_T1872->f2=Cyc_yyget_YY54(_TC88);_TC89=yyyvsp;_TC8A=_TC89[3];_TC8B=_TC8A.l;_TC8C=_TC8B.first_line;_T1872->f3=Cyc_Position_loc_to_seg(_TC8C);_TC81=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_TC81;_T1871->tl=0;_TC7F=(struct Cyc_List_List*)_T1871;}_T1870.tms=_TC7F;_TC7E=_T1870;}yyval=Cyc_YY31(_TC7E);goto _LL0;case 324:{struct Cyc_Parse_Abstractdeclarator _T1870;_TC8F=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_TC8F,0U,sizeof(struct Cyc_List_List));_TC91=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_TC91,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1872->tag=1;_TC92=yyyvsp;_TC93=& _TC92[2].v;_TC94=(union Cyc_YYSTYPE*)_TC93;
# 2506 "parse.y"
_T1872->f1=Cyc_yyget_Exp_tok(_TC94);_TC95=yyyvsp;_TC96=& _TC95[4].v;_TC97=(union Cyc_YYSTYPE*)_TC96;_T1872->f2=Cyc_yyget_YY54(_TC97);_TC98=yyyvsp;_TC99=_TC98[4];_TC9A=_TC99.l;_TC9B=_TC9A.first_line;_T1872->f3=Cyc_Position_loc_to_seg(_TC9B);_TC90=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_TC90;_TC9C=yyyvsp;_TC9D=& _TC9C[0].v;_TC9E=(union Cyc_YYSTYPE*)_TC9D;_TC9F=
Cyc_yyget_YY31(_TC9E);_T1871->tl=_TC9F.tms;_TC8E=(struct Cyc_List_List*)_T1871;}
# 2506
_T1870.tms=_TC8E;_TC8D=_T1870;}yyval=Cyc_YY31(_TC8D);goto _LL0;case 325: _TCA0=yyyvsp;_TCA1=& _TCA0[1].v;_TCA2=(union Cyc_YYSTYPE*)_TCA1;{
# 2510 "parse.y"
struct _tuple27*_T1870=Cyc_yyget_YY40(_TCA2);struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;void*_T1873;struct Cyc_Absyn_VarargInfo*_T1874;int _T1875;struct Cyc_List_List*_T1876;{struct _tuple27 _T1877=*_T1870;_T1876=_T1877.f0;_T1875=_T1877.f1;_T1874=_T1877.f2;_T1873=_T1877.f3;_T1872=_T1877.f4;_T1871=_T1877.f5;}{struct Cyc_List_List*lis=_T1876;int b=_T1875;struct Cyc_Absyn_VarargInfo*c=_T1874;void*eff=_T1873;struct Cyc_List_List*ec=_T1872;struct Cyc_List_List*qb=_T1871;_TCA3=yyyvsp;_TCA4=& _TCA3[3].v;_TCA5=(union Cyc_YYSTYPE*)_TCA4;{
struct _tuple21 _T1877=Cyc_yyget_YY62(_TCA5);struct Cyc_Absyn_Exp*_T1878;struct Cyc_Absyn_Exp*_T1879;struct Cyc_Absyn_Exp*_T187A;struct Cyc_Absyn_Exp*_T187B;_T187B=_T1877.f0;_T187A=_T1877.f1;_T1879=_T1877.f2;_T1878=_T1877.f3;{struct Cyc_Absyn_Exp*chk=_T187B;struct Cyc_Absyn_Exp*req=_T187A;struct Cyc_Absyn_Exp*ens=_T1879;struct Cyc_Absyn_Exp*thrws=_T1878;{struct Cyc_Parse_Abstractdeclarator _T187C;_TCA8=yyr;{struct Cyc_List_List*_T187D=_region_malloc(_TCA8,0U,sizeof(struct Cyc_List_List));_TCAA=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T187E=_region_malloc(_TCAA,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T187E->tag=3;_TCAC=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T187F=_region_malloc(_TCAC,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T187F->tag=1;
_T187F->f1=lis;_T187F->f2=b;_T187F->f3=c;_T187F->f4=eff;_T187F->f5=ec;_T187F->f6=qb;_T187F->f7=chk;_T187F->f8=req;_T187F->f9=ens;_T187F->f10=thrws;_TCAB=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T187F;}_T187E->f1=(void*)_TCAB;_TCA9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T187E;}_T187D->hd=(void*)_TCA9;_T187D->tl=0;_TCA7=(struct Cyc_List_List*)_T187D;}_T187C.tms=_TCA7;_TCA6=_T187C;}yyval=Cyc_YY31(_TCA6);goto _LL0;}}}}case 326: _TCAD=yyyvsp;_TCAE=& _TCAD[2].v;_TCAF=(union Cyc_YYSTYPE*)_TCAE;{
# 2515 "parse.y"
struct _tuple27*_T1870=Cyc_yyget_YY40(_TCAF);struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;void*_T1873;struct Cyc_Absyn_VarargInfo*_T1874;int _T1875;struct Cyc_List_List*_T1876;{struct _tuple27 _T1877=*_T1870;_T1876=_T1877.f0;_T1875=_T1877.f1;_T1874=_T1877.f2;_T1873=_T1877.f3;_T1872=_T1877.f4;_T1871=_T1877.f5;}{struct Cyc_List_List*lis=_T1876;int b=_T1875;struct Cyc_Absyn_VarargInfo*c=_T1874;void*eff=_T1873;struct Cyc_List_List*ec=_T1872;struct Cyc_List_List*qb=_T1871;_TCB0=yyyvsp;_TCB1=& _TCB0[4].v;_TCB2=(union Cyc_YYSTYPE*)_TCB1;{
struct _tuple21 _T1877=Cyc_yyget_YY62(_TCB2);struct Cyc_Absyn_Exp*_T1878;struct Cyc_Absyn_Exp*_T1879;struct Cyc_Absyn_Exp*_T187A;struct Cyc_Absyn_Exp*_T187B;_T187B=_T1877.f0;_T187A=_T1877.f1;_T1879=_T1877.f2;_T1878=_T1877.f3;{struct Cyc_Absyn_Exp*chk=_T187B;struct Cyc_Absyn_Exp*req=_T187A;struct Cyc_Absyn_Exp*ens=_T1879;struct Cyc_Absyn_Exp*thrws=_T1878;{struct Cyc_Parse_Abstractdeclarator _T187C;_TCB5=yyr;{struct Cyc_List_List*_T187D=_region_malloc(_TCB5,0U,sizeof(struct Cyc_List_List));_TCB7=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T187E=_region_malloc(_TCB7,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T187E->tag=3;_TCB9=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T187F=_region_malloc(_TCB9,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T187F->tag=1;
_T187F->f1=lis;
_T187F->f2=b;_T187F->f3=c;_T187F->f4=eff;_T187F->f5=ec;_T187F->f6=qb;_T187F->f7=chk;_T187F->f8=req;_T187F->f9=ens;_T187F->f10=thrws;_TCB8=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T187F;}
# 2517
_T187E->f1=(void*)_TCB8;_TCB6=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T187E;}_T187D->hd=(void*)_TCB6;_TCBA=yyyvsp;_TCBB=& _TCBA[0].v;_TCBC=(union Cyc_YYSTYPE*)_TCBB;_TCBD=
Cyc_yyget_YY31(_TCBC);_T187D->tl=_TCBD.tms;_TCB4=(struct Cyc_List_List*)_T187D;}
# 2517
_T187C.tms=_TCB4;_TCB3=_T187C;}yyval=Cyc_YY31(_TCB3);goto _LL0;}}}}case 327: _TCBF=Cyc_List_map_c;{
# 2522
struct Cyc_List_List*(*_T1870)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_TCBF;_TCBE=_T1870;}_TCC0=yyyvsp;_TCC1=_TCC0[1];_TCC2=_TCC1.l;_TCC3=_TCC2.first_line;_TCC4=Cyc_Position_loc_to_seg(_TCC3);_TCC5=yyyvsp;_TCC6=& _TCC5[2].v;_TCC7=(union Cyc_YYSTYPE*)_TCC6;_TCC8=Cyc_yyget_YY41(_TCC7);_TCC9=Cyc_List_imp_rev(_TCC8);{struct Cyc_List_List*ts=_TCBE(Cyc_Parse_typ2tvar,_TCC4,_TCC9);{struct Cyc_Parse_Abstractdeclarator _T1870;_TCCC=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_TCCC,0U,sizeof(struct Cyc_List_List));_TCCE=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_TCCE,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1872->tag=4;
_T1872->f1=ts;_TCCF=yyyvsp;_TCD0=_TCCF[1];_TCD1=_TCD0.l;_TCD2=_TCD1.first_line;_T1872->f2=Cyc_Position_loc_to_seg(_TCD2);_T1872->f3=0;_TCCD=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_TCCD;_TCD3=yyyvsp;_TCD4=& _TCD3[0].v;_TCD5=(union Cyc_YYSTYPE*)_TCD4;_TCD6=
Cyc_yyget_YY31(_TCD5);_T1871->tl=_TCD6.tms;_TCCB=(struct Cyc_List_List*)_T1871;}
# 2523
_T1870.tms=_TCCB;_TCCA=_T1870;}yyval=Cyc_YY31(_TCCA);goto _LL0;}case 328:{struct Cyc_Parse_Abstractdeclarator _T1870;_TCD9=yyr;{struct Cyc_List_List*_T1871=_region_malloc(_TCD9,0U,sizeof(struct Cyc_List_List));_TCDB=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1872=_region_malloc(_TCDB,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1872->tag=5;_TCDC=yyyvsp;_TCDD=_TCDC[1];_TCDE=_TCDD.l;_TCDF=_TCDE.first_line;
# 2527 "parse.y"
_T1872->f1=Cyc_Position_loc_to_seg(_TCDF);_TCE0=yyyvsp;_TCE1=& _TCE0[1].v;_TCE2=(union Cyc_YYSTYPE*)_TCE1;_T1872->f2=Cyc_yyget_YY46(_TCE2);_TCDA=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1872;}_T1871->hd=(void*)_TCDA;_TCE3=yyyvsp;_TCE4=& _TCE3[0].v;_TCE5=(union Cyc_YYSTYPE*)_TCE4;_TCE6=Cyc_yyget_YY31(_TCE5);_T1871->tl=_TCE6.tms;_TCD8=(struct Cyc_List_List*)_T1871;}_T1870.tms=_TCD8;_TCD7=_T1870;}yyval=Cyc_YY31(_TCD7);goto _LL0;case 329:{struct _tuple21 _T1870;_TCE8=yyyvsp;_TCE9=& _TCE8[2].v;_TCEA=(union Cyc_YYSTYPE*)_TCE9;
# 2533 "parse.y"
_T1870.f0=Cyc_yyget_Exp_tok(_TCEA);_T1870.f1=0;_T1870.f2=0;_T1870.f3=0;_TCE7=_T1870;}yyval=Cyc_YY62(_TCE7);goto _LL0;case 330:{struct _tuple21 _T1870;
# 2535 "parse.y"
_T1870.f0=0;_TCEC=yyyvsp;_TCED=& _TCEC[2].v;_TCEE=(union Cyc_YYSTYPE*)_TCED;_T1870.f1=Cyc_yyget_Exp_tok(_TCEE);_T1870.f2=0;_T1870.f3=0;_TCEB=_T1870;}yyval=Cyc_YY62(_TCEB);goto _LL0;case 331:{struct _tuple21 _T1870;
# 2537 "parse.y"
_T1870.f0=0;_T1870.f1=0;_TCF0=yyyvsp;_TCF1=& _TCF0[2].v;_TCF2=(union Cyc_YYSTYPE*)_TCF1;_T1870.f2=Cyc_yyget_Exp_tok(_TCF2);_T1870.f3=0;_TCEF=_T1870;}yyval=Cyc_YY62(_TCEF);goto _LL0;case 332:{struct _tuple21 _T1870;
# 2539 "parse.y"
_T1870.f0=0;_T1870.f1=0;_T1870.f2=0;_TCF4=yyyvsp;_TCF5=& _TCF4[2].v;_TCF6=(union Cyc_YYSTYPE*)_TCF5;_T1870.f3=Cyc_yyget_Exp_tok(_TCF6);_TCF3=_T1870;}yyval=Cyc_YY62(_TCF3);goto _LL0;case 333:{struct _tuple21 _T1870;_TCF8=yyyvsp;_TCF9=& _TCF8[2].v;_TCFA=(union Cyc_YYSTYPE*)_TCF9;
# 2541 "parse.y"
_T1870.f0=Cyc_yyget_Exp_tok(_TCFA);_T1870.f1=0;_T1870.f2=0;_T1870.f3=0;_TCF7=_T1870;}_TCFB=yyyvsp;_TCFC=& _TCFB[4].v;_TCFD=(union Cyc_YYSTYPE*)_TCFC;_TCFE=Cyc_yyget_YY62(_TCFD);_TCFF=Cyc_Parse_join_assns(_TCF7,_TCFE);yyval=Cyc_YY62(_TCFF);goto _LL0;case 334:{struct _tuple21 _T1870;
# 2543 "parse.y"
_T1870.f0=0;_TD01=yyyvsp;_TD02=& _TD01[2].v;_TD03=(union Cyc_YYSTYPE*)_TD02;_T1870.f1=Cyc_yyget_Exp_tok(_TD03);_T1870.f2=0;_T1870.f3=0;_TD00=_T1870;}_TD04=yyyvsp;_TD05=& _TD04[4].v;_TD06=(union Cyc_YYSTYPE*)_TD05;_TD07=Cyc_yyget_YY62(_TD06);_TD08=Cyc_Parse_join_assns(_TD00,_TD07);yyval=Cyc_YY62(_TD08);goto _LL0;case 335:{struct _tuple21 _T1870;
# 2545 "parse.y"
_T1870.f0=0;_T1870.f1=0;_TD0A=yyyvsp;_TD0B=& _TD0A[2].v;_TD0C=(union Cyc_YYSTYPE*)_TD0B;_T1870.f2=Cyc_yyget_Exp_tok(_TD0C);_T1870.f3=0;_TD09=_T1870;}_TD0D=yyyvsp;_TD0E=& _TD0D[4].v;_TD0F=(union Cyc_YYSTYPE*)_TD0E;_TD10=Cyc_yyget_YY62(_TD0F);_TD11=Cyc_Parse_join_assns(_TD09,_TD10);yyval=Cyc_YY62(_TD11);goto _LL0;case 336:{struct _tuple21 _T1870;
# 2547 "parse.y"
_T1870.f0=0;_T1870.f1=0;_T1870.f2=0;_TD13=yyyvsp;_TD14=& _TD13[2].v;_TD15=(union Cyc_YYSTYPE*)_TD14;_T1870.f3=Cyc_yyget_Exp_tok(_TD15);_TD12=_T1870;}_TD16=yyyvsp;_TD17=& _TD16[4].v;_TD18=(union Cyc_YYSTYPE*)_TD17;_TD19=Cyc_yyget_YY62(_TD18);_TD1A=Cyc_Parse_join_assns(_TD12,_TD19);yyval=Cyc_YY62(_TD1A);goto _LL0;case 337:{struct _tuple21 _T1870;
# 2551 "parse.y"
_T1870.f0=0;_T1870.f1=0;_T1870.f2=0;_T1870.f3=0;_TD1B=_T1870;}yyval=Cyc_YY62(_TD1B);goto _LL0;case 338: _TD1C=yyyvsp;_TD1D=_TD1C[0];
# 2552 "parse.y"
yyval=_TD1D.v;goto _LL0;case 339: _TD1E=yyyvsp;_TD1F=_TD1E[0];
# 2557 "parse.y"
yyval=_TD1F.v;goto _LL0;case 340: _TD20=yyyvsp;_TD21=_TD20[0];
# 2558 "parse.y"
yyval=_TD21.v;goto _LL0;case 341: _TD22=yyyvsp;_TD23=_TD22[0];
# 2559 "parse.y"
yyval=_TD23.v;goto _LL0;case 342: _TD24=yyyvsp;_TD25=_TD24[0];
# 2560 "parse.y"
yyval=_TD25.v;goto _LL0;case 343: _TD26=yyyvsp;_TD27=_TD26[0];
# 2561 "parse.y"
yyval=_TD27.v;goto _LL0;case 344: _TD28=yyyvsp;_TD29=_TD28[0];
# 2562 "parse.y"
yyval=_TD29.v;goto _LL0;case 345:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1870->tag=13;{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_TD2C=yyyvsp;_TD2D=& _TD2C[0].v;_TD2E=(union Cyc_YYSTYPE*)_TD2D;
# 2568 "parse.y"
*_T1871=Cyc_yyget_String_tok(_TD2E);_TD2B=(struct _fat_ptr*)_T1871;}_T1870->f1=_TD2B;_TD2F=yyyvsp;_TD30=& _TD2F[2].v;_TD31=(union Cyc_YYSTYPE*)_TD30;_T1870->f2=Cyc_yyget_Stmt_tok(_TD31);_TD2A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1870;}_TD32=(void*)_TD2A;_TD33=yyyvsp;_TD34=_TD33[0];_TD35=_TD34.l;_TD36=_TD35.first_line;_TD37=Cyc_Position_loc_to_seg(_TD36);_TD38=Cyc_Absyn_new_stmt(_TD32,_TD37);yyval=Cyc_Stmt_tok(_TD38);goto _LL0;case 346: _TD39=yyyvsp;_TD3A=_TD39[0];_TD3B=_TD3A.l;_TD3C=_TD3B.first_line;_TD3D=
# 2572 "parse.y"
Cyc_Position_loc_to_seg(_TD3C);_TD3E=Cyc_Absyn_skip_stmt(_TD3D);yyval=Cyc_Stmt_tok(_TD3E);goto _LL0;case 347: _TD3F=yyyvsp;_TD40=& _TD3F[0].v;_TD41=(union Cyc_YYSTYPE*)_TD40;_TD42=
# 2573 "parse.y"
Cyc_yyget_Exp_tok(_TD41);_TD43=yyyvsp;_TD44=_TD43[0];_TD45=_TD44.l;_TD46=_TD45.first_line;_TD47=Cyc_Position_loc_to_seg(_TD46);_TD48=Cyc_Absyn_exp_stmt(_TD42,_TD47);yyval=Cyc_Stmt_tok(_TD48);goto _LL0;case 348:
# 2578 "parse.y"
 Cyc_Parse_inside_function_definition=1;goto _LL0;case 349:
# 2582 "parse.y"
 Cyc_Parse_inside_function_definition=0;goto _LL0;case 350: _TD49=yyyvsp;_TD4A=_TD49[0];_TD4B=_TD4A.l;_TD4C=_TD4B.first_line;_TD4D=
# 2586 "parse.y"
Cyc_Position_loc_to_seg(_TD4C);_TD4E=Cyc_Absyn_skip_stmt(_TD4D);yyval=Cyc_Stmt_tok(_TD4E);goto _LL0;case 351: _TD4F=yyyvsp;_TD50=_TD4F[1];
# 2587 "parse.y"
yyval=_TD50.v;goto _LL0;case 352: _TD51=yyyvsp;_TD52=_TD51[0];_TD53=_TD52.l;_TD54=_TD53.first_line;_TD55=
# 2592 "parse.y"
Cyc_Position_loc_to_seg(_TD54);_TD56=Cyc_Absyn_skip_stmt(_TD55);yyval=Cyc_Stmt_tok(_TD56);goto _LL0;case 353: _TD57=yyyvsp;_TD58=_TD57[1];
# 2593 "parse.y"
yyval=_TD58.v;goto _LL0;case 354: _TD59=yyyvsp;_TD5A=& _TD59[0].v;_TD5B=(union Cyc_YYSTYPE*)_TD5A;_TD5C=
# 2598 "parse.y"
Cyc_yyget_YY16(_TD5B);_TD5D=yyyvsp;_TD5E=_TD5D[0];_TD5F=_TD5E.l;_TD60=_TD5F.first_line;_TD61=Cyc_Position_loc_to_seg(_TD60);_TD62=Cyc_Absyn_skip_stmt(_TD61);_TD63=Cyc_Parse_flatten_declarations(_TD5C,_TD62);yyval=Cyc_Stmt_tok(_TD63);goto _LL0;case 355: _TD64=yyyvsp;_TD65=& _TD64[0].v;_TD66=(union Cyc_YYSTYPE*)_TD65;_TD67=
# 2599 "parse.y"
Cyc_yyget_YY16(_TD66);_TD68=yyyvsp;_TD69=& _TD68[1].v;_TD6A=(union Cyc_YYSTYPE*)_TD69;_TD6B=Cyc_yyget_Stmt_tok(_TD6A);_TD6C=Cyc_Parse_flatten_declarations(_TD67,_TD6B);yyval=Cyc_Stmt_tok(_TD6C);goto _LL0;case 356:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1870->tag=13;{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_TD6F=yyyvsp;_TD70=& _TD6F[0].v;_TD71=(union Cyc_YYSTYPE*)_TD70;
# 2601 "parse.y"
*_T1871=Cyc_yyget_String_tok(_TD71);_TD6E=(struct _fat_ptr*)_T1871;}_T1870->f1=_TD6E;_TD72=yyyvsp;_TD73=& _TD72[2].v;_TD74=(union Cyc_YYSTYPE*)_TD73;_TD75=Cyc_yyget_YY16(_TD74);_TD76=Cyc_Absyn_skip_stmt(0U);_T1870->f2=Cyc_Parse_flatten_declarations(_TD75,_TD76);_TD6D=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1870;}_TD77=(void*)_TD6D;_TD78=yyyvsp;_TD79=_TD78[0];_TD7A=_TD79.l;_TD7B=_TD7A.first_line;_TD7C=Cyc_Position_loc_to_seg(_TD7B);_TD7D=Cyc_Absyn_new_stmt(_TD77,_TD7C);yyval=Cyc_Stmt_tok(_TD7D);goto _LL0;case 357:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1870->tag=13;{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_TD80=yyyvsp;_TD81=& _TD80[0].v;_TD82=(union Cyc_YYSTYPE*)_TD81;
# 2603 "parse.y"
*_T1871=Cyc_yyget_String_tok(_TD82);_TD7F=(struct _fat_ptr*)_T1871;}_T1870->f1=_TD7F;_TD83=yyyvsp;_TD84=& _TD83[2].v;_TD85=(union Cyc_YYSTYPE*)_TD84;_TD86=Cyc_yyget_YY16(_TD85);_TD87=yyyvsp;_TD88=& _TD87[3].v;_TD89=(union Cyc_YYSTYPE*)_TD88;_TD8A=Cyc_yyget_Stmt_tok(_TD89);_T1870->f2=Cyc_Parse_flatten_declarations(_TD86,_TD8A);_TD7E=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1870;}_TD8B=(void*)_TD7E;_TD8C=yyyvsp;_TD8D=_TD8C[0];_TD8E=_TD8D.l;_TD8F=_TD8E.first_line;_TD90=Cyc_Position_loc_to_seg(_TD8F);_TD91=Cyc_Absyn_new_stmt(_TD8B,_TD90);yyval=Cyc_Stmt_tok(_TD91);goto _LL0;case 358: _TD92=yyyvsp;_TD93=_TD92[0];
# 2604 "parse.y"
yyval=_TD93.v;goto _LL0;case 359: _TD94=yyyvsp;_TD95=& _TD94[0].v;_TD96=(union Cyc_YYSTYPE*)_TD95;_TD97=
# 2605 "parse.y"
Cyc_yyget_Stmt_tok(_TD96);_TD98=yyyvsp;_TD99=& _TD98[1].v;_TD9A=(union Cyc_YYSTYPE*)_TD99;_TD9B=Cyc_yyget_Stmt_tok(_TD9A);_TD9C=yyyvsp;_TD9D=_TD9C[0];_TD9E=_TD9D.l;_TD9F=_TD9E.first_line;_TDA0=Cyc_Position_loc_to_seg(_TD9F);_TDA1=Cyc_Absyn_seq_stmt(_TD97,_TD9B,_TDA0);yyval=Cyc_Stmt_tok(_TDA1);goto _LL0;case 360:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1870->tag=1;_TDA3=yyyvsp;_TDA4=& _TDA3[0].v;_TDA5=(union Cyc_YYSTYPE*)_TDA4;
# 2606 "parse.y"
_T1870->f1=Cyc_yyget_YY15(_TDA5);_TDA2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1870;}_TDA6=(void*)_TDA2;_TDA7=yyyvsp;_TDA8=_TDA7[0];_TDA9=_TDA8.l;_TDAA=_TDA9.first_line;_TDAB=Cyc_Position_loc_to_seg(_TDAA);_TDAC=Cyc_Absyn_new_decl(_TDA6,_TDAB);_TDAD=
Cyc_Absyn_skip_stmt(0U);_TDAE=
# 2606
Cyc_Parse_flatten_decl(_TDAC,_TDAD);yyval=Cyc_Stmt_tok(_TDAE);goto _LL0;case 361:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1870->tag=1;_TDB0=yyyvsp;_TDB1=& _TDB0[0].v;_TDB2=(union Cyc_YYSTYPE*)_TDB1;
# 2609 "parse.y"
_T1870->f1=Cyc_yyget_YY15(_TDB2);_TDAF=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1870;}_TDB3=(void*)_TDAF;_TDB4=yyyvsp;_TDB5=_TDB4[0];_TDB6=_TDB5.l;_TDB7=_TDB6.first_line;_TDB8=Cyc_Position_loc_to_seg(_TDB7);_TDB9=Cyc_Absyn_new_decl(_TDB3,_TDB8);_TDBA=yyyvsp;_TDBB=& _TDBA[1].v;_TDBC=(union Cyc_YYSTYPE*)_TDBB;_TDBD=Cyc_yyget_Stmt_tok(_TDBC);_TDBE=Cyc_Parse_flatten_decl(_TDB9,_TDBD);yyval=Cyc_Stmt_tok(_TDBE);goto _LL0;case 362: _TDBF=yyyvsp;_TDC0=& _TDBF[2].v;_TDC1=(union Cyc_YYSTYPE*)_TDC0;_TDC2=
# 2614 "parse.y"
Cyc_yyget_Exp_tok(_TDC1);_TDC3=yyyvsp;_TDC4=& _TDC3[4].v;_TDC5=(union Cyc_YYSTYPE*)_TDC4;_TDC6=Cyc_yyget_Stmt_tok(_TDC5);_TDC7=Cyc_Absyn_skip_stmt(0U);_TDC8=yyyvsp;_TDC9=_TDC8[0];_TDCA=_TDC9.l;_TDCB=_TDCA.first_line;_TDCC=Cyc_Position_loc_to_seg(_TDCB);_TDCD=Cyc_Absyn_ifthenelse_stmt(_TDC2,_TDC6,_TDC7,_TDCC);yyval=Cyc_Stmt_tok(_TDCD);goto _LL0;case 363: _TDCE=yyyvsp;_TDCF=& _TDCE[2].v;_TDD0=(union Cyc_YYSTYPE*)_TDCF;_TDD1=
# 2616 "parse.y"
Cyc_yyget_Exp_tok(_TDD0);_TDD2=yyyvsp;_TDD3=& _TDD2[4].v;_TDD4=(union Cyc_YYSTYPE*)_TDD3;_TDD5=Cyc_yyget_Stmt_tok(_TDD4);_TDD6=yyyvsp;_TDD7=& _TDD6[6].v;_TDD8=(union Cyc_YYSTYPE*)_TDD7;_TDD9=Cyc_yyget_Stmt_tok(_TDD8);_TDDA=yyyvsp;_TDDB=_TDDA[0];_TDDC=_TDDB.l;_TDDD=_TDDC.first_line;_TDDE=Cyc_Position_loc_to_seg(_TDDD);_TDDF=Cyc_Absyn_ifthenelse_stmt(_TDD1,_TDD5,_TDD9,_TDDE);yyval=Cyc_Stmt_tok(_TDDF);goto _LL0;case 364: _TDE0=yyyvsp;_TDE1=& _TDE0[2].v;_TDE2=(union Cyc_YYSTYPE*)_TDE1;_TDE3=
# 2622 "parse.y"
Cyc_yyget_Exp_tok(_TDE2);_TDE4=yyyvsp;_TDE5=& _TDE4[5].v;_TDE6=(union Cyc_YYSTYPE*)_TDE5;_TDE7=Cyc_yyget_YY8(_TDE6);_TDE8=yyyvsp;_TDE9=_TDE8[0];_TDEA=_TDE9.l;_TDEB=_TDEA.first_line;_TDEC=Cyc_Position_loc_to_seg(_TDEB);_TDED=Cyc_Absyn_switch_stmt(_TDE3,_TDE7,_TDEC);yyval=Cyc_Stmt_tok(_TDED);goto _LL0;case 365: _TDEE=yyyvsp;_TDEF=& _TDEE[1].v;_TDF0=(union Cyc_YYSTYPE*)_TDEF;_TDF1=
# 2625
Cyc_yyget_QualId_tok(_TDF0);_TDF2=yyyvsp;_TDF3=_TDF2[1];_TDF4=_TDF3.l;_TDF5=_TDF4.first_line;_TDF6=Cyc_Position_loc_to_seg(_TDF5);_TDF7=Cyc_Absyn_unknownid_exp(_TDF1,_TDF6);_TDF8=yyyvsp;_TDF9=& _TDF8[3].v;_TDFA=(union Cyc_YYSTYPE*)_TDF9;_TDFB=Cyc_yyget_YY8(_TDFA);_TDFC=yyyvsp;_TDFD=_TDFC[0];_TDFE=_TDFD.l;_TDFF=_TDFE.first_line;_TE00=Cyc_Position_loc_to_seg(_TDFF);_TE01=Cyc_Absyn_switch_stmt(_TDF7,_TDFB,_TE00);yyval=Cyc_Stmt_tok(_TE01);goto _LL0;case 366: _TE02=yyyvsp;_TE03=& _TE02[3].v;_TE04=(union Cyc_YYSTYPE*)_TE03;_TE05=
# 2628
Cyc_yyget_YY4(_TE04);_TE06=yyyvsp;_TE07=_TE06[1];_TE08=_TE07.l;_TE09=_TE08.first_line;_TE0A=Cyc_Position_loc_to_seg(_TE09);_TE0B=Cyc_Absyn_tuple_exp(_TE05,_TE0A);_TE0C=yyyvsp;_TE0D=& _TE0C[6].v;_TE0E=(union Cyc_YYSTYPE*)_TE0D;_TE0F=Cyc_yyget_YY8(_TE0E);_TE10=yyyvsp;_TE11=_TE10[0];_TE12=_TE11.l;_TE13=_TE12.first_line;_TE14=Cyc_Position_loc_to_seg(_TE13);_TE15=Cyc_Absyn_switch_stmt(_TE0B,_TE0F,_TE14);yyval=Cyc_Stmt_tok(_TE15);goto _LL0;case 367: _TE16=yyyvsp;_TE17=& _TE16[1].v;_TE18=(union Cyc_YYSTYPE*)_TE17;_TE19=
# 2632 "parse.y"
Cyc_yyget_Stmt_tok(_TE18);_TE1A=yyyvsp;_TE1B=& _TE1A[4].v;_TE1C=(union Cyc_YYSTYPE*)_TE1B;_TE1D=Cyc_yyget_YY8(_TE1C);_TE1E=yyyvsp;_TE1F=_TE1E[0];_TE20=_TE1F.l;_TE21=_TE20.first_line;_TE22=Cyc_Position_loc_to_seg(_TE21);_TE23=Cyc_Absyn_trycatch_stmt(_TE19,_TE1D,_TE22);yyval=Cyc_Stmt_tok(_TE23);goto _LL0;case 368:
# 2646 "parse.y"
 yyval=Cyc_YY8(0);goto _LL0;case 369:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE26=& Cyc_Absyn_Wild_p_val;_TE27=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_TE26;_TE28=(void*)_TE27;_TE29=yyyvsp;_TE2A=_TE29[0];_TE2B=_TE2A.l;_TE2C=_TE2B.first_line;_TE2D=
# 2649 "parse.y"
Cyc_Position_loc_to_seg(_TE2C);_T1871->pattern=Cyc_Absyn_new_pat(_TE28,_TE2D);_T1871->pat_vars=0;
_T1871->where_clause=0;_TE2E=yyyvsp;_TE2F=& _TE2E[2].v;_TE30=(union Cyc_YYSTYPE*)_TE2F;_T1871->body=Cyc_yyget_Stmt_tok(_TE30);_TE31=yyyvsp;_TE32=_TE31[0];_TE33=_TE32.l;_TE34=_TE33.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_TE34);_TE25=(struct Cyc_Absyn_Switch_clause*)_T1871;}
# 2649
_T1870->hd=_TE25;_TE35=yyyvsp;_TE36=& _TE35[3].v;_TE37=(union Cyc_YYSTYPE*)_TE36;
_T1870->tl=Cyc_yyget_YY8(_TE37);_TE24=(struct Cyc_List_List*)_T1870;}
# 2649
yyval=Cyc_YY8(_TE24);goto _LL0;case 370:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE3A=yyyvsp;_TE3B=& _TE3A[1].v;_TE3C=(union Cyc_YYSTYPE*)_TE3B;
# 2652 "parse.y"
_T1871->pattern=Cyc_yyget_YY9(_TE3C);_T1871->pat_vars=0;_T1871->where_clause=0;_TE3D=yyyvsp;_TE3E=_TE3D[2];_TE3F=_TE3E.l;_TE40=_TE3F.first_line;_TE41=
Cyc_Position_loc_to_seg(_TE40);_T1871->body=Cyc_Absyn_fallthru_stmt(0,_TE41);_TE42=yyyvsp;_TE43=_TE42[0];_TE44=_TE43.l;_TE45=_TE44.first_line;
_T1871->loc=Cyc_Position_loc_to_seg(_TE45);_TE39=(struct Cyc_Absyn_Switch_clause*)_T1871;}
# 2652
_T1870->hd=_TE39;_TE46=yyyvsp;_TE47=& _TE46[3].v;_TE48=(union Cyc_YYSTYPE*)_TE47;
# 2654
_T1870->tl=Cyc_yyget_YY8(_TE48);_TE38=(struct Cyc_List_List*)_T1870;}
# 2652
yyval=Cyc_YY8(_TE38);goto _LL0;case 371:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE4B=yyyvsp;_TE4C=& _TE4B[1].v;_TE4D=(union Cyc_YYSTYPE*)_TE4C;
# 2656 "parse.y"
_T1871->pattern=Cyc_yyget_YY9(_TE4D);_T1871->pat_vars=0;_T1871->where_clause=0;_TE4E=yyyvsp;_TE4F=& _TE4E[3].v;_TE50=(union Cyc_YYSTYPE*)_TE4F;_T1871->body=Cyc_yyget_Stmt_tok(_TE50);_TE51=yyyvsp;_TE52=_TE51[0];_TE53=_TE52.l;_TE54=_TE53.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_TE54);_TE4A=(struct Cyc_Absyn_Switch_clause*)_T1871;}_T1870->hd=_TE4A;_TE55=yyyvsp;_TE56=& _TE55[4].v;_TE57=(union Cyc_YYSTYPE*)_TE56;_T1870->tl=Cyc_yyget_YY8(_TE57);_TE49=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY8(_TE49);goto _LL0;case 372:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE5A=yyyvsp;_TE5B=& _TE5A[1].v;_TE5C=(union Cyc_YYSTYPE*)_TE5B;
# 2658 "parse.y"
_T1871->pattern=Cyc_yyget_YY9(_TE5C);_T1871->pat_vars=0;_TE5D=yyyvsp;_TE5E=& _TE5D[3].v;_TE5F=(union Cyc_YYSTYPE*)_TE5E;_T1871->where_clause=Cyc_yyget_Exp_tok(_TE5F);_TE60=yyyvsp;_TE61=_TE60[4];_TE62=_TE61.l;_TE63=_TE62.first_line;_TE64=
Cyc_Position_loc_to_seg(_TE63);_T1871->body=Cyc_Absyn_fallthru_stmt(0,_TE64);_TE65=yyyvsp;_TE66=_TE65[0];_TE67=_TE66.l;_TE68=_TE67.first_line;
_T1871->loc=Cyc_Position_loc_to_seg(_TE68);_TE59=(struct Cyc_Absyn_Switch_clause*)_T1871;}
# 2658
_T1870->hd=_TE59;_TE69=yyyvsp;_TE6A=& _TE69[5].v;_TE6B=(union Cyc_YYSTYPE*)_TE6A;
# 2660
_T1870->tl=Cyc_yyget_YY8(_TE6B);_TE58=(struct Cyc_List_List*)_T1870;}
# 2658
yyval=Cyc_YY8(_TE58);goto _LL0;case 373:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE6E=yyyvsp;_TE6F=& _TE6E[1].v;_TE70=(union Cyc_YYSTYPE*)_TE6F;
# 2662 "parse.y"
_T1871->pattern=Cyc_yyget_YY9(_TE70);_T1871->pat_vars=0;_TE71=yyyvsp;_TE72=& _TE71[3].v;_TE73=(union Cyc_YYSTYPE*)_TE72;_T1871->where_clause=Cyc_yyget_Exp_tok(_TE73);_TE74=yyyvsp;_TE75=& _TE74[5].v;_TE76=(union Cyc_YYSTYPE*)_TE75;_T1871->body=Cyc_yyget_Stmt_tok(_TE76);_TE77=yyyvsp;_TE78=_TE77[0];_TE79=_TE78.l;_TE7A=_TE79.first_line;_T1871->loc=Cyc_Position_loc_to_seg(_TE7A);_TE6D=(struct Cyc_Absyn_Switch_clause*)_T1871;}_T1870->hd=_TE6D;_TE7B=yyyvsp;_TE7C=& _TE7B[6].v;_TE7D=(union Cyc_YYSTYPE*)_TE7C;_T1870->tl=Cyc_yyget_YY8(_TE7D);_TE6C=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY8(_TE6C);goto _LL0;case 374: _TE7E=yyyvsp;_TE7F=& _TE7E[2].v;_TE80=(union Cyc_YYSTYPE*)_TE7F;_TE81=
# 2669 "parse.y"
Cyc_yyget_Exp_tok(_TE80);_TE82=yyyvsp;_TE83=& _TE82[4].v;_TE84=(union Cyc_YYSTYPE*)_TE83;_TE85=Cyc_yyget_Stmt_tok(_TE84);_TE86=yyyvsp;_TE87=_TE86[0];_TE88=_TE87.l;_TE89=_TE88.first_line;_TE8A=Cyc_Position_loc_to_seg(_TE89);_TE8B=Cyc_Absyn_while_stmt(_TE81,_TE85,_TE8A);yyval=Cyc_Stmt_tok(_TE8B);goto _LL0;case 375: _TE8C=yyyvsp;_TE8D=& _TE8C[1].v;_TE8E=(union Cyc_YYSTYPE*)_TE8D;_TE8F=
# 2671 "parse.y"
Cyc_yyget_Stmt_tok(_TE8E);_TE90=yyyvsp;_TE91=& _TE90[4].v;_TE92=(union Cyc_YYSTYPE*)_TE91;_TE93=Cyc_yyget_Exp_tok(_TE92);_TE94=yyyvsp;_TE95=_TE94[0];_TE96=_TE95.l;_TE97=_TE96.first_line;_TE98=Cyc_Position_loc_to_seg(_TE97);_TE99=Cyc_Absyn_do_stmt(_TE8F,_TE93,_TE98);yyval=Cyc_Stmt_tok(_TE99);goto _LL0;case 376: _TE9A=yyyvsp;_TE9B=& _TE9A[2].v;_TE9C=(union Cyc_YYSTYPE*)_TE9B;_TE9D=
# 2673 "parse.y"
Cyc_yyget_Exp_tok(_TE9C);_TE9E=yyyvsp;_TE9F=& _TE9E[4].v;_TEA0=(union Cyc_YYSTYPE*)_TE9F;_TEA1=Cyc_yyget_Exp_tok(_TEA0);_TEA2=yyyvsp;_TEA3=& _TEA2[6].v;_TEA4=(union Cyc_YYSTYPE*)_TEA3;_TEA5=Cyc_yyget_Exp_tok(_TEA4);_TEA6=yyyvsp;_TEA7=& _TEA6[8].v;_TEA8=(union Cyc_YYSTYPE*)_TEA7;_TEA9=Cyc_yyget_Stmt_tok(_TEA8);_TEAA=yyyvsp;_TEAB=_TEAA[0];_TEAC=_TEAB.l;_TEAD=_TEAC.first_line;_TEAE=Cyc_Position_loc_to_seg(_TEAD);_TEAF=Cyc_Absyn_for_stmt(_TE9D,_TEA1,_TEA5,_TEA9,_TEAE);yyval=Cyc_Stmt_tok(_TEAF);goto _LL0;case 377: _TEB0=
# 2675 "parse.y"
Cyc_Absyn_false_exp(0U);_TEB1=yyyvsp;_TEB2=& _TEB1[3].v;_TEB3=(union Cyc_YYSTYPE*)_TEB2;_TEB4=Cyc_yyget_Exp_tok(_TEB3);_TEB5=yyyvsp;_TEB6=& _TEB5[5].v;_TEB7=(union Cyc_YYSTYPE*)_TEB6;_TEB8=Cyc_yyget_Exp_tok(_TEB7);_TEB9=yyyvsp;_TEBA=& _TEB9[7].v;_TEBB=(union Cyc_YYSTYPE*)_TEBA;_TEBC=Cyc_yyget_Stmt_tok(_TEBB);_TEBD=yyyvsp;_TEBE=_TEBD[0];_TEBF=_TEBE.l;_TEC0=_TEBF.first_line;_TEC1=Cyc_Position_loc_to_seg(_TEC0);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_for_stmt(_TEB0,_TEB4,_TEB8,_TEBC,_TEC1);_TEC2=yyyvsp;_TEC3=& _TEC2[2].v;_TEC4=(union Cyc_YYSTYPE*)_TEC3;_TEC5=
Cyc_yyget_YY16(_TEC4);_TEC6=s;_TEC7=Cyc_Parse_flatten_declarations(_TEC5,_TEC6);yyval=Cyc_Stmt_tok(_TEC7);goto _LL0;}case 378: _TEC8=
# 2679
Cyc_Absyn_true_exp(0U);yyval=Cyc_Exp_tok(_TEC8);goto _LL0;case 379: _TEC9=yyyvsp;_TECA=_TEC9[0];
# 2680 "parse.y"
yyval=_TECA.v;goto _LL0;case 380:{struct _fat_ptr*_T1870=_cycalloc(sizeof(struct _fat_ptr));_TECC=yyyvsp;_TECD=& _TECC[1].v;_TECE=(union Cyc_YYSTYPE*)_TECD;
# 2683
*_T1870=Cyc_yyget_String_tok(_TECE);_TECB=(struct _fat_ptr*)_T1870;}_TECF=yyyvsp;_TED0=_TECF[0];_TED1=_TED0.l;_TED2=_TED1.first_line;_TED3=Cyc_Position_loc_to_seg(_TED2);_TED4=Cyc_Absyn_goto_stmt(_TECB,_TED3);yyval=Cyc_Stmt_tok(_TED4);goto _LL0;case 381: _TED5=yyyvsp;_TED6=_TED5[0];_TED7=_TED6.l;_TED8=_TED7.first_line;_TED9=
# 2684 "parse.y"
Cyc_Position_loc_to_seg(_TED8);_TEDA=Cyc_Absyn_continue_stmt(_TED9);yyval=Cyc_Stmt_tok(_TEDA);goto _LL0;case 382: _TEDB=yyyvsp;_TEDC=_TEDB[0];_TEDD=_TEDC.l;_TEDE=_TEDD.first_line;_TEDF=
# 2685 "parse.y"
Cyc_Position_loc_to_seg(_TEDE);_TEE0=Cyc_Absyn_break_stmt(_TEDF);yyval=Cyc_Stmt_tok(_TEE0);goto _LL0;case 383: _TEE1=yyyvsp;_TEE2=_TEE1[0];_TEE3=_TEE2.l;_TEE4=_TEE3.first_line;_TEE5=
# 2686 "parse.y"
Cyc_Position_loc_to_seg(_TEE4);_TEE6=Cyc_Absyn_return_stmt(0,_TEE5);yyval=Cyc_Stmt_tok(_TEE6);goto _LL0;case 384: _TEE7=yyyvsp;_TEE8=& _TEE7[1].v;_TEE9=(union Cyc_YYSTYPE*)_TEE8;_TEEA=
# 2687 "parse.y"
Cyc_yyget_Exp_tok(_TEE9);_TEEB=yyyvsp;_TEEC=_TEEB[0];_TEED=_TEEC.l;_TEEE=_TEED.first_line;_TEEF=Cyc_Position_loc_to_seg(_TEEE);_TEF0=Cyc_Absyn_return_stmt(_TEEA,_TEEF);yyval=Cyc_Stmt_tok(_TEF0);goto _LL0;case 385: _TEF1=yyyvsp;_TEF2=_TEF1[0];_TEF3=_TEF2.l;_TEF4=_TEF3.first_line;_TEF5=
# 2689 "parse.y"
Cyc_Position_loc_to_seg(_TEF4);_TEF6=Cyc_Absyn_fallthru_stmt(0,_TEF5);yyval=Cyc_Stmt_tok(_TEF6);goto _LL0;case 386: _TEF7=yyyvsp;_TEF8=_TEF7[0];_TEF9=_TEF8.l;_TEFA=_TEF9.first_line;_TEFB=
# 2690 "parse.y"
Cyc_Position_loc_to_seg(_TEFA);_TEFC=Cyc_Absyn_fallthru_stmt(0,_TEFB);yyval=Cyc_Stmt_tok(_TEFC);goto _LL0;case 387: _TEFD=yyyvsp;_TEFE=& _TEFD[2].v;_TEFF=(union Cyc_YYSTYPE*)_TEFE;_TF00=
# 2692 "parse.y"
Cyc_yyget_YY4(_TEFF);_TF01=yyyvsp;_TF02=_TF01[0];_TF03=_TF02.l;_TF04=_TF03.first_line;_TF05=Cyc_Position_loc_to_seg(_TF04);_TF06=Cyc_Absyn_fallthru_stmt(_TF00,_TF05);yyval=Cyc_Stmt_tok(_TF06);goto _LL0;case 388: _TF07=yyyvsp;_TF08=_TF07[0];
# 2701 "parse.y"
yyval=_TF08.v;goto _LL0;case 389: _TF09=yyyvsp;_TF0A=_TF09[0];
# 2704
yyval=_TF0A.v;goto _LL0;case 390: _TF0B=yyyvsp;_TF0C=& _TF0B[0].v;_TF0D=(union Cyc_YYSTYPE*)_TF0C;_TF0E=
# 2706 "parse.y"
Cyc_yyget_YY9(_TF0D);_TF0F=Cyc_Parse_pat2exp(_TF0E);_TF10=yyyvsp;_TF11=& _TF10[2].v;_TF12=(union Cyc_YYSTYPE*)_TF11;_TF13=Cyc_yyget_Exp_tok(_TF12);_TF14=yyyvsp;_TF15=& _TF14[4].v;_TF16=(union Cyc_YYSTYPE*)_TF15;_TF17=Cyc_yyget_Exp_tok(_TF16);_TF18=yyyvsp;_TF19=_TF18[0];_TF1A=_TF19.l;_TF1B=_TF1A.first_line;_TF1C=Cyc_Position_loc_to_seg(_TF1B);_TF1D=Cyc_Absyn_conditional_exp(_TF0F,_TF13,_TF17,_TF1C);_TF1E=Cyc_Absyn_exp_pat(_TF1D);yyval=Cyc_YY9(_TF1E);goto _LL0;case 391: _TF1F=yyyvsp;_TF20=_TF1F[0];
# 2709
yyval=_TF20.v;goto _LL0;case 392: _TF21=yyyvsp;_TF22=& _TF21[0].v;_TF23=(union Cyc_YYSTYPE*)_TF22;_TF24=
# 2711 "parse.y"
Cyc_yyget_YY9(_TF23);_TF25=Cyc_Parse_pat2exp(_TF24);_TF26=yyyvsp;_TF27=& _TF26[2].v;_TF28=(union Cyc_YYSTYPE*)_TF27;_TF29=Cyc_yyget_Exp_tok(_TF28);_TF2A=yyyvsp;_TF2B=_TF2A[0];_TF2C=_TF2B.l;_TF2D=_TF2C.first_line;_TF2E=Cyc_Position_loc_to_seg(_TF2D);_TF2F=Cyc_Absyn_or_exp(_TF25,_TF29,_TF2E);_TF30=Cyc_Absyn_exp_pat(_TF2F);yyval=Cyc_YY9(_TF30);goto _LL0;case 393: _TF31=yyyvsp;_TF32=_TF31[0];
# 2714
yyval=_TF32.v;goto _LL0;case 394: _TF33=yyyvsp;_TF34=& _TF33[0].v;_TF35=(union Cyc_YYSTYPE*)_TF34;_TF36=
# 2716 "parse.y"
Cyc_yyget_YY9(_TF35);_TF37=Cyc_Parse_pat2exp(_TF36);_TF38=yyyvsp;_TF39=& _TF38[2].v;_TF3A=(union Cyc_YYSTYPE*)_TF39;_TF3B=Cyc_yyget_Exp_tok(_TF3A);_TF3C=yyyvsp;_TF3D=_TF3C[0];_TF3E=_TF3D.l;_TF3F=_TF3E.first_line;_TF40=Cyc_Position_loc_to_seg(_TF3F);_TF41=Cyc_Absyn_and_exp(_TF37,_TF3B,_TF40);_TF42=Cyc_Absyn_exp_pat(_TF41);yyval=Cyc_YY9(_TF42);goto _LL0;case 395: _TF43=yyyvsp;_TF44=_TF43[0];
# 2719
yyval=_TF44.v;goto _LL0;case 396: _TF45=yyyvsp;_TF46=& _TF45[0].v;_TF47=(union Cyc_YYSTYPE*)_TF46;_TF48=
# 2721 "parse.y"
Cyc_yyget_YY9(_TF47);_TF49=Cyc_Parse_pat2exp(_TF48);_TF4A=yyyvsp;_TF4B=& _TF4A[2].v;_TF4C=(union Cyc_YYSTYPE*)_TF4B;_TF4D=Cyc_yyget_Exp_tok(_TF4C);_TF4E=yyyvsp;_TF4F=_TF4E[0];_TF50=_TF4F.l;_TF51=_TF50.first_line;_TF52=Cyc_Position_loc_to_seg(_TF51);_TF53=Cyc_Absyn_prim2_exp(14U,_TF49,_TF4D,_TF52);_TF54=Cyc_Absyn_exp_pat(_TF53);yyval=Cyc_YY9(_TF54);goto _LL0;case 397: _TF55=yyyvsp;_TF56=_TF55[0];
# 2724
yyval=_TF56.v;goto _LL0;case 398: _TF57=yyyvsp;_TF58=& _TF57[0].v;_TF59=(union Cyc_YYSTYPE*)_TF58;_TF5A=
# 2726 "parse.y"
Cyc_yyget_YY9(_TF59);_TF5B=Cyc_Parse_pat2exp(_TF5A);_TF5C=yyyvsp;_TF5D=& _TF5C[2].v;_TF5E=(union Cyc_YYSTYPE*)_TF5D;_TF5F=Cyc_yyget_Exp_tok(_TF5E);_TF60=yyyvsp;_TF61=_TF60[0];_TF62=_TF61.l;_TF63=_TF62.first_line;_TF64=Cyc_Position_loc_to_seg(_TF63);_TF65=Cyc_Absyn_prim2_exp(15U,_TF5B,_TF5F,_TF64);_TF66=Cyc_Absyn_exp_pat(_TF65);yyval=Cyc_YY9(_TF66);goto _LL0;case 399: _TF67=yyyvsp;_TF68=_TF67[0];
# 2729
yyval=_TF68.v;goto _LL0;case 400: _TF69=yyyvsp;_TF6A=& _TF69[0].v;_TF6B=(union Cyc_YYSTYPE*)_TF6A;_TF6C=
# 2731 "parse.y"
Cyc_yyget_YY9(_TF6B);_TF6D=Cyc_Parse_pat2exp(_TF6C);_TF6E=yyyvsp;_TF6F=& _TF6E[2].v;_TF70=(union Cyc_YYSTYPE*)_TF6F;_TF71=Cyc_yyget_Exp_tok(_TF70);_TF72=yyyvsp;_TF73=_TF72[0];_TF74=_TF73.l;_TF75=_TF74.first_line;_TF76=Cyc_Position_loc_to_seg(_TF75);_TF77=Cyc_Absyn_prim2_exp(13U,_TF6D,_TF71,_TF76);_TF78=Cyc_Absyn_exp_pat(_TF77);yyval=Cyc_YY9(_TF78);goto _LL0;case 401: _TF79=yyyvsp;_TF7A=_TF79[0];
# 2734
yyval=_TF7A.v;goto _LL0;case 402: _TF7B=yyyvsp;_TF7C=& _TF7B[1].v;_TF7D=(union Cyc_YYSTYPE*)_TF7C;_TF7E=
# 2736 "parse.y"
Cyc_yyget_YY69(_TF7D);_TF7F=yyyvsp;_TF80=& _TF7F[0].v;_TF81=(union Cyc_YYSTYPE*)_TF80;_TF82=Cyc_yyget_YY9(_TF81);_TF83=Cyc_Parse_pat2exp(_TF82);_TF84=yyyvsp;_TF85=& _TF84[2].v;_TF86=(union Cyc_YYSTYPE*)_TF85;_TF87=Cyc_yyget_Exp_tok(_TF86);_TF88=yyyvsp;_TF89=_TF88[0];_TF8A=_TF89.l;_TF8B=_TF8A.first_line;_TF8C=Cyc_Position_loc_to_seg(_TF8B);_TF8D=_TF7E(_TF83,_TF87,_TF8C);_TF8E=Cyc_Absyn_exp_pat(_TF8D);yyval=Cyc_YY9(_TF8E);goto _LL0;case 403: _TF8F=yyyvsp;_TF90=_TF8F[0];
# 2739
yyval=_TF90.v;goto _LL0;case 404: _TF91=yyyvsp;_TF92=& _TF91[1].v;_TF93=(union Cyc_YYSTYPE*)_TF92;_TF94=
# 2741 "parse.y"
Cyc_yyget_YY69(_TF93);_TF95=yyyvsp;_TF96=& _TF95[0].v;_TF97=(union Cyc_YYSTYPE*)_TF96;_TF98=Cyc_yyget_YY9(_TF97);_TF99=Cyc_Parse_pat2exp(_TF98);_TF9A=yyyvsp;_TF9B=& _TF9A[2].v;_TF9C=(union Cyc_YYSTYPE*)_TF9B;_TF9D=Cyc_yyget_Exp_tok(_TF9C);_TF9E=yyyvsp;_TF9F=_TF9E[0];_TFA0=_TF9F.l;_TFA1=_TFA0.first_line;_TFA2=Cyc_Position_loc_to_seg(_TFA1);_TFA3=_TF94(_TF99,_TF9D,_TFA2);_TFA4=Cyc_Absyn_exp_pat(_TFA3);yyval=Cyc_YY9(_TFA4);goto _LL0;case 405: _TFA5=yyyvsp;_TFA6=_TFA5[0];
# 2744
yyval=_TFA6.v;goto _LL0;case 406: _TFA7=yyyvsp;_TFA8=& _TFA7[0].v;_TFA9=(union Cyc_YYSTYPE*)_TFA8;_TFAA=
# 2746 "parse.y"
Cyc_yyget_YY9(_TFA9);_TFAB=Cyc_Parse_pat2exp(_TFAA);_TFAC=yyyvsp;_TFAD=& _TFAC[2].v;_TFAE=(union Cyc_YYSTYPE*)_TFAD;_TFAF=Cyc_yyget_Exp_tok(_TFAE);_TFB0=yyyvsp;_TFB1=_TFB0[0];_TFB2=_TFB1.l;_TFB3=_TFB2.first_line;_TFB4=Cyc_Position_loc_to_seg(_TFB3);_TFB5=Cyc_Absyn_prim2_exp(16U,_TFAB,_TFAF,_TFB4);_TFB6=Cyc_Absyn_exp_pat(_TFB5);yyval=Cyc_YY9(_TFB6);goto _LL0;case 407: _TFB7=yyyvsp;_TFB8=& _TFB7[0].v;_TFB9=(union Cyc_YYSTYPE*)_TFB8;_TFBA=
# 2748 "parse.y"
Cyc_yyget_YY9(_TFB9);_TFBB=Cyc_Parse_pat2exp(_TFBA);_TFBC=yyyvsp;_TFBD=& _TFBC[2].v;_TFBE=(union Cyc_YYSTYPE*)_TFBD;_TFBF=Cyc_yyget_Exp_tok(_TFBE);_TFC0=yyyvsp;_TFC1=_TFC0[0];_TFC2=_TFC1.l;_TFC3=_TFC2.first_line;_TFC4=Cyc_Position_loc_to_seg(_TFC3);_TFC5=Cyc_Absyn_prim2_exp(17U,_TFBB,_TFBF,_TFC4);_TFC6=Cyc_Absyn_exp_pat(_TFC5);yyval=Cyc_YY9(_TFC6);goto _LL0;case 408: _TFC7=yyyvsp;_TFC8=_TFC7[0];
# 2751
yyval=_TFC8.v;goto _LL0;case 409: _TFC9=yyyvsp;_TFCA=& _TFC9[1].v;_TFCB=(union Cyc_YYSTYPE*)_TFCA;_TFCC=
# 2753 "parse.y"
Cyc_yyget_YY6(_TFCB);_TFCD=yyyvsp;_TFCE=& _TFCD[0].v;_TFCF=(union Cyc_YYSTYPE*)_TFCE;_TFD0=Cyc_yyget_YY9(_TFCF);_TFD1=Cyc_Parse_pat2exp(_TFD0);_TFD2=yyyvsp;_TFD3=& _TFD2[2].v;_TFD4=(union Cyc_YYSTYPE*)_TFD3;_TFD5=Cyc_yyget_Exp_tok(_TFD4);_TFD6=yyyvsp;_TFD7=_TFD6[0];_TFD8=_TFD7.l;_TFD9=_TFD8.first_line;_TFDA=Cyc_Position_loc_to_seg(_TFD9);_TFDB=Cyc_Absyn_prim2_exp(_TFCC,_TFD1,_TFD5,_TFDA);_TFDC=Cyc_Absyn_exp_pat(_TFDB);yyval=Cyc_YY9(_TFDC);goto _LL0;case 410: _TFDD=yyyvsp;_TFDE=_TFDD[0];
# 2756
yyval=_TFDE.v;goto _LL0;case 411: _TFDF=yyyvsp;_TFE0=& _TFDF[1].v;_TFE1=(union Cyc_YYSTYPE*)_TFE0;_TFE2=
# 2758 "parse.y"
Cyc_yyget_YY6(_TFE1);_TFE3=yyyvsp;_TFE4=& _TFE3[0].v;_TFE5=(union Cyc_YYSTYPE*)_TFE4;_TFE6=Cyc_yyget_YY9(_TFE5);_TFE7=Cyc_Parse_pat2exp(_TFE6);_TFE8=yyyvsp;_TFE9=& _TFE8[2].v;_TFEA=(union Cyc_YYSTYPE*)_TFE9;_TFEB=Cyc_yyget_Exp_tok(_TFEA);_TFEC=yyyvsp;_TFED=_TFEC[0];_TFEE=_TFED.l;_TFEF=_TFEE.first_line;_TFF0=Cyc_Position_loc_to_seg(_TFEF);_TFF1=Cyc_Absyn_prim2_exp(_TFE2,_TFE7,_TFEB,_TFF0);_TFF2=Cyc_Absyn_exp_pat(_TFF1);yyval=Cyc_YY9(_TFF2);goto _LL0;case 412: _TFF3=yyyvsp;_TFF4=_TFF3[0];
# 2761
yyval=_TFF4.v;goto _LL0;case 413: _TFF5=yyyvsp;_TFF6=& _TFF5[1].v;_TFF7=(union Cyc_YYSTYPE*)_TFF6;_TFF8=
# 2763 "parse.y"
Cyc_yyget_YY38(_TFF7);_TFF9=yyyvsp;_TFFA=_TFF9[1];_TFFB=_TFFA.l;_TFFC=_TFFB.first_line;_TFFD=Cyc_Position_loc_to_seg(_TFFC);{void*t=Cyc_Parse_type_name_to_type(_TFF8,_TFFD);_TFFE=t;_TFFF=yyyvsp;_T1000=& _TFFF[3].v;_T1001=(union Cyc_YYSTYPE*)_T1000;_T1002=
Cyc_yyget_Exp_tok(_T1001);_T1003=yyyvsp;_T1004=_T1003[0];_T1005=_T1004.l;_T1006=_T1005.first_line;_T1007=Cyc_Position_loc_to_seg(_T1006);_T1008=Cyc_Absyn_cast_exp(_TFFE,_T1002,1,0U,_T1007);_T1009=Cyc_Absyn_exp_pat(_T1008);yyval=Cyc_YY9(_T1009);goto _LL0;}case 414: _T100A=yyyvsp;_T100B=_T100A[0];
# 2769 "parse.y"
yyval=_T100B.v;goto _LL0;case 415: _T100C=yyyvsp;_T100D=& _T100C[0].v;_T100E=(union Cyc_YYSTYPE*)_T100D;_T100F=
# 2773 "parse.y"
Cyc_yyget_YY6(_T100E);_T1010=yyyvsp;_T1011=& _T1010[1].v;_T1012=(union Cyc_YYSTYPE*)_T1011;_T1013=Cyc_yyget_Exp_tok(_T1012);_T1014=yyyvsp;_T1015=_T1014[0];_T1016=_T1015.l;_T1017=_T1016.first_line;_T1018=Cyc_Position_loc_to_seg(_T1017);_T1019=Cyc_Absyn_prim1_exp(_T100F,_T1013,_T1018);_T101A=Cyc_Absyn_exp_pat(_T1019);yyval=Cyc_YY9(_T101A);goto _LL0;case 416: _T101B=yyyvsp;_T101C=& _T101B[2].v;_T101D=(union Cyc_YYSTYPE*)_T101C;_T101E=
# 2775 "parse.y"
Cyc_yyget_YY38(_T101D);_T101F=yyyvsp;_T1020=_T101F[2];_T1021=_T1020.l;_T1022=_T1021.first_line;_T1023=Cyc_Position_loc_to_seg(_T1022);{void*t=Cyc_Parse_type_name_to_type(_T101E,_T1023);_T1024=t;_T1025=yyyvsp;_T1026=_T1025[0];_T1027=_T1026.l;_T1028=_T1027.first_line;_T1029=
Cyc_Position_loc_to_seg(_T1028);_T102A=Cyc_Absyn_sizeoftype_exp(_T1024,_T1029);_T102B=Cyc_Absyn_exp_pat(_T102A);yyval=Cyc_YY9(_T102B);goto _LL0;}case 417: _T102C=yyyvsp;_T102D=& _T102C[1].v;_T102E=(union Cyc_YYSTYPE*)_T102D;_T102F=
# 2778 "parse.y"
Cyc_yyget_Exp_tok(_T102E);_T1030=yyyvsp;_T1031=_T1030[0];_T1032=_T1031.l;_T1033=_T1032.first_line;_T1034=Cyc_Position_loc_to_seg(_T1033);_T1035=Cyc_Absyn_sizeofexp_exp(_T102F,_T1034);_T1036=Cyc_Absyn_exp_pat(_T1035);yyval=Cyc_YY9(_T1036);goto _LL0;case 418: _T1037=yyyvsp;_T1038=& _T1037[2].v;_T1039=(union Cyc_YYSTYPE*)_T1038;_T103A=
# 2780 "parse.y"
Cyc_yyget_YY38(_T1039);_T103B=*_T103A;_T103C=_T103B.f2;_T103D=yyyvsp;_T103E=& _T103D[4].v;_T103F=(union Cyc_YYSTYPE*)_T103E;_T1040=Cyc_yyget_YY3(_T103F);_T1041=Cyc_List_imp_rev(_T1040);_T1042=yyyvsp;_T1043=_T1042[0];_T1044=_T1043.l;_T1045=_T1044.first_line;_T1046=Cyc_Position_loc_to_seg(_T1045);_T1047=Cyc_Absyn_offsetof_exp(_T103C,_T1041,_T1046);_T1048=Cyc_Absyn_exp_pat(_T1047);yyval=Cyc_YY9(_T1048);goto _LL0;case 419: _T1049=yyyvsp;_T104A=_T1049[0];
# 2785 "parse.y"
yyval=_T104A.v;goto _LL0;case 420: _T104B=yyyvsp;_T104C=_T104B[0];
# 2793 "parse.y"
yyval=_T104C.v;goto _LL0;case 421: _T104D=& Cyc_Absyn_Wild_p_val;_T104E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T104D;_T104F=(void*)_T104E;_T1050=yyyvsp;_T1051=_T1050[0];_T1052=_T1051.l;_T1053=_T1052.first_line;_T1054=
# 2797 "parse.y"
Cyc_Position_loc_to_seg(_T1053);_T1055=Cyc_Absyn_new_pat(_T104F,_T1054);yyval=Cyc_YY9(_T1055);goto _LL0;case 422: _T1056=yyyvsp;_T1057=& _T1056[1].v;_T1058=(union Cyc_YYSTYPE*)_T1057;_T1059=
# 2798 "parse.y"
Cyc_yyget_Exp_tok(_T1058);_T105A=Cyc_Absyn_exp_pat(_T1059);yyval=Cyc_YY9(_T105A);goto _LL0;case 423:{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_T1870->tag=14;_T105C=yyyvsp;_T105D=& _T105C[0].v;_T105E=(union Cyc_YYSTYPE*)_T105D;
# 2799 "parse.y"
_T1870->f1=Cyc_yyget_QualId_tok(_T105E);_T105B=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T1870;}_T105F=(void*)_T105B;_T1060=yyyvsp;_T1061=_T1060[0];_T1062=_T1061.l;_T1063=_T1062.first_line;_T1064=Cyc_Position_loc_to_seg(_T1063);_T1065=Cyc_Absyn_new_pat(_T105F,_T1064);yyval=Cyc_YY9(_T1065);goto _LL0;case 424:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1870->tag=5;_T1067=yyyvsp;_T1068=& _T1067[1].v;_T1069=(union Cyc_YYSTYPE*)_T1068;
# 2800 "parse.y"
_T1870->f1=Cyc_yyget_YY9(_T1069);_T1066=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1870;}_T106A=(void*)_T1066;_T106B=yyyvsp;_T106C=_T106B[0];_T106D=_T106C.l;_T106E=_T106D.first_line;_T106F=Cyc_Position_loc_to_seg(_T106E);_T1070=Cyc_Absyn_new_pat(_T106A,_T106F);yyval=Cyc_YY9(_T1070);goto _LL0;case 425: _T1071=yyyvsp;_T1072=& _T1071[0].v;_T1073=(union Cyc_YYSTYPE*)_T1072;{
# 2802 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(_T1073);_T1074=e;{
void*_T1870=_T1074->r;struct _fat_ptr _T1871;int _T1872;short _T1873;char _T1874;enum Cyc_Absyn_Sign _T1875;_T1075=(int*)_T1870;_T1076=*_T1075;if(_T1076!=0)goto _TL2E0;_T1077=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1870;_T1078=_T1077->f1;_T1079=_T1078.LongLong_c;_T107A=_T1079.tag;switch(_T107A){case 2:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1876=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1870;_T107B=_T1876->f1;_T107C=_T107B.Char_c;_T107D=_T107C.val;_T1875=_T107D.f0;_T107E=_T1876->f1;_T107F=_T107E.Char_c;_T1080=_T107F.val;_T1874=_T1080.f1;}{enum Cyc_Absyn_Sign s=_T1875;char i=_T1874;{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T1876=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_T1876->tag=10;
# 2805
_T1876->f1=i;_T1081=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T1876;}_T1082=(void*)_T1081;_T1083=e;_T1084=_T1083->loc;_T1085=Cyc_Absyn_new_pat(_T1082,_T1084);yyval=Cyc_YY9(_T1085);goto _LL537;}case 4:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1876=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1870;_T1086=_T1876->f1;_T1087=_T1086.Short_c;_T1088=_T1087.val;_T1875=_T1088.f0;_T1089=_T1876->f1;_T108A=_T1089.Short_c;_T108B=_T108A.val;_T1873=_T108B.f1;}{enum Cyc_Absyn_Sign s=_T1875;short i=_T1873;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1876=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T1876->tag=9;
# 2807
_T1876->f1=s;_T108D=i;_T1876->f2=(int)_T108D;_T108C=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T1876;}_T108E=(void*)_T108C;_T108F=e;_T1090=_T108F->loc;_T1091=Cyc_Absyn_new_pat(_T108E,_T1090);yyval=Cyc_YY9(_T1091);goto _LL537;}case 5:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1876=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1870;_T1092=_T1876->f1;_T1093=_T1092.Int_c;_T1094=_T1093.val;_T1875=_T1094.f0;_T1095=_T1876->f1;_T1096=_T1095.Int_c;_T1097=_T1096.val;_T1872=_T1097.f1;}{enum Cyc_Absyn_Sign s=_T1875;int i=_T1872;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1876=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T1876->tag=9;
# 2809
_T1876->f1=s;_T1876->f2=i;_T1098=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T1876;}_T1099=(void*)_T1098;_T109A=e;_T109B=_T109A->loc;_T109C=Cyc_Absyn_new_pat(_T1099,_T109B);yyval=Cyc_YY9(_T109C);goto _LL537;}case 7:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1876=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1870;_T109D=_T1876->f1;_T109E=_T109D.Float_c;_T109F=_T109E.val;_T1871=_T109F.f0;_T10A0=_T1876->f1;_T10A1=_T10A0.Float_c;_T10A2=_T10A1.val;_T1872=_T10A2.f1;}{struct _fat_ptr s=_T1871;int i=_T1872;{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T1876=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_T1876->tag=11;
# 2811
_T1876->f1=s;_T1876->f2=i;_T10A3=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T1876;}_T10A4=(void*)_T10A3;_T10A5=e;_T10A6=_T10A5->loc;_T10A7=Cyc_Absyn_new_pat(_T10A4,_T10A6);yyval=Cyc_YY9(_T10A7);goto _LL537;}case 1: _T10A8=& Cyc_Absyn_Null_p_val;_T10A9=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_T10A8;_T10AA=(void*)_T10A9;_T10AB=e;_T10AC=_T10AB->loc;_T10AD=
# 2813
Cyc_Absyn_new_pat(_T10AA,_T10AC);yyval=Cyc_YY9(_T10AD);goto _LL537;case 8: goto _LL545;case 9: _LL545: _T10AE=yyyvsp;_T10AF=_T10AE[0];_T10B0=_T10AF.l;_T10B1=_T10B0.first_line;_T10B2=
# 2816
Cyc_Position_loc_to_seg(_T10B1);_T10B3=_tag_fat("strings cannot occur within patterns",sizeof(char),37U);_T10B4=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10B2,_T10B3,_T10B4);goto _LL537;case 6: _T10B5=yyyvsp;_T10B6=_T10B5[0];_T10B7=_T10B6.l;_T10B8=_T10B7.first_line;_T10B9=
# 2818
Cyc_Position_loc_to_seg(_T10B8);_T10BA=_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U);_T10BB=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10B9,_T10BA,_T10BB);goto _LL537;default: goto _LL548;}goto _TL2E1;_TL2E0: _LL548: _T10BC=yyyvsp;_T10BD=_T10BC[0];_T10BE=_T10BD.l;_T10BF=_T10BE.first_line;_T10C0=
# 2820
Cyc_Position_loc_to_seg(_T10BF);_T10C1=_tag_fat("bad constant in case",sizeof(char),21U);_T10C2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10C0,_T10C1,_T10C2);_TL2E1: _LL537:;}goto _LL0;}case 426: _T10C3=yyyvsp;_T10C4=& _T10C3[1].v;_T10C5=(union Cyc_YYSTYPE*)_T10C4;_T10C6=
# 2824 "parse.y"
Cyc_yyget_String_tok(_T10C5);_T10C7=_tag_fat("as",sizeof(char),3U);_T10C8=Cyc_strcmp(_T10C6,_T10C7);if(_T10C8==0)goto _TL2E3;_T10C9=yyyvsp;_T10CA=_T10C9[1];_T10CB=_T10CA.l;_T10CC=_T10CB.first_line;_T10CD=
Cyc_Position_loc_to_seg(_T10CC);_T10CE=_tag_fat("expecting `as'",sizeof(char),15U);_T10CF=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10CD,_T10CE,_T10CF);goto _TL2E4;_TL2E3: _TL2E4:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T1870->tag=1;_T10D1=yyyvsp;_T10D2=_T10D1[0];_T10D3=_T10D2.l;_T10D4=_T10D3.first_line;_T10D5=
Cyc_Position_loc_to_seg(_T10D4);{struct _tuple0*_T1871=_cycalloc(sizeof(struct _tuple0));_T1871->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));_T10D8=yyyvsp;_T10D9=& _T10D8[0].v;_T10DA=(union Cyc_YYSTYPE*)_T10D9;*_T1872=Cyc_yyget_String_tok(_T10DA);_T10D7=(struct _fat_ptr*)_T1872;}_T1871->f1=_T10D7;_T10D6=(struct _tuple0*)_T1871;}_T10DB=Cyc_Absyn_void_type;_T1870->f1=Cyc_Absyn_new_vardecl(_T10D5,_T10D6,_T10DB,0,0);_T10DC=yyyvsp;_T10DD=& _T10DC[2].v;_T10DE=(union Cyc_YYSTYPE*)_T10DD;
_T1870->f2=Cyc_yyget_YY9(_T10DE);_T10D0=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T1870;}_T10DF=(void*)_T10D0;_T10E0=yyyvsp;_T10E1=_T10E0[0];_T10E2=_T10E1.l;_T10E3=_T10E2.first_line;_T10E4=Cyc_Position_loc_to_seg(_T10E3);_T10E5=
# 2826
Cyc_Absyn_new_pat(_T10DF,_T10E4);yyval=Cyc_YY9(_T10E5);goto _LL0;case 427: _T10E6=yyyvsp;_T10E7=& _T10E6[0].v;_T10E8=(union Cyc_YYSTYPE*)_T10E7;_T10E9=
# 2830 "parse.y"
Cyc_yyget_String_tok(_T10E8);_T10EA=_tag_fat("alias",sizeof(char),6U);_T10EB=Cyc_strcmp(_T10E9,_T10EA);if(_T10EB==0)goto _TL2E5;_T10EC=yyyvsp;_T10ED=_T10EC[1];_T10EE=_T10ED.l;_T10EF=_T10EE.first_line;_T10F0=
Cyc_Position_loc_to_seg(_T10EF);_T10F1=_tag_fat("expecting `alias'",sizeof(char),18U);_T10F2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10F0,_T10F1,_T10F2);goto _TL2E6;_TL2E5: _TL2E6: _T10F3=yyyvsp;_T10F4=_T10F3[0];_T10F5=_T10F4.l;_T10F6=_T10F5.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T10F6);_T10F7=yyyvsp;_T10F8=& _T10F7[2].v;_T10F9=(union Cyc_YYSTYPE*)_T10F8;_T10FA=
Cyc_yyget_String_tok(_T10F9);_T10FB=location;Cyc_Parse_tvar_ok(_T10FA,_T10FB);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T10FC=tv;{struct _fat_ptr*_T1870=_cycalloc(sizeof(struct _fat_ptr));_T10FE=yyyvsp;_T10FF=& _T10FE[2].v;_T1100=(union Cyc_YYSTYPE*)_T10FF;*_T1870=Cyc_yyget_String_tok(_T1100);_T10FD=(struct _fat_ptr*)_T1870;}_T10FC->name=_T10FD;_T1101=tv;_T1101->identity=-1;_T1102=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T1870->tag=0;_T1104=& Cyc_Kinds_ek;_T1870->f1=(struct Cyc_Absyn_Kind*)_T1104;_T1103=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1870;}_T1102->kind=(void*)_T1103;_T1105=tv;_T1105->aquals_bound=0;_T1106=yyyvsp;_T1107=_T1106[0];_T1108=_T1107.l;_T1109=_T1108.first_line;_T110A=
Cyc_Position_loc_to_seg(_T1109);{struct _tuple0*_T1870=_cycalloc(sizeof(struct _tuple0));_T1870->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T110D=yyyvsp;_T110E=& _T110D[5].v;_T110F=(union Cyc_YYSTYPE*)_T110E;*_T1871=Cyc_yyget_String_tok(_T110F);_T110C=(struct _fat_ptr*)_T1871;}_T1870->f1=_T110C;_T110B=(struct _tuple0*)_T1870;}_T1110=yyyvsp;_T1111=& _T1110[4].v;_T1112=(union Cyc_YYSTYPE*)_T1111;_T1113=
Cyc_yyget_YY38(_T1112);_T1114=yyyvsp;_T1115=_T1114[4];_T1116=_T1115.l;_T1117=_T1116.first_line;_T1118=Cyc_Position_loc_to_seg(_T1117);_T1119=Cyc_Parse_type_name_to_type(_T1113,_T1118);{
# 2835
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T110A,_T110B,_T1119,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T1870->tag=2;
# 2837
_T1870->f1=tv;_T1870->f2=vd;_T111A=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T1870;}_T111B=(void*)_T111A;_T111C=location;_T111D=Cyc_Absyn_new_pat(_T111B,_T111C);yyval=Cyc_YY9(_T111D);goto _LL0;}}}case 428: _T111E=yyyvsp;_T111F=& _T111E[0].v;_T1120=(union Cyc_YYSTYPE*)_T111F;_T1121=
# 2840 "parse.y"
Cyc_yyget_String_tok(_T1120);_T1122=_tag_fat("alias",sizeof(char),6U);_T1123=Cyc_strcmp(_T1121,_T1122);if(_T1123==0)goto _TL2E7;_T1124=yyyvsp;_T1125=_T1124[1];_T1126=_T1125.l;_T1127=_T1126.first_line;_T1128=
Cyc_Position_loc_to_seg(_T1127);_T1129=_tag_fat("expecting `alias'",sizeof(char),18U);_T112A=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1128,_T1129,_T112A);goto _TL2E8;_TL2E7: _TL2E8: _T112B=yyyvsp;_T112C=_T112B[0];_T112D=_T112C.l;_T112E=_T112D.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T112E);_T112F=yyyvsp;_T1130=& _T112F[2].v;_T1131=(union Cyc_YYSTYPE*)_T1130;_T1132=
Cyc_yyget_String_tok(_T1131);_T1133=location;Cyc_Parse_tvar_ok(_T1132,_T1133);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T1134=tv;{struct _fat_ptr*_T1870=_cycalloc(sizeof(struct _fat_ptr));_T1136=yyyvsp;_T1137=& _T1136[2].v;_T1138=(union Cyc_YYSTYPE*)_T1137;*_T1870=Cyc_yyget_String_tok(_T1138);_T1135=(struct _fat_ptr*)_T1870;}_T1134->name=_T1135;_T1139=tv;_T1139->identity=-1;_T113A=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T1870->tag=0;_T113C=& Cyc_Kinds_ek;_T1870->f1=(struct Cyc_Absyn_Kind*)_T113C;_T113B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1870;}_T113A->kind=(void*)_T113B;_T113D=tv;_T113D->aquals_bound=0;_T113E=yyyvsp;_T113F=_T113E[0];_T1140=_T113F.l;_T1141=_T1140.first_line;_T1142=
Cyc_Position_loc_to_seg(_T1141);{struct _tuple0*_T1870=_cycalloc(sizeof(struct _tuple0));_T1870->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T1145=yyyvsp;_T1146=& _T1145[5].v;_T1147=(union Cyc_YYSTYPE*)_T1146;*_T1871=Cyc_yyget_String_tok(_T1147);_T1144=(struct _fat_ptr*)_T1871;}_T1870->f1=_T1144;_T1143=(struct _tuple0*)_T1870;}_T1148=yyyvsp;_T1149=& _T1148[4].v;_T114A=(union Cyc_YYSTYPE*)_T1149;_T114B=
Cyc_yyget_YY38(_T114A);_T114C=yyyvsp;_T114D=_T114C[4];_T114E=_T114D.l;_T114F=_T114E.first_line;_T1150=Cyc_Position_loc_to_seg(_T114F);_T1151=Cyc_Parse_type_name_to_type(_T114B,_T1150);{
# 2845
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T1142,_T1143,_T1151,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T1870->tag=2;
# 2847
_T1870->f1=tv;_T1870->f2=vd;_T1152=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T1870;}_T1153=(void*)_T1152;_T1154=location;_T1155=Cyc_Absyn_new_pat(_T1153,_T1154);yyval=Cyc_YY9(_T1155);goto _LL0;}}}case 429: _T1156=yyyvsp;_T1157=& _T1156[2].v;_T1158=(union Cyc_YYSTYPE*)_T1157;_T1159=
# 2850 "parse.y"
Cyc_yyget_YY14(_T1158);{struct _tuple23 _T1870=*_T1159;int _T1871;struct Cyc_List_List*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_List_List*fps=_T1872;int dots=_T1871;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T1873->tag=6;
_T1873->f1=0;_T1873->f2=1;_T1873->f3=0;_T1873->f4=fps;_T1873->f5=dots;_T115A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T1873;}_T115B=(void*)_T115A;_T115C=yyyvsp;_T115D=_T115C[0];_T115E=_T115D.l;_T115F=_T115E.first_line;_T1160=Cyc_Position_loc_to_seg(_T115F);_T1161=Cyc_Absyn_new_pat(_T115B,_T1160);yyval=Cyc_YY9(_T1161);goto _LL0;}}case 430: _T1162=yyyvsp;_T1163=& _T1162[2].v;_T1164=(union Cyc_YYSTYPE*)_T1163;_T1165=
# 2854 "parse.y"
Cyc_yyget_YY10(_T1164);{struct _tuple23 _T1870=*_T1165;int _T1871;struct Cyc_List_List*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_List_List*ps=_T1872;int dots=_T1871;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_T1873->tag=15;_T1167=yyyvsp;_T1168=& _T1167[0].v;_T1169=(union Cyc_YYSTYPE*)_T1168;
_T1873->f1=Cyc_yyget_QualId_tok(_T1169);_T1873->f2=ps;_T1873->f3=dots;_T1166=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T1873;}_T116A=(void*)_T1166;_T116B=yyyvsp;_T116C=_T116B[0];_T116D=_T116C.l;_T116E=_T116D.first_line;_T116F=Cyc_Position_loc_to_seg(_T116E);_T1170=Cyc_Absyn_new_pat(_T116A,_T116F);yyval=Cyc_YY9(_T1170);goto _LL0;}}case 431: _T1171=yyyvsp;_T1172=& _T1171[3].v;_T1173=(union Cyc_YYSTYPE*)_T1172;_T1174=
# 2858 "parse.y"
Cyc_yyget_YY14(_T1173);{struct _tuple23 _T1870=*_T1174;int _T1871;struct Cyc_List_List*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_List_List*fps=_T1872;int dots=_T1871;_T1176=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1873)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T1176;_T1175=_T1873;}_T1177=yyyvsp;_T1178=_T1177[2];_T1179=_T1178.l;_T117A=_T1179.first_line;_T117B=Cyc_Position_loc_to_seg(_T117A);_T117C=yyyvsp;_T117D=& _T117C[2].v;_T117E=(union Cyc_YYSTYPE*)_T117D;_T117F=Cyc_yyget_YY41(_T117E);{struct Cyc_List_List*exist_ts=_T1175(Cyc_Parse_typ2tvar,_T117B,_T117F);_T1180=yyyvsp;_T1181=& _T1180[0].v;_T1182=(union Cyc_YYSTYPE*)_T1181;_T1183=
Cyc_yyget_QualId_tok(_T1182);{union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,_T1183,0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_T1184=typ;_T1184->tag=0;_T1185=typ;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T1873->tag=24;_T1873->f1=ai;_T1186=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T1873;}_T1185->f1=(void*)_T1186;_T1187=typ;_T1187->f2=0;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T1873->tag=6;_T1189=typ;
_T1873->f1=(void*)_T1189;_T1873->f2=0;_T1873->f3=exist_ts;_T1873->f4=fps;_T1873->f5=dots;_T1188=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T1873;}_T118A=(void*)_T1188;_T118B=yyyvsp;_T118C=_T118B[0];_T118D=_T118C.l;_T118E=_T118D.first_line;_T118F=Cyc_Position_loc_to_seg(_T118E);_T1190=Cyc_Absyn_new_pat(_T118A,_T118F);yyval=Cyc_YY9(_T1190);goto _LL0;}}}}case 432: _T1191=yyyvsp;_T1192=& _T1191[2].v;_T1193=(union Cyc_YYSTYPE*)_T1192;_T1194=
# 2865 "parse.y"
Cyc_yyget_YY14(_T1193);{struct _tuple23 _T1870=*_T1194;int _T1871;struct Cyc_List_List*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_List_List*fps=_T1872;int dots=_T1871;_T1196=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1873)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T1196;_T1195=_T1873;}_T1197=yyyvsp;_T1198=_T1197[1];_T1199=_T1198.l;_T119A=_T1199.first_line;_T119B=Cyc_Position_loc_to_seg(_T119A);_T119C=yyyvsp;_T119D=& _T119C[1].v;_T119E=(union Cyc_YYSTYPE*)_T119D;_T119F=Cyc_yyget_YY41(_T119E);{struct Cyc_List_List*exist_ts=_T1195(Cyc_Parse_typ2tvar,_T119B,_T119F);{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T1873->tag=6;
_T1873->f1=0;_T1873->f2=0;_T1873->f3=exist_ts;_T1873->f4=fps;_T1873->f5=dots;_T11A0=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T1873;}_T11A1=(void*)_T11A0;_T11A2=yyyvsp;_T11A3=_T11A2[0];_T11A4=_T11A3.l;_T11A5=_T11A4.first_line;_T11A6=Cyc_Position_loc_to_seg(_T11A5);_T11A7=Cyc_Absyn_new_pat(_T11A1,_T11A6);yyval=Cyc_YY9(_T11A7);goto _LL0;}}}case 433:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1870->tag=5;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1871->tag=5;_T11AA=yyyvsp;_T11AB=& _T11AA[1].v;_T11AC=(union Cyc_YYSTYPE*)_T11AB;
# 2870 "parse.y"
_T1871->f1=Cyc_yyget_YY9(_T11AC);_T11A9=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1871;}_T11AD=(void*)_T11A9;_T11AE=yyyvsp;_T11AF=_T11AE[0];_T11B0=_T11AF.l;_T11B1=_T11B0.first_line;_T11B2=Cyc_Position_loc_to_seg(_T11B1);_T1870->f1=Cyc_Absyn_new_pat(_T11AD,_T11B2);_T11A8=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1870;}_T11B3=(void*)_T11A8;_T11B4=yyyvsp;_T11B5=_T11B4[0];_T11B6=_T11B5.l;_T11B7=_T11B6.first_line;_T11B8=Cyc_Position_loc_to_seg(_T11B7);_T11B9=Cyc_Absyn_new_pat(_T11B3,_T11B8);yyval=Cyc_YY9(_T11B9);goto _LL0;case 434:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T1870->tag=3;_T11BB=yyyvsp;_T11BC=_T11BB[0];_T11BD=_T11BC.l;_T11BE=_T11BD.first_line;_T11BF=
# 2872 "parse.y"
Cyc_Position_loc_to_seg(_T11BE);{struct _tuple0*_T1871=_cycalloc(sizeof(struct _tuple0));_T1871->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));_T11C2=yyyvsp;_T11C3=& _T11C2[1].v;_T11C4=(union Cyc_YYSTYPE*)_T11C3;*_T1872=Cyc_yyget_String_tok(_T11C4);_T11C1=(struct _fat_ptr*)_T1872;}_T1871->f1=_T11C1;_T11C0=(struct _tuple0*)_T1871;}_T11C5=Cyc_Absyn_void_type;_T1870->f1=Cyc_Absyn_new_vardecl(_T11BF,_T11C0,_T11C5,0,0);_T11C6=& Cyc_Absyn_Wild_p_val;_T11C7=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T11C6;_T11C8=(void*)_T11C7;_T11C9=yyyvsp;_T11CA=_T11C9[1];_T11CB=_T11CA.l;_T11CC=_T11CB.first_line;_T11CD=
# 2874
Cyc_Position_loc_to_seg(_T11CC);_T1870->f2=Cyc_Absyn_new_pat(_T11C8,_T11CD);_T11BA=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T1870;}_T11CE=(void*)_T11BA;_T11CF=yyyvsp;_T11D0=_T11CF[0];_T11D1=_T11D0.l;_T11D2=_T11D1.first_line;_T11D3=
Cyc_Position_loc_to_seg(_T11D2);_T11D4=
# 2872
Cyc_Absyn_new_pat(_T11CE,_T11D3);yyval=Cyc_YY9(_T11D4);goto _LL0;case 435: _T11D5=yyyvsp;_T11D6=& _T11D5[2].v;_T11D7=(union Cyc_YYSTYPE*)_T11D6;_T11D8=
# 2877 "parse.y"
Cyc_yyget_String_tok(_T11D7);_T11D9=_tag_fat("as",sizeof(char),3U);_T11DA=Cyc_strcmp(_T11D8,_T11D9);if(_T11DA==0)goto _TL2E9;_T11DB=yyyvsp;_T11DC=_T11DB[2];_T11DD=_T11DC.l;_T11DE=_T11DD.first_line;_T11DF=
Cyc_Position_loc_to_seg(_T11DE);_T11E0=_tag_fat("expecting `as'",sizeof(char),15U);_T11E1=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T11DF,_T11E0,_T11E1);goto _TL2EA;_TL2E9: _TL2EA:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T1870->tag=3;_T11E3=yyyvsp;_T11E4=_T11E3[0];_T11E5=_T11E4.l;_T11E6=_T11E5.first_line;_T11E7=
Cyc_Position_loc_to_seg(_T11E6);{struct _tuple0*_T1871=_cycalloc(sizeof(struct _tuple0));_T1871->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));_T11EA=yyyvsp;_T11EB=& _T11EA[1].v;_T11EC=(union Cyc_YYSTYPE*)_T11EB;*_T1872=Cyc_yyget_String_tok(_T11EC);_T11E9=(struct _fat_ptr*)_T1872;}_T1871->f1=_T11E9;_T11E8=(struct _tuple0*)_T1871;}_T11ED=Cyc_Absyn_void_type;_T1870->f1=Cyc_Absyn_new_vardecl(_T11E7,_T11E8,_T11ED,0,0);_T11EE=yyyvsp;_T11EF=& _T11EE[3].v;_T11F0=(union Cyc_YYSTYPE*)_T11EF;
# 2881
_T1870->f2=Cyc_yyget_YY9(_T11F0);_T11E2=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T1870;}_T11F1=(void*)_T11E2;_T11F2=yyyvsp;_T11F3=_T11F2[0];_T11F4=_T11F3.l;_T11F5=_T11F4.first_line;_T11F6=Cyc_Position_loc_to_seg(_T11F5);_T11F7=
# 2879
Cyc_Absyn_new_pat(_T11F1,_T11F6);yyval=Cyc_YY9(_T11F7);goto _LL0;case 436: _T11F8=yyyvsp;_T11F9=& _T11F8[2].v;_T11FA=(union Cyc_YYSTYPE*)_T11F9;_T11FB=
# 2884 "parse.y"
Cyc_yyget_String_tok(_T11FA);_T11FC=& Cyc_Kinds_ik;_T11FD=(struct Cyc_Absyn_Kind*)_T11FC;_T11FE=Cyc_Kinds_kind_to_bound(_T11FD);_T11FF=yyyvsp;_T1200=_T11FF[2];_T1201=_T1200.l;_T1202=_T1201.first_line;_T1203=Cyc_Position_loc_to_seg(_T1202);{void*tag=Cyc_Parse_id2type(_T11FB,_T11FE,0,_T1203);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T1870->tag=4;_T1205=yyyvsp;_T1206=_T1205[2];_T1207=_T1206.l;_T1208=_T1207.first_line;_T1209=
Cyc_Position_loc_to_seg(_T1208);_T120A=tag;_T1870->f1=Cyc_Parse_typ2tvar(_T1209,_T120A);_T120B=yyyvsp;_T120C=_T120B[0];_T120D=_T120C.l;_T120E=_T120D.first_line;_T120F=
Cyc_Position_loc_to_seg(_T120E);{struct _tuple0*_T1871=_cycalloc(sizeof(struct _tuple0));_T1871->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));_T1212=yyyvsp;_T1213=& _T1212[0].v;_T1214=(union Cyc_YYSTYPE*)_T1213;*_T1872=Cyc_yyget_String_tok(_T1214);_T1211=(struct _fat_ptr*)_T1872;}_T1871->f1=_T1211;_T1210=(struct _tuple0*)_T1871;}_T1215=
Cyc_Absyn_tag_type(tag);
# 2886
_T1870->f2=Cyc_Absyn_new_vardecl(_T120F,_T1210,_T1215,0,0);_T1204=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T1870;}_T1216=(void*)_T1204;_T1217=yyyvsp;_T1218=_T1217[0];_T1219=_T1218.l;_T121A=_T1219.first_line;_T121B=
# 2888
Cyc_Position_loc_to_seg(_T121A);_T121C=
# 2885
Cyc_Absyn_new_pat(_T1216,_T121B);yyval=Cyc_YY9(_T121C);goto _LL0;}case 437: _T121D=& Cyc_Kinds_ik;_T121E=(struct Cyc_Absyn_Kind*)_T121D;_T121F=
# 2890 "parse.y"
Cyc_Kinds_kind_to_bound(_T121E);{struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(_T121F);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T1870->tag=4;
_T1870->f1=tv;_T1221=yyyvsp;_T1222=_T1221[0];_T1223=_T1222.l;_T1224=_T1223.first_line;_T1225=
Cyc_Position_loc_to_seg(_T1224);{struct _tuple0*_T1871=_cycalloc(sizeof(struct _tuple0));_T1871->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));_T1228=yyyvsp;_T1229=& _T1228[0].v;_T122A=(union Cyc_YYSTYPE*)_T1229;*_T1872=Cyc_yyget_String_tok(_T122A);_T1227=(struct _fat_ptr*)_T1872;}_T1871->f1=_T1227;_T1226=(struct _tuple0*)_T1871;}_T122B=
Cyc_Absyn_var_type(tv);_T122C=Cyc_Absyn_tag_type(_T122B);
# 2892
_T1870->f2=Cyc_Absyn_new_vardecl(_T1225,_T1226,_T122C,0,0);_T1220=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T1870;}_T122D=(void*)_T1220;_T122E=yyyvsp;_T122F=_T122E[0];_T1230=_T122F.l;_T1231=_T1230.first_line;_T1232=
# 2894
Cyc_Position_loc_to_seg(_T1231);_T1233=
# 2891
Cyc_Absyn_new_pat(_T122D,_T1232);yyval=Cyc_YY9(_T1233);goto _LL0;}case 438:{struct _tuple23*_T1870=_cycalloc(sizeof(struct _tuple23));_T1235=yyyvsp;_T1236=& _T1235[0].v;_T1237=(union Cyc_YYSTYPE*)_T1236;_T1238=
# 2898 "parse.y"
Cyc_yyget_YY11(_T1237);_T1870->f0=Cyc_List_rev(_T1238);_T1870->f1=0;_T1234=(struct _tuple23*)_T1870;}yyval=Cyc_YY10(_T1234);goto _LL0;case 439:{struct _tuple23*_T1870=_cycalloc(sizeof(struct _tuple23));_T123A=yyyvsp;_T123B=& _T123A[0].v;_T123C=(union Cyc_YYSTYPE*)_T123B;_T123D=
# 2899 "parse.y"
Cyc_yyget_YY11(_T123C);_T1870->f0=Cyc_List_rev(_T123D);_T1870->f1=1;_T1239=(struct _tuple23*)_T1870;}yyval=Cyc_YY10(_T1239);goto _LL0;case 440:{struct _tuple23*_T1870=_cycalloc(sizeof(struct _tuple23));
# 2900 "parse.y"
_T1870->f0=0;_T1870->f1=1;_T123E=(struct _tuple23*)_T1870;}yyval=Cyc_YY10(_T123E);goto _LL0;case 441:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T1240=yyyvsp;_T1241=& _T1240[0].v;_T1242=(union Cyc_YYSTYPE*)_T1241;
# 2903
_T1870->hd=Cyc_yyget_YY9(_T1242);_T1870->tl=0;_T123F=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY11(_T123F);goto _LL0;case 442:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T1244=yyyvsp;_T1245=& _T1244[2].v;_T1246=(union Cyc_YYSTYPE*)_T1245;
# 2904 "parse.y"
_T1870->hd=Cyc_yyget_YY9(_T1246);_T1247=yyyvsp;_T1248=& _T1247[0].v;_T1249=(union Cyc_YYSTYPE*)_T1248;_T1870->tl=Cyc_yyget_YY11(_T1249);_T1243=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY11(_T1243);goto _LL0;case 443:{struct _tuple24*_T1870=_cycalloc(sizeof(struct _tuple24));
# 2907
_T1870->f0=0;_T124B=yyyvsp;_T124C=& _T124B[0].v;_T124D=(union Cyc_YYSTYPE*)_T124C;_T1870->f1=Cyc_yyget_YY9(_T124D);_T124A=(struct _tuple24*)_T1870;}yyval=Cyc_YY12(_T124A);goto _LL0;case 444:{struct _tuple24*_T1870=_cycalloc(sizeof(struct _tuple24));_T124F=yyyvsp;_T1250=& _T124F[0].v;_T1251=(union Cyc_YYSTYPE*)_T1250;
# 2908 "parse.y"
_T1870->f0=Cyc_yyget_YY42(_T1251);_T1252=yyyvsp;_T1253=& _T1252[1].v;_T1254=(union Cyc_YYSTYPE*)_T1253;_T1870->f1=Cyc_yyget_YY9(_T1254);_T124E=(struct _tuple24*)_T1870;}yyval=Cyc_YY12(_T124E);goto _LL0;case 445:{struct _tuple23*_T1870=_cycalloc(sizeof(struct _tuple23));_T1256=yyyvsp;_T1257=& _T1256[0].v;_T1258=(union Cyc_YYSTYPE*)_T1257;_T1259=
# 2911
Cyc_yyget_YY13(_T1258);_T1870->f0=Cyc_List_rev(_T1259);_T1870->f1=0;_T1255=(struct _tuple23*)_T1870;}yyval=Cyc_YY14(_T1255);goto _LL0;case 446:{struct _tuple23*_T1870=_cycalloc(sizeof(struct _tuple23));_T125B=yyyvsp;_T125C=& _T125B[0].v;_T125D=(union Cyc_YYSTYPE*)_T125C;_T125E=
# 2912 "parse.y"
Cyc_yyget_YY13(_T125D);_T1870->f0=Cyc_List_rev(_T125E);_T1870->f1=1;_T125A=(struct _tuple23*)_T1870;}yyval=Cyc_YY14(_T125A);goto _LL0;case 447:{struct _tuple23*_T1870=_cycalloc(sizeof(struct _tuple23));
# 2913 "parse.y"
_T1870->f0=0;_T1870->f1=1;_T125F=(struct _tuple23*)_T1870;}yyval=Cyc_YY14(_T125F);goto _LL0;case 448:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T1261=yyyvsp;_T1262=& _T1261[0].v;_T1263=(union Cyc_YYSTYPE*)_T1262;
# 2916
_T1870->hd=Cyc_yyget_YY12(_T1263);_T1870->tl=0;_T1260=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY13(_T1260);goto _LL0;case 449:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T1265=yyyvsp;_T1266=& _T1265[2].v;_T1267=(union Cyc_YYSTYPE*)_T1266;
# 2917 "parse.y"
_T1870->hd=Cyc_yyget_YY12(_T1267);_T1268=yyyvsp;_T1269=& _T1268[0].v;_T126A=(union Cyc_YYSTYPE*)_T1269;_T1870->tl=Cyc_yyget_YY13(_T126A);_T1264=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY13(_T1264);goto _LL0;case 450: _T126B=yyyvsp;_T126C=_T126B[0];
# 2922 "parse.y"
yyval=_T126C.v;goto _LL0;case 451: _T126D=yyyvsp;_T126E=& _T126D[0].v;_T126F=(union Cyc_YYSTYPE*)_T126E;_T1270=
# 2923 "parse.y"
Cyc_yyget_Exp_tok(_T126F);_T1271=yyyvsp;_T1272=& _T1271[2].v;_T1273=(union Cyc_YYSTYPE*)_T1272;_T1274=Cyc_yyget_Exp_tok(_T1273);_T1275=yyyvsp;_T1276=_T1275[0];_T1277=_T1276.l;_T1278=_T1277.first_line;_T1279=Cyc_Position_loc_to_seg(_T1278);_T127A=Cyc_Absyn_seq_exp(_T1270,_T1274,_T1279);yyval=Cyc_Exp_tok(_T127A);goto _LL0;case 452: _T127B=yyyvsp;_T127C=_T127B[0];
# 2927 "parse.y"
yyval=_T127C.v;goto _LL0;case 453: _T127D=yyyvsp;_T127E=& _T127D[0].v;_T127F=(union Cyc_YYSTYPE*)_T127E;_T1280=
# 2929 "parse.y"
Cyc_yyget_Exp_tok(_T127F);_T1281=yyyvsp;_T1282=& _T1281[1].v;_T1283=(union Cyc_YYSTYPE*)_T1282;_T1284=Cyc_yyget_YY7(_T1283);_T1285=yyyvsp;_T1286=& _T1285[2].v;_T1287=(union Cyc_YYSTYPE*)_T1286;_T1288=Cyc_yyget_Exp_tok(_T1287);_T1289=yyyvsp;_T128A=_T1289[0];_T128B=_T128A.l;_T128C=_T128B.first_line;_T128D=Cyc_Position_loc_to_seg(_T128C);_T128E=Cyc_Absyn_assignop_exp(_T1280,_T1284,_T1288,_T128D);yyval=Cyc_Exp_tok(_T128E);goto _LL0;case 454: _T128F=yyyvsp;_T1290=& _T128F[0].v;_T1291=(union Cyc_YYSTYPE*)_T1290;_T1292=
# 2931 "parse.y"
Cyc_yyget_Exp_tok(_T1291);_T1293=yyyvsp;_T1294=& _T1293[2].v;_T1295=(union Cyc_YYSTYPE*)_T1294;_T1296=Cyc_yyget_Exp_tok(_T1295);_T1297=yyyvsp;_T1298=_T1297[0];_T1299=_T1298.l;_T129A=_T1299.first_line;_T129B=Cyc_Position_loc_to_seg(_T129A);_T129C=Cyc_Absyn_swap_exp(_T1292,_T1296,_T129B);yyval=Cyc_Exp_tok(_T129C);goto _LL0;case 455:
# 2935 "parse.y"
 yyval=Cyc_YY7(0);goto _LL0;case 456:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2936 "parse.y"
_T1870->v=(void*)1U;_T129D=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T129D);goto _LL0;case 457:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2937 "parse.y"
_T1870->v=(void*)3U;_T129E=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T129E);goto _LL0;case 458:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2938 "parse.y"
_T1870->v=(void*)4U;_T129F=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T129F);goto _LL0;case 459:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2939 "parse.y"
_T1870->v=(void*)0U;_T12A0=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T12A0);goto _LL0;case 460:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2940 "parse.y"
_T1870->v=(void*)2U;_T12A1=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T12A1);goto _LL0;case 461:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2941 "parse.y"
_T1870->v=(void*)16U;_T12A2=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T12A2);goto _LL0;case 462:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2942 "parse.y"
_T1870->v=(void*)17U;_T12A3=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T12A3);goto _LL0;case 463:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2943 "parse.y"
_T1870->v=(void*)13U;_T12A4=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T12A4);goto _LL0;case 464:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2944 "parse.y"
_T1870->v=(void*)15U;_T12A5=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T12A5);goto _LL0;case 465:{struct Cyc_Core_Opt*_T1870=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2945 "parse.y"
_T1870->v=(void*)14U;_T12A6=(struct Cyc_Core_Opt*)_T1870;}yyval=Cyc_YY7(_T12A6);goto _LL0;case 466: _T12A7=yyyvsp;_T12A8=_T12A7[0];
# 2949 "parse.y"
yyval=_T12A8.v;goto _LL0;case 467: _T12A9=yyyvsp;_T12AA=& _T12A9[0].v;_T12AB=(union Cyc_YYSTYPE*)_T12AA;_T12AC=
# 2951 "parse.y"
Cyc_yyget_Exp_tok(_T12AB);_T12AD=yyyvsp;_T12AE=& _T12AD[2].v;_T12AF=(union Cyc_YYSTYPE*)_T12AE;_T12B0=Cyc_yyget_Exp_tok(_T12AF);_T12B1=yyyvsp;_T12B2=& _T12B1[4].v;_T12B3=(union Cyc_YYSTYPE*)_T12B2;_T12B4=Cyc_yyget_Exp_tok(_T12B3);_T12B5=yyyvsp;_T12B6=_T12B5[0];_T12B7=_T12B6.l;_T12B8=_T12B7.first_line;_T12B9=Cyc_Position_loc_to_seg(_T12B8);_T12BA=Cyc_Absyn_conditional_exp(_T12AC,_T12B0,_T12B4,_T12B9);yyval=Cyc_Exp_tok(_T12BA);goto _LL0;case 468: _T12BB=yyyvsp;_T12BC=& _T12BB[1].v;_T12BD=(union Cyc_YYSTYPE*)_T12BC;_T12BE=
# 2953 "parse.y"
Cyc_yyget_Exp_tok(_T12BD);_T12BF=yyyvsp;_T12C0=_T12BF[0];_T12C1=_T12C0.l;_T12C2=_T12C1.first_line;_T12C3=Cyc_Position_loc_to_seg(_T12C2);_T12C4=Cyc_Absyn_throw_exp(_T12BE,_T12C3);yyval=Cyc_Exp_tok(_T12C4);goto _LL0;case 469: _T12C5=yyyvsp;_T12C6=& _T12C5[1].v;_T12C7=(union Cyc_YYSTYPE*)_T12C6;_T12C8=
# 2955 "parse.y"
Cyc_yyget_Exp_tok(_T12C7);_T12C9=yyyvsp;_T12CA=_T12C9[0];_T12CB=_T12CA.l;_T12CC=_T12CB.first_line;_T12CD=Cyc_Position_loc_to_seg(_T12CC);_T12CE=Cyc_Absyn_New_exp(0,_T12C8,0,_T12CD);yyval=Cyc_Exp_tok(_T12CE);goto _LL0;case 470: _T12CF=yyyvsp;_T12D0=& _T12CF[1].v;_T12D1=(union Cyc_YYSTYPE*)_T12D0;_T12D2=
# 2956 "parse.y"
Cyc_yyget_Exp_tok(_T12D1);_T12D3=yyyvsp;_T12D4=_T12D3[0];_T12D5=_T12D4.l;_T12D6=_T12D5.first_line;_T12D7=Cyc_Position_loc_to_seg(_T12D6);_T12D8=Cyc_Absyn_New_exp(0,_T12D2,0,_T12D7);yyval=Cyc_Exp_tok(_T12D8);goto _LL0;case 471: _T12D9=yyyvsp;_T12DA=& _T12D9[4].v;_T12DB=(union Cyc_YYSTYPE*)_T12DA;_T12DC=
# 2957 "parse.y"
Cyc_yyget_Exp_tok(_T12DB);_T12DD=yyyvsp;_T12DE=& _T12DD[2].v;_T12DF=(union Cyc_YYSTYPE*)_T12DE;_T12E0=Cyc_yyget_Exp_tok(_T12DF);_T12E1=yyyvsp;_T12E2=_T12E1[0];_T12E3=_T12E2.l;_T12E4=_T12E3.first_line;_T12E5=Cyc_Position_loc_to_seg(_T12E4);_T12E6=Cyc_Absyn_New_exp(0,_T12DC,_T12E0,_T12E5);yyval=Cyc_Exp_tok(_T12E6);goto _LL0;case 472: _T12E7=yyyvsp;_T12E8=& _T12E7[4].v;_T12E9=(union Cyc_YYSTYPE*)_T12E8;_T12EA=
# 2958 "parse.y"
Cyc_yyget_Exp_tok(_T12E9);_T12EB=yyyvsp;_T12EC=& _T12EB[2].v;_T12ED=(union Cyc_YYSTYPE*)_T12EC;_T12EE=Cyc_yyget_Exp_tok(_T12ED);_T12EF=yyyvsp;_T12F0=_T12EF[0];_T12F1=_T12F0.l;_T12F2=_T12F1.first_line;_T12F3=Cyc_Position_loc_to_seg(_T12F2);_T12F4=Cyc_Absyn_New_exp(0,_T12EA,_T12EE,_T12F3);yyval=Cyc_Exp_tok(_T12F4);goto _LL0;case 473: _T12F5=yyyvsp;_T12F6=& _T12F5[2].v;_T12F7=(union Cyc_YYSTYPE*)_T12F6;_T12F8=
# 2959 "parse.y"
Cyc_yyget_Exp_tok(_T12F7);{struct _tuple16 _T1870=Cyc_Parse_split_seq(_T12F8);struct Cyc_Absyn_Exp*_T1871;struct Cyc_Absyn_Exp*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_Absyn_Exp*rgn=_T1872;struct Cyc_Absyn_Exp*qual=_T1871;_T12F9=rgn;_T12FA=yyyvsp;_T12FB=& _T12FA[4].v;_T12FC=(union Cyc_YYSTYPE*)_T12FB;_T12FD=Cyc_yyget_Exp_tok(_T12FC);_T12FE=qual;_T12FF=yyyvsp;_T1300=_T12FF[0];_T1301=_T1300.l;_T1302=_T1301.first_line;_T1303=Cyc_Position_loc_to_seg(_T1302);_T1304=Cyc_Absyn_New_exp(_T12F9,_T12FD,_T12FE,_T1303);yyval=Cyc_Exp_tok(_T1304);goto _LL0;}}case 474: _T1305=yyyvsp;_T1306=& _T1305[2].v;_T1307=(union Cyc_YYSTYPE*)_T1306;_T1308=
# 2960 "parse.y"
Cyc_yyget_Exp_tok(_T1307);{struct _tuple16 _T1870=Cyc_Parse_split_seq(_T1308);struct Cyc_Absyn_Exp*_T1871;struct Cyc_Absyn_Exp*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_Absyn_Exp*rgn=_T1872;struct Cyc_Absyn_Exp*qual=_T1871;_T1309=rgn;_T130A=yyyvsp;_T130B=& _T130A[4].v;_T130C=(union Cyc_YYSTYPE*)_T130B;_T130D=Cyc_yyget_Exp_tok(_T130C);_T130E=qual;_T130F=yyyvsp;_T1310=_T130F[0];_T1311=_T1310.l;_T1312=_T1311.first_line;_T1313=Cyc_Position_loc_to_seg(_T1312);_T1314=Cyc_Absyn_New_exp(_T1309,_T130D,_T130E,_T1313);yyval=Cyc_Exp_tok(_T1314);goto _LL0;}}case 475: _T1315=yyyvsp;_T1316=_T1315[0];
# 2972 "parse.y"
yyval=_T1316.v;goto _LL0;case 476: _T1317=yyyvsp;_T1318=_T1317[0];
# 2975
yyval=_T1318.v;goto _LL0;case 477: _T1319=yyyvsp;_T131A=& _T1319[0].v;_T131B=(union Cyc_YYSTYPE*)_T131A;_T131C=
# 2977 "parse.y"
Cyc_yyget_Exp_tok(_T131B);_T131D=yyyvsp;_T131E=& _T131D[2].v;_T131F=(union Cyc_YYSTYPE*)_T131E;_T1320=Cyc_yyget_Exp_tok(_T131F);_T1321=yyyvsp;_T1322=_T1321[0];_T1323=_T1322.l;_T1324=_T1323.first_line;_T1325=Cyc_Position_loc_to_seg(_T1324);_T1326=Cyc_Absyn_or_exp(_T131C,_T1320,_T1325);yyval=Cyc_Exp_tok(_T1326);goto _LL0;case 478: _T1327=yyyvsp;_T1328=_T1327[0];
# 2980
yyval=_T1328.v;goto _LL0;case 479: _T1329=yyyvsp;_T132A=& _T1329[0].v;_T132B=(union Cyc_YYSTYPE*)_T132A;_T132C=
# 2982 "parse.y"
Cyc_yyget_Exp_tok(_T132B);_T132D=yyyvsp;_T132E=& _T132D[2].v;_T132F=(union Cyc_YYSTYPE*)_T132E;_T1330=Cyc_yyget_Exp_tok(_T132F);_T1331=yyyvsp;_T1332=_T1331[0];_T1333=_T1332.l;_T1334=_T1333.first_line;_T1335=Cyc_Position_loc_to_seg(_T1334);_T1336=Cyc_Absyn_and_exp(_T132C,_T1330,_T1335);yyval=Cyc_Exp_tok(_T1336);goto _LL0;case 480: _T1337=yyyvsp;_T1338=_T1337[0];
# 2985
yyval=_T1338.v;goto _LL0;case 481: _T1339=yyyvsp;_T133A=& _T1339[0].v;_T133B=(union Cyc_YYSTYPE*)_T133A;_T133C=
# 2987 "parse.y"
Cyc_yyget_Exp_tok(_T133B);_T133D=yyyvsp;_T133E=& _T133D[2].v;_T133F=(union Cyc_YYSTYPE*)_T133E;_T1340=Cyc_yyget_Exp_tok(_T133F);_T1341=yyyvsp;_T1342=_T1341[0];_T1343=_T1342.l;_T1344=_T1343.first_line;_T1345=Cyc_Position_loc_to_seg(_T1344);_T1346=Cyc_Absyn_prim2_exp(14U,_T133C,_T1340,_T1345);yyval=Cyc_Exp_tok(_T1346);goto _LL0;case 482: _T1347=yyyvsp;_T1348=_T1347[0];
# 2990
yyval=_T1348.v;goto _LL0;case 483: _T1349=yyyvsp;_T134A=& _T1349[0].v;_T134B=(union Cyc_YYSTYPE*)_T134A;_T134C=
# 2992 "parse.y"
Cyc_yyget_Exp_tok(_T134B);_T134D=yyyvsp;_T134E=& _T134D[2].v;_T134F=(union Cyc_YYSTYPE*)_T134E;_T1350=Cyc_yyget_Exp_tok(_T134F);_T1351=yyyvsp;_T1352=_T1351[0];_T1353=_T1352.l;_T1354=_T1353.first_line;_T1355=Cyc_Position_loc_to_seg(_T1354);_T1356=Cyc_Absyn_prim2_exp(15U,_T134C,_T1350,_T1355);yyval=Cyc_Exp_tok(_T1356);goto _LL0;case 484: _T1357=yyyvsp;_T1358=_T1357[0];
# 2995
yyval=_T1358.v;goto _LL0;case 485: _T1359=yyyvsp;_T135A=& _T1359[0].v;_T135B=(union Cyc_YYSTYPE*)_T135A;_T135C=
# 2997 "parse.y"
Cyc_yyget_Exp_tok(_T135B);_T135D=yyyvsp;_T135E=& _T135D[2].v;_T135F=(union Cyc_YYSTYPE*)_T135E;_T1360=Cyc_yyget_Exp_tok(_T135F);_T1361=yyyvsp;_T1362=_T1361[0];_T1363=_T1362.l;_T1364=_T1363.first_line;_T1365=Cyc_Position_loc_to_seg(_T1364);_T1366=Cyc_Absyn_prim2_exp(13U,_T135C,_T1360,_T1365);yyval=Cyc_Exp_tok(_T1366);goto _LL0;case 486: _T1367=yyyvsp;_T1368=_T1367[0];
# 3000
yyval=_T1368.v;goto _LL0;case 487: _T1369=yyyvsp;_T136A=& _T1369[1].v;_T136B=(union Cyc_YYSTYPE*)_T136A;_T136C=
# 3002 "parse.y"
Cyc_yyget_YY69(_T136B);_T136D=yyyvsp;_T136E=& _T136D[0].v;_T136F=(union Cyc_YYSTYPE*)_T136E;_T1370=Cyc_yyget_Exp_tok(_T136F);_T1371=yyyvsp;_T1372=& _T1371[2].v;_T1373=(union Cyc_YYSTYPE*)_T1372;_T1374=Cyc_yyget_Exp_tok(_T1373);_T1375=yyyvsp;_T1376=_T1375[0];_T1377=_T1376.l;_T1378=_T1377.first_line;_T1379=Cyc_Position_loc_to_seg(_T1378);_T137A=_T136C(_T1370,_T1374,_T1379);yyval=Cyc_Exp_tok(_T137A);goto _LL0;case 488: _T137B=yyyvsp;_T137C=_T137B[0];
# 3005
yyval=_T137C.v;goto _LL0;case 489: _T137D=yyyvsp;_T137E=& _T137D[1].v;_T137F=(union Cyc_YYSTYPE*)_T137E;_T1380=
# 3007 "parse.y"
Cyc_yyget_YY69(_T137F);_T1381=yyyvsp;_T1382=& _T1381[0].v;_T1383=(union Cyc_YYSTYPE*)_T1382;_T1384=Cyc_yyget_Exp_tok(_T1383);_T1385=yyyvsp;_T1386=& _T1385[2].v;_T1387=(union Cyc_YYSTYPE*)_T1386;_T1388=Cyc_yyget_Exp_tok(_T1387);_T1389=yyyvsp;_T138A=_T1389[0];_T138B=_T138A.l;_T138C=_T138B.first_line;_T138D=Cyc_Position_loc_to_seg(_T138C);_T138E=_T1380(_T1384,_T1388,_T138D);yyval=Cyc_Exp_tok(_T138E);goto _LL0;case 490: _T138F=yyyvsp;_T1390=_T138F[0];
# 3010
yyval=_T1390.v;goto _LL0;case 491: _T1391=yyyvsp;_T1392=& _T1391[0].v;_T1393=(union Cyc_YYSTYPE*)_T1392;_T1394=
# 3012 "parse.y"
Cyc_yyget_Exp_tok(_T1393);_T1395=yyyvsp;_T1396=& _T1395[2].v;_T1397=(union Cyc_YYSTYPE*)_T1396;_T1398=Cyc_yyget_Exp_tok(_T1397);_T1399=yyyvsp;_T139A=_T1399[0];_T139B=_T139A.l;_T139C=_T139B.first_line;_T139D=Cyc_Position_loc_to_seg(_T139C);_T139E=Cyc_Absyn_prim2_exp(16U,_T1394,_T1398,_T139D);yyval=Cyc_Exp_tok(_T139E);goto _LL0;case 492: _T139F=yyyvsp;_T13A0=& _T139F[0].v;_T13A1=(union Cyc_YYSTYPE*)_T13A0;_T13A2=
# 3014 "parse.y"
Cyc_yyget_Exp_tok(_T13A1);_T13A3=yyyvsp;_T13A4=& _T13A3[2].v;_T13A5=(union Cyc_YYSTYPE*)_T13A4;_T13A6=Cyc_yyget_Exp_tok(_T13A5);_T13A7=yyyvsp;_T13A8=_T13A7[0];_T13A9=_T13A8.l;_T13AA=_T13A9.first_line;_T13AB=Cyc_Position_loc_to_seg(_T13AA);_T13AC=Cyc_Absyn_prim2_exp(17U,_T13A2,_T13A6,_T13AB);yyval=Cyc_Exp_tok(_T13AC);goto _LL0;case 493: _T13AD=yyyvsp;_T13AE=_T13AD[0];
# 3017
yyval=_T13AE.v;goto _LL0;case 494: _T13AF=yyyvsp;_T13B0=& _T13AF[1].v;_T13B1=(union Cyc_YYSTYPE*)_T13B0;_T13B2=
# 3019 "parse.y"
Cyc_yyget_YY6(_T13B1);_T13B3=yyyvsp;_T13B4=& _T13B3[0].v;_T13B5=(union Cyc_YYSTYPE*)_T13B4;_T13B6=Cyc_yyget_Exp_tok(_T13B5);_T13B7=yyyvsp;_T13B8=& _T13B7[2].v;_T13B9=(union Cyc_YYSTYPE*)_T13B8;_T13BA=Cyc_yyget_Exp_tok(_T13B9);_T13BB=yyyvsp;_T13BC=_T13BB[0];_T13BD=_T13BC.l;_T13BE=_T13BD.first_line;_T13BF=Cyc_Position_loc_to_seg(_T13BE);_T13C0=Cyc_Absyn_prim2_exp(_T13B2,_T13B6,_T13BA,_T13BF);yyval=Cyc_Exp_tok(_T13C0);goto _LL0;case 495: _T13C1=yyyvsp;_T13C2=_T13C1[0];
# 3022
yyval=_T13C2.v;goto _LL0;case 496: _T13C3=yyyvsp;_T13C4=& _T13C3[1].v;_T13C5=(union Cyc_YYSTYPE*)_T13C4;_T13C6=
# 3024 "parse.y"
Cyc_yyget_YY6(_T13C5);_T13C7=yyyvsp;_T13C8=& _T13C7[0].v;_T13C9=(union Cyc_YYSTYPE*)_T13C8;_T13CA=Cyc_yyget_Exp_tok(_T13C9);_T13CB=yyyvsp;_T13CC=& _T13CB[2].v;_T13CD=(union Cyc_YYSTYPE*)_T13CC;_T13CE=Cyc_yyget_Exp_tok(_T13CD);_T13CF=yyyvsp;_T13D0=_T13CF[0];_T13D1=_T13D0.l;_T13D2=_T13D1.first_line;_T13D3=Cyc_Position_loc_to_seg(_T13D2);_T13D4=Cyc_Absyn_prim2_exp(_T13C6,_T13CA,_T13CE,_T13D3);yyval=Cyc_Exp_tok(_T13D4);goto _LL0;case 497: _T13D5=Cyc_Absyn_eq_exp;
# 3027
yyval=Cyc_YY69(_T13D5);goto _LL0;case 498: _T13D6=Cyc_Absyn_neq_exp;
# 3028 "parse.y"
yyval=Cyc_YY69(_T13D6);goto _LL0;case 499: _T13D7=Cyc_Absyn_lt_exp;
# 3031
yyval=Cyc_YY69(_T13D7);goto _LL0;case 500: _T13D8=Cyc_Absyn_gt_exp;
# 3032 "parse.y"
yyval=Cyc_YY69(_T13D8);goto _LL0;case 501: _T13D9=Cyc_Absyn_lte_exp;
# 3033 "parse.y"
yyval=Cyc_YY69(_T13D9);goto _LL0;case 502: _T13DA=Cyc_Absyn_gte_exp;
# 3034 "parse.y"
yyval=Cyc_YY69(_T13DA);goto _LL0;case 503:
# 3037
 yyval=Cyc_YY6(0U);goto _LL0;case 504:
# 3038 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 505:
# 3041
 yyval=Cyc_YY6(1U);goto _LL0;case 506:
# 3042 "parse.y"
 yyval=Cyc_YY6(3U);goto _LL0;case 507:
# 3043 "parse.y"
 yyval=Cyc_YY6(4U);goto _LL0;case 508: _T13DB=yyyvsp;_T13DC=_T13DB[0];
# 3047 "parse.y"
yyval=_T13DC.v;goto _LL0;case 509: _T13DD=yyyvsp;_T13DE=& _T13DD[1].v;_T13DF=(union Cyc_YYSTYPE*)_T13DE;_T13E0=
# 3049 "parse.y"
Cyc_yyget_YY38(_T13DF);_T13E1=yyyvsp;_T13E2=_T13E1[1];_T13E3=_T13E2.l;_T13E4=_T13E3.first_line;_T13E5=Cyc_Position_loc_to_seg(_T13E4);{void*t=Cyc_Parse_type_name_to_type(_T13E0,_T13E5);_T13E6=t;_T13E7=yyyvsp;_T13E8=& _T13E7[3].v;_T13E9=(union Cyc_YYSTYPE*)_T13E8;_T13EA=
Cyc_yyget_Exp_tok(_T13E9);_T13EB=yyyvsp;_T13EC=_T13EB[0];_T13ED=_T13EC.l;_T13EE=_T13ED.first_line;_T13EF=Cyc_Position_loc_to_seg(_T13EE);_T13F0=Cyc_Absyn_cast_exp(_T13E6,_T13EA,1,0U,_T13EF);yyval=Cyc_Exp_tok(_T13F0);goto _LL0;}case 510: _T13F1=yyyvsp;_T13F2=_T13F1[0];
# 3054 "parse.y"
yyval=_T13F2.v;goto _LL0;case 511: _T13F3=yyyvsp;_T13F4=& _T13F3[1].v;_T13F5=(union Cyc_YYSTYPE*)_T13F4;_T13F6=
# 3055 "parse.y"
Cyc_yyget_Exp_tok(_T13F5);_T13F7=yyyvsp;_T13F8=_T13F7[0];_T13F9=_T13F8.l;_T13FA=_T13F9.first_line;_T13FB=Cyc_Position_loc_to_seg(_T13FA);_T13FC=Cyc_Absyn_increment_exp(_T13F6,0U,_T13FB);yyval=Cyc_Exp_tok(_T13FC);goto _LL0;case 512: _T13FD=yyyvsp;_T13FE=& _T13FD[1].v;_T13FF=(union Cyc_YYSTYPE*)_T13FE;_T1400=
# 3056 "parse.y"
Cyc_yyget_Exp_tok(_T13FF);_T1401=yyyvsp;_T1402=_T1401[0];_T1403=_T1402.l;_T1404=_T1403.first_line;_T1405=Cyc_Position_loc_to_seg(_T1404);_T1406=Cyc_Absyn_increment_exp(_T1400,2U,_T1405);yyval=Cyc_Exp_tok(_T1406);goto _LL0;case 513: _T1407=yyyvsp;_T1408=& _T1407[1].v;_T1409=(union Cyc_YYSTYPE*)_T1408;_T140A=
# 3057 "parse.y"
Cyc_yyget_Exp_tok(_T1409);_T140B=yyyvsp;_T140C=_T140B[0];_T140D=_T140C.l;_T140E=_T140D.first_line;_T140F=Cyc_Position_loc_to_seg(_T140E);_T1410=Cyc_Absyn_address_exp(_T140A,_T140F);yyval=Cyc_Exp_tok(_T1410);goto _LL0;case 514: _T1411=yyyvsp;_T1412=& _T1411[1].v;_T1413=(union Cyc_YYSTYPE*)_T1412;_T1414=
# 3058 "parse.y"
Cyc_yyget_Exp_tok(_T1413);_T1415=yyyvsp;_T1416=_T1415[0];_T1417=_T1416.l;_T1418=_T1417.first_line;_T1419=Cyc_Position_loc_to_seg(_T1418);_T141A=Cyc_Absyn_deref_exp(_T1414,_T1419);yyval=Cyc_Exp_tok(_T141A);goto _LL0;case 515: _T141B=yyyvsp;_T141C=& _T141B[0].v;_T141D=(union Cyc_YYSTYPE*)_T141C;_T141E=
# 3059 "parse.y"
Cyc_yyget_YY6(_T141D);_T141F=yyyvsp;_T1420=& _T141F[1].v;_T1421=(union Cyc_YYSTYPE*)_T1420;_T1422=Cyc_yyget_Exp_tok(_T1421);_T1423=yyyvsp;_T1424=_T1423[0];_T1425=_T1424.l;_T1426=_T1425.first_line;_T1427=Cyc_Position_loc_to_seg(_T1426);_T1428=Cyc_Absyn_prim1_exp(_T141E,_T1422,_T1427);yyval=Cyc_Exp_tok(_T1428);goto _LL0;case 516: _T1429=yyyvsp;_T142A=& _T1429[2].v;_T142B=(union Cyc_YYSTYPE*)_T142A;_T142C=
# 3061 "parse.y"
Cyc_yyget_YY38(_T142B);_T142D=yyyvsp;_T142E=_T142D[2];_T142F=_T142E.l;_T1430=_T142F.first_line;_T1431=Cyc_Position_loc_to_seg(_T1430);{void*t=Cyc_Parse_type_name_to_type(_T142C,_T1431);_T1432=t;_T1433=yyyvsp;_T1434=_T1433[0];_T1435=_T1434.l;_T1436=_T1435.first_line;_T1437=
Cyc_Position_loc_to_seg(_T1436);_T1438=Cyc_Absyn_sizeoftype_exp(_T1432,_T1437);yyval=Cyc_Exp_tok(_T1438);goto _LL0;}case 517: _T1439=yyyvsp;_T143A=& _T1439[1].v;_T143B=(union Cyc_YYSTYPE*)_T143A;_T143C=
# 3064 "parse.y"
Cyc_yyget_Exp_tok(_T143B);_T143D=yyyvsp;_T143E=_T143D[0];_T143F=_T143E.l;_T1440=_T143F.first_line;_T1441=Cyc_Position_loc_to_seg(_T1440);_T1442=Cyc_Absyn_sizeofexp_exp(_T143C,_T1441);yyval=Cyc_Exp_tok(_T1442);goto _LL0;case 518: _T1443=yyyvsp;_T1444=& _T1443[2].v;_T1445=(union Cyc_YYSTYPE*)_T1444;_T1446=
# 3066 "parse.y"
Cyc_yyget_YY38(_T1445);_T1447=yyyvsp;_T1448=_T1447[2];_T1449=_T1448.l;_T144A=_T1449.first_line;_T144B=Cyc_Position_loc_to_seg(_T144A);{void*t=Cyc_Parse_type_name_to_type(_T1446,_T144B);_T144C=t;_T144D=yyyvsp;_T144E=& _T144D[4].v;_T144F=(union Cyc_YYSTYPE*)_T144E;_T1450=
Cyc_yyget_YY3(_T144F);_T1451=Cyc_List_imp_rev(_T1450);_T1452=yyyvsp;_T1453=_T1452[0];_T1454=_T1453.l;_T1455=_T1454.first_line;_T1456=Cyc_Position_loc_to_seg(_T1455);_T1457=Cyc_Absyn_offsetof_exp(_T144C,_T1451,_T1456);yyval=Cyc_Exp_tok(_T1457);goto _LL0;}case 519:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1870->tag=33;
# 3071
_T1870->f1.mknd=0U;_T1870->f1.rgn=0;_T1870->f1.aqual=0;_T1870->f1.elt_type=0;_T1459=yyyvsp;_T145A=& _T1459[2].v;_T145B=(union Cyc_YYSTYPE*)_T145A;_T1870->f1.num_elts=Cyc_yyget_Exp_tok(_T145B);_T1870->f1.fat_result=0;_T1870->f1.inline_call=0;_T1458=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1870;}_T145C=(void*)_T1458;_T145D=yyyvsp;_T145E=_T145D[0];_T145F=_T145E.l;_T1460=_T145F.first_line;_T1461=
Cyc_Position_loc_to_seg(_T1460);_T1462=
# 3071
Cyc_Absyn_new_exp(_T145C,_T1461);yyval=Cyc_Exp_tok(_T1462);goto _LL0;case 520:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1870->tag=33;
# 3074 "parse.y"
_T1870->f1.mknd=0U;_T1464=yyyvsp;_T1465=& _T1464[2].v;_T1466=(union Cyc_YYSTYPE*)_T1465;_T1870->f1.rgn=Cyc_yyget_Exp_tok(_T1466);_T1467=yyyvsp;_T1468=& _T1467[4].v;_T1469=(union Cyc_YYSTYPE*)_T1468;_T1870->f1.aqual=Cyc_yyget_Exp_tok(_T1469);_T1870->f1.elt_type=0;_T146A=yyyvsp;_T146B=& _T146A[6].v;_T146C=(union Cyc_YYSTYPE*)_T146B;_T1870->f1.num_elts=Cyc_yyget_Exp_tok(_T146C);_T1870->f1.fat_result=0;_T1870->f1.inline_call=0;_T1463=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1870;}_T146D=(void*)_T1463;_T146E=yyyvsp;_T146F=_T146E[0];_T1470=_T146F.l;_T1471=_T1470.first_line;_T1472=
Cyc_Position_loc_to_seg(_T1471);_T1473=
# 3074
Cyc_Absyn_new_exp(_T146D,_T1472);yyval=Cyc_Exp_tok(_T1473);goto _LL0;case 521:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1870->tag=33;
# 3077 "parse.y"
_T1870->f1.mknd=0U;_T1475=yyyvsp;_T1476=& _T1475[2].v;_T1477=(union Cyc_YYSTYPE*)_T1476;_T1870->f1.rgn=Cyc_yyget_Exp_tok(_T1477);_T1870->f1.aqual=0;_T1870->f1.elt_type=0;_T1478=yyyvsp;_T1479=& _T1478[4].v;_T147A=(union Cyc_YYSTYPE*)_T1479;_T1870->f1.num_elts=Cyc_yyget_Exp_tok(_T147A);_T1870->f1.fat_result=0;_T1870->f1.inline_call=0;_T1474=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1870;}_T147B=(void*)_T1474;_T147C=yyyvsp;_T147D=_T147C[0];_T147E=_T147D.l;_T147F=_T147E.first_line;_T1480=
Cyc_Position_loc_to_seg(_T147F);_T1481=
# 3077
Cyc_Absyn_new_exp(_T147B,_T1480);yyval=Cyc_Exp_tok(_T1481);goto _LL0;case 522: _T1482=yyyvsp;_T1483=& _T1482[2].v;_T1484=(union Cyc_YYSTYPE*)_T1483;_T1485=
# 3080 "parse.y"
Cyc_yyget_Exp_tok(_T1484);{struct _tuple16 _T1870=Cyc_Parse_split_seq(_T1485);struct Cyc_Absyn_Exp*_T1871;struct Cyc_Absyn_Exp*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_Absyn_Exp*rgn=_T1872;struct Cyc_Absyn_Exp*qual=_T1871;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1873->tag=33;
_T1873->f1.mknd=2U;_T1873->f1.rgn=rgn;_T1873->f1.aqual=qual;_T1873->f1.elt_type=0;_T1487=yyyvsp;_T1488=& _T1487[4].v;_T1489=(union Cyc_YYSTYPE*)_T1488;_T1873->f1.num_elts=Cyc_yyget_Exp_tok(_T1489);_T1873->f1.fat_result=0;_T1873->f1.inline_call=0;_T1486=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1873;}_T148A=(void*)_T1486;_T148B=yyyvsp;_T148C=_T148B[0];_T148D=_T148C.l;_T148E=_T148D.first_line;_T148F=
Cyc_Position_loc_to_seg(_T148E);_T1490=
# 3081
Cyc_Absyn_new_exp(_T148A,_T148F);yyval=Cyc_Exp_tok(_T1490);goto _LL0;}}case 523: _T1491=yyyvsp;_T1492=& _T1491[2].v;_T1493=(union Cyc_YYSTYPE*)_T1492;_T1494=
# 3084 "parse.y"
Cyc_yyget_Exp_tok(_T1493);{struct _tuple16 _T1870=Cyc_Parse_split_seq(_T1494);struct Cyc_Absyn_Exp*_T1871;struct Cyc_Absyn_Exp*_T1872;_T1872=_T1870.f0;_T1871=_T1870.f1;{struct Cyc_Absyn_Exp*rgn=_T1872;struct Cyc_Absyn_Exp*qual=_T1871;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1873->tag=33;
_T1873->f1.mknd=0U;_T1873->f1.rgn=rgn;_T1873->f1.aqual=qual;_T1873->f1.elt_type=0;_T1496=yyyvsp;_T1497=& _T1496[4].v;_T1498=(union Cyc_YYSTYPE*)_T1497;_T1873->f1.num_elts=Cyc_yyget_Exp_tok(_T1498);_T1873->f1.fat_result=0;_T1873->f1.inline_call=1;_T1495=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1873;}_T1499=(void*)_T1495;_T149A=yyyvsp;_T149B=_T149A[0];_T149C=_T149B.l;_T149D=_T149C.first_line;_T149E=
Cyc_Position_loc_to_seg(_T149D);_T149F=
# 3085
Cyc_Absyn_new_exp(_T1499,_T149E);yyval=Cyc_Exp_tok(_T149F);goto _LL0;}}case 524: _T14A0=yyyvsp;_T14A1=& _T14A0[6].v;_T14A2=(union Cyc_YYSTYPE*)_T14A1;_T14A3=
# 3088 "parse.y"
Cyc_yyget_YY38(_T14A2);_T14A4=yyyvsp;_T14A5=_T14A4[6];_T14A6=_T14A5.l;_T14A7=_T14A6.first_line;_T14A8=Cyc_Position_loc_to_seg(_T14A7);{void*t=Cyc_Parse_type_name_to_type(_T14A3,_T14A8);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1870->tag=33;
_T1870->f1.mknd=1U;_T1870->f1.rgn=0;_T1870->f1.aqual=0;{void**_T1871=_cycalloc(sizeof(void*));*_T1871=t;_T14AA=(void**)_T1871;}_T1870->f1.elt_type=_T14AA;_T14AB=yyyvsp;_T14AC=& _T14AB[2].v;_T14AD=(union Cyc_YYSTYPE*)_T14AC;_T1870->f1.num_elts=Cyc_yyget_Exp_tok(_T14AD);_T1870->f1.fat_result=0;_T1870->f1.inline_call=0;_T14A9=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1870;}_T14AE=(void*)_T14A9;_T14AF=yyyvsp;_T14B0=_T14AF[0];_T14B1=_T14B0.l;_T14B2=_T14B1.first_line;_T14B3=
Cyc_Position_loc_to_seg(_T14B2);_T14B4=
# 3089
Cyc_Absyn_new_exp(_T14AE,_T14B3);yyval=Cyc_Exp_tok(_T14B4);goto _LL0;}case 525: _T14B5=yyyvsp;_T14B6=& _T14B5[10].v;_T14B7=(union Cyc_YYSTYPE*)_T14B6;_T14B8=
# 3092 "parse.y"
Cyc_yyget_YY38(_T14B7);_T14B9=yyyvsp;_T14BA=_T14B9[10];_T14BB=_T14BA.l;_T14BC=_T14BB.first_line;_T14BD=Cyc_Position_loc_to_seg(_T14BC);{void*t=Cyc_Parse_type_name_to_type(_T14B8,_T14BD);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1870->tag=33;
_T1870->f1.mknd=1U;_T14BF=yyyvsp;_T14C0=& _T14BF[2].v;_T14C1=(union Cyc_YYSTYPE*)_T14C0;_T1870->f1.rgn=Cyc_yyget_Exp_tok(_T14C1);_T14C2=yyyvsp;_T14C3=& _T14C2[4].v;_T14C4=(union Cyc_YYSTYPE*)_T14C3;_T1870->f1.aqual=Cyc_yyget_Exp_tok(_T14C4);{void**_T1871=_cycalloc(sizeof(void*));*_T1871=t;_T14C5=(void**)_T1871;}_T1870->f1.elt_type=_T14C5;_T14C6=yyyvsp;_T14C7=& _T14C6[6].v;_T14C8=(union Cyc_YYSTYPE*)_T14C7;_T1870->f1.num_elts=Cyc_yyget_Exp_tok(_T14C8);_T1870->f1.fat_result=0;_T1870->f1.inline_call=0;_T14BE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1870;}_T14C9=(void*)_T14BE;_T14CA=yyyvsp;_T14CB=_T14CA[0];_T14CC=_T14CB.l;_T14CD=_T14CC.first_line;_T14CE=
Cyc_Position_loc_to_seg(_T14CD);_T14CF=
# 3093
Cyc_Absyn_new_exp(_T14C9,_T14CE);yyval=Cyc_Exp_tok(_T14CF);goto _LL0;}case 526: _T14D0=yyyvsp;_T14D1=& _T14D0[8].v;_T14D2=(union Cyc_YYSTYPE*)_T14D1;_T14D3=
# 3097
Cyc_yyget_YY38(_T14D2);_T14D4=yyyvsp;_T14D5=_T14D4[8];_T14D6=_T14D5.l;_T14D7=_T14D6.first_line;_T14D8=Cyc_Position_loc_to_seg(_T14D7);{void*t=Cyc_Parse_type_name_to_type(_T14D3,_T14D8);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1870->tag=33;
_T1870->f1.mknd=1U;_T14DA=yyyvsp;_T14DB=& _T14DA[2].v;_T14DC=(union Cyc_YYSTYPE*)_T14DB;_T1870->f1.rgn=Cyc_yyget_Exp_tok(_T14DC);_T1870->f1.aqual=0;{void**_T1871=_cycalloc(sizeof(void*));*_T1871=t;_T14DD=(void**)_T1871;}_T1870->f1.elt_type=_T14DD;_T14DE=yyyvsp;_T14DF=& _T14DE[4].v;_T14E0=(union Cyc_YYSTYPE*)_T14DF;_T1870->f1.num_elts=Cyc_yyget_Exp_tok(_T14E0);_T1870->f1.fat_result=0;_T1870->f1.inline_call=0;_T14D9=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1870;}_T14E1=(void*)_T14D9;_T14E2=yyyvsp;_T14E3=_T14E2[0];_T14E4=_T14E3.l;_T14E5=_T14E4.first_line;_T14E6=
Cyc_Position_loc_to_seg(_T14E5);_T14E7=
# 3098
Cyc_Absyn_new_exp(_T14E1,_T14E6);yyval=Cyc_Exp_tok(_T14E7);goto _LL0;}case 527:{struct Cyc_Absyn_Exp*_T1870[1];_T14E9=yyyvsp;_T14EA=& _T14E9[2].v;_T14EB=(union Cyc_YYSTYPE*)_T14EA;_T1870[0]=
# 3101 "parse.y"
Cyc_yyget_Exp_tok(_T14EB);_T14EC=_tag_fat(_T1870,sizeof(struct Cyc_Absyn_Exp*),1);_T14E8=Cyc_List_list(_T14EC);}_T14ED=yyyvsp;_T14EE=_T14ED[0];_T14EF=_T14EE.l;_T14F0=_T14EF.first_line;_T14F1=Cyc_Position_loc_to_seg(_T14F0);_T14F2=Cyc_Absyn_primop_exp(18U,_T14E8,_T14F1);yyval=Cyc_Exp_tok(_T14F2);goto _LL0;case 528: _T14F3=yyyvsp;_T14F4=& _T14F3[2].v;_T14F5=(union Cyc_YYSTYPE*)_T14F4;_T14F6=
# 3103 "parse.y"
Cyc_yyget_Exp_tok(_T14F5);_T14F7=yyyvsp;_T14F8=_T14F7[0];_T14F9=_T14F8.l;_T14FA=_T14F9.first_line;_T14FB=Cyc_Position_loc_to_seg(_T14FA);_T14FC=Cyc_Absyn_tagof_exp(_T14F6,_T14FB);yyval=Cyc_Exp_tok(_T14FC);goto _LL0;case 529:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T1870->tag=37;_T14FE=yyyvsp;_T14FF=& _T14FE[2].v;_T1500=(union Cyc_YYSTYPE*)_T14FF;
# 3105 "parse.y"
_T1870->f1=Cyc_yyget_Exp_tok(_T1500);{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T1502=yyyvsp;_T1503=& _T1502[4].v;_T1504=(union Cyc_YYSTYPE*)_T1503;*_T1871=Cyc_yyget_String_tok(_T1504);_T1501=(struct _fat_ptr*)_T1871;}_T1870->f2=_T1501;_T14FD=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T1870;}_T1505=(void*)_T14FD;_T1506=yyyvsp;_T1507=_T1506[0];_T1508=_T1507.l;_T1509=_T1508.first_line;_T150A=Cyc_Position_loc_to_seg(_T1509);_T150B=Cyc_Absyn_new_exp(_T1505,_T150A);yyval=Cyc_Exp_tok(_T150B);goto _LL0;case 530:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T1870->tag=37;_T150D=yyyvsp;_T150E=& _T150D[2].v;_T150F=(union Cyc_YYSTYPE*)_T150E;_T1510=
# 3107 "parse.y"
Cyc_yyget_Exp_tok(_T150F);_T1511=yyyvsp;_T1512=_T1511[2];_T1513=_T1512.l;_T1514=_T1513.first_line;_T1515=Cyc_Position_loc_to_seg(_T1514);_T1870->f1=Cyc_Absyn_deref_exp(_T1510,_T1515);{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T1517=yyyvsp;_T1518=& _T1517[4].v;_T1519=(union Cyc_YYSTYPE*)_T1518;*_T1871=Cyc_yyget_String_tok(_T1519);_T1516=(struct _fat_ptr*)_T1871;}_T1870->f2=_T1516;_T150C=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T1870;}_T151A=(void*)_T150C;_T151B=yyyvsp;_T151C=_T151B[0];_T151D=_T151C.l;_T151E=_T151D.first_line;_T151F=Cyc_Position_loc_to_seg(_T151E);_T1520=Cyc_Absyn_new_exp(_T151A,_T151F);yyval=Cyc_Exp_tok(_T1520);goto _LL0;case 531: _T1521=yyyvsp;_T1522=& _T1521[2].v;_T1523=(union Cyc_YYSTYPE*)_T1522;_T1524=
# 3109 "parse.y"
Cyc_yyget_YY38(_T1523);_T1525=yyyvsp;_T1526=_T1525[2];_T1527=_T1526.l;_T1528=_T1527.first_line;_T1529=Cyc_Position_loc_to_seg(_T1528);{void*t=Cyc_Parse_type_name_to_type(_T1524,_T1529);_T152A=t;_T152B=yyyvsp;_T152C=_T152B[0];_T152D=_T152C.l;_T152E=_T152D.first_line;_T152F=
Cyc_Position_loc_to_seg(_T152E);_T1530=Cyc_Absyn_valueof_exp(_T152A,_T152F);yyval=Cyc_Exp_tok(_T1530);goto _LL0;}case 532: _T1531=yyyvsp;_T1532=& _T1531[1].v;_T1533=(union Cyc_YYSTYPE*)_T1532;_T1534=
# 3111 "parse.y"
Cyc_yyget_YY63(_T1533);_T1535=yyyvsp;_T1536=_T1535[0];_T1537=_T1536.l;_T1538=_T1537.first_line;_T1539=Cyc_Position_loc_to_seg(_T1538);_T153A=Cyc_Absyn_new_exp(_T1534,_T1539);yyval=Cyc_Exp_tok(_T153A);goto _LL0;case 533: _T153B=yyyvsp;_T153C=& _T153B[1].v;_T153D=(union Cyc_YYSTYPE*)_T153C;_T153E=
# 3112 "parse.y"
Cyc_yyget_Exp_tok(_T153D);_T153F=yyyvsp;_T1540=_T153F[0];_T1541=_T1540.l;_T1542=_T1541.first_line;_T1543=Cyc_Position_loc_to_seg(_T1542);_T1544=Cyc_Absyn_extension_exp(_T153E,_T1543);yyval=Cyc_Exp_tok(_T1544);goto _LL0;case 534: _T1545=yyyvsp;_T1546=& _T1545[2].v;_T1547=(union Cyc_YYSTYPE*)_T1546;_T1548=
# 3113 "parse.y"
Cyc_yyget_Exp_tok(_T1547);_T1549=yyyvsp;_T154A=_T1549[0];_T154B=_T154A.l;_T154C=_T154B.first_line;_T154D=Cyc_Position_loc_to_seg(_T154C);_T154E=Cyc_Absyn_assert_exp(_T1548,0,_T154D);yyval=Cyc_Exp_tok(_T154E);goto _LL0;case 535: _T154F=yyyvsp;_T1550=& _T154F[2].v;_T1551=(union Cyc_YYSTYPE*)_T1550;_T1552=
# 3114 "parse.y"
Cyc_yyget_Exp_tok(_T1551);_T1553=yyyvsp;_T1554=_T1553[0];_T1555=_T1554.l;_T1556=_T1555.first_line;_T1557=Cyc_Position_loc_to_seg(_T1556);_T1558=Cyc_Absyn_assert_exp(_T1552,1,_T1557);yyval=Cyc_Exp_tok(_T1558);goto _LL0;case 536: _T1559=yyyvsp;_T155A=& _T1559[2].v;_T155B=(union Cyc_YYSTYPE*)_T155A;_T155C=
# 3115 "parse.y"
Cyc_yyget_Exp_tok(_T155B);_T155D=yyyvsp;_T155E=_T155D[0];_T155F=_T155E.l;_T1560=_T155F.first_line;_T1561=Cyc_Position_loc_to_seg(_T1560);_T1562=Cyc_Absyn_assert_false_exp(_T155C,_T1561);yyval=Cyc_Exp_tok(_T1562);goto _LL0;case 537:
# 3119 "parse.y"
 yyval=Cyc_YY6(12U);goto _LL0;case 538:
# 3120 "parse.y"
 yyval=Cyc_YY6(11U);goto _LL0;case 539:
# 3121 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 540:
# 3122 "parse.y"
 yyval=Cyc_YY6(0U);goto _LL0;case 541: _T1563=yyyvsp;_T1564=& _T1563[3].v;_T1565=(union Cyc_YYSTYPE*)_T1564;{
# 3127 "parse.y"
struct _tuple31*_T1870=Cyc_yyget_YY64(_T1565);struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;struct Cyc_List_List*_T1873;{struct _tuple31 _T1874=*_T1870;_T1873=_T1874.f0;_T1872=_T1874.f1;_T1871=_T1874.f2;}{struct Cyc_List_List*outlist=_T1873;struct Cyc_List_List*inlist=_T1872;struct Cyc_List_List*clobbers=_T1871;{struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T1874=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_T1874->tag=39;_T1567=yyyvsp;_T1568=& _T1567[0].v;_T1569=(union Cyc_YYSTYPE*)_T1568;
_T1874->f1=Cyc_yyget_YY32(_T1569);_T156A=yyyvsp;_T156B=& _T156A[2].v;_T156C=(union Cyc_YYSTYPE*)_T156B;_T1874->f2=Cyc_yyget_String_tok(_T156C);_T1874->f3=outlist;_T1874->f4=inlist;_T1874->f5=clobbers;_T1566=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_T1874;}_T156D=(void*)_T1566;yyval=Cyc_YY63(_T156D);goto _LL0;}}case 542:
# 3132 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 543:
# 3133 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 544:{struct _tuple31*_T1870=_cycalloc(sizeof(struct _tuple31));
# 3137 "parse.y"
_T1870->f0=0;_T1870->f1=0;_T1870->f2=0;_T156E=(struct _tuple31*)_T1870;}yyval=Cyc_YY64(_T156E);goto _LL0;case 545: _T156F=yyyvsp;_T1570=& _T156F[1].v;_T1571=(union Cyc_YYSTYPE*)_T1570;{
# 3139 "parse.y"
struct _tuple28*_T1870=Cyc_yyget_YY65(_T1571);struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;{struct _tuple28 _T1873=*_T1870;_T1872=_T1873.f0;_T1871=_T1873.f1;}{struct Cyc_List_List*inlist=_T1872;struct Cyc_List_List*clobbers=_T1871;{struct _tuple31*_T1873=_cycalloc(sizeof(struct _tuple31));
_T1873->f0=0;_T1873->f1=inlist;_T1873->f2=clobbers;_T1572=(struct _tuple31*)_T1873;}yyval=Cyc_YY64(_T1572);goto _LL0;}}case 546: _T1573=yyyvsp;_T1574=& _T1573[2].v;_T1575=(union Cyc_YYSTYPE*)_T1574;{
# 3142 "parse.y"
struct _tuple28*_T1870=Cyc_yyget_YY65(_T1575);struct Cyc_List_List*_T1871;struct Cyc_List_List*_T1872;{struct _tuple28 _T1873=*_T1870;_T1872=_T1873.f0;_T1871=_T1873.f1;}{struct Cyc_List_List*inlist=_T1872;struct Cyc_List_List*clobbers=_T1871;{struct _tuple31*_T1873=_cycalloc(sizeof(struct _tuple31));_T1577=yyyvsp;_T1578=& _T1577[1].v;_T1579=(union Cyc_YYSTYPE*)_T1578;_T157A=
Cyc_yyget_YY67(_T1579);_T1873->f0=Cyc_List_imp_rev(_T157A);_T1873->f1=inlist;_T1873->f2=clobbers;_T1576=(struct _tuple31*)_T1873;}yyval=Cyc_YY64(_T1576);goto _LL0;}}case 547:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T157C=yyyvsp;_T157D=& _T157C[0].v;_T157E=(union Cyc_YYSTYPE*)_T157D;
# 3146
_T1870->hd=Cyc_yyget_YY68(_T157E);_T1870->tl=0;_T157B=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY67(_T157B);goto _LL0;case 548:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T1580=yyyvsp;_T1581=& _T1580[2].v;_T1582=(union Cyc_YYSTYPE*)_T1581;
# 3147 "parse.y"
_T1870->hd=Cyc_yyget_YY68(_T1582);_T1583=yyyvsp;_T1584=& _T1583[0].v;_T1585=(union Cyc_YYSTYPE*)_T1584;_T1870->tl=Cyc_yyget_YY67(_T1585);_T157F=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY67(_T157F);goto _LL0;case 549:{struct _tuple28*_T1870=_cycalloc(sizeof(struct _tuple28));
# 3151 "parse.y"
_T1870->f0=0;_T1870->f1=0;_T1586=(struct _tuple28*)_T1870;}yyval=Cyc_YY65(_T1586);goto _LL0;case 550:{struct _tuple28*_T1870=_cycalloc(sizeof(struct _tuple28));
# 3152 "parse.y"
_T1870->f0=0;_T1588=yyyvsp;_T1589=& _T1588[1].v;_T158A=(union Cyc_YYSTYPE*)_T1589;_T1870->f1=Cyc_yyget_YY66(_T158A);_T1587=(struct _tuple28*)_T1870;}yyval=Cyc_YY65(_T1587);goto _LL0;case 551:{struct _tuple28*_T1870=_cycalloc(sizeof(struct _tuple28));_T158C=yyyvsp;_T158D=& _T158C[1].v;_T158E=(union Cyc_YYSTYPE*)_T158D;_T158F=
# 3153 "parse.y"
Cyc_yyget_YY67(_T158E);_T1870->f0=Cyc_List_imp_rev(_T158F);_T1590=yyyvsp;_T1591=& _T1590[2].v;_T1592=(union Cyc_YYSTYPE*)_T1591;_T1870->f1=Cyc_yyget_YY66(_T1592);_T158B=(struct _tuple28*)_T1870;}yyval=Cyc_YY65(_T158B);goto _LL0;case 552:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T1594=yyyvsp;_T1595=& _T1594[0].v;_T1596=(union Cyc_YYSTYPE*)_T1595;
# 3156
_T1870->hd=Cyc_yyget_YY68(_T1596);_T1870->tl=0;_T1593=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY67(_T1593);goto _LL0;case 553:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T1598=yyyvsp;_T1599=& _T1598[2].v;_T159A=(union Cyc_YYSTYPE*)_T1599;
# 3157 "parse.y"
_T1870->hd=Cyc_yyget_YY68(_T159A);_T159B=yyyvsp;_T159C=& _T159B[0].v;_T159D=(union Cyc_YYSTYPE*)_T159C;_T1870->tl=Cyc_yyget_YY67(_T159D);_T1597=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY67(_T1597);goto _LL0;case 554:{struct _tuple32*_T1870=_cycalloc(sizeof(struct _tuple32));_T159F=yyyvsp;_T15A0=& _T159F[0].v;_T15A1=(union Cyc_YYSTYPE*)_T15A0;
# 3161 "parse.y"
_T1870->f0=Cyc_yyget_String_tok(_T15A1);_T15A2=yyyvsp;_T15A3=& _T15A2[2].v;_T15A4=(union Cyc_YYSTYPE*)_T15A3;_T1870->f1=Cyc_yyget_Exp_tok(_T15A4);_T159E=(struct _tuple32*)_T1870;}yyval=Cyc_YY68(_T159E);goto _LL0;case 555:
# 3166 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 556:
# 3167 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 557: _T15A5=yyyvsp;_T15A6=& _T15A5[1].v;_T15A7=(union Cyc_YYSTYPE*)_T15A6;_T15A8=
# 3168 "parse.y"
Cyc_yyget_YY66(_T15A7);_T15A9=Cyc_List_imp_rev(_T15A8);yyval=Cyc_YY66(_T15A9);goto _LL0;case 558:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T15AC=yyyvsp;_T15AD=& _T15AC[0].v;_T15AE=(union Cyc_YYSTYPE*)_T15AD;
# 3171
*_T1871=Cyc_yyget_String_tok(_T15AE);_T15AB=(struct _fat_ptr*)_T1871;}_T1870->hd=_T15AB;_T1870->tl=0;_T15AA=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY66(_T15AA);goto _LL0;case 559:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T15B1=yyyvsp;_T15B2=& _T15B1[2].v;_T15B3=(union Cyc_YYSTYPE*)_T15B2;
# 3172 "parse.y"
*_T1871=Cyc_yyget_String_tok(_T15B3);_T15B0=(struct _fat_ptr*)_T1871;}_T1870->hd=_T15B0;_T15B4=yyyvsp;_T15B5=& _T15B4[0].v;_T15B6=(union Cyc_YYSTYPE*)_T15B5;_T1870->tl=Cyc_yyget_YY66(_T15B6);_T15AF=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY66(_T15AF);goto _LL0;case 560: _T15B7=yyyvsp;_T15B8=_T15B7[0];
# 3177 "parse.y"
yyval=_T15B8.v;goto _LL0;case 561: _T15B9=yyyvsp;_T15BA=& _T15B9[0].v;_T15BB=(union Cyc_YYSTYPE*)_T15BA;_T15BC=
# 3179 "parse.y"
Cyc_yyget_Exp_tok(_T15BB);_T15BD=yyyvsp;_T15BE=& _T15BD[2].v;_T15BF=(union Cyc_YYSTYPE*)_T15BE;_T15C0=Cyc_yyget_Exp_tok(_T15BF);_T15C1=yyyvsp;_T15C2=_T15C1[0];_T15C3=_T15C2.l;_T15C4=_T15C3.first_line;_T15C5=Cyc_Position_loc_to_seg(_T15C4);_T15C6=Cyc_Absyn_subscript_exp(_T15BC,_T15C0,_T15C5);yyval=Cyc_Exp_tok(_T15C6);goto _LL0;case 562: _T15C7=yyyvsp;_T15C8=& _T15C7[0].v;_T15C9=(union Cyc_YYSTYPE*)_T15C8;_T15CA=
# 3181 "parse.y"
Cyc_yyget_Exp_tok(_T15C9);_T15CB=yyyvsp;_T15CC=_T15CB[0];_T15CD=_T15CC.l;_T15CE=_T15CD.first_line;_T15CF=Cyc_Position_loc_to_seg(_T15CE);_T15D0=Cyc_Absyn_unknowncall_exp(_T15CA,0,_T15CF);yyval=Cyc_Exp_tok(_T15D0);goto _LL0;case 563: _T15D1=yyyvsp;_T15D2=& _T15D1[0].v;_T15D3=(union Cyc_YYSTYPE*)_T15D2;_T15D4=
# 3183 "parse.y"
Cyc_yyget_Exp_tok(_T15D3);_T15D5=yyyvsp;_T15D6=& _T15D5[2].v;_T15D7=(union Cyc_YYSTYPE*)_T15D6;_T15D8=Cyc_yyget_YY4(_T15D7);_T15D9=yyyvsp;_T15DA=_T15D9[0];_T15DB=_T15DA.l;_T15DC=_T15DB.first_line;_T15DD=Cyc_Position_loc_to_seg(_T15DC);_T15DE=Cyc_Absyn_unknowncall_exp(_T15D4,_T15D8,_T15DD);yyval=Cyc_Exp_tok(_T15DE);goto _LL0;case 564: _T15DF=yyyvsp;_T15E0=& _T15DF[0].v;_T15E1=(union Cyc_YYSTYPE*)_T15E0;_T15E2=
# 3185 "parse.y"
Cyc_yyget_Exp_tok(_T15E1);{struct _fat_ptr*_T1870=_cycalloc(sizeof(struct _fat_ptr));_T15E4=yyyvsp;_T15E5=& _T15E4[2].v;_T15E6=(union Cyc_YYSTYPE*)_T15E5;*_T1870=Cyc_yyget_String_tok(_T15E6);_T15E3=(struct _fat_ptr*)_T1870;}_T15E7=yyyvsp;_T15E8=_T15E7[0];_T15E9=_T15E8.l;_T15EA=_T15E9.first_line;_T15EB=Cyc_Position_loc_to_seg(_T15EA);_T15EC=Cyc_Absyn_aggrmember_exp(_T15E2,_T15E3,_T15EB);yyval=Cyc_Exp_tok(_T15EC);goto _LL0;case 565: _T15ED=yyyvsp;_T15EE=& _T15ED[0].v;_T15EF=(union Cyc_YYSTYPE*)_T15EE;_T15F0=
# 3187 "parse.y"
Cyc_yyget_Exp_tok(_T15EF);{struct _fat_ptr*_T1870=_cycalloc(sizeof(struct _fat_ptr));_T15F2=yyyvsp;_T15F3=& _T15F2[2].v;_T15F4=(union Cyc_YYSTYPE*)_T15F3;*_T1870=Cyc_yyget_String_tok(_T15F4);_T15F1=(struct _fat_ptr*)_T1870;}_T15F5=yyyvsp;_T15F6=_T15F5[0];_T15F7=_T15F6.l;_T15F8=_T15F7.first_line;_T15F9=Cyc_Position_loc_to_seg(_T15F8);_T15FA=Cyc_Absyn_aggrarrow_exp(_T15F0,_T15F1,_T15F9);yyval=Cyc_Exp_tok(_T15FA);goto _LL0;case 566: _T15FB=yyyvsp;_T15FC=& _T15FB[0].v;_T15FD=(union Cyc_YYSTYPE*)_T15FC;_T15FE=
# 3189 "parse.y"
Cyc_yyget_Exp_tok(_T15FD);_T15FF=yyyvsp;_T1600=_T15FF[0];_T1601=_T1600.l;_T1602=_T1601.first_line;_T1603=Cyc_Position_loc_to_seg(_T1602);_T1604=Cyc_Absyn_increment_exp(_T15FE,1U,_T1603);yyval=Cyc_Exp_tok(_T1604);goto _LL0;case 567: _T1605=yyyvsp;_T1606=& _T1605[0].v;_T1607=(union Cyc_YYSTYPE*)_T1606;_T1608=
# 3191 "parse.y"
Cyc_yyget_Exp_tok(_T1607);_T1609=yyyvsp;_T160A=_T1609[0];_T160B=_T160A.l;_T160C=_T160B.first_line;_T160D=Cyc_Position_loc_to_seg(_T160C);_T160E=Cyc_Absyn_increment_exp(_T1608,3U,_T160D);yyval=Cyc_Exp_tok(_T160E);goto _LL0;case 568:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1870->tag=24;_T1610=yyyvsp;_T1611=& _T1610[1].v;_T1612=(union Cyc_YYSTYPE*)_T1611;
# 3193 "parse.y"
_T1870->f1=Cyc_yyget_YY38(_T1612);_T1870->f2=0;_T160F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1870;}_T1613=(void*)_T160F;_T1614=yyyvsp;_T1615=_T1614[0];_T1616=_T1615.l;_T1617=_T1616.first_line;_T1618=Cyc_Position_loc_to_seg(_T1617);_T1619=Cyc_Absyn_new_exp(_T1613,_T1618);yyval=Cyc_Exp_tok(_T1619);goto _LL0;case 569:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1870->tag=24;_T161B=yyyvsp;_T161C=& _T161B[1].v;_T161D=(union Cyc_YYSTYPE*)_T161C;
# 3195 "parse.y"
_T1870->f1=Cyc_yyget_YY38(_T161D);_T161E=yyyvsp;_T161F=& _T161E[4].v;_T1620=(union Cyc_YYSTYPE*)_T161F;_T1621=Cyc_yyget_YY5(_T1620);_T1870->f2=Cyc_List_imp_rev(_T1621);_T161A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1870;}_T1622=(void*)_T161A;_T1623=yyyvsp;_T1624=_T1623[0];_T1625=_T1624.l;_T1626=_T1625.first_line;_T1627=Cyc_Position_loc_to_seg(_T1626);_T1628=Cyc_Absyn_new_exp(_T1622,_T1627);yyval=Cyc_Exp_tok(_T1628);goto _LL0;case 570:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1870->tag=24;_T162A=yyyvsp;_T162B=& _T162A[1].v;_T162C=(union Cyc_YYSTYPE*)_T162B;
# 3197 "parse.y"
_T1870->f1=Cyc_yyget_YY38(_T162C);_T162D=yyyvsp;_T162E=& _T162D[4].v;_T162F=(union Cyc_YYSTYPE*)_T162E;_T1630=Cyc_yyget_YY5(_T162F);_T1870->f2=Cyc_List_imp_rev(_T1630);_T1629=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1870;}_T1631=(void*)_T1629;_T1632=yyyvsp;_T1633=_T1632[0];_T1634=_T1633.l;_T1635=_T1634.first_line;_T1636=Cyc_Position_loc_to_seg(_T1635);_T1637=Cyc_Absyn_new_exp(_T1631,_T1636);yyval=Cyc_Exp_tok(_T1637);goto _LL0;case 571:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1871->tag=0;{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));_T163B=yyyvsp;_T163C=& _T163B[0].v;_T163D=(union Cyc_YYSTYPE*)_T163C;
# 3202 "parse.y"
*_T1872=Cyc_yyget_String_tok(_T163D);_T163A=(struct _fat_ptr*)_T1872;}_T1871->f1=_T163A;_T1639=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1871;}_T1870->hd=(void*)_T1639;_T1870->tl=0;_T1638=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY3(_T1638);goto _LL0;case 572: _T163E=yyyvsp;_T163F=_T163E[0];_T1640=_T163F.l;_T1641=_T1640.first_line;_T1642=
# 3205
Cyc_Position_loc_to_seg(_T1641);_T1643=yyyvsp;_T1644=& _T1643[0].v;_T1645=(union Cyc_YYSTYPE*)_T1644;_T1646=Cyc_yyget_Int_tok(_T1645);{unsigned i=Cyc_Parse_cnst2uint(_T1642,_T1646);_T1647=i;_T1648=(int)_T1647;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1870=Cyc_Absyn_tuple_field_designator(_T1648);struct _fat_ptr*_T1871;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1872=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1870;_T1871=_T1872->f1;}{struct _fat_ptr*f=_T1871;{struct Cyc_List_List*_T1872=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1873->tag=0;
_T1873->f1=f;_T164A=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1873;}_T1872->hd=(void*)_T164A;_T1872->tl=0;_T1649=(struct Cyc_List_List*)_T1872;}yyval=Cyc_YY3(_T1649);goto _LL0;}}}case 573:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1871=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1871->tag=0;{struct _fat_ptr*_T1872=_cycalloc(sizeof(struct _fat_ptr));_T164E=yyyvsp;_T164F=& _T164E[2].v;_T1650=(union Cyc_YYSTYPE*)_T164F;
# 3209 "parse.y"
*_T1872=Cyc_yyget_String_tok(_T1650);_T164D=(struct _fat_ptr*)_T1872;}_T1871->f1=_T164D;_T164C=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1871;}_T1870->hd=(void*)_T164C;_T1651=yyyvsp;_T1652=& _T1651[0].v;_T1653=(union Cyc_YYSTYPE*)_T1652;_T1870->tl=Cyc_yyget_YY3(_T1653);_T164B=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY3(_T164B);goto _LL0;case 574: _T1654=yyyvsp;_T1655=_T1654[2];_T1656=_T1655.l;_T1657=_T1656.first_line;_T1658=
# 3212
Cyc_Position_loc_to_seg(_T1657);_T1659=yyyvsp;_T165A=& _T1659[2].v;_T165B=(union Cyc_YYSTYPE*)_T165A;_T165C=Cyc_yyget_Int_tok(_T165B);{unsigned i=Cyc_Parse_cnst2uint(_T1658,_T165C);_T165D=i;_T165E=(int)_T165D;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1870=Cyc_Absyn_tuple_field_designator(_T165E);struct _fat_ptr*_T1871;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1872=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1870;_T1871=_T1872->f1;}{struct _fat_ptr*f=_T1871;{struct Cyc_List_List*_T1872=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1873=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1873->tag=0;
_T1873->f1=f;_T1660=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1873;}_T1872->hd=(void*)_T1660;_T1661=yyyvsp;_T1662=& _T1661[0].v;_T1663=(union Cyc_YYSTYPE*)_T1662;_T1872->tl=Cyc_yyget_YY3(_T1663);_T165F=(struct Cyc_List_List*)_T1872;}yyval=Cyc_YY3(_T165F);goto _LL0;}}}case 575: _T1664=yyyvsp;_T1665=& _T1664[0].v;_T1666=(union Cyc_YYSTYPE*)_T1665;_T1667=
# 3219 "parse.y"
Cyc_yyget_QualId_tok(_T1666);_T1668=yyyvsp;_T1669=_T1668[0];_T166A=_T1669.l;_T166B=_T166A.first_line;_T166C=Cyc_Position_loc_to_seg(_T166B);_T166D=Cyc_Absyn_unknownid_exp(_T1667,_T166C);yyval=Cyc_Exp_tok(_T166D);goto _LL0;case 576: _T166E=yyyvsp;_T166F=& _T166E[2].v;_T1670=(union Cyc_YYSTYPE*)_T166F;_T1671=
# 3220 "parse.y"
Cyc_yyget_String_tok(_T1670);_T1672=yyyvsp;_T1673=_T1672[0];_T1674=_T1673.l;_T1675=_T1674.first_line;_T1676=Cyc_Position_loc_to_seg(_T1675);_T1677=Cyc_Absyn_pragma_exp(_T1671,_T1676);yyval=Cyc_Exp_tok(_T1677);goto _LL0;case 577: _T1678=yyyvsp;_T1679=_T1678[0];
# 3221 "parse.y"
yyval=_T1679.v;goto _LL0;case 578: _T167A=yyyvsp;_T167B=& _T167A[0].v;_T167C=(union Cyc_YYSTYPE*)_T167B;_T167D=
# 3222 "parse.y"
Cyc_yyget_String_tok(_T167C);_T167E=yyyvsp;_T167F=_T167E[0];_T1680=_T167F.l;_T1681=_T1680.first_line;_T1682=Cyc_Position_loc_to_seg(_T1681);_T1683=Cyc_Absyn_string_exp(_T167D,_T1682);yyval=Cyc_Exp_tok(_T1683);goto _LL0;case 579: _T1684=yyyvsp;_T1685=& _T1684[0].v;_T1686=(union Cyc_YYSTYPE*)_T1685;_T1687=
# 3223 "parse.y"
Cyc_yyget_String_tok(_T1686);_T1688=yyyvsp;_T1689=_T1688[0];_T168A=_T1689.l;_T168B=_T168A.first_line;_T168C=Cyc_Position_loc_to_seg(_T168B);_T168D=Cyc_Absyn_wstring_exp(_T1687,_T168C);yyval=Cyc_Exp_tok(_T168D);goto _LL0;case 580: _T168E=yyyvsp;_T168F=_T168E[1];
# 3224 "parse.y"
yyval=_T168F.v;goto _LL0;case 581: _T1690=yyyvsp;_T1691=& _T1690[0].v;_T1692=(union Cyc_YYSTYPE*)_T1691;_T1693=
# 3228 "parse.y"
Cyc_yyget_Exp_tok(_T1692);_T1694=yyyvsp;_T1695=_T1694[0];_T1696=_T1695.l;_T1697=_T1696.first_line;_T1698=Cyc_Position_loc_to_seg(_T1697);_T1699=Cyc_Absyn_noinstantiate_exp(_T1693,_T1698);yyval=Cyc_Exp_tok(_T1699);goto _LL0;case 582: _T169A=yyyvsp;_T169B=& _T169A[0].v;_T169C=(union Cyc_YYSTYPE*)_T169B;_T169D=
# 3231
Cyc_yyget_Exp_tok(_T169C);_T169E=yyyvsp;_T169F=& _T169E[3].v;_T16A0=(union Cyc_YYSTYPE*)_T169F;_T16A1=Cyc_yyget_YY41(_T16A0);_T16A2=Cyc_List_imp_rev(_T16A1);_T16A3=yyyvsp;_T16A4=_T16A3[0];_T16A5=_T16A4.l;_T16A6=_T16A5.first_line;_T16A7=Cyc_Position_loc_to_seg(_T16A6);_T16A8=Cyc_Absyn_instantiate_exp(_T169D,_T16A2,_T16A7);yyval=Cyc_Exp_tok(_T16A8);goto _LL0;case 583:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T1870=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T1870->tag=28;_T16AA=yyyvsp;_T16AB=& _T16AA[0].v;_T16AC=(union Cyc_YYSTYPE*)_T16AB;
# 3234
_T1870->f1=Cyc_yyget_QualId_tok(_T16AC);_T16AD=yyyvsp;_T16AE=& _T16AD[2].v;_T16AF=(union Cyc_YYSTYPE*)_T16AE;_T1870->f2=Cyc_yyget_YY41(_T16AF);_T16B0=yyyvsp;_T16B1=& _T16B0[3].v;_T16B2=(union Cyc_YYSTYPE*)_T16B1;_T16B3=Cyc_yyget_YY5(_T16B2);_T1870->f3=Cyc_List_imp_rev(_T16B3);_T1870->f4=0;_T16A9=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T1870;}_T16B4=(void*)_T16A9;_T16B5=yyyvsp;_T16B6=_T16B5[0];_T16B7=_T16B6.l;_T16B8=_T16B7.first_line;_T16B9=Cyc_Position_loc_to_seg(_T16B8);_T16BA=Cyc_Absyn_new_exp(_T16B4,_T16B9);yyval=Cyc_Exp_tok(_T16BA);goto _LL0;case 584: _T16BB=yyyvsp;_T16BC=& _T16BB[2].v;_T16BD=(union Cyc_YYSTYPE*)_T16BC;_T16BE=
# 3236 "parse.y"
Cyc_yyget_YY4(_T16BD);_T16BF=yyyvsp;_T16C0=_T16BF[0];_T16C1=_T16C0.l;_T16C2=_T16C1.first_line;_T16C3=Cyc_Position_loc_to_seg(_T16C2);_T16C4=Cyc_Absyn_tuple_exp(_T16BE,_T16C3);yyval=Cyc_Exp_tok(_T16C4);goto _LL0;case 585: _T16C5=yyyvsp;_T16C6=& _T16C5[2].v;_T16C7=(union Cyc_YYSTYPE*)_T16C6;_T16C8=
# 3238 "parse.y"
Cyc_yyget_Stmt_tok(_T16C7);_T16C9=yyyvsp;_T16CA=_T16C9[0];_T16CB=_T16CA.l;_T16CC=_T16CB.first_line;_T16CD=Cyc_Position_loc_to_seg(_T16CC);_T16CE=Cyc_Absyn_stmt_exp(_T16C8,_T16CD);yyval=Cyc_Exp_tok(_T16CE);goto _LL0;case 586: _T16CF=yyyvsp;_T16D0=& _T16CF[0].v;_T16D1=(union Cyc_YYSTYPE*)_T16D0;_T16D2=
# 3242 "parse.y"
Cyc_yyget_YY4(_T16D1);_T16D3=Cyc_List_imp_rev(_T16D2);yyval=Cyc_YY4(_T16D3);goto _LL0;case 587:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T16D5=yyyvsp;_T16D6=& _T16D5[0].v;_T16D7=(union Cyc_YYSTYPE*)_T16D6;
# 3247 "parse.y"
_T1870->hd=Cyc_yyget_Exp_tok(_T16D7);_T1870->tl=0;_T16D4=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY4(_T16D4);goto _LL0;case 588:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T16D9=yyyvsp;_T16DA=& _T16D9[2].v;_T16DB=(union Cyc_YYSTYPE*)_T16DA;
# 3249 "parse.y"
_T1870->hd=Cyc_yyget_Exp_tok(_T16DB);_T16DC=yyyvsp;_T16DD=& _T16DC[0].v;_T16DE=(union Cyc_YYSTYPE*)_T16DD;_T1870->tl=Cyc_yyget_YY4(_T16DE);_T16D8=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY4(_T16D8);goto _LL0;case 589: _T16DF=yyyvsp;_T16E0=& _T16DF[0].v;_T16E1=(union Cyc_YYSTYPE*)_T16E0;_T16E2=
# 3255 "parse.y"
Cyc_yyget_Int_tok(_T16E1);_T16E3=yyyvsp;_T16E4=_T16E3[0];_T16E5=_T16E4.l;_T16E6=_T16E5.first_line;_T16E7=Cyc_Position_loc_to_seg(_T16E6);_T16E8=Cyc_Absyn_const_exp(_T16E2,_T16E7);yyval=Cyc_Exp_tok(_T16E8);goto _LL0;case 590: _T16E9=yyyvsp;_T16EA=& _T16E9[0].v;_T16EB=(union Cyc_YYSTYPE*)_T16EA;_T16EC=
# 3256 "parse.y"
Cyc_yyget_Char_tok(_T16EB);_T16ED=yyyvsp;_T16EE=_T16ED[0];_T16EF=_T16EE.l;_T16F0=_T16EF.first_line;_T16F1=Cyc_Position_loc_to_seg(_T16F0);_T16F2=Cyc_Absyn_char_exp(_T16EC,_T16F1);yyval=Cyc_Exp_tok(_T16F2);goto _LL0;case 591: _T16F3=yyyvsp;_T16F4=& _T16F3[0].v;_T16F5=(union Cyc_YYSTYPE*)_T16F4;_T16F6=
# 3257 "parse.y"
Cyc_yyget_String_tok(_T16F5);_T16F7=yyyvsp;_T16F8=_T16F7[0];_T16F9=_T16F8.l;_T16FA=_T16F9.first_line;_T16FB=Cyc_Position_loc_to_seg(_T16FA);_T16FC=Cyc_Absyn_wchar_exp(_T16F6,_T16FB);yyval=Cyc_Exp_tok(_T16FC);goto _LL0;case 592: _T16FD=yyyvsp;_T16FE=_T16FD[0];_T16FF=_T16FE.l;_T1700=_T16FF.first_line;_T1701=
# 3259 "parse.y"
Cyc_Position_loc_to_seg(_T1700);_T1702=Cyc_Absyn_null_exp(_T1701);yyval=Cyc_Exp_tok(_T1702);goto _LL0;case 593: _T1703=yyyvsp;_T1704=& _T1703[0].v;_T1705=(union Cyc_YYSTYPE*)_T1704;{
# 3261 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(_T1705);_T1706=
Cyc_strlen(f);{int l=(int)_T1706;
int i=1;
if(l <= 0)goto _TL2EB;_T1707=f;_T1708=_T1707.curr;_T1709=(const char*)_T1708;_T170A=l - 1;{
char c=_T1709[_T170A];_T170B=c;_T170C=(int)_T170B;
if(_T170C==102)goto _TL2EF;else{goto _TL2F0;}_TL2F0: _T170D=c;_T170E=(int)_T170D;if(_T170E==70)goto _TL2EF;else{goto _TL2ED;}_TL2EF: i=0;goto _TL2EE;
_TL2ED: _T170F=c;_T1710=(int)_T170F;if(_T1710==108)goto _TL2F3;else{goto _TL2F4;}_TL2F4: _T1711=c;_T1712=(int)_T1711;if(_T1712==76)goto _TL2F3;else{goto _TL2F1;}_TL2F3: i=2;goto _TL2F2;_TL2F1: _TL2F2: _TL2EE:;}goto _TL2EC;_TL2EB: _TL2EC: _T1713=f;_T1714=i;_T1715=yyyvsp;_T1716=_T1715[0];_T1717=_T1716.l;_T1718=_T1717.first_line;_T1719=
# 3269
Cyc_Position_loc_to_seg(_T1718);_T171A=Cyc_Absyn_float_exp(_T1713,_T1714,_T1719);yyval=Cyc_Exp_tok(_T171A);goto _LL0;}}case 594:{struct _tuple0*_T1870=_cycalloc(sizeof(struct _tuple0));
# 3274 "parse.y"
_T1870->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T171D=yyyvsp;_T171E=& _T171D[0].v;_T171F=(union Cyc_YYSTYPE*)_T171E;*_T1871=Cyc_yyget_String_tok(_T171F);_T171C=(struct _fat_ptr*)_T1871;}_T1870->f1=_T171C;_T171B=(struct _tuple0*)_T1870;}yyval=Cyc_QualId_tok(_T171B);goto _LL0;case 595: _T1720=yyyvsp;_T1721=_T1720[0];
# 3275 "parse.y"
yyval=_T1721.v;goto _LL0;case 596:{struct _tuple0*_T1870=_cycalloc(sizeof(struct _tuple0));
# 3278
_T1870->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1871=_cycalloc(sizeof(struct _fat_ptr));_T1724=yyyvsp;_T1725=& _T1724[0].v;_T1726=(union Cyc_YYSTYPE*)_T1725;*_T1871=Cyc_yyget_String_tok(_T1726);_T1723=(struct _fat_ptr*)_T1871;}_T1870->f1=_T1723;_T1722=(struct _tuple0*)_T1870;}yyval=Cyc_QualId_tok(_T1722);goto _LL0;case 597: _T1727=yyyvsp;_T1728=_T1727[0];
# 3279 "parse.y"
yyval=_T1728.v;goto _LL0;case 598: _T1729=yyyvsp;_T172A=_T1729[0];
# 3284 "parse.y"
yyval=_T172A.v;goto _LL0;case 599: _T172B=yyyvsp;_T172C=_T172B[0];
# 3285 "parse.y"
yyval=_T172C.v;goto _LL0;case 600: _T172D=yyyvsp;_T172E=_T172D[0];
# 3288
yyval=_T172E.v;goto _LL0;case 601: _T172F=yyyvsp;_T1730=_T172F[0];
# 3289 "parse.y"
yyval=_T1730.v;goto _LL0;case 602: goto _LL0;case 603: _T1731=yylex_buf;
# 3294 "parse.y"
_T1731->lex_curr_pos=_T1731->lex_curr_pos - 1;goto _LL0;case 604:
# 3298 "parse.y"
 yyval=Cyc_YY71(0);goto _LL0;case 605:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple35*_T1871=_cycalloc(sizeof(struct _tuple35));_T1734=yyyvsp;_T1735=& _T1734[1].v;_T1736=(union Cyc_YYSTYPE*)_T1735;_T1737=
# 3300 "parse.y"
Cyc_yyget_String_tok(_T1736);_T1738=yyyvsp;_T1739=& _T1738[3].v;_T173A=(union Cyc_YYSTYPE*)_T1739;_T173B=Cyc_yyget_String_tok(_T173A);_T173C=_tag_fat("true",sizeof(char),5U);_T173D=Cyc_strcmp(_T173B,_T173C);_T173E=_T173D==0;_T173F=yyyvsp;_T1740=& _T173F[0].v;_T1741=(union Cyc_YYSTYPE*)_T1740;_T1742=Cyc_yyget_String_tok(_T1741);_T1743=Cyc_Parse_typevar2cvar(_T1742);_T1871->f0=Cyc_Parse_assign_cvar_pos(_T1737,_T173E,_T1743);_T1744=yyyvsp;_T1745=& _T1744[5].v;_T1746=(union Cyc_YYSTYPE*)_T1745;_T1871->f1=Cyc_yyget_YY72(_T1746);_T1733=(struct _tuple35*)_T1871;}_T1870->hd=_T1733;_T1747=yyyvsp;_T1748=& _T1747[7].v;_T1749=(union Cyc_YYSTYPE*)_T1748;_T1870->tl=Cyc_yyget_YY71(_T1749);_T1732=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY71(_T1732);goto _LL0;case 606:
# 3304 "parse.y"
 yyval=Cyc_YY72(0);goto _LL0;case 607: _T174A=yyyvsp;_T174B=& _T174A[0].v;_T174C=(union Cyc_YYSTYPE*)_T174B;_T174D=
# 3305 "parse.y"
Cyc_yyget_YY72(_T174C);yyval=Cyc_YY72(_T174D);goto _LL0;case 608:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T174F=yyyvsp;_T1750=& _T174F[0].v;_T1751=(union Cyc_YYSTYPE*)_T1750;
# 3309 "parse.y"
_T1870->hd=Cyc_yyget_YY73(_T1751);_T1870->tl=0;_T174E=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY72(_T174E);goto _LL0;case 609:{struct Cyc_List_List*_T1870=_cycalloc(sizeof(struct Cyc_List_List));_T1753=yyyvsp;_T1754=& _T1753[0].v;_T1755=(union Cyc_YYSTYPE*)_T1754;
# 3310 "parse.y"
_T1870->hd=Cyc_yyget_YY73(_T1755);_T1756=yyyvsp;_T1757=& _T1756[2].v;_T1758=(union Cyc_YYSTYPE*)_T1757;_T1870->tl=Cyc_yyget_YY72(_T1758);_T1752=(struct Cyc_List_List*)_T1870;}yyval=Cyc_YY72(_T1752);goto _LL0;case 610: _T1759=yyyvsp;_T175A=& _T1759[0].v;_T175B=(union Cyc_YYSTYPE*)_T175A;_T175C=
# 3314 "parse.y"
Cyc_yyget_String_tok(_T175B);_T175D=Cyc_Parse_typevar2cvar(_T175C);yyval=Cyc_YY45(_T175D);goto _LL0;case 611: _T175E=yyyvsp;_T175F=_T175E[0];_T1760=_T175F.l;_T1761=_T1760.first_line;_T1762=
# 3315 "parse.y"
Cyc_Position_loc_to_seg(_T1761);_T1763=yyyvsp;_T1764=& _T1763[0].v;_T1765=(union Cyc_YYSTYPE*)_T1764;_T1766=Cyc_yyget_String_tok(_T1765);_T1767=Cyc_Parse_str2type(_T1762,_T1766);yyval=Cyc_YY45(_T1767);goto _LL0;case 612: _T1768=yyyvsp;_T1769=& _T1768[0].v;_T176A=(union Cyc_YYSTYPE*)_T1769;_T176B=
# 3319 "parse.y"
Cyc_yyget_String_tok(_T176A);_T176C=yyyvsp;_T176D=& _T176C[3].v;_T176E=(union Cyc_YYSTYPE*)_T176D;_T176F=Cyc_yyget_YY73(_T176E);_T1770=Cyc_BansheeIf_check_constraint(_T176F);_T1771=Cyc_BansheeIf_add_location(_T176B,_T1770);yyval=Cyc_YY73(_T1771);goto _LL0;case 613: _T1772=yyyvsp;_T1773=& _T1772[0].v;_T1774=(union Cyc_YYSTYPE*)_T1773;_T1775=
# 3320 "parse.y"
Cyc_yyget_String_tok(_T1774);_T1776=yyyvsp;_T1777=& _T1776[3].v;_T1778=(union Cyc_YYSTYPE*)_T1777;_T1779=Cyc_yyget_YY70(_T1778);_T177A=yyyvsp;_T177B=& _T177A[5].v;_T177C=(union Cyc_YYSTYPE*)_T177B;_T177D=Cyc_yyget_YY45(_T177C);_T177E=yyyvsp;_T177F=& _T177E[7].v;_T1780=(union Cyc_YYSTYPE*)_T177F;_T1781=Cyc_yyget_YY45(_T1780);_T1782=Cyc_Parse_comparison_constraint(_T1779,_T177D,_T1781);_T1783=Cyc_BansheeIf_add_location(_T1775,_T1782);yyval=Cyc_YY73(_T1783);goto _LL0;case 614: _T1784=yyyvsp;_T1785=& _T1784[0].v;_T1786=(union Cyc_YYSTYPE*)_T1785;_T1787=
# 3321 "parse.y"
Cyc_yyget_String_tok(_T1786);_T1788=yyyvsp;_T1789=& _T1788[3].v;_T178A=(union Cyc_YYSTYPE*)_T1789;_T178B=Cyc_yyget_YY45(_T178A);_T178C=yyyvsp;_T178D=& _T178C[5].v;_T178E=(union Cyc_YYSTYPE*)_T178D;_T178F=Cyc_yyget_YY45(_T178E);_T1790=Cyc_BansheeIf_cond_equality_constraint(_T178B,_T178F);_T1791=Cyc_BansheeIf_add_location(_T1787,_T1790);yyval=Cyc_YY73(_T1791);goto _LL0;case 615: _T1792=yyyvsp;_T1793=& _T1792[0].v;_T1794=(union Cyc_YYSTYPE*)_T1793;_T1795=
# 3322 "parse.y"
Cyc_yyget_String_tok(_T1794);_T1796=yyyvsp;_T1797=& _T1796[3].v;_T1798=(union Cyc_YYSTYPE*)_T1797;_T1799=Cyc_yyget_YY45(_T1798);_T179A=yyyvsp;_T179B=& _T179A[5].v;_T179C=(union Cyc_YYSTYPE*)_T179B;_T179D=Cyc_yyget_YY45(_T179C);_T179E=Cyc_BansheeIf_equality_constraint(_T1799,_T179D);_T179F=Cyc_BansheeIf_add_location(_T1795,_T179E);yyval=Cyc_YY73(_T179F);goto _LL0;case 616: _T17A0=yyyvsp;_T17A1=& _T17A0[0].v;_T17A2=(union Cyc_YYSTYPE*)_T17A1;_T17A3=
# 3323 "parse.y"
Cyc_yyget_String_tok(_T17A2);_T17A4=yyyvsp;_T17A5=& _T17A4[3].v;_T17A6=(union Cyc_YYSTYPE*)_T17A5;_T17A7=Cyc_yyget_YY45(_T17A6);_T17A8=yyyvsp;_T17A9=& _T17A8[5].v;_T17AA=(union Cyc_YYSTYPE*)_T17A9;_T17AB=Cyc_yyget_YY45(_T17AA);_T17AC=Cyc_BansheeIf_inclusion_constraint(_T17A7,_T17AB);_T17AD=Cyc_BansheeIf_add_location(_T17A3,_T17AC);yyval=Cyc_YY73(_T17AD);goto _LL0;case 617: _T17AE=yyyvsp;_T17AF=& _T17AE[0].v;_T17B0=(union Cyc_YYSTYPE*)_T17AF;_T17B1=
# 3324 "parse.y"
Cyc_yyget_String_tok(_T17B0);_T17B2=yyyvsp;_T17B3=& _T17B2[3].v;_T17B4=(union Cyc_YYSTYPE*)_T17B3;_T17B5=Cyc_yyget_YY73(_T17B4);_T17B6=yyyvsp;_T17B7=& _T17B6[5].v;_T17B8=(union Cyc_YYSTYPE*)_T17B7;_T17B9=Cyc_yyget_YY73(_T17B8);_T17BA=Cyc_BansheeIf_implication_constraint(_T17B5,_T17B9);_T17BB=Cyc_BansheeIf_add_location(_T17B1,_T17BA);yyval=Cyc_YY73(_T17BB);goto _LL0;case 618: _T17BC=yyyvsp;_T17BD=& _T17BC[0].v;_T17BE=(union Cyc_YYSTYPE*)_T17BD;_T17BF=
# 3325 "parse.y"
Cyc_yyget_String_tok(_T17BE);_T17C0=yyyvsp;_T17C1=& _T17C0[3].v;_T17C2=(union Cyc_YYSTYPE*)_T17C1;_T17C3=Cyc_yyget_YY70(_T17C2);_T17C4=yyyvsp;_T17C5=& _T17C4[5].v;_T17C6=(union Cyc_YYSTYPE*)_T17C5;_T17C7=Cyc_yyget_YY73(_T17C6);_T17C8=yyyvsp;_T17C9=& _T17C8[7].v;_T17CA=(union Cyc_YYSTYPE*)_T17C9;_T17CB=Cyc_yyget_YY73(_T17CA);_T17CC=Cyc_Parse_composite_constraint(_T17C3,_T17C7,_T17CB);_T17CD=Cyc_BansheeIf_add_location(_T17BF,_T17CC);yyval=Cyc_YY73(_T17CD);goto _LL0;case 619:
# 3328
 yyval=Cyc_YY70(0U);goto _LL0;case 620:
# 3329 "parse.y"
 yyval=Cyc_YY70(1U);goto _LL0;case 621:
# 3330 "parse.y"
 yyval=Cyc_YY70(2U);goto _LL0;case 622:
# 3331 "parse.y"
 yyval=Cyc_YY70(3U);goto _LL0;case 623:
# 3332 "parse.y"
 yyval=Cyc_YY70(4U);goto _LL0;default: goto _LL0;}_LL0: _T17CE=yylen;
# 375 "cycbison.simple"
yyvsp_offset=yyvsp_offset - _T17CE;_T17CF=yylen;
yyssp_offset=yyssp_offset - _T17CF;_T17D0=yyvs;
# 389 "cycbison.simple"
yyvsp_offset=yyvsp_offset + 1;_T17D1=yyvsp_offset;_T17D2=_check_fat_subscript(_T17D0,sizeof(struct Cyc_Yystacktype),_T17D1);_T17D3=(struct Cyc_Yystacktype*)_T17D2;(*_T17D3).v=yyval;
# 392
if(yylen!=0)goto _TL2F5;_T17D4=yyvs;_T17D5=yyvsp_offset - 1;_T17D6=
_fat_ptr_plus(_T17D4,sizeof(struct Cyc_Yystacktype),_T17D5);_T17D7=_untag_fat_ptr_check_bound(_T17D6,sizeof(struct Cyc_Yystacktype),2U);_T17D8=_check_null(_T17D7);{struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_T17D8;_T17D9=p;_T17DA=yylloc;
_T17D9[1].l.first_line=_T17DA.first_line;_T17DB=p;_T17DC=yylloc;
_T17DB[1].l.first_column=_T17DC.first_column;_T17DD=p;_T17DE=p;_T17DF=_T17DE[0];_T17E0=_T17DF.l;
_T17DD[1].l.last_line=_T17E0.last_line;_T17E1=p;_T17E2=p;_T17E3=_T17E2[0];_T17E4=_T17E3.l;
_T17E1[1].l.last_column=_T17E4.last_column;}goto _TL2F6;
# 399
_TL2F5: _T17E5=yyvs;_T17E6=_T17E5.curr;_T17E7=(struct Cyc_Yystacktype*)_T17E6;_T17E8=yyvsp_offset;_T17E9=yyvs;_T17EA=yyvsp_offset + yylen;_T17EB=_T17EA - 1;_T17EC=_check_fat_subscript(_T17E9,sizeof(struct Cyc_Yystacktype),_T17EB);_T17ED=(struct Cyc_Yystacktype*)_T17EC;_T17EE=*_T17ED;_T17EF=_T17EE.l;_T17E7[_T17E8].l.last_line=_T17EF.last_line;_T17F0=yyvs;_T17F1=_T17F0.curr;_T17F2=(struct Cyc_Yystacktype*)_T17F1;_T17F3=yyvsp_offset;_T17F4=yyvs;_T17F5=_T17F4.curr;_T17F6=(struct Cyc_Yystacktype*)_T17F5;_T17F7=yyvsp_offset + yylen;_T17F8=_T17F7 - 1;_T17F9=_T17F6[_T17F8];_T17FA=_T17F9.l;
_T17F2[_T17F3].l.last_column=_T17FA.last_column;_TL2F6: _T17FB=Cyc_yyr1;_T17FC=yyn;_T17FD=_check_known_subscript_notnull(_T17FB,624U,sizeof(short),_T17FC);_T17FE=(short*)_T17FD;_T17FF=*_T17FE;
# 409
yyn=(int)_T17FF;_T1800=Cyc_yypgoto;_T1801=yyn - 175;_T1802=_check_known_subscript_notnull(_T1800,188U,sizeof(short),_T1801);_T1803=(short*)_T1802;_T1804=*_T1803;_T1805=(int)_T1804;_T1806=yyss;_T1807=yyssp_offset;_T1808=_check_fat_subscript(_T1806,sizeof(short),_T1807);_T1809=(short*)_T1808;_T180A=*_T1809;_T180B=(int)_T180A;
# 411
yystate=_T1805 + _T180B;
if(yystate < 0)goto _TL2F7;if(yystate > 8241)goto _TL2F7;_T180C=Cyc_yycheck;_T180D=yystate;_T180E=_T180C[_T180D];_T180F=(int)_T180E;_T1810=yyss;_T1811=_T1810.curr;_T1812=(short*)_T1811;_T1813=yyssp_offset;_T1814=_T1812[_T1813];_T1815=(int)_T1814;if(_T180F!=_T1815)goto _TL2F7;_T1816=Cyc_yytable;_T1817=yystate;_T1818=_T1816[_T1817];
yystate=(int)_T1818;goto _TL2F8;
# 415
_TL2F7: _T1819=Cyc_yydefgoto;_T181A=yyn - 175;_T181B=_check_known_subscript_notnull(_T1819,188U,sizeof(short),_T181A);_T181C=(short*)_T181B;_T181D=*_T181C;yystate=(int)_T181D;_TL2F8: goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus!=0)goto _TL2F9;
# 424
yynerrs=yynerrs + 1;_T181E=Cyc_yypact;_T181F=yystate;_T1820=_check_known_subscript_notnull(_T181E,1262U,sizeof(short),_T181F);_T1821=(short*)_T1820;_T1822=*_T1821;
# 427
yyn=(int)_T1822;
# 429
if(yyn <= -32768)goto _TL2FB;if(yyn >= 8241)goto _TL2FB;{
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
if(yyn >= 0)goto _TL2FD;_T1823=- yyn;goto _TL2FE;_TL2FD: _T1823=0;_TL2FE: x=_T1823;
_TL302: _T1824=x;_T1825=(unsigned)_T1824;_T1826=363U / sizeof(char*);
# 437
if(_T1825 < _T1826)goto _TL300;else{goto _TL301;}
# 439
_TL300: _T1827=Cyc_yycheck;_T1828=x + yyn;_T1829=_check_known_subscript_notnull(_T1827,8242U,sizeof(short),_T1828);_T182A=(short*)_T1829;_T182B=*_T182A;_T182C=(int)_T182B;_T182D=x;if(_T182C!=_T182D)goto _TL303;_T182E=Cyc_yytname;_T182F=x;_T1830=_check_known_subscript_notnull(_T182E,363U,sizeof(struct _fat_ptr),_T182F);_T1831=(struct _fat_ptr*)_T1830;_T1832=*_T1831;_T1833=
Cyc_strlen(_T1832);_T1834=_T1833 + 15U;sze=sze + _T1834;count=count + 1;goto _TL304;_TL303: _TL304:
# 438
 x=x + 1;goto _TL302;_TL301: _T1836=sze + 15;_T1837=(unsigned)_T1836;{unsigned _T1870=_T1837 + 1U;_T1839=yyregion;_T183A=_check_times(_T1870,sizeof(char));{char*_T1871=_region_malloc(_T1839,0U,_T183A);_T183B=sze + 15;{unsigned _T1872=(unsigned)_T183B;unsigned i;i=0;_TL308: if(i < _T1872)goto _TL306;else{goto _TL307;}_TL306: _T183C=i;
# 441
_T1871[_T183C]='\000';i=i + 1;goto _TL308;_TL307: _T1871[_T1872]=0;}_T1838=(char*)_T1871;}_T1835=_tag_fat(_T1838,sizeof(char),_T1870);}msg=_T1835;_T183D=msg;_T183E=
_tag_fat("parse error",sizeof(char),12U);Cyc_strcpy(_T183D,_T183E);
# 444
if(count >= 5)goto _TL309;
# 446
count=0;
if(yyn >= 0)goto _TL30B;_T183F=- yyn;goto _TL30C;_TL30B: _T183F=0;_TL30C: x=_T183F;
_TL310: _T1840=x;_T1841=(unsigned)_T1840;_T1842=363U / sizeof(char*);
# 447
if(_T1841 < _T1842)goto _TL30E;else{goto _TL30F;}
# 449
_TL30E: _T1843=Cyc_yycheck;_T1844=x + yyn;_T1845=_check_known_subscript_notnull(_T1843,8242U,sizeof(short),_T1844);_T1846=(short*)_T1845;_T1847=*_T1846;_T1848=(int)_T1847;_T1849=x;if(_T1848!=_T1849)goto _TL311;_T184A=msg;
# 451
if(count!=0)goto _TL313;_T184C=
_tag_fat(", expecting `",sizeof(char),14U);_T184B=_T184C;goto _TL314;_TL313: _T184D=
_tag_fat(" or `",sizeof(char),6U);_T184B=_T184D;_TL314:
# 451
 Cyc_strcat(_T184A,_T184B);_T184E=msg;_T184F=Cyc_yytname;_T1850=x;_T1851=_check_known_subscript_notnull(_T184F,363U,sizeof(struct _fat_ptr),_T1850);_T1852=(struct _fat_ptr*)_T1851;_T1853=*_T1852;
# 454
Cyc_strcat(_T184E,_T1853);_T1854=msg;_T1855=
_tag_fat("'",sizeof(char),2U);Cyc_strcat(_T1854,_T1855);
count=count + 1;goto _TL312;_TL311: _TL312:
# 448
 x=x + 1;goto _TL310;_TL30F: goto _TL30A;_TL309: _TL30A:
# 459
 Cyc_yyerror(msg,yystate,yychar);}goto _TL2FC;
# 463
_TL2FB: _T1856=_tag_fat("parse error",sizeof(char),12U);_T1857=yystate;_T1858=yychar;Cyc_yyerror(_T1856,_T1857,_T1858);_TL2FC: goto _TL2FA;_TL2F9: _TL2FA: goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus!=3)goto _TL315;
# 474
if(yychar!=0)goto _TL317;{int _T1870=1;_npop_handler(0);return _T1870;}_TL317:
# 483
 yychar=-2;goto _TL316;_TL315: _TL316:
# 489
 yyerrstatus=3;goto yyerrhandle;
# 493
yyerrdefault:
# 503 "cycbison.simple"
 yyerrpop:
# 505
 if(yyssp_offset!=0)goto _TL319;{int _T1870=1;_npop_handler(0);return _T1870;}_TL319:
 yyvsp_offset=yyvsp_offset + -1;_T1859=yyss;
yyssp_offset=yyssp_offset + -1;_T185A=yyssp_offset;_T185B=_check_fat_subscript(_T1859,sizeof(short),_T185A);_T185C=(short*)_T185B;_T185D=*_T185C;yystate=(int)_T185D;
# 521 "cycbison.simple"
yyerrhandle: _T185E=Cyc_yypact;_T185F=yystate;_T1860=_check_known_subscript_notnull(_T185E,1262U,sizeof(short),_T185F);_T1861=(short*)_T1860;_T1862=*_T1861;
yyn=(int)_T1862;
if(yyn!=-32768)goto _TL31B;goto yyerrdefault;_TL31B:
# 525
 yyn=yyn + 1;
if(yyn < 0)goto _TL31F;else{goto _TL321;}_TL321: if(yyn > 8241)goto _TL31F;else{goto _TL320;}_TL320: _T1863=Cyc_yycheck;_T1864=yyn;_T1865=_T1863[_T1864];_T1866=(int)_T1865;if(_T1866!=1)goto _TL31F;else{goto _TL31D;}_TL31F: goto yyerrdefault;_TL31D: _T1867=Cyc_yytable;_T1868=yyn;_T1869=_T1867[_T1868];
# 528
yyn=(int)_T1869;
if(yyn >= 0)goto _TL322;
# 531
if(yyn!=-32768)goto _TL324;goto yyerrpop;_TL324:
 yyn=- yyn;goto yyreduce;
# 535
_TL322: if(yyn!=0)goto _TL326;goto yyerrpop;_TL326:
# 537
 if(yyn!=1261)goto _TL328;{int _T1870=0;_npop_handler(0);return _T1870;}_TL328: _T186A=yyvs;
# 546
yyvsp_offset=yyvsp_offset + 1;_T186B=yyvsp_offset;_T186C=_check_fat_subscript(_T186A,sizeof(struct Cyc_Yystacktype),_T186B);_T186D=(struct Cyc_Yystacktype*)_T186C;{struct Cyc_Yystacktype _T1870;_T1870.v=yylval;_T1870.l=yylloc;_T186E=_T1870;}*_T186D=_T186E;goto yynewstate;}}}}_pop_region();}
# 3336 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _T0;struct _union_YYSTYPE_Stmt_tok _T1;unsigned _T2;union Cyc_YYSTYPE _T3;struct _union_YYSTYPE_Int_tok _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct Cyc___cycFILE*_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;union Cyc_YYSTYPE _T9;struct _union_YYSTYPE_Char_tok _TA;struct Cyc_Int_pa_PrintArg_struct _TB;char _TC;int _TD;struct Cyc___cycFILE*_TE;struct _fat_ptr _TF;struct _fat_ptr _T10;union Cyc_YYSTYPE _T11;struct _union_YYSTYPE_String_tok _T12;struct Cyc_String_pa_PrintArg_struct _T13;struct Cyc___cycFILE*_T14;struct _fat_ptr _T15;struct _fat_ptr _T16;union Cyc_YYSTYPE _T17;struct _union_YYSTYPE_QualId_tok _T18;struct Cyc_String_pa_PrintArg_struct _T19;struct Cyc___cycFILE*_T1A;struct _fat_ptr _T1B;struct _fat_ptr _T1C;union Cyc_YYSTYPE _T1D;struct _union_YYSTYPE_Exp_tok _T1E;struct Cyc_String_pa_PrintArg_struct _T1F;struct Cyc___cycFILE*_T20;struct _fat_ptr _T21;struct _fat_ptr _T22;union Cyc_YYSTYPE _T23;struct _union_YYSTYPE_Stmt_tok _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct Cyc___cycFILE*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct Cyc___cycFILE*_T29;struct _fat_ptr _T2A;struct _fat_ptr _T2B;struct Cyc_Absyn_Stmt*_T2C;struct Cyc_Absyn_Exp*_T2D;struct _tuple0*_T2E;struct _fat_ptr _T2F;char _T30;union Cyc_Absyn_Cnst _T31;_T0=v;_T1=_T0.Stmt_tok;_T2=_T1.tag;switch(_T2){case 2: _T3=v;_T4=_T3.Int_tok;_T31=_T4.val;{union Cyc_Absyn_Cnst c=_T31;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
# 3338
_T32.f1=Cyc_Absynpp_cnst2string(c);_T5=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T5;void*_T33[1];_T33[0]=& _T32;_T6=Cyc_stderr;_T7=_tag_fat("%s",sizeof(char),3U);_T8=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T6,_T7,_T8);}goto _LL0;}case 3: _T9=v;_TA=_T9.Char_tok;_T30=_TA.val;{char c=_T30;{struct Cyc_Int_pa_PrintArg_struct _T32;_T32.tag=1;_TC=c;_TD=(int)_TC;
_T32.f1=(unsigned long)_TD;_TB=_T32;}{struct Cyc_Int_pa_PrintArg_struct _T32=_TB;void*_T33[1];_T33[0]=& _T32;_TE=Cyc_stderr;_TF=_tag_fat("%c",sizeof(char),3U);_T10=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_TE,_TF,_T10);}goto _LL0;}case 4: _T11=v;_T12=_T11.String_tok;_T2F=_T12.val;{struct _fat_ptr s=_T2F;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
_T32.f1=s;_T13=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T13;void*_T33[1];_T33[0]=& _T32;_T14=Cyc_stderr;_T15=_tag_fat("\"%s\"",sizeof(char),5U);_T16=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T14,_T15,_T16);}goto _LL0;}case 5: _T17=v;_T18=_T17.QualId_tok;_T2E=_T18.val;{struct _tuple0*q=_T2E;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
_T32.f1=Cyc_Absynpp_qvar2string(q);_T19=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T19;void*_T33[1];_T33[0]=& _T32;_T1A=Cyc_stderr;_T1B=_tag_fat("%s",sizeof(char),3U);_T1C=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T1A,_T1B,_T1C);}goto _LL0;}case 6: _T1D=v;_T1E=_T1D.Exp_tok;_T2D=_T1E.val;{struct Cyc_Absyn_Exp*e=_T2D;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
_T32.f1=Cyc_Absynpp_exp2string(e);_T1F=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T1F;void*_T33[1];_T33[0]=& _T32;_T20=Cyc_stderr;_T21=_tag_fat("%s",sizeof(char),3U);_T22=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T20,_T21,_T22);}goto _LL0;}case 7: _T23=v;_T24=_T23.Stmt_tok;_T2C=_T24.val;{struct Cyc_Absyn_Stmt*s=_T2C;{struct Cyc_String_pa_PrintArg_struct _T32;_T32.tag=0;
_T32.f1=Cyc_Absynpp_stmt2string(s);_T25=_T32;}{struct Cyc_String_pa_PrintArg_struct _T32=_T25;void*_T33[1];_T33[0]=& _T32;_T26=Cyc_stderr;_T27=_tag_fat("%s",sizeof(char),3U);_T28=_tag_fat(_T33,sizeof(void*),1);Cyc_fprintf(_T26,_T27,_T28);}goto _LL0;}default: _T29=Cyc_stderr;_T2A=
_tag_fat("?",sizeof(char),2U);_T2B=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T29,_T2A,_T2B);goto _LL0;}_LL0:;}
# 3348
struct _fat_ptr Cyc_token2string(int token){struct _fat_ptr _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;int _T3;short*_T4;int _T5;short _T6;int _T7;unsigned _T8;struct _fat_ptr*_T9;int _TA;char*_TB;struct _fat_ptr*_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;
if(token > 0)goto _TL32B;_T0=
_tag_fat("end-of-file",sizeof(char),12U);return _T0;_TL32B:
 if(token!=387)goto _TL32D;_T1=Cyc_Lex_token_string;
return _T1;_TL32D:
 if(token!=396)goto _TL32F;_T2=
Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);return _T2;_TL32F:
 if(token <= 0)goto _TL331;if(token > 400)goto _TL331;_T4=Cyc_yytranslate;_T5=token;_T6=_T4[_T5];_T3=(int)_T6;goto _TL332;_TL331: _T3=363;_TL332: {int z=_T3;_T7=z;_T8=(unsigned)_T7;
if(_T8 >= 363U)goto _TL333;_T9=Cyc_yytname;_TA=z;_TB=_check_known_subscript_notnull(_T9,363U,sizeof(struct _fat_ptr),_TA);_TC=(struct _fat_ptr*)_TB;_TD=*_TC;
return _TD;_TL333: _TE=
_tag_fat(0,0,0);return _TE;}}
# 3362
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){struct _RegionHandle*_T0;struct Cyc_Lexing_lexbuf*_T1;
Cyc_Parse_parse_result=0;{struct _RegionHandle _T2=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _T2;_push_region(yyr);_T0=yyr;_T1=
# 3365
Cyc_Lexing_from_file(f);Cyc_yyparse(_T0,_T1);{struct Cyc_List_List*_T3=Cyc_Parse_parse_result;_npop_handler(0);return _T3;}_pop_region();}}
# 3369
struct Cyc_List_List*Cyc_Parse_parse_constraint_file(struct Cyc___cycFILE*f){struct _RegionHandle*_T0;struct Cyc_Lexing_lexbuf*_T1;
Cyc_Parse_constraint_graph=0;{struct _RegionHandle _T2=_new_region(0U,"yyr");struct _RegionHandle*yyr=& _T2;_push_region(yyr);_T0=yyr;_T1=
# 3372
Cyc_Lexing_from_file(f);Cyc_yyparse(_T0,_T1);{struct Cyc_List_List*_T3=Cyc_Parse_constraint_graph;_npop_handler(0);return _T3;}_pop_region();}}

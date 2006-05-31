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
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){struct Cyc_Warn_String_Warn_Warg_struct _T0;void**_T1;unsigned _T2;struct _fat_ptr _T3;struct Cyc_Parse_Exit_exn_struct*_T4;void*_T5;{struct Cyc_Warn_String_Warn_Warg_struct _T6;_T6.tag=0;
_T6.f1=msg;_T0=_T6;}{struct Cyc_Warn_String_Warn_Warg_struct _T6=_T0;void*_T7[1];_T1=_T7 + 0;*_T1=& _T6;_T2=loc;_T3=_tag_fat(_T7,sizeof(void*),1);Cyc_Warn_err2(_T2,_T3);}{struct Cyc_Parse_Exit_exn_struct*_T6=_cycalloc(sizeof(struct Cyc_Parse_Exit_exn_struct));
_T6->tag=Cyc_Parse_Exit;_T4=(struct Cyc_Parse_Exit_exn_struct*)_T6;}_T5=(void*)_T4;_throw(_T5);}
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
static void Cyc_Parse_check_single_constraint(unsigned loc,struct _fat_ptr id){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;struct Cyc_String_pa_PrintArg_struct _T3;void**_T4;unsigned _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;_T0=id;_T1=
_tag_fat("single",sizeof(char),7U);_T2=Cyc_zstrcmp(_T0,_T1);if(!_T2)goto _TL13;{struct Cyc_String_pa_PrintArg_struct _T8;_T8.tag=0;
_T8.f1=_tag_fat(" is not a valid effect constraint",sizeof(char),34U);_T3=_T8;}{struct Cyc_String_pa_PrintArg_struct _T8=_T3;void*_T9[1];_T4=_T9 + 0;*_T4=& _T8;_T5=loc;_T6=id;_T7=_tag_fat(_T9,sizeof(void*),1);Cyc_Warn_err(_T5,_T6,_T7);}goto _TL14;_TL13: _TL14:;}
# 233
static void*Cyc_Parse_effect_from_atomic(struct Cyc_List_List*effs){int _T0;struct Cyc_List_List*_T1;void*_T2;void*_T3;_T0=
Cyc_List_length(effs);if(_T0!=1)goto _TL15;_T1=
_check_null(effs);_T2=_T1->hd;return _T2;
# 237
_TL15: _T3=Cyc_Absyn_join_eff(effs);return _T3;}
# 240
static struct _tuple0*Cyc_Parse_gensym_enum (void){struct _tuple0*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct Cyc_Int_pa_PrintArg_struct _T3;int _T4;int _T5;void**_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;
static int enum_counter=0;{struct _tuple0*_T9=_cycalloc(sizeof(struct _tuple0));
_T9->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_TA=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _TB;_TB.tag=1;_T4=enum_counter;
enum_counter=_T4 + 1;_T5=_T4;_TB.f1=(unsigned long)_T5;_T3=_TB;}{struct Cyc_Int_pa_PrintArg_struct _TB=_T3;void*_TC[1];_T6=_TC + 0;*_T6=& _TB;_T7=_tag_fat("__anonymous_enum_%d__",sizeof(char),22U);_T8=_tag_fat(_TC,sizeof(void*),1);_T2=Cyc_aprintf(_T7,_T8);}*_TA=_T2;_T1=(struct _fat_ptr*)_TA;}_T9->f1=_T1;_T0=(struct _tuple0*)_T9;}
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
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){struct Cyc_List_List*_T0;void*_T1;struct _tuple8*_T2;void*_T3;int*_T4;int _T5;struct _tuple8*_T6;void*_T7;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T8;void*_T9;int*_TA;int _TB;struct _tuple8*_TC;void*_TD;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TE;struct Cyc_List_List*_TF;struct _tuple8*_T10;void*_T11;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T12;struct Cyc_List_List*_T13;struct Cyc_List_List*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_List_List*_T17;void*_T18;struct _fat_ptr*_T19;void*_T1A;int*_T1B;int _T1C;void*_T1D;void*_T1E;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T1F;void**_T20;struct _fat_ptr*_T21;struct _fat_ptr _T22;struct Cyc_String_pa_PrintArg_struct _T23;struct _fat_ptr*_T24;void**_T25;struct _fat_ptr _T26;struct _fat_ptr _T27;void**_T28;struct Cyc_Absyn_Tvar*_T29;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T2A;struct Cyc_Absyn_Kind*_T2B;struct Cyc_List_List*_T2C;struct _tuple18*_T2D;struct _tuple8*_T2E;struct _fat_ptr*_T2F;struct _tuple8*_T30;struct _fat_ptr*_T31;struct _tuple8*_T32;struct _fat_ptr*_T33;struct _tuple8*_T34;struct _fat_ptr*_T35;struct _tuple8*_T36;void*_T37;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T38;void*_T39;int*_T3A;int _T3B;struct _tuple8*_T3C;void*_T3D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3E;struct Cyc_List_List*_T3F;struct _tuple8*_T40;void*_T41;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T42;struct Cyc_List_List*_T43;struct Cyc_List_List*_T44;void*_T45;int*_T46;int _T47;struct _tuple8*_T48;void*_T49;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T4A;struct Cyc_List_List*_T4B;struct Cyc_List_List*_T4C;struct Cyc_List_List*_T4D;struct _fat_ptr*_T4E;void*_T4F;struct Cyc_List_List*_T50;void*_T51;void*_T52;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T53;struct Cyc_List_List*_T54;void*_T55;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T56;void**_T57;struct _fat_ptr*_T58;struct _fat_ptr _T59;struct Cyc_String_pa_PrintArg_struct _T5A;void**_T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;void**_T5E;struct Cyc_Absyn_Tvar*_T5F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T60;struct Cyc_Absyn_Kind*_T61;struct _tuple8*_T62;struct _fat_ptr*_T63;struct Cyc_List_List*_T64;struct Cyc_List_List*_T65;
struct Cyc_List_List*res=0;
_TL22: if(x!=0)goto _TL20;else{goto _TL21;}
_TL20: _T0=x;_T1=_T0->hd;{struct _tuple8*_T66=(struct _tuple8*)_T1;struct _fat_ptr _T67;void*_T68;void*_T69;_T2=(struct _tuple8*)_T66;_T3=_T2->f2;_T4=(int*)_T3;_T5=*_T4;if(_T5!=0)goto _TL23;_T6=(struct _tuple8*)_T66;_T7=_T6->f2;_T8=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7;_T9=_T8->f1;_TA=(int*)_T9;_TB=*_TA;if(_TB!=5)goto _TL25;_TC=(struct _tuple8*)_T66;_TD=_TC->f2;_TE=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_TD;_TF=_TE->f2;if(_TF==0)goto _TL27;_T10=(struct _tuple8*)_T66;_T11=_T10->f2;_T12=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T11;_T13=_T12->f2;_T14=(struct Cyc_List_List*)_T13;_T15=_T14->tl;if(_T15!=0)goto _TL29;{struct _tuple8 _T6A=*_T66;_T69=_T6A.f0;_T16=_T6A.f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T16;_T17=_T6B->f2;{struct Cyc_List_List _T6C=*_T17;_T18=_T6C.hd;_T68=(void*)_T18;}}}_T19=(struct _fat_ptr*)_T69;if(_T19==0)goto _TL2B;{struct _fat_ptr*v=_T69;void*i=_T68;{void*_T6A;_T1A=i;_T1B=(int*)_T1A;_T1C=*_T1B;if(_T1C!=1)goto _TL2D;_T1D=i;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T1D;_T1E=i;_T1F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T1E;_T20=& _T1F->f2;_T6A=(void**)_T20;}{void**z=(void**)_T6A;
# 330
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr));_T21=nm;{struct Cyc_String_pa_PrintArg_struct _T6B;_T6B.tag=0;_T24=v;_T6B.f1=*_T24;_T23=_T6B;}{struct Cyc_String_pa_PrintArg_struct _T6B=_T23;void*_T6C[1];_T25=_T6C + 0;*_T25=& _T6B;_T26=_tag_fat("`%s",sizeof(char),4U);_T27=_tag_fat(_T6C,sizeof(void*),1);_T22=Cyc_aprintf(_T26,_T27);}*_T21=_T22;_T28=z;{struct Cyc_Absyn_Tvar*_T6B=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_T6B->name=nm;_T6B->identity=-1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T6C=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T6C->tag=0;_T2B=& Cyc_Kinds_ik;_T6C->f1=(struct Cyc_Absyn_Kind*)_T2B;_T2A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T6C;}_T6B->kind=(void*)_T2A;_T6B->aquals_bound=0;_T29=(struct Cyc_Absyn_Tvar*)_T6B;}*_T28=Cyc_Absyn_var_type(_T29);goto _LL7;}_TL2D: goto _LL7;_LL7:;}{struct Cyc_List_List*_T6A=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T6B=_cycalloc(sizeof(struct _tuple18));
# 335
_T6B->f0=v;_T6B->f1=i;_T2D=(struct _tuple18*)_T6B;}_T6A->hd=_T2D;_T6A->tl=res;_T2C=(struct Cyc_List_List*)_T6A;}res=_T2C;goto _LL0;}_TL2B: _T2E=(struct _tuple8*)_T66;_T2F=_T2E->f0;if(_T2F==0)goto _TL2F;goto _LL5;_TL2F: goto _LL5;_TL29: _T30=(struct _tuple8*)_T66;_T31=_T30->f0;if(_T31==0)goto _TL31;goto _LL5;_TL31: goto _LL5;_TL27: _T32=(struct _tuple8*)_T66;_T33=_T32->f0;if(_T33==0)goto _TL33;goto _LL5;_TL33: goto _LL5;_TL25: _T34=(struct _tuple8*)_T66;_T35=_T34->f0;if(_T35==0)goto _TL35;_T36=(struct _tuple8*)_T66;_T37=_T36->f2;_T38=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T37;_T39=_T38->f1;_T3A=(int*)_T39;_T3B=*_T3A;if(_T3B!=4)goto _TL37;_T3C=(struct _tuple8*)_T66;_T3D=_T3C->f2;_T3E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3D;_T3F=_T3E->f2;if(_T3F==0)goto _TL39;_T40=(struct _tuple8*)_T66;_T41=_T40->f2;_T42=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T41;_T43=_T42->f2;_T44=(struct Cyc_List_List*)_T43;_T45=_T44->hd;_T46=(int*)_T45;_T47=*_T46;if(_T47!=1)goto _TL3B;_T48=(struct _tuple8*)_T66;_T49=_T48->f2;_T4A=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T49;_T4B=_T4A->f2;_T4C=(struct Cyc_List_List*)_T4B;_T4D=_T4C->tl;if(_T4D!=0)goto _TL3D;{struct _tuple8 _T6A=*_T66;_T4E=_T6A.f0;{struct _fat_ptr _T6B=*_T4E;_T67=_T6B;}_T4F=_T6A.f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4F;_T50=_T6B->f2;{struct Cyc_List_List _T6C=*_T50;_T51=_T6C.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T6D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T51;_T52=_T66->f2;_T53=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T52;_T54=_T53->f2;_T55=_T54->hd;_T56=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T55;_T57=& _T56->f2;_T69=(void**)_T57;}}}}{struct _fat_ptr v=_T67;void**z=(void**)_T69;
# 339
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr));_T58=nm;{struct Cyc_String_pa_PrintArg_struct _T6A;_T6A.tag=0;_T6A.f1=v;_T5A=_T6A;}{struct Cyc_String_pa_PrintArg_struct _T6A=_T5A;void*_T6B[1];_T5B=_T6B + 0;*_T5B=& _T6A;_T5C=_tag_fat("`%s",sizeof(char),4U);_T5D=_tag_fat(_T6B,sizeof(void*),1);_T59=Cyc_aprintf(_T5C,_T5D);}*_T58=_T59;_T5E=z;{struct Cyc_Absyn_Tvar*_T6A=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_T6A->name=nm;_T6A->identity=-1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T6B=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T6B->tag=0;_T61=& Cyc_Kinds_ek;_T6B->f1=(struct Cyc_Absyn_Kind*)_T61;_T60=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T6B;}_T6A->kind=(void*)_T60;_T6A->aquals_bound=0;_T5F=(struct Cyc_Absyn_Tvar*)_T6A;}*_T5E=Cyc_Absyn_var_type(_T5F);goto _LL0;}_TL3D: goto _LL5;_TL3B: goto _LL5;_TL39: goto _LL5;_TL37: goto _LL5;_TL35: goto _LL5;_TL23: _T62=(struct _tuple8*)_T66;_T63=_T62->f0;if(_T63==0)goto _TL3F;goto _LL5;_TL3F: _LL5: goto _LL0;_LL0:;}_T64=x;
# 323
x=_T64->tl;goto _TL22;_TL21: _T65=res;
# 344
return _T65;}
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
static void*Cyc_Parse_id2aqual(unsigned loc,struct _fat_ptr s){unsigned long _T0;struct _fat_ptr _T1;unsigned char*_T2;const char*_T3;int _T4;void*_T5;void*_T6;void*_T7;unsigned long _T8;struct _fat_ptr _T9;unsigned char*_TA;const char*_TB;char _TC;int _TD;struct _fat_ptr _TE;unsigned char*_TF;const char*_T10;char _T11;int _T12;void*_T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;void**_T16;void**_T17;unsigned _T18;struct _fat_ptr _T19;void*_T1A;_T0=
Cyc_strlen(s);if(_T0!=2U)goto _TL7A;_T1=s;_T2=_T1.curr;_T3=(const char*)_T2;{
char _T1B=_T3[1];_T4=(int)_T1B;switch(_T4){case 65: _T5=Cyc_Absyn_al_qual_type;
return _T5;case 85: _T6=Cyc_Absyn_un_qual_type;
return _T6;case 84: _T7=Cyc_Absyn_rtd_qual_type;
return _T7;default: goto _LL0;}_LL0:;}goto _TL7B;
# 450
_TL7A: _T8=Cyc_strlen(s);if(_T8!=3U)goto _TL7D;_T9=s;_TA=_T9.curr;_TB=(const char*)_TA;_TC=_TB[1];_TD=(int)_TC;
if(_TD!=82)goto _TL7F;_TE=s;_TF=_TE.curr;_T10=(const char*)_TF;_T11=_T10[2];_T12=(int)_T11;if(_T12!=67)goto _TL7F;_T13=Cyc_Absyn_rc_qual_type;
return _T13;_TL7F: goto _TL7E;_TL7D: _TL7E: _TL7B:{struct Cyc_Warn_String_Warn_Warg_struct _T1B;_T1B.tag=0;
# 454
_T1B.f1=_tag_fat("bad aqual bound ",sizeof(char),17U);_T14=_T1B;}{struct Cyc_Warn_String_Warn_Warg_struct _T1B=_T14;{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;_T1C.f1=s;_T15=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_T15;void*_T1D[2];_T16=_T1D + 0;*_T16=& _T1B;_T17=_T1D + 1;*_T17=& _T1C;_T18=loc;_T19=_tag_fat(_T1D,sizeof(void*),2);Cyc_Warn_err2(_T18,_T19);}}_T1A=Cyc_Absyn_al_qual_type;
return _T1A;}
# 458
static struct Cyc_List_List*Cyc_Parse_insert_aqual(struct _RegionHandle*yy,struct Cyc_List_List*qlist,void*aq,unsigned loc){struct Cyc_List_List*_T0;int*_T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;void**_T4;unsigned _T5;struct _fat_ptr _T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct _RegionHandle*_TA;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_TB;struct _RegionHandle*_TC;{
struct Cyc_List_List*l=qlist;_TL84: if(l!=0)goto _TL82;else{goto _TL83;}
_TL82: _T0=l;{void*_TD=_T0->hd;_T1=(int*)_TD;_T2=*_T1;if(_T2!=10)goto _TL85;{struct Cyc_Warn_String_Warn_Warg_struct _TE;_TE.tag=0;
# 462
_TE.f1=_tag_fat("Multiple alias qualifiers",sizeof(char),26U);_T3=_TE;}{struct Cyc_Warn_String_Warn_Warg_struct _TE=_T3;void*_TF[1];_T4=_TF + 0;*_T4=& _TE;_T5=loc;_T6=_tag_fat(_TF,sizeof(void*),1);Cyc_Warn_err2(_T5,_T6);}_T7=qlist;
return _T7;_TL85: goto _LL0;_LL0:;}_T8=l;
# 459
l=_T8->tl;goto _TL84;_TL83:;}_TA=yy;{struct Cyc_List_List*_TD=_region_malloc(_TA,0U,sizeof(struct Cyc_List_List));_TC=yy;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_TE=_region_malloc(_TC,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_TE->tag=10;
# 468
_TE->f1=aq;_TB=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_TE;}_TD->hd=(void*)_TB;_TD->tl=qlist;_T9=(struct Cyc_List_List*)_TD;}return _T9;}
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
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){struct _tuple14 _T0;struct Cyc_List_List*_T1;int*_T2;unsigned _T3;void*_T4;struct Cyc_Absyn_Tqual _T5;void*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct _tuple14 _TA;void*_TB;struct Cyc_Absyn_Tqual _TC;void*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct _tuple14 _T11;void*_T12;void*_T13;int*_T14;int _T15;void*_T16;void*_T17;struct Cyc_List_List*_T18;void*_T19;int _T1A;struct Cyc_List_List*_T1B;struct Cyc_List_List*_T1C;struct Cyc_List_List*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;struct Cyc_List_List*_T22;struct Cyc_List_List*_T23;int*_T24;int _T25;struct Cyc_List_List*_T26;int _T27;struct Cyc_List_List*_T28;struct Cyc_List_List*_T29;struct Cyc_List_List*_T2A;void*_T2B;struct _tuple8*_T2C;struct _tuple8 _T2D;struct _fat_ptr*_T2E;struct Cyc_List_List*_T2F;void*_T30;struct _tuple8*_T31;struct _tuple8 _T32;void*_T33;void*_T34;struct Cyc_List_List*_T35;void*_T36;void**_T37;void**_T38;int*_T39;int _T3A;struct Cyc_Absyn_ArrayInfo _T3B;struct Cyc_Absyn_ArrayInfo _T3C;struct Cyc_Absyn_ArrayInfo _T3D;struct Cyc_Absyn_ArrayInfo _T3E;struct Cyc_Absyn_ArrayInfo _T3F;struct _tuple0*_T40;struct _tuple0*_T41;struct _tuple0*_T42;struct Cyc_List_List*_T43;struct Cyc_Absyn_Exp**_T44;struct _fat_ptr _T45;struct Cyc_List_List*_T46;struct Cyc_List_List*_T47;struct Cyc_List_List*_T48;void*_T49;struct Cyc_List_List*_T4A;struct Cyc_Absyn_Exp*_T4B;struct Cyc_List_List*_T4C;void*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_List_List*_T4F;struct Cyc_List_List*_T50;void*_T51;void**_T52;void**_T53;struct Cyc_List_List*_T54;void**_T55;void*_T56;void**_T57;void**_T58;void*_T59;struct Cyc_List_List*_T5A;struct Cyc_Absyn_Tqual _T5B;unsigned _T5C;struct Cyc_Absyn_Tqual _T5D;void*_T5E;struct Cyc_List_List*_T5F;struct Cyc_List_List*_T60;struct Cyc_List_List*_T61;struct _tuple14 _T62;void*_T63;int(*_T64)(unsigned,struct _fat_ptr);unsigned _T65;struct _fat_ptr _T66;struct Cyc_List_List*_T67;struct Cyc_List_List*_T68;struct _tuple14 _T69;int(*_T6A)(unsigned,struct _fat_ptr);unsigned _T6B;struct _fat_ptr _T6C;struct Cyc_Absyn_Tqual _T6D;struct Cyc_Absyn_PtrInfo _T6E;void*_T6F;struct Cyc_List_List*_T70;struct Cyc_List_List*_T71;struct Cyc_List_List*_T72;struct _tuple14 _T73;struct Cyc_Absyn_Tqual _T74;void*_T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;struct Cyc_List_List*_T78;struct _tuple14 _T79;
# 742
if(tms!=0)goto _TL121;{struct _tuple14 _T7A;_T7A.f0=tq;_T7A.f1=t;_T7A.f2=0;_T7A.f3=atts;_T0=_T7A;}return _T0;_TL121: _T1=tms;{
void*_T7A=_T1->hd;struct Cyc_Absyn_Tqual _T7B;struct Cyc_Absyn_PtrAtts _T7C;struct Cyc_List_List*_T7D;void*_T7E;unsigned _T7F;void*_T80;_T2=(int*)_T7A;_T3=*_T2;switch(_T3){case 0:{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T81=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T7A;_T4=_T81->f1;_T80=(void*)_T4;_T7F=_T81->f2;}{void*zeroterm=_T80;unsigned ztloc=_T7F;_T5=
# 745
Cyc_Absyn_empty_tqual(0U);_T6=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);_T7=atts;_T8=tms;_T9=_T8->tl;_TA=
# 745
Cyc_Parse_apply_tms(_T5,_T6,_T7,_T9);return _TA;}case 1:{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T81=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T7A;_T80=_T81->f1;_TB=_T81->f2;_T7E=(void*)_TB;_T7F=_T81->f3;}{struct Cyc_Absyn_Exp*e=_T80;void*zeroterm=_T7E;unsigned ztloc=_T7F;_TC=
# 748
Cyc_Absyn_empty_tqual(0U);_TD=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);_TE=atts;_TF=tms;_T10=_TF->tl;_T11=
# 748
Cyc_Parse_apply_tms(_TC,_TD,_TE,_T10);return _T11;}case 3:{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T81=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T7A;_T12=_T81->f1;_T80=(void*)_T12;}{void*args=_T80;unsigned _T81;struct Cyc_Absyn_Exp*_T82;struct Cyc_Absyn_Exp*_T83;struct Cyc_Absyn_Exp*_T84;struct Cyc_Absyn_Exp*_T85;struct Cyc_List_List*_T86;struct Cyc_List_List*_T87;void*_T88;struct Cyc_Absyn_VarargInfo*_T89;int _T8A;struct Cyc_List_List*_T8B;_T13=args;_T14=(int*)_T13;_T15=*_T14;if(_T15!=1)goto _TL124;_T16=args;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T8C=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T16;_T8B=_T8C->f1;_T8A=_T8C->f2;_T89=_T8C->f3;_T17=_T8C->f4;_T88=(void*)_T17;_T87=_T8C->f5;_T86=_T8C->f6;_T85=_T8C->f7;_T84=_T8C->f8;_T83=_T8C->f9;_T82=_T8C->f10;}{struct Cyc_List_List*args2=_T8B;int c_vararg=_T8A;struct Cyc_Absyn_VarargInfo*cyc_vararg=_T89;void*eff=_T88;struct Cyc_List_List*effc=_T87;struct Cyc_List_List*qb=_T86;struct Cyc_Absyn_Exp*chks=_T85;struct Cyc_Absyn_Exp*req=_T84;struct Cyc_Absyn_Exp*ens=_T83;struct Cyc_Absyn_Exp*thrw=_T82;
# 753
struct Cyc_List_List*typvars=0;
# 755
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;{
struct Cyc_List_List*as=atts;_TL129: if(as!=0)goto _TL127;else{goto _TL128;}
_TL127: _T18=as;_T19=_T18->hd;_T1A=Cyc_Atts_fntype_att(_T19);if(!_T1A)goto _TL12A;{struct Cyc_List_List*_T8C=_cycalloc(sizeof(struct Cyc_List_List));_T1C=as;
_T8C->hd=_T1C->hd;_T8C->tl=fn_atts;_T1B=(struct Cyc_List_List*)_T8C;}fn_atts=_T1B;goto _TL12B;
# 760
_TL12A:{struct Cyc_List_List*_T8C=_cycalloc(sizeof(struct Cyc_List_List));_T1E=as;_T8C->hd=_T1E->hd;_T8C->tl=new_atts;_T1D=(struct Cyc_List_List*)_T8C;}new_atts=_T1D;_TL12B: _T1F=as;
# 756
as=_T1F->tl;goto _TL129;_TL128:;}_T20=tms;_T21=_T20->tl;
# 762
if(_T21==0)goto _TL12C;_T22=tms;_T23=_T22->tl;{
void*_T8C=_T23->hd;struct Cyc_List_List*_T8D;_T24=(int*)_T8C;_T25=*_T24;if(_T25!=4)goto _TL12E;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T8E=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T8C;_T8D=_T8E->f1;}{struct Cyc_List_List*ts=_T8D;
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
_TL134: _T35=a;_T36=_T35->hd;{struct _tuple8*_T8C=(struct _tuple8*)_T36;void*_T8D;struct Cyc_Absyn_Tqual _T8E;struct _fat_ptr*_T8F;{struct _tuple8 _T90=*_T8C;_T8F=_T90.f0;_T8E=_T90.f1;_T37=& _T8C->f2;_T8D=(void**)_T37;}{struct _fat_ptr*vopt=_T8F;struct Cyc_Absyn_Tqual tq=_T8E;void**t=(void**)_T8D;_T38=t;{
void*_T90=*_T38;unsigned _T91;void*_T92;struct Cyc_Absyn_Exp*_T93;struct Cyc_Absyn_Tqual _T94;void*_T95;_T39=(int*)_T90;_T3A=*_T39;if(_T3A!=5)goto _TL137;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T96=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T90;_T3B=_T96->f1;_T95=_T3B.elt_type;_T3C=_T96->f1;_T94=_T3C.tq;_T3D=_T96->f1;_T93=_T3D.num_elts;_T3E=_T96->f1;_T92=_T3E.zero_term;_T3F=_T96->f1;_T91=_T3F.zt_loc;}{void*et=_T95;struct Cyc_Absyn_Tqual tq=_T94;struct Cyc_Absyn_Exp*neltsopt=_T93;void*zt=_T92;unsigned ztloc=_T91;
# 786
if(neltsopt==0)goto _TL139;if(vopt==0)goto _TL139;{
struct _tuple0*v;v=_cycalloc(sizeof(struct _tuple0));_T40=v;_T40->f0.Loc_n.tag=4U;_T41=v;_T41->f0.Loc_n.val=0;_T42=v;_T42->f1=vopt;{
struct Cyc_Absyn_Exp*nelts=Cyc_Absyn_copy_exp(neltsopt);{struct Cyc_Absyn_Exp*_T96[1];_T44=_T96 + 0;*_T44=
Cyc_Absyn_var_exp(v,0U);_T45=_tag_fat(_T96,sizeof(struct Cyc_Absyn_Exp*),1);_T43=Cyc_List_list(_T45);}{struct Cyc_Absyn_Exp*e2=Cyc_Absyn_primop_exp(18U,_T43,0U);
struct Cyc_Absyn_Exp*new_req=Cyc_Absyn_lte_exp(nelts,e2,0U);{struct Cyc_List_List*_T96=_cycalloc(sizeof(struct Cyc_List_List));
_T96->hd=new_req;_T96->tl=new_requires;_T46=(struct Cyc_List_List*)_T96;}new_requires=_T46;}}}goto _TL13A;_TL139: _TL13A: goto _LL1A;}_TL137: goto _LL1A;_LL1A:;}}}_T47=a;
# 782
a=_T47->tl;goto _TL136;_TL135:;}
# 797
if(new_requires==0)goto _TL13B;{
struct Cyc_Absyn_Exp*r;
if(req==0)goto _TL13D;
r=req;goto _TL13E;
# 802
_TL13D: _T48=new_requires;_T49=_T48->hd;r=(struct Cyc_Absyn_Exp*)_T49;_T4A=new_requires;
new_requires=_T4A->tl;_TL13E:
# 805
 _TL142: if(new_requires!=0)goto _TL140;else{goto _TL141;}
_TL140: _T4B=r;_T4C=new_requires;_T4D=_T4C->hd;_T4E=(struct Cyc_Absyn_Exp*)_T4D;r=Cyc_Absyn_and_exp(_T4B,_T4E,0U);_T4F=new_requires;
# 805
new_requires=_T4F->tl;goto _TL142;_TL141:
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
_TL146: _T50=a;_T51=_T50->hd;{struct _tuple8*_T8C=(struct _tuple8*)_T51;void*_T8D;struct Cyc_Absyn_Tqual _T8E;struct _fat_ptr*_T8F;{struct _tuple8 _T90=*_T8C;_T8F=_T90.f0;_T8E=_T90.f1;_T52=& _T8C->f2;_T8D=(void**)_T52;}{struct _fat_ptr*vopt=_T8F;struct Cyc_Absyn_Tqual tq=_T8E;void**t=(void**)_T8D;
if(tags==0)goto _TL149;_T53=t;_T54=tags;_T55=t;_T56=*_T55;
*_T53=Cyc_Parse_substitute_tags(_T54,_T56);goto _TL14A;_TL149: _TL14A: _T57=t;_T58=t;_T59=*_T58;
*_T57=Cyc_Parse_array2ptr(_T59,1);}}_T5A=a;
# 818
a=_T5A->tl;goto _TL148;_TL147:;}_T5B=tq;_T5C=_T5B.loc;_T5D=
# 830
Cyc_Absyn_empty_tqual(_T5C);_T5E=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,effc,qb,fn_atts,chks,req,ens,thrw);_T5F=new_atts;_T60=
# 835
_check_null(tms);_T61=_T60->tl;_T62=
# 830
Cyc_Parse_apply_tms(_T5D,_T5E,_T5F,_T61);return _T62;}}}_TL124: _T63=args;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T8C=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T63;_T81=_T8C->f2;}{unsigned loc=_T81;{
# 837
int(*_T8C)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T64=_T8C;}_T65=loc;_T66=_tag_fat("function declaration without parameter types",sizeof(char),45U);_T64(_T65,_T66);};}case 4:{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T81=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T7A;_T7D=_T81->f1;_T7F=_T81->f2;}{struct Cyc_List_List*ts=_T7D;unsigned loc=_T7F;_T67=tms;_T68=_T67->tl;
# 844
if(_T68!=0)goto _TL14B;{struct _tuple14 _T81;
_T81.f0=tq;_T81.f1=t;_T81.f2=ts;_T81.f3=atts;_T69=_T81;}return _T69;_TL14B:{
# 849
int(*_T81)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T6A=_T81;}_T6B=loc;_T6C=
_tag_fat("type parameters must appear before function arguments in declarator",sizeof(char),68U);
# 849
_T6A(_T6B,_T6C);}case 2:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_T81=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_T7A;_T7C=_T81->f1;_T7B=_T81->f2;}{struct Cyc_Absyn_PtrAtts ptratts=_T7C;struct Cyc_Absyn_Tqual tq2=_T7B;_T6D=tq2;{struct Cyc_Absyn_PtrInfo _T81;
# 852
_T81.elt_type=t;_T81.elt_tq=tq;_T81.ptr_atts=ptratts;_T6E=_T81;}_T6F=Cyc_Absyn_pointer_type(_T6E);_T70=atts;_T71=tms;_T72=_T71->tl;_T73=Cyc_Parse_apply_tms(_T6D,_T6F,_T70,_T72);return _T73;}default:{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T81=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T7A;_T7F=_T81->f1;_T7D=_T81->f2;}{unsigned loc=_T7F;struct Cyc_List_List*atts2=_T7D;_T74=tq;_T75=t;_T76=
# 857
Cyc_List_append(atts,atts2);_T77=tms;_T78=_T77->tl;_T79=Cyc_Parse_apply_tms(_T74,_T75,_T76,_T78);return _T79;}};}}
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
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){union Cyc_Absyn_Cnst _T0;struct _union_Cnst_LongLong_c _T1;unsigned _T2;union Cyc_Absyn_Cnst _T3;struct _union_Cnst_Int_c _T4;struct _tuple5 _T5;int _T6;unsigned _T7;union Cyc_Absyn_Cnst _T8;struct _union_Cnst_Char_c _T9;struct _tuple3 _TA;char _TB;unsigned _TC;union Cyc_Absyn_Cnst _TD;struct _union_Cnst_LongLong_c _TE;struct _tuple6 _TF;long long _T10;unsigned _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;long long _T14;unsigned _T15;struct Cyc_String_pa_PrintArg_struct _T16;void**_T17;unsigned _T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;long long _T1B;char _T1C;int _T1D;_T0=x;_T1=_T0.LongLong_c;_T2=_T1.tag;switch(_T2){case 5: _T3=x;_T4=_T3.Int_c;_T5=_T4.val;_T1D=_T5.f1;{int i=_T1D;_T6=i;_T7=(unsigned)_T6;
# 1040
return _T7;}case 2: _T8=x;_T9=_T8.Char_c;_TA=_T9.val;_T1C=_TA.f1;{char c=_T1C;_TB=c;_TC=(unsigned)_TB;
return _TC;}case 6: _TD=x;_TE=_TD.LongLong_c;_TF=_TE.val;_T1B=_TF.f1;{long long x=_T1B;_T10=x;{
# 1043
unsigned long long y=(unsigned long long)_T10;
if(y <= 4294967295U)goto _TL189;_T11=loc;_T12=
_tag_fat("integer constant too large",sizeof(char),27U);_T13=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T11,_T12,_T13);goto _TL18A;_TL189: _TL18A: _T14=x;_T15=(unsigned)_T14;
return _T15;}}default:{struct Cyc_String_pa_PrintArg_struct _T1E;_T1E.tag=0;
# 1048
_T1E.f1=Cyc_Absynpp_cnst2string(x);_T16=_T1E;}{struct Cyc_String_pa_PrintArg_struct _T1E=_T16;void*_T1F[1];_T17=_T1F + 0;*_T17=& _T1E;_T18=loc;_T19=_tag_fat("expected integer constant but found %s",sizeof(char),39U);_T1A=_tag_fat(_T1F,sizeof(void*),1);Cyc_Warn_err(_T18,_T19,_T1A);}
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
static void*Cyc_Parse_str2type(unsigned loc,struct _fat_ptr s){struct _fat_ptr _T0;struct _fat_ptr _T1;int _T2;void*_T3;void*_T4;struct _fat_ptr _T5;struct _fat_ptr _T6;int _T7;void*_T8;void*_T9;void*_TA;int(*_TB)(unsigned,struct _fat_ptr);unsigned _TC;struct _fat_ptr _TD;struct Cyc_String_pa_PrintArg_struct _TE;void**_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;_T0=s;_T1=
_tag_fat("@fat",sizeof(char),5U);_T2=Cyc_strcmp(_T0,_T1);if(_T2)goto _TL1A5;else{goto _TL1A7;}
_TL1A7: _T3=Cyc_Tcutil_ptrbnd_cvar_equivalent(Cyc_Absyn_fat_bound_type);_T4=_check_null(_T3);return _T4;
# 1137
_TL1A5: _T5=s;_T6=_tag_fat("@thin @numelts{valueof_t(1U)}",sizeof(char),30U);_T7=Cyc_strcmp(_T5,_T6);if(_T7)goto _TL1A8;else{goto _TL1AA;}
_TL1AA: _T8=Cyc_Absyn_bounds_one();_T9=Cyc_Tcutil_ptrbnd_cvar_equivalent(_T8);_TA=_check_null(_T9);return _TA;_TL1A8:{
# 1140
int(*_T12)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_TB=_T12;}_TC=loc;{struct Cyc_String_pa_PrintArg_struct _T12;_T12.tag=0;_T12.f1=s;_TE=_T12;}{struct Cyc_String_pa_PrintArg_struct _T12=_TE;void*_T13[1];_TF=_T13 + 0;*_TF=& _T12;_T10=_tag_fat("Unknown type constant:: %s",sizeof(char),27U);_T11=_tag_fat(_T13,sizeof(void*),1);_TD=Cyc_aprintf(_T10,_T11);}_TB(_TC,_TD);}
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
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){union Cyc_YYSTYPE _T0;struct _fat_ptr _T1;struct _RegionHandle*_T2;void*_T3;struct Cyc_Yystacktype*_T4;struct Cyc_Yystacktype*_T5;struct _RegionHandle*_T6;unsigned _T7;struct Cyc_Yystacktype*_T8;unsigned _T9;struct Cyc_Yystacktype*_TA;unsigned _TB;struct _fat_ptr _TC;int _TD;char*_TE;short*_TF;int _T10;int _T11;int _T12;int _T13;struct _fat_ptr _T14;int _T15;int _T16;struct Cyc_Yystack_overflow_exn_struct*_T17;struct Cyc_Yystack_overflow_exn_struct*_T18;struct _fat_ptr _T19;int _T1A;short*_T1B;struct _RegionHandle*_T1C;unsigned _T1D;int _T1E;unsigned _T1F;int _T20;unsigned _T21;short*_T22;unsigned _T23;struct _fat_ptr _T24;unsigned char*_T25;short*_T26;unsigned _T27;int _T28;short*_T29;unsigned _T2A;struct _fat_ptr _T2B;int _T2C;struct Cyc_Yystacktype*_T2D;struct _RegionHandle*_T2E;unsigned _T2F;int _T30;unsigned _T31;int _T32;unsigned _T33;struct Cyc_Yystacktype*_T34;unsigned _T35;struct _fat_ptr _T36;unsigned _T37;int _T38;char*_T39;struct Cyc_Yystacktype*_T3A;struct Cyc_Yystacktype*_T3B;unsigned _T3C;struct _fat_ptr _T3D;char*_T3E;struct Cyc_Yystacktype*_T3F;short*_T40;int _T41;char*_T42;short*_T43;short _T44;struct Cyc_Lexing_lexbuf*_T45;union Cyc_YYSTYPE*_T46;union Cyc_YYSTYPE*_T47;struct Cyc_Yyltype*_T48;struct Cyc_Yyltype*_T49;int _T4A;short*_T4B;int _T4C;short _T4D;int _T4E;short*_T4F;int _T50;short _T51;int _T52;int _T53;short*_T54;int _T55;short _T56;struct _fat_ptr _T57;int _T58;char*_T59;struct Cyc_Yystacktype*_T5A;struct Cyc_Yystacktype _T5B;short*_T5C;int _T5D;char*_T5E;short*_T5F;short _T60;short*_T61;int _T62;char*_T63;short*_T64;short _T65;struct _fat_ptr _T66;int _T67;int _T68;int _T69;struct _fat_ptr _T6A;char*_T6B;char*_T6C;struct Cyc_Yystacktype*_T6D;struct Cyc_Yystacktype _T6E;int _T6F;int _T70;struct Cyc_Yystacktype*_T71;union Cyc_YYSTYPE*_T72;union Cyc_YYSTYPE*_T73;struct Cyc_Yystacktype*_T74;struct Cyc_Yystacktype _T75;struct Cyc_Yystacktype*_T76;union Cyc_YYSTYPE*_T77;union Cyc_YYSTYPE*_T78;struct Cyc_Yystacktype*_T79;union Cyc_YYSTYPE*_T7A;union Cyc_YYSTYPE*_T7B;struct Cyc_List_List*_T7C;struct Cyc_Yystacktype*_T7D;union Cyc_YYSTYPE*_T7E;union Cyc_YYSTYPE*_T7F;struct Cyc_List_List*_T80;struct Cyc_List_List*_T81;struct Cyc_List_List*_T82;struct Cyc_Absyn_Decl*_T83;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T84;struct Cyc_Yystacktype*_T85;union Cyc_YYSTYPE*_T86;union Cyc_YYSTYPE*_T87;struct Cyc_Yystacktype*_T88;union Cyc_YYSTYPE*_T89;union Cyc_YYSTYPE*_T8A;struct Cyc_Yystacktype*_T8B;struct Cyc_Yystacktype _T8C;struct Cyc_Yyltype _T8D;unsigned _T8E;struct Cyc_List_List*_T8F;struct Cyc_Absyn_Decl*_T90;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T91;struct Cyc_Yystacktype*_T92;union Cyc_YYSTYPE*_T93;union Cyc_YYSTYPE*_T94;struct Cyc_Yystacktype*_T95;union Cyc_YYSTYPE*_T96;union Cyc_YYSTYPE*_T97;struct Cyc_Yystacktype*_T98;struct Cyc_Yystacktype _T99;struct Cyc_Yyltype _T9A;unsigned _T9B;struct Cyc_Yystacktype*_T9C;union Cyc_YYSTYPE*_T9D;union Cyc_YYSTYPE*_T9E;struct Cyc_List_List*_T9F;struct Cyc_Absyn_Decl*_TA0;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_TA1;struct _fat_ptr*_TA2;struct Cyc_Yystacktype*_TA3;union Cyc_YYSTYPE*_TA4;union Cyc_YYSTYPE*_TA5;struct Cyc_Yystacktype*_TA6;union Cyc_YYSTYPE*_TA7;union Cyc_YYSTYPE*_TA8;struct Cyc_Yystacktype*_TA9;struct Cyc_Yystacktype _TAA;struct Cyc_Yyltype _TAB;unsigned _TAC;struct Cyc_List_List*_TAD;struct Cyc_Absyn_Decl*_TAE;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_TAF;struct _fat_ptr*_TB0;struct Cyc_Yystacktype*_TB1;union Cyc_YYSTYPE*_TB2;union Cyc_YYSTYPE*_TB3;struct Cyc_Yystacktype*_TB4;union Cyc_YYSTYPE*_TB5;union Cyc_YYSTYPE*_TB6;struct Cyc_Yystacktype*_TB7;struct Cyc_Yystacktype _TB8;struct Cyc_Yyltype _TB9;unsigned _TBA;struct Cyc_Yystacktype*_TBB;union Cyc_YYSTYPE*_TBC;union Cyc_YYSTYPE*_TBD;struct Cyc_Yystacktype*_TBE;union Cyc_YYSTYPE*_TBF;union Cyc_YYSTYPE*_TC0;struct Cyc_List_List*_TC1;struct Cyc_Yystacktype*_TC2;union Cyc_YYSTYPE*_TC3;union Cyc_YYSTYPE*_TC4;struct Cyc_List_List*_TC5;struct Cyc_List_List*_TC6;struct Cyc_Yystacktype*_TC7;union Cyc_YYSTYPE*_TC8;union Cyc_YYSTYPE*_TC9;struct Cyc_Yystacktype*_TCA;union Cyc_YYSTYPE*_TCB;union Cyc_YYSTYPE*_TCC;struct Cyc_Yystacktype*_TCD;union Cyc_YYSTYPE*_TCE;union Cyc_YYSTYPE*_TCF;struct Cyc_Yystacktype*_TD0;union Cyc_YYSTYPE*_TD1;union Cyc_YYSTYPE*_TD2;struct Cyc_Yystacktype*_TD3;struct Cyc_Yystacktype _TD4;struct Cyc_Yyltype _TD5;unsigned _TD6;unsigned _TD7;struct _fat_ptr _TD8;struct _fat_ptr _TD9;struct Cyc_List_List*_TDA;unsigned _TDB;unsigned _TDC;int _TDD;struct Cyc_Yystacktype*_TDE;struct Cyc_Yystacktype _TDF;struct Cyc_Yyltype _TE0;unsigned _TE1;int _TE2;struct Cyc_Yystacktype*_TE3;struct Cyc_Yystacktype _TE4;struct Cyc_Yyltype _TE5;unsigned _TE6;unsigned _TE7;struct _fat_ptr _TE8;struct _fat_ptr _TE9;struct Cyc_List_List*_TEA;struct Cyc_Absyn_Decl*_TEB;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_TEC;struct Cyc_Yystacktype*_TED;union Cyc_YYSTYPE*_TEE;union Cyc_YYSTYPE*_TEF;struct _tuple10*_TF0;struct Cyc_Yystacktype*_TF1;struct Cyc_Yystacktype _TF2;struct Cyc_Yyltype _TF3;unsigned _TF4;struct Cyc_Yystacktype*_TF5;union Cyc_YYSTYPE*_TF6;union Cyc_YYSTYPE*_TF7;struct Cyc_List_List*_TF8;struct Cyc_Absyn_Decl*_TF9;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_TFA;struct Cyc_Yystacktype*_TFB;union Cyc_YYSTYPE*_TFC;union Cyc_YYSTYPE*_TFD;struct Cyc_Yystacktype*_TFE;struct Cyc_Yystacktype _TFF;struct Cyc_Yyltype _T100;unsigned _T101;struct Cyc_Yystacktype*_T102;union Cyc_YYSTYPE*_T103;union Cyc_YYSTYPE*_T104;struct Cyc_List_List*_T105;struct Cyc_Absyn_Decl*_T106;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T107;struct Cyc_Yystacktype*_T108;union Cyc_YYSTYPE*_T109;union Cyc_YYSTYPE*_T10A;struct _tuple10*_T10B;struct Cyc_Yystacktype*_T10C;struct Cyc_Yystacktype _T10D;struct Cyc_Yyltype _T10E;unsigned _T10F;struct Cyc_Yystacktype*_T110;union Cyc_YYSTYPE*_T111;union Cyc_YYSTYPE*_T112;struct Cyc_List_List*_T113;struct Cyc_Absyn_Decl*_T114;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T115;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T116;struct Cyc_Yystacktype*_T117;struct Cyc_Yystacktype _T118;struct Cyc_Yyltype _T119;unsigned _T11A;struct Cyc_Yystacktype*_T11B;union Cyc_YYSTYPE*_T11C;union Cyc_YYSTYPE*_T11D;struct Cyc_List_List*_T11E;struct Cyc_Absyn_Decl*_T11F;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T120;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T121;struct Cyc_Yystacktype*_T122;struct Cyc_Yystacktype _T123;struct Cyc_Yyltype _T124;unsigned _T125;struct Cyc_Yystacktype*_T126;union Cyc_YYSTYPE*_T127;union Cyc_YYSTYPE*_T128;struct Cyc_List_List*_T129;struct Cyc_Absyn_Decl*_T12A;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T12B;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T12C;struct Cyc_Yystacktype*_T12D;struct Cyc_Yystacktype _T12E;struct Cyc_Yyltype _T12F;unsigned _T130;struct Cyc_Yystacktype*_T131;union Cyc_YYSTYPE*_T132;union Cyc_YYSTYPE*_T133;struct Cyc_List_List*_T134;struct Cyc_Absyn_Decl*_T135;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T136;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T137;struct Cyc_Yystacktype*_T138;struct Cyc_Yystacktype _T139;struct Cyc_Yyltype _T13A;unsigned _T13B;struct Cyc_Yystacktype*_T13C;union Cyc_YYSTYPE*_T13D;union Cyc_YYSTYPE*_T13E;struct Cyc_Yystacktype*_T13F;union Cyc_YYSTYPE*_T140;union Cyc_YYSTYPE*_T141;struct _fat_ptr _T142;struct _fat_ptr _T143;int _T144;struct Cyc_Yystacktype*_T145;union Cyc_YYSTYPE*_T146;union Cyc_YYSTYPE*_T147;struct _fat_ptr _T148;struct _fat_ptr _T149;int _T14A;struct Cyc_Yystacktype*_T14B;struct Cyc_Yystacktype _T14C;struct Cyc_Yyltype _T14D;unsigned _T14E;unsigned _T14F;struct _fat_ptr _T150;struct _fat_ptr _T151;struct Cyc_Yystacktype*_T152;struct Cyc_Yystacktype _T153;struct Cyc_List_List*_T154;struct Cyc_Yystacktype*_T155;union Cyc_YYSTYPE*_T156;union Cyc_YYSTYPE*_T157;struct Cyc_List_List*_T158;struct Cyc_Yystacktype*_T159;union Cyc_YYSTYPE*_T15A;union Cyc_YYSTYPE*_T15B;struct Cyc_List_List*_T15C;struct Cyc_Yystacktype*_T15D;union Cyc_YYSTYPE*_T15E;union Cyc_YYSTYPE*_T15F;struct Cyc_Yystacktype*_T160;union Cyc_YYSTYPE*_T161;union Cyc_YYSTYPE*_T162;struct _tuple30*_T163;struct Cyc_Yystacktype*_T164;struct Cyc_Yystacktype _T165;struct _tuple30*_T166;struct Cyc_Yystacktype*_T167;union Cyc_YYSTYPE*_T168;union Cyc_YYSTYPE*_T169;struct _tuple30*_T16A;struct _tuple30*_T16B;struct Cyc_Yystacktype*_T16C;struct Cyc_Yystacktype _T16D;struct Cyc_Yyltype _T16E;unsigned _T16F;struct Cyc_List_List*_T170;struct _tuple33*_T171;struct Cyc_Yystacktype*_T172;struct Cyc_Yystacktype _T173;struct Cyc_Yyltype _T174;unsigned _T175;struct Cyc_Yystacktype*_T176;union Cyc_YYSTYPE*_T177;union Cyc_YYSTYPE*_T178;struct Cyc_List_List*_T179;struct _tuple33*_T17A;struct Cyc_Yystacktype*_T17B;struct Cyc_Yystacktype _T17C;struct Cyc_Yyltype _T17D;unsigned _T17E;struct Cyc_Yystacktype*_T17F;union Cyc_YYSTYPE*_T180;union Cyc_YYSTYPE*_T181;struct Cyc_Yystacktype*_T182;union Cyc_YYSTYPE*_T183;union Cyc_YYSTYPE*_T184;struct Cyc_Yystacktype*_T185;struct Cyc_Yystacktype _T186;struct Cyc_List_List*_T187;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T188;struct Cyc_Yystacktype*_T189;union Cyc_YYSTYPE*_T18A;union Cyc_YYSTYPE*_T18B;void*_T18C;struct Cyc_Yystacktype*_T18D;struct Cyc_Yystacktype _T18E;struct Cyc_Yyltype _T18F;unsigned _T190;unsigned _T191;struct Cyc_Yystacktype*_T192;struct Cyc_Yystacktype _T193;struct _RegionHandle*_T194;struct Cyc_Yystacktype*_T195;union Cyc_YYSTYPE*_T196;union Cyc_YYSTYPE*_T197;struct Cyc_Parse_Declarator _T198;struct Cyc_Yystacktype*_T199;union Cyc_YYSTYPE*_T19A;union Cyc_YYSTYPE*_T19B;struct Cyc_Absyn_Stmt*_T19C;struct Cyc_Yystacktype*_T19D;struct Cyc_Yystacktype _T19E;struct Cyc_Yyltype _T19F;unsigned _T1A0;unsigned _T1A1;struct Cyc_Absyn_Fndecl*_T1A2;struct Cyc_Yystacktype*_T1A3;union Cyc_YYSTYPE*_T1A4;union Cyc_YYSTYPE*_T1A5;struct _RegionHandle*_T1A6;struct Cyc_Parse_Declaration_spec*_T1A7;struct Cyc_Parse_Declaration_spec*_T1A8;struct Cyc_Yystacktype*_T1A9;union Cyc_YYSTYPE*_T1AA;union Cyc_YYSTYPE*_T1AB;struct Cyc_Parse_Declarator _T1AC;struct Cyc_Yystacktype*_T1AD;union Cyc_YYSTYPE*_T1AE;union Cyc_YYSTYPE*_T1AF;struct Cyc_Absyn_Stmt*_T1B0;struct Cyc_Yystacktype*_T1B1;struct Cyc_Yystacktype _T1B2;struct Cyc_Yyltype _T1B3;unsigned _T1B4;unsigned _T1B5;struct Cyc_Absyn_Fndecl*_T1B6;struct _RegionHandle*_T1B7;struct Cyc_Yystacktype*_T1B8;union Cyc_YYSTYPE*_T1B9;union Cyc_YYSTYPE*_T1BA;struct Cyc_Parse_Declarator _T1BB;struct Cyc_Yystacktype*_T1BC;union Cyc_YYSTYPE*_T1BD;union Cyc_YYSTYPE*_T1BE;struct Cyc_List_List*_T1BF;struct Cyc_Yystacktype*_T1C0;union Cyc_YYSTYPE*_T1C1;union Cyc_YYSTYPE*_T1C2;struct Cyc_Absyn_Stmt*_T1C3;struct Cyc_Yystacktype*_T1C4;struct Cyc_Yystacktype _T1C5;struct Cyc_Yyltype _T1C6;unsigned _T1C7;unsigned _T1C8;struct Cyc_Absyn_Fndecl*_T1C9;struct Cyc_Yystacktype*_T1CA;union Cyc_YYSTYPE*_T1CB;union Cyc_YYSTYPE*_T1CC;struct _RegionHandle*_T1CD;struct Cyc_Parse_Declaration_spec*_T1CE;struct Cyc_Parse_Declaration_spec*_T1CF;struct Cyc_Yystacktype*_T1D0;union Cyc_YYSTYPE*_T1D1;union Cyc_YYSTYPE*_T1D2;struct Cyc_Parse_Declarator _T1D3;struct Cyc_Yystacktype*_T1D4;union Cyc_YYSTYPE*_T1D5;union Cyc_YYSTYPE*_T1D6;struct Cyc_List_List*_T1D7;struct Cyc_Yystacktype*_T1D8;union Cyc_YYSTYPE*_T1D9;union Cyc_YYSTYPE*_T1DA;struct Cyc_Absyn_Stmt*_T1DB;struct Cyc_Yystacktype*_T1DC;struct Cyc_Yystacktype _T1DD;struct Cyc_Yyltype _T1DE;unsigned _T1DF;unsigned _T1E0;struct Cyc_Absyn_Fndecl*_T1E1;struct Cyc_Yystacktype*_T1E2;union Cyc_YYSTYPE*_T1E3;union Cyc_YYSTYPE*_T1E4;struct _RegionHandle*_T1E5;struct Cyc_Parse_Declaration_spec*_T1E6;struct Cyc_Parse_Declaration_spec*_T1E7;struct Cyc_Yystacktype*_T1E8;union Cyc_YYSTYPE*_T1E9;union Cyc_YYSTYPE*_T1EA;struct Cyc_Parse_Declarator _T1EB;struct Cyc_Yystacktype*_T1EC;union Cyc_YYSTYPE*_T1ED;union Cyc_YYSTYPE*_T1EE;struct Cyc_Absyn_Stmt*_T1EF;struct Cyc_Yystacktype*_T1F0;struct Cyc_Yystacktype _T1F1;struct Cyc_Yyltype _T1F2;unsigned _T1F3;unsigned _T1F4;struct Cyc_Absyn_Fndecl*_T1F5;struct Cyc_Yystacktype*_T1F6;union Cyc_YYSTYPE*_T1F7;union Cyc_YYSTYPE*_T1F8;struct _RegionHandle*_T1F9;struct Cyc_Parse_Declaration_spec*_T1FA;struct Cyc_Parse_Declaration_spec*_T1FB;struct Cyc_Yystacktype*_T1FC;union Cyc_YYSTYPE*_T1FD;union Cyc_YYSTYPE*_T1FE;struct Cyc_Parse_Declarator _T1FF;struct Cyc_Yystacktype*_T200;union Cyc_YYSTYPE*_T201;union Cyc_YYSTYPE*_T202;struct Cyc_List_List*_T203;struct Cyc_Yystacktype*_T204;union Cyc_YYSTYPE*_T205;union Cyc_YYSTYPE*_T206;struct Cyc_Absyn_Stmt*_T207;struct Cyc_Yystacktype*_T208;struct Cyc_Yystacktype _T209;struct Cyc_Yyltype _T20A;unsigned _T20B;unsigned _T20C;struct Cyc_Absyn_Fndecl*_T20D;struct Cyc_Yystacktype*_T20E;union Cyc_YYSTYPE*_T20F;union Cyc_YYSTYPE*_T210;struct _tuple0*_T211;struct Cyc_Yystacktype*_T212;struct Cyc_Yystacktype _T213;struct _fat_ptr*_T214;struct Cyc_Yystacktype*_T215;union Cyc_YYSTYPE*_T216;union Cyc_YYSTYPE*_T217;struct Cyc_Yystacktype*_T218;struct Cyc_Yystacktype _T219;struct Cyc_Yystacktype*_T21A;union Cyc_YYSTYPE*_T21B;union Cyc_YYSTYPE*_T21C;struct Cyc_Parse_Declaration_spec _T21D;struct Cyc_Yystacktype*_T21E;struct Cyc_Yystacktype _T21F;struct Cyc_Yyltype _T220;unsigned _T221;unsigned _T222;struct Cyc_Yystacktype*_T223;struct Cyc_Yystacktype _T224;struct Cyc_Yyltype _T225;unsigned _T226;unsigned _T227;struct Cyc_List_List*_T228;struct Cyc_Yystacktype*_T229;union Cyc_YYSTYPE*_T22A;union Cyc_YYSTYPE*_T22B;struct Cyc_Parse_Declaration_spec _T22C;struct _tuple11*(*_T22D)(struct _tuple11*);struct Cyc_Yystacktype*_T22E;union Cyc_YYSTYPE*_T22F;union Cyc_YYSTYPE*_T230;struct _tuple11*_T231;struct _tuple11*_T232;struct Cyc_Yystacktype*_T233;struct Cyc_Yystacktype _T234;struct Cyc_Yyltype _T235;unsigned _T236;unsigned _T237;struct Cyc_Yystacktype*_T238;struct Cyc_Yystacktype _T239;struct Cyc_Yyltype _T23A;unsigned _T23B;unsigned _T23C;struct Cyc_List_List*_T23D;struct Cyc_List_List*_T23E;struct Cyc_Yystacktype*_T23F;union Cyc_YYSTYPE*_T240;union Cyc_YYSTYPE*_T241;struct Cyc_Absyn_Pat*_T242;struct Cyc_Yystacktype*_T243;union Cyc_YYSTYPE*_T244;union Cyc_YYSTYPE*_T245;struct Cyc_Absyn_Exp*_T246;struct Cyc_Yystacktype*_T247;struct Cyc_Yystacktype _T248;struct Cyc_Yyltype _T249;unsigned _T24A;unsigned _T24B;struct Cyc_Yystacktype*_T24C;union Cyc_YYSTYPE*_T24D;union Cyc_YYSTYPE*_T24E;struct _tuple0*_T24F;struct _tuple0*_T250;struct Cyc_List_List*_T251;void*_T252;struct Cyc_List_List*_T253;struct _tuple0*_T254;void*_T255;struct Cyc_List_List*_T256;struct Cyc_List_List*_T257;struct Cyc_List_List*_T258;struct Cyc_Yystacktype*_T259;struct Cyc_Yystacktype _T25A;struct Cyc_Yyltype _T25B;unsigned _T25C;unsigned _T25D;struct Cyc_Yystacktype*_T25E;union Cyc_YYSTYPE*_T25F;union Cyc_YYSTYPE*_T260;struct _fat_ptr _T261;struct Cyc_Yystacktype*_T262;struct Cyc_Yystacktype _T263;struct Cyc_Yyltype _T264;unsigned _T265;unsigned _T266;struct Cyc_Absyn_Tvar*_T267;struct _fat_ptr*_T268;struct Cyc_Yystacktype*_T269;union Cyc_YYSTYPE*_T26A;union Cyc_YYSTYPE*_T26B;struct Cyc_Absyn_Tvar*_T26C;struct Cyc_Absyn_Tvar*_T26D;struct Cyc_Absyn_Kind*_T26E;struct Cyc_Absyn_Kind*_T26F;struct Cyc_Absyn_Tvar*_T270;struct Cyc_Yystacktype*_T271;struct Cyc_Yystacktype _T272;struct Cyc_Yyltype _T273;unsigned _T274;unsigned _T275;struct _tuple0*_T276;struct _fat_ptr*_T277;struct Cyc_Yystacktype*_T278;union Cyc_YYSTYPE*_T279;union Cyc_YYSTYPE*_T27A;void*_T27B;struct Cyc_List_List*_T27C;struct Cyc_Absyn_Tvar*_T27D;struct Cyc_Absyn_Vardecl*_T27E;struct Cyc_Yystacktype*_T27F;struct Cyc_Yystacktype _T280;struct Cyc_Yyltype _T281;unsigned _T282;unsigned _T283;struct _fat_ptr _T284;struct Cyc_String_pa_PrintArg_struct _T285;struct Cyc_Yystacktype*_T286;union Cyc_YYSTYPE*_T287;union Cyc_YYSTYPE*_T288;void**_T289;struct _fat_ptr _T28A;struct _fat_ptr _T28B;struct Cyc_Yystacktype*_T28C;union Cyc_YYSTYPE*_T28D;union Cyc_YYSTYPE*_T28E;struct _fat_ptr _T28F;struct Cyc_Yystacktype*_T290;struct Cyc_Yystacktype _T291;struct Cyc_Yyltype _T292;unsigned _T293;unsigned _T294;struct Cyc_Absyn_Tvar*_T295;struct _fat_ptr*_T296;struct Cyc_Absyn_Tvar*_T297;struct Cyc_Absyn_Tvar*_T298;struct Cyc_Absyn_Kind*_T299;struct Cyc_Absyn_Kind*_T29A;struct Cyc_Absyn_Tvar*_T29B;struct Cyc_Yystacktype*_T29C;struct Cyc_Yystacktype _T29D;struct Cyc_Yyltype _T29E;unsigned _T29F;unsigned _T2A0;struct _tuple0*_T2A1;struct _fat_ptr*_T2A2;struct Cyc_Yystacktype*_T2A3;union Cyc_YYSTYPE*_T2A4;union Cyc_YYSTYPE*_T2A5;void*_T2A6;struct Cyc_List_List*_T2A7;struct Cyc_Absyn_Tvar*_T2A8;struct Cyc_Absyn_Vardecl*_T2A9;struct Cyc_Yystacktype*_T2AA;union Cyc_YYSTYPE*_T2AB;union Cyc_YYSTYPE*_T2AC;struct Cyc_Absyn_Exp*_T2AD;struct Cyc_Yystacktype*_T2AE;struct Cyc_Yystacktype _T2AF;struct Cyc_Yyltype _T2B0;unsigned _T2B1;unsigned _T2B2;struct Cyc_Yystacktype*_T2B3;union Cyc_YYSTYPE*_T2B4;union Cyc_YYSTYPE*_T2B5;struct _fat_ptr _T2B6;struct _fat_ptr _T2B7;int _T2B8;struct Cyc_Yystacktype*_T2B9;struct Cyc_Yystacktype _T2BA;struct Cyc_Yyltype _T2BB;unsigned _T2BC;unsigned _T2BD;struct _fat_ptr _T2BE;struct _fat_ptr _T2BF;struct Cyc_Yystacktype*_T2C0;union Cyc_YYSTYPE*_T2C1;union Cyc_YYSTYPE*_T2C2;struct Cyc_Absyn_Exp*_T2C3;struct Cyc_Yystacktype*_T2C4;struct Cyc_Yystacktype _T2C5;struct Cyc_Yystacktype*_T2C6;union Cyc_YYSTYPE*_T2C7;union Cyc_YYSTYPE*_T2C8;struct Cyc_List_List*_T2C9;struct Cyc_Yystacktype*_T2CA;union Cyc_YYSTYPE*_T2CB;union Cyc_YYSTYPE*_T2CC;struct Cyc_List_List*_T2CD;struct Cyc_List_List*_T2CE;struct Cyc_Parse_Declaration_spec _T2CF;struct Cyc_Yystacktype*_T2D0;union Cyc_YYSTYPE*_T2D1;union Cyc_YYSTYPE*_T2D2;struct Cyc_Yystacktype*_T2D3;struct Cyc_Yystacktype _T2D4;struct Cyc_Yyltype _T2D5;unsigned _T2D6;unsigned _T2D7;struct Cyc_Yystacktype*_T2D8;union Cyc_YYSTYPE*_T2D9;union Cyc_YYSTYPE*_T2DA;struct Cyc_Parse_Declaration_spec _T2DB;enum Cyc_Parse_Storage_class _T2DC;int _T2DD;struct Cyc_Yystacktype*_T2DE;struct Cyc_Yystacktype _T2DF;struct Cyc_Yyltype _T2E0;unsigned _T2E1;unsigned _T2E2;struct _fat_ptr _T2E3;struct _fat_ptr _T2E4;struct Cyc_Parse_Declaration_spec _T2E5;struct Cyc_Yystacktype*_T2E6;union Cyc_YYSTYPE*_T2E7;union Cyc_YYSTYPE*_T2E8;struct Cyc_Parse_Declaration_spec _T2E9;struct Cyc_Parse_Declaration_spec _T2EA;struct Cyc_Parse_Declaration_spec _T2EB;struct Cyc_Parse_Declaration_spec _T2EC;struct Cyc_Yystacktype*_T2ED;struct Cyc_Yystacktype _T2EE;struct Cyc_Yyltype _T2EF;unsigned _T2F0;unsigned _T2F1;struct _fat_ptr _T2F2;struct _fat_ptr _T2F3;struct Cyc_Yystacktype*_T2F4;struct Cyc_Yystacktype _T2F5;struct Cyc_Parse_Declaration_spec _T2F6;struct Cyc_Yystacktype*_T2F7;struct Cyc_Yystacktype _T2F8;struct Cyc_Yyltype _T2F9;unsigned _T2FA;unsigned _T2FB;struct Cyc_Yystacktype*_T2FC;union Cyc_YYSTYPE*_T2FD;union Cyc_YYSTYPE*_T2FE;struct Cyc_Yystacktype*_T2FF;union Cyc_YYSTYPE*_T300;union Cyc_YYSTYPE*_T301;struct Cyc_Parse_Declaration_spec _T302;struct Cyc_Parse_Declaration_spec _T303;struct Cyc_Parse_Declaration_spec _T304;struct Cyc_Yystacktype*_T305;struct Cyc_Yystacktype _T306;struct Cyc_Yyltype _T307;unsigned _T308;unsigned _T309;struct Cyc_Parse_Declaration_spec _T30A;struct Cyc_Parse_Type_specifier _T30B;struct Cyc_Yystacktype*_T30C;union Cyc_YYSTYPE*_T30D;union Cyc_YYSTYPE*_T30E;struct Cyc_Parse_Type_specifier _T30F;struct Cyc_Parse_Declaration_spec _T310;struct Cyc_Parse_Declaration_spec _T311;struct Cyc_Parse_Declaration_spec _T312;struct Cyc_Yystacktype*_T313;union Cyc_YYSTYPE*_T314;union Cyc_YYSTYPE*_T315;struct Cyc_Yystacktype*_T316;union Cyc_YYSTYPE*_T317;union Cyc_YYSTYPE*_T318;struct Cyc_Parse_Declaration_spec _T319;struct Cyc_Parse_Declaration_spec _T31A;struct Cyc_Yystacktype*_T31B;union Cyc_YYSTYPE*_T31C;union Cyc_YYSTYPE*_T31D;struct Cyc_Absyn_Tqual _T31E;struct Cyc_Parse_Declaration_spec _T31F;struct Cyc_Absyn_Tqual _T320;struct Cyc_Parse_Declaration_spec _T321;struct Cyc_Parse_Declaration_spec _T322;struct Cyc_Parse_Declaration_spec _T323;struct Cyc_Parse_Declaration_spec _T324;struct Cyc_Yystacktype*_T325;struct Cyc_Yystacktype _T326;struct Cyc_Yyltype _T327;unsigned _T328;unsigned _T329;struct Cyc_Yystacktype*_T32A;union Cyc_YYSTYPE*_T32B;union Cyc_YYSTYPE*_T32C;struct Cyc_Parse_Declaration_spec _T32D;struct Cyc_Parse_Declaration_spec _T32E;struct Cyc_Parse_Declaration_spec _T32F;struct Cyc_Parse_Declaration_spec _T330;struct Cyc_Parse_Declaration_spec _T331;struct Cyc_Parse_Declaration_spec _T332;struct Cyc_Yystacktype*_T333;struct Cyc_Yystacktype _T334;struct Cyc_Yyltype _T335;unsigned _T336;unsigned _T337;struct Cyc_Yystacktype*_T338;union Cyc_YYSTYPE*_T339;union Cyc_YYSTYPE*_T33A;struct Cyc_Yystacktype*_T33B;union Cyc_YYSTYPE*_T33C;union Cyc_YYSTYPE*_T33D;struct Cyc_Parse_Declaration_spec _T33E;struct Cyc_Parse_Declaration_spec _T33F;struct Cyc_Parse_Declaration_spec _T340;struct Cyc_Parse_Declaration_spec _T341;struct Cyc_Parse_Declaration_spec _T342;struct Cyc_Yystacktype*_T343;union Cyc_YYSTYPE*_T344;union Cyc_YYSTYPE*_T345;struct Cyc_List_List*_T346;struct Cyc_Parse_Declaration_spec _T347;struct Cyc_List_List*_T348;struct Cyc_Yystacktype*_T349;union Cyc_YYSTYPE*_T34A;union Cyc_YYSTYPE*_T34B;struct _fat_ptr _T34C;struct _fat_ptr _T34D;int _T34E;struct Cyc_Yystacktype*_T34F;struct Cyc_Yystacktype _T350;struct Cyc_Yyltype _T351;unsigned _T352;unsigned _T353;struct _fat_ptr _T354;struct _fat_ptr _T355;struct Cyc_Yystacktype*_T356;struct Cyc_Yystacktype _T357;struct Cyc_Yystacktype*_T358;struct Cyc_Yystacktype _T359;struct Cyc_List_List*_T35A;struct Cyc_Yystacktype*_T35B;union Cyc_YYSTYPE*_T35C;union Cyc_YYSTYPE*_T35D;struct Cyc_List_List*_T35E;struct Cyc_Yystacktype*_T35F;union Cyc_YYSTYPE*_T360;union Cyc_YYSTYPE*_T361;struct Cyc_Yystacktype*_T362;union Cyc_YYSTYPE*_T363;union Cyc_YYSTYPE*_T364;struct Cyc_Yystacktype*_T365;struct Cyc_Yystacktype _T366;struct Cyc_Yyltype _T367;unsigned _T368;unsigned _T369;struct Cyc_Yystacktype*_T36A;union Cyc_YYSTYPE*_T36B;union Cyc_YYSTYPE*_T36C;struct _fat_ptr _T36D;void*_T36E;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T36F;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T370;void*_T371;struct Cyc_Yystacktype*_T372;struct Cyc_Yystacktype _T373;struct Cyc_Yyltype _T374;unsigned _T375;unsigned _T376;struct Cyc_Yystacktype*_T377;union Cyc_YYSTYPE*_T378;union Cyc_YYSTYPE*_T379;struct _fat_ptr _T37A;struct Cyc_Yystacktype*_T37B;struct Cyc_Yystacktype _T37C;struct Cyc_Yyltype _T37D;unsigned _T37E;unsigned _T37F;struct Cyc_Yystacktype*_T380;union Cyc_YYSTYPE*_T381;union Cyc_YYSTYPE*_T382;struct Cyc_Absyn_Exp*_T383;void*_T384;struct Cyc_Yystacktype*_T385;struct Cyc_Yystacktype _T386;struct Cyc_Yyltype _T387;unsigned _T388;unsigned _T389;struct Cyc_Yystacktype*_T38A;struct Cyc_Yystacktype _T38B;struct Cyc_Yyltype _T38C;unsigned _T38D;unsigned _T38E;struct Cyc_Yystacktype*_T38F;union Cyc_YYSTYPE*_T390;union Cyc_YYSTYPE*_T391;struct _fat_ptr _T392;struct Cyc_Yystacktype*_T393;union Cyc_YYSTYPE*_T394;union Cyc_YYSTYPE*_T395;struct _fat_ptr _T396;struct Cyc_Yystacktype*_T397;struct Cyc_Yystacktype _T398;struct Cyc_Yyltype _T399;unsigned _T39A;unsigned _T39B;struct Cyc_Yystacktype*_T39C;union Cyc_YYSTYPE*_T39D;union Cyc_YYSTYPE*_T39E;union Cyc_Absyn_Cnst _T39F;unsigned _T3A0;struct Cyc_Yystacktype*_T3A1;struct Cyc_Yystacktype _T3A2;struct Cyc_Yyltype _T3A3;unsigned _T3A4;unsigned _T3A5;struct Cyc_Yystacktype*_T3A6;union Cyc_YYSTYPE*_T3A7;union Cyc_YYSTYPE*_T3A8;union Cyc_Absyn_Cnst _T3A9;unsigned _T3AA;void*_T3AB;struct Cyc_Yystacktype*_T3AC;struct Cyc_Yystacktype _T3AD;struct Cyc_Yystacktype*_T3AE;union Cyc_YYSTYPE*_T3AF;union Cyc_YYSTYPE*_T3B0;struct _tuple0*_T3B1;struct Cyc_Yystacktype*_T3B2;union Cyc_YYSTYPE*_T3B3;union Cyc_YYSTYPE*_T3B4;struct Cyc_List_List*_T3B5;void*_T3B6;struct Cyc_Yystacktype*_T3B7;struct Cyc_Yystacktype _T3B8;struct Cyc_Yyltype _T3B9;unsigned _T3BA;unsigned _T3BB;struct Cyc_Parse_Type_specifier _T3BC;void*_T3BD;struct Cyc_Yystacktype*_T3BE;struct Cyc_Yystacktype _T3BF;struct Cyc_Yyltype _T3C0;unsigned _T3C1;unsigned _T3C2;struct Cyc_Parse_Type_specifier _T3C3;void*_T3C4;struct Cyc_Yystacktype*_T3C5;struct Cyc_Yystacktype _T3C6;struct Cyc_Yyltype _T3C7;unsigned _T3C8;unsigned _T3C9;struct Cyc_Parse_Type_specifier _T3CA;struct Cyc_Yystacktype*_T3CB;struct Cyc_Yystacktype _T3CC;struct Cyc_Yyltype _T3CD;unsigned _T3CE;unsigned _T3CF;struct Cyc_Parse_Type_specifier _T3D0;void*_T3D1;struct Cyc_Yystacktype*_T3D2;struct Cyc_Yystacktype _T3D3;struct Cyc_Yyltype _T3D4;unsigned _T3D5;unsigned _T3D6;struct Cyc_Parse_Type_specifier _T3D7;struct Cyc_Yystacktype*_T3D8;struct Cyc_Yystacktype _T3D9;struct Cyc_Yyltype _T3DA;unsigned _T3DB;unsigned _T3DC;struct Cyc_Parse_Type_specifier _T3DD;void*_T3DE;struct Cyc_Yystacktype*_T3DF;struct Cyc_Yystacktype _T3E0;struct Cyc_Yyltype _T3E1;unsigned _T3E2;unsigned _T3E3;struct Cyc_Parse_Type_specifier _T3E4;void*_T3E5;struct Cyc_Yystacktype*_T3E6;struct Cyc_Yystacktype _T3E7;struct Cyc_Yyltype _T3E8;unsigned _T3E9;unsigned _T3EA;struct Cyc_Parse_Type_specifier _T3EB;struct Cyc_Yystacktype*_T3EC;struct Cyc_Yystacktype _T3ED;struct Cyc_Yyltype _T3EE;unsigned _T3EF;unsigned _T3F0;struct Cyc_Parse_Type_specifier _T3F1;struct Cyc_Yystacktype*_T3F2;struct Cyc_Yystacktype _T3F3;struct Cyc_Yyltype _T3F4;unsigned _T3F5;unsigned _T3F6;struct Cyc_Parse_Type_specifier _T3F7;struct Cyc_Yystacktype*_T3F8;struct Cyc_Yystacktype _T3F9;struct Cyc_Yyltype _T3FA;unsigned _T3FB;unsigned _T3FC;struct Cyc_Parse_Type_specifier _T3FD;struct Cyc_Yystacktype*_T3FE;struct Cyc_Yystacktype _T3FF;struct Cyc_Yystacktype*_T400;struct Cyc_Yystacktype _T401;struct Cyc_Yystacktype*_T402;union Cyc_YYSTYPE*_T403;union Cyc_YYSTYPE*_T404;struct Cyc_Absyn_Exp*_T405;void*_T406;struct Cyc_Yystacktype*_T407;struct Cyc_Yystacktype _T408;struct Cyc_Yyltype _T409;unsigned _T40A;unsigned _T40B;struct Cyc_Parse_Type_specifier _T40C;struct _fat_ptr _T40D;struct Cyc_Absyn_Kind*_T40E;struct Cyc_Absyn_Kind*_T40F;void*_T410;struct Cyc_Yystacktype*_T411;struct Cyc_Yystacktype _T412;struct Cyc_Yyltype _T413;unsigned _T414;unsigned _T415;struct Cyc_Parse_Type_specifier _T416;struct Cyc_Yystacktype*_T417;struct Cyc_Yystacktype _T418;struct Cyc_Yystacktype*_T419;union Cyc_YYSTYPE*_T41A;union Cyc_YYSTYPE*_T41B;void*_T41C;struct Cyc_Yystacktype*_T41D;struct Cyc_Yystacktype _T41E;struct Cyc_Yyltype _T41F;unsigned _T420;unsigned _T421;struct Cyc_Parse_Type_specifier _T422;void*_T423;struct Cyc_Yystacktype*_T424;struct Cyc_Yystacktype _T425;struct Cyc_Yyltype _T426;unsigned _T427;unsigned _T428;struct Cyc_Parse_Type_specifier _T429;struct Cyc_Yystacktype*_T42A;union Cyc_YYSTYPE*_T42B;union Cyc_YYSTYPE*_T42C;struct Cyc_Absyn_Kind*_T42D;struct Cyc_Core_Opt*_T42E;void*_T42F;struct Cyc_Yystacktype*_T430;struct Cyc_Yystacktype _T431;struct Cyc_Yyltype _T432;unsigned _T433;unsigned _T434;struct Cyc_Parse_Type_specifier _T435;struct Cyc_List_List*(*_T436)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T437)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T438)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T439;struct Cyc_Yystacktype _T43A;struct Cyc_Yyltype _T43B;unsigned _T43C;unsigned _T43D;struct Cyc_Yystacktype*_T43E;union Cyc_YYSTYPE*_T43F;union Cyc_YYSTYPE*_T440;struct Cyc_List_List*_T441;struct Cyc_List_List*_T442;struct Cyc_List_List*_T443;void*_T444;struct Cyc_Yystacktype*_T445;struct Cyc_Yystacktype _T446;struct Cyc_Yyltype _T447;unsigned _T448;unsigned _T449;struct Cyc_Parse_Type_specifier _T44A;struct Cyc_Yystacktype*_T44B;union Cyc_YYSTYPE*_T44C;union Cyc_YYSTYPE*_T44D;void*_T44E;void*_T44F;struct Cyc_Yystacktype*_T450;struct Cyc_Yystacktype _T451;struct Cyc_Yyltype _T452;unsigned _T453;unsigned _T454;struct Cyc_Parse_Type_specifier _T455;struct Cyc_Core_Opt*_T456;struct Cyc_Core_Opt*_T457;void*_T458;void*_T459;struct Cyc_Yystacktype*_T45A;struct Cyc_Yystacktype _T45B;struct Cyc_Yyltype _T45C;unsigned _T45D;unsigned _T45E;struct Cyc_Parse_Type_specifier _T45F;struct Cyc_Yystacktype*_T460;union Cyc_YYSTYPE*_T461;union Cyc_YYSTYPE*_T462;void*_T463;void*_T464;struct Cyc_Yystacktype*_T465;struct Cyc_Yystacktype _T466;struct Cyc_Yyltype _T467;unsigned _T468;unsigned _T469;struct Cyc_Parse_Type_specifier _T46A;struct Cyc_Yystacktype*_T46B;union Cyc_YYSTYPE*_T46C;union Cyc_YYSTYPE*_T46D;void*_T46E;void*_T46F;struct Cyc_Yystacktype*_T470;struct Cyc_Yystacktype _T471;struct Cyc_Yyltype _T472;unsigned _T473;unsigned _T474;struct Cyc_Parse_Type_specifier _T475;struct Cyc_Core_Opt*_T476;struct Cyc_Core_Opt*_T477;void*_T478;void*_T479;struct Cyc_Yystacktype*_T47A;struct Cyc_Yystacktype _T47B;struct Cyc_Yyltype _T47C;unsigned _T47D;unsigned _T47E;struct Cyc_Parse_Type_specifier _T47F;struct Cyc_Yystacktype*_T480;union Cyc_YYSTYPE*_T481;union Cyc_YYSTYPE*_T482;struct Cyc_Absyn_Exp*_T483;void*_T484;struct Cyc_Yystacktype*_T485;struct Cyc_Yystacktype _T486;struct Cyc_Yyltype _T487;unsigned _T488;unsigned _T489;struct Cyc_Parse_Type_specifier _T48A;struct Cyc_Yystacktype*_T48B;union Cyc_YYSTYPE*_T48C;union Cyc_YYSTYPE*_T48D;struct _fat_ptr _T48E;struct Cyc_Yystacktype*_T48F;struct Cyc_Yystacktype _T490;struct Cyc_Yyltype _T491;unsigned _T492;unsigned _T493;struct Cyc_Absyn_Kind*_T494;unsigned _T495;int _T496;struct Cyc_Yystacktype*_T497;struct Cyc_Yystacktype _T498;struct Cyc_Yyltype _T499;unsigned _T49A;struct Cyc_Absyn_Tqual _T49B;struct Cyc_Absyn_Tqual _T49C;struct Cyc_Absyn_Tqual _T49D;struct Cyc_Absyn_TypeDecl*_T49E;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T49F;struct Cyc_Absyn_Enumdecl*_T4A0;struct Cyc_Yystacktype*_T4A1;union Cyc_YYSTYPE*_T4A2;union Cyc_YYSTYPE*_T4A3;struct Cyc_Core_Opt*_T4A4;struct Cyc_Yystacktype*_T4A5;union Cyc_YYSTYPE*_T4A6;union Cyc_YYSTYPE*_T4A7;struct Cyc_Absyn_TypeDecl*_T4A8;struct Cyc_Yystacktype*_T4A9;struct Cyc_Yystacktype _T4AA;struct Cyc_Yyltype _T4AB;unsigned _T4AC;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T4AD;void*_T4AE;struct Cyc_Yystacktype*_T4AF;struct Cyc_Yystacktype _T4B0;struct Cyc_Yyltype _T4B1;unsigned _T4B2;unsigned _T4B3;struct Cyc_Parse_Type_specifier _T4B4;struct Cyc_Yystacktype*_T4B5;union Cyc_YYSTYPE*_T4B6;union Cyc_YYSTYPE*_T4B7;struct _tuple0*_T4B8;void*_T4B9;struct Cyc_Yystacktype*_T4BA;struct Cyc_Yystacktype _T4BB;struct Cyc_Yyltype _T4BC;unsigned _T4BD;unsigned _T4BE;struct Cyc_Parse_Type_specifier _T4BF;struct Cyc_Yystacktype*_T4C0;union Cyc_YYSTYPE*_T4C1;union Cyc_YYSTYPE*_T4C2;struct Cyc_List_List*_T4C3;void*_T4C4;struct Cyc_Yystacktype*_T4C5;struct Cyc_Yystacktype _T4C6;struct Cyc_Yyltype _T4C7;unsigned _T4C8;unsigned _T4C9;struct Cyc_Parse_Type_specifier _T4CA;struct Cyc_Absyn_Enumfield*_T4CB;struct Cyc_Yystacktype*_T4CC;union Cyc_YYSTYPE*_T4CD;union Cyc_YYSTYPE*_T4CE;struct Cyc_Yystacktype*_T4CF;struct Cyc_Yystacktype _T4D0;struct Cyc_Yyltype _T4D1;unsigned _T4D2;struct Cyc_Absyn_Enumfield*_T4D3;struct Cyc_Yystacktype*_T4D4;union Cyc_YYSTYPE*_T4D5;union Cyc_YYSTYPE*_T4D6;struct Cyc_Yystacktype*_T4D7;union Cyc_YYSTYPE*_T4D8;union Cyc_YYSTYPE*_T4D9;struct Cyc_Yystacktype*_T4DA;struct Cyc_Yystacktype _T4DB;struct Cyc_Yyltype _T4DC;unsigned _T4DD;struct Cyc_List_List*_T4DE;struct Cyc_Yystacktype*_T4DF;union Cyc_YYSTYPE*_T4E0;union Cyc_YYSTYPE*_T4E1;struct Cyc_List_List*_T4E2;struct Cyc_Yystacktype*_T4E3;union Cyc_YYSTYPE*_T4E4;union Cyc_YYSTYPE*_T4E5;struct Cyc_List_List*_T4E6;struct Cyc_Yystacktype*_T4E7;union Cyc_YYSTYPE*_T4E8;union Cyc_YYSTYPE*_T4E9;struct Cyc_Yystacktype*_T4EA;union Cyc_YYSTYPE*_T4EB;union Cyc_YYSTYPE*_T4EC;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T4ED;struct Cyc_Yystacktype*_T4EE;union Cyc_YYSTYPE*_T4EF;union Cyc_YYSTYPE*_T4F0;struct Cyc_Yystacktype*_T4F1;union Cyc_YYSTYPE*_T4F2;union Cyc_YYSTYPE*_T4F3;void*_T4F4;struct Cyc_Yystacktype*_T4F5;struct Cyc_Yystacktype _T4F6;struct Cyc_Yyltype _T4F7;unsigned _T4F8;unsigned _T4F9;struct Cyc_Parse_Type_specifier _T4FA;struct Cyc_List_List*(*_T4FB)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T4FC)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T4FD;struct Cyc_Yystacktype _T4FE;struct Cyc_Yyltype _T4FF;unsigned _T500;unsigned _T501;struct Cyc_Yystacktype*_T502;union Cyc_YYSTYPE*_T503;union Cyc_YYSTYPE*_T504;struct Cyc_List_List*_T505;struct Cyc_List_List*(*_T506)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T507)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T508;struct Cyc_Yystacktype _T509;struct Cyc_Yyltype _T50A;unsigned _T50B;unsigned _T50C;struct Cyc_Yystacktype*_T50D;union Cyc_YYSTYPE*_T50E;union Cyc_YYSTYPE*_T50F;struct Cyc_List_List*_T510;struct Cyc_Yystacktype*_T511;union Cyc_YYSTYPE*_T512;union Cyc_YYSTYPE*_T513;struct _tuple28 _T514;struct _tuple28*_T515;unsigned _T516;struct _tuple28*_T517;struct _tuple28 _T518;struct Cyc_Yystacktype*_T519;union Cyc_YYSTYPE*_T51A;union Cyc_YYSTYPE*_T51B;struct _tuple25 _T51C;enum Cyc_Absyn_AggrKind _T51D;struct Cyc_Yystacktype*_T51E;union Cyc_YYSTYPE*_T51F;union Cyc_YYSTYPE*_T520;struct _tuple0*_T521;struct Cyc_List_List*_T522;struct Cyc_List_List*_T523;struct Cyc_List_List*_T524;struct Cyc_List_List*_T525;struct Cyc_Yystacktype*_T526;union Cyc_YYSTYPE*_T527;union Cyc_YYSTYPE*_T528;struct Cyc_List_List*_T529;struct Cyc_Yystacktype*_T52A;union Cyc_YYSTYPE*_T52B;union Cyc_YYSTYPE*_T52C;struct _tuple25 _T52D;int _T52E;struct Cyc_Absyn_AggrdeclImpl*_T52F;struct Cyc_Yystacktype*_T530;struct Cyc_Yystacktype _T531;struct Cyc_Yyltype _T532;unsigned _T533;unsigned _T534;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T535;void*_T536;struct Cyc_Yystacktype*_T537;struct Cyc_Yystacktype _T538;struct Cyc_Yyltype _T539;unsigned _T53A;unsigned _T53B;struct Cyc_Parse_Type_specifier _T53C;struct Cyc_Yystacktype*_T53D;union Cyc_YYSTYPE*_T53E;union Cyc_YYSTYPE*_T53F;struct _tuple25 _T540;enum Cyc_Absyn_AggrKind _T541;struct Cyc_Yystacktype*_T542;union Cyc_YYSTYPE*_T543;union Cyc_YYSTYPE*_T544;struct _tuple0*_T545;struct Cyc_Core_Opt*_T546;struct Cyc_Yystacktype*_T547;union Cyc_YYSTYPE*_T548;union Cyc_YYSTYPE*_T549;struct _tuple25 _T54A;int _T54B;struct Cyc_Core_Opt*_T54C;union Cyc_Absyn_AggrInfo _T54D;struct Cyc_Yystacktype*_T54E;union Cyc_YYSTYPE*_T54F;union Cyc_YYSTYPE*_T550;struct Cyc_List_List*_T551;void*_T552;struct Cyc_Yystacktype*_T553;struct Cyc_Yystacktype _T554;struct Cyc_Yyltype _T555;unsigned _T556;unsigned _T557;struct Cyc_Parse_Type_specifier _T558;struct _tuple25 _T559;struct Cyc_Yystacktype*_T55A;union Cyc_YYSTYPE*_T55B;union Cyc_YYSTYPE*_T55C;struct _tuple25 _T55D;struct Cyc_Yystacktype*_T55E;union Cyc_YYSTYPE*_T55F;union Cyc_YYSTYPE*_T560;struct Cyc_Yystacktype*_T561;union Cyc_YYSTYPE*_T562;union Cyc_YYSTYPE*_T563;struct Cyc_List_List*_T564;struct Cyc_List_List*_T565;struct Cyc_Yystacktype*_T566;union Cyc_YYSTYPE*_T567;union Cyc_YYSTYPE*_T568;struct Cyc_List_List*_T569;void*_T56A;struct Cyc_List_List*_T56B;struct Cyc_List_List*_T56C;struct Cyc_List_List*_T56D;void(*_T56E)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*);void(*_T56F)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T570;struct Cyc_List_List*_T571;struct Cyc_List_List*_T572;struct Cyc_Yystacktype*_T573;union Cyc_YYSTYPE*_T574;union Cyc_YYSTYPE*_T575;struct Cyc_List_List*_T576;struct Cyc_Yystacktype*_T577;union Cyc_YYSTYPE*_T578;union Cyc_YYSTYPE*_T579;struct Cyc_Yystacktype*_T57A;union Cyc_YYSTYPE*_T57B;union Cyc_YYSTYPE*_T57C;struct _tuple11*_T57D;struct _RegionHandle*_T57E;struct Cyc_Yystacktype*_T57F;union Cyc_YYSTYPE*_T580;union Cyc_YYSTYPE*_T581;struct _tuple11*_T582;struct _RegionHandle*_T583;struct Cyc_Yystacktype*_T584;union Cyc_YYSTYPE*_T585;union Cyc_YYSTYPE*_T586;struct Cyc_Yystacktype*_T587;union Cyc_YYSTYPE*_T588;union Cyc_YYSTYPE*_T589;struct _tuple12 _T58A;struct Cyc_Yystacktype*_T58B;union Cyc_YYSTYPE*_T58C;union Cyc_YYSTYPE*_T58D;struct _tuple12 _T58E;struct Cyc_Yystacktype*_T58F;union Cyc_YYSTYPE*_T590;union Cyc_YYSTYPE*_T591;struct Cyc_Yystacktype*_T592;union Cyc_YYSTYPE*_T593;union Cyc_YYSTYPE*_T594;void*_T595;struct Cyc_Yystacktype*_T596;struct Cyc_Yystacktype _T597;struct Cyc_Yyltype _T598;unsigned _T599;unsigned _T59A;struct _tuple12 _T59B;struct Cyc_Yystacktype*_T59C;union Cyc_YYSTYPE*_T59D;union Cyc_YYSTYPE*_T59E;struct Cyc_Yystacktype*_T59F;union Cyc_YYSTYPE*_T5A0;union Cyc_YYSTYPE*_T5A1;struct _tuple12 _T5A2;struct Cyc_Yystacktype*_T5A3;union Cyc_YYSTYPE*_T5A4;union Cyc_YYSTYPE*_T5A5;struct Cyc_Yystacktype*_T5A6;union Cyc_YYSTYPE*_T5A7;union Cyc_YYSTYPE*_T5A8;struct Cyc_Yystacktype*_T5A9;union Cyc_YYSTYPE*_T5AA;union Cyc_YYSTYPE*_T5AB;void*_T5AC;struct Cyc_Yystacktype*_T5AD;struct Cyc_Yystacktype _T5AE;struct Cyc_Yyltype _T5AF;unsigned _T5B0;unsigned _T5B1;struct Cyc_Yystacktype*_T5B2;union Cyc_YYSTYPE*_T5B3;union Cyc_YYSTYPE*_T5B4;struct Cyc_Absyn_Tqual _T5B5;unsigned _T5B6;struct Cyc_Yystacktype*_T5B7;struct Cyc_Yystacktype _T5B8;struct Cyc_Yyltype _T5B9;unsigned _T5BA;struct Cyc_Yystacktype*_T5BB;union Cyc_YYSTYPE*_T5BC;union Cyc_YYSTYPE*_T5BD;struct Cyc_List_List*_T5BE;void*_T5BF;struct _tuple13*_T5C0;struct _RegionHandle*_T5C1;struct Cyc_List_List*_T5C2;struct _RegionHandle*_T5C3;struct _tuple16*_T5C4;struct _RegionHandle*_T5C5;struct Cyc_List_List*_T5C6;struct Cyc_Parse_Type_specifier _T5C7;struct Cyc_Yystacktype*_T5C8;struct Cyc_Yystacktype _T5C9;struct Cyc_Yyltype _T5CA;unsigned _T5CB;unsigned _T5CC;struct _RegionHandle*_T5CD;struct _RegionHandle*_T5CE;struct Cyc_List_List*_T5CF;struct Cyc_List_List*_T5D0;struct Cyc_List_List*(*_T5D1)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T5D2)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T5D3;struct Cyc_Yystacktype _T5D4;struct Cyc_Yyltype _T5D5;unsigned _T5D6;unsigned _T5D7;struct Cyc_List_List*_T5D8;struct Cyc_List_List*_T5D9;struct _tuple26 _T5DA;struct Cyc_Yystacktype*_T5DB;struct Cyc_Yystacktype _T5DC;struct Cyc_Yyltype _T5DD;unsigned _T5DE;unsigned _T5DF;struct Cyc_Yystacktype*_T5E0;union Cyc_YYSTYPE*_T5E1;union Cyc_YYSTYPE*_T5E2;struct Cyc_Yystacktype*_T5E3;union Cyc_YYSTYPE*_T5E4;union Cyc_YYSTYPE*_T5E5;struct _tuple26 _T5E6;struct _tuple26 _T5E7;struct Cyc_Yystacktype*_T5E8;struct Cyc_Yystacktype _T5E9;struct Cyc_Yyltype _T5EA;unsigned _T5EB;unsigned _T5EC;struct Cyc_Yystacktype*_T5ED;union Cyc_YYSTYPE*_T5EE;union Cyc_YYSTYPE*_T5EF;struct Cyc_Parse_Type_specifier _T5F0;struct _tuple26 _T5F1;struct Cyc_Parse_Type_specifier _T5F2;struct _tuple26 _T5F3;struct _tuple26 _T5F4;struct Cyc_Yystacktype*_T5F5;union Cyc_YYSTYPE*_T5F6;union Cyc_YYSTYPE*_T5F7;struct Cyc_Yystacktype*_T5F8;union Cyc_YYSTYPE*_T5F9;union Cyc_YYSTYPE*_T5FA;struct _tuple26 _T5FB;struct Cyc_Yystacktype*_T5FC;union Cyc_YYSTYPE*_T5FD;union Cyc_YYSTYPE*_T5FE;struct Cyc_Absyn_Tqual _T5FF;struct _tuple26 _T600;struct Cyc_Absyn_Tqual _T601;struct _tuple26 _T602;struct _tuple26 _T603;struct _tuple26 _T604;struct Cyc_Yystacktype*_T605;struct Cyc_Yystacktype _T606;struct Cyc_Yyltype _T607;unsigned _T608;unsigned _T609;struct Cyc_Yystacktype*_T60A;union Cyc_YYSTYPE*_T60B;union Cyc_YYSTYPE*_T60C;struct Cyc_Yystacktype*_T60D;union Cyc_YYSTYPE*_T60E;union Cyc_YYSTYPE*_T60F;struct _tuple26 _T610;struct _tuple26 _T611;struct _tuple26 _T612;struct Cyc_Yystacktype*_T613;union Cyc_YYSTYPE*_T614;union Cyc_YYSTYPE*_T615;struct Cyc_List_List*_T616;struct _tuple26 _T617;struct Cyc_List_List*_T618;struct _tuple26 _T619;struct Cyc_Yystacktype*_T61A;struct Cyc_Yystacktype _T61B;struct Cyc_Yyltype _T61C;unsigned _T61D;unsigned _T61E;struct Cyc_Yystacktype*_T61F;union Cyc_YYSTYPE*_T620;union Cyc_YYSTYPE*_T621;struct Cyc_Yystacktype*_T622;union Cyc_YYSTYPE*_T623;union Cyc_YYSTYPE*_T624;struct _tuple26 _T625;struct _tuple26 _T626;struct Cyc_Yystacktype*_T627;struct Cyc_Yystacktype _T628;struct Cyc_Yyltype _T629;unsigned _T62A;unsigned _T62B;struct Cyc_Yystacktype*_T62C;union Cyc_YYSTYPE*_T62D;union Cyc_YYSTYPE*_T62E;struct Cyc_Parse_Type_specifier _T62F;struct _tuple26 _T630;struct Cyc_Parse_Type_specifier _T631;struct _tuple26 _T632;struct _tuple26 _T633;struct Cyc_Yystacktype*_T634;union Cyc_YYSTYPE*_T635;union Cyc_YYSTYPE*_T636;struct Cyc_Yystacktype*_T637;union Cyc_YYSTYPE*_T638;union Cyc_YYSTYPE*_T639;struct _tuple26 _T63A;struct Cyc_Yystacktype*_T63B;union Cyc_YYSTYPE*_T63C;union Cyc_YYSTYPE*_T63D;struct Cyc_Absyn_Tqual _T63E;struct _tuple26 _T63F;struct Cyc_Absyn_Tqual _T640;struct _tuple26 _T641;struct _tuple26 _T642;struct _tuple26 _T643;struct Cyc_Yystacktype*_T644;struct Cyc_Yystacktype _T645;struct Cyc_Yyltype _T646;unsigned _T647;unsigned _T648;struct Cyc_Yystacktype*_T649;union Cyc_YYSTYPE*_T64A;union Cyc_YYSTYPE*_T64B;struct Cyc_Yystacktype*_T64C;union Cyc_YYSTYPE*_T64D;union Cyc_YYSTYPE*_T64E;struct _tuple26 _T64F;struct _tuple26 _T650;struct _tuple26 _T651;struct Cyc_Yystacktype*_T652;union Cyc_YYSTYPE*_T653;union Cyc_YYSTYPE*_T654;struct Cyc_List_List*_T655;struct _tuple26 _T656;struct Cyc_List_List*_T657;struct Cyc_List_List*_T658;struct _RegionHandle*_T659;struct Cyc_Yystacktype*_T65A;union Cyc_YYSTYPE*_T65B;union Cyc_YYSTYPE*_T65C;struct Cyc_List_List*_T65D;struct _RegionHandle*_T65E;struct Cyc_Yystacktype*_T65F;union Cyc_YYSTYPE*_T660;union Cyc_YYSTYPE*_T661;struct Cyc_Yystacktype*_T662;union Cyc_YYSTYPE*_T663;union Cyc_YYSTYPE*_T664;struct _tuple12*_T665;struct _RegionHandle*_T666;struct Cyc_Yystacktype*_T667;union Cyc_YYSTYPE*_T668;union Cyc_YYSTYPE*_T669;struct Cyc_Yystacktype*_T66A;union Cyc_YYSTYPE*_T66B;union Cyc_YYSTYPE*_T66C;struct _tuple12*_T66D;struct _RegionHandle*_T66E;struct _tuple0*_T66F;struct _fat_ptr*_T670;struct Cyc_Yystacktype*_T671;union Cyc_YYSTYPE*_T672;union Cyc_YYSTYPE*_T673;struct _tuple12*_T674;struct _RegionHandle*_T675;struct _tuple0*_T676;struct _fat_ptr*_T677;struct _tuple12*_T678;struct _RegionHandle*_T679;struct Cyc_Yystacktype*_T67A;union Cyc_YYSTYPE*_T67B;union Cyc_YYSTYPE*_T67C;struct Cyc_Yystacktype*_T67D;union Cyc_YYSTYPE*_T67E;union Cyc_YYSTYPE*_T67F;struct Cyc_Yystacktype*_T680;union Cyc_YYSTYPE*_T681;union Cyc_YYSTYPE*_T682;struct Cyc_Absyn_Exp*_T683;struct Cyc_Yystacktype*_T684;union Cyc_YYSTYPE*_T685;union Cyc_YYSTYPE*_T686;struct Cyc_List_List*(*_T687)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T688)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T689;struct Cyc_Yystacktype _T68A;struct Cyc_Yyltype _T68B;unsigned _T68C;unsigned _T68D;struct Cyc_Yystacktype*_T68E;union Cyc_YYSTYPE*_T68F;union Cyc_YYSTYPE*_T690;struct Cyc_List_List*_T691;struct Cyc_Yystacktype*_T692;union Cyc_YYSTYPE*_T693;union Cyc_YYSTYPE*_T694;struct _tuple0*_T695;struct Cyc_List_List*_T696;struct Cyc_Core_Opt*_T697;struct Cyc_Yystacktype*_T698;union Cyc_YYSTYPE*_T699;union Cyc_YYSTYPE*_T69A;int _T69B;struct Cyc_Yystacktype*_T69C;struct Cyc_Yystacktype _T69D;struct Cyc_Yyltype _T69E;unsigned _T69F;unsigned _T6A0;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T6A1;void*_T6A2;struct Cyc_Yystacktype*_T6A3;struct Cyc_Yystacktype _T6A4;struct Cyc_Yyltype _T6A5;unsigned _T6A6;unsigned _T6A7;struct Cyc_Parse_Type_specifier _T6A8;struct Cyc_Yystacktype*_T6A9;union Cyc_YYSTYPE*_T6AA;union Cyc_YYSTYPE*_T6AB;struct Cyc_Absyn_UnknownDatatypeInfo _T6AC;struct Cyc_Yystacktype*_T6AD;union Cyc_YYSTYPE*_T6AE;union Cyc_YYSTYPE*_T6AF;union Cyc_Absyn_DatatypeInfo _T6B0;struct Cyc_Yystacktype*_T6B1;union Cyc_YYSTYPE*_T6B2;union Cyc_YYSTYPE*_T6B3;struct Cyc_List_List*_T6B4;void*_T6B5;struct Cyc_Yystacktype*_T6B6;struct Cyc_Yystacktype _T6B7;struct Cyc_Yyltype _T6B8;unsigned _T6B9;unsigned _T6BA;struct Cyc_Parse_Type_specifier _T6BB;struct Cyc_Yystacktype*_T6BC;union Cyc_YYSTYPE*_T6BD;union Cyc_YYSTYPE*_T6BE;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T6BF;struct Cyc_Yystacktype*_T6C0;union Cyc_YYSTYPE*_T6C1;union Cyc_YYSTYPE*_T6C2;struct Cyc_Yystacktype*_T6C3;union Cyc_YYSTYPE*_T6C4;union Cyc_YYSTYPE*_T6C5;union Cyc_Absyn_DatatypeFieldInfo _T6C6;struct Cyc_Yystacktype*_T6C7;union Cyc_YYSTYPE*_T6C8;union Cyc_YYSTYPE*_T6C9;struct Cyc_List_List*_T6CA;void*_T6CB;struct Cyc_Yystacktype*_T6CC;struct Cyc_Yystacktype _T6CD;struct Cyc_Yyltype _T6CE;unsigned _T6CF;unsigned _T6D0;struct Cyc_Parse_Type_specifier _T6D1;struct Cyc_List_List*_T6D2;struct Cyc_Yystacktype*_T6D3;union Cyc_YYSTYPE*_T6D4;union Cyc_YYSTYPE*_T6D5;struct Cyc_List_List*_T6D6;struct Cyc_Yystacktype*_T6D7;union Cyc_YYSTYPE*_T6D8;union Cyc_YYSTYPE*_T6D9;struct Cyc_List_List*_T6DA;struct Cyc_Yystacktype*_T6DB;union Cyc_YYSTYPE*_T6DC;union Cyc_YYSTYPE*_T6DD;struct Cyc_Yystacktype*_T6DE;union Cyc_YYSTYPE*_T6DF;union Cyc_YYSTYPE*_T6E0;struct Cyc_List_List*_T6E1;struct Cyc_Yystacktype*_T6E2;union Cyc_YYSTYPE*_T6E3;union Cyc_YYSTYPE*_T6E4;struct Cyc_Yystacktype*_T6E5;union Cyc_YYSTYPE*_T6E6;union Cyc_YYSTYPE*_T6E7;struct Cyc_Absyn_Datatypefield*_T6E8;struct Cyc_Yystacktype*_T6E9;union Cyc_YYSTYPE*_T6EA;union Cyc_YYSTYPE*_T6EB;struct Cyc_Yystacktype*_T6EC;struct Cyc_Yystacktype _T6ED;struct Cyc_Yyltype _T6EE;unsigned _T6EF;struct Cyc_Yystacktype*_T6F0;union Cyc_YYSTYPE*_T6F1;union Cyc_YYSTYPE*_T6F2;struct Cyc_List_List*(*_T6F3)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T6F4)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T6F5)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T6F6;struct Cyc_Yystacktype _T6F7;struct Cyc_Yyltype _T6F8;unsigned _T6F9;unsigned _T6FA;struct Cyc_Yystacktype*_T6FB;union Cyc_YYSTYPE*_T6FC;union Cyc_YYSTYPE*_T6FD;struct Cyc_List_List*_T6FE;struct Cyc_List_List*_T6FF;struct Cyc_Absyn_Datatypefield*_T700;struct Cyc_Yystacktype*_T701;union Cyc_YYSTYPE*_T702;union Cyc_YYSTYPE*_T703;struct Cyc_Yystacktype*_T704;struct Cyc_Yystacktype _T705;struct Cyc_Yyltype _T706;unsigned _T707;struct Cyc_Yystacktype*_T708;union Cyc_YYSTYPE*_T709;union Cyc_YYSTYPE*_T70A;struct Cyc_Yystacktype*_T70B;struct Cyc_Yystacktype _T70C;struct Cyc_Yystacktype*_T70D;union Cyc_YYSTYPE*_T70E;union Cyc_YYSTYPE*_T70F;struct Cyc_Parse_Declarator _T710;struct Cyc_Parse_Declarator _T711;struct Cyc_Parse_Declarator _T712;struct Cyc_Yystacktype*_T713;union Cyc_YYSTYPE*_T714;union Cyc_YYSTYPE*_T715;struct Cyc_List_List*_T716;struct Cyc_Parse_Declarator _T717;struct Cyc_List_List*_T718;struct Cyc_Yystacktype*_T719;struct Cyc_Yystacktype _T71A;struct Cyc_Yystacktype*_T71B;union Cyc_YYSTYPE*_T71C;union Cyc_YYSTYPE*_T71D;struct Cyc_Parse_Declarator _T71E;struct Cyc_Parse_Declarator _T71F;struct Cyc_Parse_Declarator _T720;struct Cyc_Yystacktype*_T721;union Cyc_YYSTYPE*_T722;union Cyc_YYSTYPE*_T723;struct Cyc_List_List*_T724;struct Cyc_Parse_Declarator _T725;struct Cyc_List_List*_T726;struct Cyc_Parse_Declarator _T727;struct Cyc_Yystacktype*_T728;union Cyc_YYSTYPE*_T729;union Cyc_YYSTYPE*_T72A;struct Cyc_Yystacktype*_T72B;struct Cyc_Yystacktype _T72C;struct Cyc_Yyltype _T72D;unsigned _T72E;struct Cyc_Yystacktype*_T72F;struct Cyc_Yystacktype _T730;struct Cyc_Yystacktype*_T731;union Cyc_YYSTYPE*_T732;union Cyc_YYSTYPE*_T733;struct Cyc_List_List*_T734;struct _RegionHandle*_T735;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T736;struct _RegionHandle*_T737;struct Cyc_Yystacktype*_T738;struct Cyc_Yystacktype _T739;struct Cyc_Yyltype _T73A;unsigned _T73B;struct Cyc_Yystacktype*_T73C;union Cyc_YYSTYPE*_T73D;union Cyc_YYSTYPE*_T73E;struct Cyc_Parse_Declarator _T73F;struct Cyc_Yystacktype*_T740;struct Cyc_Yystacktype _T741;struct Cyc_Parse_Declarator _T742;struct Cyc_Yystacktype*_T743;union Cyc_YYSTYPE*_T744;union Cyc_YYSTYPE*_T745;struct Cyc_Parse_Declarator _T746;struct Cyc_Yystacktype*_T747;union Cyc_YYSTYPE*_T748;union Cyc_YYSTYPE*_T749;struct Cyc_Parse_Declarator _T74A;struct Cyc_List_List*_T74B;struct _RegionHandle*_T74C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T74D;struct _RegionHandle*_T74E;struct Cyc_Yystacktype*_T74F;union Cyc_YYSTYPE*_T750;union Cyc_YYSTYPE*_T751;struct Cyc_Yystacktype*_T752;struct Cyc_Yystacktype _T753;struct Cyc_Yyltype _T754;unsigned _T755;struct Cyc_Yystacktype*_T756;union Cyc_YYSTYPE*_T757;union Cyc_YYSTYPE*_T758;struct Cyc_Parse_Declarator _T759;struct Cyc_Parse_Declarator _T75A;struct Cyc_Yystacktype*_T75B;union Cyc_YYSTYPE*_T75C;union Cyc_YYSTYPE*_T75D;struct Cyc_Parse_Declarator _T75E;struct Cyc_Yystacktype*_T75F;union Cyc_YYSTYPE*_T760;union Cyc_YYSTYPE*_T761;struct Cyc_Parse_Declarator _T762;struct Cyc_List_List*_T763;struct _RegionHandle*_T764;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T765;struct _RegionHandle*_T766;struct Cyc_Yystacktype*_T767;union Cyc_YYSTYPE*_T768;union Cyc_YYSTYPE*_T769;struct Cyc_Yystacktype*_T76A;union Cyc_YYSTYPE*_T76B;union Cyc_YYSTYPE*_T76C;struct Cyc_Yystacktype*_T76D;struct Cyc_Yystacktype _T76E;struct Cyc_Yyltype _T76F;unsigned _T770;struct Cyc_Yystacktype*_T771;union Cyc_YYSTYPE*_T772;union Cyc_YYSTYPE*_T773;struct Cyc_Parse_Declarator _T774;struct Cyc_Yystacktype*_T775;union Cyc_YYSTYPE*_T776;union Cyc_YYSTYPE*_T777;struct Cyc_Yystacktype*_T778;union Cyc_YYSTYPE*_T779;union Cyc_YYSTYPE*_T77A;struct Cyc_Parse_Declarator _T77B;struct Cyc_Yystacktype*_T77C;union Cyc_YYSTYPE*_T77D;union Cyc_YYSTYPE*_T77E;struct Cyc_Parse_Declarator _T77F;struct Cyc_Yystacktype*_T780;union Cyc_YYSTYPE*_T781;union Cyc_YYSTYPE*_T782;struct Cyc_Parse_Declarator _T783;struct Cyc_List_List*_T784;struct _RegionHandle*_T785;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T786;struct _RegionHandle*_T787;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T788;struct _RegionHandle*_T789;struct Cyc_Yystacktype*_T78A;union Cyc_YYSTYPE*_T78B;union Cyc_YYSTYPE*_T78C;struct Cyc_Parse_Declarator _T78D;struct Cyc_Parse_Declarator _T78E;struct Cyc_Yystacktype*_T78F;union Cyc_YYSTYPE*_T790;union Cyc_YYSTYPE*_T791;struct Cyc_Parse_Declarator _T792;struct Cyc_Yystacktype*_T793;union Cyc_YYSTYPE*_T794;union Cyc_YYSTYPE*_T795;struct Cyc_Parse_Declarator _T796;struct Cyc_List_List*_T797;struct _RegionHandle*_T798;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T799;struct _RegionHandle*_T79A;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T79B;struct _RegionHandle*_T79C;struct Cyc_Yystacktype*_T79D;union Cyc_YYSTYPE*_T79E;union Cyc_YYSTYPE*_T79F;struct Cyc_Yystacktype*_T7A0;struct Cyc_Yystacktype _T7A1;struct Cyc_Yyltype _T7A2;unsigned _T7A3;struct Cyc_Yystacktype*_T7A4;union Cyc_YYSTYPE*_T7A5;union Cyc_YYSTYPE*_T7A6;struct Cyc_Parse_Declarator _T7A7;struct Cyc_List_List*(*_T7A8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T7A9)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T7AA;struct Cyc_Yystacktype _T7AB;struct Cyc_Yyltype _T7AC;unsigned _T7AD;unsigned _T7AE;struct Cyc_Yystacktype*_T7AF;union Cyc_YYSTYPE*_T7B0;union Cyc_YYSTYPE*_T7B1;struct Cyc_List_List*_T7B2;struct Cyc_List_List*_T7B3;struct Cyc_Parse_Declarator _T7B4;struct Cyc_Yystacktype*_T7B5;union Cyc_YYSTYPE*_T7B6;union Cyc_YYSTYPE*_T7B7;struct Cyc_Parse_Declarator _T7B8;struct Cyc_Yystacktype*_T7B9;union Cyc_YYSTYPE*_T7BA;union Cyc_YYSTYPE*_T7BB;struct Cyc_Parse_Declarator _T7BC;struct Cyc_List_List*_T7BD;struct _RegionHandle*_T7BE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T7BF;struct _RegionHandle*_T7C0;struct Cyc_Yystacktype*_T7C1;struct Cyc_Yystacktype _T7C2;struct Cyc_Yyltype _T7C3;unsigned _T7C4;struct Cyc_Yystacktype*_T7C5;union Cyc_YYSTYPE*_T7C6;union Cyc_YYSTYPE*_T7C7;struct Cyc_Parse_Declarator _T7C8;struct Cyc_Parse_Declarator _T7C9;struct Cyc_Yystacktype*_T7CA;union Cyc_YYSTYPE*_T7CB;union Cyc_YYSTYPE*_T7CC;struct Cyc_Parse_Declarator _T7CD;struct Cyc_Yystacktype*_T7CE;union Cyc_YYSTYPE*_T7CF;union Cyc_YYSTYPE*_T7D0;struct Cyc_Parse_Declarator _T7D1;struct Cyc_List_List*_T7D2;struct _RegionHandle*_T7D3;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T7D4;struct _RegionHandle*_T7D5;struct Cyc_Yystacktype*_T7D6;struct Cyc_Yystacktype _T7D7;struct Cyc_Yyltype _T7D8;unsigned _T7D9;struct Cyc_Yystacktype*_T7DA;union Cyc_YYSTYPE*_T7DB;union Cyc_YYSTYPE*_T7DC;struct Cyc_Yystacktype*_T7DD;union Cyc_YYSTYPE*_T7DE;union Cyc_YYSTYPE*_T7DF;struct Cyc_Parse_Declarator _T7E0;struct Cyc_Parse_Declarator _T7E1;struct Cyc_Yystacktype*_T7E2;union Cyc_YYSTYPE*_T7E3;union Cyc_YYSTYPE*_T7E4;struct Cyc_Yystacktype*_T7E5;struct Cyc_Yystacktype _T7E6;struct Cyc_Yyltype _T7E7;unsigned _T7E8;struct Cyc_Parse_Declarator _T7E9;struct Cyc_Yystacktype*_T7EA;union Cyc_YYSTYPE*_T7EB;union Cyc_YYSTYPE*_T7EC;struct Cyc_Yystacktype*_T7ED;struct Cyc_Yystacktype _T7EE;struct Cyc_Yyltype _T7EF;unsigned _T7F0;struct Cyc_Yystacktype*_T7F1;struct Cyc_Yystacktype _T7F2;struct Cyc_Yystacktype*_T7F3;union Cyc_YYSTYPE*_T7F4;union Cyc_YYSTYPE*_T7F5;struct Cyc_List_List*_T7F6;struct _RegionHandle*_T7F7;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T7F8;struct _RegionHandle*_T7F9;struct Cyc_Yystacktype*_T7FA;struct Cyc_Yystacktype _T7FB;struct Cyc_Yyltype _T7FC;unsigned _T7FD;struct Cyc_Yystacktype*_T7FE;union Cyc_YYSTYPE*_T7FF;union Cyc_YYSTYPE*_T800;struct Cyc_Parse_Declarator _T801;struct Cyc_Yystacktype*_T802;struct Cyc_Yystacktype _T803;struct Cyc_Yystacktype*_T804;union Cyc_YYSTYPE*_T805;union Cyc_YYSTYPE*_T806;struct Cyc_Parse_Declarator _T807;struct Cyc_Parse_Declarator _T808;struct Cyc_Parse_Declarator _T809;struct Cyc_List_List*_T80A;struct _RegionHandle*_T80B;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T80C;struct _RegionHandle*_T80D;struct Cyc_Yystacktype*_T80E;union Cyc_YYSTYPE*_T80F;union Cyc_YYSTYPE*_T810;struct Cyc_Yystacktype*_T811;struct Cyc_Yystacktype _T812;struct Cyc_Yyltype _T813;unsigned _T814;struct Cyc_Parse_Declarator _T815;struct Cyc_Yystacktype*_T816;union Cyc_YYSTYPE*_T817;union Cyc_YYSTYPE*_T818;struct Cyc_Parse_Declarator _T819;struct Cyc_Parse_Declarator _T81A;struct Cyc_Parse_Declarator _T81B;struct Cyc_List_List*_T81C;struct _RegionHandle*_T81D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T81E;struct _RegionHandle*_T81F;struct Cyc_Yystacktype*_T820;union Cyc_YYSTYPE*_T821;union Cyc_YYSTYPE*_T822;struct Cyc_Yystacktype*_T823;union Cyc_YYSTYPE*_T824;union Cyc_YYSTYPE*_T825;struct Cyc_Yystacktype*_T826;struct Cyc_Yystacktype _T827;struct Cyc_Yyltype _T828;unsigned _T829;struct Cyc_Parse_Declarator _T82A;struct Cyc_Yystacktype*_T82B;union Cyc_YYSTYPE*_T82C;union Cyc_YYSTYPE*_T82D;struct Cyc_Yystacktype*_T82E;union Cyc_YYSTYPE*_T82F;union Cyc_YYSTYPE*_T830;struct Cyc_Yystacktype*_T831;union Cyc_YYSTYPE*_T832;union Cyc_YYSTYPE*_T833;struct Cyc_Parse_Declarator _T834;struct Cyc_Parse_Declarator _T835;struct Cyc_Parse_Declarator _T836;struct Cyc_List_List*_T837;struct _RegionHandle*_T838;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T839;struct _RegionHandle*_T83A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T83B;struct _RegionHandle*_T83C;struct Cyc_Parse_Declarator _T83D;struct Cyc_Yystacktype*_T83E;union Cyc_YYSTYPE*_T83F;union Cyc_YYSTYPE*_T840;struct Cyc_Parse_Declarator _T841;struct Cyc_Parse_Declarator _T842;struct Cyc_Parse_Declarator _T843;struct Cyc_List_List*_T844;struct _RegionHandle*_T845;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T846;struct _RegionHandle*_T847;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T848;struct _RegionHandle*_T849;struct Cyc_Yystacktype*_T84A;union Cyc_YYSTYPE*_T84B;union Cyc_YYSTYPE*_T84C;struct Cyc_Yystacktype*_T84D;struct Cyc_Yystacktype _T84E;struct Cyc_Yyltype _T84F;unsigned _T850;struct Cyc_Parse_Declarator _T851;struct Cyc_List_List*(*_T852)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T853)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T854;struct Cyc_Yystacktype _T855;struct Cyc_Yyltype _T856;unsigned _T857;unsigned _T858;struct Cyc_Yystacktype*_T859;union Cyc_YYSTYPE*_T85A;union Cyc_YYSTYPE*_T85B;struct Cyc_List_List*_T85C;struct Cyc_List_List*_T85D;struct Cyc_Yystacktype*_T85E;union Cyc_YYSTYPE*_T85F;union Cyc_YYSTYPE*_T860;struct Cyc_Parse_Declarator _T861;struct Cyc_Parse_Declarator _T862;struct Cyc_Parse_Declarator _T863;struct Cyc_List_List*_T864;struct _RegionHandle*_T865;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T866;struct _RegionHandle*_T867;struct Cyc_Yystacktype*_T868;struct Cyc_Yystacktype _T869;struct Cyc_Yyltype _T86A;unsigned _T86B;struct Cyc_Parse_Declarator _T86C;struct Cyc_Yystacktype*_T86D;union Cyc_YYSTYPE*_T86E;union Cyc_YYSTYPE*_T86F;struct Cyc_Parse_Declarator _T870;struct Cyc_Parse_Declarator _T871;struct Cyc_Parse_Declarator _T872;struct Cyc_List_List*_T873;struct _RegionHandle*_T874;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T875;struct _RegionHandle*_T876;struct Cyc_Yystacktype*_T877;struct Cyc_Yystacktype _T878;struct Cyc_Yyltype _T879;unsigned _T87A;struct Cyc_Yystacktype*_T87B;union Cyc_YYSTYPE*_T87C;union Cyc_YYSTYPE*_T87D;struct Cyc_Parse_Declarator _T87E;struct Cyc_Yystacktype*_T87F;struct Cyc_Yystacktype _T880;struct Cyc_Yystacktype*_T881;union Cyc_YYSTYPE*_T882;union Cyc_YYSTYPE*_T883;struct Cyc_List_List*_T884;struct Cyc_Yystacktype*_T885;union Cyc_YYSTYPE*_T886;union Cyc_YYSTYPE*_T887;struct Cyc_List_List*_T888;struct Cyc_List_List*_T889;struct Cyc_Yystacktype*_T88A;union Cyc_YYSTYPE*_T88B;union Cyc_YYSTYPE*_T88C;struct Cyc_List_List*_T88D;struct Cyc_List_List*_T88E;struct _RegionHandle*_T88F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T890;struct _RegionHandle*_T891;struct Cyc_Yystacktype*_T892;struct Cyc_Yystacktype _T893;struct Cyc_Yyltype _T894;unsigned _T895;struct Cyc_Yystacktype*_T896;union Cyc_YYSTYPE*_T897;union Cyc_YYSTYPE*_T898;struct Cyc_Yystacktype*_T899;union Cyc_YYSTYPE*_T89A;union Cyc_YYSTYPE*_T89B;struct _tuple22*_T89C;int _T89D;struct Cyc_Absyn_PtrLoc*_T89E;struct Cyc_Yystacktype*_T89F;struct Cyc_Yystacktype _T8A0;struct Cyc_Yyltype _T8A1;unsigned _T8A2;struct Cyc_Yystacktype*_T8A3;struct Cyc_Yystacktype _T8A4;struct Cyc_Yyltype _T8A5;unsigned _T8A6;struct _RegionHandle*_T8A7;struct Cyc_Absyn_PtrLoc*_T8A8;void*_T8A9;void*_T8AA;struct Cyc_Yystacktype*_T8AB;union Cyc_YYSTYPE*_T8AC;union Cyc_YYSTYPE*_T8AD;void*_T8AE;struct Cyc_Yystacktype*_T8AF;union Cyc_YYSTYPE*_T8B0;union Cyc_YYSTYPE*_T8B1;struct Cyc_List_List*_T8B2;struct Cyc_Yystacktype*_T8B3;union Cyc_YYSTYPE*_T8B4;union Cyc_YYSTYPE*_T8B5;struct Cyc_Absyn_Tqual _T8B6;struct Cyc_List_List*_T8B7;struct _RegionHandle*_T8B8;struct Cyc_List_List*_T8B9;struct _RegionHandle*_T8BA;struct Cyc_Yystacktype*_T8BB;union Cyc_YYSTYPE*_T8BC;union Cyc_YYSTYPE*_T8BD;struct Cyc_Yystacktype*_T8BE;union Cyc_YYSTYPE*_T8BF;union Cyc_YYSTYPE*_T8C0;struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T8C1;struct _RegionHandle*_T8C2;struct Cyc_Yystacktype*_T8C3;union Cyc_YYSTYPE*_T8C4;union Cyc_YYSTYPE*_T8C5;void*_T8C6;struct Cyc_Yystacktype*_T8C7;struct Cyc_Yystacktype _T8C8;struct Cyc_Yyltype _T8C9;unsigned _T8CA;unsigned _T8CB;struct _fat_ptr _T8CC;struct _fat_ptr _T8CD;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8CE;struct _RegionHandle*_T8CF;struct Cyc_List_List*_T8D0;struct Cyc_Yystacktype*_T8D1;union Cyc_YYSTYPE*_T8D2;union Cyc_YYSTYPE*_T8D3;void*_T8D4;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8D5;struct _RegionHandle*_T8D6;struct Cyc_Yystacktype*_T8D7;union Cyc_YYSTYPE*_T8D8;union Cyc_YYSTYPE*_T8D9;void*_T8DA;struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T8DB;struct _RegionHandle*_T8DC;void*_T8DD;struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T8DE;struct _RegionHandle*_T8DF;void*_T8E0;struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T8E1;struct _RegionHandle*_T8E2;void*_T8E3;struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T8E4;struct _RegionHandle*_T8E5;void*_T8E6;struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T8E7;struct _RegionHandle*_T8E8;void*_T8E9;struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T8EA;struct _RegionHandle*_T8EB;void*_T8EC;struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T8ED;struct _RegionHandle*_T8EE;void*_T8EF;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T8F0;struct _RegionHandle*_T8F1;struct Cyc_Yystacktype*_T8F2;union Cyc_YYSTYPE*_T8F3;union Cyc_YYSTYPE*_T8F4;void*_T8F5;struct Cyc_Yystacktype*_T8F6;struct Cyc_Yystacktype _T8F7;struct Cyc_Yyltype _T8F8;unsigned _T8F9;unsigned _T8FA;struct Cyc_Yystacktype*_T8FB;union Cyc_YYSTYPE*_T8FC;union Cyc_YYSTYPE*_T8FD;struct _fat_ptr _T8FE;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T8FF;struct _RegionHandle*_T900;void*_T901;struct Cyc_Yystacktype*_T902;union Cyc_YYSTYPE*_T903;union Cyc_YYSTYPE*_T904;void*_T905;struct Cyc_Yystacktype*_T906;union Cyc_YYSTYPE*_T907;union Cyc_YYSTYPE*_T908;void*_T909;void*_T90A;void*_T90B;struct Cyc_Yystacktype*_T90C;union Cyc_YYSTYPE*_T90D;union Cyc_YYSTYPE*_T90E;void*_T90F;void*_T910;void*_T911;void*_T912;struct Cyc_Yystacktype*_T913;struct Cyc_Yystacktype _T914;struct Cyc_Yyltype _T915;unsigned _T916;unsigned _T917;struct Cyc_Yystacktype*_T918;union Cyc_YYSTYPE*_T919;union Cyc_YYSTYPE*_T91A;struct _fat_ptr _T91B;void*_T91C;struct _tuple22*_T91D;struct Cyc_Yystacktype*_T91E;struct Cyc_Yystacktype _T91F;struct Cyc_Yyltype _T920;unsigned _T921;int _T922;struct Cyc_Yystacktype*_T923;struct Cyc_Yystacktype _T924;struct Cyc_Yyltype _T925;unsigned _T926;unsigned _T927;struct _fat_ptr _T928;struct Cyc_Yystacktype*_T929;union Cyc_YYSTYPE*_T92A;union Cyc_YYSTYPE*_T92B;void*_T92C;struct _tuple22*_T92D;struct Cyc_Yystacktype*_T92E;struct Cyc_Yystacktype _T92F;struct Cyc_Yyltype _T930;unsigned _T931;struct Cyc_Yystacktype*_T932;struct Cyc_Yystacktype _T933;struct Cyc_Yyltype _T934;unsigned _T935;unsigned _T936;struct _fat_ptr _T937;struct Cyc_Yystacktype*_T938;union Cyc_YYSTYPE*_T939;union Cyc_YYSTYPE*_T93A;void*_T93B;struct _tuple22*_T93C;int _T93D;int _T93E;struct _tuple22*_T93F;struct Cyc_Yystacktype*_T940;struct Cyc_Yystacktype _T941;struct Cyc_Yyltype _T942;unsigned _T943;struct Cyc_Yystacktype*_T944;struct Cyc_Yystacktype _T945;struct Cyc_Yyltype _T946;unsigned _T947;unsigned _T948;struct _fat_ptr _T949;struct Cyc_Core_Opt*_T94A;struct Cyc_Core_Opt*_T94B;void*_T94C;struct _tuple22*_T94D;struct Cyc_Yystacktype*_T94E;struct Cyc_Yystacktype _T94F;struct Cyc_Yyltype _T950;unsigned _T951;void*_T952;int _T953;struct Cyc_Core_Opt*_T954;struct Cyc_Core_Opt*_T955;struct Cyc_Yystacktype*_T956;union Cyc_YYSTYPE*_T957;union Cyc_YYSTYPE*_T958;struct Cyc_Absyn_Exp*_T959;void*_T95A;int _T95B;int(*_T95C)(unsigned,struct _fat_ptr);struct _fat_ptr _T95D;struct Cyc_Yystacktype*_T95E;union Cyc_YYSTYPE*_T95F;union Cyc_YYSTYPE*_T960;struct _fat_ptr _T961;void*_T962;void*_T963;struct Cyc_List_List*_T964;struct Cyc_Yystacktype*_T965;union Cyc_YYSTYPE*_T966;union Cyc_YYSTYPE*_T967;struct Cyc_List_List*_T968;struct Cyc_Yystacktype*_T969;union Cyc_YYSTYPE*_T96A;union Cyc_YYSTYPE*_T96B;struct Cyc_Yystacktype*_T96C;union Cyc_YYSTYPE*_T96D;union Cyc_YYSTYPE*_T96E;struct Cyc_Core_Opt*_T96F;struct Cyc_Core_Opt*_T970;void*_T971;struct Cyc_Yystacktype*_T972;union Cyc_YYSTYPE*_T973;union Cyc_YYSTYPE*_T974;int _T975;struct Cyc_List_List*_T976;void*_T977;struct Cyc_Yystacktype*_T978;union Cyc_YYSTYPE*_T979;union Cyc_YYSTYPE*_T97A;struct Cyc_List_List*_T97B;void*_T97C;struct Cyc_Core_Opt*_T97D;struct Cyc_Core_Opt*_T97E;void*_T97F;struct _fat_ptr _T980;int _T981;char*_T982;struct Cyc_Yystacktype*_T983;struct Cyc_Yystacktype _T984;struct Cyc_Yyltype _T985;unsigned _T986;unsigned _T987;struct Cyc_Absyn_Tqual _T988;struct Cyc_Yystacktype*_T989;union Cyc_YYSTYPE*_T98A;union Cyc_YYSTYPE*_T98B;struct Cyc_Absyn_Tqual _T98C;struct Cyc_Yystacktype*_T98D;union Cyc_YYSTYPE*_T98E;union Cyc_YYSTYPE*_T98F;struct Cyc_Absyn_Tqual _T990;struct Cyc_Absyn_Tqual _T991;struct Cyc_Yystacktype*_T992;union Cyc_YYSTYPE*_T993;union Cyc_YYSTYPE*_T994;struct _tuple28 _T995;struct _tuple28*_T996;unsigned _T997;struct _tuple28*_T998;struct _tuple28 _T999;struct _tuple27*_T99A;struct Cyc_Yystacktype*_T99B;union Cyc_YYSTYPE*_T99C;union Cyc_YYSTYPE*_T99D;struct Cyc_Yystacktype*_T99E;union Cyc_YYSTYPE*_T99F;union Cyc_YYSTYPE*_T9A0;struct _tuple28 _T9A1;struct _tuple28*_T9A2;unsigned _T9A3;struct _tuple28*_T9A4;struct _tuple28 _T9A5;struct _tuple27*_T9A6;struct Cyc_Yystacktype*_T9A7;union Cyc_YYSTYPE*_T9A8;union Cyc_YYSTYPE*_T9A9;struct Cyc_List_List*_T9AA;struct Cyc_Yystacktype*_T9AB;union Cyc_YYSTYPE*_T9AC;union Cyc_YYSTYPE*_T9AD;struct Cyc_Yystacktype*_T9AE;union Cyc_YYSTYPE*_T9AF;union Cyc_YYSTYPE*_T9B0;struct _tuple28 _T9B1;struct _tuple28*_T9B2;unsigned _T9B3;struct _tuple28*_T9B4;struct _tuple28 _T9B5;struct _tuple27*_T9B6;struct Cyc_Yystacktype*_T9B7;union Cyc_YYSTYPE*_T9B8;union Cyc_YYSTYPE*_T9B9;struct Cyc_List_List*_T9BA;struct Cyc_Yystacktype*_T9BB;union Cyc_YYSTYPE*_T9BC;union Cyc_YYSTYPE*_T9BD;struct Cyc_Yystacktype*_T9BE;union Cyc_YYSTYPE*_T9BF;union Cyc_YYSTYPE*_T9C0;struct Cyc_Absyn_VarargInfo*_T9C1;struct Cyc_Absyn_VarargInfo*_T9C2;struct Cyc_Absyn_VarargInfo*_T9C3;struct Cyc_Absyn_VarargInfo*_T9C4;struct Cyc_Yystacktype*_T9C5;union Cyc_YYSTYPE*_T9C6;union Cyc_YYSTYPE*_T9C7;struct Cyc_Yystacktype*_T9C8;union Cyc_YYSTYPE*_T9C9;union Cyc_YYSTYPE*_T9CA;struct _tuple28 _T9CB;struct _tuple28*_T9CC;unsigned _T9CD;struct _tuple28*_T9CE;struct _tuple28 _T9CF;struct _tuple27*_T9D0;struct Cyc_Yystacktype*_T9D1;union Cyc_YYSTYPE*_T9D2;union Cyc_YYSTYPE*_T9D3;struct Cyc_Yystacktype*_T9D4;union Cyc_YYSTYPE*_T9D5;union Cyc_YYSTYPE*_T9D6;struct Cyc_Absyn_VarargInfo*_T9D7;struct Cyc_Absyn_VarargInfo*_T9D8;struct Cyc_Absyn_VarargInfo*_T9D9;struct Cyc_Absyn_VarargInfo*_T9DA;struct Cyc_Yystacktype*_T9DB;union Cyc_YYSTYPE*_T9DC;union Cyc_YYSTYPE*_T9DD;struct Cyc_Yystacktype*_T9DE;union Cyc_YYSTYPE*_T9DF;union Cyc_YYSTYPE*_T9E0;struct _tuple28 _T9E1;struct _tuple28*_T9E2;unsigned _T9E3;struct _tuple28*_T9E4;struct _tuple28 _T9E5;struct _tuple27*_T9E6;struct Cyc_Yystacktype*_T9E7;union Cyc_YYSTYPE*_T9E8;union Cyc_YYSTYPE*_T9E9;struct Cyc_List_List*_T9EA;struct Cyc_Yystacktype*_T9EB;union Cyc_YYSTYPE*_T9EC;union Cyc_YYSTYPE*_T9ED;struct Cyc_Yystacktype*_T9EE;struct Cyc_Yystacktype _T9EF;struct Cyc_Yyltype _T9F0;unsigned _T9F1;unsigned _T9F2;struct Cyc_Yystacktype*_T9F3;union Cyc_YYSTYPE*_T9F4;union Cyc_YYSTYPE*_T9F5;struct _fat_ptr _T9F6;void*_T9F7;struct Cyc_Yystacktype*_T9F8;union Cyc_YYSTYPE*_T9F9;union Cyc_YYSTYPE*_T9FA;struct _fat_ptr _T9FB;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T9FC;void*_T9FD;struct Cyc_Yystacktype*_T9FE;union Cyc_YYSTYPE*_T9FF;union Cyc_YYSTYPE*_TA00;void*_TA01;struct Cyc_Yystacktype*_TA02;struct Cyc_Yystacktype _TA03;struct Cyc_Yyltype _TA04;unsigned _TA05;unsigned _TA06;void*_TA07;struct Cyc_Yystacktype*_TA08;union Cyc_YYSTYPE*_TA09;union Cyc_YYSTYPE*_TA0A;struct _fat_ptr _TA0B;struct Cyc_Yystacktype*_TA0C;union Cyc_YYSTYPE*_TA0D;union Cyc_YYSTYPE*_TA0E;struct Cyc_Absyn_Kind*_TA0F;void*_TA10;struct Cyc_Yystacktype*_TA11;union Cyc_YYSTYPE*_TA12;union Cyc_YYSTYPE*_TA13;void*_TA14;struct Cyc_Yystacktype*_TA15;struct Cyc_Yystacktype _TA16;struct Cyc_Yyltype _TA17;unsigned _TA18;unsigned _TA19;void*_TA1A;struct Cyc_Yystacktype*_TA1B;union Cyc_YYSTYPE*_TA1C;union Cyc_YYSTYPE*_TA1D;struct Cyc_List_List*_TA1E;void*_TA1F;struct Cyc_Yystacktype*_TA20;struct Cyc_Yystacktype _TA21;struct _tuple28*_TA22;struct Cyc_List_List*_TA23;void**_TA24;struct Cyc_Yystacktype*_TA25;union Cyc_YYSTYPE*_TA26;union Cyc_YYSTYPE*_TA27;struct _fat_ptr _TA28;struct _tuple28*_TA29;struct Cyc_List_List*_TA2A;struct _tuple29**_TA2B;struct Cyc_Yystacktype*_TA2C;union Cyc_YYSTYPE*_TA2D;union Cyc_YYSTYPE*_TA2E;struct _fat_ptr _TA2F;struct Cyc_Yystacktype*_TA30;union Cyc_YYSTYPE*_TA31;union Cyc_YYSTYPE*_TA32;struct _tuple28*_TA33;struct _tuple28*_TA34;struct Cyc_List_List*_TA35;struct Cyc_Yystacktype*_TA36;union Cyc_YYSTYPE*_TA37;union Cyc_YYSTYPE*_TA38;struct Cyc_Yystacktype*_TA39;union Cyc_YYSTYPE*_TA3A;union Cyc_YYSTYPE*_TA3B;struct _tuple28*_TA3C;struct _tuple28*_TA3D;struct Cyc_List_List*_TA3E;struct Cyc_Yystacktype*_TA3F;union Cyc_YYSTYPE*_TA40;union Cyc_YYSTYPE*_TA41;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA42;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA43;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA44;struct Cyc_Absyn_Kind*_TA45;struct Cyc_Yystacktype*_TA46;union Cyc_YYSTYPE*_TA47;union Cyc_YYSTYPE*_TA48;struct _fat_ptr _TA49;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA4A;void*_TA4B;struct Cyc_Yystacktype*_TA4C;struct Cyc_Yystacktype _TA4D;struct Cyc_Yyltype _TA4E;unsigned _TA4F;unsigned _TA50;struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_TA51;struct Cyc_Yystacktype*_TA52;union Cyc_YYSTYPE*_TA53;union Cyc_YYSTYPE*_TA54;struct Cyc_List_List*_TA55;void*_TA56;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_TA57;struct Cyc_Yystacktype*_TA58;union Cyc_YYSTYPE*_TA59;union Cyc_YYSTYPE*_TA5A;struct Cyc_List_List*_TA5B;struct Cyc_Yystacktype*_TA5C;union Cyc_YYSTYPE*_TA5D;union Cyc_YYSTYPE*_TA5E;struct Cyc_List_List*_TA5F;void*_TA60;struct Cyc_Yystacktype*_TA61;struct Cyc_Yystacktype _TA62;struct Cyc_Yyltype _TA63;unsigned _TA64;unsigned _TA65;struct Cyc_Yystacktype*_TA66;union Cyc_YYSTYPE*_TA67;union Cyc_YYSTYPE*_TA68;struct _fat_ptr _TA69;struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_TA6A;struct Cyc_Yystacktype*_TA6B;union Cyc_YYSTYPE*_TA6C;union Cyc_YYSTYPE*_TA6D;struct Cyc_List_List*_TA6E;void*_TA6F;struct _tuple29*_TA70;struct Cyc_Yystacktype*_TA71;union Cyc_YYSTYPE*_TA72;union Cyc_YYSTYPE*_TA73;struct Cyc_Yystacktype*_TA74;union Cyc_YYSTYPE*_TA75;union Cyc_YYSTYPE*_TA76;struct Cyc_Yystacktype*_TA77;struct Cyc_Yystacktype _TA78;struct Cyc_Yyltype _TA79;unsigned _TA7A;unsigned _TA7B;struct Cyc_Yystacktype*_TA7C;union Cyc_YYSTYPE*_TA7D;union Cyc_YYSTYPE*_TA7E;struct _fat_ptr _TA7F;void*_TA80;struct Cyc_Yystacktype*_TA81;union Cyc_YYSTYPE*_TA82;union Cyc_YYSTYPE*_TA83;void*_TA84;struct Cyc_Yystacktype*_TA85;union Cyc_YYSTYPE*_TA86;union Cyc_YYSTYPE*_TA87;void*_TA88;void*_TA89;void*_TA8A;void*_TA8B;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA8C;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA8D;struct Cyc_Absyn_Kind*_TA8E;struct Cyc_Yystacktype*_TA8F;union Cyc_YYSTYPE*_TA90;union Cyc_YYSTYPE*_TA91;struct _fat_ptr _TA92;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA93;void*_TA94;struct Cyc_Yystacktype*_TA95;struct Cyc_Yystacktype _TA96;struct Cyc_Yyltype _TA97;unsigned _TA98;unsigned _TA99;void*_TA9A;struct Cyc_Yystacktype*_TA9B;union Cyc_YYSTYPE*_TA9C;union Cyc_YYSTYPE*_TA9D;void*_TA9E;void*_TA9F;struct Cyc_Yystacktype*_TAA0;union Cyc_YYSTYPE*_TAA1;union Cyc_YYSTYPE*_TAA2;struct _fat_ptr _TAA3;struct _fat_ptr _TAA4;int _TAA5;struct Cyc_Warn_String_Warn_Warg_struct _TAA6;void**_TAA7;struct Cyc_Yystacktype*_TAA8;struct Cyc_Yystacktype _TAA9;struct Cyc_Yyltype _TAAA;unsigned _TAAB;unsigned _TAAC;struct _fat_ptr _TAAD;struct Cyc_Yystacktype*_TAAE;struct Cyc_Yystacktype _TAAF;struct Cyc_Yystacktype*_TAB0;union Cyc_YYSTYPE*_TAB1;union Cyc_YYSTYPE*_TAB2;struct Cyc_List_List*_TAB3;struct Cyc_Yystacktype*_TAB4;union Cyc_YYSTYPE*_TAB5;union Cyc_YYSTYPE*_TAB6;struct Cyc_List_List*_TAB7;struct Cyc_List_List*_TAB8;struct Cyc_Yystacktype*_TAB9;struct Cyc_Yystacktype _TABA;struct Cyc_List_List*_TABB;struct Cyc_Yystacktype*_TABC;union Cyc_YYSTYPE*_TABD;union Cyc_YYSTYPE*_TABE;void*_TABF;struct Cyc_Yystacktype*_TAC0;union Cyc_YYSTYPE*_TAC1;union Cyc_YYSTYPE*_TAC2;void*_TAC3;struct Cyc_Absyn_Kind*_TAC4;struct Cyc_Absyn_Kind*_TAC5;struct Cyc_List_List*_TAC6;struct Cyc_Yystacktype*_TAC7;union Cyc_YYSTYPE*_TAC8;union Cyc_YYSTYPE*_TAC9;struct Cyc_List_List*_TACA;struct Cyc_Yystacktype*_TACB;union Cyc_YYSTYPE*_TACC;union Cyc_YYSTYPE*_TACD;struct _tuple8*_TACE;struct Cyc_Yystacktype*_TACF;struct Cyc_Yystacktype _TAD0;struct Cyc_Yyltype _TAD1;unsigned _TAD2;unsigned _TAD3;struct Cyc_List_List*_TAD4;struct Cyc_Yystacktype*_TAD5;union Cyc_YYSTYPE*_TAD6;union Cyc_YYSTYPE*_TAD7;struct _tuple8*_TAD8;struct Cyc_Yystacktype*_TAD9;struct Cyc_Yystacktype _TADA;struct Cyc_Yyltype _TADB;unsigned _TADC;unsigned _TADD;struct Cyc_Yystacktype*_TADE;union Cyc_YYSTYPE*_TADF;union Cyc_YYSTYPE*_TAE0;struct Cyc_List_List*_TAE1;struct Cyc_Yystacktype*_TAE2;union Cyc_YYSTYPE*_TAE3;union Cyc_YYSTYPE*_TAE4;struct Cyc_List_List*_TAE5;struct Cyc_Yystacktype*_TAE6;union Cyc_YYSTYPE*_TAE7;union Cyc_YYSTYPE*_TAE8;struct Cyc_Yystacktype*_TAE9;union Cyc_YYSTYPE*_TAEA;union Cyc_YYSTYPE*_TAEB;struct Cyc_Yystacktype*_TAEC;union Cyc_YYSTYPE*_TAED;union Cyc_YYSTYPE*_TAEE;struct Cyc_Absyn_Tqual _TAEF;unsigned _TAF0;struct Cyc_Yystacktype*_TAF1;struct Cyc_Yystacktype _TAF2;struct Cyc_Yyltype _TAF3;unsigned _TAF4;struct Cyc_Yystacktype*_TAF5;union Cyc_YYSTYPE*_TAF6;union Cyc_YYSTYPE*_TAF7;struct Cyc_Parse_Type_specifier _TAF8;struct Cyc_Yystacktype*_TAF9;struct Cyc_Yystacktype _TAFA;struct Cyc_Yyltype _TAFB;unsigned _TAFC;unsigned _TAFD;struct Cyc_Warn_String_Warn_Warg_struct _TAFE;void**_TAFF;struct Cyc_Yystacktype*_TB00;struct Cyc_Yystacktype _TB01;struct Cyc_Yyltype _TB02;unsigned _TB03;unsigned _TB04;struct _fat_ptr _TB05;int _TB06;struct Cyc_Warn_String_Warn_Warg_struct _TB07;void**_TB08;struct Cyc_Yystacktype*_TB09;struct Cyc_Yystacktype _TB0A;struct Cyc_Yyltype _TB0B;unsigned _TB0C;unsigned _TB0D;struct _fat_ptr _TB0E;struct _tuple0*_TB0F;struct _tuple0 _TB10;struct Cyc_Warn_String_Warn_Warg_struct _TB11;void**_TB12;struct Cyc_Yystacktype*_TB13;struct Cyc_Yystacktype _TB14;struct Cyc_Yyltype _TB15;unsigned _TB16;unsigned _TB17;struct _fat_ptr _TB18;struct _tuple8*_TB19;struct Cyc_Yystacktype*_TB1A;union Cyc_YYSTYPE*_TB1B;union Cyc_YYSTYPE*_TB1C;struct Cyc_Absyn_Tqual _TB1D;unsigned _TB1E;struct Cyc_Yystacktype*_TB1F;struct Cyc_Yystacktype _TB20;struct Cyc_Yyltype _TB21;unsigned _TB22;struct Cyc_Parse_Type_specifier _TB23;struct Cyc_Yystacktype*_TB24;struct Cyc_Yystacktype _TB25;struct Cyc_Yyltype _TB26;unsigned _TB27;unsigned _TB28;struct Cyc_Warn_String_Warn_Warg_struct _TB29;void**_TB2A;struct Cyc_Yystacktype*_TB2B;struct Cyc_Yystacktype _TB2C;struct Cyc_Yyltype _TB2D;unsigned _TB2E;unsigned _TB2F;struct _fat_ptr _TB30;struct _tuple8*_TB31;struct Cyc_Yystacktype*_TB32;union Cyc_YYSTYPE*_TB33;union Cyc_YYSTYPE*_TB34;struct Cyc_Absyn_Tqual _TB35;unsigned _TB36;struct Cyc_Yystacktype*_TB37;struct Cyc_Yystacktype _TB38;struct Cyc_Yyltype _TB39;unsigned _TB3A;struct Cyc_Parse_Type_specifier _TB3B;struct Cyc_Yystacktype*_TB3C;struct Cyc_Yystacktype _TB3D;struct Cyc_Yyltype _TB3E;unsigned _TB3F;unsigned _TB40;struct Cyc_Yystacktype*_TB41;union Cyc_YYSTYPE*_TB42;union Cyc_YYSTYPE*_TB43;struct Cyc_Parse_Abstractdeclarator _TB44;struct Cyc_Warn_String_Warn_Warg_struct _TB45;void**_TB46;struct Cyc_Yystacktype*_TB47;struct Cyc_Yystacktype _TB48;struct Cyc_Yyltype _TB49;unsigned _TB4A;unsigned _TB4B;struct _fat_ptr _TB4C;struct Cyc_Warn_String_Warn_Warg_struct _TB4D;void**_TB4E;struct Cyc_Yystacktype*_TB4F;struct Cyc_Yystacktype _TB50;struct Cyc_Yyltype _TB51;unsigned _TB52;unsigned _TB53;struct _fat_ptr _TB54;struct _tuple8*_TB55;struct Cyc_Yystacktype*_TB56;union Cyc_YYSTYPE*_TB57;union Cyc_YYSTYPE*_TB58;struct Cyc_List_List*_TB59;struct Cyc_List_List*_TB5A;struct Cyc_List_List*_TB5B;struct _fat_ptr*_TB5C;struct Cyc_Yystacktype*_TB5D;union Cyc_YYSTYPE*_TB5E;union Cyc_YYSTYPE*_TB5F;struct Cyc_List_List*_TB60;struct _fat_ptr*_TB61;struct Cyc_Yystacktype*_TB62;union Cyc_YYSTYPE*_TB63;union Cyc_YYSTYPE*_TB64;struct Cyc_Yystacktype*_TB65;union Cyc_YYSTYPE*_TB66;union Cyc_YYSTYPE*_TB67;struct Cyc_Yystacktype*_TB68;struct Cyc_Yystacktype _TB69;struct Cyc_Yystacktype*_TB6A;struct Cyc_Yystacktype _TB6B;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB6C;void*_TB6D;struct Cyc_Yystacktype*_TB6E;struct Cyc_Yystacktype _TB6F;struct Cyc_Yyltype _TB70;unsigned _TB71;unsigned _TB72;struct Cyc_Absyn_Exp*_TB73;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB74;struct Cyc_Yystacktype*_TB75;union Cyc_YYSTYPE*_TB76;union Cyc_YYSTYPE*_TB77;struct Cyc_List_List*_TB78;void*_TB79;struct Cyc_Yystacktype*_TB7A;struct Cyc_Yystacktype _TB7B;struct Cyc_Yyltype _TB7C;unsigned _TB7D;unsigned _TB7E;struct Cyc_Absyn_Exp*_TB7F;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB80;struct Cyc_Yystacktype*_TB81;union Cyc_YYSTYPE*_TB82;union Cyc_YYSTYPE*_TB83;struct Cyc_List_List*_TB84;void*_TB85;struct Cyc_Yystacktype*_TB86;struct Cyc_Yystacktype _TB87;struct Cyc_Yyltype _TB88;unsigned _TB89;unsigned _TB8A;struct Cyc_Absyn_Exp*_TB8B;struct Cyc_Yystacktype*_TB8C;struct Cyc_Yystacktype _TB8D;struct Cyc_Yyltype _TB8E;unsigned _TB8F;unsigned _TB90;struct _tuple0*_TB91;struct _fat_ptr*_TB92;struct Cyc_Yystacktype*_TB93;union Cyc_YYSTYPE*_TB94;union Cyc_YYSTYPE*_TB95;void*_TB96;struct Cyc_Yystacktype*_TB97;struct Cyc_Yystacktype _TB98;struct Cyc_Yyltype _TB99;unsigned _TB9A;unsigned _TB9B;struct Cyc_Absyn_Exp*_TB9C;struct Cyc_Absyn_Vardecl*_TB9D;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TB9E;struct Cyc_Yystacktype*_TB9F;union Cyc_YYSTYPE*_TBA0;union Cyc_YYSTYPE*_TBA1;struct Cyc_Yystacktype*_TBA2;union Cyc_YYSTYPE*_TBA3;union Cyc_YYSTYPE*_TBA4;void*_TBA5;struct Cyc_Yystacktype*_TBA6;struct Cyc_Yystacktype _TBA7;struct Cyc_Yyltype _TBA8;unsigned _TBA9;unsigned _TBAA;struct Cyc_Absyn_Exp*_TBAB;struct Cyc_Yystacktype*_TBAC;union Cyc_YYSTYPE*_TBAD;union Cyc_YYSTYPE*_TBAE;struct _tuple8*_TBAF;struct Cyc_Yystacktype*_TBB0;struct Cyc_Yystacktype _TBB1;struct Cyc_Yyltype _TBB2;unsigned _TBB3;unsigned _TBB4;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TBB5;struct Cyc_Yystacktype*_TBB6;union Cyc_YYSTYPE*_TBB7;union Cyc_YYSTYPE*_TBB8;void*_TBB9;struct Cyc_Yystacktype*_TBBA;struct Cyc_Yystacktype _TBBB;struct Cyc_Yyltype _TBBC;unsigned _TBBD;unsigned _TBBE;struct Cyc_Absyn_Exp*_TBBF;struct Cyc_List_List*_TBC0;struct _tuple34*_TBC1;struct Cyc_Yystacktype*_TBC2;union Cyc_YYSTYPE*_TBC3;union Cyc_YYSTYPE*_TBC4;struct Cyc_List_List*_TBC5;struct _tuple34*_TBC6;struct Cyc_Yystacktype*_TBC7;union Cyc_YYSTYPE*_TBC8;union Cyc_YYSTYPE*_TBC9;struct Cyc_Yystacktype*_TBCA;union Cyc_YYSTYPE*_TBCB;union Cyc_YYSTYPE*_TBCC;struct Cyc_List_List*_TBCD;struct _tuple34*_TBCE;struct Cyc_Yystacktype*_TBCF;union Cyc_YYSTYPE*_TBD0;union Cyc_YYSTYPE*_TBD1;struct Cyc_Yystacktype*_TBD2;union Cyc_YYSTYPE*_TBD3;union Cyc_YYSTYPE*_TBD4;struct Cyc_List_List*_TBD5;struct _tuple34*_TBD6;struct Cyc_Yystacktype*_TBD7;union Cyc_YYSTYPE*_TBD8;union Cyc_YYSTYPE*_TBD9;struct Cyc_Yystacktype*_TBDA;union Cyc_YYSTYPE*_TBDB;union Cyc_YYSTYPE*_TBDC;struct Cyc_Yystacktype*_TBDD;union Cyc_YYSTYPE*_TBDE;union Cyc_YYSTYPE*_TBDF;struct Cyc_Yystacktype*_TBE0;union Cyc_YYSTYPE*_TBE1;union Cyc_YYSTYPE*_TBE2;struct Cyc_List_List*_TBE3;struct Cyc_List_List*_TBE4;struct Cyc_List_List*_TBE5;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBE6;struct _fat_ptr*_TBE7;struct Cyc_Yystacktype*_TBE8;union Cyc_YYSTYPE*_TBE9;union Cyc_YYSTYPE*_TBEA;struct Cyc_List_List*_TBEB;struct Cyc_Yystacktype*_TBEC;union Cyc_YYSTYPE*_TBED;union Cyc_YYSTYPE*_TBEE;struct Cyc_List_List*_TBEF;struct Cyc_Yystacktype*_TBF0;union Cyc_YYSTYPE*_TBF1;union Cyc_YYSTYPE*_TBF2;struct Cyc_Yystacktype*_TBF3;union Cyc_YYSTYPE*_TBF4;union Cyc_YYSTYPE*_TBF5;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_TBF6;struct Cyc_Yystacktype*_TBF7;union Cyc_YYSTYPE*_TBF8;union Cyc_YYSTYPE*_TBF9;void*_TBFA;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBFB;struct _fat_ptr*_TBFC;struct Cyc_Yystacktype*_TBFD;union Cyc_YYSTYPE*_TBFE;union Cyc_YYSTYPE*_TBFF;void*_TC00;struct Cyc_Yystacktype*_TC01;union Cyc_YYSTYPE*_TC02;union Cyc_YYSTYPE*_TC03;struct Cyc_Parse_Type_specifier _TC04;struct Cyc_Yystacktype*_TC05;struct Cyc_Yystacktype _TC06;struct Cyc_Yyltype _TC07;unsigned _TC08;unsigned _TC09;struct Cyc_Yystacktype*_TC0A;struct Cyc_Yystacktype _TC0B;struct Cyc_Yyltype _TC0C;unsigned _TC0D;unsigned _TC0E;struct _fat_ptr _TC0F;struct _fat_ptr _TC10;struct _tuple8*_TC11;struct Cyc_Yystacktype*_TC12;union Cyc_YYSTYPE*_TC13;union Cyc_YYSTYPE*_TC14;struct Cyc_Parse_Type_specifier _TC15;struct Cyc_Yystacktype*_TC16;struct Cyc_Yystacktype _TC17;struct Cyc_Yyltype _TC18;unsigned _TC19;unsigned _TC1A;struct Cyc_Yystacktype*_TC1B;union Cyc_YYSTYPE*_TC1C;union Cyc_YYSTYPE*_TC1D;struct Cyc_Parse_Abstractdeclarator _TC1E;struct _tuple14 _TC1F;struct Cyc_List_List*_TC20;struct Cyc_Yystacktype*_TC21;struct Cyc_Yystacktype _TC22;struct Cyc_Yyltype _TC23;unsigned _TC24;unsigned _TC25;struct _fat_ptr _TC26;struct _fat_ptr _TC27;struct _tuple14 _TC28;struct Cyc_List_List*_TC29;struct Cyc_Yystacktype*_TC2A;struct Cyc_Yystacktype _TC2B;struct Cyc_Yyltype _TC2C;unsigned _TC2D;unsigned _TC2E;struct _fat_ptr _TC2F;struct _fat_ptr _TC30;struct _tuple8*_TC31;struct _tuple14 _TC32;struct _tuple14 _TC33;struct Cyc_Yystacktype*_TC34;union Cyc_YYSTYPE*_TC35;union Cyc_YYSTYPE*_TC36;struct _tuple8*_TC37;struct Cyc_Yystacktype*_TC38;struct Cyc_Yystacktype _TC39;struct Cyc_Yyltype _TC3A;unsigned _TC3B;unsigned _TC3C;void*_TC3D;void*_TC3E;struct Cyc_Yystacktype*_TC3F;union Cyc_YYSTYPE*_TC40;union Cyc_YYSTYPE*_TC41;struct Cyc_List_List*_TC42;void*_TC43;struct Cyc_Yystacktype*_TC44;union Cyc_YYSTYPE*_TC45;union Cyc_YYSTYPE*_TC46;void*_TC47;void*_TC48;struct Cyc_List_List*_TC49;struct Cyc_Yystacktype*_TC4A;union Cyc_YYSTYPE*_TC4B;union Cyc_YYSTYPE*_TC4C;struct Cyc_Yystacktype*_TC4D;union Cyc_YYSTYPE*_TC4E;union Cyc_YYSTYPE*_TC4F;void*_TC50;struct Cyc_Yystacktype*_TC51;union Cyc_YYSTYPE*_TC52;union Cyc_YYSTYPE*_TC53;void*_TC54;struct Cyc_List_List*_TC55;struct Cyc_Yystacktype*_TC56;union Cyc_YYSTYPE*_TC57;union Cyc_YYSTYPE*_TC58;struct Cyc_List_List*_TC59;struct Cyc_Yystacktype*_TC5A;union Cyc_YYSTYPE*_TC5B;union Cyc_YYSTYPE*_TC5C;struct Cyc_Yystacktype*_TC5D;union Cyc_YYSTYPE*_TC5E;union Cyc_YYSTYPE*_TC5F;struct Cyc_Parse_Abstractdeclarator _TC60;struct Cyc_Yystacktype*_TC61;union Cyc_YYSTYPE*_TC62;union Cyc_YYSTYPE*_TC63;struct Cyc_Yystacktype*_TC64;struct Cyc_Yystacktype _TC65;struct Cyc_Parse_Abstractdeclarator _TC66;struct Cyc_Yystacktype*_TC67;union Cyc_YYSTYPE*_TC68;union Cyc_YYSTYPE*_TC69;struct Cyc_List_List*_TC6A;struct Cyc_Yystacktype*_TC6B;union Cyc_YYSTYPE*_TC6C;union Cyc_YYSTYPE*_TC6D;struct Cyc_Parse_Abstractdeclarator _TC6E;struct Cyc_List_List*_TC6F;struct Cyc_Yystacktype*_TC70;struct Cyc_Yystacktype _TC71;struct Cyc_Parse_Abstractdeclarator _TC72;struct Cyc_List_List*_TC73;struct _RegionHandle*_TC74;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC75;struct _RegionHandle*_TC76;struct Cyc_Yystacktype*_TC77;union Cyc_YYSTYPE*_TC78;union Cyc_YYSTYPE*_TC79;struct Cyc_Yystacktype*_TC7A;struct Cyc_Yystacktype _TC7B;struct Cyc_Yyltype _TC7C;unsigned _TC7D;struct Cyc_Parse_Abstractdeclarator _TC7E;struct Cyc_List_List*_TC7F;struct _RegionHandle*_TC80;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC81;struct _RegionHandle*_TC82;struct Cyc_Yystacktype*_TC83;union Cyc_YYSTYPE*_TC84;union Cyc_YYSTYPE*_TC85;struct Cyc_Yystacktype*_TC86;struct Cyc_Yystacktype _TC87;struct Cyc_Yyltype _TC88;unsigned _TC89;struct Cyc_Yystacktype*_TC8A;union Cyc_YYSTYPE*_TC8B;union Cyc_YYSTYPE*_TC8C;struct Cyc_Parse_Abstractdeclarator _TC8D;struct Cyc_Parse_Abstractdeclarator _TC8E;struct Cyc_List_List*_TC8F;struct _RegionHandle*_TC90;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TC91;struct _RegionHandle*_TC92;struct Cyc_Yystacktype*_TC93;union Cyc_YYSTYPE*_TC94;union Cyc_YYSTYPE*_TC95;struct Cyc_Yystacktype*_TC96;union Cyc_YYSTYPE*_TC97;union Cyc_YYSTYPE*_TC98;struct Cyc_Yystacktype*_TC99;struct Cyc_Yystacktype _TC9A;struct Cyc_Yyltype _TC9B;unsigned _TC9C;struct Cyc_Parse_Abstractdeclarator _TC9D;struct Cyc_List_List*_TC9E;struct _RegionHandle*_TC9F;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TCA0;struct _RegionHandle*_TCA1;struct Cyc_Yystacktype*_TCA2;union Cyc_YYSTYPE*_TCA3;union Cyc_YYSTYPE*_TCA4;struct Cyc_Yystacktype*_TCA5;union Cyc_YYSTYPE*_TCA6;union Cyc_YYSTYPE*_TCA7;struct Cyc_Yystacktype*_TCA8;struct Cyc_Yystacktype _TCA9;struct Cyc_Yyltype _TCAA;unsigned _TCAB;struct Cyc_Yystacktype*_TCAC;union Cyc_YYSTYPE*_TCAD;union Cyc_YYSTYPE*_TCAE;struct Cyc_Parse_Abstractdeclarator _TCAF;struct Cyc_Yystacktype*_TCB0;union Cyc_YYSTYPE*_TCB1;union Cyc_YYSTYPE*_TCB2;struct Cyc_Yystacktype*_TCB3;union Cyc_YYSTYPE*_TCB4;union Cyc_YYSTYPE*_TCB5;struct Cyc_Parse_Abstractdeclarator _TCB6;struct Cyc_List_List*_TCB7;struct _RegionHandle*_TCB8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCB9;struct _RegionHandle*_TCBA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCBB;struct _RegionHandle*_TCBC;struct Cyc_Yystacktype*_TCBD;union Cyc_YYSTYPE*_TCBE;union Cyc_YYSTYPE*_TCBF;struct Cyc_Yystacktype*_TCC0;union Cyc_YYSTYPE*_TCC1;union Cyc_YYSTYPE*_TCC2;struct Cyc_Parse_Abstractdeclarator _TCC3;struct Cyc_List_List*_TCC4;struct _RegionHandle*_TCC5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCC6;struct _RegionHandle*_TCC7;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCC8;struct _RegionHandle*_TCC9;struct Cyc_Yystacktype*_TCCA;union Cyc_YYSTYPE*_TCCB;union Cyc_YYSTYPE*_TCCC;struct Cyc_Parse_Abstractdeclarator _TCCD;struct Cyc_List_List*(*_TCCE)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_TCCF)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_TCD0;struct Cyc_Yystacktype _TCD1;struct Cyc_Yyltype _TCD2;unsigned _TCD3;unsigned _TCD4;struct Cyc_Yystacktype*_TCD5;union Cyc_YYSTYPE*_TCD6;union Cyc_YYSTYPE*_TCD7;struct Cyc_List_List*_TCD8;struct Cyc_List_List*_TCD9;struct Cyc_Parse_Abstractdeclarator _TCDA;struct Cyc_List_List*_TCDB;struct _RegionHandle*_TCDC;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_TCDD;struct _RegionHandle*_TCDE;struct Cyc_Yystacktype*_TCDF;struct Cyc_Yystacktype _TCE0;struct Cyc_Yyltype _TCE1;unsigned _TCE2;struct Cyc_Yystacktype*_TCE3;union Cyc_YYSTYPE*_TCE4;union Cyc_YYSTYPE*_TCE5;struct Cyc_Parse_Abstractdeclarator _TCE6;struct Cyc_Parse_Abstractdeclarator _TCE7;struct Cyc_List_List*_TCE8;struct _RegionHandle*_TCE9;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_TCEA;struct _RegionHandle*_TCEB;struct Cyc_Yystacktype*_TCEC;struct Cyc_Yystacktype _TCED;struct Cyc_Yyltype _TCEE;unsigned _TCEF;struct Cyc_Yystacktype*_TCF0;union Cyc_YYSTYPE*_TCF1;union Cyc_YYSTYPE*_TCF2;struct Cyc_Yystacktype*_TCF3;union Cyc_YYSTYPE*_TCF4;union Cyc_YYSTYPE*_TCF5;struct Cyc_Parse_Abstractdeclarator _TCF6;struct _tuple21 _TCF7;struct Cyc_Yystacktype*_TCF8;union Cyc_YYSTYPE*_TCF9;union Cyc_YYSTYPE*_TCFA;struct _tuple21 _TCFB;struct Cyc_Yystacktype*_TCFC;union Cyc_YYSTYPE*_TCFD;union Cyc_YYSTYPE*_TCFE;struct _tuple21 _TCFF;struct Cyc_Yystacktype*_TD00;union Cyc_YYSTYPE*_TD01;union Cyc_YYSTYPE*_TD02;struct _tuple21 _TD03;struct Cyc_Yystacktype*_TD04;union Cyc_YYSTYPE*_TD05;union Cyc_YYSTYPE*_TD06;struct _tuple21 _TD07;struct Cyc_Yystacktype*_TD08;union Cyc_YYSTYPE*_TD09;union Cyc_YYSTYPE*_TD0A;struct Cyc_Yystacktype*_TD0B;union Cyc_YYSTYPE*_TD0C;union Cyc_YYSTYPE*_TD0D;struct _tuple21 _TD0E;struct _tuple21 _TD0F;struct _tuple21 _TD10;struct Cyc_Yystacktype*_TD11;union Cyc_YYSTYPE*_TD12;union Cyc_YYSTYPE*_TD13;struct Cyc_Yystacktype*_TD14;union Cyc_YYSTYPE*_TD15;union Cyc_YYSTYPE*_TD16;struct _tuple21 _TD17;struct _tuple21 _TD18;struct _tuple21 _TD19;struct Cyc_Yystacktype*_TD1A;union Cyc_YYSTYPE*_TD1B;union Cyc_YYSTYPE*_TD1C;struct Cyc_Yystacktype*_TD1D;union Cyc_YYSTYPE*_TD1E;union Cyc_YYSTYPE*_TD1F;struct _tuple21 _TD20;struct _tuple21 _TD21;struct _tuple21 _TD22;struct Cyc_Yystacktype*_TD23;union Cyc_YYSTYPE*_TD24;union Cyc_YYSTYPE*_TD25;struct Cyc_Yystacktype*_TD26;union Cyc_YYSTYPE*_TD27;union Cyc_YYSTYPE*_TD28;struct _tuple21 _TD29;struct _tuple21 _TD2A;struct _tuple21 _TD2B;struct Cyc_Yystacktype*_TD2C;struct Cyc_Yystacktype _TD2D;struct Cyc_Yystacktype*_TD2E;struct Cyc_Yystacktype _TD2F;struct Cyc_Yystacktype*_TD30;struct Cyc_Yystacktype _TD31;struct Cyc_Yystacktype*_TD32;struct Cyc_Yystacktype _TD33;struct Cyc_Yystacktype*_TD34;struct Cyc_Yystacktype _TD35;struct Cyc_Yystacktype*_TD36;struct Cyc_Yystacktype _TD37;struct Cyc_Yystacktype*_TD38;struct Cyc_Yystacktype _TD39;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD3A;struct _fat_ptr*_TD3B;struct Cyc_Yystacktype*_TD3C;union Cyc_YYSTYPE*_TD3D;union Cyc_YYSTYPE*_TD3E;struct Cyc_Yystacktype*_TD3F;union Cyc_YYSTYPE*_TD40;union Cyc_YYSTYPE*_TD41;void*_TD42;struct Cyc_Yystacktype*_TD43;struct Cyc_Yystacktype _TD44;struct Cyc_Yyltype _TD45;unsigned _TD46;unsigned _TD47;struct Cyc_Absyn_Stmt*_TD48;struct Cyc_Yystacktype*_TD49;struct Cyc_Yystacktype _TD4A;struct Cyc_Yyltype _TD4B;unsigned _TD4C;unsigned _TD4D;struct Cyc_Absyn_Stmt*_TD4E;struct Cyc_Yystacktype*_TD4F;union Cyc_YYSTYPE*_TD50;union Cyc_YYSTYPE*_TD51;struct Cyc_Absyn_Exp*_TD52;struct Cyc_Yystacktype*_TD53;struct Cyc_Yystacktype _TD54;struct Cyc_Yyltype _TD55;unsigned _TD56;unsigned _TD57;struct Cyc_Absyn_Stmt*_TD58;struct Cyc_Yystacktype*_TD59;struct Cyc_Yystacktype _TD5A;struct Cyc_Yyltype _TD5B;unsigned _TD5C;unsigned _TD5D;struct Cyc_Absyn_Stmt*_TD5E;struct Cyc_Yystacktype*_TD5F;struct Cyc_Yystacktype _TD60;struct Cyc_Yystacktype*_TD61;struct Cyc_Yystacktype _TD62;struct Cyc_Yyltype _TD63;unsigned _TD64;unsigned _TD65;struct Cyc_Absyn_Stmt*_TD66;struct Cyc_Yystacktype*_TD67;struct Cyc_Yystacktype _TD68;struct Cyc_Yystacktype*_TD69;union Cyc_YYSTYPE*_TD6A;union Cyc_YYSTYPE*_TD6B;struct Cyc_List_List*_TD6C;struct Cyc_Yystacktype*_TD6D;struct Cyc_Yystacktype _TD6E;struct Cyc_Yyltype _TD6F;unsigned _TD70;unsigned _TD71;struct Cyc_Absyn_Stmt*_TD72;struct Cyc_Absyn_Stmt*_TD73;struct Cyc_Yystacktype*_TD74;union Cyc_YYSTYPE*_TD75;union Cyc_YYSTYPE*_TD76;struct Cyc_List_List*_TD77;struct Cyc_Yystacktype*_TD78;union Cyc_YYSTYPE*_TD79;union Cyc_YYSTYPE*_TD7A;struct Cyc_Absyn_Stmt*_TD7B;struct Cyc_Absyn_Stmt*_TD7C;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD7D;struct _fat_ptr*_TD7E;struct Cyc_Yystacktype*_TD7F;union Cyc_YYSTYPE*_TD80;union Cyc_YYSTYPE*_TD81;struct Cyc_Yystacktype*_TD82;union Cyc_YYSTYPE*_TD83;union Cyc_YYSTYPE*_TD84;struct Cyc_List_List*_TD85;struct Cyc_Absyn_Stmt*_TD86;void*_TD87;struct Cyc_Yystacktype*_TD88;struct Cyc_Yystacktype _TD89;struct Cyc_Yyltype _TD8A;unsigned _TD8B;unsigned _TD8C;struct Cyc_Absyn_Stmt*_TD8D;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD8E;struct _fat_ptr*_TD8F;struct Cyc_Yystacktype*_TD90;union Cyc_YYSTYPE*_TD91;union Cyc_YYSTYPE*_TD92;struct Cyc_Yystacktype*_TD93;union Cyc_YYSTYPE*_TD94;union Cyc_YYSTYPE*_TD95;struct Cyc_List_List*_TD96;struct Cyc_Yystacktype*_TD97;union Cyc_YYSTYPE*_TD98;union Cyc_YYSTYPE*_TD99;struct Cyc_Absyn_Stmt*_TD9A;void*_TD9B;struct Cyc_Yystacktype*_TD9C;struct Cyc_Yystacktype _TD9D;struct Cyc_Yyltype _TD9E;unsigned _TD9F;unsigned _TDA0;struct Cyc_Absyn_Stmt*_TDA1;struct Cyc_Yystacktype*_TDA2;struct Cyc_Yystacktype _TDA3;struct Cyc_Yystacktype*_TDA4;union Cyc_YYSTYPE*_TDA5;union Cyc_YYSTYPE*_TDA6;struct Cyc_Absyn_Stmt*_TDA7;struct Cyc_Yystacktype*_TDA8;union Cyc_YYSTYPE*_TDA9;union Cyc_YYSTYPE*_TDAA;struct Cyc_Absyn_Stmt*_TDAB;struct Cyc_Yystacktype*_TDAC;struct Cyc_Yystacktype _TDAD;struct Cyc_Yyltype _TDAE;unsigned _TDAF;unsigned _TDB0;struct Cyc_Absyn_Stmt*_TDB1;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDB2;struct Cyc_Yystacktype*_TDB3;union Cyc_YYSTYPE*_TDB4;union Cyc_YYSTYPE*_TDB5;void*_TDB6;struct Cyc_Yystacktype*_TDB7;struct Cyc_Yystacktype _TDB8;struct Cyc_Yyltype _TDB9;unsigned _TDBA;unsigned _TDBB;struct Cyc_Absyn_Decl*_TDBC;struct Cyc_Absyn_Stmt*_TDBD;struct Cyc_Absyn_Stmt*_TDBE;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDBF;struct Cyc_Yystacktype*_TDC0;union Cyc_YYSTYPE*_TDC1;union Cyc_YYSTYPE*_TDC2;void*_TDC3;struct Cyc_Yystacktype*_TDC4;struct Cyc_Yystacktype _TDC5;struct Cyc_Yyltype _TDC6;unsigned _TDC7;unsigned _TDC8;struct Cyc_Absyn_Decl*_TDC9;struct Cyc_Yystacktype*_TDCA;union Cyc_YYSTYPE*_TDCB;union Cyc_YYSTYPE*_TDCC;struct Cyc_Absyn_Stmt*_TDCD;struct Cyc_Absyn_Stmt*_TDCE;struct Cyc_Yystacktype*_TDCF;union Cyc_YYSTYPE*_TDD0;union Cyc_YYSTYPE*_TDD1;struct Cyc_Absyn_Exp*_TDD2;struct Cyc_Yystacktype*_TDD3;union Cyc_YYSTYPE*_TDD4;union Cyc_YYSTYPE*_TDD5;struct Cyc_Absyn_Stmt*_TDD6;struct Cyc_Absyn_Stmt*_TDD7;struct Cyc_Yystacktype*_TDD8;struct Cyc_Yystacktype _TDD9;struct Cyc_Yyltype _TDDA;unsigned _TDDB;unsigned _TDDC;struct Cyc_Absyn_Stmt*_TDDD;struct Cyc_Yystacktype*_TDDE;union Cyc_YYSTYPE*_TDDF;union Cyc_YYSTYPE*_TDE0;struct Cyc_Absyn_Exp*_TDE1;struct Cyc_Yystacktype*_TDE2;union Cyc_YYSTYPE*_TDE3;union Cyc_YYSTYPE*_TDE4;struct Cyc_Absyn_Stmt*_TDE5;struct Cyc_Yystacktype*_TDE6;union Cyc_YYSTYPE*_TDE7;union Cyc_YYSTYPE*_TDE8;struct Cyc_Absyn_Stmt*_TDE9;struct Cyc_Yystacktype*_TDEA;struct Cyc_Yystacktype _TDEB;struct Cyc_Yyltype _TDEC;unsigned _TDED;unsigned _TDEE;struct Cyc_Absyn_Stmt*_TDEF;struct Cyc_Yystacktype*_TDF0;union Cyc_YYSTYPE*_TDF1;union Cyc_YYSTYPE*_TDF2;struct Cyc_Absyn_Exp*_TDF3;struct Cyc_Yystacktype*_TDF4;union Cyc_YYSTYPE*_TDF5;union Cyc_YYSTYPE*_TDF6;struct Cyc_List_List*_TDF7;struct Cyc_Yystacktype*_TDF8;struct Cyc_Yystacktype _TDF9;struct Cyc_Yyltype _TDFA;unsigned _TDFB;unsigned _TDFC;struct Cyc_Absyn_Stmt*_TDFD;struct Cyc_Yystacktype*_TDFE;union Cyc_YYSTYPE*_TDFF;union Cyc_YYSTYPE*_TE00;struct _tuple0*_TE01;struct Cyc_Yystacktype*_TE02;struct Cyc_Yystacktype _TE03;struct Cyc_Yyltype _TE04;unsigned _TE05;unsigned _TE06;struct Cyc_Absyn_Exp*_TE07;struct Cyc_Yystacktype*_TE08;union Cyc_YYSTYPE*_TE09;union Cyc_YYSTYPE*_TE0A;struct Cyc_List_List*_TE0B;struct Cyc_Yystacktype*_TE0C;struct Cyc_Yystacktype _TE0D;struct Cyc_Yyltype _TE0E;unsigned _TE0F;unsigned _TE10;struct Cyc_Absyn_Stmt*_TE11;struct Cyc_Yystacktype*_TE12;union Cyc_YYSTYPE*_TE13;union Cyc_YYSTYPE*_TE14;struct Cyc_List_List*_TE15;struct Cyc_Yystacktype*_TE16;struct Cyc_Yystacktype _TE17;struct Cyc_Yyltype _TE18;unsigned _TE19;unsigned _TE1A;struct Cyc_Absyn_Exp*_TE1B;struct Cyc_Yystacktype*_TE1C;union Cyc_YYSTYPE*_TE1D;union Cyc_YYSTYPE*_TE1E;struct Cyc_List_List*_TE1F;struct Cyc_Yystacktype*_TE20;struct Cyc_Yystacktype _TE21;struct Cyc_Yyltype _TE22;unsigned _TE23;unsigned _TE24;struct Cyc_Absyn_Stmt*_TE25;struct Cyc_Yystacktype*_TE26;union Cyc_YYSTYPE*_TE27;union Cyc_YYSTYPE*_TE28;struct Cyc_Absyn_Stmt*_TE29;struct Cyc_Yystacktype*_TE2A;union Cyc_YYSTYPE*_TE2B;union Cyc_YYSTYPE*_TE2C;struct Cyc_List_List*_TE2D;struct Cyc_Yystacktype*_TE2E;struct Cyc_Yystacktype _TE2F;struct Cyc_Yyltype _TE30;unsigned _TE31;unsigned _TE32;struct Cyc_Absyn_Stmt*_TE33;struct Cyc_List_List*_TE34;struct Cyc_Absyn_Switch_clause*_TE35;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE36;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE37;void*_TE38;struct Cyc_Yystacktype*_TE39;struct Cyc_Yystacktype _TE3A;struct Cyc_Yyltype _TE3B;unsigned _TE3C;unsigned _TE3D;struct Cyc_Yystacktype*_TE3E;union Cyc_YYSTYPE*_TE3F;union Cyc_YYSTYPE*_TE40;struct Cyc_Yystacktype*_TE41;struct Cyc_Yystacktype _TE42;struct Cyc_Yyltype _TE43;unsigned _TE44;struct Cyc_Yystacktype*_TE45;union Cyc_YYSTYPE*_TE46;union Cyc_YYSTYPE*_TE47;struct Cyc_List_List*_TE48;struct Cyc_Absyn_Switch_clause*_TE49;struct Cyc_Yystacktype*_TE4A;union Cyc_YYSTYPE*_TE4B;union Cyc_YYSTYPE*_TE4C;struct Cyc_Yystacktype*_TE4D;struct Cyc_Yystacktype _TE4E;struct Cyc_Yyltype _TE4F;unsigned _TE50;unsigned _TE51;struct Cyc_Yystacktype*_TE52;struct Cyc_Yystacktype _TE53;struct Cyc_Yyltype _TE54;unsigned _TE55;struct Cyc_Yystacktype*_TE56;union Cyc_YYSTYPE*_TE57;union Cyc_YYSTYPE*_TE58;struct Cyc_List_List*_TE59;struct Cyc_Absyn_Switch_clause*_TE5A;struct Cyc_Yystacktype*_TE5B;union Cyc_YYSTYPE*_TE5C;union Cyc_YYSTYPE*_TE5D;struct Cyc_Yystacktype*_TE5E;union Cyc_YYSTYPE*_TE5F;union Cyc_YYSTYPE*_TE60;struct Cyc_Yystacktype*_TE61;struct Cyc_Yystacktype _TE62;struct Cyc_Yyltype _TE63;unsigned _TE64;struct Cyc_Yystacktype*_TE65;union Cyc_YYSTYPE*_TE66;union Cyc_YYSTYPE*_TE67;struct Cyc_List_List*_TE68;struct Cyc_Absyn_Switch_clause*_TE69;struct Cyc_Yystacktype*_TE6A;union Cyc_YYSTYPE*_TE6B;union Cyc_YYSTYPE*_TE6C;struct Cyc_Yystacktype*_TE6D;union Cyc_YYSTYPE*_TE6E;union Cyc_YYSTYPE*_TE6F;struct Cyc_Yystacktype*_TE70;struct Cyc_Yystacktype _TE71;struct Cyc_Yyltype _TE72;unsigned _TE73;unsigned _TE74;struct Cyc_Yystacktype*_TE75;struct Cyc_Yystacktype _TE76;struct Cyc_Yyltype _TE77;unsigned _TE78;struct Cyc_Yystacktype*_TE79;union Cyc_YYSTYPE*_TE7A;union Cyc_YYSTYPE*_TE7B;struct Cyc_List_List*_TE7C;struct Cyc_Absyn_Switch_clause*_TE7D;struct Cyc_Yystacktype*_TE7E;union Cyc_YYSTYPE*_TE7F;union Cyc_YYSTYPE*_TE80;struct Cyc_Yystacktype*_TE81;union Cyc_YYSTYPE*_TE82;union Cyc_YYSTYPE*_TE83;struct Cyc_Yystacktype*_TE84;union Cyc_YYSTYPE*_TE85;union Cyc_YYSTYPE*_TE86;struct Cyc_Yystacktype*_TE87;struct Cyc_Yystacktype _TE88;struct Cyc_Yyltype _TE89;unsigned _TE8A;struct Cyc_Yystacktype*_TE8B;union Cyc_YYSTYPE*_TE8C;union Cyc_YYSTYPE*_TE8D;struct Cyc_Yystacktype*_TE8E;union Cyc_YYSTYPE*_TE8F;union Cyc_YYSTYPE*_TE90;struct Cyc_Absyn_Exp*_TE91;struct Cyc_Yystacktype*_TE92;union Cyc_YYSTYPE*_TE93;union Cyc_YYSTYPE*_TE94;struct Cyc_Absyn_Stmt*_TE95;struct Cyc_Yystacktype*_TE96;struct Cyc_Yystacktype _TE97;struct Cyc_Yyltype _TE98;unsigned _TE99;unsigned _TE9A;struct Cyc_Absyn_Stmt*_TE9B;struct Cyc_Yystacktype*_TE9C;union Cyc_YYSTYPE*_TE9D;union Cyc_YYSTYPE*_TE9E;struct Cyc_Absyn_Stmt*_TE9F;struct Cyc_Yystacktype*_TEA0;union Cyc_YYSTYPE*_TEA1;union Cyc_YYSTYPE*_TEA2;struct Cyc_Absyn_Exp*_TEA3;struct Cyc_Yystacktype*_TEA4;struct Cyc_Yystacktype _TEA5;struct Cyc_Yyltype _TEA6;unsigned _TEA7;unsigned _TEA8;struct Cyc_Absyn_Stmt*_TEA9;struct Cyc_Yystacktype*_TEAA;union Cyc_YYSTYPE*_TEAB;union Cyc_YYSTYPE*_TEAC;struct Cyc_Absyn_Exp*_TEAD;struct Cyc_Yystacktype*_TEAE;union Cyc_YYSTYPE*_TEAF;union Cyc_YYSTYPE*_TEB0;struct Cyc_Absyn_Exp*_TEB1;struct Cyc_Yystacktype*_TEB2;union Cyc_YYSTYPE*_TEB3;union Cyc_YYSTYPE*_TEB4;struct Cyc_Absyn_Exp*_TEB5;struct Cyc_Yystacktype*_TEB6;union Cyc_YYSTYPE*_TEB7;union Cyc_YYSTYPE*_TEB8;struct Cyc_Absyn_Stmt*_TEB9;struct Cyc_Yystacktype*_TEBA;struct Cyc_Yystacktype _TEBB;struct Cyc_Yyltype _TEBC;unsigned _TEBD;unsigned _TEBE;struct Cyc_Absyn_Stmt*_TEBF;struct Cyc_Absyn_Exp*_TEC0;struct Cyc_Yystacktype*_TEC1;union Cyc_YYSTYPE*_TEC2;union Cyc_YYSTYPE*_TEC3;struct Cyc_Absyn_Exp*_TEC4;struct Cyc_Yystacktype*_TEC5;union Cyc_YYSTYPE*_TEC6;union Cyc_YYSTYPE*_TEC7;struct Cyc_Absyn_Exp*_TEC8;struct Cyc_Yystacktype*_TEC9;union Cyc_YYSTYPE*_TECA;union Cyc_YYSTYPE*_TECB;struct Cyc_Absyn_Stmt*_TECC;struct Cyc_Yystacktype*_TECD;struct Cyc_Yystacktype _TECE;struct Cyc_Yyltype _TECF;unsigned _TED0;unsigned _TED1;struct Cyc_Yystacktype*_TED2;union Cyc_YYSTYPE*_TED3;union Cyc_YYSTYPE*_TED4;struct Cyc_List_List*_TED5;struct Cyc_Absyn_Stmt*_TED6;struct Cyc_Absyn_Stmt*_TED7;struct Cyc_Absyn_Exp*_TED8;struct Cyc_Yystacktype*_TED9;struct Cyc_Yystacktype _TEDA;struct _fat_ptr*_TEDB;struct Cyc_Yystacktype*_TEDC;union Cyc_YYSTYPE*_TEDD;union Cyc_YYSTYPE*_TEDE;struct Cyc_Yystacktype*_TEDF;struct Cyc_Yystacktype _TEE0;struct Cyc_Yyltype _TEE1;unsigned _TEE2;unsigned _TEE3;struct Cyc_Absyn_Stmt*_TEE4;struct Cyc_Yystacktype*_TEE5;struct Cyc_Yystacktype _TEE6;struct Cyc_Yyltype _TEE7;unsigned _TEE8;unsigned _TEE9;struct Cyc_Absyn_Stmt*_TEEA;struct Cyc_Yystacktype*_TEEB;struct Cyc_Yystacktype _TEEC;struct Cyc_Yyltype _TEED;unsigned _TEEE;unsigned _TEEF;struct Cyc_Absyn_Stmt*_TEF0;struct Cyc_Yystacktype*_TEF1;struct Cyc_Yystacktype _TEF2;struct Cyc_Yyltype _TEF3;unsigned _TEF4;unsigned _TEF5;struct Cyc_Absyn_Stmt*_TEF6;struct Cyc_Yystacktype*_TEF7;union Cyc_YYSTYPE*_TEF8;union Cyc_YYSTYPE*_TEF9;struct Cyc_Absyn_Exp*_TEFA;struct Cyc_Yystacktype*_TEFB;struct Cyc_Yystacktype _TEFC;struct Cyc_Yyltype _TEFD;unsigned _TEFE;unsigned _TEFF;struct Cyc_Absyn_Stmt*_TF00;struct Cyc_Yystacktype*_TF01;struct Cyc_Yystacktype _TF02;struct Cyc_Yyltype _TF03;unsigned _TF04;unsigned _TF05;struct Cyc_Absyn_Stmt*_TF06;struct Cyc_Yystacktype*_TF07;struct Cyc_Yystacktype _TF08;struct Cyc_Yyltype _TF09;unsigned _TF0A;unsigned _TF0B;struct Cyc_Absyn_Stmt*_TF0C;struct Cyc_Yystacktype*_TF0D;union Cyc_YYSTYPE*_TF0E;union Cyc_YYSTYPE*_TF0F;struct Cyc_List_List*_TF10;struct Cyc_Yystacktype*_TF11;struct Cyc_Yystacktype _TF12;struct Cyc_Yyltype _TF13;unsigned _TF14;unsigned _TF15;struct Cyc_Absyn_Stmt*_TF16;struct Cyc_Yystacktype*_TF17;struct Cyc_Yystacktype _TF18;struct Cyc_Yystacktype*_TF19;struct Cyc_Yystacktype _TF1A;struct Cyc_Yystacktype*_TF1B;union Cyc_YYSTYPE*_TF1C;union Cyc_YYSTYPE*_TF1D;struct Cyc_Absyn_Pat*_TF1E;struct Cyc_Absyn_Exp*_TF1F;struct Cyc_Yystacktype*_TF20;union Cyc_YYSTYPE*_TF21;union Cyc_YYSTYPE*_TF22;struct Cyc_Absyn_Exp*_TF23;struct Cyc_Yystacktype*_TF24;union Cyc_YYSTYPE*_TF25;union Cyc_YYSTYPE*_TF26;struct Cyc_Absyn_Exp*_TF27;struct Cyc_Yystacktype*_TF28;struct Cyc_Yystacktype _TF29;struct Cyc_Yyltype _TF2A;unsigned _TF2B;unsigned _TF2C;struct Cyc_Absyn_Exp*_TF2D;struct Cyc_Absyn_Pat*_TF2E;struct Cyc_Yystacktype*_TF2F;struct Cyc_Yystacktype _TF30;struct Cyc_Yystacktype*_TF31;union Cyc_YYSTYPE*_TF32;union Cyc_YYSTYPE*_TF33;struct Cyc_Absyn_Pat*_TF34;struct Cyc_Absyn_Exp*_TF35;struct Cyc_Yystacktype*_TF36;union Cyc_YYSTYPE*_TF37;union Cyc_YYSTYPE*_TF38;struct Cyc_Absyn_Exp*_TF39;struct Cyc_Yystacktype*_TF3A;struct Cyc_Yystacktype _TF3B;struct Cyc_Yyltype _TF3C;unsigned _TF3D;unsigned _TF3E;struct Cyc_Absyn_Exp*_TF3F;struct Cyc_Absyn_Pat*_TF40;struct Cyc_Yystacktype*_TF41;struct Cyc_Yystacktype _TF42;struct Cyc_Yystacktype*_TF43;union Cyc_YYSTYPE*_TF44;union Cyc_YYSTYPE*_TF45;struct Cyc_Absyn_Pat*_TF46;struct Cyc_Absyn_Exp*_TF47;struct Cyc_Yystacktype*_TF48;union Cyc_YYSTYPE*_TF49;union Cyc_YYSTYPE*_TF4A;struct Cyc_Absyn_Exp*_TF4B;struct Cyc_Yystacktype*_TF4C;struct Cyc_Yystacktype _TF4D;struct Cyc_Yyltype _TF4E;unsigned _TF4F;unsigned _TF50;struct Cyc_Absyn_Exp*_TF51;struct Cyc_Absyn_Pat*_TF52;struct Cyc_Yystacktype*_TF53;struct Cyc_Yystacktype _TF54;struct Cyc_Yystacktype*_TF55;union Cyc_YYSTYPE*_TF56;union Cyc_YYSTYPE*_TF57;struct Cyc_Absyn_Pat*_TF58;struct Cyc_Absyn_Exp*_TF59;struct Cyc_Yystacktype*_TF5A;union Cyc_YYSTYPE*_TF5B;union Cyc_YYSTYPE*_TF5C;struct Cyc_Absyn_Exp*_TF5D;struct Cyc_Yystacktype*_TF5E;struct Cyc_Yystacktype _TF5F;struct Cyc_Yyltype _TF60;unsigned _TF61;unsigned _TF62;struct Cyc_Absyn_Exp*_TF63;struct Cyc_Absyn_Pat*_TF64;struct Cyc_Yystacktype*_TF65;struct Cyc_Yystacktype _TF66;struct Cyc_Yystacktype*_TF67;union Cyc_YYSTYPE*_TF68;union Cyc_YYSTYPE*_TF69;struct Cyc_Absyn_Pat*_TF6A;struct Cyc_Absyn_Exp*_TF6B;struct Cyc_Yystacktype*_TF6C;union Cyc_YYSTYPE*_TF6D;union Cyc_YYSTYPE*_TF6E;struct Cyc_Absyn_Exp*_TF6F;struct Cyc_Yystacktype*_TF70;struct Cyc_Yystacktype _TF71;struct Cyc_Yyltype _TF72;unsigned _TF73;unsigned _TF74;struct Cyc_Absyn_Exp*_TF75;struct Cyc_Absyn_Pat*_TF76;struct Cyc_Yystacktype*_TF77;struct Cyc_Yystacktype _TF78;struct Cyc_Yystacktype*_TF79;union Cyc_YYSTYPE*_TF7A;union Cyc_YYSTYPE*_TF7B;struct Cyc_Absyn_Pat*_TF7C;struct Cyc_Absyn_Exp*_TF7D;struct Cyc_Yystacktype*_TF7E;union Cyc_YYSTYPE*_TF7F;union Cyc_YYSTYPE*_TF80;struct Cyc_Absyn_Exp*_TF81;struct Cyc_Yystacktype*_TF82;struct Cyc_Yystacktype _TF83;struct Cyc_Yyltype _TF84;unsigned _TF85;unsigned _TF86;struct Cyc_Absyn_Exp*_TF87;struct Cyc_Absyn_Pat*_TF88;struct Cyc_Yystacktype*_TF89;struct Cyc_Yystacktype _TF8A;struct Cyc_Yystacktype*_TF8B;union Cyc_YYSTYPE*_TF8C;union Cyc_YYSTYPE*_TF8D;struct Cyc_Absyn_Exp*(*_TF8E)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TF8F;union Cyc_YYSTYPE*_TF90;union Cyc_YYSTYPE*_TF91;struct Cyc_Absyn_Pat*_TF92;struct Cyc_Absyn_Exp*_TF93;struct Cyc_Yystacktype*_TF94;union Cyc_YYSTYPE*_TF95;union Cyc_YYSTYPE*_TF96;struct Cyc_Absyn_Exp*_TF97;struct Cyc_Yystacktype*_TF98;struct Cyc_Yystacktype _TF99;struct Cyc_Yyltype _TF9A;unsigned _TF9B;unsigned _TF9C;struct Cyc_Absyn_Exp*_TF9D;struct Cyc_Absyn_Pat*_TF9E;struct Cyc_Yystacktype*_TF9F;struct Cyc_Yystacktype _TFA0;struct Cyc_Yystacktype*_TFA1;union Cyc_YYSTYPE*_TFA2;union Cyc_YYSTYPE*_TFA3;struct Cyc_Absyn_Exp*(*_TFA4)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TFA5;union Cyc_YYSTYPE*_TFA6;union Cyc_YYSTYPE*_TFA7;struct Cyc_Absyn_Pat*_TFA8;struct Cyc_Absyn_Exp*_TFA9;struct Cyc_Yystacktype*_TFAA;union Cyc_YYSTYPE*_TFAB;union Cyc_YYSTYPE*_TFAC;struct Cyc_Absyn_Exp*_TFAD;struct Cyc_Yystacktype*_TFAE;struct Cyc_Yystacktype _TFAF;struct Cyc_Yyltype _TFB0;unsigned _TFB1;unsigned _TFB2;struct Cyc_Absyn_Exp*_TFB3;struct Cyc_Absyn_Pat*_TFB4;struct Cyc_Yystacktype*_TFB5;struct Cyc_Yystacktype _TFB6;struct Cyc_Yystacktype*_TFB7;union Cyc_YYSTYPE*_TFB8;union Cyc_YYSTYPE*_TFB9;struct Cyc_Absyn_Pat*_TFBA;struct Cyc_Absyn_Exp*_TFBB;struct Cyc_Yystacktype*_TFBC;union Cyc_YYSTYPE*_TFBD;union Cyc_YYSTYPE*_TFBE;struct Cyc_Absyn_Exp*_TFBF;struct Cyc_Yystacktype*_TFC0;struct Cyc_Yystacktype _TFC1;struct Cyc_Yyltype _TFC2;unsigned _TFC3;unsigned _TFC4;struct Cyc_Absyn_Exp*_TFC5;struct Cyc_Absyn_Pat*_TFC6;struct Cyc_Yystacktype*_TFC7;union Cyc_YYSTYPE*_TFC8;union Cyc_YYSTYPE*_TFC9;struct Cyc_Absyn_Pat*_TFCA;struct Cyc_Absyn_Exp*_TFCB;struct Cyc_Yystacktype*_TFCC;union Cyc_YYSTYPE*_TFCD;union Cyc_YYSTYPE*_TFCE;struct Cyc_Absyn_Exp*_TFCF;struct Cyc_Yystacktype*_TFD0;struct Cyc_Yystacktype _TFD1;struct Cyc_Yyltype _TFD2;unsigned _TFD3;unsigned _TFD4;struct Cyc_Absyn_Exp*_TFD5;struct Cyc_Absyn_Pat*_TFD6;struct Cyc_Yystacktype*_TFD7;struct Cyc_Yystacktype _TFD8;struct Cyc_Yystacktype*_TFD9;union Cyc_YYSTYPE*_TFDA;union Cyc_YYSTYPE*_TFDB;enum Cyc_Absyn_Primop _TFDC;struct Cyc_Yystacktype*_TFDD;union Cyc_YYSTYPE*_TFDE;union Cyc_YYSTYPE*_TFDF;struct Cyc_Absyn_Pat*_TFE0;struct Cyc_Absyn_Exp*_TFE1;struct Cyc_Yystacktype*_TFE2;union Cyc_YYSTYPE*_TFE3;union Cyc_YYSTYPE*_TFE4;struct Cyc_Absyn_Exp*_TFE5;struct Cyc_Yystacktype*_TFE6;struct Cyc_Yystacktype _TFE7;struct Cyc_Yyltype _TFE8;unsigned _TFE9;unsigned _TFEA;struct Cyc_Absyn_Exp*_TFEB;struct Cyc_Absyn_Pat*_TFEC;struct Cyc_Yystacktype*_TFED;struct Cyc_Yystacktype _TFEE;struct Cyc_Yystacktype*_TFEF;union Cyc_YYSTYPE*_TFF0;union Cyc_YYSTYPE*_TFF1;enum Cyc_Absyn_Primop _TFF2;struct Cyc_Yystacktype*_TFF3;union Cyc_YYSTYPE*_TFF4;union Cyc_YYSTYPE*_TFF5;struct Cyc_Absyn_Pat*_TFF6;struct Cyc_Absyn_Exp*_TFF7;struct Cyc_Yystacktype*_TFF8;union Cyc_YYSTYPE*_TFF9;union Cyc_YYSTYPE*_TFFA;struct Cyc_Absyn_Exp*_TFFB;struct Cyc_Yystacktype*_TFFC;struct Cyc_Yystacktype _TFFD;struct Cyc_Yyltype _TFFE;unsigned _TFFF;unsigned _T1000;struct Cyc_Absyn_Exp*_T1001;struct Cyc_Absyn_Pat*_T1002;struct Cyc_Yystacktype*_T1003;struct Cyc_Yystacktype _T1004;struct Cyc_Yystacktype*_T1005;union Cyc_YYSTYPE*_T1006;union Cyc_YYSTYPE*_T1007;struct _tuple8*_T1008;struct Cyc_Yystacktype*_T1009;struct Cyc_Yystacktype _T100A;struct Cyc_Yyltype _T100B;unsigned _T100C;unsigned _T100D;void*_T100E;struct Cyc_Yystacktype*_T100F;union Cyc_YYSTYPE*_T1010;union Cyc_YYSTYPE*_T1011;struct Cyc_Absyn_Exp*_T1012;struct Cyc_Yystacktype*_T1013;struct Cyc_Yystacktype _T1014;struct Cyc_Yyltype _T1015;unsigned _T1016;unsigned _T1017;struct Cyc_Absyn_Exp*_T1018;struct Cyc_Absyn_Pat*_T1019;struct Cyc_Yystacktype*_T101A;struct Cyc_Yystacktype _T101B;struct Cyc_Yystacktype*_T101C;union Cyc_YYSTYPE*_T101D;union Cyc_YYSTYPE*_T101E;enum Cyc_Absyn_Primop _T101F;struct Cyc_Yystacktype*_T1020;union Cyc_YYSTYPE*_T1021;union Cyc_YYSTYPE*_T1022;struct Cyc_Absyn_Exp*_T1023;struct Cyc_Yystacktype*_T1024;struct Cyc_Yystacktype _T1025;struct Cyc_Yyltype _T1026;unsigned _T1027;unsigned _T1028;struct Cyc_Absyn_Exp*_T1029;struct Cyc_Absyn_Pat*_T102A;struct Cyc_Yystacktype*_T102B;union Cyc_YYSTYPE*_T102C;union Cyc_YYSTYPE*_T102D;struct _tuple8*_T102E;struct Cyc_Yystacktype*_T102F;struct Cyc_Yystacktype _T1030;struct Cyc_Yyltype _T1031;unsigned _T1032;unsigned _T1033;void*_T1034;struct Cyc_Yystacktype*_T1035;struct Cyc_Yystacktype _T1036;struct Cyc_Yyltype _T1037;unsigned _T1038;unsigned _T1039;struct Cyc_Absyn_Exp*_T103A;struct Cyc_Absyn_Pat*_T103B;struct Cyc_Yystacktype*_T103C;union Cyc_YYSTYPE*_T103D;union Cyc_YYSTYPE*_T103E;struct Cyc_Absyn_Exp*_T103F;struct Cyc_Yystacktype*_T1040;struct Cyc_Yystacktype _T1041;struct Cyc_Yyltype _T1042;unsigned _T1043;unsigned _T1044;struct Cyc_Absyn_Exp*_T1045;struct Cyc_Absyn_Pat*_T1046;struct Cyc_Yystacktype*_T1047;union Cyc_YYSTYPE*_T1048;union Cyc_YYSTYPE*_T1049;struct _tuple8*_T104A;struct _tuple8 _T104B;void*_T104C;struct Cyc_Yystacktype*_T104D;union Cyc_YYSTYPE*_T104E;union Cyc_YYSTYPE*_T104F;struct Cyc_List_List*_T1050;struct Cyc_List_List*_T1051;struct Cyc_Yystacktype*_T1052;struct Cyc_Yystacktype _T1053;struct Cyc_Yyltype _T1054;unsigned _T1055;unsigned _T1056;struct Cyc_Absyn_Exp*_T1057;struct Cyc_Absyn_Pat*_T1058;struct Cyc_Yystacktype*_T1059;struct Cyc_Yystacktype _T105A;struct Cyc_Yystacktype*_T105B;struct Cyc_Yystacktype _T105C;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T105D;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T105E;void*_T105F;struct Cyc_Yystacktype*_T1060;struct Cyc_Yystacktype _T1061;struct Cyc_Yyltype _T1062;unsigned _T1063;unsigned _T1064;struct Cyc_Absyn_Pat*_T1065;struct Cyc_Yystacktype*_T1066;union Cyc_YYSTYPE*_T1067;union Cyc_YYSTYPE*_T1068;struct Cyc_Absyn_Exp*_T1069;struct Cyc_Absyn_Pat*_T106A;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T106B;struct Cyc_Yystacktype*_T106C;union Cyc_YYSTYPE*_T106D;union Cyc_YYSTYPE*_T106E;void*_T106F;struct Cyc_Yystacktype*_T1070;struct Cyc_Yystacktype _T1071;struct Cyc_Yyltype _T1072;unsigned _T1073;unsigned _T1074;struct Cyc_Absyn_Pat*_T1075;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1076;struct Cyc_Yystacktype*_T1077;union Cyc_YYSTYPE*_T1078;union Cyc_YYSTYPE*_T1079;void*_T107A;struct Cyc_Yystacktype*_T107B;struct Cyc_Yystacktype _T107C;struct Cyc_Yyltype _T107D;unsigned _T107E;unsigned _T107F;struct Cyc_Absyn_Pat*_T1080;struct Cyc_Yystacktype*_T1081;union Cyc_YYSTYPE*_T1082;union Cyc_YYSTYPE*_T1083;struct Cyc_Absyn_Exp*_T1084;int*_T1085;int _T1086;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1087;union Cyc_Absyn_Cnst _T1088;struct _union_Cnst_LongLong_c _T1089;unsigned _T108A;union Cyc_Absyn_Cnst _T108B;struct _union_Cnst_Char_c _T108C;struct _tuple3 _T108D;union Cyc_Absyn_Cnst _T108E;struct _union_Cnst_Char_c _T108F;struct _tuple3 _T1090;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T1091;void*_T1092;struct Cyc_Absyn_Exp*_T1093;unsigned _T1094;struct Cyc_Absyn_Pat*_T1095;union Cyc_Absyn_Cnst _T1096;struct _union_Cnst_Short_c _T1097;struct _tuple4 _T1098;union Cyc_Absyn_Cnst _T1099;struct _union_Cnst_Short_c _T109A;struct _tuple4 _T109B;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T109C;short _T109D;void*_T109E;struct Cyc_Absyn_Exp*_T109F;unsigned _T10A0;struct Cyc_Absyn_Pat*_T10A1;union Cyc_Absyn_Cnst _T10A2;struct _union_Cnst_Int_c _T10A3;struct _tuple5 _T10A4;union Cyc_Absyn_Cnst _T10A5;struct _union_Cnst_Int_c _T10A6;struct _tuple5 _T10A7;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T10A8;void*_T10A9;struct Cyc_Absyn_Exp*_T10AA;unsigned _T10AB;struct Cyc_Absyn_Pat*_T10AC;union Cyc_Absyn_Cnst _T10AD;struct _union_Cnst_Float_c _T10AE;struct _tuple7 _T10AF;union Cyc_Absyn_Cnst _T10B0;struct _union_Cnst_Float_c _T10B1;struct _tuple7 _T10B2;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T10B3;void*_T10B4;struct Cyc_Absyn_Exp*_T10B5;unsigned _T10B6;struct Cyc_Absyn_Pat*_T10B7;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10B8;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10B9;void*_T10BA;struct Cyc_Absyn_Exp*_T10BB;unsigned _T10BC;struct Cyc_Absyn_Pat*_T10BD;struct Cyc_Yystacktype*_T10BE;struct Cyc_Yystacktype _T10BF;struct Cyc_Yyltype _T10C0;unsigned _T10C1;unsigned _T10C2;struct _fat_ptr _T10C3;struct _fat_ptr _T10C4;struct Cyc_Yystacktype*_T10C5;struct Cyc_Yystacktype _T10C6;struct Cyc_Yyltype _T10C7;unsigned _T10C8;unsigned _T10C9;struct _fat_ptr _T10CA;struct _fat_ptr _T10CB;struct Cyc_Yystacktype*_T10CC;struct Cyc_Yystacktype _T10CD;struct Cyc_Yyltype _T10CE;unsigned _T10CF;unsigned _T10D0;struct _fat_ptr _T10D1;struct _fat_ptr _T10D2;struct Cyc_Yystacktype*_T10D3;union Cyc_YYSTYPE*_T10D4;union Cyc_YYSTYPE*_T10D5;struct _fat_ptr _T10D6;struct _fat_ptr _T10D7;int _T10D8;struct Cyc_Yystacktype*_T10D9;struct Cyc_Yystacktype _T10DA;struct Cyc_Yyltype _T10DB;unsigned _T10DC;unsigned _T10DD;struct _fat_ptr _T10DE;struct _fat_ptr _T10DF;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T10E0;struct Cyc_Yystacktype*_T10E1;struct Cyc_Yystacktype _T10E2;struct Cyc_Yyltype _T10E3;unsigned _T10E4;unsigned _T10E5;struct _tuple0*_T10E6;struct _fat_ptr*_T10E7;struct Cyc_Yystacktype*_T10E8;union Cyc_YYSTYPE*_T10E9;union Cyc_YYSTYPE*_T10EA;void*_T10EB;struct Cyc_Yystacktype*_T10EC;union Cyc_YYSTYPE*_T10ED;union Cyc_YYSTYPE*_T10EE;void*_T10EF;struct Cyc_Yystacktype*_T10F0;struct Cyc_Yystacktype _T10F1;struct Cyc_Yyltype _T10F2;unsigned _T10F3;unsigned _T10F4;struct Cyc_Absyn_Pat*_T10F5;struct Cyc_Yystacktype*_T10F6;union Cyc_YYSTYPE*_T10F7;union Cyc_YYSTYPE*_T10F8;struct _fat_ptr _T10F9;struct _fat_ptr _T10FA;int _T10FB;struct Cyc_Yystacktype*_T10FC;struct Cyc_Yystacktype _T10FD;struct Cyc_Yyltype _T10FE;unsigned _T10FF;unsigned _T1100;struct _fat_ptr _T1101;struct _fat_ptr _T1102;struct Cyc_Yystacktype*_T1103;struct Cyc_Yystacktype _T1104;struct Cyc_Yyltype _T1105;unsigned _T1106;struct Cyc_Yystacktype*_T1107;union Cyc_YYSTYPE*_T1108;union Cyc_YYSTYPE*_T1109;struct _fat_ptr _T110A;unsigned _T110B;struct Cyc_Absyn_Tvar*_T110C;struct _fat_ptr*_T110D;struct Cyc_Yystacktype*_T110E;union Cyc_YYSTYPE*_T110F;union Cyc_YYSTYPE*_T1110;struct Cyc_Absyn_Tvar*_T1111;struct Cyc_Absyn_Tvar*_T1112;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1113;struct Cyc_Absyn_Kind*_T1114;struct Cyc_Absyn_Tvar*_T1115;struct Cyc_Yystacktype*_T1116;struct Cyc_Yystacktype _T1117;struct Cyc_Yyltype _T1118;unsigned _T1119;unsigned _T111A;struct _tuple0*_T111B;struct _fat_ptr*_T111C;struct Cyc_Yystacktype*_T111D;union Cyc_YYSTYPE*_T111E;union Cyc_YYSTYPE*_T111F;struct Cyc_Yystacktype*_T1120;union Cyc_YYSTYPE*_T1121;union Cyc_YYSTYPE*_T1122;struct _tuple8*_T1123;struct Cyc_Yystacktype*_T1124;struct Cyc_Yystacktype _T1125;struct Cyc_Yyltype _T1126;unsigned _T1127;unsigned _T1128;void*_T1129;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T112A;void*_T112B;unsigned _T112C;struct Cyc_Absyn_Pat*_T112D;struct Cyc_Yystacktype*_T112E;union Cyc_YYSTYPE*_T112F;union Cyc_YYSTYPE*_T1130;struct _fat_ptr _T1131;struct _fat_ptr _T1132;int _T1133;struct Cyc_Yystacktype*_T1134;struct Cyc_Yystacktype _T1135;struct Cyc_Yyltype _T1136;unsigned _T1137;unsigned _T1138;struct _fat_ptr _T1139;struct _fat_ptr _T113A;struct Cyc_Yystacktype*_T113B;struct Cyc_Yystacktype _T113C;struct Cyc_Yyltype _T113D;unsigned _T113E;struct Cyc_Yystacktype*_T113F;union Cyc_YYSTYPE*_T1140;union Cyc_YYSTYPE*_T1141;struct _fat_ptr _T1142;unsigned _T1143;struct Cyc_Absyn_Tvar*_T1144;struct _fat_ptr*_T1145;struct Cyc_Yystacktype*_T1146;union Cyc_YYSTYPE*_T1147;union Cyc_YYSTYPE*_T1148;struct Cyc_Absyn_Tvar*_T1149;struct Cyc_Absyn_Tvar*_T114A;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T114B;struct Cyc_Absyn_Kind*_T114C;struct Cyc_Absyn_Tvar*_T114D;struct Cyc_Yystacktype*_T114E;struct Cyc_Yystacktype _T114F;struct Cyc_Yyltype _T1150;unsigned _T1151;unsigned _T1152;struct _tuple0*_T1153;struct _fat_ptr*_T1154;struct Cyc_Yystacktype*_T1155;union Cyc_YYSTYPE*_T1156;union Cyc_YYSTYPE*_T1157;struct Cyc_Yystacktype*_T1158;union Cyc_YYSTYPE*_T1159;union Cyc_YYSTYPE*_T115A;struct _tuple8*_T115B;struct Cyc_Yystacktype*_T115C;struct Cyc_Yystacktype _T115D;struct Cyc_Yyltype _T115E;unsigned _T115F;unsigned _T1160;void*_T1161;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1162;void*_T1163;unsigned _T1164;struct Cyc_Absyn_Pat*_T1165;struct Cyc_Yystacktype*_T1166;union Cyc_YYSTYPE*_T1167;union Cyc_YYSTYPE*_T1168;struct _tuple23*_T1169;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T116A;void*_T116B;struct Cyc_Yystacktype*_T116C;struct Cyc_Yystacktype _T116D;struct Cyc_Yyltype _T116E;unsigned _T116F;unsigned _T1170;struct Cyc_Absyn_Pat*_T1171;struct Cyc_Yystacktype*_T1172;union Cyc_YYSTYPE*_T1173;union Cyc_YYSTYPE*_T1174;struct _tuple23*_T1175;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T1176;struct Cyc_Yystacktype*_T1177;union Cyc_YYSTYPE*_T1178;union Cyc_YYSTYPE*_T1179;void*_T117A;struct Cyc_Yystacktype*_T117B;struct Cyc_Yystacktype _T117C;struct Cyc_Yyltype _T117D;unsigned _T117E;unsigned _T117F;struct Cyc_Absyn_Pat*_T1180;struct Cyc_Yystacktype*_T1181;union Cyc_YYSTYPE*_T1182;union Cyc_YYSTYPE*_T1183;struct _tuple23*_T1184;struct Cyc_List_List*(*_T1185)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T1186)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T1187;struct Cyc_Yystacktype _T1188;struct Cyc_Yyltype _T1189;unsigned _T118A;unsigned _T118B;struct Cyc_Yystacktype*_T118C;union Cyc_YYSTYPE*_T118D;union Cyc_YYSTYPE*_T118E;struct Cyc_List_List*_T118F;struct Cyc_Yystacktype*_T1190;union Cyc_YYSTYPE*_T1191;union Cyc_YYSTYPE*_T1192;struct _tuple0*_T1193;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1194;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1195;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T1196;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1197;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1198;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1199;void*_T119A;struct Cyc_Yystacktype*_T119B;struct Cyc_Yystacktype _T119C;struct Cyc_Yyltype _T119D;unsigned _T119E;unsigned _T119F;struct Cyc_Absyn_Pat*_T11A0;struct Cyc_Yystacktype*_T11A1;union Cyc_YYSTYPE*_T11A2;union Cyc_YYSTYPE*_T11A3;struct _tuple23*_T11A4;struct Cyc_List_List*(*_T11A5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T11A6)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T11A7;struct Cyc_Yystacktype _T11A8;struct Cyc_Yyltype _T11A9;unsigned _T11AA;unsigned _T11AB;struct Cyc_Yystacktype*_T11AC;union Cyc_YYSTYPE*_T11AD;union Cyc_YYSTYPE*_T11AE;struct Cyc_List_List*_T11AF;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T11B0;void*_T11B1;struct Cyc_Yystacktype*_T11B2;struct Cyc_Yystacktype _T11B3;struct Cyc_Yyltype _T11B4;unsigned _T11B5;unsigned _T11B6;struct Cyc_Absyn_Pat*_T11B7;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11B8;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11B9;struct Cyc_Yystacktype*_T11BA;union Cyc_YYSTYPE*_T11BB;union Cyc_YYSTYPE*_T11BC;void*_T11BD;struct Cyc_Yystacktype*_T11BE;struct Cyc_Yystacktype _T11BF;struct Cyc_Yyltype _T11C0;unsigned _T11C1;unsigned _T11C2;void*_T11C3;struct Cyc_Yystacktype*_T11C4;struct Cyc_Yystacktype _T11C5;struct Cyc_Yyltype _T11C6;unsigned _T11C7;unsigned _T11C8;struct Cyc_Absyn_Pat*_T11C9;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11CA;struct Cyc_Yystacktype*_T11CB;struct Cyc_Yystacktype _T11CC;struct Cyc_Yyltype _T11CD;unsigned _T11CE;unsigned _T11CF;struct _tuple0*_T11D0;struct _fat_ptr*_T11D1;struct Cyc_Yystacktype*_T11D2;union Cyc_YYSTYPE*_T11D3;union Cyc_YYSTYPE*_T11D4;void*_T11D5;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11D6;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11D7;void*_T11D8;struct Cyc_Yystacktype*_T11D9;struct Cyc_Yystacktype _T11DA;struct Cyc_Yyltype _T11DB;unsigned _T11DC;unsigned _T11DD;void*_T11DE;struct Cyc_Yystacktype*_T11DF;struct Cyc_Yystacktype _T11E0;struct Cyc_Yyltype _T11E1;unsigned _T11E2;unsigned _T11E3;struct Cyc_Absyn_Pat*_T11E4;struct Cyc_Yystacktype*_T11E5;union Cyc_YYSTYPE*_T11E6;union Cyc_YYSTYPE*_T11E7;struct _fat_ptr _T11E8;struct _fat_ptr _T11E9;int _T11EA;struct Cyc_Yystacktype*_T11EB;struct Cyc_Yystacktype _T11EC;struct Cyc_Yyltype _T11ED;unsigned _T11EE;unsigned _T11EF;struct _fat_ptr _T11F0;struct _fat_ptr _T11F1;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11F2;struct Cyc_Yystacktype*_T11F3;struct Cyc_Yystacktype _T11F4;struct Cyc_Yyltype _T11F5;unsigned _T11F6;unsigned _T11F7;struct _tuple0*_T11F8;struct _fat_ptr*_T11F9;struct Cyc_Yystacktype*_T11FA;union Cyc_YYSTYPE*_T11FB;union Cyc_YYSTYPE*_T11FC;void*_T11FD;struct Cyc_Yystacktype*_T11FE;union Cyc_YYSTYPE*_T11FF;union Cyc_YYSTYPE*_T1200;void*_T1201;struct Cyc_Yystacktype*_T1202;struct Cyc_Yystacktype _T1203;struct Cyc_Yyltype _T1204;unsigned _T1205;unsigned _T1206;struct Cyc_Absyn_Pat*_T1207;struct Cyc_Yystacktype*_T1208;union Cyc_YYSTYPE*_T1209;union Cyc_YYSTYPE*_T120A;struct _fat_ptr _T120B;struct Cyc_Absyn_Kind*_T120C;struct Cyc_Absyn_Kind*_T120D;void*_T120E;struct Cyc_Yystacktype*_T120F;struct Cyc_Yystacktype _T1210;struct Cyc_Yyltype _T1211;unsigned _T1212;unsigned _T1213;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1214;struct Cyc_Yystacktype*_T1215;struct Cyc_Yystacktype _T1216;struct Cyc_Yyltype _T1217;unsigned _T1218;unsigned _T1219;void*_T121A;struct Cyc_Yystacktype*_T121B;struct Cyc_Yystacktype _T121C;struct Cyc_Yyltype _T121D;unsigned _T121E;unsigned _T121F;struct _tuple0*_T1220;struct _fat_ptr*_T1221;struct Cyc_Yystacktype*_T1222;union Cyc_YYSTYPE*_T1223;union Cyc_YYSTYPE*_T1224;void*_T1225;void*_T1226;struct Cyc_Yystacktype*_T1227;struct Cyc_Yystacktype _T1228;struct Cyc_Yyltype _T1229;unsigned _T122A;unsigned _T122B;struct Cyc_Absyn_Pat*_T122C;struct Cyc_Absyn_Kind*_T122D;struct Cyc_Absyn_Kind*_T122E;void*_T122F;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1230;struct Cyc_Yystacktype*_T1231;struct Cyc_Yystacktype _T1232;struct Cyc_Yyltype _T1233;unsigned _T1234;unsigned _T1235;struct _tuple0*_T1236;struct _fat_ptr*_T1237;struct Cyc_Yystacktype*_T1238;union Cyc_YYSTYPE*_T1239;union Cyc_YYSTYPE*_T123A;void*_T123B;void*_T123C;void*_T123D;struct Cyc_Yystacktype*_T123E;struct Cyc_Yystacktype _T123F;struct Cyc_Yyltype _T1240;unsigned _T1241;unsigned _T1242;struct Cyc_Absyn_Pat*_T1243;struct _tuple23*_T1244;struct Cyc_Yystacktype*_T1245;union Cyc_YYSTYPE*_T1246;union Cyc_YYSTYPE*_T1247;struct Cyc_List_List*_T1248;struct _tuple23*_T1249;struct Cyc_Yystacktype*_T124A;union Cyc_YYSTYPE*_T124B;union Cyc_YYSTYPE*_T124C;struct Cyc_List_List*_T124D;struct _tuple23*_T124E;struct Cyc_List_List*_T124F;struct Cyc_Yystacktype*_T1250;union Cyc_YYSTYPE*_T1251;union Cyc_YYSTYPE*_T1252;struct Cyc_List_List*_T1253;struct Cyc_Yystacktype*_T1254;union Cyc_YYSTYPE*_T1255;union Cyc_YYSTYPE*_T1256;struct Cyc_Yystacktype*_T1257;union Cyc_YYSTYPE*_T1258;union Cyc_YYSTYPE*_T1259;struct _tuple24*_T125A;struct Cyc_Yystacktype*_T125B;union Cyc_YYSTYPE*_T125C;union Cyc_YYSTYPE*_T125D;struct _tuple24*_T125E;struct Cyc_Yystacktype*_T125F;union Cyc_YYSTYPE*_T1260;union Cyc_YYSTYPE*_T1261;struct Cyc_Yystacktype*_T1262;union Cyc_YYSTYPE*_T1263;union Cyc_YYSTYPE*_T1264;struct _tuple23*_T1265;struct Cyc_Yystacktype*_T1266;union Cyc_YYSTYPE*_T1267;union Cyc_YYSTYPE*_T1268;struct Cyc_List_List*_T1269;struct _tuple23*_T126A;struct Cyc_Yystacktype*_T126B;union Cyc_YYSTYPE*_T126C;union Cyc_YYSTYPE*_T126D;struct Cyc_List_List*_T126E;struct _tuple23*_T126F;struct Cyc_List_List*_T1270;struct Cyc_Yystacktype*_T1271;union Cyc_YYSTYPE*_T1272;union Cyc_YYSTYPE*_T1273;struct Cyc_List_List*_T1274;struct Cyc_Yystacktype*_T1275;union Cyc_YYSTYPE*_T1276;union Cyc_YYSTYPE*_T1277;struct Cyc_Yystacktype*_T1278;union Cyc_YYSTYPE*_T1279;union Cyc_YYSTYPE*_T127A;struct Cyc_Yystacktype*_T127B;struct Cyc_Yystacktype _T127C;struct Cyc_Yystacktype*_T127D;union Cyc_YYSTYPE*_T127E;union Cyc_YYSTYPE*_T127F;struct Cyc_Absyn_Exp*_T1280;struct Cyc_Yystacktype*_T1281;union Cyc_YYSTYPE*_T1282;union Cyc_YYSTYPE*_T1283;struct Cyc_Absyn_Exp*_T1284;struct Cyc_Yystacktype*_T1285;struct Cyc_Yystacktype _T1286;struct Cyc_Yyltype _T1287;unsigned _T1288;unsigned _T1289;struct Cyc_Absyn_Exp*_T128A;struct Cyc_Yystacktype*_T128B;struct Cyc_Yystacktype _T128C;struct Cyc_Yystacktype*_T128D;union Cyc_YYSTYPE*_T128E;union Cyc_YYSTYPE*_T128F;struct Cyc_Absyn_Exp*_T1290;struct Cyc_Yystacktype*_T1291;union Cyc_YYSTYPE*_T1292;union Cyc_YYSTYPE*_T1293;struct Cyc_Core_Opt*_T1294;struct Cyc_Yystacktype*_T1295;union Cyc_YYSTYPE*_T1296;union Cyc_YYSTYPE*_T1297;struct Cyc_Absyn_Exp*_T1298;struct Cyc_Yystacktype*_T1299;struct Cyc_Yystacktype _T129A;struct Cyc_Yyltype _T129B;unsigned _T129C;unsigned _T129D;struct Cyc_Absyn_Exp*_T129E;struct Cyc_Yystacktype*_T129F;union Cyc_YYSTYPE*_T12A0;union Cyc_YYSTYPE*_T12A1;struct Cyc_Absyn_Exp*_T12A2;struct Cyc_Yystacktype*_T12A3;union Cyc_YYSTYPE*_T12A4;union Cyc_YYSTYPE*_T12A5;struct Cyc_Absyn_Exp*_T12A6;struct Cyc_Yystacktype*_T12A7;struct Cyc_Yystacktype _T12A8;struct Cyc_Yyltype _T12A9;unsigned _T12AA;unsigned _T12AB;struct Cyc_Absyn_Exp*_T12AC;struct Cyc_Core_Opt*_T12AD;struct Cyc_Core_Opt*_T12AE;struct Cyc_Core_Opt*_T12AF;struct Cyc_Core_Opt*_T12B0;struct Cyc_Core_Opt*_T12B1;struct Cyc_Core_Opt*_T12B2;struct Cyc_Core_Opt*_T12B3;struct Cyc_Core_Opt*_T12B4;struct Cyc_Core_Opt*_T12B5;struct Cyc_Core_Opt*_T12B6;struct Cyc_Yystacktype*_T12B7;struct Cyc_Yystacktype _T12B8;struct Cyc_Yystacktype*_T12B9;union Cyc_YYSTYPE*_T12BA;union Cyc_YYSTYPE*_T12BB;struct Cyc_Absyn_Exp*_T12BC;struct Cyc_Yystacktype*_T12BD;union Cyc_YYSTYPE*_T12BE;union Cyc_YYSTYPE*_T12BF;struct Cyc_Absyn_Exp*_T12C0;struct Cyc_Yystacktype*_T12C1;union Cyc_YYSTYPE*_T12C2;union Cyc_YYSTYPE*_T12C3;struct Cyc_Absyn_Exp*_T12C4;struct Cyc_Yystacktype*_T12C5;struct Cyc_Yystacktype _T12C6;struct Cyc_Yyltype _T12C7;unsigned _T12C8;unsigned _T12C9;struct Cyc_Absyn_Exp*_T12CA;struct Cyc_Yystacktype*_T12CB;union Cyc_YYSTYPE*_T12CC;union Cyc_YYSTYPE*_T12CD;struct Cyc_Absyn_Exp*_T12CE;struct Cyc_Yystacktype*_T12CF;struct Cyc_Yystacktype _T12D0;struct Cyc_Yyltype _T12D1;unsigned _T12D2;unsigned _T12D3;struct Cyc_Absyn_Exp*_T12D4;struct Cyc_Yystacktype*_T12D5;union Cyc_YYSTYPE*_T12D6;union Cyc_YYSTYPE*_T12D7;struct Cyc_Absyn_Exp*_T12D8;struct Cyc_Yystacktype*_T12D9;struct Cyc_Yystacktype _T12DA;struct Cyc_Yyltype _T12DB;unsigned _T12DC;unsigned _T12DD;struct Cyc_Absyn_Exp*_T12DE;struct Cyc_Yystacktype*_T12DF;union Cyc_YYSTYPE*_T12E0;union Cyc_YYSTYPE*_T12E1;struct Cyc_Absyn_Exp*_T12E2;struct Cyc_Yystacktype*_T12E3;struct Cyc_Yystacktype _T12E4;struct Cyc_Yyltype _T12E5;unsigned _T12E6;unsigned _T12E7;struct Cyc_Absyn_Exp*_T12E8;struct Cyc_Yystacktype*_T12E9;union Cyc_YYSTYPE*_T12EA;union Cyc_YYSTYPE*_T12EB;struct Cyc_Absyn_Exp*_T12EC;struct Cyc_Yystacktype*_T12ED;union Cyc_YYSTYPE*_T12EE;union Cyc_YYSTYPE*_T12EF;struct Cyc_Absyn_Exp*_T12F0;struct Cyc_Yystacktype*_T12F1;struct Cyc_Yystacktype _T12F2;struct Cyc_Yyltype _T12F3;unsigned _T12F4;unsigned _T12F5;struct Cyc_Absyn_Exp*_T12F6;struct Cyc_Yystacktype*_T12F7;union Cyc_YYSTYPE*_T12F8;union Cyc_YYSTYPE*_T12F9;struct Cyc_Absyn_Exp*_T12FA;struct Cyc_Yystacktype*_T12FB;union Cyc_YYSTYPE*_T12FC;union Cyc_YYSTYPE*_T12FD;struct Cyc_Absyn_Exp*_T12FE;struct Cyc_Yystacktype*_T12FF;struct Cyc_Yystacktype _T1300;struct Cyc_Yyltype _T1301;unsigned _T1302;unsigned _T1303;struct Cyc_Absyn_Exp*_T1304;struct Cyc_Yystacktype*_T1305;union Cyc_YYSTYPE*_T1306;union Cyc_YYSTYPE*_T1307;struct Cyc_Absyn_Exp*_T1308;struct Cyc_Absyn_Exp*_T1309;struct Cyc_Yystacktype*_T130A;union Cyc_YYSTYPE*_T130B;union Cyc_YYSTYPE*_T130C;struct Cyc_Absyn_Exp*_T130D;struct Cyc_Absyn_Exp*_T130E;struct Cyc_Yystacktype*_T130F;struct Cyc_Yystacktype _T1310;struct Cyc_Yyltype _T1311;unsigned _T1312;unsigned _T1313;struct Cyc_Absyn_Exp*_T1314;struct Cyc_Yystacktype*_T1315;union Cyc_YYSTYPE*_T1316;union Cyc_YYSTYPE*_T1317;struct Cyc_Absyn_Exp*_T1318;struct Cyc_Absyn_Exp*_T1319;struct Cyc_Yystacktype*_T131A;union Cyc_YYSTYPE*_T131B;union Cyc_YYSTYPE*_T131C;struct Cyc_Absyn_Exp*_T131D;struct Cyc_Absyn_Exp*_T131E;struct Cyc_Yystacktype*_T131F;struct Cyc_Yystacktype _T1320;struct Cyc_Yyltype _T1321;unsigned _T1322;unsigned _T1323;struct Cyc_Absyn_Exp*_T1324;struct Cyc_Yystacktype*_T1325;struct Cyc_Yystacktype _T1326;struct Cyc_Yystacktype*_T1327;struct Cyc_Yystacktype _T1328;struct Cyc_Yystacktype*_T1329;union Cyc_YYSTYPE*_T132A;union Cyc_YYSTYPE*_T132B;struct Cyc_Absyn_Exp*_T132C;struct Cyc_Yystacktype*_T132D;union Cyc_YYSTYPE*_T132E;union Cyc_YYSTYPE*_T132F;struct Cyc_Absyn_Exp*_T1330;struct Cyc_Yystacktype*_T1331;struct Cyc_Yystacktype _T1332;struct Cyc_Yyltype _T1333;unsigned _T1334;unsigned _T1335;struct Cyc_Absyn_Exp*_T1336;struct Cyc_Yystacktype*_T1337;struct Cyc_Yystacktype _T1338;struct Cyc_Yystacktype*_T1339;union Cyc_YYSTYPE*_T133A;union Cyc_YYSTYPE*_T133B;struct Cyc_Absyn_Exp*_T133C;struct Cyc_Yystacktype*_T133D;union Cyc_YYSTYPE*_T133E;union Cyc_YYSTYPE*_T133F;struct Cyc_Absyn_Exp*_T1340;struct Cyc_Yystacktype*_T1341;struct Cyc_Yystacktype _T1342;struct Cyc_Yyltype _T1343;unsigned _T1344;unsigned _T1345;struct Cyc_Absyn_Exp*_T1346;struct Cyc_Yystacktype*_T1347;struct Cyc_Yystacktype _T1348;struct Cyc_Yystacktype*_T1349;union Cyc_YYSTYPE*_T134A;union Cyc_YYSTYPE*_T134B;struct Cyc_Absyn_Exp*_T134C;struct Cyc_Yystacktype*_T134D;union Cyc_YYSTYPE*_T134E;union Cyc_YYSTYPE*_T134F;struct Cyc_Absyn_Exp*_T1350;struct Cyc_Yystacktype*_T1351;struct Cyc_Yystacktype _T1352;struct Cyc_Yyltype _T1353;unsigned _T1354;unsigned _T1355;struct Cyc_Absyn_Exp*_T1356;struct Cyc_Yystacktype*_T1357;struct Cyc_Yystacktype _T1358;struct Cyc_Yystacktype*_T1359;union Cyc_YYSTYPE*_T135A;union Cyc_YYSTYPE*_T135B;struct Cyc_Absyn_Exp*_T135C;struct Cyc_Yystacktype*_T135D;union Cyc_YYSTYPE*_T135E;union Cyc_YYSTYPE*_T135F;struct Cyc_Absyn_Exp*_T1360;struct Cyc_Yystacktype*_T1361;struct Cyc_Yystacktype _T1362;struct Cyc_Yyltype _T1363;unsigned _T1364;unsigned _T1365;struct Cyc_Absyn_Exp*_T1366;struct Cyc_Yystacktype*_T1367;struct Cyc_Yystacktype _T1368;struct Cyc_Yystacktype*_T1369;union Cyc_YYSTYPE*_T136A;union Cyc_YYSTYPE*_T136B;struct Cyc_Absyn_Exp*_T136C;struct Cyc_Yystacktype*_T136D;union Cyc_YYSTYPE*_T136E;union Cyc_YYSTYPE*_T136F;struct Cyc_Absyn_Exp*_T1370;struct Cyc_Yystacktype*_T1371;struct Cyc_Yystacktype _T1372;struct Cyc_Yyltype _T1373;unsigned _T1374;unsigned _T1375;struct Cyc_Absyn_Exp*_T1376;struct Cyc_Yystacktype*_T1377;struct Cyc_Yystacktype _T1378;struct Cyc_Yystacktype*_T1379;union Cyc_YYSTYPE*_T137A;union Cyc_YYSTYPE*_T137B;struct Cyc_Absyn_Exp*(*_T137C)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T137D;union Cyc_YYSTYPE*_T137E;union Cyc_YYSTYPE*_T137F;struct Cyc_Absyn_Exp*_T1380;struct Cyc_Yystacktype*_T1381;union Cyc_YYSTYPE*_T1382;union Cyc_YYSTYPE*_T1383;struct Cyc_Absyn_Exp*_T1384;struct Cyc_Yystacktype*_T1385;struct Cyc_Yystacktype _T1386;struct Cyc_Yyltype _T1387;unsigned _T1388;unsigned _T1389;struct Cyc_Absyn_Exp*_T138A;struct Cyc_Yystacktype*_T138B;struct Cyc_Yystacktype _T138C;struct Cyc_Yystacktype*_T138D;union Cyc_YYSTYPE*_T138E;union Cyc_YYSTYPE*_T138F;struct Cyc_Absyn_Exp*(*_T1390)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T1391;union Cyc_YYSTYPE*_T1392;union Cyc_YYSTYPE*_T1393;struct Cyc_Absyn_Exp*_T1394;struct Cyc_Yystacktype*_T1395;union Cyc_YYSTYPE*_T1396;union Cyc_YYSTYPE*_T1397;struct Cyc_Absyn_Exp*_T1398;struct Cyc_Yystacktype*_T1399;struct Cyc_Yystacktype _T139A;struct Cyc_Yyltype _T139B;unsigned _T139C;unsigned _T139D;struct Cyc_Absyn_Exp*_T139E;struct Cyc_Yystacktype*_T139F;struct Cyc_Yystacktype _T13A0;struct Cyc_Yystacktype*_T13A1;union Cyc_YYSTYPE*_T13A2;union Cyc_YYSTYPE*_T13A3;struct Cyc_Absyn_Exp*_T13A4;struct Cyc_Yystacktype*_T13A5;union Cyc_YYSTYPE*_T13A6;union Cyc_YYSTYPE*_T13A7;struct Cyc_Absyn_Exp*_T13A8;struct Cyc_Yystacktype*_T13A9;struct Cyc_Yystacktype _T13AA;struct Cyc_Yyltype _T13AB;unsigned _T13AC;unsigned _T13AD;struct Cyc_Absyn_Exp*_T13AE;struct Cyc_Yystacktype*_T13AF;union Cyc_YYSTYPE*_T13B0;union Cyc_YYSTYPE*_T13B1;struct Cyc_Absyn_Exp*_T13B2;struct Cyc_Yystacktype*_T13B3;union Cyc_YYSTYPE*_T13B4;union Cyc_YYSTYPE*_T13B5;struct Cyc_Absyn_Exp*_T13B6;struct Cyc_Yystacktype*_T13B7;struct Cyc_Yystacktype _T13B8;struct Cyc_Yyltype _T13B9;unsigned _T13BA;unsigned _T13BB;struct Cyc_Absyn_Exp*_T13BC;struct Cyc_Yystacktype*_T13BD;struct Cyc_Yystacktype _T13BE;struct Cyc_Yystacktype*_T13BF;union Cyc_YYSTYPE*_T13C0;union Cyc_YYSTYPE*_T13C1;enum Cyc_Absyn_Primop _T13C2;struct Cyc_Yystacktype*_T13C3;union Cyc_YYSTYPE*_T13C4;union Cyc_YYSTYPE*_T13C5;struct Cyc_Absyn_Exp*_T13C6;struct Cyc_Yystacktype*_T13C7;union Cyc_YYSTYPE*_T13C8;union Cyc_YYSTYPE*_T13C9;struct Cyc_Absyn_Exp*_T13CA;struct Cyc_Yystacktype*_T13CB;struct Cyc_Yystacktype _T13CC;struct Cyc_Yyltype _T13CD;unsigned _T13CE;unsigned _T13CF;struct Cyc_Absyn_Exp*_T13D0;struct Cyc_Yystacktype*_T13D1;struct Cyc_Yystacktype _T13D2;struct Cyc_Yystacktype*_T13D3;union Cyc_YYSTYPE*_T13D4;union Cyc_YYSTYPE*_T13D5;enum Cyc_Absyn_Primop _T13D6;struct Cyc_Yystacktype*_T13D7;union Cyc_YYSTYPE*_T13D8;union Cyc_YYSTYPE*_T13D9;struct Cyc_Absyn_Exp*_T13DA;struct Cyc_Yystacktype*_T13DB;union Cyc_YYSTYPE*_T13DC;union Cyc_YYSTYPE*_T13DD;struct Cyc_Absyn_Exp*_T13DE;struct Cyc_Yystacktype*_T13DF;struct Cyc_Yystacktype _T13E0;struct Cyc_Yyltype _T13E1;unsigned _T13E2;unsigned _T13E3;struct Cyc_Absyn_Exp*_T13E4;struct Cyc_Absyn_Exp*(*_T13E5)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13E6)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13E7)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13E8)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13E9)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13EA)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T13EB;struct Cyc_Yystacktype _T13EC;struct Cyc_Yystacktype*_T13ED;union Cyc_YYSTYPE*_T13EE;union Cyc_YYSTYPE*_T13EF;struct _tuple8*_T13F0;struct Cyc_Yystacktype*_T13F1;struct Cyc_Yystacktype _T13F2;struct Cyc_Yyltype _T13F3;unsigned _T13F4;unsigned _T13F5;void*_T13F6;struct Cyc_Yystacktype*_T13F7;union Cyc_YYSTYPE*_T13F8;union Cyc_YYSTYPE*_T13F9;struct Cyc_Absyn_Exp*_T13FA;struct Cyc_Yystacktype*_T13FB;struct Cyc_Yystacktype _T13FC;struct Cyc_Yyltype _T13FD;unsigned _T13FE;unsigned _T13FF;struct Cyc_Absyn_Exp*_T1400;struct Cyc_Yystacktype*_T1401;struct Cyc_Yystacktype _T1402;struct Cyc_Yystacktype*_T1403;union Cyc_YYSTYPE*_T1404;union Cyc_YYSTYPE*_T1405;struct Cyc_Absyn_Exp*_T1406;struct Cyc_Yystacktype*_T1407;struct Cyc_Yystacktype _T1408;struct Cyc_Yyltype _T1409;unsigned _T140A;unsigned _T140B;struct Cyc_Absyn_Exp*_T140C;struct Cyc_Yystacktype*_T140D;union Cyc_YYSTYPE*_T140E;union Cyc_YYSTYPE*_T140F;struct Cyc_Absyn_Exp*_T1410;struct Cyc_Yystacktype*_T1411;struct Cyc_Yystacktype _T1412;struct Cyc_Yyltype _T1413;unsigned _T1414;unsigned _T1415;struct Cyc_Absyn_Exp*_T1416;struct Cyc_Yystacktype*_T1417;union Cyc_YYSTYPE*_T1418;union Cyc_YYSTYPE*_T1419;struct Cyc_Absyn_Exp*_T141A;struct Cyc_Yystacktype*_T141B;struct Cyc_Yystacktype _T141C;struct Cyc_Yyltype _T141D;unsigned _T141E;unsigned _T141F;struct Cyc_Absyn_Exp*_T1420;struct Cyc_Yystacktype*_T1421;union Cyc_YYSTYPE*_T1422;union Cyc_YYSTYPE*_T1423;struct Cyc_Absyn_Exp*_T1424;struct Cyc_Yystacktype*_T1425;struct Cyc_Yystacktype _T1426;struct Cyc_Yyltype _T1427;unsigned _T1428;unsigned _T1429;struct Cyc_Absyn_Exp*_T142A;struct Cyc_Yystacktype*_T142B;union Cyc_YYSTYPE*_T142C;union Cyc_YYSTYPE*_T142D;enum Cyc_Absyn_Primop _T142E;struct Cyc_Yystacktype*_T142F;union Cyc_YYSTYPE*_T1430;union Cyc_YYSTYPE*_T1431;struct Cyc_Absyn_Exp*_T1432;struct Cyc_Yystacktype*_T1433;struct Cyc_Yystacktype _T1434;struct Cyc_Yyltype _T1435;unsigned _T1436;unsigned _T1437;struct Cyc_Absyn_Exp*_T1438;struct Cyc_Yystacktype*_T1439;union Cyc_YYSTYPE*_T143A;union Cyc_YYSTYPE*_T143B;struct _tuple8*_T143C;struct Cyc_Yystacktype*_T143D;struct Cyc_Yystacktype _T143E;struct Cyc_Yyltype _T143F;unsigned _T1440;unsigned _T1441;void*_T1442;struct Cyc_Yystacktype*_T1443;struct Cyc_Yystacktype _T1444;struct Cyc_Yyltype _T1445;unsigned _T1446;unsigned _T1447;struct Cyc_Absyn_Exp*_T1448;struct Cyc_Yystacktype*_T1449;union Cyc_YYSTYPE*_T144A;union Cyc_YYSTYPE*_T144B;struct Cyc_Absyn_Exp*_T144C;struct Cyc_Yystacktype*_T144D;struct Cyc_Yystacktype _T144E;struct Cyc_Yyltype _T144F;unsigned _T1450;unsigned _T1451;struct Cyc_Absyn_Exp*_T1452;struct Cyc_Yystacktype*_T1453;union Cyc_YYSTYPE*_T1454;union Cyc_YYSTYPE*_T1455;struct _tuple8*_T1456;struct Cyc_Yystacktype*_T1457;struct Cyc_Yystacktype _T1458;struct Cyc_Yyltype _T1459;unsigned _T145A;unsigned _T145B;void*_T145C;struct Cyc_Yystacktype*_T145D;union Cyc_YYSTYPE*_T145E;union Cyc_YYSTYPE*_T145F;struct Cyc_List_List*_T1460;struct Cyc_List_List*_T1461;struct Cyc_Yystacktype*_T1462;struct Cyc_Yystacktype _T1463;struct Cyc_Yyltype _T1464;unsigned _T1465;unsigned _T1466;struct Cyc_Absyn_Exp*_T1467;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1468;struct Cyc_Yystacktype*_T1469;union Cyc_YYSTYPE*_T146A;union Cyc_YYSTYPE*_T146B;void*_T146C;struct Cyc_Yystacktype*_T146D;struct Cyc_Yystacktype _T146E;struct Cyc_Yyltype _T146F;unsigned _T1470;unsigned _T1471;struct Cyc_Absyn_Exp*_T1472;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1473;struct Cyc_Yystacktype*_T1474;union Cyc_YYSTYPE*_T1475;union Cyc_YYSTYPE*_T1476;struct Cyc_Yystacktype*_T1477;union Cyc_YYSTYPE*_T1478;union Cyc_YYSTYPE*_T1479;struct Cyc_Yystacktype*_T147A;union Cyc_YYSTYPE*_T147B;union Cyc_YYSTYPE*_T147C;void*_T147D;struct Cyc_Yystacktype*_T147E;struct Cyc_Yystacktype _T147F;struct Cyc_Yyltype _T1480;unsigned _T1481;unsigned _T1482;struct Cyc_Absyn_Exp*_T1483;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1484;struct Cyc_Yystacktype*_T1485;union Cyc_YYSTYPE*_T1486;union Cyc_YYSTYPE*_T1487;struct Cyc_Yystacktype*_T1488;union Cyc_YYSTYPE*_T1489;union Cyc_YYSTYPE*_T148A;void*_T148B;struct Cyc_Yystacktype*_T148C;struct Cyc_Yystacktype _T148D;struct Cyc_Yyltype _T148E;unsigned _T148F;unsigned _T1490;struct Cyc_Absyn_Exp*_T1491;struct Cyc_Yystacktype*_T1492;union Cyc_YYSTYPE*_T1493;union Cyc_YYSTYPE*_T1494;struct Cyc_Absyn_Exp*_T1495;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1496;struct Cyc_Yystacktype*_T1497;union Cyc_YYSTYPE*_T1498;union Cyc_YYSTYPE*_T1499;void*_T149A;struct Cyc_Yystacktype*_T149B;struct Cyc_Yystacktype _T149C;struct Cyc_Yyltype _T149D;unsigned _T149E;unsigned _T149F;struct Cyc_Absyn_Exp*_T14A0;struct Cyc_Yystacktype*_T14A1;union Cyc_YYSTYPE*_T14A2;union Cyc_YYSTYPE*_T14A3;struct Cyc_Absyn_Exp*_T14A4;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14A5;struct Cyc_Yystacktype*_T14A6;union Cyc_YYSTYPE*_T14A7;union Cyc_YYSTYPE*_T14A8;void*_T14A9;struct Cyc_Yystacktype*_T14AA;struct Cyc_Yystacktype _T14AB;struct Cyc_Yyltype _T14AC;unsigned _T14AD;unsigned _T14AE;struct Cyc_Absyn_Exp*_T14AF;struct Cyc_Yystacktype*_T14B0;union Cyc_YYSTYPE*_T14B1;union Cyc_YYSTYPE*_T14B2;struct _tuple8*_T14B3;struct Cyc_Yystacktype*_T14B4;struct Cyc_Yystacktype _T14B5;struct Cyc_Yyltype _T14B6;unsigned _T14B7;unsigned _T14B8;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14B9;void**_T14BA;struct Cyc_Yystacktype*_T14BB;union Cyc_YYSTYPE*_T14BC;union Cyc_YYSTYPE*_T14BD;void*_T14BE;struct Cyc_Yystacktype*_T14BF;struct Cyc_Yystacktype _T14C0;struct Cyc_Yyltype _T14C1;unsigned _T14C2;unsigned _T14C3;struct Cyc_Absyn_Exp*_T14C4;struct Cyc_Yystacktype*_T14C5;union Cyc_YYSTYPE*_T14C6;union Cyc_YYSTYPE*_T14C7;struct _tuple8*_T14C8;struct Cyc_Yystacktype*_T14C9;struct Cyc_Yystacktype _T14CA;struct Cyc_Yyltype _T14CB;unsigned _T14CC;unsigned _T14CD;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14CE;struct Cyc_Yystacktype*_T14CF;union Cyc_YYSTYPE*_T14D0;union Cyc_YYSTYPE*_T14D1;struct Cyc_Yystacktype*_T14D2;union Cyc_YYSTYPE*_T14D3;union Cyc_YYSTYPE*_T14D4;void**_T14D5;struct Cyc_Yystacktype*_T14D6;union Cyc_YYSTYPE*_T14D7;union Cyc_YYSTYPE*_T14D8;void*_T14D9;struct Cyc_Yystacktype*_T14DA;struct Cyc_Yystacktype _T14DB;struct Cyc_Yyltype _T14DC;unsigned _T14DD;unsigned _T14DE;struct Cyc_Absyn_Exp*_T14DF;struct Cyc_Yystacktype*_T14E0;union Cyc_YYSTYPE*_T14E1;union Cyc_YYSTYPE*_T14E2;struct _tuple8*_T14E3;struct Cyc_Yystacktype*_T14E4;struct Cyc_Yystacktype _T14E5;struct Cyc_Yyltype _T14E6;unsigned _T14E7;unsigned _T14E8;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14E9;struct Cyc_Yystacktype*_T14EA;union Cyc_YYSTYPE*_T14EB;union Cyc_YYSTYPE*_T14EC;void**_T14ED;struct Cyc_Yystacktype*_T14EE;union Cyc_YYSTYPE*_T14EF;union Cyc_YYSTYPE*_T14F0;void*_T14F1;struct Cyc_Yystacktype*_T14F2;struct Cyc_Yystacktype _T14F3;struct Cyc_Yyltype _T14F4;unsigned _T14F5;unsigned _T14F6;struct Cyc_Absyn_Exp*_T14F7;struct Cyc_List_List*_T14F8;struct Cyc_Absyn_Exp**_T14F9;struct Cyc_Yystacktype*_T14FA;union Cyc_YYSTYPE*_T14FB;union Cyc_YYSTYPE*_T14FC;struct _fat_ptr _T14FD;struct Cyc_Yystacktype*_T14FE;struct Cyc_Yystacktype _T14FF;struct Cyc_Yyltype _T1500;unsigned _T1501;unsigned _T1502;struct Cyc_Absyn_Exp*_T1503;struct Cyc_Yystacktype*_T1504;union Cyc_YYSTYPE*_T1505;union Cyc_YYSTYPE*_T1506;struct Cyc_Absyn_Exp*_T1507;struct Cyc_Yystacktype*_T1508;struct Cyc_Yystacktype _T1509;struct Cyc_Yyltype _T150A;unsigned _T150B;unsigned _T150C;struct Cyc_Absyn_Exp*_T150D;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T150E;struct Cyc_Yystacktype*_T150F;union Cyc_YYSTYPE*_T1510;union Cyc_YYSTYPE*_T1511;struct _fat_ptr*_T1512;struct Cyc_Yystacktype*_T1513;union Cyc_YYSTYPE*_T1514;union Cyc_YYSTYPE*_T1515;void*_T1516;struct Cyc_Yystacktype*_T1517;struct Cyc_Yystacktype _T1518;struct Cyc_Yyltype _T1519;unsigned _T151A;unsigned _T151B;struct Cyc_Absyn_Exp*_T151C;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T151D;struct Cyc_Yystacktype*_T151E;union Cyc_YYSTYPE*_T151F;union Cyc_YYSTYPE*_T1520;struct Cyc_Absyn_Exp*_T1521;struct Cyc_Yystacktype*_T1522;struct Cyc_Yystacktype _T1523;struct Cyc_Yyltype _T1524;unsigned _T1525;unsigned _T1526;struct _fat_ptr*_T1527;struct Cyc_Yystacktype*_T1528;union Cyc_YYSTYPE*_T1529;union Cyc_YYSTYPE*_T152A;void*_T152B;struct Cyc_Yystacktype*_T152C;struct Cyc_Yystacktype _T152D;struct Cyc_Yyltype _T152E;unsigned _T152F;unsigned _T1530;struct Cyc_Absyn_Exp*_T1531;struct Cyc_Yystacktype*_T1532;union Cyc_YYSTYPE*_T1533;union Cyc_YYSTYPE*_T1534;struct _tuple8*_T1535;struct Cyc_Yystacktype*_T1536;struct Cyc_Yystacktype _T1537;struct Cyc_Yyltype _T1538;unsigned _T1539;unsigned _T153A;void*_T153B;struct Cyc_Yystacktype*_T153C;struct Cyc_Yystacktype _T153D;struct Cyc_Yyltype _T153E;unsigned _T153F;unsigned _T1540;struct Cyc_Absyn_Exp*_T1541;struct Cyc_Yystacktype*_T1542;union Cyc_YYSTYPE*_T1543;union Cyc_YYSTYPE*_T1544;void*_T1545;struct Cyc_Yystacktype*_T1546;struct Cyc_Yystacktype _T1547;struct Cyc_Yyltype _T1548;unsigned _T1549;unsigned _T154A;struct Cyc_Absyn_Exp*_T154B;struct Cyc_Yystacktype*_T154C;union Cyc_YYSTYPE*_T154D;union Cyc_YYSTYPE*_T154E;struct Cyc_Absyn_Exp*_T154F;struct Cyc_Yystacktype*_T1550;struct Cyc_Yystacktype _T1551;struct Cyc_Yyltype _T1552;unsigned _T1553;unsigned _T1554;struct Cyc_Absyn_Exp*_T1555;struct Cyc_Yystacktype*_T1556;union Cyc_YYSTYPE*_T1557;union Cyc_YYSTYPE*_T1558;struct Cyc_Absyn_Exp*_T1559;struct Cyc_Yystacktype*_T155A;struct Cyc_Yystacktype _T155B;struct Cyc_Yyltype _T155C;unsigned _T155D;unsigned _T155E;struct Cyc_Absyn_Exp*_T155F;struct Cyc_Yystacktype*_T1560;union Cyc_YYSTYPE*_T1561;union Cyc_YYSTYPE*_T1562;struct Cyc_Absyn_Exp*_T1563;struct Cyc_Yystacktype*_T1564;struct Cyc_Yystacktype _T1565;struct Cyc_Yyltype _T1566;unsigned _T1567;unsigned _T1568;struct Cyc_Absyn_Exp*_T1569;struct Cyc_Yystacktype*_T156A;union Cyc_YYSTYPE*_T156B;union Cyc_YYSTYPE*_T156C;struct Cyc_Absyn_Exp*_T156D;struct Cyc_Yystacktype*_T156E;struct Cyc_Yystacktype _T156F;struct Cyc_Yyltype _T1570;unsigned _T1571;unsigned _T1572;struct Cyc_Absyn_Exp*_T1573;struct Cyc_Yystacktype*_T1574;union Cyc_YYSTYPE*_T1575;union Cyc_YYSTYPE*_T1576;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T1577;struct Cyc_Yystacktype*_T1578;union Cyc_YYSTYPE*_T1579;union Cyc_YYSTYPE*_T157A;struct Cyc_Yystacktype*_T157B;union Cyc_YYSTYPE*_T157C;union Cyc_YYSTYPE*_T157D;void*_T157E;struct _tuple31*_T157F;struct Cyc_Yystacktype*_T1580;union Cyc_YYSTYPE*_T1581;union Cyc_YYSTYPE*_T1582;struct _tuple31*_T1583;struct Cyc_Yystacktype*_T1584;union Cyc_YYSTYPE*_T1585;union Cyc_YYSTYPE*_T1586;struct _tuple31*_T1587;struct Cyc_Yystacktype*_T1588;union Cyc_YYSTYPE*_T1589;union Cyc_YYSTYPE*_T158A;struct Cyc_List_List*_T158B;struct Cyc_List_List*_T158C;struct Cyc_Yystacktype*_T158D;union Cyc_YYSTYPE*_T158E;union Cyc_YYSTYPE*_T158F;struct Cyc_List_List*_T1590;struct Cyc_Yystacktype*_T1591;union Cyc_YYSTYPE*_T1592;union Cyc_YYSTYPE*_T1593;struct Cyc_Yystacktype*_T1594;union Cyc_YYSTYPE*_T1595;union Cyc_YYSTYPE*_T1596;struct _tuple28*_T1597;struct _tuple28*_T1598;struct Cyc_Yystacktype*_T1599;union Cyc_YYSTYPE*_T159A;union Cyc_YYSTYPE*_T159B;struct _tuple28*_T159C;struct Cyc_Yystacktype*_T159D;union Cyc_YYSTYPE*_T159E;union Cyc_YYSTYPE*_T159F;struct Cyc_List_List*_T15A0;struct Cyc_Yystacktype*_T15A1;union Cyc_YYSTYPE*_T15A2;union Cyc_YYSTYPE*_T15A3;struct Cyc_List_List*_T15A4;struct Cyc_Yystacktype*_T15A5;union Cyc_YYSTYPE*_T15A6;union Cyc_YYSTYPE*_T15A7;struct Cyc_List_List*_T15A8;struct Cyc_Yystacktype*_T15A9;union Cyc_YYSTYPE*_T15AA;union Cyc_YYSTYPE*_T15AB;struct Cyc_Yystacktype*_T15AC;union Cyc_YYSTYPE*_T15AD;union Cyc_YYSTYPE*_T15AE;struct _tuple32*_T15AF;struct Cyc_Yystacktype*_T15B0;union Cyc_YYSTYPE*_T15B1;union Cyc_YYSTYPE*_T15B2;struct Cyc_Yystacktype*_T15B3;union Cyc_YYSTYPE*_T15B4;union Cyc_YYSTYPE*_T15B5;struct Cyc_Yystacktype*_T15B6;union Cyc_YYSTYPE*_T15B7;union Cyc_YYSTYPE*_T15B8;struct Cyc_List_List*_T15B9;struct Cyc_List_List*_T15BA;struct Cyc_List_List*_T15BB;struct _fat_ptr*_T15BC;struct Cyc_Yystacktype*_T15BD;union Cyc_YYSTYPE*_T15BE;union Cyc_YYSTYPE*_T15BF;struct Cyc_List_List*_T15C0;struct _fat_ptr*_T15C1;struct Cyc_Yystacktype*_T15C2;union Cyc_YYSTYPE*_T15C3;union Cyc_YYSTYPE*_T15C4;struct Cyc_Yystacktype*_T15C5;union Cyc_YYSTYPE*_T15C6;union Cyc_YYSTYPE*_T15C7;struct Cyc_Yystacktype*_T15C8;struct Cyc_Yystacktype _T15C9;struct Cyc_Yystacktype*_T15CA;union Cyc_YYSTYPE*_T15CB;union Cyc_YYSTYPE*_T15CC;struct Cyc_Absyn_Exp*_T15CD;struct Cyc_Yystacktype*_T15CE;union Cyc_YYSTYPE*_T15CF;union Cyc_YYSTYPE*_T15D0;struct Cyc_Absyn_Exp*_T15D1;struct Cyc_Yystacktype*_T15D2;struct Cyc_Yystacktype _T15D3;struct Cyc_Yyltype _T15D4;unsigned _T15D5;unsigned _T15D6;struct Cyc_Absyn_Exp*_T15D7;struct Cyc_Yystacktype*_T15D8;union Cyc_YYSTYPE*_T15D9;union Cyc_YYSTYPE*_T15DA;struct Cyc_Absyn_Exp*_T15DB;struct Cyc_Yystacktype*_T15DC;struct Cyc_Yystacktype _T15DD;struct Cyc_Yyltype _T15DE;unsigned _T15DF;unsigned _T15E0;struct Cyc_Absyn_Exp*_T15E1;struct Cyc_Yystacktype*_T15E2;union Cyc_YYSTYPE*_T15E3;union Cyc_YYSTYPE*_T15E4;struct Cyc_Absyn_Exp*_T15E5;struct Cyc_Yystacktype*_T15E6;union Cyc_YYSTYPE*_T15E7;union Cyc_YYSTYPE*_T15E8;struct Cyc_List_List*_T15E9;struct Cyc_Yystacktype*_T15EA;struct Cyc_Yystacktype _T15EB;struct Cyc_Yyltype _T15EC;unsigned _T15ED;unsigned _T15EE;struct Cyc_Absyn_Exp*_T15EF;struct Cyc_Yystacktype*_T15F0;union Cyc_YYSTYPE*_T15F1;union Cyc_YYSTYPE*_T15F2;struct Cyc_Absyn_Exp*_T15F3;struct _fat_ptr*_T15F4;struct Cyc_Yystacktype*_T15F5;union Cyc_YYSTYPE*_T15F6;union Cyc_YYSTYPE*_T15F7;struct Cyc_Yystacktype*_T15F8;struct Cyc_Yystacktype _T15F9;struct Cyc_Yyltype _T15FA;unsigned _T15FB;unsigned _T15FC;struct Cyc_Absyn_Exp*_T15FD;struct Cyc_Yystacktype*_T15FE;union Cyc_YYSTYPE*_T15FF;union Cyc_YYSTYPE*_T1600;struct Cyc_Absyn_Exp*_T1601;struct _fat_ptr*_T1602;struct Cyc_Yystacktype*_T1603;union Cyc_YYSTYPE*_T1604;union Cyc_YYSTYPE*_T1605;struct Cyc_Yystacktype*_T1606;struct Cyc_Yystacktype _T1607;struct Cyc_Yyltype _T1608;unsigned _T1609;unsigned _T160A;struct Cyc_Absyn_Exp*_T160B;struct Cyc_Yystacktype*_T160C;union Cyc_YYSTYPE*_T160D;union Cyc_YYSTYPE*_T160E;struct Cyc_Absyn_Exp*_T160F;struct Cyc_Yystacktype*_T1610;struct Cyc_Yystacktype _T1611;struct Cyc_Yyltype _T1612;unsigned _T1613;unsigned _T1614;struct Cyc_Absyn_Exp*_T1615;struct Cyc_Yystacktype*_T1616;union Cyc_YYSTYPE*_T1617;union Cyc_YYSTYPE*_T1618;struct Cyc_Absyn_Exp*_T1619;struct Cyc_Yystacktype*_T161A;struct Cyc_Yystacktype _T161B;struct Cyc_Yyltype _T161C;unsigned _T161D;unsigned _T161E;struct Cyc_Absyn_Exp*_T161F;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1620;struct Cyc_Yystacktype*_T1621;union Cyc_YYSTYPE*_T1622;union Cyc_YYSTYPE*_T1623;void*_T1624;struct Cyc_Yystacktype*_T1625;struct Cyc_Yystacktype _T1626;struct Cyc_Yyltype _T1627;unsigned _T1628;unsigned _T1629;struct Cyc_Absyn_Exp*_T162A;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T162B;struct Cyc_Yystacktype*_T162C;union Cyc_YYSTYPE*_T162D;union Cyc_YYSTYPE*_T162E;struct Cyc_Yystacktype*_T162F;union Cyc_YYSTYPE*_T1630;union Cyc_YYSTYPE*_T1631;struct Cyc_List_List*_T1632;void*_T1633;struct Cyc_Yystacktype*_T1634;struct Cyc_Yystacktype _T1635;struct Cyc_Yyltype _T1636;unsigned _T1637;unsigned _T1638;struct Cyc_Absyn_Exp*_T1639;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T163A;struct Cyc_Yystacktype*_T163B;union Cyc_YYSTYPE*_T163C;union Cyc_YYSTYPE*_T163D;struct Cyc_Yystacktype*_T163E;union Cyc_YYSTYPE*_T163F;union Cyc_YYSTYPE*_T1640;struct Cyc_List_List*_T1641;void*_T1642;struct Cyc_Yystacktype*_T1643;struct Cyc_Yystacktype _T1644;struct Cyc_Yyltype _T1645;unsigned _T1646;unsigned _T1647;struct Cyc_Absyn_Exp*_T1648;struct Cyc_List_List*_T1649;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T164A;struct _fat_ptr*_T164B;struct Cyc_Yystacktype*_T164C;union Cyc_YYSTYPE*_T164D;union Cyc_YYSTYPE*_T164E;struct Cyc_Yystacktype*_T164F;struct Cyc_Yystacktype _T1650;struct Cyc_Yyltype _T1651;unsigned _T1652;unsigned _T1653;struct Cyc_Yystacktype*_T1654;union Cyc_YYSTYPE*_T1655;union Cyc_YYSTYPE*_T1656;union Cyc_Absyn_Cnst _T1657;unsigned _T1658;int _T1659;struct Cyc_List_List*_T165A;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T165B;struct Cyc_List_List*_T165C;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T165D;struct _fat_ptr*_T165E;struct Cyc_Yystacktype*_T165F;union Cyc_YYSTYPE*_T1660;union Cyc_YYSTYPE*_T1661;struct Cyc_Yystacktype*_T1662;union Cyc_YYSTYPE*_T1663;union Cyc_YYSTYPE*_T1664;struct Cyc_Yystacktype*_T1665;struct Cyc_Yystacktype _T1666;struct Cyc_Yyltype _T1667;unsigned _T1668;unsigned _T1669;struct Cyc_Yystacktype*_T166A;union Cyc_YYSTYPE*_T166B;union Cyc_YYSTYPE*_T166C;union Cyc_Absyn_Cnst _T166D;unsigned _T166E;int _T166F;struct Cyc_List_List*_T1670;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1671;struct Cyc_Yystacktype*_T1672;union Cyc_YYSTYPE*_T1673;union Cyc_YYSTYPE*_T1674;struct Cyc_Yystacktype*_T1675;union Cyc_YYSTYPE*_T1676;union Cyc_YYSTYPE*_T1677;struct _tuple0*_T1678;struct Cyc_Yystacktype*_T1679;struct Cyc_Yystacktype _T167A;struct Cyc_Yyltype _T167B;unsigned _T167C;unsigned _T167D;struct Cyc_Absyn_Exp*_T167E;struct Cyc_Yystacktype*_T167F;union Cyc_YYSTYPE*_T1680;union Cyc_YYSTYPE*_T1681;struct _fat_ptr _T1682;struct Cyc_Yystacktype*_T1683;struct Cyc_Yystacktype _T1684;struct Cyc_Yyltype _T1685;unsigned _T1686;unsigned _T1687;struct Cyc_Absyn_Exp*_T1688;struct Cyc_Yystacktype*_T1689;struct Cyc_Yystacktype _T168A;struct Cyc_Yystacktype*_T168B;union Cyc_YYSTYPE*_T168C;union Cyc_YYSTYPE*_T168D;struct _fat_ptr _T168E;struct Cyc_Yystacktype*_T168F;struct Cyc_Yystacktype _T1690;struct Cyc_Yyltype _T1691;unsigned _T1692;unsigned _T1693;struct Cyc_Absyn_Exp*_T1694;struct Cyc_Yystacktype*_T1695;union Cyc_YYSTYPE*_T1696;union Cyc_YYSTYPE*_T1697;struct _fat_ptr _T1698;struct Cyc_Yystacktype*_T1699;struct Cyc_Yystacktype _T169A;struct Cyc_Yyltype _T169B;unsigned _T169C;unsigned _T169D;struct Cyc_Absyn_Exp*_T169E;struct Cyc_Yystacktype*_T169F;struct Cyc_Yystacktype _T16A0;struct Cyc_Yystacktype*_T16A1;union Cyc_YYSTYPE*_T16A2;union Cyc_YYSTYPE*_T16A3;struct Cyc_Absyn_Exp*_T16A4;struct Cyc_Yystacktype*_T16A5;struct Cyc_Yystacktype _T16A6;struct Cyc_Yyltype _T16A7;unsigned _T16A8;unsigned _T16A9;struct Cyc_Absyn_Exp*_T16AA;struct Cyc_Yystacktype*_T16AB;union Cyc_YYSTYPE*_T16AC;union Cyc_YYSTYPE*_T16AD;struct Cyc_Absyn_Exp*_T16AE;struct Cyc_Yystacktype*_T16AF;union Cyc_YYSTYPE*_T16B0;union Cyc_YYSTYPE*_T16B1;struct Cyc_List_List*_T16B2;struct Cyc_List_List*_T16B3;struct Cyc_Yystacktype*_T16B4;struct Cyc_Yystacktype _T16B5;struct Cyc_Yyltype _T16B6;unsigned _T16B7;unsigned _T16B8;struct Cyc_Absyn_Exp*_T16B9;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T16BA;struct Cyc_Yystacktype*_T16BB;union Cyc_YYSTYPE*_T16BC;union Cyc_YYSTYPE*_T16BD;struct Cyc_Yystacktype*_T16BE;union Cyc_YYSTYPE*_T16BF;union Cyc_YYSTYPE*_T16C0;struct Cyc_Yystacktype*_T16C1;union Cyc_YYSTYPE*_T16C2;union Cyc_YYSTYPE*_T16C3;struct Cyc_List_List*_T16C4;void*_T16C5;struct Cyc_Yystacktype*_T16C6;struct Cyc_Yystacktype _T16C7;struct Cyc_Yyltype _T16C8;unsigned _T16C9;unsigned _T16CA;struct Cyc_Absyn_Exp*_T16CB;struct Cyc_Yystacktype*_T16CC;union Cyc_YYSTYPE*_T16CD;union Cyc_YYSTYPE*_T16CE;struct Cyc_List_List*_T16CF;struct Cyc_Yystacktype*_T16D0;struct Cyc_Yystacktype _T16D1;struct Cyc_Yyltype _T16D2;unsigned _T16D3;unsigned _T16D4;struct Cyc_Absyn_Exp*_T16D5;struct Cyc_Yystacktype*_T16D6;union Cyc_YYSTYPE*_T16D7;union Cyc_YYSTYPE*_T16D8;struct Cyc_Absyn_Stmt*_T16D9;struct Cyc_Yystacktype*_T16DA;struct Cyc_Yystacktype _T16DB;struct Cyc_Yyltype _T16DC;unsigned _T16DD;unsigned _T16DE;struct Cyc_Absyn_Exp*_T16DF;struct Cyc_Yystacktype*_T16E0;union Cyc_YYSTYPE*_T16E1;union Cyc_YYSTYPE*_T16E2;struct Cyc_List_List*_T16E3;struct Cyc_List_List*_T16E4;struct Cyc_List_List*_T16E5;struct Cyc_Yystacktype*_T16E6;union Cyc_YYSTYPE*_T16E7;union Cyc_YYSTYPE*_T16E8;struct Cyc_List_List*_T16E9;struct Cyc_Yystacktype*_T16EA;union Cyc_YYSTYPE*_T16EB;union Cyc_YYSTYPE*_T16EC;struct Cyc_Yystacktype*_T16ED;union Cyc_YYSTYPE*_T16EE;union Cyc_YYSTYPE*_T16EF;struct Cyc_Yystacktype*_T16F0;union Cyc_YYSTYPE*_T16F1;union Cyc_YYSTYPE*_T16F2;union Cyc_Absyn_Cnst _T16F3;struct Cyc_Yystacktype*_T16F4;struct Cyc_Yystacktype _T16F5;struct Cyc_Yyltype _T16F6;unsigned _T16F7;unsigned _T16F8;struct Cyc_Absyn_Exp*_T16F9;struct Cyc_Yystacktype*_T16FA;union Cyc_YYSTYPE*_T16FB;union Cyc_YYSTYPE*_T16FC;char _T16FD;struct Cyc_Yystacktype*_T16FE;struct Cyc_Yystacktype _T16FF;struct Cyc_Yyltype _T1700;unsigned _T1701;unsigned _T1702;struct Cyc_Absyn_Exp*_T1703;struct Cyc_Yystacktype*_T1704;union Cyc_YYSTYPE*_T1705;union Cyc_YYSTYPE*_T1706;struct _fat_ptr _T1707;struct Cyc_Yystacktype*_T1708;struct Cyc_Yystacktype _T1709;struct Cyc_Yyltype _T170A;unsigned _T170B;unsigned _T170C;struct Cyc_Absyn_Exp*_T170D;struct Cyc_Yystacktype*_T170E;struct Cyc_Yystacktype _T170F;struct Cyc_Yyltype _T1710;unsigned _T1711;unsigned _T1712;struct Cyc_Absyn_Exp*_T1713;struct Cyc_Yystacktype*_T1714;union Cyc_YYSTYPE*_T1715;union Cyc_YYSTYPE*_T1716;unsigned long _T1717;struct _fat_ptr _T1718;unsigned char*_T1719;const char*_T171A;int _T171B;char _T171C;int _T171D;char _T171E;int _T171F;char _T1720;int _T1721;char _T1722;int _T1723;struct _fat_ptr _T1724;int _T1725;struct Cyc_Yystacktype*_T1726;struct Cyc_Yystacktype _T1727;struct Cyc_Yyltype _T1728;unsigned _T1729;unsigned _T172A;struct Cyc_Absyn_Exp*_T172B;struct _tuple0*_T172C;struct _fat_ptr*_T172D;struct Cyc_Yystacktype*_T172E;union Cyc_YYSTYPE*_T172F;union Cyc_YYSTYPE*_T1730;struct Cyc_Yystacktype*_T1731;struct Cyc_Yystacktype _T1732;struct _tuple0*_T1733;struct _fat_ptr*_T1734;struct Cyc_Yystacktype*_T1735;union Cyc_YYSTYPE*_T1736;union Cyc_YYSTYPE*_T1737;struct Cyc_Yystacktype*_T1738;struct Cyc_Yystacktype _T1739;struct Cyc_Yystacktype*_T173A;struct Cyc_Yystacktype _T173B;struct Cyc_Yystacktype*_T173C;struct Cyc_Yystacktype _T173D;struct Cyc_Yystacktype*_T173E;struct Cyc_Yystacktype _T173F;struct Cyc_Yystacktype*_T1740;struct Cyc_Yystacktype _T1741;struct Cyc_Lexing_lexbuf*_T1742;struct Cyc_List_List*_T1743;struct _tuple35*_T1744;struct Cyc_Yystacktype*_T1745;union Cyc_YYSTYPE*_T1746;union Cyc_YYSTYPE*_T1747;struct _fat_ptr _T1748;struct Cyc_Yystacktype*_T1749;union Cyc_YYSTYPE*_T174A;union Cyc_YYSTYPE*_T174B;struct _fat_ptr _T174C;struct _fat_ptr _T174D;int _T174E;int _T174F;struct Cyc_Yystacktype*_T1750;union Cyc_YYSTYPE*_T1751;union Cyc_YYSTYPE*_T1752;struct _fat_ptr _T1753;void*_T1754;struct Cyc_Yystacktype*_T1755;union Cyc_YYSTYPE*_T1756;union Cyc_YYSTYPE*_T1757;struct Cyc_Yystacktype*_T1758;union Cyc_YYSTYPE*_T1759;union Cyc_YYSTYPE*_T175A;struct Cyc_Yystacktype*_T175B;union Cyc_YYSTYPE*_T175C;union Cyc_YYSTYPE*_T175D;struct Cyc_List_List*_T175E;struct Cyc_List_List*_T175F;struct Cyc_Yystacktype*_T1760;union Cyc_YYSTYPE*_T1761;union Cyc_YYSTYPE*_T1762;struct Cyc_List_List*_T1763;struct Cyc_Yystacktype*_T1764;union Cyc_YYSTYPE*_T1765;union Cyc_YYSTYPE*_T1766;struct Cyc_Yystacktype*_T1767;union Cyc_YYSTYPE*_T1768;union Cyc_YYSTYPE*_T1769;struct Cyc_Yystacktype*_T176A;union Cyc_YYSTYPE*_T176B;union Cyc_YYSTYPE*_T176C;struct _fat_ptr _T176D;void*_T176E;struct Cyc_Yystacktype*_T176F;struct Cyc_Yystacktype _T1770;struct Cyc_Yyltype _T1771;unsigned _T1772;unsigned _T1773;struct Cyc_Yystacktype*_T1774;union Cyc_YYSTYPE*_T1775;union Cyc_YYSTYPE*_T1776;struct _fat_ptr _T1777;void*_T1778;struct Cyc_Yystacktype*_T1779;union Cyc_YYSTYPE*_T177A;union Cyc_YYSTYPE*_T177B;struct _fat_ptr _T177C;struct Cyc_Yystacktype*_T177D;union Cyc_YYSTYPE*_T177E;union Cyc_YYSTYPE*_T177F;void*_T1780;void*_T1781;void*_T1782;struct Cyc_Yystacktype*_T1783;union Cyc_YYSTYPE*_T1784;union Cyc_YYSTYPE*_T1785;struct _fat_ptr _T1786;struct Cyc_Yystacktype*_T1787;union Cyc_YYSTYPE*_T1788;union Cyc_YYSTYPE*_T1789;enum Cyc_Parse_ConstraintOps _T178A;struct Cyc_Yystacktype*_T178B;union Cyc_YYSTYPE*_T178C;union Cyc_YYSTYPE*_T178D;void*_T178E;struct Cyc_Yystacktype*_T178F;union Cyc_YYSTYPE*_T1790;union Cyc_YYSTYPE*_T1791;void*_T1792;void*_T1793;void*_T1794;struct Cyc_Yystacktype*_T1795;union Cyc_YYSTYPE*_T1796;union Cyc_YYSTYPE*_T1797;struct _fat_ptr _T1798;struct Cyc_Yystacktype*_T1799;union Cyc_YYSTYPE*_T179A;union Cyc_YYSTYPE*_T179B;void*_T179C;struct Cyc_Yystacktype*_T179D;union Cyc_YYSTYPE*_T179E;union Cyc_YYSTYPE*_T179F;void*_T17A0;void*_T17A1;void*_T17A2;struct Cyc_Yystacktype*_T17A3;union Cyc_YYSTYPE*_T17A4;union Cyc_YYSTYPE*_T17A5;struct _fat_ptr _T17A6;struct Cyc_Yystacktype*_T17A7;union Cyc_YYSTYPE*_T17A8;union Cyc_YYSTYPE*_T17A9;void*_T17AA;struct Cyc_Yystacktype*_T17AB;union Cyc_YYSTYPE*_T17AC;union Cyc_YYSTYPE*_T17AD;void*_T17AE;void*_T17AF;void*_T17B0;struct Cyc_Yystacktype*_T17B1;union Cyc_YYSTYPE*_T17B2;union Cyc_YYSTYPE*_T17B3;struct _fat_ptr _T17B4;struct Cyc_Yystacktype*_T17B5;union Cyc_YYSTYPE*_T17B6;union Cyc_YYSTYPE*_T17B7;void*_T17B8;struct Cyc_Yystacktype*_T17B9;union Cyc_YYSTYPE*_T17BA;union Cyc_YYSTYPE*_T17BB;void*_T17BC;void*_T17BD;void*_T17BE;struct Cyc_Yystacktype*_T17BF;union Cyc_YYSTYPE*_T17C0;union Cyc_YYSTYPE*_T17C1;struct _fat_ptr _T17C2;struct Cyc_Yystacktype*_T17C3;union Cyc_YYSTYPE*_T17C4;union Cyc_YYSTYPE*_T17C5;void*_T17C6;struct Cyc_Yystacktype*_T17C7;union Cyc_YYSTYPE*_T17C8;union Cyc_YYSTYPE*_T17C9;void*_T17CA;void*_T17CB;void*_T17CC;struct Cyc_Yystacktype*_T17CD;union Cyc_YYSTYPE*_T17CE;union Cyc_YYSTYPE*_T17CF;struct _fat_ptr _T17D0;struct Cyc_Yystacktype*_T17D1;union Cyc_YYSTYPE*_T17D2;union Cyc_YYSTYPE*_T17D3;enum Cyc_Parse_ConstraintOps _T17D4;struct Cyc_Yystacktype*_T17D5;union Cyc_YYSTYPE*_T17D6;union Cyc_YYSTYPE*_T17D7;void*_T17D8;struct Cyc_Yystacktype*_T17D9;union Cyc_YYSTYPE*_T17DA;union Cyc_YYSTYPE*_T17DB;void*_T17DC;void*_T17DD;void*_T17DE;int _T17DF;int _T17E0;struct _fat_ptr _T17E1;int _T17E2;char*_T17E3;struct Cyc_Yystacktype*_T17E4;struct _fat_ptr _T17E5;int _T17E6;struct _fat_ptr _T17E7;char*_T17E8;char*_T17E9;struct Cyc_Yystacktype*_T17EA;struct Cyc_Yyltype _T17EB;struct Cyc_Yystacktype*_T17EC;struct Cyc_Yyltype _T17ED;struct Cyc_Yystacktype*_T17EE;struct Cyc_Yystacktype*_T17EF;struct Cyc_Yystacktype _T17F0;struct Cyc_Yyltype _T17F1;struct Cyc_Yystacktype*_T17F2;struct Cyc_Yystacktype*_T17F3;struct Cyc_Yystacktype _T17F4;struct Cyc_Yyltype _T17F5;struct Cyc_Yystacktype*_T17F6;unsigned char*_T17F7;struct Cyc_Yystacktype*_T17F8;int _T17F9;struct _fat_ptr _T17FA;int _T17FB;int _T17FC;char*_T17FD;struct Cyc_Yystacktype*_T17FE;struct Cyc_Yystacktype _T17FF;struct Cyc_Yyltype _T1800;struct Cyc_Yystacktype*_T1801;unsigned char*_T1802;struct Cyc_Yystacktype*_T1803;int _T1804;struct _fat_ptr _T1805;unsigned char*_T1806;struct Cyc_Yystacktype*_T1807;int _T1808;int _T1809;struct Cyc_Yystacktype _T180A;struct Cyc_Yyltype _T180B;short*_T180C;int _T180D;char*_T180E;short*_T180F;short _T1810;short*_T1811;int _T1812;char*_T1813;short*_T1814;short _T1815;int _T1816;struct _fat_ptr _T1817;int _T1818;char*_T1819;short*_T181A;short _T181B;int _T181C;short*_T181D;int _T181E;short _T181F;int _T1820;struct _fat_ptr _T1821;unsigned char*_T1822;short*_T1823;int _T1824;short _T1825;int _T1826;short*_T1827;int _T1828;short _T1829;short*_T182A;int _T182B;char*_T182C;short*_T182D;short _T182E;short*_T182F;int _T1830;char*_T1831;short*_T1832;short _T1833;int _T1834;int _T1835;unsigned _T1836;unsigned _T1837;short*_T1838;int _T1839;char*_T183A;short*_T183B;short _T183C;int _T183D;int _T183E;struct _fat_ptr*_T183F;int _T1840;char*_T1841;struct _fat_ptr*_T1842;struct _fat_ptr _T1843;unsigned long _T1844;unsigned long _T1845;struct _fat_ptr _T1846;int _T1847;unsigned _T1848;char*_T1849;struct _RegionHandle*_T184A;unsigned _T184B;int _T184C;char*_T184D;unsigned _T184E;char*_T184F;struct _fat_ptr _T1850;struct _fat_ptr _T1851;int _T1852;int _T1853;unsigned _T1854;unsigned _T1855;short*_T1856;int _T1857;char*_T1858;short*_T1859;short _T185A;int _T185B;int _T185C;struct _fat_ptr _T185D;struct _fat_ptr _T185E;struct _fat_ptr _T185F;struct _fat_ptr _T1860;struct _fat_ptr _T1861;struct _fat_ptr*_T1862;int _T1863;char*_T1864;struct _fat_ptr*_T1865;struct _fat_ptr _T1866;struct _fat_ptr _T1867;struct _fat_ptr _T1868;struct _fat_ptr _T1869;int _T186A;int _T186B;struct _fat_ptr _T186C;int _T186D;char*_T186E;short*_T186F;short _T1870;short*_T1871;int _T1872;char*_T1873;short*_T1874;short _T1875;short*_T1876;int _T1877;short _T1878;int _T1879;short*_T187A;int _T187B;short _T187C;struct _fat_ptr _T187D;int _T187E;char*_T187F;struct Cyc_Yystacktype*_T1880;struct Cyc_Yystacktype _T1881;struct _RegionHandle _T1882=_new_region(0U,"yyregion");struct _RegionHandle*yyregion=& _T1882;_push_region(yyregion);{
# 149
int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;{union Cyc_YYSTYPE _T1883;_T1883.YYINITIALSVAL.tag=1U;
_T1883.YYINITIALSVAL.val=0;_T0=_T1883;}{union Cyc_YYSTYPE yylval=_T0;
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;{unsigned _T1883=200U;_T2=yyregion;_T3=_region_calloc(_T2,0U,sizeof(short),_T1883);_T1=_tag_fat(_T3,sizeof(short),_T1883);}{
# 164
struct _fat_ptr yyss=_T1;
# 166
int yyvsp_offset;{unsigned _T1883=200U;_T6=yyregion;_T7=_check_times(_T1883,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T1884=_region_malloc(_T6,0U,_T7);{unsigned _T1885=200U;unsigned i;i=0;_TL250: if(i < _T1885)goto _TL24E;else{goto _TL24F;}_TL24E: _T9=i;_T8=_T1884 + _T9;
# 169
(*_T8).v=yylval;_TB=i;_TA=_T1884 + _TB;(*_TA).l=Cyc_yynewloc();i=i + 1;goto _TL250;_TL24F:;}_T5=(struct Cyc_Yystacktype*)_T1884;}_T4=_T5;}{
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
yynewstate: _TC=yyss;
# 208
yyssp_offset=yyssp_offset + 1;_TD=yyssp_offset;_TE=_check_fat_subscript(_TC,sizeof(short),_TD);_TF=(short*)_TE;_T10=yystate;*_TF=(short)_T10;_T11=yyssp_offset;_T12=yystacksize - 1;_T13=_T12 - 14;
# 210
if(_T11 < _T13)goto _TL251;
# 212
if(yystacksize < 10000)goto _TL253;_T14=
_tag_fat("parser stack overflow",sizeof(char),22U);_T15=yystate;_T16=yychar;Cyc_yyerror(_T14,_T15,_T16);_T17=& Cyc_Yystack_overflow_val;_T18=(struct Cyc_Yystack_overflow_exn_struct*)_T17;_throw(_T18);goto _TL254;_TL253: _TL254:
# 216
 yystacksize=yystacksize * 2;
if(yystacksize <= 10000)goto _TL255;
yystacksize=10000;goto _TL256;_TL255: _TL256: _T1A=yystacksize;{unsigned _T1883=(unsigned)_T1A;_T1C=yyregion;_T1D=_check_times(_T1883,sizeof(short));{short*_T1884=_region_malloc(_T1C,0U,_T1D);_T1E=yystacksize;{unsigned _T1885=(unsigned)_T1E;unsigned i;i=0;_TL25A: if(i < _T1885)goto _TL258;else{goto _TL259;}_TL258: _T1F=i;_T20=yyssp_offset;_T21=(unsigned)_T20;
# 220
if(_T1F > _T21)goto _TL25B;_T23=i;_T22=_T1884 + _T23;_T24=yyss;_T25=_T24.curr;_T26=(short*)_T25;_T27=i;_T28=(int)_T27;*_T22=_T26[_T28];goto _TL25C;_TL25B: _T2A=i;_T29=_T1884 + _T2A;*_T29=0;_TL25C: i=i + 1;goto _TL25A;_TL259:;}_T1B=(short*)_T1884;}_T19=
# 219
_tag_fat(_T1B,sizeof(short),_T1883);}{struct _fat_ptr yyss1=_T19;_T2C=yystacksize;{unsigned _T1883=(unsigned)_T2C;_T2E=yyregion;_T2F=_check_times(_T1883,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T1884=_region_malloc(_T2E,0U,_T2F);_T30=yystacksize;{unsigned _T1885=(unsigned)_T30;unsigned i;i=0;_TL260: if(i < _T1885)goto _TL25E;else{goto _TL25F;}_TL25E: _T31=i;_T32=yyssp_offset;_T33=(unsigned)_T32;
# 224
if(_T31 > _T33)goto _TL261;_T35=i;_T34=_T1884 + _T35;_T36=yyvs;_T37=i;_T38=(int)_T37;_T39=_check_fat_subscript(_T36,sizeof(struct Cyc_Yystacktype),_T38);_T3A=(struct Cyc_Yystacktype*)_T39;*_T34=*_T3A;goto _TL262;_TL261: _T3C=i;_T3B=_T1884 + _T3C;_T3D=yyvs;_T3E=_check_fat_subscript(_T3D,sizeof(struct Cyc_Yystacktype),0);_T3F=(struct Cyc_Yystacktype*)_T3E;*_T3B=*_T3F;_TL262: i=i + 1;goto _TL260;_TL25F:;}_T2D=(struct Cyc_Yystacktype*)_T1884;}_T2B=
# 223
_tag_fat(_T2D,sizeof(struct Cyc_Yystacktype),_T1883);}{
# 222
struct _fat_ptr yyvs1=_T2B;
# 230
yyss=yyss1;
yyvs=yyvs1;}}goto _TL252;_TL251: _TL252: goto yybackup;
# 242
yybackup: _T40=Cyc_yypact;_T41=yystate;_T42=_check_known_subscript_notnull(_T40,1262U,sizeof(short),_T41);_T43=(short*)_T42;_T44=*_T43;
# 254 "cycbison.simple"
yyn=(int)_T44;
if(yyn!=-32768)goto _TL263;goto yydefault;_TL263:
# 261
 if(yychar!=-2)goto _TL265;_T45=yylex_buf;_T46=& yylval;_T47=(union Cyc_YYSTYPE*)_T46;_T48=& yylloc;_T49=(struct Cyc_Yyltype*)_T48;
# 267
yychar=Cyc_yylex(_T45,_T47,_T49);goto _TL266;_TL265: _TL266:
# 271
 if(yychar > 0)goto _TL267;
# 273
yychar1=0;
yychar=0;goto _TL268;
# 282
_TL267: if(yychar <= 0)goto _TL269;if(yychar > 400)goto _TL269;_T4B=Cyc_yytranslate;_T4C=yychar;_T4D=_T4B[_T4C];_T4A=(int)_T4D;goto _TL26A;_TL269: _T4A=363;_TL26A: yychar1=_T4A;_TL268: _T4E=yychar1;
# 299 "cycbison.simple"
yyn=yyn + _T4E;
if(yyn < 0)goto _TL26D;else{goto _TL26F;}_TL26F: if(yyn > 8241)goto _TL26D;else{goto _TL26E;}_TL26E: _T4F=Cyc_yycheck;_T50=yyn;_T51=_T4F[_T50];_T52=(int)_T51;_T53=yychar1;if(_T52!=_T53)goto _TL26D;else{goto _TL26B;}_TL26D: goto yydefault;_TL26B: _T54=Cyc_yytable;_T55=yyn;_T56=_T54[_T55];
# 302
yyn=(int)_T56;
# 309
if(yyn >= 0)goto _TL270;
# 311
if(yyn!=-32768)goto _TL272;goto yyerrlab;_TL272:
 yyn=- yyn;goto yyreduce;
# 315
_TL270: if(yyn!=0)goto _TL274;goto yyerrlab;_TL274:
# 317
 if(yyn!=1261)goto _TL276;{int _T1883=0;_npop_handler(0);return _T1883;}_TL276:
# 328 "cycbison.simple"
 if(yychar==0)goto _TL278;
yychar=-2;goto _TL279;_TL278: _TL279: _T57=yyvs;
# 332
yyvsp_offset=yyvsp_offset + 1;_T58=yyvsp_offset;_T59=_check_fat_subscript(_T57,sizeof(struct Cyc_Yystacktype),_T58);_T5A=(struct Cyc_Yystacktype*)_T59;{struct Cyc_Yystacktype _T1883;_T1883.v=yylval;_T1883.l=yylloc;_T5B=_T1883;}*_T5A=_T5B;
# 338
if(yyerrstatus==0)goto _TL27A;yyerrstatus=yyerrstatus + -1;goto _TL27B;_TL27A: _TL27B:
# 340
 yystate=yyn;goto yynewstate;
# 344
yydefault: _T5C=Cyc_yydefact;_T5D=yystate;_T5E=_check_known_subscript_notnull(_T5C,1262U,sizeof(short),_T5D);_T5F=(short*)_T5E;_T60=*_T5F;
# 346
yyn=(int)_T60;
if(yyn!=0)goto _TL27C;goto yyerrlab;_TL27C:
# 351
 yyreduce: _T61=Cyc_yyr2;_T62=yyn;_T63=_check_known_subscript_notnull(_T61,624U,sizeof(short),_T62);_T64=(short*)_T63;_T65=*_T64;
# 353
yylen=(int)_T65;_T66=yyvs;_T67=yyvsp_offset + 1;_T68=yylen;_T69=_T67 - _T68;_T6A=
_fat_ptr_plus(_T66,sizeof(struct Cyc_Yystacktype),_T69);_T6B=_untag_fat_ptr_check_bound(_T6A,sizeof(struct Cyc_Yystacktype),14U);_T6C=_check_null(_T6B);yyyvsp=(struct Cyc_Yystacktype*)_T6C;
if(yylen <= 0)goto _TL27E;_T6D=yyyvsp;_T6E=_T6D[0];
yyval=_T6E.v;goto _TL27F;_TL27E: _TL27F: _T6F=yyn;_T70=(int)_T6F;switch(_T70){case 1:
# 1323 "parse.y"
 yyval=Cyc_YY74(0);goto _LL0;case 2:
# 1326 "parse.y"
 yyval=Cyc_YY74(1);
_T71=yyyvsp + 0;_T72=&(*_T71).v;_T73=(union Cyc_YYSTYPE*)_T72;Cyc_Parse_constraint_graph=Cyc_yyget_YY71(_T73);goto _LL0;case 3: _T74=yyyvsp;_T75=_T74[0];
# 1333 "parse.y"
yyval=_T75.v;
_T76=yyyvsp + 0;_T77=&(*_T76).v;_T78=(union Cyc_YYSTYPE*)_T77;Cyc_Parse_parse_result=Cyc_yyget_YY16(_T78);goto _LL0;case 4:
# 1339 "parse.y"
 _T79=yyyvsp + 0;_T7A=&(*_T79).v;_T7B=(union Cyc_YYSTYPE*)_T7A;_T7C=Cyc_yyget_YY16(_T7B);_T7D=yyyvsp + 1;_T7E=&(*_T7D).v;_T7F=(union Cyc_YYSTYPE*)_T7E;_T80=Cyc_yyget_YY16(_T7F);_T81=Cyc_List_imp_append(_T7C,_T80);yyval=Cyc_YY16(_T81);goto _LL0;case 5:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1885=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T1885->tag=10;
# 1343 "parse.y"
_T85=yyyvsp + 0;_T86=&(*_T85).v;_T87=(union Cyc_YYSTYPE*)_T86;_T1885->f1=Cyc_yyget_QualId_tok(_T87);_T88=yyyvsp + 2;_T89=&(*_T88).v;_T8A=(union Cyc_YYSTYPE*)_T89;_T1885->f2=Cyc_yyget_YY16(_T8A);_T84=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1885;}_T1884->r=(void*)_T84;_T8B=yyyvsp;_T8C=_T8B[0];_T8D=_T8C.l;_T8E=_T8D.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_T8E);_T83=(struct Cyc_Absyn_Decl*)_T1884;}_T1883->hd=_T83;_T1883->tl=0;_T82=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T82);
Cyc_Lex_leave_using();goto _LL0;case 6:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1885=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T1885->tag=10;
# 1347 "parse.y"
_T92=yyyvsp + 0;_T93=&(*_T92).v;_T94=(union Cyc_YYSTYPE*)_T93;_T1885->f1=Cyc_yyget_QualId_tok(_T94);_T95=yyyvsp + 2;_T96=&(*_T95).v;_T97=(union Cyc_YYSTYPE*)_T96;_T1885->f2=Cyc_yyget_YY16(_T97);_T91=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1885;}_T1884->r=(void*)_T91;_T98=yyyvsp;_T99=_T98[0];_T9A=_T99.l;_T9B=_T9A.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_T9B);_T90=(struct Cyc_Absyn_Decl*)_T1884;}_T1883->hd=_T90;_T9C=yyyvsp + 4;_T9D=&(*_T9C).v;_T9E=(union Cyc_YYSTYPE*)_T9D;_T1883->tl=Cyc_yyget_YY16(_T9E);_T8F=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T8F);goto _LL0;case 7:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1885=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T1885->tag=9;{struct _fat_ptr*_T1886=_cycalloc(sizeof(struct _fat_ptr));
# 1350
_TA3=yyyvsp + 0;_TA4=&(*_TA3).v;_TA5=(union Cyc_YYSTYPE*)_TA4;*_T1886=Cyc_yyget_String_tok(_TA5);_TA2=(struct _fat_ptr*)_T1886;}_T1885->f1=_TA2;_TA6=yyyvsp + 2;_TA7=&(*_TA6).v;_TA8=(union Cyc_YYSTYPE*)_TA7;_T1885->f2=Cyc_yyget_YY16(_TA8);_TA1=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1885;}_T1884->r=(void*)_TA1;_TA9=yyyvsp;_TAA=_TA9[0];_TAB=_TAA.l;_TAC=_TAB.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_TAC);_TA0=(struct Cyc_Absyn_Decl*)_T1884;}_T1883->hd=_TA0;_T1883->tl=0;_T9F=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T9F);
Cyc_Lex_leave_namespace();goto _LL0;case 8:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1885=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T1885->tag=9;{struct _fat_ptr*_T1886=_cycalloc(sizeof(struct _fat_ptr));
# 1354 "parse.y"
_TB1=yyyvsp + 0;_TB2=&(*_TB1).v;_TB3=(union Cyc_YYSTYPE*)_TB2;*_T1886=Cyc_yyget_String_tok(_TB3);_TB0=(struct _fat_ptr*)_T1886;}_T1885->f1=_TB0;_TB4=yyyvsp + 2;_TB5=&(*_TB4).v;_TB6=(union Cyc_YYSTYPE*)_TB5;_T1885->f2=Cyc_yyget_YY16(_TB6);_TAF=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1885;}_T1884->r=(void*)_TAF;_TB7=yyyvsp;_TB8=_TB7[0];_TB9=_TB8.l;_TBA=_TB9.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_TBA);_TAE=(struct Cyc_Absyn_Decl*)_T1884;}_T1883->hd=_TAE;_TBB=yyyvsp + 4;_TBC=&(*_TBB).v;_TBD=(union Cyc_YYSTYPE*)_TBC;_T1883->tl=Cyc_yyget_YY16(_TBD);_TAD=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_TAD);goto _LL0;case 9:
# 1356 "parse.y"
 _TBE=yyyvsp + 2;_TBF=&(*_TBE).v;_TC0=(union Cyc_YYSTYPE*)_TBF;_TC1=Cyc_yyget_YY16(_TC0);_TC2=yyyvsp + 4;_TC3=&(*_TC2).v;_TC4=(union Cyc_YYSTYPE*)_TC3;_TC5=Cyc_yyget_YY16(_TC4);_TC6=Cyc_List_append(_TC1,_TC5);yyval=Cyc_YY16(_TC6);goto _LL0;case 10:
# 1358 "parse.y"
 _TC7=yyyvsp + 0;_TC8=&(*_TC7).v;_TC9=(union Cyc_YYSTYPE*)_TC8;{int is_c_include=Cyc_yyget_YY32(_TC9);
_TCA=yyyvsp + 4;_TCB=&(*_TCA).v;_TCC=(union Cyc_YYSTYPE*)_TCB;{struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(_TCC);
_TCD=yyyvsp + 5;_TCE=&(*_TCD).v;_TCF=(union Cyc_YYSTYPE*)_TCE;{struct _tuple30*_T1883=Cyc_yyget_YY56(_TCF);unsigned _T1884;struct Cyc_List_List*_T1885;{struct _tuple30 _T1886=*_T1883;_T1885=_T1886.f0;_T1884=_T1886.f1;}{struct Cyc_List_List*exs=_T1885;unsigned wc=_T1884;
_TD0=yyyvsp + 6;_TD1=&(*_TD0).v;_TD2=(union Cyc_YYSTYPE*)_TD1;{struct Cyc_List_List*hides=Cyc_yyget_YY57(_TD2);
if(exs==0)goto _TL281;if(hides==0)goto _TL281;_TD3=yyyvsp;_TD4=_TD3[0];_TD5=_TD4.l;_TD6=_TD5.first_line;_TD7=
Cyc_Position_loc_to_seg(_TD6);_TD8=_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U);_TD9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TD7,_TD8,_TD9);goto _TL282;_TL281: _TL282: _TDA=hides;_TDB=(unsigned)_TDA;
if(!_TDB)goto _TL283;_TDC=wc;_TDD=(int)_TDC;if(_TDD)goto _TL283;else{goto _TL285;}
_TL285: _TDE=yyyvsp;_TDF=_TDE[6];_TE0=_TDF.l;_TE1=_TE0.first_line;wc=Cyc_Position_loc_to_seg(_TE1);goto _TL284;_TL283: _TL284: _TE2=is_c_include;
if(_TE2)goto _TL286;else{goto _TL288;}
_TL288: if(exs!=0)goto _TL28B;else{goto _TL28C;}_TL28C: if(cycdecls!=0)goto _TL28B;else{goto _TL289;}
_TL28B: _TE3=yyyvsp;_TE4=_TE3[0];_TE5=_TE4.l;_TE6=_TE5.first_line;_TE7=Cyc_Position_loc_to_seg(_TE6);_TE8=_tag_fat("expecting \"C include\"",sizeof(char),22U);_TE9=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TE7,_TE8,_TE9);{struct Cyc_List_List*_T1886=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1887=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1888=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T1888->tag=12;
_TED=yyyvsp + 2;_TEE=&(*_TED).v;_TEF=(union Cyc_YYSTYPE*)_TEE;_T1888->f1=Cyc_yyget_YY16(_TEF);_T1888->f2=cycdecls;_T1888->f3=exs;{struct _tuple10*_T1889=_cycalloc(sizeof(struct _tuple10));_T1889->f0=wc;_T1889->f1=hides;_TF0=(struct _tuple10*)_T1889;}_T1888->f4=_TF0;_TEC=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1888;}_T1887->r=(void*)_TEC;_TF1=yyyvsp;_TF2=_TF1[0];_TF3=_TF2.l;_TF4=_TF3.first_line;_T1887->loc=Cyc_Position_loc_to_seg(_TF4);_TEB=(struct Cyc_Absyn_Decl*)_T1887;}_T1886->hd=_TEB;_TF5=yyyvsp + 7;_TF6=&(*_TF5).v;_TF7=(union Cyc_YYSTYPE*)_TF6;_T1886->tl=Cyc_yyget_YY16(_TF7);_TEA=(struct Cyc_List_List*)_T1886;}yyval=Cyc_YY16(_TEA);goto _TL28A;
# 1372
_TL289:{struct Cyc_List_List*_T1886=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1887=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T1888=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_T1888->tag=11;_TFB=yyyvsp + 2;_TFC=&(*_TFB).v;_TFD=(union Cyc_YYSTYPE*)_TFC;_T1888->f1=Cyc_yyget_YY16(_TFD);_TFA=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T1888;}_T1887->r=(void*)_TFA;_TFE=yyyvsp;_TFF=_TFE[0];_T100=_TFF.l;_T101=_T100.first_line;_T1887->loc=Cyc_Position_loc_to_seg(_T101);_TF9=(struct Cyc_Absyn_Decl*)_T1887;}_T1886->hd=_TF9;_T102=yyyvsp + 7;_T103=&(*_T102).v;_T104=(union Cyc_YYSTYPE*)_T103;_T1886->tl=Cyc_yyget_YY16(_T104);_TF8=(struct Cyc_List_List*)_T1886;}yyval=Cyc_YY16(_TF8);_TL28A: goto _TL287;
# 1375
_TL286:{struct Cyc_List_List*_T1886=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1887=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1888=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T1888->tag=12;_T108=yyyvsp + 2;_T109=&(*_T108).v;_T10A=(union Cyc_YYSTYPE*)_T109;_T1888->f1=Cyc_yyget_YY16(_T10A);_T1888->f2=cycdecls;_T1888->f3=exs;{struct _tuple10*_T1889=_cycalloc(sizeof(struct _tuple10));_T1889->f0=wc;_T1889->f1=hides;_T10B=(struct _tuple10*)_T1889;}_T1888->f4=_T10B;_T107=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T1888;}_T1887->r=(void*)_T107;_T10C=yyyvsp;_T10D=_T10C[0];_T10E=_T10D.l;_T10F=_T10E.first_line;_T1887->loc=Cyc_Position_loc_to_seg(_T10F);_T106=(struct Cyc_Absyn_Decl*)_T1887;}_T1886->hd=_T106;_T110=yyyvsp + 7;_T111=&(*_T110).v;_T112=(union Cyc_YYSTYPE*)_T111;_T1886->tl=Cyc_yyget_YY16(_T112);_T105=(struct Cyc_List_List*)_T1886;}yyval=Cyc_YY16(_T105);_TL287: goto _LL0;}}}}}case 11:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T115=& Cyc_Absyn_Porton_d_val;_T116=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_T115;
# 1378 "parse.y"
_T1884->r=(void*)_T116;_T117=yyyvsp;_T118=_T117[0];_T119=_T118.l;_T11A=_T119.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_T11A);_T114=(struct Cyc_Absyn_Decl*)_T1884;}_T1883->hd=_T114;_T11B=yyyvsp + 2;_T11C=&(*_T11B).v;_T11D=(union Cyc_YYSTYPE*)_T11C;_T1883->tl=Cyc_yyget_YY16(_T11D);_T113=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T113);goto _LL0;case 12:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T120=& Cyc_Absyn_Portoff_d_val;_T121=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_T120;
# 1380 "parse.y"
_T1884->r=(void*)_T121;_T122=yyyvsp;_T123=_T122[0];_T124=_T123.l;_T125=_T124.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_T125);_T11F=(struct Cyc_Absyn_Decl*)_T1884;}_T1883->hd=_T11F;_T126=yyyvsp + 2;_T127=&(*_T126).v;_T128=(union Cyc_YYSTYPE*)_T127;_T1883->tl=Cyc_yyget_YY16(_T128);_T11E=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T11E);goto _LL0;case 13:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T12B=& Cyc_Absyn_Tempeston_d_val;_T12C=(struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*)_T12B;
# 1382 "parse.y"
_T1884->r=(void*)_T12C;_T12D=yyyvsp;_T12E=_T12D[0];_T12F=_T12E.l;_T130=_T12F.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_T130);_T12A=(struct Cyc_Absyn_Decl*)_T1884;}_T1883->hd=_T12A;_T131=yyyvsp + 2;_T132=&(*_T131).v;_T133=(union Cyc_YYSTYPE*)_T132;_T1883->tl=Cyc_yyget_YY16(_T133);_T129=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T129);goto _LL0;case 14:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T136=& Cyc_Absyn_Tempestoff_d_val;_T137=(struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*)_T136;
# 1384 "parse.y"
_T1884->r=(void*)_T137;_T138=yyyvsp;_T139=_T138[0];_T13A=_T139.l;_T13B=_T13A.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_T13B);_T135=(struct Cyc_Absyn_Decl*)_T1884;}_T1883->hd=_T135;_T13C=yyyvsp + 2;_T13D=&(*_T13C).v;_T13E=(union Cyc_YYSTYPE*)_T13D;_T1883->tl=Cyc_yyget_YY16(_T13E);_T134=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T134);goto _LL0;case 15:
# 1385 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 16:
# 1389 "parse.y"
 Cyc_Parse_parsing_tempest=1;goto _LL0;case 17:
# 1392
 Cyc_Parse_parsing_tempest=0;goto _LL0;case 18:
# 1397 "parse.y"
 Cyc_Lex_enter_extern_c();
_T13F=yyyvsp + 1;_T140=&(*_T13F).v;_T141=(union Cyc_YYSTYPE*)_T140;_T142=Cyc_yyget_String_tok(_T141);_T143=_tag_fat("C",sizeof(char),2U);_T144=Cyc_strcmp(_T142,_T143);if(_T144!=0)goto _TL28D;
yyval=Cyc_YY32(0);goto _TL28E;
_TL28D: _T145=yyyvsp + 1;_T146=&(*_T145).v;_T147=(union Cyc_YYSTYPE*)_T146;_T148=Cyc_yyget_String_tok(_T147);_T149=_tag_fat("C include",sizeof(char),10U);_T14A=Cyc_strcmp(_T148,_T149);if(_T14A!=0)goto _TL28F;
yyval=Cyc_YY32(1);goto _TL290;
# 1403
_TL28F: _T14B=yyyvsp;_T14C=_T14B[0];_T14D=_T14C.l;_T14E=_T14D.first_line;_T14F=Cyc_Position_loc_to_seg(_T14E);_T150=_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U);_T151=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T14F,_T150,_T151);
yyval=Cyc_YY32(1);_TL290: _TL28E: goto _LL0;case 19:
# 1410 "parse.y"
 Cyc_Lex_leave_extern_c();goto _LL0;case 20:
# 1414 "parse.y"
 yyval=Cyc_YY57(0);goto _LL0;case 21: _T152=yyyvsp;_T153=_T152[2];
# 1415 "parse.y"
yyval=_T153.v;goto _LL0;case 22:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1419 "parse.y"
_T155=yyyvsp + 0;_T156=&(*_T155).v;_T157=(union Cyc_YYSTYPE*)_T156;_T1883->hd=Cyc_yyget_QualId_tok(_T157);_T1883->tl=0;_T154=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY57(_T154);goto _LL0;case 23:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1420 "parse.y"
_T159=yyyvsp + 0;_T15A=&(*_T159).v;_T15B=(union Cyc_YYSTYPE*)_T15A;_T1883->hd=Cyc_yyget_QualId_tok(_T15B);_T1883->tl=0;_T158=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY57(_T158);goto _LL0;case 24:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1421 "parse.y"
_T15D=yyyvsp + 0;_T15E=&(*_T15D).v;_T15F=(union Cyc_YYSTYPE*)_T15E;_T1883->hd=Cyc_yyget_QualId_tok(_T15F);_T160=yyyvsp + 2;_T161=&(*_T160).v;_T162=(union Cyc_YYSTYPE*)_T161;_T1883->tl=Cyc_yyget_YY57(_T162);_T15C=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY57(_T15C);goto _LL0;case 25:{struct _tuple30*_T1883=_cycalloc(sizeof(struct _tuple30));
# 1425 "parse.y"
_T1883->f0=0;_T1883->f1=0U;_T163=(struct _tuple30*)_T1883;}yyval=Cyc_YY56(_T163);goto _LL0;case 26: _T164=yyyvsp;_T165=_T164[0];
# 1426 "parse.y"
yyval=_T165.v;goto _LL0;case 27:{struct _tuple30*_T1883=_cycalloc(sizeof(struct _tuple30));
# 1430 "parse.y"
_T167=yyyvsp + 2;_T168=&(*_T167).v;_T169=(union Cyc_YYSTYPE*)_T168;_T1883->f0=Cyc_yyget_YY55(_T169);_T1883->f1=0U;_T166=(struct _tuple30*)_T1883;}yyval=Cyc_YY56(_T166);goto _LL0;case 28:{struct _tuple30*_T1883=_cycalloc(sizeof(struct _tuple30));
# 1431 "parse.y"
_T1883->f0=0;_T1883->f1=0U;_T16A=(struct _tuple30*)_T1883;}yyval=Cyc_YY56(_T16A);goto _LL0;case 29:{struct _tuple30*_T1883=_cycalloc(sizeof(struct _tuple30));
# 1432 "parse.y"
_T1883->f0=0;_T16C=yyyvsp;_T16D=_T16C[0];_T16E=_T16D.l;_T16F=_T16E.first_line;_T1883->f1=Cyc_Position_loc_to_seg(_T16F);_T16B=(struct _tuple30*)_T1883;}yyval=Cyc_YY56(_T16B);goto _LL0;case 30:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T1884=_cycalloc(sizeof(struct _tuple33));_T172=yyyvsp;_T173=_T172[0];_T174=_T173.l;_T175=_T174.first_line;
# 1437 "parse.y"
_T1884->f0=Cyc_Position_loc_to_seg(_T175);_T176=yyyvsp + 0;_T177=&(*_T176).v;_T178=(union Cyc_YYSTYPE*)_T177;_T1884->f1=Cyc_yyget_QualId_tok(_T178);_T1884->f2=0;_T171=(struct _tuple33*)_T1884;}_T1883->hd=_T171;_T1883->tl=0;_T170=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY55(_T170);goto _LL0;case 31:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T1884=_cycalloc(sizeof(struct _tuple33));_T17B=yyyvsp;_T17C=_T17B[0];_T17D=_T17C.l;_T17E=_T17D.first_line;
# 1439 "parse.y"
_T1884->f0=Cyc_Position_loc_to_seg(_T17E);_T17F=yyyvsp + 0;_T180=&(*_T17F).v;_T181=(union Cyc_YYSTYPE*)_T180;_T1884->f1=Cyc_yyget_QualId_tok(_T181);_T1884->f2=0;_T17A=(struct _tuple33*)_T1884;}_T1883->hd=_T17A;_T182=yyyvsp + 2;_T183=&(*_T182).v;_T184=(union Cyc_YYSTYPE*)_T183;_T1883->tl=Cyc_yyget_YY55(_T184);_T179=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY55(_T179);goto _LL0;case 32:
# 1443 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 33: _T185=yyyvsp;_T186=_T185[2];
# 1444 "parse.y"
yyval=_T186.v;goto _LL0;case 34:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1884->tag=1;
# 1448 "parse.y"
_T189=yyyvsp + 0;_T18A=&(*_T189).v;_T18B=(union Cyc_YYSTYPE*)_T18A;_T1884->f1=Cyc_yyget_YY15(_T18B);_T188=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1884;}_T18C=(void*)_T188;_T18D=yyyvsp;_T18E=_T18D[0];_T18F=_T18E.l;_T190=_T18F.first_line;_T191=Cyc_Position_loc_to_seg(_T190);_T1883->hd=Cyc_Absyn_new_decl(_T18C,_T191);_T1883->tl=0;_T187=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T187);goto _LL0;case 35: _T192=yyyvsp;_T193=_T192[0];
# 1449 "parse.y"
yyval=_T193.v;goto _LL0;case 36:
# 1450 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 39: _T194=yyr;
# 1484 "parse.y"
_T195=yyyvsp + 0;_T196=&(*_T195).v;_T197=(union Cyc_YYSTYPE*)_T196;_T198=Cyc_yyget_YY28(_T197);_T199=yyyvsp + 1;_T19A=&(*_T199).v;_T19B=(union Cyc_YYSTYPE*)_T19A;_T19C=Cyc_yyget_Stmt_tok(_T19B);_T19D=yyyvsp;_T19E=_T19D[0];_T19F=_T19E.l;_T1A0=_T19F.first_line;_T1A1=Cyc_Position_loc_to_seg(_T1A0);_T1A2=Cyc_Parse_make_function(_T194,0,_T198,0,_T19C,_T1A1);yyval=Cyc_YY15(_T1A2);goto _LL0;case 40:
# 1486 "parse.y"
 _T1A3=yyyvsp + 0;_T1A4=&(*_T1A3).v;_T1A5=(union Cyc_YYSTYPE*)_T1A4;{struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1A5);_T1A6=yyr;_T1A7=& d;_T1A8=(struct Cyc_Parse_Declaration_spec*)_T1A7;
_T1A9=yyyvsp + 1;_T1AA=&(*_T1A9).v;_T1AB=(union Cyc_YYSTYPE*)_T1AA;_T1AC=Cyc_yyget_YY28(_T1AB);_T1AD=yyyvsp + 2;_T1AE=&(*_T1AD).v;_T1AF=(union Cyc_YYSTYPE*)_T1AE;_T1B0=Cyc_yyget_Stmt_tok(_T1AF);_T1B1=yyyvsp;_T1B2=_T1B1[0];_T1B3=_T1B2.l;_T1B4=_T1B3.first_line;_T1B5=Cyc_Position_loc_to_seg(_T1B4);_T1B6=Cyc_Parse_make_function(_T1A6,_T1A8,_T1AC,0,_T1B0,_T1B5);yyval=Cyc_YY15(_T1B6);goto _LL0;}case 41: _T1B7=yyr;
# 1498 "parse.y"
_T1B8=yyyvsp + 0;_T1B9=&(*_T1B8).v;_T1BA=(union Cyc_YYSTYPE*)_T1B9;_T1BB=Cyc_yyget_YY28(_T1BA);_T1BC=yyyvsp + 1;_T1BD=&(*_T1BC).v;_T1BE=(union Cyc_YYSTYPE*)_T1BD;_T1BF=Cyc_yyget_YY16(_T1BE);_T1C0=yyyvsp + 2;_T1C1=&(*_T1C0).v;_T1C2=(union Cyc_YYSTYPE*)_T1C1;_T1C3=Cyc_yyget_Stmt_tok(_T1C2);_T1C4=yyyvsp;_T1C5=_T1C4[0];_T1C6=_T1C5.l;_T1C7=_T1C6.first_line;_T1C8=Cyc_Position_loc_to_seg(_T1C7);_T1C9=Cyc_Parse_make_function(_T1B7,0,_T1BB,_T1BF,_T1C3,_T1C8);yyval=Cyc_YY15(_T1C9);goto _LL0;case 42:
# 1500 "parse.y"
 _T1CA=yyyvsp + 0;_T1CB=&(*_T1CA).v;_T1CC=(union Cyc_YYSTYPE*)_T1CB;{struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1CC);_T1CD=yyr;_T1CE=& d;_T1CF=(struct Cyc_Parse_Declaration_spec*)_T1CE;
_T1D0=yyyvsp + 1;_T1D1=&(*_T1D0).v;_T1D2=(union Cyc_YYSTYPE*)_T1D1;_T1D3=Cyc_yyget_YY28(_T1D2);_T1D4=yyyvsp + 2;_T1D5=&(*_T1D4).v;_T1D6=(union Cyc_YYSTYPE*)_T1D5;_T1D7=Cyc_yyget_YY16(_T1D6);_T1D8=yyyvsp + 3;_T1D9=&(*_T1D8).v;_T1DA=(union Cyc_YYSTYPE*)_T1D9;_T1DB=Cyc_yyget_Stmt_tok(_T1DA);_T1DC=yyyvsp;_T1DD=_T1DC[0];_T1DE=_T1DD.l;_T1DF=_T1DE.first_line;_T1E0=Cyc_Position_loc_to_seg(_T1DF);_T1E1=Cyc_Parse_make_function(_T1CD,_T1CF,_T1D3,_T1D7,_T1DB,_T1E0);yyval=Cyc_YY15(_T1E1);goto _LL0;}case 43:
# 1508 "parse.y"
 _T1E2=yyyvsp + 0;_T1E3=&(*_T1E2).v;_T1E4=(union Cyc_YYSTYPE*)_T1E3;{struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1E4);_T1E5=yyr;_T1E6=& d;_T1E7=(struct Cyc_Parse_Declaration_spec*)_T1E6;
_T1E8=yyyvsp + 1;_T1E9=&(*_T1E8).v;_T1EA=(union Cyc_YYSTYPE*)_T1E9;_T1EB=Cyc_yyget_YY28(_T1EA);_T1EC=yyyvsp + 2;_T1ED=&(*_T1EC).v;_T1EE=(union Cyc_YYSTYPE*)_T1ED;_T1EF=Cyc_yyget_Stmt_tok(_T1EE);_T1F0=yyyvsp;_T1F1=_T1F0[0];_T1F2=_T1F1.l;_T1F3=_T1F2.first_line;_T1F4=Cyc_Position_loc_to_seg(_T1F3);_T1F5=Cyc_Parse_make_function(_T1E5,_T1E7,_T1EB,0,_T1EF,_T1F4);yyval=Cyc_YY15(_T1F5);goto _LL0;}case 44:
# 1511 "parse.y"
 _T1F6=yyyvsp + 0;_T1F7=&(*_T1F6).v;_T1F8=(union Cyc_YYSTYPE*)_T1F7;{struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1F8);_T1F9=yyr;_T1FA=& d;_T1FB=(struct Cyc_Parse_Declaration_spec*)_T1FA;
_T1FC=yyyvsp + 1;_T1FD=&(*_T1FC).v;_T1FE=(union Cyc_YYSTYPE*)_T1FD;_T1FF=Cyc_yyget_YY28(_T1FE);_T200=yyyvsp + 2;_T201=&(*_T200).v;_T202=(union Cyc_YYSTYPE*)_T201;_T203=Cyc_yyget_YY16(_T202);_T204=yyyvsp + 3;_T205=&(*_T204).v;_T206=(union Cyc_YYSTYPE*)_T205;_T207=Cyc_yyget_Stmt_tok(_T206);_T208=yyyvsp;_T209=_T208[0];_T20A=_T209.l;_T20B=_T20A.first_line;_T20C=Cyc_Position_loc_to_seg(_T20B);_T20D=Cyc_Parse_make_function(_T1F9,_T1FB,_T1FF,_T203,_T207,_T20C);yyval=Cyc_YY15(_T20D);goto _LL0;}case 45:
# 1516 "parse.y"
 _T20E=yyyvsp + 1;_T20F=&(*_T20E).v;_T210=(union Cyc_YYSTYPE*)_T20F;_T211=Cyc_yyget_QualId_tok(_T210);Cyc_Lex_enter_using(_T211);_T212=yyyvsp;_T213=_T212[1];yyval=_T213.v;goto _LL0;case 46:
# 1519
 Cyc_Lex_leave_using();goto _LL0;case 47:{struct _fat_ptr*_T1883=_cycalloc(sizeof(struct _fat_ptr));
# 1522
_T215=yyyvsp + 1;_T216=&(*_T215).v;_T217=(union Cyc_YYSTYPE*)_T216;*_T1883=Cyc_yyget_String_tok(_T217);_T214=(struct _fat_ptr*)_T1883;}Cyc_Lex_enter_namespace(_T214);_T218=yyyvsp;_T219=_T218[1];yyval=_T219.v;goto _LL0;case 48:
# 1525
 Cyc_Lex_leave_namespace();goto _LL0;case 49:
# 1528
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + 1;goto _LL0;case 50:
# 1531
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + -1;goto _LL0;case 51:
# 1536 "parse.y"
 _T21A=yyyvsp + 0;_T21B=&(*_T21A).v;_T21C=(union Cyc_YYSTYPE*)_T21B;_T21D=Cyc_yyget_YY17(_T21C);_T21E=yyyvsp;_T21F=_T21E[0];_T220=_T21F.l;_T221=_T220.first_line;_T222=Cyc_Position_loc_to_seg(_T221);_T223=yyyvsp;_T224=_T223[0];_T225=_T224.l;_T226=_T225.first_line;_T227=Cyc_Position_loc_to_seg(_T226);_T228=Cyc_Parse_make_declarations(_T21D,0,_T222,_T227);yyval=Cyc_YY16(_T228);goto _LL0;case 52:
# 1538 "parse.y"
 _T229=yyyvsp + 0;_T22A=&(*_T229).v;_T22B=(union Cyc_YYSTYPE*)_T22A;_T22C=Cyc_yyget_YY17(_T22B);{struct _tuple11*(*_T1883)(struct _tuple11*)=(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;_T22D=_T1883;}_T22E=yyyvsp + 1;_T22F=&(*_T22E).v;_T230=(union Cyc_YYSTYPE*)_T22F;_T231=Cyc_yyget_YY19(_T230);_T232=_T22D(_T231);_T233=yyyvsp;_T234=_T233[0];_T235=_T234.l;_T236=_T235.first_line;_T237=Cyc_Position_loc_to_seg(_T236);_T238=yyyvsp;_T239=_T238[0];_T23A=_T239.l;_T23B=_T23A.first_line;_T23C=Cyc_Position_loc_to_seg(_T23B);_T23D=Cyc_Parse_make_declarations(_T22C,_T232,_T237,_T23C);yyval=Cyc_YY16(_T23D);goto _LL0;case 53:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1541
_T23F=yyyvsp + 1;_T240=&(*_T23F).v;_T241=(union Cyc_YYSTYPE*)_T240;_T242=Cyc_yyget_YY9(_T241);_T243=yyyvsp + 3;_T244=&(*_T243).v;_T245=(union Cyc_YYSTYPE*)_T244;_T246=Cyc_yyget_Exp_tok(_T245);_T247=yyyvsp;_T248=_T247[0];_T249=_T248.l;_T24A=_T249.first_line;_T24B=Cyc_Position_loc_to_seg(_T24A);_T1883->hd=Cyc_Absyn_let_decl(_T242,_T246,_T24B);_T1883->tl=0;_T23E=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T23E);goto _LL0;case 54:  {
# 1543 "parse.y"
struct Cyc_List_List*vds=0;
_T24C=yyyvsp + 1;_T24D=&(*_T24C).v;_T24E=(union Cyc_YYSTYPE*)_T24D;{struct Cyc_List_List*ids=Cyc_yyget_YY37(_T24E);_TL294: if(ids!=0)goto _TL292;else{goto _TL293;}
_TL292:{struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0));_T24F=qv;_T24F->f0=Cyc_Absyn_Rel_n(0);_T250=qv;_T251=ids;_T252=_T251->hd;_T250->f1=(struct _fat_ptr*)_T252;{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));_T254=qv;_T255=
Cyc_Absyn_wildtyp(0);_T1883->hd=Cyc_Absyn_new_vardecl(0U,_T254,_T255,0,0);_T1883->tl=vds;_T253=(struct Cyc_List_List*)_T1883;}vds=_T253;}_T256=ids;
# 1544
ids=_T256->tl;goto _TL294;_TL293:;}{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));_T258=
# 1548
Cyc_List_imp_rev(vds);_T259=yyyvsp;_T25A=_T259[0];_T25B=_T25A.l;_T25C=_T25B.first_line;_T25D=Cyc_Position_loc_to_seg(_T25C);_T1883->hd=Cyc_Absyn_letv_decl(_T258,_T25D);_T1883->tl=0;_T257=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T257);goto _LL0;}case 55:
# 1553 "parse.y"
 _T25E=yyyvsp + 2;_T25F=&(*_T25E).v;_T260=(union Cyc_YYSTYPE*)_T25F;_T261=Cyc_yyget_String_tok(_T260);_T262=yyyvsp;_T263=_T262[2];_T264=_T263.l;_T265=_T264.first_line;_T266=Cyc_Position_loc_to_seg(_T265);Cyc_Parse_tvar_ok(_T261,_T266);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T267=tv;{struct _fat_ptr*_T1883=_cycalloc(sizeof(struct _fat_ptr));_T269=yyyvsp + 2;_T26A=&(*_T269).v;_T26B=(union Cyc_YYSTYPE*)_T26A;*_T1883=Cyc_yyget_String_tok(_T26B);_T268=(struct _fat_ptr*)_T1883;}_T267->name=_T268;_T26C=tv;_T26C->identity=-1;_T26D=tv;_T26E=& Cyc_Kinds_ek;_T26F=(struct Cyc_Absyn_Kind*)_T26E;_T26D->kind=Cyc_Kinds_kind_to_bound(_T26F);_T270=tv;_T270->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T271=yyyvsp;_T272=_T271[4];_T273=_T272.l;_T274=_T273.first_line;_T275=
Cyc_Position_loc_to_seg(_T274);{struct _tuple0*_T1883=_cycalloc(sizeof(struct _tuple0));_T1883->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_T278=yyyvsp + 4;_T279=&(*_T278).v;_T27A=(union Cyc_YYSTYPE*)_T279;*_T1884=Cyc_yyget_String_tok(_T27A);_T277=(struct _fat_ptr*)_T1884;}_T1883->f1=_T277;_T276=(struct _tuple0*)_T1883;}_T27B=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T275,_T276,_T27B,0,0);{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));_T27D=tv;_T27E=vd;_T27F=yyyvsp;_T280=_T27F[0];_T281=_T280.l;_T282=_T281.first_line;_T283=
Cyc_Position_loc_to_seg(_T282);_T1883->hd=Cyc_Absyn_region_decl(_T27D,_T27E,0,_T283);_T1883->tl=0;_T27C=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T27C);goto _LL0;}}}case 56:{struct Cyc_String_pa_PrintArg_struct _T1883;_T1883.tag=0;
# 1561
_T286=yyyvsp + 1;_T287=&(*_T286).v;_T288=(union Cyc_YYSTYPE*)_T287;_T1883.f1=Cyc_yyget_String_tok(_T288);_T285=_T1883;}{struct Cyc_String_pa_PrintArg_struct _T1883=_T285;void*_T1884[1];_T289=_T1884 + 0;*_T289=& _T1883;_T28A=_tag_fat("`%s",sizeof(char),4U);_T28B=_tag_fat(_T1884,sizeof(void*),1);_T284=Cyc_aprintf(_T28A,_T28B);}{struct _fat_ptr tvstring=_T284;
_T28C=yyyvsp + 1;_T28D=&(*_T28C).v;_T28E=(union Cyc_YYSTYPE*)_T28D;_T28F=Cyc_yyget_String_tok(_T28E);_T290=yyyvsp;_T291=_T290[1];_T292=_T291.l;_T293=_T292.first_line;_T294=Cyc_Position_loc_to_seg(_T293);Cyc_Parse_tvar_ok(_T28F,_T294);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T295=tv;{struct _fat_ptr*_T1883=_cycalloc(sizeof(struct _fat_ptr));*_T1883=tvstring;_T296=(struct _fat_ptr*)_T1883;}_T295->name=_T296;_T297=tv;_T297->identity=-1;_T298=tv;_T299=& Cyc_Kinds_ek;_T29A=(struct Cyc_Absyn_Kind*)_T299;_T298->kind=Cyc_Kinds_kind_to_bound(_T29A);_T29B=tv;_T29B->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T29C=yyyvsp;_T29D=_T29C[1];_T29E=_T29D.l;_T29F=_T29E.first_line;_T2A0=
Cyc_Position_loc_to_seg(_T29F);{struct _tuple0*_T1883=_cycalloc(sizeof(struct _tuple0));_T1883->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_T2A3=yyyvsp + 1;_T2A4=&(*_T2A3).v;_T2A5=(union Cyc_YYSTYPE*)_T2A4;*_T1884=Cyc_yyget_String_tok(_T2A5);_T2A2=(struct _fat_ptr*)_T1884;}_T1883->f1=_T2A2;_T2A1=(struct _tuple0*)_T1883;}_T2A6=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T2A0,_T2A1,_T2A6,0,0);{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));_T2A8=tv;_T2A9=vd;
_T2AA=yyyvsp + 2;_T2AB=&(*_T2AA).v;_T2AC=(union Cyc_YYSTYPE*)_T2AB;_T2AD=Cyc_yyget_YY61(_T2AC);_T2AE=yyyvsp;_T2AF=_T2AE[0];_T2B0=_T2AF.l;_T2B1=_T2B0.first_line;_T2B2=Cyc_Position_loc_to_seg(_T2B1);_T1883->hd=Cyc_Absyn_region_decl(_T2A8,_T2A9,_T2AD,_T2B2);_T1883->tl=0;_T2A7=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY16(_T2A7);goto _LL0;}}}}case 57:
# 1571 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 58:
# 1573 "parse.y"
 _T2B3=yyyvsp + 1;_T2B4=&(*_T2B3).v;_T2B5=(union Cyc_YYSTYPE*)_T2B4;_T2B6=Cyc_yyget_String_tok(_T2B5);_T2B7=_tag_fat("open",sizeof(char),5U);_T2B8=Cyc_strcmp(_T2B6,_T2B7);if(_T2B8==0)goto _TL295;_T2B9=yyyvsp;_T2BA=_T2B9[3];_T2BB=_T2BA.l;_T2BC=_T2BB.first_line;_T2BD=Cyc_Position_loc_to_seg(_T2BC);_T2BE=_tag_fat("expecting `open'",sizeof(char),17U);_T2BF=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2BD,_T2BE,_T2BF);goto _TL296;_TL295: _TL296:
 _T2C0=yyyvsp + 3;_T2C1=&(*_T2C0).v;_T2C2=(union Cyc_YYSTYPE*)_T2C1;_T2C3=Cyc_yyget_Exp_tok(_T2C2);yyval=Cyc_YY61(_T2C3);goto _LL0;case 59: _T2C4=yyyvsp;_T2C5=_T2C4[0];
# 1577
yyval=_T2C5.v;goto _LL0;case 60:
# 1578 "parse.y"
 _T2C6=yyyvsp + 0;_T2C7=&(*_T2C6).v;_T2C8=(union Cyc_YYSTYPE*)_T2C7;_T2C9=Cyc_yyget_YY16(_T2C8);_T2CA=yyyvsp + 1;_T2CB=&(*_T2CA).v;_T2CC=(union Cyc_YYSTYPE*)_T2CB;_T2CD=Cyc_yyget_YY16(_T2CC);_T2CE=Cyc_List_imp_append(_T2C9,_T2CD);yyval=Cyc_YY16(_T2CE);goto _LL0;case 61:{struct Cyc_Parse_Declaration_spec _T1883;
# 1584 "parse.y"
_T2D0=yyyvsp + 0;_T2D1=&(*_T2D0).v;_T2D2=(union Cyc_YYSTYPE*)_T2D1;_T1883.sc=Cyc_yyget_YY20(_T2D2);_T2D3=yyyvsp;_T2D4=_T2D3[0];_T2D5=_T2D4.l;_T2D6=_T2D5.first_line;_T2D7=Cyc_Position_loc_to_seg(_T2D6);_T1883.tq=Cyc_Absyn_empty_tqual(_T2D7);
_T1883.type_specs=Cyc_Parse_empty_spec(0U);_T1883.is_inline=0;_T1883.attributes=0;_T2CF=_T1883;}
# 1584
yyval=Cyc_YY17(_T2CF);goto _LL0;case 62:
# 1587 "parse.y"
 _T2D8=yyyvsp + 1;_T2D9=&(*_T2D8).v;_T2DA=(union Cyc_YYSTYPE*)_T2D9;{struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T2DA);_T2DB=two;_T2DC=_T2DB.sc;_T2DD=(int)_T2DC;
if(_T2DD==7)goto _TL297;_T2DE=yyyvsp;_T2DF=_T2DE[0];_T2E0=_T2DF.l;_T2E1=_T2E0.first_line;_T2E2=
Cyc_Position_loc_to_seg(_T2E1);_T2E3=
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U);_T2E4=_tag_fat(0U,sizeof(void*),0);
# 1589
Cyc_Warn_warn(_T2E2,_T2E3,_T2E4);goto _TL298;_TL297: _TL298:{struct Cyc_Parse_Declaration_spec _T1883;
# 1591
_T2E6=yyyvsp + 0;_T2E7=&(*_T2E6).v;_T2E8=(union Cyc_YYSTYPE*)_T2E7;_T1883.sc=Cyc_yyget_YY20(_T2E8);_T2E9=two;_T1883.tq=_T2E9.tq;_T2EA=two;_T1883.type_specs=_T2EA.type_specs;_T2EB=two;
_T1883.is_inline=_T2EB.is_inline;_T2EC=two;_T1883.attributes=_T2EC.attributes;_T2E5=_T1883;}
# 1591
yyval=Cyc_YY17(_T2E5);goto _LL0;}case 63: _T2ED=yyyvsp;_T2EE=_T2ED[0];_T2EF=_T2EE.l;_T2F0=_T2EF.first_line;_T2F1=
# 1595 "parse.y"
Cyc_Position_loc_to_seg(_T2F0);_T2F2=_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U);_T2F3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T2F1,_T2F2,_T2F3);_T2F4=yyyvsp;_T2F5=_T2F4[1];
yyval=_T2F5.v;goto _LL0;case 64:{struct Cyc_Parse_Declaration_spec _T1883;
# 1599 "parse.y"
_T1883.sc=7U;_T2F7=yyyvsp;_T2F8=_T2F7[0];_T2F9=_T2F8.l;_T2FA=_T2F9.first_line;_T2FB=Cyc_Position_loc_to_seg(_T2FA);_T1883.tq=Cyc_Absyn_empty_tqual(_T2FB);
_T2FC=yyyvsp + 0;_T2FD=&(*_T2FC).v;_T2FE=(union Cyc_YYSTYPE*)_T2FD;_T1883.type_specs=Cyc_yyget_YY21(_T2FE);_T1883.is_inline=0;_T1883.attributes=0;_T2F6=_T1883;}
# 1599
yyval=Cyc_YY17(_T2F6);goto _LL0;case 65:
# 1602 "parse.y"
 _T2FF=yyyvsp + 1;_T300=&(*_T2FF).v;_T301=(union Cyc_YYSTYPE*)_T300;{struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T301);{struct Cyc_Parse_Declaration_spec _T1883;_T303=two;
_T1883.sc=_T303.sc;_T304=two;_T1883.tq=_T304.tq;_T305=yyyvsp;_T306=_T305[0];_T307=_T306.l;_T308=_T307.first_line;_T309=
Cyc_Position_loc_to_seg(_T308);_T30A=two;_T30B=_T30A.type_specs;_T30C=yyyvsp + 0;_T30D=&(*_T30C).v;_T30E=(union Cyc_YYSTYPE*)_T30D;_T30F=Cyc_yyget_YY21(_T30E);_T1883.type_specs=Cyc_Parse_combine_specifiers(_T309,_T30B,_T30F);_T310=two;
_T1883.is_inline=_T310.is_inline;_T311=two;_T1883.attributes=_T311.attributes;_T302=_T1883;}
# 1603
yyval=Cyc_YY17(_T302);goto _LL0;}case 66:{struct Cyc_Parse_Declaration_spec _T1883;
# 1608 "parse.y"
_T1883.sc=7U;_T313=yyyvsp + 0;_T314=&(*_T313).v;_T315=(union Cyc_YYSTYPE*)_T314;_T1883.tq=Cyc_yyget_YY24(_T315);_T1883.type_specs=Cyc_Parse_empty_spec(0U);_T1883.is_inline=0;_T1883.attributes=0;_T312=_T1883;}yyval=Cyc_YY17(_T312);goto _LL0;case 67:
# 1610 "parse.y"
 _T316=yyyvsp + 1;_T317=&(*_T316).v;_T318=(union Cyc_YYSTYPE*)_T317;{struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T318);{struct Cyc_Parse_Declaration_spec _T1883;_T31A=two;
_T1883.sc=_T31A.sc;_T31B=yyyvsp + 0;_T31C=&(*_T31B).v;_T31D=(union Cyc_YYSTYPE*)_T31C;_T31E=Cyc_yyget_YY24(_T31D);_T31F=two;_T320=_T31F.tq;_T1883.tq=Cyc_Absyn_combine_tqual(_T31E,_T320);_T321=two;
_T1883.type_specs=_T321.type_specs;_T322=two;_T1883.is_inline=_T322.is_inline;_T323=two;_T1883.attributes=_T323.attributes;_T319=_T1883;}
# 1611
yyval=Cyc_YY17(_T319);goto _LL0;}case 68:{struct Cyc_Parse_Declaration_spec _T1883;
# 1615 "parse.y"
_T1883.sc=7U;_T325=yyyvsp;_T326=_T325[0];_T327=_T326.l;_T328=_T327.first_line;_T329=Cyc_Position_loc_to_seg(_T328);_T1883.tq=Cyc_Absyn_empty_tqual(_T329);
_T1883.type_specs=Cyc_Parse_empty_spec(0U);_T1883.is_inline=1;_T1883.attributes=0;_T324=_T1883;}
# 1615
yyval=Cyc_YY17(_T324);goto _LL0;case 69:
# 1618 "parse.y"
 _T32A=yyyvsp + 1;_T32B=&(*_T32A).v;_T32C=(union Cyc_YYSTYPE*)_T32B;{struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T32C);{struct Cyc_Parse_Declaration_spec _T1883;_T32E=two;
_T1883.sc=_T32E.sc;_T32F=two;_T1883.tq=_T32F.tq;_T330=two;_T1883.type_specs=_T330.type_specs;_T1883.is_inline=1;_T331=two;_T1883.attributes=_T331.attributes;_T32D=_T1883;}yyval=Cyc_YY17(_T32D);goto _LL0;}case 70:{struct Cyc_Parse_Declaration_spec _T1883;
# 1622 "parse.y"
_T1883.sc=7U;_T333=yyyvsp;_T334=_T333[0];_T335=_T334.l;_T336=_T335.first_line;_T337=Cyc_Position_loc_to_seg(_T336);_T1883.tq=Cyc_Absyn_empty_tqual(_T337);
_T1883.type_specs=Cyc_Parse_empty_spec(0U);_T1883.is_inline=0;_T338=yyyvsp + 0;_T339=&(*_T338).v;_T33A=(union Cyc_YYSTYPE*)_T339;_T1883.attributes=Cyc_yyget_YY46(_T33A);_T332=_T1883;}
# 1622
yyval=Cyc_YY17(_T332);goto _LL0;case 71:
# 1625 "parse.y"
 _T33B=yyyvsp + 1;_T33C=&(*_T33B).v;_T33D=(union Cyc_YYSTYPE*)_T33C;{struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T33D);{struct Cyc_Parse_Declaration_spec _T1883;_T33F=two;
_T1883.sc=_T33F.sc;_T340=two;_T1883.tq=_T340.tq;_T341=two;
_T1883.type_specs=_T341.type_specs;_T342=two;_T1883.is_inline=_T342.is_inline;
_T343=yyyvsp + 0;_T344=&(*_T343).v;_T345=(union Cyc_YYSTYPE*)_T344;_T346=Cyc_yyget_YY46(_T345);_T347=two;_T348=_T347.attributes;_T1883.attributes=Cyc_List_imp_append(_T346,_T348);_T33E=_T1883;}
# 1626
yyval=Cyc_YY17(_T33E);goto _LL0;}case 72:
# 1632 "parse.y"
 yyval=Cyc_YY20(4U);goto _LL0;case 73:
# 1633 "parse.y"
 yyval=Cyc_YY20(5U);goto _LL0;case 74:
# 1634 "parse.y"
 yyval=Cyc_YY20(3U);goto _LL0;case 75:
# 1635 "parse.y"
 yyval=Cyc_YY20(1U);goto _LL0;case 76:
# 1637 "parse.y"
 _T349=yyyvsp + 1;_T34A=&(*_T349).v;_T34B=(union Cyc_YYSTYPE*)_T34A;_T34C=Cyc_yyget_String_tok(_T34B);_T34D=_tag_fat("C",sizeof(char),2U);_T34E=Cyc_strcmp(_T34C,_T34D);if(_T34E==0)goto _TL299;_T34F=yyyvsp;_T350=_T34F[0];_T351=_T350.l;_T352=_T351.first_line;_T353=
Cyc_Position_loc_to_seg(_T352);_T354=_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U);_T355=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T353,_T354,_T355);goto _TL29A;_TL299: _TL29A:
 yyval=Cyc_YY20(2U);goto _LL0;case 77:
# 1641 "parse.y"
 yyval=Cyc_YY20(0U);goto _LL0;case 78:
# 1643 "parse.y"
 yyval=Cyc_YY20(6U);goto _LL0;case 79:
# 1648 "parse.y"
 yyval=Cyc_YY46(0);goto _LL0;case 80: _T356=yyyvsp;_T357=_T356[0];
# 1649 "parse.y"
yyval=_T357.v;goto _LL0;case 81: _T358=yyyvsp;_T359=_T358[3];
# 1653 "parse.y"
yyval=_T359.v;goto _LL0;case 82:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1656
_T35B=yyyvsp + 0;_T35C=&(*_T35B).v;_T35D=(union Cyc_YYSTYPE*)_T35C;_T1883->hd=Cyc_yyget_YY47(_T35D);_T1883->tl=0;_T35A=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY46(_T35A);goto _LL0;case 83:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1657 "parse.y"
_T35F=yyyvsp + 0;_T360=&(*_T35F).v;_T361=(union Cyc_YYSTYPE*)_T360;_T1883->hd=Cyc_yyget_YY47(_T361);_T362=yyyvsp + 2;_T363=&(*_T362).v;_T364=(union Cyc_YYSTYPE*)_T363;_T1883->tl=Cyc_yyget_YY46(_T364);_T35E=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY46(_T35E);goto _LL0;case 84: _T365=yyyvsp;_T366=_T365[0];_T367=_T366.l;_T368=_T367.first_line;_T369=
# 1660
Cyc_Position_loc_to_seg(_T368);_T36A=yyyvsp + 0;_T36B=&(*_T36A).v;_T36C=(union Cyc_YYSTYPE*)_T36B;_T36D=Cyc_yyget_String_tok(_T36C);_T36E=Cyc_Atts_parse_nullary_att(_T369,_T36D);yyval=Cyc_YY47(_T36E);goto _LL0;case 85: _T36F=& Cyc_Atts_Const_att_val;_T370=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_T36F;_T371=(void*)_T370;
# 1661 "parse.y"
yyval=Cyc_YY47(_T371);goto _LL0;case 86: _T372=yyyvsp;_T373=_T372[0];_T374=_T373.l;_T375=_T374.first_line;_T376=
# 1663 "parse.y"
Cyc_Position_loc_to_seg(_T375);_T377=yyyvsp + 0;_T378=&(*_T377).v;_T379=(union Cyc_YYSTYPE*)_T378;_T37A=Cyc_yyget_String_tok(_T379);_T37B=yyyvsp;_T37C=_T37B[2];_T37D=_T37C.l;_T37E=_T37D.first_line;_T37F=Cyc_Position_loc_to_seg(_T37E);_T380=yyyvsp + 2;_T381=&(*_T380).v;_T382=(union Cyc_YYSTYPE*)_T381;_T383=Cyc_yyget_Exp_tok(_T382);_T384=Cyc_Atts_parse_unary_att(_T376,_T37A,_T37F,_T383);yyval=Cyc_YY47(_T384);goto _LL0;case 87: _T385=yyyvsp;_T386=_T385[0];_T387=_T386.l;_T388=_T387.first_line;_T389=
# 1665 "parse.y"
Cyc_Position_loc_to_seg(_T388);_T38A=yyyvsp;_T38B=_T38A[2];_T38C=_T38B.l;_T38D=_T38C.first_line;_T38E=Cyc_Position_loc_to_seg(_T38D);_T38F=yyyvsp + 0;_T390=&(*_T38F).v;_T391=(union Cyc_YYSTYPE*)_T390;_T392=Cyc_yyget_String_tok(_T391);_T393=yyyvsp + 2;_T394=&(*_T393).v;_T395=(union Cyc_YYSTYPE*)_T394;_T396=Cyc_yyget_String_tok(_T395);_T397=yyyvsp;_T398=_T397[4];_T399=_T398.l;_T39A=_T399.first_line;_T39B=
Cyc_Position_loc_to_seg(_T39A);_T39C=yyyvsp + 4;_T39D=&(*_T39C).v;_T39E=(union Cyc_YYSTYPE*)_T39D;_T39F=Cyc_yyget_Int_tok(_T39E);_T3A0=Cyc_Parse_cnst2uint(_T39B,_T39F);_T3A1=yyyvsp;_T3A2=_T3A1[6];_T3A3=_T3A2.l;_T3A4=_T3A3.first_line;_T3A5=
Cyc_Position_loc_to_seg(_T3A4);_T3A6=yyyvsp + 6;_T3A7=&(*_T3A6).v;_T3A8=(union Cyc_YYSTYPE*)_T3A7;_T3A9=Cyc_yyget_Int_tok(_T3A8);_T3AA=Cyc_Parse_cnst2uint(_T3A5,_T3A9);_T3AB=
# 1665
Cyc_Atts_parse_format_att(_T389,_T38E,_T392,_T396,_T3A0,_T3AA);yyval=Cyc_YY47(_T3AB);goto _LL0;case 88: _T3AC=yyyvsp;_T3AD=_T3AC[0];
# 1677 "parse.y"
yyval=_T3AD.v;goto _LL0;case 89:
# 1679 "parse.y"
 _T3AE=yyyvsp + 0;_T3AF=&(*_T3AE).v;_T3B0=(union Cyc_YYSTYPE*)_T3AF;_T3B1=Cyc_yyget_QualId_tok(_T3B0);_T3B2=yyyvsp + 1;_T3B3=&(*_T3B2).v;_T3B4=(union Cyc_YYSTYPE*)_T3B3;_T3B5=Cyc_yyget_YY41(_T3B4);_T3B6=Cyc_Absyn_typedef_type(_T3B1,_T3B5,0,0);_T3B7=yyyvsp;_T3B8=_T3B7[0];_T3B9=_T3B8.l;_T3BA=_T3B9.first_line;_T3BB=Cyc_Position_loc_to_seg(_T3BA);_T3BC=Cyc_Parse_type_spec(_T3B6,_T3BB);yyval=Cyc_YY21(_T3BC);goto _LL0;case 90: _T3BD=Cyc_Absyn_void_type;_T3BE=yyyvsp;_T3BF=_T3BE[0];_T3C0=_T3BF.l;_T3C1=_T3C0.first_line;_T3C2=
# 1683 "parse.y"
Cyc_Position_loc_to_seg(_T3C1);_T3C3=Cyc_Parse_type_spec(_T3BD,_T3C2);yyval=Cyc_YY21(_T3C3);goto _LL0;case 91: _T3C4=Cyc_Absyn_char_type;_T3C5=yyyvsp;_T3C6=_T3C5[0];_T3C7=_T3C6.l;_T3C8=_T3C7.first_line;_T3C9=
# 1684 "parse.y"
Cyc_Position_loc_to_seg(_T3C8);_T3CA=Cyc_Parse_type_spec(_T3C4,_T3C9);yyval=Cyc_YY21(_T3CA);goto _LL0;case 92: _T3CB=yyyvsp;_T3CC=_T3CB[0];_T3CD=_T3CC.l;_T3CE=_T3CD.first_line;_T3CF=
# 1685 "parse.y"
Cyc_Position_loc_to_seg(_T3CE);_T3D0=Cyc_Parse_short_spec(_T3CF);yyval=Cyc_YY21(_T3D0);goto _LL0;case 93: _T3D1=Cyc_Absyn_sint_type;_T3D2=yyyvsp;_T3D3=_T3D2[0];_T3D4=_T3D3.l;_T3D5=_T3D4.first_line;_T3D6=
# 1686 "parse.y"
Cyc_Position_loc_to_seg(_T3D5);_T3D7=Cyc_Parse_type_spec(_T3D1,_T3D6);yyval=Cyc_YY21(_T3D7);goto _LL0;case 94: _T3D8=yyyvsp;_T3D9=_T3D8[0];_T3DA=_T3D9.l;_T3DB=_T3DA.first_line;_T3DC=
# 1687 "parse.y"
Cyc_Position_loc_to_seg(_T3DB);_T3DD=Cyc_Parse_long_spec(_T3DC);yyval=Cyc_YY21(_T3DD);goto _LL0;case 95: _T3DE=Cyc_Absyn_float_type;_T3DF=yyyvsp;_T3E0=_T3DF[0];_T3E1=_T3E0.l;_T3E2=_T3E1.first_line;_T3E3=
# 1688 "parse.y"
Cyc_Position_loc_to_seg(_T3E2);_T3E4=Cyc_Parse_type_spec(_T3DE,_T3E3);yyval=Cyc_YY21(_T3E4);goto _LL0;case 96: _T3E5=Cyc_Absyn_double_type;_T3E6=yyyvsp;_T3E7=_T3E6[0];_T3E8=_T3E7.l;_T3E9=_T3E8.first_line;_T3EA=
# 1689 "parse.y"
Cyc_Position_loc_to_seg(_T3E9);_T3EB=Cyc_Parse_type_spec(_T3E5,_T3EA);yyval=Cyc_YY21(_T3EB);goto _LL0;case 97: _T3EC=yyyvsp;_T3ED=_T3EC[0];_T3EE=_T3ED.l;_T3EF=_T3EE.first_line;_T3F0=
# 1690 "parse.y"
Cyc_Position_loc_to_seg(_T3EF);_T3F1=Cyc_Parse_signed_spec(_T3F0);yyval=Cyc_YY21(_T3F1);goto _LL0;case 98: _T3F2=yyyvsp;_T3F3=_T3F2[0];_T3F4=_T3F3.l;_T3F5=_T3F4.first_line;_T3F6=
# 1691 "parse.y"
Cyc_Position_loc_to_seg(_T3F5);_T3F7=Cyc_Parse_unsigned_spec(_T3F6);yyval=Cyc_YY21(_T3F7);goto _LL0;case 99: _T3F8=yyyvsp;_T3F9=_T3F8[0];_T3FA=_T3F9.l;_T3FB=_T3FA.first_line;_T3FC=
# 1692 "parse.y"
Cyc_Position_loc_to_seg(_T3FB);_T3FD=Cyc_Parse_complex_spec(_T3FC);yyval=Cyc_YY21(_T3FD);goto _LL0;case 100: _T3FE=yyyvsp;_T3FF=_T3FE[0];
# 1693 "parse.y"
yyval=_T3FF.v;goto _LL0;case 101: _T400=yyyvsp;_T401=_T400[0];
# 1694 "parse.y"
yyval=_T401.v;goto _LL0;case 102:
# 1696 "parse.y"
 _T402=yyyvsp + 2;_T403=&(*_T402).v;_T404=(union Cyc_YYSTYPE*)_T403;_T405=Cyc_yyget_Exp_tok(_T404);_T406=Cyc_Absyn_typeof_type(_T405);_T407=yyyvsp;_T408=_T407[0];_T409=_T408.l;_T40A=_T409.first_line;_T40B=Cyc_Position_loc_to_seg(_T40A);_T40C=Cyc_Parse_type_spec(_T406,_T40B);yyval=Cyc_YY21(_T40C);goto _LL0;case 103: _T40D=
# 1698 "parse.y"
_tag_fat("__builtin_va_list",sizeof(char),18U);_T40E=& Cyc_Kinds_bk;_T40F=(struct Cyc_Absyn_Kind*)_T40E;_T410=Cyc_Absyn_builtin_type(_T40D,_T40F);_T411=yyyvsp;_T412=_T411[0];_T413=_T412.l;_T414=_T413.first_line;_T415=Cyc_Position_loc_to_seg(_T414);_T416=Cyc_Parse_type_spec(_T410,_T415);yyval=Cyc_YY21(_T416);goto _LL0;case 104: _T417=yyyvsp;_T418=_T417[0];
# 1700 "parse.y"
yyval=_T418.v;goto _LL0;case 105:
# 1702 "parse.y"
 _T419=yyyvsp + 0;_T41A=&(*_T419).v;_T41B=(union Cyc_YYSTYPE*)_T41A;_T41C=Cyc_yyget_YY45(_T41B);_T41D=yyyvsp;_T41E=_T41D[0];_T41F=_T41E.l;_T420=_T41F.first_line;_T421=Cyc_Position_loc_to_seg(_T420);_T422=Cyc_Parse_type_spec(_T41C,_T421);yyval=Cyc_YY21(_T422);goto _LL0;case 106: _T423=
# 1704 "parse.y"
Cyc_Absyn_new_evar(0,0);_T424=yyyvsp;_T425=_T424[0];_T426=_T425.l;_T427=_T426.first_line;_T428=Cyc_Position_loc_to_seg(_T427);_T429=Cyc_Parse_type_spec(_T423,_T428);yyval=Cyc_YY21(_T429);goto _LL0;case 107:
# 1706 "parse.y"
 _T42A=yyyvsp + 2;_T42B=&(*_T42A).v;_T42C=(union Cyc_YYSTYPE*)_T42B;_T42D=Cyc_yyget_YY44(_T42C);_T42E=Cyc_Kinds_kind_to_opt(_T42D);_T42F=Cyc_Absyn_new_evar(_T42E,0);_T430=yyyvsp;_T431=_T430[0];_T432=_T431.l;_T433=_T432.first_line;_T434=Cyc_Position_loc_to_seg(_T433);_T435=Cyc_Parse_type_spec(_T42F,_T434);yyval=Cyc_YY21(_T435);goto _LL0;case 108: _T437=Cyc_List_map_c;{
# 1708 "parse.y"
struct Cyc_List_List*(*_T1883)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T437;_T436=_T1883;}{struct _tuple19*(*_T1883)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T438=_T1883;}_T439=yyyvsp;_T43A=_T439[2];_T43B=_T43A.l;_T43C=_T43B.first_line;_T43D=Cyc_Position_loc_to_seg(_T43C);_T43E=yyyvsp + 2;_T43F=&(*_T43E).v;_T440=(union Cyc_YYSTYPE*)_T43F;_T441=Cyc_yyget_YY39(_T440);_T442=Cyc_List_imp_rev(_T441);_T443=_T436(_T438,_T43D,_T442);_T444=Cyc_Absyn_tuple_type(_T443);_T445=yyyvsp;_T446=_T445[0];_T447=_T446.l;_T448=_T447.first_line;_T449=
Cyc_Position_loc_to_seg(_T448);_T44A=
# 1708
Cyc_Parse_type_spec(_T444,_T449);yyval=Cyc_YY21(_T44A);goto _LL0;case 109:
# 1711 "parse.y"
 _T44B=yyyvsp + 2;_T44C=&(*_T44B).v;_T44D=(union Cyc_YYSTYPE*)_T44C;_T44E=Cyc_yyget_YY45(_T44D);_T44F=Cyc_Absyn_rgn_handle_type(_T44E);_T450=yyyvsp;_T451=_T450[0];_T452=_T451.l;_T453=_T452.first_line;_T454=Cyc_Position_loc_to_seg(_T453);_T455=Cyc_Parse_type_spec(_T44F,_T454);yyval=Cyc_YY21(_T455);goto _LL0;case 110: _T456=& Cyc_Kinds_eko;_T457=(struct Cyc_Core_Opt*)_T456;_T458=
# 1713 "parse.y"
Cyc_Absyn_new_evar(_T457,0);_T459=Cyc_Absyn_rgn_handle_type(_T458);_T45A=yyyvsp;_T45B=_T45A[0];_T45C=_T45B.l;_T45D=_T45C.first_line;_T45E=Cyc_Position_loc_to_seg(_T45D);_T45F=Cyc_Parse_type_spec(_T459,_T45E);yyval=Cyc_YY21(_T45F);goto _LL0;case 111:
# 1715 "parse.y"
 _T460=yyyvsp + 2;_T461=&(*_T460).v;_T462=(union Cyc_YYSTYPE*)_T461;_T463=Cyc_yyget_YY58(_T462);_T464=Cyc_Absyn_aqual_handle_type(_T463);_T465=yyyvsp;_T466=_T465[0];_T467=_T466.l;_T468=_T467.first_line;_T469=Cyc_Position_loc_to_seg(_T468);_T46A=Cyc_Parse_type_spec(_T464,_T469);yyval=Cyc_YY21(_T46A);goto _LL0;case 112:
# 1717 "parse.y"
 _T46B=yyyvsp + 2;_T46C=&(*_T46B).v;_T46D=(union Cyc_YYSTYPE*)_T46C;_T46E=Cyc_yyget_YY45(_T46D);_T46F=Cyc_Absyn_tag_type(_T46E);_T470=yyyvsp;_T471=_T470[0];_T472=_T471.l;_T473=_T472.first_line;_T474=Cyc_Position_loc_to_seg(_T473);_T475=Cyc_Parse_type_spec(_T46F,_T474);yyval=Cyc_YY21(_T475);goto _LL0;case 113: _T476=& Cyc_Kinds_iko;_T477=(struct Cyc_Core_Opt*)_T476;_T478=
# 1719 "parse.y"
Cyc_Absyn_new_evar(_T477,0);_T479=Cyc_Absyn_tag_type(_T478);_T47A=yyyvsp;_T47B=_T47A[0];_T47C=_T47B.l;_T47D=_T47C.first_line;_T47E=Cyc_Position_loc_to_seg(_T47D);_T47F=Cyc_Parse_type_spec(_T479,_T47E);yyval=Cyc_YY21(_T47F);goto _LL0;case 114:
# 1721 "parse.y"
 _T480=yyyvsp + 2;_T481=&(*_T480).v;_T482=(union Cyc_YYSTYPE*)_T481;_T483=Cyc_yyget_Exp_tok(_T482);_T484=Cyc_Absyn_valueof_type(_T483);_T485=yyyvsp;_T486=_T485[0];_T487=_T486.l;_T488=_T487.first_line;_T489=Cyc_Position_loc_to_seg(_T488);_T48A=Cyc_Parse_type_spec(_T484,_T489);yyval=Cyc_YY21(_T48A);goto _LL0;case 115:
# 1726 "parse.y"
 _T48B=yyyvsp + 0;_T48C=&(*_T48B).v;_T48D=(union Cyc_YYSTYPE*)_T48C;_T48E=Cyc_yyget_String_tok(_T48D);_T48F=yyyvsp;_T490=_T48F[0];_T491=_T490.l;_T492=_T491.first_line;_T493=Cyc_Position_loc_to_seg(_T492);_T494=Cyc_Kinds_id_to_kind(_T48E,_T493);yyval=Cyc_YY44(_T494);goto _LL0;case 116: _T496=Cyc_Flags_porting_c_code;
# 1730 "parse.y"
if(!_T496)goto _TL29B;_T497=yyyvsp;_T498=_T497[0];_T499=_T498.l;_T49A=_T499.first_line;_T495=Cyc_Position_loc_to_seg(_T49A);goto _TL29C;_TL29B: _T495=0U;_TL29C: {unsigned loc=_T495;{struct Cyc_Absyn_Tqual _T1883;
_T1883.print_const=1;_T1883.q_volatile=0;_T1883.q_restrict=0;_T1883.real_const=1;_T1883.loc=loc;_T49B=_T1883;}yyval=Cyc_YY24(_T49B);goto _LL0;}case 117:{struct Cyc_Absyn_Tqual _T1883;
# 1732 "parse.y"
_T1883.print_const=0;_T1883.q_volatile=1;_T1883.q_restrict=0;_T1883.real_const=0;_T1883.loc=0U;_T49C=_T1883;}yyval=Cyc_YY24(_T49C);goto _LL0;case 118:{struct Cyc_Absyn_Tqual _T1883;
# 1733 "parse.y"
_T1883.print_const=0;_T1883.q_volatile=0;_T1883.q_restrict=1;_T1883.real_const=0;_T1883.loc=0U;_T49D=_T1883;}yyval=Cyc_YY24(_T49D);goto _LL0;case 119:  {
# 1739 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl));_T49E=ed;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_T1883->tag=1;{struct Cyc_Absyn_Enumdecl*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T1884->sc=2U;_T4A1=yyyvsp + 1;_T4A2=&(*_T4A1).v;_T4A3=(union Cyc_YYSTYPE*)_T4A2;_T1884->name=Cyc_yyget_QualId_tok(_T4A3);{struct Cyc_Core_Opt*_T1885=_cycalloc(sizeof(struct Cyc_Core_Opt));_T4A5=yyyvsp + 3;_T4A6=&(*_T4A5).v;_T4A7=(union Cyc_YYSTYPE*)_T4A6;_T1885->v=Cyc_yyget_YY49(_T4A7);_T4A4=(struct Cyc_Core_Opt*)_T1885;}_T1884->fields=_T4A4;_T4A0=(struct Cyc_Absyn_Enumdecl*)_T1884;}_T1883->f1=_T4A0;_T49F=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T1883;}_T49E->r=(void*)_T49F;_T4A8=ed;_T4A9=yyyvsp;_T4AA=_T4A9[0];_T4AB=_T4AA.l;_T4AC=_T4AB.first_line;
_T4A8->loc=Cyc_Position_loc_to_seg(_T4AC);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T1883->tag=10;
_T1883->f1=ed;_T1883->f2=0;_T4AD=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1883;}_T4AE=(void*)_T4AD;_T4AF=yyyvsp;_T4B0=_T4AF[0];_T4B1=_T4B0.l;_T4B2=_T4B1.first_line;_T4B3=Cyc_Position_loc_to_seg(_T4B2);_T4B4=Cyc_Parse_type_spec(_T4AE,_T4B3);yyval=Cyc_YY21(_T4B4);goto _LL0;}case 120:
# 1744 "parse.y"
 _T4B5=yyyvsp + 1;_T4B6=&(*_T4B5).v;_T4B7=(union Cyc_YYSTYPE*)_T4B6;_T4B8=Cyc_yyget_QualId_tok(_T4B7);_T4B9=Cyc_Absyn_enum_type(_T4B8,0);_T4BA=yyyvsp;_T4BB=_T4BA[0];_T4BC=_T4BB.l;_T4BD=_T4BC.first_line;_T4BE=Cyc_Position_loc_to_seg(_T4BD);_T4BF=Cyc_Parse_type_spec(_T4B9,_T4BE);yyval=Cyc_YY21(_T4BF);goto _LL0;case 121:
# 1746 "parse.y"
 _T4C0=yyyvsp + 2;_T4C1=&(*_T4C0).v;_T4C2=(union Cyc_YYSTYPE*)_T4C1;_T4C3=Cyc_yyget_YY49(_T4C2);_T4C4=Cyc_Absyn_anon_enum_type(_T4C3);_T4C5=yyyvsp;_T4C6=_T4C5[0];_T4C7=_T4C6.l;_T4C8=_T4C7.first_line;_T4C9=Cyc_Position_loc_to_seg(_T4C8);_T4CA=Cyc_Parse_type_spec(_T4C4,_T4C9);yyval=Cyc_YY21(_T4CA);goto _LL0;case 122:{struct Cyc_Absyn_Enumfield*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));
# 1752 "parse.y"
_T4CC=yyyvsp + 0;_T4CD=&(*_T4CC).v;_T4CE=(union Cyc_YYSTYPE*)_T4CD;_T1883->name=Cyc_yyget_QualId_tok(_T4CE);_T1883->tag=0;_T4CF=yyyvsp;_T4D0=_T4CF[0];_T4D1=_T4D0.l;_T4D2=_T4D1.first_line;_T1883->loc=Cyc_Position_loc_to_seg(_T4D2);_T4CB=(struct Cyc_Absyn_Enumfield*)_T1883;}yyval=Cyc_YY48(_T4CB);goto _LL0;case 123:{struct Cyc_Absyn_Enumfield*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));
# 1754 "parse.y"
_T4D4=yyyvsp + 0;_T4D5=&(*_T4D4).v;_T4D6=(union Cyc_YYSTYPE*)_T4D5;_T1883->name=Cyc_yyget_QualId_tok(_T4D6);_T4D7=yyyvsp + 2;_T4D8=&(*_T4D7).v;_T4D9=(union Cyc_YYSTYPE*)_T4D8;_T1883->tag=Cyc_yyget_Exp_tok(_T4D9);_T4DA=yyyvsp;_T4DB=_T4DA[0];_T4DC=_T4DB.l;_T4DD=_T4DC.first_line;_T1883->loc=Cyc_Position_loc_to_seg(_T4DD);_T4D3=(struct Cyc_Absyn_Enumfield*)_T1883;}yyval=Cyc_YY48(_T4D3);goto _LL0;case 124:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1758 "parse.y"
_T4DF=yyyvsp + 0;_T4E0=&(*_T4DF).v;_T4E1=(union Cyc_YYSTYPE*)_T4E0;_T1883->hd=Cyc_yyget_YY48(_T4E1);_T1883->tl=0;_T4DE=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY49(_T4DE);goto _LL0;case 125:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1759 "parse.y"
_T4E3=yyyvsp + 0;_T4E4=&(*_T4E3).v;_T4E5=(union Cyc_YYSTYPE*)_T4E4;_T1883->hd=Cyc_yyget_YY48(_T4E5);_T1883->tl=0;_T4E2=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY49(_T4E2);goto _LL0;case 126:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1760 "parse.y"
_T4E7=yyyvsp + 0;_T4E8=&(*_T4E7).v;_T4E9=(union Cyc_YYSTYPE*)_T4E8;_T1883->hd=Cyc_yyget_YY48(_T4E9);_T4EA=yyyvsp + 2;_T4EB=&(*_T4EA).v;_T4EC=(union Cyc_YYSTYPE*)_T4EB;_T1883->tl=Cyc_yyget_YY49(_T4EC);_T4E6=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY49(_T4E6);goto _LL0;case 127:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_T1883->tag=7;
# 1766 "parse.y"
_T4EE=yyyvsp + 0;_T4EF=&(*_T4EE).v;_T4F0=(union Cyc_YYSTYPE*)_T4EF;_T1883->f1=Cyc_yyget_YY22(_T4F0);_T1883->f2=0;_T4F1=yyyvsp + 2;_T4F2=&(*_T4F1).v;_T4F3=(union Cyc_YYSTYPE*)_T4F2;_T1883->f3=Cyc_yyget_YY25(_T4F3);_T4ED=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T1883;}_T4F4=(void*)_T4ED;_T4F5=yyyvsp;_T4F6=_T4F5[0];_T4F7=_T4F6.l;_T4F8=_T4F7.first_line;_T4F9=Cyc_Position_loc_to_seg(_T4F8);_T4FA=Cyc_Parse_type_spec(_T4F4,_T4F9);yyval=Cyc_YY21(_T4FA);goto _LL0;case 128: _T4FC=Cyc_List_map_c;{
# 1770 "parse.y"
struct Cyc_List_List*(*_T1883)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T4FC;_T4FB=_T1883;}_T4FD=yyyvsp;_T4FE=_T4FD[2];_T4FF=_T4FE.l;_T500=_T4FF.first_line;_T501=Cyc_Position_loc_to_seg(_T500);_T502=yyyvsp + 2;_T503=&(*_T502).v;_T504=(union Cyc_YYSTYPE*)_T503;_T505=Cyc_yyget_YY41(_T504);{struct Cyc_List_List*ts=_T4FB(Cyc_Parse_typ2tvar,_T501,_T505);_T507=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1883)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T507;_T506=_T1883;}_T508=yyyvsp;_T509=_T508[4];_T50A=_T509.l;_T50B=_T50A.first_line;_T50C=Cyc_Position_loc_to_seg(_T50B);_T50D=yyyvsp + 4;_T50E=&(*_T50D).v;_T50F=(union Cyc_YYSTYPE*)_T50E;_T510=Cyc_yyget_YY41(_T50F);{struct Cyc_List_List*exist_ts=_T506(Cyc_Parse_typ2tvar,_T50C,_T510);
_T511=yyyvsp + 5;_T512=&(*_T511).v;_T513=(union Cyc_YYSTYPE*)_T512;{struct _tuple28*ec_qb=Cyc_yyget_YY51(_T513);_T515=ec_qb;_T516=(unsigned)_T515;
if(!_T516)goto _TL29D;_T517=ec_qb;_T514=*_T517;goto _TL29E;_TL29D:{struct _tuple28 _T1883;_T1883.f0=0;_T1883.f1=0;_T518=_T1883;}_T514=_T518;_TL29E: {struct _tuple28 _T1883=_T514;struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_List_List*ec=_T1885;struct Cyc_List_List*qb=_T1884;
_T519=yyyvsp + 0;_T51A=&(*_T519).v;_T51B=(union Cyc_YYSTYPE*)_T51A;_T51C=Cyc_yyget_YY23(_T51B);_T51D=_T51C.f1;_T51E=yyyvsp + 1;_T51F=&(*_T51E).v;_T520=(union Cyc_YYSTYPE*)_T51F;_T521=Cyc_yyget_QualId_tok(_T520);_T522=ts;_T523=exist_ts;_T524=ec;_T525=qb;
_T526=yyyvsp + 6;_T527=&(*_T526).v;_T528=(union Cyc_YYSTYPE*)_T527;_T529=Cyc_yyget_YY25(_T528);_T52A=yyyvsp + 0;_T52B=&(*_T52A).v;_T52C=(union Cyc_YYSTYPE*)_T52B;_T52D=Cyc_yyget_YY23(_T52C);_T52E=_T52D.f0;_T52F=Cyc_Absyn_aggrdecl_impl(_T523,_T524,_T525,_T529,_T52E);_T530=yyyvsp;_T531=_T530[0];_T532=_T531.l;_T533=_T532.first_line;_T534=
Cyc_Position_loc_to_seg(_T533);{
# 1774
struct Cyc_Absyn_TypeDecl*td=Cyc_Absyn_aggr_tdecl(_T51D,2U,_T521,_T522,_T52F,0,_T534);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T1886->tag=10;
# 1777
_T1886->f1=td;_T1886->f2=0;_T535=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1886;}_T536=(void*)_T535;_T537=yyyvsp;_T538=_T537[0];_T539=_T538.l;_T53A=_T539.first_line;_T53B=Cyc_Position_loc_to_seg(_T53A);_T53C=Cyc_Parse_type_spec(_T536,_T53B);yyval=Cyc_YY21(_T53C);goto _LL0;}}}}}}case 129:
# 1780 "parse.y"
 _T53D=yyyvsp + 0;_T53E=&(*_T53D).v;_T53F=(union Cyc_YYSTYPE*)_T53E;_T540=Cyc_yyget_YY23(_T53F);_T541=_T540.f1;_T542=yyyvsp + 1;_T543=&(*_T542).v;_T544=(union Cyc_YYSTYPE*)_T543;_T545=Cyc_yyget_QualId_tok(_T544);_T547=yyyvsp + 0;_T548=&(*_T547).v;_T549=(union Cyc_YYSTYPE*)_T548;_T54A=Cyc_yyget_YY23(_T549);_T54B=_T54A.f0;if(!_T54B)goto _TL29F;{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));_T1883->v=(void*)1;_T54C=(struct Cyc_Core_Opt*)_T1883;}_T546=_T54C;goto _TL2A0;_TL29F: _T546=0;_TL2A0: _T54D=Cyc_Absyn_UnknownAggr(_T541,_T545,_T546);
_T54E=yyyvsp + 2;_T54F=&(*_T54E).v;_T550=(union Cyc_YYSTYPE*)_T54F;_T551=Cyc_yyget_YY41(_T550);_T552=
# 1780
Cyc_Absyn_aggr_type(_T54D,_T551);_T553=yyyvsp;_T554=_T553[0];_T555=_T554.l;_T556=_T555.first_line;_T557=
Cyc_Position_loc_to_seg(_T556);_T558=
# 1780
Cyc_Parse_type_spec(_T552,_T557);yyval=Cyc_YY21(_T558);goto _LL0;case 130:{struct _tuple25 _T1883;
# 1785 "parse.y"
_T1883.f0=1;_T55A=yyyvsp + 1;_T55B=&(*_T55A).v;_T55C=(union Cyc_YYSTYPE*)_T55B;_T1883.f1=Cyc_yyget_YY22(_T55C);_T559=_T1883;}yyval=Cyc_YY23(_T559);goto _LL0;case 131:{struct _tuple25 _T1883;
# 1786 "parse.y"
_T1883.f0=0;_T55E=yyyvsp + 0;_T55F=&(*_T55E).v;_T560=(union Cyc_YYSTYPE*)_T55F;_T1883.f1=Cyc_yyget_YY22(_T560);_T55D=_T1883;}yyval=Cyc_YY23(_T55D);goto _LL0;case 132:
# 1789
 yyval=Cyc_YY22(0U);goto _LL0;case 133:
# 1790 "parse.y"
 yyval=Cyc_YY22(1U);goto _LL0;case 134:
# 1793
 yyval=Cyc_YY41(0);goto _LL0;case 135:
# 1794 "parse.y"
 _T561=yyyvsp + 1;_T562=&(*_T561).v;_T563=(union Cyc_YYSTYPE*)_T562;_T564=Cyc_yyget_YY41(_T563);_T565=Cyc_List_imp_rev(_T564);yyval=Cyc_YY41(_T565);goto _LL0;case 136:
# 1799 "parse.y"
 yyval=Cyc_YY25(0);goto _LL0;case 137:  {
# 1801 "parse.y"
struct Cyc_List_List*decls=0;
_T566=yyyvsp + 0;_T567=&(*_T566).v;_T568=(union Cyc_YYSTYPE*)_T567;{struct Cyc_List_List*x=Cyc_yyget_YY26(_T568);_TL2A4: if(x!=0)goto _TL2A2;else{goto _TL2A3;}
_TL2A2: _T569=x;_T56A=_T569->hd;_T56B=(struct Cyc_List_List*)_T56A;_T56C=decls;decls=Cyc_List_imp_append(_T56B,_T56C);_T56D=x;
# 1802
x=_T56D->tl;goto _TL2A4;_TL2A3:;}{
# 1805
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags==0)goto _TL2A5;_T56F=Cyc_List_iter_c;{
void(*_T1883)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))_T56F;_T56E=_T1883;}_T570=tags;_T571=decls;_T56E(Cyc_Parse_substitute_aggrfield_tags,_T570,_T571);goto _TL2A6;_TL2A5: _TL2A6:
 yyval=Cyc_YY25(decls);goto _LL0;}}case 138:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1814 "parse.y"
_T573=yyyvsp + 0;_T574=&(*_T573).v;_T575=(union Cyc_YYSTYPE*)_T574;_T1883->hd=Cyc_yyget_YY25(_T575);_T1883->tl=0;_T572=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY26(_T572);goto _LL0;case 139:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1815 "parse.y"
_T577=yyyvsp + 1;_T578=&(*_T577).v;_T579=(union Cyc_YYSTYPE*)_T578;_T1883->hd=Cyc_yyget_YY25(_T579);_T57A=yyyvsp + 0;_T57B=&(*_T57A).v;_T57C=(union Cyc_YYSTYPE*)_T57B;_T1883->tl=Cyc_yyget_YY26(_T57C);_T576=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY26(_T576);goto _LL0;case 140: _T57E=yyr;{struct _tuple11*_T1883=_region_malloc(_T57E,0U,sizeof(struct _tuple11));
# 1821 "parse.y"
_T1883->tl=0;_T57F=yyyvsp + 0;_T580=&(*_T57F).v;_T581=(union Cyc_YYSTYPE*)_T580;_T1883->hd=Cyc_yyget_YY18(_T581);_T57D=(struct _tuple11*)_T1883;}yyval=Cyc_YY19(_T57D);goto _LL0;case 141: _T583=yyr;{struct _tuple11*_T1883=_region_malloc(_T583,0U,sizeof(struct _tuple11));
# 1823 "parse.y"
_T584=yyyvsp + 0;_T585=&(*_T584).v;_T586=(union Cyc_YYSTYPE*)_T585;_T1883->tl=Cyc_yyget_YY19(_T586);_T587=yyyvsp + 2;_T588=&(*_T587).v;_T589=(union Cyc_YYSTYPE*)_T588;_T1883->hd=Cyc_yyget_YY18(_T589);_T582=(struct _tuple11*)_T1883;}yyval=Cyc_YY19(_T582);goto _LL0;case 142:{struct _tuple12 _T1883;
# 1827 "parse.y"
_T58B=yyyvsp + 0;_T58C=&(*_T58B).v;_T58D=(union Cyc_YYSTYPE*)_T58C;_T1883.f0=Cyc_yyget_YY28(_T58D);_T1883.f1=0;_T1883.f2=0;_T58A=_T1883;}yyval=Cyc_YY18(_T58A);goto _LL0;case 143:{struct _tuple12 _T1883;
# 1828 "parse.y"
_T58F=yyyvsp + 0;_T590=&(*_T58F).v;_T591=(union Cyc_YYSTYPE*)_T590;_T1883.f0=Cyc_yyget_YY28(_T591);_T1883.f1=0;_T592=yyyvsp + 2;_T593=&(*_T592).v;_T594=(union Cyc_YYSTYPE*)_T593;_T595=Cyc_yyget_YY63(_T594);_T596=yyyvsp;_T597=_T596[1];_T598=_T597.l;_T599=_T598.first_line;_T59A=Cyc_Position_loc_to_seg(_T599);_T1883.f2=Cyc_Absyn_new_exp(_T595,_T59A);_T58E=_T1883;}yyval=Cyc_YY18(_T58E);goto _LL0;case 144:{struct _tuple12 _T1883;
# 1829 "parse.y"
_T59C=yyyvsp + 0;_T59D=&(*_T59C).v;_T59E=(union Cyc_YYSTYPE*)_T59D;_T1883.f0=Cyc_yyget_YY28(_T59E);_T59F=yyyvsp + 2;_T5A0=&(*_T59F).v;_T5A1=(union Cyc_YYSTYPE*)_T5A0;_T1883.f1=Cyc_yyget_Exp_tok(_T5A1);_T1883.f2=0;_T59B=_T1883;}yyval=Cyc_YY18(_T59B);goto _LL0;case 145:{struct _tuple12 _T1883;
# 1830 "parse.y"
_T5A3=yyyvsp + 0;_T5A4=&(*_T5A3).v;_T5A5=(union Cyc_YYSTYPE*)_T5A4;_T1883.f0=Cyc_yyget_YY28(_T5A5);_T5A6=yyyvsp + 4;_T5A7=&(*_T5A6).v;_T5A8=(union Cyc_YYSTYPE*)_T5A7;_T1883.f1=Cyc_yyget_Exp_tok(_T5A8);_T5A9=yyyvsp + 2;_T5AA=&(*_T5A9).v;_T5AB=(union Cyc_YYSTYPE*)_T5AA;_T5AC=Cyc_yyget_YY63(_T5AB);_T5AD=yyyvsp;_T5AE=_T5AD[1];_T5AF=_T5AE.l;_T5B0=_T5AF.first_line;_T5B1=Cyc_Position_loc_to_seg(_T5B0);_T1883.f2=Cyc_Absyn_new_exp(_T5AC,_T5B1);_T5A2=_T1883;}yyval=Cyc_YY18(_T5A2);goto _LL0;case 146:  {struct _RegionHandle _T1883=_new_region(0U,"temp");struct _RegionHandle*temp=& _T1883;_push_region(temp);
# 1836 "parse.y"
_T5B2=yyyvsp + 0;_T5B3=&(*_T5B2).v;_T5B4=(union Cyc_YYSTYPE*)_T5B3;{struct _tuple26 _T1884=Cyc_yyget_YY36(_T5B4);struct Cyc_List_List*_T1885;struct Cyc_Parse_Type_specifier _T1886;struct Cyc_Absyn_Tqual _T1887;_T1887=_T1884.f0;_T1886=_T1884.f1;_T1885=_T1884.f2;{struct Cyc_Absyn_Tqual tq=_T1887;struct Cyc_Parse_Type_specifier tspecs=_T1886;struct Cyc_List_List*atts=_T1885;_T5B5=tq;_T5B6=_T5B5.loc;
if(_T5B6!=0U)goto _TL2A7;_T5B7=yyyvsp;_T5B8=_T5B7[0];_T5B9=_T5B8.l;_T5BA=_T5B9.first_line;tq.loc=Cyc_Position_loc_to_seg(_T5BA);goto _TL2A8;_TL2A7: _TL2A8: {
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
_T5BB=yyyvsp + 1;_T5BC=&(*_T5BB).v;_T5BD=(union Cyc_YYSTYPE*)_T5BC;{struct Cyc_List_List*x=Cyc_yyget_YY30(_T5BD);_TL2AC: if(x!=0)goto _TL2AA;else{goto _TL2AB;}
_TL2AA: _T5BE=x;_T5BF=_T5BE->hd;{struct _tuple12*_T1888=(struct _tuple12*)_T5BF;struct Cyc_Absyn_Exp*_T1889;struct Cyc_Absyn_Exp*_T188A;struct Cyc_Parse_Declarator _T188B;{struct _tuple12 _T188C=*_T1888;_T188B=_T188C.f0;_T188A=_T188C.f1;_T1889=_T188C.f2;}{struct Cyc_Parse_Declarator d=_T188B;struct Cyc_Absyn_Exp*wd=_T188A;struct Cyc_Absyn_Exp*wh=_T1889;_T5C1=temp;{struct _tuple13*_T188C=_region_malloc(_T5C1,0U,sizeof(struct _tuple13));
_T188C->tl=decls;_T188C->hd=d;_T5C0=(struct _tuple13*)_T188C;}decls=_T5C0;_T5C3=temp;{struct Cyc_List_List*_T188C=_region_malloc(_T5C3,0U,sizeof(struct Cyc_List_List));_T5C5=temp;{struct _tuple16*_T188D=_region_malloc(_T5C5,0U,sizeof(struct _tuple16));
_T188D->f0=wd;_T188D->f1=wh;_T5C4=(struct _tuple16*)_T188D;}_T188C->hd=_T5C4;_T188C->tl=widths_and_reqs;_T5C2=(struct Cyc_List_List*)_T188C;}widths_and_reqs=_T5C2;}}_T5C6=x;
# 1840
x=_T5C6->tl;goto _TL2AC;_TL2AB:;}_T5C7=tspecs;_T5C8=yyyvsp;_T5C9=_T5C8[0];_T5CA=_T5C9.l;_T5CB=_T5CA.first_line;_T5CC=
# 1845
Cyc_Position_loc_to_seg(_T5CB);{void*t=Cyc_Parse_speclist2typ(_T5C7,_T5CC);_T5CD=temp;_T5CE=temp;_T5CF=
# 1847
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);_T5D0=widths_and_reqs;{
# 1846
struct Cyc_List_List*info=Cyc_List_rzip(_T5CD,_T5CE,_T5CF,_T5D0);_T5D2=Cyc_List_map_c;{
# 1848
struct Cyc_List_List*(*_T1888)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))_T5D2;_T5D1=_T1888;}_T5D3=yyyvsp;_T5D4=_T5D3[0];_T5D5=_T5D4.l;_T5D6=_T5D5.first_line;_T5D7=Cyc_Position_loc_to_seg(_T5D6);_T5D8=info;_T5D9=_T5D1(Cyc_Parse_make_aggr_field,_T5D7,_T5D8);yyval=Cyc_YY25(_T5D9);_npop_handler(0);goto _LL0;}}}}}_pop_region();}case 147:{struct _tuple26 _T1883;_T5DB=yyyvsp;_T5DC=_T5DB[0];_T5DD=_T5DC.l;_T5DE=_T5DD.first_line;_T5DF=
# 1857 "parse.y"
Cyc_Position_loc_to_seg(_T5DE);_T1883.f0=Cyc_Absyn_empty_tqual(_T5DF);_T5E0=yyyvsp + 0;_T5E1=&(*_T5E0).v;_T5E2=(union Cyc_YYSTYPE*)_T5E1;_T1883.f1=Cyc_yyget_YY21(_T5E2);_T1883.f2=0;_T5DA=_T1883;}yyval=Cyc_YY36(_T5DA);goto _LL0;case 148:
# 1859 "parse.y"
 _T5E3=yyyvsp + 1;_T5E4=&(*_T5E3).v;_T5E5=(union Cyc_YYSTYPE*)_T5E4;{struct _tuple26 two=Cyc_yyget_YY36(_T5E5);{struct _tuple26 _T1883;_T5E7=two;_T1883.f0=_T5E7.f0;_T5E8=yyyvsp;_T5E9=_T5E8[0];_T5EA=_T5E9.l;_T5EB=_T5EA.first_line;_T5EC=Cyc_Position_loc_to_seg(_T5EB);_T5ED=yyyvsp + 0;_T5EE=&(*_T5ED).v;_T5EF=(union Cyc_YYSTYPE*)_T5EE;_T5F0=Cyc_yyget_YY21(_T5EF);_T5F1=two;_T5F2=_T5F1.f1;_T1883.f1=Cyc_Parse_combine_specifiers(_T5EC,_T5F0,_T5F2);_T5F3=two;_T1883.f2=_T5F3.f2;_T5E6=_T1883;}yyval=Cyc_YY36(_T5E6);goto _LL0;}case 149:{struct _tuple26 _T1883;
# 1861 "parse.y"
_T5F5=yyyvsp + 0;_T5F6=&(*_T5F5).v;_T5F7=(union Cyc_YYSTYPE*)_T5F6;_T1883.f0=Cyc_yyget_YY24(_T5F7);_T1883.f1=Cyc_Parse_empty_spec(0U);_T1883.f2=0;_T5F4=_T1883;}yyval=Cyc_YY36(_T5F4);goto _LL0;case 150:
# 1863 "parse.y"
 _T5F8=yyyvsp + 1;_T5F9=&(*_T5F8).v;_T5FA=(union Cyc_YYSTYPE*)_T5F9;{struct _tuple26 two=Cyc_yyget_YY36(_T5FA);{struct _tuple26 _T1883;_T5FC=yyyvsp + 0;_T5FD=&(*_T5FC).v;_T5FE=(union Cyc_YYSTYPE*)_T5FD;_T5FF=Cyc_yyget_YY24(_T5FE);_T600=two;_T601=_T600.f0;_T1883.f0=Cyc_Absyn_combine_tqual(_T5FF,_T601);_T602=two;_T1883.f1=_T602.f1;_T603=two;_T1883.f2=_T603.f2;_T5FB=_T1883;}yyval=Cyc_YY36(_T5FB);goto _LL0;}case 151:{struct _tuple26 _T1883;_T605=yyyvsp;_T606=_T605[0];_T607=_T606.l;_T608=_T607.first_line;_T609=
# 1865 "parse.y"
Cyc_Position_loc_to_seg(_T608);_T1883.f0=Cyc_Absyn_empty_tqual(_T609);_T1883.f1=Cyc_Parse_empty_spec(0U);_T60A=yyyvsp + 0;_T60B=&(*_T60A).v;_T60C=(union Cyc_YYSTYPE*)_T60B;_T1883.f2=Cyc_yyget_YY46(_T60C);_T604=_T1883;}yyval=Cyc_YY36(_T604);goto _LL0;case 152:
# 1867 "parse.y"
 _T60D=yyyvsp + 1;_T60E=&(*_T60D).v;_T60F=(union Cyc_YYSTYPE*)_T60E;{struct _tuple26 two=Cyc_yyget_YY36(_T60F);{struct _tuple26 _T1883;_T611=two;_T1883.f0=_T611.f0;_T612=two;_T1883.f1=_T612.f1;_T613=yyyvsp + 0;_T614=&(*_T613).v;_T615=(union Cyc_YYSTYPE*)_T614;_T616=Cyc_yyget_YY46(_T615);_T617=two;_T618=_T617.f2;_T1883.f2=Cyc_List_append(_T616,_T618);_T610=_T1883;}yyval=Cyc_YY36(_T610);goto _LL0;}case 153:{struct _tuple26 _T1883;_T61A=yyyvsp;_T61B=_T61A[0];_T61C=_T61B.l;_T61D=_T61C.first_line;_T61E=
# 1873 "parse.y"
Cyc_Position_loc_to_seg(_T61D);_T1883.f0=Cyc_Absyn_empty_tqual(_T61E);_T61F=yyyvsp + 0;_T620=&(*_T61F).v;_T621=(union Cyc_YYSTYPE*)_T620;_T1883.f1=Cyc_yyget_YY21(_T621);_T1883.f2=0;_T619=_T1883;}yyval=Cyc_YY36(_T619);goto _LL0;case 154:
# 1875 "parse.y"
 _T622=yyyvsp + 1;_T623=&(*_T622).v;_T624=(union Cyc_YYSTYPE*)_T623;{struct _tuple26 two=Cyc_yyget_YY36(_T624);{struct _tuple26 _T1883;_T626=two;_T1883.f0=_T626.f0;_T627=yyyvsp;_T628=_T627[0];_T629=_T628.l;_T62A=_T629.first_line;_T62B=Cyc_Position_loc_to_seg(_T62A);_T62C=yyyvsp + 0;_T62D=&(*_T62C).v;_T62E=(union Cyc_YYSTYPE*)_T62D;_T62F=Cyc_yyget_YY21(_T62E);_T630=two;_T631=_T630.f1;_T1883.f1=Cyc_Parse_combine_specifiers(_T62B,_T62F,_T631);_T632=two;_T1883.f2=_T632.f2;_T625=_T1883;}yyval=Cyc_YY36(_T625);goto _LL0;}case 155:{struct _tuple26 _T1883;
# 1877 "parse.y"
_T634=yyyvsp + 0;_T635=&(*_T634).v;_T636=(union Cyc_YYSTYPE*)_T635;_T1883.f0=Cyc_yyget_YY24(_T636);_T1883.f1=Cyc_Parse_empty_spec(0U);_T1883.f2=0;_T633=_T1883;}yyval=Cyc_YY36(_T633);goto _LL0;case 156:
# 1879 "parse.y"
 _T637=yyyvsp + 1;_T638=&(*_T637).v;_T639=(union Cyc_YYSTYPE*)_T638;{struct _tuple26 two=Cyc_yyget_YY36(_T639);{struct _tuple26 _T1883;_T63B=yyyvsp + 0;_T63C=&(*_T63B).v;_T63D=(union Cyc_YYSTYPE*)_T63C;_T63E=Cyc_yyget_YY24(_T63D);_T63F=two;_T640=_T63F.f0;_T1883.f0=Cyc_Absyn_combine_tqual(_T63E,_T640);_T641=two;_T1883.f1=_T641.f1;_T642=two;_T1883.f2=_T642.f2;_T63A=_T1883;}yyval=Cyc_YY36(_T63A);goto _LL0;}case 157:{struct _tuple26 _T1883;_T644=yyyvsp;_T645=_T644[0];_T646=_T645.l;_T647=_T646.first_line;_T648=
# 1881 "parse.y"
Cyc_Position_loc_to_seg(_T647);_T1883.f0=Cyc_Absyn_empty_tqual(_T648);_T1883.f1=Cyc_Parse_empty_spec(0U);_T649=yyyvsp + 0;_T64A=&(*_T649).v;_T64B=(union Cyc_YYSTYPE*)_T64A;_T1883.f2=Cyc_yyget_YY46(_T64B);_T643=_T1883;}yyval=Cyc_YY36(_T643);goto _LL0;case 158:
# 1883 "parse.y"
 _T64C=yyyvsp + 1;_T64D=&(*_T64C).v;_T64E=(union Cyc_YYSTYPE*)_T64D;{struct _tuple26 two=Cyc_yyget_YY36(_T64E);{struct _tuple26 _T1883;_T650=two;_T1883.f0=_T650.f0;_T651=two;_T1883.f1=_T651.f1;_T652=yyyvsp + 0;_T653=&(*_T652).v;_T654=(union Cyc_YYSTYPE*)_T653;_T655=Cyc_yyget_YY46(_T654);_T656=two;_T657=_T656.f2;_T1883.f2=Cyc_List_append(_T655,_T657);_T64F=_T1883;}yyval=Cyc_YY36(_T64F);goto _LL0;}case 159: _T659=yyr;{struct Cyc_List_List*_T1883=_region_malloc(_T659,0U,sizeof(struct Cyc_List_List));
# 1889 "parse.y"
_T65A=yyyvsp + 0;_T65B=&(*_T65A).v;_T65C=(union Cyc_YYSTYPE*)_T65B;_T1883->hd=Cyc_yyget_YY29(_T65C);_T1883->tl=0;_T658=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY30(_T658);goto _LL0;case 160: _T65E=yyr;{struct Cyc_List_List*_T1883=_region_malloc(_T65E,0U,sizeof(struct Cyc_List_List));
# 1891 "parse.y"
_T65F=yyyvsp + 2;_T660=&(*_T65F).v;_T661=(union Cyc_YYSTYPE*)_T660;_T1883->hd=Cyc_yyget_YY29(_T661);_T662=yyyvsp + 0;_T663=&(*_T662).v;_T664=(union Cyc_YYSTYPE*)_T663;_T1883->tl=Cyc_yyget_YY30(_T664);_T65D=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY30(_T65D);goto _LL0;case 161: _T666=yyr;{struct _tuple12*_T1883=_region_malloc(_T666,0U,sizeof(struct _tuple12));
# 1896 "parse.y"
_T667=yyyvsp + 0;_T668=&(*_T667).v;_T669=(union Cyc_YYSTYPE*)_T668;_T1883->f0=Cyc_yyget_YY28(_T669);_T1883->f1=0;_T66A=yyyvsp + 1;_T66B=&(*_T66A).v;_T66C=(union Cyc_YYSTYPE*)_T66B;_T1883->f2=Cyc_yyget_YY61(_T66C);_T665=(struct _tuple12*)_T1883;}yyval=Cyc_YY29(_T665);goto _LL0;case 162: _T66E=yyr;{struct _tuple12*_T1883=_region_malloc(_T66E,0U,sizeof(struct _tuple12));{struct _tuple0*_T1884=_cycalloc(sizeof(struct _tuple0));
# 1900 "parse.y"
_T1884->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));*_T1885=_tag_fat("",sizeof(char),1U);_T670=(struct _fat_ptr*)_T1885;}_T1884->f1=_T670;_T66F=(struct _tuple0*)_T1884;}_T1883->f0.id=_T66F;_T1883->f0.varloc=0U;_T1883->f0.tms=0;_T671=yyyvsp + 1;_T672=&(*_T671).v;_T673=(union Cyc_YYSTYPE*)_T672;_T1883->f1=Cyc_yyget_Exp_tok(_T673);_T1883->f2=0;_T66D=(struct _tuple12*)_T1883;}yyval=Cyc_YY29(_T66D);goto _LL0;case 163: _T675=yyr;{struct _tuple12*_T1883=_region_malloc(_T675,0U,sizeof(struct _tuple12));{struct _tuple0*_T1884=_cycalloc(sizeof(struct _tuple0));
# 1905 "parse.y"
_T1884->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));*_T1885=_tag_fat("",sizeof(char),1U);_T677=(struct _fat_ptr*)_T1885;}_T1884->f1=_T677;_T676=(struct _tuple0*)_T1884;}_T1883->f0.id=_T676;_T1883->f0.varloc=0U;_T1883->f0.tms=0;_T1883->f1=0;_T1883->f2=0;_T674=(struct _tuple12*)_T1883;}yyval=Cyc_YY29(_T674);goto _LL0;case 164: _T679=yyr;{struct _tuple12*_T1883=_region_malloc(_T679,0U,sizeof(struct _tuple12));
# 1908 "parse.y"
_T67A=yyyvsp + 0;_T67B=&(*_T67A).v;_T67C=(union Cyc_YYSTYPE*)_T67B;_T1883->f0=Cyc_yyget_YY28(_T67C);_T67D=yyyvsp + 2;_T67E=&(*_T67D).v;_T67F=(union Cyc_YYSTYPE*)_T67E;_T1883->f1=Cyc_yyget_Exp_tok(_T67F);_T1883->f2=0;_T678=(struct _tuple12*)_T1883;}yyval=Cyc_YY29(_T678);goto _LL0;case 165:
# 1912 "parse.y"
 _T680=yyyvsp + 2;_T681=&(*_T680).v;_T682=(union Cyc_YYSTYPE*)_T681;_T683=Cyc_yyget_Exp_tok(_T682);yyval=Cyc_YY61(_T683);goto _LL0;case 166:
# 1913 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 167:
# 1919 "parse.y"
 _T684=yyyvsp + 0;_T685=&(*_T684).v;_T686=(union Cyc_YYSTYPE*)_T685;{int is_extensible=Cyc_yyget_YY32(_T686);_T688=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1883)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T688;_T687=_T1883;}_T689=yyyvsp;_T68A=_T689[2];_T68B=_T68A.l;_T68C=_T68B.first_line;_T68D=Cyc_Position_loc_to_seg(_T68C);_T68E=yyyvsp + 2;_T68F=&(*_T68E).v;_T690=(union Cyc_YYSTYPE*)_T68F;_T691=Cyc_yyget_YY41(_T690);{struct Cyc_List_List*ts=_T687(Cyc_Parse_typ2tvar,_T68D,_T691);
_T692=yyyvsp + 1;_T693=&(*_T692).v;_T694=(union Cyc_YYSTYPE*)_T693;_T695=Cyc_yyget_QualId_tok(_T694);_T696=ts;{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));_T698=yyyvsp + 4;_T699=&(*_T698).v;_T69A=(union Cyc_YYSTYPE*)_T699;_T1883->v=Cyc_yyget_YY35(_T69A);_T697=(struct Cyc_Core_Opt*)_T1883;}_T69B=is_extensible;_T69C=yyyvsp;_T69D=_T69C[0];_T69E=_T69D.l;_T69F=_T69E.first_line;_T6A0=
Cyc_Position_loc_to_seg(_T69F);{
# 1921
struct Cyc_Absyn_TypeDecl*dd=Cyc_Absyn_datatype_tdecl(2U,_T695,_T696,_T697,_T69B,_T6A0);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T1883->tag=10;
# 1923
_T1883->f1=dd;_T1883->f2=0;_T6A1=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1883;}_T6A2=(void*)_T6A1;_T6A3=yyyvsp;_T6A4=_T6A3[0];_T6A5=_T6A4.l;_T6A6=_T6A5.first_line;_T6A7=Cyc_Position_loc_to_seg(_T6A6);_T6A8=Cyc_Parse_type_spec(_T6A2,_T6A7);yyval=Cyc_YY21(_T6A8);goto _LL0;}}}case 168:
# 1926 "parse.y"
 _T6A9=yyyvsp + 0;_T6AA=&(*_T6A9).v;_T6AB=(union Cyc_YYSTYPE*)_T6AA;{int is_extensible=Cyc_yyget_YY32(_T6AB);{struct Cyc_Absyn_UnknownDatatypeInfo _T1883;
_T6AD=yyyvsp + 1;_T6AE=&(*_T6AD).v;_T6AF=(union Cyc_YYSTYPE*)_T6AE;_T1883.name=Cyc_yyget_QualId_tok(_T6AF);_T1883.is_extensible=is_extensible;_T6AC=_T1883;}_T6B0=Cyc_Absyn_UnknownDatatype(_T6AC);_T6B1=yyyvsp + 2;_T6B2=&(*_T6B1).v;_T6B3=(union Cyc_YYSTYPE*)_T6B2;_T6B4=Cyc_yyget_YY41(_T6B3);_T6B5=Cyc_Absyn_datatype_type(_T6B0,_T6B4);_T6B6=yyyvsp;_T6B7=_T6B6[0];_T6B8=_T6B7.l;_T6B9=_T6B8.first_line;_T6BA=Cyc_Position_loc_to_seg(_T6B9);_T6BB=Cyc_Parse_type_spec(_T6B5,_T6BA);yyval=Cyc_YY21(_T6BB);goto _LL0;}case 169:
# 1930 "parse.y"
 _T6BC=yyyvsp + 0;_T6BD=&(*_T6BC).v;_T6BE=(union Cyc_YYSTYPE*)_T6BD;{int is_extensible=Cyc_yyget_YY32(_T6BE);{struct Cyc_Absyn_UnknownDatatypeFieldInfo _T1883;
_T6C0=yyyvsp + 1;_T6C1=&(*_T6C0).v;_T6C2=(union Cyc_YYSTYPE*)_T6C1;_T1883.datatype_name=Cyc_yyget_QualId_tok(_T6C2);_T6C3=yyyvsp + 3;_T6C4=&(*_T6C3).v;_T6C5=(union Cyc_YYSTYPE*)_T6C4;_T1883.field_name=Cyc_yyget_QualId_tok(_T6C5);_T1883.is_extensible=is_extensible;_T6BF=_T1883;}_T6C6=Cyc_Absyn_UnknownDatatypefield(_T6BF);_T6C7=yyyvsp + 4;_T6C8=&(*_T6C7).v;_T6C9=(union Cyc_YYSTYPE*)_T6C8;_T6CA=Cyc_yyget_YY41(_T6C9);_T6CB=Cyc_Absyn_datatype_field_type(_T6C6,_T6CA);_T6CC=yyyvsp;_T6CD=_T6CC[0];_T6CE=_T6CD.l;_T6CF=_T6CE.first_line;_T6D0=Cyc_Position_loc_to_seg(_T6CF);_T6D1=Cyc_Parse_type_spec(_T6CB,_T6D0);yyval=Cyc_YY21(_T6D1);goto _LL0;}case 170:
# 1936 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 171:
# 1937 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 172:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1941 "parse.y"
_T6D3=yyyvsp + 0;_T6D4=&(*_T6D3).v;_T6D5=(union Cyc_YYSTYPE*)_T6D4;_T1883->hd=Cyc_yyget_YY34(_T6D5);_T1883->tl=0;_T6D2=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY35(_T6D2);goto _LL0;case 173:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1942 "parse.y"
_T6D7=yyyvsp + 0;_T6D8=&(*_T6D7).v;_T6D9=(union Cyc_YYSTYPE*)_T6D8;_T1883->hd=Cyc_yyget_YY34(_T6D9);_T1883->tl=0;_T6D6=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY35(_T6D6);goto _LL0;case 174:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1943 "parse.y"
_T6DB=yyyvsp + 0;_T6DC=&(*_T6DB).v;_T6DD=(union Cyc_YYSTYPE*)_T6DC;_T1883->hd=Cyc_yyget_YY34(_T6DD);_T6DE=yyyvsp + 2;_T6DF=&(*_T6DE).v;_T6E0=(union Cyc_YYSTYPE*)_T6DF;_T1883->tl=Cyc_yyget_YY35(_T6E0);_T6DA=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY35(_T6DA);goto _LL0;case 175:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 1944 "parse.y"
_T6E2=yyyvsp + 0;_T6E3=&(*_T6E2).v;_T6E4=(union Cyc_YYSTYPE*)_T6E3;_T1883->hd=Cyc_yyget_YY34(_T6E4);_T6E5=yyyvsp + 2;_T6E6=&(*_T6E5).v;_T6E7=(union Cyc_YYSTYPE*)_T6E6;_T1883->tl=Cyc_yyget_YY35(_T6E7);_T6E1=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY35(_T6E1);goto _LL0;case 176:
# 1948 "parse.y"
 yyval=Cyc_YY33(2U);goto _LL0;case 177:
# 1949 "parse.y"
 yyval=Cyc_YY33(3U);goto _LL0;case 178:
# 1950 "parse.y"
 yyval=Cyc_YY33(0U);goto _LL0;case 179:{struct Cyc_Absyn_Datatypefield*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));
# 1954 "parse.y"
_T6E9=yyyvsp + 1;_T6EA=&(*_T6E9).v;_T6EB=(union Cyc_YYSTYPE*)_T6EA;_T1883->name=Cyc_yyget_QualId_tok(_T6EB);_T1883->typs=0;_T6EC=yyyvsp;_T6ED=_T6EC[0];_T6EE=_T6ED.l;_T6EF=_T6EE.first_line;_T1883->loc=Cyc_Position_loc_to_seg(_T6EF);_T6F0=yyyvsp + 0;_T6F1=&(*_T6F0).v;_T6F2=(union Cyc_YYSTYPE*)_T6F1;_T1883->sc=Cyc_yyget_YY33(_T6F2);_T6E8=(struct Cyc_Absyn_Datatypefield*)_T1883;}yyval=Cyc_YY34(_T6E8);goto _LL0;case 180: _T6F4=Cyc_List_map_c;{
# 1956 "parse.y"
struct Cyc_List_List*(*_T1883)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T6F4;_T6F3=_T1883;}{struct _tuple19*(*_T1883)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T6F5=_T1883;}_T6F6=yyyvsp;_T6F7=_T6F6[3];_T6F8=_T6F7.l;_T6F9=_T6F8.first_line;_T6FA=Cyc_Position_loc_to_seg(_T6F9);_T6FB=yyyvsp + 3;_T6FC=&(*_T6FB).v;_T6FD=(union Cyc_YYSTYPE*)_T6FC;_T6FE=Cyc_yyget_YY39(_T6FD);_T6FF=Cyc_List_imp_rev(_T6FE);{struct Cyc_List_List*typs=_T6F3(_T6F5,_T6FA,_T6FF);{struct Cyc_Absyn_Datatypefield*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));
_T701=yyyvsp + 1;_T702=&(*_T701).v;_T703=(union Cyc_YYSTYPE*)_T702;_T1883->name=Cyc_yyget_QualId_tok(_T703);_T1883->typs=typs;_T704=yyyvsp;_T705=_T704[0];_T706=_T705.l;_T707=_T706.first_line;_T1883->loc=Cyc_Position_loc_to_seg(_T707);_T708=yyyvsp + 0;_T709=&(*_T708).v;_T70A=(union Cyc_YYSTYPE*)_T709;_T1883->sc=Cyc_yyget_YY33(_T70A);_T700=(struct Cyc_Absyn_Datatypefield*)_T1883;}yyval=Cyc_YY34(_T700);goto _LL0;}case 181: _T70B=yyyvsp;_T70C=_T70B[0];
# 1962 "parse.y"
yyval=_T70C.v;goto _LL0;case 182:
# 1964 "parse.y"
 _T70D=yyyvsp + 1;_T70E=&(*_T70D).v;_T70F=(union Cyc_YYSTYPE*)_T70E;{struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T70F);{struct Cyc_Parse_Declarator _T1883;_T711=two;
_T1883.id=_T711.id;_T712=two;_T1883.varloc=_T712.varloc;_T713=yyyvsp + 0;_T714=&(*_T713).v;_T715=(union Cyc_YYSTYPE*)_T714;_T716=Cyc_yyget_YY27(_T715);_T717=two;_T718=_T717.tms;_T1883.tms=Cyc_List_imp_append(_T716,_T718);_T710=_T1883;}yyval=Cyc_YY28(_T710);goto _LL0;}case 183: _T719=yyyvsp;_T71A=_T719[0];
# 1971 "parse.y"
yyval=_T71A.v;goto _LL0;case 184:
# 1973 "parse.y"
 _T71B=yyyvsp + 1;_T71C=&(*_T71B).v;_T71D=(union Cyc_YYSTYPE*)_T71C;{struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T71D);{struct Cyc_Parse_Declarator _T1883;_T71F=two;
_T1883.id=_T71F.id;_T720=two;_T1883.varloc=_T720.varloc;_T721=yyyvsp + 0;_T722=&(*_T721).v;_T723=(union Cyc_YYSTYPE*)_T722;_T724=Cyc_yyget_YY27(_T723);_T725=two;_T726=_T725.tms;_T1883.tms=Cyc_List_imp_append(_T724,_T726);_T71E=_T1883;}yyval=Cyc_YY28(_T71E);goto _LL0;}case 185:{struct Cyc_Parse_Declarator _T1883;
# 1978 "parse.y"
_T728=yyyvsp + 0;_T729=&(*_T728).v;_T72A=(union Cyc_YYSTYPE*)_T729;_T1883.id=Cyc_yyget_QualId_tok(_T72A);_T72B=yyyvsp;_T72C=_T72B[0];_T72D=_T72C.l;_T72E=_T72D.first_line;_T1883.varloc=Cyc_Position_loc_to_seg(_T72E);_T1883.tms=0;_T727=_T1883;}yyval=Cyc_YY28(_T727);goto _LL0;case 186: _T72F=yyyvsp;_T730=_T72F[1];
# 1979 "parse.y"
yyval=_T730.v;goto _LL0;case 187:
# 1983 "parse.y"
 _T731=yyyvsp + 2;_T732=&(*_T731).v;_T733=(union Cyc_YYSTYPE*)_T732;{struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T733);_T735=yyr;{struct Cyc_List_List*_T1883=_region_malloc(_T735,0U,sizeof(struct Cyc_List_List));_T737=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1884=_region_malloc(_T737,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1884->tag=5;_T738=yyyvsp;_T739=_T738[1];_T73A=_T739.l;_T73B=_T73A.first_line;
_T1884->f1=Cyc_Position_loc_to_seg(_T73B);_T73C=yyyvsp + 1;_T73D=&(*_T73C).v;_T73E=(union Cyc_YYSTYPE*)_T73D;_T1884->f2=Cyc_yyget_YY46(_T73E);_T736=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1884;}_T1883->hd=(void*)_T736;_T73F=d;_T1883->tl=_T73F.tms;_T734=(struct Cyc_List_List*)_T1883;}d.tms=_T734;_T740=yyyvsp;_T741=_T740[2];
yyval=_T741.v;goto _LL0;}case 188:{struct Cyc_Parse_Declarator _T1883;
# 1988 "parse.y"
_T743=yyyvsp + 0;_T744=&(*_T743).v;_T745=(union Cyc_YYSTYPE*)_T744;_T746=Cyc_yyget_YY28(_T745);_T1883.id=_T746.id;_T747=yyyvsp + 0;_T748=&(*_T747).v;_T749=(union Cyc_YYSTYPE*)_T748;_T74A=Cyc_yyget_YY28(_T749);_T1883.varloc=_T74A.varloc;_T74C=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T74C,0U,sizeof(struct Cyc_List_List));_T74E=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T74E,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1885->tag=0;_T74F=yyyvsp + 3;_T750=&(*_T74F).v;_T751=(union Cyc_YYSTYPE*)_T750;_T1885->f1=Cyc_yyget_YY54(_T751);_T752=yyyvsp;_T753=_T752[3];_T754=_T753.l;_T755=_T754.first_line;_T1885->f2=Cyc_Position_loc_to_seg(_T755);_T74D=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T74D;_T756=yyyvsp + 0;_T757=&(*_T756).v;_T758=(union Cyc_YYSTYPE*)_T757;_T759=Cyc_yyget_YY28(_T758);_T1884->tl=_T759.tms;_T74B=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T74B;_T742=_T1883;}yyval=Cyc_YY28(_T742);goto _LL0;case 189:{struct Cyc_Parse_Declarator _T1883;
# 1990 "parse.y"
_T75B=yyyvsp + 0;_T75C=&(*_T75B).v;_T75D=(union Cyc_YYSTYPE*)_T75C;_T75E=Cyc_yyget_YY28(_T75D);_T1883.id=_T75E.id;_T75F=yyyvsp + 0;_T760=&(*_T75F).v;_T761=(union Cyc_YYSTYPE*)_T760;_T762=Cyc_yyget_YY28(_T761);_T1883.varloc=_T762.varloc;_T764=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T764,0U,sizeof(struct Cyc_List_List));_T766=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T766,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1885->tag=1;
_T767=yyyvsp + 2;_T768=&(*_T767).v;_T769=(union Cyc_YYSTYPE*)_T768;_T1885->f1=Cyc_yyget_Exp_tok(_T769);_T76A=yyyvsp + 4;_T76B=&(*_T76A).v;_T76C=(union Cyc_YYSTYPE*)_T76B;_T1885->f2=Cyc_yyget_YY54(_T76C);_T76D=yyyvsp;_T76E=_T76D[4];_T76F=_T76E.l;_T770=_T76F.first_line;_T1885->f3=Cyc_Position_loc_to_seg(_T770);_T765=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T765;_T771=yyyvsp + 0;_T772=&(*_T771).v;_T773=(union Cyc_YYSTYPE*)_T772;_T774=Cyc_yyget_YY28(_T773);_T1884->tl=_T774.tms;_T763=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T763;_T75A=_T1883;}
# 1990
yyval=Cyc_YY28(_T75A);goto _LL0;case 190:
# 1993 "parse.y"
 _T775=yyyvsp + 2;_T776=&(*_T775).v;_T777=(union Cyc_YYSTYPE*)_T776;{struct _tuple27*_T1883=Cyc_yyget_YY40(_T777);struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;void*_T1886;struct Cyc_Absyn_VarargInfo*_T1887;int _T1888;struct Cyc_List_List*_T1889;{struct _tuple27 _T188A=*_T1883;_T1889=_T188A.f0;_T1888=_T188A.f1;_T1887=_T188A.f2;_T1886=_T188A.f3;_T1885=_T188A.f4;_T1884=_T188A.f5;}{struct Cyc_List_List*lis=_T1889;int b=_T1888;struct Cyc_Absyn_VarargInfo*c=_T1887;void*eff=_T1886;struct Cyc_List_List*ec=_T1885;struct Cyc_List_List*qb=_T1884;
_T778=yyyvsp + 4;_T779=&(*_T778).v;_T77A=(union Cyc_YYSTYPE*)_T779;{struct _tuple21 _T188A=Cyc_yyget_YY62(_T77A);struct Cyc_Absyn_Exp*_T188B;struct Cyc_Absyn_Exp*_T188C;struct Cyc_Absyn_Exp*_T188D;struct Cyc_Absyn_Exp*_T188E;_T188E=_T188A.f0;_T188D=_T188A.f1;_T188C=_T188A.f2;_T188B=_T188A.f3;{struct Cyc_Absyn_Exp*chk=_T188E;struct Cyc_Absyn_Exp*req=_T188D;struct Cyc_Absyn_Exp*ens=_T188C;struct Cyc_Absyn_Exp*thrws=_T188B;{struct Cyc_Parse_Declarator _T188F;
_T77C=yyyvsp + 0;_T77D=&(*_T77C).v;_T77E=(union Cyc_YYSTYPE*)_T77D;_T77F=Cyc_yyget_YY28(_T77E);_T188F.id=_T77F.id;_T780=yyyvsp + 0;_T781=&(*_T780).v;_T782=(union Cyc_YYSTYPE*)_T781;_T783=Cyc_yyget_YY28(_T782);_T188F.varloc=_T783.varloc;_T785=yyr;{struct Cyc_List_List*_T1890=_region_malloc(_T785,0U,sizeof(struct Cyc_List_List));_T787=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1891=_region_malloc(_T787,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1891->tag=3;_T789=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1892=_region_malloc(_T789,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1892->tag=1;_T1892->f1=lis;_T1892->f2=b;_T1892->f3=c;_T1892->f4=eff;_T1892->f5=ec;_T1892->f6=qb;_T1892->f7=chk;_T1892->f8=req;_T1892->f9=ens;_T1892->f10=thrws;_T788=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1892;}_T1891->f1=(void*)_T788;_T786=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1891;}_T1890->hd=(void*)_T786;_T78A=yyyvsp + 0;_T78B=&(*_T78A).v;_T78C=(union Cyc_YYSTYPE*)_T78B;_T78D=Cyc_yyget_YY28(_T78C);_T1890->tl=_T78D.tms;_T784=(struct Cyc_List_List*)_T1890;}_T188F.tms=_T784;_T77B=_T188F;}yyval=Cyc_YY28(_T77B);goto _LL0;}}}}case 191:{struct Cyc_Parse_Declarator _T1883;
# 1998 "parse.y"
_T78F=yyyvsp + 0;_T790=&(*_T78F).v;_T791=(union Cyc_YYSTYPE*)_T790;_T792=Cyc_yyget_YY28(_T791);_T1883.id=_T792.id;_T793=yyyvsp + 0;_T794=&(*_T793).v;_T795=(union Cyc_YYSTYPE*)_T794;_T796=Cyc_yyget_YY28(_T795);_T1883.varloc=_T796.varloc;_T798=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T798,0U,sizeof(struct Cyc_List_List));_T79A=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T79A,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1885->tag=3;_T79C=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T1886=_region_malloc(_T79C,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T1886->tag=0;_T79D=yyyvsp + 2;_T79E=&(*_T79D).v;_T79F=(union Cyc_YYSTYPE*)_T79E;_T1886->f1=Cyc_yyget_YY37(_T79F);_T7A0=yyyvsp;_T7A1=_T7A0[0];_T7A2=_T7A1.l;_T7A3=_T7A2.first_line;_T1886->f2=Cyc_Position_loc_to_seg(_T7A3);_T79B=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T1886;}_T1885->f1=(void*)_T79B;_T799=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T799;_T7A4=yyyvsp + 0;_T7A5=&(*_T7A4).v;_T7A6=(union Cyc_YYSTYPE*)_T7A5;_T7A7=Cyc_yyget_YY28(_T7A6);_T1884->tl=_T7A7.tms;_T797=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T797;_T78E=_T1883;}yyval=Cyc_YY28(_T78E);goto _LL0;case 192: _T7A9=Cyc_List_map_c;{
# 2001
struct Cyc_List_List*(*_T1883)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T7A9;_T7A8=_T1883;}_T7AA=yyyvsp;_T7AB=_T7AA[1];_T7AC=_T7AB.l;_T7AD=_T7AC.first_line;_T7AE=Cyc_Position_loc_to_seg(_T7AD);_T7AF=yyyvsp + 2;_T7B0=&(*_T7AF).v;_T7B1=(union Cyc_YYSTYPE*)_T7B0;_T7B2=Cyc_yyget_YY41(_T7B1);_T7B3=Cyc_List_imp_rev(_T7B2);{struct Cyc_List_List*ts=_T7A8(Cyc_Parse_typ2tvar,_T7AE,_T7B3);{struct Cyc_Parse_Declarator _T1883;
_T7B5=yyyvsp + 0;_T7B6=&(*_T7B5).v;_T7B7=(union Cyc_YYSTYPE*)_T7B6;_T7B8=Cyc_yyget_YY28(_T7B7);_T1883.id=_T7B8.id;_T7B9=yyyvsp + 0;_T7BA=&(*_T7B9).v;_T7BB=(union Cyc_YYSTYPE*)_T7BA;_T7BC=Cyc_yyget_YY28(_T7BB);_T1883.varloc=_T7BC.varloc;_T7BE=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T7BE,0U,sizeof(struct Cyc_List_List));_T7C0=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T7C0,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1885->tag=4;_T1885->f1=ts;_T7C1=yyyvsp;_T7C2=_T7C1[0];_T7C3=_T7C2.l;_T7C4=_T7C3.first_line;_T1885->f2=Cyc_Position_loc_to_seg(_T7C4);_T1885->f3=0;_T7BF=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T7BF;_T7C5=yyyvsp + 0;_T7C6=&(*_T7C5).v;_T7C7=(union Cyc_YYSTYPE*)_T7C6;_T7C8=Cyc_yyget_YY28(_T7C7);_T1884->tl=_T7C8.tms;_T7BD=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T7BD;_T7B4=_T1883;}yyval=Cyc_YY28(_T7B4);goto _LL0;}case 193:{struct Cyc_Parse_Declarator _T1883;
# 2005 "parse.y"
_T7CA=yyyvsp + 0;_T7CB=&(*_T7CA).v;_T7CC=(union Cyc_YYSTYPE*)_T7CB;_T7CD=Cyc_yyget_YY28(_T7CC);_T1883.id=_T7CD.id;_T7CE=yyyvsp + 0;_T7CF=&(*_T7CE).v;_T7D0=(union Cyc_YYSTYPE*)_T7CF;_T7D1=Cyc_yyget_YY28(_T7D0);_T1883.varloc=_T7D1.varloc;_T7D3=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T7D3,0U,sizeof(struct Cyc_List_List));_T7D5=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T7D5,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1885->tag=5;_T7D6=yyyvsp;_T7D7=_T7D6[1];_T7D8=_T7D7.l;_T7D9=_T7D8.first_line;_T1885->f1=Cyc_Position_loc_to_seg(_T7D9);_T7DA=yyyvsp + 1;_T7DB=&(*_T7DA).v;_T7DC=(union Cyc_YYSTYPE*)_T7DB;_T1885->f2=Cyc_yyget_YY46(_T7DC);_T7D4=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T7D4;
_T7DD=yyyvsp + 0;_T7DE=&(*_T7DD).v;_T7DF=(union Cyc_YYSTYPE*)_T7DE;_T7E0=Cyc_yyget_YY28(_T7DF);_T1884->tl=_T7E0.tms;_T7D2=(struct Cyc_List_List*)_T1884;}
# 2005
_T1883.tms=_T7D2;_T7C9=_T1883;}yyval=Cyc_YY28(_T7C9);goto _LL0;case 194:{struct Cyc_Parse_Declarator _T1883;
# 2012 "parse.y"
_T7E2=yyyvsp + 0;_T7E3=&(*_T7E2).v;_T7E4=(union Cyc_YYSTYPE*)_T7E3;_T1883.id=Cyc_yyget_QualId_tok(_T7E4);_T7E5=yyyvsp;_T7E6=_T7E5[0];_T7E7=_T7E6.l;_T7E8=_T7E7.first_line;_T1883.varloc=Cyc_Position_loc_to_seg(_T7E8);_T1883.tms=0;_T7E1=_T1883;}yyval=Cyc_YY28(_T7E1);goto _LL0;case 195:{struct Cyc_Parse_Declarator _T1883;
# 2013 "parse.y"
_T7EA=yyyvsp + 0;_T7EB=&(*_T7EA).v;_T7EC=(union Cyc_YYSTYPE*)_T7EB;_T1883.id=Cyc_yyget_QualId_tok(_T7EC);_T7ED=yyyvsp;_T7EE=_T7ED[0];_T7EF=_T7EE.l;_T7F0=_T7EF.first_line;_T1883.varloc=Cyc_Position_loc_to_seg(_T7F0);_T1883.tms=0;_T7E9=_T1883;}yyval=Cyc_YY28(_T7E9);goto _LL0;case 196: _T7F1=yyyvsp;_T7F2=_T7F1[1];
# 2014 "parse.y"
yyval=_T7F2.v;goto _LL0;case 197:
# 2018 "parse.y"
 _T7F3=yyyvsp + 2;_T7F4=&(*_T7F3).v;_T7F5=(union Cyc_YYSTYPE*)_T7F4;{struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T7F5);_T7F7=yyr;{struct Cyc_List_List*_T1883=_region_malloc(_T7F7,0U,sizeof(struct Cyc_List_List));_T7F9=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1884=_region_malloc(_T7F9,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1884->tag=5;_T7FA=yyyvsp;_T7FB=_T7FA[1];_T7FC=_T7FB.l;_T7FD=_T7FC.first_line;
_T1884->f1=Cyc_Position_loc_to_seg(_T7FD);_T7FE=yyyvsp + 1;_T7FF=&(*_T7FE).v;_T800=(union Cyc_YYSTYPE*)_T7FF;_T1884->f2=Cyc_yyget_YY46(_T800);_T7F8=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1884;}_T1883->hd=(void*)_T7F8;_T801=d;_T1883->tl=_T801.tms;_T7F6=(struct Cyc_List_List*)_T1883;}d.tms=_T7F6;_T802=yyyvsp;_T803=_T802[2];
yyval=_T803.v;goto _LL0;}case 198:
# 2023 "parse.y"
 _T804=yyyvsp + 0;_T805=&(*_T804).v;_T806=(union Cyc_YYSTYPE*)_T805;{struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T806);{struct Cyc_Parse_Declarator _T1883;_T808=one;
_T1883.id=_T808.id;_T809=one;_T1883.varloc=_T809.varloc;_T80B=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T80B,0U,sizeof(struct Cyc_List_List));_T80D=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T80D,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1885->tag=0;
_T80E=yyyvsp + 3;_T80F=&(*_T80E).v;_T810=(union Cyc_YYSTYPE*)_T80F;_T1885->f1=Cyc_yyget_YY54(_T810);_T811=yyyvsp;_T812=_T811[3];_T813=_T812.l;_T814=_T813.first_line;_T1885->f2=Cyc_Position_loc_to_seg(_T814);_T80C=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T80C;_T815=one;_T1884->tl=_T815.tms;_T80A=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T80A;_T807=_T1883;}
# 2024
yyval=Cyc_YY28(_T807);goto _LL0;}case 199:
# 2027 "parse.y"
 _T816=yyyvsp + 0;_T817=&(*_T816).v;_T818=(union Cyc_YYSTYPE*)_T817;{struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T818);{struct Cyc_Parse_Declarator _T1883;_T81A=one;
_T1883.id=_T81A.id;_T81B=one;_T1883.varloc=_T81B.varloc;_T81D=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T81D,0U,sizeof(struct Cyc_List_List));_T81F=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T81F,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1885->tag=1;
_T820=yyyvsp + 2;_T821=&(*_T820).v;_T822=(union Cyc_YYSTYPE*)_T821;_T1885->f1=Cyc_yyget_Exp_tok(_T822);_T823=yyyvsp + 4;_T824=&(*_T823).v;_T825=(union Cyc_YYSTYPE*)_T824;_T1885->f2=Cyc_yyget_YY54(_T825);_T826=yyyvsp;_T827=_T826[4];_T828=_T827.l;_T829=_T828.first_line;_T1885->f3=Cyc_Position_loc_to_seg(_T829);_T81E=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T81E;_T82A=one;
_T1884->tl=_T82A.tms;_T81C=(struct Cyc_List_List*)_T1884;}
# 2029
_T1883.tms=_T81C;_T819=_T1883;}
# 2028
yyval=Cyc_YY28(_T819);goto _LL0;}case 200:
# 2032 "parse.y"
 _T82B=yyyvsp + 2;_T82C=&(*_T82B).v;_T82D=(union Cyc_YYSTYPE*)_T82C;{struct _tuple27*_T1883=Cyc_yyget_YY40(_T82D);struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;void*_T1886;struct Cyc_Absyn_VarargInfo*_T1887;int _T1888;struct Cyc_List_List*_T1889;{struct _tuple27 _T188A=*_T1883;_T1889=_T188A.f0;_T1888=_T188A.f1;_T1887=_T188A.f2;_T1886=_T188A.f3;_T1885=_T188A.f4;_T1884=_T188A.f5;}{struct Cyc_List_List*lis=_T1889;int b=_T1888;struct Cyc_Absyn_VarargInfo*c=_T1887;void*eff=_T1886;struct Cyc_List_List*ec=_T1885;struct Cyc_List_List*qb=_T1884;
_T82E=yyyvsp + 4;_T82F=&(*_T82E).v;_T830=(union Cyc_YYSTYPE*)_T82F;{struct _tuple21 _T188A=Cyc_yyget_YY62(_T830);struct Cyc_Absyn_Exp*_T188B;struct Cyc_Absyn_Exp*_T188C;struct Cyc_Absyn_Exp*_T188D;struct Cyc_Absyn_Exp*_T188E;_T188E=_T188A.f0;_T188D=_T188A.f1;_T188C=_T188A.f2;_T188B=_T188A.f3;{struct Cyc_Absyn_Exp*chk=_T188E;struct Cyc_Absyn_Exp*req=_T188D;struct Cyc_Absyn_Exp*ens=_T188C;struct Cyc_Absyn_Exp*thrws=_T188B;
_T831=yyyvsp + 0;_T832=&(*_T831).v;_T833=(union Cyc_YYSTYPE*)_T832;{struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T833);{struct Cyc_Parse_Declarator _T188F;_T835=one;
_T188F.id=_T835.id;_T836=one;_T188F.varloc=_T836.varloc;_T838=yyr;{struct Cyc_List_List*_T1890=_region_malloc(_T838,0U,sizeof(struct Cyc_List_List));_T83A=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1891=_region_malloc(_T83A,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1891->tag=3;_T83C=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1892=_region_malloc(_T83C,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1892->tag=1;_T1892->f1=lis;_T1892->f2=b;_T1892->f3=c;_T1892->f4=eff;_T1892->f5=ec;_T1892->f6=qb;_T1892->f7=chk;_T1892->f8=req;_T1892->f9=ens;_T1892->f10=thrws;_T83B=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1892;}_T1891->f1=(void*)_T83B;_T839=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1891;}_T1890->hd=(void*)_T839;_T83D=one;_T1890->tl=_T83D.tms;_T837=(struct Cyc_List_List*)_T1890;}_T188F.tms=_T837;_T834=_T188F;}yyval=Cyc_YY28(_T834);goto _LL0;}}}}}case 201:
# 2038 "parse.y"
 _T83E=yyyvsp + 0;_T83F=&(*_T83E).v;_T840=(union Cyc_YYSTYPE*)_T83F;{struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T840);{struct Cyc_Parse_Declarator _T1883;_T842=one;
_T1883.id=_T842.id;_T843=one;_T1883.varloc=_T843.varloc;_T845=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T845,0U,sizeof(struct Cyc_List_List));_T847=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T847,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1885->tag=3;_T849=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T1886=_region_malloc(_T849,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T1886->tag=0;_T84A=yyyvsp + 2;_T84B=&(*_T84A).v;_T84C=(union Cyc_YYSTYPE*)_T84B;_T1886->f1=Cyc_yyget_YY37(_T84C);_T84D=yyyvsp;_T84E=_T84D[0];_T84F=_T84E.l;_T850=_T84F.first_line;_T1886->f2=Cyc_Position_loc_to_seg(_T850);_T848=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T1886;}_T1885->f1=(void*)_T848;_T846=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T846;_T851=one;_T1884->tl=_T851.tms;_T844=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T844;_T841=_T1883;}yyval=Cyc_YY28(_T841);goto _LL0;}case 202: _T853=Cyc_List_map_c;{
# 2042
struct Cyc_List_List*(*_T1883)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T853;_T852=_T1883;}_T854=yyyvsp;_T855=_T854[1];_T856=_T855.l;_T857=_T856.first_line;_T858=Cyc_Position_loc_to_seg(_T857);_T859=yyyvsp + 2;_T85A=&(*_T859).v;_T85B=(union Cyc_YYSTYPE*)_T85A;_T85C=Cyc_yyget_YY41(_T85B);_T85D=Cyc_List_imp_rev(_T85C);{struct Cyc_List_List*ts=_T852(Cyc_Parse_typ2tvar,_T858,_T85D);
_T85E=yyyvsp + 0;_T85F=&(*_T85E).v;_T860=(union Cyc_YYSTYPE*)_T85F;{struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T860);{struct Cyc_Parse_Declarator _T1883;_T862=one;
_T1883.id=_T862.id;_T863=one;_T1883.varloc=_T863.varloc;_T865=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T865,0U,sizeof(struct Cyc_List_List));_T867=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T867,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1885->tag=4;_T1885->f1=ts;_T868=yyyvsp;_T869=_T868[0];_T86A=_T869.l;_T86B=_T86A.first_line;_T1885->f2=Cyc_Position_loc_to_seg(_T86B);_T1885->f3=0;_T866=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T866;_T86C=one;_T1884->tl=_T86C.tms;_T864=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T864;_T861=_T1883;}yyval=Cyc_YY28(_T861);goto _LL0;}}case 203:
# 2046 "parse.y"
 _T86D=yyyvsp + 0;_T86E=&(*_T86D).v;_T86F=(union Cyc_YYSTYPE*)_T86E;{struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T86F);{struct Cyc_Parse_Declarator _T1883;_T871=one;
_T1883.id=_T871.id;_T872=one;_T1883.varloc=_T872.varloc;_T874=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T874,0U,sizeof(struct Cyc_List_List));_T876=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T876,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1885->tag=5;_T877=yyyvsp;_T878=_T877[1];_T879=_T878.l;_T87A=_T879.first_line;_T1885->f1=Cyc_Position_loc_to_seg(_T87A);_T87B=yyyvsp + 1;_T87C=&(*_T87B).v;_T87D=(union Cyc_YYSTYPE*)_T87C;_T1885->f2=Cyc_yyget_YY46(_T87D);_T875=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T875;_T87E=one;_T1884->tl=_T87E.tms;_T873=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T873;_T870=_T1883;}yyval=Cyc_YY28(_T870);goto _LL0;}case 204: _T87F=yyyvsp;_T880=_T87F[0];
# 2052 "parse.y"
yyval=_T880.v;goto _LL0;case 205:
# 2053 "parse.y"
 _T881=yyyvsp + 0;_T882=&(*_T881).v;_T883=(union Cyc_YYSTYPE*)_T882;_T884=Cyc_yyget_YY27(_T883);_T885=yyyvsp + 1;_T886=&(*_T885).v;_T887=(union Cyc_YYSTYPE*)_T886;_T888=Cyc_yyget_YY27(_T887);_T889=Cyc_List_imp_append(_T884,_T888);yyval=Cyc_YY27(_T889);goto _LL0;case 206:  {
# 2057 "parse.y"
struct Cyc_List_List*ans=0;
_T88A=yyyvsp + 3;_T88B=&(*_T88A).v;_T88C=(union Cyc_YYSTYPE*)_T88B;_T88D=Cyc_yyget_YY46(_T88C);if(_T88D==0)goto _TL2AD;_T88F=yyr;{struct Cyc_List_List*_T1883=_region_malloc(_T88F,0U,sizeof(struct Cyc_List_List));_T891=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1884=_region_malloc(_T891,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1884->tag=5;_T892=yyyvsp;_T893=_T892[3];_T894=_T893.l;_T895=_T894.first_line;
_T1884->f1=Cyc_Position_loc_to_seg(_T895);_T896=yyyvsp + 3;_T897=&(*_T896).v;_T898=(union Cyc_YYSTYPE*)_T897;_T1884->f2=Cyc_yyget_YY46(_T898);_T890=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1884;}_T1883->hd=(void*)_T890;_T1883->tl=ans;_T88E=(struct Cyc_List_List*)_T1883;}ans=_T88E;goto _TL2AE;_TL2AD: _TL2AE: {
# 2061
struct Cyc_Absyn_PtrLoc*ptrloc=0;
_T899=yyyvsp + 0;_T89A=&(*_T899).v;_T89B=(union Cyc_YYSTYPE*)_T89A;_T89C=Cyc_yyget_YY1(_T89B);{struct _tuple22 _T1883=*_T89C;void*_T1884;void*_T1885;unsigned _T1886;_T1886=_T1883.f0;_T1885=_T1883.f1;_T1884=_T1883.f2;{unsigned ploc=_T1886;void*nullable=_T1885;void*bound=_T1884;_T89D=Cyc_Flags_porting_c_code;
if(!_T89D)goto _TL2AF;{struct Cyc_Absyn_PtrLoc*_T1887=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));
_T1887->ptr_loc=ploc;_T89F=yyyvsp;_T8A0=_T89F[2];_T8A1=_T8A0.l;_T8A2=_T8A1.first_line;_T1887->rgn_loc=Cyc_Position_loc_to_seg(_T8A2);_T8A3=yyyvsp;_T8A4=_T8A3[1];_T8A5=_T8A4.l;_T8A6=_T8A5.first_line;_T1887->zt_loc=Cyc_Position_loc_to_seg(_T8A6);_T89E=(struct Cyc_Absyn_PtrLoc*)_T1887;}ptrloc=_T89E;goto _TL2B0;_TL2AF: _TL2B0: _T8A7=yyr;_T8A8=ptrloc;_T8A9=nullable;_T8AA=bound;
_T8AB=yyyvsp + 2;_T8AC=&(*_T8AB).v;_T8AD=(union Cyc_YYSTYPE*)_T8AC;_T8AE=Cyc_yyget_YY45(_T8AD);_T8AF=yyyvsp + 1;_T8B0=&(*_T8AF).v;_T8B1=(union Cyc_YYSTYPE*)_T8B0;_T8B2=Cyc_yyget_YY60(_T8B1);_T8B3=yyyvsp + 4;_T8B4=&(*_T8B3).v;_T8B5=(union Cyc_YYSTYPE*)_T8B4;_T8B6=Cyc_yyget_YY24(_T8B5);{void*mod=Cyc_Parse_make_pointer_mod(_T8A7,_T8A8,_T8A9,_T8AA,_T8AE,_T8B2,_T8B6);_T8B8=yyr;{struct Cyc_List_List*_T1887=_region_malloc(_T8B8,0U,sizeof(struct Cyc_List_List));
_T1887->hd=mod;_T1887->tl=ans;_T8B7=(struct Cyc_List_List*)_T1887;}ans=_T8B7;
yyval=Cyc_YY27(ans);goto _LL0;}}}}}case 207:
# 2071
 yyval=Cyc_YY60(0);goto _LL0;case 208: _T8BA=yyr;{struct Cyc_List_List*_T1883=_region_malloc(_T8BA,0U,sizeof(struct Cyc_List_List));
# 2072 "parse.y"
_T8BB=yyyvsp + 0;_T8BC=&(*_T8BB).v;_T8BD=(union Cyc_YYSTYPE*)_T8BC;_T1883->hd=Cyc_yyget_YY59(_T8BD);_T8BE=yyyvsp + 1;_T8BF=&(*_T8BE).v;_T8C0=(union Cyc_YYSTYPE*)_T8BF;_T1883->tl=Cyc_yyget_YY60(_T8C0);_T8B9=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY60(_T8B9);goto _LL0;case 209: _T8C2=yyr;{struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8C2,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_T1883->tag=0;
# 2077 "parse.y"
_T8C3=yyyvsp + 2;_T8C4=&(*_T8C3).v;_T8C5=(union Cyc_YYSTYPE*)_T8C4;_T1883->f1=Cyc_yyget_Exp_tok(_T8C5);_T8C1=(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8C6=(void*)_T8C1;yyval=Cyc_YY59(_T8C6);goto _LL0;case 210: _T8C7=yyyvsp;_T8C8=_T8C7[0];_T8C9=_T8C8.l;_T8CA=_T8C9.first_line;_T8CB=
# 2079 "parse.y"
Cyc_Position_loc_to_seg(_T8CA);_T8CC=_tag_fat("@region qualifiers are deprecated; use @effect instead",sizeof(char),55U);_T8CD=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T8CB,_T8CC,_T8CD);_T8CF=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8CF,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T1883->tag=2;{struct Cyc_List_List*_T1884=_cycalloc(sizeof(struct Cyc_List_List));
_T8D1=yyyvsp + 2;_T8D2=&(*_T8D1).v;_T8D3=(union Cyc_YYSTYPE*)_T8D2;_T1884->hd=Cyc_yyget_YY45(_T8D3);_T1884->tl=0;_T8D0=(struct Cyc_List_List*)_T1884;}_T1883->f1=_T8D0;_T8CE=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8D4=(void*)_T8CE;yyval=Cyc_YY59(_T8D4);goto _LL0;case 211: _T8D6=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8D6,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T1883->tag=2;
# 2081 "parse.y"
_T8D7=yyyvsp + 2;_T8D8=&(*_T8D7).v;_T8D9=(union Cyc_YYSTYPE*)_T8D8;_T1883->f1=Cyc_yyget_YY41(_T8D9);_T8D5=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8DA=(void*)_T8D5;yyval=Cyc_YY59(_T8DA);goto _LL0;case 212: _T8DC=yyr;{struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8DC,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));
# 2082 "parse.y"
_T1883->tag=3;_T8DB=(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8DD=(void*)_T8DB;yyval=Cyc_YY59(_T8DD);goto _LL0;case 213: _T8DF=yyr;{struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8DF,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));
# 2083 "parse.y"
_T1883->tag=4;_T8DE=(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8E0=(void*)_T8DE;yyval=Cyc_YY59(_T8E0);goto _LL0;case 214: _T8E2=yyr;{struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8E2,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));
# 2084 "parse.y"
_T1883->tag=7;_T8E1=(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8E3=(void*)_T8E1;yyval=Cyc_YY59(_T8E3);goto _LL0;case 215: _T8E5=yyr;{struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8E5,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2085 "parse.y"
_T1883->tag=5;_T8E4=(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8E6=(void*)_T8E4;yyval=Cyc_YY59(_T8E6);goto _LL0;case 216: _T8E8=yyr;{struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8E8,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2086 "parse.y"
_T1883->tag=6;_T8E7=(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8E9=(void*)_T8E7;yyval=Cyc_YY59(_T8E9);goto _LL0;case 217: _T8EB=yyr;{struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8EB,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));
# 2087 "parse.y"
_T1883->tag=8;_T8EA=(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8EC=(void*)_T8EA;yyval=Cyc_YY59(_T8EC);goto _LL0;case 218: _T8EE=yyr;{struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8EE,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));
# 2088 "parse.y"
_T1883->tag=9;_T8ED=(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8EF=(void*)_T8ED;yyval=Cyc_YY59(_T8EF);goto _LL0;case 219: _T8F1=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T8F1,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T1883->tag=10;
# 2089 "parse.y"
_T8F2=yyyvsp + 2;_T8F3=&(*_T8F2).v;_T8F4=(union Cyc_YYSTYPE*)_T8F3;_T1883->f1=Cyc_yyget_YY58(_T8F4);_T8F0=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T8F5=(void*)_T8F0;yyval=Cyc_YY59(_T8F5);goto _LL0;case 220: _T8F6=yyyvsp;_T8F7=_T8F6[0];_T8F8=_T8F7.l;_T8F9=_T8F8.first_line;_T8FA=
# 2090 "parse.y"
Cyc_Position_loc_to_seg(_T8F9);_T8FB=yyyvsp + 0;_T8FC=&(*_T8FB).v;_T8FD=(union Cyc_YYSTYPE*)_T8FC;_T8FE=Cyc_yyget_String_tok(_T8FD);{void*aq=Cyc_Parse_id2aqual(_T8FA,_T8FE);_T900=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T1883=_region_malloc(_T900,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T1883->tag=10;_T1883->f1=aq;_T8FF=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T1883;}_T901=(void*)_T8FF;yyval=Cyc_YY59(_T901);goto _LL0;}case 221:
# 2094 "parse.y"
 _T902=yyyvsp + 0;_T903=&(*_T902).v;_T904=(union Cyc_YYSTYPE*)_T903;_T905=Cyc_yyget_YY58(_T904);yyval=Cyc_YY58(_T905);goto _LL0;case 222:
# 2100 "parse.y"
 _T906=yyyvsp + 0;_T907=&(*_T906).v;_T908=(union Cyc_YYSTYPE*)_T907;_T909=Cyc_yyget_YY45(_T908);_T90A=Cyc_Absyn_al_qual_type;_T90B=Cyc_Absyn_aqual_var_type(_T909,_T90A);yyval=Cyc_YY58(_T90B);goto _LL0;case 223:
# 2104 "parse.y"
 _T90C=yyyvsp + 2;_T90D=&(*_T90C).v;_T90E=(union Cyc_YYSTYPE*)_T90D;_T90F=Cyc_yyget_YY45(_T90E);_T910=Cyc_Absyn_aqualsof_type(_T90F);_T911=Cyc_Absyn_al_qual_type;_T912=Cyc_Absyn_aqual_var_type(_T910,_T911);yyval=Cyc_YY58(_T912);goto _LL0;case 224: goto _LL0;case 225: _T913=yyyvsp;_T914=_T913[0];_T915=_T914.l;_T916=_T915.first_line;_T917=
# 2110 "parse.y"
Cyc_Position_loc_to_seg(_T916);_T918=yyyvsp + 0;_T919=&(*_T918).v;_T91A=(union Cyc_YYSTYPE*)_T919;_T91B=Cyc_yyget_String_tok(_T91A);_T91C=Cyc_Parse_id2aqual(_T917,_T91B);yyval=Cyc_YY58(_T91C);goto _LL0;case 226:{struct _tuple22*_T1883=_cycalloc(sizeof(struct _tuple22));_T91E=yyyvsp;_T91F=_T91E[0];_T920=_T91F.l;_T921=_T920.first_line;
# 2116 "parse.y"
_T1883->f0=Cyc_Position_loc_to_seg(_T921);_T1883->f1=Cyc_Absyn_true_type;_T922=Cyc_Parse_parsing_tempest;if(!_T922)goto _TL2B1;_T1883->f2=Cyc_Absyn_fat_bound_type;goto _TL2B2;_TL2B1: _T923=yyyvsp;_T924=_T923[0];_T925=_T924.l;_T926=_T925.first_line;_T927=Cyc_Position_loc_to_seg(_T926);_T928=Cyc_Position_string_of_segment(_T927);_T929=yyyvsp + 1;_T92A=&(*_T929).v;_T92B=(union Cyc_YYSTYPE*)_T92A;_T92C=Cyc_yyget_YY2(_T92B);_T1883->f2=Cyc_Parse_assign_cvar_pos(_T928,0,_T92C);_TL2B2: _T91D=(struct _tuple22*)_T1883;}yyval=Cyc_YY1(_T91D);goto _LL0;case 227:{struct _tuple22*_T1883=_cycalloc(sizeof(struct _tuple22));_T92E=yyyvsp;_T92F=_T92E[0];_T930=_T92F.l;_T931=_T930.first_line;
# 2117 "parse.y"
_T1883->f0=Cyc_Position_loc_to_seg(_T931);_T1883->f1=Cyc_Absyn_false_type;_T932=yyyvsp;_T933=_T932[0];_T934=_T933.l;_T935=_T934.first_line;_T936=Cyc_Position_loc_to_seg(_T935);_T937=Cyc_Position_string_of_segment(_T936);_T938=yyyvsp + 1;_T939=&(*_T938).v;_T93A=(union Cyc_YYSTYPE*)_T939;_T93B=Cyc_yyget_YY2(_T93A);_T1883->f2=Cyc_Parse_assign_cvar_pos(_T937,0,_T93B);_T92D=(struct _tuple22*)_T1883;}yyval=Cyc_YY1(_T92D);goto _LL0;case 228: _T93D=Cyc_Flags_override_fat;
# 2118 "parse.y"
if(!_T93D)goto _TL2B3;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2B3;_T93E=Cyc_Flags_interproc;if(!_T93E)goto _TL2B3;{struct _tuple22*_T1883=_cycalloc(sizeof(struct _tuple22));_T940=yyyvsp;_T941=_T940[0];_T942=_T941.l;_T943=_T942.first_line;
_T1883->f0=Cyc_Position_loc_to_seg(_T943);_T1883->f1=Cyc_Absyn_true_type;_T944=yyyvsp;_T945=_T944[0];_T946=_T945.l;_T947=_T946.first_line;_T948=Cyc_Position_loc_to_seg(_T947);_T949=Cyc_Position_string_of_segment(_T948);_T94A=& Cyc_Kinds_ptrbko;_T94B=(struct Cyc_Core_Opt*)_T94A;_T94C=Cyc_Absyn_cvar_type(_T94B);_T1883->f2=Cyc_Parse_assign_cvar_pos(_T949,1,_T94C);_T93F=(struct _tuple22*)_T1883;}_T93C=_T93F;goto _TL2B4;_TL2B3:{struct _tuple22*_T1883=_cycalloc(sizeof(struct _tuple22));_T94E=yyyvsp;_T94F=_T94E[0];_T950=_T94F.l;_T951=_T950.first_line;
_T1883->f0=Cyc_Position_loc_to_seg(_T951);_T1883->f1=Cyc_Absyn_true_type;_T1883->f2=Cyc_Absyn_fat_bound_type;_T94D=(struct _tuple22*)_T1883;}_T93C=_T94D;_TL2B4:
# 2118
 yyval=Cyc_YY1(_T93C);goto _LL0;case 229: _T953=Cyc_Flags_interproc;
# 2123
if(!_T953)goto _TL2B5;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2B5;_T954=& Cyc_Kinds_ptrbko;_T955=(struct Cyc_Core_Opt*)_T954;_T952=Cyc_Absyn_cvar_type(_T955);goto _TL2B6;_TL2B5: _T952=Cyc_Absyn_bounds_one();_TL2B6: yyval=Cyc_YY2(_T952);goto _LL0;case 230:
# 2124 "parse.y"
 _T956=yyyvsp + 1;_T957=&(*_T956).v;_T958=(union Cyc_YYSTYPE*)_T957;_T959=Cyc_yyget_Exp_tok(_T958);_T95A=Cyc_Absyn_thin_bounds_exp(_T959);yyval=Cyc_YY2(_T95A);goto _LL0;case 231: _T95B=Cyc_Flags_resolve;
# 2127 "parse.y"
if(_T95B)goto _TL2B7;else{goto _TL2B9;}
_TL2B9:{int(*_T1883)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T95C=_T1883;}_T95D=_tag_fat("Type variable not permitted in pointer bound",sizeof(char),45U);_T95C(0U,_T95D);goto _TL2B8;_TL2B7: _TL2B8:
 _T95E=yyyvsp + 1;_T95F=&(*_T95E).v;_T960=(union Cyc_YYSTYPE*)_T95F;_T961=Cyc_yyget_String_tok(_T960);_T962=Cyc_Parse_typevar2cvar(_T961);yyval=Cyc_YY2(_T962);goto _LL0;case 232: _T963=
# 2134 "parse.y"
Cyc_Tcutil_any_bool(0);yyval=Cyc_YY54(_T963);goto _LL0;case 233:
# 2135 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_true_type);goto _LL0;case 234:
# 2136 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_false_type);goto _LL0;case 235:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2140 "parse.y"
_T965=yyyvsp + 0;_T966=&(*_T965).v;_T967=(union Cyc_YYSTYPE*)_T966;_T1883->hd=Cyc_yyget_YY45(_T967);_T1883->tl=0;_T964=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY41(_T964);goto _LL0;case 236:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2141 "parse.y"
_T969=yyyvsp + 0;_T96A=&(*_T969).v;_T96B=(union Cyc_YYSTYPE*)_T96A;_T1883->hd=Cyc_yyget_YY45(_T96B);_T96C=yyyvsp + 2;_T96D=&(*_T96C).v;_T96E=(union Cyc_YYSTYPE*)_T96D;_T1883->tl=Cyc_yyget_YY41(_T96E);_T968=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY41(_T968);goto _LL0;case 237: _T96F=& Cyc_Kinds_eko;_T970=(struct Cyc_Core_Opt*)_T96F;_T971=
# 2145 "parse.y"
Cyc_Absyn_new_evar(_T970,0);yyval=Cyc_YY45(_T971);goto _LL0;case 238:
# 2146 "parse.y"
 _T972=yyyvsp + 0;_T973=&(*_T972).v;_T974=(union Cyc_YYSTYPE*)_T973;{struct Cyc_List_List*es=Cyc_yyget_YY41(_T974);_T975=Cyc_List_length(es);if(_T975!=1)goto _TL2BA;_T976=_check_null(es);_T977=_T976->hd;yyval=Cyc_YY45(_T977);goto _TL2BB;_TL2BA: _T978=yyyvsp + 0;_T979=&(*_T978).v;_T97A=(union Cyc_YYSTYPE*)_T979;_T97B=Cyc_yyget_YY41(_T97A);_T97C=Cyc_Absyn_join_eff(_T97B);yyval=Cyc_YY45(_T97C);_TL2BB: goto _LL0;}case 239: _T97D=& Cyc_Kinds_eko;_T97E=(struct Cyc_Core_Opt*)_T97D;_T97F=
# 2147 "parse.y"
Cyc_Absyn_new_evar(_T97E,0);yyval=Cyc_YY45(_T97F);goto _LL0;case 240: _T980=yyvs;_T981=yyvsp_offset + 1;_T982=_check_fat_subscript(_T980,sizeof(struct Cyc_Yystacktype),_T981);_T983=(struct Cyc_Yystacktype*)_T982;_T984=*_T983;_T985=_T984.l;_T986=_T985.first_line;_T987=
# 2158 "parse.y"
Cyc_Position_loc_to_seg(_T986);_T988=Cyc_Absyn_empty_tqual(_T987);yyval=Cyc_YY24(_T988);goto _LL0;case 241:
# 2159 "parse.y"
 _T989=yyyvsp + 0;_T98A=&(*_T989).v;_T98B=(union Cyc_YYSTYPE*)_T98A;_T98C=Cyc_yyget_YY24(_T98B);_T98D=yyyvsp + 1;_T98E=&(*_T98D).v;_T98F=(union Cyc_YYSTYPE*)_T98E;_T990=Cyc_yyget_YY24(_T98F);_T991=Cyc_Absyn_combine_tqual(_T98C,_T990);yyval=Cyc_YY24(_T991);goto _LL0;case 242:
# 2164 "parse.y"
 _T992=yyyvsp + 1;_T993=&(*_T992).v;_T994=(union Cyc_YYSTYPE*)_T993;{struct _tuple28*ec_qb=Cyc_yyget_YY51(_T994);_T996=ec_qb;_T997=(unsigned)_T996;
if(!_T997)goto _TL2BC;_T998=ec_qb;_T995=*_T998;goto _TL2BD;_TL2BC:{struct _tuple28 _T1883;_T1883.f0=0;_T1883.f1=0;_T999=_T1883;}_T995=_T999;_TL2BD: {struct _tuple28 _T1883=_T995;struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_List_List*ec=_T1885;struct Cyc_List_List*qb=_T1884;{struct _tuple27*_T1886=_cycalloc(sizeof(struct _tuple27));
_T1886->f0=0;_T1886->f1=0;_T1886->f2=0;_T99B=yyyvsp + 0;_T99C=&(*_T99B).v;_T99D=(union Cyc_YYSTYPE*)_T99C;_T1886->f3=Cyc_yyget_YY50(_T99D);_T1886->f4=ec;_T1886->f5=qb;_T99A=(struct _tuple27*)_T1886;}yyval=Cyc_YY40(_T99A);goto _LL0;}}}case 243:
# 2168 "parse.y"
 _T99E=yyyvsp + 2;_T99F=&(*_T99E).v;_T9A0=(union Cyc_YYSTYPE*)_T99F;{struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9A0);_T9A2=ec_qb;_T9A3=(unsigned)_T9A2;
if(!_T9A3)goto _TL2BE;_T9A4=ec_qb;_T9A1=*_T9A4;goto _TL2BF;_TL2BE:{struct _tuple28 _T1883;_T1883.f0=0;_T1883.f1=0;_T9A5=_T1883;}_T9A1=_T9A5;_TL2BF: {struct _tuple28 _T1883=_T9A1;struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_List_List*ec=_T1885;struct Cyc_List_List*qb=_T1884;{struct _tuple27*_T1886=_cycalloc(sizeof(struct _tuple27));
_T9A7=yyyvsp + 0;_T9A8=&(*_T9A7).v;_T9A9=(union Cyc_YYSTYPE*)_T9A8;_T9AA=Cyc_yyget_YY39(_T9A9);_T1886->f0=Cyc_List_imp_rev(_T9AA);_T1886->f1=0;_T1886->f2=0;_T9AB=yyyvsp + 1;_T9AC=&(*_T9AB).v;_T9AD=(union Cyc_YYSTYPE*)_T9AC;_T1886->f3=Cyc_yyget_YY50(_T9AD);_T1886->f4=ec;_T1886->f5=qb;_T9A6=(struct _tuple27*)_T1886;}yyval=Cyc_YY40(_T9A6);goto _LL0;}}}case 244:
# 2172 "parse.y"
 _T9AE=yyyvsp + 4;_T9AF=&(*_T9AE).v;_T9B0=(union Cyc_YYSTYPE*)_T9AF;{struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9B0);_T9B2=ec_qb;_T9B3=(unsigned)_T9B2;
if(!_T9B3)goto _TL2C0;_T9B4=ec_qb;_T9B1=*_T9B4;goto _TL2C1;_TL2C0:{struct _tuple28 _T1883;_T1883.f0=0;_T1883.f1=0;_T9B5=_T1883;}_T9B1=_T9B5;_TL2C1: {struct _tuple28 _T1883=_T9B1;struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_List_List*ec=_T1885;struct Cyc_List_List*qb=_T1884;{struct _tuple27*_T1886=_cycalloc(sizeof(struct _tuple27));
_T9B7=yyyvsp + 0;_T9B8=&(*_T9B7).v;_T9B9=(union Cyc_YYSTYPE*)_T9B8;_T9BA=Cyc_yyget_YY39(_T9B9);_T1886->f0=Cyc_List_imp_rev(_T9BA);_T1886->f1=1;_T1886->f2=0;_T9BB=yyyvsp + 3;_T9BC=&(*_T9BB).v;_T9BD=(union Cyc_YYSTYPE*)_T9BC;_T1886->f3=Cyc_yyget_YY50(_T9BD);_T1886->f4=ec;_T1886->f5=qb;_T9B6=(struct _tuple27*)_T1886;}yyval=Cyc_YY40(_T9B6);goto _LL0;}}}case 245:
# 2177
 _T9BE=yyyvsp + 2;_T9BF=&(*_T9BE).v;_T9C0=(union Cyc_YYSTYPE*)_T9BF;{struct _tuple8*_T1883=Cyc_yyget_YY38(_T9C0);void*_T1884;struct Cyc_Absyn_Tqual _T1885;struct _fat_ptr*_T1886;{struct _tuple8 _T1887=*_T1883;_T1886=_T1887.f0;_T1885=_T1887.f1;_T1884=_T1887.f2;}{struct _fat_ptr*n=_T1886;struct Cyc_Absyn_Tqual tq=_T1885;void*t=_T1884;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9C1=v;_T9C1->name=n;_T9C2=v;_T9C2->tq=tq;_T9C3=v;_T9C3->type=t;_T9C4=v;_T9C5=yyyvsp + 1;_T9C6=&(*_T9C5).v;_T9C7=(union Cyc_YYSTYPE*)_T9C6;_T9C4->inject=Cyc_yyget_YY32(_T9C7);
_T9C8=yyyvsp + 4;_T9C9=&(*_T9C8).v;_T9CA=(union Cyc_YYSTYPE*)_T9C9;{struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9CA);_T9CC=ec_qb;_T9CD=(unsigned)_T9CC;
if(!_T9CD)goto _TL2C2;_T9CE=ec_qb;_T9CB=*_T9CE;goto _TL2C3;_TL2C2:{struct _tuple28 _T1887;_T1887.f0=0;_T1887.f1=0;_T9CF=_T1887;}_T9CB=_T9CF;_TL2C3: {struct _tuple28 _T1887=_T9CB;struct Cyc_List_List*_T1888;struct Cyc_List_List*_T1889;_T1889=_T1887.f0;_T1888=_T1887.f1;{struct Cyc_List_List*ec=_T1889;struct Cyc_List_List*qb=_T1888;{struct _tuple27*_T188A=_cycalloc(sizeof(struct _tuple27));
_T188A->f0=0;_T188A->f1=0;_T188A->f2=v;_T9D1=yyyvsp + 3;_T9D2=&(*_T9D1).v;_T9D3=(union Cyc_YYSTYPE*)_T9D2;_T188A->f3=Cyc_yyget_YY50(_T9D3);_T188A->f4=ec;_T188A->f5=qb;_T9D0=(struct _tuple27*)_T188A;}yyval=Cyc_YY40(_T9D0);goto _LL0;}}}}}case 246:
# 2185
 _T9D4=yyyvsp + 4;_T9D5=&(*_T9D4).v;_T9D6=(union Cyc_YYSTYPE*)_T9D5;{struct _tuple8*_T1883=Cyc_yyget_YY38(_T9D6);void*_T1884;struct Cyc_Absyn_Tqual _T1885;struct _fat_ptr*_T1886;{struct _tuple8 _T1887=*_T1883;_T1886=_T1887.f0;_T1885=_T1887.f1;_T1884=_T1887.f2;}{struct _fat_ptr*n=_T1886;struct Cyc_Absyn_Tqual tq=_T1885;void*t=_T1884;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9D7=v;_T9D7->name=n;_T9D8=v;_T9D8->tq=tq;_T9D9=v;_T9D9->type=t;_T9DA=v;_T9DB=yyyvsp + 3;_T9DC=&(*_T9DB).v;_T9DD=(union Cyc_YYSTYPE*)_T9DC;_T9DA->inject=Cyc_yyget_YY32(_T9DD);
_T9DE=yyyvsp + 6;_T9DF=&(*_T9DE).v;_T9E0=(union Cyc_YYSTYPE*)_T9DF;{struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9E0);_T9E2=ec_qb;_T9E3=(unsigned)_T9E2;
if(!_T9E3)goto _TL2C4;_T9E4=ec_qb;_T9E1=*_T9E4;goto _TL2C5;_TL2C4:{struct _tuple28 _T1887;_T1887.f0=0;_T1887.f1=0;_T9E5=_T1887;}_T9E1=_T9E5;_TL2C5: {struct _tuple28 _T1887=_T9E1;struct Cyc_List_List*_T1888;struct Cyc_List_List*_T1889;_T1889=_T1887.f0;_T1888=_T1887.f1;{struct Cyc_List_List*ec=_T1889;struct Cyc_List_List*qb=_T1888;{struct _tuple27*_T188A=_cycalloc(sizeof(struct _tuple27));
_T9E7=yyyvsp + 0;_T9E8=&(*_T9E7).v;_T9E9=(union Cyc_YYSTYPE*)_T9E8;_T9EA=Cyc_yyget_YY39(_T9E9);_T188A->f0=Cyc_List_imp_rev(_T9EA);_T188A->f1=0;_T188A->f2=v;_T9EB=yyyvsp + 5;_T9EC=&(*_T9EB).v;_T9ED=(union Cyc_YYSTYPE*)_T9EC;_T188A->f3=Cyc_yyget_YY50(_T9ED);_T188A->f4=ec;_T188A->f5=qb;_T9E6=(struct _tuple27*)_T188A;}yyval=Cyc_YY40(_T9E6);goto _LL0;}}}}}case 247:
# 2195 "parse.y"
 yyval=Cyc_YY50(0);goto _LL0;case 248: _T9EE=yyyvsp;_T9EF=_T9EE[0];_T9F0=_T9EF.l;_T9F1=_T9F0.first_line;_T9F2=
# 2196 "parse.y"
Cyc_Position_loc_to_seg(_T9F1);_T9F3=yyyvsp + 0;_T9F4=&(*_T9F3).v;_T9F5=(union Cyc_YYSTYPE*)_T9F4;_T9F6=Cyc_yyget_String_tok(_T9F5);_T9F7=Cyc_Parse_id2aqual(_T9F2,_T9F6);yyval=Cyc_YY50(_T9F7);goto _LL0;case 249:
# 2199
 _T9F8=yyyvsp + 0;_T9F9=&(*_T9F8).v;_T9FA=(union Cyc_YYSTYPE*)_T9F9;_T9FB=Cyc_yyget_String_tok(_T9FA);{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_T1883->tag=1;_T1883->f1=0;_T9FC=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T1883;}_T9FD=(void*)_T9FC;_T9FE=yyyvsp + 1;_T9FF=&(*_T9FE).v;_TA00=(union Cyc_YYSTYPE*)_T9FF;_TA01=Cyc_yyget_YY50(_TA00);_TA02=yyyvsp;_TA03=_TA02[0];_TA04=_TA03.l;_TA05=_TA04.first_line;_TA06=Cyc_Position_loc_to_seg(_TA05);_TA07=Cyc_Parse_id2type(_T9FB,_T9FD,_TA01,_TA06);yyval=Cyc_YY45(_TA07);goto _LL0;case 250:
# 2200 "parse.y"
 _TA08=yyyvsp + 0;_TA09=&(*_TA08).v;_TA0A=(union Cyc_YYSTYPE*)_TA09;_TA0B=Cyc_yyget_String_tok(_TA0A);_TA0C=yyyvsp + 2;_TA0D=&(*_TA0C).v;_TA0E=(union Cyc_YYSTYPE*)_TA0D;_TA0F=Cyc_yyget_YY44(_TA0E);_TA10=Cyc_Kinds_kind_to_bound(_TA0F);_TA11=yyyvsp + 3;_TA12=&(*_TA11).v;_TA13=(union Cyc_YYSTYPE*)_TA12;_TA14=Cyc_yyget_YY50(_TA13);_TA15=yyyvsp;_TA16=_TA15[0];_TA17=_TA16.l;_TA18=_TA17.first_line;_TA19=Cyc_Position_loc_to_seg(_TA18);_TA1A=Cyc_Parse_id2type(_TA0B,_TA10,_TA14,_TA19);yyval=Cyc_YY45(_TA1A);goto _LL0;case 251:
# 2203
 yyval=Cyc_YY50(0);goto _LL0;case 252:
# 2204 "parse.y"
 _TA1B=yyyvsp + 1;_TA1C=&(*_TA1B).v;_TA1D=(union Cyc_YYSTYPE*)_TA1C;_TA1E=Cyc_yyget_YY41(_TA1D);_TA1F=Cyc_Absyn_join_eff(_TA1E);yyval=Cyc_YY50(_TA1F);goto _LL0;case 253:
# 2208 "parse.y"
 yyval=Cyc_YY51(0);goto _LL0;case 254: _TA20=yyyvsp;_TA21=_TA20[1];
# 2209 "parse.y"
yyval=_TA21.v;goto _LL0;case 255:{struct _tuple28*_T1883=_cycalloc(sizeof(struct _tuple28));{void*_T1884[1];_TA24=_T1884 + 0;
# 2214 "parse.y"
_TA25=yyyvsp + 0;_TA26=&(*_TA25).v;_TA27=(union Cyc_YYSTYPE*)_TA26;*_TA24=Cyc_yyget_YY52(_TA27);_TA28=_tag_fat(_T1884,sizeof(void*),1);_TA23=Cyc_List_list(_TA28);}_T1883->f0=_TA23;_T1883->f1=0;_TA22=(struct _tuple28*)_T1883;}yyval=Cyc_YY51(_TA22);goto _LL0;case 256:{struct _tuple28*_T1883=_cycalloc(sizeof(struct _tuple28));
# 2216 "parse.y"
_T1883->f0=0;{struct _tuple29*_T1884[1];_TA2B=_T1884 + 0;_TA2C=yyyvsp + 0;_TA2D=&(*_TA2C).v;_TA2E=(union Cyc_YYSTYPE*)_TA2D;*_TA2B=Cyc_yyget_YY53(_TA2E);_TA2F=_tag_fat(_T1884,sizeof(struct _tuple29*),1);_TA2A=Cyc_List_list(_TA2F);}_T1883->f1=_TA2A;_TA29=(struct _tuple28*)_T1883;}yyval=Cyc_YY51(_TA29);goto _LL0;case 257:
# 2219 "parse.y"
 _TA30=yyyvsp + 2;_TA31=&(*_TA30).v;_TA32=(union Cyc_YYSTYPE*)_TA31;{struct _tuple28*rest=Cyc_yyget_YY51(_TA32);_TA33=
_check_null(rest);{struct _tuple28 _T1883=*_TA33;struct Cyc_List_List*_T1884;_T1884=_T1883.f0;{struct Cyc_List_List*rpo=_T1884;_TA34=rest;{struct Cyc_List_List*_T1885=_cycalloc(sizeof(struct Cyc_List_List));
_TA36=yyyvsp + 0;_TA37=&(*_TA36).v;_TA38=(union Cyc_YYSTYPE*)_TA37;_T1885->hd=Cyc_yyget_YY52(_TA38);_T1885->tl=rpo;_TA35=(struct Cyc_List_List*)_T1885;}(*_TA34).f0=_TA35;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 258:
# 2226 "parse.y"
 _TA39=yyyvsp + 2;_TA3A=&(*_TA39).v;_TA3B=(union Cyc_YYSTYPE*)_TA3A;{struct _tuple28*rest=Cyc_yyget_YY51(_TA3B);_TA3C=
_check_null(rest);{struct _tuple28 _T1883=*_TA3C;struct Cyc_List_List*_T1884;_T1884=_T1883.f1;{struct Cyc_List_List*qb=_T1884;_TA3D=rest;{struct Cyc_List_List*_T1885=_cycalloc(sizeof(struct Cyc_List_List));
_TA3F=yyyvsp + 0;_TA40=&(*_TA3F).v;_TA41=(union Cyc_YYSTYPE*)_TA40;_T1885->hd=Cyc_yyget_YY53(_TA41);_T1885->tl=qb;_TA3E=(struct Cyc_List_List*)_T1885;}(*_TA3D).f1=_TA3E;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 259:  {
# 2236 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_TA42=kb;_TA42->tag=2;_TA43=kb;_TA43->f1=0;_TA44=kb;_TA45=& Cyc_Kinds_ek;_TA44->f2=(struct Cyc_Absyn_Kind*)_TA45;
_TA46=yyyvsp + 2;_TA47=&(*_TA46).v;_TA48=(union Cyc_YYSTYPE*)_TA47;_TA49=Cyc_yyget_String_tok(_TA48);_TA4A=kb;_TA4B=(void*)_TA4A;_TA4C=yyyvsp;_TA4D=_TA4C[2];_TA4E=_TA4D.l;_TA4F=_TA4E.first_line;_TA50=Cyc_Position_loc_to_seg(_TA4F);{void*t=Cyc_Parse_id2type(_TA49,_TA4B,0,_TA50);{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct));_T1883->tag=2;
_TA52=yyyvsp + 0;_TA53=&(*_TA52).v;_TA54=(union Cyc_YYSTYPE*)_TA53;_TA55=Cyc_yyget_YY41(_TA54);_T1883->f1=Cyc_Parse_effect_from_atomic(_TA55);_T1883->f2=t;_TA51=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T1883;}_TA56=(void*)_TA51;yyval=Cyc_YY52(_TA56);goto _LL0;}}case 260:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_T1883->tag=1;
# 2242 "parse.y"
_TA58=yyyvsp + 0;_TA59=&(*_TA58).v;_TA5A=(union Cyc_YYSTYPE*)_TA59;_TA5B=Cyc_yyget_YY41(_TA5A);_T1883->f1=Cyc_Parse_effect_from_atomic(_TA5B);
_TA5C=yyyvsp + 2;_TA5D=&(*_TA5C).v;_TA5E=(union Cyc_YYSTYPE*)_TA5D;_TA5F=Cyc_yyget_YY41(_TA5E);_T1883->f2=Cyc_Parse_effect_from_atomic(_TA5F);_TA57=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T1883;}_TA60=(void*)_TA57;
# 2242
yyval=Cyc_YY52(_TA60);goto _LL0;case 261: _TA61=yyyvsp;_TA62=_TA61[0];_TA63=_TA62.l;_TA64=_TA63.first_line;_TA65=
# 2247 "parse.y"
Cyc_Position_loc_to_seg(_TA64);_TA66=yyyvsp + 0;_TA67=&(*_TA66).v;_TA68=(union Cyc_YYSTYPE*)_TA67;_TA69=Cyc_yyget_String_tok(_TA68);Cyc_Parse_check_single_constraint(_TA65,_TA69);{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_T1883->tag=0;
_TA6B=yyyvsp + 2;_TA6C=&(*_TA6B).v;_TA6D=(union Cyc_YYSTYPE*)_TA6C;_TA6E=Cyc_yyget_YY41(_TA6D);_T1883->f1=Cyc_Parse_effect_from_atomic(_TA6E);_TA6A=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T1883;}_TA6F=(void*)_TA6A;yyval=Cyc_YY52(_TA6F);goto _LL0;case 262:{struct _tuple29*_T1883=_cycalloc(sizeof(struct _tuple29));
# 2255 "parse.y"
_TA71=yyyvsp + 2;_TA72=&(*_TA71).v;_TA73=(union Cyc_YYSTYPE*)_TA72;_T1883->f0=Cyc_yyget_YY45(_TA73);_TA74=yyyvsp + 0;_TA75=&(*_TA74).v;_TA76=(union Cyc_YYSTYPE*)_TA75;_T1883->f1=Cyc_yyget_YY45(_TA76);_TA70=(struct _tuple29*)_T1883;}yyval=Cyc_YY53(_TA70);goto _LL0;case 263:
# 2260 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_al_qual_type);goto _LL0;case 264:
# 2261 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_un_qual_type);goto _LL0;case 265:
# 2262 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rc_qual_type);goto _LL0;case 266:
# 2263 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rtd_qual_type);goto _LL0;case 267: _TA77=yyyvsp;_TA78=_TA77[0];_TA79=_TA78.l;_TA7A=_TA79.first_line;_TA7B=
# 2264 "parse.y"
Cyc_Position_loc_to_seg(_TA7A);_TA7C=yyyvsp + 0;_TA7D=&(*_TA7C).v;_TA7E=(union Cyc_YYSTYPE*)_TA7D;_TA7F=Cyc_yyget_String_tok(_TA7E);_TA80=Cyc_Parse_id2aqual(_TA7B,_TA7F);yyval=Cyc_YY58(_TA80);goto _LL0;case 268:
# 2269 "parse.y"
 _TA81=yyyvsp + 0;_TA82=&(*_TA81).v;_TA83=(union Cyc_YYSTYPE*)_TA82;_TA84=Cyc_yyget_YY58(_TA83);yyval=Cyc_YY45(_TA84);goto _LL0;case 269:
# 2272 "parse.y"
 _TA85=yyyvsp + 2;_TA86=&(*_TA85).v;_TA87=(union Cyc_YYSTYPE*)_TA86;_TA88=Cyc_yyget_YY45(_TA87);_TA89=Cyc_Absyn_aqualsof_type(_TA88);_TA8A=Cyc_Absyn_al_qual_type;_TA8B=Cyc_Absyn_aqual_var_type(_TA89,_TA8A);yyval=Cyc_YY45(_TA8B);goto _LL0;case 270:  {
# 2279 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_TA8C=kb;_TA8C->tag=0;_TA8D=kb;_TA8E=& Cyc_Kinds_aqk;_TA8D->f1=(struct Cyc_Absyn_Kind*)_TA8E;
_TA8F=yyyvsp + 0;_TA90=&(*_TA8F).v;_TA91=(union Cyc_YYSTYPE*)_TA90;_TA92=Cyc_yyget_String_tok(_TA91);_TA93=kb;_TA94=(void*)_TA93;_TA95=yyyvsp;_TA96=_TA95[0];_TA97=_TA96.l;_TA98=_TA97.first_line;_TA99=Cyc_Position_loc_to_seg(_TA98);_TA9A=Cyc_Parse_id2type(_TA92,_TA94,0,_TA99);yyval=Cyc_YY45(_TA9A);goto _LL0;}case 271:
# 2284 "parse.y"
 _TA9B=yyyvsp + 2;_TA9C=&(*_TA9B).v;_TA9D=(union Cyc_YYSTYPE*)_TA9C;_TA9E=Cyc_yyget_YY45(_TA9D);_TA9F=Cyc_Absyn_aqualsof_type(_TA9E);yyval=Cyc_YY45(_TA9F);goto _LL0;case 272:
# 2313 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 273:
# 2315 "parse.y"
 _TAA0=yyyvsp + 0;_TAA1=&(*_TAA0).v;_TAA2=(union Cyc_YYSTYPE*)_TAA1;_TAA3=Cyc_yyget_String_tok(_TAA2);_TAA4=_tag_fat("inject",sizeof(char),7U);_TAA5=Cyc_zstrcmp(_TAA3,_TAA4);if(_TAA5==0)goto _TL2C6;{struct Cyc_Warn_String_Warn_Warg_struct _T1883;_T1883.tag=0;
_T1883.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_TAA6=_T1883;}{struct Cyc_Warn_String_Warn_Warg_struct _T1883=_TAA6;void*_T1884[1];_TAA7=_T1884 + 0;*_TAA7=& _T1883;_TAA8=yyyvsp;_TAA9=_TAA8[0];_TAAA=_TAA9.l;_TAAB=_TAAA.first_line;_TAAC=Cyc_Position_loc_to_seg(_TAAB);_TAAD=_tag_fat(_T1884,sizeof(void*),1);Cyc_Warn_err2(_TAAC,_TAAD);}goto _TL2C7;_TL2C6: _TL2C7:
 yyval=Cyc_YY32(1);goto _LL0;case 274: _TAAE=yyyvsp;_TAAF=_TAAE[0];
# 2322 "parse.y"
yyval=_TAAF.v;goto _LL0;case 275:
# 2323 "parse.y"
 _TAB0=yyyvsp + 0;_TAB1=&(*_TAB0).v;_TAB2=(union Cyc_YYSTYPE*)_TAB1;_TAB3=Cyc_yyget_YY41(_TAB2);_TAB4=yyyvsp + 2;_TAB5=&(*_TAB4).v;_TAB6=(union Cyc_YYSTYPE*)_TAB5;_TAB7=Cyc_yyget_YY41(_TAB6);_TAB8=Cyc_List_imp_append(_TAB3,_TAB7);yyval=Cyc_YY41(_TAB8);goto _LL0;case 276:
# 2327 "parse.y"
 yyval=Cyc_YY41(0);goto _LL0;case 277: _TAB9=yyyvsp;_TABA=_TAB9[1];
# 2328 "parse.y"
yyval=_TABA.v;goto _LL0;case 278:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2329 "parse.y"
_TABC=yyyvsp + 2;_TABD=&(*_TABC).v;_TABE=(union Cyc_YYSTYPE*)_TABD;_TABF=Cyc_yyget_YY45(_TABE);_T1883->hd=Cyc_Absyn_regionsof_eff(_TABF);_T1883->tl=0;_TABB=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY41(_TABB);goto _LL0;case 279:
# 2331 "parse.y"
 _TAC0=yyyvsp + 0;_TAC1=&(*_TAC0).v;_TAC2=(union Cyc_YYSTYPE*)_TAC1;_TAC3=Cyc_yyget_YY45(_TAC2);_TAC4=& Cyc_Kinds_ek;_TAC5=(struct Cyc_Absyn_Kind*)_TAC4;Cyc_Parse_set_vartyp_kind(_TAC3,_TAC5,0);{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
_TAC7=yyyvsp + 0;_TAC8=&(*_TAC7).v;_TAC9=(union Cyc_YYSTYPE*)_TAC8;_T1883->hd=Cyc_yyget_YY45(_TAC9);_T1883->tl=0;_TAC6=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY41(_TAC6);goto _LL0;case 280:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2339 "parse.y"
_TACB=yyyvsp + 0;_TACC=&(*_TACB).v;_TACD=(union Cyc_YYSTYPE*)_TACC;_TACE=Cyc_yyget_YY38(_TACD);_TACF=yyyvsp;_TAD0=_TACF[0];_TAD1=_TAD0.l;_TAD2=_TAD1.first_line;_TAD3=Cyc_Position_loc_to_seg(_TAD2);_T1883->hd=Cyc_Parse_type_name_to_type(_TACE,_TAD3);_T1883->tl=0;_TACA=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY41(_TACA);goto _LL0;case 281:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2341 "parse.y"
_TAD5=yyyvsp + 0;_TAD6=&(*_TAD5).v;_TAD7=(union Cyc_YYSTYPE*)_TAD6;_TAD8=Cyc_yyget_YY38(_TAD7);_TAD9=yyyvsp;_TADA=_TAD9[0];_TADB=_TADA.l;_TADC=_TADB.first_line;_TADD=Cyc_Position_loc_to_seg(_TADC);_T1883->hd=Cyc_Parse_type_name_to_type(_TAD8,_TADD);_TADE=yyyvsp + 2;_TADF=&(*_TADE).v;_TAE0=(union Cyc_YYSTYPE*)_TADF;_T1883->tl=Cyc_yyget_YY41(_TAE0);_TAD4=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY41(_TAD4);goto _LL0;case 282:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2346 "parse.y"
_TAE2=yyyvsp + 0;_TAE3=&(*_TAE2).v;_TAE4=(union Cyc_YYSTYPE*)_TAE3;_T1883->hd=Cyc_yyget_YY38(_TAE4);_T1883->tl=0;_TAE1=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY39(_TAE1);goto _LL0;case 283:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2347 "parse.y"
_TAE6=yyyvsp + 2;_TAE7=&(*_TAE6).v;_TAE8=(union Cyc_YYSTYPE*)_TAE7;_T1883->hd=Cyc_yyget_YY38(_TAE8);_TAE9=yyyvsp + 0;_TAEA=&(*_TAE9).v;_TAEB=(union Cyc_YYSTYPE*)_TAEA;_T1883->tl=Cyc_yyget_YY39(_TAEB);_TAE5=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY39(_TAE5);goto _LL0;case 284:
# 2353 "parse.y"
 _TAEC=yyyvsp + 0;_TAED=&(*_TAEC).v;_TAEE=(union Cyc_YYSTYPE*)_TAED;{struct _tuple26 _T1883=Cyc_yyget_YY36(_TAEE);struct Cyc_List_List*_T1884;struct Cyc_Parse_Type_specifier _T1885;struct Cyc_Absyn_Tqual _T1886;_T1886=_T1883.f0;_T1885=_T1883.f1;_T1884=_T1883.f2;{struct Cyc_Absyn_Tqual tq=_T1886;struct Cyc_Parse_Type_specifier tspecs=_T1885;struct Cyc_List_List*atts=_T1884;_TAEF=tq;_TAF0=_TAEF.loc;
if(_TAF0!=0U)goto _TL2C8;_TAF1=yyyvsp;_TAF2=_TAF1[0];_TAF3=_TAF2.l;_TAF4=_TAF3.first_line;tq.loc=Cyc_Position_loc_to_seg(_TAF4);goto _TL2C9;_TL2C8: _TL2C9:
 _TAF5=yyyvsp + 1;_TAF6=&(*_TAF5).v;_TAF7=(union Cyc_YYSTYPE*)_TAF6;{struct Cyc_Parse_Declarator _T1887=Cyc_yyget_YY28(_TAF7);struct Cyc_List_List*_T1888;unsigned _T1889;struct _tuple0*_T188A;_T188A=_T1887.id;_T1889=_T1887.varloc;_T1888=_T1887.tms;{struct _tuple0*qv=_T188A;unsigned varloc=_T1889;struct Cyc_List_List*tms=_T1888;_TAF8=tspecs;_TAF9=yyyvsp;_TAFA=_TAF9[0];_TAFB=_TAFA.l;_TAFC=_TAFB.first_line;_TAFD=
Cyc_Position_loc_to_seg(_TAFC);{void*t=Cyc_Parse_speclist2typ(_TAF8,_TAFD);
struct _tuple14 _T188B=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T188C;struct Cyc_List_List*_T188D;void*_T188E;struct Cyc_Absyn_Tqual _T188F;_T188F=_T188B.f0;_T188E=_T188B.f1;_T188D=_T188B.f2;_T188C=_T188B.f3;{struct Cyc_Absyn_Tqual tq2=_T188F;void*t2=_T188E;struct Cyc_List_List*tvs=_T188D;struct Cyc_List_List*atts2=_T188C;
if(tvs==0)goto _TL2CA;{struct Cyc_Warn_String_Warn_Warg_struct _T1890;_T1890.tag=0;
_T1890.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_TAFE=_T1890;}{struct Cyc_Warn_String_Warn_Warg_struct _T1890=_TAFE;void*_T1891[1];_TAFF=_T1891 + 0;*_TAFF=& _T1890;_TB00=yyyvsp;_TB01=_TB00[1];_TB02=_TB01.l;_TB03=_TB02.first_line;_TB04=Cyc_Position_loc_to_seg(_TB03);_TB05=_tag_fat(_T1891,sizeof(void*),1);Cyc_Warn_err2(_TB04,_TB05);}goto _TL2CB;_TL2CA: _TL2CB: _TB06=
Cyc_Absyn_is_qvar_qualified(qv);if(!_TB06)goto _TL2CC;{struct Cyc_Warn_String_Warn_Warg_struct _T1890;_T1890.tag=0;
_T1890.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_TB07=_T1890;}{struct Cyc_Warn_String_Warn_Warg_struct _T1890=_TB07;void*_T1891[1];_TB08=_T1891 + 0;*_TB08=& _T1890;_TB09=yyyvsp;_TB0A=_TB09[0];_TB0B=_TB0A.l;_TB0C=_TB0B.first_line;_TB0D=Cyc_Position_loc_to_seg(_TB0C);_TB0E=_tag_fat(_T1891,sizeof(void*),1);Cyc_Warn_err2(_TB0D,_TB0E);}goto _TL2CD;_TL2CC: _TL2CD: _TB0F=qv;_TB10=*_TB0F;{
struct _fat_ptr*idopt=_TB10.f1;
if(atts2==0)goto _TL2CE;{struct Cyc_Warn_String_Warn_Warg_struct _T1890;_T1890.tag=0;
_T1890.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_TB11=_T1890;}{struct Cyc_Warn_String_Warn_Warg_struct _T1890=_TB11;void*_T1891[1];_TB12=_T1891 + 0;*_TB12=& _T1890;_TB13=yyyvsp;_TB14=_TB13[0];_TB15=_TB14.l;_TB16=_TB15.first_line;_TB17=Cyc_Position_loc_to_seg(_TB16);_TB18=_tag_fat(_T1891,sizeof(void*),1);Cyc_Warn_warn2(_TB17,_TB18);}goto _TL2CF;_TL2CE: _TL2CF:{struct _tuple8*_T1890=_cycalloc(sizeof(struct _tuple8));
_T1890->f0=idopt;_T1890->f1=tq2;_T1890->f2=t2;_TB19=(struct _tuple8*)_T1890;}yyval=Cyc_YY38(_TB19);goto _LL0;}}}}}}}case 285:
# 2368 "parse.y"
 _TB1A=yyyvsp + 0;_TB1B=&(*_TB1A).v;_TB1C=(union Cyc_YYSTYPE*)_TB1B;{struct _tuple26 _T1883=Cyc_yyget_YY36(_TB1C);struct Cyc_List_List*_T1884;struct Cyc_Parse_Type_specifier _T1885;struct Cyc_Absyn_Tqual _T1886;_T1886=_T1883.f0;_T1885=_T1883.f1;_T1884=_T1883.f2;{struct Cyc_Absyn_Tqual tq=_T1886;struct Cyc_Parse_Type_specifier tspecs=_T1885;struct Cyc_List_List*atts=_T1884;_TB1D=tq;_TB1E=_TB1D.loc;
if(_TB1E!=0U)goto _TL2D0;_TB1F=yyyvsp;_TB20=_TB1F[0];_TB21=_TB20.l;_TB22=_TB21.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB22);goto _TL2D1;_TL2D0: _TL2D1: _TB23=tspecs;_TB24=yyyvsp;_TB25=_TB24[0];_TB26=_TB25.l;_TB27=_TB26.first_line;_TB28=
Cyc_Position_loc_to_seg(_TB27);{void*t=Cyc_Parse_speclist2typ(_TB23,_TB28);
if(atts==0)goto _TL2D2;{struct Cyc_Warn_String_Warn_Warg_struct _T1887;_T1887.tag=0;
_T1887.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB29=_T1887;}{struct Cyc_Warn_String_Warn_Warg_struct _T1887=_TB29;void*_T1888[1];_TB2A=_T1888 + 0;*_TB2A=& _T1887;_TB2B=yyyvsp;_TB2C=_TB2B[0];_TB2D=_TB2C.l;_TB2E=_TB2D.first_line;_TB2F=Cyc_Position_loc_to_seg(_TB2E);_TB30=_tag_fat(_T1888,sizeof(void*),1);Cyc_Warn_warn2(_TB2F,_TB30);}goto _TL2D3;_TL2D2: _TL2D3:{struct _tuple8*_T1887=_cycalloc(sizeof(struct _tuple8));
_T1887->f0=0;_T1887->f1=tq;_T1887->f2=t;_TB31=(struct _tuple8*)_T1887;}yyval=Cyc_YY38(_TB31);goto _LL0;}}}case 286:
# 2376 "parse.y"
 _TB32=yyyvsp + 0;_TB33=&(*_TB32).v;_TB34=(union Cyc_YYSTYPE*)_TB33;{struct _tuple26 _T1883=Cyc_yyget_YY36(_TB34);struct Cyc_List_List*_T1884;struct Cyc_Parse_Type_specifier _T1885;struct Cyc_Absyn_Tqual _T1886;_T1886=_T1883.f0;_T1885=_T1883.f1;_T1884=_T1883.f2;{struct Cyc_Absyn_Tqual tq=_T1886;struct Cyc_Parse_Type_specifier tspecs=_T1885;struct Cyc_List_List*atts=_T1884;_TB35=tq;_TB36=_TB35.loc;
if(_TB36!=0U)goto _TL2D4;_TB37=yyyvsp;_TB38=_TB37[0];_TB39=_TB38.l;_TB3A=_TB39.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB3A);goto _TL2D5;_TL2D4: _TL2D5: _TB3B=tspecs;_TB3C=yyyvsp;_TB3D=_TB3C[0];_TB3E=_TB3D.l;_TB3F=_TB3E.first_line;_TB40=
Cyc_Position_loc_to_seg(_TB3F);{void*t=Cyc_Parse_speclist2typ(_TB3B,_TB40);
_TB41=yyyvsp + 1;_TB42=&(*_TB41).v;_TB43=(union Cyc_YYSTYPE*)_TB42;_TB44=Cyc_yyget_YY31(_TB43);{struct Cyc_List_List*tms=_TB44.tms;
struct _tuple14 _T1887=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T1888;struct Cyc_List_List*_T1889;void*_T188A;struct Cyc_Absyn_Tqual _T188B;_T188B=_T1887.f0;_T188A=_T1887.f1;_T1889=_T1887.f2;_T1888=_T1887.f3;{struct Cyc_Absyn_Tqual tq2=_T188B;void*t2=_T188A;struct Cyc_List_List*tvs=_T1889;struct Cyc_List_List*atts2=_T1888;
if(tvs==0)goto _TL2D6;{struct Cyc_Warn_String_Warn_Warg_struct _T188C;_T188C.tag=0;
# 2383
_T188C.f1=_tag_fat("bad type parameters on formal argument, ignoring",sizeof(char),49U);_TB45=_T188C;}{struct Cyc_Warn_String_Warn_Warg_struct _T188C=_TB45;void*_T188D[1];_TB46=_T188D + 0;*_TB46=& _T188C;_TB47=yyyvsp;_TB48=_TB47[0];_TB49=_TB48.l;_TB4A=_TB49.first_line;_TB4B=
# 2382
Cyc_Position_loc_to_seg(_TB4A);_TB4C=_tag_fat(_T188D,sizeof(void*),1);Cyc_Warn_warn2(_TB4B,_TB4C);}goto _TL2D7;_TL2D6: _TL2D7:
# 2384
 if(atts2==0)goto _TL2D8;{struct Cyc_Warn_String_Warn_Warg_struct _T188C;_T188C.tag=0;
_T188C.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB4D=_T188C;}{struct Cyc_Warn_String_Warn_Warg_struct _T188C=_TB4D;void*_T188D[1];_TB4E=_T188D + 0;*_TB4E=& _T188C;_TB4F=yyyvsp;_TB50=_TB4F[0];_TB51=_TB50.l;_TB52=_TB51.first_line;_TB53=Cyc_Position_loc_to_seg(_TB52);_TB54=_tag_fat(_T188D,sizeof(void*),1);Cyc_Warn_warn2(_TB53,_TB54);}goto _TL2D9;_TL2D8: _TL2D9:{struct _tuple8*_T188C=_cycalloc(sizeof(struct _tuple8));
_T188C->f0=0;_T188C->f1=tq2;_T188C->f2=t2;_TB55=(struct _tuple8*)_T188C;}yyval=Cyc_YY38(_TB55);goto _LL0;}}}}}case 287:
# 2391 "parse.y"
 _TB56=yyyvsp + 0;_TB57=&(*_TB56).v;_TB58=(union Cyc_YYSTYPE*)_TB57;_TB59=Cyc_yyget_YY37(_TB58);_TB5A=Cyc_List_imp_rev(_TB59);yyval=Cyc_YY37(_TB5A);goto _LL0;case 288:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));
# 2394
_TB5D=yyyvsp + 0;_TB5E=&(*_TB5D).v;_TB5F=(union Cyc_YYSTYPE*)_TB5E;*_T1884=Cyc_yyget_String_tok(_TB5F);_TB5C=(struct _fat_ptr*)_T1884;}_T1883->hd=_TB5C;_T1883->tl=0;_TB5B=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY37(_TB5B);goto _LL0;case 289:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));
# 2395 "parse.y"
_TB62=yyyvsp + 2;_TB63=&(*_TB62).v;_TB64=(union Cyc_YYSTYPE*)_TB63;*_T1884=Cyc_yyget_String_tok(_TB64);_TB61=(struct _fat_ptr*)_T1884;}_T1883->hd=_TB61;_TB65=yyyvsp + 0;_TB66=&(*_TB65).v;_TB67=(union Cyc_YYSTYPE*)_TB66;_T1883->tl=Cyc_yyget_YY37(_TB67);_TB60=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY37(_TB60);goto _LL0;case 290: _TB68=yyyvsp;_TB69=_TB68[0];
# 2399 "parse.y"
yyval=_TB69.v;goto _LL0;case 291: _TB6A=yyyvsp;_TB6B=_TB6A[0];
# 2400 "parse.y"
yyval=_TB6B.v;goto _LL0;case 292:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1883->tag=35;
# 2405 "parse.y"
_T1883->f1=0;_T1883->f2=0;_TB6C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1883;}_TB6D=(void*)_TB6C;_TB6E=yyyvsp;_TB6F=_TB6E[0];_TB70=_TB6F.l;_TB71=_TB70.first_line;_TB72=Cyc_Position_loc_to_seg(_TB71);_TB73=Cyc_Absyn_new_exp(_TB6D,_TB72);yyval=Cyc_Exp_tok(_TB73);goto _LL0;case 293:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1883->tag=35;
# 2407 "parse.y"
_T1883->f1=0;_TB75=yyyvsp + 1;_TB76=&(*_TB75).v;_TB77=(union Cyc_YYSTYPE*)_TB76;_TB78=Cyc_yyget_YY5(_TB77);_T1883->f2=Cyc_List_imp_rev(_TB78);_TB74=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1883;}_TB79=(void*)_TB74;_TB7A=yyyvsp;_TB7B=_TB7A[0];_TB7C=_TB7B.l;_TB7D=_TB7C.first_line;_TB7E=Cyc_Position_loc_to_seg(_TB7D);_TB7F=Cyc_Absyn_new_exp(_TB79,_TB7E);yyval=Cyc_Exp_tok(_TB7F);goto _LL0;case 294:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1883->tag=35;
# 2409 "parse.y"
_T1883->f1=0;_TB81=yyyvsp + 1;_TB82=&(*_TB81).v;_TB83=(union Cyc_YYSTYPE*)_TB82;_TB84=Cyc_yyget_YY5(_TB83);_T1883->f2=Cyc_List_imp_rev(_TB84);_TB80=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1883;}_TB85=(void*)_TB80;_TB86=yyyvsp;_TB87=_TB86[0];_TB88=_TB87.l;_TB89=_TB88.first_line;_TB8A=Cyc_Position_loc_to_seg(_TB89);_TB8B=Cyc_Absyn_new_exp(_TB85,_TB8A);yyval=Cyc_Exp_tok(_TB8B);goto _LL0;case 295: _TB8C=yyyvsp;_TB8D=_TB8C[2];_TB8E=_TB8D.l;_TB8F=_TB8E.first_line;_TB90=
# 2411 "parse.y"
Cyc_Position_loc_to_seg(_TB8F);{struct _tuple0*_T1883=_cycalloc(sizeof(struct _tuple0));_T1883->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_TB93=yyyvsp + 2;_TB94=&(*_TB93).v;_TB95=(union Cyc_YYSTYPE*)_TB94;*_T1884=Cyc_yyget_String_tok(_TB95);_TB92=(struct _fat_ptr*)_T1884;}_T1883->f1=_TB92;_TB91=(struct _tuple0*)_T1883;}_TB96=Cyc_Absyn_uint_type;_TB97=yyyvsp;_TB98=_TB97[2];_TB99=_TB98.l;_TB9A=_TB99.first_line;_TB9B=
Cyc_Position_loc_to_seg(_TB9A);_TB9C=Cyc_Absyn_uint_exp(0U,_TB9B);{
# 2411
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_TB90,_TB91,_TB96,_TB9C,0);_TB9D=vd;
# 2414
_TB9D->tq.real_const=1;{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_T1883->tag=26;
_T1883->f1=vd;_TB9F=yyyvsp + 4;_TBA0=&(*_TB9F).v;_TBA1=(union Cyc_YYSTYPE*)_TBA0;_T1883->f2=Cyc_yyget_Exp_tok(_TBA1);_TBA2=yyyvsp + 6;_TBA3=&(*_TBA2).v;_TBA4=(union Cyc_YYSTYPE*)_TBA3;_T1883->f3=Cyc_yyget_Exp_tok(_TBA4);_T1883->f4=0;_TB9E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T1883;}_TBA5=(void*)_TB9E;_TBA6=yyyvsp;_TBA7=_TBA6[0];_TBA8=_TBA7.l;_TBA9=_TBA8.first_line;_TBAA=Cyc_Position_loc_to_seg(_TBA9);_TBAB=Cyc_Absyn_new_exp(_TBA5,_TBAA);yyval=Cyc_Exp_tok(_TBAB);goto _LL0;}case 296:
# 2419 "parse.y"
 _TBAC=yyyvsp + 6;_TBAD=&(*_TBAC).v;_TBAE=(union Cyc_YYSTYPE*)_TBAD;_TBAF=Cyc_yyget_YY38(_TBAE);_TBB0=yyyvsp;_TBB1=_TBB0[6];_TBB2=_TBB1.l;_TBB3=_TBB2.first_line;_TBB4=Cyc_Position_loc_to_seg(_TBB3);{void*t=Cyc_Parse_type_name_to_type(_TBAF,_TBB4);{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_T1883->tag=27;
_TBB6=yyyvsp + 4;_TBB7=&(*_TBB6).v;_TBB8=(union Cyc_YYSTYPE*)_TBB7;_T1883->f1=Cyc_yyget_Exp_tok(_TBB8);_T1883->f2=t;_T1883->f3=0;_TBB5=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T1883;}_TBB9=(void*)_TBB5;_TBBA=yyyvsp;_TBBB=_TBBA[0];_TBBC=_TBBB.l;_TBBD=_TBBC.first_line;_TBBE=Cyc_Position_loc_to_seg(_TBBD);_TBBF=Cyc_Absyn_new_exp(_TBB9,_TBBE);yyval=Cyc_Exp_tok(_TBBF);goto _LL0;}case 297:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1884=_cycalloc(sizeof(struct _tuple34));
# 2427 "parse.y"
_T1884->f0=0;_TBC2=yyyvsp + 0;_TBC3=&(*_TBC2).v;_TBC4=(union Cyc_YYSTYPE*)_TBC3;_T1884->f1=Cyc_yyget_Exp_tok(_TBC4);_TBC1=(struct _tuple34*)_T1884;}_T1883->hd=_TBC1;_T1883->tl=0;_TBC0=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY5(_TBC0);goto _LL0;case 298:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1884=_cycalloc(sizeof(struct _tuple34));
# 2429 "parse.y"
_TBC7=yyyvsp + 0;_TBC8=&(*_TBC7).v;_TBC9=(union Cyc_YYSTYPE*)_TBC8;_T1884->f0=Cyc_yyget_YY42(_TBC9);_TBCA=yyyvsp + 1;_TBCB=&(*_TBCA).v;_TBCC=(union Cyc_YYSTYPE*)_TBCB;_T1884->f1=Cyc_yyget_Exp_tok(_TBCC);_TBC6=(struct _tuple34*)_T1884;}_T1883->hd=_TBC6;_T1883->tl=0;_TBC5=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY5(_TBC5);goto _LL0;case 299:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1884=_cycalloc(sizeof(struct _tuple34));
# 2431 "parse.y"
_T1884->f0=0;_TBCF=yyyvsp + 2;_TBD0=&(*_TBCF).v;_TBD1=(union Cyc_YYSTYPE*)_TBD0;_T1884->f1=Cyc_yyget_Exp_tok(_TBD1);_TBCE=(struct _tuple34*)_T1884;}_T1883->hd=_TBCE;_TBD2=yyyvsp + 0;_TBD3=&(*_TBD2).v;_TBD4=(union Cyc_YYSTYPE*)_TBD3;_T1883->tl=Cyc_yyget_YY5(_TBD4);_TBCD=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY5(_TBCD);goto _LL0;case 300:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1884=_cycalloc(sizeof(struct _tuple34));
# 2433 "parse.y"
_TBD7=yyyvsp + 2;_TBD8=&(*_TBD7).v;_TBD9=(union Cyc_YYSTYPE*)_TBD8;_T1884->f0=Cyc_yyget_YY42(_TBD9);_TBDA=yyyvsp + 3;_TBDB=&(*_TBDA).v;_TBDC=(union Cyc_YYSTYPE*)_TBDB;_T1884->f1=Cyc_yyget_Exp_tok(_TBDC);_TBD6=(struct _tuple34*)_T1884;}_T1883->hd=_TBD6;_TBDD=yyyvsp + 0;_TBDE=&(*_TBDD).v;_TBDF=(union Cyc_YYSTYPE*)_TBDE;_T1883->tl=Cyc_yyget_YY5(_TBDF);_TBD5=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY5(_TBD5);goto _LL0;case 301:
# 2437 "parse.y"
 _TBE0=yyyvsp + 0;_TBE1=&(*_TBE0).v;_TBE2=(union Cyc_YYSTYPE*)_TBE1;_TBE3=Cyc_yyget_YY42(_TBE2);_TBE4=Cyc_List_imp_rev(_TBE3);yyval=Cyc_YY42(_TBE4);goto _LL0;case 302:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T1884->tag=1;{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));
# 2438 "parse.y"
_TBE8=yyyvsp + 0;_TBE9=&(*_TBE8).v;_TBEA=(union Cyc_YYSTYPE*)_TBE9;*_T1885=Cyc_yyget_String_tok(_TBEA);_TBE7=(struct _fat_ptr*)_T1885;}_T1884->f1=_TBE7;_TBE6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1884;}_T1883->hd=(void*)_TBE6;_T1883->tl=0;_TBE5=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY42(_TBE5);goto _LL0;case 303:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2443 "parse.y"
_TBEC=yyyvsp + 0;_TBED=&(*_TBEC).v;_TBEE=(union Cyc_YYSTYPE*)_TBED;_T1883->hd=Cyc_yyget_YY43(_TBEE);_T1883->tl=0;_TBEB=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY42(_TBEB);goto _LL0;case 304:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2444 "parse.y"
_TBF0=yyyvsp + 1;_TBF1=&(*_TBF0).v;_TBF2=(union Cyc_YYSTYPE*)_TBF1;_T1883->hd=Cyc_yyget_YY43(_TBF2);_TBF3=yyyvsp + 0;_TBF4=&(*_TBF3).v;_TBF5=(union Cyc_YYSTYPE*)_TBF4;_T1883->tl=Cyc_yyget_YY42(_TBF5);_TBEF=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY42(_TBEF);goto _LL0;case 305:{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_T1883->tag=0;
# 2448 "parse.y"
_TBF7=yyyvsp + 1;_TBF8=&(*_TBF7).v;_TBF9=(union Cyc_YYSTYPE*)_TBF8;_T1883->f1=Cyc_yyget_Exp_tok(_TBF9);_TBF6=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_T1883;}_TBFA=(void*)_TBF6;yyval=Cyc_YY43(_TBFA);goto _LL0;case 306:{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T1883->tag=1;{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));
# 2449 "parse.y"
_TBFD=yyyvsp + 1;_TBFE=&(*_TBFD).v;_TBFF=(union Cyc_YYSTYPE*)_TBFE;*_T1884=Cyc_yyget_String_tok(_TBFF);_TBFC=(struct _fat_ptr*)_T1884;}_T1883->f1=_TBFC;_TBFB=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1883;}_TC00=(void*)_TBFB;yyval=Cyc_YY43(_TC00);goto _LL0;case 307:
# 2454 "parse.y"
 _TC01=yyyvsp + 0;_TC02=&(*_TC01).v;_TC03=(union Cyc_YYSTYPE*)_TC02;{struct _tuple26 _T1883=Cyc_yyget_YY36(_TC03);struct Cyc_List_List*_T1884;struct Cyc_Parse_Type_specifier _T1885;struct Cyc_Absyn_Tqual _T1886;_T1886=_T1883.f0;_T1885=_T1883.f1;_T1884=_T1883.f2;{struct Cyc_Absyn_Tqual tq=_T1886;struct Cyc_Parse_Type_specifier tss=_T1885;struct Cyc_List_List*atts=_T1884;_TC04=tss;_TC05=yyyvsp;_TC06=_TC05[0];_TC07=_TC06.l;_TC08=_TC07.first_line;_TC09=
Cyc_Position_loc_to_seg(_TC08);{void*t=Cyc_Parse_speclist2typ(_TC04,_TC09);
if(atts==0)goto _TL2DA;_TC0A=yyyvsp;_TC0B=_TC0A[0];_TC0C=_TC0B.l;_TC0D=_TC0C.first_line;_TC0E=
Cyc_Position_loc_to_seg(_TC0D);_TC0F=_tag_fat("ignoring attributes in type",sizeof(char),28U);_TC10=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC0E,_TC0F,_TC10);goto _TL2DB;_TL2DA: _TL2DB:{struct _tuple8*_T1887=_cycalloc(sizeof(struct _tuple8));
_T1887->f0=0;_T1887->f1=tq;_T1887->f2=t;_TC11=(struct _tuple8*)_T1887;}yyval=Cyc_YY38(_TC11);goto _LL0;}}}case 308:
# 2461 "parse.y"
 _TC12=yyyvsp + 0;_TC13=&(*_TC12).v;_TC14=(union Cyc_YYSTYPE*)_TC13;{struct _tuple26 _T1883=Cyc_yyget_YY36(_TC14);struct Cyc_List_List*_T1884;struct Cyc_Parse_Type_specifier _T1885;struct Cyc_Absyn_Tqual _T1886;_T1886=_T1883.f0;_T1885=_T1883.f1;_T1884=_T1883.f2;{struct Cyc_Absyn_Tqual tq=_T1886;struct Cyc_Parse_Type_specifier tss=_T1885;struct Cyc_List_List*atts=_T1884;_TC15=tss;_TC16=yyyvsp;_TC17=_TC16[0];_TC18=_TC17.l;_TC19=_TC18.first_line;_TC1A=
Cyc_Position_loc_to_seg(_TC19);{void*t=Cyc_Parse_speclist2typ(_TC15,_TC1A);
_TC1B=yyyvsp + 1;_TC1C=&(*_TC1B).v;_TC1D=(union Cyc_YYSTYPE*)_TC1C;_TC1E=Cyc_yyget_YY31(_TC1D);{struct Cyc_List_List*tms=_TC1E.tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);_TC1F=t_info;_TC20=_TC1F.f2;
if(_TC20==0)goto _TL2DC;_TC21=yyyvsp;_TC22=_TC21[1];_TC23=_TC22.l;_TC24=_TC23.first_line;_TC25=
Cyc_Position_loc_to_seg(_TC24);_TC26=_tag_fat("bad type params, ignoring",sizeof(char),26U);_TC27=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC25,_TC26,_TC27);goto _TL2DD;_TL2DC: _TL2DD: _TC28=t_info;_TC29=_TC28.f3;
if(_TC29==0)goto _TL2DE;_TC2A=yyyvsp;_TC2B=_TC2A[1];_TC2C=_TC2B.l;_TC2D=_TC2C.first_line;_TC2E=
Cyc_Position_loc_to_seg(_TC2D);_TC2F=_tag_fat("bad specifiers, ignoring",sizeof(char),25U);_TC30=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC2E,_TC2F,_TC30);goto _TL2DF;_TL2DE: _TL2DF:{struct _tuple8*_T1887=_cycalloc(sizeof(struct _tuple8));
_T1887->f0=0;_TC32=t_info;_T1887->f1=_TC32.f0;_TC33=t_info;_T1887->f2=_TC33.f1;_TC31=(struct _tuple8*)_T1887;}yyval=Cyc_YY38(_TC31);goto _LL0;}}}}case 309:
# 2474 "parse.y"
 _TC34=yyyvsp + 0;_TC35=&(*_TC34).v;_TC36=(union Cyc_YYSTYPE*)_TC35;_TC37=Cyc_yyget_YY38(_TC36);_TC38=yyyvsp;_TC39=_TC38[0];_TC3A=_TC39.l;_TC3B=_TC3A.first_line;_TC3C=Cyc_Position_loc_to_seg(_TC3B);_TC3D=Cyc_Parse_type_name_to_type(_TC37,_TC3C);yyval=Cyc_YY45(_TC3D);goto _LL0;case 310: _TC3E=
# 2475 "parse.y"
Cyc_Absyn_join_eff(0);yyval=Cyc_YY45(_TC3E);goto _LL0;case 311:
# 2476 "parse.y"
 _TC3F=yyyvsp + 1;_TC40=&(*_TC3F).v;_TC41=(union Cyc_YYSTYPE*)_TC40;_TC42=Cyc_yyget_YY41(_TC41);_TC43=Cyc_Absyn_join_eff(_TC42);yyval=Cyc_YY45(_TC43);goto _LL0;case 312:
# 2477 "parse.y"
 _TC44=yyyvsp + 2;_TC45=&(*_TC44).v;_TC46=(union Cyc_YYSTYPE*)_TC45;_TC47=Cyc_yyget_YY45(_TC46);_TC48=Cyc_Absyn_regionsof_eff(_TC47);yyval=Cyc_YY45(_TC48);goto _LL0;case 313:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2478 "parse.y"
_TC4A=yyyvsp + 0;_TC4B=&(*_TC4A).v;_TC4C=(union Cyc_YYSTYPE*)_TC4B;_T1883->hd=Cyc_yyget_YY45(_TC4C);_TC4D=yyyvsp + 2;_TC4E=&(*_TC4D).v;_TC4F=(union Cyc_YYSTYPE*)_TC4E;_T1883->tl=Cyc_yyget_YY41(_TC4F);_TC49=(struct Cyc_List_List*)_T1883;}_TC50=Cyc_Absyn_join_eff(_TC49);yyval=Cyc_YY45(_TC50);goto _LL0;case 314:
# 2479 "parse.y"
 _TC51=yyyvsp + 0;_TC52=&(*_TC51).v;_TC53=(union Cyc_YYSTYPE*)_TC52;_TC54=Cyc_yyget_YY45(_TC53);yyval=Cyc_YY45(_TC54);goto _LL0;case 315:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2485 "parse.y"
_TC56=yyyvsp + 0;_TC57=&(*_TC56).v;_TC58=(union Cyc_YYSTYPE*)_TC57;_T1883->hd=Cyc_yyget_YY45(_TC58);_T1883->tl=0;_TC55=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY41(_TC55);goto _LL0;case 316:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2486 "parse.y"
_TC5A=yyyvsp + 2;_TC5B=&(*_TC5A).v;_TC5C=(union Cyc_YYSTYPE*)_TC5B;_T1883->hd=Cyc_yyget_YY45(_TC5C);_TC5D=yyyvsp + 0;_TC5E=&(*_TC5D).v;_TC5F=(union Cyc_YYSTYPE*)_TC5E;_T1883->tl=Cyc_yyget_YY41(_TC5F);_TC59=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY41(_TC59);goto _LL0;case 317:{struct Cyc_Parse_Abstractdeclarator _T1883;
# 2490 "parse.y"
_TC61=yyyvsp + 0;_TC62=&(*_TC61).v;_TC63=(union Cyc_YYSTYPE*)_TC62;_T1883.tms=Cyc_yyget_YY27(_TC63);_TC60=_T1883;}yyval=Cyc_YY31(_TC60);goto _LL0;case 318: _TC64=yyyvsp;_TC65=_TC64[0];
# 2491 "parse.y"
yyval=_TC65.v;goto _LL0;case 319:{struct Cyc_Parse_Abstractdeclarator _T1883;
# 2493 "parse.y"
_TC67=yyyvsp + 0;_TC68=&(*_TC67).v;_TC69=(union Cyc_YYSTYPE*)_TC68;_TC6A=Cyc_yyget_YY27(_TC69);_TC6B=yyyvsp + 1;_TC6C=&(*_TC6B).v;_TC6D=(union Cyc_YYSTYPE*)_TC6C;_TC6E=Cyc_yyget_YY31(_TC6D);_TC6F=_TC6E.tms;_T1883.tms=Cyc_List_imp_append(_TC6A,_TC6F);_TC66=_T1883;}yyval=Cyc_YY31(_TC66);goto _LL0;case 320: _TC70=yyyvsp;_TC71=_TC70[1];
# 2498 "parse.y"
yyval=_TC71.v;goto _LL0;case 321:{struct Cyc_Parse_Abstractdeclarator _T1883;_TC74=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_TC74,0U,sizeof(struct Cyc_List_List));_TC76=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_TC76,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1885->tag=0;
# 2500 "parse.y"
_TC77=yyyvsp + 2;_TC78=&(*_TC77).v;_TC79=(union Cyc_YYSTYPE*)_TC78;_T1885->f1=Cyc_yyget_YY54(_TC79);_TC7A=yyyvsp;_TC7B=_TC7A[2];_TC7C=_TC7B.l;_TC7D=_TC7C.first_line;_T1885->f2=Cyc_Position_loc_to_seg(_TC7D);_TC75=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_TC75;_T1884->tl=0;_TC73=(struct Cyc_List_List*)_T1884;}_T1883.tms=_TC73;_TC72=_T1883;}yyval=Cyc_YY31(_TC72);goto _LL0;case 322:{struct Cyc_Parse_Abstractdeclarator _T1883;_TC80=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_TC80,0U,sizeof(struct Cyc_List_List));_TC82=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_TC82,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1885->tag=0;
# 2502 "parse.y"
_TC83=yyyvsp + 3;_TC84=&(*_TC83).v;_TC85=(union Cyc_YYSTYPE*)_TC84;_T1885->f1=Cyc_yyget_YY54(_TC85);_TC86=yyyvsp;_TC87=_TC86[3];_TC88=_TC87.l;_TC89=_TC88.first_line;_T1885->f2=Cyc_Position_loc_to_seg(_TC89);_TC81=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_TC81;_TC8A=yyyvsp + 0;_TC8B=&(*_TC8A).v;_TC8C=(union Cyc_YYSTYPE*)_TC8B;_TC8D=Cyc_yyget_YY31(_TC8C);_T1884->tl=_TC8D.tms;_TC7F=(struct Cyc_List_List*)_T1884;}_T1883.tms=_TC7F;_TC7E=_T1883;}yyval=Cyc_YY31(_TC7E);goto _LL0;case 323:{struct Cyc_Parse_Abstractdeclarator _T1883;_TC90=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_TC90,0U,sizeof(struct Cyc_List_List));_TC92=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_TC92,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1885->tag=1;
# 2504 "parse.y"
_TC93=yyyvsp + 1;_TC94=&(*_TC93).v;_TC95=(union Cyc_YYSTYPE*)_TC94;_T1885->f1=Cyc_yyget_Exp_tok(_TC95);_TC96=yyyvsp + 3;_TC97=&(*_TC96).v;_TC98=(union Cyc_YYSTYPE*)_TC97;_T1885->f2=Cyc_yyget_YY54(_TC98);_TC99=yyyvsp;_TC9A=_TC99[3];_TC9B=_TC9A.l;_TC9C=_TC9B.first_line;_T1885->f3=Cyc_Position_loc_to_seg(_TC9C);_TC91=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_TC91;_T1884->tl=0;_TC8F=(struct Cyc_List_List*)_T1884;}_T1883.tms=_TC8F;_TC8E=_T1883;}yyval=Cyc_YY31(_TC8E);goto _LL0;case 324:{struct Cyc_Parse_Abstractdeclarator _T1883;_TC9F=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_TC9F,0U,sizeof(struct Cyc_List_List));_TCA1=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_TCA1,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1885->tag=1;
# 2506 "parse.y"
_TCA2=yyyvsp + 2;_TCA3=&(*_TCA2).v;_TCA4=(union Cyc_YYSTYPE*)_TCA3;_T1885->f1=Cyc_yyget_Exp_tok(_TCA4);_TCA5=yyyvsp + 4;_TCA6=&(*_TCA5).v;_TCA7=(union Cyc_YYSTYPE*)_TCA6;_T1885->f2=Cyc_yyget_YY54(_TCA7);_TCA8=yyyvsp;_TCA9=_TCA8[4];_TCAA=_TCA9.l;_TCAB=_TCAA.first_line;_T1885->f3=Cyc_Position_loc_to_seg(_TCAB);_TCA0=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_TCA0;
_TCAC=yyyvsp + 0;_TCAD=&(*_TCAC).v;_TCAE=(union Cyc_YYSTYPE*)_TCAD;_TCAF=Cyc_yyget_YY31(_TCAE);_T1884->tl=_TCAF.tms;_TC9E=(struct Cyc_List_List*)_T1884;}
# 2506
_T1883.tms=_TC9E;_TC9D=_T1883;}yyval=Cyc_YY31(_TC9D);goto _LL0;case 325:
# 2510 "parse.y"
 _TCB0=yyyvsp + 1;_TCB1=&(*_TCB0).v;_TCB2=(union Cyc_YYSTYPE*)_TCB1;{struct _tuple27*_T1883=Cyc_yyget_YY40(_TCB2);struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;void*_T1886;struct Cyc_Absyn_VarargInfo*_T1887;int _T1888;struct Cyc_List_List*_T1889;{struct _tuple27 _T188A=*_T1883;_T1889=_T188A.f0;_T1888=_T188A.f1;_T1887=_T188A.f2;_T1886=_T188A.f3;_T1885=_T188A.f4;_T1884=_T188A.f5;}{struct Cyc_List_List*lis=_T1889;int b=_T1888;struct Cyc_Absyn_VarargInfo*c=_T1887;void*eff=_T1886;struct Cyc_List_List*ec=_T1885;struct Cyc_List_List*qb=_T1884;
_TCB3=yyyvsp + 3;_TCB4=&(*_TCB3).v;_TCB5=(union Cyc_YYSTYPE*)_TCB4;{struct _tuple21 _T188A=Cyc_yyget_YY62(_TCB5);struct Cyc_Absyn_Exp*_T188B;struct Cyc_Absyn_Exp*_T188C;struct Cyc_Absyn_Exp*_T188D;struct Cyc_Absyn_Exp*_T188E;_T188E=_T188A.f0;_T188D=_T188A.f1;_T188C=_T188A.f2;_T188B=_T188A.f3;{struct Cyc_Absyn_Exp*chk=_T188E;struct Cyc_Absyn_Exp*req=_T188D;struct Cyc_Absyn_Exp*ens=_T188C;struct Cyc_Absyn_Exp*thrws=_T188B;{struct Cyc_Parse_Abstractdeclarator _T188F;_TCB8=yyr;{struct Cyc_List_List*_T1890=_region_malloc(_TCB8,0U,sizeof(struct Cyc_List_List));_TCBA=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1891=_region_malloc(_TCBA,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1891->tag=3;_TCBC=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1892=_region_malloc(_TCBC,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1892->tag=1;
_T1892->f1=lis;_T1892->f2=b;_T1892->f3=c;_T1892->f4=eff;_T1892->f5=ec;_T1892->f6=qb;_T1892->f7=chk;_T1892->f8=req;_T1892->f9=ens;_T1892->f10=thrws;_TCBB=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1892;}_T1891->f1=(void*)_TCBB;_TCB9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1891;}_T1890->hd=(void*)_TCB9;_T1890->tl=0;_TCB7=(struct Cyc_List_List*)_T1890;}_T188F.tms=_TCB7;_TCB6=_T188F;}yyval=Cyc_YY31(_TCB6);goto _LL0;}}}}case 326:
# 2515 "parse.y"
 _TCBD=yyyvsp + 2;_TCBE=&(*_TCBD).v;_TCBF=(union Cyc_YYSTYPE*)_TCBE;{struct _tuple27*_T1883=Cyc_yyget_YY40(_TCBF);struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;void*_T1886;struct Cyc_Absyn_VarargInfo*_T1887;int _T1888;struct Cyc_List_List*_T1889;{struct _tuple27 _T188A=*_T1883;_T1889=_T188A.f0;_T1888=_T188A.f1;_T1887=_T188A.f2;_T1886=_T188A.f3;_T1885=_T188A.f4;_T1884=_T188A.f5;}{struct Cyc_List_List*lis=_T1889;int b=_T1888;struct Cyc_Absyn_VarargInfo*c=_T1887;void*eff=_T1886;struct Cyc_List_List*ec=_T1885;struct Cyc_List_List*qb=_T1884;
_TCC0=yyyvsp + 4;_TCC1=&(*_TCC0).v;_TCC2=(union Cyc_YYSTYPE*)_TCC1;{struct _tuple21 _T188A=Cyc_yyget_YY62(_TCC2);struct Cyc_Absyn_Exp*_T188B;struct Cyc_Absyn_Exp*_T188C;struct Cyc_Absyn_Exp*_T188D;struct Cyc_Absyn_Exp*_T188E;_T188E=_T188A.f0;_T188D=_T188A.f1;_T188C=_T188A.f2;_T188B=_T188A.f3;{struct Cyc_Absyn_Exp*chk=_T188E;struct Cyc_Absyn_Exp*req=_T188D;struct Cyc_Absyn_Exp*ens=_T188C;struct Cyc_Absyn_Exp*thrws=_T188B;{struct Cyc_Parse_Abstractdeclarator _T188F;_TCC5=yyr;{struct Cyc_List_List*_T1890=_region_malloc(_TCC5,0U,sizeof(struct Cyc_List_List));_TCC7=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1891=_region_malloc(_TCC7,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1891->tag=3;_TCC9=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1892=_region_malloc(_TCC9,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1892->tag=1;
_T1892->f1=lis;
_T1892->f2=b;_T1892->f3=c;_T1892->f4=eff;_T1892->f5=ec;_T1892->f6=qb;_T1892->f7=chk;_T1892->f8=req;_T1892->f9=ens;_T1892->f10=thrws;_TCC8=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1892;}
# 2517
_T1891->f1=(void*)_TCC8;_TCC6=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1891;}_T1890->hd=(void*)_TCC6;
_TCCA=yyyvsp + 0;_TCCB=&(*_TCCA).v;_TCCC=(union Cyc_YYSTYPE*)_TCCB;_TCCD=Cyc_yyget_YY31(_TCCC);_T1890->tl=_TCCD.tms;_TCC4=(struct Cyc_List_List*)_T1890;}
# 2517
_T188F.tms=_TCC4;_TCC3=_T188F;}yyval=Cyc_YY31(_TCC3);goto _LL0;}}}}case 327: _TCCF=Cyc_List_map_c;{
# 2522
struct Cyc_List_List*(*_T1883)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_TCCF;_TCCE=_T1883;}_TCD0=yyyvsp;_TCD1=_TCD0[1];_TCD2=_TCD1.l;_TCD3=_TCD2.first_line;_TCD4=Cyc_Position_loc_to_seg(_TCD3);_TCD5=yyyvsp + 2;_TCD6=&(*_TCD5).v;_TCD7=(union Cyc_YYSTYPE*)_TCD6;_TCD8=Cyc_yyget_YY41(_TCD7);_TCD9=Cyc_List_imp_rev(_TCD8);{struct Cyc_List_List*ts=_TCCE(Cyc_Parse_typ2tvar,_TCD4,_TCD9);{struct Cyc_Parse_Abstractdeclarator _T1883;_TCDC=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_TCDC,0U,sizeof(struct Cyc_List_List));_TCDE=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_TCDE,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1885->tag=4;
_T1885->f1=ts;_TCDF=yyyvsp;_TCE0=_TCDF[1];_TCE1=_TCE0.l;_TCE2=_TCE1.first_line;_T1885->f2=Cyc_Position_loc_to_seg(_TCE2);_T1885->f3=0;_TCDD=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_TCDD;
_TCE3=yyyvsp + 0;_TCE4=&(*_TCE3).v;_TCE5=(union Cyc_YYSTYPE*)_TCE4;_TCE6=Cyc_yyget_YY31(_TCE5);_T1884->tl=_TCE6.tms;_TCDB=(struct Cyc_List_List*)_T1884;}
# 2523
_T1883.tms=_TCDB;_TCDA=_T1883;}yyval=Cyc_YY31(_TCDA);goto _LL0;}case 328:{struct Cyc_Parse_Abstractdeclarator _T1883;_TCE9=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_TCE9,0U,sizeof(struct Cyc_List_List));_TCEB=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_TCEB,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1885->tag=5;_TCEC=yyyvsp;_TCED=_TCEC[1];_TCEE=_TCED.l;_TCEF=_TCEE.first_line;
# 2527 "parse.y"
_T1885->f1=Cyc_Position_loc_to_seg(_TCEF);_TCF0=yyyvsp + 1;_TCF1=&(*_TCF0).v;_TCF2=(union Cyc_YYSTYPE*)_TCF1;_T1885->f2=Cyc_yyget_YY46(_TCF2);_TCEA=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_TCEA;_TCF3=yyyvsp + 0;_TCF4=&(*_TCF3).v;_TCF5=(union Cyc_YYSTYPE*)_TCF4;_TCF6=Cyc_yyget_YY31(_TCF5);_T1884->tl=_TCF6.tms;_TCE8=(struct Cyc_List_List*)_T1884;}_T1883.tms=_TCE8;_TCE7=_T1883;}yyval=Cyc_YY31(_TCE7);goto _LL0;case 329:{struct _tuple21 _T1883;
# 2533 "parse.y"
_TCF8=yyyvsp + 2;_TCF9=&(*_TCF8).v;_TCFA=(union Cyc_YYSTYPE*)_TCF9;_T1883.f0=Cyc_yyget_Exp_tok(_TCFA);_T1883.f1=0;_T1883.f2=0;_T1883.f3=0;_TCF7=_T1883;}yyval=Cyc_YY62(_TCF7);goto _LL0;case 330:{struct _tuple21 _T1883;
# 2535 "parse.y"
_T1883.f0=0;_TCFC=yyyvsp + 2;_TCFD=&(*_TCFC).v;_TCFE=(union Cyc_YYSTYPE*)_TCFD;_T1883.f1=Cyc_yyget_Exp_tok(_TCFE);_T1883.f2=0;_T1883.f3=0;_TCFB=_T1883;}yyval=Cyc_YY62(_TCFB);goto _LL0;case 331:{struct _tuple21 _T1883;
# 2537 "parse.y"
_T1883.f0=0;_T1883.f1=0;_TD00=yyyvsp + 2;_TD01=&(*_TD00).v;_TD02=(union Cyc_YYSTYPE*)_TD01;_T1883.f2=Cyc_yyget_Exp_tok(_TD02);_T1883.f3=0;_TCFF=_T1883;}yyval=Cyc_YY62(_TCFF);goto _LL0;case 332:{struct _tuple21 _T1883;
# 2539 "parse.y"
_T1883.f0=0;_T1883.f1=0;_T1883.f2=0;_TD04=yyyvsp + 2;_TD05=&(*_TD04).v;_TD06=(union Cyc_YYSTYPE*)_TD05;_T1883.f3=Cyc_yyget_Exp_tok(_TD06);_TD03=_T1883;}yyval=Cyc_YY62(_TD03);goto _LL0;case 333:{struct _tuple21 _T1883;
# 2541 "parse.y"
_TD08=yyyvsp + 2;_TD09=&(*_TD08).v;_TD0A=(union Cyc_YYSTYPE*)_TD09;_T1883.f0=Cyc_yyget_Exp_tok(_TD0A);_T1883.f1=0;_T1883.f2=0;_T1883.f3=0;_TD07=_T1883;}_TD0B=yyyvsp + 4;_TD0C=&(*_TD0B).v;_TD0D=(union Cyc_YYSTYPE*)_TD0C;_TD0E=Cyc_yyget_YY62(_TD0D);_TD0F=Cyc_Parse_join_assns(_TD07,_TD0E);yyval=Cyc_YY62(_TD0F);goto _LL0;case 334:{struct _tuple21 _T1883;
# 2543 "parse.y"
_T1883.f0=0;_TD11=yyyvsp + 2;_TD12=&(*_TD11).v;_TD13=(union Cyc_YYSTYPE*)_TD12;_T1883.f1=Cyc_yyget_Exp_tok(_TD13);_T1883.f2=0;_T1883.f3=0;_TD10=_T1883;}_TD14=yyyvsp + 4;_TD15=&(*_TD14).v;_TD16=(union Cyc_YYSTYPE*)_TD15;_TD17=Cyc_yyget_YY62(_TD16);_TD18=Cyc_Parse_join_assns(_TD10,_TD17);yyval=Cyc_YY62(_TD18);goto _LL0;case 335:{struct _tuple21 _T1883;
# 2545 "parse.y"
_T1883.f0=0;_T1883.f1=0;_TD1A=yyyvsp + 2;_TD1B=&(*_TD1A).v;_TD1C=(union Cyc_YYSTYPE*)_TD1B;_T1883.f2=Cyc_yyget_Exp_tok(_TD1C);_T1883.f3=0;_TD19=_T1883;}_TD1D=yyyvsp + 4;_TD1E=&(*_TD1D).v;_TD1F=(union Cyc_YYSTYPE*)_TD1E;_TD20=Cyc_yyget_YY62(_TD1F);_TD21=Cyc_Parse_join_assns(_TD19,_TD20);yyval=Cyc_YY62(_TD21);goto _LL0;case 336:{struct _tuple21 _T1883;
# 2547 "parse.y"
_T1883.f0=0;_T1883.f1=0;_T1883.f2=0;_TD23=yyyvsp + 2;_TD24=&(*_TD23).v;_TD25=(union Cyc_YYSTYPE*)_TD24;_T1883.f3=Cyc_yyget_Exp_tok(_TD25);_TD22=_T1883;}_TD26=yyyvsp + 4;_TD27=&(*_TD26).v;_TD28=(union Cyc_YYSTYPE*)_TD27;_TD29=Cyc_yyget_YY62(_TD28);_TD2A=Cyc_Parse_join_assns(_TD22,_TD29);yyval=Cyc_YY62(_TD2A);goto _LL0;case 337:{struct _tuple21 _T1883;
# 2551 "parse.y"
_T1883.f0=0;_T1883.f1=0;_T1883.f2=0;_T1883.f3=0;_TD2B=_T1883;}yyval=Cyc_YY62(_TD2B);goto _LL0;case 338: _TD2C=yyyvsp;_TD2D=_TD2C[0];
# 2552 "parse.y"
yyval=_TD2D.v;goto _LL0;case 339: _TD2E=yyyvsp;_TD2F=_TD2E[0];
# 2557 "parse.y"
yyval=_TD2F.v;goto _LL0;case 340: _TD30=yyyvsp;_TD31=_TD30[0];
# 2558 "parse.y"
yyval=_TD31.v;goto _LL0;case 341: _TD32=yyyvsp;_TD33=_TD32[0];
# 2559 "parse.y"
yyval=_TD33.v;goto _LL0;case 342: _TD34=yyyvsp;_TD35=_TD34[0];
# 2560 "parse.y"
yyval=_TD35.v;goto _LL0;case 343: _TD36=yyyvsp;_TD37=_TD36[0];
# 2561 "parse.y"
yyval=_TD37.v;goto _LL0;case 344: _TD38=yyyvsp;_TD39=_TD38[0];
# 2562 "parse.y"
yyval=_TD39.v;goto _LL0;case 345:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1883->tag=13;{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));
# 2568 "parse.y"
_TD3C=yyyvsp + 0;_TD3D=&(*_TD3C).v;_TD3E=(union Cyc_YYSTYPE*)_TD3D;*_T1884=Cyc_yyget_String_tok(_TD3E);_TD3B=(struct _fat_ptr*)_T1884;}_T1883->f1=_TD3B;_TD3F=yyyvsp + 2;_TD40=&(*_TD3F).v;_TD41=(union Cyc_YYSTYPE*)_TD40;_T1883->f2=Cyc_yyget_Stmt_tok(_TD41);_TD3A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1883;}_TD42=(void*)_TD3A;_TD43=yyyvsp;_TD44=_TD43[0];_TD45=_TD44.l;_TD46=_TD45.first_line;_TD47=Cyc_Position_loc_to_seg(_TD46);_TD48=Cyc_Absyn_new_stmt(_TD42,_TD47);yyval=Cyc_Stmt_tok(_TD48);goto _LL0;case 346: _TD49=yyyvsp;_TD4A=_TD49[0];_TD4B=_TD4A.l;_TD4C=_TD4B.first_line;_TD4D=
# 2572 "parse.y"
Cyc_Position_loc_to_seg(_TD4C);_TD4E=Cyc_Absyn_skip_stmt(_TD4D);yyval=Cyc_Stmt_tok(_TD4E);goto _LL0;case 347:
# 2573 "parse.y"
 _TD4F=yyyvsp + 0;_TD50=&(*_TD4F).v;_TD51=(union Cyc_YYSTYPE*)_TD50;_TD52=Cyc_yyget_Exp_tok(_TD51);_TD53=yyyvsp;_TD54=_TD53[0];_TD55=_TD54.l;_TD56=_TD55.first_line;_TD57=Cyc_Position_loc_to_seg(_TD56);_TD58=Cyc_Absyn_exp_stmt(_TD52,_TD57);yyval=Cyc_Stmt_tok(_TD58);goto _LL0;case 348:
# 2578 "parse.y"
 Cyc_Parse_inside_function_definition=1;goto _LL0;case 349:
# 2582 "parse.y"
 Cyc_Parse_inside_function_definition=0;goto _LL0;case 350: _TD59=yyyvsp;_TD5A=_TD59[0];_TD5B=_TD5A.l;_TD5C=_TD5B.first_line;_TD5D=
# 2586 "parse.y"
Cyc_Position_loc_to_seg(_TD5C);_TD5E=Cyc_Absyn_skip_stmt(_TD5D);yyval=Cyc_Stmt_tok(_TD5E);goto _LL0;case 351: _TD5F=yyyvsp;_TD60=_TD5F[1];
# 2587 "parse.y"
yyval=_TD60.v;goto _LL0;case 352: _TD61=yyyvsp;_TD62=_TD61[0];_TD63=_TD62.l;_TD64=_TD63.first_line;_TD65=
# 2592 "parse.y"
Cyc_Position_loc_to_seg(_TD64);_TD66=Cyc_Absyn_skip_stmt(_TD65);yyval=Cyc_Stmt_tok(_TD66);goto _LL0;case 353: _TD67=yyyvsp;_TD68=_TD67[1];
# 2593 "parse.y"
yyval=_TD68.v;goto _LL0;case 354:
# 2598 "parse.y"
 _TD69=yyyvsp + 0;_TD6A=&(*_TD69).v;_TD6B=(union Cyc_YYSTYPE*)_TD6A;_TD6C=Cyc_yyget_YY16(_TD6B);_TD6D=yyyvsp;_TD6E=_TD6D[0];_TD6F=_TD6E.l;_TD70=_TD6F.first_line;_TD71=Cyc_Position_loc_to_seg(_TD70);_TD72=Cyc_Absyn_skip_stmt(_TD71);_TD73=Cyc_Parse_flatten_declarations(_TD6C,_TD72);yyval=Cyc_Stmt_tok(_TD73);goto _LL0;case 355:
# 2599 "parse.y"
 _TD74=yyyvsp + 0;_TD75=&(*_TD74).v;_TD76=(union Cyc_YYSTYPE*)_TD75;_TD77=Cyc_yyget_YY16(_TD76);_TD78=yyyvsp + 1;_TD79=&(*_TD78).v;_TD7A=(union Cyc_YYSTYPE*)_TD79;_TD7B=Cyc_yyget_Stmt_tok(_TD7A);_TD7C=Cyc_Parse_flatten_declarations(_TD77,_TD7B);yyval=Cyc_Stmt_tok(_TD7C);goto _LL0;case 356:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1883->tag=13;{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));
# 2601 "parse.y"
_TD7F=yyyvsp + 0;_TD80=&(*_TD7F).v;_TD81=(union Cyc_YYSTYPE*)_TD80;*_T1884=Cyc_yyget_String_tok(_TD81);_TD7E=(struct _fat_ptr*)_T1884;}_T1883->f1=_TD7E;_TD82=yyyvsp + 2;_TD83=&(*_TD82).v;_TD84=(union Cyc_YYSTYPE*)_TD83;_TD85=Cyc_yyget_YY16(_TD84);_TD86=Cyc_Absyn_skip_stmt(0U);_T1883->f2=Cyc_Parse_flatten_declarations(_TD85,_TD86);_TD7D=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1883;}_TD87=(void*)_TD7D;_TD88=yyyvsp;_TD89=_TD88[0];_TD8A=_TD89.l;_TD8B=_TD8A.first_line;_TD8C=Cyc_Position_loc_to_seg(_TD8B);_TD8D=Cyc_Absyn_new_stmt(_TD87,_TD8C);yyval=Cyc_Stmt_tok(_TD8D);goto _LL0;case 357:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1883->tag=13;{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));
# 2603 "parse.y"
_TD90=yyyvsp + 0;_TD91=&(*_TD90).v;_TD92=(union Cyc_YYSTYPE*)_TD91;*_T1884=Cyc_yyget_String_tok(_TD92);_TD8F=(struct _fat_ptr*)_T1884;}_T1883->f1=_TD8F;_TD93=yyyvsp + 2;_TD94=&(*_TD93).v;_TD95=(union Cyc_YYSTYPE*)_TD94;_TD96=Cyc_yyget_YY16(_TD95);_TD97=yyyvsp + 3;_TD98=&(*_TD97).v;_TD99=(union Cyc_YYSTYPE*)_TD98;_TD9A=Cyc_yyget_Stmt_tok(_TD99);_T1883->f2=Cyc_Parse_flatten_declarations(_TD96,_TD9A);_TD8E=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1883;}_TD9B=(void*)_TD8E;_TD9C=yyyvsp;_TD9D=_TD9C[0];_TD9E=_TD9D.l;_TD9F=_TD9E.first_line;_TDA0=Cyc_Position_loc_to_seg(_TD9F);_TDA1=Cyc_Absyn_new_stmt(_TD9B,_TDA0);yyval=Cyc_Stmt_tok(_TDA1);goto _LL0;case 358: _TDA2=yyyvsp;_TDA3=_TDA2[0];
# 2604 "parse.y"
yyval=_TDA3.v;goto _LL0;case 359:
# 2605 "parse.y"
 _TDA4=yyyvsp + 0;_TDA5=&(*_TDA4).v;_TDA6=(union Cyc_YYSTYPE*)_TDA5;_TDA7=Cyc_yyget_Stmt_tok(_TDA6);_TDA8=yyyvsp + 1;_TDA9=&(*_TDA8).v;_TDAA=(union Cyc_YYSTYPE*)_TDA9;_TDAB=Cyc_yyget_Stmt_tok(_TDAA);_TDAC=yyyvsp;_TDAD=_TDAC[0];_TDAE=_TDAD.l;_TDAF=_TDAE.first_line;_TDB0=Cyc_Position_loc_to_seg(_TDAF);_TDB1=Cyc_Absyn_seq_stmt(_TDA7,_TDAB,_TDB0);yyval=Cyc_Stmt_tok(_TDB1);goto _LL0;case 360:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1883->tag=1;
# 2606 "parse.y"
_TDB3=yyyvsp + 0;_TDB4=&(*_TDB3).v;_TDB5=(union Cyc_YYSTYPE*)_TDB4;_T1883->f1=Cyc_yyget_YY15(_TDB5);_TDB2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1883;}_TDB6=(void*)_TDB2;_TDB7=yyyvsp;_TDB8=_TDB7[0];_TDB9=_TDB8.l;_TDBA=_TDB9.first_line;_TDBB=Cyc_Position_loc_to_seg(_TDBA);_TDBC=Cyc_Absyn_new_decl(_TDB6,_TDBB);_TDBD=
Cyc_Absyn_skip_stmt(0U);_TDBE=
# 2606
Cyc_Parse_flatten_decl(_TDBC,_TDBD);yyval=Cyc_Stmt_tok(_TDBE);goto _LL0;case 361:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1883->tag=1;
# 2609 "parse.y"
_TDC0=yyyvsp + 0;_TDC1=&(*_TDC0).v;_TDC2=(union Cyc_YYSTYPE*)_TDC1;_T1883->f1=Cyc_yyget_YY15(_TDC2);_TDBF=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1883;}_TDC3=(void*)_TDBF;_TDC4=yyyvsp;_TDC5=_TDC4[0];_TDC6=_TDC5.l;_TDC7=_TDC6.first_line;_TDC8=Cyc_Position_loc_to_seg(_TDC7);_TDC9=Cyc_Absyn_new_decl(_TDC3,_TDC8);_TDCA=yyyvsp + 1;_TDCB=&(*_TDCA).v;_TDCC=(union Cyc_YYSTYPE*)_TDCB;_TDCD=Cyc_yyget_Stmt_tok(_TDCC);_TDCE=Cyc_Parse_flatten_decl(_TDC9,_TDCD);yyval=Cyc_Stmt_tok(_TDCE);goto _LL0;case 362:
# 2614 "parse.y"
 _TDCF=yyyvsp + 2;_TDD0=&(*_TDCF).v;_TDD1=(union Cyc_YYSTYPE*)_TDD0;_TDD2=Cyc_yyget_Exp_tok(_TDD1);_TDD3=yyyvsp + 4;_TDD4=&(*_TDD3).v;_TDD5=(union Cyc_YYSTYPE*)_TDD4;_TDD6=Cyc_yyget_Stmt_tok(_TDD5);_TDD7=Cyc_Absyn_skip_stmt(0U);_TDD8=yyyvsp;_TDD9=_TDD8[0];_TDDA=_TDD9.l;_TDDB=_TDDA.first_line;_TDDC=Cyc_Position_loc_to_seg(_TDDB);_TDDD=Cyc_Absyn_ifthenelse_stmt(_TDD2,_TDD6,_TDD7,_TDDC);yyval=Cyc_Stmt_tok(_TDDD);goto _LL0;case 363:
# 2616 "parse.y"
 _TDDE=yyyvsp + 2;_TDDF=&(*_TDDE).v;_TDE0=(union Cyc_YYSTYPE*)_TDDF;_TDE1=Cyc_yyget_Exp_tok(_TDE0);_TDE2=yyyvsp + 4;_TDE3=&(*_TDE2).v;_TDE4=(union Cyc_YYSTYPE*)_TDE3;_TDE5=Cyc_yyget_Stmt_tok(_TDE4);_TDE6=yyyvsp + 6;_TDE7=&(*_TDE6).v;_TDE8=(union Cyc_YYSTYPE*)_TDE7;_TDE9=Cyc_yyget_Stmt_tok(_TDE8);_TDEA=yyyvsp;_TDEB=_TDEA[0];_TDEC=_TDEB.l;_TDED=_TDEC.first_line;_TDEE=Cyc_Position_loc_to_seg(_TDED);_TDEF=Cyc_Absyn_ifthenelse_stmt(_TDE1,_TDE5,_TDE9,_TDEE);yyval=Cyc_Stmt_tok(_TDEF);goto _LL0;case 364:
# 2622 "parse.y"
 _TDF0=yyyvsp + 2;_TDF1=&(*_TDF0).v;_TDF2=(union Cyc_YYSTYPE*)_TDF1;_TDF3=Cyc_yyget_Exp_tok(_TDF2);_TDF4=yyyvsp + 5;_TDF5=&(*_TDF4).v;_TDF6=(union Cyc_YYSTYPE*)_TDF5;_TDF7=Cyc_yyget_YY8(_TDF6);_TDF8=yyyvsp;_TDF9=_TDF8[0];_TDFA=_TDF9.l;_TDFB=_TDFA.first_line;_TDFC=Cyc_Position_loc_to_seg(_TDFB);_TDFD=Cyc_Absyn_switch_stmt(_TDF3,_TDF7,_TDFC);yyval=Cyc_Stmt_tok(_TDFD);goto _LL0;case 365:
# 2625
 _TDFE=yyyvsp + 1;_TDFF=&(*_TDFE).v;_TE00=(union Cyc_YYSTYPE*)_TDFF;_TE01=Cyc_yyget_QualId_tok(_TE00);_TE02=yyyvsp;_TE03=_TE02[1];_TE04=_TE03.l;_TE05=_TE04.first_line;_TE06=Cyc_Position_loc_to_seg(_TE05);_TE07=Cyc_Absyn_unknownid_exp(_TE01,_TE06);_TE08=yyyvsp + 3;_TE09=&(*_TE08).v;_TE0A=(union Cyc_YYSTYPE*)_TE09;_TE0B=Cyc_yyget_YY8(_TE0A);_TE0C=yyyvsp;_TE0D=_TE0C[0];_TE0E=_TE0D.l;_TE0F=_TE0E.first_line;_TE10=Cyc_Position_loc_to_seg(_TE0F);_TE11=Cyc_Absyn_switch_stmt(_TE07,_TE0B,_TE10);yyval=Cyc_Stmt_tok(_TE11);goto _LL0;case 366:
# 2628
 _TE12=yyyvsp + 3;_TE13=&(*_TE12).v;_TE14=(union Cyc_YYSTYPE*)_TE13;_TE15=Cyc_yyget_YY4(_TE14);_TE16=yyyvsp;_TE17=_TE16[1];_TE18=_TE17.l;_TE19=_TE18.first_line;_TE1A=Cyc_Position_loc_to_seg(_TE19);_TE1B=Cyc_Absyn_tuple_exp(_TE15,_TE1A);_TE1C=yyyvsp + 6;_TE1D=&(*_TE1C).v;_TE1E=(union Cyc_YYSTYPE*)_TE1D;_TE1F=Cyc_yyget_YY8(_TE1E);_TE20=yyyvsp;_TE21=_TE20[0];_TE22=_TE21.l;_TE23=_TE22.first_line;_TE24=Cyc_Position_loc_to_seg(_TE23);_TE25=Cyc_Absyn_switch_stmt(_TE1B,_TE1F,_TE24);yyval=Cyc_Stmt_tok(_TE25);goto _LL0;case 367:
# 2632 "parse.y"
 _TE26=yyyvsp + 1;_TE27=&(*_TE26).v;_TE28=(union Cyc_YYSTYPE*)_TE27;_TE29=Cyc_yyget_Stmt_tok(_TE28);_TE2A=yyyvsp + 4;_TE2B=&(*_TE2A).v;_TE2C=(union Cyc_YYSTYPE*)_TE2B;_TE2D=Cyc_yyget_YY8(_TE2C);_TE2E=yyyvsp;_TE2F=_TE2E[0];_TE30=_TE2F.l;_TE31=_TE30.first_line;_TE32=Cyc_Position_loc_to_seg(_TE31);_TE33=Cyc_Absyn_trycatch_stmt(_TE29,_TE2D,_TE32);yyval=Cyc_Stmt_tok(_TE33);goto _LL0;case 368:
# 2646 "parse.y"
 yyval=Cyc_YY8(0);goto _LL0;case 369:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE36=& Cyc_Absyn_Wild_p_val;_TE37=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_TE36;_TE38=(void*)_TE37;_TE39=yyyvsp;_TE3A=_TE39[0];_TE3B=_TE3A.l;_TE3C=_TE3B.first_line;_TE3D=
# 2649 "parse.y"
Cyc_Position_loc_to_seg(_TE3C);_T1884->pattern=Cyc_Absyn_new_pat(_TE38,_TE3D);_T1884->pat_vars=0;
_T1884->where_clause=0;_TE3E=yyyvsp + 2;_TE3F=&(*_TE3E).v;_TE40=(union Cyc_YYSTYPE*)_TE3F;_T1884->body=Cyc_yyget_Stmt_tok(_TE40);_TE41=yyyvsp;_TE42=_TE41[0];_TE43=_TE42.l;_TE44=_TE43.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_TE44);_TE35=(struct Cyc_Absyn_Switch_clause*)_T1884;}
# 2649
_T1883->hd=_TE35;
_TE45=yyyvsp + 3;_TE46=&(*_TE45).v;_TE47=(union Cyc_YYSTYPE*)_TE46;_T1883->tl=Cyc_yyget_YY8(_TE47);_TE34=(struct Cyc_List_List*)_T1883;}
# 2649
yyval=Cyc_YY8(_TE34);goto _LL0;case 370:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
# 2652 "parse.y"
_TE4A=yyyvsp + 1;_TE4B=&(*_TE4A).v;_TE4C=(union Cyc_YYSTYPE*)_TE4B;_T1884->pattern=Cyc_yyget_YY9(_TE4C);_T1884->pat_vars=0;_T1884->where_clause=0;_TE4D=yyyvsp;_TE4E=_TE4D[2];_TE4F=_TE4E.l;_TE50=_TE4F.first_line;_TE51=
Cyc_Position_loc_to_seg(_TE50);_T1884->body=Cyc_Absyn_fallthru_stmt(0,_TE51);_TE52=yyyvsp;_TE53=_TE52[0];_TE54=_TE53.l;_TE55=_TE54.first_line;
_T1884->loc=Cyc_Position_loc_to_seg(_TE55);_TE49=(struct Cyc_Absyn_Switch_clause*)_T1884;}
# 2652
_T1883->hd=_TE49;
# 2654
_TE56=yyyvsp + 3;_TE57=&(*_TE56).v;_TE58=(union Cyc_YYSTYPE*)_TE57;_T1883->tl=Cyc_yyget_YY8(_TE58);_TE48=(struct Cyc_List_List*)_T1883;}
# 2652
yyval=Cyc_YY8(_TE48);goto _LL0;case 371:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
# 2656 "parse.y"
_TE5B=yyyvsp + 1;_TE5C=&(*_TE5B).v;_TE5D=(union Cyc_YYSTYPE*)_TE5C;_T1884->pattern=Cyc_yyget_YY9(_TE5D);_T1884->pat_vars=0;_T1884->where_clause=0;_TE5E=yyyvsp + 3;_TE5F=&(*_TE5E).v;_TE60=(union Cyc_YYSTYPE*)_TE5F;_T1884->body=Cyc_yyget_Stmt_tok(_TE60);_TE61=yyyvsp;_TE62=_TE61[0];_TE63=_TE62.l;_TE64=_TE63.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_TE64);_TE5A=(struct Cyc_Absyn_Switch_clause*)_T1884;}_T1883->hd=_TE5A;_TE65=yyyvsp + 4;_TE66=&(*_TE65).v;_TE67=(union Cyc_YYSTYPE*)_TE66;_T1883->tl=Cyc_yyget_YY8(_TE67);_TE59=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY8(_TE59);goto _LL0;case 372:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
# 2658 "parse.y"
_TE6A=yyyvsp + 1;_TE6B=&(*_TE6A).v;_TE6C=(union Cyc_YYSTYPE*)_TE6B;_T1884->pattern=Cyc_yyget_YY9(_TE6C);_T1884->pat_vars=0;_TE6D=yyyvsp + 3;_TE6E=&(*_TE6D).v;_TE6F=(union Cyc_YYSTYPE*)_TE6E;_T1884->where_clause=Cyc_yyget_Exp_tok(_TE6F);_TE70=yyyvsp;_TE71=_TE70[4];_TE72=_TE71.l;_TE73=_TE72.first_line;_TE74=
Cyc_Position_loc_to_seg(_TE73);_T1884->body=Cyc_Absyn_fallthru_stmt(0,_TE74);_TE75=yyyvsp;_TE76=_TE75[0];_TE77=_TE76.l;_TE78=_TE77.first_line;
_T1884->loc=Cyc_Position_loc_to_seg(_TE78);_TE69=(struct Cyc_Absyn_Switch_clause*)_T1884;}
# 2658
_T1883->hd=_TE69;
# 2660
_TE79=yyyvsp + 5;_TE7A=&(*_TE79).v;_TE7B=(union Cyc_YYSTYPE*)_TE7A;_T1883->tl=Cyc_yyget_YY8(_TE7B);_TE68=(struct Cyc_List_List*)_T1883;}
# 2658
yyval=Cyc_YY8(_TE68);goto _LL0;case 373:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));
# 2662 "parse.y"
_TE7E=yyyvsp + 1;_TE7F=&(*_TE7E).v;_TE80=(union Cyc_YYSTYPE*)_TE7F;_T1884->pattern=Cyc_yyget_YY9(_TE80);_T1884->pat_vars=0;_TE81=yyyvsp + 3;_TE82=&(*_TE81).v;_TE83=(union Cyc_YYSTYPE*)_TE82;_T1884->where_clause=Cyc_yyget_Exp_tok(_TE83);_TE84=yyyvsp + 5;_TE85=&(*_TE84).v;_TE86=(union Cyc_YYSTYPE*)_TE85;_T1884->body=Cyc_yyget_Stmt_tok(_TE86);_TE87=yyyvsp;_TE88=_TE87[0];_TE89=_TE88.l;_TE8A=_TE89.first_line;_T1884->loc=Cyc_Position_loc_to_seg(_TE8A);_TE7D=(struct Cyc_Absyn_Switch_clause*)_T1884;}_T1883->hd=_TE7D;_TE8B=yyyvsp + 6;_TE8C=&(*_TE8B).v;_TE8D=(union Cyc_YYSTYPE*)_TE8C;_T1883->tl=Cyc_yyget_YY8(_TE8D);_TE7C=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY8(_TE7C);goto _LL0;case 374:
# 2669 "parse.y"
 _TE8E=yyyvsp + 2;_TE8F=&(*_TE8E).v;_TE90=(union Cyc_YYSTYPE*)_TE8F;_TE91=Cyc_yyget_Exp_tok(_TE90);_TE92=yyyvsp + 4;_TE93=&(*_TE92).v;_TE94=(union Cyc_YYSTYPE*)_TE93;_TE95=Cyc_yyget_Stmt_tok(_TE94);_TE96=yyyvsp;_TE97=_TE96[0];_TE98=_TE97.l;_TE99=_TE98.first_line;_TE9A=Cyc_Position_loc_to_seg(_TE99);_TE9B=Cyc_Absyn_while_stmt(_TE91,_TE95,_TE9A);yyval=Cyc_Stmt_tok(_TE9B);goto _LL0;case 375:
# 2671 "parse.y"
 _TE9C=yyyvsp + 1;_TE9D=&(*_TE9C).v;_TE9E=(union Cyc_YYSTYPE*)_TE9D;_TE9F=Cyc_yyget_Stmt_tok(_TE9E);_TEA0=yyyvsp + 4;_TEA1=&(*_TEA0).v;_TEA2=(union Cyc_YYSTYPE*)_TEA1;_TEA3=Cyc_yyget_Exp_tok(_TEA2);_TEA4=yyyvsp;_TEA5=_TEA4[0];_TEA6=_TEA5.l;_TEA7=_TEA6.first_line;_TEA8=Cyc_Position_loc_to_seg(_TEA7);_TEA9=Cyc_Absyn_do_stmt(_TE9F,_TEA3,_TEA8);yyval=Cyc_Stmt_tok(_TEA9);goto _LL0;case 376:
# 2673 "parse.y"
 _TEAA=yyyvsp + 2;_TEAB=&(*_TEAA).v;_TEAC=(union Cyc_YYSTYPE*)_TEAB;_TEAD=Cyc_yyget_Exp_tok(_TEAC);_TEAE=yyyvsp + 4;_TEAF=&(*_TEAE).v;_TEB0=(union Cyc_YYSTYPE*)_TEAF;_TEB1=Cyc_yyget_Exp_tok(_TEB0);_TEB2=yyyvsp + 6;_TEB3=&(*_TEB2).v;_TEB4=(union Cyc_YYSTYPE*)_TEB3;_TEB5=Cyc_yyget_Exp_tok(_TEB4);_TEB6=yyyvsp + 8;_TEB7=&(*_TEB6).v;_TEB8=(union Cyc_YYSTYPE*)_TEB7;_TEB9=Cyc_yyget_Stmt_tok(_TEB8);_TEBA=yyyvsp;_TEBB=_TEBA[0];_TEBC=_TEBB.l;_TEBD=_TEBC.first_line;_TEBE=Cyc_Position_loc_to_seg(_TEBD);_TEBF=Cyc_Absyn_for_stmt(_TEAD,_TEB1,_TEB5,_TEB9,_TEBE);yyval=Cyc_Stmt_tok(_TEBF);goto _LL0;case 377: _TEC0=
# 2675 "parse.y"
Cyc_Absyn_false_exp(0U);_TEC1=yyyvsp + 3;_TEC2=&(*_TEC1).v;_TEC3=(union Cyc_YYSTYPE*)_TEC2;_TEC4=Cyc_yyget_Exp_tok(_TEC3);_TEC5=yyyvsp + 5;_TEC6=&(*_TEC5).v;_TEC7=(union Cyc_YYSTYPE*)_TEC6;_TEC8=Cyc_yyget_Exp_tok(_TEC7);_TEC9=yyyvsp + 7;_TECA=&(*_TEC9).v;_TECB=(union Cyc_YYSTYPE*)_TECA;_TECC=Cyc_yyget_Stmt_tok(_TECB);_TECD=yyyvsp;_TECE=_TECD[0];_TECF=_TECE.l;_TED0=_TECF.first_line;_TED1=Cyc_Position_loc_to_seg(_TED0);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_for_stmt(_TEC0,_TEC4,_TEC8,_TECC,_TED1);
_TED2=yyyvsp + 2;_TED3=&(*_TED2).v;_TED4=(union Cyc_YYSTYPE*)_TED3;_TED5=Cyc_yyget_YY16(_TED4);_TED6=s;_TED7=Cyc_Parse_flatten_declarations(_TED5,_TED6);yyval=Cyc_Stmt_tok(_TED7);goto _LL0;}case 378: _TED8=
# 2679
Cyc_Absyn_true_exp(0U);yyval=Cyc_Exp_tok(_TED8);goto _LL0;case 379: _TED9=yyyvsp;_TEDA=_TED9[0];
# 2680 "parse.y"
yyval=_TEDA.v;goto _LL0;case 380:{struct _fat_ptr*_T1883=_cycalloc(sizeof(struct _fat_ptr));
# 2683
_TEDC=yyyvsp + 1;_TEDD=&(*_TEDC).v;_TEDE=(union Cyc_YYSTYPE*)_TEDD;*_T1883=Cyc_yyget_String_tok(_TEDE);_TEDB=(struct _fat_ptr*)_T1883;}_TEDF=yyyvsp;_TEE0=_TEDF[0];_TEE1=_TEE0.l;_TEE2=_TEE1.first_line;_TEE3=Cyc_Position_loc_to_seg(_TEE2);_TEE4=Cyc_Absyn_goto_stmt(_TEDB,_TEE3);yyval=Cyc_Stmt_tok(_TEE4);goto _LL0;case 381: _TEE5=yyyvsp;_TEE6=_TEE5[0];_TEE7=_TEE6.l;_TEE8=_TEE7.first_line;_TEE9=
# 2684 "parse.y"
Cyc_Position_loc_to_seg(_TEE8);_TEEA=Cyc_Absyn_continue_stmt(_TEE9);yyval=Cyc_Stmt_tok(_TEEA);goto _LL0;case 382: _TEEB=yyyvsp;_TEEC=_TEEB[0];_TEED=_TEEC.l;_TEEE=_TEED.first_line;_TEEF=
# 2685 "parse.y"
Cyc_Position_loc_to_seg(_TEEE);_TEF0=Cyc_Absyn_break_stmt(_TEEF);yyval=Cyc_Stmt_tok(_TEF0);goto _LL0;case 383: _TEF1=yyyvsp;_TEF2=_TEF1[0];_TEF3=_TEF2.l;_TEF4=_TEF3.first_line;_TEF5=
# 2686 "parse.y"
Cyc_Position_loc_to_seg(_TEF4);_TEF6=Cyc_Absyn_return_stmt(0,_TEF5);yyval=Cyc_Stmt_tok(_TEF6);goto _LL0;case 384:
# 2687 "parse.y"
 _TEF7=yyyvsp + 1;_TEF8=&(*_TEF7).v;_TEF9=(union Cyc_YYSTYPE*)_TEF8;_TEFA=Cyc_yyget_Exp_tok(_TEF9);_TEFB=yyyvsp;_TEFC=_TEFB[0];_TEFD=_TEFC.l;_TEFE=_TEFD.first_line;_TEFF=Cyc_Position_loc_to_seg(_TEFE);_TF00=Cyc_Absyn_return_stmt(_TEFA,_TEFF);yyval=Cyc_Stmt_tok(_TF00);goto _LL0;case 385: _TF01=yyyvsp;_TF02=_TF01[0];_TF03=_TF02.l;_TF04=_TF03.first_line;_TF05=
# 2689 "parse.y"
Cyc_Position_loc_to_seg(_TF04);_TF06=Cyc_Absyn_fallthru_stmt(0,_TF05);yyval=Cyc_Stmt_tok(_TF06);goto _LL0;case 386: _TF07=yyyvsp;_TF08=_TF07[0];_TF09=_TF08.l;_TF0A=_TF09.first_line;_TF0B=
# 2690 "parse.y"
Cyc_Position_loc_to_seg(_TF0A);_TF0C=Cyc_Absyn_fallthru_stmt(0,_TF0B);yyval=Cyc_Stmt_tok(_TF0C);goto _LL0;case 387:
# 2692 "parse.y"
 _TF0D=yyyvsp + 2;_TF0E=&(*_TF0D).v;_TF0F=(union Cyc_YYSTYPE*)_TF0E;_TF10=Cyc_yyget_YY4(_TF0F);_TF11=yyyvsp;_TF12=_TF11[0];_TF13=_TF12.l;_TF14=_TF13.first_line;_TF15=Cyc_Position_loc_to_seg(_TF14);_TF16=Cyc_Absyn_fallthru_stmt(_TF10,_TF15);yyval=Cyc_Stmt_tok(_TF16);goto _LL0;case 388: _TF17=yyyvsp;_TF18=_TF17[0];
# 2701 "parse.y"
yyval=_TF18.v;goto _LL0;case 389: _TF19=yyyvsp;_TF1A=_TF19[0];
# 2704
yyval=_TF1A.v;goto _LL0;case 390:
# 2706 "parse.y"
 _TF1B=yyyvsp + 0;_TF1C=&(*_TF1B).v;_TF1D=(union Cyc_YYSTYPE*)_TF1C;_TF1E=Cyc_yyget_YY9(_TF1D);_TF1F=Cyc_Parse_pat2exp(_TF1E);_TF20=yyyvsp + 2;_TF21=&(*_TF20).v;_TF22=(union Cyc_YYSTYPE*)_TF21;_TF23=Cyc_yyget_Exp_tok(_TF22);_TF24=yyyvsp + 4;_TF25=&(*_TF24).v;_TF26=(union Cyc_YYSTYPE*)_TF25;_TF27=Cyc_yyget_Exp_tok(_TF26);_TF28=yyyvsp;_TF29=_TF28[0];_TF2A=_TF29.l;_TF2B=_TF2A.first_line;_TF2C=Cyc_Position_loc_to_seg(_TF2B);_TF2D=Cyc_Absyn_conditional_exp(_TF1F,_TF23,_TF27,_TF2C);_TF2E=Cyc_Absyn_exp_pat(_TF2D);yyval=Cyc_YY9(_TF2E);goto _LL0;case 391: _TF2F=yyyvsp;_TF30=_TF2F[0];
# 2709
yyval=_TF30.v;goto _LL0;case 392:
# 2711 "parse.y"
 _TF31=yyyvsp + 0;_TF32=&(*_TF31).v;_TF33=(union Cyc_YYSTYPE*)_TF32;_TF34=Cyc_yyget_YY9(_TF33);_TF35=Cyc_Parse_pat2exp(_TF34);_TF36=yyyvsp + 2;_TF37=&(*_TF36).v;_TF38=(union Cyc_YYSTYPE*)_TF37;_TF39=Cyc_yyget_Exp_tok(_TF38);_TF3A=yyyvsp;_TF3B=_TF3A[0];_TF3C=_TF3B.l;_TF3D=_TF3C.first_line;_TF3E=Cyc_Position_loc_to_seg(_TF3D);_TF3F=Cyc_Absyn_or_exp(_TF35,_TF39,_TF3E);_TF40=Cyc_Absyn_exp_pat(_TF3F);yyval=Cyc_YY9(_TF40);goto _LL0;case 393: _TF41=yyyvsp;_TF42=_TF41[0];
# 2714
yyval=_TF42.v;goto _LL0;case 394:
# 2716 "parse.y"
 _TF43=yyyvsp + 0;_TF44=&(*_TF43).v;_TF45=(union Cyc_YYSTYPE*)_TF44;_TF46=Cyc_yyget_YY9(_TF45);_TF47=Cyc_Parse_pat2exp(_TF46);_TF48=yyyvsp + 2;_TF49=&(*_TF48).v;_TF4A=(union Cyc_YYSTYPE*)_TF49;_TF4B=Cyc_yyget_Exp_tok(_TF4A);_TF4C=yyyvsp;_TF4D=_TF4C[0];_TF4E=_TF4D.l;_TF4F=_TF4E.first_line;_TF50=Cyc_Position_loc_to_seg(_TF4F);_TF51=Cyc_Absyn_and_exp(_TF47,_TF4B,_TF50);_TF52=Cyc_Absyn_exp_pat(_TF51);yyval=Cyc_YY9(_TF52);goto _LL0;case 395: _TF53=yyyvsp;_TF54=_TF53[0];
# 2719
yyval=_TF54.v;goto _LL0;case 396:
# 2721 "parse.y"
 _TF55=yyyvsp + 0;_TF56=&(*_TF55).v;_TF57=(union Cyc_YYSTYPE*)_TF56;_TF58=Cyc_yyget_YY9(_TF57);_TF59=Cyc_Parse_pat2exp(_TF58);_TF5A=yyyvsp + 2;_TF5B=&(*_TF5A).v;_TF5C=(union Cyc_YYSTYPE*)_TF5B;_TF5D=Cyc_yyget_Exp_tok(_TF5C);_TF5E=yyyvsp;_TF5F=_TF5E[0];_TF60=_TF5F.l;_TF61=_TF60.first_line;_TF62=Cyc_Position_loc_to_seg(_TF61);_TF63=Cyc_Absyn_prim2_exp(14U,_TF59,_TF5D,_TF62);_TF64=Cyc_Absyn_exp_pat(_TF63);yyval=Cyc_YY9(_TF64);goto _LL0;case 397: _TF65=yyyvsp;_TF66=_TF65[0];
# 2724
yyval=_TF66.v;goto _LL0;case 398:
# 2726 "parse.y"
 _TF67=yyyvsp + 0;_TF68=&(*_TF67).v;_TF69=(union Cyc_YYSTYPE*)_TF68;_TF6A=Cyc_yyget_YY9(_TF69);_TF6B=Cyc_Parse_pat2exp(_TF6A);_TF6C=yyyvsp + 2;_TF6D=&(*_TF6C).v;_TF6E=(union Cyc_YYSTYPE*)_TF6D;_TF6F=Cyc_yyget_Exp_tok(_TF6E);_TF70=yyyvsp;_TF71=_TF70[0];_TF72=_TF71.l;_TF73=_TF72.first_line;_TF74=Cyc_Position_loc_to_seg(_TF73);_TF75=Cyc_Absyn_prim2_exp(15U,_TF6B,_TF6F,_TF74);_TF76=Cyc_Absyn_exp_pat(_TF75);yyval=Cyc_YY9(_TF76);goto _LL0;case 399: _TF77=yyyvsp;_TF78=_TF77[0];
# 2729
yyval=_TF78.v;goto _LL0;case 400:
# 2731 "parse.y"
 _TF79=yyyvsp + 0;_TF7A=&(*_TF79).v;_TF7B=(union Cyc_YYSTYPE*)_TF7A;_TF7C=Cyc_yyget_YY9(_TF7B);_TF7D=Cyc_Parse_pat2exp(_TF7C);_TF7E=yyyvsp + 2;_TF7F=&(*_TF7E).v;_TF80=(union Cyc_YYSTYPE*)_TF7F;_TF81=Cyc_yyget_Exp_tok(_TF80);_TF82=yyyvsp;_TF83=_TF82[0];_TF84=_TF83.l;_TF85=_TF84.first_line;_TF86=Cyc_Position_loc_to_seg(_TF85);_TF87=Cyc_Absyn_prim2_exp(13U,_TF7D,_TF81,_TF86);_TF88=Cyc_Absyn_exp_pat(_TF87);yyval=Cyc_YY9(_TF88);goto _LL0;case 401: _TF89=yyyvsp;_TF8A=_TF89[0];
# 2734
yyval=_TF8A.v;goto _LL0;case 402:
# 2736 "parse.y"
 _TF8B=yyyvsp + 1;_TF8C=&(*_TF8B).v;_TF8D=(union Cyc_YYSTYPE*)_TF8C;_TF8E=Cyc_yyget_YY69(_TF8D);_TF8F=yyyvsp + 0;_TF90=&(*_TF8F).v;_TF91=(union Cyc_YYSTYPE*)_TF90;_TF92=Cyc_yyget_YY9(_TF91);_TF93=Cyc_Parse_pat2exp(_TF92);_TF94=yyyvsp + 2;_TF95=&(*_TF94).v;_TF96=(union Cyc_YYSTYPE*)_TF95;_TF97=Cyc_yyget_Exp_tok(_TF96);_TF98=yyyvsp;_TF99=_TF98[0];_TF9A=_TF99.l;_TF9B=_TF9A.first_line;_TF9C=Cyc_Position_loc_to_seg(_TF9B);_TF9D=_TF8E(_TF93,_TF97,_TF9C);_TF9E=Cyc_Absyn_exp_pat(_TF9D);yyval=Cyc_YY9(_TF9E);goto _LL0;case 403: _TF9F=yyyvsp;_TFA0=_TF9F[0];
# 2739
yyval=_TFA0.v;goto _LL0;case 404:
# 2741 "parse.y"
 _TFA1=yyyvsp + 1;_TFA2=&(*_TFA1).v;_TFA3=(union Cyc_YYSTYPE*)_TFA2;_TFA4=Cyc_yyget_YY69(_TFA3);_TFA5=yyyvsp + 0;_TFA6=&(*_TFA5).v;_TFA7=(union Cyc_YYSTYPE*)_TFA6;_TFA8=Cyc_yyget_YY9(_TFA7);_TFA9=Cyc_Parse_pat2exp(_TFA8);_TFAA=yyyvsp + 2;_TFAB=&(*_TFAA).v;_TFAC=(union Cyc_YYSTYPE*)_TFAB;_TFAD=Cyc_yyget_Exp_tok(_TFAC);_TFAE=yyyvsp;_TFAF=_TFAE[0];_TFB0=_TFAF.l;_TFB1=_TFB0.first_line;_TFB2=Cyc_Position_loc_to_seg(_TFB1);_TFB3=_TFA4(_TFA9,_TFAD,_TFB2);_TFB4=Cyc_Absyn_exp_pat(_TFB3);yyval=Cyc_YY9(_TFB4);goto _LL0;case 405: _TFB5=yyyvsp;_TFB6=_TFB5[0];
# 2744
yyval=_TFB6.v;goto _LL0;case 406:
# 2746 "parse.y"
 _TFB7=yyyvsp + 0;_TFB8=&(*_TFB7).v;_TFB9=(union Cyc_YYSTYPE*)_TFB8;_TFBA=Cyc_yyget_YY9(_TFB9);_TFBB=Cyc_Parse_pat2exp(_TFBA);_TFBC=yyyvsp + 2;_TFBD=&(*_TFBC).v;_TFBE=(union Cyc_YYSTYPE*)_TFBD;_TFBF=Cyc_yyget_Exp_tok(_TFBE);_TFC0=yyyvsp;_TFC1=_TFC0[0];_TFC2=_TFC1.l;_TFC3=_TFC2.first_line;_TFC4=Cyc_Position_loc_to_seg(_TFC3);_TFC5=Cyc_Absyn_prim2_exp(16U,_TFBB,_TFBF,_TFC4);_TFC6=Cyc_Absyn_exp_pat(_TFC5);yyval=Cyc_YY9(_TFC6);goto _LL0;case 407:
# 2748 "parse.y"
 _TFC7=yyyvsp + 0;_TFC8=&(*_TFC7).v;_TFC9=(union Cyc_YYSTYPE*)_TFC8;_TFCA=Cyc_yyget_YY9(_TFC9);_TFCB=Cyc_Parse_pat2exp(_TFCA);_TFCC=yyyvsp + 2;_TFCD=&(*_TFCC).v;_TFCE=(union Cyc_YYSTYPE*)_TFCD;_TFCF=Cyc_yyget_Exp_tok(_TFCE);_TFD0=yyyvsp;_TFD1=_TFD0[0];_TFD2=_TFD1.l;_TFD3=_TFD2.first_line;_TFD4=Cyc_Position_loc_to_seg(_TFD3);_TFD5=Cyc_Absyn_prim2_exp(17U,_TFCB,_TFCF,_TFD4);_TFD6=Cyc_Absyn_exp_pat(_TFD5);yyval=Cyc_YY9(_TFD6);goto _LL0;case 408: _TFD7=yyyvsp;_TFD8=_TFD7[0];
# 2751
yyval=_TFD8.v;goto _LL0;case 409:
# 2753 "parse.y"
 _TFD9=yyyvsp + 1;_TFDA=&(*_TFD9).v;_TFDB=(union Cyc_YYSTYPE*)_TFDA;_TFDC=Cyc_yyget_YY6(_TFDB);_TFDD=yyyvsp + 0;_TFDE=&(*_TFDD).v;_TFDF=(union Cyc_YYSTYPE*)_TFDE;_TFE0=Cyc_yyget_YY9(_TFDF);_TFE1=Cyc_Parse_pat2exp(_TFE0);_TFE2=yyyvsp + 2;_TFE3=&(*_TFE2).v;_TFE4=(union Cyc_YYSTYPE*)_TFE3;_TFE5=Cyc_yyget_Exp_tok(_TFE4);_TFE6=yyyvsp;_TFE7=_TFE6[0];_TFE8=_TFE7.l;_TFE9=_TFE8.first_line;_TFEA=Cyc_Position_loc_to_seg(_TFE9);_TFEB=Cyc_Absyn_prim2_exp(_TFDC,_TFE1,_TFE5,_TFEA);_TFEC=Cyc_Absyn_exp_pat(_TFEB);yyval=Cyc_YY9(_TFEC);goto _LL0;case 410: _TFED=yyyvsp;_TFEE=_TFED[0];
# 2756
yyval=_TFEE.v;goto _LL0;case 411:
# 2758 "parse.y"
 _TFEF=yyyvsp + 1;_TFF0=&(*_TFEF).v;_TFF1=(union Cyc_YYSTYPE*)_TFF0;_TFF2=Cyc_yyget_YY6(_TFF1);_TFF3=yyyvsp + 0;_TFF4=&(*_TFF3).v;_TFF5=(union Cyc_YYSTYPE*)_TFF4;_TFF6=Cyc_yyget_YY9(_TFF5);_TFF7=Cyc_Parse_pat2exp(_TFF6);_TFF8=yyyvsp + 2;_TFF9=&(*_TFF8).v;_TFFA=(union Cyc_YYSTYPE*)_TFF9;_TFFB=Cyc_yyget_Exp_tok(_TFFA);_TFFC=yyyvsp;_TFFD=_TFFC[0];_TFFE=_TFFD.l;_TFFF=_TFFE.first_line;_T1000=Cyc_Position_loc_to_seg(_TFFF);_T1001=Cyc_Absyn_prim2_exp(_TFF2,_TFF7,_TFFB,_T1000);_T1002=Cyc_Absyn_exp_pat(_T1001);yyval=Cyc_YY9(_T1002);goto _LL0;case 412: _T1003=yyyvsp;_T1004=_T1003[0];
# 2761
yyval=_T1004.v;goto _LL0;case 413:
# 2763 "parse.y"
 _T1005=yyyvsp + 1;_T1006=&(*_T1005).v;_T1007=(union Cyc_YYSTYPE*)_T1006;_T1008=Cyc_yyget_YY38(_T1007);_T1009=yyyvsp;_T100A=_T1009[1];_T100B=_T100A.l;_T100C=_T100B.first_line;_T100D=Cyc_Position_loc_to_seg(_T100C);{void*t=Cyc_Parse_type_name_to_type(_T1008,_T100D);_T100E=t;
_T100F=yyyvsp + 3;_T1010=&(*_T100F).v;_T1011=(union Cyc_YYSTYPE*)_T1010;_T1012=Cyc_yyget_Exp_tok(_T1011);_T1013=yyyvsp;_T1014=_T1013[0];_T1015=_T1014.l;_T1016=_T1015.first_line;_T1017=Cyc_Position_loc_to_seg(_T1016);_T1018=Cyc_Absyn_cast_exp(_T100E,_T1012,1,0U,_T1017);_T1019=Cyc_Absyn_exp_pat(_T1018);yyval=Cyc_YY9(_T1019);goto _LL0;}case 414: _T101A=yyyvsp;_T101B=_T101A[0];
# 2769 "parse.y"
yyval=_T101B.v;goto _LL0;case 415:
# 2773 "parse.y"
 _T101C=yyyvsp + 0;_T101D=&(*_T101C).v;_T101E=(union Cyc_YYSTYPE*)_T101D;_T101F=Cyc_yyget_YY6(_T101E);_T1020=yyyvsp + 1;_T1021=&(*_T1020).v;_T1022=(union Cyc_YYSTYPE*)_T1021;_T1023=Cyc_yyget_Exp_tok(_T1022);_T1024=yyyvsp;_T1025=_T1024[0];_T1026=_T1025.l;_T1027=_T1026.first_line;_T1028=Cyc_Position_loc_to_seg(_T1027);_T1029=Cyc_Absyn_prim1_exp(_T101F,_T1023,_T1028);_T102A=Cyc_Absyn_exp_pat(_T1029);yyval=Cyc_YY9(_T102A);goto _LL0;case 416:
# 2775 "parse.y"
 _T102B=yyyvsp + 2;_T102C=&(*_T102B).v;_T102D=(union Cyc_YYSTYPE*)_T102C;_T102E=Cyc_yyget_YY38(_T102D);_T102F=yyyvsp;_T1030=_T102F[2];_T1031=_T1030.l;_T1032=_T1031.first_line;_T1033=Cyc_Position_loc_to_seg(_T1032);{void*t=Cyc_Parse_type_name_to_type(_T102E,_T1033);_T1034=t;_T1035=yyyvsp;_T1036=_T1035[0];_T1037=_T1036.l;_T1038=_T1037.first_line;_T1039=
Cyc_Position_loc_to_seg(_T1038);_T103A=Cyc_Absyn_sizeoftype_exp(_T1034,_T1039);_T103B=Cyc_Absyn_exp_pat(_T103A);yyval=Cyc_YY9(_T103B);goto _LL0;}case 417:
# 2778 "parse.y"
 _T103C=yyyvsp + 1;_T103D=&(*_T103C).v;_T103E=(union Cyc_YYSTYPE*)_T103D;_T103F=Cyc_yyget_Exp_tok(_T103E);_T1040=yyyvsp;_T1041=_T1040[0];_T1042=_T1041.l;_T1043=_T1042.first_line;_T1044=Cyc_Position_loc_to_seg(_T1043);_T1045=Cyc_Absyn_sizeofexp_exp(_T103F,_T1044);_T1046=Cyc_Absyn_exp_pat(_T1045);yyval=Cyc_YY9(_T1046);goto _LL0;case 418:
# 2780 "parse.y"
 _T1047=yyyvsp + 2;_T1048=&(*_T1047).v;_T1049=(union Cyc_YYSTYPE*)_T1048;_T104A=Cyc_yyget_YY38(_T1049);_T104B=*_T104A;_T104C=_T104B.f2;_T104D=yyyvsp + 4;_T104E=&(*_T104D).v;_T104F=(union Cyc_YYSTYPE*)_T104E;_T1050=Cyc_yyget_YY3(_T104F);_T1051=Cyc_List_imp_rev(_T1050);_T1052=yyyvsp;_T1053=_T1052[0];_T1054=_T1053.l;_T1055=_T1054.first_line;_T1056=Cyc_Position_loc_to_seg(_T1055);_T1057=Cyc_Absyn_offsetof_exp(_T104C,_T1051,_T1056);_T1058=Cyc_Absyn_exp_pat(_T1057);yyval=Cyc_YY9(_T1058);goto _LL0;case 419: _T1059=yyyvsp;_T105A=_T1059[0];
# 2785 "parse.y"
yyval=_T105A.v;goto _LL0;case 420: _T105B=yyyvsp;_T105C=_T105B[0];
# 2793 "parse.y"
yyval=_T105C.v;goto _LL0;case 421: _T105D=& Cyc_Absyn_Wild_p_val;_T105E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T105D;_T105F=(void*)_T105E;_T1060=yyyvsp;_T1061=_T1060[0];_T1062=_T1061.l;_T1063=_T1062.first_line;_T1064=
# 2797 "parse.y"
Cyc_Position_loc_to_seg(_T1063);_T1065=Cyc_Absyn_new_pat(_T105F,_T1064);yyval=Cyc_YY9(_T1065);goto _LL0;case 422:
# 2798 "parse.y"
 _T1066=yyyvsp + 1;_T1067=&(*_T1066).v;_T1068=(union Cyc_YYSTYPE*)_T1067;_T1069=Cyc_yyget_Exp_tok(_T1068);_T106A=Cyc_Absyn_exp_pat(_T1069);yyval=Cyc_YY9(_T106A);goto _LL0;case 423:{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_T1883->tag=14;
# 2799 "parse.y"
_T106C=yyyvsp + 0;_T106D=&(*_T106C).v;_T106E=(union Cyc_YYSTYPE*)_T106D;_T1883->f1=Cyc_yyget_QualId_tok(_T106E);_T106B=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T1883;}_T106F=(void*)_T106B;_T1070=yyyvsp;_T1071=_T1070[0];_T1072=_T1071.l;_T1073=_T1072.first_line;_T1074=Cyc_Position_loc_to_seg(_T1073);_T1075=Cyc_Absyn_new_pat(_T106F,_T1074);yyval=Cyc_YY9(_T1075);goto _LL0;case 424:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1883->tag=5;
# 2800 "parse.y"
_T1077=yyyvsp + 1;_T1078=&(*_T1077).v;_T1079=(union Cyc_YYSTYPE*)_T1078;_T1883->f1=Cyc_yyget_YY9(_T1079);_T1076=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1883;}_T107A=(void*)_T1076;_T107B=yyyvsp;_T107C=_T107B[0];_T107D=_T107C.l;_T107E=_T107D.first_line;_T107F=Cyc_Position_loc_to_seg(_T107E);_T1080=Cyc_Absyn_new_pat(_T107A,_T107F);yyval=Cyc_YY9(_T1080);goto _LL0;case 425:
# 2802 "parse.y"
 _T1081=yyyvsp + 0;_T1082=&(*_T1081).v;_T1083=(union Cyc_YYSTYPE*)_T1082;{struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(_T1083);_T1084=e;{
void*_T1883=_T1084->r;struct _fat_ptr _T1884;int _T1885;short _T1886;char _T1887;enum Cyc_Absyn_Sign _T1888;_T1085=(int*)_T1883;_T1086=*_T1085;if(_T1086!=0)goto _TL2E0;_T1087=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1883;_T1088=_T1087->f1;_T1089=_T1088.LongLong_c;_T108A=_T1089.tag;switch(_T108A){case 2:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1889=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1883;_T108B=_T1889->f1;_T108C=_T108B.Char_c;_T108D=_T108C.val;_T1888=_T108D.f0;_T108E=_T1889->f1;_T108F=_T108E.Char_c;_T1090=_T108F.val;_T1887=_T1090.f1;}{enum Cyc_Absyn_Sign s=_T1888;char i=_T1887;{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T1889=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_T1889->tag=10;
# 2805
_T1889->f1=i;_T1091=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T1889;}_T1092=(void*)_T1091;_T1093=e;_T1094=_T1093->loc;_T1095=Cyc_Absyn_new_pat(_T1092,_T1094);yyval=Cyc_YY9(_T1095);goto _LL537;}case 4:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1889=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1883;_T1096=_T1889->f1;_T1097=_T1096.Short_c;_T1098=_T1097.val;_T1888=_T1098.f0;_T1099=_T1889->f1;_T109A=_T1099.Short_c;_T109B=_T109A.val;_T1886=_T109B.f1;}{enum Cyc_Absyn_Sign s=_T1888;short i=_T1886;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1889=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T1889->tag=9;
# 2807
_T1889->f1=s;_T109D=i;_T1889->f2=(int)_T109D;_T109C=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T1889;}_T109E=(void*)_T109C;_T109F=e;_T10A0=_T109F->loc;_T10A1=Cyc_Absyn_new_pat(_T109E,_T10A0);yyval=Cyc_YY9(_T10A1);goto _LL537;}case 5:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1889=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1883;_T10A2=_T1889->f1;_T10A3=_T10A2.Int_c;_T10A4=_T10A3.val;_T1888=_T10A4.f0;_T10A5=_T1889->f1;_T10A6=_T10A5.Int_c;_T10A7=_T10A6.val;_T1885=_T10A7.f1;}{enum Cyc_Absyn_Sign s=_T1888;int i=_T1885;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1889=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T1889->tag=9;
# 2809
_T1889->f1=s;_T1889->f2=i;_T10A8=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T1889;}_T10A9=(void*)_T10A8;_T10AA=e;_T10AB=_T10AA->loc;_T10AC=Cyc_Absyn_new_pat(_T10A9,_T10AB);yyval=Cyc_YY9(_T10AC);goto _LL537;}case 7:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1889=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1883;_T10AD=_T1889->f1;_T10AE=_T10AD.Float_c;_T10AF=_T10AE.val;_T1884=_T10AF.f0;_T10B0=_T1889->f1;_T10B1=_T10B0.Float_c;_T10B2=_T10B1.val;_T1885=_T10B2.f1;}{struct _fat_ptr s=_T1884;int i=_T1885;{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T1889=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_T1889->tag=11;
# 2811
_T1889->f1=s;_T1889->f2=i;_T10B3=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T1889;}_T10B4=(void*)_T10B3;_T10B5=e;_T10B6=_T10B5->loc;_T10B7=Cyc_Absyn_new_pat(_T10B4,_T10B6);yyval=Cyc_YY9(_T10B7);goto _LL537;}case 1: _T10B8=& Cyc_Absyn_Null_p_val;_T10B9=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_T10B8;_T10BA=(void*)_T10B9;_T10BB=e;_T10BC=_T10BB->loc;_T10BD=
# 2813
Cyc_Absyn_new_pat(_T10BA,_T10BC);yyval=Cyc_YY9(_T10BD);goto _LL537;case 8: goto _LL545;case 9: _LL545: _T10BE=yyyvsp;_T10BF=_T10BE[0];_T10C0=_T10BF.l;_T10C1=_T10C0.first_line;_T10C2=
# 2816
Cyc_Position_loc_to_seg(_T10C1);_T10C3=_tag_fat("strings cannot occur within patterns",sizeof(char),37U);_T10C4=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10C2,_T10C3,_T10C4);goto _LL537;case 6: _T10C5=yyyvsp;_T10C6=_T10C5[0];_T10C7=_T10C6.l;_T10C8=_T10C7.first_line;_T10C9=
# 2818
Cyc_Position_loc_to_seg(_T10C8);_T10CA=_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U);_T10CB=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10C9,_T10CA,_T10CB);goto _LL537;default: goto _LL548;}goto _TL2E1;_TL2E0: _LL548: _T10CC=yyyvsp;_T10CD=_T10CC[0];_T10CE=_T10CD.l;_T10CF=_T10CE.first_line;_T10D0=
# 2820
Cyc_Position_loc_to_seg(_T10CF);_T10D1=_tag_fat("bad constant in case",sizeof(char),21U);_T10D2=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10D0,_T10D1,_T10D2);_TL2E1: _LL537:;}goto _LL0;}case 426:
# 2824 "parse.y"
 _T10D3=yyyvsp + 1;_T10D4=&(*_T10D3).v;_T10D5=(union Cyc_YYSTYPE*)_T10D4;_T10D6=Cyc_yyget_String_tok(_T10D5);_T10D7=_tag_fat("as",sizeof(char),3U);_T10D8=Cyc_strcmp(_T10D6,_T10D7);if(_T10D8==0)goto _TL2E3;_T10D9=yyyvsp;_T10DA=_T10D9[1];_T10DB=_T10DA.l;_T10DC=_T10DB.first_line;_T10DD=
Cyc_Position_loc_to_seg(_T10DC);_T10DE=_tag_fat("expecting `as'",sizeof(char),15U);_T10DF=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10DD,_T10DE,_T10DF);goto _TL2E4;_TL2E3: _TL2E4:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T1883->tag=1;_T10E1=yyyvsp;_T10E2=_T10E1[0];_T10E3=_T10E2.l;_T10E4=_T10E3.first_line;_T10E5=
Cyc_Position_loc_to_seg(_T10E4);{struct _tuple0*_T1884=_cycalloc(sizeof(struct _tuple0));_T1884->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));_T10E8=yyyvsp + 0;_T10E9=&(*_T10E8).v;_T10EA=(union Cyc_YYSTYPE*)_T10E9;*_T1885=Cyc_yyget_String_tok(_T10EA);_T10E7=(struct _fat_ptr*)_T1885;}_T1884->f1=_T10E7;_T10E6=(struct _tuple0*)_T1884;}_T10EB=Cyc_Absyn_void_type;_T1883->f1=Cyc_Absyn_new_vardecl(_T10E5,_T10E6,_T10EB,0,0);
_T10EC=yyyvsp + 2;_T10ED=&(*_T10EC).v;_T10EE=(union Cyc_YYSTYPE*)_T10ED;_T1883->f2=Cyc_yyget_YY9(_T10EE);_T10E0=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T1883;}_T10EF=(void*)_T10E0;_T10F0=yyyvsp;_T10F1=_T10F0[0];_T10F2=_T10F1.l;_T10F3=_T10F2.first_line;_T10F4=Cyc_Position_loc_to_seg(_T10F3);_T10F5=
# 2826
Cyc_Absyn_new_pat(_T10EF,_T10F4);yyval=Cyc_YY9(_T10F5);goto _LL0;case 427:
# 2830 "parse.y"
 _T10F6=yyyvsp + 0;_T10F7=&(*_T10F6).v;_T10F8=(union Cyc_YYSTYPE*)_T10F7;_T10F9=Cyc_yyget_String_tok(_T10F8);_T10FA=_tag_fat("alias",sizeof(char),6U);_T10FB=Cyc_strcmp(_T10F9,_T10FA);if(_T10FB==0)goto _TL2E5;_T10FC=yyyvsp;_T10FD=_T10FC[1];_T10FE=_T10FD.l;_T10FF=_T10FE.first_line;_T1100=
Cyc_Position_loc_to_seg(_T10FF);_T1101=_tag_fat("expecting `alias'",sizeof(char),18U);_T1102=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1100,_T1101,_T1102);goto _TL2E6;_TL2E5: _TL2E6: _T1103=yyyvsp;_T1104=_T1103[0];_T1105=_T1104.l;_T1106=_T1105.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T1106);
_T1107=yyyvsp + 2;_T1108=&(*_T1107).v;_T1109=(union Cyc_YYSTYPE*)_T1108;_T110A=Cyc_yyget_String_tok(_T1109);_T110B=location;Cyc_Parse_tvar_ok(_T110A,_T110B);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T110C=tv;{struct _fat_ptr*_T1883=_cycalloc(sizeof(struct _fat_ptr));_T110E=yyyvsp + 2;_T110F=&(*_T110E).v;_T1110=(union Cyc_YYSTYPE*)_T110F;*_T1883=Cyc_yyget_String_tok(_T1110);_T110D=(struct _fat_ptr*)_T1883;}_T110C->name=_T110D;_T1111=tv;_T1111->identity=-1;_T1112=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T1883->tag=0;_T1114=& Cyc_Kinds_ek;_T1883->f1=(struct Cyc_Absyn_Kind*)_T1114;_T1113=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1883;}_T1112->kind=(void*)_T1113;_T1115=tv;_T1115->aquals_bound=0;_T1116=yyyvsp;_T1117=_T1116[0];_T1118=_T1117.l;_T1119=_T1118.first_line;_T111A=
Cyc_Position_loc_to_seg(_T1119);{struct _tuple0*_T1883=_cycalloc(sizeof(struct _tuple0));_T1883->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_T111D=yyyvsp + 5;_T111E=&(*_T111D).v;_T111F=(union Cyc_YYSTYPE*)_T111E;*_T1884=Cyc_yyget_String_tok(_T111F);_T111C=(struct _fat_ptr*)_T1884;}_T1883->f1=_T111C;_T111B=(struct _tuple0*)_T1883;}
_T1120=yyyvsp + 4;_T1121=&(*_T1120).v;_T1122=(union Cyc_YYSTYPE*)_T1121;_T1123=Cyc_yyget_YY38(_T1122);_T1124=yyyvsp;_T1125=_T1124[4];_T1126=_T1125.l;_T1127=_T1126.first_line;_T1128=Cyc_Position_loc_to_seg(_T1127);_T1129=Cyc_Parse_type_name_to_type(_T1123,_T1128);{
# 2835
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T111A,_T111B,_T1129,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T1883->tag=2;
# 2837
_T1883->f1=tv;_T1883->f2=vd;_T112A=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T1883;}_T112B=(void*)_T112A;_T112C=location;_T112D=Cyc_Absyn_new_pat(_T112B,_T112C);yyval=Cyc_YY9(_T112D);goto _LL0;}}}case 428:
# 2840 "parse.y"
 _T112E=yyyvsp + 0;_T112F=&(*_T112E).v;_T1130=(union Cyc_YYSTYPE*)_T112F;_T1131=Cyc_yyget_String_tok(_T1130);_T1132=_tag_fat("alias",sizeof(char),6U);_T1133=Cyc_strcmp(_T1131,_T1132);if(_T1133==0)goto _TL2E7;_T1134=yyyvsp;_T1135=_T1134[1];_T1136=_T1135.l;_T1137=_T1136.first_line;_T1138=
Cyc_Position_loc_to_seg(_T1137);_T1139=_tag_fat("expecting `alias'",sizeof(char),18U);_T113A=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T1138,_T1139,_T113A);goto _TL2E8;_TL2E7: _TL2E8: _T113B=yyyvsp;_T113C=_T113B[0];_T113D=_T113C.l;_T113E=_T113D.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T113E);
_T113F=yyyvsp + 2;_T1140=&(*_T113F).v;_T1141=(union Cyc_YYSTYPE*)_T1140;_T1142=Cyc_yyget_String_tok(_T1141);_T1143=location;Cyc_Parse_tvar_ok(_T1142,_T1143);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T1144=tv;{struct _fat_ptr*_T1883=_cycalloc(sizeof(struct _fat_ptr));_T1146=yyyvsp + 2;_T1147=&(*_T1146).v;_T1148=(union Cyc_YYSTYPE*)_T1147;*_T1883=Cyc_yyget_String_tok(_T1148);_T1145=(struct _fat_ptr*)_T1883;}_T1144->name=_T1145;_T1149=tv;_T1149->identity=-1;_T114A=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T1883->tag=0;_T114C=& Cyc_Kinds_ek;_T1883->f1=(struct Cyc_Absyn_Kind*)_T114C;_T114B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1883;}_T114A->kind=(void*)_T114B;_T114D=tv;_T114D->aquals_bound=0;_T114E=yyyvsp;_T114F=_T114E[0];_T1150=_T114F.l;_T1151=_T1150.first_line;_T1152=
Cyc_Position_loc_to_seg(_T1151);{struct _tuple0*_T1883=_cycalloc(sizeof(struct _tuple0));_T1883->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_T1155=yyyvsp + 5;_T1156=&(*_T1155).v;_T1157=(union Cyc_YYSTYPE*)_T1156;*_T1884=Cyc_yyget_String_tok(_T1157);_T1154=(struct _fat_ptr*)_T1884;}_T1883->f1=_T1154;_T1153=(struct _tuple0*)_T1883;}
_T1158=yyyvsp + 4;_T1159=&(*_T1158).v;_T115A=(union Cyc_YYSTYPE*)_T1159;_T115B=Cyc_yyget_YY38(_T115A);_T115C=yyyvsp;_T115D=_T115C[4];_T115E=_T115D.l;_T115F=_T115E.first_line;_T1160=Cyc_Position_loc_to_seg(_T115F);_T1161=Cyc_Parse_type_name_to_type(_T115B,_T1160);{
# 2845
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T1152,_T1153,_T1161,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T1883->tag=2;
# 2847
_T1883->f1=tv;_T1883->f2=vd;_T1162=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T1883;}_T1163=(void*)_T1162;_T1164=location;_T1165=Cyc_Absyn_new_pat(_T1163,_T1164);yyval=Cyc_YY9(_T1165);goto _LL0;}}}case 429:
# 2850 "parse.y"
 _T1166=yyyvsp + 2;_T1167=&(*_T1166).v;_T1168=(union Cyc_YYSTYPE*)_T1167;_T1169=Cyc_yyget_YY14(_T1168);{struct _tuple23 _T1883=*_T1169;int _T1884;struct Cyc_List_List*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_List_List*fps=_T1885;int dots=_T1884;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T1886->tag=6;
_T1886->f1=0;_T1886->f2=1;_T1886->f3=0;_T1886->f4=fps;_T1886->f5=dots;_T116A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T1886;}_T116B=(void*)_T116A;_T116C=yyyvsp;_T116D=_T116C[0];_T116E=_T116D.l;_T116F=_T116E.first_line;_T1170=Cyc_Position_loc_to_seg(_T116F);_T1171=Cyc_Absyn_new_pat(_T116B,_T1170);yyval=Cyc_YY9(_T1171);goto _LL0;}}case 430:
# 2854 "parse.y"
 _T1172=yyyvsp + 2;_T1173=&(*_T1172).v;_T1174=(union Cyc_YYSTYPE*)_T1173;_T1175=Cyc_yyget_YY10(_T1174);{struct _tuple23 _T1883=*_T1175;int _T1884;struct Cyc_List_List*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_List_List*ps=_T1885;int dots=_T1884;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_T1886->tag=15;
_T1177=yyyvsp + 0;_T1178=&(*_T1177).v;_T1179=(union Cyc_YYSTYPE*)_T1178;_T1886->f1=Cyc_yyget_QualId_tok(_T1179);_T1886->f2=ps;_T1886->f3=dots;_T1176=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T1886;}_T117A=(void*)_T1176;_T117B=yyyvsp;_T117C=_T117B[0];_T117D=_T117C.l;_T117E=_T117D.first_line;_T117F=Cyc_Position_loc_to_seg(_T117E);_T1180=Cyc_Absyn_new_pat(_T117A,_T117F);yyval=Cyc_YY9(_T1180);goto _LL0;}}case 431:
# 2858 "parse.y"
 _T1181=yyyvsp + 3;_T1182=&(*_T1181).v;_T1183=(union Cyc_YYSTYPE*)_T1182;_T1184=Cyc_yyget_YY14(_T1183);{struct _tuple23 _T1883=*_T1184;int _T1884;struct Cyc_List_List*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_List_List*fps=_T1885;int dots=_T1884;_T1186=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1886)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T1186;_T1185=_T1886;}_T1187=yyyvsp;_T1188=_T1187[2];_T1189=_T1188.l;_T118A=_T1189.first_line;_T118B=Cyc_Position_loc_to_seg(_T118A);_T118C=yyyvsp + 2;_T118D=&(*_T118C).v;_T118E=(union Cyc_YYSTYPE*)_T118D;_T118F=Cyc_yyget_YY41(_T118E);{struct Cyc_List_List*exist_ts=_T1185(Cyc_Parse_typ2tvar,_T118B,_T118F);
_T1190=yyyvsp + 0;_T1191=&(*_T1190).v;_T1192=(union Cyc_YYSTYPE*)_T1191;_T1193=Cyc_yyget_QualId_tok(_T1192);{union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,_T1193,0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_T1194=typ;_T1194->tag=0;_T1195=typ;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T1886->tag=24;_T1886->f1=ai;_T1196=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T1886;}_T1195->f1=(void*)_T1196;_T1197=typ;_T1197->f2=0;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T1886->tag=6;_T1199=typ;
_T1886->f1=(void*)_T1199;_T1886->f2=0;_T1886->f3=exist_ts;_T1886->f4=fps;_T1886->f5=dots;_T1198=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T1886;}_T119A=(void*)_T1198;_T119B=yyyvsp;_T119C=_T119B[0];_T119D=_T119C.l;_T119E=_T119D.first_line;_T119F=Cyc_Position_loc_to_seg(_T119E);_T11A0=Cyc_Absyn_new_pat(_T119A,_T119F);yyval=Cyc_YY9(_T11A0);goto _LL0;}}}}case 432:
# 2865 "parse.y"
 _T11A1=yyyvsp + 2;_T11A2=&(*_T11A1).v;_T11A3=(union Cyc_YYSTYPE*)_T11A2;_T11A4=Cyc_yyget_YY14(_T11A3);{struct _tuple23 _T1883=*_T11A4;int _T1884;struct Cyc_List_List*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_List_List*fps=_T1885;int dots=_T1884;_T11A6=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1886)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T11A6;_T11A5=_T1886;}_T11A7=yyyvsp;_T11A8=_T11A7[1];_T11A9=_T11A8.l;_T11AA=_T11A9.first_line;_T11AB=Cyc_Position_loc_to_seg(_T11AA);_T11AC=yyyvsp + 1;_T11AD=&(*_T11AC).v;_T11AE=(union Cyc_YYSTYPE*)_T11AD;_T11AF=Cyc_yyget_YY41(_T11AE);{struct Cyc_List_List*exist_ts=_T11A5(Cyc_Parse_typ2tvar,_T11AB,_T11AF);{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T1886->tag=6;
_T1886->f1=0;_T1886->f2=0;_T1886->f3=exist_ts;_T1886->f4=fps;_T1886->f5=dots;_T11B0=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T1886;}_T11B1=(void*)_T11B0;_T11B2=yyyvsp;_T11B3=_T11B2[0];_T11B4=_T11B3.l;_T11B5=_T11B4.first_line;_T11B6=Cyc_Position_loc_to_seg(_T11B5);_T11B7=Cyc_Absyn_new_pat(_T11B1,_T11B6);yyval=Cyc_YY9(_T11B7);goto _LL0;}}}case 433:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1883->tag=5;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1884->tag=5;
# 2870 "parse.y"
_T11BA=yyyvsp + 1;_T11BB=&(*_T11BA).v;_T11BC=(union Cyc_YYSTYPE*)_T11BB;_T1884->f1=Cyc_yyget_YY9(_T11BC);_T11B9=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1884;}_T11BD=(void*)_T11B9;_T11BE=yyyvsp;_T11BF=_T11BE[0];_T11C0=_T11BF.l;_T11C1=_T11C0.first_line;_T11C2=Cyc_Position_loc_to_seg(_T11C1);_T1883->f1=Cyc_Absyn_new_pat(_T11BD,_T11C2);_T11B8=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1883;}_T11C3=(void*)_T11B8;_T11C4=yyyvsp;_T11C5=_T11C4[0];_T11C6=_T11C5.l;_T11C7=_T11C6.first_line;_T11C8=Cyc_Position_loc_to_seg(_T11C7);_T11C9=Cyc_Absyn_new_pat(_T11C3,_T11C8);yyval=Cyc_YY9(_T11C9);goto _LL0;case 434:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T1883->tag=3;_T11CB=yyyvsp;_T11CC=_T11CB[0];_T11CD=_T11CC.l;_T11CE=_T11CD.first_line;_T11CF=
# 2872 "parse.y"
Cyc_Position_loc_to_seg(_T11CE);{struct _tuple0*_T1884=_cycalloc(sizeof(struct _tuple0));_T1884->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));_T11D2=yyyvsp + 1;_T11D3=&(*_T11D2).v;_T11D4=(union Cyc_YYSTYPE*)_T11D3;*_T1885=Cyc_yyget_String_tok(_T11D4);_T11D1=(struct _fat_ptr*)_T1885;}_T1884->f1=_T11D1;_T11D0=(struct _tuple0*)_T1884;}_T11D5=Cyc_Absyn_void_type;_T1883->f1=Cyc_Absyn_new_vardecl(_T11CF,_T11D0,_T11D5,0,0);_T11D6=& Cyc_Absyn_Wild_p_val;_T11D7=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T11D6;_T11D8=(void*)_T11D7;_T11D9=yyyvsp;_T11DA=_T11D9[1];_T11DB=_T11DA.l;_T11DC=_T11DB.first_line;_T11DD=
# 2874
Cyc_Position_loc_to_seg(_T11DC);_T1883->f2=Cyc_Absyn_new_pat(_T11D8,_T11DD);_T11CA=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T1883;}_T11DE=(void*)_T11CA;_T11DF=yyyvsp;_T11E0=_T11DF[0];_T11E1=_T11E0.l;_T11E2=_T11E1.first_line;_T11E3=
Cyc_Position_loc_to_seg(_T11E2);_T11E4=
# 2872
Cyc_Absyn_new_pat(_T11DE,_T11E3);yyval=Cyc_YY9(_T11E4);goto _LL0;case 435:
# 2877 "parse.y"
 _T11E5=yyyvsp + 2;_T11E6=&(*_T11E5).v;_T11E7=(union Cyc_YYSTYPE*)_T11E6;_T11E8=Cyc_yyget_String_tok(_T11E7);_T11E9=_tag_fat("as",sizeof(char),3U);_T11EA=Cyc_strcmp(_T11E8,_T11E9);if(_T11EA==0)goto _TL2E9;_T11EB=yyyvsp;_T11EC=_T11EB[2];_T11ED=_T11EC.l;_T11EE=_T11ED.first_line;_T11EF=
Cyc_Position_loc_to_seg(_T11EE);_T11F0=_tag_fat("expecting `as'",sizeof(char),15U);_T11F1=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T11EF,_T11F0,_T11F1);goto _TL2EA;_TL2E9: _TL2EA:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T1883->tag=3;_T11F3=yyyvsp;_T11F4=_T11F3[0];_T11F5=_T11F4.l;_T11F6=_T11F5.first_line;_T11F7=
Cyc_Position_loc_to_seg(_T11F6);{struct _tuple0*_T1884=_cycalloc(sizeof(struct _tuple0));_T1884->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));_T11FA=yyyvsp + 1;_T11FB=&(*_T11FA).v;_T11FC=(union Cyc_YYSTYPE*)_T11FB;*_T1885=Cyc_yyget_String_tok(_T11FC);_T11F9=(struct _fat_ptr*)_T1885;}_T1884->f1=_T11F9;_T11F8=(struct _tuple0*)_T1884;}_T11FD=Cyc_Absyn_void_type;_T1883->f1=Cyc_Absyn_new_vardecl(_T11F7,_T11F8,_T11FD,0,0);
# 2881
_T11FE=yyyvsp + 3;_T11FF=&(*_T11FE).v;_T1200=(union Cyc_YYSTYPE*)_T11FF;_T1883->f2=Cyc_yyget_YY9(_T1200);_T11F2=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T1883;}_T1201=(void*)_T11F2;_T1202=yyyvsp;_T1203=_T1202[0];_T1204=_T1203.l;_T1205=_T1204.first_line;_T1206=Cyc_Position_loc_to_seg(_T1205);_T1207=
# 2879
Cyc_Absyn_new_pat(_T1201,_T1206);yyval=Cyc_YY9(_T1207);goto _LL0;case 436:
# 2884 "parse.y"
 _T1208=yyyvsp + 2;_T1209=&(*_T1208).v;_T120A=(union Cyc_YYSTYPE*)_T1209;_T120B=Cyc_yyget_String_tok(_T120A);_T120C=& Cyc_Kinds_ik;_T120D=(struct Cyc_Absyn_Kind*)_T120C;_T120E=Cyc_Kinds_kind_to_bound(_T120D);_T120F=yyyvsp;_T1210=_T120F[2];_T1211=_T1210.l;_T1212=_T1211.first_line;_T1213=Cyc_Position_loc_to_seg(_T1212);{void*tag=Cyc_Parse_id2type(_T120B,_T120E,0,_T1213);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T1883->tag=4;_T1215=yyyvsp;_T1216=_T1215[2];_T1217=_T1216.l;_T1218=_T1217.first_line;_T1219=
Cyc_Position_loc_to_seg(_T1218);_T121A=tag;_T1883->f1=Cyc_Parse_typ2tvar(_T1219,_T121A);_T121B=yyyvsp;_T121C=_T121B[0];_T121D=_T121C.l;_T121E=_T121D.first_line;_T121F=
Cyc_Position_loc_to_seg(_T121E);{struct _tuple0*_T1884=_cycalloc(sizeof(struct _tuple0));_T1884->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));_T1222=yyyvsp + 0;_T1223=&(*_T1222).v;_T1224=(union Cyc_YYSTYPE*)_T1223;*_T1885=Cyc_yyget_String_tok(_T1224);_T1221=(struct _fat_ptr*)_T1885;}_T1884->f1=_T1221;_T1220=(struct _tuple0*)_T1884;}_T1225=
Cyc_Absyn_tag_type(tag);
# 2886
_T1883->f2=Cyc_Absyn_new_vardecl(_T121F,_T1220,_T1225,0,0);_T1214=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T1883;}_T1226=(void*)_T1214;_T1227=yyyvsp;_T1228=_T1227[0];_T1229=_T1228.l;_T122A=_T1229.first_line;_T122B=
# 2888
Cyc_Position_loc_to_seg(_T122A);_T122C=
# 2885
Cyc_Absyn_new_pat(_T1226,_T122B);yyval=Cyc_YY9(_T122C);goto _LL0;}case 437: _T122D=& Cyc_Kinds_ik;_T122E=(struct Cyc_Absyn_Kind*)_T122D;_T122F=
# 2890 "parse.y"
Cyc_Kinds_kind_to_bound(_T122E);{struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(_T122F);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T1883->tag=4;
_T1883->f1=tv;_T1231=yyyvsp;_T1232=_T1231[0];_T1233=_T1232.l;_T1234=_T1233.first_line;_T1235=
Cyc_Position_loc_to_seg(_T1234);{struct _tuple0*_T1884=_cycalloc(sizeof(struct _tuple0));_T1884->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));_T1238=yyyvsp + 0;_T1239=&(*_T1238).v;_T123A=(union Cyc_YYSTYPE*)_T1239;*_T1885=Cyc_yyget_String_tok(_T123A);_T1237=(struct _fat_ptr*)_T1885;}_T1884->f1=_T1237;_T1236=(struct _tuple0*)_T1884;}_T123B=
Cyc_Absyn_var_type(tv);_T123C=Cyc_Absyn_tag_type(_T123B);
# 2892
_T1883->f2=Cyc_Absyn_new_vardecl(_T1235,_T1236,_T123C,0,0);_T1230=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T1883;}_T123D=(void*)_T1230;_T123E=yyyvsp;_T123F=_T123E[0];_T1240=_T123F.l;_T1241=_T1240.first_line;_T1242=
# 2894
Cyc_Position_loc_to_seg(_T1241);_T1243=
# 2891
Cyc_Absyn_new_pat(_T123D,_T1242);yyval=Cyc_YY9(_T1243);goto _LL0;}case 438:{struct _tuple23*_T1883=_cycalloc(sizeof(struct _tuple23));
# 2898 "parse.y"
_T1245=yyyvsp + 0;_T1246=&(*_T1245).v;_T1247=(union Cyc_YYSTYPE*)_T1246;_T1248=Cyc_yyget_YY11(_T1247);_T1883->f0=Cyc_List_rev(_T1248);_T1883->f1=0;_T1244=(struct _tuple23*)_T1883;}yyval=Cyc_YY10(_T1244);goto _LL0;case 439:{struct _tuple23*_T1883=_cycalloc(sizeof(struct _tuple23));
# 2899 "parse.y"
_T124A=yyyvsp + 0;_T124B=&(*_T124A).v;_T124C=(union Cyc_YYSTYPE*)_T124B;_T124D=Cyc_yyget_YY11(_T124C);_T1883->f0=Cyc_List_rev(_T124D);_T1883->f1=1;_T1249=(struct _tuple23*)_T1883;}yyval=Cyc_YY10(_T1249);goto _LL0;case 440:{struct _tuple23*_T1883=_cycalloc(sizeof(struct _tuple23));
# 2900 "parse.y"
_T1883->f0=0;_T1883->f1=1;_T124E=(struct _tuple23*)_T1883;}yyval=Cyc_YY10(_T124E);goto _LL0;case 441:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2903
_T1250=yyyvsp + 0;_T1251=&(*_T1250).v;_T1252=(union Cyc_YYSTYPE*)_T1251;_T1883->hd=Cyc_yyget_YY9(_T1252);_T1883->tl=0;_T124F=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY11(_T124F);goto _LL0;case 442:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2904 "parse.y"
_T1254=yyyvsp + 2;_T1255=&(*_T1254).v;_T1256=(union Cyc_YYSTYPE*)_T1255;_T1883->hd=Cyc_yyget_YY9(_T1256);_T1257=yyyvsp + 0;_T1258=&(*_T1257).v;_T1259=(union Cyc_YYSTYPE*)_T1258;_T1883->tl=Cyc_yyget_YY11(_T1259);_T1253=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY11(_T1253);goto _LL0;case 443:{struct _tuple24*_T1883=_cycalloc(sizeof(struct _tuple24));
# 2907
_T1883->f0=0;_T125B=yyyvsp + 0;_T125C=&(*_T125B).v;_T125D=(union Cyc_YYSTYPE*)_T125C;_T1883->f1=Cyc_yyget_YY9(_T125D);_T125A=(struct _tuple24*)_T1883;}yyval=Cyc_YY12(_T125A);goto _LL0;case 444:{struct _tuple24*_T1883=_cycalloc(sizeof(struct _tuple24));
# 2908 "parse.y"
_T125F=yyyvsp + 0;_T1260=&(*_T125F).v;_T1261=(union Cyc_YYSTYPE*)_T1260;_T1883->f0=Cyc_yyget_YY42(_T1261);_T1262=yyyvsp + 1;_T1263=&(*_T1262).v;_T1264=(union Cyc_YYSTYPE*)_T1263;_T1883->f1=Cyc_yyget_YY9(_T1264);_T125E=(struct _tuple24*)_T1883;}yyval=Cyc_YY12(_T125E);goto _LL0;case 445:{struct _tuple23*_T1883=_cycalloc(sizeof(struct _tuple23));
# 2911
_T1266=yyyvsp + 0;_T1267=&(*_T1266).v;_T1268=(union Cyc_YYSTYPE*)_T1267;_T1269=Cyc_yyget_YY13(_T1268);_T1883->f0=Cyc_List_rev(_T1269);_T1883->f1=0;_T1265=(struct _tuple23*)_T1883;}yyval=Cyc_YY14(_T1265);goto _LL0;case 446:{struct _tuple23*_T1883=_cycalloc(sizeof(struct _tuple23));
# 2912 "parse.y"
_T126B=yyyvsp + 0;_T126C=&(*_T126B).v;_T126D=(union Cyc_YYSTYPE*)_T126C;_T126E=Cyc_yyget_YY13(_T126D);_T1883->f0=Cyc_List_rev(_T126E);_T1883->f1=1;_T126A=(struct _tuple23*)_T1883;}yyval=Cyc_YY14(_T126A);goto _LL0;case 447:{struct _tuple23*_T1883=_cycalloc(sizeof(struct _tuple23));
# 2913 "parse.y"
_T1883->f0=0;_T1883->f1=1;_T126F=(struct _tuple23*)_T1883;}yyval=Cyc_YY14(_T126F);goto _LL0;case 448:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2916
_T1271=yyyvsp + 0;_T1272=&(*_T1271).v;_T1273=(union Cyc_YYSTYPE*)_T1272;_T1883->hd=Cyc_yyget_YY12(_T1273);_T1883->tl=0;_T1270=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY13(_T1270);goto _LL0;case 449:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 2917 "parse.y"
_T1275=yyyvsp + 2;_T1276=&(*_T1275).v;_T1277=(union Cyc_YYSTYPE*)_T1276;_T1883->hd=Cyc_yyget_YY12(_T1277);_T1278=yyyvsp + 0;_T1279=&(*_T1278).v;_T127A=(union Cyc_YYSTYPE*)_T1279;_T1883->tl=Cyc_yyget_YY13(_T127A);_T1274=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY13(_T1274);goto _LL0;case 450: _T127B=yyyvsp;_T127C=_T127B[0];
# 2922 "parse.y"
yyval=_T127C.v;goto _LL0;case 451:
# 2923 "parse.y"
 _T127D=yyyvsp + 0;_T127E=&(*_T127D).v;_T127F=(union Cyc_YYSTYPE*)_T127E;_T1280=Cyc_yyget_Exp_tok(_T127F);_T1281=yyyvsp + 2;_T1282=&(*_T1281).v;_T1283=(union Cyc_YYSTYPE*)_T1282;_T1284=Cyc_yyget_Exp_tok(_T1283);_T1285=yyyvsp;_T1286=_T1285[0];_T1287=_T1286.l;_T1288=_T1287.first_line;_T1289=Cyc_Position_loc_to_seg(_T1288);_T128A=Cyc_Absyn_seq_exp(_T1280,_T1284,_T1289);yyval=Cyc_Exp_tok(_T128A);goto _LL0;case 452: _T128B=yyyvsp;_T128C=_T128B[0];
# 2927 "parse.y"
yyval=_T128C.v;goto _LL0;case 453:
# 2929 "parse.y"
 _T128D=yyyvsp + 0;_T128E=&(*_T128D).v;_T128F=(union Cyc_YYSTYPE*)_T128E;_T1290=Cyc_yyget_Exp_tok(_T128F);_T1291=yyyvsp + 1;_T1292=&(*_T1291).v;_T1293=(union Cyc_YYSTYPE*)_T1292;_T1294=Cyc_yyget_YY7(_T1293);_T1295=yyyvsp + 2;_T1296=&(*_T1295).v;_T1297=(union Cyc_YYSTYPE*)_T1296;_T1298=Cyc_yyget_Exp_tok(_T1297);_T1299=yyyvsp;_T129A=_T1299[0];_T129B=_T129A.l;_T129C=_T129B.first_line;_T129D=Cyc_Position_loc_to_seg(_T129C);_T129E=Cyc_Absyn_assignop_exp(_T1290,_T1294,_T1298,_T129D);yyval=Cyc_Exp_tok(_T129E);goto _LL0;case 454:
# 2931 "parse.y"
 _T129F=yyyvsp + 0;_T12A0=&(*_T129F).v;_T12A1=(union Cyc_YYSTYPE*)_T12A0;_T12A2=Cyc_yyget_Exp_tok(_T12A1);_T12A3=yyyvsp + 2;_T12A4=&(*_T12A3).v;_T12A5=(union Cyc_YYSTYPE*)_T12A4;_T12A6=Cyc_yyget_Exp_tok(_T12A5);_T12A7=yyyvsp;_T12A8=_T12A7[0];_T12A9=_T12A8.l;_T12AA=_T12A9.first_line;_T12AB=Cyc_Position_loc_to_seg(_T12AA);_T12AC=Cyc_Absyn_swap_exp(_T12A2,_T12A6,_T12AB);yyval=Cyc_Exp_tok(_T12AC);goto _LL0;case 455:
# 2935 "parse.y"
 yyval=Cyc_YY7(0);goto _LL0;case 456:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2936 "parse.y"
_T1883->v=(void*)1U;_T12AD=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12AD);goto _LL0;case 457:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2937 "parse.y"
_T1883->v=(void*)3U;_T12AE=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12AE);goto _LL0;case 458:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2938 "parse.y"
_T1883->v=(void*)4U;_T12AF=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12AF);goto _LL0;case 459:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2939 "parse.y"
_T1883->v=(void*)0U;_T12B0=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12B0);goto _LL0;case 460:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2940 "parse.y"
_T1883->v=(void*)2U;_T12B1=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12B1);goto _LL0;case 461:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2941 "parse.y"
_T1883->v=(void*)16U;_T12B2=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12B2);goto _LL0;case 462:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2942 "parse.y"
_T1883->v=(void*)17U;_T12B3=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12B3);goto _LL0;case 463:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2943 "parse.y"
_T1883->v=(void*)13U;_T12B4=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12B4);goto _LL0;case 464:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2944 "parse.y"
_T1883->v=(void*)15U;_T12B5=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12B5);goto _LL0;case 465:{struct Cyc_Core_Opt*_T1883=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2945 "parse.y"
_T1883->v=(void*)14U;_T12B6=(struct Cyc_Core_Opt*)_T1883;}yyval=Cyc_YY7(_T12B6);goto _LL0;case 466: _T12B7=yyyvsp;_T12B8=_T12B7[0];
# 2949 "parse.y"
yyval=_T12B8.v;goto _LL0;case 467:
# 2951 "parse.y"
 _T12B9=yyyvsp + 0;_T12BA=&(*_T12B9).v;_T12BB=(union Cyc_YYSTYPE*)_T12BA;_T12BC=Cyc_yyget_Exp_tok(_T12BB);_T12BD=yyyvsp + 2;_T12BE=&(*_T12BD).v;_T12BF=(union Cyc_YYSTYPE*)_T12BE;_T12C0=Cyc_yyget_Exp_tok(_T12BF);_T12C1=yyyvsp + 4;_T12C2=&(*_T12C1).v;_T12C3=(union Cyc_YYSTYPE*)_T12C2;_T12C4=Cyc_yyget_Exp_tok(_T12C3);_T12C5=yyyvsp;_T12C6=_T12C5[0];_T12C7=_T12C6.l;_T12C8=_T12C7.first_line;_T12C9=Cyc_Position_loc_to_seg(_T12C8);_T12CA=Cyc_Absyn_conditional_exp(_T12BC,_T12C0,_T12C4,_T12C9);yyval=Cyc_Exp_tok(_T12CA);goto _LL0;case 468:
# 2953 "parse.y"
 _T12CB=yyyvsp + 1;_T12CC=&(*_T12CB).v;_T12CD=(union Cyc_YYSTYPE*)_T12CC;_T12CE=Cyc_yyget_Exp_tok(_T12CD);_T12CF=yyyvsp;_T12D0=_T12CF[0];_T12D1=_T12D0.l;_T12D2=_T12D1.first_line;_T12D3=Cyc_Position_loc_to_seg(_T12D2);_T12D4=Cyc_Absyn_throw_exp(_T12CE,_T12D3);yyval=Cyc_Exp_tok(_T12D4);goto _LL0;case 469:
# 2955 "parse.y"
 _T12D5=yyyvsp + 1;_T12D6=&(*_T12D5).v;_T12D7=(union Cyc_YYSTYPE*)_T12D6;_T12D8=Cyc_yyget_Exp_tok(_T12D7);_T12D9=yyyvsp;_T12DA=_T12D9[0];_T12DB=_T12DA.l;_T12DC=_T12DB.first_line;_T12DD=Cyc_Position_loc_to_seg(_T12DC);_T12DE=Cyc_Absyn_New_exp(0,_T12D8,0,_T12DD);yyval=Cyc_Exp_tok(_T12DE);goto _LL0;case 470:
# 2956 "parse.y"
 _T12DF=yyyvsp + 1;_T12E0=&(*_T12DF).v;_T12E1=(union Cyc_YYSTYPE*)_T12E0;_T12E2=Cyc_yyget_Exp_tok(_T12E1);_T12E3=yyyvsp;_T12E4=_T12E3[0];_T12E5=_T12E4.l;_T12E6=_T12E5.first_line;_T12E7=Cyc_Position_loc_to_seg(_T12E6);_T12E8=Cyc_Absyn_New_exp(0,_T12E2,0,_T12E7);yyval=Cyc_Exp_tok(_T12E8);goto _LL0;case 471:
# 2957 "parse.y"
 _T12E9=yyyvsp + 4;_T12EA=&(*_T12E9).v;_T12EB=(union Cyc_YYSTYPE*)_T12EA;_T12EC=Cyc_yyget_Exp_tok(_T12EB);_T12ED=yyyvsp + 2;_T12EE=&(*_T12ED).v;_T12EF=(union Cyc_YYSTYPE*)_T12EE;_T12F0=Cyc_yyget_Exp_tok(_T12EF);_T12F1=yyyvsp;_T12F2=_T12F1[0];_T12F3=_T12F2.l;_T12F4=_T12F3.first_line;_T12F5=Cyc_Position_loc_to_seg(_T12F4);_T12F6=Cyc_Absyn_New_exp(0,_T12EC,_T12F0,_T12F5);yyval=Cyc_Exp_tok(_T12F6);goto _LL0;case 472:
# 2958 "parse.y"
 _T12F7=yyyvsp + 4;_T12F8=&(*_T12F7).v;_T12F9=(union Cyc_YYSTYPE*)_T12F8;_T12FA=Cyc_yyget_Exp_tok(_T12F9);_T12FB=yyyvsp + 2;_T12FC=&(*_T12FB).v;_T12FD=(union Cyc_YYSTYPE*)_T12FC;_T12FE=Cyc_yyget_Exp_tok(_T12FD);_T12FF=yyyvsp;_T1300=_T12FF[0];_T1301=_T1300.l;_T1302=_T1301.first_line;_T1303=Cyc_Position_loc_to_seg(_T1302);_T1304=Cyc_Absyn_New_exp(0,_T12FA,_T12FE,_T1303);yyval=Cyc_Exp_tok(_T1304);goto _LL0;case 473:
# 2959 "parse.y"
 _T1305=yyyvsp + 2;_T1306=&(*_T1305).v;_T1307=(union Cyc_YYSTYPE*)_T1306;_T1308=Cyc_yyget_Exp_tok(_T1307);{struct _tuple16 _T1883=Cyc_Parse_split_seq(_T1308);struct Cyc_Absyn_Exp*_T1884;struct Cyc_Absyn_Exp*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_Absyn_Exp*rgn=_T1885;struct Cyc_Absyn_Exp*qual=_T1884;_T1309=rgn;_T130A=yyyvsp + 4;_T130B=&(*_T130A).v;_T130C=(union Cyc_YYSTYPE*)_T130B;_T130D=Cyc_yyget_Exp_tok(_T130C);_T130E=qual;_T130F=yyyvsp;_T1310=_T130F[0];_T1311=_T1310.l;_T1312=_T1311.first_line;_T1313=Cyc_Position_loc_to_seg(_T1312);_T1314=Cyc_Absyn_New_exp(_T1309,_T130D,_T130E,_T1313);yyval=Cyc_Exp_tok(_T1314);goto _LL0;}}case 474:
# 2960 "parse.y"
 _T1315=yyyvsp + 2;_T1316=&(*_T1315).v;_T1317=(union Cyc_YYSTYPE*)_T1316;_T1318=Cyc_yyget_Exp_tok(_T1317);{struct _tuple16 _T1883=Cyc_Parse_split_seq(_T1318);struct Cyc_Absyn_Exp*_T1884;struct Cyc_Absyn_Exp*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_Absyn_Exp*rgn=_T1885;struct Cyc_Absyn_Exp*qual=_T1884;_T1319=rgn;_T131A=yyyvsp + 4;_T131B=&(*_T131A).v;_T131C=(union Cyc_YYSTYPE*)_T131B;_T131D=Cyc_yyget_Exp_tok(_T131C);_T131E=qual;_T131F=yyyvsp;_T1320=_T131F[0];_T1321=_T1320.l;_T1322=_T1321.first_line;_T1323=Cyc_Position_loc_to_seg(_T1322);_T1324=Cyc_Absyn_New_exp(_T1319,_T131D,_T131E,_T1323);yyval=Cyc_Exp_tok(_T1324);goto _LL0;}}case 475: _T1325=yyyvsp;_T1326=_T1325[0];
# 2972 "parse.y"
yyval=_T1326.v;goto _LL0;case 476: _T1327=yyyvsp;_T1328=_T1327[0];
# 2975
yyval=_T1328.v;goto _LL0;case 477:
# 2977 "parse.y"
 _T1329=yyyvsp + 0;_T132A=&(*_T1329).v;_T132B=(union Cyc_YYSTYPE*)_T132A;_T132C=Cyc_yyget_Exp_tok(_T132B);_T132D=yyyvsp + 2;_T132E=&(*_T132D).v;_T132F=(union Cyc_YYSTYPE*)_T132E;_T1330=Cyc_yyget_Exp_tok(_T132F);_T1331=yyyvsp;_T1332=_T1331[0];_T1333=_T1332.l;_T1334=_T1333.first_line;_T1335=Cyc_Position_loc_to_seg(_T1334);_T1336=Cyc_Absyn_or_exp(_T132C,_T1330,_T1335);yyval=Cyc_Exp_tok(_T1336);goto _LL0;case 478: _T1337=yyyvsp;_T1338=_T1337[0];
# 2980
yyval=_T1338.v;goto _LL0;case 479:
# 2982 "parse.y"
 _T1339=yyyvsp + 0;_T133A=&(*_T1339).v;_T133B=(union Cyc_YYSTYPE*)_T133A;_T133C=Cyc_yyget_Exp_tok(_T133B);_T133D=yyyvsp + 2;_T133E=&(*_T133D).v;_T133F=(union Cyc_YYSTYPE*)_T133E;_T1340=Cyc_yyget_Exp_tok(_T133F);_T1341=yyyvsp;_T1342=_T1341[0];_T1343=_T1342.l;_T1344=_T1343.first_line;_T1345=Cyc_Position_loc_to_seg(_T1344);_T1346=Cyc_Absyn_and_exp(_T133C,_T1340,_T1345);yyval=Cyc_Exp_tok(_T1346);goto _LL0;case 480: _T1347=yyyvsp;_T1348=_T1347[0];
# 2985
yyval=_T1348.v;goto _LL0;case 481:
# 2987 "parse.y"
 _T1349=yyyvsp + 0;_T134A=&(*_T1349).v;_T134B=(union Cyc_YYSTYPE*)_T134A;_T134C=Cyc_yyget_Exp_tok(_T134B);_T134D=yyyvsp + 2;_T134E=&(*_T134D).v;_T134F=(union Cyc_YYSTYPE*)_T134E;_T1350=Cyc_yyget_Exp_tok(_T134F);_T1351=yyyvsp;_T1352=_T1351[0];_T1353=_T1352.l;_T1354=_T1353.first_line;_T1355=Cyc_Position_loc_to_seg(_T1354);_T1356=Cyc_Absyn_prim2_exp(14U,_T134C,_T1350,_T1355);yyval=Cyc_Exp_tok(_T1356);goto _LL0;case 482: _T1357=yyyvsp;_T1358=_T1357[0];
# 2990
yyval=_T1358.v;goto _LL0;case 483:
# 2992 "parse.y"
 _T1359=yyyvsp + 0;_T135A=&(*_T1359).v;_T135B=(union Cyc_YYSTYPE*)_T135A;_T135C=Cyc_yyget_Exp_tok(_T135B);_T135D=yyyvsp + 2;_T135E=&(*_T135D).v;_T135F=(union Cyc_YYSTYPE*)_T135E;_T1360=Cyc_yyget_Exp_tok(_T135F);_T1361=yyyvsp;_T1362=_T1361[0];_T1363=_T1362.l;_T1364=_T1363.first_line;_T1365=Cyc_Position_loc_to_seg(_T1364);_T1366=Cyc_Absyn_prim2_exp(15U,_T135C,_T1360,_T1365);yyval=Cyc_Exp_tok(_T1366);goto _LL0;case 484: _T1367=yyyvsp;_T1368=_T1367[0];
# 2995
yyval=_T1368.v;goto _LL0;case 485:
# 2997 "parse.y"
 _T1369=yyyvsp + 0;_T136A=&(*_T1369).v;_T136B=(union Cyc_YYSTYPE*)_T136A;_T136C=Cyc_yyget_Exp_tok(_T136B);_T136D=yyyvsp + 2;_T136E=&(*_T136D).v;_T136F=(union Cyc_YYSTYPE*)_T136E;_T1370=Cyc_yyget_Exp_tok(_T136F);_T1371=yyyvsp;_T1372=_T1371[0];_T1373=_T1372.l;_T1374=_T1373.first_line;_T1375=Cyc_Position_loc_to_seg(_T1374);_T1376=Cyc_Absyn_prim2_exp(13U,_T136C,_T1370,_T1375);yyval=Cyc_Exp_tok(_T1376);goto _LL0;case 486: _T1377=yyyvsp;_T1378=_T1377[0];
# 3000
yyval=_T1378.v;goto _LL0;case 487:
# 3002 "parse.y"
 _T1379=yyyvsp + 1;_T137A=&(*_T1379).v;_T137B=(union Cyc_YYSTYPE*)_T137A;_T137C=Cyc_yyget_YY69(_T137B);_T137D=yyyvsp + 0;_T137E=&(*_T137D).v;_T137F=(union Cyc_YYSTYPE*)_T137E;_T1380=Cyc_yyget_Exp_tok(_T137F);_T1381=yyyvsp + 2;_T1382=&(*_T1381).v;_T1383=(union Cyc_YYSTYPE*)_T1382;_T1384=Cyc_yyget_Exp_tok(_T1383);_T1385=yyyvsp;_T1386=_T1385[0];_T1387=_T1386.l;_T1388=_T1387.first_line;_T1389=Cyc_Position_loc_to_seg(_T1388);_T138A=_T137C(_T1380,_T1384,_T1389);yyval=Cyc_Exp_tok(_T138A);goto _LL0;case 488: _T138B=yyyvsp;_T138C=_T138B[0];
# 3005
yyval=_T138C.v;goto _LL0;case 489:
# 3007 "parse.y"
 _T138D=yyyvsp + 1;_T138E=&(*_T138D).v;_T138F=(union Cyc_YYSTYPE*)_T138E;_T1390=Cyc_yyget_YY69(_T138F);_T1391=yyyvsp + 0;_T1392=&(*_T1391).v;_T1393=(union Cyc_YYSTYPE*)_T1392;_T1394=Cyc_yyget_Exp_tok(_T1393);_T1395=yyyvsp + 2;_T1396=&(*_T1395).v;_T1397=(union Cyc_YYSTYPE*)_T1396;_T1398=Cyc_yyget_Exp_tok(_T1397);_T1399=yyyvsp;_T139A=_T1399[0];_T139B=_T139A.l;_T139C=_T139B.first_line;_T139D=Cyc_Position_loc_to_seg(_T139C);_T139E=_T1390(_T1394,_T1398,_T139D);yyval=Cyc_Exp_tok(_T139E);goto _LL0;case 490: _T139F=yyyvsp;_T13A0=_T139F[0];
# 3010
yyval=_T13A0.v;goto _LL0;case 491:
# 3012 "parse.y"
 _T13A1=yyyvsp + 0;_T13A2=&(*_T13A1).v;_T13A3=(union Cyc_YYSTYPE*)_T13A2;_T13A4=Cyc_yyget_Exp_tok(_T13A3);_T13A5=yyyvsp + 2;_T13A6=&(*_T13A5).v;_T13A7=(union Cyc_YYSTYPE*)_T13A6;_T13A8=Cyc_yyget_Exp_tok(_T13A7);_T13A9=yyyvsp;_T13AA=_T13A9[0];_T13AB=_T13AA.l;_T13AC=_T13AB.first_line;_T13AD=Cyc_Position_loc_to_seg(_T13AC);_T13AE=Cyc_Absyn_prim2_exp(16U,_T13A4,_T13A8,_T13AD);yyval=Cyc_Exp_tok(_T13AE);goto _LL0;case 492:
# 3014 "parse.y"
 _T13AF=yyyvsp + 0;_T13B0=&(*_T13AF).v;_T13B1=(union Cyc_YYSTYPE*)_T13B0;_T13B2=Cyc_yyget_Exp_tok(_T13B1);_T13B3=yyyvsp + 2;_T13B4=&(*_T13B3).v;_T13B5=(union Cyc_YYSTYPE*)_T13B4;_T13B6=Cyc_yyget_Exp_tok(_T13B5);_T13B7=yyyvsp;_T13B8=_T13B7[0];_T13B9=_T13B8.l;_T13BA=_T13B9.first_line;_T13BB=Cyc_Position_loc_to_seg(_T13BA);_T13BC=Cyc_Absyn_prim2_exp(17U,_T13B2,_T13B6,_T13BB);yyval=Cyc_Exp_tok(_T13BC);goto _LL0;case 493: _T13BD=yyyvsp;_T13BE=_T13BD[0];
# 3017
yyval=_T13BE.v;goto _LL0;case 494:
# 3019 "parse.y"
 _T13BF=yyyvsp + 1;_T13C0=&(*_T13BF).v;_T13C1=(union Cyc_YYSTYPE*)_T13C0;_T13C2=Cyc_yyget_YY6(_T13C1);_T13C3=yyyvsp + 0;_T13C4=&(*_T13C3).v;_T13C5=(union Cyc_YYSTYPE*)_T13C4;_T13C6=Cyc_yyget_Exp_tok(_T13C5);_T13C7=yyyvsp + 2;_T13C8=&(*_T13C7).v;_T13C9=(union Cyc_YYSTYPE*)_T13C8;_T13CA=Cyc_yyget_Exp_tok(_T13C9);_T13CB=yyyvsp;_T13CC=_T13CB[0];_T13CD=_T13CC.l;_T13CE=_T13CD.first_line;_T13CF=Cyc_Position_loc_to_seg(_T13CE);_T13D0=Cyc_Absyn_prim2_exp(_T13C2,_T13C6,_T13CA,_T13CF);yyval=Cyc_Exp_tok(_T13D0);goto _LL0;case 495: _T13D1=yyyvsp;_T13D2=_T13D1[0];
# 3022
yyval=_T13D2.v;goto _LL0;case 496:
# 3024 "parse.y"
 _T13D3=yyyvsp + 1;_T13D4=&(*_T13D3).v;_T13D5=(union Cyc_YYSTYPE*)_T13D4;_T13D6=Cyc_yyget_YY6(_T13D5);_T13D7=yyyvsp + 0;_T13D8=&(*_T13D7).v;_T13D9=(union Cyc_YYSTYPE*)_T13D8;_T13DA=Cyc_yyget_Exp_tok(_T13D9);_T13DB=yyyvsp + 2;_T13DC=&(*_T13DB).v;_T13DD=(union Cyc_YYSTYPE*)_T13DC;_T13DE=Cyc_yyget_Exp_tok(_T13DD);_T13DF=yyyvsp;_T13E0=_T13DF[0];_T13E1=_T13E0.l;_T13E2=_T13E1.first_line;_T13E3=Cyc_Position_loc_to_seg(_T13E2);_T13E4=Cyc_Absyn_prim2_exp(_T13D6,_T13DA,_T13DE,_T13E3);yyval=Cyc_Exp_tok(_T13E4);goto _LL0;case 497: _T13E5=Cyc_Absyn_eq_exp;
# 3027
yyval=Cyc_YY69(_T13E5);goto _LL0;case 498: _T13E6=Cyc_Absyn_neq_exp;
# 3028 "parse.y"
yyval=Cyc_YY69(_T13E6);goto _LL0;case 499: _T13E7=Cyc_Absyn_lt_exp;
# 3031
yyval=Cyc_YY69(_T13E7);goto _LL0;case 500: _T13E8=Cyc_Absyn_gt_exp;
# 3032 "parse.y"
yyval=Cyc_YY69(_T13E8);goto _LL0;case 501: _T13E9=Cyc_Absyn_lte_exp;
# 3033 "parse.y"
yyval=Cyc_YY69(_T13E9);goto _LL0;case 502: _T13EA=Cyc_Absyn_gte_exp;
# 3034 "parse.y"
yyval=Cyc_YY69(_T13EA);goto _LL0;case 503:
# 3037
 yyval=Cyc_YY6(0U);goto _LL0;case 504:
# 3038 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 505:
# 3041
 yyval=Cyc_YY6(1U);goto _LL0;case 506:
# 3042 "parse.y"
 yyval=Cyc_YY6(3U);goto _LL0;case 507:
# 3043 "parse.y"
 yyval=Cyc_YY6(4U);goto _LL0;case 508: _T13EB=yyyvsp;_T13EC=_T13EB[0];
# 3047 "parse.y"
yyval=_T13EC.v;goto _LL0;case 509:
# 3049 "parse.y"
 _T13ED=yyyvsp + 1;_T13EE=&(*_T13ED).v;_T13EF=(union Cyc_YYSTYPE*)_T13EE;_T13F0=Cyc_yyget_YY38(_T13EF);_T13F1=yyyvsp;_T13F2=_T13F1[1];_T13F3=_T13F2.l;_T13F4=_T13F3.first_line;_T13F5=Cyc_Position_loc_to_seg(_T13F4);{void*t=Cyc_Parse_type_name_to_type(_T13F0,_T13F5);_T13F6=t;
_T13F7=yyyvsp + 3;_T13F8=&(*_T13F7).v;_T13F9=(union Cyc_YYSTYPE*)_T13F8;_T13FA=Cyc_yyget_Exp_tok(_T13F9);_T13FB=yyyvsp;_T13FC=_T13FB[0];_T13FD=_T13FC.l;_T13FE=_T13FD.first_line;_T13FF=Cyc_Position_loc_to_seg(_T13FE);_T1400=Cyc_Absyn_cast_exp(_T13F6,_T13FA,1,0U,_T13FF);yyval=Cyc_Exp_tok(_T1400);goto _LL0;}case 510: _T1401=yyyvsp;_T1402=_T1401[0];
# 3054 "parse.y"
yyval=_T1402.v;goto _LL0;case 511:
# 3055 "parse.y"
 _T1403=yyyvsp + 1;_T1404=&(*_T1403).v;_T1405=(union Cyc_YYSTYPE*)_T1404;_T1406=Cyc_yyget_Exp_tok(_T1405);_T1407=yyyvsp;_T1408=_T1407[0];_T1409=_T1408.l;_T140A=_T1409.first_line;_T140B=Cyc_Position_loc_to_seg(_T140A);_T140C=Cyc_Absyn_increment_exp(_T1406,0U,_T140B);yyval=Cyc_Exp_tok(_T140C);goto _LL0;case 512:
# 3056 "parse.y"
 _T140D=yyyvsp + 1;_T140E=&(*_T140D).v;_T140F=(union Cyc_YYSTYPE*)_T140E;_T1410=Cyc_yyget_Exp_tok(_T140F);_T1411=yyyvsp;_T1412=_T1411[0];_T1413=_T1412.l;_T1414=_T1413.first_line;_T1415=Cyc_Position_loc_to_seg(_T1414);_T1416=Cyc_Absyn_increment_exp(_T1410,2U,_T1415);yyval=Cyc_Exp_tok(_T1416);goto _LL0;case 513:
# 3057 "parse.y"
 _T1417=yyyvsp + 1;_T1418=&(*_T1417).v;_T1419=(union Cyc_YYSTYPE*)_T1418;_T141A=Cyc_yyget_Exp_tok(_T1419);_T141B=yyyvsp;_T141C=_T141B[0];_T141D=_T141C.l;_T141E=_T141D.first_line;_T141F=Cyc_Position_loc_to_seg(_T141E);_T1420=Cyc_Absyn_address_exp(_T141A,_T141F);yyval=Cyc_Exp_tok(_T1420);goto _LL0;case 514:
# 3058 "parse.y"
 _T1421=yyyvsp + 1;_T1422=&(*_T1421).v;_T1423=(union Cyc_YYSTYPE*)_T1422;_T1424=Cyc_yyget_Exp_tok(_T1423);_T1425=yyyvsp;_T1426=_T1425[0];_T1427=_T1426.l;_T1428=_T1427.first_line;_T1429=Cyc_Position_loc_to_seg(_T1428);_T142A=Cyc_Absyn_deref_exp(_T1424,_T1429);yyval=Cyc_Exp_tok(_T142A);goto _LL0;case 515:
# 3059 "parse.y"
 _T142B=yyyvsp + 0;_T142C=&(*_T142B).v;_T142D=(union Cyc_YYSTYPE*)_T142C;_T142E=Cyc_yyget_YY6(_T142D);_T142F=yyyvsp + 1;_T1430=&(*_T142F).v;_T1431=(union Cyc_YYSTYPE*)_T1430;_T1432=Cyc_yyget_Exp_tok(_T1431);_T1433=yyyvsp;_T1434=_T1433[0];_T1435=_T1434.l;_T1436=_T1435.first_line;_T1437=Cyc_Position_loc_to_seg(_T1436);_T1438=Cyc_Absyn_prim1_exp(_T142E,_T1432,_T1437);yyval=Cyc_Exp_tok(_T1438);goto _LL0;case 516:
# 3061 "parse.y"
 _T1439=yyyvsp + 2;_T143A=&(*_T1439).v;_T143B=(union Cyc_YYSTYPE*)_T143A;_T143C=Cyc_yyget_YY38(_T143B);_T143D=yyyvsp;_T143E=_T143D[2];_T143F=_T143E.l;_T1440=_T143F.first_line;_T1441=Cyc_Position_loc_to_seg(_T1440);{void*t=Cyc_Parse_type_name_to_type(_T143C,_T1441);_T1442=t;_T1443=yyyvsp;_T1444=_T1443[0];_T1445=_T1444.l;_T1446=_T1445.first_line;_T1447=
Cyc_Position_loc_to_seg(_T1446);_T1448=Cyc_Absyn_sizeoftype_exp(_T1442,_T1447);yyval=Cyc_Exp_tok(_T1448);goto _LL0;}case 517:
# 3064 "parse.y"
 _T1449=yyyvsp + 1;_T144A=&(*_T1449).v;_T144B=(union Cyc_YYSTYPE*)_T144A;_T144C=Cyc_yyget_Exp_tok(_T144B);_T144D=yyyvsp;_T144E=_T144D[0];_T144F=_T144E.l;_T1450=_T144F.first_line;_T1451=Cyc_Position_loc_to_seg(_T1450);_T1452=Cyc_Absyn_sizeofexp_exp(_T144C,_T1451);yyval=Cyc_Exp_tok(_T1452);goto _LL0;case 518:
# 3066 "parse.y"
 _T1453=yyyvsp + 2;_T1454=&(*_T1453).v;_T1455=(union Cyc_YYSTYPE*)_T1454;_T1456=Cyc_yyget_YY38(_T1455);_T1457=yyyvsp;_T1458=_T1457[2];_T1459=_T1458.l;_T145A=_T1459.first_line;_T145B=Cyc_Position_loc_to_seg(_T145A);{void*t=Cyc_Parse_type_name_to_type(_T1456,_T145B);_T145C=t;
_T145D=yyyvsp + 4;_T145E=&(*_T145D).v;_T145F=(union Cyc_YYSTYPE*)_T145E;_T1460=Cyc_yyget_YY3(_T145F);_T1461=Cyc_List_imp_rev(_T1460);_T1462=yyyvsp;_T1463=_T1462[0];_T1464=_T1463.l;_T1465=_T1464.first_line;_T1466=Cyc_Position_loc_to_seg(_T1465);_T1467=Cyc_Absyn_offsetof_exp(_T145C,_T1461,_T1466);yyval=Cyc_Exp_tok(_T1467);goto _LL0;}case 519:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1883->tag=33;
# 3071
_T1883->f1.mknd=0U;_T1883->f1.rgn=0;_T1883->f1.aqual=0;_T1883->f1.elt_type=0;_T1469=yyyvsp + 2;_T146A=&(*_T1469).v;_T146B=(union Cyc_YYSTYPE*)_T146A;_T1883->f1.num_elts=Cyc_yyget_Exp_tok(_T146B);_T1883->f1.fat_result=0;_T1883->f1.inline_call=0;_T1468=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1883;}_T146C=(void*)_T1468;_T146D=yyyvsp;_T146E=_T146D[0];_T146F=_T146E.l;_T1470=_T146F.first_line;_T1471=
Cyc_Position_loc_to_seg(_T1470);_T1472=
# 3071
Cyc_Absyn_new_exp(_T146C,_T1471);yyval=Cyc_Exp_tok(_T1472);goto _LL0;case 520:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1883->tag=33;
# 3074 "parse.y"
_T1883->f1.mknd=0U;_T1474=yyyvsp + 2;_T1475=&(*_T1474).v;_T1476=(union Cyc_YYSTYPE*)_T1475;_T1883->f1.rgn=Cyc_yyget_Exp_tok(_T1476);_T1477=yyyvsp + 4;_T1478=&(*_T1477).v;_T1479=(union Cyc_YYSTYPE*)_T1478;_T1883->f1.aqual=Cyc_yyget_Exp_tok(_T1479);_T1883->f1.elt_type=0;_T147A=yyyvsp + 6;_T147B=&(*_T147A).v;_T147C=(union Cyc_YYSTYPE*)_T147B;_T1883->f1.num_elts=Cyc_yyget_Exp_tok(_T147C);_T1883->f1.fat_result=0;_T1883->f1.inline_call=0;_T1473=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1883;}_T147D=(void*)_T1473;_T147E=yyyvsp;_T147F=_T147E[0];_T1480=_T147F.l;_T1481=_T1480.first_line;_T1482=
Cyc_Position_loc_to_seg(_T1481);_T1483=
# 3074
Cyc_Absyn_new_exp(_T147D,_T1482);yyval=Cyc_Exp_tok(_T1483);goto _LL0;case 521:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1883->tag=33;
# 3077 "parse.y"
_T1883->f1.mknd=0U;_T1485=yyyvsp + 2;_T1486=&(*_T1485).v;_T1487=(union Cyc_YYSTYPE*)_T1486;_T1883->f1.rgn=Cyc_yyget_Exp_tok(_T1487);_T1883->f1.aqual=0;_T1883->f1.elt_type=0;_T1488=yyyvsp + 4;_T1489=&(*_T1488).v;_T148A=(union Cyc_YYSTYPE*)_T1489;_T1883->f1.num_elts=Cyc_yyget_Exp_tok(_T148A);_T1883->f1.fat_result=0;_T1883->f1.inline_call=0;_T1484=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1883;}_T148B=(void*)_T1484;_T148C=yyyvsp;_T148D=_T148C[0];_T148E=_T148D.l;_T148F=_T148E.first_line;_T1490=
Cyc_Position_loc_to_seg(_T148F);_T1491=
# 3077
Cyc_Absyn_new_exp(_T148B,_T1490);yyval=Cyc_Exp_tok(_T1491);goto _LL0;case 522:
# 3080 "parse.y"
 _T1492=yyyvsp + 2;_T1493=&(*_T1492).v;_T1494=(union Cyc_YYSTYPE*)_T1493;_T1495=Cyc_yyget_Exp_tok(_T1494);{struct _tuple16 _T1883=Cyc_Parse_split_seq(_T1495);struct Cyc_Absyn_Exp*_T1884;struct Cyc_Absyn_Exp*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_Absyn_Exp*rgn=_T1885;struct Cyc_Absyn_Exp*qual=_T1884;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1886->tag=33;
_T1886->f1.mknd=2U;_T1886->f1.rgn=rgn;_T1886->f1.aqual=qual;_T1886->f1.elt_type=0;_T1497=yyyvsp + 4;_T1498=&(*_T1497).v;_T1499=(union Cyc_YYSTYPE*)_T1498;_T1886->f1.num_elts=Cyc_yyget_Exp_tok(_T1499);_T1886->f1.fat_result=0;_T1886->f1.inline_call=0;_T1496=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1886;}_T149A=(void*)_T1496;_T149B=yyyvsp;_T149C=_T149B[0];_T149D=_T149C.l;_T149E=_T149D.first_line;_T149F=
Cyc_Position_loc_to_seg(_T149E);_T14A0=
# 3081
Cyc_Absyn_new_exp(_T149A,_T149F);yyval=Cyc_Exp_tok(_T14A0);goto _LL0;}}case 523:
# 3084 "parse.y"
 _T14A1=yyyvsp + 2;_T14A2=&(*_T14A1).v;_T14A3=(union Cyc_YYSTYPE*)_T14A2;_T14A4=Cyc_yyget_Exp_tok(_T14A3);{struct _tuple16 _T1883=Cyc_Parse_split_seq(_T14A4);struct Cyc_Absyn_Exp*_T1884;struct Cyc_Absyn_Exp*_T1885;_T1885=_T1883.f0;_T1884=_T1883.f1;{struct Cyc_Absyn_Exp*rgn=_T1885;struct Cyc_Absyn_Exp*qual=_T1884;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1886->tag=33;
_T1886->f1.mknd=0U;_T1886->f1.rgn=rgn;_T1886->f1.aqual=qual;_T1886->f1.elt_type=0;_T14A6=yyyvsp + 4;_T14A7=&(*_T14A6).v;_T14A8=(union Cyc_YYSTYPE*)_T14A7;_T1886->f1.num_elts=Cyc_yyget_Exp_tok(_T14A8);_T1886->f1.fat_result=0;_T1886->f1.inline_call=1;_T14A5=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1886;}_T14A9=(void*)_T14A5;_T14AA=yyyvsp;_T14AB=_T14AA[0];_T14AC=_T14AB.l;_T14AD=_T14AC.first_line;_T14AE=
Cyc_Position_loc_to_seg(_T14AD);_T14AF=
# 3085
Cyc_Absyn_new_exp(_T14A9,_T14AE);yyval=Cyc_Exp_tok(_T14AF);goto _LL0;}}case 524:
# 3088 "parse.y"
 _T14B0=yyyvsp + 6;_T14B1=&(*_T14B0).v;_T14B2=(union Cyc_YYSTYPE*)_T14B1;_T14B3=Cyc_yyget_YY38(_T14B2);_T14B4=yyyvsp;_T14B5=_T14B4[6];_T14B6=_T14B5.l;_T14B7=_T14B6.first_line;_T14B8=Cyc_Position_loc_to_seg(_T14B7);{void*t=Cyc_Parse_type_name_to_type(_T14B3,_T14B8);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1883->tag=33;
_T1883->f1.mknd=1U;_T1883->f1.rgn=0;_T1883->f1.aqual=0;{void**_T1884=_cycalloc(sizeof(void*));*_T1884=t;_T14BA=(void**)_T1884;}_T1883->f1.elt_type=_T14BA;_T14BB=yyyvsp + 2;_T14BC=&(*_T14BB).v;_T14BD=(union Cyc_YYSTYPE*)_T14BC;_T1883->f1.num_elts=Cyc_yyget_Exp_tok(_T14BD);_T1883->f1.fat_result=0;_T1883->f1.inline_call=0;_T14B9=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1883;}_T14BE=(void*)_T14B9;_T14BF=yyyvsp;_T14C0=_T14BF[0];_T14C1=_T14C0.l;_T14C2=_T14C1.first_line;_T14C3=
Cyc_Position_loc_to_seg(_T14C2);_T14C4=
# 3089
Cyc_Absyn_new_exp(_T14BE,_T14C3);yyval=Cyc_Exp_tok(_T14C4);goto _LL0;}case 525:
# 3092 "parse.y"
 _T14C5=yyyvsp + 10;_T14C6=&(*_T14C5).v;_T14C7=(union Cyc_YYSTYPE*)_T14C6;_T14C8=Cyc_yyget_YY38(_T14C7);_T14C9=yyyvsp;_T14CA=_T14C9[10];_T14CB=_T14CA.l;_T14CC=_T14CB.first_line;_T14CD=Cyc_Position_loc_to_seg(_T14CC);{void*t=Cyc_Parse_type_name_to_type(_T14C8,_T14CD);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1883->tag=33;
_T1883->f1.mknd=1U;_T14CF=yyyvsp + 2;_T14D0=&(*_T14CF).v;_T14D1=(union Cyc_YYSTYPE*)_T14D0;_T1883->f1.rgn=Cyc_yyget_Exp_tok(_T14D1);_T14D2=yyyvsp + 4;_T14D3=&(*_T14D2).v;_T14D4=(union Cyc_YYSTYPE*)_T14D3;_T1883->f1.aqual=Cyc_yyget_Exp_tok(_T14D4);{void**_T1884=_cycalloc(sizeof(void*));*_T1884=t;_T14D5=(void**)_T1884;}_T1883->f1.elt_type=_T14D5;_T14D6=yyyvsp + 6;_T14D7=&(*_T14D6).v;_T14D8=(union Cyc_YYSTYPE*)_T14D7;_T1883->f1.num_elts=Cyc_yyget_Exp_tok(_T14D8);_T1883->f1.fat_result=0;_T1883->f1.inline_call=0;_T14CE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1883;}_T14D9=(void*)_T14CE;_T14DA=yyyvsp;_T14DB=_T14DA[0];_T14DC=_T14DB.l;_T14DD=_T14DC.first_line;_T14DE=
Cyc_Position_loc_to_seg(_T14DD);_T14DF=
# 3093
Cyc_Absyn_new_exp(_T14D9,_T14DE);yyval=Cyc_Exp_tok(_T14DF);goto _LL0;}case 526:
# 3097
 _T14E0=yyyvsp + 8;_T14E1=&(*_T14E0).v;_T14E2=(union Cyc_YYSTYPE*)_T14E1;_T14E3=Cyc_yyget_YY38(_T14E2);_T14E4=yyyvsp;_T14E5=_T14E4[8];_T14E6=_T14E5.l;_T14E7=_T14E6.first_line;_T14E8=Cyc_Position_loc_to_seg(_T14E7);{void*t=Cyc_Parse_type_name_to_type(_T14E3,_T14E8);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1883->tag=33;
_T1883->f1.mknd=1U;_T14EA=yyyvsp + 2;_T14EB=&(*_T14EA).v;_T14EC=(union Cyc_YYSTYPE*)_T14EB;_T1883->f1.rgn=Cyc_yyget_Exp_tok(_T14EC);_T1883->f1.aqual=0;{void**_T1884=_cycalloc(sizeof(void*));*_T1884=t;_T14ED=(void**)_T1884;}_T1883->f1.elt_type=_T14ED;_T14EE=yyyvsp + 4;_T14EF=&(*_T14EE).v;_T14F0=(union Cyc_YYSTYPE*)_T14EF;_T1883->f1.num_elts=Cyc_yyget_Exp_tok(_T14F0);_T1883->f1.fat_result=0;_T1883->f1.inline_call=0;_T14E9=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1883;}_T14F1=(void*)_T14E9;_T14F2=yyyvsp;_T14F3=_T14F2[0];_T14F4=_T14F3.l;_T14F5=_T14F4.first_line;_T14F6=
Cyc_Position_loc_to_seg(_T14F5);_T14F7=
# 3098
Cyc_Absyn_new_exp(_T14F1,_T14F6);yyval=Cyc_Exp_tok(_T14F7);goto _LL0;}case 527:{struct Cyc_Absyn_Exp*_T1883[1];_T14F9=_T1883 + 0;
# 3101 "parse.y"
_T14FA=yyyvsp + 2;_T14FB=&(*_T14FA).v;_T14FC=(union Cyc_YYSTYPE*)_T14FB;*_T14F9=Cyc_yyget_Exp_tok(_T14FC);_T14FD=_tag_fat(_T1883,sizeof(struct Cyc_Absyn_Exp*),1);_T14F8=Cyc_List_list(_T14FD);}_T14FE=yyyvsp;_T14FF=_T14FE[0];_T1500=_T14FF.l;_T1501=_T1500.first_line;_T1502=Cyc_Position_loc_to_seg(_T1501);_T1503=Cyc_Absyn_primop_exp(18U,_T14F8,_T1502);yyval=Cyc_Exp_tok(_T1503);goto _LL0;case 528:
# 3103 "parse.y"
 _T1504=yyyvsp + 2;_T1505=&(*_T1504).v;_T1506=(union Cyc_YYSTYPE*)_T1505;_T1507=Cyc_yyget_Exp_tok(_T1506);_T1508=yyyvsp;_T1509=_T1508[0];_T150A=_T1509.l;_T150B=_T150A.first_line;_T150C=Cyc_Position_loc_to_seg(_T150B);_T150D=Cyc_Absyn_tagof_exp(_T1507,_T150C);yyval=Cyc_Exp_tok(_T150D);goto _LL0;case 529:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T1883->tag=37;
# 3105 "parse.y"
_T150F=yyyvsp + 2;_T1510=&(*_T150F).v;_T1511=(union Cyc_YYSTYPE*)_T1510;_T1883->f1=Cyc_yyget_Exp_tok(_T1511);{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_T1513=yyyvsp + 4;_T1514=&(*_T1513).v;_T1515=(union Cyc_YYSTYPE*)_T1514;*_T1884=Cyc_yyget_String_tok(_T1515);_T1512=(struct _fat_ptr*)_T1884;}_T1883->f2=_T1512;_T150E=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T1883;}_T1516=(void*)_T150E;_T1517=yyyvsp;_T1518=_T1517[0];_T1519=_T1518.l;_T151A=_T1519.first_line;_T151B=Cyc_Position_loc_to_seg(_T151A);_T151C=Cyc_Absyn_new_exp(_T1516,_T151B);yyval=Cyc_Exp_tok(_T151C);goto _LL0;case 530:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T1883->tag=37;
# 3107 "parse.y"
_T151E=yyyvsp + 2;_T151F=&(*_T151E).v;_T1520=(union Cyc_YYSTYPE*)_T151F;_T1521=Cyc_yyget_Exp_tok(_T1520);_T1522=yyyvsp;_T1523=_T1522[2];_T1524=_T1523.l;_T1525=_T1524.first_line;_T1526=Cyc_Position_loc_to_seg(_T1525);_T1883->f1=Cyc_Absyn_deref_exp(_T1521,_T1526);{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_T1528=yyyvsp + 4;_T1529=&(*_T1528).v;_T152A=(union Cyc_YYSTYPE*)_T1529;*_T1884=Cyc_yyget_String_tok(_T152A);_T1527=(struct _fat_ptr*)_T1884;}_T1883->f2=_T1527;_T151D=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T1883;}_T152B=(void*)_T151D;_T152C=yyyvsp;_T152D=_T152C[0];_T152E=_T152D.l;_T152F=_T152E.first_line;_T1530=Cyc_Position_loc_to_seg(_T152F);_T1531=Cyc_Absyn_new_exp(_T152B,_T1530);yyval=Cyc_Exp_tok(_T1531);goto _LL0;case 531:
# 3109 "parse.y"
 _T1532=yyyvsp + 2;_T1533=&(*_T1532).v;_T1534=(union Cyc_YYSTYPE*)_T1533;_T1535=Cyc_yyget_YY38(_T1534);_T1536=yyyvsp;_T1537=_T1536[2];_T1538=_T1537.l;_T1539=_T1538.first_line;_T153A=Cyc_Position_loc_to_seg(_T1539);{void*t=Cyc_Parse_type_name_to_type(_T1535,_T153A);_T153B=t;_T153C=yyyvsp;_T153D=_T153C[0];_T153E=_T153D.l;_T153F=_T153E.first_line;_T1540=
Cyc_Position_loc_to_seg(_T153F);_T1541=Cyc_Absyn_valueof_exp(_T153B,_T1540);yyval=Cyc_Exp_tok(_T1541);goto _LL0;}case 532:
# 3111 "parse.y"
 _T1542=yyyvsp + 1;_T1543=&(*_T1542).v;_T1544=(union Cyc_YYSTYPE*)_T1543;_T1545=Cyc_yyget_YY63(_T1544);_T1546=yyyvsp;_T1547=_T1546[0];_T1548=_T1547.l;_T1549=_T1548.first_line;_T154A=Cyc_Position_loc_to_seg(_T1549);_T154B=Cyc_Absyn_new_exp(_T1545,_T154A);yyval=Cyc_Exp_tok(_T154B);goto _LL0;case 533:
# 3112 "parse.y"
 _T154C=yyyvsp + 1;_T154D=&(*_T154C).v;_T154E=(union Cyc_YYSTYPE*)_T154D;_T154F=Cyc_yyget_Exp_tok(_T154E);_T1550=yyyvsp;_T1551=_T1550[0];_T1552=_T1551.l;_T1553=_T1552.first_line;_T1554=Cyc_Position_loc_to_seg(_T1553);_T1555=Cyc_Absyn_extension_exp(_T154F,_T1554);yyval=Cyc_Exp_tok(_T1555);goto _LL0;case 534:
# 3113 "parse.y"
 _T1556=yyyvsp + 2;_T1557=&(*_T1556).v;_T1558=(union Cyc_YYSTYPE*)_T1557;_T1559=Cyc_yyget_Exp_tok(_T1558);_T155A=yyyvsp;_T155B=_T155A[0];_T155C=_T155B.l;_T155D=_T155C.first_line;_T155E=Cyc_Position_loc_to_seg(_T155D);_T155F=Cyc_Absyn_assert_exp(_T1559,0,_T155E);yyval=Cyc_Exp_tok(_T155F);goto _LL0;case 535:
# 3114 "parse.y"
 _T1560=yyyvsp + 2;_T1561=&(*_T1560).v;_T1562=(union Cyc_YYSTYPE*)_T1561;_T1563=Cyc_yyget_Exp_tok(_T1562);_T1564=yyyvsp;_T1565=_T1564[0];_T1566=_T1565.l;_T1567=_T1566.first_line;_T1568=Cyc_Position_loc_to_seg(_T1567);_T1569=Cyc_Absyn_assert_exp(_T1563,1,_T1568);yyval=Cyc_Exp_tok(_T1569);goto _LL0;case 536:
# 3115 "parse.y"
 _T156A=yyyvsp + 2;_T156B=&(*_T156A).v;_T156C=(union Cyc_YYSTYPE*)_T156B;_T156D=Cyc_yyget_Exp_tok(_T156C);_T156E=yyyvsp;_T156F=_T156E[0];_T1570=_T156F.l;_T1571=_T1570.first_line;_T1572=Cyc_Position_loc_to_seg(_T1571);_T1573=Cyc_Absyn_assert_false_exp(_T156D,_T1572);yyval=Cyc_Exp_tok(_T1573);goto _LL0;case 537:
# 3119 "parse.y"
 yyval=Cyc_YY6(12U);goto _LL0;case 538:
# 3120 "parse.y"
 yyval=Cyc_YY6(11U);goto _LL0;case 539:
# 3121 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 540:
# 3122 "parse.y"
 yyval=Cyc_YY6(0U);goto _LL0;case 541:
# 3127 "parse.y"
 _T1574=yyyvsp + 3;_T1575=&(*_T1574).v;_T1576=(union Cyc_YYSTYPE*)_T1575;{struct _tuple31*_T1883=Cyc_yyget_YY64(_T1576);struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;struct Cyc_List_List*_T1886;{struct _tuple31 _T1887=*_T1883;_T1886=_T1887.f0;_T1885=_T1887.f1;_T1884=_T1887.f2;}{struct Cyc_List_List*outlist=_T1886;struct Cyc_List_List*inlist=_T1885;struct Cyc_List_List*clobbers=_T1884;{struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T1887=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_T1887->tag=39;
_T1578=yyyvsp + 0;_T1579=&(*_T1578).v;_T157A=(union Cyc_YYSTYPE*)_T1579;_T1887->f1=Cyc_yyget_YY32(_T157A);_T157B=yyyvsp + 2;_T157C=&(*_T157B).v;_T157D=(union Cyc_YYSTYPE*)_T157C;_T1887->f2=Cyc_yyget_String_tok(_T157D);_T1887->f3=outlist;_T1887->f4=inlist;_T1887->f5=clobbers;_T1577=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_T1887;}_T157E=(void*)_T1577;yyval=Cyc_YY63(_T157E);goto _LL0;}}case 542:
# 3132 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 543:
# 3133 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 544:{struct _tuple31*_T1883=_cycalloc(sizeof(struct _tuple31));
# 3137 "parse.y"
_T1883->f0=0;_T1883->f1=0;_T1883->f2=0;_T157F=(struct _tuple31*)_T1883;}yyval=Cyc_YY64(_T157F);goto _LL0;case 545:
# 3139 "parse.y"
 _T1580=yyyvsp + 1;_T1581=&(*_T1580).v;_T1582=(union Cyc_YYSTYPE*)_T1581;{struct _tuple28*_T1883=Cyc_yyget_YY65(_T1582);struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;{struct _tuple28 _T1886=*_T1883;_T1885=_T1886.f0;_T1884=_T1886.f1;}{struct Cyc_List_List*inlist=_T1885;struct Cyc_List_List*clobbers=_T1884;{struct _tuple31*_T1886=_cycalloc(sizeof(struct _tuple31));
_T1886->f0=0;_T1886->f1=inlist;_T1886->f2=clobbers;_T1583=(struct _tuple31*)_T1886;}yyval=Cyc_YY64(_T1583);goto _LL0;}}case 546:
# 3142 "parse.y"
 _T1584=yyyvsp + 2;_T1585=&(*_T1584).v;_T1586=(union Cyc_YYSTYPE*)_T1585;{struct _tuple28*_T1883=Cyc_yyget_YY65(_T1586);struct Cyc_List_List*_T1884;struct Cyc_List_List*_T1885;{struct _tuple28 _T1886=*_T1883;_T1885=_T1886.f0;_T1884=_T1886.f1;}{struct Cyc_List_List*inlist=_T1885;struct Cyc_List_List*clobbers=_T1884;{struct _tuple31*_T1886=_cycalloc(sizeof(struct _tuple31));
_T1588=yyyvsp + 1;_T1589=&(*_T1588).v;_T158A=(union Cyc_YYSTYPE*)_T1589;_T158B=Cyc_yyget_YY67(_T158A);_T1886->f0=Cyc_List_imp_rev(_T158B);_T1886->f1=inlist;_T1886->f2=clobbers;_T1587=(struct _tuple31*)_T1886;}yyval=Cyc_YY64(_T1587);goto _LL0;}}case 547:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 3146
_T158D=yyyvsp + 0;_T158E=&(*_T158D).v;_T158F=(union Cyc_YYSTYPE*)_T158E;_T1883->hd=Cyc_yyget_YY68(_T158F);_T1883->tl=0;_T158C=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY67(_T158C);goto _LL0;case 548:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 3147 "parse.y"
_T1591=yyyvsp + 2;_T1592=&(*_T1591).v;_T1593=(union Cyc_YYSTYPE*)_T1592;_T1883->hd=Cyc_yyget_YY68(_T1593);_T1594=yyyvsp + 0;_T1595=&(*_T1594).v;_T1596=(union Cyc_YYSTYPE*)_T1595;_T1883->tl=Cyc_yyget_YY67(_T1596);_T1590=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY67(_T1590);goto _LL0;case 549:{struct _tuple28*_T1883=_cycalloc(sizeof(struct _tuple28));
# 3151 "parse.y"
_T1883->f0=0;_T1883->f1=0;_T1597=(struct _tuple28*)_T1883;}yyval=Cyc_YY65(_T1597);goto _LL0;case 550:{struct _tuple28*_T1883=_cycalloc(sizeof(struct _tuple28));
# 3152 "parse.y"
_T1883->f0=0;_T1599=yyyvsp + 1;_T159A=&(*_T1599).v;_T159B=(union Cyc_YYSTYPE*)_T159A;_T1883->f1=Cyc_yyget_YY66(_T159B);_T1598=(struct _tuple28*)_T1883;}yyval=Cyc_YY65(_T1598);goto _LL0;case 551:{struct _tuple28*_T1883=_cycalloc(sizeof(struct _tuple28));
# 3153 "parse.y"
_T159D=yyyvsp + 1;_T159E=&(*_T159D).v;_T159F=(union Cyc_YYSTYPE*)_T159E;_T15A0=Cyc_yyget_YY67(_T159F);_T1883->f0=Cyc_List_imp_rev(_T15A0);_T15A1=yyyvsp + 2;_T15A2=&(*_T15A1).v;_T15A3=(union Cyc_YYSTYPE*)_T15A2;_T1883->f1=Cyc_yyget_YY66(_T15A3);_T159C=(struct _tuple28*)_T1883;}yyval=Cyc_YY65(_T159C);goto _LL0;case 552:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 3156
_T15A5=yyyvsp + 0;_T15A6=&(*_T15A5).v;_T15A7=(union Cyc_YYSTYPE*)_T15A6;_T1883->hd=Cyc_yyget_YY68(_T15A7);_T1883->tl=0;_T15A4=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY67(_T15A4);goto _LL0;case 553:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 3157 "parse.y"
_T15A9=yyyvsp + 2;_T15AA=&(*_T15A9).v;_T15AB=(union Cyc_YYSTYPE*)_T15AA;_T1883->hd=Cyc_yyget_YY68(_T15AB);_T15AC=yyyvsp + 0;_T15AD=&(*_T15AC).v;_T15AE=(union Cyc_YYSTYPE*)_T15AD;_T1883->tl=Cyc_yyget_YY67(_T15AE);_T15A8=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY67(_T15A8);goto _LL0;case 554:{struct _tuple32*_T1883=_cycalloc(sizeof(struct _tuple32));
# 3161 "parse.y"
_T15B0=yyyvsp + 0;_T15B1=&(*_T15B0).v;_T15B2=(union Cyc_YYSTYPE*)_T15B1;_T1883->f0=Cyc_yyget_String_tok(_T15B2);_T15B3=yyyvsp + 2;_T15B4=&(*_T15B3).v;_T15B5=(union Cyc_YYSTYPE*)_T15B4;_T1883->f1=Cyc_yyget_Exp_tok(_T15B5);_T15AF=(struct _tuple32*)_T1883;}yyval=Cyc_YY68(_T15AF);goto _LL0;case 555:
# 3166 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 556:
# 3167 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 557:
# 3168 "parse.y"
 _T15B6=yyyvsp + 1;_T15B7=&(*_T15B6).v;_T15B8=(union Cyc_YYSTYPE*)_T15B7;_T15B9=Cyc_yyget_YY66(_T15B8);_T15BA=Cyc_List_imp_rev(_T15B9);yyval=Cyc_YY66(_T15BA);goto _LL0;case 558:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));
# 3171
_T15BD=yyyvsp + 0;_T15BE=&(*_T15BD).v;_T15BF=(union Cyc_YYSTYPE*)_T15BE;*_T1884=Cyc_yyget_String_tok(_T15BF);_T15BC=(struct _fat_ptr*)_T1884;}_T1883->hd=_T15BC;_T1883->tl=0;_T15BB=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY66(_T15BB);goto _LL0;case 559:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));
# 3172 "parse.y"
_T15C2=yyyvsp + 2;_T15C3=&(*_T15C2).v;_T15C4=(union Cyc_YYSTYPE*)_T15C3;*_T1884=Cyc_yyget_String_tok(_T15C4);_T15C1=(struct _fat_ptr*)_T1884;}_T1883->hd=_T15C1;_T15C5=yyyvsp + 0;_T15C6=&(*_T15C5).v;_T15C7=(union Cyc_YYSTYPE*)_T15C6;_T1883->tl=Cyc_yyget_YY66(_T15C7);_T15C0=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY66(_T15C0);goto _LL0;case 560: _T15C8=yyyvsp;_T15C9=_T15C8[0];
# 3177 "parse.y"
yyval=_T15C9.v;goto _LL0;case 561:
# 3179 "parse.y"
 _T15CA=yyyvsp + 0;_T15CB=&(*_T15CA).v;_T15CC=(union Cyc_YYSTYPE*)_T15CB;_T15CD=Cyc_yyget_Exp_tok(_T15CC);_T15CE=yyyvsp + 2;_T15CF=&(*_T15CE).v;_T15D0=(union Cyc_YYSTYPE*)_T15CF;_T15D1=Cyc_yyget_Exp_tok(_T15D0);_T15D2=yyyvsp;_T15D3=_T15D2[0];_T15D4=_T15D3.l;_T15D5=_T15D4.first_line;_T15D6=Cyc_Position_loc_to_seg(_T15D5);_T15D7=Cyc_Absyn_subscript_exp(_T15CD,_T15D1,_T15D6);yyval=Cyc_Exp_tok(_T15D7);goto _LL0;case 562:
# 3181 "parse.y"
 _T15D8=yyyvsp + 0;_T15D9=&(*_T15D8).v;_T15DA=(union Cyc_YYSTYPE*)_T15D9;_T15DB=Cyc_yyget_Exp_tok(_T15DA);_T15DC=yyyvsp;_T15DD=_T15DC[0];_T15DE=_T15DD.l;_T15DF=_T15DE.first_line;_T15E0=Cyc_Position_loc_to_seg(_T15DF);_T15E1=Cyc_Absyn_unknowncall_exp(_T15DB,0,_T15E0);yyval=Cyc_Exp_tok(_T15E1);goto _LL0;case 563:
# 3183 "parse.y"
 _T15E2=yyyvsp + 0;_T15E3=&(*_T15E2).v;_T15E4=(union Cyc_YYSTYPE*)_T15E3;_T15E5=Cyc_yyget_Exp_tok(_T15E4);_T15E6=yyyvsp + 2;_T15E7=&(*_T15E6).v;_T15E8=(union Cyc_YYSTYPE*)_T15E7;_T15E9=Cyc_yyget_YY4(_T15E8);_T15EA=yyyvsp;_T15EB=_T15EA[0];_T15EC=_T15EB.l;_T15ED=_T15EC.first_line;_T15EE=Cyc_Position_loc_to_seg(_T15ED);_T15EF=Cyc_Absyn_unknowncall_exp(_T15E5,_T15E9,_T15EE);yyval=Cyc_Exp_tok(_T15EF);goto _LL0;case 564:
# 3185 "parse.y"
 _T15F0=yyyvsp + 0;_T15F1=&(*_T15F0).v;_T15F2=(union Cyc_YYSTYPE*)_T15F1;_T15F3=Cyc_yyget_Exp_tok(_T15F2);{struct _fat_ptr*_T1883=_cycalloc(sizeof(struct _fat_ptr));_T15F5=yyyvsp + 2;_T15F6=&(*_T15F5).v;_T15F7=(union Cyc_YYSTYPE*)_T15F6;*_T1883=Cyc_yyget_String_tok(_T15F7);_T15F4=(struct _fat_ptr*)_T1883;}_T15F8=yyyvsp;_T15F9=_T15F8[0];_T15FA=_T15F9.l;_T15FB=_T15FA.first_line;_T15FC=Cyc_Position_loc_to_seg(_T15FB);_T15FD=Cyc_Absyn_aggrmember_exp(_T15F3,_T15F4,_T15FC);yyval=Cyc_Exp_tok(_T15FD);goto _LL0;case 565:
# 3187 "parse.y"
 _T15FE=yyyvsp + 0;_T15FF=&(*_T15FE).v;_T1600=(union Cyc_YYSTYPE*)_T15FF;_T1601=Cyc_yyget_Exp_tok(_T1600);{struct _fat_ptr*_T1883=_cycalloc(sizeof(struct _fat_ptr));_T1603=yyyvsp + 2;_T1604=&(*_T1603).v;_T1605=(union Cyc_YYSTYPE*)_T1604;*_T1883=Cyc_yyget_String_tok(_T1605);_T1602=(struct _fat_ptr*)_T1883;}_T1606=yyyvsp;_T1607=_T1606[0];_T1608=_T1607.l;_T1609=_T1608.first_line;_T160A=Cyc_Position_loc_to_seg(_T1609);_T160B=Cyc_Absyn_aggrarrow_exp(_T1601,_T1602,_T160A);yyval=Cyc_Exp_tok(_T160B);goto _LL0;case 566:
# 3189 "parse.y"
 _T160C=yyyvsp + 0;_T160D=&(*_T160C).v;_T160E=(union Cyc_YYSTYPE*)_T160D;_T160F=Cyc_yyget_Exp_tok(_T160E);_T1610=yyyvsp;_T1611=_T1610[0];_T1612=_T1611.l;_T1613=_T1612.first_line;_T1614=Cyc_Position_loc_to_seg(_T1613);_T1615=Cyc_Absyn_increment_exp(_T160F,1U,_T1614);yyval=Cyc_Exp_tok(_T1615);goto _LL0;case 567:
# 3191 "parse.y"
 _T1616=yyyvsp + 0;_T1617=&(*_T1616).v;_T1618=(union Cyc_YYSTYPE*)_T1617;_T1619=Cyc_yyget_Exp_tok(_T1618);_T161A=yyyvsp;_T161B=_T161A[0];_T161C=_T161B.l;_T161D=_T161C.first_line;_T161E=Cyc_Position_loc_to_seg(_T161D);_T161F=Cyc_Absyn_increment_exp(_T1619,3U,_T161E);yyval=Cyc_Exp_tok(_T161F);goto _LL0;case 568:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1883->tag=24;
# 3193 "parse.y"
_T1621=yyyvsp + 1;_T1622=&(*_T1621).v;_T1623=(union Cyc_YYSTYPE*)_T1622;_T1883->f1=Cyc_yyget_YY38(_T1623);_T1883->f2=0;_T1620=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1883;}_T1624=(void*)_T1620;_T1625=yyyvsp;_T1626=_T1625[0];_T1627=_T1626.l;_T1628=_T1627.first_line;_T1629=Cyc_Position_loc_to_seg(_T1628);_T162A=Cyc_Absyn_new_exp(_T1624,_T1629);yyval=Cyc_Exp_tok(_T162A);goto _LL0;case 569:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1883->tag=24;
# 3195 "parse.y"
_T162C=yyyvsp + 1;_T162D=&(*_T162C).v;_T162E=(union Cyc_YYSTYPE*)_T162D;_T1883->f1=Cyc_yyget_YY38(_T162E);_T162F=yyyvsp + 4;_T1630=&(*_T162F).v;_T1631=(union Cyc_YYSTYPE*)_T1630;_T1632=Cyc_yyget_YY5(_T1631);_T1883->f2=Cyc_List_imp_rev(_T1632);_T162B=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1883;}_T1633=(void*)_T162B;_T1634=yyyvsp;_T1635=_T1634[0];_T1636=_T1635.l;_T1637=_T1636.first_line;_T1638=Cyc_Position_loc_to_seg(_T1637);_T1639=Cyc_Absyn_new_exp(_T1633,_T1638);yyval=Cyc_Exp_tok(_T1639);goto _LL0;case 570:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1883->tag=24;
# 3197 "parse.y"
_T163B=yyyvsp + 1;_T163C=&(*_T163B).v;_T163D=(union Cyc_YYSTYPE*)_T163C;_T1883->f1=Cyc_yyget_YY38(_T163D);_T163E=yyyvsp + 4;_T163F=&(*_T163E).v;_T1640=(union Cyc_YYSTYPE*)_T163F;_T1641=Cyc_yyget_YY5(_T1640);_T1883->f2=Cyc_List_imp_rev(_T1641);_T163A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1883;}_T1642=(void*)_T163A;_T1643=yyyvsp;_T1644=_T1643[0];_T1645=_T1644.l;_T1646=_T1645.first_line;_T1647=Cyc_Position_loc_to_seg(_T1646);_T1648=Cyc_Absyn_new_exp(_T1642,_T1647);yyval=Cyc_Exp_tok(_T1648);goto _LL0;case 571:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1884->tag=0;{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));
# 3202 "parse.y"
_T164C=yyyvsp + 0;_T164D=&(*_T164C).v;_T164E=(union Cyc_YYSTYPE*)_T164D;*_T1885=Cyc_yyget_String_tok(_T164E);_T164B=(struct _fat_ptr*)_T1885;}_T1884->f1=_T164B;_T164A=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1884;}_T1883->hd=(void*)_T164A;_T1883->tl=0;_T1649=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY3(_T1649);goto _LL0;case 572: _T164F=yyyvsp;_T1650=_T164F[0];_T1651=_T1650.l;_T1652=_T1651.first_line;_T1653=
# 3205
Cyc_Position_loc_to_seg(_T1652);_T1654=yyyvsp + 0;_T1655=&(*_T1654).v;_T1656=(union Cyc_YYSTYPE*)_T1655;_T1657=Cyc_yyget_Int_tok(_T1656);{unsigned i=Cyc_Parse_cnst2uint(_T1653,_T1657);_T1658=i;_T1659=(int)_T1658;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1883=Cyc_Absyn_tuple_field_designator(_T1659);struct _fat_ptr*_T1884;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1885=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1883;_T1884=_T1885->f1;}{struct _fat_ptr*f=_T1884;{struct Cyc_List_List*_T1885=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1886->tag=0;
_T1886->f1=f;_T165B=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1886;}_T1885->hd=(void*)_T165B;_T1885->tl=0;_T165A=(struct Cyc_List_List*)_T1885;}yyval=Cyc_YY3(_T165A);goto _LL0;}}}case 573:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1884=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1884->tag=0;{struct _fat_ptr*_T1885=_cycalloc(sizeof(struct _fat_ptr));
# 3209 "parse.y"
_T165F=yyyvsp + 2;_T1660=&(*_T165F).v;_T1661=(union Cyc_YYSTYPE*)_T1660;*_T1885=Cyc_yyget_String_tok(_T1661);_T165E=(struct _fat_ptr*)_T1885;}_T1884->f1=_T165E;_T165D=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1884;}_T1883->hd=(void*)_T165D;_T1662=yyyvsp + 0;_T1663=&(*_T1662).v;_T1664=(union Cyc_YYSTYPE*)_T1663;_T1883->tl=Cyc_yyget_YY3(_T1664);_T165C=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY3(_T165C);goto _LL0;case 574: _T1665=yyyvsp;_T1666=_T1665[2];_T1667=_T1666.l;_T1668=_T1667.first_line;_T1669=
# 3212
Cyc_Position_loc_to_seg(_T1668);_T166A=yyyvsp + 2;_T166B=&(*_T166A).v;_T166C=(union Cyc_YYSTYPE*)_T166B;_T166D=Cyc_yyget_Int_tok(_T166C);{unsigned i=Cyc_Parse_cnst2uint(_T1669,_T166D);_T166E=i;_T166F=(int)_T166E;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1883=Cyc_Absyn_tuple_field_designator(_T166F);struct _fat_ptr*_T1884;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1885=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1883;_T1884=_T1885->f1;}{struct _fat_ptr*f=_T1884;{struct Cyc_List_List*_T1885=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1886=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1886->tag=0;
_T1886->f1=f;_T1671=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1886;}_T1885->hd=(void*)_T1671;_T1672=yyyvsp + 0;_T1673=&(*_T1672).v;_T1674=(union Cyc_YYSTYPE*)_T1673;_T1885->tl=Cyc_yyget_YY3(_T1674);_T1670=(struct Cyc_List_List*)_T1885;}yyval=Cyc_YY3(_T1670);goto _LL0;}}}case 575:
# 3219 "parse.y"
 _T1675=yyyvsp + 0;_T1676=&(*_T1675).v;_T1677=(union Cyc_YYSTYPE*)_T1676;_T1678=Cyc_yyget_QualId_tok(_T1677);_T1679=yyyvsp;_T167A=_T1679[0];_T167B=_T167A.l;_T167C=_T167B.first_line;_T167D=Cyc_Position_loc_to_seg(_T167C);_T167E=Cyc_Absyn_unknownid_exp(_T1678,_T167D);yyval=Cyc_Exp_tok(_T167E);goto _LL0;case 576:
# 3220 "parse.y"
 _T167F=yyyvsp + 2;_T1680=&(*_T167F).v;_T1681=(union Cyc_YYSTYPE*)_T1680;_T1682=Cyc_yyget_String_tok(_T1681);_T1683=yyyvsp;_T1684=_T1683[0];_T1685=_T1684.l;_T1686=_T1685.first_line;_T1687=Cyc_Position_loc_to_seg(_T1686);_T1688=Cyc_Absyn_pragma_exp(_T1682,_T1687);yyval=Cyc_Exp_tok(_T1688);goto _LL0;case 577: _T1689=yyyvsp;_T168A=_T1689[0];
# 3221 "parse.y"
yyval=_T168A.v;goto _LL0;case 578:
# 3222 "parse.y"
 _T168B=yyyvsp + 0;_T168C=&(*_T168B).v;_T168D=(union Cyc_YYSTYPE*)_T168C;_T168E=Cyc_yyget_String_tok(_T168D);_T168F=yyyvsp;_T1690=_T168F[0];_T1691=_T1690.l;_T1692=_T1691.first_line;_T1693=Cyc_Position_loc_to_seg(_T1692);_T1694=Cyc_Absyn_string_exp(_T168E,_T1693);yyval=Cyc_Exp_tok(_T1694);goto _LL0;case 579:
# 3223 "parse.y"
 _T1695=yyyvsp + 0;_T1696=&(*_T1695).v;_T1697=(union Cyc_YYSTYPE*)_T1696;_T1698=Cyc_yyget_String_tok(_T1697);_T1699=yyyvsp;_T169A=_T1699[0];_T169B=_T169A.l;_T169C=_T169B.first_line;_T169D=Cyc_Position_loc_to_seg(_T169C);_T169E=Cyc_Absyn_wstring_exp(_T1698,_T169D);yyval=Cyc_Exp_tok(_T169E);goto _LL0;case 580: _T169F=yyyvsp;_T16A0=_T169F[1];
# 3224 "parse.y"
yyval=_T16A0.v;goto _LL0;case 581:
# 3228 "parse.y"
 _T16A1=yyyvsp + 0;_T16A2=&(*_T16A1).v;_T16A3=(union Cyc_YYSTYPE*)_T16A2;_T16A4=Cyc_yyget_Exp_tok(_T16A3);_T16A5=yyyvsp;_T16A6=_T16A5[0];_T16A7=_T16A6.l;_T16A8=_T16A7.first_line;_T16A9=Cyc_Position_loc_to_seg(_T16A8);_T16AA=Cyc_Absyn_noinstantiate_exp(_T16A4,_T16A9);yyval=Cyc_Exp_tok(_T16AA);goto _LL0;case 582:
# 3231
 _T16AB=yyyvsp + 0;_T16AC=&(*_T16AB).v;_T16AD=(union Cyc_YYSTYPE*)_T16AC;_T16AE=Cyc_yyget_Exp_tok(_T16AD);_T16AF=yyyvsp + 3;_T16B0=&(*_T16AF).v;_T16B1=(union Cyc_YYSTYPE*)_T16B0;_T16B2=Cyc_yyget_YY41(_T16B1);_T16B3=Cyc_List_imp_rev(_T16B2);_T16B4=yyyvsp;_T16B5=_T16B4[0];_T16B6=_T16B5.l;_T16B7=_T16B6.first_line;_T16B8=Cyc_Position_loc_to_seg(_T16B7);_T16B9=Cyc_Absyn_instantiate_exp(_T16AE,_T16B3,_T16B8);yyval=Cyc_Exp_tok(_T16B9);goto _LL0;case 583:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T1883=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T1883->tag=28;
# 3234
_T16BB=yyyvsp + 0;_T16BC=&(*_T16BB).v;_T16BD=(union Cyc_YYSTYPE*)_T16BC;_T1883->f1=Cyc_yyget_QualId_tok(_T16BD);_T16BE=yyyvsp + 2;_T16BF=&(*_T16BE).v;_T16C0=(union Cyc_YYSTYPE*)_T16BF;_T1883->f2=Cyc_yyget_YY41(_T16C0);_T16C1=yyyvsp + 3;_T16C2=&(*_T16C1).v;_T16C3=(union Cyc_YYSTYPE*)_T16C2;_T16C4=Cyc_yyget_YY5(_T16C3);_T1883->f3=Cyc_List_imp_rev(_T16C4);_T1883->f4=0;_T16BA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T1883;}_T16C5=(void*)_T16BA;_T16C6=yyyvsp;_T16C7=_T16C6[0];_T16C8=_T16C7.l;_T16C9=_T16C8.first_line;_T16CA=Cyc_Position_loc_to_seg(_T16C9);_T16CB=Cyc_Absyn_new_exp(_T16C5,_T16CA);yyval=Cyc_Exp_tok(_T16CB);goto _LL0;case 584:
# 3236 "parse.y"
 _T16CC=yyyvsp + 2;_T16CD=&(*_T16CC).v;_T16CE=(union Cyc_YYSTYPE*)_T16CD;_T16CF=Cyc_yyget_YY4(_T16CE);_T16D0=yyyvsp;_T16D1=_T16D0[0];_T16D2=_T16D1.l;_T16D3=_T16D2.first_line;_T16D4=Cyc_Position_loc_to_seg(_T16D3);_T16D5=Cyc_Absyn_tuple_exp(_T16CF,_T16D4);yyval=Cyc_Exp_tok(_T16D5);goto _LL0;case 585:
# 3238 "parse.y"
 _T16D6=yyyvsp + 2;_T16D7=&(*_T16D6).v;_T16D8=(union Cyc_YYSTYPE*)_T16D7;_T16D9=Cyc_yyget_Stmt_tok(_T16D8);_T16DA=yyyvsp;_T16DB=_T16DA[0];_T16DC=_T16DB.l;_T16DD=_T16DC.first_line;_T16DE=Cyc_Position_loc_to_seg(_T16DD);_T16DF=Cyc_Absyn_stmt_exp(_T16D9,_T16DE);yyval=Cyc_Exp_tok(_T16DF);goto _LL0;case 586:
# 3242 "parse.y"
 _T16E0=yyyvsp + 0;_T16E1=&(*_T16E0).v;_T16E2=(union Cyc_YYSTYPE*)_T16E1;_T16E3=Cyc_yyget_YY4(_T16E2);_T16E4=Cyc_List_imp_rev(_T16E3);yyval=Cyc_YY4(_T16E4);goto _LL0;case 587:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 3247 "parse.y"
_T16E6=yyyvsp + 0;_T16E7=&(*_T16E6).v;_T16E8=(union Cyc_YYSTYPE*)_T16E7;_T1883->hd=Cyc_yyget_Exp_tok(_T16E8);_T1883->tl=0;_T16E5=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY4(_T16E5);goto _LL0;case 588:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 3249 "parse.y"
_T16EA=yyyvsp + 2;_T16EB=&(*_T16EA).v;_T16EC=(union Cyc_YYSTYPE*)_T16EB;_T1883->hd=Cyc_yyget_Exp_tok(_T16EC);_T16ED=yyyvsp + 0;_T16EE=&(*_T16ED).v;_T16EF=(union Cyc_YYSTYPE*)_T16EE;_T1883->tl=Cyc_yyget_YY4(_T16EF);_T16E9=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY4(_T16E9);goto _LL0;case 589:
# 3255 "parse.y"
 _T16F0=yyyvsp + 0;_T16F1=&(*_T16F0).v;_T16F2=(union Cyc_YYSTYPE*)_T16F1;_T16F3=Cyc_yyget_Int_tok(_T16F2);_T16F4=yyyvsp;_T16F5=_T16F4[0];_T16F6=_T16F5.l;_T16F7=_T16F6.first_line;_T16F8=Cyc_Position_loc_to_seg(_T16F7);_T16F9=Cyc_Absyn_const_exp(_T16F3,_T16F8);yyval=Cyc_Exp_tok(_T16F9);goto _LL0;case 590:
# 3256 "parse.y"
 _T16FA=yyyvsp + 0;_T16FB=&(*_T16FA).v;_T16FC=(union Cyc_YYSTYPE*)_T16FB;_T16FD=Cyc_yyget_Char_tok(_T16FC);_T16FE=yyyvsp;_T16FF=_T16FE[0];_T1700=_T16FF.l;_T1701=_T1700.first_line;_T1702=Cyc_Position_loc_to_seg(_T1701);_T1703=Cyc_Absyn_char_exp(_T16FD,_T1702);yyval=Cyc_Exp_tok(_T1703);goto _LL0;case 591:
# 3257 "parse.y"
 _T1704=yyyvsp + 0;_T1705=&(*_T1704).v;_T1706=(union Cyc_YYSTYPE*)_T1705;_T1707=Cyc_yyget_String_tok(_T1706);_T1708=yyyvsp;_T1709=_T1708[0];_T170A=_T1709.l;_T170B=_T170A.first_line;_T170C=Cyc_Position_loc_to_seg(_T170B);_T170D=Cyc_Absyn_wchar_exp(_T1707,_T170C);yyval=Cyc_Exp_tok(_T170D);goto _LL0;case 592: _T170E=yyyvsp;_T170F=_T170E[0];_T1710=_T170F.l;_T1711=_T1710.first_line;_T1712=
# 3259 "parse.y"
Cyc_Position_loc_to_seg(_T1711);_T1713=Cyc_Absyn_null_exp(_T1712);yyval=Cyc_Exp_tok(_T1713);goto _LL0;case 593:
# 3261 "parse.y"
 _T1714=yyyvsp + 0;_T1715=&(*_T1714).v;_T1716=(union Cyc_YYSTYPE*)_T1715;{struct _fat_ptr f=Cyc_yyget_String_tok(_T1716);_T1717=
Cyc_strlen(f);{int l=(int)_T1717;
int i=1;
if(l <= 0)goto _TL2EB;_T1718=f;_T1719=_T1718.curr;_T171A=(const char*)_T1719;_T171B=l - 1;{
char c=_T171A[_T171B];_T171C=c;_T171D=(int)_T171C;
if(_T171D==102)goto _TL2EF;else{goto _TL2F0;}_TL2F0: _T171E=c;_T171F=(int)_T171E;if(_T171F==70)goto _TL2EF;else{goto _TL2ED;}_TL2EF: i=0;goto _TL2EE;
_TL2ED: _T1720=c;_T1721=(int)_T1720;if(_T1721==108)goto _TL2F3;else{goto _TL2F4;}_TL2F4: _T1722=c;_T1723=(int)_T1722;if(_T1723==76)goto _TL2F3;else{goto _TL2F1;}_TL2F3: i=2;goto _TL2F2;_TL2F1: _TL2F2: _TL2EE:;}goto _TL2EC;_TL2EB: _TL2EC: _T1724=f;_T1725=i;_T1726=yyyvsp;_T1727=_T1726[0];_T1728=_T1727.l;_T1729=_T1728.first_line;_T172A=
# 3269
Cyc_Position_loc_to_seg(_T1729);_T172B=Cyc_Absyn_float_exp(_T1724,_T1725,_T172A);yyval=Cyc_Exp_tok(_T172B);goto _LL0;}}case 594:{struct _tuple0*_T1883=_cycalloc(sizeof(struct _tuple0));
# 3274 "parse.y"
_T1883->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_T172E=yyyvsp + 0;_T172F=&(*_T172E).v;_T1730=(union Cyc_YYSTYPE*)_T172F;*_T1884=Cyc_yyget_String_tok(_T1730);_T172D=(struct _fat_ptr*)_T1884;}_T1883->f1=_T172D;_T172C=(struct _tuple0*)_T1883;}yyval=Cyc_QualId_tok(_T172C);goto _LL0;case 595: _T1731=yyyvsp;_T1732=_T1731[0];
# 3275 "parse.y"
yyval=_T1732.v;goto _LL0;case 596:{struct _tuple0*_T1883=_cycalloc(sizeof(struct _tuple0));
# 3278
_T1883->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1884=_cycalloc(sizeof(struct _fat_ptr));_T1735=yyyvsp + 0;_T1736=&(*_T1735).v;_T1737=(union Cyc_YYSTYPE*)_T1736;*_T1884=Cyc_yyget_String_tok(_T1737);_T1734=(struct _fat_ptr*)_T1884;}_T1883->f1=_T1734;_T1733=(struct _tuple0*)_T1883;}yyval=Cyc_QualId_tok(_T1733);goto _LL0;case 597: _T1738=yyyvsp;_T1739=_T1738[0];
# 3279 "parse.y"
yyval=_T1739.v;goto _LL0;case 598: _T173A=yyyvsp;_T173B=_T173A[0];
# 3284 "parse.y"
yyval=_T173B.v;goto _LL0;case 599: _T173C=yyyvsp;_T173D=_T173C[0];
# 3285 "parse.y"
yyval=_T173D.v;goto _LL0;case 600: _T173E=yyyvsp;_T173F=_T173E[0];
# 3288
yyval=_T173F.v;goto _LL0;case 601: _T1740=yyyvsp;_T1741=_T1740[0];
# 3289 "parse.y"
yyval=_T1741.v;goto _LL0;case 602: goto _LL0;case 603: _T1742=yylex_buf;
# 3294 "parse.y"
_T1742->lex_curr_pos=_T1742->lex_curr_pos - 1;goto _LL0;case 604:
# 3298 "parse.y"
 yyval=Cyc_YY71(0);goto _LL0;case 605:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple35*_T1884=_cycalloc(sizeof(struct _tuple35));
# 3300 "parse.y"
_T1745=yyyvsp + 1;_T1746=&(*_T1745).v;_T1747=(union Cyc_YYSTYPE*)_T1746;_T1748=Cyc_yyget_String_tok(_T1747);_T1749=yyyvsp + 3;_T174A=&(*_T1749).v;_T174B=(union Cyc_YYSTYPE*)_T174A;_T174C=Cyc_yyget_String_tok(_T174B);_T174D=_tag_fat("true",sizeof(char),5U);_T174E=Cyc_strcmp(_T174C,_T174D);_T174F=_T174E==0;_T1750=yyyvsp + 0;_T1751=&(*_T1750).v;_T1752=(union Cyc_YYSTYPE*)_T1751;_T1753=Cyc_yyget_String_tok(_T1752);_T1754=Cyc_Parse_typevar2cvar(_T1753);_T1884->f0=Cyc_Parse_assign_cvar_pos(_T1748,_T174F,_T1754);_T1755=yyyvsp + 5;_T1756=&(*_T1755).v;_T1757=(union Cyc_YYSTYPE*)_T1756;_T1884->f1=Cyc_yyget_YY72(_T1757);_T1744=(struct _tuple35*)_T1884;}_T1883->hd=_T1744;_T1758=yyyvsp + 7;_T1759=&(*_T1758).v;_T175A=(union Cyc_YYSTYPE*)_T1759;_T1883->tl=Cyc_yyget_YY71(_T175A);_T1743=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY71(_T1743);goto _LL0;case 606:
# 3304 "parse.y"
 yyval=Cyc_YY72(0);goto _LL0;case 607:
# 3305 "parse.y"
 _T175B=yyyvsp + 0;_T175C=&(*_T175B).v;_T175D=(union Cyc_YYSTYPE*)_T175C;_T175E=Cyc_yyget_YY72(_T175D);yyval=Cyc_YY72(_T175E);goto _LL0;case 608:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 3309 "parse.y"
_T1760=yyyvsp + 0;_T1761=&(*_T1760).v;_T1762=(union Cyc_YYSTYPE*)_T1761;_T1883->hd=Cyc_yyget_YY73(_T1762);_T1883->tl=0;_T175F=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY72(_T175F);goto _LL0;case 609:{struct Cyc_List_List*_T1883=_cycalloc(sizeof(struct Cyc_List_List));
# 3310 "parse.y"
_T1764=yyyvsp + 0;_T1765=&(*_T1764).v;_T1766=(union Cyc_YYSTYPE*)_T1765;_T1883->hd=Cyc_yyget_YY73(_T1766);_T1767=yyyvsp + 2;_T1768=&(*_T1767).v;_T1769=(union Cyc_YYSTYPE*)_T1768;_T1883->tl=Cyc_yyget_YY72(_T1769);_T1763=(struct Cyc_List_List*)_T1883;}yyval=Cyc_YY72(_T1763);goto _LL0;case 610:
# 3314 "parse.y"
 _T176A=yyyvsp + 0;_T176B=&(*_T176A).v;_T176C=(union Cyc_YYSTYPE*)_T176B;_T176D=Cyc_yyget_String_tok(_T176C);_T176E=Cyc_Parse_typevar2cvar(_T176D);yyval=Cyc_YY45(_T176E);goto _LL0;case 611: _T176F=yyyvsp;_T1770=_T176F[0];_T1771=_T1770.l;_T1772=_T1771.first_line;_T1773=
# 3315 "parse.y"
Cyc_Position_loc_to_seg(_T1772);_T1774=yyyvsp + 0;_T1775=&(*_T1774).v;_T1776=(union Cyc_YYSTYPE*)_T1775;_T1777=Cyc_yyget_String_tok(_T1776);_T1778=Cyc_Parse_str2type(_T1773,_T1777);yyval=Cyc_YY45(_T1778);goto _LL0;case 612:
# 3319 "parse.y"
 _T1779=yyyvsp + 0;_T177A=&(*_T1779).v;_T177B=(union Cyc_YYSTYPE*)_T177A;_T177C=Cyc_yyget_String_tok(_T177B);_T177D=yyyvsp + 3;_T177E=&(*_T177D).v;_T177F=(union Cyc_YYSTYPE*)_T177E;_T1780=Cyc_yyget_YY73(_T177F);_T1781=Cyc_BansheeIf_check_constraint(_T1780);_T1782=Cyc_BansheeIf_add_location(_T177C,_T1781);yyval=Cyc_YY73(_T1782);goto _LL0;case 613:
# 3320 "parse.y"
 _T1783=yyyvsp + 0;_T1784=&(*_T1783).v;_T1785=(union Cyc_YYSTYPE*)_T1784;_T1786=Cyc_yyget_String_tok(_T1785);_T1787=yyyvsp + 3;_T1788=&(*_T1787).v;_T1789=(union Cyc_YYSTYPE*)_T1788;_T178A=Cyc_yyget_YY70(_T1789);_T178B=yyyvsp + 5;_T178C=&(*_T178B).v;_T178D=(union Cyc_YYSTYPE*)_T178C;_T178E=Cyc_yyget_YY45(_T178D);_T178F=yyyvsp + 7;_T1790=&(*_T178F).v;_T1791=(union Cyc_YYSTYPE*)_T1790;_T1792=Cyc_yyget_YY45(_T1791);_T1793=Cyc_Parse_comparison_constraint(_T178A,_T178E,_T1792);_T1794=Cyc_BansheeIf_add_location(_T1786,_T1793);yyval=Cyc_YY73(_T1794);goto _LL0;case 614:
# 3321 "parse.y"
 _T1795=yyyvsp + 0;_T1796=&(*_T1795).v;_T1797=(union Cyc_YYSTYPE*)_T1796;_T1798=Cyc_yyget_String_tok(_T1797);_T1799=yyyvsp + 3;_T179A=&(*_T1799).v;_T179B=(union Cyc_YYSTYPE*)_T179A;_T179C=Cyc_yyget_YY45(_T179B);_T179D=yyyvsp + 5;_T179E=&(*_T179D).v;_T179F=(union Cyc_YYSTYPE*)_T179E;_T17A0=Cyc_yyget_YY45(_T179F);_T17A1=Cyc_BansheeIf_cond_equality_constraint(_T179C,_T17A0);_T17A2=Cyc_BansheeIf_add_location(_T1798,_T17A1);yyval=Cyc_YY73(_T17A2);goto _LL0;case 615:
# 3322 "parse.y"
 _T17A3=yyyvsp + 0;_T17A4=&(*_T17A3).v;_T17A5=(union Cyc_YYSTYPE*)_T17A4;_T17A6=Cyc_yyget_String_tok(_T17A5);_T17A7=yyyvsp + 3;_T17A8=&(*_T17A7).v;_T17A9=(union Cyc_YYSTYPE*)_T17A8;_T17AA=Cyc_yyget_YY45(_T17A9);_T17AB=yyyvsp + 5;_T17AC=&(*_T17AB).v;_T17AD=(union Cyc_YYSTYPE*)_T17AC;_T17AE=Cyc_yyget_YY45(_T17AD);_T17AF=Cyc_BansheeIf_equality_constraint(_T17AA,_T17AE);_T17B0=Cyc_BansheeIf_add_location(_T17A6,_T17AF);yyval=Cyc_YY73(_T17B0);goto _LL0;case 616:
# 3323 "parse.y"
 _T17B1=yyyvsp + 0;_T17B2=&(*_T17B1).v;_T17B3=(union Cyc_YYSTYPE*)_T17B2;_T17B4=Cyc_yyget_String_tok(_T17B3);_T17B5=yyyvsp + 3;_T17B6=&(*_T17B5).v;_T17B7=(union Cyc_YYSTYPE*)_T17B6;_T17B8=Cyc_yyget_YY45(_T17B7);_T17B9=yyyvsp + 5;_T17BA=&(*_T17B9).v;_T17BB=(union Cyc_YYSTYPE*)_T17BA;_T17BC=Cyc_yyget_YY45(_T17BB);_T17BD=Cyc_BansheeIf_inclusion_constraint(_T17B8,_T17BC);_T17BE=Cyc_BansheeIf_add_location(_T17B4,_T17BD);yyval=Cyc_YY73(_T17BE);goto _LL0;case 617:
# 3324 "parse.y"
 _T17BF=yyyvsp + 0;_T17C0=&(*_T17BF).v;_T17C1=(union Cyc_YYSTYPE*)_T17C0;_T17C2=Cyc_yyget_String_tok(_T17C1);_T17C3=yyyvsp + 3;_T17C4=&(*_T17C3).v;_T17C5=(union Cyc_YYSTYPE*)_T17C4;_T17C6=Cyc_yyget_YY73(_T17C5);_T17C7=yyyvsp + 5;_T17C8=&(*_T17C7).v;_T17C9=(union Cyc_YYSTYPE*)_T17C8;_T17CA=Cyc_yyget_YY73(_T17C9);_T17CB=Cyc_BansheeIf_implication_constraint(_T17C6,_T17CA);_T17CC=Cyc_BansheeIf_add_location(_T17C2,_T17CB);yyval=Cyc_YY73(_T17CC);goto _LL0;case 618:
# 3325 "parse.y"
 _T17CD=yyyvsp + 0;_T17CE=&(*_T17CD).v;_T17CF=(union Cyc_YYSTYPE*)_T17CE;_T17D0=Cyc_yyget_String_tok(_T17CF);_T17D1=yyyvsp + 3;_T17D2=&(*_T17D1).v;_T17D3=(union Cyc_YYSTYPE*)_T17D2;_T17D4=Cyc_yyget_YY70(_T17D3);_T17D5=yyyvsp + 5;_T17D6=&(*_T17D5).v;_T17D7=(union Cyc_YYSTYPE*)_T17D6;_T17D8=Cyc_yyget_YY73(_T17D7);_T17D9=yyyvsp + 7;_T17DA=&(*_T17D9).v;_T17DB=(union Cyc_YYSTYPE*)_T17DA;_T17DC=Cyc_yyget_YY73(_T17DB);_T17DD=Cyc_Parse_composite_constraint(_T17D4,_T17D8,_T17DC);_T17DE=Cyc_BansheeIf_add_location(_T17D0,_T17DD);yyval=Cyc_YY73(_T17DE);goto _LL0;case 619:
# 3328
 yyval=Cyc_YY70(0U);goto _LL0;case 620:
# 3329 "parse.y"
 yyval=Cyc_YY70(1U);goto _LL0;case 621:
# 3330 "parse.y"
 yyval=Cyc_YY70(2U);goto _LL0;case 622:
# 3331 "parse.y"
 yyval=Cyc_YY70(3U);goto _LL0;case 623:
# 3332 "parse.y"
 yyval=Cyc_YY70(4U);goto _LL0;default: goto _LL0;}_LL0: _T17DF=yylen;
# 375 "cycbison.simple"
yyvsp_offset=yyvsp_offset - _T17DF;_T17E0=yylen;
yyssp_offset=yyssp_offset - _T17E0;_T17E1=yyvs;
# 389 "cycbison.simple"
yyvsp_offset=yyvsp_offset + 1;_T17E2=yyvsp_offset;_T17E3=_check_fat_subscript(_T17E1,sizeof(struct Cyc_Yystacktype),_T17E2);_T17E4=(struct Cyc_Yystacktype*)_T17E3;(*_T17E4).v=yyval;
# 392
if(yylen!=0)goto _TL2F5;_T17E5=yyvs;_T17E6=yyvsp_offset - 1;_T17E7=
_fat_ptr_plus(_T17E5,sizeof(struct Cyc_Yystacktype),_T17E6);_T17E8=_untag_fat_ptr_check_bound(_T17E7,sizeof(struct Cyc_Yystacktype),2U);_T17E9=_check_null(_T17E8);{struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_T17E9;
_T17EA=p + 1;_T17EB=yylloc;(*_T17EA).l.first_line=_T17EB.first_line;
_T17EC=p + 1;_T17ED=yylloc;(*_T17EC).l.first_column=_T17ED.first_column;
_T17EE=p + 1;_T17EF=p;_T17F0=_T17EF[0];_T17F1=_T17F0.l;(*_T17EE).l.last_line=_T17F1.last_line;
_T17F2=p + 1;_T17F3=p;_T17F4=_T17F3[0];_T17F5=_T17F4.l;(*_T17F2).l.last_column=_T17F5.last_column;}goto _TL2F6;
# 399
_TL2F5: _T17F7=yyvs.curr;_T17F8=(struct Cyc_Yystacktype*)_T17F7;_T17F9=yyvsp_offset;_T17F6=_T17F8 + _T17F9;_T17FA=yyvs;_T17FB=yyvsp_offset + yylen;_T17FC=_T17FB - 1;_T17FD=_check_fat_subscript(_T17FA,sizeof(struct Cyc_Yystacktype),_T17FC);_T17FE=(struct Cyc_Yystacktype*)_T17FD;_T17FF=*_T17FE;_T1800=_T17FF.l;(*_T17F6).l.last_line=_T1800.last_line;
_T1802=yyvs.curr;_T1803=(struct Cyc_Yystacktype*)_T1802;_T1804=yyvsp_offset;_T1801=_T1803 + _T1804;_T1805=yyvs;_T1806=_T1805.curr;_T1807=(struct Cyc_Yystacktype*)_T1806;_T1808=yyvsp_offset + yylen;_T1809=_T1808 - 1;_T180A=_T1807[_T1809];_T180B=_T180A.l;(*_T1801).l.last_column=_T180B.last_column;_TL2F6: _T180C=Cyc_yyr1;_T180D=yyn;_T180E=_check_known_subscript_notnull(_T180C,624U,sizeof(short),_T180D);_T180F=(short*)_T180E;_T1810=*_T180F;
# 409
yyn=(int)_T1810;_T1811=Cyc_yypgoto;_T1812=yyn - 175;_T1813=_check_known_subscript_notnull(_T1811,188U,sizeof(short),_T1812);_T1814=(short*)_T1813;_T1815=*_T1814;_T1816=(int)_T1815;_T1817=yyss;_T1818=yyssp_offset;_T1819=_check_fat_subscript(_T1817,sizeof(short),_T1818);_T181A=(short*)_T1819;_T181B=*_T181A;_T181C=(int)_T181B;
# 411
yystate=_T1816 + _T181C;
if(yystate < 0)goto _TL2F7;if(yystate > 8241)goto _TL2F7;_T181D=Cyc_yycheck;_T181E=yystate;_T181F=_T181D[_T181E];_T1820=(int)_T181F;_T1821=yyss;_T1822=_T1821.curr;_T1823=(short*)_T1822;_T1824=yyssp_offset;_T1825=_T1823[_T1824];_T1826=(int)_T1825;if(_T1820!=_T1826)goto _TL2F7;_T1827=Cyc_yytable;_T1828=yystate;_T1829=_T1827[_T1828];
yystate=(int)_T1829;goto _TL2F8;
# 415
_TL2F7: _T182A=Cyc_yydefgoto;_T182B=yyn - 175;_T182C=_check_known_subscript_notnull(_T182A,188U,sizeof(short),_T182B);_T182D=(short*)_T182C;_T182E=*_T182D;yystate=(int)_T182E;_TL2F8: goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus!=0)goto _TL2F9;
# 424
yynerrs=yynerrs + 1;_T182F=Cyc_yypact;_T1830=yystate;_T1831=_check_known_subscript_notnull(_T182F,1262U,sizeof(short),_T1830);_T1832=(short*)_T1831;_T1833=*_T1832;
# 427
yyn=(int)_T1833;
# 429
if(yyn <= -32768)goto _TL2FB;if(yyn >= 8241)goto _TL2FB;{
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
if(yyn >= 0)goto _TL2FD;_T1834=- yyn;goto _TL2FE;_TL2FD: _T1834=0;_TL2FE: x=_T1834;
_TL302: _T1835=x;_T1836=(unsigned)_T1835;_T1837=363U / sizeof(char*);
# 437
if(_T1836 < _T1837)goto _TL300;else{goto _TL301;}
# 439
_TL300: _T1838=Cyc_yycheck;_T1839=x + yyn;_T183A=_check_known_subscript_notnull(_T1838,8242U,sizeof(short),_T1839);_T183B=(short*)_T183A;_T183C=*_T183B;_T183D=(int)_T183C;_T183E=x;if(_T183D!=_T183E)goto _TL303;_T183F=Cyc_yytname;_T1840=x;_T1841=_check_known_subscript_notnull(_T183F,363U,sizeof(struct _fat_ptr),_T1840);_T1842=(struct _fat_ptr*)_T1841;_T1843=*_T1842;_T1844=
Cyc_strlen(_T1843);_T1845=_T1844 + 15U;sze=sze + _T1845;count=count + 1;goto _TL304;_TL303: _TL304:
# 438
 x=x + 1;goto _TL302;_TL301: _T1847=sze + 15;_T1848=(unsigned)_T1847;{unsigned _T1883=_T1848 + 1U;_T184A=yyregion;_T184B=_check_times(_T1883,sizeof(char));{char*_T1884=_region_malloc(_T184A,0U,_T184B);_T184C=sze + 15;{unsigned _T1885=(unsigned)_T184C;unsigned i;i=0;_TL308: if(i < _T1885)goto _TL306;else{goto _TL307;}_TL306: _T184E=i;_T184D=_T1884 + _T184E;
# 441
*_T184D='\000';i=i + 1;goto _TL308;_TL307: _T184F=_T1884 + _T1885;*_T184F=0;}_T1849=(char*)_T1884;}_T1846=_tag_fat(_T1849,sizeof(char),_T1883);}msg=_T1846;_T1850=msg;_T1851=
_tag_fat("parse error",sizeof(char),12U);Cyc_strcpy(_T1850,_T1851);
# 444
if(count >= 5)goto _TL309;
# 446
count=0;
if(yyn >= 0)goto _TL30B;_T1852=- yyn;goto _TL30C;_TL30B: _T1852=0;_TL30C: x=_T1852;
_TL310: _T1853=x;_T1854=(unsigned)_T1853;_T1855=363U / sizeof(char*);
# 447
if(_T1854 < _T1855)goto _TL30E;else{goto _TL30F;}
# 449
_TL30E: _T1856=Cyc_yycheck;_T1857=x + yyn;_T1858=_check_known_subscript_notnull(_T1856,8242U,sizeof(short),_T1857);_T1859=(short*)_T1858;_T185A=*_T1859;_T185B=(int)_T185A;_T185C=x;if(_T185B!=_T185C)goto _TL311;_T185D=msg;
# 451
if(count!=0)goto _TL313;_T185F=
_tag_fat(", expecting `",sizeof(char),14U);_T185E=_T185F;goto _TL314;_TL313: _T1860=
_tag_fat(" or `",sizeof(char),6U);_T185E=_T1860;_TL314:
# 451
 Cyc_strcat(_T185D,_T185E);_T1861=msg;_T1862=Cyc_yytname;_T1863=x;_T1864=_check_known_subscript_notnull(_T1862,363U,sizeof(struct _fat_ptr),_T1863);_T1865=(struct _fat_ptr*)_T1864;_T1866=*_T1865;
# 454
Cyc_strcat(_T1861,_T1866);_T1867=msg;_T1868=
_tag_fat("'",sizeof(char),2U);Cyc_strcat(_T1867,_T1868);
count=count + 1;goto _TL312;_TL311: _TL312:
# 448
 x=x + 1;goto _TL310;_TL30F: goto _TL30A;_TL309: _TL30A:
# 459
 Cyc_yyerror(msg,yystate,yychar);}goto _TL2FC;
# 463
_TL2FB: _T1869=_tag_fat("parse error",sizeof(char),12U);_T186A=yystate;_T186B=yychar;Cyc_yyerror(_T1869,_T186A,_T186B);_TL2FC: goto _TL2FA;_TL2F9: _TL2FA: goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus!=3)goto _TL315;
# 474
if(yychar!=0)goto _TL317;{int _T1883=1;_npop_handler(0);return _T1883;}_TL317:
# 483
 yychar=-2;goto _TL316;_TL315: _TL316:
# 489
 yyerrstatus=3;goto yyerrhandle;
# 493
yyerrdefault:
# 503 "cycbison.simple"
 yyerrpop:
# 505
 if(yyssp_offset!=0)goto _TL319;{int _T1883=1;_npop_handler(0);return _T1883;}_TL319:
 yyvsp_offset=yyvsp_offset + -1;_T186C=yyss;
yyssp_offset=yyssp_offset + -1;_T186D=yyssp_offset;_T186E=_check_fat_subscript(_T186C,sizeof(short),_T186D);_T186F=(short*)_T186E;_T1870=*_T186F;yystate=(int)_T1870;
# 521 "cycbison.simple"
yyerrhandle: _T1871=Cyc_yypact;_T1872=yystate;_T1873=_check_known_subscript_notnull(_T1871,1262U,sizeof(short),_T1872);_T1874=(short*)_T1873;_T1875=*_T1874;
yyn=(int)_T1875;
if(yyn!=-32768)goto _TL31B;goto yyerrdefault;_TL31B:
# 525
 yyn=yyn + 1;
if(yyn < 0)goto _TL31F;else{goto _TL321;}_TL321: if(yyn > 8241)goto _TL31F;else{goto _TL320;}_TL320: _T1876=Cyc_yycheck;_T1877=yyn;_T1878=_T1876[_T1877];_T1879=(int)_T1878;if(_T1879!=1)goto _TL31F;else{goto _TL31D;}_TL31F: goto yyerrdefault;_TL31D: _T187A=Cyc_yytable;_T187B=yyn;_T187C=_T187A[_T187B];
# 528
yyn=(int)_T187C;
if(yyn >= 0)goto _TL322;
# 531
if(yyn!=-32768)goto _TL324;goto yyerrpop;_TL324:
 yyn=- yyn;goto yyreduce;
# 535
_TL322: if(yyn!=0)goto _TL326;goto yyerrpop;_TL326:
# 537
 if(yyn!=1261)goto _TL328;{int _T1883=0;_npop_handler(0);return _T1883;}_TL328: _T187D=yyvs;
# 546
yyvsp_offset=yyvsp_offset + 1;_T187E=yyvsp_offset;_T187F=_check_fat_subscript(_T187D,sizeof(struct Cyc_Yystacktype),_T187E);_T1880=(struct Cyc_Yystacktype*)_T187F;{struct Cyc_Yystacktype _T1883;_T1883.v=yylval;_T1883.l=yylloc;_T1881=_T1883;}*_T1880=_T1881;goto yynewstate;}}}}_pop_region();}
# 3336 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){union Cyc_YYSTYPE _T0;struct _union_YYSTYPE_Stmt_tok _T1;unsigned _T2;union Cyc_YYSTYPE _T3;struct _union_YYSTYPE_Int_tok _T4;struct Cyc_String_pa_PrintArg_struct _T5;void**_T6;struct Cyc___cycFILE*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;union Cyc_YYSTYPE _TA;struct _union_YYSTYPE_Char_tok _TB;struct Cyc_Int_pa_PrintArg_struct _TC;char _TD;int _TE;void**_TF;struct Cyc___cycFILE*_T10;struct _fat_ptr _T11;struct _fat_ptr _T12;union Cyc_YYSTYPE _T13;struct _union_YYSTYPE_String_tok _T14;struct Cyc_String_pa_PrintArg_struct _T15;void**_T16;struct Cyc___cycFILE*_T17;struct _fat_ptr _T18;struct _fat_ptr _T19;union Cyc_YYSTYPE _T1A;struct _union_YYSTYPE_QualId_tok _T1B;struct Cyc_String_pa_PrintArg_struct _T1C;void**_T1D;struct Cyc___cycFILE*_T1E;struct _fat_ptr _T1F;struct _fat_ptr _T20;union Cyc_YYSTYPE _T21;struct _union_YYSTYPE_Exp_tok _T22;struct Cyc_String_pa_PrintArg_struct _T23;void**_T24;struct Cyc___cycFILE*_T25;struct _fat_ptr _T26;struct _fat_ptr _T27;union Cyc_YYSTYPE _T28;struct _union_YYSTYPE_Stmt_tok _T29;struct Cyc_String_pa_PrintArg_struct _T2A;void**_T2B;struct Cyc___cycFILE*_T2C;struct _fat_ptr _T2D;struct _fat_ptr _T2E;struct Cyc___cycFILE*_T2F;struct _fat_ptr _T30;struct _fat_ptr _T31;struct Cyc_Absyn_Stmt*_T32;struct Cyc_Absyn_Exp*_T33;struct _tuple0*_T34;struct _fat_ptr _T35;char _T36;union Cyc_Absyn_Cnst _T37;_T0=v;_T1=_T0.Stmt_tok;_T2=_T1.tag;switch(_T2){case 2: _T3=v;_T4=_T3.Int_tok;_T37=_T4.val;{union Cyc_Absyn_Cnst c=_T37;{struct Cyc_String_pa_PrintArg_struct _T38;_T38.tag=0;
# 3338
_T38.f1=Cyc_Absynpp_cnst2string(c);_T5=_T38;}{struct Cyc_String_pa_PrintArg_struct _T38=_T5;void*_T39[1];_T6=_T39 + 0;*_T6=& _T38;_T7=Cyc_stderr;_T8=_tag_fat("%s",sizeof(char),3U);_T9=_tag_fat(_T39,sizeof(void*),1);Cyc_fprintf(_T7,_T8,_T9);}goto _LL0;}case 3: _TA=v;_TB=_TA.Char_tok;_T36=_TB.val;{char c=_T36;{struct Cyc_Int_pa_PrintArg_struct _T38;_T38.tag=1;_TD=c;_TE=(int)_TD;
_T38.f1=(unsigned long)_TE;_TC=_T38;}{struct Cyc_Int_pa_PrintArg_struct _T38=_TC;void*_T39[1];_TF=_T39 + 0;*_TF=& _T38;_T10=Cyc_stderr;_T11=_tag_fat("%c",sizeof(char),3U);_T12=_tag_fat(_T39,sizeof(void*),1);Cyc_fprintf(_T10,_T11,_T12);}goto _LL0;}case 4: _T13=v;_T14=_T13.String_tok;_T35=_T14.val;{struct _fat_ptr s=_T35;{struct Cyc_String_pa_PrintArg_struct _T38;_T38.tag=0;
_T38.f1=s;_T15=_T38;}{struct Cyc_String_pa_PrintArg_struct _T38=_T15;void*_T39[1];_T16=_T39 + 0;*_T16=& _T38;_T17=Cyc_stderr;_T18=_tag_fat("\"%s\"",sizeof(char),5U);_T19=_tag_fat(_T39,sizeof(void*),1);Cyc_fprintf(_T17,_T18,_T19);}goto _LL0;}case 5: _T1A=v;_T1B=_T1A.QualId_tok;_T34=_T1B.val;{struct _tuple0*q=_T34;{struct Cyc_String_pa_PrintArg_struct _T38;_T38.tag=0;
_T38.f1=Cyc_Absynpp_qvar2string(q);_T1C=_T38;}{struct Cyc_String_pa_PrintArg_struct _T38=_T1C;void*_T39[1];_T1D=_T39 + 0;*_T1D=& _T38;_T1E=Cyc_stderr;_T1F=_tag_fat("%s",sizeof(char),3U);_T20=_tag_fat(_T39,sizeof(void*),1);Cyc_fprintf(_T1E,_T1F,_T20);}goto _LL0;}case 6: _T21=v;_T22=_T21.Exp_tok;_T33=_T22.val;{struct Cyc_Absyn_Exp*e=_T33;{struct Cyc_String_pa_PrintArg_struct _T38;_T38.tag=0;
_T38.f1=Cyc_Absynpp_exp2string(e);_T23=_T38;}{struct Cyc_String_pa_PrintArg_struct _T38=_T23;void*_T39[1];_T24=_T39 + 0;*_T24=& _T38;_T25=Cyc_stderr;_T26=_tag_fat("%s",sizeof(char),3U);_T27=_tag_fat(_T39,sizeof(void*),1);Cyc_fprintf(_T25,_T26,_T27);}goto _LL0;}case 7: _T28=v;_T29=_T28.Stmt_tok;_T32=_T29.val;{struct Cyc_Absyn_Stmt*s=_T32;{struct Cyc_String_pa_PrintArg_struct _T38;_T38.tag=0;
_T38.f1=Cyc_Absynpp_stmt2string(s);_T2A=_T38;}{struct Cyc_String_pa_PrintArg_struct _T38=_T2A;void*_T39[1];_T2B=_T39 + 0;*_T2B=& _T38;_T2C=Cyc_stderr;_T2D=_tag_fat("%s",sizeof(char),3U);_T2E=_tag_fat(_T39,sizeof(void*),1);Cyc_fprintf(_T2C,_T2D,_T2E);}goto _LL0;}default: _T2F=Cyc_stderr;_T30=
_tag_fat("?",sizeof(char),2U);_T31=_tag_fat(0U,sizeof(void*),0);Cyc_fprintf(_T2F,_T30,_T31);goto _LL0;}_LL0:;}
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

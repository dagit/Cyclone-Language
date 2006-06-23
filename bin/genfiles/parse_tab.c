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
_T69->name=nm;_T69->identity=- 1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T6A=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T6A->tag=0;_T2A=& Cyc_Kinds_ik;_T6A->f1=(struct Cyc_Absyn_Kind*)_T2A;_T29=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T6A;}_T69->kind=(void*)_T29;_T69->aquals_bound=0;_T28=(struct Cyc_Absyn_Tvar*)_T69;}*_T27=Cyc_Absyn_var_type(_T28);goto _LL7;}_TL2D: goto _LL7;_LL7:;}{struct Cyc_List_List*_T68=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple18*_T69=_cycalloc(sizeof(struct _tuple18));
# 335
_T69->f0=v;_T69->f1=i;_T2C=(struct _tuple18*)_T69;}_T68->hd=_T2C;_T68->tl=res;_T2B=(struct Cyc_List_List*)_T68;}res=_T2B;goto _LL0;}_TL2B: _T2D=(struct _tuple8*)_T64;_T2E=_T2D->f0;if(_T2E==0)goto _TL2F;goto _LL5;_TL2F: goto _LL5;_TL29: _T2F=(struct _tuple8*)_T64;_T30=_T2F->f0;if(_T30==0)goto _TL31;goto _LL5;_TL31: goto _LL5;_TL27: _T31=(struct _tuple8*)_T64;_T32=_T31->f0;if(_T32==0)goto _TL33;goto _LL5;_TL33: goto _LL5;_TL25: _T33=(struct _tuple8*)_T64;_T34=_T33->f0;if(_T34==0)goto _TL35;_T35=(struct _tuple8*)_T64;_T36=_T35->f2;_T37=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T36;_T38=_T37->f1;_T39=(int*)_T38;_T3A=*_T39;if(_T3A!=4)goto _TL37;_T3B=(struct _tuple8*)_T64;_T3C=_T3B->f2;_T3D=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3C;_T3E=_T3D->f2;if(_T3E==0)goto _TL39;_T3F=(struct _tuple8*)_T64;_T40=_T3F->f2;_T41=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T40;_T42=_T41->f2;_T43=(struct Cyc_List_List*)_T42;_T44=_T43->hd;_T45=(int*)_T44;_T46=*_T45;if(_T46!=1)goto _TL3B;_T47=(struct _tuple8*)_T64;_T48=_T47->f2;_T49=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T48;_T4A=_T49->f2;_T4B=(struct Cyc_List_List*)_T4A;_T4C=_T4B->tl;if(_T4C!=0)goto _TL3D;{struct _tuple8 _T68=*_T64;_T4D=_T68.f0;{struct _fat_ptr _T69=*_T4D;_T65=_T69;}_T4E=_T68.f2;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T69=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4E;_T4F=_T69->f2;{struct Cyc_List_List _T6A=*_T4F;_T50=_T6A.hd;{struct Cyc_Absyn_Evar_Absyn_Type_struct*_T6B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T50;_T51=_T64->f2;_T52=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T51;_T53=_T52->f2;_T54=_T53->hd;_T55=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T54;_T56=& _T55->f2;_T67=(void**)_T56;}}}}{struct _fat_ptr v=_T65;void**z=(void**)_T67;
# 339
struct _fat_ptr*nm;nm=_cycalloc(sizeof(struct _fat_ptr));_T57=nm;{struct Cyc_String_pa_PrintArg_struct _T68;_T68.tag=0;_T68.f1=v;_T59=_T68;}{struct Cyc_String_pa_PrintArg_struct _T68=_T59;void*_T69[1];_T69[0]=& _T68;_T5A=_tag_fat("`%s",sizeof(char),4U);_T5B=_tag_fat(_T69,sizeof(void*),1);_T58=Cyc_aprintf(_T5A,_T5B);}*_T57=_T58;_T5C=z;{struct Cyc_Absyn_Tvar*_T68=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));
_T68->name=nm;_T68->identity=- 1;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T69=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T69->tag=0;_T5F=& Cyc_Kinds_ek;_T69->f1=(struct Cyc_Absyn_Kind*)_T5F;_T5E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T69;}_T68->kind=(void*)_T5E;_T68->aquals_bound=0;_T5D=(struct Cyc_Absyn_Tvar*)_T68;}*_T5C=Cyc_Absyn_var_type(_T5D);goto _LL0;}_TL3D: goto _LL5;_TL3B: goto _LL5;_TL39: goto _LL5;_TL37: goto _LL5;_TL35: goto _LL5;_TL23: _T60=(struct _tuple8*)_T64;_T61=_T60->f0;if(_T61==0)goto _TL3F;goto _LL5;_TL3F: _LL5: goto _LL0;_LL0:;}_T62=x;
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
*_T12=s;_TF=(struct _fat_ptr*)_T12;}_T11->name=_TF;_T11->identity=- 1;_T11->kind=k;_T11->aquals_bound=aliashint;_TE=(struct Cyc_Absyn_Tvar*)_T11;}_T10=Cyc_Absyn_var_type(_TE);return _T10;}
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
static struct _fat_ptr Cyc_Parse_msg1={(unsigned char*)_TmpG0,(unsigned char*)_TmpG0,(unsigned char*)_TmpG0 + 76U};static char _TmpG1[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 625
static struct _fat_ptr Cyc_Parse_msg2={(unsigned char*)_TmpG1,(unsigned char*)_TmpG1,(unsigned char*)_TmpG1 + 84U};
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
static void*Cyc_Parse_assign_cvar_pos(struct _fat_ptr posstr,int ovfat,void*cv){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T5;const char**_T6;void*_T7;struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T8;int*_T9;const char**_TA;struct _fat_ptr _TB;unsigned char*_TC;int*_TD;void*_TE;void*_TF;void*_T10;void*_T11;_T0=cv;_T1=(int*)_T0;_T2=*_T1;if(_T2!=3)goto _TL196;_T3=cv;{struct Cyc_Absyn_Cvar_Absyn_Type_struct*_T12=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T3;_T4=cv;_T5=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T4;_T6=& _T5->f6;_T11=(const char**)_T6;_T7=cv;_T8=(struct Cyc_Absyn_Cvar_Absyn_Type_struct*)_T7;_T9=& _T8->f7;_T10=(int*)_T9;}{const char**pos=(const char**)_T11;int*ov=(int*)_T10;_TA=pos;_TB=posstr;_TC=_untag_fat_ptr_check_bound(_TB,sizeof(char),1U);
# 1100
*_TA=(const char*)_TC;_TD=ov;
*_TD=ovfat;_TE=cv;
return _TE;}_TL196: _TF=cv;
# 1104
return _TF;;}
# 1108
static void*Cyc_Parse_typevar2cvar(struct _fat_ptr s){struct Cyc_Hashtable_Table*_T0;unsigned _T1;struct Cyc_Hashtable_Table*(*_T2)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T3)(int,int(*)(void*,void*),int(*)(void*));int(*_T4)(struct _fat_ptr*,struct _fat_ptr*);int(*_T5)(struct _fat_ptr*);struct _handler_cons*_T6;int _T7;void*(*_T8)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_T9)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TA;struct _fat_ptr*_TB;void*_TC;struct Cyc_Core_Not_found_exn_struct*_TD;char*_TE;char*_TF;struct _fat_ptr _T10;struct _fat_ptr _T11;struct _fat_ptr*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;int _T15;struct Cyc_Core_Opt*_T16;struct Cyc_Core_Opt*_T17;struct _fat_ptr _T18;void(*_T19)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*);void(*_T1A)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T1B;struct _fat_ptr*_T1C;void*_T1D;void*_T1E;int(*_T1F)(unsigned,struct _fat_ptr);struct _fat_ptr _T20;
# 1111
static struct Cyc_Hashtable_Table*cvmap=0;_T0=cvmap;_T1=(unsigned)_T0;
if(_T1)goto _TL198;else{goto _TL19A;}
_TL19A: _T3=Cyc_Hashtable_create;{struct Cyc_Hashtable_Table*(*_T21)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T3;_T2=_T21;}_T4=Cyc_strptrcmp;_T5=Cyc_Hashtable_hash_stringptr;cvmap=_T2(101,_T4,_T5);goto _TL199;_TL198: _TL199: {struct _handler_cons _T21;_T6=& _T21;_push_handler(_T6);{int _T22=0;_T7=setjmp(_T21.handler);if(!_T7)goto _TL19B;_T22=1;goto _TL19C;_TL19B: _TL19C: if(_T22)goto _TL19D;else{goto _TL19F;}_TL19F: _T9=Cyc_Hashtable_lookup;{
# 1115
void*(*_T23)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(void*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_T9;_T8=_T23;}_TA=cvmap;{struct _fat_ptr*_T23=_cycalloc(sizeof(struct _fat_ptr));*_T23=s;_TB=(struct _fat_ptr*)_T23;}{void*_T23=_T8(_TA,_TB);_npop_handler(0);return _T23;}_pop_handler();goto _TL19E;_TL19D: _TC=Cyc_Core_get_exn_thrown();{void*_T23=(void*)_TC;void*_T24;_TD=(struct Cyc_Core_Not_found_exn_struct*)_T23;_TE=_TD->tag;_TF=Cyc_Core_Not_found;if(_TE!=_TF)goto _TL1A0;{
# 1118
struct _fat_ptr kind=Cyc_strchr(s,'_');_T10=kind;_T11=
_fat_ptr_plus(_T10,sizeof(char),1);{struct _fat_ptr name=Cyc_strchr(_T11,'_');_T12=& name;
_fat_ptr_inplace_plus(_T12,sizeof(char),1);_T13=kind;_T14=
_tag_fat("_PTRBND",sizeof(char),8U);_T15=Cyc_strncmp(_T13,_T14,7U);if(_T15)goto _TL1A2;else{goto _TL1A4;}
_TL1A4: _T16=& Cyc_Kinds_ptrbko;_T17=(struct Cyc_Core_Opt*)_T16;_T18=name;{void*t=Cyc_Absyn_cvar_type_name(_T17,_T18);_T1A=Cyc_Hashtable_insert;{
void(*_T25)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,void*))_T1A;_T19=_T25;}_T1B=_check_null(cvmap);{struct _fat_ptr*_T25=_cycalloc(sizeof(struct _fat_ptr));*_T25=s;_T1C=(struct _fat_ptr*)_T25;}_T1D=t;_T19(_T1B,_T1C,_T1D);_T1E=t;
return _T1E;}
# 1127
_TL1A2:{int(*_T25)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T1F=_T25;}_T20=_tag_fat("Constraint variable unknown kind",sizeof(char),33U);_T1F(0U,_T20);;}}goto _TL1A1;_TL1A0: _T24=_T23;{void*exn=_T24;_rethrow(exn);}_TL1A1:;}_TL19E:;}}}
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
static struct _fat_ptr s={(unsigned char*)_TmpG2,(unsigned char*)_TmpG2,(unsigned char*)_TmpG2 + 7U};union Cyc_Absyn_Cnst _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Int_tok;_T3=_T2.tag;if(_T3!=2)goto _TL1AD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Int_tok;_T7=_T5.val;}{union Cyc_Absyn_Cnst yy=_T7;_T6=yy;
# 1217
return _T6;}_TL1AD:
 Cyc_yythrowfail(s);;}
# 1221
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Int_tok.tag=2U;_T1.Int_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3[5U]="char";
# 1215 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Char_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Char_tok _T5;char _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3,(unsigned char*)_TmpG3,(unsigned char*)_TmpG3 + 5U};char _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Char_tok;_T3=_T2.tag;if(_T3!=3)goto _TL1AF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Char_tok;_T7=_T5.val;}{char yy=_T7;_T6=yy;
# 1218
return _T6;}_TL1AF:
 Cyc_yythrowfail(s);;}
# 1222
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Char_tok.tag=3U;_T1.Char_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4[13U]="string_t<`H>";
# 1216 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_String_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_String_tok _T5;struct _fat_ptr _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4,(unsigned char*)_TmpG4,(unsigned char*)_TmpG4 + 13U};struct _fat_ptr _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->String_tok;_T3=_T2.tag;if(_T3!=4)goto _TL1B1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.String_tok;_T7=_T5.val;}{struct _fat_ptr yy=_T7;_T6=yy;
# 1219
return _T6;}_TL1B1:
 Cyc_yythrowfail(s);;}
# 1223
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.String_tok.tag=4U;_T1.String_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG5[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1219 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY1 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY1 _T5;struct _tuple22*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG5,(unsigned char*)_TmpG5,(unsigned char*)_TmpG5 + 35U};struct _tuple22*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY1;_T3=_T2.tag;if(_T3!=8)goto _TL1B3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY1;_T7=_T5.val;}{struct _tuple22*yy=_T7;_T6=yy;
# 1222
return _T6;}_TL1B3:
 Cyc_yythrowfail(s);;}
# 1226
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY1.tag=8U;_T1.YY1.val=yy1;_T0=_T1;}return _T0;}static char _TmpG6[11U]="ptrbound_t";
# 1220 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY2 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY2 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG6,(unsigned char*)_TmpG6,(unsigned char*)_TmpG6 + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY2;_T3=_T2.tag;if(_T3!=9)goto _TL1B5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY2;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1223
return _T6;}_TL1B5:
 Cyc_yythrowfail(s);;}
# 1227
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY2.tag=9U;_T1.YY2.val=yy1;_T0=_T1;}return _T0;}static char _TmpG7[28U]="list_t<offsetof_field_t,`H>";
# 1221 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY3 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY3 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG7,(unsigned char*)_TmpG7,(unsigned char*)_TmpG7 + 28U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY3;_T3=_T2.tag;if(_T3!=10)goto _TL1B7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY3;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1224
return _T6;}_TL1B7:
 Cyc_yythrowfail(s);;}
# 1228
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY3.tag=10U;_T1.YY3.val=yy1;_T0=_T1;}return _T0;}static char _TmpG8[6U]="exp_t";
# 1222 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Exp_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Exp_tok _T5;struct Cyc_Absyn_Exp*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG8,(unsigned char*)_TmpG8,(unsigned char*)_TmpG8 + 6U};struct Cyc_Absyn_Exp*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Exp_tok;_T3=_T2.tag;if(_T3!=6)goto _TL1B9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Exp_tok;_T7=_T5.val;}{struct Cyc_Absyn_Exp*yy=_T7;_T6=yy;
# 1225
return _T6;}_TL1B9:
 Cyc_yythrowfail(s);;}
# 1229
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Exp_tok.tag=6U;_T1.Exp_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpG9[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY4 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY4 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG9,(unsigned char*)_TmpG9,(unsigned char*)_TmpG9 + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY4;_T3=_T2.tag;if(_T3!=11)goto _TL1BB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY4;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1233
return _T6;}_TL1BB:
 Cyc_yythrowfail(s);;}
# 1237
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY4.tag=11U;_T1.YY4.val=yy1;_T0=_T1;}return _T0;}static char _TmpGA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1231 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY5 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY5 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGA,(unsigned char*)_TmpGA,(unsigned char*)_TmpGA + 47U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY5;_T3=_T2.tag;if(_T3!=12)goto _TL1BD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY5;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1234
return _T6;}_TL1BD:
 Cyc_yythrowfail(s);;}
# 1238
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY5.tag=12U;_T1.YY5.val=yy1;_T0=_T1;}return _T0;}static char _TmpGB[9U]="primop_t";
# 1232 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY6 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY6 _T5;enum Cyc_Absyn_Primop _T6;
static struct _fat_ptr s={(unsigned char*)_TmpGB,(unsigned char*)_TmpGB,(unsigned char*)_TmpGB + 9U};enum Cyc_Absyn_Primop _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY6;_T3=_T2.tag;if(_T3!=13)goto _TL1BF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY6;_T7=_T5.val;}{enum Cyc_Absyn_Primop yy=_T7;_T6=yy;
# 1235
return _T6;}_TL1BF:
 Cyc_yythrowfail(s);;}
# 1239
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY6.tag=13U;_T1.YY6.val=yy1;_T0=_T1;}return _T0;}static char _TmpGC[19U]="opt_t<primop_t,`H>";
# 1233 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY7 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY7 _T5;struct Cyc_Core_Opt*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGC,(unsigned char*)_TmpGC,(unsigned char*)_TmpGC + 19U};struct Cyc_Core_Opt*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY7;_T3=_T2.tag;if(_T3!=14)goto _TL1C1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY7;_T7=_T5.val;}{struct Cyc_Core_Opt*yy=_T7;_T6=yy;
# 1236
return _T6;}_TL1C1:
 Cyc_yythrowfail(s);;}
# 1240
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY7.tag=14U;_T1.YY7.val=yy1;_T0=_T1;}return _T0;}static char _TmpGD[7U]="qvar_t";
# 1234 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_QualId_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_QualId_tok _T5;struct _tuple0*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGD,(unsigned char*)_TmpGD,(unsigned char*)_TmpGD + 7U};struct _tuple0*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->QualId_tok;_T3=_T2.tag;if(_T3!=5)goto _TL1C3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.QualId_tok;_T7=_T5.val;}{struct _tuple0*yy=_T7;_T6=yy;
# 1237
return _T6;}_TL1C3:
 Cyc_yythrowfail(s);;}
# 1241
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.QualId_tok.tag=5U;_T1.QualId_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpGE[7U]="stmt_t";
# 1237 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_Stmt_tok _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_Stmt_tok _T5;struct Cyc_Absyn_Stmt*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGE,(unsigned char*)_TmpGE,(unsigned char*)_TmpGE + 7U};struct Cyc_Absyn_Stmt*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->Stmt_tok;_T3=_T2.tag;if(_T3!=7)goto _TL1C5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.Stmt_tok;_T7=_T5.val;}{struct Cyc_Absyn_Stmt*yy=_T7;_T6=yy;
# 1240
return _T6;}_TL1C5:
 Cyc_yythrowfail(s);;}
# 1244
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.Stmt_tok.tag=7U;_T1.Stmt_tok.val=yy1;_T0=_T1;}return _T0;}static char _TmpGF[27U]="list_t<switch_clause_t,`H>";
# 1240 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY8 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY8 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpGF,(unsigned char*)_TmpGF,(unsigned char*)_TmpGF + 27U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY8;_T3=_T2.tag;if(_T3!=15)goto _TL1C7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY8;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1243
return _T6;}_TL1C7:
 Cyc_yythrowfail(s);;}
# 1247
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY8.tag=15U;_T1.YY8.val=yy1;_T0=_T1;}return _T0;}static char _TmpG10[6U]="pat_t";
# 1241 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY9 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY9 _T5;struct Cyc_Absyn_Pat*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG10,(unsigned char*)_TmpG10,(unsigned char*)_TmpG10 + 6U};struct Cyc_Absyn_Pat*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY9;_T3=_T2.tag;if(_T3!=16)goto _TL1C9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY9;_T7=_T5.val;}{struct Cyc_Absyn_Pat*yy=_T7;_T6=yy;
# 1244
return _T6;}_TL1C9:
 Cyc_yythrowfail(s);;}
# 1248
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY9.tag=16U;_T1.YY9.val=yy1;_T0=_T1;}return _T0;}static char _TmpG11[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1246 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY10 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY10 _T5;struct _tuple23*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG11,(unsigned char*)_TmpG11,(unsigned char*)_TmpG11 + 28U};struct _tuple23*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY10;_T3=_T2.tag;if(_T3!=17)goto _TL1CB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY10;_T7=_T5.val;}{struct _tuple23*yy=_T7;_T6=yy;
# 1249
return _T6;}_TL1CB:
 Cyc_yythrowfail(s);;}
# 1253
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY10.tag=17U;_T1.YY10.val=yy1;_T0=_T1;}return _T0;}static char _TmpG12[17U]="list_t<pat_t,`H>";
# 1247 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY11 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY11 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG12,(unsigned char*)_TmpG12,(unsigned char*)_TmpG12 + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY11;_T3=_T2.tag;if(_T3!=18)goto _TL1CD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY11;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1250
return _T6;}_TL1CD:
 Cyc_yythrowfail(s);;}
# 1254
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY11.tag=18U;_T1.YY11.val=yy1;_T0=_T1;}return _T0;}static char _TmpG13[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1248 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY12 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY12 _T5;struct _tuple24*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG13,(unsigned char*)_TmpG13,(unsigned char*)_TmpG13 + 36U};struct _tuple24*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY12;_T3=_T2.tag;if(_T3!=19)goto _TL1CF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY12;_T7=_T5.val;}{struct _tuple24*yy=_T7;_T6=yy;
# 1251
return _T6;}_TL1CF:
 Cyc_yythrowfail(s);;}
# 1255
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY12.tag=19U;_T1.YY12.val=yy1;_T0=_T1;}return _T0;}static char _TmpG14[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1249 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY13 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY13 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG14,(unsigned char*)_TmpG14,(unsigned char*)_TmpG14 + 47U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY13;_T3=_T2.tag;if(_T3!=20)goto _TL1D1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY13;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1252
return _T6;}_TL1D1:
 Cyc_yythrowfail(s);;}
# 1256
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY13.tag=20U;_T1.YY13.val=yy1;_T0=_T1;}return _T0;}static char _TmpG15[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1250 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY14 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY14 _T5;struct _tuple23*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG15,(unsigned char*)_TmpG15,(unsigned char*)_TmpG15 + 58U};struct _tuple23*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY14;_T3=_T2.tag;if(_T3!=21)goto _TL1D3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY14;_T7=_T5.val;}{struct _tuple23*yy=_T7;_T6=yy;
# 1253
return _T6;}_TL1D3:
 Cyc_yythrowfail(s);;}
# 1257
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY14.tag=21U;_T1.YY14.val=yy1;_T0=_T1;}return _T0;}static char _TmpG16[9U]="fndecl_t";
# 1251 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY15 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY15 _T5;struct Cyc_Absyn_Fndecl*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG16,(unsigned char*)_TmpG16,(unsigned char*)_TmpG16 + 9U};struct Cyc_Absyn_Fndecl*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY15;_T3=_T2.tag;if(_T3!=22)goto _TL1D5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY15;_T7=_T5.val;}{struct Cyc_Absyn_Fndecl*yy=_T7;_T6=yy;
# 1254
return _T6;}_TL1D5:
 Cyc_yythrowfail(s);;}
# 1258
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY15.tag=22U;_T1.YY15.val=yy1;_T0=_T1;}return _T0;}static char _TmpG17[18U]="list_t<decl_t,`H>";
# 1252 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY16 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY16 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG17,(unsigned char*)_TmpG17,(unsigned char*)_TmpG17 + 18U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY16;_T3=_T2.tag;if(_T3!=23)goto _TL1D7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY16;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1255
return _T6;}_TL1D7:
 Cyc_yythrowfail(s);;}
# 1259
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY16.tag=23U;_T1.YY16.val=yy1;_T0=_T1;}return _T0;}static char _TmpG18[12U]="decl_spec_t";
# 1255 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY17 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY17 _T5;struct Cyc_Parse_Declaration_spec _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG18,(unsigned char*)_TmpG18,(unsigned char*)_TmpG18 + 12U};struct Cyc_Parse_Declaration_spec _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY17;_T3=_T2.tag;if(_T3!=24)goto _TL1D9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY17;_T7=_T5.val;}{struct Cyc_Parse_Declaration_spec yy=_T7;_T6=yy;
# 1258
return _T6;}_TL1D9:
 Cyc_yythrowfail(s);;}
# 1262
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY17.tag=24U;_T1.YY17.val=yy1;_T0=_T1;}return _T0;}static char _TmpG19[41U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)";
# 1256 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY18 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY18 _T5;struct _tuple12 _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG19,(unsigned char*)_TmpG19,(unsigned char*)_TmpG19 + 41U};struct _tuple12 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY18;_T3=_T2.tag;if(_T3!=25)goto _TL1DB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY18;_T7=_T5.val;}{struct _tuple12 yy=_T7;_T6=yy;
# 1259
return _T6;}_TL1DB:
 Cyc_yythrowfail(s);;}
# 1263
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY18.tag=25U;_T1.YY18.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1A[23U]="declarator_list_t<`yy>";
# 1257 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY19 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY19 _T5;struct _tuple11*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1A,(unsigned char*)_TmpG1A,(unsigned char*)_TmpG1A + 23U};struct _tuple11*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY19;_T3=_T2.tag;if(_T3!=26)goto _TL1DD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY19;_T7=_T5.val;}{struct _tuple11*yy=_T7;_T6=yy;
# 1260
return _T6;}_TL1DD:
 Cyc_yythrowfail(s);;}
# 1264
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY19.tag=26U;_T1.YY19.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1B[16U]="storage_class_t";
# 1258 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY20 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY20 _T5;enum Cyc_Parse_Storage_class _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1B,(unsigned char*)_TmpG1B,(unsigned char*)_TmpG1B + 16U};enum Cyc_Parse_Storage_class _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY20;_T3=_T2.tag;if(_T3!=27)goto _TL1DF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY20;_T7=_T5.val;}{enum Cyc_Parse_Storage_class yy=_T7;_T6=yy;
# 1261
return _T6;}_TL1DF:
 Cyc_yythrowfail(s);;}
# 1265
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY20.tag=27U;_T1.YY20.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1C[17U]="type_specifier_t";
# 1259 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY21 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY21 _T5;struct Cyc_Parse_Type_specifier _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1C,(unsigned char*)_TmpG1C,(unsigned char*)_TmpG1C + 17U};struct Cyc_Parse_Type_specifier _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY21;_T3=_T2.tag;if(_T3!=28)goto _TL1E1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY21;_T7=_T5.val;}{struct Cyc_Parse_Type_specifier yy=_T7;_T6=yy;
# 1262
return _T6;}_TL1E1:
 Cyc_yythrowfail(s);;}
# 1266
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY21.tag=28U;_T1.YY21.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1D[12U]="aggr_kind_t";
# 1261 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY22 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY22 _T5;enum Cyc_Absyn_AggrKind _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1D,(unsigned char*)_TmpG1D,(unsigned char*)_TmpG1D + 12U};enum Cyc_Absyn_AggrKind _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY22;_T3=_T2.tag;if(_T3!=29)goto _TL1E3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY22;_T7=_T5.val;}{enum Cyc_Absyn_AggrKind yy=_T7;_T6=yy;
# 1264
return _T6;}_TL1E3:
 Cyc_yythrowfail(s);;}
# 1268
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY22.tag=29U;_T1.YY22.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1E[20U]="$(bool,aggr_kind_t)";
# 1262 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY23 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY23 _T5;struct _tuple25 _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1E,(unsigned char*)_TmpG1E,(unsigned char*)_TmpG1E + 20U};struct _tuple25 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY23;_T3=_T2.tag;if(_T3!=30)goto _TL1E5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY23;_T7=_T5.val;}{struct _tuple25 yy=_T7;_T6=yy;
# 1265
return _T6;}_TL1E5:
 Cyc_yythrowfail(s);;}
# 1269
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY23.tag=30U;_T1.YY23.val=yy1;_T0=_T1;}return _T0;}static char _TmpG1F[8U]="tqual_t";
# 1263 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY24 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY24 _T5;struct Cyc_Absyn_Tqual _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG1F,(unsigned char*)_TmpG1F,(unsigned char*)_TmpG1F + 8U};struct Cyc_Absyn_Tqual _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY24;_T3=_T2.tag;if(_T3!=31)goto _TL1E7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY24;_T7=_T5.val;}{struct Cyc_Absyn_Tqual yy=_T7;_T6=yy;
# 1266
return _T6;}_TL1E7:
 Cyc_yythrowfail(s);;}
# 1270
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY24.tag=31U;_T1.YY24.val=yy1;_T0=_T1;}return _T0;}static char _TmpG20[23U]="list_t<aggrfield_t,`H>";
# 1264 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY25 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY25 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG20,(unsigned char*)_TmpG20,(unsigned char*)_TmpG20 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY25;_T3=_T2.tag;if(_T3!=32)goto _TL1E9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY25;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1267
return _T6;}_TL1E9:
 Cyc_yythrowfail(s);;}
# 1271
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY25.tag=32U;_T1.YY25.val=yy1;_T0=_T1;}return _T0;}static char _TmpG21[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1265 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY26 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY26 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG21,(unsigned char*)_TmpG21,(unsigned char*)_TmpG21 + 34U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY26;_T3=_T2.tag;if(_T3!=33)goto _TL1EB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY26;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1268
return _T6;}_TL1EB:
 Cyc_yythrowfail(s);;}
# 1272
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY26.tag=33U;_T1.YY26.val=yy1;_T0=_T1;}return _T0;}static char _TmpG22[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1266 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY27 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY27 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG22,(unsigned char*)_TmpG22,(unsigned char*)_TmpG22 + 33U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY27;_T3=_T2.tag;if(_T3!=34)goto _TL1ED;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY27;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1269
return _T6;}_TL1ED:
 Cyc_yythrowfail(s);;}
# 1273
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY27.tag=34U;_T1.YY27.val=yy1;_T0=_T1;}return _T0;}static char _TmpG23[18U]="declarator_t<`yy>";
# 1267 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY28 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY28 _T5;struct Cyc_Parse_Declarator _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG23,(unsigned char*)_TmpG23,(unsigned char*)_TmpG23 + 18U};struct Cyc_Parse_Declarator _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY28;_T3=_T2.tag;if(_T3!=35)goto _TL1EF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY28;_T7=_T5.val;}{struct Cyc_Parse_Declarator yy=_T7;_T6=yy;
# 1270
return _T6;}_TL1EF:
 Cyc_yythrowfail(s);;}
# 1274
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY28.tag=35U;_T1.YY28.val=yy1;_T0=_T1;}return _T0;}static char _TmpG24[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1268 "parse.y"
static struct _tuple12*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY29 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY29 _T5;struct _tuple12*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG24,(unsigned char*)_TmpG24,(unsigned char*)_TmpG24 + 45U};struct _tuple12*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY29;_T3=_T2.tag;if(_T3!=36)goto _TL1F1;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY29;_T7=_T5.val;}{struct _tuple12*yy=_T7;_T6=yy;
# 1271
return _T6;}_TL1F1:
 Cyc_yythrowfail(s);;}
# 1275
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple12*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY29.tag=36U;_T1.YY29.val=yy1;_T0=_T1;}return _T0;}static char _TmpG25[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1269 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY30 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY30 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG25,(unsigned char*)_TmpG25,(unsigned char*)_TmpG25 + 57U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY30;_T3=_T2.tag;if(_T3!=37)goto _TL1F3;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY30;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1272
return _T6;}_TL1F3:
 Cyc_yythrowfail(s);;}
# 1276
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY30.tag=37U;_T1.YY30.val=yy1;_T0=_T1;}return _T0;}static char _TmpG26[26U]="abstractdeclarator_t<`yy>";
# 1270 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY31 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY31 _T5;struct Cyc_Parse_Abstractdeclarator _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG26,(unsigned char*)_TmpG26,(unsigned char*)_TmpG26 + 26U};struct Cyc_Parse_Abstractdeclarator _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY31;_T3=_T2.tag;if(_T3!=38)goto _TL1F5;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY31;_T7=_T5.val;}{struct Cyc_Parse_Abstractdeclarator yy=_T7;_T6=yy;
# 1273
return _T6;}_TL1F5:
 Cyc_yythrowfail(s);;}
# 1277
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY31.tag=38U;_T1.YY31.val=yy1;_T0=_T1;}return _T0;}static char _TmpG27[5U]="bool";
# 1271 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY32 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY32 _T5;int _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG27,(unsigned char*)_TmpG27,(unsigned char*)_TmpG27 + 5U};int _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY32;_T3=_T2.tag;if(_T3!=39)goto _TL1F7;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY32;_T7=_T5.val;}{int yy=_T7;_T6=yy;
# 1274
return _T6;}_TL1F7:
 Cyc_yythrowfail(s);;}
# 1278
static union Cyc_YYSTYPE Cyc_YY32(int yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY32.tag=39U;_T1.YY32.val=yy1;_T0=_T1;}return _T0;}static char _TmpG28[8U]="scope_t";
# 1272 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY33 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY33 _T5;enum Cyc_Absyn_Scope _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG28,(unsigned char*)_TmpG28,(unsigned char*)_TmpG28 + 8U};enum Cyc_Absyn_Scope _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY33;_T3=_T2.tag;if(_T3!=40)goto _TL1F9;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY33;_T7=_T5.val;}{enum Cyc_Absyn_Scope yy=_T7;_T6=yy;
# 1275
return _T6;}_TL1F9:
 Cyc_yythrowfail(s);;}
# 1279
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY33.tag=40U;_T1.YY33.val=yy1;_T0=_T1;}return _T0;}static char _TmpG29[16U]="datatypefield_t";
# 1273 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY34 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY34 _T5;struct Cyc_Absyn_Datatypefield*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG29,(unsigned char*)_TmpG29,(unsigned char*)_TmpG29 + 16U};struct Cyc_Absyn_Datatypefield*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY34;_T3=_T2.tag;if(_T3!=41)goto _TL1FB;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY34;_T7=_T5.val;}{struct Cyc_Absyn_Datatypefield*yy=_T7;_T6=yy;
# 1276
return _T6;}_TL1FB:
 Cyc_yythrowfail(s);;}
# 1280
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY34.tag=41U;_T1.YY34.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2A[27U]="list_t<datatypefield_t,`H>";
# 1274 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY35 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY35 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2A,(unsigned char*)_TmpG2A,(unsigned char*)_TmpG2A + 27U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY35;_T3=_T2.tag;if(_T3!=42)goto _TL1FD;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY35;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1277
return _T6;}_TL1FD:
 Cyc_yythrowfail(s);;}
# 1281
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY35.tag=42U;_T1.YY35.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2B[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1275 "parse.y"
static struct _tuple26 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY36 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY36 _T5;struct _tuple26 _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2B,(unsigned char*)_TmpG2B,(unsigned char*)_TmpG2B + 41U};struct _tuple26 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY36;_T3=_T2.tag;if(_T3!=43)goto _TL1FF;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY36;_T7=_T5.val;}{struct _tuple26 yy=_T7;_T6=yy;
# 1278
return _T6;}_TL1FF:
 Cyc_yythrowfail(s);;}
# 1282
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple26 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY36.tag=43U;_T1.YY36.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2C[17U]="list_t<var_t,`H>";
# 1276 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY37 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY37 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2C,(unsigned char*)_TmpG2C,(unsigned char*)_TmpG2C + 17U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY37;_T3=_T2.tag;if(_T3!=44)goto _TL201;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY37;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1279
return _T6;}_TL201:
 Cyc_yythrowfail(s);;}
# 1283
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY37.tag=44U;_T1.YY37.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1277 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY38 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY38 _T5;struct _tuple8*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2D,(unsigned char*)_TmpG2D,(unsigned char*)_TmpG2D + 31U};struct _tuple8*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY38;_T3=_T2.tag;if(_T3!=45)goto _TL203;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY38;_T7=_T5.val;}{struct _tuple8*yy=_T7;_T6=yy;
# 1280
return _T6;}_TL203:
 Cyc_yythrowfail(s);;}
# 1284
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY38.tag=45U;_T1.YY38.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2E[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1278 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY39 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY39 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2E,(unsigned char*)_TmpG2E,(unsigned char*)_TmpG2E + 42U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY39;_T3=_T2.tag;if(_T3!=46)goto _TL205;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY39;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1281
return _T6;}_TL205:
 Cyc_yythrowfail(s);;}
# 1285
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY39.tag=46U;_T1.YY39.val=yy1;_T0=_T1;}return _T0;}static char _TmpG2F[139U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1279 "parse.y"
static struct _tuple27*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY40 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY40 _T5;struct _tuple27*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG2F,(unsigned char*)_TmpG2F,(unsigned char*)_TmpG2F + 139U};struct _tuple27*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY40;_T3=_T2.tag;if(_T3!=47)goto _TL207;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY40;_T7=_T5.val;}{struct _tuple27*yy=_T7;_T6=yy;
# 1282
return _T6;}_TL207:
 Cyc_yythrowfail(s);;}
# 1286
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple27*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY40.tag=47U;_T1.YY40.val=yy1;_T0=_T1;}return _T0;}static char _TmpG30[8U]="types_t";
# 1280 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY41 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY41 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG30,(unsigned char*)_TmpG30,(unsigned char*)_TmpG30 + 8U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY41;_T3=_T2.tag;if(_T3!=48)goto _TL209;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY41;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1283
return _T6;}_TL209:
 Cyc_yythrowfail(s);;}
# 1287
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY41.tag=48U;_T1.YY41.val=yy1;_T0=_T1;}return _T0;}static char _TmpG31[24U]="list_t<designator_t,`H>";
# 1282 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY42 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY42 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG31,(unsigned char*)_TmpG31,(unsigned char*)_TmpG31 + 24U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY42;_T3=_T2.tag;if(_T3!=49)goto _TL20B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY42;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1285
return _T6;}_TL20B:
 Cyc_yythrowfail(s);;}
# 1289
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY42.tag=49U;_T1.YY42.val=yy1;_T0=_T1;}return _T0;}static char _TmpG32[13U]="designator_t";
# 1283 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY43 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY43 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG32,(unsigned char*)_TmpG32,(unsigned char*)_TmpG32 + 13U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY43;_T3=_T2.tag;if(_T3!=50)goto _TL20D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY43;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1286
return _T6;}_TL20D:
 Cyc_yythrowfail(s);;}
# 1290
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY43.tag=50U;_T1.YY43.val=yy1;_T0=_T1;}return _T0;}static char _TmpG33[7U]="kind_t";
# 1284 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY44 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY44 _T5;struct Cyc_Absyn_Kind*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG33,(unsigned char*)_TmpG33,(unsigned char*)_TmpG33 + 7U};struct Cyc_Absyn_Kind*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY44;_T3=_T2.tag;if(_T3!=51)goto _TL20F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY44;_T7=_T5.val;}{struct Cyc_Absyn_Kind*yy=_T7;_T6=yy;
# 1287
return _T6;}_TL20F:
 Cyc_yythrowfail(s);;}
# 1291
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY44.tag=51U;_T1.YY44.val=yy1;_T0=_T1;}return _T0;}static char _TmpG34[7U]="type_t";
# 1285 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY45 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY45 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG34,(unsigned char*)_TmpG34,(unsigned char*)_TmpG34 + 7U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY45;_T3=_T2.tag;if(_T3!=52)goto _TL211;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY45;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1288
return _T6;}_TL211:
 Cyc_yythrowfail(s);;}
# 1292
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY45.tag=52U;_T1.YY45.val=yy1;_T0=_T1;}return _T0;}static char _TmpG35[23U]="list_t<attribute_t,`H>";
# 1286 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY46 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY46 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG35,(unsigned char*)_TmpG35,(unsigned char*)_TmpG35 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY46;_T3=_T2.tag;if(_T3!=53)goto _TL213;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY46;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1289
return _T6;}_TL213:
 Cyc_yythrowfail(s);;}
# 1293
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY46.tag=53U;_T1.YY46.val=yy1;_T0=_T1;}return _T0;}static char _TmpG36[12U]="attribute_t";
# 1287 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY47 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY47 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG36,(unsigned char*)_TmpG36,(unsigned char*)_TmpG36 + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY47;_T3=_T2.tag;if(_T3!=54)goto _TL215;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY47;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1290
return _T6;}_TL215:
 Cyc_yythrowfail(s);;}
# 1294
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY47.tag=54U;_T1.YY47.val=yy1;_T0=_T1;}return _T0;}static char _TmpG37[12U]="enumfield_t";
# 1288 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY48 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY48 _T5;struct Cyc_Absyn_Enumfield*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG37,(unsigned char*)_TmpG37,(unsigned char*)_TmpG37 + 12U};struct Cyc_Absyn_Enumfield*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY48;_T3=_T2.tag;if(_T3!=55)goto _TL217;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY48;_T7=_T5.val;}{struct Cyc_Absyn_Enumfield*yy=_T7;_T6=yy;
# 1291
return _T6;}_TL217:
 Cyc_yythrowfail(s);;}
# 1295
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY48.tag=55U;_T1.YY48.val=yy1;_T0=_T1;}return _T0;}static char _TmpG38[23U]="list_t<enumfield_t,`H>";
# 1289 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY49 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY49 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG38,(unsigned char*)_TmpG38,(unsigned char*)_TmpG38 + 23U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY49;_T3=_T2.tag;if(_T3!=56)goto _TL219;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY49;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1292
return _T6;}_TL219:
 Cyc_yythrowfail(s);;}
# 1296
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY49.tag=56U;_T1.YY49.val=yy1;_T0=_T1;}return _T0;}static char _TmpG39[11U]="type_opt_t";
# 1290 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY50 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY50 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG39,(unsigned char*)_TmpG39,(unsigned char*)_TmpG39 + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY50;_T3=_T2.tag;if(_T3!=57)goto _TL21B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY50;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1293
return _T6;}_TL21B:
 Cyc_yythrowfail(s);;}
# 1297
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY50.tag=57U;_T1.YY50.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3A[61U]="$(list_t<effconstr_t,`H>, list_t<$(type_t,type_t)@`H,`H>)*`H";
# 1292 "parse.y"
static struct _tuple28*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY51 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY51 _T5;struct _tuple28*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3A,(unsigned char*)_TmpG3A,(unsigned char*)_TmpG3A + 61U};struct _tuple28*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY51;_T3=_T2.tag;if(_T3!=58)goto _TL21D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY51;_T7=_T5.val;}{struct _tuple28*yy=_T7;_T6=yy;
# 1295
return _T6;}_TL21D:
 Cyc_yythrowfail(s);;}
# 1299
static union Cyc_YYSTYPE Cyc_YY51(struct _tuple28*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY51.tag=58U;_T1.YY51.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3B[12U]="effconstr_t";
# 1293 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY52 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY52 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3B,(unsigned char*)_TmpG3B,(unsigned char*)_TmpG3B + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY52;_T3=_T2.tag;if(_T3!=59)goto _TL21F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY52;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1296
return _T6;}_TL21F:
 Cyc_yythrowfail(s);;}
# 1300
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY52.tag=59U;_T1.YY52.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3C[21U]="$(type_t, type_t)@`H";
# 1294 "parse.y"
static struct _tuple29*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY53 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY53 _T5;struct _tuple29*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3C,(unsigned char*)_TmpG3C,(unsigned char*)_TmpG3C + 21U};struct _tuple29*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY53;_T3=_T2.tag;if(_T3!=60)goto _TL221;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY53;_T7=_T5.val;}{struct _tuple29*yy=_T7;_T6=yy;
# 1297
return _T6;}_TL221:
 Cyc_yythrowfail(s);;}
# 1301
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple29*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY53.tag=60U;_T1.YY53.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3D[11U]="booltype_t";
# 1295 "parse.y"
static void*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY54 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY54 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3D,(unsigned char*)_TmpG3D,(unsigned char*)_TmpG3D + 11U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY54;_T3=_T2.tag;if(_T3!=61)goto _TL223;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY54;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1298
return _T6;}_TL223:
 Cyc_yythrowfail(s);;}
# 1302
static union Cyc_YYSTYPE Cyc_YY54(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY54.tag=61U;_T1.YY54.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3E[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1296 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY55 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY55 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3E,(unsigned char*)_TmpG3E,(unsigned char*)_TmpG3E + 35U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY55;_T3=_T2.tag;if(_T3!=62)goto _TL225;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY55;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1299
return _T6;}_TL225:
 Cyc_yythrowfail(s);;}
# 1303
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY55.tag=62U;_T1.YY55.val=yy1;_T0=_T1;}return _T0;}static char _TmpG3F[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1297 "parse.y"
static struct _tuple30*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY56 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY56 _T5;struct _tuple30*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG3F,(unsigned char*)_TmpG3F,(unsigned char*)_TmpG3F + 48U};struct _tuple30*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY56;_T3=_T2.tag;if(_T3!=63)goto _TL227;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY56;_T7=_T5.val;}{struct _tuple30*yy=_T7;_T6=yy;
# 1300
return _T6;}_TL227:
 Cyc_yythrowfail(s);;}
# 1304
static union Cyc_YYSTYPE Cyc_YY56(struct _tuple30*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY56.tag=63U;_T1.YY56.val=yy1;_T0=_T1;}return _T0;}static char _TmpG40[18U]="list_t<qvar_t,`H>";
# 1298 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY57 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY57 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG40,(unsigned char*)_TmpG40,(unsigned char*)_TmpG40 + 18U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY57;_T3=_T2.tag;if(_T3!=64)goto _TL229;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY57;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1301
return _T6;}_TL229:
 Cyc_yythrowfail(s);;}
# 1305
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY57.tag=64U;_T1.YY57.val=yy1;_T0=_T1;}return _T0;}static char _TmpG41[12U]="aqualtype_t";
# 1299 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY58 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY58 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG41,(unsigned char*)_TmpG41,(unsigned char*)_TmpG41 + 12U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY58;_T3=_T2.tag;if(_T3!=65)goto _TL22B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY58;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1302
return _T6;}_TL22B:
 Cyc_yythrowfail(s);;}
# 1306
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY58.tag=65U;_T1.YY58.val=yy1;_T0=_T1;}return _T0;}static char _TmpG42[20U]="pointer_qual_t<`yy>";
# 1300 "parse.y"
static void*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY59 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY59 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG42,(unsigned char*)_TmpG42,(unsigned char*)_TmpG42 + 20U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY59;_T3=_T2.tag;if(_T3!=66)goto _TL22D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY59;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1303
return _T6;}_TL22D:
 Cyc_yythrowfail(s);;}
# 1307
static union Cyc_YYSTYPE Cyc_YY59(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY59.tag=66U;_T1.YY59.val=yy1;_T0=_T1;}return _T0;}static char _TmpG43[21U]="pointer_quals_t<`yy>";
# 1301 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY60 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY60 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG43,(unsigned char*)_TmpG43,(unsigned char*)_TmpG43 + 21U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY60;_T3=_T2.tag;if(_T3!=67)goto _TL22F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY60;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1304
return _T6;}_TL22F:
 Cyc_yythrowfail(s);;}
# 1308
static union Cyc_YYSTYPE Cyc_YY60(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY60.tag=67U;_T1.YY60.val=yy1;_T0=_T1;}return _T0;}static char _TmpG44[10U]="exp_opt_t";
# 1302 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY61 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY61 _T5;struct Cyc_Absyn_Exp*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG44,(unsigned char*)_TmpG44,(unsigned char*)_TmpG44 + 10U};struct Cyc_Absyn_Exp*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY61;_T3=_T2.tag;if(_T3!=68)goto _TL231;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY61;_T7=_T5.val;}{struct Cyc_Absyn_Exp*yy=_T7;_T6=yy;
# 1305
return _T6;}_TL231:
 Cyc_yythrowfail(s);;}
# 1309
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY61.tag=68U;_T1.YY61.val=yy1;_T0=_T1;}return _T0;}static char _TmpG45[43U]="$(exp_opt_t,exp_opt_t,exp_opt_t,exp_opt_t)";
# 1303 "parse.y"
static struct _tuple21 Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY62 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY62 _T5;struct _tuple21 _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG45,(unsigned char*)_TmpG45,(unsigned char*)_TmpG45 + 43U};struct _tuple21 _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY62;_T3=_T2.tag;if(_T3!=69)goto _TL233;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY62;_T7=_T5.val;}{struct _tuple21 yy=_T7;_T6=yy;
# 1306
return _T6;}_TL233:
 Cyc_yythrowfail(s);;}
# 1310
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple21 yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY62.tag=69U;_T1.YY62.val=yy1;_T0=_T1;}return _T0;}static char _TmpG46[10U]="raw_exp_t";
# 1305 "parse.y"
static void*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY63 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY63 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG46,(unsigned char*)_TmpG46,(unsigned char*)_TmpG46 + 10U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY63;_T3=_T2.tag;if(_T3!=70)goto _TL235;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY63;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1308
return _T6;}_TL235:
 Cyc_yythrowfail(s);;}
# 1312
static union Cyc_YYSTYPE Cyc_YY63(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY63.tag=70U;_T1.YY63.val=yy1;_T0=_T1;}return _T0;}static char _TmpG47[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1306 "parse.y"
static struct _tuple31*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY64 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY64 _T5;struct _tuple31*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG47,(unsigned char*)_TmpG47,(unsigned char*)_TmpG47 + 112U};struct _tuple31*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY64;_T3=_T2.tag;if(_T3!=71)goto _TL237;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY64;_T7=_T5.val;}{struct _tuple31*yy=_T7;_T6=yy;
# 1309
return _T6;}_TL237:
 Cyc_yythrowfail(s);;}
# 1313
static union Cyc_YYSTYPE Cyc_YY64(struct _tuple31*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY64.tag=71U;_T1.YY64.val=yy1;_T0=_T1;}return _T0;}static char _TmpG48[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1307 "parse.y"
static struct _tuple28*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY65 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY65 _T5;struct _tuple28*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG48,(unsigned char*)_TmpG48,(unsigned char*)_TmpG48 + 73U};struct _tuple28*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY65;_T3=_T2.tag;if(_T3!=72)goto _TL239;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY65;_T7=_T5.val;}{struct _tuple28*yy=_T7;_T6=yy;
# 1310
return _T6;}_TL239:
 Cyc_yythrowfail(s);;}
# 1314
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple28*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY65.tag=72U;_T1.YY65.val=yy1;_T0=_T1;}return _T0;}static char _TmpG49[28U]="list_t<string_t<`H>@`H, `H>";
# 1308 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY66 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY66 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG49,(unsigned char*)_TmpG49,(unsigned char*)_TmpG49 + 28U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY66;_T3=_T2.tag;if(_T3!=73)goto _TL23B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY66;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1311
return _T6;}_TL23B:
 Cyc_yythrowfail(s);;}
# 1315
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY66.tag=73U;_T1.YY66.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4A[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1309 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY67(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY67 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY67 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4A,(unsigned char*)_TmpG4A,(unsigned char*)_TmpG4A + 38U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY67;_T3=_T2.tag;if(_T3!=74)goto _TL23D;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY67;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1312
return _T6;}_TL23D:
 Cyc_yythrowfail(s);;}
# 1316
static union Cyc_YYSTYPE Cyc_YY67(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY67.tag=74U;_T1.YY67.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4B[26U]="$(string_t<`H>, exp_t)@`H";
# 1310 "parse.y"
static struct _tuple32*Cyc_yyget_YY68(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY68 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY68 _T5;struct _tuple32*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4B,(unsigned char*)_TmpG4B,(unsigned char*)_TmpG4B + 26U};struct _tuple32*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY68;_T3=_T2.tag;if(_T3!=75)goto _TL23F;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY68;_T7=_T5.val;}{struct _tuple32*yy=_T7;_T6=yy;
# 1313
return _T6;}_TL23F:
 Cyc_yythrowfail(s);;}
# 1317
static union Cyc_YYSTYPE Cyc_YY68(struct _tuple32*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY68.tag=75U;_T1.YY68.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4C[16U]="exp_maker_fun_t";
# 1311 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY69(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY69 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY69 _T5;struct Cyc_Absyn_Exp*(*_T6)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
static struct _fat_ptr s={(unsigned char*)_TmpG4C,(unsigned char*)_TmpG4C,(unsigned char*)_TmpG4C + 16U};struct Cyc_Absyn_Exp*(*_T7)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY69;_T3=_T2.tag;if(_T3!=76)goto _TL241;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY69;_T7=_T5.val;}{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_T7;_T6=yy;
# 1314
return _T6;}_TL241:
 Cyc_yythrowfail(s);;}
# 1318
static union Cyc_YYSTYPE Cyc_YY69(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY69.tag=76U;_T1.YY69.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4D[19U]="enum ConstraintOps";
# 1313 "parse.y"
static enum Cyc_Parse_ConstraintOps Cyc_yyget_YY70(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY70 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY70 _T5;enum Cyc_Parse_ConstraintOps _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4D,(unsigned char*)_TmpG4D,(unsigned char*)_TmpG4D + 19U};enum Cyc_Parse_ConstraintOps _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY70;_T3=_T2.tag;if(_T3!=77)goto _TL243;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY70;_T7=_T5.val;}{enum Cyc_Parse_ConstraintOps yy=_T7;_T6=yy;
# 1316
return _T6;}_TL243:
 Cyc_yythrowfail(s);;}
# 1320
static union Cyc_YYSTYPE Cyc_YY70(enum Cyc_Parse_ConstraintOps yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY70.tag=77U;_T1.YY70.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4E[60U]="list_t<$(type_t, list_t<BansheeIf::constraint_t,`H>)@`H,`H>";
# 1314 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY71(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY71 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY71 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4E,(unsigned char*)_TmpG4E,(unsigned char*)_TmpG4E + 60U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY71;_T3=_T2.tag;if(_T3!=78)goto _TL245;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY71;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1317
return _T6;}_TL245:
 Cyc_yythrowfail(s);;}
# 1321
static union Cyc_YYSTYPE Cyc_YY71(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY71.tag=78U;_T1.YY71.val=yy1;_T0=_T1;}return _T0;}static char _TmpG4F[35U]="list_t<BansheeIf::constraint_t,`H>";
# 1315 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY72(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY72 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY72 _T5;struct Cyc_List_List*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG4F,(unsigned char*)_TmpG4F,(unsigned char*)_TmpG4F + 35U};struct Cyc_List_List*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY72;_T3=_T2.tag;if(_T3!=79)goto _TL247;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY72;_T7=_T5.val;}{struct Cyc_List_List*yy=_T7;_T6=yy;
# 1318
return _T6;}_TL247:
 Cyc_yythrowfail(s);;}
# 1322
static union Cyc_YYSTYPE Cyc_YY72(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY72.tag=79U;_T1.YY72.val=yy1;_T0=_T1;}return _T0;}static char _TmpG50[24U]="BansheeIf::constraint_t";
# 1316 "parse.y"
static void*Cyc_yyget_YY73(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY73 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY73 _T5;void*_T6;
static struct _fat_ptr s={(unsigned char*)_TmpG50,(unsigned char*)_TmpG50,(unsigned char*)_TmpG50 + 24U};void*_T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY73;_T3=_T2.tag;if(_T3!=80)goto _TL249;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY73;_T7=_T5.val;}{void*yy=_T7;_T6=yy;
# 1319
return _T6;}_TL249:
 Cyc_yythrowfail(s);;}
# 1323
static union Cyc_YYSTYPE Cyc_YY73(void*yy1){union Cyc_YYSTYPE _T0;{union Cyc_YYSTYPE _T1;_T1.YY73.tag=80U;_T1.YY73.val=yy1;_T0=_T1;}return _T0;}static char _TmpG51[4U]="int";
# 1318 "parse.y"
static int Cyc_yyget_YY74(union Cyc_YYSTYPE*yy1){union Cyc_YYSTYPE*_T0;union Cyc_YYSTYPE*_T1;struct _union_YYSTYPE_YY74 _T2;unsigned _T3;union Cyc_YYSTYPE*_T4;struct _union_YYSTYPE_YY74 _T5;int _T6;
static struct _fat_ptr s={(unsigned char*)_TmpG51,(unsigned char*)_TmpG51,(unsigned char*)_TmpG51 + 4U};int _T7;_T0=yy1;_T1=(union Cyc_YYSTYPE*)_T0;_T2=_T1->YY74;_T3=_T2.tag;if(_T3!=81)goto _TL24B;_T4=yy1;{union Cyc_YYSTYPE _T8=*_T4;_T5=_T8.YY74;_T7=_T5.val;}{int yy=_T7;_T6=yy;
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
static struct _fat_ptr Cyc_yytname[363U]={{(unsigned char*)_TmpG52,(unsigned char*)_TmpG52,(unsigned char*)_TmpG52 + 2U},{(unsigned char*)_TmpG53,(unsigned char*)_TmpG53,(unsigned char*)_TmpG53 + 6U},{(unsigned char*)_TmpG54,(unsigned char*)_TmpG54,(unsigned char*)_TmpG54 + 12U},{(unsigned char*)_TmpG55,(unsigned char*)_TmpG55,(unsigned char*)_TmpG55 + 5U},{(unsigned char*)_TmpG56,(unsigned char*)_TmpG56,(unsigned char*)_TmpG56 + 9U},{(unsigned char*)_TmpG57,(unsigned char*)_TmpG57,(unsigned char*)_TmpG57 + 7U},{(unsigned char*)_TmpG58,(unsigned char*)_TmpG58,(unsigned char*)_TmpG58 + 7U},{(unsigned char*)_TmpG59,(unsigned char*)_TmpG59,(unsigned char*)_TmpG59 + 8U},{(unsigned char*)_TmpG5A,(unsigned char*)_TmpG5A,(unsigned char*)_TmpG5A + 5U},{(unsigned char*)_TmpG5B,(unsigned char*)_TmpG5B,(unsigned char*)_TmpG5B + 5U},{(unsigned char*)_TmpG5C,(unsigned char*)_TmpG5C,(unsigned char*)_TmpG5C + 6U},{(unsigned char*)_TmpG5D,(unsigned char*)_TmpG5D,(unsigned char*)_TmpG5D + 4U},{(unsigned char*)_TmpG5E,(unsigned char*)_TmpG5E,(unsigned char*)_TmpG5E + 5U},{(unsigned char*)_TmpG5F,(unsigned char*)_TmpG5F,(unsigned char*)_TmpG5F + 6U},{(unsigned char*)_TmpG60,(unsigned char*)_TmpG60,(unsigned char*)_TmpG60 + 7U},{(unsigned char*)_TmpG61,(unsigned char*)_TmpG61,(unsigned char*)_TmpG61 + 7U},{(unsigned char*)_TmpG62,(unsigned char*)_TmpG62,(unsigned char*)_TmpG62 + 9U},{(unsigned char*)_TmpG63,(unsigned char*)_TmpG63,(unsigned char*)_TmpG63 + 6U},{(unsigned char*)_TmpG64,(unsigned char*)_TmpG64,(unsigned char*)_TmpG64 + 9U},{(unsigned char*)_TmpG65,(unsigned char*)_TmpG65,(unsigned char*)_TmpG65 + 9U},{(unsigned char*)_TmpG66,(unsigned char*)_TmpG66,(unsigned char*)_TmpG66 + 7U},{(unsigned char*)_TmpG67,(unsigned char*)_TmpG67,(unsigned char*)_TmpG67 + 6U},{(unsigned char*)_TmpG68,(unsigned char*)_TmpG68,(unsigned char*)_TmpG68 + 5U},{(unsigned char*)_TmpG69,(unsigned char*)_TmpG69,(unsigned char*)_TmpG69 + 8U},{(unsigned char*)_TmpG6A,(unsigned char*)_TmpG6A,(unsigned char*)_TmpG6A + 7U},{(unsigned char*)_TmpG6B,(unsigned char*)_TmpG6B,(unsigned char*)_TmpG6B + 7U},{(unsigned char*)_TmpG6C,(unsigned char*)_TmpG6C,(unsigned char*)_TmpG6C + 9U},{(unsigned char*)_TmpG6D,(unsigned char*)_TmpG6D,(unsigned char*)_TmpG6D + 3U},{(unsigned char*)_TmpG6E,(unsigned char*)_TmpG6E,(unsigned char*)_TmpG6E + 5U},{(unsigned char*)_TmpG6F,(unsigned char*)_TmpG6F,(unsigned char*)_TmpG6F + 7U},{(unsigned char*)_TmpG70,(unsigned char*)_TmpG70,(unsigned char*)_TmpG70 + 6U},{(unsigned char*)_TmpG71,(unsigned char*)_TmpG71,(unsigned char*)_TmpG71 + 3U},{(unsigned char*)_TmpG72,(unsigned char*)_TmpG72,(unsigned char*)_TmpG72 + 4U},{(unsigned char*)_TmpG73,(unsigned char*)_TmpG73,(unsigned char*)_TmpG73 + 5U},{(unsigned char*)_TmpG74,(unsigned char*)_TmpG74,(unsigned char*)_TmpG74 + 9U},{(unsigned char*)_TmpG75,(unsigned char*)_TmpG75,(unsigned char*)_TmpG75 + 6U},{(unsigned char*)_TmpG76,(unsigned char*)_TmpG76,(unsigned char*)_TmpG76 + 7U},{(unsigned char*)_TmpG77,(unsigned char*)_TmpG77,(unsigned char*)_TmpG77 + 5U},{(unsigned char*)_TmpG78,(unsigned char*)_TmpG78,(unsigned char*)_TmpG78 + 7U},{(unsigned char*)_TmpG79,(unsigned char*)_TmpG79,(unsigned char*)_TmpG79 + 16U},{(unsigned char*)_TmpG7A,(unsigned char*)_TmpG7A,(unsigned char*)_TmpG7A + 10U},{(unsigned char*)_TmpG7B,(unsigned char*)_TmpG7B,(unsigned char*)_TmpG7B + 8U},{(unsigned char*)_TmpG7C,(unsigned char*)_TmpG7C,(unsigned char*)_TmpG7C + 8U},{(unsigned char*)_TmpG7D,(unsigned char*)_TmpG7D,(unsigned char*)_TmpG7D + 4U},{(unsigned char*)_TmpG7E,(unsigned char*)_TmpG7E,(unsigned char*)_TmpG7E + 6U},{(unsigned char*)_TmpG7F,(unsigned char*)_TmpG7F,(unsigned char*)_TmpG7F + 4U},{(unsigned char*)_TmpG80,(unsigned char*)_TmpG80,(unsigned char*)_TmpG80 + 6U},{(unsigned char*)_TmpG81,(unsigned char*)_TmpG81,(unsigned char*)_TmpG81 + 7U},{(unsigned char*)_TmpG82,(unsigned char*)_TmpG82,(unsigned char*)_TmpG82 + 9U},{(unsigned char*)_TmpG83,(unsigned char*)_TmpG83,(unsigned char*)_TmpG83 + 5U},{(unsigned char*)_TmpG84,(unsigned char*)_TmpG84,(unsigned char*)_TmpG84 + 4U},{(unsigned char*)_TmpG85,(unsigned char*)_TmpG85,(unsigned char*)_TmpG85 + 5U},{(unsigned char*)_TmpG86,(unsigned char*)_TmpG86,(unsigned char*)_TmpG86 + 9U},{(unsigned char*)_TmpG87,(unsigned char*)_TmpG87,(unsigned char*)_TmpG87 + 9U},{(unsigned char*)_TmpG88,(unsigned char*)_TmpG88,(unsigned char*)_TmpG88 + 6U},{(unsigned char*)_TmpG89,(unsigned char*)_TmpG89,(unsigned char*)_TmpG89 + 10U},{(unsigned char*)_TmpG8A,(unsigned char*)_TmpG8A,(unsigned char*)_TmpG8A + 12U},{(unsigned char*)_TmpG8B,(unsigned char*)_TmpG8B,(unsigned char*)_TmpG8B + 9U},{(unsigned char*)_TmpG8C,(unsigned char*)_TmpG8C,(unsigned char*)_TmpG8C + 7U},{(unsigned char*)_TmpG8D,(unsigned char*)_TmpG8D,(unsigned char*)_TmpG8D + 8U},{(unsigned char*)_TmpG8E,(unsigned char*)_TmpG8E,(unsigned char*)_TmpG8E + 9U},{(unsigned char*)_TmpG8F,(unsigned char*)_TmpG8F,(unsigned char*)_TmpG8F + 15U},{(unsigned char*)_TmpG90,(unsigned char*)_TmpG90,(unsigned char*)_TmpG90 + 8U},{(unsigned char*)_TmpG91,(unsigned char*)_TmpG91,(unsigned char*)_TmpG91 + 7U},{(unsigned char*)_TmpG92,(unsigned char*)_TmpG92,(unsigned char*)_TmpG92 + 8U},{(unsigned char*)_TmpG93,(unsigned char*)_TmpG93,(unsigned char*)_TmpG93 + 8U},{(unsigned char*)_TmpG94,(unsigned char*)_TmpG94,(unsigned char*)_TmpG94 + 5U},{(unsigned char*)_TmpG95,(unsigned char*)_TmpG95,(unsigned char*)_TmpG95 + 7U},{(unsigned char*)_TmpG96,(unsigned char*)_TmpG96,(unsigned char*)_TmpG96 + 9U},{(unsigned char*)_TmpG97,(unsigned char*)_TmpG97,(unsigned char*)_TmpG97 + 6U},{(unsigned char*)_TmpG98,(unsigned char*)_TmpG98,(unsigned char*)_TmpG98 + 7U},{(unsigned char*)_TmpG99,(unsigned char*)_TmpG99,(unsigned char*)_TmpG99 + 5U},{(unsigned char*)_TmpG9A,(unsigned char*)_TmpG9A,(unsigned char*)_TmpG9A + 8U},{(unsigned char*)_TmpG9B,(unsigned char*)_TmpG9B,(unsigned char*)_TmpG9B + 7U},{(unsigned char*)_TmpG9C,(unsigned char*)_TmpG9C,(unsigned char*)_TmpG9C + 8U},{(unsigned char*)_TmpG9D,(unsigned char*)_TmpG9D,(unsigned char*)_TmpG9D + 7U},{(unsigned char*)_TmpG9E,(unsigned char*)_TmpG9E,(unsigned char*)_TmpG9E + 10U},{(unsigned char*)_TmpG9F,(unsigned char*)_TmpG9F,(unsigned char*)_TmpG9F + 11U},{(unsigned char*)_TmpGA0,(unsigned char*)_TmpGA0,(unsigned char*)_TmpGA0 + 13U},{(unsigned char*)_TmpGA1,(unsigned char*)_TmpGA1,(unsigned char*)_TmpGA1 + 10U},{(unsigned char*)_TmpGA2,(unsigned char*)_TmpGA2,(unsigned char*)_TmpGA2 + 14U},{(unsigned char*)_TmpGA3,(unsigned char*)_TmpGA3,(unsigned char*)_TmpGA3 + 10U},{(unsigned char*)_TmpGA4,(unsigned char*)_TmpGA4,(unsigned char*)_TmpGA4 + 8U},{(unsigned char*)_TmpGA5,(unsigned char*)_TmpGA5,(unsigned char*)_TmpGA5 + 8U},{(unsigned char*)_TmpGA6,(unsigned char*)_TmpGA6,(unsigned char*)_TmpGA6 + 6U},{(unsigned char*)_TmpGA7,(unsigned char*)_TmpGA7,(unsigned char*)_TmpGA7 + 8U},{(unsigned char*)_TmpGA8,(unsigned char*)_TmpGA8,(unsigned char*)_TmpGA8 + 10U},{(unsigned char*)_TmpGA9,(unsigned char*)_TmpGA9,(unsigned char*)_TmpGA9 + 9U},{(unsigned char*)_TmpGAA,(unsigned char*)_TmpGAA,(unsigned char*)_TmpGAA + 13U},{(unsigned char*)_TmpGAB,(unsigned char*)_TmpGAB,(unsigned char*)_TmpGAB + 10U},{(unsigned char*)_TmpGAC,(unsigned char*)_TmpGAC,(unsigned char*)_TmpGAC + 9U},{(unsigned char*)_TmpGAD,(unsigned char*)_TmpGAD,(unsigned char*)_TmpGAD + 13U},{(unsigned char*)_TmpGAE,(unsigned char*)_TmpGAE,(unsigned char*)_TmpGAE + 14U},{(unsigned char*)_TmpGAF,(unsigned char*)_TmpGAF,(unsigned char*)_TmpGAF + 14U},{(unsigned char*)_TmpGB0,(unsigned char*)_TmpGB0,(unsigned char*)_TmpGB0 + 13U},{(unsigned char*)_TmpGB1,(unsigned char*)_TmpGB1,(unsigned char*)_TmpGB1 + 12U},{(unsigned char*)_TmpGB2,(unsigned char*)_TmpGB2,(unsigned char*)_TmpGB2 + 12U},{(unsigned char*)_TmpGB3,(unsigned char*)_TmpGB3,(unsigned char*)_TmpGB3 + 12U},{(unsigned char*)_TmpGB4,(unsigned char*)_TmpGB4,(unsigned char*)_TmpGB4 + 16U},{(unsigned char*)_TmpGB5,(unsigned char*)_TmpGB5,(unsigned char*)_TmpGB5 + 14U},{(unsigned char*)_TmpGB6,(unsigned char*)_TmpGB6,(unsigned char*)_TmpGB6 + 12U},{(unsigned char*)_TmpGB7,(unsigned char*)_TmpGB7,(unsigned char*)_TmpGB7 + 12U},{(unsigned char*)_TmpGB8,(unsigned char*)_TmpGB8,(unsigned char*)_TmpGB8 + 18U},{(unsigned char*)_TmpGB9,(unsigned char*)_TmpGB9,(unsigned char*)_TmpGB9 + 11U},{(unsigned char*)_TmpGBA,(unsigned char*)_TmpGBA,(unsigned char*)_TmpGBA + 7U},{(unsigned char*)_TmpGBB,(unsigned char*)_TmpGBB,(unsigned char*)_TmpGBB + 12U},{(unsigned char*)_TmpGBC,(unsigned char*)_TmpGBC,(unsigned char*)_TmpGBC + 16U},{(unsigned char*)_TmpGBD,(unsigned char*)_TmpGBD,(unsigned char*)_TmpGBD + 18U},{(unsigned char*)_TmpGBE,(unsigned char*)_TmpGBE,(unsigned char*)_TmpGBE + 7U},{(unsigned char*)_TmpGBF,(unsigned char*)_TmpGBF,(unsigned char*)_TmpGBF + 7U},{(unsigned char*)_TmpGC0,(unsigned char*)_TmpGC0,(unsigned char*)_TmpGC0 + 7U},{(unsigned char*)_TmpGC1,(unsigned char*)_TmpGC1,(unsigned char*)_TmpGC1 + 8U},{(unsigned char*)_TmpGC2,(unsigned char*)_TmpGC2,(unsigned char*)_TmpGC2 + 9U},{(unsigned char*)_TmpGC3,(unsigned char*)_TmpGC3,(unsigned char*)_TmpGC3 + 6U},{(unsigned char*)_TmpGC4,(unsigned char*)_TmpGC4,(unsigned char*)_TmpGC4 + 6U},{(unsigned char*)_TmpGC5,(unsigned char*)_TmpGC5,(unsigned char*)_TmpGC5 + 6U},{(unsigned char*)_TmpGC6,(unsigned char*)_TmpGC6,(unsigned char*)_TmpGC6 + 6U},{(unsigned char*)_TmpGC7,(unsigned char*)_TmpGC7,(unsigned char*)_TmpGC7 + 7U},{(unsigned char*)_TmpGC8,(unsigned char*)_TmpGC8,(unsigned char*)_TmpGC8 + 6U},{(unsigned char*)_TmpGC9,(unsigned char*)_TmpGC9,(unsigned char*)_TmpGC9 + 11U},{(unsigned char*)_TmpGCA,(unsigned char*)_TmpGCA,(unsigned char*)_TmpGCA + 11U},{(unsigned char*)_TmpGCB,(unsigned char*)_TmpGCB,(unsigned char*)_TmpGCB + 11U},{(unsigned char*)_TmpGCC,(unsigned char*)_TmpGCC,(unsigned char*)_TmpGCC + 11U},{(unsigned char*)_TmpGCD,(unsigned char*)_TmpGCD,(unsigned char*)_TmpGCD + 11U},{(unsigned char*)_TmpGCE,(unsigned char*)_TmpGCE,(unsigned char*)_TmpGCE + 12U},{(unsigned char*)_TmpGCF,(unsigned char*)_TmpGCF,(unsigned char*)_TmpGCF + 13U},{(unsigned char*)_TmpGD0,(unsigned char*)_TmpGD0,(unsigned char*)_TmpGD0 + 11U},{(unsigned char*)_TmpGD1,(unsigned char*)_TmpGD1,(unsigned char*)_TmpGD1 + 11U},{(unsigned char*)_TmpGD2,(unsigned char*)_TmpGD2,(unsigned char*)_TmpGD2 + 10U},{(unsigned char*)_TmpGD3,(unsigned char*)_TmpGD3,(unsigned char*)_TmpGD3 + 9U},{(unsigned char*)_TmpGD4,(unsigned char*)_TmpGD4,(unsigned char*)_TmpGD4 + 11U},{(unsigned char*)_TmpGD5,(unsigned char*)_TmpGD5,(unsigned char*)_TmpGD5 + 12U},{(unsigned char*)_TmpGD6,(unsigned char*)_TmpGD6,(unsigned char*)_TmpGD6 + 11U},{(unsigned char*)_TmpGD7,(unsigned char*)_TmpGD7,(unsigned char*)_TmpGD7 + 17U},{(unsigned char*)_TmpGD8,(unsigned char*)_TmpGD8,(unsigned char*)_TmpGD8 + 7U},{(unsigned char*)_TmpGD9,(unsigned char*)_TmpGD9,(unsigned char*)_TmpGD9 + 8U},{(unsigned char*)_TmpGDA,(unsigned char*)_TmpGDA,(unsigned char*)_TmpGDA + 19U},{(unsigned char*)_TmpGDB,(unsigned char*)_TmpGDB,(unsigned char*)_TmpGDB + 20U},{(unsigned char*)_TmpGDC,(unsigned char*)_TmpGDC,(unsigned char*)_TmpGDC + 18U},{(unsigned char*)_TmpGDD,(unsigned char*)_TmpGDD,(unsigned char*)_TmpGDD + 9U},{(unsigned char*)_TmpGDE,(unsigned char*)_TmpGDE,(unsigned char*)_TmpGDE + 13U},{(unsigned char*)_TmpGDF,(unsigned char*)_TmpGDF,(unsigned char*)_TmpGDF + 16U},{(unsigned char*)_TmpGE0,(unsigned char*)_TmpGE0,(unsigned char*)_TmpGE0 + 18U},{(unsigned char*)_TmpGE1,(unsigned char*)_TmpGE1,(unsigned char*)_TmpGE1 + 18U},{(unsigned char*)_TmpGE2,(unsigned char*)_TmpGE2,(unsigned char*)_TmpGE2 + 10U},{(unsigned char*)_TmpGE3,(unsigned char*)_TmpGE3,(unsigned char*)_TmpGE3 + 8U},{(unsigned char*)_TmpGE4,(unsigned char*)_TmpGE4,(unsigned char*)_TmpGE4 + 4U},{(unsigned char*)_TmpGE5,(unsigned char*)_TmpGE5,(unsigned char*)_TmpGE5 + 4U},{(unsigned char*)_TmpGE6,(unsigned char*)_TmpGE6,(unsigned char*)_TmpGE6 + 4U},{(unsigned char*)_TmpGE7,(unsigned char*)_TmpGE7,(unsigned char*)_TmpGE7 + 4U},{(unsigned char*)_TmpGE8,(unsigned char*)_TmpGE8,(unsigned char*)_TmpGE8 + 4U},{(unsigned char*)_TmpGE9,(unsigned char*)_TmpGE9,(unsigned char*)_TmpGE9 + 4U},{(unsigned char*)_TmpGEA,(unsigned char*)_TmpGEA,(unsigned char*)_TmpGEA + 4U},{(unsigned char*)_TmpGEB,(unsigned char*)_TmpGEB,(unsigned char*)_TmpGEB + 4U},{(unsigned char*)_TmpGEC,(unsigned char*)_TmpGEC,(unsigned char*)_TmpGEC + 4U},{(unsigned char*)_TmpGED,(unsigned char*)_TmpGED,(unsigned char*)_TmpGED + 4U},{(unsigned char*)_TmpGEE,(unsigned char*)_TmpGEE,(unsigned char*)_TmpGEE + 4U},{(unsigned char*)_TmpGEF,(unsigned char*)_TmpGEF,(unsigned char*)_TmpGEF + 4U},{(unsigned char*)_TmpGF0,(unsigned char*)_TmpGF0,(unsigned char*)_TmpGF0 + 4U},{(unsigned char*)_TmpGF1,(unsigned char*)_TmpGF1,(unsigned char*)_TmpGF1 + 4U},{(unsigned char*)_TmpGF2,(unsigned char*)_TmpGF2,(unsigned char*)_TmpGF2 + 4U},{(unsigned char*)_TmpGF3,(unsigned char*)_TmpGF3,(unsigned char*)_TmpGF3 + 4U},{(unsigned char*)_TmpGF4,(unsigned char*)_TmpGF4,(unsigned char*)_TmpGF4 + 4U},{(unsigned char*)_TmpGF5,(unsigned char*)_TmpGF5,(unsigned char*)_TmpGF5 + 4U},{(unsigned char*)_TmpGF6,(unsigned char*)_TmpGF6,(unsigned char*)_TmpGF6 + 4U},{(unsigned char*)_TmpGF7,(unsigned char*)_TmpGF7,(unsigned char*)_TmpGF7 + 4U},{(unsigned char*)_TmpGF8,(unsigned char*)_TmpGF8,(unsigned char*)_TmpGF8 + 4U},{(unsigned char*)_TmpGF9,(unsigned char*)_TmpGF9,(unsigned char*)_TmpGF9 + 4U},{(unsigned char*)_TmpGFA,(unsigned char*)_TmpGFA,(unsigned char*)_TmpGFA + 4U},{(unsigned char*)_TmpGFB,(unsigned char*)_TmpGFB,(unsigned char*)_TmpGFB + 4U},{(unsigned char*)_TmpGFC,(unsigned char*)_TmpGFC,(unsigned char*)_TmpGFC + 4U},{(unsigned char*)_TmpGFD,(unsigned char*)_TmpGFD,(unsigned char*)_TmpGFD + 4U},{(unsigned char*)_TmpGFE,(unsigned char*)_TmpGFE,(unsigned char*)_TmpGFE + 4U},{(unsigned char*)_TmpGFF,(unsigned char*)_TmpGFF,(unsigned char*)_TmpGFF + 4U},{(unsigned char*)_TmpG100,(unsigned char*)_TmpG100,(unsigned char*)_TmpG100 + 4U},{(unsigned char*)_TmpG101,(unsigned char*)_TmpG101,(unsigned char*)_TmpG101 + 20U},{(unsigned char*)_TmpG102,(unsigned char*)_TmpG102,(unsigned char*)_TmpG102 + 5U},{(unsigned char*)_TmpG103,(unsigned char*)_TmpG103,(unsigned char*)_TmpG103 + 17U},{(unsigned char*)_TmpG104,(unsigned char*)_TmpG104,(unsigned char*)_TmpG104 + 18U},{(unsigned char*)_TmpG105,(unsigned char*)_TmpG105,(unsigned char*)_TmpG105 + 19U},{(unsigned char*)_TmpG106,(unsigned char*)_TmpG106,(unsigned char*)_TmpG106 + 16U},{(unsigned char*)_TmpG107,(unsigned char*)_TmpG107,(unsigned char*)_TmpG107 + 13U},{(unsigned char*)_TmpG108,(unsigned char*)_TmpG108,(unsigned char*)_TmpG108 + 14U},{(unsigned char*)_TmpG109,(unsigned char*)_TmpG109,(unsigned char*)_TmpG109 + 17U},{(unsigned char*)_TmpG10A,(unsigned char*)_TmpG10A,(unsigned char*)_TmpG10A + 16U},{(unsigned char*)_TmpG10B,(unsigned char*)_TmpG10B,(unsigned char*)_TmpG10B + 12U},{(unsigned char*)_TmpG10C,(unsigned char*)_TmpG10C,(unsigned char*)_TmpG10C + 19U},{(unsigned char*)_TmpG10D,(unsigned char*)_TmpG10D,(unsigned char*)_TmpG10D + 13U},{(unsigned char*)_TmpG10E,(unsigned char*)_TmpG10E,(unsigned char*)_TmpG10E + 21U},{(unsigned char*)_TmpG10F,(unsigned char*)_TmpG10F,(unsigned char*)_TmpG10F + 14U},{(unsigned char*)_TmpG110,(unsigned char*)_TmpG110,(unsigned char*)_TmpG110 + 20U},{(unsigned char*)_TmpG111,(unsigned char*)_TmpG111,(unsigned char*)_TmpG111 + 21U},{(unsigned char*)_TmpG112,(unsigned char*)_TmpG112,(unsigned char*)_TmpG112 + 13U},{(unsigned char*)_TmpG113,(unsigned char*)_TmpG113,(unsigned char*)_TmpG113 + 15U},{(unsigned char*)_TmpG114,(unsigned char*)_TmpG114,(unsigned char*)_TmpG114 + 17U},{(unsigned char*)_TmpG115,(unsigned char*)_TmpG115,(unsigned char*)_TmpG115 + 19U},{(unsigned char*)_TmpG116,(unsigned char*)_TmpG116,(unsigned char*)_TmpG116 + 19U},{(unsigned char*)_TmpG117,(unsigned char*)_TmpG117,(unsigned char*)_TmpG117 + 21U},{(unsigned char*)_TmpG118,(unsigned char*)_TmpG118,(unsigned char*)_TmpG118 + 12U},{(unsigned char*)_TmpG119,(unsigned char*)_TmpG119,(unsigned char*)_TmpG119 + 9U},{(unsigned char*)_TmpG11A,(unsigned char*)_TmpG11A,(unsigned char*)_TmpG11A + 17U},{(unsigned char*)_TmpG11B,(unsigned char*)_TmpG11B,(unsigned char*)_TmpG11B + 23U},{(unsigned char*)_TmpG11C,(unsigned char*)_TmpG11C,(unsigned char*)_TmpG11C + 24U},{(unsigned char*)_TmpG11D,(unsigned char*)_TmpG11D,(unsigned char*)_TmpG11D + 15U},{(unsigned char*)_TmpG11E,(unsigned char*)_TmpG11E,(unsigned char*)_TmpG11E + 11U},{(unsigned char*)_TmpG11F,(unsigned char*)_TmpG11F,(unsigned char*)_TmpG11F + 15U},{(unsigned char*)_TmpG120,(unsigned char*)_TmpG120,(unsigned char*)_TmpG120 + 10U},{(unsigned char*)_TmpG121,(unsigned char*)_TmpG121,(unsigned char*)_TmpG121 + 15U},{(unsigned char*)_TmpG122,(unsigned char*)_TmpG122,(unsigned char*)_TmpG122 + 25U},{(unsigned char*)_TmpG123,(unsigned char*)_TmpG123,(unsigned char*)_TmpG123 + 5U},{(unsigned char*)_TmpG124,(unsigned char*)_TmpG124,(unsigned char*)_TmpG124 + 15U},{(unsigned char*)_TmpG125,(unsigned char*)_TmpG125,(unsigned char*)_TmpG125 + 15U},{(unsigned char*)_TmpG126,(unsigned char*)_TmpG126,(unsigned char*)_TmpG126 + 11U},{(unsigned char*)_TmpG127,(unsigned char*)_TmpG127,(unsigned char*)_TmpG127 + 22U},{(unsigned char*)_TmpG128,(unsigned char*)_TmpG128,(unsigned char*)_TmpG128 + 26U},{(unsigned char*)_TmpG129,(unsigned char*)_TmpG129,(unsigned char*)_TmpG129 + 26U},{(unsigned char*)_TmpG12A,(unsigned char*)_TmpG12A,(unsigned char*)_TmpG12A + 16U},{(unsigned char*)_TmpG12B,(unsigned char*)_TmpG12B,(unsigned char*)_TmpG12B + 16U},{(unsigned char*)_TmpG12C,(unsigned char*)_TmpG12C,(unsigned char*)_TmpG12C + 24U},{(unsigned char*)_TmpG12D,(unsigned char*)_TmpG12D,(unsigned char*)_TmpG12D + 25U},{(unsigned char*)_TmpG12E,(unsigned char*)_TmpG12E,(unsigned char*)_TmpG12E + 25U},{(unsigned char*)_TmpG12F,(unsigned char*)_TmpG12F,(unsigned char*)_TmpG12F + 16U},{(unsigned char*)_TmpG130,(unsigned char*)_TmpG130,(unsigned char*)_TmpG130 + 19U},{(unsigned char*)_TmpG131,(unsigned char*)_TmpG131,(unsigned char*)_TmpG131 + 25U},{(unsigned char*)_TmpG132,(unsigned char*)_TmpG132,(unsigned char*)_TmpG132 + 35U},{(unsigned char*)_TmpG133,(unsigned char*)_TmpG133,(unsigned char*)_TmpG133 + 27U},{(unsigned char*)_TmpG134,(unsigned char*)_TmpG134,(unsigned char*)_TmpG134 + 18U},{(unsigned char*)_TmpG135,(unsigned char*)_TmpG135,(unsigned char*)_TmpG135 + 20U},{(unsigned char*)_TmpG136,(unsigned char*)_TmpG136,(unsigned char*)_TmpG136 + 19U},{(unsigned char*)_TmpG137,(unsigned char*)_TmpG137,(unsigned char*)_TmpG137 + 14U},{(unsigned char*)_TmpG138,(unsigned char*)_TmpG138,(unsigned char*)_TmpG138 + 19U},{(unsigned char*)_TmpG139,(unsigned char*)_TmpG139,(unsigned char*)_TmpG139 + 20U},{(unsigned char*)_TmpG13A,(unsigned char*)_TmpG13A,(unsigned char*)_TmpG13A + 14U},{(unsigned char*)_TmpG13B,(unsigned char*)_TmpG13B,(unsigned char*)_TmpG13B + 11U},{(unsigned char*)_TmpG13C,(unsigned char*)_TmpG13C,(unsigned char*)_TmpG13C + 23U},{(unsigned char*)_TmpG13D,(unsigned char*)_TmpG13D,(unsigned char*)_TmpG13D + 18U},{(unsigned char*)_TmpG13E,(unsigned char*)_TmpG13E,(unsigned char*)_TmpG13E + 30U},{(unsigned char*)_TmpG13F,(unsigned char*)_TmpG13F,(unsigned char*)_TmpG13F + 8U},{(unsigned char*)_TmpG140,(unsigned char*)_TmpG140,(unsigned char*)_TmpG140 + 12U},{(unsigned char*)_TmpG141,(unsigned char*)_TmpG141,(unsigned char*)_TmpG141 + 14U},{(unsigned char*)_TmpG142,(unsigned char*)_TmpG142,(unsigned char*)_TmpG142 + 13U},{(unsigned char*)_TmpG143,(unsigned char*)_TmpG143,(unsigned char*)_TmpG143 + 16U},{(unsigned char*)_TmpG144,(unsigned char*)_TmpG144,(unsigned char*)_TmpG144 + 23U},{(unsigned char*)_TmpG145,(unsigned char*)_TmpG145,(unsigned char*)_TmpG145 + 14U},{(unsigned char*)_TmpG146,(unsigned char*)_TmpG146,(unsigned char*)_TmpG146 + 18U},{(unsigned char*)_TmpG147,(unsigned char*)_TmpG147,(unsigned char*)_TmpG147 + 8U},{(unsigned char*)_TmpG148,(unsigned char*)_TmpG148,(unsigned char*)_TmpG148 + 8U},{(unsigned char*)_TmpG149,(unsigned char*)_TmpG149,(unsigned char*)_TmpG149 + 11U},{(unsigned char*)_TmpG14A,(unsigned char*)_TmpG14A,(unsigned char*)_TmpG14A + 20U},{(unsigned char*)_TmpG14B,(unsigned char*)_TmpG14B,(unsigned char*)_TmpG14B + 14U},{(unsigned char*)_TmpG14C,(unsigned char*)_TmpG14C,(unsigned char*)_TmpG14C + 9U},{(unsigned char*)_TmpG14D,(unsigned char*)_TmpG14D,(unsigned char*)_TmpG14D + 16U},{(unsigned char*)_TmpG14E,(unsigned char*)_TmpG14E,(unsigned char*)_TmpG14E + 27U},{(unsigned char*)_TmpG14F,(unsigned char*)_TmpG14F,(unsigned char*)_TmpG14F + 18U},{(unsigned char*)_TmpG150,(unsigned char*)_TmpG150,(unsigned char*)_TmpG150 + 14U},{(unsigned char*)_TmpG151,(unsigned char*)_TmpG151,(unsigned char*)_TmpG151 + 13U},{(unsigned char*)_TmpG152,(unsigned char*)_TmpG152,(unsigned char*)_TmpG152 + 12U},{(unsigned char*)_TmpG153,(unsigned char*)_TmpG153,(unsigned char*)_TmpG153 + 15U},{(unsigned char*)_TmpG154,(unsigned char*)_TmpG154,(unsigned char*)_TmpG154 + 14U},{(unsigned char*)_TmpG155,(unsigned char*)_TmpG155,(unsigned char*)_TmpG155 + 16U},{(unsigned char*)_TmpG156,(unsigned char*)_TmpG156,(unsigned char*)_TmpG156 + 11U},{(unsigned char*)_TmpG157,(unsigned char*)_TmpG157,(unsigned char*)_TmpG157 + 14U},{(unsigned char*)_TmpG158,(unsigned char*)_TmpG158,(unsigned char*)_TmpG158 + 11U},{(unsigned char*)_TmpG159,(unsigned char*)_TmpG159,(unsigned char*)_TmpG159 + 15U},{(unsigned char*)_TmpG15A,(unsigned char*)_TmpG15A,(unsigned char*)_TmpG15A + 22U},{(unsigned char*)_TmpG15B,(unsigned char*)_TmpG15B,(unsigned char*)_TmpG15B + 16U},{(unsigned char*)_TmpG15C,(unsigned char*)_TmpG15C,(unsigned char*)_TmpG15C + 17U},{(unsigned char*)_TmpG15D,(unsigned char*)_TmpG15D,(unsigned char*)_TmpG15D + 12U},{(unsigned char*)_TmpG15E,(unsigned char*)_TmpG15E,(unsigned char*)_TmpG15E + 18U},{(unsigned char*)_TmpG15F,(unsigned char*)_TmpG15F,(unsigned char*)_TmpG15F + 17U},{(unsigned char*)_TmpG160,(unsigned char*)_TmpG160,(unsigned char*)_TmpG160 + 12U},{(unsigned char*)_TmpG161,(unsigned char*)_TmpG161,(unsigned char*)_TmpG161 + 16U},{(unsigned char*)_TmpG162,(unsigned char*)_TmpG162,(unsigned char*)_TmpG162 + 11U},{(unsigned char*)_TmpG163,(unsigned char*)_TmpG163,(unsigned char*)_TmpG163 + 10U},{(unsigned char*)_TmpG164,(unsigned char*)_TmpG164,(unsigned char*)_TmpG164 + 14U},{(unsigned char*)_TmpG165,(unsigned char*)_TmpG165,(unsigned char*)_TmpG165 + 15U},{(unsigned char*)_TmpG166,(unsigned char*)_TmpG166,(unsigned char*)_TmpG166 + 20U},{(unsigned char*)_TmpG167,(unsigned char*)_TmpG167,(unsigned char*)_TmpG167 + 27U},{(unsigned char*)_TmpG168,(unsigned char*)_TmpG168,(unsigned char*)_TmpG168 + 16U},{(unsigned char*)_TmpG169,(unsigned char*)_TmpG169,(unsigned char*)_TmpG169 + 20U},{(unsigned char*)_TmpG16A,(unsigned char*)_TmpG16A,(unsigned char*)_TmpG16A + 10U},{(unsigned char*)_TmpG16B,(unsigned char*)_TmpG16B,(unsigned char*)_TmpG16B + 18U},{(unsigned char*)_TmpG16C,(unsigned char*)_TmpG16C,(unsigned char*)_TmpG16C + 21U},{(unsigned char*)_TmpG16D,(unsigned char*)_TmpG16D,(unsigned char*)_TmpG16D + 18U},{(unsigned char*)_TmpG16E,(unsigned char*)_TmpG16E,(unsigned char*)_TmpG16E + 16U},{(unsigned char*)_TmpG16F,(unsigned char*)_TmpG16F,(unsigned char*)_TmpG16F + 25U},{(unsigned char*)_TmpG170,(unsigned char*)_TmpG170,(unsigned char*)_TmpG170 + 19U},{(unsigned char*)_TmpG171,(unsigned char*)_TmpG171,(unsigned char*)_TmpG171 + 16U},{(unsigned char*)_TmpG172,(unsigned char*)_TmpG172,(unsigned char*)_TmpG172 + 20U},{(unsigned char*)_TmpG173,(unsigned char*)_TmpG173,(unsigned char*)_TmpG173 + 15U},{(unsigned char*)_TmpG174,(unsigned char*)_TmpG174,(unsigned char*)_TmpG174 + 20U},{(unsigned char*)_TmpG175,(unsigned char*)_TmpG175,(unsigned char*)_TmpG175 + 12U},{(unsigned char*)_TmpG176,(unsigned char*)_TmpG176,(unsigned char*)_TmpG176 + 15U},{(unsigned char*)_TmpG177,(unsigned char*)_TmpG177,(unsigned char*)_TmpG177 + 12U},{(unsigned char*)_TmpG178,(unsigned char*)_TmpG178,(unsigned char*)_TmpG178 + 20U},{(unsigned char*)_TmpG179,(unsigned char*)_TmpG179,(unsigned char*)_TmpG179 + 19U},{(unsigned char*)_TmpG17A,(unsigned char*)_TmpG17A,(unsigned char*)_TmpG17A + 20U},{(unsigned char*)_TmpG17B,(unsigned char*)_TmpG17B,(unsigned char*)_TmpG17B + 21U},{(unsigned char*)_TmpG17C,(unsigned char*)_TmpG17C,(unsigned char*)_TmpG17C + 21U},{(unsigned char*)_TmpG17D,(unsigned char*)_TmpG17D,(unsigned char*)_TmpG17D + 12U},{(unsigned char*)_TmpG17E,(unsigned char*)_TmpG17E,(unsigned char*)_TmpG17E + 17U},{(unsigned char*)_TmpG17F,(unsigned char*)_TmpG17F,(unsigned char*)_TmpG17F + 19U},{(unsigned char*)_TmpG180,(unsigned char*)_TmpG180,(unsigned char*)_TmpG180 + 14U},{(unsigned char*)_TmpG181,(unsigned char*)_TmpG181,(unsigned char*)_TmpG181 + 17U},{(unsigned char*)_TmpG182,(unsigned char*)_TmpG182,(unsigned char*)_TmpG182 + 23U},{(unsigned char*)_TmpG183,(unsigned char*)_TmpG183,(unsigned char*)_TmpG183 + 13U},{(unsigned char*)_TmpG184,(unsigned char*)_TmpG184,(unsigned char*)_TmpG184 + 14U},{(unsigned char*)_TmpG185,(unsigned char*)_TmpG185,(unsigned char*)_TmpG185 + 16U},{(unsigned char*)_TmpG186,(unsigned char*)_TmpG186,(unsigned char*)_TmpG186 + 16U},{(unsigned char*)_TmpG187,(unsigned char*)_TmpG187,(unsigned char*)_TmpG187 + 8U},{(unsigned char*)_TmpG188,(unsigned char*)_TmpG188,(unsigned char*)_TmpG188 + 19U},{(unsigned char*)_TmpG189,(unsigned char*)_TmpG189,(unsigned char*)_TmpG189 + 20U},{(unsigned char*)_TmpG18A,(unsigned char*)_TmpG18A,(unsigned char*)_TmpG18A + 14U},{(unsigned char*)_TmpG18B,(unsigned char*)_TmpG18B,(unsigned char*)_TmpG18B + 19U},{(unsigned char*)_TmpG18C,(unsigned char*)_TmpG18C,(unsigned char*)_TmpG18C + 20U},{(unsigned char*)_TmpG18D,(unsigned char*)_TmpG18D,(unsigned char*)_TmpG18D + 11U},{(unsigned char*)_TmpG18E,(unsigned char*)_TmpG18E,(unsigned char*)_TmpG18E + 22U},{(unsigned char*)_TmpG18F,(unsigned char*)_TmpG18F,(unsigned char*)_TmpG18F + 20U},{(unsigned char*)_TmpG190,(unsigned char*)_TmpG190,(unsigned char*)_TmpG190 + 23U},{(unsigned char*)_TmpG191,(unsigned char*)_TmpG191,(unsigned char*)_TmpG191 + 20U},{(unsigned char*)_TmpG192,(unsigned char*)_TmpG192,(unsigned char*)_TmpG192 + 22U},{(unsigned char*)_TmpG193,(unsigned char*)_TmpG193,(unsigned char*)_TmpG193 + 23U},{(unsigned char*)_TmpG194,(unsigned char*)_TmpG194,(unsigned char*)_TmpG194 + 24U},{(unsigned char*)_TmpG195,(unsigned char*)_TmpG195,(unsigned char*)_TmpG195 + 24U},{(unsigned char*)_TmpG196,(unsigned char*)_TmpG196,(unsigned char*)_TmpG196 + 15U},{(unsigned char*)_TmpG197,(unsigned char*)_TmpG197,(unsigned char*)_TmpG197 + 20U},{(unsigned char*)_TmpG198,(unsigned char*)_TmpG198,(unsigned char*)_TmpG198 + 22U},{(unsigned char*)_TmpG199,(unsigned char*)_TmpG199,(unsigned char*)_TmpG199 + 17U},{(unsigned char*)_TmpG19A,(unsigned char*)_TmpG19A,(unsigned char*)_TmpG19A + 20U},{(unsigned char*)_TmpG19B,(unsigned char*)_TmpG19B,(unsigned char*)_TmpG19B + 26U},{(unsigned char*)_TmpG19C,(unsigned char*)_TmpG19C,(unsigned char*)_TmpG19C + 12U},{(unsigned char*)_TmpG19D,(unsigned char*)_TmpG19D,(unsigned char*)_TmpG19D + 14U},{(unsigned char*)_TmpG19E,(unsigned char*)_TmpG19E,(unsigned char*)_TmpG19E + 12U},{(unsigned char*)_TmpG19F,(unsigned char*)_TmpG19F,(unsigned char*)_TmpG19F + 18U},{(unsigned char*)_TmpG1A0,(unsigned char*)_TmpG1A0,(unsigned char*)_TmpG1A0 + 16U},{(unsigned char*)_TmpG1A1,(unsigned char*)_TmpG1A1,(unsigned char*)_TmpG1A1 + 17U},{(unsigned char*)_TmpG1A2,(unsigned char*)_TmpG1A2,(unsigned char*)_TmpG1A2 + 15U},{(unsigned char*)_TmpG1A3,(unsigned char*)_TmpG1A3,(unsigned char*)_TmpG1A3 + 9U},{(unsigned char*)_TmpG1A4,(unsigned char*)_TmpG1A4,(unsigned char*)_TmpG1A4 + 13U},{(unsigned char*)_TmpG1A5,(unsigned char*)_TmpG1A5,(unsigned char*)_TmpG1A5 + 12U},{(unsigned char*)_TmpG1A6,(unsigned char*)_TmpG1A6,(unsigned char*)_TmpG1A6 + 12U},{(unsigned char*)_TmpG1A7,(unsigned char*)_TmpG1A7,(unsigned char*)_TmpG1A7 + 11U},{(unsigned char*)_TmpG1A8,(unsigned char*)_TmpG1A8,(unsigned char*)_TmpG1A8 + 11U},{(unsigned char*)_TmpG1A9,(unsigned char*)_TmpG1A9,(unsigned char*)_TmpG1A9 + 11U},{(unsigned char*)_TmpG1AA,(unsigned char*)_TmpG1AA,(unsigned char*)_TmpG1AA + 16U},{(unsigned char*)_TmpG1AB,(unsigned char*)_TmpG1AB,(unsigned char*)_TmpG1AB + 17U},{(unsigned char*)_TmpG1AC,(unsigned char*)_TmpG1AC,(unsigned char*)_TmpG1AC + 19U},{(unsigned char*)_TmpG1AD,(unsigned char*)_TmpG1AD,(unsigned char*)_TmpG1AD + 17U},{(unsigned char*)_TmpG1AE,(unsigned char*)_TmpG1AE,(unsigned char*)_TmpG1AE + 19U},{(unsigned char*)_TmpG1AF,(unsigned char*)_TmpG1AF,(unsigned char*)_TmpG1AF + 25U},{(unsigned char*)_TmpG1B0,(unsigned char*)_TmpG1B0,(unsigned char*)_TmpG1B0 + 26U},{(unsigned char*)_TmpG1B1,(unsigned char*)_TmpG1B1,(unsigned char*)_TmpG1B1 + 9U},{(unsigned char*)_TmpG1B2,(unsigned char*)_TmpG1B2,(unsigned char*)_TmpG1B2 + 20U},{(unsigned char*)_TmpG1B3,(unsigned char*)_TmpG1B3,(unsigned char*)_TmpG1B3 + 17U},{(unsigned char*)_TmpG1B4,(unsigned char*)_TmpG1B4,(unsigned char*)_TmpG1B4 + 18U},{(unsigned char*)_TmpG1B5,(unsigned char*)_TmpG1B5,(unsigned char*)_TmpG1B5 + 11U},{(unsigned char*)_TmpG1B6,(unsigned char*)_TmpG1B6,(unsigned char*)_TmpG1B6 + 12U},{(unsigned char*)_TmpG1B7,(unsigned char*)_TmpG1B7,(unsigned char*)_TmpG1B7 + 16U},{(unsigned char*)_TmpG1B8,(unsigned char*)_TmpG1B8,(unsigned char*)_TmpG1B8 + 20U},{(unsigned char*)_TmpG1B9,(unsigned char*)_TmpG1B9,(unsigned char*)_TmpG1B9 + 16U},{(unsigned char*)_TmpG1BA,(unsigned char*)_TmpG1BA,(unsigned char*)_TmpG1BA + 15U},{(unsigned char*)_TmpG1BB,(unsigned char*)_TmpG1BB,(unsigned char*)_TmpG1BB + 11U},{(unsigned char*)_TmpG1BC,(unsigned char*)_TmpG1BC,(unsigned char*)_TmpG1BC + 5U}};
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
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){union Cyc_YYSTYPE _T0;struct _fat_ptr _T1;struct _RegionHandle*_T2;void*_T3;struct Cyc_Yystacktype*_T4;struct Cyc_Yystacktype*_T5;struct _RegionHandle*_T6;unsigned _T7;unsigned _T8;unsigned _T9;struct _fat_ptr _TA;int _TB;unsigned char*_TC;short*_TD;int _TE;int _TF;int _T10;int _T11;struct _fat_ptr _T12;int _T13;int _T14;struct Cyc_Yystack_overflow_exn_struct*_T15;struct Cyc_Yystack_overflow_exn_struct*_T16;struct _fat_ptr _T17;int _T18;short*_T19;struct _RegionHandle*_T1A;unsigned _T1B;unsigned _T1C;int _T1D;unsigned _T1E;unsigned _T1F;struct _fat_ptr _T20;unsigned char*_T21;short*_T22;unsigned _T23;int _T24;unsigned _T25;struct _fat_ptr _T26;int _T27;struct Cyc_Yystacktype*_T28;struct _RegionHandle*_T29;unsigned _T2A;unsigned _T2B;int _T2C;unsigned _T2D;unsigned _T2E;struct _fat_ptr _T2F;unsigned _T30;int _T31;unsigned char*_T32;struct Cyc_Yystacktype*_T33;unsigned _T34;struct _fat_ptr _T35;unsigned char*_T36;struct Cyc_Yystacktype*_T37;short*_T38;int _T39;char*_T3A;short*_T3B;short _T3C;int _T3D;int _T3E;int _T3F;int _T40;struct Cyc_Lexing_lexbuf*_T41;union Cyc_YYSTYPE*_T42;union Cyc_YYSTYPE*_T43;struct Cyc_Yyltype*_T44;struct Cyc_Yyltype*_T45;int _T46;short*_T47;int _T48;short _T49;int _T4A;short*_T4B;int _T4C;short _T4D;int _T4E;int _T4F;short*_T50;int _T51;short _T52;int _T53;int _T54;struct _fat_ptr _T55;int _T56;unsigned char*_T57;struct Cyc_Yystacktype*_T58;struct Cyc_Yystacktype _T59;short*_T5A;int _T5B;char*_T5C;short*_T5D;short _T5E;short*_T5F;int _T60;char*_T61;short*_T62;short _T63;struct _fat_ptr _T64;int _T65;int _T66;int _T67;struct _fat_ptr _T68;unsigned char*_T69;unsigned char*_T6A;struct Cyc_Yystacktype*_T6B;struct Cyc_Yystacktype _T6C;int _T6D;int _T6E;struct Cyc_Yystacktype*_T6F;union Cyc_YYSTYPE*_T70;union Cyc_YYSTYPE*_T71;struct Cyc_Yystacktype*_T72;struct Cyc_Yystacktype _T73;struct Cyc_Yystacktype*_T74;union Cyc_YYSTYPE*_T75;union Cyc_YYSTYPE*_T76;struct Cyc_Yystacktype*_T77;union Cyc_YYSTYPE*_T78;union Cyc_YYSTYPE*_T79;struct Cyc_List_List*_T7A;struct Cyc_Yystacktype*_T7B;union Cyc_YYSTYPE*_T7C;union Cyc_YYSTYPE*_T7D;struct Cyc_List_List*_T7E;struct Cyc_List_List*_T7F;struct Cyc_List_List*_T80;struct Cyc_Absyn_Decl*_T81;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T82;struct Cyc_Yystacktype*_T83;union Cyc_YYSTYPE*_T84;union Cyc_YYSTYPE*_T85;struct Cyc_Yystacktype*_T86;union Cyc_YYSTYPE*_T87;union Cyc_YYSTYPE*_T88;struct Cyc_Yystacktype*_T89;struct Cyc_Yystacktype _T8A;struct Cyc_Yyltype _T8B;unsigned _T8C;struct Cyc_List_List*_T8D;struct Cyc_Absyn_Decl*_T8E;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T8F;struct Cyc_Yystacktype*_T90;union Cyc_YYSTYPE*_T91;union Cyc_YYSTYPE*_T92;struct Cyc_Yystacktype*_T93;union Cyc_YYSTYPE*_T94;union Cyc_YYSTYPE*_T95;struct Cyc_Yystacktype*_T96;struct Cyc_Yystacktype _T97;struct Cyc_Yyltype _T98;unsigned _T99;struct Cyc_Yystacktype*_T9A;union Cyc_YYSTYPE*_T9B;union Cyc_YYSTYPE*_T9C;struct Cyc_List_List*_T9D;struct Cyc_Absyn_Decl*_T9E;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T9F;struct _fat_ptr*_TA0;struct Cyc_Yystacktype*_TA1;union Cyc_YYSTYPE*_TA2;union Cyc_YYSTYPE*_TA3;struct Cyc_Yystacktype*_TA4;union Cyc_YYSTYPE*_TA5;union Cyc_YYSTYPE*_TA6;struct Cyc_Yystacktype*_TA7;struct Cyc_Yystacktype _TA8;struct Cyc_Yyltype _TA9;unsigned _TAA;struct Cyc_List_List*_TAB;struct Cyc_Absyn_Decl*_TAC;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_TAD;struct _fat_ptr*_TAE;struct Cyc_Yystacktype*_TAF;union Cyc_YYSTYPE*_TB0;union Cyc_YYSTYPE*_TB1;struct Cyc_Yystacktype*_TB2;union Cyc_YYSTYPE*_TB3;union Cyc_YYSTYPE*_TB4;struct Cyc_Yystacktype*_TB5;struct Cyc_Yystacktype _TB6;struct Cyc_Yyltype _TB7;unsigned _TB8;struct Cyc_Yystacktype*_TB9;union Cyc_YYSTYPE*_TBA;union Cyc_YYSTYPE*_TBB;struct Cyc_Yystacktype*_TBC;union Cyc_YYSTYPE*_TBD;union Cyc_YYSTYPE*_TBE;struct Cyc_List_List*_TBF;struct Cyc_Yystacktype*_TC0;union Cyc_YYSTYPE*_TC1;union Cyc_YYSTYPE*_TC2;struct Cyc_List_List*_TC3;struct Cyc_List_List*_TC4;struct Cyc_Yystacktype*_TC5;union Cyc_YYSTYPE*_TC6;union Cyc_YYSTYPE*_TC7;struct Cyc_Yystacktype*_TC8;union Cyc_YYSTYPE*_TC9;union Cyc_YYSTYPE*_TCA;struct Cyc_Yystacktype*_TCB;union Cyc_YYSTYPE*_TCC;union Cyc_YYSTYPE*_TCD;struct Cyc_Yystacktype*_TCE;union Cyc_YYSTYPE*_TCF;union Cyc_YYSTYPE*_TD0;struct Cyc_Yystacktype*_TD1;struct Cyc_Yystacktype _TD2;struct Cyc_Yyltype _TD3;unsigned _TD4;unsigned _TD5;struct _fat_ptr _TD6;struct _fat_ptr _TD7;struct Cyc_List_List*_TD8;unsigned _TD9;unsigned _TDA;int _TDB;struct Cyc_Yystacktype*_TDC;struct Cyc_Yystacktype _TDD;struct Cyc_Yyltype _TDE;unsigned _TDF;int _TE0;struct Cyc_Yystacktype*_TE1;struct Cyc_Yystacktype _TE2;struct Cyc_Yyltype _TE3;unsigned _TE4;unsigned _TE5;struct _fat_ptr _TE6;struct _fat_ptr _TE7;struct Cyc_List_List*_TE8;struct Cyc_Absyn_Decl*_TE9;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_TEA;struct Cyc_Yystacktype*_TEB;union Cyc_YYSTYPE*_TEC;union Cyc_YYSTYPE*_TED;struct _tuple10*_TEE;struct Cyc_Yystacktype*_TEF;struct Cyc_Yystacktype _TF0;struct Cyc_Yyltype _TF1;unsigned _TF2;struct Cyc_Yystacktype*_TF3;union Cyc_YYSTYPE*_TF4;union Cyc_YYSTYPE*_TF5;struct Cyc_List_List*_TF6;struct Cyc_Absyn_Decl*_TF7;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_TF8;struct Cyc_Yystacktype*_TF9;union Cyc_YYSTYPE*_TFA;union Cyc_YYSTYPE*_TFB;struct Cyc_Yystacktype*_TFC;struct Cyc_Yystacktype _TFD;struct Cyc_Yyltype _TFE;unsigned _TFF;struct Cyc_Yystacktype*_T100;union Cyc_YYSTYPE*_T101;union Cyc_YYSTYPE*_T102;struct Cyc_List_List*_T103;struct Cyc_Absyn_Decl*_T104;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T105;struct Cyc_Yystacktype*_T106;union Cyc_YYSTYPE*_T107;union Cyc_YYSTYPE*_T108;struct _tuple10*_T109;struct Cyc_Yystacktype*_T10A;struct Cyc_Yystacktype _T10B;struct Cyc_Yyltype _T10C;unsigned _T10D;struct Cyc_Yystacktype*_T10E;union Cyc_YYSTYPE*_T10F;union Cyc_YYSTYPE*_T110;struct Cyc_List_List*_T111;struct Cyc_Absyn_Decl*_T112;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T113;struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_T114;struct Cyc_Yystacktype*_T115;struct Cyc_Yystacktype _T116;struct Cyc_Yyltype _T117;unsigned _T118;struct Cyc_Yystacktype*_T119;union Cyc_YYSTYPE*_T11A;union Cyc_YYSTYPE*_T11B;struct Cyc_List_List*_T11C;struct Cyc_Absyn_Decl*_T11D;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T11E;struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_T11F;struct Cyc_Yystacktype*_T120;struct Cyc_Yystacktype _T121;struct Cyc_Yyltype _T122;unsigned _T123;struct Cyc_Yystacktype*_T124;union Cyc_YYSTYPE*_T125;union Cyc_YYSTYPE*_T126;struct Cyc_List_List*_T127;struct Cyc_Absyn_Decl*_T128;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T129;struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*_T12A;struct Cyc_Yystacktype*_T12B;struct Cyc_Yystacktype _T12C;struct Cyc_Yyltype _T12D;unsigned _T12E;struct Cyc_Yystacktype*_T12F;union Cyc_YYSTYPE*_T130;union Cyc_YYSTYPE*_T131;struct Cyc_List_List*_T132;struct Cyc_Absyn_Decl*_T133;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T134;struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*_T135;struct Cyc_Yystacktype*_T136;struct Cyc_Yystacktype _T137;struct Cyc_Yyltype _T138;unsigned _T139;struct Cyc_Yystacktype*_T13A;union Cyc_YYSTYPE*_T13B;union Cyc_YYSTYPE*_T13C;struct Cyc_Yystacktype*_T13D;union Cyc_YYSTYPE*_T13E;union Cyc_YYSTYPE*_T13F;struct _fat_ptr _T140;struct _fat_ptr _T141;int _T142;struct Cyc_Yystacktype*_T143;union Cyc_YYSTYPE*_T144;union Cyc_YYSTYPE*_T145;struct _fat_ptr _T146;struct _fat_ptr _T147;int _T148;struct Cyc_Yystacktype*_T149;struct Cyc_Yystacktype _T14A;struct Cyc_Yyltype _T14B;unsigned _T14C;unsigned _T14D;struct _fat_ptr _T14E;struct _fat_ptr _T14F;struct Cyc_Yystacktype*_T150;struct Cyc_Yystacktype _T151;struct Cyc_List_List*_T152;struct Cyc_Yystacktype*_T153;union Cyc_YYSTYPE*_T154;union Cyc_YYSTYPE*_T155;struct Cyc_List_List*_T156;struct Cyc_Yystacktype*_T157;union Cyc_YYSTYPE*_T158;union Cyc_YYSTYPE*_T159;struct Cyc_List_List*_T15A;struct Cyc_Yystacktype*_T15B;union Cyc_YYSTYPE*_T15C;union Cyc_YYSTYPE*_T15D;struct Cyc_Yystacktype*_T15E;union Cyc_YYSTYPE*_T15F;union Cyc_YYSTYPE*_T160;struct _tuple30*_T161;struct Cyc_Yystacktype*_T162;struct Cyc_Yystacktype _T163;struct _tuple30*_T164;struct Cyc_Yystacktype*_T165;union Cyc_YYSTYPE*_T166;union Cyc_YYSTYPE*_T167;struct _tuple30*_T168;struct _tuple30*_T169;struct Cyc_Yystacktype*_T16A;struct Cyc_Yystacktype _T16B;struct Cyc_Yyltype _T16C;unsigned _T16D;struct Cyc_List_List*_T16E;struct _tuple33*_T16F;struct Cyc_Yystacktype*_T170;struct Cyc_Yystacktype _T171;struct Cyc_Yyltype _T172;unsigned _T173;struct Cyc_Yystacktype*_T174;union Cyc_YYSTYPE*_T175;union Cyc_YYSTYPE*_T176;struct Cyc_List_List*_T177;struct _tuple33*_T178;struct Cyc_Yystacktype*_T179;struct Cyc_Yystacktype _T17A;struct Cyc_Yyltype _T17B;unsigned _T17C;struct Cyc_Yystacktype*_T17D;union Cyc_YYSTYPE*_T17E;union Cyc_YYSTYPE*_T17F;struct Cyc_Yystacktype*_T180;union Cyc_YYSTYPE*_T181;union Cyc_YYSTYPE*_T182;struct Cyc_Yystacktype*_T183;struct Cyc_Yystacktype _T184;struct Cyc_List_List*_T185;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T186;struct Cyc_Yystacktype*_T187;union Cyc_YYSTYPE*_T188;union Cyc_YYSTYPE*_T189;void*_T18A;struct Cyc_Yystacktype*_T18B;struct Cyc_Yystacktype _T18C;struct Cyc_Yyltype _T18D;unsigned _T18E;unsigned _T18F;struct Cyc_Yystacktype*_T190;struct Cyc_Yystacktype _T191;struct _RegionHandle*_T192;struct Cyc_Yystacktype*_T193;union Cyc_YYSTYPE*_T194;union Cyc_YYSTYPE*_T195;struct Cyc_Parse_Declarator _T196;struct Cyc_Yystacktype*_T197;union Cyc_YYSTYPE*_T198;union Cyc_YYSTYPE*_T199;struct Cyc_Absyn_Stmt*_T19A;struct Cyc_Yystacktype*_T19B;struct Cyc_Yystacktype _T19C;struct Cyc_Yyltype _T19D;unsigned _T19E;unsigned _T19F;struct Cyc_Absyn_Fndecl*_T1A0;struct Cyc_Yystacktype*_T1A1;union Cyc_YYSTYPE*_T1A2;union Cyc_YYSTYPE*_T1A3;struct _RegionHandle*_T1A4;struct Cyc_Parse_Declaration_spec*_T1A5;struct Cyc_Parse_Declaration_spec*_T1A6;struct Cyc_Yystacktype*_T1A7;union Cyc_YYSTYPE*_T1A8;union Cyc_YYSTYPE*_T1A9;struct Cyc_Parse_Declarator _T1AA;struct Cyc_Yystacktype*_T1AB;union Cyc_YYSTYPE*_T1AC;union Cyc_YYSTYPE*_T1AD;struct Cyc_Absyn_Stmt*_T1AE;struct Cyc_Yystacktype*_T1AF;struct Cyc_Yystacktype _T1B0;struct Cyc_Yyltype _T1B1;unsigned _T1B2;unsigned _T1B3;struct Cyc_Absyn_Fndecl*_T1B4;struct _RegionHandle*_T1B5;struct Cyc_Yystacktype*_T1B6;union Cyc_YYSTYPE*_T1B7;union Cyc_YYSTYPE*_T1B8;struct Cyc_Parse_Declarator _T1B9;struct Cyc_Yystacktype*_T1BA;union Cyc_YYSTYPE*_T1BB;union Cyc_YYSTYPE*_T1BC;struct Cyc_List_List*_T1BD;struct Cyc_Yystacktype*_T1BE;union Cyc_YYSTYPE*_T1BF;union Cyc_YYSTYPE*_T1C0;struct Cyc_Absyn_Stmt*_T1C1;struct Cyc_Yystacktype*_T1C2;struct Cyc_Yystacktype _T1C3;struct Cyc_Yyltype _T1C4;unsigned _T1C5;unsigned _T1C6;struct Cyc_Absyn_Fndecl*_T1C7;struct Cyc_Yystacktype*_T1C8;union Cyc_YYSTYPE*_T1C9;union Cyc_YYSTYPE*_T1CA;struct _RegionHandle*_T1CB;struct Cyc_Parse_Declaration_spec*_T1CC;struct Cyc_Parse_Declaration_spec*_T1CD;struct Cyc_Yystacktype*_T1CE;union Cyc_YYSTYPE*_T1CF;union Cyc_YYSTYPE*_T1D0;struct Cyc_Parse_Declarator _T1D1;struct Cyc_Yystacktype*_T1D2;union Cyc_YYSTYPE*_T1D3;union Cyc_YYSTYPE*_T1D4;struct Cyc_List_List*_T1D5;struct Cyc_Yystacktype*_T1D6;union Cyc_YYSTYPE*_T1D7;union Cyc_YYSTYPE*_T1D8;struct Cyc_Absyn_Stmt*_T1D9;struct Cyc_Yystacktype*_T1DA;struct Cyc_Yystacktype _T1DB;struct Cyc_Yyltype _T1DC;unsigned _T1DD;unsigned _T1DE;struct Cyc_Absyn_Fndecl*_T1DF;struct Cyc_Yystacktype*_T1E0;union Cyc_YYSTYPE*_T1E1;union Cyc_YYSTYPE*_T1E2;struct _RegionHandle*_T1E3;struct Cyc_Parse_Declaration_spec*_T1E4;struct Cyc_Parse_Declaration_spec*_T1E5;struct Cyc_Yystacktype*_T1E6;union Cyc_YYSTYPE*_T1E7;union Cyc_YYSTYPE*_T1E8;struct Cyc_Parse_Declarator _T1E9;struct Cyc_Yystacktype*_T1EA;union Cyc_YYSTYPE*_T1EB;union Cyc_YYSTYPE*_T1EC;struct Cyc_Absyn_Stmt*_T1ED;struct Cyc_Yystacktype*_T1EE;struct Cyc_Yystacktype _T1EF;struct Cyc_Yyltype _T1F0;unsigned _T1F1;unsigned _T1F2;struct Cyc_Absyn_Fndecl*_T1F3;struct Cyc_Yystacktype*_T1F4;union Cyc_YYSTYPE*_T1F5;union Cyc_YYSTYPE*_T1F6;struct _RegionHandle*_T1F7;struct Cyc_Parse_Declaration_spec*_T1F8;struct Cyc_Parse_Declaration_spec*_T1F9;struct Cyc_Yystacktype*_T1FA;union Cyc_YYSTYPE*_T1FB;union Cyc_YYSTYPE*_T1FC;struct Cyc_Parse_Declarator _T1FD;struct Cyc_Yystacktype*_T1FE;union Cyc_YYSTYPE*_T1FF;union Cyc_YYSTYPE*_T200;struct Cyc_List_List*_T201;struct Cyc_Yystacktype*_T202;union Cyc_YYSTYPE*_T203;union Cyc_YYSTYPE*_T204;struct Cyc_Absyn_Stmt*_T205;struct Cyc_Yystacktype*_T206;struct Cyc_Yystacktype _T207;struct Cyc_Yyltype _T208;unsigned _T209;unsigned _T20A;struct Cyc_Absyn_Fndecl*_T20B;struct Cyc_Yystacktype*_T20C;union Cyc_YYSTYPE*_T20D;union Cyc_YYSTYPE*_T20E;struct _tuple0*_T20F;struct Cyc_Yystacktype*_T210;struct Cyc_Yystacktype _T211;struct _fat_ptr*_T212;struct Cyc_Yystacktype*_T213;union Cyc_YYSTYPE*_T214;union Cyc_YYSTYPE*_T215;struct Cyc_Yystacktype*_T216;struct Cyc_Yystacktype _T217;struct Cyc_Yystacktype*_T218;union Cyc_YYSTYPE*_T219;union Cyc_YYSTYPE*_T21A;struct Cyc_Parse_Declaration_spec _T21B;struct Cyc_Yystacktype*_T21C;struct Cyc_Yystacktype _T21D;struct Cyc_Yyltype _T21E;unsigned _T21F;unsigned _T220;struct Cyc_Yystacktype*_T221;struct Cyc_Yystacktype _T222;struct Cyc_Yyltype _T223;unsigned _T224;unsigned _T225;struct Cyc_List_List*_T226;struct Cyc_Yystacktype*_T227;union Cyc_YYSTYPE*_T228;union Cyc_YYSTYPE*_T229;struct Cyc_Parse_Declaration_spec _T22A;struct _tuple11*(*_T22B)(struct _tuple11*);struct Cyc_Yystacktype*_T22C;union Cyc_YYSTYPE*_T22D;union Cyc_YYSTYPE*_T22E;struct _tuple11*_T22F;struct _tuple11*_T230;struct Cyc_Yystacktype*_T231;struct Cyc_Yystacktype _T232;struct Cyc_Yyltype _T233;unsigned _T234;unsigned _T235;struct Cyc_Yystacktype*_T236;struct Cyc_Yystacktype _T237;struct Cyc_Yyltype _T238;unsigned _T239;unsigned _T23A;struct Cyc_List_List*_T23B;struct Cyc_List_List*_T23C;struct Cyc_Yystacktype*_T23D;union Cyc_YYSTYPE*_T23E;union Cyc_YYSTYPE*_T23F;struct Cyc_Absyn_Pat*_T240;struct Cyc_Yystacktype*_T241;union Cyc_YYSTYPE*_T242;union Cyc_YYSTYPE*_T243;struct Cyc_Absyn_Exp*_T244;struct Cyc_Yystacktype*_T245;struct Cyc_Yystacktype _T246;struct Cyc_Yyltype _T247;unsigned _T248;unsigned _T249;struct Cyc_Yystacktype*_T24A;union Cyc_YYSTYPE*_T24B;union Cyc_YYSTYPE*_T24C;struct _tuple0*_T24D;struct _tuple0*_T24E;struct Cyc_List_List*_T24F;void*_T250;struct Cyc_List_List*_T251;struct _tuple0*_T252;void*_T253;struct Cyc_List_List*_T254;struct Cyc_List_List*_T255;struct Cyc_List_List*_T256;struct Cyc_Yystacktype*_T257;struct Cyc_Yystacktype _T258;struct Cyc_Yyltype _T259;unsigned _T25A;unsigned _T25B;struct Cyc_Yystacktype*_T25C;union Cyc_YYSTYPE*_T25D;union Cyc_YYSTYPE*_T25E;struct _fat_ptr _T25F;struct Cyc_Yystacktype*_T260;struct Cyc_Yystacktype _T261;struct Cyc_Yyltype _T262;unsigned _T263;unsigned _T264;struct Cyc_Absyn_Tvar*_T265;struct _fat_ptr*_T266;struct Cyc_Yystacktype*_T267;union Cyc_YYSTYPE*_T268;union Cyc_YYSTYPE*_T269;struct Cyc_Absyn_Tvar*_T26A;struct Cyc_Absyn_Tvar*_T26B;struct Cyc_Absyn_Kind*_T26C;struct Cyc_Absyn_Kind*_T26D;struct Cyc_Absyn_Tvar*_T26E;struct Cyc_Yystacktype*_T26F;struct Cyc_Yystacktype _T270;struct Cyc_Yyltype _T271;unsigned _T272;unsigned _T273;struct _tuple0*_T274;struct _fat_ptr*_T275;struct Cyc_Yystacktype*_T276;union Cyc_YYSTYPE*_T277;union Cyc_YYSTYPE*_T278;void*_T279;struct Cyc_List_List*_T27A;struct Cyc_Absyn_Tvar*_T27B;struct Cyc_Absyn_Vardecl*_T27C;struct Cyc_Yystacktype*_T27D;struct Cyc_Yystacktype _T27E;struct Cyc_Yyltype _T27F;unsigned _T280;unsigned _T281;struct _fat_ptr _T282;struct Cyc_String_pa_PrintArg_struct _T283;struct Cyc_Yystacktype*_T284;union Cyc_YYSTYPE*_T285;union Cyc_YYSTYPE*_T286;struct _fat_ptr _T287;struct _fat_ptr _T288;struct Cyc_Yystacktype*_T289;union Cyc_YYSTYPE*_T28A;union Cyc_YYSTYPE*_T28B;struct _fat_ptr _T28C;struct Cyc_Yystacktype*_T28D;struct Cyc_Yystacktype _T28E;struct Cyc_Yyltype _T28F;unsigned _T290;unsigned _T291;struct Cyc_Absyn_Tvar*_T292;struct _fat_ptr*_T293;struct Cyc_Absyn_Tvar*_T294;struct Cyc_Absyn_Tvar*_T295;struct Cyc_Absyn_Kind*_T296;struct Cyc_Absyn_Kind*_T297;struct Cyc_Absyn_Tvar*_T298;struct Cyc_Yystacktype*_T299;struct Cyc_Yystacktype _T29A;struct Cyc_Yyltype _T29B;unsigned _T29C;unsigned _T29D;struct _tuple0*_T29E;struct _fat_ptr*_T29F;struct Cyc_Yystacktype*_T2A0;union Cyc_YYSTYPE*_T2A1;union Cyc_YYSTYPE*_T2A2;void*_T2A3;struct Cyc_List_List*_T2A4;struct Cyc_Absyn_Tvar*_T2A5;struct Cyc_Absyn_Vardecl*_T2A6;struct Cyc_Yystacktype*_T2A7;union Cyc_YYSTYPE*_T2A8;union Cyc_YYSTYPE*_T2A9;struct Cyc_Absyn_Exp*_T2AA;struct Cyc_Yystacktype*_T2AB;struct Cyc_Yystacktype _T2AC;struct Cyc_Yyltype _T2AD;unsigned _T2AE;unsigned _T2AF;struct Cyc_Yystacktype*_T2B0;union Cyc_YYSTYPE*_T2B1;union Cyc_YYSTYPE*_T2B2;struct _fat_ptr _T2B3;struct _fat_ptr _T2B4;int _T2B5;struct Cyc_Yystacktype*_T2B6;struct Cyc_Yystacktype _T2B7;struct Cyc_Yyltype _T2B8;unsigned _T2B9;unsigned _T2BA;struct _fat_ptr _T2BB;struct _fat_ptr _T2BC;struct Cyc_Yystacktype*_T2BD;union Cyc_YYSTYPE*_T2BE;union Cyc_YYSTYPE*_T2BF;struct Cyc_Absyn_Exp*_T2C0;struct Cyc_Yystacktype*_T2C1;struct Cyc_Yystacktype _T2C2;struct Cyc_Yystacktype*_T2C3;union Cyc_YYSTYPE*_T2C4;union Cyc_YYSTYPE*_T2C5;struct Cyc_List_List*_T2C6;struct Cyc_Yystacktype*_T2C7;union Cyc_YYSTYPE*_T2C8;union Cyc_YYSTYPE*_T2C9;struct Cyc_List_List*_T2CA;struct Cyc_List_List*_T2CB;struct Cyc_Parse_Declaration_spec _T2CC;struct Cyc_Yystacktype*_T2CD;union Cyc_YYSTYPE*_T2CE;union Cyc_YYSTYPE*_T2CF;struct Cyc_Yystacktype*_T2D0;struct Cyc_Yystacktype _T2D1;struct Cyc_Yyltype _T2D2;unsigned _T2D3;unsigned _T2D4;struct Cyc_Yystacktype*_T2D5;union Cyc_YYSTYPE*_T2D6;union Cyc_YYSTYPE*_T2D7;struct Cyc_Parse_Declaration_spec _T2D8;enum Cyc_Parse_Storage_class _T2D9;int _T2DA;struct Cyc_Yystacktype*_T2DB;struct Cyc_Yystacktype _T2DC;struct Cyc_Yyltype _T2DD;unsigned _T2DE;unsigned _T2DF;struct _fat_ptr _T2E0;struct _fat_ptr _T2E1;struct Cyc_Parse_Declaration_spec _T2E2;struct Cyc_Yystacktype*_T2E3;union Cyc_YYSTYPE*_T2E4;union Cyc_YYSTYPE*_T2E5;struct Cyc_Parse_Declaration_spec _T2E6;struct Cyc_Parse_Declaration_spec _T2E7;struct Cyc_Parse_Declaration_spec _T2E8;struct Cyc_Parse_Declaration_spec _T2E9;struct Cyc_Yystacktype*_T2EA;struct Cyc_Yystacktype _T2EB;struct Cyc_Yyltype _T2EC;unsigned _T2ED;unsigned _T2EE;struct _fat_ptr _T2EF;struct _fat_ptr _T2F0;struct Cyc_Yystacktype*_T2F1;struct Cyc_Yystacktype _T2F2;struct Cyc_Parse_Declaration_spec _T2F3;struct Cyc_Yystacktype*_T2F4;struct Cyc_Yystacktype _T2F5;struct Cyc_Yyltype _T2F6;unsigned _T2F7;unsigned _T2F8;struct Cyc_Yystacktype*_T2F9;union Cyc_YYSTYPE*_T2FA;union Cyc_YYSTYPE*_T2FB;struct Cyc_Yystacktype*_T2FC;union Cyc_YYSTYPE*_T2FD;union Cyc_YYSTYPE*_T2FE;struct Cyc_Parse_Declaration_spec _T2FF;struct Cyc_Parse_Declaration_spec _T300;struct Cyc_Parse_Declaration_spec _T301;struct Cyc_Yystacktype*_T302;struct Cyc_Yystacktype _T303;struct Cyc_Yyltype _T304;unsigned _T305;unsigned _T306;struct Cyc_Parse_Declaration_spec _T307;struct Cyc_Parse_Type_specifier _T308;struct Cyc_Yystacktype*_T309;union Cyc_YYSTYPE*_T30A;union Cyc_YYSTYPE*_T30B;struct Cyc_Parse_Type_specifier _T30C;struct Cyc_Parse_Declaration_spec _T30D;struct Cyc_Parse_Declaration_spec _T30E;struct Cyc_Parse_Declaration_spec _T30F;struct Cyc_Yystacktype*_T310;union Cyc_YYSTYPE*_T311;union Cyc_YYSTYPE*_T312;struct Cyc_Yystacktype*_T313;union Cyc_YYSTYPE*_T314;union Cyc_YYSTYPE*_T315;struct Cyc_Parse_Declaration_spec _T316;struct Cyc_Parse_Declaration_spec _T317;struct Cyc_Yystacktype*_T318;union Cyc_YYSTYPE*_T319;union Cyc_YYSTYPE*_T31A;struct Cyc_Absyn_Tqual _T31B;struct Cyc_Parse_Declaration_spec _T31C;struct Cyc_Absyn_Tqual _T31D;struct Cyc_Parse_Declaration_spec _T31E;struct Cyc_Parse_Declaration_spec _T31F;struct Cyc_Parse_Declaration_spec _T320;struct Cyc_Parse_Declaration_spec _T321;struct Cyc_Yystacktype*_T322;struct Cyc_Yystacktype _T323;struct Cyc_Yyltype _T324;unsigned _T325;unsigned _T326;struct Cyc_Yystacktype*_T327;union Cyc_YYSTYPE*_T328;union Cyc_YYSTYPE*_T329;struct Cyc_Parse_Declaration_spec _T32A;struct Cyc_Parse_Declaration_spec _T32B;struct Cyc_Parse_Declaration_spec _T32C;struct Cyc_Parse_Declaration_spec _T32D;struct Cyc_Parse_Declaration_spec _T32E;struct Cyc_Parse_Declaration_spec _T32F;struct Cyc_Yystacktype*_T330;struct Cyc_Yystacktype _T331;struct Cyc_Yyltype _T332;unsigned _T333;unsigned _T334;struct Cyc_Yystacktype*_T335;union Cyc_YYSTYPE*_T336;union Cyc_YYSTYPE*_T337;struct Cyc_Yystacktype*_T338;union Cyc_YYSTYPE*_T339;union Cyc_YYSTYPE*_T33A;struct Cyc_Parse_Declaration_spec _T33B;struct Cyc_Parse_Declaration_spec _T33C;struct Cyc_Parse_Declaration_spec _T33D;struct Cyc_Parse_Declaration_spec _T33E;struct Cyc_Parse_Declaration_spec _T33F;struct Cyc_Yystacktype*_T340;union Cyc_YYSTYPE*_T341;union Cyc_YYSTYPE*_T342;struct Cyc_List_List*_T343;struct Cyc_Parse_Declaration_spec _T344;struct Cyc_List_List*_T345;struct Cyc_Yystacktype*_T346;union Cyc_YYSTYPE*_T347;union Cyc_YYSTYPE*_T348;struct _fat_ptr _T349;struct _fat_ptr _T34A;int _T34B;struct Cyc_Yystacktype*_T34C;struct Cyc_Yystacktype _T34D;struct Cyc_Yyltype _T34E;unsigned _T34F;unsigned _T350;struct _fat_ptr _T351;struct _fat_ptr _T352;struct Cyc_Yystacktype*_T353;struct Cyc_Yystacktype _T354;struct Cyc_Yystacktype*_T355;struct Cyc_Yystacktype _T356;struct Cyc_List_List*_T357;struct Cyc_Yystacktype*_T358;union Cyc_YYSTYPE*_T359;union Cyc_YYSTYPE*_T35A;struct Cyc_List_List*_T35B;struct Cyc_Yystacktype*_T35C;union Cyc_YYSTYPE*_T35D;union Cyc_YYSTYPE*_T35E;struct Cyc_Yystacktype*_T35F;union Cyc_YYSTYPE*_T360;union Cyc_YYSTYPE*_T361;struct Cyc_Yystacktype*_T362;struct Cyc_Yystacktype _T363;struct Cyc_Yyltype _T364;unsigned _T365;unsigned _T366;struct Cyc_Yystacktype*_T367;union Cyc_YYSTYPE*_T368;union Cyc_YYSTYPE*_T369;struct _fat_ptr _T36A;void*_T36B;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T36C;struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_T36D;void*_T36E;struct Cyc_Yystacktype*_T36F;struct Cyc_Yystacktype _T370;struct Cyc_Yyltype _T371;unsigned _T372;unsigned _T373;struct Cyc_Yystacktype*_T374;union Cyc_YYSTYPE*_T375;union Cyc_YYSTYPE*_T376;struct _fat_ptr _T377;struct Cyc_Yystacktype*_T378;struct Cyc_Yystacktype _T379;struct Cyc_Yyltype _T37A;unsigned _T37B;unsigned _T37C;struct Cyc_Yystacktype*_T37D;union Cyc_YYSTYPE*_T37E;union Cyc_YYSTYPE*_T37F;struct Cyc_Absyn_Exp*_T380;void*_T381;struct Cyc_Yystacktype*_T382;struct Cyc_Yystacktype _T383;struct Cyc_Yyltype _T384;unsigned _T385;unsigned _T386;struct Cyc_Yystacktype*_T387;struct Cyc_Yystacktype _T388;struct Cyc_Yyltype _T389;unsigned _T38A;unsigned _T38B;struct Cyc_Yystacktype*_T38C;union Cyc_YYSTYPE*_T38D;union Cyc_YYSTYPE*_T38E;struct _fat_ptr _T38F;struct Cyc_Yystacktype*_T390;union Cyc_YYSTYPE*_T391;union Cyc_YYSTYPE*_T392;struct _fat_ptr _T393;struct Cyc_Yystacktype*_T394;struct Cyc_Yystacktype _T395;struct Cyc_Yyltype _T396;unsigned _T397;unsigned _T398;struct Cyc_Yystacktype*_T399;union Cyc_YYSTYPE*_T39A;union Cyc_YYSTYPE*_T39B;union Cyc_Absyn_Cnst _T39C;unsigned _T39D;struct Cyc_Yystacktype*_T39E;struct Cyc_Yystacktype _T39F;struct Cyc_Yyltype _T3A0;unsigned _T3A1;unsigned _T3A2;struct Cyc_Yystacktype*_T3A3;union Cyc_YYSTYPE*_T3A4;union Cyc_YYSTYPE*_T3A5;union Cyc_Absyn_Cnst _T3A6;unsigned _T3A7;void*_T3A8;struct Cyc_Yystacktype*_T3A9;struct Cyc_Yystacktype _T3AA;struct Cyc_Yystacktype*_T3AB;union Cyc_YYSTYPE*_T3AC;union Cyc_YYSTYPE*_T3AD;struct _tuple0*_T3AE;struct Cyc_Yystacktype*_T3AF;union Cyc_YYSTYPE*_T3B0;union Cyc_YYSTYPE*_T3B1;struct Cyc_List_List*_T3B2;void*_T3B3;struct Cyc_Yystacktype*_T3B4;struct Cyc_Yystacktype _T3B5;struct Cyc_Yyltype _T3B6;unsigned _T3B7;unsigned _T3B8;struct Cyc_Parse_Type_specifier _T3B9;void*_T3BA;struct Cyc_Yystacktype*_T3BB;struct Cyc_Yystacktype _T3BC;struct Cyc_Yyltype _T3BD;unsigned _T3BE;unsigned _T3BF;struct Cyc_Parse_Type_specifier _T3C0;void*_T3C1;struct Cyc_Yystacktype*_T3C2;struct Cyc_Yystacktype _T3C3;struct Cyc_Yyltype _T3C4;unsigned _T3C5;unsigned _T3C6;struct Cyc_Parse_Type_specifier _T3C7;struct Cyc_Yystacktype*_T3C8;struct Cyc_Yystacktype _T3C9;struct Cyc_Yyltype _T3CA;unsigned _T3CB;unsigned _T3CC;struct Cyc_Parse_Type_specifier _T3CD;void*_T3CE;struct Cyc_Yystacktype*_T3CF;struct Cyc_Yystacktype _T3D0;struct Cyc_Yyltype _T3D1;unsigned _T3D2;unsigned _T3D3;struct Cyc_Parse_Type_specifier _T3D4;struct Cyc_Yystacktype*_T3D5;struct Cyc_Yystacktype _T3D6;struct Cyc_Yyltype _T3D7;unsigned _T3D8;unsigned _T3D9;struct Cyc_Parse_Type_specifier _T3DA;void*_T3DB;struct Cyc_Yystacktype*_T3DC;struct Cyc_Yystacktype _T3DD;struct Cyc_Yyltype _T3DE;unsigned _T3DF;unsigned _T3E0;struct Cyc_Parse_Type_specifier _T3E1;void*_T3E2;struct Cyc_Yystacktype*_T3E3;struct Cyc_Yystacktype _T3E4;struct Cyc_Yyltype _T3E5;unsigned _T3E6;unsigned _T3E7;struct Cyc_Parse_Type_specifier _T3E8;struct Cyc_Yystacktype*_T3E9;struct Cyc_Yystacktype _T3EA;struct Cyc_Yyltype _T3EB;unsigned _T3EC;unsigned _T3ED;struct Cyc_Parse_Type_specifier _T3EE;struct Cyc_Yystacktype*_T3EF;struct Cyc_Yystacktype _T3F0;struct Cyc_Yyltype _T3F1;unsigned _T3F2;unsigned _T3F3;struct Cyc_Parse_Type_specifier _T3F4;struct Cyc_Yystacktype*_T3F5;struct Cyc_Yystacktype _T3F6;struct Cyc_Yyltype _T3F7;unsigned _T3F8;unsigned _T3F9;struct Cyc_Parse_Type_specifier _T3FA;struct Cyc_Yystacktype*_T3FB;struct Cyc_Yystacktype _T3FC;struct Cyc_Yystacktype*_T3FD;struct Cyc_Yystacktype _T3FE;struct Cyc_Yystacktype*_T3FF;union Cyc_YYSTYPE*_T400;union Cyc_YYSTYPE*_T401;struct Cyc_Absyn_Exp*_T402;void*_T403;struct Cyc_Yystacktype*_T404;struct Cyc_Yystacktype _T405;struct Cyc_Yyltype _T406;unsigned _T407;unsigned _T408;struct Cyc_Parse_Type_specifier _T409;struct _fat_ptr _T40A;struct Cyc_Absyn_Kind*_T40B;struct Cyc_Absyn_Kind*_T40C;void*_T40D;struct Cyc_Yystacktype*_T40E;struct Cyc_Yystacktype _T40F;struct Cyc_Yyltype _T410;unsigned _T411;unsigned _T412;struct Cyc_Parse_Type_specifier _T413;struct Cyc_Yystacktype*_T414;struct Cyc_Yystacktype _T415;struct Cyc_Yystacktype*_T416;union Cyc_YYSTYPE*_T417;union Cyc_YYSTYPE*_T418;void*_T419;struct Cyc_Yystacktype*_T41A;struct Cyc_Yystacktype _T41B;struct Cyc_Yyltype _T41C;unsigned _T41D;unsigned _T41E;struct Cyc_Parse_Type_specifier _T41F;void*_T420;struct Cyc_Yystacktype*_T421;struct Cyc_Yystacktype _T422;struct Cyc_Yyltype _T423;unsigned _T424;unsigned _T425;struct Cyc_Parse_Type_specifier _T426;struct Cyc_Yystacktype*_T427;union Cyc_YYSTYPE*_T428;union Cyc_YYSTYPE*_T429;struct Cyc_Absyn_Kind*_T42A;struct Cyc_Core_Opt*_T42B;void*_T42C;struct Cyc_Yystacktype*_T42D;struct Cyc_Yystacktype _T42E;struct Cyc_Yyltype _T42F;unsigned _T430;unsigned _T431;struct Cyc_Parse_Type_specifier _T432;struct Cyc_List_List*(*_T433)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T434)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T435)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T436;struct Cyc_Yystacktype _T437;struct Cyc_Yyltype _T438;unsigned _T439;unsigned _T43A;struct Cyc_Yystacktype*_T43B;union Cyc_YYSTYPE*_T43C;union Cyc_YYSTYPE*_T43D;struct Cyc_List_List*_T43E;struct Cyc_List_List*_T43F;struct Cyc_List_List*_T440;void*_T441;struct Cyc_Yystacktype*_T442;struct Cyc_Yystacktype _T443;struct Cyc_Yyltype _T444;unsigned _T445;unsigned _T446;struct Cyc_Parse_Type_specifier _T447;struct Cyc_Yystacktype*_T448;union Cyc_YYSTYPE*_T449;union Cyc_YYSTYPE*_T44A;void*_T44B;void*_T44C;struct Cyc_Yystacktype*_T44D;struct Cyc_Yystacktype _T44E;struct Cyc_Yyltype _T44F;unsigned _T450;unsigned _T451;struct Cyc_Parse_Type_specifier _T452;struct Cyc_Core_Opt*_T453;struct Cyc_Core_Opt*_T454;void*_T455;void*_T456;struct Cyc_Yystacktype*_T457;struct Cyc_Yystacktype _T458;struct Cyc_Yyltype _T459;unsigned _T45A;unsigned _T45B;struct Cyc_Parse_Type_specifier _T45C;struct Cyc_Yystacktype*_T45D;union Cyc_YYSTYPE*_T45E;union Cyc_YYSTYPE*_T45F;void*_T460;void*_T461;struct Cyc_Yystacktype*_T462;struct Cyc_Yystacktype _T463;struct Cyc_Yyltype _T464;unsigned _T465;unsigned _T466;struct Cyc_Parse_Type_specifier _T467;struct Cyc_Yystacktype*_T468;union Cyc_YYSTYPE*_T469;union Cyc_YYSTYPE*_T46A;void*_T46B;void*_T46C;struct Cyc_Yystacktype*_T46D;struct Cyc_Yystacktype _T46E;struct Cyc_Yyltype _T46F;unsigned _T470;unsigned _T471;struct Cyc_Parse_Type_specifier _T472;struct Cyc_Core_Opt*_T473;struct Cyc_Core_Opt*_T474;void*_T475;void*_T476;struct Cyc_Yystacktype*_T477;struct Cyc_Yystacktype _T478;struct Cyc_Yyltype _T479;unsigned _T47A;unsigned _T47B;struct Cyc_Parse_Type_specifier _T47C;struct Cyc_Yystacktype*_T47D;union Cyc_YYSTYPE*_T47E;union Cyc_YYSTYPE*_T47F;struct Cyc_Absyn_Exp*_T480;void*_T481;struct Cyc_Yystacktype*_T482;struct Cyc_Yystacktype _T483;struct Cyc_Yyltype _T484;unsigned _T485;unsigned _T486;struct Cyc_Parse_Type_specifier _T487;struct Cyc_Yystacktype*_T488;union Cyc_YYSTYPE*_T489;union Cyc_YYSTYPE*_T48A;struct _fat_ptr _T48B;struct Cyc_Yystacktype*_T48C;struct Cyc_Yystacktype _T48D;struct Cyc_Yyltype _T48E;unsigned _T48F;unsigned _T490;struct Cyc_Absyn_Kind*_T491;unsigned _T492;int _T493;struct Cyc_Yystacktype*_T494;struct Cyc_Yystacktype _T495;struct Cyc_Yyltype _T496;unsigned _T497;struct Cyc_Absyn_Tqual _T498;struct Cyc_Absyn_Tqual _T499;struct Cyc_Absyn_Tqual _T49A;struct Cyc_Absyn_TypeDecl*_T49B;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T49C;struct Cyc_Absyn_Enumdecl*_T49D;struct Cyc_Yystacktype*_T49E;union Cyc_YYSTYPE*_T49F;union Cyc_YYSTYPE*_T4A0;struct Cyc_Core_Opt*_T4A1;struct Cyc_Yystacktype*_T4A2;union Cyc_YYSTYPE*_T4A3;union Cyc_YYSTYPE*_T4A4;struct Cyc_Absyn_TypeDecl*_T4A5;struct Cyc_Yystacktype*_T4A6;struct Cyc_Yystacktype _T4A7;struct Cyc_Yyltype _T4A8;unsigned _T4A9;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T4AA;void*_T4AB;struct Cyc_Yystacktype*_T4AC;struct Cyc_Yystacktype _T4AD;struct Cyc_Yyltype _T4AE;unsigned _T4AF;unsigned _T4B0;struct Cyc_Parse_Type_specifier _T4B1;struct Cyc_Yystacktype*_T4B2;union Cyc_YYSTYPE*_T4B3;union Cyc_YYSTYPE*_T4B4;struct _tuple0*_T4B5;void*_T4B6;struct Cyc_Yystacktype*_T4B7;struct Cyc_Yystacktype _T4B8;struct Cyc_Yyltype _T4B9;unsigned _T4BA;unsigned _T4BB;struct Cyc_Parse_Type_specifier _T4BC;struct Cyc_Yystacktype*_T4BD;union Cyc_YYSTYPE*_T4BE;union Cyc_YYSTYPE*_T4BF;struct Cyc_List_List*_T4C0;void*_T4C1;struct Cyc_Yystacktype*_T4C2;struct Cyc_Yystacktype _T4C3;struct Cyc_Yyltype _T4C4;unsigned _T4C5;unsigned _T4C6;struct Cyc_Parse_Type_specifier _T4C7;struct Cyc_Absyn_Enumfield*_T4C8;struct Cyc_Yystacktype*_T4C9;union Cyc_YYSTYPE*_T4CA;union Cyc_YYSTYPE*_T4CB;struct Cyc_Yystacktype*_T4CC;struct Cyc_Yystacktype _T4CD;struct Cyc_Yyltype _T4CE;unsigned _T4CF;struct Cyc_Absyn_Enumfield*_T4D0;struct Cyc_Yystacktype*_T4D1;union Cyc_YYSTYPE*_T4D2;union Cyc_YYSTYPE*_T4D3;struct Cyc_Yystacktype*_T4D4;union Cyc_YYSTYPE*_T4D5;union Cyc_YYSTYPE*_T4D6;struct Cyc_Yystacktype*_T4D7;struct Cyc_Yystacktype _T4D8;struct Cyc_Yyltype _T4D9;unsigned _T4DA;struct Cyc_List_List*_T4DB;struct Cyc_Yystacktype*_T4DC;union Cyc_YYSTYPE*_T4DD;union Cyc_YYSTYPE*_T4DE;struct Cyc_List_List*_T4DF;struct Cyc_Yystacktype*_T4E0;union Cyc_YYSTYPE*_T4E1;union Cyc_YYSTYPE*_T4E2;struct Cyc_List_List*_T4E3;struct Cyc_Yystacktype*_T4E4;union Cyc_YYSTYPE*_T4E5;union Cyc_YYSTYPE*_T4E6;struct Cyc_Yystacktype*_T4E7;union Cyc_YYSTYPE*_T4E8;union Cyc_YYSTYPE*_T4E9;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T4EA;struct Cyc_Yystacktype*_T4EB;union Cyc_YYSTYPE*_T4EC;union Cyc_YYSTYPE*_T4ED;struct Cyc_Yystacktype*_T4EE;union Cyc_YYSTYPE*_T4EF;union Cyc_YYSTYPE*_T4F0;void*_T4F1;struct Cyc_Yystacktype*_T4F2;struct Cyc_Yystacktype _T4F3;struct Cyc_Yyltype _T4F4;unsigned _T4F5;unsigned _T4F6;struct Cyc_Parse_Type_specifier _T4F7;struct Cyc_List_List*(*_T4F8)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T4F9)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T4FA;struct Cyc_Yystacktype _T4FB;struct Cyc_Yyltype _T4FC;unsigned _T4FD;unsigned _T4FE;struct Cyc_Yystacktype*_T4FF;union Cyc_YYSTYPE*_T500;union Cyc_YYSTYPE*_T501;struct Cyc_List_List*_T502;struct Cyc_List_List*(*_T503)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T504)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T505;struct Cyc_Yystacktype _T506;struct Cyc_Yyltype _T507;unsigned _T508;unsigned _T509;struct Cyc_Yystacktype*_T50A;union Cyc_YYSTYPE*_T50B;union Cyc_YYSTYPE*_T50C;struct Cyc_List_List*_T50D;struct Cyc_Yystacktype*_T50E;union Cyc_YYSTYPE*_T50F;union Cyc_YYSTYPE*_T510;struct _tuple28 _T511;struct _tuple28*_T512;unsigned _T513;struct _tuple28*_T514;struct _tuple28 _T515;struct Cyc_Yystacktype*_T516;union Cyc_YYSTYPE*_T517;union Cyc_YYSTYPE*_T518;struct _tuple25 _T519;enum Cyc_Absyn_AggrKind _T51A;struct Cyc_Yystacktype*_T51B;union Cyc_YYSTYPE*_T51C;union Cyc_YYSTYPE*_T51D;struct _tuple0*_T51E;struct Cyc_List_List*_T51F;struct Cyc_List_List*_T520;struct Cyc_List_List*_T521;struct Cyc_List_List*_T522;struct Cyc_Yystacktype*_T523;union Cyc_YYSTYPE*_T524;union Cyc_YYSTYPE*_T525;struct Cyc_List_List*_T526;struct Cyc_Yystacktype*_T527;union Cyc_YYSTYPE*_T528;union Cyc_YYSTYPE*_T529;struct _tuple25 _T52A;int _T52B;struct Cyc_Absyn_AggrdeclImpl*_T52C;struct Cyc_Yystacktype*_T52D;struct Cyc_Yystacktype _T52E;struct Cyc_Yyltype _T52F;unsigned _T530;unsigned _T531;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T532;void*_T533;struct Cyc_Yystacktype*_T534;struct Cyc_Yystacktype _T535;struct Cyc_Yyltype _T536;unsigned _T537;unsigned _T538;struct Cyc_Parse_Type_specifier _T539;struct Cyc_Yystacktype*_T53A;union Cyc_YYSTYPE*_T53B;union Cyc_YYSTYPE*_T53C;struct _tuple25 _T53D;enum Cyc_Absyn_AggrKind _T53E;struct Cyc_Yystacktype*_T53F;union Cyc_YYSTYPE*_T540;union Cyc_YYSTYPE*_T541;struct _tuple0*_T542;struct Cyc_Core_Opt*_T543;struct Cyc_Yystacktype*_T544;union Cyc_YYSTYPE*_T545;union Cyc_YYSTYPE*_T546;struct _tuple25 _T547;int _T548;struct Cyc_Core_Opt*_T549;union Cyc_Absyn_AggrInfo _T54A;struct Cyc_Yystacktype*_T54B;union Cyc_YYSTYPE*_T54C;union Cyc_YYSTYPE*_T54D;struct Cyc_List_List*_T54E;void*_T54F;struct Cyc_Yystacktype*_T550;struct Cyc_Yystacktype _T551;struct Cyc_Yyltype _T552;unsigned _T553;unsigned _T554;struct Cyc_Parse_Type_specifier _T555;struct _tuple25 _T556;struct Cyc_Yystacktype*_T557;union Cyc_YYSTYPE*_T558;union Cyc_YYSTYPE*_T559;struct _tuple25 _T55A;struct Cyc_Yystacktype*_T55B;union Cyc_YYSTYPE*_T55C;union Cyc_YYSTYPE*_T55D;struct Cyc_Yystacktype*_T55E;union Cyc_YYSTYPE*_T55F;union Cyc_YYSTYPE*_T560;struct Cyc_List_List*_T561;struct Cyc_List_List*_T562;struct Cyc_Yystacktype*_T563;union Cyc_YYSTYPE*_T564;union Cyc_YYSTYPE*_T565;struct Cyc_List_List*_T566;void*_T567;struct Cyc_List_List*_T568;struct Cyc_List_List*_T569;struct Cyc_List_List*_T56A;void(*_T56B)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*);void(*_T56C)(void(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_List_List*_T56D;struct Cyc_List_List*_T56E;struct Cyc_List_List*_T56F;struct Cyc_Yystacktype*_T570;union Cyc_YYSTYPE*_T571;union Cyc_YYSTYPE*_T572;struct Cyc_List_List*_T573;struct Cyc_Yystacktype*_T574;union Cyc_YYSTYPE*_T575;union Cyc_YYSTYPE*_T576;struct Cyc_Yystacktype*_T577;union Cyc_YYSTYPE*_T578;union Cyc_YYSTYPE*_T579;struct _tuple11*_T57A;struct _RegionHandle*_T57B;struct Cyc_Yystacktype*_T57C;union Cyc_YYSTYPE*_T57D;union Cyc_YYSTYPE*_T57E;struct _tuple11*_T57F;struct _RegionHandle*_T580;struct Cyc_Yystacktype*_T581;union Cyc_YYSTYPE*_T582;union Cyc_YYSTYPE*_T583;struct Cyc_Yystacktype*_T584;union Cyc_YYSTYPE*_T585;union Cyc_YYSTYPE*_T586;struct _tuple12 _T587;struct Cyc_Yystacktype*_T588;union Cyc_YYSTYPE*_T589;union Cyc_YYSTYPE*_T58A;struct _tuple12 _T58B;struct Cyc_Yystacktype*_T58C;union Cyc_YYSTYPE*_T58D;union Cyc_YYSTYPE*_T58E;struct Cyc_Yystacktype*_T58F;union Cyc_YYSTYPE*_T590;union Cyc_YYSTYPE*_T591;void*_T592;struct Cyc_Yystacktype*_T593;struct Cyc_Yystacktype _T594;struct Cyc_Yyltype _T595;unsigned _T596;unsigned _T597;struct _tuple12 _T598;struct Cyc_Yystacktype*_T599;union Cyc_YYSTYPE*_T59A;union Cyc_YYSTYPE*_T59B;struct Cyc_Yystacktype*_T59C;union Cyc_YYSTYPE*_T59D;union Cyc_YYSTYPE*_T59E;struct _tuple12 _T59F;struct Cyc_Yystacktype*_T5A0;union Cyc_YYSTYPE*_T5A1;union Cyc_YYSTYPE*_T5A2;struct Cyc_Yystacktype*_T5A3;union Cyc_YYSTYPE*_T5A4;union Cyc_YYSTYPE*_T5A5;struct Cyc_Yystacktype*_T5A6;union Cyc_YYSTYPE*_T5A7;union Cyc_YYSTYPE*_T5A8;void*_T5A9;struct Cyc_Yystacktype*_T5AA;struct Cyc_Yystacktype _T5AB;struct Cyc_Yyltype _T5AC;unsigned _T5AD;unsigned _T5AE;struct Cyc_Yystacktype*_T5AF;union Cyc_YYSTYPE*_T5B0;union Cyc_YYSTYPE*_T5B1;struct Cyc_Absyn_Tqual _T5B2;unsigned _T5B3;struct Cyc_Yystacktype*_T5B4;struct Cyc_Yystacktype _T5B5;struct Cyc_Yyltype _T5B6;unsigned _T5B7;struct Cyc_Yystacktype*_T5B8;union Cyc_YYSTYPE*_T5B9;union Cyc_YYSTYPE*_T5BA;struct Cyc_List_List*_T5BB;void*_T5BC;struct _tuple13*_T5BD;struct _RegionHandle*_T5BE;struct Cyc_List_List*_T5BF;struct _RegionHandle*_T5C0;struct _tuple16*_T5C1;struct _RegionHandle*_T5C2;struct Cyc_List_List*_T5C3;struct Cyc_Parse_Type_specifier _T5C4;struct Cyc_Yystacktype*_T5C5;struct Cyc_Yystacktype _T5C6;struct Cyc_Yyltype _T5C7;unsigned _T5C8;unsigned _T5C9;struct _RegionHandle*_T5CA;struct _RegionHandle*_T5CB;struct Cyc_List_List*_T5CC;struct Cyc_List_List*_T5CD;struct Cyc_List_List*(*_T5CE)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T5CF)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T5D0;struct Cyc_Yystacktype _T5D1;struct Cyc_Yyltype _T5D2;unsigned _T5D3;unsigned _T5D4;struct Cyc_List_List*_T5D5;struct Cyc_List_List*_T5D6;struct _tuple26 _T5D7;struct Cyc_Yystacktype*_T5D8;struct Cyc_Yystacktype _T5D9;struct Cyc_Yyltype _T5DA;unsigned _T5DB;unsigned _T5DC;struct Cyc_Yystacktype*_T5DD;union Cyc_YYSTYPE*_T5DE;union Cyc_YYSTYPE*_T5DF;struct Cyc_Yystacktype*_T5E0;union Cyc_YYSTYPE*_T5E1;union Cyc_YYSTYPE*_T5E2;struct _tuple26 _T5E3;struct _tuple26 _T5E4;struct Cyc_Yystacktype*_T5E5;struct Cyc_Yystacktype _T5E6;struct Cyc_Yyltype _T5E7;unsigned _T5E8;unsigned _T5E9;struct Cyc_Yystacktype*_T5EA;union Cyc_YYSTYPE*_T5EB;union Cyc_YYSTYPE*_T5EC;struct Cyc_Parse_Type_specifier _T5ED;struct _tuple26 _T5EE;struct Cyc_Parse_Type_specifier _T5EF;struct _tuple26 _T5F0;struct _tuple26 _T5F1;struct Cyc_Yystacktype*_T5F2;union Cyc_YYSTYPE*_T5F3;union Cyc_YYSTYPE*_T5F4;struct Cyc_Yystacktype*_T5F5;union Cyc_YYSTYPE*_T5F6;union Cyc_YYSTYPE*_T5F7;struct _tuple26 _T5F8;struct Cyc_Yystacktype*_T5F9;union Cyc_YYSTYPE*_T5FA;union Cyc_YYSTYPE*_T5FB;struct Cyc_Absyn_Tqual _T5FC;struct _tuple26 _T5FD;struct Cyc_Absyn_Tqual _T5FE;struct _tuple26 _T5FF;struct _tuple26 _T600;struct _tuple26 _T601;struct Cyc_Yystacktype*_T602;struct Cyc_Yystacktype _T603;struct Cyc_Yyltype _T604;unsigned _T605;unsigned _T606;struct Cyc_Yystacktype*_T607;union Cyc_YYSTYPE*_T608;union Cyc_YYSTYPE*_T609;struct Cyc_Yystacktype*_T60A;union Cyc_YYSTYPE*_T60B;union Cyc_YYSTYPE*_T60C;struct _tuple26 _T60D;struct _tuple26 _T60E;struct _tuple26 _T60F;struct Cyc_Yystacktype*_T610;union Cyc_YYSTYPE*_T611;union Cyc_YYSTYPE*_T612;struct Cyc_List_List*_T613;struct _tuple26 _T614;struct Cyc_List_List*_T615;struct _tuple26 _T616;struct Cyc_Yystacktype*_T617;struct Cyc_Yystacktype _T618;struct Cyc_Yyltype _T619;unsigned _T61A;unsigned _T61B;struct Cyc_Yystacktype*_T61C;union Cyc_YYSTYPE*_T61D;union Cyc_YYSTYPE*_T61E;struct Cyc_Yystacktype*_T61F;union Cyc_YYSTYPE*_T620;union Cyc_YYSTYPE*_T621;struct _tuple26 _T622;struct _tuple26 _T623;struct Cyc_Yystacktype*_T624;struct Cyc_Yystacktype _T625;struct Cyc_Yyltype _T626;unsigned _T627;unsigned _T628;struct Cyc_Yystacktype*_T629;union Cyc_YYSTYPE*_T62A;union Cyc_YYSTYPE*_T62B;struct Cyc_Parse_Type_specifier _T62C;struct _tuple26 _T62D;struct Cyc_Parse_Type_specifier _T62E;struct _tuple26 _T62F;struct _tuple26 _T630;struct Cyc_Yystacktype*_T631;union Cyc_YYSTYPE*_T632;union Cyc_YYSTYPE*_T633;struct Cyc_Yystacktype*_T634;union Cyc_YYSTYPE*_T635;union Cyc_YYSTYPE*_T636;struct _tuple26 _T637;struct Cyc_Yystacktype*_T638;union Cyc_YYSTYPE*_T639;union Cyc_YYSTYPE*_T63A;struct Cyc_Absyn_Tqual _T63B;struct _tuple26 _T63C;struct Cyc_Absyn_Tqual _T63D;struct _tuple26 _T63E;struct _tuple26 _T63F;struct _tuple26 _T640;struct Cyc_Yystacktype*_T641;struct Cyc_Yystacktype _T642;struct Cyc_Yyltype _T643;unsigned _T644;unsigned _T645;struct Cyc_Yystacktype*_T646;union Cyc_YYSTYPE*_T647;union Cyc_YYSTYPE*_T648;struct Cyc_Yystacktype*_T649;union Cyc_YYSTYPE*_T64A;union Cyc_YYSTYPE*_T64B;struct _tuple26 _T64C;struct _tuple26 _T64D;struct _tuple26 _T64E;struct Cyc_Yystacktype*_T64F;union Cyc_YYSTYPE*_T650;union Cyc_YYSTYPE*_T651;struct Cyc_List_List*_T652;struct _tuple26 _T653;struct Cyc_List_List*_T654;struct Cyc_List_List*_T655;struct _RegionHandle*_T656;struct Cyc_Yystacktype*_T657;union Cyc_YYSTYPE*_T658;union Cyc_YYSTYPE*_T659;struct Cyc_List_List*_T65A;struct _RegionHandle*_T65B;struct Cyc_Yystacktype*_T65C;union Cyc_YYSTYPE*_T65D;union Cyc_YYSTYPE*_T65E;struct Cyc_Yystacktype*_T65F;union Cyc_YYSTYPE*_T660;union Cyc_YYSTYPE*_T661;struct _tuple12*_T662;struct _RegionHandle*_T663;struct Cyc_Yystacktype*_T664;union Cyc_YYSTYPE*_T665;union Cyc_YYSTYPE*_T666;struct Cyc_Yystacktype*_T667;union Cyc_YYSTYPE*_T668;union Cyc_YYSTYPE*_T669;struct _tuple12*_T66A;struct _RegionHandle*_T66B;struct _tuple0*_T66C;struct _fat_ptr*_T66D;struct Cyc_Yystacktype*_T66E;union Cyc_YYSTYPE*_T66F;union Cyc_YYSTYPE*_T670;struct _tuple12*_T671;struct _RegionHandle*_T672;struct _tuple0*_T673;struct _fat_ptr*_T674;struct _tuple12*_T675;struct _RegionHandle*_T676;struct Cyc_Yystacktype*_T677;union Cyc_YYSTYPE*_T678;union Cyc_YYSTYPE*_T679;struct Cyc_Yystacktype*_T67A;union Cyc_YYSTYPE*_T67B;union Cyc_YYSTYPE*_T67C;struct Cyc_Yystacktype*_T67D;union Cyc_YYSTYPE*_T67E;union Cyc_YYSTYPE*_T67F;struct Cyc_Absyn_Exp*_T680;struct Cyc_Yystacktype*_T681;union Cyc_YYSTYPE*_T682;union Cyc_YYSTYPE*_T683;struct Cyc_List_List*(*_T684)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T685)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T686;struct Cyc_Yystacktype _T687;struct Cyc_Yyltype _T688;unsigned _T689;unsigned _T68A;struct Cyc_Yystacktype*_T68B;union Cyc_YYSTYPE*_T68C;union Cyc_YYSTYPE*_T68D;struct Cyc_List_List*_T68E;struct Cyc_Yystacktype*_T68F;union Cyc_YYSTYPE*_T690;union Cyc_YYSTYPE*_T691;struct _tuple0*_T692;struct Cyc_List_List*_T693;struct Cyc_Core_Opt*_T694;struct Cyc_Yystacktype*_T695;union Cyc_YYSTYPE*_T696;union Cyc_YYSTYPE*_T697;int _T698;struct Cyc_Yystacktype*_T699;struct Cyc_Yystacktype _T69A;struct Cyc_Yyltype _T69B;unsigned _T69C;unsigned _T69D;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T69E;void*_T69F;struct Cyc_Yystacktype*_T6A0;struct Cyc_Yystacktype _T6A1;struct Cyc_Yyltype _T6A2;unsigned _T6A3;unsigned _T6A4;struct Cyc_Parse_Type_specifier _T6A5;struct Cyc_Yystacktype*_T6A6;union Cyc_YYSTYPE*_T6A7;union Cyc_YYSTYPE*_T6A8;struct Cyc_Absyn_UnknownDatatypeInfo _T6A9;struct Cyc_Yystacktype*_T6AA;union Cyc_YYSTYPE*_T6AB;union Cyc_YYSTYPE*_T6AC;union Cyc_Absyn_DatatypeInfo _T6AD;struct Cyc_Yystacktype*_T6AE;union Cyc_YYSTYPE*_T6AF;union Cyc_YYSTYPE*_T6B0;struct Cyc_List_List*_T6B1;void*_T6B2;struct Cyc_Yystacktype*_T6B3;struct Cyc_Yystacktype _T6B4;struct Cyc_Yyltype _T6B5;unsigned _T6B6;unsigned _T6B7;struct Cyc_Parse_Type_specifier _T6B8;struct Cyc_Yystacktype*_T6B9;union Cyc_YYSTYPE*_T6BA;union Cyc_YYSTYPE*_T6BB;struct Cyc_Absyn_UnknownDatatypeFieldInfo _T6BC;struct Cyc_Yystacktype*_T6BD;union Cyc_YYSTYPE*_T6BE;union Cyc_YYSTYPE*_T6BF;struct Cyc_Yystacktype*_T6C0;union Cyc_YYSTYPE*_T6C1;union Cyc_YYSTYPE*_T6C2;union Cyc_Absyn_DatatypeFieldInfo _T6C3;struct Cyc_Yystacktype*_T6C4;union Cyc_YYSTYPE*_T6C5;union Cyc_YYSTYPE*_T6C6;struct Cyc_List_List*_T6C7;void*_T6C8;struct Cyc_Yystacktype*_T6C9;struct Cyc_Yystacktype _T6CA;struct Cyc_Yyltype _T6CB;unsigned _T6CC;unsigned _T6CD;struct Cyc_Parse_Type_specifier _T6CE;struct Cyc_List_List*_T6CF;struct Cyc_Yystacktype*_T6D0;union Cyc_YYSTYPE*_T6D1;union Cyc_YYSTYPE*_T6D2;struct Cyc_List_List*_T6D3;struct Cyc_Yystacktype*_T6D4;union Cyc_YYSTYPE*_T6D5;union Cyc_YYSTYPE*_T6D6;struct Cyc_List_List*_T6D7;struct Cyc_Yystacktype*_T6D8;union Cyc_YYSTYPE*_T6D9;union Cyc_YYSTYPE*_T6DA;struct Cyc_Yystacktype*_T6DB;union Cyc_YYSTYPE*_T6DC;union Cyc_YYSTYPE*_T6DD;struct Cyc_List_List*_T6DE;struct Cyc_Yystacktype*_T6DF;union Cyc_YYSTYPE*_T6E0;union Cyc_YYSTYPE*_T6E1;struct Cyc_Yystacktype*_T6E2;union Cyc_YYSTYPE*_T6E3;union Cyc_YYSTYPE*_T6E4;struct Cyc_Absyn_Datatypefield*_T6E5;struct Cyc_Yystacktype*_T6E6;union Cyc_YYSTYPE*_T6E7;union Cyc_YYSTYPE*_T6E8;struct Cyc_Yystacktype*_T6E9;struct Cyc_Yystacktype _T6EA;struct Cyc_Yyltype _T6EB;unsigned _T6EC;struct Cyc_Yystacktype*_T6ED;union Cyc_YYSTYPE*_T6EE;union Cyc_YYSTYPE*_T6EF;struct Cyc_List_List*(*_T6F0)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T6F1)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple19*(*_T6F2)(unsigned,struct _tuple8*);struct Cyc_Yystacktype*_T6F3;struct Cyc_Yystacktype _T6F4;struct Cyc_Yyltype _T6F5;unsigned _T6F6;unsigned _T6F7;struct Cyc_Yystacktype*_T6F8;union Cyc_YYSTYPE*_T6F9;union Cyc_YYSTYPE*_T6FA;struct Cyc_List_List*_T6FB;struct Cyc_List_List*_T6FC;struct Cyc_Absyn_Datatypefield*_T6FD;struct Cyc_Yystacktype*_T6FE;union Cyc_YYSTYPE*_T6FF;union Cyc_YYSTYPE*_T700;struct Cyc_Yystacktype*_T701;struct Cyc_Yystacktype _T702;struct Cyc_Yyltype _T703;unsigned _T704;struct Cyc_Yystacktype*_T705;union Cyc_YYSTYPE*_T706;union Cyc_YYSTYPE*_T707;struct Cyc_Yystacktype*_T708;struct Cyc_Yystacktype _T709;struct Cyc_Yystacktype*_T70A;union Cyc_YYSTYPE*_T70B;union Cyc_YYSTYPE*_T70C;struct Cyc_Parse_Declarator _T70D;struct Cyc_Parse_Declarator _T70E;struct Cyc_Parse_Declarator _T70F;struct Cyc_Yystacktype*_T710;union Cyc_YYSTYPE*_T711;union Cyc_YYSTYPE*_T712;struct Cyc_List_List*_T713;struct Cyc_Parse_Declarator _T714;struct Cyc_List_List*_T715;struct Cyc_Yystacktype*_T716;struct Cyc_Yystacktype _T717;struct Cyc_Yystacktype*_T718;union Cyc_YYSTYPE*_T719;union Cyc_YYSTYPE*_T71A;struct Cyc_Parse_Declarator _T71B;struct Cyc_Parse_Declarator _T71C;struct Cyc_Parse_Declarator _T71D;struct Cyc_Yystacktype*_T71E;union Cyc_YYSTYPE*_T71F;union Cyc_YYSTYPE*_T720;struct Cyc_List_List*_T721;struct Cyc_Parse_Declarator _T722;struct Cyc_List_List*_T723;struct Cyc_Parse_Declarator _T724;struct Cyc_Yystacktype*_T725;union Cyc_YYSTYPE*_T726;union Cyc_YYSTYPE*_T727;struct Cyc_Yystacktype*_T728;struct Cyc_Yystacktype _T729;struct Cyc_Yyltype _T72A;unsigned _T72B;struct Cyc_Yystacktype*_T72C;struct Cyc_Yystacktype _T72D;struct Cyc_Yystacktype*_T72E;union Cyc_YYSTYPE*_T72F;union Cyc_YYSTYPE*_T730;struct Cyc_List_List*_T731;struct _RegionHandle*_T732;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T733;struct _RegionHandle*_T734;struct Cyc_Yystacktype*_T735;struct Cyc_Yystacktype _T736;struct Cyc_Yyltype _T737;unsigned _T738;struct Cyc_Yystacktype*_T739;union Cyc_YYSTYPE*_T73A;union Cyc_YYSTYPE*_T73B;struct Cyc_Parse_Declarator _T73C;struct Cyc_Yystacktype*_T73D;struct Cyc_Yystacktype _T73E;struct Cyc_Parse_Declarator _T73F;struct Cyc_Yystacktype*_T740;union Cyc_YYSTYPE*_T741;union Cyc_YYSTYPE*_T742;struct Cyc_Parse_Declarator _T743;struct Cyc_Yystacktype*_T744;union Cyc_YYSTYPE*_T745;union Cyc_YYSTYPE*_T746;struct Cyc_Parse_Declarator _T747;struct Cyc_List_List*_T748;struct _RegionHandle*_T749;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T74A;struct _RegionHandle*_T74B;struct Cyc_Yystacktype*_T74C;union Cyc_YYSTYPE*_T74D;union Cyc_YYSTYPE*_T74E;struct Cyc_Yystacktype*_T74F;struct Cyc_Yystacktype _T750;struct Cyc_Yyltype _T751;unsigned _T752;struct Cyc_Yystacktype*_T753;union Cyc_YYSTYPE*_T754;union Cyc_YYSTYPE*_T755;struct Cyc_Parse_Declarator _T756;struct Cyc_Parse_Declarator _T757;struct Cyc_Yystacktype*_T758;union Cyc_YYSTYPE*_T759;union Cyc_YYSTYPE*_T75A;struct Cyc_Parse_Declarator _T75B;struct Cyc_Yystacktype*_T75C;union Cyc_YYSTYPE*_T75D;union Cyc_YYSTYPE*_T75E;struct Cyc_Parse_Declarator _T75F;struct Cyc_List_List*_T760;struct _RegionHandle*_T761;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T762;struct _RegionHandle*_T763;struct Cyc_Yystacktype*_T764;union Cyc_YYSTYPE*_T765;union Cyc_YYSTYPE*_T766;struct Cyc_Yystacktype*_T767;union Cyc_YYSTYPE*_T768;union Cyc_YYSTYPE*_T769;struct Cyc_Yystacktype*_T76A;struct Cyc_Yystacktype _T76B;struct Cyc_Yyltype _T76C;unsigned _T76D;struct Cyc_Yystacktype*_T76E;union Cyc_YYSTYPE*_T76F;union Cyc_YYSTYPE*_T770;struct Cyc_Parse_Declarator _T771;struct Cyc_Yystacktype*_T772;union Cyc_YYSTYPE*_T773;union Cyc_YYSTYPE*_T774;struct Cyc_Yystacktype*_T775;union Cyc_YYSTYPE*_T776;union Cyc_YYSTYPE*_T777;struct Cyc_Parse_Declarator _T778;struct Cyc_Yystacktype*_T779;union Cyc_YYSTYPE*_T77A;union Cyc_YYSTYPE*_T77B;struct Cyc_Parse_Declarator _T77C;struct Cyc_Yystacktype*_T77D;union Cyc_YYSTYPE*_T77E;union Cyc_YYSTYPE*_T77F;struct Cyc_Parse_Declarator _T780;struct Cyc_List_List*_T781;struct _RegionHandle*_T782;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T783;struct _RegionHandle*_T784;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T785;struct _RegionHandle*_T786;struct Cyc_Yystacktype*_T787;union Cyc_YYSTYPE*_T788;union Cyc_YYSTYPE*_T789;struct Cyc_Parse_Declarator _T78A;struct Cyc_Parse_Declarator _T78B;struct Cyc_Yystacktype*_T78C;union Cyc_YYSTYPE*_T78D;union Cyc_YYSTYPE*_T78E;struct Cyc_Parse_Declarator _T78F;struct Cyc_Yystacktype*_T790;union Cyc_YYSTYPE*_T791;union Cyc_YYSTYPE*_T792;struct Cyc_Parse_Declarator _T793;struct Cyc_List_List*_T794;struct _RegionHandle*_T795;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T796;struct _RegionHandle*_T797;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T798;struct _RegionHandle*_T799;struct Cyc_Yystacktype*_T79A;union Cyc_YYSTYPE*_T79B;union Cyc_YYSTYPE*_T79C;struct Cyc_Yystacktype*_T79D;struct Cyc_Yystacktype _T79E;struct Cyc_Yyltype _T79F;unsigned _T7A0;struct Cyc_Yystacktype*_T7A1;union Cyc_YYSTYPE*_T7A2;union Cyc_YYSTYPE*_T7A3;struct Cyc_Parse_Declarator _T7A4;struct Cyc_List_List*(*_T7A5)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T7A6)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T7A7;struct Cyc_Yystacktype _T7A8;struct Cyc_Yyltype _T7A9;unsigned _T7AA;unsigned _T7AB;struct Cyc_Yystacktype*_T7AC;union Cyc_YYSTYPE*_T7AD;union Cyc_YYSTYPE*_T7AE;struct Cyc_List_List*_T7AF;struct Cyc_List_List*_T7B0;struct Cyc_Parse_Declarator _T7B1;struct Cyc_Yystacktype*_T7B2;union Cyc_YYSTYPE*_T7B3;union Cyc_YYSTYPE*_T7B4;struct Cyc_Parse_Declarator _T7B5;struct Cyc_Yystacktype*_T7B6;union Cyc_YYSTYPE*_T7B7;union Cyc_YYSTYPE*_T7B8;struct Cyc_Parse_Declarator _T7B9;struct Cyc_List_List*_T7BA;struct _RegionHandle*_T7BB;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T7BC;struct _RegionHandle*_T7BD;struct Cyc_Yystacktype*_T7BE;struct Cyc_Yystacktype _T7BF;struct Cyc_Yyltype _T7C0;unsigned _T7C1;struct Cyc_Yystacktype*_T7C2;union Cyc_YYSTYPE*_T7C3;union Cyc_YYSTYPE*_T7C4;struct Cyc_Parse_Declarator _T7C5;struct Cyc_Parse_Declarator _T7C6;struct Cyc_Yystacktype*_T7C7;union Cyc_YYSTYPE*_T7C8;union Cyc_YYSTYPE*_T7C9;struct Cyc_Parse_Declarator _T7CA;struct Cyc_Yystacktype*_T7CB;union Cyc_YYSTYPE*_T7CC;union Cyc_YYSTYPE*_T7CD;struct Cyc_Parse_Declarator _T7CE;struct Cyc_List_List*_T7CF;struct _RegionHandle*_T7D0;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T7D1;struct _RegionHandle*_T7D2;struct Cyc_Yystacktype*_T7D3;struct Cyc_Yystacktype _T7D4;struct Cyc_Yyltype _T7D5;unsigned _T7D6;struct Cyc_Yystacktype*_T7D7;union Cyc_YYSTYPE*_T7D8;union Cyc_YYSTYPE*_T7D9;struct Cyc_Yystacktype*_T7DA;union Cyc_YYSTYPE*_T7DB;union Cyc_YYSTYPE*_T7DC;struct Cyc_Parse_Declarator _T7DD;struct Cyc_Parse_Declarator _T7DE;struct Cyc_Yystacktype*_T7DF;union Cyc_YYSTYPE*_T7E0;union Cyc_YYSTYPE*_T7E1;struct Cyc_Yystacktype*_T7E2;struct Cyc_Yystacktype _T7E3;struct Cyc_Yyltype _T7E4;unsigned _T7E5;struct Cyc_Parse_Declarator _T7E6;struct Cyc_Yystacktype*_T7E7;union Cyc_YYSTYPE*_T7E8;union Cyc_YYSTYPE*_T7E9;struct Cyc_Yystacktype*_T7EA;struct Cyc_Yystacktype _T7EB;struct Cyc_Yyltype _T7EC;unsigned _T7ED;struct Cyc_Yystacktype*_T7EE;struct Cyc_Yystacktype _T7EF;struct Cyc_Yystacktype*_T7F0;union Cyc_YYSTYPE*_T7F1;union Cyc_YYSTYPE*_T7F2;struct Cyc_List_List*_T7F3;struct _RegionHandle*_T7F4;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T7F5;struct _RegionHandle*_T7F6;struct Cyc_Yystacktype*_T7F7;struct Cyc_Yystacktype _T7F8;struct Cyc_Yyltype _T7F9;unsigned _T7FA;struct Cyc_Yystacktype*_T7FB;union Cyc_YYSTYPE*_T7FC;union Cyc_YYSTYPE*_T7FD;struct Cyc_Parse_Declarator _T7FE;struct Cyc_Yystacktype*_T7FF;struct Cyc_Yystacktype _T800;struct Cyc_Yystacktype*_T801;union Cyc_YYSTYPE*_T802;union Cyc_YYSTYPE*_T803;struct Cyc_Parse_Declarator _T804;struct Cyc_Parse_Declarator _T805;struct Cyc_Parse_Declarator _T806;struct Cyc_List_List*_T807;struct _RegionHandle*_T808;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T809;struct _RegionHandle*_T80A;struct Cyc_Yystacktype*_T80B;union Cyc_YYSTYPE*_T80C;union Cyc_YYSTYPE*_T80D;struct Cyc_Yystacktype*_T80E;struct Cyc_Yystacktype _T80F;struct Cyc_Yyltype _T810;unsigned _T811;struct Cyc_Parse_Declarator _T812;struct Cyc_Yystacktype*_T813;union Cyc_YYSTYPE*_T814;union Cyc_YYSTYPE*_T815;struct Cyc_Parse_Declarator _T816;struct Cyc_Parse_Declarator _T817;struct Cyc_Parse_Declarator _T818;struct Cyc_List_List*_T819;struct _RegionHandle*_T81A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T81B;struct _RegionHandle*_T81C;struct Cyc_Yystacktype*_T81D;union Cyc_YYSTYPE*_T81E;union Cyc_YYSTYPE*_T81F;struct Cyc_Yystacktype*_T820;union Cyc_YYSTYPE*_T821;union Cyc_YYSTYPE*_T822;struct Cyc_Yystacktype*_T823;struct Cyc_Yystacktype _T824;struct Cyc_Yyltype _T825;unsigned _T826;struct Cyc_Parse_Declarator _T827;struct Cyc_Yystacktype*_T828;union Cyc_YYSTYPE*_T829;union Cyc_YYSTYPE*_T82A;struct Cyc_Yystacktype*_T82B;union Cyc_YYSTYPE*_T82C;union Cyc_YYSTYPE*_T82D;struct Cyc_Yystacktype*_T82E;union Cyc_YYSTYPE*_T82F;union Cyc_YYSTYPE*_T830;struct Cyc_Parse_Declarator _T831;struct Cyc_Parse_Declarator _T832;struct Cyc_Parse_Declarator _T833;struct Cyc_List_List*_T834;struct _RegionHandle*_T835;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T836;struct _RegionHandle*_T837;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T838;struct _RegionHandle*_T839;struct Cyc_Parse_Declarator _T83A;struct Cyc_Yystacktype*_T83B;union Cyc_YYSTYPE*_T83C;union Cyc_YYSTYPE*_T83D;struct Cyc_Parse_Declarator _T83E;struct Cyc_Parse_Declarator _T83F;struct Cyc_Parse_Declarator _T840;struct Cyc_List_List*_T841;struct _RegionHandle*_T842;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T843;struct _RegionHandle*_T844;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T845;struct _RegionHandle*_T846;struct Cyc_Yystacktype*_T847;union Cyc_YYSTYPE*_T848;union Cyc_YYSTYPE*_T849;struct Cyc_Yystacktype*_T84A;struct Cyc_Yystacktype _T84B;struct Cyc_Yyltype _T84C;unsigned _T84D;struct Cyc_Parse_Declarator _T84E;struct Cyc_List_List*(*_T84F)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T850)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T851;struct Cyc_Yystacktype _T852;struct Cyc_Yyltype _T853;unsigned _T854;unsigned _T855;struct Cyc_Yystacktype*_T856;union Cyc_YYSTYPE*_T857;union Cyc_YYSTYPE*_T858;struct Cyc_List_List*_T859;struct Cyc_List_List*_T85A;struct Cyc_Yystacktype*_T85B;union Cyc_YYSTYPE*_T85C;union Cyc_YYSTYPE*_T85D;struct Cyc_Parse_Declarator _T85E;struct Cyc_Parse_Declarator _T85F;struct Cyc_Parse_Declarator _T860;struct Cyc_List_List*_T861;struct _RegionHandle*_T862;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T863;struct _RegionHandle*_T864;struct Cyc_Yystacktype*_T865;struct Cyc_Yystacktype _T866;struct Cyc_Yyltype _T867;unsigned _T868;struct Cyc_Parse_Declarator _T869;struct Cyc_Yystacktype*_T86A;union Cyc_YYSTYPE*_T86B;union Cyc_YYSTYPE*_T86C;struct Cyc_Parse_Declarator _T86D;struct Cyc_Parse_Declarator _T86E;struct Cyc_Parse_Declarator _T86F;struct Cyc_List_List*_T870;struct _RegionHandle*_T871;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T872;struct _RegionHandle*_T873;struct Cyc_Yystacktype*_T874;struct Cyc_Yystacktype _T875;struct Cyc_Yyltype _T876;unsigned _T877;struct Cyc_Yystacktype*_T878;union Cyc_YYSTYPE*_T879;union Cyc_YYSTYPE*_T87A;struct Cyc_Parse_Declarator _T87B;struct Cyc_Yystacktype*_T87C;struct Cyc_Yystacktype _T87D;struct Cyc_Yystacktype*_T87E;union Cyc_YYSTYPE*_T87F;union Cyc_YYSTYPE*_T880;struct Cyc_List_List*_T881;struct Cyc_Yystacktype*_T882;union Cyc_YYSTYPE*_T883;union Cyc_YYSTYPE*_T884;struct Cyc_List_List*_T885;struct Cyc_List_List*_T886;struct Cyc_Yystacktype*_T887;union Cyc_YYSTYPE*_T888;union Cyc_YYSTYPE*_T889;struct Cyc_List_List*_T88A;struct Cyc_List_List*_T88B;struct _RegionHandle*_T88C;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T88D;struct _RegionHandle*_T88E;struct Cyc_Yystacktype*_T88F;struct Cyc_Yystacktype _T890;struct Cyc_Yyltype _T891;unsigned _T892;struct Cyc_Yystacktype*_T893;union Cyc_YYSTYPE*_T894;union Cyc_YYSTYPE*_T895;struct Cyc_Yystacktype*_T896;union Cyc_YYSTYPE*_T897;union Cyc_YYSTYPE*_T898;struct _tuple22*_T899;int _T89A;struct Cyc_Absyn_PtrLoc*_T89B;struct Cyc_Yystacktype*_T89C;struct Cyc_Yystacktype _T89D;struct Cyc_Yyltype _T89E;unsigned _T89F;struct Cyc_Yystacktype*_T8A0;struct Cyc_Yystacktype _T8A1;struct Cyc_Yyltype _T8A2;unsigned _T8A3;struct _RegionHandle*_T8A4;struct Cyc_Absyn_PtrLoc*_T8A5;void*_T8A6;void*_T8A7;struct Cyc_Yystacktype*_T8A8;union Cyc_YYSTYPE*_T8A9;union Cyc_YYSTYPE*_T8AA;void*_T8AB;struct Cyc_Yystacktype*_T8AC;union Cyc_YYSTYPE*_T8AD;union Cyc_YYSTYPE*_T8AE;struct Cyc_List_List*_T8AF;struct Cyc_Yystacktype*_T8B0;union Cyc_YYSTYPE*_T8B1;union Cyc_YYSTYPE*_T8B2;struct Cyc_Absyn_Tqual _T8B3;struct Cyc_List_List*_T8B4;struct _RegionHandle*_T8B5;struct Cyc_List_List*_T8B6;struct _RegionHandle*_T8B7;struct Cyc_Yystacktype*_T8B8;union Cyc_YYSTYPE*_T8B9;union Cyc_YYSTYPE*_T8BA;struct Cyc_Yystacktype*_T8BB;union Cyc_YYSTYPE*_T8BC;union Cyc_YYSTYPE*_T8BD;struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T8BE;struct _RegionHandle*_T8BF;struct Cyc_Yystacktype*_T8C0;union Cyc_YYSTYPE*_T8C1;union Cyc_YYSTYPE*_T8C2;void*_T8C3;struct Cyc_Yystacktype*_T8C4;struct Cyc_Yystacktype _T8C5;struct Cyc_Yyltype _T8C6;unsigned _T8C7;unsigned _T8C8;struct _fat_ptr _T8C9;struct _fat_ptr _T8CA;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8CB;struct _RegionHandle*_T8CC;struct Cyc_List_List*_T8CD;struct Cyc_Yystacktype*_T8CE;union Cyc_YYSTYPE*_T8CF;union Cyc_YYSTYPE*_T8D0;void*_T8D1;struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T8D2;struct _RegionHandle*_T8D3;struct Cyc_Yystacktype*_T8D4;union Cyc_YYSTYPE*_T8D5;union Cyc_YYSTYPE*_T8D6;void*_T8D7;struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T8D8;struct _RegionHandle*_T8D9;void*_T8DA;struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T8DB;struct _RegionHandle*_T8DC;void*_T8DD;struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T8DE;struct _RegionHandle*_T8DF;void*_T8E0;struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T8E1;struct _RegionHandle*_T8E2;void*_T8E3;struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T8E4;struct _RegionHandle*_T8E5;void*_T8E6;struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T8E7;struct _RegionHandle*_T8E8;void*_T8E9;struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T8EA;struct _RegionHandle*_T8EB;void*_T8EC;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T8ED;struct _RegionHandle*_T8EE;struct Cyc_Yystacktype*_T8EF;union Cyc_YYSTYPE*_T8F0;union Cyc_YYSTYPE*_T8F1;void*_T8F2;struct Cyc_Yystacktype*_T8F3;struct Cyc_Yystacktype _T8F4;struct Cyc_Yyltype _T8F5;unsigned _T8F6;unsigned _T8F7;struct Cyc_Yystacktype*_T8F8;union Cyc_YYSTYPE*_T8F9;union Cyc_YYSTYPE*_T8FA;struct _fat_ptr _T8FB;struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T8FC;struct _RegionHandle*_T8FD;void*_T8FE;struct Cyc_Yystacktype*_T8FF;union Cyc_YYSTYPE*_T900;union Cyc_YYSTYPE*_T901;void*_T902;struct Cyc_Yystacktype*_T903;union Cyc_YYSTYPE*_T904;union Cyc_YYSTYPE*_T905;void*_T906;void*_T907;void*_T908;struct Cyc_Yystacktype*_T909;union Cyc_YYSTYPE*_T90A;union Cyc_YYSTYPE*_T90B;void*_T90C;void*_T90D;void*_T90E;void*_T90F;struct Cyc_Yystacktype*_T910;struct Cyc_Yystacktype _T911;struct Cyc_Yyltype _T912;unsigned _T913;unsigned _T914;struct Cyc_Yystacktype*_T915;union Cyc_YYSTYPE*_T916;union Cyc_YYSTYPE*_T917;struct _fat_ptr _T918;void*_T919;struct _tuple22*_T91A;struct Cyc_Yystacktype*_T91B;struct Cyc_Yystacktype _T91C;struct Cyc_Yyltype _T91D;unsigned _T91E;int _T91F;struct Cyc_Yystacktype*_T920;struct Cyc_Yystacktype _T921;struct Cyc_Yyltype _T922;unsigned _T923;unsigned _T924;struct _fat_ptr _T925;struct Cyc_Yystacktype*_T926;union Cyc_YYSTYPE*_T927;union Cyc_YYSTYPE*_T928;void*_T929;struct _tuple22*_T92A;struct Cyc_Yystacktype*_T92B;struct Cyc_Yystacktype _T92C;struct Cyc_Yyltype _T92D;unsigned _T92E;struct Cyc_Yystacktype*_T92F;struct Cyc_Yystacktype _T930;struct Cyc_Yyltype _T931;unsigned _T932;unsigned _T933;struct _fat_ptr _T934;struct Cyc_Yystacktype*_T935;union Cyc_YYSTYPE*_T936;union Cyc_YYSTYPE*_T937;void*_T938;struct _tuple22*_T939;int _T93A;int _T93B;struct _tuple22*_T93C;struct Cyc_Yystacktype*_T93D;struct Cyc_Yystacktype _T93E;struct Cyc_Yyltype _T93F;unsigned _T940;struct Cyc_Yystacktype*_T941;struct Cyc_Yystacktype _T942;struct Cyc_Yyltype _T943;unsigned _T944;unsigned _T945;struct _fat_ptr _T946;struct Cyc_Core_Opt*_T947;struct Cyc_Core_Opt*_T948;void*_T949;struct _tuple22*_T94A;struct Cyc_Yystacktype*_T94B;struct Cyc_Yystacktype _T94C;struct Cyc_Yyltype _T94D;unsigned _T94E;void*_T94F;int _T950;struct Cyc_Core_Opt*_T951;struct Cyc_Core_Opt*_T952;struct Cyc_Yystacktype*_T953;union Cyc_YYSTYPE*_T954;union Cyc_YYSTYPE*_T955;struct Cyc_Absyn_Exp*_T956;void*_T957;int _T958;int(*_T959)(unsigned,struct _fat_ptr);struct _fat_ptr _T95A;struct Cyc_Yystacktype*_T95B;union Cyc_YYSTYPE*_T95C;union Cyc_YYSTYPE*_T95D;struct _fat_ptr _T95E;void*_T95F;void*_T960;struct Cyc_List_List*_T961;struct Cyc_Yystacktype*_T962;union Cyc_YYSTYPE*_T963;union Cyc_YYSTYPE*_T964;struct Cyc_List_List*_T965;struct Cyc_Yystacktype*_T966;union Cyc_YYSTYPE*_T967;union Cyc_YYSTYPE*_T968;struct Cyc_Yystacktype*_T969;union Cyc_YYSTYPE*_T96A;union Cyc_YYSTYPE*_T96B;struct Cyc_Core_Opt*_T96C;struct Cyc_Core_Opt*_T96D;void*_T96E;struct Cyc_Yystacktype*_T96F;union Cyc_YYSTYPE*_T970;union Cyc_YYSTYPE*_T971;int _T972;struct Cyc_List_List*_T973;void*_T974;struct Cyc_Yystacktype*_T975;union Cyc_YYSTYPE*_T976;union Cyc_YYSTYPE*_T977;struct Cyc_List_List*_T978;void*_T979;struct Cyc_Core_Opt*_T97A;struct Cyc_Core_Opt*_T97B;void*_T97C;struct _fat_ptr _T97D;int _T97E;unsigned char*_T97F;struct Cyc_Yystacktype*_T980;struct Cyc_Yystacktype _T981;struct Cyc_Yyltype _T982;unsigned _T983;unsigned _T984;struct Cyc_Absyn_Tqual _T985;struct Cyc_Yystacktype*_T986;union Cyc_YYSTYPE*_T987;union Cyc_YYSTYPE*_T988;struct Cyc_Absyn_Tqual _T989;struct Cyc_Yystacktype*_T98A;union Cyc_YYSTYPE*_T98B;union Cyc_YYSTYPE*_T98C;struct Cyc_Absyn_Tqual _T98D;struct Cyc_Absyn_Tqual _T98E;struct Cyc_Yystacktype*_T98F;union Cyc_YYSTYPE*_T990;union Cyc_YYSTYPE*_T991;struct _tuple28 _T992;struct _tuple28*_T993;unsigned _T994;struct _tuple28*_T995;struct _tuple28 _T996;struct _tuple27*_T997;struct Cyc_Yystacktype*_T998;union Cyc_YYSTYPE*_T999;union Cyc_YYSTYPE*_T99A;struct Cyc_Yystacktype*_T99B;union Cyc_YYSTYPE*_T99C;union Cyc_YYSTYPE*_T99D;struct _tuple28 _T99E;struct _tuple28*_T99F;unsigned _T9A0;struct _tuple28*_T9A1;struct _tuple28 _T9A2;struct _tuple27*_T9A3;struct Cyc_Yystacktype*_T9A4;union Cyc_YYSTYPE*_T9A5;union Cyc_YYSTYPE*_T9A6;struct Cyc_List_List*_T9A7;struct Cyc_Yystacktype*_T9A8;union Cyc_YYSTYPE*_T9A9;union Cyc_YYSTYPE*_T9AA;struct Cyc_Yystacktype*_T9AB;union Cyc_YYSTYPE*_T9AC;union Cyc_YYSTYPE*_T9AD;struct _tuple28 _T9AE;struct _tuple28*_T9AF;unsigned _T9B0;struct _tuple28*_T9B1;struct _tuple28 _T9B2;struct _tuple27*_T9B3;struct Cyc_Yystacktype*_T9B4;union Cyc_YYSTYPE*_T9B5;union Cyc_YYSTYPE*_T9B6;struct Cyc_List_List*_T9B7;struct Cyc_Yystacktype*_T9B8;union Cyc_YYSTYPE*_T9B9;union Cyc_YYSTYPE*_T9BA;struct Cyc_Yystacktype*_T9BB;union Cyc_YYSTYPE*_T9BC;union Cyc_YYSTYPE*_T9BD;struct Cyc_Absyn_VarargInfo*_T9BE;struct Cyc_Absyn_VarargInfo*_T9BF;struct Cyc_Absyn_VarargInfo*_T9C0;struct Cyc_Absyn_VarargInfo*_T9C1;struct Cyc_Yystacktype*_T9C2;union Cyc_YYSTYPE*_T9C3;union Cyc_YYSTYPE*_T9C4;struct Cyc_Yystacktype*_T9C5;union Cyc_YYSTYPE*_T9C6;union Cyc_YYSTYPE*_T9C7;struct _tuple28 _T9C8;struct _tuple28*_T9C9;unsigned _T9CA;struct _tuple28*_T9CB;struct _tuple28 _T9CC;struct _tuple27*_T9CD;struct Cyc_Yystacktype*_T9CE;union Cyc_YYSTYPE*_T9CF;union Cyc_YYSTYPE*_T9D0;struct Cyc_Yystacktype*_T9D1;union Cyc_YYSTYPE*_T9D2;union Cyc_YYSTYPE*_T9D3;struct Cyc_Absyn_VarargInfo*_T9D4;struct Cyc_Absyn_VarargInfo*_T9D5;struct Cyc_Absyn_VarargInfo*_T9D6;struct Cyc_Absyn_VarargInfo*_T9D7;struct Cyc_Yystacktype*_T9D8;union Cyc_YYSTYPE*_T9D9;union Cyc_YYSTYPE*_T9DA;struct Cyc_Yystacktype*_T9DB;union Cyc_YYSTYPE*_T9DC;union Cyc_YYSTYPE*_T9DD;struct _tuple28 _T9DE;struct _tuple28*_T9DF;unsigned _T9E0;struct _tuple28*_T9E1;struct _tuple28 _T9E2;struct _tuple27*_T9E3;struct Cyc_Yystacktype*_T9E4;union Cyc_YYSTYPE*_T9E5;union Cyc_YYSTYPE*_T9E6;struct Cyc_List_List*_T9E7;struct Cyc_Yystacktype*_T9E8;union Cyc_YYSTYPE*_T9E9;union Cyc_YYSTYPE*_T9EA;struct Cyc_Yystacktype*_T9EB;struct Cyc_Yystacktype _T9EC;struct Cyc_Yyltype _T9ED;unsigned _T9EE;unsigned _T9EF;struct Cyc_Yystacktype*_T9F0;union Cyc_YYSTYPE*_T9F1;union Cyc_YYSTYPE*_T9F2;struct _fat_ptr _T9F3;void*_T9F4;struct Cyc_Yystacktype*_T9F5;union Cyc_YYSTYPE*_T9F6;union Cyc_YYSTYPE*_T9F7;struct _fat_ptr _T9F8;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T9F9;void*_T9FA;struct Cyc_Yystacktype*_T9FB;union Cyc_YYSTYPE*_T9FC;union Cyc_YYSTYPE*_T9FD;void*_T9FE;struct Cyc_Yystacktype*_T9FF;struct Cyc_Yystacktype _TA00;struct Cyc_Yyltype _TA01;unsigned _TA02;unsigned _TA03;void*_TA04;struct Cyc_Yystacktype*_TA05;union Cyc_YYSTYPE*_TA06;union Cyc_YYSTYPE*_TA07;struct _fat_ptr _TA08;struct Cyc_Yystacktype*_TA09;union Cyc_YYSTYPE*_TA0A;union Cyc_YYSTYPE*_TA0B;struct Cyc_Absyn_Kind*_TA0C;void*_TA0D;struct Cyc_Yystacktype*_TA0E;union Cyc_YYSTYPE*_TA0F;union Cyc_YYSTYPE*_TA10;void*_TA11;struct Cyc_Yystacktype*_TA12;struct Cyc_Yystacktype _TA13;struct Cyc_Yyltype _TA14;unsigned _TA15;unsigned _TA16;void*_TA17;struct Cyc_Yystacktype*_TA18;union Cyc_YYSTYPE*_TA19;union Cyc_YYSTYPE*_TA1A;struct Cyc_List_List*_TA1B;void*_TA1C;struct Cyc_Yystacktype*_TA1D;struct Cyc_Yystacktype _TA1E;struct _tuple28*_TA1F;struct Cyc_List_List*_TA20;struct Cyc_Yystacktype*_TA21;union Cyc_YYSTYPE*_TA22;union Cyc_YYSTYPE*_TA23;struct _fat_ptr _TA24;struct _tuple28*_TA25;struct Cyc_List_List*_TA26;struct Cyc_Yystacktype*_TA27;union Cyc_YYSTYPE*_TA28;union Cyc_YYSTYPE*_TA29;struct _fat_ptr _TA2A;struct Cyc_Yystacktype*_TA2B;union Cyc_YYSTYPE*_TA2C;union Cyc_YYSTYPE*_TA2D;struct _tuple28*_TA2E;struct _tuple28*_TA2F;struct Cyc_List_List*_TA30;struct Cyc_Yystacktype*_TA31;union Cyc_YYSTYPE*_TA32;union Cyc_YYSTYPE*_TA33;struct Cyc_Yystacktype*_TA34;union Cyc_YYSTYPE*_TA35;union Cyc_YYSTYPE*_TA36;struct _tuple28*_TA37;struct _tuple28*_TA38;struct Cyc_List_List*_TA39;struct Cyc_Yystacktype*_TA3A;union Cyc_YYSTYPE*_TA3B;union Cyc_YYSTYPE*_TA3C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA3D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA3E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA3F;struct Cyc_Absyn_Kind*_TA40;struct Cyc_Yystacktype*_TA41;union Cyc_YYSTYPE*_TA42;union Cyc_YYSTYPE*_TA43;struct _fat_ptr _TA44;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TA45;void*_TA46;struct Cyc_Yystacktype*_TA47;struct Cyc_Yystacktype _TA48;struct Cyc_Yyltype _TA49;unsigned _TA4A;unsigned _TA4B;struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_TA4C;struct Cyc_Yystacktype*_TA4D;union Cyc_YYSTYPE*_TA4E;union Cyc_YYSTYPE*_TA4F;struct Cyc_List_List*_TA50;void*_TA51;struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_TA52;struct Cyc_Yystacktype*_TA53;union Cyc_YYSTYPE*_TA54;union Cyc_YYSTYPE*_TA55;struct Cyc_List_List*_TA56;struct Cyc_Yystacktype*_TA57;union Cyc_YYSTYPE*_TA58;union Cyc_YYSTYPE*_TA59;struct Cyc_List_List*_TA5A;void*_TA5B;struct Cyc_Yystacktype*_TA5C;struct Cyc_Yystacktype _TA5D;struct Cyc_Yyltype _TA5E;unsigned _TA5F;unsigned _TA60;struct Cyc_Yystacktype*_TA61;union Cyc_YYSTYPE*_TA62;union Cyc_YYSTYPE*_TA63;struct _fat_ptr _TA64;struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_TA65;struct Cyc_Yystacktype*_TA66;union Cyc_YYSTYPE*_TA67;union Cyc_YYSTYPE*_TA68;struct Cyc_List_List*_TA69;void*_TA6A;struct _tuple29*_TA6B;struct Cyc_Yystacktype*_TA6C;union Cyc_YYSTYPE*_TA6D;union Cyc_YYSTYPE*_TA6E;struct Cyc_Yystacktype*_TA6F;union Cyc_YYSTYPE*_TA70;union Cyc_YYSTYPE*_TA71;struct Cyc_Yystacktype*_TA72;struct Cyc_Yystacktype _TA73;struct Cyc_Yyltype _TA74;unsigned _TA75;unsigned _TA76;struct Cyc_Yystacktype*_TA77;union Cyc_YYSTYPE*_TA78;union Cyc_YYSTYPE*_TA79;struct _fat_ptr _TA7A;void*_TA7B;struct Cyc_Yystacktype*_TA7C;union Cyc_YYSTYPE*_TA7D;union Cyc_YYSTYPE*_TA7E;void*_TA7F;struct Cyc_Yystacktype*_TA80;union Cyc_YYSTYPE*_TA81;union Cyc_YYSTYPE*_TA82;void*_TA83;void*_TA84;void*_TA85;void*_TA86;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA87;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA88;struct Cyc_Absyn_Kind*_TA89;struct Cyc_Yystacktype*_TA8A;union Cyc_YYSTYPE*_TA8B;union Cyc_YYSTYPE*_TA8C;struct _fat_ptr _TA8D;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_TA8E;void*_TA8F;struct Cyc_Yystacktype*_TA90;struct Cyc_Yystacktype _TA91;struct Cyc_Yyltype _TA92;unsigned _TA93;unsigned _TA94;void*_TA95;struct Cyc_Yystacktype*_TA96;union Cyc_YYSTYPE*_TA97;union Cyc_YYSTYPE*_TA98;void*_TA99;void*_TA9A;struct Cyc_Yystacktype*_TA9B;union Cyc_YYSTYPE*_TA9C;union Cyc_YYSTYPE*_TA9D;struct _fat_ptr _TA9E;struct _fat_ptr _TA9F;int _TAA0;struct Cyc_Warn_String_Warn_Warg_struct _TAA1;struct Cyc_Yystacktype*_TAA2;struct Cyc_Yystacktype _TAA3;struct Cyc_Yyltype _TAA4;unsigned _TAA5;unsigned _TAA6;struct _fat_ptr _TAA7;struct Cyc_Yystacktype*_TAA8;struct Cyc_Yystacktype _TAA9;struct Cyc_Yystacktype*_TAAA;union Cyc_YYSTYPE*_TAAB;union Cyc_YYSTYPE*_TAAC;struct Cyc_List_List*_TAAD;struct Cyc_Yystacktype*_TAAE;union Cyc_YYSTYPE*_TAAF;union Cyc_YYSTYPE*_TAB0;struct Cyc_List_List*_TAB1;struct Cyc_List_List*_TAB2;struct Cyc_Yystacktype*_TAB3;struct Cyc_Yystacktype _TAB4;struct Cyc_List_List*_TAB5;struct Cyc_Yystacktype*_TAB6;union Cyc_YYSTYPE*_TAB7;union Cyc_YYSTYPE*_TAB8;void*_TAB9;struct Cyc_Yystacktype*_TABA;union Cyc_YYSTYPE*_TABB;union Cyc_YYSTYPE*_TABC;void*_TABD;struct Cyc_Absyn_Kind*_TABE;struct Cyc_Absyn_Kind*_TABF;struct Cyc_List_List*_TAC0;struct Cyc_Yystacktype*_TAC1;union Cyc_YYSTYPE*_TAC2;union Cyc_YYSTYPE*_TAC3;struct Cyc_List_List*_TAC4;struct Cyc_Yystacktype*_TAC5;union Cyc_YYSTYPE*_TAC6;union Cyc_YYSTYPE*_TAC7;struct _tuple8*_TAC8;struct Cyc_Yystacktype*_TAC9;struct Cyc_Yystacktype _TACA;struct Cyc_Yyltype _TACB;unsigned _TACC;unsigned _TACD;struct Cyc_List_List*_TACE;struct Cyc_Yystacktype*_TACF;union Cyc_YYSTYPE*_TAD0;union Cyc_YYSTYPE*_TAD1;struct _tuple8*_TAD2;struct Cyc_Yystacktype*_TAD3;struct Cyc_Yystacktype _TAD4;struct Cyc_Yyltype _TAD5;unsigned _TAD6;unsigned _TAD7;struct Cyc_Yystacktype*_TAD8;union Cyc_YYSTYPE*_TAD9;union Cyc_YYSTYPE*_TADA;struct Cyc_List_List*_TADB;struct Cyc_Yystacktype*_TADC;union Cyc_YYSTYPE*_TADD;union Cyc_YYSTYPE*_TADE;struct Cyc_List_List*_TADF;struct Cyc_Yystacktype*_TAE0;union Cyc_YYSTYPE*_TAE1;union Cyc_YYSTYPE*_TAE2;struct Cyc_Yystacktype*_TAE3;union Cyc_YYSTYPE*_TAE4;union Cyc_YYSTYPE*_TAE5;struct Cyc_Yystacktype*_TAE6;union Cyc_YYSTYPE*_TAE7;union Cyc_YYSTYPE*_TAE8;struct Cyc_Absyn_Tqual _TAE9;unsigned _TAEA;struct Cyc_Yystacktype*_TAEB;struct Cyc_Yystacktype _TAEC;struct Cyc_Yyltype _TAED;unsigned _TAEE;struct Cyc_Yystacktype*_TAEF;union Cyc_YYSTYPE*_TAF0;union Cyc_YYSTYPE*_TAF1;struct Cyc_Parse_Type_specifier _TAF2;struct Cyc_Yystacktype*_TAF3;struct Cyc_Yystacktype _TAF4;struct Cyc_Yyltype _TAF5;unsigned _TAF6;unsigned _TAF7;struct Cyc_Warn_String_Warn_Warg_struct _TAF8;struct Cyc_Yystacktype*_TAF9;struct Cyc_Yystacktype _TAFA;struct Cyc_Yyltype _TAFB;unsigned _TAFC;unsigned _TAFD;struct _fat_ptr _TAFE;int _TAFF;struct Cyc_Warn_String_Warn_Warg_struct _TB00;struct Cyc_Yystacktype*_TB01;struct Cyc_Yystacktype _TB02;struct Cyc_Yyltype _TB03;unsigned _TB04;unsigned _TB05;struct _fat_ptr _TB06;struct _tuple0*_TB07;struct _tuple0 _TB08;struct Cyc_Warn_String_Warn_Warg_struct _TB09;struct Cyc_Yystacktype*_TB0A;struct Cyc_Yystacktype _TB0B;struct Cyc_Yyltype _TB0C;unsigned _TB0D;unsigned _TB0E;struct _fat_ptr _TB0F;struct _tuple8*_TB10;struct Cyc_Yystacktype*_TB11;union Cyc_YYSTYPE*_TB12;union Cyc_YYSTYPE*_TB13;struct Cyc_Absyn_Tqual _TB14;unsigned _TB15;struct Cyc_Yystacktype*_TB16;struct Cyc_Yystacktype _TB17;struct Cyc_Yyltype _TB18;unsigned _TB19;struct Cyc_Parse_Type_specifier _TB1A;struct Cyc_Yystacktype*_TB1B;struct Cyc_Yystacktype _TB1C;struct Cyc_Yyltype _TB1D;unsigned _TB1E;unsigned _TB1F;struct Cyc_Warn_String_Warn_Warg_struct _TB20;struct Cyc_Yystacktype*_TB21;struct Cyc_Yystacktype _TB22;struct Cyc_Yyltype _TB23;unsigned _TB24;unsigned _TB25;struct _fat_ptr _TB26;struct _tuple8*_TB27;struct Cyc_Yystacktype*_TB28;union Cyc_YYSTYPE*_TB29;union Cyc_YYSTYPE*_TB2A;struct Cyc_Absyn_Tqual _TB2B;unsigned _TB2C;struct Cyc_Yystacktype*_TB2D;struct Cyc_Yystacktype _TB2E;struct Cyc_Yyltype _TB2F;unsigned _TB30;struct Cyc_Parse_Type_specifier _TB31;struct Cyc_Yystacktype*_TB32;struct Cyc_Yystacktype _TB33;struct Cyc_Yyltype _TB34;unsigned _TB35;unsigned _TB36;struct Cyc_Yystacktype*_TB37;union Cyc_YYSTYPE*_TB38;union Cyc_YYSTYPE*_TB39;struct Cyc_Parse_Abstractdeclarator _TB3A;struct Cyc_Warn_String_Warn_Warg_struct _TB3B;struct Cyc_Yystacktype*_TB3C;struct Cyc_Yystacktype _TB3D;struct Cyc_Yyltype _TB3E;unsigned _TB3F;unsigned _TB40;struct _fat_ptr _TB41;struct Cyc_Warn_String_Warn_Warg_struct _TB42;struct Cyc_Yystacktype*_TB43;struct Cyc_Yystacktype _TB44;struct Cyc_Yyltype _TB45;unsigned _TB46;unsigned _TB47;struct _fat_ptr _TB48;struct _tuple8*_TB49;struct Cyc_Yystacktype*_TB4A;union Cyc_YYSTYPE*_TB4B;union Cyc_YYSTYPE*_TB4C;struct Cyc_List_List*_TB4D;struct Cyc_List_List*_TB4E;struct Cyc_List_List*_TB4F;struct _fat_ptr*_TB50;struct Cyc_Yystacktype*_TB51;union Cyc_YYSTYPE*_TB52;union Cyc_YYSTYPE*_TB53;struct Cyc_List_List*_TB54;struct _fat_ptr*_TB55;struct Cyc_Yystacktype*_TB56;union Cyc_YYSTYPE*_TB57;union Cyc_YYSTYPE*_TB58;struct Cyc_Yystacktype*_TB59;union Cyc_YYSTYPE*_TB5A;union Cyc_YYSTYPE*_TB5B;struct Cyc_Yystacktype*_TB5C;struct Cyc_Yystacktype _TB5D;struct Cyc_Yystacktype*_TB5E;struct Cyc_Yystacktype _TB5F;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB60;void*_TB61;struct Cyc_Yystacktype*_TB62;struct Cyc_Yystacktype _TB63;struct Cyc_Yyltype _TB64;unsigned _TB65;unsigned _TB66;struct Cyc_Absyn_Exp*_TB67;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB68;struct Cyc_Yystacktype*_TB69;union Cyc_YYSTYPE*_TB6A;union Cyc_YYSTYPE*_TB6B;struct Cyc_List_List*_TB6C;void*_TB6D;struct Cyc_Yystacktype*_TB6E;struct Cyc_Yystacktype _TB6F;struct Cyc_Yyltype _TB70;unsigned _TB71;unsigned _TB72;struct Cyc_Absyn_Exp*_TB73;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TB74;struct Cyc_Yystacktype*_TB75;union Cyc_YYSTYPE*_TB76;union Cyc_YYSTYPE*_TB77;struct Cyc_List_List*_TB78;void*_TB79;struct Cyc_Yystacktype*_TB7A;struct Cyc_Yystacktype _TB7B;struct Cyc_Yyltype _TB7C;unsigned _TB7D;unsigned _TB7E;struct Cyc_Absyn_Exp*_TB7F;struct Cyc_Yystacktype*_TB80;struct Cyc_Yystacktype _TB81;struct Cyc_Yyltype _TB82;unsigned _TB83;unsigned _TB84;struct _tuple0*_TB85;struct _fat_ptr*_TB86;struct Cyc_Yystacktype*_TB87;union Cyc_YYSTYPE*_TB88;union Cyc_YYSTYPE*_TB89;void*_TB8A;struct Cyc_Yystacktype*_TB8B;struct Cyc_Yystacktype _TB8C;struct Cyc_Yyltype _TB8D;unsigned _TB8E;unsigned _TB8F;struct Cyc_Absyn_Exp*_TB90;struct Cyc_Absyn_Vardecl*_TB91;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TB92;struct Cyc_Yystacktype*_TB93;union Cyc_YYSTYPE*_TB94;union Cyc_YYSTYPE*_TB95;struct Cyc_Yystacktype*_TB96;union Cyc_YYSTYPE*_TB97;union Cyc_YYSTYPE*_TB98;void*_TB99;struct Cyc_Yystacktype*_TB9A;struct Cyc_Yystacktype _TB9B;struct Cyc_Yyltype _TB9C;unsigned _TB9D;unsigned _TB9E;struct Cyc_Absyn_Exp*_TB9F;struct Cyc_Yystacktype*_TBA0;union Cyc_YYSTYPE*_TBA1;union Cyc_YYSTYPE*_TBA2;struct _tuple8*_TBA3;struct Cyc_Yystacktype*_TBA4;struct Cyc_Yystacktype _TBA5;struct Cyc_Yyltype _TBA6;unsigned _TBA7;unsigned _TBA8;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TBA9;struct Cyc_Yystacktype*_TBAA;union Cyc_YYSTYPE*_TBAB;union Cyc_YYSTYPE*_TBAC;void*_TBAD;struct Cyc_Yystacktype*_TBAE;struct Cyc_Yystacktype _TBAF;struct Cyc_Yyltype _TBB0;unsigned _TBB1;unsigned _TBB2;struct Cyc_Absyn_Exp*_TBB3;struct Cyc_List_List*_TBB4;struct _tuple34*_TBB5;struct Cyc_Yystacktype*_TBB6;union Cyc_YYSTYPE*_TBB7;union Cyc_YYSTYPE*_TBB8;struct Cyc_List_List*_TBB9;struct _tuple34*_TBBA;struct Cyc_Yystacktype*_TBBB;union Cyc_YYSTYPE*_TBBC;union Cyc_YYSTYPE*_TBBD;struct Cyc_Yystacktype*_TBBE;union Cyc_YYSTYPE*_TBBF;union Cyc_YYSTYPE*_TBC0;struct Cyc_List_List*_TBC1;struct _tuple34*_TBC2;struct Cyc_Yystacktype*_TBC3;union Cyc_YYSTYPE*_TBC4;union Cyc_YYSTYPE*_TBC5;struct Cyc_Yystacktype*_TBC6;union Cyc_YYSTYPE*_TBC7;union Cyc_YYSTYPE*_TBC8;struct Cyc_List_List*_TBC9;struct _tuple34*_TBCA;struct Cyc_Yystacktype*_TBCB;union Cyc_YYSTYPE*_TBCC;union Cyc_YYSTYPE*_TBCD;struct Cyc_Yystacktype*_TBCE;union Cyc_YYSTYPE*_TBCF;union Cyc_YYSTYPE*_TBD0;struct Cyc_Yystacktype*_TBD1;union Cyc_YYSTYPE*_TBD2;union Cyc_YYSTYPE*_TBD3;struct Cyc_Yystacktype*_TBD4;union Cyc_YYSTYPE*_TBD5;union Cyc_YYSTYPE*_TBD6;struct Cyc_List_List*_TBD7;struct Cyc_List_List*_TBD8;struct Cyc_List_List*_TBD9;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBDA;struct _fat_ptr*_TBDB;struct Cyc_Yystacktype*_TBDC;union Cyc_YYSTYPE*_TBDD;union Cyc_YYSTYPE*_TBDE;struct Cyc_List_List*_TBDF;struct Cyc_Yystacktype*_TBE0;union Cyc_YYSTYPE*_TBE1;union Cyc_YYSTYPE*_TBE2;struct Cyc_List_List*_TBE3;struct Cyc_Yystacktype*_TBE4;union Cyc_YYSTYPE*_TBE5;union Cyc_YYSTYPE*_TBE6;struct Cyc_Yystacktype*_TBE7;union Cyc_YYSTYPE*_TBE8;union Cyc_YYSTYPE*_TBE9;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_TBEA;struct Cyc_Yystacktype*_TBEB;union Cyc_YYSTYPE*_TBEC;union Cyc_YYSTYPE*_TBED;void*_TBEE;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_TBEF;struct _fat_ptr*_TBF0;struct Cyc_Yystacktype*_TBF1;union Cyc_YYSTYPE*_TBF2;union Cyc_YYSTYPE*_TBF3;void*_TBF4;struct Cyc_Yystacktype*_TBF5;union Cyc_YYSTYPE*_TBF6;union Cyc_YYSTYPE*_TBF7;struct Cyc_Parse_Type_specifier _TBF8;struct Cyc_Yystacktype*_TBF9;struct Cyc_Yystacktype _TBFA;struct Cyc_Yyltype _TBFB;unsigned _TBFC;unsigned _TBFD;struct Cyc_Yystacktype*_TBFE;struct Cyc_Yystacktype _TBFF;struct Cyc_Yyltype _TC00;unsigned _TC01;unsigned _TC02;struct _fat_ptr _TC03;struct _fat_ptr _TC04;struct _tuple8*_TC05;struct Cyc_Yystacktype*_TC06;union Cyc_YYSTYPE*_TC07;union Cyc_YYSTYPE*_TC08;struct Cyc_Parse_Type_specifier _TC09;struct Cyc_Yystacktype*_TC0A;struct Cyc_Yystacktype _TC0B;struct Cyc_Yyltype _TC0C;unsigned _TC0D;unsigned _TC0E;struct Cyc_Yystacktype*_TC0F;union Cyc_YYSTYPE*_TC10;union Cyc_YYSTYPE*_TC11;struct Cyc_Parse_Abstractdeclarator _TC12;struct _tuple14 _TC13;struct Cyc_List_List*_TC14;struct Cyc_Yystacktype*_TC15;struct Cyc_Yystacktype _TC16;struct Cyc_Yyltype _TC17;unsigned _TC18;unsigned _TC19;struct _fat_ptr _TC1A;struct _fat_ptr _TC1B;struct _tuple14 _TC1C;struct Cyc_List_List*_TC1D;struct Cyc_Yystacktype*_TC1E;struct Cyc_Yystacktype _TC1F;struct Cyc_Yyltype _TC20;unsigned _TC21;unsigned _TC22;struct _fat_ptr _TC23;struct _fat_ptr _TC24;struct _tuple8*_TC25;struct _tuple14 _TC26;struct _tuple14 _TC27;struct Cyc_Yystacktype*_TC28;union Cyc_YYSTYPE*_TC29;union Cyc_YYSTYPE*_TC2A;struct _tuple8*_TC2B;struct Cyc_Yystacktype*_TC2C;struct Cyc_Yystacktype _TC2D;struct Cyc_Yyltype _TC2E;unsigned _TC2F;unsigned _TC30;void*_TC31;void*_TC32;struct Cyc_Yystacktype*_TC33;union Cyc_YYSTYPE*_TC34;union Cyc_YYSTYPE*_TC35;struct Cyc_List_List*_TC36;void*_TC37;struct Cyc_Yystacktype*_TC38;union Cyc_YYSTYPE*_TC39;union Cyc_YYSTYPE*_TC3A;void*_TC3B;void*_TC3C;struct Cyc_List_List*_TC3D;struct Cyc_Yystacktype*_TC3E;union Cyc_YYSTYPE*_TC3F;union Cyc_YYSTYPE*_TC40;struct Cyc_Yystacktype*_TC41;union Cyc_YYSTYPE*_TC42;union Cyc_YYSTYPE*_TC43;void*_TC44;struct Cyc_Yystacktype*_TC45;union Cyc_YYSTYPE*_TC46;union Cyc_YYSTYPE*_TC47;void*_TC48;struct Cyc_List_List*_TC49;struct Cyc_Yystacktype*_TC4A;union Cyc_YYSTYPE*_TC4B;union Cyc_YYSTYPE*_TC4C;struct Cyc_List_List*_TC4D;struct Cyc_Yystacktype*_TC4E;union Cyc_YYSTYPE*_TC4F;union Cyc_YYSTYPE*_TC50;struct Cyc_Yystacktype*_TC51;union Cyc_YYSTYPE*_TC52;union Cyc_YYSTYPE*_TC53;struct Cyc_Parse_Abstractdeclarator _TC54;struct Cyc_Yystacktype*_TC55;union Cyc_YYSTYPE*_TC56;union Cyc_YYSTYPE*_TC57;struct Cyc_Yystacktype*_TC58;struct Cyc_Yystacktype _TC59;struct Cyc_Parse_Abstractdeclarator _TC5A;struct Cyc_Yystacktype*_TC5B;union Cyc_YYSTYPE*_TC5C;union Cyc_YYSTYPE*_TC5D;struct Cyc_List_List*_TC5E;struct Cyc_Yystacktype*_TC5F;union Cyc_YYSTYPE*_TC60;union Cyc_YYSTYPE*_TC61;struct Cyc_Parse_Abstractdeclarator _TC62;struct Cyc_List_List*_TC63;struct Cyc_Yystacktype*_TC64;struct Cyc_Yystacktype _TC65;struct Cyc_Parse_Abstractdeclarator _TC66;struct Cyc_List_List*_TC67;struct _RegionHandle*_TC68;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC69;struct _RegionHandle*_TC6A;struct Cyc_Yystacktype*_TC6B;union Cyc_YYSTYPE*_TC6C;union Cyc_YYSTYPE*_TC6D;struct Cyc_Yystacktype*_TC6E;struct Cyc_Yystacktype _TC6F;struct Cyc_Yyltype _TC70;unsigned _TC71;struct Cyc_Parse_Abstractdeclarator _TC72;struct Cyc_List_List*_TC73;struct _RegionHandle*_TC74;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_TC75;struct _RegionHandle*_TC76;struct Cyc_Yystacktype*_TC77;union Cyc_YYSTYPE*_TC78;union Cyc_YYSTYPE*_TC79;struct Cyc_Yystacktype*_TC7A;struct Cyc_Yystacktype _TC7B;struct Cyc_Yyltype _TC7C;unsigned _TC7D;struct Cyc_Yystacktype*_TC7E;union Cyc_YYSTYPE*_TC7F;union Cyc_YYSTYPE*_TC80;struct Cyc_Parse_Abstractdeclarator _TC81;struct Cyc_Parse_Abstractdeclarator _TC82;struct Cyc_List_List*_TC83;struct _RegionHandle*_TC84;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TC85;struct _RegionHandle*_TC86;struct Cyc_Yystacktype*_TC87;union Cyc_YYSTYPE*_TC88;union Cyc_YYSTYPE*_TC89;struct Cyc_Yystacktype*_TC8A;union Cyc_YYSTYPE*_TC8B;union Cyc_YYSTYPE*_TC8C;struct Cyc_Yystacktype*_TC8D;struct Cyc_Yystacktype _TC8E;struct Cyc_Yyltype _TC8F;unsigned _TC90;struct Cyc_Parse_Abstractdeclarator _TC91;struct Cyc_List_List*_TC92;struct _RegionHandle*_TC93;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_TC94;struct _RegionHandle*_TC95;struct Cyc_Yystacktype*_TC96;union Cyc_YYSTYPE*_TC97;union Cyc_YYSTYPE*_TC98;struct Cyc_Yystacktype*_TC99;union Cyc_YYSTYPE*_TC9A;union Cyc_YYSTYPE*_TC9B;struct Cyc_Yystacktype*_TC9C;struct Cyc_Yystacktype _TC9D;struct Cyc_Yyltype _TC9E;unsigned _TC9F;struct Cyc_Yystacktype*_TCA0;union Cyc_YYSTYPE*_TCA1;union Cyc_YYSTYPE*_TCA2;struct Cyc_Parse_Abstractdeclarator _TCA3;struct Cyc_Yystacktype*_TCA4;union Cyc_YYSTYPE*_TCA5;union Cyc_YYSTYPE*_TCA6;struct Cyc_Yystacktype*_TCA7;union Cyc_YYSTYPE*_TCA8;union Cyc_YYSTYPE*_TCA9;struct Cyc_Parse_Abstractdeclarator _TCAA;struct Cyc_List_List*_TCAB;struct _RegionHandle*_TCAC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCAD;struct _RegionHandle*_TCAE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCAF;struct _RegionHandle*_TCB0;struct Cyc_Yystacktype*_TCB1;union Cyc_YYSTYPE*_TCB2;union Cyc_YYSTYPE*_TCB3;struct Cyc_Yystacktype*_TCB4;union Cyc_YYSTYPE*_TCB5;union Cyc_YYSTYPE*_TCB6;struct Cyc_Parse_Abstractdeclarator _TCB7;struct Cyc_List_List*_TCB8;struct _RegionHandle*_TCB9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_TCBA;struct _RegionHandle*_TCBB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_TCBC;struct _RegionHandle*_TCBD;struct Cyc_Yystacktype*_TCBE;union Cyc_YYSTYPE*_TCBF;union Cyc_YYSTYPE*_TCC0;struct Cyc_Parse_Abstractdeclarator _TCC1;struct Cyc_List_List*(*_TCC2)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_TCC3)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_TCC4;struct Cyc_Yystacktype _TCC5;struct Cyc_Yyltype _TCC6;unsigned _TCC7;unsigned _TCC8;struct Cyc_Yystacktype*_TCC9;union Cyc_YYSTYPE*_TCCA;union Cyc_YYSTYPE*_TCCB;struct Cyc_List_List*_TCCC;struct Cyc_List_List*_TCCD;struct Cyc_Parse_Abstractdeclarator _TCCE;struct Cyc_List_List*_TCCF;struct _RegionHandle*_TCD0;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_TCD1;struct _RegionHandle*_TCD2;struct Cyc_Yystacktype*_TCD3;struct Cyc_Yystacktype _TCD4;struct Cyc_Yyltype _TCD5;unsigned _TCD6;struct Cyc_Yystacktype*_TCD7;union Cyc_YYSTYPE*_TCD8;union Cyc_YYSTYPE*_TCD9;struct Cyc_Parse_Abstractdeclarator _TCDA;struct Cyc_Parse_Abstractdeclarator _TCDB;struct Cyc_List_List*_TCDC;struct _RegionHandle*_TCDD;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_TCDE;struct _RegionHandle*_TCDF;struct Cyc_Yystacktype*_TCE0;struct Cyc_Yystacktype _TCE1;struct Cyc_Yyltype _TCE2;unsigned _TCE3;struct Cyc_Yystacktype*_TCE4;union Cyc_YYSTYPE*_TCE5;union Cyc_YYSTYPE*_TCE6;struct Cyc_Yystacktype*_TCE7;union Cyc_YYSTYPE*_TCE8;union Cyc_YYSTYPE*_TCE9;struct Cyc_Parse_Abstractdeclarator _TCEA;struct _tuple21 _TCEB;struct Cyc_Yystacktype*_TCEC;union Cyc_YYSTYPE*_TCED;union Cyc_YYSTYPE*_TCEE;struct _tuple21 _TCEF;struct Cyc_Yystacktype*_TCF0;union Cyc_YYSTYPE*_TCF1;union Cyc_YYSTYPE*_TCF2;struct _tuple21 _TCF3;struct Cyc_Yystacktype*_TCF4;union Cyc_YYSTYPE*_TCF5;union Cyc_YYSTYPE*_TCF6;struct _tuple21 _TCF7;struct Cyc_Yystacktype*_TCF8;union Cyc_YYSTYPE*_TCF9;union Cyc_YYSTYPE*_TCFA;struct _tuple21 _TCFB;struct Cyc_Yystacktype*_TCFC;union Cyc_YYSTYPE*_TCFD;union Cyc_YYSTYPE*_TCFE;struct Cyc_Yystacktype*_TCFF;union Cyc_YYSTYPE*_TD00;union Cyc_YYSTYPE*_TD01;struct _tuple21 _TD02;struct _tuple21 _TD03;struct _tuple21 _TD04;struct Cyc_Yystacktype*_TD05;union Cyc_YYSTYPE*_TD06;union Cyc_YYSTYPE*_TD07;struct Cyc_Yystacktype*_TD08;union Cyc_YYSTYPE*_TD09;union Cyc_YYSTYPE*_TD0A;struct _tuple21 _TD0B;struct _tuple21 _TD0C;struct _tuple21 _TD0D;struct Cyc_Yystacktype*_TD0E;union Cyc_YYSTYPE*_TD0F;union Cyc_YYSTYPE*_TD10;struct Cyc_Yystacktype*_TD11;union Cyc_YYSTYPE*_TD12;union Cyc_YYSTYPE*_TD13;struct _tuple21 _TD14;struct _tuple21 _TD15;struct _tuple21 _TD16;struct Cyc_Yystacktype*_TD17;union Cyc_YYSTYPE*_TD18;union Cyc_YYSTYPE*_TD19;struct Cyc_Yystacktype*_TD1A;union Cyc_YYSTYPE*_TD1B;union Cyc_YYSTYPE*_TD1C;struct _tuple21 _TD1D;struct _tuple21 _TD1E;struct _tuple21 _TD1F;struct Cyc_Yystacktype*_TD20;struct Cyc_Yystacktype _TD21;struct Cyc_Yystacktype*_TD22;struct Cyc_Yystacktype _TD23;struct Cyc_Yystacktype*_TD24;struct Cyc_Yystacktype _TD25;struct Cyc_Yystacktype*_TD26;struct Cyc_Yystacktype _TD27;struct Cyc_Yystacktype*_TD28;struct Cyc_Yystacktype _TD29;struct Cyc_Yystacktype*_TD2A;struct Cyc_Yystacktype _TD2B;struct Cyc_Yystacktype*_TD2C;struct Cyc_Yystacktype _TD2D;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD2E;struct _fat_ptr*_TD2F;struct Cyc_Yystacktype*_TD30;union Cyc_YYSTYPE*_TD31;union Cyc_YYSTYPE*_TD32;struct Cyc_Yystacktype*_TD33;union Cyc_YYSTYPE*_TD34;union Cyc_YYSTYPE*_TD35;void*_TD36;struct Cyc_Yystacktype*_TD37;struct Cyc_Yystacktype _TD38;struct Cyc_Yyltype _TD39;unsigned _TD3A;unsigned _TD3B;struct Cyc_Absyn_Stmt*_TD3C;struct Cyc_Yystacktype*_TD3D;struct Cyc_Yystacktype _TD3E;struct Cyc_Yyltype _TD3F;unsigned _TD40;unsigned _TD41;struct Cyc_Absyn_Stmt*_TD42;struct Cyc_Yystacktype*_TD43;union Cyc_YYSTYPE*_TD44;union Cyc_YYSTYPE*_TD45;struct Cyc_Absyn_Exp*_TD46;struct Cyc_Yystacktype*_TD47;struct Cyc_Yystacktype _TD48;struct Cyc_Yyltype _TD49;unsigned _TD4A;unsigned _TD4B;struct Cyc_Absyn_Stmt*_TD4C;struct Cyc_Yystacktype*_TD4D;struct Cyc_Yystacktype _TD4E;struct Cyc_Yyltype _TD4F;unsigned _TD50;unsigned _TD51;struct Cyc_Absyn_Stmt*_TD52;struct Cyc_Yystacktype*_TD53;struct Cyc_Yystacktype _TD54;struct Cyc_Yystacktype*_TD55;struct Cyc_Yystacktype _TD56;struct Cyc_Yyltype _TD57;unsigned _TD58;unsigned _TD59;struct Cyc_Absyn_Stmt*_TD5A;struct Cyc_Yystacktype*_TD5B;struct Cyc_Yystacktype _TD5C;struct Cyc_Yystacktype*_TD5D;union Cyc_YYSTYPE*_TD5E;union Cyc_YYSTYPE*_TD5F;struct Cyc_List_List*_TD60;struct Cyc_Yystacktype*_TD61;struct Cyc_Yystacktype _TD62;struct Cyc_Yyltype _TD63;unsigned _TD64;unsigned _TD65;struct Cyc_Absyn_Stmt*_TD66;struct Cyc_Absyn_Stmt*_TD67;struct Cyc_Yystacktype*_TD68;union Cyc_YYSTYPE*_TD69;union Cyc_YYSTYPE*_TD6A;struct Cyc_List_List*_TD6B;struct Cyc_Yystacktype*_TD6C;union Cyc_YYSTYPE*_TD6D;union Cyc_YYSTYPE*_TD6E;struct Cyc_Absyn_Stmt*_TD6F;struct Cyc_Absyn_Stmt*_TD70;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD71;struct _fat_ptr*_TD72;struct Cyc_Yystacktype*_TD73;union Cyc_YYSTYPE*_TD74;union Cyc_YYSTYPE*_TD75;struct Cyc_Yystacktype*_TD76;union Cyc_YYSTYPE*_TD77;union Cyc_YYSTYPE*_TD78;struct Cyc_List_List*_TD79;struct Cyc_Absyn_Stmt*_TD7A;void*_TD7B;struct Cyc_Yystacktype*_TD7C;struct Cyc_Yystacktype _TD7D;struct Cyc_Yyltype _TD7E;unsigned _TD7F;unsigned _TD80;struct Cyc_Absyn_Stmt*_TD81;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_TD82;struct _fat_ptr*_TD83;struct Cyc_Yystacktype*_TD84;union Cyc_YYSTYPE*_TD85;union Cyc_YYSTYPE*_TD86;struct Cyc_Yystacktype*_TD87;union Cyc_YYSTYPE*_TD88;union Cyc_YYSTYPE*_TD89;struct Cyc_List_List*_TD8A;struct Cyc_Yystacktype*_TD8B;union Cyc_YYSTYPE*_TD8C;union Cyc_YYSTYPE*_TD8D;struct Cyc_Absyn_Stmt*_TD8E;void*_TD8F;struct Cyc_Yystacktype*_TD90;struct Cyc_Yystacktype _TD91;struct Cyc_Yyltype _TD92;unsigned _TD93;unsigned _TD94;struct Cyc_Absyn_Stmt*_TD95;struct Cyc_Yystacktype*_TD96;struct Cyc_Yystacktype _TD97;struct Cyc_Yystacktype*_TD98;union Cyc_YYSTYPE*_TD99;union Cyc_YYSTYPE*_TD9A;struct Cyc_Absyn_Stmt*_TD9B;struct Cyc_Yystacktype*_TD9C;union Cyc_YYSTYPE*_TD9D;union Cyc_YYSTYPE*_TD9E;struct Cyc_Absyn_Stmt*_TD9F;struct Cyc_Yystacktype*_TDA0;struct Cyc_Yystacktype _TDA1;struct Cyc_Yyltype _TDA2;unsigned _TDA3;unsigned _TDA4;struct Cyc_Absyn_Stmt*_TDA5;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDA6;struct Cyc_Yystacktype*_TDA7;union Cyc_YYSTYPE*_TDA8;union Cyc_YYSTYPE*_TDA9;void*_TDAA;struct Cyc_Yystacktype*_TDAB;struct Cyc_Yystacktype _TDAC;struct Cyc_Yyltype _TDAD;unsigned _TDAE;unsigned _TDAF;struct Cyc_Absyn_Decl*_TDB0;struct Cyc_Absyn_Stmt*_TDB1;struct Cyc_Absyn_Stmt*_TDB2;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_TDB3;struct Cyc_Yystacktype*_TDB4;union Cyc_YYSTYPE*_TDB5;union Cyc_YYSTYPE*_TDB6;void*_TDB7;struct Cyc_Yystacktype*_TDB8;struct Cyc_Yystacktype _TDB9;struct Cyc_Yyltype _TDBA;unsigned _TDBB;unsigned _TDBC;struct Cyc_Absyn_Decl*_TDBD;struct Cyc_Yystacktype*_TDBE;union Cyc_YYSTYPE*_TDBF;union Cyc_YYSTYPE*_TDC0;struct Cyc_Absyn_Stmt*_TDC1;struct Cyc_Absyn_Stmt*_TDC2;struct Cyc_Yystacktype*_TDC3;union Cyc_YYSTYPE*_TDC4;union Cyc_YYSTYPE*_TDC5;struct Cyc_Absyn_Exp*_TDC6;struct Cyc_Yystacktype*_TDC7;union Cyc_YYSTYPE*_TDC8;union Cyc_YYSTYPE*_TDC9;struct Cyc_Absyn_Stmt*_TDCA;struct Cyc_Absyn_Stmt*_TDCB;struct Cyc_Yystacktype*_TDCC;struct Cyc_Yystacktype _TDCD;struct Cyc_Yyltype _TDCE;unsigned _TDCF;unsigned _TDD0;struct Cyc_Absyn_Stmt*_TDD1;struct Cyc_Yystacktype*_TDD2;union Cyc_YYSTYPE*_TDD3;union Cyc_YYSTYPE*_TDD4;struct Cyc_Absyn_Exp*_TDD5;struct Cyc_Yystacktype*_TDD6;union Cyc_YYSTYPE*_TDD7;union Cyc_YYSTYPE*_TDD8;struct Cyc_Absyn_Stmt*_TDD9;struct Cyc_Yystacktype*_TDDA;union Cyc_YYSTYPE*_TDDB;union Cyc_YYSTYPE*_TDDC;struct Cyc_Absyn_Stmt*_TDDD;struct Cyc_Yystacktype*_TDDE;struct Cyc_Yystacktype _TDDF;struct Cyc_Yyltype _TDE0;unsigned _TDE1;unsigned _TDE2;struct Cyc_Absyn_Stmt*_TDE3;struct Cyc_Yystacktype*_TDE4;union Cyc_YYSTYPE*_TDE5;union Cyc_YYSTYPE*_TDE6;struct Cyc_Absyn_Exp*_TDE7;struct Cyc_Yystacktype*_TDE8;union Cyc_YYSTYPE*_TDE9;union Cyc_YYSTYPE*_TDEA;struct Cyc_List_List*_TDEB;struct Cyc_Yystacktype*_TDEC;struct Cyc_Yystacktype _TDED;struct Cyc_Yyltype _TDEE;unsigned _TDEF;unsigned _TDF0;struct Cyc_Absyn_Stmt*_TDF1;struct Cyc_Yystacktype*_TDF2;union Cyc_YYSTYPE*_TDF3;union Cyc_YYSTYPE*_TDF4;struct _tuple0*_TDF5;struct Cyc_Yystacktype*_TDF6;struct Cyc_Yystacktype _TDF7;struct Cyc_Yyltype _TDF8;unsigned _TDF9;unsigned _TDFA;struct Cyc_Absyn_Exp*_TDFB;struct Cyc_Yystacktype*_TDFC;union Cyc_YYSTYPE*_TDFD;union Cyc_YYSTYPE*_TDFE;struct Cyc_List_List*_TDFF;struct Cyc_Yystacktype*_TE00;struct Cyc_Yystacktype _TE01;struct Cyc_Yyltype _TE02;unsigned _TE03;unsigned _TE04;struct Cyc_Absyn_Stmt*_TE05;struct Cyc_Yystacktype*_TE06;union Cyc_YYSTYPE*_TE07;union Cyc_YYSTYPE*_TE08;struct Cyc_List_List*_TE09;struct Cyc_Yystacktype*_TE0A;struct Cyc_Yystacktype _TE0B;struct Cyc_Yyltype _TE0C;unsigned _TE0D;unsigned _TE0E;struct Cyc_Absyn_Exp*_TE0F;struct Cyc_Yystacktype*_TE10;union Cyc_YYSTYPE*_TE11;union Cyc_YYSTYPE*_TE12;struct Cyc_List_List*_TE13;struct Cyc_Yystacktype*_TE14;struct Cyc_Yystacktype _TE15;struct Cyc_Yyltype _TE16;unsigned _TE17;unsigned _TE18;struct Cyc_Absyn_Stmt*_TE19;struct Cyc_Yystacktype*_TE1A;union Cyc_YYSTYPE*_TE1B;union Cyc_YYSTYPE*_TE1C;struct Cyc_Absyn_Stmt*_TE1D;struct Cyc_Yystacktype*_TE1E;union Cyc_YYSTYPE*_TE1F;union Cyc_YYSTYPE*_TE20;struct Cyc_List_List*_TE21;struct Cyc_Yystacktype*_TE22;struct Cyc_Yystacktype _TE23;struct Cyc_Yyltype _TE24;unsigned _TE25;unsigned _TE26;struct Cyc_Absyn_Stmt*_TE27;struct Cyc_List_List*_TE28;struct Cyc_Absyn_Switch_clause*_TE29;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE2A;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TE2B;void*_TE2C;struct Cyc_Yystacktype*_TE2D;struct Cyc_Yystacktype _TE2E;struct Cyc_Yyltype _TE2F;unsigned _TE30;unsigned _TE31;struct Cyc_Yystacktype*_TE32;union Cyc_YYSTYPE*_TE33;union Cyc_YYSTYPE*_TE34;struct Cyc_Yystacktype*_TE35;struct Cyc_Yystacktype _TE36;struct Cyc_Yyltype _TE37;unsigned _TE38;struct Cyc_Yystacktype*_TE39;union Cyc_YYSTYPE*_TE3A;union Cyc_YYSTYPE*_TE3B;struct Cyc_List_List*_TE3C;struct Cyc_Absyn_Switch_clause*_TE3D;struct Cyc_Yystacktype*_TE3E;union Cyc_YYSTYPE*_TE3F;union Cyc_YYSTYPE*_TE40;struct Cyc_Yystacktype*_TE41;struct Cyc_Yystacktype _TE42;struct Cyc_Yyltype _TE43;unsigned _TE44;unsigned _TE45;struct Cyc_Yystacktype*_TE46;struct Cyc_Yystacktype _TE47;struct Cyc_Yyltype _TE48;unsigned _TE49;struct Cyc_Yystacktype*_TE4A;union Cyc_YYSTYPE*_TE4B;union Cyc_YYSTYPE*_TE4C;struct Cyc_List_List*_TE4D;struct Cyc_Absyn_Switch_clause*_TE4E;struct Cyc_Yystacktype*_TE4F;union Cyc_YYSTYPE*_TE50;union Cyc_YYSTYPE*_TE51;struct Cyc_Yystacktype*_TE52;union Cyc_YYSTYPE*_TE53;union Cyc_YYSTYPE*_TE54;struct Cyc_Yystacktype*_TE55;struct Cyc_Yystacktype _TE56;struct Cyc_Yyltype _TE57;unsigned _TE58;struct Cyc_Yystacktype*_TE59;union Cyc_YYSTYPE*_TE5A;union Cyc_YYSTYPE*_TE5B;struct Cyc_List_List*_TE5C;struct Cyc_Absyn_Switch_clause*_TE5D;struct Cyc_Yystacktype*_TE5E;union Cyc_YYSTYPE*_TE5F;union Cyc_YYSTYPE*_TE60;struct Cyc_Yystacktype*_TE61;union Cyc_YYSTYPE*_TE62;union Cyc_YYSTYPE*_TE63;struct Cyc_Yystacktype*_TE64;struct Cyc_Yystacktype _TE65;struct Cyc_Yyltype _TE66;unsigned _TE67;unsigned _TE68;struct Cyc_Yystacktype*_TE69;struct Cyc_Yystacktype _TE6A;struct Cyc_Yyltype _TE6B;unsigned _TE6C;struct Cyc_Yystacktype*_TE6D;union Cyc_YYSTYPE*_TE6E;union Cyc_YYSTYPE*_TE6F;struct Cyc_List_List*_TE70;struct Cyc_Absyn_Switch_clause*_TE71;struct Cyc_Yystacktype*_TE72;union Cyc_YYSTYPE*_TE73;union Cyc_YYSTYPE*_TE74;struct Cyc_Yystacktype*_TE75;union Cyc_YYSTYPE*_TE76;union Cyc_YYSTYPE*_TE77;struct Cyc_Yystacktype*_TE78;union Cyc_YYSTYPE*_TE79;union Cyc_YYSTYPE*_TE7A;struct Cyc_Yystacktype*_TE7B;struct Cyc_Yystacktype _TE7C;struct Cyc_Yyltype _TE7D;unsigned _TE7E;struct Cyc_Yystacktype*_TE7F;union Cyc_YYSTYPE*_TE80;union Cyc_YYSTYPE*_TE81;struct Cyc_Yystacktype*_TE82;union Cyc_YYSTYPE*_TE83;union Cyc_YYSTYPE*_TE84;struct Cyc_Absyn_Exp*_TE85;struct Cyc_Yystacktype*_TE86;union Cyc_YYSTYPE*_TE87;union Cyc_YYSTYPE*_TE88;struct Cyc_Absyn_Stmt*_TE89;struct Cyc_Yystacktype*_TE8A;struct Cyc_Yystacktype _TE8B;struct Cyc_Yyltype _TE8C;unsigned _TE8D;unsigned _TE8E;struct Cyc_Absyn_Stmt*_TE8F;struct Cyc_Yystacktype*_TE90;union Cyc_YYSTYPE*_TE91;union Cyc_YYSTYPE*_TE92;struct Cyc_Absyn_Stmt*_TE93;struct Cyc_Yystacktype*_TE94;union Cyc_YYSTYPE*_TE95;union Cyc_YYSTYPE*_TE96;struct Cyc_Absyn_Exp*_TE97;struct Cyc_Yystacktype*_TE98;struct Cyc_Yystacktype _TE99;struct Cyc_Yyltype _TE9A;unsigned _TE9B;unsigned _TE9C;struct Cyc_Absyn_Stmt*_TE9D;struct Cyc_Yystacktype*_TE9E;union Cyc_YYSTYPE*_TE9F;union Cyc_YYSTYPE*_TEA0;struct Cyc_Absyn_Exp*_TEA1;struct Cyc_Yystacktype*_TEA2;union Cyc_YYSTYPE*_TEA3;union Cyc_YYSTYPE*_TEA4;struct Cyc_Absyn_Exp*_TEA5;struct Cyc_Yystacktype*_TEA6;union Cyc_YYSTYPE*_TEA7;union Cyc_YYSTYPE*_TEA8;struct Cyc_Absyn_Exp*_TEA9;struct Cyc_Yystacktype*_TEAA;union Cyc_YYSTYPE*_TEAB;union Cyc_YYSTYPE*_TEAC;struct Cyc_Absyn_Stmt*_TEAD;struct Cyc_Yystacktype*_TEAE;struct Cyc_Yystacktype _TEAF;struct Cyc_Yyltype _TEB0;unsigned _TEB1;unsigned _TEB2;struct Cyc_Absyn_Stmt*_TEB3;struct Cyc_Absyn_Exp*_TEB4;struct Cyc_Yystacktype*_TEB5;union Cyc_YYSTYPE*_TEB6;union Cyc_YYSTYPE*_TEB7;struct Cyc_Absyn_Exp*_TEB8;struct Cyc_Yystacktype*_TEB9;union Cyc_YYSTYPE*_TEBA;union Cyc_YYSTYPE*_TEBB;struct Cyc_Absyn_Exp*_TEBC;struct Cyc_Yystacktype*_TEBD;union Cyc_YYSTYPE*_TEBE;union Cyc_YYSTYPE*_TEBF;struct Cyc_Absyn_Stmt*_TEC0;struct Cyc_Yystacktype*_TEC1;struct Cyc_Yystacktype _TEC2;struct Cyc_Yyltype _TEC3;unsigned _TEC4;unsigned _TEC5;struct Cyc_Yystacktype*_TEC6;union Cyc_YYSTYPE*_TEC7;union Cyc_YYSTYPE*_TEC8;struct Cyc_List_List*_TEC9;struct Cyc_Absyn_Stmt*_TECA;struct Cyc_Absyn_Stmt*_TECB;struct Cyc_Absyn_Exp*_TECC;struct Cyc_Yystacktype*_TECD;struct Cyc_Yystacktype _TECE;struct _fat_ptr*_TECF;struct Cyc_Yystacktype*_TED0;union Cyc_YYSTYPE*_TED1;union Cyc_YYSTYPE*_TED2;struct Cyc_Yystacktype*_TED3;struct Cyc_Yystacktype _TED4;struct Cyc_Yyltype _TED5;unsigned _TED6;unsigned _TED7;struct Cyc_Absyn_Stmt*_TED8;struct Cyc_Yystacktype*_TED9;struct Cyc_Yystacktype _TEDA;struct Cyc_Yyltype _TEDB;unsigned _TEDC;unsigned _TEDD;struct Cyc_Absyn_Stmt*_TEDE;struct Cyc_Yystacktype*_TEDF;struct Cyc_Yystacktype _TEE0;struct Cyc_Yyltype _TEE1;unsigned _TEE2;unsigned _TEE3;struct Cyc_Absyn_Stmt*_TEE4;struct Cyc_Yystacktype*_TEE5;struct Cyc_Yystacktype _TEE6;struct Cyc_Yyltype _TEE7;unsigned _TEE8;unsigned _TEE9;struct Cyc_Absyn_Stmt*_TEEA;struct Cyc_Yystacktype*_TEEB;union Cyc_YYSTYPE*_TEEC;union Cyc_YYSTYPE*_TEED;struct Cyc_Absyn_Exp*_TEEE;struct Cyc_Yystacktype*_TEEF;struct Cyc_Yystacktype _TEF0;struct Cyc_Yyltype _TEF1;unsigned _TEF2;unsigned _TEF3;struct Cyc_Absyn_Stmt*_TEF4;struct Cyc_Yystacktype*_TEF5;struct Cyc_Yystacktype _TEF6;struct Cyc_Yyltype _TEF7;unsigned _TEF8;unsigned _TEF9;struct Cyc_Absyn_Stmt*_TEFA;struct Cyc_Yystacktype*_TEFB;struct Cyc_Yystacktype _TEFC;struct Cyc_Yyltype _TEFD;unsigned _TEFE;unsigned _TEFF;struct Cyc_Absyn_Stmt*_TF00;struct Cyc_Yystacktype*_TF01;union Cyc_YYSTYPE*_TF02;union Cyc_YYSTYPE*_TF03;struct Cyc_List_List*_TF04;struct Cyc_Yystacktype*_TF05;struct Cyc_Yystacktype _TF06;struct Cyc_Yyltype _TF07;unsigned _TF08;unsigned _TF09;struct Cyc_Absyn_Stmt*_TF0A;struct Cyc_Yystacktype*_TF0B;struct Cyc_Yystacktype _TF0C;struct Cyc_Yystacktype*_TF0D;struct Cyc_Yystacktype _TF0E;struct Cyc_Yystacktype*_TF0F;union Cyc_YYSTYPE*_TF10;union Cyc_YYSTYPE*_TF11;struct Cyc_Absyn_Pat*_TF12;struct Cyc_Absyn_Exp*_TF13;struct Cyc_Yystacktype*_TF14;union Cyc_YYSTYPE*_TF15;union Cyc_YYSTYPE*_TF16;struct Cyc_Absyn_Exp*_TF17;struct Cyc_Yystacktype*_TF18;union Cyc_YYSTYPE*_TF19;union Cyc_YYSTYPE*_TF1A;struct Cyc_Absyn_Exp*_TF1B;struct Cyc_Yystacktype*_TF1C;struct Cyc_Yystacktype _TF1D;struct Cyc_Yyltype _TF1E;unsigned _TF1F;unsigned _TF20;struct Cyc_Absyn_Exp*_TF21;struct Cyc_Absyn_Pat*_TF22;struct Cyc_Yystacktype*_TF23;struct Cyc_Yystacktype _TF24;struct Cyc_Yystacktype*_TF25;union Cyc_YYSTYPE*_TF26;union Cyc_YYSTYPE*_TF27;struct Cyc_Absyn_Pat*_TF28;struct Cyc_Absyn_Exp*_TF29;struct Cyc_Yystacktype*_TF2A;union Cyc_YYSTYPE*_TF2B;union Cyc_YYSTYPE*_TF2C;struct Cyc_Absyn_Exp*_TF2D;struct Cyc_Yystacktype*_TF2E;struct Cyc_Yystacktype _TF2F;struct Cyc_Yyltype _TF30;unsigned _TF31;unsigned _TF32;struct Cyc_Absyn_Exp*_TF33;struct Cyc_Absyn_Pat*_TF34;struct Cyc_Yystacktype*_TF35;struct Cyc_Yystacktype _TF36;struct Cyc_Yystacktype*_TF37;union Cyc_YYSTYPE*_TF38;union Cyc_YYSTYPE*_TF39;struct Cyc_Absyn_Pat*_TF3A;struct Cyc_Absyn_Exp*_TF3B;struct Cyc_Yystacktype*_TF3C;union Cyc_YYSTYPE*_TF3D;union Cyc_YYSTYPE*_TF3E;struct Cyc_Absyn_Exp*_TF3F;struct Cyc_Yystacktype*_TF40;struct Cyc_Yystacktype _TF41;struct Cyc_Yyltype _TF42;unsigned _TF43;unsigned _TF44;struct Cyc_Absyn_Exp*_TF45;struct Cyc_Absyn_Pat*_TF46;struct Cyc_Yystacktype*_TF47;struct Cyc_Yystacktype _TF48;struct Cyc_Yystacktype*_TF49;union Cyc_YYSTYPE*_TF4A;union Cyc_YYSTYPE*_TF4B;struct Cyc_Absyn_Pat*_TF4C;struct Cyc_Absyn_Exp*_TF4D;struct Cyc_Yystacktype*_TF4E;union Cyc_YYSTYPE*_TF4F;union Cyc_YYSTYPE*_TF50;struct Cyc_Absyn_Exp*_TF51;struct Cyc_Yystacktype*_TF52;struct Cyc_Yystacktype _TF53;struct Cyc_Yyltype _TF54;unsigned _TF55;unsigned _TF56;struct Cyc_Absyn_Exp*_TF57;struct Cyc_Absyn_Pat*_TF58;struct Cyc_Yystacktype*_TF59;struct Cyc_Yystacktype _TF5A;struct Cyc_Yystacktype*_TF5B;union Cyc_YYSTYPE*_TF5C;union Cyc_YYSTYPE*_TF5D;struct Cyc_Absyn_Pat*_TF5E;struct Cyc_Absyn_Exp*_TF5F;struct Cyc_Yystacktype*_TF60;union Cyc_YYSTYPE*_TF61;union Cyc_YYSTYPE*_TF62;struct Cyc_Absyn_Exp*_TF63;struct Cyc_Yystacktype*_TF64;struct Cyc_Yystacktype _TF65;struct Cyc_Yyltype _TF66;unsigned _TF67;unsigned _TF68;struct Cyc_Absyn_Exp*_TF69;struct Cyc_Absyn_Pat*_TF6A;struct Cyc_Yystacktype*_TF6B;struct Cyc_Yystacktype _TF6C;struct Cyc_Yystacktype*_TF6D;union Cyc_YYSTYPE*_TF6E;union Cyc_YYSTYPE*_TF6F;struct Cyc_Absyn_Pat*_TF70;struct Cyc_Absyn_Exp*_TF71;struct Cyc_Yystacktype*_TF72;union Cyc_YYSTYPE*_TF73;union Cyc_YYSTYPE*_TF74;struct Cyc_Absyn_Exp*_TF75;struct Cyc_Yystacktype*_TF76;struct Cyc_Yystacktype _TF77;struct Cyc_Yyltype _TF78;unsigned _TF79;unsigned _TF7A;struct Cyc_Absyn_Exp*_TF7B;struct Cyc_Absyn_Pat*_TF7C;struct Cyc_Yystacktype*_TF7D;struct Cyc_Yystacktype _TF7E;struct Cyc_Yystacktype*_TF7F;union Cyc_YYSTYPE*_TF80;union Cyc_YYSTYPE*_TF81;struct Cyc_Absyn_Exp*(*_TF82)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TF83;union Cyc_YYSTYPE*_TF84;union Cyc_YYSTYPE*_TF85;struct Cyc_Absyn_Pat*_TF86;struct Cyc_Absyn_Exp*_TF87;struct Cyc_Yystacktype*_TF88;union Cyc_YYSTYPE*_TF89;union Cyc_YYSTYPE*_TF8A;struct Cyc_Absyn_Exp*_TF8B;struct Cyc_Yystacktype*_TF8C;struct Cyc_Yystacktype _TF8D;struct Cyc_Yyltype _TF8E;unsigned _TF8F;unsigned _TF90;struct Cyc_Absyn_Exp*_TF91;struct Cyc_Absyn_Pat*_TF92;struct Cyc_Yystacktype*_TF93;struct Cyc_Yystacktype _TF94;struct Cyc_Yystacktype*_TF95;union Cyc_YYSTYPE*_TF96;union Cyc_YYSTYPE*_TF97;struct Cyc_Absyn_Exp*(*_TF98)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_TF99;union Cyc_YYSTYPE*_TF9A;union Cyc_YYSTYPE*_TF9B;struct Cyc_Absyn_Pat*_TF9C;struct Cyc_Absyn_Exp*_TF9D;struct Cyc_Yystacktype*_TF9E;union Cyc_YYSTYPE*_TF9F;union Cyc_YYSTYPE*_TFA0;struct Cyc_Absyn_Exp*_TFA1;struct Cyc_Yystacktype*_TFA2;struct Cyc_Yystacktype _TFA3;struct Cyc_Yyltype _TFA4;unsigned _TFA5;unsigned _TFA6;struct Cyc_Absyn_Exp*_TFA7;struct Cyc_Absyn_Pat*_TFA8;struct Cyc_Yystacktype*_TFA9;struct Cyc_Yystacktype _TFAA;struct Cyc_Yystacktype*_TFAB;union Cyc_YYSTYPE*_TFAC;union Cyc_YYSTYPE*_TFAD;struct Cyc_Absyn_Pat*_TFAE;struct Cyc_Absyn_Exp*_TFAF;struct Cyc_Yystacktype*_TFB0;union Cyc_YYSTYPE*_TFB1;union Cyc_YYSTYPE*_TFB2;struct Cyc_Absyn_Exp*_TFB3;struct Cyc_Yystacktype*_TFB4;struct Cyc_Yystacktype _TFB5;struct Cyc_Yyltype _TFB6;unsigned _TFB7;unsigned _TFB8;struct Cyc_Absyn_Exp*_TFB9;struct Cyc_Absyn_Pat*_TFBA;struct Cyc_Yystacktype*_TFBB;union Cyc_YYSTYPE*_TFBC;union Cyc_YYSTYPE*_TFBD;struct Cyc_Absyn_Pat*_TFBE;struct Cyc_Absyn_Exp*_TFBF;struct Cyc_Yystacktype*_TFC0;union Cyc_YYSTYPE*_TFC1;union Cyc_YYSTYPE*_TFC2;struct Cyc_Absyn_Exp*_TFC3;struct Cyc_Yystacktype*_TFC4;struct Cyc_Yystacktype _TFC5;struct Cyc_Yyltype _TFC6;unsigned _TFC7;unsigned _TFC8;struct Cyc_Absyn_Exp*_TFC9;struct Cyc_Absyn_Pat*_TFCA;struct Cyc_Yystacktype*_TFCB;struct Cyc_Yystacktype _TFCC;struct Cyc_Yystacktype*_TFCD;union Cyc_YYSTYPE*_TFCE;union Cyc_YYSTYPE*_TFCF;enum Cyc_Absyn_Primop _TFD0;struct Cyc_Yystacktype*_TFD1;union Cyc_YYSTYPE*_TFD2;union Cyc_YYSTYPE*_TFD3;struct Cyc_Absyn_Pat*_TFD4;struct Cyc_Absyn_Exp*_TFD5;struct Cyc_Yystacktype*_TFD6;union Cyc_YYSTYPE*_TFD7;union Cyc_YYSTYPE*_TFD8;struct Cyc_Absyn_Exp*_TFD9;struct Cyc_Yystacktype*_TFDA;struct Cyc_Yystacktype _TFDB;struct Cyc_Yyltype _TFDC;unsigned _TFDD;unsigned _TFDE;struct Cyc_Absyn_Exp*_TFDF;struct Cyc_Absyn_Pat*_TFE0;struct Cyc_Yystacktype*_TFE1;struct Cyc_Yystacktype _TFE2;struct Cyc_Yystacktype*_TFE3;union Cyc_YYSTYPE*_TFE4;union Cyc_YYSTYPE*_TFE5;enum Cyc_Absyn_Primop _TFE6;struct Cyc_Yystacktype*_TFE7;union Cyc_YYSTYPE*_TFE8;union Cyc_YYSTYPE*_TFE9;struct Cyc_Absyn_Pat*_TFEA;struct Cyc_Absyn_Exp*_TFEB;struct Cyc_Yystacktype*_TFEC;union Cyc_YYSTYPE*_TFED;union Cyc_YYSTYPE*_TFEE;struct Cyc_Absyn_Exp*_TFEF;struct Cyc_Yystacktype*_TFF0;struct Cyc_Yystacktype _TFF1;struct Cyc_Yyltype _TFF2;unsigned _TFF3;unsigned _TFF4;struct Cyc_Absyn_Exp*_TFF5;struct Cyc_Absyn_Pat*_TFF6;struct Cyc_Yystacktype*_TFF7;struct Cyc_Yystacktype _TFF8;struct Cyc_Yystacktype*_TFF9;union Cyc_YYSTYPE*_TFFA;union Cyc_YYSTYPE*_TFFB;struct _tuple8*_TFFC;struct Cyc_Yystacktype*_TFFD;struct Cyc_Yystacktype _TFFE;struct Cyc_Yyltype _TFFF;unsigned _T1000;unsigned _T1001;void*_T1002;struct Cyc_Yystacktype*_T1003;union Cyc_YYSTYPE*_T1004;union Cyc_YYSTYPE*_T1005;struct Cyc_Absyn_Exp*_T1006;struct Cyc_Yystacktype*_T1007;struct Cyc_Yystacktype _T1008;struct Cyc_Yyltype _T1009;unsigned _T100A;unsigned _T100B;struct Cyc_Absyn_Exp*_T100C;struct Cyc_Absyn_Pat*_T100D;struct Cyc_Yystacktype*_T100E;struct Cyc_Yystacktype _T100F;struct Cyc_Yystacktype*_T1010;union Cyc_YYSTYPE*_T1011;union Cyc_YYSTYPE*_T1012;enum Cyc_Absyn_Primop _T1013;struct Cyc_Yystacktype*_T1014;union Cyc_YYSTYPE*_T1015;union Cyc_YYSTYPE*_T1016;struct Cyc_Absyn_Exp*_T1017;struct Cyc_Yystacktype*_T1018;struct Cyc_Yystacktype _T1019;struct Cyc_Yyltype _T101A;unsigned _T101B;unsigned _T101C;struct Cyc_Absyn_Exp*_T101D;struct Cyc_Absyn_Pat*_T101E;struct Cyc_Yystacktype*_T101F;union Cyc_YYSTYPE*_T1020;union Cyc_YYSTYPE*_T1021;struct _tuple8*_T1022;struct Cyc_Yystacktype*_T1023;struct Cyc_Yystacktype _T1024;struct Cyc_Yyltype _T1025;unsigned _T1026;unsigned _T1027;void*_T1028;struct Cyc_Yystacktype*_T1029;struct Cyc_Yystacktype _T102A;struct Cyc_Yyltype _T102B;unsigned _T102C;unsigned _T102D;struct Cyc_Absyn_Exp*_T102E;struct Cyc_Absyn_Pat*_T102F;struct Cyc_Yystacktype*_T1030;union Cyc_YYSTYPE*_T1031;union Cyc_YYSTYPE*_T1032;struct Cyc_Absyn_Exp*_T1033;struct Cyc_Yystacktype*_T1034;struct Cyc_Yystacktype _T1035;struct Cyc_Yyltype _T1036;unsigned _T1037;unsigned _T1038;struct Cyc_Absyn_Exp*_T1039;struct Cyc_Absyn_Pat*_T103A;struct Cyc_Yystacktype*_T103B;union Cyc_YYSTYPE*_T103C;union Cyc_YYSTYPE*_T103D;struct _tuple8*_T103E;struct _tuple8 _T103F;void*_T1040;struct Cyc_Yystacktype*_T1041;union Cyc_YYSTYPE*_T1042;union Cyc_YYSTYPE*_T1043;struct Cyc_List_List*_T1044;struct Cyc_List_List*_T1045;struct Cyc_Yystacktype*_T1046;struct Cyc_Yystacktype _T1047;struct Cyc_Yyltype _T1048;unsigned _T1049;unsigned _T104A;struct Cyc_Absyn_Exp*_T104B;struct Cyc_Absyn_Pat*_T104C;struct Cyc_Yystacktype*_T104D;struct Cyc_Yystacktype _T104E;struct Cyc_Yystacktype*_T104F;struct Cyc_Yystacktype _T1050;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1051;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T1052;void*_T1053;struct Cyc_Yystacktype*_T1054;struct Cyc_Yystacktype _T1055;struct Cyc_Yyltype _T1056;unsigned _T1057;unsigned _T1058;struct Cyc_Absyn_Pat*_T1059;struct Cyc_Yystacktype*_T105A;union Cyc_YYSTYPE*_T105B;union Cyc_YYSTYPE*_T105C;struct Cyc_Absyn_Exp*_T105D;struct Cyc_Absyn_Pat*_T105E;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T105F;struct Cyc_Yystacktype*_T1060;union Cyc_YYSTYPE*_T1061;union Cyc_YYSTYPE*_T1062;void*_T1063;struct Cyc_Yystacktype*_T1064;struct Cyc_Yystacktype _T1065;struct Cyc_Yyltype _T1066;unsigned _T1067;unsigned _T1068;struct Cyc_Absyn_Pat*_T1069;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T106A;struct Cyc_Yystacktype*_T106B;union Cyc_YYSTYPE*_T106C;union Cyc_YYSTYPE*_T106D;void*_T106E;struct Cyc_Yystacktype*_T106F;struct Cyc_Yystacktype _T1070;struct Cyc_Yyltype _T1071;unsigned _T1072;unsigned _T1073;struct Cyc_Absyn_Pat*_T1074;struct Cyc_Yystacktype*_T1075;union Cyc_YYSTYPE*_T1076;union Cyc_YYSTYPE*_T1077;struct Cyc_Absyn_Exp*_T1078;int*_T1079;int _T107A;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T107B;union Cyc_Absyn_Cnst _T107C;struct _union_Cnst_LongLong_c _T107D;unsigned _T107E;union Cyc_Absyn_Cnst _T107F;struct _union_Cnst_Char_c _T1080;struct _tuple3 _T1081;union Cyc_Absyn_Cnst _T1082;struct _union_Cnst_Char_c _T1083;struct _tuple3 _T1084;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T1085;void*_T1086;struct Cyc_Absyn_Exp*_T1087;unsigned _T1088;struct Cyc_Absyn_Pat*_T1089;union Cyc_Absyn_Cnst _T108A;struct _union_Cnst_Short_c _T108B;struct _tuple4 _T108C;union Cyc_Absyn_Cnst _T108D;struct _union_Cnst_Short_c _T108E;struct _tuple4 _T108F;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T1090;short _T1091;void*_T1092;struct Cyc_Absyn_Exp*_T1093;unsigned _T1094;struct Cyc_Absyn_Pat*_T1095;union Cyc_Absyn_Cnst _T1096;struct _union_Cnst_Int_c _T1097;struct _tuple5 _T1098;union Cyc_Absyn_Cnst _T1099;struct _union_Cnst_Int_c _T109A;struct _tuple5 _T109B;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T109C;void*_T109D;struct Cyc_Absyn_Exp*_T109E;unsigned _T109F;struct Cyc_Absyn_Pat*_T10A0;union Cyc_Absyn_Cnst _T10A1;struct _union_Cnst_Float_c _T10A2;struct _tuple7 _T10A3;union Cyc_Absyn_Cnst _T10A4;struct _union_Cnst_Float_c _T10A5;struct _tuple7 _T10A6;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T10A7;void*_T10A8;struct Cyc_Absyn_Exp*_T10A9;unsigned _T10AA;struct Cyc_Absyn_Pat*_T10AB;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10AC;struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_T10AD;void*_T10AE;struct Cyc_Absyn_Exp*_T10AF;unsigned _T10B0;struct Cyc_Absyn_Pat*_T10B1;struct Cyc_Yystacktype*_T10B2;struct Cyc_Yystacktype _T10B3;struct Cyc_Yyltype _T10B4;unsigned _T10B5;unsigned _T10B6;struct _fat_ptr _T10B7;struct _fat_ptr _T10B8;struct Cyc_Yystacktype*_T10B9;struct Cyc_Yystacktype _T10BA;struct Cyc_Yyltype _T10BB;unsigned _T10BC;unsigned _T10BD;struct _fat_ptr _T10BE;struct _fat_ptr _T10BF;struct Cyc_Yystacktype*_T10C0;struct Cyc_Yystacktype _T10C1;struct Cyc_Yyltype _T10C2;unsigned _T10C3;unsigned _T10C4;struct _fat_ptr _T10C5;struct _fat_ptr _T10C6;struct Cyc_Yystacktype*_T10C7;union Cyc_YYSTYPE*_T10C8;union Cyc_YYSTYPE*_T10C9;struct _fat_ptr _T10CA;struct _fat_ptr _T10CB;int _T10CC;struct Cyc_Yystacktype*_T10CD;struct Cyc_Yystacktype _T10CE;struct Cyc_Yyltype _T10CF;unsigned _T10D0;unsigned _T10D1;struct _fat_ptr _T10D2;struct _fat_ptr _T10D3;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T10D4;struct Cyc_Yystacktype*_T10D5;struct Cyc_Yystacktype _T10D6;struct Cyc_Yyltype _T10D7;unsigned _T10D8;unsigned _T10D9;struct _tuple0*_T10DA;struct _fat_ptr*_T10DB;struct Cyc_Yystacktype*_T10DC;union Cyc_YYSTYPE*_T10DD;union Cyc_YYSTYPE*_T10DE;void*_T10DF;struct Cyc_Yystacktype*_T10E0;union Cyc_YYSTYPE*_T10E1;union Cyc_YYSTYPE*_T10E2;void*_T10E3;struct Cyc_Yystacktype*_T10E4;struct Cyc_Yystacktype _T10E5;struct Cyc_Yyltype _T10E6;unsigned _T10E7;unsigned _T10E8;struct Cyc_Absyn_Pat*_T10E9;struct Cyc_Yystacktype*_T10EA;union Cyc_YYSTYPE*_T10EB;union Cyc_YYSTYPE*_T10EC;struct _fat_ptr _T10ED;struct _fat_ptr _T10EE;int _T10EF;struct Cyc_Yystacktype*_T10F0;struct Cyc_Yystacktype _T10F1;struct Cyc_Yyltype _T10F2;unsigned _T10F3;unsigned _T10F4;struct _fat_ptr _T10F5;struct _fat_ptr _T10F6;struct Cyc_Yystacktype*_T10F7;struct Cyc_Yystacktype _T10F8;struct Cyc_Yyltype _T10F9;unsigned _T10FA;struct Cyc_Yystacktype*_T10FB;union Cyc_YYSTYPE*_T10FC;union Cyc_YYSTYPE*_T10FD;struct _fat_ptr _T10FE;unsigned _T10FF;struct Cyc_Absyn_Tvar*_T1100;struct _fat_ptr*_T1101;struct Cyc_Yystacktype*_T1102;union Cyc_YYSTYPE*_T1103;union Cyc_YYSTYPE*_T1104;struct Cyc_Absyn_Tvar*_T1105;struct Cyc_Absyn_Tvar*_T1106;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1107;struct Cyc_Absyn_Kind*_T1108;struct Cyc_Absyn_Tvar*_T1109;struct Cyc_Yystacktype*_T110A;struct Cyc_Yystacktype _T110B;struct Cyc_Yyltype _T110C;unsigned _T110D;unsigned _T110E;struct _tuple0*_T110F;struct _fat_ptr*_T1110;struct Cyc_Yystacktype*_T1111;union Cyc_YYSTYPE*_T1112;union Cyc_YYSTYPE*_T1113;struct Cyc_Yystacktype*_T1114;union Cyc_YYSTYPE*_T1115;union Cyc_YYSTYPE*_T1116;struct _tuple8*_T1117;struct Cyc_Yystacktype*_T1118;struct Cyc_Yystacktype _T1119;struct Cyc_Yyltype _T111A;unsigned _T111B;unsigned _T111C;void*_T111D;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T111E;void*_T111F;unsigned _T1120;struct Cyc_Absyn_Pat*_T1121;struct Cyc_Yystacktype*_T1122;union Cyc_YYSTYPE*_T1123;union Cyc_YYSTYPE*_T1124;struct _fat_ptr _T1125;struct _fat_ptr _T1126;int _T1127;struct Cyc_Yystacktype*_T1128;struct Cyc_Yystacktype _T1129;struct Cyc_Yyltype _T112A;unsigned _T112B;unsigned _T112C;struct _fat_ptr _T112D;struct _fat_ptr _T112E;struct Cyc_Yystacktype*_T112F;struct Cyc_Yystacktype _T1130;struct Cyc_Yyltype _T1131;unsigned _T1132;struct Cyc_Yystacktype*_T1133;union Cyc_YYSTYPE*_T1134;union Cyc_YYSTYPE*_T1135;struct _fat_ptr _T1136;unsigned _T1137;struct Cyc_Absyn_Tvar*_T1138;struct _fat_ptr*_T1139;struct Cyc_Yystacktype*_T113A;union Cyc_YYSTYPE*_T113B;union Cyc_YYSTYPE*_T113C;struct Cyc_Absyn_Tvar*_T113D;struct Cyc_Absyn_Tvar*_T113E;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T113F;struct Cyc_Absyn_Kind*_T1140;struct Cyc_Absyn_Tvar*_T1141;struct Cyc_Yystacktype*_T1142;struct Cyc_Yystacktype _T1143;struct Cyc_Yyltype _T1144;unsigned _T1145;unsigned _T1146;struct _tuple0*_T1147;struct _fat_ptr*_T1148;struct Cyc_Yystacktype*_T1149;union Cyc_YYSTYPE*_T114A;union Cyc_YYSTYPE*_T114B;struct Cyc_Yystacktype*_T114C;union Cyc_YYSTYPE*_T114D;union Cyc_YYSTYPE*_T114E;struct _tuple8*_T114F;struct Cyc_Yystacktype*_T1150;struct Cyc_Yystacktype _T1151;struct Cyc_Yyltype _T1152;unsigned _T1153;unsigned _T1154;void*_T1155;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1156;void*_T1157;unsigned _T1158;struct Cyc_Absyn_Pat*_T1159;struct Cyc_Yystacktype*_T115A;union Cyc_YYSTYPE*_T115B;union Cyc_YYSTYPE*_T115C;struct _tuple23*_T115D;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T115E;void*_T115F;struct Cyc_Yystacktype*_T1160;struct Cyc_Yystacktype _T1161;struct Cyc_Yyltype _T1162;unsigned _T1163;unsigned _T1164;struct Cyc_Absyn_Pat*_T1165;struct Cyc_Yystacktype*_T1166;union Cyc_YYSTYPE*_T1167;union Cyc_YYSTYPE*_T1168;struct _tuple23*_T1169;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T116A;struct Cyc_Yystacktype*_T116B;union Cyc_YYSTYPE*_T116C;union Cyc_YYSTYPE*_T116D;void*_T116E;struct Cyc_Yystacktype*_T116F;struct Cyc_Yystacktype _T1170;struct Cyc_Yyltype _T1171;unsigned _T1172;unsigned _T1173;struct Cyc_Absyn_Pat*_T1174;struct Cyc_Yystacktype*_T1175;union Cyc_YYSTYPE*_T1176;union Cyc_YYSTYPE*_T1177;struct _tuple23*_T1178;struct Cyc_List_List*(*_T1179)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T117A)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T117B;struct Cyc_Yystacktype _T117C;struct Cyc_Yyltype _T117D;unsigned _T117E;unsigned _T117F;struct Cyc_Yystacktype*_T1180;union Cyc_YYSTYPE*_T1181;union Cyc_YYSTYPE*_T1182;struct Cyc_List_List*_T1183;struct Cyc_Yystacktype*_T1184;union Cyc_YYSTYPE*_T1185;union Cyc_YYSTYPE*_T1186;struct _tuple0*_T1187;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1188;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T1189;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T118A;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T118B;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T118C;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T118D;void*_T118E;struct Cyc_Yystacktype*_T118F;struct Cyc_Yystacktype _T1190;struct Cyc_Yyltype _T1191;unsigned _T1192;unsigned _T1193;struct Cyc_Absyn_Pat*_T1194;struct Cyc_Yystacktype*_T1195;union Cyc_YYSTYPE*_T1196;union Cyc_YYSTYPE*_T1197;struct _tuple23*_T1198;struct Cyc_List_List*(*_T1199)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*);struct Cyc_List_List*(*_T119A)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_Yystacktype*_T119B;struct Cyc_Yystacktype _T119C;struct Cyc_Yyltype _T119D;unsigned _T119E;unsigned _T119F;struct Cyc_Yystacktype*_T11A0;union Cyc_YYSTYPE*_T11A1;union Cyc_YYSTYPE*_T11A2;struct Cyc_List_List*_T11A3;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T11A4;void*_T11A5;struct Cyc_Yystacktype*_T11A6;struct Cyc_Yystacktype _T11A7;struct Cyc_Yyltype _T11A8;unsigned _T11A9;unsigned _T11AA;struct Cyc_Absyn_Pat*_T11AB;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11AC;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T11AD;struct Cyc_Yystacktype*_T11AE;union Cyc_YYSTYPE*_T11AF;union Cyc_YYSTYPE*_T11B0;void*_T11B1;struct Cyc_Yystacktype*_T11B2;struct Cyc_Yystacktype _T11B3;struct Cyc_Yyltype _T11B4;unsigned _T11B5;unsigned _T11B6;void*_T11B7;struct Cyc_Yystacktype*_T11B8;struct Cyc_Yystacktype _T11B9;struct Cyc_Yyltype _T11BA;unsigned _T11BB;unsigned _T11BC;struct Cyc_Absyn_Pat*_T11BD;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11BE;struct Cyc_Yystacktype*_T11BF;struct Cyc_Yystacktype _T11C0;struct Cyc_Yyltype _T11C1;unsigned _T11C2;unsigned _T11C3;struct _tuple0*_T11C4;struct _fat_ptr*_T11C5;struct Cyc_Yystacktype*_T11C6;union Cyc_YYSTYPE*_T11C7;union Cyc_YYSTYPE*_T11C8;void*_T11C9;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11CA;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T11CB;void*_T11CC;struct Cyc_Yystacktype*_T11CD;struct Cyc_Yystacktype _T11CE;struct Cyc_Yyltype _T11CF;unsigned _T11D0;unsigned _T11D1;void*_T11D2;struct Cyc_Yystacktype*_T11D3;struct Cyc_Yystacktype _T11D4;struct Cyc_Yyltype _T11D5;unsigned _T11D6;unsigned _T11D7;struct Cyc_Absyn_Pat*_T11D8;struct Cyc_Yystacktype*_T11D9;union Cyc_YYSTYPE*_T11DA;union Cyc_YYSTYPE*_T11DB;struct _fat_ptr _T11DC;struct _fat_ptr _T11DD;int _T11DE;struct Cyc_Yystacktype*_T11DF;struct Cyc_Yystacktype _T11E0;struct Cyc_Yyltype _T11E1;unsigned _T11E2;unsigned _T11E3;struct _fat_ptr _T11E4;struct _fat_ptr _T11E5;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T11E6;struct Cyc_Yystacktype*_T11E7;struct Cyc_Yystacktype _T11E8;struct Cyc_Yyltype _T11E9;unsigned _T11EA;unsigned _T11EB;struct _tuple0*_T11EC;struct _fat_ptr*_T11ED;struct Cyc_Yystacktype*_T11EE;union Cyc_YYSTYPE*_T11EF;union Cyc_YYSTYPE*_T11F0;void*_T11F1;struct Cyc_Yystacktype*_T11F2;union Cyc_YYSTYPE*_T11F3;union Cyc_YYSTYPE*_T11F4;void*_T11F5;struct Cyc_Yystacktype*_T11F6;struct Cyc_Yystacktype _T11F7;struct Cyc_Yyltype _T11F8;unsigned _T11F9;unsigned _T11FA;struct Cyc_Absyn_Pat*_T11FB;struct Cyc_Yystacktype*_T11FC;union Cyc_YYSTYPE*_T11FD;union Cyc_YYSTYPE*_T11FE;struct _fat_ptr _T11FF;struct Cyc_Absyn_Kind*_T1200;struct Cyc_Absyn_Kind*_T1201;void*_T1202;struct Cyc_Yystacktype*_T1203;struct Cyc_Yystacktype _T1204;struct Cyc_Yyltype _T1205;unsigned _T1206;unsigned _T1207;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1208;struct Cyc_Yystacktype*_T1209;struct Cyc_Yystacktype _T120A;struct Cyc_Yyltype _T120B;unsigned _T120C;unsigned _T120D;void*_T120E;struct Cyc_Yystacktype*_T120F;struct Cyc_Yystacktype _T1210;struct Cyc_Yyltype _T1211;unsigned _T1212;unsigned _T1213;struct _tuple0*_T1214;struct _fat_ptr*_T1215;struct Cyc_Yystacktype*_T1216;union Cyc_YYSTYPE*_T1217;union Cyc_YYSTYPE*_T1218;void*_T1219;void*_T121A;struct Cyc_Yystacktype*_T121B;struct Cyc_Yystacktype _T121C;struct Cyc_Yyltype _T121D;unsigned _T121E;unsigned _T121F;struct Cyc_Absyn_Pat*_T1220;struct Cyc_Absyn_Kind*_T1221;struct Cyc_Absyn_Kind*_T1222;void*_T1223;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1224;struct Cyc_Yystacktype*_T1225;struct Cyc_Yystacktype _T1226;struct Cyc_Yyltype _T1227;unsigned _T1228;unsigned _T1229;struct _tuple0*_T122A;struct _fat_ptr*_T122B;struct Cyc_Yystacktype*_T122C;union Cyc_YYSTYPE*_T122D;union Cyc_YYSTYPE*_T122E;void*_T122F;void*_T1230;void*_T1231;struct Cyc_Yystacktype*_T1232;struct Cyc_Yystacktype _T1233;struct Cyc_Yyltype _T1234;unsigned _T1235;unsigned _T1236;struct Cyc_Absyn_Pat*_T1237;struct _tuple23*_T1238;struct Cyc_Yystacktype*_T1239;union Cyc_YYSTYPE*_T123A;union Cyc_YYSTYPE*_T123B;struct Cyc_List_List*_T123C;struct _tuple23*_T123D;struct Cyc_Yystacktype*_T123E;union Cyc_YYSTYPE*_T123F;union Cyc_YYSTYPE*_T1240;struct Cyc_List_List*_T1241;struct _tuple23*_T1242;struct Cyc_List_List*_T1243;struct Cyc_Yystacktype*_T1244;union Cyc_YYSTYPE*_T1245;union Cyc_YYSTYPE*_T1246;struct Cyc_List_List*_T1247;struct Cyc_Yystacktype*_T1248;union Cyc_YYSTYPE*_T1249;union Cyc_YYSTYPE*_T124A;struct Cyc_Yystacktype*_T124B;union Cyc_YYSTYPE*_T124C;union Cyc_YYSTYPE*_T124D;struct _tuple24*_T124E;struct Cyc_Yystacktype*_T124F;union Cyc_YYSTYPE*_T1250;union Cyc_YYSTYPE*_T1251;struct _tuple24*_T1252;struct Cyc_Yystacktype*_T1253;union Cyc_YYSTYPE*_T1254;union Cyc_YYSTYPE*_T1255;struct Cyc_Yystacktype*_T1256;union Cyc_YYSTYPE*_T1257;union Cyc_YYSTYPE*_T1258;struct _tuple23*_T1259;struct Cyc_Yystacktype*_T125A;union Cyc_YYSTYPE*_T125B;union Cyc_YYSTYPE*_T125C;struct Cyc_List_List*_T125D;struct _tuple23*_T125E;struct Cyc_Yystacktype*_T125F;union Cyc_YYSTYPE*_T1260;union Cyc_YYSTYPE*_T1261;struct Cyc_List_List*_T1262;struct _tuple23*_T1263;struct Cyc_List_List*_T1264;struct Cyc_Yystacktype*_T1265;union Cyc_YYSTYPE*_T1266;union Cyc_YYSTYPE*_T1267;struct Cyc_List_List*_T1268;struct Cyc_Yystacktype*_T1269;union Cyc_YYSTYPE*_T126A;union Cyc_YYSTYPE*_T126B;struct Cyc_Yystacktype*_T126C;union Cyc_YYSTYPE*_T126D;union Cyc_YYSTYPE*_T126E;struct Cyc_Yystacktype*_T126F;struct Cyc_Yystacktype _T1270;struct Cyc_Yystacktype*_T1271;union Cyc_YYSTYPE*_T1272;union Cyc_YYSTYPE*_T1273;struct Cyc_Absyn_Exp*_T1274;struct Cyc_Yystacktype*_T1275;union Cyc_YYSTYPE*_T1276;union Cyc_YYSTYPE*_T1277;struct Cyc_Absyn_Exp*_T1278;struct Cyc_Yystacktype*_T1279;struct Cyc_Yystacktype _T127A;struct Cyc_Yyltype _T127B;unsigned _T127C;unsigned _T127D;struct Cyc_Absyn_Exp*_T127E;struct Cyc_Yystacktype*_T127F;struct Cyc_Yystacktype _T1280;struct Cyc_Yystacktype*_T1281;union Cyc_YYSTYPE*_T1282;union Cyc_YYSTYPE*_T1283;struct Cyc_Absyn_Exp*_T1284;struct Cyc_Yystacktype*_T1285;union Cyc_YYSTYPE*_T1286;union Cyc_YYSTYPE*_T1287;struct Cyc_Core_Opt*_T1288;struct Cyc_Yystacktype*_T1289;union Cyc_YYSTYPE*_T128A;union Cyc_YYSTYPE*_T128B;struct Cyc_Absyn_Exp*_T128C;struct Cyc_Yystacktype*_T128D;struct Cyc_Yystacktype _T128E;struct Cyc_Yyltype _T128F;unsigned _T1290;unsigned _T1291;struct Cyc_Absyn_Exp*_T1292;struct Cyc_Yystacktype*_T1293;union Cyc_YYSTYPE*_T1294;union Cyc_YYSTYPE*_T1295;struct Cyc_Absyn_Exp*_T1296;struct Cyc_Yystacktype*_T1297;union Cyc_YYSTYPE*_T1298;union Cyc_YYSTYPE*_T1299;struct Cyc_Absyn_Exp*_T129A;struct Cyc_Yystacktype*_T129B;struct Cyc_Yystacktype _T129C;struct Cyc_Yyltype _T129D;unsigned _T129E;unsigned _T129F;struct Cyc_Absyn_Exp*_T12A0;struct Cyc_Core_Opt*_T12A1;struct Cyc_Core_Opt*_T12A2;struct Cyc_Core_Opt*_T12A3;struct Cyc_Core_Opt*_T12A4;struct Cyc_Core_Opt*_T12A5;struct Cyc_Core_Opt*_T12A6;struct Cyc_Core_Opt*_T12A7;struct Cyc_Core_Opt*_T12A8;struct Cyc_Core_Opt*_T12A9;struct Cyc_Core_Opt*_T12AA;struct Cyc_Yystacktype*_T12AB;struct Cyc_Yystacktype _T12AC;struct Cyc_Yystacktype*_T12AD;union Cyc_YYSTYPE*_T12AE;union Cyc_YYSTYPE*_T12AF;struct Cyc_Absyn_Exp*_T12B0;struct Cyc_Yystacktype*_T12B1;union Cyc_YYSTYPE*_T12B2;union Cyc_YYSTYPE*_T12B3;struct Cyc_Absyn_Exp*_T12B4;struct Cyc_Yystacktype*_T12B5;union Cyc_YYSTYPE*_T12B6;union Cyc_YYSTYPE*_T12B7;struct Cyc_Absyn_Exp*_T12B8;struct Cyc_Yystacktype*_T12B9;struct Cyc_Yystacktype _T12BA;struct Cyc_Yyltype _T12BB;unsigned _T12BC;unsigned _T12BD;struct Cyc_Absyn_Exp*_T12BE;struct Cyc_Yystacktype*_T12BF;union Cyc_YYSTYPE*_T12C0;union Cyc_YYSTYPE*_T12C1;struct Cyc_Absyn_Exp*_T12C2;struct Cyc_Yystacktype*_T12C3;struct Cyc_Yystacktype _T12C4;struct Cyc_Yyltype _T12C5;unsigned _T12C6;unsigned _T12C7;struct Cyc_Absyn_Exp*_T12C8;struct Cyc_Yystacktype*_T12C9;union Cyc_YYSTYPE*_T12CA;union Cyc_YYSTYPE*_T12CB;struct Cyc_Absyn_Exp*_T12CC;struct Cyc_Yystacktype*_T12CD;struct Cyc_Yystacktype _T12CE;struct Cyc_Yyltype _T12CF;unsigned _T12D0;unsigned _T12D1;struct Cyc_Absyn_Exp*_T12D2;struct Cyc_Yystacktype*_T12D3;union Cyc_YYSTYPE*_T12D4;union Cyc_YYSTYPE*_T12D5;struct Cyc_Absyn_Exp*_T12D6;struct Cyc_Yystacktype*_T12D7;struct Cyc_Yystacktype _T12D8;struct Cyc_Yyltype _T12D9;unsigned _T12DA;unsigned _T12DB;struct Cyc_Absyn_Exp*_T12DC;struct Cyc_Yystacktype*_T12DD;union Cyc_YYSTYPE*_T12DE;union Cyc_YYSTYPE*_T12DF;struct Cyc_Absyn_Exp*_T12E0;struct Cyc_Yystacktype*_T12E1;union Cyc_YYSTYPE*_T12E2;union Cyc_YYSTYPE*_T12E3;struct Cyc_Absyn_Exp*_T12E4;struct Cyc_Yystacktype*_T12E5;struct Cyc_Yystacktype _T12E6;struct Cyc_Yyltype _T12E7;unsigned _T12E8;unsigned _T12E9;struct Cyc_Absyn_Exp*_T12EA;struct Cyc_Yystacktype*_T12EB;union Cyc_YYSTYPE*_T12EC;union Cyc_YYSTYPE*_T12ED;struct Cyc_Absyn_Exp*_T12EE;struct Cyc_Yystacktype*_T12EF;union Cyc_YYSTYPE*_T12F0;union Cyc_YYSTYPE*_T12F1;struct Cyc_Absyn_Exp*_T12F2;struct Cyc_Yystacktype*_T12F3;struct Cyc_Yystacktype _T12F4;struct Cyc_Yyltype _T12F5;unsigned _T12F6;unsigned _T12F7;struct Cyc_Absyn_Exp*_T12F8;struct Cyc_Yystacktype*_T12F9;union Cyc_YYSTYPE*_T12FA;union Cyc_YYSTYPE*_T12FB;struct Cyc_Absyn_Exp*_T12FC;struct Cyc_Absyn_Exp*_T12FD;struct Cyc_Yystacktype*_T12FE;union Cyc_YYSTYPE*_T12FF;union Cyc_YYSTYPE*_T1300;struct Cyc_Absyn_Exp*_T1301;struct Cyc_Absyn_Exp*_T1302;struct Cyc_Yystacktype*_T1303;struct Cyc_Yystacktype _T1304;struct Cyc_Yyltype _T1305;unsigned _T1306;unsigned _T1307;struct Cyc_Absyn_Exp*_T1308;struct Cyc_Yystacktype*_T1309;union Cyc_YYSTYPE*_T130A;union Cyc_YYSTYPE*_T130B;struct Cyc_Absyn_Exp*_T130C;struct Cyc_Absyn_Exp*_T130D;struct Cyc_Yystacktype*_T130E;union Cyc_YYSTYPE*_T130F;union Cyc_YYSTYPE*_T1310;struct Cyc_Absyn_Exp*_T1311;struct Cyc_Absyn_Exp*_T1312;struct Cyc_Yystacktype*_T1313;struct Cyc_Yystacktype _T1314;struct Cyc_Yyltype _T1315;unsigned _T1316;unsigned _T1317;struct Cyc_Absyn_Exp*_T1318;struct Cyc_Yystacktype*_T1319;struct Cyc_Yystacktype _T131A;struct Cyc_Yystacktype*_T131B;struct Cyc_Yystacktype _T131C;struct Cyc_Yystacktype*_T131D;union Cyc_YYSTYPE*_T131E;union Cyc_YYSTYPE*_T131F;struct Cyc_Absyn_Exp*_T1320;struct Cyc_Yystacktype*_T1321;union Cyc_YYSTYPE*_T1322;union Cyc_YYSTYPE*_T1323;struct Cyc_Absyn_Exp*_T1324;struct Cyc_Yystacktype*_T1325;struct Cyc_Yystacktype _T1326;struct Cyc_Yyltype _T1327;unsigned _T1328;unsigned _T1329;struct Cyc_Absyn_Exp*_T132A;struct Cyc_Yystacktype*_T132B;struct Cyc_Yystacktype _T132C;struct Cyc_Yystacktype*_T132D;union Cyc_YYSTYPE*_T132E;union Cyc_YYSTYPE*_T132F;struct Cyc_Absyn_Exp*_T1330;struct Cyc_Yystacktype*_T1331;union Cyc_YYSTYPE*_T1332;union Cyc_YYSTYPE*_T1333;struct Cyc_Absyn_Exp*_T1334;struct Cyc_Yystacktype*_T1335;struct Cyc_Yystacktype _T1336;struct Cyc_Yyltype _T1337;unsigned _T1338;unsigned _T1339;struct Cyc_Absyn_Exp*_T133A;struct Cyc_Yystacktype*_T133B;struct Cyc_Yystacktype _T133C;struct Cyc_Yystacktype*_T133D;union Cyc_YYSTYPE*_T133E;union Cyc_YYSTYPE*_T133F;struct Cyc_Absyn_Exp*_T1340;struct Cyc_Yystacktype*_T1341;union Cyc_YYSTYPE*_T1342;union Cyc_YYSTYPE*_T1343;struct Cyc_Absyn_Exp*_T1344;struct Cyc_Yystacktype*_T1345;struct Cyc_Yystacktype _T1346;struct Cyc_Yyltype _T1347;unsigned _T1348;unsigned _T1349;struct Cyc_Absyn_Exp*_T134A;struct Cyc_Yystacktype*_T134B;struct Cyc_Yystacktype _T134C;struct Cyc_Yystacktype*_T134D;union Cyc_YYSTYPE*_T134E;union Cyc_YYSTYPE*_T134F;struct Cyc_Absyn_Exp*_T1350;struct Cyc_Yystacktype*_T1351;union Cyc_YYSTYPE*_T1352;union Cyc_YYSTYPE*_T1353;struct Cyc_Absyn_Exp*_T1354;struct Cyc_Yystacktype*_T1355;struct Cyc_Yystacktype _T1356;struct Cyc_Yyltype _T1357;unsigned _T1358;unsigned _T1359;struct Cyc_Absyn_Exp*_T135A;struct Cyc_Yystacktype*_T135B;struct Cyc_Yystacktype _T135C;struct Cyc_Yystacktype*_T135D;union Cyc_YYSTYPE*_T135E;union Cyc_YYSTYPE*_T135F;struct Cyc_Absyn_Exp*_T1360;struct Cyc_Yystacktype*_T1361;union Cyc_YYSTYPE*_T1362;union Cyc_YYSTYPE*_T1363;struct Cyc_Absyn_Exp*_T1364;struct Cyc_Yystacktype*_T1365;struct Cyc_Yystacktype _T1366;struct Cyc_Yyltype _T1367;unsigned _T1368;unsigned _T1369;struct Cyc_Absyn_Exp*_T136A;struct Cyc_Yystacktype*_T136B;struct Cyc_Yystacktype _T136C;struct Cyc_Yystacktype*_T136D;union Cyc_YYSTYPE*_T136E;union Cyc_YYSTYPE*_T136F;struct Cyc_Absyn_Exp*(*_T1370)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T1371;union Cyc_YYSTYPE*_T1372;union Cyc_YYSTYPE*_T1373;struct Cyc_Absyn_Exp*_T1374;struct Cyc_Yystacktype*_T1375;union Cyc_YYSTYPE*_T1376;union Cyc_YYSTYPE*_T1377;struct Cyc_Absyn_Exp*_T1378;struct Cyc_Yystacktype*_T1379;struct Cyc_Yystacktype _T137A;struct Cyc_Yyltype _T137B;unsigned _T137C;unsigned _T137D;struct Cyc_Absyn_Exp*_T137E;struct Cyc_Yystacktype*_T137F;struct Cyc_Yystacktype _T1380;struct Cyc_Yystacktype*_T1381;union Cyc_YYSTYPE*_T1382;union Cyc_YYSTYPE*_T1383;struct Cyc_Absyn_Exp*(*_T1384)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T1385;union Cyc_YYSTYPE*_T1386;union Cyc_YYSTYPE*_T1387;struct Cyc_Absyn_Exp*_T1388;struct Cyc_Yystacktype*_T1389;union Cyc_YYSTYPE*_T138A;union Cyc_YYSTYPE*_T138B;struct Cyc_Absyn_Exp*_T138C;struct Cyc_Yystacktype*_T138D;struct Cyc_Yystacktype _T138E;struct Cyc_Yyltype _T138F;unsigned _T1390;unsigned _T1391;struct Cyc_Absyn_Exp*_T1392;struct Cyc_Yystacktype*_T1393;struct Cyc_Yystacktype _T1394;struct Cyc_Yystacktype*_T1395;union Cyc_YYSTYPE*_T1396;union Cyc_YYSTYPE*_T1397;struct Cyc_Absyn_Exp*_T1398;struct Cyc_Yystacktype*_T1399;union Cyc_YYSTYPE*_T139A;union Cyc_YYSTYPE*_T139B;struct Cyc_Absyn_Exp*_T139C;struct Cyc_Yystacktype*_T139D;struct Cyc_Yystacktype _T139E;struct Cyc_Yyltype _T139F;unsigned _T13A0;unsigned _T13A1;struct Cyc_Absyn_Exp*_T13A2;struct Cyc_Yystacktype*_T13A3;union Cyc_YYSTYPE*_T13A4;union Cyc_YYSTYPE*_T13A5;struct Cyc_Absyn_Exp*_T13A6;struct Cyc_Yystacktype*_T13A7;union Cyc_YYSTYPE*_T13A8;union Cyc_YYSTYPE*_T13A9;struct Cyc_Absyn_Exp*_T13AA;struct Cyc_Yystacktype*_T13AB;struct Cyc_Yystacktype _T13AC;struct Cyc_Yyltype _T13AD;unsigned _T13AE;unsigned _T13AF;struct Cyc_Absyn_Exp*_T13B0;struct Cyc_Yystacktype*_T13B1;struct Cyc_Yystacktype _T13B2;struct Cyc_Yystacktype*_T13B3;union Cyc_YYSTYPE*_T13B4;union Cyc_YYSTYPE*_T13B5;enum Cyc_Absyn_Primop _T13B6;struct Cyc_Yystacktype*_T13B7;union Cyc_YYSTYPE*_T13B8;union Cyc_YYSTYPE*_T13B9;struct Cyc_Absyn_Exp*_T13BA;struct Cyc_Yystacktype*_T13BB;union Cyc_YYSTYPE*_T13BC;union Cyc_YYSTYPE*_T13BD;struct Cyc_Absyn_Exp*_T13BE;struct Cyc_Yystacktype*_T13BF;struct Cyc_Yystacktype _T13C0;struct Cyc_Yyltype _T13C1;unsigned _T13C2;unsigned _T13C3;struct Cyc_Absyn_Exp*_T13C4;struct Cyc_Yystacktype*_T13C5;struct Cyc_Yystacktype _T13C6;struct Cyc_Yystacktype*_T13C7;union Cyc_YYSTYPE*_T13C8;union Cyc_YYSTYPE*_T13C9;enum Cyc_Absyn_Primop _T13CA;struct Cyc_Yystacktype*_T13CB;union Cyc_YYSTYPE*_T13CC;union Cyc_YYSTYPE*_T13CD;struct Cyc_Absyn_Exp*_T13CE;struct Cyc_Yystacktype*_T13CF;union Cyc_YYSTYPE*_T13D0;union Cyc_YYSTYPE*_T13D1;struct Cyc_Absyn_Exp*_T13D2;struct Cyc_Yystacktype*_T13D3;struct Cyc_Yystacktype _T13D4;struct Cyc_Yyltype _T13D5;unsigned _T13D6;unsigned _T13D7;struct Cyc_Absyn_Exp*_T13D8;struct Cyc_Absyn_Exp*(*_T13D9)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DA)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DB)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DC)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DD)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Absyn_Exp*(*_T13DE)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);struct Cyc_Yystacktype*_T13DF;struct Cyc_Yystacktype _T13E0;struct Cyc_Yystacktype*_T13E1;union Cyc_YYSTYPE*_T13E2;union Cyc_YYSTYPE*_T13E3;struct _tuple8*_T13E4;struct Cyc_Yystacktype*_T13E5;struct Cyc_Yystacktype _T13E6;struct Cyc_Yyltype _T13E7;unsigned _T13E8;unsigned _T13E9;void*_T13EA;struct Cyc_Yystacktype*_T13EB;union Cyc_YYSTYPE*_T13EC;union Cyc_YYSTYPE*_T13ED;struct Cyc_Absyn_Exp*_T13EE;struct Cyc_Yystacktype*_T13EF;struct Cyc_Yystacktype _T13F0;struct Cyc_Yyltype _T13F1;unsigned _T13F2;unsigned _T13F3;struct Cyc_Absyn_Exp*_T13F4;struct Cyc_Yystacktype*_T13F5;struct Cyc_Yystacktype _T13F6;struct Cyc_Yystacktype*_T13F7;union Cyc_YYSTYPE*_T13F8;union Cyc_YYSTYPE*_T13F9;struct Cyc_Absyn_Exp*_T13FA;struct Cyc_Yystacktype*_T13FB;struct Cyc_Yystacktype _T13FC;struct Cyc_Yyltype _T13FD;unsigned _T13FE;unsigned _T13FF;struct Cyc_Absyn_Exp*_T1400;struct Cyc_Yystacktype*_T1401;union Cyc_YYSTYPE*_T1402;union Cyc_YYSTYPE*_T1403;struct Cyc_Absyn_Exp*_T1404;struct Cyc_Yystacktype*_T1405;struct Cyc_Yystacktype _T1406;struct Cyc_Yyltype _T1407;unsigned _T1408;unsigned _T1409;struct Cyc_Absyn_Exp*_T140A;struct Cyc_Yystacktype*_T140B;union Cyc_YYSTYPE*_T140C;union Cyc_YYSTYPE*_T140D;struct Cyc_Absyn_Exp*_T140E;struct Cyc_Yystacktype*_T140F;struct Cyc_Yystacktype _T1410;struct Cyc_Yyltype _T1411;unsigned _T1412;unsigned _T1413;struct Cyc_Absyn_Exp*_T1414;struct Cyc_Yystacktype*_T1415;union Cyc_YYSTYPE*_T1416;union Cyc_YYSTYPE*_T1417;struct Cyc_Absyn_Exp*_T1418;struct Cyc_Yystacktype*_T1419;struct Cyc_Yystacktype _T141A;struct Cyc_Yyltype _T141B;unsigned _T141C;unsigned _T141D;struct Cyc_Absyn_Exp*_T141E;struct Cyc_Yystacktype*_T141F;union Cyc_YYSTYPE*_T1420;union Cyc_YYSTYPE*_T1421;enum Cyc_Absyn_Primop _T1422;struct Cyc_Yystacktype*_T1423;union Cyc_YYSTYPE*_T1424;union Cyc_YYSTYPE*_T1425;struct Cyc_Absyn_Exp*_T1426;struct Cyc_Yystacktype*_T1427;struct Cyc_Yystacktype _T1428;struct Cyc_Yyltype _T1429;unsigned _T142A;unsigned _T142B;struct Cyc_Absyn_Exp*_T142C;struct Cyc_Yystacktype*_T142D;union Cyc_YYSTYPE*_T142E;union Cyc_YYSTYPE*_T142F;struct _tuple8*_T1430;struct Cyc_Yystacktype*_T1431;struct Cyc_Yystacktype _T1432;struct Cyc_Yyltype _T1433;unsigned _T1434;unsigned _T1435;void*_T1436;struct Cyc_Yystacktype*_T1437;struct Cyc_Yystacktype _T1438;struct Cyc_Yyltype _T1439;unsigned _T143A;unsigned _T143B;struct Cyc_Absyn_Exp*_T143C;struct Cyc_Yystacktype*_T143D;union Cyc_YYSTYPE*_T143E;union Cyc_YYSTYPE*_T143F;struct Cyc_Absyn_Exp*_T1440;struct Cyc_Yystacktype*_T1441;struct Cyc_Yystacktype _T1442;struct Cyc_Yyltype _T1443;unsigned _T1444;unsigned _T1445;struct Cyc_Absyn_Exp*_T1446;struct Cyc_Yystacktype*_T1447;union Cyc_YYSTYPE*_T1448;union Cyc_YYSTYPE*_T1449;struct _tuple8*_T144A;struct Cyc_Yystacktype*_T144B;struct Cyc_Yystacktype _T144C;struct Cyc_Yyltype _T144D;unsigned _T144E;unsigned _T144F;void*_T1450;struct Cyc_Yystacktype*_T1451;union Cyc_YYSTYPE*_T1452;union Cyc_YYSTYPE*_T1453;struct Cyc_List_List*_T1454;struct Cyc_List_List*_T1455;struct Cyc_Yystacktype*_T1456;struct Cyc_Yystacktype _T1457;struct Cyc_Yyltype _T1458;unsigned _T1459;unsigned _T145A;struct Cyc_Absyn_Exp*_T145B;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T145C;struct Cyc_Yystacktype*_T145D;union Cyc_YYSTYPE*_T145E;union Cyc_YYSTYPE*_T145F;void*_T1460;struct Cyc_Yystacktype*_T1461;struct Cyc_Yystacktype _T1462;struct Cyc_Yyltype _T1463;unsigned _T1464;unsigned _T1465;struct Cyc_Absyn_Exp*_T1466;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1467;struct Cyc_Yystacktype*_T1468;union Cyc_YYSTYPE*_T1469;union Cyc_YYSTYPE*_T146A;struct Cyc_Yystacktype*_T146B;union Cyc_YYSTYPE*_T146C;union Cyc_YYSTYPE*_T146D;struct Cyc_Yystacktype*_T146E;union Cyc_YYSTYPE*_T146F;union Cyc_YYSTYPE*_T1470;void*_T1471;struct Cyc_Yystacktype*_T1472;struct Cyc_Yystacktype _T1473;struct Cyc_Yyltype _T1474;unsigned _T1475;unsigned _T1476;struct Cyc_Absyn_Exp*_T1477;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1478;struct Cyc_Yystacktype*_T1479;union Cyc_YYSTYPE*_T147A;union Cyc_YYSTYPE*_T147B;struct Cyc_Yystacktype*_T147C;union Cyc_YYSTYPE*_T147D;union Cyc_YYSTYPE*_T147E;void*_T147F;struct Cyc_Yystacktype*_T1480;struct Cyc_Yystacktype _T1481;struct Cyc_Yyltype _T1482;unsigned _T1483;unsigned _T1484;struct Cyc_Absyn_Exp*_T1485;struct Cyc_Yystacktype*_T1486;union Cyc_YYSTYPE*_T1487;union Cyc_YYSTYPE*_T1488;struct Cyc_Absyn_Exp*_T1489;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T148A;struct Cyc_Yystacktype*_T148B;union Cyc_YYSTYPE*_T148C;union Cyc_YYSTYPE*_T148D;void*_T148E;struct Cyc_Yystacktype*_T148F;struct Cyc_Yystacktype _T1490;struct Cyc_Yyltype _T1491;unsigned _T1492;unsigned _T1493;struct Cyc_Absyn_Exp*_T1494;struct Cyc_Yystacktype*_T1495;union Cyc_YYSTYPE*_T1496;union Cyc_YYSTYPE*_T1497;struct Cyc_Absyn_Exp*_T1498;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1499;struct Cyc_Yystacktype*_T149A;union Cyc_YYSTYPE*_T149B;union Cyc_YYSTYPE*_T149C;void*_T149D;struct Cyc_Yystacktype*_T149E;struct Cyc_Yystacktype _T149F;struct Cyc_Yyltype _T14A0;unsigned _T14A1;unsigned _T14A2;struct Cyc_Absyn_Exp*_T14A3;struct Cyc_Yystacktype*_T14A4;union Cyc_YYSTYPE*_T14A5;union Cyc_YYSTYPE*_T14A6;struct _tuple8*_T14A7;struct Cyc_Yystacktype*_T14A8;struct Cyc_Yystacktype _T14A9;struct Cyc_Yyltype _T14AA;unsigned _T14AB;unsigned _T14AC;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14AD;void**_T14AE;struct Cyc_Yystacktype*_T14AF;union Cyc_YYSTYPE*_T14B0;union Cyc_YYSTYPE*_T14B1;void*_T14B2;struct Cyc_Yystacktype*_T14B3;struct Cyc_Yystacktype _T14B4;struct Cyc_Yyltype _T14B5;unsigned _T14B6;unsigned _T14B7;struct Cyc_Absyn_Exp*_T14B8;struct Cyc_Yystacktype*_T14B9;union Cyc_YYSTYPE*_T14BA;union Cyc_YYSTYPE*_T14BB;struct _tuple8*_T14BC;struct Cyc_Yystacktype*_T14BD;struct Cyc_Yystacktype _T14BE;struct Cyc_Yyltype _T14BF;unsigned _T14C0;unsigned _T14C1;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14C2;struct Cyc_Yystacktype*_T14C3;union Cyc_YYSTYPE*_T14C4;union Cyc_YYSTYPE*_T14C5;struct Cyc_Yystacktype*_T14C6;union Cyc_YYSTYPE*_T14C7;union Cyc_YYSTYPE*_T14C8;void**_T14C9;struct Cyc_Yystacktype*_T14CA;union Cyc_YYSTYPE*_T14CB;union Cyc_YYSTYPE*_T14CC;void*_T14CD;struct Cyc_Yystacktype*_T14CE;struct Cyc_Yystacktype _T14CF;struct Cyc_Yyltype _T14D0;unsigned _T14D1;unsigned _T14D2;struct Cyc_Absyn_Exp*_T14D3;struct Cyc_Yystacktype*_T14D4;union Cyc_YYSTYPE*_T14D5;union Cyc_YYSTYPE*_T14D6;struct _tuple8*_T14D7;struct Cyc_Yystacktype*_T14D8;struct Cyc_Yystacktype _T14D9;struct Cyc_Yyltype _T14DA;unsigned _T14DB;unsigned _T14DC;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T14DD;struct Cyc_Yystacktype*_T14DE;union Cyc_YYSTYPE*_T14DF;union Cyc_YYSTYPE*_T14E0;void**_T14E1;struct Cyc_Yystacktype*_T14E2;union Cyc_YYSTYPE*_T14E3;union Cyc_YYSTYPE*_T14E4;void*_T14E5;struct Cyc_Yystacktype*_T14E6;struct Cyc_Yystacktype _T14E7;struct Cyc_Yyltype _T14E8;unsigned _T14E9;unsigned _T14EA;struct Cyc_Absyn_Exp*_T14EB;struct Cyc_List_List*_T14EC;struct Cyc_Yystacktype*_T14ED;union Cyc_YYSTYPE*_T14EE;union Cyc_YYSTYPE*_T14EF;struct _fat_ptr _T14F0;struct Cyc_Yystacktype*_T14F1;struct Cyc_Yystacktype _T14F2;struct Cyc_Yyltype _T14F3;unsigned _T14F4;unsigned _T14F5;struct Cyc_Absyn_Exp*_T14F6;struct Cyc_Yystacktype*_T14F7;union Cyc_YYSTYPE*_T14F8;union Cyc_YYSTYPE*_T14F9;struct Cyc_Absyn_Exp*_T14FA;struct Cyc_Yystacktype*_T14FB;struct Cyc_Yystacktype _T14FC;struct Cyc_Yyltype _T14FD;unsigned _T14FE;unsigned _T14FF;struct Cyc_Absyn_Exp*_T1500;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1501;struct Cyc_Yystacktype*_T1502;union Cyc_YYSTYPE*_T1503;union Cyc_YYSTYPE*_T1504;struct _fat_ptr*_T1505;struct Cyc_Yystacktype*_T1506;union Cyc_YYSTYPE*_T1507;union Cyc_YYSTYPE*_T1508;void*_T1509;struct Cyc_Yystacktype*_T150A;struct Cyc_Yystacktype _T150B;struct Cyc_Yyltype _T150C;unsigned _T150D;unsigned _T150E;struct Cyc_Absyn_Exp*_T150F;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1510;struct Cyc_Yystacktype*_T1511;union Cyc_YYSTYPE*_T1512;union Cyc_YYSTYPE*_T1513;struct Cyc_Absyn_Exp*_T1514;struct Cyc_Yystacktype*_T1515;struct Cyc_Yystacktype _T1516;struct Cyc_Yyltype _T1517;unsigned _T1518;unsigned _T1519;struct _fat_ptr*_T151A;struct Cyc_Yystacktype*_T151B;union Cyc_YYSTYPE*_T151C;union Cyc_YYSTYPE*_T151D;void*_T151E;struct Cyc_Yystacktype*_T151F;struct Cyc_Yystacktype _T1520;struct Cyc_Yyltype _T1521;unsigned _T1522;unsigned _T1523;struct Cyc_Absyn_Exp*_T1524;struct Cyc_Yystacktype*_T1525;union Cyc_YYSTYPE*_T1526;union Cyc_YYSTYPE*_T1527;struct _tuple8*_T1528;struct Cyc_Yystacktype*_T1529;struct Cyc_Yystacktype _T152A;struct Cyc_Yyltype _T152B;unsigned _T152C;unsigned _T152D;void*_T152E;struct Cyc_Yystacktype*_T152F;struct Cyc_Yystacktype _T1530;struct Cyc_Yyltype _T1531;unsigned _T1532;unsigned _T1533;struct Cyc_Absyn_Exp*_T1534;struct Cyc_Yystacktype*_T1535;union Cyc_YYSTYPE*_T1536;union Cyc_YYSTYPE*_T1537;void*_T1538;struct Cyc_Yystacktype*_T1539;struct Cyc_Yystacktype _T153A;struct Cyc_Yyltype _T153B;unsigned _T153C;unsigned _T153D;struct Cyc_Absyn_Exp*_T153E;struct Cyc_Yystacktype*_T153F;union Cyc_YYSTYPE*_T1540;union Cyc_YYSTYPE*_T1541;struct Cyc_Absyn_Exp*_T1542;struct Cyc_Yystacktype*_T1543;struct Cyc_Yystacktype _T1544;struct Cyc_Yyltype _T1545;unsigned _T1546;unsigned _T1547;struct Cyc_Absyn_Exp*_T1548;struct Cyc_Yystacktype*_T1549;union Cyc_YYSTYPE*_T154A;union Cyc_YYSTYPE*_T154B;struct Cyc_Absyn_Exp*_T154C;struct Cyc_Yystacktype*_T154D;struct Cyc_Yystacktype _T154E;struct Cyc_Yyltype _T154F;unsigned _T1550;unsigned _T1551;struct Cyc_Absyn_Exp*_T1552;struct Cyc_Yystacktype*_T1553;union Cyc_YYSTYPE*_T1554;union Cyc_YYSTYPE*_T1555;struct Cyc_Absyn_Exp*_T1556;struct Cyc_Yystacktype*_T1557;struct Cyc_Yystacktype _T1558;struct Cyc_Yyltype _T1559;unsigned _T155A;unsigned _T155B;struct Cyc_Absyn_Exp*_T155C;struct Cyc_Yystacktype*_T155D;union Cyc_YYSTYPE*_T155E;union Cyc_YYSTYPE*_T155F;struct Cyc_Absyn_Exp*_T1560;struct Cyc_Yystacktype*_T1561;struct Cyc_Yystacktype _T1562;struct Cyc_Yyltype _T1563;unsigned _T1564;unsigned _T1565;struct Cyc_Absyn_Exp*_T1566;struct Cyc_Yystacktype*_T1567;union Cyc_YYSTYPE*_T1568;union Cyc_YYSTYPE*_T1569;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T156A;struct Cyc_Yystacktype*_T156B;union Cyc_YYSTYPE*_T156C;union Cyc_YYSTYPE*_T156D;struct Cyc_Yystacktype*_T156E;union Cyc_YYSTYPE*_T156F;union Cyc_YYSTYPE*_T1570;void*_T1571;struct _tuple31*_T1572;struct Cyc_Yystacktype*_T1573;union Cyc_YYSTYPE*_T1574;union Cyc_YYSTYPE*_T1575;struct _tuple31*_T1576;struct Cyc_Yystacktype*_T1577;union Cyc_YYSTYPE*_T1578;union Cyc_YYSTYPE*_T1579;struct _tuple31*_T157A;struct Cyc_Yystacktype*_T157B;union Cyc_YYSTYPE*_T157C;union Cyc_YYSTYPE*_T157D;struct Cyc_List_List*_T157E;struct Cyc_List_List*_T157F;struct Cyc_Yystacktype*_T1580;union Cyc_YYSTYPE*_T1581;union Cyc_YYSTYPE*_T1582;struct Cyc_List_List*_T1583;struct Cyc_Yystacktype*_T1584;union Cyc_YYSTYPE*_T1585;union Cyc_YYSTYPE*_T1586;struct Cyc_Yystacktype*_T1587;union Cyc_YYSTYPE*_T1588;union Cyc_YYSTYPE*_T1589;struct _tuple28*_T158A;struct _tuple28*_T158B;struct Cyc_Yystacktype*_T158C;union Cyc_YYSTYPE*_T158D;union Cyc_YYSTYPE*_T158E;struct _tuple28*_T158F;struct Cyc_Yystacktype*_T1590;union Cyc_YYSTYPE*_T1591;union Cyc_YYSTYPE*_T1592;struct Cyc_List_List*_T1593;struct Cyc_Yystacktype*_T1594;union Cyc_YYSTYPE*_T1595;union Cyc_YYSTYPE*_T1596;struct Cyc_List_List*_T1597;struct Cyc_Yystacktype*_T1598;union Cyc_YYSTYPE*_T1599;union Cyc_YYSTYPE*_T159A;struct Cyc_List_List*_T159B;struct Cyc_Yystacktype*_T159C;union Cyc_YYSTYPE*_T159D;union Cyc_YYSTYPE*_T159E;struct Cyc_Yystacktype*_T159F;union Cyc_YYSTYPE*_T15A0;union Cyc_YYSTYPE*_T15A1;struct _tuple32*_T15A2;struct Cyc_Yystacktype*_T15A3;union Cyc_YYSTYPE*_T15A4;union Cyc_YYSTYPE*_T15A5;struct Cyc_Yystacktype*_T15A6;union Cyc_YYSTYPE*_T15A7;union Cyc_YYSTYPE*_T15A8;struct Cyc_Yystacktype*_T15A9;union Cyc_YYSTYPE*_T15AA;union Cyc_YYSTYPE*_T15AB;struct Cyc_List_List*_T15AC;struct Cyc_List_List*_T15AD;struct Cyc_List_List*_T15AE;struct _fat_ptr*_T15AF;struct Cyc_Yystacktype*_T15B0;union Cyc_YYSTYPE*_T15B1;union Cyc_YYSTYPE*_T15B2;struct Cyc_List_List*_T15B3;struct _fat_ptr*_T15B4;struct Cyc_Yystacktype*_T15B5;union Cyc_YYSTYPE*_T15B6;union Cyc_YYSTYPE*_T15B7;struct Cyc_Yystacktype*_T15B8;union Cyc_YYSTYPE*_T15B9;union Cyc_YYSTYPE*_T15BA;struct Cyc_Yystacktype*_T15BB;struct Cyc_Yystacktype _T15BC;struct Cyc_Yystacktype*_T15BD;union Cyc_YYSTYPE*_T15BE;union Cyc_YYSTYPE*_T15BF;struct Cyc_Absyn_Exp*_T15C0;struct Cyc_Yystacktype*_T15C1;union Cyc_YYSTYPE*_T15C2;union Cyc_YYSTYPE*_T15C3;struct Cyc_Absyn_Exp*_T15C4;struct Cyc_Yystacktype*_T15C5;struct Cyc_Yystacktype _T15C6;struct Cyc_Yyltype _T15C7;unsigned _T15C8;unsigned _T15C9;struct Cyc_Absyn_Exp*_T15CA;struct Cyc_Yystacktype*_T15CB;union Cyc_YYSTYPE*_T15CC;union Cyc_YYSTYPE*_T15CD;struct Cyc_Absyn_Exp*_T15CE;struct Cyc_Yystacktype*_T15CF;struct Cyc_Yystacktype _T15D0;struct Cyc_Yyltype _T15D1;unsigned _T15D2;unsigned _T15D3;struct Cyc_Absyn_Exp*_T15D4;struct Cyc_Yystacktype*_T15D5;union Cyc_YYSTYPE*_T15D6;union Cyc_YYSTYPE*_T15D7;struct Cyc_Absyn_Exp*_T15D8;struct Cyc_Yystacktype*_T15D9;union Cyc_YYSTYPE*_T15DA;union Cyc_YYSTYPE*_T15DB;struct Cyc_List_List*_T15DC;struct Cyc_Yystacktype*_T15DD;struct Cyc_Yystacktype _T15DE;struct Cyc_Yyltype _T15DF;unsigned _T15E0;unsigned _T15E1;struct Cyc_Absyn_Exp*_T15E2;struct Cyc_Yystacktype*_T15E3;union Cyc_YYSTYPE*_T15E4;union Cyc_YYSTYPE*_T15E5;struct Cyc_Absyn_Exp*_T15E6;struct _fat_ptr*_T15E7;struct Cyc_Yystacktype*_T15E8;union Cyc_YYSTYPE*_T15E9;union Cyc_YYSTYPE*_T15EA;struct Cyc_Yystacktype*_T15EB;struct Cyc_Yystacktype _T15EC;struct Cyc_Yyltype _T15ED;unsigned _T15EE;unsigned _T15EF;struct Cyc_Absyn_Exp*_T15F0;struct Cyc_Yystacktype*_T15F1;union Cyc_YYSTYPE*_T15F2;union Cyc_YYSTYPE*_T15F3;struct Cyc_Absyn_Exp*_T15F4;struct _fat_ptr*_T15F5;struct Cyc_Yystacktype*_T15F6;union Cyc_YYSTYPE*_T15F7;union Cyc_YYSTYPE*_T15F8;struct Cyc_Yystacktype*_T15F9;struct Cyc_Yystacktype _T15FA;struct Cyc_Yyltype _T15FB;unsigned _T15FC;unsigned _T15FD;struct Cyc_Absyn_Exp*_T15FE;struct Cyc_Yystacktype*_T15FF;union Cyc_YYSTYPE*_T1600;union Cyc_YYSTYPE*_T1601;struct Cyc_Absyn_Exp*_T1602;struct Cyc_Yystacktype*_T1603;struct Cyc_Yystacktype _T1604;struct Cyc_Yyltype _T1605;unsigned _T1606;unsigned _T1607;struct Cyc_Absyn_Exp*_T1608;struct Cyc_Yystacktype*_T1609;union Cyc_YYSTYPE*_T160A;union Cyc_YYSTYPE*_T160B;struct Cyc_Absyn_Exp*_T160C;struct Cyc_Yystacktype*_T160D;struct Cyc_Yystacktype _T160E;struct Cyc_Yyltype _T160F;unsigned _T1610;unsigned _T1611;struct Cyc_Absyn_Exp*_T1612;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1613;struct Cyc_Yystacktype*_T1614;union Cyc_YYSTYPE*_T1615;union Cyc_YYSTYPE*_T1616;void*_T1617;struct Cyc_Yystacktype*_T1618;struct Cyc_Yystacktype _T1619;struct Cyc_Yyltype _T161A;unsigned _T161B;unsigned _T161C;struct Cyc_Absyn_Exp*_T161D;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T161E;struct Cyc_Yystacktype*_T161F;union Cyc_YYSTYPE*_T1620;union Cyc_YYSTYPE*_T1621;struct Cyc_Yystacktype*_T1622;union Cyc_YYSTYPE*_T1623;union Cyc_YYSTYPE*_T1624;struct Cyc_List_List*_T1625;void*_T1626;struct Cyc_Yystacktype*_T1627;struct Cyc_Yystacktype _T1628;struct Cyc_Yyltype _T1629;unsigned _T162A;unsigned _T162B;struct Cyc_Absyn_Exp*_T162C;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T162D;struct Cyc_Yystacktype*_T162E;union Cyc_YYSTYPE*_T162F;union Cyc_YYSTYPE*_T1630;struct Cyc_Yystacktype*_T1631;union Cyc_YYSTYPE*_T1632;union Cyc_YYSTYPE*_T1633;struct Cyc_List_List*_T1634;void*_T1635;struct Cyc_Yystacktype*_T1636;struct Cyc_Yystacktype _T1637;struct Cyc_Yyltype _T1638;unsigned _T1639;unsigned _T163A;struct Cyc_Absyn_Exp*_T163B;struct Cyc_List_List*_T163C;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T163D;struct _fat_ptr*_T163E;struct Cyc_Yystacktype*_T163F;union Cyc_YYSTYPE*_T1640;union Cyc_YYSTYPE*_T1641;struct Cyc_Yystacktype*_T1642;struct Cyc_Yystacktype _T1643;struct Cyc_Yyltype _T1644;unsigned _T1645;unsigned _T1646;struct Cyc_Yystacktype*_T1647;union Cyc_YYSTYPE*_T1648;union Cyc_YYSTYPE*_T1649;union Cyc_Absyn_Cnst _T164A;unsigned _T164B;int _T164C;struct Cyc_List_List*_T164D;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T164E;struct Cyc_List_List*_T164F;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1650;struct _fat_ptr*_T1651;struct Cyc_Yystacktype*_T1652;union Cyc_YYSTYPE*_T1653;union Cyc_YYSTYPE*_T1654;struct Cyc_Yystacktype*_T1655;union Cyc_YYSTYPE*_T1656;union Cyc_YYSTYPE*_T1657;struct Cyc_Yystacktype*_T1658;struct Cyc_Yystacktype _T1659;struct Cyc_Yyltype _T165A;unsigned _T165B;unsigned _T165C;struct Cyc_Yystacktype*_T165D;union Cyc_YYSTYPE*_T165E;union Cyc_YYSTYPE*_T165F;union Cyc_Absyn_Cnst _T1660;unsigned _T1661;int _T1662;struct Cyc_List_List*_T1663;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1664;struct Cyc_Yystacktype*_T1665;union Cyc_YYSTYPE*_T1666;union Cyc_YYSTYPE*_T1667;struct Cyc_Yystacktype*_T1668;union Cyc_YYSTYPE*_T1669;union Cyc_YYSTYPE*_T166A;struct _tuple0*_T166B;struct Cyc_Yystacktype*_T166C;struct Cyc_Yystacktype _T166D;struct Cyc_Yyltype _T166E;unsigned _T166F;unsigned _T1670;struct Cyc_Absyn_Exp*_T1671;struct Cyc_Yystacktype*_T1672;union Cyc_YYSTYPE*_T1673;union Cyc_YYSTYPE*_T1674;struct _fat_ptr _T1675;struct Cyc_Yystacktype*_T1676;struct Cyc_Yystacktype _T1677;struct Cyc_Yyltype _T1678;unsigned _T1679;unsigned _T167A;struct Cyc_Absyn_Exp*_T167B;struct Cyc_Yystacktype*_T167C;struct Cyc_Yystacktype _T167D;struct Cyc_Yystacktype*_T167E;union Cyc_YYSTYPE*_T167F;union Cyc_YYSTYPE*_T1680;struct _fat_ptr _T1681;struct Cyc_Yystacktype*_T1682;struct Cyc_Yystacktype _T1683;struct Cyc_Yyltype _T1684;unsigned _T1685;unsigned _T1686;struct Cyc_Absyn_Exp*_T1687;struct Cyc_Yystacktype*_T1688;union Cyc_YYSTYPE*_T1689;union Cyc_YYSTYPE*_T168A;struct _fat_ptr _T168B;struct Cyc_Yystacktype*_T168C;struct Cyc_Yystacktype _T168D;struct Cyc_Yyltype _T168E;unsigned _T168F;unsigned _T1690;struct Cyc_Absyn_Exp*_T1691;struct Cyc_Yystacktype*_T1692;struct Cyc_Yystacktype _T1693;struct Cyc_Yystacktype*_T1694;union Cyc_YYSTYPE*_T1695;union Cyc_YYSTYPE*_T1696;struct Cyc_Absyn_Exp*_T1697;struct Cyc_Yystacktype*_T1698;struct Cyc_Yystacktype _T1699;struct Cyc_Yyltype _T169A;unsigned _T169B;unsigned _T169C;struct Cyc_Absyn_Exp*_T169D;struct Cyc_Yystacktype*_T169E;union Cyc_YYSTYPE*_T169F;union Cyc_YYSTYPE*_T16A0;struct Cyc_Absyn_Exp*_T16A1;struct Cyc_Yystacktype*_T16A2;union Cyc_YYSTYPE*_T16A3;union Cyc_YYSTYPE*_T16A4;struct Cyc_List_List*_T16A5;struct Cyc_List_List*_T16A6;struct Cyc_Yystacktype*_T16A7;struct Cyc_Yystacktype _T16A8;struct Cyc_Yyltype _T16A9;unsigned _T16AA;unsigned _T16AB;struct Cyc_Absyn_Exp*_T16AC;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T16AD;struct Cyc_Yystacktype*_T16AE;union Cyc_YYSTYPE*_T16AF;union Cyc_YYSTYPE*_T16B0;struct Cyc_Yystacktype*_T16B1;union Cyc_YYSTYPE*_T16B2;union Cyc_YYSTYPE*_T16B3;struct Cyc_Yystacktype*_T16B4;union Cyc_YYSTYPE*_T16B5;union Cyc_YYSTYPE*_T16B6;struct Cyc_List_List*_T16B7;void*_T16B8;struct Cyc_Yystacktype*_T16B9;struct Cyc_Yystacktype _T16BA;struct Cyc_Yyltype _T16BB;unsigned _T16BC;unsigned _T16BD;struct Cyc_Absyn_Exp*_T16BE;struct Cyc_Yystacktype*_T16BF;union Cyc_YYSTYPE*_T16C0;union Cyc_YYSTYPE*_T16C1;struct Cyc_List_List*_T16C2;struct Cyc_Yystacktype*_T16C3;struct Cyc_Yystacktype _T16C4;struct Cyc_Yyltype _T16C5;unsigned _T16C6;unsigned _T16C7;struct Cyc_Absyn_Exp*_T16C8;struct Cyc_Yystacktype*_T16C9;union Cyc_YYSTYPE*_T16CA;union Cyc_YYSTYPE*_T16CB;struct Cyc_Absyn_Stmt*_T16CC;struct Cyc_Yystacktype*_T16CD;struct Cyc_Yystacktype _T16CE;struct Cyc_Yyltype _T16CF;unsigned _T16D0;unsigned _T16D1;struct Cyc_Absyn_Exp*_T16D2;struct Cyc_Yystacktype*_T16D3;union Cyc_YYSTYPE*_T16D4;union Cyc_YYSTYPE*_T16D5;struct Cyc_List_List*_T16D6;struct Cyc_List_List*_T16D7;struct Cyc_List_List*_T16D8;struct Cyc_Yystacktype*_T16D9;union Cyc_YYSTYPE*_T16DA;union Cyc_YYSTYPE*_T16DB;struct Cyc_List_List*_T16DC;struct Cyc_Yystacktype*_T16DD;union Cyc_YYSTYPE*_T16DE;union Cyc_YYSTYPE*_T16DF;struct Cyc_Yystacktype*_T16E0;union Cyc_YYSTYPE*_T16E1;union Cyc_YYSTYPE*_T16E2;struct Cyc_Yystacktype*_T16E3;union Cyc_YYSTYPE*_T16E4;union Cyc_YYSTYPE*_T16E5;union Cyc_Absyn_Cnst _T16E6;struct Cyc_Yystacktype*_T16E7;struct Cyc_Yystacktype _T16E8;struct Cyc_Yyltype _T16E9;unsigned _T16EA;unsigned _T16EB;struct Cyc_Absyn_Exp*_T16EC;struct Cyc_Yystacktype*_T16ED;union Cyc_YYSTYPE*_T16EE;union Cyc_YYSTYPE*_T16EF;char _T16F0;struct Cyc_Yystacktype*_T16F1;struct Cyc_Yystacktype _T16F2;struct Cyc_Yyltype _T16F3;unsigned _T16F4;unsigned _T16F5;struct Cyc_Absyn_Exp*_T16F6;struct Cyc_Yystacktype*_T16F7;union Cyc_YYSTYPE*_T16F8;union Cyc_YYSTYPE*_T16F9;struct _fat_ptr _T16FA;struct Cyc_Yystacktype*_T16FB;struct Cyc_Yystacktype _T16FC;struct Cyc_Yyltype _T16FD;unsigned _T16FE;unsigned _T16FF;struct Cyc_Absyn_Exp*_T1700;struct Cyc_Yystacktype*_T1701;struct Cyc_Yystacktype _T1702;struct Cyc_Yyltype _T1703;unsigned _T1704;unsigned _T1705;struct Cyc_Absyn_Exp*_T1706;struct Cyc_Yystacktype*_T1707;union Cyc_YYSTYPE*_T1708;union Cyc_YYSTYPE*_T1709;unsigned long _T170A;struct _fat_ptr _T170B;unsigned char*_T170C;const char*_T170D;int _T170E;char _T170F;int _T1710;char _T1711;int _T1712;char _T1713;int _T1714;char _T1715;int _T1716;struct _fat_ptr _T1717;int _T1718;struct Cyc_Yystacktype*_T1719;struct Cyc_Yystacktype _T171A;struct Cyc_Yyltype _T171B;unsigned _T171C;unsigned _T171D;struct Cyc_Absyn_Exp*_T171E;struct _tuple0*_T171F;struct _fat_ptr*_T1720;struct Cyc_Yystacktype*_T1721;union Cyc_YYSTYPE*_T1722;union Cyc_YYSTYPE*_T1723;struct Cyc_Yystacktype*_T1724;struct Cyc_Yystacktype _T1725;struct _tuple0*_T1726;struct _fat_ptr*_T1727;struct Cyc_Yystacktype*_T1728;union Cyc_YYSTYPE*_T1729;union Cyc_YYSTYPE*_T172A;struct Cyc_Yystacktype*_T172B;struct Cyc_Yystacktype _T172C;struct Cyc_Yystacktype*_T172D;struct Cyc_Yystacktype _T172E;struct Cyc_Yystacktype*_T172F;struct Cyc_Yystacktype _T1730;struct Cyc_Yystacktype*_T1731;struct Cyc_Yystacktype _T1732;struct Cyc_Yystacktype*_T1733;struct Cyc_Yystacktype _T1734;struct Cyc_Lexing_lexbuf*_T1735;struct Cyc_List_List*_T1736;struct _tuple35*_T1737;struct Cyc_Yystacktype*_T1738;union Cyc_YYSTYPE*_T1739;union Cyc_YYSTYPE*_T173A;struct _fat_ptr _T173B;struct Cyc_Yystacktype*_T173C;union Cyc_YYSTYPE*_T173D;union Cyc_YYSTYPE*_T173E;struct _fat_ptr _T173F;struct _fat_ptr _T1740;int _T1741;int _T1742;struct Cyc_Yystacktype*_T1743;union Cyc_YYSTYPE*_T1744;union Cyc_YYSTYPE*_T1745;struct _fat_ptr _T1746;void*_T1747;struct Cyc_Yystacktype*_T1748;union Cyc_YYSTYPE*_T1749;union Cyc_YYSTYPE*_T174A;struct Cyc_Yystacktype*_T174B;union Cyc_YYSTYPE*_T174C;union Cyc_YYSTYPE*_T174D;struct Cyc_Yystacktype*_T174E;union Cyc_YYSTYPE*_T174F;union Cyc_YYSTYPE*_T1750;struct Cyc_List_List*_T1751;struct Cyc_List_List*_T1752;struct Cyc_Yystacktype*_T1753;union Cyc_YYSTYPE*_T1754;union Cyc_YYSTYPE*_T1755;struct Cyc_List_List*_T1756;struct Cyc_Yystacktype*_T1757;union Cyc_YYSTYPE*_T1758;union Cyc_YYSTYPE*_T1759;struct Cyc_Yystacktype*_T175A;union Cyc_YYSTYPE*_T175B;union Cyc_YYSTYPE*_T175C;struct Cyc_Yystacktype*_T175D;union Cyc_YYSTYPE*_T175E;union Cyc_YYSTYPE*_T175F;struct _fat_ptr _T1760;void*_T1761;struct Cyc_Yystacktype*_T1762;struct Cyc_Yystacktype _T1763;struct Cyc_Yyltype _T1764;unsigned _T1765;unsigned _T1766;struct Cyc_Yystacktype*_T1767;union Cyc_YYSTYPE*_T1768;union Cyc_YYSTYPE*_T1769;struct _fat_ptr _T176A;void*_T176B;struct Cyc_Yystacktype*_T176C;union Cyc_YYSTYPE*_T176D;union Cyc_YYSTYPE*_T176E;struct _fat_ptr _T176F;struct Cyc_Yystacktype*_T1770;union Cyc_YYSTYPE*_T1771;union Cyc_YYSTYPE*_T1772;void*_T1773;void*_T1774;void*_T1775;struct Cyc_Yystacktype*_T1776;union Cyc_YYSTYPE*_T1777;union Cyc_YYSTYPE*_T1778;struct _fat_ptr _T1779;struct Cyc_Yystacktype*_T177A;union Cyc_YYSTYPE*_T177B;union Cyc_YYSTYPE*_T177C;enum Cyc_Parse_ConstraintOps _T177D;struct Cyc_Yystacktype*_T177E;union Cyc_YYSTYPE*_T177F;union Cyc_YYSTYPE*_T1780;void*_T1781;struct Cyc_Yystacktype*_T1782;union Cyc_YYSTYPE*_T1783;union Cyc_YYSTYPE*_T1784;void*_T1785;void*_T1786;void*_T1787;struct Cyc_Yystacktype*_T1788;union Cyc_YYSTYPE*_T1789;union Cyc_YYSTYPE*_T178A;struct _fat_ptr _T178B;struct Cyc_Yystacktype*_T178C;union Cyc_YYSTYPE*_T178D;union Cyc_YYSTYPE*_T178E;void*_T178F;struct Cyc_Yystacktype*_T1790;union Cyc_YYSTYPE*_T1791;union Cyc_YYSTYPE*_T1792;void*_T1793;void*_T1794;void*_T1795;struct Cyc_Yystacktype*_T1796;union Cyc_YYSTYPE*_T1797;union Cyc_YYSTYPE*_T1798;struct _fat_ptr _T1799;struct Cyc_Yystacktype*_T179A;union Cyc_YYSTYPE*_T179B;union Cyc_YYSTYPE*_T179C;void*_T179D;struct Cyc_Yystacktype*_T179E;union Cyc_YYSTYPE*_T179F;union Cyc_YYSTYPE*_T17A0;void*_T17A1;void*_T17A2;void*_T17A3;struct Cyc_Yystacktype*_T17A4;union Cyc_YYSTYPE*_T17A5;union Cyc_YYSTYPE*_T17A6;struct _fat_ptr _T17A7;struct Cyc_Yystacktype*_T17A8;union Cyc_YYSTYPE*_T17A9;union Cyc_YYSTYPE*_T17AA;void*_T17AB;struct Cyc_Yystacktype*_T17AC;union Cyc_YYSTYPE*_T17AD;union Cyc_YYSTYPE*_T17AE;void*_T17AF;void*_T17B0;void*_T17B1;struct Cyc_Yystacktype*_T17B2;union Cyc_YYSTYPE*_T17B3;union Cyc_YYSTYPE*_T17B4;struct _fat_ptr _T17B5;struct Cyc_Yystacktype*_T17B6;union Cyc_YYSTYPE*_T17B7;union Cyc_YYSTYPE*_T17B8;void*_T17B9;struct Cyc_Yystacktype*_T17BA;union Cyc_YYSTYPE*_T17BB;union Cyc_YYSTYPE*_T17BC;void*_T17BD;void*_T17BE;void*_T17BF;struct Cyc_Yystacktype*_T17C0;union Cyc_YYSTYPE*_T17C1;union Cyc_YYSTYPE*_T17C2;struct _fat_ptr _T17C3;struct Cyc_Yystacktype*_T17C4;union Cyc_YYSTYPE*_T17C5;union Cyc_YYSTYPE*_T17C6;enum Cyc_Parse_ConstraintOps _T17C7;struct Cyc_Yystacktype*_T17C8;union Cyc_YYSTYPE*_T17C9;union Cyc_YYSTYPE*_T17CA;void*_T17CB;struct Cyc_Yystacktype*_T17CC;union Cyc_YYSTYPE*_T17CD;union Cyc_YYSTYPE*_T17CE;void*_T17CF;void*_T17D0;void*_T17D1;int _T17D2;int _T17D3;struct _fat_ptr _T17D4;int _T17D5;unsigned char*_T17D6;struct Cyc_Yystacktype*_T17D7;struct _fat_ptr _T17D8;int _T17D9;struct _fat_ptr _T17DA;unsigned char*_T17DB;unsigned char*_T17DC;struct Cyc_Yystacktype*_T17DD;struct Cyc_Yyltype _T17DE;struct Cyc_Yystacktype*_T17DF;struct Cyc_Yyltype _T17E0;struct Cyc_Yystacktype*_T17E1;struct Cyc_Yystacktype*_T17E2;struct Cyc_Yystacktype _T17E3;struct Cyc_Yyltype _T17E4;struct Cyc_Yystacktype*_T17E5;struct Cyc_Yystacktype*_T17E6;struct Cyc_Yystacktype _T17E7;struct Cyc_Yyltype _T17E8;struct _fat_ptr _T17E9;unsigned char*_T17EA;struct Cyc_Yystacktype*_T17EB;int _T17EC;struct _fat_ptr _T17ED;int _T17EE;int _T17EF;unsigned char*_T17F0;struct Cyc_Yystacktype*_T17F1;struct Cyc_Yystacktype _T17F2;struct Cyc_Yyltype _T17F3;struct _fat_ptr _T17F4;unsigned char*_T17F5;struct Cyc_Yystacktype*_T17F6;int _T17F7;struct _fat_ptr _T17F8;unsigned char*_T17F9;struct Cyc_Yystacktype*_T17FA;int _T17FB;int _T17FC;struct Cyc_Yystacktype _T17FD;struct Cyc_Yyltype _T17FE;short*_T17FF;int _T1800;char*_T1801;short*_T1802;short _T1803;short*_T1804;int _T1805;char*_T1806;short*_T1807;short _T1808;int _T1809;struct _fat_ptr _T180A;int _T180B;unsigned char*_T180C;short*_T180D;short _T180E;int _T180F;short*_T1810;int _T1811;short _T1812;int _T1813;struct _fat_ptr _T1814;unsigned char*_T1815;short*_T1816;int _T1817;short _T1818;int _T1819;short*_T181A;int _T181B;short _T181C;short*_T181D;int _T181E;short _T181F;short*_T1820;int _T1821;char*_T1822;short*_T1823;short _T1824;int _T1825;int _T1826;int _T1827;int _T1828;unsigned _T1829;unsigned _T182A;short*_T182B;int _T182C;char*_T182D;short*_T182E;short _T182F;int _T1830;int _T1831;struct _fat_ptr*_T1832;int _T1833;char*_T1834;struct _fat_ptr*_T1835;struct _fat_ptr _T1836;unsigned long _T1837;unsigned long _T1838;struct _fat_ptr _T1839;int _T183A;unsigned _T183B;char*_T183C;struct _RegionHandle*_T183D;unsigned _T183E;unsigned _T183F;struct _fat_ptr _T1840;struct _fat_ptr _T1841;int _T1842;int _T1843;unsigned _T1844;unsigned _T1845;short*_T1846;int _T1847;char*_T1848;short*_T1849;short _T184A;int _T184B;int _T184C;struct _fat_ptr _T184D;struct _fat_ptr _T184E;struct _fat_ptr _T184F;struct _fat_ptr _T1850;struct _fat_ptr _T1851;struct _fat_ptr*_T1852;int _T1853;char*_T1854;struct _fat_ptr*_T1855;struct _fat_ptr _T1856;struct _fat_ptr _T1857;struct _fat_ptr _T1858;struct _fat_ptr _T1859;int _T185A;int _T185B;struct _fat_ptr _T185C;int _T185D;unsigned char*_T185E;short*_T185F;short _T1860;short*_T1861;int _T1862;char*_T1863;short*_T1864;short _T1865;int _T1866;int _T1867;short*_T1868;int _T1869;short _T186A;int _T186B;short*_T186C;int _T186D;short _T186E;int _T186F;int _T1870;struct _fat_ptr _T1871;int _T1872;unsigned char*_T1873;struct Cyc_Yystacktype*_T1874;struct Cyc_Yystacktype _T1875;struct _RegionHandle _T1876=_new_region(0U,"yyregion");struct _RegionHandle*yyregion=& _T1876;_push_region(yyregion);{
# 149
int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;{union Cyc_YYSTYPE _T1877;_T1877.YYINITIALSVAL.tag=1U;
_T1877.YYINITIALSVAL.val=0;_T0=_T1877;}{union Cyc_YYSTYPE yylval=_T0;
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;{unsigned _T1877=200U;_T2=yyregion;_T3=_region_calloc(_T2,0U,sizeof(short),_T1877);_T1=_tag_fat(_T3,sizeof(short),_T1877);}{
# 164
struct _fat_ptr yyss=_T1;
# 166
int yyvsp_offset;{unsigned _T1877=200U;_T6=yyregion;_T7=_check_times(_T1877,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T1878=_region_malloc(_T6,0U,_T7);{unsigned _T1879=_T1877;unsigned i;i=0;_TL250: if(i < _T1879)goto _TL24E;else{goto _TL24F;}_TL24E: _T8=i;
# 169
_T1878[_T8].v=yylval;_T9=i;_T1878[_T9].l=Cyc_yynewloc();i=i + 1;goto _TL250;_TL24F:;}_T5=(struct Cyc_Yystacktype*)_T1878;}_T4=_T5;}{
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
if(yystacksize < 10000)goto _TL253;_T12=
_tag_fat("parser stack overflow",sizeof(char),22U);_T13=yystate;_T14=yychar;Cyc_yyerror(_T12,_T13,_T14);_T15=& Cyc_Yystack_overflow_val;_T16=(struct Cyc_Yystack_overflow_exn_struct*)_T15;_throw(_T16);goto _TL254;_TL253: _TL254:
# 216
 yystacksize=yystacksize * 2;
if(yystacksize <= 10000)goto _TL255;
yystacksize=10000;goto _TL256;_TL255: _TL256: _T18=yystacksize;{unsigned _T1877=(unsigned)_T18;_T1A=yyregion;_T1B=_check_times(_T1877,sizeof(short));{short*_T1878=_region_malloc(_T1A,0U,_T1B);{unsigned _T1879=_T1877;unsigned i;i=0;_TL25A: if(i < _T1879)goto _TL258;else{goto _TL259;}_TL258: _T1C=i;_T1D=yyssp_offset;_T1E=(unsigned)_T1D;
# 220
if(_T1C > _T1E)goto _TL25B;_T1F=i;_T20=yyss;_T21=_T20.curr;_T22=(short*)_T21;_T23=i;_T24=(int)_T23;_T1878[_T1F]=_T22[_T24];goto _TL25C;_TL25B: _T25=i;_T1878[_T25]=0;_TL25C: i=i + 1;goto _TL25A;_TL259:;}_T19=(short*)_T1878;}_T17=
# 219
_tag_fat(_T19,sizeof(short),_T1877);}{struct _fat_ptr yyss1=_T17;_T27=yystacksize;{unsigned _T1877=(unsigned)_T27;_T29=yyregion;_T2A=_check_times(_T1877,sizeof(struct Cyc_Yystacktype));{struct Cyc_Yystacktype*_T1878=_region_malloc(_T29,0U,_T2A);{unsigned _T1879=_T1877;unsigned i;i=0;_TL260: if(i < _T1879)goto _TL25E;else{goto _TL25F;}_TL25E: _T2B=i;_T2C=yyssp_offset;_T2D=(unsigned)_T2C;
# 224
if(_T2B > _T2D)goto _TL261;_T2E=i;_T2F=yyvs;_T30=i;_T31=(int)_T30;_T32=_check_fat_subscript(_T2F,sizeof(struct Cyc_Yystacktype),_T31);_T33=(struct Cyc_Yystacktype*)_T32;_T1878[_T2E]=*_T33;goto _TL262;_TL261: _T34=i;_T35=yyvs;_T36=_check_fat_subscript(_T35,sizeof(struct Cyc_Yystacktype),0);_T37=(struct Cyc_Yystacktype*)_T36;_T1878[_T34]=*_T37;_TL262: i=i + 1;goto _TL260;_TL25F:;}_T28=(struct Cyc_Yystacktype*)_T1878;}_T26=
# 223
_tag_fat(_T28,sizeof(struct Cyc_Yystacktype),_T1877);}{
# 222
struct _fat_ptr yyvs1=_T26;
# 230
yyss=yyss1;
yyvs=yyvs1;}}goto _TL252;_TL251: _TL252: goto yybackup;
# 242
yybackup: _T38=Cyc_yypact;_T39=yystate;_T3A=_check_known_subscript_notnull(_T38,1262U,sizeof(short),_T39);_T3B=(short*)_T3A;_T3C=*_T3B;
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
_TL267: if(yychar <= 0)goto _TL269;if(yychar > 400)goto _TL269;_T47=Cyc_yytranslate;_T48=yychar;_T49=_T47[_T48];_T46=(int)_T49;goto _TL26A;_TL269: _T46=363;_TL26A: yychar1=_T46;_TL268: _T4A=yychar1;
# 299 "cycbison.simple"
yyn=yyn + _T4A;
if(yyn < 0)goto _TL26D;else{goto _TL26F;}_TL26F: if(yyn > 8241)goto _TL26D;else{goto _TL26E;}_TL26E: _T4B=Cyc_yycheck;_T4C=yyn;_T4D=_T4B[_T4C];_T4E=(int)_T4D;_T4F=yychar1;if(_T4E!=_T4F)goto _TL26D;else{goto _TL26B;}_TL26D: goto yydefault;_TL26B: _T50=Cyc_yytable;_T51=yyn;_T52=_T50[_T51];
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
 if(yyn!=1261)goto _TL276;{int _T1877=0;_npop_handler(0);return _T1877;}_TL276:
# 328 "cycbison.simple"
 if(yychar==0)goto _TL278;
yychar=- 2;goto _TL279;_TL278: _TL279: _T55=yyvs;
# 332
yyvsp_offset=yyvsp_offset + 1;_T56=yyvsp_offset;_T57=_check_fat_subscript(_T55,sizeof(struct Cyc_Yystacktype),_T56);_T58=(struct Cyc_Yystacktype*)_T57;{struct Cyc_Yystacktype _T1877;_T1877.v=yylval;_T1877.l=yylloc;_T59=_T1877;}*_T58=_T59;
# 338
if(yyerrstatus==0)goto _TL27A;yyerrstatus=yyerrstatus + -1;goto _TL27B;_TL27A: _TL27B:
# 340
 yystate=yyn;goto yynewstate;
# 344
yydefault: _T5A=Cyc_yydefact;_T5B=yystate;_T5C=_check_known_subscript_notnull(_T5A,1262U,sizeof(short),_T5B);_T5D=(short*)_T5C;_T5E=*_T5D;
# 346
yyn=(int)_T5E;
if(yyn!=0)goto _TL27C;goto yyerrlab;_TL27C:
# 351
 yyreduce: _T5F=Cyc_yyr2;_T60=yyn;_T61=_check_known_subscript_notnull(_T5F,624U,sizeof(short),_T60);_T62=(short*)_T61;_T63=*_T62;
# 353
yylen=(int)_T63;_T64=yyvs;_T65=yyvsp_offset + 1;_T66=yylen;_T67=_T65 - _T66;_T68=
_fat_ptr_plus(_T64,sizeof(struct Cyc_Yystacktype),_T67);_T69=_untag_fat_ptr_check_bound(_T68,sizeof(struct Cyc_Yystacktype),14U);_T6A=_check_null(_T69);yyyvsp=(struct Cyc_Yystacktype*)_T6A;
if(yylen <= 0)goto _TL27E;_T6B=yyyvsp;_T6C=_T6B[0];
yyval=_T6C.v;goto _TL27F;_TL27E: _TL27F: _T6D=yyn;_T6E=(int)_T6D;switch(_T6E){case 1:
# 1323 "parse.y"
 yyval=Cyc_YY74(0);goto _LL0;case 2:
# 1326 "parse.y"
 yyval=Cyc_YY74(1);_T6F=yyyvsp;_T70=& _T6F[0].v;_T71=(union Cyc_YYSTYPE*)_T70;
Cyc_Parse_constraint_graph=Cyc_yyget_YY71(_T71);goto _LL0;case 3: _T72=yyyvsp;_T73=_T72[0];
# 1333 "parse.y"
yyval=_T73.v;_T74=yyyvsp;_T75=& _T74[0].v;_T76=(union Cyc_YYSTYPE*)_T75;
Cyc_Parse_parse_result=Cyc_yyget_YY16(_T76);goto _LL0;case 4: _T77=yyyvsp;_T78=& _T77[0].v;_T79=(union Cyc_YYSTYPE*)_T78;_T7A=
# 1339 "parse.y"
Cyc_yyget_YY16(_T79);_T7B=yyyvsp;_T7C=& _T7B[1].v;_T7D=(union Cyc_YYSTYPE*)_T7C;_T7E=Cyc_yyget_YY16(_T7D);_T7F=Cyc_List_imp_append(_T7A,_T7E);yyval=Cyc_YY16(_T7F);goto _LL0;case 5:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1879=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T1879->tag=10;_T83=yyyvsp;_T84=& _T83[0].v;_T85=(union Cyc_YYSTYPE*)_T84;
# 1343 "parse.y"
_T1879->f1=Cyc_yyget_QualId_tok(_T85);_T86=yyyvsp;_T87=& _T86[2].v;_T88=(union Cyc_YYSTYPE*)_T87;_T1879->f2=Cyc_yyget_YY16(_T88);_T82=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1879;}_T1878->r=(void*)_T82;_T89=yyyvsp;_T8A=_T89[0];_T8B=_T8A.l;_T8C=_T8B.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_T8C);_T81=(struct Cyc_Absyn_Decl*)_T1878;}_T1877->hd=_T81;_T1877->tl=0;_T80=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T80);
Cyc_Lex_leave_using();goto _LL0;case 6:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T1879=_cycalloc(sizeof(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct));_T1879->tag=10;_T90=yyyvsp;_T91=& _T90[0].v;_T92=(union Cyc_YYSTYPE*)_T91;
# 1347 "parse.y"
_T1879->f1=Cyc_yyget_QualId_tok(_T92);_T93=yyyvsp;_T94=& _T93[2].v;_T95=(union Cyc_YYSTYPE*)_T94;_T1879->f2=Cyc_yyget_YY16(_T95);_T8F=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T1879;}_T1878->r=(void*)_T8F;_T96=yyyvsp;_T97=_T96[0];_T98=_T97.l;_T99=_T98.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_T99);_T8E=(struct Cyc_Absyn_Decl*)_T1878;}_T1877->hd=_T8E;_T9A=yyyvsp;_T9B=& _T9A[4].v;_T9C=(union Cyc_YYSTYPE*)_T9B;_T1877->tl=Cyc_yyget_YY16(_T9C);_T8D=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T8D);goto _LL0;case 7:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1879=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T1879->tag=9;{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_TA1=yyyvsp;_TA2=& _TA1[0].v;_TA3=(union Cyc_YYSTYPE*)_TA2;
# 1350
*_T187A=Cyc_yyget_String_tok(_TA3);_TA0=(struct _fat_ptr*)_T187A;}_T1879->f1=_TA0;_TA4=yyyvsp;_TA5=& _TA4[2].v;_TA6=(union Cyc_YYSTYPE*)_TA5;_T1879->f2=Cyc_yyget_YY16(_TA6);_T9F=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1879;}_T1878->r=(void*)_T9F;_TA7=yyyvsp;_TA8=_TA7[0];_TA9=_TA8.l;_TAA=_TA9.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_TAA);_T9E=(struct Cyc_Absyn_Decl*)_T1878;}_T1877->hd=_T9E;_T1877->tl=0;_T9D=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T9D);
Cyc_Lex_leave_namespace();goto _LL0;case 8:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T1879=_cycalloc(sizeof(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct));_T1879->tag=9;{struct _fat_ptr*_T187A=_cycalloc(sizeof(struct _fat_ptr));_TAF=yyyvsp;_TB0=& _TAF[0].v;_TB1=(union Cyc_YYSTYPE*)_TB0;
# 1354 "parse.y"
*_T187A=Cyc_yyget_String_tok(_TB1);_TAE=(struct _fat_ptr*)_T187A;}_T1879->f1=_TAE;_TB2=yyyvsp;_TB3=& _TB2[2].v;_TB4=(union Cyc_YYSTYPE*)_TB3;_T1879->f2=Cyc_yyget_YY16(_TB4);_TAD=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T1879;}_T1878->r=(void*)_TAD;_TB5=yyyvsp;_TB6=_TB5[0];_TB7=_TB6.l;_TB8=_TB7.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_TB8);_TAC=(struct Cyc_Absyn_Decl*)_T1878;}_T1877->hd=_TAC;_TB9=yyyvsp;_TBA=& _TB9[4].v;_TBB=(union Cyc_YYSTYPE*)_TBA;_T1877->tl=Cyc_yyget_YY16(_TBB);_TAB=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_TAB);goto _LL0;case 9: _TBC=yyyvsp;_TBD=& _TBC[2].v;_TBE=(union Cyc_YYSTYPE*)_TBD;_TBF=
# 1356 "parse.y"
Cyc_yyget_YY16(_TBE);_TC0=yyyvsp;_TC1=& _TC0[4].v;_TC2=(union Cyc_YYSTYPE*)_TC1;_TC3=Cyc_yyget_YY16(_TC2);_TC4=Cyc_List_append(_TBF,_TC3);yyval=Cyc_YY16(_TC4);goto _LL0;case 10: _TC5=yyyvsp;_TC6=& _TC5[0].v;_TC7=(union Cyc_YYSTYPE*)_TC6;{
# 1358 "parse.y"
int is_c_include=Cyc_yyget_YY32(_TC7);_TC8=yyyvsp;_TC9=& _TC8[4].v;_TCA=(union Cyc_YYSTYPE*)_TC9;{
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(_TCA);_TCB=yyyvsp;_TCC=& _TCB[5].v;_TCD=(union Cyc_YYSTYPE*)_TCC;{
struct _tuple30*_T1877=Cyc_yyget_YY56(_TCD);unsigned _T1878;struct Cyc_List_List*_T1879;{struct _tuple30 _T187A=*_T1877;_T1879=_T187A.f0;_T1878=_T187A.f1;}{struct Cyc_List_List*exs=_T1879;unsigned wc=_T1878;_TCE=yyyvsp;_TCF=& _TCE[6].v;_TD0=(union Cyc_YYSTYPE*)_TCF;{
struct Cyc_List_List*hides=Cyc_yyget_YY57(_TD0);
if(exs==0)goto _TL281;if(hides==0)goto _TL281;_TD1=yyyvsp;_TD2=_TD1[0];_TD3=_TD2.l;_TD4=_TD3.first_line;_TD5=
Cyc_Position_loc_to_seg(_TD4);_TD6=_tag_fat("hide list can only be used with export { * }, or no export block",sizeof(char),65U);_TD7=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TD5,_TD6,_TD7);goto _TL282;_TL281: _TL282: _TD8=hides;_TD9=(unsigned)_TD8;
if(!_TD9)goto _TL283;_TDA=wc;_TDB=(int)_TDA;if(_TDB)goto _TL283;else{goto _TL285;}
_TL285: _TDC=yyyvsp;_TDD=_TDC[6];_TDE=_TDD.l;_TDF=_TDE.first_line;wc=Cyc_Position_loc_to_seg(_TDF);goto _TL284;_TL283: _TL284: _TE0=is_c_include;
if(_TE0)goto _TL286;else{goto _TL288;}
_TL288: if(exs!=0)goto _TL28B;else{goto _TL28C;}_TL28C: if(cycdecls!=0)goto _TL28B;else{goto _TL289;}
_TL28B: _TE1=yyyvsp;_TE2=_TE1[0];_TE3=_TE2.l;_TE4=_TE3.first_line;_TE5=Cyc_Position_loc_to_seg(_TE4);_TE6=_tag_fat("expecting \"C include\"",sizeof(char),22U);_TE7=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_TE5,_TE6,_TE7);{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T187C=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T187C->tag=12;_TEB=yyyvsp;_TEC=& _TEB[2].v;_TED=(union Cyc_YYSTYPE*)_TEC;
_T187C->f1=Cyc_yyget_YY16(_TED);_T187C->f2=cycdecls;_T187C->f3=exs;{struct _tuple10*_T187D=_cycalloc(sizeof(struct _tuple10));_T187D->f0=wc;_T187D->f1=hides;_TEE=(struct _tuple10*)_T187D;}_T187C->f4=_TEE;_TEA=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T187C;}_T187B->r=(void*)_TEA;_TEF=yyyvsp;_TF0=_TEF[0];_TF1=_TF0.l;_TF2=_TF1.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_TF2);_TE9=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_TE9;_TF3=yyyvsp;_TF4=& _TF3[7].v;_TF5=(union Cyc_YYSTYPE*)_TF4;_T187A->tl=Cyc_yyget_YY16(_TF5);_TE8=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_TE8);goto _TL28A;
# 1372
_TL289:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T187C=_cycalloc(sizeof(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct));_T187C->tag=11;_TF9=yyyvsp;_TFA=& _TF9[2].v;_TFB=(union Cyc_YYSTYPE*)_TFA;_T187C->f1=Cyc_yyget_YY16(_TFB);_TF8=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T187C;}_T187B->r=(void*)_TF8;_TFC=yyyvsp;_TFD=_TFC[0];_TFE=_TFD.l;_TFF=_TFE.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_TFF);_TF7=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_TF7;_T100=yyyvsp;_T101=& _T100[7].v;_T102=(union Cyc_YYSTYPE*)_T101;_T187A->tl=Cyc_yyget_YY16(_T102);_TF6=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_TF6);_TL28A: goto _TL287;
# 1375
_TL286:{struct Cyc_List_List*_T187A=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Decl));{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T187C=_cycalloc(sizeof(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct));_T187C->tag=12;_T106=yyyvsp;_T107=& _T106[2].v;_T108=(union Cyc_YYSTYPE*)_T107;_T187C->f1=Cyc_yyget_YY16(_T108);_T187C->f2=cycdecls;_T187C->f3=exs;{struct _tuple10*_T187D=_cycalloc(sizeof(struct _tuple10));_T187D->f0=wc;_T187D->f1=hides;_T109=(struct _tuple10*)_T187D;}_T187C->f4=_T109;_T105=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T187C;}_T187B->r=(void*)_T105;_T10A=yyyvsp;_T10B=_T10A[0];_T10C=_T10B.l;_T10D=_T10C.first_line;_T187B->loc=Cyc_Position_loc_to_seg(_T10D);_T104=(struct Cyc_Absyn_Decl*)_T187B;}_T187A->hd=_T104;_T10E=yyyvsp;_T10F=& _T10E[7].v;_T110=(union Cyc_YYSTYPE*)_T10F;_T187A->tl=Cyc_yyget_YY16(_T110);_T103=(struct Cyc_List_List*)_T187A;}yyval=Cyc_YY16(_T103);_TL287: goto _LL0;}}}}}case 11:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T113=& Cyc_Absyn_Porton_d_val;_T114=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_T113;
# 1378 "parse.y"
_T1878->r=(void*)_T114;_T115=yyyvsp;_T116=_T115[0];_T117=_T116.l;_T118=_T117.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_T118);_T112=(struct Cyc_Absyn_Decl*)_T1878;}_T1877->hd=_T112;_T119=yyyvsp;_T11A=& _T119[2].v;_T11B=(union Cyc_YYSTYPE*)_T11A;_T1877->tl=Cyc_yyget_YY16(_T11B);_T111=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T111);goto _LL0;case 12:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T11E=& Cyc_Absyn_Portoff_d_val;_T11F=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_T11E;
# 1380 "parse.y"
_T1878->r=(void*)_T11F;_T120=yyyvsp;_T121=_T120[0];_T122=_T121.l;_T123=_T122.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_T123);_T11D=(struct Cyc_Absyn_Decl*)_T1878;}_T1877->hd=_T11D;_T124=yyyvsp;_T125=& _T124[2].v;_T126=(union Cyc_YYSTYPE*)_T125;_T1877->tl=Cyc_yyget_YY16(_T126);_T11C=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T11C);goto _LL0;case 13:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T129=& Cyc_Absyn_Tempeston_d_val;_T12A=(struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct*)_T129;
# 1382 "parse.y"
_T1878->r=(void*)_T12A;_T12B=yyyvsp;_T12C=_T12B[0];_T12D=_T12C.l;_T12E=_T12D.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_T12E);_T128=(struct Cyc_Absyn_Decl*)_T1878;}_T1877->hd=_T128;_T12F=yyyvsp;_T130=& _T12F[2].v;_T131=(union Cyc_YYSTYPE*)_T130;_T1877->tl=Cyc_yyget_YY16(_T131);_T127=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T127);goto _LL0;case 14:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Decl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Decl));_T134=& Cyc_Absyn_Tempestoff_d_val;_T135=(struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct*)_T134;
# 1384 "parse.y"
_T1878->r=(void*)_T135;_T136=yyyvsp;_T137=_T136[0];_T138=_T137.l;_T139=_T138.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_T139);_T133=(struct Cyc_Absyn_Decl*)_T1878;}_T1877->hd=_T133;_T13A=yyyvsp;_T13B=& _T13A[2].v;_T13C=(union Cyc_YYSTYPE*)_T13B;_T1877->tl=Cyc_yyget_YY16(_T13C);_T132=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T132);goto _LL0;case 15:
# 1385 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 16:
# 1389 "parse.y"
 Cyc_Parse_parsing_tempest=1;goto _LL0;case 17:
# 1392
 Cyc_Parse_parsing_tempest=0;goto _LL0;case 18:
# 1397 "parse.y"
 Cyc_Lex_enter_extern_c();_T13D=yyyvsp;_T13E=& _T13D[1].v;_T13F=(union Cyc_YYSTYPE*)_T13E;_T140=
Cyc_yyget_String_tok(_T13F);_T141=_tag_fat("C",sizeof(char),2U);_T142=Cyc_strcmp(_T140,_T141);if(_T142!=0)goto _TL28D;
yyval=Cyc_YY32(0);goto _TL28E;
_TL28D: _T143=yyyvsp;_T144=& _T143[1].v;_T145=(union Cyc_YYSTYPE*)_T144;_T146=Cyc_yyget_String_tok(_T145);_T147=_tag_fat("C include",sizeof(char),10U);_T148=Cyc_strcmp(_T146,_T147);if(_T148!=0)goto _TL28F;
yyval=Cyc_YY32(1);goto _TL290;
# 1403
_TL28F: _T149=yyyvsp;_T14A=_T149[0];_T14B=_T14A.l;_T14C=_T14B.first_line;_T14D=Cyc_Position_loc_to_seg(_T14C);_T14E=_tag_fat("expecting \"C\" or \"C include\"",sizeof(char),29U);_T14F=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T14D,_T14E,_T14F);
yyval=Cyc_YY32(1);_TL290: _TL28E: goto _LL0;case 19:
# 1410 "parse.y"
 Cyc_Lex_leave_extern_c();goto _LL0;case 20:
# 1414 "parse.y"
 yyval=Cyc_YY57(0);goto _LL0;case 21: _T150=yyyvsp;_T151=_T150[2];
# 1415 "parse.y"
yyval=_T151.v;goto _LL0;case 22:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T153=yyyvsp;_T154=& _T153[0].v;_T155=(union Cyc_YYSTYPE*)_T154;
# 1419 "parse.y"
_T1877->hd=Cyc_yyget_QualId_tok(_T155);_T1877->tl=0;_T152=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY57(_T152);goto _LL0;case 23:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T157=yyyvsp;_T158=& _T157[0].v;_T159=(union Cyc_YYSTYPE*)_T158;
# 1420 "parse.y"
_T1877->hd=Cyc_yyget_QualId_tok(_T159);_T1877->tl=0;_T156=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY57(_T156);goto _LL0;case 24:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T15B=yyyvsp;_T15C=& _T15B[0].v;_T15D=(union Cyc_YYSTYPE*)_T15C;
# 1421 "parse.y"
_T1877->hd=Cyc_yyget_QualId_tok(_T15D);_T15E=yyyvsp;_T15F=& _T15E[2].v;_T160=(union Cyc_YYSTYPE*)_T15F;_T1877->tl=Cyc_yyget_YY57(_T160);_T15A=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY57(_T15A);goto _LL0;case 25:{struct _tuple30*_T1877=_cycalloc(sizeof(struct _tuple30));
# 1425 "parse.y"
_T1877->f0=0;_T1877->f1=0U;_T161=(struct _tuple30*)_T1877;}yyval=Cyc_YY56(_T161);goto _LL0;case 26: _T162=yyyvsp;_T163=_T162[0];
# 1426 "parse.y"
yyval=_T163.v;goto _LL0;case 27:{struct _tuple30*_T1877=_cycalloc(sizeof(struct _tuple30));_T165=yyyvsp;_T166=& _T165[2].v;_T167=(union Cyc_YYSTYPE*)_T166;
# 1430 "parse.y"
_T1877->f0=Cyc_yyget_YY55(_T167);_T1877->f1=0U;_T164=(struct _tuple30*)_T1877;}yyval=Cyc_YY56(_T164);goto _LL0;case 28:{struct _tuple30*_T1877=_cycalloc(sizeof(struct _tuple30));
# 1431 "parse.y"
_T1877->f0=0;_T1877->f1=0U;_T168=(struct _tuple30*)_T1877;}yyval=Cyc_YY56(_T168);goto _LL0;case 29:{struct _tuple30*_T1877=_cycalloc(sizeof(struct _tuple30));
# 1432 "parse.y"
_T1877->f0=0;_T16A=yyyvsp;_T16B=_T16A[0];_T16C=_T16B.l;_T16D=_T16C.first_line;_T1877->f1=Cyc_Position_loc_to_seg(_T16D);_T169=(struct _tuple30*)_T1877;}yyval=Cyc_YY56(_T169);goto _LL0;case 30:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T1878=_cycalloc(sizeof(struct _tuple33));_T170=yyyvsp;_T171=_T170[0];_T172=_T171.l;_T173=_T172.first_line;
# 1437 "parse.y"
_T1878->f0=Cyc_Position_loc_to_seg(_T173);_T174=yyyvsp;_T175=& _T174[0].v;_T176=(union Cyc_YYSTYPE*)_T175;_T1878->f1=Cyc_yyget_QualId_tok(_T176);_T1878->f2=0;_T16F=(struct _tuple33*)_T1878;}_T1877->hd=_T16F;_T1877->tl=0;_T16E=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY55(_T16E);goto _LL0;case 31:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple33*_T1878=_cycalloc(sizeof(struct _tuple33));_T179=yyyvsp;_T17A=_T179[0];_T17B=_T17A.l;_T17C=_T17B.first_line;
# 1439 "parse.y"
_T1878->f0=Cyc_Position_loc_to_seg(_T17C);_T17D=yyyvsp;_T17E=& _T17D[0].v;_T17F=(union Cyc_YYSTYPE*)_T17E;_T1878->f1=Cyc_yyget_QualId_tok(_T17F);_T1878->f2=0;_T178=(struct _tuple33*)_T1878;}_T1877->hd=_T178;_T180=yyyvsp;_T181=& _T180[2].v;_T182=(union Cyc_YYSTYPE*)_T181;_T1877->tl=Cyc_yyget_YY55(_T182);_T177=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY55(_T177);goto _LL0;case 32:
# 1443 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 33: _T183=yyyvsp;_T184=_T183[2];
# 1444 "parse.y"
yyval=_T184.v;goto _LL0;case 34:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1878->tag=1;_T187=yyyvsp;_T188=& _T187[0].v;_T189=(union Cyc_YYSTYPE*)_T188;
# 1448 "parse.y"
_T1878->f1=Cyc_yyget_YY15(_T189);_T186=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1878;}_T18A=(void*)_T186;_T18B=yyyvsp;_T18C=_T18B[0];_T18D=_T18C.l;_T18E=_T18D.first_line;_T18F=Cyc_Position_loc_to_seg(_T18E);_T1877->hd=Cyc_Absyn_new_decl(_T18A,_T18F);_T1877->tl=0;_T185=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T185);goto _LL0;case 35: _T190=yyyvsp;_T191=_T190[0];
# 1449 "parse.y"
yyval=_T191.v;goto _LL0;case 36:
# 1450 "parse.y"
 yyval=Cyc_YY16(0);goto _LL0;case 39: _T192=yyr;_T193=yyyvsp;_T194=& _T193[0].v;_T195=(union Cyc_YYSTYPE*)_T194;_T196=
# 1484 "parse.y"
Cyc_yyget_YY28(_T195);_T197=yyyvsp;_T198=& _T197[1].v;_T199=(union Cyc_YYSTYPE*)_T198;_T19A=Cyc_yyget_Stmt_tok(_T199);_T19B=yyyvsp;_T19C=_T19B[0];_T19D=_T19C.l;_T19E=_T19D.first_line;_T19F=Cyc_Position_loc_to_seg(_T19E);_T1A0=Cyc_Parse_make_function(_T192,0,_T196,0,_T19A,_T19F);yyval=Cyc_YY15(_T1A0);goto _LL0;case 40: _T1A1=yyyvsp;_T1A2=& _T1A1[0].v;_T1A3=(union Cyc_YYSTYPE*)_T1A2;{
# 1486 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1A3);_T1A4=yyr;_T1A5=& d;_T1A6=(struct Cyc_Parse_Declaration_spec*)_T1A5;_T1A7=yyyvsp;_T1A8=& _T1A7[1].v;_T1A9=(union Cyc_YYSTYPE*)_T1A8;_T1AA=
Cyc_yyget_YY28(_T1A9);_T1AB=yyyvsp;_T1AC=& _T1AB[2].v;_T1AD=(union Cyc_YYSTYPE*)_T1AC;_T1AE=Cyc_yyget_Stmt_tok(_T1AD);_T1AF=yyyvsp;_T1B0=_T1AF[0];_T1B1=_T1B0.l;_T1B2=_T1B1.first_line;_T1B3=Cyc_Position_loc_to_seg(_T1B2);_T1B4=Cyc_Parse_make_function(_T1A4,_T1A6,_T1AA,0,_T1AE,_T1B3);yyval=Cyc_YY15(_T1B4);goto _LL0;}case 41: _T1B5=yyr;_T1B6=yyyvsp;_T1B7=& _T1B6[0].v;_T1B8=(union Cyc_YYSTYPE*)_T1B7;_T1B9=
# 1498 "parse.y"
Cyc_yyget_YY28(_T1B8);_T1BA=yyyvsp;_T1BB=& _T1BA[1].v;_T1BC=(union Cyc_YYSTYPE*)_T1BB;_T1BD=Cyc_yyget_YY16(_T1BC);_T1BE=yyyvsp;_T1BF=& _T1BE[2].v;_T1C0=(union Cyc_YYSTYPE*)_T1BF;_T1C1=Cyc_yyget_Stmt_tok(_T1C0);_T1C2=yyyvsp;_T1C3=_T1C2[0];_T1C4=_T1C3.l;_T1C5=_T1C4.first_line;_T1C6=Cyc_Position_loc_to_seg(_T1C5);_T1C7=Cyc_Parse_make_function(_T1B5,0,_T1B9,_T1BD,_T1C1,_T1C6);yyval=Cyc_YY15(_T1C7);goto _LL0;case 42: _T1C8=yyyvsp;_T1C9=& _T1C8[0].v;_T1CA=(union Cyc_YYSTYPE*)_T1C9;{
# 1500 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1CA);_T1CB=yyr;_T1CC=& d;_T1CD=(struct Cyc_Parse_Declaration_spec*)_T1CC;_T1CE=yyyvsp;_T1CF=& _T1CE[1].v;_T1D0=(union Cyc_YYSTYPE*)_T1CF;_T1D1=
Cyc_yyget_YY28(_T1D0);_T1D2=yyyvsp;_T1D3=& _T1D2[2].v;_T1D4=(union Cyc_YYSTYPE*)_T1D3;_T1D5=Cyc_yyget_YY16(_T1D4);_T1D6=yyyvsp;_T1D7=& _T1D6[3].v;_T1D8=(union Cyc_YYSTYPE*)_T1D7;_T1D9=Cyc_yyget_Stmt_tok(_T1D8);_T1DA=yyyvsp;_T1DB=_T1DA[0];_T1DC=_T1DB.l;_T1DD=_T1DC.first_line;_T1DE=Cyc_Position_loc_to_seg(_T1DD);_T1DF=Cyc_Parse_make_function(_T1CB,_T1CD,_T1D1,_T1D5,_T1D9,_T1DE);yyval=Cyc_YY15(_T1DF);goto _LL0;}case 43: _T1E0=yyyvsp;_T1E1=& _T1E0[0].v;_T1E2=(union Cyc_YYSTYPE*)_T1E1;{
# 1508 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1E2);_T1E3=yyr;_T1E4=& d;_T1E5=(struct Cyc_Parse_Declaration_spec*)_T1E4;_T1E6=yyyvsp;_T1E7=& _T1E6[1].v;_T1E8=(union Cyc_YYSTYPE*)_T1E7;_T1E9=
Cyc_yyget_YY28(_T1E8);_T1EA=yyyvsp;_T1EB=& _T1EA[2].v;_T1EC=(union Cyc_YYSTYPE*)_T1EB;_T1ED=Cyc_yyget_Stmt_tok(_T1EC);_T1EE=yyyvsp;_T1EF=_T1EE[0];_T1F0=_T1EF.l;_T1F1=_T1F0.first_line;_T1F2=Cyc_Position_loc_to_seg(_T1F1);_T1F3=Cyc_Parse_make_function(_T1E3,_T1E5,_T1E9,0,_T1ED,_T1F2);yyval=Cyc_YY15(_T1F3);goto _LL0;}case 44: _T1F4=yyyvsp;_T1F5=& _T1F4[0].v;_T1F6=(union Cyc_YYSTYPE*)_T1F5;{
# 1511 "parse.y"
struct Cyc_Parse_Declaration_spec d=Cyc_yyget_YY17(_T1F6);_T1F7=yyr;_T1F8=& d;_T1F9=(struct Cyc_Parse_Declaration_spec*)_T1F8;_T1FA=yyyvsp;_T1FB=& _T1FA[1].v;_T1FC=(union Cyc_YYSTYPE*)_T1FB;_T1FD=
Cyc_yyget_YY28(_T1FC);_T1FE=yyyvsp;_T1FF=& _T1FE[2].v;_T200=(union Cyc_YYSTYPE*)_T1FF;_T201=Cyc_yyget_YY16(_T200);_T202=yyyvsp;_T203=& _T202[3].v;_T204=(union Cyc_YYSTYPE*)_T203;_T205=Cyc_yyget_Stmt_tok(_T204);_T206=yyyvsp;_T207=_T206[0];_T208=_T207.l;_T209=_T208.first_line;_T20A=Cyc_Position_loc_to_seg(_T209);_T20B=Cyc_Parse_make_function(_T1F7,_T1F9,_T1FD,_T201,_T205,_T20A);yyval=Cyc_YY15(_T20B);goto _LL0;}case 45: _T20C=yyyvsp;_T20D=& _T20C[1].v;_T20E=(union Cyc_YYSTYPE*)_T20D;_T20F=
# 1516 "parse.y"
Cyc_yyget_QualId_tok(_T20E);Cyc_Lex_enter_using(_T20F);_T210=yyyvsp;_T211=_T210[1];yyval=_T211.v;goto _LL0;case 46:
# 1519
 Cyc_Lex_leave_using();goto _LL0;case 47:{struct _fat_ptr*_T1877=_cycalloc(sizeof(struct _fat_ptr));_T213=yyyvsp;_T214=& _T213[1].v;_T215=(union Cyc_YYSTYPE*)_T214;
# 1522
*_T1877=Cyc_yyget_String_tok(_T215);_T212=(struct _fat_ptr*)_T1877;}Cyc_Lex_enter_namespace(_T212);_T216=yyyvsp;_T217=_T216[1];yyval=_T217.v;goto _LL0;case 48:
# 1525
 Cyc_Lex_leave_namespace();goto _LL0;case 49:
# 1528
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + 1;goto _LL0;case 50:
# 1531
 Cyc_Parse_inside_noinference_block=Cyc_Parse_inside_noinference_block + -1;goto _LL0;case 51: _T218=yyyvsp;_T219=& _T218[0].v;_T21A=(union Cyc_YYSTYPE*)_T219;_T21B=
# 1536 "parse.y"
Cyc_yyget_YY17(_T21A);_T21C=yyyvsp;_T21D=_T21C[0];_T21E=_T21D.l;_T21F=_T21E.first_line;_T220=Cyc_Position_loc_to_seg(_T21F);_T221=yyyvsp;_T222=_T221[0];_T223=_T222.l;_T224=_T223.first_line;_T225=Cyc_Position_loc_to_seg(_T224);_T226=Cyc_Parse_make_declarations(_T21B,0,_T220,_T225);yyval=Cyc_YY16(_T226);goto _LL0;case 52: _T227=yyyvsp;_T228=& _T227[0].v;_T229=(union Cyc_YYSTYPE*)_T228;_T22A=
# 1538 "parse.y"
Cyc_yyget_YY17(_T229);{struct _tuple11*(*_T1877)(struct _tuple11*)=(struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev;_T22B=_T1877;}_T22C=yyyvsp;_T22D=& _T22C[1].v;_T22E=(union Cyc_YYSTYPE*)_T22D;_T22F=Cyc_yyget_YY19(_T22E);_T230=_T22B(_T22F);_T231=yyyvsp;_T232=_T231[0];_T233=_T232.l;_T234=_T233.first_line;_T235=Cyc_Position_loc_to_seg(_T234);_T236=yyyvsp;_T237=_T236[0];_T238=_T237.l;_T239=_T238.first_line;_T23A=Cyc_Position_loc_to_seg(_T239);_T23B=Cyc_Parse_make_declarations(_T22A,_T230,_T235,_T23A);yyval=Cyc_YY16(_T23B);goto _LL0;case 53:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T23D=yyyvsp;_T23E=& _T23D[1].v;_T23F=(union Cyc_YYSTYPE*)_T23E;_T240=
# 1541
Cyc_yyget_YY9(_T23F);_T241=yyyvsp;_T242=& _T241[3].v;_T243=(union Cyc_YYSTYPE*)_T242;_T244=Cyc_yyget_Exp_tok(_T243);_T245=yyyvsp;_T246=_T245[0];_T247=_T246.l;_T248=_T247.first_line;_T249=Cyc_Position_loc_to_seg(_T248);_T1877->hd=Cyc_Absyn_let_decl(_T240,_T244,_T249);_T1877->tl=0;_T23C=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T23C);goto _LL0;case 54:  {
# 1543 "parse.y"
struct Cyc_List_List*vds=0;_T24A=yyyvsp;_T24B=& _T24A[1].v;_T24C=(union Cyc_YYSTYPE*)_T24B;{
struct Cyc_List_List*ids=Cyc_yyget_YY37(_T24C);_TL294: if(ids!=0)goto _TL292;else{goto _TL293;}
_TL292:{struct _tuple0*qv;qv=_cycalloc(sizeof(struct _tuple0));_T24D=qv;_T24D->f0=Cyc_Absyn_Rel_n(0);_T24E=qv;_T24F=ids;_T250=_T24F->hd;_T24E->f1=(struct _fat_ptr*)_T250;{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T252=qv;_T253=
Cyc_Absyn_wildtyp(0);_T1877->hd=Cyc_Absyn_new_vardecl(0U,_T252,_T253,0,0);_T1877->tl=vds;_T251=(struct Cyc_List_List*)_T1877;}vds=_T251;}_T254=ids;
# 1544
ids=_T254->tl;goto _TL294;_TL293:;}{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T256=
# 1548
Cyc_List_imp_rev(vds);_T257=yyyvsp;_T258=_T257[0];_T259=_T258.l;_T25A=_T259.first_line;_T25B=Cyc_Position_loc_to_seg(_T25A);_T1877->hd=Cyc_Absyn_letv_decl(_T256,_T25B);_T1877->tl=0;_T255=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T255);goto _LL0;}case 55: _T25C=yyyvsp;_T25D=& _T25C[2].v;_T25E=(union Cyc_YYSTYPE*)_T25D;_T25F=
# 1553 "parse.y"
Cyc_yyget_String_tok(_T25E);_T260=yyyvsp;_T261=_T260[2];_T262=_T261.l;_T263=_T262.first_line;_T264=Cyc_Position_loc_to_seg(_T263);Cyc_Parse_tvar_ok(_T25F,_T264);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T265=tv;{struct _fat_ptr*_T1877=_cycalloc(sizeof(struct _fat_ptr));_T267=yyyvsp;_T268=& _T267[2].v;_T269=(union Cyc_YYSTYPE*)_T268;*_T1877=Cyc_yyget_String_tok(_T269);_T266=(struct _fat_ptr*)_T1877;}_T265->name=_T266;_T26A=tv;_T26A->identity=- 1;_T26B=tv;_T26C=& Cyc_Kinds_ek;_T26D=(struct Cyc_Absyn_Kind*)_T26C;_T26B->kind=Cyc_Kinds_kind_to_bound(_T26D);_T26E=tv;_T26E->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T26F=yyyvsp;_T270=_T26F[4];_T271=_T270.l;_T272=_T271.first_line;_T273=
Cyc_Position_loc_to_seg(_T272);{struct _tuple0*_T1877=_cycalloc(sizeof(struct _tuple0));_T1877->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T276=yyyvsp;_T277=& _T276[4].v;_T278=(union Cyc_YYSTYPE*)_T277;*_T1878=Cyc_yyget_String_tok(_T278);_T275=(struct _fat_ptr*)_T1878;}_T1877->f1=_T275;_T274=(struct _tuple0*)_T1877;}_T279=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T273,_T274,_T279,0,0);{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T27B=tv;_T27C=vd;_T27D=yyyvsp;_T27E=_T27D[0];_T27F=_T27E.l;_T280=_T27F.first_line;_T281=
Cyc_Position_loc_to_seg(_T280);_T1877->hd=Cyc_Absyn_region_decl(_T27B,_T27C,0,_T281);_T1877->tl=0;_T27A=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T27A);goto _LL0;}}}case 56:{struct Cyc_String_pa_PrintArg_struct _T1877;_T1877.tag=0;_T284=yyyvsp;_T285=& _T284[1].v;_T286=(union Cyc_YYSTYPE*)_T285;
# 1561
_T1877.f1=Cyc_yyget_String_tok(_T286);_T283=_T1877;}{struct Cyc_String_pa_PrintArg_struct _T1877=_T283;void*_T1878[1];_T1878[0]=& _T1877;_T287=_tag_fat("`%s",sizeof(char),4U);_T288=_tag_fat(_T1878,sizeof(void*),1);_T282=Cyc_aprintf(_T287,_T288);}{struct _fat_ptr tvstring=_T282;_T289=yyyvsp;_T28A=& _T289[1].v;_T28B=(union Cyc_YYSTYPE*)_T28A;_T28C=
Cyc_yyget_String_tok(_T28B);_T28D=yyyvsp;_T28E=_T28D[1];_T28F=_T28E.l;_T290=_T28F.first_line;_T291=Cyc_Position_loc_to_seg(_T290);Cyc_Parse_tvar_ok(_T28C,_T291);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T292=tv;{struct _fat_ptr*_T1877=_cycalloc(sizeof(struct _fat_ptr));*_T1877=tvstring;_T293=(struct _fat_ptr*)_T1877;}_T292->name=_T293;_T294=tv;_T294->identity=- 1;_T295=tv;_T296=& Cyc_Kinds_ek;_T297=(struct Cyc_Absyn_Kind*)_T296;_T295->kind=Cyc_Kinds_kind_to_bound(_T297);_T298=tv;_T298->aquals_bound=0;{
void*t=Cyc_Absyn_var_type(tv);_T299=yyyvsp;_T29A=_T299[1];_T29B=_T29A.l;_T29C=_T29B.first_line;_T29D=
Cyc_Position_loc_to_seg(_T29C);{struct _tuple0*_T1877=_cycalloc(sizeof(struct _tuple0));_T1877->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T2A0=yyyvsp;_T2A1=& _T2A0[1].v;_T2A2=(union Cyc_YYSTYPE*)_T2A1;*_T1878=Cyc_yyget_String_tok(_T2A2);_T29F=(struct _fat_ptr*)_T1878;}_T1877->f1=_T29F;_T29E=(struct _tuple0*)_T1877;}_T2A3=Cyc_Absyn_rgn_handle_type(t);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T29D,_T29E,_T2A3,0,0);{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T2A5=tv;_T2A6=vd;_T2A7=yyyvsp;_T2A8=& _T2A7[2].v;_T2A9=(union Cyc_YYSTYPE*)_T2A8;_T2AA=
Cyc_yyget_YY61(_T2A9);_T2AB=yyyvsp;_T2AC=_T2AB[0];_T2AD=_T2AC.l;_T2AE=_T2AD.first_line;_T2AF=Cyc_Position_loc_to_seg(_T2AE);_T1877->hd=Cyc_Absyn_region_decl(_T2A5,_T2A6,_T2AA,_T2AF);_T1877->tl=0;_T2A4=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY16(_T2A4);goto _LL0;}}}}case 57:
# 1571 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 58: _T2B0=yyyvsp;_T2B1=& _T2B0[1].v;_T2B2=(union Cyc_YYSTYPE*)_T2B1;_T2B3=
# 1573 "parse.y"
Cyc_yyget_String_tok(_T2B2);_T2B4=_tag_fat("open",sizeof(char),5U);_T2B5=Cyc_strcmp(_T2B3,_T2B4);if(_T2B5==0)goto _TL295;_T2B6=yyyvsp;_T2B7=_T2B6[3];_T2B8=_T2B7.l;_T2B9=_T2B8.first_line;_T2BA=Cyc_Position_loc_to_seg(_T2B9);_T2BB=_tag_fat("expecting `open'",sizeof(char),17U);_T2BC=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T2BA,_T2BB,_T2BC);goto _TL296;_TL295: _TL296: _T2BD=yyyvsp;_T2BE=& _T2BD[3].v;_T2BF=(union Cyc_YYSTYPE*)_T2BE;_T2C0=
Cyc_yyget_Exp_tok(_T2BF);yyval=Cyc_YY61(_T2C0);goto _LL0;case 59: _T2C1=yyyvsp;_T2C2=_T2C1[0];
# 1577
yyval=_T2C2.v;goto _LL0;case 60: _T2C3=yyyvsp;_T2C4=& _T2C3[0].v;_T2C5=(union Cyc_YYSTYPE*)_T2C4;_T2C6=
# 1578 "parse.y"
Cyc_yyget_YY16(_T2C5);_T2C7=yyyvsp;_T2C8=& _T2C7[1].v;_T2C9=(union Cyc_YYSTYPE*)_T2C8;_T2CA=Cyc_yyget_YY16(_T2C9);_T2CB=Cyc_List_imp_append(_T2C6,_T2CA);yyval=Cyc_YY16(_T2CB);goto _LL0;case 61:{struct Cyc_Parse_Declaration_spec _T1877;_T2CD=yyyvsp;_T2CE=& _T2CD[0].v;_T2CF=(union Cyc_YYSTYPE*)_T2CE;
# 1584 "parse.y"
_T1877.sc=Cyc_yyget_YY20(_T2CF);_T2D0=yyyvsp;_T2D1=_T2D0[0];_T2D2=_T2D1.l;_T2D3=_T2D2.first_line;_T2D4=Cyc_Position_loc_to_seg(_T2D3);_T1877.tq=Cyc_Absyn_empty_tqual(_T2D4);
_T1877.type_specs=Cyc_Parse_empty_spec(0U);_T1877.is_inline=0;_T1877.attributes=0;_T2CC=_T1877;}
# 1584
yyval=Cyc_YY17(_T2CC);goto _LL0;case 62: _T2D5=yyyvsp;_T2D6=& _T2D5[1].v;_T2D7=(union Cyc_YYSTYPE*)_T2D6;{
# 1587 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T2D7);_T2D8=two;_T2D9=_T2D8.sc;_T2DA=(int)_T2D9;
if(_T2DA==7)goto _TL297;_T2DB=yyyvsp;_T2DC=_T2DB[0];_T2DD=_T2DC.l;_T2DE=_T2DD.first_line;_T2DF=
Cyc_Position_loc_to_seg(_T2DE);_T2E0=
_tag_fat("Only one storage class is allowed in a declaration (missing ';' or ','?)",sizeof(char),73U);_T2E1=_tag_fat(0U,sizeof(void*),0);
# 1589
Cyc_Warn_warn(_T2DF,_T2E0,_T2E1);goto _TL298;_TL297: _TL298:{struct Cyc_Parse_Declaration_spec _T1877;_T2E3=yyyvsp;_T2E4=& _T2E3[0].v;_T2E5=(union Cyc_YYSTYPE*)_T2E4;
# 1591
_T1877.sc=Cyc_yyget_YY20(_T2E5);_T2E6=two;_T1877.tq=_T2E6.tq;_T2E7=two;_T1877.type_specs=_T2E7.type_specs;_T2E8=two;
_T1877.is_inline=_T2E8.is_inline;_T2E9=two;_T1877.attributes=_T2E9.attributes;_T2E2=_T1877;}
# 1591
yyval=Cyc_YY17(_T2E2);goto _LL0;}case 63: _T2EA=yyyvsp;_T2EB=_T2EA[0];_T2EC=_T2EB.l;_T2ED=_T2EC.first_line;_T2EE=
# 1595 "parse.y"
Cyc_Position_loc_to_seg(_T2ED);_T2EF=_tag_fat("__extension__ keyword ignored in declaration",sizeof(char),45U);_T2F0=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T2EE,_T2EF,_T2F0);_T2F1=yyyvsp;_T2F2=_T2F1[1];
yyval=_T2F2.v;goto _LL0;case 64:{struct Cyc_Parse_Declaration_spec _T1877;
# 1599 "parse.y"
_T1877.sc=7U;_T2F4=yyyvsp;_T2F5=_T2F4[0];_T2F6=_T2F5.l;_T2F7=_T2F6.first_line;_T2F8=Cyc_Position_loc_to_seg(_T2F7);_T1877.tq=Cyc_Absyn_empty_tqual(_T2F8);_T2F9=yyyvsp;_T2FA=& _T2F9[0].v;_T2FB=(union Cyc_YYSTYPE*)_T2FA;
_T1877.type_specs=Cyc_yyget_YY21(_T2FB);_T1877.is_inline=0;_T1877.attributes=0;_T2F3=_T1877;}
# 1599
yyval=Cyc_YY17(_T2F3);goto _LL0;case 65: _T2FC=yyyvsp;_T2FD=& _T2FC[1].v;_T2FE=(union Cyc_YYSTYPE*)_T2FD;{
# 1602 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T2FE);{struct Cyc_Parse_Declaration_spec _T1877;_T300=two;
_T1877.sc=_T300.sc;_T301=two;_T1877.tq=_T301.tq;_T302=yyyvsp;_T303=_T302[0];_T304=_T303.l;_T305=_T304.first_line;_T306=
Cyc_Position_loc_to_seg(_T305);_T307=two;_T308=_T307.type_specs;_T309=yyyvsp;_T30A=& _T309[0].v;_T30B=(union Cyc_YYSTYPE*)_T30A;_T30C=Cyc_yyget_YY21(_T30B);_T1877.type_specs=Cyc_Parse_combine_specifiers(_T306,_T308,_T30C);_T30D=two;
_T1877.is_inline=_T30D.is_inline;_T30E=two;_T1877.attributes=_T30E.attributes;_T2FF=_T1877;}
# 1603
yyval=Cyc_YY17(_T2FF);goto _LL0;}case 66:{struct Cyc_Parse_Declaration_spec _T1877;
# 1608 "parse.y"
_T1877.sc=7U;_T310=yyyvsp;_T311=& _T310[0].v;_T312=(union Cyc_YYSTYPE*)_T311;_T1877.tq=Cyc_yyget_YY24(_T312);_T1877.type_specs=Cyc_Parse_empty_spec(0U);_T1877.is_inline=0;_T1877.attributes=0;_T30F=_T1877;}yyval=Cyc_YY17(_T30F);goto _LL0;case 67: _T313=yyyvsp;_T314=& _T313[1].v;_T315=(union Cyc_YYSTYPE*)_T314;{
# 1610 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T315);{struct Cyc_Parse_Declaration_spec _T1877;_T317=two;
_T1877.sc=_T317.sc;_T318=yyyvsp;_T319=& _T318[0].v;_T31A=(union Cyc_YYSTYPE*)_T319;_T31B=Cyc_yyget_YY24(_T31A);_T31C=two;_T31D=_T31C.tq;_T1877.tq=Cyc_Absyn_combine_tqual(_T31B,_T31D);_T31E=two;
_T1877.type_specs=_T31E.type_specs;_T31F=two;_T1877.is_inline=_T31F.is_inline;_T320=two;_T1877.attributes=_T320.attributes;_T316=_T1877;}
# 1611
yyval=Cyc_YY17(_T316);goto _LL0;}case 68:{struct Cyc_Parse_Declaration_spec _T1877;
# 1615 "parse.y"
_T1877.sc=7U;_T322=yyyvsp;_T323=_T322[0];_T324=_T323.l;_T325=_T324.first_line;_T326=Cyc_Position_loc_to_seg(_T325);_T1877.tq=Cyc_Absyn_empty_tqual(_T326);
_T1877.type_specs=Cyc_Parse_empty_spec(0U);_T1877.is_inline=1;_T1877.attributes=0;_T321=_T1877;}
# 1615
yyval=Cyc_YY17(_T321);goto _LL0;case 69: _T327=yyyvsp;_T328=& _T327[1].v;_T329=(union Cyc_YYSTYPE*)_T328;{
# 1618 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T329);{struct Cyc_Parse_Declaration_spec _T1877;_T32B=two;
_T1877.sc=_T32B.sc;_T32C=two;_T1877.tq=_T32C.tq;_T32D=two;_T1877.type_specs=_T32D.type_specs;_T1877.is_inline=1;_T32E=two;_T1877.attributes=_T32E.attributes;_T32A=_T1877;}yyval=Cyc_YY17(_T32A);goto _LL0;}case 70:{struct Cyc_Parse_Declaration_spec _T1877;
# 1622 "parse.y"
_T1877.sc=7U;_T330=yyyvsp;_T331=_T330[0];_T332=_T331.l;_T333=_T332.first_line;_T334=Cyc_Position_loc_to_seg(_T333);_T1877.tq=Cyc_Absyn_empty_tqual(_T334);
_T1877.type_specs=Cyc_Parse_empty_spec(0U);_T1877.is_inline=0;_T335=yyyvsp;_T336=& _T335[0].v;_T337=(union Cyc_YYSTYPE*)_T336;_T1877.attributes=Cyc_yyget_YY46(_T337);_T32F=_T1877;}
# 1622
yyval=Cyc_YY17(_T32F);goto _LL0;case 71: _T338=yyyvsp;_T339=& _T338[1].v;_T33A=(union Cyc_YYSTYPE*)_T339;{
# 1625 "parse.y"
struct Cyc_Parse_Declaration_spec two=Cyc_yyget_YY17(_T33A);{struct Cyc_Parse_Declaration_spec _T1877;_T33C=two;
_T1877.sc=_T33C.sc;_T33D=two;_T1877.tq=_T33D.tq;_T33E=two;
_T1877.type_specs=_T33E.type_specs;_T33F=two;_T1877.is_inline=_T33F.is_inline;_T340=yyyvsp;_T341=& _T340[0].v;_T342=(union Cyc_YYSTYPE*)_T341;_T343=
Cyc_yyget_YY46(_T342);_T344=two;_T345=_T344.attributes;_T1877.attributes=Cyc_List_imp_append(_T343,_T345);_T33B=_T1877;}
# 1626
yyval=Cyc_YY17(_T33B);goto _LL0;}case 72:
# 1632 "parse.y"
 yyval=Cyc_YY20(4U);goto _LL0;case 73:
# 1633 "parse.y"
 yyval=Cyc_YY20(5U);goto _LL0;case 74:
# 1634 "parse.y"
 yyval=Cyc_YY20(3U);goto _LL0;case 75:
# 1635 "parse.y"
 yyval=Cyc_YY20(1U);goto _LL0;case 76: _T346=yyyvsp;_T347=& _T346[1].v;_T348=(union Cyc_YYSTYPE*)_T347;_T349=
# 1637 "parse.y"
Cyc_yyget_String_tok(_T348);_T34A=_tag_fat("C",sizeof(char),2U);_T34B=Cyc_strcmp(_T349,_T34A);if(_T34B==0)goto _TL299;_T34C=yyyvsp;_T34D=_T34C[0];_T34E=_T34D.l;_T34F=_T34E.first_line;_T350=
Cyc_Position_loc_to_seg(_T34F);_T351=_tag_fat("only extern or extern \"C\" is allowed",sizeof(char),37U);_T352=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T350,_T351,_T352);goto _TL29A;_TL299: _TL29A:
 yyval=Cyc_YY20(2U);goto _LL0;case 77:
# 1641 "parse.y"
 yyval=Cyc_YY20(0U);goto _LL0;case 78:
# 1643 "parse.y"
 yyval=Cyc_YY20(6U);goto _LL0;case 79:
# 1648 "parse.y"
 yyval=Cyc_YY46(0);goto _LL0;case 80: _T353=yyyvsp;_T354=_T353[0];
# 1649 "parse.y"
yyval=_T354.v;goto _LL0;case 81: _T355=yyyvsp;_T356=_T355[3];
# 1653 "parse.y"
yyval=_T356.v;goto _LL0;case 82:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T358=yyyvsp;_T359=& _T358[0].v;_T35A=(union Cyc_YYSTYPE*)_T359;
# 1656
_T1877->hd=Cyc_yyget_YY47(_T35A);_T1877->tl=0;_T357=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY46(_T357);goto _LL0;case 83:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T35C=yyyvsp;_T35D=& _T35C[0].v;_T35E=(union Cyc_YYSTYPE*)_T35D;
# 1657 "parse.y"
_T1877->hd=Cyc_yyget_YY47(_T35E);_T35F=yyyvsp;_T360=& _T35F[2].v;_T361=(union Cyc_YYSTYPE*)_T360;_T1877->tl=Cyc_yyget_YY46(_T361);_T35B=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY46(_T35B);goto _LL0;case 84: _T362=yyyvsp;_T363=_T362[0];_T364=_T363.l;_T365=_T364.first_line;_T366=
# 1660
Cyc_Position_loc_to_seg(_T365);_T367=yyyvsp;_T368=& _T367[0].v;_T369=(union Cyc_YYSTYPE*)_T368;_T36A=Cyc_yyget_String_tok(_T369);_T36B=Cyc_Atts_parse_nullary_att(_T366,_T36A);yyval=Cyc_YY47(_T36B);goto _LL0;case 85: _T36C=& Cyc_Atts_Const_att_val;_T36D=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_T36C;_T36E=(void*)_T36D;
# 1661 "parse.y"
yyval=Cyc_YY47(_T36E);goto _LL0;case 86: _T36F=yyyvsp;_T370=_T36F[0];_T371=_T370.l;_T372=_T371.first_line;_T373=
# 1663 "parse.y"
Cyc_Position_loc_to_seg(_T372);_T374=yyyvsp;_T375=& _T374[0].v;_T376=(union Cyc_YYSTYPE*)_T375;_T377=Cyc_yyget_String_tok(_T376);_T378=yyyvsp;_T379=_T378[2];_T37A=_T379.l;_T37B=_T37A.first_line;_T37C=Cyc_Position_loc_to_seg(_T37B);_T37D=yyyvsp;_T37E=& _T37D[2].v;_T37F=(union Cyc_YYSTYPE*)_T37E;_T380=Cyc_yyget_Exp_tok(_T37F);_T381=Cyc_Atts_parse_unary_att(_T373,_T377,_T37C,_T380);yyval=Cyc_YY47(_T381);goto _LL0;case 87: _T382=yyyvsp;_T383=_T382[0];_T384=_T383.l;_T385=_T384.first_line;_T386=
# 1665 "parse.y"
Cyc_Position_loc_to_seg(_T385);_T387=yyyvsp;_T388=_T387[2];_T389=_T388.l;_T38A=_T389.first_line;_T38B=Cyc_Position_loc_to_seg(_T38A);_T38C=yyyvsp;_T38D=& _T38C[0].v;_T38E=(union Cyc_YYSTYPE*)_T38D;_T38F=Cyc_yyget_String_tok(_T38E);_T390=yyyvsp;_T391=& _T390[2].v;_T392=(union Cyc_YYSTYPE*)_T391;_T393=Cyc_yyget_String_tok(_T392);_T394=yyyvsp;_T395=_T394[4];_T396=_T395.l;_T397=_T396.first_line;_T398=
Cyc_Position_loc_to_seg(_T397);_T399=yyyvsp;_T39A=& _T399[4].v;_T39B=(union Cyc_YYSTYPE*)_T39A;_T39C=Cyc_yyget_Int_tok(_T39B);_T39D=Cyc_Parse_cnst2uint(_T398,_T39C);_T39E=yyyvsp;_T39F=_T39E[6];_T3A0=_T39F.l;_T3A1=_T3A0.first_line;_T3A2=
Cyc_Position_loc_to_seg(_T3A1);_T3A3=yyyvsp;_T3A4=& _T3A3[6].v;_T3A5=(union Cyc_YYSTYPE*)_T3A4;_T3A6=Cyc_yyget_Int_tok(_T3A5);_T3A7=Cyc_Parse_cnst2uint(_T3A2,_T3A6);_T3A8=
# 1665
Cyc_Atts_parse_format_att(_T386,_T38B,_T38F,_T393,_T39D,_T3A7);yyval=Cyc_YY47(_T3A8);goto _LL0;case 88: _T3A9=yyyvsp;_T3AA=_T3A9[0];
# 1677 "parse.y"
yyval=_T3AA.v;goto _LL0;case 89: _T3AB=yyyvsp;_T3AC=& _T3AB[0].v;_T3AD=(union Cyc_YYSTYPE*)_T3AC;_T3AE=
# 1679 "parse.y"
Cyc_yyget_QualId_tok(_T3AD);_T3AF=yyyvsp;_T3B0=& _T3AF[1].v;_T3B1=(union Cyc_YYSTYPE*)_T3B0;_T3B2=Cyc_yyget_YY41(_T3B1);_T3B3=Cyc_Absyn_typedef_type(_T3AE,_T3B2,0,0);_T3B4=yyyvsp;_T3B5=_T3B4[0];_T3B6=_T3B5.l;_T3B7=_T3B6.first_line;_T3B8=Cyc_Position_loc_to_seg(_T3B7);_T3B9=Cyc_Parse_type_spec(_T3B3,_T3B8);yyval=Cyc_YY21(_T3B9);goto _LL0;case 90: _T3BA=Cyc_Absyn_void_type;_T3BB=yyyvsp;_T3BC=_T3BB[0];_T3BD=_T3BC.l;_T3BE=_T3BD.first_line;_T3BF=
# 1683 "parse.y"
Cyc_Position_loc_to_seg(_T3BE);_T3C0=Cyc_Parse_type_spec(_T3BA,_T3BF);yyval=Cyc_YY21(_T3C0);goto _LL0;case 91: _T3C1=Cyc_Absyn_char_type;_T3C2=yyyvsp;_T3C3=_T3C2[0];_T3C4=_T3C3.l;_T3C5=_T3C4.first_line;_T3C6=
# 1684 "parse.y"
Cyc_Position_loc_to_seg(_T3C5);_T3C7=Cyc_Parse_type_spec(_T3C1,_T3C6);yyval=Cyc_YY21(_T3C7);goto _LL0;case 92: _T3C8=yyyvsp;_T3C9=_T3C8[0];_T3CA=_T3C9.l;_T3CB=_T3CA.first_line;_T3CC=
# 1685 "parse.y"
Cyc_Position_loc_to_seg(_T3CB);_T3CD=Cyc_Parse_short_spec(_T3CC);yyval=Cyc_YY21(_T3CD);goto _LL0;case 93: _T3CE=Cyc_Absyn_sint_type;_T3CF=yyyvsp;_T3D0=_T3CF[0];_T3D1=_T3D0.l;_T3D2=_T3D1.first_line;_T3D3=
# 1686 "parse.y"
Cyc_Position_loc_to_seg(_T3D2);_T3D4=Cyc_Parse_type_spec(_T3CE,_T3D3);yyval=Cyc_YY21(_T3D4);goto _LL0;case 94: _T3D5=yyyvsp;_T3D6=_T3D5[0];_T3D7=_T3D6.l;_T3D8=_T3D7.first_line;_T3D9=
# 1687 "parse.y"
Cyc_Position_loc_to_seg(_T3D8);_T3DA=Cyc_Parse_long_spec(_T3D9);yyval=Cyc_YY21(_T3DA);goto _LL0;case 95: _T3DB=Cyc_Absyn_float_type;_T3DC=yyyvsp;_T3DD=_T3DC[0];_T3DE=_T3DD.l;_T3DF=_T3DE.first_line;_T3E0=
# 1688 "parse.y"
Cyc_Position_loc_to_seg(_T3DF);_T3E1=Cyc_Parse_type_spec(_T3DB,_T3E0);yyval=Cyc_YY21(_T3E1);goto _LL0;case 96: _T3E2=Cyc_Absyn_double_type;_T3E3=yyyvsp;_T3E4=_T3E3[0];_T3E5=_T3E4.l;_T3E6=_T3E5.first_line;_T3E7=
# 1689 "parse.y"
Cyc_Position_loc_to_seg(_T3E6);_T3E8=Cyc_Parse_type_spec(_T3E2,_T3E7);yyval=Cyc_YY21(_T3E8);goto _LL0;case 97: _T3E9=yyyvsp;_T3EA=_T3E9[0];_T3EB=_T3EA.l;_T3EC=_T3EB.first_line;_T3ED=
# 1690 "parse.y"
Cyc_Position_loc_to_seg(_T3EC);_T3EE=Cyc_Parse_signed_spec(_T3ED);yyval=Cyc_YY21(_T3EE);goto _LL0;case 98: _T3EF=yyyvsp;_T3F0=_T3EF[0];_T3F1=_T3F0.l;_T3F2=_T3F1.first_line;_T3F3=
# 1691 "parse.y"
Cyc_Position_loc_to_seg(_T3F2);_T3F4=Cyc_Parse_unsigned_spec(_T3F3);yyval=Cyc_YY21(_T3F4);goto _LL0;case 99: _T3F5=yyyvsp;_T3F6=_T3F5[0];_T3F7=_T3F6.l;_T3F8=_T3F7.first_line;_T3F9=
# 1692 "parse.y"
Cyc_Position_loc_to_seg(_T3F8);_T3FA=Cyc_Parse_complex_spec(_T3F9);yyval=Cyc_YY21(_T3FA);goto _LL0;case 100: _T3FB=yyyvsp;_T3FC=_T3FB[0];
# 1693 "parse.y"
yyval=_T3FC.v;goto _LL0;case 101: _T3FD=yyyvsp;_T3FE=_T3FD[0];
# 1694 "parse.y"
yyval=_T3FE.v;goto _LL0;case 102: _T3FF=yyyvsp;_T400=& _T3FF[2].v;_T401=(union Cyc_YYSTYPE*)_T400;_T402=
# 1696 "parse.y"
Cyc_yyget_Exp_tok(_T401);_T403=Cyc_Absyn_typeof_type(_T402);_T404=yyyvsp;_T405=_T404[0];_T406=_T405.l;_T407=_T406.first_line;_T408=Cyc_Position_loc_to_seg(_T407);_T409=Cyc_Parse_type_spec(_T403,_T408);yyval=Cyc_YY21(_T409);goto _LL0;case 103: _T40A=
# 1698 "parse.y"
_tag_fat("__builtin_va_list",sizeof(char),18U);_T40B=& Cyc_Kinds_bk;_T40C=(struct Cyc_Absyn_Kind*)_T40B;_T40D=Cyc_Absyn_builtin_type(_T40A,_T40C);_T40E=yyyvsp;_T40F=_T40E[0];_T410=_T40F.l;_T411=_T410.first_line;_T412=Cyc_Position_loc_to_seg(_T411);_T413=Cyc_Parse_type_spec(_T40D,_T412);yyval=Cyc_YY21(_T413);goto _LL0;case 104: _T414=yyyvsp;_T415=_T414[0];
# 1700 "parse.y"
yyval=_T415.v;goto _LL0;case 105: _T416=yyyvsp;_T417=& _T416[0].v;_T418=(union Cyc_YYSTYPE*)_T417;_T419=
# 1702 "parse.y"
Cyc_yyget_YY45(_T418);_T41A=yyyvsp;_T41B=_T41A[0];_T41C=_T41B.l;_T41D=_T41C.first_line;_T41E=Cyc_Position_loc_to_seg(_T41D);_T41F=Cyc_Parse_type_spec(_T419,_T41E);yyval=Cyc_YY21(_T41F);goto _LL0;case 106: _T420=
# 1704 "parse.y"
Cyc_Absyn_new_evar(0,0);_T421=yyyvsp;_T422=_T421[0];_T423=_T422.l;_T424=_T423.first_line;_T425=Cyc_Position_loc_to_seg(_T424);_T426=Cyc_Parse_type_spec(_T420,_T425);yyval=Cyc_YY21(_T426);goto _LL0;case 107: _T427=yyyvsp;_T428=& _T427[2].v;_T429=(union Cyc_YYSTYPE*)_T428;_T42A=
# 1706 "parse.y"
Cyc_yyget_YY44(_T429);_T42B=Cyc_Kinds_kind_to_opt(_T42A);_T42C=Cyc_Absyn_new_evar(_T42B,0);_T42D=yyyvsp;_T42E=_T42D[0];_T42F=_T42E.l;_T430=_T42F.first_line;_T431=Cyc_Position_loc_to_seg(_T430);_T432=Cyc_Parse_type_spec(_T42C,_T431);yyval=Cyc_YY21(_T432);goto _LL0;case 108: _T434=Cyc_List_map_c;{
# 1708 "parse.y"
struct Cyc_List_List*(*_T1877)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T434;_T433=_T1877;}{struct _tuple19*(*_T1877)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T435=_T1877;}_T436=yyyvsp;_T437=_T436[2];_T438=_T437.l;_T439=_T438.first_line;_T43A=Cyc_Position_loc_to_seg(_T439);_T43B=yyyvsp;_T43C=& _T43B[2].v;_T43D=(union Cyc_YYSTYPE*)_T43C;_T43E=Cyc_yyget_YY39(_T43D);_T43F=Cyc_List_imp_rev(_T43E);_T440=_T433(_T435,_T43A,_T43F);_T441=Cyc_Absyn_tuple_type(_T440);_T442=yyyvsp;_T443=_T442[0];_T444=_T443.l;_T445=_T444.first_line;_T446=
Cyc_Position_loc_to_seg(_T445);_T447=
# 1708
Cyc_Parse_type_spec(_T441,_T446);yyval=Cyc_YY21(_T447);goto _LL0;case 109: _T448=yyyvsp;_T449=& _T448[2].v;_T44A=(union Cyc_YYSTYPE*)_T449;_T44B=
# 1711 "parse.y"
Cyc_yyget_YY45(_T44A);_T44C=Cyc_Absyn_rgn_handle_type(_T44B);_T44D=yyyvsp;_T44E=_T44D[0];_T44F=_T44E.l;_T450=_T44F.first_line;_T451=Cyc_Position_loc_to_seg(_T450);_T452=Cyc_Parse_type_spec(_T44C,_T451);yyval=Cyc_YY21(_T452);goto _LL0;case 110: _T453=& Cyc_Kinds_eko;_T454=(struct Cyc_Core_Opt*)_T453;_T455=
# 1713 "parse.y"
Cyc_Absyn_new_evar(_T454,0);_T456=Cyc_Absyn_rgn_handle_type(_T455);_T457=yyyvsp;_T458=_T457[0];_T459=_T458.l;_T45A=_T459.first_line;_T45B=Cyc_Position_loc_to_seg(_T45A);_T45C=Cyc_Parse_type_spec(_T456,_T45B);yyval=Cyc_YY21(_T45C);goto _LL0;case 111: _T45D=yyyvsp;_T45E=& _T45D[2].v;_T45F=(union Cyc_YYSTYPE*)_T45E;_T460=
# 1715 "parse.y"
Cyc_yyget_YY58(_T45F);_T461=Cyc_Absyn_aqual_handle_type(_T460);_T462=yyyvsp;_T463=_T462[0];_T464=_T463.l;_T465=_T464.first_line;_T466=Cyc_Position_loc_to_seg(_T465);_T467=Cyc_Parse_type_spec(_T461,_T466);yyval=Cyc_YY21(_T467);goto _LL0;case 112: _T468=yyyvsp;_T469=& _T468[2].v;_T46A=(union Cyc_YYSTYPE*)_T469;_T46B=
# 1717 "parse.y"
Cyc_yyget_YY45(_T46A);_T46C=Cyc_Absyn_tag_type(_T46B);_T46D=yyyvsp;_T46E=_T46D[0];_T46F=_T46E.l;_T470=_T46F.first_line;_T471=Cyc_Position_loc_to_seg(_T470);_T472=Cyc_Parse_type_spec(_T46C,_T471);yyval=Cyc_YY21(_T472);goto _LL0;case 113: _T473=& Cyc_Kinds_iko;_T474=(struct Cyc_Core_Opt*)_T473;_T475=
# 1719 "parse.y"
Cyc_Absyn_new_evar(_T474,0);_T476=Cyc_Absyn_tag_type(_T475);_T477=yyyvsp;_T478=_T477[0];_T479=_T478.l;_T47A=_T479.first_line;_T47B=Cyc_Position_loc_to_seg(_T47A);_T47C=Cyc_Parse_type_spec(_T476,_T47B);yyval=Cyc_YY21(_T47C);goto _LL0;case 114: _T47D=yyyvsp;_T47E=& _T47D[2].v;_T47F=(union Cyc_YYSTYPE*)_T47E;_T480=
# 1721 "parse.y"
Cyc_yyget_Exp_tok(_T47F);_T481=Cyc_Absyn_valueof_type(_T480);_T482=yyyvsp;_T483=_T482[0];_T484=_T483.l;_T485=_T484.first_line;_T486=Cyc_Position_loc_to_seg(_T485);_T487=Cyc_Parse_type_spec(_T481,_T486);yyval=Cyc_YY21(_T487);goto _LL0;case 115: _T488=yyyvsp;_T489=& _T488[0].v;_T48A=(union Cyc_YYSTYPE*)_T489;_T48B=
# 1726 "parse.y"
Cyc_yyget_String_tok(_T48A);_T48C=yyyvsp;_T48D=_T48C[0];_T48E=_T48D.l;_T48F=_T48E.first_line;_T490=Cyc_Position_loc_to_seg(_T48F);_T491=Cyc_Kinds_id_to_kind(_T48B,_T490);yyval=Cyc_YY44(_T491);goto _LL0;case 116: _T493=Cyc_Flags_porting_c_code;
# 1730 "parse.y"
if(!_T493)goto _TL29B;_T494=yyyvsp;_T495=_T494[0];_T496=_T495.l;_T497=_T496.first_line;_T492=Cyc_Position_loc_to_seg(_T497);goto _TL29C;_TL29B: _T492=0U;_TL29C: {unsigned loc=_T492;{struct Cyc_Absyn_Tqual _T1877;
_T1877.print_const=1;_T1877.q_volatile=0;_T1877.q_restrict=0;_T1877.real_const=1;_T1877.loc=loc;_T498=_T1877;}yyval=Cyc_YY24(_T498);goto _LL0;}case 117:{struct Cyc_Absyn_Tqual _T1877;
# 1732 "parse.y"
_T1877.print_const=0;_T1877.q_volatile=1;_T1877.q_restrict=0;_T1877.real_const=0;_T1877.loc=0U;_T499=_T1877;}yyval=Cyc_YY24(_T499);goto _LL0;case 118:{struct Cyc_Absyn_Tqual _T1877;
# 1733 "parse.y"
_T1877.print_const=0;_T1877.q_volatile=0;_T1877.q_restrict=1;_T1877.real_const=0;_T1877.loc=0U;_T49A=_T1877;}yyval=Cyc_YY24(_T49A);goto _LL0;case 119:  {
# 1739 "parse.y"
struct Cyc_Absyn_TypeDecl*ed;ed=_cycalloc(sizeof(struct Cyc_Absyn_TypeDecl));_T49B=ed;{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct));_T1877->tag=1;{struct Cyc_Absyn_Enumdecl*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl));_T1878->sc=2U;_T49E=yyyvsp;_T49F=& _T49E[1].v;_T4A0=(union Cyc_YYSTYPE*)_T49F;_T1878->name=Cyc_yyget_QualId_tok(_T4A0);{struct Cyc_Core_Opt*_T1879=_cycalloc(sizeof(struct Cyc_Core_Opt));_T4A2=yyyvsp;_T4A3=& _T4A2[3].v;_T4A4=(union Cyc_YYSTYPE*)_T4A3;_T1879->v=Cyc_yyget_YY49(_T4A4);_T4A1=(struct Cyc_Core_Opt*)_T1879;}_T1878->fields=_T4A1;_T49D=(struct Cyc_Absyn_Enumdecl*)_T1878;}_T1877->f1=_T49D;_T49C=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_T1877;}_T49B->r=(void*)_T49C;_T4A5=ed;_T4A6=yyyvsp;_T4A7=_T4A6[0];_T4A8=_T4A7.l;_T4A9=_T4A8.first_line;
_T4A5->loc=Cyc_Position_loc_to_seg(_T4A9);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T1877->tag=10;
_T1877->f1=ed;_T1877->f2=0;_T4AA=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1877;}_T4AB=(void*)_T4AA;_T4AC=yyyvsp;_T4AD=_T4AC[0];_T4AE=_T4AD.l;_T4AF=_T4AE.first_line;_T4B0=Cyc_Position_loc_to_seg(_T4AF);_T4B1=Cyc_Parse_type_spec(_T4AB,_T4B0);yyval=Cyc_YY21(_T4B1);goto _LL0;}case 120: _T4B2=yyyvsp;_T4B3=& _T4B2[1].v;_T4B4=(union Cyc_YYSTYPE*)_T4B3;_T4B5=
# 1744 "parse.y"
Cyc_yyget_QualId_tok(_T4B4);_T4B6=Cyc_Absyn_enum_type(_T4B5,0);_T4B7=yyyvsp;_T4B8=_T4B7[0];_T4B9=_T4B8.l;_T4BA=_T4B9.first_line;_T4BB=Cyc_Position_loc_to_seg(_T4BA);_T4BC=Cyc_Parse_type_spec(_T4B6,_T4BB);yyval=Cyc_YY21(_T4BC);goto _LL0;case 121: _T4BD=yyyvsp;_T4BE=& _T4BD[2].v;_T4BF=(union Cyc_YYSTYPE*)_T4BE;_T4C0=
# 1746 "parse.y"
Cyc_yyget_YY49(_T4BF);_T4C1=Cyc_Absyn_anon_enum_type(_T4C0);_T4C2=yyyvsp;_T4C3=_T4C2[0];_T4C4=_T4C3.l;_T4C5=_T4C4.first_line;_T4C6=Cyc_Position_loc_to_seg(_T4C5);_T4C7=Cyc_Parse_type_spec(_T4C1,_T4C6);yyval=Cyc_YY21(_T4C7);goto _LL0;case 122:{struct Cyc_Absyn_Enumfield*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_T4C9=yyyvsp;_T4CA=& _T4C9[0].v;_T4CB=(union Cyc_YYSTYPE*)_T4CA;
# 1752 "parse.y"
_T1877->name=Cyc_yyget_QualId_tok(_T4CB);_T1877->tag=0;_T4CC=yyyvsp;_T4CD=_T4CC[0];_T4CE=_T4CD.l;_T4CF=_T4CE.first_line;_T1877->loc=Cyc_Position_loc_to_seg(_T4CF);_T4C8=(struct Cyc_Absyn_Enumfield*)_T1877;}yyval=Cyc_YY48(_T4C8);goto _LL0;case 123:{struct Cyc_Absyn_Enumfield*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Enumfield));_T4D1=yyyvsp;_T4D2=& _T4D1[0].v;_T4D3=(union Cyc_YYSTYPE*)_T4D2;
# 1754 "parse.y"
_T1877->name=Cyc_yyget_QualId_tok(_T4D3);_T4D4=yyyvsp;_T4D5=& _T4D4[2].v;_T4D6=(union Cyc_YYSTYPE*)_T4D5;_T1877->tag=Cyc_yyget_Exp_tok(_T4D6);_T4D7=yyyvsp;_T4D8=_T4D7[0];_T4D9=_T4D8.l;_T4DA=_T4D9.first_line;_T1877->loc=Cyc_Position_loc_to_seg(_T4DA);_T4D0=(struct Cyc_Absyn_Enumfield*)_T1877;}yyval=Cyc_YY48(_T4D0);goto _LL0;case 124:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T4DC=yyyvsp;_T4DD=& _T4DC[0].v;_T4DE=(union Cyc_YYSTYPE*)_T4DD;
# 1758 "parse.y"
_T1877->hd=Cyc_yyget_YY48(_T4DE);_T1877->tl=0;_T4DB=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY49(_T4DB);goto _LL0;case 125:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T4E0=yyyvsp;_T4E1=& _T4E0[0].v;_T4E2=(union Cyc_YYSTYPE*)_T4E1;
# 1759 "parse.y"
_T1877->hd=Cyc_yyget_YY48(_T4E2);_T1877->tl=0;_T4DF=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY49(_T4DF);goto _LL0;case 126:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T4E4=yyyvsp;_T4E5=& _T4E4[0].v;_T4E6=(union Cyc_YYSTYPE*)_T4E5;
# 1760 "parse.y"
_T1877->hd=Cyc_yyget_YY48(_T4E6);_T4E7=yyyvsp;_T4E8=& _T4E7[2].v;_T4E9=(union Cyc_YYSTYPE*)_T4E8;_T1877->tl=Cyc_yyget_YY49(_T4E9);_T4E3=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY49(_T4E3);goto _LL0;case 127:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct));_T1877->tag=7;_T4EB=yyyvsp;_T4EC=& _T4EB[0].v;_T4ED=(union Cyc_YYSTYPE*)_T4EC;
# 1766 "parse.y"
_T1877->f1=Cyc_yyget_YY22(_T4ED);_T1877->f2=0;_T4EE=yyyvsp;_T4EF=& _T4EE[2].v;_T4F0=(union Cyc_YYSTYPE*)_T4EF;_T1877->f3=Cyc_yyget_YY25(_T4F0);_T4EA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T1877;}_T4F1=(void*)_T4EA;_T4F2=yyyvsp;_T4F3=_T4F2[0];_T4F4=_T4F3.l;_T4F5=_T4F4.first_line;_T4F6=Cyc_Position_loc_to_seg(_T4F5);_T4F7=Cyc_Parse_type_spec(_T4F1,_T4F6);yyval=Cyc_YY21(_T4F7);goto _LL0;case 128: _T4F9=Cyc_List_map_c;{
# 1770 "parse.y"
struct Cyc_List_List*(*_T1877)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T4F9;_T4F8=_T1877;}_T4FA=yyyvsp;_T4FB=_T4FA[2];_T4FC=_T4FB.l;_T4FD=_T4FC.first_line;_T4FE=Cyc_Position_loc_to_seg(_T4FD);_T4FF=yyyvsp;_T500=& _T4FF[2].v;_T501=(union Cyc_YYSTYPE*)_T500;_T502=Cyc_yyget_YY41(_T501);{struct Cyc_List_List*ts=_T4F8(Cyc_Parse_typ2tvar,_T4FE,_T502);_T504=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1877)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T504;_T503=_T1877;}_T505=yyyvsp;_T506=_T505[4];_T507=_T506.l;_T508=_T507.first_line;_T509=Cyc_Position_loc_to_seg(_T508);_T50A=yyyvsp;_T50B=& _T50A[4].v;_T50C=(union Cyc_YYSTYPE*)_T50B;_T50D=Cyc_yyget_YY41(_T50C);{struct Cyc_List_List*exist_ts=_T503(Cyc_Parse_typ2tvar,_T509,_T50D);_T50E=yyyvsp;_T50F=& _T50E[5].v;_T510=(union Cyc_YYSTYPE*)_T50F;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T510);_T512=ec_qb;_T513=(unsigned)_T512;
if(!_T513)goto _TL29D;_T514=ec_qb;_T511=*_T514;goto _TL29E;_TL29D:{struct _tuple28 _T1877;_T1877.f0=0;_T1877.f1=0;_T515=_T1877;}_T511=_T515;_TL29E: {struct _tuple28 _T1877=_T511;struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_List_List*ec=_T1879;struct Cyc_List_List*qb=_T1878;_T516=yyyvsp;_T517=& _T516[0].v;_T518=(union Cyc_YYSTYPE*)_T517;_T519=
Cyc_yyget_YY23(_T518);_T51A=_T519.f1;_T51B=yyyvsp;_T51C=& _T51B[1].v;_T51D=(union Cyc_YYSTYPE*)_T51C;_T51E=Cyc_yyget_QualId_tok(_T51D);_T51F=ts;_T520=exist_ts;_T521=ec;_T522=qb;_T523=yyyvsp;_T524=& _T523[6].v;_T525=(union Cyc_YYSTYPE*)_T524;_T526=
Cyc_yyget_YY25(_T525);_T527=yyyvsp;_T528=& _T527[0].v;_T529=(union Cyc_YYSTYPE*)_T528;_T52A=Cyc_yyget_YY23(_T529);_T52B=_T52A.f0;_T52C=Cyc_Absyn_aggrdecl_impl(_T520,_T521,_T522,_T526,_T52B);_T52D=yyyvsp;_T52E=_T52D[0];_T52F=_T52E.l;_T530=_T52F.first_line;_T531=
Cyc_Position_loc_to_seg(_T530);{
# 1774
struct Cyc_Absyn_TypeDecl*td=Cyc_Absyn_aggr_tdecl(_T51A,2U,_T51E,_T51F,_T52C,0,_T531);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T187A->tag=10;
# 1777
_T187A->f1=td;_T187A->f2=0;_T532=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T187A;}_T533=(void*)_T532;_T534=yyyvsp;_T535=_T534[0];_T536=_T535.l;_T537=_T536.first_line;_T538=Cyc_Position_loc_to_seg(_T537);_T539=Cyc_Parse_type_spec(_T533,_T538);yyval=Cyc_YY21(_T539);goto _LL0;}}}}}}case 129: _T53A=yyyvsp;_T53B=& _T53A[0].v;_T53C=(union Cyc_YYSTYPE*)_T53B;_T53D=
# 1780 "parse.y"
Cyc_yyget_YY23(_T53C);_T53E=_T53D.f1;_T53F=yyyvsp;_T540=& _T53F[1].v;_T541=(union Cyc_YYSTYPE*)_T540;_T542=Cyc_yyget_QualId_tok(_T541);_T544=yyyvsp;_T545=& _T544[0].v;_T546=(union Cyc_YYSTYPE*)_T545;_T547=Cyc_yyget_YY23(_T546);_T548=_T547.f0;if(!_T548)goto _TL29F;{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));_T1877->v=(void*)1;_T549=(struct Cyc_Core_Opt*)_T1877;}_T543=_T549;goto _TL2A0;_TL29F: _T543=0;_TL2A0: _T54A=Cyc_Absyn_UnknownAggr(_T53E,_T542,_T543);_T54B=yyyvsp;_T54C=& _T54B[2].v;_T54D=(union Cyc_YYSTYPE*)_T54C;_T54E=
Cyc_yyget_YY41(_T54D);_T54F=
# 1780
Cyc_Absyn_aggr_type(_T54A,_T54E);_T550=yyyvsp;_T551=_T550[0];_T552=_T551.l;_T553=_T552.first_line;_T554=
Cyc_Position_loc_to_seg(_T553);_T555=
# 1780
Cyc_Parse_type_spec(_T54F,_T554);yyval=Cyc_YY21(_T555);goto _LL0;case 130:{struct _tuple25 _T1877;
# 1785 "parse.y"
_T1877.f0=1;_T557=yyyvsp;_T558=& _T557[1].v;_T559=(union Cyc_YYSTYPE*)_T558;_T1877.f1=Cyc_yyget_YY22(_T559);_T556=_T1877;}yyval=Cyc_YY23(_T556);goto _LL0;case 131:{struct _tuple25 _T1877;
# 1786 "parse.y"
_T1877.f0=0;_T55B=yyyvsp;_T55C=& _T55B[0].v;_T55D=(union Cyc_YYSTYPE*)_T55C;_T1877.f1=Cyc_yyget_YY22(_T55D);_T55A=_T1877;}yyval=Cyc_YY23(_T55A);goto _LL0;case 132:
# 1789
 yyval=Cyc_YY22(0U);goto _LL0;case 133:
# 1790 "parse.y"
 yyval=Cyc_YY22(1U);goto _LL0;case 134:
# 1793
 yyval=Cyc_YY41(0);goto _LL0;case 135: _T55E=yyyvsp;_T55F=& _T55E[1].v;_T560=(union Cyc_YYSTYPE*)_T55F;_T561=
# 1794 "parse.y"
Cyc_yyget_YY41(_T560);_T562=Cyc_List_imp_rev(_T561);yyval=Cyc_YY41(_T562);goto _LL0;case 136:
# 1799 "parse.y"
 yyval=Cyc_YY25(0);goto _LL0;case 137:  {
# 1801 "parse.y"
struct Cyc_List_List*decls=0;_T563=yyyvsp;_T564=& _T563[0].v;_T565=(union Cyc_YYSTYPE*)_T564;{
struct Cyc_List_List*x=Cyc_yyget_YY26(_T565);_TL2A4: if(x!=0)goto _TL2A2;else{goto _TL2A3;}
_TL2A2: _T566=x;_T567=_T566->hd;_T568=(struct Cyc_List_List*)_T567;_T569=decls;decls=Cyc_List_imp_append(_T568,_T569);_T56A=x;
# 1802
x=_T56A->tl;goto _TL2A4;_TL2A3:;}{
# 1805
struct Cyc_List_List*tags=Cyc_Parse_get_aggrfield_tags(decls);
if(tags==0)goto _TL2A5;_T56C=Cyc_List_iter_c;{
void(*_T1877)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*)=(void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))_T56C;_T56B=_T1877;}_T56D=tags;_T56E=decls;_T56B(Cyc_Parse_substitute_aggrfield_tags,_T56D,_T56E);goto _TL2A6;_TL2A5: _TL2A6:
 yyval=Cyc_YY25(decls);goto _LL0;}}case 138:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T570=yyyvsp;_T571=& _T570[0].v;_T572=(union Cyc_YYSTYPE*)_T571;
# 1814 "parse.y"
_T1877->hd=Cyc_yyget_YY25(_T572);_T1877->tl=0;_T56F=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY26(_T56F);goto _LL0;case 139:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T574=yyyvsp;_T575=& _T574[1].v;_T576=(union Cyc_YYSTYPE*)_T575;
# 1815 "parse.y"
_T1877->hd=Cyc_yyget_YY25(_T576);_T577=yyyvsp;_T578=& _T577[0].v;_T579=(union Cyc_YYSTYPE*)_T578;_T1877->tl=Cyc_yyget_YY26(_T579);_T573=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY26(_T573);goto _LL0;case 140: _T57B=yyr;{struct _tuple11*_T1877=_region_malloc(_T57B,0U,sizeof(struct _tuple11));
# 1821 "parse.y"
_T1877->tl=0;_T57C=yyyvsp;_T57D=& _T57C[0].v;_T57E=(union Cyc_YYSTYPE*)_T57D;_T1877->hd=Cyc_yyget_YY18(_T57E);_T57A=(struct _tuple11*)_T1877;}yyval=Cyc_YY19(_T57A);goto _LL0;case 141: _T580=yyr;{struct _tuple11*_T1877=_region_malloc(_T580,0U,sizeof(struct _tuple11));_T581=yyyvsp;_T582=& _T581[0].v;_T583=(union Cyc_YYSTYPE*)_T582;
# 1823 "parse.y"
_T1877->tl=Cyc_yyget_YY19(_T583);_T584=yyyvsp;_T585=& _T584[2].v;_T586=(union Cyc_YYSTYPE*)_T585;_T1877->hd=Cyc_yyget_YY18(_T586);_T57F=(struct _tuple11*)_T1877;}yyval=Cyc_YY19(_T57F);goto _LL0;case 142:{struct _tuple12 _T1877;_T588=yyyvsp;_T589=& _T588[0].v;_T58A=(union Cyc_YYSTYPE*)_T589;
# 1827 "parse.y"
_T1877.f0=Cyc_yyget_YY28(_T58A);_T1877.f1=0;_T1877.f2=0;_T587=_T1877;}yyval=Cyc_YY18(_T587);goto _LL0;case 143:{struct _tuple12 _T1877;_T58C=yyyvsp;_T58D=& _T58C[0].v;_T58E=(union Cyc_YYSTYPE*)_T58D;
# 1828 "parse.y"
_T1877.f0=Cyc_yyget_YY28(_T58E);_T1877.f1=0;_T58F=yyyvsp;_T590=& _T58F[2].v;_T591=(union Cyc_YYSTYPE*)_T590;_T592=Cyc_yyget_YY63(_T591);_T593=yyyvsp;_T594=_T593[1];_T595=_T594.l;_T596=_T595.first_line;_T597=Cyc_Position_loc_to_seg(_T596);_T1877.f2=Cyc_Absyn_new_exp(_T592,_T597);_T58B=_T1877;}yyval=Cyc_YY18(_T58B);goto _LL0;case 144:{struct _tuple12 _T1877;_T599=yyyvsp;_T59A=& _T599[0].v;_T59B=(union Cyc_YYSTYPE*)_T59A;
# 1829 "parse.y"
_T1877.f0=Cyc_yyget_YY28(_T59B);_T59C=yyyvsp;_T59D=& _T59C[2].v;_T59E=(union Cyc_YYSTYPE*)_T59D;_T1877.f1=Cyc_yyget_Exp_tok(_T59E);_T1877.f2=0;_T598=_T1877;}yyval=Cyc_YY18(_T598);goto _LL0;case 145:{struct _tuple12 _T1877;_T5A0=yyyvsp;_T5A1=& _T5A0[0].v;_T5A2=(union Cyc_YYSTYPE*)_T5A1;
# 1830 "parse.y"
_T1877.f0=Cyc_yyget_YY28(_T5A2);_T5A3=yyyvsp;_T5A4=& _T5A3[4].v;_T5A5=(union Cyc_YYSTYPE*)_T5A4;_T1877.f1=Cyc_yyget_Exp_tok(_T5A5);_T5A6=yyyvsp;_T5A7=& _T5A6[2].v;_T5A8=(union Cyc_YYSTYPE*)_T5A7;_T5A9=Cyc_yyget_YY63(_T5A8);_T5AA=yyyvsp;_T5AB=_T5AA[1];_T5AC=_T5AB.l;_T5AD=_T5AC.first_line;_T5AE=Cyc_Position_loc_to_seg(_T5AD);_T1877.f2=Cyc_Absyn_new_exp(_T5A9,_T5AE);_T59F=_T1877;}yyval=Cyc_YY18(_T59F);goto _LL0;case 146:  {struct _RegionHandle _T1877=_new_region(0U,"temp");struct _RegionHandle*temp=& _T1877;_push_region(temp);_T5AF=yyyvsp;_T5B0=& _T5AF[0].v;_T5B1=(union Cyc_YYSTYPE*)_T5B0;{
# 1836 "parse.y"
struct _tuple26 _T1878=Cyc_yyget_YY36(_T5B1);struct Cyc_List_List*_T1879;struct Cyc_Parse_Type_specifier _T187A;struct Cyc_Absyn_Tqual _T187B;_T187B=_T1878.f0;_T187A=_T1878.f1;_T1879=_T1878.f2;{struct Cyc_Absyn_Tqual tq=_T187B;struct Cyc_Parse_Type_specifier tspecs=_T187A;struct Cyc_List_List*atts=_T1879;_T5B2=tq;_T5B3=_T5B2.loc;
if(_T5B3!=0U)goto _TL2A7;_T5B4=yyyvsp;_T5B5=_T5B4[0];_T5B6=_T5B5.l;_T5B7=_T5B6.first_line;tq.loc=Cyc_Position_loc_to_seg(_T5B7);goto _TL2A8;_TL2A7: _TL2A8: {
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;_T5B8=yyyvsp;_T5B9=& _T5B8[1].v;_T5BA=(union Cyc_YYSTYPE*)_T5B9;{
struct Cyc_List_List*x=Cyc_yyget_YY30(_T5BA);_TL2AC: if(x!=0)goto _TL2AA;else{goto _TL2AB;}
_TL2AA: _T5BB=x;_T5BC=_T5BB->hd;{struct _tuple12*_T187C=(struct _tuple12*)_T5BC;struct Cyc_Absyn_Exp*_T187D;struct Cyc_Absyn_Exp*_T187E;struct Cyc_Parse_Declarator _T187F;{struct _tuple12 _T1880=*_T187C;_T187F=_T1880.f0;_T187E=_T1880.f1;_T187D=_T1880.f2;}{struct Cyc_Parse_Declarator d=_T187F;struct Cyc_Absyn_Exp*wd=_T187E;struct Cyc_Absyn_Exp*wh=_T187D;_T5BE=temp;{struct _tuple13*_T1880=_region_malloc(_T5BE,0U,sizeof(struct _tuple13));
_T1880->tl=decls;_T1880->hd=d;_T5BD=(struct _tuple13*)_T1880;}decls=_T5BD;_T5C0=temp;{struct Cyc_List_List*_T1880=_region_malloc(_T5C0,0U,sizeof(struct Cyc_List_List));_T5C2=temp;{struct _tuple16*_T1881=_region_malloc(_T5C2,0U,sizeof(struct _tuple16));
_T1881->f0=wd;_T1881->f1=wh;_T5C1=(struct _tuple16*)_T1881;}_T1880->hd=_T5C1;_T1880->tl=widths_and_reqs;_T5BF=(struct Cyc_List_List*)_T1880;}widths_and_reqs=_T5BF;}}_T5C3=x;
# 1840
x=_T5C3->tl;goto _TL2AC;_TL2AB:;}_T5C4=tspecs;_T5C5=yyyvsp;_T5C6=_T5C5[0];_T5C7=_T5C6.l;_T5C8=_T5C7.first_line;_T5C9=
# 1845
Cyc_Position_loc_to_seg(_T5C8);{void*t=Cyc_Parse_speclist2typ(_T5C4,_T5C9);_T5CA=temp;_T5CB=temp;_T5CC=
# 1847
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);_T5CD=widths_and_reqs;{
# 1846
struct Cyc_List_List*info=Cyc_List_rzip(_T5CA,_T5CB,_T5CC,_T5CD);_T5CF=Cyc_List_map_c;{
# 1848
struct Cyc_List_List*(*_T187C)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple17*),unsigned,struct Cyc_List_List*))_T5CF;_T5CE=_T187C;}_T5D0=yyyvsp;_T5D1=_T5D0[0];_T5D2=_T5D1.l;_T5D3=_T5D2.first_line;_T5D4=Cyc_Position_loc_to_seg(_T5D3);_T5D5=info;_T5D6=_T5CE(Cyc_Parse_make_aggr_field,_T5D4,_T5D5);yyval=Cyc_YY25(_T5D6);_npop_handler(0);goto _LL0;}}}}}_pop_region();}case 147:{struct _tuple26 _T1877;_T5D8=yyyvsp;_T5D9=_T5D8[0];_T5DA=_T5D9.l;_T5DB=_T5DA.first_line;_T5DC=
# 1857 "parse.y"
Cyc_Position_loc_to_seg(_T5DB);_T1877.f0=Cyc_Absyn_empty_tqual(_T5DC);_T5DD=yyyvsp;_T5DE=& _T5DD[0].v;_T5DF=(union Cyc_YYSTYPE*)_T5DE;_T1877.f1=Cyc_yyget_YY21(_T5DF);_T1877.f2=0;_T5D7=_T1877;}yyval=Cyc_YY36(_T5D7);goto _LL0;case 148: _T5E0=yyyvsp;_T5E1=& _T5E0[1].v;_T5E2=(union Cyc_YYSTYPE*)_T5E1;{
# 1859 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T5E2);{struct _tuple26 _T1877;_T5E4=two;_T1877.f0=_T5E4.f0;_T5E5=yyyvsp;_T5E6=_T5E5[0];_T5E7=_T5E6.l;_T5E8=_T5E7.first_line;_T5E9=Cyc_Position_loc_to_seg(_T5E8);_T5EA=yyyvsp;_T5EB=& _T5EA[0].v;_T5EC=(union Cyc_YYSTYPE*)_T5EB;_T5ED=Cyc_yyget_YY21(_T5EC);_T5EE=two;_T5EF=_T5EE.f1;_T1877.f1=Cyc_Parse_combine_specifiers(_T5E9,_T5ED,_T5EF);_T5F0=two;_T1877.f2=_T5F0.f2;_T5E3=_T1877;}yyval=Cyc_YY36(_T5E3);goto _LL0;}case 149:{struct _tuple26 _T1877;_T5F2=yyyvsp;_T5F3=& _T5F2[0].v;_T5F4=(union Cyc_YYSTYPE*)_T5F3;
# 1861 "parse.y"
_T1877.f0=Cyc_yyget_YY24(_T5F4);_T1877.f1=Cyc_Parse_empty_spec(0U);_T1877.f2=0;_T5F1=_T1877;}yyval=Cyc_YY36(_T5F1);goto _LL0;case 150: _T5F5=yyyvsp;_T5F6=& _T5F5[1].v;_T5F7=(union Cyc_YYSTYPE*)_T5F6;{
# 1863 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T5F7);{struct _tuple26 _T1877;_T5F9=yyyvsp;_T5FA=& _T5F9[0].v;_T5FB=(union Cyc_YYSTYPE*)_T5FA;_T5FC=Cyc_yyget_YY24(_T5FB);_T5FD=two;_T5FE=_T5FD.f0;_T1877.f0=Cyc_Absyn_combine_tqual(_T5FC,_T5FE);_T5FF=two;_T1877.f1=_T5FF.f1;_T600=two;_T1877.f2=_T600.f2;_T5F8=_T1877;}yyval=Cyc_YY36(_T5F8);goto _LL0;}case 151:{struct _tuple26 _T1877;_T602=yyyvsp;_T603=_T602[0];_T604=_T603.l;_T605=_T604.first_line;_T606=
# 1865 "parse.y"
Cyc_Position_loc_to_seg(_T605);_T1877.f0=Cyc_Absyn_empty_tqual(_T606);_T1877.f1=Cyc_Parse_empty_spec(0U);_T607=yyyvsp;_T608=& _T607[0].v;_T609=(union Cyc_YYSTYPE*)_T608;_T1877.f2=Cyc_yyget_YY46(_T609);_T601=_T1877;}yyval=Cyc_YY36(_T601);goto _LL0;case 152: _T60A=yyyvsp;_T60B=& _T60A[1].v;_T60C=(union Cyc_YYSTYPE*)_T60B;{
# 1867 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T60C);{struct _tuple26 _T1877;_T60E=two;_T1877.f0=_T60E.f0;_T60F=two;_T1877.f1=_T60F.f1;_T610=yyyvsp;_T611=& _T610[0].v;_T612=(union Cyc_YYSTYPE*)_T611;_T613=Cyc_yyget_YY46(_T612);_T614=two;_T615=_T614.f2;_T1877.f2=Cyc_List_append(_T613,_T615);_T60D=_T1877;}yyval=Cyc_YY36(_T60D);goto _LL0;}case 153:{struct _tuple26 _T1877;_T617=yyyvsp;_T618=_T617[0];_T619=_T618.l;_T61A=_T619.first_line;_T61B=
# 1873 "parse.y"
Cyc_Position_loc_to_seg(_T61A);_T1877.f0=Cyc_Absyn_empty_tqual(_T61B);_T61C=yyyvsp;_T61D=& _T61C[0].v;_T61E=(union Cyc_YYSTYPE*)_T61D;_T1877.f1=Cyc_yyget_YY21(_T61E);_T1877.f2=0;_T616=_T1877;}yyval=Cyc_YY36(_T616);goto _LL0;case 154: _T61F=yyyvsp;_T620=& _T61F[1].v;_T621=(union Cyc_YYSTYPE*)_T620;{
# 1875 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T621);{struct _tuple26 _T1877;_T623=two;_T1877.f0=_T623.f0;_T624=yyyvsp;_T625=_T624[0];_T626=_T625.l;_T627=_T626.first_line;_T628=Cyc_Position_loc_to_seg(_T627);_T629=yyyvsp;_T62A=& _T629[0].v;_T62B=(union Cyc_YYSTYPE*)_T62A;_T62C=Cyc_yyget_YY21(_T62B);_T62D=two;_T62E=_T62D.f1;_T1877.f1=Cyc_Parse_combine_specifiers(_T628,_T62C,_T62E);_T62F=two;_T1877.f2=_T62F.f2;_T622=_T1877;}yyval=Cyc_YY36(_T622);goto _LL0;}case 155:{struct _tuple26 _T1877;_T631=yyyvsp;_T632=& _T631[0].v;_T633=(union Cyc_YYSTYPE*)_T632;
# 1877 "parse.y"
_T1877.f0=Cyc_yyget_YY24(_T633);_T1877.f1=Cyc_Parse_empty_spec(0U);_T1877.f2=0;_T630=_T1877;}yyval=Cyc_YY36(_T630);goto _LL0;case 156: _T634=yyyvsp;_T635=& _T634[1].v;_T636=(union Cyc_YYSTYPE*)_T635;{
# 1879 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T636);{struct _tuple26 _T1877;_T638=yyyvsp;_T639=& _T638[0].v;_T63A=(union Cyc_YYSTYPE*)_T639;_T63B=Cyc_yyget_YY24(_T63A);_T63C=two;_T63D=_T63C.f0;_T1877.f0=Cyc_Absyn_combine_tqual(_T63B,_T63D);_T63E=two;_T1877.f1=_T63E.f1;_T63F=two;_T1877.f2=_T63F.f2;_T637=_T1877;}yyval=Cyc_YY36(_T637);goto _LL0;}case 157:{struct _tuple26 _T1877;_T641=yyyvsp;_T642=_T641[0];_T643=_T642.l;_T644=_T643.first_line;_T645=
# 1881 "parse.y"
Cyc_Position_loc_to_seg(_T644);_T1877.f0=Cyc_Absyn_empty_tqual(_T645);_T1877.f1=Cyc_Parse_empty_spec(0U);_T646=yyyvsp;_T647=& _T646[0].v;_T648=(union Cyc_YYSTYPE*)_T647;_T1877.f2=Cyc_yyget_YY46(_T648);_T640=_T1877;}yyval=Cyc_YY36(_T640);goto _LL0;case 158: _T649=yyyvsp;_T64A=& _T649[1].v;_T64B=(union Cyc_YYSTYPE*)_T64A;{
# 1883 "parse.y"
struct _tuple26 two=Cyc_yyget_YY36(_T64B);{struct _tuple26 _T1877;_T64D=two;_T1877.f0=_T64D.f0;_T64E=two;_T1877.f1=_T64E.f1;_T64F=yyyvsp;_T650=& _T64F[0].v;_T651=(union Cyc_YYSTYPE*)_T650;_T652=Cyc_yyget_YY46(_T651);_T653=two;_T654=_T653.f2;_T1877.f2=Cyc_List_append(_T652,_T654);_T64C=_T1877;}yyval=Cyc_YY36(_T64C);goto _LL0;}case 159: _T656=yyr;{struct Cyc_List_List*_T1877=_region_malloc(_T656,0U,sizeof(struct Cyc_List_List));_T657=yyyvsp;_T658=& _T657[0].v;_T659=(union Cyc_YYSTYPE*)_T658;
# 1889 "parse.y"
_T1877->hd=Cyc_yyget_YY29(_T659);_T1877->tl=0;_T655=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY30(_T655);goto _LL0;case 160: _T65B=yyr;{struct Cyc_List_List*_T1877=_region_malloc(_T65B,0U,sizeof(struct Cyc_List_List));_T65C=yyyvsp;_T65D=& _T65C[2].v;_T65E=(union Cyc_YYSTYPE*)_T65D;
# 1891 "parse.y"
_T1877->hd=Cyc_yyget_YY29(_T65E);_T65F=yyyvsp;_T660=& _T65F[0].v;_T661=(union Cyc_YYSTYPE*)_T660;_T1877->tl=Cyc_yyget_YY30(_T661);_T65A=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY30(_T65A);goto _LL0;case 161: _T663=yyr;{struct _tuple12*_T1877=_region_malloc(_T663,0U,sizeof(struct _tuple12));_T664=yyyvsp;_T665=& _T664[0].v;_T666=(union Cyc_YYSTYPE*)_T665;
# 1896 "parse.y"
_T1877->f0=Cyc_yyget_YY28(_T666);_T1877->f1=0;_T667=yyyvsp;_T668=& _T667[1].v;_T669=(union Cyc_YYSTYPE*)_T668;_T1877->f2=Cyc_yyget_YY61(_T669);_T662=(struct _tuple12*)_T1877;}yyval=Cyc_YY29(_T662);goto _LL0;case 162: _T66B=yyr;{struct _tuple12*_T1877=_region_malloc(_T66B,0U,sizeof(struct _tuple12));{struct _tuple0*_T1878=_cycalloc(sizeof(struct _tuple0));
# 1900 "parse.y"
_T1878->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));*_T1879=_tag_fat("",sizeof(char),1U);_T66D=(struct _fat_ptr*)_T1879;}_T1878->f1=_T66D;_T66C=(struct _tuple0*)_T1878;}_T1877->f0.id=_T66C;_T1877->f0.varloc=0U;_T1877->f0.tms=0;_T66E=yyyvsp;_T66F=& _T66E[1].v;_T670=(union Cyc_YYSTYPE*)_T66F;_T1877->f1=Cyc_yyget_Exp_tok(_T670);_T1877->f2=0;_T66A=(struct _tuple12*)_T1877;}yyval=Cyc_YY29(_T66A);goto _LL0;case 163: _T672=yyr;{struct _tuple12*_T1877=_region_malloc(_T672,0U,sizeof(struct _tuple12));{struct _tuple0*_T1878=_cycalloc(sizeof(struct _tuple0));
# 1905 "parse.y"
_T1878->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));*_T1879=_tag_fat("",sizeof(char),1U);_T674=(struct _fat_ptr*)_T1879;}_T1878->f1=_T674;_T673=(struct _tuple0*)_T1878;}_T1877->f0.id=_T673;_T1877->f0.varloc=0U;_T1877->f0.tms=0;_T1877->f1=0;_T1877->f2=0;_T671=(struct _tuple12*)_T1877;}yyval=Cyc_YY29(_T671);goto _LL0;case 164: _T676=yyr;{struct _tuple12*_T1877=_region_malloc(_T676,0U,sizeof(struct _tuple12));_T677=yyyvsp;_T678=& _T677[0].v;_T679=(union Cyc_YYSTYPE*)_T678;
# 1908 "parse.y"
_T1877->f0=Cyc_yyget_YY28(_T679);_T67A=yyyvsp;_T67B=& _T67A[2].v;_T67C=(union Cyc_YYSTYPE*)_T67B;_T1877->f1=Cyc_yyget_Exp_tok(_T67C);_T1877->f2=0;_T675=(struct _tuple12*)_T1877;}yyval=Cyc_YY29(_T675);goto _LL0;case 165: _T67D=yyyvsp;_T67E=& _T67D[2].v;_T67F=(union Cyc_YYSTYPE*)_T67E;_T680=
# 1912 "parse.y"
Cyc_yyget_Exp_tok(_T67F);yyval=Cyc_YY61(_T680);goto _LL0;case 166:
# 1913 "parse.y"
 yyval=Cyc_YY61(0);goto _LL0;case 167: _T681=yyyvsp;_T682=& _T681[0].v;_T683=(union Cyc_YYSTYPE*)_T682;{
# 1919 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T683);_T685=Cyc_List_map_c;{
struct Cyc_List_List*(*_T1877)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T685;_T684=_T1877;}_T686=yyyvsp;_T687=_T686[2];_T688=_T687.l;_T689=_T688.first_line;_T68A=Cyc_Position_loc_to_seg(_T689);_T68B=yyyvsp;_T68C=& _T68B[2].v;_T68D=(union Cyc_YYSTYPE*)_T68C;_T68E=Cyc_yyget_YY41(_T68D);{struct Cyc_List_List*ts=_T684(Cyc_Parse_typ2tvar,_T68A,_T68E);_T68F=yyyvsp;_T690=& _T68F[1].v;_T691=(union Cyc_YYSTYPE*)_T690;_T692=
Cyc_yyget_QualId_tok(_T691);_T693=ts;{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));_T695=yyyvsp;_T696=& _T695[4].v;_T697=(union Cyc_YYSTYPE*)_T696;_T1877->v=Cyc_yyget_YY35(_T697);_T694=(struct Cyc_Core_Opt*)_T1877;}_T698=is_extensible;_T699=yyyvsp;_T69A=_T699[0];_T69B=_T69A.l;_T69C=_T69B.first_line;_T69D=
Cyc_Position_loc_to_seg(_T69C);{
# 1921
struct Cyc_Absyn_TypeDecl*dd=Cyc_Absyn_datatype_tdecl(2U,_T692,_T693,_T694,_T698,_T69D);{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct));_T1877->tag=10;
# 1923
_T1877->f1=dd;_T1877->f2=0;_T69E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_T1877;}_T69F=(void*)_T69E;_T6A0=yyyvsp;_T6A1=_T6A0[0];_T6A2=_T6A1.l;_T6A3=_T6A2.first_line;_T6A4=Cyc_Position_loc_to_seg(_T6A3);_T6A5=Cyc_Parse_type_spec(_T69F,_T6A4);yyval=Cyc_YY21(_T6A5);goto _LL0;}}}case 168: _T6A6=yyyvsp;_T6A7=& _T6A6[0].v;_T6A8=(union Cyc_YYSTYPE*)_T6A7;{
# 1926 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6A8);{struct Cyc_Absyn_UnknownDatatypeInfo _T1877;_T6AA=yyyvsp;_T6AB=& _T6AA[1].v;_T6AC=(union Cyc_YYSTYPE*)_T6AB;
_T1877.name=Cyc_yyget_QualId_tok(_T6AC);_T1877.is_extensible=is_extensible;_T6A9=_T1877;}_T6AD=Cyc_Absyn_UnknownDatatype(_T6A9);_T6AE=yyyvsp;_T6AF=& _T6AE[2].v;_T6B0=(union Cyc_YYSTYPE*)_T6AF;_T6B1=Cyc_yyget_YY41(_T6B0);_T6B2=Cyc_Absyn_datatype_type(_T6AD,_T6B1);_T6B3=yyyvsp;_T6B4=_T6B3[0];_T6B5=_T6B4.l;_T6B6=_T6B5.first_line;_T6B7=Cyc_Position_loc_to_seg(_T6B6);_T6B8=Cyc_Parse_type_spec(_T6B2,_T6B7);yyval=Cyc_YY21(_T6B8);goto _LL0;}case 169: _T6B9=yyyvsp;_T6BA=& _T6B9[0].v;_T6BB=(union Cyc_YYSTYPE*)_T6BA;{
# 1930 "parse.y"
int is_extensible=Cyc_yyget_YY32(_T6BB);{struct Cyc_Absyn_UnknownDatatypeFieldInfo _T1877;_T6BD=yyyvsp;_T6BE=& _T6BD[1].v;_T6BF=(union Cyc_YYSTYPE*)_T6BE;
_T1877.datatype_name=Cyc_yyget_QualId_tok(_T6BF);_T6C0=yyyvsp;_T6C1=& _T6C0[3].v;_T6C2=(union Cyc_YYSTYPE*)_T6C1;_T1877.field_name=Cyc_yyget_QualId_tok(_T6C2);_T1877.is_extensible=is_extensible;_T6BC=_T1877;}_T6C3=Cyc_Absyn_UnknownDatatypefield(_T6BC);_T6C4=yyyvsp;_T6C5=& _T6C4[4].v;_T6C6=(union Cyc_YYSTYPE*)_T6C5;_T6C7=Cyc_yyget_YY41(_T6C6);_T6C8=Cyc_Absyn_datatype_field_type(_T6C3,_T6C7);_T6C9=yyyvsp;_T6CA=_T6C9[0];_T6CB=_T6CA.l;_T6CC=_T6CB.first_line;_T6CD=Cyc_Position_loc_to_seg(_T6CC);_T6CE=Cyc_Parse_type_spec(_T6C8,_T6CD);yyval=Cyc_YY21(_T6CE);goto _LL0;}case 170:
# 1936 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 171:
# 1937 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 172:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T6D0=yyyvsp;_T6D1=& _T6D0[0].v;_T6D2=(union Cyc_YYSTYPE*)_T6D1;
# 1941 "parse.y"
_T1877->hd=Cyc_yyget_YY34(_T6D2);_T1877->tl=0;_T6CF=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY35(_T6CF);goto _LL0;case 173:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T6D4=yyyvsp;_T6D5=& _T6D4[0].v;_T6D6=(union Cyc_YYSTYPE*)_T6D5;
# 1942 "parse.y"
_T1877->hd=Cyc_yyget_YY34(_T6D6);_T1877->tl=0;_T6D3=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY35(_T6D3);goto _LL0;case 174:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T6D8=yyyvsp;_T6D9=& _T6D8[0].v;_T6DA=(union Cyc_YYSTYPE*)_T6D9;
# 1943 "parse.y"
_T1877->hd=Cyc_yyget_YY34(_T6DA);_T6DB=yyyvsp;_T6DC=& _T6DB[2].v;_T6DD=(union Cyc_YYSTYPE*)_T6DC;_T1877->tl=Cyc_yyget_YY35(_T6DD);_T6D7=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY35(_T6D7);goto _LL0;case 175:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T6DF=yyyvsp;_T6E0=& _T6DF[0].v;_T6E1=(union Cyc_YYSTYPE*)_T6E0;
# 1944 "parse.y"
_T1877->hd=Cyc_yyget_YY34(_T6E1);_T6E2=yyyvsp;_T6E3=& _T6E2[2].v;_T6E4=(union Cyc_YYSTYPE*)_T6E3;_T1877->tl=Cyc_yyget_YY35(_T6E4);_T6DE=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY35(_T6DE);goto _LL0;case 176:
# 1948 "parse.y"
 yyval=Cyc_YY33(2U);goto _LL0;case 177:
# 1949 "parse.y"
 yyval=Cyc_YY33(3U);goto _LL0;case 178:
# 1950 "parse.y"
 yyval=Cyc_YY33(0U);goto _LL0;case 179:{struct Cyc_Absyn_Datatypefield*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T6E6=yyyvsp;_T6E7=& _T6E6[1].v;_T6E8=(union Cyc_YYSTYPE*)_T6E7;
# 1954 "parse.y"
_T1877->name=Cyc_yyget_QualId_tok(_T6E8);_T1877->typs=0;_T6E9=yyyvsp;_T6EA=_T6E9[0];_T6EB=_T6EA.l;_T6EC=_T6EB.first_line;_T1877->loc=Cyc_Position_loc_to_seg(_T6EC);_T6ED=yyyvsp;_T6EE=& _T6ED[0].v;_T6EF=(union Cyc_YYSTYPE*)_T6EE;_T1877->sc=Cyc_yyget_YY33(_T6EF);_T6E5=(struct Cyc_Absyn_Datatypefield*)_T1877;}yyval=Cyc_YY34(_T6E5);goto _LL0;case 180: _T6F1=Cyc_List_map_c;{
# 1956 "parse.y"
struct Cyc_List_List*(*_T1877)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple19*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))_T6F1;_T6F0=_T1877;}{struct _tuple19*(*_T1877)(unsigned,struct _tuple8*)=(struct _tuple19*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ;_T6F2=_T1877;}_T6F3=yyyvsp;_T6F4=_T6F3[3];_T6F5=_T6F4.l;_T6F6=_T6F5.first_line;_T6F7=Cyc_Position_loc_to_seg(_T6F6);_T6F8=yyyvsp;_T6F9=& _T6F8[3].v;_T6FA=(union Cyc_YYSTYPE*)_T6F9;_T6FB=Cyc_yyget_YY39(_T6FA);_T6FC=Cyc_List_imp_rev(_T6FB);{struct Cyc_List_List*typs=_T6F0(_T6F2,_T6F7,_T6FC);{struct Cyc_Absyn_Datatypefield*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Datatypefield));_T6FE=yyyvsp;_T6FF=& _T6FE[1].v;_T700=(union Cyc_YYSTYPE*)_T6FF;
_T1877->name=Cyc_yyget_QualId_tok(_T700);_T1877->typs=typs;_T701=yyyvsp;_T702=_T701[0];_T703=_T702.l;_T704=_T703.first_line;_T1877->loc=Cyc_Position_loc_to_seg(_T704);_T705=yyyvsp;_T706=& _T705[0].v;_T707=(union Cyc_YYSTYPE*)_T706;_T1877->sc=Cyc_yyget_YY33(_T707);_T6FD=(struct Cyc_Absyn_Datatypefield*)_T1877;}yyval=Cyc_YY34(_T6FD);goto _LL0;}case 181: _T708=yyyvsp;_T709=_T708[0];
# 1962 "parse.y"
yyval=_T709.v;goto _LL0;case 182: _T70A=yyyvsp;_T70B=& _T70A[1].v;_T70C=(union Cyc_YYSTYPE*)_T70B;{
# 1964 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T70C);{struct Cyc_Parse_Declarator _T1877;_T70E=two;
_T1877.id=_T70E.id;_T70F=two;_T1877.varloc=_T70F.varloc;_T710=yyyvsp;_T711=& _T710[0].v;_T712=(union Cyc_YYSTYPE*)_T711;_T713=Cyc_yyget_YY27(_T712);_T714=two;_T715=_T714.tms;_T1877.tms=Cyc_List_imp_append(_T713,_T715);_T70D=_T1877;}yyval=Cyc_YY28(_T70D);goto _LL0;}case 183: _T716=yyyvsp;_T717=_T716[0];
# 1971 "parse.y"
yyval=_T717.v;goto _LL0;case 184: _T718=yyyvsp;_T719=& _T718[1].v;_T71A=(union Cyc_YYSTYPE*)_T719;{
# 1973 "parse.y"
struct Cyc_Parse_Declarator two=Cyc_yyget_YY28(_T71A);{struct Cyc_Parse_Declarator _T1877;_T71C=two;
_T1877.id=_T71C.id;_T71D=two;_T1877.varloc=_T71D.varloc;_T71E=yyyvsp;_T71F=& _T71E[0].v;_T720=(union Cyc_YYSTYPE*)_T71F;_T721=Cyc_yyget_YY27(_T720);_T722=two;_T723=_T722.tms;_T1877.tms=Cyc_List_imp_append(_T721,_T723);_T71B=_T1877;}yyval=Cyc_YY28(_T71B);goto _LL0;}case 185:{struct Cyc_Parse_Declarator _T1877;_T725=yyyvsp;_T726=& _T725[0].v;_T727=(union Cyc_YYSTYPE*)_T726;
# 1978 "parse.y"
_T1877.id=Cyc_yyget_QualId_tok(_T727);_T728=yyyvsp;_T729=_T728[0];_T72A=_T729.l;_T72B=_T72A.first_line;_T1877.varloc=Cyc_Position_loc_to_seg(_T72B);_T1877.tms=0;_T724=_T1877;}yyval=Cyc_YY28(_T724);goto _LL0;case 186: _T72C=yyyvsp;_T72D=_T72C[1];
# 1979 "parse.y"
yyval=_T72D.v;goto _LL0;case 187: _T72E=yyyvsp;_T72F=& _T72E[2].v;_T730=(union Cyc_YYSTYPE*)_T72F;{
# 1983 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T730);_T732=yyr;{struct Cyc_List_List*_T1877=_region_malloc(_T732,0U,sizeof(struct Cyc_List_List));_T734=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1878=_region_malloc(_T734,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1878->tag=5;_T735=yyyvsp;_T736=_T735[1];_T737=_T736.l;_T738=_T737.first_line;
_T1878->f1=Cyc_Position_loc_to_seg(_T738);_T739=yyyvsp;_T73A=& _T739[1].v;_T73B=(union Cyc_YYSTYPE*)_T73A;_T1878->f2=Cyc_yyget_YY46(_T73B);_T733=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1878;}_T1877->hd=(void*)_T733;_T73C=d;_T1877->tl=_T73C.tms;_T731=(struct Cyc_List_List*)_T1877;}d.tms=_T731;_T73D=yyyvsp;_T73E=_T73D[2];
yyval=_T73E.v;goto _LL0;}case 188:{struct Cyc_Parse_Declarator _T1877;_T740=yyyvsp;_T741=& _T740[0].v;_T742=(union Cyc_YYSTYPE*)_T741;_T743=
# 1988 "parse.y"
Cyc_yyget_YY28(_T742);_T1877.id=_T743.id;_T744=yyyvsp;_T745=& _T744[0].v;_T746=(union Cyc_YYSTYPE*)_T745;_T747=Cyc_yyget_YY28(_T746);_T1877.varloc=_T747.varloc;_T749=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T749,0U,sizeof(struct Cyc_List_List));_T74B=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T74B,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1879->tag=0;_T74C=yyyvsp;_T74D=& _T74C[3].v;_T74E=(union Cyc_YYSTYPE*)_T74D;_T1879->f1=Cyc_yyget_YY54(_T74E);_T74F=yyyvsp;_T750=_T74F[3];_T751=_T750.l;_T752=_T751.first_line;_T1879->f2=Cyc_Position_loc_to_seg(_T752);_T74A=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T74A;_T753=yyyvsp;_T754=& _T753[0].v;_T755=(union Cyc_YYSTYPE*)_T754;_T756=Cyc_yyget_YY28(_T755);_T1878->tl=_T756.tms;_T748=(struct Cyc_List_List*)_T1878;}_T1877.tms=_T748;_T73F=_T1877;}yyval=Cyc_YY28(_T73F);goto _LL0;case 189:{struct Cyc_Parse_Declarator _T1877;_T758=yyyvsp;_T759=& _T758[0].v;_T75A=(union Cyc_YYSTYPE*)_T759;_T75B=
# 1990 "parse.y"
Cyc_yyget_YY28(_T75A);_T1877.id=_T75B.id;_T75C=yyyvsp;_T75D=& _T75C[0].v;_T75E=(union Cyc_YYSTYPE*)_T75D;_T75F=Cyc_yyget_YY28(_T75E);_T1877.varloc=_T75F.varloc;_T761=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T761,0U,sizeof(struct Cyc_List_List));_T763=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T763,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1879->tag=1;_T764=yyyvsp;_T765=& _T764[2].v;_T766=(union Cyc_YYSTYPE*)_T765;
_T1879->f1=Cyc_yyget_Exp_tok(_T766);_T767=yyyvsp;_T768=& _T767[4].v;_T769=(union Cyc_YYSTYPE*)_T768;_T1879->f2=Cyc_yyget_YY54(_T769);_T76A=yyyvsp;_T76B=_T76A[4];_T76C=_T76B.l;_T76D=_T76C.first_line;_T1879->f3=Cyc_Position_loc_to_seg(_T76D);_T762=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T762;_T76E=yyyvsp;_T76F=& _T76E[0].v;_T770=(union Cyc_YYSTYPE*)_T76F;_T771=Cyc_yyget_YY28(_T770);_T1878->tl=_T771.tms;_T760=(struct Cyc_List_List*)_T1878;}_T1877.tms=_T760;_T757=_T1877;}
# 1990
yyval=Cyc_YY28(_T757);goto _LL0;case 190: _T772=yyyvsp;_T773=& _T772[2].v;_T774=(union Cyc_YYSTYPE*)_T773;{
# 1993 "parse.y"
struct _tuple27*_T1877=Cyc_yyget_YY40(_T774);struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;void*_T187A;struct Cyc_Absyn_VarargInfo*_T187B;int _T187C;struct Cyc_List_List*_T187D;{struct _tuple27 _T187E=*_T1877;_T187D=_T187E.f0;_T187C=_T187E.f1;_T187B=_T187E.f2;_T187A=_T187E.f3;_T1879=_T187E.f4;_T1878=_T187E.f5;}{struct Cyc_List_List*lis=_T187D;int b=_T187C;struct Cyc_Absyn_VarargInfo*c=_T187B;void*eff=_T187A;struct Cyc_List_List*ec=_T1879;struct Cyc_List_List*qb=_T1878;_T775=yyyvsp;_T776=& _T775[4].v;_T777=(union Cyc_YYSTYPE*)_T776;{
struct _tuple21 _T187E=Cyc_yyget_YY62(_T777);struct Cyc_Absyn_Exp*_T187F;struct Cyc_Absyn_Exp*_T1880;struct Cyc_Absyn_Exp*_T1881;struct Cyc_Absyn_Exp*_T1882;_T1882=_T187E.f0;_T1881=_T187E.f1;_T1880=_T187E.f2;_T187F=_T187E.f3;{struct Cyc_Absyn_Exp*chk=_T1882;struct Cyc_Absyn_Exp*req=_T1881;struct Cyc_Absyn_Exp*ens=_T1880;struct Cyc_Absyn_Exp*thrws=_T187F;{struct Cyc_Parse_Declarator _T1883;_T779=yyyvsp;_T77A=& _T779[0].v;_T77B=(union Cyc_YYSTYPE*)_T77A;_T77C=
Cyc_yyget_YY28(_T77B);_T1883.id=_T77C.id;_T77D=yyyvsp;_T77E=& _T77D[0].v;_T77F=(union Cyc_YYSTYPE*)_T77E;_T780=Cyc_yyget_YY28(_T77F);_T1883.varloc=_T780.varloc;_T782=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T782,0U,sizeof(struct Cyc_List_List));_T784=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T784,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1885->tag=3;_T786=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1886=_region_malloc(_T786,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1886->tag=1;_T1886->f1=lis;_T1886->f2=b;_T1886->f3=c;_T1886->f4=eff;_T1886->f5=ec;_T1886->f6=qb;_T1886->f7=chk;_T1886->f8=req;_T1886->f9=ens;_T1886->f10=thrws;_T785=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1886;}_T1885->f1=(void*)_T785;_T783=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T783;_T787=yyyvsp;_T788=& _T787[0].v;_T789=(union Cyc_YYSTYPE*)_T788;_T78A=Cyc_yyget_YY28(_T789);_T1884->tl=_T78A.tms;_T781=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T781;_T778=_T1883;}yyval=Cyc_YY28(_T778);goto _LL0;}}}}case 191:{struct Cyc_Parse_Declarator _T1877;_T78C=yyyvsp;_T78D=& _T78C[0].v;_T78E=(union Cyc_YYSTYPE*)_T78D;_T78F=
# 1998 "parse.y"
Cyc_yyget_YY28(_T78E);_T1877.id=_T78F.id;_T790=yyyvsp;_T791=& _T790[0].v;_T792=(union Cyc_YYSTYPE*)_T791;_T793=Cyc_yyget_YY28(_T792);_T1877.varloc=_T793.varloc;_T795=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T795,0U,sizeof(struct Cyc_List_List));_T797=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T797,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1879->tag=3;_T799=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T187A=_region_malloc(_T799,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T187A->tag=0;_T79A=yyyvsp;_T79B=& _T79A[2].v;_T79C=(union Cyc_YYSTYPE*)_T79B;_T187A->f1=Cyc_yyget_YY37(_T79C);_T79D=yyyvsp;_T79E=_T79D[0];_T79F=_T79E.l;_T7A0=_T79F.first_line;_T187A->f2=Cyc_Position_loc_to_seg(_T7A0);_T798=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T187A;}_T1879->f1=(void*)_T798;_T796=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T796;_T7A1=yyyvsp;_T7A2=& _T7A1[0].v;_T7A3=(union Cyc_YYSTYPE*)_T7A2;_T7A4=Cyc_yyget_YY28(_T7A3);_T1878->tl=_T7A4.tms;_T794=(struct Cyc_List_List*)_T1878;}_T1877.tms=_T794;_T78B=_T1877;}yyval=Cyc_YY28(_T78B);goto _LL0;case 192: _T7A6=Cyc_List_map_c;{
# 2001
struct Cyc_List_List*(*_T1877)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T7A6;_T7A5=_T1877;}_T7A7=yyyvsp;_T7A8=_T7A7[1];_T7A9=_T7A8.l;_T7AA=_T7A9.first_line;_T7AB=Cyc_Position_loc_to_seg(_T7AA);_T7AC=yyyvsp;_T7AD=& _T7AC[2].v;_T7AE=(union Cyc_YYSTYPE*)_T7AD;_T7AF=Cyc_yyget_YY41(_T7AE);_T7B0=Cyc_List_imp_rev(_T7AF);{struct Cyc_List_List*ts=_T7A5(Cyc_Parse_typ2tvar,_T7AB,_T7B0);{struct Cyc_Parse_Declarator _T1877;_T7B2=yyyvsp;_T7B3=& _T7B2[0].v;_T7B4=(union Cyc_YYSTYPE*)_T7B3;_T7B5=
Cyc_yyget_YY28(_T7B4);_T1877.id=_T7B5.id;_T7B6=yyyvsp;_T7B7=& _T7B6[0].v;_T7B8=(union Cyc_YYSTYPE*)_T7B7;_T7B9=Cyc_yyget_YY28(_T7B8);_T1877.varloc=_T7B9.varloc;_T7BB=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T7BB,0U,sizeof(struct Cyc_List_List));_T7BD=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T7BD,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1879->tag=4;_T1879->f1=ts;_T7BE=yyyvsp;_T7BF=_T7BE[0];_T7C0=_T7BF.l;_T7C1=_T7C0.first_line;_T1879->f2=Cyc_Position_loc_to_seg(_T7C1);_T1879->f3=0;_T7BC=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T7BC;_T7C2=yyyvsp;_T7C3=& _T7C2[0].v;_T7C4=(union Cyc_YYSTYPE*)_T7C3;_T7C5=Cyc_yyget_YY28(_T7C4);_T1878->tl=_T7C5.tms;_T7BA=(struct Cyc_List_List*)_T1878;}_T1877.tms=_T7BA;_T7B1=_T1877;}yyval=Cyc_YY28(_T7B1);goto _LL0;}case 193:{struct Cyc_Parse_Declarator _T1877;_T7C7=yyyvsp;_T7C8=& _T7C7[0].v;_T7C9=(union Cyc_YYSTYPE*)_T7C8;_T7CA=
# 2005 "parse.y"
Cyc_yyget_YY28(_T7C9);_T1877.id=_T7CA.id;_T7CB=yyyvsp;_T7CC=& _T7CB[0].v;_T7CD=(union Cyc_YYSTYPE*)_T7CC;_T7CE=Cyc_yyget_YY28(_T7CD);_T1877.varloc=_T7CE.varloc;_T7D0=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T7D0,0U,sizeof(struct Cyc_List_List));_T7D2=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T7D2,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1879->tag=5;_T7D3=yyyvsp;_T7D4=_T7D3[1];_T7D5=_T7D4.l;_T7D6=_T7D5.first_line;_T1879->f1=Cyc_Position_loc_to_seg(_T7D6);_T7D7=yyyvsp;_T7D8=& _T7D7[1].v;_T7D9=(union Cyc_YYSTYPE*)_T7D8;_T1879->f2=Cyc_yyget_YY46(_T7D9);_T7D1=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T7D1;_T7DA=yyyvsp;_T7DB=& _T7DA[0].v;_T7DC=(union Cyc_YYSTYPE*)_T7DB;_T7DD=
Cyc_yyget_YY28(_T7DC);_T1878->tl=_T7DD.tms;_T7CF=(struct Cyc_List_List*)_T1878;}
# 2005
_T1877.tms=_T7CF;_T7C6=_T1877;}yyval=Cyc_YY28(_T7C6);goto _LL0;case 194:{struct Cyc_Parse_Declarator _T1877;_T7DF=yyyvsp;_T7E0=& _T7DF[0].v;_T7E1=(union Cyc_YYSTYPE*)_T7E0;
# 2012 "parse.y"
_T1877.id=Cyc_yyget_QualId_tok(_T7E1);_T7E2=yyyvsp;_T7E3=_T7E2[0];_T7E4=_T7E3.l;_T7E5=_T7E4.first_line;_T1877.varloc=Cyc_Position_loc_to_seg(_T7E5);_T1877.tms=0;_T7DE=_T1877;}yyval=Cyc_YY28(_T7DE);goto _LL0;case 195:{struct Cyc_Parse_Declarator _T1877;_T7E7=yyyvsp;_T7E8=& _T7E7[0].v;_T7E9=(union Cyc_YYSTYPE*)_T7E8;
# 2013 "parse.y"
_T1877.id=Cyc_yyget_QualId_tok(_T7E9);_T7EA=yyyvsp;_T7EB=_T7EA[0];_T7EC=_T7EB.l;_T7ED=_T7EC.first_line;_T1877.varloc=Cyc_Position_loc_to_seg(_T7ED);_T1877.tms=0;_T7E6=_T1877;}yyval=Cyc_YY28(_T7E6);goto _LL0;case 196: _T7EE=yyyvsp;_T7EF=_T7EE[1];
# 2014 "parse.y"
yyval=_T7EF.v;goto _LL0;case 197: _T7F0=yyyvsp;_T7F1=& _T7F0[2].v;_T7F2=(union Cyc_YYSTYPE*)_T7F1;{
# 2018 "parse.y"
struct Cyc_Parse_Declarator d=Cyc_yyget_YY28(_T7F2);_T7F4=yyr;{struct Cyc_List_List*_T1877=_region_malloc(_T7F4,0U,sizeof(struct Cyc_List_List));_T7F6=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1878=_region_malloc(_T7F6,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1878->tag=5;_T7F7=yyyvsp;_T7F8=_T7F7[1];_T7F9=_T7F8.l;_T7FA=_T7F9.first_line;
_T1878->f1=Cyc_Position_loc_to_seg(_T7FA);_T7FB=yyyvsp;_T7FC=& _T7FB[1].v;_T7FD=(union Cyc_YYSTYPE*)_T7FC;_T1878->f2=Cyc_yyget_YY46(_T7FD);_T7F5=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1878;}_T1877->hd=(void*)_T7F5;_T7FE=d;_T1877->tl=_T7FE.tms;_T7F3=(struct Cyc_List_List*)_T1877;}d.tms=_T7F3;_T7FF=yyyvsp;_T800=_T7FF[2];
yyval=_T800.v;goto _LL0;}case 198: _T801=yyyvsp;_T802=& _T801[0].v;_T803=(union Cyc_YYSTYPE*)_T802;{
# 2023 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T803);{struct Cyc_Parse_Declarator _T1877;_T805=one;
_T1877.id=_T805.id;_T806=one;_T1877.varloc=_T806.varloc;_T808=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T808,0U,sizeof(struct Cyc_List_List));_T80A=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T80A,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1879->tag=0;_T80B=yyyvsp;_T80C=& _T80B[3].v;_T80D=(union Cyc_YYSTYPE*)_T80C;
_T1879->f1=Cyc_yyget_YY54(_T80D);_T80E=yyyvsp;_T80F=_T80E[3];_T810=_T80F.l;_T811=_T810.first_line;_T1879->f2=Cyc_Position_loc_to_seg(_T811);_T809=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T809;_T812=one;_T1878->tl=_T812.tms;_T807=(struct Cyc_List_List*)_T1878;}_T1877.tms=_T807;_T804=_T1877;}
# 2024
yyval=Cyc_YY28(_T804);goto _LL0;}case 199: _T813=yyyvsp;_T814=& _T813[0].v;_T815=(union Cyc_YYSTYPE*)_T814;{
# 2027 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T815);{struct Cyc_Parse_Declarator _T1877;_T817=one;
_T1877.id=_T817.id;_T818=one;_T1877.varloc=_T818.varloc;_T81A=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T81A,0U,sizeof(struct Cyc_List_List));_T81C=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T81C,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1879->tag=1;_T81D=yyyvsp;_T81E=& _T81D[2].v;_T81F=(union Cyc_YYSTYPE*)_T81E;
_T1879->f1=Cyc_yyget_Exp_tok(_T81F);_T820=yyyvsp;_T821=& _T820[4].v;_T822=(union Cyc_YYSTYPE*)_T821;_T1879->f2=Cyc_yyget_YY54(_T822);_T823=yyyvsp;_T824=_T823[4];_T825=_T824.l;_T826=_T825.first_line;_T1879->f3=Cyc_Position_loc_to_seg(_T826);_T81B=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T81B;_T827=one;
_T1878->tl=_T827.tms;_T819=(struct Cyc_List_List*)_T1878;}
# 2029
_T1877.tms=_T819;_T816=_T1877;}
# 2028
yyval=Cyc_YY28(_T816);goto _LL0;}case 200: _T828=yyyvsp;_T829=& _T828[2].v;_T82A=(union Cyc_YYSTYPE*)_T829;{
# 2032 "parse.y"
struct _tuple27*_T1877=Cyc_yyget_YY40(_T82A);struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;void*_T187A;struct Cyc_Absyn_VarargInfo*_T187B;int _T187C;struct Cyc_List_List*_T187D;{struct _tuple27 _T187E=*_T1877;_T187D=_T187E.f0;_T187C=_T187E.f1;_T187B=_T187E.f2;_T187A=_T187E.f3;_T1879=_T187E.f4;_T1878=_T187E.f5;}{struct Cyc_List_List*lis=_T187D;int b=_T187C;struct Cyc_Absyn_VarargInfo*c=_T187B;void*eff=_T187A;struct Cyc_List_List*ec=_T1879;struct Cyc_List_List*qb=_T1878;_T82B=yyyvsp;_T82C=& _T82B[4].v;_T82D=(union Cyc_YYSTYPE*)_T82C;{
struct _tuple21 _T187E=Cyc_yyget_YY62(_T82D);struct Cyc_Absyn_Exp*_T187F;struct Cyc_Absyn_Exp*_T1880;struct Cyc_Absyn_Exp*_T1881;struct Cyc_Absyn_Exp*_T1882;_T1882=_T187E.f0;_T1881=_T187E.f1;_T1880=_T187E.f2;_T187F=_T187E.f3;{struct Cyc_Absyn_Exp*chk=_T1882;struct Cyc_Absyn_Exp*req=_T1881;struct Cyc_Absyn_Exp*ens=_T1880;struct Cyc_Absyn_Exp*thrws=_T187F;_T82E=yyyvsp;_T82F=& _T82E[0].v;_T830=(union Cyc_YYSTYPE*)_T82F;{
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T830);{struct Cyc_Parse_Declarator _T1883;_T832=one;
_T1883.id=_T832.id;_T833=one;_T1883.varloc=_T833.varloc;_T835=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_T835,0U,sizeof(struct Cyc_List_List));_T837=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_T837,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1885->tag=3;_T839=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1886=_region_malloc(_T839,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1886->tag=1;_T1886->f1=lis;_T1886->f2=b;_T1886->f3=c;_T1886->f4=eff;_T1886->f5=ec;_T1886->f6=qb;_T1886->f7=chk;_T1886->f8=req;_T1886->f9=ens;_T1886->f10=thrws;_T838=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1886;}_T1885->f1=(void*)_T838;_T836=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_T836;_T83A=one;_T1884->tl=_T83A.tms;_T834=(struct Cyc_List_List*)_T1884;}_T1883.tms=_T834;_T831=_T1883;}yyval=Cyc_YY28(_T831);goto _LL0;}}}}}case 201: _T83B=yyyvsp;_T83C=& _T83B[0].v;_T83D=(union Cyc_YYSTYPE*)_T83C;{
# 2038 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T83D);{struct Cyc_Parse_Declarator _T1877;_T83F=one;
_T1877.id=_T83F.id;_T840=one;_T1877.varloc=_T840.varloc;_T842=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T842,0U,sizeof(struct Cyc_List_List));_T844=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T844,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1879->tag=3;_T846=yyr;{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_T187A=_region_malloc(_T846,0U,sizeof(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct));_T187A->tag=0;_T847=yyyvsp;_T848=& _T847[2].v;_T849=(union Cyc_YYSTYPE*)_T848;_T187A->f1=Cyc_yyget_YY37(_T849);_T84A=yyyvsp;_T84B=_T84A[0];_T84C=_T84B.l;_T84D=_T84C.first_line;_T187A->f2=Cyc_Position_loc_to_seg(_T84D);_T845=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_T187A;}_T1879->f1=(void*)_T845;_T843=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T843;_T84E=one;_T1878->tl=_T84E.tms;_T841=(struct Cyc_List_List*)_T1878;}_T1877.tms=_T841;_T83E=_T1877;}yyval=Cyc_YY28(_T83E);goto _LL0;}case 202: _T850=Cyc_List_map_c;{
# 2042
struct Cyc_List_List*(*_T1877)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T850;_T84F=_T1877;}_T851=yyyvsp;_T852=_T851[1];_T853=_T852.l;_T854=_T853.first_line;_T855=Cyc_Position_loc_to_seg(_T854);_T856=yyyvsp;_T857=& _T856[2].v;_T858=(union Cyc_YYSTYPE*)_T857;_T859=Cyc_yyget_YY41(_T858);_T85A=Cyc_List_imp_rev(_T859);{struct Cyc_List_List*ts=_T84F(Cyc_Parse_typ2tvar,_T855,_T85A);_T85B=yyyvsp;_T85C=& _T85B[0].v;_T85D=(union Cyc_YYSTYPE*)_T85C;{
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T85D);{struct Cyc_Parse_Declarator _T1877;_T85F=one;
_T1877.id=_T85F.id;_T860=one;_T1877.varloc=_T860.varloc;_T862=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T862,0U,sizeof(struct Cyc_List_List));_T864=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T864,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1879->tag=4;_T1879->f1=ts;_T865=yyyvsp;_T866=_T865[0];_T867=_T866.l;_T868=_T867.first_line;_T1879->f2=Cyc_Position_loc_to_seg(_T868);_T1879->f3=0;_T863=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T863;_T869=one;_T1878->tl=_T869.tms;_T861=(struct Cyc_List_List*)_T1878;}_T1877.tms=_T861;_T85E=_T1877;}yyval=Cyc_YY28(_T85E);goto _LL0;}}case 203: _T86A=yyyvsp;_T86B=& _T86A[0].v;_T86C=(union Cyc_YYSTYPE*)_T86B;{
# 2046 "parse.y"
struct Cyc_Parse_Declarator one=Cyc_yyget_YY28(_T86C);{struct Cyc_Parse_Declarator _T1877;_T86E=one;
_T1877.id=_T86E.id;_T86F=one;_T1877.varloc=_T86F.varloc;_T871=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_T871,0U,sizeof(struct Cyc_List_List));_T873=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_T873,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1879->tag=5;_T874=yyyvsp;_T875=_T874[1];_T876=_T875.l;_T877=_T876.first_line;_T1879->f1=Cyc_Position_loc_to_seg(_T877);_T878=yyyvsp;_T879=& _T878[1].v;_T87A=(union Cyc_YYSTYPE*)_T879;_T1879->f2=Cyc_yyget_YY46(_T87A);_T872=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_T872;_T87B=one;_T1878->tl=_T87B.tms;_T870=(struct Cyc_List_List*)_T1878;}_T1877.tms=_T870;_T86D=_T1877;}yyval=Cyc_YY28(_T86D);goto _LL0;}case 204: _T87C=yyyvsp;_T87D=_T87C[0];
# 2052 "parse.y"
yyval=_T87D.v;goto _LL0;case 205: _T87E=yyyvsp;_T87F=& _T87E[0].v;_T880=(union Cyc_YYSTYPE*)_T87F;_T881=
# 2053 "parse.y"
Cyc_yyget_YY27(_T880);_T882=yyyvsp;_T883=& _T882[1].v;_T884=(union Cyc_YYSTYPE*)_T883;_T885=Cyc_yyget_YY27(_T884);_T886=Cyc_List_imp_append(_T881,_T885);yyval=Cyc_YY27(_T886);goto _LL0;case 206:  {
# 2057 "parse.y"
struct Cyc_List_List*ans=0;_T887=yyyvsp;_T888=& _T887[3].v;_T889=(union Cyc_YYSTYPE*)_T888;_T88A=
Cyc_yyget_YY46(_T889);if(_T88A==0)goto _TL2AD;_T88C=yyr;{struct Cyc_List_List*_T1877=_region_malloc(_T88C,0U,sizeof(struct Cyc_List_List));_T88E=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1878=_region_malloc(_T88E,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1878->tag=5;_T88F=yyyvsp;_T890=_T88F[3];_T891=_T890.l;_T892=_T891.first_line;
_T1878->f1=Cyc_Position_loc_to_seg(_T892);_T893=yyyvsp;_T894=& _T893[3].v;_T895=(union Cyc_YYSTYPE*)_T894;_T1878->f2=Cyc_yyget_YY46(_T895);_T88D=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1878;}_T1877->hd=(void*)_T88D;_T1877->tl=ans;_T88B=(struct Cyc_List_List*)_T1877;}ans=_T88B;goto _TL2AE;_TL2AD: _TL2AE: {
# 2061
struct Cyc_Absyn_PtrLoc*ptrloc=0;_T896=yyyvsp;_T897=& _T896[0].v;_T898=(union Cyc_YYSTYPE*)_T897;_T899=
Cyc_yyget_YY1(_T898);{struct _tuple22 _T1877=*_T899;void*_T1878;void*_T1879;unsigned _T187A;_T187A=_T1877.f0;_T1879=_T1877.f1;_T1878=_T1877.f2;{unsigned ploc=_T187A;void*nullable=_T1879;void*bound=_T1878;_T89A=Cyc_Flags_porting_c_code;
if(!_T89A)goto _TL2AF;{struct Cyc_Absyn_PtrLoc*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_PtrLoc));
_T187B->ptr_loc=ploc;_T89C=yyyvsp;_T89D=_T89C[2];_T89E=_T89D.l;_T89F=_T89E.first_line;_T187B->rgn_loc=Cyc_Position_loc_to_seg(_T89F);_T8A0=yyyvsp;_T8A1=_T8A0[1];_T8A2=_T8A1.l;_T8A3=_T8A2.first_line;_T187B->zt_loc=Cyc_Position_loc_to_seg(_T8A3);_T89B=(struct Cyc_Absyn_PtrLoc*)_T187B;}ptrloc=_T89B;goto _TL2B0;_TL2AF: _TL2B0: _T8A4=yyr;_T8A5=ptrloc;_T8A6=nullable;_T8A7=bound;_T8A8=yyyvsp;_T8A9=& _T8A8[2].v;_T8AA=(union Cyc_YYSTYPE*)_T8A9;_T8AB=
Cyc_yyget_YY45(_T8AA);_T8AC=yyyvsp;_T8AD=& _T8AC[1].v;_T8AE=(union Cyc_YYSTYPE*)_T8AD;_T8AF=Cyc_yyget_YY60(_T8AE);_T8B0=yyyvsp;_T8B1=& _T8B0[4].v;_T8B2=(union Cyc_YYSTYPE*)_T8B1;_T8B3=Cyc_yyget_YY24(_T8B2);{void*mod=Cyc_Parse_make_pointer_mod(_T8A4,_T8A5,_T8A6,_T8A7,_T8AB,_T8AF,_T8B3);_T8B5=yyr;{struct Cyc_List_List*_T187B=_region_malloc(_T8B5,0U,sizeof(struct Cyc_List_List));
_T187B->hd=mod;_T187B->tl=ans;_T8B4=(struct Cyc_List_List*)_T187B;}ans=_T8B4;
yyval=Cyc_YY27(ans);goto _LL0;}}}}}case 207:
# 2071
 yyval=Cyc_YY60(0);goto _LL0;case 208: _T8B7=yyr;{struct Cyc_List_List*_T1877=_region_malloc(_T8B7,0U,sizeof(struct Cyc_List_List));_T8B8=yyyvsp;_T8B9=& _T8B8[0].v;_T8BA=(union Cyc_YYSTYPE*)_T8B9;
# 2072 "parse.y"
_T1877->hd=Cyc_yyget_YY59(_T8BA);_T8BB=yyyvsp;_T8BC=& _T8BB[1].v;_T8BD=(union Cyc_YYSTYPE*)_T8BC;_T1877->tl=Cyc_yyget_YY60(_T8BD);_T8B6=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY60(_T8B6);goto _LL0;case 209: _T8BF=yyr;{struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8BF,0U,sizeof(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct));_T1877->tag=0;_T8C0=yyyvsp;_T8C1=& _T8C0[2].v;_T8C2=(union Cyc_YYSTYPE*)_T8C1;
# 2077 "parse.y"
_T1877->f1=Cyc_yyget_Exp_tok(_T8C2);_T8BE=(struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8C3=(void*)_T8BE;yyval=Cyc_YY59(_T8C3);goto _LL0;case 210: _T8C4=yyyvsp;_T8C5=_T8C4[0];_T8C6=_T8C5.l;_T8C7=_T8C6.first_line;_T8C8=
# 2079 "parse.y"
Cyc_Position_loc_to_seg(_T8C7);_T8C9=_tag_fat("@region qualifiers are deprecated; use @effect instead",sizeof(char),55U);_T8CA=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T8C8,_T8C9,_T8CA);_T8CC=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8CC,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T1877->tag=2;{struct Cyc_List_List*_T1878=_cycalloc(sizeof(struct Cyc_List_List));_T8CE=yyyvsp;_T8CF=& _T8CE[2].v;_T8D0=(union Cyc_YYSTYPE*)_T8CF;
_T1878->hd=Cyc_yyget_YY45(_T8D0);_T1878->tl=0;_T8CD=(struct Cyc_List_List*)_T1878;}_T1877->f1=_T8CD;_T8CB=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8D1=(void*)_T8CB;yyval=Cyc_YY59(_T8D1);goto _LL0;case 211: _T8D3=yyr;{struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8D3,0U,sizeof(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct));_T1877->tag=2;_T8D4=yyyvsp;_T8D5=& _T8D4[2].v;_T8D6=(union Cyc_YYSTYPE*)_T8D5;
# 2081 "parse.y"
_T1877->f1=Cyc_yyget_YY41(_T8D6);_T8D2=(struct Cyc_Parse_Effect_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8D7=(void*)_T8D2;yyval=Cyc_YY59(_T8D7);goto _LL0;case 212: _T8D9=yyr;{struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8D9,0U,sizeof(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct));
# 2082 "parse.y"
_T1877->tag=3;_T8D8=(struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8DA=(void*)_T8D8;yyval=Cyc_YY59(_T8DA);goto _LL0;case 213: _T8DC=yyr;{struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8DC,0U,sizeof(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct));
# 2083 "parse.y"
_T1877->tag=4;_T8DB=(struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8DD=(void*)_T8DB;yyval=Cyc_YY59(_T8DD);goto _LL0;case 214: _T8DF=yyr;{struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8DF,0U,sizeof(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct));
# 2084 "parse.y"
_T1877->tag=7;_T8DE=(struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8E0=(void*)_T8DE;yyval=Cyc_YY59(_T8E0);goto _LL0;case 215: _T8E2=yyr;{struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8E2,0U,sizeof(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2085 "parse.y"
_T1877->tag=5;_T8E1=(struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8E3=(void*)_T8E1;yyval=Cyc_YY59(_T8E3);goto _LL0;case 216: _T8E5=yyr;{struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8E5,0U,sizeof(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct));
# 2086 "parse.y"
_T1877->tag=6;_T8E4=(struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8E6=(void*)_T8E4;yyval=Cyc_YY59(_T8E6);goto _LL0;case 217: _T8E8=yyr;{struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8E8,0U,sizeof(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct));
# 2087 "parse.y"
_T1877->tag=8;_T8E7=(struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8E9=(void*)_T8E7;yyval=Cyc_YY59(_T8E9);goto _LL0;case 218: _T8EB=yyr;{struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8EB,0U,sizeof(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct));
# 2088 "parse.y"
_T1877->tag=9;_T8EA=(struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8EC=(void*)_T8EA;yyval=Cyc_YY59(_T8EC);goto _LL0;case 219: _T8EE=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8EE,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T1877->tag=10;_T8EF=yyyvsp;_T8F0=& _T8EF[2].v;_T8F1=(union Cyc_YYSTYPE*)_T8F0;
# 2089 "parse.y"
_T1877->f1=Cyc_yyget_YY58(_T8F1);_T8ED=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8F2=(void*)_T8ED;yyval=Cyc_YY59(_T8F2);goto _LL0;case 220: _T8F3=yyyvsp;_T8F4=_T8F3[0];_T8F5=_T8F4.l;_T8F6=_T8F5.first_line;_T8F7=
# 2090 "parse.y"
Cyc_Position_loc_to_seg(_T8F6);_T8F8=yyyvsp;_T8F9=& _T8F8[0].v;_T8FA=(union Cyc_YYSTYPE*)_T8F9;_T8FB=Cyc_yyget_String_tok(_T8FA);{void*aq=Cyc_Parse_id2aqual(_T8F7,_T8FB);_T8FD=yyr;{struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*_T1877=_region_malloc(_T8FD,0U,sizeof(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct));_T1877->tag=10;_T1877->f1=aq;_T8FC=(struct Cyc_Parse_Alias_ptrqual_Parse_Pointer_qual_struct*)_T1877;}_T8FE=(void*)_T8FC;yyval=Cyc_YY59(_T8FE);goto _LL0;}case 221: _T8FF=yyyvsp;_T900=& _T8FF[0].v;_T901=(union Cyc_YYSTYPE*)_T900;_T902=
# 2094 "parse.y"
Cyc_yyget_YY58(_T901);yyval=Cyc_YY58(_T902);goto _LL0;case 222: _T903=yyyvsp;_T904=& _T903[0].v;_T905=(union Cyc_YYSTYPE*)_T904;_T906=
# 2100 "parse.y"
Cyc_yyget_YY45(_T905);_T907=Cyc_Absyn_al_qual_type;_T908=Cyc_Absyn_aqual_var_type(_T906,_T907);yyval=Cyc_YY58(_T908);goto _LL0;case 223: _T909=yyyvsp;_T90A=& _T909[2].v;_T90B=(union Cyc_YYSTYPE*)_T90A;_T90C=
# 2104 "parse.y"
Cyc_yyget_YY45(_T90B);_T90D=Cyc_Absyn_aqualsof_type(_T90C);_T90E=Cyc_Absyn_al_qual_type;_T90F=Cyc_Absyn_aqual_var_type(_T90D,_T90E);yyval=Cyc_YY58(_T90F);goto _LL0;case 224: goto _LL0;case 225: _T910=yyyvsp;_T911=_T910[0];_T912=_T911.l;_T913=_T912.first_line;_T914=
# 2110 "parse.y"
Cyc_Position_loc_to_seg(_T913);_T915=yyyvsp;_T916=& _T915[0].v;_T917=(union Cyc_YYSTYPE*)_T916;_T918=Cyc_yyget_String_tok(_T917);_T919=Cyc_Parse_id2aqual(_T914,_T918);yyval=Cyc_YY58(_T919);goto _LL0;case 226:{struct _tuple22*_T1877=_cycalloc(sizeof(struct _tuple22));_T91B=yyyvsp;_T91C=_T91B[0];_T91D=_T91C.l;_T91E=_T91D.first_line;
# 2116 "parse.y"
_T1877->f0=Cyc_Position_loc_to_seg(_T91E);_T1877->f1=Cyc_Absyn_true_type;_T91F=Cyc_Parse_parsing_tempest;if(!_T91F)goto _TL2B1;_T1877->f2=Cyc_Absyn_fat_bound_type;goto _TL2B2;_TL2B1: _T920=yyyvsp;_T921=_T920[0];_T922=_T921.l;_T923=_T922.first_line;_T924=Cyc_Position_loc_to_seg(_T923);_T925=Cyc_Position_string_of_segment(_T924);_T926=yyyvsp;_T927=& _T926[1].v;_T928=(union Cyc_YYSTYPE*)_T927;_T929=Cyc_yyget_YY2(_T928);_T1877->f2=Cyc_Parse_assign_cvar_pos(_T925,0,_T929);_TL2B2: _T91A=(struct _tuple22*)_T1877;}yyval=Cyc_YY1(_T91A);goto _LL0;case 227:{struct _tuple22*_T1877=_cycalloc(sizeof(struct _tuple22));_T92B=yyyvsp;_T92C=_T92B[0];_T92D=_T92C.l;_T92E=_T92D.first_line;
# 2117 "parse.y"
_T1877->f0=Cyc_Position_loc_to_seg(_T92E);_T1877->f1=Cyc_Absyn_false_type;_T92F=yyyvsp;_T930=_T92F[0];_T931=_T930.l;_T932=_T931.first_line;_T933=Cyc_Position_loc_to_seg(_T932);_T934=Cyc_Position_string_of_segment(_T933);_T935=yyyvsp;_T936=& _T935[1].v;_T937=(union Cyc_YYSTYPE*)_T936;_T938=Cyc_yyget_YY2(_T937);_T1877->f2=Cyc_Parse_assign_cvar_pos(_T934,0,_T938);_T92A=(struct _tuple22*)_T1877;}yyval=Cyc_YY1(_T92A);goto _LL0;case 228: _T93A=Cyc_Flags_override_fat;
# 2118 "parse.y"
if(!_T93A)goto _TL2B3;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2B3;_T93B=Cyc_Flags_interproc;if(!_T93B)goto _TL2B3;{struct _tuple22*_T1877=_cycalloc(sizeof(struct _tuple22));_T93D=yyyvsp;_T93E=_T93D[0];_T93F=_T93E.l;_T940=_T93F.first_line;
_T1877->f0=Cyc_Position_loc_to_seg(_T940);_T1877->f1=Cyc_Absyn_true_type;_T941=yyyvsp;_T942=_T941[0];_T943=_T942.l;_T944=_T943.first_line;_T945=Cyc_Position_loc_to_seg(_T944);_T946=Cyc_Position_string_of_segment(_T945);_T947=& Cyc_Kinds_ptrbko;_T948=(struct Cyc_Core_Opt*)_T947;_T949=Cyc_Absyn_cvar_type(_T948);_T1877->f2=Cyc_Parse_assign_cvar_pos(_T946,1,_T949);_T93C=(struct _tuple22*)_T1877;}_T939=_T93C;goto _TL2B4;_TL2B3:{struct _tuple22*_T1877=_cycalloc(sizeof(struct _tuple22));_T94B=yyyvsp;_T94C=_T94B[0];_T94D=_T94C.l;_T94E=_T94D.first_line;
_T1877->f0=Cyc_Position_loc_to_seg(_T94E);_T1877->f1=Cyc_Absyn_true_type;_T1877->f2=Cyc_Absyn_fat_bound_type;_T94A=(struct _tuple22*)_T1877;}_T939=_T94A;_TL2B4:
# 2118
 yyval=Cyc_YY1(_T939);goto _LL0;case 229: _T950=Cyc_Flags_interproc;
# 2123
if(!_T950)goto _TL2B5;if(Cyc_Parse_inside_noinference_block!=0)goto _TL2B5;_T951=& Cyc_Kinds_ptrbko;_T952=(struct Cyc_Core_Opt*)_T951;_T94F=Cyc_Absyn_cvar_type(_T952);goto _TL2B6;_TL2B5: _T94F=Cyc_Absyn_bounds_one();_TL2B6: yyval=Cyc_YY2(_T94F);goto _LL0;case 230: _T953=yyyvsp;_T954=& _T953[1].v;_T955=(union Cyc_YYSTYPE*)_T954;_T956=
# 2124 "parse.y"
Cyc_yyget_Exp_tok(_T955);_T957=Cyc_Absyn_thin_bounds_exp(_T956);yyval=Cyc_YY2(_T957);goto _LL0;case 231: _T958=Cyc_Flags_resolve;
# 2127 "parse.y"
if(_T958)goto _TL2B7;else{goto _TL2B9;}
_TL2B9:{int(*_T1877)(unsigned,struct _fat_ptr)=(int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort;_T959=_T1877;}_T95A=_tag_fat("Type variable not permitted in pointer bound",sizeof(char),45U);_T959(0U,_T95A);goto _TL2B8;_TL2B7: _TL2B8: _T95B=yyyvsp;_T95C=& _T95B[1].v;_T95D=(union Cyc_YYSTYPE*)_T95C;_T95E=
Cyc_yyget_String_tok(_T95D);_T95F=Cyc_Parse_typevar2cvar(_T95E);yyval=Cyc_YY2(_T95F);goto _LL0;case 232: _T960=
# 2134 "parse.y"
Cyc_Tcutil_any_bool(0);yyval=Cyc_YY54(_T960);goto _LL0;case 233:
# 2135 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_true_type);goto _LL0;case 234:
# 2136 "parse.y"
 yyval=Cyc_YY54(Cyc_Absyn_false_type);goto _LL0;case 235:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T962=yyyvsp;_T963=& _T962[0].v;_T964=(union Cyc_YYSTYPE*)_T963;
# 2140 "parse.y"
_T1877->hd=Cyc_yyget_YY45(_T964);_T1877->tl=0;_T961=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY41(_T961);goto _LL0;case 236:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T966=yyyvsp;_T967=& _T966[0].v;_T968=(union Cyc_YYSTYPE*)_T967;
# 2141 "parse.y"
_T1877->hd=Cyc_yyget_YY45(_T968);_T969=yyyvsp;_T96A=& _T969[2].v;_T96B=(union Cyc_YYSTYPE*)_T96A;_T1877->tl=Cyc_yyget_YY41(_T96B);_T965=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY41(_T965);goto _LL0;case 237: _T96C=& Cyc_Kinds_eko;_T96D=(struct Cyc_Core_Opt*)_T96C;_T96E=
# 2145 "parse.y"
Cyc_Absyn_new_evar(_T96D,0);yyval=Cyc_YY45(_T96E);goto _LL0;case 238: _T96F=yyyvsp;_T970=& _T96F[0].v;_T971=(union Cyc_YYSTYPE*)_T970;{
# 2146 "parse.y"
struct Cyc_List_List*es=Cyc_yyget_YY41(_T971);_T972=Cyc_List_length(es);if(_T972!=1)goto _TL2BA;_T973=_check_null(es);_T974=_T973->hd;yyval=Cyc_YY45(_T974);goto _TL2BB;_TL2BA: _T975=yyyvsp;_T976=& _T975[0].v;_T977=(union Cyc_YYSTYPE*)_T976;_T978=Cyc_yyget_YY41(_T977);_T979=Cyc_Absyn_join_eff(_T978);yyval=Cyc_YY45(_T979);_TL2BB: goto _LL0;}case 239: _T97A=& Cyc_Kinds_eko;_T97B=(struct Cyc_Core_Opt*)_T97A;_T97C=
# 2147 "parse.y"
Cyc_Absyn_new_evar(_T97B,0);yyval=Cyc_YY45(_T97C);goto _LL0;case 240: _T97D=yyvs;_T97E=yyvsp_offset + 1;_T97F=_check_fat_subscript(_T97D,sizeof(struct Cyc_Yystacktype),_T97E);_T980=(struct Cyc_Yystacktype*)_T97F;_T981=*_T980;_T982=_T981.l;_T983=_T982.first_line;_T984=
# 2158 "parse.y"
Cyc_Position_loc_to_seg(_T983);_T985=Cyc_Absyn_empty_tqual(_T984);yyval=Cyc_YY24(_T985);goto _LL0;case 241: _T986=yyyvsp;_T987=& _T986[0].v;_T988=(union Cyc_YYSTYPE*)_T987;_T989=
# 2159 "parse.y"
Cyc_yyget_YY24(_T988);_T98A=yyyvsp;_T98B=& _T98A[1].v;_T98C=(union Cyc_YYSTYPE*)_T98B;_T98D=Cyc_yyget_YY24(_T98C);_T98E=Cyc_Absyn_combine_tqual(_T989,_T98D);yyval=Cyc_YY24(_T98E);goto _LL0;case 242: _T98F=yyyvsp;_T990=& _T98F[1].v;_T991=(union Cyc_YYSTYPE*)_T990;{
# 2164 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T991);_T993=ec_qb;_T994=(unsigned)_T993;
if(!_T994)goto _TL2BC;_T995=ec_qb;_T992=*_T995;goto _TL2BD;_TL2BC:{struct _tuple28 _T1877;_T1877.f0=0;_T1877.f1=0;_T996=_T1877;}_T992=_T996;_TL2BD: {struct _tuple28 _T1877=_T992;struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_List_List*ec=_T1879;struct Cyc_List_List*qb=_T1878;{struct _tuple27*_T187A=_cycalloc(sizeof(struct _tuple27));
_T187A->f0=0;_T187A->f1=0;_T187A->f2=0;_T998=yyyvsp;_T999=& _T998[0].v;_T99A=(union Cyc_YYSTYPE*)_T999;_T187A->f3=Cyc_yyget_YY50(_T99A);_T187A->f4=ec;_T187A->f5=qb;_T997=(struct _tuple27*)_T187A;}yyval=Cyc_YY40(_T997);goto _LL0;}}}case 243: _T99B=yyyvsp;_T99C=& _T99B[2].v;_T99D=(union Cyc_YYSTYPE*)_T99C;{
# 2168 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T99D);_T99F=ec_qb;_T9A0=(unsigned)_T99F;
if(!_T9A0)goto _TL2BE;_T9A1=ec_qb;_T99E=*_T9A1;goto _TL2BF;_TL2BE:{struct _tuple28 _T1877;_T1877.f0=0;_T1877.f1=0;_T9A2=_T1877;}_T99E=_T9A2;_TL2BF: {struct _tuple28 _T1877=_T99E;struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_List_List*ec=_T1879;struct Cyc_List_List*qb=_T1878;{struct _tuple27*_T187A=_cycalloc(sizeof(struct _tuple27));_T9A4=yyyvsp;_T9A5=& _T9A4[0].v;_T9A6=(union Cyc_YYSTYPE*)_T9A5;_T9A7=
Cyc_yyget_YY39(_T9A6);_T187A->f0=Cyc_List_imp_rev(_T9A7);_T187A->f1=0;_T187A->f2=0;_T9A8=yyyvsp;_T9A9=& _T9A8[1].v;_T9AA=(union Cyc_YYSTYPE*)_T9A9;_T187A->f3=Cyc_yyget_YY50(_T9AA);_T187A->f4=ec;_T187A->f5=qb;_T9A3=(struct _tuple27*)_T187A;}yyval=Cyc_YY40(_T9A3);goto _LL0;}}}case 244: _T9AB=yyyvsp;_T9AC=& _T9AB[4].v;_T9AD=(union Cyc_YYSTYPE*)_T9AC;{
# 2172 "parse.y"
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9AD);_T9AF=ec_qb;_T9B0=(unsigned)_T9AF;
if(!_T9B0)goto _TL2C0;_T9B1=ec_qb;_T9AE=*_T9B1;goto _TL2C1;_TL2C0:{struct _tuple28 _T1877;_T1877.f0=0;_T1877.f1=0;_T9B2=_T1877;}_T9AE=_T9B2;_TL2C1: {struct _tuple28 _T1877=_T9AE;struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_List_List*ec=_T1879;struct Cyc_List_List*qb=_T1878;{struct _tuple27*_T187A=_cycalloc(sizeof(struct _tuple27));_T9B4=yyyvsp;_T9B5=& _T9B4[0].v;_T9B6=(union Cyc_YYSTYPE*)_T9B5;_T9B7=
Cyc_yyget_YY39(_T9B6);_T187A->f0=Cyc_List_imp_rev(_T9B7);_T187A->f1=1;_T187A->f2=0;_T9B8=yyyvsp;_T9B9=& _T9B8[3].v;_T9BA=(union Cyc_YYSTYPE*)_T9B9;_T187A->f3=Cyc_yyget_YY50(_T9BA);_T187A->f4=ec;_T187A->f5=qb;_T9B3=(struct _tuple27*)_T187A;}yyval=Cyc_YY40(_T9B3);goto _LL0;}}}case 245: _T9BB=yyyvsp;_T9BC=& _T9BB[2].v;_T9BD=(union Cyc_YYSTYPE*)_T9BC;{
# 2177
struct _tuple8*_T1877=Cyc_yyget_YY38(_T9BD);void*_T1878;struct Cyc_Absyn_Tqual _T1879;struct _fat_ptr*_T187A;{struct _tuple8 _T187B=*_T1877;_T187A=_T187B.f0;_T1879=_T187B.f1;_T1878=_T187B.f2;}{struct _fat_ptr*n=_T187A;struct Cyc_Absyn_Tqual tq=_T1879;void*t=_T1878;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9BE=v;_T9BE->name=n;_T9BF=v;_T9BF->tq=tq;_T9C0=v;_T9C0->type=t;_T9C1=v;_T9C2=yyyvsp;_T9C3=& _T9C2[1].v;_T9C4=(union Cyc_YYSTYPE*)_T9C3;_T9C1->inject=Cyc_yyget_YY32(_T9C4);_T9C5=yyyvsp;_T9C6=& _T9C5[4].v;_T9C7=(union Cyc_YYSTYPE*)_T9C6;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9C7);_T9C9=ec_qb;_T9CA=(unsigned)_T9C9;
if(!_T9CA)goto _TL2C2;_T9CB=ec_qb;_T9C8=*_T9CB;goto _TL2C3;_TL2C2:{struct _tuple28 _T187B;_T187B.f0=0;_T187B.f1=0;_T9CC=_T187B;}_T9C8=_T9CC;_TL2C3: {struct _tuple28 _T187B=_T9C8;struct Cyc_List_List*_T187C;struct Cyc_List_List*_T187D;_T187D=_T187B.f0;_T187C=_T187B.f1;{struct Cyc_List_List*ec=_T187D;struct Cyc_List_List*qb=_T187C;{struct _tuple27*_T187E=_cycalloc(sizeof(struct _tuple27));
_T187E->f0=0;_T187E->f1=0;_T187E->f2=v;_T9CE=yyyvsp;_T9CF=& _T9CE[3].v;_T9D0=(union Cyc_YYSTYPE*)_T9CF;_T187E->f3=Cyc_yyget_YY50(_T9D0);_T187E->f4=ec;_T187E->f5=qb;_T9CD=(struct _tuple27*)_T187E;}yyval=Cyc_YY40(_T9CD);goto _LL0;}}}}}case 246: _T9D1=yyyvsp;_T9D2=& _T9D1[4].v;_T9D3=(union Cyc_YYSTYPE*)_T9D2;{
# 2185
struct _tuple8*_T1877=Cyc_yyget_YY38(_T9D3);void*_T1878;struct Cyc_Absyn_Tqual _T1879;struct _fat_ptr*_T187A;{struct _tuple8 _T187B=*_T1877;_T187A=_T187B.f0;_T1879=_T187B.f1;_T1878=_T187B.f2;}{struct _fat_ptr*n=_T187A;struct Cyc_Absyn_Tqual tq=_T1879;void*t=_T1878;
struct Cyc_Absyn_VarargInfo*v;v=_cycalloc(sizeof(struct Cyc_Absyn_VarargInfo));_T9D4=v;_T9D4->name=n;_T9D5=v;_T9D5->tq=tq;_T9D6=v;_T9D6->type=t;_T9D7=v;_T9D8=yyyvsp;_T9D9=& _T9D8[3].v;_T9DA=(union Cyc_YYSTYPE*)_T9D9;_T9D7->inject=Cyc_yyget_YY32(_T9DA);_T9DB=yyyvsp;_T9DC=& _T9DB[6].v;_T9DD=(union Cyc_YYSTYPE*)_T9DC;{
struct _tuple28*ec_qb=Cyc_yyget_YY51(_T9DD);_T9DF=ec_qb;_T9E0=(unsigned)_T9DF;
if(!_T9E0)goto _TL2C4;_T9E1=ec_qb;_T9DE=*_T9E1;goto _TL2C5;_TL2C4:{struct _tuple28 _T187B;_T187B.f0=0;_T187B.f1=0;_T9E2=_T187B;}_T9DE=_T9E2;_TL2C5: {struct _tuple28 _T187B=_T9DE;struct Cyc_List_List*_T187C;struct Cyc_List_List*_T187D;_T187D=_T187B.f0;_T187C=_T187B.f1;{struct Cyc_List_List*ec=_T187D;struct Cyc_List_List*qb=_T187C;{struct _tuple27*_T187E=_cycalloc(sizeof(struct _tuple27));_T9E4=yyyvsp;_T9E5=& _T9E4[0].v;_T9E6=(union Cyc_YYSTYPE*)_T9E5;_T9E7=
Cyc_yyget_YY39(_T9E6);_T187E->f0=Cyc_List_imp_rev(_T9E7);_T187E->f1=0;_T187E->f2=v;_T9E8=yyyvsp;_T9E9=& _T9E8[5].v;_T9EA=(union Cyc_YYSTYPE*)_T9E9;_T187E->f3=Cyc_yyget_YY50(_T9EA);_T187E->f4=ec;_T187E->f5=qb;_T9E3=(struct _tuple27*)_T187E;}yyval=Cyc_YY40(_T9E3);goto _LL0;}}}}}case 247:
# 2195 "parse.y"
 yyval=Cyc_YY50(0);goto _LL0;case 248: _T9EB=yyyvsp;_T9EC=_T9EB[0];_T9ED=_T9EC.l;_T9EE=_T9ED.first_line;_T9EF=
# 2196 "parse.y"
Cyc_Position_loc_to_seg(_T9EE);_T9F0=yyyvsp;_T9F1=& _T9F0[0].v;_T9F2=(union Cyc_YYSTYPE*)_T9F1;_T9F3=Cyc_yyget_String_tok(_T9F2);_T9F4=Cyc_Parse_id2aqual(_T9EF,_T9F3);yyval=Cyc_YY50(_T9F4);goto _LL0;case 249: _T9F5=yyyvsp;_T9F6=& _T9F5[0].v;_T9F7=(union Cyc_YYSTYPE*)_T9F6;_T9F8=
# 2199
Cyc_yyget_String_tok(_T9F7);{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct));_T1877->tag=1;_T1877->f1=0;_T9F9=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T1877;}_T9FA=(void*)_T9F9;_T9FB=yyyvsp;_T9FC=& _T9FB[1].v;_T9FD=(union Cyc_YYSTYPE*)_T9FC;_T9FE=Cyc_yyget_YY50(_T9FD);_T9FF=yyyvsp;_TA00=_T9FF[0];_TA01=_TA00.l;_TA02=_TA01.first_line;_TA03=Cyc_Position_loc_to_seg(_TA02);_TA04=Cyc_Parse_id2type(_T9F8,_T9FA,_T9FE,_TA03);yyval=Cyc_YY45(_TA04);goto _LL0;case 250: _TA05=yyyvsp;_TA06=& _TA05[0].v;_TA07=(union Cyc_YYSTYPE*)_TA06;_TA08=
# 2200 "parse.y"
Cyc_yyget_String_tok(_TA07);_TA09=yyyvsp;_TA0A=& _TA09[2].v;_TA0B=(union Cyc_YYSTYPE*)_TA0A;_TA0C=Cyc_yyget_YY44(_TA0B);_TA0D=Cyc_Kinds_kind_to_bound(_TA0C);_TA0E=yyyvsp;_TA0F=& _TA0E[3].v;_TA10=(union Cyc_YYSTYPE*)_TA0F;_TA11=Cyc_yyget_YY50(_TA10);_TA12=yyyvsp;_TA13=_TA12[0];_TA14=_TA13.l;_TA15=_TA14.first_line;_TA16=Cyc_Position_loc_to_seg(_TA15);_TA17=Cyc_Parse_id2type(_TA08,_TA0D,_TA11,_TA16);yyval=Cyc_YY45(_TA17);goto _LL0;case 251:
# 2203
 yyval=Cyc_YY50(0);goto _LL0;case 252: _TA18=yyyvsp;_TA19=& _TA18[1].v;_TA1A=(union Cyc_YYSTYPE*)_TA19;_TA1B=
# 2204 "parse.y"
Cyc_yyget_YY41(_TA1A);_TA1C=Cyc_Absyn_join_eff(_TA1B);yyval=Cyc_YY50(_TA1C);goto _LL0;case 253:
# 2208 "parse.y"
 yyval=Cyc_YY51(0);goto _LL0;case 254: _TA1D=yyyvsp;_TA1E=_TA1D[1];
# 2209 "parse.y"
yyval=_TA1E.v;goto _LL0;case 255:{struct _tuple28*_T1877=_cycalloc(sizeof(struct _tuple28));{void*_T1878[1];_TA21=yyyvsp;_TA22=& _TA21[0].v;_TA23=(union Cyc_YYSTYPE*)_TA22;_T1878[0]=
# 2214 "parse.y"
Cyc_yyget_YY52(_TA23);_TA24=_tag_fat(_T1878,sizeof(void*),1);_TA20=Cyc_List_list(_TA24);}_T1877->f0=_TA20;_T1877->f1=0;_TA1F=(struct _tuple28*)_T1877;}yyval=Cyc_YY51(_TA1F);goto _LL0;case 256:{struct _tuple28*_T1877=_cycalloc(sizeof(struct _tuple28));
# 2216 "parse.y"
_T1877->f0=0;{struct _tuple29*_T1878[1];_TA27=yyyvsp;_TA28=& _TA27[0].v;_TA29=(union Cyc_YYSTYPE*)_TA28;_T1878[0]=Cyc_yyget_YY53(_TA29);_TA2A=_tag_fat(_T1878,sizeof(struct _tuple29*),1);_TA26=Cyc_List_list(_TA2A);}_T1877->f1=_TA26;_TA25=(struct _tuple28*)_T1877;}yyval=Cyc_YY51(_TA25);goto _LL0;case 257: _TA2B=yyyvsp;_TA2C=& _TA2B[2].v;_TA2D=(union Cyc_YYSTYPE*)_TA2C;{
# 2219 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(_TA2D);_TA2E=
_check_null(rest);{struct _tuple28 _T1877=*_TA2E;struct Cyc_List_List*_T1878;_T1878=_T1877.f0;{struct Cyc_List_List*rpo=_T1878;_TA2F=rest;{struct Cyc_List_List*_T1879=_cycalloc(sizeof(struct Cyc_List_List));_TA31=yyyvsp;_TA32=& _TA31[0].v;_TA33=(union Cyc_YYSTYPE*)_TA32;
_T1879->hd=Cyc_yyget_YY52(_TA33);_T1879->tl=rpo;_TA30=(struct Cyc_List_List*)_T1879;}(*_TA2F).f0=_TA30;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 258: _TA34=yyyvsp;_TA35=& _TA34[2].v;_TA36=(union Cyc_YYSTYPE*)_TA35;{
# 2226 "parse.y"
struct _tuple28*rest=Cyc_yyget_YY51(_TA36);_TA37=
_check_null(rest);{struct _tuple28 _T1877=*_TA37;struct Cyc_List_List*_T1878;_T1878=_T1877.f1;{struct Cyc_List_List*qb=_T1878;_TA38=rest;{struct Cyc_List_List*_T1879=_cycalloc(sizeof(struct Cyc_List_List));_TA3A=yyyvsp;_TA3B=& _TA3A[0].v;_TA3C=(union Cyc_YYSTYPE*)_TA3B;
_T1879->hd=Cyc_yyget_YY53(_TA3C);_T1879->tl=qb;_TA39=(struct Cyc_List_List*)_T1879;}(*_TA38).f1=_TA39;
yyval=Cyc_YY51(rest);goto _LL0;}}}case 259:  {
# 2236 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct));_TA3D=kb;_TA3D->tag=2;_TA3E=kb;_TA3E->f1=0;_TA3F=kb;_TA40=& Cyc_Kinds_ek;_TA3F->f2=(struct Cyc_Absyn_Kind*)_TA40;_TA41=yyyvsp;_TA42=& _TA41[2].v;_TA43=(union Cyc_YYSTYPE*)_TA42;_TA44=
Cyc_yyget_String_tok(_TA43);_TA45=kb;_TA46=(void*)_TA45;_TA47=yyyvsp;_TA48=_TA47[2];_TA49=_TA48.l;_TA4A=_TA49.first_line;_TA4B=Cyc_Position_loc_to_seg(_TA4A);{void*t=Cyc_Parse_id2type(_TA44,_TA46,0,_TA4B);{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct));_T1877->tag=2;_TA4D=yyyvsp;_TA4E=& _TA4D[0].v;_TA4F=(union Cyc_YYSTYPE*)_TA4E;_TA50=
Cyc_yyget_YY41(_TA4F);_T1877->f1=Cyc_Parse_effect_from_atomic(_TA50);_T1877->f2=t;_TA4C=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T1877;}_TA51=(void*)_TA4C;yyval=Cyc_YY52(_TA51);goto _LL0;}}case 260:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct));_T1877->tag=1;_TA53=yyyvsp;_TA54=& _TA53[0].v;_TA55=(union Cyc_YYSTYPE*)_TA54;_TA56=
# 2242 "parse.y"
Cyc_yyget_YY41(_TA55);_T1877->f1=Cyc_Parse_effect_from_atomic(_TA56);_TA57=yyyvsp;_TA58=& _TA57[2].v;_TA59=(union Cyc_YYSTYPE*)_TA58;_TA5A=
Cyc_yyget_YY41(_TA59);_T1877->f2=Cyc_Parse_effect_from_atomic(_TA5A);_TA52=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T1877;}_TA5B=(void*)_TA52;
# 2242
yyval=Cyc_YY52(_TA5B);goto _LL0;case 261: _TA5C=yyyvsp;_TA5D=_TA5C[0];_TA5E=_TA5D.l;_TA5F=_TA5E.first_line;_TA60=
# 2247 "parse.y"
Cyc_Position_loc_to_seg(_TA5F);_TA61=yyyvsp;_TA62=& _TA61[0].v;_TA63=(union Cyc_YYSTYPE*)_TA62;_TA64=Cyc_yyget_String_tok(_TA63);Cyc_Parse_check_single_constraint(_TA60,_TA64);{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct));_T1877->tag=0;_TA66=yyyvsp;_TA67=& _TA66[2].v;_TA68=(union Cyc_YYSTYPE*)_TA67;_TA69=
Cyc_yyget_YY41(_TA68);_T1877->f1=Cyc_Parse_effect_from_atomic(_TA69);_TA65=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T1877;}_TA6A=(void*)_TA65;yyval=Cyc_YY52(_TA6A);goto _LL0;case 262:{struct _tuple29*_T1877=_cycalloc(sizeof(struct _tuple29));_TA6C=yyyvsp;_TA6D=& _TA6C[2].v;_TA6E=(union Cyc_YYSTYPE*)_TA6D;
# 2255 "parse.y"
_T1877->f0=Cyc_yyget_YY45(_TA6E);_TA6F=yyyvsp;_TA70=& _TA6F[0].v;_TA71=(union Cyc_YYSTYPE*)_TA70;_T1877->f1=Cyc_yyget_YY45(_TA71);_TA6B=(struct _tuple29*)_T1877;}yyval=Cyc_YY53(_TA6B);goto _LL0;case 263:
# 2260 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_al_qual_type);goto _LL0;case 264:
# 2261 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_un_qual_type);goto _LL0;case 265:
# 2262 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rc_qual_type);goto _LL0;case 266:
# 2263 "parse.y"
 yyval=Cyc_YY58(Cyc_Absyn_rtd_qual_type);goto _LL0;case 267: _TA72=yyyvsp;_TA73=_TA72[0];_TA74=_TA73.l;_TA75=_TA74.first_line;_TA76=
# 2264 "parse.y"
Cyc_Position_loc_to_seg(_TA75);_TA77=yyyvsp;_TA78=& _TA77[0].v;_TA79=(union Cyc_YYSTYPE*)_TA78;_TA7A=Cyc_yyget_String_tok(_TA79);_TA7B=Cyc_Parse_id2aqual(_TA76,_TA7A);yyval=Cyc_YY58(_TA7B);goto _LL0;case 268: _TA7C=yyyvsp;_TA7D=& _TA7C[0].v;_TA7E=(union Cyc_YYSTYPE*)_TA7D;_TA7F=
# 2269 "parse.y"
Cyc_yyget_YY58(_TA7E);yyval=Cyc_YY45(_TA7F);goto _LL0;case 269: _TA80=yyyvsp;_TA81=& _TA80[2].v;_TA82=(union Cyc_YYSTYPE*)_TA81;_TA83=
# 2272 "parse.y"
Cyc_yyget_YY45(_TA82);_TA84=Cyc_Absyn_aqualsof_type(_TA83);_TA85=Cyc_Absyn_al_qual_type;_TA86=Cyc_Absyn_aqual_var_type(_TA84,_TA85);yyval=Cyc_YY45(_TA86);goto _LL0;case 270:  {
# 2279 "parse.y"
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*kb;kb=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_TA87=kb;_TA87->tag=0;_TA88=kb;_TA89=& Cyc_Kinds_aqk;_TA88->f1=(struct Cyc_Absyn_Kind*)_TA89;_TA8A=yyyvsp;_TA8B=& _TA8A[0].v;_TA8C=(union Cyc_YYSTYPE*)_TA8B;_TA8D=
Cyc_yyget_String_tok(_TA8C);_TA8E=kb;_TA8F=(void*)_TA8E;_TA90=yyyvsp;_TA91=_TA90[0];_TA92=_TA91.l;_TA93=_TA92.first_line;_TA94=Cyc_Position_loc_to_seg(_TA93);_TA95=Cyc_Parse_id2type(_TA8D,_TA8F,0,_TA94);yyval=Cyc_YY45(_TA95);goto _LL0;}case 271: _TA96=yyyvsp;_TA97=& _TA96[2].v;_TA98=(union Cyc_YYSTYPE*)_TA97;_TA99=
# 2284 "parse.y"
Cyc_yyget_YY45(_TA98);_TA9A=Cyc_Absyn_aqualsof_type(_TA99);yyval=Cyc_YY45(_TA9A);goto _LL0;case 272:
# 2313 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 273: _TA9B=yyyvsp;_TA9C=& _TA9B[0].v;_TA9D=(union Cyc_YYSTYPE*)_TA9C;_TA9E=
# 2315 "parse.y"
Cyc_yyget_String_tok(_TA9D);_TA9F=_tag_fat("inject",sizeof(char),7U);_TAA0=Cyc_zstrcmp(_TA9E,_TA9F);if(_TAA0==0)goto _TL2C6;{struct Cyc_Warn_String_Warn_Warg_struct _T1877;_T1877.tag=0;
_T1877.f1=_tag_fat("missing type in function declaration",sizeof(char),37U);_TAA1=_T1877;}{struct Cyc_Warn_String_Warn_Warg_struct _T1877=_TAA1;void*_T1878[1];_T1878[0]=& _T1877;_TAA2=yyyvsp;_TAA3=_TAA2[0];_TAA4=_TAA3.l;_TAA5=_TAA4.first_line;_TAA6=Cyc_Position_loc_to_seg(_TAA5);_TAA7=_tag_fat(_T1878,sizeof(void*),1);Cyc_Warn_err2(_TAA6,_TAA7);}goto _TL2C7;_TL2C6: _TL2C7:
 yyval=Cyc_YY32(1);goto _LL0;case 274: _TAA8=yyyvsp;_TAA9=_TAA8[0];
# 2322 "parse.y"
yyval=_TAA9.v;goto _LL0;case 275: _TAAA=yyyvsp;_TAAB=& _TAAA[0].v;_TAAC=(union Cyc_YYSTYPE*)_TAAB;_TAAD=
# 2323 "parse.y"
Cyc_yyget_YY41(_TAAC);_TAAE=yyyvsp;_TAAF=& _TAAE[2].v;_TAB0=(union Cyc_YYSTYPE*)_TAAF;_TAB1=Cyc_yyget_YY41(_TAB0);_TAB2=Cyc_List_imp_append(_TAAD,_TAB1);yyval=Cyc_YY41(_TAB2);goto _LL0;case 276:
# 2327 "parse.y"
 yyval=Cyc_YY41(0);goto _LL0;case 277: _TAB3=yyyvsp;_TAB4=_TAB3[1];
# 2328 "parse.y"
yyval=_TAB4.v;goto _LL0;case 278:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TAB6=yyyvsp;_TAB7=& _TAB6[2].v;_TAB8=(union Cyc_YYSTYPE*)_TAB7;_TAB9=
# 2329 "parse.y"
Cyc_yyget_YY45(_TAB8);_T1877->hd=Cyc_Absyn_regionsof_eff(_TAB9);_T1877->tl=0;_TAB5=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY41(_TAB5);goto _LL0;case 279: _TABA=yyyvsp;_TABB=& _TABA[0].v;_TABC=(union Cyc_YYSTYPE*)_TABB;_TABD=
# 2331 "parse.y"
Cyc_yyget_YY45(_TABC);_TABE=& Cyc_Kinds_ek;_TABF=(struct Cyc_Absyn_Kind*)_TABE;Cyc_Parse_set_vartyp_kind(_TABD,_TABF,0);{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TAC1=yyyvsp;_TAC2=& _TAC1[0].v;_TAC3=(union Cyc_YYSTYPE*)_TAC2;
_T1877->hd=Cyc_yyget_YY45(_TAC3);_T1877->tl=0;_TAC0=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY41(_TAC0);goto _LL0;case 280:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TAC5=yyyvsp;_TAC6=& _TAC5[0].v;_TAC7=(union Cyc_YYSTYPE*)_TAC6;_TAC8=
# 2339 "parse.y"
Cyc_yyget_YY38(_TAC7);_TAC9=yyyvsp;_TACA=_TAC9[0];_TACB=_TACA.l;_TACC=_TACB.first_line;_TACD=Cyc_Position_loc_to_seg(_TACC);_T1877->hd=Cyc_Parse_type_name_to_type(_TAC8,_TACD);_T1877->tl=0;_TAC4=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY41(_TAC4);goto _LL0;case 281:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TACF=yyyvsp;_TAD0=& _TACF[0].v;_TAD1=(union Cyc_YYSTYPE*)_TAD0;_TAD2=
# 2341 "parse.y"
Cyc_yyget_YY38(_TAD1);_TAD3=yyyvsp;_TAD4=_TAD3[0];_TAD5=_TAD4.l;_TAD6=_TAD5.first_line;_TAD7=Cyc_Position_loc_to_seg(_TAD6);_T1877->hd=Cyc_Parse_type_name_to_type(_TAD2,_TAD7);_TAD8=yyyvsp;_TAD9=& _TAD8[2].v;_TADA=(union Cyc_YYSTYPE*)_TAD9;_T1877->tl=Cyc_yyget_YY41(_TADA);_TACE=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY41(_TACE);goto _LL0;case 282:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TADC=yyyvsp;_TADD=& _TADC[0].v;_TADE=(union Cyc_YYSTYPE*)_TADD;
# 2346 "parse.y"
_T1877->hd=Cyc_yyget_YY38(_TADE);_T1877->tl=0;_TADB=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY39(_TADB);goto _LL0;case 283:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TAE0=yyyvsp;_TAE1=& _TAE0[2].v;_TAE2=(union Cyc_YYSTYPE*)_TAE1;
# 2347 "parse.y"
_T1877->hd=Cyc_yyget_YY38(_TAE2);_TAE3=yyyvsp;_TAE4=& _TAE3[0].v;_TAE5=(union Cyc_YYSTYPE*)_TAE4;_T1877->tl=Cyc_yyget_YY39(_TAE5);_TADF=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY39(_TADF);goto _LL0;case 284: _TAE6=yyyvsp;_TAE7=& _TAE6[0].v;_TAE8=(union Cyc_YYSTYPE*)_TAE7;{
# 2353 "parse.y"
struct _tuple26 _T1877=Cyc_yyget_YY36(_TAE8);struct Cyc_List_List*_T1878;struct Cyc_Parse_Type_specifier _T1879;struct Cyc_Absyn_Tqual _T187A;_T187A=_T1877.f0;_T1879=_T1877.f1;_T1878=_T1877.f2;{struct Cyc_Absyn_Tqual tq=_T187A;struct Cyc_Parse_Type_specifier tspecs=_T1879;struct Cyc_List_List*atts=_T1878;_TAE9=tq;_TAEA=_TAE9.loc;
if(_TAEA!=0U)goto _TL2C8;_TAEB=yyyvsp;_TAEC=_TAEB[0];_TAED=_TAEC.l;_TAEE=_TAED.first_line;tq.loc=Cyc_Position_loc_to_seg(_TAEE);goto _TL2C9;_TL2C8: _TL2C9: _TAEF=yyyvsp;_TAF0=& _TAEF[1].v;_TAF1=(union Cyc_YYSTYPE*)_TAF0;{
struct Cyc_Parse_Declarator _T187B=Cyc_yyget_YY28(_TAF1);struct Cyc_List_List*_T187C;unsigned _T187D;struct _tuple0*_T187E;_T187E=_T187B.id;_T187D=_T187B.varloc;_T187C=_T187B.tms;{struct _tuple0*qv=_T187E;unsigned varloc=_T187D;struct Cyc_List_List*tms=_T187C;_TAF2=tspecs;_TAF3=yyyvsp;_TAF4=_TAF3[0];_TAF5=_TAF4.l;_TAF6=_TAF5.first_line;_TAF7=
Cyc_Position_loc_to_seg(_TAF6);{void*t=Cyc_Parse_speclist2typ(_TAF2,_TAF7);
struct _tuple14 _T187F=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T1880;struct Cyc_List_List*_T1881;void*_T1882;struct Cyc_Absyn_Tqual _T1883;_T1883=_T187F.f0;_T1882=_T187F.f1;_T1881=_T187F.f2;_T1880=_T187F.f3;{struct Cyc_Absyn_Tqual tq2=_T1883;void*t2=_T1882;struct Cyc_List_List*tvs=_T1881;struct Cyc_List_List*atts2=_T1880;
if(tvs==0)goto _TL2CA;{struct Cyc_Warn_String_Warn_Warg_struct _T1884;_T1884.tag=0;
_T1884.f1=_tag_fat("parameter with bad type params",sizeof(char),31U);_TAF8=_T1884;}{struct Cyc_Warn_String_Warn_Warg_struct _T1884=_TAF8;void*_T1885[1];_T1885[0]=& _T1884;_TAF9=yyyvsp;_TAFA=_TAF9[1];_TAFB=_TAFA.l;_TAFC=_TAFB.first_line;_TAFD=Cyc_Position_loc_to_seg(_TAFC);_TAFE=_tag_fat(_T1885,sizeof(void*),1);Cyc_Warn_err2(_TAFD,_TAFE);}goto _TL2CB;_TL2CA: _TL2CB: _TAFF=
Cyc_Absyn_is_qvar_qualified(qv);if(!_TAFF)goto _TL2CC;{struct Cyc_Warn_String_Warn_Warg_struct _T1884;_T1884.tag=0;
_T1884.f1=_tag_fat("parameter cannot be qualified with a namespace",sizeof(char),47U);_TB00=_T1884;}{struct Cyc_Warn_String_Warn_Warg_struct _T1884=_TB00;void*_T1885[1];_T1885[0]=& _T1884;_TB01=yyyvsp;_TB02=_TB01[0];_TB03=_TB02.l;_TB04=_TB03.first_line;_TB05=Cyc_Position_loc_to_seg(_TB04);_TB06=_tag_fat(_T1885,sizeof(void*),1);Cyc_Warn_err2(_TB05,_TB06);}goto _TL2CD;_TL2CC: _TL2CD: _TB07=qv;_TB08=*_TB07;{
struct _fat_ptr*idopt=_TB08.f1;
if(atts2==0)goto _TL2CE;{struct Cyc_Warn_String_Warn_Warg_struct _T1884;_T1884.tag=0;
_T1884.f1=_tag_fat("extra attributes on parameter, ignoring",sizeof(char),40U);_TB09=_T1884;}{struct Cyc_Warn_String_Warn_Warg_struct _T1884=_TB09;void*_T1885[1];_T1885[0]=& _T1884;_TB0A=yyyvsp;_TB0B=_TB0A[0];_TB0C=_TB0B.l;_TB0D=_TB0C.first_line;_TB0E=Cyc_Position_loc_to_seg(_TB0D);_TB0F=_tag_fat(_T1885,sizeof(void*),1);Cyc_Warn_warn2(_TB0E,_TB0F);}goto _TL2CF;_TL2CE: _TL2CF:{struct _tuple8*_T1884=_cycalloc(sizeof(struct _tuple8));
_T1884->f0=idopt;_T1884->f1=tq2;_T1884->f2=t2;_TB10=(struct _tuple8*)_T1884;}yyval=Cyc_YY38(_TB10);goto _LL0;}}}}}}}case 285: _TB11=yyyvsp;_TB12=& _TB11[0].v;_TB13=(union Cyc_YYSTYPE*)_TB12;{
# 2368 "parse.y"
struct _tuple26 _T1877=Cyc_yyget_YY36(_TB13);struct Cyc_List_List*_T1878;struct Cyc_Parse_Type_specifier _T1879;struct Cyc_Absyn_Tqual _T187A;_T187A=_T1877.f0;_T1879=_T1877.f1;_T1878=_T1877.f2;{struct Cyc_Absyn_Tqual tq=_T187A;struct Cyc_Parse_Type_specifier tspecs=_T1879;struct Cyc_List_List*atts=_T1878;_TB14=tq;_TB15=_TB14.loc;
if(_TB15!=0U)goto _TL2D0;_TB16=yyyvsp;_TB17=_TB16[0];_TB18=_TB17.l;_TB19=_TB18.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB19);goto _TL2D1;_TL2D0: _TL2D1: _TB1A=tspecs;_TB1B=yyyvsp;_TB1C=_TB1B[0];_TB1D=_TB1C.l;_TB1E=_TB1D.first_line;_TB1F=
Cyc_Position_loc_to_seg(_TB1E);{void*t=Cyc_Parse_speclist2typ(_TB1A,_TB1F);
if(atts==0)goto _TL2D2;{struct Cyc_Warn_String_Warn_Warg_struct _T187B;_T187B.tag=0;
_T187B.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB20=_T187B;}{struct Cyc_Warn_String_Warn_Warg_struct _T187B=_TB20;void*_T187C[1];_T187C[0]=& _T187B;_TB21=yyyvsp;_TB22=_TB21[0];_TB23=_TB22.l;_TB24=_TB23.first_line;_TB25=Cyc_Position_loc_to_seg(_TB24);_TB26=_tag_fat(_T187C,sizeof(void*),1);Cyc_Warn_warn2(_TB25,_TB26);}goto _TL2D3;_TL2D2: _TL2D3:{struct _tuple8*_T187B=_cycalloc(sizeof(struct _tuple8));
_T187B->f0=0;_T187B->f1=tq;_T187B->f2=t;_TB27=(struct _tuple8*)_T187B;}yyval=Cyc_YY38(_TB27);goto _LL0;}}}case 286: _TB28=yyyvsp;_TB29=& _TB28[0].v;_TB2A=(union Cyc_YYSTYPE*)_TB29;{
# 2376 "parse.y"
struct _tuple26 _T1877=Cyc_yyget_YY36(_TB2A);struct Cyc_List_List*_T1878;struct Cyc_Parse_Type_specifier _T1879;struct Cyc_Absyn_Tqual _T187A;_T187A=_T1877.f0;_T1879=_T1877.f1;_T1878=_T1877.f2;{struct Cyc_Absyn_Tqual tq=_T187A;struct Cyc_Parse_Type_specifier tspecs=_T1879;struct Cyc_List_List*atts=_T1878;_TB2B=tq;_TB2C=_TB2B.loc;
if(_TB2C!=0U)goto _TL2D4;_TB2D=yyyvsp;_TB2E=_TB2D[0];_TB2F=_TB2E.l;_TB30=_TB2F.first_line;tq.loc=Cyc_Position_loc_to_seg(_TB30);goto _TL2D5;_TL2D4: _TL2D5: _TB31=tspecs;_TB32=yyyvsp;_TB33=_TB32[0];_TB34=_TB33.l;_TB35=_TB34.first_line;_TB36=
Cyc_Position_loc_to_seg(_TB35);{void*t=Cyc_Parse_speclist2typ(_TB31,_TB36);_TB37=yyyvsp;_TB38=& _TB37[1].v;_TB39=(union Cyc_YYSTYPE*)_TB38;_TB3A=
Cyc_yyget_YY31(_TB39);{struct Cyc_List_List*tms=_TB3A.tms;
struct _tuple14 _T187B=Cyc_Parse_apply_tms(tq,t,atts,tms);struct Cyc_List_List*_T187C;struct Cyc_List_List*_T187D;void*_T187E;struct Cyc_Absyn_Tqual _T187F;_T187F=_T187B.f0;_T187E=_T187B.f1;_T187D=_T187B.f2;_T187C=_T187B.f3;{struct Cyc_Absyn_Tqual tq2=_T187F;void*t2=_T187E;struct Cyc_List_List*tvs=_T187D;struct Cyc_List_List*atts2=_T187C;
if(tvs==0)goto _TL2D6;{struct Cyc_Warn_String_Warn_Warg_struct _T1880;_T1880.tag=0;
# 2383
_T1880.f1=_tag_fat("bad type parameters on formal argument, ignoring",sizeof(char),49U);_TB3B=_T1880;}{struct Cyc_Warn_String_Warn_Warg_struct _T1880=_TB3B;void*_T1881[1];_T1881[0]=& _T1880;_TB3C=yyyvsp;_TB3D=_TB3C[0];_TB3E=_TB3D.l;_TB3F=_TB3E.first_line;_TB40=
# 2382
Cyc_Position_loc_to_seg(_TB3F);_TB41=_tag_fat(_T1881,sizeof(void*),1);Cyc_Warn_warn2(_TB40,_TB41);}goto _TL2D7;_TL2D6: _TL2D7:
# 2384
 if(atts2==0)goto _TL2D8;{struct Cyc_Warn_String_Warn_Warg_struct _T1880;_T1880.tag=0;
_T1880.f1=_tag_fat("bad attributes on parameter, ignoring",sizeof(char),38U);_TB42=_T1880;}{struct Cyc_Warn_String_Warn_Warg_struct _T1880=_TB42;void*_T1881[1];_T1881[0]=& _T1880;_TB43=yyyvsp;_TB44=_TB43[0];_TB45=_TB44.l;_TB46=_TB45.first_line;_TB47=Cyc_Position_loc_to_seg(_TB46);_TB48=_tag_fat(_T1881,sizeof(void*),1);Cyc_Warn_warn2(_TB47,_TB48);}goto _TL2D9;_TL2D8: _TL2D9:{struct _tuple8*_T1880=_cycalloc(sizeof(struct _tuple8));
_T1880->f0=0;_T1880->f1=tq2;_T1880->f2=t2;_TB49=(struct _tuple8*)_T1880;}yyval=Cyc_YY38(_TB49);goto _LL0;}}}}}case 287: _TB4A=yyyvsp;_TB4B=& _TB4A[0].v;_TB4C=(union Cyc_YYSTYPE*)_TB4B;_TB4D=
# 2391 "parse.y"
Cyc_yyget_YY37(_TB4C);_TB4E=Cyc_List_imp_rev(_TB4D);yyval=Cyc_YY37(_TB4E);goto _LL0;case 288:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_TB51=yyyvsp;_TB52=& _TB51[0].v;_TB53=(union Cyc_YYSTYPE*)_TB52;
# 2394
*_T1878=Cyc_yyget_String_tok(_TB53);_TB50=(struct _fat_ptr*)_T1878;}_T1877->hd=_TB50;_T1877->tl=0;_TB4F=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY37(_TB4F);goto _LL0;case 289:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_TB56=yyyvsp;_TB57=& _TB56[2].v;_TB58=(union Cyc_YYSTYPE*)_TB57;
# 2395 "parse.y"
*_T1878=Cyc_yyget_String_tok(_TB58);_TB55=(struct _fat_ptr*)_T1878;}_T1877->hd=_TB55;_TB59=yyyvsp;_TB5A=& _TB59[0].v;_TB5B=(union Cyc_YYSTYPE*)_TB5A;_T1877->tl=Cyc_yyget_YY37(_TB5B);_TB54=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY37(_TB54);goto _LL0;case 290: _TB5C=yyyvsp;_TB5D=_TB5C[0];
# 2399 "parse.y"
yyval=_TB5D.v;goto _LL0;case 291: _TB5E=yyyvsp;_TB5F=_TB5E[0];
# 2400 "parse.y"
yyval=_TB5F.v;goto _LL0;case 292:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1877->tag=35;
# 2405 "parse.y"
_T1877->f1=0;_T1877->f2=0;_TB60=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1877;}_TB61=(void*)_TB60;_TB62=yyyvsp;_TB63=_TB62[0];_TB64=_TB63.l;_TB65=_TB64.first_line;_TB66=Cyc_Position_loc_to_seg(_TB65);_TB67=Cyc_Absyn_new_exp(_TB61,_TB66);yyval=Cyc_Exp_tok(_TB67);goto _LL0;case 293:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1877->tag=35;
# 2407 "parse.y"
_T1877->f1=0;_TB69=yyyvsp;_TB6A=& _TB69[1].v;_TB6B=(union Cyc_YYSTYPE*)_TB6A;_TB6C=Cyc_yyget_YY5(_TB6B);_T1877->f2=Cyc_List_imp_rev(_TB6C);_TB68=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1877;}_TB6D=(void*)_TB68;_TB6E=yyyvsp;_TB6F=_TB6E[0];_TB70=_TB6F.l;_TB71=_TB70.first_line;_TB72=Cyc_Position_loc_to_seg(_TB71);_TB73=Cyc_Absyn_new_exp(_TB6D,_TB72);yyval=Cyc_Exp_tok(_TB73);goto _LL0;case 294:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct));_T1877->tag=35;
# 2409 "parse.y"
_T1877->f1=0;_TB75=yyyvsp;_TB76=& _TB75[1].v;_TB77=(union Cyc_YYSTYPE*)_TB76;_TB78=Cyc_yyget_YY5(_TB77);_T1877->f2=Cyc_List_imp_rev(_TB78);_TB74=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T1877;}_TB79=(void*)_TB74;_TB7A=yyyvsp;_TB7B=_TB7A[0];_TB7C=_TB7B.l;_TB7D=_TB7C.first_line;_TB7E=Cyc_Position_loc_to_seg(_TB7D);_TB7F=Cyc_Absyn_new_exp(_TB79,_TB7E);yyval=Cyc_Exp_tok(_TB7F);goto _LL0;case 295: _TB80=yyyvsp;_TB81=_TB80[2];_TB82=_TB81.l;_TB83=_TB82.first_line;_TB84=
# 2411 "parse.y"
Cyc_Position_loc_to_seg(_TB83);{struct _tuple0*_T1877=_cycalloc(sizeof(struct _tuple0));_T1877->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_TB87=yyyvsp;_TB88=& _TB87[2].v;_TB89=(union Cyc_YYSTYPE*)_TB88;*_T1878=Cyc_yyget_String_tok(_TB89);_TB86=(struct _fat_ptr*)_T1878;}_T1877->f1=_TB86;_TB85=(struct _tuple0*)_T1877;}_TB8A=Cyc_Absyn_uint_type;_TB8B=yyyvsp;_TB8C=_TB8B[2];_TB8D=_TB8C.l;_TB8E=_TB8D.first_line;_TB8F=
Cyc_Position_loc_to_seg(_TB8E);_TB90=Cyc_Absyn_uint_exp(0U,_TB8F);{
# 2411
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_TB84,_TB85,_TB8A,_TB90,0);_TB91=vd;
# 2414
_TB91->tq.real_const=1;{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct));_T1877->tag=26;
_T1877->f1=vd;_TB93=yyyvsp;_TB94=& _TB93[4].v;_TB95=(union Cyc_YYSTYPE*)_TB94;_T1877->f2=Cyc_yyget_Exp_tok(_TB95);_TB96=yyyvsp;_TB97=& _TB96[6].v;_TB98=(union Cyc_YYSTYPE*)_TB97;_T1877->f3=Cyc_yyget_Exp_tok(_TB98);_T1877->f4=0;_TB92=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T1877;}_TB99=(void*)_TB92;_TB9A=yyyvsp;_TB9B=_TB9A[0];_TB9C=_TB9B.l;_TB9D=_TB9C.first_line;_TB9E=Cyc_Position_loc_to_seg(_TB9D);_TB9F=Cyc_Absyn_new_exp(_TB99,_TB9E);yyval=Cyc_Exp_tok(_TB9F);goto _LL0;}case 296: _TBA0=yyyvsp;_TBA1=& _TBA0[6].v;_TBA2=(union Cyc_YYSTYPE*)_TBA1;_TBA3=
# 2419 "parse.y"
Cyc_yyget_YY38(_TBA2);_TBA4=yyyvsp;_TBA5=_TBA4[6];_TBA6=_TBA5.l;_TBA7=_TBA6.first_line;_TBA8=Cyc_Position_loc_to_seg(_TBA7);{void*t=Cyc_Parse_type_name_to_type(_TBA3,_TBA8);{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct));_T1877->tag=27;_TBAA=yyyvsp;_TBAB=& _TBAA[4].v;_TBAC=(union Cyc_YYSTYPE*)_TBAB;
_T1877->f1=Cyc_yyget_Exp_tok(_TBAC);_T1877->f2=t;_T1877->f3=0;_TBA9=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T1877;}_TBAD=(void*)_TBA9;_TBAE=yyyvsp;_TBAF=_TBAE[0];_TBB0=_TBAF.l;_TBB1=_TBB0.first_line;_TBB2=Cyc_Position_loc_to_seg(_TBB1);_TBB3=Cyc_Absyn_new_exp(_TBAD,_TBB2);yyval=Cyc_Exp_tok(_TBB3);goto _LL0;}case 297:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1878=_cycalloc(sizeof(struct _tuple34));
# 2427 "parse.y"
_T1878->f0=0;_TBB6=yyyvsp;_TBB7=& _TBB6[0].v;_TBB8=(union Cyc_YYSTYPE*)_TBB7;_T1878->f1=Cyc_yyget_Exp_tok(_TBB8);_TBB5=(struct _tuple34*)_T1878;}_T1877->hd=_TBB5;_T1877->tl=0;_TBB4=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY5(_TBB4);goto _LL0;case 298:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1878=_cycalloc(sizeof(struct _tuple34));_TBBB=yyyvsp;_TBBC=& _TBBB[0].v;_TBBD=(union Cyc_YYSTYPE*)_TBBC;
# 2429 "parse.y"
_T1878->f0=Cyc_yyget_YY42(_TBBD);_TBBE=yyyvsp;_TBBF=& _TBBE[1].v;_TBC0=(union Cyc_YYSTYPE*)_TBBF;_T1878->f1=Cyc_yyget_Exp_tok(_TBC0);_TBBA=(struct _tuple34*)_T1878;}_T1877->hd=_TBBA;_T1877->tl=0;_TBB9=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY5(_TBB9);goto _LL0;case 299:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1878=_cycalloc(sizeof(struct _tuple34));
# 2431 "parse.y"
_T1878->f0=0;_TBC3=yyyvsp;_TBC4=& _TBC3[2].v;_TBC5=(union Cyc_YYSTYPE*)_TBC4;_T1878->f1=Cyc_yyget_Exp_tok(_TBC5);_TBC2=(struct _tuple34*)_T1878;}_T1877->hd=_TBC2;_TBC6=yyyvsp;_TBC7=& _TBC6[0].v;_TBC8=(union Cyc_YYSTYPE*)_TBC7;_T1877->tl=Cyc_yyget_YY5(_TBC8);_TBC1=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY5(_TBC1);goto _LL0;case 300:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple34*_T1878=_cycalloc(sizeof(struct _tuple34));_TBCB=yyyvsp;_TBCC=& _TBCB[2].v;_TBCD=(union Cyc_YYSTYPE*)_TBCC;
# 2433 "parse.y"
_T1878->f0=Cyc_yyget_YY42(_TBCD);_TBCE=yyyvsp;_TBCF=& _TBCE[3].v;_TBD0=(union Cyc_YYSTYPE*)_TBCF;_T1878->f1=Cyc_yyget_Exp_tok(_TBD0);_TBCA=(struct _tuple34*)_T1878;}_T1877->hd=_TBCA;_TBD1=yyyvsp;_TBD2=& _TBD1[0].v;_TBD3=(union Cyc_YYSTYPE*)_TBD2;_T1877->tl=Cyc_yyget_YY5(_TBD3);_TBC9=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY5(_TBC9);goto _LL0;case 301: _TBD4=yyyvsp;_TBD5=& _TBD4[0].v;_TBD6=(union Cyc_YYSTYPE*)_TBD5;_TBD7=
# 2437 "parse.y"
Cyc_yyget_YY42(_TBD6);_TBD8=Cyc_List_imp_rev(_TBD7);yyval=Cyc_YY42(_TBD8);goto _LL0;case 302:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T1878->tag=1;{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));_TBDC=yyyvsp;_TBDD=& _TBDC[0].v;_TBDE=(union Cyc_YYSTYPE*)_TBDD;
# 2438 "parse.y"
*_T1879=Cyc_yyget_String_tok(_TBDE);_TBDB=(struct _fat_ptr*)_T1879;}_T1878->f1=_TBDB;_TBDA=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1878;}_T1877->hd=(void*)_TBDA;_T1877->tl=0;_TBD9=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY42(_TBD9);goto _LL0;case 303:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TBE0=yyyvsp;_TBE1=& _TBE0[0].v;_TBE2=(union Cyc_YYSTYPE*)_TBE1;
# 2443 "parse.y"
_T1877->hd=Cyc_yyget_YY43(_TBE2);_T1877->tl=0;_TBDF=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY42(_TBDF);goto _LL0;case 304:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TBE4=yyyvsp;_TBE5=& _TBE4[1].v;_TBE6=(union Cyc_YYSTYPE*)_TBE5;
# 2444 "parse.y"
_T1877->hd=Cyc_yyget_YY43(_TBE6);_TBE7=yyyvsp;_TBE8=& _TBE7[0].v;_TBE9=(union Cyc_YYSTYPE*)_TBE8;_T1877->tl=Cyc_yyget_YY42(_TBE9);_TBE3=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY42(_TBE3);goto _LL0;case 305:{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct));_T1877->tag=0;_TBEB=yyyvsp;_TBEC=& _TBEB[1].v;_TBED=(union Cyc_YYSTYPE*)_TBEC;
# 2448 "parse.y"
_T1877->f1=Cyc_yyget_Exp_tok(_TBED);_TBEA=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_T1877;}_TBEE=(void*)_TBEA;yyval=Cyc_YY43(_TBEE);goto _LL0;case 306:{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_FieldName_Absyn_Designator_struct));_T1877->tag=1;{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_TBF1=yyyvsp;_TBF2=& _TBF1[1].v;_TBF3=(union Cyc_YYSTYPE*)_TBF2;
# 2449 "parse.y"
*_T1878=Cyc_yyget_String_tok(_TBF3);_TBF0=(struct _fat_ptr*)_T1878;}_T1877->f1=_TBF0;_TBEF=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1877;}_TBF4=(void*)_TBEF;yyval=Cyc_YY43(_TBF4);goto _LL0;case 307: _TBF5=yyyvsp;_TBF6=& _TBF5[0].v;_TBF7=(union Cyc_YYSTYPE*)_TBF6;{
# 2454 "parse.y"
struct _tuple26 _T1877=Cyc_yyget_YY36(_TBF7);struct Cyc_List_List*_T1878;struct Cyc_Parse_Type_specifier _T1879;struct Cyc_Absyn_Tqual _T187A;_T187A=_T1877.f0;_T1879=_T1877.f1;_T1878=_T1877.f2;{struct Cyc_Absyn_Tqual tq=_T187A;struct Cyc_Parse_Type_specifier tss=_T1879;struct Cyc_List_List*atts=_T1878;_TBF8=tss;_TBF9=yyyvsp;_TBFA=_TBF9[0];_TBFB=_TBFA.l;_TBFC=_TBFB.first_line;_TBFD=
Cyc_Position_loc_to_seg(_TBFC);{void*t=Cyc_Parse_speclist2typ(_TBF8,_TBFD);
if(atts==0)goto _TL2DA;_TBFE=yyyvsp;_TBFF=_TBFE[0];_TC00=_TBFF.l;_TC01=_TC00.first_line;_TC02=
Cyc_Position_loc_to_seg(_TC01);_TC03=_tag_fat("ignoring attributes in type",sizeof(char),28U);_TC04=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC02,_TC03,_TC04);goto _TL2DB;_TL2DA: _TL2DB:{struct _tuple8*_T187B=_cycalloc(sizeof(struct _tuple8));
_T187B->f0=0;_T187B->f1=tq;_T187B->f2=t;_TC05=(struct _tuple8*)_T187B;}yyval=Cyc_YY38(_TC05);goto _LL0;}}}case 308: _TC06=yyyvsp;_TC07=& _TC06[0].v;_TC08=(union Cyc_YYSTYPE*)_TC07;{
# 2461 "parse.y"
struct _tuple26 _T1877=Cyc_yyget_YY36(_TC08);struct Cyc_List_List*_T1878;struct Cyc_Parse_Type_specifier _T1879;struct Cyc_Absyn_Tqual _T187A;_T187A=_T1877.f0;_T1879=_T1877.f1;_T1878=_T1877.f2;{struct Cyc_Absyn_Tqual tq=_T187A;struct Cyc_Parse_Type_specifier tss=_T1879;struct Cyc_List_List*atts=_T1878;_TC09=tss;_TC0A=yyyvsp;_TC0B=_TC0A[0];_TC0C=_TC0B.l;_TC0D=_TC0C.first_line;_TC0E=
Cyc_Position_loc_to_seg(_TC0D);{void*t=Cyc_Parse_speclist2typ(_TC09,_TC0E);_TC0F=yyyvsp;_TC10=& _TC0F[1].v;_TC11=(union Cyc_YYSTYPE*)_TC10;_TC12=
Cyc_yyget_YY31(_TC11);{struct Cyc_List_List*tms=_TC12.tms;
struct _tuple14 t_info=Cyc_Parse_apply_tms(tq,t,atts,tms);_TC13=t_info;_TC14=_TC13.f2;
if(_TC14==0)goto _TL2DC;_TC15=yyyvsp;_TC16=_TC15[1];_TC17=_TC16.l;_TC18=_TC17.first_line;_TC19=
Cyc_Position_loc_to_seg(_TC18);_TC1A=_tag_fat("bad type params, ignoring",sizeof(char),26U);_TC1B=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC19,_TC1A,_TC1B);goto _TL2DD;_TL2DC: _TL2DD: _TC1C=t_info;_TC1D=_TC1C.f3;
if(_TC1D==0)goto _TL2DE;_TC1E=yyyvsp;_TC1F=_TC1E[1];_TC20=_TC1F.l;_TC21=_TC20.first_line;_TC22=
Cyc_Position_loc_to_seg(_TC21);_TC23=_tag_fat("bad specifiers, ignoring",sizeof(char),25U);_TC24=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_TC22,_TC23,_TC24);goto _TL2DF;_TL2DE: _TL2DF:{struct _tuple8*_T187B=_cycalloc(sizeof(struct _tuple8));
_T187B->f0=0;_TC26=t_info;_T187B->f1=_TC26.f0;_TC27=t_info;_T187B->f2=_TC27.f1;_TC25=(struct _tuple8*)_T187B;}yyval=Cyc_YY38(_TC25);goto _LL0;}}}}case 309: _TC28=yyyvsp;_TC29=& _TC28[0].v;_TC2A=(union Cyc_YYSTYPE*)_TC29;_TC2B=
# 2474 "parse.y"
Cyc_yyget_YY38(_TC2A);_TC2C=yyyvsp;_TC2D=_TC2C[0];_TC2E=_TC2D.l;_TC2F=_TC2E.first_line;_TC30=Cyc_Position_loc_to_seg(_TC2F);_TC31=Cyc_Parse_type_name_to_type(_TC2B,_TC30);yyval=Cyc_YY45(_TC31);goto _LL0;case 310: _TC32=
# 2475 "parse.y"
Cyc_Absyn_join_eff(0);yyval=Cyc_YY45(_TC32);goto _LL0;case 311: _TC33=yyyvsp;_TC34=& _TC33[1].v;_TC35=(union Cyc_YYSTYPE*)_TC34;_TC36=
# 2476 "parse.y"
Cyc_yyget_YY41(_TC35);_TC37=Cyc_Absyn_join_eff(_TC36);yyval=Cyc_YY45(_TC37);goto _LL0;case 312: _TC38=yyyvsp;_TC39=& _TC38[2].v;_TC3A=(union Cyc_YYSTYPE*)_TC39;_TC3B=
# 2477 "parse.y"
Cyc_yyget_YY45(_TC3A);_TC3C=Cyc_Absyn_regionsof_eff(_TC3B);yyval=Cyc_YY45(_TC3C);goto _LL0;case 313:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TC3E=yyyvsp;_TC3F=& _TC3E[0].v;_TC40=(union Cyc_YYSTYPE*)_TC3F;
# 2478 "parse.y"
_T1877->hd=Cyc_yyget_YY45(_TC40);_TC41=yyyvsp;_TC42=& _TC41[2].v;_TC43=(union Cyc_YYSTYPE*)_TC42;_T1877->tl=Cyc_yyget_YY41(_TC43);_TC3D=(struct Cyc_List_List*)_T1877;}_TC44=Cyc_Absyn_join_eff(_TC3D);yyval=Cyc_YY45(_TC44);goto _LL0;case 314: _TC45=yyyvsp;_TC46=& _TC45[0].v;_TC47=(union Cyc_YYSTYPE*)_TC46;_TC48=
# 2479 "parse.y"
Cyc_yyget_YY45(_TC47);yyval=Cyc_YY45(_TC48);goto _LL0;case 315:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TC4A=yyyvsp;_TC4B=& _TC4A[0].v;_TC4C=(union Cyc_YYSTYPE*)_TC4B;
# 2485 "parse.y"
_T1877->hd=Cyc_yyget_YY45(_TC4C);_T1877->tl=0;_TC49=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY41(_TC49);goto _LL0;case 316:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_TC4E=yyyvsp;_TC4F=& _TC4E[2].v;_TC50=(union Cyc_YYSTYPE*)_TC4F;
# 2486 "parse.y"
_T1877->hd=Cyc_yyget_YY45(_TC50);_TC51=yyyvsp;_TC52=& _TC51[0].v;_TC53=(union Cyc_YYSTYPE*)_TC52;_T1877->tl=Cyc_yyget_YY41(_TC53);_TC4D=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY41(_TC4D);goto _LL0;case 317:{struct Cyc_Parse_Abstractdeclarator _T1877;_TC55=yyyvsp;_TC56=& _TC55[0].v;_TC57=(union Cyc_YYSTYPE*)_TC56;
# 2490 "parse.y"
_T1877.tms=Cyc_yyget_YY27(_TC57);_TC54=_T1877;}yyval=Cyc_YY31(_TC54);goto _LL0;case 318: _TC58=yyyvsp;_TC59=_TC58[0];
# 2491 "parse.y"
yyval=_TC59.v;goto _LL0;case 319:{struct Cyc_Parse_Abstractdeclarator _T1877;_TC5B=yyyvsp;_TC5C=& _TC5B[0].v;_TC5D=(union Cyc_YYSTYPE*)_TC5C;_TC5E=
# 2493 "parse.y"
Cyc_yyget_YY27(_TC5D);_TC5F=yyyvsp;_TC60=& _TC5F[1].v;_TC61=(union Cyc_YYSTYPE*)_TC60;_TC62=Cyc_yyget_YY31(_TC61);_TC63=_TC62.tms;_T1877.tms=Cyc_List_imp_append(_TC5E,_TC63);_TC5A=_T1877;}yyval=Cyc_YY31(_TC5A);goto _LL0;case 320: _TC64=yyyvsp;_TC65=_TC64[1];
# 2498 "parse.y"
yyval=_TC65.v;goto _LL0;case 321:{struct Cyc_Parse_Abstractdeclarator _T1877;_TC68=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_TC68,0U,sizeof(struct Cyc_List_List));_TC6A=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_TC6A,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1879->tag=0;_TC6B=yyyvsp;_TC6C=& _TC6B[2].v;_TC6D=(union Cyc_YYSTYPE*)_TC6C;
# 2500 "parse.y"
_T1879->f1=Cyc_yyget_YY54(_TC6D);_TC6E=yyyvsp;_TC6F=_TC6E[2];_TC70=_TC6F.l;_TC71=_TC70.first_line;_T1879->f2=Cyc_Position_loc_to_seg(_TC71);_TC69=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_TC69;_T1878->tl=0;_TC67=(struct Cyc_List_List*)_T1878;}_T1877.tms=_TC67;_TC66=_T1877;}yyval=Cyc_YY31(_TC66);goto _LL0;case 322:{struct Cyc_Parse_Abstractdeclarator _T1877;_TC74=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_TC74,0U,sizeof(struct Cyc_List_List));_TC76=yyr;{struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_TC76,0U,sizeof(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct));_T1879->tag=0;_TC77=yyyvsp;_TC78=& _TC77[3].v;_TC79=(union Cyc_YYSTYPE*)_TC78;
# 2502 "parse.y"
_T1879->f1=Cyc_yyget_YY54(_TC79);_TC7A=yyyvsp;_TC7B=_TC7A[3];_TC7C=_TC7B.l;_TC7D=_TC7C.first_line;_T1879->f2=Cyc_Position_loc_to_seg(_TC7D);_TC75=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_TC75;_TC7E=yyyvsp;_TC7F=& _TC7E[0].v;_TC80=(union Cyc_YYSTYPE*)_TC7F;_TC81=Cyc_yyget_YY31(_TC80);_T1878->tl=_TC81.tms;_TC73=(struct Cyc_List_List*)_T1878;}_T1877.tms=_TC73;_TC72=_T1877;}yyval=Cyc_YY31(_TC72);goto _LL0;case 323:{struct Cyc_Parse_Abstractdeclarator _T1877;_TC84=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_TC84,0U,sizeof(struct Cyc_List_List));_TC86=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_TC86,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1879->tag=1;_TC87=yyyvsp;_TC88=& _TC87[1].v;_TC89=(union Cyc_YYSTYPE*)_TC88;
# 2504 "parse.y"
_T1879->f1=Cyc_yyget_Exp_tok(_TC89);_TC8A=yyyvsp;_TC8B=& _TC8A[3].v;_TC8C=(union Cyc_YYSTYPE*)_TC8B;_T1879->f2=Cyc_yyget_YY54(_TC8C);_TC8D=yyyvsp;_TC8E=_TC8D[3];_TC8F=_TC8E.l;_TC90=_TC8F.first_line;_T1879->f3=Cyc_Position_loc_to_seg(_TC90);_TC85=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_TC85;_T1878->tl=0;_TC83=(struct Cyc_List_List*)_T1878;}_T1877.tms=_TC83;_TC82=_T1877;}yyval=Cyc_YY31(_TC82);goto _LL0;case 324:{struct Cyc_Parse_Abstractdeclarator _T1877;_TC93=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_TC93,0U,sizeof(struct Cyc_List_List));_TC95=yyr;{struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_TC95,0U,sizeof(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct));_T1879->tag=1;_TC96=yyyvsp;_TC97=& _TC96[2].v;_TC98=(union Cyc_YYSTYPE*)_TC97;
# 2506 "parse.y"
_T1879->f1=Cyc_yyget_Exp_tok(_TC98);_TC99=yyyvsp;_TC9A=& _TC99[4].v;_TC9B=(union Cyc_YYSTYPE*)_TC9A;_T1879->f2=Cyc_yyget_YY54(_TC9B);_TC9C=yyyvsp;_TC9D=_TC9C[4];_TC9E=_TC9D.l;_TC9F=_TC9E.first_line;_T1879->f3=Cyc_Position_loc_to_seg(_TC9F);_TC94=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_TC94;_TCA0=yyyvsp;_TCA1=& _TCA0[0].v;_TCA2=(union Cyc_YYSTYPE*)_TCA1;_TCA3=
Cyc_yyget_YY31(_TCA2);_T1878->tl=_TCA3.tms;_TC92=(struct Cyc_List_List*)_T1878;}
# 2506
_T1877.tms=_TC92;_TC91=_T1877;}yyval=Cyc_YY31(_TC91);goto _LL0;case 325: _TCA4=yyyvsp;_TCA5=& _TCA4[1].v;_TCA6=(union Cyc_YYSTYPE*)_TCA5;{
# 2510 "parse.y"
struct _tuple27*_T1877=Cyc_yyget_YY40(_TCA6);struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;void*_T187A;struct Cyc_Absyn_VarargInfo*_T187B;int _T187C;struct Cyc_List_List*_T187D;{struct _tuple27 _T187E=*_T1877;_T187D=_T187E.f0;_T187C=_T187E.f1;_T187B=_T187E.f2;_T187A=_T187E.f3;_T1879=_T187E.f4;_T1878=_T187E.f5;}{struct Cyc_List_List*lis=_T187D;int b=_T187C;struct Cyc_Absyn_VarargInfo*c=_T187B;void*eff=_T187A;struct Cyc_List_List*ec=_T1879;struct Cyc_List_List*qb=_T1878;_TCA7=yyyvsp;_TCA8=& _TCA7[3].v;_TCA9=(union Cyc_YYSTYPE*)_TCA8;{
struct _tuple21 _T187E=Cyc_yyget_YY62(_TCA9);struct Cyc_Absyn_Exp*_T187F;struct Cyc_Absyn_Exp*_T1880;struct Cyc_Absyn_Exp*_T1881;struct Cyc_Absyn_Exp*_T1882;_T1882=_T187E.f0;_T1881=_T187E.f1;_T1880=_T187E.f2;_T187F=_T187E.f3;{struct Cyc_Absyn_Exp*chk=_T1882;struct Cyc_Absyn_Exp*req=_T1881;struct Cyc_Absyn_Exp*ens=_T1880;struct Cyc_Absyn_Exp*thrws=_T187F;{struct Cyc_Parse_Abstractdeclarator _T1883;_TCAC=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_TCAC,0U,sizeof(struct Cyc_List_List));_TCAE=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_TCAE,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1885->tag=3;_TCB0=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1886=_region_malloc(_TCB0,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1886->tag=1;
_T1886->f1=lis;_T1886->f2=b;_T1886->f3=c;_T1886->f4=eff;_T1886->f5=ec;_T1886->f6=qb;_T1886->f7=chk;_T1886->f8=req;_T1886->f9=ens;_T1886->f10=thrws;_TCAF=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1886;}_T1885->f1=(void*)_TCAF;_TCAD=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_TCAD;_T1884->tl=0;_TCAB=(struct Cyc_List_List*)_T1884;}_T1883.tms=_TCAB;_TCAA=_T1883;}yyval=Cyc_YY31(_TCAA);goto _LL0;}}}}case 326: _TCB1=yyyvsp;_TCB2=& _TCB1[2].v;_TCB3=(union Cyc_YYSTYPE*)_TCB2;{
# 2515 "parse.y"
struct _tuple27*_T1877=Cyc_yyget_YY40(_TCB3);struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;void*_T187A;struct Cyc_Absyn_VarargInfo*_T187B;int _T187C;struct Cyc_List_List*_T187D;{struct _tuple27 _T187E=*_T1877;_T187D=_T187E.f0;_T187C=_T187E.f1;_T187B=_T187E.f2;_T187A=_T187E.f3;_T1879=_T187E.f4;_T1878=_T187E.f5;}{struct Cyc_List_List*lis=_T187D;int b=_T187C;struct Cyc_Absyn_VarargInfo*c=_T187B;void*eff=_T187A;struct Cyc_List_List*ec=_T1879;struct Cyc_List_List*qb=_T1878;_TCB4=yyyvsp;_TCB5=& _TCB4[4].v;_TCB6=(union Cyc_YYSTYPE*)_TCB5;{
struct _tuple21 _T187E=Cyc_yyget_YY62(_TCB6);struct Cyc_Absyn_Exp*_T187F;struct Cyc_Absyn_Exp*_T1880;struct Cyc_Absyn_Exp*_T1881;struct Cyc_Absyn_Exp*_T1882;_T1882=_T187E.f0;_T1881=_T187E.f1;_T1880=_T187E.f2;_T187F=_T187E.f3;{struct Cyc_Absyn_Exp*chk=_T1882;struct Cyc_Absyn_Exp*req=_T1881;struct Cyc_Absyn_Exp*ens=_T1880;struct Cyc_Absyn_Exp*thrws=_T187F;{struct Cyc_Parse_Abstractdeclarator _T1883;_TCB9=yyr;{struct Cyc_List_List*_T1884=_region_malloc(_TCB9,0U,sizeof(struct Cyc_List_List));_TCBB=yyr;{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_T1885=_region_malloc(_TCBB,0U,sizeof(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct));_T1885->tag=3;_TCBD=yyr;{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_T1886=_region_malloc(_TCBD,0U,sizeof(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct));_T1886->tag=1;
_T1886->f1=lis;
_T1886->f2=b;_T1886->f3=c;_T1886->f4=eff;_T1886->f5=ec;_T1886->f6=qb;_T1886->f7=chk;_T1886->f8=req;_T1886->f9=ens;_T1886->f10=thrws;_TCBC=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_T1886;}
# 2517
_T1885->f1=(void*)_TCBC;_TCBA=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_T1885;}_T1884->hd=(void*)_TCBA;_TCBE=yyyvsp;_TCBF=& _TCBE[0].v;_TCC0=(union Cyc_YYSTYPE*)_TCBF;_TCC1=
Cyc_yyget_YY31(_TCC0);_T1884->tl=_TCC1.tms;_TCB8=(struct Cyc_List_List*)_T1884;}
# 2517
_T1883.tms=_TCB8;_TCB7=_T1883;}yyval=Cyc_YY31(_TCB7);goto _LL0;}}}}case 327: _TCC3=Cyc_List_map_c;{
# 2522
struct Cyc_List_List*(*_T1877)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_TCC3;_TCC2=_T1877;}_TCC4=yyyvsp;_TCC5=_TCC4[1];_TCC6=_TCC5.l;_TCC7=_TCC6.first_line;_TCC8=Cyc_Position_loc_to_seg(_TCC7);_TCC9=yyyvsp;_TCCA=& _TCC9[2].v;_TCCB=(union Cyc_YYSTYPE*)_TCCA;_TCCC=Cyc_yyget_YY41(_TCCB);_TCCD=Cyc_List_imp_rev(_TCCC);{struct Cyc_List_List*ts=_TCC2(Cyc_Parse_typ2tvar,_TCC8,_TCCD);{struct Cyc_Parse_Abstractdeclarator _T1877;_TCD0=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_TCD0,0U,sizeof(struct Cyc_List_List));_TCD2=yyr;{struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_TCD2,0U,sizeof(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct));_T1879->tag=4;
_T1879->f1=ts;_TCD3=yyyvsp;_TCD4=_TCD3[1];_TCD5=_TCD4.l;_TCD6=_TCD5.first_line;_T1879->f2=Cyc_Position_loc_to_seg(_TCD6);_T1879->f3=0;_TCD1=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_TCD1;_TCD7=yyyvsp;_TCD8=& _TCD7[0].v;_TCD9=(union Cyc_YYSTYPE*)_TCD8;_TCDA=
Cyc_yyget_YY31(_TCD9);_T1878->tl=_TCDA.tms;_TCCF=(struct Cyc_List_List*)_T1878;}
# 2523
_T1877.tms=_TCCF;_TCCE=_T1877;}yyval=Cyc_YY31(_TCCE);goto _LL0;}case 328:{struct Cyc_Parse_Abstractdeclarator _T1877;_TCDD=yyr;{struct Cyc_List_List*_T1878=_region_malloc(_TCDD,0U,sizeof(struct Cyc_List_List));_TCDF=yyr;{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_T1879=_region_malloc(_TCDF,0U,sizeof(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct));_T1879->tag=5;_TCE0=yyyvsp;_TCE1=_TCE0[1];_TCE2=_TCE1.l;_TCE3=_TCE2.first_line;
# 2527 "parse.y"
_T1879->f1=Cyc_Position_loc_to_seg(_TCE3);_TCE4=yyyvsp;_TCE5=& _TCE4[1].v;_TCE6=(union Cyc_YYSTYPE*)_TCE5;_T1879->f2=Cyc_yyget_YY46(_TCE6);_TCDE=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_T1879;}_T1878->hd=(void*)_TCDE;_TCE7=yyyvsp;_TCE8=& _TCE7[0].v;_TCE9=(union Cyc_YYSTYPE*)_TCE8;_TCEA=Cyc_yyget_YY31(_TCE9);_T1878->tl=_TCEA.tms;_TCDC=(struct Cyc_List_List*)_T1878;}_T1877.tms=_TCDC;_TCDB=_T1877;}yyval=Cyc_YY31(_TCDB);goto _LL0;case 329:{struct _tuple21 _T1877;_TCEC=yyyvsp;_TCED=& _TCEC[2].v;_TCEE=(union Cyc_YYSTYPE*)_TCED;
# 2533 "parse.y"
_T1877.f0=Cyc_yyget_Exp_tok(_TCEE);_T1877.f1=0;_T1877.f2=0;_T1877.f3=0;_TCEB=_T1877;}yyval=Cyc_YY62(_TCEB);goto _LL0;case 330:{struct _tuple21 _T1877;
# 2535 "parse.y"
_T1877.f0=0;_TCF0=yyyvsp;_TCF1=& _TCF0[2].v;_TCF2=(union Cyc_YYSTYPE*)_TCF1;_T1877.f1=Cyc_yyget_Exp_tok(_TCF2);_T1877.f2=0;_T1877.f3=0;_TCEF=_T1877;}yyval=Cyc_YY62(_TCEF);goto _LL0;case 331:{struct _tuple21 _T1877;
# 2537 "parse.y"
_T1877.f0=0;_T1877.f1=0;_TCF4=yyyvsp;_TCF5=& _TCF4[2].v;_TCF6=(union Cyc_YYSTYPE*)_TCF5;_T1877.f2=Cyc_yyget_Exp_tok(_TCF6);_T1877.f3=0;_TCF3=_T1877;}yyval=Cyc_YY62(_TCF3);goto _LL0;case 332:{struct _tuple21 _T1877;
# 2539 "parse.y"
_T1877.f0=0;_T1877.f1=0;_T1877.f2=0;_TCF8=yyyvsp;_TCF9=& _TCF8[2].v;_TCFA=(union Cyc_YYSTYPE*)_TCF9;_T1877.f3=Cyc_yyget_Exp_tok(_TCFA);_TCF7=_T1877;}yyval=Cyc_YY62(_TCF7);goto _LL0;case 333:{struct _tuple21 _T1877;_TCFC=yyyvsp;_TCFD=& _TCFC[2].v;_TCFE=(union Cyc_YYSTYPE*)_TCFD;
# 2541 "parse.y"
_T1877.f0=Cyc_yyget_Exp_tok(_TCFE);_T1877.f1=0;_T1877.f2=0;_T1877.f3=0;_TCFB=_T1877;}_TCFF=yyyvsp;_TD00=& _TCFF[4].v;_TD01=(union Cyc_YYSTYPE*)_TD00;_TD02=Cyc_yyget_YY62(_TD01);_TD03=Cyc_Parse_join_assns(_TCFB,_TD02);yyval=Cyc_YY62(_TD03);goto _LL0;case 334:{struct _tuple21 _T1877;
# 2543 "parse.y"
_T1877.f0=0;_TD05=yyyvsp;_TD06=& _TD05[2].v;_TD07=(union Cyc_YYSTYPE*)_TD06;_T1877.f1=Cyc_yyget_Exp_tok(_TD07);_T1877.f2=0;_T1877.f3=0;_TD04=_T1877;}_TD08=yyyvsp;_TD09=& _TD08[4].v;_TD0A=(union Cyc_YYSTYPE*)_TD09;_TD0B=Cyc_yyget_YY62(_TD0A);_TD0C=Cyc_Parse_join_assns(_TD04,_TD0B);yyval=Cyc_YY62(_TD0C);goto _LL0;case 335:{struct _tuple21 _T1877;
# 2545 "parse.y"
_T1877.f0=0;_T1877.f1=0;_TD0E=yyyvsp;_TD0F=& _TD0E[2].v;_TD10=(union Cyc_YYSTYPE*)_TD0F;_T1877.f2=Cyc_yyget_Exp_tok(_TD10);_T1877.f3=0;_TD0D=_T1877;}_TD11=yyyvsp;_TD12=& _TD11[4].v;_TD13=(union Cyc_YYSTYPE*)_TD12;_TD14=Cyc_yyget_YY62(_TD13);_TD15=Cyc_Parse_join_assns(_TD0D,_TD14);yyval=Cyc_YY62(_TD15);goto _LL0;case 336:{struct _tuple21 _T1877;
# 2547 "parse.y"
_T1877.f0=0;_T1877.f1=0;_T1877.f2=0;_TD17=yyyvsp;_TD18=& _TD17[2].v;_TD19=(union Cyc_YYSTYPE*)_TD18;_T1877.f3=Cyc_yyget_Exp_tok(_TD19);_TD16=_T1877;}_TD1A=yyyvsp;_TD1B=& _TD1A[4].v;_TD1C=(union Cyc_YYSTYPE*)_TD1B;_TD1D=Cyc_yyget_YY62(_TD1C);_TD1E=Cyc_Parse_join_assns(_TD16,_TD1D);yyval=Cyc_YY62(_TD1E);goto _LL0;case 337:{struct _tuple21 _T1877;
# 2551 "parse.y"
_T1877.f0=0;_T1877.f1=0;_T1877.f2=0;_T1877.f3=0;_TD1F=_T1877;}yyval=Cyc_YY62(_TD1F);goto _LL0;case 338: _TD20=yyyvsp;_TD21=_TD20[0];
# 2552 "parse.y"
yyval=_TD21.v;goto _LL0;case 339: _TD22=yyyvsp;_TD23=_TD22[0];
# 2557 "parse.y"
yyval=_TD23.v;goto _LL0;case 340: _TD24=yyyvsp;_TD25=_TD24[0];
# 2558 "parse.y"
yyval=_TD25.v;goto _LL0;case 341: _TD26=yyyvsp;_TD27=_TD26[0];
# 2559 "parse.y"
yyval=_TD27.v;goto _LL0;case 342: _TD28=yyyvsp;_TD29=_TD28[0];
# 2560 "parse.y"
yyval=_TD29.v;goto _LL0;case 343: _TD2A=yyyvsp;_TD2B=_TD2A[0];
# 2561 "parse.y"
yyval=_TD2B.v;goto _LL0;case 344: _TD2C=yyyvsp;_TD2D=_TD2C[0];
# 2562 "parse.y"
yyval=_TD2D.v;goto _LL0;case 345:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1877->tag=13;{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_TD30=yyyvsp;_TD31=& _TD30[0].v;_TD32=(union Cyc_YYSTYPE*)_TD31;
# 2568 "parse.y"
*_T1878=Cyc_yyget_String_tok(_TD32);_TD2F=(struct _fat_ptr*)_T1878;}_T1877->f1=_TD2F;_TD33=yyyvsp;_TD34=& _TD33[2].v;_TD35=(union Cyc_YYSTYPE*)_TD34;_T1877->f2=Cyc_yyget_Stmt_tok(_TD35);_TD2E=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1877;}_TD36=(void*)_TD2E;_TD37=yyyvsp;_TD38=_TD37[0];_TD39=_TD38.l;_TD3A=_TD39.first_line;_TD3B=Cyc_Position_loc_to_seg(_TD3A);_TD3C=Cyc_Absyn_new_stmt(_TD36,_TD3B);yyval=Cyc_Stmt_tok(_TD3C);goto _LL0;case 346: _TD3D=yyyvsp;_TD3E=_TD3D[0];_TD3F=_TD3E.l;_TD40=_TD3F.first_line;_TD41=
# 2572 "parse.y"
Cyc_Position_loc_to_seg(_TD40);_TD42=Cyc_Absyn_skip_stmt(_TD41);yyval=Cyc_Stmt_tok(_TD42);goto _LL0;case 347: _TD43=yyyvsp;_TD44=& _TD43[0].v;_TD45=(union Cyc_YYSTYPE*)_TD44;_TD46=
# 2573 "parse.y"
Cyc_yyget_Exp_tok(_TD45);_TD47=yyyvsp;_TD48=_TD47[0];_TD49=_TD48.l;_TD4A=_TD49.first_line;_TD4B=Cyc_Position_loc_to_seg(_TD4A);_TD4C=Cyc_Absyn_exp_stmt(_TD46,_TD4B);yyval=Cyc_Stmt_tok(_TD4C);goto _LL0;case 348:
# 2578 "parse.y"
 Cyc_Parse_inside_function_definition=1;goto _LL0;case 349:
# 2582 "parse.y"
 Cyc_Parse_inside_function_definition=0;goto _LL0;case 350: _TD4D=yyyvsp;_TD4E=_TD4D[0];_TD4F=_TD4E.l;_TD50=_TD4F.first_line;_TD51=
# 2586 "parse.y"
Cyc_Position_loc_to_seg(_TD50);_TD52=Cyc_Absyn_skip_stmt(_TD51);yyval=Cyc_Stmt_tok(_TD52);goto _LL0;case 351: _TD53=yyyvsp;_TD54=_TD53[1];
# 2587 "parse.y"
yyval=_TD54.v;goto _LL0;case 352: _TD55=yyyvsp;_TD56=_TD55[0];_TD57=_TD56.l;_TD58=_TD57.first_line;_TD59=
# 2592 "parse.y"
Cyc_Position_loc_to_seg(_TD58);_TD5A=Cyc_Absyn_skip_stmt(_TD59);yyval=Cyc_Stmt_tok(_TD5A);goto _LL0;case 353: _TD5B=yyyvsp;_TD5C=_TD5B[1];
# 2593 "parse.y"
yyval=_TD5C.v;goto _LL0;case 354: _TD5D=yyyvsp;_TD5E=& _TD5D[0].v;_TD5F=(union Cyc_YYSTYPE*)_TD5E;_TD60=
# 2598 "parse.y"
Cyc_yyget_YY16(_TD5F);_TD61=yyyvsp;_TD62=_TD61[0];_TD63=_TD62.l;_TD64=_TD63.first_line;_TD65=Cyc_Position_loc_to_seg(_TD64);_TD66=Cyc_Absyn_skip_stmt(_TD65);_TD67=Cyc_Parse_flatten_declarations(_TD60,_TD66);yyval=Cyc_Stmt_tok(_TD67);goto _LL0;case 355: _TD68=yyyvsp;_TD69=& _TD68[0].v;_TD6A=(union Cyc_YYSTYPE*)_TD69;_TD6B=
# 2599 "parse.y"
Cyc_yyget_YY16(_TD6A);_TD6C=yyyvsp;_TD6D=& _TD6C[1].v;_TD6E=(union Cyc_YYSTYPE*)_TD6D;_TD6F=Cyc_yyget_Stmt_tok(_TD6E);_TD70=Cyc_Parse_flatten_declarations(_TD6B,_TD6F);yyval=Cyc_Stmt_tok(_TD70);goto _LL0;case 356:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1877->tag=13;{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_TD73=yyyvsp;_TD74=& _TD73[0].v;_TD75=(union Cyc_YYSTYPE*)_TD74;
# 2601 "parse.y"
*_T1878=Cyc_yyget_String_tok(_TD75);_TD72=(struct _fat_ptr*)_T1878;}_T1877->f1=_TD72;_TD76=yyyvsp;_TD77=& _TD76[2].v;_TD78=(union Cyc_YYSTYPE*)_TD77;_TD79=Cyc_yyget_YY16(_TD78);_TD7A=Cyc_Absyn_skip_stmt(0U);_T1877->f2=Cyc_Parse_flatten_declarations(_TD79,_TD7A);_TD71=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1877;}_TD7B=(void*)_TD71;_TD7C=yyyvsp;_TD7D=_TD7C[0];_TD7E=_TD7D.l;_TD7F=_TD7E.first_line;_TD80=Cyc_Position_loc_to_seg(_TD7F);_TD81=Cyc_Absyn_new_stmt(_TD7B,_TD80);yyval=Cyc_Stmt_tok(_TD81);goto _LL0;case 357:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct));_T1877->tag=13;{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_TD84=yyyvsp;_TD85=& _TD84[0].v;_TD86=(union Cyc_YYSTYPE*)_TD85;
# 2603 "parse.y"
*_T1878=Cyc_yyget_String_tok(_TD86);_TD83=(struct _fat_ptr*)_T1878;}_T1877->f1=_TD83;_TD87=yyyvsp;_TD88=& _TD87[2].v;_TD89=(union Cyc_YYSTYPE*)_TD88;_TD8A=Cyc_yyget_YY16(_TD89);_TD8B=yyyvsp;_TD8C=& _TD8B[3].v;_TD8D=(union Cyc_YYSTYPE*)_TD8C;_TD8E=Cyc_yyget_Stmt_tok(_TD8D);_T1877->f2=Cyc_Parse_flatten_declarations(_TD8A,_TD8E);_TD82=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T1877;}_TD8F=(void*)_TD82;_TD90=yyyvsp;_TD91=_TD90[0];_TD92=_TD91.l;_TD93=_TD92.first_line;_TD94=Cyc_Position_loc_to_seg(_TD93);_TD95=Cyc_Absyn_new_stmt(_TD8F,_TD94);yyval=Cyc_Stmt_tok(_TD95);goto _LL0;case 358: _TD96=yyyvsp;_TD97=_TD96[0];
# 2604 "parse.y"
yyval=_TD97.v;goto _LL0;case 359: _TD98=yyyvsp;_TD99=& _TD98[0].v;_TD9A=(union Cyc_YYSTYPE*)_TD99;_TD9B=
# 2605 "parse.y"
Cyc_yyget_Stmt_tok(_TD9A);_TD9C=yyyvsp;_TD9D=& _TD9C[1].v;_TD9E=(union Cyc_YYSTYPE*)_TD9D;_TD9F=Cyc_yyget_Stmt_tok(_TD9E);_TDA0=yyyvsp;_TDA1=_TDA0[0];_TDA2=_TDA1.l;_TDA3=_TDA2.first_line;_TDA4=Cyc_Position_loc_to_seg(_TDA3);_TDA5=Cyc_Absyn_seq_stmt(_TD9B,_TD9F,_TDA4);yyval=Cyc_Stmt_tok(_TDA5);goto _LL0;case 360:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1877->tag=1;_TDA7=yyyvsp;_TDA8=& _TDA7[0].v;_TDA9=(union Cyc_YYSTYPE*)_TDA8;
# 2606 "parse.y"
_T1877->f1=Cyc_yyget_YY15(_TDA9);_TDA6=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1877;}_TDAA=(void*)_TDA6;_TDAB=yyyvsp;_TDAC=_TDAB[0];_TDAD=_TDAC.l;_TDAE=_TDAD.first_line;_TDAF=Cyc_Position_loc_to_seg(_TDAE);_TDB0=Cyc_Absyn_new_decl(_TDAA,_TDAF);_TDB1=
Cyc_Absyn_skip_stmt(0U);_TDB2=
# 2606
Cyc_Parse_flatten_decl(_TDB0,_TDB1);yyval=Cyc_Stmt_tok(_TDB2);goto _LL0;case 361:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct));_T1877->tag=1;_TDB4=yyyvsp;_TDB5=& _TDB4[0].v;_TDB6=(union Cyc_YYSTYPE*)_TDB5;
# 2609 "parse.y"
_T1877->f1=Cyc_yyget_YY15(_TDB6);_TDB3=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T1877;}_TDB7=(void*)_TDB3;_TDB8=yyyvsp;_TDB9=_TDB8[0];_TDBA=_TDB9.l;_TDBB=_TDBA.first_line;_TDBC=Cyc_Position_loc_to_seg(_TDBB);_TDBD=Cyc_Absyn_new_decl(_TDB7,_TDBC);_TDBE=yyyvsp;_TDBF=& _TDBE[1].v;_TDC0=(union Cyc_YYSTYPE*)_TDBF;_TDC1=Cyc_yyget_Stmt_tok(_TDC0);_TDC2=Cyc_Parse_flatten_decl(_TDBD,_TDC1);yyval=Cyc_Stmt_tok(_TDC2);goto _LL0;case 362: _TDC3=yyyvsp;_TDC4=& _TDC3[2].v;_TDC5=(union Cyc_YYSTYPE*)_TDC4;_TDC6=
# 2614 "parse.y"
Cyc_yyget_Exp_tok(_TDC5);_TDC7=yyyvsp;_TDC8=& _TDC7[4].v;_TDC9=(union Cyc_YYSTYPE*)_TDC8;_TDCA=Cyc_yyget_Stmt_tok(_TDC9);_TDCB=Cyc_Absyn_skip_stmt(0U);_TDCC=yyyvsp;_TDCD=_TDCC[0];_TDCE=_TDCD.l;_TDCF=_TDCE.first_line;_TDD0=Cyc_Position_loc_to_seg(_TDCF);_TDD1=Cyc_Absyn_ifthenelse_stmt(_TDC6,_TDCA,_TDCB,_TDD0);yyval=Cyc_Stmt_tok(_TDD1);goto _LL0;case 363: _TDD2=yyyvsp;_TDD3=& _TDD2[2].v;_TDD4=(union Cyc_YYSTYPE*)_TDD3;_TDD5=
# 2616 "parse.y"
Cyc_yyget_Exp_tok(_TDD4);_TDD6=yyyvsp;_TDD7=& _TDD6[4].v;_TDD8=(union Cyc_YYSTYPE*)_TDD7;_TDD9=Cyc_yyget_Stmt_tok(_TDD8);_TDDA=yyyvsp;_TDDB=& _TDDA[6].v;_TDDC=(union Cyc_YYSTYPE*)_TDDB;_TDDD=Cyc_yyget_Stmt_tok(_TDDC);_TDDE=yyyvsp;_TDDF=_TDDE[0];_TDE0=_TDDF.l;_TDE1=_TDE0.first_line;_TDE2=Cyc_Position_loc_to_seg(_TDE1);_TDE3=Cyc_Absyn_ifthenelse_stmt(_TDD5,_TDD9,_TDDD,_TDE2);yyval=Cyc_Stmt_tok(_TDE3);goto _LL0;case 364: _TDE4=yyyvsp;_TDE5=& _TDE4[2].v;_TDE6=(union Cyc_YYSTYPE*)_TDE5;_TDE7=
# 2622 "parse.y"
Cyc_yyget_Exp_tok(_TDE6);_TDE8=yyyvsp;_TDE9=& _TDE8[5].v;_TDEA=(union Cyc_YYSTYPE*)_TDE9;_TDEB=Cyc_yyget_YY8(_TDEA);_TDEC=yyyvsp;_TDED=_TDEC[0];_TDEE=_TDED.l;_TDEF=_TDEE.first_line;_TDF0=Cyc_Position_loc_to_seg(_TDEF);_TDF1=Cyc_Absyn_switch_stmt(_TDE7,_TDEB,_TDF0);yyval=Cyc_Stmt_tok(_TDF1);goto _LL0;case 365: _TDF2=yyyvsp;_TDF3=& _TDF2[1].v;_TDF4=(union Cyc_YYSTYPE*)_TDF3;_TDF5=
# 2625
Cyc_yyget_QualId_tok(_TDF4);_TDF6=yyyvsp;_TDF7=_TDF6[1];_TDF8=_TDF7.l;_TDF9=_TDF8.first_line;_TDFA=Cyc_Position_loc_to_seg(_TDF9);_TDFB=Cyc_Absyn_unknownid_exp(_TDF5,_TDFA);_TDFC=yyyvsp;_TDFD=& _TDFC[3].v;_TDFE=(union Cyc_YYSTYPE*)_TDFD;_TDFF=Cyc_yyget_YY8(_TDFE);_TE00=yyyvsp;_TE01=_TE00[0];_TE02=_TE01.l;_TE03=_TE02.first_line;_TE04=Cyc_Position_loc_to_seg(_TE03);_TE05=Cyc_Absyn_switch_stmt(_TDFB,_TDFF,_TE04);yyval=Cyc_Stmt_tok(_TE05);goto _LL0;case 366: _TE06=yyyvsp;_TE07=& _TE06[3].v;_TE08=(union Cyc_YYSTYPE*)_TE07;_TE09=
# 2628
Cyc_yyget_YY4(_TE08);_TE0A=yyyvsp;_TE0B=_TE0A[1];_TE0C=_TE0B.l;_TE0D=_TE0C.first_line;_TE0E=Cyc_Position_loc_to_seg(_TE0D);_TE0F=Cyc_Absyn_tuple_exp(_TE09,_TE0E);_TE10=yyyvsp;_TE11=& _TE10[6].v;_TE12=(union Cyc_YYSTYPE*)_TE11;_TE13=Cyc_yyget_YY8(_TE12);_TE14=yyyvsp;_TE15=_TE14[0];_TE16=_TE15.l;_TE17=_TE16.first_line;_TE18=Cyc_Position_loc_to_seg(_TE17);_TE19=Cyc_Absyn_switch_stmt(_TE0F,_TE13,_TE18);yyval=Cyc_Stmt_tok(_TE19);goto _LL0;case 367: _TE1A=yyyvsp;_TE1B=& _TE1A[1].v;_TE1C=(union Cyc_YYSTYPE*)_TE1B;_TE1D=
# 2632 "parse.y"
Cyc_yyget_Stmt_tok(_TE1C);_TE1E=yyyvsp;_TE1F=& _TE1E[4].v;_TE20=(union Cyc_YYSTYPE*)_TE1F;_TE21=Cyc_yyget_YY8(_TE20);_TE22=yyyvsp;_TE23=_TE22[0];_TE24=_TE23.l;_TE25=_TE24.first_line;_TE26=Cyc_Position_loc_to_seg(_TE25);_TE27=Cyc_Absyn_trycatch_stmt(_TE1D,_TE21,_TE26);yyval=Cyc_Stmt_tok(_TE27);goto _LL0;case 368:
# 2646 "parse.y"
 yyval=Cyc_YY8(0);goto _LL0;case 369:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE2A=& Cyc_Absyn_Wild_p_val;_TE2B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_TE2A;_TE2C=(void*)_TE2B;_TE2D=yyyvsp;_TE2E=_TE2D[0];_TE2F=_TE2E.l;_TE30=_TE2F.first_line;_TE31=
# 2649 "parse.y"
Cyc_Position_loc_to_seg(_TE30);_T1878->pattern=Cyc_Absyn_new_pat(_TE2C,_TE31);_T1878->pat_vars=0;
_T1878->where_clause=0;_TE32=yyyvsp;_TE33=& _TE32[2].v;_TE34=(union Cyc_YYSTYPE*)_TE33;_T1878->body=Cyc_yyget_Stmt_tok(_TE34);_TE35=yyyvsp;_TE36=_TE35[0];_TE37=_TE36.l;_TE38=_TE37.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_TE38);_TE29=(struct Cyc_Absyn_Switch_clause*)_T1878;}
# 2649
_T1877->hd=_TE29;_TE39=yyyvsp;_TE3A=& _TE39[3].v;_TE3B=(union Cyc_YYSTYPE*)_TE3A;
_T1877->tl=Cyc_yyget_YY8(_TE3B);_TE28=(struct Cyc_List_List*)_T1877;}
# 2649
yyval=Cyc_YY8(_TE28);goto _LL0;case 370:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE3E=yyyvsp;_TE3F=& _TE3E[1].v;_TE40=(union Cyc_YYSTYPE*)_TE3F;
# 2652 "parse.y"
_T1878->pattern=Cyc_yyget_YY9(_TE40);_T1878->pat_vars=0;_T1878->where_clause=0;_TE41=yyyvsp;_TE42=_TE41[2];_TE43=_TE42.l;_TE44=_TE43.first_line;_TE45=
Cyc_Position_loc_to_seg(_TE44);_T1878->body=Cyc_Absyn_fallthru_stmt(0,_TE45);_TE46=yyyvsp;_TE47=_TE46[0];_TE48=_TE47.l;_TE49=_TE48.first_line;
_T1878->loc=Cyc_Position_loc_to_seg(_TE49);_TE3D=(struct Cyc_Absyn_Switch_clause*)_T1878;}
# 2652
_T1877->hd=_TE3D;_TE4A=yyyvsp;_TE4B=& _TE4A[3].v;_TE4C=(union Cyc_YYSTYPE*)_TE4B;
# 2654
_T1877->tl=Cyc_yyget_YY8(_TE4C);_TE3C=(struct Cyc_List_List*)_T1877;}
# 2652
yyval=Cyc_YY8(_TE3C);goto _LL0;case 371:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE4F=yyyvsp;_TE50=& _TE4F[1].v;_TE51=(union Cyc_YYSTYPE*)_TE50;
# 2656 "parse.y"
_T1878->pattern=Cyc_yyget_YY9(_TE51);_T1878->pat_vars=0;_T1878->where_clause=0;_TE52=yyyvsp;_TE53=& _TE52[3].v;_TE54=(union Cyc_YYSTYPE*)_TE53;_T1878->body=Cyc_yyget_Stmt_tok(_TE54);_TE55=yyyvsp;_TE56=_TE55[0];_TE57=_TE56.l;_TE58=_TE57.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_TE58);_TE4E=(struct Cyc_Absyn_Switch_clause*)_T1878;}_T1877->hd=_TE4E;_TE59=yyyvsp;_TE5A=& _TE59[4].v;_TE5B=(union Cyc_YYSTYPE*)_TE5A;_T1877->tl=Cyc_yyget_YY8(_TE5B);_TE4D=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY8(_TE4D);goto _LL0;case 372:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE5E=yyyvsp;_TE5F=& _TE5E[1].v;_TE60=(union Cyc_YYSTYPE*)_TE5F;
# 2658 "parse.y"
_T1878->pattern=Cyc_yyget_YY9(_TE60);_T1878->pat_vars=0;_TE61=yyyvsp;_TE62=& _TE61[3].v;_TE63=(union Cyc_YYSTYPE*)_TE62;_T1878->where_clause=Cyc_yyget_Exp_tok(_TE63);_TE64=yyyvsp;_TE65=_TE64[4];_TE66=_TE65.l;_TE67=_TE66.first_line;_TE68=
Cyc_Position_loc_to_seg(_TE67);_T1878->body=Cyc_Absyn_fallthru_stmt(0,_TE68);_TE69=yyyvsp;_TE6A=_TE69[0];_TE6B=_TE6A.l;_TE6C=_TE6B.first_line;
_T1878->loc=Cyc_Position_loc_to_seg(_TE6C);_TE5D=(struct Cyc_Absyn_Switch_clause*)_T1878;}
# 2658
_T1877->hd=_TE5D;_TE6D=yyyvsp;_TE6E=& _TE6D[5].v;_TE6F=(union Cyc_YYSTYPE*)_TE6E;
# 2660
_T1877->tl=Cyc_yyget_YY8(_TE6F);_TE5C=(struct Cyc_List_List*)_T1877;}
# 2658
yyval=Cyc_YY8(_TE5C);goto _LL0;case 373:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_Switch_clause*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_TE72=yyyvsp;_TE73=& _TE72[1].v;_TE74=(union Cyc_YYSTYPE*)_TE73;
# 2662 "parse.y"
_T1878->pattern=Cyc_yyget_YY9(_TE74);_T1878->pat_vars=0;_TE75=yyyvsp;_TE76=& _TE75[3].v;_TE77=(union Cyc_YYSTYPE*)_TE76;_T1878->where_clause=Cyc_yyget_Exp_tok(_TE77);_TE78=yyyvsp;_TE79=& _TE78[5].v;_TE7A=(union Cyc_YYSTYPE*)_TE79;_T1878->body=Cyc_yyget_Stmt_tok(_TE7A);_TE7B=yyyvsp;_TE7C=_TE7B[0];_TE7D=_TE7C.l;_TE7E=_TE7D.first_line;_T1878->loc=Cyc_Position_loc_to_seg(_TE7E);_TE71=(struct Cyc_Absyn_Switch_clause*)_T1878;}_T1877->hd=_TE71;_TE7F=yyyvsp;_TE80=& _TE7F[6].v;_TE81=(union Cyc_YYSTYPE*)_TE80;_T1877->tl=Cyc_yyget_YY8(_TE81);_TE70=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY8(_TE70);goto _LL0;case 374: _TE82=yyyvsp;_TE83=& _TE82[2].v;_TE84=(union Cyc_YYSTYPE*)_TE83;_TE85=
# 2669 "parse.y"
Cyc_yyget_Exp_tok(_TE84);_TE86=yyyvsp;_TE87=& _TE86[4].v;_TE88=(union Cyc_YYSTYPE*)_TE87;_TE89=Cyc_yyget_Stmt_tok(_TE88);_TE8A=yyyvsp;_TE8B=_TE8A[0];_TE8C=_TE8B.l;_TE8D=_TE8C.first_line;_TE8E=Cyc_Position_loc_to_seg(_TE8D);_TE8F=Cyc_Absyn_while_stmt(_TE85,_TE89,_TE8E);yyval=Cyc_Stmt_tok(_TE8F);goto _LL0;case 375: _TE90=yyyvsp;_TE91=& _TE90[1].v;_TE92=(union Cyc_YYSTYPE*)_TE91;_TE93=
# 2671 "parse.y"
Cyc_yyget_Stmt_tok(_TE92);_TE94=yyyvsp;_TE95=& _TE94[4].v;_TE96=(union Cyc_YYSTYPE*)_TE95;_TE97=Cyc_yyget_Exp_tok(_TE96);_TE98=yyyvsp;_TE99=_TE98[0];_TE9A=_TE99.l;_TE9B=_TE9A.first_line;_TE9C=Cyc_Position_loc_to_seg(_TE9B);_TE9D=Cyc_Absyn_do_stmt(_TE93,_TE97,_TE9C);yyval=Cyc_Stmt_tok(_TE9D);goto _LL0;case 376: _TE9E=yyyvsp;_TE9F=& _TE9E[2].v;_TEA0=(union Cyc_YYSTYPE*)_TE9F;_TEA1=
# 2673 "parse.y"
Cyc_yyget_Exp_tok(_TEA0);_TEA2=yyyvsp;_TEA3=& _TEA2[4].v;_TEA4=(union Cyc_YYSTYPE*)_TEA3;_TEA5=Cyc_yyget_Exp_tok(_TEA4);_TEA6=yyyvsp;_TEA7=& _TEA6[6].v;_TEA8=(union Cyc_YYSTYPE*)_TEA7;_TEA9=Cyc_yyget_Exp_tok(_TEA8);_TEAA=yyyvsp;_TEAB=& _TEAA[8].v;_TEAC=(union Cyc_YYSTYPE*)_TEAB;_TEAD=Cyc_yyget_Stmt_tok(_TEAC);_TEAE=yyyvsp;_TEAF=_TEAE[0];_TEB0=_TEAF.l;_TEB1=_TEB0.first_line;_TEB2=Cyc_Position_loc_to_seg(_TEB1);_TEB3=Cyc_Absyn_for_stmt(_TEA1,_TEA5,_TEA9,_TEAD,_TEB2);yyval=Cyc_Stmt_tok(_TEB3);goto _LL0;case 377: _TEB4=
# 2675 "parse.y"
Cyc_Absyn_false_exp(0U);_TEB5=yyyvsp;_TEB6=& _TEB5[3].v;_TEB7=(union Cyc_YYSTYPE*)_TEB6;_TEB8=Cyc_yyget_Exp_tok(_TEB7);_TEB9=yyyvsp;_TEBA=& _TEB9[5].v;_TEBB=(union Cyc_YYSTYPE*)_TEBA;_TEBC=Cyc_yyget_Exp_tok(_TEBB);_TEBD=yyyvsp;_TEBE=& _TEBD[7].v;_TEBF=(union Cyc_YYSTYPE*)_TEBE;_TEC0=Cyc_yyget_Stmt_tok(_TEBF);_TEC1=yyyvsp;_TEC2=_TEC1[0];_TEC3=_TEC2.l;_TEC4=_TEC3.first_line;_TEC5=Cyc_Position_loc_to_seg(_TEC4);{struct Cyc_Absyn_Stmt*s=Cyc_Absyn_for_stmt(_TEB4,_TEB8,_TEBC,_TEC0,_TEC5);_TEC6=yyyvsp;_TEC7=& _TEC6[2].v;_TEC8=(union Cyc_YYSTYPE*)_TEC7;_TEC9=
Cyc_yyget_YY16(_TEC8);_TECA=s;_TECB=Cyc_Parse_flatten_declarations(_TEC9,_TECA);yyval=Cyc_Stmt_tok(_TECB);goto _LL0;}case 378: _TECC=
# 2679
Cyc_Absyn_true_exp(0U);yyval=Cyc_Exp_tok(_TECC);goto _LL0;case 379: _TECD=yyyvsp;_TECE=_TECD[0];
# 2680 "parse.y"
yyval=_TECE.v;goto _LL0;case 380:{struct _fat_ptr*_T1877=_cycalloc(sizeof(struct _fat_ptr));_TED0=yyyvsp;_TED1=& _TED0[1].v;_TED2=(union Cyc_YYSTYPE*)_TED1;
# 2683
*_T1877=Cyc_yyget_String_tok(_TED2);_TECF=(struct _fat_ptr*)_T1877;}_TED3=yyyvsp;_TED4=_TED3[0];_TED5=_TED4.l;_TED6=_TED5.first_line;_TED7=Cyc_Position_loc_to_seg(_TED6);_TED8=Cyc_Absyn_goto_stmt(_TECF,_TED7);yyval=Cyc_Stmt_tok(_TED8);goto _LL0;case 381: _TED9=yyyvsp;_TEDA=_TED9[0];_TEDB=_TEDA.l;_TEDC=_TEDB.first_line;_TEDD=
# 2684 "parse.y"
Cyc_Position_loc_to_seg(_TEDC);_TEDE=Cyc_Absyn_continue_stmt(_TEDD);yyval=Cyc_Stmt_tok(_TEDE);goto _LL0;case 382: _TEDF=yyyvsp;_TEE0=_TEDF[0];_TEE1=_TEE0.l;_TEE2=_TEE1.first_line;_TEE3=
# 2685 "parse.y"
Cyc_Position_loc_to_seg(_TEE2);_TEE4=Cyc_Absyn_break_stmt(_TEE3);yyval=Cyc_Stmt_tok(_TEE4);goto _LL0;case 383: _TEE5=yyyvsp;_TEE6=_TEE5[0];_TEE7=_TEE6.l;_TEE8=_TEE7.first_line;_TEE9=
# 2686 "parse.y"
Cyc_Position_loc_to_seg(_TEE8);_TEEA=Cyc_Absyn_return_stmt(0,_TEE9);yyval=Cyc_Stmt_tok(_TEEA);goto _LL0;case 384: _TEEB=yyyvsp;_TEEC=& _TEEB[1].v;_TEED=(union Cyc_YYSTYPE*)_TEEC;_TEEE=
# 2687 "parse.y"
Cyc_yyget_Exp_tok(_TEED);_TEEF=yyyvsp;_TEF0=_TEEF[0];_TEF1=_TEF0.l;_TEF2=_TEF1.first_line;_TEF3=Cyc_Position_loc_to_seg(_TEF2);_TEF4=Cyc_Absyn_return_stmt(_TEEE,_TEF3);yyval=Cyc_Stmt_tok(_TEF4);goto _LL0;case 385: _TEF5=yyyvsp;_TEF6=_TEF5[0];_TEF7=_TEF6.l;_TEF8=_TEF7.first_line;_TEF9=
# 2689 "parse.y"
Cyc_Position_loc_to_seg(_TEF8);_TEFA=Cyc_Absyn_fallthru_stmt(0,_TEF9);yyval=Cyc_Stmt_tok(_TEFA);goto _LL0;case 386: _TEFB=yyyvsp;_TEFC=_TEFB[0];_TEFD=_TEFC.l;_TEFE=_TEFD.first_line;_TEFF=
# 2690 "parse.y"
Cyc_Position_loc_to_seg(_TEFE);_TF00=Cyc_Absyn_fallthru_stmt(0,_TEFF);yyval=Cyc_Stmt_tok(_TF00);goto _LL0;case 387: _TF01=yyyvsp;_TF02=& _TF01[2].v;_TF03=(union Cyc_YYSTYPE*)_TF02;_TF04=
# 2692 "parse.y"
Cyc_yyget_YY4(_TF03);_TF05=yyyvsp;_TF06=_TF05[0];_TF07=_TF06.l;_TF08=_TF07.first_line;_TF09=Cyc_Position_loc_to_seg(_TF08);_TF0A=Cyc_Absyn_fallthru_stmt(_TF04,_TF09);yyval=Cyc_Stmt_tok(_TF0A);goto _LL0;case 388: _TF0B=yyyvsp;_TF0C=_TF0B[0];
# 2701 "parse.y"
yyval=_TF0C.v;goto _LL0;case 389: _TF0D=yyyvsp;_TF0E=_TF0D[0];
# 2704
yyval=_TF0E.v;goto _LL0;case 390: _TF0F=yyyvsp;_TF10=& _TF0F[0].v;_TF11=(union Cyc_YYSTYPE*)_TF10;_TF12=
# 2706 "parse.y"
Cyc_yyget_YY9(_TF11);_TF13=Cyc_Parse_pat2exp(_TF12);_TF14=yyyvsp;_TF15=& _TF14[2].v;_TF16=(union Cyc_YYSTYPE*)_TF15;_TF17=Cyc_yyget_Exp_tok(_TF16);_TF18=yyyvsp;_TF19=& _TF18[4].v;_TF1A=(union Cyc_YYSTYPE*)_TF19;_TF1B=Cyc_yyget_Exp_tok(_TF1A);_TF1C=yyyvsp;_TF1D=_TF1C[0];_TF1E=_TF1D.l;_TF1F=_TF1E.first_line;_TF20=Cyc_Position_loc_to_seg(_TF1F);_TF21=Cyc_Absyn_conditional_exp(_TF13,_TF17,_TF1B,_TF20);_TF22=Cyc_Absyn_exp_pat(_TF21);yyval=Cyc_YY9(_TF22);goto _LL0;case 391: _TF23=yyyvsp;_TF24=_TF23[0];
# 2709
yyval=_TF24.v;goto _LL0;case 392: _TF25=yyyvsp;_TF26=& _TF25[0].v;_TF27=(union Cyc_YYSTYPE*)_TF26;_TF28=
# 2711 "parse.y"
Cyc_yyget_YY9(_TF27);_TF29=Cyc_Parse_pat2exp(_TF28);_TF2A=yyyvsp;_TF2B=& _TF2A[2].v;_TF2C=(union Cyc_YYSTYPE*)_TF2B;_TF2D=Cyc_yyget_Exp_tok(_TF2C);_TF2E=yyyvsp;_TF2F=_TF2E[0];_TF30=_TF2F.l;_TF31=_TF30.first_line;_TF32=Cyc_Position_loc_to_seg(_TF31);_TF33=Cyc_Absyn_or_exp(_TF29,_TF2D,_TF32);_TF34=Cyc_Absyn_exp_pat(_TF33);yyval=Cyc_YY9(_TF34);goto _LL0;case 393: _TF35=yyyvsp;_TF36=_TF35[0];
# 2714
yyval=_TF36.v;goto _LL0;case 394: _TF37=yyyvsp;_TF38=& _TF37[0].v;_TF39=(union Cyc_YYSTYPE*)_TF38;_TF3A=
# 2716 "parse.y"
Cyc_yyget_YY9(_TF39);_TF3B=Cyc_Parse_pat2exp(_TF3A);_TF3C=yyyvsp;_TF3D=& _TF3C[2].v;_TF3E=(union Cyc_YYSTYPE*)_TF3D;_TF3F=Cyc_yyget_Exp_tok(_TF3E);_TF40=yyyvsp;_TF41=_TF40[0];_TF42=_TF41.l;_TF43=_TF42.first_line;_TF44=Cyc_Position_loc_to_seg(_TF43);_TF45=Cyc_Absyn_and_exp(_TF3B,_TF3F,_TF44);_TF46=Cyc_Absyn_exp_pat(_TF45);yyval=Cyc_YY9(_TF46);goto _LL0;case 395: _TF47=yyyvsp;_TF48=_TF47[0];
# 2719
yyval=_TF48.v;goto _LL0;case 396: _TF49=yyyvsp;_TF4A=& _TF49[0].v;_TF4B=(union Cyc_YYSTYPE*)_TF4A;_TF4C=
# 2721 "parse.y"
Cyc_yyget_YY9(_TF4B);_TF4D=Cyc_Parse_pat2exp(_TF4C);_TF4E=yyyvsp;_TF4F=& _TF4E[2].v;_TF50=(union Cyc_YYSTYPE*)_TF4F;_TF51=Cyc_yyget_Exp_tok(_TF50);_TF52=yyyvsp;_TF53=_TF52[0];_TF54=_TF53.l;_TF55=_TF54.first_line;_TF56=Cyc_Position_loc_to_seg(_TF55);_TF57=Cyc_Absyn_prim2_exp(14U,_TF4D,_TF51,_TF56);_TF58=Cyc_Absyn_exp_pat(_TF57);yyval=Cyc_YY9(_TF58);goto _LL0;case 397: _TF59=yyyvsp;_TF5A=_TF59[0];
# 2724
yyval=_TF5A.v;goto _LL0;case 398: _TF5B=yyyvsp;_TF5C=& _TF5B[0].v;_TF5D=(union Cyc_YYSTYPE*)_TF5C;_TF5E=
# 2726 "parse.y"
Cyc_yyget_YY9(_TF5D);_TF5F=Cyc_Parse_pat2exp(_TF5E);_TF60=yyyvsp;_TF61=& _TF60[2].v;_TF62=(union Cyc_YYSTYPE*)_TF61;_TF63=Cyc_yyget_Exp_tok(_TF62);_TF64=yyyvsp;_TF65=_TF64[0];_TF66=_TF65.l;_TF67=_TF66.first_line;_TF68=Cyc_Position_loc_to_seg(_TF67);_TF69=Cyc_Absyn_prim2_exp(15U,_TF5F,_TF63,_TF68);_TF6A=Cyc_Absyn_exp_pat(_TF69);yyval=Cyc_YY9(_TF6A);goto _LL0;case 399: _TF6B=yyyvsp;_TF6C=_TF6B[0];
# 2729
yyval=_TF6C.v;goto _LL0;case 400: _TF6D=yyyvsp;_TF6E=& _TF6D[0].v;_TF6F=(union Cyc_YYSTYPE*)_TF6E;_TF70=
# 2731 "parse.y"
Cyc_yyget_YY9(_TF6F);_TF71=Cyc_Parse_pat2exp(_TF70);_TF72=yyyvsp;_TF73=& _TF72[2].v;_TF74=(union Cyc_YYSTYPE*)_TF73;_TF75=Cyc_yyget_Exp_tok(_TF74);_TF76=yyyvsp;_TF77=_TF76[0];_TF78=_TF77.l;_TF79=_TF78.first_line;_TF7A=Cyc_Position_loc_to_seg(_TF79);_TF7B=Cyc_Absyn_prim2_exp(13U,_TF71,_TF75,_TF7A);_TF7C=Cyc_Absyn_exp_pat(_TF7B);yyval=Cyc_YY9(_TF7C);goto _LL0;case 401: _TF7D=yyyvsp;_TF7E=_TF7D[0];
# 2734
yyval=_TF7E.v;goto _LL0;case 402: _TF7F=yyyvsp;_TF80=& _TF7F[1].v;_TF81=(union Cyc_YYSTYPE*)_TF80;_TF82=
# 2736 "parse.y"
Cyc_yyget_YY69(_TF81);_TF83=yyyvsp;_TF84=& _TF83[0].v;_TF85=(union Cyc_YYSTYPE*)_TF84;_TF86=Cyc_yyget_YY9(_TF85);_TF87=Cyc_Parse_pat2exp(_TF86);_TF88=yyyvsp;_TF89=& _TF88[2].v;_TF8A=(union Cyc_YYSTYPE*)_TF89;_TF8B=Cyc_yyget_Exp_tok(_TF8A);_TF8C=yyyvsp;_TF8D=_TF8C[0];_TF8E=_TF8D.l;_TF8F=_TF8E.first_line;_TF90=Cyc_Position_loc_to_seg(_TF8F);_TF91=_TF82(_TF87,_TF8B,_TF90);_TF92=Cyc_Absyn_exp_pat(_TF91);yyval=Cyc_YY9(_TF92);goto _LL0;case 403: _TF93=yyyvsp;_TF94=_TF93[0];
# 2739
yyval=_TF94.v;goto _LL0;case 404: _TF95=yyyvsp;_TF96=& _TF95[1].v;_TF97=(union Cyc_YYSTYPE*)_TF96;_TF98=
# 2741 "parse.y"
Cyc_yyget_YY69(_TF97);_TF99=yyyvsp;_TF9A=& _TF99[0].v;_TF9B=(union Cyc_YYSTYPE*)_TF9A;_TF9C=Cyc_yyget_YY9(_TF9B);_TF9D=Cyc_Parse_pat2exp(_TF9C);_TF9E=yyyvsp;_TF9F=& _TF9E[2].v;_TFA0=(union Cyc_YYSTYPE*)_TF9F;_TFA1=Cyc_yyget_Exp_tok(_TFA0);_TFA2=yyyvsp;_TFA3=_TFA2[0];_TFA4=_TFA3.l;_TFA5=_TFA4.first_line;_TFA6=Cyc_Position_loc_to_seg(_TFA5);_TFA7=_TF98(_TF9D,_TFA1,_TFA6);_TFA8=Cyc_Absyn_exp_pat(_TFA7);yyval=Cyc_YY9(_TFA8);goto _LL0;case 405: _TFA9=yyyvsp;_TFAA=_TFA9[0];
# 2744
yyval=_TFAA.v;goto _LL0;case 406: _TFAB=yyyvsp;_TFAC=& _TFAB[0].v;_TFAD=(union Cyc_YYSTYPE*)_TFAC;_TFAE=
# 2746 "parse.y"
Cyc_yyget_YY9(_TFAD);_TFAF=Cyc_Parse_pat2exp(_TFAE);_TFB0=yyyvsp;_TFB1=& _TFB0[2].v;_TFB2=(union Cyc_YYSTYPE*)_TFB1;_TFB3=Cyc_yyget_Exp_tok(_TFB2);_TFB4=yyyvsp;_TFB5=_TFB4[0];_TFB6=_TFB5.l;_TFB7=_TFB6.first_line;_TFB8=Cyc_Position_loc_to_seg(_TFB7);_TFB9=Cyc_Absyn_prim2_exp(16U,_TFAF,_TFB3,_TFB8);_TFBA=Cyc_Absyn_exp_pat(_TFB9);yyval=Cyc_YY9(_TFBA);goto _LL0;case 407: _TFBB=yyyvsp;_TFBC=& _TFBB[0].v;_TFBD=(union Cyc_YYSTYPE*)_TFBC;_TFBE=
# 2748 "parse.y"
Cyc_yyget_YY9(_TFBD);_TFBF=Cyc_Parse_pat2exp(_TFBE);_TFC0=yyyvsp;_TFC1=& _TFC0[2].v;_TFC2=(union Cyc_YYSTYPE*)_TFC1;_TFC3=Cyc_yyget_Exp_tok(_TFC2);_TFC4=yyyvsp;_TFC5=_TFC4[0];_TFC6=_TFC5.l;_TFC7=_TFC6.first_line;_TFC8=Cyc_Position_loc_to_seg(_TFC7);_TFC9=Cyc_Absyn_prim2_exp(17U,_TFBF,_TFC3,_TFC8);_TFCA=Cyc_Absyn_exp_pat(_TFC9);yyval=Cyc_YY9(_TFCA);goto _LL0;case 408: _TFCB=yyyvsp;_TFCC=_TFCB[0];
# 2751
yyval=_TFCC.v;goto _LL0;case 409: _TFCD=yyyvsp;_TFCE=& _TFCD[1].v;_TFCF=(union Cyc_YYSTYPE*)_TFCE;_TFD0=
# 2753 "parse.y"
Cyc_yyget_YY6(_TFCF);_TFD1=yyyvsp;_TFD2=& _TFD1[0].v;_TFD3=(union Cyc_YYSTYPE*)_TFD2;_TFD4=Cyc_yyget_YY9(_TFD3);_TFD5=Cyc_Parse_pat2exp(_TFD4);_TFD6=yyyvsp;_TFD7=& _TFD6[2].v;_TFD8=(union Cyc_YYSTYPE*)_TFD7;_TFD9=Cyc_yyget_Exp_tok(_TFD8);_TFDA=yyyvsp;_TFDB=_TFDA[0];_TFDC=_TFDB.l;_TFDD=_TFDC.first_line;_TFDE=Cyc_Position_loc_to_seg(_TFDD);_TFDF=Cyc_Absyn_prim2_exp(_TFD0,_TFD5,_TFD9,_TFDE);_TFE0=Cyc_Absyn_exp_pat(_TFDF);yyval=Cyc_YY9(_TFE0);goto _LL0;case 410: _TFE1=yyyvsp;_TFE2=_TFE1[0];
# 2756
yyval=_TFE2.v;goto _LL0;case 411: _TFE3=yyyvsp;_TFE4=& _TFE3[1].v;_TFE5=(union Cyc_YYSTYPE*)_TFE4;_TFE6=
# 2758 "parse.y"
Cyc_yyget_YY6(_TFE5);_TFE7=yyyvsp;_TFE8=& _TFE7[0].v;_TFE9=(union Cyc_YYSTYPE*)_TFE8;_TFEA=Cyc_yyget_YY9(_TFE9);_TFEB=Cyc_Parse_pat2exp(_TFEA);_TFEC=yyyvsp;_TFED=& _TFEC[2].v;_TFEE=(union Cyc_YYSTYPE*)_TFED;_TFEF=Cyc_yyget_Exp_tok(_TFEE);_TFF0=yyyvsp;_TFF1=_TFF0[0];_TFF2=_TFF1.l;_TFF3=_TFF2.first_line;_TFF4=Cyc_Position_loc_to_seg(_TFF3);_TFF5=Cyc_Absyn_prim2_exp(_TFE6,_TFEB,_TFEF,_TFF4);_TFF6=Cyc_Absyn_exp_pat(_TFF5);yyval=Cyc_YY9(_TFF6);goto _LL0;case 412: _TFF7=yyyvsp;_TFF8=_TFF7[0];
# 2761
yyval=_TFF8.v;goto _LL0;case 413: _TFF9=yyyvsp;_TFFA=& _TFF9[1].v;_TFFB=(union Cyc_YYSTYPE*)_TFFA;_TFFC=
# 2763 "parse.y"
Cyc_yyget_YY38(_TFFB);_TFFD=yyyvsp;_TFFE=_TFFD[1];_TFFF=_TFFE.l;_T1000=_TFFF.first_line;_T1001=Cyc_Position_loc_to_seg(_T1000);{void*t=Cyc_Parse_type_name_to_type(_TFFC,_T1001);_T1002=t;_T1003=yyyvsp;_T1004=& _T1003[3].v;_T1005=(union Cyc_YYSTYPE*)_T1004;_T1006=
Cyc_yyget_Exp_tok(_T1005);_T1007=yyyvsp;_T1008=_T1007[0];_T1009=_T1008.l;_T100A=_T1009.first_line;_T100B=Cyc_Position_loc_to_seg(_T100A);_T100C=Cyc_Absyn_cast_exp(_T1002,_T1006,1,0U,_T100B);_T100D=Cyc_Absyn_exp_pat(_T100C);yyval=Cyc_YY9(_T100D);goto _LL0;}case 414: _T100E=yyyvsp;_T100F=_T100E[0];
# 2769 "parse.y"
yyval=_T100F.v;goto _LL0;case 415: _T1010=yyyvsp;_T1011=& _T1010[0].v;_T1012=(union Cyc_YYSTYPE*)_T1011;_T1013=
# 2773 "parse.y"
Cyc_yyget_YY6(_T1012);_T1014=yyyvsp;_T1015=& _T1014[1].v;_T1016=(union Cyc_YYSTYPE*)_T1015;_T1017=Cyc_yyget_Exp_tok(_T1016);_T1018=yyyvsp;_T1019=_T1018[0];_T101A=_T1019.l;_T101B=_T101A.first_line;_T101C=Cyc_Position_loc_to_seg(_T101B);_T101D=Cyc_Absyn_prim1_exp(_T1013,_T1017,_T101C);_T101E=Cyc_Absyn_exp_pat(_T101D);yyval=Cyc_YY9(_T101E);goto _LL0;case 416: _T101F=yyyvsp;_T1020=& _T101F[2].v;_T1021=(union Cyc_YYSTYPE*)_T1020;_T1022=
# 2775 "parse.y"
Cyc_yyget_YY38(_T1021);_T1023=yyyvsp;_T1024=_T1023[2];_T1025=_T1024.l;_T1026=_T1025.first_line;_T1027=Cyc_Position_loc_to_seg(_T1026);{void*t=Cyc_Parse_type_name_to_type(_T1022,_T1027);_T1028=t;_T1029=yyyvsp;_T102A=_T1029[0];_T102B=_T102A.l;_T102C=_T102B.first_line;_T102D=
Cyc_Position_loc_to_seg(_T102C);_T102E=Cyc_Absyn_sizeoftype_exp(_T1028,_T102D);_T102F=Cyc_Absyn_exp_pat(_T102E);yyval=Cyc_YY9(_T102F);goto _LL0;}case 417: _T1030=yyyvsp;_T1031=& _T1030[1].v;_T1032=(union Cyc_YYSTYPE*)_T1031;_T1033=
# 2778 "parse.y"
Cyc_yyget_Exp_tok(_T1032);_T1034=yyyvsp;_T1035=_T1034[0];_T1036=_T1035.l;_T1037=_T1036.first_line;_T1038=Cyc_Position_loc_to_seg(_T1037);_T1039=Cyc_Absyn_sizeofexp_exp(_T1033,_T1038);_T103A=Cyc_Absyn_exp_pat(_T1039);yyval=Cyc_YY9(_T103A);goto _LL0;case 418: _T103B=yyyvsp;_T103C=& _T103B[2].v;_T103D=(union Cyc_YYSTYPE*)_T103C;_T103E=
# 2780 "parse.y"
Cyc_yyget_YY38(_T103D);_T103F=*_T103E;_T1040=_T103F.f2;_T1041=yyyvsp;_T1042=& _T1041[4].v;_T1043=(union Cyc_YYSTYPE*)_T1042;_T1044=Cyc_yyget_YY3(_T1043);_T1045=Cyc_List_imp_rev(_T1044);_T1046=yyyvsp;_T1047=_T1046[0];_T1048=_T1047.l;_T1049=_T1048.first_line;_T104A=Cyc_Position_loc_to_seg(_T1049);_T104B=Cyc_Absyn_offsetof_exp(_T1040,_T1045,_T104A);_T104C=Cyc_Absyn_exp_pat(_T104B);yyval=Cyc_YY9(_T104C);goto _LL0;case 419: _T104D=yyyvsp;_T104E=_T104D[0];
# 2785 "parse.y"
yyval=_T104E.v;goto _LL0;case 420: _T104F=yyyvsp;_T1050=_T104F[0];
# 2793 "parse.y"
yyval=_T1050.v;goto _LL0;case 421: _T1051=& Cyc_Absyn_Wild_p_val;_T1052=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T1051;_T1053=(void*)_T1052;_T1054=yyyvsp;_T1055=_T1054[0];_T1056=_T1055.l;_T1057=_T1056.first_line;_T1058=
# 2797 "parse.y"
Cyc_Position_loc_to_seg(_T1057);_T1059=Cyc_Absyn_new_pat(_T1053,_T1058);yyval=Cyc_YY9(_T1059);goto _LL0;case 422: _T105A=yyyvsp;_T105B=& _T105A[1].v;_T105C=(union Cyc_YYSTYPE*)_T105B;_T105D=
# 2798 "parse.y"
Cyc_yyget_Exp_tok(_T105C);_T105E=Cyc_Absyn_exp_pat(_T105D);yyval=Cyc_YY9(_T105E);goto _LL0;case 423:{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct));_T1877->tag=14;_T1060=yyyvsp;_T1061=& _T1060[0].v;_T1062=(union Cyc_YYSTYPE*)_T1061;
# 2799 "parse.y"
_T1877->f1=Cyc_yyget_QualId_tok(_T1062);_T105F=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_T1877;}_T1063=(void*)_T105F;_T1064=yyyvsp;_T1065=_T1064[0];_T1066=_T1065.l;_T1067=_T1066.first_line;_T1068=Cyc_Position_loc_to_seg(_T1067);_T1069=Cyc_Absyn_new_pat(_T1063,_T1068);yyval=Cyc_YY9(_T1069);goto _LL0;case 424:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1877->tag=5;_T106B=yyyvsp;_T106C=& _T106B[1].v;_T106D=(union Cyc_YYSTYPE*)_T106C;
# 2800 "parse.y"
_T1877->f1=Cyc_yyget_YY9(_T106D);_T106A=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1877;}_T106E=(void*)_T106A;_T106F=yyyvsp;_T1070=_T106F[0];_T1071=_T1070.l;_T1072=_T1071.first_line;_T1073=Cyc_Position_loc_to_seg(_T1072);_T1074=Cyc_Absyn_new_pat(_T106E,_T1073);yyval=Cyc_YY9(_T1074);goto _LL0;case 425: _T1075=yyyvsp;_T1076=& _T1075[0].v;_T1077=(union Cyc_YYSTYPE*)_T1076;{
# 2802 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(_T1077);_T1078=e;{
void*_T1877=_T1078->r;struct _fat_ptr _T1878;int _T1879;short _T187A;char _T187B;enum Cyc_Absyn_Sign _T187C;_T1079=(int*)_T1877;_T107A=*_T1079;if(_T107A!=0)goto _TL2E0;_T107B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1877;_T107C=_T107B->f1;_T107D=_T107C.LongLong_c;_T107E=_T107D.tag;switch(_T107E){case 2:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T187D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1877;_T107F=_T187D->f1;_T1080=_T107F.Char_c;_T1081=_T1080.val;_T187C=_T1081.f0;_T1082=_T187D->f1;_T1083=_T1082.Char_c;_T1084=_T1083.val;_T187B=_T1084.f1;}{enum Cyc_Absyn_Sign s=_T187C;char i=_T187B;{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct));_T187D->tag=10;
# 2805
_T187D->f1=i;_T1085=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_T187D;}_T1086=(void*)_T1085;_T1087=e;_T1088=_T1087->loc;_T1089=Cyc_Absyn_new_pat(_T1086,_T1088);yyval=Cyc_YY9(_T1089);goto _LL537;}case 4:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T187D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1877;_T108A=_T187D->f1;_T108B=_T108A.Short_c;_T108C=_T108B.val;_T187C=_T108C.f0;_T108D=_T187D->f1;_T108E=_T108D.Short_c;_T108F=_T108E.val;_T187A=_T108F.f1;}{enum Cyc_Absyn_Sign s=_T187C;short i=_T187A;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T187D->tag=9;
# 2807
_T187D->f1=s;_T1091=i;_T187D->f2=(int)_T1091;_T1090=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T187D;}_T1092=(void*)_T1090;_T1093=e;_T1094=_T1093->loc;_T1095=Cyc_Absyn_new_pat(_T1092,_T1094);yyval=Cyc_YY9(_T1095);goto _LL537;}case 5:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T187D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1877;_T1096=_T187D->f1;_T1097=_T1096.Int_c;_T1098=_T1097.val;_T187C=_T1098.f0;_T1099=_T187D->f1;_T109A=_T1099.Int_c;_T109B=_T109A.val;_T1879=_T109B.f1;}{enum Cyc_Absyn_Sign s=_T187C;int i=_T1879;{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct));_T187D->tag=9;
# 2809
_T187D->f1=s;_T187D->f2=i;_T109C=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_T187D;}_T109D=(void*)_T109C;_T109E=e;_T109F=_T109E->loc;_T10A0=Cyc_Absyn_new_pat(_T109D,_T109F);yyval=Cyc_YY9(_T10A0);goto _LL537;}case 7:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T187D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T1877;_T10A1=_T187D->f1;_T10A2=_T10A1.Float_c;_T10A3=_T10A2.val;_T1878=_T10A3.f0;_T10A4=_T187D->f1;_T10A5=_T10A4.Float_c;_T10A6=_T10A5.val;_T1879=_T10A6.f1;}{struct _fat_ptr s=_T1878;int i=_T1879;{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_T187D=_cycalloc(sizeof(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct));_T187D->tag=11;
# 2811
_T187D->f1=s;_T187D->f2=i;_T10A7=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_T187D;}_T10A8=(void*)_T10A7;_T10A9=e;_T10AA=_T10A9->loc;_T10AB=Cyc_Absyn_new_pat(_T10A8,_T10AA);yyval=Cyc_YY9(_T10AB);goto _LL537;}case 1: _T10AC=& Cyc_Absyn_Null_p_val;_T10AD=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_T10AC;_T10AE=(void*)_T10AD;_T10AF=e;_T10B0=_T10AF->loc;_T10B1=
# 2813
Cyc_Absyn_new_pat(_T10AE,_T10B0);yyval=Cyc_YY9(_T10B1);goto _LL537;case 8: goto _LL545;case 9: _LL545: _T10B2=yyyvsp;_T10B3=_T10B2[0];_T10B4=_T10B3.l;_T10B5=_T10B4.first_line;_T10B6=
# 2816
Cyc_Position_loc_to_seg(_T10B5);_T10B7=_tag_fat("strings cannot occur within patterns",sizeof(char),37U);_T10B8=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10B6,_T10B7,_T10B8);goto _LL537;case 6: _T10B9=yyyvsp;_T10BA=_T10B9[0];_T10BB=_T10BA.l;_T10BC=_T10BB.first_line;_T10BD=
# 2818
Cyc_Position_loc_to_seg(_T10BC);_T10BE=_tag_fat("long long's in patterns not yet implemented",sizeof(char),44U);_T10BF=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10BD,_T10BE,_T10BF);goto _LL537;default: goto _LL548;}goto _TL2E1;_TL2E0: _LL548: _T10C0=yyyvsp;_T10C1=_T10C0[0];_T10C2=_T10C1.l;_T10C3=_T10C2.first_line;_T10C4=
# 2820
Cyc_Position_loc_to_seg(_T10C3);_T10C5=_tag_fat("bad constant in case",sizeof(char),21U);_T10C6=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10C4,_T10C5,_T10C6);_TL2E1: _LL537:;}goto _LL0;}case 426: _T10C7=yyyvsp;_T10C8=& _T10C7[1].v;_T10C9=(union Cyc_YYSTYPE*)_T10C8;_T10CA=
# 2824 "parse.y"
Cyc_yyget_String_tok(_T10C9);_T10CB=_tag_fat("as",sizeof(char),3U);_T10CC=Cyc_strcmp(_T10CA,_T10CB);if(_T10CC==0)goto _TL2E3;_T10CD=yyyvsp;_T10CE=_T10CD[1];_T10CF=_T10CE.l;_T10D0=_T10CF.first_line;_T10D1=
Cyc_Position_loc_to_seg(_T10D0);_T10D2=_tag_fat("expecting `as'",sizeof(char),15U);_T10D3=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10D1,_T10D2,_T10D3);goto _TL2E4;_TL2E3: _TL2E4:{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T1877->tag=1;_T10D5=yyyvsp;_T10D6=_T10D5[0];_T10D7=_T10D6.l;_T10D8=_T10D7.first_line;_T10D9=
Cyc_Position_loc_to_seg(_T10D8);{struct _tuple0*_T1878=_cycalloc(sizeof(struct _tuple0));_T1878->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));_T10DC=yyyvsp;_T10DD=& _T10DC[0].v;_T10DE=(union Cyc_YYSTYPE*)_T10DD;*_T1879=Cyc_yyget_String_tok(_T10DE);_T10DB=(struct _fat_ptr*)_T1879;}_T1878->f1=_T10DB;_T10DA=(struct _tuple0*)_T1878;}_T10DF=Cyc_Absyn_void_type;_T1877->f1=Cyc_Absyn_new_vardecl(_T10D9,_T10DA,_T10DF,0,0);_T10E0=yyyvsp;_T10E1=& _T10E0[2].v;_T10E2=(union Cyc_YYSTYPE*)_T10E1;
_T1877->f2=Cyc_yyget_YY9(_T10E2);_T10D4=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T1877;}_T10E3=(void*)_T10D4;_T10E4=yyyvsp;_T10E5=_T10E4[0];_T10E6=_T10E5.l;_T10E7=_T10E6.first_line;_T10E8=Cyc_Position_loc_to_seg(_T10E7);_T10E9=
# 2826
Cyc_Absyn_new_pat(_T10E3,_T10E8);yyval=Cyc_YY9(_T10E9);goto _LL0;case 427: _T10EA=yyyvsp;_T10EB=& _T10EA[0].v;_T10EC=(union Cyc_YYSTYPE*)_T10EB;_T10ED=
# 2830 "parse.y"
Cyc_yyget_String_tok(_T10EC);_T10EE=_tag_fat("alias",sizeof(char),6U);_T10EF=Cyc_strcmp(_T10ED,_T10EE);if(_T10EF==0)goto _TL2E5;_T10F0=yyyvsp;_T10F1=_T10F0[1];_T10F2=_T10F1.l;_T10F3=_T10F2.first_line;_T10F4=
Cyc_Position_loc_to_seg(_T10F3);_T10F5=_tag_fat("expecting `alias'",sizeof(char),18U);_T10F6=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T10F4,_T10F5,_T10F6);goto _TL2E6;_TL2E5: _TL2E6: _T10F7=yyyvsp;_T10F8=_T10F7[0];_T10F9=_T10F8.l;_T10FA=_T10F9.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T10FA);_T10FB=yyyvsp;_T10FC=& _T10FB[2].v;_T10FD=(union Cyc_YYSTYPE*)_T10FC;_T10FE=
Cyc_yyget_String_tok(_T10FD);_T10FF=location;Cyc_Parse_tvar_ok(_T10FE,_T10FF);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T1100=tv;{struct _fat_ptr*_T1877=_cycalloc(sizeof(struct _fat_ptr));_T1102=yyyvsp;_T1103=& _T1102[2].v;_T1104=(union Cyc_YYSTYPE*)_T1103;*_T1877=Cyc_yyget_String_tok(_T1104);_T1101=(struct _fat_ptr*)_T1877;}_T1100->name=_T1101;_T1105=tv;_T1105->identity=- 1;_T1106=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T1877->tag=0;_T1108=& Cyc_Kinds_ek;_T1877->f1=(struct Cyc_Absyn_Kind*)_T1108;_T1107=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1877;}_T1106->kind=(void*)_T1107;_T1109=tv;_T1109->aquals_bound=0;_T110A=yyyvsp;_T110B=_T110A[0];_T110C=_T110B.l;_T110D=_T110C.first_line;_T110E=
Cyc_Position_loc_to_seg(_T110D);{struct _tuple0*_T1877=_cycalloc(sizeof(struct _tuple0));_T1877->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T1111=yyyvsp;_T1112=& _T1111[5].v;_T1113=(union Cyc_YYSTYPE*)_T1112;*_T1878=Cyc_yyget_String_tok(_T1113);_T1110=(struct _fat_ptr*)_T1878;}_T1877->f1=_T1110;_T110F=(struct _tuple0*)_T1877;}_T1114=yyyvsp;_T1115=& _T1114[4].v;_T1116=(union Cyc_YYSTYPE*)_T1115;_T1117=
Cyc_yyget_YY38(_T1116);_T1118=yyyvsp;_T1119=_T1118[4];_T111A=_T1119.l;_T111B=_T111A.first_line;_T111C=Cyc_Position_loc_to_seg(_T111B);_T111D=Cyc_Parse_type_name_to_type(_T1117,_T111C);{
# 2835
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T110E,_T110F,_T111D,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T1877->tag=2;
# 2837
_T1877->f1=tv;_T1877->f2=vd;_T111E=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T1877;}_T111F=(void*)_T111E;_T1120=location;_T1121=Cyc_Absyn_new_pat(_T111F,_T1120);yyval=Cyc_YY9(_T1121);goto _LL0;}}}case 428: _T1122=yyyvsp;_T1123=& _T1122[0].v;_T1124=(union Cyc_YYSTYPE*)_T1123;_T1125=
# 2840 "parse.y"
Cyc_yyget_String_tok(_T1124);_T1126=_tag_fat("alias",sizeof(char),6U);_T1127=Cyc_strcmp(_T1125,_T1126);if(_T1127==0)goto _TL2E7;_T1128=yyyvsp;_T1129=_T1128[1];_T112A=_T1129.l;_T112B=_T112A.first_line;_T112C=
Cyc_Position_loc_to_seg(_T112B);_T112D=_tag_fat("expecting `alias'",sizeof(char),18U);_T112E=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T112C,_T112D,_T112E);goto _TL2E8;_TL2E7: _TL2E8: _T112F=yyyvsp;_T1130=_T112F[0];_T1131=_T1130.l;_T1132=_T1131.first_line;{
unsigned location=Cyc_Position_loc_to_seg(_T1132);_T1133=yyyvsp;_T1134=& _T1133[2].v;_T1135=(union Cyc_YYSTYPE*)_T1134;_T1136=
Cyc_yyget_String_tok(_T1135);_T1137=location;Cyc_Parse_tvar_ok(_T1136,_T1137);{
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T1138=tv;{struct _fat_ptr*_T1877=_cycalloc(sizeof(struct _fat_ptr));_T113A=yyyvsp;_T113B=& _T113A[2].v;_T113C=(union Cyc_YYSTYPE*)_T113B;*_T1877=Cyc_yyget_String_tok(_T113C);_T1139=(struct _fat_ptr*)_T1877;}_T1138->name=_T1139;_T113D=tv;_T113D->identity=- 1;_T113E=tv;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct));_T1877->tag=0;_T1140=& Cyc_Kinds_ek;_T1877->f1=(struct Cyc_Absyn_Kind*)_T1140;_T113F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T1877;}_T113E->kind=(void*)_T113F;_T1141=tv;_T1141->aquals_bound=0;_T1142=yyyvsp;_T1143=_T1142[0];_T1144=_T1143.l;_T1145=_T1144.first_line;_T1146=
Cyc_Position_loc_to_seg(_T1145);{struct _tuple0*_T1877=_cycalloc(sizeof(struct _tuple0));_T1877->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T1149=yyyvsp;_T114A=& _T1149[5].v;_T114B=(union Cyc_YYSTYPE*)_T114A;*_T1878=Cyc_yyget_String_tok(_T114B);_T1148=(struct _fat_ptr*)_T1878;}_T1877->f1=_T1148;_T1147=(struct _tuple0*)_T1877;}_T114C=yyyvsp;_T114D=& _T114C[4].v;_T114E=(union Cyc_YYSTYPE*)_T114D;_T114F=
Cyc_yyget_YY38(_T114E);_T1150=yyyvsp;_T1151=_T1150[4];_T1152=_T1151.l;_T1153=_T1152.first_line;_T1154=Cyc_Position_loc_to_seg(_T1153);_T1155=Cyc_Parse_type_name_to_type(_T114F,_T1154);{
# 2845
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(_T1146,_T1147,_T1155,0,0);{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct));_T1877->tag=2;
# 2847
_T1877->f1=tv;_T1877->f2=vd;_T1156=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_T1877;}_T1157=(void*)_T1156;_T1158=location;_T1159=Cyc_Absyn_new_pat(_T1157,_T1158);yyval=Cyc_YY9(_T1159);goto _LL0;}}}case 429: _T115A=yyyvsp;_T115B=& _T115A[2].v;_T115C=(union Cyc_YYSTYPE*)_T115B;_T115D=
# 2850 "parse.y"
Cyc_yyget_YY14(_T115C);{struct _tuple23 _T1877=*_T115D;int _T1878;struct Cyc_List_List*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_List_List*fps=_T1879;int dots=_T1878;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T187A->tag=6;
_T187A->f1=0;_T187A->f2=1;_T187A->f3=0;_T187A->f4=fps;_T187A->f5=dots;_T115E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T187A;}_T115F=(void*)_T115E;_T1160=yyyvsp;_T1161=_T1160[0];_T1162=_T1161.l;_T1163=_T1162.first_line;_T1164=Cyc_Position_loc_to_seg(_T1163);_T1165=Cyc_Absyn_new_pat(_T115F,_T1164);yyval=Cyc_YY9(_T1165);goto _LL0;}}case 430: _T1166=yyyvsp;_T1167=& _T1166[2].v;_T1168=(union Cyc_YYSTYPE*)_T1167;_T1169=
# 2854 "parse.y"
Cyc_yyget_YY10(_T1168);{struct _tuple23 _T1877=*_T1169;int _T1878;struct Cyc_List_List*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_List_List*ps=_T1879;int dots=_T1878;{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct));_T187A->tag=15;_T116B=yyyvsp;_T116C=& _T116B[0].v;_T116D=(union Cyc_YYSTYPE*)_T116C;
_T187A->f1=Cyc_yyget_QualId_tok(_T116D);_T187A->f2=ps;_T187A->f3=dots;_T116A=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_T187A;}_T116E=(void*)_T116A;_T116F=yyyvsp;_T1170=_T116F[0];_T1171=_T1170.l;_T1172=_T1171.first_line;_T1173=Cyc_Position_loc_to_seg(_T1172);_T1174=Cyc_Absyn_new_pat(_T116E,_T1173);yyval=Cyc_YY9(_T1174);goto _LL0;}}case 431: _T1175=yyyvsp;_T1176=& _T1175[3].v;_T1177=(union Cyc_YYSTYPE*)_T1176;_T1178=
# 2858 "parse.y"
Cyc_yyget_YY14(_T1177);{struct _tuple23 _T1877=*_T1178;int _T1878;struct Cyc_List_List*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_List_List*fps=_T1879;int dots=_T1878;_T117A=Cyc_List_map_c;{
struct Cyc_List_List*(*_T187A)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T117A;_T1179=_T187A;}_T117B=yyyvsp;_T117C=_T117B[2];_T117D=_T117C.l;_T117E=_T117D.first_line;_T117F=Cyc_Position_loc_to_seg(_T117E);_T1180=yyyvsp;_T1181=& _T1180[2].v;_T1182=(union Cyc_YYSTYPE*)_T1181;_T1183=Cyc_yyget_YY41(_T1182);{struct Cyc_List_List*exist_ts=_T1179(Cyc_Parse_typ2tvar,_T117F,_T1183);_T1184=yyyvsp;_T1185=& _T1184[0].v;_T1186=(union Cyc_YYSTYPE*)_T1185;_T1187=
Cyc_yyget_QualId_tok(_T1186);{union Cyc_Absyn_AggrInfo ai=Cyc_Absyn_UnknownAggr(0U,_T1187,0);
struct Cyc_Absyn_AppType_Absyn_Type_struct*typ;typ=_cycalloc(sizeof(struct Cyc_Absyn_AppType_Absyn_Type_struct));_T1188=typ;_T1188->tag=0;_T1189=typ;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct));_T187A->tag=24;_T187A->f1=ai;_T118A=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T187A;}_T1189->f1=(void*)_T118A;_T118B=typ;_T118B->f2=0;{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T187A->tag=6;_T118D=typ;
_T187A->f1=(void*)_T118D;_T187A->f2=0;_T187A->f3=exist_ts;_T187A->f4=fps;_T187A->f5=dots;_T118C=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T187A;}_T118E=(void*)_T118C;_T118F=yyyvsp;_T1190=_T118F[0];_T1191=_T1190.l;_T1192=_T1191.first_line;_T1193=Cyc_Position_loc_to_seg(_T1192);_T1194=Cyc_Absyn_new_pat(_T118E,_T1193);yyval=Cyc_YY9(_T1194);goto _LL0;}}}}case 432: _T1195=yyyvsp;_T1196=& _T1195[2].v;_T1197=(union Cyc_YYSTYPE*)_T1196;_T1198=
# 2865 "parse.y"
Cyc_yyget_YY14(_T1197);{struct _tuple23 _T1877=*_T1198;int _T1878;struct Cyc_List_List*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_List_List*fps=_T1879;int dots=_T1878;_T119A=Cyc_List_map_c;{
struct Cyc_List_List*(*_T187A)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))_T119A;_T1199=_T187A;}_T119B=yyyvsp;_T119C=_T119B[1];_T119D=_T119C.l;_T119E=_T119D.first_line;_T119F=Cyc_Position_loc_to_seg(_T119E);_T11A0=yyyvsp;_T11A1=& _T11A0[1].v;_T11A2=(union Cyc_YYSTYPE*)_T11A1;_T11A3=Cyc_yyget_YY41(_T11A2);{struct Cyc_List_List*exist_ts=_T1199(Cyc_Parse_typ2tvar,_T119F,_T11A3);{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct));_T187A->tag=6;
_T187A->f1=0;_T187A->f2=0;_T187A->f3=exist_ts;_T187A->f4=fps;_T187A->f5=dots;_T11A4=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_T187A;}_T11A5=(void*)_T11A4;_T11A6=yyyvsp;_T11A7=_T11A6[0];_T11A8=_T11A7.l;_T11A9=_T11A8.first_line;_T11AA=Cyc_Position_loc_to_seg(_T11A9);_T11AB=Cyc_Absyn_new_pat(_T11A5,_T11AA);yyval=Cyc_YY9(_T11AB);goto _LL0;}}}case 433:{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1877->tag=5;{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct));_T1878->tag=5;_T11AE=yyyvsp;_T11AF=& _T11AE[1].v;_T11B0=(union Cyc_YYSTYPE*)_T11AF;
# 2870 "parse.y"
_T1878->f1=Cyc_yyget_YY9(_T11B0);_T11AD=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1878;}_T11B1=(void*)_T11AD;_T11B2=yyyvsp;_T11B3=_T11B2[0];_T11B4=_T11B3.l;_T11B5=_T11B4.first_line;_T11B6=Cyc_Position_loc_to_seg(_T11B5);_T1877->f1=Cyc_Absyn_new_pat(_T11B1,_T11B6);_T11AC=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_T1877;}_T11B7=(void*)_T11AC;_T11B8=yyyvsp;_T11B9=_T11B8[0];_T11BA=_T11B9.l;_T11BB=_T11BA.first_line;_T11BC=Cyc_Position_loc_to_seg(_T11BB);_T11BD=Cyc_Absyn_new_pat(_T11B7,_T11BC);yyval=Cyc_YY9(_T11BD);goto _LL0;case 434:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T1877->tag=3;_T11BF=yyyvsp;_T11C0=_T11BF[0];_T11C1=_T11C0.l;_T11C2=_T11C1.first_line;_T11C3=
# 2872 "parse.y"
Cyc_Position_loc_to_seg(_T11C2);{struct _tuple0*_T1878=_cycalloc(sizeof(struct _tuple0));_T1878->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));_T11C6=yyyvsp;_T11C7=& _T11C6[1].v;_T11C8=(union Cyc_YYSTYPE*)_T11C7;*_T1879=Cyc_yyget_String_tok(_T11C8);_T11C5=(struct _fat_ptr*)_T1879;}_T1878->f1=_T11C5;_T11C4=(struct _tuple0*)_T1878;}_T11C9=Cyc_Absyn_void_type;_T1877->f1=Cyc_Absyn_new_vardecl(_T11C3,_T11C4,_T11C9,0,0);_T11CA=& Cyc_Absyn_Wild_p_val;_T11CB=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T11CA;_T11CC=(void*)_T11CB;_T11CD=yyyvsp;_T11CE=_T11CD[1];_T11CF=_T11CE.l;_T11D0=_T11CF.first_line;_T11D1=
# 2874
Cyc_Position_loc_to_seg(_T11D0);_T1877->f2=Cyc_Absyn_new_pat(_T11CC,_T11D1);_T11BE=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T1877;}_T11D2=(void*)_T11BE;_T11D3=yyyvsp;_T11D4=_T11D3[0];_T11D5=_T11D4.l;_T11D6=_T11D5.first_line;_T11D7=
Cyc_Position_loc_to_seg(_T11D6);_T11D8=
# 2872
Cyc_Absyn_new_pat(_T11D2,_T11D7);yyval=Cyc_YY9(_T11D8);goto _LL0;case 435: _T11D9=yyyvsp;_T11DA=& _T11D9[2].v;_T11DB=(union Cyc_YYSTYPE*)_T11DA;_T11DC=
# 2877 "parse.y"
Cyc_yyget_String_tok(_T11DB);_T11DD=_tag_fat("as",sizeof(char),3U);_T11DE=Cyc_strcmp(_T11DC,_T11DD);if(_T11DE==0)goto _TL2E9;_T11DF=yyyvsp;_T11E0=_T11DF[2];_T11E1=_T11E0.l;_T11E2=_T11E1.first_line;_T11E3=
Cyc_Position_loc_to_seg(_T11E2);_T11E4=_tag_fat("expecting `as'",sizeof(char),15U);_T11E5=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_err(_T11E3,_T11E4,_T11E5);goto _TL2EA;_TL2E9: _TL2EA:{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct));_T1877->tag=3;_T11E7=yyyvsp;_T11E8=_T11E7[0];_T11E9=_T11E8.l;_T11EA=_T11E9.first_line;_T11EB=
Cyc_Position_loc_to_seg(_T11EA);{struct _tuple0*_T1878=_cycalloc(sizeof(struct _tuple0));_T1878->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));_T11EE=yyyvsp;_T11EF=& _T11EE[1].v;_T11F0=(union Cyc_YYSTYPE*)_T11EF;*_T1879=Cyc_yyget_String_tok(_T11F0);_T11ED=(struct _fat_ptr*)_T1879;}_T1878->f1=_T11ED;_T11EC=(struct _tuple0*)_T1878;}_T11F1=Cyc_Absyn_void_type;_T1877->f1=Cyc_Absyn_new_vardecl(_T11EB,_T11EC,_T11F1,0,0);_T11F2=yyyvsp;_T11F3=& _T11F2[3].v;_T11F4=(union Cyc_YYSTYPE*)_T11F3;
# 2881
_T1877->f2=Cyc_yyget_YY9(_T11F4);_T11E6=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_T1877;}_T11F5=(void*)_T11E6;_T11F6=yyyvsp;_T11F7=_T11F6[0];_T11F8=_T11F7.l;_T11F9=_T11F8.first_line;_T11FA=Cyc_Position_loc_to_seg(_T11F9);_T11FB=
# 2879
Cyc_Absyn_new_pat(_T11F5,_T11FA);yyval=Cyc_YY9(_T11FB);goto _LL0;case 436: _T11FC=yyyvsp;_T11FD=& _T11FC[2].v;_T11FE=(union Cyc_YYSTYPE*)_T11FD;_T11FF=
# 2884 "parse.y"
Cyc_yyget_String_tok(_T11FE);_T1200=& Cyc_Kinds_ik;_T1201=(struct Cyc_Absyn_Kind*)_T1200;_T1202=Cyc_Kinds_kind_to_bound(_T1201);_T1203=yyyvsp;_T1204=_T1203[2];_T1205=_T1204.l;_T1206=_T1205.first_line;_T1207=Cyc_Position_loc_to_seg(_T1206);{void*tag=Cyc_Parse_id2type(_T11FF,_T1202,0,_T1207);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T1877->tag=4;_T1209=yyyvsp;_T120A=_T1209[2];_T120B=_T120A.l;_T120C=_T120B.first_line;_T120D=
Cyc_Position_loc_to_seg(_T120C);_T120E=tag;_T1877->f1=Cyc_Parse_typ2tvar(_T120D,_T120E);_T120F=yyyvsp;_T1210=_T120F[0];_T1211=_T1210.l;_T1212=_T1211.first_line;_T1213=
Cyc_Position_loc_to_seg(_T1212);{struct _tuple0*_T1878=_cycalloc(sizeof(struct _tuple0));_T1878->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));_T1216=yyyvsp;_T1217=& _T1216[0].v;_T1218=(union Cyc_YYSTYPE*)_T1217;*_T1879=Cyc_yyget_String_tok(_T1218);_T1215=(struct _fat_ptr*)_T1879;}_T1878->f1=_T1215;_T1214=(struct _tuple0*)_T1878;}_T1219=
Cyc_Absyn_tag_type(tag);
# 2886
_T1877->f2=Cyc_Absyn_new_vardecl(_T1213,_T1214,_T1219,0,0);_T1208=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T1877;}_T121A=(void*)_T1208;_T121B=yyyvsp;_T121C=_T121B[0];_T121D=_T121C.l;_T121E=_T121D.first_line;_T121F=
# 2888
Cyc_Position_loc_to_seg(_T121E);_T1220=
# 2885
Cyc_Absyn_new_pat(_T121A,_T121F);yyval=Cyc_YY9(_T1220);goto _LL0;}case 437: _T1221=& Cyc_Kinds_ik;_T1222=(struct Cyc_Absyn_Kind*)_T1221;_T1223=
# 2890 "parse.y"
Cyc_Kinds_kind_to_bound(_T1222);{struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar(_T1223);{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct));_T1877->tag=4;
_T1877->f1=tv;_T1225=yyyvsp;_T1226=_T1225[0];_T1227=_T1226.l;_T1228=_T1227.first_line;_T1229=
Cyc_Position_loc_to_seg(_T1228);{struct _tuple0*_T1878=_cycalloc(sizeof(struct _tuple0));_T1878->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));_T122C=yyyvsp;_T122D=& _T122C[0].v;_T122E=(union Cyc_YYSTYPE*)_T122D;*_T1879=Cyc_yyget_String_tok(_T122E);_T122B=(struct _fat_ptr*)_T1879;}_T1878->f1=_T122B;_T122A=(struct _tuple0*)_T1878;}_T122F=
Cyc_Absyn_var_type(tv);_T1230=Cyc_Absyn_tag_type(_T122F);
# 2892
_T1877->f2=Cyc_Absyn_new_vardecl(_T1229,_T122A,_T1230,0,0);_T1224=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_T1877;}_T1231=(void*)_T1224;_T1232=yyyvsp;_T1233=_T1232[0];_T1234=_T1233.l;_T1235=_T1234.first_line;_T1236=
# 2894
Cyc_Position_loc_to_seg(_T1235);_T1237=
# 2891
Cyc_Absyn_new_pat(_T1231,_T1236);yyval=Cyc_YY9(_T1237);goto _LL0;}case 438:{struct _tuple23*_T1877=_cycalloc(sizeof(struct _tuple23));_T1239=yyyvsp;_T123A=& _T1239[0].v;_T123B=(union Cyc_YYSTYPE*)_T123A;_T123C=
# 2898 "parse.y"
Cyc_yyget_YY11(_T123B);_T1877->f0=Cyc_List_rev(_T123C);_T1877->f1=0;_T1238=(struct _tuple23*)_T1877;}yyval=Cyc_YY10(_T1238);goto _LL0;case 439:{struct _tuple23*_T1877=_cycalloc(sizeof(struct _tuple23));_T123E=yyyvsp;_T123F=& _T123E[0].v;_T1240=(union Cyc_YYSTYPE*)_T123F;_T1241=
# 2899 "parse.y"
Cyc_yyget_YY11(_T1240);_T1877->f0=Cyc_List_rev(_T1241);_T1877->f1=1;_T123D=(struct _tuple23*)_T1877;}yyval=Cyc_YY10(_T123D);goto _LL0;case 440:{struct _tuple23*_T1877=_cycalloc(sizeof(struct _tuple23));
# 2900 "parse.y"
_T1877->f0=0;_T1877->f1=1;_T1242=(struct _tuple23*)_T1877;}yyval=Cyc_YY10(_T1242);goto _LL0;case 441:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1244=yyyvsp;_T1245=& _T1244[0].v;_T1246=(union Cyc_YYSTYPE*)_T1245;
# 2903
_T1877->hd=Cyc_yyget_YY9(_T1246);_T1877->tl=0;_T1243=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY11(_T1243);goto _LL0;case 442:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1248=yyyvsp;_T1249=& _T1248[2].v;_T124A=(union Cyc_YYSTYPE*)_T1249;
# 2904 "parse.y"
_T1877->hd=Cyc_yyget_YY9(_T124A);_T124B=yyyvsp;_T124C=& _T124B[0].v;_T124D=(union Cyc_YYSTYPE*)_T124C;_T1877->tl=Cyc_yyget_YY11(_T124D);_T1247=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY11(_T1247);goto _LL0;case 443:{struct _tuple24*_T1877=_cycalloc(sizeof(struct _tuple24));
# 2907
_T1877->f0=0;_T124F=yyyvsp;_T1250=& _T124F[0].v;_T1251=(union Cyc_YYSTYPE*)_T1250;_T1877->f1=Cyc_yyget_YY9(_T1251);_T124E=(struct _tuple24*)_T1877;}yyval=Cyc_YY12(_T124E);goto _LL0;case 444:{struct _tuple24*_T1877=_cycalloc(sizeof(struct _tuple24));_T1253=yyyvsp;_T1254=& _T1253[0].v;_T1255=(union Cyc_YYSTYPE*)_T1254;
# 2908 "parse.y"
_T1877->f0=Cyc_yyget_YY42(_T1255);_T1256=yyyvsp;_T1257=& _T1256[1].v;_T1258=(union Cyc_YYSTYPE*)_T1257;_T1877->f1=Cyc_yyget_YY9(_T1258);_T1252=(struct _tuple24*)_T1877;}yyval=Cyc_YY12(_T1252);goto _LL0;case 445:{struct _tuple23*_T1877=_cycalloc(sizeof(struct _tuple23));_T125A=yyyvsp;_T125B=& _T125A[0].v;_T125C=(union Cyc_YYSTYPE*)_T125B;_T125D=
# 2911
Cyc_yyget_YY13(_T125C);_T1877->f0=Cyc_List_rev(_T125D);_T1877->f1=0;_T1259=(struct _tuple23*)_T1877;}yyval=Cyc_YY14(_T1259);goto _LL0;case 446:{struct _tuple23*_T1877=_cycalloc(sizeof(struct _tuple23));_T125F=yyyvsp;_T1260=& _T125F[0].v;_T1261=(union Cyc_YYSTYPE*)_T1260;_T1262=
# 2912 "parse.y"
Cyc_yyget_YY13(_T1261);_T1877->f0=Cyc_List_rev(_T1262);_T1877->f1=1;_T125E=(struct _tuple23*)_T1877;}yyval=Cyc_YY14(_T125E);goto _LL0;case 447:{struct _tuple23*_T1877=_cycalloc(sizeof(struct _tuple23));
# 2913 "parse.y"
_T1877->f0=0;_T1877->f1=1;_T1263=(struct _tuple23*)_T1877;}yyval=Cyc_YY14(_T1263);goto _LL0;case 448:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1265=yyyvsp;_T1266=& _T1265[0].v;_T1267=(union Cyc_YYSTYPE*)_T1266;
# 2916
_T1877->hd=Cyc_yyget_YY12(_T1267);_T1877->tl=0;_T1264=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY13(_T1264);goto _LL0;case 449:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1269=yyyvsp;_T126A=& _T1269[2].v;_T126B=(union Cyc_YYSTYPE*)_T126A;
# 2917 "parse.y"
_T1877->hd=Cyc_yyget_YY12(_T126B);_T126C=yyyvsp;_T126D=& _T126C[0].v;_T126E=(union Cyc_YYSTYPE*)_T126D;_T1877->tl=Cyc_yyget_YY13(_T126E);_T1268=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY13(_T1268);goto _LL0;case 450: _T126F=yyyvsp;_T1270=_T126F[0];
# 2922 "parse.y"
yyval=_T1270.v;goto _LL0;case 451: _T1271=yyyvsp;_T1272=& _T1271[0].v;_T1273=(union Cyc_YYSTYPE*)_T1272;_T1274=
# 2923 "parse.y"
Cyc_yyget_Exp_tok(_T1273);_T1275=yyyvsp;_T1276=& _T1275[2].v;_T1277=(union Cyc_YYSTYPE*)_T1276;_T1278=Cyc_yyget_Exp_tok(_T1277);_T1279=yyyvsp;_T127A=_T1279[0];_T127B=_T127A.l;_T127C=_T127B.first_line;_T127D=Cyc_Position_loc_to_seg(_T127C);_T127E=Cyc_Absyn_seq_exp(_T1274,_T1278,_T127D);yyval=Cyc_Exp_tok(_T127E);goto _LL0;case 452: _T127F=yyyvsp;_T1280=_T127F[0];
# 2927 "parse.y"
yyval=_T1280.v;goto _LL0;case 453: _T1281=yyyvsp;_T1282=& _T1281[0].v;_T1283=(union Cyc_YYSTYPE*)_T1282;_T1284=
# 2929 "parse.y"
Cyc_yyget_Exp_tok(_T1283);_T1285=yyyvsp;_T1286=& _T1285[1].v;_T1287=(union Cyc_YYSTYPE*)_T1286;_T1288=Cyc_yyget_YY7(_T1287);_T1289=yyyvsp;_T128A=& _T1289[2].v;_T128B=(union Cyc_YYSTYPE*)_T128A;_T128C=Cyc_yyget_Exp_tok(_T128B);_T128D=yyyvsp;_T128E=_T128D[0];_T128F=_T128E.l;_T1290=_T128F.first_line;_T1291=Cyc_Position_loc_to_seg(_T1290);_T1292=Cyc_Absyn_assignop_exp(_T1284,_T1288,_T128C,_T1291);yyval=Cyc_Exp_tok(_T1292);goto _LL0;case 454: _T1293=yyyvsp;_T1294=& _T1293[0].v;_T1295=(union Cyc_YYSTYPE*)_T1294;_T1296=
# 2931 "parse.y"
Cyc_yyget_Exp_tok(_T1295);_T1297=yyyvsp;_T1298=& _T1297[2].v;_T1299=(union Cyc_YYSTYPE*)_T1298;_T129A=Cyc_yyget_Exp_tok(_T1299);_T129B=yyyvsp;_T129C=_T129B[0];_T129D=_T129C.l;_T129E=_T129D.first_line;_T129F=Cyc_Position_loc_to_seg(_T129E);_T12A0=Cyc_Absyn_swap_exp(_T1296,_T129A,_T129F);yyval=Cyc_Exp_tok(_T12A0);goto _LL0;case 455:
# 2935 "parse.y"
 yyval=Cyc_YY7(0);goto _LL0;case 456:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2936 "parse.y"
_T1877->v=(void*)1U;_T12A1=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A1);goto _LL0;case 457:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2937 "parse.y"
_T1877->v=(void*)3U;_T12A2=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A2);goto _LL0;case 458:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2938 "parse.y"
_T1877->v=(void*)4U;_T12A3=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A3);goto _LL0;case 459:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2939 "parse.y"
_T1877->v=(void*)0U;_T12A4=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A4);goto _LL0;case 460:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2940 "parse.y"
_T1877->v=(void*)2U;_T12A5=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A5);goto _LL0;case 461:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2941 "parse.y"
_T1877->v=(void*)16U;_T12A6=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A6);goto _LL0;case 462:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2942 "parse.y"
_T1877->v=(void*)17U;_T12A7=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A7);goto _LL0;case 463:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2943 "parse.y"
_T1877->v=(void*)13U;_T12A8=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A8);goto _LL0;case 464:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2944 "parse.y"
_T1877->v=(void*)15U;_T12A9=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12A9);goto _LL0;case 465:{struct Cyc_Core_Opt*_T1877=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 2945 "parse.y"
_T1877->v=(void*)14U;_T12AA=(struct Cyc_Core_Opt*)_T1877;}yyval=Cyc_YY7(_T12AA);goto _LL0;case 466: _T12AB=yyyvsp;_T12AC=_T12AB[0];
# 2949 "parse.y"
yyval=_T12AC.v;goto _LL0;case 467: _T12AD=yyyvsp;_T12AE=& _T12AD[0].v;_T12AF=(union Cyc_YYSTYPE*)_T12AE;_T12B0=
# 2951 "parse.y"
Cyc_yyget_Exp_tok(_T12AF);_T12B1=yyyvsp;_T12B2=& _T12B1[2].v;_T12B3=(union Cyc_YYSTYPE*)_T12B2;_T12B4=Cyc_yyget_Exp_tok(_T12B3);_T12B5=yyyvsp;_T12B6=& _T12B5[4].v;_T12B7=(union Cyc_YYSTYPE*)_T12B6;_T12B8=Cyc_yyget_Exp_tok(_T12B7);_T12B9=yyyvsp;_T12BA=_T12B9[0];_T12BB=_T12BA.l;_T12BC=_T12BB.first_line;_T12BD=Cyc_Position_loc_to_seg(_T12BC);_T12BE=Cyc_Absyn_conditional_exp(_T12B0,_T12B4,_T12B8,_T12BD);yyval=Cyc_Exp_tok(_T12BE);goto _LL0;case 468: _T12BF=yyyvsp;_T12C0=& _T12BF[1].v;_T12C1=(union Cyc_YYSTYPE*)_T12C0;_T12C2=
# 2953 "parse.y"
Cyc_yyget_Exp_tok(_T12C1);_T12C3=yyyvsp;_T12C4=_T12C3[0];_T12C5=_T12C4.l;_T12C6=_T12C5.first_line;_T12C7=Cyc_Position_loc_to_seg(_T12C6);_T12C8=Cyc_Absyn_throw_exp(_T12C2,_T12C7);yyval=Cyc_Exp_tok(_T12C8);goto _LL0;case 469: _T12C9=yyyvsp;_T12CA=& _T12C9[1].v;_T12CB=(union Cyc_YYSTYPE*)_T12CA;_T12CC=
# 2955 "parse.y"
Cyc_yyget_Exp_tok(_T12CB);_T12CD=yyyvsp;_T12CE=_T12CD[0];_T12CF=_T12CE.l;_T12D0=_T12CF.first_line;_T12D1=Cyc_Position_loc_to_seg(_T12D0);_T12D2=Cyc_Absyn_New_exp(0,_T12CC,0,_T12D1);yyval=Cyc_Exp_tok(_T12D2);goto _LL0;case 470: _T12D3=yyyvsp;_T12D4=& _T12D3[1].v;_T12D5=(union Cyc_YYSTYPE*)_T12D4;_T12D6=
# 2956 "parse.y"
Cyc_yyget_Exp_tok(_T12D5);_T12D7=yyyvsp;_T12D8=_T12D7[0];_T12D9=_T12D8.l;_T12DA=_T12D9.first_line;_T12DB=Cyc_Position_loc_to_seg(_T12DA);_T12DC=Cyc_Absyn_New_exp(0,_T12D6,0,_T12DB);yyval=Cyc_Exp_tok(_T12DC);goto _LL0;case 471: _T12DD=yyyvsp;_T12DE=& _T12DD[4].v;_T12DF=(union Cyc_YYSTYPE*)_T12DE;_T12E0=
# 2957 "parse.y"
Cyc_yyget_Exp_tok(_T12DF);_T12E1=yyyvsp;_T12E2=& _T12E1[2].v;_T12E3=(union Cyc_YYSTYPE*)_T12E2;_T12E4=Cyc_yyget_Exp_tok(_T12E3);_T12E5=yyyvsp;_T12E6=_T12E5[0];_T12E7=_T12E6.l;_T12E8=_T12E7.first_line;_T12E9=Cyc_Position_loc_to_seg(_T12E8);_T12EA=Cyc_Absyn_New_exp(0,_T12E0,_T12E4,_T12E9);yyval=Cyc_Exp_tok(_T12EA);goto _LL0;case 472: _T12EB=yyyvsp;_T12EC=& _T12EB[4].v;_T12ED=(union Cyc_YYSTYPE*)_T12EC;_T12EE=
# 2958 "parse.y"
Cyc_yyget_Exp_tok(_T12ED);_T12EF=yyyvsp;_T12F0=& _T12EF[2].v;_T12F1=(union Cyc_YYSTYPE*)_T12F0;_T12F2=Cyc_yyget_Exp_tok(_T12F1);_T12F3=yyyvsp;_T12F4=_T12F3[0];_T12F5=_T12F4.l;_T12F6=_T12F5.first_line;_T12F7=Cyc_Position_loc_to_seg(_T12F6);_T12F8=Cyc_Absyn_New_exp(0,_T12EE,_T12F2,_T12F7);yyval=Cyc_Exp_tok(_T12F8);goto _LL0;case 473: _T12F9=yyyvsp;_T12FA=& _T12F9[2].v;_T12FB=(union Cyc_YYSTYPE*)_T12FA;_T12FC=
# 2959 "parse.y"
Cyc_yyget_Exp_tok(_T12FB);{struct _tuple16 _T1877=Cyc_Parse_split_seq(_T12FC);struct Cyc_Absyn_Exp*_T1878;struct Cyc_Absyn_Exp*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_Absyn_Exp*rgn=_T1879;struct Cyc_Absyn_Exp*qual=_T1878;_T12FD=rgn;_T12FE=yyyvsp;_T12FF=& _T12FE[4].v;_T1300=(union Cyc_YYSTYPE*)_T12FF;_T1301=Cyc_yyget_Exp_tok(_T1300);_T1302=qual;_T1303=yyyvsp;_T1304=_T1303[0];_T1305=_T1304.l;_T1306=_T1305.first_line;_T1307=Cyc_Position_loc_to_seg(_T1306);_T1308=Cyc_Absyn_New_exp(_T12FD,_T1301,_T1302,_T1307);yyval=Cyc_Exp_tok(_T1308);goto _LL0;}}case 474: _T1309=yyyvsp;_T130A=& _T1309[2].v;_T130B=(union Cyc_YYSTYPE*)_T130A;_T130C=
# 2960 "parse.y"
Cyc_yyget_Exp_tok(_T130B);{struct _tuple16 _T1877=Cyc_Parse_split_seq(_T130C);struct Cyc_Absyn_Exp*_T1878;struct Cyc_Absyn_Exp*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_Absyn_Exp*rgn=_T1879;struct Cyc_Absyn_Exp*qual=_T1878;_T130D=rgn;_T130E=yyyvsp;_T130F=& _T130E[4].v;_T1310=(union Cyc_YYSTYPE*)_T130F;_T1311=Cyc_yyget_Exp_tok(_T1310);_T1312=qual;_T1313=yyyvsp;_T1314=_T1313[0];_T1315=_T1314.l;_T1316=_T1315.first_line;_T1317=Cyc_Position_loc_to_seg(_T1316);_T1318=Cyc_Absyn_New_exp(_T130D,_T1311,_T1312,_T1317);yyval=Cyc_Exp_tok(_T1318);goto _LL0;}}case 475: _T1319=yyyvsp;_T131A=_T1319[0];
# 2972 "parse.y"
yyval=_T131A.v;goto _LL0;case 476: _T131B=yyyvsp;_T131C=_T131B[0];
# 2975
yyval=_T131C.v;goto _LL0;case 477: _T131D=yyyvsp;_T131E=& _T131D[0].v;_T131F=(union Cyc_YYSTYPE*)_T131E;_T1320=
# 2977 "parse.y"
Cyc_yyget_Exp_tok(_T131F);_T1321=yyyvsp;_T1322=& _T1321[2].v;_T1323=(union Cyc_YYSTYPE*)_T1322;_T1324=Cyc_yyget_Exp_tok(_T1323);_T1325=yyyvsp;_T1326=_T1325[0];_T1327=_T1326.l;_T1328=_T1327.first_line;_T1329=Cyc_Position_loc_to_seg(_T1328);_T132A=Cyc_Absyn_or_exp(_T1320,_T1324,_T1329);yyval=Cyc_Exp_tok(_T132A);goto _LL0;case 478: _T132B=yyyvsp;_T132C=_T132B[0];
# 2980
yyval=_T132C.v;goto _LL0;case 479: _T132D=yyyvsp;_T132E=& _T132D[0].v;_T132F=(union Cyc_YYSTYPE*)_T132E;_T1330=
# 2982 "parse.y"
Cyc_yyget_Exp_tok(_T132F);_T1331=yyyvsp;_T1332=& _T1331[2].v;_T1333=(union Cyc_YYSTYPE*)_T1332;_T1334=Cyc_yyget_Exp_tok(_T1333);_T1335=yyyvsp;_T1336=_T1335[0];_T1337=_T1336.l;_T1338=_T1337.first_line;_T1339=Cyc_Position_loc_to_seg(_T1338);_T133A=Cyc_Absyn_and_exp(_T1330,_T1334,_T1339);yyval=Cyc_Exp_tok(_T133A);goto _LL0;case 480: _T133B=yyyvsp;_T133C=_T133B[0];
# 2985
yyval=_T133C.v;goto _LL0;case 481: _T133D=yyyvsp;_T133E=& _T133D[0].v;_T133F=(union Cyc_YYSTYPE*)_T133E;_T1340=
# 2987 "parse.y"
Cyc_yyget_Exp_tok(_T133F);_T1341=yyyvsp;_T1342=& _T1341[2].v;_T1343=(union Cyc_YYSTYPE*)_T1342;_T1344=Cyc_yyget_Exp_tok(_T1343);_T1345=yyyvsp;_T1346=_T1345[0];_T1347=_T1346.l;_T1348=_T1347.first_line;_T1349=Cyc_Position_loc_to_seg(_T1348);_T134A=Cyc_Absyn_prim2_exp(14U,_T1340,_T1344,_T1349);yyval=Cyc_Exp_tok(_T134A);goto _LL0;case 482: _T134B=yyyvsp;_T134C=_T134B[0];
# 2990
yyval=_T134C.v;goto _LL0;case 483: _T134D=yyyvsp;_T134E=& _T134D[0].v;_T134F=(union Cyc_YYSTYPE*)_T134E;_T1350=
# 2992 "parse.y"
Cyc_yyget_Exp_tok(_T134F);_T1351=yyyvsp;_T1352=& _T1351[2].v;_T1353=(union Cyc_YYSTYPE*)_T1352;_T1354=Cyc_yyget_Exp_tok(_T1353);_T1355=yyyvsp;_T1356=_T1355[0];_T1357=_T1356.l;_T1358=_T1357.first_line;_T1359=Cyc_Position_loc_to_seg(_T1358);_T135A=Cyc_Absyn_prim2_exp(15U,_T1350,_T1354,_T1359);yyval=Cyc_Exp_tok(_T135A);goto _LL0;case 484: _T135B=yyyvsp;_T135C=_T135B[0];
# 2995
yyval=_T135C.v;goto _LL0;case 485: _T135D=yyyvsp;_T135E=& _T135D[0].v;_T135F=(union Cyc_YYSTYPE*)_T135E;_T1360=
# 2997 "parse.y"
Cyc_yyget_Exp_tok(_T135F);_T1361=yyyvsp;_T1362=& _T1361[2].v;_T1363=(union Cyc_YYSTYPE*)_T1362;_T1364=Cyc_yyget_Exp_tok(_T1363);_T1365=yyyvsp;_T1366=_T1365[0];_T1367=_T1366.l;_T1368=_T1367.first_line;_T1369=Cyc_Position_loc_to_seg(_T1368);_T136A=Cyc_Absyn_prim2_exp(13U,_T1360,_T1364,_T1369);yyval=Cyc_Exp_tok(_T136A);goto _LL0;case 486: _T136B=yyyvsp;_T136C=_T136B[0];
# 3000
yyval=_T136C.v;goto _LL0;case 487: _T136D=yyyvsp;_T136E=& _T136D[1].v;_T136F=(union Cyc_YYSTYPE*)_T136E;_T1370=
# 3002 "parse.y"
Cyc_yyget_YY69(_T136F);_T1371=yyyvsp;_T1372=& _T1371[0].v;_T1373=(union Cyc_YYSTYPE*)_T1372;_T1374=Cyc_yyget_Exp_tok(_T1373);_T1375=yyyvsp;_T1376=& _T1375[2].v;_T1377=(union Cyc_YYSTYPE*)_T1376;_T1378=Cyc_yyget_Exp_tok(_T1377);_T1379=yyyvsp;_T137A=_T1379[0];_T137B=_T137A.l;_T137C=_T137B.first_line;_T137D=Cyc_Position_loc_to_seg(_T137C);_T137E=_T1370(_T1374,_T1378,_T137D);yyval=Cyc_Exp_tok(_T137E);goto _LL0;case 488: _T137F=yyyvsp;_T1380=_T137F[0];
# 3005
yyval=_T1380.v;goto _LL0;case 489: _T1381=yyyvsp;_T1382=& _T1381[1].v;_T1383=(union Cyc_YYSTYPE*)_T1382;_T1384=
# 3007 "parse.y"
Cyc_yyget_YY69(_T1383);_T1385=yyyvsp;_T1386=& _T1385[0].v;_T1387=(union Cyc_YYSTYPE*)_T1386;_T1388=Cyc_yyget_Exp_tok(_T1387);_T1389=yyyvsp;_T138A=& _T1389[2].v;_T138B=(union Cyc_YYSTYPE*)_T138A;_T138C=Cyc_yyget_Exp_tok(_T138B);_T138D=yyyvsp;_T138E=_T138D[0];_T138F=_T138E.l;_T1390=_T138F.first_line;_T1391=Cyc_Position_loc_to_seg(_T1390);_T1392=_T1384(_T1388,_T138C,_T1391);yyval=Cyc_Exp_tok(_T1392);goto _LL0;case 490: _T1393=yyyvsp;_T1394=_T1393[0];
# 3010
yyval=_T1394.v;goto _LL0;case 491: _T1395=yyyvsp;_T1396=& _T1395[0].v;_T1397=(union Cyc_YYSTYPE*)_T1396;_T1398=
# 3012 "parse.y"
Cyc_yyget_Exp_tok(_T1397);_T1399=yyyvsp;_T139A=& _T1399[2].v;_T139B=(union Cyc_YYSTYPE*)_T139A;_T139C=Cyc_yyget_Exp_tok(_T139B);_T139D=yyyvsp;_T139E=_T139D[0];_T139F=_T139E.l;_T13A0=_T139F.first_line;_T13A1=Cyc_Position_loc_to_seg(_T13A0);_T13A2=Cyc_Absyn_prim2_exp(16U,_T1398,_T139C,_T13A1);yyval=Cyc_Exp_tok(_T13A2);goto _LL0;case 492: _T13A3=yyyvsp;_T13A4=& _T13A3[0].v;_T13A5=(union Cyc_YYSTYPE*)_T13A4;_T13A6=
# 3014 "parse.y"
Cyc_yyget_Exp_tok(_T13A5);_T13A7=yyyvsp;_T13A8=& _T13A7[2].v;_T13A9=(union Cyc_YYSTYPE*)_T13A8;_T13AA=Cyc_yyget_Exp_tok(_T13A9);_T13AB=yyyvsp;_T13AC=_T13AB[0];_T13AD=_T13AC.l;_T13AE=_T13AD.first_line;_T13AF=Cyc_Position_loc_to_seg(_T13AE);_T13B0=Cyc_Absyn_prim2_exp(17U,_T13A6,_T13AA,_T13AF);yyval=Cyc_Exp_tok(_T13B0);goto _LL0;case 493: _T13B1=yyyvsp;_T13B2=_T13B1[0];
# 3017
yyval=_T13B2.v;goto _LL0;case 494: _T13B3=yyyvsp;_T13B4=& _T13B3[1].v;_T13B5=(union Cyc_YYSTYPE*)_T13B4;_T13B6=
# 3019 "parse.y"
Cyc_yyget_YY6(_T13B5);_T13B7=yyyvsp;_T13B8=& _T13B7[0].v;_T13B9=(union Cyc_YYSTYPE*)_T13B8;_T13BA=Cyc_yyget_Exp_tok(_T13B9);_T13BB=yyyvsp;_T13BC=& _T13BB[2].v;_T13BD=(union Cyc_YYSTYPE*)_T13BC;_T13BE=Cyc_yyget_Exp_tok(_T13BD);_T13BF=yyyvsp;_T13C0=_T13BF[0];_T13C1=_T13C0.l;_T13C2=_T13C1.first_line;_T13C3=Cyc_Position_loc_to_seg(_T13C2);_T13C4=Cyc_Absyn_prim2_exp(_T13B6,_T13BA,_T13BE,_T13C3);yyval=Cyc_Exp_tok(_T13C4);goto _LL0;case 495: _T13C5=yyyvsp;_T13C6=_T13C5[0];
# 3022
yyval=_T13C6.v;goto _LL0;case 496: _T13C7=yyyvsp;_T13C8=& _T13C7[1].v;_T13C9=(union Cyc_YYSTYPE*)_T13C8;_T13CA=
# 3024 "parse.y"
Cyc_yyget_YY6(_T13C9);_T13CB=yyyvsp;_T13CC=& _T13CB[0].v;_T13CD=(union Cyc_YYSTYPE*)_T13CC;_T13CE=Cyc_yyget_Exp_tok(_T13CD);_T13CF=yyyvsp;_T13D0=& _T13CF[2].v;_T13D1=(union Cyc_YYSTYPE*)_T13D0;_T13D2=Cyc_yyget_Exp_tok(_T13D1);_T13D3=yyyvsp;_T13D4=_T13D3[0];_T13D5=_T13D4.l;_T13D6=_T13D5.first_line;_T13D7=Cyc_Position_loc_to_seg(_T13D6);_T13D8=Cyc_Absyn_prim2_exp(_T13CA,_T13CE,_T13D2,_T13D7);yyval=Cyc_Exp_tok(_T13D8);goto _LL0;case 497: _T13D9=Cyc_Absyn_eq_exp;
# 3027
yyval=Cyc_YY69(_T13D9);goto _LL0;case 498: _T13DA=Cyc_Absyn_neq_exp;
# 3028 "parse.y"
yyval=Cyc_YY69(_T13DA);goto _LL0;case 499: _T13DB=Cyc_Absyn_lt_exp;
# 3031
yyval=Cyc_YY69(_T13DB);goto _LL0;case 500: _T13DC=Cyc_Absyn_gt_exp;
# 3032 "parse.y"
yyval=Cyc_YY69(_T13DC);goto _LL0;case 501: _T13DD=Cyc_Absyn_lte_exp;
# 3033 "parse.y"
yyval=Cyc_YY69(_T13DD);goto _LL0;case 502: _T13DE=Cyc_Absyn_gte_exp;
# 3034 "parse.y"
yyval=Cyc_YY69(_T13DE);goto _LL0;case 503:
# 3037
 yyval=Cyc_YY6(0U);goto _LL0;case 504:
# 3038 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 505:
# 3041
 yyval=Cyc_YY6(1U);goto _LL0;case 506:
# 3042 "parse.y"
 yyval=Cyc_YY6(3U);goto _LL0;case 507:
# 3043 "parse.y"
 yyval=Cyc_YY6(4U);goto _LL0;case 508: _T13DF=yyyvsp;_T13E0=_T13DF[0];
# 3047 "parse.y"
yyval=_T13E0.v;goto _LL0;case 509: _T13E1=yyyvsp;_T13E2=& _T13E1[1].v;_T13E3=(union Cyc_YYSTYPE*)_T13E2;_T13E4=
# 3049 "parse.y"
Cyc_yyget_YY38(_T13E3);_T13E5=yyyvsp;_T13E6=_T13E5[1];_T13E7=_T13E6.l;_T13E8=_T13E7.first_line;_T13E9=Cyc_Position_loc_to_seg(_T13E8);{void*t=Cyc_Parse_type_name_to_type(_T13E4,_T13E9);_T13EA=t;_T13EB=yyyvsp;_T13EC=& _T13EB[3].v;_T13ED=(union Cyc_YYSTYPE*)_T13EC;_T13EE=
Cyc_yyget_Exp_tok(_T13ED);_T13EF=yyyvsp;_T13F0=_T13EF[0];_T13F1=_T13F0.l;_T13F2=_T13F1.first_line;_T13F3=Cyc_Position_loc_to_seg(_T13F2);_T13F4=Cyc_Absyn_cast_exp(_T13EA,_T13EE,1,0U,_T13F3);yyval=Cyc_Exp_tok(_T13F4);goto _LL0;}case 510: _T13F5=yyyvsp;_T13F6=_T13F5[0];
# 3054 "parse.y"
yyval=_T13F6.v;goto _LL0;case 511: _T13F7=yyyvsp;_T13F8=& _T13F7[1].v;_T13F9=(union Cyc_YYSTYPE*)_T13F8;_T13FA=
# 3055 "parse.y"
Cyc_yyget_Exp_tok(_T13F9);_T13FB=yyyvsp;_T13FC=_T13FB[0];_T13FD=_T13FC.l;_T13FE=_T13FD.first_line;_T13FF=Cyc_Position_loc_to_seg(_T13FE);_T1400=Cyc_Absyn_increment_exp(_T13FA,0U,_T13FF);yyval=Cyc_Exp_tok(_T1400);goto _LL0;case 512: _T1401=yyyvsp;_T1402=& _T1401[1].v;_T1403=(union Cyc_YYSTYPE*)_T1402;_T1404=
# 3056 "parse.y"
Cyc_yyget_Exp_tok(_T1403);_T1405=yyyvsp;_T1406=_T1405[0];_T1407=_T1406.l;_T1408=_T1407.first_line;_T1409=Cyc_Position_loc_to_seg(_T1408);_T140A=Cyc_Absyn_increment_exp(_T1404,2U,_T1409);yyval=Cyc_Exp_tok(_T140A);goto _LL0;case 513: _T140B=yyyvsp;_T140C=& _T140B[1].v;_T140D=(union Cyc_YYSTYPE*)_T140C;_T140E=
# 3057 "parse.y"
Cyc_yyget_Exp_tok(_T140D);_T140F=yyyvsp;_T1410=_T140F[0];_T1411=_T1410.l;_T1412=_T1411.first_line;_T1413=Cyc_Position_loc_to_seg(_T1412);_T1414=Cyc_Absyn_address_exp(_T140E,_T1413);yyval=Cyc_Exp_tok(_T1414);goto _LL0;case 514: _T1415=yyyvsp;_T1416=& _T1415[1].v;_T1417=(union Cyc_YYSTYPE*)_T1416;_T1418=
# 3058 "parse.y"
Cyc_yyget_Exp_tok(_T1417);_T1419=yyyvsp;_T141A=_T1419[0];_T141B=_T141A.l;_T141C=_T141B.first_line;_T141D=Cyc_Position_loc_to_seg(_T141C);_T141E=Cyc_Absyn_deref_exp(_T1418,_T141D);yyval=Cyc_Exp_tok(_T141E);goto _LL0;case 515: _T141F=yyyvsp;_T1420=& _T141F[0].v;_T1421=(union Cyc_YYSTYPE*)_T1420;_T1422=
# 3059 "parse.y"
Cyc_yyget_YY6(_T1421);_T1423=yyyvsp;_T1424=& _T1423[1].v;_T1425=(union Cyc_YYSTYPE*)_T1424;_T1426=Cyc_yyget_Exp_tok(_T1425);_T1427=yyyvsp;_T1428=_T1427[0];_T1429=_T1428.l;_T142A=_T1429.first_line;_T142B=Cyc_Position_loc_to_seg(_T142A);_T142C=Cyc_Absyn_prim1_exp(_T1422,_T1426,_T142B);yyval=Cyc_Exp_tok(_T142C);goto _LL0;case 516: _T142D=yyyvsp;_T142E=& _T142D[2].v;_T142F=(union Cyc_YYSTYPE*)_T142E;_T1430=
# 3061 "parse.y"
Cyc_yyget_YY38(_T142F);_T1431=yyyvsp;_T1432=_T1431[2];_T1433=_T1432.l;_T1434=_T1433.first_line;_T1435=Cyc_Position_loc_to_seg(_T1434);{void*t=Cyc_Parse_type_name_to_type(_T1430,_T1435);_T1436=t;_T1437=yyyvsp;_T1438=_T1437[0];_T1439=_T1438.l;_T143A=_T1439.first_line;_T143B=
Cyc_Position_loc_to_seg(_T143A);_T143C=Cyc_Absyn_sizeoftype_exp(_T1436,_T143B);yyval=Cyc_Exp_tok(_T143C);goto _LL0;}case 517: _T143D=yyyvsp;_T143E=& _T143D[1].v;_T143F=(union Cyc_YYSTYPE*)_T143E;_T1440=
# 3064 "parse.y"
Cyc_yyget_Exp_tok(_T143F);_T1441=yyyvsp;_T1442=_T1441[0];_T1443=_T1442.l;_T1444=_T1443.first_line;_T1445=Cyc_Position_loc_to_seg(_T1444);_T1446=Cyc_Absyn_sizeofexp_exp(_T1440,_T1445);yyval=Cyc_Exp_tok(_T1446);goto _LL0;case 518: _T1447=yyyvsp;_T1448=& _T1447[2].v;_T1449=(union Cyc_YYSTYPE*)_T1448;_T144A=
# 3066 "parse.y"
Cyc_yyget_YY38(_T1449);_T144B=yyyvsp;_T144C=_T144B[2];_T144D=_T144C.l;_T144E=_T144D.first_line;_T144F=Cyc_Position_loc_to_seg(_T144E);{void*t=Cyc_Parse_type_name_to_type(_T144A,_T144F);_T1450=t;_T1451=yyyvsp;_T1452=& _T1451[4].v;_T1453=(union Cyc_YYSTYPE*)_T1452;_T1454=
Cyc_yyget_YY3(_T1453);_T1455=Cyc_List_imp_rev(_T1454);_T1456=yyyvsp;_T1457=_T1456[0];_T1458=_T1457.l;_T1459=_T1458.first_line;_T145A=Cyc_Position_loc_to_seg(_T1459);_T145B=Cyc_Absyn_offsetof_exp(_T1450,_T1455,_T145A);yyval=Cyc_Exp_tok(_T145B);goto _LL0;}case 519:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1877->tag=33;
# 3071
_T1877->f1.mknd=0U;_T1877->f1.rgn=0;_T1877->f1.aqual=0;_T1877->f1.elt_type=0;_T145D=yyyvsp;_T145E=& _T145D[2].v;_T145F=(union Cyc_YYSTYPE*)_T145E;_T1877->f1.num_elts=Cyc_yyget_Exp_tok(_T145F);_T1877->f1.fat_result=0;_T1877->f1.inline_call=0;_T145C=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1877;}_T1460=(void*)_T145C;_T1461=yyyvsp;_T1462=_T1461[0];_T1463=_T1462.l;_T1464=_T1463.first_line;_T1465=
Cyc_Position_loc_to_seg(_T1464);_T1466=
# 3071
Cyc_Absyn_new_exp(_T1460,_T1465);yyval=Cyc_Exp_tok(_T1466);goto _LL0;case 520:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1877->tag=33;
# 3074 "parse.y"
_T1877->f1.mknd=0U;_T1468=yyyvsp;_T1469=& _T1468[2].v;_T146A=(union Cyc_YYSTYPE*)_T1469;_T1877->f1.rgn=Cyc_yyget_Exp_tok(_T146A);_T146B=yyyvsp;_T146C=& _T146B[4].v;_T146D=(union Cyc_YYSTYPE*)_T146C;_T1877->f1.aqual=Cyc_yyget_Exp_tok(_T146D);_T1877->f1.elt_type=0;_T146E=yyyvsp;_T146F=& _T146E[6].v;_T1470=(union Cyc_YYSTYPE*)_T146F;_T1877->f1.num_elts=Cyc_yyget_Exp_tok(_T1470);_T1877->f1.fat_result=0;_T1877->f1.inline_call=0;_T1467=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1877;}_T1471=(void*)_T1467;_T1472=yyyvsp;_T1473=_T1472[0];_T1474=_T1473.l;_T1475=_T1474.first_line;_T1476=
Cyc_Position_loc_to_seg(_T1475);_T1477=
# 3074
Cyc_Absyn_new_exp(_T1471,_T1476);yyval=Cyc_Exp_tok(_T1477);goto _LL0;case 521:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1877->tag=33;
# 3077 "parse.y"
_T1877->f1.mknd=0U;_T1479=yyyvsp;_T147A=& _T1479[2].v;_T147B=(union Cyc_YYSTYPE*)_T147A;_T1877->f1.rgn=Cyc_yyget_Exp_tok(_T147B);_T1877->f1.aqual=0;_T1877->f1.elt_type=0;_T147C=yyyvsp;_T147D=& _T147C[4].v;_T147E=(union Cyc_YYSTYPE*)_T147D;_T1877->f1.num_elts=Cyc_yyget_Exp_tok(_T147E);_T1877->f1.fat_result=0;_T1877->f1.inline_call=0;_T1478=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1877;}_T147F=(void*)_T1478;_T1480=yyyvsp;_T1481=_T1480[0];_T1482=_T1481.l;_T1483=_T1482.first_line;_T1484=
Cyc_Position_loc_to_seg(_T1483);_T1485=
# 3077
Cyc_Absyn_new_exp(_T147F,_T1484);yyval=Cyc_Exp_tok(_T1485);goto _LL0;case 522: _T1486=yyyvsp;_T1487=& _T1486[2].v;_T1488=(union Cyc_YYSTYPE*)_T1487;_T1489=
# 3080 "parse.y"
Cyc_yyget_Exp_tok(_T1488);{struct _tuple16 _T1877=Cyc_Parse_split_seq(_T1489);struct Cyc_Absyn_Exp*_T1878;struct Cyc_Absyn_Exp*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_Absyn_Exp*rgn=_T1879;struct Cyc_Absyn_Exp*qual=_T1878;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187A->tag=33;
_T187A->f1.mknd=2U;_T187A->f1.rgn=rgn;_T187A->f1.aqual=qual;_T187A->f1.elt_type=0;_T148B=yyyvsp;_T148C=& _T148B[4].v;_T148D=(union Cyc_YYSTYPE*)_T148C;_T187A->f1.num_elts=Cyc_yyget_Exp_tok(_T148D);_T187A->f1.fat_result=0;_T187A->f1.inline_call=0;_T148A=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187A;}_T148E=(void*)_T148A;_T148F=yyyvsp;_T1490=_T148F[0];_T1491=_T1490.l;_T1492=_T1491.first_line;_T1493=
Cyc_Position_loc_to_seg(_T1492);_T1494=
# 3081
Cyc_Absyn_new_exp(_T148E,_T1493);yyval=Cyc_Exp_tok(_T1494);goto _LL0;}}case 523: _T1495=yyyvsp;_T1496=& _T1495[2].v;_T1497=(union Cyc_YYSTYPE*)_T1496;_T1498=
# 3084 "parse.y"
Cyc_yyget_Exp_tok(_T1497);{struct _tuple16 _T1877=Cyc_Parse_split_seq(_T1498);struct Cyc_Absyn_Exp*_T1878;struct Cyc_Absyn_Exp*_T1879;_T1879=_T1877.f0;_T1878=_T1877.f1;{struct Cyc_Absyn_Exp*rgn=_T1879;struct Cyc_Absyn_Exp*qual=_T1878;{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T187A->tag=33;
_T187A->f1.mknd=0U;_T187A->f1.rgn=rgn;_T187A->f1.aqual=qual;_T187A->f1.elt_type=0;_T149A=yyyvsp;_T149B=& _T149A[4].v;_T149C=(union Cyc_YYSTYPE*)_T149B;_T187A->f1.num_elts=Cyc_yyget_Exp_tok(_T149C);_T187A->f1.fat_result=0;_T187A->f1.inline_call=1;_T1499=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T187A;}_T149D=(void*)_T1499;_T149E=yyyvsp;_T149F=_T149E[0];_T14A0=_T149F.l;_T14A1=_T14A0.first_line;_T14A2=
Cyc_Position_loc_to_seg(_T14A1);_T14A3=
# 3085
Cyc_Absyn_new_exp(_T149D,_T14A2);yyval=Cyc_Exp_tok(_T14A3);goto _LL0;}}case 524: _T14A4=yyyvsp;_T14A5=& _T14A4[6].v;_T14A6=(union Cyc_YYSTYPE*)_T14A5;_T14A7=
# 3088 "parse.y"
Cyc_yyget_YY38(_T14A6);_T14A8=yyyvsp;_T14A9=_T14A8[6];_T14AA=_T14A9.l;_T14AB=_T14AA.first_line;_T14AC=Cyc_Position_loc_to_seg(_T14AB);{void*t=Cyc_Parse_type_name_to_type(_T14A7,_T14AC);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1877->tag=33;
_T1877->f1.mknd=1U;_T1877->f1.rgn=0;_T1877->f1.aqual=0;{void**_T1878=_cycalloc(sizeof(void*));*_T1878=t;_T14AE=(void**)_T1878;}_T1877->f1.elt_type=_T14AE;_T14AF=yyyvsp;_T14B0=& _T14AF[2].v;_T14B1=(union Cyc_YYSTYPE*)_T14B0;_T1877->f1.num_elts=Cyc_yyget_Exp_tok(_T14B1);_T1877->f1.fat_result=0;_T1877->f1.inline_call=0;_T14AD=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1877;}_T14B2=(void*)_T14AD;_T14B3=yyyvsp;_T14B4=_T14B3[0];_T14B5=_T14B4.l;_T14B6=_T14B5.first_line;_T14B7=
Cyc_Position_loc_to_seg(_T14B6);_T14B8=
# 3089
Cyc_Absyn_new_exp(_T14B2,_T14B7);yyval=Cyc_Exp_tok(_T14B8);goto _LL0;}case 525: _T14B9=yyyvsp;_T14BA=& _T14B9[10].v;_T14BB=(union Cyc_YYSTYPE*)_T14BA;_T14BC=
# 3092 "parse.y"
Cyc_yyget_YY38(_T14BB);_T14BD=yyyvsp;_T14BE=_T14BD[10];_T14BF=_T14BE.l;_T14C0=_T14BF.first_line;_T14C1=Cyc_Position_loc_to_seg(_T14C0);{void*t=Cyc_Parse_type_name_to_type(_T14BC,_T14C1);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1877->tag=33;
_T1877->f1.mknd=1U;_T14C3=yyyvsp;_T14C4=& _T14C3[2].v;_T14C5=(union Cyc_YYSTYPE*)_T14C4;_T1877->f1.rgn=Cyc_yyget_Exp_tok(_T14C5);_T14C6=yyyvsp;_T14C7=& _T14C6[4].v;_T14C8=(union Cyc_YYSTYPE*)_T14C7;_T1877->f1.aqual=Cyc_yyget_Exp_tok(_T14C8);{void**_T1878=_cycalloc(sizeof(void*));*_T1878=t;_T14C9=(void**)_T1878;}_T1877->f1.elt_type=_T14C9;_T14CA=yyyvsp;_T14CB=& _T14CA[6].v;_T14CC=(union Cyc_YYSTYPE*)_T14CB;_T1877->f1.num_elts=Cyc_yyget_Exp_tok(_T14CC);_T1877->f1.fat_result=0;_T1877->f1.inline_call=0;_T14C2=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1877;}_T14CD=(void*)_T14C2;_T14CE=yyyvsp;_T14CF=_T14CE[0];_T14D0=_T14CF.l;_T14D1=_T14D0.first_line;_T14D2=
Cyc_Position_loc_to_seg(_T14D1);_T14D3=
# 3093
Cyc_Absyn_new_exp(_T14CD,_T14D2);yyval=Cyc_Exp_tok(_T14D3);goto _LL0;}case 526: _T14D4=yyyvsp;_T14D5=& _T14D4[8].v;_T14D6=(union Cyc_YYSTYPE*)_T14D5;_T14D7=
# 3097
Cyc_yyget_YY38(_T14D6);_T14D8=yyyvsp;_T14D9=_T14D8[8];_T14DA=_T14D9.l;_T14DB=_T14DA.first_line;_T14DC=Cyc_Position_loc_to_seg(_T14DB);{void*t=Cyc_Parse_type_name_to_type(_T14D7,_T14DC);{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct));_T1877->tag=33;
_T1877->f1.mknd=1U;_T14DE=yyyvsp;_T14DF=& _T14DE[2].v;_T14E0=(union Cyc_YYSTYPE*)_T14DF;_T1877->f1.rgn=Cyc_yyget_Exp_tok(_T14E0);_T1877->f1.aqual=0;{void**_T1878=_cycalloc(sizeof(void*));*_T1878=t;_T14E1=(void**)_T1878;}_T1877->f1.elt_type=_T14E1;_T14E2=yyyvsp;_T14E3=& _T14E2[4].v;_T14E4=(union Cyc_YYSTYPE*)_T14E3;_T1877->f1.num_elts=Cyc_yyget_Exp_tok(_T14E4);_T1877->f1.fat_result=0;_T1877->f1.inline_call=0;_T14DD=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_T1877;}_T14E5=(void*)_T14DD;_T14E6=yyyvsp;_T14E7=_T14E6[0];_T14E8=_T14E7.l;_T14E9=_T14E8.first_line;_T14EA=
Cyc_Position_loc_to_seg(_T14E9);_T14EB=
# 3098
Cyc_Absyn_new_exp(_T14E5,_T14EA);yyval=Cyc_Exp_tok(_T14EB);goto _LL0;}case 527:{struct Cyc_Absyn_Exp*_T1877[1];_T14ED=yyyvsp;_T14EE=& _T14ED[2].v;_T14EF=(union Cyc_YYSTYPE*)_T14EE;_T1877[0]=
# 3101 "parse.y"
Cyc_yyget_Exp_tok(_T14EF);_T14F0=_tag_fat(_T1877,sizeof(struct Cyc_Absyn_Exp*),1);_T14EC=Cyc_List_list(_T14F0);}_T14F1=yyyvsp;_T14F2=_T14F1[0];_T14F3=_T14F2.l;_T14F4=_T14F3.first_line;_T14F5=Cyc_Position_loc_to_seg(_T14F4);_T14F6=Cyc_Absyn_primop_exp(18U,_T14EC,_T14F5);yyval=Cyc_Exp_tok(_T14F6);goto _LL0;case 528: _T14F7=yyyvsp;_T14F8=& _T14F7[2].v;_T14F9=(union Cyc_YYSTYPE*)_T14F8;_T14FA=
# 3103 "parse.y"
Cyc_yyget_Exp_tok(_T14F9);_T14FB=yyyvsp;_T14FC=_T14FB[0];_T14FD=_T14FC.l;_T14FE=_T14FD.first_line;_T14FF=Cyc_Position_loc_to_seg(_T14FE);_T1500=Cyc_Absyn_tagof_exp(_T14FA,_T14FF);yyval=Cyc_Exp_tok(_T1500);goto _LL0;case 529:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T1877->tag=37;_T1502=yyyvsp;_T1503=& _T1502[2].v;_T1504=(union Cyc_YYSTYPE*)_T1503;
# 3105 "parse.y"
_T1877->f1=Cyc_yyget_Exp_tok(_T1504);{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T1506=yyyvsp;_T1507=& _T1506[4].v;_T1508=(union Cyc_YYSTYPE*)_T1507;*_T1878=Cyc_yyget_String_tok(_T1508);_T1505=(struct _fat_ptr*)_T1878;}_T1877->f2=_T1505;_T1501=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T1877;}_T1509=(void*)_T1501;_T150A=yyyvsp;_T150B=_T150A[0];_T150C=_T150B.l;_T150D=_T150C.first_line;_T150E=Cyc_Position_loc_to_seg(_T150D);_T150F=Cyc_Absyn_new_exp(_T1509,_T150E);yyval=Cyc_Exp_tok(_T150F);goto _LL0;case 530:{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct));_T1877->tag=37;_T1511=yyyvsp;_T1512=& _T1511[2].v;_T1513=(union Cyc_YYSTYPE*)_T1512;_T1514=
# 3107 "parse.y"
Cyc_yyget_Exp_tok(_T1513);_T1515=yyyvsp;_T1516=_T1515[2];_T1517=_T1516.l;_T1518=_T1517.first_line;_T1519=Cyc_Position_loc_to_seg(_T1518);_T1877->f1=Cyc_Absyn_deref_exp(_T1514,_T1519);{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T151B=yyyvsp;_T151C=& _T151B[4].v;_T151D=(union Cyc_YYSTYPE*)_T151C;*_T1878=Cyc_yyget_String_tok(_T151D);_T151A=(struct _fat_ptr*)_T1878;}_T1877->f2=_T151A;_T1510=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_T1877;}_T151E=(void*)_T1510;_T151F=yyyvsp;_T1520=_T151F[0];_T1521=_T1520.l;_T1522=_T1521.first_line;_T1523=Cyc_Position_loc_to_seg(_T1522);_T1524=Cyc_Absyn_new_exp(_T151E,_T1523);yyval=Cyc_Exp_tok(_T1524);goto _LL0;case 531: _T1525=yyyvsp;_T1526=& _T1525[2].v;_T1527=(union Cyc_YYSTYPE*)_T1526;_T1528=
# 3109 "parse.y"
Cyc_yyget_YY38(_T1527);_T1529=yyyvsp;_T152A=_T1529[2];_T152B=_T152A.l;_T152C=_T152B.first_line;_T152D=Cyc_Position_loc_to_seg(_T152C);{void*t=Cyc_Parse_type_name_to_type(_T1528,_T152D);_T152E=t;_T152F=yyyvsp;_T1530=_T152F[0];_T1531=_T1530.l;_T1532=_T1531.first_line;_T1533=
Cyc_Position_loc_to_seg(_T1532);_T1534=Cyc_Absyn_valueof_exp(_T152E,_T1533);yyval=Cyc_Exp_tok(_T1534);goto _LL0;}case 532: _T1535=yyyvsp;_T1536=& _T1535[1].v;_T1537=(union Cyc_YYSTYPE*)_T1536;_T1538=
# 3111 "parse.y"
Cyc_yyget_YY63(_T1537);_T1539=yyyvsp;_T153A=_T1539[0];_T153B=_T153A.l;_T153C=_T153B.first_line;_T153D=Cyc_Position_loc_to_seg(_T153C);_T153E=Cyc_Absyn_new_exp(_T1538,_T153D);yyval=Cyc_Exp_tok(_T153E);goto _LL0;case 533: _T153F=yyyvsp;_T1540=& _T153F[1].v;_T1541=(union Cyc_YYSTYPE*)_T1540;_T1542=
# 3112 "parse.y"
Cyc_yyget_Exp_tok(_T1541);_T1543=yyyvsp;_T1544=_T1543[0];_T1545=_T1544.l;_T1546=_T1545.first_line;_T1547=Cyc_Position_loc_to_seg(_T1546);_T1548=Cyc_Absyn_extension_exp(_T1542,_T1547);yyval=Cyc_Exp_tok(_T1548);goto _LL0;case 534: _T1549=yyyvsp;_T154A=& _T1549[2].v;_T154B=(union Cyc_YYSTYPE*)_T154A;_T154C=
# 3113 "parse.y"
Cyc_yyget_Exp_tok(_T154B);_T154D=yyyvsp;_T154E=_T154D[0];_T154F=_T154E.l;_T1550=_T154F.first_line;_T1551=Cyc_Position_loc_to_seg(_T1550);_T1552=Cyc_Absyn_assert_exp(_T154C,0,_T1551);yyval=Cyc_Exp_tok(_T1552);goto _LL0;case 535: _T1553=yyyvsp;_T1554=& _T1553[2].v;_T1555=(union Cyc_YYSTYPE*)_T1554;_T1556=
# 3114 "parse.y"
Cyc_yyget_Exp_tok(_T1555);_T1557=yyyvsp;_T1558=_T1557[0];_T1559=_T1558.l;_T155A=_T1559.first_line;_T155B=Cyc_Position_loc_to_seg(_T155A);_T155C=Cyc_Absyn_assert_exp(_T1556,1,_T155B);yyval=Cyc_Exp_tok(_T155C);goto _LL0;case 536: _T155D=yyyvsp;_T155E=& _T155D[2].v;_T155F=(union Cyc_YYSTYPE*)_T155E;_T1560=
# 3115 "parse.y"
Cyc_yyget_Exp_tok(_T155F);_T1561=yyyvsp;_T1562=_T1561[0];_T1563=_T1562.l;_T1564=_T1563.first_line;_T1565=Cyc_Position_loc_to_seg(_T1564);_T1566=Cyc_Absyn_assert_false_exp(_T1560,_T1565);yyval=Cyc_Exp_tok(_T1566);goto _LL0;case 537:
# 3119 "parse.y"
 yyval=Cyc_YY6(12U);goto _LL0;case 538:
# 3120 "parse.y"
 yyval=Cyc_YY6(11U);goto _LL0;case 539:
# 3121 "parse.y"
 yyval=Cyc_YY6(2U);goto _LL0;case 540:
# 3122 "parse.y"
 yyval=Cyc_YY6(0U);goto _LL0;case 541: _T1567=yyyvsp;_T1568=& _T1567[3].v;_T1569=(union Cyc_YYSTYPE*)_T1568;{
# 3127 "parse.y"
struct _tuple31*_T1877=Cyc_yyget_YY64(_T1569);struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;struct Cyc_List_List*_T187A;{struct _tuple31 _T187B=*_T1877;_T187A=_T187B.f0;_T1879=_T187B.f1;_T1878=_T187B.f2;}{struct Cyc_List_List*outlist=_T187A;struct Cyc_List_List*inlist=_T1879;struct Cyc_List_List*clobbers=_T1878;{struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_T187B=_cycalloc(sizeof(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct));_T187B->tag=39;_T156B=yyyvsp;_T156C=& _T156B[0].v;_T156D=(union Cyc_YYSTYPE*)_T156C;
_T187B->f1=Cyc_yyget_YY32(_T156D);_T156E=yyyvsp;_T156F=& _T156E[2].v;_T1570=(union Cyc_YYSTYPE*)_T156F;_T187B->f2=Cyc_yyget_String_tok(_T1570);_T187B->f3=outlist;_T187B->f4=inlist;_T187B->f5=clobbers;_T156A=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_T187B;}_T1571=(void*)_T156A;yyval=Cyc_YY63(_T1571);goto _LL0;}}case 542:
# 3132 "parse.y"
 yyval=Cyc_YY32(0);goto _LL0;case 543:
# 3133 "parse.y"
 yyval=Cyc_YY32(1);goto _LL0;case 544:{struct _tuple31*_T1877=_cycalloc(sizeof(struct _tuple31));
# 3137 "parse.y"
_T1877->f0=0;_T1877->f1=0;_T1877->f2=0;_T1572=(struct _tuple31*)_T1877;}yyval=Cyc_YY64(_T1572);goto _LL0;case 545: _T1573=yyyvsp;_T1574=& _T1573[1].v;_T1575=(union Cyc_YYSTYPE*)_T1574;{
# 3139 "parse.y"
struct _tuple28*_T1877=Cyc_yyget_YY65(_T1575);struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;{struct _tuple28 _T187A=*_T1877;_T1879=_T187A.f0;_T1878=_T187A.f1;}{struct Cyc_List_List*inlist=_T1879;struct Cyc_List_List*clobbers=_T1878;{struct _tuple31*_T187A=_cycalloc(sizeof(struct _tuple31));
_T187A->f0=0;_T187A->f1=inlist;_T187A->f2=clobbers;_T1576=(struct _tuple31*)_T187A;}yyval=Cyc_YY64(_T1576);goto _LL0;}}case 546: _T1577=yyyvsp;_T1578=& _T1577[2].v;_T1579=(union Cyc_YYSTYPE*)_T1578;{
# 3142 "parse.y"
struct _tuple28*_T1877=Cyc_yyget_YY65(_T1579);struct Cyc_List_List*_T1878;struct Cyc_List_List*_T1879;{struct _tuple28 _T187A=*_T1877;_T1879=_T187A.f0;_T1878=_T187A.f1;}{struct Cyc_List_List*inlist=_T1879;struct Cyc_List_List*clobbers=_T1878;{struct _tuple31*_T187A=_cycalloc(sizeof(struct _tuple31));_T157B=yyyvsp;_T157C=& _T157B[1].v;_T157D=(union Cyc_YYSTYPE*)_T157C;_T157E=
Cyc_yyget_YY67(_T157D);_T187A->f0=Cyc_List_imp_rev(_T157E);_T187A->f1=inlist;_T187A->f2=clobbers;_T157A=(struct _tuple31*)_T187A;}yyval=Cyc_YY64(_T157A);goto _LL0;}}case 547:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1580=yyyvsp;_T1581=& _T1580[0].v;_T1582=(union Cyc_YYSTYPE*)_T1581;
# 3146
_T1877->hd=Cyc_yyget_YY68(_T1582);_T1877->tl=0;_T157F=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY67(_T157F);goto _LL0;case 548:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1584=yyyvsp;_T1585=& _T1584[2].v;_T1586=(union Cyc_YYSTYPE*)_T1585;
# 3147 "parse.y"
_T1877->hd=Cyc_yyget_YY68(_T1586);_T1587=yyyvsp;_T1588=& _T1587[0].v;_T1589=(union Cyc_YYSTYPE*)_T1588;_T1877->tl=Cyc_yyget_YY67(_T1589);_T1583=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY67(_T1583);goto _LL0;case 549:{struct _tuple28*_T1877=_cycalloc(sizeof(struct _tuple28));
# 3151 "parse.y"
_T1877->f0=0;_T1877->f1=0;_T158A=(struct _tuple28*)_T1877;}yyval=Cyc_YY65(_T158A);goto _LL0;case 550:{struct _tuple28*_T1877=_cycalloc(sizeof(struct _tuple28));
# 3152 "parse.y"
_T1877->f0=0;_T158C=yyyvsp;_T158D=& _T158C[1].v;_T158E=(union Cyc_YYSTYPE*)_T158D;_T1877->f1=Cyc_yyget_YY66(_T158E);_T158B=(struct _tuple28*)_T1877;}yyval=Cyc_YY65(_T158B);goto _LL0;case 551:{struct _tuple28*_T1877=_cycalloc(sizeof(struct _tuple28));_T1590=yyyvsp;_T1591=& _T1590[1].v;_T1592=(union Cyc_YYSTYPE*)_T1591;_T1593=
# 3153 "parse.y"
Cyc_yyget_YY67(_T1592);_T1877->f0=Cyc_List_imp_rev(_T1593);_T1594=yyyvsp;_T1595=& _T1594[2].v;_T1596=(union Cyc_YYSTYPE*)_T1595;_T1877->f1=Cyc_yyget_YY66(_T1596);_T158F=(struct _tuple28*)_T1877;}yyval=Cyc_YY65(_T158F);goto _LL0;case 552:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1598=yyyvsp;_T1599=& _T1598[0].v;_T159A=(union Cyc_YYSTYPE*)_T1599;
# 3156
_T1877->hd=Cyc_yyget_YY68(_T159A);_T1877->tl=0;_T1597=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY67(_T1597);goto _LL0;case 553:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T159C=yyyvsp;_T159D=& _T159C[2].v;_T159E=(union Cyc_YYSTYPE*)_T159D;
# 3157 "parse.y"
_T1877->hd=Cyc_yyget_YY68(_T159E);_T159F=yyyvsp;_T15A0=& _T159F[0].v;_T15A1=(union Cyc_YYSTYPE*)_T15A0;_T1877->tl=Cyc_yyget_YY67(_T15A1);_T159B=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY67(_T159B);goto _LL0;case 554:{struct _tuple32*_T1877=_cycalloc(sizeof(struct _tuple32));_T15A3=yyyvsp;_T15A4=& _T15A3[0].v;_T15A5=(union Cyc_YYSTYPE*)_T15A4;
# 3161 "parse.y"
_T1877->f0=Cyc_yyget_String_tok(_T15A5);_T15A6=yyyvsp;_T15A7=& _T15A6[2].v;_T15A8=(union Cyc_YYSTYPE*)_T15A7;_T1877->f1=Cyc_yyget_Exp_tok(_T15A8);_T15A2=(struct _tuple32*)_T1877;}yyval=Cyc_YY68(_T15A2);goto _LL0;case 555:
# 3166 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 556:
# 3167 "parse.y"
 yyval=Cyc_YY66(0);goto _LL0;case 557: _T15A9=yyyvsp;_T15AA=& _T15A9[1].v;_T15AB=(union Cyc_YYSTYPE*)_T15AA;_T15AC=
# 3168 "parse.y"
Cyc_yyget_YY66(_T15AB);_T15AD=Cyc_List_imp_rev(_T15AC);yyval=Cyc_YY66(_T15AD);goto _LL0;case 558:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T15B0=yyyvsp;_T15B1=& _T15B0[0].v;_T15B2=(union Cyc_YYSTYPE*)_T15B1;
# 3171
*_T1878=Cyc_yyget_String_tok(_T15B2);_T15AF=(struct _fat_ptr*)_T1878;}_T1877->hd=_T15AF;_T1877->tl=0;_T15AE=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY66(_T15AE);goto _LL0;case 559:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T15B5=yyyvsp;_T15B6=& _T15B5[2].v;_T15B7=(union Cyc_YYSTYPE*)_T15B6;
# 3172 "parse.y"
*_T1878=Cyc_yyget_String_tok(_T15B7);_T15B4=(struct _fat_ptr*)_T1878;}_T1877->hd=_T15B4;_T15B8=yyyvsp;_T15B9=& _T15B8[0].v;_T15BA=(union Cyc_YYSTYPE*)_T15B9;_T1877->tl=Cyc_yyget_YY66(_T15BA);_T15B3=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY66(_T15B3);goto _LL0;case 560: _T15BB=yyyvsp;_T15BC=_T15BB[0];
# 3177 "parse.y"
yyval=_T15BC.v;goto _LL0;case 561: _T15BD=yyyvsp;_T15BE=& _T15BD[0].v;_T15BF=(union Cyc_YYSTYPE*)_T15BE;_T15C0=
# 3179 "parse.y"
Cyc_yyget_Exp_tok(_T15BF);_T15C1=yyyvsp;_T15C2=& _T15C1[2].v;_T15C3=(union Cyc_YYSTYPE*)_T15C2;_T15C4=Cyc_yyget_Exp_tok(_T15C3);_T15C5=yyyvsp;_T15C6=_T15C5[0];_T15C7=_T15C6.l;_T15C8=_T15C7.first_line;_T15C9=Cyc_Position_loc_to_seg(_T15C8);_T15CA=Cyc_Absyn_subscript_exp(_T15C0,_T15C4,_T15C9);yyval=Cyc_Exp_tok(_T15CA);goto _LL0;case 562: _T15CB=yyyvsp;_T15CC=& _T15CB[0].v;_T15CD=(union Cyc_YYSTYPE*)_T15CC;_T15CE=
# 3181 "parse.y"
Cyc_yyget_Exp_tok(_T15CD);_T15CF=yyyvsp;_T15D0=_T15CF[0];_T15D1=_T15D0.l;_T15D2=_T15D1.first_line;_T15D3=Cyc_Position_loc_to_seg(_T15D2);_T15D4=Cyc_Absyn_unknowncall_exp(_T15CE,0,_T15D3);yyval=Cyc_Exp_tok(_T15D4);goto _LL0;case 563: _T15D5=yyyvsp;_T15D6=& _T15D5[0].v;_T15D7=(union Cyc_YYSTYPE*)_T15D6;_T15D8=
# 3183 "parse.y"
Cyc_yyget_Exp_tok(_T15D7);_T15D9=yyyvsp;_T15DA=& _T15D9[2].v;_T15DB=(union Cyc_YYSTYPE*)_T15DA;_T15DC=Cyc_yyget_YY4(_T15DB);_T15DD=yyyvsp;_T15DE=_T15DD[0];_T15DF=_T15DE.l;_T15E0=_T15DF.first_line;_T15E1=Cyc_Position_loc_to_seg(_T15E0);_T15E2=Cyc_Absyn_unknowncall_exp(_T15D8,_T15DC,_T15E1);yyval=Cyc_Exp_tok(_T15E2);goto _LL0;case 564: _T15E3=yyyvsp;_T15E4=& _T15E3[0].v;_T15E5=(union Cyc_YYSTYPE*)_T15E4;_T15E6=
# 3185 "parse.y"
Cyc_yyget_Exp_tok(_T15E5);{struct _fat_ptr*_T1877=_cycalloc(sizeof(struct _fat_ptr));_T15E8=yyyvsp;_T15E9=& _T15E8[2].v;_T15EA=(union Cyc_YYSTYPE*)_T15E9;*_T1877=Cyc_yyget_String_tok(_T15EA);_T15E7=(struct _fat_ptr*)_T1877;}_T15EB=yyyvsp;_T15EC=_T15EB[0];_T15ED=_T15EC.l;_T15EE=_T15ED.first_line;_T15EF=Cyc_Position_loc_to_seg(_T15EE);_T15F0=Cyc_Absyn_aggrmember_exp(_T15E6,_T15E7,_T15EF);yyval=Cyc_Exp_tok(_T15F0);goto _LL0;case 565: _T15F1=yyyvsp;_T15F2=& _T15F1[0].v;_T15F3=(union Cyc_YYSTYPE*)_T15F2;_T15F4=
# 3187 "parse.y"
Cyc_yyget_Exp_tok(_T15F3);{struct _fat_ptr*_T1877=_cycalloc(sizeof(struct _fat_ptr));_T15F6=yyyvsp;_T15F7=& _T15F6[2].v;_T15F8=(union Cyc_YYSTYPE*)_T15F7;*_T1877=Cyc_yyget_String_tok(_T15F8);_T15F5=(struct _fat_ptr*)_T1877;}_T15F9=yyyvsp;_T15FA=_T15F9[0];_T15FB=_T15FA.l;_T15FC=_T15FB.first_line;_T15FD=Cyc_Position_loc_to_seg(_T15FC);_T15FE=Cyc_Absyn_aggrarrow_exp(_T15F4,_T15F5,_T15FD);yyval=Cyc_Exp_tok(_T15FE);goto _LL0;case 566: _T15FF=yyyvsp;_T1600=& _T15FF[0].v;_T1601=(union Cyc_YYSTYPE*)_T1600;_T1602=
# 3189 "parse.y"
Cyc_yyget_Exp_tok(_T1601);_T1603=yyyvsp;_T1604=_T1603[0];_T1605=_T1604.l;_T1606=_T1605.first_line;_T1607=Cyc_Position_loc_to_seg(_T1606);_T1608=Cyc_Absyn_increment_exp(_T1602,1U,_T1607);yyval=Cyc_Exp_tok(_T1608);goto _LL0;case 567: _T1609=yyyvsp;_T160A=& _T1609[0].v;_T160B=(union Cyc_YYSTYPE*)_T160A;_T160C=
# 3191 "parse.y"
Cyc_yyget_Exp_tok(_T160B);_T160D=yyyvsp;_T160E=_T160D[0];_T160F=_T160E.l;_T1610=_T160F.first_line;_T1611=Cyc_Position_loc_to_seg(_T1610);_T1612=Cyc_Absyn_increment_exp(_T160C,3U,_T1611);yyval=Cyc_Exp_tok(_T1612);goto _LL0;case 568:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1877->tag=24;_T1614=yyyvsp;_T1615=& _T1614[1].v;_T1616=(union Cyc_YYSTYPE*)_T1615;
# 3193 "parse.y"
_T1877->f1=Cyc_yyget_YY38(_T1616);_T1877->f2=0;_T1613=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1877;}_T1617=(void*)_T1613;_T1618=yyyvsp;_T1619=_T1618[0];_T161A=_T1619.l;_T161B=_T161A.first_line;_T161C=Cyc_Position_loc_to_seg(_T161B);_T161D=Cyc_Absyn_new_exp(_T1617,_T161C);yyval=Cyc_Exp_tok(_T161D);goto _LL0;case 569:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1877->tag=24;_T161F=yyyvsp;_T1620=& _T161F[1].v;_T1621=(union Cyc_YYSTYPE*)_T1620;
# 3195 "parse.y"
_T1877->f1=Cyc_yyget_YY38(_T1621);_T1622=yyyvsp;_T1623=& _T1622[4].v;_T1624=(union Cyc_YYSTYPE*)_T1623;_T1625=Cyc_yyget_YY5(_T1624);_T1877->f2=Cyc_List_imp_rev(_T1625);_T161E=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1877;}_T1626=(void*)_T161E;_T1627=yyyvsp;_T1628=_T1627[0];_T1629=_T1628.l;_T162A=_T1629.first_line;_T162B=Cyc_Position_loc_to_seg(_T162A);_T162C=Cyc_Absyn_new_exp(_T1626,_T162B);yyval=Cyc_Exp_tok(_T162C);goto _LL0;case 570:{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct));_T1877->tag=24;_T162E=yyyvsp;_T162F=& _T162E[1].v;_T1630=(union Cyc_YYSTYPE*)_T162F;
# 3197 "parse.y"
_T1877->f1=Cyc_yyget_YY38(_T1630);_T1631=yyyvsp;_T1632=& _T1631[4].v;_T1633=(union Cyc_YYSTYPE*)_T1632;_T1634=Cyc_yyget_YY5(_T1633);_T1877->f2=Cyc_List_imp_rev(_T1634);_T162D=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_T1877;}_T1635=(void*)_T162D;_T1636=yyyvsp;_T1637=_T1636[0];_T1638=_T1637.l;_T1639=_T1638.first_line;_T163A=Cyc_Position_loc_to_seg(_T1639);_T163B=Cyc_Absyn_new_exp(_T1635,_T163A);yyval=Cyc_Exp_tok(_T163B);goto _LL0;case 571:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1878->tag=0;{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));_T163F=yyyvsp;_T1640=& _T163F[0].v;_T1641=(union Cyc_YYSTYPE*)_T1640;
# 3202 "parse.y"
*_T1879=Cyc_yyget_String_tok(_T1641);_T163E=(struct _fat_ptr*)_T1879;}_T1878->f1=_T163E;_T163D=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1878;}_T1877->hd=(void*)_T163D;_T1877->tl=0;_T163C=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY3(_T163C);goto _LL0;case 572: _T1642=yyyvsp;_T1643=_T1642[0];_T1644=_T1643.l;_T1645=_T1644.first_line;_T1646=
# 3205
Cyc_Position_loc_to_seg(_T1645);_T1647=yyyvsp;_T1648=& _T1647[0].v;_T1649=(union Cyc_YYSTYPE*)_T1648;_T164A=Cyc_yyget_Int_tok(_T1649);{unsigned i=Cyc_Parse_cnst2uint(_T1646,_T164A);_T164B=i;_T164C=(int)_T164B;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1877=Cyc_Absyn_tuple_field_designator(_T164C);struct _fat_ptr*_T1878;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1879=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1877;_T1878=_T1879->f1;}{struct _fat_ptr*f=_T1878;{struct Cyc_List_List*_T1879=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T187A->tag=0;
_T187A->f1=f;_T164E=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T187A;}_T1879->hd=(void*)_T164E;_T1879->tl=0;_T164D=(struct Cyc_List_List*)_T1879;}yyval=Cyc_YY3(_T164D);goto _LL0;}}}case 573:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T1878=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T1878->tag=0;{struct _fat_ptr*_T1879=_cycalloc(sizeof(struct _fat_ptr));_T1652=yyyvsp;_T1653=& _T1652[2].v;_T1654=(union Cyc_YYSTYPE*)_T1653;
# 3209 "parse.y"
*_T1879=Cyc_yyget_String_tok(_T1654);_T1651=(struct _fat_ptr*)_T1879;}_T1878->f1=_T1651;_T1650=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T1878;}_T1877->hd=(void*)_T1650;_T1655=yyyvsp;_T1656=& _T1655[0].v;_T1657=(union Cyc_YYSTYPE*)_T1656;_T1877->tl=Cyc_yyget_YY3(_T1657);_T164F=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY3(_T164F);goto _LL0;case 574: _T1658=yyyvsp;_T1659=_T1658[2];_T165A=_T1659.l;_T165B=_T165A.first_line;_T165C=
# 3212
Cyc_Position_loc_to_seg(_T165B);_T165D=yyyvsp;_T165E=& _T165D[2].v;_T165F=(union Cyc_YYSTYPE*)_T165E;_T1660=Cyc_yyget_Int_tok(_T165F);{unsigned i=Cyc_Parse_cnst2uint(_T165C,_T1660);_T1661=i;_T1662=(int)_T1661;{
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1877=Cyc_Absyn_tuple_field_designator(_T1662);struct _fat_ptr*_T1878;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1879=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T1877;_T1878=_T1879->f1;}{struct _fat_ptr*f=_T1878;{struct Cyc_List_List*_T1879=_cycalloc(sizeof(struct Cyc_List_List));{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T187A=_cycalloc(sizeof(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct));_T187A->tag=0;
_T187A->f1=f;_T1664=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T187A;}_T1879->hd=(void*)_T1664;_T1665=yyyvsp;_T1666=& _T1665[0].v;_T1667=(union Cyc_YYSTYPE*)_T1666;_T1879->tl=Cyc_yyget_YY3(_T1667);_T1663=(struct Cyc_List_List*)_T1879;}yyval=Cyc_YY3(_T1663);goto _LL0;}}}case 575: _T1668=yyyvsp;_T1669=& _T1668[0].v;_T166A=(union Cyc_YYSTYPE*)_T1669;_T166B=
# 3219 "parse.y"
Cyc_yyget_QualId_tok(_T166A);_T166C=yyyvsp;_T166D=_T166C[0];_T166E=_T166D.l;_T166F=_T166E.first_line;_T1670=Cyc_Position_loc_to_seg(_T166F);_T1671=Cyc_Absyn_unknownid_exp(_T166B,_T1670);yyval=Cyc_Exp_tok(_T1671);goto _LL0;case 576: _T1672=yyyvsp;_T1673=& _T1672[2].v;_T1674=(union Cyc_YYSTYPE*)_T1673;_T1675=
# 3220 "parse.y"
Cyc_yyget_String_tok(_T1674);_T1676=yyyvsp;_T1677=_T1676[0];_T1678=_T1677.l;_T1679=_T1678.first_line;_T167A=Cyc_Position_loc_to_seg(_T1679);_T167B=Cyc_Absyn_pragma_exp(_T1675,_T167A);yyval=Cyc_Exp_tok(_T167B);goto _LL0;case 577: _T167C=yyyvsp;_T167D=_T167C[0];
# 3221 "parse.y"
yyval=_T167D.v;goto _LL0;case 578: _T167E=yyyvsp;_T167F=& _T167E[0].v;_T1680=(union Cyc_YYSTYPE*)_T167F;_T1681=
# 3222 "parse.y"
Cyc_yyget_String_tok(_T1680);_T1682=yyyvsp;_T1683=_T1682[0];_T1684=_T1683.l;_T1685=_T1684.first_line;_T1686=Cyc_Position_loc_to_seg(_T1685);_T1687=Cyc_Absyn_string_exp(_T1681,_T1686);yyval=Cyc_Exp_tok(_T1687);goto _LL0;case 579: _T1688=yyyvsp;_T1689=& _T1688[0].v;_T168A=(union Cyc_YYSTYPE*)_T1689;_T168B=
# 3223 "parse.y"
Cyc_yyget_String_tok(_T168A);_T168C=yyyvsp;_T168D=_T168C[0];_T168E=_T168D.l;_T168F=_T168E.first_line;_T1690=Cyc_Position_loc_to_seg(_T168F);_T1691=Cyc_Absyn_wstring_exp(_T168B,_T1690);yyval=Cyc_Exp_tok(_T1691);goto _LL0;case 580: _T1692=yyyvsp;_T1693=_T1692[1];
# 3224 "parse.y"
yyval=_T1693.v;goto _LL0;case 581: _T1694=yyyvsp;_T1695=& _T1694[0].v;_T1696=(union Cyc_YYSTYPE*)_T1695;_T1697=
# 3228 "parse.y"
Cyc_yyget_Exp_tok(_T1696);_T1698=yyyvsp;_T1699=_T1698[0];_T169A=_T1699.l;_T169B=_T169A.first_line;_T169C=Cyc_Position_loc_to_seg(_T169B);_T169D=Cyc_Absyn_noinstantiate_exp(_T1697,_T169C);yyval=Cyc_Exp_tok(_T169D);goto _LL0;case 582: _T169E=yyyvsp;_T169F=& _T169E[0].v;_T16A0=(union Cyc_YYSTYPE*)_T169F;_T16A1=
# 3231
Cyc_yyget_Exp_tok(_T16A0);_T16A2=yyyvsp;_T16A3=& _T16A2[3].v;_T16A4=(union Cyc_YYSTYPE*)_T16A3;_T16A5=Cyc_yyget_YY41(_T16A4);_T16A6=Cyc_List_imp_rev(_T16A5);_T16A7=yyyvsp;_T16A8=_T16A7[0];_T16A9=_T16A8.l;_T16AA=_T16A9.first_line;_T16AB=Cyc_Position_loc_to_seg(_T16AA);_T16AC=Cyc_Absyn_instantiate_exp(_T16A1,_T16A6,_T16AB);yyval=Cyc_Exp_tok(_T16AC);goto _LL0;case 583:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T1877=_cycalloc(sizeof(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct));_T1877->tag=28;_T16AE=yyyvsp;_T16AF=& _T16AE[0].v;_T16B0=(union Cyc_YYSTYPE*)_T16AF;
# 3234
_T1877->f1=Cyc_yyget_QualId_tok(_T16B0);_T16B1=yyyvsp;_T16B2=& _T16B1[2].v;_T16B3=(union Cyc_YYSTYPE*)_T16B2;_T1877->f2=Cyc_yyget_YY41(_T16B3);_T16B4=yyyvsp;_T16B5=& _T16B4[3].v;_T16B6=(union Cyc_YYSTYPE*)_T16B5;_T16B7=Cyc_yyget_YY5(_T16B6);_T1877->f3=Cyc_List_imp_rev(_T16B7);_T1877->f4=0;_T16AD=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T1877;}_T16B8=(void*)_T16AD;_T16B9=yyyvsp;_T16BA=_T16B9[0];_T16BB=_T16BA.l;_T16BC=_T16BB.first_line;_T16BD=Cyc_Position_loc_to_seg(_T16BC);_T16BE=Cyc_Absyn_new_exp(_T16B8,_T16BD);yyval=Cyc_Exp_tok(_T16BE);goto _LL0;case 584: _T16BF=yyyvsp;_T16C0=& _T16BF[2].v;_T16C1=(union Cyc_YYSTYPE*)_T16C0;_T16C2=
# 3236 "parse.y"
Cyc_yyget_YY4(_T16C1);_T16C3=yyyvsp;_T16C4=_T16C3[0];_T16C5=_T16C4.l;_T16C6=_T16C5.first_line;_T16C7=Cyc_Position_loc_to_seg(_T16C6);_T16C8=Cyc_Absyn_tuple_exp(_T16C2,_T16C7);yyval=Cyc_Exp_tok(_T16C8);goto _LL0;case 585: _T16C9=yyyvsp;_T16CA=& _T16C9[2].v;_T16CB=(union Cyc_YYSTYPE*)_T16CA;_T16CC=
# 3238 "parse.y"
Cyc_yyget_Stmt_tok(_T16CB);_T16CD=yyyvsp;_T16CE=_T16CD[0];_T16CF=_T16CE.l;_T16D0=_T16CF.first_line;_T16D1=Cyc_Position_loc_to_seg(_T16D0);_T16D2=Cyc_Absyn_stmt_exp(_T16CC,_T16D1);yyval=Cyc_Exp_tok(_T16D2);goto _LL0;case 586: _T16D3=yyyvsp;_T16D4=& _T16D3[0].v;_T16D5=(union Cyc_YYSTYPE*)_T16D4;_T16D6=
# 3242 "parse.y"
Cyc_yyget_YY4(_T16D5);_T16D7=Cyc_List_imp_rev(_T16D6);yyval=Cyc_YY4(_T16D7);goto _LL0;case 587:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T16D9=yyyvsp;_T16DA=& _T16D9[0].v;_T16DB=(union Cyc_YYSTYPE*)_T16DA;
# 3247 "parse.y"
_T1877->hd=Cyc_yyget_Exp_tok(_T16DB);_T1877->tl=0;_T16D8=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY4(_T16D8);goto _LL0;case 588:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T16DD=yyyvsp;_T16DE=& _T16DD[2].v;_T16DF=(union Cyc_YYSTYPE*)_T16DE;
# 3249 "parse.y"
_T1877->hd=Cyc_yyget_Exp_tok(_T16DF);_T16E0=yyyvsp;_T16E1=& _T16E0[0].v;_T16E2=(union Cyc_YYSTYPE*)_T16E1;_T1877->tl=Cyc_yyget_YY4(_T16E2);_T16DC=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY4(_T16DC);goto _LL0;case 589: _T16E3=yyyvsp;_T16E4=& _T16E3[0].v;_T16E5=(union Cyc_YYSTYPE*)_T16E4;_T16E6=
# 3255 "parse.y"
Cyc_yyget_Int_tok(_T16E5);_T16E7=yyyvsp;_T16E8=_T16E7[0];_T16E9=_T16E8.l;_T16EA=_T16E9.first_line;_T16EB=Cyc_Position_loc_to_seg(_T16EA);_T16EC=Cyc_Absyn_const_exp(_T16E6,_T16EB);yyval=Cyc_Exp_tok(_T16EC);goto _LL0;case 590: _T16ED=yyyvsp;_T16EE=& _T16ED[0].v;_T16EF=(union Cyc_YYSTYPE*)_T16EE;_T16F0=
# 3256 "parse.y"
Cyc_yyget_Char_tok(_T16EF);_T16F1=yyyvsp;_T16F2=_T16F1[0];_T16F3=_T16F2.l;_T16F4=_T16F3.first_line;_T16F5=Cyc_Position_loc_to_seg(_T16F4);_T16F6=Cyc_Absyn_char_exp(_T16F0,_T16F5);yyval=Cyc_Exp_tok(_T16F6);goto _LL0;case 591: _T16F7=yyyvsp;_T16F8=& _T16F7[0].v;_T16F9=(union Cyc_YYSTYPE*)_T16F8;_T16FA=
# 3257 "parse.y"
Cyc_yyget_String_tok(_T16F9);_T16FB=yyyvsp;_T16FC=_T16FB[0];_T16FD=_T16FC.l;_T16FE=_T16FD.first_line;_T16FF=Cyc_Position_loc_to_seg(_T16FE);_T1700=Cyc_Absyn_wchar_exp(_T16FA,_T16FF);yyval=Cyc_Exp_tok(_T1700);goto _LL0;case 592: _T1701=yyyvsp;_T1702=_T1701[0];_T1703=_T1702.l;_T1704=_T1703.first_line;_T1705=
# 3259 "parse.y"
Cyc_Position_loc_to_seg(_T1704);_T1706=Cyc_Absyn_null_exp(_T1705);yyval=Cyc_Exp_tok(_T1706);goto _LL0;case 593: _T1707=yyyvsp;_T1708=& _T1707[0].v;_T1709=(union Cyc_YYSTYPE*)_T1708;{
# 3261 "parse.y"
struct _fat_ptr f=Cyc_yyget_String_tok(_T1709);_T170A=
Cyc_strlen(f);{int l=(int)_T170A;
int i=1;
if(l <= 0)goto _TL2EB;_T170B=f;_T170C=_T170B.curr;_T170D=(const char*)_T170C;_T170E=l - 1;{
char c=_T170D[_T170E];_T170F=c;_T1710=(int)_T170F;
if(_T1710==102)goto _TL2EF;else{goto _TL2F0;}_TL2F0: _T1711=c;_T1712=(int)_T1711;if(_T1712==70)goto _TL2EF;else{goto _TL2ED;}_TL2EF: i=0;goto _TL2EE;
_TL2ED: _T1713=c;_T1714=(int)_T1713;if(_T1714==108)goto _TL2F3;else{goto _TL2F4;}_TL2F4: _T1715=c;_T1716=(int)_T1715;if(_T1716==76)goto _TL2F3;else{goto _TL2F1;}_TL2F3: i=2;goto _TL2F2;_TL2F1: _TL2F2: _TL2EE:;}goto _TL2EC;_TL2EB: _TL2EC: _T1717=f;_T1718=i;_T1719=yyyvsp;_T171A=_T1719[0];_T171B=_T171A.l;_T171C=_T171B.first_line;_T171D=
# 3269
Cyc_Position_loc_to_seg(_T171C);_T171E=Cyc_Absyn_float_exp(_T1717,_T1718,_T171D);yyval=Cyc_Exp_tok(_T171E);goto _LL0;}}case 594:{struct _tuple0*_T1877=_cycalloc(sizeof(struct _tuple0));
# 3274 "parse.y"
_T1877->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T1721=yyyvsp;_T1722=& _T1721[0].v;_T1723=(union Cyc_YYSTYPE*)_T1722;*_T1878=Cyc_yyget_String_tok(_T1723);_T1720=(struct _fat_ptr*)_T1878;}_T1877->f1=_T1720;_T171F=(struct _tuple0*)_T1877;}yyval=Cyc_QualId_tok(_T171F);goto _LL0;case 595: _T1724=yyyvsp;_T1725=_T1724[0];
# 3275 "parse.y"
yyval=_T1725.v;goto _LL0;case 596:{struct _tuple0*_T1877=_cycalloc(sizeof(struct _tuple0));
# 3278
_T1877->f0=Cyc_Absyn_Rel_n(0);{struct _fat_ptr*_T1878=_cycalloc(sizeof(struct _fat_ptr));_T1728=yyyvsp;_T1729=& _T1728[0].v;_T172A=(union Cyc_YYSTYPE*)_T1729;*_T1878=Cyc_yyget_String_tok(_T172A);_T1727=(struct _fat_ptr*)_T1878;}_T1877->f1=_T1727;_T1726=(struct _tuple0*)_T1877;}yyval=Cyc_QualId_tok(_T1726);goto _LL0;case 597: _T172B=yyyvsp;_T172C=_T172B[0];
# 3279 "parse.y"
yyval=_T172C.v;goto _LL0;case 598: _T172D=yyyvsp;_T172E=_T172D[0];
# 3284 "parse.y"
yyval=_T172E.v;goto _LL0;case 599: _T172F=yyyvsp;_T1730=_T172F[0];
# 3285 "parse.y"
yyval=_T1730.v;goto _LL0;case 600: _T1731=yyyvsp;_T1732=_T1731[0];
# 3288
yyval=_T1732.v;goto _LL0;case 601: _T1733=yyyvsp;_T1734=_T1733[0];
# 3289 "parse.y"
yyval=_T1734.v;goto _LL0;case 602: goto _LL0;case 603: _T1735=yylex_buf;
# 3294 "parse.y"
_T1735->lex_curr_pos=_T1735->lex_curr_pos - 1;goto _LL0;case 604:
# 3298 "parse.y"
 yyval=Cyc_YY71(0);goto _LL0;case 605:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple35*_T1878=_cycalloc(sizeof(struct _tuple35));_T1738=yyyvsp;_T1739=& _T1738[1].v;_T173A=(union Cyc_YYSTYPE*)_T1739;_T173B=
# 3300 "parse.y"
Cyc_yyget_String_tok(_T173A);_T173C=yyyvsp;_T173D=& _T173C[3].v;_T173E=(union Cyc_YYSTYPE*)_T173D;_T173F=Cyc_yyget_String_tok(_T173E);_T1740=_tag_fat("true",sizeof(char),5U);_T1741=Cyc_strcmp(_T173F,_T1740);_T1742=_T1741==0;_T1743=yyyvsp;_T1744=& _T1743[0].v;_T1745=(union Cyc_YYSTYPE*)_T1744;_T1746=Cyc_yyget_String_tok(_T1745);_T1747=Cyc_Parse_typevar2cvar(_T1746);_T1878->f0=Cyc_Parse_assign_cvar_pos(_T173B,_T1742,_T1747);_T1748=yyyvsp;_T1749=& _T1748[5].v;_T174A=(union Cyc_YYSTYPE*)_T1749;_T1878->f1=Cyc_yyget_YY72(_T174A);_T1737=(struct _tuple35*)_T1878;}_T1877->hd=_T1737;_T174B=yyyvsp;_T174C=& _T174B[7].v;_T174D=(union Cyc_YYSTYPE*)_T174C;_T1877->tl=Cyc_yyget_YY71(_T174D);_T1736=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY71(_T1736);goto _LL0;case 606:
# 3304 "parse.y"
 yyval=Cyc_YY72(0);goto _LL0;case 607: _T174E=yyyvsp;_T174F=& _T174E[0].v;_T1750=(union Cyc_YYSTYPE*)_T174F;_T1751=
# 3305 "parse.y"
Cyc_yyget_YY72(_T1750);yyval=Cyc_YY72(_T1751);goto _LL0;case 608:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1753=yyyvsp;_T1754=& _T1753[0].v;_T1755=(union Cyc_YYSTYPE*)_T1754;
# 3309 "parse.y"
_T1877->hd=Cyc_yyget_YY73(_T1755);_T1877->tl=0;_T1752=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY72(_T1752);goto _LL0;case 609:{struct Cyc_List_List*_T1877=_cycalloc(sizeof(struct Cyc_List_List));_T1757=yyyvsp;_T1758=& _T1757[0].v;_T1759=(union Cyc_YYSTYPE*)_T1758;
# 3310 "parse.y"
_T1877->hd=Cyc_yyget_YY73(_T1759);_T175A=yyyvsp;_T175B=& _T175A[2].v;_T175C=(union Cyc_YYSTYPE*)_T175B;_T1877->tl=Cyc_yyget_YY72(_T175C);_T1756=(struct Cyc_List_List*)_T1877;}yyval=Cyc_YY72(_T1756);goto _LL0;case 610: _T175D=yyyvsp;_T175E=& _T175D[0].v;_T175F=(union Cyc_YYSTYPE*)_T175E;_T1760=
# 3314 "parse.y"
Cyc_yyget_String_tok(_T175F);_T1761=Cyc_Parse_typevar2cvar(_T1760);yyval=Cyc_YY45(_T1761);goto _LL0;case 611: _T1762=yyyvsp;_T1763=_T1762[0];_T1764=_T1763.l;_T1765=_T1764.first_line;_T1766=
# 3315 "parse.y"
Cyc_Position_loc_to_seg(_T1765);_T1767=yyyvsp;_T1768=& _T1767[0].v;_T1769=(union Cyc_YYSTYPE*)_T1768;_T176A=Cyc_yyget_String_tok(_T1769);_T176B=Cyc_Parse_str2type(_T1766,_T176A);yyval=Cyc_YY45(_T176B);goto _LL0;case 612: _T176C=yyyvsp;_T176D=& _T176C[0].v;_T176E=(union Cyc_YYSTYPE*)_T176D;_T176F=
# 3319 "parse.y"
Cyc_yyget_String_tok(_T176E);_T1770=yyyvsp;_T1771=& _T1770[3].v;_T1772=(union Cyc_YYSTYPE*)_T1771;_T1773=Cyc_yyget_YY73(_T1772);_T1774=Cyc_BansheeIf_check_constraint(_T1773);_T1775=Cyc_BansheeIf_add_location(_T176F,_T1774);yyval=Cyc_YY73(_T1775);goto _LL0;case 613: _T1776=yyyvsp;_T1777=& _T1776[0].v;_T1778=(union Cyc_YYSTYPE*)_T1777;_T1779=
# 3320 "parse.y"
Cyc_yyget_String_tok(_T1778);_T177A=yyyvsp;_T177B=& _T177A[3].v;_T177C=(union Cyc_YYSTYPE*)_T177B;_T177D=Cyc_yyget_YY70(_T177C);_T177E=yyyvsp;_T177F=& _T177E[5].v;_T1780=(union Cyc_YYSTYPE*)_T177F;_T1781=Cyc_yyget_YY45(_T1780);_T1782=yyyvsp;_T1783=& _T1782[7].v;_T1784=(union Cyc_YYSTYPE*)_T1783;_T1785=Cyc_yyget_YY45(_T1784);_T1786=Cyc_Parse_comparison_constraint(_T177D,_T1781,_T1785);_T1787=Cyc_BansheeIf_add_location(_T1779,_T1786);yyval=Cyc_YY73(_T1787);goto _LL0;case 614: _T1788=yyyvsp;_T1789=& _T1788[0].v;_T178A=(union Cyc_YYSTYPE*)_T1789;_T178B=
# 3321 "parse.y"
Cyc_yyget_String_tok(_T178A);_T178C=yyyvsp;_T178D=& _T178C[3].v;_T178E=(union Cyc_YYSTYPE*)_T178D;_T178F=Cyc_yyget_YY45(_T178E);_T1790=yyyvsp;_T1791=& _T1790[5].v;_T1792=(union Cyc_YYSTYPE*)_T1791;_T1793=Cyc_yyget_YY45(_T1792);_T1794=Cyc_BansheeIf_cond_equality_constraint(_T178F,_T1793);_T1795=Cyc_BansheeIf_add_location(_T178B,_T1794);yyval=Cyc_YY73(_T1795);goto _LL0;case 615: _T1796=yyyvsp;_T1797=& _T1796[0].v;_T1798=(union Cyc_YYSTYPE*)_T1797;_T1799=
# 3322 "parse.y"
Cyc_yyget_String_tok(_T1798);_T179A=yyyvsp;_T179B=& _T179A[3].v;_T179C=(union Cyc_YYSTYPE*)_T179B;_T179D=Cyc_yyget_YY45(_T179C);_T179E=yyyvsp;_T179F=& _T179E[5].v;_T17A0=(union Cyc_YYSTYPE*)_T179F;_T17A1=Cyc_yyget_YY45(_T17A0);_T17A2=Cyc_BansheeIf_equality_constraint(_T179D,_T17A1);_T17A3=Cyc_BansheeIf_add_location(_T1799,_T17A2);yyval=Cyc_YY73(_T17A3);goto _LL0;case 616: _T17A4=yyyvsp;_T17A5=& _T17A4[0].v;_T17A6=(union Cyc_YYSTYPE*)_T17A5;_T17A7=
# 3323 "parse.y"
Cyc_yyget_String_tok(_T17A6);_T17A8=yyyvsp;_T17A9=& _T17A8[3].v;_T17AA=(union Cyc_YYSTYPE*)_T17A9;_T17AB=Cyc_yyget_YY45(_T17AA);_T17AC=yyyvsp;_T17AD=& _T17AC[5].v;_T17AE=(union Cyc_YYSTYPE*)_T17AD;_T17AF=Cyc_yyget_YY45(_T17AE);_T17B0=Cyc_BansheeIf_inclusion_constraint(_T17AB,_T17AF);_T17B1=Cyc_BansheeIf_add_location(_T17A7,_T17B0);yyval=Cyc_YY73(_T17B1);goto _LL0;case 617: _T17B2=yyyvsp;_T17B3=& _T17B2[0].v;_T17B4=(union Cyc_YYSTYPE*)_T17B3;_T17B5=
# 3324 "parse.y"
Cyc_yyget_String_tok(_T17B4);_T17B6=yyyvsp;_T17B7=& _T17B6[3].v;_T17B8=(union Cyc_YYSTYPE*)_T17B7;_T17B9=Cyc_yyget_YY73(_T17B8);_T17BA=yyyvsp;_T17BB=& _T17BA[5].v;_T17BC=(union Cyc_YYSTYPE*)_T17BB;_T17BD=Cyc_yyget_YY73(_T17BC);_T17BE=Cyc_BansheeIf_implication_constraint(_T17B9,_T17BD);_T17BF=Cyc_BansheeIf_add_location(_T17B5,_T17BE);yyval=Cyc_YY73(_T17BF);goto _LL0;case 618: _T17C0=yyyvsp;_T17C1=& _T17C0[0].v;_T17C2=(union Cyc_YYSTYPE*)_T17C1;_T17C3=
# 3325 "parse.y"
Cyc_yyget_String_tok(_T17C2);_T17C4=yyyvsp;_T17C5=& _T17C4[3].v;_T17C6=(union Cyc_YYSTYPE*)_T17C5;_T17C7=Cyc_yyget_YY70(_T17C6);_T17C8=yyyvsp;_T17C9=& _T17C8[5].v;_T17CA=(union Cyc_YYSTYPE*)_T17C9;_T17CB=Cyc_yyget_YY73(_T17CA);_T17CC=yyyvsp;_T17CD=& _T17CC[7].v;_T17CE=(union Cyc_YYSTYPE*)_T17CD;_T17CF=Cyc_yyget_YY73(_T17CE);_T17D0=Cyc_Parse_composite_constraint(_T17C7,_T17CB,_T17CF);_T17D1=Cyc_BansheeIf_add_location(_T17C3,_T17D0);yyval=Cyc_YY73(_T17D1);goto _LL0;case 619:
# 3328
 yyval=Cyc_YY70(0U);goto _LL0;case 620:
# 3329 "parse.y"
 yyval=Cyc_YY70(1U);goto _LL0;case 621:
# 3330 "parse.y"
 yyval=Cyc_YY70(2U);goto _LL0;case 622:
# 3331 "parse.y"
 yyval=Cyc_YY70(3U);goto _LL0;case 623:
# 3332 "parse.y"
 yyval=Cyc_YY70(4U);goto _LL0;default: goto _LL0;}_LL0: _T17D2=yylen;
# 375 "cycbison.simple"
yyvsp_offset=yyvsp_offset - _T17D2;_T17D3=yylen;
yyssp_offset=yyssp_offset - _T17D3;_T17D4=yyvs;
# 389 "cycbison.simple"
yyvsp_offset=yyvsp_offset + 1;_T17D5=yyvsp_offset;_T17D6=_check_fat_subscript(_T17D4,sizeof(struct Cyc_Yystacktype),_T17D5);_T17D7=(struct Cyc_Yystacktype*)_T17D6;(*_T17D7).v=yyval;
# 392
if(yylen!=0)goto _TL2F5;_T17D8=yyvs;_T17D9=yyvsp_offset - 1;_T17DA=
_fat_ptr_plus(_T17D8,sizeof(struct Cyc_Yystacktype),_T17D9);_T17DB=_untag_fat_ptr_check_bound(_T17DA,sizeof(struct Cyc_Yystacktype),2U);_T17DC=_check_null(_T17DB);{struct Cyc_Yystacktype*p=(struct Cyc_Yystacktype*)_T17DC;_T17DD=p;_T17DE=yylloc;
_T17DD[1].l.first_line=_T17DE.first_line;_T17DF=p;_T17E0=yylloc;
_T17DF[1].l.first_column=_T17E0.first_column;_T17E1=p;_T17E2=p;_T17E3=_T17E2[0];_T17E4=_T17E3.l;
_T17E1[1].l.last_line=_T17E4.last_line;_T17E5=p;_T17E6=p;_T17E7=_T17E6[0];_T17E8=_T17E7.l;
_T17E5[1].l.last_column=_T17E8.last_column;}goto _TL2F6;
# 399
_TL2F5: _T17E9=yyvs;_T17EA=_T17E9.curr;_T17EB=(struct Cyc_Yystacktype*)_T17EA;_T17EC=yyvsp_offset;_T17ED=yyvs;_T17EE=yyvsp_offset + yylen;_T17EF=_T17EE - 1;_T17F0=_check_fat_subscript(_T17ED,sizeof(struct Cyc_Yystacktype),_T17EF);_T17F1=(struct Cyc_Yystacktype*)_T17F0;_T17F2=*_T17F1;_T17F3=_T17F2.l;_T17EB[_T17EC].l.last_line=_T17F3.last_line;_T17F4=yyvs;_T17F5=_T17F4.curr;_T17F6=(struct Cyc_Yystacktype*)_T17F5;_T17F7=yyvsp_offset;_T17F8=yyvs;_T17F9=_T17F8.curr;_T17FA=(struct Cyc_Yystacktype*)_T17F9;_T17FB=yyvsp_offset + yylen;_T17FC=_T17FB - 1;_T17FD=_T17FA[_T17FC];_T17FE=_T17FD.l;
_T17F6[_T17F7].l.last_column=_T17FE.last_column;_TL2F6: _T17FF=Cyc_yyr1;_T1800=yyn;_T1801=_check_known_subscript_notnull(_T17FF,624U,sizeof(short),_T1800);_T1802=(short*)_T1801;_T1803=*_T1802;
# 409
yyn=(int)_T1803;_T1804=Cyc_yypgoto;_T1805=yyn - 175;_T1806=_check_known_subscript_notnull(_T1804,188U,sizeof(short),_T1805);_T1807=(short*)_T1806;_T1808=*_T1807;_T1809=(int)_T1808;_T180A=yyss;_T180B=yyssp_offset;_T180C=_check_fat_subscript(_T180A,sizeof(short),_T180B);_T180D=(short*)_T180C;_T180E=*_T180D;_T180F=(int)_T180E;
# 411
yystate=_T1809 + _T180F;
if(yystate < 0)goto _TL2F7;if(yystate > 8241)goto _TL2F7;_T1810=Cyc_yycheck;_T1811=yystate;_T1812=_T1810[_T1811];_T1813=(int)_T1812;_T1814=yyss;_T1815=_T1814.curr;_T1816=(short*)_T1815;_T1817=yyssp_offset;_T1818=_T1816[_T1817];_T1819=(int)_T1818;if(_T1813!=_T1819)goto _TL2F7;_T181A=Cyc_yytable;_T181B=yystate;_T181C=_T181A[_T181B];
yystate=(int)_T181C;goto _TL2F8;
# 415
_TL2F7: _T181D=Cyc_yydefgoto;_T181E=yyn - 175;_T181F=_T181D[_T181E];yystate=(int)_T181F;_TL2F8: goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus!=0)goto _TL2F9;
# 424
yynerrs=yynerrs + 1;_T1820=Cyc_yypact;_T1821=yystate;_T1822=_check_known_subscript_notnull(_T1820,1262U,sizeof(short),_T1821);_T1823=(short*)_T1822;_T1824=*_T1823;
# 427
yyn=(int)_T1824;_T1825=yyn;_T1826=- 32768;
# 429
if(_T1825 <= _T1826)goto _TL2FB;if(yyn >= 8241)goto _TL2FB;{
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
if(yyn >= 0)goto _TL2FD;_T1827=- yyn;goto _TL2FE;_TL2FD: _T1827=0;_TL2FE: x=_T1827;
_TL302: _T1828=x;_T1829=(unsigned)_T1828;_T182A=363U / sizeof(char*);
# 437
if(_T1829 < _T182A)goto _TL300;else{goto _TL301;}
# 439
_TL300: _T182B=Cyc_yycheck;_T182C=x + yyn;_T182D=_check_known_subscript_notnull(_T182B,8242U,sizeof(short),_T182C);_T182E=(short*)_T182D;_T182F=*_T182E;_T1830=(int)_T182F;_T1831=x;if(_T1830!=_T1831)goto _TL303;_T1832=Cyc_yytname;_T1833=x;_T1834=_check_known_subscript_notnull(_T1832,363U,sizeof(struct _fat_ptr),_T1833);_T1835=(struct _fat_ptr*)_T1834;_T1836=*_T1835;_T1837=
Cyc_strlen(_T1836);_T1838=_T1837 + 15U;sze=sze + _T1838;count=count + 1;goto _TL304;_TL303: _TL304:
# 438
 x=x + 1;goto _TL302;_TL301: _T183A=sze + 15;_T183B=(unsigned)_T183A;{unsigned _T1877=_T183B + 1U;_T183D=yyregion;_T183E=_check_times(_T1877,sizeof(char));{char*_T1878=_region_malloc(_T183D,0U,_T183E);{unsigned _T1879=_T1877;unsigned i;i=0;_TL308: if(i < _T1879)goto _TL306;else{goto _TL307;}_TL306: _T183F=i;
# 441
_T1878[_T183F]='\000';i=i + 1;goto _TL308;_TL307: _T1878[_T1879]=0;}_T183C=(char*)_T1878;}_T1839=_tag_fat(_T183C,sizeof(char),_T1877);}msg=_T1839;_T1840=msg;_T1841=
_tag_fat("parse error",sizeof(char),12U);Cyc_strcpy(_T1840,_T1841);
# 444
if(count >= 5)goto _TL309;
# 446
count=0;
if(yyn >= 0)goto _TL30B;_T1842=- yyn;goto _TL30C;_TL30B: _T1842=0;_TL30C: x=_T1842;
_TL310: _T1843=x;_T1844=(unsigned)_T1843;_T1845=363U / sizeof(char*);
# 447
if(_T1844 < _T1845)goto _TL30E;else{goto _TL30F;}
# 449
_TL30E: _T1846=Cyc_yycheck;_T1847=x + yyn;_T1848=_check_known_subscript_notnull(_T1846,8242U,sizeof(short),_T1847);_T1849=(short*)_T1848;_T184A=*_T1849;_T184B=(int)_T184A;_T184C=x;if(_T184B!=_T184C)goto _TL311;_T184D=msg;
# 451
if(count!=0)goto _TL313;_T184F=
_tag_fat(", expecting `",sizeof(char),14U);_T184E=_T184F;goto _TL314;_TL313: _T1850=
_tag_fat(" or `",sizeof(char),6U);_T184E=_T1850;_TL314:
# 451
 Cyc_strcat(_T184D,_T184E);_T1851=msg;_T1852=Cyc_yytname;_T1853=x;_T1854=_check_known_subscript_notnull(_T1852,363U,sizeof(struct _fat_ptr),_T1853);_T1855=(struct _fat_ptr*)_T1854;_T1856=*_T1855;
# 454
Cyc_strcat(_T1851,_T1856);_T1857=msg;_T1858=
_tag_fat("'",sizeof(char),2U);Cyc_strcat(_T1857,_T1858);
count=count + 1;goto _TL312;_TL311: _TL312:
# 448
 x=x + 1;goto _TL310;_TL30F: goto _TL30A;_TL309: _TL30A:
# 459
 Cyc_yyerror(msg,yystate,yychar);}goto _TL2FC;
# 463
_TL2FB: _T1859=_tag_fat("parse error",sizeof(char),12U);_T185A=yystate;_T185B=yychar;Cyc_yyerror(_T1859,_T185A,_T185B);_TL2FC: goto _TL2FA;_TL2F9: _TL2FA: goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus!=3)goto _TL315;
# 474
if(yychar!=0)goto _TL317;{int _T1877=1;_npop_handler(0);return _T1877;}_TL317:
# 483
 yychar=- 2;goto _TL316;_TL315: _TL316:
# 489
 yyerrstatus=3;goto yyerrhandle;
# 493
yyerrdefault:
# 503 "cycbison.simple"
 yyerrpop:
# 505
 if(yyssp_offset!=0)goto _TL319;{int _T1877=1;_npop_handler(0);return _T1877;}_TL319:
 yyvsp_offset=yyvsp_offset + -1;_T185C=yyss;
yyssp_offset=yyssp_offset + -1;_T185D=yyssp_offset;_T185E=_check_fat_subscript(_T185C,sizeof(short),_T185D);_T185F=(short*)_T185E;_T1860=*_T185F;yystate=(int)_T1860;
# 521 "cycbison.simple"
yyerrhandle: _T1861=Cyc_yypact;_T1862=yystate;_T1863=_check_known_subscript_notnull(_T1861,1262U,sizeof(short),_T1862);_T1864=(short*)_T1863;_T1865=*_T1864;
yyn=(int)_T1865;_T1866=yyn;_T1867=- 32768;
if(_T1866!=_T1867)goto _TL31B;goto yyerrdefault;_TL31B:
# 525
 yyn=yyn + 1;
if(yyn < 0)goto _TL31F;else{goto _TL321;}_TL321: if(yyn > 8241)goto _TL31F;else{goto _TL320;}_TL320: _T1868=Cyc_yycheck;_T1869=yyn;_T186A=_T1868[_T1869];_T186B=(int)_T186A;if(_T186B!=1)goto _TL31F;else{goto _TL31D;}_TL31F: goto yyerrdefault;_TL31D: _T186C=Cyc_yytable;_T186D=yyn;_T186E=_T186C[_T186D];
# 528
yyn=(int)_T186E;
if(yyn >= 0)goto _TL322;_T186F=yyn;_T1870=- 32768;
# 531
if(_T186F!=_T1870)goto _TL324;goto yyerrpop;_TL324:
 yyn=- yyn;goto yyreduce;
# 535
_TL322: if(yyn!=0)goto _TL326;goto yyerrpop;_TL326:
# 537
 if(yyn!=1261)goto _TL328;{int _T1877=0;_npop_handler(0);return _T1877;}_TL328: _T1871=yyvs;
# 546
yyvsp_offset=yyvsp_offset + 1;_T1872=yyvsp_offset;_T1873=_check_fat_subscript(_T1871,sizeof(struct Cyc_Yystacktype),_T1872);_T1874=(struct Cyc_Yystacktype*)_T1873;{struct Cyc_Yystacktype _T1877;_T1877.v=yylval;_T1877.l=yylloc;_T1875=_T1877;}*_T1874=_T1875;goto yynewstate;}}}}_pop_region();}
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
struct _fat_ptr Cyc_token2string(int token){struct _fat_ptr _T0;struct _fat_ptr _T1;struct _fat_ptr _T2;int _T3;short*_T4;int _T5;short _T6;int _T7;unsigned _T8;struct _fat_ptr*_T9;int _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;
if(token > 0)goto _TL32B;_T0=
_tag_fat("end-of-file",sizeof(char),12U);return _T0;_TL32B:
 if(token!=387)goto _TL32D;_T1=Cyc_Lex_token_string;
return _T1;_TL32D:
 if(token!=396)goto _TL32F;_T2=
Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);return _T2;_TL32F:
 if(token <= 0)goto _TL331;if(token > 400)goto _TL331;_T4=Cyc_yytranslate;_T5=token;_T6=_T4[_T5];_T3=(int)_T6;goto _TL332;_TL331: _T3=363;_TL332: {int z=_T3;_T7=z;_T8=(unsigned)_T7;
if(_T8 >= 363U)goto _TL333;_T9=Cyc_yytname;_TA=z;_TB=_T9[_TA];
return _TB;_TL333: _TC=
_tag_fat(0,0,0);return _TC;}}
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
